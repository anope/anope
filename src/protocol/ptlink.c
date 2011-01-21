/* PTLink IRCD functions
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
#include "ptlink.h"
#include "version.h"

IRCDVar myIrcd[] = {
    {"PTlink 6.15.*+",          /* ircd name */
     "+o",                      /* nickserv mode */
     "+o",                      /* chanserv mode */
     "+o",                      /* memoserv mode */
     "+o",                      /* hostserv mode */
     "+io",                     /* operserv mode */
     "+o",                      /* botserv mode  */
     "+h",                      /* helpserv mode */
     "+i",                      /* Dev/Null mode */
     "+io",                     /* Global mode   */
     "+o",                      /* nickserv alias mode */
     "+o",                      /* chanserv alias mode */
     "+o",                      /* memoserv alias mode */
     "+io",                     /* hostserv alias mode */
     "+io",                     /* operserv alias mode */
     "+o",                      /* botserv alias mode  */
     "+h",                      /* helpserv alias mode */
     "+i",                      /* Dev/Null alias mode */
     "+io",                     /* Global alias mode   */
     "+",                       /* Used by BotServ Bots */
     2,                         /* Chan Max Symbols     */
     "-inpsmtCRKOASdcqBNl",     /* Modes to Remove */
     "+ao",                     /* Channel Umode used by Botserv bots */
     1,                         /* SVSNICK */
     1,                         /* Vhost  */
     0,                         /* Has Owner */
     NULL,                      /* Mode to set for an owner */
     NULL,                      /* Mode to unset for an owner */
     "+a",                      /* Mode to set for chan admin */
     "-a",                      /* Mode to unset for chan admin */
     "+r",                      /* Mode On Reg          */
     NULL,                      /* Mode on ID for Roots */
     NULL,                      /* Mode on ID for Admins */
     NULL,                      /* Mode on ID for Opers */
     "-r",                      /* Mode on UnReg        */
     NULL,                      /* Mode on Nick Change  */
     1,                         /* Supports SGlines     */
     1,                         /* Supports SQlines     */
     1,                         /* Supports SZlines     */
     0,                         /* Supports Halfop +h   */
     4,                         /* Number of server args */
     0,                         /* Join 2 Set           */
     0,                         /* Join 2 Message       */
     1,                         /* Has exceptions +e    */
     0,                         /* TS Topic Forward     */
     0,                         /* TS Topci Backward    */
     0,                         /* Protected Umode      */
     0,                         /* Has Admin            */
     0,                         /* Chan SQlines         */
     0,                         /* Quit on Kill         */
     0,                         /* SVSMODE unban        */
     1,                         /* Has Protect          */
     0,                         /* Reverse              */
     1,                         /* Chan Reg             */
     CMODE_r,                   /* Channel Mode         */
     1,                         /* vidents              */
     0,                         /* svshold              */
     0,                         /* time stamp on mode   */
     0,                         /* NICKIP               */
     0,                         /* O:LINE               */
     1,                         /* UMODE                */
     1,                         /* VHOST ON NICK        */
     0,                         /* Change RealName      */
     CMODE_K,                   /* No Knock             */
     CMODE_A,                   /* Admin Only           */
     DEFAULT_MLOCK,             /* Default MLOCK        */
     UMODE_VH,                  /* Vhost Mode           */
     1,                         /* +f                   */
     0,                         /* +L                   */
     CMODE_f,
     0,
     1,
     1,                         /* No Knock requires +i */
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
     1,                         /* support helper umode */
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
     0,                         /* NOQUIT       */
     0,                         /* TSMODE       */
     0,                         /* UNCONNECT    */
     0,                         /* NICKIP       */
     0,                         /* SJOIN        */
     CAPAB_ZIP,                 /* ZIP          */
     0,                         /* BURST        */
     0,                         /* TS5          */
     0,                         /* TS3          */
     0,                         /* DKEY         */
     CAPAB_PT4,                 /* PT4          */
     CAPAB_SCS,                 /* SCS          */
     CAPAB_QS,                  /* QS           */
     0,                         /* UID          */
     0,                         /* KNOCK        */
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
    UMODE_A, UMODE_B, 0,        /* A B C */
    0, 0, 0,                    /* D E F */
    0, UMODE_H, 0,              /* G H I */
    0, 0, 0,                    /* J K L */
    0, UMODE_N, UMODE_O,        /* M N O */
    0, 0, UMODE_R,              /* P Q R */
    UMODE_S, UMODE_T, 0,        /* S T U */
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
    UMODE_p, 0, UMODE_r,        /* p q r */
    UMODE_s, 0, 0,              /* s t u */
    UMODE_v, UMODE_w, 0,        /* v w x */
    UMODE_y,                    /* y */
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

    'v', 0, 0, 'a', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    'o', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


CMMode myCmmodes[128] = {
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL},
    {NULL},
    {NULL},
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
    {CMODE_A, 0, NULL, NULL},   /* A */
    {CMODE_B, 0, NULL, NULL},   /* B */
    {CMODE_C, 0, NULL, NULL},   /* C */
    {0},                        /* D */
    {0},                        /* E */
    {0},                        /* F */
    {0},                        /* G */
    {0},                        /* H */
    {0},                        /* I */
    {0},                        /* J */
    {CMODE_K, 0, NULL, NULL},   /* K */
    {0},                        /* L */
    {0},                        /* M */
    {CMODE_N, 0, NULL, NULL},   /* N */
    {CMODE_O, 0, NULL, NULL},   /* O */
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
    {CMODE_d, 0, NULL, NULL},
    {0},                        /* e */
    {CMODE_f, 0, set_flood, cs_set_flood},
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
    {CMODE_q, 0, NULL, NULL},
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
    {'d', CMODE_d, 0, NULL, NULL},
    {'f', CMODE_f, 0, get_flood, cs_get_flood},
    {'i', CMODE_i, 0, NULL, NULL},
    {'k', CMODE_k, 0, get_key, cs_get_key},
    {'l', CMODE_l, CBM_MINUS_NO_ARG, get_limit, cs_get_limit},
    {'m', CMODE_m, 0, NULL, NULL},
    {'n', CMODE_n, 0, NULL, NULL},
    {'p', CMODE_p, 0, NULL, NULL},
    {'q', CMODE_q, 0, NULL, NULL},
    {'r', CMODE_r, 0, NULL, NULL},
    {'s', CMODE_s, 0, NULL, NULL},
    {'t', CMODE_t, 0, NULL, NULL},
    {'A', CMODE_A, 0, NULL, NULL},
    {'B', CMODE_B, 0, NULL, NULL},
    {'C', CMODE_C, 0, NULL, NULL},
    {'K', CMODE_K, 0, NULL, NULL},
    {'N', CMODE_N, 0, NULL, NULL},
    {'O', CMODE_O, 0, NULL, NULL},
    {'R', CMODE_R, 0, NULL, NULL},
    {'S', CMODE_S, 0, NULL, NULL},
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
    {0},                        /* h */
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



void ptlink_cmd_bot_chan_mode(char *nick, char *chan)
{
    anope_cmd_mode(s_ChanServ, chan, "%s %s %s", ircd->botchanumode, nick,
                   nick);
}

/*
  :%s SJOIN %lu %s %s %s :%s
	parv[0] = sender
	parv[1] = channel TS (channel creation time)
	parv[2] = channel
	parv[3] = modes + n arguments (key and/or limit) 
	... [n]  = if(key and/or limit) mode arguments
	parv[4+n] = flags+nick list (all in one parameter)
	NOTE: ignore channel modes if we already have the channel with a gr
*/
int anope_event_sjoin(char *source, int ac, char **av)
{
    do_sjoin(source, ac, av);
    return MOD_CONT;
}

/*
 * Note: This function has no validation whatsoever. Also, as of PTlink6.15.1
 * when you /deoper you get to keep your vindent, but you lose your vhost. In
 * that case serives will *NOT* modify it's internal record for the vhost. We
 * need to address this in the future.
 */
/*
  :%s NEWMASK %s
	parv[0] = sender
	parv[1] = new mask (if no '@', hostname is assumed)
*/
int anope_event_newmask(char *source, int ac, char **av)
{
    User *u;
    char *newhost = NULL, *newuser = NULL;
    int tofree = 0;

    if (ac != 1)
        return MOD_CONT;
    u = finduser(source);

    if (!u) {
        if (debug) {
            alog("debug: NEWMASK for nonexistent user %s", av[0]);
        }
        return MOD_CONT;
    }

    if ((u->mode & (UMODE_NM | UMODE_VH)) == (UMODE_NM | UMODE_VH)) {
        /* This NEWMASK should be discarded because it's sent due to a +r by
         * someone with a ptlink-masked host. PTlink has our correct host, so
         * we can just ignore this :) Or we'll get ptlink's old host which is
         * not what we want. -GD
         */
        u->mode &= ~UMODE_NM;
        if (debug)
            alog("debug: Ignoring NEWMASK because it's sent because of SVSMODE +r");
        return MOD_CONT;
    }

    newuser = myStrGetOnlyToken(av[0], '@', 0);
    if (newuser) {
        newhost = myStrGetTokenRemainder(av[0], '@', 1);
        tofree = 1;
        change_user_username(u, newuser);
        free(newuser);
    } else {
        newhost = av[0];
    }

    if (newhost && *newhost == '@')
        newhost++;

    u->mode |= UMODE_VH;

    if (newhost)
        change_user_host(u, newhost);

    if (tofree)
        free(newhost);

    return MOD_CONT;
}

/*
 NICK %s %d %lu %s %s %s %s %s :%s
	parv[0] = nickname
	parv[1] = hopcount 
	parv[2] = nick TS (nick introduction time)
	parv[3] = umodes
    parv[4] = username
    parv[5] = hostname
    parv[6] = spoofed hostname
    parv[7] = server
    parv[8] = nick info
*/
/*
 Change NICK
	parv[0] = old nick
	parv[1] = new nick
	parv[2] = TS (timestamp from user's server when nick changed was received)
*/
/*
 NICK xpto 2 561264 +rw irc num.myisp.pt mask.myisp.pt uc.ptlink.net :Just me
       0   1  2      3   4   5            6              7             8

*/
int anope_event_nick(char *source, int ac, char **av)
{
    User *user;

    if (ac != 2) {
        user = do_nick(source, av[0], av[4], av[5], av[7], av[8],
                       strtoul(av[2], NULL, 10), 0, 0, av[6], NULL);
        if (user)
            anope_set_umode(user, 1, &av[3]);
    } else {
        do_nick(source, av[0], NULL, NULL, NULL, NULL,
                strtoul(av[1], NULL, 10), 0, 0, NULL, NULL);
    }
    return MOD_CONT;
}

/*
  :%s SERVER %s %d %s :%s
	parv[0] = server from where the server was introduced to us 
  	parv[1] = server name
	parv[2] = hop count (1 wen are directly connected)
	parv[3] = server version
	parv[4] = server description
*/
int anope_event_server(char *source, int ac, char **av)
{
    if (!stricmp(av[1], "1")) {
        uplink = sstrdup(av[0]);
    }
    do_server(source, av[0], av[1], av[3], NULL);
    return MOD_CONT;
}

int anope_event_436(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;

    m_nickcoll(av[0]);
    return MOD_CONT;
}


void moduleAddIRCDMsgs(void)
{
    Message *m;

    updateProtectDetails("PROTECT", "PROTECTME", "protect", "deprotect",
                         "AUTOPROTECT", "+a", "-a");

    m = createMessage("401", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("402", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("436", anope_event_436);
    addCoreMessage(IRCD, m);
    m = createMessage("461", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("AWAY", anope_event_away);
    addCoreMessage(IRCD, m);
    m = createMessage("INVITE", anope_event_invite);
    addCoreMessage(IRCD, m);
    m = createMessage("JOIN", anope_event_join);
    addCoreMessage(IRCD, m);
    m = createMessage("KICK", anope_event_kick);
    addCoreMessage(IRCD, m);
    m = createMessage("KILL", anope_event_kill);
    addCoreMessage(IRCD, m);
    m = createMessage("MODE", anope_event_mode);
    addCoreMessage(IRCD, m);
    m = createMessage("MOTD", anope_event_motd);
    addCoreMessage(IRCD, m);
    m = createMessage("NICK", anope_event_nick);
    addCoreMessage(IRCD, m);
    m = createMessage("NOTICE", anope_event_notice);
    addCoreMessage(IRCD, m);
    m = createMessage("PART", anope_event_part);
    addCoreMessage(IRCD, m);
    m = createMessage("PASS", anope_event_pass);
    addCoreMessage(IRCD, m);
    m = createMessage("PING", anope_event_ping);
    addCoreMessage(IRCD, m);
    m = createMessage("PRIVMSG", anope_event_privmsg);
    addCoreMessage(IRCD, m);
    m = createMessage("QUIT", anope_event_quit);
    addCoreMessage(IRCD, m);
    m = createMessage("SERVER", anope_event_server);
    addCoreMessage(IRCD, m);
    m = createMessage("SQUIT", anope_event_squit);
    addCoreMessage(IRCD, m);
    m = createMessage("TOPIC", anope_event_topic);
    addCoreMessage(IRCD, m);
    m = createMessage("USER", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("WALLOPS", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("WHOIS", anope_event_whois);
    addCoreMessage(IRCD, m);
    m = createMessage("AKILL", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("GLOBOPS", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("GNOTICE", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("GOPER", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("RAKILL", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("SILENCE", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("SVSKILL", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("SVSMODE", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("SVSNICK", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("SVSNOOP", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("SQLINE", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("UNSQLINE", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("NEWMASK", anope_event_newmask);
    addCoreMessage(IRCD, m);
    m = createMessage("CAPAB", anope_event_capab);
    addCoreMessage(IRCD, m);
    m = createMessage("SVINFO", anope_event_svinfo);
    addCoreMessage(IRCD, m);
    m = createMessage("SVSINFO", anope_event_svsinfo);
    addCoreMessage(IRCD, m);
    m = createMessage("SJOIN", anope_event_sjoin);
    addCoreMessage(IRCD, m);
    m = createMessage("REHASH", anope_event_rehash);
    addCoreMessage(IRCD, m);
    m = createMessage("ADMIN", anope_event_admin);
    addCoreMessage(IRCD, m);
    m = createMessage("CREDITS", anope_event_credits);
    addCoreMessage(IRCD, m);
    m = createMessage("ERROR", anope_event_error);
    addCoreMessage(IRCD, m);
    m = createMessage("NJOIN", anope_event_sjoin);
    addCoreMessage(IRCD, m);
    m = createMessage("NNICK", anope_event_nick);
    addCoreMessage(IRCD, m);
    m = createMessage("ZLINE", anope_event_null);
    addCoreMessage(IRCD, m);
    m = createMessage("UNZLINE", anope_event_null);
    addCoreMessage(IRCD, m);
}

int anope_event_svsinfo(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_svinfo(char *source, int ac, char **av)
{
    return MOD_CONT;
}

/* Event: PROTOCTL */
int anope_event_capab(char *source, int ac, char **av)
{
    capab_parse(ac, av);
    return MOD_CONT;
}

/*
   :%s SQLINE %s :%s
	parv[0] = sender 
	parv[1] = sqlined nick/mask
	parv[2] = reason
*/
void ptlink_cmd_sqline(char *mask, char *reason)
{
    send_cmd(ServerName, "SQLINE %s :%s", mask, reason);
}

/*
  :%s SVSADMIN %s :%s
  	parv[0] = sender (services client)
	parv[1]	= target server
    parv[2] = operation
	  operations:
		noopers - remove existing opers and disable o:lines
*/
void ptlink_cmd_svsnoop(char *server, int set)
{
    send_cmd(NULL, "SVSADMIN %s :%s", server, set ? "noopers" : "rehash");
}

void ptlink_cmd_svsadmin(char *server, int set)
{
    ptlink_cmd_svsnoop(server, set);
}

/*
  :%s UNGLINE %s
	parv[0] = sender (server if on network synchronization)
	parv[1] = glined usert@host mask or ALL to remove all glines
*/
void ptlink_cmd_remove_akill(char *user, char *host)
{
    send_cmd(NULL, "UNGLINE %s@%s", user, host);
}


void anope_part(char *nick, char *chan)
{
    send_cmd(nick, "PART %s", chan);
}
void anope_topic(char *whosets, char *chan, char *whosetit, char *topic,
                 time_t when)
{
    send_cmd(whosets, "TOPIC %s %s %lu :%s", chan, whosetit,
             (unsigned long int) when, topic);
}

/*
 :%s UNSQLINE %s
	parv[0] = sender 
	parv[1] = sqlined nick/mask
*/
void ptlink_cmd_unsqline(char *user)
{
    send_cmd(NULL, "UNSQLINE %s", user);
}

void ptlink_cmd_join(char *user, char *channel, time_t chantime)
{
    send_cmd(ServerName, "SJOIN %ld %s + :%s", (long int) chantime,
             channel, user);
}

/*
   :%s GLINE %s %lu %s %s
	parv[0] = sender (server if on network synchronization)
	parv[1] = glined usert@host mask
	parv[2] = gline duration time (seconds)
	parv[3] = who added the gline 
	parv[4] = reason
*/
void ptlink_cmd_akill(char *user, char *host, char *who, time_t when,
                      time_t expires, char *reason)
{
    send_cmd(ServerName, "GLINE %s@%s %i %s :%s", user, host, 86400 * 2,
             who, reason);
}


void ptlink_cmd_svskill(char *source, char *user, char *buf)
{
    if (!buf) {
        return;
    }

    if (!source || !user) {
        return;
    }

    send_cmd(source, "KILL %s :%s", user, buf);
}

/*
  :%s SVSMODE %s %s :%s
  	parv[0] = sender (services client)
	parv[1]	= target client nick
	parv[2] = mode changes
	parv[3] = extra parameter ( if news setting mode(+n) )
  e.g.:	:NickServ SVSMODE Lamego +rn 1991234
*/
void ptlink_cmd_svsmode(User * u, int ac, char **av)
{
    send_cmd(ServerName, "SVSMODE %s %s%s%s", u->nick, av[0],
             (ac == 2 ? " " : ""), (ac == 2 ? av[1] : ""));

    /* If we set +r on someone +NRah (1 or more of those modes), PTlink will
     * send us a NEWMASK with their ptlink-masked-host. If we want HostServ
     * to work for them, we will need to send our NEWMASK after we receive
     * theirs. Thus we make a hack and store in moduleData that we need to
     * look out for that.
     */
    if ((strchr(av[0], 'r')
         && ((u->mode & UMODE_N) || (u->mode & UMODE_R)
             || (u->mode & UMODE_a) || (u->mode & UMODE_h))))
        u->mode |= UMODE_NM;
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

void ptlink_cmd_squit(char *servname, char *message)
{
    send_cmd(NULL, "SQUIT %s :%s", servname, message);
}

/* PONG */
void ptlink_cmd_pong(char *servname, char *who)
{
    send_cmd(servname, "PONG %s", who);
}

/*
  SVINFO %d %d
	parv[0] = server name
	parv[1] = current supported protocol version
	parv[2] = minimum supported protocol version

  See the ptlink.h for information on PTLINK_TS_CURRENT, and
  PTLINK_TS_MIN
*/
void ptlink_cmd_svinfo()
{
#if defined(PTLINK_TS_CURRENT) && defined(PTLINK_TS_MIN)
    send_cmd(NULL, "SVINFO %d %d %lu", PTLINK_TS_CURRENT, PTLINK_TS_MIN,
             (unsigned long int) time(NULL));
#else
    /* hardwired if the defs some how go missing */
    send_cmd(NULL, "SVINFO 6 3 %lu", (unsigned long int) time(NULL));
#endif
}

/*
  SVSINFO %lu %d
  	parv[0] = sender (server name)
	parv[1] = local services data TS
	parv[1] = max global users
*/
void ptlink_cmd_svsinfo()
{
    send_cmd(NULL, "SVSINFO %lu %d", (unsigned long int) time(NULL),
             maxusercnt);
}

/*
  PASS %s :TS
	parv[1] = connection password 
	(TS indicates this is server uses TS protocol and SVINFO will be sent 
	for protocol compatibility checking)
*/
void ptlink_cmd_pass(char *pass)
{
    send_cmd(NULL, "PASS %s :TS", pass);
}

/*
  CAPAB :%s
	parv[1] = capability list 
*/
void ptlink_cmd_capab()
{
    send_cmd(NULL, "CAPAB :QS PTS4");
}


void ptlink_cmd_server(char *servname, int hop, char *descript)
{
    send_cmd(NULL, "SERVER %s %d Anope.Services%s :%s", servname, hop,
             version_number_dotted, descript);
}
void ptlink_cmd_connect(int servernum)
{
    me_server =
        new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);

    if (servernum == 1)
        ptlink_cmd_pass(RemotePassword);
    else if (servernum == 2)
        ptlink_cmd_pass(RemotePassword2);
    else if (servernum == 3)
        ptlink_cmd_pass(RemotePassword3);

    ptlink_cmd_capab();
    ptlink_cmd_server(ServerName, 1, ServerDesc);
    ptlink_cmd_svinfo();
    ptlink_cmd_svsinfo();
}

void ptlink_cmd_bob()
{
    /* Not used */
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

/*
  :%s TOPIC %s %s %lu :%s
	parv[0] = sender prefix
	parv[1] = channel
	parv[2] = topic nick
    parv[3] = topic time
    parv[4] = topic text
*/
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

/*
  :%s MODE %s :%s
	parv[0] = sender
	parv[1] = target nick (==sender)
	parv[2] = mode change string
*/
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

void ptlink_cmd_notice_ops(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "NOTICE @%s :%s", dest, buf);
}


void ptlink_cmd_notice(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    if (NSDefFlags & NI_MSG) {
        ptlink_cmd_privmsg2(source, dest, buf);
    } else {
        send_cmd(source, "NOTICE %s :%s", dest, buf);
    }
}

void ptlink_cmd_notice2(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE %s :%s", dest, msg);
}

void ptlink_cmd_privmsg(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "PRIVMSG %s :%s", dest, buf);
}

void ptlink_cmd_privmsg2(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG %s :%s", dest, msg);
}

void ptlink_cmd_serv_notice(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE $%s :%s", dest, msg);
}

void ptlink_cmd_serv_privmsg(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG $%s :%s", dest, msg);
}

/* GLOBOPS */
void ptlink_cmd_global(char *source, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source ? source : ServerName, "GLOBOPS :%s", buf);
}

/* 391 */
void ptlink_cmd_391(char *source, char *timestr)
{
    if (!timestr) {
        return;
    }
    send_cmd(NULL, "391 :%s %s :%s", source, ServerName, timestr);
}

/* 250 */
void ptlink_cmd_250(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "250 %s", buf);
}

/* 307 */
void ptlink_cmd_307(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "307 %s", buf);
}

/* 311 */
void ptlink_cmd_311(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "311 %s", buf);
}

/* 312 */
void ptlink_cmd_312(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "312 %s", buf);
}

/* 317 */
void ptlink_cmd_317(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "317 %s", buf);
}

/* 219 */
void ptlink_cmd_219(char *source, char *letter)
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
void ptlink_cmd_401(char *source, char *who)
{
    if (!source || !who) {
        return;
    }
    send_cmd(ServerName, "401 %s %s :No such service.", source, who);
}

/* 318 */
void ptlink_cmd_318(char *source, char *who)
{
    if (!source || !who) {
        return;
    }

    send_cmd(ServerName, "318 %s %s :End of /WHOIS list.", source, who);
}

/* 242 */
void ptlink_cmd_242(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "242 %s", buf);
}

/* 243 */
void ptlink_cmd_243(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "243 %s", buf);
}

/* 211 */
void ptlink_cmd_211(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "211 %s", buf);
}

void ptlink_cmd_mode(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "MODE %s %s", dest, buf);
}

/*
 NICK %s %d %lu %s %s %s %s %s :%s
	parv[1] = nickname
	parv[2] = hopcount 
	parv[3] = nick TS (nick introduction time)
	parv[4] = umodes
    parv[5] = username
    parv[6] = hostname
    parv[7] = spoofed hostname
    parv[8] = server
    parv[9] = nick info
*/
void ptlink_cmd_nick(char *nick, char *name, char *mode)
{
    EnforceQlinedNick(nick, NULL);
    send_cmd(NULL, "NICK %s 1 %lu %s %s %s %s %s :%s", nick,
             (unsigned long int) time(NULL), mode, ServiceUser,
             ServiceHost, ServiceHost, ServerName, name);
    ptlink_cmd_sqline(nick, "Reserved for services");
}

void ptlink_cmd_kick(char *source, char *chan, char *user, char *buf)
{
    if (!buf) {
        return;
    }

    if (buf) {
        send_cmd(source, "KICK %s %s :%s", chan, user, buf);
    } else {
        send_cmd(source, "KICK %s %s", chan, user);
    }
}

/* QUIT */
void ptlink_cmd_quit(char *source, char *buf)
{
    if (buf) {
        send_cmd(source, "QUIT :%s", buf);
    } else {
        send_cmd(source, "QUIT");
    }
}

void ptlink_cmd_part(char *nick, char *chan, char *buf)
{
    if (buf) {
        send_cmd(nick, "PART %s :%s", chan, buf);
    } else {
        send_cmd(nick, "PART %s", chan);
    }
}

/*
  :%s TOPIC %s %s %lu :%s
	parv[0] = sender prefix
	parv[1] = channel
	parv[2] = topic nick
    parv[3] = topic time
    parv[4] = topic text
*/
void ptlink_cmd_topic(char *whosets, char *chan, char *whosetit,
                      char *topic, time_t when)
{
    send_cmd(whosets, "TOPIC %s %s %lu :%s", chan, whosetit,
             (long int) time(NULL), topic);
}

void ptlink_cmd_vhost_off(User * u)
{
    /* does not support vhosting */
}

void ptlink_cmd_vhost_on(char *nick, char *vIdent, char *vhost)
{
    User *u;

    if (vIdent) {
        send_cmd(s_HostServ, "NEWMASK %s@%s %s", vIdent, vhost, nick);
    } else {
        send_cmd(s_HostServ, "NEWMASK %s %s", vhost, nick);
    }

    if ((u = finduser(nick)))
        u->mode |= UMODE_VH;
}

/* INVITE */
void ptlink_cmd_invite(char *source, char *chan, char *nick)
{
    if (!source || !chan || !nick) {
        return;
    }

    send_cmd(source, "INVITE %s %s", nick, chan);
}

void ptlink_cmd_372(char *source, char *msg)
{
    send_cmd(ServerName, "372 %s :- %s", source, msg);
}

void ptlink_cmd_372_error(char *source)
{
    send_cmd(ServerName, "422 %s :- MOTD file not found!  Please "
             "contact your IRC administrator.", source);
}

void ptlink_cmd_375(char *source)
{
    send_cmd(ServerName, "375 %s :- %s Message of the Day",
             source, ServerName);
}

void ptlink_cmd_376(char *source)
{
    send_cmd(ServerName, "376 %s :End of /MOTD command.", source);
}


void ptlink_set_umode(User * user, int ac, char **av)
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
                if (is_services_admin(user)) {
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
        }
    }
}

int anope_event_ping(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    ptlink_cmd_pong(ac > 1 ? av[1] : ServerName, av[0]);
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

void ptlink_cmd_bot_nick(char *nick, char *user, char *host, char *real,
                         char *modes)
{
    EnforceQlinedNick(nick, s_BotServ);
    send_cmd(NULL, "NICK %s 1 %lu %s %s %s %s %s :%s", nick,
             (unsigned long int) time(NULL), modes, user, host, host,
             ServerName, real);
    ptlink_cmd_sqline(nick, "Reserved for services");

}

void ptlink_cmd_351(char *source)
{
    send_cmd(ServerName, "351 %s Anope-%s %s :%s - %s (%s) -- %s",
             source, version_number, ServerName, ircd->name, version_flags,
             EncModule, version_build);


}

/* SVSHOLD - set */
void ptlink_cmd_svshold(char *nick)
{
    /* Not supported by this IRCD */
}

/* SVSHOLD - release */
void ptlink_cmd_release_svshold(char *nick)
{
    /* Not Supported by this IRCD */
}

/*
:%s UNZLINE %s
	parv[0] = sender 
	parv[1] = zlined host
*/
void ptlink_cmd_unszline(char *mask)
{
    send_cmd(s_OperServ, "UNZLINE %s", mask);
}

/*
:%s ZLINE %s :%s
	parv[0] = sender 
	parv[1] = zlined host
	parv[2] = time
	parv[3] = reason
*/
void ptlink_cmd_szline(char *mask, char *reason, char *whom)
{
    send_cmd(s_OperServ, "ZLINE %s %ld :%s", mask,
             (long int) time(NULL) + 86400 * 2, reason);
}

/*  
:%s UNSXLINE %s
	parv[0] = sender 
	parv[1] = info ban mask
*/
void ptlink_cmd_unsgline(char *mask)
{
    send_cmd(ServerName, "UNSXLINE :%s", mask);
}


/*
 * sxline - add info ban line
 *
 *	parv[0] = sender prefix
 *  	parv[1] = mask length
 *	parv[2] = real name banned mask:reason
 */
void ptlink_cmd_sgline(char *mask, char *reason)
{
    send_cmd(ServerName, "SXLINE %d :%s:%s", (int) strlen(mask), mask,
             reason);
}

/* SVSNICK */
/*
 :%s SVSNICK %s %s
	parv[0] = sender (services client)
	parv[1]	= target client nick
	parv[2] = new nick
  e.g.:	:NickServ SVSNICK Smiler 67455223 _Smiler-
*/
void ptlink_cmd_svsnick(char *source, char *guest, time_t when)
{
    if (!source || !guest) {
        return;
    }
    send_cmd(NULL, "SVSNICK %s %s :%ld", source, guest, (long int) when);
}

void ptlink_cmd_guest_nick(char *nick, char *user, char *host, char *real,
                           char *modes)
{
    send_cmd(NULL, "NICK %s 1 %lu %s %s %s %s %s :%s", nick,
             (unsigned long int) time(NULL), modes, user, host, host,
             ServerName, real);
}


void ptlink_cmd_unban(char *name, char *nick)
{
    /* Not Supported by this IRCD */
}

/* SVSMODE channel modes */

void ptlink_cmd_svsmode_chan(char *name, char *mode, char *nick)
{
    /* Not Supported by this IRCD */
}

void ptlink_cmd_svso(char *source, char *nick, char *flag)
{
    /* Not Supported by this IRCD */
}


/* SVSMODE +d */
/* sent if svid is something weird */
void ptlink_cmd_svid_umode(char *nick, time_t ts)
{
    /* Not Supported by this ircd */
}

/* SVSMODE +d */
/* nc_change was = 1, and there is no na->status */
void ptlink_cmd_nc_change(User * u)
{
    /* Not Supported by this ircd */
}

/* SVSMODE +d */
/* sent if svid is something weird */
void ptlink_cmd_svid_umode2(User * u, char *ts)
{
    common_svsmode(u, "+r", NULL);
}

void ptlink_cmd_svid_umode3(User * u, char *ts)
{
    /* Bahamuts have this extra one, since they can check on even nick changes */
}

/* NICK <newnick>  */
/*
 :%s NICK %s %lu
	parv[0] = old nick
	parv[1] = new nick
	parv[2] = TS (timestamp from user's server when nick changed was received)
*/
void ptlink_cmd_chg_nick(char *oldnick, char *newnick)
{
    if (!oldnick || !newnick) {
        return;
    }

    send_cmd(oldnick, "NICK %s %ld", newnick, (long int) time(NULL));
}

/*
 :%s SVSJOIN %s :%s
  	parv[0] = sender (services client)
	parv[1]	= target client nick
	parv[2] = channels list 
  	:OperServ SVSJOIN Trystan #Admin
*/
void ptlink_cmd_svsjoin(char *source, char *nick, char *chan, char *param)
{
    send_cmd(source, "SVSJOIN %s %s", nick, chan);
}

/*
  :%s SVSPART %s :%s
  	parv[0] = sender (services client)
	parv[1]	= target client nick
	parv[2] = channels list 
  e.g.:	:ChanServ SVSPART mynick 4163321 #Chan1,#Chan2
*/
void ptlink_cmd_svspart(char *source, char *nick, char *chan)
{
    send_cmd(source, "SVSPART %s :%s", nick, chan);
}

void ptlink_cmd_swhois(char *source, char *who, char *mask)
{
    /* not supported */
}

int anope_event_notice(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_pass(char *source, int ac, char **av)
{
    return MOD_CONT;
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

int anope_event_invite(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int ptlink_flood_mode_check(char *value)
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
}

void ptlink_cmd_eob()
{
    /* not supported  */
}

void ptlink_cmd_jupe(char *jserver, char *who, char *reason)
{
    char rbuf[256];

    snprintf(rbuf, sizeof(rbuf), "Juped by %s%s%s", who,
             reason ? ": " : "", reason ? reason : "");

    if (findserver(servlist, jserver))
        ptlink_cmd_squit(jserver, rbuf);
    ptlink_cmd_server(jserver, 1, rbuf);
    new_server(me_server, jserver, rbuf, SERVER_JUPED, NULL);
}

/* GLOBOPS - to handle old WALLOPS */
void ptlink_cmd_global_legacy(char *source, char *fmt)
{
    send_cmd(source ? source : ServerName, "GLOBOPS :%s", fmt);
}

/* 
  1 = valid nick
  0 = nick is in valid
*/
int ptlink_valid_nick(char *nick)
{
    /* no hard coded invalid nicks */
    return 1;
}

/* 
  1 = valid chan
  0 = chan is in valid
*/
int ptlink_valid_chan(char *cahn)
{
    /* no hard coded invalid chan */
    return 1;
}


void ptlink_cmd_ctcp(char *source, char *dest, char *buf)
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
    pmodule_cmd_svsnoop(ptlink_cmd_svsnoop);
    pmodule_cmd_remove_akill(ptlink_cmd_remove_akill);
    pmodule_cmd_topic(ptlink_cmd_topic);
    pmodule_cmd_vhost_off(ptlink_cmd_vhost_off);
    pmodule_cmd_akill(ptlink_cmd_akill);
    pmodule_cmd_svskill(ptlink_cmd_svskill);
    pmodule_cmd_svsmode(ptlink_cmd_svsmode);
    pmodule_cmd_372(ptlink_cmd_372);
    pmodule_cmd_372_error(ptlink_cmd_372_error);
    pmodule_cmd_375(ptlink_cmd_375);
    pmodule_cmd_376(ptlink_cmd_376);
    pmodule_cmd_nick(ptlink_cmd_nick);
    pmodule_cmd_guest_nick(ptlink_cmd_guest_nick);
    pmodule_cmd_mode(ptlink_cmd_mode);
    pmodule_cmd_bot_nick(ptlink_cmd_bot_nick);
    pmodule_cmd_kick(ptlink_cmd_kick);
    pmodule_cmd_notice_ops(ptlink_cmd_notice_ops);
    pmodule_cmd_notice(ptlink_cmd_notice);
    pmodule_cmd_notice2(ptlink_cmd_notice2);
    pmodule_cmd_privmsg(ptlink_cmd_privmsg);
    pmodule_cmd_privmsg2(ptlink_cmd_privmsg2);
    pmodule_cmd_serv_notice(ptlink_cmd_serv_notice);
    pmodule_cmd_serv_privmsg(ptlink_cmd_serv_privmsg);
    pmodule_cmd_bot_chan_mode(ptlink_cmd_bot_chan_mode);
    pmodule_cmd_351(ptlink_cmd_351);
    pmodule_cmd_quit(ptlink_cmd_quit);
    pmodule_cmd_pong(ptlink_cmd_pong);
    pmodule_cmd_join(ptlink_cmd_join);
    pmodule_cmd_unsqline(ptlink_cmd_unsqline);
    pmodule_cmd_invite(ptlink_cmd_invite);
    pmodule_cmd_part(ptlink_cmd_part);
    pmodule_cmd_391(ptlink_cmd_391);
    pmodule_cmd_250(ptlink_cmd_250);
    pmodule_cmd_307(ptlink_cmd_307);
    pmodule_cmd_311(ptlink_cmd_311);
    pmodule_cmd_312(ptlink_cmd_312);
    pmodule_cmd_317(ptlink_cmd_317);
    pmodule_cmd_219(ptlink_cmd_219);
    pmodule_cmd_401(ptlink_cmd_401);
    pmodule_cmd_318(ptlink_cmd_318);
    pmodule_cmd_242(ptlink_cmd_242);
    pmodule_cmd_243(ptlink_cmd_243);
    pmodule_cmd_211(ptlink_cmd_211);
    pmodule_cmd_global(ptlink_cmd_global);
    pmodule_cmd_global_legacy(ptlink_cmd_global_legacy);
    pmodule_cmd_sqline(ptlink_cmd_sqline);
    pmodule_cmd_squit(ptlink_cmd_squit);
    pmodule_cmd_svso(ptlink_cmd_svso);
    pmodule_cmd_chg_nick(ptlink_cmd_chg_nick);
    pmodule_cmd_svsnick(ptlink_cmd_svsnick);
    pmodule_cmd_vhost_on(ptlink_cmd_vhost_on);
    pmodule_cmd_connect(ptlink_cmd_connect);
    pmodule_cmd_bob(ptlink_cmd_bob);
    pmodule_cmd_svshold(ptlink_cmd_svshold);
    pmodule_cmd_release_svshold(ptlink_cmd_release_svshold);
    pmodule_cmd_unsgline(ptlink_cmd_unsgline);
    pmodule_cmd_unszline(ptlink_cmd_unszline);
    pmodule_cmd_szline(ptlink_cmd_szline);
    pmodule_cmd_sgline(ptlink_cmd_sgline);
    pmodule_cmd_unban(ptlink_cmd_unban);
    pmodule_cmd_svsmode_chan(ptlink_cmd_svsmode_chan);
    pmodule_cmd_svid_umode(ptlink_cmd_svid_umode);
    pmodule_cmd_nc_change(ptlink_cmd_nc_change);
    pmodule_cmd_svid_umode2(ptlink_cmd_svid_umode2);
    pmodule_cmd_svid_umode3(ptlink_cmd_svid_umode3);
    pmodule_cmd_svsjoin(ptlink_cmd_svsjoin);
    pmodule_cmd_svspart(ptlink_cmd_svspart);
    pmodule_cmd_swhois(ptlink_cmd_swhois);
    pmodule_cmd_eob(ptlink_cmd_eob);
    pmodule_flood_mode_check(ptlink_flood_mode_check);
    pmodule_cmd_jupe(ptlink_cmd_jupe);
    pmodule_valid_nick(ptlink_valid_nick);
    pmodule_valid_chan(ptlink_valid_chan);
    pmodule_cmd_ctcp(ptlink_cmd_ctcp);
    pmodule_set_umode(ptlink_set_umode);
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


    pmodule_ircd_version("PTlink 6.15.*+");
    pmodule_ircd_cap(myIrcdcap);
    pmodule_ircd_var(myIrcd);
    pmodule_ircd_cbmodeinfos(myCbmodeinfos);
    pmodule_ircd_cumodes(myCumodes);
    pmodule_ircd_flood_mode_char_set("+f");
    pmodule_ircd_flood_mode_char_remove("-f");
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

    return MOD_CONT;
}

/* EOF */
