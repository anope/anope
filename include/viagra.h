/* Viagra IRCD functions
 *
 * (C) 2003-2005 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

#ifdef IRC_VIAGRA


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

#endif

