/* Include file for high-level encryption routines.
 *
 * (C) 2003-2011 Anope Team
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

/** Encrypt the string src into dest
 * @param src The source string
 * @param dest The destination strnig
 */
void enc_encrypt(const Anope::string &src, Anope::string &dest)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnEncrypt, OnEncrypt(src, dest));
}

/** Decrypt the encrypted string src into dest
 * @param src The encrypted string
 * @param desc The destination string
 * @return true on success
 */
bool enc_decrypt(const Anope::string &src, Anope::string &dest)
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
		return true;

	return false;
}

