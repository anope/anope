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
 *
 */
/*************************************************************************/

#include "module.h"

class CommandOSOLine : public Command
{
 public:
	CommandOSOLine() : Command("OLINE", 2, 2, "operserv/oline")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		const char *flag = params[1].c_str();
		User *u2 = NULL;

		/* let's check whether the user is online */
		if (!(u2 = finduser(nick)))
			notice_lang(Config.s_OperServ, u, NICK_X_NOT_IN_USE, nick);
		else if (u2 && flag[0] == '+')
		{
			ircdproto->SendSVSO(Config.s_OperServ, nick, flag);
			u2->SetMode(findbot(Config.s_OperServ), UMODE_OPER);
			notice_lang(Config.s_OperServ, u2, OPER_OLINE_IRCOP);
			notice_lang(Config.s_OperServ, u, OPER_OLINE_SUCCESS, flag, nick);
			ircdproto->SendGlobops(findbot(Config.s_OperServ), "\2%s\2 used OLINE for %s", u->nick.c_str(), nick);
		}
		else if (u2 && flag[0] == '-')
		{
			ircdproto->SendSVSO(Config.s_OperServ, nick, flag);
			notice_lang(Config.s_OperServ, u, OPER_OLINE_SUCCESS, flag, nick);
			ircdproto->SendGlobops(findbot(Config.s_OperServ), "\2%s\2 used OLINE for %s", u->nick.c_str(), nick);
		}
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_OLINE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "OLINE", OPER_OLINE_SYNTAX);
	}
};

class OSOLine : public Module
{
 public:
	OSOLine(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSOLine());

		if (!ircd->omode)
			throw ModuleException("Your IRCd does not support OMODE.");

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_OLINE);
	}
};

MODULE_INIT(OSOLine)
