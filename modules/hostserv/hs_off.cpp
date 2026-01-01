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

class CommandHSOff final
	: public Command
{
public:
	CommandHSOff(Module *creator) : Command(creator, "hostserv/off", 0, 0)
	{
		this->SetDesc(_("Deactivates your assigned vhost"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();

		const NickAlias *na = NickAlias::Find(u->nick);
		if (!na || na->nc != u->Account() || !na->HasVHost())
			na = u->AccountNick();

		if (!na || !na->HasVHost())
			source.Reply(HOST_NOT_ASSIGNED);
		else
		{
			u->vhost.clear();
			IRCD->SendVHostDel(u);
			u->UpdateHost();
			Log(LOG_COMMAND, source, this) << "to disable their vhost";
			source.Reply(_("Your vhost was removed and the normal cloaking restored."));
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Deactivates the vhost currently assigned to the nick in use. "
			"When you use this command any user who performs a /whois "
			"on you will see your real host/IP address."
		));
		return true;
	}
};

class HSOff final
	: public Module
{
	CommandHSOff commandhsoff;

public:
	HSOff(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandhsoff(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSOff)
