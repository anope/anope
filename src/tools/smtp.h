/*
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef SMTP_H
#define SMTP_H

#include "sysconf.h"
#define CoreExport
#include "hashcomp.h"

/*************************************************************************/

/* Some Linux boxes (or maybe glibc includes) require this for the
 * prototype of strsignal(). */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <string>
#include <vector>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cerrno>

/* Windows does not have:
 * unistd.h, grp.h,
 * netdb.h, netinet/in.h,
 * sys/socket.h, sys/time.h
 * Windows requires:
 * winsock.h
 * -- codemastr
 */

#ifndef _WIN32
# include <unistd.h>
#endif

#ifndef _WIN32
# include <netdb.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <arpa/inet.h>
#else
# include <winsock.h>
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include <sys/types.h>

#ifndef _WIN32
# include <sys/time.h>
#endif

#ifdef _AIX
extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *, const char *, size_t);
# if 0 /* These break on some AIX boxes (4.3.1 reported). */
extern int socket(int, int, int);
extern int connect(int, struct sockaddr *, int);
# endif
#endif /* _AIX */

/* Some SUN fixs */
#ifdef __sun
/* Solaris specific code, types that do not exist in Solaris'
 *  * sys/types.h
 *   **/
# ifndef INADDR_NONE
#  define INADDR_NONE (-1)
# endif
#endif

/*************************************************************************/

#ifdef _WIN32
typedef SOCKET ano_socket_t;
#define ano_sockclose(fd) closesocket(fd)
#define ano_sockread(fd, buf, len) recv(fd, buf, len, 0)
#define ano_sockwrite(fd, buf, len) send(fd, buf, len, 0)
#else
typedef int ano_socket_t;
#define ano_sockclose(fd) close(fd)
#define ano_sockread(fd, buf, len) read(fd, buf, len)
#define ano_sockwrite(fd, buf, len) write(fd, buf, len)
#define SOCKET_ERROR -1
#endif

/* Data structures */
struct smtp_message
{
	std::vector<ci::string> smtp_headers;
	std::vector<ci::string> smtp_body;
	ci::string from, to;
	ano_socket_t sock;
};

struct smtp_message mail;

/* set this to 1 if you want to get a log otherwise it runs silent */
int smtp_debug = 0;

#endif /* SMTP_H */
