/* HostServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

void myHostServHelp(User *u);

class CommandHSSet : public Command
{
 public:
	CommandHSSet() : Command("SET", 2, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		if (!u->nc->HasPriv("hostserv/set"))
		{
			notice_lang(s_HostServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		const char *nick = params[0].c_str();
		const char *rawhostmask = params[1].c_str();
		char *hostmask = new char[HOSTMAX];

		NickAlias *na;
		int32 tmp_time;
		char *s;

		char *vIdent = NULL;

		vIdent = myStrGetOnlyToken(rawhostmask, '@', 0); /* Get the first substring, @ as delimiter */
		if (vIdent)
		{
			rawhostmask = myStrGetTokenRemainder(rawhostmask, '@', 1); /* get the remaining string */
			if (!rawhostmask)
			{
				notice_lang(s_HostServ, u, HOST_SET_SYNTAX, s_HostServ);
				delete [] vIdent;
				delete [] hostmask;
				return MOD_CONT;
			}
			if (strlen(vIdent) > USERMAX - 1)
			{
				notice_lang(s_HostServ, u, HOST_SET_IDENTTOOLONG, USERMAX);
				delete [] vIdent;
				delete [] rawhostmask;
				delete [] hostmask;
				return MOD_CONT;
			}
			else
			{
				for (s = vIdent; *s; ++s)
				{
					if (!isvalidchar(*s))
					{
						notice_lang(s_HostServ, u, HOST_SET_IDENT_ERROR);
						delete [] vIdent;
						delete [] rawhostmask;
						delete [] hostmask;
						return MOD_CONT;
					}
				}
			}
			if (!ircd->vident)
			{
				notice_lang(s_HostServ, u, HOST_NO_VIDENT);
				delete [] vIdent;
				delete [] rawhostmask;
				delete [] hostmask;
				return MOD_CONT;
			}
		}
		if (strlen(rawhostmask) < HOSTMAX - 1)
			snprintf(hostmask, HOSTMAX - 1, "%s", rawhostmask);
		else
		{
			notice_lang(s_HostServ, u, HOST_SET_TOOLONG, HOSTMAX);
			if (vIdent)
			{
				delete [] vIdent;
				delete [] rawhostmask;
			}
			delete [] hostmask;
			return MOD_CONT;
		}

		if (!isValidHost(hostmask, 3))
		{
			notice_lang(s_HostServ, u, HOST_SET_ERROR);
			if (vIdent)
			{
				delete [] vIdent;
				delete [] rawhostmask;
			}
			delete [] hostmask;
			return MOD_CONT;
		}


		tmp_time = time(NULL);

		if ((na = findnick(nick)))
		{
			if (na->status & NS_FORBIDDEN)
			{
				notice_lang(s_HostServ, u, NICK_X_FORBIDDEN, nick);
				if (vIdent)
				{
					delete [] vIdent;
					delete [] rawhostmask;
				}
				delete [] hostmask;
				return MOD_CONT;
			}
			if (vIdent && ircd->vident)
				alog("vHost for user \002%s\002 set to \002%s@%s\002 by oper \002%s\002", nick, vIdent, hostmask, u->nick);
			else
				alog("vHost for user \002%s\002 set to \002%s\002 by oper \002%s\002", nick, hostmask, u->nick);
			addHostCore(nick, vIdent, hostmask, u->nick, tmp_time);
			if (vIdent)
				notice_lang(s_HostServ, u, HOST_IDENT_SET, nick, vIdent, hostmask);
			else
				notice_lang(s_HostServ, u, HOST_SET, nick, hostmask);
		}
		else
			notice_lang(s_HostServ, u, HOST_NOREG, nick);
		delete [] hostmask;
		if (vIdent)
		{
			delete [] vIdent;
			delete [] rawhostmask;
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_HostServ, u, HOST_HELP_SET);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_HostServ, u, "SET", HOST_SET_SYNTAX);
	}
};

class HSSet : public Module
{
 public:
	HSSet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(HOSTSERV, new CommandHSSet(), MOD_UNIQUE);

		this->SetHostHelp(myHostServHelp);
	}
};

/**
 * Add the help response to anopes /hs help output.
 * @param u The user who is requesting help
 **/
void myHostServHelp(User *u)
{
	notice_lang(s_HostServ, u, HOST_HELP_CMD_SET);
}

MODULE_INIT("hs_set", HSSet)
