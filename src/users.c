/* Routines to maintain a list of online users.
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

#include "services.h"

#define HASH(nick)	(((nick)[0]&31)<<5 | ((nick)[1]&31))
User *userlist[1024];

#define HASH2(nick)	(((nick)[0]&31)<<5 | ((nick)[1]&31))
Uid *uidlist[1024];

int32 usercnt = 0, opcnt = 0;
uint32 maxusercnt = 0;
time_t maxusertime;

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
    user->nickTrack = NULL;     /* ensure no default tracking nick */
    return user;
}

/*************************************************************************/

/* Change the nickname of a user, and move pointers as necessary. */

static void change_user_nick(User * user, const char *nick)
{
    User **list;
    int is_same;

    /* Sanity check to make sure we don't segfault */
    if (!user || !nick || !*nick) {
        return;
    }

    is_same = (!stricmp(user->nick, nick) ? 1 : 0);

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

void update_host(User * user)
{
    if (user->na && (nick_identified(user)
                     || (!(user->na->nc->flags & NI_SECURE)
                         && nick_recognized(user)))) {
        if (user->na->last_usermask)
            free(user->na->last_usermask);

        user->na->last_usermask =
            smalloc(strlen(common_get_vident(user)) +
                    strlen(common_get_vhost(user)) + 2);
        sprintf(user->na->last_usermask, "%s@%s", common_get_vident(user),
                common_get_vhost(user));
    }

    if (debug)
        alog("debug: %s changes its host to %s", user->nick,
             common_get_vhost(user));
}


/*************************************************************************/

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

/*************************************************************************/

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
    if (user->vident)
        free(user->vident);
    user->vident = sstrdup(username);
    if (user->na && (nick_identified(user)
                     || (!(user->na->nc->flags & NI_SECURE)
                         && nick_recognized(user)))) {
        if (user->na->last_usermask)
            free(user->na->last_usermask);

        user->na->last_usermask =
            smalloc(strlen(common_get_vident(user)) +
                    strlen(common_get_vhost(user)) + 2);
        sprintf(user->na->last_usermask, "%s@%s", common_get_vident(user),
                common_get_vhost(user));
    }
    if (debug)
        alog("debug: %s changes its username to %s", user->nick, username);
}

/*************************************************************************/

/* Remove and free a User structure. */

void delete_user(User * user)
{
    struct u_chanlist *c, *c2;
    struct u_chaninfolist *ci, *ci2;
    char *realname;

    if (LogUsers) {
        realname = normalizeBuffer(user->realname);
        if (ircd->vhost) {
            alog("LOGUSERS: %s (%s@%s => %s) (%s) left the network (%s).",
                 user->nick, user->username, user->host,
                 (user->vhost ? user->vhost : "(none)"),
                 realname, user->server->name);
        } else {
            alog("LOGUSERS: %s (%s@%s) (%s) left the network (%s).",
                 user->nick, user->username, user->host,
                 realname, user->server->name);
        }
        free(realname);
    }
    send_event(EVENT_USER_LOGOFF, 1, user->nick);

    if (debug >= 2)
        alog("debug: delete_user() called");
    usercnt--;
    if (is_oper(user))
        opcnt--;
    if (debug >= 2)
        alog("debug: delete_user(): free user data");
    free(user->username);
    free(user->host);
    if (user->chost)
        free(user->chost);
    if (user->vhost)
        free(user->vhost);
    if (user->vident)
        free(user->vident);
    if (user->uid) {
        free(user->uid);
    }
    Anope_Free(user->realname);
    Anope_Free(user->hostip);
    if (debug >= 2) {
        alog("debug: delete_user(): remove from channels");
    }
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

    if (user->nickTrack)
        free(user->nickTrack);

    moduleCleanStruct(&user->moduleData);

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
            if (ircd->vhost) {
                if (user->vhost)
                    mem += strlen(user->vhost) + 1;
            }
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

    if (!nick || !*nick) {
        if (debug) {
            alog("debug: finduser() called with NULL values");
        }
        return NULL;
    }

    if (debug >= 3)
        alog("debug: finduser(%p)", nick);
    user = userlist[HASH(nick)];
    while (user && stricmp(user->nick, nick) != 0)
        user = user->next;
    if (debug >= 3)
        alog("debug: finduser(%s) -> 0x%p", nick, (void *) user);
    return user;
}


/*************************************************************************/

/* Iterate over all users in the user list.  Return NULL at end of list. */

static User *current;
static int next_index;

User *firstuser(void)
{
    next_index = 0;
    current = NULL;
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

User *find_byuid(const char *uid)
{
    User *u, *next;

    if (!uid) {
        if (debug)
            alog("debug: find_byuid() called with NULL-value");
        return NULL;
    }

    u = first_uid();
    while (u) {
        next = next_uid();
        if (u->uid) {
            if (!stricmp(uid, u->uid)) {
                return u;
            }
        }
        u = next;
    }
    return NULL;
}

static User *current_uid;
static int next_index_uid;

User *first_uid(void)
{
    next_index_uid = 0;
    current_uid = NULL;
    while (next_index_uid < 1024 && current_uid == NULL) {
        current_uid = userlist[next_index_uid++];
    }
    if (debug >= 2) {
        alog("debug: first_uid() returning %s %s",
             current_uid ? current_uid->nick : "NULL (end of list)",
             current_uid ? current_uid->uid : "");
    }
    return current_uid;
}

User *next_uid(void)
{
    if (current_uid)
        current_uid = current_uid->next;
    if (!current_uid && next_index_uid < 1024) {
        while (next_index_uid < 1024 && current_uid == NULL)
            current_uid = userlist[next_index_uid++];
    }
    if (debug >= 2) {
        alog("debug: next_uid() returning %s %s",
             current_uid ? current_uid->nick : "NULL (end of list)",
             current_uid ? current_uid->uid : "");
    }
    return current_uid;
}

Uid *new_uid(const char *nick, char *uid)
{
    Uid *u, **list;

    u = scalloc(sizeof(Uid), 1);
    if (!nick || !uid) {
        return NULL;
    }
    strscpy(u->nick, nick, NICKMAX);
    list = &uidlist[HASH2(u->nick)];
    u->next = *list;
    if (*list)
        (*list)->prev = u;
    *list = u;
    u->uid = sstrdup(uid);
    return u;
}

Uid *find_uid(const char *nick)
{
    Uid *u;
    int i;

    for (i = 0; i < 1024; i++) {
        for (u = uidlist[i]; u; u = u->next) {
            if (!stricmp(nick, u->nick)) {
                return u;
            }
        }
    }
    return NULL;
}

Uid *find_nickuid(const char *uid)
{
    Uid *u;
    int i;

    for (i = 0; i < 1024; i++) {
        for (u = uidlist[i]; u; u = u->next) {
            if (!stricmp(uid, u->uid)) {
                return u;
            }
        }
    }
    return NULL;
}

/*************************************************************************/
/*************************************************************************/

/* Handle a server NICK command. */
/**
 * Adds a new user to anopes internal userlist.
 *
 * If the SVID passed is 2, the user will not be marked registered or requested to ID.
 * This is an addition to accomodate IRCds where we cannot determine this based on the NICK 
 * or UID command. Some IRCd's keep +r on when changing nicks and do not use SVID (ex. InspIRCd 1.2).
 * Instead we get a METADATA command containing the accountname the user was last identified to.
 * Since this is received after the user is introduced to us we should not yet mark the user
 * as identified or ask him to identify. We will mark him as recognized for the time being and let
 * him keep his +r if he has it.
 * It is the responsibility of the protocol module to make sure that this is either invalidated,
 * or changed to identified. ~ Viper
 **/
User *do_nick(const char *source, char *nick, char *username, char *host,
              char *server, char *realname, time_t ts, uint32 svid,
              uint32 ip, char *vhost, char *uid)
{
    User *user = NULL;

    char *tmp = NULL;
    NickAlias *old_na;          /* Old nick rec */
    int nc_changed = 1;         /* Did nick core change? */
    int status = 0;             /* Status to apply */
    char mask[USERMAX + HOSTMAX + 2];
    char *logrealname;
    char *oldnick;

    if (!*source) {
        char ipbuf[16];
        struct in_addr addr;

        if (ircd->nickvhost) {
            if (vhost) {
                if (!strcmp(vhost, "*")) {
                    vhost = NULL;
                    if (debug)
                        alog("debug: new user�with no vhost in NICK command: %s", nick);
                }
            }
        }

        /* This is a new user; create a User structure for it. */
        if (debug)
            alog("debug: new user: %s", nick);

        if (ircd->nickip) {
            addr.s_addr = htonl(ip);
            ntoa(addr, ipbuf, sizeof(ipbuf));
        }


        if (LogUsers) {
        /**
         * Ugly swap routine for Flop's bug :)
         **/
            if (realname) {
                tmp = strchr(realname, '%');
                while (tmp) {
                    *tmp = '-';
                    tmp = strchr(realname, '%');
                }
            }
            logrealname = normalizeBuffer(realname);

        /**
         * End of ugly swap
         **/

            if (ircd->nickvhost) {
                if (ircd->nickip) {
                    alog("LOGUSERS: %s (%s@%s => %s) (%s) [%s] connected to the network (%s).", nick, username, host, (vhost ? vhost : "none"), logrealname, ipbuf, server);
                } else {
                    alog("LOGUSERS: %s (%s@%s => %s) (%s) connected to the network (%s).", nick, username, host, (vhost ? vhost : "none"), logrealname, server);
                }
            } else {
                if (ircd->nickip) {
                    alog("LOGUSERS: %s (%s@%s) (%s) [%s] connected to the network (%s).", nick, username, host, logrealname, ipbuf, server);
                } else {
                    alog("LOGUSERS: %s (%s@%s) (%s) connected to the network (%s).", nick, username, host, logrealname, server);
                }
            }
            Anope_Free(logrealname);
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
        if (check_akill(nick, username, host, vhost, ipbuf)) {
/*            return NULL; */
        }

/**
 * DefCon AKILL system, if we want to akill all connecting user's here's where to do it
 * then force check_akill again on them...
 **/
        /* don't akill on netmerges -Certus */
        /* don't akill clients introduced by ulines. -Viper */
        if (is_sync(findserver(servlist, server))
            && checkDefCon(DEFCON_AKILL_NEW_CLIENTS) && !is_ulined(server)) {
            strncpy(mask, "*@", 3);
            strncat(mask, host, HOSTMAX);
            alog("DEFCON: adding akill for %s", mask);
            add_akill(NULL, mask, s_OperServ,
                      time(NULL) + dotime(DefConAKILL),
                      DefConAkillReason ? DefConAkillReason :
                      "DEFCON AKILL");
            if (check_akill(nick, username, host, vhost, ipbuf)) {
/*            return NULL; */
            }
        }

        /* SGLINE */
        if (ircd->sgline) {
            if (check_sgline(nick, realname))
                return NULL;
        }

        /* SQLINE */
        if (ircd->sqline) {
            if (check_sqline(nick, 0))
                return NULL;
        }

        /* SZLINE */
        if (ircd->szline && ircd->nickip) {
            if (check_szline(nick, ipbuf))
                return NULL;
        }
        /* Now check for session limits */
        if (LimitSessions && !is_ulined(server)
            && !add_session(nick, host, ipbuf))
            return NULL;

        /* Allocate User structure and fill it in. */
        user = new_user(nick);
        user->username = sstrdup(username);
        user->host = sstrdup(host);
        user->server = findserver(servlist, server);
        user->realname = sstrdup(realname);
        user->timestamp = ts;
        user->my_signon = time(NULL);
        user->chost = vhost ? sstrdup(vhost) : sstrdup(host);
        user->vhost = vhost ? sstrdup(vhost) : sstrdup(host);
        if (uid) {
            user->uid = sstrdup(uid);   /* p10/ts6 stuff */
        } else {
            user->uid = NULL;
        }
        user->vident = sstrdup(username);
        /* We now store the user's ip in the user_ struct,
         * because we will use it in serveral places -- DrStein */
        if (ircd->nickip) {
            user->hostip = sstrdup(ipbuf);
        } else {
            user->hostip = NULL;
        }

        if (svid == 0) {
            display_news(user, NEWS_LOGON);
            display_news(user, NEWS_RANDOM);
        }

        if (svid == 2 && user->na) {
            /* We do not yet know if the user should be identified or not.
             * mark him as recognized for now. 
             * It s up to the protocol module to make sure this either becomes ID'd or
             * is invalidated. ~ Viper */
            if (debug) 
                alog("debug: Marking %s as recognized..", user->nick);
            user->svid = 1;
            user->na->status |= NS_RECOGNIZED;
            nc_changed = 0;
        } else if (svid == ts && user->na) {
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

            anope_cmd_svid_umode(user->nick, user->timestamp);

        } else {
            user->svid = 1;
        }
        send_event(EVENT_NEWNICK, 1, nick);

    } else {
        /* An old user changing nicks. */
        if (UseTS6 && ircd->ts6)
            user = find_byuid(source);

        if (!user)
            user = finduser(source);

        if (!user) {
            alog("user: NICK from nonexistent nick %s", source);
            return NULL;
        }
        user->isSuperAdmin = 0; /* Dont let people nick change and stay SuperAdmins */
        if (debug)
            alog("debug: %s changes nick to %s", source, nick);

        if (LogUsers) {
            logrealname = normalizeBuffer(user->realname);
            if (ircd->vhost) {
                alog("LOGUSERS: %s (%s@%s => %s) (%s) changed nick to %s (%s).", user->nick, user->username, user->host, (user->vhost ? user->vhost : "(none)"), logrealname, nick, user->server->name);
            } else {
                alog("LOGUSERS: %s (%s@%s) (%s) changed nick to %s (%s).",
                     user->nick, user->username, user->host, logrealname,
                     nick, user->server->name);
            }
            if (logrealname) {
                free(logrealname);
            }
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

            oldnick = sstrdup(user->nick);
            change_user_nick(user, nick);

            if ((old_na ? old_na->nc : NULL) ==
                (user->na ? user->na->nc : NULL))
                nc_changed = 0;

            if (!nc_changed && (user->na))
                user->na->status |= status;
            else {
                anope_cmd_nc_change(user);
            }

            send_event(EVENT_CHANGE_NICK, 2, nick, oldnick);
            free(oldnick);
        }

        if (ircd->sqline) {
            if (!is_oper(user) && check_sqline(user->nick, 1))
                return NULL;
        }

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
            char tsbuf[16];
            user->na->last_seen = time(NULL);

            if (user->na->last_usermask)
                free(user->na->last_usermask);
            user->na->last_usermask =
                smalloc(strlen(common_get_vident(user)) +
                        strlen(common_get_vhost(user)) + 2);
            sprintf(user->na->last_usermask, "%s@%s",
                    common_get_vident(user), common_get_vhost(user));

            snprintf(tsbuf, sizeof(tsbuf), "%lu",
                     (unsigned long int) user->timestamp);
            anope_cmd_svid_umode2(user, tsbuf);

            alog("%s: %s!%s@%s automatically identified for nick %s",
                 s_NickServ, user->nick, user->username,
                 user->host, user->nick);
        }
    }

    /* Bahamut sets -r on every nick changes, so we must test it even if nc_changed == 0 */
    if (ircd->check_nick_id) {
        if (nick_identified(user)) {
            char tsbuf[16];
            snprintf(tsbuf, sizeof(tsbuf), "%lu",
                     (unsigned long int) user->timestamp);
            anope_cmd_svid_umode3(user, tsbuf);
        }
    }

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

    user = finduser(av[0]);
    if (!user) {
        alog("user: MODE %s for nonexistent nick %s: %s", av[1], av[0],
             merge_args(ac, av));
        return;
    }

    anope_set_umode(user, ac - 1, &av[1]);
}

/* Handle a UMODE2 command for a user.
 *	av[0] = modes
 */

void do_umode2(const char *source, int ac, char **av)
{
    User *user;

    user = finduser(source);
    if (!user) {
        alog("user: MODE %s for nonexistent nick %s: %s", av[0], source,
             merge_args(ac, av));
        return;
    }

    anope_set_umode(user, ac, &av[0]);
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
    if (debug) {
        alog("debug: %s quits", source);
    }
    if ((na = user->na) && (!(na->status & NS_VERBOTEN))
        && (!(na->nc->flags & NI_SUSPENDED))
        && (na->status & (NS_IDENTIFIED | NS_RECOGNIZED))) {
        na->last_seen = time(NULL);
        if (na->last_quit)
            free(na->last_quit);
        na->last_quit = *av[0] ? sstrdup(av[0]) : NULL;
    }
    if (LimitSessions && !is_ulined(user->server->name)) {
        del_session(user->host);
    }
    delete_user(user);
}

/*************************************************************************/

/* Handle a KILL command.
 *	av[0] = nick being killed
 *	av[1] = reason
 */

void do_kill(char *nick, char *msg)
{
    User *user;
    NickAlias *na;

    user = finduser(nick);
    if (!user) {
        if (debug) {
            alog("debug: KILL of nonexistent nick: %s", nick);
        }
        return;
    }
    if (debug) {
        alog("debug: %s killed", nick);
    }
    if ((na = user->na) && (!(na->status & NS_VERBOTEN))
        && (!(na->nc->flags & NI_SUSPENDED))
        && (na->status & (NS_IDENTIFIED | NS_RECOGNIZED))) {
        na->last_seen = time(NULL);
        if (na->last_quit)
            free(na->last_quit);
        na->last_quit = *msg ? sstrdup(msg) : NULL;

    }
    if (LimitSessions && !is_ulined(user->server->name)) {
        del_session(user->host);
    }
    delete_user(user);
}

/*************************************************************************/
/*************************************************************************/

/* Is the given user protected from kicks and negative mode changes? */

int is_protected(User * user)
{
    if (ircd->protectedumode) {
        return (user->mode & ircd->protectedumode);
    } else {
        return 0;
    }
}

/*************************************************************************/

/* Is the given nick an oper? */

int is_oper(User * user)
{
    if (user) {
        return (user->mode & anope_get_oper_mode());
    } else {
        return 0;
    }
}

/*************************************************************************/
/*************************************************************************/

/* Is the given user ban-excepted? */
int is_excepted(ChannelInfo * ci, User * user)
{
    if (!ci->c || !ircd->except)
        return 0;

    if (elist_match_user(ci->c->excepts, user))
        return 1;

    return 0;
}

/*************************************************************************/

/* Is the given MASK ban-excepted? */
int is_excepted_mask(ChannelInfo * ci, char *mask)
{
    if (!ci->c || !ircd->except)
        return 0;

    if (elist_match_mask(ci->c->excepts, mask, 0))
        return 1;

    return 0;
}


/*************************************************************************/

/* Does the user's usermask match the given mask (either nick!user@host or
 * just user@host)?
 */

static int _match_usermask(const char *mask, User * user, boolean full)
{
    char *mask2;
    char *nick, *username, *host;
    int result;

    if (!mask || !*mask) {
        return 0;
    }

    mask2 = sstrdup(mask);

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
            && ((full && match_wild_nocase(host, user->host))
                || match_wild_nocase(host, user->vhost)
                || match_wild_nocase(host, user->chost));
    } else {
        result = match_wild_nocase(username, user->username)
            && ((full && match_wild_nocase(host, user->host))
                || match_wild_nocase(host, user->vhost)
                || match_wild_nocase(host, user->chost));
    }

    free(mask2);
    return result;
}

int match_usermask(const char *mask, User *user)
{
	return _match_usermask(mask, user, false);
}

int match_usermask_full(const char *mask, User *user, boolean full)
{
	return _match_usermask(mask, user, full);
}

/*************************************************************************/

/* simlar to match_usermask, except here we pass the host as the IP */

int match_userip(const char *mask, User * user, char *iphost)
{
    char *mask2;
    char *nick, *username, *host;
    int result;

    if (!mask || !*mask) {
        return 0;
    }

    mask2 = sstrdup(mask);

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
            && (match_wild_nocase(host, iphost)
                || match_wild_nocase(host, user->vhost));
    } else {
        result = match_wild_nocase(username, user->username)
            && (match_wild_nocase(host, iphost)
                || match_wild_nocase(host, user->vhost));
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
    int ulen = strlen(common_get_vident(u));

    /* Get us a buffer the size of the username plus hostname.  The result
     * will never be longer than this (and will often be shorter), thus we
     * can use strcpy() and sprintf() safely.
     */
    end = mask = smalloc(ulen + strlen(common_get_vhost(u)) + 3);
    end += sprintf(end, "%s%s@",
                   (ulen <
                    (*(common_get_vident(u)) ==
                     '~' ? USERMAX + 1 : USERMAX) ? "*" : ""),
                   (*(common_get_vident(u)) ==
                    '~' ? common_get_vident(u) +
                    1 : common_get_vident(u)));

    if (strspn(common_get_vhost(u), "0123456789.") ==
        strlen(common_get_vhost(u))
        && (s = strchr(common_get_vhost(u), '.'))
        && (s = strchr(s + 1, '.'))
        && (s = strchr(s + 1, '.'))
        && (!strchr(s + 1, '.'))) {     /* IP addr */
        s = sstrdup(common_get_vhost(u));
        *strrchr(s, '.') = 0;

        sprintf(end, "%s.*", s);
        free(s);
    } else {
        if ((s = strchr(common_get_vhost(u), '.')) && strchr(s + 1, '.')) {
            s = sstrdup(strchr(common_get_vhost(u), '.') - 1);
            *s = '*';
            strcpy(end, s);
            free(s);
        } else {
            strcpy(end, common_get_vhost(u));
        }
    }
    return mask;
}

/*************************************************************************/
