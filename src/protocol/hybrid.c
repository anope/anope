/* Hybrid IRCD functions
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
#include "pseudo.h"
#include "hybrid.h"
#include "version.h"

IRCDVar myIrcd[] = {
    {"HybridIRCd 7.*",          /* ircd name */
     "+o",                      /* nickserv mode */
     "+o",                      /* chanserv mode */
     "+o",                      /* memoserv mode */
     NULL,                      /* hostserv mode */
     "+aio",                    /* operserv mode */
     "+o",                      /* botserv mode  */
     "+h",                      /* helpserv mode */
     "+i",                      /* Dev/Null mode */
     "+io",                     /* Global mode   */
     "+o",                      /* nickserv alias mode */
     "+o",                      /* chanserv alias mode */
     "+o",                      /* memoserv alias mode */
     NULL,                      /* hostserv alias mode */
     "+aio",                    /* operserv alias mode */
     "+o",                      /* botserv alias mode  */
     "+h",                      /* helpserv alias mode */
     "+i",                      /* Dev/Null alias mode */
     "+io",                     /* Global alias mode   */
     "+",                       /* Used by BotServ Bots */
     3,                         /* Chan Max Symbols     */
     "-ailmnpst",               /* Modes to Remove */
     "+o",                      /* Channel Umode used by Botserv bots */
     0,                         /* SVSNICK */
     0,                         /* Vhost  */
     0,                         /* Has Owner */
     NULL,                      /* Mode to set for an owner */
     NULL,                      /* Mode to unset for an owner */
     NULL,                      /* Mode to set for admin */
     NULL,                      /* Mode to unset for admin */
     NULL,                      /* Mode On Reg          */
     NULL,                      /* Mode on ID for Roots */
     NULL,                      /* Mode on ID for Admins */
     NULL,                      /* Mode on ID for Opers */
     NULL,                      /* Mode on UnReg        */
     NULL,                      /* Mode on Nick Change  */
     1,                         /* Supports SGlines     */
     1,                         /* Supports SQlines     */
     0,                         /* Supports SZlines     */
     1,                         /* Supports Halfop +h   */
     3,                         /* Number of server args */
     1,                         /* Join 2 Set           */
     1,                         /* Join 2 Message       */
     1,                         /* Has exceptions +e    */
     0,                         /* TS Topic Forward     */
     0,                         /* TS Topci Backward    */
     0,                         /* Protected Umode      */
     0,                         /* Has Admin            */
     0,                         /* Chan SQlines         */
     0,                         /* Quit on Kill         */
     0,                         /* SVSMODE unban        */
     0,                         /* Has Protect          */
     0,                         /* Reverse              */
     0,                         /* Chan Reg             */
     0,                         /* Channel Mode         */
     0,                         /* vidents              */
     0,                         /* svshold              */
     0,                         /* time stamp on mode   */
     0,                         /* NICKIP               */
     0,                         /* UMODE                */
     0,                         /* O:LINE               */
     0,                         /* VHOST ON NICK        */
     0,                         /* Change RealName      */
     CMODE_p,                   /* No Knock             */
     0,                         /* Admin Only           */
     DEFAULT_MLOCK,             /* Default MLOCK        */
     0,                         /* Vhost Mode           */
     0,                         /* +f                   */
     0,                         /* +L                   */
     0,                         /* +f Mode                          */
     0,                         /* +L Mode                              */
     0,                         /* On nick change check if they could be identified */
     0,                         /* No Knock requires +i */
     NULL,                      /* CAPAB Chan Modes             */
     0,                         /* We support TOKENS */
     1,                         /* TOKENS are CASE inSensitive */
     0,                         /* TIME STAMPS are BASE64 */
     0,                         /* +I support */
     0,                         /* SJOIN ban char */
     0,                         /* SJOIN except char */
     0,                         /* SJOIN invite char */
     0,                         /* Can remove User Channel Modes with SVSMODE */
     0,                         /* Sglines are not enforced until user reconnects */
     NULL,                      /* vhost char */
     0,                         /* ts6 */
     0,                         /* support helper umode */
     0,                         /* p10 */
     NULL,                      /* character set */
     0,                         /* reports sync state */
     0,                         /* CIDR channelbans */
     0,                         /* +j */
     0,                         /* +j mode */
     0,                         /* Use delayed client introduction. */
     }
    ,
    {NULL}
};

IRCDCAPAB myIrcdcap[] = {
    {
     CAPAB_NOQUIT,              /* NOQUIT       */
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



void hybrid_set_umode(User * user, int ac, char **av)
{
    int add = 1;                /* 1 if adding modes, 0 if deleting */
    char *modes = av[0];

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
        case 'd':
            if (ac == 0) {
                alog("user: umode +d with no parameter (?) for user %s",
                     user->nick);
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
                    anope_cmd_global(s_OperServ,
                                     "\2%s\2 is now an IRC operator.",
                                     user->nick);
                display_news(user, NEWS_OPER);

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
    0, 0, 0,                    /* P Q R */
    0, 0, 0,                    /* S T U */
    0, 0, 0,                    /* V W X */
    0,                          /* Y */
    0,                          /* Z */
    0, 0, 0,                    /* [ \ ] */
    0, 0, 0,                    /* ^ _ ` */
    UMODE_a, UMODE_b, UMODE_c,  /* a b c */
    UMODE_d, 0, UMODE_f,        /* d e f */
    UMODE_g, 0, UMODE_i,        /* g h i */
    0, UMODE_k, UMODE_l,        /* j k l */
    0, UMODE_n, UMODE_o,        /* m n o */
    0, 0, UMODE_r,              /* p q r */
    UMODE_s, 0, UMODE_u,        /* s t u */
    0, UMODE_w, UMODE_x,        /* v w x */
    UMODE_y,                    /* y */
    UMODE_z,                    /* z */
    0, 0, 0,                    /* { | } */
    0, 0                        /* ~ � */
};


char myCsmodes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0,
    'a',                        /* (33) !  */
    0, 0, 0,
    'h',
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
    {0},                        /* F */
    {0},                        /* G */
    {0},                        /* H */
    {0},                        /* I */
    {0},                        /* J */
    {0},                        /* K */
    {0},                        /* L */
    {0},                        /* M */
    {0},                        /* N */
    {0},                        /* O */
    {0},                        /* P */
    {0},                        /* Q */
    {0},
    {0},                        /* S */
    {0},                        /* T */
    {0},                        /* U */
    {0},                        /* V */
    {0},                        /* W */
    {0},                        /* X */
    {0},                        /* Y */
    {0},                        /* Z */
    {0}, {0}, {0}, {0}, {0}, {0},
    {CMODE_a, 0, NULL, NULL},
    {0},                        /* b */
    {0},                        /* c */
    {0},                        /* d */
    {0},                        /* e */
    {0},                        /* f */
    {0},                        /* g */
    {0},                        /* h */
    {CMODE_i, 0, NULL, NULL},
    {0},                        /* j */
    {CMODE_k, 0, chan_set_key, cs_set_key},
    {CMODE_l, CBM_MINUS_NO_ARG, set_limit, cs_set_limit},
    {CMODE_m, 0, NULL, NULL},
    {CMODE_n, 0, NULL, NULL},
    {0},                        /* o */
    {CMODE_p, 0, NULL, NULL},
    {0},                        /* q */
    {0},
    {CMODE_s, 0, NULL, NULL},
    {CMODE_t, 0, NULL, NULL},
    {0},
    {0},                        /* v */
    {0},                        /* w */
    {0},                        /* x */
    {0},                        /* y */
    {0},                        /* z */
    {0}, {0}, {0}, {0}
};

CBModeInfo myCbmodeinfos[] = {
    {'a', CMODE_a, 0, NULL, NULL},
    {'i', CMODE_i, 0, NULL, NULL},
    {'k', CMODE_k, 0, get_key, cs_get_key},
    {'l', CMODE_l, CBM_MINUS_NO_ARG, get_limit, cs_get_limit},
    {'m', CMODE_m, 0, NULL, NULL},
    {'n', CMODE_n, 0, NULL, NULL},
    {'p', CMODE_p, 0, NULL, NULL},
    {'s', CMODE_s, 0, NULL, NULL},
    {'t', CMODE_t, 0, NULL, NULL},
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
    {CUS_HALFOP, 0, check_valid_op},
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



void hybrid_cmd_notice(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    if (NSDefFlags & NI_MSG) {
        hybrid_cmd_privmsg2(source, dest, buf);
    } else {
        send_cmd(source, "NOTICE %s :%s", dest, buf);
    }
}

void hybrid_cmd_notice2(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE %s :%s", dest, msg);
}

void hybrid_cmd_privmsg(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "PRIVMSG %s :%s", dest, buf);
}

void hybrid_cmd_privmsg2(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG %s :%s", dest, msg);
}

void hybrid_cmd_serv_notice(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE $$%s :%s", dest, msg);
}

void hybrid_cmd_serv_privmsg(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG $$%s :%s", dest, msg);
}


void hybrid_cmd_global(char *source, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source ? source : ServerName, "OPERWALL :%s", buf);
}

/* GLOBOPS - to handle old WALLOPS */
void hybrid_cmd_global_legacy(char *source, char *fmt)
{
    send_cmd(source ? source : ServerName, "OPERWALL :%s", fmt);
}

int anope_event_sjoin(char *source, int ac, char **av)
{
    do_sjoin(source, ac, av);
    return MOD_CONT;
}

int anope_event_nick(char *source, int ac, char **av)
{
    if (ac != 2) {
        User *user = do_nick(source, av[0], av[4], av[5], av[6], av[7],
                             strtoul(av[2], NULL, 10), 0, 0, NULL, NULL);
        if (user)
            anope_set_umode(user, 1, &av[3]);
    } else {
        do_nick(source, av[0], NULL, NULL, NULL, NULL,
                strtoul(av[1], NULL, 10), 0, 0, NULL, NULL);
    }
    return MOD_CONT;
}

int anope_event_topic(char *source, int ac, char **av)
{
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

        strscpy(c->topic_setter, source, sizeof(c->topic_setter));
        c->topic_time = topic_time;

        record_topic(av[0]);

		if (ac > 1 && *av[1])
		    send_event(EVENT_TOPIC_UPDATED, 2, av[0], av[1]);
		else
		    send_event(EVENT_TOPIC_UPDATED, 2, av[0], "");
    }
    return MOD_CONT;
}

int anope_event_tburst(char *source, int ac, char **av)
{
    if (ac != 5)
        return MOD_CONT;

    av[0] = av[1];
    av[1] = av[3];
    av[3] = av[4];
    do_topic(source, 4, av);
    return MOD_CONT;
}

int anope_event_436(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;

    m_nickcoll(av[0]);
    return MOD_CONT;
}


/* *INDENT-OFF* */
void moduleAddIRCDMsgs(void) {
    Message *m;

    updateProtectDetails("PROTECT","PROTECTME","protect","deprotect","AUTOPROTECT","+","-");

    m = createMessage("401",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("402",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("436",       anope_event_436); addCoreMessage(IRCD,m);
    m = createMessage("AWAY",      anope_event_away); addCoreMessage(IRCD,m);
    m = createMessage("INVITE",    anope_event_invite); addCoreMessage(IRCD,m);
    m = createMessage("JOIN",      anope_event_join); addCoreMessage(IRCD,m);
    m = createMessage("KICK",      anope_event_kick); addCoreMessage(IRCD,m);
    m = createMessage("KILL",      anope_event_kill); addCoreMessage(IRCD,m);
    m = createMessage("MODE",      anope_event_mode); addCoreMessage(IRCD,m);
    m = createMessage("MOTD",      anope_event_motd); addCoreMessage(IRCD,m);
    m = createMessage("NICK",      anope_event_nick); addCoreMessage(IRCD,m);
    m = createMessage("NOTICE",    anope_event_notice); addCoreMessage(IRCD,m);
    m = createMessage("PART",      anope_event_part); addCoreMessage(IRCD,m);
    m = createMessage("PASS",      anope_event_pass); addCoreMessage(IRCD,m);
    m = createMessage("PING",      anope_event_ping); addCoreMessage(IRCD,m);
    m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
    m = createMessage("QUIT",      anope_event_quit); addCoreMessage(IRCD,m);
    m = createMessage("SERVER",    anope_event_server); addCoreMessage(IRCD,m);
    m = createMessage("SQUIT",     anope_event_squit); addCoreMessage(IRCD,m);
    m = createMessage("TOPIC",     anope_event_topic); addCoreMessage(IRCD,m);
    m = createMessage("TBURST",    anope_event_tburst); addCoreMessage(IRCD,m);
    m = createMessage("USER",      anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("WALLOPS",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("WHOIS",     anope_event_whois); addCoreMessage(IRCD,m);
    m = createMessage("AKILL",     anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("GLOBOPS",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("GNOTICE",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("GOPER",     anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("RAKILL",    anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SILENCE",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SVSKILL",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SVSMODE",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SVSNICK",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SVSNOOP",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SQLINE",    anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("UNSQLINE",  anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("CAPAB",     anope_event_capab); addCoreMessage(IRCD,m);
    m = createMessage("SJOIN",     anope_event_sjoin); addCoreMessage(IRCD,m);
    m = createMessage("SVINFO",    anope_event_svinfo); addCoreMessage(IRCD,m);
/*    m = createMessage("EOB",       anope_event_eob); addCoreMessage(IRCD,m); */
    m = createMessage("ADMIN",     anope_event_admin); addCoreMessage(IRCD,m);
    m = createMessage("ERROR",     anope_event_error); addCoreMessage(IRCD,m);
}

/* *INDENT-ON* */


void hybrid_cmd_sqline(char *mask, char *reason)
{
    if (!mask || !reason) {
        return;
    }
    
    send_cmd(ServerName, "RESV * %s :%s", mask, reason);
}
void hybrid_cmd_unsgline(char *mask)
{
    if (!mask) {
        return;
    }
    
    send_cmd(ServerName, "UNXLINE * %s", mask);
}

void hybrid_cmd_unszline(char *mask)
{
    /* Does not support */
}
void hybrid_cmd_szline(char *mask, char *reason, char *whom)
{
    /* Does not support */
}

void hybrid_cmd_svsnoop(char *server, int set)
{
    /* does not support */
}

void hybrid_cmd_svsadmin(char *server, int set)
{
    hybrid_cmd_svsnoop(server, set);
}

void hybrid_cmd_sgline(char *mask, char *reason)
{
    if (!mask || !reason) {
        return;
    }
    
    send_cmd(ServerName, "XLINE * %s 0 :%s", mask, reason);
}

void hybrid_cmd_remove_akill(char *user, char *host)
{
    /* hybrid seems to support UNKLINE  -Certus */
    send_cmd(s_OperServ, "UNKLINE * %s %s", user, host);
}

void hybrid_cmd_topic(char *whosets, char *chan, char *whosetit,
                      char *topic, time_t when)
{
    send_cmd(whosets, "TOPIC %s :%s", chan, topic);
}

void hybrid_cmd_vhost_off(User * u)
{
	if (ircd->vhost && ircd->vident)
	{
		send_cmd(NULL, "ENCAP * CHGIDENT %s %s", u->nick, u->username);
		send_cmd(NULL, "ENCAP * CHGHOST %s %s", u->nick, u->host);
	}
}

void hybrid_cmd_vhost_on(char *nick, char *vIdent, char *vhost)
{
	if (ircd->vhost && ircd->vident)
	{
		send_cmd(NULL, "ENCAP * CHGIDENT %s %s", nick, vIdent);
		send_cmd(NULL, "ENCAP * CHGHOST %s %s", nick, vhost);
	}
}

void hybrid_cmd_unsqline(char *user)
{
    if (!user) {
        return;
    }
    
    send_cmd(ServerName, "UNRESV * %s", user);
}

void hybrid_cmd_join(char *user, char *channel, time_t chantime)
{
    send_cmd(NULL, "SJOIN %ld %s + :%s", (long int) chantime, channel,
             user);
}

/*
oper:		the nick of the oper performing the kline
target.server:	the server(s) this kline is destined for
duration:	the duration if a tkline, 0 if permanent.
user:		the 'user' portion of the kline
host:		the 'host' portion of the kline
reason:		the reason for the kline.
*/

void hybrid_cmd_akill(char *user, char *host, char *who, time_t when,
                      time_t expires, char *reason)
{
    send_cmd(s_OperServ, "KLINE * %ld %s %s :%s",
             (long int) (expires - (long) time(NULL)), user, host, reason);
}

void hybrid_cmd_svskill(char *source, char *user, char *buf)
{
    if (!buf) {
        return;
    }

    if (!source || !user) {
        return;
    }

    send_cmd(source, "KILL %s :%s", user, buf);
}


void hybrid_cmd_svsmode(User * u, int ac, char **av)
{
    /* Hybrid does not support SVSMODE */
}

/*
 * SVINFO
 *      parv[0] = sender prefix
 *      parv[1] = TS_CURRENT for the server
 *      parv[2] = TS_MIN for the server
 *      parv[3] = server is standalone or connected to non-TS only
 *      parv[4] = server's idea of UTC time
 */
void hybrid_cmd_svinfo()
{
    send_cmd(NULL, "SVINFO 5 5 0 :%ld", (long int) time(NULL));
}

/* CAPAB */
/*
  QS     - Can handle quit storm removal
  EX     - Can do channel +e exemptions 
  CHW    - Can do channel wall @#
  LL     - Can do lazy links 
  IE     - Can do invite exceptions 
  EOB    - Can do EOB message
  KLN    - Can do KLINE message 
  GLN    - Can do GLINE message 
  HOPS   - can do half ops (+h)
  HUB    - This server is a HUB 
  AOPS   - Can do anon ops (+a) 
  UID    - Can do UIDs
  ZIP    - Can do ZIPlinks
  ENC    - Can do ENCrypted links 
  KNOCK  -  supports KNOCK 
  TBURST - supports TBURST
  PARA	 - supports invite broadcasting for +p
  ENCAP	 - ?
*/
void hybrid_cmd_capab()
{
    send_cmd(NULL,
             "CAPAB :QS EX CHW IE KLN GLN HOPS HUB AOPS KNOCK TBURST PARA");
}

/* PASS */
void hybrid_cmd_pass(char *pass)
{
    send_cmd(NULL, "PASS %s :TS", pass);
}

/* SERVER name hop descript */
void hybrid_cmd_server(char *servname, int hop, char *descript)
{
    send_cmd(NULL, "SERVER %s %d :%s", servname, hop, descript);
}
void hybrid_cmd_connect(int servernum)
{
    me_server =
        new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);

    if (servernum == 1)
        hybrid_cmd_pass(RemotePassword);
    else if (servernum == 2)
        hybrid_cmd_pass(RemotePassword2);
    else if (servernum == 3)
        hybrid_cmd_pass(RemotePassword3);

    hybrid_cmd_capab();
    hybrid_cmd_server(ServerName, 1, ServerDesc);
    hybrid_cmd_svinfo();
}

void hybrid_cmd_bob()
{
    /* Not used */
}

void hybrid_cmd_svsinfo()
{
    /* not used */
}



void hybrid_cmd_bot_nick(char *nick, char *user, char *host, char *real,
                         char *modes)
{
    EnforceQlinedNick(nick, s_BotServ);
    send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s :%s", nick,
             (long int) time(NULL), modes, user, host, ServerName, real);

}

void hybrid_cmd_part(char *nick, char *chan, char *buf)
{
    if (buf) {
        send_cmd(nick, "PART %s :%s", chan, buf);
    } else {
        send_cmd(nick, "PART %s", chan);
    }
}

int anope_event_ping(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    hybrid_cmd_pong(ac > 1 ? av[1] : ServerName, av[0]);
    return MOD_CONT;
}

int anope_event_away(char *source, int ac, char **av)
{
    if (!source) {
        return MOD_CONT;
    }
    m_away(source, (ac ? av[0] : NULL));
    return MOD_CONT;
}

int anope_event_kill(char *source, int ac, char **av)
{
    if (ac != 2)
        return MOD_CONT;

    m_kill(av[0], av[1]);
    return MOD_CONT;
}

int anope_event_kick(char *source, int ac, char **av)
{
    if (ac != 3)
        return MOD_CONT;
    do_kick(source, ac, av);
    return MOD_CONT;
}

int anope_event_eob(char *source, int ac, char **av)
{
    Server *s;
    s = findserver(servlist, source);
    /* If we found a server with the given source, that one just
     * finished bursting. If there was no source, then our uplink
     * server finished bursting. -GD
     */
    if (!s && serv_uplink)
        s = serv_uplink;
    finish_sync(s, 1);

    return MOD_CONT;
}

void hybrid_cmd_eob()
{
/*    send_cmd(ServerName, "EOB"); */
}


int anope_event_join(char *source, int ac, char **av)
{
    if (ac != 1)
        return MOD_CONT;
    do_join(source, ac, av);
    return MOD_CONT;
}

int anope_event_motd(char *source, int ac, char **av)
{
    if (!source) {
        return MOD_CONT;
    }

    m_motd(source);
    return MOD_CONT;
}

int anope_event_privmsg(char *source, int ac, char **av)
{
    if (ac != 2)
        return MOD_CONT;
    m_privmsg(source, av[0], av[1]);
    return MOD_CONT;
}

int anope_event_part(char *source, int ac, char **av)
{
    if (ac < 1 || ac > 2)
        return MOD_CONT;
    do_part(source, ac, av);
    return MOD_CONT;
}

int anope_event_whois(char *source, int ac, char **av)
{
    if (source && ac >= 1) {
        m_whois(source, av[0]);
    }
    return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(char *source, int ac, char **av)
{
    if (!stricmp(av[1], "1")) {
        uplink = sstrdup(av[0]);
    }
    do_server(source, av[0], av[1], av[2], NULL);
    return MOD_CONT;
}

int anope_event_squit(char *source, int ac, char **av)
{
    if (ac != 2)
        return MOD_CONT;
    do_squit(source, ac, av);
    return MOD_CONT;
}

int anope_event_quit(char *source, int ac, char **av)
{
    if (ac != 1)
        return MOD_CONT;
    do_quit(source, ac, av);
    return MOD_CONT;
}

void hybrid_cmd_372(char *source, char *msg)
{
    send_cmd(ServerName, "372 %s :- %s", source, msg);
}

void hybrid_cmd_372_error(char *source)
{
    send_cmd(ServerName, "422 %s :- MOTD file not found!  Please "
             "contact your IRC administrator.", source);
}

void hybrid_cmd_375(char *source)
{
    send_cmd(ServerName, "375 %s :- %s Message of the Day",
             source, ServerName);
}

void hybrid_cmd_376(char *source)
{
    send_cmd(ServerName, "376 %s :End of /MOTD command.", source);
}

/* 391 */
void hybrid_cmd_391(char *source, char *timestr)
{
    if (!timestr) {
        return;
    }
    send_cmd(NULL, "391 :%s %s :%s", source, ServerName, timestr);
}

/* 250 */
void hybrid_cmd_250(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "250 %s", buf);
}

/* 307 */
void hybrid_cmd_307(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "307 %s", buf);
}

/* 311 */
void hybrid_cmd_311(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "311 %s", buf);
}

/* 312 */
void hybrid_cmd_312(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "312 %s", buf);
}

/* 317 */
void hybrid_cmd_317(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "317 %s", buf);
}

/* 219 */
void hybrid_cmd_219(char *source, char *letter)
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
void hybrid_cmd_401(char *source, char *who)
{
    if (!source || !who) {
        return;
    }
    send_cmd(ServerName, "401 %s %s :No such service.", source, who);
}

/* 318 */
void hybrid_cmd_318(char *source, char *who)
{
    if (!source || !who) {
        return;
    }

    send_cmd(ServerName, "318 %s %s :End of /WHOIS list.", source, who);
}

/* 242 */
void hybrid_cmd_242(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "242 %s", buf);
}

/* 243 */
void hybrid_cmd_243(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "243 %s", buf);
}

/* 211 */
void hybrid_cmd_211(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "211 %s", buf);
}

void hybrid_cmd_mode(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "MODE %s %s", dest, buf);
}

void hybrid_cmd_nick(char *nick, char *name, char *mode)
{
    EnforceQlinedNick(nick, NULL);
    send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s :%s", nick,
             (long int) time(NULL), mode, ServiceUser, ServiceHost,
             ServerName, (name));
}

void hybrid_cmd_kick(char *source, char *chan, char *user, char *buf)
{
    if (buf) {
        send_cmd(source, "KICK %s %s :%s", chan, user, buf);
    } else {
        send_cmd(source, "KICK %s %s", chan, user);
    }
}

void hybrid_cmd_notice_ops(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "NOTICE @%s :%s", dest, buf);
}

void hybrid_cmd_bot_chan_mode(char *nick, char *chan)
{
    anope_cmd_mode(nick, chan, "%s %s", ircd->botchanumode, nick);
}

/* QUIT */
void hybrid_cmd_quit(char *source, char *buf)
{
    if (buf) {
        send_cmd(source, "QUIT :%s", buf);
    } else {
        send_cmd(source, "QUIT");
    }
}

/* PONG */
void hybrid_cmd_pong(char *servname, char *who)
{
    send_cmd(servname, "PONG %s", who);
}

/* INVITE */
void hybrid_cmd_invite(char *source, char *chan, char *nick)
{
    if (!source || !chan || !nick) {
        return;
    }

    send_cmd(source, "INVITE %s %s", nick, chan);
}

/* SQUIT */
void hybrid_cmd_squit(char *servname, char *message)
{
    if (!servname || !message) {
        return;
    }

    send_cmd(NULL, "SQUIT %s :%s", servname, message);
}

int anope_event_mode(char *source, int ac, char **av)
{
    if (ac < 2)
        return MOD_CONT;

    if (*av[0] == '#' || *av[0] == '&') {
        do_cmode(source, ac, av);
    } else {
        do_umode(source, ac, av);
    }
    return MOD_CONT;
}

void hybrid_cmd_351(char *source)
{
    send_cmd(ServerName, "351 %s Anope-%s %s :%s - %s (%s) -- %s",
             source, version_number, ServerName, ircd->name, version_flags,
             EncModule, version_build);
}

/* Event: PROTOCTL */
int anope_event_capab(char *source, int ac, char **av)
{
	int argvsize = 8;
	int argc;
	char **argv;
	char *str;
	
	if (ac < 1)
		return MOD_CONT;
	
	/* We get the params as one arg, we should split it for capab_parse */
	argv = scalloc(argvsize, sizeof(char *));
	argc = 0;
	while ((str = myStrGetToken(av[0], ' ', argc))) {
		if (argc == argvsize) {
			argvsize += 8;
			argv = srealloc(argv, argvsize * sizeof(char *));
		}
		argv[argc] = str;
		argc++;
	}
	
    capab_parse(argc, argv);
	
	/* Free our built ac/av */
	for (argvsize = 0; argvsize < argc; argvsize++) {
		free(argv[argvsize]);
	}
	free(argv);
	
    return MOD_CONT;
}

/* SVSHOLD - set */
void hybrid_cmd_svshold(char *nick)
{
    /* Not supported by this IRCD */
}

/* SVSHOLD - release */
void hybrid_cmd_release_svshold(char *nick)
{
    /* Not Supported by this IRCD */
}

/* SVSNICK */
void hybrid_cmd_svsnick(char *nick, char *newnick, time_t when)
{
	if (ircd->svsnick)
		send_cmd(NULL, "SVSNICK %s %s", nick, newnick);
}

void hybrid_cmd_guest_nick(char *nick, char *user, char *host, char *real,
                           char *modes)
{
    send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s :%s", nick,
             (long int) time(NULL), modes, user, host, ServerName, real);
}

void hybrid_cmd_svso(char *source, char *nick, char *flag)
{
    /* Not Supported by this IRCD */
}

void hybrid_cmd_unban(char *name, char *nick)
{
    /* Not Supported by this IRCD */
}

/* SVSMODE channel modes */

void hybrid_cmd_svsmode_chan(char *name, char *mode, char *nick)
{
    /* Not Supported by this IRCD */
}

/* SVSMODE +d */
/* sent if svid is something weird */
void hybrid_cmd_svid_umode(char *nick, time_t ts)
{
    send_cmd(ServerName, "SVSMODE %s +d 1", nick);
}

/* SVSMODE +d */
/* nc_change was = 1, and there is no na->status */
void hybrid_cmd_nc_change(User * u)
{
    /* not used */
}

/* SVSMODE +d */
void hybrid_cmd_svid_umode2(User * u, char *ts)
{
    /* not used */
}


void hybrid_cmd_svid_umode3(User * u, char *ts)
{
    /* not used */
}

/* NICK <newnick>  */
void hybrid_cmd_chg_nick(char *oldnick, char *newnick)
{
    if (!oldnick || !newnick) {
        return;
    }

    send_cmd(oldnick, "NICK %s", newnick);
}

/*
 * SVINFO
 *      parv[0] = sender prefix
 *      parv[1] = TS_CURRENT for the server
 *      parv[2] = TS_MIN for the server
 *      parv[3] = server is standalone or connected to non-TS only
 *      parv[4] = server's idea of UTC time
 */
int anope_event_svinfo(char *source, int ac, char **av)
{
    /* currently not used but removes the message : unknown message from server */
    return MOD_CONT;
}

int anope_event_pass(char *source, int ac, char **av)
{
    /* currently not used but removes the message : unknown message from server */
    return MOD_CONT;
}

void hybrid_cmd_svsjoin(char *source, char *nick, char *chan, char *param)
{
    /* Not Supported by this IRCD */
}

void hybrid_cmd_svspart(char *source, char *nick, char *chan)
{
    /* Not Supported by this IRCD */
}

void hybrid_cmd_swhois(char *source, char *who, char *mask)
{
    /* not supported */
}

int anope_event_notice(char *source, int ac, char **av)
{
    return hybrid_event_notice(source, ac, av);
}

int anope_event_admin(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_invite(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int hybrid_flood_mode_check(char *value)
{
    return 0;
}

int anope_event_error(char *source, int ac, char **av)
{
    if (ac >= 1) {
        if (debug) {
            alog("debug: %s", av[0]);
        }
    }
    return MOD_CONT;
}

void hybrid_cmd_jupe(char *jserver, char *who, char *reason)
{
    char rbuf[256];

    snprintf(rbuf, sizeof(rbuf), "Juped by %s%s%s", who,
             reason ? ": " : "", reason ? reason : "");

    if (findserver(servlist, jserver))
        hybrid_cmd_squit(jserver, rbuf);
    hybrid_cmd_server(jserver, 2, rbuf);
    new_server(me_server, jserver, rbuf, SERVER_JUPED, NULL);
}

/* 
  1 = valid nick
  0 = nick is in valid
*/
int hybrid_valid_nick(char *nick)
{
    /* no hard coded invalid nicks */
    return 1;
}

/* 
  1 = valid chan
  0 = chan is in valid
*/
int hybrid_valid_chan(char *chan)
{
    /* no hard coded invalid chans */
    return 1;
}


void hybrid_cmd_ctcp(char *source, char *dest, char *buf)
{
    char *s;

    if (!buf) {
        return;
    } else {
        s = normalizeBuffer(buf);
    }

    send_cmd(source, "NOTICE %s :\1%s \1", dest, s);
    free(s);
}

static int on_server_connect(int argc, char **argv)
{
	Server *s = findserver(servlist, argv[0]);
	if (s != NULL && s->uplink == me_server && !(s->flags & SERVER_JUPED))
	{
		/* Test these commands to see if they exist */
		send_cmd(s_ChanServ, "SVSNICK %s %s", s_ChanServ, s_ChanServ);
		send_cmd(s_ChanServ, "ENCAP * CHGHOST %s .", s_ChanServ);
	}
	
	return MOD_CONT;
}

int hybrid_event_notice(char *source, int argc, char **argv)
{
	if (argc > 1)
	{
		Server *s = findserver(servlist, source);
		if (s != NULL)
		{
			if (!ircd->svsnick && !strcmp(argv[0], s_ChanServ) && strstr(argv[1], "already in use"))
			{
				ircd->svsnick = 1;
				alog("SVSNICK support enabled");
			}
			else if (!ircd->vhost && !strcmp(argv[0], s_ChanServ) && !strcmp(argv[1], "Invalid hostname"))
			{
				ircd->vhost = ircd->vident = 1;
				ircd->hostservmode = ircd->hostservaliasmode = sstrdup("+o");
				alog("VHost support enabled");
				if (s_HostServ)
	        			anope_cmd_nick(s_HostServ, desc_HostServ, ircd->hostservmode);
				if (s_HostServAlias)
					anope_cmd_nick(s_HostServAlias, desc_HostServAlias, ircd->hostservaliasmode);
			}
		}
	}
	
	return MOD_CONT;
}


/**
 * Tell anope which function we want to perform each task inside of anope.
 * These prototypes must match what anope expects.
 **/
void moduleAddAnopeCmds()
{
    pmodule_cmd_svsnoop(hybrid_cmd_svsnoop);
    pmodule_cmd_remove_akill(hybrid_cmd_remove_akill);
    pmodule_cmd_topic(hybrid_cmd_topic);
    pmodule_cmd_vhost_off(hybrid_cmd_vhost_off);
    pmodule_cmd_akill(hybrid_cmd_akill);
    pmodule_cmd_svskill(hybrid_cmd_svskill);
    pmodule_cmd_svsmode(hybrid_cmd_svsmode);
    pmodule_cmd_372(hybrid_cmd_372);
    pmodule_cmd_372_error(hybrid_cmd_372_error);
    pmodule_cmd_375(hybrid_cmd_375);
    pmodule_cmd_376(hybrid_cmd_376);
    pmodule_cmd_nick(hybrid_cmd_nick);
    pmodule_cmd_guest_nick(hybrid_cmd_guest_nick);
    pmodule_cmd_mode(hybrid_cmd_mode);
    pmodule_cmd_bot_nick(hybrid_cmd_bot_nick);
    pmodule_cmd_kick(hybrid_cmd_kick);
    pmodule_cmd_notice_ops(hybrid_cmd_notice_ops);
    pmodule_cmd_notice(hybrid_cmd_notice);
    pmodule_cmd_notice2(hybrid_cmd_notice2);
    pmodule_cmd_privmsg(hybrid_cmd_privmsg);
    pmodule_cmd_privmsg2(hybrid_cmd_privmsg2);
    pmodule_cmd_serv_notice(hybrid_cmd_serv_notice);
    pmodule_cmd_serv_privmsg(hybrid_cmd_serv_privmsg);
    pmodule_cmd_bot_chan_mode(hybrid_cmd_bot_chan_mode);
    pmodule_cmd_351(hybrid_cmd_351);
    pmodule_cmd_quit(hybrid_cmd_quit);
    pmodule_cmd_pong(hybrid_cmd_pong);
    pmodule_cmd_join(hybrid_cmd_join);
    pmodule_cmd_unsqline(hybrid_cmd_unsqline);
    pmodule_cmd_invite(hybrid_cmd_invite);
    pmodule_cmd_part(hybrid_cmd_part);
    pmodule_cmd_391(hybrid_cmd_391);
    pmodule_cmd_250(hybrid_cmd_250);
    pmodule_cmd_307(hybrid_cmd_307);
    pmodule_cmd_311(hybrid_cmd_311);
    pmodule_cmd_312(hybrid_cmd_312);
    pmodule_cmd_317(hybrid_cmd_317);
    pmodule_cmd_219(hybrid_cmd_219);
    pmodule_cmd_401(hybrid_cmd_401);
    pmodule_cmd_318(hybrid_cmd_318);
    pmodule_cmd_242(hybrid_cmd_242);
    pmodule_cmd_243(hybrid_cmd_243);
    pmodule_cmd_211(hybrid_cmd_211);
    pmodule_cmd_global(hybrid_cmd_global);
    pmodule_cmd_global_legacy(hybrid_cmd_global_legacy);
    pmodule_cmd_sqline(hybrid_cmd_sqline);
    pmodule_cmd_squit(hybrid_cmd_squit);
    pmodule_cmd_svso(hybrid_cmd_svso);
    pmodule_cmd_chg_nick(hybrid_cmd_chg_nick);
    pmodule_cmd_svsnick(hybrid_cmd_svsnick);
    pmodule_cmd_vhost_on(hybrid_cmd_vhost_on);
    pmodule_cmd_connect(hybrid_cmd_connect);
    pmodule_cmd_bob(hybrid_cmd_bob);
    pmodule_cmd_svshold(hybrid_cmd_svshold);
    pmodule_cmd_release_svshold(hybrid_cmd_release_svshold);
    pmodule_cmd_unsgline(hybrid_cmd_unsgline);
    pmodule_cmd_unszline(hybrid_cmd_unszline);
    pmodule_cmd_szline(hybrid_cmd_szline);
    pmodule_cmd_sgline(hybrid_cmd_sgline);
    pmodule_cmd_unban(hybrid_cmd_unban);
    pmodule_cmd_svsmode_chan(hybrid_cmd_svsmode_chan);
    pmodule_cmd_svid_umode(hybrid_cmd_svid_umode);
    pmodule_cmd_nc_change(hybrid_cmd_nc_change);
    pmodule_cmd_svid_umode2(hybrid_cmd_svid_umode2);
    pmodule_cmd_svid_umode3(hybrid_cmd_svid_umode3);
    pmodule_cmd_svsjoin(hybrid_cmd_svsjoin);
    pmodule_cmd_svspart(hybrid_cmd_svspart);
    pmodule_cmd_swhois(hybrid_cmd_swhois);
    pmodule_cmd_eob(hybrid_cmd_eob);
    pmodule_flood_mode_check(hybrid_flood_mode_check);
    pmodule_cmd_jupe(hybrid_cmd_jupe);
    pmodule_valid_nick(hybrid_valid_nick);
    pmodule_valid_chan(hybrid_valid_chan);
    pmodule_cmd_ctcp(hybrid_cmd_ctcp);
    pmodule_set_umode(hybrid_set_umode);
}

/** 
 * Now tell anope how to use us.
 **/
int AnopeInit(int argc, char **argv)
{
    EvtHook *hook;

    moduleAddAuthor("Anope");
    moduleAddVersion(VERSION_STRING);
    moduleSetType(PROTOCOL);

    pmodule_ircd_version("Hybrid IRCd 7.0");
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
    pmodule_permchan_mode(0);

    moduleAddAnopeCmds();
    moduleAddIRCDMsgs();

    hook = createEventHook(EVENT_SERVER_CONNECT, on_server_connect);
    moduleAddEventHook(hook);

    return MOD_CONT;
}
