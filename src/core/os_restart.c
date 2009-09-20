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

class CommandOSRestart : public Command
{
 public:
	CommandOSRestart() : Command("RESTART", 0, 0, "operserv/restart")
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		quitmsg = new char[31 + strlen(u->nick)];
		if (!quitmsg)
			quitmsg = "RESTART command received, but out of memory!";
		else
			sprintf(const_cast<char *>(quitmsg), /* XXX */ "RESTART command received from %s", u->nick);

		if (GlobalOnCycle)
			oper_global(NULL, "%s", GlobalOnCycleMessage);
		/*	raise(SIGHUP); */
		do_restart_services();
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_RESTART);
		return true;
	}
};

class OSRestart : public Module
{
 public:
	OSRestart(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(OPERSERV, new CommandOSRestart());
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_RESTART);
	}
};

MODULE_INIT(OSRestart)
