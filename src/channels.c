/* Channel-handling routines.
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
#include "language.h"

Channel *chanlist[1024];

#define HASH(chan)	((chan)[1] ? ((chan)[1]&31)<<5 | ((chan)[2]&31) : 0)

void add_ban(Channel * chan, char *mask);
void add_exception(Channel * chan, char *mask);
void add_invite(Channel * chan, char *mask);
void chan_adduser2(User * user, Channel * c);
Channel *chan_create(char *chan);
void chan_delete(Channel * c);
void del_ban(Channel * chan, char *mask);
void del_exception(Channel * chan, char *mask);
void del_invite(Channel * chan, char *mask);
char *get_flood(Channel * chan);
char *get_key(Channel * chan);
char *get_limit(Channel * chan);
char *get_redirect(Channel * chan);
Channel *join_user_update(User * user, Channel * chan, char *name);
void set_flood(Channel * chan, char *value);
void set_key(Channel * chan, char *value);
void set_limit(Channel * chan, char *value);
void set_redirect(Channel * chan, char *value);
void do_mass_mode(char *modes);

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
    int servermode = !!strchr(source, '.');
    char *modes = av[0], mode;
    CBMode *cbm;
    CMMode *cmm;
    CUMode *cum;

    if (debug)
        alog("debug: Changing modes for %s to %s", chan->name,
             merge_args(ac, av));

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
            User *user;

            if (ac == 0) {
                alog("channel: mode %c%c with no parameter (?) for channel %s", add ? '+' : '-', mode, chan->name);
                continue;
            }
            ac--;
            av++;

            if ((cum->flags & CUF_PROTECT_BOTSERV) && !add) {
                BotInfo *bi;

                if ((bi = findbot(*av))) {
                    anope_cmd_mode(bi->nick, chan->name, "+%c %s", mode,
                                   bi->nick);
                    continue;
                }
            }

            if (!(user = finduser(*av))) {
                alog("channel: MODE %s %c%c for nonexistent user %s",
                     chan->name, (add ? '+' : '-'), mode, *av);
                continue;
            }

            if (debug)
                alog("debug: Setting %c%c on %s for %s", (add ? '+' : '-'),
                     mode, chan->name, user->nick);

            if (add) {
                if (check && cum->is_valid
                    && !cum->is_valid(user, chan, servermode))
                    continue;
                chan_set_user_status(chan, user, cum->status);
            } else {
                chan_remove_user_status(chan, user, cum->status);
            }
        } else if ((cbm = &cbmodes[(int) mode])->flag != 0) {
            if (add)
                chan->mode |= cbm->flag;
            else
                chan->mode &= ~cbm->flag;

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
        } else if ((cmm = &cmmodes[(int) mode])->addmask) {
            if (ac == 0) {
                alog("channel: mode %c%c with no parameter (?) for channel %s", add ? '+' : '-', mode, chan->name);
                continue;
            }

            ac--;
            av++;
            add ? cmm->addmask(chan, *av) : cmm->delmask(chan, *av);
        }
    }

    if (check)
        check_modes(chan);
}

/*************************************************************************/

/* Set the status of an user on a channel */

void chan_set_user_status(Channel * chan, User * user, int16 status)
{
    struct u_chanlist *uc;

    if (HelpChannel && status == CUS_OP
        && !stricmp(chan->name, HelpChannel))
        common_svsmode(user, "+h", NULL);

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
            alog("Error: findchan() called with NULL values");
        }
        return NULL;
    }

    if (debug >= 3)
        alog("debug: findchan(%p)", chan);
    c = chanlist[HASH(chan)];
    while (c) {
        if (stricmp(c->name, chan) == 0)
            return c;
        c = c->next;
    }
    if (debug >= 3)
        alog("debug: findchan(%s) -> %p", chan, c);
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
    char *s, *t;
    struct u_chanlist *c, *nextc;

    user = finduser(source);
    if (!user) {
        alog("user: JOIN from nonexistent user %s: %s", source,
             merge_args(ac, av));
        return;
    }

    t = av[0];
    while (*(s = t)) {
        t = s + strcspn(s, ",");
        if (*t)
            *t++ = 0;
        if (debug)
            alog("debug: %s joins %s", source, s);

        if (*s == '0') {
            c = user->chans;
            while (c) {
                nextc = c->next;
                chan_deluser(user, c->chan);
                free(c);
                c = nextc;
            }
            user->chans = NULL;
            continue;
        }

        /* Make sure check_kick comes before chan_adduser, so banned users
         * don't get to see things like channel keys. */
        if (check_kick(user, s))
            continue;

/*	chan_adduser(user, s); */
        join_user_update(user, findchan(s), s);


/*        c = scalloc(sizeof(*c), 1);
        c->next = user->chans;
        if (user->chans)
            user->chans->prev = c;
        user->chans = c;
        c->chan = findchan(s); */
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

        user = finduser(s);
        if (!user) {
            alog("user: KICK for nonexistent user %s on %s: %s", s, av[0],
                 merge_args(ac - 2, av + 2));
            continue;
        }
        if (debug)
            alog("debug: kicking %s from %s", s, av[0]);
        for (c = user->chans; c && stricmp(av[0], c->chan->name) != 0;
             c = c->next);
        if (c) {
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

    user = finduser(source);
    if (!user) {
        alog("user: PART from nonexistent user %s: %s", source,
             merge_args(ac, av));
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

   On Connect there is
   SJOIN !11LkOb #ircops +nt :@Trystan

   av[0] = time stamp (base64)
   av[1] = channel
   av[2] = modes
   av[3] = users + bans

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
    char *s, *end, cubuf[ircd->max_symbols + 2], *end2,
        *cumodes[ircd->max_symbols + 1];


    int is_sqlined = 0;
    int ts = 0;

    if (ircd->sjb64) {
        ts = base64dects(av[0]);
    } else {
        ts = strtoul(av[0], NULL, 10);
    }

    /* Double check to avoid unknown modes that need parameters */
    if (ac >= 4 && ac <= 6) {
        c = findchan(av[1]);
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
                if (*s == ircd->sjoinbanchar) {
                    add_ban(c, myStrGetToken(s, ircd->sjoinbanchar, 1));
                    if (!end)
                        break;
                    s = end + 1;
                    continue;
                }
            }
            if (ircd->sjoinexchar) {
                if (*s == ircd->sjoinexchar) {
                    add_exception(c,
                                  myStrGetToken(s, ircd->sjoinexchar, 1));
                    if (!end)
                        break;
                    s = end + 1;
                    continue;
                }
            }

            while (csmodes[(int) *s] != 0)
                *end2++ = csmodes[(int) *s++];
            *end2 = 0;

            user = finduser(s);
            if (!user) {
                alog("user: SJOIN for nonexistent user %s on %s", s,
                     av[1]);
                return;
            }

            if (is_sqlined && !is_oper(user)) {
                anope_cmd_kick(s_OperServ, av[1], s, "Q-Lined");
            } else {
                if (!check_kick(user, av[1])) {
                    /* Make the user join; if the channel does not exist it
                     * will be created there. This ensures that the channel
                     * is not created to be immediately destroyed, and
                     * that the locked key or topic is not shown to anyone
                     * who joins the channel when empty.
                     */
                    c = join_user_update(user, c, av[1]);

                    /* We update user mode on the channel */
                    if (end2 - cubuf > 1) {
                        int i;

                        for (i = 1; i < end2 - cubuf; i++)
                            cumodes[i] = user->nick;
                        chan_set_modes(source, c, 1 + (end2 - cubuf - 1),
                                       cumodes, 1);
                    }
                }
            }

            if (!end)
                break;
            s = end + 1;
        }

        if (c) {
            /* Set the timestamp */
            c->creation_time = ts;
            /* We now update the channel mode. */
            chan_set_modes(source, c, ac - 3, &av[2], 1);
        }

        /* Unreal just had to be different */
    } else if (ac == 3) {
        c = findchan(av[1]);
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

            user = finduser(s);
            if (!user) {
                alog("user: SJOIN for nonexistent user %s on %s", s,
                     av[1]);
                return;
            }

            if (is_sqlined && !is_oper(user)) {
                anope_cmd_kick(s_OperServ, av[1], s, "Q-Lined");
            } else {
                if (!check_kick(user, av[1])) {
                    /* Make the user join; if the channel does not exist it
                     * will be created there. This ensures that the channel
                     * is not created to be immediately destroyed, and
                     * that the locked key or topic is not shown to anyone
                     * who joins the channel when empty.
                     */
                    c = join_user_update(user, c, av[1]);

                    /* We update user mode on the channel */
                    if (end2 - cubuf > 1) {
                        int i;

                        for (i = 1; i < end2 - cubuf; i++)
                            cumodes[i] = user->nick;
                        chan_set_modes(source, c, 1 + (end2 - cubuf - 1),
                                       cumodes, 1);
                    }
                }
            }

            if (!end)
                break;
            s = end + 1;
        }

        if (c) {
            c->creation_time = ts;
        }
    } else if (ac == 2) {
        user = finduser(source);
        if (!user) {
            alog("user: SJOIN for nonexistent user %s on %s", source,
                 av[1]);
            return;
        }

        if (check_kick(user, av[1]))
            return;

        c = findchan(av[1]);
        if (ircd->chansqline) {
            if (!c)
                is_sqlined = check_chan_sqline(av[1]);
        }

        if (is_sqlined && !is_oper(user)) {
            anope_cmd_kick(s_OperServ, av[1], user->nick, "Q-Lined");
        } else {
            c = join_user_update(user, c, av[1]);
            c->creation_time = ts;
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

    chan = findchan(av[0]);
    if (!chan) {
        ci = cs_findchan(av[0]);
        if (!(ci && (ci->flags & CI_VERBOTEN)))
            alog("channel: MODE %s for nonexistent channel %s",
                 merge_args(ac - 1, av + 1), av[0]);
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
    int ts;
    time_t topic_time;

    if (ircd->sjb64) {
        ts = base64dects(av[0]);
    } else {
        ts = strtoul(av[0], NULL, 10);
    }

    topic_time = ts;

    if (!c) {
        alog("channel: TOPIC %s for nonexistent channel %s",
             merge_args(ac - 1, av + 1), av[0]);
        return;
    }

    if (check_topiclock(c, topic_time))
        return;

    if (c->topic) {
        free(c->topic);
        c->topic = NULL;
    }
    if (ac > 3 && *av[3])
        c->topic = sstrdup(av[3]);

    strscpy(c->topic_setter, av[1], sizeof(c->topic_setter));
    c->topic_time = topic_time;

    record_topic(av[0]);
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

/* Add/remove a user to/from a channel, creating or deleting the channel as
 * necessary.  If creating the channel, restore mode lock and topic as
 * necessary.  Also check for auto-opping and auto-voicing.
 * Modified, so ignored users won't get any status via services -certus */


void chan_adduser2(User * user, Channel * c)
{
    struct c_userlist *u;
    char *chan = c->name;

    if (get_ignore(user->nick) == NULL) {

        if (ircd->owner && check_should_owner(user, chan)) {
            chan_set_user_status(c, user, CUS_OWNER | CUS_OP);
        } else if (ircd->protect && check_should_protect(user, chan)) {
            chan_set_user_status(c, user, CUS_PROTECT | CUS_OP);
        } else if (ircd->admin && check_should_protect(user, chan)) {
            chan_set_user_status(c, user, CUS_PROTECT | CUS_OP);
        } else if (check_should_op(user, chan)) {
            chan_set_user_status(c, user, CUS_OP);
        } else if (ircd->halfop && check_should_halfop(user, chan)) {
            chan_set_user_status(c, user, CUS_HALFOP);
        } else if (check_should_voice(user, chan)) {
            chan_set_user_status(c, user, CUS_VOICE);
        }
    }

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
        if (c->ci && c->ci->entry_message)
            notice_user(whosends(c->ci), user, "[%s] %s", c->name,
                        c->ci->entry_message);
    }

    /**
     * We let the bot join even if it was an ignored user, as if we dont, and the ignored user dosnt just leave, the bot will never
     * make it into the channel, leaving the channel botless even for legit users - Rob
     **/
    if (s_BotServ && c->ci && c->ci->bi) {
        if (c->usercount == BSMinUsers)
            bot_join(c->ci);
        if (c->usercount >= BSMinUsers && (c->ci->botflags & BS_GREET)
            && user->na && user->na->nc->greet
            && check_access(user, c->ci, CA_GREET)) {
            anope_cmd_privmsg(c->ci->bi->nick, c->name, "[%s] %s",
                              user->na->nick, user->na->nc->greet);
            c->ci->bi->lastmsg = time(NULL);
        }
    }
}

/*************************************************************************/

/* This creates the channel structure (was originally in
   chan_adduser, but splitted to make it more efficient to use for
   SJOINs). */

Channel *chan_create(char *chan)
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
    c->creation_time = time(NULL);
    /* Store ChannelInfo pointer in channel record */
    c->ci = cs_findchan(chan);
    if (c->ci)
        c->ci->c = c;
    /* Restore locked modes and saved topic */
    if (c->ci) {
        check_modes(c);
        restore_topic(chan);
        stick_all(c->ci);
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

    snprintf(limit, sizeof(limit), "%lu", chan->limit);
    return limit;
}

/*************************************************************************/

char *get_redirect(Channel * chan)
{
    return chan->redirect;
}

/*************************************************************************/

Channel *join_user_update(User * user, Channel * chan, char *name)
{
    struct u_chanlist *c;

    /* If it's a new channel, so we need to create it first. */
    if (!chan)
        chan = chan_create(name);

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

void set_key(Channel * chan, char *value)
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
    int ac, i;
    char **av;
    Channel *c;
    char *myModes;

    if (!modes) {
        return;
    }

    /* Prevent modes being altered by split_buf */
    myModes = sstrdup(modes);
    ac = split_buf(myModes, &av, 1);

    for (i = 0; i < 1024; i++) {
        for (c = chanlist[i]; c; c = c->next) {
            if (c->bouncy_modes) {
                return;
            } else {
                anope_cmd_mode(s_OperServ, c->name, "%s", modes);
                chan_set_modes(s_OperServ, c, ac, av, 1);
            }
        }
    }
}

/*************************************************************************/
