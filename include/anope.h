/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef ANOPE_H
#define ANOPE_H

#include "hashcomp.h"

namespace Anope
{
	template<typename T> class map : public std::map<string, T> { };
	template<typename T> class insensitive_map : public std::map<string, T, ci::less> { };

	/**
	 * A wrapper string class around all the other string classes, this class will
	 * allow us to only require one type of string everywhere that can be converted
	 * at any time to a specific type of string.
	 */
	class CoreExport string
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
		static const size_type npos = static_cast<size_type>(-1);

		/**
		 * Constructors that can take in any type of string.
		 */
		string() : _string("") { }
		string(char chr) : _string() { _string = chr; }
		string(size_type n, char chr) : _string(n, chr) { }
		string(const char *_str) : _string(_str) { }
		string(const std::string &_str) : _string(_str) { }
		string(const ci::string &_str) : _string(_str.c_str()) { }
		string(const string &_str, size_type pos = 0, size_type n = npos) : _string(_str._string, pos, n) { }
		template <class InputIterator> string(InputIterator first, InputIterator last) : _string(first, last) { }

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
		inline void trim()
		{
			while (!this->_string.empty() && isspace(this->_string[0]))
				this->_string.erase(this->_string.begin());
			while (!this->_string.empty() && isspace(this->_string[this->_string.length() - 1]))
				this->_string.erase(this->_string.length() - 1);
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

		/**
		 * Determine if string consists of only numbers.
		 */
		inline bool is_number_only() const { return this->find_first_not_of("0123456789.-") == npos; }
		inline bool is_pos_number_only() const { return this->find_first_not_of("0123456789.") == npos; }

		/**
		 * Replace parts of the string.
		 */
		inline string replace(size_type pos, size_type n, const string &_str) { return string(this->_string.replace(pos, n, _str._string)); }
		inline string replace(size_type pos, size_type n, const string &_str, size_type pos1, size_type n1) { return string(this->_string.replace(pos, n, _str._string, pos1, n1)); }
		inline string replace(size_type pos, size_type n, size_type n1, char chr) { return string(this->_string.replace(pos, n, n1, chr)); }
		inline string replace(iterator first, iterator last, const string &_str) { return string(this->_string.replace(first, last, _str._string)); }
		inline string replace(iterator first, iterator last, size_type n, char chr) { return string(this->_string.replace(first, last, n, chr)); }
		template <class InputIterator> inline string replace(iterator first, iterator last, InputIterator f, InputIterator l) { return string(this->_string.replace(first, last, f, l)); }
		inline string replace_all_cs(const string &_orig, const string &_repl)
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
		inline string replace_all_ci(const string &_orig, const string &_repl)
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
		inline string lower()
		{
			Anope::string new_string = *this;
			for (size_type i = 0; i < new_string.length(); ++i)
				new_string[i] = std::tolower(new_string[i], Anope::casemap);
			return new_string;
		}

		/**
		 * Get the string in uppercase.
		 */
		inline string upper()
		{
			Anope::string new_string = *this;
			for (size_type i = 0; i < new_string.length(); ++i)
				new_string[i] = std::toupper(new_string[i], Anope::casemap);
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
	};

	inline std::ostream &operator<<(std::ostream &os, const string &_str) { return os << _str._string; }

	inline const string operator+(char chr, const string &str) { string tmp(chr); tmp += str; return tmp; }
	inline const string operator+(const char *_str, const string &str) { string tmp(_str); tmp += str; return tmp; }
	inline const string operator+(const std::string &_str, const string &str) { string tmp(_str); tmp += str; return tmp; }

	static const char *const compiled = __TIME__ " " __DATE__;

	/** The current system time, which is pretty close to being accurate.
	 * Use this unless you need very specific time checks
	 */
	extern CoreExport time_t CurTime;

	extern CoreExport string Version();
	extern CoreExport string VersionShort();
	extern CoreExport string VersionBuildString();
	extern CoreExport int VersionMajor();
	extern CoreExport int VersionMinor();
	extern CoreExport int VersionPatch();

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

	/** Returns a sequence of data formatted as the format argument specifies.
	 ** After the format parameter, the function expects at least as many
	 ** additional arguments as specified in format.
	 * @param fmt Format of the Message
	 * @param ... any number of parameters
	 * @return a Anope::string
	 */
	extern CoreExport string printf(const char *fmt, ...);

	/** Return the last error code
	 * @return The error code
	 */
	extern CoreExport int LastErrorCode();

	/** Return the last error, uses errno/GetLastError() to determine this
	 * @return An error message
	 */
	extern CoreExport const string LastError();
}

/** sepstream allows for splitting token seperated lists.
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
	/** Last position of a seperator token
	 */
	Anope::string::iterator last_starting_position;
	/** Current string position
	 */
	Anope::string::iterator n;
	/** Seperator value
	 */
	char sep;
 public:
	/** Create a sepstream and fill it with the provided data
	 */
	sepstream(const Anope::string &source, char seperator);
	virtual ~sepstream() { }

	/** Fetch the next token from the stream
	 * @param token The next token from the stream is placed here
	 * @return True if tokens still remain, false if there are none left
	 */
	virtual bool GetToken(Anope::string &token);

	/** Fetch the entire remaining stream, without tokenizing
	 * @return The remaining part of the stream
	 */
	virtual const Anope::string GetRemaining();

	/** Returns true if the end of the stream has been reached
	 * @return True if the end of the stream has been reached, otherwise false
	 */
	virtual bool StreamEnd();
};

/** A derived form of sepstream, which seperates on commas
 */
class commasepstream : public sepstream
{
 public:
	/** Initialize with comma seperator
	 */
	commasepstream(const Anope::string &source) : sepstream(source, ',') { }
};

/** A derived form of sepstream, which seperates on spaces
 */
class spacesepstream : public sepstream
{
 public:
	/** Initialize with space seperator
	 */
	spacesepstream(const Anope::string &source) : sepstream(source, ' ') { }
};

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

class ConvertException : public CoreException
{
 public:
	ConvertException(const Anope::string &reason = "") : CoreException(reason) { }

	virtual ~ConvertException() throw() { }
};

/** Convert something to a string
 */
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

/** Casts to be used instead of dynamic_cast, this uses dynamic_cast
 * for debug builds and static_cast/reinterpret_cast on releass builds
 * to speed up the program because dynamic_cast relies on RTTI.
 */
#ifdef DEBUG_BUILD
# include <typeinfo>
#endif
template<typename T, typename O> inline T anope_dynamic_static_cast(O ptr)
{
#ifdef DEBUG_BUILD
	T ret = dynamic_cast<T>(ptr);
	if (ptr != NULL && ret == NULL)
		throw CoreException(Anope::string("anope_dynamic_static_cast<") + typeid(T).name() + ">(" + typeid(O).name() + ") fail");
	return ret;
#else
	return static_cast<T>(ptr);
#endif
}

template<typename T, typename O> inline T anope_dynamic_reinterpret_cast(O ptr)
{
#ifdef DEBUG_BUILD
	T ret = dynamic_cast<T>(ptr);
	if (ptr != NULL && ret == NULL)
		throw CoreException(Anope::string("anope_dynamic_reinterpret_cast<") + typeid(T).name() + ">(" + typeid(O).name() + ") fail");
	return ret;
#else
	return reinterpret_cast<T>(ptr);
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

	Anope::string ToString() const
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

	std::vector<Anope::string> ToVector() const
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

#endif // ANOPE_H
