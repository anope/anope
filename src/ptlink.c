/* PTLink IRCD functions
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

#include "services.h"
#include "pseudo.h"

#ifdef IRC_PTLINK

const char version_protocol[] = "PTlink 6.14.5+";

IRCDVar ircd[] = {
    {"PTlinkIRCd 6.14.*",       /* ircd name */
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
     "-cdfiklmnpqstRS",         /* Modes to Remove */
     "+ao",                     /* Channel Umode used by Botserv bots */
     1,                         /* SVSNICK */
     1,                         /* Vhost  */
     0,                         /* Has Owner */
     NULL,                      /* Mode to set for an owner */
     NULL,                      /* Mode to unset for an owner */
     "+r",                      /* Mode On Reg          */
     "-r",                      /* Mode on UnReg        */
     "+d",                      /* Mode on Nick Change  */
     0,                         /* Supports SGlines     */
     1,                         /* Supports SQlines     */
     0,                         /* Supports SZlines     */
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
     1,                         /* UMODE                */
     0,                         /* O:LINE               */
     1,                         /* VHOST ON NICK        */
     0,                         /* Change RealName      */
     0,                         /* ChanServ extra   */
     CMODE_K,                   /* No Knock             */
     CMODE_A,                   /* Admin Only           */
     DEFAULT_MLOCK,             /* Default MLOCK       */
     UMODE_o,                   /* Vhost Mode           */
     1,                         /* +f                   */
     0,                         /* +L                   */
     CMODE_f,
     0,
     1,
     1,                         /* No Knock requires +i */
     NULL,                      /* CAPAB Chan Modes             */
     }
    ,
    {NULL}
};

IRCDCAPAB ircdcap[] = {
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
     0}
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
    0,
    UMODE_h, UMODE_i, 0, 0, 0, 0, 0, UMODE_o,
    0,
    0, UMODE_r, 0, 0, 0, 0, UMODE_w,
    0,
    0,
    0,
    0, 0, 0, 0, 0
};

char csmodes[128] = {
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


CMMode cmmodes[128] = {
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


CBMode cbmodes[128] = {
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
    {CMODE_N, 0, NULL, NULL},
    {0},                        /* O */
    {0},                        /* P */
    {0},                        /* Q */
    {CMODE_R, 0, NULL, NULL},   /* R */
    {CMODE_S, 0, NULL, NULL},
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
    {CMODE_k, 0, set_key, cs_set_key},
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

CBModeInfo cbmodeinfos[] = {
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
    {'K', CMODE_K, 0, NULL, NULL},
    {'N', CMODE_N, 0, NULL, NULL},
    {'R', CMODE_R, 0, NULL, NULL},
    {'S', CMODE_S, 0, NULL, NULL},
    {0}
};

CUMode cumodes[128] = {
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



void anope_cmd_bot_chan_mode(char *nick, char *chan)
{
    anope_cmd_mode(s_ChanServ, chan, "%s %s %s", ircd->botchanumode, nick,
                   nick);
}

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
int anope_event_newmask(char *source, int ac, char **av)
{
    User *u;
    char *newhost = NULL, *newuser = NULL;

    if (ac != 1)
        return MOD_CONT;
    u = finduser(source);

    if (!u) {
        alog("user: NEWMASK for nonexistent user %s", av[0]);
        return MOD_CONT;
    }

    newuser = myStrGetOnlyToken(av[0], '@', 0);
    if (newuser) {
        newhost = myStrGetTokenRemainder(av[0], '@', 1);
        change_user_username(u, newuser);
    } else {
        newhost = av[0];
    }

    if (*newhost == '@')
        newhost++;

    if (newhost) {
        change_user_host(u, newhost);
    }

    return MOD_CONT;
}


int anope_event_nick(char *source, int ac, char **av)
{
    User *user;

    if (ac != 2) {
        user = do_nick(source, av[0], av[4], av[6], av[7], av[8],
                       strtoul(av[2], NULL, 10), 0, 0, av[5], NULL);
        if (user)
            anope_set_umode(user, 1, &av[3]);
    } else {
        do_nick(source, av[0], NULL, NULL, NULL, NULL,
                strtoul(av[1], NULL, 10), 0, 0, NULL, NULL);
    }
    return MOD_CONT;
}

int anope_event_server(char *source, int ac, char **av)
{
    char *uplink;

    if (!stricmp(av[1], "1"))
        uplink = sstrdup(av[0]);
    do_server(source, ac, av);
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

    m = createMessage("401",       NULL); addCoreMessage(IRCD,m);
    m = createMessage("436",       anope_event_436); addCoreMessage(IRCD,m);
    m = createMessage("AWAY",      anope_event_away); addCoreMessage(IRCD,m);
    m = createMessage("INVITE",    NULL); addCoreMessage(IRCD,m);
    m = createMessage("JOIN",      anope_event_join); addCoreMessage(IRCD,m);
    m = createMessage("KICK",      anope_event_kick); addCoreMessage(IRCD,m);
    m = createMessage("KILL",      anope_event_kill); addCoreMessage(IRCD,m);
    m = createMessage("MODE",      anope_event_mode); addCoreMessage(IRCD,m);
    m = createMessage("MOTD",      anope_event_motd); addCoreMessage(IRCD,m);
    m = createMessage("NICK",      anope_event_nick); addCoreMessage(IRCD,m);
    m = createMessage("NOTICE",    NULL); addCoreMessage(IRCD,m);
    m = createMessage("PART",      anope_event_part); addCoreMessage(IRCD,m);
    m = createMessage("PASS",      NULL); addCoreMessage(IRCD,m);
    m = createMessage("PING",      anope_event_ping); addCoreMessage(IRCD,m);
    m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
    m = createMessage("QUIT",      anope_event_quit); addCoreMessage(IRCD,m);
    m = createMessage("SERVER",    anope_event_server); addCoreMessage(IRCD,m);
    m = createMessage("SQUIT",     anope_event_squit); addCoreMessage(IRCD,m);
    m = createMessage("STATS",     m_stats); addCoreMessage(IRCD,m);
    m = createMessage("TIME",      m_time); addCoreMessage(IRCD,m);
    m = createMessage("TOPIC",     anope_event_topic); addCoreMessage(IRCD,m);
    m = createMessage("USER",      NULL); addCoreMessage(IRCD,m);
    m = createMessage("WALLOPS",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("WHOIS",     anope_event_whois); addCoreMessage(IRCD,m);

    /* DALnet specific messages */
    m = createMessage("AKILL",     NULL); addCoreMessage(IRCD,m);
    m = createMessage("GLOBOPS",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("GNOTICE",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("GOPER",     NULL); addCoreMessage(IRCD,m);
    m = createMessage("RAKILL",    NULL); addCoreMessage(IRCD,m);
    m = createMessage("SILENCE",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("SVSKILL",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("SVSMODE",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("SVSNICK",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("SVSNOOP",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("SQLINE",    NULL); addCoreMessage(IRCD,m);
    m = createMessage("UNSQLINE",  NULL); addCoreMessage(IRCD,m);

    m = createMessage("NEWMASK",  anope_event_newmask); addCoreMessage(IRCD,m);
    m = createMessage("CAPAB", 	  anope_event_capab); addCoreMessage(IRCD,m);
    m = createMessage("SVINFO",   anope_event_svinfo); addCoreMessage(IRCD,m);
    m = createMessage("SVSINFO",  anope_event_svsinfo); addCoreMessage(IRCD,m);
    m = createMessage("SJOIN",     anope_event_sjoin); addCoreMessage(IRCD,m);
}

/* *INDENT-ON* */


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

void anope_cmd_sqline(char *mask, char *reason)
{
    send_cmd(NULL, "SQLINE %s :%s", mask, reason);
}

void anope_cmd_svsnoop(char *server, int set)
{
    send_cmd(NULL, "SVSADMIN %s :%s", server, set ? "noopers" : "rehash");
}

void anope_cmd_remove_akill(char *user, char *host)
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
    send_cmd(whosets, "TOPIC %s %s %lu :%s", chan, whosetit, when, topic);
}

void anope_cmd_unsqline(char *user)
{
    send_cmd(NULL, "UNSQLINE %s", user);
}

void anope_cmd_join(char *user, char *channel, time_t chantime)
{
    send_cmd(user, "JOIN %s", channel);
}

void anope_cmd_akill(char *user, char *host, char *who, time_t when,
                     time_t expires, char *reason)
{
    send_cmd(ServerName, "GLINE %s@%s %i %s :%s", user, host, 86400 * 2,
             who, reason);
}


void anope_cmd_svskill(char *source, char *user, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    if (!source || !user) {
        return;
    }

    send_cmd(source, "KILL %s :%s", user, buf);
}


void anope_cmd_svsmode(User * u, int ac, char **av)
{
    send_cmd(ServerName, "SVSMODE %s %s%s%s", u->nick, av[0],
             (ac == 2 ? " " : ""), (ac == 2 ? av[1] : ""));
}


void anope_cmd_squit(char *servname, char *message)
{
    send_cmd(servname, "SQUIT %s :%s", servname, message);
}

/* PONG */
void anope_cmd_pong(char *servname, char *who)
{
    send_cmd(servname, "PONG %s", who);
}

void anope_cmd_connect(int servernum)
{
    if (servernum == 1)
        anope_cmd_pass(RemotePassword);
    else if (servernum == 2)
        anope_cmd_pass(RemotePassword2);
    else if (servernum == 3)
        anope_cmd_pass(RemotePassword3);
    anope_cmd_server(ServerName, 1, ServerDesc);
    anope_cmd_svinfo();
    anope_cmd_svsinfo();
}

void anope_cmd_svinfo()
{
    send_cmd(NULL, "SVINFO 3 6 %lu", time(NULL));
}

void anope_cmd_svsinfo()
{
    send_cmd(NULL, "SVSINFO %lu %d", time(NULL), maxusercnt);
}

/* PASS */
void anope_cmd_pass(char *pass)
{
    send_cmd(NULL, "PASS %s :TS", pass);
}

void anope_cmd_server(char *servname, int hop, char *descript)
{
    send_cmd(NULL, "SERVER %s %d Anope.Services%s :%s", servname, hop,
             version_number, descript);
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

void anope_cmd_notice_ops(char *source, char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    send_cmd(NULL, "NOTICE @%s :%s", dest, buf);
}


void anope_cmd_notice(char *source, char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    if (UsePrivmsg) {
        anope_cmd_privmsg2(source, dest, buf);
    } else {
        send_cmd(source, "NOTICE %s :%s", dest, buf);
    }
}

void anope_cmd_notice2(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE %s :%s", dest, msg);
}

void anope_cmd_privmsg(char *source, char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    send_cmd(source, "PRIVMSG %s :%s", dest, buf);
}

void anope_cmd_privmsg2(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG %s :%s", dest, msg);
}

void anope_cmd_serv_notice(char *source, char *dest, char *msg)
{
    send_cmd(source, "NOTICE $%s :%s", dest, msg);
}

void anope_cmd_serv_privmsg(char *source, char *dest, char *msg)
{
    send_cmd(source, "PRIVMSG $%s :%s", dest, msg);
}

/* GLOBOPS */
void anope_cmd_global(char *source, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    send_cmd(source ? source : ServerName, "GLOBOPS :%s", buf);
}

/* 391 */
void anope_cmd_391(char *source, char *timestr)
{
    if (!timestr) {
        return;
    }
    send_cmd(NULL, "391 :%s %s :%s", source, ServerName, timestr);
}

/* 250 */
void anope_cmd_250(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    send_cmd(NULL, "250 %s ", buf);
}

/* 307 */
void anope_cmd_307(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "307 %s ", buf);
}

/* 311 */
void anope_cmd_311(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "311 %s ", buf);
}

/* 312 */
void anope_cmd_312(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "312 %s ", buf);
}

/* 317 */
void anope_cmd_317(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    send_cmd(ServerName, "317 %s ", buf);
}

/* 219 */
void anope_cmd_219(char *source, char *letter)
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
void anope_cmd_401(char *source, char *who)
{
    if (!source || !who) {
        return;
    }
    send_cmd(ServerName, "401 %s %s :No such service.", source, who);
}

/* 318 */
void anope_cmd_318(char *source, char *who)
{
    if (!source || !who) {
        return;
    }

    send_cmd(ServerName, "318 %s %s :End of /WHOIS list.", source, who);
}

/* 242 */
void anope_cmd_242(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    send_cmd(NULL, "242 %s ", buf);
}

/* 243 */
void anope_cmd_243(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    send_cmd(NULL, "243 %s ", buf);
}

/* 211 */
void anope_cmd_211(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    send_cmd(NULL, "211 %s ", buf);
}

void anope_cmd_mode(char *source, char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!buf) {
        return;
    }

    send_cmd(source, "MODE %s %s", dest, buf);
}

void anope_cmd_nick(char *nick, char *name, char *mode)
{
    send_cmd(NULL, "NICK %s 1 %lu %s %s %s %s %s :%s", nick, time(NULL),
             mode, ServiceUser, ServiceHost, ServiceHost, ServerName,
             name);
    anope_cmd_sqline(nick, "Reserved for services");
}

void anope_cmd_kick(char *source, char *chan, char *user, const char *fmt,
                    ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
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
void anope_cmd_quit(char *source, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }

    if (buf) {
        send_cmd(source, "QUIT :%s", buf);
    } else {
        send_cmd(source, "QUIT");
    }
}

void anope_cmd_part(char *nick, char *chan, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }

    if (buf) {
        send_cmd(nick, "PART %s :%s", chan, buf);
    } else {
        send_cmd(nick, "PART %s", chan);
    }
}

void anope_cmd_topic(char *whosets, char *chan, char *whosetit,
                     char *topic, time_t when)
{
    send_cmd(whosets, "TOPIC %s :%s", chan, topic);
}

void anope_cmd_vhost_off(char *nick)
{
    /* does not support vhosting */
}

void anope_cmd_vhost_on(char *nick, char *vIdent, char *vhost)
{
    /* does not support vhosting */
}

/* INVITE */
void anope_cmd_invite(char *source, char *chan, char *nick)
{
    if (!source || !chan || !nick) {
        return;
    }

    send_cmd(source, "INVITE %s %s", nick, chan);
}

void anope_cmd_372(char *source, char *msg)
{
    send_cmd(ServerName, "372 %s :- %s", source, msg);
}

void anope_cmd_372_error(char *source)
{
    send_cmd(ServerName, "372 %s :- MOTD file not found!  Please "
             "contact your IRC administrator.", source);
}

void anope_cmd_375(char *source)
{
    send_cmd(ServerName, "375 %s :- %s Message of the Day",
             source, ServerName);
}

void anope_cmd_376(char *source)
{
    send_cmd(ServerName, "376 %s :End of /MOTD command.", source);
}


void anope_set_umode(User * user, int ac, char **av)
{
    int add = 1;                /* 1 if adding modes, 0 if deleting */
    char *modes = av[0];

    ac--;

    if (debug)
        alog("debug: Changing mode for %s to %s", user->nick, modes);

    while (*modes) {

        add ? (user->mode |= umodes[(int) *modes]) : (user->mode &=
                                                      ~umodes[(int)
                                                              *modes]);

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
                if (is_services_admin(user)) {
                    send_cmd(ServerName, "SVSMODE %s +a", user->nick);
                    user->mode |= UMODE_a;
                }

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

int anope_event_ping(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    anope_cmd_pong(ac > 1 ? av[1] : ServerName, av[0]);
    return MOD_CONT;
}

int anope_event_away(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;

    if (!source) {
        return MOD_CONT;
    }
    m_away(source, av[0]);
    return MOD_CONT;
}

void anope_cmd_bot_nick(char *nick, char *user, char *host, char *real,
                        char *modes)
{
    send_cmd(NULL, "NICK %s 1 %lu %s %s %s %s %s :%s", nick, time(NULL),
             modes, user, host, host, ServerName, real);
    anope_cmd_sqline(nick, "Reserved for services");

}

void anope_cmd_351(char *source)
{
    send_cmd(ServerName, "351 %s Anope-%s %s :%s - %s -- %s",
             source, version_number, ServerName, ircd->name, version_flags,
             version_build);
}

/* SVSHOLD - set */
void anope_cmd_svshold(char *nick)
{
    /* Not supported by this IRCD */
}

/* SVSHOLD - release */
void anope_cmd_relase_svshold(char *nick)
{
    /* Not Supported by this IRCD */
}

/* UNSGLINE */
void anope_cmd_unsgline(char *mask)
{
    /* Not Supported by this IRCD */
}

/* UNSZLINE */
void anope_cmd_unszline(char *mask)
{
    /* Not Supported by this IRCD */
}

/* SZLINE */
void anope_cmd_szline(char *mask, char *reason)
{
    /* Not Supported by this IRCD */
}

/* SGLINE */
void anope_cmd_sgline(char *mask, char *reason)
{
    /* Not Supported by this IRCD */
}

/* SVSNICK */
void anope_cmd_svsnick(char *source, char *guest, time_t when)
{
    if (!source || !guest) {
        return;
    }
    send_cmd(NULL, "SVSNICK %s %s :%ld", source, guest, when);
}

void anope_cmd_guest_nick(char *nick, char *user, char *host, char *real,
                          char *modes)
{
    send_cmd(NULL, "NICK %s 1 %lu %s %s %s %s %s :%s", nick, time(NULL),
             modes, user, host, host, ServerName, real);
}


void anope_cmd_unban(char *name, char *nick)
{
    /* Not Supported by this IRCD */
}

void anope_cmd_svso(char *source, char *nick, char *flag)
{
    /* Not Supported by this IRCD */
}


/* SVSMODE +d */
/* sent if svid is something weird */
void anope_cmd_svid_umode(char *nick, time_t ts)
{
    /* Not Supported by this ircd */
}

/* SVSMODE +d */
/* nc_change was = 1, and there is no na->status */
void anope_cmd_nc_change(User * u)
{
    common_svsmode(u, "+d", "1");
}

/* SVSMODE +d */
/* sent if svid is something weird */
void anope_cmd_svid_umode2(User * u, char *ts)
{
    common_svsmode(u, "+r", NULL);
}

void anope_cmd_svid_umode3(User * u, char *ts)
{
    /* Bahamuts have this extra one, since they can check on even nick changes */
}

/* NICK <newnick>  */
void anope_cmd_chg_nick(char *oldnick, char *newnick)
{
    if (!oldnick || !newnick) {
        return;
    }

    send_cmd(oldnick, "NICK %s", newnick);
}


#endif
