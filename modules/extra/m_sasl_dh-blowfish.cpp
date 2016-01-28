/*
 *
 * (C) 2014 Daniel Vassdal <shutter@canternet.org>
 * (C) 2011-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

/* RequiredLibraries: ssl,crypto */
/* RequiredWindowsLibraries: ssleay32,libeay32 */

#include "module.h"
#include "modules/sasl.h"

#include <openssl/bn.h>
#include <openssl/dh.h>
#include <openssl/blowfish.h>

using namespace SASL;

class DHBS : public Mechanism
{
	void Err(Session* sess, BIGNUM* key = NULL)
	{
		if (key)
			BN_free(key);

		sasl->Fail(sess);
		delete sess;
	}

 public:
	struct DHBSSession : SASL::Session
	{
		DH* dh;
		DHBSSession(Mechanism *m, const Anope::string &u, DH* dh_params) : SASL::Session(m, u)
		{
			if (!(dh = DH_new()))
				return;

			dh->g = BN_dup(dh_params->g);
			dh->p = BN_dup(dh_params->p);

			if (!DH_generate_key(dh))
			{
				DH_free(dh);
				dh = NULL;
			}
		}

		~DHBSSession()
		{
			if (dh)
				DH_free(dh);
		}
	};

	DH* dh_params;
	const size_t keysize;
	SASL::Session* CreateSession(const Anope::string &uid) anope_override
	{
		return new DHBSSession(this, uid, dh_params);
	}

	DHBS(Module *o) : Mechanism(o, "DH-BLOWFISH"), keysize(256 / 8)
	{
		if (!(dh_params = DH_new()))
			throw ModuleException("DH_new() failed!");

		if (!DH_generate_parameters_ex(dh_params, keysize * 8, 5, NULL))
		{
			DH_free(dh_params);
			throw ModuleException("Could not generate DH-params");
		}
	}

	~DHBS()
	{
		DH_free(dh_params);
	}

	void ProcessMessage(SASL::Session *session, const SASL::Message &m) anope_override
	{
		DHBSSession *sess = anope_dynamic_static_cast<DHBSSession *>(session);

		if (!sess->dh)
		{
			sasl->SendMessage(sess, "D", "A");
			delete sess;
			return;
		}

		if (m.type == "S")
		{
			// Format: [ss]<p>[ss]<g>[ss]<pub_key>
			// Where ss is a unsigned short with the size of the key
			const BIGNUM* dhval[] = { sess->dh->p, sess->dh->g, sess->dh->pub_key };

			// Find the size of our buffer - initialized at 6 because of string size data
			size_t size = 6;
			for (size_t i = 0; i < 3; i++)
				size += BN_num_bytes(dhval[i]);

			// Fill in the DH data
			std::vector<unsigned char> buffer(size);
			for (size_t i = 0, pos = 0; i < 3; i++)
			{
				*reinterpret_cast<uint16_t*>(&buffer[pos]) = htons(BN_num_bytes(dhval[i]));
				pos += 2;
				BN_bn2bin(dhval[i], &buffer[pos]);
				pos += BN_num_bytes(dhval[i]);
			}

			Anope::string encoded;
			Anope::B64Encode(Anope::string(buffer.begin(), buffer.end()), encoded);
			sasl->SendMessage(sess, "C", encoded);
		}
		else if (m.type == "C")
		{
			// Make sure we have some data - actual size check is done later
			if (m.data.length() < 10)
				return Err(sess);

			// Format: [ss]<key><username><\0><encrypted>

			Anope::string decoded;
			Anope::B64Decode(m.data, decoded);

			// As we rely on the client giving us a null terminator at the right place,
			// let's add one extra in case the client tries to crash us
			const size_t decodedlen = decoded.length();
			decoded.push_back('\0');

			// Make sure we have enough data for at least the key, a one letter username, and a block of data
			if (decodedlen < keysize + 2 + 2 + 8)
				return Err(sess);

			const unsigned char* data = reinterpret_cast<const unsigned char*>(decoded.data());

			// Control the size of the key
			if (ntohs(*reinterpret_cast<const uint16_t*>(&data[0])) != keysize)
				return Err(sess);

			// Convert pubkey from binary
			size_t pos = 2;
			BIGNUM* pubkey = BN_bin2bn(&data[pos], keysize, NULL);
			if (!pubkey)
				return Err(sess);

			// Find shared key
			std::vector<unsigned char> secretkey(DH_size(sess->dh) + 1, 0);
			if (DH_compute_key(&secretkey[0], pubkey, sess->dh) != static_cast<int>(keysize))
				return Err(sess, pubkey);

			// Set decryption key
			BF_KEY BFKey;
			BF_set_key(&BFKey, keysize, &secretkey[0]);

			pos += keysize;
			const Anope::string username = reinterpret_cast<const char*>(&data[pos]);
			// Check that the username is valid, and that we have at least one block of data
			// 2 + 1 + 8 = uint16_t size for keylen, \0 for username, 8 for one block of data
			if (username.empty() || username.length() + keysize + 2 + 1 + 8 > decodedlen || !IRCD->IsNickValid(username))
				return Err(sess, pubkey);

			pos += username.length() + 1;
			size_t size = decodedlen - pos;

			// Blowfish data blocks are 64 bits wide - valid format?
			if (size % 8)
				return Err(sess, pubkey);

			std::vector<char> decrypted(size + 1, 0);
			for (size_t i = 0; i < size; i += 8)
				BF_ecb_encrypt(&data[pos + i], reinterpret_cast<unsigned char*>(&decrypted[i]), &BFKey, BF_DECRYPT);

			std::string password = &decrypted[0];
			if (password.empty() || password.find_first_of("\r\n") != Anope::string::npos)
				return Err(sess, pubkey);

			SASL::IdentifyRequest* req = new SASL::IdentifyRequest(this->owner, m.source, username, password);
			FOREACH_MOD(OnCheckAuthentication, (NULL, req));
			req->Dispatch();

			BN_free(pubkey);
		}
	}
};


class ModuleSASLDHBS : public Module
{
	DHBS dhbs;

 public:
	ModuleSASLDHBS(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR | EXTRA),
		dhbs(this)
	{
	}
};

MODULE_INIT(ModuleSASLDHBS)
