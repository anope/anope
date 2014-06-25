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

namespace
{
	std::vector<Anope::string> order;
	std::map<Anope::string, std::vector<Anope::string> > permissions;
}

class XOPChanAccess : public ChanServ::ChanAccess
{
 public:
	Anope::string type;

	XOPChanAccess(ChanServ::AccessProvider *p) : ChanAccess(p)
	{
	}

	bool HasPriv(const Anope::string &priv) const override
	{
		for (std::vector<Anope::string>::iterator it = std::find(order.begin(), order.end(), this->type); it != order.end(); ++it)
		{
			const std::vector<Anope::string> &privs = permissions[*it];
			if (std::find(privs.begin(), privs.end(), priv) != privs.end())
				return true;
		}
		return false;
	}

	Anope::string AccessSerialize() const
	{
		return this->type;
	}

	void AccessUnserialize(const Anope::string &data) override
	{
		this->type = data;
	}

	static Anope::string DetermineLevel(const ChanServ::ChanAccess *access)
	{
		if (access->provider->name == "access/xop")
		{
			const XOPChanAccess *xaccess = anope_dynamic_static_cast<const XOPChanAccess *>(access);
			return xaccess->type;
		}
		else
		{
			std::map<Anope::string, int> count;

			for (std::map<Anope::string, std::vector<Anope::string> >::const_iterator it = permissions.begin(), it_end = permissions.end(); it != it_end; ++it)
			{
				int &c = count[it->first];
				const std::vector<Anope::string> &perms = it->second;
				for (unsigned i = 0; i < perms.size(); ++i)
					if (access->HasPriv(perms[i]))
						++c;
			}

			Anope::string max;
			int maxn = 0;
			for (std::map<Anope::string, int>::iterator it = count.begin(), it_end = count.end(); it != it_end; ++it)
				if (it->second > maxn)
				{
					maxn = it->second;
					max = it->first;
				}

			return max;
		}
	}
};

class XOPAccessProvider : public ChanServ::AccessProvider
{
 public:
	XOPAccessProvider(Module *o) : AccessProvider(o, "access/xop")
	{
	}

	ChanServ::ChanAccess *Create() override
	{
		return new XOPChanAccess(this);
	}
};

class CommandCSXOP : public Command
{
 private:
	void DoAdd(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params.size() > 2 ? params[2] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, channel %s list modification is temporarily disabled."), source.command.c_str());
			return;
		}

		XOPChanAccess tmp_access(NULL);
		tmp_access.ci = ci;
		tmp_access.type = source.command.upper();

		ChanServ::AccessGroup access = source.AccessFor(ci);
		const ChanServ::ChanAccess *highest = access.Highest();
		bool override = false;

		std::vector<Anope::string>::iterator cmd_it = std::find(order.begin(), order.end(), source.command.upper()),
			access_it = highest ? std::find(order.begin(), order.end(), XOPChanAccess::DetermineLevel(highest)) : order.end();

		if (!access.founder && (!access.HasPriv("ACCESS_CHANGE") || cmd_it <= access_it))
		{
			if (source.HasPriv("chanserv/access/modify"))
				override = true;
			else
			{
				source.Reply(_("Access denied. You do not have the \002{0}\002 privilege on \002{1}\002."), "ACCESS_CHANGE", ci->name);
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

		for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
		{
			const ChanServ::ChanAccess *a = ci->GetAccess(i);

			if (a->Mask().equals_ci(mask))
			{
				if ((!highest || *a >= *highest) && !access.founder && !source.HasPriv("chanserv/access/modify"))
				{
					source.Reply(_("Access denied. You do not have enough privileges on \002{0}\002 to lower the access of \002{1}\002."), ci->name, a->Mask());
					return;
				}

				delete ci->EraseAccess(i);
				break;
			}
		}

		unsigned access_max = Config->GetModule("chanserv")->Get<unsigned>("accessmax", "1024");
		if (access_max && ci->GetDeepAccessCount() >= access_max)
		{
			source.Reply(_("Sorry, you can only have %d access entries on a channel, including access entries from other channels."), access_max);
			return;
		}

		ServiceReference<ChanServ::AccessProvider> provider("AccessProvider", "access/xop");
		if (!provider)
			return;
		XOPChanAccess *acc = anope_dynamic_static_cast<XOPChanAccess *>(provider->Create());
		acc->SetMask(mask, ci);
		acc->creator = source.GetNick();
		acc->type = source.command.upper();
		acc->last_seen = 0;
		acc->created = Anope::CurTime;
		ci->AddAccess(acc);

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to add " << mask;

		Event::OnAccessAdd(&Event::AccessAdd::OnAccessAdd, ci, source, acc);
		source.Reply(_("\002%s\002 added to %s %s list."), acc->Mask(), ci->name.c_str(), source.command.c_str());
	}

	void DoDel(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{
		NickServ::Account *nc = source.nc;
		Anope::string mask = params.size() > 2 ? params[2] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, channel %s list modification is temporarily disabled."), source.command.c_str());
			return;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s %s list is empty."), ci->name.c_str(), source.command.c_str());
			return;
		}

		XOPChanAccess tmp_access(NULL);
		tmp_access.ci = ci;
		tmp_access.type = source.command.upper();

		ChanServ::AccessGroup access = source.AccessFor(ci);
		const ChanServ::ChanAccess *highest = access.Highest();
		bool override = false;

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

		std::vector<Anope::string>::iterator cmd_it = std::find(order.begin(), order.end(), source.command.upper()),
			access_it = highest ? std::find(order.begin(), order.end(), XOPChanAccess::DetermineLevel(highest)) : order.end();

		if (!mask.equals_ci(nc->display) && !access.founder && (!access.HasPriv("ACCESS_CHANGE") || cmd_it <= access_it))
		{
			if (source.HasPriv("chanserv/access/modify"))
				override = true;
			else
			{
				source.Reply(_("Access denied. You do not have the \002{0}\002 privilege on \002{1}\002."), "ACCESS_CHANGE", ci->name);
				return;
			}
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class XOPDelCallback : public NumberList
			{
				CommandSource &source;
				ChanServ::Channel *ci;
				Command *c;
				unsigned deleted;
				Anope::string nicks;
				bool override;
			 public:
				XOPDelCallback(CommandSource &_source, ChanServ::Channel *_ci, Command *_c, bool _override, const Anope::string &numlist) : NumberList(numlist, true), source(_source), ci(_ci), c(_c), deleted(0), override(_override)
				{
				}

				~XOPDelCallback()
				{
					if (!deleted)
						 source.Reply(_("No matching entries on %s %s list."), ci->name.c_str(), source.command.c_str());
					else
					{
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, c, ci) << "to delete " << nicks;

						if (deleted == 1)
							source.Reply(_("Deleted one entry from %s %s list."), ci->name.c_str(), source.command.c_str());
						else
							source.Reply(_("Deleted %d entries from %s %s list."), deleted, ci->name.c_str(), source.command.c_str());
					}
				}

				void HandleNumber(unsigned number) override
				{
					if (!number || number > ci->GetAccessCount())
						return;

					ChanServ::ChanAccess *caccess = ci->GetAccess(number - 1);

					if (caccess->provider->name != "access/xop" || this->source.command.upper() != caccess->AccessSerialize())
						return;

					++deleted;
					if (!nicks.empty())
						nicks += ", " + caccess->Mask();
					else
						nicks = caccess->Mask();

					ci->EraseAccess(number - 1);
					Event::OnAccessDel(&Event::AccessDel::OnAccessDel, ci, source, caccess);
					delete caccess;
				}
			}
			delcallback(source, ci, this, override, mask);
			delcallback.Process();
		}
		else
		{
			for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
			{
				ChanServ::ChanAccess *a = ci->GetAccess(i);

				if (a->provider->name != "access/xop" || source.command.upper() != a->AccessSerialize())
					continue;

				if (a->Mask().equals_ci(mask))
				{
					Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to delete " << a->Mask();

					source.Reply(_("\002%s\002 deleted from %s %s list."), a->Mask().c_str(), ci->name.c_str(), source.command.c_str());

					ci->EraseAccess(i);
					Event::OnAccessDel(&Event::AccessDel::OnAccessDel, ci, source, a);
					delete a;

					return;
				}
			}

			source.Reply(_("\002%s\002 not found on %s %s list."), mask.c_str(), ci->name.c_str(), source.command.c_str());
		}
	}

	void DoList(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{

		const Anope::string &nick = params.size() > 2 ? params[2] : "";

		ChanServ::AccessGroup access = source.AccessFor(ci);

		if (!access.HasPriv("ACCESS_LIST") && !source.HasCommand("chanserv/access/list"))
		{
			source.Reply(_("Access denied. You do not have the \002{0}\002 privilege on \002{1}\002."), "ACCESS_LIST", ci->name);
			return;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s %s list is empty."), ci->name.c_str(), source.command.c_str());
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Mask"));

		if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class XOPListCallback : public NumberList
			{
				ListFormatter &list;
				ChanServ::Channel *ci;
				CommandSource &source;
			 public:
				XOPListCallback(ListFormatter &_list, ChanServ::Channel *_ci, const Anope::string &numlist, CommandSource &src) : NumberList(numlist, false), list(_list), ci(_ci), source(src)
				{
				}

				void HandleNumber(unsigned Number) override
				{
					if (!Number || Number > ci->GetAccessCount())
						return;

					const ChanServ::ChanAccess *a = ci->GetAccess(Number - 1);

					if (a->provider->name != "access/xop" || this->source.command.upper() != a->AccessSerialize())
						return;

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(Number);
					entry["Mask"] = a->Mask();
					this->list.AddEntry(entry);
				}
			} nl_list(list, ci, nick, source);
			nl_list.Process();
		}
		else
		{
			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				const ChanServ::ChanAccess *a = ci->GetAccess(i);

				if (a->provider->name != "access/xop" || source.command.upper() != a->AccessSerialize())
					continue;
				else if (!nick.empty() && !Anope::Match(a->Mask(), nick))
					continue;

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				entry["Mask"] = a->Mask();
				list.AddEntry(entry);
			}
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on %s access list."), ci->name.c_str());
		else
		{
			std::vector<Anope::string> replies;
			list.Process(replies);

			source.Reply(_("%s list for %s"), source.command.c_str(), ci->name.c_str());
			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
	}

	void DoClear(CommandSource &source, ChanServ::Channel *ci)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, channel %s list modification is temporarily disabled."), source.command.c_str());
			return;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(_("%s %s list is empty."), ci->name.c_str(), source.command.c_str());
			return;
		}

		if (!source.AccessFor(ci).HasPriv("FOUNDER") && !source.HasPriv("chanserv/access/modify"))
		{
			source.Reply(_("Access denied. You do not have the \002{0}\002 privilege on \002{1}\002."), "FOUNDER", ci->name);
			return;
		}

		bool override = !source.AccessFor(ci).HasPriv("FOUNDER");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to clear the access list";

		for (unsigned i = ci->GetAccessCount(); i > 0; --i)
		{
			const ChanServ::ChanAccess *access = ci->GetAccess(i - 1);

			if (access->provider->name != "access/xop" || source.command.upper() != access->AccessSerialize())
				continue;

			delete ci->EraseAccess(i - 1);
		}

		Event::OnAccessClear(&Event::AccessClear::OnAccessClear, ci, source);

		source.Reply(_("Channel %s %s list has been cleared."), ci->name.c_str(), source.command.c_str());
	}

 public:
	CommandCSXOP(Module *modname) : Command(modname, "chanserv/xop", 2, 4)
	{
		this->SetSyntax(_("\037channel\037 ADD \037mask\037"));
		this->SetSyntax(_("\037channel\037 DEL {\037mask\037 | \037entry-num\037 | \037list\037}"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 CLEAR"));
	}

 	const Anope::string GetDesc(CommandSource &source) const override
	{
		return Anope::printf(Language::Translate(source.GetAccount(), _("Modify the list of %s users")), source.command.upper().c_str());
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChanServ::Channel *ci = ChanServ::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), params[0]);
			return;
		}

		const Anope::string &cmd = params[1];

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, ci, params);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, ci, params);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, ci, params);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(source, ci);
		else
			this->OnSyntaxError(source, "");
	}


	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.equals_ci("ADD"))
		{
			source.Reply(_("The \002{0} ADD\002 command adds \037mask\037 to the {0} list of \037channel\037."
			               " If you have the privilege to use this command, you may use it if you have more privileges than it grants."),
			               source.command);

			source.Reply(_("Use of this command requires the \002{0}\002 privilege on \037channel\037.\n"
			               "Example:\n"
			               "         {1} #anope ADD Adam"
			               "         Adds \"Adam\" to the access list of \"#anope\" with the privilege set {1}"),
			               "ACCESS_CHANGE", source.command);
		}
		else if (subcommand.equals_ci("DEL"))
			source.Reply(_("The \002{0} DEL\002 command removes the given \037mask\037 from the {0} list of \037channel\037."
			               " If a list of entry numbers is given, those entries are deleted.\n"
			               "\n"
			               "Use of this command requires the \002{1}\002 privilege on \037channel\037.\n"
			               "Example:\n"
			               "         {0} #anope DEL Cronus"
			               "         Removes \"Cronus\" from the {0} list of \"#anope\""),
			               source.command, "ACCESS_CHANGE");
		else if (subcommand.equals_ci("LIST"))
			source.Reply(_("The \002{0} LIST\002 command displays the {0} list of \037channel\037."
			               " If a wildcard mask is given, only those entries matching the mask are displayed."
			               " If a list of entry numbers is given, only those entries are shown.\n"
			               "\n"
			               "Use of this command requires the \002{1}\002 privilege on \037channel\037.\n"
			               "\n"
			               "Example:"
			               "         {0} #anope LIST 2-5,7-9\n"
			               "         List entries numbered 2 through 5 and 7 through 9 on the {0} of \"#anope\""),
			               source.command, "ACCESS_LIST");
		else if (subcommand.equals_ci("CLEAR"))
			source.Reply(_("The \002{0} CLEAR\002 command clears the {0} list of \037channel\037."
			               "\n"
			               "Use of this command requires the \002{1}\002 privilege on \037channel\037."),
			               source.command, "FOUNDER");

		else
		{
			source.Reply(_("Maintains the \002{0} list\002 for a channel."
			               " Users who match an access entry on the {0} list receive the following privileges:\n"
			               "\n"),
			               source.command);

			Anope::string buf;
			for (unsigned i = 0; i < permissions[source.command].size(); ++i)
				buf += "," + permissions[source.command][i];

			source.Reply(buf);

			CommandInfo *help = source.service->FindCommand("generic/help");
			if (help)
			{
				source.Reply(_("\n"
				               "The \002ADD\002 command adds \037mask\037 to the {0} list.\n"
				               "Use of this command requires the \002{change}\002 privilege on \037channel\037.\n"
				               "\002{msg}{service} {help} {command} ADD\002 for more information.\n"
				               "\n"
				               "The \002DEL\002 command removes \037mask\037 from the {0} list.\n"
				               "Use of this command requires the \002{change}\002 privilege on \037channel\037.\n"
				               "\002{msg}{service} {help} {command} DEL\002 for more information.\n"
				               "\n"
				               "The \002LIST\002 command shows the {0} list for \037channel\037.\n"
				               "Use of this commands requires the \002{list}\002 privilege on \037channel\037.\n"
				               "\002{msg}{service} {help} {command} LIST\002 for more information.\n"
				               "\n"
				               "The \002CLEAR\002 command clears the {0} list."
				               "Use of this command requires the \002{founder}\002 privilege on \037channel\037.\n"
				               "\002{msg}{service} {help} {command} CLEAR\002 for more information.)"),
				               source.command, "change"_kw = "ACCESS_CHANGE", "list"_kw = "ACCESS_LIST", "help"_kw = help->cname);

				Anope::string access_cmd, flags_cmd;
				BotInfo *access_bi, *flags_bi;
				Command::FindCommandFromService("chanserv/access", access_bi, access_cmd);
				Command::FindCommandFromService("chanserv/flags", flags_bi, access_cmd);
				if (!access_cmd.empty() || !flags_cmd.empty())
				{
					source.Reply(_("\n"
					                "Alternative methods of modifying channel access lists are available:\n"));
					if (!access_cmd.empty())
						source.Reply(_("See \002{0}{1} {2} {3}\002 for more information about the access list."), Config->StrictPrivmsg, access_bi->nick, help->cname, access_cmd);
					if (!flags_cmd.empty())
						source.Reply(_("See \002{0}{1} {2} {3}\002 for more information about the flags system."), Config->StrictPrivmsg, flags_bi->nick, help->cname, flags_cmd);
				}
			}
		}
		return true;
	}
};

class CSXOP : public Module
{
	XOPAccessProvider accessprovider;
	CommandCSXOP commandcsxop;

 public:
	CSXOP(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, accessprovider(this)
		, commandcsxop(this)
	{
		this->SetPermanent(true);

	}

	void OnReload(Configuration::Conf *conf) override
	{
		order.clear();
		permissions.clear();

		for (int i = 0; i < conf->CountBlock("privilege"); ++i)
		{
			Configuration::Block *block = conf->GetBlock("privilege", i);
			const Anope::string &pname = block->Get<const Anope::string>("name");

			ChanServ::Privilege *p = ChanServ::service ? ChanServ::service->FindPrivilege(pname) : nullptr;
			if (p == NULL)
				continue;

			const Anope::string &xop = block->Get<const Anope::string>("xop");
			if (pname.empty() || xop.empty())
				continue;

			permissions[xop].push_back(pname);
		}

		for (int i = 0; i < conf->CountBlock("command"); ++i)
		{
			Configuration::Block *block = conf->GetBlock("command", i);
			const Anope::string &cname = block->Get<const Anope::string>("name"),
				&cserv = block->Get<const Anope::string>("command");
			if (cname.empty() || cserv != "chanserv/xop")
				continue;

			order.push_back(cname);
		}
	}
};

MODULE_INIT(CSXOP)
