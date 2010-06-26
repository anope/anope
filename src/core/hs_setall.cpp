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

class CommandHSSetAll : public Command
{
 public:
	CommandHSSetAll() : Command("SETALL", 2, 2, "hostserv/set")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		const char *rawhostmask = params[1].c_str();
		char *hostmask = new char[Config.HostLen];

		NickAlias *na;
		int32 tmp_time;
		char *s;

		char *vIdent = NULL;

		if (!(na = findnick(nick)))
		{
			delete [] hostmask;
			notice_lang(Config.s_HostServ, u, HOST_NOREG, nick);
			return MOD_CONT;
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			delete [] hostmask;
			notice_lang(Config.s_HostServ, u, NICK_X_FORBIDDEN, nick);
			return MOD_CONT;
		}

		vIdent = myStrGetOnlyToken(rawhostmask, '@', 0); /* Get the first substring, @ as delimiter */
		if (vIdent)
		{
			rawhostmask = myStrGetTokenRemainder(rawhostmask, '@', 1); /* get the remaining string */
			if (!rawhostmask)
			{
				notice_lang(Config.s_HostServ, u, HOST_SETALL_SYNTAX, Config.s_HostServ);
				delete [] vIdent;
				delete [] hostmask;
				return MOD_CONT;
			}
			if (strlen(vIdent) > Config.UserLen)
			{
				notice_lang(Config.s_HostServ, u, HOST_SET_IDENTTOOLONG, Config.UserLen);
				delete [] vIdent;
				delete [] rawhostmask;
				delete [] hostmask;
				return MOD_CONT;
			}
			else
			{
				for (s = vIdent; *s; ++s)
					if (!isvalidchar(*s))
					{
						notice_lang(Config.s_HostServ, u, HOST_SET_IDENT_ERROR);
						delete [] vIdent;
						delete [] rawhostmask;
						delete [] hostmask;
						return MOD_CONT;
					}
			}
			if (!ircd->vident)
			{
				notice_lang(Config.s_HostServ, u, HOST_NO_VIDENT);
				delete [] vIdent;
				delete [] rawhostmask;
				delete [] hostmask;
				return MOD_CONT;
			}
		}

		if (strlen(rawhostmask) < Config.HostLen)
			snprintf(hostmask, Config.HostLen, "%s", rawhostmask);
		else
		{
			notice_lang(Config.s_HostServ, u, HOST_SET_TOOLONG, Config.HostLen);
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
			notice_lang(Config.s_HostServ, u, HOST_SET_ERROR);
			if (vIdent)
			{
				delete [] vIdent;
				delete [] rawhostmask;
			}
			delete [] hostmask;
			return MOD_CONT;
		}

		tmp_time = time(NULL);

		Alog() << "vHost for all nicks in group \002" << nick << "\002 set to \002" << (vIdent && ircd->vident ? vIdent : "") << (vIdent && ircd->vident ? "@" : "") << hostmask << " \002 by oper \002" << u->nick << "\002";

		na->hostinfo.SetVhost(vIdent ? vIdent : "", hostmask, u->nick);
		HostServSyncVhosts(na);
		FOREACH_MOD(I_OnSetVhost, OnSetVhost(na));
		if (vIdent)
			notice_lang(Config.s_HostServ, u, HOST_IDENT_SETALL, nick, vIdent, hostmask);
		else
			notice_lang(Config.s_HostServ, u, HOST_SETALL, nick, hostmask);
		if (vIdent)
		{
			delete [] vIdent;
			delete [] rawhostmask;
		}
		delete [] hostmask;
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_HostServ, u, HOST_HELP_SETALL);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_HostServ, u, "SETALL", HOST_SETALL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_HostServ, u, HOST_HELP_CMD_SETALL);
	}
};

class HSSetAll : public Module
{
 public:
	HSSetAll(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, new CommandHSSetAll());
	}
};

MODULE_INIT(HSSetAll)
