/* This module generates and compares password hashes using SHA256 algorithms.
 * To help reduce the risk of dictionary attacks, the code appends random bytes
 * (so-called "salt") to the original plain text before generating hashes and
 * stores this salt appended to the result. To verify another plain text value
 * against the given hash, this module will retrieve the salt value from the
 * password string and use it when computing a new hash of the plain text.
 *
 * If an intruder gets access to your system or uses a brute force attack,
 * salt will not provide much value.
 * IMPORTANT: DATA HASHES CANNOT BE "DECRYPTED" BACK TO PLAIN TEXT.
 *
 * Modified for Anope.
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Taken from InspIRCd ( www.inspircd.org )
 *  see http://wiki.inspircd.org/Credits
 *
 * This program is free but copyrighted software; see
 * the file COPYING for details.
 */

/* FIPS 180-2 SHA-224/256/384/512 implementation
 * Last update: 05/23/2005
 * Issue date:  04/30/2005
 *
 * Copyright (C) 2005 Olivier Gay <olivier.gay@a3.epfl.ch>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "module.h"

static const unsigned SHA256_DIGEST_SIZE = 256 / 8;
static const unsigned SHA256_BLOCK_SIZE = 512 / 8;

/** An sha256 context
 */
class SHA256Context
{
 public:
	unsigned tot_len;
	unsigned len;
	unsigned char block[2 * SHA256_BLOCK_SIZE];
	uint32 h[8];
};

inline static uint32 SHFR(uint32 x, uint32 n) { return x >> n; }
inline static uint32 ROTR(uint32 x, uint32 n) { return (x >> n) | (x << ((sizeof(x) << 3) - n)); }
inline static uint32 ROTL(uint32 x, uint32 n) { return (x << n) | (x >> ((sizeof(x) << 3) - n)); }
inline static uint32 CH(uint32 x, uint32 y, uint32 z) { return (x & y) ^ (~x & z); }
inline static uint32 MAJ(uint32 x, uint32 y, uint32 z) { return (x & y) ^ (x & z) ^ (y & z); }

inline static uint32 SHA256_F1(uint32 x) { return ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22); }
inline static uint32 SHA256_F2(uint32 x) { return ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25); }
inline static uint32 SHA256_F3(uint32 x) { return ROTR(x, 7) ^ ROTR(x, 18) ^ SHFR(x, 3); }
inline static uint32 SHA256_F4(uint32 x) { return ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10); }

inline static void UNPACK32(unsigned x, unsigned char *str)
{
	str[3] = static_cast<uint8>(x);
	str[2] = static_cast<uint8>(x >> 8);
	str[1] = static_cast<uint8>(x >> 16);
	str[0] = static_cast<uint8>(x >> 24);
}

inline static void PACK32(unsigned char *str, uint32 &x)
{
	x = static_cast<uint32>(str[3]) | static_cast<uint32>(str[2]) << 8 | static_cast<uint32>(str[1]) << 16 | static_cast<uint32>(str[0]) << 24;
}

/* Macros used for loops unrolling */

inline static void SHA256_SCR(uint32 w[64], int i)
{
	w[i] = SHA256_F4(w[i - 2]) + w[i - 7] + SHA256_F3(w[i - 15]) + w[i - 16];
}

uint32 sha256_k[64] =
{
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

class ESHA256 : public Module
{
	unsigned iv[8];
	bool use_iv;

	/* initializes the IV with a new random value */
	void NewRandomIV()
	{
		for (int i = 0; i < 8; ++i)
			iv[i] = getrandom32();
	}

	/* returns the IV as base64-encrypted string */
	std::string GetIVString()
	{
		unsigned char buf[33];
		char buf2[512];
		for (int i = 0; i < 8; ++i)
			UNPACK32(iv[i], &buf[i << 2]);
		b64_encode(reinterpret_cast<char *>(buf), 32, buf2, 512);
		return buf2;
	}

	/* splits the appended IV from the password string so it can be used for the next encryption */
	/* password format:  <hashmethod>:<password_b64>:<iv_b64> */
	void GetIVFromPass(std::string &password)
	{
		size_t pos = password.find(":");
		std::string buf(password, password.find(":", pos + 1) + 1, password.size());
		unsigned char buf2[33];
		b64_decode(buf.c_str(), reinterpret_cast<char *>(buf2), 33);
		for (int i = 0 ; i < 8; ++i)
			PACK32(&buf2[i<<2], iv[i]);
	}

	void SHA256Init(SHA256Context *ctx)
	{
		for (int i = 0; i < 8; ++i)
			ctx->h[i] = iv[i];
		ctx->len = 0;
		ctx->tot_len = 0;
	}

	void SHA256Transform(SHA256Context *ctx, unsigned char *message, unsigned block_nb)
	{
		uint32 w[64], wv[8];
		unsigned char *sub_block;
		for (unsigned i = 1; i <= block_nb; ++i)
		{
			int j;
			sub_block = message + ((i - 1) << 6);

			for (j = 0; j < 16; ++j)
				PACK32(&sub_block[j << 2], w[j]);
			for (j = 16; j < 64; ++j)
				SHA256_SCR(w, j);
			for (j = 0; j < 8; ++j)
				wv[j] = ctx->h[j];
			for (j = 0; j < 64; ++j)
			{
				uint32 t1 = wv[7] + SHA256_F2(wv[4]) + CH(wv[4], wv[5], wv[6]) + sha256_k[j] + w[j];
				uint32 t2 = SHA256_F1(wv[0]) + MAJ(wv[0], wv[1], wv[2]);
				wv[7] = wv[6];
				wv[6] = wv[5];
				wv[5] = wv[4];
				wv[4] = wv[3] + t1;
				wv[3] = wv[2];
				wv[2] = wv[1];
				wv[1] = wv[0];
				wv[0] = t1 + t2;
			}
			for (j = 0; j < 8; ++j)
				ctx->h[j] += wv[j];
		}
	}

	void SHA256Update(SHA256Context *ctx, const unsigned char *message, unsigned len)
	{
		/*
		 * XXX here be dragons!
		 * After many hours of pouring over this, I think I've found the problem.
		 * When Special created our module from the reference one, he used:
		 *
		 *     unsigned rem_len = SHA256_BLOCK_SIZE - ctx->len;
		 *
		 * instead of the reference's version of:
		 *
		 *     unsigned tmp_len = SHA256_BLOCK_SIZE - ctx->len;
		 *     unsigned rem_len = len < tmp_len ? len : tmp_len;
		 *
		 * I've changed back to the reference version of this code, and it seems to work with no errors.
		 * So I'm inclined to believe this was the problem..
		 *             -- w00t (January 06, 2008)
		 */
		unsigned tmp_len = SHA256_BLOCK_SIZE - ctx->len, rem_len = len < tmp_len ? len : tmp_len;

		memcpy(&ctx->block[ctx->len], message, rem_len);
		if (ctx->len + len < SHA256_BLOCK_SIZE)
		{
			ctx->len += len;
			return;
		}
		unsigned new_len = len - rem_len, block_nb = new_len / SHA256_BLOCK_SIZE;
		unsigned char *shifted_message = new unsigned char[len - rem_len];
		memcpy(shifted_message, message + rem_len, len - rem_len);
		SHA256Transform(ctx, ctx->block, 1);
		SHA256Transform(ctx, shifted_message, block_nb);
		rem_len = new_len % SHA256_BLOCK_SIZE;
		memcpy(ctx->block, &shifted_message[block_nb << 6], rem_len);
		delete [] shifted_message;
		ctx->len = rem_len;
		ctx->tot_len += (block_nb + 1) << 6;
	}

	void SHA256Final(SHA256Context *ctx, unsigned char *digest)
	{
		unsigned block_nb = 1 + ((SHA256_BLOCK_SIZE - 9) < (ctx->len % SHA256_BLOCK_SIZE));
		unsigned len_b = (ctx->tot_len + ctx->len) << 3;
		unsigned pm_len = block_nb << 6;
		memset(ctx->block + ctx->len, 0, pm_len - ctx->len);
		ctx->block[ctx->len] = 0x80;
		UNPACK32(len_b, ctx->block + pm_len - 4);
		SHA256Transform(ctx, ctx->block, block_nb);
		for (int i = 0 ; i < 8; ++i)
			UNPACK32(ctx->h[i], &digest[i << 2]);
	}

/**********   ANOPE ******/
 public:
	ESHA256(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(ENCRYPTION);

		ModuleManager::Attach(I_OnEncrypt, this);
		ModuleManager::Attach(I_OnEncryptInPlace, this);
		ModuleManager::Attach(I_OnDecrypt, this);
		ModuleManager::Attach(I_OnCheckPassword, this);

		use_iv = false;
	}

	EventReturn OnEncrypt(const std::string &src, std::string &dest)
	{
		char digest[SHA256_DIGEST_SIZE];
		char cpass[1000];
		SHA256Context ctx;
		std::stringstream buf;

		if (!use_iv)
			NewRandomIV();
		else
			use_iv = false;

		SHA256Init(&ctx);
		SHA256Update(&ctx, reinterpret_cast<const unsigned char *>(src.c_str()), src.size());
		SHA256Final(&ctx, reinterpret_cast<unsigned char *>(digest));

		b64_encode(digest, SHA256_DIGEST_SIZE, cpass, 1000);
		buf << "sha256:" << cpass << ":" << GetIVString();
		Alog(LOG_DEBUG_2) << "(enc_sha256) hashed password from [" << src << "] to [" << buf.str() << " ]";
		dest = buf.str();
		return EVENT_ALLOW;
	}

	EventReturn OnEncryptInPlace(std::string &buf)
	{
		return this->OnEncrypt(buf, buf);
	}

	EventReturn OnDecrypt(const std::string &hashm, std::string &src, std::string &dest)
	{
		if (hashm != "sha256")
			return EVENT_CONTINUE;
		return EVENT_STOP;
	}

	EventReturn OnCheckPassword(const std::string &hashm, std::string &plaintext, std::string &password)
	{
		if (hashm != "sha256")
			return EVENT_CONTINUE;
		std::string buf;

		GetIVFromPass(password);
		use_iv = true;
		this->OnEncrypt(plaintext, buf);

		if (password == buf)
		{
			/* if we are NOT the first module in the list,
			 * we want to re-encrypt the pass with the new encryption
			 */
			if (Config.EncModuleList.front() != this->name)
				enc_encrypt(plaintext, password );
			return EVENT_ALLOW;
		}
		return EVENT_STOP;
	}
};

MODULE_INIT(ESHA256)
