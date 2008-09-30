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


void ratbox_set_umode(User * user, int ac, const char **av);
void ratbox_cmd_372(const char *source, const char *msg);
void ratbox_cmd_372_error(const char *source);
void ratbox_cmd_375(const char *source);
void ratbox_cmd_376(const char *source);
void ratbox_cmd_notice(const char *source, const char *dest, const char *buf);
void ratbox_cmd_notice2(const char *source, const char *dest, const char *msg);
void ratbox_cmd_privmsg(const char *source, const char *dest, const char *buf);
void ratbox_cmd_serv_notice(const char *source, const char *dest, const char *msg);
void ratbox_cmd_serv_privmsg(const char *source, const char *dest, const char *msg);
void ratbox_cmd_bot_chan_mode(const char *nick, const char *chan);
void ratbox_cmd_351(const char *source);
void ratbox_cmd_quit(const char *source, const char *buf);
void ratbox_cmd_pong(const char *servname, const char *who);
void ratbox_cmd_join(const char *user, const char *channel, time_t chantime);
void ratbox_cmd_unsqline(const char *user);
void ratbox_cmd_invite(const char *source, const char *chan, const char *nick);
void ratbox_cmd_part(const char *nick, const char *chan, const char *buf);
void ratbox_cmd_391(const char *source, const char *timestr);
void ratbox_cmd_250(const char *buf);
void ratbox_cmd_307(const char *buf);
void ratbox_cmd_311(const char *buf);
void ratbox_cmd_312(const char *buf);
void ratbox_cmd_317(const char *buf);
void ratbox_cmd_219(const char *source, const char *letter);
void ratbox_cmd_401(const char *source, const char *who);
void ratbox_cmd_318(const char *source, const char *who);
void ratbox_cmd_242(const char *buf);
void ratbox_cmd_243(const char *buf);
void ratbox_cmd_211(const char *buf);
void ratbox_cmd_global(const char *source, const char *buf);
void ratbox_cmd_sqline(const char *mask, const char *reason);
void ratbox_cmd_squit(const char *servname, const char *message);
void ratbox_cmd_svso(const char *source, const char *nick, const char *flag);
void ratbox_cmd_chg_nick(const char *oldnick, const char *newnick);
void ratbox_cmd_svsnick(const char *source, const char *guest, time_t when);
void ratbox_cmd_vhost_on(const char *nick, const char *vIdent, const char *vhost);
void ratbox_cmd_connect(int servernum);
void ratbox_cmd_svshold(const char *nick);
void ratbox_cmd_release_svshold(const char *nick);
void ratbox_cmd_unsgline(const char *mask);
void ratbox_cmd_unszline(const char *mask);
void ratbox_cmd_szline(const char *mask, const char *reason, const char *whom);
void ratbox_cmd_sgline(const char *mask, const char *reason);
void ratbox_cmd_unban(const char *name, const char *nick);
void ratbox_cmd_svsmode_chan(const char *name, const char *mode, const char *nick);
void ratbox_cmd_svid_umode(const char *nick, time_t ts);
void ratbox_cmd_nc_change(User * u);
void ratbox_cmd_svid_umode2(User * u, const char *ts);
void ratbox_cmd_svid_umode3(User * u, const char *ts);
void ratbox_cmd_eob();
int ratbox_flood_mode_check(const char *value);
void ratbox_cmd_jupe(const char *jserver, const char *who, const char *reason);
int ratbox_valid_nick(const char *nick);
void ratbox_cmd_ctcp(const char *source, const char *dest, const char *buf);

class RatboxProto : public IRCDProtoNew {
	public:
		void cmd_remove_akill(const char *, const char *);
		void cmd_topic(const char *, const char *, const char *, const char *, time_t);
		void cmd_akill(const char *, const char *, const char *, time_t, time_t, const char *);
		void cmd_svskill(const char *source, const char *user, const char *buf);
		void cmd_svsmode(User *u, int ac, const char **av);
		void cmd_nick(const char *, const char *, const char *);
		void cmd_mode(const char *source, const char *dest, const char *buf);
		void cmd_bot_nick(const char *, const char *, const char *, const char *, const char *);
		void cmd_kick(const char *source, const char *chan, const char *user, const char *buf);
		void cmd_notice_ops(const char *, const char *, const char *);
} ircd_proto;
