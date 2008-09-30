/* ShadowIRCD functions
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

/* The protocol revision. */
#define PROTOCOL_REVISION 3402


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
#define UMODE_v 0x00080000
#define UMODE_A 0x00100000
#define UMODE_E 0x00200000
#define UMODE_G 0x00400000
#define UMODE_R 0x00800000
#define UMODE_e 0x01000000
#define UMODE_O 0x02000000
#define UMODE_H 0x04000000

#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040
#define CMODE_l 0x00000080
#define CMODE_K 0x00000100
#define CMODE_A 0x00000200
#define CMODE_r 0x00000400
#define CMODE_z 0x00000800
#define CMODE_S 0x00001000
#define CMODE_c 0x00002000
#define CMODE_E 0x00004000
#define CMODE_F 0x00008000
#define CMODE_G 0x00010000
#define CMODE_L 0x00020000
#define CMODE_N 0x00040000
#define CMODE_O 0x00080000
#define CMODE_P 0x00100000
#define CMODE_R 0x00200000
#define CMODE_T 0x00400000
#define CMODE_V 0x00800000

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

void shadowircd_set_umode(User * user, int ac, const char **av);
void shadowircd_cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when);
void shadowircd_cmd_vhost_off(User * u);
void shadowircd_cmd_akill(const char *user, const char *host, const char *who, time_t when,time_t expires, const char *reason);
void shadowircd_cmd_svskill(const char *source, const char *user, const char *buf);
void shadowircd_cmd_svsmode(User * u, int ac, const char **av);
void shadowircd_cmd_372(const char *source, const char *msg);
void shadowircd_cmd_372_error(const char *source);
void shadowircd_cmd_375(const char *source);
void shadowircd_cmd_376(const char *source);
void shadowircd_cmd_nick(const char *nick, const char *name, const char *modes);
void shadowircd_cmd_guest_nick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void shadowircd_cmd_mode(const char *source, const char *dest, const char *buf);
void shadowircd_cmd_bot_nick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void shadowircd_cmd_kick(const char *source, const char *chan, const char *user, const char *buf);
void shadowircd_cmd_notice_ops(const char *source, const char *dest, const char *buf);
void shadowircd_cmd_notice(const char *source, const char *dest, const char *buf);
void shadowircd_cmd_notice2(const char *source, const char *dest, const char *msg);
void shadowircd_cmd_privmsg(const char *source, const char *dest, const char *buf);
void shadowircd_cmd_privmsg2(const char *source, const char *dest, const char *msg);
void shadowircd_cmd_serv_notice(const char *source, const char *dest, const char *msg);
void shadowircd_cmd_serv_privmsg(const char *source, const char *dest, const char *msg);
void shadowircd_cmd_bot_chan_mode(const char *nick, const char *chan);
void shadowircd_cmd_351(const char *source);
void shadowircd_cmd_quit(const char *source, const char *buf);
void shadowircd_cmd_pong(const char *servname, const char *who);
void shadowircd_cmd_join(const char *user, const char *channel, time_t chantime);
void shadowircd_cmd_unsqline(const char *user);
void shadowircd_cmd_invite(const char *source, const char *chan, const char *nick);
void shadowircd_cmd_part(const char *nick, const char *chan, const char *buf);
void shadowircd_cmd_391(const char *source, const char *timestr);
void shadowircd_cmd_250(const char *buf);
void shadowircd_cmd_307(const char *buf);
void shadowircd_cmd_311(const char *buf);
void shadowircd_cmd_312(const char *buf);
void shadowircd_cmd_317(const char *buf);
void shadowircd_cmd_219(const char *source, const char *letter);
void shadowircd_cmd_401(const char *source, const char *who);
void shadowircd_cmd_318(const char *source, const char *who);
void shadowircd_cmd_242(const char *buf);
void shadowircd_cmd_243(const char *buf);
void shadowircd_cmd_211(const char *buf);
void shadowircd_cmd_global(const char *source, const char *buf);
void shadowircd_cmd_global_legacy(const char *source, const char *fmt);
void shadowircd_cmd_sqline(const char *mask, const char *reason);
void shadowircd_cmd_squit(const char *servname, const char *message);
void shadowircd_cmd_svso(const char *source, const char *nick, const char *flag);
void shadowircd_cmd_chg_nick(const char *oldnick, const char *newnick);
void shadowircd_cmd_svsnick(const char *source, const char *guest, time_t when);
void shadowircd_cmd_vhost_on(const char *nick, const char *vIdent, const char *vhost);
void shadowircd_cmd_connect(int servernum);
void shadowircd_cmd_svshold(const char *nick);
void shadowircd_cmd_release_svshold(const char *nick);
void shadowircd_cmd_unsgline(const char *mask);
void shadowircd_cmd_unszline(const char *mask);
void shadowircd_cmd_szline(const char *mask, const char *reason, const char *whom);
void shadowircd_cmd_sgline(const char *mask, const char *reason);
void shadowircd_cmd_unban(const char *name, const char *nick);
void shadowircd_cmd_svsmode_chan(const char *name, const char *mode, const char *nick);
void shadowircd_cmd_svid_umode(const char *nick, time_t ts);
void shadowircd_cmd_nc_change(User * u);
void shadowircd_cmd_svid_umode2(User * u, const char *ts);
void shadowircd_cmd_svid_umode3(User * u, const char *ts);
void shadowircd_cmd_eob();
int shadowircd_flood_mode_check(const char *value);
void shadowircd_cmd_jupe(const char *jserver, const char *who, const char *reason);
int shadowircd_valid_nick(const char *nick);
void shadowircd_cmd_ctcp(const char *source, const char *dest, const char *buf);

class ShadowIRCdProto : public IRCDProtoNew {
	public:
		void cmd_remove_akill(const char *, const char *);
} ircd_proto;
