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
 *
 */
/*************************************************************************/

#include "module.h"

class CommandNSGhost : public Command
{
 public:
	CommandNSGhost() : Command("GHOST", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		std::string pass = params.size() > 1 ? params[1].c_str() : "";
		NickAlias *na = findnick(nick);

		if (!finduser(nick))
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_IN_USE, nick);
		else if (!na)
			notice_lang(Config.s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
		else if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else if (na->nc->HasFlag(NI_SUSPENDED))
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, na->nick);
		else if (!stricmp(nick, u->nick.c_str()))
			notice_lang(Config.s_NickServ, u, NICK_NO_GHOST_SELF);
		else if (!pass.empty())
		{
			int res = enc_check_password(pass, na->nc->pass);
			if (res == 1)
			{
				std::string buf = "GHOST command used by " + u->nick;
				kill_user(Config.s_NickServ, nick, buf.c_str());
				notice_lang(Config.s_NickServ, u, NICK_GHOST_KILLED, nick);
			}
			else
			{
				notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
				if (!res)
				{
					Alog() << Config.s_NickServ << ": GHOST: invalid password for " << nick << " by " << u->GetMask();
					if (bad_password(u))
						return MOD_STOP;
				}
			}
		}
		else
		{
			if (u->Account() == na->nc || (!(na->nc->HasFlag(NI_SECURE)) && is_on_access(u, na->nc)))
			{
				std::string buf = "GHOST command used by " + u->nick;
				kill_user(Config.s_NickServ, nick, buf.c_str());
				notice_lang(Config.s_NickServ, u, NICK_GHOST_KILLED, nick);
			}
			else
				notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_GHOST);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "GHOST", NICK_GHOST_SYNTAX);
	}
};

class NSGhost : public Module
{
 public:
	NSGhost(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSGhost());

		ModuleManager::Attach(I_OnNickServHelp, this);
	}
	void OnNickServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_GHOST);
	}
};

MODULE_INIT(NSGhost)
