/* Include file for high-level encryption routines.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id: encrypt.h,v 1.4 2003/07/20 01:15:49 dane Exp $ 
 *
 */

extern int encrypt(const char *src, int len, char *dest, int size);
extern int encrypt_in_place(char *buf, int size);
extern int check_password(const char *plaintext, const char *password);
