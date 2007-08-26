/* NickServ core functions
 *
 * (C) 2003-2007 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

#define TO_COLLIDE   0          /* Collide the user with this nick */
#define TO_RELEASE   1          /* Release a collided nick */

int do_logout(User * u);
void myNickServHelp(User * u);

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
    moduleAddVersion("$Id$");
    moduleSetType(CORE);

    c = createCommand("LOGOUT", do_logout, NULL, -1, NICK_HELP_LOGOUT, -1,
                      NICK_SERVADMIN_HELP_LOGOUT,
                      NICK_SERVADMIN_HELP_LOGOUT);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

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
    notice_lang(s_NickServ, u, NICK_HELP_CMD_LOGOUT);
}

/**
 * The /ns logout command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_logout(User * u)
{
    char *nick = strtok(NULL, " ");
    char *param = strtok(NULL, " ");
    User *u2;

    if (!is_services_admin(u) && nick) {
        syntax_error(s_NickServ, u, "LOGOUT", NICK_LOGOUT_SYNTAX);
    } else if (!(u2 = (nick ? finduser(nick) : u))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (!u2->na) {
        if (nick)
            notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
        else
            notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
    } else if (u2->na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, u2->na->nick);
    } else if (!nick && !nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (nick && is_services_admin(u2)) {
        notice_lang(s_NickServ, u, NICK_LOGOUT_SERVICESADMIN, nick);
    } else {
        if (nick && param && !stricmp(param, "REVALIDATE")) {
            cancel_user(u2);
            validate_user(u2);
        } else {
            u2->na->status &= ~(NS_IDENTIFIED | NS_RECOGNIZED);
        }

        if (ircd->modeonreg) {
            common_svsmode(u2, ircd->modeonunreg, "1");
        }

        u->isSuperAdmin = 0;    /* Dont let people logout and remain a SuperAdmin */
        alog("%s: %s!%s@%s logged out nickname %s", s_NickServ, u->nick,
             u->username, u->host, u2->nick);

        if (nick)
            notice_lang(s_NickServ, u, NICK_LOGOUT_X_SUCCEEDED, nick);
        else
            notice_lang(s_NickServ, u, NICK_LOGOUT_SUCCEEDED);

        /* Stop nick tracking if enabled */
        if (NSNickTracking)
			/* Shouldn't this be u2? -GD */
            nsStopNickTracking(u);

        /* Clear any timers again */
        if (u->na->nc->flags & NI_KILLPROTECT) {
            del_ns_timeout(u->na, TO_COLLIDE);
        }
		
		/* Send out an event */
		send_event(EVENT_NICK_LOGOUT, 1, u2->nick);
    }
    return MOD_CONT;
}
