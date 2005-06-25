/* InspIRCd beta 3 functions
 *
 * (C) 2003-2005 Anope Team
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
#include "inspircd.h"

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
    {"InspIRCd 1.0 Beta",       /* ircd name */
     "+o",                      /* nickserv mode */
     "+o",                      /* chanserv mode */
     "+o",                      /* memoserv mode */
     "+o",                      /* hostserv mode */
     "+io",                     /* operserv mode */
     "+o",                      /* botserv mode  */
     "+o",                      /* helpserv mode */
     "+i",                      /* Dev/Null mode */
     "+io",                     /* Global mode   */
     "+o",                      /* nickserv alias mode */
     "+o",                      /* chanserv alias mode */
     "+o",                      /* memoserv alias mode */
     "+io",                     /* hostserv alias mode */
     "+io",                     /* operserv alias mode */
     "+o",                      /* botserv alias mode  */
     "+o",                      /* helpserv alias mode */
     "+i",                      /* Dev/Null alias mode */
     "+io",                     /* Global alias mode   */
     "+i",                      /* Used by BotServ Bots */
     5,                         /* Chan Max Symbols     */
     "-cilmnpstuzCGKNOQRSV",    /* Modes to Remove */
     "+ao",                     /* Channel Umode used by Botserv bots */
     1,                         /* SVSNICK */
     1,                         /* Vhost  */
     1,                         /* Has Owner */
     "+q",                      /* Mode to set for an owner */
     "-q",                      /* Mode to unset for an owner */
     "+a",                      /* Mode to set for channel admin */
     "-a",                      /* Mode to unset for channel admin */
     "+r",                      /* Mode On Reg          */
     "-r",                      /* Mode on UnReg        */
     "-r",                      /* Mode on Nick Change  */
     0,                         /* Supports SGlines     */
     1,                         /* Supports SQlines     */
     1,                         /* Supports SZlines     */
     1,                         /* Supports Halfop +h   */
     3,                         /* Number of server args */
     0,                         /* Join 2 Set           */
     0,                         /* Join 2 Message       */
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
     0,                         /* vidents              */
     0,                         /* svshold              */
     0,                         /* time stamp on mode   */
     0,                         /* NICKIP               */
     0,                         /* O:LINE               */
     1,                         /* UMODE               */
     1,                         /* VHOST ON NICK        */
     1,                         /* Change RealName      */
     CMODE_K,                   /* No Knock             */
     0,                         /* Admin Only           */
     DEFAULT_MLOCK,             /* Default MLOCK       */
     UMODE_x,                   /* Vhost Mode           */
     0,                         /* +f                   */
     1,                         /* +L                   */
     0,
     CMODE_L,
     0,
     0,                         /* No Knock requires +i */
     NULL,                      /* CAPAB Chan Modes             */
     1,                         /* We support inspircd TOKENS */
     0,                         /* TOKENS are CASE Sensitive */
     0,                         /* TIME STAMPS are BASE64 */
     0,                         /* +I support */
     0,                         /* SJOIN ban char */
     0,                         /* SJOIN except char */
     0,                         /* SJOIN invite char */
     0,                         /* Can remove User Channel Modes with SVSMODE */
     0,                         /* Sglines are not enforced until user reconnects */
     "x",                       /* vhost char */
     0,                         /* ts6 */
     0,                         /* support helper umode */
     0,                         /* p10 */
     NULL,                      /* character set */
     1,                         /* reports sync state */
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
     CAPAB_TOKEN,               /* TOKEN        */
     0,                         /* VHOST        */
     0,                         /* SSJ3         */
     0,                         /* NICK2        */
     0,                         /* UMODE2       */
     0,                         /* VL           */
     0,                         /* TLKEXT       */
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
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,
    0,
    0,
    0, 0, 0, 0, 0, 0, 0,
    0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
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
    {CMODE_k, 0, set_key, cs_set_key},
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
            update_host(user);
            break;
        }
    }
}

/* Set mod_current_buffer from here */
void inspircd_set_mod_current_buffer(int ac, char **av)
{
    if (ac >= 3)
        mod_current_buffer = sstrdup(av[2]);
    else
        mod_current_buffer = NULL;
}

int anope_event_nickchange(char *source, int ac, char **av);
int anope_event_servertopic(char *source, int ac, char **av);
int anope_event_servermode(char *source, int ac, char **av);

/* *INDENT-OFF* */
void moduleAddIRCDMsgs(void) {
    Message *m;

    // user has no choice but to use tokens with InspIRCd :)
    UseTokens = 1;

    updateProtectDetails("PROTECT","PROTECTME","!protect","!deprotect","AUTOPROTECT","+a","-a");

    m = createMessage("$",      anope_event_null); addCoreMessage(IRCD,m);		// send routing table
    m = createMessage("X",      anope_event_null); addCoreMessage(IRCD,m);		// begin netburst NOW
    m = createMessage("Y",      anope_event_eob); addCoreMessage(IRCD,m);		// end of UPLINK netburst
    m = createMessage("H",      anope_event_null); addCoreMessage(IRCD,m);		// local sync start
    m = createMessage("F",      anope_event_null); addCoreMessage(IRCD,m);		// local sync end
    m = createMessage("f",      anope_event_null); addCoreMessage(IRCD,m);		// local U-lined sync end
    m = createMessage("*",      anope_event_null); addCoreMessage(IRCD,m);		// no operation
    m = createMessage("|",      anope_event_null); addCoreMessage(IRCD,m);		// oper type
    m = createMessage("-",      anope_event_null); addCoreMessage(IRCD,m);		// start mesh operation
    m = createMessage("s",      anope_event_server); addCoreMessage(IRCD,m);		// passive server response
    m = createMessage("U",      anope_event_null); addCoreMessage(IRCD,m);		// active service connect
    m = createMessage("J",      anope_event_join); addCoreMessage(IRCD,m);		// join
    m = createMessage("k",      anope_event_kick); addCoreMessage(IRCD,m);		// kick
    m = createMessage("K",      anope_event_kill); addCoreMessage(IRCD,m);		// kill
    m = createMessage("M",      anope_event_servermode); addCoreMessage(IRCD,m);	// servermode
    m = createMessage("m",      anope_event_mode); addCoreMessage(IRCD,m);              // mode
    m = createMessage("N",      anope_event_nick); addCoreMessage(IRCD,m);		// nick (introduce)
    m = createMessage("n",      anope_event_nickchange); addCoreMessage(IRCD,m);	// nick (change)
    m = createMessage("V",      anope_event_null); addCoreMessage(IRCD,m);		// notice
    m = createMessage("L",      anope_event_part); addCoreMessage(IRCD,m);		// part
    m = createMessage("?",      anope_event_ping); addCoreMessage(IRCD,m);		// ping
    m = createMessage("P",      anope_event_privmsg); addCoreMessage(IRCD,m);		// privmsg
    m = createMessage("Q",      anope_event_quit); addCoreMessage(IRCD,m);		// quit
    m = createMessage("&",      anope_event_squit); addCoreMessage(IRCD,m);		// squit
    m = createMessage("t",      anope_event_topic); addCoreMessage(IRCD,m);		// topic (user)
    m = createMessage("T",      anope_event_servertopic); addCoreMessage(IRCD,m);	// topic (server)
    m = createMessage("@",      anope_event_null); addCoreMessage(IRCD,m);		// wallops
    m = createMessage("b",      anope_event_chghost); addCoreMessage(IRCD,m);		// change displayed host
    m = createMessage("a",      anope_event_chgname); addCoreMessage(IRCD,m);		// change gecos
}

char* xsumtable = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

char* CreateSum()
{
	int q = 0;
        static char sum[8];
        sum[7] = '\0';
        for(q = 0; q < 7; q++)
                sum[q] = xsumtable[rand()%52];
        return sum;
}

/* Event: PROTOCTL */
int anope_event_capab(char *source, int ac, char **av)
{
    capab_parse(ac, av);
    return MOD_CONT;
}

int anope_event_nickchange(char *source, int ac, char **av)
{
    do_nick(av[0], av[1], NULL, NULL, NULL, NULL, 0, 0, 0, NULL, NULL);
    return MOD_CONT;
}

void inspircd_cmd_svsnoop(char *server, int set)
{
    send_cmd(CreateSum(), "*");
}

void inspircd_cmd_svsadmin(char *server, int set)
{
    inspircd_cmd_svsnoop(server, set);
}

void inspircd_cmd_remove_akill(char *user, char *host)
{
    send_cmd(CreateSum(), ". %s@%s %s", user, host, s_OperServ);
}

void inspircd_cmd_topic(char *whosets, char *chan, char *whosetit,
                     char *topic, time_t when)
{
    send_cmd(CreateSum(), "t %s %s :%s", whosets, chan, topic);
}

void inspircd_cmd_vhost_off(User * u)
{
    send_cmd(CreateSum(), "m %s %s -x", s_HostServ, u->nick);
}

void inspircd_cmd_akill(char *user, char *host, char *who, time_t when,
                     time_t expires, char *reason)
{
    send_cmd(CreateSum(), "# %s@%s %s %lu %lu :%s", user, host, who, (unsigned long)when, (unsigned long)86400 * 2, reason);
}

void inspircd_cmd_svskill(char *source, char *user, char *buf)
{
    if (!buf || !source || !user) {
        return;
    }
    send_cmd(CreateSum(),"K %s %s :%s", s_OperServ, user, buf);
}

void inspircd_cmd_svsmode(User * u, int ac, char **av)
{
    send_cmd(CreateSum(), "M %s %s", u->nick, av[0]);
}


void inspircd_cmd_372(char *source, char *msg)
{
}

void inspircd_cmd_372_error(char *source)
{
}

void inspircd_cmd_375(char *source)
{
}

void inspircd_cmd_376(char *source)
{
}

void inspircd_cmd_nick(char *nick, char *name, char *modes)
{
    // N <time> <nick> <host> <displayed-host> <ident> <modes> <ipaddress> <server> :<GECOS>
    send_cmd(CreateSum(), "N %lu %s %s %s %s %s 0.0.0.0 %s :%s",(long int) time(NULL),nick,ServiceHost,ServiceHost,ServiceUser,modes,ServerName,name);
}

void inspircd_cmd_guest_nick(char *nick, char *user, char *host, char *real,
                          char *modes)
{
    send_cmd(CreateSum(), "NICK %s 1 %ld %s %s %s 0 %s * :%s", nick,
             (long int) time(NULL), user, host, ServerName, modes, real);
    send_cmd(CreateSum(), "N %lu %s %s %s %s %s 0.0.0.0 %s :%s",(long int) time(NULL),nick,host,host,user,modes,ServerName,real);
}

void inspircd_cmd_mode(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }
    send_cmd(CreateSum(), "m %s %s %s", source, dest, buf);
}

void inspircd_cmd_bot_nick(char *nick, char *user, char *host, char *real,
                        char *modes)
{
    send_cmd(CreateSum(), "N %lu %s %s %s %s %s 0.0.0.0 %s :%s",(long int) time(NULL),nick,host,host,user,modes,ServerName,real);
}

void inspircd_cmd_kick(char *source, char *chan, char *user, char *buf)
{
    if (buf) {
        send_cmd(CreateSum(), "k %s %s %s :%s", source, user, chan, buf);
    } else {
        send_cmd(CreateSum(), "k %s %s %s :", source, user, chan);
    }
}

void inspircd_cmd_notice_ops(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(CreateSum(), "V %s @%s :%s", source, dest, buf);
}


void inspircd_cmd_notice(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    if (UsePrivmsg) {
        inspircd_cmd_privmsg2(source, dest, buf);
    } else {
        send_cmd(CreateSum(), "V %s %s :%s", source, dest, buf);
    }
}

void inspircd_cmd_notice2(char *source, char *dest, char *msg)
{
    send_cmd(CreateSum(), "V %s %s :%s", source, dest, msg);
}

void inspircd_cmd_privmsg(char *source, char *dest, char *buf)
{
    if (!buf) {
        return;
    }

    send_cmd(CreateSum(), "P %s %s :%s", source, dest, buf);
}

void inspircd_cmd_privmsg2(char *source, char *dest, char *msg)
{
    send_cmd(CreateSum(), "P %s %s :%s", source, dest, msg);
}

void inspircd_cmd_serv_notice(char *source, char *dest, char *msg)
{
   if (!uplink)
   {
      send_cmd(CreateSum(), "V %s * :%s", source, msg);
   }
   else
   if (!strcmp(dest,uplink))
   {
      alog("debug: serv_notice output to %s", dest);
      send_cmd(CreateSum(), "V %s * :%s", source, msg);
   }
}

void inspircd_cmd_serv_privmsg(char *source, char *dest, char *msg)
{
   if (!uplink)
   {
      send_cmd(CreateSum(), "P %s * :%s", source, msg);
   }
   else
   if (!strcmp(dest,uplink))
   {
      send_cmd(CreateSum(), "P %s * :%s", source, msg);
   }
}


void inspircd_cmd_bot_chan_mode(char *nick, char *chan)
{
    anope_cmd_mode(nick, chan, "%s %s %s", ircd->botchanumode, nick, nick);
}

void inspircd_cmd_351(char *source)
{
}

/* QUIT */
void inspircd_cmd_quit(char *source, char *buf)
{
    if (buf) {
        send_cmd(CreateSum(), "Q %s :%s", source, buf);
    } else {
        send_cmd(CreateSum(), "QUIT %s :", source);
    }
}

/* PROTOCTL */
void inspircd_cmd_protoctl()
{
}

/* PASS */
void inspircd_cmd_pass(char *pass)
{
}

int anope_event_eob(char *source, int ac, char **av)
{
	send_cmd(CreateSum(), "/ NickServ");
	send_cmd(CreateSum(), "v %s Anope-%s %s :%s - %s -- %s", ServerName, version_number, ServerName, ircd->name, version_flags, version_build);
	return MOD_CONT;
}

/* SERVER name hop descript */
void inspircd_cmd_server(char *servname, int hop, char *descript)
{
    if (hop == 0)
    {
          send_cmd(CreateSum(), "H %s",servname);
    }
    else
    {
          char pw[1024];
          switch (hop)
          {
              case 1:
                 strncpy(pw,RemotePassword,1023);
              break;
              case 2:
                 strncpy(pw,RemotePassword2,1023);
              break;
              case 3:
                 strncpy(pw,RemotePassword3,1023);
              break;
          }	
          send_cmd(CreateSum(), "U %s %s :%s", servname, pw, descript);
    }
}

/* PONG */
void inspircd_cmd_pong(char *servname, char *who)
{
    send_cmd(CreateSum(), "!");
}

/* JOIN */
void inspircd_cmd_join(char *user, char *channel, time_t chantime)
{
    send_cmd(CreateSum(), "J %s %s", user, channel);
}

/* UNSQLINE */
void inspircd_cmd_unsqline(char *user)
{
    if (!user) {
        return;
    }
    send_cmd(CreateSum(), "[ %s %s", user, s_OperServ);
}

/* CHGHOST */
void inspircd_cmd_chghost(char *nick, char *vhost)
{
    if (!nick || !vhost) {
        return;
    }
    send_cmd(CreateSum(), "b %s %s", nick, vhost);
}

/* CHGIDENT */
void inspircd_cmd_chgident(char *nick, char *vIdent)
{
}

/* INVITE */
void inspircd_cmd_invite(char *source, char *chan, char *nick)
{
    if (!source || !chan || !nick) {
        return;
    }

    send_cmd(CreateSum(), "i %s %s %s", source, nick, chan);
}

/* PART */
void inspircd_cmd_part(char *nick, char *chan, char *buf)
{
    if (!nick || !chan) {
        return;
    }
    if (buf) {
        send_cmd(CreateSum(), "L %s %s :%s", nick, chan, buf);
    } else {
        send_cmd(CreateSum(), "L %s %s :", nick, chan);
    }
}

/* 391 */
void inspircd_cmd_391(char *source, char *timestr)
{
}

/* 250 */
void inspircd_cmd_250(char *buf)
{
}

/* 307 */
void inspircd_cmd_307(char *buf)
{
}

/* 311 */
void inspircd_cmd_311(char *buf)
{
}

/* 312 */
void inspircd_cmd_312(char *buf)
{
}

/* 317 */
void inspircd_cmd_317(char *buf)
{
}

/* 219 */
void inspircd_cmd_219(char *source, char *letter)
{
}

/* 401 */
void inspircd_cmd_401(char *source, char *who)
{
}

/* 318 */
void inspircd_cmd_318(char *source, char *who)
{
}

/* 242 */
void inspircd_cmd_242(char *buf)
{
}

/* 243 */
void inspircd_cmd_243(char *buf)
{
}

/* 211 */
void inspircd_cmd_211(char *buf)
{
}

/* GLOBOPS */
void inspircd_cmd_global(char *source, char *buf)
{
    send_cmd(CreateSum(), "V %s @* :%s", (source ? source : s_OperServ), buf);
}

/* SQLINE */
void inspircd_cmd_sqline(char *mask, char *reason)
{
    if (!mask || !reason) {
        return;
    }
    // { <mask> <who-set-it> <time-set> <duration> :<reason>
    send_cmd(CreateSum(), "{ %s %s %lu 0 :%s", mask, ServerName, (unsigned long)time(NULL), reason);
}

/* SQUIT */
void inspircd_cmd_squit(char *servname, char *message)
{
    if (!servname) {
        return;
    }

    send_cmd(CreateSum(), "& %s", servname);
}

/* SVSO */
void inspircd_cmd_svso(char *source, char *nick, char *flag)
{
    send_cmd(CreateSum(),"M %s %s",nick,flag);
}

/* NICK <newnick>  */
void inspircd_cmd_chg_nick(char *oldnick, char *newnick)
{
    if (!oldnick || !newnick) {
        return;
    }

    send_cmd("n %s %s", oldnick, newnick);
}

/* SVSNICK */
void inspircd_cmd_svsnick(char *source, char *guest, time_t when)
{
    if (!source || !guest) {
        return;
    }
    send_cmd(CreateSum(), "n %s %s", source, guest);
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
    me_server =
        new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);

    srand(time(NULL));

    inspircd_cmd_server(ServerName, servernum, ServerDesc);
}

/* Events */

int anope_event_ping(char *source, int ac, char **av)
{
    inspircd_cmd_pong("", "");
    return MOD_CONT;
}

int anope_event_436(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_away(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_servertopic(char *source, int ac, char **av)
{
    /* T 1115252145 ViaraiX #deck8 :Welcome to Deck8 */
    char *v[32];
    if (ac != 4)
       return MOD_CONT;
    v[0] = av[2];
    v[1] = av[1];
    v[2] = av[0];
    v[3] = av[3];
    do_topic(NULL, 4, v); // no source (server set)
    return MOD_CONT;
}

int anope_event_topic(char *source, int ac, char **av)
{
    // t [Brain] #chatspike :this is a topic test fsfdsfsdfds
    char *v[32];
    if (ac != 3)
        return MOD_CONT;
    v[0] = av[1];
    v[1] = av[0];
    v[2] = "0";
    v[3] = av[2];
    do_topic(av[0], 4, v);
    return MOD_CONT;
}

int anope_event_squit(char *source, int ac, char **av)
{
    char *v[32];
    if (ac != 1)
        return MOD_CONT;
    v[0] = av[1];
    do_squit(av[1], 1, v);
    return MOD_CONT;
}

int anope_event_quit(char *source, int ac, char **av)
{
    char *v[32];
    if (ac != 2)
        return MOD_CONT;
    v[0] = av[1];
    v[1] = av[2];
    do_quit(av[0], 2, v);
    return MOD_CONT;
}

int anope_event_servermode(char *source, int ac, char **av)
{
    char* v[32];
    Channel* c;
    int i = 0;

    if (*av[0] == '#')
    {
	    c = findchan(av[0]);
            for (i = 1; i < ac; i++)
                v[i-1] = av[i];
	    if (c)
	    {
	    	chan_set_modes(s_OperServ, c, ac-1, v, 1);
	    }
    }
    else
    {
	    v[0] = av[0];
	    for (i = 0; i < ac; i++)
	       	v[i+1] = av[i];
	    anope_event_mode(av[0], ac+1, v);
    }
    return MOD_CONT;
}


int anope_event_mode(char *source, int ac, char **av)
{
    char *v[32];
    int i;
    if (ac < 3)
        return MOD_CONT;

    for (i = 1; i < ac; i++)
    {
        v[i-1] = av[i];
    }
    if (*av[1] == '#') {
        do_cmode(av[1], ac-1, v);
    } else {
        do_umode(av[0], ac-1, v);
    }
    return MOD_CONT;
}


int anope_event_kill(char *source, int ac, char **av)
{
    if (ac != 3)
        return MOD_CONT;
    m_kill(av[1], av[2]);
    return MOD_CONT;
}

int anope_event_kick(char *source, int ac, char **av)
{
    // Syntax: k <source> <dest> <channel> :<reason>
    char *v[32];
    if (ac != 4)
        return MOD_CONT;    v[0] = av[1];
    v[1] = av[2];
    v[2] = av[3];
    do_kick(av[0], 3, v);
    return MOD_CONT;
}


int anope_event_join(char *source, int ac, char **av)
{
    User *u;
    Channel *c;
    int i;
    char* new_av[32];
    char* cumodes[3];
    char thismodes[256];
    cumodes[0] = thismodes;

    // some sanity checks
    if (ac < 2) {
            return MOD_CONT;
    }

    u = finduser(av[0]); // get the user info

    if (!u) {
        return MOD_CONT;
    }

    cumodes[1] = av[0];

    // iterate through each channel the user is joining
    for (i = 1; i < ac; i++)
    {
            // the channel name
            new_av[0] = av[i];

            // strip off the permissions
            strcpy(thismodes,"");
            if (*new_av[0] != '#')
            {
		    switch (*new_av[0])
		    {
			case '@':
				strcpy(thismodes,"+o");
			break;
			case '%':
				strcpy(thismodes,"+h");
			break;
			case '+':
				strcpy(thismodes,"+v");
			break;
		    }
                    new_av[0]++;
            }

            do_join(av[0], 1, new_av);
            c = findchan(new_av[0]);
            if ((c) && (strcmp(thismodes,"")))
            {
                    chan_set_modes(av[0], c, 2, cumodes, 1);
            }
    }

    return MOD_CONT;

}

int anope_event_motd(char *source, int ac, char **av)
{
    return MOD_CONT;
}

int anope_event_setname(char *source, int ac, char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(av[0]);
    if (!u) {
        if (debug) {
            alog("user: SETNAME for nonexistent user %s", source);
        }
        return MOD_CONT;
    }
    change_user_realname(u, av[1]);
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
            alog("user: CHGNAME for nonexistent user %s", av[0]);
        }
        return MOD_CONT;
    }

    change_user_realname(u, av[1]);
    return MOD_CONT;
}

int anope_event_setident(char *source, int ac, char **av)
{
    return MOD_CONT;
}
int anope_event_chgident(char *source, int ac, char **av)
{
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
            alog("user: SETHOST for nonexistent user %s", source);
        }
        return MOD_CONT;
    }
    change_user_host(u, av[1]);
    return MOD_CONT;
}

int anope_event_nick(char *source, int ac, char **av)
{
    User *user;
    struct in_addr addy;
    uint32* ad = (uint32*)&addy;

// someone really should comment this -- Brain

// User *do_nick(const char *source, char *nick, char *username, char *host,
//              char *server, char *realname, time_t ts, uint32 svid,
//              uint32 ip, char *vhost, char *uid)

// Syntax: N <time> <nick> <host> <displayed-host> <ident> <modes> <ipaddress> <server> :<GECOS>

    if (ac == 9) {
	// fix by brain - 27 apr 2005
	// During a net merge between two meshed clients there may be some spurious "bouncing"
	// nick introductions (e.g. introductions for clients we already have). If this happens,
	// we should ignore any that claim to be from us as these are just confirmations of the
	// services "complement" - processing them will cause all kinds of session limit oddities.
	if ((strcmp(av[7],ServerName)) && (!finduser(av[1])))
	{
	        inet_aton(av[6],&addy);
	        user = do_nick("", av[1], av[4], av[2], uplink, av[8],
	               strtoul(av[0], NULL, 10), 0, htonl(*ad), av[3], NULL);
	        if (user)
	            anope_set_umode(user, 1, &av[5]);
	}
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
            alog("user: CHGHOST for nonexistent user %s", av[0]);
        }
        return MOD_CONT;
    }

    change_user_host(u, av[1]);
    return MOD_CONT;
}



/* EVENT: SERVER */
int anope_event_server(char *source, int ac, char **av)
{
    // to shield anope from the intricacies of the mesh,
    // anope only sees the uplink at all times (and receives
    // all nick introductions etc as if they'd come from the
    // uplink directly, 1 hop away.
    if (!uplink)
    {
	    uplink = sstrdup(av[0]);
	    do_server(source, av[0], av[1], av[3], NULL);
    }
    return MOD_CONT;
}


int anope_event_privmsg(char *source, int ac, char **av)
{
    if (ac != 3)
        return MOD_CONT;
    m_privmsg(av[0], av[1], av[2]);
    return MOD_CONT;
}

int anope_event_part(char *source, int ac, char **av)
{
    char *v[32];
    if (ac != 3)
        return MOD_CONT;
    v[0] = av[1]; // channel
    v[1] = av[2]; // reason
    do_part(av[0], 2, v);
    return MOD_CONT;
}

int anope_event_whois(char *source, int ac, char **av)
{
    return MOD_CONT;
}

/* SVSHOLD - set */
void inspircd_cmd_svshold(char *nick)
{
    /* Not supported by this IRCD */
}

/* SVSHOLD - release */
void inspircd_cmd_release_svshold(char *nick)
{
    /* Not Supported by this IRCD */
}

/* UNSGLINE */
void inspircd_cmd_unsgline(char *mask)
{
    /* Not Supported by this IRCD */
}

/* UNSZLINE */
void inspircd_cmd_unszline(char *mask)
{
    // ] <mask> <who>
    send_cmd(CreateSum(), "] %s %s", mask, s_OperServ);
}

/* SZLINE */
void inspircd_cmd_szline(char *mask, char *reason, char *whom)
{
    // } <mask> <who-set-it> <time-set> <duration> :<reason>
    send_cmd(CreateSum(), "} %s %s %lu %lu :%s",mask,whom,(unsigned long)time(NULL),(unsigned long)86400 * 2, reason);
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
    /* Not supported by this IRCD */
}

/* SVSMODE +d */
/* nc_change was = 1, and there is no na->status */
void inspircd_cmd_nc_change(User * u)
{
    common_svsmode(u, "-r", "1");
}

/* SVSMODE +r */
void inspircd_cmd_svid_umode2(User * u, char *ts)
{
    send_cmd(CreateSum(),"m %s %s +r",s_OperServ,u->nick);
}

void inspircd_cmd_svid_umode3(User * u, char *ts)
{
    /* not used */
}

void inspircd_cmd_svsjoin(char *source, char *nick, char *chan)
{
    send_cmd(source, "J %s %s", nick, chan);
}

void inspircd_cmd_svspart(char *source, char *nick, char *chan)
{
    send_cmd(source, "L %s %s :", nick, chan);
}

void inspircd_cmd_swhois(char *source, char *who, char *mask)
{
    /* Not supported at U type server */
}

void inspircd_cmd_eob()
{
    /* Not implemented in this version of inspircd.c */
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
    // no +f mode!
    return 1;
}

void inspircd_cmd_jupe(char *jserver, char *who, char *reason)
{
    send_cmd(CreateSum(), "_ %s :%s",jserver,reason);
}

/* GLOBOPS - to handle old WALLOPS */
void inspircd_cmd_global_legacy(char *source, char *fmt)
{
    send_cmd(CreateSum(), "@ %s :%s", source ? source : ServerName, fmt);
}

int inspircd_valid_nick(char *nick)
{
    // no special nicks
    return 1;
}

void inspircd_cmd_ctcp(char *source, char *dest, char *buf)
{
    char *s;
    if (!buf) {
        return;
    } else {
        s = normalizeBuffer(buf);
        send_cmd(CreateSum(), "V %s %s :\1%s \1", source, dest, s);
        free(s);
    }
}


/**
 * Tell anope which function we want to perform each task inside of anope.
 * These prototypes must match what anope expects.
 **/
void moduleAddAnopeCmds() {
	pmodule_set_mod_current_buffer(inspircd_set_mod_current_buffer);
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
    pmodule_cmd_eob(inspircd_cmd_eob);
    pmodule_flood_mode_check(inspircd_flood_mode_check);
    pmodule_cmd_jupe(inspircd_cmd_jupe);
    pmodule_valid_nick(inspircd_valid_nick);
    pmodule_cmd_ctcp(inspircd_cmd_ctcp);
    pmodule_set_umode(inspircd_set_umode);
}

/** 
 * Now tell anope how to use us.
 **/
int AnopeInit(int argc, char **argv) {

	moduleAddAuthor("Brain");
        moduleAddVersion("$Id$");
        moduleSetType(PROTOCOL);

        pmodule_ircd_version("InspIRCd 1.0 Beta3");
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
    moduleAddIRCDMsgs();

    return MOD_CONT;
}

