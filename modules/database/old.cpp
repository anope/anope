/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

/* Dependencies: anope_chanserv.access */

#include "module.h"
#include "modules/operserv/session.h"
#include "modules/botserv/kick.h"
#include "modules/chanserv/mode.h"
#include "modules/botserv/badwords.h"
#include "modules/operserv/news.h"
#include "modules/operserv/forbid.h"
#include "modules/chanserv/entrymsg.h"
#include "modules/nickserv/suspend.h"
#include "modules/chanserv/suspend.h"
#include "modules/chanserv/access.h"
#include "modules/nickserv/access.h"

#define READ(x) \
if (true) \
{ \
	if ((x) < 0) \
		printf("Error, the database is broken, line %d, trying to continue... no guarantee.\n", __LINE__); \
} \
else \
	static_cast<void>(0)

#define getc_db(f) (fgetc((f)->fp))
#define read_db(f, buf, len) (fread((buf), 1, (len), (f)->fp))
#define read_buffer(buf, f) ((read_db((f), (buf), sizeof(buf)) == sizeof(buf)) ? 0 : -1)

#define OLD_BI_PRIVATE	0x0001

#define OLD_NI_KILLPROTECT	0x00000001 /* Kill others who take this nick */
#define OLD_NI_SECURE		0x00000002 /* Don't recognize unless IDENTIFY'd */
#define OLD_NI_MSG			0x00000004 /* Use PRIVMSGs instead of NOTICEs */
#define OLD_NI_MEMO_HARDMAX	0x00000008 /* Don't allow user to change memo limit */
#define OLD_NI_MEMO_SIGNON	0x00000010 /* Notify of memos at signon and un-away */
#define OLD_NI_MEMO_RECEIVE	0x00000020 /* Notify of new memos when sent */
#define OLD_NI_PRIVATE		0x00000040 /* Don't show in LIST to non-servadmins */
#define OLD_NI_HIDE_EMAIL	0x00000080 /* Don't show E-mail in INFO */
#define OLD_NI_HIDE_MASK	0x00000100 /* Don't show last seen address in INFO */
#define OLD_NI_HIDE_QUIT	0x00000200 /* Don't show last quit message in INFO */
#define OLD_NI_KILL_QUICK	0x00000400 /* Kill in 20 seconds instead of 60 */
#define OLD_NI_KILL_IMMED	0x00000800 /* Kill immediately instead of in 60 sec */
#define OLD_NI_MEMO_MAIL	0x00010000 /* User gets email on memo */
#define OLD_NI_HIDE_STATUS	0x00020000 /* Don't show services access status */
#define OLD_NI_SUSPENDED	0x00040000 /* Nickname is suspended */
#define OLD_NI_AUTOOP		0x00080000 /* Autoop nickname in channels */

#define OLD_NS_NO_EXPIRE		0x0004     /* nick won't expire */
#define OLD_NS_VERBOTEN			0x0002

#define OLD_CI_KEEPTOPIC		0x00000001
#define OLD_CI_SECUREOPS		0x00000002
#define OLD_CI_PRIVATE			0x00000004
#define OLD_CI_TOPICLOCK		0x00000008
#define OLD_CI_RESTRICTED		0x00000010
#define OLD_CI_PEACE			0x00000020
#define OLD_CI_SECURE			0x00000040
#define OLD_CI_VERBOTEN			0x00000080
#define OLD_CI_ENCRYPTEDPW		0x00000100
#define OLD_CI_NO_EXPIRE		0x00000200
#define OLD_CI_MEMO_HARDMAX		0x00000400
#define OLD_CI_OPNOTICE			0x00000800
#define OLD_CI_SECUREFOUNDER	0x00001000
#define OLD_CI_SIGNKICK			0x00002000
#define OLD_CI_SIGNKICK_LEVEL	0x00004000
#define OLD_CI_XOP				0x00008000
#define OLD_CI_SUSPENDED		0x00010000

/* BotServ SET flags */
#define OLD_BS_DONTKICKOPS		0x00000001
#define OLD_BS_DONTKICKVOICES	0x00000002
#define OLD_BS_FANTASY			0x00000004
#define OLD_BS_SYMBIOSIS		0x00000008
#define OLD_BS_GREET			0x00000010
#define OLD_BS_NOBOT			0x00000020

/* BotServ Kickers flags */
#define OLD_BS_KICK_BOLDS 		0x80000000
#define OLD_BS_KICK_COLORS 		0x40000000
#define OLD_BS_KICK_REVERSES	0x20000000
#define OLD_BS_KICK_UNDERLINES	0x10000000
#define OLD_BS_KICK_BADWORDS	0x08000000
#define OLD_BS_KICK_CAPS		0x04000000
#define OLD_BS_KICK_FLOOD		0x02000000
#define OLD_BS_KICK_REPEAT		0x01000000

#define OLD_NEWS_LOGON	0
#define OLD_NEWS_OPER	1
#define OLD_NEWS_RANDOM	2

enum
{
	TTB_BOLDS,
	TTB_COLORS,
	TTB_REVERSES,
	TTB_UNDERLINES,
	TTB_BADWORDS,
	TTB_CAPS,
	TTB_FLOOD,
	TTB_REPEAT,
};

static struct mlock_info
{
	char c;
	uint32_t m;
} mlock_infos[] = {
	{'i', 0x00000001},
	{'m', 0x00000002},
	{'n', 0x00000004},
	{'p', 0x00000008},
	{'s', 0x00000010},
	{'t', 0x00000020},
	{'R', 0x00000100},
	{'r', 0x00000200},
	{'c', 0x00000400},
	{'A', 0x00000800},
	{'K', 0x00002000},
	{'O', 0x00008000},
	{'Q', 0x00010000},
	{'S', 0x00020000},
	{'G', 0x00100000},
	{'C', 0x00200000},
	{'u', 0x00400000},
	{'z', 0x00800000},
	{'N', 0x01000000},
	{'M', 0x04000000}
};

static Anope::string hashm;

enum
{
	LANG_EN_US, /* United States English */
	LANG_JA_JIS, /* Japanese (JIS encoding) */
	LANG_JA_EUC, /* Japanese (EUC encoding) */
	LANG_JA_SJIS, /* Japanese (SJIS encoding) */
	LANG_ES, /* Spanish */
	LANG_PT, /* Portugese */
	LANG_FR, /* French */
	LANG_TR, /* Turkish */
	LANG_IT, /* Italian */
	LANG_DE, /* German */
	LANG_CAT, /* Catalan */
	LANG_GR, /* Greek */
	LANG_NL, /* Dutch */
	LANG_RU, /* Russian */
	LANG_HUN, /* Hungarian */
	LANG_PL /* Polish */
};

static void process_mlock(ChanServ::Channel *ci, uint32_t lock, bool status, uint32_t *limit, Anope::string *key)
{
	ServiceReference<ModeLocks> mlocks;
	
	if (!mlocks)
		return;

	for (unsigned i = 0; i < (sizeof(mlock_infos) / sizeof(mlock_info)); ++i)
		if (lock & mlock_infos[i].m)
		{
			ChannelMode *cm = ModeManager::FindChannelModeByChar(mlock_infos[i].c);
			if (cm)
			{
				if (limit && mlock_infos[i].c == 'l')
					mlocks->SetMLock(ci, cm, status, stringify(*limit));
				else if (key && mlock_infos[i].c == 'k')
					mlocks->SetMLock(ci, cm, status, *key);
				else
					mlocks->SetMLock(ci, cm, status);
			}
		}
}

static const char Base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char Pad64 = '=';

static void my_b64_encode(const Anope::string &src, Anope::string &target)
{
	size_t src_pos = 0, src_len = src.length();
	unsigned char input[3];

	target.clear();

	while (src_len - src_pos > 2)
	{
		input[0] = src[src_pos++];
		input[1] = src[src_pos++];
		input[2] = src[src_pos++];

		target += Base64[input[0] >> 2];
		target += Base64[((input[0] & 0x03) << 4) + (input[1] >> 4)];
		target += Base64[((input[1] & 0x0f) << 2) + (input[2] >> 6)];
		target += Base64[input[2] & 0x3f];
	}

	/* Now we worry about padding */
	if (src_pos != src_len)
	{
		input[0] = input[1] = input[2] = 0;
		for (size_t i = 0; i < src_len - src_pos; ++i)
			input[i] = src[src_pos + i];

		target += Base64[input[0] >> 2];
		target += Base64[((input[0] & 0x03) << 4) + (input[1] >> 4)];
		if (src_pos == src_len - 1)
			target += Pad64;
		else
			target += Base64[((input[1] & 0x0f) << 2) + (input[2] >> 6)];
		target += Pad64;
	}
}

static Anope::string Hex(const char *data, size_t l)
{
	const char hextable[] = "0123456789abcdef";

	std::string rv;
	for (size_t i = 0; i < l; ++i)
	{
		unsigned char c = data[i];
		rv += hextable[c >> 4];
		rv += hextable[c & 0xF];
	}
	return rv;
}

static Anope::string GetLevelName(int level)
{
	switch (level)
	{
		case 0:
			return "INVITE";
		case 1:
			return "AKICK";
		case 2:
			return "SET";
		case 3:
			return "UNBAN";
		case 4:
			return "AUTOOP";
		case 5:
			return "AUTODEOP";
		case 6:
			return "AUTOVOICE";
		case 7:
			return "OP";
		case 8:
			return "ACCESS_LIST";
		case 9:
			return "CLEAR";
		case 10:
			return "NOJOIN";
		case 11:
			return "ACCESS_CHANGE";
		case 12:
			return "MEMO";
		case 13:
			return "ASSIGN";
		case 14:
			return "BADWORDS";
		case 15:
			return "NOKICK";
		case 16:
			return "FANTASIA";
		case 17:
			return "SAY";
		case 18:
			return "GREET";
		case 19:
			return "VOICEME";
		case 20:
			return "VOICE";
		case 21:
			return "GETKEY";
		case 22:
			return "AUTOHALFOP";
		case 23:
			return "AUTOPROTECT";
		case 24:
			return "OPME";
		case 25:
			return "HALFOPME";
		case 26:
			return "HALFOP";
		case 27:
			return "PROTECTME";
		case 28:
			return "PROTECT";
		case 29:
			return "KICKME";
		case 30:
			return "KICK";
		case 31:
			return "SIGNKICK";
		case 32:
			return "BANME";
		case 33:
			return "BAN";
		case 34:
			return "TOPIC";
		case 35:
			return "INFO";
		default:
			return "INVALID";
	}
}

static char *strscpy(char *d, const char *s, size_t len)
{
	char *d_orig = d;

	if (!len)
		return d;
	while (--len && (*d++ = *s++));
	*d = '\0';
	return d_orig;
}

struct dbFILE
{
	int mode;				/* 'r' for reading, 'w' for writing */
	FILE *fp;				/* The normal file descriptor */
	char filename[1024];	/* Name of the database file */
};

static dbFILE *open_db_read(const char *service, const char *filename, int version)
{
	dbFILE *f;
	FILE *fp;
	int myversion;

	f = new dbFILE;
	strscpy(f->filename, (Anope::DataDir + "/" + filename).c_str(), sizeof(f->filename));
	f->mode = 'r';
	fp = fopen(f->filename, "rb");
	if (!fp)
	{
		Log() << "Can't read " << service << " database " << f->filename;
		delete f;
		return NULL;
	}
	f->fp = fp;
	myversion = fgetc(fp) << 24 | fgetc(fp) << 16 | fgetc(fp) << 8 | fgetc(fp);
	if (feof(fp))
	{
		Log() << "Error reading version number on " << f->filename << ": End of file detected.";
		delete f;
		return NULL;
	}
	else if (myversion < version)
	{
		Log() << "Unsuported database version (" << myversion << ") on " << f->filename << ".";
		delete f;
		return NULL;
	}
	return f;
}

void close_db(dbFILE *f)
{
	fclose(f->fp);
	delete f;
}

static int read_int16(int16_t *ret, dbFILE *f)
{
	int c1, c2;

	*ret = 0;

	c1 = fgetc(f->fp);
	c2 = fgetc(f->fp);
	if (c1 == EOF || c2 == EOF)
		return -1;
	*ret = c1 << 8 | c2;
	return 0;
}

static int read_uint16(uint16_t *ret, dbFILE *f)
{
	int c1, c2;

	*ret = 0;

	c1 = fgetc(f->fp);
	c2 = fgetc(f->fp);
	if (c1 == EOF || c2 == EOF)
		return -1;
	*ret = c1 << 8 | c2;
	return 0;
}

static int read_string(Anope::string &str, dbFILE *f)
{
	str.clear();
	uint16_t len;

	if (read_uint16(&len, f) < 0)
		return -1;
	if (len == 0)
		return 0;
	char *s = new char[len];
	if (len != fread(s, 1, len, f->fp))
	{
		delete [] s;
		return -1;
	}
	str = s;
	delete [] s;
	return 0;
}

static int read_uint32(uint32_t *ret, dbFILE *f)
{
	int c1, c2, c3, c4;

	*ret = 0;

	c1 = fgetc(f->fp);
	c2 = fgetc(f->fp);
	c3 = fgetc(f->fp);
	c4 = fgetc(f->fp);
	if (c1 == EOF || c2 == EOF || c3 == EOF || c4 == EOF)
		return -1;
	*ret = c1 << 24 | c2 << 16 | c3 << 8 | c4;
	return 0;
}

int read_int32(int32_t *ret, dbFILE *f)
{
	int c1, c2, c3, c4;

	*ret = 0;

	c1 = fgetc(f->fp);
	c2 = fgetc(f->fp);
	c3 = fgetc(f->fp);
	c4 = fgetc(f->fp);
	if (c1 == EOF || c2 == EOF || c3 == EOF || c4 == EOF)
		return -1;
	*ret = c1 << 24 | c2 << 16 | c3 << 8 | c4;
	return 0;
}

static void LoadNicks()
{
	if (!NickServ::service)
		return;
	dbFILE *f = open_db_read("NickServ", "nick.db", 14);
	if (f == NULL)
		return;
	for (int i = 0; i < 1024; ++i)
		for (int c; (c = getc_db(f)) == 1;)
		{
			Anope::string buffer;

			READ(read_string(buffer, f));

			NickServ::Account *nc = Serialize::New<NickServ::Account *>();
			nc->SetDisplay(buffer);

			const Anope::string settings[] = { "killprotect", "kill_quick", "ns_secure", "ns_private", "hide_email",
				"hide_mask", "hide_quit", "memo_signon", "memo_receive", "autoop", "msg", "ns_keepmodes" };
			for (unsigned j = 0; j < sizeof(settings) / sizeof(Anope::string); ++j)
				nc->UnsetS<bool>(settings[j].upper());

			char pwbuf[32];
			READ(read_buffer(pwbuf, f));
			if (hashm == "plain")
			{
				Anope::string p;
				my_b64_encode(pwbuf, p);
				nc->SetPassword(p);
			}
			else if (hashm == "md5" || hashm == "oldmd5")
				nc->SetPassword(Hex(pwbuf, 16));
			else if (hashm == "sha1")
				nc->SetPassword(Hex(pwbuf, 20));
			else
				nc->SetPassword(Hex(pwbuf, strlen(pwbuf)));
			nc->SetPassword(hashm + ":" + nc->GetPassword());

			READ(read_string(buffer, f));
			nc->SetEmail(buffer);

			READ(read_string(buffer, f));
			if (!buffer.empty())
				nc->Extend<Anope::string>("greet", buffer);

			uint32_t u32;
			READ(read_uint32(&u32, f));
			//nc->icq = u32;

			READ(read_string(buffer, f));
			//nc->url = buffer;

			READ(read_uint32(&u32, f));
			if (u32 & OLD_NI_KILLPROTECT)
				nc->SetS<bool>("KILLPROTECT", true);
			if (u32 & OLD_NI_SECURE)
				nc->SetS<bool>("NS_SECURE", true);
			if (u32 & OLD_NI_MSG)
				nc->SetS<bool>("MSG", true);
			if (u32 & OLD_NI_MEMO_HARDMAX)
				nc->SetS<bool>("MEMO_HARDMAX", true);
			if (u32 & OLD_NI_MEMO_SIGNON)
				nc->SetS<bool>("MEMO_SIGNON", true);
			if (u32 & OLD_NI_MEMO_RECEIVE)
				nc->SetS<bool>("MEMO_RECEIVE", true);
			if (u32 & OLD_NI_PRIVATE)
				nc->SetS<bool>("NS_PRIVATE", true);
			if (u32 & OLD_NI_HIDE_EMAIL)
				nc->SetS<bool>("HIDE_EMAIL", true);
			if (u32 & OLD_NI_HIDE_MASK)
				nc->SetS<bool>("HIDE_MASK", true);
			if (u32 & OLD_NI_HIDE_QUIT)
				nc->SetS<bool>("HIDE_QUIT", true);
			if (u32 & OLD_NI_KILL_QUICK)
				nc->SetS<bool>("KILL_QUICK", true);
			if (u32 & OLD_NI_KILL_IMMED)
				nc->SetS<bool>("KILL_IMMED", true);
			if (u32 & OLD_NI_MEMO_MAIL)
				nc->SetS<bool>("MEMO_MAIL", true);
			if (u32 & OLD_NI_HIDE_STATUS)
				nc->SetS<bool>("HIDE_STATUS", true);
			if (u32 & OLD_NI_SUSPENDED)
			{
				NSSuspendInfo *si = Serialize::New<NSSuspendInfo *>();
				if (si)
				{
					si->SetAccount(nc);
				}
			}
			if (!(u32 & OLD_NI_AUTOOP))
				nc->SetS<bool>("AUTOOP", true);

			uint16_t u16;
			READ(read_uint16(&u16, f));
			switch (u16)
			{
				case LANG_ES:
					nc->SetLanguage("es_ES");
					break;
				case LANG_PT:
					nc->SetLanguage("pt_PT");
					break;
				case LANG_FR:
					nc->SetLanguage("fr_FR");
					break;
				case LANG_TR:
					nc->SetLanguage("tr_TR");
					break;
				case LANG_IT:
					nc->SetLanguage("it_IT");
					break;
				case LANG_DE:
					nc->SetLanguage("de_DE");
					break;
				case LANG_CAT:
					nc->SetLanguage("ca_ES"); // yes, iso639 defines catalan as CA
					break;
				case LANG_GR:
					nc->SetLanguage("el_GR");
					break;
				case LANG_NL:
					nc->SetLanguage("nl_NL");
					break;
				case LANG_RU:
					nc->SetLanguage("ru_RU");
					break;
				case LANG_HUN:
					nc->SetLanguage("hu_HU");
					break;
				case LANG_PL:
					nc->SetLanguage("pl_PL");
					break;
				case LANG_EN_US:
				case LANG_JA_JIS:
				case LANG_JA_EUC:
				case LANG_JA_SJIS: // these seem to be unused
				default:
					nc->SetLanguage("en");
			}

			READ(read_uint16(&u16, f));
			for (uint16_t j = 0; j < u16; ++j)
			{
				READ(read_string(buffer, f));

				NickAccess *a = Serialize::New<NickAccess *>();
				if (a)
				{
					a->SetAccount(nc);
					a->SetMask(buffer);
				}
			}

			int16_t i16;
			READ(read_int16(&i16, f));
			READ(read_int16(&i16, f));
			MemoServ::MemoInfo *mi = nc->GetMemos();
			if (mi)
				mi->SetMemoMax(i16);
			for (int16_t j = 0; j < i16; ++j)
			{
				MemoServ::Memo *m = Serialize::New<MemoServ::Memo *>();
				READ(read_uint32(&u32, f));
				uint16_t flags;
				READ(read_uint16(&flags, f));
				int32_t tmp32;
				READ(read_int32(&tmp32, f));
				if (m)
					m->SetTime(tmp32);
				char sbuf[32];
				READ(read_buffer(sbuf, f));
				if (m)
					m->SetSender(sbuf);
				Anope::string text;
				READ(read_string(text, f));
				if (m)
					m->SetText(text);
			}
			READ(read_uint16(&u16, f));
			READ(read_int16(&i16, f));

			Log(LOG_DEBUG) << "Loaded NickServ::Account " << nc->GetDisplay();
		}

	for (int i = 0; i < 1024; ++i)
		for (int c; (c = getc_db(f)) == 1;)
		{
			Anope::string nick, last_usermask, last_realname, last_quit;
			time_t time_registered, last_seen;

			READ(read_string(nick, f));
			READ(read_string(last_usermask, f));
			READ(read_string(last_realname, f));
			READ(read_string(last_quit, f));

			int32_t tmp32;
			READ(read_int32(&tmp32, f));
			time_registered = tmp32;
			READ(read_int32(&tmp32, f));
			last_seen = tmp32;

			uint16_t tmpu16;
			READ(read_uint16(&tmpu16, f));

			Anope::string core;
			READ(read_string(core, f));
			NickServ::Account *nc = NickServ::FindAccount(core);
			if (nc == NULL)
			{
				Log() << "Skipping coreless nick " << nick << " with core " << core;
				continue;
			}

			if (tmpu16 & OLD_NS_VERBOTEN)
			{
				if (nc->GetDisplay().find_first_of("?*") != Anope::string::npos)
				{
					delete nc;
					continue;
				}

				ForbidData *d = Serialize::New<ForbidData *>();
				if (d)
				{
					d->SetMask(nc->GetDisplay());
					d->SetCreator(last_usermask);
					d->SetReason(last_realname);
					d->SetType(FT_NICK);
				}
				
				delete nc;
				continue;
			}

			NickServ::Nick *na = Serialize::New<NickServ::Nick *>();
			na->SetNick(nick);
			na->SetAccount(nc);
			na->SetLastUsermask(last_usermask);
			na->SetLastRealname(last_realname);
			na->SetLastQuit(last_quit);
			na->SetTimeRegistered(time_registered);
			na->SetLastSeen(last_seen);

			if (tmpu16 & OLD_NS_NO_EXPIRE)
				na->SetS<bool>("NS_NO_EXPIRE", true);

			Log(LOG_DEBUG) << "Loaded NickServ::Nick " << na->GetNick();
		}

	close_db(f); /* End of section Ia */
}

static void LoadVHosts()
{
	dbFILE *f = open_db_read("HostServ", "hosts.db", 3);
	if (f == NULL)
		return;

	for (int c; (c = getc_db(f)) == 1;)
	{
		Anope::string nick, ident, host, creator;
		int32_t vtime;

		READ(read_string(nick, f));
		READ(read_string(ident, f));
		READ(read_string(host, f));
		READ(read_string(creator, f));
		READ(read_int32(&vtime, f));

		NickServ::Nick *na = NickServ::FindNick(nick);
		if (na == NULL)
		{
			Log() << "Removing vhost for non-existent nick " << nick;
			continue;
		}

		HostServ::VHost *vhost = Serialize::New<HostServ::VHost *>();
		if (vhost == nullptr)
			continue;

		vhost->SetOwner(na);
		vhost->SetIdent(ident);
		vhost->SetHost(host);
		vhost->SetCreator(creator);
		vhost->SetCreated(vtime);

		na->SetVHost(vhost);

		Log() << "Loaded vhost for " << na->GetNick();
	}

	close_db(f);
}

static void LoadBots()
{
	dbFILE *f = open_db_read("Botserv", "bot.db", 10);
	if (f == NULL)
		return;

	for (int c; (c = getc_db(f)) == 1;)
	{
		Anope::string nick, user, host, real;
		int16_t flags, chancount;
		int32_t created;

		READ(read_string(nick, f));
		READ(read_string(user, f));
		READ(read_string(host, f));
		READ(read_string(real, f));
		READ(read_int16(&flags, f));
		READ(read_int32(&created, f));
		READ(read_int16(&chancount, f));

		ServiceBot *bi = ServiceBot::Find(nick, true);
		if (!bi)
			bi = new ServiceBot(nick, user, host, real);
		bi->bi->SetCreated(created);

		if (flags & OLD_BI_PRIVATE)
			bi->bi->SetOperOnly(true);

		Log(LOG_DEBUG) << "Loaded bot " << bi->nick;
	}

	close_db(f);
}

static void LoadChannels()
{
	ServiceReference<BadWords> badwords;
	ServiceReference<ChanServ::ChanServService> chanserv;
	
	if (!chanserv)
		return;

	ServiceReference<ForbidService> forbid;
	dbFILE *f = open_db_read("ChanServ", "chan.db", 16);
	if (f == NULL)
		return;

	for (int i = 0; i < 256; ++i)
		for (int c; (c = getc_db(f)) == 1;)
		{
			Anope::string buffer;
			char namebuf[64];
			READ(read_buffer(namebuf, f));
			ChanServ::Channel *ci = Serialize::New<ChanServ::Channel *>();
			ci->SetName(namebuf);

			const Anope::string settings[] = { "keeptopic", "peace", "cs_private", "restricted", "cs_secure", "secureops", "securefounder",
				"signkick", "signkick_level", "topiclock", "persist", "noautoop", "cs_keepmodes" };
			for (unsigned j = 0; j < sizeof(settings) / sizeof(Anope::string); ++j)
				ci->UnsetS<bool>(settings[j].upper());

			READ(read_string(buffer, f));
			ci->SetFounder(NickServ::FindAccount(buffer));

			READ(read_string(buffer, f));
			ci->SetSuccessor(NickServ::FindAccount(buffer));

			char pwbuf[32];
			READ(read_buffer(pwbuf, f));

			Anope::string desc;
			READ(read_string(desc, f));
			ci->SetDesc(desc);
			READ(read_string(buffer, f));
			READ(read_string(buffer, f));

			int32_t tmp32;
			READ(read_int32(&tmp32, f));
			ci->SetTimeRegistered(tmp32);

			READ(read_int32(&tmp32, f));
			ci->SetLastUsed(tmp32);

			Anope::string last_topic;
			READ(read_string(last_topic, f));
			ci->SetLastTopic(last_topic);

			READ(read_buffer(pwbuf, f));
			ci->SetLastTopicSetter(pwbuf);

			READ(read_int32(&tmp32, f));
			ci->SetLastTopicTime(tmp32);

			uint32_t tmpu32;
			READ(read_uint32(&tmpu32, f));
			// Temporary flags cleanup
			tmpu32 &= ~0x80000000;
			if (tmpu32 & OLD_CI_KEEPTOPIC)
				ci->SetS<bool>("KEEPTOPIC", true);
			if (tmpu32 & OLD_CI_SECUREOPS)
				ci->SetS<bool>("SECUREOPS", true);
			if (tmpu32 & OLD_CI_PRIVATE)
				ci->SetS<bool>("CS_PRIVATE", true);
			if (tmpu32 & OLD_CI_TOPICLOCK)
				ci->SetS<bool>("TOPICLOCK", true);
			if (tmpu32 & OLD_CI_RESTRICTED)
				ci->SetS<bool>("RESTRICTED", true);
			if (tmpu32 & OLD_CI_PEACE)
				ci->SetS<bool>("PEACE", true);
			if (tmpu32 & OLD_CI_SECURE)
				ci->SetS<bool>("CS_SECURE", true);
			if (tmpu32 & OLD_CI_NO_EXPIRE)
				ci->SetS<bool>("CS_NO_EXPIRE", true);
			if (tmpu32 & OLD_CI_MEMO_HARDMAX)
				ci->SetS<bool>("MEMO_HARDMAX", true);
			if (tmpu32 & OLD_CI_SECUREFOUNDER)
				ci->SetS<bool>("SECUREFOUNDER", true);
			if (tmpu32 & OLD_CI_SIGNKICK)
				ci->SetS<bool>("SIGNKICK", true);
			if (tmpu32 & OLD_CI_SIGNKICK_LEVEL)
				ci->SetS<bool>("SIGNKICK_LEVEL", true);

			Anope::string forbidby, forbidreason;
			READ(read_string(forbidby, f));
			READ(read_string(forbidreason, f));
			if (tmpu32 & OLD_CI_SUSPENDED)
			{
				CSSuspendInfo *si = Serialize::New<CSSuspendInfo *>();
				if (si)
				{
					si->SetChannel(ci);
					si->SetBy(forbidby);
				}
			}
			bool forbid_chan = tmpu32 & OLD_CI_VERBOTEN;

			int16_t tmp16;
			READ(read_int16(&tmp16, f));
			ci->SetBanType(tmp16);

			READ(read_int16(&tmp16, f));
			if (tmp16 > 36)
				tmp16 = 36;
			for (int16_t j = 0; j < tmp16; ++j)
			{
				int16_t level;
				READ(read_int16(&level, f));

				if (level == ChanServ::ACCESS_INVALID)
					level = ChanServ::ACCESS_FOUNDER;

				if (j == 10 && level < 0) // NOJOIN
					ci->UnsetS<bool>("RESTRICTED"); // If CSDefRestricted was enabled this can happen

				ci->SetLevel(GetLevelName(j), level);
			}

			bool xop = tmpu32 & OLD_CI_XOP;
			uint16_t tmpu16;
			READ(read_uint16(&tmpu16, f));
			for (uint16_t j = 0; j < tmpu16; ++j)
			{
				uint16_t in_use;
				READ(read_uint16(&in_use, f));
				if (in_use)
				{
					ChanServ::ChanAccess *access = NULL;

					if (xop)
					{
						access = Serialize::New<XOPChanAccess *>();
					}
					else
					{
						access = Serialize::New<AccessChanAccess *>();
					}

					if (access)
						access->SetChannel(ci);

					int16_t level;
					READ(read_int16(&level, f));
					if (access)
					{
						if (xop)
						{
							switch (level)
							{
								case 3:
									access->AccessUnserialize("VOP");
									break;
								case 4:
									access->AccessUnserialize("HOP");
									break;
								case 5:
									access->AccessUnserialize("AOP");
									break;
								case 10:
									access->AccessUnserialize("SOP");
									break;
							}
						}
						else
							access->AccessUnserialize(stringify(level));
					}

					Anope::string mask;
					READ(read_string(mask, f));
					if (access)
					{
						access->SetMask(mask);
						NickServ::Nick *na = NickServ::FindNick(mask);
						if (na)
							na->SetAccount(na->GetAccount());
					}

					READ(read_int32(&tmp32, f));
					if (access)
					{
						access->SetLastSeen(tmp32);
						access->SetCreator("Unknown");
						access->SetCreated(Anope::CurTime);
					}
				}
			}

			READ(read_uint16(&tmpu16, f));
			for (uint16_t j = 0; j < tmpu16; ++j)
			{
				uint16_t flags;
				READ(read_uint16(&flags, f));
				if (flags & 0x0001)
				{
					Anope::string mask, reason, creator;
					READ(read_string(mask, f));
					READ(read_string(reason, f));
					READ(read_string(creator, f));
					READ(read_int32(&tmp32, f));

					ci->AddAkick(creator, mask, reason, tmp32);
				}
			}

			READ(read_uint32(&tmpu32, f)); // mlock on
			ci->Extend<uint32_t>("mlock_on", tmpu32);
			READ(read_uint32(&tmpu32, f)); // mlock off
			ci->Extend<uint32_t>("mlock_off", tmpu32);
			READ(read_uint32(&tmpu32, f)); // mlock limit
			ci->Extend<uint32_t>("mlock_limit", tmpu32);
			READ(read_string(buffer, f)); // key
			ci->Extend<Anope::string>("mlock_key", buffer);
			READ(read_string(buffer, f)); // +f
			READ(read_string(buffer, f)); // +L

			READ(read_int16(&tmp16, f));
			READ(read_int16(&tmp16, f));
			MemoServ::MemoInfo *mi = ci->GetMemos();
			if (mi)
				mi->SetMemoMax(tmp16);
			for (int16_t j = 0; j < tmp16; ++j)
			{
				READ(read_uint32(&tmpu32, f));
				READ(read_uint16(&tmpu16, f));
				MemoServ::Memo *m = Serialize::New<MemoServ::Memo *>();
				READ(read_int32(&tmp32, f));
				if (m)
					m->SetTime(tmp32);
				char sbuf[32];
				READ(read_buffer(sbuf, f));
				if (m)
					m->SetSender(sbuf);
				Anope::string text;
				READ(read_string(text, f));
				if (m)
					m->SetText(text);
			}

			READ(read_string(buffer, f));
			if (!buffer.empty())
			{
				EntryMsg *e = Serialize::New<EntryMsg *>();
				if (e)
				{
					e->SetChannel(ci);
					e->SetCreator("Unknown");
					e->SetMessage(buffer);
					e->SetWhen(Anope::CurTime);
				}
			}

			READ(read_string(buffer, f));
			ci->SetBot(ServiceBot::Find(buffer, true));

			READ(read_int32(&tmp32, f));
			if (tmp32 & OLD_BS_DONTKICKOPS)
				ci->SetS<bool>("BS_DONTKICKOPS", true);
			if (tmp32 & OLD_BS_DONTKICKVOICES)
				ci->SetS<bool>("BS_DONTKICKVOICES", true);
			if (tmp32 & OLD_BS_FANTASY)
				ci->SetS<bool>("BS_FANTASY", true);
			if (tmp32 & OLD_BS_GREET)
				ci->SetS<bool>("BS_GREET", true);
			if (tmp32 & OLD_BS_NOBOT)
				ci->SetS<bool>("BS_NOBOT", true);

			KickerData *kd = GetKickerData(ci);
			if (kd)
			{
				kd->SetBolds(tmp32 & OLD_BS_KICK_BOLDS);
				kd->SetColors(tmp32 & OLD_BS_KICK_COLORS);
				kd->SetReverses(tmp32 & OLD_BS_KICK_REVERSES);
				kd->SetUnderlines(tmp32 & OLD_BS_KICK_UNDERLINES);
				kd->SetBadwords(tmp32 & OLD_BS_KICK_BADWORDS);
				kd->SetCaps(tmp32 & OLD_BS_KICK_CAPS);
				kd->SetFlood(tmp32 & OLD_BS_KICK_FLOOD);
				kd->SetRepeat(tmp32 & OLD_BS_KICK_REPEAT);
			}

			READ(read_int16(&tmp16, f));
			for (int16_t j = 0; j < tmp16; ++j)
			{
				int16_t ttb;
				READ(read_int16(&ttb, f));
				switch (j)
				{
					case TTB_BOLDS:
						kd->SetTTBBolds(ttb);
						break;
					case TTB_COLORS:
						kd->SetTTBColors(ttb);
						break;
					case TTB_REVERSES:
						kd->SetTTBReverses(ttb);
						break;
					case TTB_UNDERLINES:
						kd->SetTTBUnderlines(ttb);
						break;
					case TTB_BADWORDS:
						kd->SetTTBBadwords(ttb);
						break;
					case TTB_CAPS:
						kd->SetTTBCaps(ttb);
						break;
					case TTB_FLOOD:
						kd->SetTTBFlood(ttb);
						break;
					case TTB_REPEAT:
						kd->SetTTBRepeat(ttb);
						break;
				}
			}

			READ(read_int16(&tmp16, f));
			if (kd)
				kd->SetCapsMin(tmp16);
			READ(read_int16(&tmp16, f));
			if (kd)
				kd->SetCapsPercent(tmp16);
			READ(read_int16(&tmp16, f));
			if (kd)
				kd->SetFloodLines(tmp16);
			READ(read_int16(&tmp16, f));
			if (kd)
				kd->SetFloodSecs(tmp16);
			READ(read_int16(&tmp16, f));
			if (kd)
				kd->SetRepeatTimes(tmp16);

			READ(read_uint16(&tmpu16, f));
			for (uint16_t j = 0; j < tmpu16; ++j)
			{
				uint16_t in_use;
				READ(read_uint16(&in_use, f));
				if (in_use)
				{
					READ(read_string(buffer, f));
					uint16_t type;
					READ(read_uint16(&type, f));

					BadWordType bwtype = BW_ANY;
					if (type == 1)
						bwtype = BW_SINGLE;
					else if (type == 2)
						bwtype = BW_START;
					else if (type == 3)
						bwtype = BW_END;

					if (badwords)
						badwords->AddBadWord(ci, buffer, bwtype);
				}
			}

			if (forbid_chan)
			{
				if (ci->GetName().find_first_of("?*") != Anope::string::npos)
				{
					delete ci;
					continue;
				}

				ForbidData *d = Serialize::New<ForbidData *>();
				if (d)
				{
					d->SetMask(ci->GetName());
					d->SetCreator(forbidby);
					d->SetReason(forbidreason);
					d->SetType(FT_CHAN);
				}
				
				delete ci;
				continue;
			}

			Log(LOG_DEBUG) << "Loaded channel " << ci->GetName();
		}

	close_db(f);
}

static void LoadOper()
{
	dbFILE *f = open_db_read("OperServ", "oper.db", 13);
	if (f == NULL)
		return;

	XLineManager *akill, *sqline, *snline, *szline;
	akill = sqline = snline = szline = NULL;

	for (XLineManager *xl : XLineManager::XLineManagers)
	{
		if (xl->Type() == 'G')
			akill = xl;
		else if (xl->Type() == 'Q')
			sqline = xl;
		else if (xl->Type() == 'N')
			snline = xl;
		else if (xl->Type() == 'Z')
			szline = xl;
	}

	int32_t tmp32;
	READ(read_int32(&tmp32, f));
	READ(read_int32(&tmp32, f));

	int16_t capacity;
	read_int16(&capacity, f); // AKill count
	for (int16_t i = 0; i < capacity; ++i)
	{
		Anope::string user, host, by, reason;
		int32_t seton, expires;

		READ(read_string(user, f));
		READ(read_string(host, f));
		READ(read_string(by, f));
		READ(read_string(reason, f));
		READ(read_int32(&seton, f));
		READ(read_int32(&expires, f));

		if (!akill)
			continue;

		XLine *x = Serialize::New<XLine *>();
		x->SetMask(user + "@" + host);
		x->SetBy(by);
		x->SetExpires(expires);
		x->SetReason(reason);
		x->SetID(XLineManager::GenerateUID());
		x->SetCreated(seton);

		akill->AddXLine(x);
	}

	read_int16(&capacity, f); // SNLines
	for (int16_t i = 0; i < capacity; ++i)
	{
		Anope::string mask, by, reason;
		int32_t seton, expires;

		READ(read_string(mask, f));
		READ(read_string(by, f));
		READ(read_string(reason, f));
		READ(read_int32(&seton, f));
		READ(read_int32(&expires, f));

		if (!snline)
			continue;

		XLine *x = Serialize::New<XLine *>();
		x->SetMask(mask);
		x->SetBy(by);
		x->SetExpires(expires);
		x->SetReason(reason);
		x->SetID(XLineManager::GenerateUID());
		x->SetCreated(seton);

		snline->AddXLine(x);
	}

	read_int16(&capacity, f); // SQLines
	for (int16_t i = 0; i < capacity; ++i)
	{
		Anope::string mask, by, reason;
		int32_t seton, expires;

		READ(read_string(mask, f));
		READ(read_string(by, f));
		READ(read_string(reason, f));
		READ(read_int32(&seton, f));
		READ(read_int32(&expires, f));

		if (!sqline)
			continue;

		XLine *x = Serialize::New<XLine *>();
		x->SetMask(mask);
		x->SetBy(by);
		x->SetExpires(expires);
		x->SetReason(reason);
		x->SetID(XLineManager::GenerateUID());
		x->SetCreated(seton);

		sqline->AddXLine(x);
	}

	read_int16(&capacity, f); // SZLines
	for (int16_t i = 0; i < capacity; ++i)
	{
		Anope::string mask, by, reason;
		int32_t seton, expires;

		READ(read_string(mask, f));
		READ(read_string(by, f));
		READ(read_string(reason, f));
		READ(read_int32(&seton, f));
		READ(read_int32(&expires, f));

		if (!szline)
			continue;

		XLine *x = Serialize::New<XLine *>();
		x->SetMask(mask);
		x->SetBy(by);
		x->SetExpires(expires);
		x->SetReason(reason);
		x->SetID(XLineManager::GenerateUID());
		x->SetCreated(seton);

		szline->AddXLine(x);
	}

	close_db(f);
}

static void LoadExceptions()
{
	dbFILE *f = open_db_read("OperServ", "exception.db", 9);
	if (f == NULL)
		return;

	int16_t num;
	READ(read_int16(&num, f));
	for (int i = 0; i < num; ++i)
	{
		Anope::string mask, reason;
		int16_t limit;
		char who[32];
		int32_t time, expires;

		READ(read_string(mask, f));
		READ(read_int16(&limit, f));
		READ(read_buffer(who, f));
		READ(read_string(reason, f));
		READ(read_int32(&time, f));
		READ(read_int32(&expires, f));

		Exception *e = Serialize::New<Exception *>();
		if (e)
		{
			e->SetMask(mask);
			e->SetLimit(limit);
			e->SetWho(who);
			e->SetTime(time);
			e->SetExpires(expires);
			e->SetReason(reason);
		}
	}

	close_db(f);
}

static void LoadNews()
{
	dbFILE *f = open_db_read("OperServ", "news.db", 9);

	if (f == NULL)
		return;

	int16_t n;
	READ(read_int16(&n, f));

	for (int16_t i = 0; i < n; i++)
	{
		int16_t type;
		NewsItem *ni = Serialize::New<NewsItem *>();
		
		if (!ni)
			break;

		READ(read_int16(&type, f));

		switch (type)
		{
			case OLD_NEWS_LOGON:
				ni->SetNewsType(NEWS_LOGON);
				break;
			case OLD_NEWS_OPER:
				ni->SetNewsType(NEWS_OPER);
				break;
			case OLD_NEWS_RANDOM:
				ni->SetNewsType(NEWS_RANDOM);
				break;
		}

		int32_t unused;
		READ(read_int32(&unused, f));

		Anope::string text;
		READ(read_string(text, f));
		ni->SetText(text);

		char who[32];
		READ(read_buffer(who, f));
		ni->SetWho(who);

		int32_t tmp;
		READ(read_int32(&tmp, f));
		ni->SetTime(tmp);
	}

	close_db(f);
}

class DBOld : public Module
	, public EventHook<Event::LoadDatabase>
	, public EventHook<Event::UplinkSync>
{
	ExtensibleItem<uint32_t> mlock_on, mlock_off, mlock_limit; // XXX these are no longer required because of confmodes
	ExtensibleItem<Anope::string> mlock_key;

 public:
	DBOld(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE | VENDOR)
		, EventHook<Event::LoadDatabase>(this)
		, EventHook<Event::UplinkSync>(this)
		, mlock_on(this, "mlock_on")
		, mlock_off(this, "mlock_off")
		, mlock_limit(this, "mlock_limit")
		, mlock_key(this, "mlock_key")
	{
		hashm = Config->GetModule(this)->Get<Anope::string>("hash");

		if (hashm != "md5" && hashm != "oldmd5" && hashm != "sha1" && hashm != "plain" && hashm != "sha256")
			throw ModuleException("Invalid hash method");
	}

	EventReturn OnLoadDatabase() override
	{
		LoadNicks();
		LoadVHosts();
		LoadBots();
		LoadChannels();
		LoadOper();
		LoadExceptions();
		LoadNews();

		return EVENT_STOP;
	}

	void OnUplinkSync(Server *s) override
	{
		if (!ChanServ::service)
			return;
		for (auto& it : ChanServ::service->GetChannels())
		{
			ChanServ::Channel *ci = it.second;
			uint32_t *limit = mlock_limit.Get(ci);
			Anope::string *key = mlock_key.Get(ci);

			uint32_t *u = mlock_on.Get(ci);
			if (u)
			{
				process_mlock(ci, *u, true, limit, key);
				mlock_on.Unset(ci);
			}

			u = mlock_off.Get(ci);
			if (u)
			{
				process_mlock(ci, *u, false, limit, key);
				mlock_off.Unset(ci);
			}

			mlock_limit.Unset(ci);
			mlock_key.Unset(ci);

			if (ci->c)
				ci->c->CheckModes();
		}
	}
};

template<> void ModuleInfo<DBOld>(ModuleDef *def)
{
	def->Depends("chanserv.access");
}

MODULE_INIT(DBOld)

