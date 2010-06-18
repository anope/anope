 /* POSIX emulation layer for Windows.
 *
 * Copyright (C) 2008 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008 Anope Team <info@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#ifdef WIN32
#include "services.h"

const char *dlerror()
{
	static char errbuf[513];
	DWORD err = GetLastError();
	if (!err)
		return NULL;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, 0, errbuf, 512, NULL);
	return errbuf;
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
 
	if (WSAStringToAddress((LPSTR) src, af, NULL, reinterpret_cast<LPSOCKADDR>(&sa), &address_length) == 0) 
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
 
	if (WSAAddressToString(reinterpret_cast<LPSOCKADDR>(&sa), address_length, NULL, dst, &string_length) == 0)
		return dst;
 
	return NULL;
}

#endif
