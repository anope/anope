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

class CommandOSRestart : public Command
{
 public:
	CommandOSRestart() : Command("RESTART", 0, 0, "operserv/restart")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		quitmsg = new char[31 + u->nick.length()];
		if (!quitmsg)
			quitmsg = "RESTART command received, but out of memory!";
		else
			sprintf(const_cast<char *>(quitmsg), /* XXX */ "RESTART command received from %s", u->nick.c_str());

		if (Config.GlobalOnCycle)
			oper_global(NULL, "%s", Config.GlobalOnCycleMessage);
		/*	raise(SIGHUP); */
		do_restart_services();
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_RESTART);
		return true;
	}
};

class OSRestart : public Module
{
 public:
	OSRestart(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
		this->AddCommand(OPERSERV, new CommandOSRestart());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_RESTART);
	}
};

MODULE_INIT(OSRestart)
