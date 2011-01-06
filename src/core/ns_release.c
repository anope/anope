/* NickServ core functions
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

Command *c;

int do_release(User * u);
void myNickServHelp(User * u);
int myHelpResonse(User * u);

/**
 * Create the command, and tell anope about it.
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT to allow the module, MOD_STOP to stop it
 **/
int AnopeInit(int argc, char **argv)
{
    moduleAddAuthor("Anope");
    moduleAddVersion(VERSION_STRING);
    moduleSetType(CORE);

    c = createCommand("RELEASE", do_release, NULL, -1, -1, -1, -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    moduleAddHelp(c, myHelpResonse);

    moduleSetNickHelp(myNickServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User * u)
{
    notice_lang(s_NickServ, u, NICK_HELP_CMD_RELEASE);
}

/**
 * Show the extended help on the RELEASE command.
 * @param u The user who is requesting help
 **/
int myHelpResonse(User * u)
{
    char relstr[192];

    /* Convert NSReleaseTimeout seconds to string format */
    duration(u->na, relstr, sizeof(relstr), NSReleaseTimeout);
    
    notice_help(s_NickServ, u, NICK_HELP_RELEASE, relstr);
    do_help_limited(s_NickServ, u, c);

    return MOD_CONT;
}

/**
 * The /ns release command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_release(User * u)
{
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    NickAlias *na;

    if (!nick) {
        syntax_error(s_NickServ, u, "RELEASE", NICK_RELEASE_SYNTAX);
    } else if (!(na = findnick(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else if (na->nc->flags & NI_SUSPENDED) {
        notice_lang(s_NickServ, u, NICK_X_SUSPENDED, na->nick);
    } else if (!(na->status & NS_KILL_HELD)) {
        notice_lang(s_NickServ, u, NICK_RELEASE_NOT_HELD, nick);
    } else if (pass) {
        int res = enc_check_password(pass, na->nc->pass);
        if (res == 1) {
            release(na, 0);
            notice_lang(s_NickServ, u, NICK_RELEASED);
        } else {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
            if (res == 0) {
                alog("%s: RELEASE: invalid password for %s by %s!%s@%s",
                     s_NickServ, nick, u->nick, u->username, u->host);
                bad_password(u);
            }
        }
    } else {
        if (group_identified(u, na->nc)
            || (!(na->nc->flags & NI_SECURE) && is_on_access(u, na->nc))) {
            release(na, 0);
            notice_lang(s_NickServ, u, NICK_RELEASED);
        } else {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
        }
    }
    return MOD_CONT;
}

/* EOF */
