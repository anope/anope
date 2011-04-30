/*
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef ANOPE_H
#define ANOPE_H

#include <string>
#include <vector>
#include <set>
#include "hashcomp.h"

class Message;

namespace Anope
{
	template<typename T> class map : public std::map<string, T> { };
	template<typename T> class insensitive_map : public std::map<string, T, std::less<ci::string> > { };

	/**
	 * A wrapper string class around all the other string classes, this class will
	 * allow us to only require one type of string everywhere that can be converted
	 * at any time to a specific type of string.
	 */
	class string
	{
	 private:
		/**
		 * The actual string is stored in an std::string as it can be converted to
		 * ci::string, irc::string, or a C-style string at any time.
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
		string(const irc::string &_str) : _string(_str.c_str()) { }
		string(const string &_str, size_type pos = 0, size_type n = npos) : _string(_str._string, pos, n) { }
		template <class InputIterator> string(InputIterator first, InputIterator last) : _string(first, last) { }

		/**
		 * Assignment operators, so any type of string can be assigned to this class.
		 */
		inline string &operator=(char chr) { this->_string = chr; return *this; }
		inline string &operator=(const char *_str) { this->_string = _str; return *this; }
		inline string &operator=(const std::string &_str) { this->_string = _str; return *this; }
		inline string &operator=(const ci::string &_str) { this->_string = _str.c_str(); return *this; }
		inline string &operator=(const irc::string &_str) { this->_string = _str.c_str(); return *this; }
		inline string &operator=(const string &_str) { if (this != &_str) this->_string = _str._string; return *this; }

		/**
		 * Equality operators, to compare to any type of string.
		 */
		inline bool operator==(const char *_str) const { return this->_string == _str; }
		inline bool operator==(const std::string &_str) const { return this->_string == _str; }
		inline bool operator==(const ci::string &_str) const { return ci::string(this->_string.c_str()) == _str; }
		inline bool operator==(const irc::string &_str) const { return irc::string(this->_string.c_str()) == _str; }
		inline bool operator==(const string &_str) const { return this->_string == _str._string; }

		inline bool equals_cs(const char *_str) const { return this->_string == _str; }
		inline bool equals_cs(const std::string &_str) const { return this->_string == _str; }
		inline bool equals_cs(const ci::string &_str) const { return this->_string == _str.c_str(); }
		inline bool equals_cs(const irc::string &_str) const { return this->_string == _str.c_str(); }
		inline bool equals_cs(const string &_str) const { return this->_string == _str._string; }

		inline bool equals_ci(const char *_str) const { return ci::string(this->_string.c_str()) == _str; }
		inline bool equals_ci(const std::string &_str) const { return ci::string(this->_string.c_str()) == _str.c_str(); }
		inline bool equals_ci(const ci::string &_str) const { return _str == this->_string.c_str(); }
		inline bool equals_ci(const irc::string &_str) const { return ci::string(this->_string.c_str()) == _str.c_str(); }
		inline bool equals_ci(const string &_str) const { return ci::string(this->_string.c_str()) == _str._string.c_str(); }

		inline bool equals_irc(const char *_str) const { return irc::string(this->_string.c_str()) == _str; }
		inline bool equals_irc(const std::string &_str) const { return irc::string(this->_string.c_str()) == _str.c_str(); }
		inline bool equals_irc(const ci::string &_str) const { return irc::string(this->_string.c_str()) == _str.c_str(); }
		inline bool equals_irc(const irc::string &_str) const { return _str == this->_string.c_str(); }
		inline bool equals_irc(const string &_str) const { return irc::string(this->_string.c_str()) == _str._string.c_str(); }

		/**
		 * Inequality operators, exact opposites of the above.
		 */
		inline bool operator!=(const char *_str) const { return !operator==(_str); }
		inline bool operator!=(const std::string &_str) const { return !operator==(_str); }
		inline bool operator!=(const ci::string &_str) const { return !operator==(_str); }
		inline bool operator!=(const irc::string &_str) const { return !operator==(_str); }
		inline bool operator!=(const string &_str) const { return !operator==(_str); }

		/**
		 * Compound addition operators, overloaded to do concatenation.
		 */
		inline string &operator+=(char chr) { this->_string += chr; return *this; }
		inline string &operator+=(const char *_str) { this->_string += _str; return *this; }
		inline string &operator+=(const std::string &_str) { this->_string += _str; return *this; }
		inline string &operator+=(const ci::string &_str) { this->_string += _str.c_str(); return *this; }
		inline string &operator+=(const irc::string &_str) { this->_string += _str.c_str(); return *this; }
		inline string &operator+=(const string &_str) { if (this != &_str) this->_string += _str._string; return *this; }

		/**
		 * Addition operators, overloaded to do concatenation.
		 */
		inline const string operator+(char chr) const { return string(*this) += chr; }
		inline const string operator+(const char *_str) const { return string(*this) += _str; }
		inline const string operator+(const std::string &_str) const { return string(*this) += _str; }
		inline const string operator+(const ci::string &_str) const { return string(*this) += _str; }
		inline const string operator+(const irc::string &_str) const { return string(*this) += _str; }
		inline const string operator+(const string &_str) const { return string(*this) += _str; }

		friend const string operator+(char chr, const string &str);
		friend const string operator+(const char *_str, const string &str);
		friend const string operator+(const std::string &_str, const string &str);
		friend const string operator+(const ci::string &_str, const string &str);
		friend const string operator+(const irc::string &_str, const string &str);

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
		inline irc::string irc_str() const { return irc::string(this->_string.c_str()); }

		/**
		 * Returns if the string is empty or not.
		 */
		inline bool empty() const { return this->_string.empty(); }

		/**
		 * Returns the string's length.
		 */
		inline size_type length() const { return this->_string.length(); }

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

	/** Hash an Anope::string for unorderd_map, passed as the third template arg to unordered_map
	 */
	struct hash
	{
		/* VS 2008 specific code */
		enum { bucket_size = 4, min_buckets = 8 };
		bool operator()(const string &s1, const string &s2) const;
		/* End of 2008 specific code */

		/** Hash an Anope::string for unordered_map
		 * @param s The string
		 * @return A hash value for the string
		 */
		bool operator()(const string &s) const;
	};

	inline std::ostream &operator<<(std::ostream &os, const string &_str) { return os << _str._string; }

	inline const string operator+(char chr, const string &str) { string tmp(chr); tmp += str; return tmp; }
	inline const string operator+(const char *_str, const string &str) { string tmp(_str); tmp += str; return tmp; }
	inline const string operator+(const std::string &_str, const string &str) { string tmp(_str); tmp += str; return tmp; }
	inline const string operator+(const ci::string &_str, const string &str) { string tmp(_str); tmp += str; return tmp; }
	inline const string operator+(const irc::string &_str, const string &str) { string tmp(_str); tmp += str; return tmp; }

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
	extern CoreExport int VersionBuild();

	/** Check whether two strings match.
	 * @param str The string to check against the pattern (e.g. foobar)
	 * @param mask The pattern to check (e.g. foo*bar)
	 * @param case_sensitive Whether or not the match is case sensitive, default false.
	 */
	extern CoreExport bool Match(const Anope::string &str, const Anope::string &mask, bool case_sensitive = false);

	/** Find a message in the message table
	 * @param name The name of the message were looking for
	 * @return NULL if we cant find it, or a pointer to the Message if we can
	 */
	extern CoreExport std::vector<Message *> FindMessage(const string &name);

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
	extern CoreExport void Unhex(const Anope::string &src, Anope::string &dest);
	extern CoreExport void Unhex(const Anope::string &src, char *dest);
	
	/** Base 64 encode a string
	 * @param src The string to encode
	 * @param target Where the encoded string is placed
	 */
	extern CoreExport void B64Encode(const Anope::string &src, Anope::string &target);
	
	/** Base 64 decode a string
	 * @param src The base64 encoded string
	 * @param target The plain text result
	 */
	extern CoreExport void B64Decode(const Anope::string &src, Anope::string &target);

	/** Returns a sequence of data formatted as the format argument specifies.
	 ** After the format parameter, the function expects at least as many
	 ** additional arguments as specified in format.
	 * @param fmt Format of the Message
	 * @param ... any number of parameters
	 * @return a Anope::string
	 */
	extern CoreExport string printf(const char *fmt, ...);

	/** Return the last error, uses errno/GetLastError() to determin this
	 * @return An error message
	 */
	extern CoreExport const Anope::string LastError();
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

/** The base class that most classes in Anope inherit from
 */
class dynamic_reference_base;
class CoreExport Base
{
	/* References to this base class */
	std::set<dynamic_reference_base *> References;
 public:
	Base();
	virtual ~Base();
	void AddReference(dynamic_reference_base *r);
	void DelReference(dynamic_reference_base *r);
	static void operator delete(void *ptr);
};

class dynamic_reference_base : public Base
{
 protected:
	bool invalid;
 public:
	dynamic_reference_base() : invalid(false) { }
	virtual ~dynamic_reference_base() { }
	inline void Invalidate() { this->invalid = true; }
};

template<typename T>
class dynamic_reference : public dynamic_reference_base
{
 protected:
	T *ref;
 public:
	dynamic_reference(T *obj) : ref(obj)
	{
		if (ref)
			ref->AddReference(this);
	}

	virtual ~dynamic_reference()
	{
		if (this->invalid)
		{
			this->invalid = false;
			this->ref = NULL;
		}
		else if (this->operator bool())
			ref->DelReference(this);
	}

	virtual operator bool()
	{
		if (this->invalid)
		{
			this->invalid = false;
			this->ref = NULL;
		}
		return this->ref != NULL;
	}

	virtual inline operator T*()
	{
		if (this->operator bool())
			return this->ref;
		return NULL;
	}

	virtual inline T *operator->()
	{
		if (this->operator bool())
			return this->ref;
		return NULL;
	}

	virtual inline void operator=(T *newref)
	{
		if (this->invalid)
		{
			this->invalid = false;
			this->ref = NULL;
		}
		else if (this->operator bool())
			this->ref->DelReference(this);
		this->ref = newref;
		if (this->operator bool())
			this->ref->AddReference(this);
	}
};

#endif // ANOPE_H
