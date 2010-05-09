/*
 *   IRC - Internet Relay Chat, atheme2anope_identify.c
 *   (C) Copyright 2009, the Anope team (team@anope.org) 
 *
 *
 *   This program is free software; you can redistribute it and/or 
modify
 *   it under the terms of the GNU General Public License (see it online
 *   at http://www.gnu.org/copyleft/gpl.html) as published by the Free
 *   Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 */

#include "module.h"
#include <crypt.h>

#define TO_COLLIDE 0
#define XTOI(c) ((c)>9 ? (c)-'A'+10 : (c)-'0')
#define RAWMD5_PREFIX "$rawmd5$"

int do_atheme_identify(User *u);
int decrypt_password_posix(NickAlias *na, char *pass, char *tmp_pass);
int decrypt_password_md5(NickAlias *na, char *pass, char *tmp_pass);

int AnopeInit(int argc, char **argv)
{
	moduleAddAuthor("Anope");
	moduleAddVersion("$Id$");
	moduleSetType(SUPPORTED);

	Command *c;

	c = createCommand("ID", do_atheme_identify, NULL, NICK_HELP_IDENTIFY, -1, -1, -1, -1);
	moduleAddCommand(NICKSERV, c, MOD_HEAD);

	c = createCommand("IDENTIFY", do_atheme_identify, NULL, NICK_HELP_IDENTIFY, -1, -1, -1, -1);
	moduleAddCommand(NICKSERV, c, MOD_HEAD);

	c = createCommand("SIDENTIFY", do_atheme_identify, NULL, NICK_HELP_IDENTIFY, -1, -1, -1, -1);
	moduleAddCommand(NICKSERV, c, MOD_HEAD);

	return MOD_CONT;
}

int do_atheme_identify(User *u)
{
	char *buf, *pass;
	NickAlias *na;
	char tmp_pass[PASSMAX], tsbuf[16], modes[512];
	int loggedin = 0, len, res;
	NickRequest *nr;

	buf = moduleGetLastBuffer();
	pass = myStrGetToken(buf, ' ', 0);

	if (!pass) {
		syntax_error(s_NickServ, u, "IDENTIFY", NICK_IDENTIFY_SYNTAX);
		return MOD_CONT;
	}
	else if (!(na = u->na)) {
		if ((nr = findrequestnick(u->nick)))
			notice_lang(s_NickServ, u, NICK_IS_PREREG);
		else
			notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);

		free(pass);
		return MOD_CONT;
	}
	else if (na->status & NS_VERBOTEN) {
		notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);

		free(pass);
		return MOD_CONT;
	}
	else if (na->nc->flags & NI_SUSPENDED) {
		notice_lang(s_NickServ, u, NICK_X_SUSPENDED, na->nick);

		free(pass);
		return MOD_CONT;
	}
	else if (nick_identified(u)) {
		notice_lang(s_NickServ, u, NICK_ALREADY_IDENTIFIED);

		free(pass);
		return MOD_CONT;
	}

	if ((res = enc_check_password(pass, u->na->nc->pass)))
		loggedin = 2;
	else if (res == -1) {
		notice_lang(s_NickServ, u, NICK_IDENTIFY_FAILED);
	}
	if (loggedin || (enc_decrypt(na->nc->pass, tmp_pass, PASSMAX - 1)))
	{
		if (!loggedin)
		{
			if (decrypt_password_posix(na, pass, tmp_pass))
			{
				loggedin = 1;
			}
			else if (decrypt_password_md5(na, pass, tmp_pass))
			{
				loggedin = 1;
			}
		}

		if (loggedin)
		{

			if (loggedin == 1)
				alog("%s: Decrypted Atheme password for %s", s_NickServ, u->nick);
			enc_encrypt(pass, strlen(pass), u->na->nc->pass, PASSMAX - 1);

			if (!(na->status & NS_IDENTIFIED) && !(na->status & NS_RECOGNIZED))
			{
				if (na->last_usermask)
					free(na->last_usermask);

				na->last_usermask = scalloc(strlen(common_get_vident(u)) + strlen(common_get_vhost(u)) + 2, 1);

				sprintf(na->last_usermask, "%s@%s", common_get_vident(u), common_get_vhost(u));

				if (na->last_realname)
					free(na->last_realname);

				na->last_realname = sstrdup(u->realname);
			}

			na->status |= NS_IDENTIFIED;
			na->last_seen = time(NULL);
			snprintf(tsbuf, sizeof(tsbuf), "%lu", (unsigned long int) u->timestamp);

			if (ircd->modeonreg)
			{
				len = strlen(ircd->modeonreg);
				strncpy(modes, ircd->modeonreg, 512);

				if (ircd->rootmodeonid && is_services_root(u))
				{
					strncat(modes, ircd->rootmodeonid, 512 - len);
				}
				else if (ircd->adminmodeonid && is_services_admin(u))
				{
					strncat(modes, ircd->adminmodeonid, 512 - len);
				}
				else if (ircd->opermodeonid && is_services_oper(u))
				{
					strncat(modes, ircd->opermodeonid, 512 - len);
				}
				if (ircd->tsonmode)
					common_svsmode(u, modes, tsbuf);
				else
					common_svsmode(u, modes, "");
			}

			send_event(EVENT_NICK_IDENTIFY, 1, u->nick);
			alog("%s: %s!%s@%s identified for nick %s", s_NickServ, u->nick, u->username, u->host, u->nick);
			notice_lang(s_NickServ, u, NICK_IDENTIFY_SUCCEEDED);
			if (ircd->vhost)
				do_on_id(u);
			if (NSModeOnID)
				do_setmodes(u);

			if (NSForceEmail && u->na && !u->na->nc->email) {
				notice_lang(s_NickServ, u, NICK_IDENTIFY_EMAIL_REQUIRED);
				notice_lang(s_NickServ, u, NICK_IDENTIFY_EMAIL_HOWTO);
			}

			if (NSNickTracking)
				nsStartNickTracking(u);

			if (na->nc->flags & NI_KILLPROTECT)
				del_ns_timeout(na, TO_COLLIDE);

			free(pass);

			return MOD_STOP;
		}
		else {
			alog("%s: Failed IDENTIFY for %s!%s@%s", s_NickServ, u->nick, u->username, u->host);
			notice_lang(s_NickServ, u, PASSWORD_INCORRECT);
			bad_password(u);
		}
	}
	else {
		alog("%s: Failed IDENTIFY for %s!%s@%s", s_NickServ, u->nick, u->username, u->host);
		notice_lang(s_NickServ, u, PASSWORD_INCORRECT);
		bad_password(u);
	}

	if (pass)
		free(pass);

	return MOD_CONT;
}

int decrypt_password_posix(NickAlias *na, char *pass, char *tmp_pass)
{
	char *decpw;
	char tmp_pass_dec[PASSMAX];

	decpw = crypt(pass, tmp_pass);
	strscpy(tmp_pass_dec, decpw, PASSMAX);

	if (!strcmp(tmp_pass_dec, tmp_pass))
		return 1;
	
	return 0;
}

/********************************************************************/

/*
  Copyright (C) 1999 Aladdin Enterprises.  All rights reserved.

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  L. Peter Deutsch
  ghost@aladdin.com

 */
/*
  Independent implementation of MD5 (RFC 1321).

  This code implements the MD5 Algorithm defined in RFC 1321.
  It is derived directly from the text of the RFC and not from the
  reference implementation.

  The original and principal author of md5.c is L. Peter Deutsch
  <ghost@aladdin.com>.  Other authors are noted in the change history
  that follows (in reverse chronological order):

  1999-11-04 lpd Edited comments slightly for automatic TOC extraction.
  1999-10-18 lpd Fixed typo in header comment (ansi2knr rather than md5).
  1999-05-03 lpd Original version.
 */
/*
 * This code has some adaptations for the Ghostscript environment, but it
 * will compile and run correctly in any environment with 8-bit chars and
 * 32-bit ints.  Specifically, it assumes that if the following are
 * defined, they have the same meaning as in Ghostscript: P1, P2, P3,
 * ARCH_IS_BIG_ENDIAN.
 */

typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t; /* 32-bit word */

///* Define the state of the MD5 Algorithm. */
typedef struct md5_state_s {
    md5_word_t count[2];	/* message length in bits, lsw first */
    md5_word_t abcd[4];		/* digest buffer */
    md5_byte_t buf[64];		/* accumulate block */
} md5_state_t;

/********************************************************************/

#include <string.h>

#define T1 0xd76aa478
#define T2 0xe8c7b756
#define T3 0x242070db
#define T4 0xc1bdceee
#define T5 0xf57c0faf
#define T6 0x4787c62a
#define T7 0xa8304613
#define T8 0xfd469501
#define T9 0x698098d8
#define T10 0x8b44f7af
#define T11 0xffff5bb1
#define T12 0x895cd7be
#define T13 0x6b901122
#define T14 0xfd987193
#define T15 0xa679438e
#define T16 0x49b40821
#define T17 0xf61e2562
#define T18 0xc040b340
#define T19 0x265e5a51
#define T20 0xe9b6c7aa
#define T21 0xd62f105d
#define T22 0x02441453
#define T23 0xd8a1e681
#define T24 0xe7d3fbc8
#define T25 0x21e1cde6
#define T26 0xc33707d6
#define T27 0xf4d50d87
#define T28 0x455a14ed
#define T29 0xa9e3e905
#define T30 0xfcefa3f8
#define T31 0x676f02d9
#define T32 0x8d2a4c8a
#define T33 0xfffa3942
#define T34 0x8771f681
#define T35 0x6d9d6122
#define T36 0xfde5380c
#define T37 0xa4beea44
#define T38 0x4bdecfa9
#define T39 0xf6bb4b60
#define T40 0xbebfbc70
#define T41 0x289b7ec6
#define T42 0xeaa127fa
#define T43 0xd4ef3085
#define T44 0x04881d05
#define T45 0xd9d4d039
#define T46 0xe6db99e5
#define T47 0x1fa27cf8
#define T48 0xc4ac5665
#define T49 0xf4292244
#define T50 0x432aff97
#define T51 0xab9423a7
#define T52 0xfc93a039
#define T53 0x655b59c3
#define T54 0x8f0ccc92
#define T55 0xffeff47d
#define T56 0x85845dd1
#define T57 0x6fa87e4f
#define T58 0xfe2ce6e0
#define T59 0xa3014314
#define T60 0x4e0811a1
#define T61 0xf7537e82
#define T62 0xbd3af235
#define T63 0x2ad7d2bb
#define T64 0xeb86d391

static void
md5_process(md5_state_t *pms, const md5_byte_t *data /*[64]*/)
{
    md5_word_t
	a = pms->abcd[0], b = pms->abcd[1],
	c = pms->abcd[2], d = pms->abcd[3];
    md5_word_t t;

    /*
     * On big-endian machines, we must arrange the bytes in the right
     * order.  (This also works on machines of unknown byte order.)
     */
    md5_word_t X[16];
    const md5_byte_t *xp = data;
    int i;

    for (i = 0; i < 16; ++i, xp += 4)
	X[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

    /* Round 1. */
    /* Let [abcd k s i] denote the operation
       a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + F(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
    /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  7,  T1);
    SET(d, a, b, c,  1, 12,  T2);
    SET(c, d, a, b,  2, 17,  T3);
    SET(b, c, d, a,  3, 22,  T4);
    SET(a, b, c, d,  4,  7,  T5);
    SET(d, a, b, c,  5, 12,  T6);
    SET(c, d, a, b,  6, 17,  T7);
    SET(b, c, d, a,  7, 22,  T8);
    SET(a, b, c, d,  8,  7,  T9);
    SET(d, a, b, c,  9, 12, T10);
    SET(c, d, a, b, 10, 17, T11);
    SET(b, c, d, a, 11, 22, T12);
    SET(a, b, c, d, 12,  7, T13);
    SET(d, a, b, c, 13, 12, T14);
    SET(c, d, a, b, 14, 17, T15);
    SET(b, c, d, a, 15, 22, T16);
#undef SET

     /* Round 2. */
     /* Let [abcd k s i] denote the operation
          a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + G(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
     /* Do the following 16 operations. */
    SET(a, b, c, d,  1,  5, T17);
    SET(d, a, b, c,  6,  9, T18);
    SET(c, d, a, b, 11, 14, T19);
    SET(b, c, d, a,  0, 20, T20);
    SET(a, b, c, d,  5,  5, T21);
    SET(d, a, b, c, 10,  9, T22);
    SET(c, d, a, b, 15, 14, T23);
    SET(b, c, d, a,  4, 20, T24);
    SET(a, b, c, d,  9,  5, T25);
    SET(d, a, b, c, 14,  9, T26);
    SET(c, d, a, b,  3, 14, T27);
    SET(b, c, d, a,  8, 20, T28);
    SET(a, b, c, d, 13,  5, T29);
    SET(d, a, b, c,  2,  9, T30);
    SET(c, d, a, b,  7, 14, T31);
    SET(b, c, d, a, 12, 20, T32);
#undef SET

     /* Round 3. */
     /* Let [abcd k s t] denote the operation
          a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + H(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
     /* Do the following 16 operations. */
    SET(a, b, c, d,  5,  4, T33);
    SET(d, a, b, c,  8, 11, T34);
    SET(c, d, a, b, 11, 16, T35);
    SET(b, c, d, a, 14, 23, T36);
    SET(a, b, c, d,  1,  4, T37);
    SET(d, a, b, c,  4, 11, T38);
    SET(c, d, a, b,  7, 16, T39);
    SET(b, c, d, a, 10, 23, T40);
    SET(a, b, c, d, 13,  4, T41);
    SET(d, a, b, c,  0, 11, T42);
    SET(c, d, a, b,  3, 16, T43);
    SET(b, c, d, a,  6, 23, T44);
    SET(a, b, c, d,  9,  4, T45);
    SET(d, a, b, c, 12, 11, T46);
    SET(c, d, a, b, 15, 16, T47);
    SET(b, c, d, a,  2, 23, T48);
#undef SET

     /* Round 4. */
     /* Let [abcd k s t] denote the operation
          a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
  t = a + I(b,c,d) + X[k] + Ti;\
  a = ROTATE_LEFT(t, s) + b
     /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  6, T49);
    SET(d, a, b, c,  7, 10, T50);
    SET(c, d, a, b, 14, 15, T51);
    SET(b, c, d, a,  5, 21, T52);
    SET(a, b, c, d, 12,  6, T53);
    SET(d, a, b, c,  3, 10, T54);
    SET(c, d, a, b, 10, 15, T55);
    SET(b, c, d, a,  1, 21, T56);
    SET(a, b, c, d,  8,  6, T57);
    SET(d, a, b, c, 15, 10, T58);
    SET(c, d, a, b,  6, 15, T59);
    SET(b, c, d, a, 13, 21, T60);
    SET(a, b, c, d,  4,  6, T61);
    SET(d, a, b, c, 11, 10, T62);
    SET(c, d, a, b,  2, 15, T63);
    SET(b, c, d, a,  9, 21, T64);
#undef SET

     /* Then perform the following additions. (That is increment each
        of the four registers by the value it had before this block
        was started.) */
    pms->abcd[0] += a;
    pms->abcd[1] += b;
    pms->abcd[2] += c;
    pms->abcd[3] += d;
}

void
md5_init(md5_state_t *pms)
{
    pms->count[0] = pms->count[1] = 0;
    pms->abcd[0] = 0x67452301;
    pms->abcd[1] = 0xefcdab89;
    pms->abcd[2] = 0x98badcfe;
    pms->abcd[3] = 0x10325476;
}

void
md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes)
{
    const md5_byte_t *p = data;
    int left = nbytes;
    int offset = (pms->count[0] >> 3) & 63;
    md5_word_t nbits = (md5_word_t)(nbytes << 3);

    if (nbytes <= 0)
	return;

    /* Update the message length. */
    pms->count[1] += nbytes >> 29;
    pms->count[0] += nbits;
    if (pms->count[0] < nbits)
	pms->count[1]++;

    /* Process an initial partial block. */
    if (offset) {
	int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

	memcpy(pms->buf + offset, p, copy);
	if (offset + copy < 64)
	    return;
	p += copy;
	left -= copy;
	md5_process(pms, pms->buf);
    }

    /* Process full blocks. */
    for (; left >= 64; p += 64, left -= 64)
	md5_process(pms, p);

    /* Process a final partial block. */
    if (left)
	memcpy(pms->buf, p, left);
}

void
md5_finish(md5_state_t *pms, md5_byte_t digest[16])
{
    static const md5_byte_t pad[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    md5_byte_t data[8];
    int i;

    /* Save the length before padding. */
    for (i = 0; i < 8; ++i)
	data[i] = (md5_byte_t)(pms->count[i >> 2] >> ((i & 3) << 3));
    /* Pad to 56 bytes mod 64. */
    md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);
    /* Append the length. */
    md5_append(pms, data, 8);
    for (i = 0; i < 16; ++i)
	digest[i] = (md5_byte_t)(pms->abcd[i >> 2] >> ((i & 3) << 3));
}

/********************************************************************/

int decrypt_password_md5(NickAlias *na, char *pass, char *tmp_pass)
{
	char output[2 * 16 + sizeof(RAWMD5_PREFIX)];
	md5_state_t ctx;
	unsigned char digest[16];
	int i;
	char tmp_pass_dec[PASSMAX];

	md5_init(&ctx);
	md5_append(&ctx, (const unsigned char *) pass, strlen(pass));
	md5_finish(&ctx, digest);

	strcpy(output, RAWMD5_PREFIX);

	for (i = 0; i < 16; i++)
		sprintf(output + sizeof(RAWMD5_PREFIX) - 1 + i * 2, "%02x", 255 & digest[i]);

	strscpy(tmp_pass_dec, output, PASSMAX);

	if (!strcmp(tmp_pass_dec, tmp_pass))
		return 1;

	return 0;
}
