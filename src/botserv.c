/* BotServ functions
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

/*************************************************************************/

#include "services.h"
#include "pseudo.h"

/**
 * RFC: defination of a valid nick
 * nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
 * letter     =  %x41-5A / %x61-7A       ; A-Z / a-z
 * digit      =  %x30-39                 ; 0-9
 * special    =  %x5B-60 / %x7B-7D       ; "[", "]", "\", "`", "_", "^", "{", "|", "}"
 **/
#define isvalidnick(c) ( isalnum(c) || ((c) >='\x5B' && (c) <='\x60') || ((c) >='\x7B' && (c) <='\x7D') || (c)=='-' )


/*************************************************************************/

BotInfo *botlists[256];         /* Hash list of bots */
int nbots = 0;

/*************************************************************************/

BotInfo *makebot(char *nick);
static UserData *get_user_data(Channel * c, User * u);
static void unassign(User * u, ChannelInfo * ci);

static void check_ban(ChannelInfo * ci, User * u, int ttbtype);
static void bot_kick(ChannelInfo * ci, User * u, int message, ...);
static void bot_raw_ban(User * requester, ChannelInfo * ci, char *nick,
                        char *reason);
static void bot_raw_kick(User * requester, ChannelInfo * ci, char *nick,
                         char *reason);
static void bot_raw_mode(User * requester, ChannelInfo * ci, char *mode,
                         char *nick);

static int do_help(User * u);
static int do_bot(User * u);
static int do_botlist(User * u);
static int do_assign(User * u);
static int do_unassign(User * u);
static int do_info(User * u);
static int do_set(User * u);
static int do_kickcmd(User * u);
static int do_badwords(User * u);
static int do_say(User * u);
static int do_act(User * u);
void moduleAddBotServCmds(void);
char *normalizeBuffer(char *);
/*************************************************************************/
/* *INDENT-OFF* */
void moduleAddBotServCmds(void) {
    Command *c;
    c = createCommand("HELP",do_help,NULL,  -1,-1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("BOTLIST",		    do_botlist,  NULL,  BOT_HELP_BOTLIST,            -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("ASSIGN",			    do_assign,   NULL,  BOT_HELP_ASSIGN,             -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("UNASSIGN",		    do_unassign, NULL,  BOT_HELP_UNASSIGN,           -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("INFO",               do_info,     NULL,  BOT_HELP_INFO,               -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("SET",                do_set,      NULL,  BOT_HELP_SET,-1, BOT_SERVADMIN_HELP_SET,BOT_SERVADMIN_HELP_SET, BOT_SERVADMIN_HELP_SET); addCoreCommand(BOTSERV,c);
    c = createCommand("SET DONTKICKOPS",    NULL,        NULL,  BOT_HELP_SET_DONTKICKOPS,    -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("SET DONTKICKVOICES", NULL,        NULL,  BOT_HELP_SET_DONTKICKVOICES, -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("SET FANTASY",		NULL,		 NULL,	BOT_HELP_SET_FANTASY,		 -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("SET GREET",			NULL,		 NULL,	BOT_HELP_SET_GREET,		 	 -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("SET SYMBIOSIS",		NULL,		 NULL,	BOT_HELP_SET_SYMBIOSIS,		 -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("KICK",               do_kickcmd,  NULL,  BOT_HELP_KICK,               -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("KICK BADWORDS",      NULL,        NULL,  BOT_HELP_KICK_BADWORDS,      -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("KICK BOLDS",         NULL,        NULL,  BOT_HELP_KICK_BOLDS,         -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("KICK CAPS",          NULL,        NULL,  BOT_HELP_KICK_CAPS,          -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("KICK COLORS",        NULL,        NULL,  BOT_HELP_KICK_COLORS,        -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("KICK FLOOD",         NULL,        NULL,  BOT_HELP_KICK_FLOOD,         -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("KICK REPEAT",        NULL,        NULL,  BOT_HELP_KICK_REPEAT,        -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("KICK REVERSES",      NULL,        NULL,  BOT_HELP_KICK_REVERSES,      -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("KICK UNDERLINES",    NULL,        NULL,  BOT_HELP_KICK_UNDERLINES,    -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("BADWORDS",           do_badwords, NULL,  BOT_HELP_BADWORDS,           -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("SAY",				do_say,		 NULL,  BOT_HELP_SAY,				 -1,-1,-1,-1); addCoreCommand(BOTSERV,c);
    c = createCommand("ACT",				do_act,		 NULL,  BOT_HELP_ACT,				 -1,-1,-1,-1); addCoreCommand(BOTSERV,c);

    /* Services admins commands */
    c = createCommand("BOT",		do_bot,   is_services_admin,  -1,-1, BOT_SERVADMIN_HELP_BOT,BOT_SERVADMIN_HELP_BOT, BOT_SERVADMIN_HELP_BOT); addCoreCommand(BOTSERV,c);
    c = createCommand("SET NOBOT", 	NULL,	 NULL,	-1,-1, BOT_SERVADMIN_HELP_SET_NOBOT,BOT_SERVADMIN_HELP_SET_NOBOT, BOT_SERVADMIN_HELP_SET_NOBOT); addCoreCommand(BOTSERV,c);
    c = createCommand("SET PRIVATE", NULL,	 NULL,	-1,-1, BOT_SERVADMIN_HELP_SET_PRIVATE,BOT_SERVADMIN_HELP_SET_PRIVATE, BOT_SERVADMIN_HELP_SET_PRIVATE); addCoreCommand(BOTSERV,c);
}
/* *INDENT-ON* */
/*************************************************************************/
/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_botserv_stats(long *nrec, long *memuse)
{
    long count = 0, mem = 0;
    int i;
    BotInfo *bi;

    for (i = 0; i < 256; i++) {
        for (bi = botlists[i]; bi; bi = bi->next) {
            count++;
            mem += sizeof(*bi);
            mem += strlen(bi->nick) + 1;
            mem += strlen(bi->user) + 1;
            mem += strlen(bi->host) + 1;
            mem += strlen(bi->real) + 1;
        }
    }

    *nrec = count;
    *memuse = mem;
}

/*************************************************************************/
/*************************************************************************/

/* BotServ initialization. */

void bs_init(void)
{
    Command *cmd;
    moduleAddBotServCmds();
    cmd = findCommand(BOTSERV, "SET SYMBIOSIS");
    if (cmd)
        cmd->help_param1 = s_ChanServ;
}

/*************************************************************************/

/* Main BotServ routine. */

void botserv(User * u, char *buf)
{
    char *cmd, *s;

    cmd = strtok(buf, " ");

    if (!cmd) {
        return;
    } else if (stricmp(cmd, "\1PING") == 0) {
        if (!(s = strtok(NULL, "")))
            s = "\1";
        notice(s_BotServ, u->nick, "\1PING %s", s);
    } else if (skeleton) {
        notice_lang(s_BotServ, u, SERVICE_OFFLINE, s_BotServ);
    } else {
        mod_run_cmd(s_BotServ, u, BOTSERV, cmd);
    }

}

/*************************************************************************/

/* Handles all messages sent to bots. (Currently only answers to pings ;) */

void botmsgs(User * u, BotInfo * bi, char *buf)
{
    char *cmd = strtok(buf, " ");
    char *s;

    if (!cmd || !u)
        return;

    if (!stricmp(cmd, "\1PING")) {
        if (!(s = strtok(NULL, "")))
            s = "\1";
        notice(bi->nick, u->nick, "\1PING %s", s);
    }
}

/*************************************************************************/

/* Handles all messages that are sent to registered channels where a
 * bot is on.
 *
 */

void botchanmsgs(User * u, ChannelInfo * ci, char *buf)
{
    int c;
    int16 cstatus = 0;
    char *cmd;
    UserData *ud;


    if (!u || !buf || !ci) {
        return;
    }

    /* Answer to ping if needed, without breaking the buffer. */
    if (!strnicmp(buf, "\1PING", 5))
        notice(ci->bi->nick, u->nick, buf);

    /* If it's a /me, cut the CTCP part at the beginning (not
     * at the end, because one character just doesn't matter,
     * but the ACTION may create strange behaviours with the
     * caps or badwords kickers */
    if (!strnicmp(buf, "\1ACTION ", 8))
        buf += 8;

    /* Now we can make kicker stuff. We try to order the checks
     * from the fastest one to the slowest one, since there's
     * no need to process other kickers if an user is kicked before
     * the last kicker check.
     *
     * But FIRST we check whether the user is protected in any
     * way.
     */

    /* We first retrieve the user status on the channel if needed */
    if (ci->botflags & (BS_DONTKICKOPS | BS_DONTKICKVOICES))
        cstatus = chan_get_user_status(ci->c, u);

    if (buf && !check_access(u, ci, CA_NOKICK) &&
        (!(ci->botflags & BS_DONTKICKOPS)
         || !(cstatus & (CUS_HALFOP | CUS_OP | CUS_OWNER | CUS_PROTECT)))

        && (!(ci->botflags & BS_DONTKICKVOICES) || !(cstatus & CUS_VOICE))) {
        /* Bolds kicker */
        if ((ci->botflags & BS_KICK_BOLDS) && strchr(buf, 2)) {
            check_ban(ci, u, TTB_BOLDS);
            bot_kick(ci, u, BOT_REASON_BOLD);
            return;
        }

        /* Color kicker */
        if ((ci->botflags & BS_KICK_COLORS) && strchr(buf, 3)) {
            check_ban(ci, u, TTB_COLORS);
            bot_kick(ci, u, BOT_REASON_COLOR);
            return;
        }

        /* Reverses kicker */
        if ((ci->botflags & BS_KICK_REVERSES) && strchr(buf, 22)) {
            check_ban(ci, u, TTB_REVERSES);
            bot_kick(ci, u, BOT_REASON_REVERSE);
            return;
        }

        /* Underlines kicker */
        if ((ci->botflags & BS_KICK_UNDERLINES) && strchr(buf, 31)) {
            check_ban(ci, u, TTB_UNDERLINES);
            bot_kick(ci, u, BOT_REASON_UNDERLINE);
            return;
        }

        /* Caps kicker */
        if ((ci->botflags & BS_KICK_CAPS)
            && ((c = strlen(buf)) >= ci->capsmin)) {
            int i = 0;
            char *s = buf;

            do {
                if (isupper(*s))
                    i++;
            } while (*s++);

            if (i >= ci->capsmin && i * 100 / c >= ci->capspercent) {
                check_ban(ci, u, TTB_CAPS);
                bot_kick(ci, u, BOT_REASON_CAPS);
                return;
            }
        }

        /* Bad words kicker */
        if (ci->botflags & BS_KICK_BADWORDS) {
            int i;
            int mustkick = 0;
            char *nbuf;
            BadWord *bw;

            /* Normalize the buffer */
            nbuf = normalizeBuffer(buf);

            for (i = 0, bw = ci->badwords; i < ci->bwcount; i++, bw++) {
                if (!bw->in_use)
                    continue;

                if (bw->type == BW_ANY
                    && ((BSCaseSensitive && strstr(nbuf, bw->word))
                        || (!BSCaseSensitive && stristr(nbuf, bw->word)))) {
                    mustkick = 1;
                } else if (bw->type == BW_SINGLE) {
                    int len = strlen(bw->word);

                    if ((BSCaseSensitive && strstr(nbuf, bw->word))
                        || (!BSCaseSensitive
                            && (!stricmp(nbuf, bw->word)))) {
                        mustkick = 1;
                        /* two next if are quite odd isn't it? =) */
                    } else if ((strchr(nbuf, ' ') == nbuf + len)
                               &&
                               ((BSCaseSensitive
                                 && (strstr(nbuf, bw->word) == nbuf))
                                || (!BSCaseSensitive
                                    && (stristr(nbuf, bw->word) ==
                                        nbuf)))) {
                        mustkick = 1;
                    } else
                        if ((strrchr(nbuf, ' ') ==
                             nbuf + strlen(nbuf) - len - 1)
                            &&
                            ((BSCaseSensitive
                              && (strstr(nbuf, bw->word) ==
                                  nbuf + strlen(nbuf) - len))
                             || (!BSCaseSensitive
                                 && (stristr(nbuf, bw->word) ==
                                     nbuf + strlen(nbuf) - len)))) {
                        mustkick = 1;
                    } else {
                        char *wordbuf = scalloc(len + 3, 1);

                        wordbuf[0] = ' ';
                        wordbuf[len + 1] = ' ';
                        wordbuf[len + 2] = '\0';
                        memcpy(wordbuf + 1, bw->word, len);

                        if ((BSCaseSensitive && (strstr(nbuf, wordbuf)))
                            || (!BSCaseSensitive
                                && (stristr(nbuf, wordbuf))))
                            mustkick = 1;
                    }
                } else if (bw->type == BW_START) {
                    int len = strlen(bw->word);

                    if ((BSCaseSensitive
                         && (!strncmp(nbuf, bw->word, len)))
                        || (!BSCaseSensitive
                            && (!strnicmp(nbuf, bw->word, len)))) {
                        mustkick = 1;
                    } else {
                        char *wordbuf = scalloc(len + 2, 1);

                        memcpy(wordbuf + 1, bw->word, len);
                        wordbuf[0] = ' ';
                        wordbuf[len + 1] = '\0';

                        if ((BSCaseSensitive && (strstr(nbuf, wordbuf)))
                            || (!BSCaseSensitive
                                && (stristr(nbuf, wordbuf))))
                            mustkick = 1;

                        free(wordbuf);
                    }
                } else if (bw->type == BW_END) {
                    int len = strlen(bw->word);

                    if ((BSCaseSensitive
                         &&
                         (!strncmp
                          (nbuf + strlen(nbuf) - len, bw->word, len)))
                        || (!BSCaseSensitive
                            &&
                            (!strnicmp
                             (nbuf + strlen(nbuf) - len, bw->word,
                              len)))) {
                        mustkick = 1;
                    } else {
                        char *wordbuf = scalloc(len + 2, 1);

                        memcpy(wordbuf, bw->word, len);
                        wordbuf[len] = ' ';
                        wordbuf[len + 1] = '\0';

                        if ((BSCaseSensitive && (strstr(nbuf, wordbuf)))
                            || (!BSCaseSensitive
                                && (stristr(nbuf, wordbuf))))
                            mustkick = 1;

                        free(wordbuf);
                    }
                }

                if (mustkick) {
                    check_ban(ci, u, TTB_BADWORDS);
                    if (BSGentleBWReason)
                        bot_kick(ci, u, BOT_REASON_BADWORD_GENTLE);
                    else
                        bot_kick(ci, u, BOT_REASON_BADWORD, bw->word);
                    return;
                }
            }

            /* Free the normalized buffer */
            if (nbuf)
                free(nbuf);
        }

        /* Flood kicker */
        if (ci->botflags & BS_KICK_FLOOD) {
            time_t now = time(NULL);

            ud = get_user_data(ci->c, u);
            if (!ud) {
                return;
            }

            if (now - ud->last_start > ci->floodsecs) {
                ud->last_start = time(NULL);
                ud->lines = 0;
            }

            ud->lines++;
            if (ud->lines >= ci->floodlines) {
                check_ban(ci, u, TTB_FLOOD);
                bot_kick(ci, u, BOT_REASON_FLOOD);
                return;
            }
        }

        /* Repeat kicker */
        if (ci->botflags & BS_KICK_REPEAT) {
            ud = get_user_data(ci->c, u);
            if (!ud) {
                return;
            }
            if (ud->lastline && stricmp(ud->lastline, buf)) {
                free(ud->lastline);
                ud->lastline = sstrdup(buf);
                ud->times = 0;
            } else {
                if (!ud->lastline)
                    ud->lastline = sstrdup(buf);
                ud->times++;
            }

            if (ud->times >= ci->repeattimes) {
                check_ban(ci, u, TTB_REPEAT);
                bot_kick(ci, u, BOT_REASON_REPEAT);
                return;
            }
        }
    }


    /* return if the user is on the ignore list  */
    if (get_ignore(u->nick) != NULL) {
        return;
    }

    /* Fantaisist commands */

    if (buf && (ci->botflags & BS_FANTASY) && *buf == '!'
        && check_access(u, ci, CA_FANTASIA)) {
        cmd = strtok(buf, " ");

        if (cmd) {
            if (!stricmp(cmd, "!deowner") && ircd->owner) {
                if (is_founder(u, ci))
                    bot_raw_mode(u, ci, ircd->ownerunset, u->nick);
            } else if (!stricmp(cmd, "!kb")) {
                char *target = strtok(NULL, " ");
                char *reason = strtok(NULL, "");

                if (!target && check_access(u, ci, CA_BANME)) {
                    bot_raw_ban(u, ci, u->nick, "Requested");
                } else if (target && check_access(u, ci, CA_BAN)) {
                    if (!stricmp(target, ci->bi->nick)) {
                        bot_raw_ban(u, ci, u->nick, "Oops!");
                    } else {
                        if (!reason)
                            bot_raw_ban(u, ci, target, "Requested");
                        else
                            bot_raw_ban(u, ci, target, reason);
                    }
                }
            } else if ((!stricmp(cmd, "!kick")) || (!stricmp(cmd, "!k"))) {
                char *target = strtok(NULL, " ");
                char *reason = strtok(NULL, "");

                if (!target && check_access(u, ci, CA_KICKME)) {
                    bot_raw_kick(u, ci, u->nick, "Requested");
                } else if (target && check_access(u, ci, CA_KICK)) {
                    if (!stricmp(target, ci->bi->nick))
                        bot_raw_kick(u, ci, u->nick, "Oops!");
                    else if (!reason)
                        bot_raw_kick(u, ci, target, "Requested");
                    else
                        bot_raw_kick(u, ci, target, reason);
                }
            } else if (!stricmp(cmd, "!owner") && ircd->owner) {
                if (is_founder(u, ci))
                    bot_raw_mode(u, ci, ircd->ownerset, u->nick);
            } else if (!stricmp(cmd, "!seen")) {
                char *target = strtok(NULL, " ");
                char buf[BUFSIZE];

                if (target) {
                    User *u2;
                    NickAlias *na;
                    ChanAccess *access;

                    if (!stricmp(ci->bi->nick, target)) {
                        /* If we look for the bot */
                        snprintf(buf, sizeof(buf),
                                 getstring(u->na, BOT_SEEN_BOT), u->nick);
                        anope_cmd_privmsg(ci->bi->nick, ci->name, "%s",
                                          buf);
                    } else if (!(na = findnick(target))
                               || (na->status & NS_VERBOTEN)) {
                        /* If the nick is not registered or forbidden */
                        snprintf(buf, sizeof(buf),
                                 getstring(u->na, BOT_SEEN_UNKNOWN),
                                 target);
                        anope_cmd_privmsg(ci->bi->nick, ci->name, "%s",
                                          buf);
                    } else if ((u2 = nc_on_chan(ci->c, na->nc))) {
                        /* If the nick we're looking for is on the channel,
                         * there are three possibilities: it's yourself,
                         * it's the nick we look for, it's an alias of the
                         * nick we look for.
                         */
                        if (u == u2 || (u->na && u->na->nc == na->nc))
                            snprintf(buf, sizeof(buf),
                                     getstring(u->na, BOT_SEEN_YOU),
                                     u->nick);
                        else if (!stricmp(u2->nick, target))
                            snprintf(buf, sizeof(buf),
                                     getstring(u->na, BOT_SEEN_ON_CHANNEL),
                                     u2->nick);
                        else
                            snprintf(buf, sizeof(buf),
                                     getstring(u->na,
                                               BOT_SEEN_ON_CHANNEL_AS),
                                     target, u2->nick);
                        anope_cmd_privmsg(ci->bi->nick, ci->name, "%s",
                                          buf);
                    } else if ((access = get_access_entry(na->nc, ci))) {
                        /* User is on the access list but not present actually.
                           Special case: if access->last_seen is 0 it's that we
                           never seen the user.
                         */
                        if (access->last_seen) {
                            char durastr[192];
                            duration(u->na, durastr, sizeof(durastr),
                                     time(NULL) - access->last_seen);
                            snprintf(buf, sizeof(buf),
                                     getstring(u->na, BOT_SEEN_ON), target,
                                     durastr);
                        } else {
                            snprintf(buf, sizeof(buf),
                                     getstring(u->na, BOT_SEEN_NEVER),
                                     target);
                        }
                        anope_cmd_privmsg(ci->bi->nick, ci->name, "%s",
                                          buf);
                    } else {
                        /* All other cases */
                        snprintf(buf, sizeof(buf),
                                 getstring(u->na, BOT_SEEN_UNKNOWN),
                                 target);
                        anope_cmd_privmsg(ci->bi->nick, ci->name, "%s",
                                          buf);
                    }
                }
            } else if (!stricmp(cmd, "!unban")
                       && check_access(u, ci, CA_UNBAN)) {
                char *target = strtok(NULL, " ");

                if (!target)
                    common_unban(ci, u->nick);
                else
                    common_unban(ci, target);
            } else {
                CSModeUtil *util = csmodeutils;

                do {
                    if (!stricmp(cmd, util->bsname)) {
                        char *target = strtok(NULL, " ");

                        if (!target
                            && check_access(u, ci, util->levelself))
                            bot_raw_mode(u, ci, util->mode, u->nick);
                        else if (target
                                 && check_access(u, ci, util->level))
                            bot_raw_mode(u, ci, util->mode, target);
                    }
                } while ((++util)->name != NULL);
            }
        }
    }
}

/*************************************************************************/

/* Load/save data files. */


#define SAFE(x) do {					\
    if ((x) < 0) {					\
	if (!forceload)					\
	    fatal("Read error on %s", BotDBName);	\
	failed = 1;					\
	break;						\
    }							\
} while (0)

void load_bs_dbase(void)
{
    dbFILE *f;
    int c, ver;
    int16 tmp16;
    int32 tmp32;
    BotInfo *bi;
    int failed = 0;

    if (!(f = open_db(s_BotServ, BotDBName, "r", BOT_VERSION)))
        return;

    ver = get_file_version(f);

    while (!failed && (c = getc_db(f)) != 0) {
        char *s;

        if (c != 1)
            fatal("Invalid format in %s %d", BotDBName, c);

        SAFE(read_string(&s, f));
        bi = makebot(s);
        free(s);
        SAFE(read_string(&bi->user, f));
        SAFE(read_string(&bi->host, f));
        SAFE(read_string(&bi->real, f));
        if (ver >= 10) {
            SAFE(read_int16(&tmp16, f));
            bi->flags = tmp16;
        }
        SAFE(read_int32(&tmp32, f));
        bi->created = tmp32;
        SAFE(read_int16(&tmp16, f));
        bi->chancount = tmp16;
    }

    close_db(f);
}

#undef SAFE

/*************************************************************************/

#define SAFE(x) do {						\
    if ((x) < 0) {						\
	restore_db(f);						\
	log_perror("Write error on %s", BotDBName);		\
	if (time(NULL) - lastwarn > WarningTimeout) {		\
	    anope_cmd_global(NULL, "Write error on %s: %s", BotDBName,	\
			strerror(errno));			\
	    lastwarn = time(NULL);				\
	}							\
	return;							\
    }								\
} while (0)

void save_bs_dbase(void)
{
    dbFILE *f;
    BotInfo *bi;
    static time_t lastwarn = 0;
    int i;

    if (!(f = open_db(s_BotServ, BotDBName, "w", BOT_VERSION)))
        return;

    for (i = 0; i < 256; i++) {
        for (bi = botlists[i]; bi; bi = bi->next) {
            SAFE(write_int8(1, f));
            SAFE(write_string(bi->nick, f));
            SAFE(write_string(bi->user, f));
            SAFE(write_string(bi->host, f));
            SAFE(write_string(bi->real, f));
            SAFE(write_int16(bi->flags, f));
            SAFE(write_int32(bi->created, f));
            SAFE(write_int16(bi->chancount, f));
        }
    }
    SAFE(write_int8(0, f));

    close_db(f);

}

#undef SAFE

/*************************************************************************/

void save_bs_rdb_dbase(void)
{
#ifdef USE_RDB
    int i;
    BotInfo *bi;

    if (!rdb_open())
        return;

    rdb_clear_table("anope_bs_core");

    for (i = 0; i < 256; i++) {
        for (bi = botlists[i]; bi; bi = bi->next) {
            rdb_save_bs_core(bi);
        }
    }
    rdb_close();
#endif
}

/*************************************************************************/

/* Inserts a bot in the bot list. I can't be much explicit mh? */

static void insert_bot(BotInfo * bi)
{
    BotInfo *ptr, *prev;

    for (prev = NULL, ptr = botlists[tolower(*bi->nick)];
         ptr != NULL && stricmp(ptr->nick, bi->nick) < 0;
         prev = ptr, ptr = ptr->next);
    bi->prev = prev;
    bi->next = ptr;
    if (!prev)
        botlists[tolower(*bi->nick)] = bi;
    else
        prev->next = bi;
    if (ptr)
        ptr->prev = bi;
}

/*************************************************************************/

BotInfo *makebot(char *nick)
{
    BotInfo *bi;

    if (!nick) {
        if (debug) {
            alog("debug: makebot called with NULL values");
        }
        return NULL;
    }

    bi = scalloc(sizeof(BotInfo), 1);
    bi->nick = sstrdup(nick);
    bi->lastmsg = time(NULL);
    insert_bot(bi);
    nbots++;
    return bi;
}

/*************************************************************************/

static void change_bot_nick(BotInfo * bi, char *newnick)
{
    if (bi->next)
        bi->next->prev = bi->prev;
    if (bi->prev)
        bi->prev->next = bi->next;
    else
        botlists[tolower(*bi->nick)] = bi->next;

    if (bi->nick)
        free(bi->nick);
    bi->nick = sstrdup(newnick);

    insert_bot(bi);
}

/*************************************************************************/

static int delbot(BotInfo * bi)
{
    cs_remove_bot(bi);

    if (bi->next)
        bi->next->prev = bi->prev;
    if (bi->prev)
        bi->prev->next = bi->next;
    else
        botlists[tolower(*bi->nick)] = bi->next;

    nbots--;

    free(bi->nick);
    free(bi->user);
    free(bi->host);
    free(bi->real);

    free(bi);

    return 1;
}

/*************************************************************************/

BotInfo *findbot(char *nick)
{
    BotInfo *bi;

    if (!nick || !*nick)
        return NULL;

    for (bi = botlists[tolower(*nick)]; bi; bi = bi->next)
        if (!stricmp(nick, bi->nick))
            return bi;

    return NULL;
}

/*************************************************************************/

/* Unassign a bot from a channel. Assumes u, ci and ci->bi are not NULL */

static void unassign(User * u, ChannelInfo * ci)
{
    if (ci->c && ci->c->usercount >= BSMinUsers) {
        anope_cmd_part(ci->bi->nick, ci->name, "UNASSIGN from %s",
                       u->nick);
    }
    ci->bi->chancount--;
    ci->bi = NULL;
}

/*************************************************************************/

/* Returns ban data associated with an user if it exists, allocates it
   otherwise. */

static BanData *get_ban_data(Channel * c, User * u)
{
    char mask[BUFSIZE];
    BanData *bd, *next;
    time_t now = time(NULL);

    if (!c || !u)
        return NULL;

    snprintf(mask, sizeof(mask), "%s@%s", u->username,
             common_get_vhost(u));

    for (bd = c->bd; bd; bd = next) {
        if (now - bd->last_use > BSKeepData) {
            if (bd->next)
                bd->next->prev = bd->prev;
            if (bd->prev)
                bd->prev->next = bd->next;
            else
                c->bd = bd->next;
            if (bd->mask)
                free(bd->mask);
            next = bd->next;
            free(bd);
            continue;
        }
        if (!stricmp(bd->mask, mask)) {
            bd->last_use = now;
            return bd;
        }
        next = bd->next;
    }

    /* If we fall here it is that we haven't found the record */
    bd = scalloc(sizeof(BanData), 1);
    bd->mask = sstrdup(mask);
    bd->last_use = now;

    bd->prev = NULL;
    bd->next = c->bd;
    if (bd->next)
        bd->next->prev = bd;
    c->bd = bd;

    return bd;
}

/*************************************************************************/

/* Returns BotServ data associated with an user on a given channel.
 * Allocates it if necessary.
 */

static UserData *get_user_data(Channel * c, User * u)
{
    struct c_userlist *user;

    if (!c || !u)
        return NULL;

    for (user = c->users; user; user = user->next) {
        if (user->user == u) {
            if (user->ud) {
                time_t now = time(NULL);

                /* Checks whether data is obsolete */
                if (now - user->ud->last_use > BSKeepData) {
                    if (user->ud->lastline)
                        free(user->ud->lastline);
                    /* We should not free and realloc, but reset to 0
                       instead. */
                    memset(user->ud, 0, sizeof(UserData));
                    user->ud->last_use = now;
                }

                return user->ud;
            } else {
                user->ud = scalloc(sizeof(UserData), 1);
                user->ud->last_use = time(NULL);
                return user->ud;
            }
        }
    }

    return NULL;
}

/*************************************************************************/

/* Makes the bot join a channel and op himself. */

void bot_join(ChannelInfo * ci)
{
    int i;

    if (!ci || !ci->c || !ci->bi)
        return;

    if (BSSmartJoin) {
        /* We check for bans */
        int count = ci->c->bancount;
        if (count) {
            char botmask[BUFSIZE];
            char **bans = scalloc(sizeof(char *) * count, 1);
            char *av[3];

            memcpy(bans, ci->c->bans, sizeof(char *) * count);
            snprintf(botmask, sizeof(botmask), "%s!%s@%s", ci->bi->nick,
                     ci->bi->user, ci->bi->host);

            av[0] = ci->c->name;
            av[1] = sstrdup("-b");
            for (i = 0; i < count; i++) {
                if (match_wild_nocase(ci->c->bans[i], botmask)) {
                    anope_cmd_mode(ci->bi->nick, ci->name, "%s", bans[i]);
                    av[2] = sstrdup(bans[i]);
                    do_cmode(ci->bi->nick, 3, av);
                    free(av[2]);
                }
            }
            free(av[1]);
            free(bans);
        }

        /* Should we be invited? */
        if ((ci->c->mode & CMODE_i)
            || (ci->c->limit && ci->c->usercount >= ci->c->limit))
            anope_cmd_notice_ops(NULL, ci->c->name,
                                 "%s invited %s into the channel.",
                                 ci->bi->nick, ci->bi->nick);
    }
    anope_cmd_join(ci->bi->nick, ci->c->name, ci->c->creation_time);
    anope_cmd_bot_chan_mode(ci->bi->nick, ci->c->name);
}

/*************************************************************************/

/* This makes the bot rejoin all channel he is on when he gets killed
 * or changed.
 */

void bot_rejoin_all(BotInfo * bi)
{
    int i;
    ChannelInfo *ci;

    for (i = 0; i < 256; i++)
        for (ci = chanlists[i]; ci; ci = ci->next)
            if (ci->bi == bi && ci->c && (ci->c->usercount >= BSMinUsers))
                bot_join(ci);
}

/*************************************************************************/

/* This makes a ban if the user has to have one. In every cases it increments
   the kick count for the user. */

static void check_ban(ChannelInfo * ci, User * u, int ttbtype)
{
    BanData *bd = get_ban_data(ci->c, u);

    if (!bd)
        return;

    bd->ttb[ttbtype]++;
    if (bd->ttb[ttbtype] == ci->ttb[ttbtype]) {
        char *av[3];
        char mask[BUFSIZE];

        bd->ttb[ttbtype] = 0;

        av[0] = ci->name;
        av[1] = sstrdup("+b");
        get_idealban(ci, u, mask, sizeof(mask));
        av[2] = mask;
        anope_cmd_mode(ci->bi->nick, av[0], "+b %s", av[2]);
        do_cmode(ci->bi->nick, 3, av);
        free(av[1]);
    }
}

/*************************************************************************/

/* This makes a bot kick an user. Works somewhat like notice_lang in fact ;) */

static void bot_kick(ChannelInfo * ci, User * u, int message, ...)
{
    va_list args;
    char buf[1024];
    const char *fmt;
    char *av[3];

    if (!ci || !ci->bi || !ci->c || !u)
        return;

    va_start(args, message);
    fmt = getstring(u->na, message);
    if (!fmt)
        return;
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    av[0] = ci->name;
    av[1] = u->nick;
    av[2] = buf;
    anope_cmd_kick(ci->bi->nick, av[0], av[1], "%s", av[2]);
    do_kick(ci->bi->nick, 3, av);
}

/*************************************************************************/

/* Makes a simple ban and kicks the target */

static void bot_raw_ban(User * requester, ChannelInfo * ci, char *nick,
                        char *reason)
{
    char *av[3];
    char mask[BUFSIZE];
    User *u = finduser(nick);

    if (!u)
        return;

    if (ircd->protectedumode) {
        if (is_protected(u) && (requester != u)) {
            anope_cmd_privmsg(ci->bi->nick, ci->name, "%s",
                              getstring2(NULL, PERMISSION_DENIED));
            return;
        }
    }

    if ((ci->flags & CI_PEACE) && stricmp(requester->nick, nick)
        && (get_access(u, ci) >= get_access(requester, ci)))
        return;

    if (ircd->except) {
        if (is_excepted(ci, u) == 1) {
            anope_cmd_privmsg(ci->bi->nick, ci->name, "%s",
                              getstring2(NULL, BOT_EXCEPT));
            return;
        }
    }

    av[0] = ci->name;
    av[1] = sstrdup("+b");
    get_idealban(ci, u, mask, sizeof(mask));
    av[2] = mask;
    anope_cmd_mode(ci->bi->nick, av[0], "+b %s", av[2]);
    do_cmode(ci->bi->nick, 3, av);
    free(av[1]);

    av[0] = ci->name;
    av[1] = nick;

    if (!reason) {
        av[2] = ci->bi->nick;
    } else {
        if (strlen(reason) > 200)
            reason[200] = '\0';
        av[2] = reason;
    }

    anope_cmd_kick(ci->bi->nick, av[0], av[1], "%s", av[2]);
    do_kick(ci->bi->nick, 3, av);
}

/*************************************************************************/

/* Makes a kick with a "dynamic" reason ;) */

static void bot_raw_kick(User * requester, ChannelInfo * ci, char *nick,
                         char *reason)
{
    char *av[3];
    User *u = finduser(nick);

    if (!u || !is_on_chan(ci->c, u))
        return;

    if (ircd->protectedumode) {
        if (is_protected(u) && (requester != u)) {
            anope_cmd_privmsg(ci->bi->nick, ci->name, "%s",
                              getstring2(NULL, PERMISSION_DENIED));
            return;
        }
    }

    if ((ci->flags & CI_PEACE) && stricmp(requester->nick, nick)
        && (get_access(u, ci) >= get_access(requester, ci)))
        return;

    av[0] = ci->name;
    av[1] = nick;

    if (!reason) {
        av[2] = ci->bi->nick;
    } else {
        if (strlen(reason) > 200)
            reason[200] = '\0';
        av[2] = reason;
    }

    anope_cmd_kick(ci->bi->nick, av[0], av[1], "%s", av[2]);
    do_kick(ci->bi->nick, 3, av);
}

/*************************************************************************/

/* Makes a mode operation on a channel for a nick */

static void bot_raw_mode(User * requester, ChannelInfo * ci, char *mode,
                         char *nick)
{
    char *av[3];
    User *u = finduser(nick);

    if (!u || !is_on_chan(ci->c, u))
        return;

    if (ircd->protectedumode) {
        if (is_protected(u) && *mode == '-' && (requester != u)) {
            anope_cmd_privmsg(ci->bi->nick, ci->name, "%s",
                              getstring2(NULL, PERMISSION_DENIED));
            return;
        }
    }

    if (*mode == '-' && (ci->flags & CI_PEACE)
        && stricmp(requester->nick, nick)
        && (get_access(u, ci) >= get_access(requester, ci)))
        return;

    av[0] = ci->name;
    av[1] = mode;
    av[2] = nick;

    anope_cmd_mode(ci->bi->nick, av[0], "%s %s", av[1], av[2]);
    do_cmode(ci->bi->nick, 3, av);
}

/*************************************************************************/

static int do_help(User * u)
{
    char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_BotServ, u, BOT_HELP, BSMinUsers);
        if (is_services_oper(u))
            notice_help(s_BotServ, u, BOT_SERVADMIN_HELP);
        moduleDisplayHelp(4, u);
    } else {
        mod_help_cmd(s_BotServ, u, BOTSERV, cmd);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_bot(User * u)
{
    BotInfo *bi;
    char *cmd = strtok(NULL, " ");
    char *ch = NULL;

    if (!cmd)
        syntax_error(s_BotServ, u, "BOT", BOT_BOT_SYNTAX);
    else if (!stricmp(cmd, "ADD")) {
        char *nick = strtok(NULL, " ");
        char *user = strtok(NULL, " ");
        char *host = strtok(NULL, " ");
        char *real = strtok(NULL, "");

        if (!nick || !user || !host || !real)
            syntax_error(s_BotServ, u, "BOT", BOT_BOT_SYNTAX);
        else if (readonly)
            notice_lang(s_BotServ, u, BOT_BOT_READONLY);
        else if (findbot(nick))
            notice_lang(s_BotServ, u, BOT_BOT_ALREADY_EXISTS, nick);
        else {
            NickAlias *na;

                /**
		 * Check the nick is valid re RFC 2812
		 **/
            if (isdigit(nick[0]) || nick[0] == '-') {
                notice_lang(s_BotServ, u, BOT_BAD_NICK);
                return MOD_CONT;
            }
            for (ch = nick; *ch && (ch - nick) < NICKMAX; ch++) {
                if (!isvalidnick(*ch)) {
                    notice_lang(s_BotServ, u, BOT_BAD_NICK);
                    return MOD_CONT;
                }
            }
            if (!isValidHost(host, 3)) {
                notice_lang(s_BotServ, u, BOT_BAD_HOST);
                return MOD_CONT;
            }
            for (ch = user; *ch && (ch - user) < USERMAX; ch++) {
                if (!isalnum(*ch)) {
                    notice_lang(s_BotServ, u, BOT_BAD_IDENT);
                    return MOD_CONT;
                }
            }

                /**
		 * Check the host is valid re RFC 2812
		 **/

            /* Check whether it's a services client's nick and return if so - Certus */
            /* use nickIsServices reduce the total number lines of code  - TSL */

            if (nickIsServices(nick, 0)) {
                notice_lang(s_BotServ, u, BOT_BOT_CREATION_FAILED);
                return MOD_CONT;
            }

            /* We check whether the nick is registered, and drop it if so. */
            if ((na = findnick(nick))) {
                if (NSSecureAdmins && nick_is_services_admin(na->nc)
                    && !is_services_root(u)) {
                    notice_lang(s_BotServ, u, PERMISSION_DENIED);
                    return MOD_CONT;
                }
                delnick(na);
            }

            bi = makebot(nick);
            if (!bi) {
                notice_lang(s_BotServ, u, BOT_BOT_CREATION_FAILED);
                return MOD_CONT;
            }

            bi->user = sstrdup(user);
            bi->host = sstrdup(host);
            bi->real = sstrdup(real);
            bi->created = time(NULL);
            bi->chancount = 0;

            /* We check whether user with this nick is online, and kill it if so */
            EnforceQlinedNick(nick, s_BotServ);

            /* We make the bot online, ready to serve */
            anope_cmd_bot_nick(bi->nick, bi->user, bi->host, bi->real,
                               ircd->botserv_bot_mode);

            notice_lang(s_BotServ, u, BOT_BOT_ADDED, bi->nick, bi->user,
                        bi->host, bi->real);
        }
    } else if (!stricmp(cmd, "CHANGE")) {
        char *oldnick = strtok(NULL, " ");
        char *nick = strtok(NULL, " ");
        char *user = strtok(NULL, " ");
        char *host = strtok(NULL, " ");
        char *real = strtok(NULL, "");

        if (!oldnick || !nick)
            syntax_error(s_BotServ, u, "BOT", BOT_BOT_SYNTAX);
        else if (readonly)
            notice_lang(s_BotServ, u, BOT_BOT_READONLY);
        else if (!(bi = findbot(oldnick)))
            notice_lang(s_BotServ, u, BOT_DOES_NOT_EXIST, oldnick);
        else {
            NickAlias *na;

            /* Checks whether there *are* changes.
             * Case sensitive because we may want to change just the case.
             * And we must finally check that the nick is not already
             * taken by another bot.
             */
            if (!strcmp(bi->nick, nick)
                && ((user) ? !strcmp(bi->user, user) : 1)
                && ((host) ? !strcmp(bi->host, host) : 1)
                && ((real) ? !strcmp(bi->real, real) : 1)) {
                notice_lang(s_BotServ, u, BOT_BOT_ANY_CHANGES);
                return MOD_CONT;
            }

            /* Check whether it's a services client's nick and return if so - Certus */
            /* use nickIsServices() to reduce the number of lines of code  - TSL */
            if (nickIsServices(nick, 0)) {
                notice_lang(s_BotServ, u, BOT_BOT_CREATION_FAILED);
                return MOD_CONT;
            }

               /**
		 * Check the nick is valid re RFC 2812
		 **/
            if (isdigit(nick[0]) || nick[0] == '-') {
                notice_lang(s_BotServ, u, BOT_BAD_NICK);
                return MOD_CONT;
            }
            for (ch = nick; *ch && (ch - nick) < NICKMAX; ch++) {
                if (!isvalidnick(*ch)) {
                    notice_lang(s_BotServ, u, BOT_BAD_NICK);
                    return MOD_CONT;
                }
            }
            if (!isValidHost(host, 3)) {
                notice_lang(s_BotServ, u, BOT_BAD_HOST);
                return MOD_CONT;
            }

            for (ch = user; *ch && (ch - user) < USERMAX; ch++) {
                if (!isalnum(*ch)) {
                    notice_lang(s_BotServ, u, BOT_BAD_IDENT);
                    return MOD_CONT;
                }
            }

            if (stricmp(bi->nick, nick) && findbot(nick)) {
                notice_lang(s_BotServ, u, BOT_BOT_ALREADY_EXISTS, nick);
                return MOD_CONT;
            }

            if (stricmp(bi->nick, nick)) {
                /* The new nick is really different, so we remove the Q line for
                   the old nick. */
                if (ircd->sqline) {
                    anope_cmd_unsqline(bi->nick);
                }

                /* We check whether the nick is registered, and drop it if so */
                if ((na = findnick(nick)))
                    delnick(na);

                /* We check whether user with this nick is online, and kill it if so */
                EnforceQlinedNick(nick, s_BotServ);
            }

            if (strcmp(nick, bi->nick))
                change_bot_nick(bi, nick);

            if (user && strcmp(user, bi->user)) {
                free(bi->user);
                bi->user = sstrdup(user);
            }
            if (host && strcmp(host, bi->host)) {
                free(bi->host);
                bi->host = sstrdup(host);
            }
            if (real && strcmp(real, bi->real)) {
                free(bi->real);
                bi->real = sstrdup(real);
            }

            /* If only the nick changes, we just make the bot change his nick,
               else we must make it quit and rejoin. */
            if (!user)
                anope_cmd_chg_nick(oldnick, bi->nick);
            else {
                anope_cmd_quit(oldnick, "Quit: Be right back");

                anope_cmd_bot_nick(bi->nick, bi->user, bi->host, bi->real,
                                   ircd->botserv_bot_mode);
                bot_rejoin_all(bi);
            }

            notice_lang(s_BotServ, u, BOT_BOT_CHANGED, oldnick, bi->nick,
                        bi->user, bi->host, bi->real);
        }
    } else if (!stricmp(cmd, "DEL")) {
        char *nick = strtok(NULL, " ");

        if (!nick)
            syntax_error(s_BotServ, u, "BOT", BOT_BOT_SYNTAX);
        else if (readonly)
            notice_lang(s_BotServ, u, BOT_BOT_READONLY);
        else if (!(bi = findbot(nick)))
            notice_lang(s_BotServ, u, BOT_DOES_NOT_EXIST, nick);
        else {
            anope_cmd_quit(bi->nick,
                           "Quit: Help! I'm being deleted by %s!",
                           u->nick);
            if (ircd->sqline) {
                anope_cmd_unsqline(bi->nick);
            }
            delbot(bi);

            notice_lang(s_BotServ, u, BOT_BOT_DELETED, nick);
        }
    } else if (!stricmp(cmd, "LIST"))
        do_botlist(u);
    else
        syntax_error(s_BotServ, u, "BOT", BOT_BOT_SYNTAX);

    return MOD_CONT;
}

/*************************************************************************/

static int do_botlist(User * u)
{
    int i, count = 0;
    BotInfo *bi;

    if (!nbots) {
        notice_lang(s_BotServ, u, BOT_BOTLIST_EMPTY);
        return MOD_CONT;
    }

    for (i = 0; i < 256; i++) {
        for (bi = botlists[i]; bi; bi = bi->next) {
            if (!(bi->flags & BI_PRIVATE)) {
                if (!count)
                    notice_lang(s_BotServ, u, BOT_BOTLIST_HEADER);
                count++;
                notice_user(s_BotServ, u, "   %-15s  (%s@%s)", bi->nick,
                            bi->user, bi->host);
            }
        }
    }

    if (is_oper(u) && count < nbots) {
        notice_lang(s_BotServ, u, BOT_BOTLIST_PRIVATE_HEADER);

        for (i = 0; i < 256; i++) {
            for (bi = botlists[i]; bi; bi = bi->next) {
                if (bi->flags & BI_PRIVATE) {
                    notice_user(s_BotServ, u, "   %-15s  (%s@%s)",
                                bi->nick, bi->user, bi->host);
                    count++;
                }
            }
        }
    }

    if (!count)
        notice_lang(s_BotServ, u, BOT_BOTLIST_EMPTY);
    else
        notice_lang(s_BotServ, u, BOT_BOTLIST_FOOTER, count);
    return MOD_CONT;
}

/*************************************************************************/

static int do_assign(User * u)
{
    char *chan = strtok(NULL, " ");
    char *nick = strtok(NULL, " ");
    BotInfo *bi;
    ChannelInfo *ci;

    if (readonly)
        notice_lang(s_BotServ, u, BOT_ASSIGN_READONLY);
    else if (!chan || !nick)
        syntax_error(s_BotServ, u, "ASSIGN", BOT_ASSIGN_SYNTAX);
    else if (!(bi = findbot(nick)))
        notice_lang(s_BotServ, u, BOT_DOES_NOT_EXIST, nick);
    else if (bi->flags & BI_PRIVATE && !is_oper(u))
        notice_lang(s_BotServ, u, PERMISSION_DENIED);
    else if (!(ci = cs_findchan(chan)))
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    else if (ci->flags & CI_VERBOTEN)
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    else if ((ci->bi) && (stricmp(ci->bi->nick, nick) == 0))
        notice_lang(s_BotServ, u, BOT_ASSIGN_ALREADY, ci->bi->nick, chan);
    else if ((ci->botflags & BS_NOBOT)
             || (!check_access(u, ci, CA_ASSIGN) && !is_services_admin(u)))
        notice_lang(s_BotServ, u, PERMISSION_DENIED);
    else {
        if (ci->bi)
            unassign(u, ci);
        ci->bi = bi;
        bi->chancount++;
        if (ci->c && ci->c->usercount >= BSMinUsers) {
            bot_join(ci);
        }
        notice_lang(s_BotServ, u, BOT_ASSIGN_ASSIGNED, bi->nick, ci->name);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_unassign(User * u)
{
    char *chan = strtok(NULL, " ");
    ChannelInfo *ci;

    if (readonly)
        notice_lang(s_BotServ, u, BOT_ASSIGN_READONLY);
    else if (!chan)
        syntax_error(s_BotServ, u, "UNASSIGN", BOT_UNASSIGN_SYNTAX);
    else if (!(ci = cs_findchan(chan)))
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    else if (ci->flags & CI_VERBOTEN)
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    else if (!is_services_admin(u) && !check_access(u, ci, CA_ASSIGN))
        notice_lang(s_BotServ, u, ACCESS_DENIED);
    else {
        if (ci->bi)
            unassign(u, ci);
        notice_lang(s_BotServ, u, BOT_UNASSIGN_UNASSIGNED, ci->name);
    }
    return MOD_CONT;
}

/*************************************************************************/

static void send_bot_channels(User * u, BotInfo * bi)
{
    int i;
    ChannelInfo *ci;
    char buf[307], *end;

    *buf = 0;
    end = buf;

    for (i = 0; i < 256; i++) {
        for (ci = chanlists[i]; ci; ci = ci->next) {
            if (ci->bi == bi) {
                if (strlen(buf) + strlen(ci->name) > 300) {
                    notice_user(s_BotServ, u, buf);
                    *buf = 0;
                    end = buf;
                }
                end +=
                    snprintf(end, sizeof(buf) - (end - buf), " %s ",
                             ci->name);
            }
        }
    }

    if (*buf)
        notice_user(s_BotServ, u, buf);
    return;
}

static int do_info(User * u)
{
    BotInfo *bi;
    ChannelInfo *ci;
    char *query = strtok(NULL, " ");

    int need_comma = 0, is_servadmin = is_services_admin(u);
    char buf[BUFSIZE], *end;
    const char *commastr = getstring(u->na, COMMA_SPACE);

    if (!query)
        syntax_error(s_BotServ, u, "INFO", BOT_INFO_SYNTAX);
    else if ((bi = findbot(query))) {
        char buf[BUFSIZE];
        struct tm *tm;

        notice_lang(s_BotServ, u, BOT_INFO_BOT_HEADER, bi->nick);
        notice_lang(s_BotServ, u, BOT_INFO_BOT_MASK, bi->user, bi->host);
        notice_lang(s_BotServ, u, BOT_INFO_BOT_REALNAME, bi->real);
        tm = localtime(&bi->created);
        strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
        notice_lang(s_BotServ, u, BOT_INFO_BOT_CREATED, buf);
        notice_lang(s_BotServ, u, BOT_INFO_BOT_OPTIONS,
                    getstring(u->na,
                              (bi->
                               flags & BI_PRIVATE) ? BOT_INFO_OPT_PRIVATE :
                              BOT_INFO_OPT_NONE));
        notice_lang(s_BotServ, u, BOT_INFO_BOT_USAGE, bi->chancount);

        if (is_services_admin(u))
            send_bot_channels(u, bi);
    } else if ((ci = cs_findchan(query))) {
        if (!is_servadmin && !is_founder(u, ci)) {
            notice_lang(s_BotServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        notice_lang(s_BotServ, u, BOT_INFO_CHAN_HEADER, ci->name);
        if (ci->bi)
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_BOT, ci->bi->nick);
        else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_BOT_NONE);

        if (ci->botflags & BS_KICK_BADWORDS) {
            if (ci->ttb[TTB_BADWORDS])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BADWORDS_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_BADWORDS]);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BADWORDS,
                            getstring(u->na, BOT_INFO_ACTIVE));
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BADWORDS,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_BOLDS) {
            if (ci->ttb[TTB_BOLDS])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BOLDS_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_BOLDS]);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BOLDS,
                            getstring(u->na, BOT_INFO_ACTIVE));
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BOLDS,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_CAPS) {
            if (ci->ttb[TTB_CAPS])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_CAPS_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_CAPS], ci->capsmin,
                            ci->capspercent);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_CAPS_ON,
                            getstring(u->na, BOT_INFO_ACTIVE), ci->capsmin,
                            ci->capspercent);
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_CAPS_OFF,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_COLORS) {
            if (ci->ttb[TTB_COLORS])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_COLORS_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_COLORS]);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_COLORS,
                            getstring(u->na, BOT_INFO_ACTIVE));
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_COLORS,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_FLOOD) {
            if (ci->ttb[TTB_FLOOD])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_FLOOD_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_FLOOD], ci->floodlines,
                            ci->floodsecs);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_FLOOD_ON,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->floodlines, ci->floodsecs);
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_FLOOD_OFF,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_REPEAT) {
            if (ci->ttb[TTB_REPEAT])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REPEAT_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_REPEAT], ci->repeattimes);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REPEAT_ON,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->repeattimes);
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REPEAT_OFF,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_REVERSES) {
            if (ci->ttb[TTB_REVERSES])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REVERSES_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_REVERSES]);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REVERSES,
                            getstring(u->na, BOT_INFO_ACTIVE));
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REVERSES,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_UNDERLINES) {
            if (ci->ttb[TTB_UNDERLINES])
                notice_lang(s_BotServ, u,
                            BOT_INFO_CHAN_KICK_UNDERLINES_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_UNDERLINES]);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_UNDERLINES,
                            getstring(u->na, BOT_INFO_ACTIVE));
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_UNDERLINES,
                        getstring(u->na, BOT_INFO_INACTIVE));

        end = buf;
        *end = 0;
        if (ci->botflags & BS_DONTKICKOPS) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%s",
                            getstring(u->na, BOT_INFO_OPT_DONTKICKOPS));
            need_comma = 1;
        }
        if (ci->botflags & BS_DONTKICKVOICES) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->na, BOT_INFO_OPT_DONTKICKVOICES));
            need_comma = 1;
        }
        if (ci->botflags & BS_FANTASY) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->na, BOT_INFO_OPT_FANTASY));
            need_comma = 1;
        }
        if (ci->botflags & BS_GREET) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->na, BOT_INFO_OPT_GREET));
            need_comma = 1;
        }
        if (ci->botflags & BS_NOBOT) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->na, BOT_INFO_OPT_NOBOT));
            need_comma = 1;
        }
        if (ci->botflags & BS_SYMBIOSIS) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->na, BOT_INFO_OPT_SYMBIOSIS));
            need_comma = 1;
        }
        notice_lang(s_BotServ, u, BOT_INFO_CHAN_OPTIONS,
                    *buf ? buf : getstring(u->na, BOT_INFO_OPT_NONE));

    } else
        notice_lang(s_BotServ, u, BOT_INFO_NOT_FOUND, query);
    return MOD_CONT;
}

/*************************************************************************/

static int do_set(User * u)
{
    char *chan = strtok(NULL, " ");
    char *option = strtok(NULL, " ");
    char *value = strtok(NULL, " ");
    int is_servadmin = is_services_admin(u);

    ChannelInfo *ci;

    if (readonly)
        notice_lang(s_BotServ, u, BOT_SET_DISABLED);
    else if (!chan || !option || !value)
        syntax_error(s_BotServ, u, "SET", BOT_SET_SYNTAX);
    else if (is_servadmin && !stricmp(option, "PRIVATE")) {
        BotInfo *bi;

        if ((bi = findbot(chan))) {
            if (!stricmp(value, "ON")) {
                bi->flags |= BI_PRIVATE;
                notice_lang(s_BotServ, u, BOT_SET_PRIVATE_ON, bi->nick);
            } else if (!stricmp(value, "OFF")) {
                bi->flags &= ~BI_PRIVATE;
                notice_lang(s_BotServ, u, BOT_SET_PRIVATE_OFF, bi->nick);
            } else {
                syntax_error(s_BotServ, u, "SET PRIVATE",
                             BOT_SET_PRIVATE_SYNTAX);
            }
        } else {
            notice_lang(s_BotServ, u, BOT_DOES_NOT_EXIST, chan);
        }
        return MOD_CONT;
    } else if (!(ci = cs_findchan(chan)))
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    else if (ci->flags & CI_VERBOTEN)
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    else if (!is_servadmin && !check_access(u, ci, CA_SET))
        notice_lang(s_BotServ, u, ACCESS_DENIED);
    else {
        if (!stricmp(option, "DONTKICKOPS")) {
            if (!stricmp(value, "ON")) {
                ci->botflags |= BS_DONTKICKOPS;
                notice_lang(s_BotServ, u, BOT_SET_DONTKICKOPS_ON,
                            ci->name);
            } else if (!stricmp(value, "OFF")) {
                ci->botflags &= ~BS_DONTKICKOPS;
                notice_lang(s_BotServ, u, BOT_SET_DONTKICKOPS_OFF,
                            ci->name);
            } else {
                syntax_error(s_BotServ, u, "SET DONTKICKOPS",
                             BOT_SET_DONTKICKOPS_SYNTAX);
            }
        } else if (!stricmp(option, "DONTKICKVOICES")) {
            if (!stricmp(value, "ON")) {
                ci->botflags |= BS_DONTKICKVOICES;
                notice_lang(s_BotServ, u, BOT_SET_DONTKICKVOICES_ON,
                            ci->name);
            } else if (!stricmp(value, "OFF")) {
                ci->botflags &= ~BS_DONTKICKVOICES;
                notice_lang(s_BotServ, u, BOT_SET_DONTKICKVOICES_OFF,
                            ci->name);
            } else {
                syntax_error(s_BotServ, u, "SET DONTKICKVOICES",
                             BOT_SET_DONTKICKVOICES_SYNTAX);
            }
        } else if (!stricmp(option, "FANTASY")) {
            if (!stricmp(value, "ON")) {
                ci->botflags |= BS_FANTASY;
                notice_lang(s_BotServ, u, BOT_SET_FANTASY_ON, ci->name);
            } else if (!stricmp(value, "OFF")) {
                ci->botflags &= ~BS_FANTASY;
                notice_lang(s_BotServ, u, BOT_SET_FANTASY_OFF, ci->name);
            } else {
                syntax_error(s_BotServ, u, "SET FANTASY",
                             BOT_SET_FANTASY_SYNTAX);
            }
        } else if (!stricmp(option, "GREET")) {
            if (!stricmp(value, "ON")) {
                ci->botflags |= BS_GREET;
                notice_lang(s_BotServ, u, BOT_SET_GREET_ON, ci->name);
            } else if (!stricmp(value, "OFF")) {
                ci->botflags &= ~BS_GREET;
                notice_lang(s_BotServ, u, BOT_SET_GREET_OFF, ci->name);
            } else {
                syntax_error(s_BotServ, u, "SET GREET",
                             BOT_SET_GREET_SYNTAX);
            }
        } else if (is_servadmin && !stricmp(option, "NOBOT")) {
            if (!stricmp(value, "ON")) {
                ci->botflags |= BS_NOBOT;
                if (ci->bi)
                    unassign(u, ci);
                notice_lang(s_BotServ, u, BOT_SET_NOBOT_ON, ci->name);
            } else if (!stricmp(value, "OFF")) {
                ci->botflags &= ~BS_NOBOT;
                notice_lang(s_BotServ, u, BOT_SET_NOBOT_OFF, ci->name);
            } else {
                syntax_error(s_BotServ, u, "SET NOBOT",
                             BOT_SET_NOBOT_SYNTAX);
            }
        } else if (!stricmp(option, "SYMBIOSIS")) {
            if (!stricmp(value, "ON")) {
                ci->botflags |= BS_SYMBIOSIS;
                notice_lang(s_BotServ, u, BOT_SET_SYMBIOSIS_ON, ci->name);
            } else if (!stricmp(value, "OFF")) {
                ci->botflags &= ~BS_SYMBIOSIS;
                notice_lang(s_BotServ, u, BOT_SET_SYMBIOSIS_OFF, ci->name);
            } else {
                syntax_error(s_BotServ, u, "SET SYMBIOSIS",
                             BOT_SET_SYMBIOSIS_SYNTAX);
            }
        } else {
            notice_help(s_BotServ, u, BOT_SET_UNKNOWN, option);
        }
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_kickcmd(User * u)
{
    char *chan = strtok(NULL, " ");
    char *option = strtok(NULL, " ");
    char *value = strtok(NULL, " ");
    char *ttb = strtok(NULL, " ");

    ChannelInfo *ci;

    if (readonly)
        notice_lang(s_BotServ, u, BOT_KICK_DISABLED);
    else if (!chan || !option || !value)
        syntax_error(s_BotServ, u, "KICK", BOT_KICK_SYNTAX);
    else if (stricmp(value, "ON") && stricmp(value, "OFF"))
        syntax_error(s_BotServ, u, "KICK", BOT_KICK_SYNTAX);
    else if (!(ci = cs_findchan(chan)))
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    else if (ci->flags & CI_VERBOTEN)
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    else if (!is_services_admin(u) && !check_access(u, ci, CA_SET))
        notice_lang(s_BotServ, u, ACCESS_DENIED);
    else {
        if (!stricmp(option, "BADWORDS")) {
            if (!stricmp(value, "ON")) {
                if (ttb) {
                    ci->ttb[TTB_BADWORDS] =
                        strtol(ttb, (char **) NULL, 10);
                    /* Only error if errno returns ERANGE or EINVAL or we are less then 0 - TSL */
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_BADWORDS] < 0) {
                        /* leaving the debug behind since we might want to know what these are */
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_BADWORDS]);
                        }
                        /* reset the value back to 0 - TSL */
                        ci->ttb[TTB_BADWORDS] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else {
                    ci->ttb[TTB_BADWORDS] = 0;
                }
                ci->botflags |= BS_KICK_BADWORDS;
                if (ci->ttb[TTB_BADWORDS])
                    notice_lang(s_BotServ, u, BOT_KICK_BADWORDS_ON_BAN,
                                ci->ttb[TTB_BADWORDS]);
                else
                    notice_lang(s_BotServ, u, BOT_KICK_BADWORDS_ON);
            } else {
                ci->botflags &= ~BS_KICK_BADWORDS;
                notice_lang(s_BotServ, u, BOT_KICK_BADWORDS_OFF);
            }
        } else if (!stricmp(option, "BOLDS")) {
            if (!stricmp(value, "ON")) {
                if (ttb) {
                    ci->ttb[TTB_BOLDS] = strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_BOLDS] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_BOLDS]);
                        }
                        ci->ttb[TTB_BOLDS] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_BOLDS] = 0;
                ci->botflags |= BS_KICK_BOLDS;
                if (ci->ttb[TTB_BOLDS])
                    notice_lang(s_BotServ, u, BOT_KICK_BOLDS_ON_BAN,
                                ci->ttb[TTB_BOLDS]);
                else
                    notice_lang(s_BotServ, u, BOT_KICK_BOLDS_ON);
            } else {
                ci->botflags &= ~BS_KICK_BOLDS;
                notice_lang(s_BotServ, u, BOT_KICK_BOLDS_OFF);
            }
        } else if (!stricmp(option, "CAPS")) {
            if (!stricmp(value, "ON")) {
                char *min = strtok(NULL, " ");
                char *percent = strtok(NULL, " ");

                if (ttb) {
                    ci->ttb[TTB_CAPS] = strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_CAPS] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_CAPS]);
                        }
                        ci->ttb[TTB_CAPS] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_CAPS] = 0;

                if (!min)
                    ci->capsmin = 10;
                else
                    ci->capsmin = atol(min);
                if (ci->capsmin < 1)
                    ci->capsmin = 10;

                if (!percent)
                    ci->capspercent = 25;
                else
                    ci->capspercent = atol(percent);
                if (ci->capspercent < 1 || ci->capspercent > 100)
                    ci->capspercent = 25;

                ci->botflags |= BS_KICK_CAPS;
                if (ci->ttb[TTB_CAPS])
                    notice_lang(s_BotServ, u, BOT_KICK_CAPS_ON_BAN,
                                ci->capsmin, ci->capspercent,
                                ci->ttb[TTB_CAPS]);
                else
                    notice_lang(s_BotServ, u, BOT_KICK_CAPS_ON,
                                ci->capsmin, ci->capspercent);
            } else {
                ci->botflags &= ~BS_KICK_CAPS;
                notice_lang(s_BotServ, u, BOT_KICK_CAPS_OFF);
            }
        } else if (!stricmp(option, "COLORS")) {
            if (!stricmp(value, "ON")) {
                if (ttb) {
                    ci->ttb[TTB_COLORS] = strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_COLORS] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_COLORS]);
                        }
                        ci->ttb[TTB_COLORS] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_COLORS] = 0;
                ci->botflags |= BS_KICK_COLORS;
                if (ci->ttb[TTB_COLORS])
                    notice_lang(s_BotServ, u, BOT_KICK_COLORS_ON_BAN,
                                ci->ttb[TTB_COLORS]);
                else
                    notice_lang(s_BotServ, u, BOT_KICK_COLORS_ON);
            } else {
                ci->botflags &= ~BS_KICK_COLORS;
                notice_lang(s_BotServ, u, BOT_KICK_COLORS_OFF);
            }
        } else if (!stricmp(option, "FLOOD")) {
            if (!stricmp(value, "ON")) {
                char *lines = strtok(NULL, " ");
                char *secs = strtok(NULL, " ");

                if (ttb) {
                    ci->ttb[TTB_FLOOD] = strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_FLOOD] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_FLOOD]);
                        }
                        ci->ttb[TTB_FLOOD] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_FLOOD] = 0;

                if (!lines)
                    ci->floodlines = 6;
                else
                    ci->floodlines = atol(lines);
                if (ci->floodlines < 2)
                    ci->floodlines = 6;

                if (!secs)
                    ci->floodsecs = 10;
                else
                    ci->floodsecs = atol(secs);
                if (ci->floodsecs < 1 || ci->floodsecs > BSKeepData)
                    ci->floodsecs = 10;

                ci->botflags |= BS_KICK_FLOOD;
                if (ci->ttb[TTB_FLOOD])
                    notice_lang(s_BotServ, u, BOT_KICK_FLOOD_ON_BAN,
                                ci->floodlines, ci->floodsecs,
                                ci->ttb[TTB_FLOOD]);
                else
                    notice_lang(s_BotServ, u, BOT_KICK_FLOOD_ON,
                                ci->floodlines, ci->floodsecs);
            } else {
                ci->botflags &= ~BS_KICK_FLOOD;
                notice_lang(s_BotServ, u, BOT_KICK_FLOOD_OFF);
            }
        } else if (!stricmp(option, "REPEAT")) {
            if (!stricmp(value, "ON")) {
                char *times = strtok(NULL, " ");

                if (ttb) {
                    ci->ttb[TTB_REPEAT] = strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_REPEAT] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_REPEAT]);
                        }
                        ci->ttb[TTB_REPEAT] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_REPEAT] = 0;

                if (!times)
                    ci->repeattimes = 3;
                else
                    ci->repeattimes = atol(times);
                if (ci->repeattimes < 2)
                    ci->repeattimes = 3;

                ci->botflags |= BS_KICK_REPEAT;
                if (ci->ttb[TTB_REPEAT])
                    notice_lang(s_BotServ, u, BOT_KICK_REPEAT_ON_BAN,
                                ci->repeattimes, ci->ttb[TTB_REPEAT]);
                else
                    notice_lang(s_BotServ, u, BOT_KICK_REPEAT_ON,
                                ci->repeattimes);
            } else {
                ci->botflags &= ~BS_KICK_REPEAT;
                notice_lang(s_BotServ, u, BOT_KICK_REPEAT_OFF);
            }
        } else if (!stricmp(option, "REVERSES")) {
            if (!stricmp(value, "ON")) {
                if (ttb) {
                    ci->ttb[TTB_REVERSES] =
                        strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_REVERSES] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_REVERSES]);
                        }
                        ci->ttb[TTB_REVERSES] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_REVERSES] = 0;
                ci->botflags |= BS_KICK_REVERSES;
                if (ci->ttb[TTB_REVERSES])
                    notice_lang(s_BotServ, u, BOT_KICK_REVERSES_ON_BAN,
                                ci->ttb[TTB_REVERSES]);
                else
                    notice_lang(s_BotServ, u, BOT_KICK_REVERSES_ON);
            } else {
                ci->botflags &= ~BS_KICK_REVERSES;
                notice_lang(s_BotServ, u, BOT_KICK_REVERSES_OFF);
            }
        } else if (!stricmp(option, "UNDERLINES")) {
            if (!stricmp(value, "ON")) {
                if (ttb) {
                    ci->ttb[TTB_UNDERLINES] =
                        strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_UNDERLINES] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_UNDERLINES]);
                        }
                        ci->ttb[TTB_UNDERLINES] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_UNDERLINES] = 0;
                ci->botflags |= BS_KICK_UNDERLINES;
                if (ci->ttb[TTB_UNDERLINES])
                    notice_lang(s_BotServ, u, BOT_KICK_UNDERLINES_ON_BAN,
                                ci->ttb[TTB_UNDERLINES]);
                else
                    notice_lang(s_BotServ, u, BOT_KICK_UNDERLINES_ON);
            } else {
                ci->botflags &= ~BS_KICK_UNDERLINES;
                notice_lang(s_BotServ, u, BOT_KICK_UNDERLINES_OFF);
            }
        } else
            notice_help(s_BotServ, u, BOT_KICK_UNKNOWN, option);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int badwords_del_callback(User * u, int num, va_list args)
{
    BadWord *bw;
    ChannelInfo *ci = va_arg(args, ChannelInfo *);
    int *last = va_arg(args, int *);
    if (num < 1 || num > ci->bwcount)
        return 0;
    *last = num;

    bw = &ci->badwords[num - 1];
    if (bw->word)
        free(bw->word);
    bw->word = NULL;
    bw->in_use = 0;

    return 1;
}

static int badwords_list(User * u, int index, ChannelInfo * ci,
                         int *sent_header)
{
    BadWord *bw = &ci->badwords[index];

    if (!bw->in_use)
        return 0;
    if (!*sent_header) {
        notice_lang(s_BotServ, u, BOT_BADWORDS_LIST_HEADER, ci->name);
        *sent_header = 1;
    }

    notice_lang(s_BotServ, u, BOT_BADWORDS_LIST_FORMAT, index + 1,
                bw->word,
                ((bw->type ==
                  BW_SINGLE) ? "(SINGLE)" : ((bw->type ==
                                              BW_START) ? "(START)"
                                             : ((bw->type ==
                                                 BW_END) ? "(END)" : "")))
        );
    return 1;
}

static int badwords_list_callback(User * u, int num, va_list args)
{
    ChannelInfo *ci = va_arg(args, ChannelInfo *);
    int *sent_header = va_arg(args, int *);
    if (num < 1 || num > ci->bwcount)
        return 0;
    return badwords_list(u, num - 1, ci, sent_header);
}

static int do_badwords(User * u)
{
    char *chan = strtok(NULL, " ");
    char *cmd = strtok(NULL, " ");
    char *word = strtok(NULL, "");
    ChannelInfo *ci;
    BadWord *bw;

    int i;
    int need_args = (cmd
                     && (!stricmp(cmd, "LIST") || !stricmp(cmd, "CLEAR")));

    if (!cmd || (need_args ? 0 : !word)) {
        syntax_error(s_BotServ, u, "BADWORDS", BOT_BADWORDS_SYNTAX);
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!check_access(u, ci, CA_BADWORDS)
               && (!need_args || !is_services_admin(u))) {
        notice_lang(s_BotServ, u, ACCESS_DENIED);
    } else if (stricmp(cmd, "ADD") == 0) {

        char *opt, *pos;
        int type = BW_ANY;

        if (readonly) {
            notice_lang(s_BotServ, u, BOT_BADWORDS_DISABLED);
            return MOD_CONT;
        }

        pos = strrchr(word, ' ');
        if (pos) {
            opt = pos + 1;
            if (*opt) {
                if (!stricmp(opt, "SINGLE"))
                    type = BW_SINGLE;
                else if (!stricmp(opt, "START"))
                    type = BW_START;
                else if (!stricmp(opt, "END"))
                    type = BW_END;
                if (type != BW_ANY)
                    *pos = 0;
            }
        }

        for (bw = ci->badwords, i = 0; i < ci->bwcount; bw++, i++) {
            if (bw->word && ((BSCaseSensitive && (!strcmp(bw->word, word)))
                             || (!BSCaseSensitive
                                 && (!stricmp(bw->word, word))))) {
                notice_lang(s_BotServ, u, BOT_BADWORDS_ALREADY_EXISTS,
                            bw->word, ci->name);
                return MOD_CONT;
            }
        }

        for (i = 0; i < ci->bwcount; i++) {
            if (!ci->badwords[i].in_use)
                break;
        }
        if (i == ci->bwcount) {
            if (i < BSBadWordsMax) {
                ci->bwcount++;
                ci->badwords =
                    srealloc(ci->badwords, sizeof(BadWord) * ci->bwcount);
            } else {
                notice_lang(s_BotServ, u, BOT_BADWORDS_REACHED_LIMIT,
                            BSBadWordsMax);
                return MOD_CONT;
            }
        }
        bw = &ci->badwords[i];
        bw->in_use = 1;
        bw->word = sstrdup(word);
        bw->type = type;

        notice_lang(s_BotServ, u, BOT_BADWORDS_ADDED, bw->word, ci->name);

    } else if (stricmp(cmd, "DEL") == 0) {

        if (readonly) {
            notice_lang(s_BotServ, u, BOT_BADWORDS_DISABLED);
            return MOD_CONT;
        }

        /* Special case: is it a number/list?  Only do search if it isn't. */
        if (isdigit(*word) && strspn(word, "1234567890,-") == strlen(word)) {
            int count, deleted, last = -1;
            deleted =
                process_numlist(word, &count, badwords_del_callback, u, ci,
                                &last);
            if (!deleted) {
                if (count == 1) {
                    notice_lang(s_BotServ, u, BOT_BADWORDS_NO_SUCH_ENTRY,
                                last, ci->name);
                } else {
                    notice_lang(s_BotServ, u, BOT_BADWORDS_NO_MATCH,
                                ci->name);
                }
            } else if (deleted == 1) {
                notice_lang(s_BotServ, u, BOT_BADWORDS_DELETED_ONE,
                            ci->name);
            } else {
                notice_lang(s_BotServ, u, BOT_BADWORDS_DELETED_SEVERAL,
                            deleted, ci->name);
            }
        } else {
            for (i = 0; i < ci->bwcount; i++) {
                if (ci->badwords[i].in_use
                    && !stricmp(ci->badwords[i].word, word))
                    break;
            }
            if (i == ci->bwcount) {
                notice_lang(s_BotServ, u, BOT_BADWORDS_NOT_FOUND, word,
                            chan);
                return MOD_CONT;
            }
            bw = &ci->badwords[i];
            notice_lang(s_BotServ, u, BOT_BADWORDS_DELETED, bw->word,
                        ci->name);
            if (bw->word)
                free(bw->word);
            bw->word = NULL;
            bw->in_use = 0;
        }

    } else if (stricmp(cmd, "LIST") == 0) {
        int sent_header = 0;

        if (ci->bwcount == 0) {
            notice_lang(s_BotServ, u, BOT_BADWORDS_LIST_EMPTY, chan);
            return MOD_CONT;
        }
        if (word && strspn(word, "1234567890,-") == strlen(word)) {
            process_numlist(word, NULL, badwords_list_callback, u, ci,
                            &sent_header);
        } else {
            for (i = 0; i < ci->bwcount; i++) {
                if (!(ci->badwords[i].in_use))
                    continue;
                if (word && ci->badwords[i].word
                    && !match_wild_nocase(word, ci->badwords[i].word))
                    continue;
                badwords_list(u, i, ci, &sent_header);
            }
        }
        if (!sent_header)
            notice_lang(s_BotServ, u, BOT_BADWORDS_NO_MATCH, chan);

    } else if (stricmp(cmd, "CLEAR") == 0) {

        if (readonly) {
            notice_lang(s_BotServ, u, BOT_BADWORDS_DISABLED);
            return MOD_CONT;
        }

        for (i = 0; i < ci->bwcount; i++)
            if (ci->badwords[i].word)
                free(ci->badwords[i].word);

        free(ci->badwords);
        ci->badwords = NULL;
        ci->bwcount = 0;

        notice_lang(s_BotServ, u, BOT_BADWORDS_CLEAR);

    } else {
        syntax_error(s_BotServ, u, "BADWORDS", BOT_BADWORDS_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_say(User * u)
{
    ChannelInfo *ci;

    char *chan = strtok(NULL, " ");
    char *text = strtok(NULL, "");

    if (!chan || !text)
        syntax_error(s_BotServ, u, "SAY", BOT_SAY_SYNTAX);
    else if (!(ci = cs_findchan(chan)))
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    else if (ci->flags & CI_VERBOTEN)
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    else if (!ci->bi)
        notice_help(s_BotServ, u, BOT_NOT_ASSIGNED);
    else if (!ci->c || ci->c->usercount < BSMinUsers)
        notice_lang(s_BotServ, u, BOT_NOT_ON_CHANNEL, ci->name);
    else if (!check_access(u, ci, CA_SAY))
        notice_lang(s_BotServ, u, ACCESS_DENIED);
    else {
        if (text[0] != '\001') {
            anope_cmd_privmsg(ci->bi->nick, ci->name, "%s", text);
            ci->bi->lastmsg = time(NULL);
            if (logchan && LogBot)
                anope_cmd_privmsg(ci->bi->nick, LogChannel,
                                  ":SAY %s %s %s", u->nick, ci->name,
                                  text);
        } else {
            syntax_error(s_BotServ, u, "SAY", BOT_SAY_SYNTAX);
        }
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_act(User * u)
{
    ChannelInfo *ci;

    char *chan = strtok(NULL, " ");
    char *text = strtok(NULL, "");

    if (!chan || !text)
        syntax_error(s_BotServ, u, "ACT", BOT_ACT_SYNTAX);
    else if (!(ci = cs_findchan(chan)))
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    else if (ci->flags & CI_VERBOTEN)
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    else if (!ci->bi)
        notice_help(s_BotServ, u, BOT_NOT_ASSIGNED);
    else if (!ci->c || ci->c->usercount < BSMinUsers)
        notice_lang(s_BotServ, u, BOT_NOT_ON_CHANNEL, ci->name);
    else if (!check_access(u, ci, CA_SAY))
        notice_lang(s_BotServ, u, ACCESS_DENIED);
    else {
        anope_cmd_privmsg(ci->bi->nick, ci->name, "%cACTION %s%c", 1,
                          text, 1);
        ci->bi->lastmsg = time(NULL);
        if (logchan && LogBot)
            anope_cmd_privmsg(ci->bi->nick, LogChannel, ":ACT %s %s %s",
                              u->nick, ci->name, text);
    }
    return MOD_CONT;
}

/*************************************************************************/
/**
 * Normalize buffer stripping control characters and colors
 * @param A string to be parsed for control and color codes
 * @return A string stripped of control and color codes
 */
char *normalizeBuffer(char *buf)
{
    char *newbuf;
    int i, len, j = 0;

    len = strlen(buf);
    newbuf = (char *) malloc(sizeof(char) * len + 1);

    for (i = 0; i < len; i++) {
        switch (buf[i]) {
            /* Bold ctrl char */
        case 2:
            break;
            /* Color ctrl char */
        case 3:
            /* If the next character is a digit, its also removed */
            if (isdigit(buf[i + 1])) {
                i++;

                /* Check for background color code
                 * and remove it as well
                 */
                if (buf[i + 1] == ',') {
                    i++;

                    if (isdigit(buf[i + 1]))
                        i++;
                }
            }

            break;

            /* Reverse ctrl char */
        case 22:
            break;
            /* Underline ctrl char */
        case 31:
            break;
            /* A valid char gets copied into the new buffer */
        default:
            newbuf[j] = buf[i];
            j++;
        }
    }

    /* Terminate the string */
    newbuf[j] = 0;

    return (newbuf);
}
