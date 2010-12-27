/*
 * Copyright (C) 2002-2010 InspIRCd Development Team
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * These classes have been copied from InspIRCd and modified
 * for use in Anope.
 */

#include "services.h"

/*
 *
 * This is an implementation of two special string classes:
 *
 * irc::string which is a case-insensitive equivalent to
 * std::string which is not only case-insensitive but
 * can also do scandanavian comparisons, e.g. { = [, etc.
 *
 * ci::string which is a case-insensitive equivalent to
 * std::string.
 *
 * These classes depend on rfc_case_insensitive_map and
 * ascii_case_insensitive_map
 *
 */
 
/* VS 2008 specific function */
bool Anope::hash::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return s1.str().compare(s2.str()) < 0;
}

/** Hash an Anope::string for unordered_map
 * @param s The string
 * @return A hash value for the string
 */
bool Anope::hash::operator()(const Anope::string &s) const
{
	register size_t t = 0;

	for (Anope::string::const_iterator it = s.begin(), it_end = s.end(); it != it_end; ++it)
		t = 5 * t + *it;

	return t;
}

bool irc::irc_char_traits::eq(char c1st, char c2nd)
{
	return rfc_case_insensitive_map[static_cast<unsigned char>(c1st)] == rfc_case_insensitive_map[static_cast<unsigned char>(c2nd)];
}

bool irc::irc_char_traits::ne(char c1st, char c2nd)
{
	return rfc_case_insensitive_map[static_cast<unsigned char>(c1st)] != rfc_case_insensitive_map[static_cast<unsigned char>(c2nd)];
}

bool irc::irc_char_traits::lt(char c1st, char c2nd)
{
	return rfc_case_insensitive_map[static_cast<unsigned char>(c1st)] < rfc_case_insensitive_map[static_cast<unsigned char>(c2nd)];
}

int irc::irc_char_traits::compare(const char *str1, const char *str2, size_t n)
{
	for (unsigned i = 0; i < n; ++i)
	{
		if (rfc_case_insensitive_map[static_cast<unsigned char>(*str1)] > rfc_case_insensitive_map[static_cast<unsigned char>(*str2)])
			return 1;

		if (rfc_case_insensitive_map[static_cast<unsigned char>(*str1)] < rfc_case_insensitive_map[static_cast<unsigned char>(*str2)])
			return -1;

		if (!*str1 || !*str2)
			return 0;

		++str1;
		++str2;
	}
	return 0;
}

const char *irc::irc_char_traits::find(const char *s1, int n, char c)
{
	while (n-- > 0 && rfc_case_insensitive_map[static_cast<unsigned char>(*s1)] != rfc_case_insensitive_map[static_cast<unsigned char>(c)])
		++s1;
	return n >= 0 ? s1 : NULL;
}

const char irc::irc_char_traits::chartolower(char c1)
{
	return rfc_case_insensitive_map[static_cast<unsigned char>(c1)];
}

/* VS 2008 specific function */
bool irc::hash::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return s1.irc_str().compare(s2.irc_str()) < 0;
}

/** Hash an irc::string for unordered_map
 * @param s The string
 * @return A hash value for the string
 */
size_t irc::hash::operator()(const irc::string &s) const
{
        register size_t t = 0;

        for (irc::string::const_iterator it = s.begin(), it_end = s.end(); it != it_end; ++it)
                t = 5 * t + rfc_case_insensitive_map[static_cast<const unsigned char>(*it)];

        return t;
}

size_t irc::hash::operator()(const Anope::string &s) const
{
        return operator()(s.irc_str());
}

bool ci::ci_char_traits::eq(char c1st, char c2nd)
{
	return ascii_case_insensitive_map[static_cast<unsigned char>(c1st)] == ascii_case_insensitive_map[static_cast<unsigned char>(c2nd)];
}

bool ci::ci_char_traits::ne(char c1st, char c2nd)
{
	return ascii_case_insensitive_map[static_cast<unsigned char>(c1st)] != ascii_case_insensitive_map[static_cast<unsigned char>(c2nd)];
}

bool ci::ci_char_traits::lt(char c1st, char c2nd)
{
	return ascii_case_insensitive_map[static_cast<unsigned char>(c1st)] < ascii_case_insensitive_map[static_cast<unsigned char>(c2nd)];
}

int ci::ci_char_traits::compare(const char *str1, const char *str2, size_t n)
{
	for (unsigned i = 0; i < n; ++i)
	{
		if (ascii_case_insensitive_map[static_cast<unsigned char>(*str1)] > ascii_case_insensitive_map[static_cast<unsigned char>(*str2)])
			return 1;

		if (ascii_case_insensitive_map[static_cast<unsigned char>(*str1)] < ascii_case_insensitive_map[static_cast<unsigned char>(*str2)])
			return -1;

		if (!*str1 || !*str2)
			return 0;

		++str1;
		++str2;
	}
	return 0;
}

const char *ci::ci_char_traits::find(const char *s1, int n, char c)
{
	while (n-- > 0 && ascii_case_insensitive_map[static_cast<unsigned char>(*s1)] != ascii_case_insensitive_map[static_cast<unsigned char>(c)])
		++s1;
	return n >= 0 ? s1 : NULL;
}

const char ci::ci_char_traits::chartolower(char c1)
{
	return ascii_case_insensitive_map[static_cast<unsigned char>(c1)];
}

/* VS 2008 specific function */
bool ci::hash::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return s1.ci_str().compare(s2.ci_str()) < 0;
}

/** Hash a ci::string for unordered_map
 * @param s The string
 * @return A hash value for the string
 */
size_t ci::hash::operator()(const ci::string &s) const
{
	register size_t t = 0;

	for (ci::string::const_iterator it = s.begin(), it_end = s.end(); it != it_end; ++it)
		t = 5 * t + ascii_case_insensitive_map[static_cast<const unsigned char>(*it)];

	return t;
}

size_t ci::hash::operator()(const Anope::string &s) const
{
	return operator()(s.ci_str());
}

const char std::std_char_traits::chartolower(char c1)
{
	return c1;
}

/** Compare two Anope::strings as ci::strings
 * @param s1 The first string
 * @param s2 The second string
 * @return true if they are equal
 */
bool std::equal_to<ci::string>::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return s1.ci_str() == s2.ci_str();
}

/** Compare two Anope::strings as irc::strings
 * @param s1 The first string
 * @param s2 The second string
 * @return true if they are equal
 */
bool std::equal_to<irc::string>::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return s1.irc_str() == s2.irc_str();
}

/** Compare two Anope::strings as ci::strings and find which one is less
 * @param s1 The first string
 * @param s2 The second string
 * @return true if s1 < s2, else false
 */
bool std::less<ci::string>::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return s1.ci_str().compare(s2.ci_str()) < 0;
}

/** Compare two Anope::strings as irc::strings and find which one is les
 * @param s1 The first string
 * @param s2 The second string
 * @return true if s1 < s2, else false
 */
bool std::less<irc::string>::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return s1.irc_str().compare(s2.irc_str()) < 0;
}

sepstream::sepstream(const Anope::string &source, char seperator) : tokens(source), sep(seperator)
{
	last_starting_position = n = tokens.begin();
}

bool sepstream::GetToken(Anope::string &token)
{
	Anope::string::iterator lsp = last_starting_position;

	while (n != tokens.end())
	{
		if (*n == sep || n + 1 == tokens.end())
		{
			last_starting_position = n + 1;
			token = Anope::string(lsp, n + 1 == tokens.end() ? n + 1 : n);

			while (token.length() && token.rfind(sep) == token.length() - 1)
				token.erase(token.end() - 1);

			++n;

			return true;
		}

		++n;
	}

	token.clear();
	return false;
}

const Anope::string sepstream::GetRemaining()
{
	return Anope::string(n, tokens.end());
}

bool sepstream::StreamEnd()
{
	return n == tokens.end();
}

