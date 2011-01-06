/* Rage IRCD functions
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
#include "rageircd.h"
#include "version.h"

IRCDVar myIrcd[] = {
    {"RageIRCd 2.0.*",          /* ircd name */
     "+d",                      /* nickserv mode */
     "+d",                      /* chanserv mode */
     "+d",                      /* memoserv mode */
     "+d",                      /* hostserv mode */
     "+di",                     /* operserv mode */
     "+d",                      /* botserv mode  */
     "+dh",                     /* helpserv mode */
     "+di",                     /* Dev/Null mode */
     "+di",                     /* Global mode   */
     "+o",                      /* nickserv alias mode */
     "+o",                      /* chanserv alias mode */
     "+o",                      /* memoserv alias mode */
     "+io",                     /* hostserv alias mode */
     "+io",                     /* operserv alias mode */
     "+o",                      /* botserv alias mode  */
     "+o",                      /* helpserv alias mode */
     "+i",                      /* Dev/Null alias mode */
     "+io",                     /* Global alias mode   */
     "+",                       /* Used by BotServ Bots */
     3,                         /* Chan Max Symbols     */
     "-ilmnpRstcOACNM",         /* Modes to Remove */
     "+o",                      /* Channel Umode used by Botserv bots */
     1,                         /* SVSNICK */
     1,                         /* Vhost  */
     0,                         /* Has Owner */
     NULL,                      /* Mode to set for an owner */
     NULL,                      /* Mode to unset for an owner */
     "+a",                      /* Mode to set for channel admin */
     "-a",                      /* Mode to unset for channel admin */
     "+rd",                     /* Mode On Reg          */
     NULL,                      /* Mode on ID for Roots */
     NULL,                      /* Mode on ID for Admins */
     NULL,                      /* Mode on ID for Opers */
     "-rd",                     /* Mode on UnReg        */
     "-r+d",                    /* Mode on Nick Change  */
     1,                         /* Supports SGlines     */
     1,                         /* Supports SQlines     */
     0,                         /* Supports SZlines     */
     1,                         /* Supports Halfop +h   */
     3,                         /* Number of server args */
     0,                         /* Join 2 Set           */
     0,                         /* Join 2 Message       */
     1,                         /* Has exceptions +e    */
     0,                         /* TS Topic Forward     */
     0,                         /* TS Topic Backward    */
     0,                         /* Protected Umode      */
     1,                         /* Has Admin            */
     1,                         /* Chan SQlines         */
     1,                         /* Quit on Kill         */
     0,                         /* SVSMODE unban        */
     0,                         /* Has Protect          */
     0,                         /* Reverse              */
     1,                         /* Chan Reg             */
     CMODE_r,                   /* Channel Mode         */
     0,                         /* vidents              */
     1,                         /* svshold              */
     1,                         /* time stamp on mode   */
     1,                         /* NICKIP               */
     0,                         /* O:LINE               */
     1,                         /* UMODE               */
     1,                         /* VHOST ON NICK        */
     0,                         /* Change RealName      */
     CMODE_p,                   /* No Knock             */
     CMODE_A,                   /* Admin Only           */
     DEFAULT_MLOCK,             /* Default MLOCK        */
     UMODE_x,                   /* Vhost Mode           */
     0,                         /* +f                   */
     0,                         /* +L                   */
     0,
     0,
     1,
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
     "x",                       /* vhost char */
     0,                         /* ts6 */
     1,                         /* support helper umode */
     0,                         /* p10 */
     NULL,                      /* character set */
     1,                         /* reports sync state */
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
     CAPAB_TSMODE,              /* TSMODE       */
     CAPAB_UNCONNECT,           /* UNCONNECT    */
     0,                         /* NICKIP       */
     0,                         /* SJOIN        */
     CAPAB_ZIP,                 /* ZIP          */
     CAPAB_BURST,               /* BURST        */
     0,                         /* TS5          */
     0,                         /* TS3          */
     CAPAB_DKEY,                /* DKEY         */
     0,                         /* PT4          */
     0,                         /* SCS          */
     0,                         /* QS           */
     CAPAB_UID,                 /* UID          */
     0,                         /* KNOCK        */
     0,                         /* CLIENT       */
     0,                         /* IPV6         */
     0,                         /* SSJ5         */
     CAPAB_SN2,                 /* SN2          */
     CAPAB_TOKEN,               /* TOKEN        */
     CAPAB_VHOST,               /* VHOST        */
     CAPAB_SSJ3,                /* SSJ3         */
     0,                         /* NICK2        */
     0,                         /* UMODE2       */
     0,                         /* VL           */
     0,                         /* TLKEXT       */
     0,                         /* DODKEY       */
     0,                         /* DOZIP        */
     0, 0, 0}
};


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
    UMODE_A, 0, 0,              /* A B C */
    0, 0, 0,                    /* D E F */
    0, 0, 0,                    /* G H I */
    0, 0, 0,                    /* J K L */
    0, 0, 0,                    /* M N O */
    0, 0, UMODE_R,              /* P Q R */
    0, 0, 0,                    /* S T U */
    0, 0, 0,                    /* V W X */
    0,                          /* Y */
    0,                          /* Z */
    0, 0, 0,                    /* [ \ ] */
    0, 0, 0,                    /* ^ _ ` */
    UMODE_a, 0, 0,              /* a b c */
    0, 0, 0,                    /* d e f */
    0, UMODE_h, UMODE_i,        /* g h i */
    0, 0, 0,                    /* j k l */
    0, 0, UMODE_o,              /* m n o */
    0, 0, UMODE_r,              /* p q r */
    0, 0, 0,                    /* s t u */
    0, UMODE_w, 0,              /* v w x */
    0,                          /* y */
    0,                          /* z */
    0, 0, 0,                    /* { | } */
    0, 0                        /* ~ � */
};

char myCsmodes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0,
    0,
    0, 0, 0,
    'h',                        /* (37) % Channel halfops */
    0, 0, 0, 0,
    'a',                        /* * Channel Admins */

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
    {CMODE_A, CBM_NO_USER_MLOCK, NULL, NULL},
    {0},                        /* B */
    {CMODE_C, 0, NULL, NULL},
    {0},                        /* D */
    {0},                        /* E */
    {0},                        /* F */
    {0},                        /* G */
    {0},                        /* H */
    {0},                        /* I */
    {0},                        /* J */
    {0},                        /* K */
    {0},                        /* L */
    {CMODE_M, 0, NULL, NULL},   /* M */
    {CMODE_N, 0, NULL, NULL},   /* N */
    {CMODE_O, CBM_NO_USER_MLOCK, NULL, NULL},
    {0},                        /* P */
    {0},                        /* Q */
    {CMODE_R, 0, NULL, NULL},   /* R */
    {CMODE_S, 0, NULL, NULL},   /* S */
    {0},                        /* T */
    {0},                        /* U */
    {0},                        /* V */
    {0},                        /* W */
    {0},                        /* X */
    {0},                        /* Y */
    {0},                        /* Z */
    {0}, {0}, {0}, {0}, {0}, {0},
    {0},                        /* a */
    {0},                        /* b */
    {CMODE_c, 0, NULL, NULL},
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
    {CMODE_r, CBM_NO_MLOCK, NULL, NULL},
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
    {'c', CMODE_c, 0, NULL, NULL},
    {'i', CMODE_i, 0, NULL, NULL},
    {'k', CMODE_k, 0, get_key, cs_get_key},
    {'l', CMODE_l, CBM_MINUS_NO_ARG, get_limit, cs_get_limit},
    {'m', CMODE_m, 0, NULL, NULL},
    {'n', CMODE_n, 0, NULL, NULL},
    {'p', CMODE_p, 0, NULL, NULL},
    {'r', CMODE_r, 0, NULL, NULL},
    {'s', CMODE_s, 0, NULL, NULL},
    {'t', CMODE_t, 0, NULL, NULL},
    {'A', CMODE_A, 0, NULL, NULL},
    {'C', CMODE_C, 0, NULL, NULL},
    {'M', CMODE_M, 0, NULL, NULL},
    {'N', CMODE_N, 0, NULL, NULL},
    {'O', CMODE_O, 0, NULL, NULL},
    {'R', CMODE_R, 0, NULL, NULL},
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
    {CUS_PROTECT, CUF_PROTECT_BOTSERV, check_valid_admin},
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



void rageircd_cmd_bot_unban(ChannelInfo * ci, char *nick)
{
    send_cmd(ServerName, "SVSMODE %s -b %s", ci->name, nick);
}

int anope_event_sjoin(char *source, int ac, char **av)
{
    do_sjoin(source, ac, av);
    return MOD_CONT;
}

int anope_event_nick(char *source, int ac, char **av)
{
    User *user;

    if (ac != 2) {
        user = do_nick(source, av[0], av[4], av[5], av[6], av[9],
                       strtoul(av[2], NULL, 10), strtoul(av[7], NULL, 0),
                       strtoul(av[8], NULL, 0), "*", NULL);
        if (user)
            anope_set_umode(user, 1, &av[3]);
    } else {
        do_nick(source, av[0], NULL, NULL, NULL, NULL,
                strtoul(av[1], NULL, 10), 0, 0, NULL, NULL);
    }
    return MOD_CONT;
}

/* Event: PROTOCTL */
int anope_event_capab(char *source, int ac, char **av)
{
    capab_parse(ac, av);
    return MOD_CONT;
}

int anope_event_vhost(char *source, int ac, char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(av[0]);
    if (!u) {
        if (debug) {
            alog("debug: VHOST for nonexistent user %s", av[0]);
        }
        return MOD_CONT;
    }

    change_user_host(u, av[1]);
    return MOD_CONT;
}

/*
** SNICK
**      source  = NULL       
**	parv[0] = nickname     Trystan
**      parv[1] = timestamp    1090113640
**      parv[2] = hops         1
**      parv[3] = username     Trystan
**	parv[4] = host         c-24-2-101-227.client.comcast.net
**      parv[5] = IP           402810339
**      parv[6] = vhost        mynet-27CCA80D.client.comcast.net
**      parv[7] = server       rage2.nomadirc.net
**	parv[8] = servicestamp 0
**      parv[9] = modes      +ix
** 	parv[10] = info      Dreams are answers to questions not yet asked
*/

int anope_event_snick(char *source, int ac, char **av)
{
    User *user;

    if (ac != 2) {
        user = do_nick(source, av[0], av[3], av[4], av[7], av[10],
                       strtoul(av[1], NULL, 10), strtoul(av[8], NULL, 0),
                       strtoul(av[5], NULL, 0), av[6], NULL);
        if (user) {
            anope_set_umode(user, 1, &av[9]);
        }
    }
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
    
    updateProtectDetails("ADMIN","ADMINME","admin","deadmin","AUTOADMIN","+a","-a");

    m = createMessage("401",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("402",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("436",       anope_event_436); addCoreMessage(IRCD,m);
    m = createMessage("482",       anope_event_482); addCoreMessage(IRCD,m);
    m = createMessage("461",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("AWAY",      anope_event_away); addCoreMessage(IRCD,m);
    m = createMessage("INVITE",    anope_event_null); addCoreMessage(IRCD,m);
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
    m = createMessage("USER",      anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("WALLOPS",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("WHOIS",     anope_event_whois); addCoreMessage(IRCD,m);
    m = createMessage("AKILL",     anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("GLOBOPS",   anope_event_globops); addCoreMessage(IRCD,m);
    m = createMessage("GOPER",     anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("RAKILL",    anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SILENCE",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SVSKILL",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SVSMODE",   anope_event_mode); addCoreMessage(IRCD,m);
    m = createMessage("SVSNICK",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SVSNOOP",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SQLINE",    anope_event_sqline); addCoreMessage(IRCD,m);
    m = createMessage("UNSQLINE",  anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("CAPAB", 	   anope_event_capab); addCoreMessage(IRCD,m);
    m = createMessage("CS",        anope_event_cs); addCoreMessage(IRCD,m);
    m = createMessage("HS",        anope_event_hs); addCoreMessage(IRCD,m);
    m = createMessage("MS",        anope_event_ms); addCoreMessage(IRCD,m);
    m = createMessage("NS",        anope_event_ns); addCoreMessage(IRCD,m);
    m = createMessage("OS",        anope_event_os); addCoreMessage(IRCD,m);
    m = createMessage("RS",        anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SGLINE",    anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SJOIN",     anope_event_sjoin); addCoreMessage(IRCD,m);
    m = createMessage("SS",        anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SVINFO",    anope_event_svinfo); addCoreMessage(IRCD,m);
    m = createMessage("SZLINE",    anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("UNSGLINE",  anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("UNSZLINE",  anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SNICK",	   anope_event_snick); addCoreMessage(IRCD,m);
    m = createMessage("VHOST",	   anope_event_vhost); addCoreMessage(IRCD,m);
    m = createMessage("MYID",	   anope_event_myid); addCoreMessage(IRCD,m);
    m = createMessage("GNOTICE",   anope_event_notice); addCoreMessage(IRCD,m);
    m = createMessage("ERROR",     anope_event_error); addCoreMessage(IRCD,m);
    m = createMessage("BURST",     anope_event_burst); addCoreMessage(IRCD,m);
    m = createMessage("REHASH",     anope_event_rehash); addCoreMessage(IRCD,m);
    m = createMessage("ADMIN",      anope_event_admin); addCoreMessage(IRCD,m);
    m = createMessage("CREDITS",    anope_event_credits); addCoreMessage(IRCD,m);
}

/* *INDENT-ON* */
int anope_event_error(char *source, int ac, char **av)
{
    if (ac >= 1) {
        if (debug) {
            alog("debug: %s", av[0]);
        }
    }
    return MOD_CONT;
}


int anope_event_burst(char *source, int ac, char **av)
{
    Server *s;
    s = findserver(servlist, source);
    if (ac > 1) {
        /* for future use  - start burst */
} else {

    /* If we found a server with the given source, that one just
     * finished bursting. If there was no source, then our uplink
     * server finished bursting. -GD
     */
    if (!s && serv_uplink)
        s = serv_uplink;
    finish_sync(s, 1);
}

return MOD_CONT;
}

void rageircd_cmd_sqline(char *mask, char *reason)
{
    if (!mask || !reason) {
        return;
    }

    send_cmd(NULL, "SQLINE %s :%s", mask, reason);
}

void rageircd_cmd_unsgline(char *mask)
{
    send_cmd(NULL, "UNSGLINE 0 :%s", mask);
}

void rageircd_cmd_unszline(char *mask)
{
    send_cmd(NULL, "UNSZLINE 0 %s", mask);
}

void rageircd_cmd_szline(char *mask, char *reason, char *whom)
{
    send_cmd(NULL, "SZLINE %s :%s", mask, reason);
}

void rageircd_cmd_svsnoop(char *server, int set)
{
    send_cmd(NULL, "SVSNOOP %s %s", server, (set ? "+" : "-"));
}

void rageircd_cmd_svsadmin(char *server, int set)
{
    rageircd_cmd_svsnoop(server, set);
}

void rageircd_cmd_sgline(char *mask, char *reason)
{
    send_cmd(NULL, "SGLINE %d :%s:%s", (int)strlen(mask), mask, reason);

}

void rageircd_cmd_remove_akill(char *user, char *host)
{
    send_cmd(NULL, "RAKILL %s %s", host, user);
}


/* PART */
void rageircd_cmd_part(char *nick, char *chan, char *buf)
{
    if (!nick || !chan) {
        return;
    }

    if (buf) {
        send_cmd(nick, "PART %s :%s", chan, buf);
    } else {
        send_cmd(nick, "PART %s", chan);
    }
}

void rageircd_cmd_topic(char *whosets, char *chan, char *whosetit,
                        char *topic, time_t when)
{
    send_cmd(whosets, "TOPIC %s %s %lu :%s", chan, whosetit,
             (unsigned long int) when, topic);
}

void rageircd_cmd_vhost_off(User * u)
{
    common_svsmode(u, "-x", NULL);
    notice_lang(s_HostServ, u, HOST_OFF_UNREAL, u->nick, ircd->vhostchar);
}

void rageircd_cmd_chghost(char *nick, char *vhost)
{
    if (!nick || !vhost) {
        return;
    }
    send_cmd(ServerName, "VHOST %s %s", nick, vhost);
}

void rageircd_cmd_vhost_on(char *nick, char *vIdent, char *vhost)
{
    send_cmd(s_HostServ, "SVSMODE %s +x", nick);
    rageircd_cmd_chghost(nick, vhost);
}

void rageircd_cmd_unsqline(char *user)
{
    send_cmd(NULL, "UNSQLINE %s", user);
}

void rageircd_cmd_join(char *user, char *channel, time_t chantime)
{
    send_cmd(user, "SJOIN %ld %s", (long int) chantime, channel);
}

void rageircd_cmd_akill(char *user, char *host, char *who, time_t when,
                        time_t expires, char *reason)
{
    send_cmd(NULL, "AKILL %s %s %d %s %ld :%s", host, user, 86400 * 2, who,
             (long int) time(NULL), reason);
}

void rageircd_cmd_svskill(char *source, char *user, char *buf)
{
    if (!buf) {
        return;
    }

    if (!source || !user) {
        return;
    }

    send_cmd(source, "SVSKILL %s :%s", user, buf);
}

void rageircd_cmd_svsmode(User * u, int ac, char **av)
{
    send_cmd(ServerName, "SVSMODE %s %ld %s%s%s", u->nick,
             (long int) u->timestamp, av[0], (ac == 2 ? " " : ""),
             (ac == 2 ? av[1] : ""));
}

void rageircd_cmd_squit(char *servname, char *message)
{
    send_cmd(NULL, "SQUIT %s :%s", servname, message);
}

/* PONG */
void rageircd_cmd_pong(char *servname, char *who)
{
    send_cmd(servname, "PONG %s", who);
}

void rageircd_cmd_svinfo()
{
    send_cmd(NULL, "SVINFO 5 3 0 %ld bluemoon 0", (long int) time(NULL));
}

void rageircd_cmd_capab()
{
    /*  CAPAB BURST UNCONNECT ZIP SSJ3 SN2 VHOST SUID TOK1 TSMODE */
    send_cmd(NULL, "CAPAB BURST UNCONNECT SSJ3 SN2 VHOST TSMODE");
}

void rageircd_cmd_server(char *servname, int hop, char *descript)
{
    send_cmd(NULL, "SERVER %s %d :%s", servname, hop, descript);
}

/* PASS */
void rageircd_cmd_pass(char *pass)
{
    send_cmd(NULL, "PASS %s :TS", pass);
}

void rageircd_cmd_burst()
{
    send_cmd(NULL, "BURST");
}

void rageircd_cmd_connect(int servernum)
{
    if (Numeric) {
        me_server =
            new_server(NULL, ServerName, ServerDesc, SERVER_ISME, Numeric);
    } else {
        me_server =
            new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);
    }

    if (servernum == 1)
        rageircd_cmd_pass(RemotePassword);
    else if (servernum == 2)
        rageircd_cmd_pass(RemotePassword2);
    else if (servernum == 3)
        rageircd_cmd_pass(RemotePassword3);
    rageircd_cmd_capab();
    if (Numeric) {
        send_cmd(NULL, "MYID !%s", Numeric);
    }
    rageircd_cmd_server(ServerName, 1, ServerDesc);
    rageircd_cmd_svinfo();
    rageircd_cmd_burst();
}

void rageircd_cmd_bob()
{
    /* Not used */
}

void rageircd_set_umode(User * user, int ac, char **av)
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
                if (is_services_oper(user)) {
                    common_svsmode(user, "+a", NULL);
                    user->mode |= UMODE_a;
                }
            } else {
                opcnt--;
            }
            break;
        case 'r':
            if (add && !nick_identified(user)) {
                common_svsmode(user, "-r", NULL);
                user->mode &= ~UMODE_r;
            }
            break;
        case 'x':
            update_host(user);
            break;
        }
    }
}

/* GLOBOPS */
void rageircd_cmd_global(char *source, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source ? source : ServerName, "GLOBOPS :%s", buf);
}

void rageircd_cmd_notice_ops(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "NOTICE @%s :%s", dest, buf);
}


void rageircd_cmd_notice(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    if (NSDefFlags & NI_MSG) {
        rageircd_cmd_privmsg2(source, dest, buf);
    } else {
        send_cmd(source, "NOTICE %s :%s", dest, buf);
    }
}

void rageircd_cmd_notice2(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE %s :%s", dest, msg);
}

void rageircd_cmd_privmsg(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "PRIVMSG %s :%s", dest, buf);
}

void rageircd_cmd_privmsg2(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG %s :%s", dest, msg);
}

void rageircd_cmd_serv_notice(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE $%s :%s", dest, msg);
}

void rageircd_cmd_serv_privmsg(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG $%s :%s", dest, msg);
}

int anope_event_away(char *source, int ac, char **av)
{
    if (!source) {
        return MOD_CONT;
    }
    m_away(source, (ac ? av[0] : NULL));
    return MOD_CONT;
}

int anope_event_ping(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    rageircd_cmd_pong(ac > 1 ? av[1] : ServerName, av[0]);
    return MOD_CONT;
}

void rageircd_cmd_351(char *source)
{
    send_cmd(ServerName, "351 %s Anope-%s %s :%s - %s (%s) -- %s",
             source, version_number, ServerName, ircd->name, version_flags,
             EncModule, version_build);
}

void rageircd_cmd_mode(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    if (ircdcap->tsmode) {
        if (uplink_capab & ircdcap->tsmode || UseTSMODE) {
            send_cmd(source, "MODE %s 0 %s", dest, buf);
        } else {
            send_cmd(source, "MODE %s %s", dest, buf);
        }
    } else {
        send_cmd(source, "MODE %s %s", dest, buf);
    }
}


void rageircd_cmd_kick(char *source, char *chan, char *user, char *buf)
{
    if (buf) {
        send_cmd(source, "KICK %s %s :%s", chan, user, buf);
    } else {
        send_cmd(source, "KICK %s %s", chan, user);
    }
}

void rageircd_cmd_372(char *source, char *msg)
{
    send_cmd(ServerName, "372 %s :- %s", source, msg);
}

void rageircd_cmd_372_error(char *source)
{
    send_cmd(ServerName, "422 %s :- MOTD file not found!  Please "
             "contact your IRC administrator.", source);
}

void rageircd_cmd_375(char *source)
{
    send_cmd(ServerName, "375 %s :- %s Message of the Day",
             source, ServerName);
}

void rageircd_cmd_376(char *source)
{
    send_cmd(ServerName, "376 %s :End of /MOTD command.", source);
}

/* INVITE */
void rageircd_cmd_invite(char *source, char *chan, char *nick)
{
    if (!source || !chan || !nick) {
        return;
    }

    send_cmd(source, "INVITE %s %s", nick, chan);
}

/* 391 */
void rageircd_cmd_391(char *source, char *timestr)
{
    if (!timestr) {
        return;
    }
    send_cmd(NULL, "391 :%s %s :%s", source, ServerName, timestr);
}

/* 250 */
void rageircd_cmd_250(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "250 %s", buf);
}

/* 307 */
void rageircd_cmd_307(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "307 %s", buf);
}

/* 311 */
void rageircd_cmd_311(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "311 %s", buf);
}

/* 312 */
void rageircd_cmd_312(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "312 %s", buf);
}

/* 317 */
void rageircd_cmd_317(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "317 %s", buf);
}

/* 219 */
void rageircd_cmd_219(char *source, char *letter)
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
void rageircd_cmd_401(char *source, char *who)
{
    if (!source || !who) {
        return;
    }
    send_cmd(ServerName, "401 %s %s :No such service.", source, who);
}

/* 318 */
void rageircd_cmd_318(char *source, char *who)
{
    if (!source || !who) {
        return;
    }

    send_cmd(ServerName, "318 %s %s :End of /WHOIS list.", source, who);
}

/* 242 */
void rageircd_cmd_242(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "242 %s", buf);
}

/* 243 */
void rageircd_cmd_243(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "243 %s", buf);
}

/* 211 */
void rageircd_cmd_211(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "211 %s", buf);
}

void rageircd_cmd_nick(char *nick, char *name, char *modes)
{
    EnforceQlinedNick(nick, NULL);
    send_cmd(NULL, "SNICK %s %ld 1 %s %s 0 * %s 0 %s :%s", nick,
             (long int) time(NULL), ServiceUser, ServiceHost, ServerName,
             modes, name);
    rageircd_cmd_sqline(nick, "Reserved for services");
}

/* EVENT : OS */
int anope_event_os(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    m_privmsg(source, s_OperServ, av[0]);
    return MOD_CONT;
}

/* EVENT : NS */
int anope_event_ns(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    m_privmsg(source, s_NickServ, av[0]);
    return MOD_CONT;
}

/* EVENT : MS */
int anope_event_ms(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    m_privmsg(source, s_MemoServ, av[0]);
    return MOD_CONT;
}

/* EVENT : HS */
int anope_event_hs(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    m_privmsg(source, s_HostServ, av[0]);
    return MOD_CONT;
}

/* EVENT : CS */
int anope_event_cs(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    m_privmsg(source, s_ChanServ, av[0]);
    return MOD_CONT;
}

/* QUIT */
void rageircd_cmd_quit(char *source, char *buf)
{
    if (buf) {
        send_cmd(source, "QUIT :%s", buf);
    } else {
        send_cmd(source, "QUIT");
    }
}

void rageircd_cmd_bot_nick(char *nick, char *user, char *host, char *real,
                           char *modes)
{
    EnforceQlinedNick(nick, s_BotServ);
    send_cmd(NULL, "SNICK %s %ld 1 %s %s 0 * %s 0 %s :%s", nick,
             (long int) time(NULL), user, host, ServerName, modes, real);
    rageircd_cmd_sqline(nick, "Reserved for services");
}

/* SVSMODE -b */
void rageircd_cmd_unban(char *name, char *nick)
{
    rageircd_cmd_svsmode_chan(name, "-b", nick);
}


/* SVSMODE channel modes */

void rageircd_cmd_svsmode_chan(char *name, char *mode, char *nick)
{
    if (nick) {
        send_cmd(ServerName, "SVSMODE %s %s %s", name, mode, nick);
    } else {
        send_cmd(ServerName, "SVSMODE %s %s", name, mode);
    }
}

void rageircd_cmd_bot_chan_mode(char *nick, char *chan)
{
    anope_cmd_mode(nick, chan, "%s %s", ircd->botchanumode, nick);
}

int anope_event_server(char *source, int ac, char **av)
{
    if (!stricmp(av[1], "1")) {
        uplink = sstrdup(av[0]);
    }
    do_server(source, av[0], av[1], av[2], NULL);
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


int anope_event_482(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_topic(char *source, int ac, char **av)
{
    if (ac != 4)
        return MOD_CONT;
    do_topic(source, ac, av);
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

/* SVSHOLD - set */
void rageircd_cmd_svshold(char *nick)
{
    send_cmd(ServerName, "SVSHOLD %s %d :%s", nick, NSReleaseTimeout,
             "Being held for registered user");
}

/* SVSHOLD - release */
void rageircd_cmd_release_svshold(char *nick)
{
    send_cmd(ServerName, "SVSHOLD %s 0", nick);
}

void rageircd_cmd_svsnick(char *source, char *guest, time_t when)
{
    if (!source || !guest) {
        return;
    }
    send_cmd(NULL, "SVSNICK %s %s :%ld", source, guest, (long int) when);
}

void rageircd_cmd_guest_nick(char *nick, char *user, char *host,
                             char *real, char *modes)
{
    send_cmd(NULL, "SNICK %s %ld 1 %s %s 0 * %s 0 %s :%s", nick,
             (long int) time(NULL), user, host, ServerName, modes, real);
}


void rageircd_cmd_svso(char *source, char *nick, char *flag)
{
    /* Not Supported by this IRCD */
}


/* SVSMODE +d */
/* sent if svid is something weird */
void rageircd_cmd_svid_umode(char *nick, time_t ts)
{
    send_cmd(ServerName, "SVSMODE %s %lu +d 1", nick,
             (unsigned long int) ts);
}

/* SVSMODE +d */
/* nc_change was = 1, and there is no na->status */
void rageircd_cmd_nc_change(User * u)
{
    common_svsmode(u, "+d", "1");
}

/* SVSMODE +d */
void rageircd_cmd_svid_umode2(User * u, char *ts)
{
    /* not used by bahamut ircds */
}

void rageircd_cmd_svid_umode3(User * u, char *ts)
{
    if (u->svid != u->timestamp) {
        common_svsmode(u, "+rd", ts);
    } else {
        common_svsmode(u, "+r", NULL);
    }
}

/* NICK <newnick>  */
void rageircd_cmd_chg_nick(char *oldnick, char *newnick)
{
    if (!oldnick || !newnick) {
        return;
    }

    send_cmd(oldnick, "NICK %s", newnick);
}

int anope_event_myid(char *source, int ac, char **av)
{
    /* currently not used but removes the message : unknown message from server */
    return MOD_CONT;
}

int anope_event_pass(char *source, int ac, char **av)
{
    /* currently not used but removes the message : unknown message from server */
    return MOD_CONT;
}

/*
 * SVINFO
 *	parv[0] = sender prefix
 *
 *	if (parc == 2)
 *		parv[1] = ZIP (compression initialisation)
 *
 *	if (parc > 2)
 *		parv[1] = TS_CURRENT
 *		parv[2] = TS_MIN
 *		parv[3] = standalone or connected to non-TS (unused)
 *		parv[4] = UTC time
 *		parv[5] = ircd codename
 *		parv[6] = masking keys 
 */
int anope_event_svinfo(char *source, int ac, char **av)
{
    /* currently not used but removes the message : unknown message from server */
    return MOD_CONT;
}

int anope_event_gnotice(char *source, int ac, char **av)
{
    /* currently not used but removes the message : unknown message from server */
    return MOD_CONT;
}

int anope_event_notice(char *source, int ac, char **av)
{
    /* currently not used but removes the message : unknown message from server */
    return MOD_CONT;
}

int anope_event_sqline(char *source, int ac, char **av)
{
    /* currently not used but removes the message : unknown message from server */
    return MOD_CONT;
}

void rageircd_cmd_svsjoin(char *source, char *nick, char *chan, char *param)
{
    /* Find no reference to it in the code and docs */
}

void rageircd_cmd_svspart(char *source, char *nick, char *chan)
{
    /* Find no reference to it in the code and docs */
}

void rageircd_cmd_swhois(char *source, char *who, char *mask)
{
    /* not supported */
}

int anope_event_rehash(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_credits(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_admin(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_globops(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int rageircd_flood_mode_check(char *value)
{
    return 0;
}

void rageircd_cmd_eob()
{
    send_cmd(NULL, "BURST 0");
}

void rageircd_cmd_jupe(char *jserver, char *who, char *reason)
{
    char rbuf[256];

    snprintf(rbuf, sizeof(rbuf), "Juped by %s%s%s", who,
             reason ? ": " : "", reason ? reason : "");

    if (findserver(servlist, jserver))
        rageircd_cmd_squit(jserver, rbuf);
    rageircd_cmd_server(jserver, 2, rbuf);
    new_server(me_server, jserver, rbuf, SERVER_JUPED, NULL);
}

/* GLOBOPS - to handle old WALLOPS */
void rageircd_cmd_global_legacy(char *source, char *fmt)
{
    send_cmd(source ? source : ServerName, "GLOBOPS :%s", fmt);
}

/* 
  1 = valid nick
  0 = nick is in valid
*/
int rageircd_valid_nick(char *nick)
{
    /* no hard coded invalid nicks */
    return 1;
}

/* 
  1 = valid chan
  0 = chan is in valid
*/
int rageircd_valid_chan(char *chan)
{
    /* no hard coded invalid nicks */
    return 1;
}


void rageircd_cmd_ctcp(char *source, char *dest, char *buf)
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


/**
 * Tell anope which function we want to perform each task inside of anope.
 * These prototypes must match what anope expects.
 **/
void moduleAddAnopeCmds()
{
    pmodule_cmd_svsnoop(rageircd_cmd_svsnoop);
    pmodule_cmd_remove_akill(rageircd_cmd_remove_akill);
    pmodule_cmd_topic(rageircd_cmd_topic);
    pmodule_cmd_vhost_off(rageircd_cmd_vhost_off);
    pmodule_cmd_akill(rageircd_cmd_akill);
    pmodule_cmd_svskill(rageircd_cmd_svskill);
    pmodule_cmd_svsmode(rageircd_cmd_svsmode);
    pmodule_cmd_372(rageircd_cmd_372);
    pmodule_cmd_372_error(rageircd_cmd_372_error);
    pmodule_cmd_375(rageircd_cmd_375);
    pmodule_cmd_376(rageircd_cmd_376);
    pmodule_cmd_nick(rageircd_cmd_nick);
    pmodule_cmd_guest_nick(rageircd_cmd_guest_nick);
    pmodule_cmd_mode(rageircd_cmd_mode);
    pmodule_cmd_bot_nick(rageircd_cmd_bot_nick);
    pmodule_cmd_kick(rageircd_cmd_kick);
    pmodule_cmd_notice_ops(rageircd_cmd_notice_ops);
    pmodule_cmd_notice(rageircd_cmd_notice);
    pmodule_cmd_notice2(rageircd_cmd_notice2);
    pmodule_cmd_privmsg(rageircd_cmd_privmsg);
    pmodule_cmd_privmsg2(rageircd_cmd_privmsg2);
    pmodule_cmd_serv_notice(rageircd_cmd_serv_notice);
    pmodule_cmd_serv_privmsg(rageircd_cmd_serv_privmsg);
    pmodule_cmd_bot_chan_mode(rageircd_cmd_bot_chan_mode);
    pmodule_cmd_351(rageircd_cmd_351);
    pmodule_cmd_quit(rageircd_cmd_quit);
    pmodule_cmd_pong(rageircd_cmd_pong);
    pmodule_cmd_join(rageircd_cmd_join);
    pmodule_cmd_unsqline(rageircd_cmd_unsqline);
    pmodule_cmd_invite(rageircd_cmd_invite);
    pmodule_cmd_part(rageircd_cmd_part);
    pmodule_cmd_391(rageircd_cmd_391);
    pmodule_cmd_250(rageircd_cmd_250);
    pmodule_cmd_307(rageircd_cmd_307);
    pmodule_cmd_311(rageircd_cmd_311);
    pmodule_cmd_312(rageircd_cmd_312);
    pmodule_cmd_317(rageircd_cmd_317);
    pmodule_cmd_219(rageircd_cmd_219);
    pmodule_cmd_401(rageircd_cmd_401);
    pmodule_cmd_318(rageircd_cmd_318);
    pmodule_cmd_242(rageircd_cmd_242);
    pmodule_cmd_243(rageircd_cmd_243);
    pmodule_cmd_211(rageircd_cmd_211);
    pmodule_cmd_global(rageircd_cmd_global);
    pmodule_cmd_global_legacy(rageircd_cmd_global_legacy);
    pmodule_cmd_sqline(rageircd_cmd_sqline);
    pmodule_cmd_squit(rageircd_cmd_squit);
    pmodule_cmd_svso(rageircd_cmd_svso);
    pmodule_cmd_chg_nick(rageircd_cmd_chg_nick);
    pmodule_cmd_svsnick(rageircd_cmd_svsnick);
    pmodule_cmd_vhost_on(rageircd_cmd_vhost_on);
    pmodule_cmd_connect(rageircd_cmd_connect);
    pmodule_cmd_bob(rageircd_cmd_bob);
    pmodule_cmd_svshold(rageircd_cmd_svshold);
    pmodule_cmd_release_svshold(rageircd_cmd_release_svshold);
    pmodule_cmd_unsgline(rageircd_cmd_unsgline);
    pmodule_cmd_unszline(rageircd_cmd_unszline);
    pmodule_cmd_szline(rageircd_cmd_szline);
    pmodule_cmd_sgline(rageircd_cmd_sgline);
    pmodule_cmd_unban(rageircd_cmd_unban);
    pmodule_cmd_svsmode_chan(rageircd_cmd_svsmode_chan);
    pmodule_cmd_svid_umode(rageircd_cmd_svid_umode);
    pmodule_cmd_nc_change(rageircd_cmd_nc_change);
    pmodule_cmd_svid_umode2(rageircd_cmd_svid_umode2);
    pmodule_cmd_svid_umode3(rageircd_cmd_svid_umode3);
    pmodule_cmd_svsjoin(rageircd_cmd_svsjoin);
    pmodule_cmd_svspart(rageircd_cmd_svspart);
    pmodule_cmd_swhois(rageircd_cmd_swhois);
    pmodule_cmd_eob(rageircd_cmd_eob);
    pmodule_flood_mode_check(rageircd_flood_mode_check);
    pmodule_cmd_jupe(rageircd_cmd_jupe);
    pmodule_valid_nick(rageircd_valid_nick);
    pmodule_valid_chan(rageircd_valid_chan);
    pmodule_cmd_ctcp(rageircd_cmd_ctcp);
    pmodule_set_umode(rageircd_set_umode);
}

/** 
 * Now tell anope how to use us.
 **/
int AnopeInit(int argc, char **argv)
{

    moduleAddAuthor("Anope");
    moduleAddVersion
        (VERSION_STRING);
    moduleSetType(PROTOCOL);

    pmodule_ircd_version("RageIRCd 2.0.x");
    pmodule_ircd_cap(myIrcdcap);
    pmodule_ircd_var(myIrcd);
    pmodule_ircd_cbmodeinfos(myCbmodeinfos);
    pmodule_ircd_cumodes(myCumodes);
    pmodule_ircd_flood_mode_char_set("");
    pmodule_ircd_flood_mode_char_remove("");
    pmodule_ircd_cbmodes(myCbmodes);
    pmodule_ircd_cmmodes(myCmmodes);
    pmodule_ircd_csmodes(myCsmodes);
    pmodule_ircd_useTSMode(1);

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

    return MOD_CONT;
}

/* EOF */
