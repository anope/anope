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

int do_noop(User * u);
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

    c = createCommand("NOOP", do_noop, is_services_admin, OPER_HELP_NOOP,
                      -1, -1, -1, -1);
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
        notice_lang(s_OperServ, u, OPER_HELP_CMD_NOOP);
    }
}

/**
 * The /os noop command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_noop(User * u)
{
    char *cmd = strtok(NULL, " ");
    char *server = strtok(NULL, " ");

    if (!cmd || !server) {
        syntax_error(s_OperServ, u, "NOOP", OPER_NOOP_SYNTAX);
    } else if (!stricmp(cmd, "SET")) {
        User *u2;
        User *u3 = NULL;
        char reason[NICKMAX + 32];

        /* Remove the O:lines */
        anope_cmd_svsnoop(server, 1);

        snprintf(reason, sizeof(reason), "NOOP command used by %s",
                 u->nick);
        if (WallOSNoOp)
            anope_cmd_global(s_OperServ, "\2%s\2 used NOOP on \2%s\2",
                             u->nick, server);
        notice_lang(s_OperServ, u, OPER_NOOP_SET, server);

        /* Kill all the IRCops of the server */
        for (u2 = firstuser(); u2; u2 = u3) {
            u3 = nextuser();
            if ((u2) && is_oper(u2) && (u2->server->name)
                && match_wild(server, u2->server->name)) {
                kill_user(s_OperServ, u2->nick, reason);
            }
        }
    } else if (!stricmp(cmd, "REVOKE")) {
        anope_cmd_svsnoop(server, 0);
        notice_lang(s_OperServ, u, OPER_NOOP_REVOKE, server);
    } else {
        syntax_error(s_OperServ, u, "NOOP", OPER_NOOP_SYNTAX);
    }
    return MOD_CONT;
}
