/*
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain

Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

/* #define LITTLE_ENDIAN * This should be #define'd if true. */

#include "module.h"

struct SHA1_CTX
{
	uint32 state[5];
	uint32 count[2];
	unsigned char buffer[64];
};

void SHA1Transform(uint32 state[5], const unsigned char buffer[64]);
void SHA1Init(SHA1_CTX *context);
void SHA1Update(SHA1_CTX *context, const unsigned char *data, uint32 len);
void SHA1Final(unsigned char digest[20], SHA1_CTX *context);

inline static uint32 rol(uint32 value, uint32 bits) { return (value << bits) | (value >> (32 - bits)); }

union CHAR64LONG16
{
	unsigned char c[64];
	uint32 l[16];
};

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
inline static uint32 blk0(CHAR64LONG16 *block, uint32 i)
{
#ifdef LITTLE_ENDIAN
	return block->l[i] = (rol(block->l[i], 24) & 0xFF00FF00) | (rol(block->l[i], 8) & 0x00FF00FF);
#else
	return block->l[i];
#endif
}
inline static uint32 blk(CHAR64LONG16 *block, uint32 i) { return block->l[i & 15] = rol(block->l[(i + 13) & 15] ^ block->l[(i + 8) & 15] ^ block->l[(i + 2) & 15] ^ block->l[i & 15],1); }

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
inline static void R0(CHAR64LONG16 *block, uint32 v, uint32 &w, uint32 x, uint32 y, uint32 &z, uint32 i) { z += ((w & (x ^ y)) ^ y) + blk0(block, i) + 0x5A827999 + rol(v, 5); w = rol(w, 30); }
inline static void R1(CHAR64LONG16 *block, uint32 v, uint32 &w, uint32 x, uint32 y, uint32 &z, uint32 i) { z += ((w & (x ^ y)) ^ y) + blk(block, i) + 0x5A827999 + rol(v, 5); w = rol(w, 30); }
inline static void R2(CHAR64LONG16 *block, uint32 v, uint32 &w, uint32 x, uint32 y, uint32 &z, uint32 i) { z += (w ^ x ^ y) + blk(block, i) + 0x6ED9EBA1 + rol(v, 5); w = rol(w, 30); }
inline static void R3(CHAR64LONG16 *block, uint32 v, uint32 &w, uint32 x, uint32 y, uint32 &z, uint32 i) { z += (((w | x) & y) | (w & x)) + blk(block, i) + 0x8F1BBCDC + rol(v, 5); w = rol(w, 30); }
inline static void R4(CHAR64LONG16 *block, uint32 v, uint32 &w, uint32 x, uint32 y, uint32 &z, uint32 i) { z += (w ^ x ^ y) + blk(block, i) + 0xCA62C1D6 + rol(v, 5); w = rol(w, 30); }

/* Hash a single 512-bit block. This is the core of the algorithm. */

void SHA1Transform(uint32 state[5], const unsigned char buffer[64])
{
	uint32 a, b, c, d, e;
	static unsigned char workspace[64];
	CHAR64LONG16 *block = reinterpret_cast<CHAR64LONG16 *>(workspace);
	memcpy(block, buffer, 64);
	/* Copy context->state[] to working vars */
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];
	/* 4 rounds of 20 operations each. Loop unrolled. */
	R0(block, a, b, c, d, e, 0); R0(block, e, a, b, c, d, 1); R0(block, d, e, a, b, c, 2); R0(block, c, d, e, a, b, 3);
	R0(block, b, c, d, e, a, 4); R0(block, a, b, c, d, e, 5); R0(block, e, a, b, c, d, 6); R0(block, d, e, a, b, c, 7);
	R0(block, c, d, e, a, b, 8); R0(block, b, c, d, e, a, 9); R0(block, a, b, c, d, e, 10); R0(block, e, a, b, c, d, 11);
	R0(block, d, e, a, b, c, 12); R0(block, c, d, e, a, b, 13); R0(block, b, c, d, e, a, 14); R0(block, a, b, c, d, e, 15);
	R1(block, e, a, b, c, d, 16); R1(block, d, e, a, b, c, 17); R1(block, c, d, e, a, b, 18); R1(block, b, c, d, e, a, 19);
	R2(block, a, b, c, d, e, 20); R2(block, e, a, b, c, d, 21); R2(block, d, e, a, b, c, 22); R2(block, c, d, e, a, b, 23);
	R2(block, b, c, d, e, a, 24); R2(block, a, b, c, d, e, 25); R2(block, e, a, b, c, d, 26); R2(block, d, e, a, b, c, 27);
	R2(block, c, d, e, a, b, 28); R2(block, b, c, d, e, a, 29); R2(block, a, b, c, d, e, 30); R2(block, e, a, b, c, d, 31);
	R2(block, d, e, a, b, c, 32); R2(block, c, d, e, a, b, 33); R2(block, b, c, d, e, a, 34); R2(block, a, b, c, d, e, 35);
	R2(block, e, a, b, c, d, 36); R2(block, d, e, a, b, c, 37); R2(block, c, d, e, a, b, 38); R2(block, b, c, d, e, a, 39);
	R3(block, a, b, c, d, e, 40); R3(block, e, a, b, c, d, 41); R3(block, d, e, a, b, c, 42); R3(block, c, d, e, a, b, 43);
	R3(block, b, c, d, e, a, 44); R3(block, a, b, c, d, e, 45); R3(block, e, a, b, c, d, 46); R3(block, d, e, a, b, c, 47);
	R3(block, c, d, e, a, b, 48); R3(block, b, c, d, e, a, 49); R3(block, a, b, c, d, e, 50); R3(block, e, a, b, c, d, 51);
	R3(block, d, e, a, b, c, 52); R3(block, c, d, e, a, b, 53); R3(block, b, c, d, e, a, 54); R3(block, a, b, c, d, e, 55);
	R3(block, e, a, b, c, d, 56); R3(block, d, e, a, b, c, 57); R3(block, c, d, e, a, b, 58); R3(block, b, c, d, e, a, 59);
	R4(block, a, b, c, d, e, 60); R4(block, e, a, b, c, d, 61); R4(block, d, e, a, b, c, 62); R4(block, c, d, e, a, b, 63);
	R4(block, b, c, d, e, a, 64); R4(block, a, b, c, d, e, 65); R4(block, e, a, b, c, d, 66); R4(block, d, e, a, b, c, 67);
	R4(block, c, d, e, a, b, 68); R4(block, b, c, d, e, a, 69); R4(block, a, b, c, d, e, 70); R4(block, e, a, b, c, d, 71);
	R4(block, d, e, a, b, c, 72); R4(block, c, d, e, a, b, 73); R4(block, b, c, d, e, a, 74); R4(block, a, b, c, d, e, 75);
	R4(block, e, a, b, c, d, 76); R4(block, d, e, a, b, c, 77); R4(block, c, d, e, a, b, 78); R4(block, b, c, d, e, a, 79);
	/* Add the working vars back into context.state[] */
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	/* Wipe variables */
	a = b = c = d = e = 0;
}

/* SHA1Init - Initialize new context */

void SHA1Init(SHA1_CTX *context)
{
	/* SHA1 initialization constants */
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
	context->state[4] = 0xC3D2E1F0;
	context->count[0] = context->count[1] = 0;
}

/* Run your data through this. */

void SHA1Update(SHA1_CTX *context, const unsigned char *data, uint32 len)
{
	uint32 i, j;

	j = (context->count[0] >> 3) & 63;
	if ((context->count[0] += len << 3) < (len << 3))
		++context->count[1];
	context->count[1] += len >> 29;
	if (j + len > 63)
	{
		memcpy(&context->buffer[j], data, (i = 64 - j));
		SHA1Transform(context->state, context->buffer);
		for (; i + 63 < len; i += 64)
			SHA1Transform(context->state, &data[i]);
		j = 0;
	}
	else
		i = 0;
	memcpy(&context->buffer[j], &data[i], len - i);
}

/* Add padding and return the message digest. */

void SHA1Final(unsigned char digest[21], SHA1_CTX *context)
{
	uint32 i;
	unsigned char finalcount[8];

	for (i = 0; i < 8; ++i)
		finalcount[i] = static_cast<unsigned char>((context->count[i >= 4 ? 0 : 1] >> ((3 - (i & 3)) * 8)) & 255); /* Endian independent */
	SHA1Update(context, reinterpret_cast<const unsigned char *>("\200"), 1);
	while ((context->count[0] & 504) != 448)
		SHA1Update(context, reinterpret_cast<const unsigned char *>("\0"), 1);
	SHA1Update(context, finalcount, 8); /* Should cause a SHA1Transform() */
	for (i = 0; i < 20; ++i)
		digest[i] = static_cast<unsigned char>((context->state[i>>2] >> ((3 - (i & 3)) * 8)) & 255);
	/* Wipe variables */
	i = 0;
	memset(context->buffer, 0, 64);
	memset(context->state, 0, 20);
	memset(context->count, 0, 8);
	memset(&finalcount, 0, 8);
	SHA1Transform(context->state, context->buffer);
}

/*****************************************************************************/
/*****************************************************************************/

/* Module stuff. */

class ESHA1 : public Module
{
 public:
	ESHA1(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(ENCRYPTION);

		Implementation i[] = { I_OnEncrypt, I_OnDecrypt, I_OnCheckAuthentication };
		ModuleManager::Attach(i, this, 3);
	}

	EventReturn OnEncrypt(const Anope::string &src, Anope::string &dest)
	{
		SHA1_CTX context;
		char digest[21] = "";
		Anope::string buf = "sha1:";

		SHA1Init(&context);
		SHA1Update(&context, reinterpret_cast<const unsigned char *>(src.c_str()), src.length());
		SHA1Final(reinterpret_cast<unsigned char *>(digest), &context);

		buf += Anope::Hex(digest, 20);
		Log(LOG_DEBUG_2) << "(enc_sha1) hashed password from [" << src << "] to [" << buf << "]";
		dest = buf;
		return EVENT_ALLOW;
	}

	EventReturn OnDecrypt(const Anope::string &hashm, const Anope::string &src, Anope::string &dest)
	{
		if (!hashm.equals_cs("sha1"))
			return EVENT_CONTINUE;
		return EVENT_STOP;
	}

	EventReturn OnCheckAuthentication(User *u, Command *c, const std::vector<Anope::string> &params, const Anope::string &account, const Anope::string &password)
	{
		NickAlias *na = findnick(account);
		NickCore *nc = na ? na->nc : NULL;
		if (na == NULL)
			return EVENT_CONTINUE;

		size_t pos = nc->pass.find(':');
		if (pos == Anope::string::npos)
			return EVENT_CONTINUE;
		Anope::string hash_method(nc->pass.begin(), nc->pass.begin() + pos);
		if (!hash_method.equals_cs("sha1"))
			return EVENT_CONTINUE;

		Anope::string buf;
		this->OnEncrypt(password, buf);
		if (nc->pass.equals_cs(buf))
		{
			/* when we are NOT the first module in the list,
			 * we want to re-encrypt the pass with the new encryption
			 */
			if (!this->name.equals_ci(Config->EncModuleList.front()))
				enc_encrypt(password, nc->pass);
			return EVENT_ALLOW;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ESHA1)
