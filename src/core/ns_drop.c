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

int do_drop(User * u);
int do_unlink(User * u);
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
    moduleAddVersion(VERSION_STRING);
    moduleSetType(CORE);

    c = createCommand("DROP", do_drop, NULL, -1, NICK_HELP_DROP, -1,
                      NICK_SERVADMIN_HELP_DROP, NICK_SERVADMIN_HELP_DROP);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);
    c = createCommand("UNLINK", do_unlink, NULL, -1, -1, -1, -1, -1);
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
    notice_lang(s_NickServ, u, NICK_HELP_CMD_DROP);
}

/**
 * The /ns drop command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_drop(User * u)
{
    char *nick = strtok(NULL, " ");
    NickAlias *na;
    NickRequest *nr = NULL;
    int is_servadmin = is_services_admin(u);
    int is_mine;                /* Does the nick being dropped belong to the user that is dropping? */
    char *my_nick = NULL;

    if (readonly && !is_servadmin) {
        notice_lang(s_NickServ, u, NICK_DROP_DISABLED);
        return MOD_CONT;
    }

    if (!(na = (nick ? findnick(nick) : u->na))) {
        if (nick) {
            if ((nr = findrequestnick(nick)) && is_servadmin) {
                if (readonly)
                    notice_lang(s_NickServ, u, READ_ONLY_MODE);
                if (WallDrop)
                    anope_cmd_global(s_NickServ,
                                     "\2%s\2 used DROP on \2%s\2", u->nick,
                                     nick);
                alog("%s: %s!%s@%s dropped nickname %s (e-mail: %s)",
                     s_NickServ, u->nick, u->username, u->host,
                     nr->nick, nr->email);
                delnickrequest(nr);
                notice_lang(s_NickServ, u, NICK_X_DROPPED, nick);
            } else {
                notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
            }
        } else
            notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
        return MOD_CONT;
    }

    is_mine = (u->na && (u->na->nc == na->nc));
    if (is_mine && !nick)
        my_nick = sstrdup(na->nick);

    if (is_mine && !nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (!is_mine && !is_servadmin) {
        notice_lang(s_NickServ, u, ACCESS_DENIED);
    } else if (NSSecureAdmins && !is_mine && nick_is_services_admin(na->nc)
               && !is_services_root(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
    } else {
        if (readonly)
            notice_lang(s_NickServ, u, READ_ONLY_MODE);

        if (ircd->sqline && (na->status & NS_VERBOTEN)) {
            anope_cmd_unsqline(na->nick);
        }

        alog("%s: %s!%s@%s dropped nickname %s (group %s) (e-mail: %s)",
             s_NickServ, u->nick, u->username, u->host,
             na->nick, na->nc->display,
             (na->nc->email ? na->nc->email : "none"));
        delnick(na);
        send_event(EVENT_NICK_DROPPED, 1, (my_nick ? my_nick : nick));

        if (!is_mine) {
            if (WallDrop)
                anope_cmd_global(s_NickServ, "\2%s\2 used DROP on \2%s\2",
                                 u->nick, nick);
            notice_lang(s_NickServ, u, NICK_X_DROPPED, nick);
        } else {
            if (nick)
                notice_lang(s_NickServ, u, NICK_X_DROPPED, nick);
            else
                notice_lang(s_NickServ, u, NICK_DROPPED);
            if (my_nick) {
                free(my_nick);
            }
        }
    }
    return MOD_CONT;
}


int do_unlink(User * u)
{
    notice_lang(s_NickServ, u, OBSOLETE_COMMAND, "DROP");
    return MOD_CONT;
}
