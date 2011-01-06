/* Include file for high-level encryption routines.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

typedef struct encryption_ {
    int (*encrypt)(const char *src, int len, char *dest, int size);
    int (*encrypt_check_len)(int passlen, int bufsize);
    int (*decrypt)(const char *src, char *dest, int size);
    int (*check_password)(const char *plaintext, const char *password);
} Encryption;

