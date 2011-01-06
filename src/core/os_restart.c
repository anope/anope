/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
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

#ifdef _WIN32
/* OperServ restart needs access to this if were gonna avoid sending ourself a signal */
extern MDE void do_restart_services(void);
#endif

int do_restart(User * u);
void myOperServHelp(User * u);

/**
 * Create the command, and tell anope about it.
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT to allow the module, MOD_STOP to stop it
 **/
int AnopeInit(int argc, char **argv)
{
    Command *c;

    moduleAddAuthor("Anope");
    moduleAddVersion(VERSION_STRING);
    moduleSetType(CORE);
    c = createCommand("RESTART", do_restart, is_services_root,
                      OPER_HELP_RESTART, -1, -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);

    moduleSetOperHelp(myOperServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}


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
    quitmsg = calloc(31 + strlen(u->nick), 1);
    if (!quitmsg)
        quitmsg = "RESTART command received, but out of memory!";
    else
        sprintf(quitmsg, "RESTART command received from %s", u->nick);

    if (GlobalOnCycle) {
        oper_global(NULL, "%s", GlobalOnCycleMessage);
    }
    /*    raise(SIGHUP); */
    do_restart_services();
#else
    notice_lang(s_OperServ, u, OPER_CANNOT_RESTART);
#endif
    return MOD_CONT;
}
