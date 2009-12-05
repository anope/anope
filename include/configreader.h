#ifndef _CONFIGREADER_H_
#define _CONFIGREADER_H_

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <deque>

/** A configuration key and value pair
 */
typedef std::pair<std::string, std::string> KeyVal;

/** A list of related configuration keys and values
 */
typedef std::vector<KeyVal> KeyValList;

/** An entire config file, built up of KeyValLists
 */
typedef std::multimap<std::string, KeyValList> ConfigDataHash;

// Required forward definitions
class ServerConfig;

/** Types of data in the core config
 */
enum ConfigDataType {
	DT_NOTHING, // No data
	DT_INTEGER, // Integer
	DT_UINTEGER, // Unsigned Integer
	DT_LUINTEGER, // Long Unsigned Integer
	DT_CHARPTR, // Char pointer
	DT_STRING, // std::string
	DT_BOOLEAN, // Boolean
	DT_HOSTNAME, // Hostname syntax
	DT_NOSPACES, // No spaces
	DT_IPADDRESS, // IP address (v4, v6)
	DT_TIME, // Time value
	DT_NORELOAD = 32, // Item can't be reloaded after startup
	DT_ALLOW_WILD = 64, // Allow wildcards/CIDR in DT_IPADDRESS
	DT_ALLOW_NEWLINE = 128 // New line characters allowed in DT_CHARPTR
};

/** Holds a config value, either string, integer or boolean.
 * Callback functions receive one or more of these, either on
 * their own as a reference, or in a reference to a deque of them.
 * The callback function can then alter the values of the ValueItem
 * classes to validate the settings.
 */
class ValueItem
{
		/** Actual data */
		std::string v;
	public:
		/** Initialize with an int */
		ValueItem(int);
		/** Initialize with a bool */
		ValueItem(bool);
		/** Initialize with a char pointer */
		ValueItem(const char *);
		/** Initialize with an std::string */
		ValueItem(const std::string &);
		/** Initialize with a long */
		ValueItem(long);
		/** Change value to a char pointer */
		//void Set(char *);
		/** Change value to a const char pointer */
		void Set(const char *);
		/** Change value to an std::string */
		void Set(const std::string &);
		/** Change value to an int */
		void Set(int);
		/** Get value as an int */
		int GetInteger();
		/** Get value as a string */
		const char *GetString() const;
		/** Get value as a string */
		inline const std::string &GetValue() const { return v; }
		/** Get value as a bool */
		bool GetBool();
};

/** The base class of the container 'ValueContainer'
 * used internally by the core to hold core values.
 */
class ValueContainerBase
{
	public:
		/** Constructor */
		ValueContainerBase() { }
		/** Destructor */
		virtual ~ValueContainerBase() { }
};

/** ValueContainer is used to contain pointers to different
 * core values such as the server name, maximum number of
 * clients etc.
 * It is specialized to hold a data type, then pointed at
 * a value in the ServerConfig class. When the value has been
 * read and validated, the Set method is called to write the
 * value safely in a type-safe manner.
 */
template<typename T> class ValueContainer : public ValueContainerBase
{
		/** Contained item */
		T val;
	public:
		/** Initialize with nothing */
		ValueContainer() : ValueContainerBase(), val(NULL) { }
		/** Initialize with a value of type T */
		ValueContainer(T Val) : ValueContainerBase(), val(Val) { }
		/** Initialize with a copy */
		ValueContainer(const ValueContainer &Val) : ValueContainerBase(), val(Val.val) { }
		ValueContainer &operator=(const ValueContainer &Val)
		{
			val = Val.val;
			return *this;
		}
		/** Change value to type T of size s */
		void Set(const T newval, size_t s)
		{
			memcpy(val, newval, s);
		}
};

/** This a specific version of ValueContainer to handle character arrays specially
 */
template<> class ValueContainer<char **> : public ValueContainerBase
{
		/** Contained item */
		char **val;
	public:
		/** Initialize with nothing */
		ValueContainer() : ValueContainerBase(), val(NULL) { }
		/** Initialize with a value of type T */
		ValueContainer(char **Val) : ValueContainerBase(), val(Val) { }
		/** Initialize with a copy */
		ValueContainer(const ValueContainer &Val) : ValueContainerBase(), val(Val.val) { }
		ValueContainer &operator=(const ValueContainer &Val)
		{
			val = Val.val;
			return *this;
		}
		/** Change value to type T of size s */
		void Set(const char *newval, size_t s)
		{
			if (*val) delete [] *val;
			if (!*newval) {
				*val = NULL;
				return;
			}
			*val = new char[s];
			strlcpy(*val, newval, s);
		}
};

/** This a specific version of ValueContainer to handle std::string specially
 */
template<> class ValueContainer<std::string *> : public ValueContainerBase
{
		/** Contained item */
		std::string *val;
	public:
		/** Initialize with nothing */
		ValueContainer() : ValueContainerBase(), val(NULL) { }
		/** Initialize with an std::string */
		ValueContainer(std::string *Val) : ValueContainerBase(), val(Val) { }
		/** Initialize with a copy */
		ValueContainer(const ValueContainer &Val) : ValueContainerBase(), val(Val.val) { }
		ValueContainer &operator=(const ValueContainer &Val)
		{
			val = Val.val;
			return *this;
		}
		/** Change value to given std::string */
		void Set(const std::string &newval)
		{
			*val = newval;
		}
		/** Change value to given char pointer */
		void Set(const char *newval)
		{
			*val = newval;
		}
};

/** A specialization of ValueContainer to hold a pointer to a bool
 */
typedef ValueContainer<bool *> ValueContainerBool;

/** A specialization of ValueContainer to hold a pointer to
 * an unsigned int
 */
typedef ValueContainer<unsigned *> ValueContainerUInt;

/** A specialization of ValueContainer to hold a pointer to
 * a long unsigned int
 */
typedef ValueContainer<long unsigned *> ValueContainerLUInt;

/** A specialization of ValueContainer to hold a pointer to
 * a char array.
 */
typedef ValueContainer<char **> ValueContainerChar;

/** A specialization of ValueContainer to hold a pointer to
 * an int
 */
typedef ValueContainer<int *> ValueContainerInt;

/** A specialization of ValueContainer to hold a pointer to
 * a time_t
 */
typedef ValueContainer<time_t *> ValueContainerTime;

/** A specialization of ValueContainer to hold a pointer to
 * an std::string
 */
typedef ValueContainer<std::string *> ValueContainerString;

/** A set of ValueItems used by multi-value validator functions
 */
typedef std::deque<ValueItem> ValueList;

/** A callback for validating a single value
 */
typedef bool (*Validator)(ServerConfig *, const char *, const char *, ValueItem &);
/** A callback for validating multiple value entries
 */
typedef bool (*MultiValidator)(ServerConfig *, const char *, const char **, ValueList &, int *, bool);
/** A callback indicating the end of a group of entries
 */
typedef bool (*MultiNotify)(ServerConfig *, const char *, bool);

/** Holds a core configuration item and its callbacks
 */
struct InitialConfig
{
	/** Tag name */
	const char *tag;
	/** Value name */
	const char *value;
	/** Default, if not defined */
	const char *default_value;
	/** Value containers */
	ValueContainerBase *val;
	/** Data types */
	int datatype;
	/** Validation function */
	Validator validation_function;
};

/** Holds a core configuration item and its callbacks
 * where there may be more than one item
 */
struct MultiConfig
{
	/** Tag name */
	const char *tag;
	/** One or more items within tag */
	const char *items[17];
	/** One or more defaults for items within tags */
	const char *items_default[17];
	/** One or more data types */
	int datatype[17];
	/** Initialization function */
	MultiNotify init_function;
	/** Validation function */
	MultiValidator validation_function;
	/** Completion function */
	MultiNotify finish_function;
};

/** This class holds the bulk of the runtime configuration for the ircd.
 * It allows for reading new config values, accessing configuration files,
 * and storage of the configuration data needed to run the ircd, such as
 * the servername, connect classes, /ADMIN data, MOTDs and filenames etc.
 */
class ServerConfig
{
	private:
		/** This variable holds the names of all
		 * files included from the main one. This
		 * is used to make sure that no files are
		 * recursively included.
		 */
		std::vector<std::string> include_stack;
		/** Check that there is only one of each configuration item
		 */
		bool CheckOnce(const char *);
	public:
		std::ostringstream errstr;
		ConfigDataHash newconfig;
		/** This holds all the information in the config file,
		 * it's indexed by tag name to a vector of key/values.
		 */
		ConfigDataHash config_data;
		/** Construct a new ServerConfig
		 */
		ServerConfig();
		/** Clears the include stack in preperation for a Read() call.
		 */
		void ClearStack();
		/** Read the entire configuration into memory
		 * and initialize this class. All other methods
		 * should be used only by the core.
		 */
		int Read(bool);
		/** Report a configuration error given in errormessage.
		 * @param bail If this is set to true, the error is sent to the console, and the program exits
		 * @param connection If this is set to a non-null value, and bail is false, the errors are spooled to
		 * this connection as SNOTICEs.
		 * If the parameter is NULL, the messages are spooled to all connections via WriteOpers as SNOTICEs.
		 */
		void ReportConfigError(const std::string &, bool);
		/** Load 'filename' into 'target', with the new config parser everything is parsed into
		 * tag/key/value at load-time rather than at read-value time.
		 */
		bool LoadConf(ConfigDataHash &, const char *, std::ostringstream &);
		/** Load 'filename' into 'target', with the new config parser everything is parsed into
		 * tag/key/value at load-time rather than at read-value time.
		 */
		bool LoadConf(ConfigDataHash &, const std::string &, std::ostringstream &);
		// Both these return true if the value existed or false otherwise
		/** Writes 'length' chars into 'result' as a string
		 */
		bool ConfValue(ConfigDataHash &, const char *, const char *, int, char *, int, bool = false);
		/** Writes 'length' chars into 'result' as a string
		 */
		bool ConfValue(ConfigDataHash &, const char *, const char *, const char *, int, char *, int, bool = false);
		/** Writes 'length' chars into 'result' as a string
		 */
		bool ConfValue(ConfigDataHash &, const std::string &, const std::string &, int, std::string &, bool = false);
		/** Writes 'length' chars into 'result' as a string
		 */
		bool ConfValue(ConfigDataHash &, const std::string &, const std::string &, const std::string &, int, std::string &, bool = false);
		/** Tries to convert the value to an integer and write it to 'result'
		 */
		bool ConfValueInteger(ConfigDataHash &, const char *, const char *, int, int &);
		/** Tries to convert the value to an integer and write it to 'result'
		 */
		bool ConfValueInteger(ConfigDataHash &, const char *, const char *, const char *, int, int &);
		/** Tries to convert the value to an integer and write it to 'result'
		 */
		bool ConfValueInteger(ConfigDataHash &, const std::string &, const std::string &, int, int &);
		/** Tries to convert the value to an integer and write it to 'result'
		 */
		bool ConfValueInteger(ConfigDataHash &, const std::string &, const std::string &, const std::string &, int, int &);
		/** Returns true if the value exists and has a true value, false otherwise
		 */
		bool ConfValueBool(ConfigDataHash &, const char *, const char *, int);
		/** Returns true if the value exists and has a true value, false otherwise
		 */
		bool ConfValueBool(ConfigDataHash &, const char *, const char *, const char *, int);
		/** Returns true if the value exists and has a true value, false otherwise
		 */
		bool ConfValueBool(ConfigDataHash &, const std::string &, const std::string &, int);
		/** Returns true if the value exists and has a true value, false otherwise
		 */
		bool ConfValueBool(ConfigDataHash &, const std::string &, const std::string &, const std::string &, int);
		/** Returns the number of occurences of tag in the config file
		 */
		int ConfValueEnum(ConfigDataHash &, const char *);
		/** Returns the number of occurences of tag in the config file
		 */
		int ConfValueEnum(ConfigDataHash &, const std::string &);
		/** Returns the numbers of vars inside the index'th 'tag in the config file
		 */
		int ConfVarEnum(ConfigDataHash &, const char *, int);
		/** Returns the numbers of vars inside the index'th 'tag in the config file
		 */
		int ConfVarEnum(ConfigDataHash &, const std::string &, int);
		void ValidateHostname(const char *, const std::string &, const std::string &);
		void ValidateIP(const char *p, const std::string &, const std::string &, bool);
		void ValidateNoSpaces(const char *, const std::string &, const std::string &);



		/** Below here is a list of variables which contain the config files values
		 */
		/* IRCd module in use */
		char *IRCDModule;

		/* Host to connect to **/
		char *LocalHost;
		/* Port */
		unsigned LocalPort;
		/* List of uplink servers to try and connect to */
		std::list<Uplink *> Uplinks;

		/* Our server name */
		char *ServerName;
		/* Our servers description */
		char *ServerDesc;
		/* The username/ident of services clients */
		char *ServiceUser;
		/* The hostname if services clients */
		char *ServiceHost;

		/* Help channel, ops here get usermode +h **/
		char *HelpChannel;
		/* Log channel */
		char *LogChannel;
		/* Name of the network were on */
		char *NetworkName;
		/* The max legnth of nicks */
		unsigned NickLen;

		/* NickServ Name */
		char *s_NickServ;
		/* ChanServ Name */
		char *s_ChanServ;
		/* MemoServ Name */
		char *s_MemoServ;
		/* BotServ Name */
		char *s_BotServ;
		/* OperServ name */
		char *s_OperServ;
		/* Global name */
		char *s_GlobalNoticer;
		/* NickServs realname */
		char *desc_NickServ;
		/* ChanServ realname */
		char *desc_ChanServ;
		/* MemoServ relname */
		char *desc_MemoServ;
		/* BotServ realname */
		char *desc_BotServ;
		/* OperServ realname */
		char *desc_OperServ;
		/* Global realname */
		char *desc_GlobalNoticer;

		/* Name of the hostserv database */
		char *HostDBName;
		/* HostServ Name */
		char *s_HostServ;
		/* HostServ realname */
		char *desc_HostServ;

		/* Filename for the PID file */
		char *PIDFilename;
		/* MOTD filename */
		char *MOTDFilename;
		/* NickServ DB name */
		char *NickDBName;
		/* DB name for nicks being registered */
		char *PreNickDBName;
		/* Channel DB name */
		char *ChanDBName;
		/* Botserv DB name */
		char *BotDBName;
		/* OperServ db name */
		char *OperDBName;
		/* News DB name */
		char *NewsDBName;

		/* True if its ok to not be able to save backs */
		bool NoBackupOkay;
		/* Do password checking when new people register */
		bool StrictPasswords;
		/* How many times you're allowed to give a bad password before being killed */
		unsigned BadPassLimit;
		/* How long before bad passwords are forgotten */ 
		time_t BadPassTimeout;
		/* Delay between automatic database updates */
		time_t UpdateTimeout;
		/* Delay between checks for expired nicks and channels */
		time_t ExpireTimeout;
		/* How long to wait for something from the uplink, this is passed to select() */
		time_t ReadTimeout;
		/* How often to send program errors */
		time_t WarningTimeout;
		/* How long to process things such as timers to see if there is anything to calll */
		time_t TimeoutCheck;
		/* Num of days logfiles are kept */
		int KeepLogs;
		/* Number of days backups are kept */
		int KeepBackups;
		/* Forbidding requires a reason */
		bool ForceForbidReason;
		/* Services should use privmsgs instead of notices */
		bool UsePrivmsg;
		/* Services only respond to full PRIVMSG client@services.server.name messaegs */
		bool UseStrictPrivMsg;
		/* Dump a core file if we crash */
		bool DumpCore;
		/* Log users connecting/existing/changing nicks */
		bool LogUsers;
		/* Number of seconds between consecutive uses of the REGISTER command
		 * Not to be confused with NSRegDelay */
		unsigned NickRegDelay;
		/* Max number if news items allowed in the list */
		unsigned NewsCount;
		/* Default mlock modes */
		std::string MLock;

		/* Services can use email */
		bool UseMail;
		/* Path to the sendmail executable */
		char *SendMailPath;
		/* Address to send from */
		char *SendFrom;
		/* Only opers can have services send mail */
		bool RestrictMail;
		/* Delay between sending mail */
		time_t MailDelay;
		/* Don't quote the To: address */
		bool DontQuoteAddresses;

		/* Prefix of guest nicks when a user gets forced off of a nick */
		char *NSGuestNickPrefix;
		/* Allow users to set kill immed on */
		bool NSAllowKillImmed;
		/* Don't allow nicks to use /ns group to regroup nicks */ 
		bool NSNoGroupChange;
		/* Default flags for newly registered nicks */
		Flags<NickCoreFlag> NSDefFlags;
		/* Default language used by services */
		unsigned NSDefLanguage;
		/* Users must be connected this long before they can register
		 * Not to be confused with NickRegDelay */
		time_t NSRegDelay;
		/* Time before the registering mail will be resent */
		time_t NSResendDelay;
		/* How long before nicks expir */
		time_t NSExpire;
		/* Time before NickRequests expire */
		time_t NSRExpire;
		/* Force email when registering */
		bool NSForceEmail;
		/* Max number of nicks in a group */
		int NSMaxAliases;
		/* Max number of allowed strings on the access list */
		unsigned NSAccessMax;
		/* Enforcer client user name */
		char *NSEnforcerUser;
		/* Enforcer client hostname */
		char *NSEnforcerHost;
		/* How long before recovered nicks are released */
		time_t NSReleaseTimeout;
		/* /nickserv list is oper only */
		bool NSListOpersOnly;
		/* Max number of entries that can be returned from the list command */
		unsigned NSListMax;
		/* Only allow usermode +a etc on real services admins */
		bool NSSecureAdmins;
		/* Services opers must be /operd on the ircd aswell */
		bool NSStrictPrivileges;
		/* Use email to verify new users registering */
		bool NSEmailReg;
		/* Set the proper channel modes on users when they identify */
		bool NSModeOnID;
		/* Add the users hostnask their access list when they register */
		bool NSAddAccessOnReg;

		/* Default flags for newly registered channels */
		Flags<ChannelInfoFlag> CSDefFlags;
		/* Max number of channels a user can own */
		unsigned CSMaxReg;
		/* Time before a channel expires */
		time_t CSExpire;
		/* Default ban type to use for channels */
		int CSDefBantype;
		/* Max number of entries allowed on channel access lists */
		unsigned CSAccessMax;
		/* Max number of entries allowed on autokick lists */
		unsigned CSAutokickMax;
		/* Default autokick reason */
		char *CSAutokickReason;
		/* Time ChanServ should stay in the channel to hold it to keep users from getting in */
		time_t CSInhabit;
		/* ChanServ's LIST command is oper only */
		bool CSListOpersOnly;
		/* Max number of entries allowed to be returned from the LIST command */
		unsigned CSListMax;
		/* true to make ChanServ oper only */
		bool CSOpersOnly;

		/* Max number of memos allowed */
		unsigned MSMaxMemos;
		/* Time you must wait between sending memos */
		time_t MSSendDelay;
		/* Notify all of the aliases of the core the memo was sent to */ 
		bool MSNotifyAll;
		/* Who can use memos reciepts */ 
		unsigned MSMemoReceipt;

		/* Defai;t BotServ flags */
		Flags<BotServFlag> BSDefFlags;
		/* How long before botserv forgets a user. This is used for flood kickers etc */
		time_t BSKeepData;
		/* Min number of users to have in the channel before the service bot joins */
		unsigned BSMinUsers;
		/* Max number of words allowed on the badwordslist */
		unsigned BSBadWordsMax;
		/* BotServ bot only joins if it would normally allowed to, abides by bans etc */
		bool BSSmartJoin;
		/* Dont tell users what badword they used */
		bool BSGentleBWReason;
		/* Case sensitive badwords matching */
		bool BSCaseSensitive;
		/* Char to use for the fantasy char, eg ! */
		char *BSFantasyCharacter;

		/* Only show /stats o to opers */
		bool HideStatsO;
		/* Send out a global when services shut down or restart */
		bool GlobalOnCycle;
		/* Don't include the opers name in globals */
		bool AnonymousGlobal;
		/* Dont allow users to register nicks with oper names in them */
		bool RestrictOperNicks;
		/* Message to send when shutting down */
		char *GlobalOnCycleMessage;
		/* Message to send when starting up */
		char *GlobalOnCycleUP;
		/* Super admin is allowed */
		bool SuperAdmin;
		/* Log things said through ACT/SAY */
		bool LogBot;
		/* Log when new user max is reached */
		bool LogMaxUsers;
		/* Default expiry time for akills */
		time_t AutokillExpiry;
		/* Default expiry time for chan kills */
		time_t ChankillExpiry;
		/* Default expiry time for SGLine Expire */
		time_t SGLineExpiry;
		/* Default expiry time for SQLines */
		time_t SQLineExpiry;
		/* Default expiry time for SZLine */
		time_t SZLineExpiry;
		/* Actually akill the user when the akill is added */
		bool AkillOnAdd;
		/* Kill users on SGline */
		bool KillonSGline;
		/* Kill users on SQline */
		bool KillonSQline;
		/* Send a WALLOPS/GLOBOPS when a user opers */
		bool WallOper;
		/* Send a WALLOPS/GLOBOPS when a nonoper tries to use OperServ */
		bool WallBadOS;
		/* Send a WALLOPS/GLOBOPS when someone uses the GLOBAL command */
		bool WallOSGlobal;
		/* Send a WALLOPS/GLOBOPS when someone uses the MODE command */
		bool WallOSMode;
		/* Send a WALLOPS/GLOBOPS when someone uses the CLEARMODES command */
		bool WallOSClearmodes;
		/* Send a WALLOPS/GLOBOPS when someone uses the KICK command */
		bool WallOSKick;
		/* Send a WALLOPS/GLOBOPS when someone uses the AKILL command */
		bool WallOSAkill;
		/* Send a WALLOPS/GLOBOPS when someone uses the SGLINE command */
		bool WallOSSGLine;
		/* Send a WALLOPS/GLOBOPS when someone uses the SQLINE command */
		bool WallOSSQLine;
		/* Send a WALLOPS/GLOBOPS when someone uses the SZLINE command */
		bool WallOSSZLine;
		/* Send a WALLOPS/GLOBOPS when someone uses the NOOP command */
		bool WallOSNoOp;
		/* Send a WALLOPS/GLOBOPS when when someone uses the JUPE command */
		bool WallOSJupe;
		/* Send a WALLOPS/GLOBOPS when an akill expires */
		bool WallAkillExpire;
		/* Send a WALLOPS/GLOBOPS when SGLines expire */
		bool WallSGLineExpire;
		/* Send a WALLOPS/GLOBOPS when SQLines expire */
		bool WallSQLineExpire;
		/* Send a WALLOPS/GLOBOPS when SZLines expire */
		bool WallSZLineExpire;
		/* Send a WALLOPS/GLOBOPS when exceptions expire */
		bool WallExceptionExpire;
		/* Send a WALLOPS/GLOBOPS when DROP is used */
		bool WallDrop;
		/* Send a WALLOPS/GLOBOPS when FORBID is used */
		bool WallForbid;
		/* Send a WALLOPS/GLOBOPS when GETPASS is used */
		bool WallGetpass;
		/* Send a WALLOPS/GLOBOPS when SETPASS is used */
		bool WallSetpass;
		/* Add the akillers nick to the akill reason */
		bool AddAkiller;

		/* Limit sessions */
		bool LimitSessions;
		/* The default session limit */
		unsigned DefSessionLimit;
		/* How long before exceptions expire */
		time_t ExceptionExpiry;
		/* How many times to kill before adding an KILL */ 
		int MaxSessionKill;
		/* Max limit that can be used for exceptions */
		unsigned MaxSessionLimit;
		/* How long session akills should last */
		time_t SessionAutoKillExpiry;
		/* DB name for exceptions */
		char *ExceptionDBName;
		/* Reason to use for session kills */ 
		char *SessionLimitExceeded;
		/* Optional second reason */
		char *SessionLimitDetailsLoc;
		/* OperServ requires you to be an operator */
		bool OSOpersOnly;

		/* List of modules to autoload */
		std::list<std::string> ModulesAutoLoad;
		/* Encryption modules */
		std::list<std::string> EncModuleList;
		/* HostServ Core Modules */
		std::list<std::string> HostServCoreModules;
		/* MemoServ Core Modules */
		std::list<std::string> MemoServCoreModules;
		/* BotServ Core Modules */
		std::list<std::string> BotServCoreModules;
		/* OperServ Core Modules */
		std::list<std::string> OperServCoreModules;
		/* NickServ Core Modules */
		std::list<std::string> NickServCoreModules;
		/* ChanServ Core Modules */
		std::list<std::string> ChanServCoreModules;

		/* Default defcon level */
		int DefConLevel;
		/* Timeout before defcon is reset */
		time_t DefConTimeOut;
		/* Session limiit to use when using defcon */
		int DefConSessionLimit;
		/* How long to add akills for defcon */
		time_t DefConAKILL;
		/* Chan modes for defcon */
		char *DefConChanModes;
		/* Should we global on defcon */
		bool GlobalOnDefcon;
		/* Should we send DefconMessage aswell? */
		bool GlobalOnDefconMore;
		/* Message to send when defcon is off */
		char *DefConOffMessage;
		/* Message to send when defcon is on*/
		char *DefconMessage;
		/* Reason to akill clients for defcon */
		char *DefConAkillReason;

		/* User keys to use for generating random hashes for pass codes etc */
		long unsigned int UserKey1;
		long unsigned int UserKey2;
		long unsigned int UserKey3;

		/* Numeric */
		char *Numeric;
		/* Array of ulined servers */
		char **Ulines;
		/* Number of ulines */
		int NumUlines;

		/* List of available opertypes */
		std::list<OperType *> MyOperTypes;
		/* List of pairs of opers and their opertype from the config */
		std::list<std::pair<std::string, std::string> > Opers;
};

/** This class can be used on its own to represent an exception, or derived to represent a module-specific exception.
 * When a module whishes to abort, e.g. within a constructor, it should throw an exception using ModuleException or
 * a class derived from ModuleException. If a module throws an exception during its constructor, the module will not
 * be loaded. If this happens, the error message returned by ModuleException::GetReason will be displayed to the user
 * attempting to load the module, or dumped to the console if the ircd is currently loading for the first time.
 */
class ConfigException : public CoreException
{
	public:
		/** Default constructor, just uses the error mesage 'Config threw an exception'.
		 */
		ConfigException() : CoreException("Config threw an exception", "Config Parser") {}
		/** This constructor can be used to specify an error message before throwing.
		 */
		ConfigException(const std::string &message) : CoreException(message, "Config Parser") {}
		/** This destructor solves world hunger, cancels the world debt, and causes the world to end.
		 * Actually no, it does nothing. Never mind.
		 * @throws Nothing!
		 */
		virtual ~ConfigException() throw() { };
};

#define CONF_NO_ERROR 0x000000
#define CONF_NOT_A_NUMBER 0x000010
#define CONF_INT_NEGATIVE 0x000080
#define CONF_VALUE_NOT_FOUND 0x000100
#define CONF_FILE_NOT_FOUND 0x000200

/** Allows reading of values from configuration files
 * This class allows a module to read from either the main configuration file (inspircd.conf) or from
 * a module-specified configuration file. It may either be instantiated with one parameter or none.
 * Constructing the class using one parameter allows you to specify a path to your own configuration
 * file, otherwise, inspircd.conf is read.
 */
class CoreExport ConfigReader
{
	protected:
		/** The contents of the configuration file
		 * This protected member should never be accessed by a module (and cannot be accessed unless the
		 * core is changed). It will contain a pointer to the configuration file data with unneeded data
		 * (such as comments) stripped from it.
		 */
		ConfigDataHash *data;
		/** Used to store errors
		 */
		std::ostringstream *errorlog;
		/** If we're using our own config data hash or not
		 */
		bool privatehash;
		/** True if an error occured reading the config file
		 */
		bool readerror;
		/** Error code
		 */
		long error;
	public:
		/** Default constructor.
		 * This constructor initialises the ConfigReader class to read services.conf.
		 */
		ConfigReader();
		/** Overloaded constructor.
		 * This constructor initialises the ConfigReader class to read a user-specified config file
		 */
		ConfigReader(const std::string &);
		/** Default destructor.
		 * This method destroys the ConfigReader class.
		 */
		~ConfigReader();
		/** Retrieves a value from the config file.
		 * This method retrieves a value from the config file. Where multiple copies of the tag
		 * exist in the config file, index indicates which of the values to retrieve.
		 */
		std::string ReadValue(const std::string &, const std::string &, int, bool = false);
		/** Retrieves a value from the config file.
		 * This method retrieves a value from the config file. Where multiple copies of the tag
		 * exist in the config file, index indicates which of the values to retrieve. If the
		 * tag is not found the default value is returned instead.
		 */
		std::string ReadValue(const std::string &, const std::string &, const std::string &, int, bool = false);
		/** Retrieves a boolean value from the config file.
		 * This method retrieves a boolean value from the config file. Where multiple copies of the tag
		 * exist in the config file, index indicates which of the values to retrieve. The values "1", "yes"
		 * and "true" in the config file count as true to ReadFlag, and any other value counts as false.
		 */
		bool ReadFlag(const std::string &, const std::string &, int);
		/** Retrieves a boolean value from the config file.
		 * This method retrieves a boolean value from the config file. Where multiple copies of the tag
		 * exist in the config file, index indicates which of the values to retrieve. The values "1", "yes"
		 * and "true" in the config file count as true to ReadFlag, and any other value counts as false.
		 * If the tag is not found, the default value is used instead.
		 */
		bool ReadFlag(const std::string &, const std::string &, const std::string &, int);
		/** Retrieves an integer value from the config file.
		 * This method retrieves an integer value from the config file. Where multiple copies of the tag
		 * exist in the config file, index indicates which of the values to retrieve. Any invalid integer
		 * values in the tag will cause the objects error value to be set, and any call to GetError() will
		 * return CONF_INVALID_NUMBER to be returned. need_positive is set if the number must be non-negative.
		 * If a negative number is placed into a tag which is specified positive, 0 will be returned and GetError()
		 * will return CONF_INT_NEGATIVE. Note that need_positive is not suitable to get an unsigned int - you
		 * should cast the result to achieve that effect.
		 */
		int ReadInteger(const std::string &, const std::string &, int, bool);
		/** Retrieves an integer value from the config file.
		 * This method retrieves an integer value from the config file. Where multiple copies of the tag
		 * exist in the config file, index indicates which of the values to retrieve. Any invalid integer
		 * values in the tag will cause the objects error value to be set, and any call to GetError() will
		 * return CONF_INVALID_NUMBER to be returned. needs_unsigned is set if the number must be unsigned.
		 * If a signed number is placed into a tag which is specified unsigned, 0 will be returned and GetError()
		 * will return CONF_NOT_UNSIGNED. If the tag is not found, the default value is used instead.
		 */
		int ReadInteger(const std::string &, const std::string &, const std::string &, int, bool);
		/** Returns the last error to occur.
		 * Valid errors can be found by looking in modules.h. Any nonzero value indicates an error condition.
		 * A call to GetError() resets the error flag back to 0.
		 */
		long GetError();
		/** Counts the number of times a given tag appears in the config file.
		 * This method counts the number of times a tag appears in a config file, for use where
		 * there are several tags of the same kind, e.g. with opers and connect types. It can be
		 * used with the index value of ConfigReader::ReadValue to loop through all copies of a
		 * multiple instance tag.
		 */
		int Enumerate(const std::string &);
		/** Returns true if a config file is valid.
		 * This method is partially implemented and will only return false if the config
		 * file does not exist or could not be opened.
		 */
		bool Verify();
		/** Dumps the list of errors in a config file to an output location. If bail is true,
		 * then the program will abort. If bail is false and user points to a valid user
		 * record, the error report will be spooled to the given user by means of NOTICE.
		 * if bool is false AND user is false, the error report will be spooled to all opers
		 * by means of a NOTICE to all opers.
		 */
		void DumpErrors(bool);
		/** Returns the number of items within a tag.
		 * For example if the tag was &lt;test tag="blah" data="foo"&gt; then this
		 * function would return 2. Spaces and newlines both qualify as valid seperators
		 * between values.
		 */
		int EnumerateValues(const std::string &, int);
};

#endif
