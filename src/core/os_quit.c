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

class CommandOSQuit : public Command
{
 public:
	CommandOSQuit() : Command("QUIT", 0, 0, "operserv/quit")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{

		quitmsg = new char[28 + u->nick.length()];
		if (!quitmsg)
			quitmsg = "QUIT command received, but out of memory!";
		else
			sprintf(const_cast<char *>(quitmsg), "QUIT command received from %s", u->nick.c_str()); // XXX we know this is safe, but..

		if (Config.GlobalOnCycle)
			oper_global(NULL, "%s", Config.GlobalOnCycleMessage);
		quitting = 1;
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_QUIT);
		return true;
	}
};

class OSQuit : public Module
{
 public:
	OSQuit(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSQuit());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_QUIT);
	}
};

MODULE_INIT(OSQuit)
