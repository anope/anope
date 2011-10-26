/*
 * Copyright (C) 2002-2011 InspIRCd Development Team
 * Copyright (C) 2008-2011 Anope Team <team@anope.org>
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

