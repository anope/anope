/* PTLink IRCD functions
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

#ifdef IRC_PTLINK


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
   PTlink 6.18          TS CURRENT is 9  and MIN is 3
   PTLink 6.19 		TS CURRENT is 10 and MIN is 9

   If you are running 6.18 or 6.19 do not touch these values as they will
   allow you to connect

   If you are running an older version of PTLink, first think about updating
   your ircd, or changing the TS_CURRENT to 6 to allow services to connect
*/

#define PTLINK_TS_CURRENT 9
#define PTLINK_TS_MIN 3


#endif


