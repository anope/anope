/*
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
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

#ifndef _WIN32
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
# ifdef HAVE_VA_LIST_AS_ARRAY
#   define VA_COPY(DEST,SRC) (*(DEST) = *(SRC))
# else
#   define VA_COPY(DEST, SRC) va_copy(DEST, SRC)
# endif
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
	#define MODULE_INIT(x, y) \
		extern "C" DllExport Module *init_module(const std::string &modname, const std::string &creator) \
		{ \
			return new y(x, creator); \
		} \
		BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved) \
		{ \
			switch ( nReason ) \
			{ \
				case DLL_PROCESS_ATTACH: \
				case DLL_PROCESS_DETACH: \
					break; \
			} \
			return TRUE; \
		} \
		extern "C" DllExport void destroy_module(y *m) \
		{ \
		    delete m; \
		}

#else
	#define MODULE_INIT(x, y) \
		extern "C" DllExport Module *init_module(const std::string &modname, const std::string &creator) \
		{ \
			return new y(x, creator); \
		} \
		extern "C" DllExport void destroy_module(y *m) \
		{ \
			delete m; \
		}
#endif

/* Miscellaneous definitions. */
#include "defs.h"
#include "slist.h"
#include "events.h"

/* pull in the various bits of STL to pull in */
#include <string>
#include <map>
#include <exception>
#include <list>
#include <vector>

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
	const std::string err;
	/** Source of the exception
	 */
	const std::string source;
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
	virtual ~CoreException() throw() {};
	/** Returns the reason for the exception.
	 * The module should probably put something informative here as the user will see this upon failure.
	 */
	virtual const char* GetReason()
	{
		return err.c_str();
	}

	virtual const char* GetSource()
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
	virtual ~ModuleException() throw() {};
};

/** Class with the ability to be extended with key:value pairs.
 * Thanks to InspIRCd for this.
 */
class Extensible
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
	template<typename T> bool Extend(const std::string &key, T* p)
	{
		/* This will only add an item if it doesnt already exist,
		 * the return value is a std::pair of an iterator to the
		 * element, and a bool saying if it was actually inserted.
		 */
		return this->Extension_Items.insert(std::make_pair(key, static_cast<char *>(p))).second;
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
	bool Shrink(const std::string &key);

	/** Get an extension item.
	 *
	 * @param key The key parameter is an arbitary string which identifies the extension data
	 * @param p If you provide a non-existent key, this value will be NULL. Otherwise a pointer to the item you requested will be placed in this templated parameter.
	 * @return Returns true if the item was found and false if it was nor, regardless of wether 'p' is NULL. This allows you to store NULL values in Extensible.
	 */
	template<typename T> bool GetExt(const std::string &key, T* &p)
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
};

/*************************************************************************/

/* forward declarations, mostly used by older code */
class User;
class ChannelInfo;


typedef struct server_ Server;
typedef struct channel_ Channel;
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
typedef struct newsitem_ NewsItem;
typedef struct exception_ Exception;
typedef struct cbmode_ CBMode;
typedef struct cbmodeinfo_ CBModeInfo;
typedef struct cmmode_ CMMode;
typedef struct csmode_ CSMode;
typedef struct cumode_ CUMode;
typedef struct csmodeutil_ CSModeUtil;
typedef struct session_ Session;

#include "bots.h"

/*************************************************************************/

/* Protocol tweaks */

typedef struct ircdvars_ IRCDVar;
typedef struct ircdcapab_ IRCDCAPAB;

struct ircdvars_ {
	const char *name;				/* Name of the IRCd command */
	const char *pseudoclient_mode;			/* Mode used by BotServ Bots	*/
	int max_symbols;			/* Chan Max Symbols		*/
	const char *modestoremove;			/* Channel Modes to remove	*/
	const char *botchanumode;			/* Modes set when botserv joins a channel */
	int svsnick;				/* Supports SVSNICK		*/
	int vhost;				/* Supports vhost		*/
	int owner;				/* Supports Owner		*/
	const char *ownerset;				/* Mode to set for owner	*/
	const char *ownerunset;			/* Mode to unset for a owner	*/
	const char *adminset;				/* Mode to set for admin	*/
	const char *adminunset;			/* Mode to unset for admin	*/
	const char *modeonreg;			/* Mode on Register		*/
	const char *rootmodeonid;					 /* Mode on ID for ROOTS		 */
	const char *adminmodeonid;					/* Mode on ID for ADMINS		*/
	const char *opermodeonid;					 /* Mode on ID for OPERS		 */
	const char *modeonunreg;			/* Mode on Unregister		*/
	const char *modeonnick;			/* Mode on nick change		*/
	int sgline;				/* Supports SGline		*/
	int sqline;				/* Supports SQline		*/
	int szline;				/* Supports SZline		*/
	int halfop;				/* Supports HalfOp		*/
	int numservargs;			/* Number of Server Args	*/
	int join2set;				/* Join 2 Set Modes		*/
	int join2msg;				/* Join 2 Message		*/
	int except;				/* exception +e			*/
	int topictsforward;			/* TS on Topics Forward		*/
	int topictsbackward;			/* TS on Topics Backward	*/
	uint32 protectedumode;			/* What is the Protected Umode	*/
	int admin;				/* Has Admin 			*/
	int chansqline;				/* Supports Channel Sqlines	*/
	int quitonkill;				/* IRCD sends QUIT when kill	*/
	int svsmode_unban;			/* svsmode can be used to unban	*/
	int protect;				/* Has protect modes		*/
	int reversekickcheck;			/* Can reverse ban check	*/
	int chanreg;				/* channel mode +r for register	*/
	uint32 regmode;				/* Mode to use for +r 		*/
	int vident;				/* Supports vidents		*/
	int svshold;				/* Supports svshold		*/
	int tsonmode;				/* Timestamp on mode changes	*/
	int nickip;					/* Sends IP on NICK		*/
	int omode;					/* On the fly o:lines		*/
	int umode;					/* change user modes		*/
	int nickvhost;				/* Users vhost sent during NICK */
	int chgreal;				/* Change RealName		*/
	uint32 noknock;				/* Channel Mode for no knock	*/
	uint32 adminmode;			/* Admin Only Channel Mode	*/
	uint32 defmlock;			/* Default mlock modes		*/
	uint32 vhostmode;			/* Vhost mode			*/
	int fmode;					/* +f				*/
	int Lmode;					/* +L				*/
	uint32 chan_fmode;			/* Mode 			*/
	uint32 chan_lmode;			/* Mode				*/
	int check_nick_id;			/* On nick change check if they could be identified */
	int knock_needs_i;			/* Check if we needed +i when setting NOKNOCK */
	char *chanmodes;			/* If the ircd sends CHANMODE in CAPAB this is where we store it */
	int token;					/* Does Anope support the tokens for the ircd */
	int sjb64;					/* Base 64 encode TIMESTAMP */
	int invitemode;				/* +I  */
	int sjoinbanchar;			/* use single quotes to define it */
	int sjoinexchar;			/* use single quotes to define it */
	int sjoininvchar;			/* use single quotes to define it */
	int svsmode_ucmode;			/* Can remove User Channel Modes with SVSMODE */
	int sglineenforce;
	const char *vhostchar;			/* char used for vhosting */
	int ts6;					/* ircd is TS6 */
	int supporthelper;			/* +h helper umodes */
	int p10;					/* ircd is P10  */
	char *nickchars;			/* character set */
	int sync;					/* reports sync state */
	int cidrchanbei;			/* channel bans/excepts/invites support CIDR (syntax: +b *!*@192.168.0.0/15)
							 * 0 for no support, 1 for strict cidr support, anything else
							 * for ircd specific support (nefarious only cares about first /mask) */
	const char *globaltldprefix;		/* TLD prefix used for Global */
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
  uint32 umode2;
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
#ifdef USE_MYSQL
	uint32 id;		/* Database ID; see mysql.c */
#endif
};

typedef struct {
	int16 memomax;
	std::vector<Memo *> memos;
} MemoInfo;



/*************************************************************************/

// For NickServ
#include "account.h"

/*************************************************************************/

/* Channel info structures.  Stored similarly to the nicks, except that
 * the second character of the channel name, not the first, is used to
 * determine the list.  (Hashing based on the first character of the name
 * wouldn't get very far. ;) ) */

/* Access levels for users. */
typedef struct {
	uint16 in_use;	/* 1 if this entry is in use, else 0 */
	int16 level;
	NickCore *nc;	/* Guaranteed to be non-NULL if in use, NULL if not */
	time_t last_seen;
} ChanAccess;

/* Note that these two levels also serve as exclusive boundaries for valid
 * access levels.  ACCESS_FOUNDER may be assumed to be strictly greater
 * than any valid access level, and ACCESS_INVALID may be assumed to be
 * strictly less than any valid access level. Also read below.
 */
#define ACCESS_FOUNDER	10000	/* Numeric level indicating founder access */
#define ACCESS_INVALID	-10000	/* Used in levels[] for disabled settings */
/* There is one exception to the above access levels: SuperAdmins will have
 * access level 10001. This level is never stored, however; it is only used
 * in comparison and to let SuperAdmins win from founders where needed
 */
#define ACCESS_SUPERADMIN 10001

/* Levels for xOP */

#define ACCESS_VOP		3
#define ACCESS_HOP	  	4
#define ACCESS_AOP		5
#define ACCESS_SOP		10

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
#define CI_VERBOTEN	0x00000080
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
/* #define CA_AUTOADMIN				32
#define CA_ADMINME		  		33
#define CA_ADMIN					34 */
	/* Why are these commented out and not removed? -GD */
#define CA_BANME					32
#define CA_BAN			  		33
#define CA_TOPIC					34
#define CA_INFO			 		35

#define CA_SIZE		36

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

/* ChanServ mode utilities commands */

struct csmodeutil_ {
	const char *name;			/* Name of the ChanServ command */
	const char *bsname;			/* Name of the BotServ fantasy command */
	const char *mode;			/* Mode (ie. +o) */
	int32 notice;			/* Notice flag (for the damn OPNOTICE) */
	int level;			/* Level required to use the command */
	int levelself;			/* Level required to use the command for himself */
};

typedef struct {
	int what;
	const char *name;
	int desc;
} LevelInfo;


/*************************************************************************/

/* Server data */

typedef enum {
	SSYNC_UNKNOWN	   = 0,	/* We can't get the sync state   */
	SSYNC_IN_PROGRESS   = 1,	/* Sync is currently in progress */
	SSYNC_DONE		  = 2		/* We're in sync				 */
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

/*************************************************************************/
#include "users.h"


struct cbmode_ {
	uint32 flag;			/* Long value that represents the mode */
	uint16 flags;			/* Flags applying to this mode (CBM_* below) */

	/* Function to associate a value with the mode */
	void (*setvalue)	(Channel *chan, const char *value);
	void (*cssetvalue)	(ChannelInfo *ci, const char *value);
};

#define CBM_MINUS_NO_ARG 	0x0001		/* No argument for unset */
#define CBM_NO_MLOCK		0x0002		/* Can't be MLOCKed */
#define CBM_NO_USER_MLOCK   	0x0004	  	/* Can't be MLOCKed by non-opers */

struct cbmodeinfo_ {
	char mode;				/* The mode */
	uint32 flag;			/* Long value that represents the mode */
	uint16 flags;			/* CBM_* above */

	/* Function to retrieve the value associated to the mode (optional) */
	char * (*getvalue) 		(Channel *chan);
	char * (*csgetvalue) 	(ChannelInfo *ci);
};

struct cmmode_ {
	void (*addmask) (Channel *chan, const char *mask);
	void (*delmask) (Channel *chan, const char *mask);
};

struct cumode_ {
	int16 status;			/* CUS_* below */
	int16 flags;			/* CUF_* below */

	int (*is_valid) (User *user, Channel *chan, int servermode);
};

/* Channel user mode flags */

#define CUF_PROTECT_BOTSERV 	0x0001

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

struct channel_ {
	Channel *next, *prev;
	char name[CHANMAX];
	ChannelInfo *ci;			/* Corresponding ChannelInfo */
	time_t creation_time;		/* When channel was created */
	char *topic;
	char topic_setter[NICKMAX];		/* Who set the topic */
	time_t topic_time;			/* When topic was set */
	uint32 mode;			/* Binary modes only */
	uint32 limit;			/* 0 if none */
	char *key;				/* NULL if none */
	char *redirect;			/* +L; NULL if none */
	char *flood;			/* +f; NULL if none */
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

/* Constants for news types. */

#define NEWS_LOGON		0
#define NEWS_OPER		1
#define NEWS_RANDOM 		2

/*************************************************************************/

/* Ignorance list data. */

typedef struct ignore_data {
	struct ignore_data *prev, *next;
	char *mask;
	time_t time;	/* When do we stop ignoring them? */
} IgnoreData;

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

struct newsitem_ {
	uint16 type;
	uint32 num;				  /* Numbering is separate for login and oper news */
	char *text;
	char who[NICKMAX];
	time_t time;
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

struct session_ {
	Session *prev, *next;
	char *host;
	int count;				  /* Number of clients with this host */
	int hits;				   /* Number of subsequent kills for a host */
};

/*************************************************************************/
typedef struct ircd_modes_ {
		int user_invis;
		int user_oper;
		int chan_invite;
		int chan_secret;
		int chan_private;
		int chan_key;
		int chan_limit;
} IRCDModes;



/*************************************************************************/
/**
 * DEFCON Defines
 **/
#define DEFCON_NO_NEW_CHANNELS 		1	/* No New Channel Registrations */
#define DEFCON_NO_NEW_NICKS 		2	/* No New Nick Registrations */
#define DEFCON_NO_MLOCK_CHANGE		4	/* No MLOCK changes */
#define DEFCON_FORCE_CHAN_MODES		8	/* Force Chan Mode */
#define DEFCON_REDUCE_SESSION		16	/* Reduce Session Limit */
#define DEFCON_NO_NEW_CLIENTS		32	/* Kill any NEW clients */
#define DEFCON_OPER_ONLY		64	/* Restrict services to oper's only */
#define DEFCON_SILENT_OPER_ONLY		128	/* Silently ignore non-opers */
#define DEFCON_AKILL_NEW_CLIENTS	256	/* AKILL any new clients */
#define DEFCON_NO_NEW_MEMOS		512	/* No New Memos Sent */

/*************************************************************************/

/* Memo Flags */
#define MF_UNREAD	0x0001	/* Memo has not yet been read */
#define MF_RECEIPT	0x0002	/* Sender requested receipt */
#define MF_NOTIFYS	  0x0004  /* Memo is a notification of receitp */

/* Nickname status flags: */
#define NS_VERBOTEN	0x0002	  /* Nick may not be registered or used */
#define NS_NO_EXPIRE	0x0004	  /* Nick never expires */
#define NS_IDENTIFIED	0x8000	  /* User has IDENTIFY'd */
#define NS_RECOGNIZED	0x4000	  /* ON_ACCESS true && SECURE flag not set */
#define NS_ON_ACCESS	0x2000	  /* User comes from a known address */
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
#define NI_SERVICES_OPER 	0x00001000  /* User is a Services operator */
#define NI_SERVICES_ADMIN	0x00002000  /* User is a Services admin */
#define NI_ENCRYPTEDPW		0x00004000  /* Nickname password is encrypted */
#define NI_SERVICES_ROOT		0x00008000  /* User is a Services root */
#define NI_MEMO_MAIL			0x00010000  /* User gets email on memo */
#define NI_HIDE_STATUS		  0x00020000  /* Don't show services access status */
#define NI_SUSPENDED		0x00040000  /* Nickname is suspended */
#define NI_AUTOOP		0x00080000  /* Autoop nickname in channels */
#define NI_NOEXPIRE		0x00100000 /* nicks in this group won't expire */

// Old NS_VERBOTEN, very fucking temporary.
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

#define CUS_OP			0x0001
#define CUS_VOICE		0x0002
#define CUS_HALFOP		0x0004		/* Halfop (+h) */
#define CUS_OWNER		0x0008		/* Owner/Founder (+q) */
#define CUS_PROTECT		0x0010		/* Protected users (+a) */
#define CUS_DEOPPED		0x0080		/* User has been specifically deopped */

#define MUT_DEOP   		0
#define MUT_OP			1
#define MUT_DEVOICE		2
#define MUT_VOICE		3
#define MUT_DEHALFOP		4
#define MUT_HALFOP		5
#define MUT_DEPROTECT   	6
#define MUT_PROTECT	 	7

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
#define CAPAB_UMODE2	0x02000000
#define CAPAB_VL		0x04000000
#define CAPAB_TLKEXT	0x08000000
#define CAPAB_CHANMODE  0x10000000
#define CAPAB_SJB64	 0x20000000
#define CAPAB_NICKCHARS 0x40000000

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

class IRCDProto {

private:

		virtual void SendSVSKillInternal(const char *, const char *, const char *) = 0;
		virtual void SendModeInternal(BotInfo *, const char *, const char *) = 0;
		virtual void SendKickInternal(BotInfo *bi, const char *, const char *, const char *) = 0;
		virtual void SendNoticeChanopsInternal(BotInfo *bi, const char *, const char *) = 0;
		virtual void SendMessageInternal(BotInfo *bi, const char *dest, const char *buf)
		{
			if (NSDefFlags & NI_MSG)
				SendPrivmsgInternal(bi, dest, buf);
			else
				SendNoticeInternal(bi, dest, buf);
		}
		virtual void SendNoticeInternal(BotInfo *bi, const char *dest, const char *msg)
		{
			send_cmd(ircd->ts6 ? bi->uid : bi->nick, "NOTICE %s :%s", dest, msg);
		}
		virtual void SendPrivmsgInternal(BotInfo *bi, const char *dest, const char *buf)
		{
			send_cmd(ircd->ts6 ? bi->uid : bi->nick, "PRIVMSG %s :%s", dest, buf);
		}
		virtual void SendQuitInternal(BotInfo *bi, const char *buf)
		{
			if (buf)
				send_cmd(ircd->ts6 ? bi->uid : bi->nick, "QUIT :%s", buf);
			else
				send_cmd(ircd->ts6 ? bi->uid : bi->nick, "QUIT");
		}
		virtual void SendPartInternal(BotInfo *bi, const char *chan, const char *buf)
		{
			if (buf)
				send_cmd(ircd->ts6 ? bi->uid : bi->nick, "PART %s :%s", chan, buf);
			else
				send_cmd(ircd->ts6 ? bi->uid : bi->nick, "PART %s", chan);
		}
		virtual void SendGlobopsInternal(const char *source, const char *buf)
		{
			BotInfo *bi = findbot(source);
			if (bi)
				send_cmd(ircd->ts6 ? bi->uid : bi->nick, "GLOBOPS :%s", buf);
			else
				send_cmd(ServerName, "GLOBOPS :%s", buf);
		}
		virtual void SendCTCPInternal(BotInfo *bi, const char *dest, const char *buf)
		{
			char *s = normalizeBuffer(buf);
			send_cmd(ircd->ts6 ? bi->uid : bi->nick, "NOTICE %s :\1%s\1", dest, s);
			delete [] s;
		}
		virtual void SendNumericInternal(const char *source, int numeric, const char *dest, const char *buf)
		{
			send_cmd(source, "%03d %s %s", numeric, dest, buf);
		}
	public:

		virtual ~IRCDProto() { }

		virtual void SendSVSNOOP(const char *, int) { }
		virtual void SendAkillDel(const char *, const char *) = 0;
		virtual void SendTopic(BotInfo *, const char *, const char *, const char *, time_t) = 0;
		virtual void SendVhostDel(User *) { }
		virtual void SendAkill(const char *, const char *, const char *, time_t, time_t, const char *) = 0;
		virtual void SendSVSKill(const char *source, const char *user, const char *fmt, ...)
		{
			va_list args;
			char buf[BUFSIZE] = "";
			va_start(args, fmt);
			vsnprintf(buf, BUFSIZE - 1, fmt, args);
			va_end(args);
			SendSVSKillInternal(source, user, buf);
		}
		virtual void SendSVSMode(User *, int, const char **) = 0;
		virtual void SendMode(BotInfo *bi, const char *dest, const char *fmt, ...)
		{
			va_list args;
			char buf[BUFSIZE] = "";
			va_start(args, fmt);
			vsnprintf(buf, BUFSIZE - 1, fmt, args);
			va_end(args);
			SendModeInternal(bi, dest, buf);
		}
		virtual void SendClientIntroduction(const char *, const char *, const char *, const char *, const char *, const char *uid) = 0;
		virtual void SendKick(BotInfo *bi, const char *chan, const char *user, const char *fmt, ...)
		{
			va_list args;
			char buf[BUFSIZE] = "";
			va_start(args, fmt);
			vsnprintf(buf, BUFSIZE - 1, fmt, args);
			va_end(args);
			SendKickInternal(bi, chan, user, buf);
		}
		virtual void SendNoticeChanops(BotInfo *bi, const char *dest, const char *fmt, ...)
		{
			va_list args;
			char buf[BUFSIZE] = "";
			va_start(args, fmt);
			vsnprintf(buf, BUFSIZE - 1, fmt, args);
			va_end(args);
			SendNoticeChanopsInternal(bi, dest, buf);
		}
		virtual void SendMessage(BotInfo *bi, const char *dest, const char *fmt, ...)
		{
			va_list args;
			char buf[BUFSIZE] = "";
			va_start(args, fmt);
			vsnprintf(buf, BUFSIZE - 1, fmt, args);
			va_end(args);
			SendMessageInternal(bi, dest, buf);
		}
		virtual void SendNotice(BotInfo *bi, const char *dest, const char *fmt, ...)
		{
			va_list args;
			char buf[BUFSIZE] = "";
			va_start(args, fmt);
			vsnprintf(buf, BUFSIZE - 1, fmt, args);
			va_end(args);
			SendNoticeInternal(bi, dest, buf);
		}
		virtual void SendAction(BotInfo *bi, const char *dest, const char *fmt, ...)
		{
			va_list args;
			char buf[BUFSIZE] = "", actionbuf[BUFSIZE] = "";
			va_start(args, fmt);
			vsnprintf(buf, BUFSIZE - 1, fmt, args);
			va_end(args);
			snprintf(actionbuf, BUFSIZE - 1, "%cACTION %s%c", 1, buf, 1);
			SendPrivmsgInternal(bi, dest, actionbuf);
		}
		virtual void SendPrivmsg(BotInfo *bi, const char *dest, const char *fmt, ...)
		{
			va_list args;
			char buf[BUFSIZE] = "";
			va_start(args, fmt);
			vsnprintf(buf, BUFSIZE - 1, fmt, args);
			va_end(args);
			SendPrivmsgInternal(bi, dest, buf);
		}
		virtual void SendGlobalNotice(BotInfo *bi, const char *dest, const char *msg)
		{
			send_cmd(ircd->ts6 ? bi->uid : bi->nick, "NOTICE %s%s :%s", ircd->globaltldprefix, dest, msg);
		}
		virtual void SendGlobalPrivmsg(BotInfo *bi, const char *dest, const char *msg)
		{
			send_cmd(ircd->ts6 ? bi->uid : bi->nick, "PRIVMSG %s%s :%s", ircd->globaltldprefix, dest, msg);
		}
		virtual void SendBotOp(const char *, const char *) = 0;

		/** XXX: This is a hack for NickServ enforcers. It is deprecated.
		 * If I catch any developer using this in new code, I will RIP YOUR BALLS OFF.
		 * Thanks.
		 * -- w00t
		 */
		virtual void SendQuit(const char *nick, const char *buf) MARK_DEPRECATED
		{
			send_cmd(nick, "QUIT");
		}
		virtual void SendQuit(BotInfo *bi, const char *fmt, ...)
		{
			va_list args;
			char buf[BUFSIZE] = "";
			va_start(args, fmt);
			vsnprintf(buf, BUFSIZE - 1, fmt, args);
			va_end(args);
			SendQuitInternal(bi, buf);
		}
		virtual void SendPong(const char *servname, const char *who)
		{
			send_cmd(servname, "PONG %s", who);
		}
		virtual void SendJoin(BotInfo *bi, const char *, time_t) = 0;
		virtual void SendSQLineDel(const char *) = 0;
		virtual void SendInvite(BotInfo *bi, const char *chan, const char *nick)
		{
			send_cmd(ircd->ts6 ? bi->uid : bi->nick, "INVITE %s %s", nick, chan);
		}
		virtual void SendPart(BotInfo *bi, const char *chan, const char *fmt, ...)
		{
			if (fmt) {
				va_list args;
				char buf[BUFSIZE] = "";
				va_start(args, fmt);
				vsnprintf(buf, BUFSIZE - 1, fmt, args);
				va_end(args);
				SendPartInternal(bi, chan, buf);
			}
			else SendPartInternal(bi, chan, NULL);
		}
		virtual void SendGlobops(const char *source, const char *fmt, ...)
		{
			va_list args;
			char buf[BUFSIZE] = "";
			va_start(args, fmt);
			vsnprintf(buf, BUFSIZE - 1, fmt, args);
			va_end(args);
			SendGlobopsInternal(source, buf);
		}
		virtual void SendSQLine(const char *, const char *) = 0;
		virtual void SendSquit(const char *servname, const char *message)
		{
			send_cmd(NULL, "SQUIT %s :%s", servname, message);
		}
		virtual void SendSVSO(const char *, const char *, const char *) { }
		virtual void SendChangeBotNick(BotInfo *bi, const char *newnick)
		{
			send_cmd(ircd->ts6 ? bi->uid : bi->nick, "NICK %s", newnick);
		}
		virtual void SendForceNickChange(const char *oldnick, const char *newnick, time_t when)
		{
			send_cmd(NULL, "SVSNICK %s %s :%ld", oldnick, newnick, static_cast<long>(when));
		}
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
		virtual void SendSVID(const char *, time_t) { }
		virtual void SendUnregisteredNick(User *) { }
		virtual void SendSVID2(User *, const char *) { }
		virtual void SendSVID3(User *, const char *) { }
		virtual void SendCTCP(BotInfo *bi, const char *dest, const char *fmt, ...)
		{
			va_list args;
			char buf[BUFSIZE] = "";
			va_start(args, fmt);
			vsnprintf(buf, BUFSIZE - 1, fmt, args);
			va_end(args);
			SendCTCPInternal(bi, dest, buf);
		}
		virtual void SendSVSJoin(const char *, const char *, const char *, const char *) { }
		virtual void SendSVSPart(const char *, const char *, const char *) { }
		virtual void SendSWhois(const char *, const char *, const char *) { }
		virtual void SendEOB() { }
		virtual void SendServer(const char *, int, const char *) = 0;
		virtual void ProcessUsermodes(User *, int, const char **) = 0;
		virtual int IsNickValid(const char *) { return 1; }
		virtual int IsChannelValid(const char *) { return 1; }
		virtual int IsFloodModeParamValid(const char *) { return 0; }
		virtual void SendNumeric(const char *source, int numeric, const char *dest, const char *fmt, ...)
		{
			va_list args;
			char buf[BUFSIZE] = "";
			va_start(args, fmt);
			vsnprintf(buf, BUFSIZE - 1, fmt, args);
			va_end(args);
			SendNumericInternal(source, numeric, dest, *buf ? buf : NULL);
		}
};

class IRCDTS6Proto : public IRCDProto
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

/*************************************************************************/

#endif	/* SERVICES_H */
