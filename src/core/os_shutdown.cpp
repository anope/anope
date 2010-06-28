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

class CommandOSShutdown : public Command
{
 public:
	CommandOSShutdown() : Command("SHUTDOWN", 0, 0, "operserv/shutdown")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		quitmsg = new char[32 + u->nick.length()];
		if (!quitmsg)
			quitmsg = "SHUTDOWN command received, but out of memory!";
		else
			sprintf(const_cast<char *>(quitmsg), /* XXX */ "SHUTDOWN command received from %s", u->nick.c_str());

		if (Config.GlobalOnCycle)
			oper_global(NULL, "%s", Config.GlobalOnCycleMessage);
		shutting_down = 1;
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_SHUTDOWN);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_SHUTDOWN);
	}
};

class OSShutdown : public Module
{
 public:
	OSShutdown(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSShutdown());
	}
};

MODULE_INIT(OSShutdown)
