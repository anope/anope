/* SolidIRCD functions
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

/*************************************************************************/

#define UMODE_a 0x00000001  /* umode +a - Services Admin */
#define UMODE_h 0x00000002  /* umode +h - Helper */
#define UMODE_i 0x00000004  /* umode +i - Invisible */
#define UMODE_o 0x00000008  /* umode +o - Oper */
#define UMODE_r 0x00000010  /* umode +r - registered nick */
#define UMODE_w 0x00000020  /* umode +w - Get wallops */
#define UMODE_A 0x00000040  /* umode +A - Server Admin */
#define UMODE_x 0x00000080  /* umode +x - Squelch with notice */
#define UMODE_X 0x00000100  /* umode +X - Squelch without notice */
#define UMODE_F 0x00000200  /* umode +F - no cptr->since message rate throttle */
#define UMODE_j 0x00000400  /* umode +j - client rejection notices */
#define UMODE_K 0x00000800  /* umode +K - U: lined server kill messages */
#define UMODE_O 0x00001000  /* umode +O - Local Oper */
#define UMODE_s 0x00002000  /* umode +s - Server notices */
#define UMODE_c 0x00004000  /* umode +c - Client connections/exits */
#define UMODE_k 0x00008000  /* umode +k - Server kill messages */
#define UMODE_f 0x00010000  /* umode +f - Server flood messages */
#define UMODE_y 0x00020000  /* umode +y - Stats/links */
#define UMODE_d 0x00040000  /* umode +d - Debug info */
#define UMODE_g 0x00080000  /* umode +g - Globops */
#define UMODE_b 0x00100000  /* umode +b - Chatops */
#define UMODE_n 0x00200000  /* umode +n - Routing Notices */
#define UMODE_m 0x00400000  /* umode +m - spambot notices */
#define UMODE_e 0x00800000  /* umode +e - oper notices for the above +D */
#define UMODE_D 0x01000000  /* umode +D - Hidden dccallow umode */
#define UMODE_I 0x02000000  /* umode +I - invisible oper (masked) */
#define UMODE_C 0x04000000  /* umode +C - conops */
#define UMODE_v 0x10000000	/* umode +v - hostmasking */
#define UMODE_H 0x20000000  /* umode +H - Oper Hiding */
#define UMODE_z 0x40000000  /* umode +z - SSL */
#define UMODE_R 0x80000000  /* umode +R - No non registered msgs */

#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040	/* These two used only by ChanServ */
#define CMODE_l 0x00000080
#define CMODE_R 0x00000100	/* Only identified users can join */
#define CMODE_r 0x00000200	/* Set for all registered channels */
#define CMODE_c 0x00000400	/* Colors can't be used */
#define CMODE_M 0x00000800      /* Non-regged nicks can't send messages */
#define CMODE_j 0x00001000      /* join throttle */
#define CMODE_S 0x00002000      /* SSL only */
#define CMODE_N 0x00004000     /* No Nickname change */
#define CMODE_O 0x00008000	/* Only opers can join */


#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

void solidircd_set_umode(User * user, int ac, const char **av);
void solidircd_cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when);
void solidircd_SendVhostDel(User * u);
void solidircd_SendAkill(const char *user, const char *host, const char *who, time_t when,time_t expires, const char *reason);
void solidircd_SendSVSKill(const char *source, const char *user, const char *buf);
void solidircd_SendSVSMode(User * u, int ac, const char **av);
void solidircd_cmd_372(const char *source, const char *msg);
void solidircd_cmd_372_error(const char *source);
void solidircd_cmd_375(const char *source);
void solidircd_cmd_376(const char *source);
void solidircd_cmd_nick(const char *nick, const char *name, const char *modes);
void solidircd_SendGuestNick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void solidircd_SendMode(const char *source, const char *dest, const char *buf);
void solidircd_SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void solidircd_SendKick(const char *source, const char *chan, const char *user, const char *buf);
void solidircd_SendNoticeChanops(const char *source, const char *dest, const char *buf);
void solidircd_cmd_notice(const char *source, const char *dest, const char *buf);
void solidircd_cmd_notice2(const char *source, const char *dest, const char *msg);
void solidircd_cmd_privmsg(const char *source, const char *dest, const char *buf);
void solidircd_cmd_privmsg2(const char *source, const char *dest, const char *msg);
void solidircd_SendGlobalNotice(const char *source, const char *dest, const char *msg);
void solidircd_SendGlobalPrivmsg(const char *source, const char *dest, const char *msg);
void solidircd_SendBotOp(const char *nick, const char *chan);
void solidircd_cmd_351(const char *source);
void solidircd_SendQuit(const char *source, const char *buf);
void solidircd_cmd_pong(const char *servname, const char *who);
void solidircd_cmd_join(const char *user, const char *channel, time_t chantime);
void solidircd_cmd_unsqline(const char *user);
void solidircd_cmd_invite(const char *source, const char *chan, const char *nick);
void solidircd_cmd_part(const char *nick, const char *chan, const char *buf);
void solidircd_cmd_391(const char *source, const char *timestr);
void solidircd_cmd_250(const char *buf);
void solidircd_cmd_307(const char *buf);
void solidircd_cmd_311(const char *buf);
void solidircd_cmd_312(const char *buf);
void solidircd_cmd_317(const char *buf);
void solidircd_cmd_219(const char *source, const char *letter);
void solidircd_cmd_401(const char *source, const char *who);
void solidircd_cmd_318(const char *source, const char *who);
void solidircd_cmd_242(const char *buf);
void solidircd_cmd_243(const char *buf);
void solidircd_cmd_211(const char *buf);
void solidircd_cmd_global(const char *source, const char *buf);
void solidircd_cmd_global_legacy(const char *source, const char *fmt);
void solidircd_cmd_sqline(const char *mask, const char *reason);
void solidircd_cmd_squit(const char *servname, const char *message);
void solidircd_cmd_svso(const char *source, const char *nick, const char *flag);
void solidircd_cmd_chg_nick(const char *oldnick, const char *newnick);
void solidircd_cmd_svsnick(const char *source, const char *guest, time_t when);
void solidircd_cmd_vhost_on(const char *nick, const char *vIdent, const char *vhost);
void solidircd_cmd_connect(int servernum);
void solidircd_cmd_svshold(const char *nick);
void solidircd_cmd_release_svshold(const char *nick);
void solidircd_cmd_unsgline(const char *mask);
void solidircd_cmd_unszline(const char *mask);
void solidircd_cmd_szline(const char *mask, const char *reason, const char *whom);
void solidircd_cmd_sgline(const char *mask, const char *reason);
void solidircd_cmd_unban(const char *name, const char *nick);
void solidircd_SendSVSMode_chan(const char *name, const char *mode, const char *nick);
void solidircd_cmd_svid_umode(const char *nick, time_t ts);
void solidircd_cmd_nc_change(User * u);
void solidircd_cmd_svid_umode2(User * u, const char *ts);
void solidircd_cmd_svid_umode3(User * u, const char *ts);
void solidircd_cmd_eob();
int solidircd_flood_mode_check(const char *value);
void solidircd_cmd_jupe(const char *jserver, const char *who, const char *reason);
int solidircd_valid_nick(const char *nick);
void solidircd_cmd_ctcp(const char *source, const char *dest, const char *buf);

class SolidIRCdProto : public IRCDProtoNew {
	public:
		void SendSVSNOOP(const char *, int);
		void SendAkillDel(const char *, const char *);
} ircd_proto;
