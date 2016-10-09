/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
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
#include "modules/hostserv/del.h"

class CommandHSDel : public Command
{
 public:
	CommandHSDel(Module *creator) : Command(creator, "hostserv/del", 1, 1)
	{
		this->SetDesc(_("Delete the vhost of another user"));
		this->SetSyntax(_("\037user\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		const Anope::string &nick = params[0];
		NickServ::Nick *na = NickServ::FindNick(nick);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		HostServ::VHost *vhost = na->GetVHost();
		if (vhost == nullptr)
		{
			source.Reply(_("\002{0}\002 doesn't have a vhost."), na->GetNick());
			return;
		}

		Log(LOG_ADMIN, source, this) << "for user " << na->GetNick();
		EventManager::Get()->Dispatch(&Event::DeleteVhost::OnDeleteVhost, na);
		vhost->Delete();
		source.Reply(_("Vhost for \002{0}\002 has been removed."), na->GetNick());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Removes the vhost of \037user\037."));
		return true;
	}
};

class CommandHSDelAll : public Command
{
 public:
	CommandHSDelAll(Module *creator) : Command(creator, "hostserv/delall", 1, 1)
	{
		this->SetDesc(_("Delete the vhost for all nicks in a group"));
		this->SetSyntax(_("\037group\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		const Anope::string &nick = params[0];
		NickServ::Nick *na = NickServ::FindNick(nick);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		EventManager::Get()->Dispatch(&Event::DeleteVhost::OnDeleteVhost, na);

		NickServ::Account *nc = na->GetAccount();
		for (NickServ::Nick *na2 : nc->GetRefs<NickServ::Nick *>())
		{
			HostServ::VHost *vhost = na2->GetVHost();
			if (vhost != nullptr)
			{
				vhost->Delete();
			}
		}

		Log(LOG_ADMIN, source, this) << "for all nicks in group " << nc->GetDisplay();
		source.Reply(_("Vhosts for group \002{0}\002 have been removed."), nc->GetDisplay());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Removes the vhost of all nicks in the group \037group\037."));
		return true;
	}
};

class HSDel : public Module
{
	CommandHSDel commandhsdel;
	CommandHSDelAll commandhsdelall;

 public:
	HSDel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhsdel(this)
		, commandhsdelall(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSDel)
