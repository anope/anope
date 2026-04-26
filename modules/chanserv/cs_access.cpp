// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

static std::map<Anope::string, int16_t, ci::less> defaultLevels;

static inline void reset_levels(ChannelInfo *ci)
{
	ci->ClearLevels();
	for (auto &[priv, level] : defaultLevels)
		ci->SetLevel(priv, level);
}

static Anope::string LevelToString(CommandSource &source, int16_t level)
{
	if (level == ACCESS_INVALID)
		return source.Translate(_("(disabled)"));

	if (level == ACCESS_FOUNDER)
		return source.Translate(_("(founder only)"));

	return Anope::ToString(level);
}

class AccessChanAccess final
	: public ChanAccess
{
public:
	int level = 0;

	AccessChanAccess(AccessProvider *p) : ChanAccess(p)
	{
	}

	bool HasPriv(const Anope::string &name) const override
	{
		return this->ci->GetLevel(name) != ACCESS_INVALID && this->level >= this->ci->GetLevel(name);
	}

	Anope::string AccessSerialize() const override
	{
		return Anope::ToString(this->level);
	}

	void AccessUnserialize(const Anope::string &data) override
	{
		if (auto l = Anope::TryConvert<int>(data))
			this->level = l.value();
	}

	bool operator>(const ChanAccess &other) const override
	{
		if (this->provider != other.provider)
			return ChanAccess::operator>(other);
		else
			return this->level > anope_dynamic_static_cast<const AccessChanAccess *>(&other)->level;
	}

	bool operator<(const ChanAccess &other) const override
	{
		if (this->provider != other.provider)
			return ChanAccess::operator<(other);
		else
			return this->level < anope_dynamic_static_cast<const AccessChanAccess *>(&other)->level;
	}
};

class AccessAccessProvider final
	: public AccessProvider
{
public:
	static AccessAccessProvider *me;

	AccessAccessProvider(Module *o) : AccessProvider(o, "access/access")
	{
		me = this;
	}

	ChanAccess *Create() override
	{
		return new AccessChanAccess(this);
	}
};
AccessAccessProvider *AccessAccessProvider::me;

class CommandCSAccess final
	: public Command
{
private:
	static void AddEntry(ListFormatter &list, const ChannelInfo *ci, const ChanAccess *access, unsigned number)
	{
		Anope::string timebuf;
		if (ci->c)
		{
			for (const auto &[_, cuc] : ci->c->users)
			{
				ChannelInfo *p;
				if (access->Matches(cuc->user, cuc->user->Account(), p))
					timebuf = TIME_NOW;
			}
		}
		if (timebuf.empty())
		{
			if (access->last_seen == 0)
				timebuf = TIME_NEVER;
			else
				timebuf = Anope::strftime(access->last_seen, NULL, true);
		}

		ListFormatter::ListEntry entry;
		entry["Number"] = Anope::ToString(number);
		entry["Level"] = access->AccessSerialize();
		entry["Mask"] = access->Mask();
		entry["Creator"] = access->creator;
		entry["Last seen"] = timebuf;
		entry["Description"] = access->description;
		list.AddEntry(entry);
	}


	void DoAdd(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params[2];
		Anope::string description = params.size() > 4 ? params[4] : "";
		Privilege *p = NULL;
		int level = ACCESS_INVALID;

		if (auto lvl = Anope::TryConvert<int>(params[3]))
			level = lvl.value();
		else
		{
			p = PrivilegeManager::FindPrivilege(params[3]);
			if (p != NULL && defaultLevels[p->name])
				level = defaultLevels[p->name];
		}

		if (!level)
		{
			source.Reply(_("Access level must be non-zero."));
			return;
		}
		else if (level <= ACCESS_INVALID || level >= ACCESS_FOUNDER)
		{
			source.Reply(CHAN_ACCESS_LEVEL_RANGE, ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
			return;
		}

		AccessGroup u_access = source.AccessFor(ci);
		const ChanAccess *highest = u_access.Highest();

		AccessChanAccess tmp_access(AccessAccessProvider::me);
		tmp_access.ci = ci;
		tmp_access.level = level;

		bool override = false;
		const NickAlias *na = NULL;

		if ((!highest || *highest <= tmp_access) && !u_access.founder)
		{
			if (source.HasPriv("chanserv/access/modify"))
				override = true;
			else
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
		}

		const auto &csconf = Config->GetModule("chanserv");
		if (IRCD->IsChannelValid(mask))
		{
			if (csconf.Get<bool>("disallow_channel_access"))
			{
				source.Reply(_("Channels may not be on access lists."));
				return;
			}

			ChannelInfo *targ_ci = ChannelInfo::Find(mask);
			if (targ_ci == NULL)
			{
				source.Reply(CHAN_X_NOT_REGISTERED, mask.c_str());
				return;
			}
			else if (ci == targ_ci)
			{
				source.Reply(_("You can't add a channel to its own access list."));
				return;
			}

			mask = targ_ci->name;
		}
		else
		{
			na = NickAlias::Find(mask);
			if (na)
			{
				if (na->nc->HasExt("NEVEROP"))
				{
					source.Reply(_("\002%s\002 does not wish to be added to channel access lists."),
						na->nc->display.c_str());
					return;
				}
				mask = na->nick;
			}
			else
			{
				if (csconf.Get<bool>("disallow_hostmask_access"))
				{
					source.Reply(_("Masks and unregistered users may not be on access lists."));
					return;
				}

				if (mask.find_first_of("!*@") == Anope::string::npos)
				{
					auto *targ = User::Find(mask, true);
					if (!targ)
					{
						source.Reply(NICK_X_NOT_IN_USE, mask.c_str());
						return;
					}

					auto *targnc = targ->Account();
					if (!targnc)
					{
						source.Reply(NICK_X_NOT_REGISTERED, targ->nick.c_str());
						return;
					}

					mask = targnc->display;
					if (description.empty())
						description = targ->nick;
				}
				else
				{
					// Normalize the entry mask.
					const auto cleanmask = Entry(mask).GetCleanMask();
					if (csconf.Get<bool>("disallow_malformed_hostmask") && cleanmask != mask)
					{
						source.Reply(CHAN_ACCESS_MALFORMED, cleanmask.c_str());
						return;
					}

					mask = cleanmask;
				}
			}
		}

		for (unsigned i = ci->GetAccessCount(); i > 0; --i)
		{
			const ChanAccess *access = ci->GetAccess(i - 1);
			if ((na && na->nc == access->GetAccount()) || mask.equals_ci(access->Mask()))
			{
				/* Don't allow lowering from a level >= u_level */
				if ((!highest || *access >= *highest) && !u_access.founder && !source.HasPriv("chanserv/access/modify"))
				{
					source.Reply(ACCESS_DENIED);
					return;
				}
				delete ci->EraseAccess(i - 1);
				break;
			}
		}

		const auto access_count = ci->GetDeepAccessCount();
		const auto access_max = Config->GetModule("chanserv").Get<unsigned>("accessmax", "1000");
		if (access_max && access_count >= access_max)
		{
			if (access_count == ci->GetAccessCount())
				source.Reply(access_max, CHAN_ACCESS_LIMIT, access_max);
			else
				source.Reply(access_max, CHAN_ACCESS_LIMIT_DEEP, access_max);
			return;
		}

		ServiceReference<AccessProvider> provider("AccessProvider", "access/access");
		if (!provider)
			return;
		auto *access = anope_dynamic_static_cast<AccessChanAccess *>(provider->Create());
		access->SetMask(mask, ci);
		access->creator = source.GetNick();
		access->level = level;
		access->last_seen = 0;
		access->created = Anope::CurTime;
		access->description = description;
		ci->AddAccess(access);

		FOREACH_MOD(OnAccessAdd, (ci, source, access, false));

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to add " << mask << " with level " << level;
		if (p != NULL)
			source.Reply(_("\002%s\002 added to %s access list at privilege %s (level %d)"), access->Mask().c_str(), ci->name.c_str(), p->name.c_str(), level);
		else
			source.Reply(_("\002%s\002 added to %s access list at level \002%d\002."), access->Mask().c_str(), ci->name.c_str(), level);
	}

	void DoDel(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params[2];

		const NickAlias *na = NickAlias::Find(mask);
		if (na && na->nc)
		{
			mask = na->nc->display;
		}
		else if (!isdigit(mask[0]) && mask.find_first_of("#!*@") == Anope::string::npos)
		{
			User *targ = User::Find(mask, true);
			if (targ != NULL)
				mask = "*!*@" + targ->GetDisplayedHost();
			else
			{
				source.Reply(NICK_X_NOT_REGISTERED, mask.c_str());
				return;
			}
		}

		if (!ci->GetAccessCount())
			source.Reply(_("%s access list is empty."), ci->name.c_str());
		else if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class AccessDelCallback final
				: public NumberList
			{
				CommandSource &source;
				ChannelInfo *ci;
				Command *c;
				unsigned deleted = 0;
				Anope::string nicks;
				bool denied = false;
				bool override = false;
			public:
				AccessDelCallback(CommandSource &_source, ChannelInfo *_ci, Command *_c, const Anope::string &numlist) : NumberList(numlist, true), source(_source), ci(_ci), c(_c)
				{
					if (!source.AccessFor(ci).HasPriv("ACCESS_CHANGE") && source.HasPriv("chanserv/access/modify"))
						this->override = true;
				}

				~AccessDelCallback() override
				{
					if (denied && !deleted)
						source.Reply(ACCESS_DENIED);
					else if (!deleted)
						source.Reply(_("No matching entries on %s access list."), ci->name.c_str());
					else
					{
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, c, ci) << "to delete " << nicks;
						if (deleted == 1)
							source.Reply(_("Deleted %s from %s access list."), nicks.c_str(), ci->name.c_str());

						else
							source.Reply(deleted, N_("Deleted %d entry from %s access list.", "Deleted %d entries from %s access list."), deleted, ci->name.c_str());
					}
				}

				void HandleNumber(unsigned Number) override
				{
					if (!Number || Number > ci->GetAccessCount())
						return;

					ChanAccess *access = ci->GetAccess(Number - 1);

					AccessGroup ag = source.AccessFor(ci);
					const ChanAccess *u_highest = ag.Highest();

					if ((!u_highest || *u_highest <= *access) && !ag.founder && !this->override && access->GetAccount() != source.nc)
					{
						denied = true;
						return;
					}

					++deleted;
					if (!nicks.empty())
						nicks += ", ";
					nicks += access->Mask();

					ci->EraseAccess(Number - 1);

					FOREACH_MOD(OnAccessDel, (ci, source, access, false));
					delete access;
				}
			}
			delcallback(source, ci, this, mask);
			delcallback.Process();
		}
		else
		{
			AccessGroup u_access = source.AccessFor(ci);
			const ChanAccess *highest = u_access.Highest();

			for (unsigned i = ci->GetAccessCount(); i > 0; --i)
			{
				ChanAccess *access = ci->GetAccess(i - 1);
				if (mask.equals_ci(access->Mask()))
				{
					if (access->GetAccount() != source.nc && !u_access.founder && (!highest || *highest <= *access) && !source.HasPriv("chanserv/access/modify"))
						source.Reply(ACCESS_DENIED);
					else
					{
						source.Reply(_("\002%s\002 deleted from %s access list."), access->Mask().c_str(), ci->name.c_str());
						bool override = !u_access.founder && !u_access.HasPriv("ACCESS_CHANGE") && access->GetAccount() != source.nc;
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << access->Mask();

						ci->EraseAccess(i - 1);
						FOREACH_MOD(OnAccessDel, (ci, source, access, false));
						delete access;
					}
					return;
				}
			}

			source.Reply(_("\002%s\002 not found on %s access list."), mask.c_str(), ci->name.c_str());
		}
	}

	void ProcessList(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params, ListFormatter &list)
	{
		const Anope::string &nick = params.size() > 2 ? params[2] : "";
		const auto show_all = params.size() > 3 && params[3].equals_ci("ALL");
		unsigned foreign = 0;

		if (!ci->GetAccessCount())
			source.Reply(_("%s access list is empty."), ci->name.c_str());
		else if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class AccessListCallback final
				: public NumberList
			{
				ListFormatter &list;
				ChannelInfo *ci;
				bool show_all;
				unsigned &foreign;

			public:
				AccessListCallback(ListFormatter &_list, ChannelInfo *_ci, const Anope::string &numlist, bool _show_all, unsigned &_foreign)
					: NumberList(numlist, false)
					, list(_list)
					, ci(_ci)
					, show_all(_show_all)
					, foreign(_foreign)
				{
				}

				void HandleNumber(unsigned number) override
				{
					if (!number || number > ci->GetAccessCount())
						return;

					const ChanAccess *access = ci->GetAccess(number - 1);
					if (!show_all && access->provider->name != "access/access")
					{
						foreign++;
						return;
					}

					AddEntry(this->list, ci, access, number);
				}
			}
			nl_list(list, ci, nick, show_all, foreign);
			nl_list.Process();
		}
		else
		{
			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				const ChanAccess *access = ci->GetAccess(i);

				if (!nick.empty() && !Anope::Match(access->Mask(), nick))
					continue;

				if (!show_all && access->provider->name != "access/access")
				{
					foreign++;
					continue;
				}

				AddEntry(list, ci, access, i + 1);
			}
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on %s access list."), ci->name.c_str());
		else
		{
			source.Reply(_("Access list for %s:"), ci->name.c_str());
			list.SendTo(source);
			source.Reply(_("End of access list"));
		}

		if (foreign)
		{
			const auto full_command = Anope::Format("%s %s %s %s", source.command.c_str(),
				ci->name.c_str(), params[1].upper().c_str(), nick.empty() ? "*" : nick.c_str()).nobreak();

			source.Reply(foreign, CHAN_ACCESS_FOREIGN, foreign, full_command.c_str());
		}
	}

	void DoList(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s access list is empty."), ci->name.c_str());
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Level")).AddColumn(_("Mask")).AddColumn(_("Description"));
		list.SetFlexible([](ListFormatter::ListEntry &row)
		{
			return row["Description"].empty()
				? _("{number}: \002{mask}\002 = {level}")
				: _("{number}: \002{mask}\002 = {level} ({description})");
		});

		this->ProcessList(source, ci, params, list);
	}

	void DoView(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s access list is empty."), ci->name.c_str());
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Level")).AddColumn(_("Mask")).AddColumn(_("Creator")).AddColumn(_("Last seen")).AddColumn(_("Description"));
		list.SetFlexible([](ListFormatter::ListEntry &row)
		{
			return row["Description"].empty()
				? _("{number}: \002{mask}\002 = {level} -- created by {creator}; last seen {last_seen}")
				: _("{number}: \002{mask}\002 = {level} -- created by {creator}; last seen {last_seen} ({description})");
		});

		this->ProcessList(source, ci, params, list);
	}

	void DoClear(CommandSource &source, ChannelInfo *ci)
	{
		if (!source.IsFounder(ci) && !source.HasPriv("chanserv/access/modify"))
			source.Reply(ACCESS_DENIED);
		else
		{
			FOREACH_MOD(OnAccessClear, (ci, source));

			ci->ClearAccess();

			source.Reply(_("Channel %s access list has been cleared."), ci->name.c_str());

			bool override = !source.IsFounder(ci);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to clear the access list";
		}
	}

public:
	CommandCSAccess(Module *creator) : Command(creator, "chanserv/access", 2, 5)
	{
		this->SetDesc(_("Modify the list of privileged users"));
		this->SetSyntax(_("\037channel\037 ADD \037mask\037 \037level\037 [\037description\037]"));
		this->SetSyntax(_("\037channel\037 DEL {\037mask\037 | \037entry-num\037 | \037list\037}"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | \037list\037] [ALL]"));
		this->SetSyntax(_("\037channel\037 VIEW [\037mask\037 | \037list\037] [ALL]"));
		this->SetSyntax(_("\037channel\037 CLEAR"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[1];
		const Anope::string &nick = params.size() > 2 ? params[2] : "";
		const Anope::string &s = params.size() > 3 ? params[3] : "";

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		bool is_list = cmd.equals_ci("LIST") || cmd.equals_ci("VIEW");
		bool is_clear = cmd.equals_ci("CLEAR");
		bool is_del = cmd.equals_ci("DEL");

		bool has_access = false;
		if (source.HasPriv("chanserv/access/modify"))
			has_access = true;
		else if (is_list && source.HasPriv("chanserv/access/list"))
			has_access = true;
		else if (is_list && source.AccessFor(ci).HasPriv("ACCESS_LIST"))
			has_access = true;
		else if (source.AccessFor(ci).HasPriv("ACCESS_CHANGE"))
			has_access = true;
		else if (is_del)
		{
			const NickAlias *na = NickAlias::Find(nick);
			if (na && na->nc == source.GetAccount())
				has_access = true;
		}

		/* If LIST, we don't *require* any parameters, but we can take any.
		 * If DEL, we require a nick and no level.
		 * Else (ADD), we require a level (which implies a nick). */
		if (is_list || is_clear ? 0 : (cmd.equals_ci("DEL") ? (nick.empty() || !s.empty()) : s.empty()))
			this->OnSyntaxError(source, cmd);
		else if (!has_access)
			source.Reply(ACCESS_DENIED);
		else if (Anope::ReadOnly && !is_list)
			source.Reply(READ_ONLY_MODE);
		else if (cmd.equals_ci("ADD"))
			this->DoAdd(source, ci, params);
		else if (cmd.equals_ci("DEL"))
			this->DoDel(source, ci, params);
		else if (cmd.equals_ci("LIST"))
			this->DoList(source, ci, params);
		else if (cmd.equals_ci("VIEW"))
			this->DoView(source, ci, params);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(source, ci);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Maintains the \002access list\002 for a channel. The access "
				"list specifies which users are allowed chanop status or "
				"access to %s commands on the channel. Different "
				"user levels allow for access to different subsets of "
				"privileges. Any registered user not on the access list has "
				"a user level of 0, and any unregistered user has a user level "
				"of -1."
				"\n\n"
				"The \002%s\033ADD\002 command adds the given mask to the "
				"access list with the given user level; if the mask is "
				"already present on the list, its access level is changed to "
				"the level specified in the command. The \037level\037 specified "
				"may be a numerical level or the name of a privilege (eg AUTOOP). "
				"When a user joins the channel the access they receive is from the "
				"highest level entry in the access list."
			),
			source.service->nick.c_str(),
			source.command.nobreak().c_str());

		if (!Config->GetModule("chanserv").Get<bool>("disallow_channel_access"))
		{
			source.Reply(_(
				"The given mask may also be a channel, which will use the "
				"access list from the other channel up to the given \037level\037."
			));
		}

		source.Reply(" ");
		source.Reply(_(
				"The \002%s\033DEL\002 command removes the given nick from the "
				"access list. If a list of entry numbers is given, those "
				"entries are deleted.  (See the example for LIST below.) "
				"You may remove yourself from an access list, even if you "
				"do not have access to modify that list otherwise."
				"\n\n"
				"The \002%s\033LIST\002 command displays the access list. If "
				"a wildcard mask is given, only those entries matching the "
				"mask are displayed. If a list of entry numbers is given, "
				"only those entries are shown. The \002ALL\002 option allows "
				"listing entries from other access systems as well as levels."
				"\n\n"
				"The \002%s\033VIEW\002 command displays the access list similar "
				"to \002%s\033LIST\002 but shows the creator and last used time."
				"\n\n"
				"The \002%s\033CLEAR\002 command clears all entries of the "
				"access list."
			),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str());

		ExampleWrapper examples;
		examples.AddEntry("#channel LIST 2-5,7-9", _(
			"Lists access entries on \037#channel\037 numbered 2 through 5 and 7 through 9."
		));
		examples.AddEntry("#channel LIST *nick*", _(
			"Lists access entries on \037#channel\037 that match \037*nick*\037."
		));
		examples.SendTo(source);

		BotInfo *bi;
		Anope::string cmd;
		if (Command::FindFromService("chanserv/levels", bi, cmd))
		{
			source.Reply(" ");
			source.Reply(_(
					"\002User access levels\002 can be seen by using the "
					"\002%s\002 command; type \002%s\033LEVELS\002 for "
					"information."
				),
				cmd.c_str(),
				bi->GetQueryCommand("generic/help").c_str());
		}
		return true;
	}
};

class CommandCSLevels final
	: public Command
{
	void DoSet(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &what = params[2];
		const Anope::string &lev = params[3];

		int level;

		if (lev.equals_ci("FOUNDER"))
			level = ACCESS_FOUNDER;
		else
		{
			if (auto lvl = Anope::TryConvert<int>(lev))
				level = lvl.value();
			else
			{
				this->OnSyntaxError(source, "SET");
				return;
			}
		}

		if (level <= ACCESS_INVALID || level > ACCESS_FOUNDER)
			source.Reply(_("Level must be between %d and %d inclusive."), ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
		else
		{
			Privilege *p = PrivilegeManager::FindPrivilege(what);
			if (p == NULL)
			{
				source.Reply(_("Setting \002%s\002 not known. Type \002%s\033LEVELS\002 for a list of valid settings."),
					what.c_str(), source.service->GetQueryCommand("generic/help").c_str());
			}
			else
			{
				bool override = !source.AccessFor(ci).HasPriv("FOUNDER");
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to set " << p->name << " to level " << level;

				ci->SetLevel(p->name, level);
				FOREACH_MOD(OnLevelChange, (source, ci, p->name, level));

				if (level == ACCESS_FOUNDER)
					source.Reply(_("Level for %s on channel %s changed to founder only."), p->name.c_str(), ci->name.c_str());
				else
					source.Reply(_("Level for \002%s\002 on channel %s changed to \002%d\002."), p->name.c_str(), ci->name.c_str(), level);
			}
		}
	}

	void DoDisable(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &what = params[2];

		/* Don't allow disabling of the founder level. It would be hard to change it back if you don't have access to use this command */
		if (what.equals_ci("FOUNDER"))
		{
			source.Reply(_("You can not disable the founder privilege because it would be impossible to re-enable it at a later time."));
			return;
		}

		Privilege *p = PrivilegeManager::FindPrivilege(what);
		if (p != NULL)
		{
			bool override = !source.AccessFor(ci).HasPriv("FOUNDER");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable " << p->name;

			ci->SetLevel(p->name, ACCESS_INVALID);
			FOREACH_MOD(OnLevelChange, (source, ci, p->name, ACCESS_INVALID));

			source.Reply(_("\002%s\002 disabled on channel %s."), p->name.c_str(), ci->name.c_str());
			return;
		}

		source.Reply(_("Setting \002%s\002 not known. Type \002%s\033LEVELS\002 for a list of valid settings."),
			what.c_str(), source.service->GetQueryCommand("generic/help").c_str());
	}

	static void DoList(CommandSource &source, ChannelInfo *ci)
	{
		source.Reply(_("Access level settings for channel %s:"), ci->name.c_str());

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Name")).AddColumn(_("Level"));
		list.SetFlexible(_("\002{name}\002 = {level}"));

		const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();

		for (const auto &p : privs)
		{
			int16_t j = ci->GetLevel(p.name);

			ListFormatter::ListEntry entry;
			entry["Name"] = p.name;
			entry["Level"] = LevelToString(source, j);
			list.AddEntry(entry);
		}

		list.SendTo(source);
	}

	void DoReset(CommandSource &source, ChannelInfo *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("FOUNDER");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to reset all levels";

		reset_levels(ci);
		FOREACH_MOD(OnLevelChange, (source, ci, "ALL", 0));

		source.Reply(_("Access levels for \002%s\002 reset to defaults."), ci->name.c_str());
	}

public:
	CommandCSLevels(Module *creator) : Command(creator, "chanserv/levels", 2, 4)
	{
		this->SetDesc(_("Redefine the meanings of access levels"));
		this->SetSyntax(_("\037channel\037 SET \037type\037 \037level\037"));
		this->SetSyntax(_("\037channel\037 {DIS | DISABLE} \037type\037"));
		this->SetSyntax(_("\037channel\037 LIST"));
		this->SetSyntax(_("\037channel\037 RESET"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[1];
		const Anope::string &what = params.size() > 2 ? params[2] : "";
		const Anope::string &s = params.size() > 3 ? params[3] : "";

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		bool has_access = false;
		if (source.HasPriv("chanserv/access/modify"))
			has_access = true;
		else if (cmd.equals_ci("LIST") && source.HasPriv("chanserv/access/list"))
			has_access = true;
		else if (source.AccessFor(ci).HasPriv("FOUNDER"))
			has_access = true;

		/* If SET, we want two extra parameters; if DIS[ABLE] or FOUNDER, we want only
		 * one; else, we want none.
		 */
		if (cmd.equals_ci("SET") ? s.empty() : (cmd.substr(0, 3).equals_ci("DIS") ? (what.empty() || !s.empty()) : !what.empty()))
			this->OnSyntaxError(source, cmd);
		else if (!has_access)
			source.Reply(ACCESS_DENIED);
		else if (Anope::ReadOnly && !cmd.equals_ci("LIST"))
			source.Reply(READ_ONLY_MODE);
		else if (cmd.equals_ci("SET"))
			this->DoSet(source, ci, params);
		else if (cmd.equals_ci("DIS") || cmd.equals_ci("DISABLE"))
			this->DoDisable(source, ci, params);
		else if (cmd.equals_ci("LIST"))
			this->DoList(source, ci);
		else if (cmd.equals_ci("RESET"))
			this->DoReset(source, ci);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.equals_ci("DESC"))
		{
			source.Reply(_("The following feature/function names are available:"));

			ListFormatter list(source.GetAccount());
			list.AddColumn(_("Name")).AddColumn(_("Default")).AddColumn(_("Description"));
			list.SetFlexible(_("\002{name}\002: defaults to {default} ({description})"));

			for (const auto &p : PrivilegeManager::GetPrivileges())
			{
				ListFormatter::ListEntry entry;
				entry["Name"] = p.name;
				entry["Default"] = LevelToString(source, defaultLevels[p.name]);
				entry["Description"] = source.Translate(p.desc.c_str());
				list.AddEntry(entry);
			}

			list.SendTo(source);
		}
		else
		{
			this->SendSyntax(source);
			source.Reply(" ");
			source.Reply(_(
					"The \002%s\002 command allows fine control over the meaning of "
					"the numeric access levels used for channels. With this "
					"command, you can define the access level required for most "
					"of %s's functions. (The \002SET\033FOUNDER\002 and this command "
					"are always restricted to the channel founder)."
					"\n\n"
					"\002%s\033SET\002 allows the access level for a function or group of "
					"functions to be changed. \002%s\033DISABLE\002 (or \002DIS\002 for short) "
					"disables an automatic feature or disallows access to a "
					"function by anyone, INCLUDING the founder (although, the founder "
					"can always re-enable it). Use \002%s\033SET founder\002 to make a level "
					"founder only."
					"\n\n"
					"\002%s\033LIST\002 shows the current levels for each function or "
					"group of functions. \002%s\033RESET\002 resets the levels to the "
					"default levels of a newly-created channel."
					"\n\n"
					"For a list of the features and functions whose levels can be "
					"set, see \002HELP\033%s\033DESC\002."
				),
				source.service->nick.c_str(),
				source.command.nobreak().c_str(),
				source.command.nobreak().c_str(),
				source.command.nobreak().c_str(),
				source.command.nobreak().c_str(),
				source.command.nobreak().c_str(),
				source.command.nobreak().c_str(),
				source.command.nobreak().c_str());
		}
		return true;
	}
};

class CSAccess final
	: public Module
{
	AccessAccessProvider accessprovider;
	CommandCSAccess commandcsaccess;
	CommandCSLevels commandcslevels;

public:
	CSAccess(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		accessprovider(this), commandcsaccess(this), commandcslevels(this)
	{
		this->SetPermanent(true);

	}

	void OnReload(Configuration::Conf &conf) override
	{
		defaultLevels.clear();

		for (int i = 0; i < conf.CountBlock("privilege"); ++i)
		{
			const auto &priv = conf.GetBlock("privilege", i);

			const Anope::string &pname = priv.Get<const Anope::string>("name");

			Privilege *p = PrivilegeManager::FindPrivilege(pname);
			if (p == NULL)
				continue;

			const Anope::string &value = priv.Get<const Anope::string>("level");
			if (value.empty())
				continue;
			else if (value.equals_ci("founder"))
				defaultLevels[p->name] = ACCESS_FOUNDER;
			else if (value.equals_ci("disabled"))
				defaultLevels[p->name] = ACCESS_INVALID;
			else
				defaultLevels[p->name] = priv.Get<int16_t>("level");
		}
	}

	void OnCreateChan(ChannelInfo *ci) override
	{
		reset_levels(ci);
	}

	EventReturn OnGroupCheckPriv(const AccessGroup *group, const Anope::string &priv) override
	{
		if (group->ci == NULL)
			return EVENT_CONTINUE;

		const ChanAccess *highest = group->Highest();
		if (highest && highest->provider == &accessprovider)
		{
			/* Access accessprovider is the only accessprovider with the concept of negative access,
			 * so check they don't have negative access
			 */
			const auto *aca = anope_dynamic_static_cast<const AccessChanAccess *>(highest);

			if (aca->level < 0)
				return EVENT_CONTINUE;
		}

		/* Special case. Allows a level of -1 to match anyone, and a level of 0 to match anyone identified. */
		int16_t level = group->ci->GetLevel(priv);
		if (level == -1)
			return EVENT_ALLOW;
		else if (level == 0 && group->nc && !group->nc->HasExt("UNCONFIRMED"))
			return EVENT_ALLOW;
		return EVENT_CONTINUE;
	}
};

MODULE_INIT(CSAccess)
