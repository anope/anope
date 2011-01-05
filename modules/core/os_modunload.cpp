/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
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

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &mname = params[0];

		Module *m = FindModule(mname);
		if (!m)
		{
			source.Reply(OPER_MODULE_ISNT_LOADED, mname.c_str());
			return MOD_CONT;
		}
		
		if (!m->handle)
		{
			source.Reply(OPER_MODULE_REMOVE_FAIL, m->name.c_str());
			return MOD_CONT;
		}
	
		if (m->GetPermanent() || m->type == PROTOCOL)
		{
			source.Reply(OPER_MODULE_NO_UNLOAD);
			return MOD_CONT;
		}

		Log() << "Trying to unload module [" << mname << "]";

		ModuleReturn status = ModuleManager::UnloadModule(m, u);

		if (status == MOD_ERR_OK)
		{
			source.Reply(OPER_MODULE_UNLOADED, mname.c_str());
			ircdproto->SendGlobops(OperServ, "%s unloaded module %s", u->nick.c_str(), mname.c_str());
		}
		else
			source.Reply(OPER_MODULE_REMOVE_FAIL, mname.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(OPER_HELP_MODUNLOAD);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "MODUNLOAD", OPER_MODULE_UNLOAD_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(OPER_HELP_CMD_MODUNLOAD);
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
