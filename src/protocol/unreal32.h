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

void unreal_set_umode(User * user, int ac, const char **av);
void unreal_cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when);
void unreal_cmd_vhost_off(User * u);
void unreal_cmd_akill(const char *user, const char *host, const char *who, time_t when,time_t expires, const char *reason);
void unreal_cmd_svskill(const char *source, const char *user, const char *buf);
void unreal_cmd_svsmode(User * u, int ac, const char **av);
void unreal_cmd_372(const char *source, const char *msg);
void unreal_cmd_372_error(const char *source);
void unreal_cmd_375(const char *source);
void unreal_cmd_376(const char *source);
void unreal_cmd_nick(const char *nick, const char *name, const char *modes);
void unreal_cmd_guest_nick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void unreal_cmd_mode(const char *source, const char *dest, const char *buf);
void unreal_cmd_bot_nick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void unreal_cmd_kick(const char *source, const char *chan, const char *user, const char *buf);
void unreal_cmd_notice_ops(const char *source, const char *dest, const char *buf);
void unreal_cmd_notice(const char *source, const char *dest, const char *buf);
void unreal_cmd_notice2(const char *source, const char *dest, const char *msg);
void unreal_cmd_privmsg(const char *source, const char *dest, const char *buf);
void unreal_cmd_serv_notice(const char *source, const char *dest, const char *msg);
void unreal_cmd_serv_privmsg(const char *source, const char *dest, const char *msg);
void unreal_cmd_bot_chan_mode(const char *nick, const char *chan);
void unreal_cmd_351(const char *source);
void unreal_cmd_quit(const char *source, const char *buf);
void unreal_cmd_pong(const char *servname, const char *who);
void unreal_cmd_join(const char *user, const char *channel, time_t chantime);
void unreal_cmd_unsqline(const char *user);
void unreal_cmd_invite(const char *source, const char *chan, const char *nick);
void unreal_cmd_part(const char *nick, const char *chan, const char *buf);
void unreal_cmd_391(const char *source, const char *timestr);
void unreal_cmd_250(const char *buf);
void unreal_cmd_307(const char *buf);
void unreal_cmd_311(const char *buf);
void unreal_cmd_312(const char *buf);
void unreal_cmd_317(const char *buf);
void unreal_cmd_219(const char *source, const char *letter);
void unreal_cmd_401(const char *source, const char *who);
void unreal_cmd_318(const char *source, const char *who);
void unreal_cmd_242(const char *buf);
void unreal_cmd_243(const char *buf);
void unreal_cmd_211(const char *buf);
void unreal_cmd_global(const char *source, const char *buf);
void unreal_cmd_global_legacy(const char *source, const char *fmt);
void unreal_cmd_sqline(const char *mask, const char *reason);
void unreal_cmd_squit(const char *servname, const char *message);
void unreal_cmd_svso(const char *source, const char *nick, const char *flag);
void unreal_cmd_chg_nick(const char *oldnick, const char *newnick);
void unreal_cmd_svsnick(const char *source, const char *guest, time_t when);
void unreal_cmd_vhost_on(const char *nick, const char *vIdent, const char *vhost);
void unreal_cmd_connect(int servernum);
void unreal_cmd_svshold(const char *nick);
void unreal_cmd_release_svshold(const char *nick);
void unreal_cmd_unsgline(const char *mask);
void unreal_cmd_unszline(const char *mask);
void unreal_cmd_szline(const char *mask, const char *reason, const char *whom);
void unreal_cmd_sgline(const char *mask, const char *reason);
void unreal_cmd_unban(const char *name, const char *nick);
void unreal_cmd_svsmode_chan(const char *name, const char *mode, const char *nick);
void unreal_cmd_svid_umode(const char *nick, time_t ts);
void unreal_cmd_nc_change(User * u);
void unreal_cmd_svid_umode2(User * u, const char *ts);
void unreal_cmd_svid_umode3(User * u, const char *ts);
void unreal_cmd_eob();
int unreal_flood_mode_check(const char *value);
void unreal_cmd_jupe(const char *jserver, const char *who, const char *reason);
int unreal_valid_nick(const char *nick);
void unreal_cmd_ctcp(const char *source, const char *dest, const char *buf);

class UnrealIRCdProto : public IRCDProtoNew {
	public:
		void cmd_svsnoop(const char *, int);
		void cmd_remove_akill(const char *, const char *);
} ircd_proto;
