/* ShadowIRCD functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
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

void shadowircd_set_umode(User * user, int ac, char **av);
void shadowircd_cmd_svsnoop(char *server, int set);
void shadowircd_cmd_remove_akill(char *user, char *host);
void shadowircd_cmd_topic(char *whosets, char *chan, char *whosetit, char *topic, time_t when);
void shadowircd_cmd_vhost_off(User * u);
void shadowircd_cmd_akill(char *user, char *host, char *who, time_t when,time_t expires, char *reason);
void shadowircd_cmd_svskill(char *source, char *user, char *buf);
void shadowircd_cmd_svsmode(User * u, int ac, char **av);
void shadowircd_cmd_372(char *source, char *msg);
void shadowircd_cmd_372_error(char *source);
void shadowircd_cmd_375(char *source);
void shadowircd_cmd_376(char *source);
void shadowircd_cmd_nick(char *nick, char *name, char *modes);
void shadowircd_cmd_guest_nick(char *nick, char *user, char *host, char *real, char *modes);
void shadowircd_cmd_mode(char *source, char *dest, char *buf);
void shadowircd_cmd_bot_nick(char *nick, char *user, char *host, char *real, char *modes);
void shadowircd_cmd_kick(char *source, char *chan, char *user, char *buf);
void shadowircd_cmd_notice_ops(char *source, char *dest, char *buf);
void shadowircd_cmd_notice(char *source, char *dest, char *buf);
void shadowircd_cmd_notice2(char *source, char *dest, char *msg);
void shadowircd_cmd_privmsg(char *source, char *dest, char *buf);
void shadowircd_cmd_privmsg2(char *source, char *dest, char *msg);
void shadowircd_cmd_serv_notice(char *source, char *dest, char *msg);
void shadowircd_cmd_serv_privmsg(char *source, char *dest, char *msg);
void shadowircd_cmd_bot_chan_mode(char *nick, char *chan);
void shadowircd_cmd_351(char *source);
void shadowircd_cmd_quit(char *source, char *buf);
void shadowircd_cmd_pong(char *servname, char *who);
void shadowircd_cmd_join(char *user, char *channel, time_t chantime);
void shadowircd_cmd_unsqline(char *user);
void shadowircd_cmd_invite(char *source, char *chan, char *nick);
void shadowircd_cmd_part(char *nick, char *chan, char *buf);
void shadowircd_cmd_391(char *source, char *timestr);
void shadowircd_cmd_250(char *buf);
void shadowircd_cmd_307(char *buf);
void shadowircd_cmd_311(char *buf);
void shadowircd_cmd_312(char *buf);
void shadowircd_cmd_317(char *buf);
void shadowircd_cmd_219(char *source, char *letter);
void shadowircd_cmd_401(char *source, char *who);
void shadowircd_cmd_318(char *source, char *who);
void shadowircd_cmd_242(char *buf);
void shadowircd_cmd_243(char *buf);
void shadowircd_cmd_211(char *buf);
void shadowircd_cmd_global(char *source, char *buf);
void shadowircd_cmd_global_legacy(char *source, char *fmt);
void shadowircd_cmd_sqline(char *mask, char *reason);
void shadowircd_cmd_squit(char *servname, char *message);
void shadowircd_cmd_svso(char *source, char *nick, char *flag);
void shadowircd_cmd_chg_nick(char *oldnick, char *newnick);
void shadowircd_cmd_svsnick(char *source, char *guest, time_t when);
void shadowircd_cmd_vhost_on(char *nick, char *vIdent, char *vhost);
void shadowircd_cmd_connect(int servernum);
void shadowircd_cmd_bob();
void shadowircd_cmd_svshold(char *nick);
void shadowircd_cmd_release_svshold(char *nick);
void shadowircd_cmd_unsgline(char *mask);
void shadowircd_cmd_unszline(char *mask);
void shadowircd_cmd_szline(char *mask, char *reason, char *whom);
void shadowircd_cmd_sgline(char *mask, char *reason);
void shadowircd_cmd_unban(char *name, char *nick);
void shadowircd_cmd_svsmode_chan(char *name, char *mode, char *nick);
void shadowircd_cmd_svid_umode(char *nick, time_t ts);
void shadowircd_cmd_nc_change(User * u);
void shadowircd_cmd_svid_umode2(User * u, char *ts);
void shadowircd_cmd_svid_umode3(User * u, char *ts);
void shadowircd_cmd_eob();
int shadowircd_flood_mode_check(char *value);
void shadowircd_cmd_jupe(char *jserver, char *who, char *reason);
int shadowircd_valid_nick(char *nick);
void shadowircd_cmd_ctcp(char *source, char *dest, char *buf);
