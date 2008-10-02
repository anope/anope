/* Rage IRCD functions
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
#define UMODE_R 0x80000000
#define UMODE_x 0x40000000

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
#define CMODE_M 0x00000800      /* Non-regged nicks can't send messages */
#define CMODE_N 0x00001000
#define CMODE_S 0x00002000
#define CMODE_C 0x00004000
#define CMODE_A 0x00008000
#define CMODE_O 0x00010000		/* Only opers can join */

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

void rageircd_set_umode(User * user, int ac, const char **av);
void rageircd_cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when);
void rageircd_SendVhostDel(User * u);
void rageircd_SendAkill(const char *user, const char *host, const char *who, time_t when,time_t expires, const char *reason);
void rageircd_SendSVSKill(const char *source, const char *user, const char *buf);
void rageircd_SendSVSMode(User * u, int ac, const char **av);
void rageircd_cmd_372(const char *source, const char *msg);
void rageircd_cmd_372_error(const char *source);
void rageircd_cmd_375(const char *source);
void rageircd_cmd_376(const char *source);
void rageircd_cmd_nick(const char *nick, const char *name, const char *modes);
void rageircd_SendGuestNick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void rageircd_SendMode(const char *source, const char *dest, const char *buf);
void rageircd_SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void rageircd_SendKick(const char *source, const char *chan, const char *user, const char *buf);
void rageircd_SendNoticeChanops(const char *source, const char *dest, const char *buf);
void rageircd_cmd_notice(const char *source, const char *dest, const char *buf);
void rageircd_cmd_notice2(const char *source, const char *dest, const char *msg);
void rageircd_cmd_privmsg(const char *source, const char *dest, const char *buf);
void rageircd_cmd_privmsg2(const char *source, const char *dest, const char *msg);
void rageircd_SendGlobalNotice(const char *source, const char *dest, const char *msg);
void rageircd_SendGlobalPrivmsg(const char *source, const char *dest, const char *msg);
void rageircd_SendBotOp(const char *nick, const char *chan);
void rageircd_cmd_351(const char *source);
void rageircd_SendQuit(const char *source, const char *buf);
void rageircd_SendPong(const char *servname, const char *who);
void rageircd_SendJoin(const char *user, const char *channel, time_t chantime);
void rageircd_SendSQLineDel(const char *user);
void rageircd_SendInvite(const char *source, const char *chan, const char *nick);
void rageircd_SendPart(const char *nick, const char *chan, const char *buf);
void rageircd_cmd_391(const char *source, const char *timestr);
void rageircd_cmd_250(const char *buf);
void rageircd_cmd_307(const char *buf);
void rageircd_cmd_311(const char *buf);
void rageircd_cmd_312(const char *buf);
void rageircd_cmd_317(const char *buf);
void rageircd_cmd_219(const char *source, const char *letter);
void rageircd_cmd_401(const char *source, const char *who);
void rageircd_cmd_318(const char *source, const char *who);
void rageircd_cmd_242(const char *buf);
void rageircd_cmd_243(const char *buf);
void rageircd_cmd_211(const char *buf);
void rageircd_SendGlobops(const char *source, const char *buf);
void rageircd_SendGlobops_legacy(const char *source, const char *fmt);
void rageircd_SendSQLine(const char *mask, const char *reason);
void rageircd_SendSquit(const char *servname, const char *message);
void rageircd_SendSVSO(const char *source, const char *nick, const char *flag);
void rageircd_SendChangeBotNick(const char *oldnick, const char *newnick);
void rageircd_SendForceNickChange(const char *source, const char *guest, time_t when);
void rageircd_SendVhost(const char *nick, const char *vIdent, const char *vhost);
void rageircd_SendConnect(int servernum);
void rageircd_SendSVSHOLD(const char *nick);
void rageircd_SendSVSHOLDDel(const char *nick);
void rageircd_SendSGLineDel(const char *mask);
void rageircd_SendSZLineDel(const char *mask);
void rageircd_SendSZLine(const char *mask, const char *reason, const char *whom);
void rageircd_SendSGLine(const char *mask, const char *reason);
void rageircd_SendBanDel(const char *name, const char *nick);
void rageircd_SendSVSMode_chan(const char *name, const char *mode, const char *nick);
void rageircd_SendSVID(const char *nick, time_t ts);
void rageircd_SendUnregisteredNick(User * u);
void rageircd_SendSVID2(User * u, const char *ts);
void rageircd_SendSVID3(User * u, const char *ts);
void rageircd_SendEOB();
int rageircd_flood_mode_check(const char *value);
void rageircd_SendJupe(const char *jserver, const char *who, const char *reason);
int rageircd_valid_nick(const char *nick);
void rageircd_SendCTCP(const char *source, const char *dest, const char *buf);

class RageIRCdProto : public IRCDProtoNew {
	public:
		void SendSVSNOOP(const char *, int);
		void SendAkillDel(const char *, const char *);
} ircd_proto;
