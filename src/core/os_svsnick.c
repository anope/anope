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

class CommandOSSVSNick : public Command
{
 public:
	CommandOSSVSNick() : Command("SVSNICK", 2, 2, "operserv/svsnick")
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		const char *newnick = params[1].c_str();

		NickAlias *na;
		const char *c;

		/* Truncate long nicknames to NICKMAX-2 characters */
		if (strlen(newnick) > NICKMAX - 2)
		{
			notice_lang(s_OperServ, u, NICK_X_TRUNCATED, newnick, NICKMAX - 2, newnick);
			params[1] = params[1].substr(0, NICKMAX - 2);
			newnick = params[1].c_str();
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
		else if ((na = findnick(newnick)) && (na->HasFlag(NS_FORBIDDEN)))
			notice_lang(s_OperServ, u, NICK_X_FORBIDDEN, newnick);
		else
		{
			notice_lang(s_OperServ, u, OPER_SVSNICK_NEWNICK, nick, newnick);
			ircdproto->SendGlobops(s_OperServ, "%s used SVSNICK to change %s to %s", u->nick, nick, newnick);
			ircdproto->SendForceNickChange(nick, newnick, time(NULL));
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_SVSNICK);
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

		this->AddCommand(OPERSERV, new CommandOSSVSNick());

		if (!ircd->svsnick)
			throw ModuleException("Your IRCd does not support SVSNICK");

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_SVSNICK);
	}
};

MODULE_INIT(OSSVSNick)
