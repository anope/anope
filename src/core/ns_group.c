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

int do_group(User * u);
void myNickServHelp(User * u);
int do_glist(User * u);
int do_listlinks(User * u);

NickAlias *makealias(const char *nick, NickCore * nc);

/* Obsolete commands */
int do_link(User * u);

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

    c = createCommand("GROUP", do_group, NULL, NICK_HELP_GROUP, -1, -1, -1,
                      -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

    c = createCommand("LINK", do_link, NULL, -1, -1, -1, -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

    c = createCommand("GLIST", do_glist, NULL, -1, NICK_HELP_GLIST, -1,
                      NICK_SERVADMIN_HELP_GLIST,
                      NICK_SERVADMIN_HELP_GLIST);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

    c = createCommand("LISTLINKS", do_listlinks, NULL, -1, -1, -1, -1, -1);
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
    notice_lang(s_NickServ, u, NICK_HELP_CMD_GROUP);
    notice_lang(s_NickServ, u, NICK_HELP_CMD_GLIST);
}

/**
 * The /ns group command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
/* Register a nick in a specified group. */

int do_group(User * u)
{
    NickAlias *na, *target;
    NickCore *nc;
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    int i;
    char tsbuf[16];
    char modes[512];
    int len;

    if (NSEmailReg && (findrequestnick(u->nick))) {
        notice_lang(s_NickServ, u, NICK_REQUESTED);
        return MOD_CONT;
    }

    if (readonly) {
        notice_lang(s_NickServ, u, NICK_GROUP_DISABLED);
        return MOD_CONT;
    }
    if (checkDefCon(DEFCON_NO_NEW_NICKS)) {
        notice_lang(s_NickServ, u, OPER_DEFCON_DENIED);
        return MOD_CONT;
    }

    if (!anope_valid_nick(u->nick)) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, u->nick);
        return MOD_CONT;
    }

    if (RestrictOperNicks) {
        for (i = 0; i < RootNumber; i++) {
            if (stristr(u->nick, ServicesRoots[i]) && !is_oper(u)) {
                notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED,
                            u->nick);
                return MOD_CONT;
            }
        }
        for (i = 0; i < servadmins.count && (nc = servadmins.list[i]); i++) {
            if (stristr(u->nick, nc->display) && !is_oper(u)) {
                notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED,
                            u->nick);
                return MOD_CONT;
            }
        }
        for (i = 0; i < servopers.count && (nc = servopers.list[i]); i++) {
            if (stristr(u->nick, nc->display) && !is_oper(u)) {
                notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED,
                            u->nick);
                return MOD_CONT;
            }
        }
    }

    if (!nick || !pass) {
        syntax_error(s_NickServ, u, "GROUP", NICK_GROUP_SYNTAX);
    } else if (!(target = findnick(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (time(NULL) < u->lastnickreg + NSRegDelay) {
        notice_lang(s_NickServ, u, NICK_GROUP_PLEASE_WAIT, NSRegDelay);
    } else if (u->na && (u->na->status & NS_VERBOTEN)) {
        alog("%s: %s@%s tried to use GROUP from FORBIDden nick %s",
             s_NickServ, u->username, u->host, u->nick);
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, u->nick);
    } else if (u->na && (u->na->nc->flags & NI_SUSPENDED)) {
        alog("%s: %s!%s@%s tried to use GROUP from SUSPENDED nick %s",
             s_NickServ, u->nick, u->username, u->host, target->nick);
        notice_lang(s_NickServ, u, NICK_X_SUSPENDED, u->nick);
    } else if (u->na && NSNoGroupChange) {
        notice_lang(s_NickServ, u, NICK_GROUP_CHANGE_DISABLED, s_NickServ);
    } else if (u->na && !nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (target && (target->nc->flags & NI_SUSPENDED)) {
        alog("%s: %s!%s@%s tried to use GROUP from SUSPENDED nick %s",
             s_NickServ, u->nick, u->username, u->host, target->nick);
        notice_lang(s_NickServ, u, NICK_X_SUSPENDED, target->nick);
    } else if (target->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
    } else if (u->na && target->nc == u->na->nc) {
        notice_lang(s_NickServ, u, NICK_GROUP_SAME, target->nick);
    } else if (NSMaxAliases && (target->nc->aliases.count >= NSMaxAliases)
               && !nick_is_services_admin(target->nc)) {
        notice_lang(s_NickServ, u, NICK_GROUP_TOO_MANY, target->nick,
                    s_NickServ, s_NickServ);
    } else if (enc_check_password(pass, target->nc->pass) != 1) {
        alog("%s: Failed GROUP for %s!%s@%s (invalid password)",
             s_NickServ, u->nick, u->username, u->host);
        notice_lang(s_NickServ, u, PASSWORD_INCORRECT);
        bad_password(u);
    } else {
        /* If the nick is already registered, drop it.
         * If not, check that it is valid.
         */
        if (u->na) {
            delnick(u->na);
        } else {
            int prefixlen = strlen(NSGuestNickPrefix);
            int nicklen = strlen(u->nick);

            if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1
                && stristr(u->nick, NSGuestNickPrefix) == u->nick
                && strspn(u->nick + prefixlen,
                          "1234567890") == nicklen - prefixlen) {
                notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED,
                            u->nick);
                return MOD_CONT;
            }
        }
        na = makealias(u->nick, target->nc);

        if (na) {
            na->last_usermask =
                scalloc(strlen(common_get_vident(u)) +
                        strlen(common_get_vhost(u)) + 2, 1);
            sprintf(na->last_usermask, "%s@%s", common_get_vident(u),
                    common_get_vhost(u));
            na->last_realname = sstrdup(u->realname);
            na->time_registered = na->last_seen = time(NULL);
            na->status = (int16) (NS_IDENTIFIED | NS_RECOGNIZED);

            if (!(na->nc->flags & NI_SERVICES_ROOT)) {
                for (i = 0; i < RootNumber; i++) {
                    if (!stricmp(ServicesRoots[i], u->nick)) {
                        na->nc->flags |= NI_SERVICES_ROOT;
                        break;
                    }
                }
            }

            u->na = na;
            na->u = u;

#ifdef USE_RDB
            /* Is this really needed? Since this is a new alias it will get
             * its unique id on the next update, since it was previously
             * deleted by delnick. Must observe...
             */
            if (rdb_open()) {
                rdb_save_ns_alias(na);
                rdb_close();
            }
#endif
            send_event(EVENT_GROUP, 1, u->nick);
            alog("%s: %s!%s@%s makes %s join group of %s (%s) (e-mail: %s)", s_NickServ, u->nick, u->username, u->host, u->nick, target->nick, target->nc->display, (target->nc->email ? target->nc->email : "none"));
            notice_lang(s_NickServ, u, NICK_GROUP_JOINED, target->nick);

            u->lastnickreg = time(NULL);
            snprintf(tsbuf, sizeof(tsbuf), "%lu",
                     (unsigned long int) u->timestamp);
            if (ircd->modeonreg) {
                len = strlen(ircd->modeonreg);
                strncpy(modes,ircd->modeonreg,512);
	       if(ircd->rootmodeonid && is_services_root(u)) {
                    strncat(modes,ircd->rootmodeonid,512-len);
	        } else if(ircd->adminmodeonid && is_services_admin(u)) {
                    strncat(modes,ircd->adminmodeonid,512-len);
	        } else if(ircd->opermodeonid && is_services_oper(u)) {
                    strncat(modes,ircd->opermodeonid,512-len);
                }
                if (ircd->tsonmode) {
                    common_svsmode(u, modes, tsbuf);
                } else {
                    common_svsmode(u, modes, NULL);
                }
            }

            check_memos(u);

            /* Enable nick tracking if enabled */
            if (NSNickTracking)
                nsStartNickTracking(u);
        } else {
            alog("%s: makealias(%s) failed", s_NickServ, u->nick);
            notice_lang(s_NickServ, u, NICK_GROUP_FAILED);
        }
    }
    return MOD_CONT;
}


/* Creates a new alias in NickServ database. */

NickAlias *makealias(const char *nick, NickCore * nc)
{
    NickAlias *na;

    /* Just need to make the alias */
    na = scalloc(1, sizeof(NickAlias));
    na->nick = sstrdup(nick);
    na->nc = nc;
    slist_add(&nc->aliases, na);
    alpha_insert_alias(na);
    return na;
}


int do_link(User * u)
{
    notice_lang(s_NickServ, u, OBSOLETE_COMMAND, "GROUP");
    return MOD_CONT;
}

int do_glist(User * u)
{
    char *nick = strtok(NULL, " ");

    NickAlias *na, *na2;
    int is_servadmin = is_services_admin(u);
    int nick_ided = nick_identified(u);
    int i;

    if ((nick ? (stricmp(nick, u->nick) ? !is_servadmin : !nick_ided)
         : !nick_ided)) {
        notice_lang(s_NickServ, u,
                    (nick_ided ? ACCESS_DENIED :
                     NICK_IDENTIFY_REQUIRED), s_NickServ);
    } else if ((!nick ? !(na = u->na) : !(na = findnick(nick)))) {
        notice_lang(s_NickServ, u,
                    (!nick ? NICK_NOT_REGISTERED : NICK_X_NOT_REGISTERED),
                    nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else {
        time_t expt;
        struct tm *tm;
        char buf[BUFSIZE];
        int wont_expire;

        notice_lang(s_NickServ, u,
                    nick ? NICK_GLIST_HEADER_X : NICK_GLIST_HEADER,
                    na->nc->display);
        for (i = 0; i < na->nc->aliases.count; i++) {
            na2 = na->nc->aliases.list[i];
            if (na2->nc == na->nc) {
                if (!(wont_expire = na2->status & NS_NO_EXPIRE)) {
                    expt = na2->last_seen + NSExpire;
                    tm = localtime(&expt);
                    strftime_lang(buf, sizeof(buf), na2->u,
                                  STRFTIME_DATE_TIME_FORMAT, tm);
                }
                notice_lang(s_NickServ, u,
                            ((is_services_admin(u) && !wont_expire)
                             ? NICK_GLIST_REPLY_ADMIN : NICK_GLIST_REPLY),
                            (wont_expire ? '!' : ' '), na2->nick, buf);
            }
        }
        notice_lang(s_NickServ, u, NICK_GLIST_FOOTER,
                    na->nc->aliases.count);
    }
    return MOD_CONT;
}


int do_listlinks(User * u)
{
    notice_lang(s_NickServ, u, OBSOLETE_COMMAND, "GLIST");
    return MOD_CONT;
}
