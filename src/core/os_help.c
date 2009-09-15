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

class CommandOSHelp : public Command
{
 public:
	CommandOSHelp() : Command("HELP", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		mod_help_cmd(s_OperServ, u, OPERSERV, params[0].c_str());
		return MOD_CONT;
	}

	void OnSyntaxError(User *u)
	{
		notice_help(s_OperServ, u, OPER_HELP);
		moduleDisplayHelp(s_OperServ, u);
		notice_help(s_OperServ, u, OPER_HELP_LOGGED);
	}
};

class OSHelp : public Module
{
 public:
	OSHelp(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(OPERSERV, new CommandOSHelp());
	}
};

MODULE_INIT("os_help", OSHelp)
