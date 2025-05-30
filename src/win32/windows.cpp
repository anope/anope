/* POSIX emulation layer for Windows.
 *
 * (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * (C) 2008-2025 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

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

int mkstemp(char *input)
{
	input = _mktemp(input);
	if (input == NULL)
	{
		errno = EEXIST;
		return -1;
	}

	int fd = open(input, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE);
	return fd;
}

#endif
