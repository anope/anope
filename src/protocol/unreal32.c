/* Unreal IRCD 3.2.x functions
 *
 * (C) 2003-2009 Anope Team
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


/* User Modes */
#define UMODE_a 0x00000001  /* a Services Admin */
#define UMODE_h 0x00000002  /* h Available for help (HelpOp) */
#define UMODE_i 0x00000004  /* i Invisible (not shown in /who) */
#define UMODE_o 0x00000008  /* o Global IRC Operator */
#define UMODE_r 0x00000010  /* r Identifies the nick as being registered  */
#define UMODE_w 0x00000020  /* w Can listen to wallop messages */
#define UMODE_A 0x00000040  /* A Server Admin */
#define UMODE_N 0x00000080  /* N Network Administrator */
#define UMODE_O 0x00000100  /* O Local IRC Operator */
#define UMODE_C 0x00000200  /* C Co-Admin */
#define UMODE_d 0x00000400  /* d Makes it so you can not receive channel PRIVMSGs */
#define UMODE_p 0x00000800  /* Hides the channels you are in in a /whois reply  */
#define UMODE_q 0x00001000  /* q Only U:Lines can kick you (Services Admins Only) */
#define UMODE_s 0x00002000  /* s Can listen to server notices  */
#define UMODE_t 0x00004000  /* t Says you are using a /vhost  */
#define UMODE_v 0x00008000  /* v Receives infected DCC Send Rejection notices */
#define UMODE_z 0x00010000  /* z Indicates that you are an SSL client */
#define UMODE_B 0x00020000  /* B Marks you as being a Bot */
#define UMODE_G 0x00040000  /* G Filters out all the bad words per configuration */
#define UMODE_H 0x00080000  /* H Hide IRCop Status (IRCop Only) */
#define UMODE_S 0x00100000  /* S services client */
#define UMODE_V 0x00200000  /* V Marks you as a WebTV user */
#define UMODE_W 0x00400000  /* W Lets you see when people do a /whois on you */
#define UMODE_T 0x00800000  /* T Prevents you from receiving CTCPs  */
#define UMODE_g 0x20000000  /* g Can send & read globops and locops */
#define UMODE_x 0x40000000  /* x Gives user a hidden hostname */
#define UMODE_R 0x80000000  /* Allows you to only receive PRIVMSGs/NOTICEs from registered (+r) users */


/*************************************************************************/

/* Channel Modes */

#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040		/* These two used only by ChanServ */
#define CMODE_l 0x00000080
#define CMODE_R 0x00000100		/* Only identified users can join */
#define CMODE_r 0x00000200		/* Set for all registered channels */
#define CMODE_c 0x00000400
#define CMODE_A 0x00000800
/* #define CMODE_H 0x00001000   Was now +I may not join, but Unreal Removed it and it will not set in 3.2 */
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
#define CMODE_T 0x02000000
#define CMODE_M 0x04000000


/* Default Modes with MLOCK */

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

IRCDVar myIrcd[] = {
	{"UnrealIRCd 3.2.x",		/* ircd name */
	 "+Soi",					/* Modes used by pseudoclients */
	 5,						 /* Chan Max Symbols	 */
	 "-cilmnpstuzACGHKMNOQRSTV",		/* Modes to Remove */
	 "+ao",					 /* Channel Umode used by Botserv bots */
	 1,						 /* SVSNICK */
	 1,						 /* Vhost  */
	 1,						 /* Has Owner */
	 "+q",					  /* Mode to set for an owner */
	 "-q",					  /* Mode to unset for an owner */
	 "+a",					  /* Mode to set for channel admin */
	 "-a",					  /* Mode to unset for channel admin */
	 "-r+d",				 /* Mode on UnReg */
	 1,						 /* Supports SGlines	 */
	 1,						 /* Supports SQlines	 */
	 1,						 /* Supports SZlines	 */
	 1,						 /* Supports Halfop +h   */
	 3,						 /* Number of server args */
	 0,						 /* Join 2 Set		   */
	 0,						 /* Join 2 Message	   */
	 1,						 /* Has exceptions +e	*/
	 1,						 /* TS Topic Forward	 */
	 0,						 /* TS Topci Backward	*/
	 UMODE_S,				   /* Protected Umode	  */
	 0,						 /* Has Admin			*/
	 0,						 /* Chan SQlines		 */
	 0,						 /* Quit on Kill		 */
	 1,						 /* SVSMODE unban		*/
	 1,						 /* Has Protect		  */
	 1,						 /* Reverse			  */
	 1,						 /* Chan Reg			 */
	 CMODE_r,				   /* Channel Mode		 */
	 1,						 /* vidents			  */
	 1,						 /* svshold			  */
	 1,						 /* time stamp on mode   */
	 1,						 /* NICKIP			   */
	 1,						 /* O:LINE			   */
	 1,						 /* UMODE			   */
	 1,						 /* VHOST ON NICK		*/
	 1,						 /* Change RealName	  */
	 CMODE_K,				   /* No Knock			 */
	 CMODE_A,				   /* Admin Only		   */
	 DEFAULT_MLOCK,			 /* Default MLOCK	   */
	 UMODE_x,				   /* Vhost Mode		   */
	 1,						 /* +f				   */
	 1,						 /* +L				   */
	 CMODE_f,				   /* +f Mode						  */
	 CMODE_L,				   /* +L Mode						  */
	 0,						 /* On nick change check if they could be identified */
	 1,						 /* No Knock requires +i */
	 NULL,					  /* CAPAB Chan Modes			 */
	 1,						 /* We support Unreal TOKENS */
	 1,						 /* TIME STAMPS are BASE64 */
	 1,						 /* +I support */
	 '&',					   /* SJOIN ban char */
	 '\"',					  /* SJOIN except char */
	 '\'',					  /* SJOIN invite char */
	 1,						 /* Can remove User Channel Modes with SVSMODE */
	 0,						 /* Sglines are not enforced until user reconnects */
	 "x",					   /* vhost char */
	 0,						 /* ts6 */
	 1,						 /* support helper umode */
	 0,						 /* p10 */
	 NULL,					  /* character set */
	 0,						 /* CIDR channelbans */
	 "$",					   /* TLD Prefix for Global */
	 }
	,
	{NULL}
};

IRCDCAPAB myIrcdcap[] = {
	{
	 CAPAB_NOQUIT,			  /* NOQUIT	   */
	 0,						 /* TSMODE	   */
	 0,						 /* UNCONNECT	*/
	 CAPAB_NICKIP,			  /* NICKIP	   */
	 0,						 /* SJOIN		*/
	 CAPAB_ZIP,				 /* ZIP		  */
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
	 CAPAB_TOKEN,			   /* TOKEN		*/
	 0,						 /* VHOST		*/
	 CAPAB_SSJ3,				/* SSJ3		 */
	 CAPAB_NICK2,			   /* NICK2		*/
	 CAPAB_VL,				  /* VL		   */
	 CAPAB_TLKEXT,			  /* TLKEXT	   */
	 0,						 /* DODKEY	   */
	 0,						 /* DOZIP		*/
	 CAPAB_CHANMODE,			/* CHANMODE			 */
	 CAPAB_SJB64,
	 CAPAB_NICKCHARS,
	 }
};


unsigned long umodes[128] = {
	0, 0, 0,					/* Unused */
	0, 0, 0,					/* Unused */
	0, 0, 0,					/* Unused, Unused, Horzontal Tab */
	0, 0, 0,					/* Line Feed, Unused, Unused */
	0, 0, 0,					/* Carriage Return, Unused, Unused */
	0, 0, 0,					/* Unused */
	0, 0, 0,					/* Unused */
	0, 0, 0,					/* Unused */
	0, 0, 0,					/* Unused */
	0, 0, 0,					/* Unused */
	0, 0, 0,					/* Unused, Unused, Space */
	0, 0, 0,					/* ! " #  */
	0, 0, 0,					/* $ % &  */
	0, 0, 0,					/* ! ( )  */
	0, 0, 0,					/* * + ,  */
	0, 0, 0,					/* - . /  */
	0, 0,					   /* 0 1 */
	0, 0,					   /* 2 3 */
	0, 0,					   /* 4 5 */
	0, 0,					   /* 6 7 */
	0, 0,					   /* 8 9 */
	0, 0,					   /* : ; */
	0, 0, 0,					/* < = > */
	0, 0,					   /* ? @ */
	UMODE_A, UMODE_B, UMODE_C,  /* A B C */
	0, 0, 0,					/* D E F */
	UMODE_G, UMODE_H, 0,		/* G H I */
	0, 0, 0,					/* J K L */
	0, UMODE_N, UMODE_O,		/* M N O */
	0, 0, UMODE_R,			  /* P Q R */
	UMODE_S, UMODE_T, 0,		/* S T U */
	UMODE_V, UMODE_W, 0,		/* V W X */
	0,						  /* Y */
	0,						  /* Z */
	0, 0, 0,					/* [ \ ] */
	0, 0, 0,					/* ^ _ ` */
	UMODE_a, 0, 0,			  /* a b c */
	UMODE_d, 0, 0,			  /* d e f */
	UMODE_g, UMODE_h, UMODE_i,  /* g h i */
	0, 0, 0,					/* j k l */
	0, 0, UMODE_o,			  /* m n o */
	UMODE_p, UMODE_q, UMODE_r,  /* p q r */
	UMODE_s, UMODE_t, 0,		/* s t u */
	UMODE_v, UMODE_w, UMODE_x,  /* v w x */
	0,						  /* y */
	UMODE_z,					/* z */
	0, 0, 0,					/* { | } */
	0, 0						/* ~ � */
};

char myCsmodes[128] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0,
	0,
	0, 0, 0,
	'h',						/* (37) % Channel halfops */
	'b',						/* (38) & bans */
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
	{NULL}, {NULL}, {NULL},	 /* BCD */
	{NULL}, {NULL}, {NULL},	 /* EFG */
	{NULL},					 /* H */
	{add_invite, del_invite},   /* I */
	{NULL},					 /* J */
	{NULL}, {NULL}, {NULL},	 /* KLM */
	{NULL}, {NULL}, {NULL},	 /* NOP */
	{NULL}, {NULL}, {NULL},	 /* QRS */
	{NULL}, {NULL}, {NULL},	 /* TUV */
	{NULL}, {NULL}, {NULL},	 /* WXY */
	{NULL},					 /* Z */
	{NULL}, {NULL},			 /* (char 91 - 92) */
	{NULL}, {NULL}, {NULL},	 /* (char 93 - 95) */
	{NULL},					 /* ` (char 96) */
	{NULL},					 /* a (char 97) */
	{add_ban, del_ban},		 /* b */
	{NULL}, {NULL},			 /* cd */
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
	{0},						/* B */
	{CMODE_C, 0, NULL, NULL},   /* C */
	{0},						/* D */
	{0},						/* E */
	{0},						/* F */
	{CMODE_G, 0, NULL, NULL},   /* G */
	{0},						/* H */
	{0},						/* I */
	{0},						/* J */
	{CMODE_K, 0, NULL, NULL},   /* K */
	{CMODE_L, 0, set_redirect, cs_set_redirect},
	{CMODE_M, 0, NULL, NULL},   /* M */
	{CMODE_N, 0, NULL, NULL},   /* N */
	{CMODE_O, CBM_NO_USER_MLOCK, NULL, NULL},
	{0},						/* P */
	{CMODE_Q, 0, NULL, NULL},   /* Q */
	{CMODE_R, 0, NULL, NULL},   /* R */
	{CMODE_S, 0, NULL, NULL},   /* S */
	{CMODE_T, 0, NULL, NULL},   /* T */
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
	{CMODE_f, 0, set_flood, cs_set_flood},
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


/* svswatch
 * parv[0] - sender
 * parv[1] - target nick
 * parv[2] - parameters
 */
void unreal_cmd_svswatch(const char *sender, const char *nick, const char *parm)
{
	send_cmd(sender, "Bw %s :%s", nick, parm);
}

void unreal_cmd_netinfo(int ac, const char **av)
{
	send_cmd(NULL, "AO %ld %ld %d %s 0 0 0 :%s", static_cast<long>(maxusercnt), static_cast<long>(time(NULL)), atoi(av[2]), av[3], av[7]);
}
/* PROTOCTL */
/*
   NICKv2 = Nick Version 2
   VHP	= Sends hidden host
   UMODE2 = sends UMODE2 on user modes
   NICKIP = Sends IP on NICK
   TOKEN  = Use tokens to talk
   SJ3	= Supports SJOIN
   NOQUIT = No Quit
   TKLEXT = Extended TKL we don't use it but best to have it
   SJB64  = Base64 encoded time stamps
   VL	 = Version Info
   NS	 = Numeric Server

*/
void unreal_cmd_capab()
{
	if (Numeric)
	{
		send_cmd(NULL, "PROTOCTL NICKv2 VHP UMODE2 NICKIP TOKEN SJOIN SJOIN2 SJ3 NOQUIT TKLEXT SJB64 VL");
	}
	else
	{
		send_cmd(NULL, "PROTOCTL NICKv2 VHP UMODE2 NICKIP TOKEN SJOIN SJOIN2 SJ3 NOQUIT TKLEXT SJB64");
	}
}

/* PASS */
void unreal_cmd_pass(const char *pass)
{
	send_cmd(NULL, "PASS :%s", pass);
}

/* CHGHOST */
void unreal_cmd_chghost(const char *nick, const char *vhost)
{
	if (!nick || !vhost) {
		return;
	}
	send_cmd(ServerName, "AL %s %s", nick, vhost);
}

/* CHGIDENT */
void unreal_cmd_chgident(const char *nick, const char *vIdent)
{
	if (!nick || !vIdent) {
		return;
	}
	send_cmd(ServerName, "AZ %s %s", nick, vIdent);
}



class UnrealIRCdProto : public IRCDProto
{
	void ProcessUsermodes(User *user, int ac, const char **av)
	{
		int add = 1; /* 1 if adding modes, 0 if deleting */
		const char *modes = av[0];
		--ac;
		if (!user || !modes) return; /* Prevent NULLs from doing bad things */
		if (debug) alog("debug: Changing mode for %s to %s", user->nick, modes);

		while (*modes) {
			uint32 backup = user->mode;

			/* This looks better, much better than "add ? (do_add) : (do_remove)".
			 * At least this is readable without paying much attention :) -GD */
			if (add)
				user->mode |= umodes[static_cast<int>(*modes)];
			else
				user->mode &= ~umodes[static_cast<int>(*modes)];

			switch (*modes++)
			{
				case '+':
					add = 1;
					break;
				case '-':
					add = 0;
					break;
				case 'd':
					if (ac <= 0)
						break;
					if (isdigit(*av[1]))
					{
						//user->svid = strtoul(av[1], NULL, 0);
						user->mode = backup;  /* Ugly fix, but should do the job ~ Viper */
						continue; // +d was setting a service stamp, ignore the usermode +-d.
					}
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
					if (add && !nick_identified(user)) {
						common_svsmode(user, "-r", NULL);
						user->mode &= ~UMODE_r;
					}
					break;
				case 'x':
					update_host(user);
					break;
				default:
					break;
			}
		}
	}


	/* SVSNOOP */
	void SendSVSNOOP(const char *server, int set)
	{
		send_cmd(NULL, "f %s %s", server, set ? "+" : "-");
	}

	void SendAkillDel(const char *user, const char *host)
	{
		send_cmd(NULL, "BD - G %s %s %s", user, host, s_OperServ);
	}

	void SendTopic(BotInfo *whosets, const char *chan, const char *whosetit, const char *topic, time_t when)
	{
		send_cmd(whosets->nick, ") %s %s %lu :%s", chan, whosetit, static_cast<unsigned long>(when), topic);
	}

	void SendVhostDel(User *u)
	{
		send_cmd(s_HostServ, "v %s -xt", u->nick);
		send_cmd(s_HostServ, "v %s +x", u->nick);
		notice_lang(s_HostServ, u, HOST_OFF);
	}

	void SendAkill(const char *user, const char *host, const char *who, time_t when, time_t expires, const char *reason)
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = expires - time(NULL);
		if (timeleft > 172800) timeleft = 172800;
		send_cmd(NULL, "BD + G %s %s %s %ld %ld :%s", user, host, who, static_cast<long>(time(NULL) + timeleft), static_cast<long>(when), reason);
	}

	/*
	** svskill
	**	parv[0] = servername
	**	parv[1] = client
	**	parv[2] = kill message
	*/
	void SendSVSKillInternal(const char *source, const char *user, const char *buf)
	{
		if (!source || !user || !buf) return;
		send_cmd(source, "h %s :%s", user, buf);
	}

	/*
	 * m_svsmode() added by taz
	 * parv[0] - sender
	 * parv[1] - username to change mode for
	 * parv[2] - modes to change
	 * parv[3] - Service Stamp (if mode == d)
	 */
	void SendSVSMode(User *u, int ac, const char **av)
	{
		if (ac >= 1) {
			if (!u || !av[0]) return;
			send_cmd(ServerName, "v %s %s", u->nick, merge_args(ac, av));
		}
	}

	void SendModeInternal(BotInfo *source, const char *dest, const char *buf)
	{
		if (!buf) return;
		send_cmd(source->nick, "G %s %s", dest, buf);
	}

	void SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes, const char *uid)
	{
		EnforceQlinedNick(nick, s_BotServ);
		send_cmd(NULL, "& %s 1 %ld %s %s %s 0 %s %s%s :%s", nick, static_cast<long>(time(NULL)), user, host, ServerName, modes, host,
			myIrcd->nickip ? " *" : " ", real);
		SendSQLine(nick, "Reserved for services");
	}

	void SendKickInternal(BotInfo *source, const char *chan, const char *user, const char *buf)
	{
		if (buf) send_cmd(source->nick, "H %s %s :%s", chan, user, buf);
		else send_cmd(source->nick, "H %s %s", chan, user);
	}

	void SendNoticeChanopsInternal(BotInfo *source, const char *dest, const char *buf)
	{
		if (!buf) return;
		send_cmd(source->nick, "B @%s :%s", dest, buf);
	}

	void SendBotOp(const char *nick, const char *chan)
	{
		SendMode(findbot(nick), chan, "%s %s %s", myIrcd->botchanumode, nick, nick);
	}

	/* SERVER name hop descript */
	/* Unreal 3.2 actually sends some info about itself in the descript area */
	void SendServer(Server *server)
	{
		if (Numeric)
			send_cmd(NULL, "SERVER %s %d :U0-*-%s %s", server->name, server->hops, Numeric, server->desc);
		else
			send_cmd(NULL, "SERVER %s %d :%s", server->name, server->hops, server->desc);
	}

	/* JOIN */
	void SendJoin(BotInfo *user, const char *channel, time_t chantime)
	{
		send_cmd(ServerName, "~ !%s %s :%s", base64enc(static_cast<long>(chantime)), channel, user->nick);
	}

	/* unsqline
	**	parv[0] = sender
	**	parv[1] = nickmask
	*/
	void SendSQLineDel(const char *user)
	{
		if (!user) return;
		send_cmd(NULL, "d %s", user);
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
	void SendSQLine(const char *mask, const char *reason)
	{
		if (!mask || !reason) return;
		send_cmd(NULL, "c %s :%s", mask, reason);
	}

	/*
	** svso
	**	  parv[0] = sender prefix
	**	  parv[1] = nick
	**	  parv[2] = options
	*/
	void SendSVSO(const char *source, const char *nick, const char *flag)
	{
		if (!source || !nick || !flag) return;
		send_cmd(source, "BB %s %s", nick, flag);
	}

	/* NICK <newnick>  */
	void SendChangeBotNick(BotInfo *oldnick, const char *newnick)
	{
		if (!oldnick || !newnick) return;
		send_cmd(oldnick->nick, "& %s %ld", newnick, static_cast<long>(time(NULL)));
	}

	/* Functions that use serval cmd functions */

	void SendVhost(const char *nick, const char *vIdent, const char *vhost)
	{
		if (!nick) return;
		if (vIdent) unreal_cmd_chgident(nick, vIdent);
		unreal_cmd_chghost(nick, vhost);
	}

	void SendConnect()
	{
		unreal_cmd_capab();
		unreal_cmd_pass(uplink_server->password);
		if (Numeric)
			me_server = new_server(NULL, ServerName, ServerDesc, SERVER_ISME, Numeric);
		else
			me_server = new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);
		SendServer(me_server);
	}

	/* SVSHOLD - set */
	void SendSVSHold(const char *nick)
	{
		send_cmd(NULL, "BD + Q H %s %s %ld %ld :%s", nick, ServerName, static_cast<long>(time(NULL) + NSReleaseTimeout),
			static_cast<long>(time(NULL)), "Being held for registered user");
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const char *nick)
	{
		send_cmd(NULL, "BD - Q * %s %s", nick, ServerName);
	}

	/* UNSGLINE */
	/*
	 * SVSNLINE - :realname mask
	*/
	void SendSGLineDel(const char *mask)
	{
		send_cmd(NULL, "BR - :%s", mask);
	}

	/* UNSZLINE */
	void SendSZLineDel(const char *mask)
	{
		send_cmd(NULL, "BD - Z * %s %s", mask, s_OperServ);
	}

	/* SZLINE */
	void SendSZLine(const char *mask, const char *reason, const char *whom)
	{
		send_cmd(NULL, "BD + Z * %s %s %ld %ld :%s", mask, whom, static_cast<long>(time(NULL) + 172800), static_cast<long>(time(NULL)), reason);
	}

	/* SGLINE */
	/*
	 * SVSNLINE + reason_where_is_space :realname mask with spaces
	*/
	void SendSGLine(const char *mask, const char *reason)
	{
		char edited_reason[BUFSIZE];
		strlcpy(edited_reason, reason, BUFSIZE);
		strnrepl(edited_reason, BUFSIZE, " ", "_");
		send_cmd(NULL, "BR + %s :%s", edited_reason, mask);
	}

	/* SVSMODE -b */
	void SendBanDel(const char *name, const char *nick)
	{
		SendSVSModeChan(name, "-b", nick);
	}


	/* SVSMODE channel modes */

	void SendSVSModeChan(const char *name, const char *mode, const char *nick)
	{
		if (nick) send_cmd(ServerName, "n %s %s %s", name, mode, nick);
		else send_cmd(ServerName, "n %s %s", name, mode);
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
	void SendSVSJoin(const char *source, const char *nick, const char *chan, const char *param)
	{
		if (param) send_cmd(source, "BX %s %s :%s", nick, chan, param);
		else send_cmd(source, "BX %s :%s", nick, chan);
	}

	/* svspart
		parv[0] - sender
		parv[1] - nick to make part
		parv[2] - channel(s) to part
	*/
	void SendSVSPart(const char *source, const char *nick, const char *chan)
	{
		send_cmd(source, "BT %s :%s", nick, chan);
	}

	void SendSWhois(const char *source, const char *who, const char *mask)
	{
		send_cmd(source, "BA %s :%s", who, mask);
	}

	void SendEOB()
	{
		send_cmd(ServerName, "ES");
	}


	/* check if +f mode is valid for the ircd */
	/* borrowed part of the new check from channels.c in Unreal */
	int IsFloodModeParamValid(const char *value)
	{
		char *dp, *end;
		/* NEW +F */
		char xbuf[256], *p, *p2, *x = xbuf + 1;
		int v;
		if (!value) return 0;
		if (*value != ':' && strtoul((*value == '*' ? value + 1 : value), &dp, 10) > 0 && *dp == ':' && *(++dp) && strtoul(dp, &end, 10) > 0 && !*end) return 1;
		else {
			/* '['<number><1 letter>[optional: '#'+1 letter],[next..]']'':'<number> */
			strncpy(xbuf, value, sizeof(xbuf));
			p2 = strchr(xbuf + 1, ']');
			if (!p2) return 0;
			*p2 = '\0';
			if (*(p2 + 1) != ':') return 0;
			for (x = strtok(xbuf + 1, ","); x; x = strtok(NULL, ",")) {
				/* <number><1 letter>[optional: '#'+1 letter] */
				p = x;
				while (isdigit(*p)) ++p;
				if (!*p || !(*p == 'c' || *p == 'j' || *p == 'k' || *p == 'm' || *p == 'n' || *p == 't')) continue; /* continue instead of break for forward compatability. */
				*p = '\0';
				v = atoi(x);
				if (v < 1 || v > 999) return 0;
				++p;
			}
			return 1;
		}
	}

	/*
	  1 = valid nick
	  0 = nick is in valid
	*/
	int IsNickValid(const char *nick)
	{
		if (!stricmp("ircd", nick) || !stricmp("irc", nick))
			return 0;
		return 1;
	}

	int IsChannelValid(const char *chan)
	{
		if (strchr(chan, ':') || *chan != '#') return 0;
		return 1;
	}

	void SetAutoIdentificationToken(User *u)
	{
		int *c;
		char svidbuf[15];

		if (!u->nc)
			return;

		srand(time(NULL));
		snprintf(svidbuf, sizeof(svidbuf), "%i", rand());

		if (u->nc->GetExt("authenticationtoken", c))
		{
			delete [] c;
			u->nc->Shrink("authenticationtoken");
		}

		u->nc->Extend("authenticationtoken", sstrdup(svidbuf));

		common_svsmode(u, "+rd", svidbuf);
	}

} ircd_proto;



/* Event: PROTOCTL */
int anope_event_capab(const char *source, int ac, const char **av)
{
	capab_parse(ac, av);
	return MOD_CONT;
}

/* Events */
int anope_event_ping(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	ircdproto->SendPong(ac > 1 ? av[1] : ServerName, av[0]);
	return MOD_CONT;
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
int anope_event_netinfo(const char *source, int ac, const char **av)
{
	unreal_cmd_netinfo(ac, av);
	return MOD_CONT;
}

int anope_event_eos(const char *source, int ac, const char **av)
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

int anope_event_436(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;

	m_nickcoll(av[0]);
	return MOD_CONT;
}

/*
** away
**	  parv[0] = sender prefix
**	  parv[1] = away message
*/
int anope_event_away(const char *source, int ac, const char **av)
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

/* Unreal sends USER modes with this */
/*
	umode2
	parv[0] - sender
	parv[1] - modes to change
*/
int anope_event_umode2(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;

	const char *newav[4];
	newav[0] = source;
	newav[1] = av[0];
	do_umode(source, ac, newav);
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
	if (ac != 1)
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

	u = finduser(av[0]);
	if (!u) {
		if (debug) {
			alog("debug: CHGNAME for nonexistent user %s", av[0]);
		}
		return MOD_CONT;
	}

	u->SetRealname(av[1]);
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

/*
** NICK - new
**	  source  = NULL
**	parv[0] = nickname
**	  parv[1] = hopcount
**	  parv[2] = timestamp
**	  parv[3] = username
**	  parv[4] = hostname
**	  parv[5] = servername
**  if NICK version 1:
**	  parv[6] = servicestamp
**	parv[7] = info
**  if NICK version 2:
**	parv[6] = servicestamp
**	  parv[7] = umodes
**	parv[8] = virthost, * if none
**	parv[9] = info
**  if NICKIP:
**	  parv[9] = ip
**	  parv[10] = info
**
** NICK - change
**	  source  = oldnick
**	parv[0] = new nickname
**	  parv[1] = hopcount
*/
/*
 do_nick(const char *source, char *nick, char *username, char *host,
			  char *server, char *realname, time_t ts,
			  uint32 ip, char *vhost, char *uid)
*/
int anope_event_nick(const char *source, int ac, const char **av)
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
					strtoul(av[2], NULL, 10), 0, "*", NULL);

		} else if (ac == 11) {
			user = do_nick(source, av[0], av[3], av[4], av[5], av[10],
						   strtoul(av[2], NULL, 10), ntohl(decode_ip(av[9])), av[8], NULL);
			if (user)
			{
				/* Check to see if the user should be identified because their
				 * services id matches the one in their nickcore
				 */
				user->CheckAuthenticationToken(av[6]);

				ircdproto->ProcessUsermodes(user, 1, &av[7]);
			}

		} else {
			/* NON NICKIP */
			user = do_nick(source, av[0], av[3], av[4], av[5], av[9],
						   strtoul(av[2], NULL, 10), 0, av[8],
						   NULL);
			if (user)
			{
				/* Check to see if the user should be identified because their
				 * services id matches the one in their nickcore
				 */
				user->CheckAuthenticationToken(av[6]);

				ircdproto->ProcessUsermodes(user, 1, &av[7]);
			}
		}
	} else {
		do_nick(source, av[0], NULL, NULL, NULL, NULL,
				strtoul(av[1], NULL, 10), 0, NULL, NULL);
	}
	return MOD_CONT;
}


int anope_event_chghost(const char *source, int ac, const char **av)
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

	u->SetDisplayedHost(av[1]);
	return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(const char *source, int ac, const char **av)
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
		delete [] vl;
		delete [] desc;
		delete [] upnumeric;
	} else {
		do_server(source, av[0], av[1], av[2], NULL);
	}

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


int anope_event_error(const char *source, int ac, const char **av)
{
	if (av[0]) {
		if (debug) {
			alog("debug: %s", av[0]);
		}
	if(strstr(av[0],"No matching link configuration")!=0) {
		alog("Error: Your IRCD's link block may not setup correctly, please check unrealircd.conf");
	}
	}
	return MOD_CONT;
}

int anope_event_sdesc(const char *source, int ac, const char **av)
{
	Server *s;
	s = findserver(servlist, source);

	if (s) {
		s->desc = const_cast<char *>(av[0]); // XXX Unsafe cast -- CyberBotX
	}

	return MOD_CONT;
}

int anope_event_sjoin(const char *source, int ac, const char **av)
{
	do_sjoin(source, ac, av);
	return MOD_CONT;
}

void moduleAddIRCDMsgs() {
	Message *m;

	updateProtectDetails("PROTECT","PROTECTME","protect","deprotect","AUTOPROTECT","+a","-a");
	updateOwnerDetails("OWNER", "DEOWNER", ircd->ownerset, ircd->ownerunset);

	m = createMessage("436",	   anope_event_436); addCoreMessage(IRCD,m);
	m = createMessage("AWAY",	  anope_event_away); addCoreMessage(IRCD,m);
	m = createMessage("6",		anope_event_away); addCoreMessage(IRCD,m);
	m = createMessage("JOIN",	  anope_event_join); addCoreMessage(IRCD,m);
	m = createMessage("C",		anope_event_join); addCoreMessage(IRCD,m);
	m = createMessage("KICK",	  anope_event_kick); addCoreMessage(IRCD,m);
	m = createMessage("H",		anope_event_kick); addCoreMessage(IRCD,m);
	m = createMessage("KILL",	  anope_event_kill); addCoreMessage(IRCD,m);
	m = createMessage(".",		anope_event_kill); addCoreMessage(IRCD,m);
	m = createMessage("MODE",	  anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("G",		anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("MOTD",	  anope_event_motd); addCoreMessage(IRCD,m);
	m = createMessage("F",		anope_event_motd); addCoreMessage(IRCD,m);
	m = createMessage("NICK",	  anope_event_nick); addCoreMessage(IRCD,m);
	m = createMessage("&",		anope_event_nick); addCoreMessage(IRCD,m);
	m = createMessage("PART",	  anope_event_part); addCoreMessage(IRCD,m);
	m = createMessage("D",		anope_event_part); addCoreMessage(IRCD,m);
	m = createMessage("PING",	  anope_event_ping); addCoreMessage(IRCD,m);
	m = createMessage("8",		anope_event_ping); addCoreMessage(IRCD,m);
	m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
	m = createMessage("!",		anope_event_privmsg); addCoreMessage(IRCD,m);
	m = createMessage("QUIT",	  anope_event_quit); addCoreMessage(IRCD,m);
	m = createMessage(",",		anope_event_quit); addCoreMessage(IRCD,m);
	m = createMessage("SERVER",	anope_event_server); addCoreMessage(IRCD,m);
	m = createMessage("'",		anope_event_server); addCoreMessage(IRCD,m);
	m = createMessage("SQUIT",	 anope_event_squit); addCoreMessage(IRCD,m);
	m = createMessage("-",		anope_event_squit); addCoreMessage(IRCD,m);
	m = createMessage("TOPIC",	 anope_event_topic); addCoreMessage(IRCD,m);
	m = createMessage(")",		anope_event_topic); addCoreMessage(IRCD,m);
	m = createMessage("SVSMODE",   anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("n",		anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("SVS2MODE",   anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("v",	   anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("WHOIS",	 anope_event_whois); addCoreMessage(IRCD,m);
	m = createMessage("#",		anope_event_whois); addCoreMessage(IRCD,m);
	m = createMessage("PROTOCTL",  anope_event_capab); addCoreMessage(IRCD,m);
	m = createMessage("_",		anope_event_capab); addCoreMessage(IRCD,m);
	m = createMessage("CHGHOST",   anope_event_chghost); addCoreMessage(IRCD,m);
	m = createMessage("AL",		anope_event_chghost); addCoreMessage(IRCD,m);
	m = createMessage("CHGIDENT",  anope_event_chgident); addCoreMessage(IRCD,m);
	m = createMessage("AZ",		anope_event_chgident); addCoreMessage(IRCD,m);
	m = createMessage("CHGNAME",   anope_event_chgname); addCoreMessage(IRCD,m);
	m = createMessage("BK",		anope_event_chgname); addCoreMessage(IRCD,m);
	m = createMessage("NETINFO",   anope_event_netinfo); addCoreMessage(IRCD,m);
	m = createMessage("AO",	   anope_event_netinfo); addCoreMessage(IRCD,m);
	m = createMessage("SETHOST",   anope_event_sethost); addCoreMessage(IRCD,m);
	m = createMessage("AA",		anope_event_sethost); addCoreMessage(IRCD,m);
	m = createMessage("SETIDENT",  anope_event_setident); addCoreMessage(IRCD,m);
	m = createMessage("AD",		anope_event_setident); addCoreMessage(IRCD,m);
	m = createMessage("SETNAME",   anope_event_setname); addCoreMessage(IRCD,m);
	m = createMessage("AE",		anope_event_setname); addCoreMessage(IRCD,m);
	m = createMessage("EOS", 	   anope_event_eos); addCoreMessage(IRCD,m);
	m = createMessage("ES",	   anope_event_eos); addCoreMessage(IRCD,m);
	m = createMessage("ERROR", 	   anope_event_error); addCoreMessage(IRCD,m);
	m = createMessage("5",		anope_event_error); addCoreMessage(IRCD,m);
	m = createMessage("UMODE2",	anope_event_umode2); addCoreMessage(IRCD,m);
	m = createMessage("|",		anope_event_umode2); addCoreMessage(IRCD,m);
	m = createMessage("SJOIN",	  anope_event_sjoin); addCoreMessage(IRCD,m);
	m = createMessage("~",		anope_event_sjoin); addCoreMessage(IRCD,m);
	m = createMessage("SDESC",	  anope_event_sdesc); addCoreMessage(IRCD,m);
	m = createMessage("AG",	   anope_event_sdesc); addCoreMessage(IRCD,m);

	/* The non token version of these is in messages.c */
	m = createMessage("2",		 m_stats); addCoreMessage(IRCD,m);
	m = createMessage(">",		 m_time); addCoreMessage(IRCD,m);
	m = createMessage("+",		 m_version); addCoreMessage(IRCD,m);
}


class ProtoUnreal : public Module
{
 public:
	ProtoUnreal(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(PROTOCOL);

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

		pmodule_ircd_proto(&ircd_proto);
		moduleAddIRCDMsgs();
		
		ModuleManager::Attach(I_OnDelCore, this);
	}

	void OnDelCore(NickCore *nc)
	{
		char *c;

		if (nc->GetExt("authenticationtoken", c))
		{
			delete [] c;
			nc->Shrink("authenticationtoken");
		}
	}

};

MODULE_INIT("unreal32", ProtoUnreal)
