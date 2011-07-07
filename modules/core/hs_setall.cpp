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
#include "hostserv.h"

class CommandHSSetAll : public Command
{
 public:
	CommandHSSetAll() : Command("SETALL", 2, 2, "hostserv/set")
	{
		this->SetDesc(_("Set the vhost for all nicks in a group"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		Anope::string nick = params[0];

		NickAlias *na = findnick(nick);
		if (na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return MOD_CONT;
		}

		Anope::string rawhost = params[1];

		Anope::string user, host;
		size_t a = rawhost.find('@');

		if (a == Anope::string::npos)
			host = rawhost;
		else
		{
			user = rawhost.substr(0, a);
			host = rawhost.substr(a + 1);
		}

		if (host.empty())
		{
			this->OnSyntaxError(source, "");
			return MOD_CONT;
		}

		if (!user.empty())
		{
			if (user.length() > Config->UserLen)
			{
				source.Reply(HOST_SET_IDENTTOOLONG, Config->UserLen);
				return MOD_CONT;
			}
			else if (!ircd->vident)
			{
				source.Reply(HOST_NO_VIDENT);
				return MOD_CONT;
			}
			for (Anope::string::iterator s = user.begin(), s_end = user.end(); s != s_end; ++s)
				if (!isvalidchar(*s))
				{
					source.Reply(HOST_SET_IDENT_ERROR);
					return MOD_CONT;
				}
		}

		if (host.length() > Config->HostLen)
		{
			source.Reply(_(HOST_SET_TOOLONG), Config->HostLen);
			return MOD_CONT;
		}

		if (!isValidHost(host, 3))
		{
			source.Reply(_(HOST_SET_ERROR));
			return MOD_CONT;
		}

		Log(LOG_ADMIN, u, this) << "to set the vhost for all nicks in group " << na->nc->display << " to " << (!user.empty() ? user + "@" : "") << host;

		na->hostinfo.SetVhost(user, host, u->nick);
		hostserv->Sync(na);
		FOREACH_MOD(I_OnSetVhost, OnSetVhost(na));
		if (!user.empty())
			source.Reply(_("vhost for group \002%s\002 set to \002%s\002@\002%s\002."), nick.c_str(), user.c_str(), host.c_str());
		else
			source.Reply(_("vhost for group \002%s\002 set to \002%s\002."), nick.c_str(), host.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002SETALL\002 \002<nick>\002 \002<hostmask>\002.\n"
				"Sets the vhost for all nicks in the same group as that\n"
				"of the given nick.  If your IRCD supports vIdents, then\n"
				"using SETALL <nick> <ident>@<hostmask> will set idents\n"
				"for users as well as vhosts.\n"
				"* NOTE, this will not update the vhost for any nicks\n"
				"added to the group after this command was used."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SETALL", _("SETALL \002<nick>\002 \002<hostmask>\002."));
	}
};

class HSSetAll : public Module
{
	CommandHSSetAll commandhssetall;

 public:
	HSSetAll(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!hostserv)
			throw ModuleException("HostServ is not loaded!");

		this->AddCommand(hostserv->Bot(), &commandhssetall);
	}
};

MODULE_INIT(HSSetAll)
