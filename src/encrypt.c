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

#include "services.h"
#include "encrypt.h"

Encryption encryption;

/******************************************************************************/
void encmodule_encrypt(int (*func)
                        (const char *src, int len, char *dest, int size))
{
    encryption.encrypt = func;
}

void encmodule_encrypt_check_len(int (*func) (int passlen, int bufsize))
{
    encryption.encrypt_check_len = func;
}

void encmodule_decrypt(int (*func) (const char *src, char *dest, int size))
{
    encryption.decrypt = func;
}

void encmodule_check_password(int (*func)
                               (const char *plaintext,
                                const char *password))
{
    encryption.check_password = func;
}

/******************************************************************************/


/** 
 * Encrypt string `src' of length `len', placing the result in buffer
 * `dest' of size `size'.  Returns 0 on success, -1 on error.
 **/
int enc_encrypt(const char *src, int len, char *dest, int size)
{
    if (encryption.encrypt) {
        return encryption.encrypt(src, len, dest, size);
    }
    return -1;
}

/** 
 * Check whether the result of encrypting a password of length `passlen'
 * will fit in a buffer of size `bufsize'.  Returns 0 if the encrypted
 * password would fit in the buffer, otherwise returns the maximum length
 * password that would fit (this value will be smaller than `passlen').
 * If the result of encrypting even a 1-byte password would exceed the
 * specified buffer size, generates a fatal error.
 **/
int enc_encrypt_check_len(int passlen, int bufsize)
{
    if (encryption.encrypt_check_len) {
        return encryption.encrypt_check_len(passlen, bufsize);
    }
    return -1;
}

/**
 * Decrypt encrypted string `src' into buffer `dest' of length `len'.
 * Returns 1 (not 0) on success, 0 if the encryption algorithm does not
 * allow decryption, and -1 if another failure occurred (e.g. destination
 * buffer too small).
 **/
int enc_decrypt(const char *src, char *dest, int size)
{
    if (encryption.decrypt) {
        return encryption.decrypt(src, dest, size);
    }
    return -1;
}

/**
 * Check an input password `plaintext' against a stored, encrypted password
 * `password'.  Return value is:
 *   1 if the password matches
 *   0 if the password does not match
 *   -1 if an error occurred while checking
 **/
int enc_check_password(const char *plaintext, const char *password)
{
    if (encryption.check_password) {
        return encryption.check_password(plaintext, password);
    }
    return -1;
}

/* EOF */
