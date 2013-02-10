/*
 *
 * Copyright (C) 2002-2011 InspIRCd Development Team
 * Copyright (C) 2008-2013 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 */

#include "services.h"
#include "hashcomp.h"
#include "anope.h"

/* Case map in use by Anope */
std::locale Anope::casemap = std::locale(std::locale(), new Anope::ascii_ctype<char>());

/*
 *
 * This is an implementation of a special string class, ci::string,
 * which is a case-insensitive equivalent to std::string.
 *
 */
 
bool ci::ci_char_traits::eq(char c1st, char c2nd)
{
	const std::ctype<char> &ct = std::use_facet<std::ctype<char> >(Anope::casemap);
	return ct.toupper(c1st) == ct.toupper(c2nd);
}

bool ci::ci_char_traits::ne(char c1st, char c2nd)
{
	const std::ctype<char> &ct = std::use_facet<std::ctype<char> >(Anope::casemap);
	return ct.toupper(c1st) != ct.toupper(c2nd);
}

bool ci::ci_char_traits::lt(char c1st, char c2nd)
{
	const std::ctype<char> &ct = std::use_facet<std::ctype<char> >(Anope::casemap);
	return ct.toupper(c1st) < ct.toupper(c2nd);
}

int ci::ci_char_traits::compare(const char *str1, const char *str2, size_t n)
{
	const std::ctype<char> &ct = std::use_facet<std::ctype<char> >(Anope::casemap);

	for (unsigned i = 0; i < n; ++i)
	{
		register char c1 = ct.toupper(*str1), c2 = ct.toupper(*str2);

		if (c1 > c2)
			return 1;
		else if (c1 < c2)
			return -1;
		else if (!c1 || !c2)
			return 0;

		++str1;
		++str2;
	}
	return 0;
}

const char *ci::ci_char_traits::find(const char *s1, int n, char c)
{
	const std::ctype<char> &ct = std::use_facet<std::ctype<char> >(Anope::casemap);
	register char c_u = ct.toupper(c);
	while (n-- > 0 && ct.toupper(*s1) != c_u)
		++s1;
	return n >= 0 ? s1 : NULL;
}

bool ci::less::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return s1.ci_str().compare(s2.ci_str()) < 0;
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

bool sepstream::GetToken(Anope::string &token, int num)
{
	int i;
	for (i = 0; i < num + 1 && this->GetToken(token); ++i);
	return i == num + 1;
}

int sepstream::NumTokens()
{
	int i;
	Anope::string token;
	for (i = 0; this->GetToken(token); ++i);
	return i;
}

bool sepstream::GetTokenRemainder(Anope::string &token, int num)
{
	if (this->GetToken(token, num))
	{
		if (!this->StreamEnd())
			token += sep + this->GetRemaining();

		return true;
	}

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

