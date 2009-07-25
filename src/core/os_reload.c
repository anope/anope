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

class CommandOSReload : public Command
{
 public:
	CommandOSReload() : Command("RELOAD", 0, 0, "operserv/reload")
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		if (!read_config(1))
		{
			quitmsg = new char[28 + strlen(u->nick)];
			if (!quitmsg)
				quitmsg = "Error during the reload of the configuration file, but out of memory!";
			else
				sprintf(const_cast<char *>(quitmsg), /* XXX */ "Error during the reload of the configuration file!");
			quitting = 1;
		}

		FOREACH_MOD(I_OnReload, OnReload(false));
		notice_lang(s_OperServ, u, OPER_RELOAD);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_RELOAD);
		return true;
	}
};

class OSReload : public Module
{
 public:
	OSReload(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSReload(), MOD_UNIQUE);
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_RELOAD);
	}
};

MODULE_INIT("os_reload", OSReload)
