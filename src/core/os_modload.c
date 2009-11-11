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

class CommandOSModLoad : public Command
{
 public:
	CommandOSModLoad() : Command("MODLOAD", 1, 1, "operserv/modload")
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *name = params[0].c_str();

		Module *m = findModule(name);
		if (m)
		{
			notice_lang(s_OperServ, u, OPER_MODULE_ALREADY_LOADED, name);
			return MOD_CONT;
		}

		int status = ModuleManager::LoadModule(name, u);
		if (status != MOD_ERR_OK)
		{
			notice_lang(s_OperServ, u, OPER_MODULE_LOAD_FAIL, name);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_MODLOAD);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
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
		this->SetPermanent(true);

		this->AddCommand(OPERSERV, new CommandOSModLoad());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_MODLOAD);
	}
};

MODULE_INIT(OSModLoad)
