/* PTLink IRCD functions
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
#define UMODE_B 0x00000080
#define UMODE_H 0x00000100
#define UMODE_N 0x00000200
#define UMODE_O 0x00000400
#define UMODE_p 0x00000800
#define UMODE_R 0x00001000
#define UMODE_s 0x00002000
#define UMODE_S 0x00004000
#define UMODE_T 0x00008000
#define UMODE_v 0x00001000
#define UMODE_y 0x00002000
#define UMODE_z 0x00004000

#define UMODE_VH 0x00008000		/* Fake umode used for internal vhost things */
#define UMODE_NM 0x00010000		/* Fake umode used for internal NEWMASK things */
/* Let's hope for a better vhost-system with PTlink7 ;) */


#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040		/* These two used only by ChanServ */
#define CMODE_l 0x00000080
#define CMODE_A 0x00000400
#define CMODE_B 0x00000800
#define CMODE_c 0x00001000
#define CMODE_d 0x00002000
#define CMODE_f 0x00004000
#define CMODE_K 0x00008000
#define CMODE_O 0x00010000
#define CMODE_q 0x00020000
#define CMODE_S 0x00040000
#define CMODE_N 0x00080000
#define CMODE_R 0x00000100		/* Only identified users can join */
#define CMODE_r 0x00000200		/* Set for all registered channels */
#define CMODE_C 0x00100000

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

/*
   The following variables are set to define the TS protocol version
   that we support. 

   PTLink 6.14 to 6.17  TS CURRENT is 6  and MIN is 3
   PTlink 6.18          TS CURRENT is 9  and MIN is 3
   PTLink 6.19 		TS CURRENT is 10 and MIN is 9

   If you are running 6.18 or 6.19 do not touch these values as they will
   allow you to connect

   If you are running an older version of PTLink, first think about updating
   your ircd, or changing the TS_CURRENT to 6 to allow services to connect
*/

#define PTLINK_TS_CURRENT 9
#define PTLINK_TS_MIN 3

void ptlink_set_umode(User * user, int ac, char **av);
void ptlink_cmd_svsnoop(char *server, int set);
void ptlink_cmd_remove_akill(char *user, char *host);
void ptlink_cmd_topic(char *whosets, char *chan, char *whosetit, char *topic, time_t when);
void ptlink_cmd_vhost_off(User * u);
void ptlink_cmd_akill(char *user, char *host, char *who, time_t when,time_t expires, char *reason);
void ptlink_cmd_svskill(char *source, char *user, char *buf);
void ptlink_cmd_svsmode(User * u, int ac, char **av);
void ptlink_cmd_372(char *source, char *msg);
void ptlink_cmd_372_error(char *source);
void ptlink_cmd_375(char *source);
void ptlink_cmd_376(char *source);
void ptlink_cmd_nick(char *nick, char *name, char *modes);
void ptlink_cmd_guest_nick(char *nick, char *user, char *host, char *real, char *modes);
void ptlink_cmd_mode(char *source, char *dest, char *buf);
void ptlink_cmd_bot_nick(char *nick, char *user, char *host, char *real, char *modes);
void ptlink_cmd_kick(char *source, char *chan, char *user, char *buf);
void ptlink_cmd_notice_ops(char *source, char *dest, char *buf);
void ptlink_cmd_notice(char *source, char *dest, char *buf);
void ptlink_cmd_notice2(char *source, char *dest, char *msg);
void ptlink_cmd_privmsg(char *source, char *dest, char *buf);
void ptlink_cmd_privmsg2(char *source, char *dest, char *msg);
void ptlink_cmd_serv_notice(char *source, char *dest, char *msg);
void ptlink_cmd_serv_privmsg(char *source, char *dest, char *msg);
void ptlink_cmd_bot_chan_mode(char *nick, char *chan);
void ptlink_cmd_351(char *source);
void ptlink_cmd_quit(char *source, char *buf);
void ptlink_cmd_pong(char *servname, char *who);
void ptlink_cmd_join(char *user, char *channel, time_t chantime);
void ptlink_cmd_unsqline(char *user);
void ptlink_cmd_invite(char *source, char *chan, char *nick);
void ptlink_cmd_part(char *nick, char *chan, char *buf);
void ptlink_cmd_391(char *source, char *timestr);
void ptlink_cmd_250(char *buf);
void ptlink_cmd_307(char *buf);
void ptlink_cmd_311(char *buf);
void ptlink_cmd_312(char *buf);
void ptlink_cmd_317(char *buf);
void ptlink_cmd_219(char *source, char *letter);
void ptlink_cmd_401(char *source, char *who);
void ptlink_cmd_318(char *source, char *who);
void ptlink_cmd_242(char *buf);
void ptlink_cmd_243(char *buf);
void ptlink_cmd_211(char *buf);
void ptlink_cmd_global(char *source, char *buf);
void ptlink_cmd_global_legacy(char *source, char *fmt);
void ptlink_cmd_sqline(char *mask, char *reason);
void ptlink_cmd_squit(char *servname, char *message);
void ptlink_cmd_svso(char *source, char *nick, char *flag);
void ptlink_cmd_chg_nick(char *oldnick, char *newnick);
void ptlink_cmd_svsnick(char *source, char *guest, time_t when);
void ptlink_cmd_vhost_on(char *nick, char *vIdent, char *vhost);
void ptlink_cmd_connect(int servernum);
void ptlink_cmd_bob();
void ptlink_cmd_svshold(char *nick);
void ptlink_cmd_release_svshold(char *nick);
void ptlink_cmd_unsgline(char *mask);
void ptlink_cmd_unszline(char *mask);
void ptlink_cmd_szline(char *mask, char *reason, char *whom);
void ptlink_cmd_sgline(char *mask, char *reason);
void ptlink_cmd_unban(char *name, char *nick);
void ptlink_cmd_svsmode_chan(char *name, char *mode, char *nick);
void ptlink_cmd_svid_umode(char *nick, time_t ts);
void ptlink_cmd_nc_change(User * u);
void ptlink_cmd_svid_umode2(User * u, char *ts);
void ptlink_cmd_svid_umode3(User * u, char *ts);
void ptlink_cmd_eob();
int ptlink_flood_mode_check(char *value);
void ptlink_cmd_jupe(char *jserver, char *who, char *reason);
int ptlink_valid_nick(char *nick);
void ptlink_cmd_ctcp(char *source, char *dest, char *buf);


