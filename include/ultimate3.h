/* Ultimate IRCD 3.0 functions
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

#ifdef IRC_ULTIMATE3


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

#endif

