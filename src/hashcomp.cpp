/*
 * Copyright (C) 2002-2010 InspIRCd Development Team
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * These classes have been copied from InspIRCd and modified
 * for use in Anope.
 */

#include "anope.h"

/******************************************************
 *
 * This is the implementation of our special irc::string
 * class which is a case-insensitive equivalent to
 * std::string which is not only case-insensitive but
 * can also do scandanavian comparisons, e.g. { = [, etc.
 *
 * This class depends on the const array 'national_case_insensitive_map'.
 *
 ******************************************************/

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

/** Compare two std::string's values for hashing in hash_map
 * @param s1 The first string
 * @param s2 The second string
 * @return similar to strcmp, zero for equal, less than zero for str1
 * being less and greater than zero for str1 being greater than str2.
 */
bool hash_compare_std_string::operator()(const std::string &s1, const std::string &s2) const
{
	register int i = std::char_traits<char>::compare(s1.c_str(), s2.c_str(), s1.length() < s2.length() ? s1.length() : s2.length());
	if (!i)
		return s1.length() < s2.length();
	return i < 0;
}

bool hash_compare_std_string::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return operator()(s1.str(), s2.str());
}

/** Return a hash value for a string
 * @param s The string
 * @return The hash value
 */
size_t hash_compare_std_string::operator()(const std::string &s) const
{
	register size_t t = 0;

	for (std::string::const_iterator it = s.begin(), it_end = s.end(); it != it_end; ++it)
		t = 5 * t + static_cast<const unsigned char>(*it);

	return t;
}

size_t hash_compare_std_string::operator()(const Anope::string &s) const
{
	return operator()(s.str());
}

/** Compare two ci::string's values for hashing in hash_map
 * @param s1 The first string
 * @param s2 The second string
 * @return similar to strcmp, zero for equal, less than zero for str1
 * being less and greater than zero for str1 being greater than str2.
 */
bool hash_compare_ci_string::operator()(const ci::string &s1, const ci::string &s2) const
{
	register int i = ci::ci_char_traits::compare(s1.c_str(), s2.c_str(), s1.length() < s2.length() ? s1.length() : s2.length());
	if (!i)
		return s1.length() < s2.length();
	return i < 0;
}

bool hash_compare_ci_string::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return operator()(s1.ci_str(), s2.ci_str());
}

/** Return a hash value for a string using case insensitivity
 * @param s The string
 * @return The hash value
 */
size_t hash_compare_ci_string::operator()(const ci::string &s) const
{
	register size_t t = 0;

	for (ci::string::const_iterator it = s.begin(), it_end = s.end(); it != it_end; ++it)
		t = 5 * t + ascii_case_insensitive_map[static_cast<const unsigned char>(*it)];

	return t;
}

size_t hash_compare_ci_string::operator()(const Anope::string &s) const
{
	return operator()(s.ci_str());
}

/** Compare two irc::string's values for hashing in hash_map
 * @param s1 The first string
 * @param s2 The second string
 * @return similar to strcmp, zero for equal, less than zero for str1
 * being less and greater than zero for str1 being greater than str2.
 */
bool hash_compare_irc_string::operator()(const irc::string &s1, const irc::string &s2) const
{
	register int i = irc::irc_char_traits::compare(s1.c_str(), s2.c_str(), s1.length() < s2.length() ? s1.length() : s2.length());
	if (!i)
		return s1.length() < s2.length();
	return i < 0;
}

bool hash_compare_irc_string::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return operator()(s1.irc_str(), s2.irc_str());
}

/** Return a hash value for a string using RFC1459 case sensitivity rules
 * @param s The string
 * @return The hash value
 */
size_t hash_compare_irc_string::operator()(const irc::string &s) const
{
	register size_t t = 0;

	for (irc::string::const_iterator it = s.begin(), it_end = s.end(); it != it_end; ++it)
		t = 5 * t + rfc_case_insensitive_map[static_cast<const unsigned char>(*it)];

	return t;
}

size_t hash_compare_irc_string::operator()(const Anope::string &s) const
{
	return operator()(s.irc_str());
}
