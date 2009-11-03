/*
 *
 * (C) 2003-2009 Anope Team
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
#include "config.h"

#ifndef MAX_CMD_HASH
#define MAX_CMD_HASH 1024
#endif

/* Some SUN fixs */
#ifdef __sun
/* Solaris specific code, types that do not exist in Solaris'
 * sys/types.h
 **/
#undef u_int8_t
#undef u_int16_t
#undef u_int32_t
#undef u_int_64_t
#define u_int8_t uint8_t
#define u_int16_t uint16_t
#define u_int32_t uint32_t
#define u_int64_t uint64_t

#ifndef INADDR_NONE
#define INADDR_NONE (-1)
#endif

#endif


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Windows does not have: unistd.h, grp.h, netdb.h, netinet/in.h, sys/socket.h, sys/time.h
 * Windows requires: winsock.h
 * -- codemastr
 */

#ifndef _WIN32
#include <unistd.h>
#endif

#include <signal.h>
#include <time.h>
#include <errno.h>

#ifndef _WIN32
#include <grp.h>
#endif

#include <limits.h>

#ifndef _WIN32
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#else
#include <winsock.h>
#include <windows.h>
#endif

#include <sys/stat.h>	/* for umask() on some systems */
#include <sys/types.h>

#ifdef HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif

#ifdef _WIN32
#include <sys/timeb.h>
#include <direct.h>
#include <io.h>
#endif

#include <fcntl.h>

#ifndef _WIN32
#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#endif
#endif

#ifndef _WIN32
#include <dirent.h>
#endif

#ifdef _WIN32
/* VS2008 hates having this define before its own */
#define vsnprintf			   _vsnprintf
#endif

#if HAVE_STRINGS_H
# include <strings.h>
#endif

#if HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

#include "sockets.h"

#ifndef va_copy
# ifdef __va_copy
#  define VA_COPY(DEST,SRC) __va_copy((DEST),(SRC))
# else
#  define VA_COPY(DEST, SRC) memcpy ((&DEST), (&SRC), sizeof(va_list))
# endif
#else
# define VA_COPY(DEST, SRC) va_copy(DEST, SRC)
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

/* We also have our own encrypt(). */
#define encrypt encrypt_


#ifdef __WINS__
#ifndef BKCHECK
#define BKCHECK
  extern "C" void __pfnBkCheck() {}
#endif
#endif


#if INTTYPE_WORKAROUND
# undef int16
# undef int32
#endif

#ifndef _WIN32
	#define MARK_DEPRECATED __attribute((deprecated))
#else
	#define MARK_DEPRECATED
#endif

#ifdef _WIN32
# ifdef MODULE_COMPILE
#  define CoreExport __declspec(dllimport)
#  define DllExport __declspec(dllexport)
# else
#  define CoreExport __declspec(dllexport)
#  define DllExport __declspec(dllimport)
# endif
#else
# define DllExport
# define CoreExport
#endif

/** This definition is used as shorthand for the various classes
 * and functions needed to make a module loadable by the OS.
 * It defines the class factory and external init_module function.
 */
#ifdef _WIN32
	#define MODULE_INIT(x) \
		extern "C" DllExport Module *init_module(const std::string &, const std::string &); \
		extern "C" Module *init_module(const std::string &modname, const std::string &creator) \
		{ \
			return new x(modname, creator); \
		} \
		BOOLEAN WINAPI DllMain(HINSTANCE, DWORD nReason, LPVOID) \
		{ \
			switch ( nReason ) \
			{ \
				case DLL_PROCESS_ATTACH: \
				case DLL_PROCESS_DETACH: \
					break; \
			} \
			return TRUE; \
		} \
		extern "C" DllExport void destroy_module(x *); \
		extern "C" void destroy_module(x *m) \
		{ \
			delete m; \
		}

#else
	#define MODULE_INIT(x) \
		extern "C" DllExport Module *init_module(const std::string &modname, const std::string &creator) \
		{ \
			return new x(modname, creator); \
		} \
		extern "C" DllExport void destroy_module(x *m) \
		{ \
			delete m; \
		}
#endif

/* Miscellaneous definitions. */
#include "defs.h"
#include "slist.h"

/* pull in the various bits of STL to pull in */
#include <string>
#include <map>
#include <exception>
#include <list>
#include <vector>
#include <deque>
#include <bitset>

/** This class can be used on its own to represent an exception, or derived to represent a module-specific exception.
 * When a module whishes to abort, e.g. within a constructor, it should throw an exception using ModuleException or
 * a class derived from ModuleException. If a module throws an exception during its constructor, the module will not
 * be loaded. If this happens, the error message returned by ModuleException::GetReason will be displayed to the user
 * attempting to load the module, or dumped to the console if the ircd is currently loading for the first time.
 */
class CoreExport CoreException : public std::exception
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
	CoreException(const std::string &message) : err(message), source("The core") {}
	/** This constructor can be used to specify an error message before throwing,
	 * and to specify the source of the exception.
	 */
	CoreException(const std::string &message, const std::string &src) : err(message), source(src) {}
	/** This destructor solves world hunger, cancels the world debt, and causes the world to end.
	 * Actually no, it does nothing. Never mind.
	 * @throws Nothing!
	 */
	virtual ~CoreException() throw() {}
	/** Returns the reason for the exception.
	 * The module should probably put something informative here as the user will see this upon failure.
	 */
	virtual const char* GetReason() const
	{
		return err.c_str();
	}

	virtual const char* GetSource() const
	{
		return source.c_str();
	}
};

class CoreExport ModuleException : public CoreException
{
 public:
	/** Default constructor, just uses the error mesage 'Module threw an exception'.
	 */
	ModuleException() : CoreException("Module threw an exception", "A Module") {}

	/** This constructor can be used to specify an error message before throwing.
	 */
	ModuleException(const std::string &message) : CoreException(message, "A Module") {}
	/** This destructor solves world hunger, cancels the world debt, and causes the world to end.
	 * Actually no, it does nothing. Never mind.
	 * @throws Nothing!
	 */
	virtual ~ModuleException() throw() {}
};

/** Class with the ability to be extended with key:value pairs.
 * Thanks to InspIRCd for this.
 */
class CoreExport Extensible
{
 private:
	std::map<std::string, void *> Extension_Items;

 public:
	/** Extend an Extensible class.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @param p This parameter is a pointer to any data you wish to associate with the object
	 *
	 * You must provide a key to store the data as via the parameter 'key' and store the data
	 * in the templated parameter 'p'.
	 * The data will be inserted into the map. If the data already exists, you may not insert it
	 * twice, Extensible::Extend will return false in this case.
	 *
	 * @return Returns true on success, false if otherwise
	 */
	template<typename T> bool Extend(const std::string &key, T *p)
	{
		/* This will only add an item if it doesnt already exist,
		 * the return value is a std::pair of an iterator to the
		 * element, and a bool saying if it was actually inserted.
		 */
		return this->Extension_Items.insert(std::make_pair(key, reinterpret_cast<char *>(p))).second;
	}

	/** Extend an Extensible class.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 *
	 * You must provide a key to store the data as via the parameter 'key', this single-parameter
	 * version takes no 'data' parameter, this is used purely for boolean values.
	 * The key will be inserted into the map with a NULL 'data' pointer. If the key already exists
	 * then you may not insert it twice, Extensible::Extend will return false in this case.
	 *
	 * @return Returns true on success, false if otherwise
	 */
	bool Extend(const std::string &key)
	{
		/* This will only add an item if it doesnt already exist,
		 * the return value is a std::pair of an iterator to the
		 * element, and a bool saying if it was actually inserted.
		 */
		return this->Extension_Items.insert(std::make_pair(key, static_cast<char *>(NULL))).second;
	}

	/** Shrink an Extensible class.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 *
	 * You must provide a key name. The given key name will be removed from the classes data. If
	 * you provide a nonexistent key (case is important) then the function will return false.
	 * @return Returns true on success.
	 */
	bool Shrink(const std::string &key)
	{
		/* map::size_type map::erase( const key_type& key );
		 * returns the number of elements removed, std::map
		 * is single-associative so this should only be 0 or 1
		 */
		return this->Extension_Items.erase(key);
	}

	/** Get an extension item.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @param p If you provide a non-existent key, this value will be NULL. Otherwise a pointer to the item you requested will be placed in this templated parameter.
	 * @return Returns true if the item was found and false if it was nor, regardless of wether 'p' is NULL. This allows you to store NULL values in Extensible.
	 */
	template<typename T> bool GetExt(const std::string &key, T *&p)
	{
		std::map<std::string, void *>::iterator iter = this->Extension_Items.find(key); /* Find the item */
		if(iter != this->Extension_Items.end())
		{
			p = static_cast<T *>(iter->second);	/* Item found */
			return true;
		}
		else
		{
			p = NULL;		/* Item not found */
			return false;
		}
	}

	/** Get an extension item.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @return Returns true if the item was found and false if it was not.
	 *
	 * This single-parameter version only checks if the key exists, it does nothing with
	 * the 'data' field and is probably only useful in conjunction with the single-parameter
	 * version of Extend().
	 */
	bool GetExt(const std::string &key)
	{
		return (this->Extension_Items.find(key) != this->Extension_Items.end());
	}

	/** Get a list of all extension items names.
	 * @param list A deque of strings to receive the list
	 * @return This function writes a list of all extension items stored
	 *         in this object by name into the given deque and returns void.
	 */
	void GetExtList(std::deque<std::string> &list)
	{
		for (std::map<std::string, void *>::iterator i = Extension_Items.begin(); i != Extension_Items.end(); ++i)
		{
			list.push_back(i->first);
		}
	}

};

/*************************************************************************/

/* forward declarations, mostly used by older code */
class User;
class ChannelInfo;
class Channel;

typedef struct server_ Server;
typedef struct c_elist EList;
typedef struct c_elist_entry Entry;
typedef struct memo_ Memo;
typedef struct badword_ BadWord;
typedef struct bandata_ BanData;
typedef struct userdata_ UserData;
typedef struct mailinfo_ MailInfo;
typedef struct akill_ Akill;
typedef struct sxline_ SXLine;
typedef struct hostcore_ HostCore;
typedef struct exception_ Exception;
typedef struct session_ Session;

enum UserModeName
{
	UMODE_BEGIN,

	/* Usermodes */
	UMODE_SERV_ADMIN, UMODE_BOT, UMODE_CO_ADMIN, UMODE_FILTER, UMODE_HIDEOPER, UMODE_NETADMIN,
	UMODE_REGPRIV, UMODE_PROTECTED, UMODE_NO_CTCP, UMODE_WEBTV, UMODE_WHOIS, UMODE_ADMIN, UMODE_DEAF,
	UMODE_GLOBOPS, UMODE_HELPOP, UMODE_INVIS, UMODE_OPER, UMODE_PRIV, UMODE_GOD, UMODE_REGISTERED,
	UMODE_SNOMASK, UMODE_VHOST, UMODE_WALLOPS, UMODE_CLOAK, UMODE_SSL, UMODE_CALLERID, UMODE_COMMONCHANS,
	UMODE_HIDDEN, UMODE_STRIPCOLOR,

	UMODE_END
};

enum ChannelModeName
{
	CMODE_BEGIN,

	/* Channel modes */
	CMODE_BLOCKCOLOR, CMODE_FLOOD, CMODE_INVITE, CMODE_KEY, CMODE_LIMIT, CMODE_MODERATED, CMODE_NOEXTERNAL,
	CMODE_PRIVATE, CMODE_REGISTERED, CMODE_SECRET, CMODE_TOPIC, CMODE_AUDITORIUM, CMODE_SSL, CMODE_ADMINONLY,
	CMODE_NOCTCP, CMODE_FILTER, CMODE_NOKNOCK, CMODE_REDIRECT, CMODE_REGMODERATED, CMODE_NONICK, CMODE_OPERONLY,
	CMODE_NOKICK, CMODE_REGISTEREDONLY, CMODE_STRIPCOLOR, CMODE_NONOTICE, CMODE_NOINVITE, CMODE_ALLINVITE,
	CMODE_BLOCKCAPS, CMODE_PERM, CMODE_NICKFLOOD, CMODE_JOINFLOOD,

	/* b/e/I */
	CMODE_BAN, CMODE_EXCEPT,
	CMODE_INVITEOVERRIDE,

	/* v/h/o/a/q */
	CMODE_VOICE, CMODE_HALFOP, CMODE_OP,
	CMODE_PROTECT, CMODE_OWNER,

	CMODE_END
};

#include "bots.h"
#include "opertype.h"

/*************************************************************************/

/* Protocol tweaks */

typedef struct ircdvars_ IRCDVar;
typedef struct ircdcapab_ IRCDCAPAB;

struct ircdvars_ {
	const char *name;				/* Name of the IRCd command */
	const char *pseudoclient_mode;			/* Mode used by BotServ Bots	*/
	int max_symbols;			/* Chan Max Symbols		*/
	const char *botchanumode;			/* Modes set when botserv joins a channel */
	int svsnick;				/* Supports SVSNICK		*/
	int vhost;				/* Supports vhost		*/
	const char *modeonunreg;			/* Mode on Unregister		*/
	int sgline;				/* Supports SGline		*/
	int sqline;				/* Supports SQline		*/
	int szline;				/* Supports SZline		*/
	int numservargs;			/* Number of Server Args	*/
	int join2set;				/* Join 2 Set Modes		*/
	int join2msg;				/* Join 2 Message		*/
	int topictsforward;			/* TS on Topics Forward		*/
	int topictsbackward;			/* TS on Topics Backward	*/
	int chansqline;				/* Supports Channel Sqlines	*/
	int quitonkill;				/* IRCD sends QUIT when kill	*/
	int svsmode_unban;			/* svsmode can be used to unban	*/
	int reversekickcheck;			/* Can reverse ban check	*/
	int vident;				/* Supports vidents		*/
	int svshold;				/* Supports svshold		*/
	int tsonmode;				/* Timestamp on mode changes	*/
	int nickip;					/* Sends IP on NICK		*/
	int omode;					/* On the fly o:lines		*/
	int umode;					/* change user modes		*/
	int nickvhost;				/* Users vhost sent during NICK */
	int chgreal;				/* Change RealName		*/
	std::bitset<128> DefMLock;   /* Default mlock modes		*/
	int check_nick_id;			/* On nick change check if they could be identified */
	int knock_needs_i;			/* Check if we needed +i when setting NOKNOCK */
	char *chanmodes;			/* If the ircd sends CHANMODE in CAPAB this is where we store it */
	int token;					/* Does Anope support the tokens for the ircd */
	int sjb64;					/* Base 64 encode TIMESTAMP */
	int sjoinbanchar;			/* use single quotes to define it */
	int sjoinexchar;			/* use single quotes to define it */
	int sjoininvchar;			/* use single quotes to define it */
	int svsmode_ucmode;			/* Can remove User Channel Modes with SVSMODE */
	int sglineenforce;
	int ts6;					/* ircd is TS6 */
	int p10;					/* ircd is P10  */
	char *nickchars;			/* character set */
	int cidrchanbei;			/* channel bans/excepts/invites support CIDR (syntax: +b *!*@192.168.0.0/15)
							 * 0 for no support, 1 for strict cidr support, anything else
							 * for ircd specific support (nefarious only cares about first /mask) */
	const char *globaltldprefix;		/* TLD prefix used for Global */
	bool b_delay_auth;			/* Auth for users is sent after the initial NICK/UID command */
};

struct ircdcapab_ {
  uint32 noquit;
  uint32 tsmode;
  uint32 unconnect;
  uint32 nickip;
  uint32 nsjoin;
  uint32 zip;
  uint32 burst;
  uint32 ts5;
  uint32 ts3;
  uint32 dkey;
  uint32 pt4;
  uint32 scs;
  uint32 qs;
  uint32 uid;
  uint32 knock;
  uint32 client;
  uint32 ipv6;
  uint32 ssj5;
  uint32 sn2;
  uint32 token;
  uint32 vhost;
  uint32 ssj3;
  uint32 nick2;
  uint32 vl;
  uint32 tlkext;
  uint32 dodkey;
  uint32 dozip;
  uint32 chanmodes;
  uint32 sjb64;
  uint32 nickchars;
};

/*************************************************************************/

/* File version for each database. Was one version for all before but was
   changed so they are now easier to maintain. =) */

#define BOT_VERSION 		10
#define CHAN_VERSION 		16
#define EXCEPTION_VERSION 	9
#define NEWS_VERSION 		9
#define NICK_VERSION 		14
#define PRE_NICK_VERSION 	2
#define OPER_VERSION 		13
#define HELP_VERSION 		1
#define HOST_VERSION 		3

/*************************************************************************/

/* Memo info structures.  Since both nicknames and channels can have memos,
 * we encapsulate memo data in a MemoList to make it easier to handle. */

struct memo_ {
	uint32 number;	/* Index number -- not necessarily array position! */
	uint16 flags;
	time_t time;	/* When it was sent */
	char sender[NICKMAX];
	char *text;
};

typedef struct {
	int16 memomax;
	std::vector<Memo *> memos;
} MemoInfo;


/*************************************************************************/

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
struct ChanAccess {
	uint16 in_use;	/* 1 if this entry is in use, else 0 */
	int16 level;
	NickCore *nc;	/* Guaranteed to be non-NULL if in use, NULL if not */
	time_t last_seen;
};

/* AutoKick data. */
typedef struct {
	int16 in_use;   /* Always 0 if not in use */
	int16 is_nick;	/* 1 if a regged nickname, 0 if a nick!user@host mask */
	uint16 flags;
	union {
		char *mask;		/* Guaranteed to be non-NULL if in use, NULL if not */
		NickCore *nc;	/* Same */
	} u;
	char *reason;

	char *creator;
	time_t addtime;
} AutoKick;

#define AK_USED		0x0001
#define AK_ISNICK	0x0002
#define AK_STUCK		0x0004

/* Structure used to contain bad words. */

struct badword_ {
	uint16 in_use;
	char *word;
	uint16 type;		/* BW_* below */
};

#define BW_ANY 		0
#define BW_SINGLE 	1
#define BW_START 	2
#define BW_END 		3

#include "regchannel.h"

/* Retain topic even after last person leaves channel */
#define CI_KEEPTOPIC	0x00000001
/* Don't allow non-authorized users to be opped */
#define CI_SECUREOPS	0x00000002
/* Hide channel from ChanServ LIST command */
#define CI_PRIVATE	0x00000004
/* Topic can only be changed by SET TOPIC */
#define CI_TOPICLOCK	0x00000008
/* Those not allowed ops are kickbanned */
#define CI_RESTRICTED	0x00000010
/* Don't allow ChanServ and BotServ commands to do bad things to bigger levels */
#define CI_PEACE  0x00000020
/* Don't allow any privileges unless a user is IDENTIFY'd with NickServ */
#define CI_SECURE	0x00000040
/* Don't allow the channel to be registered or used */
#define CI_FORBIDDEN	0x00000080
/* Channel password is encrypted */
#define CI_ENCRYPTEDPW	0x00000100
/* Channel does not expire */
#define CI_NO_EXPIRE	0x00000200
/* Channel memo limit may not be changed */
#define CI_MEMO_HARDMAX	0x00000400
/* Send notice to channel on use of OP/DEOP */
#define CI_OPNOTICE	0x00000800
/* Stricter control of channel founder status */
#define CI_SECUREFOUNDER 0x00001000
/* Always sign kicks */
#define CI_SIGNKICK 0x00002000
/* Sign kicks if level is < than the one defined by the SIGNKICK level */
#define CI_SIGNKICK_LEVEL 0x00004000
/* Use the xOP lists */
#define CI_XOP 0x00008000
/* Channel is suspended */
#define CI_SUSPENDED 0x00010000

/* TEMPORARY - ChanServ is on the channel. */
#define CI_INHABIT 0x80000000

/* Indices for cmd_access[]: */
#define CA_INVITE			0
#define CA_AKICK			1
#define CA_SET				2	/* but not FOUNDER or PASSWORD */
#define CA_UNBAN			3
#define CA_AUTOOP			4
#define CA_AUTODEOP			5	/* Maximum, not minimum */
#define CA_AUTOVOICE			6
#define CA_OPDEOP			7	/* ChanServ commands OP and DEOP */
#define CA_ACCESS_LIST			8
#define CA_CLEAR			9
#define CA_NOJOIN			10	/* Maximum */
#define CA_ACCESS_CHANGE 		11
#define CA_MEMO				12
#define CA_ASSIGN	   		13  /* BotServ ASSIGN command */
#define CA_BADWORDS	 		14  /* BotServ BADWORDS command */
#define CA_NOKICK	   		15  /* Not kicked by the bot */
#define CA_FANTASIA   			16
#define CA_SAY				17
#define CA_GREET					18
#define CA_VOICEME			19
#define CA_VOICE			20
#define CA_GETKEY		   		21
#define CA_AUTOHALFOP			22
#define CA_AUTOPROTECT			23
#define CA_OPDEOPME		 		24
#define CA_HALFOPME		 		25
#define CA_HALFOP		   		26
#define CA_PROTECTME				27
#define CA_PROTECT		  		28
#define CA_KICKME		   		29
#define CA_KICK			 		30
#define CA_SIGNKICK			31
#define CA_BANME					32
#define CA_BAN			  		33
#define CA_TOPIC					34
#define CA_INFO			 		35
#define CA_AUTOOWNER			36
#define CA_OWNER			37
#define CA_OWNERME			38

#define CA_SIZE		39

/* BotServ SET flags */
#define BS_DONTKICKOPS	  	0x00000001
#define BS_DONTKICKVOICES   	0x00000002
#define BS_FANTASY		  	0x00000004
#define BS_SYMBIOSIS			0x00000008
#define BS_GREET				0x00000010
#define BS_NOBOT				0x00000020

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
#define TTB_BOLDS 		0
#define TTB_COLORS 		1
#define TTB_REVERSES		2
#define TTB_UNDERLINES  	3
#define TTB_BADWORDS		4
#define TTB_CAPS		5
#define TTB_FLOOD		6
#define TTB_REPEAT		7
#define TTB_SIZE 		8

/*************************************************************************/

typedef struct {
	int what;
	const char *name;
	int desc;
} LevelInfo;


/*************************************************************************/

/* Server data */

typedef enum {
	SSYNC_IN_PROGRESS   = 0,	/* Sync is currently in progress */
	SSYNC_DONE		  = 1		/* We're in sync				 */
} SyncState;

struct server_ {
	Server *next, *prev;

	char *name;	 /* Server name						*/
	uint16 hops;	/* Hops between services and server   */
	char *desc;	 /* Server description				 */
	uint16 flags;   /* Some info flags, as defined below  */
	char *suid;	 /* Server Univeral ID				 */
	SyncState sync; /* Server sync state (see above)	  */

	Server *links;	/* Linked list head for linked servers 	  */
	Server *uplink;	/* Server which pretends to be the uplink */
};

#define SERVER_ISME  0x0001
#define SERVER_JUPED 0x0002
#define SERVER_ISUPLINK	0x0004

/*************************************************************************/

#define CUS_OP			0x0001
#define CUS_VOICE		0x0002
#define CUS_HALFOP		0x0004		/* Halfop (+h) */
#define CUS_OWNER		0x0008		/* Owner/Founder (+q) */
#define CUS_PROTECT		0x0010		/* Protected users (+a) */
/* #define CUS_DEOPPED		0x0080	*/	/* Removed due to IRCd checking it */

/*************************************************************************/

#include "users.h"

/* This structure stocks ban data since it must not be removed when
 * user is kicked.
 */

struct bandata_ {
	BanData *next, *prev;

	char *mask;				/* Since a nick is unsure and a User structure
						   	   is unsafe */
	time_t last_use;		/* Since time is the only way to check
						   	   whether it's still useful */
	int16 ttb[TTB_SIZE];
};

/* This structure stocks information on every user that will be used by
 * BotServ. */

struct userdata_ {
	/* Data validity */
	time_t last_use;

	/* for flood kicker */
	int16 lines;
	time_t last_start;

	/* for repeat kicker */
	char *lastline;
	int16 times;
};

/* Channelban type flags */
#define ENTRYTYPE_NONE		   0x00000000
#define ENTRYTYPE_CIDR4		  0x00000001
#define ENTRYTYPE_NICK_WILD	  0x00000004
#define ENTRYTYPE_NICK		   0x00000008
#define ENTRYTYPE_USER_WILD	  0x00000010
#define ENTRYTYPE_USER		   0x00000020
#define ENTRYTYPE_HOST_WILD	  0x00000040
#define ENTRYTYPE_HOST		   0x00000080

struct c_userlist {
	struct c_userlist *next, *prev;
	User *user;
	UserData *ud;
};

class CoreExport Channel : public Extensible
{
  private:
	  /** A map of channel modes with their parameters set on this channel
	   */
	  std::map<ChannelModeName, std::string> Params;

  public:
	Channel() { }

	~Channel()
	{
		Params.clear();
	}

	Channel *next, *prev;
	char name[CHANMAX];
	ChannelInfo *ci;			/* Corresponding ChannelInfo */
	time_t creation_time;		/* When channel was created */
	char *topic;
	char topic_setter[NICKMAX];		/* Who set the topic */
	time_t topic_time;			/* When topic was set */
	std::bitset<128> modes;

	EList *bans;
	EList *excepts;
	EList *invites;

	struct c_userlist *users;
	uint16 usercount;

	BanData *bd;

	time_t server_modetime;		/* Time of last server MODE */
	time_t chanserv_modetime;	/* Time of last check_modes() */
	int16 server_modecount;		/* Number of server MODEs this second */
	int16 chanserv_modecount;	/* Number of check_mode()'s this sec */
	int16 bouncy_modes;			/* Did we fail to set modes here? */
	int16 topic_sync;		   /* Is the topic in sync? */

	/**
	 * See if a channel has a mode
	 * @param Name The mode name
	 * @return true or false
	 */
	bool HasMode(ChannelModeName Name);

	/**
	 * Set a mode on a channel
	 * @param Name The mode name
	 */
	void SetMode(ChannelModeName Name);

	/**
	 * Set a mode on a channel
	 * @param Mode The mode
	 */
	void SetMode(char Mode);

	/**
	 * Remove a mode from a channel
	 * @param Name The mode name
	 */
	void RemoveMode(ChannelModeName Name);

	/**
	 * Remove a mode from a channel
	 * @param Mode The mode
	 */
	void RemoveMode(char Mode);

	/** Clear all the modes from the channel
	 * @param client The client unsetting the modes
	 */
	void ClearModes(char *client = NULL);

	/** Clear all the bans from the channel
	 * @param client The client unsetting the modes
	 */
	void ClearBans(char *client = NULL);

	/** Clear all the excepts from the channel
	 * @param client The client unsetting the modes
	 */
	void ClearExcepts(char *client = NULL);

	/** Clear all the invites from the channel
	 * @param client The client unsetting the modes
	 */
	void ClearInvites(char *client = NULL);

	/** Set a channel mode param on the channel
	 * @param Name The mode
	 * @param param The param
	 * @param true on success
	 */
	bool SetParam(ChannelModeName Name, std::string &param);

	/** Unset a param from the channel
	 * @param Name The mode
	 */
	void UnsetParam(ChannelModeName Name);

	/** Get a param from the channel
	 * @param Name The mode
	 * @param Target a string to put the param into
	 * @return true on success
	 */
	const bool GetParam(ChannelModeName Name, std::string *Target);

	/** Check if a mode is set and has a param
	 * @param Name The mode
	 */
	const bool HasParam(ChannelModeName Name);
};

struct c_elist {
	Entry *entries;
	int32 count;
};

struct c_elist_entry {
	Entry *next, *prev;
	uint32 type;
	uint32 cidr_ip;			 /* IP mask for CIDR matching */
	uint32 cidr_mask;		   /* Netmask for CIDR matching */
	char *nick, *user, *host, *mask;
};

/*************************************************************************/

/* Ignorance list data. */

typedef struct ignore_data {
	struct ignore_data *prev, *next;
	char *mask;
	time_t time;	/* When do we stop ignoring them? */
} IgnoreData;

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
	char who[NICKMAX];
	time_t time;
};

/*************************************************************************/

/* Mail data */

struct mailinfo_ {
	FILE *pipe;
	User *sender;
	NickCore *recipient;
	NickRequest *recip;
};

/*************************************************************************/

struct akill_ {
	char *user;			/* User part of the AKILL */
	char *host;			/* Host part of the AKILL */

	char *by;			/* Who set the akill */
	char *reason;		/* Why they got akilled */

	time_t seton;		/* When it was set */
	time_t expires;		/* When it expires */
};

/*************************************************************************/

/* Structure for OperServ SGLINE and SZLINE commands */

struct sxline_ {
	char *mask;
	char *by;
	char *reason;
	time_t seton;
	time_t expires;
};


/************************************************************************/

/* Host serv structures */

struct hostcore_ {
	HostCore *next;
	char *nick;				/* Owner of the vHost */
	char *vIdent;			/* vIdent for the user */
	char *vHost;			/* Vhost for this user */
	char *creator;			/* Oper Nick of the oper who set the vhost */
	time_t time;			/* Date/Time vHost was set */
};

/*************************************************************************/


struct exception_ {
	char *mask;				 /* Hosts to which this exception applies */
	int limit;				  /* Session limit for exception */
	char who[NICKMAX];		  /* Nick of person who added the exception */
	char *reason;			   /* Reason for exception's addition */
	time_t time;				/* When this exception was added */
	time_t expires;			 /* Time when it expires. 0 == no expiry */
	int num;					/* Position in exception list; used to track
								 * positions when deleting entries. It is
								 * symbolic and used internally. It is
								 * calculated at load time and never saved. */
};

/*************************************************************************/

/** The different types of modes
*/
enum ModeType
{
	/* Regular mode */
	MODE_REGULAR,
	/* b/e/I */
	MODE_LIST,
	/* k/l etc */
	MODE_PARAM,
	/* v/h/o/a/q */
	MODE_STATUS
};

/** This class is a user mode, all user modes use this/inherit from this
 */
class UserMode
{
  public:

	/* Mode name */
	UserModeName Name;
	/* The mode char */
	char ModeChar;

	/** Default constructor
	 * @param nName The mode name
	 */
	UserMode(UserModeName mName)
	{
		this->Name = mName;
	}

	/** Default destructor
	 */
	virtual ~UserMode() { }
};

/** This class is a channel mode, all channel modes use this/inherit from this
 */
class ChannelMode
{
  public:

	/* Mode name */
	ChannelModeName Name;
	/* Type of mode this is */
	ModeType Type;
	/* The mode char */
	char ModeChar;

	/** Default constructor
	 * @param mName The mode name
	 */
	ChannelMode(ChannelModeName mName)
	{
		this->Name = mName;
		this->Type = MODE_REGULAR;
	}

	/** Default destructor
	 */
	virtual ~ChannelMode() { }

	/** Can a user set this mode, used for mlock
	 * NOTE: User CAN be NULL, this is for checking if it can be locked with defcon
	 * @param u The user, or NULL
	 */
	virtual bool CanSet(User *u) { return true; }
};

/** This is a mode for lists, eg b/e/I. These modes should inherit from this
 */
class ChannelModeList : public ChannelMode
{
  public:

	/** Default constructor
	 * @param mName The mode name
	 */
	ChannelModeList(ChannelModeName mName) : ChannelMode(mName)
	{
		this->Type = MODE_LIST;
	}

	/** Default destructor
	 */
	virtual ~ChannelModeList() { }

	/** Is the mask valid
	 * @param mask The mask
	 * @return true for yes, false for no
	 */
	virtual bool IsValid(const char *mask) { return true; }

	/** Add the mask to the channel, this should be overridden
	 * @param chan The channel
	 * @param mask The mask
	 */
	virtual void AddMask(Channel *chan, const char *mask) { }

	/** Delete the mask from the channel, this should be overridden
	 * @param chan The channel
	 * @param mask The mask
	 */
	virtual void DelMask(Channel *chan, const char *mask) { }

};

/** This is a mode with a paramater, eg +k/l. These modes should use/inherit from this
*/
class ChannelModeParam : public ChannelMode
{
  public:

	/** Default constructor
	 * @param mName The mode name
	 * @param MinusArg true if this mode sends no arg when unsetting
	 */
	ChannelModeParam(ChannelModeName mName, bool MinusArg = false) : ChannelMode(mName)
	{
		this->Type = MODE_PARAM;
		MinusNoArg = MinusArg;
	}

	/** Default destructor
	 */
	virtual ~ChannelModeParam() { }

	/* Should we send an arg when unsetting this mode? */
	bool MinusNoArg;

	/** Is the param valid
	 * @param value The param
	 * @return true for yes, false for no
	 */
	virtual bool IsValid(const char *value) { return true; }
};

/** This is a mode that is a channel status, eg +v/h/o/a/q.
*/
class ChannelModeStatus : public ChannelMode
{
  public:
	/** CUS_ values, see below
	*/
	int16 Status;
	/* The symbol, eg @ % + */
	char Symbol;

	/** Default constructor
	 * @param mName The mode name
	 * @param mStatus A CUS_ value
	 * @param mSymbol The symbol for the mode, eg @ % +
	 * @param mProtectBotServ Should botserv clients reset this on themself if it gets unset>
	 */
	ChannelModeStatus(ChannelModeName mName, int16 mStatus, char mSymbol, bool mProtectBotServ = false) : ChannelMode(mName)
	{
		this->Type = MODE_STATUS;
		this->Status = mStatus;
		this->Symbol = mSymbol;
		this->ProtectBotServ = mProtectBotServ;
	}

	/** Default destructor
	 */
	virtual ~ChannelModeStatus() { }

	/* Should botserv protect itself with this mode? eg if someone -o's botserv it will +o */
	bool ProtectBotServ;
};

/** This class manages modes, and has the ability to add channel and
 * user modes to Anope to track internally
 */
class CoreExport ModeManager
{
  public:
	/* User modes */
	static std::map<char, UserMode *> UserModesByChar;
	static std::map<UserModeName, UserMode *> UserModesByName;
	/* Channel modes */
	static std::map<char, ChannelMode *> ChannelModesByChar;
	static std::map<ChannelModeName, ChannelMode *> ChannelModesByName;
	/* Although there are two different maps for UserModes and ChannelModes
	 * the pointers in each are the same. This is used to increase
	 * efficiency.
	 */

	/** Add a user mode to Anope
	 * @param Mode The mode
	 * @param um A UserMode or UserMode derived class
	 * @return true on success, false on error
	 */
	static bool AddUserMode(char Mode, UserMode *um);

	/** Add a channel mode to Anope
	 * @param Mode The mode
	 * @param cm A ChannelMode or ChannelMode derived class
	 * @return true on success, false on error
	 */
	static bool AddChannelMode(char Mode, ChannelMode *cm);

	/** Find a channel mode
	 * @param Mode The mode
	 * @return The mode class
	 */
	static ChannelMode *FindChannelModeByChar(char Mode);

	/** Find a user mode
	 * @param Mode The mode
	 * @return The mode class
	 */
	static UserMode *FindUserModeByChar(char Mode);

	/** Find a channel mode
	 * @param Mode The modename
	 * @return The mode class
	 */
	static ChannelMode *FindChannelModeByName(ChannelModeName Name);

	/** Find a user mode
	 * @param Mode The modename
	 * @return The mode class
	 */
	static UserMode *FindUserModeByName(UserModeName Name);

	/** Gets the channel mode char for a symbol (eg + returns v)
	 * @param Value The symbol
	 * @return The char
	 */
	static char GetStatusChar(char Value);
};

/** Channel mode +b
 */
class ChannelModeBan : public ChannelModeList
{
  public:
	ChannelModeBan() : ChannelModeList(CMODE_BAN) { }

	void AddMask(Channel *chan, const char *mask);

	void DelMask(Channel *chan, const char *mask);
};

/** Channel mode +e
 */
class ChannelModeExcept : public ChannelModeList
{
  public:
	ChannelModeExcept() : ChannelModeList(CMODE_EXCEPT) { }

	void AddMask(Channel *chan, const char *mask);

	void DelMask(Channel *chan, const char *mask);
};

/** Channel mode +I
 */
class ChannelModeInvite : public ChannelModeList
{
  public:
	ChannelModeInvite() : ChannelModeList(CMODE_INVITEOVERRIDE) { }

	void AddMask(Channel *chan, const char *mask);

	void DelMask(Channel *chan, const char *mask);
};

/** Channel mode +k (key)
 */
class ChannelModeKey : public ChannelModeParam
{
  public:
	ChannelModeKey() : ChannelModeParam(CMODE_KEY) { }

	bool IsValid(const char *value);
};

/** Channel mode +f (flood)
 */
class ChannelModeFlood : public ChannelModeParam
{
  public:
	ChannelModeFlood() : ChannelModeParam(CMODE_FLOOD) { }

	bool IsValid(const char *value);
};

/** This class is used for channel mode +A (Admin only)
 * Only opers can mlock it
 */
class ChannelModeAdmin : public ChannelMode
{
  public:
	ChannelModeAdmin() : ChannelMode(CMODE_ADMINONLY) { }

	/* Opers only */
	bool CanSet(User *u);
};

/** This class is used for channel mode +O (Opers only)
 * Only opers can mlock it
 */
class ChannelModeOper : public ChannelMode
{
  public:
	ChannelModeOper() : ChannelMode(CMODE_OPERONLY) { }

	/* Opers only */
	bool CanSet(User *u);
};

/** This class is used for channel mode +r (registered channel)
 * No one may mlock r
 */
class ChannelModeRegistered : public ChannelMode
{
  public:
	ChannelModeRegistered() : ChannelMode(CMODE_REGISTERED) { }

	/* No one mlocks +r */
	bool CanSet(User *u);
};


/*************************************************************************/

struct session_ {
	Session *prev, *next;
	char *host;
	int count;				  /* Number of clients with this host */
	int hits;				   /* Number of subsequent kills for a host */
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

/* Memo Flags */
#define MF_UNREAD	0x0001	/* Memo has not yet been read */
#define MF_RECEIPT	0x0002	/* Sender requested receipt */
#define MF_NOTIFYS	  0x0004  /* Memo is a notification of receitp */

/* Nickname status flags: */
#define NS_FORBIDDEN	0x0002	  /* Nick may not be registered or used */
#define NS_NO_EXPIRE	0x0004	  /* Nick never expires */
//#define NS_IDENTIFIED	0x8000	  /* User has IDENTIFY'd */
//#define NS_RECOGNIZED	0x4000	  /* ON_ACCESS true && SECURE flag not set */
//#define NS_ON_ACCESS	0x2000	  /* User comes from a known address */
#define NS_KILL_HELD	0x1000	  /* Nick is being held after a kill */
#define NS_GUESTED	0x0100		/* SVSNICK has been sent but nick has not
					 				 * yet changed. An enforcer will be
					 				 * introduced when it does change. */
#define NS_MASTER	0x0200		/* Was a master nick; used to import old databases */
#define NS_TRANSGROUP	0xC000		/* Status flags that can be passed to a nick of the
									   same group during nick change */
#define NS_TEMPORARY	0xFF00	  /* All temporary status flags */
/* These two are not used anymore */
#define NS_OLD_ENCRYPTEDPW	0x0001  /* Nickname password is encrypted */




/* Nickname setting flags: */
#define NI_KILLPROTECT		0x00000001  /* Kill others who take this nick */
#define NI_SECURE		0x00000002  /* Don't recognize unless IDENTIFY'd */
#define NI_MSG				  0x00000004  /* Use PRIVMSGs instead of NOTICEs */
#define NI_MEMO_HARDMAX		0x00000008  /* Don't allow user to change memo limit */
#define NI_MEMO_SIGNON		0x00000010  /* Notify of memos at signon and un-away */
#define NI_MEMO_RECEIVE		0x00000020  /* Notify of new memos when sent */
#define NI_PRIVATE		0x00000040  /* Don't show in LIST to non-servadmins */
#define NI_HIDE_EMAIL		0x00000080  /* Don't show E-mail in INFO */
#define NI_HIDE_MASK		0x00000100  /* Don't show last seen address in INFO */
#define NI_HIDE_QUIT		0x00000200  /* Don't show last quit message in INFO */
#define NI_KILL_QUICK		0x00000400  /* Kill in 20 seconds instead of 60 */
#define NI_KILL_IMMED		0x00000800  /* Kill immediately instead of in 60 sec */
//#define NI_SERVICES_OPER 	0x00001000  /* No longer used */
//#define NI_SERVICES_ADMIN	0x00002000  /* No longer used */
#define NI_ENCRYPTEDPW		0x00004000  /* Nickname password is encrypted */
//#define NI_SERVICES_ROOT		0x00008000  /* No longer used */
#define NI_MEMO_MAIL			0x00010000  /* User gets email on memo */
#define NI_HIDE_STATUS		  0x00020000  /* Don't show services access status */
#define NI_SUSPENDED		0x00040000  /* Nickname is suspended */
#define NI_AUTOOP		0x00080000  /* Autoop nickname in channels */
#define NI_NOEXPIRE		0x00100000 /* nicks in this group won't expire */

// Old NS_FORBIDDEN, very fucking temporary.
#define NI_FORBIDDEN	0x80000000

/* Languages.  Never insert anything in the middle of this list, or
 * everybody will start getting the wrong language!  If you want to change
 * the order the languages are displayed in for NickServ HELP SET LANGUAGE,
 * do it in language.c.
 */
#define LANG_EN_US			  0	/* United States English */
#define LANG_JA_JIS			 1	/* Japanese (JIS encoding) */
#define LANG_JA_EUC			 2	/* Japanese (EUC encoding) */
#define LANG_JA_SJIS			3	/* Japanese (SJIS encoding) */
#define LANG_ES				 4	/* Spanish */
#define LANG_PT				 5	/* Portugese */
#define LANG_FR				 6	/* French */
#define LANG_TR				 7	/* Turkish */
#define LANG_IT				 8	/* Italian */
#define LANG_DE				 9   	/* German */
#define LANG_CAT			   10   	/* Catalan */
#define LANG_GR				11   	/* Greek */
#define LANG_NL				12   	/* Dutch */
#define LANG_RU				13   	/* Russian */
#define LANG_HUN			   14   	/* Hungarian */
#define LANG_PL				15   	/* Polish */

#define NUM_LANGS			  16	/* Number of languages */
#define USED_LANGS			 13	/* Number of languages provided */


#define DEF_LANGUAGE	LANG_EN_US

#define BI_PRIVATE		0x0001
#define BI_CHANSERV		0x0002
#define BI_BOTSERV		0x0004
#define BI_HOSTSERV		0x0008
#define BI_OPERSERV		0x0010
#define BI_MEMOSERV		0x0020
#define BI_NICKSERV		0x0040
#define BI_GLOBAL		0x0080

/*************************************************************************/
/* CAPAB stuffs */

typedef struct capabinfo_ CapabInfo;
struct capabinfo_ {
	const char *token;
	uint32 flag;
};

#define CAPAB_NOQUIT	0x00000001
#define CAPAB_TSMODE	0x00000002
#define CAPAB_UNCONNECT 0x00000004
#define CAPAB_NICKIP	0x00000008
#define CAPAB_NSJOIN	0x00000010
#define CAPAB_ZIP	   0x00000020
#define CAPAB_BURST	 0x00000040
#define CAPAB_TS3	   0x00000080
#define CAPAB_TS5	   0x00000100
#define CAPAB_DKEY	  0x00000200
#define CAPAB_DOZIP	 0x00000400
#define CAPAB_DODKEY	0x00000800
#define CAPAB_QS		0x00001000
#define CAPAB_SCS	   0x00002000
#define CAPAB_PT4	   0x00004000
#define CAPAB_UID	   0x00008000
#define CAPAB_KNOCK	 0x00010000
#define CAPAB_CLIENT	0x00020000
#define CAPAB_IPV6	  0x00040000
#define CAPAB_SSJ5	  0x00080000
#define CAPAB_SN2	   0x00100000
#define CAPAB_VHOST	 0x00200000
#define CAPAB_TOKEN	 0x00400000
#define CAPAB_SSJ3	  0x00800000
#define CAPAB_NICK2	 0x01000000
#define CAPAB_VL		0x02000000
#define CAPAB_TLKEXT	0x04000000
#define CAPAB_CHANMODE  0x08000000
#define CAPAB_SJB64	 0x10000000
#define CAPAB_NICKCHARS 0x20000000

/*************************************************************************/

/**
 * RFC: defination of a valid nick
 * nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
 * letter	 =  %x41-5A / %x61-7A	   ; A-Z / a-z
 * digit	  =  %x30-39				 ; 0-9
 * special	=  %x5B-60 / %x7B-7D	   ; "[", "]", "\", "`", "_", "^", "{", "|", "}"
 **/
#define isvalidnick(c) ( isalnum(c) || ((c) >='\x5B' && (c) <='\x60') || ((c) >='\x7B' && (c) <='\x7D') || (c)=='-' )

/*************************************************************************/

/*
 * Forward declaration reqired, because the base IRCDProto class uses some crap from in here.
 */
class IRCDProto;
struct Uplink;
class ServerConfig;
#include "extern.h"
#include "configreader.h"

class CoreExport IRCDProto
{
 private:
		virtual void SendSVSKillInternal(const char *, const char *, const char *) = 0;
		virtual void SendModeInternal(BotInfo *, const char *, const char *) = 0;
		virtual void SendKickInternal(BotInfo *bi, const char *, const char *, const char *) = 0;
		virtual void SendNoticeChanopsInternal(BotInfo *bi, const char *, const char *) = 0;
		virtual void SendMessageInternal(BotInfo *bi, const char *dest, const char *buf);
		virtual void SendNoticeInternal(BotInfo *bi, const char *dest, const char *msg);
		virtual void SendPrivmsgInternal(BotInfo *bi, const char *dest, const char *buf);
		virtual void SendQuitInternal(BotInfo *bi, const char *buf);
		virtual void SendPartInternal(BotInfo *bi, const char *chan, const char *buf);
		virtual void SendGlobopsInternal(const char *source, const char *buf);
		virtual void SendCTCPInternal(BotInfo *bi, const char *dest, const char *buf);
		virtual void SendNumericInternal(const char *source, int numeric, const char *dest, const char *buf);
	public:

		virtual ~IRCDProto() { }

		virtual void SendSVSNOOP(const char *, int) { }
		virtual void SendAkillDel(const char *, const char *) = 0;
		virtual void SendTopic(BotInfo *, const char *, const char *, const char *, time_t) = 0;
		virtual void SendVhostDel(User *) { }
		virtual void SendAkill(const char *, const char *, const char *, time_t, time_t, const char *) = 0;
		virtual void SendSVSKill(const char *source, const char *user, const char *fmt, ...);
		virtual void SendSVSMode(User *, int, const char **) = 0;
		virtual void SendMode(BotInfo *bi, const char *dest, const char *fmt, ...);
		virtual void SendClientIntroduction(const char *, const char *, const char *, const char *, const char *, const char *uid) = 0;
		virtual void SendKick(BotInfo *bi, const char *chan, const char *user, const char *fmt, ...);
		virtual void SendNoticeChanops(BotInfo *bi, const char *dest, const char *fmt, ...);
		virtual void SendMessage(BotInfo *bi, const char *dest, const char *fmt, ...);
		virtual void SendNotice(BotInfo *bi, const char *dest, const char *fmt, ...);
		virtual void SendAction(BotInfo *bi, const char *dest, const char *fmt, ...);
		virtual void SendPrivmsg(BotInfo *bi, const char *dest, const char *fmt, ...);
		virtual void SendGlobalNotice(BotInfo *bi, const char *dest, const char *msg);
		virtual void SendGlobalPrivmsg(BotInfo *bi, const char *dest, const char *msg);
		virtual void SendBotOp(const char *, const char *) = 0;

		/** XXX: This is a hack for NickServ enforcers. It is deprecated.
		 * If I catch any developer using this in new code, I will RIP YOUR BALLS OFF.
		 * Thanks.
		 * -- w00t
		 */
		virtual void SendQuit(const char *nick, const char *) MARK_DEPRECATED;
		virtual void SendQuit(BotInfo *bi, const char *fmt, ...);
		virtual void SendPong(const char *servname, const char *who);
		virtual void SendJoin(BotInfo *bi, const char *, time_t) = 0;
		virtual void SendSQLineDel(const char *) = 0;
		virtual void SendInvite(BotInfo *bi, const char *chan, const char *nick);
		virtual void SendPart(BotInfo *bi, const char *chan, const char *fmt, ...);
		virtual void SendGlobops(const char *source, const char *fmt, ...);
		virtual void SendSQLine(const char *, const char *) = 0;
		virtual void SendSquit(const char *servname, const char *message);
		virtual void SendSVSO(const char *, const char *, const char *) { }
		virtual void SendChangeBotNick(BotInfo *bi, const char *newnick);
		virtual void SendForceNickChange(const char *oldnick, const char *newnick, time_t when);
		virtual void SendVhost(const char *, const char *, const char *) { }
		virtual void SendConnect() = 0;
		virtual void SendSVSHold(const char *) { }
		virtual void SendSVSHoldDel(const char *) { }
		virtual void SendSGLineDel(const char *) { }
		virtual void SendSZLineDel(const char *) { }
		virtual void SendSZLine(const char *, const char *, const char *) { }
		virtual void SendSGLine(const char *, const char *) { }
		virtual void SendBanDel(const char *, const char *) { }
		virtual void SendSVSModeChan(const char *, const char *, const char *) { }
		virtual void SendUnregisteredNick(User *) { }
		virtual void SendCTCP(BotInfo *bi, const char *dest, const char *fmt, ...);
		virtual void SendSVSJoin(const char *, const char *, const char *, const char *) { }
		virtual void SendSVSPart(const char *, const char *, const char *) { }
		virtual void SendSWhois(const char *, const char *, const char *) { }
		virtual void SendEOB() { }
		virtual void SendServer(Server *) = 0;
		virtual void ProcessUsermodes(User *, int, const char **) = 0;
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

class CoreExport IRCDTS6Proto : public IRCDProto
{
};

/*************************************************************************/

struct Uplink {
	char *host;
	unsigned port;
	char *password;
	Uplink(const char *_host, int _port, const char *_password)
	{
		host = sstrdup(_host);
		port = _port;
		password = sstrdup(_password);
	}
	~Uplink()
	{
		delete [] host;
		delete [] password;
	}
};

class Anope
{
 public:
	/** Check whether two strings match.
	 * @param mask The pattern to check (e.g. foo*bar)
	 * @param str The string to check against the pattern (e.g. foobar)
	 * @param case_sensitive Whether or not the match is case sensitive, default false.
	 */
	CoreExport static bool Match(const std::string &str, const std::string &mask, bool case_sensitive = false);
};

/*************************************************************************/

/** Pair of nick/opertype lookup. It's stored like this currently, because config is parsed before db load.
 * XXX: It would be nice to not need this. UGH.
 */
E std::list<std::pair<std::string, std::string> > svsopers_in_config;
/** List of available opertypes.
 */
E std::list<OperType *> MyOperTypes;

#endif	/* SERVICES_H */
