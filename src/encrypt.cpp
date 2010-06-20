/* Include file for high-level encryption routines.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"

/******************************************************************************/

/**
 * Encrypt string `src' of length `len', placing the result in buffer
 * `dest' of size `size'.  Returns 0 on success, -1 on error.
 **/
int enc_encrypt(const std::string &src, std::string &dest)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnEncrypt, OnEncrypt(src, dest));
	if (MOD_RESULT == EVENT_ALLOW)
		return 0;
	return -1;
}

/**
 * Encrypt null-terminated string stored in buffer `buf' of size `size',
 * placing the result in the same buffer.  Returns 0 on success, -1 on
 * error.
 **/
int enc_encrypt_in_place(std::string &buf)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnEncryptInPlace, OnEncryptInPlace(buf));
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
int enc_decrypt(const std::string &src, std::string &dest)
{
	size_t pos = src.find(":");
	if (pos == std::string::npos)
	{
		Alog() << "Error: enc_decrypt() called with invalid password string (" << src << ")";
		return -1;
	}
	std::string hashm(src.begin(), src.begin() + pos);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnDecrypt, OnDecrypt(hashm, src, dest));
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
int enc_check_password(std::string &plaintext, std::string &password)
{
	std::string hashm;
	size_t pos = password.find(":");
	if (pos == std::string::npos)
	{
		Alog() << "Error: enc_check_password() called with invalid password string (" << password << ")";
		return 0;
	}
	hashm.assign(password.begin(), password.begin() + pos);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnCheckPassword, OnCheckPassword(hashm, plaintext, password));
	if (MOD_RESULT == EVENT_ALLOW)
		return 1;
	return 0;
}
