/* Ultimate IRCD 3.0 functions
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

#define UMODE_a 0x00000001
#define UMODE_h 0x00000002
#define UMODE_i 0x00000004
#define UMODE_o 0x00000008
#define UMODE_r 0x00000010
#define UMODE_w 0x00000020
#define UMODE_A 0x00000040
#define UMODE_0 0x00000080
#define UMODE_Z 0x00000100   /* umode +Z - Services Root Admin */
#define UMODE_S 0x00000200   /* umode +S - Services Client */
#define UMODE_D 0x00000400   /* umode +D - has seen dcc warning message */
#define UMODE_d 0x00000800   /* umode +d - user is deaf to channel messages */
#define UMODE_W 0x00001000   /* umode +d - user is deaf to channel messages */
#define UMODE_p 0x04000000
#define UMODE_P 0x20000000   /* umode +P - Services Admin */
#define UMODE_x 0x40000000
#define UMODE_R 0x80000000


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
#define CMODE_c 0x00000400		/* Colors can't be used */
#define CMODE_A 0x00000800
#define CMODE_N 0x00001000
#define CMODE_S 0x00002000
#define CMODE_K 0x00004000
#define CMODE_O 0x00008000		/* Only opers can join */
#define CMODE_q 0x00010000      /* No Quit Reason */
#define CMODE_M 0x00020000      /* Non-regged nicks can't send messages */

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

void ultimate3_ProcessUsermodes(User * user, int ac, const char **av);
void ultimate3_cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when);
void ultimate3_SendVhostDel(User * u);
void ultimate3_SendAkill(const char *user, const char *host, const char *who, time_t when,time_t expires, const char *reason);
void ultimate3_SendSVSKill(const char *source, const char *user, const char *buf);
void ultimate3_SendSVSMode(User * u, int ac, const char **av);
void ultimate3_cmd_372(const char *source, const char *msg);
void ultimate3_cmd_372_error(const char *source);
void ultimate3_cmd_375(const char *source);
void ultimate3_cmd_376(const char *source);
void ultimate3_cmd_nick(const char *nick, const char *name, const char *modes);
void ultimate3_SendGuestNick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void ultimate3_SendMode(const char *source, const char *dest, const char *buf);
void ultimate3_SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void ultimate3_SendKick(const char *source, const char *chan, const char *user, const char *buf);
void ultimate3_SendNoticeChanops(const char *source, const char *dest, const char *buf);
void ultimate3_cmd_notice(const char *source, const char *dest, const char *buf);
void ultimate3_cmd_notice2(const char *source, const char *dest, const char *msg);
void ultimate3_cmd_privmsg(const char *source, const char *dest, const char *buf);
void ultimate3_cmd_privmsg2(const char *source, const char *dest, const char *msg);
void ultimate3_SendGlobalNotice(const char *source, const char *dest, const char *msg);
void ultimate3_SendGlobalPrivmsg(const char *source, const char *dest, const char *msg);
void ultimate3_SendBotOp(const char *nick, const char *chan);
void ultimate3_cmd_351(const char *source);
void ultimate3_SendQuit(const char *source, const char *buf);
void ultimate3_SendPong(const char *servname, const char *who);
void ultimate3_SendJoin(const char *user, const char *channel, time_t chantime);
void ultimate3_SendSQLineDel(const char *user);
void ultimate3_SendInvite(const char *source, const char *chan, const char *nick);
void ultimate3_SendPart(const char *nick, const char *chan, const char *buf);
void ultimate3_cmd_391(const char *source, const char *timestr);
void ultimate3_cmd_250(const char *buf);
void ultimate3_cmd_307(const char *buf);
void ultimate3_cmd_311(const char *buf);
void ultimate3_cmd_312(const char *buf);
void ultimate3_cmd_317(const char *buf);
void ultimate3_cmd_219(const char *source, const char *letter);
void ultimate3_cmd_401(const char *source, const char *who);
void ultimate3_cmd_318(const char *source, const char *who);
void ultimate3_cmd_242(const char *buf);
void ultimate3_cmd_243(const char *buf);
void ultimate3_cmd_211(const char *buf);
void ultimate3_SendGlobops(const char *source, const char *buf);
void ultimate3_SendGlobops_legacy(const char *source, const char *fmt);
void ultimate3_SendSQLine(const char *mask, const char *reason);
void ultimate3_SendSquit(const char *servname, const char *message);
void ultimate3_SendSVSO(const char *source, const char *nick, const char *flag);
void ultimate3_SendChangeBotNick(const char *oldnick, const char *newnick);
void ultimate3_SendForceNickChange(const char *source, const char *guest, time_t when);
void ultimate3_SendVhost(const char *nick, const char *vIdent, const char *vhost);
void ultimate3_SendConnect(int servernum);
void ultimate3_SendSVSHOLD(const char *nick);
void ultimate3_SendSVSHOLDDel(const char *nick);
void ultimate3_SendSGLineDel(const char *mask);
void ultimate3_SendSZLineDel(const char *mask);
void ultimate3_SendSZLine(const char *mask, const char *reason, const char *whom);
void ultimate3_SendSGLine(const char *mask, const char *reason);
void ultimate3_SendBanDel(const char *name, const char *nick);
void ultimate3_SendSVSMode_chan(const char *name, const char *mode, const char *nick);
void ultimate3_SendSVID(const char *nick, time_t ts);
void ultimate3_SendUnregisteredNick(User * u);
void ultimate3_SendSVID2(User * u, const char *ts);
void ultimate3_SendSVID3(User * u, const char *ts);
void ultimate3_SendEOB();
int ultimate3_flood_mode_check(const char *value);
void ultimate3_SendJupe(const char *jserver, const char *who, const char *reason);
int ultimate3_IsNickValid(const char *nick);
void ultimate3_SendCTCP(const char *source, const char *dest, const char *buf);

class UltimateIRCdProto : public IRCDProtoNew {
	public:
		void SendSVSNOOP(const char *, int);
		void SendAkillDel(const char *, const char *);
} ircd_proto;
