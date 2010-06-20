/*
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#ifndef SERVICES_H
#define SERVICES_H

/*************************************************************************/

#include "sysconf.h"

#define BUFSIZE 1024

/* Some SUN fixs */
#ifdef __sun
/* Solaris specific code, types that do not exist in Solaris'
 * sys/types.h
 */
# undef u_int8_t
# undef u_int16_t
# undef u_int32_t
# undef u_int_64_t
# define u_int8_t uint8_t
# define u_int16_t uint16_t
# define u_int32_t uint32_t
# define u_int64_t uint64_t

# ifndef INADDR_NONE
#  define INADDR_NONE (-1)
# endif
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#include <sys/stat.h> /* for umask() on some systems */
#include <sys/types.h>
#include <assert.h>
#include <fcntl.h>

#ifndef _WIN32
# include <unistd.h>
# include <grp.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <dirent.h>
# ifdef HAVE_BACKTRACE
#  include <execinfo.h>
# endif
# define DllExport
# define CoreExport
# define MARK_DEPRECATED __attribute((deprecated))
# define DeleteFile unlink
#else
# include <winsock2.h>
# include <ws2tcpip.h>
# include <windows.h>
# include <sys/timeb.h>
# include <direct.h>
# include <io.h>
# ifdef MODULE_COMPILE
#  define CoreExport __declspec(dllimport)
#  define DllExport __declspec(dllexport)
# else
#  define CoreExport __declspec(dllexport)
#  define DllExport __declspec(dllimport)
# endif
/* VS2008 hates having this define before its own */
# define vsnprintf _vsnprintf
/* We have our own inet_pton and inet_ntop (Windows doesn't have its own) */
# define inet_pton inet_pton_
# define inet_ntop inet_ntop_
# define MARK_DEPRECATED

extern CoreExport const char *dlerror();
extern CoreExport int inet_pton(int af, const char *src, void *dst);
extern CoreExport const char *inet_ntop(int af, const void *src, char *dst, size_t size);
#endif

/* Telling compilers about printf()-like functions: */
#ifdef __GNUC__
# define FORMAT(type, fmt, start) __attribute__((format(type, fmt, start)))
#else
# define FORMAT(type, fmt, start)
#endif

#ifdef HAVE_GETTIMEOFDAY
# include <sys/time.h>
#endif

#if HAVE_STRINGS_H
# include <strings.h>
#endif

#if HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

#ifndef va_copy
# ifdef __va_copy
#  define VA_COPY(DEST,SRC) __va_copy((DEST), (SRC))
# else
#  define VA_COPY(DEST, SRC) memcpy ((&DEST), (&SRC), sizeof(va_list))
# endif
#else
# define VA_COPY(DEST, SRC) va_copy((DEST), (SRC))
#endif

#ifdef _AIX
/* Some AIX boxes seem to have bogus includes that don't have these
 * prototypes. */
extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *, const char *, size_t);
# undef FD_ZERO
# define FD_ZERO(p) memset((p), 0, sizeof(*(p)))
#endif /* _AIX */

/* Alias stricmp/strnicmp to strcasecmp/strncasecmp if we have the latter
 * but not the former. */
#if !HAVE_STRICMP && HAVE_STRCASECMP
# define stricmp strcasecmp
# define strnicmp strncasecmp
#endif

/* We have our own versions of toupper()/tolower(). */
#include <ctype.h>
#undef tolower
#undef toupper
#define tolower tolower_
#define toupper toupper_

#ifdef __WINS__
# ifndef BKCHECK
#  define BKCHECK
extern "C" void __pfnBkCheck() {}
#  endif
#endif

#if INTTYPE_WORKAROUND
# undef int16
# undef int32
#endif

/** This definition is used as shorthand for the various classes
 * and functions needed to make a module loadable by the OS.
 * It defines the class factory and external AnopeInit and AnopeFini functions.
 */
#ifdef _WIN32
# define MODULE_INIT(x) \
	extern "C" DllExport Module *AnopeInit(const std::string &, const std::string &); \
	extern "C" Module *AnopeInit(const std::string &modname, const std::string &creator) \
	{ \
		return new x(modname, creator); \
	} \
	BOOLEAN WINAPI DllMain(HINSTANCE, DWORD nReason, LPVOID) \
	{ \
		switch (nReason) \
		{ \
			case DLL_PROCESS_ATTACH: \
			case DLL_PROCESS_DETACH: \
				break; \
		} \
		return TRUE; \
	} \
	extern "C" DllExport void AnopeFini(x *); \
	extern "C" void AnopeFini(x *m) \
	{ \
		delete m; \
	}
#else
# define MODULE_INIT(x) \
	extern "C" DllExport Module *AnopeInit(const std::string &modname, const std::string &creator) \
	{ \
		return new x(modname, creator); \
	} \
	extern "C" DllExport void AnopeFini(x *m) \
	{ \
		delete m; \
	}
#endif

/* Miscellaneous definitions. */
#include "hashcomp.h"

/* Pull in the various bits of STL */
#include <iostream>
#include <string>
#include <map>
#include <exception>
#include <list>
#include <vector>
#include <deque>
#include <bitset>
#include <set>

/** This class can be used on its own to represent an exception, or derived to represent a module-specific exception.
 * When a module whishes to abort, e.g. within a constructor, it should throw an exception using ModuleException or
 * a class derived from ModuleException. If a module throws an exception during its constructor, the module will not
 * be loaded. If this happens, the error message returned by ModuleException::GetReason will be displayed to the user
 * attempting to load the module, or dumped to the console if the ircd is currently loading for the first time.
 */
class CoreException : public std::exception
{
 protected:
	/** Holds the error message to be displayed
	 */
	std::string err;
	/** Source of the exception
	 */
	std::string source;
 public:
	/** Default constructor, just uses the error mesage 'Core threw an exception'.
	 */
	CoreException() : err("Core threw an exception"), source("The core") {}
	/** This constructor can be used to specify an error message before throwing.
	 */
	CoreException(const char *message) : err(message), source("The core") {}
	CoreException(const std::string &message) : err(message), source("The core") {}
	CoreException(const ci::string &message) : err(message.c_str()), source("The core") {}
	/** This constructor can be used to specify an error message before throwing,
	 * and to specify the source of the exception.
	 */
	CoreException(const char *message, const char *src) : err(message), source(src) {}
	CoreException(const std::string &message, const char *src) : err(message), source(src) {}
	CoreException(const ci::string &message, const char *src) : err(message.c_str()), source(src) {}
	CoreException(const std::string &message, const std::string &src) : err(message), source(src) {}
	CoreException(const ci::string &message, const std::string &src) : err(message.c_str()), source(src) {}
	CoreException(const std::string &message, const ci::string &src) : err(message), source(src.c_str()) {}
	CoreException(const ci::string &message, const ci::string &src) : err(message.c_str()), source(src.c_str()) {}
	/** This destructor solves world hunger, cancels the world debt, and causes the world to end.
	 * Actually no, it does nothing. Never mind.
	 * @throws Nothing!
	 */
	virtual ~CoreException() throw() {}
	/** Returns the reason for the exception.
	 * The module should probably put something informative here as the user will see this upon failure.
	 */
	virtual const char *GetReason() const
	{
		return err.c_str();
	}

	virtual const char *GetSource() const
	{
		return source.c_str();
	}
};

class ModuleException : public CoreException
{
 public:
	/** Default constructor, just uses the error mesage 'Module threw an exception'.
	 */
	ModuleException() : CoreException("Module threw an exception", "A Module") {}

	/** This constructor can be used to specify an error message before throwing.
	 */
	ModuleException(const char *message) : CoreException(message, "A Module") {}
	ModuleException(const std::string &message) : CoreException(message, "A Module") {}
	ModuleException(const ci::string &message) : CoreException(message, "A Module") {}
	/** This destructor solves world hunger, cancels the world debt, and causes the world to end.
	 * Actually no, it does nothing. Never mind.
	 * @throws Nothing!
	 */
	virtual ~ModuleException() throw() {}
};

class DatabaseException : public CoreException
{
 public:
	/** This constructor can be used to specify an error message before throwing.
	 * @param mmessage The exception
	 */
	DatabaseException(const std::string &message) : CoreException(message, "A database module") { }

	/** Destructor
	 * @throws Nothing
	 */
	virtual ~DatabaseException() throw() { }
};

/*************************************************************************/

#include "sockets.h"

/*************************************************************************/

/** Class with the ability to keep flags on items, they should extend from this
 * where T is an enum.
 */
template<typename T> class Flags
{
 protected:
	std::bitset<128> Flag_Values;

 public:
	/** Add a flag to this item
	 * @param Value The flag
	 */
	void SetFlag(T Value)
	{
		Flag_Values[Value] = true;
	}

	/** Remove a flag from this item
	 * @param Value The flag
	 */
	void UnsetFlag(T Value)
	{
		Flag_Values[Value] = false;
	}

	/** Check if this item has a flag
	 * @param Value The flag
	 * @return true or false
	 */
	bool HasFlag(T Value) const
	{
		return Flag_Values[Value];
	}

	/** Check how many flags are set
	 * @return The number of flags set
	 */
	size_t FlagCount() const
	{
		return Flag_Values.count();
	}

	/** Unset all of the flags
	 */
	void ClearFlags()
	{
		Flag_Values.reset();
	}
};

/*************************************************************************/

/* forward declarations, mostly used by older code */
class User;
class ChannelInfo;
class Channel;
class Server;
struct EList;
struct Session;

#include "extensible.h"
#include "threadengine.h"
#include "bots.h"
#include "opertype.h"
#include "modes.h"

/*************************************************************************/

/* Protocol tweaks */

struct IRCDVar
{
	const char *name;				/* Name of the IRCd command */
	const char *pseudoclient_mode;	/* Mode used by BotServ Bots */
	int max_symbols;				/* Chan Max Symbols */
	const char *botchanumode;		/* Modes set when botserv joins a channel */
	int svsnick;					/* Supports SVSNICK */
	int vhost;						/* Supports vhost */
	int snline;						/* Supports SNline */
	int sqline;						/* Supports SQline */
	int szline;						/* Supports SZline */
	int numservargs;				/* Number of Server Args */
	int join2set;					/* Join 2 Set Modes */
	int join2msg;					/* Join 2 Message */
	int topictsforward;				/* TS on Topics Forward */
	int topictsbackward;			/* TS on Topics Backward */
	int chansqline;					/* Supports Channel Sqlines */
	int quitonkill;					/* IRCD sends QUIT when kill */
	int svsmode_unban;				/* svsmode can be used to unban */
	int reversekickcheck;			/* Can reverse ban check */
	int vident;						/* Supports vidents */
	int svshold;					/* Supports svshold */
	int tsonmode;					/* Timestamp on mode changes */
	int nickip;						/* Sends IP on NICK */
	int omode;						/* On the fly o:lines */
	int umode;						/* change user modes */
	int nickvhost;					/* Users vhost sent during NICK */
	int chgreal;					/* Change RealName */
	int knock_needs_i;				/* Check if we needed +i when setting NOKNOCK */
	int token;						/* Does Anope support the tokens for the ircd */
	int sjb64;
	int svsmode_ucmode;				/* Can remove User Channel Modes with SVSMODE */
	int sglineenforce;
	int ts6;						/* ircd is TS6 */
	int p10;						/* ircd is P10  */
	int cidrchanbei;				/* channel bans/excepts/invites support CIDR (syntax: +b *!*@192.168.0.0/15)
									 * 0 for no support, 1 for strict cidr support, anything else
									 * for ircd specific support (nefarious only cares about first /mask) */
	const char *globaltldprefix;	/* TLD prefix used for Global */
	unsigned maxmodes;				/* Max modes to send per line */
};

/*************************************************************************/

/** Memo Flags
 */
enum MemoFlag
{
	/* Memo is unread */
	MF_UNREAD,
	/* SEnder requests a receipt */
	MF_RECEIPT,
	/* Memo is a notification of receipt */
	MF_NOTIFYS
};

/* Memo info structures.  Since both nicknames and channels can have memos,
 * we encapsulate memo data in a MemoList to make it easier to handle. */
class Memo : public Flags<MemoFlag>
{
 public:
	uint32 number;	/* Index number -- not necessarily array position! */
	time_t time;	/* When it was sent */
	std::string sender;
	char *text;
};

struct MemoInfo
{
	int16 memomax;
	std::vector<Memo *> memos;
};

/*************************************************************************/

class CoreExport HostInfo
{
 private:
	std::string Ident;
	std::string Host;
	std::string Creator;
	time_t Time;

 public:
 	/** Set a vhost for the user
	 * @param ident The ident
	 * @param host The host
	 * @param creator Who created the vhost
	 * @param time When the vhost was craated
	 */
	void SetVhost(const std::string &ident, const std::string &host, const std::string &creator, time_t created = time(NULL));

	/** Remove a users vhost
	 **/
	void RemoveVhost();

	/** Check if the user has a vhost
	 * @return true or false
	 */
	bool HasVhost() const;

	/** Retrieve the vhost ident
	 * @return the ident
	 */
	const std::string &GetIdent() const;

	/** Retrieve the vhost host
	 * @return the host
	 */
	const std::string &GetHost() const;

	/** Retrieve the vhost creator
	 * @return the creator
	 */
	const std::string &GetCreator() const;

	/** Retrieve when the vhost was crated
	 * @return the time it was created
	 */
	const time_t GetTime() const;
};

// For NickServ
#include "account.h"

/*************************************************************************/

enum AccessLevel
{
	/* Note that these two levels also serve as exclusive boundaries for valid
	 * access levels.  ACCESS_FOUNDER may be assumed to be strictly greater
	 * than any valid access level, and ACCESS_INVALID may be assumed to be
	 * strictly less than any valid access level. Also read below.
	 */
	ACCESS_FOUNDER = 10001, /* Numeric level indicating founder access */
	ACCESS_INVALID = -10000, /* Used in levels[] for disabled settings */
	/* There is one exception to the above access levels: SuperAdmins will have
	 * access level 10001. This level is never stored, however; it is only used
	 * in comparison and to let SuperAdmins win from founders where needed
	 */
	ACCESS_SUPERADMIN = 10002,
	/* Levels for xOP */
	ACCESS_VOP = 3,
	ACCESS_HOP = 4,
	ACCESS_AOP = 5,
	ACCESS_SOP = 10,
	ACCESS_QOP = 10000
};

/* Channel info structures.  Stored similarly to the nicks, except that
 * the second character of the channel name, not the first, is used to
 * determine the list.  (Hashing based on the first character of the name
 * wouldn't get very far. ;) ) */

/* Access levels for users. */
struct ChanAccess
{
	int16 level;
	NickCore *nc; /* Guaranteed to be non-NULL if in use, NULL if not */
	time_t last_seen;
	std::string creator;
};

/** Flags for auto kick
 */
enum AutoKickFlag
{
	/* Is by nick core, not mask */
	AK_ISNICK,
	/* This entry is stuck */
	AK_STUCK
};

/* AutoKick data. */
class AutoKick : public Flags<AutoKickFlag>
{
 public:
	/* Only one of these can be in use */
	std::string mask;
	NickCore *nc;

	std::string reason;
	std::string creator;
	time_t addtime;
	time_t last_used;
};

/** Flags for badwords
 */
enum BadWordType
{
	/* Always kicks if the word is said */
	BW_ANY,
	/* User must way the entire word */
	BW_SINGLE,
	/* The word has to start with the badword */
	BW_START,
	/* The word has to end with the badword */
	BW_END
};

/* Structure used to contain bad words. */
struct BadWord
{
	std::string word;
	BadWordType type;
};

/* Indices for cmd_access[]: */
enum ChannelAccess
{
	CA_INVITE,
	CA_AKICK,
	CA_SET,			/* but not FOUNDER or PASSWORD */
	CA_UNBAN,
	CA_AUTOOP,
	CA_AUTODEOP,	/* Maximum, not minimum */
	CA_AUTOVOICE,
	CA_OPDEOP,		/* ChanServ commands OP and DEOP */
	CA_ACCESS_LIST,
	CA_CLEAR,
	CA_NOJOIN,		/* Maximum */
	CA_ACCESS_CHANGE,
	CA_MEMO,
	CA_ASSIGN,		/* BotServ ASSIGN command */
	CA_BADWORDS,	/* BotServ BADWORDS command */
	CA_NOKICK,		/* Not kicked by the bot */
	CA_FANTASIA,
	CA_SAY,
	CA_GREET,
	CA_VOICEME,
	CA_VOICE,
	CA_GETKEY,
	CA_AUTOHALFOP,
	CA_AUTOPROTECT,
	CA_OPDEOPME,
	CA_HALFOPME,
	CA_HALFOP,
	CA_PROTECTME,
	CA_PROTECT,
	CA_KICKME,
	CA_KICK,
	CA_SIGNKICK,
	CA_BANME,
	CA_BAN,
	CA_TOPIC,
	CA_INFO,
	CA_AUTOOWNER,
	CA_OWNER,
	CA_OWNERME,
	CA_FOUNDER,
	CA_SIZE
};

/* BotServ SET flags */
enum BotServFlag
{
	/* BotServ won't kick ops */
	BS_DONTKICKOPS,
	/* BotServ won't kick voices */
	BS_DONTKICKVOICES,
	/* BotServ bot accepts fantasy commands */
	BS_FANTASY,
	/* BotServ bot sets modes etc instead of ChanServ */
	BS_SYMBIOSIS,
	/* BotServ should show greets */
	BS_GREET,
	/* BotServ bots are not allowed to be in this channel */
	BS_NOBOT,
	/* BotServ kicks for bolds */
	BS_KICK_BOLDS,
	/* BotServ kicks for colors */
	BS_KICK_COLORS,
	/* BOtServ kicks for reverses */
	BS_KICK_REVERSES,
	/* BotServ kicks for underlines */
	BS_KICK_UNDERLINES,
	/* BotServ kicks for badwords */
	BS_KICK_BADWORDS,
	/* BotServ kicks for caps */
	BS_KICK_CAPS,
	/* BotServ kicks for flood */
	BS_KICK_FLOOD,
	/* BotServ kicks for repeating */
	BS_KICK_REPEAT
};

/* Indices for TTB (Times To Ban) */
#define TTB_BOLDS 		0
#define TTB_COLORS 		1
#define TTB_REVERSES		2
#define TTB_UNDERLINES  	3
#define TTB_BADWORDS		4
#define TTB_CAPS		5
#define TTB_FLOOD		6
#define TTB_REPEAT		7
#define TTB_SIZE 		8

#include "regchannel.h"

/*************************************************************************/

struct LevelInfo
{
	int what;
	const char *name;
	int desc;
};


/*************************************************************************/

#include "users.h"
/* This structure stocks ban data since it must not be removed when
 * user is kicked.
 */

struct BanData
{
	BanData *next, *prev;

	char *mask;			/* Since a nick is unsure and a User structure is unsafe */
	time_t last_use;	/* Since time is the only way to check whether it's still useful */
	int16 ttb[TTB_SIZE];
};

#include "channels.h"

/** Channelban type flags
 */
enum EntryType
{
	ENTRYTYPE_NONE,
	ENTRYTYPE_CIDR4,
	ENTRYTYPE_NICK_WILD,
	ENTRYTYPE_NICK,
	ENTRYTYPE_USER_WILD,
	ENTRYTYPE_USER,
	ENTRYTYPE_HOST_WILD,
	ENTRYTYPE_HOST
};

class Entry : public Flags<EntryType>
{
 public:
	Entry *next, *prev;
	uint32 cidr_ip;		/* IP mask for CIDR matching */
	uint32 cidr_mask;	/* Netmask for CIDR matching */
	char *nick, *user, *host, *mask;
};

struct EList
{
	Entry *entries;
	int32 count;
};

/*************************************************************************/

/* Ignorance list data. */

struct IgnoreData
{
	IgnoreData *prev, *next;
	char *mask;
	time_t time; /* When do we stop ignoring them? */
};

/*************************************************************************/

/* News stuff */

#define MSG_MAX 11

enum NewsType
{
	NEWS_LOGON,
	NEWS_RANDOM,
	NEWS_OPER
};

struct newsmsgs
{
	NewsType type;
	const char *name;
	int msgs[MSG_MAX + 1];
};

struct NewsItem
{
	NewsType type;
	uint32 num;
	std::string Text;
	std::string who;
	time_t time;
};

/*************************************************************************/

/* Mail data */

struct MailInfo
{
	FILE *pipe;
	User *sender;
	NickCore *recipient;
	NickRequest *recip;
};

/*************************************************************************/

struct Exception
{
	char *mask;		/* Hosts to which this exception applies */
	int limit;		/* Session limit for exception */
	char *who;		/* Nick of person who added the exception */
	char *reason;	/* Reason for exception's addition */
	time_t time;	/* When this exception was added */
	time_t expires;	/* Time when it expires. 0 == no expiry */
	int num;		/* Position in exception list; used to track
					 * positions when deleting entries. It is
					 * symbolic and used internally. It is
					 * calculated at load time and never saved. */
};

/*************************************************************************/

typedef unordered_map_namespace::unordered_map<std::string, Session *, hash_compare_std_string> session_map;
extern CoreExport session_map SessionList;

struct Session
{
	char *host;
	int count;	/* Number of clients with this host */
	int hits;	/* Number of subsequent kills for a host */
};

/*************************************************************************/

/* Defcon */
enum DefconLevel
{
	DEFCON_NO_NEW_CHANNELS,
	DEFCON_NO_NEW_NICKS,
	DEFCON_NO_MLOCK_CHANGE,
	DEFCON_FORCE_CHAN_MODES,
	DEFCON_REDUCE_SESSION,
	DEFCON_NO_NEW_CLIENTS,
	DEFCON_OPER_ONLY,
	DEFCON_SILENT_OPER_ONLY,
	DEFCON_AKILL_NEW_CLIENTS,
	DEFCON_NO_NEW_MEMOS
};

/*************************************************************************/

/* Languages.  Never insert anything in the middle of this list, or
 * everybody will start getting the wrong language!  If you want to change
 * the order the languages are displayed in for NickServ HELP SET LANGUAGE,
 * do it in language.c.
 */
enum Languages
{
	LANG_EN_US,		/* United States English */
	LANG_JA_JIS,	/* Japanese (JIS encoding) */
	LANG_JA_EUC,	/* Japanese (EUC encoding) */
	LANG_JA_SJIS,	/* Japanese (SJIS encoding) */
	LANG_ES,		/* Spanish */
	LANG_PT,		/* Portugese */
	LANG_FR,		/* French */
	LANG_TR,		/* Turkish */
	LANG_IT,		/* Italian */
	LANG_DE,		/* German */
	LANG_CAT,		/* Catalan */
	LANG_GR,		/* Greek */
	LANG_NL,		/* Dutch */
	LANG_RU,		/* Russian */
	LANG_HUN,		/* Hungarian */
	LANG_PL,		/* Polish */
	NUM_LANGS		/* Number of languages */
};
static const int USED_LANGS = 13; /* Number of languages provided */

static const int DEF_LANGUAGE = LANG_EN_US;

/*************************************************************************/

/**
 * RFC: defination of a valid nick
 * nickname =  ( letter / special ) *8( letter / digit / special / "-" )
 * letter   =  %x41-5A / %x61-7A ; A-Z / a-z
 * digit    =  %x30-39 ; 0-9
 * special  =  %x5B-60 / %x7B-7D ; "[", "]", "\", "`", "_", "^", "{", "|", "}"
 **/
#define isvalidnick(c) (isalnum(c) || ((c) >= '\x5B' && (c) <= '\x60') || ((c) >= '\x7B' && (c) <= '\x7D') || (c) == '-')

/*************************************************************************/

/*
 * Forward declaration reqired, because the base IRCDProto class uses some crap from in here.
 */
class IRCDProto;
struct Uplink;
class ServerConfig;

#include "extern.h"
#include "operserv.h"
#include "mail.h"
#include "servers.h"
#include "config.h"

class CoreExport IRCDProto
{
 private:
	virtual void SendSVSKillInternal(BotInfo *, User *, const char *) = 0;
	virtual void SendModeInternal(BotInfo *, Channel *, const char *) = 0;
	virtual void SendModeInternal(BotInfo *, User *, const char *) = 0;
	virtual void SendKickInternal(BotInfo *, Channel *, User *, const char *) = 0;
	virtual void SendNoticeChanopsInternal(BotInfo *bi, Channel *, const char *) = 0;
	virtual void SendMessageInternal(BotInfo *bi, const char *dest, const char *buf);
	virtual void SendNoticeInternal(BotInfo *bi, const char *dest, const char *msg);
	virtual void SendPrivmsgInternal(BotInfo *bi, const char *dest, const char *buf);
	virtual void SendQuitInternal(BotInfo *bi, const char *buf);
	virtual void SendPartInternal(BotInfo *bi, Channel *chan, const char *buf);
	virtual void SendGlobopsInternal(BotInfo *source, const char *buf);
	virtual void SendCTCPInternal(BotInfo *bi, const char *dest, const char *buf);
	virtual void SendNumericInternal(const char *source, int numeric, const char *dest, const char *buf);
 public:
	virtual ~IRCDProto() { }

	virtual void SendSVSNOOP(const char *, int) { }
	virtual void SendTopic(BotInfo *, Channel *, const char *, const char *) = 0;
	virtual void SendVhostDel(User *) { }
	virtual void SendAkill(XLine *) = 0;
	virtual void SendAkillDel(XLine *) = 0;
	virtual void SendSVSKill(BotInfo *source, User *user, const char *fmt, ...);
	virtual void SendSVSMode(User *, int, const char **) = 0;
	virtual void SendMode(BotInfo *bi, Channel *dest, const char *fmt, ...);
	virtual void SendMode(BotInfo *bi, User *u, const char *fmt, ...);
	virtual void SendClientIntroduction(const std::string &, const std::string &, const std::string &, const std::string &, const char *, const std::string &uid) = 0;
	virtual void SendKick(BotInfo *bi, Channel *chan, User *user, const char *fmt, ...);
	virtual void SendNoticeChanops(BotInfo *bi, Channel *dest, const char *fmt, ...);
	virtual void SendMessage(BotInfo *bi, const char *dest, const char *fmt, ...);
	virtual void SendNotice(BotInfo *bi, const char *dest, const char *fmt, ...);
	virtual void SendAction(BotInfo *bi, const char *dest, const char *fmt, ...);
	virtual void SendPrivmsg(BotInfo *bi, const char *dest, const char *fmt, ...);
	virtual void SendGlobalNotice(BotInfo *bi, Server *dest, const char *msg);
	virtual void SendGlobalPrivmsg(BotInfo *bi, Server *desc, const char *msg);

	/** XXX: This is a hack for NickServ enforcers. It is deprecated.
	 * If I catch any developer using this in new code, I will RIP YOUR BALLS OFF.
	 * Thanks.
	 * -- w00t
	 */
	virtual void SendQuit(const char *nick, const char *) MARK_DEPRECATED;
	virtual void SendQuit(BotInfo *bi, const char *fmt, ...);
	virtual void SendPing(const char *servname, const char *who);
	virtual void SendPong(const char *servname, const char *who);
	virtual void SendJoin(BotInfo *bi, const char *, time_t) = 0;
	virtual void SendSQLineDel(XLine *x) = 0;
	virtual void SendInvite(BotInfo *bi, const char *chan, const char *nick);
	virtual void SendPart(BotInfo *bi, Channel *chan, const char *fmt, ...);
	virtual void SendGlobops(BotInfo *source, const char *fmt, ...);
	virtual void SendSQLine(XLine *x) = 0;
	virtual void SendSquit(const char *servname, const char *message);
	virtual void SendSVSO(const char *, const char *, const char *) { }
	virtual void SendChangeBotNick(BotInfo *bi, const char *newnick);
	virtual void SendForceNickChange(User *u, const char *newnick, time_t when);
	virtual void SendVhost(User *, const std::string &, const std::string &) { }
	virtual void SendConnect() = 0;
	virtual void SendSVSHold(const char *) { }
	virtual void SendSVSHoldDel(const char *) { }
	virtual void SendSGLineDel(XLine *) { }
	virtual void SendSZLineDel(XLine *) { }
	virtual void SendSZLine(XLine *) { }
	virtual void SendSGLine(XLine *) { }
	virtual void SendBanDel(Channel *, const std::string &) { }
	virtual void SendSVSModeChan(Channel *, const char *, const char *) { }
	virtual void SendUnregisteredNick(User *) { }
	virtual void SendCTCP(BotInfo *bi, const char *dest, const char *fmt, ...);
	virtual void SendSVSJoin(const char *, const char *, const char *, const char *) { }
	virtual void SendSVSPart(const char *, const char *, const char *) { }
	virtual void SendSWhois(const char *, const char *, const char *) { }
	virtual void SendEOB() { }
	virtual void SendServer(Server *) = 0;
	virtual int IsNickValid(const char *) { return 1; }
	virtual int IsChannelValid(const char *);
	virtual void SendNumeric(const char *source, int numeric, const char *dest, const char *fmt, ...);

	/** Sends a message logging a user into an account, where ircds support such a feature.
	 * @param u The user logging in
	 * @param account The account the user is logging into
	 */
	virtual void SendAccountLogin(User *u, NickCore *account) { }

	/** Sends a message logging a user out of an account, where ircds support such a feature.
	 * @param u The user logging out
	 * @param account The account the user is logging out of
	 */
	virtual void SendAccountLogout(User *u, NickCore *account) { }

	/** Set a users auto identification token
	 * @param u The user
	 */
	virtual void SetAutoIdentificationToken(User *u) { }
};

class IRCDTS6Proto : public IRCDProto
{
};

/*************************************************************************/

struct Uplink
{
	char *host;
	unsigned port;
	char *password;
	bool ipv6;

	Uplink(const char *_host, int _port, const char *_password, bool _ipv6)
	{
		host = sstrdup(_host);
		port = _port;
		password = sstrdup(_password);
		ipv6 = _ipv6;
	}
	~Uplink()
	{
		delete [] host;
		delete [] password;
	}
};

enum LogLevel
{
	LOG_NORMAL,
	LOG_TERMINAL,
	LOG_DEBUG,
	LOG_DEBUG_2,
	LOG_DEBUG_3,
	LOG_DEBUG_4
};

class CoreExport Alog
{
 private:
	std::stringstream buf;
	LogLevel Level;
 public:
	Alog(LogLevel val = LOG_NORMAL);
	~Alog();
	template<typename T> Alog &operator<<(T val)
	{
		buf << val;
		return *this;
	}
};

struct Message; // XXX

class CoreExport Anope
{
 public:
	/** Check whether two strings match.
	 * @param str The string to check against the pattern (e.g. foobar)
	 * @param mask The pattern to check (e.g. foo*bar)
	 * @param case_sensitive Whether or not the match is case sensitive, default false.
	 */
	static bool Match(const std::string &str, const std::string &mask, bool case_sensitive = false);
	inline static bool Match(const ci::string &str, const ci::string &mask) { return Match(str.c_str(), mask.c_str(), false); }

	/** Add a message to Anope
	 * @param name The message name as sent by the IRCd
	 * @param func A callback function that will be called when this message is received
	 * @return The new message object
	 */
	static Message *AddMessage(const std::string &name, int (*func)(const char *source, int ac, const char **av));

	/** Deletes a message from Anope
	 * XXX Im not sure what will happen if this function is called indirectly from message function pointed to by this message.. must check
	 * @param m The message
	 * @return true if the message was found and deleted, else false
	 */
	static bool DelMessage(Message *m);
};

/*************************************************************************/

#include "timers.h"

/** Timer for colliding nicks to force people off of nicknames
 */
class NickServCollide : public Timer
{
	/* The nick */
	std::string nick;

 public:
	/** Default constructor
	 * @param _nick The nick were colliding
	 * @param delay How long to delay before kicking the user off the nick
	 */
	NickServCollide(const std::string &_nick, time_t delay);

	/** Default destructor
	 */
	virtual ~NickServCollide();

	/** Called when the delay is up
	 * @param t The current time
	 */
	void Tick(time_t t);
};

/** Timers for releasing nicks to be available for use
 */
class NickServRelease : public Timer
{
	/* The nick */
	std::string nick;

 public:
	/* The uid of the services enforcer client (used for TS6 ircds) */
	std::string uid;

	/** Default constructor
	 * @param _nick The nick
	 * @param _uid the uid of the enforcer, if any
	 * @param delay The delay before the nick is released
	 */
	NickServRelease(const std::string &_nick, const std::string &_uid, time_t delay);

	/** Default destructor
	 */
	virtual ~NickServRelease();

	/** Called when the delay is up
	 * @param t The current time
	 */
	void Tick(time_t t);
};

/** A timer used to keep the BotServ bot/ChanServ in the channel
 * after kicking the last user in a channel
 */
class ChanServTimer : public Timer
{
 private:
	Channel *c;

 public:
 	/** Default constructor
	 * @param chan The channel
	 */
	ChanServTimer(Channel *chan);

	/** Called when th edelay is up
	 * @param The current time
	 */
	void Tick(time_t);
};

/** A class to process numbered lists (passed to most DEL/LIST/VIEW commands).
 * The function HandleNumber is called for every number in the list. Note that
 * if descending is true it gets called in descending order. This is so deleting
 * the index passed to the function from an array will not cause the other indexes
 * passed to the function to be incorrect. This keeps us from having to have an
 * 'in use' flag on everything.
 */
class NumberList
{
 private:
	std::set<unsigned> numbers;

	bool desc;
 public:
	/** Processes a numbered list
	 * @param list The list
	 * @param descending True to make HandleNumber get called with numbers in descending order
	 */
	NumberList(const std::string &list, bool descending);

	/** Destructor, does nothing
	 */
	virtual ~NumberList();

	/** Should be called after the constructors are done running. This calls the callbacks.
	 */
	void Process();

	/** Called with a number from the list
	 * @param Number The number
	 */
	virtual void HandleNumber(unsigned Number);

	/** Called when there is an error with the numbered list
	 * Return false to immediatly stop processing the list and return
	 * This is all done before we start calling HandleNumber, so no numbers will have been processed yet
	 * @param list The list
	 * @return false to stop processing
	 */
	virtual bool InvalidRange(const std::string &list);
};

#endif /* SERVICES_H */
