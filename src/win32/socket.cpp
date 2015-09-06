 /* POSIX emulation layer for Windows.
 *
 * Copyright (C) 2008-2014 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#define WIN32_NO_OVERRIDE
#include "services.h"

inline static bool is_socket(int fd)
{
	int optval;
	socklen_t optlen = sizeof(optval);
	return getsockopt(fd, SOL_SOCKET, SO_TYPE, reinterpret_cast<char *>(&optval), &optlen) == 0;
}

int read(int fd, char *buf, size_t count)
{
	if (is_socket(fd))
		return recv(fd, buf, count, 0);
	else
		return _read(fd, buf, count);
}

int write(int fd, const char *buf, size_t count)
{
	if (is_socket(fd))
		return send(fd, buf, count, 0);
	else
		return _write(fd, buf, count);
}

int windows_close(int fd)
{
	if (is_socket(fd))
		return closesocket(fd);
	else
		return close(fd);
}

int windows_accept(int fd, struct sockaddr *addr, int *addrlen)
{
	int i = accept(fd, addr, addrlen);
	if (i == INVALID_SOCKET)
		return -1;
	return i;
}

/** This is inet_pton, but it works on Windows
 * @param af The protocol type, AF_INET or AF_INET6
 * @param src The address
 * @param dst Struct to put results in
 * @return 1 on success, -1 on error
 */
int windows_inet_pton(int af, const char *src, void *dst)
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
const char *windows_inet_ntop(int af, const void *src, char *dst, size_t size)
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

int fcntl(int fd, int cmd, int arg)
{
	if (cmd == F_GETFL)
	{
		return 0;
	}
	else if (cmd == F_SETFL)
	{
		if (arg & O_NONBLOCK)
		{
			unsigned long opt = 1;
			return ioctlsocket(fd, FIONBIO, &opt);
		}
		else
		{
			unsigned long opt = 0;
			return ioctlsocket(fd, FIONBIO, &opt);
		}
	}

	return -1;
}
