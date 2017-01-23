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

class CommandHSOn : public Command
{
 public:
	CommandHSOn(Module *creator) : Command(creator, "hostserv/on", 0, 1)
	{
		this->SetDesc(_("Activates a vhost"));
		this->SetSyntax(_("[\037vhost\037]"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!IRCD->CanSetVHost)
			return; // HostServ wouldn't even be loaded at this point

		User *u = source.GetUser();
		const Anope::string &v = params.empty() ? "" : params[0];
		HostServ::VHost *vhost;

		if (!v.empty())
		{
			vhost = HostServ::FindVHost(u->Account(), v);
			if (vhost == nullptr)
			{
				source.Reply(_("You do not have the vhost \002{0}\002."), v);
				return;
			}
		}
		else
		{
			vhost = HostServ::FindVHost(u->Account());
			if (vhost == nullptr)
			{
				source.Reply(_("You do not have any vhosts associated with your account."), vhost);
				return;
			}
		}

		source.Reply(_("Your vhost of \002{0}\002 is now activated."), vhost->Mask());
		
		logger.Command(LogType::ADMIN, source, _("{source} used {command} to enable their vhost of {0}"), vhost->Mask());
		IRCD->Send<messages::VhostSet>(u, vhost->GetIdent(), vhost->GetHost());
		u->vhost = vhost->GetHost();
		if (IRCD->CanSetVIdent && !vhost->GetIdent().empty())
			u->SetVIdent(vhost->GetIdent());
		u->UpdateHost();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Activates a vhost. If \037vhost\037 is specified, it must be a vhost assigned to your account."
		               " If \037vhost\037 is not specified, your default vhost will be activated."));
		return true;
	}
};

class HSOn : public Module
{
	CommandHSOn commandhson;

 public:
	HSOn(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhson(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSOn)
