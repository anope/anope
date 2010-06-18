/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
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

class CommandNSRecover : public Command
{
 public:
	CommandNSRecover() : Command("RECOVER", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		std::string pass = params.size() > 1 ? params[1].c_str() : "";
		NickAlias *na;
		User *u2;

		if (!(u2 = finduser(nick)))
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_IN_USE, nick);
		else if (!(na = findnick(u2->nick)))
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
		else if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else if (na->nc->HasFlag(NI_SUSPENDED))
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, na->nick);
		else if (!stricmp(nick, u->nick.c_str()))
			notice_lang(Config.s_NickServ, u, NICK_NO_RECOVER_SELF);
		else if (!pass.empty())
		{
			int res = enc_check_password(pass, na->nc->pass);

			if (res == 1)
			{
				char relstr[192];

				notice_lang(Config.s_NickServ, u2, FORCENICKCHANGE_NOW);
				u2->Collide(na);

				/* Convert Config.NSReleaseTimeout seconds to string format */
				duration(na->nc, relstr, sizeof(relstr), Config.NSReleaseTimeout);

				notice_lang(Config.s_NickServ, u, NICK_RECOVERED, Config.s_NickServ, nick, relstr);
			}
			else
			{
				notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
				if (!res)
				{
					Alog() << Config.s_NickServ << ": RECOVER: invalid password for " << nick << " by " << u->GetMask();
					if (bad_password(u))
						return MOD_STOP;
				}
			}
		}
		else
		{
			if (u->Account() == na->nc || (!(na->nc->HasFlag(NI_SECURE)) && is_on_access(u, na->nc)))
			{
				char relstr[192];

				notice_lang(Config.s_NickServ, u2, FORCENICKCHANGE_NOW);
				u2->Collide(na);

				/* Convert Config.NSReleaseTimeout seconds to string format */
				duration(na->nc, relstr, sizeof(relstr), Config.NSReleaseTimeout);

				notice_lang(Config.s_NickServ, u, NICK_RECOVERED, Config.s_NickServ, nick, relstr);
			}
			else
				notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		char relstr[192];

		/* Convert Config.NSReleaseTimeout seconds to string format */
		duration(u->Account(), relstr, sizeof(relstr), Config.NSReleaseTimeout);

		notice_help(Config.s_NickServ, u, NICK_HELP_RECOVER, relstr);
		//do_help_limited(Config.s_NickServ, u, this);

		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "RECOVER", NICK_RECOVER_SYNTAX);
	}
};

class NSRecover : public Module
{
 public:
	NSRecover(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSRecover());

		ModuleManager::Attach(I_OnNickServHelp, this);
	}
	void OnNickServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_RECOVER);
	}
};

MODULE_INIT(NSRecover)
