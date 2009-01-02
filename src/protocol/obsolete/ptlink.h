/* PTLink IRCD functions
 *
 * (C) 2003-2009 Anope Team
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
#define UMODE_B 0x00000080
#define UMODE_H 0x00000100
#define UMODE_N 0x00000200
#define UMODE_O 0x00000400
#define UMODE_p 0x00000800
#define UMODE_R 0x00001000
#define UMODE_s 0x00002000
#define UMODE_S 0x00004000
#define UMODE_T 0x00008000
#define UMODE_v 0x00001000
#define UMODE_y 0x00002000
#define UMODE_z 0x00004000

#define UMODE_VH 0x00008000		/* Fake umode used for internal vhost things */
#define UMODE_NM 0x00010000		/* Fake umode used for internal NEWMASK things */
/* Let's hope for a better vhost-system with PTlink7 ;) */


#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040		/* These two used only by ChanServ */
#define CMODE_l 0x00000080
#define CMODE_A 0x00000400
#define CMODE_B 0x00000800
#define CMODE_c 0x00001000
#define CMODE_d 0x00002000
#define CMODE_f 0x00004000
#define CMODE_K 0x00008000
#define CMODE_O 0x00010000
#define CMODE_q 0x00020000
#define CMODE_S 0x00040000
#define CMODE_N 0x00080000
#define CMODE_R 0x00000100		/* Only identified users can join */
#define CMODE_r 0x00000200		/* Set for all registered channels */
#define CMODE_C 0x00100000

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

/*
   The following variables are set to define the TS protocol version
   that we support.

   PTLink 6.14 to 6.17  TS CURRENT is 6  and MIN is 3
   PTlink 6.18		  TS CURRENT is 9  and MIN is 3
   PTLink 6.19 		TS CURRENT is 10 and MIN is 9

   If you are running 6.18 or 6.19 do not touch these values as they will
   allow you to connect

   If you are running an older version of PTLink, first think about updating
   your ircd, or changing the TS_CURRENT to 6 to allow services to connect
*/

#define PTLINK_TS_CURRENT 9
#define PTLINK_TS_MIN 3

void ptlink_ProcessUsermodes(User * user, int ac, const char **av);
void ptlink_cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when);
void ptlink_SendVhostDel(User * u);
void ptlink_SendAkill(const char *user, const char *host, const char *who, time_t when,time_t expires, const char *reason);
void ptlink_SendSVSKill(const char *source, const char *user, const char *buf);
void ptlink_SendSVSMode(User * u, int ac, const char **av);
void ptlink_cmd_372(const char *source, const char *msg);
void ptlink_cmd_372_error(const char *source);
void ptlink_cmd_375(const char *source);
void ptlink_cmd_376(const char *source);
void ptlink_cmd_nick(const char *nick, const char *name, const char *modes);
void ptlink_SendGuestNick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void ptlink_SendMode(const char *source, const char *dest, const char *buf);
void ptlink_SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void ptlink_SendKick(const char *source, const char *chan, const char *user, const char *buf);
void ptlink_SendNoticeChanops(const char *source, const char *dest, const char *buf);
void ptlink_cmd_notice(const char *source, const char *dest, const char *buf);
void ptlink_cmd_notice2(const char *source, const char *dest, const char *msg);
void ptlink_cmd_privmsg(const char *source, const char *dest, const char *buf);
void ptlink_cmd_privmsg2(const char *source, const char *dest, const char *msg);
void ptlink_SendGlobalNotice(const char *source, const char *dest, const char *msg);
void ptlink_SendGlobalPrivmsg(const char *source, const char *dest, const char *msg);
void ptlink_SendBotOp(const char *nick, const char *chan);
void ptlink_cmd_351(const char *source);
void ptlink_SendQuit(const char *source, const char *buf);
void ptlink_SendPong(const char *servname, const char *who);
void ptlink_SendJoin(const char *user, const char *channel, time_t chantime);
void ptlink_SendSQLineDel(const char *user);
void ptlink_SendInvite(const char *source, const char *chan, const char *nick);
void ptlink_SendPart(const char *nick, const char *chan, const char *buf);
void ptlink_cmd_391(const char *source, const char *timestr);
void ptlink_cmd_250(const char *buf);
void ptlink_cmd_307(const char *buf);
void ptlink_cmd_311(const char *buf);
void ptlink_cmd_312(const char *buf);
void ptlink_cmd_317(const char *buf);
void ptlink_cmd_219(const char *source, const char *letter);
void ptlink_cmd_401(const char *source, const char *who);
void ptlink_cmd_318(const char *source, const char *who);
void ptlink_cmd_242(const char *buf);
void ptlink_cmd_243(const char *buf);
void ptlink_cmd_211(const char *buf);
void ptlink_SendGlobops(const char *source, const char *buf);
void ptlink_SendGlobops_legacy(const char *source, const char *fmt);
void ptlink_SendSQLine(const char *mask, const char *reason);
void ptlink_SendSquit(const char *servname, const char *message);
void ptlink_SendSVSO(const char *source, const char *nick, const char *flag);
void ptlink_SendChangeBotNick(const char *oldnick, const char *newnick);
void ptlink_SendForceNickChange(const char *source, const char *guest, time_t when);
void ptlink_SendVhost(const char *nick, const char *vIdent, const char *vhost);
void ptlink_SendConnect(int servernum);
void ptlink_SendSVSHOLD(const char *nick);
void ptlink_SendSVSHOLDDel(const char *nick);
void ptlink_SendSGLineDel(const char *mask);
void ptlink_SendSZLineDel(const char *mask);
void ptlink_SendSZLine(const char *mask, const char *reason, const char *whom);
void ptlink_SendSGLine(const char *mask, const char *reason);
void ptlink_SendBanDel(const char *name, const char *nick);
void ptlink_SendSVSMode_chan(const char *name, const char *mode, const char *nick);
void ptlink_SendSVID(const char *nick, time_t ts);
void ptlink_SendUnregisteredNick(User * u);
void ptlink_SendSVID2(User * u, const char *ts);
void ptlink_SendSVID3(User * u, const char *ts);
void ptlink_SendEOB();
int ptlink_IsFloodModeParamValid(const char *value);
void ptlink_SendJupe(const char *jserver, const char *who, const char *reason);
int ptlink_IsNickValid(const char *nick);
void ptlink_SendCTCP(const char *source, const char *dest, const char *buf);

class PTlinkProto : public IRCDProtoNew {
	public:
		void SendSVSNOOP(const char *, int);
		void SendAkillDel(const char *, const char *);
} ircd_proto;
