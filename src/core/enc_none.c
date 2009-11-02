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
		ModuleManager::Attach(I_OnEncryptCheckLen, this);
		ModuleManager::Attach(I_OnDecrypt, this);
		ModuleManager::Attach(I_OnCheckPassword, this);
	}

	EventReturn OnEncrypt(const char *src,int len,char *dest,int size)
	{
		if(size>=len)
		{
			memset(dest,0,size);
			strlcpy(dest,src,len);
			return EVENT_ALLOW;
		}
		return EVENT_STOP;
	}

	EventReturn OnEncryptInPlace(char *buf, int size)
	{
		return EVENT_ALLOW;
	}

	EventReturn OnEncryptCheckLen(int passlen, int bufsize)
	{
		if(bufsize>=passlen)
		{
			return EVENT_ALLOW;
		}
		return EVENT_STOP;
	}

	EventReturn OnDecrypt(const char *src, char *dest, int size) {
		memset(dest,0,size);
		strlcpy(dest,src,size);
		return EVENT_ALLOW;
	}

	EventReturn OnCheckPassword(const char *plaintext, char *password) 
	{
		if(strcmp(plaintext,password)==0)
		{
			/* if we are NOT the first module in the list, 
			 * we want to re-encrypt the pass with the new encryption
			 */
			if (stricmp(EncModuleList[0], this->name.c_str()))
			{
				enc_encrypt(plaintext, strlen(password), password, PASSMAX -1 );
			}
			return EVENT_ALLOW;
		}
		return EVENT_CONTINUE;
	}

};
/* EOF */


MODULE_INIT(ENone)
