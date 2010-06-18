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

class CommandOSModUnLoad : public Command
{
 public:
	CommandOSModUnLoad() : Command("MODUNLOAD", 1, 1, "operserv/modload")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *name = params[0].c_str();
		int status;

		Module *m = findModule(name);
		if (!m)
		{
			notice_lang(Config.s_OperServ, u, OPER_MODULE_ISNT_LOADED, name);
			return MOD_CONT;
		}

		Alog() << "Trying to unload module [" << name << "]";

		status = ModuleManager::UnloadModule(m, u);

		if (status != MOD_ERR_OK)
			notice_lang(Config.s_OperServ, u, OPER_MODULE_REMOVE_FAIL, name);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_MODUNLOAD);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "MODUNLOAD", OPER_MODULE_UNLOAD_SYNTAX);
	}
};

class OSModUnLoad : public Module
{
 public:
	OSModUnLoad(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
		this->SetPermanent(true);

		this->AddCommand(OPERSERV, new CommandOSModUnLoad());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_MODUNLOAD);
	}
};

MODULE_INIT(OSModUnLoad)
