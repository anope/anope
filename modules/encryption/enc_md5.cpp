/* Module for encryption using MD5.
 *
 * Modified for Anope.
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Taken from IRC Services and is copyright (c) 1996-2002 Andrew Church.
 *	 Email: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 */

#include "module.h"
#include "modules/encryption.h"

#include "md5/md5.c"

class MD5Context final
	: public Encryption::Context
{
private:
	MD5_CTX context;

public:
	MD5Context()
	{
		MD5_Init(&context);
	}

	void Update(const unsigned char *input, size_t len) override
	{
		MD5_Update(&context, input, len);
	}

	Anope::string Finalize() override
	{
		unsigned char digest[16];
		MD5_Final(digest, &context);
		return Anope::string(reinterpret_cast<const char *>(&digest), sizeof(digest));
	}
};

class EMD5 final
	: public Module
{
private:
	Encryption::SimpleProvider<MD5Context> md5provider;

public:
	EMD5(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, ENCRYPTION | VENDOR)
		, md5provider(this, "md5", 16, 64)
	{
		if (ModuleManager::FindFirstOf(ENCRYPTION) == this)
			throw ModuleException("enc_md5 is deprecated and can not be used as a primary encryption method");

		md5provider.Check({
			{ "d41d8cd98f00b204e9800998ecf8427e", "" },
			{ "9e107d9d372bb6826bd81d3542a419d6", "The quick brown fox jumps over the lazy dog" },
		});
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
		if (!hash_method.equals_cs("md5"))
			return;

		auto enc = "md5:" + Anope::Hex(md5provider.Encrypt(req->GetPassword()));
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

MODULE_INIT(EMD5)
