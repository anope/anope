/* Routines to maintain a list of online users.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$
 *
 */

#include "services.h"

#define HASH(nick)	(((nick)[0]&31)<<5 | ((nick)[1]&31))
User *userlist[1024];

int32 usercnt = 0, opcnt = 0, maxusercnt = 0;
time_t maxusertime;

static unsigned long umodes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, UMODE_A, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3)
    UMODE_P,
#else
    0,
#endif
    0,
#if defined(IRC_BAHAMUT) || defined(IRC_ULTIMATE)
    UMODE_R,
#else
    0,
#endif
    0, 0, 0, 0, 0, 0, 0,
#ifdef IRC_ULTIMATE3
    UMODE_Z,
#else
    0,
#endif
    0, 0, 0, 0, 0,
    0, UMODE_a, 0, 0, 0, 0, 0,
#ifdef IRC_DREAMFORGE
    UMODE_g,
#else
    0,
#endif
    UMODE_h, UMODE_i, 0, 0, 0, 0, 0, UMODE_o,
#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3)
    UMODE_p,
#else
    0,
#endif
    0, UMODE_r, 0, 0, 0, 0, UMODE_w,
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_ULTIMATE3) || defined(IRC_VIAGRA) || defined(IRC_RAGE2)
    UMODE_x,
#else
    0,
#endif
    0,
    0,
    0, 0, 0, 0, 0
};

/*************************************************************************/
/*************************************************************************/

/* Allocate a new User structure, fill in basic values, link it to the
 * overall list, and return it.  Always successful.
 */

static User *new_user(const char *nick)
{
    User *user, **list;

    user = scalloc(sizeof(User), 1);
    if (!nick)
        nick = "";
    strscpy(user->nick, nick, NICKMAX);
    list = &userlist[HASH(user->nick)];
    user->next = *list;
    if (*list)
        (*list)->prev = user;
    *list = user;
    user->na = findnick(nick);
    if (user->na)
        user->na->u = user;
    usercnt++;
    if (usercnt > maxusercnt) {
        maxusercnt = usercnt;
        maxusertime = time(NULL);
        if (LogMaxUsers)
            alog("user: New maximum user count: %d", maxusercnt);
    }
    user->isSuperAdmin = 0;     /* always set SuperAdmin to 0 for new users */
    return user;
}

/*************************************************************************/

/* Change the nickname of a user, and move pointers as necessary. */

static void change_user_nick(User * user, const char *nick)
{
    User **list;
    int is_same = (!stricmp(user->nick, nick) ? 1 : 0);

    if (user->prev)
        user->prev->next = user->next;
    else
        userlist[HASH(user->nick)] = user->next;
    if (user->next)
        user->next->prev = user->prev;
    user->nick[1] = 0;          /* paranoia for zero-length nicks */
    strscpy(user->nick, nick, NICKMAX);
    list = &userlist[HASH(user->nick)];
    user->next = *list;
    user->prev = NULL;
    if (*list)
        (*list)->prev = user;
    *list = user;

    /* Only if old and new nick aren't the same; no need to waste time */
    if (!is_same) {
        if (user->na)
            user->na->u = NULL;
        user->na = findnick(nick);
        if (user->na)
            user->na->u = user;
    }
}

/*************************************************************************/

#ifdef HAS_VHOST

static void update_host(User * user)
{
    if (user->na && (nick_identified(user)
                     || (!(user->na->nc->flags & NI_SECURE)
                         && nick_recognized(user)))) {
        if (user->na->last_usermask)
            free(user->na->last_usermask);

        user->na->last_usermask =
            smalloc(strlen(GetIdent(user)) + strlen(GetHost(user)) + 2);
        sprintf(user->na->last_usermask, "%s@%s", GetIdent(user),
                GetHost(user));
    }

    if (debug)
        alog("debug: %s changes its host to %s", user->nick,
             GetHost(user));
}

#endif

/*************************************************************************/

#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_ULTIMATE3) || defined(IRC_VIAGRA) || defined(IRC_PTLINK) || defined(IRC_RAGE2)

/* Change the (virtual) hostname of a user. */

void change_user_host(User * user, const char *host)
{
    if (user->vhost)
        free(user->vhost);
    user->vhost = sstrdup(host);

    if (debug)
        alog("debug: %s changes its vhost to %s", user->nick, host);



    update_host(user);
}

#endif
/*************************************************************************/

#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_VIAGRA) || defined(IRC_PTLINK)
/* Change the realname of a user. */

void change_user_realname(User * user, const char *realname)
{
    if (user->realname)
        free(user->realname);
    user->realname = sstrdup(realname);

    if (user->na && (nick_identified(user)
                     || (!(user->na->nc->flags & NI_SECURE)
                         && nick_recognized(user)))) {
        if (user->na->last_realname)
            free(user->na->last_realname);
        user->na->last_realname = sstrdup(realname);
    }

    if (debug)
        alog("debug: %s changes its realname to %s", user->nick, realname);
}


/*************************************************************************/

/* Change the username of a user. */

void change_user_username(User * user, const char *username)
{
    if (user->username)
        free(user->username);
    user->username = sstrdup(username);
    if (user->na && (nick_identified(user)
                     || (!(user->na->nc->flags & NI_SECURE)
                         && nick_recognized(user)))) {
        if (user->na->last_usermask)
            free(user->na->last_usermask);

        user->na->last_usermask =
            smalloc(strlen(GetIdent(user)) + strlen(GetHost(user)) + 2);
        sprintf(user->na->last_usermask, "%s@%s", GetIdent(user),
                GetHost(user));
    }
    if (debug)
        alog("debug: %s changes its username to %s", user->nick, username);
}

#endif

/*************************************************************************/

void set_umode(User * user, int ac, char **av)
{
    int add = 1;                /* 1 if adding modes, 0 if deleting */
    char *modes = av[0];

    ac--;

    if (debug)
        alog("debug: Changing mode for %s to %s", user->nick, modes);

    while (*modes) {

        add ? (user->mode |= umodes[(int) *modes]) : (user->mode &=
                                                      ~umodes[(int)
                                                              *modes]);

        switch (*modes++) {
        case '+':
            add = 1;
            break;
        case '-':
            add = 0;
            break;
#if defined(IRC_BAHAMUT) && !defined(IRC_ULTIMATE3)
        case 'a':
            if (add && !is_services_admin(user)) {
                send_cmd(ServerName, "SVSMODE %s -a", user->nick);
                user->mode &= ~UMODE_a;
            }
            break;
#endif
#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3)
        case 'a':
            if (add && !is_services_oper(user)) {
                send_cmd(ServerName, "SVSMODE %s -a", user->nick);
                user->mode &= ~UMODE_a;
            }
            break;
        case 'P':
            if (add && !is_services_admin(user)) {
                send_cmd(ServerName, "SVSMODE %s -P", user->nick);
                user->mode &= ~UMODE_P;
            }
            break;
#endif
#if defined(IRC_ULTIMATE)
        case 'R':
            if (add && !is_services_root(user)) {
                send_cmd(ServerName, "SVSMODE %s -R", user->nick);
                user->mode &= ~UMODE_R;
            }
            break;
#endif
#if defined(IRC_ULTIMATE3)
        case 'Z':
            if (add && !is_services_root(user)) {
                send_cmd(ServerName, "SVSMODE %s -Z", user->nick);
                user->mode &= ~UMODE_Z;
            }
            break;
#endif
        case 'd':
            if (ac == 0) {
#if !defined(IRC_ULTIMATE) && !defined(IRC_UNREAL)
                alog("user: umode +d with no parameter (?) for user %s",
                     user->nick);
#endif
                break;
            }

            ac--;
            av++;
            user->svid = strtoul(*av, NULL, 0);
            break;
        case 'o':
            if (add) {
                opcnt++;

                if (WallOper)
                    wallops(s_OperServ, "\2%s\2 is now an IRC operator.",
                            user->nick);
                display_news(user, NEWS_OPER);
#if defined(IRC_PTLINK)
                if (is_services_admin(user)) {
                    send_cmd(ServerName, "SVSMODE %s +a", user->nick);
                    user->mode |= UMODE_a;
                }
#endif
#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3) || defined(IRC_RAGE2)
                if (is_services_oper(user)) {
                    send_cmd(ServerName, "SVSMODE %s +a", user->nick);
                    user->mode |= UMODE_a;
                }
#endif

#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3)
                if (is_services_admin(user)) {
                    send_cmd(ServerName, "SVSMODE %s +P", user->nick);
                    user->mode |= UMODE_P;
                }
#endif

#ifdef IRC_ULTIMATE
                if (is_services_root(user)) {
                    send_cmd(ServerName, "SVSMODE %s +R", user->nick);
                    user->mode |= UMODE_R;
                }
#endif
#ifdef IRC_ULTIMATE3
                if (is_services_root(user)) {
                    send_cmd(ServerName, "SVSMODE %s +Z", user->nick);
                    user->mode |= UMODE_Z;
                }
#endif
            } else {
                opcnt--;
            }
            break;
        case 'r':
            if (add && !nick_identified(user)) {
                send_cmd(ServerName, "SVSMODE %s -r", user->nick);
                user->mode &= ~UMODE_r;
            }
            break;
#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_ULTIMATE3) || defined(IRC_VIAGRA) || defined(IRC_RAGE2)
        case 'x':
            update_host(user);
            break;
#endif
        }
    }
}

/*************************************************************************/

/* Remove and free a User structure. */

void delete_user(User * user)
{
    struct u_chanlist *c, *c2;
    struct u_chaninfolist *ci, *ci2;

    if (LogUsers) {
#ifdef HAS_VHOST
        alog("LOGUSERS: %s (%s@%s => %s) (%s) left the network (%s).",
             user->nick, user->username, user->host,
             (user->vhost ? user->vhost : "(none)"), user->realname,
             user->server->name);
#else
        alog("LOGUSERS: %s (%s@%s) (%s) left the network (%s).",
             user->nick, user->username, user->host,
             user->realname, user->server->name);
#endif
    }

    if (debug >= 2)
        alog("debug: delete_user() called");
    usercnt--;
    if (is_oper(user))
        opcnt--;
    if (debug >= 2)
        alog("debug: delete_user(): free user data");
    free(user->username);
    free(user->host);
#ifdef HAS_VHOST
    if (user->vhost)
        free(user->vhost);
#endif
    free(user->realname);
    if (debug >= 2)
        alog("debug: delete_user(): remove from channels");
    c = user->chans;
    while (c) {
        c2 = c->next;
        chan_deluser(user, c->chan);
        free(c);
        c = c2;
    }
    /* This called only here now */
    cancel_user(user);
    if (user->na)
        user->na->u = NULL;
    if (debug >= 2)
        alog("debug: delete_user(): free founder data");
    ci = user->founder_chans;
    while (ci) {
        ci2 = ci->next;
        free(ci);
        ci = ci2;
    }

    moduleCleanStruct(user->moduleData);

    if (debug >= 2)
        alog("debug: delete_user(): delete from list");
    if (user->prev)
        user->prev->next = user->next;
    else
        userlist[HASH(user->nick)] = user->next;
    if (user->next)
        user->next->prev = user->prev;
    if (debug >= 2)
        alog("debug: delete_user(): free user structure");
    free(user);
    if (debug >= 2)
        alog("debug: delete_user() done");
}

/*************************************************************************/
/*************************************************************************/

/* Return statistics.  Pointers are assumed to be valid. */

void get_user_stats(long *nusers, long *memuse)
{
    long count = 0, mem = 0;
    int i;
    User *user;
    struct u_chanlist *uc;
    struct u_chaninfolist *uci;

    for (i = 0; i < 1024; i++) {
        for (user = userlist[i]; user; user = user->next) {
            count++;
            mem += sizeof(*user);
            if (user->username)
                mem += strlen(user->username) + 1;
            if (user->host)
                mem += strlen(user->host) + 1;
#ifdef HAS_VHOST
            if (user->vhost)
                mem += strlen(user->vhost) + 1;
#endif
            if (user->realname)
                mem += strlen(user->realname) + 1;
            if (user->server->name)
                mem += strlen(user->server->name) + 1;
            for (uc = user->chans; uc; uc = uc->next)
                mem += sizeof(*uc);
            for (uci = user->founder_chans; uci; uci = uci->next)
                mem += sizeof(*uci);
        }
    }
    *nusers = count;
    *memuse = mem;
}

/*************************************************************************/

/* Find a user by nick.  Return NULL if user could not be found. */

User *finduser(const char *nick)
{
    User *user;

    if (debug >= 3)
        alog("debug: finduser(%p)", nick);
    user = userlist[HASH(nick)];
    while (user && stricmp(user->nick, nick) != 0)
        user = user->next;
    if (debug >= 3)
        alog("debug: finduser(%s) -> %p", nick, user);
    return user;
}

/*************************************************************************/

/* Iterate over all users in the user list.  Return NULL at end of list. */

static User *current;
static int next_index;

User *firstuser(void)
{
    next_index = 0;
    while (next_index < 1024 && current == NULL)
        current = userlist[next_index++];
    if (debug)
        alog("debug: firstuser() returning %s",
             current ? current->nick : "NULL (end of list)");
    return current;
}

User *nextuser(void)
{
    if (current)
        current = current->next;
    if (!current && next_index < 1024) {
        while (next_index < 1024 && current == NULL)
            current = userlist[next_index++];
    }
    if (debug)
        alog("debug: nextuser() returning %s",
             current ? current->nick : "NULL (end of list)");
    return current;
}

/*************************************************************************/
/*************************************************************************/

/* Handle a server NICK command. */

User *do_nick(const char *source, char *nick, char *username, char *host,
              char *server, char *realname, time_t ts, uint32 svid, ...)
{
    User *user;

    char *tmp = NULL;
    NickAlias *old_na;          /* Old nick rec */
    int nc_changed = 1;         /* Did nick core change? */
    int status = 0;             /* Status to apply */
    char mask[USERMAX + HOSTMAX + 2];

    if (!*source) {
#ifdef HAS_NICKIP
        char ipbuf[16];
        struct in_addr addr;
        uint32 ip;
#endif
#ifdef HAS_VHOST
        char *vhost = NULL;
#endif
#if defined(HAS_NICKIP) || defined(HAS_NICKVHOST)
        va_list args;
        va_start(args, svid);
#endif

#ifdef HAS_NICKIP
        ip = va_arg(args, uint32);
#endif

#ifdef HAS_NICKVHOST
        vhost = va_arg(args, char *);
        if (!strcmp(vhost, "*")) {
            vhost = NULL;
            if (debug)
                alog("debug: new user with no vhost in NICK command: %s",
                     nick);
        }
#endif

        /* This is a new user; create a User structure for it. */
        if (debug)
            alog("debug: new user: %s", nick);

        if (LogUsers) {
        /**
	 * Ugly swap routine for Flop's bug :)
 	 **/
            tmp = strchr(realname, '%');
            while (tmp) {
                *tmp = '-';
                tmp = strchr(realname, '%');
            }
        /**
	 * End of ugly swap
	 **/

#ifdef HAS_NICKIP
            addr.s_addr = htonl(ip);
            ntoa(addr, ipbuf, sizeof(ipbuf));
#endif

#ifdef HAS_NICKVHOST
# ifdef HAS_NICKIP
            alog("LOGUSERS: %s (%s@%s => %s) (%s) [%s] connected to the network (%s).", nick, username, host, vhost, realname, ipbuf, server);
# else
            alog("LOGUSERS: %s (%s@%s => %s) (%s) connected to the network (%s).", nick, username, host, vhost, realname, server);
# endif
#else
# ifdef HAS_NICKIP
            alog("LOGUSERS: %s (%s@%s) (%s) [%s] connected to the network (%s).", nick, username, host, realname, ipbuf, server);
# else
            alog("LOGUSERS: %s (%s@%s) (%s) connected to the network (%s).", nick, username, host, realname, server);
# endif
#endif
        }

        /* We used to ignore the ~ which a lot of ircd's use to indicate no
         * identd response.  That caused channel bans to break, so now we
         * just take what the server gives us.  People are still encouraged
         * to read the RFCs and stop doing anything to usernames depending
         * on the result of an identd lookup.
         */

        /* First check for AKILLs. */
        /* DONT just return null if its an akill match anymore - yes its more efficent to, however, now that ircd's are
         * starting to use things like E/F lines, we cant be 100% sure the client will be removed from the network :/
         * as such, create a user_struct, and if the client is removed, we'll delete it again when the QUIT notice
         * comes in from the ircd.
         **/
        if (check_akill(nick, username, host,
#ifdef HAS_NICKVHOST
                        vhost,
#else
                        NULL,
#endif
#ifdef HAS_NICKIP
                        ipbuf)) {
#else
                        NULL)) {
#endif
/*            return NULL; */
        }

/**
 * DefCon AKILL system, if we want to akill all connecting user's here's where to do it
 * then force check_akill again on them...
 **/
        if (checkDefCon(DEFCON_AKILL_NEW_CLIENTS)) {
            strncpy(mask, "*@", 3);
            strncat(mask, host, HOSTMAX);
            alog("DEFCON: adding akill for %s", mask);
            add_akill(NULL, mask, s_OperServ,
                      time(NULL) + dotime(DefConAKILL),
                      DefConAkillReason ? DefConAkillReason :
                      "DEFCON AKILL");
            if (check_akill(nick, username, host,
#ifdef HAS_NICKVHOST
                            vhost,
#else
                            NULL,
#endif
#ifdef HAS_NICKIP
                            ipbuf)) {
#else
                            NULL)) {
#endif
/*            return NULL; */
            }
        }
#ifdef IRC_BAHAMUT
        /* Next for SGLINEs */
        if (check_sgline(nick, realname))
            return NULL;
#endif

        /* And for SQLINEs */
        if (check_sqline(nick, 0))
            return NULL;

#ifndef STREAMLINED
        /* Now check for session limits */
        if (LimitSessions && !add_session(nick, host))
            return NULL;
#endif

        /* And finally, for proxy ;) */
#ifdef USE_THREADS
# ifdef HAS_NICKIP
        if (ProxyDetect && proxy_check(nick, host, ip))
# else
        if (ProxyDetect && proxy_check(nick, host, 0))
# endif
            return NULL;
#endif

        /* Allocate User structure and fill it in. */
        user = new_user(nick);
        user->username = sstrdup(username);
        user->host = sstrdup(host);
        user->server = findserver(servlist, server);
        user->realname = sstrdup(realname);
        user->timestamp = ts;
        user->my_signon = time(NULL);

#ifdef HAS_VHOST
        user->vhost = vhost ? sstrdup(vhost) : sstrdup(host);
#endif

        if (CheckClones) {
            /* Check to see if it looks like clones. */
            check_clones(user);
        }

        if (svid == 0) {
            display_news(user, NEWS_LOGON);
            display_news(user, NEWS_RANDOM);
        }

        if (svid == ts && user->na) {
            /* Timestamp and svid match, and nick is registered; automagically identify the nick */
            user->svid = svid;
            user->na->status |= NS_IDENTIFIED;
            check_memos(user);
            nc_changed = 0;

            /* Start nick tracking if available */
            if (NSNickTracking)
                nsStartNickTracking(user);

        } else if (svid != 1) {
            /* Resets the svid because it doesn't match */
            user->svid = 1;
#ifdef IRC_BAHAMUT
            send_cmd(ServerName, "SVSMODE %s %lu +d 1", user->nick,
                     user->timestamp);
#else
#ifndef IRC_PTLINK
            send_cmd(ServerName, "SVSMODE %s +d 1", user->nick);
#endif
#endif
        } else {
            user->svid = 1;
        }

    } else {
        /* An old user changing nicks. */
        user = finduser(source);
        if (!user) {
            alog("user: NICK from nonexistent nick %s", source);
            return NULL;
        }
        user->isSuperAdmin = 0; /* Dont let people nick change and stay SuperAdmins */
        if (debug)
            alog("debug: %s changes nick to %s", source, nick);

        if (LogUsers) {
#ifdef HAS_VHOST
            alog("LOGUSERS: %s (%s@%s => %s) (%s) changed his nick to %s (%s).", user->nick, user->username, user->host, (user->vhost ? user->vhost : "(none)"), user->realname, nick, user->server->name);
#else
            alog("LOGUSERS: %s (%s@%s) (%s) changed his nick to %s (%s).",
                 user->nick, user->username, user->host,
                 user->realname, nick, user->server->name);
#endif
        }

        user->timestamp = ts;

        if (stricmp(nick, user->nick) == 0) {
            /* No need to redo things */
            change_user_nick(user, nick);
            nc_changed = 0;
        } else {
            /* Update this only if nicks aren't the same */
            user->my_signon = time(NULL);

            old_na = user->na;
            if (old_na) {
                if (nick_recognized(user))
                    user->na->last_seen = time(NULL);
                status = old_na->status & NS_TRANSGROUP;
                cancel_user(user);
            }

            change_user_nick(user, nick);

            if ((old_na ? old_na->nc : NULL) ==
                (user->na ? user->na->nc : NULL))
                nc_changed = 0;

            if (!nc_changed && (user->na))
                user->na->status |= status;
            else {
#if !defined(IRC_BAHAMUT) && !defined(IRC_PTLINK)
                /* Because on Bahamut it would be already -r */
                change_user_mode(user, "-r+d", "1");
#else
                change_user_mode(user, "+d", "1");
#endif
            }
        }

        if (!is_oper(user) && check_sqline(user->nick, 1))
            return NULL;

    }                           /* if (!*source) */

    /* Check for nick tracking to bypass identification */
    if (NSNickTracking && nsCheckNickTracking(user)) {
        user->na->status |= NS_IDENTIFIED;
        nc_changed = 0;
    }

    if (nc_changed || !nick_recognized(user)) {
        if (validate_user(user))
            check_memos(user);
    } else {
        if (nick_identified(user)) {
            user->na->last_seen = time(NULL);

            if (user->na->last_usermask)
                free(user->na->last_usermask);
            user->na->last_usermask =
                smalloc(strlen(GetIdent(user)) + strlen(GetHost(user)) +
                        2);
            sprintf(user->na->last_usermask, "%s@%s", GetIdent(user),
                    GetHost(user));

#ifdef IRC_PTLINK
            change_user_mode(user, "+r", NULL);
#endif

#if !defined(IRC_BAHAMUT) && !defined(IRC_PTLINK)
            if (user->svid != user->timestamp) {
                char tsbuf[16];
                snprintf(tsbuf, sizeof(tsbuf), "%lu", user->timestamp);
                change_user_mode(user, "+rd", tsbuf);
            } else {
                change_user_mode(user, "+r", NULL);
            }
#endif


            alog("%s: %s!%s@%s automatically identified for nick %s",
                 s_NickServ, user->nick, user->username, GetHost(user),
                 user->nick);
        }
    }

/* Bahamut sets -r on every nick changes, so we must test it even if nc_changed == 0 */
#ifdef IRC_BAHAMUT
    if (nick_identified(user)) {
        if (user->svid != user->timestamp) {
            char tsbuf[16];
            snprintf(tsbuf, sizeof(tsbuf), "%lu", user->timestamp);
            change_user_mode(user, "+rd", tsbuf);
        } else {
            change_user_mode(user, "+r", NULL);
        }
    }
#endif

    return user;
}

/*************************************************************************/

/* Handle a MODE command for a user.
 *	av[0] = nick to change mode for
 *	av[1] = modes
 */

void do_umode(const char *source, int ac, char **av)
{
    User *user;

    if (stricmp(source, av[0]) != 0) {
        alog("user: MODE %s %s from different nick %s!", av[0], av[1],
             source);
        wallops(NULL, "%s attempted to change mode %s for %s", source,
                av[1], av[0]);
        return;
    }

    user = finduser(source);
    if (!user) {
        alog("user: MODE %s for nonexistent nick %s: %s", av[1], source,
             merge_args(ac, av));
        return;
    }

    set_umode(user, ac - 1, &av[1]);
}

/*************************************************************************/

/* Handle a QUIT command.
 *	av[0] = reason
 */

void do_quit(const char *source, int ac, char **av)
{
    User *user;
    NickAlias *na;

    user = finduser(source);
    if (!user) {
        alog("user: QUIT from nonexistent user %s: %s", source,
             merge_args(ac, av));
        return;
    }
    if (debug)
        alog("debug: %s quits", source);
    if ((na = user->na) && (!(na->status & NS_VERBOTEN))
        && (na->status & (NS_IDENTIFIED | NS_RECOGNIZED))) {
        na->last_seen = time(NULL);
        if (na->last_quit)
            free(na->last_quit);
        na->last_quit = *av[0] ? sstrdup(av[0]) : NULL;
    }
#ifndef STREAMLINED
    if (LimitSessions)
        del_session(user->host);
#endif
    delete_user(user);
}

/*************************************************************************/

/* Handle a KILL command.
 *	av[0] = nick being killed
 *	av[1] = reason
 */

void do_kill(const char *source, int ac, char **av)
{
    User *user;
    NickAlias *na;

    user = finduser(av[0]);
    if (!user)
        return;
    if (debug)
        alog("debug: %s killed", av[0]);
    if ((na = user->na) && (!(na->status & NS_VERBOTEN))
        && (na->status & (NS_IDENTIFIED | NS_RECOGNIZED))) {
        na->last_seen = time(NULL);
        if (na->last_quit)
            free(na->last_quit);
        na->last_quit = *av[1] ? sstrdup(av[1]) : NULL;

    }
#ifndef STREAMLINED
    if (LimitSessions)
        del_session(user->host);
#endif
    delete_user(user);
}

/*************************************************************************/
/*************************************************************************/

#if defined(IRC_ULTIMATE) || defined(IRC_ULTIMATE3)

/* Is the given user protected from kicks and negative mode changes? */

int is_protected(User * user)
{
    return (user->mode & UMODE_p);
}
#endif

/*************************************************************************/

/* Is the given nick an oper? */

int is_oper(User * user)
{
    return (user->mode & UMODE_o);
}

/*************************************************************************/
/*************************************************************************/

#ifdef HAS_EXCEPT
/* Is the given user ban-excepted? */
int is_excepted(ChannelInfo * ci, User * user)
{
    int count, i;
    int isexcepted = 0;
    char **excepts;

    if (!ci->c)
        return 0;

    count = ci->c->exceptcount;
    excepts = scalloc(sizeof(char *) * count, 1);
    memcpy(excepts, ci->c->excepts, sizeof(char *) * count);

    for (i = 0; i < count; i++) {
        if (match_usermask(excepts[i], user)) {
            isexcepted = 1;
        }
    }
    free(excepts);
    return isexcepted;
}

/*************************************************************************/

/* Is the given MASK ban-excepted? */
int is_excepted_mask(ChannelInfo * ci, char *mask)
{
    int count, i;
    int isexcepted = 0;
    char **excepts;

    if (!ci->c)
        return 0;

    count = ci->c->exceptcount;
    excepts = scalloc(sizeof(char *) * count, 1);
    memcpy(excepts, ci->c->excepts, sizeof(char *) * count);

    for (i = 0; i < count; i++) {
        if (match_wild_nocase(excepts[i], mask)) {
            isexcepted = 1;
        }
    }
    free(excepts);
    return isexcepted;
}

#endif
/*************************************************************************/

/* Does the user's usermask match the given mask (either nick!user@host or
 * just user@host)?
 */

int match_usermask(const char *mask, User * user)
{
    char *mask2 = sstrdup(mask);
    char *nick, *username, *host;
    int result;

    if (strchr(mask2, '!')) {
        nick = strtok(mask2, "!");
        username = strtok(NULL, "@");
    } else {
        nick = NULL;
        username = strtok(mask2, "@");
    }
    host = strtok(NULL, "");
    if (!username || !host) {
        free(mask2);
        return 0;
    }

    if (nick) {
        result = match_wild_nocase(nick, user->nick)
            && match_wild_nocase(username, user->username)
            && (match_wild_nocase(host, user->host)
#ifdef HAS_VHOST
                || match_wild_nocase(host, user->vhost)
#endif
            );
    } else {
        result = match_wild_nocase(username, user->username)
            && (match_wild_nocase(host, user->host)
#ifdef HAS_VHOST
                || match_wild_nocase(host, user->vhost)
#endif
            );
    }

    free(mask2);
    return result;
}

/*************************************************************************/

/* Split a usermask up into its constitutent parts.  Returned strings are
 * malloc()'d, and should be free()'d when done with.  Returns "*" for
 * missing parts.
 */

void split_usermask(const char *mask, char **nick, char **user,
                    char **host)
{
    char *mask2 = sstrdup(mask);

    *nick = strtok(mask2, "!");
    *user = strtok(NULL, "@");
    *host = strtok(NULL, "");
    /* Handle special case: mask == user@host */
    if (*nick && !*user && strchr(*nick, '@')) {
        *nick = NULL;
        *user = strtok(mask2, "@");
        *host = strtok(NULL, "");
    }
    if (!*nick)
        *nick = "*";
    if (!*user)
        *user = "*";
    if (!*host)
        *host = "*";
    *nick = sstrdup(*nick);
    *user = sstrdup(*user);
    *host = sstrdup(*host);
    free(mask2);
}

/*************************************************************************/

/* Given a user, return a mask that will most likely match any address the
 * user will have from that location.  For IP addresses, wildcards the
 * appropriate subnet mask (e.g. 35.1.1.1 -> 35.*; 128.2.1.1 -> 128.2.*);
 * for named addresses, wildcards the leftmost part of the name unless the
 * name only contains two parts.  If the username begins with a ~, delete
 * it.  The returned character string is malloc'd and should be free'd
 * when done with.
 */

char *create_mask(User * u)
{
    char *mask, *s, *end;
    int ulen = strlen(GetIdent(u));

    /* Get us a buffer the size of the username plus hostname.  The result
     * will never be longer than this (and will often be shorter), thus we
     * can use strcpy() and sprintf() safely.
     */
    end = mask = smalloc(ulen + strlen(GetHost(u)) + 3);
    end += sprintf(end, "%s%s@",
                   (ulen <
                    (*(GetIdent(u)) ==
                     '~' ? USERMAX + 1 : USERMAX) ? "*" : ""),
                   (*(GetIdent(u)) ==
                    '~' ? GetIdent(u) + 1 : GetIdent(u)));

    if (strspn(GetHost(u), "0123456789.") == strlen(GetHost(u))
        && (s = strchr(GetHost(u), '.'))
        && (s = strchr(s + 1, '.'))
        && (s = strchr(s + 1, '.'))
        && (!strchr(s + 1, '.'))) {     /* IP addr */
        s = sstrdup(GetHost(u));
        *strrchr(s, '.') = 0;

        sprintf(end, "%s.*", s);
        free(s);
    } else {
        if ((s = strchr(GetHost(u), '.')) && strchr(s + 1, '.')) {
            s = sstrdup(strchr(GetHost(u), '.') - 1);
            *s = '*';
            strcpy(end, s);
            free(s);
        } else {
            strcpy(end, GetHost(u));
        }
    }
    return mask;
}

/*************************************************************************/
