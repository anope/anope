/* HostServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandHSSet : public Command
{
 public:
	CommandHSSet(Module *creator) : Command(creator, "hostserv/set", 2, 2)
	{
		this->SetDesc(_("Set the vhost of another user"));
		this->SetSyntax(_("\037nick\037 \037hostmask\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = params[0];

		NickAlias *na = findnick(nick);
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
			if (user.length() > Config->UserLen)
			{
				source.Reply(HOST_SET_IDENTTOOLONG, Config->UserLen);
				return;
			}
			else if (!ircd->vident)
			{
				source.Reply(HOST_NO_VIDENT);
				return;
			}
			for (Anope::string::iterator s = user.begin(), s_end = user.end(); s != s_end; ++s)
				if (!isvalidchar(*s))
				{
					source.Reply(HOST_SET_IDENT_ERROR);
					return;
				}
		}

		if (host.length() > Config->HostLen)
		{
			source.Reply(HOST_SET_TOOLONG, Config->HostLen);
			return;
		}

		if (!isValidHost(host, 3))
		{
			source.Reply(HOST_SET_ERROR);
			return;
		}

		Log(LOG_ADMIN, u, this) << "to set the vhost of " << na->nick << " to " << (!user.empty() ? user + "@" : "") << host;

		na->hostinfo.SetVhost(user, host, u->nick);
		FOREACH_MOD(I_OnSetVhost, OnSetVhost(na));
		if (!user.empty())
			source.Reply(_("VHost for \002%s\002 set to \002%s\002@\002%s\002."), nick.c_str(), user.c_str(), host.c_str());
		else
			source.Reply(_("VHost for \002%s\002 set to \002%s\002."), nick.c_str(), host.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets the vhost for the given nick to that of the given\n"
				"hostmask.  If your IRCD supports vIdents, then using\n"
				"SET <nick> <ident>@<hostmask> set idents for users as \n"
				"well as vhosts."));
		return true;
	}
};

class CommandHSSetAll : public Command
{
	void Sync(NickAlias *na)
	{
		if (!na || !na->hostinfo.HasVhost())
			return;
	
		for (std::list<NickAlias *>::iterator it = na->nc->aliases.begin(), it_end = na->nc->aliases.end(); it != it_end; ++it)
		{
			NickAlias *nick = *it;
			nick->hostinfo.SetVhost(na->hostinfo.GetIdent(), na->hostinfo.GetHost(), na->hostinfo.GetCreator());
		}
	}

 public:
	CommandHSSetAll(Module *creator) : Command(creator, "hostserv/setall", 2, 2)
	{
		this->SetDesc(_("Set the vhost for all nicks in a group"));
		this->SetSyntax(_("\037nick\037 \037hostmask>\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		Anope::string nick = params[0];

		NickAlias *na = findnick(nick);
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
			if (user.length() > Config->UserLen)
			{
				source.Reply(HOST_SET_IDENTTOOLONG, Config->UserLen);
				return;
			}
			else if (!ircd->vident)
			{
				source.Reply(HOST_NO_VIDENT);
				return;
			}
			for (Anope::string::iterator s = user.begin(), s_end = user.end(); s != s_end; ++s)
				if (!isvalidchar(*s))
				{
					source.Reply(HOST_SET_IDENT_ERROR);
					return;
				}
		}

		if (host.length() > Config->HostLen)
		{
			source.Reply(HOST_SET_TOOLONG, Config->HostLen);
			return;
		}

		if (!isValidHost(host, 3))
		{
			source.Reply(HOST_SET_ERROR);
			return;
		}

		Log(LOG_ADMIN, u, this) << "to set the vhost of " << na->nick << " to " << (!user.empty() ? user + "@" : "") << host;

		na->hostinfo.SetVhost(user, host, u->nick);
		this->Sync(na);
		FOREACH_MOD(I_OnSetVhost, OnSetVhost(na));
		if (!user.empty())
			source.Reply(_("VHost for group \002%s\002 set to \002%s\002@\002%s\002."), nick.c_str(), user.c_str(), host.c_str());
		else
			source.Reply(_("VHost for group \002%s\002 set to \002%s\002."), nick.c_str(), host.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets the vhost for all nicks in the same group as that\n"
				"of the given nick.  If your IRCD supports vIdents, then\n"
				"using SETALL <nick> <ident>@<hostmask> will set idents\n"
				"for users as well as vhosts.\n"
				"* NOTE, this will not update the vhost for any nicks\n"
				"added to the group after this command was used."));
		return true;
	}
};

class HSSet : public Module
{
	CommandHSSet commandhsset;
	CommandHSSetAll commandhssetall;

 public:
	HSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), commandhsset(this), commandhssetall(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandhsset);
		ModuleManager::RegisterService(&commandhssetall);
	}
};

MODULE_INIT(HSSet)
