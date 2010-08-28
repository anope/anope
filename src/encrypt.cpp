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
int enc_encrypt(const Anope::string &src, Anope::string &dest)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnEncrypt, OnEncrypt(src, dest));
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
int enc_decrypt(const Anope::string &src, Anope::string &dest)
{
	size_t pos = src.find(':');
	if (pos == Anope::string::npos)
	{
		Log() << "Error: enc_decrypt() called with invalid password string (" << src << ")";
		return -1;
	}
	Anope::string hashm(src.begin(), src.begin() + pos);

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
int enc_check_password(Anope::string &plaintext, Anope::string &password)
{
	size_t pos = password.find(':');
	if (pos == Anope::string::npos)
	{
		Log() << "Error: enc_check_password() called with invalid password string (" << password << ")";
		return 0;
	}
	Anope::string hashm(password.begin(), password.begin() + pos);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnCheckPassword, OnCheckPassword(hashm, plaintext, password));
	if (MOD_RESULT == EVENT_ALLOW)
		return 1;
	return 0;
}
