/* Hybrid IRCD functions
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

#ifdef IRC_HYBRID

#define PROTECT_SET_MODE "+"
#define PROTECT_UNSET_MODE "-"
#define CS_CMD_PROTECT "PROTECT"
#define CS_CMD_DEPROTECT "DEPROTECT"
#define FANT_PROTECT_ADD "!protect"
#define FANT_PROTECT_DEL "!deprotect"
#define LEVEL_PROTECT_WORD "AUTOPROTECT"
#define LEVELINFO_PROTECT_WORD "PROTECT"
#define LEVELINFO_PROTECTME_WORD "PROTECTME"

#define UMODE_a 0x00000001  /* Admin status */
#define UMODE_b 0x00000080  /* See bot and drone flooding notices */
#define UMODE_c 0x00000100  /* Client connection/quit notices */
#define UMODE_d 0x00000200  /* See debugging notices */
#define UMODE_f 0x00000400  /* See I: line full notices */
#define UMODE_g 0x00000800  /* Server Side Ignore */
#define UMODE_i 0x00000004  /* Not shown in NAMES or WHO unless you share a channel */
#define UMODE_k 0x00001000  /* See server generated KILL messages */
#define UMODE_l 0x00002000  /* See LOCOPS messages */
#define UMODE_n 0x00004000  /* See client nick changes */
#define UMODE_o 0x00000008  /* Operator status */
#define UMODE_r 0x00000010  /* See rejected client notices */
#define UMODE_s 0x00008000  /* See general server notices */
#define UMODE_u 0x00010000  /* See unauthorized client notices */
#define UMODE_w 0x00000020  /* See server generated WALLOPS */
#define UMODE_x 0x00020000  /* See remote server connection and split notices */
#define UMODE_y 0x00040000  /* See LINKS, STATS (if configured), TRACE notices */
#define UMODE_z 0x00080000  /* See oper generated WALLOPS */

#define CMODE_i 0x00000001     /* Invite only */
#define CMODE_m 0x00000002     /* Users without +v/h/o cannot send text to the channel */
#define CMODE_n 0x00000004     /* Users must be in the channel to send text to it */
#define CMODE_p 0x00000008     /* Private is obsolete, this now restricts KNOCK */
#define CMODE_s 0x00000010     /* The channel does not show up on NAMES or LIST */
#define CMODE_t 0x00000020     /* Only chanops can change the topic */
#define CMODE_k 0x00000040     /* Key/password for the channel. */
#define CMODE_l 0x00000080     /* Limit the number of users in a channel */
#define CMODE_a 0x00000400     /* Anonymous ops, chanops are hidden */


#define DEFAULT_MLOCK CMODE_n | CMODE_t

#endif


