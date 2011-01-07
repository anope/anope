/* Ultimate IRCD 2 functions
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

#define UMODE_a 0x00000001
#define UMODE_h 0x00000002
#define UMODE_i 0x00000004
#define UMODE_o 0x00000008
#define UMODE_r 0x00000010
#define UMODE_w 0x00000020
#define UMODE_A 0x00000040
#define UMODE_p 0x04000000
#define UMODE_R 0x08000000
#define UMODE_P 0x20000000
#define UMODE_g 0x80000000
#define UMODE_x 0x40000000


#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040		/* These two used only by ChanServ */
#define CMODE_l 0x00000080
#define CMODE_f 0x00000400
#define CMODE_x 0x00000800
#define CMODE_A 0x00001000
#define CMODE_I 0x00002000
#define CMODE_K 0x00004000
#define CMODE_L 0x00008000
#define CMODE_O 0x00010000
#define CMODE_S 0x00020000
#define CMODE_R 0x00000100		/* Only identified users can join */
#define CMODE_r 0x00000200		/* Set for all registered channels */

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

void ultimate2_set_umode(User * user, int ac, char **av);
void ultimate2_cmd_svsnoop(char *server, int set);
void ultimate2_cmd_remove_akill(char *user, char *host);
void ultimate2_cmd_topic(char *whosets, char *chan, char *whosetit, char *topic, time_t when);
void ultimate2_cmd_vhost_off(User * u);
void ultimate2_cmd_akill(char *user, char *host, char *who, time_t when,time_t expires, char *reason);
void ultimate2_cmd_svskill(char *source, char *user, char *buf);
void ultimate2_cmd_svsmode(User * u, int ac, char **av);
void ultimate2_cmd_372(char *source, char *msg);
void ultimate2_cmd_372_error(char *source);
void ultimate2_cmd_375(char *source);
void ultimate2_cmd_376(char *source);
void ultimate2_cmd_nick(char *nick, char *name, char *modes);
void ultimate2_cmd_guest_nick(char *nick, char *user, char *host, char *real, char *modes);
void ultimate2_cmd_mode(char *source, char *dest, char *buf);
void ultimate2_cmd_bot_nick(char *nick, char *user, char *host, char *real, char *modes);
void ultimate2_cmd_kick(char *source, char *chan, char *user, char *buf);
void ultimate2_cmd_notice_ops(char *source, char *dest, char *buf);
void ultimate2_cmd_notice(char *source, char *dest, char *buf);
void ultimate2_cmd_notice2(char *source, char *dest, char *msg);
void ultimate2_cmd_privmsg(char *source, char *dest, char *buf);
void ultimate2_cmd_privmsg2(char *source, char *dest, char *msg);
void ultimate2_cmd_serv_notice(char *source, char *dest, char *msg);
void ultimate2_cmd_serv_privmsg(char *source, char *dest, char *msg);
void ultimate2_cmd_bot_chan_mode(char *nick, char *chan);
void ultimate2_cmd_351(char *source);
void ultimate2_cmd_quit(char *source, char *buf);
void ultimate2_cmd_pong(char *servname, char *who);
void ultimate2_cmd_join(char *user, char *channel, time_t chantime);
void ultimate2_cmd_unsqline(char *user);
void ultimate2_cmd_invite(char *source, char *chan, char *nick);
void ultimate2_cmd_part(char *nick, char *chan, char *buf);
void ultimate2_cmd_391(char *source, char *timestr);
void ultimate2_cmd_250(char *buf);
void ultimate2_cmd_307(char *buf);
void ultimate2_cmd_311(char *buf);
void ultimate2_cmd_312(char *buf);
void ultimate2_cmd_317(char *buf);
void ultimate2_cmd_219(char *source, char *letter);
void ultimate2_cmd_401(char *source, char *who);
void ultimate2_cmd_318(char *source, char *who);
void ultimate2_cmd_242(char *buf);
void ultimate2_cmd_243(char *buf);
void ultimate2_cmd_211(char *buf);
void ultimate2_cmd_global(char *source, char *buf);
void ultimate2_cmd_global_legacy(char *source, char *fmt);
void ultimate2_cmd_sqline(char *mask, char *reason);
void ultimate2_cmd_squit(char *servname, char *message);
void ultimate2_cmd_svso(char *source, char *nick, char *flag);
void ultimate2_cmd_chg_nick(char *oldnick, char *newnick);
void ultimate2_cmd_svsnick(char *source, char *guest, time_t when);
void ultimate2_cmd_vhost_on(char *nick, char *vIdent, char *vhost);
void ultimate2_cmd_connect(int servernum);
void ultimate2_cmd_bob();
void ultimate2_cmd_svshold(char *nick);
void ultimate2_cmd_release_svshold(char *nick);
void ultimate2_cmd_unsgline(char *mask);
void ultimate2_cmd_unszline(char *mask);
void ultimate2_cmd_szline(char *mask, char *reason, char *whom);
void ultimate2_cmd_sgline(char *mask, char *reason);
void ultimate2_cmd_unban(char *name, char *nick);
void ultimate2_cmd_svsmode_chan(char *name, char *mode, char *nick);
void ultimate2_cmd_svid_umode(char *nick, time_t ts);
void ultimate2_cmd_nc_change(User * u);
void ultimate2_cmd_svid_umode2(User * u, char *ts);
void ultimate2_cmd_svid_umode3(User * u, char *ts);
void ultimate2_cmd_eob();
int ultimate2_flood_mode_check(char *value);
void ultimate2_cmd_jupe(char *jserver, char *who, char *reason);
int ultimate2_valid_nick(char *nick);
void ultimate2_cmd_ctcp(char *source, char *dest, char *buf);


