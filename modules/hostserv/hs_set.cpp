/* HostServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandHSSet final
	: public Command
{
public:
	CommandHSSet(Module *creator) : Command(creator, "hostserv/set", 2, 2)
	{
		this->SetDesc(_("Set the vhost of another user"));
		this->SetSyntax(_("\037nick\037 \037hostmask\037"));
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
				source.Reply(HOST_SET_IDENT_ERROR);
				return;
			}
		}

		if (host.length() > IRCD->MaxHost)
		{
			source.Reply(HOST_SET_TOOLONG, IRCD->MaxHost);
			return;
		}

		if (!IRCD->IsHostValid(host))
		{
			source.Reply(HOST_SET_ERROR);
			return;
		}

		Log(LOG_ADMIN, source, this) << "to set the vhost of " << na->nick << " to " << (!user.empty() ? user + "@" : "") << host;

		na->SetVHost(user, host, source.GetNick());
		FOREACH_MOD(OnSetVHost, (na));
		source.Reply(_("VHost for \002%s\002 set to \002%s\002."), nick.c_str(), na->GetVHostMask().c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Sets the vhost for the given nick to that of the given "
			"hostmask.  If your IRCD supports vidents, then using "
			"SET <nick> <ident>@<hostmask> set idents for users as "
			"well as vhosts."
		));
		return true;
	}
};

class CommandHSSetAll final
	: public Command
{
	static void Sync(const NickAlias *na)
	{
		if (!na || !na->HasVHost())
			return;

		for (auto *nick : *na->nc->aliases)
		{
			if (nick && nick != na)
				nick->SetVHost(na->GetVHostIdent(), na->GetVHostHost(), na->GetVHostCreator());
		}
	}

public:
	CommandHSSetAll(Module *creator) : Command(creator, "hostserv/setall", 2, 2)
	{
		this->SetDesc(_("Set the vhost for all nicks in a group"));
		this->SetSyntax(_("\037nick\037 \037hostmask\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		Anope::string nick = params[0];

		NickAlias *na = NickAlias::Find(nick);
		if (na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
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
				source.Reply(HOST_SET_IDENT_ERROR);
				return;
			}
		}

		if (host.length() > IRCD->MaxHost)
		{
			source.Reply(HOST_SET_TOOLONG, IRCD->MaxHost);
			return;
		}

		if (!IRCD->IsHostValid(host))
		{
			source.Reply(HOST_SET_ERROR);
			return;
		}

		Log(LOG_ADMIN, source, this) << "to set the vhost of " << na->nick << " to " << (!user.empty() ? user + "@" : "") << host;

		na->SetVHost(user, host, source.GetNick());
		this->Sync(na);
		FOREACH_MOD(OnSetVHost, (na));
		source.Reply(_("VHost for group \002%s\002 set to \002%s\002."), nick.c_str(), na->GetVHostMask().c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Sets the vhost for all nicks in the same group as that "
			"of the given nick.  If your IRCD supports vidents, then "
			"using SETALL <nick> <ident>@<hostmask> will set idents "
			"for users as well as vhosts."
			"\n\n"
			"* NOTE, this will not update the vhost for any nicks "
			"added to the group after this command was used."
		));
		return true;
	}
};

class HSSet final
	: public Module
{
	CommandHSSet commandhsset;
	CommandHSSetAll commandhssetall;

public:
	HSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR), commandhsset(this), commandhssetall(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSSet)
