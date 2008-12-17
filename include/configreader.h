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
