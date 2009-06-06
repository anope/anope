/* NickServ core functions
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

class CommandNSSuspend : public Command
{
 public:
	CommandNSSuspend() : Command("SUSPEND", 2, 2, "nickserv/suspend")
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		NickAlias *na, *na2;
		User *u2;
		const char *nick = params[0].c_str();
		const char *reason = params[1].c_str();
		int i;

		if (readonly)
		{
			notice_lang(s_NickServ, u, READ_ONLY_MODE);
			return MOD_CONT;
		}

		if (!(na = findnick(nick)))
		{
			notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
			return MOD_CONT;
		}

		if (na->status & NS_FORBIDDEN)
		{
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
			return MOD_CONT;
		}

		if (NSSecureAdmins && na->nc->IsServicesOper())
		{
			notice_lang(s_NickServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (na)
		{
			na->nc->flags |= NI_SUSPENDED;
			na->nc->flags |= NI_SECURE;
			na->nc->flags &= ~(NI_KILLPROTECT | NI_KILL_QUICK | NI_KILL_IMMED);

			for (i = 0; i < na->nc->aliases.count; ++i)
			{
				na2 = static_cast<NickAlias *>(na->nc->aliases.list[i]);
				if (na2->nc == na->nc)
				{
					na2->status &= ~(NS_IDENTIFIED | NS_RECOGNIZED);
					na2->last_quit = sstrdup(reason);
					/* removes nicktracking */
					if ((u2 = finduser(na2->nick)))
						u2->nc = NULL;
					/* force guestnick */
					collide(na2, 0);
				}
			}

			if (WallForbid)
				ircdproto->SendGlobops(s_NickServ, "\2%s\2 used SUSPEND on \2%s\2", u->nick, nick);

			alog("%s: %s set SUSPEND for nick %s", s_NickServ, u->nick, nick);
			notice_lang(s_NickServ, u, NICK_SUSPEND_SUCCEEDED, nick);
			
			FOREACH_MOD(I_OnNickSuspended, OnNickSuspend(na))
		}
		else
		{
			alog("%s: Valid SUSPEND for %s by %s failed", s_NickServ, nick, u->nick);
			notice_lang(s_NickServ, u, NICK_SUSPEND_FAILED, nick);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_NickServ, u, NICK_SERVADMIN_HELP_SUSPEND);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_NickServ, u, "SUSPEND", NICK_SUSPEND_SYNTAX);
	}
};

class CommandNSUnSuspend : public Command
{
 public:
	CommandNSUnSuspend() : Command("UNSUSPEND", 1, 1, "nickserv/suspend")
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		NickAlias *na;
		const char *nick = params[0].c_str();

		if (readonly)
		{
			notice_lang(s_NickServ, u, READ_ONLY_MODE);
			return MOD_CONT;
		}

		if (!(na = findnick(nick)))
		{
			notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
			return MOD_CONT;
		}

		if (na->status & NS_FORBIDDEN)
		{
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
			return MOD_CONT;
		}

		if (NSSecureAdmins && na->nc->IsServicesOper())
		{
			notice_lang(s_NickServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (na)
		{
			na->nc->flags &= ~NI_SUSPENDED;

			if (WallForbid)
				ircdproto->SendGlobops(s_NickServ, "\2%s\2 used UNSUSPEND on \2%s\2", u->nick, nick);

			alog("%s: %s set UNSUSPEND for nick %s", s_NickServ, u->nick, nick);
			notice_lang(s_NickServ, u, NICK_UNSUSPEND_SUCCEEDED, nick);
			
			FOREACH_MOD(I_OnNickUnsuspended, OnNickUnsuspended(na));
		}
		else
		{
			alog("%s: Valid UNSUSPEND for %s by %s failed", s_NickServ, nick, u->nick);
			notice_lang(s_NickServ, u, NICK_UNSUSPEND_FAILED, nick);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_NickServ, u, NICK_SERVADMIN_HELP_UNSUSPEND);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_NickServ, u, "UNSUSPEND", NICK_UNSUSPEND_SYNTAX);
	}
};

class NSSuspend : public Module
{
 public:
	NSSuspend(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSSuspend(), MOD_UNIQUE);
		this->AddCommand(NICKSERV, new CommandNSUnSuspend(), MOD_UNIQUE);
	}
	void NickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_SUSPEND);
		notice_lang(s_NickServ, u, NICK_HELP_CMD_UNSUSPEND);
	}
};

MODULE_INIT("ns_suspend", NSSuspend)
