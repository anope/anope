/* Viagra IRCD functions
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

void viagra_set_umode(User * user, int ac, const char **av);
void viagra_cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when);
void viagra_SendVhostDel(User * u);
void viagra_SendAkill(const char *user, const char *host, const char *who, time_t when,time_t expires, const char *reason);
void viagra_SendSVSKill(const char *source, const char *user, const char *buf);
void viagra_SendSVSMode(User * u, int ac, const char **av);
void viagra_cmd_372(const char *source, const char *msg);
void viagra_cmd_372_error(const char *source);
void viagra_cmd_375(const char *source);
void viagra_cmd_376(const char *source);
void viagra_cmd_nick(const char *nick, const char *name, const char *modes);
void viagra_SendGuestNick(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void viagra_SendMode(const char *source, const char *dest, const char *buf);
void viagra_SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes);
void viagra_SendKick(const char *source, const char *chan, const char *user, const char *buf);
void viagra_SendNoticeChanops(const char *source, const char *dest, const char *buf);
void viagra_cmd_notice(const char *source, const char *dest, const char *buf);
void viagra_cmd_notice2(const char *source, const char *dest, const char *msg);
void viagra_cmd_privmsg(const char *source, const char *dest, const char *buf);
void viagra_cmd_privmsg2(const char *source, const char *dest, const char *msg);
void viagra_SendGlobalNotice(const char *source, const char *dest, const char *msg);
void viagra_SendGlobalPrivmsg(const char *source, const char *dest, const char *msg);
void viagra_SendBotOp(const char *nick, const char *chan);
void viagra_cmd_351(const char *source);
void viagra_SendQuit(const char *source, const char *buf);
void viagra_SendPong(const char *servname, const char *who);
void viagra_SendJoin(const char *user, const char *channel, time_t chantime);
void viagra_SendSQLineDel(const char *user);
void viagra_SendInvite(const char *source, const char *chan, const char *nick);
void viagra_SendPart(const char *nick, const char *chan, const char *buf);
void viagra_cmd_391(const char *source, const char *timestr);
void viagra_cmd_250(const char *buf);
void viagra_cmd_307(const char *buf);
void viagra_cmd_311(const char *buf);
void viagra_cmd_312(const char *buf);
void viagra_cmd_317(const char *buf);
void viagra_cmd_219(const char *source, const char *letter);
void viagra_cmd_401(const char *source, const char *who);
void viagra_cmd_318(const char *source, const char *who);
void viagra_cmd_242(const char *buf);
void viagra_cmd_243(const char *buf);
void viagra_cmd_211(const char *buf);
void viagra_SendGlobops(const char *source, const char *buf);
void viagra_SendGlobops_legacy(const char *source, const char *fmt);
void viagra_SendSQLine(const char *mask, const char *reason);
void viagra_SendSquit(const char *servname, const char *message);
void viagra_SendSVSO(const char *source, const char *nick, const char *flag);
void viagra_SendChangeBotNick(const char *oldnick, const char *newnick);
void viagra_SendForceNickChange(const char *source, const char *guest, time_t when);
void viagra_SendVhost(const char *nick, const char *vIdent, const char *vhost);
void viagra_cmd_connect(int servernum);
void viagra_cmd_svshold(const char *nick);
void viagra_cmd_release_svshold(const char *nick);
void viagra_cmd_unsgline(const char *mask);
void viagra_cmd_unszline(const char *mask);
void viagra_cmd_szline(const char *mask, const char *reason, const char *whom);
void viagra_cmd_sgline(const char *mask, const char *reason);
void viagra_cmd_unban(const char *name, const char *nick);
void viagra_SendSVSMode_chan(const char *name, const char *mode, const char *nick);
void viagra_cmd_svid_umode(const char *nick, time_t ts);
void viagra_cmd_nc_change(User * u);
void viagra_cmd_svid_umode2(User * u, const char *ts);
void viagra_cmd_svid_umode3(User * u, const char *ts);
void viagra_cmd_eob();
int viagra_flood_mode_check(const char *value);
void viagra_cmd_jupe(const char *jserver, const char *who, const char *reason);
int viagra_valid_nick(const char *nick);
void viagra_cmd_ctcp(const char *source, const char *dest, const char *buf);

class ViagraIRCdProto : public IRCDProtoNew {
	public:
		void SendSVSNOOP(const char *, int);
		void SendAkillDel(const char *, const char *);
} ircd_proto;
