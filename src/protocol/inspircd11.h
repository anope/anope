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

class InspIRCdProto : public IRCDProto {
	public:
		void SendAkillDel(const char *, const char *);
		void cmd_topic(const char *, const char *, const char *, const char *, time_t);
		void SendVhostDel(User *);
		void SendAkill(const char *, const char *, const char *, time_t, time_t, const char *);
		void SendSVSKill(const char *, const char *, const char *);
		void SendSVSMode(User *, int, const char **);
		void SendGuestNick(const char *, const char *, const char *, const char *, const char *);
		void SendMode(const char *, const char *, const char *);
		void SendClientIntroduction(const char *, const char *, const char *, const char *, const char *);
		void SendKick(const char *, const char *, const char *, const char *);
		void SendNoticeChanops(const char *, const char *, const char *);
		void SendBotOp(const char *, const char *);
		void SendJoin(const char *, const char *, time_t);
		void SendSQLineDel(const char *);
		void cmd_sqline(const char *, const char *);
		void cmd_squit(const char *, const char *);
		void cmd_vhost_on(const char *, const char *, const char *);
		void cmd_connect();
		void cmd_svshold(const char *);
		void cmd_release_svshold(const char *);
		void cmd_unszline(const char *);
		void cmd_szline(const char *, const char *, const char *);
		void cmd_nc_change(User *);
		void cmd_svid_umode2(User *, const char *);
		void cmd_svsjoin(const char *, const char *, const char *, const char *);
		void cmd_svspart(const char *, const char *, const char *);
		void cmd_eob();
		void cmd_server(const char *, int, const char *);
		void set_umode(User *, int, const char **);
		int flood_mode_check(const char *);
		void cmd_numeric(const char *, int, const char *, const char *);
} ircd_proto;
