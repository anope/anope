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

class CommandOSUpdate : public Command
{
 public:
	CommandOSUpdate() : Command("UPDATE", 0, 0, "operserv/update")
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		notice_lang(s_OperServ, u, OPER_UPDATING);
		save_data = 1;
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_UPDATE);
		return true;
	}
};

class OSUpdate : public Module
{
 public:
	OSUpdate(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSUpdate(), MOD_UNIQUE);
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_UPDATE);
	}
};

MODULE_INIT("os_update", OSUpdate)
