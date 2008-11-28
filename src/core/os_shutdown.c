/* OperServ core functions
 *
 * (C) 2003-2008 Anope Team
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

int do_shutdown(User * u);
void myOperServHelp(User * u);

class OSShutdown : public Module
{
 public:
	OSShutdown(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("SHUTDOWN", do_shutdown, is_services_root, OPER_HELP_SHUTDOWN, -1, -1, -1, -1);
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
		notice_lang(s_OperServ, u, OPER_HELP_CMD_SHUTDOWN);
	}
}

/**
 * The /os shutdown command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_shutdown(User * u)
{
	quitmsg = new char[32 + strlen(u->nick)];
	if (!quitmsg)
		quitmsg = "SHUTDOWN command received, but out of memory!";
	else
		sprintf(const_cast<char *>(quitmsg), /* XXX */ "SHUTDOWN command received from %s", u->nick);

	if (GlobalOnCycle) {
		oper_global(NULL, "%s", GlobalOnCycleMessage);
	}
	save_data = 1;
	delayed_quit = 1;
	return MOD_CONT;
}

MODULE_INIT("os_shutdown", OSShutdown)
