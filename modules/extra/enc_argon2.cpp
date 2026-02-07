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
/// if(WIN32)
///   find_package("argon2" REQUIRED)
///   target_link_libraries(${SO} PRIVATE "argon2::argon2")
/// else()
///   pkg_check_modules("ARGON2" IMPORTED_TARGET REQUIRED "libargon2")
///   target_link_libraries(${SO} PRIVATE PkgConfig::ARGON2)
/// endif()
/// END CMAKE

#include <climits>
#include <random>
#include <regex>

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

	bool ShouldRehash(Encryption::Provider *provider, const Anope::string &password)
	{
		if (provider != defaultprovider)
			return true;

		static std::regex pattern("^\\$argon2(?:i|d|id)(?:\\$v=(\\d+))?\\$m=(\\d+),t=(\\d+),p=(\\d+)\\$([A-Za-z0-9+\\/=]+)\\$([A-Za-z0-9+\\/=]+)$", std::regex::optimize);

		std::smatch matches;
		if (!std::regex_match(password.str(), matches, pattern))
			return true; // Unable to determine, assume yes.

		const auto version = Anope::TryConvert<uint32_t>(matches[1].str());
		if (!version || *version != ARGON2_VERSION_NUMBER)
			return true;

		const auto memory_cost = Anope::TryConvert<uint32_t>(matches[2].str());
		if (!memory_cost || *memory_cost != Argon2Context::memory_cost)
			return true;

		const auto time_cost = Anope::TryConvert<uint32_t>(matches[3].str());
		if (!time_cost || *time_cost != Argon2Context::time_cost)
			return true;

		const auto parallelism = Anope::TryConvert<uint32_t>(matches[4].str());
		if (!parallelism || *parallelism != Argon2Context::parallelism)
			return true;

		const auto salt = Anope::B64Decode(matches[5].str());
		if (salt.length() != Argon2Context::salt_length)
			return true;

		const auto hash = Anope::B64Decode(matches[6].str());
		if (hash.length() != Argon2Context::hash_length)
			return true;

		return false;
	}

public:
	EArgon2(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, ENCRYPTION | VENDOR)
		, argon2dprovider(this, Argon2_d)
		, argon2iprovider(this, Argon2_i)
		, argon2idprovider(this, Argon2_id)
	{
		argon2dprovider.Check({
			{ "$argon2d$v=19$m=10,t=10,p=1$VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw$fNS8JrvE8EqKwQ", "" },
			{ "$argon2d$v=19$m=10,t=10,p=1$VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw$hTvpprMF0TwszQ", "The quick brown fox jumps over the lazy dog" },
		});
		argon2iprovider.Check({
			{ "$argon2i$v=19$m=10,t=10,p=1$VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw$neE6hYxRp4TCJA", "" },
			{ "$argon2i$v=19$m=10,t=10,p=1$VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw$/JAt4FdP1MFD+A", "The quick brown fox jumps over the lazy dog" },
		});
		argon2idprovider.Check({
			{ "$argon2id$v=19$m=10,t=10,p=1$VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw$wuNeHixFDS6Tkg", "" },
			{ "$argon2id$v=19$m=10,t=10,p=1$VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw$Po8RcmxZ7vHmdg", "The quick brown fox jumps over the lazy dog" },
		});
	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &block = Config->GetModule(this);
		this->defaultprovider      = GetAlgorithm(block.Get<const Anope::string>("algorithm", "argon2id"));
		Argon2Context::memory_cost = block.Get<uint32_t>("memory_cost", "131072");
		Argon2Context::time_cost   = block.Get<uint32_t>("time_cost",   "3");
		Argon2Context::parallelism = block.Get<uint32_t>("parallelism", "1");
		Argon2Context::hash_length = block.Get<uint32_t>("hash_length", "32");
		Argon2Context::salt_length = block.Get<uint32_t>("salt_length", "32");
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
		auto *na = NickAlias::Find(req->GetAccount());
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
			if (ModuleManager::FindFirstOf(ENCRYPTION) != this || ShouldRehash(provider, hash_value))
				Anope::Encrypt(req->GetPassword(), nc->pass);
			req->Success(this, na);
		}
	}
};

MODULE_INIT(EArgon2)
