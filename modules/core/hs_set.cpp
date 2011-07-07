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

class CommandHSSet : public Command
{
 public:
	CommandHSSet() : Command("SET", 2, 2, "hostserv/set")
	{
		this->SetDesc(_("Set the vhost of another user"));
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

		Log(LOG_ADMIN, u, this) << "to set the vhost of " << na->nick << " to " << (!user.empty() ? user + "@" : "") << host;

		na->hostinfo.SetVhost(user, host, u->nick);
		FOREACH_MOD(I_OnSetVhost, OnSetVhost(na));
		if (!user.empty())
			source.Reply(_("vhost for \002%s\002 set to \002%s\002@\002%s\002."), nick.c_str(), user.c_str(), host.c_str());
		else
			source.Reply(_("vhost for \002%s\002 set to \002%s\002."), nick.c_str(), host.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002SET\002 \002<nick>\002 \002<hostmask>\002.\n"
				"Sets the vhost for the given nick to that of the given\n"
				"hostmask.  If your IRCD supports vIdents, then using\n"
				"SET <nick> <ident>@<hostmask> set idents for users as \n"
				"well as vhosts."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SET", _("SET \002<nick>\002 \002<hostmask>\002."));
	}
};

class HSSet : public Module
{
	CommandHSSet commandhsset;

 public:
	HSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!hostserv)
			throw ModuleException("HostServ is not loaded!");

		this->AddCommand(hostserv->Bot(), &commandhsset);
	}
};

MODULE_INIT(HSSet)
