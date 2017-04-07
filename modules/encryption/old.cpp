/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/encryption.h"

class OldMD5Provider : public Encryption::Provider
{
	ServiceReference<Encryption::Provider> md5;
	
 public:
 	OldMD5Provider(Module *creator) : Encryption::Provider(creator, "oldmd5")
		, md5("md5")
	{
	}

	Encryption::Context *CreateContext(Encryption::IV *iv) override
	{
		if (md5)
			return md5->CreateContext(iv);
		return NULL;
	}

	Encryption::IV GetDefaultIV() override
	{
		if (md5)
			return md5->GetDefaultIV();
		return Encryption::IV(static_cast<const uint32_t *>(NULL), 0);
	}
};

class EOld : public Module
	, public EventHook<Event::Encrypt>
	, public EventHook<Event::CheckAuthentication>
{
	OldMD5Provider oldmd5provider;
	ServiceReference<Encryption::Provider> md5;

	inline static char XTOI(char c) { return c > 9 ? c - 'A' + 10 : c - '0'; }

 public:
	EOld(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, ENCRYPTION | VENDOR)
		, EventHook<Event::Encrypt>(this)
		, EventHook<Event::CheckAuthentication>(this)
		, oldmd5provider(this)
		, md5("md5")
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

		Encryption::Context *context = md5->CreateContext();
		context->Update(reinterpret_cast<const unsigned char *>(src.c_str()), src.length());
		context->Finalize();

		Encryption::Hash hash = context->GetFinalizedHash();

		char digest[32], digest2[16];
		memset(digest, 0, sizeof(digest));
		if (hash.second > sizeof(digest))
			throw CoreException("Hash too large");
		memcpy(digest, hash.first, hash.second);

		for (int i = 0; i < 32; i += 2)
			digest2[i / 2] = XTOI(digest[i]) << 4 | XTOI(digest[i + 1]);

		Anope::string buf = "oldmd5:" + Anope::Hex(digest2, sizeof(digest2));

		logger.Debug2("hashed password from [{0}] to [{1}]", src, buf);

		dest = buf;
		delete context;
		return EVENT_ALLOW;
	}

	void OnCheckAuthentication(User *, NickServ::IdentifyRequest *req) override
	{
		NickServ::Nick *na = NickServ::FindNick(req->GetAccount());
		if (na == NULL)
			return;
		NickServ::Account *nc = na->GetAccount();

		size_t pos = nc->GetPassword().find(':');
		if (pos == Anope::string::npos)
			return;
		Anope::string hash_method(nc->GetPassword().begin(), nc->GetPassword().begin() + pos);
		if (!hash_method.equals_cs("oldmd5"))
			return;

		Anope::string buf;
		this->OnEncrypt(req->GetPassword(), buf);
		if (nc->GetPassword().equals_cs(buf))
		{
			/* if we are NOT the first module in the list,
			 * we want to re-encrypt the pass with the new encryption
			 */
			if (ModuleManager::FindFirstOf(ENCRYPTION) != this)
			{
				Anope::string p;
				Anope::Encrypt(req->GetPassword(), p);
				nc->SetPassword(p);
			}
			req->Success(this);
		}
	}
};

MODULE_INIT(EOld)
