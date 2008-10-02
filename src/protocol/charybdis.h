/* Ratbox IRCD functions
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

#define UMODE_a 0x00000001 /* umode a - admin */
#define UMODE_g 0x00000002 /* umode g - caller ID */
#define UMODE_i 0x00000004 /* umode i - invisible */
#define UMODE_o 0x00000008 /* umode o - operator */
#define UMODE_z 0x00000010 /* umode u - operwall */
#define UMODE_w 0x00000020 /* umode w - wallops */
#define UMODE_s 0x00000040 /* umode s - server notices */
#define UMODE_Q 0x00000080 /* umode Q - block forwarding */
#define UMODE_R 0x00000200 /* umode R - reject messages from unauthenticated users */
#define UMODE_S 0x00000400 /* umode S - network service */
#define UMODE_l 0x00020000 /* umode l - locops */


#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040
#define CMODE_l 0x00000080
#define CMODE_f 0x00000100
#define CMODE_c 0x00000200
#define CMODE_r 0x00000400
#define CMODE_P 0x00000800
#define CMODE_g 0x00001000
#define CMODE_z 0x00002000
#define CMODE_j 0x00004000
#define CMODE_F 0x00008000
#define CMODE_L 0x00010000
#define CMODE_Q 0x00020000

#define DEFAULT_MLOCK CMODE_n | CMODE_t


class CharybdisProto : public IRCDTS6Proto {
		void SendSVSKillInternal(const char *, const char *, const char *);
		void SendModeInternal(const char *, const char *, const char *);
		void SendKickInternal(const char *, const char *, const char *, const char *);
		void SendNoticeChanopsInternal(const char *, const char *, const char *);
	public:
		void SendAkillDel(const char *, const char *);
		void SendVhostDel(User *);
		void SendAkill(const char *, const char *, const char *, time_t, time_t, const char *);
		void SendSVSMode(User *, int, const char **);
		void SendClientIntroduction(const char *, const char *, const char *, const char *, const char *);
		void SendBotOp(const char *, const char *);
		void SendQuit(const char *, const char *);
		void SendPong(const char *, const char *);
		void SendJoin(const char *, const char *, time_t);
		void SendSQLineDel(const char *);
		void SendInvite(const char *, const char *, const char *);
		void SendPart(const char *, const char *, const char *);
		void SendGlobops(const char *, const char *);
		void SendSQLine(const char *, const char *);
		void SendForceNickChange(const char *, const char *, time_t);
		void SendVhost(const char *, const char *, const char *);
		void SendConnect();
		void SendSVSHOLD(const char *);
		void SendSVSHOLDDel(const char *);
		void SendSGLineDel(const char *);
		void SendSGLine(const char *, const char *);
		void SendServer(const char *, int, const char *);
		void ProcessUsermodes(User *, int, const char **);
		int IsNickValid(const char *);
		int IsFloodModeParamValid(const char *);
		void SendNumeric(const char *, int, const char *, const char *);
} ircd_proto;
