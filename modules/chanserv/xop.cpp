/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

/* Dependencies: anope_chanserv.main */

#include "module.h"
#include "modules/chanserv.h"
#include "modules/chanserv/access.h"
#include "modules/chanserv/main/chanaccess.h"
#include "main/chanaccesstype.h"

namespace
{
	std::vector<Anope::string> order;
	std::map<Anope::string, std::vector<Anope::string> > permissions;
}

class XOPChanAccessImpl : public XOPChanAccess
{
	friend class XOPChanAccessType;

	Serialize::Storage<Anope::string> type;

 public:
	using XOPChanAccess::XOPChanAccess;

	const Anope::string GetType() override;
	void SetType(const Anope::string &) override;

	bool HasPriv(const Anope::string &priv) override
	{
		for (std::vector<Anope::string>::iterator it = std::find(order.begin(), order.end(), this->GetType()); it != order.end(); ++it)
		{
			const std::vector<Anope::string> &privs = permissions[*it];
			if (std::find(privs.begin(), privs.end(), priv) != privs.end())
				return true;
		}
		return false;
	}

	Anope::string AccessSerialize() override
	{
		return this->GetType();
	}

	void AccessUnserialize(const Anope::string &data) override
	{
		this->SetType(data);
	}

	static Anope::string DetermineLevel(ChanServ::ChanAccess *access)
	{
		if (access->GetSerializableType()->GetName() == NAME)
		{
			XOPChanAccess *xaccess = anope_dynamic_static_cast<XOPChanAccess *>(access);
			return xaccess->GetType();
		}

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

};

class XOPChanAccessType : public ChanAccessType<XOPChanAccessImpl>
{
 public:
	Serialize::Field<XOPChanAccessImpl, Anope::string> type;

	XOPChanAccessType(Module *me) : ChanAccessType<XOPChanAccessImpl>(me)
		, type(this, "type", &XOPChanAccessImpl::type)
	{
		Serialize::SetParent(XOPChanAccess::NAME, ChanServ::ChanAccess::NAME);
	}
};

const Anope::string XOPChanAccessImpl::GetType()
{
	return Get(&XOPChanAccessType::type);
}

void XOPChanAccessImpl::SetType(const Anope::string &i)
{
	Object::Set(&XOPChanAccessType::type, i);
}

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
			source.Reply(_("Sorry, channel {0} list modification is temporarily disabled."), source.GetCommand());
			return;
		}

		ChanServ::AccessGroup access = source.AccessFor(ci);
		ChanServ::ChanAccess *highest = access.Highest();

		std::vector<Anope::string>::iterator cmd_it = std::find(order.begin(), order.end(), source.GetCommand().upper()),
			access_it = highest ? std::find(order.begin(), order.end(), XOPChanAccessImpl::DetermineLevel(highest)) : order.end();

		if (!access.founder && (!access.HasPriv("ACCESS_CHANGE") || cmd_it <= access_it))
		{
			if (!source.HasPriv("chanserv/access/modify"))
			{
				source.Reply(_("Access denied. You do not have the \002{0}\002 privilege on \002{1}\002."), "ACCESS_CHANGE", ci->GetName());
				return;
			}
		}

		NickServ::Nick *na = NickServ::FindNick(mask);

		if (!na && Config->GetModule("chanserv/main")->Get<bool>("disallow_hostmask_access"))
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

		if (na)
			mask = na->GetNick();

		for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
		{
			ChanServ::ChanAccess *a = ci->GetAccess(i);

			if ((na && na->GetAccount() == a->GetAccount()) || mask.equals_ci(a->Mask()))
			{
				if ((!highest || *a >= *highest) && !access.founder && !source.HasPriv("chanserv/access/modify"))
				{
					source.Reply(_("Access denied. You do not have enough privileges on \002{0}\002 to lower the access of \002{1}\002."), ci->GetName(), a->Mask());
					return;
				}

				a->Delete();
				break;
			}
		}

		unsigned access_max = Config->GetModule("chanserv/main")->Get<unsigned>("accessmax", "1024");
		if (access_max && ci->GetAccessCount() >= access_max)
		{
			source.Reply(_("Sorry, you can only have %d access entries on a channel, including access entries from other channels."), access_max);
			return;
		}

		XOPChanAccess *acc = Serialize::New<XOPChanAccess *>();
		if (na)
			acc->SetAccount(na->GetAccount());
		acc->SetChannel(ci);
		acc->SetMask(mask);
		acc->SetCreator(source.GetNick());
		acc->SetType(source.GetCommand().upper());
		acc->SetLastSeen(0);
		acc->SetCreated(Anope::CurTime);

		logger.Command(source, ci, _("{source} used {command} on {channel} to add {0}"), mask);

		EventManager::Get()->Dispatch(&Event::AccessAdd::OnAccessAdd, ci, source, acc);
		source.Reply(_("\002{0}\002 added to {1} {2} list."), acc->Mask(), ci->GetName(), source.GetCommand());
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
			source.Reply(_("Sorry, channel {0} list modification is temporarily disabled."), source.GetCommand());
			return;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(_("{0} {1} list is empty."), ci->GetName(), source.GetCommand());
			return;
		}

		ChanServ::AccessGroup access = source.AccessFor(ci);
		ChanServ::ChanAccess *highest = access.Highest();

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

		std::vector<Anope::string>::iterator cmd_it = std::find(order.begin(), order.end(), source.GetCommand().upper()),
			access_it = highest ? std::find(order.begin(), order.end(), XOPChanAccessImpl::DetermineLevel(highest)) : order.end();

		if (!mask.equals_ci(nc->GetDisplay()) && !access.founder && (!access.HasPriv("ACCESS_CHANGE") || cmd_it <= access_it))
		{
			if (!source.HasPriv("chanserv/access/modify"))
			{
				source.Reply(_("Access denied. You do not have the \002{0}\002 privilege on \002{1}\002."), "ACCESS_CHANGE", ci->GetName());
				return;
			}
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			unsigned int deleted = 0;
			Anope::string nicks;

			NumberList(mask, true,
				[&](unsigned int number)
				{
					if (!number || number > ci->GetAccessCount())
						return;

					ChanServ::ChanAccess *caccess = ci->GetAccess(number - 1);

					if (caccess->GetSerializableType()->GetName() != "XOPChanAccess" || source.GetCommand().upper() != caccess->AccessSerialize())
						return;

					++deleted;
					if (!nicks.empty())
						nicks += ", " + caccess->Mask();
					else
						nicks = caccess->Mask();

					EventManager::Get()->Dispatch(&Event::AccessDel::OnAccessDel, ci, source, caccess);
					caccess->Delete();
				},
				[&]()
				{
					if (!deleted)
						 source.Reply(_("No matching entries on {0} {1} list."), ci->GetName(), source.GetCommand());
					else
					{
						logger.Command(source, ci, _("{source} used {command} on {channel} to delete {0}"), nicks);

						if (deleted == 1)
							source.Reply(_("Deleted one entry from {0} {1} list."), ci->GetName(), source.GetCommand());
						else
							source.Reply(_("Deleted {0} entries from {1} {2} list."), deleted, ci->GetName(), source.GetCommand());
					}
				});
		}
		else
		{
			for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
			{
				ChanServ::ChanAccess *a = ci->GetAccess(i);

				if (a->GetSerializableType()->GetName() != "XOPChanAccess" || source.GetCommand().upper() != a->AccessSerialize())
					continue;

				if (a->Mask().equals_ci(mask))
				{
					logger.Command(source, ci, _("{source} used {command} on {channel} to delete {0}"), a->GetMask());

					source.Reply(_("\002{0}\002 deleted from {1} {2} list."), a->Mask(), ci->GetName(), source.GetCommand());

					EventManager::Get()->Dispatch(&Event::AccessDel::OnAccessDel, ci, source, a);
					a->Delete();

					return;
				}
			}

			source.Reply(_("\002{0}\002 not found on {1} {2} list."), mask, ci->GetName(), source.GetCommand());
		}
	}

	void DoList(CommandSource &source, ChanServ::Channel *ci, const std::vector<Anope::string> &params)
	{

		const Anope::string &nick = params.size() > 2 ? params[2] : "";

		ChanServ::AccessGroup access = source.AccessFor(ci);

		if (!access.HasPriv("ACCESS_LIST") && !source.HasPriv("chanserv/access/list"))
		{
			source.Reply(_("Access denied. You do not have the \002{0}\002 privilege on \002{1}\002."), "ACCESS_LIST", ci->GetName());
			return;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(_("{0} {1} list is empty."), ci->GetName(), source.GetCommand());
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Mask"));

		if (!nick.empty() && nick.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			NumberList(nick, false,
				[&](unsigned int number)
				{
					if (!number || number > ci->GetAccessCount())
						return;

					ChanServ::ChanAccess *a = ci->GetAccess(number - 1);

					if (a->GetSerializableType()->GetName() != "XOPChanAccess" || source.GetCommand().upper() != a->AccessSerialize())
						return;

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(number);
					entry["Mask"] = a->Mask();
					list.AddEntry(entry);
				},
				[]{});
		}
		else
		{
			for (unsigned i = 0, end = ci->GetAccessCount(); i < end; ++i)
			{
				ChanServ::ChanAccess *a = ci->GetAccess(i);

				if (a->GetSerializableType()->GetName() != "XOPChanAccess" || source.GetCommand().upper() != a->AccessSerialize())
					continue;
				if (!nick.empty() && !Anope::Match(a->Mask(), nick))
					continue;

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				entry["Mask"] = a->Mask();
				list.AddEntry(entry);
			}
		}

		if (list.IsEmpty())
		{
			source.Reply(_("No matching entries on {0} access list."), ci->GetName());
		}
		else
		{
			std::vector<Anope::string> replies;
			list.Process(replies);

			source.Reply(_("{0} list for {1}"), source.GetCommand(), ci->GetName());
			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
	}

	void DoClear(CommandSource &source, ChanServ::Channel *ci)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, channel {0} list modification is temporarily disabled."), source.GetCommand());
			return;
		}

		if (!ci->GetAccessCount())
		{
			source.Reply(_("{0} {1} list is empty."), ci->GetName(), source.GetCommand());
			return;
		}

		if (!source.AccessFor(ci).HasPriv("FOUNDER") && !source.HasOverridePriv("chanserv/access/modify"))
		{
			source.Reply(_("Access denied. You do not have the \002{0}\002 privilege on \002{1}\002."), "FOUNDER", ci->GetName());
			return;
		}

		logger.Command(source, ci, _("{source} used {command} on {channel} to clear the access list"));

		for (unsigned i = ci->GetAccessCount(); i > 0; --i)
		{
			ChanServ::ChanAccess *access = ci->GetAccess(i - 1);

			if (access->GetSerializableType()->GetName() != "XOPChanAccess" || source.GetCommand().upper() != access->AccessSerialize())
				continue;

			access->Delete();
		}

		EventManager::Get()->Dispatch(&Event::AccessClear::OnAccessClear, ci, source);

		source.Reply(_("Channel {0} {1} list has been cleared."), ci->GetName(), source.GetCommand());
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
		return Anope::Format(Language::Translate(source.GetAccount(), _("Modify the list of {0} users")), source.GetCommand().upper());
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
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
			               source.GetCommand());

			source.Reply(_("Use of this command requires the \002{0}\002 privilege on \037channel\037.\n"
			               "Example:\n"
			               "         {1} #anope ADD Adam"
			               "         Adds \"Adam\" to the access list of \"#anope\" with the privilege set {1}"),
			               "ACCESS_CHANGE", source.GetCommand());
		}
		else if (subcommand.equals_ci("DEL"))
			source.Reply(_("The \002{0} DEL\002 command removes the given \037mask\037 from the {0} list of \037channel\037."
			               " If a list of entry numbers is given, those entries are deleted.\n"
			               "\n"
			               "Use of this command requires the \002{1}\002 privilege on \037channel\037.\n"
			               "Example:\n"
			               "         {0} #anope DEL Cronus"
			               "         Removes \"Cronus\" from the {0} list of \"#anope\""),
			               source.GetCommand(), "ACCESS_CHANGE");
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
			               source.GetCommand(), "ACCESS_LIST");
		else if (subcommand.equals_ci("CLEAR"))
			source.Reply(_("The \002{0} CLEAR\002 command clears the {0} list of \037channel\037."
			               "\n"
			               "Use of this command requires the \002{1}\002 privilege on \037channel\037."),
			               source.GetCommand(), "FOUNDER");

		else
		{
			source.Reply(_("Maintains the \002{0} list\002 for a channel."
			               " Users who match an access entry on the {0} list receive the following privileges:\n"
			               "\n"),
			               source.GetCommand());

			Anope::string buf;
			for (unsigned i = 0; i < permissions[source.GetCommand()].size(); ++i)
				buf += "," + permissions[source.GetCommand()][i];

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
				               source.GetCommand(), "change"_kw = "ACCESS_CHANGE", "list"_kw = "ACCESS_LIST", "help"_kw = help->cname);

				Anope::string access_cmd, flags_cmd;
				ServiceBot *access_bi, *flags_bi;
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
	CommandCSXOP commandcsxop;
	XOPChanAccessType xopaccesstype;

 public:
	CSXOP(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsxop(this)
		, xopaccesstype(this)
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
			const Anope::string &pname = block->Get<Anope::string>("name");

			ChanServ::Privilege *p = ChanServ::service ? ChanServ::service->FindPrivilege(pname) : nullptr;
			if (p == NULL)
				continue;

			const Anope::string &xop = block->Get<Anope::string>("xop");
			if (pname.empty() || xop.empty())
				continue;

			permissions[xop].push_back(pname);
		}

		for (int i = 0; i < conf->CountBlock("command"); ++i)
		{
			Configuration::Block *block = conf->GetBlock("command", i);
			const Anope::string &cname = block->Get<Anope::string>("name"),
				&cserv = block->Get<Anope::string>("command");
			if (cname.empty() || cserv != "chanserv/xop")
				continue;

			order.push_back(cname);
		}
	}
};

template<> void ModuleInfo<CSXOP>(ModuleDef *def)
{
	def->Depends("chanserv");
}

MODULE_INIT(CSXOP)
