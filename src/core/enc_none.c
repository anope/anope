/* Module for plain text encryption.
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 */

#include "module.h"

class ENone : public Module
{
 public:
	ENone(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(ENCRYPTION);

		ModuleManager::Attach(I_OnEncrypt, this);
		ModuleManager::Attach(I_OnEncryptInPlace, this);
		ModuleManager::Attach(I_OnDecrypt, this);
		ModuleManager::Attach(I_OnCheckPassword, this);
	}

	EventReturn OnEncrypt(const std::string &src, std::string &dest)
	{
		std::string buf = "plain:";
		char cpass[1000];
		b64_encode(src.c_str(), src.size(), cpass, 1000);
		buf.append(cpass);
		if (debug > 1)
			alog("debug: (enc_none) hashed password from [%s] to [%s]", src.c_str(), buf.c_str());
		dest.assign(buf);
		return EVENT_ALLOW;
	}

	EventReturn OnEncryptInPlace(std::string &buf)
	{
		return this->OnEncrypt(buf, buf);
	}

	EventReturn OnDecrypt(const std::string &hashm, const std::string &src, std::string &dest)
	{
		if (hashm != "plain")
			return EVENT_CONTINUE;
		char cpass[1000];
		size_t pos = src.find(":");
		std::string buf(src.begin()+pos+1, src.end());
		b64_decode(buf.c_str(), static_cast<char *>(cpass), 1000);
		dest.assign(cpass);
		return EVENT_ALLOW;
	}

	EventReturn OnCheckPassword(const std::string &hashm, std::string &plaintext, std::string &password) 
	{
		if (hashm != "plain")
			return EVENT_CONTINUE;
		std::string buf;
		this->OnEncrypt(plaintext, buf);
		if(!password.compare(buf))
		{
			/* if we are NOT the first module in the list, 
			 * we want to re-encrypt the pass with the new encryption
			 */
			if (Config.EncModuleList.front().compare(this->name))
			{
				enc_encrypt(plaintext, password);
			}
			return EVENT_ALLOW;
		}
		return EVENT_STOP;
	}

};

MODULE_INIT(ENone)
