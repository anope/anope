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

class CommandHSSetAll : public Command
{
 public:
	CommandHSSetAll() : Command("SETALL", 2, 2, "hostserv/set")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		
		const Anope::string &nick = params[0];
		Anope::string rawhostmask = params[1];

		int32 tmp_time;

		NickAlias *na = findnick(nick);
		if (!na)
		{
			source.Reply(HOST_NOREG, nick.c_str());
			return MOD_CONT;
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			source.Reply(NICK_X_FORBIDDEN, nick.c_str());
			return MOD_CONT;
		}

		Anope::string vIdent = myStrGetToken(rawhostmask, '@', 0); /* Get the first substring, @ as delimiter */
		if (!vIdent.empty())
		{
			rawhostmask = myStrGetTokenRemainder(rawhostmask, '@', 1); /* get the remaining string */
			if (rawhostmask.empty())
			{
				source.Reply(HOST_SETALL_SYNTAX, Config->s_HostServ.c_str());
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

		Anope::string hostmask;
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

		tmp_time = Anope::CurTime;

		Log(LOG_ADMIN, u, this) << "to set the vhost for all nicks in group " << na->nc->display << " to " << (!vIdent.empty() ? vIdent + "@" : "") << hostmask;

		na->hostinfo.SetVhost(vIdent, hostmask, u->nick);
		HostServSyncVhosts(na);
		FOREACH_MOD(I_OnSetVhost, OnSetVhost(na));
		if (!vIdent.empty())
			source.Reply(HOST_IDENT_SETALL, nick.c_str(), vIdent.c_str(), hostmask.c_str());
		else
			source.Reply(HOST_SETALL, nick.c_str(), hostmask.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(HOST_HELP_SETALL);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SETALL", HOST_SETALL_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(HOST_HELP_CMD_SETALL);
	}
};

class HSSetAll : public Module
{
	CommandHSSetAll commandhssetall;

 public:
	HSSetAll(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, &commandhssetall);
	}
};

MODULE_INIT(HSSetAll)
