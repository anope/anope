/*
 * Anope IRC Services
 *
 * Copyright (C) 2016 Anope Team <team@anope.org>
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

#include "module.h"

class CommandHSInfo : public Command
{
 public:
	CommandHSInfo(Module *creator) : Command(creator, "hostserv/info", 0, 1)
	{
		this->SetDesc(_("Displays information about vhosts"));
		this->SetSyntax(_("[\037account\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		NickServ::Account *acc = source.GetAccount();

		if (!params.empty())
		{
			if (!source.HasPriv("hostserv/auspex"))
			{
				source.Reply(_("Access denied. You do not have the operator privilege \002{0}\002."), "hostserv/auspex");
				return;
			}

			NickServ::Nick *nick = NickServ::FindNick(params[0]);
			if (nick == nullptr)
			{
				source.Reply(_("\002{0}\002 isn't registered."), params[0]);
				return;
			}

			acc = nick->GetAccount();
		}

		std::vector<HostServ::VHost *> vhosts = acc->GetRefs<HostServ::VHost *>();

		if (vhosts.empty())
		{
			source.Reply(_("\002{0}\002 does not have any vhosts."), acc->GetDisplay());
			return;
		}

		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Vhost"));
		if (source.HasPriv("hostserv/auspex"))
			list.AddColumn(_("Creator"));
		list.AddColumn(_("Created"));

		for (HostServ::VHost *vhost : vhosts)
		{
			ListFormatter::ListEntry entry;

			entry["Vhost"] = vhost->Mask() + (vhost->IsDefault() ? " (default)" : "");
			entry["Creator"] = vhost->GetCreator();
			entry["Created"] = Anope::strftime(vhost->GetCreated(), NULL, true);
			list.AddEntry(entry);
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		source.Reply(_("Vhosts for \002{0}\002:"), acc->GetDisplay());
		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Displays vhost information about the given account, such as the "
		               "associated vhosts(s), creator, and time set. Services operators may "
		               "provide an account name to view the vhosts of."));

		return true;
	}
};

class HSInfo : public Module
{
	CommandHSInfo commandhsinfo;

 public:
	HSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhsinfo(this)
	{

	}
};

MODULE_INIT(HSInfo)
