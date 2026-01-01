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

class CommandHSDel final
	: public Command
{
public:
	CommandHSDel(Module *creator) : Command(creator, "hostserv/del", 1, 1)
	{
		this->SetDesc(_("Delete the vhost of another user"));
		this->SetSyntax(_("\037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const Anope::string &nick = params[0];
		NickAlias *na = NickAlias::Find(nick);
		if (na)
		{
			Log(LOG_ADMIN, source, this) << "for user " << na->nick;
			FOREACH_MOD(OnDeleteVHost, (na));
			na->RemoveVHost();
			source.Reply(_("VHost for \002%s\002 removed."), nick.c_str());
		}
		else
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Deletes the vhost assigned to the given nick from the database."));
		return true;
	}
};

class CommandHSDelAll final
	: public Command
{
public:
	CommandHSDelAll(Module *creator) : Command(creator, "hostserv/delall", 1, 1)
	{
		this->SetDesc(_("Deletes the vhost for all nicks in an account"));
		this->SetSyntax(_("\037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const Anope::string &nick = params[0];
		NickAlias *na = NickAlias::Find(nick);
		if (na)
		{
			FOREACH_MOD(OnDeleteVHost, (na));
			const NickCore *nc = na->nc;
			for (auto *alias : *nc->aliases)
			{
				na = alias;
				na->RemoveVHost();
			}
			Log(LOG_ADMIN, source, this) << "for all nicks in account " << nc->display;
			source.Reply(_("VHosts for account \002%s\002 have been removed."), nc->display.c_str());
		}
		else
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Deletes the vhost for all nicks in the same account as that of the given nick."));
		return true;
	}
};

class HSDel final
	: public Module
{
	CommandHSDel commandhsdel;
	CommandHSDelAll commandhsdelall;

public:
	HSDel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandhsdel(this), commandhsdelall(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSDel)
