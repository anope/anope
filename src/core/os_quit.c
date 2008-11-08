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

int do_os_quit(User * u);
void myOperServHelp(User * u);

class OSQuit : public Module
{
 public:
	OSQuit(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("QUIT", do_os_quit, is_services_root, OPER_HELP_QUIT, -1, -1, -1, -1);
		this->AddCommand(OPERSERV, c, MOD_UNIQUE);

		moduleSetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
    if (is_services_root(u)) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_QUIT);
    }
}

/**
 * The /os quit command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_os_quit(User * u)
{
    quitmsg = (char *)calloc(28 + strlen(u->nick), 1);
    if (!quitmsg)
        quitmsg = "QUIT command received, but out of memory!";
    else
        sprintf((char *)quitmsg, "QUIT command received from %s", u->nick); // XXX we know this is safe, but..

    if (GlobalOnCycle) {
        oper_global(NULL, "%s", GlobalOnCycleMessage);
    }
    quitting = 1;
    return MOD_CONT;
}

MODULE_INIT("os_quit", OSQuit)
