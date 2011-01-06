/* Unreal IRCD 3.1.x functions
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
#include "unreal31.h"
#include "version.h"

IRCDVar myIrcd[] = {
    {"UnrealIRCd 3.1.x",        /* ircd name */
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
     "-cilmnpstuzACGHKNOQRSV",  /* Modes to Remove */
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
     0,                         /* Supports SGlines     */
     1,                         /* Supports SQlines     */
     1,                         /* Supports SZlines     */
     1,                         /* Supports Halfop +h   */
     3,                         /* Number of server args */
     0,                         /* Join 2 Set           */
     0,                         /* Join 2 Message       */
     1,                         /* Has exceptions +e    */
     1,                         /* TS Topic Forward     */
     0,                         /* TS Topci Backward    */
     0,                         /* Protected Umode      */
     0,                         /* Has Admin            */
     0,                         /* Chan SQlines         */
     0,                         /* Quit on Kill         */
     0,                         /* SVSMODE unban        */
     1,                         /* Has Protect          */
     1,                         /* Reverse              */
     1,                         /* Chan Reg             */
     CMODE_r,                   /* Channel Mode         */
     1,                         /* vidents              */
     1,                         /* svshold              */
     1,                         /* time stamp on mode   */
     0,                         /* NICKIP               */
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
     CMODE_f,
     CMODE_L,
     0,
     1,                         /* No Knock requires +i */
     NULL,                      /* CAPAB Chan Modes             */
     0,                         /* We support Unreal TOKENS */
     1,                         /* TOKENS are CASE inSensitive */
     0,                         /* TIME STAMPS are BASE64 */
     0,                         /* +I support */
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
     0,
     0, 0}
};

unsigned long umodes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, UMODE_A, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,
    0,
    0,
    0, 0, 0, 0, 0, 0, 0,
    0,
    0, 0, 0, 0, 0,
    0, UMODE_a, 0, 0, 0, 0, 0,
    UMODE_g,
    UMODE_h, UMODE_i, 0, 0, 0, 0, 0, UMODE_o,
    0,
    0, UMODE_r, 0, 0, 0, 0, UMODE_w,
    UMODE_x,
    0,
    0,
    0, 0, 0, 0, 0
};


char myCsmodes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0,
    0,
    0, 0, 0,
    'h',                        /* (37) % Channel halfops */
    0,
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
    {CMODE_C, 0, NULL, NULL},   /* C */
    {0},                        /* D */
    {0},                        /* E */
    {0},                        /* F */
    {CMODE_G, 0, NULL, NULL},   /* G */
    {CMODE_H, CBM_NO_USER_MLOCK, NULL, NULL},
    {0},                        /* I */
    {0},                        /* J */
    {CMODE_K, 0, NULL, NULL},   /* K */
    {CMODE_L, 0, set_redirect, cs_set_redirect},
    {0},                        /* M */
    {CMODE_N, 0, NULL, NULL},   /* N */
    {CMODE_O, CBM_NO_USER_MLOCK, NULL, NULL},
    {0},                        /* P */
    {CMODE_Q, 0, NULL, NULL},   /* Q */
    {CMODE_R, 0, NULL, NULL},   /* R */
    {CMODE_S, 0, NULL, NULL},   /* S */
    {0},                        /* T */
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
    {'H', CMODE_H, 0, NULL, NULL},
    {'K', CMODE_K, 0, NULL, NULL},
    {'L', CMODE_L, 0, get_redirect, cs_get_redirect},
    {'N', CMODE_N, 0, NULL, NULL},
    {'O', CMODE_O, 0, NULL, NULL},
    {'Q', CMODE_Q, 0, NULL, NULL},
    {'R', CMODE_R, 0, NULL, NULL},
    {'S', CMODE_S, 0, NULL, NULL},
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
            if (!add)
            {
                if (user->vhost)
                    free(user->vhost);
                user->vhost = NULL;
            }
            update_host(user);
            break;
        }
    }
}


/* *INDENT-OFF* */
void moduleAddIRCDMsgs(void) {
    Message *m;

    updateProtectDetails("PROTECT","PROTECTME","protect","deprotect","AUTOPROTECT","+a","-a");

    m = createMessage("401",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("402",       anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("436",       anope_event_436); addCoreMessage(IRCD,m);
    m = createMessage("AWAY",      anope_event_away); addCoreMessage(IRCD,m);
    m = createMessage("INVITE",    anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("JOIN",      anope_event_join); addCoreMessage(IRCD,m);
    m = createMessage("KICK",      anope_event_kick); addCoreMessage(IRCD,m);
    m = createMessage("KILL",      anope_event_kill); addCoreMessage(IRCD,m);
    m = createMessage("MODE",      anope_event_mode); addCoreMessage(IRCD,m);
    m = createMessage("MOTD",      anope_event_motd); addCoreMessage(IRCD,m);
    m = createMessage("NICK",      anope_event_nick); addCoreMessage(IRCD,m);
    m = createMessage("NOTICE",    anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("PART",      anope_event_part); addCoreMessage(IRCD,m);
    m = createMessage("PASS",      anope_event_null); addCoreMessage(IRCD,m);
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
    m = createMessage("PROTOCTL",  anope_event_capab); addCoreMessage(IRCD,m);
    m = createMessage("CHGHOST",   anope_event_chghost); addCoreMessage(IRCD,m);
    m = createMessage("CHGIDENT",  anope_event_chgident); addCoreMessage(IRCD,m);
    m = createMessage("CHGNAME",   anope_event_chgname); addCoreMessage(IRCD,m);
    m = createMessage("NETINFO",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SETHOST",   anope_event_sethost); addCoreMessage(IRCD,m);
    m = createMessage("SETIDENT",  anope_event_setident); addCoreMessage(IRCD,m);
    m = createMessage("SETNAME",   anope_event_setname); addCoreMessage(IRCD,m);
    m = createMessage("REHASH",    anope_event_rehash); addCoreMessage(IRCD,m);
    m = createMessage("ADMIN",     anope_event_admin); addCoreMessage(IRCD,m);
    m = createMessage("CREDITS",   anope_event_credits); addCoreMessage(IRCD,m);
}

/* *INDENT-ON* */

/* Event: PROTOCTL */
int anope_event_capab(char *source, int ac, char **av)
{
    capab_parse(ac, av);
    return MOD_CONT;
}

void unreal_cmd_svsnoop(char *server, int set)
{
    send_cmd(NULL, "SVSNOOP %s %s", server, (set ? "+" : "-"));
}

void unreal_cmd_svsadmin(char *server, int set)
{
    unreal_cmd_svsnoop(server, set);
}

void unreal_cmd_remove_akill(char *user, char *host)
{
    send_cmd(NULL, "TKL - G %s %s %s", user, host, s_OperServ);
}

void unreal_cmd_topic(char *whosets, char *chan, char *whosetit,
                      char *topic, time_t when)
{
    send_cmd(whosets, "TOPIC %s %s %lu :%s", chan, whosetit,
             (unsigned long int) when, topic);
}

void unreal_cmd_vhost_off(User * u)
{
    common_svsmode(u, "-xt", NULL);
    common_svsmode(u, "+x", NULL);
    notice_lang(s_HostServ, u, HOST_OFF_UNREAL, u->nick, ircd->vhostchar);
}

void unreal_cmd_akill(char *user, char *host, char *who, time_t when,
                      time_t expires, char *reason)
{
    send_cmd(NULL, "TKL + G %s %s %s %ld %ld :%s", user, host, who,
             (long int) time(NULL) + 86400 * 2, (long int) when, reason);
}

void unreal_cmd_svskill(char *source, char *user, char *buf)
{
    if (!buf) {
        return;
    }

    if (!source || !user) {
        return;
    }

    send_cmd(source, "SVSKILL %s :%s", user, buf);
}

void unreal_cmd_svsmode(User * u, int ac, char **av)
{
    send_cmd(ServerName, "SVSMODE %s %s%s%s", u->nick, av[0],
             (ac == 2 ? " " : ""), (ac == 2 ? av[1] : ""));
}


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
    send_cmd(NULL, "NICK %s 1 %ld %s %s %s 0 %s * :%s", nick,
             (long int) time(NULL), ServiceUser, ServiceHost, ServerName,
             modes, name);
    unreal_cmd_sqline(nick, "Reserved for services");
}

void unreal_cmd_guest_nick(char *nick, char *user, char *host, char *real,
                           char *modes)
{
    send_cmd(NULL, "NICK %s 1 %ld %s %s %s 0 %s * :%s", nick,
             (long int) time(NULL), user, host, ServerName, modes, real);
}

void unreal_cmd_mode(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "MODE %s %s", dest, buf);
}

void unreal_cmd_bot_nick(char *nick, char *user, char *host, char *real,
                         char *modes)
{
    EnforceQlinedNick(nick, s_BotServ);
    send_cmd(NULL, "NICK %s 1 %ld %s %s %s 0 %s * :%s", nick,
             (long int) time(NULL), user, host, ServerName, modes, real);
    unreal_cmd_sqline(nick, "Reserved for services");
}

void unreal_cmd_kick(char *source, char *chan, char *user, char *buf)
{
    if (buf) {
        send_cmd(source, "KICK %s %s :%s", chan, user, buf);
    } else {
        send_cmd(source, "KICK %s %s", chan, user);
    }
}

void unreal_cmd_notice_ops(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "NOTICE @%s :%s", dest, buf);
}


void unreal_cmd_notice(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    if (NSDefFlags & NI_MSG) {
        unreal_cmd_privmsg2(source, dest, buf);
    } else {
        send_cmd(source, "NOTICE %s :%s", dest, buf);
    }
}

void unreal_cmd_notice2(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE %s :%s", dest, msg);
}

void unreal_cmd_privmsg(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "PRIVMSG %s :%s", dest, buf);
}

void unreal_cmd_privmsg2(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG %s :%s", dest, msg);
}

void unreal_cmd_serv_notice(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE $%s :%s", dest, msg);
}

void unreal_cmd_serv_privmsg(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG $%s :%s", dest, msg);
}


void unreal_cmd_bot_chan_mode(char *nick, char *chan)
{
    anope_cmd_mode(nick, chan, "%s %s %s", ircd->botchanumode, nick, nick);
}

void unreal_cmd_351(char *source)
{
    send_cmd(ServerName, "351 %s Anope-%s %s :%s - %s (%s) -- %s",
             source, version_number, ServerName, ircd->name, version_flags,
             EncModule, version_build);
}

/* QUIT */
void unreal_cmd_quit(char *source, char *buf)
{
    if (buf) {
        send_cmd(source, "QUIT :%s", buf);
    } else {
        send_cmd(source, "QUIT");
    }
}

/* PROTOCTL */
void unreal_cmd_protoctl()
{
    /*
       See Unreal's protoctl.txt for reference
       VHP - Send hostnames in NICKv2 even if not sethosted 
     */
    send_cmd(NULL, "PROTOCTL NICKv2 VHP");
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
    send_cmd(NULL, "SERVER %s %d :%s", servname, hop, descript);
}

/* PONG */
void unreal_cmd_pong(char *servname, char *who)
{
    send_cmd(servname, "PONG %s", who);
}

/* JOIN */
/* Althought Unreal 3.2 does not need the timestamp others do so
   we get it in the common function call */
void unreal_cmd_join(char *user, char *channel, time_t chantime)
{
    send_cmd(user, "JOIN %s", channel);
}

/* UNSQLINE */
void unreal_cmd_unsqline(char *user)
{
    if (!user) {
        return;
    }
    send_cmd(NULL, "UNSQLINE %s", user);
}

/* CHGHOST */
void unreal_cmd_chghost(char *nick, char *vhost)
{
    if (!nick || !vhost) {
        return;
    }
    send_cmd(ServerName, "CHGHOST %s %s", nick, vhost);
}

/* CHGIDENT */
void unreal_cmd_chgident(char *nick, char *vIdent)
{
    if (!nick || !vIdent) {
        return;
    }
    send_cmd(ServerName, "CHGIDENT %s %s", nick, vIdent);
}

/* INVITE */
void unreal_cmd_invite(char *source, char *chan, char *nick)
{
    if (!source || !chan || !nick) {
        return;
    }

    send_cmd(source, "INVITE %s %s", nick, chan);
}

/* PART */
void unreal_cmd_part(char *nick, char *chan, char *buf)
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
void unreal_cmd_391(char *source, char *timestr)
{
    if (!timestr) {
        return;
    }
    send_cmd(NULL, "391 :%s %s :%s", source, ServerName, timestr);
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

    send_cmd(source ? source : ServerName, "GLOBOPS :%s", buf);
}

/* SQLINE */
void unreal_cmd_sqline(char *mask, char *reason)
{
    if (!mask || !reason) {
        return;
    }

    send_cmd(NULL, "SQLINE %s :%s", mask, reason);
}

/* SQUIT */
void unreal_cmd_squit(char *servname, char *message)
{
    if (!servname || !message) {
        return;
    }

    send_cmd(NULL, "SQUIT %s :%s", servname, message);
}

/* SVSO */
void unreal_cmd_svso(char *source, char *nick, char *flag)
{
    if (!source || !nick || !flag) {
        return;
    }

    send_cmd(source, "SVSO %s %s", nick, flag);
}

/* NICK <newnick>  */
void unreal_cmd_chg_nick(char *oldnick, char *newnick)
{
    if (!oldnick || !newnick) {
        return;
    }

    send_cmd(oldnick, "NICK %s", newnick);
}

/* SVSNICK */
void unreal_cmd_svsnick(char *source, char *guest, time_t when)
{
    if (!source || !guest) {
        return;
    }
    send_cmd(NULL, "SVSNICK %s %s :%ld", source, guest, (long int) when);
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
    me_server =
        new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);

    unreal_cmd_protoctl();
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

    change_user_host(u, av[0]);
    return MOD_CONT;
}


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
        } else {
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

/* SVSHOLD - set */
void unreal_cmd_svshold(char *nick)
{
    /* Not supported by this IRCD */
}

/* SVSHOLD - release */
void unreal_cmd_release_svshold(char *nick)
{
    /* Not Supported by this IRCD */
}

/* UNSGLINE */
void unreal_cmd_unsgline(char *mask)
{
    /* Not Supported by this IRCD */
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
void unreal_cmd_sgline(char *mask, char *reason)
{
    /* Not Supported by this IRCD */
}

void unreal_cmd_unban(char *name, char *nick)
{
    /* Not Supported by this IRCD */
}

/* SVSMODE channel modes */

void unreal_cmd_svsmode_chan(char *name, char *mode, char *nick)
{
    /* Not Supported by this IRCD */
}


/* SVSMODE +d */
/* sent if svid is something weird */
void unreal_cmd_svid_umode(char *nick, time_t ts)
{
    send_cmd(ServerName, "SVSMODE %s +d 1", nick);
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

/* svsjoin
	parv[0] - sender
	parv[1] - nick to make join
	parv[2] - channel(s) to join
        parv[3] - (optional) channel key(s)
*/
void unreal_cmd_svsjoin(char *source, char *nick, char *chan, char *param)
{
    if (param) {
        send_cmd(source, "SVSJOIN %s %s :%s", nick, chan, param);
    } else {
        send_cmd(source, "SVSJOIN %s :%s", nick, chan);
    }
}

/* svspart
	parv[0] - sender
	parv[1] - nick to make part
	parv[2] - channel(s) to part
*/
void unreal_cmd_svspart(char *source, char *nick, char *chan)
{
    send_cmd(source, "SVSPART %s :%s", nick, chan);
}

void unreal_cmd_swhois(char *source, char *who, char *mask)
{
    /* Can anyone tell me if 3.1 has this? */
}

void unreal_cmd_eob()
{
    /* Can anyone tell me if 3.1 has this? */
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

int unreal_flood_mode_check(char *value)
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

/* GLOBOPS - to handle old WALLOPS */
void unreal_cmd_global_legacy(char *source, char *fmt)
{
    send_cmd(source ? source : ServerName, "%s :%s",
             send_token("GLOBOPS", "]"), fmt);
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

    send_cmd(source, "NOTICE %s :\1%s \1", dest, s);
    free(s);
}


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
}

/** 
 * Now tell anope how to use us.
 **/
int AnopeInit(int argc, char **argv)
{

    moduleAddAuthor("Anope");
    moduleAddVersion(VERSION_STRING);
    moduleSetType(PROTOCOL);

    pmodule_ircd_version("UnrealIRCd 3.1.1+");
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
