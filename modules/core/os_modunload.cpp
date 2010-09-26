/* OperServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandOSModUnLoad : public Command
{
 public:
	CommandOSModUnLoad() : Command("MODUNLOAD", 1, 1, "operserv/modload")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string mname = params[0];
		int status;

		Module *m = FindModule(mname);
		if (!m)
		{
			u->SendMessage(OperServ, OPER_MODULE_ISNT_LOADED, mname.c_str());
			return MOD_CONT;
		}

		Log() << "Trying to unload module [" << mname << "]";

		status = ModuleManager::UnloadModule(m, u);

		if (status != MOD_ERR_OK)
			u->SendMessage(OperServ, OPER_MODULE_REMOVE_FAIL, mname.c_str());

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(OperServ, OPER_HELP_MODUNLOAD);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(OperServ, u, "MODUNLOAD", OPER_MODULE_UNLOAD_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(OperServ, OPER_HELP_CMD_MODUNLOAD);
	}
};

class OSModUnLoad : public Module
{
	CommandOSModUnLoad commandosmodunload;

 public:
	OSModUnLoad(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);
		this->SetPermanent(true);

		this->AddCommand(OperServ, &commandosmodunload);
	}
};

MODULE_INIT(OSModUnLoad)
