/* Bahamut functions
 *
 * (C) 2003 Anope Team
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

#ifdef IRC_SOLID

#define PROTECT_SET_MODE "+"
#define PROTECT_UNSET_MODE "-"
#define CS_CMD_PROTECT "PROTECT"
#define CS_CMD_DEPROTECT "DEPROTECT"
#define FANT_PROTECT_ADD "!protect"
#define FANT_PROTECT_DEL "!deprotect"
#define LEVEL_PROTECT_WORD "AUTOPROTECT"
#define LEVELINFO_PROTECT_WORD "PROTECT"
#define LEVELINFO_PROTECTME_WORD "PROTECTME"

#define UMODE_a 0x00000001  /* umode +a - Services Admin */
#define UMODE_h 0x00000002  /* umode +h - Helper */
#define UMODE_i 0x00000004  /* umode +i - Invisible */
#define UMODE_o 0x00000008  /* umode +o - Oper */
#define UMODE_r 0x00000010  /* umode +r - registered nick */
#define UMODE_w 0x00000020  /* umode +w - Get wallops */
#define UMODE_A 0x00000040  /* umode +A - Server Admin */
#define UMODE_x 0x00000080  /* umode +x - Squelch with notice */
#define UMODE_X 0x00000100  /* umode +X - Squelch without notice */
#define UMODE_F 0x00000200  /* umode +F - no cptr->since message rate throttle */
#define UMODE_j 0x00000400  /* umode +j - client rejection notices */
#define UMODE_K 0x00000800  /* umode +K - U: lined server kill messages */
#define UMODE_O 0x00001000  /* umode +O - Local Oper */
#define UMODE_s 0x00002000  /* umode +s - Server notices */
#define UMODE_c 0x00004000  /* umode +c - Client connections/exits */
#define UMODE_k 0x00008000  /* umode +k - Server kill messages */
#define UMODE_f 0x00010000  /* umode +f - Server flood messages */
#define UMODE_y 0x00020000  /* umode +y - Stats/links */
#define UMODE_d 0x00040000  /* umode +d - Debug info */
#define UMODE_g 0x00080000  /* umode +g - Globops */
#define UMODE_b 0x00100000  /* umode +b - Chatops */
#define UMODE_n 0x00200000  /* umode +n - Routing Notices */
#define UMODE_m 0x00400000  /* umode +m - spambot notices */
#define UMODE_e 0x00800000  /* umode +e - oper notices for the above +D */
#define UMODE_D 0x01000000  /* umode +D - Hidden dccallow umode */
#define UMODE_I 0x02000000  /* umode +I - invisible oper (masked) */
#define UMODE_C 0x04000000  /* umode +C - conops */
#define UMODE_v 0x10000000	/* umode +v - hostmasking */
#define UMODE_H 0x20000000  /* umode +H - Oper Hiding */
#define UMODE_z 0x40000000  /* umode +z - SSL */
#define UMODE_R 0x80000000  /* umode +R - No non registered msgs */

#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040	/* These two used only by ChanServ */
#define CMODE_l 0x00000080
#define CMODE_R 0x00000100	/* Only identified users can join */
#define CMODE_r 0x00000200	/* Set for all registered channels */
#define CMODE_c 0x00000400	/* Colors can't be used */
#define CMODE_M 0x00000800      /* Non-regged nicks can't send messages */
#define CMODE_j 0x00001000      /* join throttle */
#define CMODE_O 0x00008000	/* Only opers can join */

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

#endif

