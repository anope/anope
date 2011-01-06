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

int do_os_kick(User * u);
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

    c = createCommand("KICK", do_os_kick, is_services_oper, OPER_HELP_KICK,
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
        notice_lang(s_OperServ, u, OPER_HELP_CMD_KICK);
    }
}

/**
 * The /os kick command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_os_kick(User * u)
{
    char *argv[3];
    char *chan, *nick, *s;
    Channel *c;

    chan = strtok(NULL, " ");
    nick = strtok(NULL, " ");
    s = strtok(NULL, "");
    if (!chan || !nick || !s) {
        syntax_error(s_OperServ, u, "KICK", OPER_KICK_SYNTAX);
        return MOD_CONT;
    }
    if (!(c = findchan(chan))) {
        notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
        return MOD_CONT;
    }
    anope_cmd_kick(s_OperServ, chan, nick, "%s (%s)", u->nick, s);
    if (WallOSKick)
        anope_cmd_global(s_OperServ, "%s used KICK on %s/%s", u->nick,
                         nick, chan);
    argv[0] = sstrdup(chan);
    argv[1] = sstrdup(nick);
    argv[2] = sstrdup(s);
    do_kick(s_OperServ, 3, argv);
    free(argv[2]);
    free(argv[1]);
    free(argv[0]);
    return MOD_CONT;
}
