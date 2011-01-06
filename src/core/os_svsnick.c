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

int do_svsnick(User * u);
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

    c = createCommand("SVSNICK", do_svsnick, is_services_root,
                      OPER_HELP_SVSNICK, -1, -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);

    moduleSetOperHelp(myOperServHelp);
    if (!ircd->svsnick) {
        return MOD_STOP;
    }
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
    if (is_services_admin(u) && u->isSuperAdmin) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_SVSNICK);
    }
}

/**
 * The /os svsnick command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
/* Forcefully change a user's nickname */

int do_svsnick(User * u)
{
    char *nick = strtok(NULL, " ");
    char *newnick = strtok(NULL, " ");

    NickAlias *na;
    char *c;

    /* Only allow this if SuperAdmin is enabled */
    if (!u->isSuperAdmin) {
        notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ONLY);
        return MOD_CONT;
    }

    if (!nick || !newnick) {
        syntax_error(s_OperServ, u, "SVSNICK", OPER_SVSNICK_SYNTAX);
        return MOD_CONT;
    }

    /* Truncate long nicknames to NICKMAX-2 characters */
    if (strlen(newnick) > (NICKMAX - 2)) {
        notice_lang(s_OperServ, u, NICK_X_TRUNCATED,
                    newnick, NICKMAX - 2, newnick);
        newnick[NICKMAX - 2] = '\0';
    }

    /* Check for valid characters */
    if (*newnick == '-' || isdigit(*newnick)) {
        notice_lang(s_OperServ, u, NICK_X_ILLEGAL, newnick);
        return MOD_CONT;
    }
    for (c = newnick; *c && (c - newnick) < NICKMAX; c++) {
        if (!isvalidnick(*c)) {
            notice_lang(s_OperServ, u, NICK_X_ILLEGAL, newnick);
            return MOD_CONT;
        }
    }

    /* Check for a nick in use or a forbidden/suspended nick */
    if (!finduser(nick)) {
        notice_lang(s_OperServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (finduser(newnick)) {
        notice_lang(s_OperServ, u, NICK_X_IN_USE, newnick);
    } else if ((na = findnick(newnick)) && (na->status & NS_VERBOTEN)) {
        notice_lang(s_OperServ, u, NICK_X_FORBIDDEN, newnick);
    } else {
        notice_lang(s_OperServ, u, OPER_SVSNICK_NEWNICK, nick, newnick);
        anope_cmd_global(s_OperServ, "%s used SVSNICK to change %s to %s",
                         u->nick, nick, newnick);
        anope_cmd_svsnick(nick, newnick, time(NULL));
    }
    return MOD_CONT;
}
