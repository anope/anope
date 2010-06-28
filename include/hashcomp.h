/*
 * Copyright (C) 2002-2010 InspIRCd Development Team
 * Copyright (C) 2009-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * These classes have been copied from InspIRCd and modified
 * for use in Anope.
 *
 *
 */

#ifndef HASHCOMP_H
#define HASHCOMP_H

#include <string>

#ifndef _WIN32
//# ifdef HASHMAP_DEPRECATED /* If gcc ver > 4.3 */
# if 1
/* GCC4.3+ has deprecated hash_map and uses tr1. But of course, uses a different include to MSVC. */
#  include <tr1/unordered_map>
#  define unordered_map_namespace std::tr1
# else
#  include <ext/hash_map>
/* Oddball linux namespace for hash_map */
#  define unordered_map_namespace __gnu_cxx
#  define unordered_map hash_map
# endif
#else
# if _MSV_VER >= 1600
/* MSVC 2010+ has tr1. Though MSVC and GCC use different includes! */
#  include <unordered_map>
#  define unordered_map_namespace std::tr1
# else
#  include <hash_map>
#  define unordered_map_namespace stdext
#  define unordered_map hash_map
# endif
#endif

/*******************************************************
 * This file contains classes and templates that deal
 * with the comparison and hashing of 'irc strings'.
 * An 'irc string' is a string which compares in a
 * case insensitive manner, and as per RFC 1459 will
 * treat [ identical to {, ] identical to }, and \
 * as identical to |.
 *
 * Our hashing functions are designed  to accept
 * std::string and compare/hash them as type irc::string
 * by converting them internally. This makes them
 * backwards compatible with other code which is not
 * aware of irc::string.
 *******************************************************/

/** A mapping of uppercase to lowercase, including scandinavian
 * 'oddities' as specified by RFC1459, e.g. { -> [, and | -> \
 */
unsigned const char rfc_case_insensitive_map[256] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,								/* 0-19 */
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,						/* 20-39 */
	40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,						/* 40-59 */
	60, 61, 62, 63, 64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,			/* 60-79 */
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 94, 95, 96, 97, 98, 99,		/* 80-99 */
	100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,	/* 100-119 */
	120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,	/* 120-139 */
	140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,	/* 140-159 */
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,	/* 160-179 */
	180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,	/* 180-199 */
	200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219,	/* 200-219 */
	220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,	/* 220-239 */
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255						/* 240-255 */
};

/** Case insensitive map, ASCII rules.
 * That is;
 * [ != {, but A == a.
 */
unsigned const char ascii_case_insensitive_map[256] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,								/* 0-19 */
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,						/* 20-39 */
	40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,						/* 40-59 */
	60, 61, 62, 63, 64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,			/* 60-79 */
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 94, 95, 96, 97, 98, 99,			/* 80-99 */
	100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,	/* 100-119 */
	120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,	/* 120-139 */
	140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,	/* 140-159 */
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,	/* 160-179 */
	180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,	/* 180-199 */
	200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219,	/* 200-219 */
	220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,	/* 220-239 */
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255						/* 240-255 */
};

/** The irc namespace contains a number of helper classes.
 */
namespace irc
{
	/** The irc_char_traits class is used for RFC-style comparison of strings.
	 * This class is used to implement irc::string, a case-insensitive, RFC-
	 * comparing string class.
	 */
	struct irc_char_traits : std::char_traits<char>
	{
		/** Check if two chars match.
		 * @param c1st First character
		 * @param c2nd Second character
		 * @return true if the characters are equal
		 */
		static bool eq(char c1st, char c2nd);

		/** Check if two chars do NOT match.
		 * @param c1st First character
		 * @param c2nd Second character
		 * @return true if the characters are unequal
		 */
		static bool ne(char c1st, char c2nd);

		/** Check if one char is less than another.
		 * @param c1st First character
		 * @param c2nd Second character
		 * @return true if c1st is less than c2nd
		 */
		static bool lt(char c1st, char c2nd);

		/** Compare two strings of size n.
		 * @param str1 First string
		 * @param str2 Second string
		 * @param n Length to compare to
		 * @return similar to strcmp, zero for equal, less than zero for str1
		 * being less and greater than zero for str1 being greater than str2.
		 */
		static int compare(const char *str1, const char *str2, size_t n);

		/** Find a char within a string up to position n.
		 * @param s1 String to find in
		 * @param n Position to search up to
		 * @param c Character to search for
		 * @return Pointer to the first occurance of c in s1
		 */
		static const char *find(const char *s1, int n, char c);
	};

	/** This typedef declares irc::string based upon irc_char_traits.
	 */
	typedef std::basic_string<char, irc_char_traits, std::allocator<char> > string;
}

/** The ci namespace contains a number of helper classes.
 */
namespace ci
{
	/** The ci_char_traits class is used for ASCII-style comparison of strings.
	 * This class is used to implement ci::string, a case-insensitive, ASCII-
	 * comparing string class.
	 */
	struct CoreExport ci_char_traits : std::char_traits<char>
	{
		/** Check if two chars match.
		 * @param c1st First character
		 * @param c2nd Second character
		 * @return true if the characters are equal
		 */
		static bool eq(char c1st, char c2nd);

		/** Check if two chars do NOT match.
		 * @param c1st First character
		 * @param c2nd Second character
		 * @return true if the characters are unequal
		 */
		static bool ne(char c1st, char c2nd);

		/** Check if one char is less than another.
		 * @param c1st First character
		 * @param c2nd Second character
		 * @return true if c1st is less than c2nd
		 */
		static bool lt(char c1st, char c2nd);

		/** Compare two strings of size n.
		 * @param str1 First string
		 * @param str2 Second string
		 * @param n Length to compare to
		 * @return similar to strcmp, zero for equal, less than zero for str1
		 * being less and greater than zero for str1 being greater than str2.
		 */
		static int compare(const char *str1, const char *str2, size_t n);

		/** Find a char within a string up to position n.
		 * @param s1 String to find in
		 * @param n Position to search up to
		 * @param c Character to search for
		 * @return Pointer to the first occurance of c in s1
		 */
		static const char *find(const char *s1, int n, char c);
	};

	/** This typedef declares ci::string based upon ci_char_traits.
	 */
	typedef std::basic_string<char, ci_char_traits, std::allocator<char> > string;
}

/* Define operators for using >> and << with irc::string to an ostream on an istream. */
/* This was endless fun. No. Really. */
/* It was also the first core change Ommeh made, if anyone cares */

/** Operator << for irc::string
 */
inline std::ostream &operator<<(std::ostream &os, const irc::string &str) { return os << std::string(str.c_str()); }

/** Operator >> for irc::string
 */
inline std::istream &operator>>(std::istream &is, irc::string &str)
{
	std::string tmp;
	is >> tmp;
	str = tmp.c_str();
	return is;
}

/** Operator << for ci::string
 */
inline std::ostream &operator<<(std::ostream &os, const ci::string &str) { return os << std::string(str.c_str()); }

/** Operator >> for ci::string
 */
inline std::istream &operator>>(std::istream &is, ci::string &str)
{
	std::string tmp;
	is >> tmp;
	str = tmp.c_str();
	return is;
}

/* Define operators for + and == with irc::string to std::string for easy assignment
 * and comparison
 *
 * Operator +
 */
inline std::string operator+(std::string &leftval, irc::string &rightval)
{
	return leftval + std::string(rightval.c_str());
}

/* Define operators for + and == with irc::string to std::string for easy assignment
 * and comparison
 *
 * Operator +
 */
inline irc::string operator+(irc::string &leftval, std::string &rightval)
{
	return leftval + irc::string(rightval.c_str());
}

/* Define operators for + and == with ci::string to std::string for easy assignment
 * and comparison
 *
 * Operator +
 */
inline std::string operator+(std::string &leftval, ci::string &rightval)
{
	return leftval + std::string(rightval.c_str());
}

/* Define operators for + and == with ci::string to std::string for easy assignment
 * and comparison
 *
 * Operator +
 */
inline ci::string operator+(ci::string &leftval, std::string &rightval)
{
	return leftval + ci::string(rightval.c_str());
}

/* Define operators for + and == with irc::string to ci::string for easy assignment
 * and comparison
 *
 * Operator +
 */
inline irc::string operator+(irc::string &leftval, ci::string &rightval)
{
	return leftval + irc::string(rightval.c_str());
}

/* Define operators for + and == with irc::string to ci::string for easy assignment
 * and comparison
 *
 * Operator +
 */
inline ci::string operator+(ci::string &leftval, irc::string &rightval)
{
	return leftval + ci::string(rightval.c_str());
}

/* Define operators for + and == with irc::string to std::string for easy assignment
 * and comparison
 *
 * Operator ==
 */
inline bool operator==(const std::string &leftval, const irc::string &rightval)
{
	return leftval.c_str() == rightval;
}

/* Define operators for + and == with irc::string to std::string for easy assignment
 * and comparison
 *
 * Operator ==
 */
inline bool operator==(const irc::string &leftval, const std::string &rightval)
{
	return leftval == rightval.c_str();
}

/* Define operators for + and == with ci::string to std::string for easy assignment
 * and comparison
 *
 * Operator ==
 */
inline bool operator==(const std::string &leftval, const ci::string &rightval)
{
	return leftval.c_str() == rightval;
}

/* Define operators for + and == with ci::string to std::string for easy assignment
 * and comparison
 *
 * Operator ==
 */
inline bool operator==(const ci::string &leftval, const std::string &rightval)
{
	return leftval == rightval.c_str();
}

/* Define operators for + and == with irc::string to ci::string for easy assignment
 * and comparison
 *
 * Operator ==
 */
inline bool operator==(const ci::string &leftval, const irc::string &rightval)
{
	return leftval.c_str() == rightval;
}

/* Define operators for + and == with irc::string to ci::string for easy assignment
 * and comparison
 *
 * Operator ==
 */
inline bool operator==(const irc::string &leftval, const ci::string &rightval)
{
	return leftval == rightval.c_str();
}

/* Define operators != for irc::string to std::string for easy comparison
 */
inline bool operator!=(const irc::string &leftval, const std::string &rightval)
{
	return !(leftval == rightval.c_str());
}

/* Define operators != for std::string to irc::string for easy comparison
 */
inline bool operator!=(const std::string &leftval, const irc::string &rightval)
{
	return !(leftval.c_str() == rightval);
}

/* Define operators != for ci::string to std::string for easy comparison
 */
inline bool operator!=(const ci::string &leftval, const std::string &rightval)
{
	return !(leftval == rightval.c_str());
}

/* Define operators != for ci::string to irc::string for easy comparison
 */
inline bool operator!=(const std::string &leftval, const ci::string &rightval)
{
	return !(leftval.c_str() == rightval);
}

/* Define operators != for irc::string to ci::string for easy comparison
 */
inline bool operator!=(const irc::string &leftval, const ci::string &rightval)
{
	return !(leftval == rightval.c_str());
}

/* Define operators != for irc::string to ci::string for easy comparison
 */
inline bool operator!=(const ci::string &leftval, const irc::string &rightval)
{
	return !(leftval.c_str() == rightval);
}

/** Assign an irc::string to a std::string.
 */
//inline std::string assign(const irc::string &other) { return other.c_str(); }

/** Assign a std::string to an irc::string.
 */
//inline irc::string assign(const std::string &other) { return other.c_str(); }

/** Assign an ci::string to a std::string.
 */
//inline std::string assign(const ci::string &other) { return other.c_str(); }

/** Assign a std::string to an ci::string.
 */
//inline ci::string assign(const std::string &other) { return other.c_str(); }

/** Assign an irc::string to a ci::string.
 */
//inline ci::string assign(const irc::string &other) { return other.c_str(); }

/** Assign a ci::string to an irc::string.
 */
//inline irc::string assign(const ci::string &other) { return other.c_str(); }

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
	std::string tokens;
	/** Last position of a seperator token
	 */
	std::string::iterator last_starting_position;
	/** Current string position
	 */
	std::string::iterator n;
	/** Seperator value
	 */
	char sep;
 public:
	/** Create a sepstream and fill it with the provided data
	 */
	sepstream(const std::string &source, char seperator);
	sepstream(const ci::string &source, char seperator);
	sepstream(const char *source, char seperator);
	virtual ~sepstream() { }

	/** Fetch the next token from the stream
	 * @param token The next token from the stream is placed here
	 * @return True if tokens still remain, false if there are none left
	 */
	virtual bool GetToken(std::string &token);
	virtual bool GetToken(ci::string &token);

	/** Fetch the entire remaining stream, without tokenizing
	 * @return The remaining part of the stream
	 */
	virtual const std::string GetRemaining();

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
	commasepstream(const std::string &source) : sepstream(source, ',') { }
	commasepstream(const ci::string &source) : sepstream(source, ',') { }
	commasepstream(const char *source) : sepstream(source, ',') { }
};

/** A derived form of sepstream, which seperates on spaces
 */
class spacesepstream : public sepstream
{
 public:
	/** Initialize with space seperator
	 */
	spacesepstream(const std::string &source) : sepstream(source, ' ') { }
	spacesepstream(const ci::string &source) : sepstream(source, ' ') { }
	spacesepstream(const char *source) : sepstream(source, ' ') { }
};

/** Class used to hash a std::string, given as the third argument to the unordered_map template
 */
class CoreExport hash_compare_std_string
{
 public:
#if defined(_WIN32) && _MSV_VER < 1600
	enum { bucket_size = 4, min_buckets = 8 };

	/** Compare two std::string's values for hashing in hash_map
	 * @param s1 The first string
	 * @param s2 The second string
	 * @return similar to strcmp, zero for equal, less than zero for str1
	 * being less and greater than zero for str1 being greater than str2.
	 */
	bool operator()(const std::string &s1, const std::string &s2) const;
#endif

	/** Return a hash value for a string
	 * @param s The string
	 * @return The hash value
	 */
	size_t operator()(const std::string &s) const;
};

/** Class used to hash a ci::string, given as the third argument to the unordered_map template
 */
class CoreExport hash_compare_ci_string
{
 public:
#if defined(_WIN32) && _MSV_VER < 1600
	enum { bucket_size = 4, min_buckets = 8 };

	/** Compare two ci::string's values for hashing in hash_map
	 * @param s1 The first string
	 * @param s2 The second string
	 * @return similar to strcmp, zero for equal, less than zero for str1
	 * being less and greater than zero for str1 being greater than str2.
	 */
	bool operator()(const ci::string &s1, const ci::string &s2) const;
#endif
	/** Return a hash value for a string using case insensitivity
	 * @param s The string
	 * @return The hash value
	 */
	size_t operator()(const ci::string &s) const;
};

/** Class used to hash a irc::string, given as the third argument to the unordered_map template
 */
class CoreExport hash_compare_irc_string
{
 public:
#if defined(_WIN32) && _MSV_VER < 1600
	enum { bucket_size = 4, min_buckets = 8 };

	/** Compare two irc::string's values for hashing in hash_map
	 * @param s1 The first string
	 * @param s2 The second string
	 * @return similar to strcmp, zero for equal, less than zero for str1
	 * being less and greater than zero for str1 being greater than str2.
	 */
	bool operator()(const irc::string &s1, const irc::string &s2) const;
#endif
	/** Return a hash value for a string using RFC1459 case sensitivity rules
	 * @param s The stirng
	 * @return The hash value
	 */
	size_t operator()(const irc::string &s) const;
};

#endif // HASHCOMP_H
