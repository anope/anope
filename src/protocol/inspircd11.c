/* inspircd 1.1 beta 6+ functions
 *
 * (C) 2005-2007 Craig Edwards <brain@inspircd.org>
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
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
#include "inspircd11.h"

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef _WIN32
#include "winsock.h"
int inet_aton(const char *name, struct in_addr *addr)
{
    uint32 a = inet_addr(name);
    addr->s_addr = a;
    return a != (uint32) - 1;
}
#endif

IRCDVar myIrcd[] = {
    {"InspIRCd 1.1",            /* ircd name */
     "+oI",                     /* nickserv mode */
     "+oI",                     /* chanserv mode */
     "+oI",                     /* memoserv mode */
     "+oI",                     /* hostserv mode */
     "+ioI",                    /* operserv mode */
     "+oI",                     /* botserv mode  */
     "+oI",                     /* helpserv mode */
     "+iI",                     /* Dev/Null mode */
     "+ioI",                    /* Global mode   */
     "+oI",                     /* nickserv alias mode */
     "+oI",                     /* chanserv alias mode */
     "+oI",                     /* memoserv alias mode */
     "+ioI",                    /* hostserv alias mode */
     "+ioI",                    /* operserv alias mode */
     "+oI",                     /* botserv alias mode  */
     "+oI",                     /* helpserv alias mode */
     "+iI",                     /* Dev/Null alias mode */
     "+ioI",                    /* Global alias mode   */
     "+sI",                     /* Used by BotServ Bots */
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
     "+r",                      /* Mode On Reg          */
     NULL,                      /* Mode on ID for Roots */
     NULL,                      /* Mode on ID for Admins */
     NULL,                      /* Mode on ID for Opers */
     "-r",                      /* Mode on UnReg        */
     "-r",                      /* Mode on Nick Change  */
     1,                         /* Supports SGlines     */
     1,                         /* Supports SQlines     */
     1,                         /* Supports SZlines     */
     1,                         /* Supports Halfop +h   */
     4,                         /* Number of server args */
     0,                         /* Join 2 Set           */
     1,                         /* Join 2 Message       */
     0,                         /* Has exceptions +e    */
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
     0,                         /* svshold              */
     0,                         /* time stamp on mode   */
     0,                         /* NICKIP               */
     1,                         /* O:LINE               */
     1,                         /* UMODE               */
     1,                         /* VHOST ON NICK        */
     0,                         /* Change RealName      */
     CMODE_K,                   /* No Knock             */
     0,                         /* Admin Only           */
     DEFAULT_MLOCK,             /* Default MLOCK       */
     UMODE_x,                   /* Vhost Mode           */
     0,                         /* +f                   */
     1,                         /* +L                   */
     CMODE_f,
     CMODE_L,
     0,
     1,                         /* No Knock requires +i */
     NULL,                      /* CAPAB Chan Modes             */
     0,                         /* We support inspircd TOKENS */
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
     1,                         /* CIDR channelbans */
     "$",                       /* TLD Prefix for Global */
     }
    ,
    {NULL}
};


IRCDCAPAB myIrcdcap[] = {
    {
     CAPAB_NOQUIT,              /* NOQUIT       */
     0,                         /* TSMODE       */
     1,                         /* UNCONNECT    */
     0,                         /* NICKIP       */
     0,                         /* SJOIN        */
     0,                         /* ZIP          */
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
     0,                         /* TOKEN        */
     0,                         /* VHOST        */
     CAPAB_SSJ3,                /* SSJ3         */
     CAPAB_NICK2,               /* NICK2        */
     0,                         /* UMODE2       */
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
    'a',
    0, 0, 0, 0,

    'v', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    'o', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'q', 0
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
    {NULL, NULL},
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
    {CMODE_u, 0, NULL, NULL},
    {0},                        /* v */
    {0},                        /* w */
    {0},                        /* x */
    {0},                        /* y */
    {CMODE_z, 0, NULL, NULL},
    {0}, {0}, {0}, {0}
};

CBModeInfo myCbmodeinfos[] = {
    {'f', CMODE_f, 0, NULL, NULL},
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

static int has_servicesmod = 0;
static int has_globopsmod = 0;

/* These are sanity checks to insure we are supported.
   The ircd tends to /squit us if we issue unsupported cmds.
   - katsklaw */
static int has_svsholdmod = 0;
static int has_chghostmod = 0;
static int has_chgidentmod = 0;
static int has_messagefloodmod = 0;
static int has_banexceptionmod = 0;
static int has_inviteexceptionmod = 0;

void inspircd_set_umode(User * user, int ac, const char **av)
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
			user->svid = (add ? user->timestamp : 0);
            if (add && !nick_identified(user)) {
                common_svsmode(user, "-r", NULL);
                user->mode &= ~UMODE_r;
            }
            break;
        case 'x':
			if (add) user->chost = user->vhost;
            update_host(user);
            break;
        }
    }
}


/* *INDENT-OFF* */
void moduleAddIRCDMsgs(void) {
    Message *m;

    updateProtectDetails("PROTECT","PROTECTME","protect","deprotect","AUTOPROTECT","+a","-a");

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
    m = createMessage("BURST",     anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("ENDBURST",  anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("CAPAB",     anope_event_capab); addCoreMessage(IRCD,m);
    m = createMessage("PART",      anope_event_part); addCoreMessage(IRCD,m);
    m = createMessage("PING",      anope_event_ping); addCoreMessage(IRCD,m);
    m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
    m = createMessage("QUIT",      anope_event_quit); addCoreMessage(IRCD,m);
    m = createMessage("SERVER",    anope_event_server); addCoreMessage(IRCD,m);
    m = createMessage("SQUIT",     anope_event_squit); addCoreMessage(IRCD,m);
    m = createMessage("RSQUIT",    anope_event_rsquit); addCoreMessage(IRCD,m);
    m = createMessage("TOPIC",     anope_event_topic); addCoreMessage(IRCD,m);
    m = createMessage("WALLOPS",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("WHOIS",     anope_event_whois); addCoreMessage(IRCD,m);
    m = createMessage("GLOBOPS",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SILENCE",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SVSMODE",   anope_event_mode) ;addCoreMessage(IRCD,m);
    m = createMessage("QLINE",     anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("GLINE",     anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("ELINE",     anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("ZLINE",     anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("ADDLINE",   anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("FHOST",     anope_event_chghost); addCoreMessage(IRCD,m);
    m = createMessage("CHGIDENT",  anope_event_chgident); addCoreMessage(IRCD,m);
    m = createMessage("FNAME",     anope_event_chgname); addCoreMessage(IRCD,m);
    m = createMessage("METADATA",  anope_event_null); addCoreMessage(IRCD,m);
    m = createMessage("SETHOST",   anope_event_sethost); addCoreMessage(IRCD,m);
    m = createMessage("SETIDENT",  anope_event_setident); addCoreMessage(IRCD,m);
    m = createMessage("SETNAME",   anope_event_setname); addCoreMessage(IRCD,m);
    m = createMessage("REHASH",    anope_event_rehash); addCoreMessage(IRCD,m);
    m = createMessage("ADMIN",     anope_event_admin); addCoreMessage(IRCD,m);
    m = createMessage("CREDITS",   anope_event_credits); addCoreMessage(IRCD,m);
    m = createMessage("FJOIN",     anope_event_fjoin); addCoreMessage(IRCD,m);
    m = createMessage("FMODE",     anope_event_fmode); addCoreMessage(IRCD,m);
    m = createMessage("FTOPIC",    anope_event_ftopic); addCoreMessage(IRCD,m);
    m = createMessage("VERSION",   anope_event_version); addCoreMessage(IRCD,m);
    m = createMessage("OPERTYPE",  anope_event_opertype); addCoreMessage(IRCD,m);
    m = createMessage("IDLE",      anope_event_idle); addCoreMessage(IRCD,m);
}

/* *INDENT-ON* */

void inspircd_cmd_svsadmin(const char *server, int set)
{
    /* Not Supported by this IRCD */
}

void InspIRCdProto::cmd_remove_akill(const char *user, const char *host)
{
	send_cmd(s_OperServ, "GLINE %s@%s", user, host);
}

void InspIRCdProto::cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when)
{
	send_cmd(whosets, "FTOPIC %s %lu %s :%s", chan, static_cast<unsigned long>(when), whosetit, topic);
}

/* CHGHOST */
void inspircd_cmd_chghost(const char *nick, const char *vhost)
{
    if (has_chghostmod == 1) {
    if (!nick || !vhost) {
        return;
    }
    send_cmd(s_OperServ, "CHGHOST %s %s", nick, vhost);
    } else {
	anope_cmd_global(s_OperServ, "CHGHOST not loaded!");
    }
}

void InspIRCdProto::cmd_vhost_off(User *u)
{
	inspircd_cmd_chghost(u->nick, (u->mode & umodes['x']) ? u->chost.c_str() : u->host);
}

void InspIRCdProto::cmd_akill(const char *user, const char *host, const char *who, time_t when, time_t expires, const char *reason)
{
	// Calculate the time left before this would expire, capping it at 2 days
	time_t timeleft = expires - time(NULL);
	if (timeleft > 172800) timeleft = 172800;
	send_cmd(ServerName, "ADDLINE G %s@%s %s %ld %ld :%s", user, host, who, static_cast<long>(when), static_cast<long>(timeleft), reason);
}

void InspIRCdProto::cmd_svskill(const char *source, const char *user, const char *buf)
{
	if (!buf || !source || !user) return;
	send_cmd(source, "KILL %s :%s", user, buf);
}

void InspIRCdProto::cmd_svsmode(User *u, int ac, const char **av)
{
	/* This was originally done using this:
	   send_cmd(s_NickServ, "MODE %s %s%s%s", u->nick, av[0], (ac == 2 ? " " : ""), (ac == 2 ? av[1] : ""));
	   * but that's the dirty way of doing things... */
	send_cmd(s_NickServ, "MODE %s %s", u->nick, merge_args(ac, av));
}


void inspircd_cmd_372(const char *source, const char *msg)
{
    send_cmd(ServerName, "372 %s :- %s", source, msg);
}

void inspircd_cmd_372_error(const char *source)
{
    send_cmd(ServerName, "422 %s :- MOTD file not found!  Please "
             "contact your IRC administrator.", source);
}

void inspircd_cmd_375(const char *source)
{
    send_cmd(ServerName, "375 %s :- %s Message of the Day",
             source, ServerName);
}

void inspircd_cmd_376(const char *source)
{
    send_cmd(ServerName, "376 %s :End of /MOTD command.", source);
}

void InspIRCdProto::cmd_nick(const char *nick, const char *name, const char *modes)
{
	/* :test.chatspike.net NICK 1133519355 Brain synapse.brainbox.winbot.co.uk netadmin.chatspike.net ~brain +xwsioS 10.0.0.2 :Craig Edwards */
	send_cmd(ServerName, "NICK %ld %s %s %s %s +%s 0.0.0.0 :%s", static_cast<long>(time(NULL)), nick, ServiceHost, ServiceHost, ServiceUser, modes, name);
	/* Don't send ServerName as the source here... -GD */
	send_cmd(nick, "OPERTYPE Service");
}

void InspIRCdProto::cmd_guest_nick(const char *nick, const char *user, const char *host, const char *real, const char *modes)
{
	send_cmd(ServerName, "NICK %ld %s %s %s %s +%s 0.0.0.0 :%s", static_cast<long>(time(NULL)), nick, host, host, user, modes, real);
}

void InspIRCdProto::cmd_mode(const char *source, const char *dest, const char *buf)
{
	if (!buf) return;
	Channel *c = findchan(dest);
	send_cmd(source ? source : s_OperServ, "FMODE %s %u %s", dest, static_cast<unsigned>(c ? c->creation_time : time(NULL)), buf);
}

int anope_event_version(const char *source, int ac, const char **av)
{
    return MOD_CONT;
}

int anope_event_idle(const char *source, int ac, const char **av)
{
    if (ac == 1) {
        send_cmd(av[0], "IDLE %s %ld 0", source, (long int) time(NULL));
    }
    return MOD_CONT;
}

int anope_event_ftopic(const char *source, int ac, const char **av)
{
    /* :source FTOPIC channel ts setby :topic */
    const char *temp;
    if (ac < 4)
        return MOD_CONT;
    temp = av[1];               /* temp now holds ts */
    av[1] = av[2];              /* av[1] now holds set by */
    av[2] = temp;               /* av[2] now holds ts */
    do_topic(source, ac, av);
    return MOD_CONT;
}

int anope_event_opertype(const char *source, int ac, const char **av)
{
    /* opertype is equivalent to mode +o because servers
       dont do this directly */
    User *u;
    u = finduser(source);
    if (u && !is_oper(u)) {
        const char *newav[2];
        newav[0] = source;
        newav[1] = "+o";
        return anope_event_mode(source, 2, newav);
    } else
        return MOD_CONT;
}

int anope_event_fmode(const char *source, int ac, const char **av)
{
    const char *newav[25];
    int n, o;
    Channel *c;

    /* :source FMODE #test 12345678 +nto foo */
    if (ac < 3)
        return MOD_CONT;

    /* Checking the TS for validity to avoid desyncs */
    if ((c = findchan(av[0]))) {
        if (c->creation_time > strtol(av[1], NULL, 10)) {
            /* Our TS is bigger, we should lower it */
            c->creation_time = strtol(av[1], NULL, 10);
        } else if (c->creation_time < strtol(av[1], NULL, 10)) {
            /* The TS we got is bigger, we should ignore this message. */
            return MOD_CONT;
        }
    } else {
        /* Got FMODE for a non-existing channel */
    	return MOD_CONT;
    }

    /* TS's are equal now, so we can proceed with parsing */
    n = o = 0;
    while (n < ac) {
        if (n != 1) {
            newav[o] = av[n];
            o++;
            if (debug)
                alog("Param: %s", newav[o - 1]);
        }
        n++;
    }

    return anope_event_mode(source, ac - 1, newav);
}

int anope_event_fjoin(const char *source, int ac, const char **av)
{
    const char *newav[10];

    /* value used for myStrGetToken */
    int curtoken = 0;

    /* storing the current nick */
    char *curnick;

    /* these are used to generate the final string that is passed to ircservices' core */
    int nlen = 0;
    char nicklist[514];

    /* temporary buffer */
    char prefixandnick[60];

    *nicklist = '\0';
    *prefixandnick = '\0';

    if (ac < 3)
        return MOD_CONT;

    curnick = myStrGetToken(av[2], ' ', curtoken);
    while (curnick != NULL) {
        for (; *curnick; curnick++) {
            /* I bet theres a better way to do this... */
            if ((*curnick == '&') ||
                (*curnick == '~') || (*curnick == '@') || (*curnick == '%')
                || (*curnick == '+')) {
                prefixandnick[nlen++] = *curnick;
                continue;
            } else {
                if (*curnick == ',') {
                    curnick++;
                    strncpy(prefixandnick + nlen, curnick,
                            sizeof(prefixandnick) - nlen);
                    break;
                } else {
                    alog("fjoin: unrecognised prefix: %c", *curnick);
                }
            }
        }
        strncat(nicklist, prefixandnick, 513);
        strncat(nicklist, " ", 513);
        curtoken++;
        curnick = myStrGetToken(av[2], ' ', curtoken);
        nlen = 0;
    }

    newav[0] = av[1];           /* timestamp */
    newav[1] = av[0];           /* channel name */
    newav[2] = "+";             /* channel modes */
    newav[3] = nicklist;
    do_sjoin(source, 4, newav);

    return MOD_CONT;
}

void InspIRCdProto::cmd_bot_nick(const char *nick, const char *user, const char *host, const char *real, const char *modes)
{
	send_cmd(ServerName, "NICK %ld %s %s %s %s +%s 0.0.0.0 :%s", static_cast<long>(time(NULL)), nick, host, host, user, modes, real);
	send_cmd(nick, "OPERTYPE Bot");
}

void InspIRCdProto::cmd_kick(const char *source, const char *chan, const char *user, const char *buf)
{
	if (buf) send_cmd(source, "KICK %s %s :%s", chan, user, buf);
	else send_cmd(source, "KICK %s %s :%s", chan, user, user);
}

void InspIRCdProto::cmd_notice_ops(const char *source, const char *dest, const char *buf)
{
	if (!buf) return;
	send_cmd(ServerName, "NOTICE @%s :%s", dest, buf);
}

void InspIRCdProto::cmd_bot_chan_mode(const char *nick, const char *chan)
{
	anope_cmd_mode(nick, chan, "%s %s %s", ircd->botchanumode, nick, nick);
}

void inspircd_cmd_351(const char *source)
{
    send_cmd(ServerName, "351 %s Anope-%s %s :%s - %s (%s) -- %s",
             source, version_number, ServerName, ircd->name, version_flags,
             EncModule, version_build);
}

/* PROTOCTL */
void inspircd_cmd_protoctl()
{
}

static char currentpass[1024];

/* PASS */
void inspircd_cmd_pass(const char *pass)
{
    strncpy(currentpass, pass, 1024);
}

/* SERVER services-dev.chatspike.net password 0 :Description here */
void inspircd_cmd_server(const char *servname, int hop, const char *descript)
{
    send_cmd(ServerName, "SERVER %s %s %d :%s", servname, currentpass, hop,
             descript);
}

/* JOIN */
void InspIRCdProto::cmd_join(const char *user, const char *channel, time_t chantime)
{
	send_cmd(user, "JOIN %s", channel);
}

/* UNSQLINE */
void InspIRCdProto::cmd_unsqline(const char *user)
{
	if (!user) return;
	send_cmd(s_OperServ, "QLINE %s", user);
}

/* CHGIDENT */
void inspircd_cmd_chgident(const char *nick, const char *vIdent)
{
	if (has_chgidentmod == 1) {
		if (!nick || !vIdent || !*vIdent) {
        	return;
    	}
    	send_cmd(s_OperServ, "CHGIDENT %s %s", nick, vIdent);
    } else {
		anope_cmd_global(s_OperServ, "CHGIDENT not loaded!");
    }
}

/* 391 */
void inspircd_cmd_391(const char *source, const char *timestr)
{
    if (!timestr) {
        return;
    }
    send_cmd(NULL, "391 :%s %s :%s", source, ServerName, timestr);
}

/* 250 */
void inspircd_cmd_250(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "250 %s", buf);
}

/* 307 */
void inspircd_cmd_307(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "307 %s", buf);
}

/* 311 */
void inspircd_cmd_311(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "311 %s", buf);
}

/* 312 */
void inspircd_cmd_312(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "312 %s", buf);
}

/* 317 */
void inspircd_cmd_317(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "317 %s", buf);
}

/* 219 */
void inspircd_cmd_219(const char *source, const char *letter)
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
void inspircd_cmd_401(const char *source, const char *who)
{
    if (!source || !who) {
        return;
    }
    send_cmd(ServerName, "401 %s %s :No such service.", source, who);
}

/* 318 */
void inspircd_cmd_318(const char *source, const char *who)
{
    if (!source || !who) {
        return;
    }

    send_cmd(ServerName, "318 %s %s :End of /WHOIS list.", source, who);
}

/* 242 */
void inspircd_cmd_242(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "242 %s", buf);
}

/* 243 */
void inspircd_cmd_243(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "243 %s", buf);
}

/* 211 */
void inspircd_cmd_211(const char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "211 %s", buf);
}

/* SQLINE */
void InspIRCdProto::cmd_sqline(const char *mask, const char *reason)
{
	if (!mask || !reason) return;
	send_cmd(ServerName, "ADDLINE Q %s %s %ld 0 :%s", mask, s_OperServ, static_cast<long>(time(NULL)), reason);
}

/* SQUIT */
void InspIRCdProto::cmd_squit(const char *servname, const char *message)
{
	if (!servname || !message) return;
	send_cmd(ServerName, "SQUIT %s :%s", servname, message);
}

/* Functions that use serval cmd functions */

void InspIRCdProto::cmd_vhost_on(const char *nick, const char *vIdent, const char *vhost)
{
	if (!nick) return;
	if (vIdent) inspircd_cmd_chgident(nick, vIdent);
	inspircd_cmd_chghost(nick, vhost);
}

void InspIRCdProto::cmd_connect()
{
	if (servernum == 1) inspircd_cmd_pass(RemotePassword);
	else if (servernum == 2) inspircd_cmd_pass(RemotePassword2);
	else if (servernum == 3) inspircd_cmd_pass(RemotePassword3);
	inspircd_cmd_server(ServerName, 0, ServerDesc);
	send_cmd(NULL, "BURST");
	send_cmd(ServerName, "VERSION :Anope-%s %s :%s - %s (%s) -- %s", version_number, ServerName, ircd->name, version_flags, EncModule, version_build);
	me_server = new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);
}

/* Events */

int anope_event_ping(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;
    /* ((ac > 1) ? av[1] : ServerName) */
    ircd_proto.cmd_pong(ServerName, av[0]);
    return MOD_CONT;
}

int anope_event_436(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;

    m_nickcoll(av[0]);
    return MOD_CONT;
}

int anope_event_away(const char *source, int ac, const char **av)
{
    if (!source) {
        return MOD_CONT;
    }
    m_away(source, (ac ? av[0] : NULL));
    return MOD_CONT;
}

/* Taken from hybrid.c, topic syntax is identical */

int anope_event_topic(const char *source, int ac, const char **av)
{
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

    return MOD_CONT;
}

int anope_event_squit(const char *source, int ac, const char **av)
{
    if (ac != 2)
        return MOD_CONT;
    do_squit(source, ac, av);
    return MOD_CONT;
}

int anope_event_rsquit(const char *source, int ac, const char **av)
{
    if (ac < 1 || ac > 3)
        return MOD_CONT;

    /* Horrible workaround to an insp bug (#) in how RSQUITs are sent - mark */
    if (ac > 1 && strcmp(ServerName, av[0]) == 0)
        do_squit(source, ac - 1, av + 1);
    else
        do_squit(source, ac, av);

    return MOD_CONT;
}

int anope_event_quit(const char *source, int ac, const char **av)
{
    if (ac != 1)
        return MOD_CONT;
    do_quit(source, ac, av);
    return MOD_CONT;
}


int anope_event_mode(const char *source, int ac, const char **av)
{
    if (ac < 2)
        return MOD_CONT;

    if (*av[0] == '#' || *av[0] == '&') {
        do_cmode(source, ac, av);
    } else {
        /* InspIRCd lets opers change another
           users modes, we have to kludge this
           as it slightly breaks RFC1459
         */
        if (!strcasecmp(source, av[0])) {
            do_umode(source, ac, av);
        } else {
            do_umode(av[0], ac, av);
        }
    }
    return MOD_CONT;
}


int anope_event_kill(const char *source, int ac, const char **av)
{
    if (ac != 2)
        return MOD_CONT;

    m_kill(av[0], av[1]);
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
    if (ac != 2)
        return MOD_CONT;
    do_join(source, ac, av);
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

int anope_event_setname(const char *source, int ac, const char **av)
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

    u->SetRealname(av[0]);
    return MOD_CONT;
}

int anope_event_chgname(const char *source, int ac, const char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(source);
    if (!u) {
        if (debug) {
            alog("debug: FNAME for nonexistent user %s", source);
        }
        return MOD_CONT;
    }

    u->SetRealname(av[0]);
    return MOD_CONT;
}

int anope_event_setident(const char *source, int ac, const char **av)
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

    u->SetIdent(av[0]);
    return MOD_CONT;
}

int anope_event_chgident(const char *source, int ac, const char **av)
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

    u->SetIdent(av[1]);
    return MOD_CONT;
}

int anope_event_sethost(const char *source, int ac, const char **av)
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

    u->SetDisplayedHost(av[0]);
    return MOD_CONT;
}


int anope_event_nick(const char *source, int ac, const char **av)
{
    User *user;
    struct in_addr addy;
    uint32 *ad = (uint32 *) & addy;

    if (ac != 1) {
        if (ac == 8) {
			int svid = 0;
			int ts = strtoul(av[0], NULL, 10);

			if (strchr(av[5], 'r') != NULL)
				svid = ts;

            inet_aton(av[6], &addy);
            user = do_nick("", av[1],   /* nick */
                           av[4],   /* username */
                           av[2],   /* realhost */
                           source,  /* server */
                           av[7],   /* realname */
                           ts, svid, htonl(*ad), av[3], NULL);
            if (user) {
                anope_set_umode(user, 1, &av[5]);
				user->chost = av[3];
			}
        }
    } else {
        do_nick(source, av[0], NULL, NULL, NULL, NULL, 0, 0, 0, NULL,
                NULL);
    }
    return MOD_CONT;
}


int anope_event_chghost(const char *source, int ac, const char **av)
{
    User *u;

    if (ac != 1)
        return MOD_CONT;

    u = finduser(source);
    if (!u) {
        if (debug) {
            alog("debug: FHOST for nonexistent user %s", source);
        }
        return MOD_CONT;
    }

    u->SetDisplayedHost(av[0]);
    return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(const char *source, int ac, const char **av)
{
    if (!stricmp(av[1], "1")) {
        uplink = sstrdup(av[0]);
    }
    do_server(source, av[0], av[1], av[2], NULL);
    return MOD_CONT;
}


int anope_event_privmsg(const char *source, int ac, const char **av)
{
    if (ac != 2)
        return MOD_CONT;
    m_privmsg(source, av[0], av[1]);
    return MOD_CONT;
}

int anope_event_part(const char *source, int ac, const char **av)
{
    if (ac < 1 || ac > 2)
        return MOD_CONT;
    do_part(source, ac, av);
    return MOD_CONT;
}

int anope_event_whois(const char *source, int ac, const char **av)
{
    if (source && ac >= 1) {
        m_whois(source, av[0]);
    }
    return MOD_CONT;
}

int anope_event_capab(const char *source, int ac, const char **av)
{
    int argc;
    CBModeInfo *cbmi;

    if (strcasecmp(av[0], "START") == 0) {
        /* reset CAPAB */
        has_servicesmod = 0;
        has_globopsmod = 0;
	has_svsholdmod = 0;
	has_chghostmod = 0;
	has_chgidentmod = 0;

    } else if (strcasecmp(av[0], "MODULES") == 0) {
        if (strstr(av[1], "m_globops.so")) {
            has_globopsmod = 1;
        }
        if (strstr(av[1], "m_services.so")) {
            has_servicesmod = 1;
        }
	if (strstr(av[1], "m_svshold.so")) {
            has_svsholdmod = 1;
        }
	if (strstr(av[1], "m_chghost.so")) {
            has_chghostmod = 1;
        }
	if (strstr(av[1], "m_chgident.so")) {
            has_chgidentmod = 1;
        }
        if (strstr(av[1], "m_messageflood.so")) {
            has_messagefloodmod = 1;
        }
        if (strstr(av[1], "m_banexception.so")) {
            has_banexceptionmod = 1;
        }
        if (strstr(av[1], "m_inviteexception.so")) {
            has_inviteexceptionmod = 1;
        }
    } else if (strcasecmp(av[0], "END") == 0) {
        if (!has_globopsmod) {
            send_cmd(NULL,
                     "ERROR :m_globops is not loaded. This is required by Anope");
            quitmsg = "Remote server does not have the m_globops module loaded, and this is required.";
            quitting = 1;
            return MOD_STOP;
        }
        if (!has_servicesmod) {
            send_cmd(NULL,
                     "ERROR :m_services is not loaded. This is required by Anope");
            quitmsg = "Remote server does not have the m_services module loaded, and this is required.";
            quitting = 1;
            return MOD_STOP;
        }
        if (!has_svsholdmod) {
            anope_cmd_global(s_OperServ, "SVSHOLD missing, Usage disabled until module is loaded.");
        }
        if (!has_chghostmod) {
            anope_cmd_global(s_OperServ, "CHGHOST missing, Usage disabled until module is loaded.");
        }
        if (!has_chgidentmod) {
            anope_cmd_global(s_OperServ, "CHGIDENT missing, Usage disabled until module is loaded.");
        }
        if (has_messagefloodmod) {
            cbmi = myCbmodeinfos;

            /* Find 'f' in myCbmodeinfos and add the relevant bits to myCbmodes and myCbmodeinfos
             * to enable +f support if found. This is needed because we're really not set up to
             * handle modular ircds which can have modes enabled/disabled as they please :( - mark
             */
            while ((cbmi->mode != 'f')) {
                cbmi++;
            }
            if (cbmi) {
                cbmi->getvalue = get_flood;
                cbmi->csgetvalue = cs_get_flood;

                myCbmodes['f'].flag = CMODE_f;
                myCbmodes['f'].flags = 0;
                myCbmodes['f'].setvalue = set_flood;
                myCbmodes['f'].cssetvalue = cs_set_flood;

                pmodule_ircd_cbmodeinfos(myCbmodeinfos);
                pmodule_ircd_cbmodes(myCbmodes);

                ircd->fmode = 1;
            }
            else {
                alog("Support for channelmode +f can not be enabled");
                if (debug) {
                    alog("debug: 'f' missing from myCbmodeinfos");
                }
            }
        }
        if (has_banexceptionmod) {
            myCmmodes['e'].addmask = add_exception;
            myCmmodes['e'].delmask = del_exception;
            ircd->except = 1;
        }
        if (has_inviteexceptionmod) {
            myCmmodes['I'].addmask = add_invite;
            myCmmodes['I'].delmask = del_invite;
            ircd->invitemode = 1;
        }
        ircd->svshold = has_svsholdmod;

        if (has_banexceptionmod || has_inviteexceptionmod) {
            pmodule_ircd_cmmodes(myCmmodes);
        }

        /* Generate a fake capabs parsing call so things like NOQUIT work
         * fine. It's ugly, but it works....
         */
        argc = 6;
		const char *argv[] = {"NOQUIT", "SSJ3", "NICK2", "VL", "TLKEXT", "UNCONNECT"};

        capab_parse(argc, argv);
    }
    return MOD_CONT;
}

/* SVSHOLD - set */
void InspIRCdProto::cmd_svshold(const char *nick)
{
	send_cmd(s_OperServ, "SVSHOLD %s %ds :%s", nick, NSReleaseTimeout, "Being held for registered user");
}

/* SVSHOLD - release */
void InspIRCdProto::cmd_release_svshold(const char *nick)
{
	send_cmd(s_OperServ, "SVSHOLD %s", nick);
}

/* UNSZLINE */
void InspIRCdProto::cmd_unszline(const char *mask)
{
	send_cmd(s_OperServ, "ZLINE %s", mask);
}

/* SZLINE */
void inspircd_cmd_szline(const char *mask, const char *reason, const char *whom)
{
	send_cmd(ServerName, "ADDLINE Z %s %s %ld 0 :%s", mask, whom, static_cast<long>(time(NULL)), reason);
}

/* SVSMODE channel modes */

void inspircd_cmd_svsmode_chan(const char *name, const char *mode, const char *nick)
{
    /* Not Supported by this IRCD */
}


/* SVSMODE +d */
/* sent if svid is something weird */
void inspircd_cmd_svid_umode(const char *nick, time_t ts)
{
    if (debug)
        alog("debug: common_svsmode(0)");
}

/* SVSMODE +d */
/* nc_change was = 1, and there is no na->status */
void inspircd_cmd_nc_change(User * u)
{
    if (debug)
        alog("debug: common_svsmode(1)");
    common_svsmode(u, "-r", NULL);
}

/* SVSMODE +r */
void inspircd_cmd_svid_umode2(User * u, const char *ts)
{
    if (debug)
        alog("debug: common_svsmode(2)");
    common_svsmode(u, "+r", NULL);
}

void inspircd_cmd_svid_umode3(User * u, const char *ts)
{
    /* not used */
}

void inspircd_cmd_svsjoin(const char *source, const char *nick, const char *chan, const char *param)
{
    	send_cmd(source, "SVSJOIN %s %s", nick, chan);
}

void inspircd_cmd_svspart(const char *source, const char *nick, const char *chan)
{
        send_cmd(source, "SVSPART %s %s", nick, chan);
}

void inspircd_cmd_swhois(const char *source, const char *who, const char *mask)
{
	/* Not used currently */
}

void inspircd_cmd_eob()
{
    send_cmd(NULL, "ENDBURST");
}


int anope_event_rehash(const char *source, int ac, const char **av)
{
    return MOD_CONT;
}

int anope_event_credits(const char *source, int ac, const char **av)
{
    return MOD_CONT;
}

int anope_event_admin(const char *source, int ac, const char **av)
{
    return MOD_CONT;
}

int inspircd_flood_mode_check(const char *value)
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

void inspircd_cmd_jupe(const char *jserver, const char *who, const char *reason)
{
    char rbuf[256];

    snprintf(rbuf, sizeof(rbuf), "Juped by %s%s%s", who,
             reason ? ": " : "", reason ? reason : "");

    if (findserver(servlist, jserver))
        ircd_proto.cmd_squit(jserver, rbuf);
    inspircd_cmd_server(jserver, 1, rbuf);
    new_server(me_server, jserver, rbuf, SERVER_JUPED, NULL);
}

int inspircd_valid_nick(const char *nick)
{
    return 1;
}

int inspircd_valid_chan(const char *chan)
{
    return 1;
}


void inspircd_cmd_ctcp(const char *source, const char *dest, const char *buf)
{
    char *s;

    if (!buf) {
        return;
    } else {
        s = normalizeBuffer(buf);
    }

    send_cmd(source, "NOTICE %s :\1%s\1", dest, s);
    free(s);
}


/**
 * Tell anope which function we want to perform each task inside of anope.
 * These prototypes must match what anope expects.
 **/
void moduleAddAnopeCmds()
{
    pmodule_cmd_372(inspircd_cmd_372);
    pmodule_cmd_372_error(inspircd_cmd_372_error);
    pmodule_cmd_375(inspircd_cmd_375);
    pmodule_cmd_376(inspircd_cmd_376);
    pmodule_cmd_351(inspircd_cmd_351);
    pmodule_cmd_391(inspircd_cmd_391);
    pmodule_cmd_250(inspircd_cmd_250);
    pmodule_cmd_307(inspircd_cmd_307);
    pmodule_cmd_311(inspircd_cmd_311);
    pmodule_cmd_312(inspircd_cmd_312);
    pmodule_cmd_317(inspircd_cmd_317);
    pmodule_cmd_219(inspircd_cmd_219);
    pmodule_cmd_401(inspircd_cmd_401);
    pmodule_cmd_318(inspircd_cmd_318);
    pmodule_cmd_242(inspircd_cmd_242);
    pmodule_cmd_243(inspircd_cmd_243);
    pmodule_cmd_211(inspircd_cmd_211);
    pmodule_cmd_svsmode_chan(inspircd_cmd_svsmode_chan);
    pmodule_cmd_svid_umode(inspircd_cmd_svid_umode);
    pmodule_cmd_nc_change(inspircd_cmd_nc_change);
    pmodule_cmd_svid_umode2(inspircd_cmd_svid_umode2);
    pmodule_cmd_svid_umode3(inspircd_cmd_svid_umode3);
    pmodule_cmd_svsjoin(inspircd_cmd_svsjoin);
    pmodule_cmd_svspart(inspircd_cmd_svspart);
    pmodule_cmd_swhois(inspircd_cmd_swhois);
    pmodule_cmd_eob(inspircd_cmd_eob);
    pmodule_flood_mode_check(inspircd_flood_mode_check);
    pmodule_cmd_jupe(inspircd_cmd_jupe);
    pmodule_valid_nick(inspircd_valid_nick);
    pmodule_valid_chan(inspircd_valid_chan);
    pmodule_cmd_ctcp(inspircd_cmd_ctcp);
    pmodule_set_umode(inspircd_set_umode);
}

/**
 * Now tell anope how to use us.
 **/
int AnopeInit(int argc, char **argv)
{

    moduleAddAuthor("Anope");
    moduleAddVersion
        ("$Id$");
    moduleSetType(PROTOCOL);

    pmodule_ircd_version("inspircdIRCd 1.1");
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

    moduleAddAnopeCmds();
	pmodule_ircd_proto(&ircd_proto);
    moduleAddIRCDMsgs();

    return MOD_CONT;
}

