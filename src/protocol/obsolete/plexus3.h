/* PlexusIRCD IRCD functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#define UMODE_i 0x00000001
#define UMODE_o 0x00000002
#define UMODE_w 0x00000004
#define UMODE_a 0x00000008
#define UMODE_x 0x00000010
#define UMODE_S 0x00000020
#define UMODE_r 0x00000040
#define UMODE_R 0x00000080
#define UMODE_N 0x00000100

#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040		/* These two used only by ChanServ */
#define CMODE_l 0x00000080
#define CMODE_M 0x00001000
#define CMODE_c 0x00002000
#define CMODE_O 0x00004000
#define CMODE_R 0x00008000
#define CMODE_N 0x00010000
#define CMODE_B 0x00020000
#define CMODE_S 0x00040000

#define DEFAULT_MLOCK CMODE_n | CMODE_t

void plexus_ProcessUsermodes(User * user, int ac, const char **av);
void plexus_cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when);
void plexus_SendVhostDel(User * u);
void plexus_SendAkill(const char *user, const char *host, const char *who, time_t when,time_t expires, const char *reason);
void plexus_SendSVSKill(const char *source, const char *user, const char *buf);
void plexus_SendSVSMode(User * u, int ac, const char **av);
void plexus_cmd_372(const char *source, const char *msg);
void plexus_cmd_372_error(const char *source);
void plexus_cmd_375(const char *source);
void plexus_cmd_376(const char *source);
void plexus_cmd_nick(const char *nick, const char *name, const char *modes);
void plexus_SendGuestNick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void plexus_SendMode(const char *source, const char *dest, const char *buf);
void plexus_SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void plexus_SendKick(const char *source, const char *chan, const char *user, const char *buf);
void plexus_SendNoticeChanops(const char *source, const char *dest, const char *buf);
void plexus_cmd_notice(const char *source, const char *dest, const char *buf);
void plexus_cmd_notice2(const char *source, const char *dest, const char *msg);
void plexus_cmd_privmsg(const char *source, const char *dest, const char *buf);
void plexus_cmd_privmsg2(const char *source, const char *dest, const char *msg);
void plexus_SendGlobalNotice(const char *source, const char *dest, const char *msg);
void plexus_SendGlobalPrivmsg(const char *source, const char *dest, const char *msg);
void plexus_SendBotOp(const char *nick, const char *chan);
void plexus_cmd_351(const char *source);
void plexus_SendQuit(const char *source, const char *buf);
void plexus_SendPong(const char *servname, const char *who);
void plexus_SendJoin(const char *user, const char *channel, time_t chantime);
void plexus_SendSQLineDel(const char *user);
void plexus_SendInvite(const char *source, const char *chan, const char *nick);
void plexus_SendPart(const char *nick, const char *chan, const char *buf);
void plexus_cmd_391(const char *source, const char *timestr);
void plexus_cmd_250(const char *buf);
void plexus_cmd_307(const char *buf);
void plexus_cmd_311(const char *buf);
void plexus_cmd_312(const char *buf);
void plexus_cmd_317(const char *buf);
void plexus_cmd_219(const char *source, const char *letter);
void plexus_cmd_401(const char *source, const char *who);
void plexus_cmd_318(const char *source, const char *who);
void plexus_cmd_242(const char *buf);
void plexus_cmd_243(const char *buf);
void plexus_cmd_211(const char *buf);
void plexus_SendGlobops(const char *source, const char *buf);
void plexus_SendGlobops_legacy(const char *source, const char *fmt);
void plexus_SendSQLine(const char *mask, const char *reason);
void plexus_SendSquit(const char *servname, const char *message);
void plexus_SendSVSO(const char *source, const char *nick, const char *flag);
void plexus_SendChangeBotNick(const char *oldnick, const char *newnick);
void plexus_SendForceNickChange(const char *source, const char *guest, time_t when);
void plexus_SendVhost(const char *nick, const char *vIdent, const char *vhost);
void plexus_SendConnect(int servernum);
void plexus_SendSVSHOLD(const char *nick);
void plexus_SendSVSHOLDDel(const char *nick);
void plexus_SendSGLineDel(const char *mask);
void plexus_SendSZLineDel(const char *mask);
void plexus_SendSZLine(const char *mask, const char *reason, const char *whom);
void plexus_SendSGLine(const char *mask, const char *reason);
void plexus_SendBanDel(const char *name, const char *nick);
void plexus_SendSVSMode_chan(const char *name, const char *mode, const char *nick);
void plexus_SendSVID(const char *nick, time_t ts);
void plexus_SendUnregisteredNick(User * u);
void plexus_SendSVID2(User * u, const char *ts);
void plexus_SendSVID3(User * u, const char *ts);
void plexus_SendEOB();
int plexus_IsFloodModeParamValid(const char *value);
void plexus_SendJupe(const char *jserver, const char *who, const char *reason);
int plexus_IsNickValid(const char *nick);
void plexus_SendCTCP(const char *source, const char *dest, const char *buf);

class PleXusIRCdProto : public IRCDProtoNew {
	public:
		void SendSVSNOOP(const char *, int);
		void SendAkillDel(const char *, const char *);
} ircd_proto;
