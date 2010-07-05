/* Module for encryption using MD5.
 *
 * Modified for Anope.
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Taken from IRC Services and is copyright (c) 1996-2002 Andrew Church.
 *	 E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 */

#include "module.h"

/*************************************************************************/

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

/* MD5 context. */
struct MD5_CTX
{
	unsigned state[4]; /* state (ABCD) */
	unsigned count[2]; /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64]; /* input buffer */
};

/* MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
 */

/* Constants for MD5Transform routine.
 */
enum
{
	S11 = 7,
	S12 = 12,
	S13 = 17,
	S14 = 22,
	S21 = 5,
	S22 = 9,
	S23 = 14,
	S24 = 20,
	S31 = 4,
	S32 = 11,
	S33 = 16,
	S34 = 23,
	S41 = 6,
	S42 = 10,
	S43 = 15,
	S44 = 21
};

void MD5Transform(unsigned [4], const unsigned char [64]);
void Encode(unsigned char *, unsigned *, unsigned);
void Decode(unsigned *, const unsigned char *, unsigned);

static unsigned char PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
inline static unsigned F(unsigned x, unsigned y, unsigned z) { return (x & y) | (~x & z); }
inline static unsigned G(unsigned x, unsigned y, unsigned z) { return (x & z) | (y & ~z); }
inline static unsigned H(unsigned x, unsigned y, unsigned z) { return x ^ y ^ z; }
inline static unsigned I(unsigned x, unsigned y, unsigned z) { return y ^ (x | ~z); }

/* ROTATE_LEFT rotates x left n bits.
 */
inline static unsigned ROTATE_LEFT(unsigned x, unsigned n) { return (x << n) | (x >> (32 - n)); }

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
 * Rotation is separate from addition to prevent recomputation.
 */
inline static void FF(unsigned &a, unsigned b, unsigned c, unsigned d, unsigned x, unsigned s, unsigned ac)
{
	a += F(b, c, d) + x + ac;
	a = ROTATE_LEFT(a, s);
	a += b;
}
inline static void GG(unsigned &a, unsigned b, unsigned c, unsigned d, unsigned x, unsigned s, unsigned ac)
{
	a += G(b, c, d) + x + ac;
	a = ROTATE_LEFT(a, s);
	a += b;
}
inline static void HH(unsigned &a, unsigned b, unsigned c, unsigned d, unsigned x, unsigned s, unsigned ac)
{
	a += H(b, c, d) + x + ac;
	a = ROTATE_LEFT(a, s);
	a += b;
}
inline static void II(unsigned &a, unsigned b, unsigned c, unsigned d, unsigned x, unsigned s, unsigned ac)
{
	a += I(b, c, d) + x + ac;
	a = ROTATE_LEFT(a, s);
	a += b;
}

/* MD5 initialization. Begins an MD5 operation, writing a new context.
 */
void MD5Init(MD5_CTX *context)
{
	context->count[0] = context->count[1] = 0;
	/* Load magic initialization constants. */
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest
 * operation, processing another message block, and updating the
 * context.
 */
void MD5Update(MD5_CTX *context, const unsigned char *input, unsigned inputLen)
{
	unsigned i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (context->count[0] >> 3) & 0x3F;

	/* Update number of bits */
	if ((context->count[0] += inputLen << 3) < (inputLen << 3))
		++context->count[1];
	context->count[1] += inputLen >> 29;

	partLen = 64 - index;

	/* Transform as many times as possible. */
	if (inputLen >= partLen)
	{
		memcpy(&context->buffer[index], input, partLen);
		MD5Transform(context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
			MD5Transform(context->state, &input[i]);

		index = 0;
	}
	else
		i = 0;

	/* Buffer remaining input */
	memcpy(&context->buffer[index], &input[i], inputLen - i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
 * the message digest and zeroizing the context.
 */
void MD5Final(unsigned char digest[16], MD5_CTX *context)
{
	unsigned char bits[8];
	unsigned index, padLen;

	/* Save number of bits */
	Encode(bits, context->count, 8);

	/* Pad out to 56 mod 64. */
	index = (context->count[0] >> 3) & 0x3f;
	padLen = index < 56 ? 56 - index : 120 - index;
	MD5Update(context, PADDING, padLen);

	/* Append length (before padding) */
	MD5Update(context, bits, 8);
	/* Store state in digest */
	Encode(digest, context->state, 16);

	/* Zeroize sensitive information. */
	memset(context, 0, sizeof(*context));
}

/* MD5 basic transformation. Transforms state based on block.
 */
void MD5Transform(unsigned state[4], const unsigned char block[64])
{
	unsigned a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	Decode(x, block, 64);

	/* Round 1 */
	FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
	FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
	HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
	II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	/* Zeroize sensitive information. */
	memset(x, 0, sizeof(x));
}

/* Encodes input (unsigned) into output (unsigned char). Assumes len is
 * a multiple of 4.
 */
void Encode(unsigned char *output, unsigned *input, unsigned len)
{
	unsigned i, j;

	for (i = 0, j = 0; j < len; ++i, j += 4)
	{
		output[j] = static_cast<unsigned char>(input[i] & 0xff);
		output[j + 1] = static_cast<unsigned char>((input[i] >> 8) & 0xff);
		output[j + 2] = static_cast<unsigned char>((input[i] >> 16) & 0xff);
		output[j + 3] = static_cast<unsigned char>((input[i] >> 24) & 0xff);
	}
}

/* Decodes input (unsigned char) into output (unsigned). Assumes len is
 * a multiple of 4.
 */
void Decode(unsigned *output, const unsigned char *input, unsigned len)
{
	unsigned i, j;

	for (i = 0, j = 0; j < len; ++i, j += 4)
		output[i] = static_cast<unsigned>(input[j]) | (static_cast<unsigned>(input[j + 1]) << 8) | (static_cast<unsigned>(input[j + 2]) << 16) | (static_cast<unsigned>(input[j + 3]) << 24);
}

/*************************************************************************/

/* Module stuff. */

class EMD5 : public Module
{
 public:
	EMD5(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(ENCRYPTION);

		ModuleManager::Attach(I_OnEncrypt, this);
		ModuleManager::Attach(I_OnDecrypt, this);
		ModuleManager::Attach(I_OnCheckPassword, this);
	}

	EventReturn OnEncrypt(const std::string &src, std::string &dest)
	{
		MD5_CTX context;
		char *digest = new char[Config.PassLen];
		std::string buf = "md5:";
		char cpass[1000];

		MD5Init(&context);
		MD5Update(&context, reinterpret_cast<const unsigned char *>(src.c_str()), src.size());
		MD5Final(reinterpret_cast<unsigned char *>(digest), &context);

		b64_encode(digest, 16, cpass, 1000);
		buf += cpass;
		Alog(LOG_DEBUG_2) << "(enc_md5) hashed password from [" << src << "] to [" << buf << "]";
		dest = buf;
		delete [] digest;
		return EVENT_ALLOW;
	}

	EventReturn OnDecrypt(const std::string &hashm, const std::string &src, std::string &dest)
	{
		if (hashm != "md5")
			return EVENT_CONTINUE;
		return EVENT_STOP;
	}

	EventReturn OnCheckPassword(const std::string &hashm, std::string &plaintext, std::string &password)
	{
		if (hashm != "md5")
			return EVENT_CONTINUE;
		std::string buf;
		this->OnEncrypt(plaintext, buf);
		if (password == buf)
		{
			/* if we are NOT the first module in the list,
			 * we want to re-encrypt the pass with the new encryption
			 */
			if (Config.EncModuleList.front() != this->name)
				enc_encrypt(plaintext, password);
			return EVENT_ALLOW;
		}
		return EVENT_STOP;
	}
};

MODULE_INIT(EMD5)
