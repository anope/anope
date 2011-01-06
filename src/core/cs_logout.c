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

int do_logout(User * u);
void myChanServHelp(User * u);
void make_unidentified(User * u, ChannelInfo * ci);

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

    c = createCommand("LOGOUT", do_logout, NULL, -1, CHAN_HELP_LOGOUT, -1,
                      CHAN_SERVADMIN_HELP_LOGOUT,
                      CHAN_SERVADMIN_HELP_LOGOUT);
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
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_LOGOUT);
}

/**
 * The /cs command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_logout(User * u)
{
    char *chan = strtok(NULL, " ");
    char *nick = strtok(NULL, " ");
    ChannelInfo *ci;
    User *u2 = NULL;
    int is_servadmin = is_services_admin(u);

    if (!chan || (!nick && !is_servadmin)) {
        syntax_error(s_ChanServ, u, "LOGOUT",
                     (!is_servadmin ? CHAN_LOGOUT_SYNTAX :
                      CHAN_LOGOUT_SERVADMIN_SYNTAX));
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (!is_servadmin && (ci->flags & CI_VERBOTEN)) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (nick && !(u2 = finduser(nick))) {
        notice_lang(s_ChanServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (!is_servadmin && u2 != u && !is_real_founder(u, ci)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
	/* Since founders can not logout we should tell them -katsklaw */
    } else if (u2 == u && is_real_founder(u, ci)) {
	notice_lang(s_ChanServ, u, CHAN_LOGOUT_FOUNDER_FAILED, chan);
    } else {
        if (u2) {
            make_unidentified(u2, ci);
            notice_lang(s_ChanServ, u, CHAN_LOGOUT_SUCCEEDED, nick, chan);
            alog("%s: User %s!%s@%s has been logged out of channel %s.",
                 s_ChanServ, u2->nick, u2->username, u2->host, chan);
        } else {
            int i;
            for (i = 0; i < 1024; i++)
                for (u2 = userlist[i]; u2; u2 = u2->next)
                    make_unidentified(u2, ci);
            notice_lang(s_ChanServ, u, CHAN_LOGOUT_ALL_SUCCEEDED, chan);
            alog("%s: All users identified have been logged out of channel %s.", s_ChanServ, chan);
        }

    }
    return MOD_CONT;
}

void make_unidentified(User * u, ChannelInfo * ci)
{
    struct u_chaninfolist *uci;

    if (!u || !ci)
        return;

    for (uci = u->founder_chans; uci; uci = uci->next) {
        if (uci->chan == ci) {
            if (uci->next)
                uci->next->prev = uci->prev;
            if (uci->prev)
                uci->prev->next = uci->next;
            else
                u->founder_chans = uci->next;
            free(uci);
            break;
        }
    }
}
