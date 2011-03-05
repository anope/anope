/*
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
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
# ifndef INADDR_NONE
#  define INADDR_NONE (-1)
# endif
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <string.h>

#include <signal.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#include <sys/stat.h> /* for umask() on some systems */
#include <sys/types.h>
#include <fcntl.h>
#include <typeinfo>

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
# define setenv(x, y, z) SetEnvironmentVariable(x, y)
# define unsetenv(x) SetEnvironmentVariable(x, NULL)
# define MARK_DEPRECATED

extern CoreExport USHORT WindowsGetLanguage(const char *lang);
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

#if GETTEXT_FOUND
# include <libintl.h>
# define _(x) anope_gettext(x)
#else
# define _(x) x
#endif

/** This definition is used as shorthand for the various classes
 * and functions needed to make a module loadable by the OS.
 * It defines the class factory and external AnopeInit and AnopeFini functions.
 */
#ifdef _WIN32
# define MODULE_INIT(x) \
	extern "C" DllExport Module *AnopeInit(const Anope::string &, const Anope::string &); \
	extern "C" Module *AnopeInit(const Anope::string &modname, const Anope::string &creator) \
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
	extern "C" DllExport Module *AnopeInit(const Anope::string &modname, const Anope::string &creator) \
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
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <exception>
#include <list>
#include <vector>
#include <deque>
#include <bitset>
#include <set>

#include "anope.h"
#include "patricia.h"

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
	Anope::string err;
	/** Source of the exception
	 */
	Anope::string source;
 public:
	/** Default constructor, just uses the error mesage 'Core threw an exception'.
	 */
	CoreException() : err("Core threw an exception"), source("The core") { }
	/** This constructor can be used to specify an error message before throwing.
	 */
	CoreException(const Anope::string &message) : err(message), source("The core") { }
	/** This constructor can be used to specify an error message before throwing,
	 * and to specify the source of the exception.
	 */
	CoreException(const Anope::string &message, const Anope::string &src) : err(message), source(src) { }
	/** This destructor solves world hunger, cancels the world debt, and causes the world to end.
	 * Actually no, it does nothing. Never mind.
	 * @throws Nothing!
	 */
	virtual ~CoreException() throw() { }
	/** Returns the reason for the exception.
	 * The module should probably put something informative here as the user will see this upon failure.
	 */
	virtual const Anope::string &GetReason() const
	{
		return err;
	}

	virtual const Anope::string &GetSource() const
	{
		return source;
	}
};

class FatalException : public CoreException
{
 public:
	FatalException(const Anope::string &reason = "") : CoreException(reason) { }

	virtual ~FatalException() throw() { }
};

class ModuleException : public CoreException
{
 public:
	/** Default constructor, just uses the error mesage 'Module threw an exception'.
	 */
	ModuleException() : CoreException("Module threw an exception", "A Module") { }

	/** This constructor can be used to specify an error message before throwing.
	 */
	ModuleException(const Anope::string &message) : CoreException(message, "A Module") { }
	/** This destructor solves world hunger, cancels the world debt, and causes the world to end.
	 * Actually no, it does nothing. Never mind.
	 * @throws Nothing!
	 */
	virtual ~ModuleException() throw() { }
};

class DatabaseException : public CoreException
{
 public:
	/** This constructor can be used to specify an error message before throwing.
	 * @param mmessage The exception
	 */
	DatabaseException(const Anope::string &message) : CoreException(message, "A database module") { }

	/** Destructor
	 * @throws Nothing
	 */
	virtual ~DatabaseException() throw() { }
};

/** Debug cast to be used instead of dynamic_cast, this uses dynamic_cast
 * for debug builds and static_cast on releass builds to speed up the program
 * because dynamic_cast relies on RTTI.
 */
template<typename T, typename O> inline T debug_cast(O ptr)
{
#ifdef DEBUG_BUILD
	T ret = dynamic_cast<T>(ptr);
	if (ptr != NULL && ret == NULL)
		throw CoreException(Anope::string("debug_cast<") + typeid(T).name() + ">(" + typeid(O).name() + ") fail");
	return ret;
#else
	return static_cast<T>(ptr);
#endif
}

/*************************************************************************/

/** Class with the ability to keep flags on items, they should extend from this
 * where T is an enum.
 */
template<typename T, size_t Size = 32> class Flags
{
 protected:
	std::bitset<Size> Flag_Values;
	const Anope::string *Flag_Strings;

 public:
 	Flags(const Anope::string *flag_strings) : Flag_Strings(flag_strings) { }

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
		return Flag_Values.test(Value);
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

	std::vector<Anope::string> ToString()
	{
		std::vector<Anope::string> ret;
		for (unsigned i = 0; this->Flag_Strings && !this->Flag_Strings[i].empty(); ++i)
			if (this->HasFlag(static_cast<T>(i)))
				ret.push_back(this->Flag_Strings[i]);
		return ret;
	}

	void FromString(const std::vector<Anope::string> &strings)
	{
		for (unsigned i = 0; this->Flag_Strings && !this->Flag_Strings[i].empty(); ++i)
			for (unsigned j = 0; j < strings.size(); ++j)
				if (this->Flag_Strings[i] == strings[j])
					this->SetFlag(static_cast<T>(i));
	}
};

#include "sockets.h"
#include "socketengine.h"
#include "extensible.h"
#include "timers.h"
#include "dns.h"

/*************************************************************************/

class ConvertException : public CoreException
{
 public:
	ConvertException(const Anope::string &reason = "") : CoreException(reason) { }

	virtual ~ConvertException() throw() { }
};

template<typename T> inline Anope::string stringify(const T &x)
{
	std::ostringstream stream;

	if (!(stream << x))
		throw ConvertException("Stringify fail");

	return stream.str();
}

template<typename T> inline void convert(const Anope::string &s, T &x, Anope::string &leftover, bool failIfLeftoverChars = true)
{
	leftover.clear();
	std::istringstream i(s.str());
	char c;
	bool res = i >> x;
	if (!res)
		throw ConvertException("Convert fail");
	if (failIfLeftoverChars)
	{
		if (i.get(c))
			throw ConvertException("Convert fail");
	}
	else
	{
		std::string left;
		getline(i, left);
		leftover = left;
	}
}

template<typename T> inline void convert(const Anope::string &s, T &x, bool failIfLeftoverChars = true)
{
	Anope::string Unused;
	convert(s, x, Unused, failIfLeftoverChars);
}

template<typename T> inline T convertTo(const Anope::string &s, Anope::string &leftover, bool failIfLeftoverChars = true)
{
	T x;
	convert(s, x, leftover, failIfLeftoverChars);
	return x;
}

template<typename T> inline T convertTo(const Anope::string &s, bool failIfLeftoverChars = true)
{
	T x;
	convert(s, x, failIfLeftoverChars);
	return x;
}

/*************************************************************************/

class User;
class NickCore;
class NickAlias;
class BotInfo;
class ChannelInfo;
class Channel;
class Server;
struct EList;
struct Session;

#include "threadengine.h"
#include "opertype.h"
#include "modes.h"

/*************************************************************************/

/* Protocol tweaks */

struct IRCDVar
{
	const char *name;				/* Name of the IRCd command */
	const char *pseudoclient_mode;	/* Mode used by BotServ Bots */
	int svsnick;					/* Supports SVSNICK */
	int vhost;						/* Supports vhost */
	int snline;						/* Supports SNline */
	int sqline;						/* Supports SQline */
	int szline;						/* Supports SZline */
	int join2msg;					/* Join 2 Message */
	int chansqline;					/* Supports Channel Sqlines */
	int quitonkill;					/* IRCD sends QUIT when kill */
	int vident;						/* Supports vidents */
	int svshold;					/* Supports svshold */
	int tsonmode;					/* Timestamp on mode changes */
	int omode;						/* On the fly o:lines */
	int umode;						/* change user modes */
	int knock_needs_i;				/* Check if we needed +i when setting NOKNOCK */
	int svsmode_ucmode;				/* Can remove User Channel Modes with SVSMODE */
	int sglineenforce;
	int ts6;						/* ircd is TS6 */
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
	/* Sender requests a receipt */
	MF_RECEIPT,
	/* Memo is a notification of receipt */
	MF_NOTIFYS
};

const Anope::string MemoFlagStrings[] = {
	"MF_UNREAD", "MF_RECEIPT", "MF_NOTIFYS", ""
};

/* Memo info structures.  Since both nicknames and channels can have memos,
 * we encapsulate memo data in a MemoList to make it easier to handle. */
class Memo : public Flags<MemoFlag>
{
 public:
	Memo();
	time_t time;	/* When it was sent */
	Anope::string sender;
	Anope::string text;
};

struct CoreExport MemoInfo
{
	int16 memomax;
	std::vector<Memo *> memos;
	std::vector<ci::string> ignores;

	unsigned GetIndex(Memo *m) const;
	void Del(unsigned index);
	void Del(Memo *m);
	bool HasIgnore(User *u);
};

/*************************************************************************/

class CoreExport HostInfo
{
 private:
	Anope::string Ident;
	Anope::string Host;
	Anope::string Creator;
	time_t Time;

 public:
 	/** Set a vhost for the user
	 * @param ident The ident
	 * @param host The host
	 * @param creator Who created the vhost
	 * @param time When the vhost was craated
	 */
	void SetVhost(const Anope::string &ident, const Anope::string &host, const Anope::string &creator, time_t created = Anope::CurTime);

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
	const Anope::string &GetIdent() const;

	/** Retrieve the vhost host
	 * @return the host
	 */
	const Anope::string &GetHost() const;

	/** Retrieve the vhost creator
	 * @return the creator
	 */
	const Anope::string &GetCreator() const;

	/** Retrieve when the vhost was crated
	 * @return the time it was created
	 */
	const time_t GetTime() const;
};

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
	Anope::string mask;	/* Mask of the access entry */
	NickCore *nc;		/* NC of the entry, if the entry is a valid nickcore */
	time_t last_seen;
	Anope::string creator;
};

/** Flags for auto kick
 */
enum AutoKickFlag
{
	/* Is by nick core, not mask */
	AK_ISNICK
};

const Anope::string AutoKickFlagString[] = { "AK_ISNICK", "" };

/* AutoKick data. */
class AutoKick : public Flags<AutoKickFlag>
{
 public:
 	AutoKick() : Flags<AutoKickFlag>(AutoKickFlagString) { }
	/* Only one of these can be in use */
	Anope::string mask;
	NickCore *nc;

	Anope::string reason;
	Anope::string creator;
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
	Anope::string word;
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
	CA_AUTOVOICE,
	CA_OPDEOP,		/* ChanServ commands OP and DEOP */
	CA_ACCESS_LIST,
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
	CA_MODE,
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
	BS_BEGIN,
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
	BS_KICK_REPEAT,
	/* BotServ kicks for italics */
	BS_KICK_ITALICS,
	/* BotServ kicks for amsgs */
	BS_KICK_AMSGS,
	/* Send fantasy replies back to the channel via PRIVMSG */
	BS_MSG_PRIVMSG,
	/* Send fantasy replies back to the channel via NOTICE */
	BS_MSG_NOTICE,
	/* Send fantasy replies back to the channel via NOTICE to ops */
	BS_MSG_NOTICEOPS,
	BS_END
};

const Anope::string BotServFlagStrings[] = {
	"BEGIN", "DONTKICKOPS", "DONTKICKVOICES", "FANTASY", "SYMBIOSIS", "GREET", "NOBOT",
	"KICK_BOLDs", "KICK_COLORS", "KICK_REVERSES", "KICK_UNDERLINES", "KICK_BADWORDS", "KICK_CAPS",
	"KICK_FLOOD", "KICK_REPEAT", "KICK_ITALICS", "KICK_AMSGS", "MSG_PRIVMSG", "MSG_NOTICE",
	"MSG_NOTICEOPS", ""
};

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
	TTB_ITALICS,
	TTB_AMSGS,
	TTB_SIZE
};

#include "regchannel.h"

/*************************************************************************/

/* This structure stocks ban data since it must not be removed when
 * user is kicked.
 */

#include "users.h"
#include "account.h"
#include "bots.h"

struct BanData
{
	Anope::string mask;	/* Since a nick is unsure and a User structure is unsafe */
	time_t last_use;	/* Since time is the only way to check whether it's still useful */
	int16 ttb[TTB_SIZE];
};

struct LevelInfo
{
	int what;
	Anope::string name;
	const char *desc;
};

#include "channels.h"

/** Channelban type flags
 */
enum EntryType
{
	ENTRYTYPE_NONE,
	ENTRYTYPE_CIDR,
	ENTRYTYPE_NICK_WILD,
	ENTRYTYPE_NICK,
	ENTRYTYPE_USER_WILD,
	ENTRYTYPE_USER,
	ENTRYTYPE_HOST_WILD,
	ENTRYTYPE_HOST
};

class CoreExport Entry : public Flags<EntryType>
{
	Anope::string mask;

 public:
	unsigned char cidr_len;
	Anope::string nick, user, host;

	/** Constructor
	 * @param _host A full nick!ident@host/cidr mask
	 */
	Entry(const Anope::string &_host);

	/** Get the banned mask for this entry
	 * @return The mask
	 */
	const Anope::string GetMask();

	/** Check if this entry matches a user
	 * @param u The user
	 * @param full True to match against a users real host and IP
	 * @return true on match
	 */
	bool Matches(User *u, bool full = false) const;
};

/*************************************************************************/

/* News stuff */

enum NewsType
{
	NEWS_LOGON,
	NEWS_RANDOM,
	NEWS_OPER
};

struct NewsMessages
{
	NewsType type;
	Anope::string name;
	const char *msgs[10];
};

struct NewsItem
{
	NewsType type;
	uint32 num;
	Anope::string Text;
	Anope::string who;
	time_t time;
};

/*************************************************************************/

/* Mail data */

struct MailInfo
{
	FILE *pipe;
	User *sender;
	NickCore *recipient;
};

/*************************************************************************/

struct Exception
{
	Anope::string mask;		/* Hosts to which this exception applies */
	unsigned limit;				/* Session limit for exception */
	Anope::string who;		/* Nick of person who added the exception */
	Anope::string reason;	/* Reason for exception's addition */
	time_t time;			/* When this exception was added */
	time_t expires;			/* Time when it expires. 0 == no expiry */
};

/*************************************************************************/

extern CoreExport patricia_tree<Session *> SessionList;

struct Session
{
	Anope::string host;
	unsigned count;	/* Number of clients with this host */
	unsigned hits;	/* Number of subsequent kills for a host */
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
class IRCdMessage;
struct Uplink;
class ServerConfig;

#include "extern.h"
#include "language.h"
#include "operserv.h"
#include "mail.h"
#include "servers.h"
#include "logger.h"
#include "config.h"

class CoreExport IRCDProto
{
 private:
	virtual void SendSVSKillInternal(const BotInfo *, const User *, const Anope::string &) = 0;
	virtual void SendModeInternal(const BotInfo *, const Channel *, const Anope::string &) = 0;
	virtual void SendModeInternal(const BotInfo *, const User *, const Anope::string &) = 0;
	virtual void SendKickInternal(const BotInfo *, const Channel *, const User *, const Anope::string &) = 0;
	virtual void SendNoticeChanopsInternal(const BotInfo *bi, const Channel *, const Anope::string &) = 0;
	virtual void SendMessageInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &buf);
	virtual void SendNoticeInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &msg);
	virtual void SendPrivmsgInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &buf);
	virtual void SendQuitInternal(const User *u, const Anope::string &buf);
	virtual void SendPartInternal(const BotInfo *bi, const Channel *chan, const Anope::string &buf);
	virtual void SendGlobopsInternal(const BotInfo *source, const Anope::string &buf);
	virtual void SendCTCPInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &buf);
	virtual void SendNumericInternal(const Anope::string &source, int numeric, const Anope::string &dest, const Anope::string &buf);
 public:
	virtual ~IRCDProto() { }

	virtual void SendSVSNOOP(const Anope::string &, int) { }
	virtual void SendTopic(BotInfo *, Channel *) = 0;
	virtual void SendVhostDel(User *) { }
	virtual void SendAkill(User *, const XLine *) = 0;
	virtual void SendAkillDel(const XLine *) = 0;
	virtual void SendSVSKill(const BotInfo *source, const User *user, const char *fmt, ...);
	virtual void SendMode(const BotInfo *bi, const Channel *dest, const char *fmt, ...);
	virtual void SendMode(const BotInfo *bi, const User *u, const char *fmt, ...);
	virtual void SendClientIntroduction(const User *u, const Anope::string &) = 0;
	virtual void SendKick(const BotInfo *bi, const Channel *chan, const User *user, const char *fmt, ...);
	virtual void SendNoticeChanops(const BotInfo *bi, const Channel *dest, const char *fmt, ...);
	virtual void SendMessage(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendNotice(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendAction(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendPrivmsg(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg);
	virtual void SendGlobalPrivmsg(const BotInfo *bi, const Server *desc, const Anope::string &msg);

	virtual void SendQuit(const User *u, const char *fmt, ...);
	virtual void SendPing(const Anope::string &servname, const Anope::string &who);
	virtual void SendPong(const Anope::string &servname, const Anope::string &who);
	virtual void SendJoin(BotInfo *, Channel *, const ChannelStatus *) = 0;
	virtual void SendSQLineDel(const XLine *x) { }
	virtual void SendInvite(const BotInfo *bi, const Anope::string &chan, const Anope::string &nick);
	virtual void SendPart(const BotInfo *bi, const Channel *chan, const char *fmt, ...);
	virtual void SendGlobops(const BotInfo *source, const char *fmt, ...);
	virtual void SendSQLine(User *, const XLine *x) { }
	virtual void SendSquit(const Anope::string &servname, const Anope::string &message);
	virtual void SendSVSO(const Anope::string &, const Anope::string &, const Anope::string &) { }
	virtual void SendChangeBotNick(const BotInfo *bi, const Anope::string &newnick);
	virtual void SendForceNickChange(const User *u, const Anope::string &newnick, time_t when);
	virtual void SendVhost(User *, const Anope::string &, const Anope::string &) { }
	virtual void SendConnect() = 0;
	virtual void SendSVSHold(const Anope::string &) { }
	virtual void SendSVSHoldDel(const Anope::string &) { }
	virtual void SendSGLineDel(const XLine *) { }
	virtual void SendSZLineDel(const XLine *) { }
	virtual void SendSZLine(User *u, const XLine *) { }
	virtual void SendSGLine(User *, const XLine *) { }
	virtual void SendUnregisteredNick(const User *) { }
	virtual void SendCTCP(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendSVSJoin(const Anope::string &, const Anope::string &, const Anope::string &, const Anope::string &) { }
	virtual void SendSWhois(const Anope::string &, const Anope::string &, const Anope::string &) { }
	virtual void SendBOB() { }
	virtual void SendEOB() { }
	virtual void SendServer(const Server *) = 0;
	virtual bool IsNickValid(const Anope::string &) { return true; }
	virtual bool IsChannelValid(const Anope::string &);
	virtual void SendNumeric(const Anope::string &source, int numeric, const Anope::string &dest, const char *fmt, ...);

	/** Sends a message logging a user into an account, where ircds support such a feature.
	 * @param u The user logging in
	 * @param account The account the user is logging into
	 */
	virtual void SendAccountLogin(const User *u, const NickCore *account) { }

	/** Sends a message logging a user out of an account, where ircds support such a feature.
	 * @param u The user logging out
	 * @param account The account the user is logging out of
	 */
	virtual void SendAccountLogout(const User *u, const NickCore *account) { }

	/** Set a users auto identification token
	 * @param u The user
	 */
	virtual void SetAutoIdentificationToken(User *u) { }

	/** Send a channel creation message to the uplink.
	 * On most TS6 IRCds this is a SJOIN with no nick
	 */
	virtual void SendChannel(Channel *c) { }
};

class CoreExport IRCdMessage
{
 public:
	virtual bool On436(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnAway(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnJoin(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnKick(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnKill(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnMode(const Anope::string &, const std::vector<Anope::string> &) = 0;
	virtual bool OnUID(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnNick(const Anope::string &, const std::vector<Anope::string> &) = 0;
	virtual bool OnPart(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnPing(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnPrivmsg(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnQuit(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnServer(const Anope::string &, const std::vector<Anope::string> &) = 0;
	virtual bool OnSQuit(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnTopic(const Anope::string &, const std::vector<Anope::string> &) = 0;
	virtual bool OnWhois(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnCapab(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnSJoin(const Anope::string &, const std::vector<Anope::string> &) = 0;
	virtual bool OnError(const Anope::string &, const std::vector<Anope::string> &);
};

/*************************************************************************/

struct Uplink
{
	Anope::string host;
	unsigned port;
	Anope::string password;
	bool ipv6;

	Uplink(const Anope::string &_host, int _port, const Anope::string &_password, bool _ipv6) : host(_host), port(_port), password(_password), ipv6(_ipv6) { }
};

/** A class to process numbered lists (passed to most DEL/LIST/VIEW commands).
 * The function HandleNumber is called for every number in the list. Note that
 * if descending is true it gets called in descending order. This is so deleting
 * the index passed to the function from an array will not cause the other indexes
 * passed to the function to be incorrect. This keeps us from having to have an
 * 'in use' flag on everything.
 */
class CoreExport NumberList
{
 private:
	bool is_valid;

	std::set<unsigned> numbers;

	bool desc;
 public:
	/** Processes a numbered list
	 * @param list The list
	 * @param descending True to make HandleNumber get called with numbers in descending order
	 */
	NumberList(const Anope::string &list, bool descending);

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
	virtual bool InvalidRange(const Anope::string &list);
};

#endif /* SERVICES_H */
