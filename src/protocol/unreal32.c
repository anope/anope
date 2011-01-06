/* Unreal IRCD 3.2.x functions
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
#include "unreal32.h"
#include "version.h"

IRCDVar myIrcd[] = {
    {"UnrealIRCd 3.2.x",        /* ircd name */
     "+oS",                     /* nickserv mode */
     "+oS",                     /* chanserv mode */
     "+oS",                     /* memoserv mode */
     "+oS",                     /* hostserv mode */
     "+ioS",                    /* operserv mode */
     "+oS",                     /* botserv mode  */
     "+oS",                     /* helpserv mode */
     "+iS",                     /* Dev/Null mode */
     "+ioS",                    /* Global mode   */
     "+oS",                     /* nickserv alias mode */
     "+oS",                     /* chanserv alias mode */
     "+oS",                     /* memoserv alias mode */
     "+ioS",                    /* hostserv alias mode */
     "+ioS",                    /* operserv alias mode */
     "+oS",                     /* botserv alias mode  */
     "+oS",                     /* helpserv alias mode */
     "+iS",                     /* Dev/Null alias mode */
     "+ioS",                    /* Global alias mode   */
     "+qS",                     /* Used by BotServ Bots */
     5,                         /* Chan Max Symbols     */
     "-cijlmnpstuzACGKMNOQRSTV",     /* Modes to Remove */
     "+ao",                     /* Channel Umode used by Botserv bots */
     1,                         /* SVSNICK */
     1,                         /* Vhost  */
     1,                         /* Has Owner */
     "+q",                      /* Mode to set for an owner */
     "-q",                      /* Mode to unset for an owner */
     "+a",                      /* Mode to set for channel admin */
     "-a",                      /* Mode to unset for channel admin */
     "+rd",                     /* Mode On Reg          */
     NULL,                      /* Mode on ID for Roots */
     NULL,                      /* Mode on ID for Admins */
     NULL,                      /* Mode on ID for Opers */
     "-r+d",                    /* Mode on UnReg        */
     "-r+d",                    /* Mode on Nick Change  */
     1,                         /* Supports SGlines     */
     1,                         /* Supports SQlines     */
     1,                         /* Supports SZlines     */
     1,                         /* Supports Halfop +h   */
     3,                         /* Number of server args */
     0,                         /* Join 2 Set           */
     0,                         /* Join 2 Message       */
     1,                         /* Has exceptions +e    */
     1,                         /* TS Topic Forward     */
     0,                         /* TS Topci Backward    */
     UMODE_S,                   /* Protected Umode      */
     0,                         /* Has Admin            */
     0,                         /* Chan SQlines         */
     0,                         /* Quit on Kill         */
     1,                         /* SVSMODE unban        */
     1,                         /* Has Protect          */
     1,                         /* Reverse              */
     1,                         /* Chan Reg             */
     CMODE_r,                   /* Channel Mode         */
     1,                         /* vidents              */
     1,                         /* svshold              */
     1,                         /* time stamp on mode   */
     1,                         /* NICKIP               */
     1,                         /* O:LINE               */
     1,                         /* UMODE               */
     1,                         /* VHOST ON NICK        */
     1,                         /* Change RealName      */
     CMODE_K,                   /* No Knock             */
     CMODE_A,                   /* Admin Only           */
     DEFAULT_MLOCK,             /* Default MLOCK       */
     UMODE_x,                   /* Vhost Mode           */
     1,                         /* +f                   */
     1,                         /* +L                   */
     CMODE_f,                   /* +f Mode                          */
     CMODE_L,                   /* +L Mode                          */
     0,                         /* On nick change check if they could be identified */
     1,                         /* No Knock requires +i */
     NULL,                      /* CAPAB Chan Modes             */
     1,                         /* We support Unreal TOKENS */
     0,                         /* TOKENS are CASE Sensitive */
     1,                         /* TIME STAMPS are BASE64 */
     1,                         /* +I support */
     '&',                       /* SJOIN ban char */
     '\"',                      /* SJOIN except char */
     '\'',                      /* SJOIN invite char */
     1,                         /* Can remove User Channel Modes with SVSMODE */
     0,                         /* Sglines are not enforced until user reconnects */
     "x",                       /* vhost char */
     0,                         /* ts6 */
     1,                         /* support helper umode */
     0,                         /* p10 */
     NULL,                      /* character set */
     1,                         /* reports sync state */
     0,                         /* CIDR channelbans */
     1,                         /* +j */
     CMODE_j,                   /* +j Mode */
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
     CAPAB_NICKIP,              /* NICKIP       */
     0,                         /* SJOIN        */
     CAPAB_ZIP,                 /* ZIP          */
     0,                         /* BURST        */
     0,                         /* TS5          */
     0,                         /* TS3          */
     0,                         /* DKEY         */
     0,                         /* PT4          */
     0,                         /* SCS          */
     0,                         /* QS           */
     0,                         /* UID          */
     0,                         /* KNOCK        */
     0,                         /* CLIENT       */
     0,                         /* IPV6         */
     0,                         /* SSJ5         */
     0,                         /* SN2          */
     CAPAB_TOKEN,               /* TOKEN        */
     0,                         /* VHOST        */
     CAPAB_SSJ3,                /* SSJ3         */
     CAPAB_NICK2,               /* NICK2        */
     CAPAB_UMODE2,              /* UMODE2       */
     CAPAB_VL,                  /* VL           */
     CAPAB_TLKEXT,              /* TLKEXT       */
     0,                         /* DODKEY       */
     0,                         /* DOZIP        */
     CAPAB_CHANMODE,            /* CHANMODE             */
     CAPAB_SJB64,
     CAPAB_NICKCHARS,
     }
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
    UMODE_A, UMODE_B, UMODE_C,  /* A B C */
    0, 0, 0,                    /* D E F */
    UMODE_G, UMODE_H, 0,        /* G H I */
    0, 0, 0,                    /* J K L */
    0, UMODE_N, UMODE_O,        /* M N O */
    0, 0, UMODE_R,              /* P Q R */
    UMODE_S, UMODE_T, 0,        /* S T U */
    UMODE_V, UMODE_W, 0,        /* V W X */
    0,                          /* Y */
    0,                          /* Z */
    0, 0, 0,                    /* [ \ ] */
    0, 0, 0,                    /* ^ _ ` */
    UMODE_a, 0, 0,              /* a b c */
    UMODE_d, 0, 0,              /* d e f */
    UMODE_g, UMODE_h, UMODE_i,  /* g h i */
    0, 0, 0,                    /* j k l */
    0, 0, UMODE_o,              /* m n o */
    UMODE_p, UMODE_q, UMODE_r,  /* p q r */
    UMODE_s, UMODE_t, 0,        /* s t u */
    UMODE_v, UMODE_w, UMODE_x,  /* v w x */
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
    'h',                        /* (37) % Channel halfops */
    'b',                        /* (38) & bans */
    0, 0, 0,
    'q',

    'v', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    'o', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'a', 0
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
    {add_invite, del_invite},   /* I */
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
    {add_ban, del_ban},         /* b */
    {NULL}, {NULL},             /* cd */
    {add_exception, del_exception},
    {NULL}, {NULL},
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
    {CMODE_C, 0, NULL, NULL},   /* C */
    {0},                        /* D */
    {0},                        /* E */
    {0},                        /* F */
    {CMODE_G, 0, NULL, NULL},   /* G */
    {0},                        /* H */
    {0},                        /* I */
    {0},                        /* J */
    {CMODE_K, 0, NULL, NULL},   /* K */
    {CMODE_L, 0, set_redirect, cs_set_redirect},
    {CMODE_M, 0, NULL, NULL},   /* M */
    {CMODE_N, 0, NULL, NULL},   /* N */
    {CMODE_O, CBM_NO_USER_MLOCK, NULL, NULL},
    {0},                        /* P */
    {CMODE_Q, 0, NULL, NULL},   /* Q */
    {CMODE_R, 0, NULL, NULL},   /* R */
    {CMODE_S, 0, NULL, NULL},   /* S */
    {CMODE_T, 0, NULL, NULL},   /* T */
    {0},                        /* U */
    {CMODE_V, 0, NULL, NULL},   /* V */
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
    {CMODE_f, 0, set_flood, cs_set_flood},
    {0},                        /* g */
    {0},                        /* h */
    {CMODE_i, 0, NULL, NULL},
    {CMODE_j, CBM_MINUS_NO_ARG | CBM_NO_MLOCK, chan_set_throttle, cs_set_throttle}, /* j */
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
    {CMODE_u, 0, NULL, NULL},
    {0},                        /* v */
    {0},                        /* w */
    {0},                        /* x */
    {0},                        /* y */
    {CMODE_z, 0, NULL, NULL},
    {0}, {0}, {0}, {0}
};

CBModeInfo myCbmodeinfos[] = {
    {'c', CMODE_c, 0, NULL, NULL},
    {'f', CMODE_f, 0, get_flood, cs_get_flood},
    {'i', CMODE_i, 0, NULL, NULL},
    {'j', CMODE_j, 0, get_throttle, cs_get_throttle},
    {'k', CMODE_k, 0, get_key, cs_get_key},
    {'l', CMODE_l, CBM_MINUS_NO_ARG, get_limit, cs_get_limit},
    {'m', CMODE_m, 0, NULL, NULL},
    {'n', CMODE_n, 0, NULL, NULL},
    {'p', CMODE_p, 0, NULL, NULL},
    {'r', CMODE_r, 0, NULL, NULL},
    {'s', CMODE_s, 0, NULL, NULL},
    {'t', CMODE_t, 0, NULL, NULL},
    {'u', CMODE_u, 0, NULL, NULL},
    {'z', CMODE_z, 0, NULL, NULL},
    {'A', CMODE_A, 0, NULL, NULL},
    {'C', CMODE_C, 0, NULL, NULL},
    {'G', CMODE_G, 0, NULL, NULL},
    {'K', CMODE_K, 0, NULL, NULL},
    {'L', CMODE_L, 0, get_redirect, cs_get_redirect},
    {'M', CMODE_M, 0, NULL, NULL},
    {'N', CMODE_N, 0, NULL, NULL},
    {'O', CMODE_O, 0, NULL, NULL},
    {'Q', CMODE_Q, 0, NULL, NULL},
    {'R', CMODE_R, 0, NULL, NULL},
    {'S', CMODE_S, 0, NULL, NULL},
    {'T', CMODE_T, 0, NULL, NULL},
    {'V', CMODE_V, 0, NULL, NULL},
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

    {CUS_PROTECT, CUF_PROTECT_BOTSERV, check_valid_op},
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
    {CUS_OWNER, 0, check_valid_op},
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


void unreal_set_umode(User * user, int ac, char **av)
{
    int add = 1;                /* 1 if adding modes, 0 if deleting */
    char *modes = av[0];

    ac--;

    if (!user || !modes) {
        /* Prevent NULLs from doing bad things */
        return;
    }

    if (debug)
        alog("debug: Changing mode for %s to %s", user->nick, modes);

    while (*modes) {
        uint32 backup = user->mode;

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
            if (ac <= 0) {
                break;
            }
            if (isdigit(*av[1])) {
                user->svid = strtoul(av[1], NULL, 0);
                user->mode = backup;  /* Ugly fix, but should do the job ~ Viper */
            }
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
            } else {
                opcnt--;
            }
            break;
        case 'a':
            if (UnRestrictSAdmin) {
                break;
            }
            if (add && !is_services_admin(user)) {
                common_svsmode(user, "-a", NULL);
                user->mode &= ~UMODE_a;
            }
            break;
        case 'r':
            if (add && !nick_identified(user)) {
                common_svsmode(user, "-r", NULL);
                user->mode &= ~UMODE_r;
            }
            break;
        case 'x':
            if (!add) {
                if (user->vhost)
                    free(user->vhost);
                user->vhost = NULL;
            }
            update_host(user);
            break;
        }
    }
}




/* Event: PROTOCTL */
int anope_event_capab(char *source, int ac, char **av)
{
    capab_parse(ac, av);
    return MOD_CONT;
}

/* SVSNOOP */
void unreal_cmd_svsnoop(char *server, int set)
{
    send_cmd(NULL, "%s %s %s", send_token("SVSNOOP", "f"), server,
             (set ? "+" : "-"));
}

void unreal_cmd_svsadmin(char *server, int set)
{
    unreal_cmd_svsnoop(server, set);
}

void unreal_cmd_remove_akill(char *user, char *host)
{
    send_cmd(NULL, "%s - G %s %s %s", send_token("TKL", "BD"), user, host,
             s_OperServ);
}

void unreal_cmd_topic(char *whosets, char *chan, char *whosetit,
                      char *topic, time_t when)
{
    send_cmd(whosets, "%s %s %s %lu :%s", send_token("TOPIC", ")"), chan,
             whosetit, (unsigned long int) when, topic);
}

void unreal_cmd_vhost_off(User * u)
{
    common_svsmode(u, "-xt", NULL);
    common_svsmode(u, "+x", NULL);

    notice_lang(s_HostServ, u, HOST_OFF);
}

void unreal_cmd_akill(char *user, char *host, char *who, time_t when,
                      time_t expires, char *reason)
{
    send_cmd(NULL, "%s + G %s %s %s %ld %ld :%s", send_token("TKL", "BD"),
             user, host, who, (long int) time(NULL) + 86400 * 2,
             (long int) when, reason);
}

/*
** svskill
**	parv[0] = servername
**	parv[1] = client
**	parv[2] = kill message
*/
void unreal_cmd_svskill(char *source, char *user, char *buf)
{
    if (!source || !user || !buf) {
        return;
    }
    send_cmd(source, "%s %s :%s", send_token("SVSKILL", "h"), user, buf);
}

/*
 * m_svsmode() added by taz
 * parv[0] - sender
 * parv[1] - username to change mode for
 * parv[2] - modes to change
 * parv[3] - Service Stamp (if mode == d)
 */
void unreal_cmd_svsmode(User * u, int ac, char **av)
{
    if (ac >= 1) {
        if (!u || !av[0]) {
            return;
        }
        if (UseSVS2MODE) {
            send_cmd(ServerName, "%s %s %s%s%s",
                     send_token("SVS2MODE", "v"), u->nick, av[0],
                     (ac == 2 ? " " : ""), (ac == 2 ? av[1] : ""));
        } else {
            send_cmd(ServerName, "%s %s %s%s%s",
                     send_token("SVSMODE", "n"), u->nick, av[0],
                     (ac == 2 ? " " : ""), (ac == 2 ? av[1] : ""));
        }
    }
}

/* 372 */
void unreal_cmd_372(char *source, char *msg)
{
    send_cmd(ServerName, "372 %s :- %s", source, msg);
}

void unreal_cmd_372_error(char *source)
{
    send_cmd(ServerName, "422 %s :- MOTD file not found!  Please "
             "contact your IRC administrator.", source);
}

void unreal_cmd_375(char *source)
{
    send_cmd(ServerName, "375 %s :- %s Message of the Day",
             source, ServerName);
}

void unreal_cmd_376(char *source)
{
    send_cmd(ServerName, "376 %s :End of /MOTD command.", source);
}

void unreal_cmd_nick(char *nick, char *name, char *modes)
{
    EnforceQlinedNick(nick, NULL);
    send_cmd(NULL, "%s %s 1 %ld %s %s %s 0 %s %s%s :%s",
             send_token("NICK", "&"), nick, (long int) time(NULL),
             ServiceUser, ServiceHost, ServerName, modes, ServiceHost,
             (myIrcd->nickip ? " *" : " "), name);
    unreal_cmd_sqline(nick, "Reserved for services");
}

void unreal_cmd_guest_nick(char *nick, char *user, char *host, char *real,
                           char *modes)
{
    send_cmd(NULL, "%s %s 1 %ld %s %s %s 0 %s %s%s :%s",
             send_token("NICK", "&"), nick, (long int) time(NULL), user,
             host, ServerName, modes, host, (myIrcd->nickip ? " *" : " "),
             real);
}

void unreal_cmd_mode(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "%s %s %s", send_token("MODE", "G"), dest, buf);
}

void unreal_cmd_bot_nick(char *nick, char *user, char *host, char *real,
                         char *modes)
{
    EnforceQlinedNick(nick, s_BotServ);
    send_cmd(NULL, "%s %s 1 %ld %s %s %s 0 %s %s%s :%s",
             send_token("NICK", "&"), nick, (long int) time(NULL), user,
             host, ServerName, modes, host, (myIrcd->nickip ? " *" : " "),
             real);
    unreal_cmd_sqline(nick, "Reserved for services");
}

void unreal_cmd_kick(char *source, char *chan, char *user, char *buf)
{
    if (buf) {
        send_cmd(source, "%s %s %s :%s", send_token("KICK", "H"), chan,
                 user, buf);
    } else {
        send_cmd(source, "%s %s %s", send_token("KICK", "H"), chan, user);
    }
}

void unreal_cmd_notice_ops(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "%s @%s :%s", send_token("NOTICE", "B"), dest, buf);
}


void unreal_cmd_notice(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    if (NSDefFlags & NI_MSG) {
        unreal_cmd_privmsg2(source, dest, buf);
    } else {
        send_cmd(source, "%s %s :%s", send_token("NOTICE", "B"), dest,
                 buf);
    }
}

void unreal_cmd_notice2(char *source, char *dest, char *msg)
{
    send_cmd(source, "%s %s :%s", send_token("NOTICE", "B"), dest, msg);
}

void unreal_cmd_privmsg(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "%s %s :%s", send_token("PRIVMSG", "!"), dest, buf);
}

void unreal_cmd_privmsg2(char *source, char *dest, char *msg)
{
    send_cmd(source, "%s %s :%s", send_token("PRIVMSG", "!"), dest, msg);
}

void unreal_cmd_serv_notice(char *source, char *dest, char *msg)
{
    send_cmd(source, "%s $%s :%s", send_token("NOTICE", "B"), dest, msg);
}

void unreal_cmd_serv_privmsg(char *source, char *dest, char *msg)
{
    send_cmd(source, "%s $%s :%s", send_token("PRIVMSG", "!"), dest, msg);
}

void unreal_cmd_bot_chan_mode(char *nick, char *chan)
{
    anope_cmd_mode(nick, chan, "%s %s %s", myIrcd->botchanumode, nick,
                   nick);
}

void unreal_cmd_351(char *source)
{
    send_cmd(ServerName, "351 %s Anope-%s %s :%s - %s (%s) -- %s",
             source, version_number, ServerName, myIrcd->name,
             version_flags, EncModule, version_build);
}

/* QUIT */
void unreal_cmd_quit(char *source, char *buf)
{
    if (buf) {
        send_cmd(source, "%s :%s", send_token("QUIT", ","), buf);
    } else {
        send_cmd(source, "%s", send_token("QUIT", ","));
    }
}

/* PROTOCTL */
/*
   NICKv2 = Nick Version 2
   VHP    = Sends hidden host 
   UMODE2 = sends UMODE2 on user modes
   NICKIP = Sends IP on NICK
   TOKEN  = Use tokens to talk
   SJ3    = Supports SJOIN
   NOQUIT = No Quit
   TKLEXT = Extended TKL we don't use it but best to have it
   SJB64  = Base64 encoded time stamps
   VL     = Version Info
   NS     = Numeric Server

*/
void unreal_cmd_capab()
{
    if (UseTokens) {
        if (Numeric) {
            send_cmd(NULL,
                     "PROTOCTL NICKv2 VHP UMODE2 NICKIP TOKEN SJOIN SJOIN2 SJ3 NOQUIT TKLEXT SJB64 VL");
        } else {
            send_cmd(NULL,
                     "PROTOCTL NICKv2 VHP UMODE2 NICKIP TOKEN SJOIN SJOIN2 SJ3 NOQUIT TKLEXT SJB64");
        }
    } else {
        if (Numeric) {
            send_cmd(NULL,
                     "PROTOCTL NICKv2 VHP UMODE2 NICKIP SJOIN SJOIN2 SJ3 NOQUIT TKLEXT SJB64 VL");
        } else {
            send_cmd(NULL,
                     "PROTOCTL NICKv2 VHP UMODE2 NICKIP SJOIN SJOIN2 SJ3 NOQUIT TKLEXT SJB64");
        }
    }
}

/* PASS */
void unreal_cmd_pass(char *pass)
{
    send_cmd(NULL, "PASS :%s", pass);
}

/* SERVER name hop descript */
/* Unreal 3.2 actually sends some info about itself in the descript area */
void unreal_cmd_server(char *servname, int hop, char *descript)
{
    if (Numeric) {
        send_cmd(NULL, "SERVER %s %d :U0-*-%s %s", servname, hop, Numeric,
                 descript);
    } else {
        send_cmd(NULL, "SERVER %s %d :%s", servname, hop, descript);
    }
}

/* PONG */
void unreal_cmd_pong(char *servname, char *who)
{
    send_cmd(servname, "%s %s", send_token("PONG", "9"), who);
}

/* JOIN */
void unreal_cmd_join(char *user, char *channel, time_t chantime)
{
    send_cmd(ServerName, "%s !%s %s :%s",
             send_token("SJOIN", "~"), base64enc((long int) chantime),
             channel, user);
    /* send_cmd(user, "%s %s", send_token("JOIN", "C"), channel); */
}

/* unsqline
**	parv[0] = sender
**	parv[1] = nickmask
*/
void unreal_cmd_unsqline(char *user)
{
    if (!user) {
        return;
    }
    send_cmd(NULL, "%s %s", send_token("UNSQLINE", "d"), user);
}

/* CHGHOST */
void unreal_cmd_chghost(char *nick, char *vhost)
{
    if (!nick || !vhost) {
        return;
    }
    send_cmd(ServerName, "%s %s %s", send_token("CHGHOST", "AL"), nick,
             vhost);
}

/* CHGIDENT */
void unreal_cmd_chgident(char *nick, char *vIdent)
{
    if (!nick || !vIdent) {
        return;
    }
    send_cmd(ServerName, "%s %s %s", send_token("CHGIDENT", "AZ"), nick,
             vIdent);
}

/* INVITE */
void unreal_cmd_invite(char *source, char *chan, char *nick)
{
    if (!source || !chan || !nick) {
        return;
    }

    send_cmd(source, "%s %s %s", send_token("INVITE", "*"), nick, chan);
}

/* PART */
void unreal_cmd_part(char *nick, char *chan, char *buf)
{
    if (!nick || !chan) {
        return;
    }

    if (buf) {
        send_cmd(nick, "%s %s :%s", send_token("PART", "D"), chan, buf);
    } else {
        send_cmd(nick, "%s %s", send_token("PART", "D"), chan);
    }
}

/* 391    RPL_TIME ":%s 391 %s %s :%s" */
void unreal_cmd_391(char *source, char *timestr)
{
    if (!timestr) {
        return;
    }
    send_cmd(ServerName, "391 %s %s :%s", source, ServerName, timestr);
}

/* 250 */
void unreal_cmd_250(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "250 %s", buf);
}

/* 307 */
void unreal_cmd_307(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "307 %s", buf);
}

/* 311 */
void unreal_cmd_311(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "311 %s", buf);
}

/* 312 */
void unreal_cmd_312(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "312 %s", buf);
}

/* 317 */
void unreal_cmd_317(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "317 %s", buf);
}

/* 219 */
void unreal_cmd_219(char *source, char *letter)
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
void unreal_cmd_401(char *source, char *who)
{
    if (!source || !who) {
        return;
    }
    send_cmd(ServerName, "401 %s %s :No such service.", source, who);
}

/* 318 */
void unreal_cmd_318(char *source, char *who)
{
    if (!source || !who) {
        return;
    }

    send_cmd(ServerName, "318 %s %s :End of /WHOIS list.", source, who);
}

/* 242 */
void unreal_cmd_242(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "242 %s", buf);
}

/* 243 */
void unreal_cmd_243(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "243 %s", buf);
}

/* 211 */
void unreal_cmd_211(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "211 %s", buf);
}

/* GLOBOPS */
void unreal_cmd_global(char *source, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source ? source : ServerName, "%s :%s",
             send_token("GLOBOPS", "]"), buf);
}

/* GLOBOPS - to handle old WALLOPS */
void unreal_cmd_global_legacy(char *source, char *fmt)
{
    send_cmd(source ? source : ServerName, "%s :%s",
             send_token("GLOBOPS", "]"), fmt);
}

/* SQLINE */
/*
**	parv[0] = sender
**	parv[1] = nickmask
**	parv[2] = reason
** 
** - Unreal will translate this to TKL for us
**
*/
void unreal_cmd_sqline(char *mask, char *reason)
{
    if (!mask || !reason) {
        return;
    }

    send_cmd(NULL, "%s %s :%s", send_token("SQLINE", "c"), mask, reason);
}

/* SQUIT */
void unreal_cmd_squit(char *servname, char *message)
{
    if (!servname || !message) {
        return;
    }

    send_cmd(NULL, "%s %s :%s", send_token("SQUIT", "-"), servname,
             message);
}

/*
** svso
**      parv[0] = sender prefix
**      parv[1] = nick
**      parv[2] = options
*/
void unreal_cmd_svso(char *source, char *nick, char *flag)
{
    if (!source || !nick || !flag) {
        return;
    }

    send_cmd(source, "%s %s %s", send_token("SVSO", "BB"), nick, flag);
}

/* NICK <newnick>  */
void unreal_cmd_chg_nick(char *oldnick, char *newnick)
{
    if (!oldnick || !newnick) {
        return;
    }

    send_cmd(oldnick, "%s %s %ld", send_token("NICK", "&"), newnick,
             (long int) time(NULL));
}

/* SVSNICK */
/*
**      parv[0] = sender
**      parv[1] = old nickname
**      parv[2] = new nickname
**      parv[3] = timestamp
*/
void unreal_cmd_svsnick(char *source, char *guest, time_t when)
{
    if (!source || !guest) {
        return;
    }
    send_cmd(NULL, "%s %s %s :%ld", send_token("SVSNICK", "e"), source,
             guest, (long int) when);
}

/* Functions that use serval cmd functions */

void unreal_cmd_vhost_on(char *nick, char *vIdent, char *vhost)
{
    if (!nick) {
        return;
    }
    if (vIdent) {
        unreal_cmd_chgident(nick, vIdent);
    }
    unreal_cmd_chghost(nick, vhost);
}

void unreal_cmd_connect(int servernum)
{
    if (Numeric) {
        me_server =
            new_server(NULL, ServerName, ServerDesc, SERVER_ISME, Numeric);
    } else {
        me_server =
            new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);
    }

    unreal_cmd_capab();
    if (servernum == 1) {
        unreal_cmd_pass(RemotePassword);
    }
    if (servernum == 2) {
        unreal_cmd_pass(RemotePassword2);
    }
    if (servernum == 3) {
        unreal_cmd_pass(RemotePassword3);
    }
    unreal_cmd_server(ServerName, 1, ServerDesc);
}

void unreal_cmd_bob()
{
    /* Not used */
}

/* Events */

int anope_event_ping(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    unreal_cmd_pong(ac > 1 ? av[1] : ServerName, av[0]);
    return MOD_CONT;
}

void unreal_cmd_netinfo(int ac, char **av)
{
    send_cmd(NULL, "%s %ld %ld %d %s 0 0 0 :%s",
             send_token("NETINFO", "AO"), (long int) maxusercnt,
             (long int) time(NULL), atoi(av[2]), av[3], av[7]);
}

/* netinfo
 *  argv[0] = max global count
 *  argv[1] = time of end sync
 *  argv[2] = unreal protocol using (numeric)
 *  argv[3] = cloak-crc (> u2302)
 *  argv[4] = free(**)
 *  argv[5] = free(**)
 *  argv[6] = free(**)
 *  argv[7] = ircnet
 */
int anope_event_netinfo(char *source, int ac, char **av)
{
    unreal_cmd_netinfo(ac, av);
    return MOD_CONT;
}


/* TKL
 *           add:      remove:    spamfilter:    spamfilter+TKLEXT  sqline:
 * parv[ 1]: +         -          +/-            +                  +/-
 * parv[ 2]: type      type       type           type               type
 * parv[ 3]: user      user       target         target             hold
 * parv[ 4]: host      host       action         action             host
 * parv[ 5]: setby     removedby  (un)setby      setby              setby
 * parv[ 6]: expire_at            expire_at (0)  expire_at (0)      expire_at
 * parv[ 7]: set_at               set_at         set_at             set_at
 * parv[ 8]: reason               regex          tkl duration       reason
 * parv[ 9]:                                     tkl reason [A]        
 * parv[10]:                                     regex              
 *
*/
int anope_event_tkl(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_eos(char *source, int ac, char **av)
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

int anope_event_436(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;

    m_nickcoll(av[0]);
    return MOD_CONT;
}

/*
** away
**      parv[0] = sender prefix
**      parv[1] = away message
*/
int anope_event_away(char *source, int ac, char **av)
{
    if (!source) {
        return MOD_CONT;
    }
    m_away(source, (ac ? av[0] : NULL));
    return MOD_CONT;
}

/*
** m_topic
**	parv[0] = sender prefix
**	parv[1] = topic text
**
**	For servers using TS: 
**	parv[0] = sender prefix
**	parv[1] = channel name
**	parv[2] = topic nickname
**	parv[3] = topic time
**	parv[4] = topic text
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

int anope_event_pass(char *source, int ac, char **av)
{
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

/* Unreal sends USER modes with this */
/*
    umode2
    parv[0] - sender
    parv[1] - modes to change
*/
int anope_event_umode2(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;

    do_umode2(source, ac, av);
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

int anope_event_sethost(char *source, int ac, char **av)
{
    User *u;

    if (ac != 1)
        return MOD_CONT;

    u = finduser(source);
    if (!u) {
        if (debug) {
            alog("debug: SETHOST for nonexistent user %s", source);
        }
        return MOD_CONT;
    }

    if (u->mode & UMODE_x)
        change_user_host(u, av[0]);
    else {
        if (u->chost)
            free(u->chost);
        u->chost = sstrdup(av[0]);
    }
    return MOD_CONT;
}

/*
** NICK - new
**      source  = NULL
**	parv[0] = nickname
**      parv[1] = hopcount
**      parv[2] = timestamp
**      parv[3] = username
**      parv[4] = hostname
**      parv[5] = servername
**  if NICK version 1:
**      parv[6] = servicestamp
**	parv[7] = info
**  if NICK version 2:
**	parv[6] = servicestamp
**      parv[7] = umodes
**	parv[8] = virthost, * if none
**	parv[9] = info
**  if NICKIP:
**      parv[9] = ip
**      parv[10] = info
** 
** NICK - change 
**      source  = oldnick
**	parv[0] = new nickname
**      parv[1] = hopcount
*/
/*
 do_nick(const char *source, char *nick, char *username, char *host,
              char *server, char *realname, time_t ts, uint32 svid,
              uint32 ip, char *vhost, char *uid)
*/
int anope_event_nick(char *source, int ac, char **av)
{
    User *user;

    if (ac != 2) {
        if (ac == 7) {
            /* 
               <codemastr> that was a bug that is now fixed in 3.2.1
               <codemastr> in  some instances it would use the non-nickv2 format
               <codemastr> it's sent when a nick collision occurs 
               - so we have to leave it around for now -TSL
             */
            do_nick(source, av[0], av[3], av[4], av[5], av[6],
                    strtoul(av[2], NULL, 10), 0, 0, "*", NULL);

        } else if (ac == 11) {
            user = do_nick(source, av[0], av[3], av[4], av[5], av[10],
                           strtoul(av[2], NULL, 10), strtoul(av[6], NULL,
                                                             0),
                           ntohl(decode_ip(av[9])), av[8], NULL);
            if (user)
                anope_set_umode(user, 1, &av[7]);

        } else {
            /* NON NICKIP */
            user = do_nick(source, av[0], av[3], av[4], av[5], av[9],
                           strtoul(av[2], NULL, 10), strtoul(av[6], NULL,
                                                             0), 0, av[8],
                           NULL);
            if (user)
                anope_set_umode(user, 1, &av[7]);
        }
    } else {
        do_nick(source, av[0], NULL, NULL, NULL, NULL,
                strtoul(av[1], NULL, 10), 0, 0, NULL, NULL);
    }
    return MOD_CONT;
}


int anope_event_chghost(char *source, int ac, char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(av[0]);
    if (!u) {
        if (debug) {
            alog("debug: CHGHOST for nonexistent user %s", av[0]);
        }
        return MOD_CONT;
    }

    change_user_host(u, av[1]);
    return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(char *source, int ac, char **av)
{
    char *desc;
    char *vl;
    char *upnumeric;

    if (!stricmp(av[1], "1")) {
        uplink = sstrdup(av[0]);
        vl = myStrGetToken(av[2], ' ', 0);
        upnumeric = myStrGetToken(vl, '-', 2);
        desc = myStrGetTokenRemainder(av[2], ' ', 1);
        do_server(source, av[0], av[1], desc, upnumeric);
        Anope_Free(vl);
        Anope_Free(desc);
        Anope_Free(upnumeric);
    } else {
        do_server(source, av[0], av[1], av[2], NULL);
    }

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

/* SVSHOLD - set */
void unreal_cmd_svshold(char *nick)
{
    send_cmd(NULL, "%s + Q H %s %s %ld %ld :%s", send_token("TKL", "BD"),
             nick, ServerName, (long int) time(NULL) + NSReleaseTimeout,
             (long int) time(NULL), "Being held for registered user");
}

/* SVSHOLD - release */
void unreal_cmd_release_svshold(char *nick)
{
    send_cmd(NULL, "%s - Q * %s %s", send_token("TKL", "BD"), nick,
             ServerName);
}

/* UNSGLINE */
/*
 * SVSNLINE - :realname mask
*/
void unreal_cmd_unsgline(char *mask)
{
    send_cmd(NULL, "%s - :%s", send_token("SVSNLINE", "BR"), mask);
}

/* UNSZLINE */
void unreal_cmd_unszline(char *mask)
{
    send_cmd(NULL, "%s - Z * %s %s", send_token("TKL", "BD"), mask,
             s_OperServ);
}

/* SZLINE */
void unreal_cmd_szline(char *mask, char *reason, char *whom)
{
    send_cmd(NULL, "%s + Z * %s %s %ld %ld :%s", send_token("TKL", "BD"),
             mask, whom, (long int) time(NULL) + 86400 * 2,
             (long int) time(NULL), reason);
}

/* SGLINE */
/*
 * SVSNLINE + reason_where_is_space :realname mask with spaces
*/
void unreal_cmd_sgline(char *mask, char *reason)
{
    strnrepl(reason, BUFSIZE, " ", "_");
    send_cmd(NULL, "%s + %s :%s", send_token("SVSNLINE", "BR"), reason,
             mask);
}

/* SVSMODE -b */
void unreal_cmd_unban(char *name, char *nick)
{
    unreal_cmd_svsmode_chan(name, "-b", nick);
}


/* SVSMODE channel modes */

void unreal_cmd_svsmode_chan(char *name, char *mode, char *nick)
{
    if (nick) {
        send_cmd(ServerName, "%s %s %s %s", send_token("SVSMODE", "n"),
                 name, mode, nick);
    } else {
        send_cmd(ServerName, "%s %s %s", send_token("SVSMODE", "n"), name,
                 mode);
    }
}


/* SVSMODE +d */
/* sent if svid is something weird */
void unreal_cmd_svid_umode(char *nick, time_t ts)
{
    if (UseSVS2MODE) {
        send_cmd(ServerName, "%s %s +d 1", send_token("SVS2MODE", "v"),
                 nick);
    } else {
        send_cmd(ServerName, "%s %s +d 1", send_token("SVSMODE", "n"),
                 nick);
    }
}

/* SVSMODE +d */
/* nc_change was = 1, and there is no na->status */
void unreal_cmd_nc_change(User * u)
{
    common_svsmode(u, "-r+d", "1");
}

/* SVSMODE +r */
void unreal_cmd_svid_umode2(User * u, char *ts)
{
    if (u->svid != u->timestamp) {
        common_svsmode(u, "+rd", ts);
    } else {
        common_svsmode(u, "+r", NULL);
    }
}

void unreal_cmd_svid_umode3(User * u, char *ts)
{
    /* not used */
}

int anope_event_error(char *source, int ac, char **av)
{
    if (av[0]) {
        if (debug) {
            alog("debug: %s", av[0]);
        }
	if(strstr(av[0],"No matching link configuration")!=0) {
	    alog("Error: Your IRCD's link block may not be setup correctly, please check unrealircd.conf");
	}
    }
    return MOD_CONT;
}

int anope_event_notice(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_smo(char *source, int ac, char **av)
{
    return MOD_CONT;
}

/* svsjoin
	parv[0] - sender
	parv[1] - nick to make join
	parv[2] - channel to join
	parv[3] - (optional) channel key(s)
*/
/* In older Unreal SVSJOIN and SVSNLINE tokens were mixed so SVSJOIN and SVSNLINE are broken
   when coming from a none TOKEN'd server
*/
void unreal_cmd_svsjoin(char *source, char *nick, char *chan, char *param)
{
    if (param) {
        send_cmd(source, "%s %s %s :%s", send_token("SVSJOIN", "BX"), nick, chan, param);
    } else {
        send_cmd(source, "%s %s :%s", send_token("SVSJOIN", "BX"), nick, chan);
    }
}

/* svspart
	parv[0] - sender
	parv[1] - nick to make part
	parv[2] - channel(s) to part
*/
void unreal_cmd_svspart(char *source, char *nick, char *chan)
{
    send_cmd(source, "%s %s :%s", send_token("SVSPART", "BT"), nick, chan);
}

int anope_event_globops(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_swhois(char *source, int ac, char **av)
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

int anope_event_sdesc(char *source, int ac, char **av)
{
    Server *s;
    s = findserver(servlist, source);

    if (s) {
        s->desc = av[0];
    }

    return MOD_CONT;
}

int anope_event_sjoin(char *source, int ac, char **av)
{
    do_sjoin(source, ac, av);
    return MOD_CONT;
}

void unreal_cmd_swhois(char *source, char *who, char *mask)
{
    send_cmd(source, "%s %s :%s", send_token("SWHOIS", "BA"), who, mask);
}

void unreal_cmd_eob()
{
    send_cmd(ServerName, "%s", send_token("EOS", "ES"));
}

/* svswatch
 * parv[0] - sender
 * parv[1] - target nick
 * parv[2] - parameters
 */
void unreal_cmd_svswatch(char *sender, char *nick, char *parm)
{
    send_cmd(sender, "%s %s :%s", send_token("SVSWATCH", "Bw"), nick,
             parm);
}

/* check if +f mode is valid for the ircd */
/* borrowed part of the new check from channels.c in Unreal */
int unreal_flood_mode_check(char *value)
{
    char *dp, *end;
    /* NEW +F */
    char xbuf[256], *p, *p2, *x = xbuf + 1;
    int v;

    if (!value) {
        return 0;
    }

    if (*value != ':'
        && (strtoul((*value == '*' ? value + 1 : value), &dp, 10) > 0)
        && (*dp == ':') && (*(++dp) != 0) && (strtoul(dp, &end, 10) > 0)
        && (*end == 0)) {
        return 1;
    } else {
        /* '['<number><1 letter>[optional: '#'+1 letter],[next..]']'':'<number> */
        strncpy(xbuf, value, sizeof(xbuf));
        p2 = strchr(xbuf + 1, ']');
        if (!p2) {
            return 0;
        }
        *p2 = '\0';
        if (*(p2 + 1) != ':') {
            return 0;
        }
        for (x = strtok(xbuf + 1, ","); x; x = strtok(NULL, ",")) {
            /* <number><1 letter>[optional: '#'+1 letter] */
            p = x;
            while (isdigit(*p)) {
                p++;
            }
            if ((*p == '\0')
                || !((*p == 'c') || (*p == 'j') || (*p == 'k')
                     || (*p == 'm') || (*p == 'n') || (*p == 't'))) {
                continue;       /* continue instead of break for forward compatability. */
            }
            *p = '\0';
            v = atoi(x);
            if ((v < 1) || (v > 999)) {
                return 0;
            }
            p++;
        }
        return 1;
    }
}

void unreal_cmd_jupe(char *jserver, char *who, char *reason)
{
    char rbuf[256];

    snprintf(rbuf, sizeof(rbuf), "Juped by %s%s%s", who,
             reason ? ": " : "", reason ? reason : "");

    if (findserver(servlist, jserver))
        unreal_cmd_squit(jserver, rbuf);
    unreal_cmd_server(jserver, 2, rbuf);
    new_server(me_server, jserver, rbuf, SERVER_JUPED, NULL);
}

/* 
  1 = valid nick
  0 = nick is in valid
*/
int unreal_valid_nick(char *nick)
{
    if (!stricmp("ircd", nick)) {
        return 0;
    }
    if (!stricmp("irc", nick)) {
        return 0;
    }
    return 1;
}

int unreal_valid_chan(char *chan) {
    if (strchr(chan, ':')) {
      return 0;
    }
    return 1;
}

void unreal_cmd_ctcp(char *source, char *dest, char *buf)
{
    char *s;
    if (!buf) {
        return;
    } else {
        s = normalizeBuffer(buf);
    }

    send_cmd(source, "%s %s :\1%s \1", send_token("NOTICE", "B"), dest, s);
    free(s);
}

int unreal_jointhrottle_mode_check(char *value)
{
    char *tempValue, *one, *two;
    int param1, param2;

    if (!value)
        return 0;

    tempValue = sstrdup(value);
    one = strtok(tempValue, ":");
    two = strtok(NULL, "");
    if (one && two) {
        param1 = atoi(one);
        param2 = atoi(two);
        if ((param1 >= 1) && (param1 <= 255) && (param2 >= 1) && (param2 <= 999)) {
            free(tempValue);
            return 1;
        }
    }
    free(tempValue);
    return 0;
}

/* *INDENT-OFF* */
void moduleAddIRCDMsgs(void) {
    Message *m;

    updateProtectDetails("PROTECT","PROTECTME","protect","deprotect","AUTOPROTECT","+a","-a");

    m = createMessage("401",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("402",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("436",       anope_event_436); addCoreMessage(IRCD,m);
    m = createMessage("451",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("461",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("AWAY",      anope_event_away); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("6",        anope_event_away); addCoreMessage(IRCD,m);
    }
    m = createMessage("INVITE",    anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("*",        anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("JOIN",      anope_event_join); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("C",        anope_event_join); addCoreMessage(IRCD,m);
    }
    m = createMessage("KICK",      anope_event_kick); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("H",        anope_event_kick); addCoreMessage(IRCD,m);
    }
    m = createMessage("KILL",      anope_event_kill); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage(".",        anope_event_kill); addCoreMessage(IRCD,m);
    }
    m = createMessage("MODE",      anope_event_mode); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("G",        anope_event_mode); addCoreMessage(IRCD,m);
    }
    m = createMessage("MOTD",      anope_event_motd); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("F",        anope_event_motd); addCoreMessage(IRCD,m);
    }
    m = createMessage("NICK",      anope_event_nick); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("&",        anope_event_nick); addCoreMessage(IRCD,m);
    }
    m = createMessage("NOTICE",    anope_event_notice); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("B",        anope_event_notice); addCoreMessage(IRCD,m);
    }
    m = createMessage("PART",      anope_event_part); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("D",        anope_event_part); addCoreMessage(IRCD,m);
    }
    m = createMessage("PING",      anope_event_ping); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("8",        anope_event_ping); addCoreMessage(IRCD,m);
    }
    m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("!",        anope_event_privmsg); addCoreMessage(IRCD,m);
    }
    m = createMessage("QUIT",      anope_event_quit); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage(",",        anope_event_quit); addCoreMessage(IRCD,m);
    }
    m = createMessage("SERVER",    anope_event_server); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("'",        anope_event_server); addCoreMessage(IRCD,m);
    }
    m = createMessage("SQUIT",     anope_event_squit); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("-",        anope_event_squit); addCoreMessage(IRCD,m);
    }
    m = createMessage("TOPIC",     anope_event_topic); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage(")",        anope_event_topic); addCoreMessage(IRCD,m);
    }
    m = createMessage("SVSMODE",   anope_event_mode); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("n",        anope_event_mode); addCoreMessage(IRCD,m);
    }
    m = createMessage("SVS2MODE",   anope_event_mode); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("v",       anope_event_mode); addCoreMessage(IRCD,m);
    }
    m = createMessage("USER",      anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("%",        anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("WALLOPS",   anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("=",        anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("WHOIS",     anope_event_whois); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("#",        anope_event_whois); addCoreMessage(IRCD,m);
    }
    m = createMessage("AKILL",     anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("V",       anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("GLOBOPS",   anope_event_globops); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("]",        anope_event_globops); addCoreMessage(IRCD,m);
    }
    m = createMessage("GNOTICE",   anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("Z",        anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("GOPER",     anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("[",        anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("RAKILL",    anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("Y",        anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("SILENCE",   anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("U",        anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("SVSKILL",   anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("h",        anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("SVSNICK",   anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("e",        anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("SVSNOOP",   anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("f",        anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("SQLINE",    anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("c",        anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("UNSQLINE",  anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("d",        anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("PROTOCTL",  anope_event_capab); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("_",        anope_event_capab); addCoreMessage(IRCD,m);
    }
    m = createMessage("CHGHOST",   anope_event_chghost); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("AL",        anope_event_chghost); addCoreMessage(IRCD,m);
    }
    m = createMessage("CHGIDENT",  anope_event_chgident); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("AZ",        anope_event_chgident); addCoreMessage(IRCD,m);
    }
    m = createMessage("CHGNAME",   anope_event_chgname); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("BK",        anope_event_chgname); addCoreMessage(IRCD,m);
    }
    m = createMessage("NETINFO",   anope_event_netinfo); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("AO",       anope_event_netinfo); addCoreMessage(IRCD,m);
    }
    m = createMessage("SETHOST",   anope_event_sethost); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("AA",        anope_event_sethost); addCoreMessage(IRCD,m);
    }
    m = createMessage("SETIDENT",  anope_event_setident); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("AD",        anope_event_setident); addCoreMessage(IRCD,m);
    }
    m = createMessage("SETNAME",   anope_event_setname); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("AE",        anope_event_setname); addCoreMessage(IRCD,m);
    }
    m = createMessage("TKL", 	   anope_event_tkl); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("BD",       anope_event_tkl); addCoreMessage(IRCD,m);
    }
    m = createMessage("EOS", 	   anope_event_eos); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("ES",       anope_event_eos); addCoreMessage(IRCD,m);
    }
    m = createMessage("PASS", 	   anope_event_pass); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("<",        anope_event_pass); addCoreMessage(IRCD,m);
    }
    m = createMessage("ERROR", 	   anope_event_error); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("5",        anope_event_error); addCoreMessage(IRCD,m);
    }
    m = createMessage("SMO", 	   anope_event_smo); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("AU",       anope_event_smo); addCoreMessage(IRCD,m);
    }
    m = createMessage("UMODE2",    anope_event_umode2); addCoreMessage(IRCD,m);
    if (UseTokens) {
     m = createMessage("|",        anope_event_umode2); addCoreMessage(IRCD,m);
    }
    m = createMessage("SWHOIS",    anope_event_swhois); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("BA",        anope_event_swhois); addCoreMessage(IRCD,m);
    }
    m = createMessage("SJOIN",      anope_event_sjoin); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("~",        anope_event_sjoin); addCoreMessage(IRCD,m);
    }
    m = createMessage("REHASH",     anope_event_rehash); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("O",        anope_event_rehash); addCoreMessage(IRCD,m);
    }
    m = createMessage("ADMIN",      anope_event_admin); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("@",        anope_event_admin); addCoreMessage(IRCD,m);
    }
    m = createMessage("CREDITS",    anope_event_credits); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("AJ",       anope_event_credits); addCoreMessage(IRCD,m);
    }
    m = createMessage("SDESC",      anope_event_sdesc); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("AG",       anope_event_sdesc); addCoreMessage(IRCD,m);
    }
    m = createMessage("HTM",        anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("BH",       anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("HELP",        anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("4",       anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("TRACE",        anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("b",       anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("LAG",        anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("AF",       anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("RPING",        anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("AM",       anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("SENDSNO",    anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("Ss",       anope_event_null); addCoreMessage(IRCD,m);
    }
    m = createMessage("SENDUMODE",  anope_event_null); addCoreMessage(IRCD,m);
    if (UseTokens) {
      m = createMessage("AP",       anope_event_null); addCoreMessage(IRCD,m);
    }

    /* The none token version of these is in messages.c */
    if (UseTokens) {
     m = createMessage("2",         m_stats); addCoreMessage(IRCD,m);
     m = createMessage(">",         m_time); addCoreMessage(IRCD,m);
     m = createMessage("+",         m_version); addCoreMessage(IRCD,m);
    }
}

/* *INDENT-ON* */

/**
 * Tell anope which function we want to perform each task inside of anope.
 * These prototypes must match what anope expects.
 **/
void moduleAddAnopeCmds()
{
    pmodule_cmd_svsnoop(unreal_cmd_svsnoop);
    pmodule_cmd_remove_akill(unreal_cmd_remove_akill);
    pmodule_cmd_topic(unreal_cmd_topic);
    pmodule_cmd_vhost_off(unreal_cmd_vhost_off);
    pmodule_cmd_akill(unreal_cmd_akill);
    pmodule_cmd_svskill(unreal_cmd_svskill);
    pmodule_cmd_svsmode(unreal_cmd_svsmode);
    pmodule_cmd_372(unreal_cmd_372);
    pmodule_cmd_372_error(unreal_cmd_372_error);
    pmodule_cmd_375(unreal_cmd_375);
    pmodule_cmd_376(unreal_cmd_376);
    pmodule_cmd_nick(unreal_cmd_nick);
    pmodule_cmd_guest_nick(unreal_cmd_guest_nick);
    pmodule_cmd_mode(unreal_cmd_mode);
    pmodule_cmd_bot_nick(unreal_cmd_bot_nick);
    pmodule_cmd_kick(unreal_cmd_kick);
    pmodule_cmd_notice_ops(unreal_cmd_notice_ops);
    pmodule_cmd_notice(unreal_cmd_notice);
    pmodule_cmd_notice2(unreal_cmd_notice2);
    pmodule_cmd_privmsg(unreal_cmd_privmsg);
    pmodule_cmd_privmsg2(unreal_cmd_privmsg2);
    pmodule_cmd_serv_notice(unreal_cmd_serv_notice);
    pmodule_cmd_serv_privmsg(unreal_cmd_serv_privmsg);
    pmodule_cmd_bot_chan_mode(unreal_cmd_bot_chan_mode);
    pmodule_cmd_351(unreal_cmd_351);
    pmodule_cmd_quit(unreal_cmd_quit);
    pmodule_cmd_pong(unreal_cmd_pong);
    pmodule_cmd_join(unreal_cmd_join);
    pmodule_cmd_unsqline(unreal_cmd_unsqline);
    pmodule_cmd_invite(unreal_cmd_invite);
    pmodule_cmd_part(unreal_cmd_part);
    pmodule_cmd_391(unreal_cmd_391);
    pmodule_cmd_250(unreal_cmd_250);
    pmodule_cmd_307(unreal_cmd_307);
    pmodule_cmd_311(unreal_cmd_311);
    pmodule_cmd_312(unreal_cmd_312);
    pmodule_cmd_317(unreal_cmd_317);
    pmodule_cmd_219(unreal_cmd_219);
    pmodule_cmd_401(unreal_cmd_401);
    pmodule_cmd_318(unreal_cmd_318);
    pmodule_cmd_242(unreal_cmd_242);
    pmodule_cmd_243(unreal_cmd_243);
    pmodule_cmd_211(unreal_cmd_211);
    pmodule_cmd_global(unreal_cmd_global);
    pmodule_cmd_global_legacy(unreal_cmd_global_legacy);
    pmodule_cmd_sqline(unreal_cmd_sqline);
    pmodule_cmd_squit(unreal_cmd_squit);
    pmodule_cmd_svso(unreal_cmd_svso);
    pmodule_cmd_chg_nick(unreal_cmd_chg_nick);
    pmodule_cmd_svsnick(unreal_cmd_svsnick);
    pmodule_cmd_vhost_on(unreal_cmd_vhost_on);
    pmodule_cmd_connect(unreal_cmd_connect);
    pmodule_cmd_bob(unreal_cmd_bob);
    pmodule_cmd_svshold(unreal_cmd_svshold);
    pmodule_cmd_release_svshold(unreal_cmd_release_svshold);
    pmodule_cmd_unsgline(unreal_cmd_unsgline);
    pmodule_cmd_unszline(unreal_cmd_unszline);
    pmodule_cmd_szline(unreal_cmd_szline);
    pmodule_cmd_sgline(unreal_cmd_sgline);
    pmodule_cmd_unban(unreal_cmd_unban);
    pmodule_cmd_svsmode_chan(unreal_cmd_svsmode_chan);
    pmodule_cmd_svid_umode(unreal_cmd_svid_umode);
    pmodule_cmd_nc_change(unreal_cmd_nc_change);
    pmodule_cmd_svid_umode2(unreal_cmd_svid_umode2);
    pmodule_cmd_svid_umode3(unreal_cmd_svid_umode3);
	pmodule_cmd_svsjoin(unreal_cmd_svsjoin);
	pmodule_cmd_svspart(unreal_cmd_svspart);
	pmodule_cmd_swhois(unreal_cmd_swhois);
    pmodule_cmd_eob(unreal_cmd_eob);
    pmodule_flood_mode_check(unreal_flood_mode_check);
    pmodule_cmd_jupe(unreal_cmd_jupe);
    pmodule_valid_nick(unreal_valid_nick);
    pmodule_valid_chan(unreal_valid_chan);
    pmodule_cmd_ctcp(unreal_cmd_ctcp);
    pmodule_set_umode(unreal_set_umode);
    pmodule_jointhrottle_mode_check(unreal_jointhrottle_mode_check);
}

/** 
 * Now tell anope how to use us.
 **/
int AnopeInit(int argc, char **argv)
{

    moduleAddAuthor("Anope");
    moduleAddVersion(VERSION_STRING);
    moduleSetType(PROTOCOL);

    pmodule_ircd_version("UnrealIRCd 3.2+");
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
