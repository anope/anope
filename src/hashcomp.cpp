/*
 * Anope IRC Services
 *
 * Copyright (C) 2002-2011 InspIRCd Development Team
 * Copyright (C) 2008-2017 Anope Team <team@anope.org>
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

#ifdef Boost_FOUND
#include <boost/locale.hpp>
#endif

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

size_t Anope::hash::operator()(const Anope::string &s) const
{
	return std::hash<std::string>()(s.str());
}

bool Anope::compare::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return s1.equals_cs(s2);
}

size_t Anope::hash_ci::operator()(const Anope::string &s) const
{
	return std::hash<std::string>()(s.lower().str());
}

bool Anope::compare_ci::operator()(const Anope::string &s1, const Anope::string &s2) const
{
	return s1.equals_ci(s2);
}

size_t Anope::hash_locale::operator()(const Anope::string &s) const
{
#ifdef Boost_FOUND
	if (Config != nullptr && Config->locale != nullptr)
	{
		return Anope::locale::hash(s.str());
	}
#endif

	return Anope::hash_ci()(s);
}

bool Anope::compare_locale::operator()(const Anope::string &s1, const Anope::string &s2) const
{
#ifdef Boost_FOUND
	if (Config != nullptr && Config->locale != nullptr)
	{
		return Anope::locale::compare(s1.str(), s2.str()) == 0;
	}
#endif

	return Anope::compare_ci()(s1, s2);
}

Anope::string Anope::transform(const Anope::string &s)
{
#ifdef Boost_FOUND
	if (Config != nullptr && Config->locale != nullptr)
	{
		return Anope::locale::transform(s.str());
	}
#endif

	return s.lower();
}

#ifdef Boost_FOUND

std::locale Anope::locale::generate(const std::string &name)
{
	boost::locale::generator gen;
	return gen.generate(name);
}

int Anope::locale::compare(const std::string &s1, const std::string &s2)
{
	std::string c1 = boost::locale::conv::between(s1.data(), s1.data() + s1.length(),
		Config->locale->name(),
		"");

	std::string c2 = boost::locale::conv::between(s2.data(), s2.data() + s2.length(),
		Config->locale->name(),
		"");

	const boost::locale::collator<char> &ct = std::use_facet<boost::locale::collator<char>>(*Config->locale);
	return ct.compare(boost::locale::collator_base::secondary, c1, c2);
}

long Anope::locale::hash(const std::string &s)
{
	const boost::locale::collator<char> &ct = std::use_facet<boost::locale::collator<char>>(*Config->locale);
	return ct.hash(boost::locale::collator_base::secondary, s.data(), s.data() + s.length());
}

std::string Anope::locale::transform(const std::string &s)
{
	const boost::locale::collator<char> &ct = std::use_facet<boost::locale::collator<char>>(*Config->locale);
	return ct.transform(boost::locale::collator_base::secondary, s);
}

#endif

