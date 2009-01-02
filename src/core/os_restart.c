/* OperServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at info@anope.org
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

#ifdef _WIN32
/* OperServ restart needs access to this if were gonna avoid sending ourself a signal */
extern MDE void do_restart_services();
#endif

int do_restart(User * u);
void myOperServHelp(User * u);

class OSRestart : public Module
{
 public:
	OSRestart(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		c = createCommand("RESTART", do_restart, is_services_root, OPER_HELP_RESTART, -1, -1, -1, -1);
		this->AddCommand(OPERSERV, c, MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
	if (is_services_root(u)) {
		notice_lang(s_OperServ, u, OPER_HELP_CMD_RESTART);
	}
}

/**
 * The /os restart command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_restart(User * u)
{
#ifdef SERVICES_BIN
	quitmsg = new char[31 + strlen(u->nick)];
	if (!quitmsg)
		quitmsg = "RESTART command received, but out of memory!";
	else
		sprintf(const_cast<char *>(quitmsg), /* XXX */ "RESTART command received from %s", u->nick);

	if (GlobalOnCycle) {
		oper_global(NULL, "%s", GlobalOnCycleMessage);
	}
	/*	raise(SIGHUP); */
	do_restart_services();
#else
	notice_lang(s_OperServ, u, OPER_CANNOT_RESTART);
#endif
	return MOD_CONT;
}

MODULE_INIT("os_restart", OSRestart)
