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

class CommandOSModReLoad : public Command
{
 public:
	CommandOSModReLoad() : Command("MODRELOAD", 1, 1, "operserv/modload")
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

		if (m->GetPermanent())
		{
			source.Reply(OPER_MODULE_NO_UNLOAD);
			return MOD_CONT;
		}

		/* Unrecoverable */
		bool fatal = m->type == PROTOCOL;
		ModuleReturn status = ModuleManager::UnloadModule(m, u);

		if (status != MOD_ERR_OK)
		{
			source.Reply(OPER_MODULE_REMOVE_FAIL, mname.c_str());
			return MOD_CONT;
		}

		status = ModuleManager::LoadModule(mname, u);
		if (status == MOD_ERR_OK)
		{
			ircdproto->SendGlobops(OperServ, "%s reloaded module %s", u->nick.c_str(), mname.c_str());
			source.Reply(OPER_MODULE_RELOADED, mname.c_str());

			/* If a user is loading this module, then the core databases have already been loaded
			 * so trigger the event manually
			 */
			m = FindModule(mname);
			if (m)
				m->OnPostLoadDatabases();
		}
		else
		{
			if (fatal)
				throw FatalException("Unable to reload module " + mname);
			else
				source.Reply(OPER_MODULE_LOAD_FAIL, mname.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(OPER_HELP_MODRELOAD);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "MODLOAD", OPER_MODULE_RELOAD_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(OPER_HELP_CMD_MODRELOAD);
	}
};

class OSModReLoad : public Module
{
	CommandOSModReLoad commandosmodreload;

 public:
	OSModReLoad(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);
		this->SetPermanent(true);

		this->AddCommand(OperServ, &commandosmodreload);
	}
};

MODULE_INIT(OSModReLoad)
