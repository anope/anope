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

void myChanServHelp(User * u);
int do_invite(User * u);

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

    c = createCommand("INVITE", do_invite, NULL, CHAN_HELP_INVITE, -1, -1,
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
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_INVITE);
}

/**
 * The /cs invite command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_invite(User * u)
{
    char *chan = strtok(NULL, " ");
    Channel *c;
    ChannelInfo *ci;

    if (!chan) {
        syntax_error(s_ChanServ, u, "INVITE", CHAN_INVITE_SYNTAX);
    } else if (!(c = findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (ci->flags & CI_SUSPENDED) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!u || !check_access(u, ci, CA_INVITE)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else {
        anope_cmd_invite(whosends(ci), chan, u->nick);
    }
    return MOD_CONT;
}
