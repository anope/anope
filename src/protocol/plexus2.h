/* PlexusIRCD IRCD functions
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
#define UMODE_R 0x00000010
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
#define UMODE_S 0x00080000


#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040		/* These two used only by ChanServ */
#define CMODE_l 0x00000080
#define CMODE_a 0x00000400
#define CMODE_Z 0x00000800
#define CMODE_M 0x00001000
#define CMODE_c 0x00002000
#define CMODE_O 0x00004000
#define CMODE_R 0x00008000
#define CMODE_N 0x00010000


#define DEFAULT_MLOCK CMODE_n | CMODE_t

void plexus_set_umode(User * user, int ac, char **av);
void plexus_cmd_svsnoop(char *server, int set);
void plexus_cmd_remove_akill(char *user, char *host);
void plexus_cmd_topic(char *whosets, char *chan, char *whosetit, char *topic, time_t when);
void plexus_cmd_vhost_off(User * u);
void plexus_cmd_akill(char *user, char *host, char *who, time_t when,time_t expires, char *reason);
void plexus_cmd_svskill(char *source, char *user, char *buf);
void plexus_cmd_svsmode(User * u, int ac, char **av);
void plexus_cmd_372(char *source, char *msg);
void plexus_cmd_372_error(char *source);
void plexus_cmd_375(char *source);
void plexus_cmd_376(char *source);
void plexus_cmd_nick(char *nick, char *name, char *modes);
void plexus_cmd_guest_nick(char *nick, char *user, char *host, char *real, char *modes);
void plexus_cmd_mode(char *source, char *dest, char *buf);
void plexus_cmd_bot_nick(char *nick, char *user, char *host, char *real, char *modes);
void plexus_cmd_kick(char *source, char *chan, char *user, char *buf);
void plexus_cmd_notice_ops(char *source, char *dest, char *buf);
void plexus_cmd_notice(char *source, char *dest, char *buf);
void plexus_cmd_notice2(char *source, char *dest, char *msg);
void plexus_cmd_privmsg(char *source, char *dest, char *buf);
void plexus_cmd_privmsg2(char *source, char *dest, char *msg);
void plexus_cmd_serv_notice(char *source, char *dest, char *msg);
void plexus_cmd_serv_privmsg(char *source, char *dest, char *msg);
void plexus_cmd_bot_chan_mode(char *nick, char *chan);
void plexus_cmd_351(char *source);
void plexus_cmd_quit(char *source, char *buf);
void plexus_cmd_pong(char *servname, char *who);
void plexus_cmd_join(char *user, char *channel, time_t chantime);
void plexus_cmd_unsqline(char *user);
void plexus_cmd_invite(char *source, char *chan, char *nick);
void plexus_cmd_part(char *nick, char *chan, char *buf);
void plexus_cmd_391(char *source, char *timestr);
void plexus_cmd_250(char *buf);
void plexus_cmd_307(char *buf);
void plexus_cmd_311(char *buf);
void plexus_cmd_312(char *buf);
void plexus_cmd_317(char *buf);
void plexus_cmd_219(char *source, char *letter);
void plexus_cmd_401(char *source, char *who);
void plexus_cmd_318(char *source, char *who);
void plexus_cmd_242(char *buf);
void plexus_cmd_243(char *buf);
void plexus_cmd_211(char *buf);
void plexus_cmd_global(char *source, char *buf);
void plexus_cmd_global_legacy(char *source, char *fmt);
void plexus_cmd_sqline(char *mask, char *reason);
void plexus_cmd_squit(char *servname, char *message);
void plexus_cmd_svso(char *source, char *nick, char *flag);
void plexus_cmd_chg_nick(char *oldnick, char *newnick);
void plexus_cmd_svsnick(char *source, char *guest, time_t when);
void plexus_cmd_vhost_on(char *nick, char *vIdent, char *vhost);
void plexus_cmd_connect(int servernum);
void plexus_cmd_bob();
void plexus_cmd_svshold(char *nick);
void plexus_cmd_release_svshold(char *nick);
void plexus_cmd_unsgline(char *mask);
void plexus_cmd_unszline(char *mask);
void plexus_cmd_szline(char *mask, char *reason, char *whom);
void plexus_cmd_sgline(char *mask, char *reason);
void plexus_cmd_unban(char *name, char *nick);
void plexus_cmd_svsmode_chan(char *name, char *mode, char *nick);
void plexus_cmd_svid_umode(char *nick, time_t ts);
void plexus_cmd_nc_change(User * u);
void plexus_cmd_svid_umode2(User * u, char *ts);
void plexus_cmd_svid_umode3(User * u, char *ts);
void plexus_cmd_eob();
int plexus_flood_mode_check(char *value);
void plexus_cmd_jupe(char *jserver, char *who, char *reason);
int plexus_valid_nick(char *nick);
void plexus_cmd_ctcp(char *source, char *dest, char *buf);


