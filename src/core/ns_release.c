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

class CommandNSRelease : public Command
{
 public:
	CommandNSRelease() : Command("RELEASE", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		std::string pass = params.size() > 1 ? params[1].c_str() : "";
		NickAlias *na;

		if (!(na = findnick(nick)))
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
		else if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else if (na->nc->HasFlag(NI_SUSPENDED))
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, na->nick);
		else if (!(na->HasFlag(NS_HELD)))
			notice_lang(Config.s_NickServ, u, NICK_RELEASE_NOT_HELD, nick);
		else if (!pass.empty())
		{
			int res = enc_check_password(pass, na->nc->pass);
			if (res == 1)
			{
				na->Release();
				notice_lang(Config.s_NickServ, u, NICK_RELEASED);
			}
			else
			{
				notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
				if (!res)
				{
					Alog() << Config.s_NickServ << ": RELEASE: invalid password for " << nick << " by " << u->GetMask();
					if (bad_password(u))
						return MOD_STOP;
				}
			}
		}
		else
		{
			if (u->Account() == na->nc || (!(na->nc->HasFlag(NI_SECURE)) && is_on_access(u, na->nc)))
			{
				na->Release();
				notice_lang(Config.s_NickServ, u, NICK_RELEASED);
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

		notice_help(Config.s_NickServ, u, NICK_HELP_RELEASE, relstr);
		//do_help_limited(Config.s_NickServ, u, this);

		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "RELEASE", NICK_RELEASE_SYNTAX);
	}
};

class NSRelease : public Module
{
 public:
	NSRelease(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSRelease());

		ModuleManager::Attach(I_OnNickServHelp, this);
	}
	void OnNickServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_RELEASE);
	}
};

MODULE_INIT(NSRelease)
