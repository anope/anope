/* Module for providing SHA-2 hashing
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 *
 */

#include <climits>
#include <random>

#include "sha2/sha2.c"

#include "module.h"
#include "modules/encryption.h"

template <typename SHAContext,
	void (* SHAInit)(SHAContext *),
	void (* SHAUpdate)(SHAContext *, const unsigned char *, unsigned int),
	void (* SHAFinal)(SHAContext *, unsigned char *)>
class SHA2Context final
	: public Encryption::Context
{
private:
	SHAContext context;
	const size_t digest_size;

public:
	SHA2Context(size_t ds)
		: digest_size(ds)
	{
		SHAInit(&context);
	}

	void Update(const unsigned char *data, size_t len) override
	{
		SHAUpdate(&context, data, len);
	}

	Anope::string Finalize() override
	{
		std::vector<unsigned char> digest(digest_size);
		SHAFinal(&context, digest.data());
		return Anope::string(reinterpret_cast<const char *>(digest.data()), digest.size());
	}
};

template <typename SHAContext,
	void (* SHAInit)(SHAContext *),
	void (* SHAUpdate)(SHAContext *, const unsigned char *, unsigned int),
	void (* SHAFinal)(SHAContext *, unsigned char *)>
class SHA2Provider final
	: public Encryption::Provider
{
public:
	SHA2Provider(Module *creator, const Anope::string &algorithm, size_t bs, size_t ds)
		: Encryption::Provider(creator, algorithm, bs, ds)
	{
	}

	std::unique_ptr<Encryption::Context> CreateContext() override
	{
		return std::make_unique<SHA2Context<SHAContext, SHAInit, SHAUpdate, SHAFinal>>(this->digest_size);
	}
};

class ESHA2 final
	: public Module
{
private:
	Encryption::Provider *defaultprovider = nullptr;
	SHA2Provider<sha224_ctx, sha224_init, sha224_update, sha224_final> sha224provider;
	SHA2Provider<sha256_ctx, sha256_init, sha256_update, sha256_final> sha256provider;
	SHA2Provider<sha384_ctx, sha384_init, sha384_update, sha384_final> sha384provider;
	SHA2Provider<sha512_ctx, sha512_init, sha512_update, sha512_final> sha512provider;

	Anope::string GenerateKey(size_t keylen)
	{
		static std::random_device device;
		static std::mt19937 engine(device());
		static std::uniform_int_distribution<int> dist(CHAR_MIN, CHAR_MAX);
		Anope::string keybuf(keylen, ' ');
		for (size_t i = 0; i < keylen; ++i)
			keybuf[i] = static_cast<char>(dist(engine));
		return keybuf;
	}

	Encryption::Provider *GetAlgorithm(const Anope::string &algorithm)
	{
		if (algorithm == "sha224")
			return &sha224provider;
		if (algorithm == "sha256")
			return &sha256provider;
		if (algorithm == "sha384")
			return &sha384provider;
		if (algorithm == "sha512")
			return &sha512provider;
		return nullptr;
	}

public:
	ESHA2(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, ENCRYPTION | VENDOR)
		, sha224provider(this, "sha224", SHA224_BLOCK_SIZE, SHA224_DIGEST_SIZE)
		, sha256provider(this, "sha256", SHA256_BLOCK_SIZE, SHA256_DIGEST_SIZE)
		, sha384provider(this, "sha384", SHA384_BLOCK_SIZE, SHA384_DIGEST_SIZE)
		, sha512provider(this, "sha512", SHA512_BLOCK_SIZE, SHA512_DIGEST_SIZE)
	{
		sha224provider.Check({
			{ "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f", "" },
			{ "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525", "The quick brown fox jumps over the lazy dog" },
		});
		sha256provider.Check({
			{ "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", "" },
			{ "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592", "The quick brown fox jumps over the lazy dog" },
		});
		sha384provider.Check({
			{ "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b", "" },
			{ "ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1", "The quick brown fox jumps over the lazy dog" },
		});
		sha512provider.Check({
			{ "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e", "" },
			{ "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6", "The quick brown fox jumps over the lazy dog" },
		});
	}

	void OnReload(Configuration::Conf *conf) override
	{
		this->defaultprovider = GetAlgorithm(Config->GetModule(this)->Get<const Anope::string>("algorithm", "sha256"));
	}

	EventReturn OnEncrypt(const Anope::string &src, Anope::string &dest) override
	{
		if (!defaultprovider)
			return EVENT_CONTINUE;

		auto key = GenerateKey(defaultprovider->digest_size);
		auto hmac = defaultprovider->HMAC(key, src);
		auto enc = "hmac-" + defaultprovider->name + ":" + Anope::Hex(hmac) + ":" + Anope::Hex(key);
		Log(LOG_DEBUG_2) << "(enc_sha2) hashed password from [" << src << "] to [" << enc << "]";
		dest = enc;
		return EVENT_ALLOW;
	}

	void OnCheckAuthentication(User *, IdentifyRequest *req) override
	{
		const auto *na = NickAlias::Find(req->GetAccount());
		if (!na)
			return;

		NickCore *nc = na->nc;
		auto apos = nc->pass.find(':');
		if (apos == Anope::string::npos)
			return;

		Anope::string hash_method(nc->pass.begin(), nc->pass.begin() + apos);
		if (hash_method.compare(0, 5, "hmac-", 5))
			return; // Not a HMAC hash.

		auto provider = GetAlgorithm(hash_method.substr(5));
		if (!provider)
			return; // Not a hash for this module.

		auto bpos = nc->pass.find(':', apos + 1);
		if (bpos == Anope::string::npos)
			return; // No HMAC key.

		Anope::string pass_hex(nc->pass.begin() + apos + 1, nc->pass.begin() + bpos);
		Anope::string key_hex(nc->pass.begin() + bpos + 1, nc->pass.end());
		Anope::string key;
		Anope::Unhex(key_hex, key);

		auto enc = Anope::Hex(provider->HMAC(key, req->GetPassword()));
		if (pass_hex.equals_cs(enc))
		{
			// If we are NOT the first encryption module or the algorithm is
			// different we want to re-encrypt the password with the primary
			// encryption method.
			if (ModuleManager::FindFirstOf(ENCRYPTION) != this || provider != defaultprovider)
				Anope::Encrypt(req->GetPassword(), nc->pass);
			req->Success(this);
		}
	}
};

MODULE_INIT(ESHA2)
