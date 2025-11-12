// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
// Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#ifdef _WIN32
#include "services.h"
#include "anope.h"

#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

static struct WindowsLanguage final
{
	Anope::string languageName;
	USHORT windowsLanguageName;
} WindowsLanguages[] = {
	{"de_DE", LANG_GERMAN},
	{"el_GR", LANG_GREEK},
	{"en_US", LANG_ENGLISH},
	{"es_ES", LANG_SPANISH},
	{"fr_FR", LANG_FRENCH},
	{"it_IT", LANG_ITALIAN},
	{"nl_NL", LANG_DUTCH},
	{"pl_PL", LANG_POLISH},
	{"pt_PT", LANG_PORTUGUESE},
	{"tr_TR", LANG_TURKISH},
};

static WSADATA wsa;

void OnStartup()
{
	if (WSAStartup(MAKEWORD(2, 0), &wsa))
		throw CoreException("Failed to initialize WinSock library");
}

void OnShutdown()
{
	WSACleanup();
}

USHORT WindowsGetLanguage(const Anope::string &lang)
{
	for (int i = 0; i < sizeof(WindowsLanguages) / sizeof(WindowsLanguage); ++i)
	{
		WindowsLanguage &l = WindowsLanguages[i];

		if (lang == l.languageName || !lang.find(l.languageName + "."))
			return l.windowsLanguageName;
	}

	return LANG_NEUTRAL;
}

int setenv(const char *name, const char *value, int overwrite)
{
	return SetEnvironmentVariable(name, value);
}

int unsetenv(const char *name)
{
	return SetEnvironmentVariable(name, NULL);
}

#endif
