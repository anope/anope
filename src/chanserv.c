/* ChanServ functions.
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

#include "services.h"
#include "pseudo.h"

/*************************************************************************/
/* *INDENT-OFF* */

ChannelInfo *chanlists[256];

static int def_levels[][2] = {
    { CA_AUTOOP,                     5 },
    { CA_AUTOVOICE,                  3 },
    { CA_AUTODEOP,                  -1 },
    { CA_NOJOIN,                    -2 },
    { CA_INVITE,                     5 },
    { CA_AKICK,                     10 },
    { CA_SET,     		ACCESS_INVALID },
    { CA_CLEAR,   		ACCESS_INVALID },
    { CA_UNBAN,                      5 },
    { CA_OPDEOP,                     5 },
    { CA_ACCESS_LIST,                1 },
    { CA_ACCESS_CHANGE,             10 },
    { CA_MEMO,                      10 },
    { CA_ASSIGN,  		ACCESS_INVALID },
    { CA_BADWORDS,                  10 },
    { CA_NOKICK,                     1 },
    { CA_FANTASIA,			         3 },
    { CA_SAY,				         5 },
    { CA_GREET,                      5 },
    { CA_VOICEME,			         3 },
    { CA_VOICE,				         5 },
    { CA_GETKEY,                     5 },
    { CA_AUTOHALFOP,                 4 },
    { CA_AUTOPROTECT,               10 },
    { CA_OPDEOPME,                   5 },
    { CA_HALFOPME,                   4 },
    { CA_HALFOP,                     5 },
    { CA_PROTECTME,                 10 },
    { CA_PROTECT,  		ACCESS_INVALID },
    { CA_KICKME,               		 5 },
    { CA_KICK,                       5 },
    { CA_SIGNKICK, 		ACCESS_INVALID },
    { CA_BANME,                      5 },
    { CA_BAN,                        5 },
    { CA_TOPIC,         ACCESS_INVALID },
    { CA_INFO,          ACCESS_INVALID },
    { -1 }
};


LevelInfo levelinfo[] = {
	{ CA_AUTODEOP,      "AUTODEOP",     CHAN_LEVEL_AUTODEOP },
	{ CA_AUTOHALFOP,    "AUTOHALFOP",   CHAN_LEVEL_AUTOHALFOP },
	{ CA_AUTOOP,        "AUTOOP",       CHAN_LEVEL_AUTOOP },
	{ CA_AUTOPROTECT,   "",  CHAN_LEVEL_AUTOPROTECT },
	{ CA_AUTOVOICE,     "AUTOVOICE",    CHAN_LEVEL_AUTOVOICE },
	{ CA_NOJOIN,        "NOJOIN",       CHAN_LEVEL_NOJOIN },
	{ CA_SIGNKICK,      "SIGNKICK",     CHAN_LEVEL_SIGNKICK },
	{ CA_ACCESS_LIST,   "ACC-LIST",     CHAN_LEVEL_ACCESS_LIST },
	{ CA_ACCESS_CHANGE, "ACC-CHANGE",   CHAN_LEVEL_ACCESS_CHANGE },
	{ CA_AKICK,         "AKICK",        CHAN_LEVEL_AKICK },
	{ CA_SET,           "SET",          CHAN_LEVEL_SET },
	{ CA_BAN,           "BAN",          CHAN_LEVEL_BAN },
	{ CA_BANME,         "BANME",        CHAN_LEVEL_BANME },
	{ CA_CLEAR,         "CLEAR",        CHAN_LEVEL_CLEAR },
	{ CA_GETKEY,        "GETKEY",       CHAN_LEVEL_GETKEY },
	{ CA_HALFOP,        "HALFOP",       CHAN_LEVEL_HALFOP },
	{ CA_HALFOPME,      "HALFOPME",     CHAN_LEVEL_HALFOPME },
	{ CA_INFO,          "INFO",         CHAN_LEVEL_INFO },
	{ CA_KICK,          "KICK",         CHAN_LEVEL_KICK },
	{ CA_KICKME,        "KICKME",       CHAN_LEVEL_KICKME },
	{ CA_INVITE,        "INVITE",       CHAN_LEVEL_INVITE },
	{ CA_OPDEOP,        "OPDEOP",       CHAN_LEVEL_OPDEOP },
	{ CA_OPDEOPME,      "OPDEOPME",     CHAN_LEVEL_OPDEOPME },
	{ CA_PROTECT,       "",      CHAN_LEVEL_PROTECT },
	{ CA_PROTECTME,     "",    CHAN_LEVEL_PROTECTME },
	{ CA_TOPIC,         "TOPIC",        CHAN_LEVEL_TOPIC },
	{ CA_UNBAN,         "UNBAN",        CHAN_LEVEL_UNBAN },
	{ CA_VOICE,         "VOICE",        CHAN_LEVEL_VOICE },
	{ CA_VOICEME,       "VOICEME",      CHAN_LEVEL_VOICEME },
	{ CA_MEMO,          "MEMO",         CHAN_LEVEL_MEMO },
	{ CA_ASSIGN,        "ASSIGN",       CHAN_LEVEL_ASSIGN },
	{ CA_BADWORDS,      "BADWORDS",     CHAN_LEVEL_BADWORDS },
	{ CA_FANTASIA,      "FANTASIA",     CHAN_LEVEL_FANTASIA },
	{ CA_GREET,	    "GREET",	    CHAN_LEVEL_GREET },
	{ CA_NOKICK,        "NOKICK",       CHAN_LEVEL_NOKICK },
	{ CA_SAY,	    "SAY",	    CHAN_LEVEL_SAY },
        { -1 }
};
int levelinfo_maxwidth = 0;

CSModeUtil csmodeutils[] = {
    { "DEOP",      "deop",     "-o", CI_OPNOTICE, CA_OPDEOP,  CA_OPDEOPME },
    { "OP",        "op",       "+o", CI_OPNOTICE, CA_OPDEOP,  CA_OPDEOPME },
    { "DEVOICE",   "devoice",  "-v", 0,           CA_VOICE,   CA_VOICEME  },
    { "VOICE",     "voice",    "+v", 0,           CA_VOICE,   CA_VOICEME  },
    { "DEHALFOP",  "dehalfop", "-h", 0,           CA_HALFOP,  CA_HALFOPME },
    { "HALFOP",    "halfop",   "+h", 0,           CA_HALFOP,  CA_HALFOPME },
    { "DEPROTECT", "",         "",   0,           CA_PROTECT, CA_PROTECTME },
    { "PROTECT",   "",         "",   0,           CA_PROTECT, CA_PROTECTME },
    { NULL }
};

/*************************************************************************/

void moduleAddChanServCmds(void) {
    modules_core_init(ChanServCoreNumber, ChanServCoreModules);
}

/* *INDENT-ON* */
/*************************************************************************/
/*************************************************************************/

/* Returns modes for mlock in a nice way. */

char *get_mlock_modes(ChannelInfo * ci, int complete)
{
    static char res[BUFSIZE];

    char *end = res;

    if (ci->mlock_on || ci->mlock_off) {
        int n = 0;
        CBModeInfo *cbmi = cbmodeinfos;

        if (ci->mlock_on) {
            *end++ = '+';
            n++;

            do {
                if (ci->mlock_on & cbmi->flag)
                    *end++ = cbmi->mode;
            } while ((++cbmi)->mode != 0 && ++n < sizeof(res) - 1);

            cbmi = cbmodeinfos;
        }

        if (ci->mlock_off) {
            *end++ = '-';
            n++;

            do {
                if (ci->mlock_off & cbmi->flag)
                    *end++ = cbmi->mode;
            } while ((++cbmi)->mode != 0 && ++n < sizeof(res) - 1);

            cbmi = cbmodeinfos;
        }

        if (ci->mlock_on && complete) {
            do {
                if (cbmi->csgetvalue && (ci->mlock_on & cbmi->flag)) {
                    char *value = cbmi->csgetvalue(ci);

                    if (value) {
                        *end++ = ' ';
                        while (*value)
                            *end++ = *value++;
                    }
                }
            } while ((++cbmi)->mode != 0 && ++n < sizeof(res) - 1);
        }
    }

    *end = 0;

    return res;
}

/* Display total number of registered channels and info about each; or, if
 * a specific channel is given, display information about that channel
 * (like /msg ChanServ INFO <channel>).  If count_only != 0, then only
 * display the number of registered channels (the channel parameter is
 * ignored).
 */

void listchans(int count_only, const char *chan)
{
    int count = 0;
    ChannelInfo *ci;
    int i;

    if (count_only) {

        for (i = 0; i < 256; i++) {
            for (ci = chanlists[i]; ci; ci = ci->next)
                count++;
        }
        printf("%d channels registered.\n", count);

    } else if (chan) {

        struct tm *tm;
        char buf[BUFSIZE];

        if (!(ci = cs_findchan(chan))) {
            printf("Channel %s not registered.\n", chan);
            return;
        }
        if (ci->flags & CI_VERBOTEN) {
            printf("Channel %s is FORBIDden.\n", ci->name);
        } else {
            printf("Information about channel %s:\n", ci->name);
            printf("        Founder: %s\n", ci->founder->display);
            printf("    Description: %s\n", ci->desc);
            tm = localtime(&ci->time_registered);
            strftime(buf, sizeof(buf),
                     getstring(NULL, STRFTIME_DATE_TIME_FORMAT), tm);
            printf("     Registered: %s\n", buf);
            tm = localtime(&ci->last_used);
            strftime(buf, sizeof(buf),
                     getstring(NULL, STRFTIME_DATE_TIME_FORMAT), tm);
            printf("      Last used: %s\n", buf);
            if (ci->last_topic) {
                printf("     Last topic: %s\n", ci->last_topic);
                printf("   Topic set by: %s\n", ci->last_topic_setter);
            }
            if (ci->url)
                printf("            URL: %s\n", ci->url);
            if (ci->email)
                printf(" E-mail address: %s\n", ci->email);
            printf("        Options: ");
            if (!ci->flags) {
                printf("None\n");
            } else {
                int need_comma = 0;
                static const char commastr[] = ", ";
                if (ci->flags & CI_PRIVATE) {
                    printf("Private");
                    need_comma = 1;
                }
                if (ci->flags & CI_KEEPTOPIC) {
                    printf("%sTopic Retention",
                           need_comma ? commastr : "");
                    need_comma = 1;
                }
                if (ci->flags & CI_TOPICLOCK) {
                    printf("%sTopic Lock", need_comma ? commastr : "");
                    need_comma = 1;
                }
                if (ci->flags & CI_SECUREOPS) {
                    printf("%sSecure Ops", need_comma ? commastr : "");
                    need_comma = 1;
                }
                if (ci->flags & CI_RESTRICTED) {
                    printf("%sRestricted Access",
                           need_comma ? commastr : "");
                    need_comma = 1;
                }
                if (ci->flags & CI_SECURE) {
                    printf("%sSecure", need_comma ? commastr : "");
                    need_comma = 1;
                }
                if (ci->flags & CI_NO_EXPIRE) {
                    printf("%sNo Expire", need_comma ? commastr : "");
                    need_comma = 1;
                }
                printf("\n");
            }
            if (ci->mlock_on || ci->mlock_off) {
                printf("      Mode lock: %s\n", get_mlock_modes(ci, 1));
            }
            if (ci->flags & CI_SUSPENDED) {
                printf
                    ("This nickname is currently suspended by %s, reason: %s\n",
                     ci->forbidby,
                     (ci->forbidreason ? ci->forbidreason : "No reason"));
            }
        }

    } else {

        for (i = 0; i < 256; i++) {
            for (ci = chanlists[i]; ci; ci = ci->next) {
                printf("  %s %-20s  %s\n",
                       ci->flags & CI_NO_EXPIRE ? "!" : " ", ci->name,
                       ci->flags & CI_VERBOTEN ?
                       "Disallowed (FORBID)" : (ci->flags & CI_SUSPENDED ?
                                                "Disallowed (SUSPENDED)" :
                                                ci->desc));
                count++;
            }
        }
        printf("%d channels registered.\n", count);

    }
}

/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_chanserv_stats(long *nrec, long *memuse)
{
    long count = 0, mem = 0;
    int i, j;
    ChannelInfo *ci;

    for (i = 0; i < 256; i++) {
        for (ci = chanlists[i]; ci; ci = ci->next) {
            count++;
            mem += sizeof(*ci);
            if (ci->desc)
                mem += strlen(ci->desc) + 1;
            if (ci->url)
                mem += strlen(ci->url) + 1;
            if (ci->email)
                mem += strlen(ci->email) + 1;
            mem += ci->accesscount * sizeof(ChanAccess);
            mem += ci->akickcount * sizeof(AutoKick);
            for (j = 0; j < ci->akickcount; j++) {
                if (!(ci->akick[j].flags & AK_ISNICK)
                    && ci->akick[j].u.mask)
                    mem += strlen(ci->akick[j].u.mask) + 1;
                if (ci->akick[j].reason)
                    mem += strlen(ci->akick[j].reason) + 1;
                if (ci->akick[j].creator)
                    mem += strlen(ci->akick[j].creator) + 1;
            }
            if (ci->mlock_key)
                mem += strlen(ci->mlock_key) + 1;
            if (ircd->fmode) {
                if (ci->mlock_flood)
                    mem += strlen(ci->mlock_flood) + 1;
            }
            if (ircd->Lmode) {
                if (ci->mlock_redirect)
                    mem += strlen(ci->mlock_redirect) + 1;
            }
            if (ircd->jmode) {
                if (ci->mlock_throttle)
                    mem += strlen(ci->mlock_throttle) + 1;
            }
            if (ci->last_topic)
                mem += strlen(ci->last_topic) + 1;
            if (ci->entry_message)
                mem += strlen(ci->entry_message) + 1;
            if (ci->forbidby)
                mem += strlen(ci->forbidby) + 1;
            if (ci->forbidreason)
                mem += strlen(ci->forbidreason) + 1;
            if (ci->levels)
                mem += sizeof(*ci->levels) * CA_SIZE;
            mem += ci->memos.memocount * sizeof(Memo);
            for (j = 0; j < ci->memos.memocount; j++) {
                if (ci->memos.memos[j].text)
                    mem += strlen(ci->memos.memos[j].text) + 1;
            }
            if (ci->ttb)
                mem += sizeof(*ci->ttb) * TTB_SIZE;
            mem += ci->bwcount * sizeof(BadWord);
            for (j = 0; j < ci->bwcount; j++)
                if (ci->badwords[j].word)
                    mem += strlen(ci->badwords[j].word) + 1;
        }
    }
    *nrec = count;
    *memuse = mem;
}

/*************************************************************************/
/*************************************************************************/

/* ChanServ initialization. */

void cs_init(void)
{
    moduleAddChanServCmds();
}

/*************************************************************************/

/* Main ChanServ routine. */

void chanserv(User * u, char *buf)
{
    char *cmd, *s;

    cmd = strtok(buf, " ");

    if (!cmd) {
        return;
    } else if (stricmp(cmd, "\1PING") == 0) {
        if (!(s = strtok(NULL, ""))) {
            s = "";
        }
        anope_cmd_ctcp(s_ChanServ, u->nick, "PING %s", s);
    } else if (skeleton) {
        notice_lang(s_ChanServ, u, SERVICE_OFFLINE, s_ChanServ);
    } else {
        mod_run_cmd(s_ChanServ, u, CHANSERV, cmd);
    }
}

/*************************************************************************/

/* Load/save data files. */


#define SAFE(x) do {					\
    if ((x) < 0) {					\
	if (!forceload)					\
	    fatal("Read error on %s", ChanDBName);	\
	failed = 1;					\
	break;						\
    }							\
} while (0)

void load_cs_dbase(void)
{
    dbFILE *f;
    int ver, i, j, c;
    ChannelInfo *ci, **last, *prev;
    int failed = 0;

    if (!(f = open_db(s_ChanServ, ChanDBName, "r", CHAN_VERSION)))
        return;

    ver = get_file_version(f);

    for (i = 0; i < 256 && !failed; i++) {
        uint16 tmp16;
        uint32 tmp32;
        int n_levels;
        char *s;
        NickAlias *na;

        last = &chanlists[i];
        prev = NULL;
        while ((c = getc_db(f)) != 0) {
            if (c != 1)
                fatal("Invalid format in %s", ChanDBName);
            ci = scalloc(sizeof(ChannelInfo), 1);
            *last = ci;
            last = &ci->next;
            ci->prev = prev;
            prev = ci;
            SAFE(read_buffer(ci->name, f));
            SAFE(read_string(&s, f));
            if (s) {
                if (ver >= 13)
                    ci->founder = findcore(s);
                else {
                    na = findnick(s);
                    if (na)
                        ci->founder = na->nc;
                    else
                        ci->founder = NULL;
                }
                free(s);
            } else
                ci->founder = NULL;
            if (ver >= 7) {
                SAFE(read_string(&s, f));
                if (s) {
                    if (ver >= 13)
                        ci->successor = findcore(s);
                    else {
                        na = findnick(s);
                        if (na)
                            ci->successor = na->nc;
                        else
                            ci->successor = NULL;
                    }
                    free(s);
                } else
                    ci->successor = NULL;
            } else {
                ci->successor = NULL;
            }
            SAFE(read_buffer(ci->founderpass, f));
            SAFE(read_string(&ci->desc, f));
            if (!ci->desc)
                ci->desc = sstrdup("");
            SAFE(read_string(&ci->url, f));
            SAFE(read_string(&ci->email, f));
            SAFE(read_int32(&tmp32, f));
            ci->time_registered = tmp32;
            SAFE(read_int32(&tmp32, f));
            ci->last_used = tmp32;
            SAFE(read_string(&ci->last_topic, f));
            SAFE(read_buffer(ci->last_topic_setter, f));
            SAFE(read_int32(&tmp32, f));
            ci->last_topic_time = tmp32;
            SAFE(read_int32(&ci->flags, f));

            /* Leaveops cleanup */
            if (ver <= 13 && (ci->flags & 0x00000020))
                ci->flags &= ~0x00000020;
            /* Temporary flags cleanup */
            ci->flags &= ~CI_INHABIT;

            if (ver >= 9) {
                SAFE(read_string(&ci->forbidby, f));
                SAFE(read_string(&ci->forbidreason, f));
            } else {
                ci->forbidreason = NULL;
                ci->forbidby = NULL;
            }
            if (ver >= 9)
                SAFE(read_int16(&tmp16, f));
            else
                tmp16 = CSDefBantype;
            ci->bantype = tmp16;
            SAFE(read_int16(&tmp16, f));
            n_levels = tmp16;
            ci->levels = scalloc(2 * CA_SIZE, 1);
            reset_levels(ci);
            for (j = 0; j < n_levels; j++) {
                SAFE(read_int16(&tmp16, f));
                if (j < CA_SIZE)
                    ci->levels[j] = (int16) tmp16;
            }
            /* To avoid levels list silly hacks */
            if (ver < 10)
                ci->levels[CA_OPDEOPME] = ci->levels[CA_OPDEOP];
            if (ver < 11) {
                ci->levels[CA_KICKME] = ci->levels[CA_OPDEOP];
                ci->levels[CA_KICK] = ci->levels[CA_OPDEOP];
            }
            if (ver < 15) {

                /* Old Ultimate levels import */
                /* We now conveniently use PROTECT internals for Ultimate's ADMIN support - ShadowMaster */
                /* Doh, must of course be done before we change the values were trying to import - ShadowMaster */
                ci->levels[CA_AUTOPROTECT] = ci->levels[32];
                ci->levels[CA_PROTECTME] = ci->levels[33];
                ci->levels[CA_PROTECT] = ci->levels[34];

                ci->levels[CA_BANME] = ci->levels[CA_OPDEOP];
                ci->levels[CA_BAN] = ci->levels[CA_OPDEOP];
                ci->levels[CA_TOPIC] = ACCESS_INVALID;


            }

            SAFE(read_int16(&ci->accesscount, f));
            if (ci->accesscount) {
                ci->access = scalloc(ci->accesscount, sizeof(ChanAccess));
                for (j = 0; j < ci->accesscount; j++) {
                    SAFE(read_int16(&ci->access[j].in_use, f));
                    if (ci->access[j].in_use) {
                        SAFE(read_int16(&tmp16, f));
                        ci->access[j].level = (int16) tmp16;
                        SAFE(read_string(&s, f));
                        if (s) {
                            if (ver >= 13)
                                ci->access[j].nc = findcore(s);
                            else {
                                na = findnick(s);
                                if (na)
                                    ci->access[j].nc = na->nc;
                                else
                                    ci->access[j].nc = NULL;
                            }
                            free(s);
                        }
                        if (ci->access[j].nc == NULL)
                            ci->access[j].in_use = 0;
                        if (ver >= 11) {
                            SAFE(read_int32(&tmp32, f));
                            ci->access[j].last_seen = tmp32;
                        } else {
                            ci->access[j].last_seen = 0;        /* Means we have never seen the user */
                        }
                    }
                }
            } else {
                ci->access = NULL;
            }

            SAFE(read_int16(&ci->akickcount, f));
            if (ci->akickcount) {
                ci->akick = scalloc(ci->akickcount, sizeof(AutoKick));
                for (j = 0; j < ci->akickcount; j++) {
                    if (ver >= 15) {
                        SAFE(read_int16(&ci->akick[j].flags, f));
                    } else {
                        SAFE(read_int16(&tmp16, f));
                        if (tmp16)
                            ci->akick[j].flags |= AK_USED;
                    }
                    if (ci->akick[j].flags & AK_USED) {
                        if (ver < 15) {
                            SAFE(read_int16(&tmp16, f));
                            if (tmp16)
                                ci->akick[j].flags |= AK_ISNICK;
                        }
                        SAFE(read_string(&s, f));
                        if (ci->akick[j].flags & AK_ISNICK) {
                            if (ver >= 13) {
                                ci->akick[j].u.nc = findcore(s);
                            } else {
                                na = findnick(s);
                                if (na)
                                    ci->akick[j].u.nc = na->nc;
                                else
                                    ci->akick[j].u.nc = NULL;
                            }
                            if (!ci->akick[j].u.nc)
                                ci->akick[j].flags &= ~AK_USED;
                            free(s);
                        } else {
                            ci->akick[j].u.mask = s;
                        }
                        SAFE(read_string(&s, f));
                        if (ci->akick[j].flags & AK_USED)
                            ci->akick[j].reason = s;
                        else if (s)
                            free(s);
                        if (ver >= 9) {
                            SAFE(read_string(&s, f));
                            if (ci->akick[j].flags & AK_USED) {
                                ci->akick[j].creator = s;
                            } else if (s) {
                                free(s);
                            }
                            SAFE(read_int32(&tmp32, f));
                            if (ci->akick[j].flags & AK_USED)
                                ci->akick[j].addtime = tmp32;
                        } else {
                            ci->akick[j].creator = NULL;
                            ci->akick[j].addtime = 0;
                        }
                    }

                    /* Bugfix */
                    if ((ver == 15) && ci->akick[j].flags > 8) {
                        ci->akick[j].flags = 0;
                        ci->akick[j].u.nc = NULL;
                        ci->akick[j].u.nc = NULL;
                        ci->akick[j].addtime = 0;
                        ci->akick[j].creator = NULL;
                        ci->akick[j].reason = NULL;
                    }
                }
            } else {
                ci->akick = NULL;
            }

            if (ver >= 10) {
                SAFE(read_int32(&ci->mlock_on, f));
                SAFE(read_int32(&ci->mlock_off, f));
            } else {
                SAFE(read_int16(&tmp16, f));
                ci->mlock_on = tmp16;
                SAFE(read_int16(&tmp16, f));
                ci->mlock_off = tmp16;
            }
            SAFE(read_int32(&ci->mlock_limit, f));
            SAFE(read_string(&ci->mlock_key, f));
            if (ver >= 10) {
                SAFE(read_string(&ci->mlock_flood, f));
                SAFE(read_string(&ci->mlock_redirect, f));
                /* We added support for channelmode +j tracking,
                 * however unless for some other reason we need to
                 * change the DB format, it is being saved to DB. ~ Viper
                SAFE(read_string(&ci->mlock_throttle, f));*/
            }

            SAFE(read_int16(&tmp16, f));
            ci->memos.memocount = (int16) tmp16;
            SAFE(read_int16(&tmp16, f));
            ci->memos.memomax = (int16) tmp16;
            if (ci->memos.memocount) {
                Memo *memos;
                memos = scalloc(sizeof(Memo) * ci->memos.memocount, 1);
                ci->memos.memos = memos;
                for (j = 0; j < ci->memos.memocount; j++, memos++) {
                    SAFE(read_int32(&memos->number, f));
                    SAFE(read_int16(&memos->flags, f));
                    SAFE(read_int32(&tmp32, f));
                    memos->time = tmp32;
                    SAFE(read_buffer(memos->sender, f));
                    SAFE(read_string(&memos->text, f));
                    memos->moduleData = NULL;
                }
            }

            SAFE(read_string(&ci->entry_message, f));

            ci->c = NULL;

            /* Some cleanup */
            if (ver <= 11) {
                /* Cleanup: Founder must be != than successor */
                if (!(ci->flags & CI_VERBOTEN)
                    && ci->successor == ci->founder) {
                    alog("Warning: founder and successor of %s are equal. Cleaning up.", ci->name);
                    ci->successor = NULL;
                }
            }

            /* BotServ options */

            if (ver >= 8) {
                int n_ttb;

                SAFE(read_string(&s, f));
                if (s) {
                    ci->bi = findbot(s);
                    free(s);
                } else
                    ci->bi = NULL;

                SAFE(read_int32(&tmp32, f));
                ci->botflags = tmp32;
                SAFE(read_int16(&tmp16, f));
                n_ttb = tmp16;
                ci->ttb = scalloc(2 * TTB_SIZE, 1);
                for (j = 0; j < n_ttb; j++) {
                    SAFE(read_int16(&tmp16, f));
                    if (j < TTB_SIZE)
                        ci->ttb[j] = (int16) tmp16;
                }
                for (j = n_ttb; j < TTB_SIZE; j++)
                    ci->ttb[j] = 0;
                SAFE(read_int16(&tmp16, f));
                ci->capsmin = tmp16;
                SAFE(read_int16(&tmp16, f));
                ci->capspercent = tmp16;
                SAFE(read_int16(&tmp16, f));
                ci->floodlines = tmp16;
                SAFE(read_int16(&tmp16, f));
                ci->floodsecs = tmp16;
                SAFE(read_int16(&tmp16, f));
                ci->repeattimes = tmp16;

                SAFE(read_int16(&ci->bwcount, f));
                if (ci->bwcount) {
                    ci->badwords = scalloc(ci->bwcount, sizeof(BadWord));
                    for (j = 0; j < ci->bwcount; j++) {
                        SAFE(read_int16(&ci->badwords[j].in_use, f));
                        if (ci->badwords[j].in_use) {
                            SAFE(read_string(&ci->badwords[j].word, f));
                            SAFE(read_int16(&ci->badwords[j].type, f));
                        }
                    }
                } else {
                    ci->badwords = NULL;
                }
            } else {
                ci->bi = NULL;
                ci->botflags = 0;
                ci->ttb = scalloc(2 * TTB_SIZE, 1);
                for (j = 0; j < TTB_SIZE; j++)
                    ci->ttb[j] = 0;
                ci->bwcount = 0;
                ci->badwords = NULL;
            }

        }                       /* while (getc_db(f) != 0) */

        *last = NULL;

    }                           /* for (i) */

    close_db(f);

    /* Check for non-forbidden channels with no founder.
       Makes also other essential tasks. */
    for (i = 0; i < 256; i++) {
        ChannelInfo *next;
        for (ci = chanlists[i]; ci; ci = next) {
            next = ci->next;
            if (!(ci->flags & CI_VERBOTEN) && !ci->founder) {
                alog("%s: database load: Deleting founderless channel %s",
                     s_ChanServ, ci->name);
                delchan(ci);
                continue;
            }
            if (ver < 13) {
                ChanAccess *access, *access2;
                AutoKick *akick, *akick2;
                int k;

                if (ci->flags & CI_VERBOTEN)
                    continue;
                /* Need to regenerate the channel count for the founder */
                ci->founder->channelcount++;
                /* Check for eventual double entries in access/akick lists. */
                for (j = 0, access = ci->access; j < ci->accesscount;
                     j++, access++) {
                    if (!access->in_use)
                        continue;
                    for (k = 0, access2 = ci->access; k < j;
                         k++, access2++) {
                        if (access2->in_use && access2->nc == access->nc) {
                            alog("%s: deleting %s channel access entry of %s because it is already in the list (this is OK).", s_ChanServ, access->nc->display, ci->name);
                            memset(access, 0, sizeof(ChanAccess));
                            break;
                        }
                    }
                }
                for (j = 0, akick = ci->akick; j < ci->akickcount;
                     j++, akick++) {
                    if (!(akick->flags & AK_USED)
                        || !(akick->flags & AK_ISNICK))
                        continue;
                    for (k = 0, akick2 = ci->akick; k < j; k++, akick2++) {
                        if ((akick2->flags & AK_USED)
                            && (akick2->flags & AK_ISNICK)
                            && akick2->u.nc == akick->u.nc) {
                            alog("%s: deleting %s channel akick entry of %s because it is already in the list (this is OK).", s_ChanServ, akick->u.nc->display, ci->name);
                            if (akick->reason)
                                free(akick->reason);
                            if (akick->creator)
                                free(akick->creator);
                            memset(akick, 0, sizeof(AutoKick));
                            break;
                        }
                    }
                }
            }
        }
    }
}

#undef SAFE

/*************************************************************************/

#define SAFE(x) do {						\
    if ((x) < 0) {						\
	restore_db(f);						\
	log_perror("Write error on %s", ChanDBName);		\
	if (time(NULL) - lastwarn > WarningTimeout) {		\
	    anope_cmd_global(NULL, "Write error on %s: %s", ChanDBName,	\
			strerror(errno));			\
	    lastwarn = time(NULL);				\
	}							\
	return;							\
    }								\
} while (0)

void save_cs_dbase(void)
{
    dbFILE *f;
    int i, j;
    ChannelInfo *ci;
    Memo *memos;
    static time_t lastwarn = 0;

    if (!(f = open_db(s_ChanServ, ChanDBName, "w", CHAN_VERSION)))
        return;

    for (i = 0; i < 256; i++) {
        int16 tmp16;

        for (ci = chanlists[i]; ci; ci = ci->next) {
            SAFE(write_int8(1, f));
            SAFE(write_buffer(ci->name, f));
            if (ci->founder)
                SAFE(write_string(ci->founder->display, f));
            else
                SAFE(write_string(NULL, f));
            if (ci->successor)
                SAFE(write_string(ci->successor->display, f));
            else
                SAFE(write_string(NULL, f));
            SAFE(write_buffer(ci->founderpass, f));
            SAFE(write_string(ci->desc, f));
            SAFE(write_string(ci->url, f));
            SAFE(write_string(ci->email, f));
            SAFE(write_int32(ci->time_registered, f));
            SAFE(write_int32(ci->last_used, f));
            SAFE(write_string(ci->last_topic, f));
            SAFE(write_buffer(ci->last_topic_setter, f));
            SAFE(write_int32(ci->last_topic_time, f));
            SAFE(write_int32(ci->flags, f));
            SAFE(write_string(ci->forbidby, f));
            SAFE(write_string(ci->forbidreason, f));
            SAFE(write_int16(ci->bantype, f));

            tmp16 = CA_SIZE;
            SAFE(write_int16(tmp16, f));
            for (j = 0; j < CA_SIZE; j++)
                SAFE(write_int16(ci->levels[j], f));

            SAFE(write_int16(ci->accesscount, f));
            for (j = 0; j < ci->accesscount; j++) {
                SAFE(write_int16(ci->access[j].in_use, f));
                if (ci->access[j].in_use) {
                    SAFE(write_int16(ci->access[j].level, f));
                    SAFE(write_string(ci->access[j].nc->display, f));
                    SAFE(write_int32(ci->access[j].last_seen, f));
                }
            }

            SAFE(write_int16(ci->akickcount, f));
            for (j = 0; j < ci->akickcount; j++) {
                SAFE(write_int16(ci->akick[j].flags, f));
                if (ci->akick[j].flags & AK_USED) {
                    if (ci->akick[j].flags & AK_ISNICK)
                        SAFE(write_string(ci->akick[j].u.nc->display, f));
                    else
                        SAFE(write_string(ci->akick[j].u.mask, f));
                    SAFE(write_string(ci->akick[j].reason, f));
                    SAFE(write_string(ci->akick[j].creator, f));
                    SAFE(write_int32(ci->akick[j].addtime, f));
                }
            }

            SAFE(write_int32(ci->mlock_on, f));
            SAFE(write_int32(ci->mlock_off, f));
            SAFE(write_int32(ci->mlock_limit, f));
            SAFE(write_string(ci->mlock_key, f));
            SAFE(write_string(ci->mlock_flood, f));
            SAFE(write_string(ci->mlock_redirect, f));
            /* Current DB format does not hold +j yet..  ~ Viper
            SAFE(write_string(ci->mlock_throttle, f));*/
            SAFE(write_int16(ci->memos.memocount, f));
            SAFE(write_int16(ci->memos.memomax, f));
            memos = ci->memos.memos;
            for (j = 0; j < ci->memos.memocount; j++, memos++) {
                SAFE(write_int32(memos->number, f));
                SAFE(write_int16(memos->flags, f));
                SAFE(write_int32(memos->time, f));
                SAFE(write_buffer(memos->sender, f));
                SAFE(write_string(memos->text, f));
            }

            SAFE(write_string(ci->entry_message, f));

            if (ci->bi)
                SAFE(write_string(ci->bi->nick, f));
            else
                SAFE(write_string(NULL, f));

            SAFE(write_int32(ci->botflags, f));

            tmp16 = TTB_SIZE;
            SAFE(write_int16(tmp16, f));
            for (j = 0; j < TTB_SIZE; j++)
                SAFE(write_int16(ci->ttb[j], f));

            SAFE(write_int16(ci->capsmin, f));
            SAFE(write_int16(ci->capspercent, f));
            SAFE(write_int16(ci->floodlines, f));
            SAFE(write_int16(ci->floodsecs, f));
            SAFE(write_int16(ci->repeattimes, f));

            SAFE(write_int16(ci->bwcount, f));
            for (j = 0; j < ci->bwcount; j++) {
                SAFE(write_int16(ci->badwords[j].in_use, f));
                if (ci->badwords[j].in_use) {
                    SAFE(write_string(ci->badwords[j].word, f));
                    SAFE(write_int16(ci->badwords[j].type, f));
                }
            }
        }                       /* for (chanlists[i]) */

        SAFE(write_int8(0, f));

    }                           /* for (i) */

    close_db(f);

}

#undef SAFE

/*************************************************************************/

void save_cs_rdb_dbase(void)
{
#ifdef USE_RDB
    int i;
    ChannelInfo *ci;

    if (!rdb_open())
        return;

    if (rdb_tag_table("anope_cs_info") == 0) {
        alog("Unable to tag table 'anope_cs_info' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_tag_table("anope_cs_access") == 0) {
        alog("Unable to tag table 'anope_cs_access' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_tag_table("anope_cs_levels") == 0) {
        alog("Unable to tag table 'anope_cs_levels' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_tag_table("anope_cs_akicks") == 0) {
        alog("Unable to tag table 'anope_cs_akicks' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_tag_table("anope_cs_badwords") == 0) {
        alog("Unable to tag table 'anope_cs_badwords' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_tag_table("anope_cs_ttb") == 0) {
        alog("Unable to tag table 'anope_cs_ttb' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_tag_table_where("anope_ms_info", "serv='CHAN'") == 0) {
        alog("Unable to tag table 'anope_ms_info' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }

    for (i = 0; i < 256; i++) {
        for (ci = chanlists[i]; ci; ci = ci->next) {
            if (rdb_save_cs_info(ci) == 0) {
                alog("Unable to save ChanInfo for %s - ChanServ RDB save failed.", ci->name);
                rdb_close();
                return;
            }
        }                       /* for (chanlists[i]) */
    }                           /* for (i) */

    if (rdb_clean_table("anope_cs_info") == 0) {
        alog("Unable to clean table 'anope_cs_info' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_clean_table("anope_cs_access") == 0) {
        alog("Unable to clean table 'anope_cs_access' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_clean_table("anope_cs_levels") == 0) {
        alog("Unable to clean table 'anope_cs_levels' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_clean_table("anope_cs_akicks") == 0) {
        alog("Unable to clean table 'anope_cs_akicks' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_clean_table("anope_cs_badwords") == 0) {
        alog("Unable to clean table 'anope_cs_badwords' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_clean_table("anope_cs_ttb") == 0) {
        alog("Unable to clean table 'anope_cs_ttb' - ChanServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_clean_table_where("anope_ms_info", "serv='CHAN'") == 0)
        alog("Unable to clean table 'anope_ms_info' - ChanServ RDB save failed.");

    rdb_close();
#endif
}

/*************************************************************************/

/* Check the current modes on a channel; if they conflict with a mode lock,
 * fix them.
 *
 * Also check to make sure that every mode set or unset is allowed by the
 * defcon mlock settings. This is more important than any normal channel
 * mlock'd mode. --gdex (21-04-07)
 */

void check_modes(Channel * c)
{
    char modebuf[64], argbuf[BUFSIZE], *end = modebuf, *end2 = argbuf;
    uint32 modes = 0;
    ChannelInfo *ci;
    CBModeInfo *cbmi = NULL;
    CBMode *cbm = NULL;
    boolean DefConOn = DefConLevel != 5;

    if (!c) {
        if (debug) {
            alog("debug: check_modes called with NULL values");
        }
        return;
    }

    if (c->bouncy_modes)
        return;

    /* Check for mode bouncing */
    if (c->server_modecount >= 3 && c->chanserv_modecount >= 3) {
        anope_cmd_global(NULL,
                         "Warning: unable to set modes on channel %s.  "
                         "Are your servers' U:lines configured correctly?",
                         c->name);
        alog("%s: Bouncy modes on channel %s", s_ChanServ, c->name);
        c->bouncy_modes = 1;
        return;
    }

    if (c->chanserv_modetime != time(NULL)) {
        c->chanserv_modecount = 0;
        c->chanserv_modetime = time(NULL);
    }
    c->chanserv_modecount++;

    /* Check if the channel is registered; if not remove mode -r */
    if (!(ci = c->ci)) {
        if (ircd->regmode) {
            if (c->mode & ircd->regmode) {
                c->mode &= ~ircd->regmode;
                anope_cmd_mode(whosends(ci), c->name, "-r");
            }
        }
        /* Channels that are not regged also need the defcon modes.. ~ Viper */
        /* return; */
    }

    /* Initialize the modes-var to set all modes not set yet but which should
     * be set as by mlock and defcon.
     */
    if (ci) 
        modes = ~c->mode & ci->mlock_on;
    if (DefConOn && DefConModesSet)
        modes |= (~c->mode & DefConModesOn);

    /* Initialize the buffers */
    *end++ = '+';
    cbmi = cbmodeinfos;

    do {
        if (modes & cbmi->flag) {
            *end++ = cbmi->mode;
            c->mode |= cbmi->flag;

            /* Add the eventual parameter and modify the Channel structure */
            if (cbmi->getvalue && cbmi->csgetvalue) {
                char *value;
                /* Check if it's a defcon or mlock mode */
                if (DefConOn && DefConModesOn & cbmi->flag)
                    value = cbmi->csgetvalue(&DefConModesCI);
                else if (ci)
                    value = cbmi->csgetvalue(ci);
                else {
                    value = NULL;
                    if (debug) 
                        alog ("Warning: setting modes with unknown origin.");
                }

                cbm = &cbmodes[(int) cbmi->mode];
                cbm->setvalue(c, value);

                if (value) {
                    *end2++ = ' ';
                    while (*value)
                        *end2++ = *value++;
                }
            }
        } else if (cbmi->getvalue && cbmi->csgetvalue
                   && ((ci && (ci->mlock_on & cbmi->flag))
                       || (DefConOn && DefConModesOn & cbmi->flag))
                   && (c->mode & cbmi->flag)) {
            char *value = cbmi->getvalue(c);
            char *csvalue;

            /* Check if it's a defcon or mlock mode */
            if (DefConOn && DefConModesOn & cbmi->flag)
                csvalue = cbmi->csgetvalue(&DefConModesCI);
            else if (ci)
                csvalue = cbmi->csgetvalue(ci);
            else {
                csvalue = NULL;
                if (debug)
                    alog ("Warning: setting modes with unknown origin.");
            }

            /* Lock and actual values don't match, so fix the mode */
            if (value && csvalue && strcmp(value, csvalue)) {
                *end++ = cbmi->mode;

                cbm = &cbmodes[(int) cbmi->mode];
                cbm->setvalue(c, csvalue);

                *end2++ = ' ';
                while (*csvalue)
                    *end2++ = *csvalue++;
            }
        }
    } while ((++cbmi)->mode != 0);

    if (*(end - 1) == '+')
        end--;

    modes = 0;
    if (ci) {
        modes = c->mode & ci->mlock_off;
        /* Make sure we don't remove a mode just set by defcon.. ~ Viper */
        if (DefConOn && DefConModesSet)
            modes &= ~(modes & DefConModesOn);
    }
    if (DefConOn && DefConModesSet)
        modes |= c->mode & DefConModesOff;

    if (modes) {
        *end++ = '-';
        cbmi = cbmodeinfos;

        do {
            if (modes & cbmi->flag) {
                *end++ = cbmi->mode;
                c->mode &= ~cbmi->flag;

                /* Add the eventual parameter and clean up the Channel structure */
                if (cbmi->getvalue) {
                    cbm = &cbmodes[(int) cbmi->mode];

                    if (!(cbm->flags & CBM_MINUS_NO_ARG)) {
                        char *value = cbmi->getvalue(c);

                        if (value) {
                            *end2++ = ' ';
                            while (*value)
                                *end2++ = *value++;
                        }
                    }

                    cbm->setvalue(c, NULL);
                }
            }
        } while ((++cbmi)->mode != 0);
    }

    if (end == modebuf)
        return;

    *end = 0;
    *end2 = 0;

    anope_cmd_mode((ci ? whosends(ci) : s_OperServ), c->name, "%s%s", modebuf,
                   (end2 == argbuf ? "" : argbuf));
}

/*************************************************************************/

int check_valid_admin(User * user, Channel * chan, int servermode)
{
    if (!chan || !chan->ci)
        return 1;

    if (!ircd->admin) {
        return 0;
    }

    /* They will be kicked; no need to deop, no need to update our internal struct too */
    if (chan->ci->flags & CI_VERBOTEN)
        return 0;

    if (servermode && !check_access(user, chan->ci, CA_AUTOPROTECT)) {
        notice_lang(s_ChanServ, user, CHAN_IS_REGISTERED, s_ChanServ);
        anope_cmd_mode(whosends(chan->ci), chan->name, "%s %s",
                       ircd->adminunset, GET_USER(user));
        return 0;
    }

    if (check_access(user, chan->ci, CA_AUTODEOP)) {
        anope_cmd_mode(whosends(chan->ci), chan->name, "%s %s",
                       ircd->adminunset, GET_USER(user));
        return 0;
    }

    return 1;
}

/*************************************************************************/

/* Check whether a user is allowed to be opped on a channel; if they
 * aren't, deop them.  If serverop is 1, the +o was done by a server.
 * Return 1 if the user is allowed to be opped, 0 otherwise. */

int check_valid_op(User * user, Channel * chan, int servermode)
{
    char *tmp;
    if (!chan || !chan->ci)
        return 1;

    /* They will be kicked; no need to deop, no need to update our internal struct too */
    if (chan->ci->flags & CI_VERBOTEN)
        return 0;

    if (servermode && !check_access(user, chan->ci, CA_AUTOOP)) {
        notice_lang(s_ChanServ, user, CHAN_IS_REGISTERED, s_ChanServ);
        if (ircd->halfop) {
            if (ircd->owner && ircd->protect) {
                if (check_access(user, chan->ci, CA_AUTOHALFOP)) {
                    tmp = stripModePrefix(ircd->ownerunset);
                    anope_cmd_mode(whosends(chan->ci), chan->name,
                                   "%so%s %s %s %s", ircd->adminunset,
                                   tmp, GET_USER(user),
                                   GET_USER(user), GET_USER(user));
                    free(tmp);
                } else {
                    tmp = stripModePrefix(ircd->ownerunset);
                    anope_cmd_mode(whosends(chan->ci), chan->name,
                                   "%sho%s %s %s %s %s",
                                   ircd->adminunset, tmp,
                                   GET_USER(user), GET_USER(user), GET_USER(user),
                                   GET_USER(user));
                    free(tmp);
                }
            } else if (!ircd->owner && ircd->protect) {
                if (check_access(user, chan->ci, CA_AUTOHALFOP)) {
                    anope_cmd_mode(whosends(chan->ci), chan->name,
                                   "%so %s %s", ircd->adminunset,
                                   GET_USER(user), GET_USER(user));
                } else {
                    anope_cmd_mode(whosends(chan->ci), chan->name,
                                   "%soh %s %s %s", ircd->adminunset,
                                   GET_USER(user), GET_USER(user), GET_USER(user));
                }
            } else {
                if (check_access(user, chan->ci, CA_AUTOHALFOP)) {
                    anope_cmd_mode(whosends(chan->ci), chan->name, "-o %s",
                                   GET_USER(user));
                } else {
                    anope_cmd_mode(whosends(chan->ci), chan->name,
                                   "-ho %s %s", GET_USER(user), GET_USER(user));
                }
            }
        } else {
            anope_cmd_mode(whosends(chan->ci), chan->name, "-o %s",
                           GET_USER(user));
        }
        return 0;
    }

    if (check_access(user, chan->ci, CA_AUTODEOP)) {
        if (ircd->halfop) {
            if (ircd->owner && ircd->protect) {
                tmp = stripModePrefix(ircd->ownerunset);
                anope_cmd_mode(whosends(chan->ci), chan->name,
                               "%sho%s %s %s %s %s", ircd->adminunset,
                               tmp, GET_USER(user), GET_USER(user),
                               GET_USER(user), GET_USER(user));
                free(tmp);
            } else {
                anope_cmd_mode(whosends(chan->ci), chan->name, "-ho %s %s",
                               GET_USER(user), GET_USER(user));
            }
        } else {
            anope_cmd_mode(whosends(chan->ci), chan->name, "-o %s",
                           GET_USER(user));
        }
        return 0;
    }

    return 1;
}

/*************************************************************************/

/* Check whether a user should be opped on a channel, and if so, do it.
 * Return 1 if the user was opped, 0 otherwise.  (Updates the channel's
 * last used time if the user was opped.) */

int check_should_op(User * user, char *chan)
{
    ChannelInfo *ci = cs_findchan(chan);

    if (!ci || (ci->flags & CI_VERBOTEN) || *chan == '+')
        return 0;

    if ((ci->flags & CI_SECURE) && !nick_identified(user))
        return 0;

    if (check_access(user, ci, CA_AUTOOP)) {
        anope_cmd_mode(whosends(ci), chan, "+o %s", GET_USER(user));
        return 1;
    }

    return 0;
}

/*************************************************************************/

/* Check whether a user should be voiced on a channel, and if so, do it.
 * Return 1 if the user was voiced, 0 otherwise. */

int check_should_voice(User * user, char *chan)
{
    ChannelInfo *ci = cs_findchan(chan);

    if (!ci || (ci->flags & CI_VERBOTEN) || *chan == '+')
        return 0;

    if ((ci->flags & CI_SECURE) && !nick_identified(user))
        return 0;

    if (check_access(user, ci, CA_AUTOVOICE)) {
        anope_cmd_mode(whosends(ci), chan, "+v %s", GET_USER(user));
        return 1;
    }

    return 0;
}

/*************************************************************************/

int check_should_halfop(User * user, char *chan)
{
    ChannelInfo *ci = cs_findchan(chan);

    if (!ci || (ci->flags & CI_VERBOTEN) || *chan == '+')
        return 0;

    if (check_access(user, ci, CA_AUTOHALFOP)) {
        anope_cmd_mode(whosends(ci), chan, "+h %s", GET_USER(user));
        return 1;
    }

    return 0;
}

/*************************************************************************/

int check_should_owner(User * user, char *chan)
{
    char *tmp;
    ChannelInfo *ci = cs_findchan(chan);

    if (!ci || (ci->flags & CI_VERBOTEN) || *chan == '+')
        return 0;

    if (((ci->flags & CI_SECUREFOUNDER) && is_real_founder(user, ci))
        || (!(ci->flags & CI_SECUREFOUNDER) && is_founder(user, ci))) {
        tmp = stripModePrefix(ircd->ownerset);
        anope_cmd_mode(whosends(ci), chan, "+o%s %s %s", tmp, GET_USER(user),
                       GET_USER(user));
        free(tmp);
        return 1;
    }

    return 0;
}

/*************************************************************************/

int check_should_protect(User * user, char *chan)
{
    char *tmp;
    ChannelInfo *ci = cs_findchan(chan);

    if (!ci || (ci->flags & CI_VERBOTEN) || *chan == '+')
        return 0;

    if (check_access(user, ci, CA_AUTOPROTECT)) {
        tmp = stripModePrefix(ircd->adminset);
        anope_cmd_mode(whosends(ci), chan, "+o%s %s %s", tmp, GET_USER(user),
                       GET_USER(user));
        free(tmp);
        return 1;
    }

    return 0;
}

/*************************************************************************/

/* Tiny helper routine to get ChanServ out of a channel after it went in. */

static void timeout_leave(Timeout * to)
{
    char *chan = to->data;
    ChannelInfo *ci = cs_findchan(chan);

    if (ci)                     /* Check cos the channel may be dropped in the meantime */
        ci->flags &= ~CI_INHABIT;

    anope_cmd_part(s_ChanServ, chan, NULL);
    free(to->data);
}


/* Check whether a user is permitted to be on a channel.  If so, return 0;
 * else, kickban the user with an appropriate message (could be either
 * AKICK or restricted access) and return 1.  Note that this is called
 * _before_ the user is added to internal channel lists (so do_kick() is
 * not called). The channel TS must be given for a new channel.
 */

int check_kick(User * user, char *chan, time_t chants)
{
    ChannelInfo *ci = cs_findchan(chan);
    Channel *c;
    AutoKick *akick;
    int i, set_modes = 0;
    NickCore *nc;
    char *av[4];
    int ac;
    char buf[BUFSIZE];
    char mask[BUFSIZE];
    const char *reason;
    Timeout *t;

    if (!ci)
        return 0;

    if (user->isSuperAdmin == 1)
        return 0;

    /* We don't enforce services restrictions on clients on ulined services 
     * as this will likely lead to kick/rejoin floods. ~ Viper */
    if (is_ulined(user->server->name)) {
        return 0;
    }

    if (ci->flags & CI_VERBOTEN) {
        get_idealban(ci, user, mask, sizeof(mask));
        reason =
            ci->forbidreason ? ci->forbidreason : getstring(user->na,
                                                            CHAN_MAY_NOT_BE_USED);
        set_modes = 1;
        goto kick;
    }

    if (ci->flags & CI_SUSPENDED) {
        get_idealban(ci, user, mask, sizeof(mask));
        reason =
            ci->forbidreason ? ci->forbidreason : getstring(user->na,
                                                            CHAN_MAY_NOT_BE_USED);
        set_modes = 1;
        goto kick;
    }

    if (nick_recognized(user))
        nc = user->na->nc;
    else
        nc = NULL;

    /*
     * Before we go through akick lists, see if they're excepted FIRST
     * We cannot kick excempted users that are akicked or not on the channel access list
     * as that will start services <-> server wars which ends up as a DoS against services.
     *
     * UltimateIRCd 3.x at least informs channel staff when a joining user is matching an exempt.
     */
    if (ircd->except && is_excepted(ci, user) == 1) {
        return 0;
    }

    for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
        if (!(akick->flags & AK_USED))
            continue;
        if ((akick->flags & AK_ISNICK && akick->u.nc == nc)
            || (!(akick->flags & AK_ISNICK)
                && match_usermask(akick->u.mask, user))) {
            if (debug >= 2)
                alog("debug: %s matched akick %s", user->nick,
                     (akick->flags & AK_ISNICK) ? akick->u.nc->
                     display : akick->u.mask);
            if (akick->flags & AK_ISNICK)
                get_idealban(ci, user, mask, sizeof(mask));
            else
                strscpy(mask, akick->u.mask, sizeof(mask));
            reason = akick->reason ? akick->reason : CSAutokickReason;
            goto kick;
        }
    }

    if (check_access(user, ci, CA_NOJOIN)) {
        get_idealban(ci, user, mask, sizeof(mask));
        reason = getstring(user->na, CHAN_NOT_ALLOWED_TO_JOIN);
        goto kick;
    }

    return 0;

  kick:
    if (debug)
        alog("debug: channel: AutoKicking %s!%s@%s from %s", user->nick,
             user->username, user->host, chan);

    /* Remember that the user has not been added to our channel user list
     * yet, so we check whether the channel does not exist OR has no user
     * on it (before SJOIN would have created the channel structure, while
     * JOIN would not). */
    /* Don't check for CI_INHABIT before for the Channel record cos else
     * c may be NULL even if it exists */
    if ((!(c = findchan(chan)) || c->usercount == 0)
        && !(ci->flags & CI_INHABIT)) {
        anope_cmd_join(s_ChanServ, chan, (c ? c->creation_time : chants));
        /* Lets hide the channel from /list just incase someone does /list
         * while we are here. - katsklaw
         * We don't want to block users from joining a legit chan though.. - Viper
         */
        if (set_modes)
            anope_cmd_mode(s_ChanServ, chan, "+ntsi");
        t = add_timeout(CSInhabit, timeout_leave, 0);
        t->data = sstrdup(chan);
        ci->flags |= CI_INHABIT;
    }

    if (c) {
        if (ircdcap->tsmode) {
            snprintf(buf, BUFSIZE - 1, "%ld", (long int) time(NULL));
            av[0] = chan;
            av[1] = buf;
            av[2] = sstrdup("+b");
            av[3] = mask;
            ac = 4;
        } else {
            av[0] = chan;
            av[1] = sstrdup("+b");
            av[2] = mask;
            ac = 3;
        }

        do_cmode(whosends(ci), ac, av);

        if (ircdcap->tsmode)
            free(av[2]);
        else
            free(av[1]);
    }

    anope_cmd_mode(whosends(ci), chan, "+b %s", mask);
    anope_cmd_kick(whosends(ci), chan, user->nick, "%s", reason);

    return 1;
}

/*************************************************************************/

/* Record the current channel topic in the ChannelInfo structure. */

void record_topic(const char *chan)
{
    Channel *c;
    ChannelInfo *ci;

    if (readonly)
        return;

    c = findchan(chan);
    if (!c || !(ci = c->ci))
        return;

    if (ci->last_topic)
        free(ci->last_topic);

    if (c->topic)
        ci->last_topic = sstrdup(c->topic);
    else
        ci->last_topic = NULL;

    strscpy(ci->last_topic_setter, c->topic_setter, NICKMAX);
    ci->last_topic_time = c->topic_time;
}

/*************************************************************************/

/* Restore the topic in a channel when it's created, if we should. */

void restore_topic(char *chan)
{
    Channel *c = findchan(chan);
    ChannelInfo *ci;

    if (!c || !(ci = c->ci))
        return;
    /* We can be sure that the topic will be in sync when we return -GD */
    c->topic_sync = 1;
    if (!(ci->flags & CI_KEEPTOPIC)) {
        /* We need to reset the topic here, since it's currently empty and
         * should be updated with a TOPIC from the IRCd soon. -GD
         */
        ci->last_topic = NULL;
        strscpy(ci->last_topic_setter, whosends(ci), NICKMAX);
        ci->last_topic_time = time(NULL);
        return;
    }
    if (c->topic)
        free(c->topic);
    if (ci->last_topic) {
        c->topic = sstrdup(ci->last_topic);
        strscpy(c->topic_setter, ci->last_topic_setter, NICKMAX);
        c->topic_time = ci->last_topic_time;
    } else {
        c->topic = NULL;
        strscpy(c->topic_setter, whosends(ci), NICKMAX);
    }
    if (ircd->join2set) {
        if (whosends(ci) == s_ChanServ) {
            anope_cmd_join(s_ChanServ, chan, c->creation_time);
            anope_cmd_mode(NULL, chan, "+o %s", GET_BOT(s_ChanServ));
        }
    }
    anope_cmd_topic(whosends(ci), c->name, c->topic_setter,
                    c->topic ? c->topic : "", c->topic_time);
    if (ircd->join2set) {
        if (whosends(ci) == s_ChanServ) {
            anope_cmd_part(s_ChanServ, c->name, NULL);
        }
    }
}

/*************************************************************************/

/* See if the topic is locked on the given channel, and return 1 (and fix
 * the topic) if so. */

int check_topiclock(Channel * c, time_t topic_time)
{
    ChannelInfo *ci;

    if (!c) {
        if (debug) {
            alog("debug: check_topiclock called with NULL values");
        }
        return 0;
    }

    if (!(ci = c->ci) || !(ci->flags & CI_TOPICLOCK))
        return 0;

    if (c->topic)
        free(c->topic);
    if (ci->last_topic) {
        c->topic = sstrdup(ci->last_topic);
        strscpy(c->topic_setter, ci->last_topic_setter, NICKMAX);
    } else {
        c->topic = NULL;
        /* Bot assigned & Symbiosis ON?, the bot will set the topic - doc */
        /* Altough whosends() also checks for BSMinUsers -GD */
        strscpy(c->topic_setter, whosends(ci), NICKMAX);
    }

    if (ircd->topictsforward) {
        /* Because older timestamps are rejected */
        /* Some how the topic_time from do_topic is 0 set it to current + 1 */
        if (!topic_time) {
            c->topic_time = time(NULL) + 1;
        } else {
            c->topic_time = topic_time + 1;
        }
    } else {
        /* If no last topic, we can't use last topic time! - doc */
        if (ci->last_topic)
            c->topic_time = ci->last_topic_time;
        else
            c->topic_time = time(NULL) + 1;
    }

    if (ircd->join2set) {
        if (whosends(ci) == s_ChanServ) {
            anope_cmd_join(s_ChanServ, c->name, c->creation_time);
            anope_cmd_mode(NULL, c->name, "+o %s", GET_BOT(s_ChanServ));
        }
    }

    anope_cmd_topic(whosends(ci), c->name, c->topic_setter,
                    c->topic ? c->topic : "", c->topic_time);

    if (ircd->join2set) {
        if (whosends(ci) == s_ChanServ) {
            anope_cmd_part(s_ChanServ, c->ci->name, NULL);
        }
    }
    return 1;
}

/*************************************************************************/

/* Remove all channels which have expired. */

void expire_chans()
{
    ChannelInfo *ci, *next;
    int i;
    time_t now = time(NULL);

    if (!CSExpire)
        return;

    for (i = 0; i < 256; i++) {
        for (ci = chanlists[i]; ci; ci = next) {
            next = ci->next;
            if (!ci->c && now - ci->last_used >= CSExpire
                && !(ci->
                     flags & (CI_VERBOTEN | CI_NO_EXPIRE | CI_SUSPENDED)))
            {
                send_event(EVENT_CHAN_EXPIRE, 1, ci->name);
                alog("Expiring channel %s (founder: %s)", ci->name,
                     (ci->founder ? ci->founder->display : "(none)"));
                delchan(ci);
            }
        }
    }
}

/*************************************************************************/

/* Remove a (deleted or expired) nickname from all channel lists. */

void cs_remove_nick(const NickCore * nc)
{
    int i, j, k;
    ChannelInfo *ci, *next;
    ChanAccess *ca;
    AutoKick *akick;

    for (i = 0; i < 256; i++) {
        for (ci = chanlists[i]; ci; ci = next) {
            next = ci->next;
            if (ci->founder == nc) {
                if (ci->successor) {
                    NickCore *nc2 = ci->successor;
                    if (!nick_is_services_admin(nc2) && nc2->channelmax > 0
                        && nc2->channelcount >= nc2->channelmax) {
                        alog("%s: Successor (%s) of %s owns too many channels, " "deleting channel", s_ChanServ, nc2->display, ci->name);
                        delchan(ci);
                        continue;
                    } else {
                        alog("%s: Transferring foundership of %s from deleted " "nick %s to successor %s", s_ChanServ, ci->name, nc->display, nc2->display);
                        ci->founder = nc2;
                        ci->successor = NULL;
                        nc2->channelcount++;
                    }
                } else {
                    alog("%s: Deleting channel %s owned by deleted nick %s", s_ChanServ, ci->name, nc->display);
                    if (ircd->regmode) {
                        /* Maybe move this to delchan() ? */
                        if ((ci->c) && (ci->c->mode & ircd->regmode)) {
                            ci->c->mode &= ~ircd->regmode;
                            anope_cmd_mode(whosends(ci), ci->name, "-r");
                        }
                    }

                    delchan(ci);
                    continue;
                }
            }

            if (ci->successor == nc)
                ci->successor = NULL;

            for (ca = ci->access, j = ci->accesscount; j > 0; ca++, j--) {
                if (ca->in_use && ca->nc == nc) {
                    ca->in_use = 0;
                    ca->nc = NULL;
                }
            }
            CleanAccess(ci);

            for (akick = ci->akick, j = 0; j < ci->akickcount; akick++, j++) {
                if ((akick->flags & AK_USED) && (akick->flags & AK_ISNICK)
                    && akick->u.nc == nc) {
                    if (akick->creator) {
                        free(akick->creator);
                        akick->creator = NULL;
                    }
                    if (akick->reason) {
                        free(akick->reason);
                        akick->reason = NULL;
                    }
                    akick->flags = 0;
                    akick->addtime = 0;
                    akick->u.nc = NULL;

                    /* Only one occurance can exist in every akick list.. ~ Viper */
                    break;
                }
            }

            /* Are there any akicks behind us? 
             * If so, move all following akicks.. ~ Viper */
            if (j < ci->akickcount - 1) {
                for (k = j + 1; k < ci->akickcount; j++, k++) {
                    if (ci->akick[k].flags & AK_USED) {
                        /* Move the akick one place ahead and clear the original */
                        if (ci->akick[k].flags & AK_ISNICK) {
                            ci->akick[j].u.nc = ci->akick[k].u.nc;
                            ci->akick[k].u.nc = NULL;
                        } else {
                            ci->akick[j].u.mask = sstrdup(ci->akick[k].u.mask);
                            free(ci->akick[k].u.mask);
                            ci->akick[k].u.mask = NULL;
                        }

                        if (ci->akick[k].reason) {
                            ci->akick[j].reason = sstrdup(ci->akick[k].reason);
                            free(ci->akick[k].reason);
                            ci->akick[k].reason = NULL;
                        } else
                            ci->akick[j].reason = NULL;

                        ci->akick[j].creator = sstrdup(ci->akick[k].creator);
                        free(ci->akick[k].creator);
                        ci->akick[k].creator = NULL;

                        ci->akick[j].flags = ci->akick[k].flags;
                        ci->akick[k].flags = 0;

                        ci->akick[j].addtime = ci->akick[k].addtime;
                        ci->akick[k].addtime = 0;
                    }
                }
            }

            /* After moving only the last entry should still be empty.
             * Free the place no longer in use... ~ Viper */
            ci->akickcount = j;
            ci->akick = srealloc(ci->akick,sizeof(AutoKick) * ci->akickcount);
        }
    }
}

/*************************************************************************/

/* Removes any reference to a bot */

void cs_remove_bot(const BotInfo * bi)
{
    int i;
    ChannelInfo *ci;

    for (i = 0; i < 256; i++)
        for (ci = chanlists[i]; ci; ci = ci->next)
            if (ci->bi == bi)
                ci->bi = NULL;
}

/*************************************************************************/

/* Return the ChannelInfo structure for the given channel, or NULL if the
 * channel isn't registered. */

ChannelInfo *cs_findchan(const char *chan)
{
    ChannelInfo *ci;

    if (!chan || !*chan) {
        if (debug) {
            alog("debug: cs_findchan() called with NULL values");
        }
        return NULL;
    }

    for (ci = chanlists[(unsigned char) tolower(chan[1])]; ci;
         ci = ci->next) {
        if (stricmp(ci->name, chan) == 0)
            return ci;
    }
    return NULL;
}

/*************************************************************************/

/* Return 1 if the user's access level on the given channel falls into the
 * given category, 0 otherwise.  Note that this may seem slightly confusing
 * in some cases: for example, check_access(..., CA_NOJOIN) returns true if
 * the user does _not_ have access to the channel (i.e. matches the NOJOIN
 * criterion). */

int check_access(User * user, ChannelInfo * ci, int what)
{
    int level;
    int limit;

    if (!user || !ci) {
        return 0;
    }

    level = get_access(user, ci);
    limit = ci->levels[what];

    /* Resetting the last used time */
    if (level > 0)
        ci->last_used = time(NULL);

    if (level >= ACCESS_FOUNDER)
        return (what == CA_AUTODEOP || what == CA_NOJOIN) ? 0 : 1;
    /* Hacks to make flags work */
    if (what == CA_AUTODEOP && (ci->flags & CI_SECUREOPS) && level == 0)
        return 1;
    if (limit == ACCESS_INVALID)
        return 0;
    if (what == CA_AUTODEOP || what == CA_NOJOIN)
        return level <= ci->levels[what];
    else
        return level >= ci->levels[what];
}

/*************************************************************************/
/*********************** ChanServ private routines ***********************/
/*************************************************************************/

/* Insert a channel alphabetically into the database. */

void alpha_insert_chan(ChannelInfo * ci)
{
    ChannelInfo *ptr, *prev;
    char *chan;

    if (!ci) {
        if (debug) {
            alog("debug: alpha_insert_chan called with NULL values");
        }
        return;
    }

    chan = ci->name;

    for (prev = NULL, ptr = chanlists[(unsigned char) tolower(chan[1])];
         ptr != NULL && stricmp(ptr->name, chan) < 0;
         prev = ptr, ptr = ptr->next);
    ci->prev = prev;
    ci->next = ptr;
    if (!prev)
        chanlists[(unsigned char) tolower(chan[1])] = ci;
    else
        prev->next = ci;
    if (ptr)
        ptr->prev = ci;
}

/*************************************************************************/

/* Add a channel to the database.  Returns a pointer to the new ChannelInfo
 * structure if the channel was successfully registered, NULL otherwise.
 * Assumes channel does not already exist. */

ChannelInfo *makechan(const char *chan)
{
    int i;
    ChannelInfo *ci;

    ci = scalloc(sizeof(ChannelInfo), 1);
    strscpy(ci->name, chan, CHANMAX);
    ci->time_registered = time(NULL);
    reset_levels(ci);
    ci->ttb = scalloc(2 * TTB_SIZE, 1);
    for (i = 0; i < TTB_SIZE; i++)
        ci->ttb[i] = 0;
    alpha_insert_chan(ci);
    return ci;
}

/*************************************************************************/

/* Remove a channel from the ChanServ database.  Return 1 on success, 0
 * otherwise. */

int delchan(ChannelInfo * ci)
{
    int i;
    NickCore *nc;
    User *u;
    struct u_chaninfolist *cilist, *cilist_next;

    if (!ci) {
        if (debug) {
            alog("debug: delchan called with NULL values");
        }
        return 0;
    }

    nc = ci->founder;

    if (debug >= 2) {
        alog("debug: delchan removing %s", ci->name);
    }

    if (ci->bi) {
        ci->bi->chancount--;
    }

    if (debug >= 2) {
        alog("debug: delchan top of removing the bot");
    }
    if (ci->c) {
        if (ci->bi && ci->c->usercount >= BSMinUsers) {
            anope_cmd_part(ci->bi->nick, ci->c->name, NULL);
        }
        ci->c->ci = NULL;
    }
    if (debug >= 2) {
        alog("debug: delchan() Bot has been removed moving on");
    }

    if (debug >= 2) {
        alog("debug: delchan() founder cleanup");
    }
    for (i = 0; i < 1024; i++) {
        for (u = userlist[i]; u; u = u->next) {
            cilist = u->founder_chans;
            while (cilist) {
                cilist_next = cilist->next;
                if (cilist->chan == ci) {
                    if (debug)
                        alog("debug: Dropping founder login of %s for %s",
                             u->nick, ci->name);
                    if (cilist->next)
                        cilist->next->prev = cilist->prev;
                    if (cilist->prev)
                        cilist->prev->next = cilist->next;
                    else
                        u->founder_chans = cilist->next;
                    free(cilist);
                }
                cilist = cilist_next;
            }
        }
    }
    if (debug >= 2) {
        alog("debug: delchan() founder cleanup done");
    }

    if (ci->next)
        ci->next->prev = ci->prev;
    if (ci->prev)
        ci->prev->next = ci->next;
    else
        chanlists[(unsigned char) tolower(ci->name[1])] = ci->next;
    if (ci->desc)
        free(ci->desc);
    if (ci->url)
        free(ci->url);
    if (ci->email)
        free(ci->email);
    if (ci->entry_message)
        free(ci->entry_message);

    if (ci->mlock_key)
        free(ci->mlock_key);
    if (ircd->fmode) {
        if (ci->mlock_flood)
            free(ci->mlock_flood);
    }
    if (ircd->Lmode) {
        if (ci->mlock_redirect)
            free(ci->mlock_redirect);
    }
    if (ircd->jmode) {
        if (ci->mlock_throttle)
            free(ci->mlock_throttle);
    }
    if (ci->last_topic)
        free(ci->last_topic);
    if (ci->forbidby)
        free(ci->forbidby);
    if (ci->forbidreason)
        free(ci->forbidreason);
    if (ci->access)
        free(ci->access);
    if (debug >= 2) {
        alog("debug: delchan() top of the akick list");
    }
    for (i = 0; i < ci->akickcount; i++) {
        if (!(ci->akick[i].flags & AK_ISNICK) && ci->akick[i].u.mask)
            free(ci->akick[i].u.mask);
        if (ci->akick[i].reason)
            free(ci->akick[i].reason);
        if (ci->akick[i].creator)
            free(ci->akick[i].creator);
    }
    if (debug >= 2) {
        alog("debug: delchan() done with the akick list");
    }
    if (ci->akick)
        free(ci->akick);
    if (ci->levels)
        free(ci->levels);
    if (debug >= 2) {
        alog("debug: delchan() top of the memo list");
    }
    if (ci->memos.memos) {
        for (i = 0; i < ci->memos.memocount; i++) {
            if (ci->memos.memos[i].text)
                free(ci->memos.memos[i].text);
            moduleCleanStruct(&ci->memos.memos[i].moduleData);
        }
        free(ci->memos.memos);
    }
    if (debug >= 2) {
        alog("debug: delchan() done with the memo list");
    }
    if (ci->ttb)
        free(ci->ttb);

    if (debug >= 2) {
        alog("debug: delchan() top of the badword list");
    }
    for (i = 0; i < ci->bwcount; i++) {
        if (ci->badwords[i].word)
            free(ci->badwords[i].word);
    }
    if (ci->badwords)
        free(ci->badwords);
    if (debug >= 2) {
        alog("debug: delchan() done with the badword list");
    }


    if (debug >= 2) {
        alog("debug: delchan() calling on moduleCleanStruct()");
    }
    moduleCleanStruct(&ci->moduleData);

    free(ci);
    if (nc)
        nc->channelcount--;

    if (debug >= 2) {
        alog("debug: delchan() all done");
    }
    return 1;
}

/*************************************************************************/

/* Reset channel access level values to their default state. */

void reset_levels(ChannelInfo * ci)
{
    int i;

    if (!ci) {
        if (debug) {
            alog("debug: reset_levels called with NULL values");
        }
        return;
    }

    if (ci->levels)
        free(ci->levels);
    ci->levels = scalloc(CA_SIZE * sizeof(*ci->levels), 1);
    for (i = 0; def_levels[i][0] >= 0; i++)
        ci->levels[def_levels[i][0]] = def_levels[i][1];
}

/*************************************************************************/

/* Does the given user have founder access to the channel? */

int is_founder(User * user, ChannelInfo * ci)
{
    if (!user || !ci) {
        return 0;
    }

    if (user->isSuperAdmin) {
        return 1;
    }

    if (user->na && user->na->nc == ci->founder) {
        if ((nick_identified(user)
             || (nick_recognized(user) && !(ci->flags & CI_SECURE))))
            return 1;
    }
    if (is_identified(user, ci))
        return 1;
    return 0;
}

/*************************************************************************/

int is_real_founder(User * user, ChannelInfo * ci)
{
    if (user->isSuperAdmin) {
        return 1;
    }

    if (user->na && user->na->nc == ci->founder) {
        if ((nick_identified(user)
             || (nick_recognized(user) && !(ci->flags & CI_SECURE))))
            return 1;
    }
    return 0;
}

/*************************************************************************/

/* Has the given user password-identified as founder for the channel? */

int is_identified(User * user, ChannelInfo * ci)
{
    struct u_chaninfolist *c;

    for (c = user->founder_chans; c; c = c->next) {
        if (c->chan == ci)
            return 1;
    }
    return 0;
}

/*************************************************************************/

/* Returns the ChanAccess entry for an user */

ChanAccess *get_access_entry(NickCore * nc, ChannelInfo * ci)
{
    ChanAccess *access;
    int i;

    if (!ci || !nc) {
        return NULL;
    }

    for (access = ci->access, i = 0; i < ci->accesscount; access++, i++)
        if (access->in_use && access->nc == nc)
            return access;

    return NULL;
}

/*************************************************************************/

/* Return the access level the given user has on the channel.  If the
 * channel doesn't exist, the user isn't on the access list, or the channel
 * is CS_SECURE and the user hasn't IDENTIFY'd with NickServ, return 0. */

int get_access(User * user, ChannelInfo * ci)
{
    ChanAccess *access;

    if (!ci || !user)
        return 0;

    /* SuperAdmin always has highest level */
    if (user->isSuperAdmin)
        return ACCESS_SUPERADMIN;

    if (is_founder(user, ci))
        return ACCESS_FOUNDER;

    if (!user->na)
        return 0;

    if (nick_identified(user)
        || (nick_recognized(user) && !(ci->flags & CI_SECURE)))
        if ((access = get_access_entry(user->na->nc, ci)))
            return access->level;

    if (nick_identified(user))
        return 0;

    return 0;
}

/*************************************************************************/

void update_cs_lastseen(User * user, ChannelInfo * ci)
{
    ChanAccess *access;

    if (!ci || !user || !user->na)
        return;

    if (is_founder(user, ci) || nick_identified(user)
        || (nick_recognized(user) && !(ci->flags & CI_SECURE)))
        if ((access = get_access_entry(user->na->nc, ci)))
            access->last_seen = time(NULL);
}

/*************************************************************************/

/* Returns the best ban possible for an user depending of the bantype
   value. */

int get_idealban(ChannelInfo * ci, User * u, char *ret, int retlen)
{
    char *mask;

    if (!ci || !u || !ret || retlen == 0)
        return 0;

    switch (ci->bantype) {
    case 0:
        snprintf(ret, retlen, "*!%s@%s", common_get_vident(u),
                 common_get_vhost(u));
        return 1;
    case 1:
        snprintf(ret, retlen, "*!%s%s@%s",
                 (strlen(common_get_vident(u)) <
                  (*(common_get_vident(u)) ==
                   '~' ? USERMAX + 1 : USERMAX) ? "*" : ""),
                 (*(common_get_vident(u)) ==
                  '~' ? common_get_vident(u) + 1 : common_get_vident(u)),
                 common_get_vhost(u));
        return 1;
    case 2:
        snprintf(ret, retlen, "*!*@%s", common_get_vhost(u));
        return 1;
    case 3:
        mask = create_mask(u);
        snprintf(ret, retlen, "*!%s", mask);
        free(mask);
        return 1;

    default:
        return 0;
    }
}

/*************************************************************************/

char *cs_get_flood(ChannelInfo * ci)
{
    if (!ci) {
        return NULL;
    } else {
        if (ircd->fmode) {
            return ci->mlock_flood;
        } else {
            return NULL;
        }
    }
}

/*************************************************************************/

char *cs_get_throttle(ChannelInfo * ci)
{
    if (!ci) {
        return NULL;
    } else {
        if (ircd->jmode) {
            return ci->mlock_throttle;
        } else {
            return NULL;
        }
    }
}

/*************************************************************************/

char *cs_get_key(ChannelInfo * ci)
{
    if (!ci) {
        return NULL;
    } else {
        return ci->mlock_key;
    }
}

/*************************************************************************/

char *cs_get_limit(ChannelInfo * ci)
{
    static char limit[16];

    if (!ci) {
        return NULL;
    }

    if (ci->mlock_limit == 0)
        return NULL;

    snprintf(limit, sizeof(limit), "%lu",
             (unsigned long int) ci->mlock_limit);
    return limit;
}

/*************************************************************************/

char *cs_get_redirect(ChannelInfo * ci)
{
    if (!ci) {
        return NULL;
    } else {
        if (ircd->Lmode) {
            return ci->mlock_redirect;
        } else {
            return NULL;
        }
    }
}

/*************************************************************************/

/* This is a dummy function part of making anope accept modes
 * it does actively parse.. ~ Viper */
char *cs_get_unkwn(ChannelInfo * ci)
{
    return NULL;
}

/*************************************************************************/

void cs_set_flood(ChannelInfo * ci, char *value)
{
    if (!ci) {
        return;
    }

    if (ci->mlock_flood)
        free(ci->mlock_flood);

    /* This looks ugly, but it works ;) */
    if (anope_flood_mode_check(value)) {
        ci->mlock_flood = sstrdup(value);
    } else {
        ci->mlock_on &= ~ircd->chan_fmode;
        ci->mlock_flood = NULL;
    }
}

/*************************************************************************/

void cs_set_throttle(ChannelInfo * ci, char *value)
{
    if (!ci)
        return;

    if (ci->mlock_throttle)
        free(ci->mlock_throttle);

    if (anope_jointhrottle_mode_check(value)) {
        ci->mlock_throttle = sstrdup(value);
    } else {
        ci->mlock_on &= ~ircd->chan_jmode;
        ci->mlock_throttle = NULL;
    }
}

/*************************************************************************/

void cs_set_key(ChannelInfo * ci, char *value)
{
    if (!ci) {
        return;
    }

    if (ci->mlock_key)
        free(ci->mlock_key);

    /* Don't allow keys with a coma */
    if (value && *value != ':' && !strchr(value, ',')) {
        ci->mlock_key = sstrdup(value);
    } else {
        ci->mlock_on &= ~anope_get_key_mode();
        ci->mlock_key = NULL;
    }
}

/*************************************************************************/

void cs_set_limit(ChannelInfo * ci, char *value)
{
    if (!ci) {
        return;
    }

    ci->mlock_limit = value ? strtoul(value, NULL, 10) : 0;

    if (ci->mlock_limit <= 0)
        ci->mlock_on &= ~anope_get_limit_mode();
}

/*************************************************************************/

void cs_set_redirect(ChannelInfo * ci, char *value)
{
    if (!ci) {
        return;
    }

    if (ci->mlock_redirect)
        free(ci->mlock_redirect);

    /* Don't allow keys with a coma */
    if (value && *value == '#') {
        ci->mlock_redirect = sstrdup(value);
    } else {
        ci->mlock_on &= ~ircd->chan_lmode;
        ci->mlock_redirect = NULL;
    }
}

/*************************************************************************/

/* This is a dummy function to make anope parse a param for a mode,
 * yet we don't use that param internally.. ~ Viper */
void cs_set_unkwn(ChannelInfo * ci, char *value)
{
    /* Do nothing.. */
}

/*************************************************************************/

int get_access_level(ChannelInfo * ci, NickAlias * na)
{
    ChanAccess *access;
    int num;

    if (!ci || !na) {
        return 0;
    }

    if (na->nc == ci->founder) {
        return ACCESS_FOUNDER;
    }

    for (num = 0; num < ci->accesscount; num++) {

        access = &ci->access[num];

        if (access->nc && access->nc == na->nc && access->in_use) {
            return access->level;
        }

    }

    return 0;

}

const char *get_xop_level(int level)
{
    if (level < ACCESS_VOP) {
        return "Err";
    } else if (ircd->halfop && level < ACCESS_HOP) {
        return "VOP";
    } else if (!ircd->halfop && level < ACCESS_AOP) {
        return "VOP";
    } else if (ircd->halfop && level < ACCESS_AOP) {
        return "HOP";
    } else if (level < ACCESS_SOP) {
        return "AOP";
    } else if (level < ACCESS_FOUNDER) {
        return "SOP";
    } else {
        return "Founder";
    }

}

/*************************************************************************/
/*********************** ChanServ command routines ***********************/
/*************************************************************************/

/*************************************************************************/


/*************************************************************************/

/* `last' is set to the last index this routine was called with
 * `perm' is incremented whenever a permission-denied error occurs
 */


/*************************************************************************/

/* Is the mask stuck? */

AutoKick *is_stuck(ChannelInfo * ci, char *mask)
{
    int i;
    AutoKick *akick;

    if (!ci) {
        return NULL;
    }

    for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
        if (!(akick->flags & AK_USED) || (akick->flags & AK_ISNICK)
            || !(akick->flags & AK_STUCK))
            continue;
        /* Example: mask = *!*@*.org and akick->u.mask = *!*@*.anope.org */
        if (match_wild_nocase(mask, akick->u.mask))
            return akick;
        if (ircd->reversekickcheck) {
            /* Example: mask = *!*@irc.anope.org and akick->u.mask = *!*@*.anope.org */
            if (match_wild_nocase(akick->u.mask, mask))
                return akick;
        }
    }

    return NULL;
}

/* Ban the stuck mask in a safe manner. */

void stick_mask(ChannelInfo * ci, AutoKick * akick)
{
    char *av[2];
    Entry *ban;

    if (!ci) {
        return;
    }

    if (ci->c->bans && ci->c->bans->entries != 0) {
        for (ban = ci->c->bans->entries; ban; ban = ban->next) {
            /* If akick is already covered by a wider ban.
               Example: c->bans[i] = *!*@*.org and akick->u.mask = *!*@*.epona.org */
            if (entry_match_mask(ban, sstrdup(akick->u.mask), 0))
                return;

            if (ircd->reversekickcheck) {
                /* If akick is wider than a ban already in place.
                   Example: c->bans[i] = *!*@irc.epona.org and akick->u.mask = *!*@*.epona.org */
                if (match_wild_nocase(akick->u.mask, ban->mask))
                    return;
            }
        }
    }

    /* Falling there means set the ban */
    av[0] = sstrdup("+b");
    av[1] = akick->u.mask;
    anope_cmd_mode(whosends(ci), ci->c->name, "+b %s", akick->u.mask);
    chan_set_modes(s_ChanServ, ci->c, 2, av, 1);
    free(av[0]);
}

/* Ban the stuck mask in a safe manner. */

void stick_all(ChannelInfo * ci)
{
    int i;
    char *av[2];
    AutoKick *akick;

    if (!ci) {
        return;
    }

    for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
        if (!(akick->flags & AK_USED) || (akick->flags & AK_ISNICK)
            || !(akick->flags & AK_STUCK))
            continue;

        av[0] = sstrdup("+b");
        av[1] = akick->u.mask;
        anope_cmd_mode(whosends(ci), ci->c->name, "+b %s", akick->u.mask);
        chan_set_modes(s_ChanServ, ci->c, 2, av, 1);
        free(av[0]);
    }
}

/** Reorder the access list to get rid of unused entries
 * @param ci The channel to reorder the access of
 */
void CleanAccess(ChannelInfo *ci)
{
	int a, b;

	if (!ci)
		return;

	for (b = 0; b < ci->accesscount; b++)
	{
		if (ci->access[b].in_use)
		{
			for (a = 0; a < ci->accesscount; a++)
			{
				if (a > b)
					break;
				if (!ci->access[a].in_use)
				{
					ci->access[a].in_use = 1;
					ci->access[a].level = ci->access[b].level;
					ci->access[a].nc = ci->access[b].nc;
					ci->access[a].last_seen = ci->access[b].last_seen;
					ci->access[b].nc = NULL;
					ci->access[b].in_use = 0;
					break;
				}
			}
		}
	}

	/* After reordering, entries on the end of the list may be empty, remove them */
	for (b = ci->accesscount - 1; b >= 0; --b)
	{
		if (ci->access[b].in_use)
			break;
		ci->accesscount--;
	}

	/* Reallocate the access list to only use the memory we need */
	ci->access = srealloc(ci->access, sizeof(ChanAccess) * ci->accesscount);
}

