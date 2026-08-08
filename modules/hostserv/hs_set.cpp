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

class CommandHSSet final
	: public Command
{
public:
	CommandHSSet(Module *creator) : Command(creator, "hostserv/set", 2, 3)
	{
		this->SetDesc(_("Set the vhost of a nick"));
		this->SetSyntax(_("\037nick\037 \037hostmask\037 [SYNC]"));
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
		if (na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
		}

		auto sync = false;
		if (params.size() > 2)
		{
			if (!params[2].equals_ci("SYNC"))
			{
				this->OnSyntaxError(source, "");
				return;
			}

			na = na->nc->na;
			sync = true;
		}

		Anope::string rawhostmask = params[1];

		Anope::string user, host;
		size_t a = rawhostmask.find('@');

		if (a == Anope::string::npos)
			host = rawhostmask;
		else
		{
			user = rawhostmask.substr(0, a);
			host = rawhostmask.substr(a + 1);
		}

		if (host.empty())
		{
			this->OnSyntaxError(source, "");
			return;
		}

		if (!user.empty())
		{
			if (!IRCD->CanSetVIdent)
			{
				source.Reply(HOST_NO_VIDENT);
				return;
			}
			else if (!IRCD->IsIdentValid(user))
			{
				source.Reply(HOST_SET_VIDENT_ERROR);
				return;
			}
		}

		if (host.length() > IRCD->MaxHost)
		{
			source.Reply(HOST_SET_VHOST_TOO_LONG, IRCD->MaxHost);
			return;
		}

		if (!IRCD->IsHostValid(host))
		{
			source.Reply(HOST_SET_VHOST_ERROR);
			return;
		}

		na->SetVHost(user, host, source.GetNick());
		FOREACH_MOD(OnSetVHost, (na));

		if (sync)
		{
			for (auto *nick : *na->nc->aliases)
			{
				if (nick && nick != na)
					na->SetVHost(user, host, source.GetNick());
			}

			source.Reply(_("VHost for account \002%s\002 set to \002%s\002."), na->nick.c_str(), na->GetVHostMask().c_str());
			Log(LOG_ADMIN, source, this) << "to set the vhost of account " << na->nick << " to " << na->GetVHostMask();
		}
		else
		{
			source.Reply(_("VHost for \002%s\002 set to \002%s\002."), na->nick.c_str(), na->GetVHostMask().c_str());
			Log(LOG_ADMIN, source, this) << "to set the vhost of " << na->nick << " to " << na->GetVHostMask();
		}

	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Sets the vhost for the given nick to the given host. If your IRCd supports vidents "
			"then you can also specify a user@host mask."
			"\n\n"
			"If SYNC is specified then the vhost will be synchronised to all nicks currently "
			"grouped to the account."
		));
		return true;
	}
};

class HSSet final
	: public Module
{
private:
	CommandHSSet commandhsset;

public:
	HSSet(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandhsset(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSSet)
