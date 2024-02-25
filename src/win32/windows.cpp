/* POSIX emulation layer for Windows.
 *
 * (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * (C) 2008-2024 Anope Team <team@anope.org>
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
	{"ca_ES", LANG_CATALAN},
	{"de_DE", LANG_GERMAN},
	{"el_GR", LANG_GREEK},
	{"en_US", LANG_ENGLISH},
	{"es_ES", LANG_SPANISH},
	{"fr_FR", LANG_FRENCH},
	{"hu_HU", LANG_HUNGARIAN},
	{"it_IT", LANG_ITALIAN},
	{"nl_NL", LANG_DUTCH},
	{"pl_PL", LANG_POLISH},
	{"pt_PT", LANG_PORTUGUESE},
	{"ru_RU", LANG_RUSSIAN},
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

/** Like gettimeofday(), but it works on Windows.
 * @param tv A timeval struct
 * @param tz Should be NULL, it is not used
 * @return 0 on success
 */
int gettimeofday(timeval *tv, void *)
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	tv->tv_sec = Anope::CurTime;
	tv->tv_usec = st.wMilliseconds;

	return 0;
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

void getcwd(char *buf, size_t sz)
{
	GetCurrentDirectory(sz, buf);
}

#endif
