/* NickServ core functions
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

class CommandNSSuspend : public Command
{
 public:
	CommandNSSuspend() : Command("SUSPEND", 2, 2, "nickserv/suspend")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickAlias *na;
		User *u2;
		Anope::string nick = params[0];
		Anope::string reason = params[1];

		if (readonly)
		{
			notice_lang(Config->s_NickServ, u, READ_ONLY_MODE);
			return MOD_CONT;
		}

		if (!(na = findnick(nick)))
		{
			notice_lang(Config->s_NickServ, u, NICK_X_NOT_REGISTERED, nick.c_str());
			return MOD_CONT;
		}

		if (na->HasFlag(NS_FORBIDDEN))
		{
			notice_lang(Config->s_NickServ, u, NICK_X_FORBIDDEN, na->nick.c_str());
			return MOD_CONT;
		}

		if (Config->NSSecureAdmins && na->nc->IsServicesOper())
		{
			notice_lang(Config->s_NickServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		na->nc->SetFlag(NI_SUSPENDED);
		na->nc->SetFlag(NI_SECURE);
		na->nc->UnsetFlag(NI_KILLPROTECT);
		na->nc->UnsetFlag(NI_KILL_QUICK);
		na->nc->UnsetFlag(NI_KILL_IMMED);

		for (std::list<NickAlias *>::iterator it = na->nc->aliases.begin(), it_end = na->nc->aliases.end(); it != it_end; ++it)
		{
			NickAlias *na2 = *it;

			if (na2->nc == na->nc)
			{
				na2->last_quit = reason;

				if ((u2 = finduser(na2->nick)))
				{
					u2->Logout();
					u2->Collide(na2);
				}
			}
		}

		if (Config->WallForbid)
			ircdproto->SendGlobops(NickServ, "\2%s\2 used SUSPEND on \2%s\2", u->nick.c_str(), nick.c_str());

		Alog() << Config->s_NickServ << ": " << u->nick << " set SUSPEND for nick " << nick;
		notice_lang(Config->s_NickServ, u, NICK_SUSPEND_SUCCEEDED, nick.c_str());

		FOREACH_MOD(I_OnNickSuspended, OnNickSuspend(na));

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_NickServ, u, NICK_SERVADMIN_HELP_SUSPEND);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_NickServ, u, "SUSPEND", NICK_SUSPEND_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_NickServ, u, NICK_HELP_CMD_SUSPEND);
	}
};

class CommandNSUnSuspend : public Command
{
 public:
	CommandNSUnSuspend() : Command("UNSUSPEND", 1, 1, "nickserv/suspend")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickAlias *na;
		Anope::string nick = params[0];

		if (readonly)
		{
			notice_lang(Config->s_NickServ, u, READ_ONLY_MODE);
			return MOD_CONT;
		}

		if (!(na = findnick(nick)))
		{
			notice_lang(Config->s_NickServ, u, NICK_X_NOT_REGISTERED, nick.c_str());
			return MOD_CONT;
		}

		if (na->HasFlag(NS_FORBIDDEN))
		{
			notice_lang(Config->s_NickServ, u, NICK_X_FORBIDDEN, na->nick.c_str());
			return MOD_CONT;
		}

		if (Config->NSSecureAdmins && na->nc->IsServicesOper())
		{
			notice_lang(Config->s_NickServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		na->nc->UnsetFlag(NI_SUSPENDED);

		if (Config->WallForbid)
			ircdproto->SendGlobops(NickServ, "\2%s\2 used UNSUSPEND on \2%s\2", u->nick.c_str(), nick.c_str());

		Alog() << Config->s_NickServ << ": " << u->nick << " set UNSUSPEND for nick " << nick;
		notice_lang(Config->s_NickServ, u, NICK_UNSUSPEND_SUCCEEDED, nick.c_str());

		FOREACH_MOD(I_OnNickUnsuspended, OnNickUnsuspended(na));

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_NickServ, u, NICK_SERVADMIN_HELP_UNSUSPEND);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_NickServ, u, "UNSUSPEND", NICK_UNSUSPEND_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_NickServ, u, NICK_HELP_CMD_UNSUSPEND);
	}
};

class NSSuspend : public Module
{
	CommandNSSuspend commandnssuspend;
	CommandNSUnSuspend commandnsunsuspend;

 public:
	NSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnssuspend);
		this->AddCommand(NickServ, &commandnsunsuspend);
	}
};

MODULE_INIT(NSSuspend)
