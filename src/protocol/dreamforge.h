/* DreamForge IRCD functions
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

void dreamforge_set_umode(User * user, int ac, char **av);
void dreamforge_cmd_svsnoop(char *server, int set);
void dreamforge_cmd_remove_akill(char *user, char *host);
void dreamforge_cmd_topic(char *whosets, char *chan, char *whosetit, char *topic, time_t when);
void dreamforge_cmd_vhost_off(User * u);
void dreamforge_cmd_akill(char *user, char *host, char *who, time_t when,time_t expires, char *reason);
void dreamforge_cmd_svskill(char *source, char *user, char *buf);
void dreamforge_cmd_svsmode(User * u, int ac, char **av);
void dreamforge_cmd_372(char *source, char *msg);
void dreamforge_cmd_372_error(char *source);
void dreamforge_cmd_375(char *source);
void dreamforge_cmd_376(char *source);
void dreamforge_cmd_nick(char *nick, char *name, char *modes);
void dreamforge_cmd_guest_nick(char *nick, char *user, char *host, char *real, char *modes);
void dreamforge_cmd_mode(char *source, char *dest, char *buf);
void dreamforge_cmd_bot_nick(char *nick, char *user, char *host, char *real, char *modes);
void dreamforge_cmd_kick(char *source, char *chan, char *user, char *buf);
void dreamforge_cmd_notice_ops(char *source, char *dest, char *buf);
void dreamforge_cmd_notice(char *source, char *dest, char *buf);
void dreamforge_cmd_notice2(char *source, char *dest, char *msg);
void dreamforge_cmd_privmsg(char *source, char *dest, char *buf);
void dreamforge_cmd_privmsg2(char *source, char *dest, char *msg);
void dreamforge_cmd_serv_notice(char *source, char *dest, char *msg);
void dreamforge_cmd_serv_privmsg(char *source, char *dest, char *msg);
void dreamforge_cmd_bot_chan_mode(char *nick, char *chan);
void dreamforge_cmd_351(char *source);
void dreamforge_cmd_quit(char *source, char *buf);
void dreamforge_cmd_pong(char *servname, char *who);
void dreamforge_cmd_join(char *user, char *channel, time_t chantime);
void dreamforge_cmd_unsqline(char *user);
void dreamforge_cmd_invite(char *source, char *chan, char *nick);
void dreamforge_cmd_part(char *nick, char *chan, char *buf);
void dreamforge_cmd_391(char *source, char *timestr);
void dreamforge_cmd_250(char *buf);
void dreamforge_cmd_307(char *buf);
void dreamforge_cmd_311(char *buf);
void dreamforge_cmd_312(char *buf);
void dreamforge_cmd_317(char *buf);
void dreamforge_cmd_219(char *source, char *letter);
void dreamforge_cmd_401(char *source, char *who);
void dreamforge_cmd_318(char *source, char *who);
void dreamforge_cmd_242(char *buf);
void dreamforge_cmd_243(char *buf);
void dreamforge_cmd_211(char *buf);
void dreamforge_cmd_global(char *source, char *buf);
void dreamforge_cmd_global_legacy(char *source, char *fmt);
void dreamforge_cmd_sqline(char *mask, char *reason);
void dreamforge_cmd_squit(char *servname, char *message);
void dreamforge_cmd_svso(char *source, char *nick, char *flag);
void dreamforge_cmd_chg_nick(char *oldnick, char *newnick);
void dreamforge_cmd_svsnick(char *source, char *guest, time_t when);
void dreamforge_cmd_vhost_on(char *nick, char *vIdent, char *vhost);
void dreamforge_cmd_connect(int servernum);
void dreamforge_cmd_bob();
void dreamforge_cmd_svshold(char *nick);
void dreamforge_cmd_release_svshold(char *nick);
void dreamforge_cmd_unsgline(char *mask);
void dreamforge_cmd_unszline(char *mask);
void dreamforge_cmd_szline(char *mask, char *reason, char *whom);
void dreamforge_cmd_sgline(char *mask, char *reason);
void dreamforge_cmd_unban(char *name, char *nick);
void dreamforge_cmd_svsmode_chan(char *name, char *mode, char *nick);
void dreamforge_cmd_svid_umode(char *nick, time_t ts);
void dreamforge_cmd_nc_change(User * u);
void dreamforge_cmd_svid_umode2(User * u, char *ts);
void dreamforge_cmd_svid_umode3(User * u, char *ts);
void dreamforge_cmd_eob();
int dreamforge_flood_mode_check(char *value);
void dreamforge_cmd_jupe(char *jserver, char *who, char *reason);
int dreamforge_valid_nick(char *nick);
void dreamforge_cmd_ctcp(char *source, char *dest, char *buf);

