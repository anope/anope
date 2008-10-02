/* Rage IRCD functions
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

#define UMODE_a 0x00000001
#define UMODE_h 0x00000002
#define UMODE_i 0x00000004
#define UMODE_o 0x00000008
#define UMODE_r 0x00000010
#define UMODE_w 0x00000020
#define UMODE_A 0x00000040
#define UMODE_R 0x80000000
#define UMODE_x 0x40000000

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
#define CMODE_c 0x00000400		/* Colors can't be used */
#define CMODE_M 0x00000800      /* Non-regged nicks can't send messages */
#define CMODE_N 0x00001000
#define CMODE_S 0x00002000
#define CMODE_C 0x00004000
#define CMODE_A 0x00008000
#define CMODE_O 0x00010000		/* Only opers can join */

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

void rageircd_set_umode(User * user, int ac, const char **av);
void rageircd_cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when);
void rageircd_SendVhostDel(User * u);
void rageircd_SendAkill(const char *user, const char *host, const char *who, time_t when,time_t expires, const char *reason);
void rageircd_SendSVSKill(const char *source, const char *user, const char *buf);
void rageircd_SendSVSMode(User * u, int ac, const char **av);
void rageircd_cmd_372(const char *source, const char *msg);
void rageircd_cmd_372_error(const char *source);
void rageircd_cmd_375(const char *source);
void rageircd_cmd_376(const char *source);
void rageircd_cmd_nick(const char *nick, const char *name, const char *modes);
void rageircd_SendGuestNick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void rageircd_SendMode(const char *source, const char *dest, const char *buf);
void rageircd_cmd_bot_nick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void rageircd_cmd_kick(const char *source, const char *chan, const char *user, const char *buf);
void rageircd_cmd_notice_ops(const char *source, const char *dest, const char *buf);
void rageircd_cmd_notice(const char *source, const char *dest, const char *buf);
void rageircd_cmd_notice2(const char *source, const char *dest, const char *msg);
void rageircd_cmd_privmsg(const char *source, const char *dest, const char *buf);
void rageircd_cmd_privmsg2(const char *source, const char *dest, const char *msg);
void rageircd_cmd_serv_notice(const char *source, const char *dest, const char *msg);
void rageircd_cmd_serv_privmsg(const char *source, const char *dest, const char *msg);
void rageircd_cmd_bot_chan_mode(const char *nick, const char *chan);
void rageircd_cmd_351(const char *source);
void rageircd_cmd_quit(const char *source, const char *buf);
void rageircd_cmd_pong(const char *servname, const char *who);
void rageircd_cmd_join(const char *user, const char *channel, time_t chantime);
void rageircd_cmd_unsqline(const char *user);
void rageircd_cmd_invite(const char *source, const char *chan, const char *nick);
void rageircd_cmd_part(const char *nick, const char *chan, const char *buf);
void rageircd_cmd_391(const char *source, const char *timestr);
void rageircd_cmd_250(const char *buf);
void rageircd_cmd_307(const char *buf);
void rageircd_cmd_311(const char *buf);
void rageircd_cmd_312(const char *buf);
void rageircd_cmd_317(const char *buf);
void rageircd_cmd_219(const char *source, const char *letter);
void rageircd_cmd_401(const char *source, const char *who);
void rageircd_cmd_318(const char *source, const char *who);
void rageircd_cmd_242(const char *buf);
void rageircd_cmd_243(const char *buf);
void rageircd_cmd_211(const char *buf);
void rageircd_cmd_global(const char *source, const char *buf);
void rageircd_cmd_global_legacy(const char *source, const char *fmt);
void rageircd_cmd_sqline(const char *mask, const char *reason);
void rageircd_cmd_squit(const char *servname, const char *message);
void rageircd_cmd_svso(const char *source, const char *nick, const char *flag);
void rageircd_cmd_chg_nick(const char *oldnick, const char *newnick);
void rageircd_cmd_svsnick(const char *source, const char *guest, time_t when);
void rageircd_cmd_vhost_on(const char *nick, const char *vIdent, const char *vhost);
void rageircd_cmd_connect(int servernum);
void rageircd_cmd_svshold(const char *nick);
void rageircd_cmd_release_svshold(const char *nick);
void rageircd_cmd_unsgline(const char *mask);
void rageircd_cmd_unszline(const char *mask);
void rageircd_cmd_szline(const char *mask, const char *reason, const char *whom);
void rageircd_cmd_sgline(const char *mask, const char *reason);
void rageircd_cmd_unban(const char *name, const char *nick);
void rageircd_SendSVSMode_chan(const char *name, const char *mode, const char *nick);
void rageircd_cmd_svid_umode(const char *nick, time_t ts);
void rageircd_cmd_nc_change(User * u);
void rageircd_cmd_svid_umode2(User * u, const char *ts);
void rageircd_cmd_svid_umode3(User * u, const char *ts);
void rageircd_cmd_eob();
int rageircd_flood_mode_check(const char *value);
void rageircd_cmd_jupe(const char *jserver, const char *who, const char *reason);
int rageircd_valid_nick(const char *nick);
void rageircd_cmd_ctcp(const char *source, const char *dest, const char *buf);

class RageIRCdProto : public IRCDProtoNew {
	public:
		void SendSVSNOOP(const char *, int);
		void SendAkillDel(const char *, const char *);
} ircd_proto;
