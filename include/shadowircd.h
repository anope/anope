/* ShadowIRCD functions
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

#ifdef IRC_SHADOWIRCD

/* The protocol revision. */
#define PROTOCOL_REVISION 3402


#define UMODE_a 0x00000001
#define UMODE_C 0x00000002
#define UMODE_i 0x00000004
#define UMODE_o 0x00000008
#define UMODE_z 0x00000010
#define UMODE_w 0x00000020
#define UMODE_s 0x00000040
#define UMODE_c 0x00000080
#define UMODE_r 0x00000100
#define UMODE_k 0x00000200
#define UMODE_f 0x00000400
#define UMODE_y 0x00000800
#define UMODE_d 0x00001000
#define UMODE_n 0x00002000
#define UMODE_x 0x00004000
#define UMODE_u 0x00008000
#define UMODE_b 0x00010000
#define UMODE_l 0x00020000
#define UMODE_g 0x00040000
#define UMODE_v 0x00080000
#define UMODE_A 0x00100000
#define UMODE_E 0x00200000
#define UMODE_G 0x00400000
#define UMODE_R 0x00800000
#define UMODE_e 0x01000000
#define UMODE_O 0x02000000
#define UMODE_H 0x04000000

#define CMODE_i 0x00000001
#define CMODE_m 0x00000002
#define CMODE_n 0x00000004
#define CMODE_p 0x00000008
#define CMODE_s 0x00000010
#define CMODE_t 0x00000020
#define CMODE_k 0x00000040
#define CMODE_l 0x00000080
#define CMODE_K 0x00000100
#define CMODE_A 0x00000200
#define CMODE_r 0x00000400
#define CMODE_z 0x00000800
#define CMODE_S 0x00001000
#define CMODE_c 0x00002000
#define CMODE_E 0x00004000
#define CMODE_F 0x00008000
#define CMODE_G 0x00010000
#define CMODE_L 0x00020000
#define CMODE_N 0x00040000
#define CMODE_O 0x00080000
#define CMODE_P 0x00100000
#define CMODE_R 0x00200000
#define CMODE_T 0x00400000
#define CMODE_V 0x00800000

#define DEFAULT_MLOCK CMODE_n | CMODE_t | CMODE_r

#endif


