/* OperServ core functions
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

class CommandOSSVSNick : public Command
{
 public:
	CommandOSSVSNick() : Command("SVSNICK", 2, 2, "operserv/svsnick")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		ci::string newnick = params[1];
		User *u2;

		NickAlias *na;

		/* Truncate long nicknames to NICKMAX-2 characters */
		if (newnick.length() > Config.NickLen)
		{
			notice_lang(Config.s_OperServ, u, NICK_X_TRUNCATED, newnick.c_str(), Config.NickLen, newnick.c_str());
			newnick = params[1].substr(0, Config.NickLen);
		}

		/* Check for valid characters */
		if (newnick[0] == '-' || isdigit(newnick[0]))
		{
			notice_lang(Config.s_OperServ, u, NICK_X_ILLEGAL, newnick.c_str());
			return MOD_CONT;
		}
		for (unsigned i = 0; i < newnick.size(); ++i)
		{
			if (!isvalidnick(newnick[i]))
			{
				notice_lang(Config.s_OperServ, u, NICK_X_ILLEGAL, newnick.c_str());
				return MOD_CONT;
			}
		}

		/* Check for a nick in use or a forbidden/suspended nick */
		if (!(u2 = finduser(nick)))
			notice_lang(Config.s_OperServ, u, NICK_X_NOT_IN_USE, nick);
		else if (finduser(newnick.c_str()))
			notice_lang(Config.s_OperServ, u, NICK_X_IN_USE, newnick.c_str());
		else if ((na = findnick(newnick.c_str())) && (na->HasFlag(NS_FORBIDDEN)))
			notice_lang(Config.s_OperServ, u, NICK_X_FORBIDDEN, newnick.c_str());
		else
		{
			notice_lang(Config.s_OperServ, u, OPER_SVSNICK_NEWNICK, nick, newnick.c_str());
			ircdproto->SendGlobops(findbot(Config.s_OperServ), "%s used SVSNICK to change %s to %s", u->nick.c_str(), nick, newnick.c_str());
			ircdproto->SendForceNickChange(u2, newnick.c_str(), time(NULL));
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_SVSNICK);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "SVSNICK", OPER_SVSNICK_SYNTAX);
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
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_SVSNICK);
	}
};

MODULE_INIT(OSSVSNick)
