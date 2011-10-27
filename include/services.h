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
#include <sys/wait.h>
#include <fcntl.h>
#include <typeinfo>

#if GETTEXT_FOUND
# include <libintl.h>
#endif
#define _(x) x

#ifndef _WIN32
# include <unistd.h>
# include <grp.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <dirent.h>
# define DllExport
# define CoreExport
# define MARK_DEPRECATED __attribute((deprecated))
# define DeleteFile unlink
#else
# include "anope_windows.h"
#endif

/* Telling compilers about printf()-like functions: */
#ifdef __GNUC__
# define FORMAT(type, fmt, start) __attribute__((format(type, fmt, start)))
#else
# define FORMAT(type, fmt, start)
#endif

#if HAVE_STRINGS_H
# include <strings.h>
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
extern int toupper(char);
extern int tolower(char);

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
	Flags() : Flag_Strings(NULL) { }
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

	Anope::string ToString()
	{
		std::vector<Anope::string> v = ToVector();
		Anope::string flag_buf;
		for (unsigned i = 0; i < v.size(); ++i)
			flag_buf += v[i] + " ";
		flag_buf.trim();
		return flag_buf;
	}

	void FromString(const Anope::string &str)
	{
		spacesepstream sep(str);
		Anope::string buf;
		std::vector<Anope::string> v;

		while (sep.GetToken(buf))
			v.push_back(buf);

		FromVector(v);
	}

	std::vector<Anope::string> ToVector()
	{
		std::vector<Anope::string> ret;
		for (unsigned i = 0; this->Flag_Strings && !this->Flag_Strings[i].empty(); ++i)
			if (this->HasFlag(static_cast<T>(i)))
				ret.push_back(this->Flag_Strings[i]);
		return ret;
	}

	void FromVector(const std::vector<Anope::string> &strings)
	{
		this->ClearFlags();

		for (unsigned i = 0; this->Flag_Strings && !this->Flag_Strings[i].empty(); ++i)
			for (unsigned j = 0; j < strings.size(); ++j)
				if (this->Flag_Strings[i] == strings[j])
					this->SetFlag(static_cast<T>(i));
	}
};

class Module;

template<typename T> class CoreExport Service : public Base
{
	static Anope::map<T *> services;
 public:
 	static T* FindService(const Anope::string &n)
	{
		typename Anope::map<T *>::iterator it = Service<T>::services.find(n);
		if (it != Service<T>::services.end())
			return it->second;
		return NULL;
	}

	static std::vector<Anope::string> GetServiceKeys()
	{
		std::vector<Anope::string> keys;
		for (typename Anope::map<T *>::iterator it = Service<T>::services.begin(), it_end = Service<T>::services.end(); it != it_end; ++it)
			keys.push_back(it->first);
		return keys;
	}

	Module *owner;
	Anope::string name;

	Service(Module *o, const Anope::string &n) : owner(o), name(n)
	{
		this->Register();
	}

	virtual ~Service()
	{
		this->Unregister();
	}

	void Register()
	{
		if (Service<T>::services.find(this->name) != Service<T>::services.end())
			throw ModuleException("Service with name " + this->name + " already exists");
		Service<T>::services[this->name] = static_cast<T *>(this);
	}

	void Unregister()
	{
		Service<T>::services.erase(this->name);
	}
};
template<typename T> Anope::map<T *> Service<T>::services;

template class Service<Base>;

#include "sockets.h"
#include "socketengine.h"
#include "extensible.h"
#include "timers.h"
#include "dns.h"

/*************************************************************************/

class Signal : public Pipe
{
	static std::vector<Signal *> SignalHandlers;
	static void SignalHandler(int signal);

	struct sigaction action, old;
 public:
	int signal;

	Signal(int s);
	~Signal();

	virtual void OnNotify() = 0;
};

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
class Entry;

#include "threadengine.h"
#include "opertype.h"
#include "modes.h"
#include "serialize.h"

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
	int certfp;					/* IRCd sends a SSL users certificate fingerprint */
};

/*************************************************************************/

/** Memo Flags
 */
enum MemoFlag
{
	/* Memo is unread */
	MF_UNREAD,
	/* Sender requests a receipt */
	MF_RECEIPT
};

const Anope::string MemoFlagStrings[] = {
	"MF_UNREAD", "MF_RECEIPT", ""
};

/* Memo info structures.  Since both nicknames and channels can have memos,
 * we encapsulate memo data in a MemoList to make it easier to handle. */
class CoreExport Memo : public Flags<MemoFlag>, public Serializable<Memo>
{
 public:
 	Memo();
	serialized_data serialize();
	static void unserialize(serialized_data &);

	Anope::string owner;
	time_t time;	/* When it was sent */
	Anope::string sender;
	Anope::string text;
};

struct CoreExport MemoInfo
{
	int16_t memomax;
	std::vector<Memo *> memos;
	std::vector<Anope::string> ignores;

	unsigned GetIndex(Memo *m) const;
	void Del(unsigned index);
	void Del(Memo *m);
	bool HasIgnore(User *u);
};

struct Session
{
	Anope::string host;		/* Host of the session */
	unsigned count;			/* Number of clients with this host */
	unsigned hits;			/* Number of subsequent kills for a host */
};

struct Exception;

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
	time_t GetTime() const;
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
	BS_END
};

const Anope::string BotServFlagStrings[] = {
	"BEGIN", "DONTKICKOPS", "DONTKICKVOICES", "FANTASY", "GREET", "NOBOT",
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

#include "access.h"
#include "regchannel.h"

/*************************************************************************/

/* This structure stocks ban data since it must not be removed when
 * user is kicked.
 */

#include "users.h"
#include "account.h"
#include "commands.h"
#include "bots.h"
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
	ChannelModeName modename;

 public:
	unsigned char cidr_len;
	Anope::string mask;
	Anope::string nick, user, host;

	/** Constructor
	 * @param _host A full nick!ident@host/cidr mask
 	 * @param mode What mode this host is for - can be CMODE_BEGIN for unknown/no mode
	 */
	Entry(ChannelModeName mode, const Anope::string &_host);

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

/* Mail data */

struct MailInfo
{
	FILE *pipe;
	User *sender;
	NickCore *recipient;
};

/*************************************************************************/

/* Defcon */

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
class ConfigurationFile;
class XLine;

#include "extern.h"
#include "language.h"
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

	virtual void SendSVSNOOP(const Server *, bool) { }
	virtual void SendTopic(BotInfo *, Channel *) = 0;
	virtual void SendVhostDel(User *) { }
	virtual void SendAkill(User *, const XLine *) = 0;
	virtual void SendAkillDel(const XLine *) = 0;
	virtual void SendSVSKill(const BotInfo *source, const User *user, const char *fmt, ...);
	virtual void SendMode(const BotInfo *bi, const Channel *dest, const char *fmt, ...);
	virtual void SendMode(const BotInfo *bi, const User *u, const char *fmt, ...);
	virtual void SendClientIntroduction(const User *u) = 0;
	virtual void SendKick(const BotInfo *bi, const Channel *chan, const User *user, const char *fmt, ...);
	virtual void SendMessage(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendNotice(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendAction(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendPrivmsg(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg);
	virtual void SendGlobalPrivmsg(const BotInfo *bi, const Server *desc, const Anope::string &msg);

	virtual void SendQuit(const User *u, const char *fmt, ...);
	virtual void SendPing(const Anope::string &servname, const Anope::string &who);
	virtual void SendPong(const Anope::string &servname, const Anope::string &who);
	virtual void SendJoin(User *, Channel *, const ChannelStatus *) = 0;
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
	virtual void SendCTCP(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendSVSJoin(const Anope::string &, const Anope::string &, const Anope::string &, const Anope::string &) { }
	virtual void SendSWhois(const Anope::string &, const Anope::string &, const Anope::string &) { }
	virtual void SendBOB() { }
	virtual void SendEOB() { }
	virtual void SendServer(const Server *) = 0;
	virtual bool IsNickValid(const Anope::string &) { return true; }
	virtual bool IsChannelValid(const Anope::string &);
	virtual void SendNumeric(const Anope::string &source, int numeric, const Anope::string &dest, const char *fmt, ...);
	virtual void SendLogin(User *u) = 0;
	virtual void SendLogout(User *u) = 0;

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
	bool operator==(const Uplink &other) const
	{
		if (this->host != other.host)
			return false;
		if (this->port != other.port)
			return false;
		if (this->password != other.password)
			return false;
		if (this->ipv6 != other.ipv6)
			return false;
		return true;
	}
	inline bool operator!=(const Uplink &other) const { return !(*this == other); }
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
