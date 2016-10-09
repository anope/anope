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
	CommandHSOn(Module *creator) : Command(creator, "hostserv/on", 0, 0)
	{
		this->SetDesc(_("Activates your assigned vhost"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!IRCD->CanSetVHost)
			return; // HostServ wouldn't even be loaded at this point

		User *u = source.GetUser();
		NickServ::Nick *na = NickServ::FindNick(u->nick);
		HostServ::VHost *vhost = nullptr;

		if (na && na->GetAccount() == source.GetAccount())
			vhost = na->GetVHost();

		if (vhost == nullptr)
			vhost = NickServ::FindNick(u->Account()->GetDisplay())->GetVHost();

		if (vhost == nullptr)
		{
			source.Reply(_("There is no vhost assigned to this nickname."));
			return;
		}

		if (!vhost->GetIdent().empty())
			source.Reply(_("Your vhost of \002{0}\002@\002{1}\002 is now activated."), vhost->GetIdent(), vhost->GetHost());
		else
			source.Reply(_("Your vhost of \002{0}\002 is now activated."), vhost->GetHost());
		
		Log(LOG_COMMAND, source, this) << "to enable their vhost of " << (!vhost->GetIdent().empty() ? vhost->GetIdent() + "@" : "") << vhost->GetHost();
		IRCD->SendVhost(u, vhost->GetIdent(), vhost->GetHost());
		u->vhost = vhost->GetHost();
		if (IRCD->CanSetVIdent && !vhost->GetIdent().empty())
			u->SetVIdent(vhost->GetIdent());
		u->UpdateHost();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Activates your vhost."));
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
