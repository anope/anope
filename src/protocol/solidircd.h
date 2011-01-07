/* SolidIRCD functions
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

/*************************************************************************/

#define UMODE_a 0x00000001  /* umode +a - Services Admin */
#define UMODE_h 0x00000002  /* umode +h - Helper */
#define UMODE_i 0x00000004  /* umode +i - Invisible */
#define UMODE_o 0x00000008  /* umode +o - Oper */
#define UMODE_r 0x00000010  /* umode +r - registered nick */
#define UMODE_w 0x00000020  /* umode +w - Get wallops */
#define UMODE_A 0x00000040  /* umode +A - Server Admin */
#define UMODE_x 0x00000080  /* umode +x - Squelch with notice */
#define UMODE_X 0x00000100  /* umode +X - Squelch without notice */
#define UMODE_F 0x00000200  /* umode +F - no cptr->since message rate throttle */
#define UMODE_j 0x00000400  /* umode +j - client rejection notices */
#define UMODE_K 0x00000800  /* umode +K - U: lined server kill messages */
#define UMODE_O 0x00001000  /* umode +O - Local Oper */
#define UMODE_s 0x00002000  /* umode +s - Server notices */
#define UMODE_c 0x00004000  /* umode +c - Client connections/exits */
#define UMODE_k 0x00008000  /* umode +k - Server kill messages */
#define UMODE_f 0x00010000  /* umode +f - Server flood messages */
#define UMODE_y 0x00020000  /* umode +y - Stats/links */
#define UMODE_d 0x00040000  /* umode +d - Debug info */
#define UMODE_g 0x00080000  /* umode +g - Globops */
#define UMODE_b 0x00100000  /* umode +b - Chatops */
#define UMODE_n 0x00200000  /* umode +n - Routing Notices */
#define UMODE_m 0x00400000  /* umode +m - spambot notices */
#define UMODE_e 0x00800000  /* umode +e - oper notices for the above +D */
#define UMODE_D 0x01000000  /* umode +D - Hidden dccallow umode */
#define UMODE_I 0x02000000  /* umode +I - invisible oper (masked) */
#define UMODE_C 0x04000000  /* umode +C - conops */
#define UMODE_v 0x10000000	/* umode +v - hostmasking */
#define UMODE_H 0x20000000  /* umode +H - Oper Hiding */
#define UMODE_z 0x40000000  /* umode +z - SSL */
#define UMODE_R 0x80000000  /* umode +R - No non registered msgs */

#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040	/* These two used only by ChanServ */
#define CMODE_l 0x00000080
#define CMODE_R 0x00000100	/* Only identified users can join */
#define CMODE_r 0x00000200	/* Set for all registered channels */
#define CMODE_c 0x00000400	/* Colors can't be used */
#define CMODE_M 0x00000800      /* Non-regged nicks can't send messages */
#define CMODE_j 0x00001000      /* join throttle */
#define CMODE_S 0x00002000      /* SSL only */
#define CMODE_N 0x00004000     /* No Nickname change */
#define CMODE_O 0x00008000	/* Only opers can join */


#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

void solidircd_set_umode(User * user, int ac, char **av);
void solidircd_cmd_svsnoop(char *server, int set);
void solidircd_cmd_remove_akill(char *user, char *host);
void solidircd_cmd_topic(char *whosets, char *chan, char *whosetit, char *topic, time_t when);
void solidircd_cmd_vhost_off(User * u);
void solidircd_cmd_akill(char *user, char *host, char *who, time_t when,time_t expires, char *reason);
void solidircd_cmd_svskill(char *source, char *user, char *buf);
void solidircd_cmd_svsmode(User * u, int ac, char **av);
void solidircd_cmd_372(char *source, char *msg);
void solidircd_cmd_372_error(char *source);
void solidircd_cmd_375(char *source);
void solidircd_cmd_376(char *source);
void solidircd_cmd_nick(char *nick, char *name, char *modes);
void solidircd_cmd_guest_nick(char *nick, char *user, char *host, char *real, char *modes);
void solidircd_cmd_mode(char *source, char *dest, char *buf);
void solidircd_cmd_bot_nick(char *nick, char *user, char *host, char *real, char *modes);
void solidircd_cmd_kick(char *source, char *chan, char *user, char *buf);
void solidircd_cmd_notice_ops(char *source, char *dest, char *buf);
void solidircd_cmd_notice(char *source, char *dest, char *buf);
void solidircd_cmd_notice2(char *source, char *dest, char *msg);
void solidircd_cmd_privmsg(char *source, char *dest, char *buf);
void solidircd_cmd_privmsg2(char *source, char *dest, char *msg);
void solidircd_cmd_serv_notice(char *source, char *dest, char *msg);
void solidircd_cmd_serv_privmsg(char *source, char *dest, char *msg);
void solidircd_cmd_bot_chan_mode(char *nick, char *chan);
void solidircd_cmd_351(char *source);
void solidircd_cmd_quit(char *source, char *buf);
void solidircd_cmd_pong(char *servname, char *who);
void solidircd_cmd_join(char *user, char *channel, time_t chantime);
void solidircd_cmd_unsqline(char *user);
void solidircd_cmd_invite(char *source, char *chan, char *nick);
void solidircd_cmd_part(char *nick, char *chan, char *buf);
void solidircd_cmd_391(char *source, char *timestr);
void solidircd_cmd_250(char *buf);
void solidircd_cmd_307(char *buf);
void solidircd_cmd_311(char *buf);
void solidircd_cmd_312(char *buf);
void solidircd_cmd_317(char *buf);
void solidircd_cmd_219(char *source, char *letter);
void solidircd_cmd_401(char *source, char *who);
void solidircd_cmd_318(char *source, char *who);
void solidircd_cmd_242(char *buf);
void solidircd_cmd_243(char *buf);
void solidircd_cmd_211(char *buf);
void solidircd_cmd_global(char *source, char *buf);
void solidircd_cmd_global_legacy(char *source, char *fmt);
void solidircd_cmd_sqline(char *mask, char *reason);
void solidircd_cmd_squit(char *servname, char *message);
void solidircd_cmd_svso(char *source, char *nick, char *flag);
void solidircd_cmd_chg_nick(char *oldnick, char *newnick);
void solidircd_cmd_svsnick(char *source, char *guest, time_t when);
void solidircd_cmd_vhost_on(char *nick, char *vIdent, char *vhost);
void solidircd_cmd_connect(int servernum);
void solidircd_cmd_bob();
void solidircd_cmd_svshold(char *nick);
void solidircd_cmd_release_svshold(char *nick);
void solidircd_cmd_unsgline(char *mask);
void solidircd_cmd_unszline(char *mask);
void solidircd_cmd_szline(char *mask, char *reason, char *whom);
void solidircd_cmd_sgline(char *mask, char *reason);
void solidircd_cmd_unban(char *name, char *nick);
void solidircd_cmd_svsmode_chan(char *name, char *mode, char *nick);
void solidircd_cmd_svid_umode(char *nick, time_t ts);
void solidircd_cmd_nc_change(User * u);
void solidircd_cmd_svid_umode2(User * u, char *ts);
void solidircd_cmd_svid_umode3(User * u, char *ts);
void solidircd_cmd_eob();
int solidircd_flood_mode_check(char *value);
void solidircd_cmd_jupe(char *jserver, char *who, char *reason);
int solidircd_valid_nick(char *nick);
void solidircd_cmd_ctcp(char *source, char *dest, char *buf);
