/*
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

#ifndef SMTP_H
#define SMTP_H

/*************************************************************************/

/* Some Linux boxes (or maybe glibc includes) require this for the
 * prototype of strsignal(). */
#define _GNU_SOURCE

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <ctype.h>

#ifdef _AIX
extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *, const char *, size_t);
# if 0	/* These break on some AIX boxes (4.3.1 reported). */
extern int socket(int, int, int);
extern int connect(int, struct sockaddr *, int);
# endif
#endif /* _AIX */


/*************************************************************************/

#ifdef _WIN32
#include <winsock.h>
typedef SOCKET				ano_socket_t;
#define ano_sockclose(fd)		closesocket(fd)
#define ano_sockread(fd, buf, len)	recv(fd, buf, len, 0)
#define ano_sockwrite(fd, buf, len)	 send(fd, buf, len, 0)
#else
typedef	int				ano_socket_t;
#define ano_sockclose(fd)		close(fd)
#define ano_sockread(fd, buf, len)	read(fd, buf, len)
#define ano_sockwrite(fd, buf, len) 	write(fd, buf, len)
#define SOCKET_ERROR -1
#endif


/* Data structures */
struct smtp_header {
    char *header;
    struct smtp_header *next;
};

struct smtp_body_line {
    char *line;
    struct smtp_body_line *next;
};

struct smtp_message {
    struct smtp_header *smtp_headers, *smtp_headers_tail;
    struct smtp_body_line *smtp_body, *smtp_body_tail;
    char *from;
    char *to;
    ano_socket_t sock;
};

struct smtp_message mail;

/* set this to 1 if you want to get a log otherwise it runs silent */
int smtp_debug = 0;

#endif	/* SMTP_H */
