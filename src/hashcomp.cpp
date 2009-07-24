/*
 * Copyright (C) 2002-2009 InspIRCd Development Team
 * Copyright (C) 2008-2009 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * These classes have been copied from InspIRCd and modified
 * for use in Anope.
 *
 * $Id$
 *
 */

#include "services.h"
#include "hashcomp.h"

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

sepstream::sepstream(const std::string &source, char seperator) : tokens(source), sep(seperator)
{
	last_starting_position = n = tokens.begin();
}

bool sepstream::GetToken(std::string &token)
{
	std::string::iterator lsp = last_starting_position;

	while (n != tokens.end())
	{
		if (*n == sep || n + 1 == tokens.end())
		{
			last_starting_position = n + 1;
			token = std::string(lsp, n + 1 == tokens.end() ? n + 1 : n++);

			while (token.length() && token.find_last_of(sep) == token.length() - 1)
				token.erase(token.end() - 1);

			if (token.empty())
				++n;

			return n == tokens.end() ? false : true;
		}

		++n;
	}

	token = "";
	return false;
}

const std::string sepstream::GetRemaining()
{
	return std::string(n, tokens.end());
}

bool sepstream::StreamEnd()
{
	return n + 1 == tokens.end();
}
