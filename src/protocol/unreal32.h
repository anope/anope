/* Unreal IRCD 3.2.x functions
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@unreal.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

/*************************************************************************/

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

class UnrealIRCdProto : public IRCDProto {
	public:
		void SendSVSNOOP(const char *, int);
		void SendAkillDel(const char *, const char *);
		void cmd_topic(const char *, const char *, const char *, const char *, time_t);
		void SendVhostDel(User *);
		void SendAkill(const char *, const char *, const char *, time_t, time_t, const char *);
		void SendSVSKill(const char *, const char *, const char *);
		void SendSVSMode(User *, int, const char **);
		void SendGuestNick(const char *, const char *, const char *, const char *, const char *);
		void SendMode(const char *, const char *, const char *);
		void SendClientIntroduction(const char *, const char *, const char *, const char *, const char *);
		void SendKick(const char *, const char *, const char *, const char *);
		void SendNoticeChanops(const char *, const char *, const char *);
		void SendBotOp(const char *, const char *);
		void SendJoin(const char *, const char *, time_t);
		void SendSQLineDel(const char *);
		void SendSQLine(const char *, const char *);
		void SendSVSO(const char *, const char *, const char *);
		void SendChangeBotNick(const char *, const char *);
		void SendVhost(const char *, const char *, const char *);
		void SendConnect();
		void SendSVSHOLD(const char *);
		void SendSVSHOLDDel(const char *);
		void SendSGLineDel(const char *);
		void SendSZLineDel(const char *);
		void SendSZLine(const char *, const char *, const char *);
		void SendSGLine(const char *, const char *);
		void SendBanDel(const char *, const char *);
		void SendSVSMode_chan(const char *, const char *, const char *);
		void SendSVID(const char *, time_t);
		void SendUnregisteredNick(User *);
		void SendSVID2(User *, const char *);
		void SendSVSJoin(const char *, const char *, const char *, const char *);
		void SendSVSPart(const char *, const char *, const char *);
		void cmd_swhois(const char *, const char *, const char *);
		void cmd_eob();
		void cmd_server(const char *, int, const char *);
		void set_umode(User *, int, const char **);
		int valid_nick(const char *);
		int valid_chan(const char *);
		int flood_mode_check(const char *);
} ircd_proto;
