/* inspircd beta 6 functions
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

/*************************************************************************/

#define UMODE_a 0x00000001
#define UMODE_h 0x00000002
#define UMODE_i 0x00000004
#define UMODE_o 0x00000008
#define UMODE_r 0x00000010
#define UMODE_w 0x00000020
#define UMODE_A 0x00000040
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
#define CMODE_c 0x00000400
#define CMODE_A 0x00000800
#define CMODE_H 0x00001000
#define CMODE_K 0x00002000
#define CMODE_L 0x00004000
#define CMODE_O 0x00008000
#define CMODE_Q 0x00010000
#define CMODE_S 0x00020000
#define CMODE_V 0x00040000
#define CMODE_f 0x00080000
#define CMODE_G 0x00100000
#define CMODE_C 0x00200000
#define CMODE_u 0x00400000
#define CMODE_z 0x00800000
#define CMODE_N 0x01000000
#define CMODE_R 0x00000100		/* Only identified users can join */
#define CMODE_r 0x00000200		/* Set for all registered channels */

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

void inspircd_set_umode(User * user, int ac, const char **av);
void inspircd_cmd_372(const char *source, const char *msg);
void inspircd_cmd_372_error(const char *source);
void inspircd_cmd_375(const char *source);
void inspircd_cmd_376(const char *source);
void inspircd_cmd_351(const char *source);
void inspircd_cmd_join(const char *user, const char *channel, time_t chantime);
void inspircd_cmd_unsqline(const char *user);
void inspircd_cmd_invite(const char *source, const char *chan, const char *nick);
void inspircd_cmd_part(const char *nick, const char *chan, const char *buf);
void inspircd_cmd_391(const char *source, const char *timestr);
void inspircd_cmd_250(const char *buf);
void inspircd_cmd_307(const char *buf);
void inspircd_cmd_311(const char *buf);
void inspircd_cmd_312(const char *buf);
void inspircd_cmd_317(const char *buf);
void inspircd_cmd_219(const char *source, const char *letter);
void inspircd_cmd_401(const char *source, const char *who);
void inspircd_cmd_318(const char *source, const char *who);
void inspircd_cmd_242(const char *buf);
void inspircd_cmd_243(const char *buf);
void inspircd_cmd_211(const char *buf);
void inspircd_cmd_global(const char *source, const char *buf);
void inspircd_cmd_sqline(const char *mask, const char *reason);
void inspircd_cmd_squit(const char *servname, const char *message);
void inspircd_cmd_svso(const char *source, const char *nick, const char *flag);
void inspircd_cmd_chg_nick(const char *oldnick, const char *newnick);
void inspircd_cmd_svsnick(const char *source, const char *guest, time_t when);
void inspircd_cmd_vhost_on(const char *nick, const char *vIdent, const char *vhost);
void inspircd_cmd_connect(int servernum);
void inspircd_cmd_svshold(const char *nick);
void inspircd_cmd_release_svshold(const char *nick);
void inspircd_cmd_unsgline(const char *mask);
void inspircd_cmd_unszline(const char *mask);
void inspircd_cmd_szline(const char *mask, const char *reason, const char *whom);
void inspircd_cmd_sgline(const char *mask, const char *reason);
void inspircd_cmd_unban(const char *name, const char *nick);
void inspircd_cmd_svsmode_chan(const char *name, const char *mode, const char *nick);
void inspircd_cmd_svid_umode(const char *nick, time_t ts);
void inspircd_cmd_nc_change(User * u);
void inspircd_cmd_svid_umode2(User * u, const char *ts);
void inspircd_cmd_svid_umode3(User * u, const char *ts);
void inspircd_cmd_eob();
int inspircd_flood_mode_check(const char *value);
void inspircd_cmd_jupe(const char *jserver, const char *who, const char *reason);
int inspircd_valid_nick(const char *nick);
void inspircd_cmd_ctcp(const char *source, const char *dest, const char *buf);
int anope_event_fjoin(const char *source, int ac, const char **av);
int anope_event_fmode(const char *source, int ac, const char **av);
int anope_event_ftopic(const char *source, int ac, const char **av);
int anope_event_sanick(const char *source, int ac, const char **av);
int anope_event_samode(const char *source, int ac, const char **av);
int anope_event_sajoin(const char *source, int ac, const char **av);
int anope_event_sapart(const char *source, int ac, const char **av);
int anope_event_version(const char *source, int ac, const char **av);
int anope_event_opertype(const char *source, int ac, const char **av);
int anope_event_idle(const char *source, int ac, const char **av);
int anope_event_rsquit(const char *source, int ac, const char **av);

class InspIRCdProto : public IRCDProtoNew {
	public:
		void cmd_remove_akill(const char *, const char *);
		void cmd_topic(const char *, const char *, const char *, const char *, time_t);
		void cmd_vhost_off(User *);
		void cmd_akill(const char *, const char *, const char *, time_t, time_t, const char *);
		void cmd_svskill(const char *, const char *, const char *);
		void cmd_svsmode(User *, int, const char **);
		void cmd_nick(const char *, const char *, const char *);
		void cmd_guest_nick(const char *, const char *, const char *, const char *, const char *);
		void cmd_mode(const char *, const char *, const char *);
		void cmd_bot_nick(const char *, const char *, const char *, const char *, const char *);
		void cmd_kick(const char *, const char *, const char *, const char *);
		void cmd_notice_ops(const char *, const char *, const char *);
		void cmd_bot_chan_mode(const char *, const char *);
} ircd_proto;
