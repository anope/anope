/* DreamForge IRCD functions
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

#ifdef IRC_DREAMFORGE

#define PROTECT_SET_MODE "+"
#define PROTECT_UNSET_MODE "-"
#define CS_CMD_PROTECT "PROTECT"
#define CS_CMD_DEPROTECT "DEPROTECT"
#define FANT_PROTECT_ADD "!protect"
#define FANT_PROTECT_DEL "!deprotect"
#define LEVEL_PROTECT_WORD "AUTOPROTECT"
#define LEVELINFO_PROTECT_WORD "PROTECT"
#define LEVELINFO_PROTECTME_WORD "PROTECTME"

#define UMODE_a 0x00000001  /* Services Admin */ 
#define UMODE_h 0x00000002  /* Help system operator */
#define UMODE_i 0x00000004  /* makes user invisible */
#define UMODE_o 0x00000008  /* Operator */
#define UMODE_r 0x00000010  /* Nick set by services as registered */
#define UMODE_w 0x00000020  /* send wallops to them */
#define UMODE_A 0x00000040  /* Admin */
#define UMODE_O 0x00000080  /* Local operator */
#define UMODE_s 0x00000100  /* server notices such as kill */
#define UMODE_k 0x00000200  /* Show server-kills... */
#define UMODE_c 0x00000400  /* Show client information */
#define UMODE_f 0x00000800  /* Receive flood warnings */
#define UMODE_g 0x80000000  /* Shows some global messages */

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

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

#endif

