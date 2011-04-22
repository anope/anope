 /* POSIX emulation layer for Windows.
 *
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2011 Anope Team <info@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifdef _WIN32
#include "services.h"

struct WindowsLanguage
{
	const char *languageName;
	USHORT windowsLanguageName;
};

WindowsLanguage WindowsLanguages[] = {
	{"ca_ES", LANG_CATALAN},
	{"de_DE", LANG_GERMAN},
	{"el_GR", LANG_GREEK},
	{"es_ES", LANG_SPANISH},
	{"fr_FR", LANG_FRENCH},
	{"hu_HU", LANG_HUNGARIAN},
	{"it_IT", LANG_ITALIAN},
	{"nl_NL", LANG_DUTCH},
	{"pl_PL", LANG_POLISH},
	{"pt_PT", LANG_PORTUGUESE},
	{"ru_RU", LANG_RUSSIAN},
	{"tr_TR", LANG_TURKISH},
	{NULL, 0}
};

USHORT WindowsGetLanguage(const char *lang)
{
	for (int i = 0; WindowsLanguages[i].languageName; ++i)
		if (!strcmp(lang, WindowsLanguages[i].languageName))
			return WindowsLanguages[i].windowsLanguageName;
	return LANG_NEUTRAL;
}

/** This is inet_pton, but it works on Windows
 * @param af The protocol type, AF_INET or AF_INET6
 * @param src The address
 * @param dst Struct to put results in
 * @return 1 on sucess, -1 on error
 */
int inet_pton(int af, const char *src, void *dst)
{
	int address_length;
	sockaddr_storage sa;
	sockaddr_in *sin = reinterpret_cast<sockaddr_in *>(&sa);
	sockaddr_in6 *sin6 = reinterpret_cast<sockaddr_in6 *>(&sa);

	switch (af)
	{
		case AF_INET:
			address_length = sizeof(sockaddr_in);
			break;
		case AF_INET6:
			address_length = sizeof(sockaddr_in6);
			break;
		default:
			return -1;
	}

	if (!WSAStringToAddress(static_cast<LPSTR>(const_cast<char *>(src)), af, NULL, reinterpret_cast<LPSOCKADDR>(&sa), &address_length))
	{
		switch (af)
		{
			case AF_INET:
				memcpy(dst, &sin->sin_addr, sizeof(in_addr));
				break;
			case AF_INET6:
				memcpy(dst, &sin6->sin6_addr, sizeof(in6_addr));
				break;
		}
		return 1;
	}

	return 0;
}

/** This is inet_ntop, but it works on Windows
 * @param af The protocol type, AF_INET or AF_INET6
 * @param src Network address structure
 * @param dst After converting put it here
 * @param size sizeof the dest
 * @return dst
 */
const char *inet_ntop(int af, const void *src, char *dst, size_t size)
{
	int address_length;
	DWORD string_length = size;
	sockaddr_storage sa;
	sockaddr_in *sin = reinterpret_cast<sockaddr_in *>(&sa);
	sockaddr_in6 *sin6 = reinterpret_cast<sockaddr_in6 *>(&sa);

	memset(&sa, 0, sizeof(sa));

	switch (af)
	{
		case AF_INET:
			address_length = sizeof(sockaddr_in);
			sin->sin_family = af;
			memcpy(&sin->sin_addr, src, sizeof(in_addr));
			break;
		case AF_INET6:
			address_length = sizeof(sockaddr_in6);
			sin6->sin6_family = af;
			memcpy(&sin6->sin6_addr, src, sizeof(in6_addr));
			break;
		default:
			return NULL;
	}

	if (!WSAAddressToString(reinterpret_cast<LPSOCKADDR>(&sa), address_length, NULL, dst, &string_length))
		return dst;

	return NULL;
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
				buf = "Microsoft Windows Millenium Edition";
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

#endif
