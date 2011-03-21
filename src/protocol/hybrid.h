/* Hybrid IRCD functions
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

#define UMODE_a 0x00000001  /* Admin status */
#define UMODE_b 0x00000080  /* See bot and drone flooding notices */
#define UMODE_c 0x00000100  /* Client connection/quit notices */
#define UMODE_d 0x00000200  /* See debugging notices */
#define UMODE_f 0x00000400  /* See I: line full notices */
#define UMODE_g 0x00000800  /* Server Side Ignore */
#define UMODE_i 0x00000004  /* Not shown in NAMES or WHO unless you share a channel */
#define UMODE_k 0x00001000  /* See server generated KILL messages */
#define UMODE_l 0x00002000  /* See LOCOPS messages */
#define UMODE_n 0x00004000  /* See client nick changes */
#define UMODE_o 0x00000008  /* Operator status */
#define UMODE_r 0x00000010  /* See rejected client notices */
#define UMODE_s 0x00008000  /* See general server notices */
#define UMODE_u 0x00010000  /* See unauthorized client notices */
#define UMODE_w 0x00000020  /* See server generated WALLOPS */
#define UMODE_x 0x00020000  /* See remote server connection and split notices */
#define UMODE_y 0x00040000  /* See LINKS, STATS (if configured), TRACE notices */
#define UMODE_z 0x00080000  /* See oper generated WALLOPS */

#define CMODE_i 0x00000001     /* Invite only */
#define CMODE_m 0x00000002     /* Users without +v/h/o cannot send text to the channel */
#define CMODE_n 0x00000004     /* Users must be in the channel to send text to it */
#define CMODE_p 0x00000008     /* Private is obsolete, this now restricts KNOCK */
#define CMODE_s 0x00000010     /* The channel does not show up on NAMES or LIST */
#define CMODE_t 0x00000020     /* Only chanops can change the topic */
#define CMODE_k 0x00000040     /* Key/password for the channel. */
#define CMODE_l 0x00000080     /* Limit the number of users in a channel */
#define CMODE_a 0x00000400     /* Anonymous ops, chanops are hidden */


#define DEFAULT_MLOCK CMODE_n | CMODE_t

void hybrid_set_umode(User * user, int ac, char **av);
void hybrid_cmd_svsnoop(char *server, int set);
void hybrid_cmd_remove_akill(char *user, char *host);
void hybrid_cmd_topic(char *whosets, char *chan, char *whosetit, char *topic, time_t when);
void hybrid_cmd_vhost_off(User * u);
void hybrid_cmd_akill(char *user, char *host, char *who, time_t when,time_t expires, char *reason);
void hybrid_cmd_svskill(char *source, char *user, char *buf);
void hybrid_cmd_svsmode(User * u, int ac, char **av);
void hybrid_cmd_372(char *source, char *msg);
void hybrid_cmd_372_error(char *source);
void hybrid_cmd_375(char *source);
void hybrid_cmd_376(char *source);
void hybrid_cmd_nick(char *nick, char *name, char *modes);
void hybrid_cmd_guest_nick(char *nick, char *user, char *host, char *real, char *modes);
void hybrid_cmd_mode(char *source, char *dest, char *buf);
void hybrid_cmd_bot_nick(char *nick, char *user, char *host, char *real, char *modes);
void hybrid_cmd_kick(char *source, char *chan, char *user, char *buf);
void hybrid_cmd_notice_ops(char *source, char *dest, char *buf);
void hybrid_cmd_notice(char *source, char *dest, char *buf);
void hybrid_cmd_notice2(char *source, char *dest, char *msg);
void hybrid_cmd_privmsg(char *source, char *dest, char *buf);
void hybrid_cmd_privmsg2(char *source, char *dest, char *msg);
void hybrid_cmd_serv_notice(char *source, char *dest, char *msg);
void hybrid_cmd_serv_privmsg(char *source, char *dest, char *msg);
void hybrid_cmd_bot_chan_mode(char *nick, char *chan);
void hybrid_cmd_351(char *source);
void hybrid_cmd_quit(char *source, char *buf);
void hybrid_cmd_pong(char *servname, char *who);
void hybrid_cmd_join(char *user, char *channel, time_t chantime);
void hybrid_cmd_unsqline(char *user);
void hybrid_cmd_invite(char *source, char *chan, char *nick);
void hybrid_cmd_part(char *nick, char *chan, char *buf);
void hybrid_cmd_391(char *source, char *timestr);
void hybrid_cmd_250(char *buf);
void hybrid_cmd_307(char *buf);
void hybrid_cmd_311(char *buf);
void hybrid_cmd_312(char *buf);
void hybrid_cmd_317(char *buf);
void hybrid_cmd_219(char *source, char *letter);
void hybrid_cmd_401(char *source, char *who);
void hybrid_cmd_318(char *source, char *who);
void hybrid_cmd_242(char *buf);
void hybrid_cmd_243(char *buf);
void hybrid_cmd_211(char *buf);
void hybrid_cmd_global(char *source, char *buf);
void hybrid_cmd_global_legacy(char *source, char *fmt);
void hybrid_cmd_sqline(char *mask, char *reason);
void hybrid_cmd_squit(char *servname, char *message);
void hybrid_cmd_svso(char *source, char *nick, char *flag);
void hybrid_cmd_chg_nick(char *oldnick, char *newnick);
void hybrid_cmd_svsnick(char *source, char *guest, time_t when);
void hybrid_cmd_vhost_on(char *nick, char *vIdent, char *vhost);
void hybrid_cmd_connect(int servernum);
void hybrid_cmd_bob();
void hybrid_cmd_svshold(char *nick);
void hybrid_cmd_release_svshold(char *nick);
void hybrid_cmd_unsgline(char *mask);
void hybrid_cmd_unszline(char *mask);
void hybrid_cmd_szline(char *mask, char *reason, char *whom);
void hybrid_cmd_sgline(char *mask, char *reason);
void hybrid_cmd_unban(char *name, char *nick);
void hybrid_cmd_svsmode_chan(char *name, char *mode, char *nick);
void hybrid_cmd_svid_umode(char *nick, time_t ts);
void hybrid_cmd_nc_change(User * u);
void hybrid_cmd_svid_umode2(User * u, char *ts);
void hybrid_cmd_svid_umode3(User * u, char *ts);
void hybrid_cmd_eob();
int hybrid_flood_mode_check(char *value);
void hybrid_cmd_jupe(char *jserver, char *who, char *reason);
int hybrid_valid_nick(char *nick);
void hybrid_cmd_ctcp(char *source, char *dest, char *buf);
int hybrid_event_notice(char *source, int argc, char **argv);
