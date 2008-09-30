/* Charybdis IRCD functions
 *
 * (C) 2006 William Pitcock <nenolod -at- charybdis.be>
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "pseudo.h"
#include "charybdis.h"

IRCDVar myIrcd[] = {
    {"Charybdis 1.0+",           /* ircd name */
     "+oiS",                     /* nickserv mode */
     "+oiS",                     /* chanserv mode */
     "+oiS",                     /* memoserv mode */
     "+oiS",                     /* hostserv mode */
     "+oaiS",                    /* operserv mode */
     "+oiS",                     /* botserv mode  */
     "+oiS",                     /* helpserv mode */
     "+oiS",                     /* Dev/Null mode */
     "+oiS",                     /* Global mode   */
     "+oiS",                     /* nickserv alias mode */
     "+oiS",                     /* chanserv alias mode */
     "+oiS",                     /* memoserv alias mode */
     "+oiS",                     /* hostserv alias mode */
     "+oaiS",                    /* operserv alias mode */
     "+oiS",                     /* botserv alias mode  */
     "+oiS",                     /* helpserv alias mode */
     "+oiS",                     /* Dev/Null alias mode */
     "+oiS",                     /* Global alias mode   */
     "+oiS",                     /* Used by BotServ Bots */
     2,                         /* Chan Max Symbols     */
     "-cilmnpstrgzQF",          /* Modes to Remove */
     "+o",                      /* Channel Umode used by Botserv bots */
     1,                         /* SVSNICK */
     1,                         /* Vhost  */
     0,                         /* Has Owner */
     NULL,                      /* Mode to set for an owner */
     NULL,                      /* Mode to unset for an owner */
     NULL,                      /* Mode to set for chan admin */
     NULL,                      /* Mode to unset for chan admin */
     NULL,                      /* Mode On Reg          */
     NULL,                      /* Mode on ID for Roots */
     NULL,                      /* Mode on ID for Admins */
     NULL,                      /* Mode on ID for Opers */
     NULL,                      /* Mode on UnReg        */
     NULL,                      /* Mode on Nick Change  */
     1,                         /* Supports SGlines     */
     1,                         /* Supports SQlines     */
     1,                         /* Supports SZlines     */
     0,                         /* Supports Halfop +h   */
     3,                         /* Number of server args */
     1,                         /* Join 2 Set           */
     1,                         /* Join 2 Message       */
     1,                         /* Has exceptions +e    */
     0,                         /* TS Topic Forward     */
     0,                         /* TS Topci Backward    */
     0,                         /* Protected Umode      */
     0,                         /* Has Admin            */
     1,                         /* Chan SQlines         */
     0,                         /* Quit on Kill         */
     0,                         /* SVSMODE unban        */
     0,                         /* Has Protect          */
     0,                         /* Reverse              */
     0,                         /* Chan Reg             */
     0,                         /* Channel Mode         */
     0,                         /* vidents              */
     1,                         /* svshold              */
     0,                         /* time stamp on mode   */
     0,                         /* NICKIP               */
     0,                         /* UMODE                */
     0,                         /* O:LINE               */
     1,                         /* VHOST ON NICK        */
     0,                         /* Change RealName      */
     CMODE_p,                   /* No Knock             */
     0,                         /* Admin Only           */
     DEFAULT_MLOCK,             /* Default MLOCK        */
     0,                         /* Vhost Mode           */
     1,                         /* +f                   */
     1,                         /* +L                   */
     CMODE_j,                   /* +f Mode                          */
     CMODE_f,                   /* +L Mode (+f <target>)            */
     1,                         /* On nick change check if they could be identified */
     0,                         /* No Knock requires +i */
     NULL,                      /* CAPAB Chan Modes             */
     0,                         /* We support TOKENS */
     1,                         /* TOKENS are CASE inSensitive */
     0,                         /* TIME STAMPS are BASE64 */
     1,                         /* +I support */
     0,                         /* SJOIN ban char */
     0,                         /* SJOIN except char */
     0,                         /* SJOIN invite char */
     0,                         /* Can remove User Channel Modes with SVSMODE */
     0,                         /* Sglines are not enforced until user reconnects */
     NULL,                      /* vhost char */
     1,                         /* ts6 */
     0,                         /* support helper umode */
     0,                         /* p10 */
     NULL,                      /* character set */
     0,                         /* reports sync state */
     1,                         /* CIDR channelbans */
     "$$",                      /* TLD Prefix for Global */
     }
    ,
    {NULL}
};

IRCDCAPAB myIrcdcap[] = {
    {
     0,                         /* NOQUIT       */
     0,                         /* TSMODE       */
     0,                         /* UNCONNECT    */
     0,                         /* NICKIP       */
     0,                         /* SJOIN        */
     CAPAB_ZIP,                 /* ZIP          */
     0,                         /* BURST        */
     CAPAB_TS5,                 /* TS5          */
     0,                         /* TS3          */
     0,                         /* DKEY         */
     0,                         /* PT4          */
     0,                         /* SCS          */
     CAPAB_QS,                  /* QS           */
     CAPAB_UID,                 /* UID          */
     CAPAB_KNOCK,               /* KNOCK        */
     0,                         /* CLIENT       */
     0,                         /* IPV6         */
     0,                         /* SSJ5         */
     0,                         /* SN2          */
     0,                         /* TOKEN        */
     0,                         /* VHOST        */
     0,                         /* SSJ3         */
     0,                         /* NICK2        */
     0,                         /* UMODE2       */
     0,                         /* VL           */
     0,                         /* TLKEXT       */
     0,                         /* DODKEY       */
     0,                         /* DOZIP        */
     0, 0, 0}
};

/*******************************************************************/

void charybdis_set_umode(User * user, int ac, const char **av)
{
    int add = 1;                /* 1 if adding modes, 0 if deleting */
    const char *modes = av[0];

    ac--;

    if (debug)
        alog("debug: Changing mode for %s to %s", user->nick, modes);

    while (*modes) {

        /* This looks better, much better than "add ? (do_add) : (do_remove)".
         * At least this is readable without paying much attention :) -GD
         */
        if (add)
            user->mode |= umodes[(int) *modes];
        else
            user->mode &= ~umodes[(int) *modes];

        switch (*modes++) {
        case '+':
            add = 1;
            break;
        case '-':
            add = 0;
            break;
        case 'o':
            if (add) {
                opcnt++;

                if (WallOper)
                    anope_cmd_global(s_OperServ,
                                     "\2%s\2 is now an IRC operator.",
                                     user->nick);
                display_news(user, NEWS_OPER);

            } else {
                opcnt--;
            }
            break;
        }
    }
}

unsigned long umodes[128] = {
    0, 0, 0,                    /* Unused */
    0, 0, 0,                    /* Unused */
    0, 0, 0,                    /* Unused, Unused, Horzontal Tab */
    0, 0, 0,                    /* Line Feed, Unused, Unused */
    0, 0, 0,                    /* Carriage Return, Unused, Unused */
    0, 0, 0,                    /* Unused */
    0, 0, 0,                    /* Unused */
    0, 0, 0,                    /* Unused */
    0, 0, 0,                    /* Unused */
    0, 0, 0,                    /* Unused */
    0, 0, 0,                    /* Unused, Unused, Space */
    0, 0, 0,                    /* ! " #  */
    0, 0, 0,                    /* $ % &  */
    0, 0, 0,                    /* ! ( )  */
    0, 0, 0,                    /* * + ,  */
    0, 0, 0,                    /* - . /  */
    0, 0,                       /* 0 1 */
    0, 0,                       /* 2 3 */
    0, 0,                       /* 4 5 */
    0, 0,                       /* 6 7 */
    0, 0,                       /* 8 9 */
    0, 0,                       /* : ; */
    0, 0, 0,                    /* < = > */
    0, 0,                       /* ? @ */
    0, 0, 0,                    /* A B C */
    0, 0, 0,                    /* D E F */
    0, 0, 0,                    /* G H I */
    0, 0, 0,                    /* J K L */
    0, 0, 0,                    /* M N O */
    0, UMODE_Q, UMODE_R,        /* P Q R */
    UMODE_S, 0, 0,              /* S T U */
    0, 0, 0,                    /* V W X */
    0,                          /* Y */
    0,                          /* Z */
    0, 0, 0,                    /* [ \ ] */
    0, 0, 0,                    /* ^ _ ` */
    UMODE_a, 0, 0,              /* a b c */
    0, 0, 0,                    /* d e f */
    UMODE_g, 0, UMODE_i,        /* g h i */
    0, 0, UMODE_l,              /* j k l */
    0, 0, UMODE_o,              /* m n o */
    0, 0, 0,                    /* p q r */
    UMODE_s, 0, 0,              /* s t u */
    0, UMODE_w, 0,              /* v w x */
    0,                          /* y */
    UMODE_z,                    /* z */
    0, 0, 0,                    /* { | } */
    0, 0                        /* ~ � */
};


char myCsmodes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0,
    0,
    0, 0, 0,
    0,
    0, 0, 0, 0,
    0,

    'v', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    'o', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

CMMode myCmmodes[128] = {
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL},     /* BCD */
    {NULL}, {NULL}, {NULL},     /* EFG */
    {NULL},                     /* H */
    {add_invite, del_invite},
    {NULL},                     /* J */
    {NULL}, {NULL}, {NULL},     /* KLM */
    {NULL}, {NULL}, {NULL},     /* NOP */
    {NULL}, {NULL}, {NULL},     /* QRS */
    {NULL}, {NULL}, {NULL},     /* TUV */
    {NULL}, {NULL}, {NULL},     /* WXY */
    {NULL},                     /* Z */
    {NULL}, {NULL},             /* (char 91 - 92) */
    {NULL}, {NULL}, {NULL},     /* (char 93 - 95) */
    {NULL},                     /* ` (char 96) */
    {NULL},                     /* a (char 97) */
    {add_ban, del_ban},
    {NULL},
    {NULL},
    {add_exception, del_exception},
    {NULL},
    {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}
};


CBMode myCbmodes[128] = {
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0},
    {0},                        /* A */
    {0},                        /* B */
    {0},                        /* C */
    {0},                        /* D */
    {0},                        /* E */
    {CMODE_F, 0, NULL, NULL},   /* F */
    {0},                        /* G */
    {0},                        /* H */
    {0},                        /* I */
    {0},                        /* J */
    {0},                        /* K */
    {CMODE_L, CBM_NO_USER_MLOCK, NULL, NULL},   /* L */
    {0},                        /* M */
    {0},                        /* N */
    {0},                        /* O */
    {CMODE_P, CBM_NO_USER_MLOCK, NULL, NULL},   /* P */
    {CMODE_Q, 0, NULL, NULL},   /* Q */
    {0},                        /* R */
    {0},                        /* S */
    {0},                        /* T */
    {0},                        /* U */
    {0},                        /* V */
    {0},                        /* W */
    {0},                        /* X */
    {0},                        /* Y */
    {0},                        /* Z */
    {0}, {0}, {0}, {0}, {0}, {0},
    {0},
    {0},                        /* b */
    {CMODE_c, 0, NULL, NULL},   /* c */
    {0},                        /* d */
    {0},                        /* e */
    {CMODE_f, 0, set_redirect, cs_set_redirect},  /* f */
    {CMODE_g, 0, NULL, NULL},   /* g */
    {0},                        /* h */
    {CMODE_i, 0, NULL, NULL},
    {CMODE_j, 0, set_flood, cs_set_flood},       /* j */
    {CMODE_k, 0, chan_set_key, cs_set_key},
    {CMODE_l, CBM_MINUS_NO_ARG, set_limit, cs_set_limit},
    {CMODE_m, 0, NULL, NULL},
    {CMODE_n, 0, NULL, NULL},
    {0},                        /* o */
    {CMODE_p, 0, NULL, NULL},
    {0},                        /* q */
    {CMODE_r, 0, NULL, NULL},
    {CMODE_s, 0, NULL, NULL},
    {CMODE_t, 0, NULL, NULL},
    {0},
    {0},                        /* v */
    {0},                        /* w */
    {0},                        /* x */
    {0},                        /* y */
    {CMODE_z, 0, NULL, NULL},   /* z */
    {0}, {0}, {0}, {0}
};

CBModeInfo myCbmodeinfos[] = {
    {'c', CMODE_c, 0, NULL, NULL},
    {'f', CMODE_f, CBM_MINUS_NO_ARG, get_redirect, cs_get_redirect},
    {'g', CMODE_g, 0, NULL, NULL},
    {'i', CMODE_i, 0, NULL, NULL},
    {'j', CMODE_j, CBM_MINUS_NO_ARG, get_flood, cs_get_flood},
    {'k', CMODE_k, 0, get_key, cs_get_key},
    {'l', CMODE_l, CBM_MINUS_NO_ARG, get_limit, cs_get_limit},
    {'m', CMODE_m, 0, NULL, NULL},
    {'n', CMODE_n, 0, NULL, NULL},
    {'p', CMODE_p, 0, NULL, NULL},
    {'r', CMODE_r, 0, NULL, NULL},
    {'s', CMODE_s, 0, NULL, NULL},
    {'t', CMODE_t, 0, NULL, NULL},
    {'z', CMODE_z, 0, NULL, NULL},
    {'F', CMODE_F, 0, NULL, NULL},
    {'L', CMODE_L, 0, NULL, NULL},
    {'P', CMODE_P, 0, NULL, NULL},
    {'Q', CMODE_Q, 0, NULL, NULL},
    {0}
};


CUMode myCumodes[128] = {
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},

    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},

    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},

    {0},

    {0},                        /* a */
    {0},                        /* b */
    {0},                        /* c */
    {0},                        /* d */
    {0},                        /* e */
    {0},                        /* f */
    {0},                        /* g */
    {0},
    {0},                        /* i */
    {0},                        /* j */
    {0},                        /* k */
    {0},                        /* l */
    {0},                        /* m */
    {0},                        /* n */
    {CUS_OP, CUF_PROTECT_BOTSERV, check_valid_op},
    {0},                        /* p */
    {0},                        /* q */
    {0},                        /* r */
    {0},                        /* s */
    {0},                        /* t */
    {0},                        /* u */
    {CUS_VOICE, 0, NULL},
    {0},                        /* w */
    {0},                        /* x */
    {0},                        /* y */
    {0},                        /* z */
    {0}, {0}, {0}, {0}, {0}
};



void CharybdisProto::cmd_message(const char *source, const char *dest, const char *buf)
{
	if (!buf) return;
	if (NSDefFlags & NI_MSG) cmd_privmsg(source, dest, buf);
	else cmd_notice(source, dest, buf);
}

void charybdis_cmd_notice2(const char *source, const char *dest, const char *msg)
{
	Uid *ud = find_uid(source);
	User *u = finduser(dest);
	send_cmd(UseTS6 ? (ud ? ud->uid : source) : source, "NOTICE %s :%s", UseTS6 ? (u ? u->uid : dest) : dest, msg);
}

void CharybdisProto::cmd_privmsg(const char *source, const char *dest, const char *buf)
{
	if (!buf) return;
	Uid *ud = find_uid(source), *ud2 = find_uid(dest);
	send_cmd(UseTS6 ? (ud ? ud->uid : source) : source, "PRIVMSG %s :%s", UseTS6 ? (ud2 ? ud2->uid : dest) : dest, buf);
}

void CharybdisProto::cmd_global(const char *source, const char *buf)
{
	if (!buf) return;
	if (source) {
		Uid *u = find_uid(source);
		if (u) {
			send_cmd(UseTS6 ? u->uid : source, "OPERWALL :%s", buf);
			return;
		}
	}
	// The original code uses WALLOPS here, but above is OPERWALL, Ratbox uses OPERWALL as well, so I changed this one as well -- CyberBotX
	send_cmd(UseTS6 ? TS6SID : ServerName, "OPERWALL :%s", buf);
}

int anope_event_sjoin(const char *source, int ac, const char **av)
{
    do_sjoin(source, ac, av);
    return MOD_CONT;
}

/*
   Non TS6

   av[0] = nick
   av[1] = hop
   av[2] = ts
   av[3] = modes
   av[4] = user
   av[5] = host
   av[6] = server
   av[7] = info

   TS6
   av[0] = nick
   av[1] = hop
   av[2] = ts
   av[3] = modes
   av[4] = user
   av[5] = host
   av[6] = IP
   av[7] = UID
   av[8] = info

*/
int anope_event_nick(const char *source, int ac, const char **av)
{
    Server *s;
    User *user;

    if (UseTS6 && ac == 9) {
        s = findserver_uid(servlist, source);
        /* Source is always the server */
        user = do_nick("", av[0], av[4], av[5], s->name, av[8],
                       strtoul(av[2], NULL, 10), 0, 0, NULL, av[7]);
        if (user) {
            anope_set_umode(user, 1, &av[3]);
        }
    } else {
        if (ac != 2) {
            user = do_nick(source, av[0], av[4], av[5], av[6], av[7],
                           strtoul(av[2], NULL, 10), 0, 0, NULL, NULL);
            if (user)
                anope_set_umode(user, 1, &av[3]);
        } else {
            do_nick(source, av[0], NULL, NULL, NULL, NULL,
                    strtoul(av[1], NULL, 10), 0, 0, NULL, NULL);
        }
    }
    return MOD_CONT;
}

/*
   TS6
   av[0] = nick
   av[1] = hop
   av[2] = ts
   av[3] = modes
   av[4] = user
   av[5] = vhost
   av[6] = IP
   av[7] = UID
   ac[8] = host or *
   ac[9] = services login
   av[10] = info

*/
int anope_event_euid(const char *source, int ac, const char **av)
{
    Server *s;
    User *user;
    time_t ts;

    if (UseTS6 && ac == 11) {
        s = findserver_uid(servlist, source);
        /* Source is always the server */
	ts = strtoul(av[2], NULL, 10);
        user = do_nick("", av[0], av[4], !strcmp(av[8], "*") ? av[5] : av[8], s->name, av[10],
                       ts, !stricmp(av[0], av[9]) ? ts : 0, 0, av[5], av[7]);
        if (user) {
            anope_set_umode(user, 1, &av[3]);
        }
    }
    return MOD_CONT;
}

int anope_event_topic(const char *source, int ac, const char **av)
{
    User *u;

    if (ac == 4) {
        do_topic(source, ac, av);
    } else {
        Channel *c = findchan(av[0]);
        time_t topic_time = time(NULL);

        if (!c) {
            if (debug) {
                alog("debug: TOPIC %s for nonexistent channel %s",
                     merge_args(ac - 1, av + 1), av[0]);
            }
            return MOD_CONT;
        }

        if (check_topiclock(c, topic_time))
            return MOD_CONT;

        if (c->topic) {
            free(c->topic);
            c->topic = NULL;
        }
        if (ac > 1 && *av[1])
            c->topic = sstrdup(av[1]);

        if (UseTS6) {
            u = find_byuid(source);
            if (u) {
                strscpy(c->topic_setter, u->nick, sizeof(c->topic_setter));
            } else {
                strscpy(c->topic_setter, source, sizeof(c->topic_setter));
            }
        } else {
            strscpy(c->topic_setter, source, sizeof(c->topic_setter));
        }
        c->topic_time = topic_time;

        record_topic(av[0]);

		if (ac > 1 && *av[1])
		    send_event(EVENT_TOPIC_UPDATED, 2, av[0], av[1]);
		else
		    send_event(EVENT_TOPIC_UPDATED, 2, av[0], "");
    }
    return MOD_CONT;
}

int anope_event_tburst(const char *source, int ac, const char **av)
{
    char *setter;
    Channel *c;
    time_t topic_time;

    if (ac != 4) {
        return MOD_CONT;
    }

    setter = myStrGetToken(av[2], '!', 0);

    c = findchan(av[0]);
    topic_time = strtol(av[1], NULL, 10);

    if (!c) {
        if (debug) {
            alog("debug: TOPIC %s for nonexistent channel %s",
                 merge_args(ac - 1, av + 1), av[0]);
        }
        if (setter)
            free(setter);
        return MOD_CONT;
    }

    if (check_topiclock(c, topic_time)) {
        if (setter)
            free(setter);
        return MOD_CONT;
    }

    if (c->topic) {
        free(c->topic);
        c->topic = NULL;
    }
    if (ac > 1 && *av[3])
        c->topic = sstrdup(av[3]);

    strscpy(c->topic_setter, setter, sizeof(c->topic_setter));
    c->topic_time = topic_time;

    record_topic(av[0]);

	if (ac > 1 && *av[3])
	    send_event(EVENT_TOPIC_UPDATED, 2, av[0], av[3]);
	else
	    send_event(EVENT_TOPIC_UPDATED, 2, av[0], "");

    if (setter)
        free(setter);

    return MOD_CONT;
}

int anope_event_436(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;

    m_nickcoll(av[0]);
    return MOD_CONT;
}


/* *INDENT-OFF* */
void moduleAddIRCDMsgs(void)
{
    Message *m;

    updateProtectDetails("PROTECT","PROTECTME","protect","deprotect","AUTOPROTECT","+","-");

    if (UseTS6) {
        TS6SID = sstrdup(Numeric);
    }

    m = createMessage("401",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("402",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("436",       anope_event_436); addCoreMessage(IRCD,m);
    m = createMessage("AWAY",      anope_event_away); addCoreMessage(IRCD,m);
    m = createMessage("INVITE",    anope_event_invite); addCoreMessage(IRCD,m);
    m = createMessage("JOIN",      anope_event_join); addCoreMessage(IRCD,m);
    m = createMessage("KICK",      anope_event_kick); addCoreMessage(IRCD,m);
    m = createMessage("KILL",      anope_event_kill); addCoreMessage(IRCD,m);
    m = createMessage("MODE",      anope_event_mode); addCoreMessage(IRCD,m);
    m = createMessage("TMODE",     anope_event_tmode); addCoreMessage(IRCD,m);
    m = createMessage("MOTD",      anope_event_motd); addCoreMessage(IRCD,m);
    m = createMessage("NICK",      anope_event_nick); addCoreMessage(IRCD,m);
    m = createMessage("BMASK",     anope_event_bmask); addCoreMessage(IRCD,m);
    m = createMessage("UID",       anope_event_nick); addCoreMessage(IRCD,m);
    m = createMessage("NOTICE",    anope_event_notice); addCoreMessage(IRCD,m);
    m = createMessage("PART",      anope_event_part); addCoreMessage(IRCD,m);
    m = createMessage("PASS",      anope_event_pass); addCoreMessage(IRCD,m);
    m = createMessage("PING",      anope_event_ping); addCoreMessage(IRCD,m);
    m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
    m = createMessage("QUIT",      anope_event_quit); addCoreMessage(IRCD,m);
    m = createMessage("SERVER",    anope_event_server); addCoreMessage(IRCD,m);
    m = createMessage("SQUIT",     anope_event_squit); addCoreMessage(IRCD,m);
    m = createMessage("TOPIC",     anope_event_topic); addCoreMessage(IRCD,m);
    m = createMessage("TB",        anope_event_tburst); addCoreMessage(IRCD,m);
    m = createMessage("USER",      anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("WALLOPS",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("WHOIS",     anope_event_whois); addCoreMessage(IRCD,m);
    m = createMessage("SVSMODE",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SVSNICK",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("CAPAB",     anope_event_capab); addCoreMessage(IRCD,m);
    m = createMessage("SJOIN",     anope_event_sjoin); addCoreMessage(IRCD,m);
    m = createMessage("SVINFO",    anope_event_svinfo); addCoreMessage(IRCD,m);
    m = createMessage("ADMIN",     anope_event_admin); addCoreMessage(IRCD,m);
    m = createMessage("ERROR",     anope_event_error); addCoreMessage(IRCD,m);
    m = createMessage("421",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("ENCAP",     anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SID",       anope_event_sid); addCoreMessage(IRCD,m);
    m = createMessage("EUID",      anope_event_euid); addCoreMessage(IRCD,m);
}

/* *INDENT-ON* */


void CharybdisProto::cmd_sqline(const char *mask, const char *reason)
{
	Uid *ud = find_uid(s_OperServ);
	send_cmd(UseTS6 ? (ud ? ud->uid : s_OperServ) : s_OperServ, "RESV * %s :%s", mask, reason);
}

void CharybdisProto::cmd_unsgline(const char *mask)
{
	Uid *ud = find_uid(s_OperServ);
	send_cmd(UseTS6 ? (ud ? ud->uid : s_OperServ) : s_OperServ, "UNXLINE * %s", mask);
}

void charybdis_cmd_svsadmin(const char *server, int set)
{
}

void CharybdisProto::cmd_sgline(const char *mask, const char *reason)
{
	Uid *ud = find_uid(s_OperServ);
	send_cmd(UseTS6 ? (ud ? ud->uid : s_OperServ) : s_OperServ, "XLINE * %s 0 :%s", mask, reason);
}

void CharybdisProto::cmd_remove_akill(const char *user, const char *host)
{
	Uid *ud = find_uid(s_OperServ);
	send_cmd(UseTS6 ? (ud ? ud->uid : s_OperServ) : s_OperServ, "UNKLINE * %s %s", user, host);
}

void CharybdisProto::cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when)
{
	Uid *ud = find_uid(whosets);
	send_cmd(UseTS6 ? (ud ? ud->uid : whosets) : whosets, "TOPIC %s :%s", chan, topic);
}

void CharybdisProto::cmd_vhost_off(User *u)
{
	send_cmd(UseTS6 ? TS6SID : ServerName, "ENCAP * CHGHOST %s :%s", u->nick, u->host);
}

void CharybdisProto::cmd_vhost_on(const char *nick, const char *vIdent, const char *vhost)
{
	send_cmd(UseTS6 ? TS6SID : ServerName, "ENCAP * CHGHOST %s :%s", nick, vhost);
}

void CharybdisProto::cmd_unsqline(const char *user)
{
	Uid *ud = find_uid(s_OperServ);
	send_cmd(UseTS6 ? (ud ? ud->uid : s_OperServ) : s_OperServ, "UNRESV * %s", user);
}

void CharybdisProto::cmd_join(const char *user, const char *channel, time_t chantime)
{
	Uid *ud = find_uid(user);
	send_cmd(NULL, "SJOIN %ld %s + :%s", static_cast<long>(chantime), channel, UseTS6 ? (ud ? ud->uid : user) : user);
}

/*
oper:		the nick of the oper performing the kline
target.server:	the server(s) this kline is destined for
duration:	the duration if a tkline, 0 if permanent.
user:		the 'user' portion of the kline
host:		the 'host' portion of the kline
reason:		the reason for the kline.
*/

void CharybdisProto::cmd_akill(const char *user, const char *host, const char *who, time_t when, time_t expires, const char *reason)
{
	Uid *ud = find_uid(s_OperServ);
	send_cmd(UseTS6 ? (ud ? ud->uid : s_OperServ) : s_OperServ, "KLINE * %ld %s %s :%s", static_cast<long>(expires - time(NULL)), user, host, reason);
}

void CharybdisProto::cmd_svskill(const char *source, const char *user, const char *buf)
{
	if (!source || !user || !buf) return;
	Uid *ud = find_uid(source), *ud2 = find_uid(user);
	send_cmd(UseTS6 ? (ud ? ud->uid : source) : source, "KILL %s :%s", UseTS6 ? (ud2 ? ud2->uid : user) : user, buf);
}

void CharybdisProto::cmd_svsmode(User *u, int ac, const char **av)
{
	send_cmd(UseTS6 ? TS6SID : ServerName, "SVSMODE %s %s", u->nick, av[0]);
}

/*
 * SVINFO
 *      parv[0] = sender prefix
 *      parv[1] = TS_CURRENT for the server
 *      parv[2] = TS_MIN for the server
 *      parv[3] = server is standalone or connected to non-TS only
 *      parv[4] = server's idea of UTC time
 */
void charybdis_cmd_svinfo()
{
    send_cmd(NULL, "SVINFO 6 3 0 :%ld", (long int) time(NULL));
}

void charybdis_cmd_svsinfo()
{

}

/* CAPAB */
/*
  QS       - Can handle quit storm removal
  EX       - Can do channel +e exemptions
  CHW      - Can do channel wall @#
  LL       - Can do lazy links
  IE       - Can do invite exceptions
  EOB      - Can do EOB message
  KLN      - Can do KLINE message
  GLN      - Can do GLINE message
  HUB      - This server is a HUB
  UID      - Can do UIDs
  ZIP      - Can do ZIPlinks
  ENC      - Can do ENCrypted links
  KNOCK    - supports KNOCK
  TBURST   - supports TBURST
  PARA	   - supports invite broadcasting for +p
  ENCAP	   - supports message encapsulation
  SERVICES - supports services-oriented TS6 extensions
  EUID     - supports EUID and non-ENCAP CHGHOST
*/
void charybdis_cmd_capab()
{
    send_cmd(NULL,
             "CAPAB :QS EX CHW IE KLN GLN KNOCK TB UNKLN CLUSTER ENCAP SERVICES EUID");
}

/* PASS */
void charybdis_cmd_pass(const char *pass)
{
    if (UseTS6) {
        send_cmd(NULL, "PASS %s TS 6 :%s", pass, TS6SID);
    } else {
        send_cmd(NULL, "PASS %s :TS", pass);
    }
}

/* SERVER name hop descript */
void charybdis_cmd_server(const char *servname, int hop, const char *descript)
{
    send_cmd(NULL, "SERVER %s %d :%s", servname, hop, descript);
}

void CharybdisProto::cmd_connect()
{
	/* Make myself known to myself in the serverlist */
	if (UseTS6) me_server = new_server(NULL, ServerName, ServerDesc, SERVER_ISME, TS6SID);
	else me_server = new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);
	if (servernum == 1) charybdis_cmd_pass(RemotePassword);
	else if (servernum == 2) charybdis_cmd_pass(RemotePassword2);
	else if (servernum == 3) charybdis_cmd_pass(RemotePassword3);
	charybdis_cmd_capab();
	charybdis_cmd_server(ServerName, 1, ServerDesc);
	charybdis_cmd_svinfo();
}

void CharybdisProto::cmd_bot_nick(const char *nick, const char *user, const char *host, const char *real, const char *modes)
{
	EnforceQlinedNick(nick, NULL);
	if (UseTS6) {
		char *uidbuf = ts6_uid_retrieve();
		send_cmd(TS6SID, "UID %s 1 %ld %s %s %s 0 %s :%s", nick, static_cast<long>(time(NULL)), modes, user, host, uidbuf, real);
		new_uid(nick, uidbuf);
	}
	else send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s :%s", nick, static_cast<long>(time(NULL)), modes, user, host, ServerName, real);
	cmd_sqline(nick, "Reserved for services");
}

void CharybdisProto::cmd_part(const char *nick, const char *chan, const char *buf)
{
	Uid *ud = find_uid(nick);
	if (buf) send_cmd(UseTS6 ? ud->uid : nick, "PART %s :%s", chan, buf);
	else send_cmd(UseTS6 ? ud->uid : nick, "PART %s", chan);
}

int anope_event_ping(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;
    ircd_proto.cmd_pong(ac > 1 ? av[1] : ServerName, av[0]);
    return MOD_CONT;
}

int anope_event_away(const char *source, int ac, const char **av)
{
    User *u = NULL;

    if (UseTS6) {
        u = find_byuid(source);
    }

    m_away((UseTS6 ? (u ? u->nick : source) : source),
           (ac ? av[0] : NULL));
    return MOD_CONT;
}

int anope_event_kill(const char *source, int ac, const char **av)
{
    User *u = NULL;

    if (ac != 2)
        return MOD_CONT;

    if (UseTS6) {
        u = find_byuid(av[0]);
    }

    m_kill(u ? u->nick : av[0], av[1]);
    return MOD_CONT;
}

int anope_event_kick(const char *source, int ac, const char **av)
{
    if (ac != 3)
        return MOD_CONT;
    do_kick(source, ac, av);
    return MOD_CONT;
}

int anope_event_join(const char *source, int ac, const char **av)
{
    if (ac != 1) {
	/* ignore cmodes in JOIN as per TS6 v8 */
        do_sjoin(source, ac > 2 ? 2 : ac, av);
        return MOD_CONT;
    } else {
        do_join(source, ac, av);
    }
    return MOD_CONT;
}

int anope_event_motd(const char *source, int ac, const char **av)
{
    if (!source) {
        return MOD_CONT;
    }

    m_motd(source);
    return MOD_CONT;
}

int anope_event_privmsg(const char *source, int ac, const char **av)
{
    User *u;
    Uid *ud;

    if (ac != 2) {
        return MOD_CONT;
    }

    u = find_byuid(source);
    ud = find_nickuid(av[0]);
    m_privmsg((UseTS6 ? (u ? u->nick : source) : source),
              (UseTS6 ? (ud ? ud->nick : av[0]) : av[0]), av[1]);
    return MOD_CONT;
}

int anope_event_part(const char *source, int ac, const char **av)
{
    User *u;

    if (ac < 1 || ac > 2) {
        return MOD_CONT;
    }

    u = find_byuid(source);
    do_part((UseTS6 ? (u ? u->nick : source) : source), ac, av);

    return MOD_CONT;
}

int anope_event_whois(const char *source, int ac, const char **av)
{
    Uid *ud;

    if (source && ac >= 1) {
        ud = find_nickuid(av[0]);
        m_whois(source, (UseTS6 ? (ud ? ud->nick : av[0]) : av[0]));
    }
    return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(const char *source, int ac, const char **av)
{
    if (!stricmp(av[1], "1")) {
        uplink = sstrdup(av[0]);
        if (UseTS6 && TS6UPLINK) {
            do_server(source, av[0], av[1], av[2], TS6UPLINK);
        } else {
            do_server(source, av[0], av[1], av[2], NULL);
        }
    } else {
        do_server(source, av[0], av[1], av[2], NULL);
    }
    return MOD_CONT;
}

int anope_event_sid(const char *source, int ac, const char **av)
{
    Server *s;

    /* :42X SID trystan.nomadirc.net 2 43X :ircd-charybdis test server */

    s = findserver_uid(servlist, source);

    do_server(s->name, av[0], av[1], av[3], av[2]);
    return MOD_CONT;
}

int anope_event_squit(const char *source, int ac, const char **av)
{
    if (ac != 2)
        return MOD_CONT;
    do_squit(source, ac, av);
    return MOD_CONT;
}

int anope_event_quit(const char *source, int ac, const char **av)
{
    User *u;

    if (ac != 1) {
        return MOD_CONT;
    }

    u = find_byuid(source);

    do_quit((UseTS6 ? (u ? u->nick : source) : source), ac, av);
    return MOD_CONT;
}

void charybdis_cmd_372(const char *source, const char *msg)
{
    send_cmd((UseTS6 ? TS6SID : ServerName), "372 %s :- %s", source, msg);
}

void charybdis_cmd_372_error(const char *source)
{
    send_cmd((UseTS6 ? TS6SID : ServerName),
             "422 %s :- MOTD file not found!  Please "
             "contact your IRC administrator.", source);
}

void charybdis_cmd_375(const char *source)
{
    send_cmd((UseTS6 ? TS6SID : ServerName),
             "375 %s :- %s Message of the Day", source, ServerName);
}

void charybdis_cmd_376(const char *source)
{
    send_cmd((UseTS6 ? TS6SID : ServerName),
             "376 %s :End of /MOTD command.", source);
}

/* 391 */
void charybdis_cmd_391(const char *source, const char *timestr)
{
    if (!timestr) {
        return;
    }
    send_cmd(NULL, "391 :%s %s :%s", source, ServerName, timestr);
}

/* 250 */
void charybdis_cmd_250(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "250 %s", buf);
}

/* 307 */
void charybdis_cmd_307(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd((UseTS6 ? TS6SID : ServerName), "307 %s", buf);
}

/* 311 */
void charybdis_cmd_311(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd((UseTS6 ? TS6SID : ServerName), "311 %s", buf);
}

/* 312 */
void charybdis_cmd_312(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd((UseTS6 ? TS6SID : ServerName), "312 %s", buf);
}

/* 317 */
void charybdis_cmd_317(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd((UseTS6 ? TS6SID : ServerName), "317 %s", buf);
}

/* 219 */
void charybdis_cmd_219(const char *source, const char *letter)
{
    if (!source) {
        return;
    }

    if (letter) {
        send_cmd(NULL, "219 %s %c :End of /STATS report.", source,
                 *letter);
    } else {
        send_cmd(NULL, "219 %s l :End of /STATS report.", source);
    }
}

/* 401 */
void charybdis_cmd_401(const char *source, const char *who)
{
    if (!source || !who) {
        return;
    }
    send_cmd((UseTS6 ? TS6SID : ServerName), "401 %s %s :No such service.",
             source, who);
}

/* 318 */
void charybdis_cmd_318(const char *source, const char *who)
{
    if (!source || !who) {
        return;
    }

    send_cmd((UseTS6 ? TS6SID : ServerName),
             "318 %s %s :End of /WHOIS list.", source, who);
}

/* 242 */
void charybdis_cmd_242(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "242 %s", buf);
}

/* 243 */
void charybdis_cmd_243(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "243 %s", buf);
}

/* 211 */
void charybdis_cmd_211(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "211 %s", buf);
}

void CharybdisProto::cmd_mode(const char *source, const char *dest, const char *buf)
{
	if (!buf) return;
	if (source) {
		Uid *ud = find_uid(source);
		send_cmd(UseTS6 ? (ud ? ud->uid : source) : source, "MODE %s %s", dest, buf);
	}
	else send_cmd(source, "MODE %s %s", dest, buf);
}

void charybdis_cmd_tmode(const char *source, const char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!*buf) {
        return;
    }

    send_cmd(NULL, "MODE %s %s", dest, buf);
}

void CharybdisProto::cmd_nick(const char *nick, const char *name, const char *mode)
{
	EnforceQlinedNick(nick, NULL);
	if (UseTS6) {
		char *uidbuf = ts6_uid_retrieve();
		send_cmd(TS6SID, "UID %s 1 %ld %s %s %s 0 %s :%s", nick, static_cast<long>(time(NULL)), mode, ServiceUser, ServiceHost, uidbuf, name);
		new_uid(nick, uidbuf);
	}
	else send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s :%s", nick, static_cast<long>(time(NULL)), mode, ServiceUser, ServiceHost, ServerName, name);
	cmd_sqline(nick, "Reserved for services");
}

void CharybdisProto::cmd_kick(const char *source, const char *chan, const char *user, const char *buf)
{
	Uid *ud = find_uid(source);
	User *u = finduser(user);
	if (buf) send_cmd(UseTS6 ? (ud ? ud->uid : source) : source, "KICK %s %s :%s", chan, UseTS6 ? (u ? u->uid : user) : user, buf);
	else send_cmd(UseTS6 ? (ud ? ud->uid : source) : source, "KICK %s %s", chan, UseTS6 ? (u ? u->uid : user) : user);
}

void CharybdisProto::cmd_notice_ops(const char *source, const char *dest, const char *buf)
{
	if (!buf) return;
	Uid *ud = find_uid(source);
	send_cmd(UseTS6 ? (ud ? ud->uid : source) : source, "NOTICE @%s :%s", dest, buf);
}

void CharybdisProto::cmd_bot_chan_mode(const char *nick, const char *chan)
{
	if (UseTS6) {
		Uid *u = find_uid(nick);
		charybdis_cmd_tmode(nick, chan, "%s %s", ircd->botchanumode, u ? u->uid : nick);
	}
	else anope_cmd_mode(ServerName, chan, "%s %s", ircd->botchanumode, nick);
}

/* QUIT */
void CharybdisProto::cmd_quit(const char *source, const char *buf)
{
	Uid *ud = find_uid(source);
	if (buf) send_cmd(UseTS6 ? (ud ? ud->uid : source) : source, "QUIT :%s", buf);
	else send_cmd(UseTS6 ? (ud ? ud->uid : source) : source, "QUIT");
}

/* PONG */
void CharybdisProto::cmd_pong(const char *servname, const char *who)
{
	/* deliberately no SID in the first parameter -- jilles */
	if (UseTS6) send_cmd(TS6SID, "PONG %s :%s", servname, who);
	else send_cmd(servname, "PONG %s :%s", servname, who);
}

/* INVITE */
void CharybdisProto::cmd_invite(const char *source, const char *chan, const char *nick)
{
	if (!source || !chan || !nick) return;
	Uid *ud = find_uid(source);
	User *u = finduser(nick);
	send_cmd(UseTS6 ? (ud ? ud->uid : source) : source, "INVITE %s %s", UseTS6 ? (u ? u->uid : nick) : nick, chan);
}

int anope_event_mode(const char *source, int ac, const char **av)
{
    User *u, *u2;

    if (ac < 2) {
        return MOD_CONT;
    }

    if (*av[0] == '#' || *av[0] == '&') {
        do_cmode(source, ac, av);
    } else {
        if (UseTS6) {
            u = find_byuid(source);
            u2 = find_byuid(av[0]);
            av[0] = u2->nick;
            do_umode(u->nick, ac, av);
        } else {
            do_umode(source, ac, av);
        }
    }
    return MOD_CONT;
}

int anope_event_tmode(const char *source, int ac, const char **av)
{
    if (ac > 2 && (*av[1] == '#' || *av[1] == '&')) {
        do_cmode(source, ac, av);
    }
    return MOD_CONT;
}

void charybdis_cmd_351(const char *source)
{
    send_cmd((UseTS6 ? TS6SID : ServerName),
             "351 %s Anope-%s %s :%s - %s (%s) -- %s", source, version_number,
             ServerName, ircd->name, version_flags, EncModule, version_build);
}

/* Event: PROTOCTL */
int anope_event_capab(const char *source, int ac, const char **av)
{
    int argvsize = 8;
    int argc;
    const char **argv;
    char *str;

    if (ac < 1)
        return MOD_CONT;

    /* We get the params as one arg, we should split it for capab_parse */
    argv = (const char **)scalloc(argvsize, sizeof(const char *));
    argc = 0;
    while ((str = myStrGetToken(av[0], ' ', argc))) {
        if (argc == argvsize) {
            argvsize += 8;
            argv = (const char **)srealloc(argv, argvsize * sizeof(const char *));
        }
        argv[argc] = str;
        argc++;
    }

    capab_parse(argc, argv);

    /* Free our built ac/av */
    for (argvsize = 0; argvsize < argc; argvsize++) {
        free((char *)argv[argvsize]);
    }
    free((char **)argv);

    return MOD_CONT;
}

/* SVSHOLD - set */
void CharybdisProto::cmd_svshold(const char *nick)
{
	send_cmd(NULL, "ENCAP * NICKDELAY 300 %s", nick);
}

/* SVSHOLD - release */
void CharybdisProto::cmd_release_svshold(const char *nick)
{
	send_cmd(NULL, "ENCAP * NICKDELAY 0 %s", nick);
}

/* SVSNICK */
void CharybdisProto::cmd_svsnick(const char *oldnick, const char *newnick, time_t when)
{
	if (!oldnick || !newnick) return;
	User *u = finduser(oldnick);
	if (!u) return;
	send_cmd(NULL, "ENCAP %s RSFNC %s %s %ld %ld", u->server->name, u->nick, newnick, static_cast<long>(when), static_cast<long>(u->timestamp));
}

/*
 * SVINFO
 *      parv[0] = sender prefix
 *      parv[1] = TS_CURRENT for the server
 *      parv[2] = TS_MIN for the server
 *      parv[3] = server is standalone or connected to non-TS only
 *      parv[4] = server's idea of UTC time
 */
int anope_event_svinfo(const char *source, int ac, const char **av)
{
    /* currently not used but removes the message : unknown message from server */
    return MOD_CONT;
}

int anope_event_pass(const char *source, int ac, const char **av)
{
    if (UseTS6) {
        TS6UPLINK = sstrdup(av[3]);
    }
    return MOD_CONT;
}

int anope_event_notice(const char *source, int ac, const char **av)
{
    return MOD_CONT;
}

int anope_event_admin(const char *source, int ac, const char **av)
{
    return MOD_CONT;
}

int anope_event_invite(const char *source, int ac, const char **av)
{
    return MOD_CONT;
}

int anope_event_bmask(const char *source, int ac, const char **av)
{
    Channel *c;
    char *bans;
    char *b;
    int count, i;

    /* :42X BMASK 1106409026 #ircops b :*!*@*.aol.com */
    /*             0          1      2   3            */
    c = findchan(av[1]);

    if (c) {
        bans = sstrdup(av[3]);
        count = myNumToken(bans, ' ');
        for (i = 0; i <= count - 1; i++) {
            b = myStrGetToken(bans, ' ', i);
            if (!stricmp(av[2], "b")) {
                add_ban(c, b);
            }
            if (!stricmp(av[2], "e")) {
                add_exception(c, b);
            }
            if (!stricmp(av[2], "I")) {
                add_invite(c, b);
            }
            if (b)
                free(b);
        }
        free(bans);
    }
    return MOD_CONT;
}

int charybdis_flood_mode_check(const char *value)
{
    char *dp, *end;

    if (value && *value != ':'
        && (strtoul((*value == '*' ? value + 1 : value), &dp, 10) > 0)
        && (*dp == ':') && (*(++dp) != 0) && (strtoul(dp, &end, 10) > 0)
        && (*end == 0)) {
        return 1;
    } else {
        return 0;
    }

    return 0;
}

int anope_event_error(const char *source, int ac, const char **av)
{
    if (ac >= 1) {
        if (debug) {
            alog("debug: %s", av[0]);
        }
    }
    return MOD_CONT;
}

void charybdis_cmd_jupe(const char *jserver, const char *who, const char *reason)
{
    char rbuf[256];

    snprintf(rbuf, sizeof(rbuf), "Juped by %s%s%s", who,
             reason ? ": " : "", reason ? reason : "");

    if (findserver(servlist, jserver))
        ircd_proto.cmd_squit(jserver, rbuf);
    charybdis_cmd_server(jserver, 2, rbuf);
    new_server(me_server, jserver, rbuf, SERVER_JUPED, NULL);
}

/*
  1 = valid nick
  0 = nick is in valid
*/
int charybdis_valid_nick(const char *nick)
{
    /* TS6 Save extension -Certus */
    if (isdigit(*nick))
        return 0;
    return 1;
}

/*
  1 = valid chan
  0 = chan is invalid
*/
int charybdis_valid_chan(const char *chan)
{
    /* no hard coded invalid chan */
    return 1;
}


int charybdis_send_account(int argc, char **argv)
{
    send_cmd((UseTS6 ? TS6SID : ServerName), "ENCAP * SU %s :%s",
	argv[0], argv[0]);

    return MOD_CONT;
}

/* XXX: We need a hook on /ns logout before this is useful! --nenolod */
/* We have one now! -GD */
int charybdis_send_deaccount(int argc, char **argv)
{
    send_cmd((UseTS6 ? TS6SID : ServerName), "ENCAP * SU %s",
	argv[0]);

    return MOD_CONT;
}

/**
 * Tell anope which function we want to perform each task inside of anope.
 * These prototypes must match what anope expects.
 **/
void moduleAddAnopeCmds()
{
    pmodule_cmd_372(charybdis_cmd_372);
    pmodule_cmd_372_error(charybdis_cmd_372_error);
    pmodule_cmd_375(charybdis_cmd_375);
    pmodule_cmd_376(charybdis_cmd_376);
    pmodule_cmd_351(charybdis_cmd_351);
    pmodule_cmd_391(charybdis_cmd_391);
    pmodule_cmd_250(charybdis_cmd_250);
    pmodule_cmd_307(charybdis_cmd_307);
    pmodule_cmd_311(charybdis_cmd_311);
    pmodule_cmd_312(charybdis_cmd_312);
    pmodule_cmd_317(charybdis_cmd_317);
    pmodule_cmd_219(charybdis_cmd_219);
    pmodule_cmd_401(charybdis_cmd_401);
    pmodule_cmd_318(charybdis_cmd_318);
    pmodule_cmd_242(charybdis_cmd_242);
    pmodule_cmd_243(charybdis_cmd_243);
    pmodule_cmd_211(charybdis_cmd_211);
    pmodule_flood_mode_check(charybdis_flood_mode_check);
    pmodule_cmd_jupe(charybdis_cmd_jupe);
    pmodule_valid_nick(charybdis_valid_nick);
    pmodule_valid_chan(charybdis_valid_chan);
    pmodule_set_umode(charybdis_set_umode);
}

/**
 * Now tell anope how to use us.
 **/
int AnopeInit(int argc, char **argv)
{
    EvtHook *hk;

    moduleAddAuthor("Anope");
    moduleAddVersion("$Id: charybdis.c 953 2006-01-14 11:36:29Z certus $");
    moduleSetType(PROTOCOL);

    pmodule_ircd_version("Charybdis 1.0/1.1+");
    pmodule_ircd_cap(myIrcdcap);
    pmodule_ircd_var(myIrcd);
    pmodule_ircd_cbmodeinfos(myCbmodeinfos);
    pmodule_ircd_cumodes(myCumodes);
    pmodule_ircd_flood_mode_char_set("");
    pmodule_ircd_flood_mode_char_remove("");
    pmodule_ircd_cbmodes(myCbmodes);
    pmodule_ircd_cmmodes(myCmmodes);
    pmodule_ircd_csmodes(myCsmodes);
    pmodule_ircd_useTSMode(0);

        /** Deal with modes anope _needs_ to know **/
    pmodule_invis_umode(UMODE_i);
    pmodule_oper_umode(UMODE_o);
    pmodule_invite_cmode(CMODE_i);
    pmodule_secret_cmode(CMODE_s);
    pmodule_private_cmode(CMODE_p);
    pmodule_key_mode(CMODE_k);
    pmodule_limit_mode(CMODE_l);

    moduleAddAnopeCmds();
	pmodule_ircd_proto(&ircd_proto);
    moduleAddIRCDMsgs();

    hk = createEventHook(EVENT_NICK_IDENTIFY, charybdis_send_account);
    moduleAddEventHook(hk);

    hk = createEventHook(EVENT_NICK_REGISTERED, charybdis_send_account);
    moduleAddEventHook(hk);

    /* XXX: It'd be nice if we could have an event like this, but it's not there yet :( */
	/* It's there now! Trystan said so! -GD */
    hk = createEventHook(EVENT_NICK_LOGOUT, charybdis_send_deaccount);
    moduleAddEventHook(hk);

    return MOD_CONT;
}

/* Clean up allocated things in here */
void AnopeFini(void)
{
	if (UseTS6)
		free(TS6SID);
}

/* EOF */
