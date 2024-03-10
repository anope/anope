/* Include file for high-level encryption routines.
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/encryption.h"

class EOld final
	: public Module
{
private:
	ServiceReference<Encryption::Provider> md5;

	inline static char XTOI(char c)
	{
		return c > 9 ? c - 'A' + 10 : c - '0';
	}

public:
	EOld(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, ENCRYPTION | VENDOR)
		, md5("Encryption::Provider", "md5")
	{
		if (ModuleManager::FindFirstOf(ENCRYPTION) == this)
			throw ModuleException("enc_old is deprecated and can not be used as a primary encryption method");

		ModuleManager::LoadModule("enc_md5", User::Find(creator, true));
		if (!md5)
			throw ModuleException("Unable to find md5 reference");
	}

	EventReturn OnEncrypt(const Anope::string &src, Anope::string &dest) override
	{
		if (!md5)
			return EVENT_CONTINUE;

		char digest[32];
		memset(digest, 0, sizeof(digest));

		auto hash = md5->Encrypt(src);
		if (hash.length() != sizeof(digest))
			return EVENT_CONTINUE; // Probably a bug?
		memcpy(digest, hash.data(), hash.length());

		char digest2[16];
		for (size_t i = 0; i < sizeof(digest); i += 2)
			digest2[i / 2] = XTOI(digest[i]) << 4 | XTOI(digest[i + 1]);

		auto enc = "oldmd5:" + Anope::Hex(digest2, sizeof(digest2));
		Log(LOG_DEBUG_2) << "(enc_old) hashed password from [" << src << "] to [" << enc << "]";
		dest = enc;
		return EVENT_ALLOW;
	}

	void OnCheckAuthentication(User *, IdentifyRequest *req) override
	{
		const NickAlias *na = NickAlias::Find(req->GetAccount());
		if (!na)
			return;

		NickCore *nc = na->nc;
		size_t pos = nc->pass.find(':');
		if (pos == Anope::string::npos)
			return;

		Anope::string hash_method(nc->pass.begin(), nc->pass.begin() + pos);
		if (!hash_method.equals_cs("oldmd5"))
			return;

		Anope::string buf;
		this->OnEncrypt(req->GetPassword(), buf);
		if (nc->pass.equals_cs(buf))
		{
			// If we are NOT the first encryption module we want to re-encrypt
			// the password with the primary encryption method.
			if (ModuleManager::FindFirstOf(ENCRYPTION) != this)
				Anope::Encrypt(req->GetPassword(), nc->pass);
			req->Success(this);
		}
	}
};

MODULE_INIT(EOld)
