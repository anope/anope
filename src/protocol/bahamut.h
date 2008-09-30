/* Bahamut functions
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
#define UMODE_R 0x80000000  /* unmode +R - No non registered msgs */

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
#define CMODE_O 0x00008000	/* Only opers can join */

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

void bahamut_set_umode(User * user, int ac, const char **av);
void bahamut_cmd_372(const char *source, const char *msg);
void bahamut_cmd_372_error(const char *source);
void bahamut_cmd_375(const char *source);
void bahamut_cmd_376(const char *source);
void bahamut_cmd_351(const char *source);
void bahamut_cmd_unsqline(const char *user);
void bahamut_cmd_invite(const char *source, const char *chan, const char *nick);
void bahamut_cmd_part(const char *nick, const char *chan, const char *buf);
void bahamut_cmd_391(const char *source, const char *timestr);
void bahamut_cmd_250(const char *buf);
void bahamut_cmd_307(const char *buf);
void bahamut_cmd_311(const char *buf);
void bahamut_cmd_312(const char *buf);
void bahamut_cmd_317(const char *buf);
void bahamut_cmd_219(const char *source, const char *letter);
void bahamut_cmd_401(const char *source, const char *who);
void bahamut_cmd_318(const char *source, const char *who);
void bahamut_cmd_242(const char *buf);
void bahamut_cmd_243(const char *buf);
void bahamut_cmd_211(const char *buf);
void bahamut_cmd_global(const char *source, const char *buf);
void bahamut_cmd_sqline(const char *mask, const char *reason);
void bahamut_cmd_squit(const char *servname, const char *message);
void bahamut_cmd_svso(const char *source, const char *nick, const char *flag);
void bahamut_cmd_chg_nick(const char *oldnick, const char *newnick);
void bahamut_cmd_svsnick(const char *source, const char *guest, time_t when);
void bahamut_cmd_vhost_on(const char *nick, const char *vIdent, const char *vhost);
void bahamut_cmd_connect(int servernum);
void bahamut_cmd_svshold(const char *nick);
void bahamut_cmd_release_svshold(const char *nick);
void bahamut_cmd_unsgline(const char *mask);
void bahamut_cmd_unszline(const char *mask);
void bahamut_cmd_szline(const char *mask, const char *reason, const char *whom);
void bahamut_cmd_sgline(const char *mask, const char *reason);
void bahamut_cmd_unban(const char *name, const char *nick);
void bahamut_cmd_svsmode_chan(const char *name, const char *mode, const char *nick);
void bahamut_cmd_svid_umode(const char *nick, time_t ts);
void bahamut_cmd_nc_change(User * u);
void bahamut_cmd_svid_umode2(User * u, const char *ts);
void bahamut_cmd_svid_umode3(User * u, const char *ts);
void bahamut_cmd_eob();
int bahamut_flood_mode_check(const char *value);
void bahamut_cmd_jupe(const char *jserver, const char *who, const char *reason);
int bahamut_valid_nick(const char *nick);
void bahamut_cmd_ctcp(const char *source, const char *dest, const char *buf);

class BahamutIRCdProto : public IRCDProtoNew {
	public:
		void cmd_svsnoop(const char *, int);
		void cmd_remove_akill(const char *, const char *);
		void cmd_topic(const char *, const char *, const char *, const char *, time_t);
		void cmd_akill(const char *, const char *, const char *, time_t, time_t, const char *);
		void cmd_svskill(const char *, const char *, const char *);
		void cmd_svsmode(User *, int, const char **);
		void cmd_nick(const char *, const char *, const char *);
		void cmd_guest_nick(const char *, const char *, const char *, const char *, const char *);
		void cmd_mode(const char *, const char *, const char *);
		void cmd_bot_nick(const char *, const char *, const char *, const char *, const char *);
		void cmd_kick(const char *, const char *, const char *, const char *);
		void cmd_notice_ops(const char *, const char *, const char *);
		void cmd_bot_chan_mode(const char *, const char *);
		void cmd_join(const char *, const char *, time_t);
} ircd_proto;
