/* inspircd 2.0 + functions
 *
 * (C) 2009  Jan Milants <Viper@Anope.org>
 * (C) 2011 Adam <Adam@anope.org>
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Partially based on code of Denora IRC Stats.
 * Based on InspIRCd 1.1 code of Anope by Anope Team.
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */


/*************************************************************************/

#include "services.h"
#include "pseudo.h"
#include "inspircd20.h"
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

static int16 get_new_statusmode()
{
	static int16 last_mode = CUS_PROTECT; /* highest value in services.h */
	if (last_mode == 0x8000)
		return 0;
	last_mode = last_mode << 1;
	if (last_mode == CUS_DEOPPED)
		last_mode = last_mode << 1; /* get around CUS_DEOPPED */
	return last_mode;
}

uint32 get_mode_from_char(const char c)
{
	struct chmodeinfo *p = chmodes;

	while (p->modechar != 0)
	{
		if (p->modechar == c)
			return p->mode;
		++p;
	}

	return 0;
}

static char flood_mode_set[3];
static char flood_mode_unset[3];

static void do_nothing(Channel *c, char *mask) { }

IRCDVar myIrcd[] = {
	{"InspIRCd 2.0",			/* ircd name */
	 "+I",					/* nickserv mode */
	 "+I",					/* chanserv mode */
	 "+I",					/* memoserv mode */
	 "+I",					/* hostserv mode */
	 "+ioI",				/* operserv mode */
	 "+I",					/* botserv mode  */
	 "+I",					/* helpserv mode */
	 "+iI",					/* Dev/Null mode */
	 "+iI",					/* Global mode   */
	 "+I",					/* nickserv alias mode */
	 "+I",					/* chanserv alias mode */
	 "+I",					/* memoserv alias mode */
	 "+I",					/* hostserv alias mode */
	 "+ioI",				/* operserv alias mode */
	 "+I",					/* botserv alias mode  */
	 "+I",					/* helpserv alias mode */
	 "+iI",					/* Dev/Null alias mode */
	 "+iI",					/* Global alias mode   */
	 "+I",					/* Used by BotServ Bots */
	 5,						 /* Chan Max Symbols	 */
	 "-cijlmnpstuzACGHKNOQRSV", /* Modes to Remove */
	 "+oa",					 /* Channel Umode used by Botserv bots */
	 1,						 /* SVSNICK */
	 1,						 /* Vhost  */
	 0,						 /* Has Owner */
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
	 0,						 /* Supports SGlines	 */
	 1,						 /* Supports SQlines	 */
	 1,						 /* Supports SZlines	 */
	 0,						 /* Supports Halfop +h   */
	 4,						 /* Number of server args */
	 0,						 /* Join 2 Set		   */
	 0,						 /* Join 2 Message	   */
	 0,						 /* Has exceptions +e	*/
	 1,						 /* TS Topic Forward	 */
	 0,						 /* TS Topic Backward	*/
	 0,						 /* Protected Umode	  */
	 0,						 /* Has Admin			*/
	 0,						 /* Chan SQlines		 */
	 0,						 /* Quit on Kill		 */
	 0,						 /* SVSMODE unban		*/
	 0,						 /* Has Protect		  */
	 1,						 /* Reverse			  */
	 1,						 /* Chan Reg			 */
	 CMODE_r,				   /* Channel Mode		 */
	 1,						 /* vidents			  */
	 0,						 /* svshold			  */
	 0,						 /* time stamp on mode   */
	 1,						 /* NICKIP			   */
	 0,						 /* O:LINE			   */
	 1,						 /* UMODE			   */
	 1,						 /* VHOST ON NICK		*/
	 0,						 /* Change RealName	  */
	 0,						 /* No Knock			 */
	 0,						 /* Admin Only		   */
	 DEFAULT_MLOCK,			 /* Default MLOCK	   */
	 UMODE_x,				   /* Vhost Mode		   */
	 0,						 /* +f				   */
	 0,						 /* +L				   */
	 CMODE_f,
	 CMODE_L,
	 0,
	 1,						 /* No Knock requires +i */
	 NULL,					  /* CAPAB Chan Modes			 */
	 0,						 /* We support inspircd TOKENS */
	 1,						 /* TOKENS are CASE inSensitive */
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
	 0,						 /* +j */
	 CMODE_j,				   /* +j Mode */
	 1,						 /* Use delayed client introduction. */
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
	0, 
	0, UMODE_B, 0, 0, 0,			   /* A - E */
	0, UMODE_G, UMODE_H, UMODE_I, 0,   /* F - J */
	0, 0, 0, 0, 0,					 /* K - O */
	0, UMODE_Q, UMODE_R, UMODE_S, 0,   /* P - T */
	0, 0, UMODE_W, 0, 0,			   /* U - Y */
	0,								 /* Z */
	0, 0, 0, 0, 0, 0,
	0, 0, UMODE_c, UMODE_d, 0,		 /* a - e */
	0, UMODE_g, UMODE_h, UMODE_i, 0,   /* f - j */
	UMODE_k, 0, 0, 0, UMODE_o,		 /* k - o */
	0, 0, UMODE_r, UMODE_s, 0,		 /* p - t */
	0, 0, UMODE_w, UMODE_x, 0,		 /* u - y */
	0,								 /* z */
	0, 0, 0, 0, 0
};


char myCsmodes[128] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0,
	0,
	0, 0, 0,
	0,
	0,
	0, 0, 0, 0,

	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static char sym_from_char(const char c)
{
	int i;
	for (i = 0; i < 128; ++i)
		if (myCsmodes[i] == c)
			return i;
	return 0;
}

CMMode myCmmodes[128] = {
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* 0-7 */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* 8-15 */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* 16-23 */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* 24-31 */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* 32-39 */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* 40-47 */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* 48-55 */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* 56-63 */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* 64-71 @-G */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* 72-79 H-O */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* 80-87 P-W */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* 88-95 X-_ */
	{NULL}, /* 96 */
	{NULL}, /* 97 a */
	{NULL}, /* 98 b */
	{NULL}, /* c */
	{NULL}, /* d */
	{NULL}, /* e */
	{NULL}, /* f */
	{NULL}, /* g */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* h-o */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, /* p-w */
	{NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL}, {NULL} /* z,y,z... */
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
	{0},						/* A */
	{0},						/* B */
	{0},						/* C */
	{0},						/* D */
	{0},						/* E */
	{0},						/* F */
	{0},						/* G */
	{0},						/* H */
	{0},						/* I */
	{0},						/* J */
	{0},						/* K */
	{0},						/* L */
	{CMODE_M, 0, NULL, NULL},   /* M */
	{0},						/* N */
	{0},						/* O */
	{0},						/* P */
	{0},						/* Q */
	{CMODE_R, 0, NULL, NULL},   /* R */
	{0},						/* S */
	{0},						/* T */
	{0},						/* U */
	{0},						/* V */
	{0},						/* W */
	{0},						/* X */
	{0},						/* Y */
	{0},						/* Z */
	{0}, {0}, {0}, {0}, {0}, {0},
	{0},						/* a */
	{0},						/* b */
	{0},						/* c */
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
	{0},
	{0},						/* v */
	{0},						/* w */
	{0},						/* x */
	{0},						/* y */
	{0},
	{0}, {0}, {0}, {0}
};

CBModeInfo myCbmodeinfos[] = {
	{'a', 0, 0, NULL, NULL},
	{'c', CMODE_c, 0, NULL, NULL},
	{'d', 0, 0, NULL, NULL},
	{'f', CMODE_f, 0, NULL, NULL},
	{'g', 0, 0, NULL, NULL},
	{'i', CMODE_i, 0, NULL, NULL},
	{'j', CMODE_j, 0, NULL, NULL},
	{'k', CMODE_k, 0, get_key, cs_get_key},
	{'l', CMODE_l, CBM_MINUS_NO_ARG, get_limit, cs_get_limit},
	{'m', CMODE_m, 0, NULL, NULL},
	{'n', CMODE_n, 0, NULL, NULL},
	{'p', CMODE_p, 0, NULL, NULL},
	{'r', CMODE_r, 0, NULL, NULL},
	{'s', CMODE_s, 0, NULL, NULL},
	{'t', CMODE_t, 0, NULL, NULL},
	{'u', CMODE_u, 0, NULL, NULL},
	{'v', 0, 0, NULL, NULL},
	{'w', 0, 0, NULL, NULL},
	{'x', 0, 0, NULL, NULL},
	{'y', 0, 0, NULL, NULL},
	{'z', CMODE_z, 0, NULL, NULL},
	{'A', CMODE_A, 0, NULL, NULL},
	{'B', 0, 0, NULL, NULL},
	{'C', CMODE_C, 0, NULL, NULL},
	{'D', 0, 0, NULL, NULL},
	{'E', 0, 0, NULL, NULL},
	{'F', CMODE_F, 0, NULL, NULL},
	{'G', CMODE_G, 0, NULL, NULL},
	{'H', 0, 0, NULL, NULL},
	{'J', CMODE_J, 0, NULL, NULL},
	{'K', CMODE_K, 0, NULL, NULL},
	{'L', CMODE_L, 0, NULL, NULL},
	{'M', CMODE_M, 0, NULL, NULL},
	{'N', CMODE_N, 0, NULL, NULL},
	{'O', CMODE_O, 0, NULL, NULL},
	{'P', CMODE_P, 0, NULL, NULL},
	{'Q', CMODE_Q, 0, NULL, NULL},
	{'R', CMODE_R, 0, NULL, NULL},
	{'S', CMODE_S, 0, NULL, NULL},
	{'T', CMODE_T, 0, NULL, NULL},
	{'U', 0, 0, NULL, NULL},
	{'V', 0, 0, NULL, NULL},
	{'W', 0, 0, NULL, NULL},
	{'X', 0, 0, NULL, NULL},
	{'Y', 0, 0, NULL, NULL},
	{'Z', 0, 0, NULL, NULL},
	{0}
};

static CBModeInfo *find_cbinfo(const char mode)
{
	CBModeInfo *p = myCbmodeinfos;

	while (p->flag != 0)
	{
		if (p->mode == mode)
		{
			return p;
		}
		++p;
	}

	return NULL;
}

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

	{0},						/* a */
	{0},						/* b */
	{0},						/* c */
	{0},						/* d */
	{0},						/* e */
	{0},						/* f */
	{0},						/* g */
	{0},						/* h */
	{0},						/* i */
	{0},						/* j */
	{0},						/* k */
	{0},						/* l */
	{0},						/* m */
	{0},						/* n */
	{CUS_OP, CUF_PROTECT_BOTSERV, check_valid_op},
	{0},						/* p */
	{0},						/* q */
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

/* This will be set to 1 if a burst is in progess. */
static int burst = 0;
/* Here we store for which user we last got a UID with +r. */
User *u_intro_regged = NULL;

static int has_servicesmod = 0;
static int has_chghostmod = 0;
static int has_chgidentmod = 0;
static int has_servprotectmod = 0;
static int has_hidechansmod = 0;

void inspircd_set_umode(User *user, int ac, char **av)
{
	int add = 1;				/* 1 if adding modes, 0 if deleting */
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
					anope_cmd_global(s_OperServ, "\2%s\2 is now an IRC operator.",
							user->nick);
				}
				display_news(user, NEWS_OPER);
			} else {
				opcnt--;
			}
			break;
		case 'r':
			user->svid = (add ? user->timestamp : 0);
			if (burst && user == u_intro_regged)
				break;
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

	m = createMessage("436",	   anope_event_436); addCoreMessage(IRCD,m);
	m = createMessage("AWAY",	  anope_event_away); addCoreMessage(IRCD,m);
	m = createMessage("INVITE",	anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("JOIN",	  anope_event_join); addCoreMessage(IRCD,m);
	m = createMessage("KICK",	  anope_event_kick); addCoreMessage(IRCD,m);
	m = createMessage("KILL",	  anope_event_kill); addCoreMessage(IRCD,m);
	m = createMessage("MODE",	  anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("MOTD",	  anope_event_motd); addCoreMessage(IRCD,m);
	m = createMessage("NICK",	  anope_event_nick); addCoreMessage(IRCD,m);
	m = createMessage("NOTICE",	anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("BURST",	 anope_event_burst); addCoreMessage(IRCD,m);
	m = createMessage("ENDBURST",  anope_event_eob); addCoreMessage(IRCD,m);
	m = createMessage("CAPAB",	 anope_event_capab); addCoreMessage(IRCD,m);
	m = createMessage("PART",	  anope_event_part); addCoreMessage(IRCD,m);
	m = createMessage("PING",	  anope_event_ping); addCoreMessage(IRCD,m);
	m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
	m = createMessage("QUIT",	  anope_event_quit); addCoreMessage(IRCD,m);
	m = createMessage("SERVER",	anope_event_server); addCoreMessage(IRCD,m);
	m = createMessage("SQUIT",	 anope_event_squit); addCoreMessage(IRCD,m);
	m = createMessage("RSQUIT",	anope_event_rsquit); addCoreMessage(IRCD,m);
	m = createMessage("TOPIC",	 anope_event_topic); addCoreMessage(IRCD,m);
	m = createMessage("WALLOPS",   anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("WHOIS",	 anope_event_whois); addCoreMessage(IRCD,m);
	m = createMessage("GLOBOPS",   anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("SILENCE",   anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("SVSMODE",   anope_event_mode) ;addCoreMessage(IRCD,m);
	m = createMessage("QLINE",	 anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("GLINE",	 anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("ELINE",	 anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("ZLINE",	 anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("ADDLINE",   anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("DELLINE",   anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("FHOST",	 anope_event_chghost); addCoreMessage(IRCD,m);
	m = createMessage("FIDENT",  anope_event_chgident); addCoreMessage(IRCD,m);
	m = createMessage("FNAME",	 anope_event_chgname); addCoreMessage(IRCD,m);
	m = createMessage("METADATA",  anope_event_metadata); addCoreMessage(IRCD,m);
	m = createMessage("SETHOST",   anope_event_sethost); addCoreMessage(IRCD,m);
	m = createMessage("SETIDENT",  anope_event_setident); addCoreMessage(IRCD,m);
	m = createMessage("SETNAME",   anope_event_setname); addCoreMessage(IRCD,m);
	m = createMessage("REHASH",	anope_event_rehash); addCoreMessage(IRCD,m);
	m = createMessage("ADMIN",	 anope_event_admin); addCoreMessage(IRCD,m);
	m = createMessage("CREDITS",   anope_event_credits); addCoreMessage(IRCD,m);
	m = createMessage("FJOIN",	 anope_event_fjoin); addCoreMessage(IRCD,m);
	m = createMessage("FMODE",	 anope_event_fmode); addCoreMessage(IRCD,m);
	m = createMessage("FTOPIC",	anope_event_ftopic); addCoreMessage(IRCD,m);
	m = createMessage("VERSION",   anope_event_version); addCoreMessage(IRCD,m);
	m = createMessage("OPERTYPE",  anope_event_opertype); addCoreMessage(IRCD,m);
	m = createMessage("IDLE",	  anope_event_idle); addCoreMessage(IRCD,m);
	m = createMessage("ENCAP",	 anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("UID",	   anope_event_uid); addCoreMessage(IRCD,m);
	m = createMessage("SVSWATCH",  anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("SVSSILENCE",anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("PUSH",	  anope_event_null); addCoreMessage(IRCD,m);
	m = createMessage("TIME",	  anope_event_time); addCoreMessage(IRCD,m);
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
	Uid *ud = find_uid(s_OperServ);
	send_cmd(ud->uid, "GLINE %s@%s", user, host);
}

void inspircd_cmd_topic(char *whosets, char *chan, char *whosetit,
		char *topic, time_t when)
{
	Uid *ud;
	if (!whosets) return;
	ud = find_uid(whosets);
	send_cmd(ud ? ud->uid : whosets, "FTOPIC %s %lu %s :%s", chan,
			 (unsigned long int) when, whosetit, topic);
}

void inspircd_cmd_vhost_off(User * u)
{
	common_svsmode(u, "-x", NULL);
	common_svsmode(u, "+x", NULL);

	if (has_chgidentmod && u->username && u->vident && strcmp(u->username, u->vident) != 0)
	{
		inspircd_cmd_chgident(u->nick, u->username);
	}
}

void inspircd_cmd_akill(char *user, char *host, char *who, time_t when,
		time_t expires, char *reason)
{
	/* Calculate the time left before this would expire, capping it at 2 days */
	time_t timeleft = expires - time(NULL);
	if (timeleft > 172800 || timeleft < 0)
		timeleft = 172800;
	send_cmd(TS6SID, "ADDLINE G %s@%s %s %ld %ld :%s", user, host, who,
			 (long int) time(NULL), (long int) timeleft, reason);
}

void inspircd_cmd_svskill(char *source, char *user, char *buf)
{
	Uid *ud1;
	User *u;

	if (!buf || !source || !user)
		return;

	ud1 = find_uid(source);
	u = finduser(user);
	send_cmd(ud1 ? ud1->uid : TS6SID, "KILL %s :%s", u ? u->uid : user, buf);
}

void inspircd_cmd_svsmode(User * u, int ac, char **av)
{
	Uid *ud = find_uid(s_NickServ);
	send_cmd(ud->uid, "MODE %s %s", u->nick, merge_args(ac, av));

	if (strstr(av[0], "+r") && u->na) {
		send_cmd(TS6SID, "METADATA %s accountname :%s", u->uid, u->na->nc->display);
	} else if (strstr(av[0], "-r")) {
		send_cmd(TS6SID, "METADATA %s accountname :", u->uid);
	}
}


void inspircd_cmd_372(char *source, char *msg)
{
	User *u = finduser(source);

	send_cmd(TS6SID, "PUSH %s ::%s 372 %s :- %s", u ? u->uid : source, ServerName, source, msg);
}

void inspircd_cmd_372_error(char *source)
{
	User *u = finduser(source);

	send_cmd(TS6SID, "PUSH %s ::%s 422 %s :- MOTD file not found!  Please "
			"contact your IRC administrator.", u ? u->uid : source, ServerName, source);
}

void inspircd_cmd_375(char *source)
{
	User *u = finduser(source);

	send_cmd(TS6SID, "PUSH %s ::%s 375 %s :- %s Message of the Day", u ? u->uid : source,
			ServerName, source, ServerName);
}

void inspircd_cmd_376(char *source)
{
	User *u = finduser(source);

	send_cmd(TS6SID, "PUSH %s ::%s 376 %s :End of /MOTD command.", u ? u->uid : source, ServerName, source);
}

void inspircd_cmd_nick(char *nick, char *name, char *modes)
{
	char *nicknumbuf = ts6_uid_retrieve();
	send_cmd(TS6SID, "KILL %s :Services enforced kill", nick);
	send_cmd(TS6SID, "UID %s %ld %s %s %s %s 0.0.0.0 %ld +%s :%s",
			 nicknumbuf, (long int) time(NULL), nick, ServiceHost,
			 ServiceHost, ServiceUser, (long int) time(NULL), modes, name);
	new_uid(nick, nicknumbuf);
	if (strchr(modes, 'o') != NULL)
		send_cmd(nicknumbuf, "OPERTYPE Service");
}

void inspircd_cmd_guest_nick(char *nick, char *user, char *host,
						char *real, char *modes)
{
	char *nicknumbuf = ts6_uid_retrieve();
	send_cmd(TS6SID, "UID %s %ld %s %s %s %s 0.0.0.0 %ld +%s :%s", nicknumbuf,
			 (long int) time(NULL), nick, host, host, user, (long int) time(NULL), modes, real);
	new_uid(nick, nicknumbuf);
}

void inspircd_cmd_mode(char *source, char *dest, char *buf)
{
	Channel *c;
	Uid *ud = NULL;

	if (!buf)
		return;

	c = findchan(dest);
	if (source) ud = find_uid(source);
	send_cmd(ud ? ud->uid : TS6SID, "FMODE %s %u %s", dest, (unsigned int)((c) ? c->creation_time : time(NULL)), buf);
}

int anope_event_version(char *source, int ac, char **av)
{
	return MOD_CONT;
}

int anope_event_idle(char *source, int ac, char **av)
{
	Uid *ud;
	BotInfo *bi;
	if (ac == 1) {
		ud = find_nickuid(av[0]);
		if (ud) {
			bi = findbot(ud->nick);
			send_cmd(ud->uid, "IDLE %s %ld %ld", source, start_time, bi ? time(NULL) - bi->lastmsg : 0);
		}
	}
	return MOD_CONT;
}

int anope_event_ftopic(char *source, int ac, char **av)
{
	/* :source FTOPIC channel ts setby :topic */
	char *temp;
	if (ac < 4)
		return MOD_CONT;
	temp = av[1];			   /* temp now holds ts */
	av[1] = av[2];			  /* av[1] now holds set by */
	av[2] = temp;			   /* av[2] now holds ts */
	do_topic(source, ac, av);
	return MOD_CONT;
}

int anope_event_opertype(char *source, int ac, char **av)
{
	/* opertype is equivalent to mode +o because servers
	   dont do this directly */
	User *u;
	u = find_byuid(source);
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

/*
 * [Nov 03 22:31:57.695076 2009] debug: Received: :964 FJOIN #test 1223763723 +BPSnt :,964AAAAAB ,964AAAAAC ,966AAAAAA
 *
 * 0: name
 * 1: channel ts (when it was created, see protocol docs for more info)
 * 2: channel modes + params (NOTE: this may definitely be more than one param!)
 * last: users
 */
int anope_event_fjoin(char *source, int ac, char **av)
{
	char *newav[64];
	char nicklist[4096] = ""; /* inspircd thing */

	char **argv;
	int i, argc;

	if (ac <= 3)
		return MOD_CONT;
	
	argc = split_buf(av[ac - 1], &argv, 1);
	for (i = 0; i < argc; ++i)
	{
		char *nick = argv[i];

		while (*nick != ',')
		{
			nicklist[strlen(nicklist)] = sym_from_char(*nick);
			nick++;
		}
		nick++;

		strlcat(nicklist, nick, sizeof(nicklist));
		strlcat(nicklist, " ", sizeof(nicklist));
	}

	newav[0] = av[1];		   /* timestamp */
	newav[1] = av[0];		   /* channel name */
	for (i = 2; i < ac - 1; i++)
		newav[i] = av[i];	   /* Modes */
	newav[i] = nicklist;		/* Nicknames */
	i++;

	if (debug)
		alog("Final FJOIN string: %s", merge_args(i, newav));

	do_sjoin(source, i, newav);
	return MOD_CONT;
}

void inspircd_cmd_bot_nick(char *nick, char *user, char *host, char *real,
					  char *modes)
{
	char *nicknumbuf = ts6_uid_retrieve();
	send_cmd(TS6SID, "UID %s %ld %s %s %s %s 0.0.0.0 %ld +%s :%s",
			 nicknumbuf, (long int) time(NULL), nick, host, host, user,
			 (long int) time(NULL), modes, real);
	new_uid(nick, nicknumbuf);
	if (strchr(modes, 'o') != NULL)
		send_cmd(nicknumbuf, "OPERTYPE Bot");
}

void inspircd_cmd_kick(char *source, char *chan, char *user, char *buf)
{
	User *u = finduser(user);
	Uid *ud = (source ? find_uid(source) : NULL), *ud2 = NULL;
	if (!u) ud2 = (user ? find_uid(user) : NULL);

	if (buf) {
		send_cmd(ud ? ud->uid : source, "KICK %s %s :%s", chan, u ? u->uid : ud2 ? ud2->uid : user, buf);
	} else {
		send_cmd(ud ? ud->uid : source, "KICK %s %s :%s", chan, u ? u->uid : ud2 ? ud2->uid : user, user);
	}
}

void inspircd_cmd_notice_ops(char *source, char *dest, char *buf)
{
	Uid *ud = (source ? find_uid(source) : NULL);
	if (!buf)
		return;

	send_cmd(ud ? ud->uid : TS6SID, "NOTICE @%s :%s", dest, buf);
}


void inspircd_cmd_notice(char *source, char *dest, char *buf)
{
	if (!buf)
		return;

	if (NSDefFlags & NI_MSG) {
		inspircd_cmd_privmsg2(source, dest, buf);
	} else {
		Uid *ud = (source ? find_uid(source) : NULL);
		User *u = finduser(dest);

		send_cmd(ud ? ud->uid : TS6SID, "NOTICE %s :%s", u ? u->uid : dest, buf);
	}
}

void inspircd_cmd_notice2(char *source, char *dest, char *msg)
{
	Uid *ud = (source ? find_uid(source) : NULL);
	User *u = finduser(dest);

	send_cmd(ud ? ud->uid : TS6SID, "NOTICE %s :%s", u ? u->uid : dest, msg);
}

void inspircd_cmd_privmsg(char *source, char *dest, char *buf)
{
	Uid *ud;
	User *u;

	if (!buf)
		return;

	ud = (source ? find_uid(source) : NULL);
	u = finduser(dest);

	send_cmd(ud ? ud->uid : TS6SID, "PRIVMSG %s :%s", u ? u->uid : dest, buf);
}

void inspircd_cmd_privmsg2(char *source, char *dest, char *msg)
{
	Uid *ud = (source ? find_uid(source) : NULL);
	User *u = finduser(dest);

	send_cmd(ud ? ud->uid : TS6SID, "PRIVMSG %s :%s", u ? u->uid : dest, msg);
}

void inspircd_cmd_serv_notice(char *source, char *dest, char *msg)
{
	Uid *ud = (source ? find_uid(source) : NULL);
	send_cmd(ud ? ud->uid : TS6SID, "NOTICE $%s :%s", dest, msg);
}

void inspircd_cmd_serv_privmsg(char *source, char *dest, char *msg)
{
	Uid *ud = (source ? find_uid(source) : NULL);
	send_cmd(ud ? ud->uid : TS6SID, "PRIVMSG $%s :%s", dest, msg);
}


void inspircd_cmd_bot_chan_mode(char *nick, char *chan)
{
	anope_cmd_mode(nick, chan, "%s %s %s", ircd->botchanumode, GET_BOT(nick), GET_BOT(nick));
}

void inspircd_cmd_351(char *source)
{
	User *u = finduser(source);
	if (!u) return;

	send_cmd(TS6SID, "PUSH %s ::%s 351 %s Anope-%s %s :%s - %s (%s) -- %s",
			u->uid, ServerName, source, version_number, ServerName, ircd->name,
			version_flags, EncModule, version_build);
}

/* QUIT */
void inspircd_cmd_quit(char *source, char *buf)
{
	Uid *ud= NULL;
	if (source) ud = find_uid(source);
	if (buf) {
		send_cmd(ud ? ud->uid : source, "QUIT :%s", buf);
	} else {
		send_cmd(ud ? ud->uid : source, "QUIT :Exiting");
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
void inspircd_cmd_server(char *servname, int hop, char *descript, char *sid)
{
	send_cmd(NULL, "SERVER %s %s %d %s :%s", servname, currentpass, hop,
			sid ? sid : TS6SID, descript);
}

/* PONG */
void inspircd_cmd_pong(char *servname, char *who)
{
	send_cmd(servname, "PONG %s", who);
}

/* JOIN */
void inspircd_cmd_join(char *user, char *channel, time_t chantime)
{
	Uid *ud = (user ? find_uid(user) : NULL);
	send_cmd(TS6SID, "FJOIN %s %ud + :,%s", channel, (unsigned int)chantime, ud ? ud->uid : user);
}

/* UNSQLINE */
void inspircd_cmd_unsqline(char *user)
{
	if (!user)
		return;

	send_cmd(TS6SID, "DELLINE Q %s", user);
}

/* CHGHOST */
void inspircd_cmd_chghost(char *nick, char *vhost)
{
	Uid *ud;

	if (has_chghostmod != 1) {
		anope_cmd_global(s_OperServ, "CHGHOST not loaded!");
		return;
	}
	if (!nick || !vhost)
		return;

	ud = find_uid(s_OperServ);
	send_cmd(ud->uid, "CHGHOST %s %s", nick, vhost);
}

/* CHGIDENT */
void inspircd_cmd_chgident(char *nick, char *vIdent)
{
	Uid *ud;

	if (has_chgidentmod != 1) {
		anope_cmd_global(s_OperServ, "CHGIDENT not loaded!");
		return;
	}
	if (!nick || !vIdent || !*vIdent)
		return;

	ud = find_uid(s_OperServ);
	send_cmd(ud->uid, "CHGIDENT %s %s", nick, vIdent);
}

/* INVITE */
void inspircd_cmd_invite(char *source, char *chan, char *nick)
{
	Uid *ud;

	if (!source || !chan || !nick)
		return;

	ud = find_uid(source);
	send_cmd(ud ? ud->uid : TS6SID, "INVITE %s %s", nick, chan);
}

/* PART */
void inspircd_cmd_part(char *nick, char *chan, char *buf)
{
	Uid *ud;

	if (!nick || !chan)
		return;

	ud = find_uid(nick);
	if (!ud) return;
	if (buf) {
		send_cmd(ud->uid, "PART %s :%s", chan, buf);
	} else {
		send_cmd(ud->uid, "PART %s :Leaving", chan);
	}
}



/* 391 */
void inspircd_cmd_391(char *source, char *buf)
{
	/* Numeric 391 is not used by InspIRCd.
	 * Instead time requests are done through the TIME s2s command and
	 * by the client server translated into a rfc conform syntax. ~ Viper */
	return;
}

/* 250 */
void inspircd_cmd_250(char *buf)
{
	char *target;
	User *u;

	if (!buf)
		return;

	target = myStrGetToken(buf, ' ', 0);
	u = finduser(target);
	free(target);
	if (!u) return;

	send_cmd(TS6SID, "PUSH %s ::%s 250 %s", u->uid, ServerName, buf);
}

/* 307 */
void inspircd_cmd_307(char *buf)
{
	char *target;
	User *u;

	if (!buf)
		return;

	target = myStrGetToken(buf, ' ', 0);
	u = finduser(target);
	free(target);
	if (!u) return;

	send_cmd(TS6SID, "PUSH %s ::%s 307 %s", u->uid, ServerName, buf);
}

/* 311 */
void inspircd_cmd_311(char *buf)
{
	char *target;
	User *u;

	if (!buf)
		return;

	target = myStrGetToken(buf, ' ', 0);
	u = finduser(target);
	free(target);
	if (!u) return;

	send_cmd(TS6SID, "PUSH %s ::%s 311 %s", u->uid, ServerName, buf);
}

/* 312 */
void inspircd_cmd_312(char *buf)
{
	char *target;
	User *u;

	if (!buf)
		return;

	target = myStrGetToken(buf, ' ', 0);
	u = finduser(target);
	free(target);
	if (!u) return;

	send_cmd(TS6SID, "PUSH %s ::%s 312 %s", u->uid, ServerName, buf);
}

/* 317 */
void inspircd_cmd_317(char *buf)
{
	char *target;
	User *u;

	if (!buf)
		return;

	target = myStrGetToken(buf, ' ', 0);
	u = finduser(target);
	free(target);
	if (!u) return;

	send_cmd(TS6SID, "PUSH %s ::%s 317 %s", u->uid, ServerName, buf);
}

/* 219 */
void inspircd_cmd_219(char *source, char *letter)
{
	User *u = finduser(source);
	if (!source)
		return;

	if (letter)
		send_cmd(TS6SID, "PUSH %s ::%s 219 %s %c :End of /STATS report.", u ? u->uid : source, ServerName, source, *letter);
	else
		send_cmd(TS6SID, "PUSH %s ::%s 219 %s l :End of /STATS report.", u ? u->uid : source, ServerName, source);
}

/* 401 */
void inspircd_cmd_401(char *source, char *who)
{
	User *u = finduser(source);
	if (!source || !who)
		return;

	send_cmd(TS6SID, "PUSH %s ::%s 401 %s %s :No such service.", u ? u->uid : source, ServerName, source, who);
}

/* 318 */
void inspircd_cmd_318(char *source, char *who)
{
	User *u = finduser(source);
	if (!source || !who)
		return;

	send_cmd(TS6SID, "PUSH %s ::%s 318 %s %s :End of /WHOIS list.", u ? u->uid : source, ServerName, source, who);
}

/* 242 */
void inspircd_cmd_242(char *buf)
{
	char *target;
	User *u;

	if (!buf)
		return;

	target = myStrGetToken(buf, ' ', 0);
	u = finduser(target);
	free(target);
	if (!u) return;

	send_cmd(TS6SID, "PUSH %s ::%s 242 %s", u->uid, ServerName, buf);
}

/* 243 */
void inspircd_cmd_243(char *buf)
{
	char *target;
	User *u;

	if (!buf)
		return;

	target = myStrGetToken(buf, ' ', 0);
	u = finduser(target);
	free(target);
	if (!u) return;

	send_cmd(TS6SID, "PUSH %s ::%s 243 %s", u->uid, ServerName, buf);
}

/* 211 */
void inspircd_cmd_211(char *buf)
{
	char *target;
	User *u;

	if (!buf)
		return;

	target = myStrGetToken(buf, ' ', 0);
	u = finduser(target);
	free(target);
	if (!u) return;

	send_cmd(TS6SID, "PUSH %s ::%s 211 %s", u->uid, ServerName, buf);
}

/* GLOBOPS */
void inspircd_cmd_global(char *source, char *buf)
{
	Uid *ud = NULL;
	if (!buf)
		return;

	if (source) ud = find_uid(source);
	if (!ud) ud = find_uid(s_OperServ);

	send_cmd(ud ? ud->uid : TS6SID, "SNONOTICE g :%s", buf);
}

/* SQLINE */
void inspircd_cmd_sqline(char *mask, char *reason)
{
	if (!mask || !reason)
		return;

	send_cmd(TS6SID, "ADDLINE Q %s %s %ld 0 :%s", mask, s_OperServ,
			 (long int) time(NULL), reason);
}

/* SQUIT */
void inspircd_cmd_squit(char *servname, char *message)
{
	if (!servname || !message)
		return;

	send_cmd(TS6SID, "SQUIT %s :%s", servname, message);
}

/* SVSO */
void inspircd_cmd_svso(char *source, char *nick, char *flag)
{
}

/* NICK <newnick>  */
void inspircd_cmd_chg_nick(char *oldnick, char *newnick)
{
	Uid *ud;
	if (!oldnick || !newnick)
		return;
	ud = find_uid(oldnick);
	if (!ud)
		ud = find_uid(newnick);

	if (ud)
		strscpy(ud->nick, newnick, NICKMAX);
	send_cmd(ud ? ud->uid : oldnick, "NICK %s %ld", newnick, time(NULL));
}

/* SVSNICK */
void inspircd_cmd_svsnick(char *source, char *guest, time_t when)
{
	User *u;

	if (!source || !guest)
		return;

	u = finduser(source);
	/* Please note that inspircd will now echo back a nickchange for this SVSNICK. */
	send_cmd(TS6SID, "SVSNICK %s %s %lu", u ? u->uid : source, guest, (unsigned long) when);
}

/* Functions that use serval cmd functions */

void inspircd_cmd_vhost_on(char *nick, char *vIdent, char *vhost)
{
	if (!nick)
		return;

	if (vIdent)
		inspircd_cmd_chgident(nick, vIdent);

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
	send_cmd(NULL, "CAPAB START 1202");
	send_cmd(NULL, "CAPAB CAPABILITIES :PROTOCOL=1202");
	send_cmd(NULL, "CAPAB END");
	inspircd_cmd_server(ServerName, 0, ServerDesc, TS6SID);
	me_server = new_server(NULL, ServerName, ServerDesc, SERVER_ISME, TS6SID);
}

void inspircd_cmd_bob()
{
	send_cmd(TS6SID, "BURST");
	send_cmd(TS6SID, "VERSION :Anope-%s %s :%s - %s (%s) -- %s",
			 version_number, ServerName, ircd->name, version_flags,
			 EncModule, version_build);
}

/* Events */

int anope_event_ping(char *source, int ac, char **av)
{
	if (ac < 1)
		return MOD_CONT;

	if (ac < 2)
		inspircd_cmd_pong(TS6SID, av[0]);

	if (ac == 2) {
		char buf[BUFSIZE];
		snprintf(buf, BUFSIZE - 1, "%s %s", av[1], av[0]);
		inspircd_cmd_pong(TS6SID, buf);
	}
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
	User *u;
	if (!source)
		return MOD_CONT;

	u = find_byuid(source);
	m_away(u ? u->nick : source, (ac ? av[0] : NULL));
	return MOD_CONT;
}

int anope_event_topic(char *source, int ac, char **av)
{
	Channel *c = findchan(av[0]);
	time_t topic_time = time(NULL);
	User *u = find_byuid(source);

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

	strscpy(c->topic_setter, u ? u->nick : source, sizeof(c->topic_setter));
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
	Server *s;

	if (ac < 1 || ac > 3)
		return MOD_CONT;

	/* On InspIRCd we must send a SQUIT when we receive RSQUIT for a server we have juped */
	s = findserver(servlist, av[0]);
	if (!s)
		s = findserver_uid(servlist, av[0]);
	if (s && s->flags & SERVER_JUPED)
	{
		send_cmd(TS6SID, "SQUIT %s :%s", s->suid, ac > 1 ? av[1] : "");
	}

	do_squit(source, ac, av);
	return MOD_CONT;
}

int anope_event_quit(char *source, int ac, char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = find_byuid(source);
	do_quit(u ? u->nick : source, ac, av);
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
		User *u = find_byuid(source);
		User *u2 = find_byuid(av[0]);

		/* This can happen with server-origin modes. */
		if (u == NULL)
			u = u2;

		/* If it's still null, drop it like fire.
		 * most likely situation was that server introduced a nick
		 * which we subsequently akilled */
		if (u == NULL)
			return MOD_CONT;

		av[0] = u2->nick;
		do_umode(u->nick, ac, av);
	}
	return MOD_CONT;
}


int anope_event_kill(char *source, int ac, char **av)
{
	Uid *ud;
	User *u;

	if (ac != 2)
		return MOD_CONT;

	u = find_byuid(av[0]);
	ud = find_nickuid(av[0]);

	m_kill(u ? u->nick : (ud ? ud->nick : av[0]), av[1]);
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
	if (!source)
		return MOD_CONT;

	m_motd(source);
	return MOD_CONT;
}

int anope_event_setname(char *source, int ac, char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = find_byuid(source);
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

	u = find_byuid(source);
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

	u = find_byuid(source);
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

	u = find_byuid(source);
	if (!u) {
		if (debug) {
			alog("debug: CHGIDENT for nonexistent user %s", av[0]);
		}
		return MOD_CONT;
	}

	change_user_username(u, av[0]);
	return MOD_CONT;
}

int anope_event_sethost(char *source, int ac, char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = find_byuid(source);
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
	do_nick(source, av[0], NULL, NULL, NULL, NULL, 0, 0, 0, NULL, NULL);
	return MOD_CONT;
}

/*
 * [Nov 03 22:09:58.176252 2009] debug: Received: :964 UID 964AAAAAC 1225746297 w00t2 localhost testnet.user w00t 127.0.0.1 1225746302 +iosw +ACGJKLNOQcdfgjklnoqtx :Robin Burchell <w00t@inspircd.org>
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
 *
 * For now we assume that if we get  a +r, the user owns the nick...  THIS IS NOT TRUE!!!
 * If a user changes nick, he keeps his +r, but his METADATA will be incorrect. So we will be cancelling this
 * if we don't get a matching METDATA for him.. ~Viper
 */
int anope_event_uid(char *source, int ac, char **av)
{
	User *user = NULL;
	struct in_addr addy;
	Server *s = findserver_uid(servlist, source);
	uint32 *ad = (uint32 *) & addy;
	int ts = strtoul(av[1], NULL, 10);
	int regged = ((strchr(av[8], 'r') != NULL) ? ts : 0);

	/* The previously introduced user got recognized, but didn't deserve it. 
	 * Invalidate the recognition. */
	user = u_intro_regged;
	u_intro_regged = NULL;
	if (user) {
		if (debug)
			alog("debug: User %s had +r but received no account info.", user->nick);
		if (user->na)
		user->na->status &= ~NS_RECOGNIZED;
		validate_user(user);
		common_svsmode(user, "-r", NULL);
	}
	user = NULL;

	inet_aton(av[6], &addy);
	/* do_nick() will mark the user as recognized - not identified - for now.
	 * It is up to the protocol module to either mark the user as identified or invalidate the 
	 * recognition in a later stage. ~ Viper */
	user = do_nick("", av[2],   /* nick */
			av[5],			  /* username */
			av[3],			  /* realhost */
			s->name,			/* server */
			av[ac - 1],		 /* realname */
			ts, regged && burst ? 2 : 0, htonl(*ad), av[4], av[0]);

	if (user) {
		/* Unless we receive metadata, this user will be logged out next run... */
		if (regged && burst && user->na) u_intro_regged = user;
		anope_set_umode(user, 1, &av[8]);
	}
	return MOD_CONT;
}

/*
 * We need to process the METADATA accountname information to check whether people are actually using their own nicks.
 * We cannot trust +r since it is not removed upon nick change, so people would be able to take over nicks by changing to them while
 * services are down. To prevent this we compare their accountname info and make sure it s the same as their display nick. ~ Viper
 */
int anope_event_metadata(char *source, int ac, char **av)
{
	User *u;
	Server *s;
	NickAlias *na;

	if (ac != 3 || !u_intro_regged || !burst)
		return MOD_CONT;

	s = findserver_uid(servlist, source);
	if (!s) {
		if (debug) alog("debug: Received METADATA from unknown source.");
		return MOD_CONT;
	}
	if (strcmp(av[1], "accountname"))
		return MOD_CONT;

	u = find_byuid(av[0]);
	if (!u) {
		if (debug) alog("debug: METADATA for nonexistent user %s.", av[0]);
		return MOD_CONT;
	}
	if (u != u_intro_regged) {
		if (debug) alog("debug: ERROR: Expected different user in METADA.");
		return MOD_CONT;
	}
	u_intro_regged = NULL;

	na = findnick(av[2]);
	if (na && u->na) {
		if (na->nc == u->na->nc) {
			/* The user was previously ID'd to the same nickgroup. Auto-ID him. */
			u->na->status |= NS_IDENTIFIED;
			check_memos(u);

			if (NSNickTracking)
				nsStartNickTracking(u);

			u->na->last_seen = time(NULL);
			if (u->na->last_usermask)
				free(u->na->last_usermask);
			u->na->last_usermask =
					smalloc(strlen(common_get_vident(u)) +
					strlen(common_get_vhost(u)) + 2);
			sprintf(u->na->last_usermask, "%s@%s",
					common_get_vident(u), common_get_vhost(u));

			alog("%s: %s!%s@%s automatically identified for nick %s",
					s_NickServ, u->nick, u->username, u->host, u->nick);
		} else {
			/* The user was not previously ID'd to this nickgroup but another...
			 * Invalidate the recognition. */
			if (debug)
				alog("debug: User %s had +r but did not receive matching account info.", u->nick);
			u->na->status &= ~NS_RECOGNIZED;
			common_svsmode(u, "-r", NULL);
			validate_user(u);
		}
	}

	return MOD_CONT;
}

int anope_event_burst(char *source, int ac, char **av)
{
	/* As of now a burst is in progress.. */
	burst = 1;

	return MOD_CONT;
}

int anope_event_eob(char *source, int ac, char **av)
{
	User *u = u_intro_regged;
	u_intro_regged = NULL;

	/* The burst is complete.. check if there was a user that got recognized and still needs to be invalidated.*/
	if (u) {
		if (u->na)
		u->na->status &= ~NS_RECOGNIZED;
		common_svsmode(u, "-r", NULL);
		validate_user(u);
	}

	/* End of burst.. */
	burst = 0;

	return MOD_CONT;
}

int anope_event_chghost(char *source, int ac, char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = find_byuid(source);
	if (!u) {
		if (debug) {
			alog("debug: FHOST for nonexistent user %s.", source);
		}
		return MOD_CONT;
	}

	change_user_host(u, av[0]);
	return MOD_CONT;
}

/*
 * [Nov 04 00:08:46.308435 2009] debug: Received: SERVER irc.inspircd.com pass 0 964 :Testnet Central!
 * 0: name
 * 1: pass
 * 2: hops
 * 3: numeric
 * 4: desc
 */
int anope_event_server(char *source, int ac, char **av)
{
	if (!stricmp(av[2], "0")) {
		uplink = sstrdup(av[0]);
	}
	do_server(source, av[0], av[2], av[4], av[3]);
	return MOD_CONT;
}


int anope_event_privmsg(char *source, int ac, char **av)
{
	Uid *ud;
	User *u = find_byuid(source);

	if (ac != 2 || !u)
		return MOD_CONT;

	ud = find_nickuid(av[0]);
	m_privmsg((u ? u->nick : source), (ud ? ud->nick : av[0]), av[1]);
	return MOD_CONT;
}

int anope_event_part(char *source, int ac, char **av)
{
	User *u;
	if (ac < 1 || ac > 2)
		return MOD_CONT;

	u = find_byuid(source);
	do_part((u ? u->nick : source), ac, av);
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
	char *argv[6];

	if (strcasecmp(av[0], "START") == 0) {
		if (ac < 2 || atoi(av[1]) < 1202)
		{
			send_cmd(NULL, "ERROR :Protocol mismatch, no or invalid protocol version given in CAPAB START");
			quitmsg = "Protocol mismatch, no or invalid protocol version given in CAPAB START";
			quitting = 1;
			return MOD_STOP;
		}
		/* reset CAPAB */
		has_servicesmod = 0;
		has_chghostmod = 0;
		has_chgidentmod = 0;
		has_servprotectmod = 0;
		has_hidechansmod = 0;
	} else if (strcasecmp(av[0], "MODSUPPORT") == 0) {
	        if (strstr(av[1], "m_services_account.so"))
			has_servicesmod = 1;
		if (strstr(av[1], "m_chghost.so"))
			has_chghostmod = 1;
		if (strstr(av[1], "m_chgident.so"))
			has_chgidentmod = 1;
	} else if (strcasecmp(av[0], "MODULES") == 0) {
		if (strstr(av[1], "m_svshold.so"))
			ircd->svshold = 1;
	} else if (strcasecmp(av[0], "CHANMODES") == 0) {
		char **argv;
		int argc = split_buf(av[1], &argv, 1);
		int i;
		
		for (i = 0; i < argc; ++i)
		{
			char *modename = argv[i];
			char *modechar = strchr(modename, '=');
			if (!modechar)
				continue;
			*modechar++ = 0;

			if (!strcasecmp(modename, "ban"))
			{
				myCmmodes[(int) *modechar].addmask = add_ban;
				myCmmodes[(int) *modechar].delmask = del_ban;
			}
			else if (!strcasecmp(modename, "banexception"))
			{
				myCmmodes[(int) *modechar].addmask = add_exception;
				myCmmodes[(int) *modechar].delmask = del_exception;
				ircd->except = 1;
			}
			else if (!strcasecmp(modename, "invex"))
			{
				myCmmodes[(int) *modechar].addmask = add_invite;
				myCmmodes[(int) *modechar].delmask = del_invite;
				ircd->invitemode = 1;
			}
			else if (!strcasecmp(modename, "admin"))
			{
				ircd->botchanumode = "+ao";
				ircd->protect = 1;

				myCumodes[(int) *(modechar + 1)].status = CUS_PROTECT;
				myCumodes[(int) *(modechar + 1)].flags = CUF_PROTECT_BOTSERV;
				myCumodes[(int) *(modechar + 1)].is_valid = check_valid_op;
			}
			else if (!strcasecmp(modename, "founder"))
			{
				ircd->owner = 1;

				myCumodes[(int) *(modechar + 1)].status = CUS_OWNER;
				myCumodes[(int) *(modechar + 1)].flags = 0;
				myCumodes[(int) *(modechar + 1)].is_valid = check_valid_op;
			}
			else if (!strcasecmp(modename, "halfop"))
			{
				ircd->halfop = 1;

				myCumodes[(int) *(modechar + 1)].status = CUS_HALFOP;
				myCumodes[(int) *(modechar + 1)].flags = 0;
				myCumodes[(int) *(modechar + 1)].is_valid = check_valid_op;
			}
			else if (!strcasecmp(modename, "flood"))
			{
				CBModeInfo *cb = find_cbinfo(*modechar);
				if (cb)
				{
					cb->getvalue = get_flood;
					cb->csgetvalue = cs_get_flood;
				}

				myCbmodes[(int) *modechar].flag = CMODE_f;
				myCbmodes[(int) *modechar].setvalue = set_flood;
				myCbmodes[(int) *modechar].cssetvalue = cs_set_flood;

				snprintf(flood_mode_set, sizeof(flood_mode_set), "+%c", *modechar);
				snprintf(flood_mode_unset, sizeof(flood_mode_unset), "-%c", *modechar);

				ircd->fmode = 1;
			}
			else if (!strcasecmp(modename, "joinflood"))
			{
				CBModeInfo *cb = find_cbinfo(*modechar);
				if (cb)
				{
					cb->getvalue = get_throttle;
					cb->csgetvalue = cs_get_throttle;
				}

				myCbmodes[(int) *modechar].flag = CMODE_j;
				myCbmodes[(int) *modechar].flags |= CBM_NO_MLOCK;
				myCbmodes[(int) *modechar].setvalue = chan_set_throttle;
				myCbmodes[(int) *modechar].cssetvalue = cs_set_throttle;

				ircd->jmode = 1;
			}
			else if (!strcasecmp(modename, "redirect"))
			{
				CBModeInfo *cb = find_cbinfo(*modechar);
				if (cb)
				{
					cb->getvalue = get_redirect;
					cb->csgetvalue = cs_get_redirect;
				}

				myCbmodes[(int) *modechar].flag = CMODE_L;
				myCbmodes[(int) *modechar].setvalue = set_redirect;
				myCbmodes[(int) *modechar].cssetvalue = cs_set_redirect;

				ircd->Lmode = 1;
			}
			else if (!strcasecmp(modename, "filter") || !strcasecmp(modename, "kicknorejoin"))
			{
				myCbmodes[(int) *modechar].flags |= CBM_NO_MLOCK;
			}
			else if (!strcasecmp(modename, "operonly") || !strcasecmp(modename, "permanent"))
			{
				myCbmodes[(int) *modechar].flags |= CBM_NO_USER_MLOCK;
			}
		}
	} else if (strcasecmp(av[0], "USERMODES") == 0) {
		char **argv;
		int argc = split_buf(av[1], &argv, 1);
		int i;

		for (i = 0; i < argc; ++i)
		{
			char *name = argv[i];
			char *value = strchr(name, '=');
			if (!value)
				continue;
			*value++ = 0;

			if (!strcasecmp(name, "hidechans"))
				has_hidechansmod = 1;
			else if (!strcasecmp(name, "servprotect"))
				has_servprotectmod = 1;
		}
	} else if (strcasecmp(av[0], "CAPABILITIES") == 0) {
		char **argv;
		int argc = split_buf(av[1], &argv, 1);
		int i;

		for (i = 0; i < argc; ++i)
		{
			char *name = argv[i];
			char *value = strchr(name, '=');
			if (!value)
				continue;
			*value++ = 0;

			/* PREFIX=(yqaohvV)!~&@%+- */
			if (!strcasecmp(name, "PREFIX") && value)
			{
				int prefixlen = strlen(value), j;

				for (j = 0; j < prefixlen / 2; ++j)
				{
					switch (value[j])
					{
						case '(':
						case ')':
							break;
						default:
							if (debug)
								alog("Tied %c => %c", value[prefixlen / 2 + j], value[j]);
							myCsmodes[(int) value[prefixlen / 2 + j]] = value[j];
							if (!myCumodes[(int) value[j]].status)
							{
								myCumodes[(int) value[j]].status = get_new_statusmode();
								myCumodes[(int) value[j]].flags = 0;
								myCumodes[(int) value[j]].is_valid = check_valid_op;
							}
							break;
					}
				}
			}
			/* CHANMODES=IXYZbegw,k,FHJLdfjl,ABCDGKMNOPQRSTcimnprstuz
			 *           listmodes, unset with param, unset no param, regular
			 */
			else if (!strcasecmp(name, "CHANMODES"))
			{
				char *listmodes = myStrGetToken(value, ',', 0);
				char *unsetparam = myStrGetToken(value, ',', 1);
				char *unsetnoparam = myStrGetToken(value, ',', 2);
				char *regular = myStrGetToken(value, ',', 3);

				if (listmodes)
				{
					int i;
					for (i = 0; i < strlen(listmodes); ++i)
					{
						if (!myCmmodes[(int) listmodes[i]].addmask)
							myCmmodes[(int) listmodes[i]].addmask = do_nothing;
						if (!myCmmodes[(int) listmodes[i]].delmask)
							myCmmodes[(int) listmodes[i]].delmask = do_nothing;
					}
				}
				if (unsetparam)
				{
					 int i;
					 for (i = 0; i < strlen(unsetparam); ++i)
					 {
					 	CBModeInfo *cb = find_cbinfo(unsetparam[i]);
						if (cb)
						{
							if (!cb->getvalue)
								cb->getvalue = get_unkwn;
							if (!cb->csgetvalue)
								cb->csgetvalue = cs_get_unkwn;
						}

						myCbmodes[(int) unsetparam[i]].flag = get_mode_from_char(unsetparam[i]);
						if (!myCbmodes[(int) unsetparam[i]].setvalue)
							myCbmodes[(int) unsetparam[i]].setvalue = set_unkwn;
						if (!myCbmodes[(int) unsetparam[i]].cssetvalue)
							myCbmodes[(int) unsetparam[i]].cssetvalue = cs_set_unkwn;
					 }
				}
				if (unsetnoparam)
				{
					int i;
					for (i = 0; i < strlen(unsetnoparam); ++i)
					{
						CBModeInfo *cb = find_cbinfo(unsetnoparam[i]);
						if (cb)
						{
							cb->flags |= CBM_MINUS_NO_ARG;

							if (!cb->getvalue)
								cb->getvalue = get_unkwn;
							if (!cb->csgetvalue)
								cb->csgetvalue = cs_get_unkwn;
						}

						myCbmodes[(int) unsetnoparam[i]].flag = get_mode_from_char(unsetnoparam[i]);
						myCbmodes[(int) unsetnoparam[i]].flags |= CBM_MINUS_NO_ARG;
						if (!myCbmodes[(int) unsetnoparam[i]].setvalue)
							myCbmodes[(int) unsetnoparam[i]].setvalue = set_unkwn;
						if (!myCbmodes[(int) unsetnoparam[i]].cssetvalue)
							myCbmodes[(int) unsetnoparam[i]].cssetvalue = cs_set_unkwn;
					}
				}
				if (regular)
				{
					int i;
					for (i = 0; i < strlen(regular); ++i)
					{
						CBModeInfo *cb = find_cbinfo(regular[i]);
						if (cb)
						{
							cb->getvalue = NULL;
							cb->csgetvalue = NULL;
						}

						myCbmodes[(int) regular[i]].flag = get_mode_from_char(regular[i]);
						myCbmodes[(int) regular[i]].setvalue = NULL;
						myCbmodes[(int) regular[i]].cssetvalue = NULL;
					}
				}

				if (listmodes)
					free(listmodes);
				if (unsetparam)
					free(unsetparam);
				if (unsetnoparam)
					free(unsetnoparam);
				if (regular)
					free(regular);
			}
		}
	} else if (strcasecmp(av[0], "END") == 0) {
		if (!has_servicesmod) {
			send_cmd(NULL, "ERROR :m_services_account is not loaded. This is required by Anope");
			quitmsg = "Remote server does not have the m_services_account module loaded, and this is required.";
			quitting = 1;
			return MOD_STOP;
		}
		if (!has_hidechansmod) {
			send_cmd(NULL, "ERROR :m_hidechans is not loaded. This is required by Anope");
			quitmsg = "Remote server does not have the m_hidechans module loaded, and this is required.";
			quitting = 1;
			return MOD_STOP;
		}
		if (!ircd->svshold) {
			anope_cmd_global(s_OperServ, "SVSHOLD missing, Usage disabled until module is loaded.");
		}
		if (!has_chghostmod) {
			anope_cmd_global(s_OperServ, "CHGHOST missing, Usage disabled until module is loaded.");
		}
		if (!has_chgidentmod) {
			anope_cmd_global(s_OperServ, "CHGIDENT missing, Usage disabled until module is loaded.");
		}

		if (has_servprotectmod) {
			ircd->nickservmode = "+oIk";
			ircd->chanservmode = "+oIk";
			ircd->memoservmode = "+Ik";
			ircd->hostservmode = "+oIk";
			ircd->operservmode = "+ioIk";
			ircd->botservmode = "+Ik";
			ircd->helpservmode = "+Ik";
			ircd->devnullmode = "+iIk";
			ircd->globalmode = "+iIk";

			ircd->nickservaliasmode = "+oIk";
			ircd->chanservaliasmode = "+oIk";
			ircd->memoservaliasmode = "+Ik";
			ircd->hostservaliasmode = "+oIk";
			ircd->operservaliasmode = "+ioIk";
			ircd->botservaliasmode = "+Ik";
			ircd->helpservaliasmode = "+Ik";
			ircd->devnullvaliasmode = "+iIk";
			ircd->globalaliasmode = "+iIk";

			ircd->botserv_bot_mode = "+Ik";
			alog("Support for usermode +k enabled. Pseudo-clients will be gods.");
		}

		/* Generate a fake capabs parsing call so things like NOQUIT work
		 * fine. It's ugly, but it works....
		 */
		argv[0] = "NOQUIT";
		argv[1] = "SSJ3";
		argv[2] = "NICK2";
		argv[3] = "VL";
		argv[4] = "TLKEXT";
		argv[5] = "UNCONNECT";

		capab_parse(6, argv);

		/* check defcon */
		if (DefConLevel && !defconParseModeString(DefConChanModes))
		{
			send_cmd(TS6SID, "ERROR :Defcon modes failed to validate");
			quitmsg = "Defcon modes failed to validate";
			quitting = 1;		
			return MOD_CONT;
		}

		/* We have received our CAPAB, now we can introduce our clients. */
		init_tertiary();
	}

	pmodule_ircd_cbmodeinfos(myCbmodeinfos);
	pmodule_ircd_cumodes(myCumodes);
	pmodule_ircd_flood_mode_char_set(flood_mode_set);
	pmodule_ircd_flood_mode_char_remove(flood_mode_unset);
	pmodule_ircd_cbmodes(myCbmodes);
	pmodule_ircd_cmmodes(myCmmodes);
	pmodule_ircd_csmodes(myCsmodes);

	return MOD_CONT;
}

/* SVSHOLD - set */
void inspircd_cmd_svshold(char *nick)
{
	Uid *ud = find_uid(s_OperServ);
	send_cmd(ud->uid, "SVSHOLD %s %ds :%s", nick, NSReleaseTimeout,
			"Being held for registered user");
}

/* SVSHOLD - release */
void inspircd_cmd_release_svshold(char *nick)
{
	Uid *ud = find_uid(s_OperServ);
	send_cmd(ud->uid, "SVSHOLD %s", nick);
}

/* UNSGLINE */
void inspircd_cmd_unsgline(char *mask)
{
	/* Not Supported by this IRCD */
}

/* UNSZLINE */
void inspircd_cmd_unszline(char *mask)
{
	send_cmd(TS6SID, "DELLINE Z %s", mask);
}

/* SZLINE */
void inspircd_cmd_szline(char *mask, char *reason, char *whom)
{
	send_cmd(TS6SID, "ADDLINE Z %s %s %ld 0 :%s", mask, whom,
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
	User *u = finduser(nick);
	Uid *ud = (source ? find_uid(source) : NULL);
	send_cmd(ud ? ud->uid : source, "SVSJOIN %s %s", u ? u->uid : nick, chan);
}

void inspircd_cmd_svspart(char *source, char *nick, char *chan)
{
	User *u = finduser(nick);
	Uid *ud = (source ? find_uid(source) : NULL);
	send_cmd(ud ? ud->uid : source, "SVSPART %s %s", u ? u->uid : nick, chan);
}

void inspircd_cmd_swhois(char *source, char *who, char *mask)
{
	User *u = finduser(who);
	send_cmd(TS6SID, "METADATA %s swhois :%s", u ? u->uid : who, mask);
}

void inspircd_cmd_eob()
{
	send_cmd(TS6SID, "ENDBURST");
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
	char *sid;

	snprintf(rbuf, sizeof(rbuf), "Juped by %s%s%s", who,
			 reason ? ": " : "", reason ? reason : "");

	if (findserver(servlist, jserver))
		inspircd_cmd_squit(jserver, rbuf);
	for (sid = ts6_sid_retrieve(); findserver_uid(servlist, sid); sid = ts6_sid_retrieve());
	inspircd_cmd_server(jserver, 1, rbuf, sid);
	new_server(me_server, jserver, rbuf, SERVER_JUPED, sid);
}

/* GLOBOPS - to handle old WALLOPS */
void inspircd_cmd_global_legacy(char *source, char *fmt)
{
	Uid *ud = NULL;

	if (source)
		ud = find_uid(source);
	if (!ud)
		ud = find_uid(s_OperServ);

	send_cmd(ud->uid, "SNONOTICE g :%s", fmt);
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
	Uid *ud;
	User *u = finduser(dest);

	if (!buf) {
		return;
	} else {
		s = normalizeBuffer(buf);
	}
	ud = (source ? find_uid(source) : NULL);

	send_cmd(ud ? ud->uid : TS6SID, "NOTICE %s :\1%s\1", u ? u->uid : dest, s);
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

int anope_event_time(char *source, int ac, char **av)
{
	if (ac !=2)
		return MOD_CONT;

	send_cmd(TS6SID, "TIME %s %s %ld", source, av[1], (long int) time(NULL));

	/* We handled it, don't pass it on to the core..
	 * The core doesn't understand our syntax anyways.. */
	return MOD_STOP;
}

/*******************************************************************/

/*
 * TS6 generator code, provided by DukePyrolator
 */

static int ts6_sid_initted = 0;
static char ts6_new_sid[4];

int is_sid(char *sid)
{
	/* Returns true if the string given is exactly 3 characters long,
	 * starts with a digit, and the other two characters are A-Z or digits
	 */
	return ((strlen(sid)  == 3) && isdigit(sid[0]) &&
			((sid[1] >= 'A' && sid[1] <= 'Z') || isdigit(sid[1])) &&
			((sid[2] >= 'A' && sid[2] <= 'Z') || isdigit(sid[2])));
}

void ts6_sid_increment(unsigned pos)
{
	/*
	 * An SID must be exactly 3 characters long, starts with a digit,
	 * and the other two characters are A-Z or digits
	 * The rules for generating an SID go like this...
	 * --> ABCDEFGHIJKLMNOPQRSTUVWXYZ --> 0123456789 --> WRAP
	 */
	if (!pos) {
		/* At pos 0, if we hit '9', we've run out of available SIDs,
		 * reset the SID to the smallest possible value and try again. */
		if (ts6_new_sid[pos] == '9') {
			ts6_new_sid[0] = '0';
			ts6_new_sid[1] = 'A';
			ts6_new_sid[2] = 'A';
		} else
			/* But if we haven't, just keep incrementing merrily. */
			++ts6_new_sid[0];
	} else {
		if (ts6_new_sid[pos] == 'Z')
			ts6_new_sid[pos] = '0';
		else if (ts6_new_sid[pos] == '9') {
			ts6_new_sid[pos] = 'A';
			ts6_sid_increment(pos - 1);
		} else
			++ts6_new_sid[pos];
	}
}

char *ts6_sid_retrieve()
{
	if (!ts6_sid_initted) {
		/* Initialize ts6_new_sid with the services server SID */
		snprintf(ts6_new_sid, 4, "%s", TS6SID);
		ts6_sid_initted = 1;
	}
	while (1) {
		/* Check if the new SID is used by a known server */
		if (!findserver_uid(servlist, ts6_new_sid))
			/* return the new SID */
			return ts6_new_sid;

		/* Add one to the last SID */
		ts6_sid_increment(2);
	}
	/* not reached */
	return "";
}

/*******************************************************************/

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
	int noforksave = nofork;

	moduleAddAuthor("Anope");
	moduleAddVersion(VERSION_STRING);
	moduleSetType(PROTOCOL);

	if (!UseTS6) {
		nofork = 1; /* We're going down, set nofork so this error is printed */
		alog("FATAL ERROR: The InspIRCd 2.0 protocol module requires UseTS6 to be enabled in the services.conf.");
		nofork = noforksave;
		return MOD_STOP;
	}

	if (Numeric && is_sid(Numeric))
		TS6SID = sstrdup(Numeric);
	else {
		nofork = 1; /* We're going down, set nofork so this error is printed */
		alog("FATAL ERROR: The InspIRCd 2.0 protocol module requires the Numeric in the services.conf to contain a TS6SID.");
		nofork = noforksave;
		return MOD_STOP;
	}

	pmodule_ircd_version("InspIRCd 2.0");
	pmodule_ircd_cap(myIrcdcap);
	pmodule_ircd_var(myIrcd);
	pmodule_ircd_cbmodeinfos(myCbmodeinfos);
	pmodule_ircd_cumodes(myCumodes);
	pmodule_ircd_flood_mode_char_set(flood_mode_set);
	pmodule_ircd_flood_mode_char_remove(flood_mode_unset);
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
	pmodule_permchan_mode(CMODE_P);

	moduleAddAnopeCmds();
	moduleAddIRCDMsgs();

	return MOD_CONT;
}

/* EOF */
