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
		char *GetString();
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
typedef bool (*MultiValidator)(ServerConfig *, const char *, const char **, ValueList &, int *);
/** A callback indicating the end of a group of entries
 */
typedef bool (*MultiNotify)(ServerConfig *, const char *);

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
		/** Process an include directive
		 */
		bool DoInclude(ConfigDataHash &, const std::string &, std::ostringstream &);
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
};

/** Initialize the disabled commands list
 */
E bool InitializeDisabledCommands(const char *);

/** This class can be used on its own to represent an exception, or derived to represent a module-specific exception.
 * When a module whishes to abort, e.g. within a constructor, it should throw an exception using ModuleException or
 * a class derived from ModuleException. If a module throws an exception during its constructor, the module will not
 * be loaded. If this happens, the error message returned by ModuleException::GetReason will be displayed to the user
 * attempting to load the module, or dumped to the console if the ircd is currently loading for the first time.
 */
class ConfigException : public std::exception
{
	protected:
		/** Holds the error message to be displayed
		 */
		const std::string err;
	public:
		/** Default constructor, just uses the error mesage 'Config threw an exception'.
		 */
		ConfigException() : err("Config threw an exception") { }
		/** This constructor can be used to specify an error message before throwing.
		 */
		ConfigException(const std::string &message) : err(message) {}
		/** This destructor solves world hunger, cancels the world debt, and causes the world to end.
		 * Actually no, it does nothing. Never mind.
		 * @throws Nothing!
		 */
		virtual ~ConfigException() throw() { };
		/** Returns the reason for the exception.
		 * The module should probably put something informative here as the user will see this upon failure.
		 */
		virtual const char *GetReason()
		{
			return err.c_str();
		}
};

#endif
