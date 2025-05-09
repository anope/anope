/* Module for plain text encryption.
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 */

#include "module.h"

class ENone final
	: public Module
{
public:
	ENone(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, ENCRYPTION | VENDOR)
	{
		if (ModuleManager::FindFirstOf(ENCRYPTION) == this)
			throw ModuleException("enc_none is deprecated and can not be used as a primary encryption method");
	}

	void OnCheckAuthentication(User *, IdentifyRequest *req) override
	{
		const auto *na = NickAlias::Find(req->GetAccount());
		if (!na)
			return;

		NickCore *nc = na->nc;
		auto pos = nc->pass.find(':');
		if (pos == Anope::string::npos)
			return;

		Anope::string hash_method(nc->pass.begin(), nc->pass.begin() + pos);
		if (!hash_method.equals_cs("plain"))
			return;

		Anope::string b64pass;
		Anope::B64Encode(req->GetPassword(), b64pass);
		auto enc = "plain:" + b64pass;
		if (nc->pass.equals_cs(enc))
		{
			// If we are NOT the first encryption module we want to re-encrypt
			// the password with the primary encryption method.
			if (ModuleManager::FindFirstOf(ENCRYPTION) != this)
				Anope::Encrypt(req->GetPassword(), nc->pass);
			req->Success(this);
		}
	}
};

MODULE_INIT(ENone)
