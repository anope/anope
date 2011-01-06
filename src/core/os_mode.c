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

int do_os_mode(User * u);
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

    c = createCommand("MODE", do_os_mode, is_services_oper, OPER_HELP_MODE,
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
    if (is_services_oper(u)) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_MODE);
    }
}

/**
 * The /os mode command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_os_mode(User * u)
{
    int ac;
    char **av;
    char *chan = strtok(NULL, " "), *modes = strtok(NULL, "");
    Channel *c;

    if (!chan || !modes) {
        syntax_error(s_OperServ, u, "MODE", OPER_MODE_SYNTAX);
        return MOD_CONT;
    }

    if (!(c = findchan(chan))) {
        notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
        return MOD_CONT;
    } else if ((ircd->adminmode) && (!is_services_admin(u))
               && (c->mode & ircd->adminmode)) {
        notice_lang(s_OperServ, u, PERMISSION_DENIED);
        return MOD_CONT;
    } else {
        anope_cmd_mode(s_OperServ, chan, "%s", modes);

        ac = split_buf(modes, &av, 1);
        chan_set_modes(s_OperServ, c, ac, av, -1);
        free(av);

        if (WallOSMode)
            anope_cmd_global(s_OperServ, "%s used MODE %s on %s", u->nick,
                             modes, chan);
    }
    return MOD_CONT;
}
