/* Ratbox IRCD functions
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

#define UMODE_a 0x00000001 /* umode a - admin */
#define UMODE_g 0x00000002 /* umode g - caller ID */
#define UMODE_i 0x00000004 /* umode i - invisible */
#define UMODE_o 0x00000008 /* umode o - operator */
#define UMODE_z 0x00000010 /* umode u - operwall */
#define UMODE_w 0x00000020 /* umode w - wallops */
#define UMODE_s 0x00000040 /* umode s - server notices */
#define UMODE_Q 0x00000080 /* umode Q - block forwarding */
#define UMODE_R 0x00000200 /* umode R - reject messages from unauthenticated users */
#define UMODE_S 0x00000400 /* umode S - network service */
#define UMODE_l 0x00020000 /* umode l - locops */


#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040
#define CMODE_l 0x00000080
#define CMODE_f 0x00000100
#define CMODE_c 0x00000200
#define CMODE_r 0x00000400
#define CMODE_P 0x00000800
#define CMODE_g 0x00001000
#define CMODE_z 0x00002000
#define CMODE_j 0x00004000
#define CMODE_F 0x00008000
#define CMODE_L 0x00010000
#define CMODE_Q 0x00020000

#define DEFAULT_MLOCK CMODE_n | CMODE_t


void charybdis_set_umode(User * user, int ac, char **av);
void charybdis_cmd_svsnoop(char *server, int set);
void charybdis_cmd_remove_akill(char *user, char *host);
void charybdis_cmd_topic(char *whosets, char *chan, char *whosetit, char *topic, time_t when);
void charybdis_cmd_vhost_off(User * u);
void charybdis_cmd_akill(char *user, char *host, char *who, time_t when,time_t expires, char *reason);
void charybdis_cmd_svskill(char *source, char *user, char *buf);
void charybdis_cmd_svsmode(User * u, int ac, char **av);
void charybdis_cmd_372(char *source, char *msg);
void charybdis_cmd_372_error(char *source);
void charybdis_cmd_375(char *source);
void charybdis_cmd_376(char *source);
void charybdis_cmd_nick(char *nick, char *name, char *modes);
void charybdis_cmd_guest_nick(char *nick, char *user, char *host, char *real, char *modes);
void charybdis_cmd_mode(char *source, char *dest, char *buf);
void charybdis_cmd_bot_nick(char *nick, char *user, char *host, char *real, char *modes);
void charybdis_cmd_kick(char *source, char *chan, char *user, char *buf);
void charybdis_cmd_notice_ops(char *source, char *dest, char *buf);
void charybdis_cmd_notice(char *source, char *dest, char *buf);
void charybdis_cmd_notice2(char *source, char *dest, char *msg);
void charybdis_cmd_privmsg(char *source, char *dest, char *buf);
void charybdis_cmd_privmsg2(char *source, char *dest, char *msg);
void charybdis_cmd_serv_notice(char *source, char *dest, char *msg);
void charybdis_cmd_serv_privmsg(char *source, char *dest, char *msg);
void charybdis_cmd_bot_chan_mode(char *nick, char *chan);
void charybdis_cmd_351(char *source);
void charybdis_cmd_quit(char *source, char *buf);
void charybdis_cmd_pong(char *servname, char *who);
void charybdis_cmd_join(char *user, char *channel, time_t chantime);
void charybdis_cmd_unsqline(char *user);
void charybdis_cmd_invite(char *source, char *chan, char *nick);
void charybdis_cmd_part(char *nick, char *chan, char *buf);
void charybdis_cmd_391(char *source, char *timestr);
void charybdis_cmd_250(char *buf);
void charybdis_cmd_307(char *buf);
void charybdis_cmd_311(char *buf);
void charybdis_cmd_312(char *buf);
void charybdis_cmd_317(char *buf);
void charybdis_cmd_219(char *source, char *letter);
void charybdis_cmd_401(char *source, char *who);
void charybdis_cmd_318(char *source, char *who);
void charybdis_cmd_242(char *buf);
void charybdis_cmd_243(char *buf);
void charybdis_cmd_211(char *buf);
void charybdis_cmd_global(char *source, char *buf);
void charybdis_cmd_global_legacy(char *source, char *fmt);
void charybdis_cmd_sqline(char *mask, char *reason);
void charybdis_cmd_squit(char *servname, char *message);
void charybdis_cmd_svso(char *source, char *nick, char *flag);
void charybdis_cmd_chg_nick(char *oldnick, char *newnick);
void charybdis_cmd_svsnick(char *source, char *guest, time_t when);
void charybdis_cmd_vhost_on(char *nick, char *vIdent, char *vhost);
void charybdis_cmd_connect(int servernum);
void charybdis_cmd_bob();
void charybdis_cmd_svshold(char *nick);
void charybdis_cmd_release_svshold(char *nick);
void charybdis_cmd_unsgline(char *mask);
void charybdis_cmd_unszline(char *mask);
void charybdis_cmd_szline(char *mask, char *reason, char *whom);
void charybdis_cmd_sgline(char *mask, char *reason);
void charybdis_cmd_unban(char *name, char *nick);
void charybdis_cmd_svsmode_chan(char *name, char *mode, char *nick);
void charybdis_cmd_svid_umode(char *nick, time_t ts);
void charybdis_cmd_nc_change(User * u);
void charybdis_cmd_svid_umode2(User * u, char *ts);
void charybdis_cmd_svid_umode3(User * u, char *ts);
void charybdis_cmd_eob();
int charybdis_flood_mode_check(char *value);
void charybdis_cmd_jupe(char *jserver, char *who, char *reason);
int charybdis_valid_nick(char *nick);
void charybdis_cmd_ctcp(char *source, char *dest, char *buf);


