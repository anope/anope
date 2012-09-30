/*
 * Copyright (C) 2002-2011 InspIRCd Development Team
 * Copyright (C) 2009-2012 Anope Team <team@anope.org>
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
#include <locale>

#include "services.h"

namespace Anope
{
	class string;

	extern std::locale casemap;

	template<typename charT>
	class ascii_ctype : public std::ctype<charT>
	{
	 public:
		charT do_toupper(charT c) const anope_override
		{
			if (c >= 'a' && c <= 'z')
				return c - 32;
			else
				return c;
		}

		charT do_tolower(charT c) const anope_override
		{
			if (c >= 'A' && c <= 'Z')
				return c + 32;
			else
				return c;
		}
	};

	template<typename charT>
	class rfc1459_ctype : public ascii_ctype<charT>
	{
	 public:
		charT do_toupper(charT c) const anope_override
		{
			if (c == '{' || c == '}' || c == '|')
				return c - 32;
			else
				return ascii_ctype<charT>::do_toupper(c);
		}

		charT do_tolower(charT c) const anope_override
		{
			if (c == '[' || c == ']' || c == '\\')
				return c + 32;
			else
				return ascii_ctype<charT>::do_tolower(c);
		}
	};
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

	struct CoreExport less
	{
		/** Compare two Anope::strings as ci::strings and find which one is less
		 * @param s1 The first string
		 * @param s2 The second string
		 * @return true if s1 < s2, else false
		 */
		bool operator()(const Anope::string &s1, const Anope::string &s2) const;
	};
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

/* Define operators != for ci::string to std::string for easy comparison
 */
inline bool operator!=(const ci::string &leftval, const std::string &rightval)
{
	return !(leftval == rightval.c_str());
}

/* Define operators != for std::string to ci::string for easy comparison
 */
inline bool operator!=(const std::string &leftval, const ci::string &rightval)
{
	return !(leftval.c_str() == rightval);
}

#endif // HASHCOMP_H