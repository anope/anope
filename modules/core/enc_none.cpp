/* Module for plain text encryption.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 */

#include "module.h"

class ENone : public Module
{
 public:
	ENone(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(ENCRYPTION);

		ModuleManager::Attach(I_OnEncrypt, this);
		ModuleManager::Attach(I_OnDecrypt, this);
		ModuleManager::Attach(I_OnCheckPassword, this);
	}

	EventReturn OnEncrypt(const Anope::string &src, Anope::string &dest)
	{
		Anope::string buf = "plain:";
		Anope::string cpass;
		b64_encode(src, cpass);
		buf += cpass;
		Log(LOG_DEBUG_2) << "(enc_none) hashed password from [" << src << "] to [" << buf << "]";
		dest = buf;
		return EVENT_ALLOW;
	}

	EventReturn OnDecrypt(const Anope::string &hashm, const Anope::string &src, Anope::string &dest)
	{
		if (!hashm.equals_cs("plain"))
			return EVENT_CONTINUE;
		size_t pos = src.find(':');
		Anope::string buf = src.substr(pos + 1);
		b64_decode(buf, dest);
		return EVENT_ALLOW;
	}

	EventReturn OnCheckPassword(const Anope::string &hashm, Anope::string &plaintext, Anope::string &password)
	{
		if (!hashm.equals_cs("plain"))
			return EVENT_CONTINUE;
		Anope::string buf;
		this->OnEncrypt(plaintext, buf);
		if (password.equals_cs(buf))
		{
			/* if we are NOT the first module in the list,
			 * we want to re-encrypt the pass with the new encryption
			 */
			if (!this->name.equals_ci(Config->EncModuleList.front()))
				enc_encrypt(plaintext, password);
			return EVENT_ALLOW;
		}
		return EVENT_STOP;
	}
};

MODULE_INIT(ENone)
