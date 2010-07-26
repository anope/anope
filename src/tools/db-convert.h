/*
 *   Copyright (C) 2003-2010 Anope Team <team@anope.org>
 *   Copyright (C) 2005-2006 Florian Schulze <certus@anope.org>
 *   Copyright (C) 2008-2010 Robin Burchell <w00t@inspircd.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License (see it online
 *   at http://www.gnu.org/copyleft/gpl.html) as published by the Free
 *   Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 */

#ifndef DB_CONVERT_H
#define DB_CONVERT_H

#include <string>
#include <iostream>
#include <fstream>

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <fcntl.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <io.h>
#endif
#include "sysconf.h"

#ifndef _WIN32
#define C_LBLUE "\033[1;34m"
#define C_NONE "\033[m"
#else
#define C_LBLUE ""
#define C_NONE ""
#endif

#define getc_db(f) (fgetc((f)->fp))
#define HASH(nick) ((tolower((nick)[0]) & 31)<<5 | (tolower((nick)[1]) & 31))
#define HASH2(chan) ((chan)[1] ? ((chan)[1] & 31)<<5 | ((chan)[2] & 31) : 0)
#define read_buffer(buf, f) (read_db((f), (buf), sizeof(buf)) == sizeof(buf))
#define write_buffer(buf, f) (write_db((f), (buf), sizeof(buf)) == sizeof(buf))
#define read_db(f, buf, len) (fread((buf), 1, (len), (f)->fp))
#define write_db(f, buf, len) (fwrite((buf), 1, (len), (f)->fp))
#define read_int8(ret, f) ((*(ret) = fgetc((f)->fp)) == EOF ? -1 : 0)
#define write_int8(val, f) (fputc((val), (f)->fp) == EOF ? -1 : 0)
#define SAFE(x) \
if (true) \
{ \
	if ((x) < 0) \
		printf("Error, the database is broken, trying to continue... no guarantee.\n"); \
} \
else \
	static_cast<void>(0)
#define READ(x) \
if (true) \
{ \
	if ((x) < 0) \
	{ \
		printf("Error, the database is broken, trying to continue... no guarantee.\n"); \
		exit(0); \
	} \
} \
else \
	static_cast<void>(0)

struct Memo
{
	uint32 number;		/* Index number -- not necessarily array position! */
	uint16 flags;		/* Flags */
	time_t time;		/* When was it sent? */
	char sender[32];	/* Name of the sender */
	char *text;
};

struct dbFILE
{
	int mode;				/* 'r' for reading, 'w' for writing */
	FILE *fp;				/* The normal file descriptor */
	char filename[1024];	/* Name of the database file */
};

struct MemoInfo
{
	int16 memocount;	/* Current # of memos */
	int16 memomax;		/* Max # of memos one can hold*/
	Memo *memos;		/* Pointer to original memos */
};

struct NickCore
{
	NickCore *next, *prev;

	char *display;			/* How the nick is displayed */
	char pass[32];			/* Password of the nicks */
	char *email;			/* E-mail associated to the nick */
	char *greet;			/* Greet associated to the nick */
	uint32 icq;				/* ICQ # associated to the nick */
	char *url;				/* URL associated to the nick */
	uint32 flags;			/* See NI_* below */
	uint16 language;		/* Language selected by nickname owner (LANG_*) */
	uint16 accesscount;		/* # of entries */
	char **access;			/* Array of strings */
	MemoInfo memos;			/* Memo information */
	uint16 channelcount;	/* Number of channels currently registered */
	int unused;				/* Used for nick collisions */
	int aliascount;			/* How many aliases link to us? Remove the core if 0 */
};

struct NickAlias
{
	NickAlias *next, *prev;
	char *nick;				/* Nickname */
	time_t time_registered;	/* When the nick was registered */
	time_t last_seen;		/* When it was seen online for the last time */
	uint16 status;			/* See NS_* below */
	NickCore *nc;			/* I'm an alias of this */

	char *last_usermask;
	char *last_realname;
	char *last_quit;
};

struct ChanAccess
{
	uint16 in_use;	/* 1 if this entry is in use, else 0 */
	int16 level;
	NickCore *nc;	/* Guaranteed to be non-NULL if in use, NULL if not */
	time_t last_seen;
};

struct AutoKick
{
	int16 in_use;		/* Always 0 if not in use */
	int16 is_nick;		/* 1 if a regged nickname, 0 if a nick!user@host mask */
	uint16 flags;
	union
	{
		char *mask;		/* Guaranteed to be non-NULL if in use, NULL if not */
		NickCore *nc;	/* Same */
	} u;
	char *reason;
	char *creator;
	time_t addtime;
};

struct BadWord
{
	uint16 in_use;
	char *word;
	uint16 type;
};

struct ChannelInfo
{
	ChannelInfo *next, *prev;

	char name[64];					/* Channel name */
	char *founder;					/* Who registered the channel */
	char *successor;				/* Who gets the channel if the founder nick is dropped or expires */
	char founderpass[32];			/* Channel password */
	char *desc;						/* Description */
	char *url;						/* URL */
	char *email;					/* Email address */
	time_t time_registered;			/* When was it registered */
	time_t last_used;				/* When was it used hte last time */
	char *last_topic;				/* Last topic on the channel */
	char last_topic_setter[32];		/* Who set the last topic */
	time_t last_topic_time;			/* When the last topic was set */
	uint32 flags;					/* Flags */
	char *forbidby;					/* if forbidden: who did it */
	char *forbidreason;				/* if forbidden: why */
	int16 bantype;					/* Bantype */
	int16 *levels;					/* Access levels for commands */
	uint16 accesscount;				/* # of pple with access */
	ChanAccess *access;				/* List of authorized users */
	uint16 akickcount;				/* # of akicked pple */
	AutoKick *akick;				/* List of users to kickban */
	uint32 mlock_on, mlock_off;		/* See channel modes below */
	uint32 mlock_limit;				/* 0 if no limit */
	char *mlock_key;				/* NULL if no key */
	char *mlock_flood;				/* NULL if no +f */
	char *mlock_redirect;			/* NULL if no +L */
	char *entry_message;			/* Notice sent on entering channel */
	MemoInfo memos;					/* Memos */
	char *bi;						/* Bot used on this channel */
	uint32 botflags;				/* BS_* below */
	int16 *ttb;						/* Times to ban for each kicker */
	uint16 bwcount;					/* Badword count */
	BadWord *badwords;				/* For BADWORDS kicker */
	int16 capsmin, capspercent;		/* For CAPS kicker */
	int16 floodlines, floodsecs;	/* For FLOOD kicker */
	int16 repeattimes;				/* For REPEAT kicker */
};

struct HostCore
{
	HostCore *next;
	char *nick;
	char *vIdent;
	char *vHost;
	char *creator;
	int32 time;
};

dbFILE *open_db_read(const char *service, const char *filename, int version);
NickCore *findcore(const char *nick, int version);
NickAlias *findnick(const char *nick);
ChannelInfo *cs_findchan(const char *chan);
char *strscpy(char *d, const char *s, size_t len);
int write_file_version(dbFILE * f, uint32 version);
int mystricmp(const char *s1, const char *s2);
int delnick(NickAlias *na, int donttouchthelist);
int write_string(const char *s, dbFILE * f);
int write_ptr(const void *ptr, dbFILE * f);
int read_int16(int16 * ret, dbFILE * f);
int read_int32(int32 * ret, dbFILE * f);
int read_uint16(uint16 * ret, dbFILE * f);
int read_uint32(uint32 * ret, dbFILE * f);
int read_string(char **ret, dbFILE * f);
int write_int16(uint16 val, dbFILE * f);
int write_int32(uint32 val, dbFILE * f);
int read_ptr(void **ret, dbFILE * f);
int delcore(NickCore *nc);
void alpha_insert_chan(ChannelInfo * ci);
void close_db(dbFILE * f);

ChannelInfo *chanlists[256];
NickAlias *nalists[1024];
NickCore *nclists[1024];
HostCore *head = NULL;

int b64_encode(char *src, size_t srclength, char *target, size_t targsize);

/* Memo Flags */
#define MF_UNREAD	0x0001 /* Memo has not yet been read */
#define MF_RECEIPT	0x0002 /* Sender requested receipt */
#define MF_NOTIFYS	0x0004 /* Memo is a notification of receitp */

/* Nickname status flags: */
#define NS_FORBIDDEN	0x0002 /* Nick may not be registered or used */
#define NS_NO_EXPIRE	0x0004 /* Nick never expires */

/* Nickname setting flags: */
#define NI_KILLPROTECT	0x00000001 /* Kill others who take this nick */
#define NI_SECURE		0x00000002 /* Don't recognize unless IDENTIFY'd */
#define NI_MSG			0x00000004 /* Use PRIVMSGs instead of NOTICEs */
#define NI_MEMO_HARDMAX	0x00000008 /* Don't allow user to change memo limit */
#define NI_MEMO_SIGNON	0x00000010 /* Notify of memos at signon and un-away */
#define NI_MEMO_RECEIVE	0x00000020 /* Notify of new memos when sent */
#define NI_PRIVATE		0x00000040 /* Don't show in LIST to non-servadmins */
#define NI_HIDE_EMAIL	0x00000080 /* Don't show E-mail in INFO */
#define NI_HIDE_MASK	0x00000100 /* Don't show last seen address in INFO */
#define NI_HIDE_QUIT	0x00000200 /* Don't show last quit message in INFO */
#define NI_KILL_QUICK	0x00000400 /* Kill in 20 seconds instead of 60 */
#define NI_KILL_IMMED	0x00000800 /* Kill immediately instead of in 60 sec */
#define NI_ENCRYPTEDPW	0x00004000 /* Nickname password is encrypted */
#define NI_MEMO_MAIL	0x00010000 /* User gets email on memo */
#define NI_HIDE_STATUS	0x00020000 /* Don't show services access status */
#define NI_SUSPENDED	0x00040000 /* Nickname is suspended */
#define NI_AUTOOP		0x00080000 /* Autoop nickname in channels */
#define NI_NOEXPIRE		0x00100000 /* nicks in this group won't expire */

// Old NS_FORBIDDEN, very fucking temporary.
#define NI_FORBIDDEN	0x80000000

/* Retain topic even after last person leaves channel */
#define CI_KEEPTOPIC		0x00000001
/* Don't allow non-authorized users to be opped */
#define CI_SECUREOPS		0x00000002
/* Hide channel from ChanServ LIST command */
#define CI_PRIVATE			0x00000004
/* Topic can only be changed by SET TOPIC */
#define CI_TOPICLOCK		0x00000008
/* Those not allowed ops are kickbanned */
#define CI_RESTRICTED		0x00000010
/* Don't allow ChanServ and BotServ commands to do bad things to bigger levels */
#define CI_PEACE			0x00000020
/* Don't allow any privileges unless a user is IDENTIFY'd with NickServ */
#define CI_SECURE			0x00000040
/* Don't allow the channel to be registered or used */
#define CI_FORBIDDEN		0x00000080
/* Channel password is encrypted */
#define CI_ENCRYPTEDPW		0x00000100
/* Channel does not expire */
#define CI_NO_EXPIRE		0x00000200
/* Channel memo limit may not be changed */
#define CI_MEMO_HARDMAX		0x00000400
/* Send notice to channel on use of OP/DEOP */
#define CI_OPNOTICE			0x00000800
/* Stricter control of channel founder status */
#define CI_SECUREFOUNDER	0x00001000
/* Always sign kicks */
#define CI_SIGNKICK			0x00002000
/* Sign kicks if level is < than the one defined by the SIGNKICK level */
#define CI_SIGNKICK_LEVEL	0x00004000
/* Use the xOP lists */
#define CI_XOP				0x00008000
/* Channel is suspended */
#define CI_SUSPENDED		0x00010000

/* akick */
#define AK_USED		0x0001
#define AK_ISNICK	0x0002
#define AK_STUCK	0x0004

/* botflags */
#define BI_PRIVATE	0x0001
#define BI_CHANSERV	0x0002
#define BI_BOTSERV	0x0004
#define BI_HOSTSERV	0x0008
#define BI_OPERSERV	0x0010
#define BI_MEMOSERV	0x0020
#define BI_NICKSERV	0x0040
#define BI_GLOBAL	0x0080

/* BotServ SET flags */
#define BS_DONTKICKOPS		0x00000001
#define BS_DONTKICKVOICES	0x00000002
#define BS_FANTASY			0x00000004
#define BS_SYMBIOSIS		0x00000008
#define BS_GREET			0x00000010
#define BS_NOBOT			0x00000020

/* BotServ Kickers flags */
#define BS_KICK_BOLDS 		0x80000000
#define BS_KICK_COLORS 		0x40000000
#define BS_KICK_REVERSES	0x20000000
#define BS_KICK_UNDERLINES	0x10000000
#define BS_KICK_BADWORDS	0x08000000
#define BS_KICK_CAPS		0x04000000
#define BS_KICK_FLOOD		0x02000000
#define BS_KICK_REPEAT		0x01000000

/* Indices for TTB (Times To Ban) */
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
	TTB_SIZE
};

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

const std::string GetLanguageID(int id)
{
	switch (id)
	{
		case LANG_EN_US:
			return "en";
			break;
		case LANG_JA_JIS:
		case LANG_JA_EUC:
		case LANG_JA_SJIS: // these seem to be unused
			return "en";
			break;
		case LANG_ES:
			return "es";
			break;
		case LANG_PT:
			return "pt";
			break;
		case LANG_FR:
			return "fr";
			break;
		case LANG_TR:
			return "tr";
			break;
		case LANG_IT:
			return "it";
			break;
		case LANG_DE:
			return "de";
			break;
		case LANG_CAT:
			return "ca"; // yes, iso639 defines catalan as CA
			break;
		case LANG_GR:
			return "gr";
			break;
		case LANG_NL:
			return "nl";
			break;
		case LANG_RU:
			return "ru";
			break;
		case LANG_HUN:
			return "hu";
			break;
		case LANG_PL:
			return "pl";
			break;
		default:
			abort();
	}
}

/* Open a database file for reading and check for the version */
dbFILE *open_db_read(const char *service, const char *filename, int version)
{
	dbFILE *f;
	FILE *fp;
	int myversion;

	f = new dbFILE;
	if (!f)
	{
		printf("Can't allocate memory for %s database %s.\n", service, filename);
		exit(0);
	}
	strscpy(f->filename, filename, sizeof(f->filename));
	f->mode = 'r';
	fp = fopen(f->filename, "rb");
	if (!fp)
	{
		printf("Can't read %s database %s.\n", service, f->filename);
		//free(f);
		delete f;
		return NULL;
	}
	f->fp = fp;
	myversion = fgetc(fp) << 24 | fgetc(fp) << 16 | fgetc(fp) << 8 | fgetc(fp);
	if (feof(fp))
	{
		printf("Error reading version number on %s: End of file detected.\n", f->filename);
		exit(0);
	}
	else if (myversion < version)
	{
		printf("Unsuported database version (%d) on %s.\n", myversion, f->filename);
		exit(0);
	}
	return f;
}

/* Close it */
void close_db(dbFILE *f)
{
	fclose(f->fp);
	delete f;
}

int read_int16(int16 *ret, dbFILE *f)
{
	int c1, c2;

	c1 = fgetc(f->fp);
	c2 = fgetc(f->fp);
	if (c1 == EOF || c2 == EOF)
		return -1;
	*ret = c1 << 8 | c2;
	return 0;
}

int read_uint16(uint16 *ret, dbFILE *f)
{
	int c1, c2;

	c1 = fgetc(f->fp);
	c2 = fgetc(f->fp);
	if (c1 == EOF || c2 == EOF)
		return -1;
	*ret = c1 << 8 | c2;
	return 0;
}

int write_int16(uint16 val, dbFILE *f)
{
	if (fputc((val >> 8) & 0xFF, f->fp) == EOF || fputc(val & 0xFF, f->fp) == EOF)
		return -1;
	return 0;
}

int read_int32(int32 *ret, dbFILE *f)
{
	int c1, c2, c3, c4;

	c1 = fgetc(f->fp);
	c2 = fgetc(f->fp);
	c3 = fgetc(f->fp);
	c4 = fgetc(f->fp);
	if (c1 == EOF || c2 == EOF || c3 == EOF || c4 == EOF)
		return -1;
	*ret = c1 << 24 | c2 << 16 | c3 << 8 | c4;
	return 0;
}

int read_uint32(uint32 *ret, dbFILE *f)
{
	int c1, c2, c3, c4;

	c1 = fgetc(f->fp);
	c2 = fgetc(f->fp);
	c3 = fgetc(f->fp);
	c4 = fgetc(f->fp);
	if (c1 == EOF || c2 == EOF || c3 == EOF || c4 == EOF)
		return -1;
	*ret = c1 << 24 | c2 << 16 | c3 << 8 | c4;
	return 0;
}

int write_int32(uint32 val, dbFILE *f)
{
	if (fputc((val >> 24) & 0xFF, f->fp) == EOF)
		return -1;
	if (fputc((val >> 16) & 0xFF, f->fp) == EOF)
		return -1;
	if (fputc((val >> 8) & 0xFF, f->fp) == EOF)
		return -1;
	if (fputc((val) & 0xFF, f->fp) == EOF)
		return -1;
	return 0;
}

int read_ptr(void **ret, dbFILE *f)
{
	int c;

	c = fgetc(f->fp);
	if (c == EOF)
		return -1;
	*ret = c ? reinterpret_cast<void *>(1) : reinterpret_cast<void *>(0);
	return 0;
}

int write_ptr(const void *ptr, dbFILE * f)
{
	if (fputc(ptr ? 1 : 0, f->fp) == EOF)
		return -1;
	return 0;
}

int read_string(char **ret, dbFILE *f)
{
	char *s;
	uint16 len;

	if (read_uint16(&len, f) < 0)
		return -1;
	if (len == 0)
	{
		*ret = NULL;
		return 0;
	}
	s = new char[len];
	if (len != fread(s, 1, len, f->fp))
	{
		delete [] s;
		return -1;
	}
	*ret = s;
	return 0;
}

int write_string(const char *s, dbFILE *f)
{
	uint32 len;

	if (!s)
		return write_int16(0, f);
	len = strlen(s);
	if (len > 65534)
		len = 65534;
	if (write_int16(static_cast<uint16>(len + 1), f) < 0)
		return -1;
	if (len > 0 && fwrite(s, 1, len, f->fp) != len)
		return -1;
	if (fputc(0, f->fp) == EOF)
		return -1;
	return 0;
}

NickCore *findcore(const char *nick, int unused)
{
	NickCore *nc;

	for (nc = nclists[HASH(nick)]; nc; nc = nc->next)
		if (!mystricmp(nc->display, nick) && ((nc->unused && unused) || (!nc->unused && !unused)))
			return nc;

	return NULL;
}

NickAlias *findnick(const char *nick)
{
	NickAlias *na;

	for (na = nalists[HASH(nick)]; na; na = na->next)
		if (!mystricmp(na->nick, nick))
			return na;

	return NULL;
}

int write_file_version(dbFILE *f, uint32 version)
{
	FILE *fp = f->fp;
	if (fputc(version >> 24 & 0xFF, fp) < 0 || fputc(version >> 16 & 0xFF, fp) < 0 || fputc(version >> 8 & 0xFF, fp) < 0 || fputc(version & 0xFF, fp) < 0)
	{
		printf("Error writing version number on %s.\n", f->filename);
		exit(0);
	}
	return 1;
}

/* strscpy:  Copy at most len-1 characters from a string to a buffer, and
 *		   add a null terminator after the last character copied.
 */

char *strscpy(char *d, const char *s, size_t len)
{
	char *d_orig = d;

	if (!len)
		return d;
	while (--len && (*d++ = *s++));
	*d = '\0';
	return d_orig;
}

int mystricmp(const char *s1, const char *s2)
{
	register int c;

	while ((c = tolower(*s1)) == tolower(*s2))
	{
		if (!c)
			return 0;
		++s1;
		++s2;
	}
	if (c < tolower(*s2))
		return -1;
	return 1;
}

int delnick(NickAlias *na, int donttouchthelist)
{
	if (!donttouchthelist)
	{
		/* Remove us from the aliases list */
		if (na->next)
			na->next->prev = na->prev;
		if (na->prev)
			na->prev->next = na->next;
		else
			nalists[HASH(na->nick)] = na->next;
	}

	if (na->last_usermask)
		delete [] na->last_usermask;
	if (na->last_realname)
		delete [] na->last_realname;
	if (na->last_quit)
		delete [] na->last_quit;
	/* free() us */
	delete [] na->nick;
	delete na;
	return 1;
}

int delcore(NickCore *nc)
{
	int i;
	/* Remove the core from the list */
	if (nc->next)
		nc->next->prev = nc->prev;
	if (nc->prev)
		nc->prev->next = nc->next;
	else
		nclists[HASH(nc->display)] = nc->next;

	delete [] nc->display;
	if (nc->email)
		delete [] nc->email;
	if (nc->greet)
		delete [] nc->greet;
	if (nc->url)
		delete [] nc->url;
	if (nc->access)
	{
		for (i = 0; i < nc->accesscount; ++i)
			if (nc->access[i])
				delete [] nc->access[i];
		delete [] nc->access;
	}
	if (nc->memos.memos)
	{
		for (i = 0; i < nc->memos.memocount; ++i)
			if (nc->memos.memos[i].text)
				delete [] nc->memos.memos[i].text;
		delete [] nc->memos.memos;
	}
	delete nc;
	return 1;
}

ChannelInfo *cs_findchan(const char *chan)
{
	ChannelInfo *ci;
	for (ci = chanlists[tolower(chan[1])]; ci; ci = ci->next)
		if (!mystricmp(ci->name, chan))
			return ci;
	return NULL;
}

void alpha_insert_chan(ChannelInfo *ci)
{
	ChannelInfo *ptr, *prev;
	char *chan = ci->name;

	for (prev = NULL, ptr = chanlists[tolower(chan[1])]; ptr && mystricmp(ptr->name, chan) < 0; prev = ptr, ptr = ptr->next);
	ci->prev = prev;
	ci->next = ptr;
	if (!prev)
		chanlists[tolower(chan[1])] = ci;
	else
		prev->next = ci;
	if (ptr)
		ptr->prev = ci;
}

HostCore *findHostCore(char *nick)
{
	for (HostCore *hc = head; hc; hc = hc->next)
		if (nick && hc->nick && !mystricmp(hc->nick, nick))
			return hc;
	return NULL;
}

static char *int_to_base64(long);
static long base64_to_int(char *);

const char *base64enc(long i)
{
	if (i < 0)
		return "0";
	return int_to_base64(i);
}

long base64dec(char *b64)
{
	if (b64)
		return base64_to_int(b64);
	else
		return 0;
}

static const char Base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char Pad64 = '=';

/* (From RFC1521 and draft-ietf-dnssec-secext-03.txt)
   The following encoding technique is taken from RFC 1521 by Borenstein
   and Freed.  It is reproduced here in a slightly edited form for
   convenience.

   A 65-character subset of US-ASCII is used, enabling 6 bits to be
   represented per printable character. (The extra 65th character, "=",
   is used to signify a special processing function.)

   The encoding process represents 24-bit groups of input bits as output
   strings of 4 encoded characters. Proceeding from left to right, a
   24-bit input group is formed by concatenating 3 8-bit input groups.
   These 24 bits are then treated as 4 concatenated 6-bit groups, each
   of which is translated into a single digit in the base64 alphabet.

   Each 6-bit group is used as an index into an array of 64 printable
   characters. The character referenced by the index is placed in the
   output string.

						 Table 1: The Base64 Alphabet

	  Value Encoding  Value Encoding  Value Encoding  Value Encoding
		  0 A			17 R			34 i			51 z
		  1 B			18 S			35 j			52 0
		  2 C			19 T			36 k			53 1
		  3 D			20 U			37 l			54 2
		  4 E			21 V			38 m			55 3
		  5 F			22 W			39 n			56 4
		  6 G			23 X			40 o			57 5
		  7 H			24 Y			41 p			58 6
		  8 I			25 Z			42 q			59 7
		  9 J			26 a			43 r			60 8
		 10 K			27 b			44 s			61 9
		 11 L			28 c			45 t			62 +
		 12 M			29 d			46 u			63 /
		 13 N			30 e			47 v
		 14 O			31 f			48 w		 (pad) =
		 15 P			32 g			49 x
		 16 Q			33 h			50 y

   Special processing is performed if fewer than 24 bits are available
   at the end of the data being encoded.  A full encoding quantum is
   always completed at the end of a quantity.  When fewer than 24 input
   bits are available in an input group, zero bits are added (on the
   right) to form an integral number of 6-bit groups.  Padding at the
   end of the data is performed using the '=' character.

   Since all base64 input is an integral number of octets, only the
		 -------------------------------------------------
   following cases can arise:

	   (1) the final quantum of encoding input is an integral
		   multiple of 24 bits; here, the final unit of encoded
	   output will be an integral multiple of 4 characters
	   with no "=" padding,
	   (2) the final quantum of encoding input is exactly 8 bits;
		   here, the final unit of encoded output will be two
	   characters followed by two "=" padding characters, or
	   (3) the final quantum of encoding input is exactly 16 bits;
		   here, the final unit of encoded output will be three
	   characters followed by one "=" padding character.
   */

int b64_encode(char *src, size_t srclength, char *target, size_t targsize)
{
	size_t datalength = 0;
	unsigned char input[3];
	unsigned char output[4];
	size_t i;

	while (srclength > 2)
	{
		input[0] = *src++;
		input[1] = *src++;
		input[2] = *src++;
		srclength -= 3;

		output[0] = input[0] >> 2;
		output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
		output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
		output[3] = input[2] & 0x3f;

		if (datalength + 4 > targsize)
			return -1;
		target[datalength++] = Base64[output[0]];
		target[datalength++] = Base64[output[1]];
		target[datalength++] = Base64[output[2]];
		target[datalength++] = Base64[output[3]];
	}

	/* Now we worry about padding. */
	if (srclength)
	{
		/* Get what's left. */
		input[0] = input[1] = input[2] = '\0';
		for (i = 0; i < srclength; ++i)
			input[i] = *src++;

		output[0] = input[0] >> 2;
		output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
		output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);

		if (datalength + 4 > targsize)
			return -1;
		target[datalength++] = Base64[output[0]];
		target[datalength++] = Base64[output[1]];
		if (srclength == 1)
			target[datalength++] = Pad64;
		else
			target[datalength++] = Base64[output[2]];
		target[datalength++] = Pad64;
	}
	if (datalength >= targsize)
		return -1;
	target[datalength] = '\0';  /* Returned value doesn't count \0. */
	return datalength;
}

/* skips all whitespace anywhere.
   converts characters, four at a time, starting at (or after)
   src from base - 64 numbers into three 8 bit bytes in the target area.
   it returns the number of data bytes stored at the target, or -1 on error.
 */

int b64_decode(const char *src, char *target, size_t targsize)
{
	int tarindex, state, ch;
	const char *pos;

	state = 0;
	tarindex = 0;

	while ((ch = *src++))
	{
		if (isspace(ch)) /* Skip whitespace anywhere. */
			continue;

		if (ch == Pad64)
			break;

		pos = strchr(Base64, ch);
		if (!pos) /* A non-base64 character. */
			return -1;

		switch (state)
		{
			case 0:
				if (target)
				{
					if (static_cast<size_t>(tarindex) >= targsize)
						return -1;
					target[tarindex] = (pos - Base64) << 2;
				}
				state = 1;
				break;
			case 1:
				if (target)
				{
					if (static_cast<size_t>(tarindex) + 1 >= targsize)
						return -1;
					target[tarindex] |= (pos - Base64) >> 4;
					target[tarindex + 1] = ((pos - Base64) & 0x0f) << 4;
				}
				++tarindex;
				state = 2;
				break;
			case 2:
				if (target)
				{
					if (static_cast<size_t>(tarindex) + 1 >= targsize)
						return -1;
					target[tarindex] |= (pos - Base64) >> 2;
					target[tarindex + 1] = ((pos - Base64) & 0x03) << 6;
				}
				++tarindex;
				state = 3;
				break;
			case 3:
				if (target)
				{
					if (static_cast<size_t>(tarindex) >= targsize)
						return (-1);
					target[tarindex] |= pos - Base64;
				}
				++tarindex;
				state = 0;
				break;
			default:
				abort();
		}
	}

	/*
	 * We are done decoding Base-64 chars.  Let's see if we ended
	 * on a byte boundary, and/or with erroneous trailing characters.
	 */

	if (ch == Pad64) /* We got a pad char. */
	{
		ch = *src++; /* Skip it, get next. */
		switch (state)
		{
			case 0: /* Invalid = in first position */
			case 1: /* Invalid = in second position */
				return (-1);

			case 2: /* Valid, means one byte of info */
				/* Skip any number of spaces. */
				for (; ch != '\0'; ch = *src++)
					if (!isspace(ch))
						break;
				/* Make sure there is another trailing = sign. */
				if (ch != Pad64)
					return -1;
				ch = *src++; /* Skip the = */
				/* Fall through to "single trailing =" case. */
				/* FALLTHROUGH */

			case 3: /* Valid, means two bytes of info */
				/*
				 * We know this char is an =.  Is there anything but
				 * whitespace after it?
				 */
				for (; ch != '\0'; ch = *src++)
					if (!isspace(ch))
						return (-1);

				/*
				 * Now make sure for cases 2 and 3 that the "extra"
				 * bits that slopped past the last full byte were
				 * zeros.  If we don't check them, they become a
				 * subliminal channel.
				 */
				if (target && target[tarindex])
					return -1;
		}
	}
	else
	{
		/*
		 * We ended by seeing the end of the string.  Make sure we
		 * have no partial bytes lying around.
		 */
		if (state)
			return -1;
	}

	return tarindex;
}

/* ':' and '#' and '&' and '+' and '@' must never be in this table. */
/* these tables must NEVER CHANGE! >) */
char int6_to_base64_map[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D',
	'E', 'F',
	'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V',
	'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l',
	'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'{', '}'
};

char base64_to_int6_map[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
	-1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
	-1, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, -1, 63, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

static char *int_to_base64(long val)
{
	/* 32/6 == max 6 bytes for representation,
	 * +1 for the null, +1 for byte boundaries
	 */
	static char base64buf[8];
	long i = 7;

	base64buf[i] = '\0';

	/* Temporary debugging code.. remove before 2038 ;p.
	 * This might happen in case of 64bit longs (opteron/ia64),
	 * if the value is then too large it can easily lead to
	 * a buffer underflow and thus to a crash. -- Syzop
	 */
	if (val > 2147483647L)
		abort();

	do
	{
		base64buf[--i] = int6_to_base64_map[val & 63];
	}
	while (val >>= 6);

	return base64buf + i;
}

static long base64_to_int(char *b64)
{
	int v = base64_to_int6_map[static_cast<unsigned char>(*b64++)];

	if (!b64)
		return 0;

	while (*b64)
	{
		v <<= 6;
		v += base64_to_int6_map[static_cast<unsigned char>(*b64++)];
	}

	return v;
}

#endif // DB_CONVERT_H
