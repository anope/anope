// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
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
#include "modules/chanserv/akick.h"

#define AKICK_EXT "akicks"

class AKickService final
	: public ChanServ::AutoKickService
{
private:
	struct AKickList final
		: Serialize::Checker<std::vector<ChanServ::AutoKick *>>
	{
		AKickList(Extensible *)
			: Serialize::Checker<std::vector<ChanServ::AutoKick *>>(AKICK_EXT)
		{
		}
	};
	ExtensibleItem<AKickList> akickext;

public:
	AKickService(Module *m)
		: ChanServ::AutoKickService(m)
		, akickext(m, AKICK_EXT)
	{
	}

	ChanServ::AutoKick *AddAKick(ChannelInfo *ci, const Anope::string &user, NickCore *akicknc, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0) override
	{
		auto *akick = new ChanServ::AutoKick();
		akick->ci = ci;
		akick->nc = akicknc;
		akick->reason = reason;
		akick->creator = user;
		akick->addtime = t;
		akick->last_used = lu;

		auto *akicks = akickext.Require(ci);
		(*akicks)->push_back(akick);

		akicknc->AddChannelReference(ci);

		return akick;
	}

	ChanServ::AutoKick *AddAKick(ChannelInfo *ci, const Anope::string &user, const Anope::string &mask, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0)  override
	{
		auto *akick = new ChanServ::AutoKick();
		akick->ci = ci;
		akick->mask = mask;
		akick->nc = nullptr;
		akick->reason = reason;
		akick->creator = user;
		akick->addtime = t;
		akick->last_used = lu;

		auto *akicks = akickext.Require(ci);
		(*akicks)->push_back(akick);

		return akick;
	}

	ChanServ::AutoKick *GetAKick(ChannelInfo *ci, unsigned index) override
	{
		auto *akicks = akickext.Get(ci);
		if (!akicks || index >= (*akicks)->size())
			return nullptr;

		auto *akick = (*akicks)->at(index);
		akick->QueueUpdate();
		return akick;
	}

	unsigned GetAKickCount(ChannelInfo *ci) override
	{
		auto *akicks = akickext.Get(ci);
		return akicks ? (*akicks)->size() : 0;
	}

	void EraseAKick(ChannelInfo *ci, unsigned index) override
	{
		auto *akicks = akickext.Get(ci);
		if (akicks && index < (*akicks)->size())
			delete (*akicks)->at(index);
	}

	void EraseAKick(ChannelInfo *ci, ChanServ::AutoKick *akick) override
	{
		auto *akicks = akickext.Get(ci);
		if (!akicks)
			return;

		auto it = std::find((*akicks)->begin(), (*akicks)->end(), akick);
		if (it != (*akicks)->end())
			(*akicks)->erase(it);
	}

	void ClearAKick(ChannelInfo *ci) override
	{
		auto *akicks = akickext.Get(ci);
		if (!akicks)
			return; // No akick list.

		while (!(*akicks)->empty())
			delete (*akicks)->back();
	}
};

struct AutoKickType final
	: public Serialize::Type
{
	AutoKickType()
		: Serialize::Type(CHANSERV_AUTO_KICK_TYPE)
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *ak = static_cast<const ChanServ::AutoKick *>(obj);
		data.Store("ci", ak->ci->name);
		if (ak->nc)
			data.Store("ncid", ak->nc->GetId());
		else
			data.Store("mask", ak->mask);
		data.Store("reason", ak->reason);
		data.Store("creator", ak->creator);
		data.Store("addtime", ak->addtime);
		data.Store("last_used", ak->last_used);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		Anope::string sci, snc;
		uint64_t sncid = 0;

		data["ci"] >> sci;
		data["nc"] >> snc; // Deprecated 2.0 field
		data["ncid"] >> sncid;

		ChannelInfo *ci = ChannelInfo::Find(sci);
		if (!ci)
			return NULL;

		ChanServ::AutoKick *ak;
		auto *nc = sncid ? NickCore::FindId(sncid) : NickCore::Find(snc);
		if (obj)
		{
			ak = anope_dynamic_static_cast<ChanServ::AutoKick *>(obj);
			data["creator"] >> ak->creator;
			data["reason"] >> ak->reason;
			ak->nc = nc;
			data["mask"] >> ak->mask;
			data["addtime"] >> ak->addtime;
			data["last_used"] >> ak->last_used;
		}
		else
		{
			time_t addtime, lastused;
			data["addtime"] >> addtime;
			data["last_used"] >> lastused;

			Anope::string screator, sreason, smask;

			data["creator"] >> screator;
			data["reason"] >> sreason;
			data["mask"] >> smask;

			if (nc)
				ak = ChanServ::akick_service->AddAKick(ci, screator, nc, sreason, addtime, lastused);
			else
				ak = ChanServ::akick_service->AddAKick(ci, screator, smask, sreason, addtime, lastused);
		}

		return ak;
	}
};

class CommandCSAKick final
	: public Command
{
	void Enforce(CommandSource &source, ChannelInfo *ci)
	{
		Channel *c = ci->c;
		int count = 0;

		if (!c)
		{
			return;
		}

		for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
		{
			ChanUserContainer *uc = it->second;
			++it;

			if (c->CheckKick(uc->user))
				++count;
		}

		bool override = !source.AccessFor(ci).HasPriv("AKICK");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "ENFORCE, affects " << count << " users";

		source.Reply(_("AKICK ENFORCE for \002%s\002 complete; \002%d\002 users were affected."), ci->name.c_str(), count);
	}

	void DoAdd(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params[2];
		Anope::string reason = params.size() > 3 ? params[3] : "";
		const NickAlias *na = NickAlias::Find(mask);
		NickCore *nc = NULL;
		const ChanServ::AutoKick *akick;
		unsigned reasonmax = Config->GetModule("chanserv").Get<unsigned>("reasonmax", "200");

		if (reason.length() > reasonmax)
			reason = reason.substr(0, reasonmax);

		if (IRCD->IsExtbanValid(mask))
			; /* If this is an extban don't try to complete the mask */
		else if (IRCD->IsChannelValid(mask))
		{
			/* Also don't try to complete the mask if this is a channel */

			if (mask.equals_ci(ci->name) && ci->HasExt("PEACE"))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
		}
		else if (!na)
		{
			/* If the mask contains a realname the reason must be prepended with a : */
			if (mask.find('#') != Anope::string::npos)
			{
				size_t r = reason.find(':');
				if (r != Anope::string::npos)
				{
					mask += " " + reason.substr(0, r);
					mask.trim();
					reason = reason.substr(r + 1);
					reason.trim();
				}
				else
				{
					mask = mask + " " + reason;
					reason.clear();
				}
			}

			Entry e("", mask);

			mask = (e.nick.empty() ? "*" : e.nick) + "!"
				+ (e.user.empty() ? "*" : e.user) + "@"
				+ (e.host.empty() ? "*" : e.host);
			if (!e.real.empty())
				mask += "#" + e.real;
		}
		else
			nc = na->nc;

		/* Check excepts BEFORE we get this far */
		if (ci->c)
		{
			for (const auto &mode : ci->c->GetModeList("EXCEPT"))
			{
				if (Anope::Match(mode, mask))
				{
					source.Reply(CHAN_EXCEPTED, mask.c_str(), ci->name.c_str());
					return;
				}
			}
		}

		bool override = !source.AccessFor(ci).HasPriv("AKICK");
		/* Opers overriding get to bypass PEACE */
		if (override)
			;
		/* These peace checks are only for masks */
		else if (IRCD->IsChannelValid(mask))
			;
		/* Check whether target nick has equal/higher access
		* or whether the mask matches a user with higher/equal access - Viper */
		else if (ci->HasExt("PEACE") && nc)
		{
			AccessGroup nc_access = ci->AccessFor(nc), u_access = source.AccessFor(ci);
			if (nc == ci->GetFounder() || nc_access >= u_access)
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
		}
		else if (ci->HasExt("PEACE"))
		{
			/* Match against all currently online users with equal or
			 * higher access. - Viper */
			for (const auto &[_, u2] : UserListByNick)
			{
				AccessGroup nc_access = ci->AccessFor(nc), u_access = source.AccessFor(ci);
				Entry entry_mask("", mask);

				if ((ci->AccessFor(u2).HasPriv("FOUNDER") || nc_access >= u_access) && entry_mask.Matches(u2))
				{
					source.Reply(ACCESS_DENIED);
					return;
				}
			}

			/* Match against the last mask of all nickalias's with equal
			 * or higher access. - Viper */
			for (const auto &[_, na2] : *NickAliasList)
			{
				na = na2;

				AccessGroup nc_access = ci->AccessFor(na->nc), u_access = source.AccessFor(ci);
				if (na->nc && (na->nc == ci->GetFounder() || nc_access >= u_access))
				{
					Anope::string buf = na->nick + "!" + na->last_userhost;
					if (Anope::Match(buf, mask))
					{
						source.Reply(ACCESS_DENIED);
						return;
					}
				}
			 }
		}

		for (unsigned j = 0, end = ChanServ::akick_service->GetAKickCount(ci); j < end; ++j)
		{
			akick = ChanServ::akick_service->GetAKick(ci, j);
			if (akick->nc ? akick->nc == nc : mask.equals_ci(akick->mask))
			{
				source.Reply(_("\002%s\002 already exists on %s autokick list."), akick->nc ? akick->nc->display.c_str() : akick->mask.c_str(), ci->name.c_str());
				return;
			}
		}

		if (ChanServ::akick_service->GetAKickCount(ci) >= Config->GetModule(this->owner).Get<unsigned>("autokickmax"))
		{
			source.Reply(_("Sorry, you can only have %d autokick masks on a channel."), Config->GetModule(this->owner).Get<unsigned>("autokickmax"));
			return;
		}

		if (nc)
			akick = ChanServ::akick_service->AddAKick(ci, source.GetNick(), nc, reason);
		else
			akick = ChanServ::akick_service->AddAKick(ci, source.GetNick(), mask, reason);

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to add " << mask << (reason == "" ? "" : ": ") << reason;

		FOREACH_MOD(OnAKickAdd, (source, ci, akick));

		source.Reply(_("\002%s\002 added to %s autokick list."), mask.c_str(), ci->name.c_str());

		this->Enforce(source, ci);
	}

	void DoDel(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &mask = params[2];
		unsigned i, end;

		if (!ChanServ::akick_service->GetAKickCount(ci))
		{
			source.Reply(_("%s autokick list is empty."), ci->name.c_str());
			return;
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class AkickDelCallback final
				: public NumberList
			{
				CommandSource &source;
				ChannelInfo *ci;
				Command *c;
				unsigned deleted = 0;
				Anope::string lastdeleted;
				AccessGroup ag;
			public:
				AkickDelCallback(CommandSource &_source, ChannelInfo *_ci, Command *_c, const Anope::string &list) : NumberList(list, true), source(_source), ci(_ci), c(_c), ag(source.AccessFor(ci))
				{
				}

				~AkickDelCallback() override
				{
					switch (deleted)
					{
						case 0:
							source.Reply(_("No matching entries on %s autokick list."), ci->name.c_str());
							break;

						case 1:
							source.Reply(_("Deleted %s from %s autokick list."), lastdeleted.c_str(), ci->name.c_str());
							break;

						default:
							source.Reply(deleted, N_("Deleted %d entry from %s autokick list.", "Deleted %d entries from %s autokick list."), deleted, ci->name.c_str());
							break;
					}
				}

				void HandleNumber(unsigned number) override
				{
					if (!number || number > ChanServ::akick_service->GetAKickCount(ci))
						return;

					const auto *akick = ChanServ::akick_service->GetAKick(ci, number - 1);

					FOREACH_MOD(OnAKickDel, (source, ci, akick));

					bool override = !ag.HasPriv("AKICK");
					lastdeleted = (akick->nc ? akick->nc->display : akick->mask);
					Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, c, ci) << "to delete " << lastdeleted;

					++deleted;
					ChanServ::akick_service->EraseAKick(ci, number - 1);
				}
			}
			delcallback(source, ci, this, mask);
			delcallback.Process();
		}
		else
		{
			const NickAlias *na = NickAlias::Find(mask);
			const NickCore *nc = na ? *na->nc : NULL;

			for (i = 0, end = ChanServ::akick_service->GetAKickCount(ci); i < end; ++i)
			{
				const auto *akick = ChanServ::akick_service->GetAKick(ci, i);

				if (akick->nc ? akick->nc == nc : mask.equals_ci(akick->mask))
					break;
			}

			if (i == ChanServ::akick_service->GetAKickCount(ci))
			{
				source.Reply(_("\002%s\002 not found on %s autokick list."), mask.c_str(), ci->name.c_str());
				return;
			}

			bool override = !source.AccessFor(ci).HasPriv("AKICK");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << mask;

			FOREACH_MOD(OnAKickDel, (source, ci, ChanServ::akick_service->GetAKick(ci, i)));

			ChanServ::akick_service->EraseAKick(ci, i);

			source.Reply(_("\002%s\002 deleted from %s autokick list."), mask.c_str(), ci->name.c_str());
		}
	}

	void ProcessList(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params, ListFormatter &list)
	{
		const Anope::string &mask = params.size() > 2 ? params[2] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class AkickListCallback final
				: public NumberList
			{
				ListFormatter &list;
				ChannelInfo *ci;

			public:
				AkickListCallback(ListFormatter &_list, ChannelInfo *_ci, const Anope::string &numlist) : NumberList(numlist, false), list(_list), ci(_ci)
				{
				}

				void HandleNumber(unsigned number) override
				{
					if (!number || number > ChanServ::akick_service->GetAKickCount(ci))
						return;

					const auto *akick = ChanServ::akick_service->GetAKick(ci, number - 1);

					Anope::string timebuf, lastused;
					if (akick->addtime)
						timebuf = Anope::strftime(akick->addtime, NULL, true);
					else
						timebuf = TIME_UNKNOWN;
					if (akick->last_used)
						lastused = Anope::strftime(akick->last_used, NULL, true);
					else
						lastused = TIME_NEVER;

					ListFormatter::ListEntry entry;
					entry["Number"] = Anope::ToString(number);
					if (akick->nc)
						entry["Mask"] = akick->nc->display;
					else
						entry["Mask"] = akick->mask;
					entry["Creator"] = akick->creator;
					entry["Created"] = timebuf;
					entry["Last used"] = lastused;
					entry["Reason"] = akick->reason;
					this->list.AddEntry(entry);
				}
			}
			nl_list(list, ci, mask);
			nl_list.Process();
		}
		else
		{
			for (unsigned i = 0, end = ChanServ::akick_service->GetAKickCount(ci); i < end; ++i)
			{
				const auto *akick = ChanServ::akick_service->GetAKick(ci, i);

				if (!mask.empty())
				{
					if (!akick->nc && !Anope::Match(akick->mask, mask))
						continue;
					if (akick->nc && !Anope::Match(akick->nc->display, mask))
						continue;
				}

				Anope::string timebuf, lastused;
				if (akick->addtime)
					timebuf = Anope::strftime(akick->addtime, NULL, true);
				else
					timebuf = TIME_UNKNOWN;
				if (akick->last_used)
					lastused = Anope::strftime(akick->last_used, NULL, true);
				else
					lastused = TIME_NEVER;

				ListFormatter::ListEntry entry;
				entry["Number"] = Anope::ToString(i + 1);
				if (akick->nc)
					entry["Mask"] = akick->nc->display;
				else
					entry["Mask"] = akick->mask;
				entry["Creator"] = akick->creator;
				entry["Created"] = timebuf;
				entry["Last used"] = lastused;
				entry["Reason"] = akick->reason;
				list.AddEntry(entry);
			}
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on %s autokick list."), ci->name.c_str());
		else
		{
			source.Reply(_("Autokick list for %s:"), ci->name.c_str());
			list.SendTo(source);
			source.Reply(_("End of autokick list"));
		}
	}

	void DoList(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		if (!ChanServ::akick_service->GetAKickCount(ci))
		{
			source.Reply(_("%s autokick list is empty."), ci->name.c_str());
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Mask")).AddColumn(_("Reason"));
		list.SetFlexible([](ListFormatter::ListEntry &row)
		{
			return row["Reason"].empty()
				? _("{number}: \002{mask}\002")
				: _("{number}: \002{mask}\002 ({reason})");
		});

		this->ProcessList(source, ci, params, list);
	}

	void DoView(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		if (!ChanServ::akick_service->GetAKickCount(ci))
		{
			source.Reply(_("%s autokick list is empty."), ci->name.c_str());
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Mask")).AddColumn(_("Creator")).AddColumn(_("Created")).AddColumn(_("Last used")).AddColumn(_("Reason"));
		list.SetFlexible([](ListFormatter::ListEntry &row)
		{
			return row["Reason"].empty()
				? _("{number}: \002{mask}\002 -- added by {creator} on {created}; last used: {last_used}")
				: _("{number}: \002{mask}\002 -- added by {creator} on {created}; last used: {last_used} ({reason})");
		});

		this->ProcessList(source, ci, params, list);
	}

	void DoEnforce(CommandSource &source, ChannelInfo *ci)
	{
		Channel *c = ci->c;

		if (!c)
		{
			source.Reply(CHAN_X_NOT_IN_USE, ci->name.c_str());
			return;
		}

		this->Enforce(source, ci);
	}

	void DoClear(CommandSource &source, ChannelInfo *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("AKICK");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to clear the akick list";

		ChanServ::akick_service->ClearAKick(ci);
		source.Reply(_("Channel %s akick list has been cleared."), ci->name.c_str());
	}

public:
	CommandCSAKick(Module *creator) : Command(creator, "chanserv/akick", 2, 4)
	{
		this->SetDesc(_("Maintain the AutoKick list"));
		this->SetSyntax(_("\037channel\037 ADD {\037nick\037 | \037mask\037} [\037reason\037]"));
		this->SetSyntax(_("\037channel\037 DEL {\037nick\037 | \037mask\037 | \037entry-num\037 | \037list\037}"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | \037entry-num\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 VIEW [\037mask\037 | \037entry-num\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 ENFORCE"));
		this->SetSyntax(_("\037channel\037 CLEAR"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Anope::string chan = params[0];
		Anope::string cmd = params[1];
		Anope::string mask = params.size() > 2 ? params[2] : "";

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		bool is_list = cmd.equals_ci("LIST") || cmd.equals_ci("VIEW");

		bool has_access = false;
		if (source.AccessFor(ci).HasPriv("AKICK") || source.HasPriv("chanserv/access/modify"))
			has_access = true;
		else if (is_list && source.HasPriv("chanserv/access/list"))
			has_access = true;

		if (mask.empty() && (cmd.equals_ci("ADD") || cmd.equals_ci("DEL")))
			this->OnSyntaxError(source, cmd);
		else if (!has_access)
			source.Reply(ACCESS_DENIED);
		else if (!cmd.equals_ci("LIST") && !cmd.equals_ci("VIEW") && !cmd.equals_ci("ENFORCE") && Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);
		else if (cmd.equals_ci("ADD"))
			this->DoAdd(source, ci, params);
		else if (cmd.equals_ci("DEL"))
			this->DoDel(source, ci, params);
		else if (cmd.equals_ci("LIST"))
			this->DoList(source, ci, params);
		else if (cmd.equals_ci("VIEW"))
			this->DoView(source, ci, params);
		else if (cmd.equals_ci("ENFORCE"))
			this->DoEnforce(source, ci);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(source, ci);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		BotInfo *bi = Config->GetClient("NickServ");
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Maintains the \002AutoKick list\002 for a channel. If a user "
				"on the AutoKick list attempts to join the channel, "
				"%s will ban that user from the channel, then kick "
				"the user."
				"\n\n"
				"The \002%s\032ADD\002 command adds the given nick or mask "
				"to the AutoKick list. If a \037reason\037 is given with "
				"the command, that reason will be used when the user is "
				"kicked; if not, the default reason is \"User has been "
				"banned from the channel\". "
				"When akicking a \037registered nick\037 the %s account "
				"will be added to the akick list instead of the mask. "
				"All users within that nickgroup will then be akicked. "
				"\n\n"
				"The \002%s\032DEL\002 command removes the given nick or mask "
				"from the AutoKick list. It does not, however, remove any "
				"bans placed by an AutoKick; those must be removed "
				"manually."
				"\n\n"
				"The \002%s\032LIST\002 command displays the AutoKick list, or "
				"optionally only those AutoKick entries which match the "
				"given mask."
				"\n\n"
				"The \002%s\032VIEW\002 command is a more verbose version of the "
				"\002%s\032LIST\002 command."
				"\n\n"
				"The \002%s\032ENFORCE\002 command causes %s to enforce the "
				"current akick list by removing those users who match an "
				"akick mask."
				"\n\n"
				"The \002%s\032CLEAR\002 command clears all entries of the "
				"akick list."
			),
			source.service->nick.c_str(),
			source.command.nobreak().c_str(),
			bi ? bi->nick.c_str() : "NickServ",
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str());

		return true;
	}
};

class CSAKick final
	: public Module
{
private:
	AKickService akick_service;
	AutoKickType autokick_type;
	CommandCSAKick commandcsakick;

public:
	CSAKick(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, akick_service(this)
		, commandcsakick(this)
	{
	}

	void OnCreateChan(ChannelInfo *ci) override
	{
		OnDelChan(ci); // copy ctor might have been called
	}

	void OnDelChan(ChannelInfo *ci) override
	{
		akick_service.ClearAKick(ci);
	}

	void OnDelCore(NickCore *nc) override
	{
		std::deque<ChannelInfo *> chans;
		nc->GetChannelReferences(chans);

		for (auto *ci : chans)
		{
			for (unsigned j = 0; j < akick_service.GetAKickCount(ci); ++j)
			{
				const auto *akick = akick_service.GetAKick(ci, j);
				if (akick->nc == nc)
				{
					akick_service.EraseAKick(ci, j);
					break;
				}
			}
		}
	}

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) override
	{
		if (!c->ci || c->MatchesList(u, "EXCEPT"))
			return EVENT_CONTINUE;


		for (unsigned j = 0, end = ChanServ::akick_service->GetAKickCount(c->ci); j < end; ++j)
		{
			auto *autokick = ChanServ::akick_service->GetAKick(c->ci, j);
			bool kick = false;

			if (autokick->nc)
				kick = autokick->nc == u->Account();
			else if (IRCD->IsChannelValid(autokick->mask))
			{
				Channel *chan = Channel::Find(autokick->mask);
				kick = chan != NULL && chan->FindUser(u);
			}
			else
				kick = Entry("BAN", autokick->mask).Matches(u);

			if (kick)
			{
				Log(LOG_DEBUG_2) << u->nick << " matched akick " << (autokick->nc ? autokick->nc->display : autokick->mask);
				autokick->last_used = Anope::CurTime;
				if (!autokick->nc && autokick->mask.find('#') == Anope::string::npos)
					mask = autokick->mask;
				reason = autokick->reason;
				if (reason.empty())
				{
					reason = Language::Translate(u, Config->GetModule(this).Get<const Anope::string>("autokickreason").c_str());
					reason = Anope::Template(reason, {
						{ "channel", c->name },
						{ "nick",    u->nick },
					});
				}
				if (reason.empty())
					reason = Language::Translate(u, _("User has been banned from the channel"));
				return EVENT_STOP;
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(CSAKick)
