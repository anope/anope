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

class CommandOSGlobal : public Command
{
 public:
	CommandOSGlobal() : Command("GLOBAL", 1, 1, "operserv/global")
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *msg = params[0].c_str();

		if (WallOSGlobal)
			ircdproto->SendGlobops(s_OperServ, "\2%s\2 just used GLOBAL command.", u->nick);
		oper_global(u->nick, "%s", msg);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_GLOBAL, s_GlobalNoticer);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "GLOBAL", OPER_GLOBAL_SYNTAX);
	}
};

class OSGlobal : public Module
{
 public:
	OSGlobal(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSGlobal());
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_GLOBAL);
	}
};

MODULE_INIT("os_global", OSGlobal)
