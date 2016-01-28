/* POSIX emulation layer for Windows.
 *
 * (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * (C) 2008-2016 Anope Team <team@anope.org>
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

static struct WindowsLanguage
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

Anope::string GetWindowsVersion()
{
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	BOOL bOsVersionInfoEx = GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osvi));
	if (!bOsVersionInfoEx)
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osvi)))
			return "";
	}
	GetSystemInfo(&si);

	Anope::string buf, extra, cputype;
	/* Determine CPU type 32 or 64 */
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		cputype = " 64-bit";
	else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
		cputype = " 32-bit";
	else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		cputype = " Itanium 64-bit";

	switch (osvi.dwPlatformId)
	{
		/* test for the Windows NT product family. */
		case VER_PLATFORM_WIN32_NT:
			/* Windows Vista or Windows Server 2008 */
			if (osvi.dwMajorVersion == 6 && !osvi.dwMinorVersion)
			{
				if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					extra = " Enterprise Edition";
				else if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					extra = " Datacenter Edition";
				else if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
					extra = " Home Premium/Basic";
				if (osvi.dwMinorVersion == 0)
				{
					if (osvi.wProductType & VER_NT_WORKSTATION)
						buf = "Microsoft Windows Vista" + cputype + extra;
					else
						buf = "Microsoft Windows Server 2008" + cputype + extra;
				}
				else if (osvi.dwMinorVersion == 1)
				{
					if (osvi.wProductType & VER_NT_WORKSTATION)
						buf = "Microsoft Windows 7" + cputype + extra;
					else
						buf = "Microsoft Windows Server 2008 R2" + cputype + extra;
				}
			}
			/* Windows 2003 or Windows XP Pro 64 */
			if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
			{
				if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					extra = " Datacenter Edition";
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					extra = " Enterprise Edition";
#ifdef VER_SUITE_COMPUTE_SERVER
				else if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
					extra = " Compute Cluster Edition";
#endif
				else if (osvi.wSuiteMask == VER_SUITE_BLADE)
					extra = " Web Edition";
				else
					extra = " Standard Edition";
				if (osvi.wProductType & VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					buf = "Microsoft Windows XP Professional x64 Edition" + extra;
				else
					buf = "Microsoft Windows Server 2003 Family" + cputype + extra;
			}
			if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
			{
				if (osvi.wSuiteMask & VER_SUITE_EMBEDDEDNT)
					extra = " Embedded";
				else if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
					extra = " Home Edition";
				buf = "Microsoft Windows XP" + extra;
			}
			if (osvi.dwMajorVersion == 5 && !osvi.dwMinorVersion)
			{
				if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					extra = " Datacenter Server";
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					extra = " Advanced Server";
				else
					extra = " Server";
				buf = "Microsoft Windows 2000" + extra;
			}
			if (osvi.dwMajorVersion <= 4)
			{
				if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					extra = " Enterprise Edition";
				buf = "Microsoft Windows NT Server 4.0" + extra;
			}
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			if (osvi.dwMajorVersion == 4 && !osvi.dwMinorVersion)
			{
				if (osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B')
					extra = " OSR2";
				buf = "Microsoft Windows 95" + extra;
			}
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
			{
				if (osvi.szCSDVersion[1] == 'A')
					extra = "SE";
				buf = "Microsoft Windows 98" + extra;
			}
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
				buf = "Microsoft Windows Millennium Edition";
	}
	return buf;
}

bool SupportedWindowsVersion()
{
	OSVERSIONINFOEX osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	BOOL bOsVersionInfoEx = GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osvi));
	if (!bOsVersionInfoEx)
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osvi)))
			return false;
	}

	switch (osvi.dwPlatformId)
	{
		/* test for the Windows NT product family. */
		case VER_PLATFORM_WIN32_NT:
			/* win nt4 */
			if (osvi.dwMajorVersion <= 4)
				return false;
			/* the rest */
			return true;
		/* win95 win98 winME */
		case VER_PLATFORM_WIN32_WINDOWS:
			return false;
	}
	return false;
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
