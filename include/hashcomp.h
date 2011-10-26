/*
 * Copyright (C) 2002-2011 InspIRCd Development Team
 * Copyright (C) 2009-2011 Anope Team <team@anope.org>
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

#ifdef _WIN32
# ifdef MODULE_COMPILE
#  define CoreExport __declspec(dllimport)
#  define DllExport __declspec(dllexport)
# else
#  define CoreExport __declspec(dllexport)
#  define DllExport __declspec(dllimport)
# endif
#else
# define CoreExport
# define DllExport
#endif

#include <string>

namespace Anope
{
	class string;
}

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
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,                                   /* 0-19 */
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,                         /* 20-39 */
	40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,                         /* 40-59 */
	60, 61, 62, 63, 64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,             /* 60-79 */
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 94, 95, 96, 97, 98, 99,           /* 80-99 */
	100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,     /* 100-119 */
	120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,     /* 120-139 */
	140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,     /* 140-159 */
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,     /* 160-179 */
	180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,     /* 180-199 */
	200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219,     /* 200-219 */
	220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,     /* 220-239 */
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255                          /* 240-255 */
};

/** Case insensitive map, ASCII rules.
 * That is;
 * [ != {, but A == a.
 */
unsigned const char ascii_case_insensitive_map[256] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,                                   /* 0-19 */
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,                         /* 20-39 */
	40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,                         /* 40-59 */
	60, 61, 62, 63, 64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,             /* 60-79 */
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 94, 95, 96, 97, 98, 99,              /* 80-99 */
	100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,     /* 100-119 */
	120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,     /* 120-139 */
	140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,     /* 140-159 */
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,     /* 160-179 */
	180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,     /* 180-199 */
	200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219,     /* 200-219 */
	220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,     /* 220-239 */
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255                          /* 240-255 */
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

namespace std
{
	/** An overload for std::less<ci::string> that uses Anope::string, passed for the third template argument
	 * to std::map and std::multimap
	 */
	template<> struct CoreExport less<ci::string>
	{
	 public:
		/** Compare two Anope::strings as ci::strings and find which one is less
		 * @param s1 The first string
		 * @param s2 The second string
		 * @return true if s1 < s2, else false
		 */
		bool operator()(const Anope::string &s1, const Anope::string &s2) const;
	};

	/** An overload for std;:less<irc::string> that uses Anope::string, passed for the third tempalte argument
	 * to std::map and std::multimap
	 */
	template<> struct CoreExport less<irc::string>
	{
	 public:
		/** Compare two Anope::strings as irc::strings and find which one is less
		 * @param s1 The first string
		 * @param s2 The second string
		 * @return true if s1 < s2, else false
		 */
		bool operator()(const Anope::string &s1, const Anope::string &s2) const;
	};
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

#endif // HASHCOMP_H
