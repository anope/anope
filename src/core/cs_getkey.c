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

int do_getkey(User * u);
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

    c = createCommand("GETKEY", do_getkey, NULL, CHAN_HELP_GETKEY, -1, -1,
                      -1, -1);
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
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_GETKEY);
}

/**
 * The /cs getkey command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_getkey(User * u)
{
    char *chan = strtok(NULL, " ");
    ChannelInfo *ci;

    if (!chan) {
        syntax_error(s_ChanServ, u, "GETKEY", CHAN_GETKEY_SYNTAX);
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!check_access(u, ci, CA_GETKEY)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (!ci->c || !ci->c->key) {
        notice_lang(s_ChanServ, u, CHAN_GETKEY_NOKEY, chan);
    } else {
        alog("%s: %s!%s@%s used GETKEY on %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_GETKEY_KEY, chan, ci->c->key);
    }

    return MOD_CONT;
}
