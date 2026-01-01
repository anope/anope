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

#include "sha2/sha2.c"

#include "module.h"

class ESHA256 final
	: public Module
{
private:
	unsigned iv[8];

	/* returns the IV as base64-encrypted string */
	Anope::string GetIVString()
	{
		char buf[33];
		for (int i = 0; i < 8; ++i)
			UNPACK32(iv[i], reinterpret_cast<unsigned char *>(&buf[i << 2]));
		buf[32] = '\0';
		return Anope::Hex(buf, 32);
	}

	/* splits the appended IV from the password string so it can be used for the next encryption */
	/* password format:  <hashmethod>:<password_b64>:<iv_b64> */
	void GetIVFromPass(const Anope::string &password)
	{
		size_t pos = password.find(':');
		Anope::string buf = password.substr(password.find(':', pos + 1) + 1, password.length());
		char buf2[33];
		Anope::Unhex(buf, buf2, sizeof(buf2));
		for (int i = 0 ; i < 8; ++i)
			PACK32(reinterpret_cast<unsigned char *>(&buf2[i << 2]), &iv[i]);
	}

	Anope::string EncryptInternal(const Anope::string &src)
	{
		sha256_ctx ctx;
		sha256_init(&ctx);
		for (size_t i = 0; i < 8; ++i)
			ctx.h[i] = iv[i];
		sha256_update(&ctx, reinterpret_cast<const unsigned char *>(src.data()), src.length());
		unsigned char digest[SHA256_DIGEST_SIZE];
		sha256_final(&ctx, digest);
		Anope::string hash(reinterpret_cast<const char *>(&digest), sizeof(digest));

		return "sha256:" + Anope::Hex(hash) + ":" + GetIVString();
	}

public:
	ESHA256(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, DEPRECATED | ENCRYPTION | VENDOR)
	{
		if (ModuleManager::FindFirstOf(ENCRYPTION) == this)
			throw ModuleException("enc_sha256 is deprecated and can not be used as a primary encryption method");
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
		if (!hash_method.equals_cs("sha256"))
			return;

		GetIVFromPass(nc->pass);
		auto enc = EncryptInternal(req->GetPassword());
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

MODULE_INIT(ESHA256)
