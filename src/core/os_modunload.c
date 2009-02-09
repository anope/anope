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

class CommandOSModUnLoad : public Command
{
 public:
	CommandOSModUnLoad() : Command("MODUNLOAD", 1, 1)
	{
	}

	CommandResult Execute(User *u, std::vector<std::string> &params)
	{
		const char *name = params[0].c_str();
		int status;

		Module *m = findModule(name);
		if (!m)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		alog("Trying to unload module [%s]", name);

		status = ModuleManager::UnloadModule(m, u);

		if (status != MOD_ERR_OK)
			notice_lang(s_OperServ, u, OPER_MODULE_REMOVE_FAIL, name);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_root(u))
			return false;

		notice_lang(s_OperServ, u, OPER_HELP_MODUNLOAD);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "MODUNLOAD", OPER_MODULE_UNLOAD_SYNTAX);
	}
};

class OSModUnLoad : public Module
{
 public:
	OSModUnLoad(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->SetPermanent(true);

		this->AddCommand(OPERSERV, new CommandOSModUnLoad(), MOD_UNIQUE);

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
		notice_lang(s_OperServ, u, OPER_HELP_CMD_MODUNLOAD);
}

MODULE_INIT("os_modunload", OSModUnLoad)
