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

int do_operumodes(User * u);
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

    c = createCommand("UMODE", do_operumodes, is_services_root,
                      OPER_HELP_UMODE, -1, -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);

    moduleSetOperHelp(myOperServHelp);

    if (!ircd->umode) {
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
        notice_lang(s_OperServ, u, OPER_HELP_CMD_UMODE);
    }
}

/**
 * Change any user's UMODES
 *
 * modified to be part of the SuperAdmin directive -jester
 * check user flag for SuperAdmin -rob
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 */
int do_operumodes(User * u)
{
    char *nick = strtok(NULL, " ");
    char *modes = strtok(NULL, "");

    User *u2;

    /* Only allow this if SuperAdmin is enabled */
    if (!u->isSuperAdmin) {
        notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ONLY);
        return MOD_CONT;
    }

    if (!nick || !modes) {
        syntax_error(s_OperServ, u, "UMODE", OPER_UMODE_SYNTAX);
        return MOD_CONT;
    }

    /**
     * Only accept a +/- mode string
     *-rob
     **/
    if ((modes[0] != '+') && (modes[0] != '-')) {
        syntax_error(s_OperServ, u, "UMODE", OPER_UMODE_SYNTAX);
        return MOD_CONT;
    }
    if (!(u2 = finduser(nick))) {
        notice_lang(s_OperServ, u, NICK_X_NOT_IN_USE, nick);
    } else {
        common_svsmode(u2, modes, NULL);

        notice_lang(s_OperServ, u, OPER_UMODE_SUCCESS, nick);
        notice_lang(s_OperServ, u2, OPER_UMODE_CHANGED, u->nick);

        if (WallOSMode)
            anope_cmd_global(s_OperServ, "\2%s\2 used UMODE on %s",
                             u->nick, nick);
    }
    return MOD_CONT;
}
