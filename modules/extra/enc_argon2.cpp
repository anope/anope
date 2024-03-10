/* Module for providing Argon2 hashing
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 *
 */

/* RequiredLibraries: argon2 */
/* RequiredWindowsLibraries: argon2 */

#include <climits>
#include <random>

#include <argon2.h>

#include "module.h"
#include "modules/encryption.h"

class Argon2Context final
	: public Encryption::Context
{
private:
	Anope::string buffer;
	argon2_type type;

	Anope::string GenerateSalt()
	{
		static std::random_device device;
		static std::mt19937 engine(device());
		static std::uniform_int_distribution<int> dist(CHAR_MIN, CHAR_MAX);
		Anope::string saltbuf(this->salt_length, ' ');
		for (size_t i = 0; i < this->salt_length; ++i)
			saltbuf[i] = static_cast<char>(dist(engine));
		return saltbuf;
	}

public:
	static uint32_t memory_cost;
	static uint32_t time_cost;
	static uint32_t parallelism;
	static uint32_t hash_length;
	static uint32_t salt_length;

	Argon2Context(argon2_type at)
		: type(at)
	{
	}

	void Update(const unsigned char *data, size_t len) override
	{
		buffer.append(reinterpret_cast<const char *>(data), len);
	}

	Anope::string Finalize() override
	{
		auto salt = GenerateSalt();

		// Calculate the size of and allocate the output buffer.
		auto length = argon2_encodedlen(this->time_cost, this->memory_cost, this->parallelism,
			this->salt_length, this->hash_length, this->type);

		std::vector<char> digest(length);
		auto result = argon2_hash(this->time_cost, this->memory_cost, this->parallelism,
			buffer.c_str(), buffer.length(), salt.c_str(), salt.length(), nullptr,
			this->hash_length, digest.data(), digest.size(), this->type,
			ARGON2_VERSION_NUMBER);

		if (result == ARGON2_OK)
			return Anope::string(digest.data(), digest.size());

		Log(LOG_DEBUG_2) << "Argon2 error: " << argon2_error_message(result);
		return {};
	}
};

uint32_t Argon2Context::memory_cost;
uint32_t Argon2Context::time_cost;
uint32_t Argon2Context::parallelism;
uint32_t Argon2Context::hash_length;
uint32_t Argon2Context::salt_length;

class Argon2Provider final
	: public Encryption::Provider
{
private:
	argon2_type type;

public:
	Argon2Provider(Module *creator, argon2_type at)
		: Encryption::Provider(creator, argon2_type2string(at, 0), 0, 0)
		, type(at)
	{
	}

	bool Compare(const Anope::string &hash, const Anope::string &plain) override
	{
		return argon2_verify(hash.c_str(), plain.c_str(), plain.length(), this->type) == ARGON2_OK;
	}

	std::unique_ptr<Encryption::Context> CreateContext() override
	{
		return std::make_unique<Argon2Context>(this->type);
	}

	Anope::string ToPrintable(const Anope::string &hash) override
	{
		// We have no way to make this printable without the creating context
		// so we always return the printed form.
		return hash;
	}
};


class EArgon2 final
	: public Module
{
private:
	Encryption::Provider *defaultprovider = nullptr;
	Argon2Provider argon2dprovider;
	Argon2Provider argon2iprovider;
	Argon2Provider argon2idprovider;

	Encryption::Provider *GetAlgorithm(const Anope::string &algorithm)
	{
		if (algorithm == "argon2d")
			return &argon2dprovider;
		if (algorithm == "argon2i")
			return &argon2iprovider;
		if (algorithm == "argon2id")
			return &argon2idprovider;
		return nullptr;
	}

public:
	EArgon2(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, ENCRYPTION | VENDOR)
		, argon2dprovider(this, Argon2_d)
		, argon2iprovider(this, Argon2_i)
		, argon2idprovider(this, Argon2_id)

	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		const auto *block = Config->GetModule(this);
		this->defaultprovider      = GetAlgorithm(block->Get<const Anope::string>("algorithm", "argon2id"));
		Argon2Context::memory_cost = block->Get<uint32_t>("memory_cost", "131072");
		Argon2Context::time_cost   = block->Get<uint32_t>("time_cost",   "3");
		Argon2Context::parallelism = block->Get<uint32_t>("parallelism", "1");
		Argon2Context::hash_length = block->Get<uint32_t>("hash_length", "32");
		Argon2Context::salt_length = block->Get<uint32_t>("salt_length", "32");
	}

	EventReturn OnEncrypt(const Anope::string &src, Anope::string &dest) override
	{
		if (!defaultprovider)
			return EVENT_CONTINUE;

		auto hash = defaultprovider->Encrypt(src);
		auto enc = defaultprovider->name + ":" + hash;
		Log(LOG_DEBUG_2) << "(enc_argon2) hashed password from [" << src << "] to [" << enc << "]";
		dest = enc;
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
		auto provider = GetAlgorithm(hash_method);
		if (!provider)
			return; // Not a hash for this module.

		Anope::string hash_value(nc->pass.begin() + pos + 1, nc->pass.end());
		if (provider->Compare(hash_value, req->GetPassword()))
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

MODULE_INIT(EArgon2)
