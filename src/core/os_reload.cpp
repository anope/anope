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
 *
 */
/*************************************************************************/

#include "module.h"

class CommandOSReload : public Command
{
 public:
	CommandOSReload() : Command("RELOAD", 0, 0, "operserv/reload")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		if (!read_config(1))
		{
			quitmsg = new char[28 + u->nick.length()];
			if (!quitmsg)
				quitmsg = "Error during the reload of the configuration file, but out of memory!";
			else
				sprintf(const_cast<char *>(quitmsg), /* XXX */ "Error during the reload of the configuration file!");
			quitting = 1;
		}

		FOREACH_MOD(I_OnReload, OnReload(false));
		notice_lang(Config.s_OperServ, u, OPER_RELOAD);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_RELOAD);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_RELOAD);
	}
};

class OSReload : public Module
{
 public:
	OSReload(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSReload());
	}
};

MODULE_INIT(OSReload)
