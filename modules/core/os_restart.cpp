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

class CommandOSRestart : public Command
{
 public:
	CommandOSRestart() : Command("RESTART", 0, 0, "operserv/restart")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		quitmsg = "RESTART command received from " + u->nick;

		if (Config->GlobalOnCycle)
			oper_global("", "%s", Config->GlobalOnCycleMessage.c_str());
		/*	raise(SIGHUP); */
		do_restart_services();
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(OPER_HELP_RESTART);
		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(OPER_HELP_CMD_RESTART);
	}
};

class OSRestart : public Module
{
	CommandOSRestart commandosrestart;

 public:
	OSRestart(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosrestart);
	}
};

MODULE_INIT(OSRestart)
