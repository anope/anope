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

class CommandHSOn final
	: public Command
{
public:
	CommandHSOn(Module *creator) : Command(creator, "hostserv/on", 0, 0)
	{
		this->SetDesc(_("Activate your assigned vhost"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!IRCD->CanSetVHost)
			return; // HostServ wouldn't even be loaded at this point

		User *u = source.GetUser();
		const NickAlias *na = NickAlias::Find(u->nick);
		if (!na || na->nc != u->Account() || !na->HasVHost())
			na = u->AccountNick();
		if (na && u->Account() == na->nc && na->HasVHost())
		{
			source.Reply(_("Your vhost of \002%s\002 is now activated."), na->GetVHostMask().c_str());
			Log(LOG_COMMAND, source, this) << "to enable their vhost of " << na->GetVHostMask();
			IRCD->SendVHost(u, na->GetVHostIdent(), na->GetVHostHost());
			u->vhost = na->GetVHostHost();
			if (IRCD->CanSetVIdent && !na->GetVHostIdent().empty())
				u->SetVIdent(na->GetVHostIdent());
			u->UpdateHost();
		}
		else
			source.Reply(HOST_NOT_ASSIGNED);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Activates the vhost currently assigned to the nick in use. "
			"When you use this command any user who performs a /whois "
			"on you will see the vhost instead of your real host/IP address."
		));
		return true;
	}
};

class HSOn final
	: public Module
{
	CommandHSOn commandhson;

public:
	HSOn(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandhson(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSOn)
