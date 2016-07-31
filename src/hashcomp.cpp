/*
 * Anope IRC Services
 *
 * Copyright (C) 2002-2011 InspIRCd Development Team
 * Copyright (C) 2008-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "services.h"
#include "hashcomp.h"
#include "anope.h"
#include "config.h"

unsigned char Anope::tolower(unsigned char c)
{
	if (!Config || !Config->CaseMapLower[c])
		return std::tolower(c);

	return Config->CaseMapLower[c];
}

unsigned char Anope::toupper(unsigned char c)
{
	if (!Config || !Config->CaseMapUpper[c])
		return std::toupper(c);

	return Config->CaseMapUpper[c];
}

/*
 *
 * This is an implementation of a special string class, ci::string,
 * which is a case-insensitive equivalent to std::string.
 *
 */

bool ci::ci_char_traits::eq(char c1st, char c2nd)
{
	return Anope::toupper(c1st) == Anope::toupper(c2nd);
}

bool ci::ci_char_traits::ne(char c1st, char c2nd)
{
	return !eq(c1st, c2nd);
}

bool ci::ci_char_traits::lt(char c1st, char c2nd)
{
	return Anope::toupper(c1st) < Anope::toupper(c2nd);
}

int ci::ci_char_traits::compare(const char *str1, const char *str2, size_t n)
{
	for (unsigned i = 0; i < n; ++i)
	{
		unsigned char c1 = Anope::toupper(*str1),
				c2 = Anope::toupper(*str2);

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
	while (n-- > 0 && Anope::toupper(*s1) != Anope::toupper(c))
		++s1;
	return n >= 0 ? s1 : NULL;
}

bool ci::less::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return s1.ci_str().compare(s2.ci_str()) < 0;
}

sepstream::sepstream(const Anope::string &source, char seperator, bool ae) : tokens(source), sep(seperator), pos(0), allow_empty(ae)
{
}

bool sepstream::GetToken(Anope::string &token)
{
	if (this->StreamEnd())
	{
		token.clear();
		return false;
	}

	if (!this->allow_empty)
	{
		this->pos = this->tokens.find_first_not_of(this->sep, this->pos);
		if (this->pos == std::string::npos)
		{
			this->pos = this->tokens.length() + 1;
			token.clear();
			return false;
		}
	}

	size_t p = this->tokens.find(this->sep, this->pos);
	if (p == std::string::npos)
		p = this->tokens.length();

	token = this->tokens.substr(this->pos, p - this->pos);
	this->pos = p + 1;

	return true;
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
	return !this->StreamEnd() ? this->tokens.substr(this->pos) : "";
}

bool sepstream::StreamEnd()
{
	return this->pos > this->tokens.length();
}

