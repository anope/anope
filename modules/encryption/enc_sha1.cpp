// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

/// BEGIN CMAKE
/// target_link_libraries(${SO} PRIVATE "vendored_sha1")
/// END CMAKE

#include "module.h"
#include "modules/encryption.h"

#include "sha1/sha1.h"

class SHA1Context final
	: public Encryption::Context
{
private:
	SHA1_CTX context;

public:
	SHA1Context()
	{
		SHA1Init(&context);
	}

	void Update(const unsigned char *input, size_t len) override
	{
		SHA1Update(&context, input, len);
	}

	Anope::string Finalize() override
	{
		unsigned char digest[20];
		SHA1Final(digest, &context);
		return Anope::string(reinterpret_cast<const char *>(&digest), sizeof(digest));
	}
};

class ESHA1 final
	: public Module
{
private:
	Encryption::SimpleProvider<SHA1Context> sha1provider;

public:
	ESHA1(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, DEPRECATED | ENCRYPTION | VENDOR)
		, sha1provider(this, "sha1", 20, 64)
	{
		if (ModuleManager::FindFirstOf(ENCRYPTION) == this)
			throw ModuleException("enc_sha1 is deprecated and can not be used as a primary encryption method");

		sha1provider.Check({
			{ "da39a3ee5e6b4b0d3255bfef95601890afd80709", "" },
			{ "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12", "The quick brown fox jumps over the lazy dog" },
		});
	}

	void OnCheckAuthentication(User *, IdentifyRequest *req) override
	{
		auto *na = NickAlias::Find(req->GetAccount());
		if (!na)
			return;

		NickCore *nc = na->nc;
		auto pos = nc->pass.find(':');
		if (pos == Anope::string::npos)
			return;

		Anope::string hash_method(nc->pass.begin(), nc->pass.begin() + pos);
		if (!hash_method.equals_cs("sha1"))
			return;

		auto enc = "sha1:" + Anope::Hex(sha1provider.Encrypt(req->GetPassword()));
		if (nc->pass.equals_cs(enc))
		{
			// If we are NOT the first encryption module we want to re-encrypt
			// the password with the primary encryption method.
			if (ModuleManager::FindFirstOf(ENCRYPTION) != this)
				Anope::Encrypt(req->GetPassword(), nc->pass);
			req->Success(this, na);
		}
	}
};

MODULE_INIT(ESHA1)
