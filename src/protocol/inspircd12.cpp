/* inspircd 1.2 functions
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

#define UMODE_a 0x00000001
#define UMODE_h 0x00000002
#define UMODE_i 0x00000004
#define UMODE_o 0x00000008
#define UMODE_r 0x00000010
#define UMODE_w 0x00000020
#define UMODE_A 0x00000040
#define UMODE_g 0x80000000
#define UMODE_x 0x40000000

#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040		/* These two used only by ChanServ */
#define CMODE_l 0x00000080
#define CMODE_c 0x00000400
#define CMODE_A 0x00000800
#define CMODE_H 0x00001000
#define CMODE_K 0x00002000
#define CMODE_L 0x00004000
#define CMODE_O 0x00008000
#define CMODE_Q 0x00010000
#define CMODE_S 0x00020000
#define CMODE_V 0x00040000
#define CMODE_f 0x00080000
#define CMODE_G 0x00100000
#define CMODE_C 0x00200000
#define CMODE_u 0x00400000
#define CMODE_z 0x00800000
#define CMODE_N 0x01000000
#define CMODE_R 0x00000100		/* Only identified users can join */
#define CMODE_r 0x00000200		/* Set for all registered channels */

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

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
	{"InspIRCd 1.2",			/* ircd name */
	 "+I",					  /* Modes used by pseudoclients */
	 5,						 /* Chan Max Symbols	 */
	 "-cilmnpstuzACGHKNOQRSV",  /* Modes to Remove */
	 "+ao",					 /* Channel Umode used by Botserv bots */
	 1,						 /* SVSNICK */
	 1,						 /* Vhost  */
	 1,						 /* Has Owner */
	 "+q",					  /* Mode to set for an owner */
	 "-q",					  /* Mode to unset for an owner */
	 "+a",					  /* Mode to set for channel admin */
	 "-a",					  /* Mode to unset for channel admin */
	 "+r",					  /* Mode On Reg		  */
	 NULL,					  /* Mode on ID for Roots */
	 NULL,					  /* Mode on ID for Admins */
	 NULL,					  /* Mode on ID for Opers */
	 "-r",					  /* Mode on UnReg		*/
	 "-r",					  /* Mode on Nick Change  */
	 1,						 /* Supports SGlines	 */
	 1,						 /* Supports SQlines	 */
	 1,						 /* Supports SZlines	 */
	 1,						 /* Supports Halfop +h   */
	 4,						 /* Number of server args */
	 0,						 /* Join 2 Set		   */
	 1,						 /* Join 2 Message	   */
	 0,						 /* Has exceptions +e	*/
	 1,						 /* TS Topic Forward	 */
	 0,						 /* TS Topci Backward	*/
	 0,						 /* Protected Umode	  */
	 0,						 /* Has Admin			*/
	 0,						 /* Chan SQlines		 */
	 0,						 /* Quit on Kill		 */
	 0,						 /* SVSMODE unban		*/
	 1,						 /* Has Protect		  */
	 1,						 /* Reverse			  */
	 1,						 /* Chan Reg			 */
	 CMODE_r,				   /* Channel Mode		 */
	 1,						 /* vidents			  */
	 1,						 /* svshold			  */
	 0,						 /* time stamp on mode   */
	 0,						 /* NICKIP			   */
	 1,						 /* O:LINE			   */
	 1,						 /* UMODE			   */
	 1,						 /* VHOST ON NICK		*/
	 0,						 /* Change RealName	  */
	 CMODE_K,				   /* No Knock			 */
	 0,						 /* Admin Only		   */
	 DEFAULT_MLOCK,			 /* Default MLOCK	   */
	 UMODE_x,				   /* Vhost Mode		   */
	 0,						 /* +f				   */
	 1,						 /* +L				   */
	 CMODE_f,
	 CMODE_L,
	 0,
	 1,						 /* No Knock requires +i */
	 NULL,					  /* CAPAB Chan Modes			 */
	 0,						 /* We support inspircd TOKENS */
	 0,						 /* TIME STAMPS are BASE64 */
	 0,						 /* +I support */
	 0,						 /* SJOIN ban char */
	 0,						 /* SJOIN except char */
	 0,						 /* SJOIN invite char */
	 0,						 /* Can remove User Channel Modes with SVSMODE */
	 0,						 /* Sglines are not enforced until user reconnects */
	 "x",					   /* vhost char */
	 1,						 /* ts6 */
	 1,						 /* support helper umode */
	 0,						 /* p10 */
	 NULL,					  /* character set */
	 0,						 /* reports sync state */
	 1,						 /* CIDR channelbans */
	 "$",					   /* TLD Prefix for Global */
	 }
	,
	{NULL}
};


IRCDCAPAB myIrcdcap[] = {
	{
	 CAPAB_NOQUIT,			  /* NOQUIT	   */
	 0,						 /* TSMODE	   */
	 1,						 /* UNCONNECT	*/
	 0,						 /* NICKIP	   */
	 0,						 /* SJOIN		*/
	 0,						 /* ZIP		  */
	 0,						 /* BURST		*/
	 0,						 /* TS5		  */
	 0,						 /* TS3		  */
	 0,						 /* DKEY		 */
	 0,						 /* PT4		  */
	 0,						 /* SCS		  */
	 0,						 /* QS		   */
	 0,						 /* UID		  */
	 0,						 /* KNOCK		*/
	 0,						 /* CLIENT	   */
	 0,						 /* IPV6		 */
	 0,						 /* SSJ5		 */
	 0,						 /* SN2		  */
	 0,						 /* TOKEN		*/
	 0,						 /* VHOST		*/
	 CAPAB_SSJ3,				/* SSJ3		 */
	 CAPAB_NICK2,			   /* NICK2		*/
	 0,						 /* UMODE2	   */
	 CAPAB_VL,				  /* VL		   */
	 CAPAB_TLKEXT,			  /* TLKEXT	   */
	 0,						 /* DODKEY	   */
	 0,						 /* DOZIP		*/
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
	'h',						/* (37) % Channel halfops */
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
	{0},						/* B */
	{CMODE_C, 0, NULL, NULL},   /* C */
	{0},						/* D */
	{0},						/* E */
	{0},						/* F */
	{CMODE_G, 0, NULL, NULL},   /* G */
	{CMODE_H, CBM_NO_USER_MLOCK, NULL, NULL},
	{0},						/* I */
	{0},						/* J */
	{CMODE_K, 0, NULL, NULL},   /* K */
	{CMODE_L, 0, set_redirect, cs_set_redirect},
	{0},						/* M */
	{CMODE_N, 0, NULL, NULL},   /* N */
	{CMODE_O, CBM_NO_USER_MLOCK, NULL, NULL},
	{0},						/* P */
	{CMODE_Q, 0, NULL, NULL},   /* Q */
	{CMODE_R, 0, NULL, NULL},   /* R */
	{CMODE_S, 0, NULL, NULL},   /* S */
	{0},						/* T */
	{0},						/* U */
	{CMODE_V, 0, NULL, NULL},   /* V */
	{0},						/* W */
	{0},						/* X */
	{0},						/* Y */
	{0},						/* Z */
	{0}, {0}, {0}, {0}, {0}, {0},
	{0},						/* a */
	{0},						/* b */
	{CMODE_c, 0, NULL, NULL},
	{0},						/* d */
	{0},						/* e */
	{0},						/* f */
	{0},						/* g */
	{0},						/* h */
	{CMODE_i, 0, NULL, NULL},
	{0},						/* j */
	{CMODE_k, 0, chan_set_key, cs_set_key},
	{CMODE_l, CBM_MINUS_NO_ARG, set_limit, cs_set_limit},
	{CMODE_m, 0, NULL, NULL},
	{CMODE_n, 0, NULL, NULL},
	{0},						/* o */
	{CMODE_p, 0, NULL, NULL},
	{0},						/* q */
	{CMODE_r, CBM_NO_MLOCK, NULL, NULL},
	{CMODE_s, 0, NULL, NULL},
	{CMODE_t, 0, NULL, NULL},
	{CMODE_u, 0, NULL, NULL},
	{0},						/* v */
	{0},						/* w */
	{0},						/* x */
	{0},						/* y */
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
	{0},						/* b */
	{0},						/* c */
	{0},						/* d */
	{0},						/* e */
	{0},						/* f */
	{0},						/* g */
	{CUS_HALFOP, 0, check_valid_op},
	{0},						/* i */
	{0},						/* j */
	{0},						/* k */
	{0},						/* l */
	{0},						/* m */
	{0},						/* n */
	{CUS_OP, CUF_PROTECT_BOTSERV, check_valid_op},
	{0},						/* p */
	{CUS_OWNER, 0, check_valid_op},
	{0},						/* r */
	{0},						/* s */
	{0},						/* t */
	{0},						/* u */
	{CUS_VOICE, 0, NULL},
	{0},						/* w */
	{0},						/* x */
	{0},						/* y */
	{0},						/* z */
	{0}, {0}, {0}, {0}, {0}
};

static int has_servicesmod = 0;
static int has_globopsmod = 0;
static int has_svsholdmod = 0;
static int has_chghostmod = 0;
static int has_chgidentmod = 0;
static int has_messagefloodmod = 0;
static int has_banexceptionmod = 0;
static int has_inviteexceptionmod = 0;


/* CHGHOST */
void inspircd_cmd_chghost(const char *nick, const char *vhost)
{
	if (has_chghostmod != 1)
	{
		ircdproto->SendGlobops(s_OperServ, "CHGHOST not loaded!");
		return;
	}

	BotInfo *bi = findbot(s_OperServ);
	send_cmd(bi->uid, "CHGHOST %s %s", nick, vhost);
}

int anope_event_idle(const char *source, int ac, const char **av)
{
	BotInfo *bi = findbot(av[0]);
	if (!bi)
		return MOD_CONT;

	send_cmd(bi->uid, "IDLE %s %ld 0", source, (long int) time(NULL));
	return MOD_CONT;
}

static char currentpass[1024];

/* PASS */
void inspircd_cmd_pass(const char *pass)
{
	strncpy(currentpass, pass, 1024);
}


class InspIRCdProto : public IRCDProto
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
					break;
				case 'r':
					user->svid = add ? user->timestamp : 0;
					if (add && !nick_identified(user)) {
						common_svsmode(user, "-r", NULL);
						user->mode &= ~UMODE_r;
					}
					break;
				case 'x':
					if (add) user->chost = user->vhost;
					update_host(user);
			}
		}
	}

	/* *INDENT-ON* */

	void SendAkillDel(const char *user, const char *host)
	{
		BotInfo *bi = findbot(s_OperServ);
		send_cmd(bi->uid, "GLINE %s@%s", user, host);
	}

	void SendTopic(BotInfo *whosets, const char *chan, const char *whosetit, const char *topic, time_t when)
	{
		send_cmd(whosets->uid, "FTOPIC %s %lu %s :%s", chan, static_cast<unsigned long>(when), whosetit, topic);
	}

	void SendVhostDel(User *u)
	{
		inspircd_cmd_chghost(u->uid, (u->mode & umodes['x']) ? u->chost.c_str() : u->host);
	}

	void SendAkill(const char *user, const char *host, const char *who, time_t when, time_t expires, const char *reason)
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = expires - time(NULL);
		if (timeleft > 172800)
			timeleft = 172800;
		send_cmd(ServerName, "ADDLINE G %s@%s %s %ld %ld :%s", user, host, who, static_cast<long int>(when), static_cast<long int>(timeleft), reason);
	}

	void SendSVSKillInternal(const char *source, const char *user, const char *buf)
	{
		BotInfo *bi = findbot(source);
		User *u = finduser(user);
		send_cmd(bi ? bi->uid : TS6SID, "KILL %s :%s", u ? u->uid : user, buf);
	}

	void SendSVSMode(User *u, int ac, const char **av)
	{
		BotInfo *bi = findbot(s_NickServ);
		send_cmd(bi->uid, "MODE %s %s", u->uid, merge_args(ac, av));
	}

	void SendNumericInternal(const char *source, int numeric, const char *dest, const char *buf)
	{
		send_cmd(TS6SID, "PUSH %s ::%s %03d %s %s", dest, source, numeric, dest, buf);
	}

	void SendGuestNick(const char *nick, const char *user, const char *host, const char *real, const char *modes)
	{
		send_cmd(TS6SID, "UID %ld %s %s %s %s +%s 0.0.0.0 :%s", static_cast<long>(time(NULL)), nick, host, host, user, modes, real);
	}

	void SendModeInternal(BotInfo *source, const char *dest, const char *buf)
	{
		Channel *c = findchan(dest);
		send_cmd(source ? source->uid : TS6SID, "FMODE %s %u %s", dest, static_cast<unsigned>(c ? c->creation_time : time(NULL)), buf);
	}

	void SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes, const char *uid)
	{
		send_cmd(ServerName, "UID %s %ld %s %s %s %s 0.0.0.0 %ld +%s :%s", uid, static_cast<long>(time(NULL)), nick, host, host, user, static_cast<long>(time(NULL)), modes, real);
	}

	void SendKickInternal(BotInfo *source, const char *chan, const char *user, const char *buf)
	{
		User *u = finduser(user);
		if (buf)
			send_cmd(source->uid, "KICK %s %s :%s", chan, u->uid, buf);
		else
			send_cmd(source->uid, "KICK %s %s :%s", chan, u->uid, user);
	}

	void SendNoticeChanopsInternal(BotInfo *source, const char *dest, const char *buf)
	{
		send_cmd(TS6SID, "NOTICE @%s :%s", dest, buf);
	}

	void SendBotOp(const char *nick, const char *chan)
	{
		SendMode(findbot(nick), chan, "%s %s %s", ircd->botchanumode, nick, nick);
	}

	/* SERVER services-dev.chatspike.net password 0 :Description here */
	void SendServer(const char *servname, int hop, const char *descript)
	{
		send_cmd(NULL, "SERVER %s %s %d %s :%s", servname, currentpass, hop, TS6SID, descript);
	}

	/* JOIN */
	void SendJoin(BotInfo *user, const char *channel, time_t chantime)
	{
		send_cmd(NULL, "FJOIN %s %ld + :,%s", channel, static_cast<long>(chantime), user->uid.c_str());
	}

	/* UNSQLINE */
	void SendSQLineDel(const char *user)
	{
		send_cmd(TS6SID, "DELLINE Q %s", user);
	}

	/* SQLINE */
	void SendSQLine(const char *mask, const char *reason)
	{
		send_cmd(TS6SID, "ADDLINE Q %s %s %ld 0 :%s", mask, s_OperServ, static_cast<long>(time(NULL)), reason);
	}

	/* SQUIT */
	void SendSquit(const char *servname, const char *message)
	{
		send_cmd(TS6SID, "SQUIT %s :%s", servname, message);
	}

	/* Functions that use serval cmd functions */

	void SendVhost(const char *nick, const char *vIdent, const char *vhost)
	{
		if (vIdent)
			inspircd_cmd_chgident(nick, vIdent);
		inspircd_cmd_chghost(nick, vhost);
	}

	void SendConnect()
	{
		inspircd_cmd_pass(uplink_server->password);
		SendServer(ServerName, 0, ServerDesc);
		send_cmd(NULL, "BURST");
		send_cmd(TS6SID, "VERSION :Anope-%s %s :%s - %s (%s) -- %s", version_number, ServerName, ircd->name, version_flags, EncModule, version_build);
		me_server = new_server(NULL, ServerName, ServerDesc, SERVER_ISME, TS6SID);
	}

	/* CHGIDENT */
	void inspircd_cmd_chgident(const char *nick, const char *vIdent)
	{
		if (has_chgidentmod == 0)
		{
			ircdproto->SendGlobops(s_OperServ, "CHGIDENT not loaded!");
		}
		else
		{
			BotInfo *bi = findbot(s_OperServ);
			send_cmd(bi->uid, "CHGIDENT %s %s", nick, vIdent);
		}
	}

	/* SVSHOLD - set */
	void SendSVSHold(const char *nick)
	{
		BotInfo *bi = findbot(s_OperServ);
		send_cmd(bi->uid, "SVSHOLD %s %ud :%s", nick, (unsigned int)NSReleaseTimeout, "Being held for registered user");
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const char *nick)
	{
		BotInfo *bi = findbot(s_OperServ);
		send_cmd(bi->uid, "SVSHOLD %s", nick);
	}

	/* UNSZLINE */
	void SendSZLineDel(const char *mask)
	{
		send_cmd(TS6SID, "DELLINE Z %s", mask);
	}

	/* SZLINE */
	void SendSZLine(const char *mask, const char *reason, const char *whom)
	{
		send_cmd(TS6SID, "ADDLINE Z %s %s %ld 0 :%s", mask, whom, static_cast<long>(time(NULL)), reason);
	}

	/* SVSMODE +d */
	/* nc_change was = 1, and there is no na->status */
	void SendUnregisteredNick(User *u)
	{
		common_svsmode(u, "-r", NULL);
	}

	/* SVSMODE +r */
	void SendSVID2(User *u, const char *ts)
	{
		common_svsmode(u, "+r", NULL);
	}

	void SendSVSJoin(const char *source, const char *nick, const char *chan, const char *param)
	{
		User *u = finduser(nick);
		BotInfo *bi = findbot(source);
		send_cmd(bi->uid, "SVSJOIN %s %s", u->uid, chan);
	}

	void SendSVSPart(const char *source, const char *nick, const char *chan)
	{
		User *u = finduser(nick);
		BotInfo *bi = findbot(source);
		send_cmd(bi->uid, "SVSPART %s %s", u->uid, chan);
	}

	void SendEOB()
	{
		send_cmd(TS6SID, "ENDBURST");
	}

	int IsFloodModeParamValid(const char *value)
	{
		char *dp, *end;
		if (value && *value != ':' && strtoul((*value == '*' ? value + 1 : value), &dp, 10) > 0 && *dp == ':' && *(++dp) && strtoul(dp, &end, 10) > 0 && !*end) return 1;
		else return 0;
	}

	void SendGlobopsInternal(const char *source, const char *buf)
	{
		BotInfo *bi = findbot(source);
		if (bi)
		{
			if (has_globopsmod)
				send_cmd(ircd->ts6 ? bi->uid : bi->nick, "SNONOTICE g :%s", buf);
			else
				send_cmd(ircd->ts6 ? bi->uid : bi->nick, "SNONOTICE A :%s", buf);
		}
	}
	
	int IsNickValid(const char *nick)
	{
		/* InspIRCd, like TS6, uses UIDs on collision, so... */
		if (isdigit(*nick))
			return 0;
		return 1;
	}
} ircd_proto;






int anope_event_ftopic(const char *source, int ac, const char **av)
{
	/* :source FTOPIC channel ts setby :topic */
	const char *temp;
	if (ac < 4)
		return MOD_CONT;
	temp = av[1];			   /* temp now holds ts */
	av[1] = av[2];			  /* av[1] now holds set by */
	av[2] = temp;			   /* av[2] now holds ts */
	do_topic(source, ac, av);
	return MOD_CONT;
}

int anope_event_mode(const char *source, int ac, const char **av)
{
	if (*av[0] == '#' || *av[0] == '&')
	{
		do_cmode(source, ac, av);
	}
	else
	{
		/* InspIRCd lets opers change another
		   users modes, we have to kludge this
		   as it slightly breaks RFC1459
		 */
		User *u = find_byuid(source);
		User *u2 = find_byuid(av[0]);

		// This can happen with server-origin modes.
		if (u == NULL)
			u = u2;

		// drop it like fire.
		// most likely situation was
		if (u == NULL || u2 == NULL)
			return MOD_CONT;

		av[0] = u2->nick;
		do_umode(u->nick, ac, av);
	}
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

/*
 * [Nov 03 22:31:57.695076 2008] debug: Received: :964 FJOIN #test 1223763723 +BPSnt :,964AAAAAB ,964AAAAAC ,966AAAAAA
 *
 * 0: name
 * 1: channel ts (when it was created, see protocol docs for more info)
 * 2: channel modes + params (NOTEL this may definitely be more than one param!)
 * last: users
 */
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

	if (ac <= 4)
		return MOD_CONT;

	curnick = myStrGetToken(av[ac - 1], ' ', curtoken);
	while (curnick != NULL)
	{
		for (; *curnick; curnick++)
		{
			/* XXX: bleagh! -- w00t */
			switch (*curnick)
			{
				case 'q':
					prefixandnick[nlen++] = '~';
					break;
				case 'a':
					prefixandnick[nlen++] = '&';
					break;
				case 'o':
					prefixandnick[nlen++] = '@';
					break;
				case 'h':
					prefixandnick[nlen++] = '%';
					break;
				case 'v':
					prefixandnick[nlen++] = '+';
					break;
				case ',':
					curnick++;
					strncpy(prefixandnick + nlen, curnick, sizeof(prefixandnick) - nlen);
					goto endnick;
					break;
				default:
					alog("fjoin: unrecognised prefix: %c", *curnick);
					break;
			}
		}

// Much as I hate goto.. I can't `break 2' to get here.. XXX ugly
endnick:
		strncat(nicklist, prefixandnick, 513);
		strncat(nicklist, " ", 513);
		curtoken++;
		curnick = myStrGetToken(av[ac - 1], ' ', curtoken);
		nlen = 0;
	}

	newav[0] = av[1];		   /* timestamp */
	newav[1] = av[0];		   /* channel name */
	newav[2] = av[2];			 /* channel modes */ // XXX: this is incorrect, it doesn't take into account +L etc, modes which require params.. call FMODE instead? -- w00t
	newav[3] = nicklist;
	do_sjoin(source, 4, newav);

	return MOD_CONT;
}

/* Events */
int anope_event_ping(const char *source, int ac, const char **av)
{
	ircdproto->SendPong(TS6SID, av[0]);
	return MOD_CONT;
}

int anope_event_436(const char *source, int ac, const char **av)
{
	m_nickcoll(av[0]);
	return MOD_CONT;
}

int anope_event_away(const char *source, int ac, const char **av)
{
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
	do_squit(source, ac, av);
	return MOD_CONT;
}

int anope_event_rsquit(const char *source, int ac, const char **av)
{
	do_squit(source, ac, av);
	return MOD_CONT;
}

int anope_event_quit(const char *source, int ac, const char **av)
{
	do_quit(source, ac, av);
	return MOD_CONT;
}


int anope_event_kill(const char *source, int ac, const char **av)
{
	m_kill(av[0], av[1]);
	return MOD_CONT;
}

int anope_event_kick(const char *source, int ac, const char **av)
{
	do_kick(source, ac, av);
	return MOD_CONT;
}


int anope_event_join(const char *source, int ac, const char **av)
{
	do_join(source, ac, av);
	return MOD_CONT;
}

int anope_event_motd(const char *source, int ac, const char **av)
{
	m_motd(source);
	return MOD_CONT;
}

int anope_event_setname(const char *source, int ac, const char **av)
{
	User *u;

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
	do_nick(source, av[0], NULL, NULL, NULL, NULL, 0, 0, 0, NULL, NULL);
	return MOD_CONT;
}


/*
 * [Nov 03 22:09:58.176252 2008] debug: Received: :964 UID 964AAAAAC 1225746297 w00t2 localhost testnet.user w00t 127.0.0.1 1225746302 +iosw +ACGJKLNOQcdfgjklnoqtx :Robin Burchell <w00t@inspircd.org>
 * 0: uid
 * 1: ts
 * 2: nick
 * 3: host
 * 4: dhost
 * 5: ident
 * 6: ip
 * 7: signon
 * 8+: modes and params -- IMPORTANT, some modes (e.g. +s) may have parameters. So don't assume a fixed position of realname!
 * last: realname
*/

int anope_event_uid(const char *source, int ac, const char **av)
{
	User *user;
	struct in_addr addy;
	Server *s = findserver_uid(servlist, source);
	uint32 *ad = (uint32 *) &addy;
	int svid = 0;
	int ts = strtoul(av[1], NULL, 10);

	if (strchr(av[8], 'r') != NULL)
		svid = ts;

	inet_aton(av[6], &addy);
	user = do_nick("", av[2],   /* nick */
			av[5],   /* username */
			av[3],   /* realhost */
			s->name,  /* server */
			av[ac - 1],   /* realname */
			ts, svid, htonl(*ad), av[4], av[0]);
	if (user)
	{
		ircdproto->ProcessUsermodes(user, 1, &av[8]);
		user->chost = av[4];
	}

	return MOD_CONT;
}

int anope_event_chghost(const char *source, int ac, const char **av)
{
	User *u;

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

/*
 * [Nov 04 00:08:46.308435 2008] debug: Received: SERVER irc.inspircd.com pass 0 964 :Testnet Central!
 * 0: name
 * 1: pass
 * 2: hops
 * 3: numeric
 * 4: desc
 */
int anope_event_server(const char *source, int ac, const char **av)
{
	if (!stricmp(av[2], "0"))
	{
		uplink = sstrdup(av[0]);
	}
	do_server(source, av[0], av[2], av[4], av[3]);
	return MOD_CONT;
}


int anope_event_privmsg(const char *source, int ac, const char **av)
{
	User *u = find_byuid(source);
	BotInfo *bi = findbot(av[0]);
	m_privmsg(u->nick, bi ? bi->nick: av[0], av[1]);
	return MOD_CONT;
}

int anope_event_part(const char *source, int ac, const char **av)
{
	do_part(source, ac, av);
	return MOD_CONT;
}

int anope_event_whois(const char *source, int ac, const char **av)
{
	m_whois(source, av[0]);
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
		if (strstr(av[1], "m_services_account.so")) {
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
		if (!has_servicesmod) {
			send_cmd(NULL, "ERROR :m_services_account.so is not loaded. This is required by Anope");
			quitmsg = "ERROR: Remote server does not have the m_services_account module loaded, and this is required.";
			quitting = 1;
			return MOD_STOP;
		}
		if (!has_svsholdmod) {
			ircdproto->SendGlobops(s_OperServ, "SVSHOLD missing, Usage disabled until module is loaded.");
		}
		if (!has_chghostmod) {
			ircdproto->SendGlobops(s_OperServ, "CHGHOST missing, Usage disabled until module is loaded.");
		}
		if (!has_chgidentmod) {
			ircdproto->SendGlobops(s_OperServ, "CHGIDENT missing, Usage disabled until module is loaded.");
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

void moduleAddIRCDMsgs(void) {
	Message *m;

	updateProtectDetails("PROTECT","PROTECTME","protect","deprotect","AUTOPROTECT","+a","-a");

	m = createMessage("436",	   anope_event_436); addCoreMessage(IRCD,m);
	m = createMessage("AWAY",	  anope_event_away); addCoreMessage(IRCD,m);
	m = createMessage("JOIN",	  anope_event_join); addCoreMessage(IRCD,m);
	m = createMessage("KICK",	  anope_event_kick); addCoreMessage(IRCD,m);
	m = createMessage("KILL",	  anope_event_kill); addCoreMessage(IRCD,m);
	m = createMessage("MODE",	  anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("MOTD",	  anope_event_motd); addCoreMessage(IRCD,m);
	m = createMessage("NICK",	  anope_event_nick); addCoreMessage(IRCD,m);
	m = createMessage("UID",	  anope_event_uid); addCoreMessage(IRCD,m);
	m = createMessage("CAPAB",	 anope_event_capab); addCoreMessage(IRCD,m);
	m = createMessage("PART",	  anope_event_part); addCoreMessage(IRCD,m);
	m = createMessage("PING",	  anope_event_ping); addCoreMessage(IRCD,m);
	m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
	m = createMessage("QUIT",	  anope_event_quit); addCoreMessage(IRCD,m);
	m = createMessage("SERVER",	anope_event_server); addCoreMessage(IRCD,m);
	m = createMessage("SQUIT",	 anope_event_squit); addCoreMessage(IRCD,m);
	m = createMessage("RSQUIT",	anope_event_rsquit); addCoreMessage(IRCD,m);
	m = createMessage("TOPIC",	 anope_event_topic); addCoreMessage(IRCD,m);
	m = createMessage("WHOIS",	 anope_event_whois); addCoreMessage(IRCD,m);
	m = createMessage("SVSMODE",   anope_event_mode) ;addCoreMessage(IRCD,m);
	m = createMessage("FHOST",	 anope_event_chghost); addCoreMessage(IRCD,m);
	m = createMessage("CHGIDENT",  anope_event_chgident); addCoreMessage(IRCD,m);
	m = createMessage("FNAME",	 anope_event_chgname); addCoreMessage(IRCD,m);
	m = createMessage("SETHOST",   anope_event_sethost); addCoreMessage(IRCD,m);
	m = createMessage("SETIDENT",  anope_event_setident); addCoreMessage(IRCD,m);
	m = createMessage("SETNAME",   anope_event_setname); addCoreMessage(IRCD,m);
	m = createMessage("FJOIN",	 anope_event_fjoin); addCoreMessage(IRCD,m);
	m = createMessage("FMODE",	 anope_event_fmode); addCoreMessage(IRCD,m);
	m = createMessage("FTOPIC",	anope_event_ftopic); addCoreMessage(IRCD,m);
	m = createMessage("OPERTYPE",  anope_event_opertype); addCoreMessage(IRCD,m);
	m = createMessage("IDLE",	  anope_event_idle); addCoreMessage(IRCD,m);
}

class ProtoInspIRCd : public Module
{
 public:
	ProtoInspIRCd(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(PROTOCOL);

		TS6SID = sstrdup(Numeric);

		pmodule_ircd_version("InspIRCd 1.2");
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

		pmodule_ircd_proto(&ircd_proto);
		moduleAddIRCDMsgs();
	}

	~ProtoInspIRCd()
	{
		free(TS6SID);
	}
};

MODULE_INIT("inspircd12", ProtoInspIRCd)
