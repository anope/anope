/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "account.h"
#include "regchannel.h"
#include "users.h"

/** A configuration key and value pair
 */
typedef std::pair<Anope::string, Anope::string> KeyVal;

/** A list of related configuration keys and values
 */
typedef std::vector<KeyVal> KeyValList;

/** An entire config file, built up of KeyValLists
 */
typedef std::multimap<Anope::string, KeyValList> ConfigDataHash;

// Required forward definitions

/** Types of data in the core config
 */
enum ConfigDataType
{
	DT_NOTHING, // No data
	DT_INTEGER, // Integer
	DT_UINTEGER, // Unsigned Integer
	DT_LUINTEGER, // Long Unsigned Integer
	DT_STRING, // Anope::string
	DT_BOOLEAN, // Boolean
	DT_HOSTNAME, // Hostname syntax
	DT_NOSPACES, // No spaces
	DT_IPADDRESS, // IP address (v4, v6)
	DT_TIME, // Time value
	DT_NORELOAD = 32, // Item can't be reloaded after startup
	DT_ALLOW_WILD = 64, // Allow wildcards/CIDR in DT_IPADDRESS
	DT_ALLOW_NEWLINE = 128, // New line characters allowed in DT_STRING
	DT_ALLOW_EMPTY = 256 // Allow empty value
};

/** Holds a config value, either string, integer or boolean.
 * Callback functions receive one or more of these, either on
 * their own as a reference, or in a reference to a deque of them.
 * The callback function can then alter the values of the ValueItem
 * classes to validate the settings.
 */
class CoreExport ValueItem
{
 private:
	/** Actual data */
	Anope::string v;
 public:
 	/** Initialize with nothing */
 	ValueItem();
	/** Initialize with an int */
	ValueItem(int);
	/** Initialize with a bool */
	ValueItem(bool);
	/** Initialize with an Anope::string */
	ValueItem(const Anope::string &);
	/** Initialize with a long */
	ValueItem(long);
	/** Change value to an Anope::string */
	void Set(const Anope::string &);
	/** Change value to an int */
	void Set(int);
	/** Get value as an int */
	int GetInteger() const;
	/** Get value as a string */
	const char *GetString() const;
	/** Get value as an Anope::string */
	inline const Anope::string &GetValue() const { return v; }
	/** Get value as a bool */
	bool GetBool() const;
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
 private:
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

/** This a specific version of ValueContainer to handle Anope::string specially
 */
template<> class ValueContainer<Anope::string *> : public ValueContainerBase
{
 private:
	/** Contained item */
	Anope::string *val;
 public:
	/** Initialize with nothing */
	ValueContainer() : ValueContainerBase(), val(NULL) { }
	/** Initialize with an std::string */
	ValueContainer(Anope::string *Val) : ValueContainerBase(), val(Val) { }
	/** Initialize with a copy */
	ValueContainer(const ValueContainer &Val) : ValueContainerBase(), val(Val.val) { }
	ValueContainer &operator=(const ValueContainer &Val)
	{
		val = Val.val;
		return *this;
	}

	/** Change value to given Anope::string */
	void Set(const Anope::string &newval)
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
 * an int
 */
typedef ValueContainer<int *> ValueContainerInt;

/** A specialization of ValueContainer to hold a pointer to
 * a time_t
 */
typedef ValueContainer<time_t *> ValueContainerTime;

/** A specialization of ValueContainer to hold a pointer to
 * an Anope::string
 */
typedef ValueContainer<Anope::string *> ValueContainerString;

/** A set of ValueItems used by multi-value validator functions
 */
typedef std::deque<ValueItem> ValueList;

/** A callback for validating a single value
 */
typedef bool (*Validator)(ServerConfig *, const Anope::string &, const Anope::string &, ValueItem &);
/** A callback for validating multiple value entries
 */
typedef bool (*MultiValidator)(ServerConfig *, const Anope::string &, const Anope::string *, ValueList &, int *);
/** A callback indicating the end of a group of entries
 */
typedef bool (*MultiNotify)(ServerConfig *, const Anope::string &);

bool ValidateNotEmpty(ServerConfig *, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidateNotZero(ServerConfig *, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidateEmailReg(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidatePort(ServerConfig *, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidateGuestPrefix(ServerConfig *conf, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidateBantype(ServerConfig *, const Anope::string &, const Anope::string &, ValueItem &data);
bool ValidateChanServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidateMemoServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidateBotServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidateHostServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidateLimitSessions(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidateOperServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidateGlobal(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidateNickLen(ServerConfig *, const Anope::string &, const Anope::string &, ValueItem &data);
bool ValidateMail(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data);
bool ValidateGlobalOnCycle(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data);

/** Represents a configuration file
 */
class ConfigurationFile
{
	Anope::string name;
	bool executable;
	FILE *fp;
 public:
	ConfigurationFile(const Anope::string &, bool);
	~ConfigurationFile();
	const Anope::string &GetName() const;

	bool IsOpen() const;
	bool Open();
	void Close();
	bool End() const;
	Anope::string Read();
};

/** Holds all of the core configuration items
 */
class CoreExport ConfigItems
{
 public:
	/** Holds a core configuration item and its callbacks
	 */
	struct Item
	{
		/** Tag name */
		Anope::string tag;
		/** Value name */
		Anope::string value;
		/** Default, if not defined */
		Anope::string default_value;
		/** Value containers */
		ValueContainerBase *val;
		/** Data types */
		int datatype;
		/** Validation function */
		Validator validation_function;
	} *Values;

	/** Holds a core configuration item and its callbacks
	 * where there may be more than one item
	 */
	struct MultiItem
	{
		/** Tag name */
		Anope::string tag;
		/** One or more items within tag */
		Anope::string items[17];
		/** One or more defaults for items within tags */
		Anope::string items_default[17];
		/** One or more data types */
		int datatype[17];
		/** Initialization function */
		MultiNotify init_function;
		/** Validation function */
		MultiValidator validation_function;
		/** Completion function */
		MultiNotify finish_function;
	} *MultiValues;

	ConfigItems(ServerConfig *conf);
	~ConfigItems();
};

/** This class holds the bulk of the runtime configuration for Anope.
 * It allows for reading new config values, accessing configuration files,
 * and storage of the configuration data needed to run Anope.
 */
class CoreExport ServerConfig
{
 private:
	/** Check that there is only one of each configuration item
	 */
	bool CheckOnce(const Anope::string &);
 public:
	/** This holds all the information in the config file,
	 * it's indexed by tag name to a vector of key/values.
	 */
	ConfigDataHash config_data;
	/** Construct a new ServerConfig
	 */
	ServerConfig();

	/** Read the entire configuration into memory
	 * and initialize this class. All other methods
	 * should be used only by the core.
	 */
	void Read();
	/** Load the configuration file into 'this'. With the new config parser everything is parsed into
	 * tag/key/value at load-time rather than at read-value time.
	 */
	void LoadConf(ConfigurationFile &file);
	// Both these return true if the value existed or false otherwise
	/** Writes 'length' chars into 'result' as a string
	 */
	bool ConfValue(ConfigDataHash &, const Anope::string &, const Anope::string &, int, Anope::string &, bool = false);
	/** Writes 'length' chars into 'result' as a string
	 */
	bool ConfValue(ConfigDataHash &, const Anope::string &, const Anope::string &, const Anope::string &, int, Anope::string &, bool = false);
	/** Tries to convert the value to an integer and write it to 'result'
	 */
	bool ConfValueInteger(ConfigDataHash &, const Anope::string &, const Anope::string &, int, int &);
	/** Tries to convert the value to an integer and write it to 'result'
	 */
	bool ConfValueInteger(ConfigDataHash &, const Anope::string &, const Anope::string &, const Anope::string &, int, int &);
	/** Returns true if the value exists and has a true value, false otherwise
	 */
	bool ConfValueBool(ConfigDataHash &, const Anope::string &, const Anope::string &, int);
	/** Returns true if the value exists and has a true value, false otherwise
	 */
	bool ConfValueBool(ConfigDataHash &, const Anope::string &, const Anope::string &, const Anope::string &, int);
	/** Returns the number of occurences of tag in the config file
	 */
	int ConfValueEnum(const ConfigDataHash &, const Anope::string &);
	/** Returns the numbers of vars inside the index'th 'tag in the config file
	 */
	int ConfVarEnum(ConfigDataHash &, const Anope::string &, int);
	void ValidateHostname(const Anope::string &, const Anope::string &, const Anope::string &) const;
	void ValidateIP(const Anope::string &p, const Anope::string &, const Anope::string &, bool) const;
	void ValidateNoSpaces(const Anope::string &, const Anope::string &, const Anope::string &) const;

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

	/** Below here is a list of variables which contain the config files values
	 */
	/* Host to bind to */
	Anope::string LocalHost;
	/* List of uplink servers to try and connect to */
	std::vector<Uplink *> Uplinks;

	/* Our server name */
	Anope::string ServerName;
	/* Our servers description */
	Anope::string ServerDesc;

	/* Name of the network were on */
	Anope::string NetworkName;
	/* The max legnth of nicks */
	unsigned NickLen;
	/* Max length of idents */
	unsigned UserLen;
	/* Max lenght of hostnames */
	unsigned HostLen;

	/* Max length of passwords */
	unsigned PassLen;

	/* Filename for the PID file */
	Anope::string PIDFilename;
	/* MOTD filename */
	Anope::string MOTDFilename;

	Anope::string BotServ;
	Anope::string ChanServ;
	Anope::string Global;
	Anope::string HostServ;
	Anope::string NickServ;
	Anope::string OperServ;
	Anope::string MemoServ;

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
	/* Number of days backups are kept */
	int KeepBackups;
	/* Forbidding requires a reason */
	bool ForceForbidReason;
	/* Services should use privmsgs instead of notices */
	bool UsePrivmsg;
	/* Services only respond to full PRIVMSG client@services.server.name messages */
	bool UseStrictPrivMsg;
	/* This is not a configurable option.
	 * Config::Config will set it depending on the value of UseStrictPrivMsg */
	Anope::string UseStrictPrivMsgString;
	/* Number of seconds between consecutive uses of the REGISTER command
	 * Not to be confused with NSRegDelay */
	unsigned NickRegDelay;
	/* Max number if news items allowed in the list */
	unsigned NewsCount;
	/* Default mlock modes */
	Anope::string MLock;
	/* Unmlockable modes */
	Anope::string NoMLock;
	/* Modes that are required to be on registered channels */
	Anope::string CSRequire;
	/* Default botmodes on channels, defaults to ao */
	Anope::string BotModes;
	/* THe actual modes */
	ChannelStatus BotModeList;
	/* How long to wait between connection attempts */
	int RetryWait;
	/* If services should hide unprivileged commands */
	bool HidePrivilegedCommands;
	/* If set, nicks cant be owned/everything is entirely account based */
	bool NoNicknameOwnership;
	/* Regex engine to use */
	Anope::string RegexEngine;

	/* A vector of our logfile options */
	std::vector<LogInfo *> LogInfos;

	/* Services can use email */
	bool UseMail;
	/* Path to the sendmail executable */
	Anope::string SendMailPath;
	/* Address to send from */
	Anope::string SendFrom;
	/* Only opers can have services send mail */
	bool RestrictMail;
	/* Delay between sending mail */
	time_t MailDelay;
	/* Don't quote the To: address */
	bool DontQuoteAddresses;
	/* Mail messages to send */
	Anope::string MailRegistrationSubject, MailRegistrationMessage;
	Anope::string MailResetSubject, MailResetMessage;
	Anope::string MailSendpassSubject, MailSendpassMessage;
	Anope::string MailEmailchangeSubject, MailEmailchangeMessage;
	Anope::string MailMemoSubject, MailMemoMessage;

	/* Nameserver to use for resolving hostnames */
	Anope::string NameServer;
	/* TIme before a DNS query is considered dead */
	time_t DNSTimeout;

	/* Prefix of guest nicks when a user gets forced off of a nick */
	Anope::string NSGuestNickPrefix;
	/* Allow users to set kill immed on */
	bool NSAllowKillImmed;
	/* Don't allow nicks to use /ns group to regroup nicks */
	bool NSNoGroupChange;
	/* Default flags for newly registered nicks */
	Flags<NickCoreFlag, NI_END> NSDefFlags;
	/* All languages Anope is aware about */
	Anope::string Languages;
	/* Default language used by services */
	Anope::string NSDefLanguage;
	/* Users must be connected this long before they can register
	 * Not to be confused with NickRegDelay */
	time_t NSRegDelay;
	/* Time before the registering mail will be resent */
	time_t NSResendDelay;
	/* How long before nicks expire */
	time_t NSExpire;
	/* How long before suspended nicks expire */
	time_t NSSuspendExpire;
	/* Time before unconfirmed nicks expire */
	time_t NSUnconfirmedExpire;
	/* Force email when registering */
	bool NSForceEmail;
	/* Force users to validate new email addresses */
	bool NSConfirmEmailChanges;
	/* Max number of nicks in a group */
	unsigned NSMaxAliases;
	/* Max number of allowed strings on the access list */
	unsigned NSAccessMax;
	/* Enforcer client user name */
	Anope::string NSEnforcerUser;
	/* Enforcer client hostname */
	Anope::string NSEnforcerHost;
	/* How long before recovered nicks are released */
	time_t NSReleaseTimeout;
	/* Max number of entries that can be returned from the list command */
	unsigned NSListMax;
	/* Only allow usermode +a etc on real services admins */
	bool NSSecureAdmins;
	/* Services opers must be /operd on the ircd aswell */
	bool NSStrictPrivileges;
	/* Type of confirmation to use, or to disable registration completely */
	Anope::string NSRegistration;
	/* A message sent to unregistered users on connect */
	Anope::string NSUnregisteredNotice;
	/* Core NickServ modules */
	Anope::string NickCoreModules;
	/* Set the proper channel modes on users when they identify */
	bool NSModeOnID;
	/* Add the users hostnask their access list when they register */
	bool NSAddAccessOnReg;
	/* Maximum number of channels on AJoin */
	unsigned AJoinMax;

	/* Core ChanServ modules */
	Anope::string ChanCoreModules;
	/* Default flags for newly registered channels */
	Flags<ChannelInfoFlag, CI_END> CSDefFlags;
	/* Max number of channels a user can own */
	unsigned CSMaxReg;
	/* Time before a channel expires */
	time_t CSExpire;
	/* How long before suspended channels expire */
	time_t CSSuspendExpire;
	/* How long before forbidden channels expire */
	time_t CSForbidExpire;
	/* Default ban type to use for channels */
	int CSDefBantype;
	/* Max number of entries allowed on channel access lists */
	unsigned CSAccessMax;
	/* Max number of entries allowed on autokick lists */
	unsigned CSAutokickMax;
	/* Default autokick reason */
	Anope::string CSAutokickReason;
	/* Time ChanServ should stay in the channel to hold it to keep users from getting in */
	time_t CSInhabit;
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

	/* Valid chars allowed in vhosts */
	Anope::string VhostChars;
	/* Allow undotted vhosts? */
	bool VhostUndotted;
	/* Chars disallowed at the beginning or end of vhosts */
	Anope::string VhostDisallowBE;

	/* Core BotServ modules */
	Anope::string BotCoreModules;
	/* Default BotServ flags */
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
	Anope::string BSFantasyCharacter;

	/* Only show /stats o to opers */
	bool HideStatsO;
	/* Send out a global when services shut down or restart */
	bool GlobalOnCycle;
	/* Don't include the opers name in globals */
	bool AnonymousGlobal;
	/* Dont allow users to register nicks with oper names in them */
	bool RestrictOperNicks;
	/* Message to send when shutting down */
	Anope::string GlobalOnCycleMessage;
	/* Message to send when starting up */
	Anope::string GlobalOnCycleUP;
	/* Super admin is allowed */
	bool SuperAdmin;
	/* Default expiry time for akills */
	time_t AutokillExpiry;
	/* Default expiry time for chan kills */
	time_t ChankillExpiry;
	/* Default expiry time for SNLine Expire */
	time_t SNLineExpiry;
	/* Default expiry time for SQLines */
	time_t SQLineExpiry;
	/* Actually akill the user when the akill is added */
	bool AkillOnAdd;
	/* Kill users on SNLine */
	bool KillonSNline;
	/* Kill users on SQline */
	bool KillonSQline;
	/* Add the akillers nick to the akill reason */
	bool AddAkiller;
	/* Add akill ids to akill reason */
	bool AkillIds;

	/* Limit sessions */
	bool LimitSessions;
	/* The default session limit */
	unsigned DefSessionLimit;
	/* How long before exceptions expire */
	time_t ExceptionExpiry;
	/* How many times to kill before adding an KILL */
	unsigned MaxSessionKill;
	/* Max limit that can be used for exceptions */
	unsigned MaxSessionLimit;
	/* How long session akills should last */
	time_t SessionAutoKillExpiry;
	/* Reason to use for session kills */
	Anope::string SessionLimitExceeded;
	/* Optional second reason */
	Anope::string SessionLimitDetailsLoc;
	/* OperServ requires you to be an operator */
	bool OSOpersOnly;

	/* List of modules to autoload */
	std::list<Anope::string> ModulesAutoLoad;

	/* Seed to use for RNG */
	unsigned long Seed;

	/* Numeric */
	Anope::string Numeric;
	/* Array of ulined servers */
	std::list<Anope::string> Ulines;

	/* List of available opertypes */
	std::list<OperType *> MyOperTypes;
	/* List of pairs of opers and their opertype from the config */
	std::vector<Oper *> Opers;
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
	ConfigException() : CoreException("Config threw an exception", "Config Parser") { }
	/** This constructor can be used to specify an error message before throwing.
	 */
	ConfigException(const Anope::string &message) : CoreException(message, "Config Parser") { }
	/** This destructor solves world hunger, cancels the world debt, and causes the world to end.
	 * Actually no, it does nothing. Never mind.
	 * @throws Nothing!
	 */
	virtual ~ConfigException() throw() { }
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
	/** True if an error occured reading the config file
	 */
	bool readerror;
	/** Error code
	 */
	long error;
 public:
	/** Default constructor.
	 * This constructor initialises the ConfigReader class to read the configuration file(s).
	 */
	ConfigReader();
	/** Overloaded constructor.
	 * This constructor initialises the ConfigReader class to read a user-specified config file
	 */
	ConfigReader(const Anope::string &);
	/** Default destructor.
	 * This method destroys the ConfigReader class.
	 */
	~ConfigReader();
	/** Retrieves a value from the config file.
	 * This method retrieves a value from the config file. Where multiple copies of the tag
	 * exist in the config file, index indicates which of the values to retrieve.
	 */
	Anope::string ReadValue(const Anope::string &, const Anope::string &, int, bool = false);
	/** Retrieves a value from the config file.
	 * This method retrieves a value from the config file. Where multiple copies of the tag
	 * exist in the config file, index indicates which of the values to retrieve. If the
	 * tag is not found the default value is returned instead.
	 */
	Anope::string ReadValue(const Anope::string &, const Anope::string &, const Anope::string &, int, bool = false);
	/** Retrieves a boolean value from the config file.
	 * This method retrieves a boolean value from the config file. Where multiple copies of the tag
	 * exist in the config file, index indicates which of the values to retrieve. The values "1", "yes"
	 * and "true" in the config file count as true to ReadFlag, and any other value counts as false.
	 */
	bool ReadFlag(const Anope::string &, const Anope::string &, int);
	/** Retrieves a boolean value from the config file.
	 * This method retrieves a boolean value from the config file. Where multiple copies of the tag
	 * exist in the config file, index indicates which of the values to retrieve. The values "1", "yes"
	 * and "true" in the config file count as true to ReadFlag, and any other value counts as false.
	 * If the tag is not found, the default value is used instead.
	 */
	bool ReadFlag(const Anope::string &, const Anope::string &, const Anope::string &, int);
	/** Retrieves an integer value from the config file.
	 * This method retrieves an integer value from the config file. Where multiple copies of the tag
	 * exist in the config file, index indicates which of the values to retrieve. Any invalid integer
	 * values in the tag will cause the objects error value to be set, and any call to GetError() will
	 * return CONF_INVALID_NUMBER to be returned. need_positive is set if the number must be non-negative.
	 * If a negative number is placed into a tag which is specified positive, 0 will be returned and GetError()
	 * will return CONF_INT_NEGATIVE. Note that need_positive is not suitable to get an unsigned int - you
	 * should cast the result to achieve that effect.
	 */
	int ReadInteger(const Anope::string &, const Anope::string &, int, bool);
	/** Retrieves an integer value from the config file.
	 * This method retrieves an integer value from the config file. Where multiple copies of the tag
	 * exist in the config file, index indicates which of the values to retrieve. Any invalid integer
	 * values in the tag will cause the objects error value to be set, and any call to GetError() will
	 * return CONF_INVALID_NUMBER to be returned. needs_unsigned is set if the number must be unsigned.
	 * If a signed number is placed into a tag which is specified unsigned, 0 will be returned and GetError()
	 * will return CONF_NOT_UNSIGNED. If the tag is not found, the default value is used instead.
	 */
	int ReadInteger(const Anope::string &, const Anope::string &, const Anope::string &, int, bool);
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
	int Enumerate(const Anope::string &) const;
	/** Returns true if a config file is valid.
	 * This method is partially implemented and will only return false if the config
	 * file does not exist or could not be opened.
	 */
	bool Verify();
	/** Returns the number of items within a tag.
	 * For example if the tag was &lt;test tag="blah" data="foo"&gt; then this
	 * function would return 2. Spaces and newlines both qualify as valid seperators
	 * between values.
	 */
	int EnumerateValues(const Anope::string &, int);
};

extern ConfigurationFile services_conf;
extern ServerConfig *Config;

#endif // CONFIG_H
