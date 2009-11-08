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

class CommandNSGhost : public Command
{
 public:
	CommandNSGhost() : Command("GHOST", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		const char *pass = params.size() > 1 ? params[1].c_str() : NULL;
		NickAlias *na = findnick(nick);

		if (!finduser(nick))
			notice_lang(s_NickServ, u, NICK_X_NOT_IN_USE, nick);
		else if (!na)
			notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
		else if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else if (na->nc->HasFlag(NI_SUSPENDED))
			notice_lang(s_NickServ, u, NICK_X_SUSPENDED, na->nick);
		else if (!stricmp(nick, u->nick))
			notice_lang(s_NickServ, u, NICK_NO_GHOST_SELF);
		else if (pass)
		{
			int res = enc_check_password(pass, na->nc->pass);
			if (res == 1)
			{
				char buf[NICKMAX + 32];
				snprintf(buf, sizeof(buf), "GHOST command used by %s", u->nick);
				kill_user(s_NickServ, nick, buf);
				notice_lang(s_NickServ, u, NICK_GHOST_KILLED, nick);
			}
			else
			{
				notice_lang(s_NickServ, u, ACCESS_DENIED);
				if (!res)
				{
					alog("%s: GHOST: invalid password for %s by %s!%s@%s", s_NickServ, nick, u->nick, u->GetIdent().c_str(), u->host);
					bad_password(u);
				}
			}
		}
		else
		{
			if (u->nc == na->nc || (!(na->nc->HasFlag(NI_SECURE)) && is_on_access(u, na->nc)))
			{
				char buf[NICKMAX + 32];
				snprintf(buf, sizeof(buf), "GHOST command used by %s", u->nick);
				kill_user(s_NickServ, nick, buf);
				notice_lang(s_NickServ, u, NICK_GHOST_KILLED, nick);
			}
			else
				notice_lang(s_NickServ, u, ACCESS_DENIED);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_NickServ, u, NICK_HELP_GHOST);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_NickServ, u, "GHOST", NICK_GHOST_SYNTAX);
	}
};

class NSGhost : public Module
{
 public:
	NSGhost(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSGhost());

		ModuleManager::Attach(I_OnNickServHelp, this);
	}
	void OnNickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_GHOST);
	}
};

MODULE_INIT(NSGhost)
