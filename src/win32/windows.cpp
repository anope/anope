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
int gettimeofday(timeval *tv, char *)
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	
	tv->tv_sec = Anope::CurTime;
	tv->tv_usec = st.wMilliseconds;
	
	return 0;
}

#endif
