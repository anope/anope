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

int do_forbid(User * u);
void myNickServHelp(User * u);
NickAlias *makenick(const char *nick);

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

    c = createCommand("FORBID", do_forbid, is_services_admin, -1, -1, -1,
                      NICK_SERVADMIN_HELP_FORBID,
                      NICK_SERVADMIN_HELP_FORBID);
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
    if (is_services_admin(u)) {
        notice_lang(s_NickServ, u, NICK_HELP_CMD_FORBID);
    }
}

/**
 * The /ns forbid command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_forbid(User * u)
{
    NickAlias *na;
    char *nick = strtok(NULL, " ");
    char *reason = strtok(NULL, "");

    /* Assumes that permission checking has already been done. */
    if (!nick || (ForceForbidReason && !reason)) {
        syntax_error(s_NickServ, u, "FORBID",
                     (ForceForbidReason ? NICK_FORBID_SYNTAX_REASON :
                      NICK_FORBID_SYNTAX));
        return MOD_CONT;
    }

    if (readonly)
        notice_lang(s_NickServ, u, READ_ONLY_MODE);
    if (!anope_valid_nick(nick)) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
        return MOD_CONT;
    }
    if ((na = findnick(nick)) != NULL) {
        if (NSSecureAdmins && nick_is_services_admin(na->nc)
            && !is_services_root(u)) {
            notice_lang(s_NickServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }
        delnick(na);
    }
    na = makenick(nick);
    if (na) {
        na->status |= NS_VERBOTEN;
        na->last_usermask = sstrdup(u->nick);
        if (reason)
            na->last_realname = sstrdup(reason);

        na->u = finduser(na->nick);
        if (na->u)
            na->u->na = na;

        if (na->u) {
            notice_lang(s_NickServ, na->u, FORCENICKCHANGE_NOW);
            collide(na, 0);
        }


        if (ircd->sqline) {
            anope_cmd_sqline(na->nick, ((reason) ? reason : "Forbidden"));
        }

        if (WallForbid)
            anope_cmd_global(s_NickServ, "\2%s\2 used FORBID on \2%s\2",
                             u->nick, nick);

        alog("%s: %s set FORBID for nick %s", s_NickServ, u->nick, nick);
        notice_lang(s_NickServ, u, NICK_FORBID_SUCCEEDED, nick);
        send_event(EVENT_NICK_FORBIDDEN, 1, nick);
    } else {
        alog("%s: Valid FORBID for %s by %s failed", s_NickServ, nick,
             u->nick);
        notice_lang(s_NickServ, u, NICK_FORBID_FAILED, nick);
    }
    return MOD_CONT;
}

NickAlias *makenick(const char *nick)
{
    NickAlias *na;
    NickCore *nc;

    /* First make the core */
    nc = scalloc(1, sizeof(NickCore));
    nc->display = sstrdup(nick);
    slist_init(&nc->aliases);
    insert_core(nc);
    alog("%s: group %s has been created", s_NickServ, nc->display);

    /* Then make the alias */
    na = scalloc(1, sizeof(NickAlias));
    na->nick = sstrdup(nick);
    na->nc = nc;
    slist_add(&nc->aliases, na);
    alpha_insert_alias(na);
    return na;
}
