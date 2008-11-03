/* Ratbox IRCD functions
 *
 * (C) 2003-2008 Anope Team
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

#define UMODE_a 0x00000001
#define UMODE_C 0x00000002
#define UMODE_i 0x00000004
#define UMODE_o 0x00000008
#define UMODE_z 0x00000010
#define UMODE_w 0x00000020
#define UMODE_s 0x00000040
#define UMODE_c 0x00000080
#define UMODE_r 0x00000100
#define UMODE_k 0x00000200
#define UMODE_f 0x00000400
#define UMODE_y 0x00000800
#define UMODE_d 0x00001000
#define UMODE_n 0x00002000
#define UMODE_x 0x00004000
#define UMODE_u 0x00008000
#define UMODE_b 0x00010000
#define UMODE_l 0x00020000
#define UMODE_g 0x00040000
#define UMODE_Z 0x00080000

#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040
#define CMODE_l 0x00000080


#define DEFAULT_MLOCK CMODE_n | CMODE_t

IRCDVar myIrcd[] = {
    {"Ratbox 2.0+",             /* ircd name */
     "+oi",                     /* nickserv mode */
     "+oi",                     /* chanserv mode */
     "+oi",                     /* memoserv mode */
     "+oi",                     /* hostserv mode */
     "+oai",                    /* operserv mode */
     "+oi",                     /* botserv mode  */
     "+oi",                     /* helpserv mode */
     "+oi",                     /* Global mode   */
     "+oi",                     /* Used by BotServ Bots */
     2,                         /* Chan Max Symbols     */
     "-acilmnpst",              /* Modes to Remove */
     "+o",                      /* Channel Umode used by Botserv bots */
     0,                         /* SVSNICK */
     0,                         /* Vhost  */
     0,                         /* Has Owner */
     NULL,                      /* Mode to set for an owner */
     NULL,                      /* Mode to unset for an owner */
     NULL,                      /* Mode to set for chan admin */
     NULL,                      /* Mode to unset for chan admin */
     NULL,                      /* Mode On Reg          */
     NULL,                      /* Mode on ID for Roots */
     NULL,                      /* Mode on ID for Admins */
     NULL,                      /* Mode on ID for Opers */
     NULL,                      /* Mode on UnReg        */
     NULL,                      /* Mode on Nick Change  */
     1,                         /* Supports SGlines     */
     1,                         /* Supports SQlines     */
     0,                         /* Supports SZlines     */
     0,                         /* Supports Halfop +h   */
     3,                         /* Number of server args */
     1,                         /* Join 2 Set           */
     1,                         /* Join 2 Message       */
     1,                         /* Has exceptions +e    */
     0,                         /* TS Topic Forward     */
     0,                         /* TS Topci Backward    */
     0,                         /* Protected Umode      */
     0,                         /* Has Admin            */
     1,                         /* Chan SQlines         */
     0,                         /* Quit on Kill         */
     0,                         /* SVSMODE unban        */
     0,                         /* Has Protect          */
     0,                         /* Reverse              */
     0,                         /* Chan Reg             */
     0,                         /* Channel Mode         */
     0,                         /* vidents              */
     0,                         /* svshold              */
     0,                         /* time stamp on mode   */
     0,                         /* NICKIP               */
     0,                         /* UMODE                */
     0,                         /* O:LINE               */
     0,                         /* VHOST ON NICK        */
     0,                         /* Change RealName      */
     CMODE_p,                   /* No Knock             */
     0,                         /* Admin Only           */
     DEFAULT_MLOCK,             /* Default MLOCK        */
     0,                         /* Vhost Mode           */
     0,                         /* +f                   */
     0,                         /* +L                   */
     0,                         /* +f Mode                          */
     0,                         /* +L Mode                              */
     0,                         /* On nick change check if they could be identified */
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
     NULL,                      /* vhost char */
     1,                         /* ts6 */
     0,                         /* support helper umode */
     0,                         /* p10 */
     NULL,                      /* character set */
     0,                         /* reports sync state */
     0,                         /* CIDR channelbans */
     "$$",                      /* TLD Prefix for Global */
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
     CAPAB_TS5,                 /* TS5          */
     0,                         /* TS3          */
     0,                         /* DKEY         */
     0,                         /* PT4          */
     0,                         /* SCS          */
     CAPAB_QS,                  /* QS           */
     CAPAB_UID,                 /* UID          */
     CAPAB_KNOCK,               /* KNOCK        */
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
    0, 0, 0,                    /* A B C */
    0, 0, 0,                    /* D E F */
    0, 0, 0,                    /* G H I */
    0, 0, 0,                    /* J K L */
    0, 0, 0,                    /* M N O */
    0, 0, 0,                    /* P Q R */
    0, 0, 0,                    /* S T U */
    0, 0, 0,                    /* V W X */
    0,                          /* Y */
    0,                          /* Z */
    0, 0, 0,                    /* [ \ ] */
    0, 0, 0,                    /* ^ _ ` */
    UMODE_a, UMODE_b, 0,        /* a b c */
    UMODE_d, 0, 0,              /* d e f */
    UMODE_g, 0, UMODE_i,        /* g h i */
    0, 0, UMODE_l,              /* j k l */
    0, UMODE_n, UMODE_o,  /* m n o */
    0, 0, 0,                    /* p q r */
    0, 0, UMODE_u,              /* s t u */
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
    {0},                        /* N */
    {0},                        /* O */
    {0},                        /* P */
    {0},                        /* Q */
    {0},                        /* R */
    {0},                        /* S */
    {0},                        /* T */
    {0},                        /* U */
    {0},                        /* V */
    {0},                        /* W */
    {0},                        /* X */
    {0},                        /* Y */
    {0},                        /* Z */
    {0}, {0}, {0}, {0}, {0}, {0},
    {0},
    {0},                        /* b */
    {0},                        /* c */
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
    {0},
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
    {'i', CMODE_i, 0, NULL, NULL},
    {'k', CMODE_k, 0, get_key, cs_get_key},
    {'l', CMODE_l, CBM_MINUS_NO_ARG, get_limit, cs_get_limit},
    {'m', CMODE_m, 0, NULL, NULL},
    {'n', CMODE_n, 0, NULL, NULL},
    {'p', CMODE_p, 0, NULL, NULL},
    {'s', CMODE_s, 0, NULL, NULL},
    {'t', CMODE_t, 0, NULL, NULL},
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
    {0},
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




/*
 * SVINFO
 *      parv[0] = sender prefix
 *      parv[1] = TS_CURRENT for the server
 *      parv[2] = TS_MIN for the server
 *      parv[3] = server is standalone or connected to non-TS only
 *      parv[4] = server's idea of UTC time
 */
void ratbox_cmd_svinfo()
{
    send_cmd(NULL, "SVINFO 6 3 0 :%ld", (long int) time(NULL));
}

void ratbox_cmd_svsinfo()
{

}

void ratbox_cmd_tmode(const char *source, const char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    if (!*buf) {
        return;
    }

    send_cmd(NULL, "MODE %s %s", dest, buf);
}


/* CAPAB */
/*
  QS     - Can handle quit storm removal
  EX     - Can do channel +e exemptions
  CHW    - Can do channel wall @#
  LL     - Can do lazy links
  IE     - Can do invite exceptions
  EOB    - Can do EOB message
  KLN    - Can do KLINE message
  GLN    - Can do GLINE message
  HUB    - This server is a HUB
  UID    - Can do UIDs
  ZIP    - Can do ZIPlinks
  ENC    - Can do ENCrypted links
  KNOCK  -  supports KNOCK
  TBURST - supports TBURST
  PARA	 - supports invite broadcasting for +p
  ENCAP	 - ?
*/
void ratbox_cmd_capab()
{
    send_cmd(NULL,
             "CAPAB :QS EX CHW IE KLN GLN KNOCK TB UNKLN CLUSTER ENCAP");
}

/* PASS */
void ratbox_cmd_pass(const char *pass)
{
    if (UseTS6) {
        send_cmd(NULL, "PASS %s TS 6 :%s", pass, TS6SID);
    } else {
        send_cmd(NULL, "PASS %s :TS", pass);
    }
}

class RatboxProto : public IRCDTS6Proto
{
	void ProcessUsermodes(User *user, int ac, const char **av)
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
				case 'o':
					if (add) {
						++opcnt;
						if (WallOper) ircdproto->SendGlobops(s_OperServ, "\2%s\2 is now an IRC operator.", user->nick);
						display_news(user, NEWS_OPER);
					}
					else --opcnt;
			}
		}
	}

	void SendGlobopsInternal(const char *source, const char *buf)
	{
		if (source)
		{
			BotInfo *bi = findbot(source);
			if (bi)
			{
				send_cmd(UseTS6 ? bi->uid : source, "OPERWALL :%s", buf);
				return;
			}
		}
		send_cmd(UseTS6 ? TS6SID : ServerName, "OPERWALL :%s", buf);
	}

	void SendSQLine(const char *mask, const char *reason)
	{
		BotInfo *bi = findbot(s_OperServ);
		send_cmd(UseTS6 ? (bi ? bi->uid : s_OperServ) : s_OperServ, "RESV * %s :%s", mask, reason);
	}

	void SendSGLineDel(const char *mask)
	{
		BotInfo *bi = findbot(s_OperServ);
		send_cmd(UseTS6 ? (bi ? bi->uid : s_OperServ) : s_OperServ, "UNXLINE * %s", mask);
	}

	void SendSGLine(const char *mask, const char *reason)
	{
		BotInfo *bi = findbot(s_OperServ);
		send_cmd(UseTS6 ? (bi ? bi->uid : s_OperServ) : s_OperServ, "XLINE * %s 0 :%s", mask, reason);
	}

	void SendAkillDel(const char *user, const char *host)
	{
		BotInfo *bi = findbot(s_OperServ);
		send_cmd(UseTS6 ? (bi ? bi->uid : s_OperServ) : s_OperServ, "UNKLINE * %s %s", user, host);
	}

	void SendSQLineDel(const char *user)
	{
		BotInfo *bi = findbot(s_OperServ);
		send_cmd(UseTS6 ? (bi ? bi->uid : s_OperServ) : s_OperServ, "UNRESV * %s", user);
	}

	void SendJoin(BotInfo *user, const char *channel, time_t chantime)
	{
		send_cmd(NULL, "SJOIN %ld %s + :%s", static_cast<long>(chantime), channel, UseTS6 ? user->uid.c_str() : user->nick);
	}

	/*
	oper:		the nick of the oper performing the kline
	target.server:	the server(s) this kline is destined for
	duration:	the duration if a tkline, 0 if permanent.
	user:		the 'user' portion of the kline
	host:		the 'host' portion of the kline
	reason:		the reason for the kline.
	*/

	void SendAkill(const char *user, const char *host, const char *who, time_t when, time_t expires, const char *reason)
	{
		BotInfo *bi = findbot(s_OperServ);
		send_cmd(UseTS6 ? (bi ? bi->uid : s_OperServ) : s_OperServ, "KLINE * %ld %s %s :%s", static_cast<long>(expires - time(NULL)), user, host, reason);
	}

	void SendSVSKillInternal(const char *source, const char *user, const char *buf)
	{
		BotInfo *bi = findbot(source);
		User *u = find_byuid(user);
		send_cmd(UseTS6 ? (bi ? bi->uid : source) : source, "KILL %s :%s", UseTS6 ? (u ? u->uid : user) : user, buf);
	}

	void SendSVSMode(User *u, int ac, const char **av)
	{
		send_cmd(UseTS6 ? TS6SID : ServerName, "SVSMODE %s %s", u->nick, av[0]);
	}

	/* SERVER name hop descript */
	void SendServer(const char *servname, int hop, const char *descript)
	{
		send_cmd(NULL, "SERVER %s %d :%s", servname, hop, descript);
	}

	void SendConnect()
	{
		/* Make myself known to myself in the serverlist */
		if (UseTS6) me_server = new_server(NULL, ServerName, ServerDesc, SERVER_ISME, TS6SID);
		else me_server = new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);
		if (servernum == 1) ratbox_cmd_pass(RemotePassword);
		else if (servernum == 2) ratbox_cmd_pass(RemotePassword2);
		else if (servernum == 3) ratbox_cmd_pass(RemotePassword3);
		ratbox_cmd_capab();
		SendServer(ServerName, 1, ServerDesc);
		ratbox_cmd_svinfo();
	}

	void SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes, const char *uid)
	{
		EnforceQlinedNick(nick, NULL);
		if (UseTS6) {
			char *uidbuf = ts6_uid_retrieve();
			send_cmd(TS6SID, "UID %s 1 %ld %s %s %s 0 %s :%s", nick, static_cast<long>(time(NULL)), modes, user, host, uidbuf, real);
		}
		else send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s :%s", nick, static_cast<long>(time(NULL)), modes, user, host, ServerName, real);
		SendSQLine(nick, "Reserved for services");
	}

	void SendPartInternal(BotInfo *bi, const char *chan, const char *buf)
	{
		if (buf)
			send_cmd(UseTS6 ? bi->uid : bi->nick, "PART %s :%s", chan, buf);
		else
			send_cmd(UseTS6 ? bi->uid : bi->nick, "PART %s", chan);
	}

	void SendNumericInternal(const char *source, int numeric, const char *dest, const char *buf)
	{
		// This might need to be set in the call to SendNumeric instead of here, will review later -- CyberBotX
		send_cmd(UseTS6 ? TS6SID : source, "%03d %s %s", numeric, dest, buf);
	}

	void SendModeInternal(BotInfo *bi, const char *dest, const char *buf)
	{
		if (bi)
		{
			send_cmd(UseTS6 ? bi->uid : bi->nick, "MODE %s %s", dest, buf);
		}
		else send_cmd(UseTS6 ? TS6SID : ServerName, "MODE %s %s", dest, buf);
	}

	void SendKickInternal(BotInfo *bi, const char *chan, const char *user, const char *buf)
	{
		User *u = finduser(user);
		if (buf) send_cmd(UseTS6 ? bi->uid : bi->nick, "KICK %s %s :%s", chan, UseTS6 ? (u ? u->uid : user) : user, buf);
		else send_cmd(UseTS6 ? bi->uid : bi->nick, "KICK %s %s", chan, UseTS6 ? (u ? u->uid : user) : user);
	}

	void SendNoticeChanopsInternal(BotInfo *source, const char *dest, const char *buf)
	{
		if (!buf) return;
		send_cmd(NULL, "NOTICE @%s :%s", dest, buf);
	}

	void SendBotOp(const char *nick, const char *chan)
	{
		if (UseTS6) {
			BotInfo *bi = findbot(nick);
			ratbox_cmd_tmode(nick, chan, "%s %s", ircd->botchanumode, bi ? bi->uid.c_str() : nick);
		}
		else SendMode(findbot(nick), chan, "%s %s", ircd->botchanumode, nick);
	}

	/* QUIT */
	void SendQuitInternal(BotInfo *bi, const char *buf)
	{
		if (buf) send_cmd(UseTS6 ? bi->uid : bi->nick, "QUIT :%s", buf);
		else send_cmd(UseTS6 ? bi->uid : bi->nick, "QUIT");
	}

	/* PONG */
	void SendPong(const char *servname, const char *who)
	{
		if (UseTS6) send_cmd(TS6SID, "PONG %s", who);
		else send_cmd(servname, "PONG %s", who);
	}

	/* INVITE */
	void SendInvite(BotInfo *source, const char *chan, const char *nick)
	{
		User *u = finduser(nick);
		send_cmd(UseTS6 ? source->uid : source->nick, "INVITE %s %s", UseTS6 ? (u ? u->uid : nick) : nick, chan);
	}

	/*
	  1 = valid nick
	  0 = nick is in valid
	*/
	int IsNickValid(const char *nick)
	{
		/* TS6 Save extension -Certus */
		if (isdigit(*nick)) return 0;
		return 1;
	}

	void SendTopic(BotInfo *bi, const char *chan, const char *whosetit, const char *topic, time_t when)
	{
		send_cmd(UseTS6 ? bi->uid : bi->nick, "TOPIC %s :%s", chan, topic);
	}
} ircd_proto;




int anope_event_sjoin(const char *source, int ac, const char **av)
{
    do_sjoin(source, ac, av);
    return MOD_CONT;
}

/*
   Non TS6

   av[0] = nick
   av[1] = hop
   av[2] = ts
   av[3] = modes
   av[4] = user
   av[5] = host
   av[6] = server
   av[7] = info

   TS6
   av[0] = nick
   av[1] = hop
   av[2] = ts
   av[3] = modes
   av[4] = user
   av[5] = host
   av[6] = IP
   av[7] = UID
   av[8] = info

*/
int anope_event_nick(const char *source, int ac, const char **av)
{
    Server *s;
    User *user;

    if (UseTS6 && ac == 9) {
        s = findserver_uid(servlist, source);
        /* Source is always the server */
        user = do_nick("", av[0], av[4], av[5], s->name, av[8],
                       strtoul(av[2], NULL, 10), 0, 0, "*", av[7]);
        if (user) {
            ircdproto->ProcessUsermodes(user, 1, &av[3]);
        }
    } else {
        if (ac != 2) {
            user = do_nick(source, av[0], av[4], av[5], av[6], av[7],
                           strtoul(av[2], NULL, 10), 0, 0, "*", NULL);
            if (user)
                ircdproto->ProcessUsermodes(user, 1, &av[3]);
        } else {
            do_nick(source, av[0], NULL, NULL, NULL, NULL,
                    strtoul(av[1], NULL, 10), 0, 0, NULL, NULL);
        }
    }
    return MOD_CONT;
}

int anope_event_topic(const char *source, int ac, const char **av)
{
    User *u;

    if (ac == 4) {
        do_topic(source, ac, av);
    } else {
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

        if (UseTS6) {
            u = find_byuid(source);
            if (u) {
                strscpy(c->topic_setter, u->nick, sizeof(c->topic_setter));
            } else {
                strscpy(c->topic_setter, source, sizeof(c->topic_setter));
            }
        } else {
            strscpy(c->topic_setter, source, sizeof(c->topic_setter));
        }
        c->topic_time = topic_time;

        record_topic(av[0]);

		if (ac > 1 && *av[1])
		    send_event(EVENT_TOPIC_UPDATED, 2, av[0], av[1]);
		else
		    send_event(EVENT_TOPIC_UPDATED, 2, av[0], "");
    }
    return MOD_CONT;
}

int anope_event_tburst(const char *source, int ac, const char **av)
{
    char *setter;
    Channel *c;
    time_t topic_time;

    if (ac != 4) {
        return MOD_CONT;
    }

    setter = myStrGetToken(av[2], '!', 0);

    c = findchan(av[0]);
    topic_time = strtol(av[1], NULL, 10);

    if (!c) {
        if (debug) {
            alog("debug: TOPIC %s for nonexistent channel %s",
                 merge_args(ac - 1, av + 1), av[0]);
        }
        if (setter)
            free(setter);
        return MOD_CONT;
    }

    if (check_topiclock(c, topic_time)) {
        if (setter)
            free(setter);
        return MOD_CONT;
    }

    if (c->topic) {
        free(c->topic);
        c->topic = NULL;
    }
    if (ac > 1 && *av[3])
        c->topic = sstrdup(av[3]);

    strscpy(c->topic_setter, setter, sizeof(c->topic_setter));
    c->topic_time = topic_time;

    record_topic(av[0]);
    if (setter)
        free(setter);
    return MOD_CONT;
}

int anope_event_436(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;

    m_nickcoll(av[0]);
    return MOD_CONT;
}


int anope_event_ping(const char *source, int ac, const char **av)
{
    if (ac < 1)
        return MOD_CONT;
    ircdproto->SendPong(ac > 1 ? av[1] : ServerName, av[0]);
    return MOD_CONT;
}

int anope_event_away(const char *source, int ac, const char **av)
{
    User *u = NULL;

    if (UseTS6) {
        u = find_byuid(source);
    }

    m_away((UseTS6 ? (u ? u->nick : source) : source),
           (ac ? av[0] : NULL));
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
    if (ac != 1) {
        do_sjoin(source, ac, av);
        return MOD_CONT;
    } else {
        do_join(source, ac, av);
    }
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

int anope_event_privmsg(const char *source, int ac, const char **av)
{
    User *u;
	BotInfo *bi;

    if (ac != 2) {
        return MOD_CONT;
    }

    u = find_byuid(source);
    bi = findbot(av[0]);
	// XXX: this is really the same as charybdis
    m_privmsg(source, av[0], av[1]);
    return MOD_CONT;
}

int anope_event_part(const char *source, int ac, const char **av)
{
    User *u;

    if (ac < 1 || ac > 2) {
        return MOD_CONT;
    }

    u = find_byuid(source);
    do_part((UseTS6 ? (u ? u->nick : source) : source), ac, av);

    return MOD_CONT;
}

int anope_event_whois(const char *source, int ac, const char **av)
{
    BotInfo *bi;

    if (source && ac >= 1) {
        bi = findbot(av[0]);
        m_whois(source, (UseTS6 ? bi->uid.c_str() : bi->nick));
    }
    return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(const char *source, int ac, const char **av)
{
    if (!stricmp(av[1], "1")) {
        uplink = sstrdup(av[0]);
        if (UseTS6 && TS6UPLINK) {
            do_server(source, av[0], av[1], av[2], TS6UPLINK);
        } else {
            do_server(source, av[0], av[1], av[2], NULL);
        }
    } else {
        do_server(source, av[0], av[1], av[2], NULL);
    }
    return MOD_CONT;
}

int anope_event_sid(const char *source, int ac, const char **av)
{
    Server *s;

    /* :42X SID trystan.nomadirc.net 2 43X :ircd-ratbox test server */

    s = findserver_uid(servlist, source);

    do_server(s->name, av[0], av[1], av[3], av[2]);
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
    User *u;

    if (ac != 1) {
        return MOD_CONT;
    }

    u = find_byuid(source);

    do_quit((UseTS6 ? (u ? u->nick : source) : source), ac, av);
    return MOD_CONT;
}

int anope_event_mode(const char *source, int ac, const char **av)
{
    User *u, *u2;

    if (ac < 2) {
        return MOD_CONT;
    }

    if (*av[0] == '#' || *av[0] == '&') {
        do_cmode(source, ac, av);
    } else {
        if (UseTS6) {
            u = find_byuid(source);
            u2 = find_byuid(av[0]);
            av[0] = u2->nick;
            do_umode(u->nick, ac, av);
        } else {
            do_umode(source, ac, av);
        }
    }
    return MOD_CONT;
}

int anope_event_tmode(const char *source, int ac, const char **av)
{
    if (*av[1] == '#' || *av[1] == '&') {
        do_cmode(source, ac, av);
    }
    return MOD_CONT;
}

/* Event: PROTOCTL */
int anope_event_capab(const char *source, int ac, const char **av)
{
    int argvsize = 8;
    int argc;
    const char **argv;
    char *str;

    if (ac < 1)
        return MOD_CONT;

    /* We get the params as one arg, we should split it for capab_parse */
    argv = (const char **)scalloc(argvsize, sizeof(const char *));
    argc = 0;
    while ((str = myStrGetToken(av[0], ' ', argc))) {
        if (argc == argvsize) {
            argvsize += 8;
            argv = (const char **)srealloc(argv, argvsize * sizeof(const char *));
        }
        argv[argc] = str;
        argc++;
    }

    capab_parse(argc, argv);

    /* Free our built ac/av */
    for (argvsize = 0; argvsize < argc; argvsize++) {
        free((char *)argv[argvsize]);
    }
    free((char **)argv);

    return MOD_CONT;
}

int anope_event_pass(const char *source, int ac, const char **av)
{
    if (UseTS6) {
        TS6UPLINK = sstrdup(av[3]);
    }
    return MOD_CONT;
}

int anope_event_bmask(const char *source, int ac, const char **av)
{
    Channel *c;
    char *bans;
    char *b;
    int count, i;

    /* :42X BMASK 1106409026 #ircops b :*!*@*.aol.com */
    /*             0          1      2   3            */
    c = findchan(av[1]);

    if (c) {
        bans = sstrdup(av[3]);
        count = myNumToken(bans, ' ');
        for (i = 0; i <= count - 1; i++) {
            b = myStrGetToken(bans, ' ', i);
            if (!stricmp(av[2], "b")) {
                add_ban(c, b);
            }
            if (!stricmp(av[2], "e")) {
                add_exception(c, b);
            }
            if (!stricmp(av[2], "I")) {
                add_invite(c, b);
            }
            if (b)
                free(b);
        }
        free(bans);
    }
    return MOD_CONT;
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



/* *INDENT-OFF* */
void moduleAddIRCDMsgs(void)
{
    Message *m;

    updateProtectDetails("PROTECT","PROTECTME","protect","deprotect","AUTOPROTECT","+","-");

    if (UseTS6) {
        TS6SID = sstrdup(Numeric);
        UseTSMODE = 1;  /* TMODE */
    }

    m = createMessage("436",       anope_event_436); addCoreMessage(IRCD,m);
    m = createMessage("AWAY",      anope_event_away); addCoreMessage(IRCD,m);
    m = createMessage("JOIN",      anope_event_join); addCoreMessage(IRCD,m);
    m = createMessage("KICK",      anope_event_kick); addCoreMessage(IRCD,m);
    m = createMessage("KILL",      anope_event_kill); addCoreMessage(IRCD,m);
    m = createMessage("MODE",      anope_event_mode); addCoreMessage(IRCD,m);
    m = createMessage("TMODE",     anope_event_tmode); addCoreMessage(IRCD,m);
    m = createMessage("MOTD",      anope_event_motd); addCoreMessage(IRCD,m);
    m = createMessage("NICK",      anope_event_nick); addCoreMessage(IRCD,m);
    m = createMessage("BMASK",     anope_event_bmask); addCoreMessage(IRCD,m);
    m = createMessage("UID",       anope_event_nick); addCoreMessage(IRCD,m);
    m = createMessage("PART",      anope_event_part); addCoreMessage(IRCD,m);
    m = createMessage("PASS",      anope_event_pass); addCoreMessage(IRCD,m);
    m = createMessage("PING",      anope_event_ping); addCoreMessage(IRCD,m);
    m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
    m = createMessage("QUIT",      anope_event_quit); addCoreMessage(IRCD,m);
    m = createMessage("SERVER",    anope_event_server); addCoreMessage(IRCD,m);
    m = createMessage("SQUIT",     anope_event_squit); addCoreMessage(IRCD,m);
    m = createMessage("TOPIC",     anope_event_topic); addCoreMessage(IRCD,m);
    m = createMessage("TB",        anope_event_tburst); addCoreMessage(IRCD,m);
    m = createMessage("WHOIS",     anope_event_whois); addCoreMessage(IRCD,m);
    m = createMessage("CAPAB",     anope_event_capab); addCoreMessage(IRCD,m);
    m = createMessage("SJOIN",     anope_event_sjoin); addCoreMessage(IRCD,m);
    m = createMessage("ERROR",     anope_event_error); addCoreMessage(IRCD,m);
    m = createMessage("SID",       anope_event_sid); addCoreMessage(IRCD,m);
}

/* *INDENT-ON* */

/**
 * Now tell anope how to use us.
 **/
int AnopeInit(int argc, char **argv)
{

    moduleAddAuthor("Anope");
    moduleAddVersion("$Id$");
    moduleSetType(PROTOCOL);

    pmodule_ircd_version("Ratbox IRCD 2.0+");
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

	pmodule_ircd_proto(&ircd_proto);
    moduleAddIRCDMsgs();

    return MOD_CONT;
}

MODULE_INIT("ratbox")
