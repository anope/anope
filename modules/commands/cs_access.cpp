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

class AccessChanAccess : public ChanServ::ChanAccess
{
 public:
	AccessChanAccess(Serialize::TypeBase *type) : ChanServ::ChanAccess(type) { }
	AccessChanAccess(Serialize::TypeBase *type, Serialize::ID id) : ChanServ::ChanAccess(type, id) { }

	int GetLevel();
	void SetLevel(const int &);

	bool HasPriv(const Anope::string &name) override
	{
		return this->GetChannel()->GetLevel(name) != ChanServ::ACCESS_INVALID && this->GetLevel() >= this->GetChannel()->GetLevel(name);
	}

	Anope::string AccessSerialize()
	{
		return stringify(this->GetLevel());
	}

	void AccessUnserialize(const Anope::string &data) override
	{
		try
		{
			this->SetLevel(convertTo<int>(data));
		}
		catch (const ConvertException &)
		{
		}
	}

	bool operator>(ChanServ::ChanAccess &other) override
	{
		if (this->GetSerializableType() != other.GetSerializableType())
			return ChanServ::ChanAccess::operator>(other);
		else
			return this->GetLevel() > anope_dynamic_static_cast<AccessChanAccess *>(&other)->GetLevel();
	}

	bool operator<(ChanServ::ChanAccess &other) override
	{
		if (this->GetSerializableType() != other.GetSerializableType())
			return ChanAccess::operator<(other);
		else
			return this->GetLevel() < anope_dynamic_static_cast<AccessChanAccess *>(&other)->GetLevel();
	}
};

class AccessChanAccessType : public Serialize::Type<AccessChanAccess, ChanServ::ChanAccessType>
{
 public:
	Serialize::Field<AccessChanAccess, int> level;

	AccessChanAccessType(Module *me) : Serialize::Type<AccessChanAccess, ChanAccessType>(me, "AccessChanAccess")
		, level(this, "level")
	{
		SetParent(ChanServ::chanaccess);
	}
};

int AccessChanAccess::GetLevel()
{
	return Get(&AccessChanAccessType::level);
}

void AccessChanAccess::SetLevel(const int &i)
{
	Object::Set(&AccessChanAccessType::level, i);
}

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
			if (p != NULL && p->level)
				level = p->level;
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
		ChanServ::ChanAccess *highest = u_access.Highest();

		AccessChanAccess tmp_access(nullptr);
		tmp_access.SetChannel(ci);
		tmp_access.SetLevel(level);

		bool override = false;

		if ((!highest || *highest <= tmp_access) && !u_access.founder)
		{
			if (source.HasPriv("chanserv/access/modify"))
				override = true;
			else
			{
				source.Reply(_("Access denied. You do not have enough privileges on \002{0}\002 to add someone at level \002{1}\002."), ci->GetName(), level);
				return;
			}
		}

		NickServ::Nick *na = nullptr;
		ChanServ::Channel *targ_ci = nullptr;

		if (IRCD->IsChannelValid(mask))
		{
			if (Config->GetModule("chanserv")->Get<bool>("disallow_channel_access"))
			{
				source.Reply(_("Channels may not be on access lists."));
				return;
			}

			targ_ci = ChanServ::Find(mask);
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

			mask = targ_ci->GetName();
		}
		else
		{
			na = NickServ::FindNick(mask);
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
			ChanServ::ChanAccess *access = ci->GetAccess(i - 1);
			if (mask.equals_ci(access->Mask()))
			{
				/* Don't allow lowering from a level >= u_level */
				if ((!highest || *access >= *highest) && !u_access.founder && !source.HasPriv("chanserv/access/modify"))
				{
					source.Reply(_("Access denied. You do not have enough privileges on \002{0}\002 to lower the access of \002{1}\002."), ci->GetName(), access->Mask());
					return;
				}
				delete access;
				break;
			}
		}

		unsigned access_max = Config->GetModule("chanserv")->Get<unsigned>("accessmax", "1024");
		if (access_max && ci->GetDeepAccessCount() >= access_max)
		{
			source.Reply(_("Sorry, you can only have %d access entries on a channel, including access entries from other channels."), access_max);
			return;
		}

		AccessChanAccess *access = anope_dynamic_static_cast<AccessChanAccess *>(accesschanaccess.Create());
		if (na)
			access->SetObj(na->GetAccount());
		else if (targ_ci)
			access->SetObj(targ_ci);
		access->SetChannel(ci);
		access->SetMask(mask);
		access->SetCreator(source.GetNick());
		access->SetLevel(level);
		access->SetLastSeen(0);
		access->SetCreated(Anope::CurTime);

		Event::OnAccessAdd(&Event::AccessAdd::OnAccessAdd, ci, source, access);

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to add " << mask << " with level " << level;
		if (p != NULL)
			source.Reply(_("\002{0}\002 added to the access list of \002{1}\002 with privilege \002{2}\002 (level \002{3}\002)."), access->Mask(), ci->GetName(), p->name, level);
		else
			source.Reply(_("\002{0}\002 added to the access list of \002{1}\002 at level \002{2}\002."), access->Mask(), ci->GetName(), level);
	}

	void DoDel(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params[2];

		if (!ci->GetAccessCount())
		{
			source.Reply(_("The access list for \002{0}\002 is empty."), ci->GetName());
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
			bool override = !source.AccessFor(ci).HasPriv("ACCESS_CHANGE") && source.HasPriv("chanserv/access/modify");
			Anope::string nicks;
			bool denied = false;
			unsigned int deleted = 0;

			NumberList(mask, true,
				[&](unsigned int num)
				{
					if (!num || num > ci->GetAccessCount())
						return;

					ChanServ::ChanAccess *access = ci->GetAccess(num - 1);

					ChanServ::AccessGroup ag = source.AccessFor(ci);
					ChanServ::ChanAccess *u_highest = ag.Highest();

					if ((!u_highest || *u_highest <= *access) && !ag.founder && !override && access->GetObj() != source.nc)
					{
						denied = true;
						return;
					}

					++deleted;
					if (!nicks.empty())
						nicks += ", " + access->Mask();
					else
						nicks = access->Mask();

					Event::OnAccessDel(&Event::AccessDel::OnAccessDel, ci, source, access);
					delete access;
				},
				[&]()
				{
					if (denied && !deleted)
						source.Reply(_("Access denied. You do not have enough privileges on \002{0}\002 to remove any access entries matching \002{1}\002."));
					else if (!deleted)
						source.Reply(_("There are no entries matching \002{0}\002 on the access list of \002{1}\002."), mask, ci->GetName());
					else
					{
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << nicks;

						if (deleted == 1)
							source.Reply(_("Deleted \0021\002 entry from the access list of \002{0}\002."), ci->GetName());
						else
							source.Reply(_("Deleted \002{0}\002 entries from the access list of \002{1}\002."), deleted, ci->GetName());
					}
				});
		}
		else
		{
			ChanServ::AccessGroup u_access = source.AccessFor(ci);
			ChanServ::ChanAccess *highest = u_access.Highest();

			for (unsigned i = ci->GetAccessCount(); i > 0; --i)
			{
				ChanServ::ChanAccess *access = ci->GetAccess(i - 1);
				if (mask.equals_ci(access->Mask()))
				{
					if (access->GetObj() != source.nc && !u_access.founder && (!highest || *highest <= *access) && !source.HasPriv("chanserv/access/modify"))
						source.Reply(_("Access denied. You do not have enough privileges on \002{0}\002 to remove the access of \002{1}\002."), ci->GetName(), access->Mask());
					else
					{
						source.Reply(_("\002{0}\002 deleted from the access list of \002{1}\002."), access->Mask(), ci->GetName());
						bool override = !u_access.founder && !u_access.HasPriv("ACCESS_CHANGE") && !access->Mask().equals_ci(source.nc->GetDisplay());
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << access->Mask();

						Event::OnAccessDel(&Event::AccessDel::OnAccessDel, ci, source, access);
						delete access;
					}
					return;
				}
			}

			source.Reply(_("\002{0}\002 was not found on the access list of \002{1}\002."), mask, ci->GetName());
		}
	}

	void ProcessList(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params, ListFormatter &list)
	{
		const Anope::string &nick = params.size() > 2 ? params[2] : "";

		if (!ci->GetAccessCount())
		{
			source.Reply(_("The access list for \002{0}\002 is empty."), ci->GetName());
			return;
		}

		if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			NumberList(nick, false,
				[&](unsigned int number)
				{
					if (!number || number > ci->GetAccessCount())
						return;

					ChanServ::ChanAccess *access = ci->GetAccess(number - 1);

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
						if (access->GetLastSeen() == 0)
							timebuf = "Never";
						else
							timebuf = Anope::strftime(access->GetLastSeen(), NULL, true);
					}

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(number);
					entry["Level"] = access->AccessSerialize();
					entry["Mask"] = access->Mask();
					entry["By"] = access->GetCreator();
					entry["Last seen"] = timebuf;
					list.AddEntry(entry);
				},
				[&](){});
		}
		else
		{
			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				ChanServ::ChanAccess *access = ci->GetAccess(i);

				if (!nick.empty() && !Anope::Match(access->Mask(), nick))
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
					if (access->GetLastSeen() == 0)
						timebuf = "Never";
					else
						timebuf = Anope::strftime(access->GetLastSeen(), NULL, true);
				}

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				entry["Level"] = access->AccessSerialize();
				entry["Mask"] = access->Mask();
				entry["By"] = access->GetCreator();
				entry["Last seen"] = timebuf;
				list.AddEntry(entry);
			}
		}

		if (list.IsEmpty())
		{
			source.Reply(_("No matching entries on the access list of \002{0}\002."), ci->GetName());
			return;
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		source.Reply(_("Access list for \002{0}\002:"), ci->GetName());

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("End of access list."));
	}

	void DoList(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		if (!ci->GetAccessCount())
		{
			source.Reply(_("The access list for \002{0}\002 is empty."), ci->GetName());
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
			source.Reply(_("The access list for \002{0}\002 is empty."), ci->GetName());
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
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->GetName());
			return;
		}

		Event::OnAccessClear(&Event::AccessClear::OnAccessClear, ci, source);

		ci->ClearAccess();

		source.Reply(_("The access list of \002{0}\002 has been cleared."), ci->GetName());

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
			NickServ::Nick *na = NickServ::FindNick(nick);
			if (na && na->GetAccount() == source.GetAccount())
				has_access = true;
		}

		/* If LIST, we don't *require* any parameters, but we can take any.
		 * If DEL, we require a nick and no level.
		 * Else (ADD), we require a level (which implies a nick). */
		if (is_list || is_clear ? 0 : (cmd.equals_ci("DEL") ? (nick.empty() || !s.empty()) : s.empty()))
		{
			this->OnSyntaxError(source, cmd);
			return;
		}

		if (!has_access)
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), is_list ? "ACCESS_LIST" : "ACCESS_CHANGE", ci->GetName());
			return;
		}

		if (Anope::ReadOnly && !is_list)
		{
			source.Reply(_("Sorry, channel access list modification is temporarily disabled."));
			return;
		}

		if (cmd.equals_ci("ADD"))
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
			ServiceBot *bi;
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
					source.Reply(_("Level for privilege \002{0}\002 on channel \002{1}\002 changed to \002founder only\002."), p->name, ci->GetName());
				else
					source.Reply(_("Level for privilege \002{0}\002 on channel \002{1}\002 changed to \002{3}\002."), p->name, ci->GetName(), level);
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

			source.Reply(_("Privileged \002{0}\002 disabled on channel \002{1}\002."), p->name, ci->GetName());
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

		source.Reply(_("Access level settings for channel \002{0}\002"), ci->GetName());

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

		ci->ClearLevels();
		this->onlevelchange(&Event::LevelChange::OnLevelChange, source, ci, "ALL", 0);

		source.Reply(_("Levels for \002{0}\002 reset to defaults."), ci->GetName());
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
		{
			this->OnSyntaxError(source, cmd);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("FOUNDER") && !source.HasPriv("chanserv/access/modify"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->GetName());	
			return;
		}

		if (Anope::ReadOnly && !cmd.equals_ci("LIST"))
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		if (cmd.equals_ci("SET"))
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
			ServiceBot *bi;
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
	, public EventHook<Event::GroupCheckPriv>
{
	CommandCSAccess commandcsaccess;
	CommandCSLevels commandcslevels;
	EventHandlers<Event::LevelChange> onlevelchange;
	AccessChanAccessType accesschanaccesstype;

 public:
	CSAccess(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::GroupCheckPriv>("OnGroupCheckPriv")
		, commandcsaccess(this)
		, commandcslevels(this, onlevelchange)
		, onlevelchange(this, "OnLevelChange")
		, accesschanaccesstype(this)
	{
		this->SetPermanent(true);

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
