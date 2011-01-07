/* Viagra IRCD functions
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

/* User Modes */
#define UMODE_A 0x00000040  /* Is a Server Administrator. */
#define UMODE_C 0x00002000  /* Is a Server Co Administrator. */
#define UMODE_I 0x00008000  /* Stealth mode, makes you beeing hidden at channel. invisible joins/parts. */
#define UMODE_N	0x00000400  /* Is a Network Administrator. */
#define UMODE_O 0x00004000  /* Local IRC Operator. */
#define UMODE_Q 0x00001000  /* Is an Abuse Administrator. */
#define UMODE_R 0x08000000  /* Cant receive messages from non registered user. */
#define UMODE_S	0x00000080  /* Is a Network Service. For Services only. */
#define UMODE_T	0x00000800  /* Is a Technical Administrator. */
#define UMODE_a 0x00000001  /* Is a Services Administrator. */
#define UMODE_b 0x00040000  /* Can listen to generic bot warnings. */
#define UMODE_c 0x00010000  /* See's all connects/disconnects on local server. */
#define UMODE_d	0x00000100  /* Can listen to debug and channel cration notices. */
#define UMODE_e 0x00080000  /* Can see client connections/exits on remote servers. */
#define UMODE_f 0x00100000  /* Listen to flood/spam alerts from server. */
#define UMODE_g	0x00000200  /* Can read & send to globops, and locops. */
#define UMODE_h 0x00000002  /* Is a Help Operator. */
#define UMODE_i 0x00000004  /* Invisible (Not shown in /who and /names searches). */
#define UMODE_n 0x00020000  /* Can see client nick change notices. */
#define UMODE_o 0x00000008  /* Global IRC Operator. */
#define UMODE_r 0x00000010  /* Identifies the nick as being registered. */
#define UMODE_s 0x00200000  /* Can listen to generic server messages. */
#define UMODE_w 0x00000020  /* Can listen to wallop messages. */
#define UMODE_x 0x40000000  /* Gives the user hidden hostname. */


/* Channel Modes */
#define CMODE_i 0x00000001  /* Invite-only allowed. */
#define CMODE_m 0x00000002  /* Moderated channel, noone can speak and changing nick except users with mode +vho */
#define CMODE_n 0x00000004  /* No messages from outside channel */
#define CMODE_p 0x00000008  /* Private channel. */
#define CMODE_s 0x00000010  /* Secret channel. */
#define CMODE_t 0x00000020  /* Only channel operators may set the topic */
#define CMODE_k 0x00000040  /* Needs the channel key to join the channel */
#define CMODE_l 0x00000080  /* Channel may hold at most <number> of users */
#define CMODE_R 0x00000100  /* Requires a registered nickname to join the channel. */
#define CMODE_r 0x00000200  /* Channel is registered. */
#define CMODE_c 0x00000400  /* No ANSI color can be sent to the channel */
#define CMODE_M 0x00000800  /* Requires a registered nickname to speak at the channel. */
#define CMODE_H 0x00001000  /* HelpOps only channel. */
#define CMODE_O 0x00008000  /* IRCOps only channel. */
#define CMODE_S 0x00020000  /* Strips all mesages out of colors. */
#define CMODE_N 0x01000000  /* No nickchanges allowed. */
#define CMODE_P 0x02000000  /* "Peace mode" No kicks allowed unless by u:lines */
#define CMODE_x 0x04000000  /* No bold/underlined or reversed text can be sent to the channel */

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

void viagra_set_umode(User * user, int ac, char **av);
void viagra_cmd_svsnoop(char *server, int set);
void viagra_cmd_remove_akill(char *user, char *host);
void viagra_cmd_topic(char *whosets, char *chan, char *whosetit, char *topic, time_t when);
void viagra_cmd_vhost_off(User * u);
void viagra_cmd_akill(char *user, char *host, char *who, time_t when,time_t expires, char *reason);
void viagra_cmd_svskill(char *source, char *user, char *buf);
void viagra_cmd_svsmode(User * u, int ac, char **av);
void viagra_cmd_372(char *source, char *msg);
void viagra_cmd_372_error(char *source);
void viagra_cmd_375(char *source);
void viagra_cmd_376(char *source);
void viagra_cmd_nick(char *nick, char *name, char *modes);
void viagra_cmd_guest_nick(char *nick, char *user, char *host, char *real, char *modes);
void viagra_cmd_mode(char *source, char *dest, char *buf);
void viagra_cmd_bot_nick(char *nick, char *user, char *host, char *real, char *modes);
void viagra_cmd_kick(char *source, char *chan, char *user, char *buf);
void viagra_cmd_notice_ops(char *source, char *dest, char *buf);
void viagra_cmd_notice(char *source, char *dest, char *buf);
void viagra_cmd_notice2(char *source, char *dest, char *msg);
void viagra_cmd_privmsg(char *source, char *dest, char *buf);
void viagra_cmd_privmsg2(char *source, char *dest, char *msg);
void viagra_cmd_serv_notice(char *source, char *dest, char *msg);
void viagra_cmd_serv_privmsg(char *source, char *dest, char *msg);
void viagra_cmd_bot_chan_mode(char *nick, char *chan);
void viagra_cmd_351(char *source);
void viagra_cmd_quit(char *source, char *buf);
void viagra_cmd_pong(char *servname, char *who);
void viagra_cmd_join(char *user, char *channel, time_t chantime);
void viagra_cmd_unsqline(char *user);
void viagra_cmd_invite(char *source, char *chan, char *nick);
void viagra_cmd_part(char *nick, char *chan, char *buf);
void viagra_cmd_391(char *source, char *timestr);
void viagra_cmd_250(char *buf);
void viagra_cmd_307(char *buf);
void viagra_cmd_311(char *buf);
void viagra_cmd_312(char *buf);
void viagra_cmd_317(char *buf);
void viagra_cmd_219(char *source, char *letter);
void viagra_cmd_401(char *source, char *who);
void viagra_cmd_318(char *source, char *who);
void viagra_cmd_242(char *buf);
void viagra_cmd_243(char *buf);
void viagra_cmd_211(char *buf);
void viagra_cmd_global(char *source, char *buf);
void viagra_cmd_global_legacy(char *source, char *fmt);
void viagra_cmd_sqline(char *mask, char *reason);
void viagra_cmd_squit(char *servname, char *message);
void viagra_cmd_svso(char *source, char *nick, char *flag);
void viagra_cmd_chg_nick(char *oldnick, char *newnick);
void viagra_cmd_svsnick(char *source, char *guest, time_t when);
void viagra_cmd_vhost_on(char *nick, char *vIdent, char *vhost);
void viagra_cmd_connect(int servernum);
void viagra_cmd_bob();
void viagra_cmd_svshold(char *nick);
void viagra_cmd_release_svshold(char *nick);
void viagra_cmd_unsgline(char *mask);
void viagra_cmd_unszline(char *mask);
void viagra_cmd_szline(char *mask, char *reason, char *whom);
void viagra_cmd_sgline(char *mask, char *reason);
void viagra_cmd_unban(char *name, char *nick);
void viagra_cmd_svsmode_chan(char *name, char *mode, char *nick);
void viagra_cmd_svid_umode(char *nick, time_t ts);
void viagra_cmd_nc_change(User * u);
void viagra_cmd_svid_umode2(User * u, char *ts);
void viagra_cmd_svid_umode3(User * u, char *ts);
void viagra_cmd_eob();
int viagra_flood_mode_check(char *value);
void viagra_cmd_jupe(char *jserver, char *who, char *reason);
int viagra_valid_nick(char *nick);
void viagra_cmd_ctcp(char *source, char *dest, char *buf);
