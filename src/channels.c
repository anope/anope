/* Channel-handling routines.
 *
 * (C) 2003-2008 Anope Team
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
#include "language.h"

Channel *chanlist[1024];

#define HASH(chan)	((chan)[1] ? ((chan)[1]&31)<<5 | ((chan)[2]&31) : 0)

/**************************** External Calls *****************************/
/*************************************************************************/

void chan_deluser(User * user, Channel * c)
{
    struct c_userlist *u;

    if (c->ci)
        update_cs_lastseen(user, c->ci);

    for (u = c->users; u && u->user != user; u = u->next);
    if (!u)
        return;

    if (u->ud) {
        if (u->ud->lastline)
            free(u->ud->lastline);
        free(u->ud);
    }

    if (u->next)
        u->next->prev = u->prev;
    if (u->prev)
        u->prev->next = u->next;
    else
        c->users = u->next;
    free(u);
    c->usercount--;

    if (s_BotServ && c->ci && c->ci->bi && c->usercount == BSMinUsers - 1) {
        anope_cmd_part(c->ci->bi->nick, c->name, NULL);
    }

    if (!c->users)
        chan_delete(c);
}

/*************************************************************************/

/* Returns a fully featured binary modes string. If complete is 0, the
 * eventual parameters won't be added to the string.
 */

char *chan_get_modes(Channel * chan, int complete, int plus)
{
    static char res[BUFSIZE];
    char *end = res;

    if (chan->mode) {
        int n = 0;
        CBModeInfo *cbmi = cbmodeinfos;

        do {
            if (chan->mode & cbmi->flag)
                *end++ = cbmi->mode;
        } while ((++cbmi)->flag != 0 && ++n < sizeof(res) - 1);

        if (complete) {
            cbmi = cbmodeinfos;

            do {
                if (cbmi->getvalue && (chan->mode & cbmi->flag) &&
                    (plus || !(cbmi->flags & CBM_MINUS_NO_ARG))) {
                    char *value = cbmi->getvalue(chan);

                    if (value) {
                        *end++ = ' ';
                        while (*value)
                            *end++ = *value++;
                    }
                }
            } while ((++cbmi)->flag != 0 && ++n < sizeof(res) - 1);
        }
    }

    *end = 0;

    return res;
}

/*************************************************************************/

/* Retrieves the status of an user on a channel */

int chan_get_user_status(Channel * chan, User * user)
{
    struct u_chanlist *uc;

    for (uc = user->chans; uc; uc = uc->next)
        if (uc->chan == chan)
            return uc->status;

    return 0;
}

/*************************************************************************/

/* Has the given user the given status on the given channel? :p */

int chan_has_user_status(Channel * chan, User * user, int16 status)
{
    struct u_chanlist *uc;

    for (uc = user->chans; uc; uc = uc->next) {
        if (uc->chan == chan) {
            if (debug) {
                alog("debug: chan_has_user_status wanted %d the user is %d", status, uc->status);
            }
            return (uc->status & status);
        }
    }
    return 0;
}

/*************************************************************************/

/* Remove the status of an user on a channel */

void chan_remove_user_status(Channel * chan, User * user, int16 status)
{
    struct u_chanlist *uc;

    if (debug >= 2)
        alog("debug: removing user status (%d) from %s for %s", status,
             user->nick, chan->name);

    for (uc = user->chans; uc; uc = uc->next) {
        if (uc->chan == chan) {
            uc->status &= ~status;
            break;
        }
    }
}

/*************************************************************************/

void chan_set_modes(const char *source, Channel * chan, int ac, char **av,
                    int check)
{
    int add = 1;
    char *modes = av[0], mode;
    CBMode *cbm;
    CMMode *cmm;
    CUMode *cum;
    unsigned char botmode = 0;
    BotInfo *bi;
    User *u, *user;
    int i, real_ac = ac;
    char **real_av = av;

    if (debug)
        alog("debug: Changing modes for %s to %s", chan->name,
             merge_args(ac, av));

    u = finduser(source);
    if (u && (chan_get_user_status(chan, u) & CUS_DEOPPED)) {
        char *s;

        if (debug)
            alog("debug: Removing instead of setting due to DEOPPED flag");

        /* Swap adding and removing of the modes */
        for (s = av[0]; *s; s++) {
            if (*s == '+')
                *s = '-';
            else if (*s == '-')
                *s = '+';
        }

        /* Set the resulting mode buffer */
        anope_cmd_mode(whosends(chan->ci), chan->name, merge_args(ac, av));

        return;
    }

    ac--;

    while ((mode = *modes++)) {

        switch (mode) {
        case '+':
            add = 1;
            continue;
        case '-':
            add = 0;
            continue;
        }

        if (((int) mode) < 0) {
            if (debug)
                alog("Debug: Malformed mode detected on %s.", chan->name);
            continue;
        }

        if ((cum = &cumodes[(int) mode])->status != 0) {
            if (ac == 0) {
                alog("channel: mode %c%c with no parameter (?) for channel %s", add ? '+' : '-', mode, chan->name);
                continue;
            }
            ac--;
            av++;

            if ((cum->flags & CUF_PROTECT_BOTSERV) && !add) {
                if ((bi = findbot(*av))) {
                    if (!botmode || botmode != mode) {
                        anope_cmd_mode(bi->nick, chan->name, "+%c %s",
                                       mode, bi->nick);
                        botmode = mode;
                        continue;
                    } else {
                        botmode = mode;
                        continue;
                    }
                }
            } else {
                if ((bi = findbot(*av))) {
                    continue;
                }
            }

            if (!(user = finduser(*av))
                && !(UseTS6 && ircd->ts6 && (user = find_byuid(*av)))) {
                if (debug) {
                    alog("debug: MODE %s %c%c for nonexistent user %s",
                         chan->name, (add ? '+' : '-'), mode, *av);
                }
                continue;
            }

            if (debug)
                alog("debug: Setting %c%c on %s for %s", (add ? '+' : '-'),
                     mode, chan->name, user->nick);

            if (add) {
                chan_set_user_status(chan, user, cum->status);
                /* If this does +o, remove any DEOPPED flag */
                if (cum->status & CUS_OP)
                    chan_remove_user_status(chan, user, CUS_DEOPPED);
            } else {
                chan_remove_user_status(chan, user, cum->status);
            }

        } else if ((cbm = &cbmodes[(int) mode])->flag != 0) {
            if (check >= 0) {
                if (add)
                    chan->mode |= cbm->flag;
                else
                    chan->mode &= ~cbm->flag;
            }

            if (cbm->setvalue) {
                if (add || !(cbm->flags & CBM_MINUS_NO_ARG)) {
                    if (ac == 0) {
                        alog("channel: mode %c%c with no parameter (?) for channel %s", add ? '+' : '-', mode, chan->name);
                        continue;
                    }
                    ac--;
                    av++;
                }
                cbm->setvalue(chan, add ? *av : NULL);
            }

            if (check < 0) {
                if (add)
                    chan->mode |= cbm->flag;
                else
                    chan->mode &= ~cbm->flag;
            }
        } else if ((cmm = &cmmodes[(int) mode])->addmask) {
            if (ac == 0) {
                alog("channel: mode %c%c with no parameter (?) for channel %s", add ? '+' : '-', mode, chan->name);
                continue;
            }

            ac--;
            av++;
            if (add)
                cmm->addmask(chan, *av);
            else
                cmm->delmask(chan, *av);
        }
    }

    if (check > 0) {
        check_modes(chan);

        if (check < 2) {
            /* Walk through all users we've set modes for and see if they are
             * valid. Invalid modes (like +o with SECUREOPS on) will be removed
             */
            real_ac--;
            real_av++;
            for (i = 0; i < real_ac; i++) {
                if ((user = finduser(*real_av)) && is_on_chan(chan, user))
                    chan_set_correct_modes(user, chan, 0);
                real_av++;
            }
        }
    }
}

/*************************************************************************/

/* Set the status of an user on a channel */

void chan_set_user_status(Channel * chan, User * user, int16 status)
{
    struct u_chanlist *uc;

    if (debug >= 2)
        alog("debug: setting user status (%d) on %s for %s", status,
             user->nick, chan->name);

    if (HelpChannel && ircd->supporthelper && (status & CUS_OP)
        && (stricmp(chan->name, HelpChannel) == 0)
        && (!chan->ci || check_access(user, chan->ci, CA_AUTOOP))) {
        if (debug) {
            alog("debug: %s being given +h for having %d status in %s",
                 user->nick, status, chan->name);
        }
        common_svsmode(user, "+h", NULL);
    }

    for (uc = user->chans; uc; uc = uc->next) {
        if (uc->chan == chan) {
            uc->status |= status;
            break;
        }
    }
}

/*************************************************************************/

/* Return the Channel structure corresponding to the named channel, or NULL
 * if the channel was not found.  chan is assumed to be non-NULL and valid
 * (i.e. pointing to a channel name of 2 or more characters). */

Channel *findchan(const char *chan)
{
    Channel *c;

    if (!chan || !*chan) {
        if (debug) {
            alog("debug: findchan() called with NULL values");
        }
        return NULL;
    }

    if (debug >= 3)
        alog("debug: findchan(%p)", chan);
    c = chanlist[HASH(chan)];
    while (c) {
        if (stricmp(c->name, chan) == 0) {
            if (debug >= 3)
                alog("debug: findchan(%s) -> %p", chan, (void *) c);
            return c;
        }
        c = c->next;
    }
    if (debug >= 3)
        alog("debug: findchan(%s) -> %p", chan, (void *) c);
    return NULL;
}

/*************************************************************************/

/* Iterate over all channels in the channel list.  Return NULL at end of
 * list.
 */

static Channel *current;
static int next_index;

Channel *firstchan(void)
{
    next_index = 0;
    while (next_index < 1024 && current == NULL)
        current = chanlist[next_index++];
    if (debug >= 3)
        alog("debug: firstchan() returning %s",
             current ? current->name : "NULL (end of list)");
    return current;
}

Channel *nextchan(void)
{
    if (current)
        current = current->next;
    if (!current && next_index < 1024) {
        while (next_index < 1024 && current == NULL)
            current = chanlist[next_index++];
    }
    if (debug >= 3)
        alog("debug: nextchan() returning %s",
             current ? current->name : "NULL (end of list)");
    return current;
}

/*************************************************************************/

/* Return statistics.  Pointers are assumed to be valid. */

void get_channel_stats(long *nrec, long *memuse)
{
    long count = 0, mem = 0;
    Channel *chan;
    struct c_userlist *cu;
    BanData *bd;
    int i, j;

    for (i = 0; i < 1024; i++) {
        for (chan = chanlist[i]; chan; chan = chan->next) {
            count++;
            mem += sizeof(*chan);
            if (chan->topic)
                mem += strlen(chan->topic) + 1;
            if (chan->key)
                mem += strlen(chan->key) + 1;
            if (ircd->fmode) {
                if (chan->flood)
                    mem += strlen(chan->flood) + 1;
            }
            if (ircd->Lmode) {
                if (chan->redirect)
                    mem += strlen(chan->redirect) + 1;
            }
            mem += sizeof(char *) * chan->bansize;
            for (j = 0; j < chan->bancount; j++) {
                if (chan->bans[j])
                    mem += strlen(chan->bans[j]) + 1;
            }
            if (ircd->except) {
                mem += sizeof(char *) * chan->exceptsize;
                for (j = 0; j < chan->exceptcount; j++) {
                    if (chan->excepts[j])
                        mem += strlen(chan->excepts[j]) + 1;
                }
            }
            if (ircd->invitemode) {
                mem += sizeof(char *) * chan->invitesize;
                for (j = 0; j < chan->invitecount; j++) {
                    if (chan->invite[j])
                        mem += strlen(chan->invite[j]) + 1;
                }
            }
            for (cu = chan->users; cu; cu = cu->next) {
                mem += sizeof(*cu);
                if (cu->ud) {
                    mem += sizeof(*cu->ud);
                    if (cu->ud->lastline)
                        mem += strlen(cu->ud->lastline) + 1;
                }
            }
            for (bd = chan->bd; bd; bd = bd->next) {
                if (bd->mask)
                    mem += strlen(bd->mask) + 1;
                mem += sizeof(*bd);
            }
        }
    }
    *nrec = count;
    *memuse = mem;
}

/*************************************************************************/

/* Is the given nick on the given channel? */

int is_on_chan(Channel * c, User * u)
{
    struct u_chanlist *uc;

    for (uc = u->chans; uc; uc = uc->next)
        if (uc->chan == c)
            return 1;

    return 0;
}

/*************************************************************************/

/* Is the given nick on the given channel?
   This function supports links. */

User *nc_on_chan(Channel * c, NickCore * nc)
{
    struct c_userlist *u;

    if (!c || !nc)
        return NULL;

    for (u = c->users; u; u = u->next) {
        if (u->user->na && u->user->na->nc == nc
            && nick_recognized(u->user))
            return u->user;
    }
    return NULL;
}

/*************************************************************************/
/*************************** Message Handling ****************************/
/*************************************************************************/

/* Handle a JOIN command.
 *	av[0] = channels to join
 */

void do_join(const char *source, int ac, char **av)
{
    User *user;
    Channel *chan;
    char *s, *t;
    struct u_chanlist *c, *nextc;
    char *channame;
    time_t ts = time(NULL);

    if (UseTS6 && ircd->ts6) {
        user = find_byuid(source);
        if (!user)
            user = finduser(source);
    } else {
        user = finduser(source);
    }
    if (!user) {
        if (debug) {
            alog("debug: JOIN from nonexistent user %s: %s", source,
                 merge_args(ac, av));
        }
        return;
    }

    t = av[0];
    while (*(s = t)) {
        t = s + strcspn(s, ",");
        if (*t)
            *t++ = 0;

        if (*s == '0') {
            c = user->chans;
            while (c) {
                nextc = c->next;
                channame = sstrdup(c->chan->name);
                send_event(EVENT_PART_CHANNEL, 3, EVENT_START, user->nick,
                           channame);
                chan_deluser(user, c->chan);
                send_event(EVENT_PART_CHANNEL, 3, EVENT_STOP, user->nick,
                           channame);
                free(channame);
                free(c);
                c = nextc;
            }
            user->chans = NULL;
            continue;
        }

        /* how about not triggering the JOIN event on an actual /part :) -certus */
        send_event(EVENT_JOIN_CHANNEL, 3, EVENT_START, source, s);

        /* Make sure check_kick comes before chan_adduser, so banned users
         * don't get to see things like channel keys. */
        /* If channel already exists, check_kick() will use correct TS.
         * Otherwise, we lose. */
        if (check_kick(user, s, ts))
            continue;

        if (ac == 2) {
            if (debug) {
                alog("debug: recieved a new TS for JOIN: %ld",
                     (long int) ts);
            }
            ts = strtoul(av[1], NULL, 10);
        }

        chan = findchan(s);
        chan = join_user_update(user, chan, s, ts);
        chan_set_correct_modes(user, chan, 1);

        send_event(EVENT_JOIN_CHANNEL, 3, EVENT_STOP, source, s);
    }
}

/*************************************************************************/

/* Handle a KICK command.
 *	av[0] = channel
 *	av[1] = nick(s) being kicked
 *	av[2] = reason
 */

void do_kick(const char *source, int ac, char **av)
{
    BotInfo *bi;
    ChannelInfo *ci;
    User *user;
    char *s, *t;
    struct u_chanlist *c;

    t = av[1];
    while (*(s = t)) {
        t = s + strcspn(s, ",");
        if (*t)
            *t++ = 0;

        /* If it is the bot that is being kicked, we make it rejoin the
         * channel and stop immediately.
         *      --lara
         */
        if (s_BotServ && (bi = findbot(s)) && (ci = cs_findchan(av[0]))) {
            bot_join(ci);
            continue;
        }

        if (UseTS6 && ircd->ts6) {
            user = find_byuid(s);
            if (!user) {
                user = finduser(s);
            }
        } else {
            user = finduser(s);
        }
        if (!user) {
            if (debug) {
                alog("debug: KICK for nonexistent user %s on %s: %s", s,
                     av[0], merge_args(ac - 2, av + 2));
            }
            continue;
        }
        if (debug) {
            alog("debug: kicking %s from %s", user->nick, av[0]);
        }
        for (c = user->chans; c && stricmp(av[0], c->chan->name) != 0;
             c = c->next);
        if (c) {
            send_event(EVENT_CHAN_KICK, 2, user->nick, av[0]);
            chan_deluser(user, c->chan);
            if (c->next)
                c->next->prev = c->prev;
            if (c->prev)
                c->prev->next = c->next;
            else
                user->chans = c->next;
            free(c);
        }
    }
}

/*************************************************************************/

/* Handle a PART command.
 *	av[0] = channels to leave
 *	av[1] = reason (optional)
 */

void do_part(const char *source, int ac, char **av)
{
    User *user;
    char *s, *t;
    struct u_chanlist *c;
    char *channame;

    if (UseTS6 && ircd->ts6) {
        user = find_byuid(source);
        if (!user)
            user = finduser(source);
    } else {
        user = finduser(source);
    }
    if (!user) {
        if (debug) {
            alog("debug: PART from nonexistent user %s: %s", source,
                 merge_args(ac, av));
        }
        return;
    }
    t = av[0];
    while (*(s = t)) {
        t = s + strcspn(s, ",");
        if (*t)
            *t++ = 0;
        if (debug)
            alog("debug: %s leaves %s", source, s);
        for (c = user->chans; c && stricmp(s, c->chan->name) != 0;
             c = c->next);
        if (c) {
            if (!c->chan) {
                alog("user: BUG parting %s: channel entry found but c->chan NULL", s);
                return;
            }
            channame = sstrdup(c->chan->name);
            send_event(EVENT_PART_CHANNEL, (ac >= 2 ? 4 : 3), EVENT_START,
                       user->nick, channame, (ac >= 2 ? av[1] : ""));

            chan_deluser(user, c->chan);
            if (c->next)
                c->next->prev = c->prev;
            if (c->prev)
                c->prev->next = c->next;
            else
                user->chans = c->next;
            free(c);

            send_event(EVENT_PART_CHANNEL, (ac >= 2 ? 4 : 3), EVENT_STOP,
                       user->nick, channame, (ac >= 2 ? av[1] : ""));
            free(channame);
        }
    }
}

/*************************************************************************/

/* Handle a SJOIN command.

   On channel creation, syntax is:

   av[0] = timestamp
   av[1] = channel name
   av[2|3|4] = modes   \   depends of whether the modes k and l
   av[3|4|5] = users   /   are set or not.

   When a single user joins an (existing) channel, it is:

   av[0] = timestamp
   av[1] = user

   ============================================================

   Unreal SJOIN

   On Services connect there is
   SJOIN !11LkOb #ircops +nt :@Trystan &*!*@*.aol.com "*@*.home.com

   av[0] = time stamp (base64)
   av[1] = channel
   av[2] = modes
   av[3] = users + bans + exceptions

   On Channel Creation or a User joins an existing
   Luna.NomadIrc.Net SJOIN !11LkW9 #akill :@Trystan
   Luna.NomadIrc.Net SJOIN !11LkW9 #akill :Trystan`

   av[0] = time stamp (base64)
   av[1] = channel
   av[2] = users

*/

void do_sjoin(const char *source, int ac, char **av)
{
    Channel *c;
    User *user;
    Server *serv;
    struct c_userlist *cu;
    char *s = NULL;
    char *end, cubuf[7], *end2, *cumodes[6];
    int is_sqlined = 0;
    int ts = 0;
    int is_created = 0;
    int keep_their_modes = 1;

    serv = findserver(servlist, source);

    if (ircd->sjb64) {
        ts = base64dects(av[0]);
    } else {
        ts = strtoul(av[0], NULL, 10);
    }
    c = findchan(av[1]);
    if (c != NULL) {
        if (c->creation_time == 0 || ts == 0)
            c->creation_time = 0;
        else if (c->creation_time > ts) {
            c->creation_time = ts;
            for (cu = c->users; cu; cu = cu->next) {
                /* XXX */
                cumodes[0] = "-ov";
                cumodes[1] = cu->user->nick;
                cumodes[2] = cu->user->nick;
                chan_set_modes(source, c, 3, cumodes, 2);
            }
            if (c->ci && c->ci->bi) {
                /* This is ugly, but it always works */
                anope_cmd_part(c->ci->bi->nick, c->name, "TS reop");
                bot_join(c->ci);
            }
            /* XXX simple modes and bans */
        } else if (c->creation_time < ts)
            keep_their_modes = 0;
    } else
        is_created = 1;

    /* Double check to avoid unknown modes that need parameters */
    if (ac >= 4) {
        if (ircd->chansqline) {
            if (!c)
                is_sqlined = check_chan_sqline(av[1]);
        }

        cubuf[0] = '+';
        cumodes[0] = cubuf;

        /* We make all the users join */
        s = av[ac - 1];         /* Users are always the last element */

        while (*s) {
            end = strchr(s, ' ');
            if (end)
                *end = 0;

            end2 = cubuf + 1;


            if (ircd->sjoinbanchar) {
                if (*s == ircd->sjoinbanchar && keep_their_modes) {
                    add_ban(c, myStrGetToken(s, ircd->sjoinbanchar, 1));
                    if (!end)
                        break;
                    s = end + 1;
                    continue;
                }
            }
            if (ircd->sjoinexchar) {
                if (*s == ircd->sjoinexchar && keep_their_modes) {
                    add_exception(c,
                                  myStrGetToken(s, ircd->sjoinexchar, 1));
                    if (!end)
                        break;
                    s = end + 1;
                    continue;
                }
            }

            if (ircd->sjoininvchar) {
                if (*s == ircd->sjoininvchar && keep_their_modes) {
                    add_invite(c, myStrGetToken(s, ircd->sjoininvchar, 1));
                    if (!end)
                        break;
                    s = end + 1;
                    continue;
                }
            }

            while (csmodes[(int) *s] != 0)
                *end2++ = csmodes[(int) *s++];
            *end2 = 0;


            if (UseTS6 && ircd->ts6) {
                user = find_byuid(s);
                if (!user)
                    user = finduser(s);
            } else {
                user = finduser(s);
            }

            if (!user) {
                if (debug) {
                    alog("debug: SJOIN for nonexistent user %s on %s", s,
                         av[1]);
                }
                return;
            }

            if (is_sqlined && !is_oper(user)) {
                anope_cmd_kick(s_OperServ, av[1], s, "Q-Lined");
            } else {
                if (!check_kick(user, av[1], ts)) {
                    send_event(EVENT_JOIN_CHANNEL, 3, EVENT_START,
                               user->nick, av[1]);

                    /* Make the user join; if the channel does not exist it
                     * will be created there. This ensures that the channel
                     * is not created to be immediately destroyed, and
                     * that the locked key or topic is not shown to anyone
                     * who joins the channel when empty.
                     */
                    c = join_user_update(user, c, av[1], ts);

                    /* We update user mode on the channel */
                    if (end2 - cubuf > 1 && keep_their_modes) {
                        int i;

                        for (i = 1; i < end2 - cubuf; i++)
                            cumodes[i] = user->nick;
                        chan_set_modes(source, c, 1 + (end2 - cubuf - 1),
                                       cumodes, 2);
                    }

                    if (c->ci && (!serv || is_sync(serv))
                        && !c->topic_sync)
                        restore_topic(c->name);
                    chan_set_correct_modes(user, c, 1);

                    send_event(EVENT_JOIN_CHANNEL, 3, EVENT_STOP,
                               user->nick, av[1]);
                }
            }

            if (!end)
                break;
            s = end + 1;
        }

        if (c && keep_their_modes) {
            /* We now update the channel mode. */
            chan_set_modes(source, c, ac - 3, &av[2], 2);
        }

        /* Unreal just had to be different */
    } else if (ac == 3 && !ircd->ts6) {
        if (ircd->chansqline) {
            if (!c)
                is_sqlined = check_chan_sqline(av[1]);
        }

        cubuf[0] = '+';
        cumodes[0] = cubuf;

        /* We make all the users join */
        s = av[2];              /* Users are always the last element */

        while (*s) {
            end = strchr(s, ' ');
            if (end)
                *end = 0;

            end2 = cubuf + 1;

            while (csmodes[(int) *s] != 0)
                *end2++ = csmodes[(int) *s++];
            *end2 = 0;

            if (UseTS6 && ircd->ts6) {
                user = find_byuid(s);
                if (!user)
                    user = finduser(s);
            } else {
                user = finduser(s);
            }

            if (!user) {
                if (debug) {
                    alog("debug: SJOIN for nonexistent user %s on %s", s,
                         av[1]);
                }
                return;
            }

            if (is_sqlined && !is_oper(user)) {
                anope_cmd_kick(s_OperServ, av[1], s, "Q-Lined");
            } else {
                if (!check_kick(user, av[1], ts)) {
                    send_event(EVENT_JOIN_CHANNEL, 3, EVENT_START,
                               user->nick, av[1]);

                    /* Make the user join; if the channel does not exist it
                     * will be created there. This ensures that the channel
                     * is not created to be immediately destroyed, and
                     * that the locked key or topic is not shown to anyone
                     * who joins the channel when empty.
                     */
                    c = join_user_update(user, c, av[1], ts);

                    /* We update user mode on the channel */
                    if (end2 - cubuf > 1 && keep_their_modes) {
                        int i;

                        for (i = 1; i < end2 - cubuf; i++)
                            cumodes[i] = user->nick;
                        chan_set_modes(source, c, 1 + (end2 - cubuf - 1),
                                       cumodes, 2);
                    }

                    chan_set_correct_modes(user, c, 1);

                    send_event(EVENT_JOIN_CHANNEL, 3, EVENT_STOP,
                               user->nick, av[1]);
                }
            }

            if (!end)
                break;
            s = end + 1;
        }
    } else if (ac == 3 && ircd->ts6) {
        if (ircd->chansqline) {
            if (!c)
                is_sqlined = check_chan_sqline(av[1]);
        }

        cubuf[0] = '+';
        cumodes[0] = cubuf;

        /* We make all the users join */
        s = sstrdup(source);    /* Users are always the last element */

        while (*s) {
            end = strchr(s, ' ');
            if (end)
                *end = 0;

            end2 = cubuf + 1;

            while (csmodes[(int) *s] != 0)
                *end2++ = csmodes[(int) *s++];
            *end2 = 0;

            if (UseTS6 && ircd->ts6) {
                user = find_byuid(s);
                if (!user)
                    user = finduser(s);
            } else {
                user = finduser(s);
            }
            if (!user) {
                if (debug) {
                    alog("debug: SJOIN for nonexistent user %s on %s", s,
                         av[1]);
                }
                free(s);
                return;
            }

            if (is_sqlined && !is_oper(user)) {
                anope_cmd_kick(s_OperServ, av[1], s, "Q-Lined");
            } else {
                if (!check_kick(user, av[1], ts)) {
                    send_event(EVENT_JOIN_CHANNEL, 3, EVENT_START,
                               user->nick, av[1]);

                    /* Make the user join; if the channel does not exist it
                     * will be created there. This ensures that the channel
                     * is not created to be immediately destroyed, and
                     * that the locked key or topic is not shown to anyone
                     * who joins the channel when empty.
                     */
                    c = join_user_update(user, c, av[1], ts);

                    /* We update user mode on the channel */
                    if (end2 - cubuf > 1 && keep_their_modes) {
                        int i;

                        for (i = 1; i < end2 - cubuf; i++)
                            cumodes[i] = user->nick;
                        chan_set_modes(source, c, 1 + (end2 - cubuf - 1),
                                       cumodes, 2);
                    }

                    chan_set_correct_modes(user, c, 1);

                    send_event(EVENT_JOIN_CHANNEL, 3, EVENT_STOP,
                               user->nick, av[1]);
                }
            }

            if (!end)
                break;
            s = end + 1;
        }
        free(s);
    } else if (ac == 2) {
        if (UseTS6 && ircd->ts6) {
            user = find_byuid(source);
            if (!user)
                user = finduser(source);
        } else {
            user = finduser(source);
        }
        if (!user) {
            if (debug) {
                alog("debug: SJOIN for nonexistent user %s on %s", source,
                     av[1]);
            }
            return;
        }

        if (check_kick(user, av[1], ts))
            return;

        if (ircd->chansqline) {
            if (!c)
                is_sqlined = check_chan_sqline(av[1]);
        }

        if (is_sqlined && !is_oper(user)) {
            anope_cmd_kick(s_OperServ, av[1], user->nick, "Q-Lined");
        } else {
            send_event(EVENT_JOIN_CHANNEL, 3, EVENT_START, user->nick,
                       av[1]);

            c = join_user_update(user, c, av[1], ts);
            if (is_created && c->ci)
                restore_topic(c->name);
            chan_set_correct_modes(user, c, 1);

            send_event(EVENT_JOIN_CHANNEL, 3, EVENT_STOP, user->nick,
                       av[1]);
        }
    }
}


/*************************************************************************/

/* Handle a channel MODE command. */

void do_cmode(const char *source, int ac, char **av)
{
    Channel *chan;
    ChannelInfo *ci = NULL;
    int i;
    char *t;

    if (ircdcap->tsmode) {
        /* TSMODE for bahamut - leave this code out to break MODEs. -GD */
        /* if they don't send it in CAPAB check if we just want to enable it */
        if (uplink_capab & ircdcap->tsmode || UseTSMODE) {
            for (i = 0; i < strlen(av[1]); i++) {
                if (!isdigit(av[1][i]))
                    break;
            }
            if (av[1][i] == '\0') {
                /* We have a valid TS field in av[1] now, so we can strip it off */
                /* After we swap av[0] and av[1] ofcourse to not break stuff! :) */
                t = av[0];
                av[0] = av[1];
                av[1] = t;
                ac--;
                av++;
            } else {
                alog("TSMODE enabled but MODE has no valid TS");
            }
        }
    }

    /* :42XAAAAAO TMODE 1106409026 #ircops +b *!*@*.aol.com */
    if (UseTS6 && ircd->ts6) {
        if (isdigit(av[0][0])) {
            ac--;
            av++;
        }
    }

    chan = findchan(av[0]);
    if (!chan) {
        if (debug) {
            ci = cs_findchan(av[0]);
            if (!(ci && (ci->flags & CI_VERBOTEN)))
                alog("debug: MODE %s for nonexistent channel %s",
                     merge_args(ac - 1, av + 1), av[0]);
        }
        return;
    }

    /* This shouldn't trigger on +o, etc. */
    if (strchr(source, '.') && !av[1][strcspn(av[1], "bovahq")]) {
        if (time(NULL) != chan->server_modetime) {
            chan->server_modecount = 0;
            chan->server_modetime = time(NULL);
        }
        chan->server_modecount++;
    }

    ac--;
    av++;
    chan_set_modes(source, chan, ac, av, 1);
}

/*************************************************************************/

/* Handle a TOPIC command. */

void do_topic(const char *source, int ac, char **av)
{
    Channel *c = findchan(av[0]);
    ChannelInfo *ci;
    int ts;
    time_t topic_time;
    char *topicsetter;

    if (ircd->sjb64) {
        ts = base64dects(av[2]);
        if (debug) {
            alog("debug: encoded TOPIC TS %s converted to %d", av[2], ts);
        }
    } else {
        ts = strtoul(av[2], NULL, 10);
    }

    topic_time = ts;

    if (!c) {
        if (debug) {
            alog("debug: TOPIC %s for nonexistent channel %s",
                 merge_args(ac - 1, av + 1), av[0]);
        }
        return;
    }

    /* We can be sure that the topic will be in sync here -GD */
    c->topic_sync = 1;

    ci = c->ci;

    /* For Unreal, cut off the ! and any futher part of the topic setter.
     * This way, nick!ident@host setters will only show the nick. -GD
     */
    topicsetter = myStrGetToken(av[1], '!', 0);

    /* If the current topic we have matches the last known topic for this
     * channel exactly, there's no need to update anything and we can as
     * well just return silently without updating anything. -GD
     */
    if ((ac > 3) && *av[3] && ci && ci->last_topic
        && (strcmp(av[3], ci->last_topic) == 0)
        && (strcmp(topicsetter, ci->last_topic_setter) == 0)) {
        free(topicsetter);
        return;
    }

    if (check_topiclock(c, topic_time)) {
        free(topicsetter);
        return;
    }

    if (c->topic) {
        free(c->topic);
        c->topic = NULL;
    }
    if (ac > 3 && *av[3]) {
        c->topic = sstrdup(av[3]);
    }

    strscpy(c->topic_setter, topicsetter, sizeof(c->topic_setter));
    c->topic_time = topic_time;
    free(topicsetter);

    record_topic(av[0]);

    if (ci && ci->last_topic) {
        send_event(EVENT_TOPIC_UPDATED, 2, av[0], ci->last_topic);
    } else {
        send_event(EVENT_TOPIC_UPDATED, 2, av[0], "");
    }
}

/*************************************************************************/
/**************************** Internal Calls *****************************/
/*************************************************************************/

void add_ban(Channel * chan, char *mask)
{
    /* check for NULL values otherwise we will segfault */
    if (!chan || !mask) {
        if (debug) {
            alog("debug: add_ban called with NULL values");
        }
        return;
    }

    if (s_BotServ && BSSmartJoin && chan->ci && chan->ci->bi
        && chan->usercount >= BSMinUsers) {
        char botmask[BUFSIZE];
        BotInfo *bi = chan->ci->bi;

        snprintf(botmask, sizeof(botmask), "%s!%s@%s", bi->nick, bi->user,
                 bi->host);
        if (match_wild_nocase(mask, botmask)) {
            anope_cmd_mode(bi->nick, chan->name, "-b %s", mask);
            return;
        }
    }

    if (chan->bancount >= chan->bansize) {
        chan->bansize += 8;
        chan->bans = srealloc(chan->bans, sizeof(char *) * chan->bansize);
    }
    chan->bans[chan->bancount++] = sstrdup(mask);

    if (debug)
        alog("debug: Added ban %s to channel %s", mask, chan->name);
}

/*************************************************************************/

void add_exception(Channel * chan, char *mask)
{
    if (!chan || !mask) {
        if (debug)
            alog("debug: add_exception called with NULL values");
        return;
    }

    if (chan->exceptcount >= chan->exceptsize) {
        chan->exceptsize += 8;
        chan->excepts =
            srealloc(chan->excepts, sizeof(char *) * chan->exceptsize);
    }
    chan->excepts[chan->exceptcount++] = sstrdup(mask);

    if (debug)
        alog("debug: Added except %s to channel %s", mask, chan->name);
}

/*************************************************************************/

void add_invite(Channel * chan, char *mask)
{
    if (!chan || !mask) {
        if (debug)
            alog("debug: add_invite called with NULL values");
        return;
    }

    if (chan->invitecount >= chan->invitesize) {
        chan->invitesize += 8;
        chan->invite =
            srealloc(chan->invite, sizeof(char *) * chan->invitesize);
    }
    chan->invite[chan->invitecount++] = sstrdup(mask);

    if (debug)
        alog("debug: Added invite %s to channel %s", mask, chan->name);
}

/*************************************************************************/

/**
 * Set the correct modes, or remove the ones granted without permission,
 * for the specified user on ths specified channel. This doesn't give
 * modes to ignored users, but does remove them if needed.
 * @param user The user to give/remove modes to/from
 * @param c The channel to give/remove modes on
 * @param give_modes Set to 1 to give modes, 0 to not give modes
 * @return void
 **/
void chan_set_correct_modes(User * user, Channel * c, int give_modes)
{
    char *tmp;
    char modebuf[BUFSIZE];
    char userbuf[BUFSIZE];
    int status;
    int add_modes = 0;
    int rem_modes = 0;
    ChannelInfo *ci;

    if (!c || !(ci = c->ci))
        return;

    if ((ci->flags & CI_VERBOTEN) || (*(c->name) == '+'))
        return;

    status = chan_get_user_status(c, user);

    if (debug)
        alog("debug: Setting correct user modes for %s on %s (current status: %d, %sgiving modes)", user->nick, c->name, status, (give_modes ? "" : "not "));

    /* Changed the second line of this if a bit, to make sure unregistered
     * users can always get modes (IE: they always have autoop enabled). Before
     * this change, you were required to have a registered nick to be able
     * to receive modes. I wonder who added that... *looks at Rob* ;) -GD
     */
    if (give_modes && (get_ignore(user->nick) == NULL)
        && (!user->na || !(user->na->nc->flags & NI_AUTOOP))) {
        if (ircd->owner && is_founder(user, ci))
            add_modes |= CUS_OWNER;
        else if ((ircd->protect || ircd->admin)
                 && check_access(user, ci, CA_AUTOPROTECT))
            add_modes |= CUS_PROTECT;
        if (check_access(user, ci, CA_AUTOOP))
            add_modes |= CUS_OP;
        else if (ircd->halfop && check_access(user, ci, CA_AUTOHALFOP))
            add_modes |= CUS_HALFOP;
        else if (check_access(user, ci, CA_AUTOVOICE))
            add_modes |= CUS_VOICE;
    }

    /* We check if every mode they have is legally acquired here, and remove
     * the modes that they're not allowed to have. But only if SECUREOPS is
     * on, because else every mode is legal. -GD
     * Unless the channel has just been created. -heinz
     *     Or the user matches CA_AUTODEOP... -GD
     */
    if (((ci->flags & CI_SECUREOPS) || (c->usercount == 1)
         || check_access(user, ci, CA_AUTODEOP))
        && !is_ulined(user->server->name)) {
        if (ircd->owner && (status & CUS_OWNER) && !is_founder(user, ci))
            rem_modes |= CUS_OWNER;
        if ((ircd->protect || ircd->admin) && (status & CUS_PROTECT)
            && !check_access(user, ci, CA_AUTOPROTECT)
            && !check_access(user, ci, CA_PROTECTME))
            rem_modes |= CUS_PROTECT;
        if ((status & CUS_OP) && !check_access(user, ci, CA_AUTOOP)
            && !check_access(user, ci, CA_OPDEOPME))
            rem_modes |= CUS_OP;
        if (ircd->halfop && (status & CUS_HALFOP)
            && !check_access(user, ci, CA_AUTOHALFOP)
            && !check_access(user, ci, CA_HALFOPME))
            rem_modes |= CUS_HALFOP;
    }

    /* No modes to add or remove, exit function -GD */
    if (!add_modes && !rem_modes)
        return;

    /* No need for strn* functions for modebuf, as every possible string
     * will always fit in. -GD
     */
    strcpy(modebuf, "");
    strcpy(userbuf, "");
    if (add_modes > 0) {
        strcat(modebuf, "+");
        if ((add_modes & CUS_OWNER) && !(status & CUS_OWNER)) {
            tmp = stripModePrefix(ircd->ownerset);
            strcat(modebuf, tmp);
            free(tmp);
            strcat(userbuf, " ");
            strcat(userbuf, user->nick);
        } else {
            add_modes &= ~CUS_OWNER;
        }
        if ((add_modes & CUS_PROTECT) && !(status & CUS_PROTECT)) {
            tmp = stripModePrefix(ircd->adminset);
            strcat(modebuf, tmp);
            free(tmp);
            strcat(userbuf, " ");
            strcat(userbuf, user->nick);
        } else {
            add_modes &= ~CUS_PROTECT;
        }
        if ((add_modes & CUS_OP) && !(status & CUS_OP)) {
            strcat(modebuf, "o");
            strcat(userbuf, " ");
            strcat(userbuf, user->nick);
            rem_modes |= CUS_DEOPPED;
        } else {
            add_modes &= ~CUS_OP;
        }
        if ((add_modes & CUS_HALFOP) && !(status & CUS_HALFOP)) {
            strcat(modebuf, "h");
            strcat(userbuf, " ");
            strcat(userbuf, user->nick);
        } else {
            add_modes &= ~CUS_HALFOP;
        }
        if ((add_modes & CUS_VOICE) && !(status & CUS_VOICE)) {
            strcat(modebuf, "v");
            strcat(userbuf, " ");
            strcat(userbuf, user->nick);
        } else {
            add_modes &= ~CUS_VOICE;
        }
    }
    if (rem_modes > 0) {
        strcat(modebuf, "-");
        if (rem_modes & CUS_OWNER) {
            tmp = stripModePrefix(ircd->ownerset);
            strcat(modebuf, tmp);
            free(tmp);
            strcat(userbuf, " ");
            strcat(userbuf, user->nick);
        }
        if (rem_modes & CUS_PROTECT) {
            tmp = stripModePrefix(ircd->adminset);
            strcat(modebuf, tmp);
            free(tmp);
            strcat(userbuf, " ");
            strcat(userbuf, user->nick);
        }
        if (rem_modes & CUS_OP) {
            strcat(modebuf, "o");
            strcat(userbuf, " ");
            strcat(userbuf, user->nick);
            add_modes |= CUS_DEOPPED;
        }
        if (rem_modes & CUS_HALFOP) {
            strcat(modebuf, "h");
            strcat(userbuf, " ");
            strcat(userbuf, user->nick);
        }
    }

    /* Here, both can be empty again due to the "isn't it set already?"
     * checks above. -GD
     */
    if (!add_modes && !rem_modes)
        return;

    anope_cmd_mode(whosends(ci), c->name, "%s%s", modebuf, userbuf);
    if (add_modes > 0)
        chan_set_user_status(c, user, add_modes);
    if (rem_modes > 0)
        chan_remove_user_status(c, user, rem_modes);
}

/*************************************************************************/

/* Add/remove a user to/from a channel, creating or deleting the channel as
 * necessary.  If creating the channel, restore mode lock and topic as
 * necessary.  Also check for auto-opping and auto-voicing.
 */

void chan_adduser2(User * user, Channel * c)
{
    struct c_userlist *u;

    u = scalloc(sizeof(struct c_userlist), 1);
    u->next = c->users;
    if (c->users)
        c->users->prev = u;
    c->users = u;
    u->user = user;
    c->usercount++;

    if (get_ignore(user->nick) == NULL) {
        if (c->ci && (check_access(user, c->ci, CA_MEMO))
            && (c->ci->memos.memocount > 0)) {
            if (c->ci->memos.memocount == 1) {
                notice_lang(s_MemoServ, user, MEMO_X_ONE_NOTICE,
                            c->ci->memos.memocount, c->ci->name);
            } else {
                notice_lang(s_MemoServ, user, MEMO_X_MANY_NOTICE,
                            c->ci->memos.memocount, c->ci->name);
            }
        }
        /* Added channelname to entrymsg - 30.03.2004, Certus */
        /* Also, don't send the entrymsg when bursting -GD */
        if (c->ci && c->ci->entry_message && is_sync(user->server))
            notice_user(whosends(c->ci), user, "[%s] %s", c->name,
                        c->ci->entry_message);
    }

    /**
     * We let the bot join even if it was an ignored user, as if we don't, 
     * and the ignored user dosnt just leave, the bot will never
     * make it into the channel, leaving the channel botless even for 
     * legit users - Rob
     **/
    if (s_BotServ && c->ci && c->ci->bi) {
        if (c->usercount == BSMinUsers)
            bot_join(c->ci);
        if (c->usercount >= BSMinUsers && (c->ci->botflags & BS_GREET)
            && user->na && user->na->nc->greet
            && check_access(user, c->ci, CA_GREET)) {
            /* Only display the greet if the main uplink we're connected
             * to has synced, or we'll get greet-floods when the net
             * recovers from a netsplit. -GD
             */
            if (is_sync(user->server)) {
                anope_cmd_privmsg(c->ci->bi->nick, c->name, "[%s] %s",
                                  user->na->nick, user->na->nc->greet);
                c->ci->bi->lastmsg = time(NULL);
            }
        }
    }
}

/*************************************************************************/

/* This creates the channel structure (was originally in
   chan_adduser, but splitted to make it more efficient to use for
   SJOINs). */

Channel *chan_create(char *chan, time_t ts)
{
    Channel *c;
    Channel **list;

    if (debug)
        alog("debug: Creating channel %s", chan);
    /* Allocate pre-cleared memory */
    c = scalloc(sizeof(Channel), 1);
    strscpy(c->name, chan, sizeof(c->name));
    list = &chanlist[HASH(c->name)];
    c->next = *list;
    if (*list)
        (*list)->prev = c;
    *list = c;
    c->creation_time = ts;
    /* Store ChannelInfo pointer in channel record */
    c->ci = cs_findchan(chan);
    if (c->ci)
        c->ci->c = c;
    /* Restore locked modes and saved topic */
    if (c->ci) {
        check_modes(c);
        stick_all(c->ci);
    }

    if (serv_uplink && is_sync(serv_uplink) && (!(c->topic_sync))) {
        restore_topic(chan);
    }

    return c;
}

/*************************************************************************/

/* This destroys the channel structure, freeing everything in it. */

void chan_delete(Channel * c)
{
    BanData *bd, *next;
    int i;

    if (debug)
        alog("debug: Deleting channel %s", c->name);

    for (bd = c->bd; bd; bd = next) {
        if (bd->mask)
            free(bd->mask);
        next = bd->next;
        free(bd);
    }

    if (c->ci)
        c->ci->c = NULL;

    if (c->topic)
        free(c->topic);

    if (c->key)
        free(c->key);
    if (ircd->fmode) {
        if (c->flood)
            free(c->flood);
    }
    if (ircd->Lmode) {
        if (c->redirect)
            free(c->redirect);
    }

    for (i = 0; i < c->bancount; ++i) {
        if (c->bans[i])
            free(c->bans[i]);
        else
            alog("channel: BUG freeing %s: bans[%d] is NULL!", c->name, i);
    }
    if (c->bansize)
        free(c->bans);

    if (ircd->except) {
        for (i = 0; i < c->exceptcount; ++i) {
            if (c->excepts[i])
                free(c->excepts[i]);
            else
                alog("channel: BUG freeing %s: excepts[%d] is NULL!",
                     c->name, i);
        }
        if (c->exceptsize)
            free(c->excepts);
    }

    if (ircd->invitemode) {
        for (i = 0; i < c->invitecount; ++i) {
            if (c->invite[i])
                free(c->invite[i]);
            else
                alog("channel: BUG freeing %s: invite[%d] is NULL!",
                     c->name, i);
        }
        if (c->invitesize)
            free(c->invite);
    }

    if (c->next)
        c->next->prev = c->prev;
    if (c->prev)
        c->prev->next = c->next;
    else
        chanlist[HASH(c->name)] = c->next;

    free(c);
}

/*************************************************************************/

void del_ban(Channel * chan, char *mask)
{
    char **s = chan->bans;
    int i = 0;
    AutoKick *akick;

    /* Sanity check as it seems some IRCD will just send -b without a mask */
    if (!mask) {
        return;
    }

    while (i < chan->bancount && strcmp(*s, mask) != 0) {
        i++;
        s++;
    }

    if (i < chan->bancount) {
        chan->bancount--;
        if (i < chan->bancount)
            memmove(s, s + 1, sizeof(char *) * (chan->bancount - i));

        if (debug)
            alog("debug: Deleted ban %s from channel %s", mask,
                 chan->name);
    }

    if (chan->ci && (akick = is_stuck(chan->ci, mask)))
        stick_mask(chan->ci, akick);
}

/*************************************************************************/

void del_exception(Channel * chan, char *mask)
{
    int i;
    int reset = 0;

    /* Sanity check as it seems some IRCD will just send -e without a mask */
    if (!mask) {
        return;
    }

    for (i = 0; i < chan->exceptcount; i++) {
        if ((!reset) && (stricmp(chan->excepts[i], mask) == 0)) {
            free(chan->excepts[i]);
            reset = 1;
        }
        if (reset)
            chan->excepts[i] =
                (i == chan->exceptcount) ? NULL : chan->excepts[i + 1];
    }

    if (reset)
        chan->exceptcount--;

    if (debug)
        alog("debug: Deleted except %s to channel %s", mask, chan->name);
}

/*************************************************************************/

void del_invite(Channel * chan, char *mask)
{
    int i;
    int reset = 0;

    /* Sanity check as it seems some IRCD will just send -I without a mask */
    if (!mask) {
        return;
    }

    for (i = 0; i < chan->invitecount; i++) {
        if ((!reset) && (stricmp(chan->invite[i], mask) == 0)) {
            free(chan->invite[i]);
            reset = 1;
        }
        if (reset)
            chan->invite[i] =
                (i == chan->invitecount) ? NULL : chan->invite[i + 1];
    }

    if (reset)
        chan->invitecount--;

    if (debug)
        alog("debug: Deleted invite %s to channel %s", mask, chan->name);
}


/*************************************************************************/

char *get_flood(Channel * chan)
{
    return chan->flood;
}

/*************************************************************************/

char *get_key(Channel * chan)
{
    return chan->key;
}

/*************************************************************************/

char *get_limit(Channel * chan)
{
    static char limit[16];

    if (chan->limit == 0)
        return NULL;

    snprintf(limit, sizeof(limit), "%lu", (unsigned long int) chan->limit);
    return limit;
}

/*************************************************************************/

char *get_redirect(Channel * chan)
{
    return chan->redirect;
}

/*************************************************************************/

Channel *join_user_update(User * user, Channel * chan, char *name,
                          time_t chants)
{
    struct u_chanlist *c;

    /* If it's a new channel, so we need to create it first. */
    if (!chan)
        chan = chan_create(name, chants);

    if (debug)
        alog("debug: %s joins %s", user->nick, chan->name);

    c = scalloc(sizeof(*c), 1);
    c->next = user->chans;
    if (user->chans)
        user->chans->prev = c;
    user->chans = c;
    c->chan = chan;

    chan_adduser2(user, chan);

    return chan;
}

/*************************************************************************/

void set_flood(Channel * chan, char *value)
{
    if (chan->flood)
        free(chan->flood);
    chan->flood = value ? sstrdup(value) : NULL;

    if (debug)
        alog("debug: Flood mode for channel %s set to %s", chan->name,
             chan->flood ? chan->flood : "no flood settings");
}

/*************************************************************************/

void chan_set_key(Channel * chan, char *value)
{
    if (chan->key)
        free(chan->key);
    chan->key = value ? sstrdup(value) : NULL;

    if (debug)
        alog("debug: Key of channel %s set to %s", chan->name,
             chan->key ? chan->key : "no key");
}

/*************************************************************************/

void set_limit(Channel * chan, char *value)
{
    chan->limit = value ? strtoul(value, NULL, 10) : 0;

    if (debug)
        alog("debug: Limit of channel %s set to %u", chan->name,
             chan->limit);
}

/*************************************************************************/

void set_redirect(Channel * chan, char *value)
{
    if (chan->redirect)
        free(chan->redirect);
    chan->redirect = value ? sstrdup(value) : NULL;

    if (debug)
        alog("debug: Redirect of channel %s set to %s", chan->name,
             chan->redirect ? chan->redirect : "no redirect");
}

void do_mass_mode(char *modes)
{
    int ac;
    char **av;
    Channel *c;
    char *myModes;

    if (!modes) {
        return;
    }

    /* Prevent modes being altered by split_buf */
    myModes = sstrdup(modes);
    ac = split_buf(myModes, &av, 1);

    for (c = firstchan(); c; c = nextchan()) {
        if (c->bouncy_modes) {
            free(av);
            free(myModes);
            return;
        } else {
            anope_cmd_mode(s_OperServ, c->name, "%s", modes);
            chan_set_modes(s_OperServ, c, ac, av, 1);
        }
    }
    free(av);
    free(myModes);
}

/*************************************************************************/

void restore_unsynced_topics(void)
{
    Channel *c;

    for (c = firstchan(); c; c = nextchan()) {
        if (!(c->topic_sync))
            restore_topic(c->name);
    }
}

/*************************************************************************/
