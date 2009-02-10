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

void myOperServHelp(User *u);

class CommandOSRestart : public Command
{
 public:
	CommandOSRestart() : Command("RESTART", 0, 0)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
#ifdef SERVICES_BIN
		quitmsg = new char[31 + strlen(u->nick)];
		if (!quitmsg)
			quitmsg = "RESTART command received, but out of memory!";
		else
			sprintf(const_cast<char *>(quitmsg), /* XXX */ "RESTART command received from %s", u->nick);

		if (GlobalOnCycle)
			oper_global(NULL, "%s", GlobalOnCycleMessage);
		/*	raise(SIGHUP); */
		do_restart_services();
#else
		notice_lang(s_OperServ, u, OPER_CANNOT_RESTART);
#endif
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_root(u))
			return false;

		notice_lang(s_OperServ, u, OPER_HELP_RESTART);
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
		this->AddCommand(OPERSERV, new CommandOSRestart(), MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};

/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	if (is_services_root(u))
		notice_lang(s_OperServ, u, OPER_HELP_CMD_RESTART);
}

MODULE_INIT("os_restart", OSRestart)
