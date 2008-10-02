/* Bahamut functions
 *
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
#include "bahamut.h"

IRCDVar myIrcd[] = {
    {"BahamutIRCd 1.4.*/1.8.*", /* ircd name */
     "+o",                      /* nickserv mode */
     "+o",                      /* chanserv mode */
     "+o",                      /* memoserv mode */
     "+",                       /* hostserv mode */
     "+io",                     /* operserv mode */
     "+o",                      /* botserv mode  */
     "+h",                      /* helpserv mode */
     "+i",                      /* Dev/Null mode */
     "+io",                     /* Global mode   */
     "+o",                      /* nickserv alias mode */
     "+o",                      /* chanserv alias mode */
     "+o",                      /* memoserv alias mode */
     "+",                       /* hostserv alias mode */
     "+io",                     /* operserv alias mode */
     "+o",                      /* botserv alias mode  */
     "+h",                      /* helpserv alias mode */
     "+i",                      /* Dev/Null alias mode */
     "+io",                     /* Global alias mode   */
     "+",                       /* Used by BotServ Bots */
     2,                         /* Chan Max Symbols     */
     "-cilmnpstOR",             /* Modes to Remove */
     "+o",                      /* Channel Umode used by Botserv bots */
     1,                         /* SVSNICK */
     0,                         /* Vhost  */
     0,                         /* Has Owner */
     NULL,                      /* Mode to set for an owner */
     NULL,                      /* Mode to unset for an owner */
     NULL,                      /* Mode to set for channel admin */
     NULL,                      /* Mode to unset for channel admin */
     "+rd",                     /* Mode On Reg          */
     NULL,                      /* Mode on ID for Roots */
     NULL,                      /* Mode on ID for Admins */
     NULL,                      /* Mode on ID for Opers */
     "-r+d",                    /* Mode on UnReg        */
     "+d",                      /* Mode on Nick Change  */
     1,                         /* Supports SGlines     */
     1,                         /* Supports SQlines     */
     1,                         /* Supports SZlines     */
     0,                         /* Supports Halfop +h   */
     3,                         /* Number of server args */
     0,                         /* Join 2 Set           */
     0,                         /* Join 2 Message       */
     1,                         /* Has exceptions +e    */
     0,                         /* TS Topic Forward     */
     0,                         /* TS Topci Backward    */
     0,                         /* Protected Umode      */
     0,                         /* Has Admin            */
     1,                         /* Chan SQlines         */
     1,                         /* Quit on Kill         */
     1,                         /* SVSMODE unban        */
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
     0,                         /* VHOST ON NICK        */
     0,                         /* Change RealName      */
     0,                         /* No Knock             */
     0,                         /* Admin Only           */
     DEFAULT_MLOCK,             /* Default MLOCK       */
     0,                         /* Vhost Mode           */
     1,                         /* +f                   */
     0,                         /* +L                   */
     CMODE_j,                   /* Mode */
     0,                         /* Mode */
     1,
     1,                         /* No Knock requires +i */
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
     0,                         /* ts6 */
     1,                         /* support helper umode */
     0,                         /* p10 */
     NULL,                      /* character set */
     1,                         /* reports sync state */
     0,                         /* CIDR channelbans */
     "$",                       /* TLD Prefix for Global */
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
     0,                         /* ZIP          */
     CAPAB_BURST,               /* BURST        */
     0,                         /* TS5          */
     0,                         /* TS3          */
     CAPAB_DKEY,                /* DKEY         */
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
     0,                         /* SSJ3         */
     0,                         /* NICK2        */
     0,                         /* UMODE2       */
     0,                         /* VL           */
     0,                         /* TLKEXT       */
     0,                         /* DODKEY       */
     CAPAB_DOZIP,               /* DOZIP        */
     0, 0, 0}
};


void BahamutIRCdProto::ProcessUsermodes(User *user, int ac, const char **av)
{
	int add = 1; /* 1 if adding modes, 0 if deleting */
	const char *modes = av[0];
	--ac;
	if (debug) alog("debug: Changing mode for %s to %s", user->nick, modes);
	while (*modes) {
		/* This looks better, much better than "add ? (do_add) : (do_remove)".
		 * At least this is readable without paying much attention :) -GD */
		if (add) user->mode |= umodes[static_cast<int>(*modes)];
		else user->mode &= ~umodes[static_cast<int>(*modes)];
		switch (*modes++) {
			case '+':
				add = 1;
				break;
			case '-':
				add = 0;
				break;
			case 'a':
				if (UnRestrictSAdmin) break;
				if (add && !is_services_admin(user)) {
					common_svsmode(user, "-a", NULL);
					user->mode &= ~UMODE_a;
				}
				break;
			case 'd':
				if (!ac) {
					alog("user: umode +d with no parameter (?) for user %s", user->nick);
					break;
				}
				--ac;
				++av;
				user->svid = strtoul(*av, NULL, 0);
				break;
			case 'o':
				if (add) {
					++opcnt;
					if (WallOper) anope_SendGlobops(s_OperServ, "\2%s\2 is now an IRC operator.", user->nick);
					display_news(user, NEWS_OPER);
				}
				else --opcnt;
				break;
			case 'r':
				if (add && !nick_identified(user)) {
					common_svsmode(user, "-r", NULL);
					user->mode &= ~UMODE_r;
				}
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
    UMODE_D, 0, UMODE_F,        /* D E F */
    0, 0, UMODE_I,              /* G H I */
    0, UMODE_K, 0,              /* J K L */
    0, 0, UMODE_O,              /* M N O */
    0, 0, UMODE_R,              /* P Q R */
    0, 0, 0,                    /* S T U */
    0, 0, UMODE_X,              /* V W X */
    0,                          /* Y */
    0,                          /* Z */
    0, 0, 0,                    /* [ \ ] */
    0, 0, 0,                    /* ^ _ ` */
    UMODE_a, UMODE_b, UMODE_c,  /* a b c */
    UMODE_d, UMODE_e, UMODE_f,  /* d e f */
    UMODE_g, UMODE_h, UMODE_i,  /* g h i */
    UMODE_j, UMODE_k, 0,        /* j k l */
    UMODE_m, UMODE_n, UMODE_o,  /* m n o */
    0, 0, UMODE_r,              /* p q r */
    UMODE_s, 0, 0,              /* s t u */
    0, UMODE_w, UMODE_x,        /* v w x */
    UMODE_y,                    /* y */
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
    {CMODE_M, 0, NULL, NULL},   /* M */
    {0},                        /* N */
    {CMODE_O, CBM_NO_USER_MLOCK, NULL, NULL},
    {0},                        /* P */
    {0},                        /* Q */
    {CMODE_R, 0, NULL, NULL},   /* R */
    {0},                        /* S */
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
    {CMODE_j, 0, set_flood, cs_set_flood},      /* j */
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
    {'j', CMODE_j, 0, get_flood, cs_get_flood},
    {'k', CMODE_k, 0, get_key, cs_get_key},
    {'l', CMODE_l, CBM_MINUS_NO_ARG, get_limit, cs_get_limit},
    {'m', CMODE_m, 0, NULL, NULL},
    {'n', CMODE_n, 0, NULL, NULL},
    {'p', CMODE_p, 0, NULL, NULL},
    {'r', CMODE_r, 0, NULL, NULL},
    {'s', CMODE_s, 0, NULL, NULL},
    {'t', CMODE_t, 0, NULL, NULL},
    {'M', CMODE_M, 0, NULL, NULL},
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



void BahamutIRCdProto::SendModeInternal(const char *source, const char *dest, const char *buf)
{
	if (!buf) return;
	if (ircdcap->tsmode && (uplink_capab & ircdcap->tsmode)) send_cmd(source, "MODE %s 0 %s", dest, buf);
	else send_cmd(source, "MODE %s %s", dest, buf);
}

/* SVSHOLD - set */
void BahamutIRCdProto::SendSVSHOLD(const char *nick)
{
	send_cmd(ServerName, "SVSHOLD %s %d :%s", nick, NSReleaseTimeout, "Being held for registered user");
}

/* SVSHOLD - release */
void BahamutIRCdProto::SendSVSHOLDDel(const char *nick)
{
	send_cmd(ServerName, "SVSHOLD %s 0", nick);
}

/* SVSMODE -b */
void BahamutIRCdProto::SendBanDel(const char *name, const char *nick)
{
	SendSVSMode_chan(name, "-b", nick);
}


/* SVSMODE channel modes */

void BahamutIRCdProto::SendSVSMode_chan(const char *name, const char *mode, const char *nick)
{
	if (nick) send_cmd(ServerName, "SVSMODE %s %s %s", name, mode, nick);
	else send_cmd(ServerName, "SVSMODE %s %s", name, mode);
}

void BahamutIRCdProto::SendBotOp(const char *nick, const char *chan)
{
	SendMode(nick, chan, "%s %s", ircd->botchanumode, nick);
}

/* EVENT: SJOIN */
int anope_event_sjoin(const char *source, int ac, const char **av)
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
int anope_event_nick(const char *source, int ac, const char **av)
{
    User *user;

    if (ac != 2) {
        user = do_nick(source, av[0], av[4], av[5], av[6], av[9],
                       strtoul(av[2], NULL, 10), strtoul(av[7], NULL, 0),
                       strtoul(av[8], NULL, 0), NULL, NULL);
        if (user) {
            anope_ProcessUsermodes(user, 1, &av[3]);
        }
    } else {
        do_nick(source, av[0], NULL, NULL, NULL, NULL,
                strtoul(av[1], NULL, 10), 0, 0, NULL, NULL);
    }
    return MOD_CONT;
}

/* EVENT : CAPAB */
int anope_event_capab(const char *source, int ac, const char **av)
{
    capab_parse(ac, av);
    return MOD_CONT;
}

/* EVENT : OS */
int anope_event_os(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;
    m_privmsg(source, s_OperServ, av[0]);
    return MOD_CONT;
}

/* EVENT : NS */
int anope_event_ns(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;
    m_privmsg(source, s_NickServ, av[0]);
    return MOD_CONT;
}

/* EVENT : MS */
int anope_event_ms(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;
    m_privmsg(source, s_MemoServ, av[0]);
    return MOD_CONT;
}

/* EVENT : HS */
int anope_event_hs(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;
    m_privmsg(source, s_HostServ, av[0]);
    return MOD_CONT;
}

/* EVENT : CS */
int anope_event_cs(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;
    m_privmsg(source, s_ChanServ, av[0]);
    return MOD_CONT;
}

int anope_event_436(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;

    m_nickcoll(av[0]);
    return MOD_CONT;
}


/* SQLINE */
void BahamutIRCdProto::SendSQLine(const char *mask, const char *reason)
{
	if (!mask || !reason) return;
	send_cmd(NULL, "SQLINE %s :%s", mask, reason);
}

/* UNSGLINE */
void BahamutIRCdProto::SendSGLineDel(const char *mask)
{
	send_cmd(NULL, "UNSGLINE 0 :%s", mask);
}

/* UNSZLINE */
void BahamutIRCdProto::SendSZLineDel(const char *mask)
{
	/* this will likely fail so its only here for legacy */
	send_cmd(NULL, "UNSZLINE 0 %s", mask);
	/* this is how we are supposed to deal with it */
	send_cmd(NULL, "RAKILL %s *", mask);
}

/* SZLINE */
void BahamutIRCdProto::SendSZLine(const char *mask, const char *reason, const char *whom)
{
	/* this will likely fail so its only here for legacy */
	send_cmd(NULL, "SZLINE %s :%s", mask, reason);
	/* this is how we are supposed to deal with it */
	send_cmd(NULL, "AKILL %s * %d %s %ld :%s", mask, 172800, whom, static_cast<long>(time(NULL)), reason);
}

/* SVSNOOP */
void BahamutIRCdProto::SendSVSNOOP(const char *server, int set)
{
	send_cmd(NULL, "SVSNOOP %s %s", server, set ? "+" : "-");
}

/* SGLINE */
void BahamutIRCdProto::SendSGLine(const char *mask, const char *reason)
{
	send_cmd(NULL, "SGLINE %d :%s:%s", static_cast<int>(strlen(mask)), mask, reason);
}

/* RAKILL */
void BahamutIRCdProto::SendAkillDel(const char *user, const char *host)
{
	send_cmd(NULL, "RAKILL %s %s", host, user);
}

/* TOPIC */
void BahamutIRCdProto::SendTopic(BotInfo *whosets, const char *chan, const char *whosetit, const char *topic, time_t when)
{
	send_cmd(whosets->nick, "TOPIC %s %s %lu :%s", chan, whosetit, static_cast<unsigned long>(when), topic);
}

/* UNSQLINE */
void BahamutIRCdProto::SendSQLineDel(const char *user)
{
	send_cmd(NULL, "UNSQLINE %s", user);
}

/* JOIN - SJOIN */
void BahamutIRCdProto::SendJoin(const char *user, const char *channel, time_t chantime)
{
	send_cmd(user, "SJOIN %ld %s", static_cast<long>(chantime), channel);
}

void bahamut_cmd_burst()
{
    send_cmd(NULL, "BURST");
}

/* AKILL */
/* parv[1]=host
 * parv[2]=user
 * parv[3]=length
 * parv[4]=akiller
 * parv[5]=time set
 * parv[6]=reason
 */
void BahamutIRCdProto::SendAkill(const char *user, const char *host, const char *who, time_t when, time_t expires, const char *reason)
{
	// Calculate the time left before this would expire, capping it at 2 days
	time_t timeleft = expires - time(NULL);
	if (timeleft > 172800) timeleft = 172800;
	send_cmd(NULL, "AKILL %s %s %d %s %ld :%s", host, user, timeleft, who, static_cast<long>(time(NULL)), reason);
}

/* SVSKILL */
/* parv[0] = servername
 * parv[1] = client
 * parv[2] = nick stamp
 * parv[3] = kill message
 */
/*
  Note: if the stamp is null 0, the below usage is correct of Bahamut
*/
void BahamutIRCdProto::SendSVSKillInternal(const char *source, const char *user, const char *buf)
{
	if (!source || !user || !buf) return;
	send_cmd(source, "SVSKILL %s :%s", user, buf);
}

/* SVSMODE */
/* parv[0] - sender
 * parv[1] - nick
 * parv[2] - TS (or mode, depending on svs version)
 * parv[3] - mode (or services id if old svs version)
 * parv[4] - optional arguement (services id)
 */
void BahamutIRCdProto::SendSVSMode(User *u, int ac, const char **av)
{
	send_cmd(ServerName, "SVSMODE %s %ld %s", u->nick, static_cast<long>(u->timestamp), merge_args(ac, av));
}

/*
 * SVINFO
 *       parv[0] = sender prefix
 *       parv[1] = TS_CURRENT for the server
 *       parv[2] = TS_MIN for the server
 *       parv[3] = server is standalone or connected to non-TS only
 *       parv[4] = server's idea of UTC time
 */
void bahamut_cmd_svinfo()
{
    send_cmd(NULL, "SVINFO 3 1 0 :%ld", (long int) time(NULL));
}

/* PASS */
void bahamut_cmd_pass(const char *pass)
{
    send_cmd(NULL, "PASS %s :TS", pass);
}

/* SERVER */
void BahamutIRCdProto::SendServer(const char *servname, int hop, const char *descript)
{
	send_cmd(NULL, "SERVER %s %d :%s", servname, hop, descript);
}

/* CAPAB */
void bahamut_cmd_capab()
{
    send_cmd(NULL,
             "CAPAB SSJOIN NOQUIT BURST UNCONNECT NICKIP TSMODE TS3");
}

void BahamutIRCdProto::SendConnect()
{
	me_server = new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);
	if (servernum == 1) bahamut_cmd_pass(RemotePassword);
	else if (servernum == 2) bahamut_cmd_pass(RemotePassword2);
	else if (servernum == 3) bahamut_cmd_pass(RemotePassword3);
	bahamut_cmd_capab();
	SendServer(ServerName, 1, ServerDesc);
	bahamut_cmd_svinfo();
	bahamut_cmd_burst();
}





/* EVENT : SERVER */
int anope_event_server(const char *source, int ac, const char **av)
{
    if (!stricmp(av[1], "1")) {
        uplink = sstrdup(av[0]);
    }

    do_server(source, av[0], av[1], av[2], NULL);
    return MOD_CONT;
}

/* EVENT : PRIVMSG */
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

int anope_event_topic(const char *source, int ac, const char **av)
{
    if (ac != 4)
        return MOD_CONT;
    do_topic(source, ac, av);
    return MOD_CONT;
}

int anope_event_squit(const char *source, int ac, const char **av)
{
    if (ac != 2)
        return MOD_CONT;
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

/* EVENT: MODE */
int anope_event_mode(const char *source, int ac, const char **av)
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

/* EVENT: KILL */
int anope_event_kill(const char *source, int ac, const char **av)
{
    if (ac != 2)
        return MOD_CONT;

    m_kill(av[0], av[1]);
    return MOD_CONT;
}

/* EVENT: KICK */
int anope_event_kick(const char *source, int ac, const char **av)
{
    if (ac != 3)
        return MOD_CONT;
    do_kick(source, ac, av);
    return MOD_CONT;
}

/* EVENT: JOIN */
int anope_event_join(const char *source, int ac, const char **av)
{
    if (ac != 1)
        return MOD_CONT;
    do_join(source, ac, av);
    return MOD_CONT;
}

/* EVENT: MOTD */
int anope_event_motd(const char *source, int ac, const char **av)
{
    if (!source) {
        return MOD_CONT;
    }

    m_motd(source);
    return MOD_CONT;
}

void BahamutIRCdProto::SendNoticeChanops(const char *source, const char *dest, const char *buf)
{
	if (!buf) return;
	send_cmd(NULL, "NOTICE @%s :%s", dest, buf);
}

void BahamutIRCdProto::SendKick(const char *source, const char *chan, const char *user, const char *buf)
{
	if (buf) send_cmd(source, "KICK %s %s :%s", chan, user, buf);
	else send_cmd(source, "KICK %s %s", chan, user);
}

int anope_event_away(const char *source, int ac, const char **av)
{
    if (!source) {
        return MOD_CONT;
    }
    m_away(source, (ac ? av[0] : NULL));
    return MOD_CONT;
}

int anope_event_ping(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;
    ircd_proto.SendPong(ac > 1 ? av[1] : ServerName, av[0]);
    return MOD_CONT;
}

void BahamutIRCdProto::SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes)
{
	EnforceQlinedNick(nick, s_BotServ);
	send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s 0 0 :%s", nick, static_cast<long>(time(NULL)), modes, user, host, ServerName, real);
	SendSQLine(nick, "Reserved for services");
}

void BahamutIRCdProto::SendGuestNick(const char *nick, const char *user, const char *host, const char *real, const char *modes)
{
	send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s 0 0 :%s", nick, static_cast<long>(time(NULL)), modes, user, host, ServerName, real);
}

/* SVSMODE +d */
/* sent if svid is something weird */
void BahamutIRCdProto::SendSVID(const char *nick, time_t ts)
{
	send_cmd(ServerName, "SVSMODE %s %lu +d 1", nick, static_cast<unsigned long>(ts));
}


/* SVSMODE +d */
/* nc_change was = 1, and there is no na->status */
void BahamutIRCdProto::SendUnregisteredNick(User *u)
{
	common_svsmode(u, "+d", "1");
}

void BahamutIRCdProto::SendSVID3(User *u, const char *ts)
{
	if (u->svid != u->timestamp) common_svsmode(u, "+rd", ts);
	else common_svsmode(u, "+r", NULL);
}

int anope_event_error(const char *source, int ac, const char **av)
{
    if (ac >= 1) {
        if (debug) {
            alog("debug: %s", av[0]);
        }
    }
    return MOD_CONT;
}

void BahamutIRCdProto::SendEOB()
{
	send_cmd(NULL, "BURST 0");
}

int anope_event_burst(const char *source, int ac, const char **av)
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

int BahamutIRCdProto::IsFloodModeParamValid(const char *value)
{
	char *dp, *end;
	if (value && *value != ':' && strtoul((*value == '*' ? value + 1 : value), &dp, 10) > 0 && *dp == ':' && *(++dp) && strtoul(dp, &end, 10) > 0 && !*end) return 1;
	else return 0;
}

/* this avoids "undefined symbol" messages of those whom try to load mods that
   call on this function */
void bahamut_cmd_chghost(const char *nick, const char *vhost)
{
    if (debug) {
        alog("debug: This IRCD does not support vhosting");
    }
}

/* *INDENT-OFF* */
void moduleAddIRCDMsgs(void) {
    Message *m;


    /* first update the cs protect info about this ircd */
    updateProtectDetails("PROTECT","PROTECTME","protect","deprotect","AUTOPROTECT","+","-");

    /* now add the commands */
    m = createMessage("436",       anope_event_436); addCoreMessage(IRCD,m);
    m = createMessage("AWAY",      anope_event_away); addCoreMessage(IRCD,m);
    m = createMessage("JOIN",      anope_event_join); addCoreMessage(IRCD,m);
    m = createMessage("KICK",      anope_event_kick); addCoreMessage(IRCD,m);
    m = createMessage("KILL",      anope_event_kill); addCoreMessage(IRCD,m);
    m = createMessage("MODE",      anope_event_mode); addCoreMessage(IRCD,m);
    m = createMessage("MOTD",      anope_event_motd); addCoreMessage(IRCD,m);
    m = createMessage("NICK",      anope_event_nick); addCoreMessage(IRCD,m);
    m = createMessage("PART",      anope_event_part); addCoreMessage(IRCD,m);
    m = createMessage("PING",      anope_event_ping); addCoreMessage(IRCD,m);
    m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
    m = createMessage("QUIT",      anope_event_quit); addCoreMessage(IRCD,m);
    m = createMessage("SERVER",    anope_event_server); addCoreMessage(IRCD,m);
    m = createMessage("SQUIT",     anope_event_squit); addCoreMessage(IRCD,m);
    m = createMessage("TOPIC",     anope_event_topic); addCoreMessage(IRCD,m);
    m = createMessage("WHOIS",     anope_event_whois); addCoreMessage(IRCD,m);
    m = createMessage("SVSMODE",   anope_event_mode); addCoreMessage(IRCD,m);
    m = createMessage("CAPAB", 	   anope_event_capab); addCoreMessage(IRCD,m);
    m = createMessage("CS",        anope_event_cs); addCoreMessage(IRCD,m);
    m = createMessage("HS",        anope_event_hs); addCoreMessage(IRCD,m);
    m = createMessage("MS",        anope_event_ms); addCoreMessage(IRCD,m);
    m = createMessage("NS",        anope_event_ns); addCoreMessage(IRCD,m);
    m = createMessage("OS",        anope_event_os); addCoreMessage(IRCD,m);
    m = createMessage("SJOIN",     anope_event_sjoin); addCoreMessage(IRCD,m);
    m = createMessage("ERROR",     anope_event_error); addCoreMessage(IRCD,m);
    m = createMessage("BURST",     anope_event_burst); addCoreMessage(IRCD,m);
}

/* *INDENT-ON* */

/**
 * Now tell anope how to use us.
 **/
int AnopeInit(int argc, char **argv)
{

    moduleAddAuthor("Anope");
    moduleAddVersion
        ("$Id$");
    moduleSetType(PROTOCOL);

    pmodule_ircd_version("BahamutIRCd 1.4.*/1.8.*");
    pmodule_ircd_cap(myIrcdcap);
    pmodule_ircd_var(myIrcd);
    pmodule_ircd_cbmodeinfos(myCbmodeinfos);
    pmodule_ircd_cumodes(myCumodes);
    pmodule_ircd_flood_mode_char_set("+j");
    pmodule_ircd_flood_mode_char_remove("-j");
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

	pmodule_ircd_proto(&ircd_proto);
    moduleAddIRCDMsgs();

    return MOD_CONT;
}

MODULE_INIT("bahamut")
