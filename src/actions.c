/* Various routines to perform simple actions.
 *
 * (C) 2003 Anope Team
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

#include "services.h"

/*************************************************************************/

/**
 * Note a bad password attempt for the given user.  If they've used up
 * their limit, toss them off.
 * @param u the User to check
 * @return void
 */
void bad_password(User * u)
{
    time_t now = time(NULL);

    if (!u || !BadPassLimit) {
        return;
    }

    if (BadPassTimeout > 0 && u->invalid_pw_time > 0
        && u->invalid_pw_time < now - BadPassTimeout)
        u->invalid_pw_count = 0;
    u->invalid_pw_count++;
    u->invalid_pw_time = now;
    if (u->invalid_pw_count >= BadPassLimit) {
        kill_user(NULL, u->nick, "Too many invalid passwords");
    }
}

/*************************************************************************/

/**
 * Remove a user from the IRC network.
 * @param source is the nick which should generate the kill, or NULL for a server-generated kill.
 * @param user to remove
 * @param reason for the kill
 * @return void
 */
void kill_user(char *source, char *user, char *reason)
{
    char buf[BUFSIZE];

    if (!user || !*user) {
        return;
    }
    if (!source || !*source) {
        source = ServerName;
    }
    if (!reason) {
        reason = "";
    }

    snprintf(buf, sizeof(buf), "%s (%s)", source, reason);

    anope_cmd_svskill(source, user, buf);

    if (!ircd->quitonkill) {
        do_kill(user, buf);
    }
}

/*************************************************************************/

/**
 * Check and enforce SQlines
 * @param mask of the sqline
 * @param reason for the sqline
 * @return void
 */
void sqline(char *mask, char *reason)
{
    int i;
    Channel *c, *next;
    char *av[3];
    struct c_userlist *cu, *cunext;

    if (ircd->chansqline) {
        if (*mask == '#') {
            anope_cmd_sqline(mask, reason);

            for (i = 0; i < 1024; i++) {
                for (c = chanlist[i]; c; c = next) {
                    next = c->next;

                    if (!match_wild_nocase(mask, c->name)) {
                        continue;
                    }
                    for (cu = c->users; cu; cu = cunext) {
                        cunext = cu->next;
                        if (is_oper(cu->user)) {
                            continue;
                        }
                        av[0] = c->name;
                        av[1] = cu->user->nick;
                        av[2] = reason;
                        anope_cmd_kick(s_OperServ, av[0], av[1],
                                       "Q-Lined: %s", av[2]);
                        do_kick(s_ChanServ, 3, av);
                    }
                }
            }
        } else {
            anope_cmd_sqline(mask, reason);
        }
    } else {
        anope_cmd_sqline(mask, reason);
    }
}

/*************************************************************************/

/**
 * Unban the nick from a channel
 * @param ci channel info for the channel
 * @param nick to remove the ban for
 * @return void
 */
void common_unban(ChannelInfo * ci, char *nick)
{
    int count, i;
    char *av[3], **bans;
    User *u;
    char *host;

    if (!ci || !ci->c || !nick) {
        return;
    }

    if (!(u = finduser(nick))) {
        return;
    }

    host = host_resolve(u->host);

    if (ircd->svsmode_unban) {
        anope_cmd_unban(ci->name, nick);
    } else {
        av[0] = ci->name;
        av[1] = sstrdup("-b");
        count = ci->c->bancount;
        bans = scalloc(sizeof(char *) * count, 1);
        memcpy(bans, ci->c->bans, sizeof(char *) * count);
        for (i = 0; i < count; i++) {
            if (match_usermask(bans[i], u)) {
                anope_cmd_mode(whosends(ci), ci->name, "-b %s", bans[i]);
                av[2] = bans[i];
                do_cmode(whosends(ci), 3, av);
            }
            if (host) {
                if (match_userip(bans[i], u, host)) {
                    anope_cmd_mode(whosends(ci), ci->name, "-b %s",
                                   bans[i]);
                    av[2] = bans[i];
                    do_cmode(whosends(ci), 3, av);
                }
            }
        }
        free(bans);
        free(av[1]);
    }
}

/*************************************************************************/

/**
 * Prepare to set SVSMODE and update internal user modes
 * @param u user to apply modes to
 * @param modes the modes to set on the user
 * @param arg the arguments for the user modes
 * @return void
 */
void common_svsmode(User * u, char *modes, char *arg)
{
    int ac = 1;
    char *av[2];

    av[0] = modes;
    if (arg) {
        av[1] = arg;
        ac++;
    }

    anope_cmd_svsmode(u, ac, av);
    anope_set_umode(u, ac, av);
}

/*************************************************************************/

/**
 * Get the vhost for the user, if set else return the host, on ircds without
 * vhost this returns the host
 * @param u user to get the vhost for
 * @return vhost
 */
char *common_get_vhost(User * u)
{
    if (!u) {
        return NULL;
    }
    if (ircd->vhostmode) {
        if (u->mode & ircd->vhostmode) {
            return u->vhost;
            /* ptlink hack since there is no user mode
               for vhost, simply compare the host to the
               vhost struct memember */
        } else if (stricmp(u->vhost, u->host)) {
            return u->vhost;
        } else {
            return u->host;
        }
    } else {
        return u->host;
    }
}

/*************************************************************************/

/**
 * Get the vident for the user, if set else return the ident, on ircds without
 * vident this returns the ident
 * @param u user to get info the vident for
 * @return vident
 */
char *common_get_vident(User * u)
{
    if (!u) {
        return NULL;
    }
    if (ircd->vhostmode) {
        if (u->mode & ircd->vhostmode) {
            return u->vident;
            /* ptlink hack since there is no user mode
               for vhost, simply compare the host to the
               vhost struct memember */
        } else if (u->vident) {
            if (stricmp(u->vident, u->username)) {
                return u->vident;
            } else {
                return u->username;
            }
        } else {
            return u->username;
        }
    } else {
        if (u->vident) {
            if (stricmp(u->vident, u->username)) {
                return u->vident;
            } else {
                return u->username;
            }
        } else {
            return u->username;
        }
    }
}
