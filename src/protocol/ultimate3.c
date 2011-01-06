/* Ultimate IRCD 3 functions
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
#include "ultimate3.h"
#include "version.h"

IRCDVar myIrcd[] = {
    {"UltimateIRCd 3.0.*",      /* ircd name */
     "+S",                      /* nickserv mode */
     "+S",                      /* chanserv mode */
     "+S",                      /* memoserv mode */
     "+o",                      /* hostserv mode */
     "+iS",                     /* operserv mode */
     "+S",                      /* botserv mode  */
     "+Sh",                     /* helpserv mode */
     "+iS",                     /* Dev/Null mode */
     "+iS",                     /* Global mode   */
     "+o",                      /* nickserv alias mode */
     "+o",                      /* chanserv alias mode */
     "+o",                      /* memoserv alias mode */
     "+io",                     /* hostserv alias mode */
     "+io",                     /* operserv alias mode */
     "+o",                      /* botserv alias mode  */
     "+h",                      /* helpserv alias mode */
     "+i",                      /* Dev/Null alias mode */
     "+io",                     /* Global alias mode   */
     "+S",                      /* Used by BotServ Bots */
     5,                         /* Chan Max Symbols     */
     "-ilmnpqstRKAO",           /* Modes to Remove */
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
     "-r+d",                    /* Mode on UnReg        */
     "+d",                      /* Mode on Nick Change  */
     1,                         /* Supports SGlines     */
     1,                         /* Supports SQlines     */
     1,                         /* Supports SZlines     */
     1,                         /* Supports Halfop +h   */
     3,                         /* Number of server args */
     0,                         /* Join 2 Set           */
     0,                         /* Join 2 Message       */
     1,                         /* Has exceptions +e    */
     0,                         /* TS Topic Forward     */
     0,                         /* TS Topci Backward    */
     UMODE_p,                   /* Protected Umode      */
     1,                         /* Has Admin            */
     1,                         /* Chan SQlines         */
     0,                         /* Quit on Kill         */
     1,                         /* SVSMODE unban        */
     0,                         /* Has Protect          */
     0,                         /* Reverse              */
     1,                         /* Chan Reg             */
     CMODE_r,                   /* Channel Mode         */
     0,                         /* vidents              */
     0,                         /* svshold              */
     1,                         /* time stamp on mode   */
     1,                         /* NICKIP               */
     0,                         /* O:LINE               */
     1,                         /* UMODE               */
     1,                         /* VHOST ON NICK        */
     0,                         /* Change RealName      */
     CMODE_K,                   /* No Knock             */
     CMODE_A,                   /* Admin Only           */
     DEFAULT_MLOCK,             /* Default MLOCK       */
     UMODE_x,                   /* Vhost Mode           */
     0,                         /* +f                   */
     0,                         /* +L                   */
     0,                         /* +f Mode                          */
     0,                         /* +L Mode                              */
     1,                         /* On nick change check if they could be identified */
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
     1,                         /* Sglines are enforced */
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
     CAPAB_NSJOIN,              /* SJOIN        */
     CAPAB_ZIP,                 /* ZIP          */
     CAPAB_BURST,               /* BURST        */
     CAPAB_TS5,                 /* TS5          */
     0,                         /* TS3          */
     CAPAB_DKEY,                /* DKEY         */
     0,                         /* PT4          */
     0,                         /* SCS          */
     0,                         /* QS           */
     0,                         /* UID          */
     0,                         /* KNOCK        */
     CAPAB_CLIENT,              /* CLIENT       */
     CAPAB_IPV6,                /* IPV6         */
     CAPAB_SSJ5,                /* SSJ5         */
     0,                         /* SN2          */
     0,                         /* TOKEN        */
     0,                         /* VHOST        */
     0,                         /* SSJ3         */
     0,                         /* NICK2        */
     0,                         /* UMODE2       */
     0,                         /* VL           */
     0,                         /* TLKEXT       */
     CAPAB_DODKEY,              /* DODKEY       */
     CAPAB_DOZIP,               /* DOZIP        */
     0, 0, 0}
};

void ultimate3_set_umode(User * user, int ac, char **av)
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
        case 'a':
            if (add && !is_services_oper(user)) {
                common_svsmode(user, "-a", NULL);
                user->mode &= ~UMODE_a;
            }
            break;
        case 'P':
            if (add && !is_services_admin(user)) {
                common_svsmode(user, "-P", NULL);
                user->mode &= ~UMODE_P;
            }
            break;
        case 'Z':
            if (add && !is_services_root(user)) {
                common_svsmode(user, "-Z", NULL);
                user->mode &= ~UMODE_Z;
            }
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
                if (WallOper) {
                    anope_cmd_global(s_OperServ,
                                     "\2%s\2 is now an IRC operator.",
                                     user->nick);
                }
                display_news(user, NEWS_OPER);
                if (is_services_oper(user)) {
                    common_svsmode(user, "+a", NULL);
                    user->mode |= UMODE_a;
                }
                if (is_services_admin(user)) {
                    common_svsmode(user, "+P", NULL);
                    user->mode |= UMODE_P;
                }
                if (is_services_root(user)) {
                    common_svsmode(user, "+Z", NULL);
                    user->mode |= UMODE_Z;
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
    UMODE_D, 0, 0,              /* D E F */
    0, 0, 0,                    /* G H I */
    0, 0, 0,                    /* J K L */
    0, 0, UMODE_0,              /* M N O */
    UMODE_P, 0, UMODE_R,        /* P Q R */
    UMODE_S, 0, 0,              /* S T U */
    0, UMODE_W, 0,              /* V W X */
    0,                          /* Y */
    UMODE_Z,                    /* Z */
    0, 0, 0,                    /* [ \ ] */
    0, 0, 0,                    /* ^ _ ` */
    UMODE_a, 0, 0,              /* a b c */
    UMODE_d, 0, 0,              /* d e f */
    0, UMODE_h, UMODE_i,        /* g h i */
    0, 0, 0,                    /* j k l */
    0, 0, UMODE_o,              /* m n o */
    UMODE_p, 0, UMODE_r,        /* p q r */
    0, 0, 0,                    /* s t u */
    0, UMODE_w, UMODE_x,        /* v w x */
    0,                          /* y */
    0,                          /* z */
    0, 0, 0,                    /* { | } */
    0, 0                        /* ~ � */
};


char myCsmodes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0,
    'a',                        /* (33) ! Channel Admins */
    0, 0, 0,
    'h',                        /* (37) % Channel halfops */
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
    {CMODE_A, CBM_NO_USER_MLOCK, NULL, NULL},
    {0},                        /* B */
    {0},                        /* C */
    {0},                        /* D */
    {0},                        /* E */
    {0},                        /* F */
    {0},                        /* G */
    {0},                        /* H */
    {0},                        /* I */
    {0},                        /* J */
    {CMODE_K, 0, NULL, NULL},   /* K */
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
    {CMODE_c, 0, NULL, NULL},   /* c */
    {0},                        /* d */
    {0},                        /* e */
    {0},                        /* f */
    {0},                        /* g */
    {0},                        /* h */
    {CMODE_i, 0, NULL, NULL},   /* i */
    {0},                        /* j */
    {CMODE_k, 0, chan_set_key, cs_set_key},
    {CMODE_l, CBM_MINUS_NO_ARG, set_limit, cs_set_limit},
    {CMODE_m, 0, NULL, NULL},   /* m */
    {CMODE_n, 0, NULL, NULL},   /* n */
    {0},                        /* o */
    {CMODE_p, 0, NULL, NULL},   /* p */
    {CMODE_q, 0, NULL, NULL},   /* q */
    {CMODE_r, CBM_NO_MLOCK, NULL, NULL},
    {CMODE_s, 0, NULL, NULL},   /* s */
    {CMODE_t, 0, NULL, NULL},   /* t */
    {0},                        /* u */
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
    {'q', CMODE_q, 0, NULL, NULL},
    {'r', CMODE_r, 0, NULL, NULL},
    {'s', CMODE_s, 0, NULL, NULL},
    {'t', CMODE_t, 0, NULL, NULL},
    {'A', CMODE_A, 0, NULL, NULL},
    {'K', CMODE_K, 0, NULL, NULL},
    {'M', CMODE_M, 0, NULL, NULL},
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

/* SVSMODE -b */
void ultimate3_cmd_unban(char *name, char *nick)
{
    ultimate3_cmd_svsmode_chan(name, "-b", nick);
}


/* SVSMODE channel modes */

void ultimate3_cmd_svsmode_chan(char *name, char *mode, char *nick)
{
    if (nick) {
        send_cmd(ServerName, "SVSMODE %s %s %s", name, mode, nick);
    } else {
        send_cmd(ServerName, "SVSMODE %s %s", name, mode);
    }
}

int anope_event_sjoin(char *source, int ac, char **av)
{
    do_sjoin(source, ac, av);
    return MOD_CONT;
}


/*
** NICK - new
**      source  = NULL
**	parv[0] = nickname
**      parv[1] = hopcount
**      parv[2] = timestamp
**      parv[3] = modes
**      parv[4] = username
**      parv[5] = hostname
**      parv[6] = server
**	parv[7] = servicestamp
**      parv[8] = IP
**	parv[9] = info
** NICK - change 
**      source  = oldnick
**	parv[0] = new nickname
**      parv[1] = hopcount
*/
int anope_event_nick(char *source, int ac, char **av)
{
    if (ac != 2) {
        User *user = do_nick(source, av[0], av[4], av[5], av[6], av[9],
                             strtoul(av[2], NULL, 10), strtoul(av[7], NULL,
                                                               0),
                             strtoul(av[8], NULL, 0), "*", NULL);
        if (user)
            anope_set_umode(user, 1, &av[3]);
    } else {
        do_nick(source, av[0], NULL, NULL, NULL, NULL,
                strtoul(av[1], NULL, 10), 0, 0, NULL, NULL);
    }
    return MOD_CONT;
}

int anope_event_sethost(char *source, int ac, char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(av[0]);
    if (!u) {
        if (debug) {
            alog("debug: SETHOST for nonexistent user %s", av[0]);
        }
        return MOD_CONT;
    }

    change_user_host(u, av[1]);
    return MOD_CONT;
}

int anope_event_capab(char *source, int ac, char **av)
{
    capab_parse(ac, av);
    return MOD_CONT;
}

/*
** CLIENT
**      source  = NULL       
**	parv[0] = nickname   Trystan
**      parv[1] = hopcount   1
**      parv[2] = timestamp  1090083810
**      parv[3] = modes      +ix
**	parv[4] = modes ?    +
**      parv[5] = username   Trystan
**      parv[6] = hostname   c-24-2-101-227.client.comcast.net
**      parv[7] = vhost      3223f75b.2b32ee69.client.comcast.net
**	parv[8] = server     WhiteRose.No.Eu.Shadow-Realm.org
**      parv[9] = svid       0 
**	parv[10] = ip         402810339
** 	parv[11] = info      Dreams are answers to questions not yet asked
*/
int anope_event_client(char *source, int ac, char **av)
{
    if (ac != 2) {
        User *user = do_nick(source, av[0], av[5], av[6], av[8], av[11],
                             strtoul(av[2], NULL, 10), strtoul(av[9], NULL,
                                                               0),
                             strtoul(av[10], NULL, 0), av[7], NULL);
        if (user) {
            anope_set_umode(user, 1, &av[3]);
        }
    }
    return MOD_CONT;
}

/* *INDENT-OFF* */
void moduleAddIRCDMsgs(void) {
    Message *m;

    updateProtectDetails("ADMIN","ADMINME","admin","deadmin","AUTOADMIN","+a","-a");

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
    m = createMessage("USER",      anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("WALLOPS",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("WHOIS",     anope_event_whois); addCoreMessage(IRCD,m);
    m = createMessage("AKILL",     anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("GLOBOPS",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("GNOTICE",   anope_event_gnotice); addCoreMessage(IRCD,m);
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
    m = createMessage("SETHOST",   anope_event_sethost); addCoreMessage(IRCD,m);
    m = createMessage("NETINFO",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("GCONNECT",  anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("NETGLOBAL", anope_event_netglobal); addCoreMessage(IRCD,m);
    m = createMessage("CHATOPS",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("NETCTRL",   anope_event_netctrl); addCoreMessage(IRCD,m);
    m = createMessage("CLIENT",	   anope_event_client); addCoreMessage(IRCD,m);
    m = createMessage("SMODE",	   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("ERROR",	   anope_event_error); addCoreMessage(IRCD,m);
    m = createMessage("BURST",	   anope_event_burst); addCoreMessage(IRCD,m);
    m = createMessage("REHASH",    anope_event_rehash); addCoreMessage(IRCD,m);
    m = createMessage("ADMIN",     anope_event_admin); addCoreMessage(IRCD,m);
    m = createMessage("CREDITS",   anope_event_credits); addCoreMessage(IRCD,m);
    m = createMessage("SMODE",     anope_event_mode); addCoreMessage(IRCD,m);
    m = createMessage("EOBURST",   anope_event_eob); addCoreMessage(IRCD,m);
}

/* *INDENT-ON* */


void ultimate3_cmd_sqline(char *mask, char *reason)
{
    if (!mask || !reason) {
        return;
    }

    send_cmd(NULL, "SQLINE %s :%s", mask, reason);
}

void ultimate3_cmd_unsgline(char *mask)
{
    send_cmd(NULL, "UNSGLINE 0 :%s", mask);
}

void ultimate3_cmd_unszline(char *mask)
{
    send_cmd(NULL, "UNSZLINE 0 %s", mask);
}

/* As of alpha27 (one after our offical support szline was removed */
/* quote the changelog ---
   Complete rewrite of the kline/akill/zline system. (s)zlines no longer exist.
   K: lines set on IP addresses without username portions (or *) are treated as Z: lines used to be.
*/
void ultimate3_cmd_szline(char *mask, char *reason, char *whom)
{
    send_cmd(NULL, "AKILL %s * %d %s %ld :%s", mask, 86400 * 2, whom,
             (long int) time(NULL), reason);
    /* leaving this in here in case some legacy user asks for it */
    /* send_cmd(NULL, "SZLINE %s :%s", mask, reason); */
}

void ultimate3_cmd_svsnoop(char *server, int set)
{
    send_cmd(NULL, "SVSNOOP %s %s", server, (set ? "+" : "-"));
}

void ultimate3_cmd_svsadmin(char *server, int set)
{
    ultimate3_cmd_svsnoop(server, set);
}

void ultimate3_cmd_sgline(char *mask, char *reason)
{
    send_cmd(NULL, "SGLINE %d :%s:%s", (int)strlen(mask), mask, reason);
}

void ultimate3_cmd_remove_akill(char *user, char *host)
{
    send_cmd(NULL, "RAKILL %s %s", host, user);
}

void ultimate3_cmd_vhost_off(User * u)
{
    common_svsmode(u, "-x", NULL);
    notice_lang(s_HostServ, u, HOST_OFF_UNREAL, u->nick, ircd->vhostchar);
}

void ultimate3_cmd_vhost_on(char *nick, char *vIdent, char *vhost)
{
    send_cmd(s_HostServ, "SVSMODE %s +x", nick);
    send_cmd(ServerName, "SETHOST %s %s", nick, vhost);
}

void ultimate3_cmd_join(char *user, char *channel, time_t chantime)
{
    send_cmd(user, "SJOIN %ld %s", (long int) chantime, channel);
}

void ultimate3_cmd_akill(char *user, char *host, char *who, time_t when,
                         time_t expires, char *reason)
{
    send_cmd(NULL, "AKILL %s %s %d %s %ld :%s", host, user, 86400 * 2, who,
             (long int) time(NULL), reason);
}

void ultimate3_cmd_svskill(char *source, char *user, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "SVSKILL %s :%s", user, buf);
}


void ultimate3_cmd_svsmode(User * u, int ac, char **av)
{
    send_cmd(ServerName, "SVSMODE %s %ld %s%s%s", u->nick,
             (long int) u->timestamp, av[0], (ac == 2 ? " " : ""),
             (ac == 2 ? av[1] : ""));
}

void anope_squit(char *servname, char *message)
{
    send_cmd(servname, "SQUIT %s :%s", servname, message);
}

void anope_pong(char *servname)
{
    send_cmd(servname, "PONG %s", servname);
}

/* Events */

int anope_event_ping(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    ultimate3_cmd_pong(ac > 1 ? av[1] : ServerName, av[0]);
    return MOD_CONT;
}

int anope_event_436(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;

    m_nickcoll(av[0]);
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

int anope_event_setname(char *source, int ac, char **av)
{
    User *u;

    if (ac != 1)
        return MOD_CONT;

    u = finduser(source);
    if (!u) {
        if (debug) {
            alog("debug: SETNAME for nonexistent user %s", source);
        }
        return MOD_CONT;
    }

    change_user_realname(u, av[0]);
    return MOD_CONT;
}

int anope_event_chgname(char *source, int ac, char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(av[0]);
    if (!u) {
        if (debug) {
            alog("debug: CHGNAME for nonexistent user %s", av[0]);
        }
        return MOD_CONT;
    }

    change_user_realname(u, av[1]);
    return MOD_CONT;
}

int anope_event_setident(char *source, int ac, char **av)
{
    User *u;

    if (ac != 1)
        return MOD_CONT;

    u = finduser(source);
    if (!u) {
        if (debug) {
            alog("debug: SETIDENT for nonexistent user %s", source);
        }
        return MOD_CONT;
    }

    change_user_username(u, av[0]);
    return MOD_CONT;
}
int anope_event_chgident(char *source, int ac, char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(av[0]);
    if (!u) {
        if (debug) {
            alog("debug: CHGIDENT for nonexistent user %s", av[0]);
        }
        return MOD_CONT;
    }

    change_user_username(u, av[1]);
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

void ultimate3_cmd_topic(char *whosets, char *chan, char *whosetit,
                         char *topic, time_t when)
{
    send_cmd(whosets, "TOPIC %s %s %lu :%s", chan, whosetit,
             (unsigned long int) when, topic);
}

void ultimate3_cmd_372(char *source, char *msg)
{
    send_cmd(ServerName, "372 %s :- %s", source, msg);
}

void ultimate3_cmd_372_error(char *source)
{
    send_cmd(ServerName, "422 %s :- MOTD file not found!  Please "
             "contact your IRC administrator.", source);
}

void ultimate3_cmd_375(char *source)
{
    send_cmd(ServerName, "375 %s :- %s Message of the Day",
             source, ServerName);
}

void ultimate3_cmd_376(char *source)
{
    send_cmd(ServerName, "376 %s :End of /MOTD command.", source);
}

void ultimate3_cmd_nick(char *nick, char *name, char *modes)
{
    EnforceQlinedNick(nick, NULL);
    send_cmd(NULL, "CLIENT %s 1 %ld %s + %s %s * %s 0 0 :%s", nick,
             (long int) time(NULL), modes, ServiceUser, ServiceHost,
             ServerName, name);
    ultimate3_cmd_sqline(nick, "Reserved for services");
}

void ultimate3_cmd_guest_nick(char *nick, char *user, char *host,
                              char *real, char *modes)
{
    send_cmd(NULL, "CLIENT %s 1 %ld %s + %s %s * %s 0 0 :%s", nick,
             (long int) time(NULL), modes, user, host, ServerName, real);
}

void ultimate3_cmd_mode(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "MODE %s %s", dest, buf);
}

void ultimate3_cmd_bot_nick(char *nick, char *user, char *host, char *real,
                            char *modes)
{
    EnforceQlinedNick(nick, s_BotServ);
    send_cmd(NULL, "CLIENT %s 1 %ld %s + %s %s * %s 0 0 :%s", nick,
             (long int) time(NULL), modes, user, host, ServerName, real);
    ultimate3_cmd_sqline(nick, "Reserved for services");
}

void ultimate3_cmd_kick(char *source, char *chan, char *user, char *buf)
{
    if (buf) {
        send_cmd(source, "KICK %s %s :%s", chan, user, buf);
    } else {
        send_cmd(source, "KICK %s %s", chan, user);
    }
}

void ultimate3_cmd_notice_ops(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "NOTICE @%s :%s", dest, buf);
}


void ultimate3_cmd_notice(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    if (NSDefFlags & NI_MSG) {
        ultimate3_cmd_privmsg2(source, dest, buf);
    } else {
        send_cmd(source, "NOTICE %s :%s", dest, buf);
    }
}

void ultimate3_cmd_notice2(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE %s :%s", dest, msg);
}

void ultimate3_cmd_privmsg(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "PRIVMSG %s :%s", dest, buf);
}

void ultimate3_cmd_privmsg2(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG %s :%s", dest, msg);
}

void ultimate3_cmd_serv_notice(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE $%s :%s", dest, msg);
}

void ultimate3_cmd_serv_privmsg(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG $%s :%s", dest, msg);
}

void ultimate3_cmd_bot_chan_mode(char *nick, char *chan)
{
    anope_cmd_mode(nick, chan, "%s %s %s", ircd->botchanumode, nick, nick);
}

void ultimate3_cmd_351(char *source)
{
    send_cmd(ServerName, "351 %s Anope-%s %s :%s - %s (%s) -- %s",
             source, version_number, ServerName, ircd->name, version_flags,
             EncModule,version_build);

}

/* QUIT */
void ultimate3_cmd_quit(char *source, char *buf)
{
    if (!buf) {
        return;
    }

    if (buf) {
        send_cmd(source, "QUIT :%s", buf);
    } else {
        send_cmd(source, "QUIT");
    }
}

/* PROTOCTL */
void ultimate3_cmd_capab()
{
    send_cmd(NULL,
             "CAPAB TS5 NOQUIT SSJ5 BURST UNCONNECT TSMODE NICKIP CLIENT");
}

/* PASS */
void ultimate3_cmd_pass(char *pass)
{
    send_cmd(NULL, "PASS %s :TS", pass);
}

/* SERVER name hop descript */
/* Unreal 3.2 actually sends some info about itself in the descript area */
void ultimate3_cmd_server(char *servname, int hop, char *descript)
{
    send_cmd(NULL, "SERVER %s %d :%s", servname, hop, descript);
}

/* PONG */
void ultimate3_cmd_pong(char *servname, char *who)
{
    send_cmd(servname, "PONG %s", who);
}

/* UNSQLINE */
void ultimate3_cmd_unsqline(char *user)
{
    if (!user) {
        return;
    }
    send_cmd(NULL, "UNSQLINE %s", user);
}

/* CHGHOST */
void ultimate3_cmd_chghost(char *nick, char *vhost)
{
    if (!nick || !vhost) {
        return;
    }
    send_cmd(ServerName, "CHGHOST %s %s", nick, vhost);
}

/* CHGIDENT */
void ultimate3_cmd_chgident(char *nick, char *vIdent)
{
    if (!nick || !vIdent) {
        return;
    }
    send_cmd(ServerName, "CHGIDENT %s %s", nick, vIdent);
}

/* INVITE */
void ultimate3_cmd_invite(char *source, char *chan, char *nick)
{
    if (!source || !chan || !nick) {
        return;
    }

    send_cmd(source, "INVITE %s %s", nick, chan);
}

/* PART */
void ultimate3_cmd_part(char *nick, char *chan, char *buf)
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

/* 391 */
void ultimate3_cmd_391(char *source, char *timestr)
{
    if (!timestr) {
        return;
    }
    send_cmd(NULL, "391 :%s %s :%s", source, ServerName, timestr);
}

/* 250 */
void ultimate3_cmd_250(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "250 %s", buf);
}

/* 307 */
void ultimate3_cmd_307(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "307 %s", buf);
}

/* 311 */
void ultimate3_cmd_311(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "311 %s", buf);
}

/* 312 */
void ultimate3_cmd_312(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "312 %s", buf);
}

/* 317 */
void ultimate3_cmd_317(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "317 %s", buf);
}

/* 219 */
void ultimate3_cmd_219(char *source, char *letter)
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
void ultimate3_cmd_401(char *source, char *who)
{
    if (!source || !who) {
        return;
    }
    send_cmd(ServerName, "401 %s %s :No such service.", source, who);
}

/* 318 */
void ultimate3_cmd_318(char *source, char *who)
{
    if (!source || !who) {
        return;
    }

    send_cmd(ServerName, "318 %s %s :End of /WHOIS list.", source, who);
}

/* 242 */
void ultimate3_cmd_242(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "242 %s", buf);
}

/* 243 */
void ultimate3_cmd_243(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "243 %s", buf);
}

/* 211 */
void ultimate3_cmd_211(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "211 %s", buf);
}

/* GLOBOPS */
void ultimate3_cmd_global(char *source, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source ? source : ServerName, "GLOBOPS :%s", buf);
}

/* SQUIT */
void ultimate3_cmd_squit(char *servname, char *message)
{
    if (!servname || !message) {
        return;
    }

    send_cmd(NULL, "SQUIT %s :%s", servname, message);
}

/* SVSO */
void ultimate3_cmd_svso(char *source, char *nick, char *flag)
{
    if (!source || !nick || !flag) {
        return;
    }

    send_cmd(source, "SVSO %s %s", nick, flag);
}

/* NICK <newnick>  */
void ultimate3_cmd_chg_nick(char *oldnick, char *newnick)
{
    if (!oldnick || !newnick) {
        return;
    }

    send_cmd(oldnick, "NICK %s", newnick);
}

/* SVSNICK */
void ultimate3_cmd_svsnick(char *source, char *guest, time_t when)
{
    if (!source || !guest) {
        return;
    }
    send_cmd(NULL, "SVSNICK %s %s :%ld", source, guest, (long int) when);
}

/* Functions that use serval cmd functions */
/*
 * SVINFO 
 *       parv[0] = sender prefix 
 *       parv[1] = TS_CURRENT for the server 
 *       parv[2] = TS_MIN for the server 
 *       parv[3] = server is standalone or connected to non-TS only 
 *       parv[4] = server's idea of UTC time
 */
void ultimate3_cmd_svinfo()
{
    send_cmd(NULL, "SVINFO 5 3 0 :%ld", (long int) time(NULL));
}

void ultimate3_cmd_burst()
{
    send_cmd(NULL, "BURST");
}


void ultimate3_cmd_connect(int servernum)
{
    me_server =
        new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);

    if (servernum == 1) {
        ultimate3_cmd_pass(RemotePassword);
    }
    if (servernum == 2) {
        ultimate3_cmd_pass(RemotePassword2);
    }
    if (servernum == 3) {
        ultimate3_cmd_pass(RemotePassword3);
    }
    ultimate3_cmd_capab();
    ultimate3_cmd_server(ServerName, 1, ServerDesc);
    ultimate3_cmd_svinfo();
    ultimate3_cmd_burst();
}

void ultimate3_cmd_bob()
{
    /* Not used */
}

/* SVSHOLD - set */
void ultimate3_cmd_svshold(char *nick)
{
    /* Not supported by this IRCD */
}

/* SVSHOLD - release */
void ultimate3_cmd_release_svshold(char *nick)
{
    /* Not Supported by this IRCD */
}

/* SVSMODE +d */
/* sent if svid is something weird */
void ultimate3_cmd_svid_umode(char *nick, time_t ts)
{
    send_cmd(ServerName, "SVSMODE %s %lu +d 1", nick,
             (unsigned long int) ts);
}

/* SVSMODE +d */
/* nc_change was = 1, and there is no na->status */
void ultimate3_cmd_nc_change(User * u)
{
    common_svsmode(u, "+d", "1");
}

/* SVSMODE +d */
void ultimate3_cmd_svid_umode2(User * u, char *ts)
{
    /* not used by bahamut ircds */
}

void ultimate3_cmd_svid_umode3(User * u, char *ts)
{
    if (u->svid != u->timestamp) {
        common_svsmode(u, "+rd", ts);
    } else {
        common_svsmode(u, "+r", NULL);
    }

}

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

int anope_event_gnotice(char *source, int ac, char **av)
{
    /* currently not used but removes the message : unknown message from server */
    return MOD_CONT;
}

int anope_event_netctrl(char *source, int ac, char **av)
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

void ultimate3_cmd_svsjoin(char *source, char *nick, char *chan, char *param)
{
    /* Not Supported by this IRCD */
}

void ultimate3_cmd_svspart(char *source, char *nick, char *chan)
{
    /* Not Supported by this IRCD */
}

void ultimate3_cmd_swhois(char *source, char *who, char *mask)
{
    /* not supported */
}


void ultimate3_cmd_eob()
{
    send_cmd(NULL, "BURST 0");
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


int anope_event_eob(char *source, int ac, char **av)
{
    Server *s;

    if (ac == 1) {
        s = findserver(servlist, av[0]);
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


int anope_event_burst(char *source, int ac, char **av)
{
    Server *s;
    s = findserver(servlist, source);
    if (!ac) {
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

int anope_event_netglobal(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_invite(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int ultiamte3_flood_mode_check(char *value)
{
    return 0;
}

void ultimate3_cmd_jupe(char *jserver, char *who, char *reason)
{
    char rbuf[256];

    snprintf(rbuf, sizeof(rbuf), "Juped by %s%s%s", who,
             reason ? ": " : "", reason ? reason : "");

    if (findserver(servlist, jserver))
        ultimate3_cmd_squit(jserver, rbuf);
    ultimate3_cmd_server(jserver, 2, rbuf);
    new_server(me_server, jserver, rbuf, SERVER_JUPED, NULL);
}

/* GLOBOPS - to handle old WALLOPS */
void ultimate3_cmd_global_legacy(char *source, char *fmt)
{
    send_cmd(source ? source : ServerName, "GLOBOPS :%s", fmt);
}

/* 
  1 = valid nick
  0 = nick is in valid
*/
int ultiamte3_valid_nick(char *nick)
{
    /* no hard coded invalid nicks */
    return 1;
}

/* 
  1 = valid chan
  0 = chan is in valid
*/
int ultiamte3_valid_chan(char *chan)
{
    /* no hard coded invalid chans */
    return 1;
}


void ultimate3_cmd_ctcp(char *source, char *dest, char *buf)
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
    pmodule_cmd_svsnoop(ultimate3_cmd_svsnoop);
    pmodule_cmd_remove_akill(ultimate3_cmd_remove_akill);
    pmodule_cmd_topic(ultimate3_cmd_topic);
    pmodule_cmd_vhost_off(ultimate3_cmd_vhost_off);
    pmodule_cmd_akill(ultimate3_cmd_akill);
    pmodule_cmd_svskill(ultimate3_cmd_svskill);
    pmodule_cmd_svsmode(ultimate3_cmd_svsmode);
    pmodule_cmd_372(ultimate3_cmd_372);
    pmodule_cmd_372_error(ultimate3_cmd_372_error);
    pmodule_cmd_375(ultimate3_cmd_375);
    pmodule_cmd_376(ultimate3_cmd_376);
    pmodule_cmd_nick(ultimate3_cmd_nick);
    pmodule_cmd_guest_nick(ultimate3_cmd_guest_nick);
    pmodule_cmd_mode(ultimate3_cmd_mode);
    pmodule_cmd_bot_nick(ultimate3_cmd_bot_nick);
    pmodule_cmd_kick(ultimate3_cmd_kick);
    pmodule_cmd_notice_ops(ultimate3_cmd_notice_ops);
    pmodule_cmd_notice(ultimate3_cmd_notice);
    pmodule_cmd_notice2(ultimate3_cmd_notice2);
    pmodule_cmd_privmsg(ultimate3_cmd_privmsg);
    pmodule_cmd_privmsg2(ultimate3_cmd_privmsg2);
    pmodule_cmd_serv_notice(ultimate3_cmd_serv_notice);
    pmodule_cmd_serv_privmsg(ultimate3_cmd_serv_privmsg);
    pmodule_cmd_bot_chan_mode(ultimate3_cmd_bot_chan_mode);
    pmodule_cmd_351(ultimate3_cmd_351);
    pmodule_cmd_quit(ultimate3_cmd_quit);
    pmodule_cmd_pong(ultimate3_cmd_pong);
    pmodule_cmd_join(ultimate3_cmd_join);
    pmodule_cmd_unsqline(ultimate3_cmd_unsqline);
    pmodule_cmd_invite(ultimate3_cmd_invite);
    pmodule_cmd_part(ultimate3_cmd_part);
    pmodule_cmd_391(ultimate3_cmd_391);
    pmodule_cmd_250(ultimate3_cmd_250);
    pmodule_cmd_307(ultimate3_cmd_307);
    pmodule_cmd_311(ultimate3_cmd_311);
    pmodule_cmd_312(ultimate3_cmd_312);
    pmodule_cmd_317(ultimate3_cmd_317);
    pmodule_cmd_219(ultimate3_cmd_219);
    pmodule_cmd_401(ultimate3_cmd_401);
    pmodule_cmd_318(ultimate3_cmd_318);
    pmodule_cmd_242(ultimate3_cmd_242);
    pmodule_cmd_243(ultimate3_cmd_243);
    pmodule_cmd_211(ultimate3_cmd_211);
    pmodule_cmd_global(ultimate3_cmd_global);
    pmodule_cmd_global_legacy(ultimate3_cmd_global_legacy);
    pmodule_cmd_sqline(ultimate3_cmd_sqline);
    pmodule_cmd_squit(ultimate3_cmd_squit);
    pmodule_cmd_svso(ultimate3_cmd_svso);
    pmodule_cmd_chg_nick(ultimate3_cmd_chg_nick);
    pmodule_cmd_svsnick(ultimate3_cmd_svsnick);
    pmodule_cmd_vhost_on(ultimate3_cmd_vhost_on);
    pmodule_cmd_connect(ultimate3_cmd_connect);
    pmodule_cmd_bob(ultimate3_cmd_bob);
    pmodule_cmd_svshold(ultimate3_cmd_svshold);
    pmodule_cmd_release_svshold(ultimate3_cmd_release_svshold);
    pmodule_cmd_unsgline(ultimate3_cmd_unsgline);
    pmodule_cmd_unszline(ultimate3_cmd_unszline);
    pmodule_cmd_szline(ultimate3_cmd_szline);
    pmodule_cmd_sgline(ultimate3_cmd_sgline);
    pmodule_cmd_unban(ultimate3_cmd_unban);
    pmodule_cmd_svsmode_chan(ultimate3_cmd_svsmode_chan);
    pmodule_cmd_svid_umode(ultimate3_cmd_svid_umode);
    pmodule_cmd_nc_change(ultimate3_cmd_nc_change);
    pmodule_cmd_svid_umode2(ultimate3_cmd_svid_umode2);
    pmodule_cmd_svid_umode3(ultimate3_cmd_svid_umode3);
    pmodule_cmd_svsjoin(ultimate3_cmd_svsjoin);
    pmodule_cmd_svspart(ultimate3_cmd_svspart);
    pmodule_cmd_swhois(ultimate3_cmd_swhois);
    pmodule_cmd_eob(ultimate3_cmd_eob);
    pmodule_flood_mode_check(ultiamte3_flood_mode_check);
    pmodule_cmd_jupe(ultimate3_cmd_jupe);
    pmodule_valid_nick(ultiamte3_valid_nick);
    pmodule_valid_chan(ultiamte3_valid_chan);
    pmodule_cmd_ctcp(ultimate3_cmd_ctcp);
    pmodule_set_umode(ultimate3_set_umode);
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

    pmodule_ircd_version("UltimateIRCd 3.0.0.a26+");
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

    return MOD_CONT;
}
