/* HostServ core functions
 *
 * (C) 2003-2010 Anope Team
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
	CommandHSSet() : Command("SET", 2, 2, "hostserv/set")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		Anope::string nick = params[0];
		Anope::string rawhostmask = params[1];
		Anope::string hostmask;

		Anope::string vIdent = myStrGetToken(rawhostmask, '@', 0); /* Get the first substring, @ as delimiter */
		if (!vIdent.empty())
		{
			rawhostmask = myStrGetTokenRemainder(rawhostmask, '@', 1); /* get the remaining string */
			if (rawhostmask.empty())
			{
				source.Reply(HOST_SET_SYNTAX, Config->s_HostServ.c_str());
				return MOD_CONT;
			}
			if (vIdent.length() > Config->UserLen)
			{
				source.Reply(HOST_SET_IDENTTOOLONG, Config->UserLen);
				return MOD_CONT;
			}
			else
			{
				for (Anope::string::iterator s = vIdent.begin(), s_end = vIdent.end(); s != s_end; ++s)
					if (!isvalidchar(*s))
					{
						source.Reply(HOST_SET_IDENT_ERROR);
						return MOD_CONT;
					}
			}
			if (!ircd->vident)
			{
				source.Reply(HOST_NO_VIDENT);
				return MOD_CONT;
			}
		}
		if (rawhostmask.length() < Config->HostLen)
			hostmask = rawhostmask;
		else
		{
			source.Reply(HOST_SET_TOOLONG, Config->HostLen);
			return MOD_CONT;
		}

		if (!isValidHost(hostmask, 3))
		{
			source.Reply(HOST_SET_ERROR);
			return MOD_CONT;
		}

		NickAlias *na = findnick(nick);
		if ((na = findnick(nick)))
		{
			if (na->HasFlag(NS_FORBIDDEN))
			{
				source.Reply(NICK_X_FORBIDDEN, nick.c_str());
				return MOD_CONT;
			}

			Log(LOG_ADMIN, u, this) << "to set the vhost of " << na->nick << " to " << (!vIdent.empty() ? vIdent + "@" : "") << hostmask;

			na->hostinfo.SetVhost(vIdent, hostmask, u->nick);
			FOREACH_MOD(I_OnSetVhost, OnSetVhost(na));
			if (!vIdent.empty())
				source.Reply(HOST_IDENT_SET, nick.c_str(), vIdent.c_str(), hostmask.c_str());
			else
				source.Reply(HOST_SET, nick.c_str(), hostmask.c_str());
		}
		else
			source.Reply(HOST_NOREG, nick.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(HostServ, HOST_HELP_SET);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(HostServ, u, "SET", HOST_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(HostServ, HOST_HELP_CMD_SET);
	}
};

class HSSet : public Module
{
	CommandHSSet commandhsset;

 public:
	HSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, &commandhsset);
	}
};

MODULE_INIT(HSSet)
