/* inspircd 1.1 beta 6+ functions
 *
 * (C) 2005-2007 Craig Edwards <brain@inspircd.org>
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
#include "inspircd11.h"
#include "version.h"

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
     "+I",                      /* nickserv mode */
     "+I",                      /* chanserv mode */
     "+I",                      /* memoserv mode */
     "+I",                      /* hostserv mode */
     "+ioI",                    /* operserv mode */
     "+I",                      /* botserv mode  */
     "+I",                      /* helpserv mode */
     "+iI",                     /* Dev/Null mode */
     "+iI",                     /* Global mode   */
     "+I",                      /* nickserv alias mode */
     "+I",                      /* chanserv alias mode */
     "+I",                      /* memoserv alias mode */
     "+I",                      /* hostserv alias mode */
     "+ioI",                    /* operserv alias mode */
     "+I",                      /* botserv alias mode  */
     "+I",                      /* helpserv alias mode */
     "+iI",                     /* Dev/Null alias mode */
     "+iI",                     /* Global alias mode   */
     "+I",                      /* Used by BotServ Bots */
     5,                         /* Chan Max Symbols     */
     "-cijlmnpstuzACGHKNOQRSV", /* Modes to Remove */
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
    {'f', CMODE_f, 0, NULL, NULL},
    {'c', CMODE_c, 0, NULL, NULL},
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

void inspircd_set_umode(User * user, int ac, char **av)
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

void inspircd_cmd_svsnoop(char *server, int set)
{
    /* Not Supported by this IRCD */
}

void inspircd_cmd_svsadmin(char *server, int set)
{
    /* Not Supported by this IRCD */
}

void inspircd_cmd_remove_akill(char *user, char *host)
{
    send_cmd(s_OperServ, "GLINE %s@%s", user, host);
}

void
inspircd_cmd_topic(char *whosets, char *chan, char *whosetit,
                   char *topic, time_t when)
{
    send_cmd(whosets, "FTOPIC %s %lu %s :%s", chan,
             (unsigned long int) when, whosetit, topic);
}

void inspircd_cmd_vhost_off(User * u)
{
	 send_cmd(s_OperServ,  "MODE %s -%s",  u->nick,  myIrcd->vhostchar);
	 inspircd_cmd_chghost(u->nick, u->host);

	 if (has_chgidentmod && u->username && u->vident && strcmp(u->username, u->vident) > 0)
	 {
		 inspircd_cmd_chgident(u->nick, u->username);
	 }
}

void
inspircd_cmd_akill(char *user, char *host, char *who, time_t when,
                   time_t expires, char *reason)
{
    send_cmd(ServerName, "ADDLINE G %s@%s %s %ld %ld :%s", user, host, who,
             (long int) time(NULL), (long int) 86400 * 2, reason);
}

void inspircd_cmd_svskill(char *source, char *user, char *buf)
{
    if (!buf || !source || !user)
        return;

    send_cmd(source, "KILL %s :%s", user, buf);
}

void inspircd_cmd_svsmode(User * u, int ac, char **av)
{
    /* This was originally done using this:
       send_cmd(s_NickServ, "MODE %s %s%s%s", u->nick, av[0], (ac == 2 ? " " : ""), (ac == 2 ? av[1] : ""));
       * but that's the dirty way of doing things...
     */
    send_cmd(s_NickServ, "MODE %s %s", u->nick, merge_args(ac, av));
}


void inspircd_cmd_372(char *source, char *msg)
{
    send_cmd(ServerName, "372 %s :- %s", source, msg);
}

void inspircd_cmd_372_error(char *source)
{
    send_cmd(ServerName, "422 %s :- MOTD file not found!  Please "
             "contact your IRC administrator.", source);
}

void inspircd_cmd_375(char *source)
{
    send_cmd(ServerName, "375 %s :- %s Message of the Day",
             source, ServerName);
}

void inspircd_cmd_376(char *source)
{
    send_cmd(ServerName, "376 %s :End of /MOTD command.", source);
}

void inspircd_cmd_nick(char *nick, char *name, char *modes)
{
    /* :test.chatspike.net NICK 1133519355 Brain synapse.brainbox.winbot.co.uk netadmin.chatspike.net ~brain +xwsioS 10.0.0.2 :Craig Edwards */
    send_cmd(ServerName, "NICK %ld %s %s %s %s +%s 0.0.0.0 :%s",
             (long int) time(NULL), nick, ServiceHost, ServiceHost,
             ServiceUser, modes, name);
    if (strchr(modes, 'o') != NULL)
        /* Don't send ServerName as the source here... -GD */
        send_cmd(nick, "OPERTYPE Service");
}

void
inspircd_cmd_guest_nick(char *nick, char *user, char *host,
                        char *real, char *modes)
{
    send_cmd(ServerName, "NICK %ld %s %s %s %s +%s 0.0.0.0 :%s",
             (long int) time(NULL), nick, host, host, user, modes, real);
}

void inspircd_cmd_mode(char *source, char *dest, char *buf)
{
    Channel *c;
    if (!buf) {
        return;
    }
    
    c = findchan(dest);
    send_cmd(source ? source : s_OperServ, "FMODE %s %u %s", dest, (unsigned int)((c) ? c->creation_time : time(NULL)), buf);
}

int anope_event_version(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_idle(char *source, int ac, char **av)
{
    if (ac == 1) {
        send_cmd(av[0], "IDLE %s %ld 0", source, (long int) time(NULL));
    }
    return MOD_CONT;
}

int anope_event_ftopic(char *source, int ac, char **av)
{
    /* :source FTOPIC channel ts setby :topic */
    char *temp;
    if (ac < 4)
        return MOD_CONT;
    temp = av[1];               /* temp now holds ts */
    av[1] = av[2];              /* av[1] now holds set by */
    av[2] = temp;               /* av[2] now holds ts */
    do_topic(source, ac, av);
    return MOD_CONT;
}

int anope_event_opertype(char *source, int ac, char **av)
{
    /* opertype is equivalent to mode +o because servers
       dont do this directly */
    User *u;
    u = finduser(source);
    if (u && !is_oper(u)) {
        char *newav[2];
        newav[0] = source;
        newav[1] = "+o";
        return anope_event_mode(source, 2, newav);
    } else
        return MOD_CONT;
}

int anope_event_fmode(char *source, int ac, char **av)
{
    char *newav[128];
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

int anope_event_fjoin(char *source, int ac, char **av)
{
    char *newav[10];

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

void
inspircd_cmd_bot_nick(char *nick, char *user, char *host, char *real,
                      char *modes)
{
    send_cmd(ServerName, "NICK %ld %s %s %s %s +%s 0.0.0.0 :%s",
             (long int) time(NULL), nick, host, host, user, modes, real);
    if (strchr(modes, 'o') != NULL)
        send_cmd(nick, "OPERTYPE Bot");
}

void inspircd_cmd_kick(char *source, char *chan, char *user, char *buf)
{
    if (buf) {
        send_cmd(source, "KICK %s %s :%s", chan, user, buf);
    } else {
        send_cmd(source, "KICK %s %s :%s", chan, user, user);
    }
}

void inspircd_cmd_notice_ops(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "NOTICE @%s :%s", dest, buf);
}


void inspircd_cmd_notice(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    if (NSDefFlags & NI_MSG) {
        inspircd_cmd_privmsg2(source, dest, buf);
    } else {
        send_cmd(source, "NOTICE %s :%s", dest, buf);
    }
}

void inspircd_cmd_notice2(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE %s :%s", dest, msg);
}

void inspircd_cmd_privmsg(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source, "PRIVMSG %s :%s", dest, buf);
}

void inspircd_cmd_privmsg2(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG %s :%s", dest, msg);
}

void inspircd_cmd_serv_notice(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE $%s :%s", dest, msg);
}

void inspircd_cmd_serv_privmsg(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG $%s :%s", dest, msg);
}


void inspircd_cmd_bot_chan_mode(char *nick, char *chan)
{
    anope_cmd_mode(nick, chan, "%s %s %s", ircd->botchanumode, nick, nick);
}

void inspircd_cmd_351(char *source)
{
    send_cmd(ServerName, "351 %s Anope-%s %s :%s - %s (%s) -- %s",
             source, version_number, ServerName, ircd->name, version_flags,
             EncModule, version_build);
}

/* QUIT */
void inspircd_cmd_quit(char *source, char *buf)
{
    if (buf) {
        send_cmd(source, "QUIT :%s", buf);
    } else {
        send_cmd(source, "QUIT :Exiting");
    }
}

/* PROTOCTL */
void inspircd_cmd_protoctl()
{
}

static char currentpass[1024];

/* PASS */
void inspircd_cmd_pass(char *pass)
{
    strncpy(currentpass, pass, 1024);
}

/* SERVER services-dev.chatspike.net password 0 :Description here */
void inspircd_cmd_server(char *servname, int hop, char *descript)
{
    send_cmd(ServerName, "SERVER %s %s %d :%s", servname, currentpass, hop,
             descript);
}

/* PONG */
void inspircd_cmd_pong(char *servname, char *who)
{
    send_cmd(servname, "PONG %s", who);
}

/* JOIN */
void inspircd_cmd_join(char *user, char *channel, time_t chantime)
{
    send_cmd(user, "JOIN %s %ld", channel, chantime);
}

/* UNSQLINE */
void inspircd_cmd_unsqline(char *user)
{
    if (!user) {
        return;
    }
    send_cmd(s_OperServ, "QLINE %s", user);
}

/* CHGHOST */
void inspircd_cmd_chghost(char *nick, char *vhost)
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

/* CHGIDENT */
void inspircd_cmd_chgident(char *nick, char *vIdent)
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

/* INVITE */
void inspircd_cmd_invite(char *source, char *chan, char *nick)
{
    if (!source || !chan || !nick) {
        return;
    }

    send_cmd(source, "INVITE %s %s", nick, chan);
}

/* PART */
void inspircd_cmd_part(char *nick, char *chan, char *buf)
{
    if (!nick || !chan) {
        return;
    }

    if (buf) {
        send_cmd(nick, "PART %s :%s", chan, buf);
    } else {
        send_cmd(nick, "PART %s :Leaving", chan);
    }
}

/* 391 */
void inspircd_cmd_391(char *source, char *timestr)
{
    if (!timestr) {
        return;
    }
    send_cmd(NULL, "391 :%s %s :%s", source, ServerName, timestr);
}

/* 250 */
void inspircd_cmd_250(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "250 %s", buf);
}

/* 307 */
void inspircd_cmd_307(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "307 %s", buf);
}

/* 311 */
void inspircd_cmd_311(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "311 %s", buf);
}

/* 312 */
void inspircd_cmd_312(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "312 %s", buf);
}

/* 317 */
void inspircd_cmd_317(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "317 %s", buf);
}

/* 219 */
void inspircd_cmd_219(char *source, char *letter)
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
void inspircd_cmd_401(char *source, char *who)
{
    if (!source || !who) {
        return;
    }
    send_cmd(ServerName, "401 %s %s :No such service.", source, who);
}

/* 318 */
void inspircd_cmd_318(char *source, char *who)
{
    if (!source || !who) {
        return;
    }

    send_cmd(ServerName, "318 %s %s :End of /WHOIS list.", source, who);
}

/* 242 */
void inspircd_cmd_242(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "242 %s", buf);
}

/* 243 */
void inspircd_cmd_243(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "243 %s", buf);
}

/* 211 */
void inspircd_cmd_211(char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(NULL, "211 %s", buf);
}

/* GLOBOPS */
void inspircd_cmd_global(char *source, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(source ? source : ServerName, "GLOBOPS :%s", buf);
}

/* SQLINE */
void inspircd_cmd_sqline(char *mask, char *reason)
{
    if (!mask || !reason) {
        return;
    }

    send_cmd(ServerName, "ADDLINE Q %s %s %ld 0 :%s", mask, s_OperServ,
             (long int) time(NULL), reason);
}

/* SQUIT */
void inspircd_cmd_squit(char *servname, char *message)
{
    if (!servname || !message) {
        return;
    }

    send_cmd(ServerName, "SQUIT %s :%s", servname, message);
}

/* SVSO */
void inspircd_cmd_svso(char *source, char *nick, char *flag)
{
}

/* NICK <newnick>  */
void inspircd_cmd_chg_nick(char *oldnick, char *newnick)
{
    if (!oldnick || !newnick) {
        return;
    }

    send_cmd(oldnick, "NICK %s", newnick);
}

/* SVSNICK */
void inspircd_cmd_svsnick(char *source, char *guest, time_t when)
{
    if (!source || !guest) {
        return;
    }
    /* Please note that inspircd will now echo back a nickchange for this SVSNICK */
    send_cmd(ServerName, "SVSNICK %s %s :%lu", source, guest,
             (unsigned long) when);
}

/* Functions that use serval cmd functions */

void inspircd_cmd_vhost_on(char *nick, char *vIdent, char *vhost)
{
    if (!nick) {
        return;
    }
    if (vIdent) {
        inspircd_cmd_chgident(nick, vIdent);
    }
    inspircd_cmd_chghost(nick, vhost);
}

void inspircd_cmd_connect(int servernum)
{
    if (servernum == 1) {
        inspircd_cmd_pass(RemotePassword);
    }
    if (servernum == 2) {
        inspircd_cmd_pass(RemotePassword2);
    }
    if (servernum == 3) {
        inspircd_cmd_pass(RemotePassword3);
    }
    inspircd_cmd_server(ServerName, 0, ServerDesc);
    send_cmd(NULL, "BURST");
    send_cmd(ServerName, "VERSION :Anope-%s %s :%s - %s (%s) -- %s",
             version_number, ServerName, ircd->name, version_flags,
             EncModule, version_build);

    me_server =
        new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);
}

void inspircd_cmd_bob()
{
    /* Not used */
}

/* Events */

int anope_event_ping(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    /* ((ac > 1) ? av[1] : ServerName) */
    inspircd_cmd_pong(ServerName, av[0]);
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

/* Taken from hybrid.c, topic syntax is identical */

int anope_event_topic(char *source, int ac, char **av)
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

int anope_event_squit(char *source, int ac, char **av)
{
    if (ac != 2)
        return MOD_CONT;
    do_squit(source, ac, av);
    return MOD_CONT;
}

int anope_event_rsquit(char *source, int ac, char **av)
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
    if (ac != 2)
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

    u = finduser(source);
    if (!u) {
        if (debug) {
            alog("debug: FNAME for nonexistent user %s", source);
        }
        return MOD_CONT;
    }

    change_user_realname(u, av[0]);
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
            if (user)
                anope_set_umode(user, 1, &av[5]);
        }
    } else {
        do_nick(source, av[0], NULL, NULL, NULL, NULL, 0, 0, 0, NULL,
                NULL);
    }
    return MOD_CONT;
}


int anope_event_chghost(char *source, int ac, char **av)
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

    change_user_host(u, av[0]);
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

int anope_event_capab(char *source, int ac, char **av)
{
    int argc;
    char **argv;
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
        argv = scalloc(argc, sizeof(char *));
        argv[0] = "NOQUIT";
        argv[1] = "SSJ3";
        argv[2] = "NICK2";
        argv[3] = "VL";
        argv[4] = "TLKEXT";
        argv[5] = "UNCONNECT";

        capab_parse(argc, argv);
    }
    return MOD_CONT;
}

/* SVSHOLD - set */
void inspircd_cmd_svshold(char *nick)
{
	send_cmd(s_OperServ, "SVSHOLD %s %ds :%s", nick, NSReleaseTimeout,
             "Being held for registered user");
}

/* SVSHOLD - release */
void inspircd_cmd_release_svshold(char *nick)
{
	send_cmd(s_OperServ, "SVSHOLD %s", nick);
}

/* UNSGLINE */
void inspircd_cmd_unsgline(char *mask)
{
    /* Not Supported by this IRCD */
}

/* UNSZLINE */
void inspircd_cmd_unszline(char *mask)
{
    send_cmd(s_OperServ, "ZLINE %s", mask);
}

/* SZLINE */
void inspircd_cmd_szline(char *mask, char *reason, char *whom)
{
    send_cmd(ServerName, "ADDLINE Z %s %s %ld 0 :%s", mask, whom,
             (long int) time(NULL), reason);
}

/* SGLINE */
void inspircd_cmd_sgline(char *mask, char *reason)
{
    /* Not Supported by this IRCD */
}

void inspircd_cmd_unban(char *name, char *nick)
{
    /* Not Supported by this IRCD */
}

/* SVSMODE channel modes */

void inspircd_cmd_svsmode_chan(char *name, char *mode, char *nick)
{
    /* Not Supported by this IRCD */
}


/* SVSMODE +d */
/* sent if svid is something weird */
void inspircd_cmd_svid_umode(char *nick, time_t ts)
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
void inspircd_cmd_svid_umode2(User * u, char *ts)
{
    if (debug)
        alog("debug: common_svsmode(2)");
    common_svsmode(u, "+r", NULL);
}

void inspircd_cmd_svid_umode3(User * u, char *ts)
{
    /* not used */
}

void inspircd_cmd_svsjoin(char *source, char *nick, char *chan, char *param)
{
    	send_cmd(source, "SVSJOIN %s %s", nick, chan);
}

void inspircd_cmd_svspart(char *source, char *nick, char *chan)
{
        send_cmd(source, "SVSPART %s %s", nick, chan);
}

void inspircd_cmd_swhois(char *source, char *who, char *mask)
{
	/* Not used currently */
}

void inspircd_cmd_eob()
{
    send_cmd(NULL, "ENDBURST");
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

int inspircd_flood_mode_check(char *value)
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

void inspircd_cmd_jupe(char *jserver, char *who, char *reason)
{
    char rbuf[256];

    snprintf(rbuf, sizeof(rbuf), "Juped by %s%s%s", who,
             reason ? ": " : "", reason ? reason : "");

    if (findserver(servlist, jserver))
        inspircd_cmd_squit(jserver, rbuf);
    inspircd_cmd_server(jserver, 1, rbuf);
    new_server(me_server, jserver, rbuf, SERVER_JUPED, NULL);
}

/* GLOBOPS - to handle old WALLOPS */
void inspircd_cmd_global_legacy(char *source, char *fmt)
{
    send_cmd(source ? source : s_OperServ, "GLOBOPS :%s", fmt);
}

int inspircd_valid_nick(char *nick)
{
    return 1;
}

int inspircd_valid_chan(char *chan)
{
    return 1;
}


void inspircd_cmd_ctcp(char *source, char *dest, char *buf)
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


int inspircd_jointhrottle_mode_check(char *value)
{
    char *tempValue, *one, *two;
    int param1 = 0;
    int param2 = 0;

    if (!value)
        return 0;

    tempValue = sstrdup(value);
    one = strtok(tempValue, ":");
    two = strtok(NULL, "");
    if (one && two) {
        param1 = atoi(one);
        param2 = atoi(two);
    }
    if ((param1 >= 1) && (param1 <= 255) && (param2 >= 1) && (param2 <= 999))
        return 1;
    return 0;
}


/**
 * Tell anope which function we want to perform each task inside of anope.
 * These prototypes must match what anope expects.
 **/
void moduleAddAnopeCmds()
{
    pmodule_cmd_svsnoop(inspircd_cmd_svsnoop);
    pmodule_cmd_remove_akill(inspircd_cmd_remove_akill);
    pmodule_cmd_topic(inspircd_cmd_topic);
    pmodule_cmd_vhost_off(inspircd_cmd_vhost_off);
    pmodule_cmd_akill(inspircd_cmd_akill);
    pmodule_cmd_svskill(inspircd_cmd_svskill);
    pmodule_cmd_svsmode(inspircd_cmd_svsmode);
    pmodule_cmd_372(inspircd_cmd_372);
    pmodule_cmd_372_error(inspircd_cmd_372_error);
    pmodule_cmd_375(inspircd_cmd_375);
    pmodule_cmd_376(inspircd_cmd_376);
    pmodule_cmd_nick(inspircd_cmd_nick);
    pmodule_cmd_guest_nick(inspircd_cmd_guest_nick);
    pmodule_cmd_mode(inspircd_cmd_mode);
    pmodule_cmd_bot_nick(inspircd_cmd_bot_nick);
    pmodule_cmd_kick(inspircd_cmd_kick);
    pmodule_cmd_notice_ops(inspircd_cmd_notice_ops);
    pmodule_cmd_notice(inspircd_cmd_notice);
    pmodule_cmd_notice2(inspircd_cmd_notice2);
    pmodule_cmd_privmsg(inspircd_cmd_privmsg);
    pmodule_cmd_privmsg2(inspircd_cmd_privmsg2);
    pmodule_cmd_serv_notice(inspircd_cmd_serv_notice);
    pmodule_cmd_serv_privmsg(inspircd_cmd_serv_privmsg);
    pmodule_cmd_bot_chan_mode(inspircd_cmd_bot_chan_mode);
    pmodule_cmd_351(inspircd_cmd_351);
    pmodule_cmd_quit(inspircd_cmd_quit);
    pmodule_cmd_pong(inspircd_cmd_pong);
    pmodule_cmd_join(inspircd_cmd_join);
    pmodule_cmd_unsqline(inspircd_cmd_unsqline);
    pmodule_cmd_invite(inspircd_cmd_invite);
    pmodule_cmd_part(inspircd_cmd_part);
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
    pmodule_cmd_global(inspircd_cmd_global);
    pmodule_cmd_global_legacy(inspircd_cmd_global_legacy);
    pmodule_cmd_sqline(inspircd_cmd_sqline);
    pmodule_cmd_squit(inspircd_cmd_squit);
    pmodule_cmd_svso(inspircd_cmd_svso);
    pmodule_cmd_chg_nick(inspircd_cmd_chg_nick);
    pmodule_cmd_svsnick(inspircd_cmd_svsnick);
    pmodule_cmd_vhost_on(inspircd_cmd_vhost_on);
    pmodule_cmd_connect(inspircd_cmd_connect);
    pmodule_cmd_bob(inspircd_cmd_bob);
    pmodule_cmd_svshold(inspircd_cmd_svshold);
    pmodule_cmd_release_svshold(inspircd_cmd_release_svshold);
    pmodule_cmd_unsgline(inspircd_cmd_unsqline);
    pmodule_cmd_unszline(inspircd_cmd_unszline);
    pmodule_cmd_szline(inspircd_cmd_szline);
    pmodule_cmd_sgline(inspircd_cmd_sgline);
    pmodule_cmd_unban(inspircd_cmd_unban);
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
    pmodule_jointhrottle_mode_check(inspircd_jointhrottle_mode_check);
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
    pmodule_permchan_mode(0);

    moduleAddAnopeCmds();
    moduleAddIRCDMsgs();

    return MOD_CONT;
}

