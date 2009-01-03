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

#include "hashcomp.h"

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
			token = std::string(lsp, n + 1 == tokens.end() ? n + 1 : ++n);

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
