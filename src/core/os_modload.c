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

class CommandOSModLoad : public Command
{
 public:
	CommandOSModLoad() : Command("MODLOAD", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		char *name;

		name = strtok(NULL, "");
		if (!name)
		{
			syntax_error(s_OperServ, u, "MODLOAD", OPER_MODULE_LOAD_SYNTAX);
			return MOD_CONT;
		}

		Module *m = findModule(name);
		if (m)
		{
			notice_lang(s_OperServ, u, OPER_MODULE_LOAD_FAIL, name);
			return MOD_CONT;
		}

		int status = ModuleManager::LoadModule(name, u);
		if (status != MOD_ERR_OK)
		{
			notice_lang(s_OperServ, u, OPER_MODULE_LOAD_FAIL, name);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_root(u))
			return false;

		notice_lang(s_OperServ, u, OPER_HELP_MODLOAD);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "MODLOAD", OPER_MODULE_LOAD_SYNTAX);
	}
};

class OSModLoad : public Module
{
 public:
	OSModLoad(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSModLoad(), MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	if (is_services_root(u))
		notice_lang(s_OperServ, u, OPER_HELP_CMD_MODLOAD);
}

MODULE_INIT("os_modload", OSModLoad)
