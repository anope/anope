/* OperServ core functions
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

void myOperServHelp(User *u);

class CommandOSSVSNick : public Command
{
 public:
	CommandOSSVSNick() : Command("SVSNICK", 2, 2)
	{
	}

	CommandResult Execute(User *u, std::vector<std::string> &params)
	{
		const char *nick = params[0].c_str();
		const char *newnick = params[1].c_str();

		NickAlias *na;
		char *c;

		/* Only allow this if SuperAdmin is enabled */
		if (!u->isSuperAdmin)
		{
			notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ONLY);
			return MOD_CONT;
		}

		/* Truncate long nicknames to NICKMAX-2 characters */
		if (strlen(newnick) > NICKMAX - 2)
		{
			notice_lang(s_OperServ, u, NICK_X_TRUNCATED, newnick, NICKMAX - 2, newnick);
			newnick[NICKMAX - 2] = '\0';
		}

		/* Check for valid characters */
		if (*newnick == '-' || isdigit(*newnick))
		{
			notice_lang(s_OperServ, u, NICK_X_ILLEGAL, newnick);
			return MOD_CONT;
		}
		for (c = newnick; *c && c - newnick < NICKMAX; ++c)
		{
			if (!isvalidnick(*c))
			{
				notice_lang(s_OperServ, u, NICK_X_ILLEGAL, newnick);
				return MOD_CONT;
			}
		}

		/* Check for a nick in use or a forbidden/suspended nick */
		if (!finduser(nick))
			notice_lang(s_OperServ, u, NICK_X_NOT_IN_USE, nick);
		else if (finduser(newnick))
			notice_lang(s_OperServ, u, NICK_X_IN_USE, newnick);
		else if ((na = findnick(newnick)) && (na->status & NS_FORBIDDEN))
			notice_lang(s_OperServ, u, NICK_X_FORBIDDEN, newnick);
		else
		{
			notice_lang(s_OperServ, u, OPER_SVSNICK_NEWNICK, nick, newnick);
			ircdproto->SendGlobops(s_OperServ, "%s used SVSNICK to change %s to %s", u->nick, nick, newnick);
			ircdproto->SendForceNickChange(nick, newnick, time(NULL));
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_root(u))
			return false;

		notice_lang(s_OperServ, u, OPER_HELP_SVSNICK);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "SVSNICK", OPER_SVSNICK_SYNTAX);
	}
};

class OSSVSNick : public Module
{
 public:
	OSSVSNick(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSSVSNick(), MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
		if (!ircd->svsnick)
			throw ModuleException("Your IRCd does not support SVSNICK");
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	if (is_services_admin(u) && u->isSuperAdmin)
		notice_lang(s_OperServ, u, OPER_HELP_CMD_SVSNICK);
}

MODULE_INIT("os_svsnick", OSSVSNick)
