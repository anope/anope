/* Module for providing bcrypt hashing
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 *
 */


#include <climits>
#include <random>

#include "bcrypt/crypt_blowfish.c"

#include "module.h"
#include "modules/encryption.h"

class BCryptContext final
	: public Encryption::Context
{
private:
	Anope::string buffer;

	Anope::string GenerateSalt()
	{
		static std::random_device device;
		static std::mt19937 engine(device());
		static std::uniform_int_distribution<int> dist(CHAR_MIN, CHAR_MAX);
		char entropy[16];
		for (size_t i = 0; i < sizeof(entropy); ++i)
			entropy[i] = static_cast<char>(dist(engine));

		char salt[32];
		if (!_crypt_gensalt_blowfish_rn("$2a$", rounds, entropy, sizeof(entropy), salt, sizeof(salt)))
		{
			Log(LOG_DEBUG) << "Unable to generate a salt for Bcrypt: " << strerror(errno);
			return {};
		}
		return salt;
	}

public:
	static unsigned long rounds;

	static Anope::string Hash(const Anope::string &data, const Anope::string &salt)
	{
		char hash[64];
		if (!_crypt_blowfish_rn(data.c_str(), salt.c_str(), hash, sizeof(hash)))
		{
			Log(LOG_DEBUG) << "Unable to generate a hash for Bcrypt: " << strerror(errno);
			return {};
		}
		return hash;
	}

	void Update(const unsigned char *data, size_t len) override
	{
		buffer.append(reinterpret_cast<const char *>(data), len);
	}

	Anope::string Finalize() override
	{
		auto salt = GenerateSalt();
		if (salt.empty())
			return {};
		return Hash(this->buffer, salt);
	}
};

unsigned long BCryptContext::rounds = 10;

class BCryptProvider final
	: public Encryption::Provider
{
public:
	BCryptProvider(Module *creator)
		: Encryption::Provider(creator, "bcrypt", 0, 0)
	{
	}

	bool Compare(const Anope::string &hash, const Anope::string &plain) override
	{
		auto newhash = BCryptContext::Hash(plain, hash);
		return !newhash.empty() && hash.equals_cs(newhash);
	}

	std::unique_ptr<Encryption::Context> CreateContext() override
	{
		return std::make_unique<BCryptContext>();
	}

	Anope::string ToPrintable(const Anope::string &hash) override
	{
		// The crypt_blowfish library does not expose a raw form.
		return hash;
	}
};


class EBCrypt final
	: public Module
{
private:
	BCryptProvider bcryptprovider;
	static const size_t BCRYPT_MAX_LEN = 72;

public:
	EBCrypt(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, ENCRYPTION | VENDOR)
		, bcryptprovider(this)
	{
		bcryptprovider.Check({
			{ "$2a$10$c9lUAuJmTYXEfNuLOiyIp.lZTMM.Rw5qsSAyZhvGT9EC3JevkUuOu", "" },
			{ "$2a$10$YV4jDSGs0ZtQbpL6IHtNO.lt5Q.uzghIohCcnERQVBGyw7QJMfyhe", "The quick brown fox jumps over the lazy dog" },
		});
	}

	EventReturn OnEncrypt(const Anope::string &src, Anope::string &dest) override
	{
		if (src.length() > BCRYPT_MAX_LEN)
			return EVENT_CONTINUE;

		dest = "bcrypt:" + bcryptprovider.Encrypt(src);
		Log(LOG_DEBUG_2) << "(enc_bcrypt) hashed password from [" << src << "] to [" << dest << "]";
		return EVENT_ALLOW;
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
		if (!hash_method.equals_cs("bcrypt"))
			return;

		Anope::string hash_value(nc->pass.begin() + pos + 1, nc->pass.end());
		if (bcryptprovider.Compare(hash_value, req->GetPassword()))
		{
			unsigned long rounds = 0;

			// Try to extract the rounds count to check if we need to
			// re-encrypt the password.
			pos = hash_value.find('$', 4);
			if (pos != Anope::string::npos)
				rounds = Anope::Convert<unsigned long>(hash_value.substr(4, pos - 4), 0);

			if (!rounds)
				Log(LOG_DEBUG) << "Unable to determine the rounds of a bcrypt hash: " << hash_value;

			// If we are NOT the first encryption module or the Bcrypt rounds
			// are different we want to re-encrypt the password with the primary
			// encryption method.
			if (ModuleManager::FindFirstOf(ENCRYPTION) != this || (rounds && rounds != BCryptContext::rounds))
				Anope::Encrypt(req->GetPassword(), nc->pass);
			req->Success(this);
		}
	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto maxpasslen = conf.GetModule("nickserv").Get<unsigned>("maxpasslen", "50");
		if (maxpasslen > BCRYPT_MAX_LEN && ModuleManager::FindFirstOf(ENCRYPTION) == this)
			Log(this) << "Warning: {nickserv}:maxpasslen is set to " << maxpasslen << " which is longer than the bcrypt maximum length of " << BCRYPT_MAX_LEN;

		auto &block = conf.GetModule(this);

		auto rounds = block.Get<unsigned long>("rounds", "10");
		if (rounds < 10 || rounds > 32)
		{
			Log(this) << "Bcrypt rounds MUST be between 10 and 32 inclusive; using 10 instead of " << rounds << '.';
			BCryptContext::rounds = 10;
			return;
		}

		if (rounds > 14)
			Log(this) << "Bcrypt rounds higher than 14 are very CPU intensive; are you sure you want to use " << rounds << '?';
		BCryptContext::rounds = rounds;
	}
};

MODULE_INIT(EBCrypt)
