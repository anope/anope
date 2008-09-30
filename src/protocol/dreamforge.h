/* DreamForge IRCD functions
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

#define UMODE_a 0x00000001  /* Services Admin */
#define UMODE_h 0x00000002  /* Help system operator */
#define UMODE_i 0x00000004  /* makes user invisible */
#define UMODE_o 0x00000008  /* Operator */
#define UMODE_r 0x00000010  /* Nick set by services as registered */
#define UMODE_w 0x00000020  /* send wallops to them */
#define UMODE_A 0x00000040  /* Admin */
#define UMODE_O 0x00000080  /* Local operator */
#define UMODE_s 0x00000100  /* server notices such as kill */
#define UMODE_k 0x00000200  /* Show server-kills... */
#define UMODE_c 0x00000400  /* Show client information */
#define UMODE_f 0x00000800  /* Receive flood warnings */
#define UMODE_g 0x80000000  /* Shows some global messages */

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

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

void dreamforge_set_umode(User * user, int ac, const char **av);
void dreamforge_cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when);
void dreamforge_cmd_vhost_off(User * u);
void dreamforge_cmd_akill(const char *user, const char *host, const char *who, time_t when,time_t expires, const char *reason);
void dreamforge_cmd_svskill(const char *source, const char *user, const char *buf);
void dreamforge_cmd_svsmode(User * u, int ac, const char **av);
void dreamforge_cmd_372(const char *source, const char *msg);
void dreamforge_cmd_372_error(const char *source);
void dreamforge_cmd_375(const char *source);
void dreamforge_cmd_376(const char *source);
void dreamforge_cmd_nick(const char *nick, const char *name, const char *modes);
void dreamforge_cmd_guest_nick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void dreamforge_cmd_mode(const char *source, const char *dest, const char *buf);
void dreamforge_cmd_bot_nick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void dreamforge_cmd_kick(const char *source, const char *chan, const char *user, const char *buf);
void dreamforge_cmd_notice_ops(const char *source, const char *dest, const char *buf);
void dreamforge_cmd_notice(const char *source, const char *dest, const char *buf);
void dreamforge_cmd_notice2(const char *source, const char *dest, const char *msg);
void dreamforge_cmd_privmsg(const char *source, const char *dest, const char *buf);
void dreamforge_cmd_privmsg2(const char *source, const char *dest, const char *msg);
void dreamforge_cmd_serv_notice(const char *source, const char *dest, const char *msg);
void dreamforge_cmd_serv_privmsg(const char *source, const char *dest, const char *msg);
void dreamforge_cmd_bot_chan_mode(const char *nick, const char *chan);
void dreamforge_cmd_351(const char *source);
void dreamforge_cmd_quit(const char *source, const char *buf);
void dreamforge_cmd_pong(const char *servname, const char *who);
void dreamforge_cmd_join(const char *user, const char *channel, time_t chantime);
void dreamforge_cmd_unsqline(const char *user);
void dreamforge_cmd_invite(const char *source, const char *chan, const char *nick);
void dreamforge_cmd_part(const char *nick, const char *chan, const char *buf);
void dreamforge_cmd_391(const char *source, const char *timestr);
void dreamforge_cmd_250(const char *buf);
void dreamforge_cmd_307(const char *buf);
void dreamforge_cmd_311(const char *buf);
void dreamforge_cmd_312(const char *buf);
void dreamforge_cmd_317(const char *buf);
void dreamforge_cmd_219(const char *source, const char *letter);
void dreamforge_cmd_401(const char *source, const char *who);
void dreamforge_cmd_318(const char *source, const char *who);
void dreamforge_cmd_242(const char *buf);
void dreamforge_cmd_243(const char *buf);
void dreamforge_cmd_211(const char *buf);
void dreamforge_cmd_global(const char *source, const char *buf);
void dreamforge_cmd_global_legacy(const char *source, const char *fmt);
void dreamforge_cmd_sqline(const char *mask, const char *reason);
void dreamforge_cmd_squit(const char *servname, const char *message);
void dreamforge_cmd_svso(const char *source, const char *nick, const char *flag);
void dreamforge_cmd_chg_nick(const char *oldnick, const char *newnick);
void dreamforge_cmd_svsnick(const char *source, const char *guest, time_t when);
void dreamforge_cmd_vhost_on(const char *nick, const char *vIdent, const char *vhost);
void dreamforge_cmd_connect(int servernum);
void dreamforge_cmd_svshold(const char *nick);
void dreamforge_cmd_release_svshold(const char *nick);
void dreamforge_cmd_unsgline(const char *mask);
void dreamforge_cmd_unszline(const char *mask);
void dreamforge_cmd_szline(const char *mask, const char *reason, const char *whom);
void dreamforge_cmd_sgline(const char *mask, const char *reason);
void dreamforge_cmd_unban(const char *name, const char *nick);
void dreamforge_cmd_svsmode_chan(const char *name, const char *mode, const char *nick);
void dreamforge_cmd_svid_umode(const char *nick, time_t ts);
void dreamforge_cmd_nc_change(User * u);
void dreamforge_cmd_svid_umode2(User * u, const char *ts);
void dreamforge_cmd_svid_umode3(User * u, const char *ts);
void dreamforge_cmd_eob();
int dreamforge_flood_mode_check(const char *value);
void dreamforge_cmd_jupe(const char *jserver, const char *who, const char *reason);
int dreamforge_valid_nick(const char *nick);
void dreamforge_cmd_ctcp(const char *source, const char *dest, const char *buf);

class DreamForgeProto : public IRCDProtoNew {
	public:
		void cmd_svsnoop(const char *, int);
		void cmd_remove_akill(const char *, const char *);
} ircd_proto;
