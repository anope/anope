/* POSIX emulation layer for Windows.
 *
 * (C) 2008-2024 Anope Team
 * Contact us at team@anope.org
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
