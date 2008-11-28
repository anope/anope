/* OperServ core functions
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
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

int do_modload(User * u);
void myOperServHelp(User * u);

class OSModLoad : public Module
{
 public:
	OSModLoad(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("MODLOAD", do_modload, is_services_root, -1, -1, -1, -1, OPER_HELP_MODLOAD);
		this->AddCommand(OPERSERV, c, MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
	if (is_services_root(u)) {
		notice_lang(s_OperServ, u, OPER_HELP_CMD_MODLOAD);
	}
}

/**
 * The /os modload command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_modload(User * u)
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
		notice_lang(s_OperServ, u, OPER_MODULE_LOAD_FAIL, m->name.c_str());
	}

	return MOD_CONT;
}

MODULE_INIT("os_modload", OSModLoad)
