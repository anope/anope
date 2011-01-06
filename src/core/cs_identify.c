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

int do_identify(User * u);
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

    c = createCommand("IDENTIFY", do_identify, NULL, CHAN_HELP_IDENTIFY,
                      -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);

    c = createCommand("ID", do_identify, NULL, CHAN_HELP_IDENTIFY, -1, -1,
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
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_IDENTIFY);
}

/**
 * The /cs command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_identify(User * u)
{
    char *chan = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    ChannelInfo *ci;
    struct u_chaninfolist *uc;

    if (!pass) {
        syntax_error(s_ChanServ, u, "IDENTIFY", CHAN_IDENTIFY_SYNTAX);
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!nick_identified(u)) {
        notice_lang(s_ChanServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (is_founder(u, ci)) {
        notice_lang(s_ChanServ, u, NICK_ALREADY_IDENTIFIED);
    } else {
        int res;

        if ((res = enc_check_password(pass, ci->founderpass)) == 1) {
            if (!is_identified(u, ci)) {
                uc = scalloc(sizeof(*uc), 1);
                uc->next = u->founder_chans;
                if (u->founder_chans)
                    u->founder_chans->prev = uc;
                u->founder_chans = uc;
                uc->chan = ci;
                alog("%s: %s!%s@%s identified for %s", s_ChanServ, u->nick,
                     u->username, u->host, ci->name);
            }

            notice_lang(s_ChanServ, u, CHAN_IDENTIFY_SUCCEEDED, chan);
        } else if (res < 0) {
            alog("%s: check_password failed for %s", s_ChanServ, ci->name);
            notice_lang(s_ChanServ, u, CHAN_IDENTIFY_FAILED);
        } else {
            alog("%s: Failed IDENTIFY for %s by %s!%s@%s",
                 s_ChanServ, ci->name, u->nick, u->username, u->host);
            notice_lang(s_ChanServ, u, PASSWORD_INCORRECT);
            bad_password(u);
        }

    }
    return MOD_CONT;
}
