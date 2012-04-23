/* Module for plain text encryption.
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 */

#include "module.h"

class ENone : public Module
{
 public:
	ENone(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, ENCRYPTION)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnEncrypt, I_OnDecrypt, I_OnCheckAuthentication };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	EventReturn OnEncrypt(const Anope::string &src, Anope::string &dest) anope_override
	{
		Anope::string buf = "plain:";
		Anope::string cpass;
		Anope::B64Encode(src, cpass);
		buf += cpass;
		Log(LOG_DEBUG_2) << "(enc_none) hashed password from [" << src << "] to [" << buf << "]";
		dest = buf;
		return EVENT_ALLOW;
	}

	EventReturn OnDecrypt(const Anope::string &hashm, const Anope::string &src, Anope::string &dest) anope_override
	{
		if (!hashm.equals_cs("plain"))
			return EVENT_CONTINUE;
		size_t pos = src.find(':');
		Anope::string buf = src.substr(pos + 1);
		Anope::B64Decode(buf, dest);
		return EVENT_ALLOW;
	}

	EventReturn OnCheckAuthentication(Command *c, CommandSource *source, const std::vector<Anope::string> &params, const Anope::string &account, const Anope::string &password) anope_override
	{
		const NickAlias *na = findnick(account);
		if (na == NULL)
			return EVENT_CONTINUE;
		NickCore *nc = na->nc;

		size_t pos = nc->pass.find(':');
		if (pos == Anope::string::npos)
			return EVENT_CONTINUE;
		Anope::string hash_method(nc->pass.begin(), nc->pass.begin() + pos);
		if (!hash_method.equals_cs("plain"))
			return EVENT_CONTINUE;

		Anope::string buf;
		this->OnEncrypt(password, buf);
		if (nc->pass.equals_cs(buf))
		{
			/* if we are NOT the first module in the list,
			 * we want to re-encrypt the pass with the new encryption
			 */
			if (ModuleManager::FindFirstOf(ENCRYPTION) != this)
				enc_encrypt(password, nc->pass);
			return EVENT_ALLOW;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ENone)
