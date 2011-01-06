/* ChanServ core functions
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

int do_status(User * u);
void myChanServHelp(User * u);

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

    c = createCommand("STATUS", do_status, is_services_admin, -1, -1, -1,
                      CHAN_SERVADMIN_HELP_STATUS,
                      CHAN_SERVADMIN_HELP_STATUS);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);

    moduleSetChanHelp(myChanServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
    if (is_services_admin(u)) {
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_STATUS);
    }
}

/**
 * The /cs status command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_status(User * u)
{
    ChannelInfo *ci;
    User *u2;
    char *nick, *chan;
    char *temp = NULL;

    chan = strtok(NULL, " ");
    nick = strtok(NULL, " ");
    if (!nick || strtok(NULL, " ")) {
        notice_lang(s_ChanServ, u, CHAN_STATUS_SYNTAX);
        return MOD_CONT;
    }
    if (!(ci = cs_findchan(chan))) {
        temp = chan;
        chan = nick;
        nick = temp;
        ci = cs_findchan(chan);
    }
    if (!ci) {
        notice_lang(s_ChanServ, u, CHAN_STATUS_NOT_REGGED, temp);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_STATUS_FORBIDDEN, chan);
        return MOD_CONT;
    } else if ((u2 = finduser(nick)) != NULL) {
        notice_lang(s_ChanServ, u, CHAN_STATUS_INFO, chan, nick,
                    get_access(u2, ci));
    } else {                    /* !u2 */
        notice_lang(s_ChanServ, u, CHAN_STATUS_NOTONLINE, nick);
    }
    return MOD_CONT;
}
