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

int do_global(User * u);
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

    c = createCommand("GLOBAL", do_global, is_services_admin,
                      OPER_HELP_GLOBAL, -1, -1, -1, -1);
    c->help_param1 = s_GlobalNoticer;
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
    if (is_services_admin(u)) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_GLOBAL);
    }
}

/**
 * The /os global command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_global(User * u)
{
    char *msg = strtok(NULL, "");

    if (!msg) {
        syntax_error(s_OperServ, u, "GLOBAL", OPER_GLOBAL_SYNTAX);
        return MOD_CONT;
    }
    if (WallOSGlobal)
        anope_cmd_global(s_OperServ, "\2%s\2 just used GLOBAL command.",
                         u->nick);
    oper_global(u->nick, "%s", msg);
    return MOD_CONT;
}
