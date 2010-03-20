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

class ModuleException : public CoreException
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
struct EList;

typedef struct bandata_ BanData;
typedef struct mailinfo_ MailInfo;
typedef struct akill_ Akill;
typedef struct exception_ Exception;
typedef struct session_ Session;

#include "extensible.h"
#include "bots.h"
#include "opertype.h"
#include "modes.h"

/*************************************************************************/

/* Protocol tweaks */

typedef struct ircdvars_ IRCDVar;

struct ircdvars_ {
	const char *name;				/* Name of the IRCd command */
	const char *pseudoclient_mode;			/* Mode used by BotServ Bots	*/
	int max_symbols;			/* Chan Max Symbols		*/
	const char *botchanumode;			/* Modes set when botserv joins a channel */
	int svsnick;				/* Supports SVSNICK		*/
	int vhost;				/* Supports vhost		*/
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
	int check_nick_id;			/* On nick change check if they could be identified */
	int knock_needs_i;			/* Check if we needed +i when setting NOKNOCK */
	int token;					/* Does Anope support the tokens for the ircd */
	int sjb64;
	int svsmode_ucmode;			/* Can remove User Channel Modes with SVSMODE */
	int sglineenforce;
	int ts6;					/* ircd is TS6 */
	int p10;					/* ircd is P10  */
	int cidrchanbei;			/* channel bans/excepts/invites support CIDR (syntax: +b *!*@192.168.0.0/15)
							 * 0 for no support, 1 for strict cidr support, anything else
							 * for ircd specific support (nefarious only cares about first /mask) */
	const char *globaltldprefix;		/* TLD prefix used for Global */
	bool b_delay_auth;			/* Auth for users is sent after the initial NICK/UID command */
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

typedef struct {
	int16 memomax;
	std::vector<Memo *> memos;
} MemoInfo;


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
	void SetVhost(const std::string &ident, const std::string &host, const std::string &creator, time_t time = time(NULL));

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
	uint16 in_use;	/* 1 if this entry is in use, else 0 */
	int16 level;
	NickCore *nc;	/* Guaranteed to be non-NULL if in use, NULL if not */
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
 	bool InUse;
	/* Only one of these can be in use */
	std::string mask;
	NickCore *nc;

	std::string reason;
	std::string creator;
	time_t addtime;
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
	bool InUse;
	std::string word;
	BadWordType type;
};

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

/** Flags set on servers
 */
enum ServerFlag
{
	SERVER_START,

	/* This server is me */
	SERVER_ISME,
	/* This server was juped */
	SERVER_JUPED,
	/* This server is the current uplink */
	SERVER_ISUPLINK,

	SERVER_END
};

class Server : public Flags<ServerFlag>
{
 public:
	Server *next, *prev;

	char *name;	 /* Server name						*/
	uint16 hops;	/* Hops between services and server   */
	char *desc;	 /* Server description				 */
	char *suid;	 /* Server Univeral ID				 */
	SyncState sync; /* Server sync state (see above)	  */

	Server *links;	/* Linked list head for linked servers 	  */
	Server *uplink;	/* Server which pretends to be the uplink */
};

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
	uint32 cidr_ip;			 /* IP mask for CIDR matching */
	uint32 cidr_mask;		   /* Netmask for CIDR matching */
	char *nick, *user, *host, *mask;
};

struct EList
{
	Entry *entries;
	int32 count;
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
	std::string who;
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

enum SXLineType
{
	SX_SGLINE,
	SX_SQLINE,
	SX_SZLINE
};

struct SXLine {
	char *mask;
	char *by;
	char *reason;
	time_t seton;
	time_t expires;
};


/************************************************************************/

struct exception_ {
	char *mask;				 /* Hosts to which this exception applies */
	int limit;				  /* Session limit for exception */
	char *who;		/* Nick of person who added the exception */
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

/*************************************************************************/

/* Types of capab
 */
enum CapabType
{
	CAPAB_BEGIN,

	CAPAB_NOQUIT,
	CAPAB_TSMODE,
	CAPAB_UNCONNECT,
	CAPAB_NICKIP,
	CAPAB_NSJOIN,
	CAPAB_ZIP,
	CAPAB_BURST,
	CAPAB_TS3,
	CAPAB_TS5,
	CAPAB_DKEY,
	CAPAB_DOZIP,
	CAPAB_DODKEY,
	CAPAB_QS,
	CAPAB_SCS,
	CAPAB_PT4,
	CAPAB_UID,
	CAPAB_KNOCK,
	CAPAB_CLIENT,
	CAPAB_IPV6,
	CAPAB_SSJ5,
	CAPAB_SN2,
	CAPAB_VHOST,
	CAPAB_TOKEN,
	CAPAB_SSJ3,
	CAPAB_NICK2,
	CAPAB_VL,
	CAPAB_TLKEXT,
	CAPAB_CHANMODE,
	CAPAB_SJB64,
	CAPAB_NICKCHARS,

	CAPAB_END
};

/* CAPAB stuffs */
struct CapabInfo
{
	std::string Token;
	CapabType Flag;
};

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
		virtual void SendAkill(Akill *) = 0;
		virtual void SendAkillDel(Akill *) = 0;
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
		virtual void SendPong(const char *servname, const char *who);
		virtual void SendJoin(BotInfo *bi, const char *, time_t) = 0;
		virtual void SendSQLineDel(const std::string &) = 0;
		virtual void SendInvite(BotInfo *bi, const char *chan, const char *nick);
		virtual void SendPart(BotInfo *bi, Channel *chan, const char *fmt, ...);
		virtual void SendGlobops(BotInfo *source, const char *fmt, ...);
		virtual void SendSQLine(const std::string &, const std::string &) = 0;
		virtual void SendSquit(const char *servname, const char *message);
		virtual void SendSVSO(const char *, const char *, const char *) { }
		virtual void SendChangeBotNick(BotInfo *bi, const char *newnick);
		virtual void SendForceNickChange(User *u, const char *newnick, time_t when);
		virtual void SendVhost(User *, const std::string &, const std::string &) { }
		virtual void SendConnect() = 0;
		virtual void SendSVSHold(const char *) { }
		virtual void SendSVSHoldDel(const char *) { }
		virtual void SendSGLineDel(SXLine *) { }
		virtual void SendSZLineDel(SXLine *) { }
		virtual void SendSZLine(SXLine *) { }
		virtual void SendSGLine(SXLine *) { }
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

enum LogLevel
{
	LOG_NORMAL,
	LOG_DEBUG,
	LOG_DEBUG_2,
	LOG_DEBUG_3,
	LOG_DEBUG_4
};

class CoreExport Alog
{
 private:
	std::stringstream buf;
	bool logit;
 public:
	Alog(LogLevel val = LOG_NORMAL);
	~Alog();
	template<typename T> Alog& operator<<(T val)
	{
		if (logit)
			buf << val;
		return *this;
	}
};


class CoreExport Anope
{
 public:
	/** Check whether two strings match.
	 * @param mask The pattern to check (e.g. foo*bar)
	 * @param str The string to check against the pattern (e.g. foobar)
	 * @param case_sensitive Whether or not the match is case sensitive, default false.
	 */
	static bool Match(const std::string &str, const std::string &mask, bool case_sensitive = false);
	inline static bool Match(const ci::string &str, const ci::string &mask) { return Match(str.c_str(), mask.c_str(), false); }
};

/*************************************************************************/

#include "timers.h"

/** Timer for colliding nicks to force people off of nicknames
 */
class NickServCollide : public Timer
{
 public:
	/* NickAlias of the nick who were kicking off */
	NickAlias *na;
	/* Return for the std::map::insert */
	std::pair<std::map<NickAlias *, NickServCollide *>::iterator, bool> it;

	/** Default constructor
	 * @param nickalias The nick alias were kicking off
	 * @param delay How long to delay before kicking the user off the nick
	 */
	NickServCollide(NickAlias *nickalias, time_t delay);

	/** Default destructor
	 */
	~NickServCollide();

	/** Called when the delay is up
	 * @param t The current time
	 */
	void Tick(time_t t);

	/** Clear all timers for a nick
	 * @param na The nick to remove the timers for
	 */
	static void ClearTimers(NickAlias *na);
};

/** Timers for releasing nicks to be available for use
 */
class NickServRelease : public Timer
{
 public:
	/* The nick */
 	NickAlias *na;
	/* The uid of the services enforcer client (used for TS6 ircds) */
	std::string uid;
	/* Return for std::map::insert */
	std::pair<std::map<NickAlias *, NickServRelease *>::iterator, bool> it;

	/** Default constructor
	 * @param nickalias The nick
	 * @param delay The delay before the nick is released
	 */
	NickServRelease(NickAlias *nickalias, time_t delay);

	/** Default destructor
	 */
	~NickServRelease();

	/** Called when the delay is up
	 * @param t The current time
	 */
	void Tick(time_t t);

	/** Clear all timers for a nick
	 * @param na The nick to remove the timers for
	 * @param dorelase true to actually call release(), false to just remove the timers
	 */
	static void ClearTimers(NickAlias *na, bool dorelease = false);
};

#endif	/* SERVICES_H */
