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

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		quitmsg = "QUIT command received from " + u->nick;

		if (Config.GlobalOnCycle)
			oper_global("", "%s", Config.GlobalOnCycleMessage.c_str());
		quitting = 1;
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_QUIT);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_QUIT);
	}
};

class OSQuit : public Module
{
	CommandOSQuit commandosquit;

 public:
	OSQuit(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosquit);
	}
};

MODULE_INIT(OSQuit)
