/*
 *
 * (C) 2003 Anope Team
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

/* Some Linux boxes (or maybe glibc includes) require this for the
 * prototype of strsignal(). */
#define _GNU_SOURCE

/* Some AIX boxes define int16 and int32 on their own.  Blarph. */
#if INTTYPE_WORKAROUND
# define int16 builtin_int16
# define int32 builtin_int32
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>	/* for umask() on some systems */
#include <sys/types.h>
#include <sys/time.h>

#ifdef USE_RDB
# define MAX_SQL_BUF   4096
#endif

#ifdef USE_MYSQL
# define MYSQL_WARNING 2
# define MYSQL_ERROR   4
#ifdef HAVE_MYSQL_MYSQL_H
# include <mysql.h>
# include <errmsg.h>
#else
# include <mysql/mysql.h>
# include <mysql/errmsg.h>
#endif
#endif

#if HAVE_STRINGS_H
# include <strings.h>
#endif

#if HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

#ifdef USE_THREADS
# include <pthread.h>
#endif

#ifdef _AIX
/* Some AIX boxes seem to have bogus includes that don't have these
 * prototypes. */
extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *, const char *, size_t);
# if 0	/* These break on some AIX boxes (4.3.1 reported). */
extern int gettimeofday(struct timeval *, struct timezone *);
extern int socket(int, int, int);
extern int bind(int, struct sockaddr *, int);
extern int connect(int, struct sockaddr *, int);
extern int shutdown(int, int);
# endif
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
extern int toupper(char), tolower(char);

/* We also have our own encrypt(). */
#define encrypt encrypt_


#if INTTYPE_WORKAROUND
# undef int16
# undef int32
#endif


/* Miscellaneous definitions. */
#include "defs.h"
#include "slist.h"

/*************************************************************************/

typedef struct server_ Server;
typedef struct user_ User;
typedef struct channel_ Channel;
typedef struct ModuleData_ ModuleData;			/* ModuleData struct */
typedef struct ModuleDataItem_ ModuleDataItem;		/* A Module Data Item struct */
typedef struct memo_ Memo;
typedef struct nickrequest_ NickRequest;
typedef struct nickalias_ NickAlias;
typedef struct nickcore_ NickCore;
typedef struct botinfo_ BotInfo;
typedef struct chaninfo_ ChannelInfo;
typedef struct badword_ BadWord;
typedef struct bandata_ BanData;
typedef struct userdata_ UserData;
typedef struct mailinfo_ MailInfo;
typedef struct akill_ Akill;
typedef struct sxline_ SXLine;
typedef struct hostcore_ HostCore;
typedef struct newsitem_ NewsItem;
typedef struct hostcache_ HostCache;
typedef struct exception_ Exception;
typedef struct cbmode_ CBMode;
typedef struct cbmodeinfo_ CBModeInfo;
typedef struct cmmode_ CMMode;
typedef struct csmode_ CSMode;
typedef struct cumode_ CUMode;
typedef struct csmodeutil_ CSModeUtil;

/*************************************************************************/

typedef enum { false, true } boolean;

/*************************************************************************/

/* Protocol tweaks */


#include "hybrid.h"
#include "viagra.h"
#include "bahamut.h"
#include "rageircd.h"
#include "ptlink.h"
#include "ultimate2.h"
#include "unreal31.h"
#include "ultimate3.h"
#include "dreamforge.h"
#include "unreal32.h"

typedef struct ircdvars_ IRCDVar;
typedef struct ircdcapab_ IRCDCAPAB;

struct ircdvars_ {
	char *name;				/* Name of the ChanServ command */
	char *nickservmode;			/* Mode used by NickServ	*/
	char *chanservmode;			/* Mode used by ChanServ	*/
	char *memoservmode;			/* Mode used by MemoServ	*/
	char *hostservmode;			/* Mode used by HostServ	*/
	char *operservmode;			/* Mode used by OperServ	*/
	char *botservmode;			/* Mode used by BotServ		*/
	char *helpservmode;			/* Mode used by HelpServ	*/
	char *devnullmode;			/* Mode used by Dev/Null	*/
	char *globalmode;			/* Mode used by Global		*/
	char *nickservaliasmode;		/* Mode used by NickServ Alias	*/
	char *chanservaliasmode;		/* Mode used by ChanServ Alias	*/
	char *memoservaliasmode;		/* Mode used by MemoServ Alias	*/
	char *hostservaliasmode;		/* Mode used by HostServ Alias	*/
	char *operservaliasmode;		/* Mode used by OperServ Alias	*/
	char *botservaliasmode;			/* Mode used by BotServ	 Alias	*/
	char *helpservaliasmode;		/* Mode used by HelpServ Alias	*/
	char *devnullvaliasmode;		/* Mode used by Dev/Null Alias	*/
	char *globalaliasmode;			/* Mode used by Global	 Alias	*/
	char *botserv_bot_mode;			/* Mode used by BotServ Bots	*/
	int max_symbols;			/* Chan Max Symbols		*/
	char *modestoremove;			/* Channel Modes to remove	*/
	char *botchanumode;			/* Modes set when botserv joins a channel */
	int svsnick;				/* Supports SVSNICK		*/
	int vhost;				/* Supports vhost		*/
	int owner;				/* Supports Owner		*/
	char *ownerset;				/* Mode to set for owner	*/
	char *ownerunset;			/* Mode to unset for a owner	*/
	char *modeonreg;			/* Mode on Register		*/
	char *modeonunreg;			/* Mode on Unregister		*/
	char *modeonnick;			/* Mode on nick change		*/
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
	int nickip;				/* Sends IP on NICK		*/
	int omode;				/* On the fly o:lines		*/
	int umode;				/* change user modes		*/
	int nickvhost;				/* Users vhost sent during NICK */
	int chgreal;				/* Change RealName		*/
	int extrahelp;				/* Lang file entry for extra 	*/
	uint32 noknock;				/* Channel Mode for no knock	*/
	uint32 adminmode;			/* Admin Only Channel Mode	*/
	uint32 defmlock;			/* Default mlock modes		*/
	uint32 vhostmode;			/* Vhost mode			*/
	int fmode;				/* +f				*/
	int Lmode;				/* +L				*/
	uint32 chan_fmode;			/* Mode 			*/
	uint32 chan_lmode;			/* Mode				*/
	int check_nick_id;			/* On nick change check if they could be identified */
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
};

/*************************************************************************/

/* File version for each database. Was one version for all before but was
   changed so they are now easier to maintain. =) */

#define BOT_VERSION 		10
#define CHAN_VERSION 		16
#define EXCEPTION_VERSION 	9
#define NEWS_VERSION 		9
#define NICK_VERSION 		13
#define PRE_NICK_VERSION 	1
#define OPER_VERSION 		13

/*************************************************************************/


/**
 * ModuleData strucs used to allow modules to add / delete module Data from existing structs
 */

struct ModuleDataItem_ {
	char *key;							/* The key */
	char *value;							/* The Value */
	ModuleDataItem *next;						/* The next ModuleDataItem in this list */
};

struct ModuleData_ {	
	char *moduleName;						/* Which module we belong to */
	ModuleDataItem *di;						/* The first Item they own */
	ModuleData *next;						/* The next ModuleData record */
};

/*************************************************************************/

/* Memo info structures.  Since both nicknames and channels can have memos,
 * we encapsulate memo data in a MemoList to make it easier to handle. */

struct memo_ {
    uint32 number;	/* Index number -- not necessarily array position! */
    int16 flags;
    time_t time;	/* When it was sent */
    char sender[NICKMAX];
    char *text;
    ModuleData *moduleData[1024]; 	/* Module saved data attached to the Memo */
};

typedef struct {
    int16 memocount, memomax;
    Memo *memos;
} MemoInfo;

/*************************************************************************/

/* NickServ nickname structures. */


struct nickrequest_ {
	NickRequest *next, *prev;
	char *nick;
	char *passcode;
	char *password;
	char *email;
	time_t requested;
	time_t lastmail;			/* Unsaved */
};

struct nickalias_ {
	NickAlias *next, *prev;
	char *nick;				/* Nickname */
	char *last_quit;			/* Last quit message */
	char *last_realname;			/* Last realname */
	char *last_usermask;			/* Last usermask */
        time_t time_registered;			/* When the nick was registered */
        time_t last_seen;			/* When it was seen online for the last time */
        int16 status;				/* See NS_* below */
        NickCore *nc;				/* I'm an alias of this */
        /* Not saved */
        ModuleData *moduleData[1024]; 		/* Module saved data attached to the nick alias */
    	User *u;				/* Current online user that has me */
};

struct nickcore_ {
	NickCore *next, *prev;

	char *display;				/* How the nick is displayed */
	char *pass;				/* Password of the nicks */
	char *email;				/* E-mail associated to the nick */
	char *greet;				/* Greet associated to the nick */
	uint32 icq;				/* ICQ # associated to the nick */
	char *url;				/* URL associated to the nick */
	int32 flags;				/* See NI_* below */
	uint16 language;			/* Language selected by nickname owner (LANG_*) */
        int16 accesscount;			/* # of entries */
        char **access;				/* Array of strings */
        MemoInfo memos;
        uint16 channelcount;			/* Number of channels currently registered */
        uint16 channelmax;			/* Maximum number of channels allowed */

        /* Unsaved data */
        ModuleData *moduleData[1024];	  	/* Module saved data attached to the NickCore */
        time_t lastmail;			/* Last time this nick record got a mail */
        SList aliases;				/* List of aliases */
};


/*************************************************************************/

/* Bot info structures. Note that since there won't be many bots,
 * they're not in a hash list.
 *	--lara
 */

struct botinfo_ {
 	BotInfo *next, *prev;

 	char *nick;    			/* Nickname of the bot */
 	char *user;    			/* Its user name */
 	char *host;    			/* Its hostname */
 	char *real;     		/* Its real name */
 	int16 flags;			/* Bot flags -- see BI_* below */
 	time_t created; 		/* Birth date ;) */
	int16 chancount;		/* Number of channels that use the bot. */
	/* Dynamic data */
 	time_t lastmsg;			/* Last time we said something */
};



/* Channel info structures.  Stored similarly to the nicks, except that
 * the second character of the channel name, not the first, is used to
 * determine the list.  (Hashing based on the first character of the name
 * wouldn't get very far. ;) ) */

/* Access levels for users. */
typedef struct {
    int16 in_use;	/* 1 if this entry is in use, else 0 */
    int16 level;
    NickCore *nc;	/* Guaranteed to be non-NULL if in use, NULL if not */
    time_t last_seen;
} ChanAccess;

/* Note that these two levels also serve as exclusive boundaries for valid
 * access levels.  ACCESS_FOUNDER may be assumed to be strictly greater
 * than any valid access level, and ACCESS_INVALID may be assumed to be
 * strictly less than any valid access level.
 */
#define ACCESS_FOUNDER	10000	/* Numeric level indicating founder access */
#define ACCESS_INVALID	-10000	/* Used in levels[] for disabled settings */

/* Levels for xOP */

#define ACCESS_VOP		3
#define ACCESS_HOP      	4
#define ACCESS_AOP		5
#define ACCESS_SOP		10

/* AutoKick data. */
typedef struct {
    int16 in_use;   /* Always 0 if not in use */
    int16 is_nick;	/* 1 if a regged nickname, 0 if a nick!user@host mask */

	int16 flags;

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
#define AK_STUCK    	0x0004

/* Structure used to contain bad words. */

struct badword_ {
	int16 in_use;
	char *word;
	int16 type;		/* BW_* below */
};

#define BW_ANY 		0
#define BW_SINGLE 	1
#define BW_START 	2
#define BW_END 		3


struct chaninfo_ {
    ChannelInfo *next, *prev;
    char name[CHANMAX];
    NickCore *founder;
    NickCore *successor;		/* Who gets the channel if the founder
					 			 * nick is dropped or expires */
    char founderpass[PASSMAX];
    char *desc;
    char *url;
    char *email;

    time_t time_registered;
    time_t last_used;
    char *last_topic;			/* Last topic on the channel */
    char last_topic_setter[NICKMAX];	/* Who set the last topic */
    time_t last_topic_time;		/* When the last topic was set */

    uint32 flags;				/* See below */
    char *forbidby;
    char *forbidreason;

    int16 bantype;
    int16 *levels;				/* Access levels for commands */

    int16 accesscount;
    ChanAccess *access;			/* List of authorized users */
    int16 akickcount;
    AutoKick *akick;			/* List of users to kickban */

    uint32 mlock_on, mlock_off;		/* See channel modes below */
    uint32 mlock_limit;			/* 0 if no limit */
    char *mlock_key;			/* NULL if no key */
    char *mlock_flood;			/* NULL if no +f */
    char *mlock_redirect;		/* NULL if no +L */

    char *entry_message;		/* Notice sent on entering channel */

    MemoInfo memos;

    struct channel_ *c;			/* Pointer to channel record (if   *
					 			 *    channel is currently in use) */
								 
    ModuleData *moduleData[1024]; 	/* Module saved data attached to the ChannelInfo */

    /* For BotServ */

    BotInfo *bi;					/* Bot used on this channel */
    uint32 botflags;				/* BS_* below */
    int16 *ttb;						/* Times to ban for each kicker */

    int16 bwcount;
    BadWord *badwords;				/* For BADWORDS kicker */
    int16 capsmin, capspercent;		/* For CAPS kicker */
    int16 floodlines, floodsecs;	/* For FLOOD kicker */
    int16 repeattimes;				/* For REPEAT kicker */
};

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
#define CA_ASSIGN       		13  /* BotServ ASSIGN command */
#define CA_BADWORDS     		14  /* BotServ BADWORDS command */
#define CA_NOKICK       		15  /* Not kicked by the bot */
#define CA_FANTASIA   			16
#define CA_SAY				17
#define CA_GREET            		18
#define CA_VOICEME			19
#define CA_VOICE			20
#define CA_GETKEY           		21
#define CA_AUTOHALFOP			22
#define CA_AUTOPROTECT			23
#define CA_OPDEOPME         		24
#define CA_HALFOPME         		25
#define CA_HALFOP           		26
#define CA_PROTECTME        		27
#define CA_PROTECT          		28
#define CA_KICKME           		29
#define CA_KICK             		30
#define CA_SIGNKICK			31
/* #define CA_AUTOADMIN        		32
#define CA_ADMINME          		33
#define CA_ADMIN            		34 */
#define CA_BANME            		32
#define CA_BAN              		33
#define CA_TOPIC            		34
#define CA_INFO             		35

#define CA_SIZE		36

/* BotServ SET flags */
#define BS_DONTKICKOPS      	0x00000001
#define BS_DONTKICKVOICES   	0x00000002
#define BS_FANTASY          	0x00000004
#define BS_SYMBIOSIS        	0x00000008
#define BS_GREET            	0x00000010
#define BS_NOBOT            	0x00000020

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
	char *name;			/* Name of the ChanServ command */
	char *bsname;			/* Name of the BotServ fantasy command */
	char *mode;			/* Mode (ie. +o) */
	int32 notice;			/* Notice flag (for the damn OPNOTICE) */
	int level;			/* Level required to use the command */
	int levelself;			/* Level required to use the command for himself */
};

/*************************************************************************/

/* Server data */

struct server_ {
    Server *next, *prev;
    
    char *name;		/* Server name 				*/
    uint16 hops;	/* Hops between services and server 	*/
    char *desc;		/* Server description 			*/
    uint16 flags;	/* Some info flags, as defined below 	*/
    char *suid;		/* Server Univeral ID		     	*/

    Server *links;	/* Linked list head for linked servers 	*/
    Server *uplink;	/* Server which pretends to be the uplink */
};

#define SERVER_ISME  0x0001
#define SERVER_JUPED 0x0002

/*************************************************************************/

/* Online user and channel data. */
struct user_ {
    User *next, *prev;

    char nick[NICKMAX];

    char *username;		/* ident			*/
    char *host;             	/* User's real hostname 	*/
    char *vhost;            	/* User's virtual hostname 	*/
    char *vident;           	/* User's virtual ident 	*/
    char *realname;		/* Realname 			*/
    Server *server;		/* Server user is connected to  */
    char *nickTrack;		/* Nick Tracking 		*/
    time_t timestamp;		/* Timestamp of the nick 	*/
    time_t my_signon;		/* When did _we_ see the user?  */
    uint32 svid;		/* Services ID 			*/
    uint32 mode;		/* See below 			*/
    char *uid;			/* Univeral ID			*/

    NickAlias *na;

    ModuleData *moduleData[1024];		/* defined for it, it should allow the module Add/Get */	
    
    int isSuperAdmin;		/* is SuperAdmin on or off? */

    struct u_chanlist {
		struct u_chanlist *next, *prev;
		Channel *chan;
		int16 status;		/* Associated flags; see CSTATUS_* below. */
    } *chans;				/* Channels user has joined */

    struct u_chaninfolist {
		struct u_chaninfolist *next, *prev;
		ChannelInfo *chan;
    } *founder_chans;			/* Channels user has identified for */

    short invalid_pw_count;		/* # of invalid password attempts */
    time_t invalid_pw_time;		/* Time of last invalid password */

    time_t lastmemosend;		/* Last time MS SEND command used */
    time_t lastnickreg;			/* Last time NS REGISTER cmd used */
    time_t lastmail;			/* Last time this user sent a mail */
};



struct cbmode_ {
	uint32 flag;			/* Long value that represents the mode */
	uint16 flags;			/* Flags applying to this mode (CBM_* below) */

	/* Function to associate a value with the mode */
	void (*setvalue)	(Channel *chan, char *value);
	void (*cssetvalue)	(ChannelInfo *ci, char *value);
};

#define CBM_MINUS_NO_ARG 	0x0001		/* No argument for unset */
#define CBM_NO_MLOCK		0x0002		/* Can't be MLOCKed */
#define CBM_NO_USER_MLOCK   	0x0004      	/* Can't be MLOCKed by non-opers */

struct cbmodeinfo_ {
	char mode;				/* The mode */
	uint32 flag;			/* Long value that represents the mode */
	uint16 flags;			/* CBM_* above */

	/* Function to retrieve the value associated to the mode (optional) */
	char * (*getvalue) 		(Channel *chan);
	char * (*csgetvalue) 	(ChannelInfo *ci);
};

struct cmmode_ {
	void (*addmask) (Channel *chan, char *mask);
	void (*delmask) (Channel *chan, char *mask);
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
    int32 bancount, bansize;
    char **bans;
    int32 exceptcount, exceptsize;
    char **excepts;
    struct c_userlist {
		struct c_userlist *next, *prev;
		User *user;
		UserData *ud;
    } *users;
    int16 usercount;

    BanData *bd;

    time_t server_modetime;		/* Time of last server MODE */
    time_t chanserv_modetime;	/* Time of last check_modes() */
    int16 server_modecount;		/* Number of server MODEs this second */
    int16 chanserv_modecount;	/* Number of check_mode()'s this sec */
    int16 bouncy_modes;			/* Did we fail to set modes here? */
};

/*************************************************************************/

/* Constants for news types. */

#define NEWS_LOGON		0
#define NEWS_OPER		1
#define NEWS_RANDOM 		2

/*************************************************************************/

/* Ignorance list data. */

typedef struct ignore_data {
    struct ignore_data *next;
    char who[NICKMAX];
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

/*************************************************************************/

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
    int16 type;
    int32 num;                  /* Numbering is separate for login and oper news */
    char *text;
    char who[NICKMAX];
    time_t time;
};

/*************************************************************************/


struct exception_ {
    char *mask;                 /* Hosts to which this exception applies */
    int limit;                  /* Session limit for exception */
    char who[NICKMAX];          /* Nick of person who added the exception */
    char *reason;               /* Reason for exception's addition */
    time_t time;                /* When this exception was added */
    time_t expires;             /* Time when it expires. 0 == no expiry */
    int num;                    /* Position in exception list; used to track
                                 * positions when deleting entries. It is
                                 * symbolic and used internally. It is
                                 * calculated at load time and never saved. */
};

/*************************************************************************/

/* Proxy stuff */



struct hostcache_ {
	HostCache *prev, *next;

	char *host;							/* The hostname */
	uint32 ip;							/* The IP address */

	int16 status;						/* HC_* below */
	time_t used;						/* When was this entry last used? */
};

/* We assume that values < 0 are in-progress values, and values > 0 are
 * proxy value. 0 is the normal value.
 */

#define HC_QUEUED		-2				/* Waiting to be scanned */
#define HC_PROGRESS		-1				/* Currently being scanned */
#define HC_NORMAL		 0				/* No proxy found on this host */
#define HC_WINGATE		 1				/* Wingate found */
#define HC_SOCKS4		 2				/* Socks4 found */
#define HC_SOCKS5		 3				/* Socks5 found */
#define HC_HTTP			 4				/* HTTP proxy found */

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


/* Nickname status flags: */
#define NS_VERBOTEN	0x0002      /* Nick may not be registered or used */
#define NS_NO_EXPIRE	0x0004      /* Nick never expires */
#define NS_IDENTIFIED	0x8000      /* User has IDENTIFY'd */
#define NS_RECOGNIZED	0x4000      /* ON_ACCESS true && SECURE flag not set */
#define NS_ON_ACCESS	0x2000      /* User comes from a known address */
#define NS_KILL_HELD	0x1000      /* Nick is being held after a kill */
#define NS_GUESTED	0x0100	    /* SVSNICK has been sent but nick has not
				     				 * yet changed. An enforcer will be
				     				 * introduced when it does change. */
#define NS_MASTER	0x0200		/* Was a master nick; used to import old databases */
#define NS_TRANSGROUP	0xC000		/* Status flags that can be passed to a nick of the
									   same group during nick change */
#define NS_TEMPORARY	0xFF00      /* All temporary status flags */
/* These two are not used anymore */
#define NS_OLD_ENCRYPTEDPW	0x0001  /* Nickname password is encrypted */

/* Nickname setting flags: */
#define NI_KILLPROTECT		0x00000001  /* Kill others who take this nick */
#define NI_SECURE		0x00000002  /* Don't recognize unless IDENTIFY'd */
#define NI_MSG                  0x00000004  /* Use PRIVMSGs instead of NOTICEs */
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
#define NI_SERVICES_ROOT        0x00008000  /* User is a Services root */
#define NI_MEMO_MAIL            0x00010000  /* User gets email on memo */
#define NI_HIDE_STATUS          0x00020000  /* Don't show services access status */

/* Languages.  Never insert anything in the middle of this list, or
 * everybody will start getting the wrong language!  If you want to change
 * the order the languages are displayed in for NickServ HELP SET LANGUAGE,
 * do it in language.c.
 */
#define LANG_EN_US              0	/* United States English */
#define LANG_JA_JIS             1	/* Japanese (JIS encoding) */
#define LANG_JA_EUC             2	/* Japanese (EUC encoding) */
#define LANG_JA_SJIS            3	/* Japanese (SJIS encoding) */
#define LANG_ES                 4	/* Spanish */
#define LANG_PT                 5	/* Portugese */
#define LANG_FR                 6	/* French */
#define LANG_TR                 7	/* Turkish */
#define LANG_IT                 8	/* Italian */
#define LANG_DE                 9   	/* German */
#define LANG_CAT               10   	/* Catalan */
#define LANG_GR                11   	/* Greek */
#define LANG_NL                12   	/* Dutch */
#define LANG_RU                13   	/* Russian */
#define LANG_HUN               14   	/* Hungarian */
#define LANG_PL                15   	/* Polish */

#define NUM_LANGS              16	/* Number of languages */
#define USED_LANGS             13	/* Number of languages provided */


#define DEF_LANGUAGE	LANG_EN_US

#define BI_PRIVATE		0x0001

#define CUS_OP			0x0001
#define CUS_VOICE		0x0002
#define CUS_HALFOP		0x0004		/* Halfop (+h) */
#define CUS_OWNER		0x0008		/* Owner/Founder (+q) */
#define CUS_PROTECT		0x0010		/* Protected users (+a) */

#define MUT_DEOP   		0
#define MUT_OP			1
#define MUT_DEVOICE		2
#define MUT_VOICE		3
#define MUT_DEHALFOP		4
#define MUT_HALFOP		5
#define MUT_DEPROTECT   	6
#define MUT_PROTECT     	7


#define CAPAB_NOQUIT 	0x00000001
#define CAPAB_TSMODE 	0x00000002
#define CAPAB_UNCONNECT 0x00000004
#define CAPAB_NICKIP    0x00000008
#define CAPAB_NSJOIN    0x00000010
#define CAPAB_ZIP	0x00000020
#define CAPAB_BURST     0x00000040
#define CAPAB_TS3       0x00000080
#define CAPAB_TS5       0x00000100
#define CAPAB_DKEY      0x00000200
#define CAPAB_DOZIP	0x00000400
#define CAPAB_DODKEY    0x00000800
#define CAPAB_QS        0x00001000
#define CAPAB_SCS       0x00002000
#define CAPAB_PT4       0x00004000
#define CAPAB_UID       0x00008000
#define CAPAB_KNOCK     0x00010000
#define CAPAB_CLIENT    0x00020000
#define CAPAB_IPV6      0x00040000
#define CAPAB_SSJ5      0x00080000
#define CAPAB_SN2       0x00100000
#define CAPAB_VHOST     0x00200000
#define CAPAB_TOKEN     0x00400000
#define CAPAB_SSJ3      0x00800000
#define CAPAB_NICK2     0x01000000
#define CAPAB_UMODE2    0x02000000
#define CAPAB_VL        0x04000000
#define CAPAB_TLKEXT    0x08000000


/*************************************************************************/

#include "extern.h"

/*************************************************************************/

#endif	/* SERVICES_H */
