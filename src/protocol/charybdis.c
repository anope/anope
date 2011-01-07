/* Charybdis IRCD functions
 *
 * (C) 2006 William Pitcock <nenolod -at- charybdis.be>
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 */

#include "services.h"
#include "pseudo.h"
#include "charybdis.h"
#include "version.h"

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
     "-cijlmnpstrgzQF",         /* Modes to Remove */
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
     0,                         /* +j */
     0,                         /* +j Mode */
     0,                         /* Use delayed client introduction. */
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

void charybdis_set_umode(User * user, int ac, char **av)
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



void charybdis_cmd_notice(char *source, char *dest, char *buf)
{
    Uid *ud;
    User *u;

    if (!buf) {
        return;
    }

    if (NSDefFlags & NI_MSG) {
        charybdis_cmd_privmsg2(source, dest, buf);
    } else {
        ud = find_uid(source);
        u = finduser(dest);
        send_cmd((UseTS6 ? (ud ? ud->uid : source) : source),
                 "NOTICE %s :%s", (UseTS6 ? (u ? u->uid : dest) : dest),
                 buf);
    }
}

void charybdis_cmd_notice2(char *source, char *dest, char *msg)
{
    Uid *ud;
    User *u;

    ud = find_uid(source);
    u = finduser(dest);
    send_cmd((UseTS6 ? (ud ? ud->uid : source) : source), "NOTICE %s :%s",
             (UseTS6 ? (u ? u->uid : dest) : dest), msg);
}

void charybdis_cmd_privmsg(char *source, char *dest, char *buf)
{
    Uid *ud, *ud2;

    if (!buf) {
        return;
    }
    ud = find_uid(source);
    ud2 = find_uid(dest);

    send_cmd((UseTS6 ? (ud ? ud->uid : source) : source), "PRIVMSG %s :%s",
             (UseTS6 ? (ud2 ? ud2->uid : dest) : dest), buf);
}

void charybdis_cmd_privmsg2(char *source, char *dest, char *msg)
{
    Uid *ud, *ud2;

    ud = find_uid(source);
    ud2 = find_uid(dest);

    send_cmd((UseTS6 ? (ud ? ud->uid : source) : source), "PRIVMSG %s :%s",
             (UseTS6 ? (ud2 ? ud2->uid : dest) : dest), msg);
}

void charybdis_cmd_serv_notice(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE $$%s :%s", dest, msg);
}

void charybdis_cmd_serv_privmsg(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG $$%s :%s", dest, msg);
}


void charybdis_cmd_global(char *source, char *buf)
{
    Uid *u;

    if (!buf) {
        return;
    }

    if (source) {
        u = find_uid(source);
        if (u) {
            send_cmd((UseTS6 ? u->uid : source), "OPERWALL :%s", buf);
        } else {
            send_cmd((UseTS6 ? TS6SID : ServerName), "WALLOPS :%s", buf);
        }
    } else {
        send_cmd((UseTS6 ? TS6SID : ServerName), "WALLOPS :%s", buf);
    }
}

/* GLOBOPS - to handle old WALLOPS */
void charybdis_cmd_global_legacy(char *source, char *fmt)
{
    Uid *u;

    if (source) {
        u = find_uid(source);
        if (u) {
            send_cmd((UseTS6 ? u->uid : source), "OPERWALL :%s", fmt);
        } else {
            send_cmd((UseTS6 ? TS6SID : ServerName), "WALLOPS :%s", fmt);
        }
    } else {
        send_cmd((UseTS6 ? TS6SID : ServerName), "WALLOPS :%s", fmt);
    }

    send_cmd(source ? source : ServerName, "OPERWALL :%s", fmt);
}

int anope_event_sjoin(char *source, int ac, char **av)
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
int anope_event_nick(char *source, int ac, char **av)
{
    Server *s;
    User *user;

    if (UseTS6 && ac == 9) {
        s = findserver_uid(servlist, source);
        /* Source is always the server */
        *source = '\0';
        user = do_nick(source, av[0], av[4], av[5], s->name, av[8],
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
int anope_event_euid(char *source, int ac, char **av)
{
    Server *s;
    User *user;
    time_t ts;

    if (UseTS6 && ac == 11) {
        s = findserver_uid(servlist, source);
        /* Source is always the server */
        *source = '\0';
	ts = strtoul(av[2], NULL, 10);
        user = do_nick(source, av[0], av[4], !strcmp(av[8], "*") ? av[5] : av[8], s->name, av[10],
                       ts, !stricmp(av[0], av[9]) ? ts : 0, 0, av[5], av[7]);
        if (user) {
            anope_set_umode(user, 1, &av[3]);
        }
    }
    return MOD_CONT;
}

int anope_event_topic(char *source, int ac, char **av)
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

int anope_event_tburst(char *source, int ac, char **av)
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

int anope_event_436(char *source, int ac, char **av)
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


void charybdis_cmd_sqline(char *mask, char *reason)
{
    Uid *ud;

    ud = find_uid(s_OperServ);
    send_cmd((UseTS6 ? (ud ? ud->uid : s_OperServ) : s_OperServ),
             "RESV * %s :%s", mask, reason);
}

void charybdis_cmd_unsgline(char *mask)
{
    Uid *ud;

    ud = find_uid(s_OperServ);
    send_cmd((UseTS6 ? (ud ? ud->uid : s_OperServ) : s_OperServ),
             "UNXLINE * %s", mask);
}

void charybdis_cmd_unszline(char *mask)
{
    /* not supported */
}

void charybdis_cmd_szline(char *mask, char *reason, char *whom)
{
    /* not supported */
}

void charybdis_cmd_svsnoop(char *server, int set)
{
    /* does not support */
}

void charybdis_cmd_svsadmin(char *server, int set)
{
    charybdis_cmd_svsnoop(server, set);
}

void charybdis_cmd_sgline(char *mask, char *reason)
{
    Uid *ud;

    ud = find_uid(s_OperServ);
    send_cmd((UseTS6 ? (ud ? ud->uid : s_OperServ) : s_OperServ),
             "XLINE * %s 0 :%s", mask, reason);
}

void charybdis_cmd_remove_akill(char *user, char *host)
{
    Uid *ud;

    ud = find_uid(s_OperServ);
    send_cmd((UseTS6 ? (ud ? ud->uid : s_OperServ) : s_OperServ),
             "UNKLINE * %s %s", user, host);
}

void charybdis_cmd_topic(char *whosets, char *chan, char *whosetit,
                      char *topic, time_t when)
{
    Uid *ud;

    ud = find_uid(whosets);
    send_cmd((UseTS6 ? (ud ? ud->uid : whosets) : whosets), "TOPIC %s :%s",
             chan, topic);
}

void charybdis_cmd_vhost_off(User * u)
{
    send_cmd((UseTS6 ? TS6SID : ServerName), "ENCAP * CHGHOST %s :%s",
             u->nick, u->host);
}

void charybdis_cmd_vhost_on(char *nick, char *vIdent, char *vhost)
{
    send_cmd((UseTS6 ? TS6SID : ServerName), "ENCAP * CHGHOST %s :%s",
             nick, vhost);
}

void charybdis_cmd_unsqline(char *user)
{
    Uid *ud;

    ud = find_uid(s_OperServ);
    send_cmd((UseTS6 ? (ud ? ud->uid : s_OperServ) : s_OperServ),
             "UNRESV * %s", user);
}

void charybdis_cmd_join(char *user, char *channel, time_t chantime)
{
    Uid *ud;

    ud = find_uid(user);
    send_cmd(NULL, "SJOIN %ld %s + :%s", (long int) chantime,
             channel, (UseTS6 ? (ud ? ud->uid : user) : user));
}

/*
oper:		the nick of the oper performing the kline
target.server:	the server(s) this kline is destined for
duration:	the duration if a tkline, 0 if permanent.
user:		the 'user' portion of the kline
host:		the 'host' portion of the kline
reason:		the reason for the kline.
*/

void charybdis_cmd_akill(char *user, char *host, char *who, time_t when,
                      time_t expires, char *reason)
{
    Uid *ud;

    ud = find_uid(s_OperServ);

    send_cmd((UseTS6 ? (ud ? ud->uid : s_OperServ) : s_OperServ),
             "KLINE * %ld %s %s :%s",
             (long int) (expires - (long) time(NULL)), user, host, reason);
}

void charybdis_cmd_svskill(char *source, char *user, char *buf)
{
    Uid *ud, *ud2;

    if (!buf) {
        return;
    }

    if (!source || !user) {
        return;
    }

    ud = find_uid(source);
    ud2 = find_uid(user);
    send_cmd((UseTS6 ? (ud ? ud->uid : source) : source), "KILL %s :%s",
             (UseTS6 ? (ud2 ? ud2->uid : user) : user), buf);
}

void charybdis_cmd_svsmode(User * u, int ac, char **av)
{
    send_cmd((UseTS6 ? TS6SID : ServerName), "SVSMODE %s %s", u->nick,
             av[0]);
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
void charybdis_cmd_pass(char *pass)
{
    if (UseTS6) {
        send_cmd(NULL, "PASS %s TS 6 :%s", pass, TS6SID);
    } else {
        send_cmd(NULL, "PASS %s :TS", pass);
    }
}

/* SERVER name hop descript */
void charybdis_cmd_server(char *servname, int hop, char *descript)
{
    send_cmd(NULL, "SERVER %s %d :%s", servname, hop, descript);
}

void charybdis_cmd_connect(int servernum)
{
    /* Make myself known to myself in the serverlist */
    if (UseTS6) {
        me_server =
            new_server(NULL, ServerName, ServerDesc, SERVER_ISME, TS6SID);
    } else {
        me_server =
            new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);
    }
    if (servernum == 1)
        charybdis_cmd_pass(RemotePassword);
    else if (servernum == 2)
        charybdis_cmd_pass(RemotePassword2);
    else if (servernum == 3)
        charybdis_cmd_pass(RemotePassword3);

    charybdis_cmd_capab();
    charybdis_cmd_server(ServerName, 1, ServerDesc);
    charybdis_cmd_svinfo();
}

void charybdis_cmd_bob()
{
    /* Not used */
}

void charybdis_cmd_bot_nick(char *nick, char *user, char *host, char *real,
                         char *modes)
{
    EnforceQlinedNick(nick, NULL);
    if (UseTS6) {
        char *uidbuf = ts6_uid_retrieve();
        send_cmd(TS6SID, "UID %s 1 %ld %s %s %s 0 %s :%s", nick,
                 (long int) time(NULL), modes, user, host, uidbuf,
                 real);
        new_uid(nick, uidbuf);
    } else {
        send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s :%s", nick,
                 (long int) time(NULL), modes, user, host, ServerName,
                 real);
    }
    charybdis_cmd_sqline(nick, "Reserved for services");
}

void charybdis_cmd_part(char *nick, char *chan, char *buf)
{
    Uid *ud;

    ud = find_uid(nick);

    if (buf) {
        send_cmd((UseTS6 ? ud->uid : nick), "PART %s :%s", chan, buf);
    } else {
        send_cmd((UseTS6 ? ud->uid : nick), "PART %s", chan);
    }
}

int anope_event_ping(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    charybdis_cmd_pong(ac > 1 ? av[1] : ServerName, av[0]);
    return MOD_CONT;
}

int anope_event_away(char *source, int ac, char **av)
{
    User *u = NULL;

    if (UseTS6) {
        u = find_byuid(source);
    }

    m_away((UseTS6 ? (u ? u->nick : source) : source),
           (ac ? av[0] : NULL));
    return MOD_CONT;
}

int anope_event_kill(char *source, int ac, char **av)
{
    User *u = NULL;
    Uid *ud = NULL;

    if (ac != 2)
        return MOD_CONT;

    if (UseTS6) {
        u = find_byuid(av[0]);
        ud = find_nickuid(av[0]);
    }

    m_kill(u ? u->nick : (ud ? ud->nick : av[0]), av[1]);
    return MOD_CONT;
}

int anope_event_kick(char *source, int ac, char **av)
{
    if (ac != 3)
        return MOD_CONT;
    do_kick(source, ac, av);
    return MOD_CONT;
}

void charybdis_cmd_eob()
{
    /* doesn't support EOB */
}

int anope_event_join(char *source, int ac, char **av)
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

int anope_event_part(char *source, int ac, char **av)
{
    User *u;

    if (ac < 1 || ac > 2) {
        return MOD_CONT;
    }

    u = find_byuid(source);
    do_part((UseTS6 ? (u ? u->nick : source) : source), ac, av);

    return MOD_CONT;
}

int anope_event_whois(char *source, int ac, char **av)
{
    Uid *ud;

    if (source && ac >= 1) {
        ud = find_nickuid(av[0]);
        m_whois(source, (UseTS6 ? (ud ? ud->nick : av[0]) : av[0]));
    }
    return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(char *source, int ac, char **av)
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

int anope_event_sid(char *source, int ac, char **av)
{
    Server *s;

    /* :42X SID trystan.nomadirc.net 2 43X :ircd-charybdis test server */

    s = findserver_uid(servlist, source);

    do_server(s->name, av[0], av[1], av[3], av[2]);
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
    User *u;

    if (ac != 1) {
        return MOD_CONT;
    }

    u = find_byuid(source);

    do_quit((UseTS6 ? (u ? u->nick : source) : source), ac, av);
    return MOD_CONT;
}

void charybdis_cmd_372(char *source, char *msg)
{
    send_cmd((UseTS6 ? TS6SID : ServerName), "372 %s :- %s", source, msg);
}

void charybdis_cmd_372_error(char *source)
{
    send_cmd((UseTS6 ? TS6SID : ServerName),
             "422 %s :- MOTD file not found!  Please "
             "contact your IRC administrator.", source);
}

void charybdis_cmd_375(char *source)
{
    send_cmd((UseTS6 ? TS6SID : ServerName),
             "375 %s :- %s Message of the Day", source, ServerName);
}

void charybdis_cmd_376(char *source)
{
    send_cmd((UseTS6 ? TS6SID : ServerName),
             "376 %s :End of /MOTD command.", source);
}

/* 391 */
void charybdis_cmd_391(char *source, char *timestr)
{
    if (!timestr) {
        return;
    }
    send_cmd(NULL, "391 :%s %s :%s", source, ServerName, timestr);
}

/* 250 */
void charybdis_cmd_250(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "250 %s", buf);
}

/* 307 */
void charybdis_cmd_307(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd((UseTS6 ? TS6SID : ServerName), "307 %s", buf);
}

/* 311 */
void charybdis_cmd_311(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd((UseTS6 ? TS6SID : ServerName), "311 %s", buf);
}

/* 312 */
void charybdis_cmd_312(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd((UseTS6 ? TS6SID : ServerName), "312 %s", buf);
}

/* 317 */
void charybdis_cmd_317(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd((UseTS6 ? TS6SID : ServerName), "317 %s", buf);
}

/* 219 */
void charybdis_cmd_219(char *source, char *letter)
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
void charybdis_cmd_401(char *source, char *who)
{
    if (!source || !who) {
        return;
    }
    send_cmd((UseTS6 ? TS6SID : ServerName), "401 %s %s :No such service.",
             source, who);
}

/* 318 */
void charybdis_cmd_318(char *source, char *who)
{
    if (!source || !who) {
        return;
    }

    send_cmd((UseTS6 ? TS6SID : ServerName),
             "318 %s %s :End of /WHOIS list.", source, who);
}

/* 242 */
void charybdis_cmd_242(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "242 %s", buf);
}

/* 243 */
void charybdis_cmd_243(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "243 %s", buf);
}

/* 211 */
void charybdis_cmd_211(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "211 %s", buf);
}

void charybdis_cmd_mode(char *source, char *dest, char *buf)
{
    Uid *ud;
    if (!buf) {
        return;
    }

    if (source) {
        ud = find_uid(source);
        send_cmd((UseTS6 ? (ud ? ud->uid : source) : source), "MODE %s %s",
                 dest, buf);
    } else {
        send_cmd(NULL, "MODE %s %s", dest, buf);
    }
}

void charybdis_cmd_tmode(char *source, char *dest, const char *fmt, ...)
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

void charybdis_cmd_nick(char *nick, char *name, char *mode)
{
    EnforceQlinedNick(nick, NULL);
    if (UseTS6) {
        char *uidbuf = ts6_uid_retrieve();
        send_cmd(TS6SID, "UID %s 1 %ld %s %s %s 0 %s :%s", nick,
                 (long int) time(NULL), mode, ServiceUser, ServiceHost,
                 uidbuf, name);
        new_uid(nick, uidbuf);
    } else {
        send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s :%s", nick,
                 (long int) time(NULL), mode, ServiceUser, ServiceHost,
                 ServerName, name);
    }
    charybdis_cmd_sqline(nick, "Reserved for services");
}

void charybdis_cmd_kick(char *source, char *chan, char *user, char *buf)
{
    Uid *ud;
    User *u;

    ud = find_uid(source);
    u = finduser(user);

    if (buf) {
        send_cmd((UseTS6 ? (ud ? ud->uid : source) : source),
                 "KICK %s %s :%s", chan,
                 (UseTS6 ? (u ? u->uid : user) : user), buf);
    } else {
        send_cmd((UseTS6 ? (ud ? ud->uid : source) : source), "KICK %s %s",
                 chan, (UseTS6 ? (u ? u->uid : user) : user));
    }
}

void charybdis_cmd_notice_ops(char *source, char *dest, char *buf)
{
    Uid *ud;
    ud = find_uid(source);

    if (!buf) {
        return;
    }

    send_cmd((UseTS6 ? (ud ? ud->uid : source) : source), "NOTICE @%s :%s", dest, buf);
}

void charybdis_cmd_bot_chan_mode(char *nick, char *chan)
{
    Uid *u;

    if (UseTS6) {
        u = find_uid(nick);
        charybdis_cmd_tmode(nick, chan, "%s %s", ircd->botchanumode,
                         (u ? u->uid : nick));
    } else {
        anope_cmd_mode(ServerName, chan, "%s %s", ircd->botchanumode, nick);
    }
}

/* QUIT */
void charybdis_cmd_quit(char *source, char *buf)
{
    Uid *ud;
    ud = find_uid(source);

    if (buf) {
        send_cmd((UseTS6 ? (ud ? ud->uid : source) : source), "QUIT :%s",
                 buf);
    } else {
        send_cmd((UseTS6 ? (ud ? ud->uid : source) : source), "QUIT");
    }
}

/* PONG */
void charybdis_cmd_pong(char *servname, char *who)
{
    if (UseTS6) {
        /* deliberately no SID in the first parameter -- jilles */
        send_cmd(TS6SID, "PONG %s :%s", servname, who);
    } else {
        send_cmd(servname, "PONG %s :%s", servname, who);
    }
}

/* INVITE */
void charybdis_cmd_invite(char *source, char *chan, char *nick)
{
    Uid *ud;
    User *u;

    if (!source || !chan || !nick) {
        return;
    }

    ud = find_uid(source);
    u = finduser(nick);

    send_cmd((UseTS6 ? (ud ? ud->uid : source) : source), "INVITE %s %s",
             (UseTS6 ? (u ? u->uid : nick) : nick), chan);
}

/* SQUIT */
void charybdis_cmd_squit(char *servname, char *message)
{
    if (!servname || !message) {
        return;
    }

    send_cmd(NULL, "SQUIT %s :%s", servname, message);
}

int anope_event_mode(char *source, int ac, char **av)
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

int anope_event_tmode(char *source, int ac, char **av)
{
    if (ac > 2 && (*av[1] == '#' || *av[1] == '&')) {
        do_cmode(source, ac, av);
    }
    return MOD_CONT;
}

void charybdis_cmd_351(char *source)
{
    send_cmd((UseTS6 ? TS6SID : ServerName),
             "351 %s Anope-%s %s :%s - %s (%s) -- %s", source, version_number,
             ServerName, ircd->name, version_flags, EncModule, version_build);
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
void charybdis_cmd_svshold(char *nick)
{
    send_cmd(NULL, "ENCAP * NICKDELAY 300 %s", nick);
}

/* SVSHOLD - release */
void charybdis_cmd_release_svshold(char *nick)
{
    send_cmd(NULL, "ENCAP * NICKDELAY 0 %s", nick);
}

/* SVSNICK */
void charybdis_cmd_svsnick(char *nick, char *newnick, time_t when)
{
    User *u;

    if (!nick || !newnick) {
        return;
    }

    u = finduser(nick);
    if (!u)
        return;
    send_cmd(NULL, "ENCAP %s RSFNC %s %s %ld %ld", u->server->name,
             u->nick, newnick, (long int)when, (long int)u->timestamp);
}

void charybdis_cmd_guest_nick(char *nick, char *user, char *host, char *real,
                           char *modes)
{
    /* not supported  */
}

void charybdis_cmd_svso(char *source, char *nick, char *flag)
{
    /* Not Supported by this IRCD */
}

void charybdis_cmd_unban(char *name, char *nick)
{
    /* Not Supported by this IRCD */
}

/* SVSMODE channel modes */

void charybdis_cmd_svsmode_chan(char *name, char *mode, char *nick)
{
    /* Not Supported by this IRCD */
}

/* SVSMODE +d */
/* sent if svid is something weird */
void charybdis_cmd_svid_umode(char *nick, time_t ts)
{
    /* not supported */
}

/* SVSMODE +d */
/* nc_change was = 1, and there is no na->status */
void charybdis_cmd_nc_change(User * u)
{
    /* not supported */
}

/* SVSMODE +d */
void charybdis_cmd_svid_umode2(User * u, char *ts)
{
    /* not supported */
}

void charybdis_cmd_svid_umode3(User * u, char *ts)
{
    /* not used */
}

/* NICK <newnick>  */
void charybdis_cmd_chg_nick(char *oldnick, char *newnick)
{
    Uid *ud;

    if (!oldnick || !newnick) {
        return;
    }

    ud = find_uid(oldnick);
    if (ud)
        strscpy(ud->nick, newnick, NICKMAX);

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
    if (UseTS6) {
        TS6UPLINK = sstrdup(av[3]);
    }
    return MOD_CONT;
}

void charybdis_cmd_svsjoin(char *source, char *nick, char *chan, char *param)
{
    /* Not Supported by this IRCD */
}

void charybdis_cmd_svspart(char *source, char *nick, char *chan)
{
    /* Not Supported by this IRCD */
}

void charybdis_cmd_swhois(char *source, char *who, char *mask)
{
    /* not supported */
}

int anope_event_notice(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_admin(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_invite(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_bmask(char *source, int ac, char **av)
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

int charybdis_flood_mode_check(char *value)
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

int anope_event_error(char *source, int ac, char **av)
{
    if (ac >= 1) {
        if (debug) {
            alog("debug: %s", av[0]);
        }
    }
    return MOD_CONT;
}

void charybdis_cmd_jupe(char *jserver, char *who, char *reason)
{
    char rbuf[256];

    snprintf(rbuf, sizeof(rbuf), "Juped by %s%s%s", who,
             reason ? ": " : "", reason ? reason : "");

    if (findserver(servlist, jserver))
        charybdis_cmd_squit(jserver, rbuf);
    charybdis_cmd_server(jserver, 2, rbuf);
    new_server(me_server, jserver, rbuf, SERVER_JUPED, NULL);
}

/* 
  1 = valid nick
  0 = nick is in valid
*/
int charybdis_valid_nick(char *nick)
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
int charybdis_valid_chan(char *chan)
{
    /* no hard coded invalid chan */
    return 1;
}


void charybdis_cmd_ctcp(char *source, char *dest, char *buf)
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
    pmodule_cmd_svsnoop(charybdis_cmd_svsnoop);
    pmodule_cmd_remove_akill(charybdis_cmd_remove_akill);
    pmodule_cmd_topic(charybdis_cmd_topic);
    pmodule_cmd_vhost_off(charybdis_cmd_vhost_off);
    pmodule_cmd_akill(charybdis_cmd_akill);
    pmodule_cmd_svskill(charybdis_cmd_svskill);
    pmodule_cmd_svsmode(charybdis_cmd_svsmode);
    pmodule_cmd_372(charybdis_cmd_372);
    pmodule_cmd_372_error(charybdis_cmd_372_error);
    pmodule_cmd_375(charybdis_cmd_375);
    pmodule_cmd_376(charybdis_cmd_376);
    pmodule_cmd_nick(charybdis_cmd_nick);
    pmodule_cmd_guest_nick(charybdis_cmd_guest_nick);
    pmodule_cmd_mode(charybdis_cmd_mode);
    pmodule_cmd_bot_nick(charybdis_cmd_bot_nick);
    pmodule_cmd_kick(charybdis_cmd_kick);
    pmodule_cmd_notice_ops(charybdis_cmd_notice_ops);
    pmodule_cmd_notice(charybdis_cmd_notice);
    pmodule_cmd_notice2(charybdis_cmd_notice2);
    pmodule_cmd_privmsg(charybdis_cmd_privmsg);
    pmodule_cmd_privmsg2(charybdis_cmd_privmsg2);
    pmodule_cmd_serv_notice(charybdis_cmd_serv_notice);
    pmodule_cmd_serv_privmsg(charybdis_cmd_serv_privmsg);
    pmodule_cmd_bot_chan_mode(charybdis_cmd_bot_chan_mode);
    pmodule_cmd_351(charybdis_cmd_351);
    pmodule_cmd_quit(charybdis_cmd_quit);
    pmodule_cmd_pong(charybdis_cmd_pong);
    pmodule_cmd_join(charybdis_cmd_join);
    pmodule_cmd_unsqline(charybdis_cmd_unsqline);
    pmodule_cmd_invite(charybdis_cmd_invite);
    pmodule_cmd_part(charybdis_cmd_part);
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
    pmodule_cmd_global(charybdis_cmd_global);
    pmodule_cmd_global_legacy(charybdis_cmd_global_legacy);
    pmodule_cmd_sqline(charybdis_cmd_sqline);
    pmodule_cmd_squit(charybdis_cmd_squit);
    pmodule_cmd_svso(charybdis_cmd_svso);
    pmodule_cmd_chg_nick(charybdis_cmd_chg_nick);
    pmodule_cmd_svsnick(charybdis_cmd_svsnick);
    pmodule_cmd_vhost_on(charybdis_cmd_vhost_on);
    pmodule_cmd_connect(charybdis_cmd_connect);
    pmodule_cmd_bob(charybdis_cmd_bob);
    pmodule_cmd_svshold(charybdis_cmd_svshold);
    pmodule_cmd_release_svshold(charybdis_cmd_release_svshold);
    pmodule_cmd_unsgline(charybdis_cmd_unsgline);
    pmodule_cmd_unszline(charybdis_cmd_unszline);
    pmodule_cmd_szline(charybdis_cmd_szline);
    pmodule_cmd_sgline(charybdis_cmd_sgline);
    pmodule_cmd_unban(charybdis_cmd_unban);
    pmodule_cmd_svsmode_chan(charybdis_cmd_svsmode_chan);
    pmodule_cmd_svid_umode(charybdis_cmd_svid_umode);
    pmodule_cmd_nc_change(charybdis_cmd_nc_change);
    pmodule_cmd_svid_umode2(charybdis_cmd_svid_umode2);
    pmodule_cmd_svid_umode3(charybdis_cmd_svid_umode3);
    pmodule_cmd_svsjoin(charybdis_cmd_svsjoin);
    pmodule_cmd_svspart(charybdis_cmd_svspart);
    pmodule_cmd_swhois(charybdis_cmd_swhois);
    pmodule_cmd_eob(charybdis_cmd_eob);
    pmodule_flood_mode_check(charybdis_flood_mode_check);
    pmodule_cmd_jupe(charybdis_cmd_jupe);
    pmodule_valid_nick(charybdis_valid_nick);
    pmodule_valid_chan(charybdis_valid_chan);
    pmodule_cmd_ctcp(charybdis_cmd_ctcp);
    pmodule_set_umode(charybdis_set_umode);
}

/** 
 * Now tell anope how to use us.
 **/
int AnopeInit(int argc, char **argv)
{
    EvtHook *hk;

    moduleAddAuthor("Anope");
    moduleAddVersion(VERSION_STRING);
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
    pmodule_permchan_mode(0);

    moduleAddAnopeCmds();
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
