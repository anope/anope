/* ChanServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/chanserv.h"
#include "modules/cs_access.h"

static std::map<Anope::string, int16_t, ci::less> defaultLevels;

static inline void reset_levels(ChanServ::Channel *ci)
{
	ci->ClearLevels();
	for (std::map<Anope::string, int16_t, ci::less>::iterator it = defaultLevels.begin(), it_end = defaultLevels.end(); it != it_end; ++it)
		ci->SetLevel(it->first, it->second);
}

class AccessChanAccess : public ChanServ::ChanAccess
{
 public:
	int level;

	AccessChanAccess(ChanServ::AccessProvider *p) : ChanAccess(p), level(0)
	{
	}

	bool HasPriv(const Anope::string &name) const override
	{
		return this->ci->GetLevel(name) != ChanServ::ACCESS_INVALID && this->level >= this->ci->GetLevel(name);
	}

	Anope::string AccessSerialize() const
	{
		return stringify(this->level);
	}

	void AccessUnserialize(const Anope::string &data) override
	{
		try
		{
			this->level = convertTo<int>(data);
		}
		catch (const ConvertException &)
		{
		}
	}

	bool operator>(const ChanServ::ChanAccess &other) const override
	{
		if (this->provider != other.provider)
			return ChanServ::ChanAccess::operator>(other);
		else
			return this->level > anope_dynamic_static_cast<const AccessChanAccess *>(&other)->level;
	}

	bool operator<(const ChanServ::ChanAccess &other) const override
	{
		if (this->provider != other.provider)
			return ChanAccess::operator<(other);
		else
			return this->level < anope_dynamic_static_cast<const AccessChanAccess *>(&other)->level;
	}
};

class AccessAccessProvider : public ChanServ::AccessProvider
{
 public:
	static AccessAccessProvider *me;

	AccessAccessProvider(Module *o) : ChanServ::AccessProvider(o, "access/access")
	{
		me = this;
	}

	ChanServ::ChanAccess *Create() override
	{
		return new AccessChanAccess(this);
	}
};
AccessAccessProvider* AccessAccessProvider::me;

class CommandCSAccess : public Command
{
	void DoAdd(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params[2];
		ChanServ::Privilege *p = NULL;
		int level = ChanServ::ACCESS_INVALID;

		try
		{
			level = convertTo<int>(params[3]);
		}
		catch (const ConvertException &)
		{
			p = ChanServ::service ? ChanServ::service->FindPrivilege(params[3]) : nullptr;
			if (p != NULL && defaultLevels[p->name])
				level = defaultLevels[p->name];
		}

		if (!level)
		{
			source.Reply(_("Access level must be non-zero."));
			return;
		}

		if (level <= ChanServ::ACCESS_INVALID || level >= ChanServ::ACCESS_FOUNDER)
		{
			source.Reply(_("Access level must be between \002{0}\002 and \002{1}\002 inclusive."), ChanServ::ACCESS_INVALID + 1, ChanServ::ACCESS_FOUNDER - 1);
			return;
		}

		ChanServ::AccessGroup u_access = source.AccessFor(ci);
		const ChanServ::ChanAccess *highest = u_access.Highest();

		AccessChanAccess tmp_access(AccessAccessProvider::me);
		tmp_access.ci = ci;
		tmp_access.level = level;

		bool override = false;

		if ((!highest || *highest <= tmp_access) && !u_access.founder)
		{
			if (source.HasPriv("chanserv/access/modify"))
				override = true;
			else
			{
				source.Reply(_("Access denied. You do not have enough privileges on \002{0}\002 to add someone at level \002{1}\002."), ci->name, level);
				return;
			}
		}

		if (IRCD->IsChannelValid(mask))
		{
			if (Config->GetModule("chanserv")->Get<bool>("disallow_channel_access"))
			{
				source.Reply(_("Channels may not be on access lists."));
				return;
			}

			ChanServ::Channel *targ_ci = ChanServ::Find(mask);
			if (targ_ci == NULL)
			{
				source.Reply(_("Channel \002{0}\002 isn't registered."), mask);
				return;
			}

			if (ci == targ_ci)
			{
				source.Reply(_("You can't add a channel to its own access list."));
				return;
			}

			mask = targ_ci->name;
		}
		else
		{
			const NickServ::Nick *na = NickServ::FindNick(mask);
			if (!na && Config->GetModule("chanserv")->Get<bool>("disallow_hostmask_access"))
			{
				source.Reply(_("Masks and unregistered users may not be on access lists."));
				return;
			}

			if (mask.find_first_of("!*@") == Anope::string::npos && !na)
			{
				User *targ = User::Find(mask, true);
				if (targ != NULL)
					mask = "*!*@" + targ->GetDisplayedHost();
				else
				{
					source.Reply(_("\002{0}\002 isn't registered."), mask);
					return;
				}
			}
		}

		for (unsigned i = ci->GetAccessCount(); i > 0; --i)
		{
			const ChanServ::ChanAccess *access = ci->GetAccess(i - 1);
			if (mask.equals_ci(access->mask))
			{
				/* Don't allow lowering from a level >= u_level */
				if ((!highest || *access >= *highest) && !u_access.founder && !source.HasPriv("chanserv/access/modify"))
				{
					source.Reply(_("Access denied. You do not have enough privileges on \002{0}\002 to lower the access of \002{1}\002."), ci->name, access->mask);
					return;
				}
				delete ci->EraseAccess(i - 1);
				break;
			}
		}

		unsigned access_max = Config->GetModule("chanserv")->Get<unsigned>("accessmax", "1024");
		if (access_max && ci->GetDeepAccessCount() >= access_max)
		{
			source.Reply(_("Sorry, you can only have %d access entries on a channel, including access entries from other channels."), access_max);
			return;
		}

		ServiceReference<ChanServ::AccessProvider> provider("AccessProvider", "access/access");
		if (!provider)
			return;
		AccessChanAccess *access = anope_dynamic_static_cast<AccessChanAccess *>(provider->Create());
		access->ci = ci;
		access->mask = mask;
		access->creator = source.GetNick();
		access->level = level;
		access->last_seen = 0;
		access->created = Anope::CurTime;
		ci->AddAccess(access);

		Event::OnAccessAdd(&Event::AccessAdd::OnAccessAdd, ci, source, access);

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to add " << mask << " with level " << level;
		if (p != NULL)
			source.Reply(_("\002{0}\002 added to the access list of \002{1}\002 with privilege \002{2}\002 (level \002{3}\002)."), access->mask, ci->name, p->name, level);
		else
			source.Reply(_("\002{0}\002 added to the access list of \002{1}\002 at level \002{2}\002."), access->mask, ci->name, level);
	}

	void DoDel(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params[2];

		if (!ci->GetAccessCount())
		{
			source.Reply(_("The access list for \002{0}\002 is empty."), ci->name);
			return;
		}

		if (!isdigit(mask[0]) && mask.find_first_of("#!*@") == Anope::string::npos && !NickServ::FindNick(mask))
		{
			User *targ = User::Find(mask, true);
			if (targ != NULL)
				mask = "*!*@" + targ->GetDisplayedHost();
			else
			{
				source.Reply(_("\002{0}\002 isn't registered."), mask);
				return;
			}
		}

		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class AccessDelCallback : public NumberList
			{
				CommandSource &source;
				ChanServ::Channel *ci;
				Command *c;
				unsigned deleted;
				Anope::string Nicks;
				bool denied;
				bool override;
				Anope::string mask;
			 public:
				AccessDelCallback(CommandSource &_source, ChanServ::Channel *_ci, Command *_c, const Anope::string &numlist) : NumberList(numlist, true), source(_source), ci(_ci), c(_c), deleted(0), denied(false), override(false), mask(numlist)
				{
					if (!source.AccessFor(ci).HasPriv("ACCESS_CHANGE") && source.HasPriv("chanserv/access/modify"))
						this->override = true;
				}

				~AccessDelCallback()
				{
					if (denied && !deleted)
						source.Reply(_("Access denied. You do not have enough privileges on \002{0}\002 to remove any access entries matching \002{1}\002."));
					else if (!deleted)
						source.Reply(_("There are no entries matching \002{0}\002 on the access list of \002{1}\002."), mask, ci->name);
					else
					{
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, c, ci) << "to delete " << Nicks;

						if (deleted == 1)
							source.Reply(_("Deleted \0021\002 entry from the access list of \002{0}\002."), ci->name);
						else
							source.Reply(_("Deleted \002{0}\002 entries from the access list of \002{1}\002."), deleted, ci->name);
					}
				}

				void HandleNumber(unsigned Number) override
				{
					if (!Number || Number > ci->GetAccessCount())
						return;

					ChanServ::ChanAccess *access = ci->GetAccess(Number - 1);

					ChanServ::AccessGroup ag = source.AccessFor(ci);
					const ChanServ::ChanAccess *u_highest = ag.Highest();

					if ((!u_highest || *u_highest <= *access) && !ag.founder && !this->override && !access->mask.equals_ci(source.nc->display))
					{
						denied = true;
						return;
					}

					++deleted;
					if (!Nicks.empty())
						Nicks += ", " + access->mask;
					else
						Nicks = access->mask;

					ci->EraseAccess(Number - 1);

					Event::OnAccessDel(&Event::AccessDel::OnAccessDel, ci, source, access);
					delete access;
				}
			}
			delcallback(source, ci, this, mask);
			delcallback.Process();
		}
		else
		{
			ChanServ::AccessGroup u_access = source.AccessFor(ci);
			const ChanServ::ChanAccess *highest = u_access.Highest();

			for (unsigned i = ci->GetAccessCount(); i > 0; --i)
			{
				ChanServ::ChanAccess *access = ci->GetAccess(i - 1);
				if (mask.equals_ci(access->mask))
				{
					if (!access->mask.equals_ci(source.nc->display) && !u_access.founder && (!highest || *highest <= *access) && !source.HasPriv("chanserv/access/modify"))
						source.Reply(_("Access denied. You do not have enough privileges on \002{0}\002 to remove the access of \002{1}\002."), ci->name, access->mask);
					else
					{
						source.Reply(_("\002{0}\002 deleted from the access list of \002{1}\002."), access->mask, ci->name);
						bool override = !u_access.founder && !u_access.HasPriv("ACCESS_CHANGE") && !access->mask.equals_ci(source.nc->display);
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << access->mask;

						ci->EraseAccess(i - 1);
						Event::OnAccessDel(&Event::AccessDel::OnAccessDel, ci, source, access);
						delete access;
					}
					return;
				}
			}

			source.Reply(_("\002{0}\002 was not found on the access list of \002{1}\002."), mask, ci->name);
		}
	}

	void ProcessList(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params, ListFormatter &list)
	{
		const Anope::string &nick = params.size() > 2 ? params[2] : "";

		if (!ci->GetAccessCount())
		{
			source.Reply(_("The access list for \002{0}\002 is empty."), ci->name);
			return;
		}

		if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class AccessListCallback : public NumberList
			{
				ListFormatter &list;
				ChanServ::Channel *ci;

			 public:
				AccessListCallback(ListFormatter &_list, ChanServ::Channel *_ci, const Anope::string &numlist) : NumberList(numlist, false), list(_list), ci(_ci)
				{
				}

				void HandleNumber(unsigned number) override
				{
					if (!number || number > ci->GetAccessCount())
						return;

					const ChanServ::ChanAccess *access = ci->GetAccess(number - 1);

					Anope::string timebuf;
					if (ci->c)
						for (Channel::ChanUserList::const_iterator cit = ci->c->users.begin(), cit_end = ci->c->users.end(); cit != cit_end; ++cit)
						{
							ChanServ::ChanAccess::Path p;
							if (access->Matches(cit->second->user, cit->second->user->Account(), p))
								timebuf = "Now";
						}
					if (timebuf.empty())
					{
						if (access->last_seen == 0)
							timebuf = "Never";
						else
							timebuf = Anope::strftime(access->last_seen, NULL, true);
					}

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(number);
					entry["Level"] = access->AccessSerialize();
					entry["Mask"] = access->mask;
					entry["By"] = access->creator;
					entry["Last seen"] = timebuf;
					this->list.AddEntry(entry);
				}
			}
			nl_list(list, ci, nick);
			nl_list.Process();
		}
		else
		{
			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				const ChanServ::ChanAccess *access = ci->GetAccess(i);

				if (!nick.empty() && !Anope::Match(access->mask, nick))
					continue;

				Anope::string timebuf;
				if (ci->c)
					for (Channel::ChanUserList::const_iterator cit = ci->c->users.begin(), cit_end = ci->c->users.end(); cit != cit_end; ++cit)
					{
						ChanServ::ChanAccess::Path p;
						if (access->Matches(cit->second->user, cit->second->user->Account(), p))
							timebuf = "Now";
					}
				if (timebuf.empty())
				{
					if (access->last_seen == 0)
						timebuf = "Never";
					else
						timebuf = Anope::strftime(access->last_seen, NULL, true);
				}

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				entry["Level"] = access->AccessSerialize();
				entry["Mask"] = access->mask;
				entry["By"] = access->creator;
				entry["Last seen"] = timebuf;
				list.AddEntry(entry);
			}
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on the access list of \002{0}\002."), ci->name);
		else
		{
			std::vector<Anope::string> replies;
			list.Process(replies);

			source.Reply(_("Access list for \002{0}\002:"), ci->name);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);

			source.Reply(_("End of access list"));
		}

	}

	void DoList(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		if (!ci->GetAccessCount())
		{
			source.Reply(_("The access list for \002{0}\002 is empty."), ci->name);
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Level")).AddColumn(_("Mask"));
		this->ProcessList(source, ci, params, list);
	}

	void DoView(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		if (!ci->GetAccessCount())
		{
			source.Reply(_("The access list for \002{0}\002 is empty."), ci->name);
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Level")).AddColumn(_("Mask")).AddColumn(_("By")).AddColumn(_("Last seen"));
		this->ProcessList(source, ci, params, list);
	}

	void DoClear(CommandSource &source, ChanServ::Channel *ci)
	{
		if (!source.IsFounder(ci) && !source.HasPriv("chanserv/access/modify"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->name);
			return;
		}

		Event::OnAccessClear(&Event::AccessClear::OnAccessClear, ci, source);

		ci->ClearAccess();

		source.Reply(_("The access list of \002{0}\002 has been cleared."), ci->name);

		bool override = !source.IsFounder(ci);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to clear the access list";
	}

 public:
	CommandCSAccess(Module *creator) : Command(creator, "chanserv/access", 2, 4)
	{
		this->SetDesc(_("Modify the list of privileged users"));
		this->SetSyntax(_("\037channel\037 ADD \037mask\037 \037level\037"));
		this->SetSyntax(_("\037channel\037 DEL {\037mask\037 | \037entry-num\037 | \037list\037}"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 VIEW [\037mask\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 CLEAR"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &cmd = params[1];
		const Anope::string &nick = params.size() > 2 ? params[2] : "";
		const Anope::string &s = params.size() > 3 ? params[3] : "";

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		bool is_list = cmd.equals_ci("LIST") || cmd.equals_ci("VIEW");
		bool is_clear = cmd.equals_ci("CLEAR");
		bool is_del = cmd.equals_ci("DEL");

		ChanServ::AccessGroup access = source.AccessFor(ci);

		bool has_access = false;
		if (source.HasPriv("chanserv/access/modify"))
			has_access = true;
		else if (is_list && source.AccessFor(ci).HasPriv("ACCESS_LIST"))
			has_access = true;
		else if (source.AccessFor(ci).HasPriv("ACCESS_CHANGE"))
			has_access = true;
		else if (is_del)
		{
			const NickServ::Nick *na = NickServ::FindNick(nick);
			if (na && na->nc == source.GetAccount())
				has_access = true;
		}

		/* If LIST, we don't *require* any parameters, but we can take any.
		 * If DEL, we require a nick and no level.
		 * Else (ADD), we require a level (which implies a nick). */
		if (is_list || is_clear ? 0 : (cmd.equals_ci("DEL") ? (nick.empty() || !s.empty()) : s.empty()))
			this->OnSyntaxError(source, cmd);
		else if (!has_access)
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), is_list ? "ACCESS_LIST" : "ACCESS_CHANGE", ci->name);
		else if (Anope::ReadOnly && !is_list)
			source.Reply(_("Sorry, channel access list modification is temporarily disabled."));
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
		if (subcommand.equals_ci("ADD"))
		{
			source.Reply(_("The \002{0} ADD\002 adds \037mask\037 to the access list of \037channel\037 at level \037level\037."
			               " If \037mask\037 is already present on the access list, the access level for it is changed to \037level\037."
			               " The \037level\037 may be a numerical level between \002{1}\002 and \002{2}\002 or the name of a privilege (eg. \002{3}\002)."
			               " The privilege set granted to a given user is the union of the privileges of access entries that match the user."
			               " Use of this command requires the \002{4}\002 privilege on \037channel\037."),
			               source.command, ChanServ::ACCESS_INVALID + 1, ChanServ::ACCESS_FOUNDER - 1, "AUTOOP", "ACCESS_CHANGE");

			if (!Config->GetModule("chanserv")->Get<bool>("disallow_channel_access"))
				source.Reply(_("The given \037mask\037 may also be a channel, which will use the access list from the other channel up to the given \037level\037."));

			//XXX show def levels

			source.Reply(_("\n"
			               "Examples:\n"
			               "         {command} #anope ADD Adam 9001\n"
			               "          Adds \"Adam\" to the access list of \"#anope\" at level \"9001\".\n"
			               "\n"
			               "         {command} #anope ADD *!*@anope.org AUTOOP\n"
			               "         Adds the host mask \"*!*@anope.org\" to the access list of \"#anope\" with the privilege \"AUTOOP\"."));
		}
		else if (subcommand.equals_ci("DEL"))
			source.Reply(_("The \002{0} DEL\002 command removes \037mask\037 from the access list of \037channel\037."
				       " If a list of entry numbers is given, those entries are deleted."
			               " You may remove yourself from an access list, even if you do not have access to modify that list otherwise."
			               " Use of this command requires the \002{1}\002 privilege on \037channel\037.\n"
			               "\n"
			               "Example:\n"
			               "         {command} #anope del DukePyrolator\n"
			               "         Removes the access of \"DukePyrolator\" from \"#anope\"."),
			               source.command, "ACCESS_CHANGE");
		else if (subcommand.equals_ci("LIST") || subcommand.equals_ci("VIEW"))
			source.Reply(_("The \002{0} LIST\002 and \002{0} VIEW\002 command displays the access list of \037channel\037."
			               " If a wildcard mask is given, only those entries matching the mask are displayed."
			               " If a list of entry numbers is given, only those entries are shown."
			               " \002VIEW\002 is similar to \002LIST\002 but also shows who created the access entry, and when the access entry was last used."
			               " Use of these commands requires the \002{1}\002 privilege on \037channel\037.\n"
			               "\n"
			               "Example:\n"
					"         {0} #anope LIST 2-5,7-9\n"
					"         Lists access entries numbered 2 through 5 and 7 through 9 on #anope."),
			                source.command, "ACCESS_LIST");
		else if (subcommand.equals_ci("CLEAR"))
			source.Reply(_("The \002{0} CLEAR\002 command clears the access list of \037channel\037."
				       " Use of this command requires the \002{1}\002 privilege on \037channel\037."),
			               source.command, "FOUNDER");

		else
		{
			source.Reply(_("Maintains the access list for \037channel\037. The access list specifies which users are granted which privileges to the channel."
			               " The access system uses numerical levels to represent different sets of privileges. Users who are identified but do not match any entries on"
			               " the access list has a level of 0. Unregistered or unidentified users who do not match any entries have a user level of 0."));
			BotInfo *bi;
			Anope::string name;
			CommandInfo *help = source.service->FindCommand("generic/help");
			if (Command::FindCommandFromService("chanserv/levels", bi, name) && help)
				source.Reply(_("\n"
				              "Access levels can be configured via the \002{levels}\002 command. See \002{msg}{service} {help} {levels}\002 for more information."),
				              "msg"_kw = Config->StrictPrivmsg, "service"_kw = bi->nick, "help"_kw = help->cname, "levels"_kw = name);

			if (help)
				source.Reply(_("\n"
				               "The \002ADD\002 command adds \037mask\037 to the access list at level \037level\037.\n"
				               "Use of this command requires the \002{change}\002 privilege on \037channel\037.\n"
				               "\002{msg}{service} {help} {command} ADD\002 for more information.\n"
				               "\n"
				               "The \002DEL\002 command removes \037mask\037 from the access list.\n"
				               "Use of this command requires the \002{change}\002 privilege on \037channel\037.\n"
				               "\002{msg}{service} {help} {command} DEL\002 for more information.\n"
				               "\n"
				               "The \002LIST\002 and \002VIEW\002 commands both show the access list for \037channel\037, but \002VIEW\002 also shows who created the access entry, and when the user was last seen.\n"
				               "Use of these commands requires the \002{list}\002 privilege on \037channel\037.\n"
				               "\002{msg}{service} {help} {command} [LIST | VIEW]\002 for more information.\n"
				               "\n"
				               "The \002CLEAR\002 command clears the access list."
				               "Use of this command requires the \002{founder}\002 privilege on \037channel\037.\n"
				               "\002{msg}{service} {help} {command} CLEAR\002 for more information."),
				               "msg"_kw = Config->StrictPrivmsg, "service"_kw = source.service->nick, "command"_kw = source.command,
				               "help"_kw = help->cname, "change"_kw = "ACCESS_CHANGE", "list"_kw = "ACCESS_LIST", "founder"_kw = "FOUNDER");
		}

		return true;
	}
};

class CommandCSLevels : public Command
{
	void DoSet(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &what = params[2];
		const Anope::string &lev = params[3];

		int level;

		if (lev.equals_ci("FOUNDER"))
			level = ChanServ::ACCESS_FOUNDER;
		else
		{
			try
			{
				level = convertTo<int>(lev);
			}
			catch (const ConvertException &)
			{
				this->OnSyntaxError(source, "SET");
				return;
			}
		}

		if (level <= ChanServ::ACCESS_INVALID || level > ChanServ::ACCESS_FOUNDER)
			source.Reply(_("Level must be between \002{0}\002 and \002{1}\002 inclusive."), ChanServ::ACCESS_INVALID + 1, ChanServ::ACCESS_FOUNDER - 1);
		else
		{
			ChanServ::Privilege *p = ChanServ::service ? ChanServ::service->FindPrivilege(what) : nullptr;
			if (p == NULL)
			{
				CommandInfo *help = source.service->FindCommand("generic/help");
				if (help)
					source.Reply(_("There is no such privilege \002{0}\002. See \002{0}{1} {2} {3}\002 for a list of valid settings."),
					                what, Config->StrictPrivmsg, source.service->nick, help->cname, source.command);
			}
			else
			{
				bool override = !source.AccessFor(ci).HasPriv("FOUNDER");
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to set " << p->name << " to level " << level;

				ci->SetLevel(p->name, level);
				this->onlevelchange(&Event::LevelChange::OnLevelChange, source, ci, p->name, level);

				if (level == ChanServ::ACCESS_FOUNDER)
					source.Reply(_("Level for privilege \002{0}\002 on channel \002{1}\002 changed to \002founder only\002."), p->name, ci->name);
				else
					source.Reply(_("Level for privilege \002{0}\002 on channel \002{1}\002 changed to \002{3}\002."), p->name, ci->name, level);
			}
		}
	}

	void DoDisable(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		const Anope::string &what = params[2];

		/* Don't allow disabling of the founder level. It would be hard to change it back if you dont have access to use this command */
		if (what.equals_ci("FOUNDER"))
		{
			source.Reply(_("You can not disable the founder privilege because it would be impossible to reenable it at a later time."));
			return;
		}

		ChanServ::Privilege *p = ChanServ::service ? ChanServ::service->FindPrivilege(what) : nullptr;
		if (p != NULL)
		{
			bool override = !source.AccessFor(ci).HasPriv("FOUNDER");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable " << p->name;

			ci->SetLevel(p->name, ChanServ::ACCESS_INVALID);
			this->onlevelchange(&Event::LevelChange::OnLevelChange, source, ci, p->name, ChanServ::ACCESS_INVALID);

			source.Reply(_("Privileged \002{0}\002 disabled on channel \002{1}\002."), p->name, ci->name);
			return;
		}

		CommandInfo *help = source.service->FindCommand("generic/help");
		if (help)
			source.Reply(_("There is no such privilege \002{0}\002. See \002{0}{1} {2} {3}\002 for a list of valid settings."),
			               what, Config->StrictPrivmsg, source.service->nick, help->cname, source.command);
	}

	void DoList(CommandSource &source, ChanServ::Channel *ci)
	{
		if (!ChanServ::service)
			return;

		source.Reply(_("Access level settings for channel \002{0}\002"), ci->name);

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Name")).AddColumn(_("Level"));

		const std::vector<ChanServ::Privilege> &privs = ChanServ::service->GetPrivileges();

		for (unsigned i = 0; i < privs.size(); ++i)
		{
			const ChanServ::Privilege &p = privs[i];
			int16_t j = ci->GetLevel(p.name);

			ListFormatter::ListEntry entry;
			entry["Name"] = p.name;

			if (j == ChanServ::ACCESS_INVALID)
				entry["Level"] = Language::Translate(source.GetAccount(), _("(disabled)"));
			else if (j == ChanServ::ACCESS_FOUNDER)
				entry["Level"] = Language::Translate(source.GetAccount(), _("(founder only)"));
			else
				entry["Level"] = stringify(j);

			list.AddEntry(entry);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

	void DoReset(CommandSource &source, ChanServ::Channel *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("FOUNDER");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to reset all levels";

		reset_levels(ci);
		this->onlevelchange(&Event::LevelChange::OnLevelChange, source, ci, "ALL", 0);

		source.Reply(_("Levels for \002{0}\002 reset to defaults."), ci->name);
	}

	EventHandlers<Event::LevelChange> &onlevelchange;

 public:
	CommandCSLevels(Module *creator, EventHandlers<Event::LevelChange> &event) : Command(creator, "chanserv/levels", 2, 4), onlevelchange(event)
	{
		this->SetDesc(_("Redefine the meanings of access levels"));
		this->SetSyntax(_("\037channel\037 SET \037privilege\037 \037level\037"));
		this->SetSyntax(_("\037channel\037 {DIS | DISABLE} \037privilege\037"));
		this->SetSyntax(_("\037channel\037 LIST"));
		this->SetSyntax(_("\037channel\037 RESET"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &cmd = params[1];
		const Anope::string &what = params.size() > 2 ? params[2] : "";
		const Anope::string &s = params.size() > 3 ? params[3] : "";

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		/* If SET, we want two extra parameters; if DIS[ABLE] or FOUNDER, we want only
		 * one; else, we want none.
		 */
		if (cmd.equals_ci("SET") ? s.empty() : (cmd.substr(0, 3).equals_ci("DIS") ? (what.empty() || !s.empty()) : !what.empty()))
			this->OnSyntaxError(source, cmd);
		else if (!source.AccessFor(ci).HasPriv("FOUNDER") && !source.HasPriv("chanserv/access/modify"))
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->name);	
		else if (Anope::ReadOnly && !cmd.equals_ci("LIST"))
			source.Reply(_("Services are in read-only mode."));
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
			source.Reply(_("The following privileges are available:"));

			ListFormatter list(source.GetAccount());
			list.AddColumn(_("Name")).AddColumn(_("Description"));

			if (ChanServ::service)
			{
				const std::vector<ChanServ::Privilege> &privs = ChanServ::service->GetPrivileges();
				for (unsigned i = 0; i < privs.size(); ++i)
				{
					const ChanServ::Privilege &p = privs[i];
					ListFormatter::ListEntry entry;
					entry["Name"] = p.name;
					entry["Description"] = Language::Translate(source.nc, p.desc.c_str());
					list.AddEntry(entry);
				}
			}

			std::vector<Anope::string> replies;
			list.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
		else
		{
			BotInfo *bi;
			Anope::string name;
			if (!Command::FindCommandFromService("chanserv/access", bi, name) || bi != source.service)
				return false;
			CommandInfo *help = source.service->FindCommand("generic/help");
			if (!help)
				return false;

			source.Reply(_("The \002{0}\002 command allows fine control over the meaning of numeric access levels used in the \002{1}\001 command.\n"
			               "\n"
			               "\002{0} SET\002 allows changing which \037privilege\037 is included in a given \037level\37.\n"
			               "\n"
			               "\002{0} DISABLE\002 disables a privilege and prevents anyone from be granted it, even channel founders."
			               " The \002{2}\002 privilege can not be disabled.\n"
			               "\n"
			               "\002{0} LIST\002 shows the current level for each privilege.\n"
			               "\n"
			               "\002{0} RESET\002 resets the levels to the default levels for newly registered channels.\n"
			               "\n"
			               "For the list of privileges and their descriptions, see \002{3} {4} DESC\002."),
				       source.command, name, "FOUNDER", help->cname, source.command);
		}
		return true;
	}
};

class CSAccess : public Module
	, public EventHook<Event::CreateChan>
	, public EventHook<Event::GroupCheckPriv>
{
	AccessAccessProvider accessprovider;
	CommandCSAccess commandcsaccess;
	CommandCSLevels commandcslevels;
	EventHandlers<Event::LevelChange> onlevelchange;

 public:
	CSAccess(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::CreateChan>("OnCreateChan")
		, EventHook<Event::GroupCheckPriv>("OnGroupCheckPriv")
		, accessprovider(this)
		, commandcsaccess(this)
		, commandcslevels(this, onlevelchange)
		, onlevelchange(this, "OnLevelChange")
	{
		this->SetPermanent(true);

	}

	void OnReload(Configuration::Conf *conf) override
	{
		defaultLevels.clear();

		for (int i = 0; i < conf->CountBlock("privilege"); ++i)
		{
			Configuration::Block *priv = conf->GetBlock("privilege", i);

			const Anope::string &pname = priv->Get<const Anope::string>("name");

			ChanServ::Privilege *p = ChanServ::service ? ChanServ::service->FindPrivilege(pname) : nullptr;
			if (p == NULL)
				continue;

			const Anope::string &value = priv->Get<const Anope::string>("level");
			if (value.empty())
				continue;
			else if (value.equals_ci("founder"))
				defaultLevels[p->name] = ChanServ::ACCESS_FOUNDER;
			else if (value.equals_ci("disabled"))
				defaultLevels[p->name] = ChanServ::ACCESS_INVALID;
			else
				defaultLevels[p->name] = priv->Get<int16_t>("level");
		}
	}

	void OnCreateChan(ChanServ::Channel *ci) override
	{
		reset_levels(ci);
	}

	EventReturn OnGroupCheckPriv(const ChanServ::AccessGroup *group, const Anope::string &priv) override
	{
		if (group->ci == NULL)
			return EVENT_CONTINUE;
		/* Special case. Allows a level of < 0 to match anyone, and a level of 0 to match anyone identified. */
		int16_t level = group->ci->GetLevel(priv);
		if (level < 0)
			return EVENT_ALLOW;
		else if (level == 0 && group->nc)
			return EVENT_ALLOW;
		return EVENT_CONTINUE;
	}
};

MODULE_INIT(CSAccess)
