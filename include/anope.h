/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

#include <signal.h>

#include "hashcomp.h"

namespace Anope
{
	/**
	 * A wrapper string class around all the other string classes, this class will
	 * allow us to only require one type of string everywhere that can be converted
	 * at any time to a specific type of string.
	 */
	class CoreExport string final
	{
	private:
		/**
		 * The actual string is stored in an std::string as it can be converted to
		 * ci::string, or a C-style string at any time.
		 */
		std::string _string;
	public:
		/**
		 * Extras.
		 */
		typedef std::string::iterator iterator;
		typedef std::string::const_iterator const_iterator;
		typedef std::string::reverse_iterator reverse_iterator;
		typedef std::string::const_reverse_iterator const_reverse_iterator;
		typedef std::string::size_type size_type;
		typedef std::string::value_type value_type;
		static const size_type npos = static_cast<size_type>(-1);

		/**
		 * Constructors that can take in any type of string.
		 */
		string() : _string("") { }
		string(char chr) : _string() { _string = chr; }
		string(size_type n, char chr) : _string(n, chr) { }
		string(const char *_str) : _string(_str) { }
		string(const char *_str, size_type n) : _string(_str, n) { }
		string(const std::string &_str) : _string(_str) { }
		string(const ci::string &_str) : _string(_str.c_str()) { }
		string(const string &_str, size_type pos, size_type n = npos) : _string(_str._string, pos, n) { }
		template <class InputIterator> string(InputIterator first, InputIterator last) : _string(first, last) { }
		string(const string &) = default;

		/**
		 * Assignment operators, so any type of string can be assigned to this class.
		 */
		inline string &operator=(char chr) { this->_string = chr; return *this; }
		inline string &operator=(const char *_str) { this->_string = _str; return *this; }
		inline string &operator=(const std::string &_str) { this->_string = _str; return *this; }
		inline string &operator=(const string &_str) { if (this != &_str) this->_string = _str._string; return *this; }

		/**
		 * Equality operators, to compare to any type of string.
		 */
		inline bool operator==(const char *_str) const { return this->_string == _str; }
		inline bool operator==(const std::string &_str) const { return this->_string == _str; }
		inline bool operator==(const string &_str) const { return this->_string == _str._string; }

		inline bool equals_cs(const char *_str) const { return this->_string == _str; }
		inline bool equals_cs(const std::string &_str) const { return this->_string == _str; }
		inline bool equals_cs(const string &_str) const { return this->_string == _str._string; }

		inline bool equals_ci(const char *_str) const { return ci::string(this->_string.c_str()) == _str; }
		inline bool equals_ci(const std::string &_str) const { return ci::string(this->_string.c_str()) == _str.c_str(); }
		inline bool equals_ci(const string &_str) const { return ci::string(this->_string.c_str()) == _str._string.c_str(); }

		/**
		 * Inequality operators, exact opposites of the above.
		 */
		inline bool operator!=(const char *_str) const { return !operator==(_str); }
		inline bool operator!=(const std::string &_str) const { return !operator==(_str); }
		inline bool operator!=(const string &_str) const { return !operator==(_str); }

		/**
		 * Compound addition operators, overloaded to do concatenation.
		 */
		inline string &operator+=(char chr) { this->_string += chr; return *this; }
		inline string &operator+=(const char *_str) { this->_string += _str; return *this; }
		inline string &operator+=(const std::string &_str) { this->_string += _str; return *this; }
		inline string &operator+=(const string &_str) { if (this != &_str) this->_string += _str._string; return *this; }

		/**
		 * Addition operators, overloaded to do concatenation.
		 */
		inline const string operator+(char chr) const { return string(*this) += chr; }
		inline const string operator+(const char *_str) const { return string(*this) += _str; }
		inline const string operator+(const std::string &_str) const { return string(*this) += _str; }
		inline const string operator+(const string &_str) const { return string(*this) += _str; }

		friend const string operator+(char chr, const string &str);
		friend const string operator+(const char *_str, const string &str);
		friend const string operator+(const std::string &_str, const string &str);

		/**
		 * Less-than operator.
		 */
		inline bool operator<(const string &_str) const { return this->_string < _str._string; }

		/**
		 * The following functions return the various types of strings.
		 */
		inline const char *c_str() const { return this->_string.c_str(); }
		inline const char *data() const { return this->_string.data(); }
		inline std::string &str() { return this->_string; }
		inline const std::string &str() const { return this->_string; }
		inline ci::string ci_str() const { return ci::string(this->_string.c_str()); }

		/**
		 * Returns if the string is empty or not.
		 */
		inline bool empty() const { return this->_string.empty(); }

		/**
		 * Returns the string's length.
		 */
		inline size_type length() const { return this->_string.length(); }

		/**
		 * Returns the size of the currently allocated storage space in the string object.
		 * This can be equal or greater than the length of the string.
		 */
		inline size_type capacity() const { return this->_string.capacity(); }

		/**
		 * Add a char to the end of the string.
		 */
		inline void push_back(char c) { return this->_string.push_back(c); }

		inline string &append(const string &s) { this->_string.append(s.str()); return *this; }
		inline string &append(const char *s, size_t n) { this->_string.append(s, n); return *this; }

		/**
		 * Resizes the string content to n characters.
		 */
		inline void resize(size_type n) { return this->_string.resize(n); }

		/**
		 * Erases characters from the string.
		 */
		inline iterator erase(const iterator &i) { return this->_string.erase(i); }
		inline iterator erase(const iterator &first, const iterator &last) { return this->_string.erase(first, last); }
		inline void erase(size_type pos = 0, size_type n = std::string::npos) { this->_string.erase(pos, n); }

		/**
		 * Trim leading and trailing white spaces from the string.
		 */

		inline string &ltrim(const Anope::string &what = " \t\r\n")
		{
			while (!this->_string.empty() && what.find(this->_string[0]) != Anope::string::npos)
				this->_string.erase(this->_string.begin());
			return *this;
		}

		inline string &rtrim(const Anope::string &what = " \t\r\n")
		{
			while (!this->_string.empty() && what.find(this->_string[this->_string.length() - 1]) != Anope::string::npos)
				this->_string.erase(this->_string.length() - 1);
			return *this;
		}

		inline string &trim(const Anope::string &what = " \t\r\n")
		{
			this->ltrim(what);
			this->rtrim(what);
			return *this;
		}

		/**
		 * Clears the string.
		 */
		inline void clear() { this->_string.clear(); }

		/**
		 * Find substrings of the string.
		 */
		inline size_type find(const string &_str, size_type pos = 0) const { return this->_string.find(_str._string, pos); }
		inline size_type find(char chr, size_type pos = 0) const { return this->_string.find(chr, pos); }
		inline size_type find_ci(const string &_str, size_type pos = 0) const { return ci::string(this->_string.c_str()).find(ci::string(_str._string.c_str()), pos); }
		inline size_type find_ci(char chr, size_type pos = 0) const { return ci::string(this->_string.c_str()).find(chr, pos); }

		inline size_type rfind(const string &_str, size_type pos = npos) const { return this->_string.rfind(_str._string, pos); }
		inline size_type rfind(char chr, size_type pos = npos) const { return this->_string.rfind(chr, pos); }
		inline size_type rfind_ci(const string &_str, size_type pos = npos) const { return ci::string(this->_string.c_str()).rfind(ci::string(_str._string.c_str()), pos); }
		inline size_type rfind_ci(char chr, size_type pos = npos) const { return ci::string(this->_string.c_str()).rfind(chr, pos); }

		inline size_type find_first_of(const string &_str, size_type pos = 0) const { return this->_string.find_first_of(_str._string, pos); }
		inline size_type find_first_of_ci(const string &_str, size_type pos = 0) const { return ci::string(this->_string.c_str()).find_first_of(ci::string(_str._string.c_str()), pos); }

		inline size_type find_first_not_of(const string &_str, size_type pos = 0) const { return this->_string.find_first_not_of(_str._string, pos); }
		inline size_type find_first_not_of_ci(const string &_str, size_type pos = 0) const { return ci::string(this->_string.c_str()).find_first_not_of(ci::string(_str._string.c_str()), pos); }

		inline size_type find_last_of(const string &_str, size_type pos = npos) const { return this->_string.find_last_of(_str._string, pos); }
		inline size_type find_last_of_ci(const string &_str, size_type pos = npos) const { return ci::string(this->_string.c_str()).find_last_of(ci::string(_str._string.c_str()), pos); }

		inline size_type find_last_not_of(const string &_str, size_type pos = npos) const { return this->_string.find_last_not_of(_str._string, pos); }
		inline size_type find_last_not_of_ci(const string &_str, size_type pos = npos) const { return ci::string(this->_string.c_str()).find_last_not_of(ci::string(_str._string.c_str()), pos); }

		inline int compare(size_t pos, size_t len, const string &str) const { return ci::string(this->_string.c_str()).compare(pos, len, ci::string(str.c_str())); }
		inline int compare(size_t pos, size_t len, const string &str, size_t subpos, size_type sublen = npos) const { return ci::string(this->_string.c_str()).compare(pos, len, ci::string(str.c_str()), subpos, sublen); }
		inline int compare(size_t pos, size_t len, const char *s, size_type n = npos) const { return ci::string(this->_string.c_str()).compare(pos, len, s, n); }

		/**
		 * Determine if string consists of only numbers.
		 */
		inline bool is_number_only() const { return this->find_first_not_of("0123456789.-") == npos; }
		inline bool is_pos_number_only() const { return this->find_first_not_of("0123456789.") == npos; }

		/**
		 * In IRC messages we use a substitute (ASCII 0x1A) instead of a space
		 * (ASCII 0x20) so it doesn't get line wrapped when put into a message.
		 * The line wrapper will convert this to a space before it is sent to
		 * clients.
		 */
		inline Anope::string nobreak() const { return this->replace_all_cs("\x20", "\x1A"); }

		/**
		 * Replace parts of the string.
		 */
		inline string replace(size_type pos, size_type n, const string &_str) { return string(this->_string.replace(pos, n, _str._string)); }
		inline string replace(size_type pos, size_type n, const string &_str, size_type pos1, size_type n1) { return string(this->_string.replace(pos, n, _str._string, pos1, n1)); }
		inline string replace(size_type pos, size_type n, size_type n1, char chr) { return string(this->_string.replace(pos, n, n1, chr)); }
		inline string replace(iterator first, iterator last, const string &_str) { return string(this->_string.replace(first, last, _str._string)); }
		inline string replace(iterator first, iterator last, size_type n, char chr) { return string(this->_string.replace(first, last, n, chr)); }
		template <class InputIterator> inline string replace(iterator first, iterator last, InputIterator f, InputIterator l) { return string(this->_string.replace(first, last, f, l)); }
		inline string replace_all_cs(const string &_orig, const string &_repl) const
		{
			Anope::string new_string = *this;
			size_type pos = new_string.find(_orig), orig_length = _orig.length(), repl_length = _repl.length();
			while (pos != npos)
			{
				new_string = new_string.substr(0, pos) + _repl + new_string.substr(pos + orig_length);
				pos = new_string.find(_orig, pos + repl_length);
			}
			return new_string;
		}
		inline string replace_all_ci(const string &_orig, const string &_repl) const
		{
			Anope::string new_string = *this;
			size_type pos = new_string.find_ci(_orig), orig_length = _orig.length(), repl_length = _repl.length();
			while (pos != npos)
			{
				new_string = new_string.substr(0, pos) + _repl + new_string.substr(pos + orig_length);
				pos = new_string.find_ci(_orig, pos + repl_length);
			}
			return new_string;
		}

		/**
		 * Get the string in lowercase.
		 */
		inline string lower() const
		{
			Anope::string new_string = *this;
			for (auto &chr : new_string)
				chr = Anope::tolower(chr);
			return new_string;
		}

		/**
		 * Get the string in uppercase.
		 */
		inline string upper() const
		{
			Anope::string new_string = *this;
			for (auto &chr : new_string)
				chr = Anope::toupper(chr);
			return new_string;
		}

		/**
		 * Get a substring of the string.
		 */
		inline string substr(size_type pos = 0, size_type n = npos) const { return string(this->_string.substr(pos, n)); }

		/**
		 * Iterators to the string.
		 */
		inline iterator begin() { return this->_string.begin(); }
		inline const_iterator begin() const { return this->_string.begin(); }
		inline iterator end() { return this->_string.end(); }
		inline const_iterator end() const { return this->_string.end(); }
		inline reverse_iterator rbegin() { return this->_string.rbegin(); }
		inline const_reverse_iterator rbegin() const { return this->_string.rbegin(); }
		inline reverse_iterator rend() { return this->_string.rend(); }
		inline const_reverse_iterator rend() const { return this->_string.rend(); }

		/**
		 * Subscript operator, to access individual characters of the string.
		 */
		inline char &operator[](size_type n) { return this->_string[n]; }
		inline const char &operator[](size_type n) const { return this->_string[n]; }

		/**
		 * Stream insertion operator, must be friend because they cannot be inside the class.
		 */
		friend std::ostream &operator<<(std::ostream &os, const string &_str);
		friend std::istream &operator>>(std::istream &is, string &_str);
	};

	inline std::ostream &operator<<(std::ostream &os, const string &_str) { return os << _str._string; }
	/* This is not standard to make operator>> behave like operator<< in that it will allow extracting a whole line, not just one word */
	inline std::istream &operator>>(std::istream &is, string &_str) { return std::getline(is,  _str._string); }

	inline const string operator+(char chr, const string &str) { string tmp(chr); tmp += str; return tmp; }
	inline const string operator+(const char *_str, const string &str) { string tmp(_str); tmp += str; return tmp; }
	inline const string operator+(const std::string &_str, const string &str) { string tmp(_str); tmp += str; return tmp; }

	struct hash_ci final
	{
		inline size_t operator()(const string &s) const
		{
			return std::hash<std::string>()(s.lower().str());
		}
	};

	struct hash_cs final
	{
		inline size_t operator()(const string &s) const
		{
			return std::hash<std::string>()(s.str());
		}
	};

	struct compare final
	{
		inline bool operator()(const string &s1, const string &s2) const
		{
			return s1.equals_ci(s2);
		}
	};

	template<typename T>
	using map = std::map<string, T, ci::less>;

	template<typename T>
	using multimap = std::multimap<string, T, ci::less>;

	template<typename T>
	using unordered_map = std::unordered_map<string, T, hash_ci, compare>;

#if !REPRODUCIBLE_BUILD
	static const char *const compiled = __TIME__ " " __DATE__;
#endif

	/** The time Anope started.
	 */
	extern CoreExport time_t StartTime;

	/** The value to return from main()
	 */
	extern int ReturnValue;

	extern sig_atomic_t Signal;
	extern CoreExport bool Quitting;
	extern CoreExport bool Restarting;
	extern CoreExport Anope::string QuitReason;

	/** The current system time, which is pretty close to being accurate.
	 * Use this unless you need very specific time checks
	 */
	extern CoreExport time_t CurTime;
	extern CoreExport long long CurTimeNs;

	/** The debug level we are running at.
	 */
	extern CoreExport unsigned Debug;

	/** Other command line options.
	 */
	extern CoreExport bool ReadOnly, NoFork, NoThird, NoPID, NoExpire, ProtocolDebug;

	/** The root of the Anope installation. Usually ~/anope
	 */
	extern CoreExport Anope::string ServicesDir;

	/** Anope binary name (eg anope)
	 */
	extern CoreExport Anope::string ServicesBin;

	/** Various directory paths. These can be set at runtime by command line args
	 */
	extern CoreExport Anope::string ConfigDir;
	extern CoreExport Anope::string DataDir;
	extern CoreExport Anope::string ModuleDir;
	extern CoreExport Anope::string LocaleDir;
	extern CoreExport Anope::string LogDir;

	/** The uplink we are currently connected to
	 */
	extern CoreExport size_t CurrentUplink;

	/** Various methods to determine the Anope version running
	 */
	extern CoreExport string Version();
	extern CoreExport string VersionShort();
	extern CoreExport string VersionBuildString();
	extern CoreExport int VersionMajor();
	extern CoreExport int VersionMinor();
	extern CoreExport int VersionPatch();

	/** Determines if we are still attached to the terminal, and can print
	 * messages to the user via stderr/stdout.
	 * @return true if still attached
	 */
	extern bool AtTerm();

	/** Used to "fork" the process and go into the background during initial startup
	 * while we are AtTerm(). The actual fork is not done here, but earlier, and this
	 * simply notifies the parent via kill() to exit().
	 */
	extern void Fork();

	/** Does something with the signal in Anope::Signal
	 */
	extern void HandleSignal();

	/** One of the first functions called, does general initialization such as reading
	 * command line args, loading the configuration, doing the initial fork() if necessary,
	 * initializing language support, loading modules, and loading databases.
	 * @throws CoreException if something bad went wrong
	 */
	extern bool Init(int ac, char **av);

	/** Calls the save database event
	 */
	extern CoreExport void SaveDatabases();

	/** Check whether two strings match.
	 * @param str The string to check against the pattern (e.g. foobar)
	 * @param mask The pattern to check (e.g. foo*bar)
	 * @param case_sensitive Whether or not the match is case sensitive, default false.
	 * @param use_regex Whether or not to try regex. case_sensitive is not used in regex.
	 */
	extern CoreExport bool Match(const string &str, const string &mask, bool case_sensitive = false, bool use_regex = false);

	/** Converts a string to hex
	 * @param the data to be converted
	 * @return a anope::string containing the hex value
	 */
	extern CoreExport string Hex(const string &data);
	extern CoreExport string Hex(const char *data, unsigned len);

	/** Converts a string from hex
	 * @param src The data to be converted
	 * @param dest The destination string
	 */
	extern CoreExport void Unhex(const string &src, string &dest);
	extern CoreExport void Unhex(const string &src, char *dest, size_t sz);

	/** Base 64 encode a string
	 * @param src The string to encode
	 * @param target Where the encoded string is placed
	 */
	extern CoreExport void B64Encode(const string &src, string &target);

	/** Base 64 decode a string
	 * @param src The base64 encoded string
	 * @param target The plain text result
	 */
	extern CoreExport void B64Decode(const string &src, string &target);

	/** Encrypts what is in 'src' to 'dest'
	 * @param src The source string to encrypt
	 * @param dest The destination where the encrypted string is placed
	 */
	extern CoreExport bool Encrypt(const Anope::string &src, Anope::string &dest);

	/** Returns a sequence of data formatted as the format argument specifies.
	 ** After the format parameter, the function expects at least as many
	 ** additional arguments as specified in format.
	 * @param fmt Format of the Message
	 * @param ... any number of parameters
	 * @return a Anope::string
	 */
	extern CoreExport string printf(const char *fmt, ...) ATTR_FORMAT(1, 2);

	/** Return the last error code
	 * @return The error code
	 */
	extern CoreExport int LastErrorCode();

	/** Return the last error, uses errno/GetLastError() to determine this
	 * @return An error message
	 */
	extern CoreExport string LastError();

	/** Determines if a path is a file
	 */
	extern CoreExport bool IsFile(const Anope::string &file);

	/** Converts a string into seconds
	 * @param s The string, eg 3d
	 * @return The time represented by the string, eg 259,200
	 */
	extern CoreExport time_t DoTime(const Anope::string &s);

	/** Retrieves a human readable string representing the time in seconds
	 * @param seconds The time on seconds, eg 60
	 * @param nc The account to use language settings for to translate this string, if applicable
	 * @return A human readable string, eg "1 minute"
	 */
	extern CoreExport Anope::string Duration(time_t seconds, const NickCore *nc = NULL);

	/** Generates a human readable string of type "expires in ..."
	 * @param expires time in seconds
	 * @param nc The account to use language settings for to translate this string, if applicable
	 * @return A human readable string, eg "expires in 5 days"
	 */
	extern CoreExport Anope::string Expires(time_t seconds, const NickCore *nc = NULL);

	/** Converts a time in seconds (epoch) to a human readable format.
	 * @param t The time
	 * @param nc The account to use language settings for to translate this string, if applicable
	 * @param short_output If true, the output is just a date (eg, "Apr 12 20:18:22 2009 MSD"), else it includes the date and how long ago/from now that date is, (eg "Apr 12 20:18:22 2009 MSD (1313 days, 9 hours, 32 minutes ago)"
	 */
	extern CoreExport Anope::string strftime(time_t t, const NickCore *nc = NULL, bool short_output = false);

	/** Normalize buffer, stripping control characters and colors
	 * @param A string to be parsed for control and color codes
	 * @return A string stripped of control and color codes
	 */
	extern CoreExport Anope::string NormalizeBuffer(const Anope::string &);

	/** Parses a raw message from the uplink and calls its command handler.
	 * @param message Raw message from the uplink
	 */
	extern void Process(const Anope::string &message);

	/** Calls the command handler for an already parsed message.
	 * @param source Source of the message.
	 * @param command Command name.
	 * @param params Any extra parameters.
	 * @param tags IRCv3 message tags.
	 */
	extern CoreExport void ProcessInternal(MessageSource &src, const Anope::string &command, const std::vector<Anope::string> &params, const Anope::map<Anope::string> & tags);

	/** Does a blocking dns query and returns the first IP.
	 * @param host host to look up
	 * @param type inet addr type
	 * @return the IP if it was found, else the host
	 */
	extern Anope::string Resolve(const Anope::string &host, int type);

	/** Does a blocking dns query and returns all IPs.
	 * @param host host to look up
	 * @param type inet addr type
	 * @return A list of all IPs that the host resolves to
	 */
	extern std::vector<Anope::string> ResolveMultiple(const Anope::string &host, int type);

	/** Generate a string of random letters and numbers
	 * @param len The length of the string returned
	 */
	extern CoreExport Anope::string Random(size_t len);

	/** Generate a random number. */
	extern CoreExport int RandomNumber();

	/** Calculates the levenshtein distance between two strings.
	 * @param s1 The first string.
	 * @param s2 The second string.
	 */
	extern CoreExport size_t Distance(const Anope::string &s1, const Anope::string &s2);

	/** Update the current time. */
	extern CoreExport void UpdateTime();

	/** Expands a path fragment that is relative to the base directory.
	 * @param base The base directory that it is relative to.
	 * @param fragment The fragment to expand.
	 */
	extern CoreExport Anope::string Expand(const Anope::string &base, const Anope::string &fragment);

	/** Expands a config path. */
	inline auto ExpandConfig(const Anope::string &path) { return Expand(ConfigDir, path); }

	/** Expands a data path. */
	inline auto ExpandData(const Anope::string &path) { return Expand(DataDir, path); }

	/** Expands a locale path. */
	inline auto ExpandLocale(const Anope::string &path) { return Expand(LocaleDir, path); }

	/** Expands a log path. */
	inline auto ExpandLog(const Anope::string &path) { return Expand(LogDir, path); }

	/** Expands a module path. */
	inline auto ExpandModule(const Anope::string &path) { return Expand(ModuleDir, path); }

	/** Formats a CTCP message for sending to a client.
	 * @param name The name of the CTCP.
	 * @param body If present then the body of the CTCP.
	 * @return A formatted CTCP ready to send to a client.
	 */
	extern CoreExport Anope::string FormatCTCP(const Anope::string &name, const Anope::string &body = "");

	/** Parses a CTCP message received from a client.
	 * @param text The raw message to parse.
	 * @param name The location to store the name of the CTCP.
	 * @param body The location to store body of the CTCP if one is present.
	 * @return True if the message was a well formed CTCP; otherwise, false.
	 */
	extern CoreExport bool ParseCTCP(const Anope::string &text, Anope::string &name, Anope::string &body);

	/** Replaces template variables within a string with values from a map.
	 * @param str The string to template from.
	 * @param vars The variables to replace within the string.
	 * @return The specified string with all variables replaced within it.
	 */
	extern CoreExport Anope::string Template(const Anope::string &str, const Anope::map<Anope::string> &vars);
}

/** sepstream allows for splitting token separated lists.
 * Each successive call to sepstream::GetToken() returns
 * the next token, until none remain, at which point the method returns
 * an empty string.
 */
class CoreExport sepstream
{
private:
	/** Original string.
	 */
	Anope::string tokens;
	/** Separator value
	 */
	char sep;
	/** Current string position
	 */
	size_t pos = 0;
	/** If set then GetToken() can return an empty string
	 */
	bool allow_empty;
public:
	/** Create a sepstream and fill it with the provided data
	 */
	sepstream(const Anope::string &source, char separator, bool allowempty = false);

	/** Retrieves the underlying string. */
	const auto &GetString() const { return tokens; }

	/** Fetch the next token from the stream
	 * @param token The next token from the stream is placed here
	 * @return True if tokens still remain, false if there are none left
	 */
	bool GetToken(Anope::string &token);

	/** Gets token number 'num' from the stream
	 * @param token The token is placed here
	 * @param num The token number to fetch
	 * @return True if the token was able to be fetched
	 */
	bool GetToken(Anope::string &token, int num);

	/** Gets every token from this stream
	 * @param token Tokens are pushed back here
	 */
	template<typename T> void GetTokens(T &token)
	{
		token.clear();
		Anope::string t;
		while (this->GetToken(t))
			token.push_back(t);
	}

	/** Gets token number 'num' from the stream and all remaining tokens.
	 * @param token The token is placed here
	 * @param num The token number to fetch
	 * @return True if the token was able to be fetched
	 */
	bool GetTokenRemainder(Anope::string &token, int num);

	/** Determines the number of tokens in this stream.
	 * @return The number of tokens in this stream
	 */
	int NumTokens();

	/** Fetch the entire remaining stream, without tokenizing
	 * @return The remaining part of the stream
	 */
	Anope::string GetRemaining();

	/** Returns true if the end of the stream has been reached
	 * @return True if the end of the stream has been reached, otherwise false
	 */
	bool StreamEnd();
};

/** A derived form of sepstream, which separates on commas
 */
class commasepstream final
	: public sepstream
{
public:
	/** Initialize with comma separator
	 */
	commasepstream(const Anope::string &source, bool allowempty = false) : sepstream(source, ',', allowempty) { }
};

/** A derived form of sepstream, which separates on spaces
 */
class spacesepstream final
	: public sepstream
{
public:
	/** Initialize with space separator
	 */
	spacesepstream(const Anope::string &source) : sepstream(source, ' ') { }
};

/** This class can be used on its own to represent an exception, or derived to represent a module-specific exception.
 * When a module whishes to abort, e.g. within a constructor, it should throw an exception using ModuleException or
 * a class derived from ModuleException. If a module throws an exception during its constructor, the module will not
 * be loaded. If this happens, the error message returned by ModuleException::GetReason will be displayed to the user
 * attempting to load the module, or dumped to the console if the ircd is currently loading for the first time.
 */
class CoreExport CoreException
	: public std::exception
{
protected:
	/** Holds the error message to be displayed
	 */
	Anope::string err;
	/** Source of the exception
	 */
	Anope::string source;
public:
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
	virtual ~CoreException() noexcept = default;
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

class CoreExport ModuleException
	: public CoreException
{
public:
	/** This constructor can be used to specify an error message before throwing.
	 */
	ModuleException(const Anope::string &message) : CoreException(message, "A Module") { }
	/** This destructor solves world hunger, cancels the world debt, and causes the world to end.
	 * Actually no, it does nothing. Never mind.
	 * @throws Nothing!
	 */
	virtual ~ModuleException() noexcept = default;
};

/** Casts to be used instead of dynamic_cast, this uses dynamic_cast
 * for debug builds and static_cast on release builds
 * to speed up the program because dynamic_cast relies on RTTI.
 */
#ifdef DEBUG_BUILD
# include <typeinfo>
template<typename T, typename O> inline T anope_dynamic_static_cast(O ptr)
{
	T ret = dynamic_cast<T>(ptr);
	if (ptr != NULL && ret == NULL)
		throw CoreException(Anope::string("anope_dynamic_static_cast<") + typeid(T).name() + ">(" + typeid(O).name() + ") fail");
	return ret;
}
#else
template<typename T, typename O> inline T anope_dynamic_static_cast(O ptr)
{
	return static_cast<T>(ptr);
}
#endif

#include "convert.h"
