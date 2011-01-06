/*
 *
 * (C) 2004-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

#ifndef SOCKETS_H
#define SOCKETS_H

#ifdef _WIN32
typedef SOCKET				ano_socket_t;
#define ano_sockread(fd, buf, len)	recv(fd, buf, len, 0)
#define ano_sockwrite(fd, buf, len)	 send(fd, buf, len, 0)
#define ano_sockclose(fd)		closesocket(fd)
#define ano_sockgeterr()		WSAGetLastError()
#define ano_sockseterr(err)		WSASetLastError(err)
/* ano_sockstrerror in sockutil.c */
/* ano_socksetnonb in sockutil.c */
#define ano_sockerrnonb(err)		(err == WSAEINPROGRESS || err == WSAEWOULDBLOCK)
#define SOCKERR_EBADF			WSAENOTSOCK
#define SOCKERR_EINTR			WSAEINTR
#define SOCKERR_EINVAL			WSAEINVAL
#define SOCKERR_EINPROGRESS		WSAEINPROGRESS
#else
typedef	int				ano_socket_t;
#define ano_sockread(fd, buf, len)	read(fd, buf, len)
#define ano_sockwrite(fd, buf, len) 	write(fd, buf, len)
#define ano_sockclose(fd)		close(fd)
#define ano_sockgeterr()		errno
#define ano_sockseterr(err)		errno = err
#define ano_sockstrerror(err)		strerror(err)
#define ano_socksetnonb(fd)		fcntl(fd, F_SETFL, O_NONBLOCK)
#define ano_sockerrnonb(err)		(err == EINPROGRESS)
#define SOCKERR_EBADF			EBADF
#define SOCKERR_EINTR			EINTR
#define SOCKERR_EINVAL			EINVAL
#define SOCKERR_EINPROGRESS		EINPROGRESS
#endif

#endif
