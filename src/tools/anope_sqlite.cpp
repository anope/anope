/*
 * Anope IRC Services
 *
 * Copyright (C) 2016 Anope Team <team@anope.org>
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

#include <sqlite3ext.h>

#include <algorithm>
#include <string>

#ifdef _WIN32
# define DllExport __declspec(dllexport)
#else
# define DllExport
#endif

SQLITE_EXTENSION_INIT1

namespace
{

enum collation
{
	ANOPE_ASCII,
	ANOPE_RFC1459,
	ANOPE_RFC1459_INSPIRCD
};

char (*anope_tolower)(char) = nullptr;

char tolower_ascii(char c)
{
	if (c >= 'A' && c <= 'Z')
		return c + 0x20;
	else
		return c;
}

char tolower_rfc1459_inspircd(char c)
{
	if (c == '[' || c == ']' || c == '\\')
		return c + 0x20;
	else
		return tolower_ascii(c);
}

char tolower_rfc1459(char c)
{
	if (c == '[' || c == ']' || c == '\\' || c == '^')
		return c + 0x20;
	else
		return tolower_ascii(c);
}

void anope_canonicalize(sqlite3_context *context, int argc, sqlite3_value **argv)
{
	sqlite3_value *arg = argv[0];
	const unsigned char *text = sqlite3_value_text(arg);

	if (text == nullptr || anope_tolower == nullptr)
		return;

	std::string result(reinterpret_cast<const char *>(text));
	std::transform(result.begin(), result.end(), result.begin(), anope_tolower);

	sqlite3_result_text(context, result.c_str(), -1, SQLITE_TRANSIENT);
}

int sqlite_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi, collation col)
{
	SQLITE_EXTENSION_INIT2(pApi);

	switch (col)
	{
		case ANOPE_ASCII:
			anope_tolower = tolower_ascii;
			break;
		case ANOPE_RFC1459:
			anope_tolower = tolower_rfc1459;
			break;
		case ANOPE_RFC1459_INSPIRCD:
			anope_tolower = tolower_rfc1459_inspircd;
			break;
		default:
			return SQLITE_INTERNAL;
	}

	int rc = sqlite3_create_function_v2(db, "anope_canonicalize", 1, SQLITE_DETERMINISTIC, NULL, anope_canonicalize, NULL, NULL, NULL);

	return rc;
}

} // namespace

extern "C" DllExport
int sqlite3_anopesqliteascii_init(sqlite3 *, char **, const sqlite3_api_routines *);

extern "C" DllExport
int sqlite3_anopesqliterfc1459_init(sqlite3 *, char **, const sqlite3_api_routines *);

extern "C" DllExport
int sqlite3_anopesqliterfc1459inspircd_init(sqlite3 *, char **, const sqlite3_api_routines *);

int sqlite3_anopesqliteascii_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
{
	return sqlite_init(db, pzErrMsg, pApi, ANOPE_ASCII);
}

int sqlite3_anopesqliterfc1459_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
{
	return sqlite_init(db, pzErrMsg, pApi, ANOPE_RFC1459);
}

int sqlite3_anopesqliterfc1459inspircd_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
{
	return sqlite_init(db, pzErrMsg, pApi, ANOPE_RFC1459_INSPIRCD);
}

