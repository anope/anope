/*
 *
 * (C) 2014 Daniel Vassdal <shutter@canternet.org>
 * (C) 2014-2016 Anope Team
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
#include <openssl/aes.h>

using namespace SASL;

class DHAES : public Mechanism
{
	void Err(Session* sess, BIGNUM* key = NULL)
	{
		if (key)
			BN_free(key);

		sasl->Fail(sess);
		delete sess;
	}

 public:
	struct DHAESSession : SASL::Session
	{
		DH* dh;
		DHAESSession(Mechanism *m, const Anope::string &u, DH* dh_params) : SASL::Session(m, u)
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

		~DHAESSession()
		{
			if (dh)
				DH_free(dh);
		}
	};

	DH* dh_params;
	const size_t keysize;
	SASL::Session* CreateSession(const Anope::string &uid) anope_override
	{
		return new DHAESSession(this, uid, dh_params);
	}

	DHAES(Module *o) : Mechanism(o, "DH-AES"), keysize(256 / 8)
	{
		if (!(dh_params = DH_new()))
			throw ModuleException("DH_new() failed!");

		if (!DH_generate_parameters_ex(dh_params, keysize * 8, 5, NULL))
		{
			DH_free(dh_params);
			throw ModuleException("Could not generate DH-params");
		}
	}

	~DHAES()
	{
		DH_free(dh_params);
	}

	void ProcessMessage(SASL::Session *session, const SASL::Message &m) anope_override
	{
		DHAESSession *sess = anope_dynamic_static_cast<DHAESSession *>(session);

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

			// Format: [ss]<key>[ss]<iv>[ss]<encrypted>
			// <encrypted> = <username>\0<password>\0

			Anope::string decoded;
			Anope::B64Decode(m.data, decoded);

			// Make sure we have an IV and at least one encrypted block
			if ((decoded.length() < keysize + 2 + (AES_BLOCK_SIZE * 2)) || ((decoded.length() - keysize - 2) % AES_BLOCK_SIZE))
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
			std::vector<unsigned char> secretkey(keysize);
			if (DH_compute_key(&secretkey[0], pubkey, sess->dh) != static_cast<int>(keysize))
				return Err(sess, pubkey);

			// Set decryption key
			AES_KEY AESKey;
			AES_set_decrypt_key(&secretkey[0], keysize * 8, &AESKey);

			// Fetch IV
			pos += keysize;
			std::vector<unsigned char> IV(data + pos, data + pos + AES_BLOCK_SIZE);

			// Find encrypted blocks, and decrypt
			pos += AES_BLOCK_SIZE;
			size_t size = decoded.length() - pos;
			std::vector<char> decrypted(size + 2, 0);
			AES_cbc_encrypt(&data[pos], reinterpret_cast<unsigned char*>(&decrypted[0]), size, &AESKey, &IV[0], AES_DECRYPT);

			std::string username = &decrypted[0];
			std::string password = &decrypted[username.length() + 1];

			if (username.empty() || password.empty() || !IRCD->IsNickValid(username) || password.find_first_of("\r\n") != Anope::string::npos)
				return Err(sess, pubkey);

			SASL::IdentifyRequest* req = new SASL::IdentifyRequest(this->owner, m.source, username, password);
			FOREACH_MOD(OnCheckAuthentication, (NULL, req));
			req->Dispatch();

			BN_free(pubkey);
		}
	}
};


class ModuleSASLDHAES : public Module
{
	DHAES dhaes;

 public:
	ModuleSASLDHAES(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR | EXTRA),
		dhaes(this)
	{
	}
};

MODULE_INIT(ModuleSASLDHAES)
