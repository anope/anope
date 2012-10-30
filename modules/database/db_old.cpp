/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

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
#define read_buffer(buf, f) (read_db((f), (buf), sizeof(buf)) == sizeof(buf))

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

#define OLD_CI_KEEPTOPIC		0x00000001
#define OLD_CI_SECUREOPS		0x00000002
#define OLD_CI_PRIVATE			0x00000004
#define OLD_CI_TOPICLOCK		0x00000008
#define OLD_CI_RESTRICTED		0x00000010
#define OLD_CI_PEACE			0x00000020
#define OLD_CI_SECURE			0x00000040
#define OLD_CI_FORBIDDEN		0x00000080
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

static void process_mlock(ChannelInfo *ci, uint32_t lock, bool status)
{
	for (unsigned i = 0; i < (sizeof(mlock_infos) / sizeof(mlock_info)); ++i)
		if (lock & mlock_infos[i].m)
		{
			ChannelMode *cm = ModeManager::FindChannelModeByChar(mlock_infos[i].c);
			if (cm)
				ci->SetMLock(cm, status);
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
			return "OPDEOP";
		case 8:
			return "LIST";
		case 9:
			return "CLEAR";
		case 10:
			return "NOJOIN";
		case 11:
			return "CHANGE";
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
			return "OPDEOPME";
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
	strscpy(f->filename, (db_dir + "/" + filename).c_str(), sizeof(f->filename));
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
	dbFILE *f = open_db_read("NickServ", "nick.db", 14);
	if (f == NULL)
		return;
	for (int i = 0; i < 1024; ++i)
		for (int c; (c = getc_db(f)) == 1;)
		{
			Anope::string buffer;

			READ(read_string(buffer, f));
			NickCore *nc = new NickCore(buffer);

			char pwbuf[32];
			READ(read_buffer(pwbuf, f));
			if (hashm == "plain")
				my_b64_encode(pwbuf, nc->pass);
			else if (hashm == "md5" || hashm == "oldmd5")
				nc->pass = Hex(pwbuf, 16);
			else if (hashm == "sha1")
				nc->pass = Hex(pwbuf, 20);
			else
				nc->pass = Hex(pwbuf, strlen(pwbuf));
			nc->pass = hashm + ":" + nc->pass;

			READ(read_string(buffer, f));
			nc->email = buffer;

			READ(read_string(buffer, f));
			nc->greet = buffer;

			uint32_t uint;
			READ(read_uint32(&uint, f));
			//nc->icq = uint;

			READ(read_string(buffer, f));
			//nc->url = buffer;

			READ(read_uint32(&uint, f));
			if (uint & OLD_NI_KILLPROTECT)
				nc->SetFlag(NI_KILLPROTECT);
			if (uint & OLD_NI_SECURE)
				nc->SetFlag(NI_SECURE);
			if (uint & OLD_NI_MSG)
				nc->SetFlag(NI_MSG);
			if (uint & OLD_NI_MEMO_HARDMAX)
				nc->SetFlag(NI_MEMO_HARDMAX);
			if (uint & OLD_NI_MEMO_SIGNON)
				nc->SetFlag(NI_MEMO_SIGNON);
			if (uint & OLD_NI_MEMO_RECEIVE)
				nc->SetFlag(NI_MEMO_RECEIVE);
			if (uint & OLD_NI_PRIVATE)
				nc->SetFlag(NI_PRIVATE);
			if (uint & OLD_NI_HIDE_EMAIL)
				nc->SetFlag(NI_HIDE_EMAIL);
			if (uint & OLD_NI_HIDE_MASK)
				nc->SetFlag(NI_HIDE_MASK);
			if (uint & OLD_NI_HIDE_QUIT)
				nc->SetFlag(NI_HIDE_QUIT);
			if (uint & OLD_NI_KILL_QUICK)
				nc->SetFlag(NI_KILL_QUICK);
			if (uint & OLD_NI_KILL_IMMED)
				nc->SetFlag(NI_KILL_IMMED);
			if (uint & OLD_NI_MEMO_MAIL)
				nc->SetFlag(NI_MEMO_MAIL);
			if (uint & OLD_NI_HIDE_STATUS)
				nc->SetFlag(NI_HIDE_STATUS);
			if (uint & OLD_NI_SUSPENDED)
				nc->SetFlag(NI_SUSPENDED);
			if (!(uint & OLD_NI_AUTOOP))
				nc->SetFlag(NI_AUTOOP);

			uint16_t u16;
			READ(read_uint16(&u16, f));
			switch (u16)
			{
				case LANG_ES:
					nc->language = "es_ES";
					break;
				case LANG_PT:
					nc->language = "pt_PT";
					break;
				case LANG_FR:
					nc->language = "fr_FR";
					break;
				case LANG_TR:
					nc->language = "tr_TR";
					break;
				case LANG_IT:
					nc->language = "it_IT";
					break;
				case LANG_DE:
					nc->language = "de_DE";
					break;
				case LANG_CAT:
					nc->language = "ca_ES"; // yes, iso639 defines catalan as CA
					break;
				case LANG_GR:
					nc->language = "el_GR";
					break;
				case LANG_NL:
					nc->language = "nl_NL";
					break;
				case LANG_RU:
					nc->language = "ru_RU";
					break;
				case LANG_HUN:
					nc->language = "hu_HU";
					break;
				case LANG_PL:
					nc->language = "pl_PL";
					break;
				case LANG_EN_US:
				case LANG_JA_JIS:
				case LANG_JA_EUC:
				case LANG_JA_SJIS: // these seem to be unused
				default:
					nc->language = "en_US";
			}

			READ(read_uint16(&u16, f));
			for (uint16_t j = 0; j < u16; ++j)
			{
				READ(read_string(buffer, f));
				nc->access.push_back(buffer);
			}

			int16_t i16;
			READ(read_int16(&i16, f));
			READ(read_int16(&nc->memos.memomax, f));
			for (int16_t j = 0; j < i16; ++j)
			{
				Memo *m = new Memo;
				READ(read_uint32(&uint, f));
				uint16_t flags;
				READ(read_uint16(&flags, f));
				int32_t tmp32;
				READ(read_int32(&tmp32, f));
				m->time = tmp32;
				char sbuf[32];
				READ(read_buffer(sbuf, f));
				m->sender = sbuf;
				READ(read_string(m->text, f));
				m->owner = nc->display;
				nc->memos.memos->push_back(m);
			}
			READ(read_uint16(&u16, f));
			READ(read_int16(&i16, f));

			Log(LOG_DEBUG) << "Loaded NickCore " << nc->display;
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
			NickCore *nc = findcore(core);
			if (nc == NULL)
			{
				Log() << "Skipping coreless nick " << nick << " with core " << core;
				continue;
			}
			NickAlias *na = new NickAlias(nick, nc);
			na->last_usermask = last_usermask;
			na->last_realname = last_realname;
			na->last_quit = last_quit;
			na->time_registered = time_registered;
			na->last_seen = last_seen;

			if (tmpu16 & OLD_NS_NO_EXPIRE)
				na->SetFlag(NS_NO_EXPIRE);

			Log(LOG_DEBUG) << "Loaded NickAlias " << na->nick;
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

		NickAlias *na = findnick(nick);
		if (na == NULL)
		{
			Log() << "Removing vhost for nonexistant nick " << nick;
			continue;
		}

		na->SetVhost(ident, host, creator, vtime);

		Log() << "Loaded vhost for " << na->nick;
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

		BotInfo *bi = findbot(nick);
		if (!bi)
			bi = new BotInfo(nick, user, host, real);
		bi->created = created;

		if (flags & OLD_BI_PRIVATE)
			bi->SetFlag(BI_PRIVATE);

		Log(LOG_DEBUG) << "Loaded bot " << bi->nick;
	}

	close_db(f);
}

static void LoadChannels()
{
	dbFILE *f = open_db_read("ChanServ", "chan.db", 16);
	if (f == NULL)
		return;

	for (int i = 0; i < 256; ++i)
		for (int c; (c = getc_db(f)) == 1;)
		{
			Anope::string buffer;
			char namebuf[64];
			READ(read_buffer(namebuf, f));
			ChannelInfo *ci = new ChannelInfo(namebuf);

			READ(read_string(buffer, f));
			ci->SetFounder(findcore(buffer));

			READ(read_string(buffer, f));
			ci->successor = findcore(buffer);

			char pwbuf[32];
			READ(read_buffer(pwbuf, f));

			READ(read_string(ci->desc, f));
			READ(read_string(buffer, f));
			READ(read_string(buffer, f));

			int32_t tmp32;
			READ(read_int32(&tmp32, f));
			ci->time_registered = tmp32;

			READ(read_int32(&tmp32, f));
			ci->last_used = tmp32;

			READ(read_string(ci->last_topic, f));

			READ(read_buffer(pwbuf, f));
			ci->last_topic_setter = pwbuf;

			READ(read_int32(&tmp32, f));
			ci->last_topic_time = tmp32;

			uint32_t tmpu32;
			READ(read_uint32(&tmpu32, f));
			// Temporary flags cleanup
			tmpu32 &= ~0x80000000;
			if (tmpu32 & OLD_CI_KEEPTOPIC)
				ci->SetFlag(CI_KEEPTOPIC);
			if (tmpu32 & OLD_CI_SECUREOPS)
				ci->SetFlag(CI_SECUREOPS);
			if (tmpu32 & OLD_CI_PRIVATE)
				ci->SetFlag(CI_PRIVATE);
			if (tmpu32 & OLD_CI_TOPICLOCK)
				ci->SetFlag(CI_TOPICLOCK);
			if (tmpu32 & OLD_CI_RESTRICTED)
				ci->SetFlag(CI_RESTRICTED);
			if (tmpu32 & OLD_CI_PEACE)
				ci->SetFlag(CI_PEACE);
			if (tmpu32 & OLD_CI_SECURE)
				ci->SetFlag(CI_SECURE);
			if (tmpu32 & OLD_CI_NO_EXPIRE)
				ci->SetFlag(CI_NO_EXPIRE);
			if (tmpu32 & OLD_CI_MEMO_HARDMAX)
				ci->SetFlag(CI_MEMO_HARDMAX);
			if (tmpu32 & OLD_CI_SECUREFOUNDER)
				ci->SetFlag(CI_SECUREFOUNDER);
			if (tmpu32 & OLD_CI_SIGNKICK)
				ci->SetFlag(CI_SIGNKICK);
			if (tmpu32 & OLD_CI_SIGNKICK_LEVEL)
				ci->SetFlag(CI_SIGNKICK_LEVEL);
			if (tmpu32 & OLD_CI_SUSPENDED)
				ci->SetFlag(CI_SUSPENDED);

			READ(read_string(buffer, f));
			READ(read_string(buffer, f));

			int16_t tmp16;
			READ(read_int16(&tmp16, f));
			ci->bantype = tmp16;

			READ(read_int16(&tmp16, f));
			if (tmp16 > 36)
				tmp16 = 36;
			for (int16_t j = 0; j < tmp16; ++j)
			{
				int16_t level;
				READ(read_int16(&level, f));

				ci->SetLevel(GetLevelName(j), level);
			}

			service_reference<AccessProvider> provider("AccessProvider", "access/access");
			uint16_t tmpu16;
			READ(read_uint16(&tmpu16, f));
			for (uint16_t j = 0; j < tmpu16; ++j)
			{
				uint16_t in_use;
				READ(read_uint16(&in_use, f));
				if (in_use)
				{
					ChanAccess *access = provider ? provider->Create() : NULL;
					if (access)
						access->ci = ci;

					int16_t level;
					READ(read_int16(&level, f));
					if (access)
						access->Unserialize(stringify(level));

					Anope::string mask;
					READ(read_string(mask, f));
					if (access)
						access->mask = mask;

					READ(read_int32(&tmp32, f));
					if (access)
					{
						access->last_seen = tmp32;
						access->creator = "Unknown";
						access->created = Anope::CurTime;

						ci->AddAccess(access);
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
			process_mlock(ci, tmpu32, true);
			READ(read_uint32(&tmpu32, f)); // mlock off
			process_mlock(ci, tmpu32, false);
			READ(read_uint32(&tmpu32, f)); // mlock limit
			READ(read_string(buffer, f));
			READ(read_string(buffer, f));
			READ(read_string(buffer, f));

			READ(read_int16(&tmp16, f));
			READ(read_int16(&ci->memos.memomax, f));
			for (int16_t j = 0; j < tmp16; ++j)
			{
				READ(read_uint32(&tmpu32, f));
				READ(read_uint16(&tmpu16, f));
				Memo *m = new Memo;
				READ(read_int32(&tmp32, f));
				m->time = tmp32;
				char sbuf[32];
				READ(read_buffer(sbuf, f));
				m->sender = sbuf;
				READ(read_string(m->text, f));
				m->owner = ci->name;
				ci->memos.memos->push_back(m);
			}

			READ(read_string(buffer, f));

			READ(read_string(buffer, f));
			ci->bi = findbot(buffer);

			READ(read_int32(&tmp32, f));
			if (tmp32 & OLD_BS_DONTKICKOPS)
				ci->botflags.SetFlag(BS_DONTKICKOPS);
			if (tmp32 & OLD_BS_DONTKICKVOICES)
				ci->botflags.SetFlag(BS_DONTKICKVOICES);
			if (tmp32 & OLD_BS_FANTASY)
				ci->botflags.SetFlag(BS_FANTASY);
			if (tmp32 & OLD_BS_GREET)
				ci->botflags.SetFlag(BS_GREET);
			if (tmp32 & OLD_BS_NOBOT)
				ci->botflags.SetFlag(BS_NOBOT);
			if (tmp32 & OLD_BS_KICK_BOLDS)
				ci->botflags.SetFlag(BS_KICK_BOLDS);
			if (tmp32 & OLD_BS_KICK_COLORS)
				ci->botflags.SetFlag(BS_KICK_COLORS);
			if (tmp32 & OLD_BS_KICK_REVERSES)
				ci->botflags.SetFlag(BS_KICK_REVERSES);
			if (tmp32 & OLD_BS_KICK_UNDERLINES)
				ci->botflags.SetFlag(BS_KICK_UNDERLINES);
			if (tmp32 & OLD_BS_KICK_BADWORDS)
				ci->botflags.SetFlag(BS_KICK_BADWORDS);
			if (tmp32 & OLD_BS_KICK_CAPS)
				ci->botflags.SetFlag(BS_KICK_CAPS);
			if (tmp32 & OLD_BS_KICK_FLOOD)
				ci->botflags.SetFlag(BS_KICK_FLOOD);
			if (tmp32 & OLD_BS_KICK_REPEAT)
				ci->botflags.SetFlag(BS_KICK_REPEAT);

			READ(read_int16(&tmp16, f));
			for (int16_t j = 0; j < tmp16; ++j)
			{
				int16_t ttb;
				READ(read_int16(&ttb, f));
				if (j < TTB_SIZE)
					ci->ttb[j] = ttb;
			}

			READ(read_int16(&tmp16, f));
			ci->capsmin = tmp16;
			READ(read_int16(&tmp16, f));
			ci->capspercent = tmp16;
			READ(read_int16(&tmp16, f));
			ci->floodlines = tmp16;
			READ(read_int16(&tmp16, f));
			ci->floodsecs = tmp16;
			READ(read_int16(&tmp16, f));
			ci->repeattimes = tmp16;

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

					ci->AddBadWord(buffer, bwtype);
				}
			}

			Log(LOG_DEBUG) << "Loaded channel " << ci->name;
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

	for (std::list<XLineManager *>::iterator it = XLineManager::XLineManagers.begin(), it_end = XLineManager::XLineManagers.end(); it != it_end; ++it)
	{
		XLineManager *xl = *it;
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

		XLine *x = new XLine(user + "@" + host, by, expires, reason, XLineManager::GenerateUID());
		x->Created = seton;
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

		XLine *x = new XLine(mask, by, expires, reason, XLineManager::GenerateUID());
		x->Created = seton;
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

		XLine *x = new XLine(mask, by, expires, reason, XLineManager::GenerateUID());
		x->Created = seton;
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

		XLine *x = new XLine(mask, by, expires, reason, XLineManager::GenerateUID());
		x->Created = seton;
		szline->AddXLine(x);
	}

	close_db(f);
}

class DBOld : public Module
{
 public:
	DBOld(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnLoadDatabase };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		ConfigReader conf;
		hashm = conf.ReadValue("db_old", "hash", "", 0);

		if (hashm != "md5" && hashm != "oldmd5" && hashm != "sha1" && hashm != "plain" && hashm != "sha256")
			throw ModuleException("Invalid hash method");
	}

	EventReturn OnLoadDatabase() anope_override
	{
		LoadNicks();
		LoadVHosts();
		LoadBots();
		LoadChannels();
		LoadOper();

		return EVENT_STOP;
	}
};

MODULE_INIT(DBOld)

