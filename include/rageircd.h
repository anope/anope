/* Rage IRCD functions
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

#ifdef IRC_RAGE2

#define PROTECT_SET_MODE "+a"
#define PROTECT_UNSET_MODE "+a"
#define FANT_PROTECT_ADD "!admin"
#define FANT_PROTECT_DEL "!deadmin"
#define LEVEL_PROTECT_WORD "AUTOADMIN"
#define LEVELINFO_PROTECT_WORD "ADMIN"
#define LEVELINFO_PROTECTME_WORD "ADMINME"
#define CS_CMD_PROTECT "ADMIN"
#define CS_CMD_DEPROTECT "DEADMIN"

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
#define CMODE_A 0x00000800
#define CMODE_N 0x00001000
#define CMODE_S 0x00002000
#define CMODE_C 0x00004000
#define CMODE_c 0x00000400		/* Colors can't be used */
#define CMODE_M 0x00000800              /* Non-regged nicks can't send messages */
#define CMODE_O 0x00008000		/* Only opers can join */
#define CMODE_R 0x00000100		/* Only identified users can join */
#define CMODE_r 0x00000200		/* Set for all registered channels */
#define CMODE_I 0x08000000

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

#endif

