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
int do_operoline(User * u);

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

    c = createCommand("OLINE", do_operoline, is_services_root,
                      OPER_HELP_OLINE, -1, -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);

    moduleSetOperHelp(myOperServHelp);

    if (!ircd->omode) {
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
        notice_lang(s_OperServ, u, OPER_HELP_CMD_OLINE);
    }
}

/**
 * The /os oline command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_operoline(User * u)
{
    char *nick = strtok(NULL, " ");
    char *flags = strtok(NULL, "");
    User *u2 = NULL;

    /* Only allow this if SuperAdmin is enabled */
    if (!u->isSuperAdmin) {
        notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ONLY);
        return MOD_CONT;
    }

    if (!nick || !flags) {
        syntax_error(s_OperServ, u, "OLINE", OPER_OLINE_SYNTAX);
        return MOD_CONT;
    } else {
        /* let's check whether the user is online */
        if (!(u2 = finduser(nick))) {
            notice_lang(s_OperServ, u, NICK_X_NOT_IN_USE, nick);
        } else if (u2 && flags[0] == '+') {
            anope_cmd_svso(s_OperServ, nick, flags);
            common_svsmode(u2, "+o", NULL);
            notice_lang(s_OperServ, u2, OPER_OLINE_IRCOP);
            notice_lang(s_OperServ, u, OPER_OLINE_SUCCESS, flags, nick);
            anope_cmd_global(s_OperServ, "\2%s\2 used OLINE for %s",
                             u->nick, nick);
        } else if (u2 && flags[0] == '-') {
            anope_cmd_svso(s_OperServ, nick, flags);
            notice_lang(s_OperServ, u, OPER_OLINE_SUCCESS, flags, nick);
            anope_cmd_global(s_OperServ, "\2%s\2 used OLINE for %s",
                             u->nick, nick);
        } else
            syntax_error(s_OperServ, u, "OLINE", OPER_OLINE_SYNTAX);
    }
    return MOD_CONT;
}
