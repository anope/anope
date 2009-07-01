/* Include file for high-level encryption routines.
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$ 
 *
 */

#include "services.h"
#include "modules.h"

/******************************************************************************/

/** 
 * Encrypt string `src' of length `len', placing the result in buffer
 * `dest' of size `size'.  Returns 0 on success, -1 on error.
 **/
int enc_encrypt(const char *src, int len, char *dest, int size)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnEncrypt, OnEncrypt(src, len, dest, size));
	if (MOD_RESULT == EVENT_ALLOW)
		return 0;
	return -1;
}

/**
 * Encrypt null-terminated string stored in buffer `buf' of size `size',
 * placing the result in the same buffer.  Returns 0 on success, -1 on
 * error.
 **/
int enc_encrypt_in_place(char *buf, int size)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnEncryptInPlace, OnEncryptInPlace(buf, size));
	if (MOD_RESULT == EVENT_ALLOW)
	        return 0;
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
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnEncryptCheckLen, OnEncryptCheckLen(passlen, bufsize));
	if (MOD_RESULT == EVENT_ALLOW)
	        return 0;
	return -1;
}

/**
 * Decrypt encrypted string `src' into buffer `dest' of length `len'.
 * Returns 1 (not 0) on success, -1 if the encryption algorithm does not
 * allow decryption, and -1 if another failure occurred (e.g. destination
 * buffer too small).
 **/
int enc_decrypt(const char *src, char *dest, int size)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnDecrypt, OnDecrypt(src, dest, size));
	if (MOD_RESULT == EVENT_ALLOW)
	        return 1;
	return -1;
}

/**
 * Check an input password `plaintext' against a stored, encrypted password
 * `password'.  Return value is:
 *   1 if the password matches
 *   0 if the password does not match
 *   0 if an error occurred while checking
 **/
int enc_check_password(const char *plaintext, const char *password)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnCheckPassword, OnCheckPassword(plaintext, password));
	if (MOD_RESULT == EVENT_ALLOW)
	        return 1;
	return 0;
}

/* EOF */
