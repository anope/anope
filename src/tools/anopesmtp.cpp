/* smtp stuff handler for win32.
 *
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * Written by Dominick Meglio <codemastr@unrealircd.com>
 * *nix port by Trystan Scott Lee <trystan@nomadirc.net>
 */

#include "sysconf.h"

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
#include <iostream>
#include <fstream>

#ifndef _WIN32
# include <unistd.h>
# include <netdb.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <sys/time.h>
#else
# include <winsock.h>
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include <sys/types.h>

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
	std::vector<std::string> smtp_headers;
	std::vector<std::string> smtp_body;
	std::string from;
	std::string to;
	ano_socket_t sock;
};

int smtp_debug = 0;

struct smtp_message smail;

static std::string get_logname(struct tm *tm = NULL)
{
	char timestamp[32];

	if (!tm)
	{
		time_t t = time(NULL);
		tm = localtime(&t);
	}

	strftime(timestamp, sizeof(timestamp), "%Y%m%d", tm);
	std::string name = std::string("anopesmtp.") + timestamp;
	return name;
}

/* Log stuff to the log file with a datestamp.  Note that errno is
 * preserved by this routine and log_perror().
 */

void alog(const char *fmt, ...)
{
	if (!smtp_debug || !fmt)
		return;

	std::fstream file;
	file.open(get_logname().c_str(), std::ios_base::out | std::ios_base::app);

	if (!file.is_open())
		return;

	va_list args;
	va_start(args, fmt);

	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	char buf[256];
	strftime(buf, sizeof(buf) - 1, "[%b %d %H:%M:%S %Y] ", tm);
	file << buf;
	vsnprintf(buf, sizeof(buf), fmt, args);
	file << buf << std::endl;
	va_end(args);
	va_end(args);

	file.close();
}

/* Remove a trailing \r\n */
std::string strip(const std::string &buf)
{
	std::string newbuf = buf;
	char c = newbuf[newbuf.size() - 1];
	while (c == '\n' || c == '\r')
	{
		newbuf.erase(newbuf.end() - 1);
		c = newbuf[newbuf.size() - 1];
	}
	return newbuf;
}

/* Is the buffer a header? */
bool smtp_is_header(const std::string &buf)
{
	size_t tmp = buf.find(' ');

	if (tmp == std::string::npos)
		return false;

	if (tmp > 0 && buf[tmp - 1] == ':')
		return true;
	return false;
}

/* Parse a header into a name and value */
void smtp_parse_header(const std::string &buf, std::string &header, std::string &value)
{
	std::string newbuf = strip(buf);

	size_t space = newbuf.find(' ');
	if (space != std::string::npos)
	{
		header = newbuf.substr(0, space);
		value = newbuf.substr(space + 1);
	}
	else
	{
		header = newbuf;
		value = "";
	}
}

/* Have we reached the end of input? */
bool smtp_is_end(const std::string &buf)
{
	if (buf[0] == '.')
		if (buf[1] == '\r' || buf[1] == '\n')
			return true;

	return false;
}

/* Set who the email is to */
void smtp_set_to(const std::string &to)
{
	smail.to = to;
	size_t c = smail.to.rfind('<');
	if (c != std::string::npos && c + 1 < smail.to.size())
	{
		smail.to = smail.to.substr(c + 1);
		smail.to.erase(smail.to.end() - 1);
	}
}

/* Establish a connection to the SMTP server */
int smtp_connect(const char *host, unsigned short port)
{
	struct sockaddr_in addr;

	if ((smail.sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
		return 0;

	if ((addr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
	{
		struct hostent *hent;
		if (!(hent = gethostbyname(host)))
			return 0;
		memcpy(&addr.sin_addr, hent->h_addr, hent->h_length);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port ? port : 25);
	if (connect(smail.sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		ano_sockclose(smail.sock);
		return 0;
	}

	return 1;
}

/* Send a line of text */
int smtp_send(const char *text)
{
	int result = ano_sockwrite(smail.sock, text, strlen(text));

	alog("SMTP: sent %s",text);

	if (result == SOCKET_ERROR)
		ano_sockclose(smail.sock);

	return result;
}

/* Read a line of text */
int smtp_read(char *buf, int len)
{
	int result;

	memset(buf, 0, len);
	result = ano_sockread(smail.sock, buf, len);

	if (result == SOCKET_ERROR)
		ano_sockclose(smail.sock);

	return result;
}

/* Retrieve a response code */
int smtp_get_code(const std::string &text)
{
	size_t tmp = text.find(' ');

	if (tmp == std::string::npos)
		return 0;

	return atol(text.substr(0, tmp).c_str());
}

/* Send the email */
int smtp_send_email()
{
	char buf[1024];
	if (!smtp_read(buf, 1024))
	{
		alog("SMTP: error reading buffer");
		return 0;
	}

	int code = smtp_get_code(buf);
	if (code != 220)
	{
		alog("SMTP: error expected code 220 got %d",code);
		return 0;
	}

	if (!smtp_send("HELO anope\r\n"))
	{
		alog("SMTP: error writting to socket");
		return 0;
	}

	if (!smtp_read(buf, 1024))
	{
		alog("SMTP: error reading buffer");
		return 0;
	}

	code = smtp_get_code(buf);
	if (code != 250)
	{
		alog("SMTP: error expected code 250 got %d",code);
		return 0;
	}

	strcpy(buf, "MAIL FROM: <");
	strcat(buf, smail.from.c_str());
	strcat(buf, ">\r\n");

	if (!smtp_send(buf))
	{
		alog("SMTP: error writting to socket");
		return 0;
	}

	if (!smtp_read(buf, 1024))
	{
		alog("SMTP: error reading buffer");
		return 0;
	}

	code = smtp_get_code(buf);
	if (code != 250)
		return 0;

	strcpy(buf, "RCPT TO: <");
	strcat(buf, smail.to.c_str());
	strcat(buf, ">\r\n");

	if (!smtp_send(buf))
	{
		alog("SMTP: error writting to socket");
		return 0;
	}

	if (!smtp_read(buf, 1024))
	{
		alog("SMTP: error reading buffer");
		return 0;
	}

	code = smtp_get_code(buf);
	if (smtp_get_code(buf) != 250)
	{
		alog("SMTP: error expected code 250 got %d",code);
		return 0;
	}

	if (!smtp_send("DATA\r\n"))
	{
		alog("SMTP: error writting to socket");
		return 0;
	}

	if (!smtp_read(buf, 1024))
	{
		alog("SMTP: error reading buffer");
		return 0;
	}

	code = smtp_get_code(buf);
	if (code != 354)
	{
		alog("SMTP: error expected code 354 got %d",code);
		return 0;
	}

	for (std::vector<std::string>::const_iterator it = smail.smtp_headers.begin(), it_end = smail.smtp_headers.end(); it != it_end; ++it)
		if (!smtp_send(it->c_str()))
		{
			alog("SMTP: error writting to socket");
			return 0;
		}

	if (!smtp_send("\r\n"))
	{
		alog("SMTP: error writting to socket");
		return 0;
	}

	bool skip_done = false;
	for (std::vector<std::string>::const_iterator it = smail.smtp_body.begin(), it_end = smail.smtp_body.end(); it != it_end; ++it)
		if (skip_done)
		{
			if (!smtp_send(it->c_str()))
			{
				alog("SMTP: error writting to socket");
				return 0;
			}
		}
		else
			skip_done = true;

	if (!smtp_send("\r\n.\r\n"))
	{
		alog("SMTP: error writting to socket");
		return 0;
	}

	return 1;
}

void smtp_disconnect()
{
	smtp_send("QUIT\r\n");
	ano_sockclose(smail.sock);
}

int main(int argc, char *argv[])
{
	/* Win32 stuff */
#ifdef _WIN32
	WSADATA wsa;
#endif

	if (argc == 1)
		return 0;
	
	if (argc == 3 && !strcmp(argv[2], "--debug"))
		smtp_debug = 1;

	char *server = strtok(argv[1], ":"), *aport;
	short port;
	if ((aport = strtok(NULL, "")))
		port = atoi(aport);
	else
		port = 25;

	if (!server)
	{
		alog("No Server");
		/* Bad, bad, bad. This was a return from main with no value! -GD */
		return 0;
	}
	else
		alog("SMTP: server %s port %d",server,port);

	/* The WSAStartup function initiates use of WS2_32.DLL by a process. */
	/* guessing we can skip it under *nix */
#ifdef _WIN32
	if (WSAStartup(MAKEWORD(1, 1), &wsa))
		return 0;
#endif

	char buf[8192];
	bool headers_done = false;
	/* Read the message and parse it */
	while (fgets(buf, 8192, stdin))
	{
		if (smtp_is_header(buf) && !headers_done)
		{
			smail.smtp_headers.push_back(strip(buf) + "\r\n");
			std::string header, value;
			smtp_parse_header(buf, header, value);
			if (header == "From:")
			{
				alog("SMTP: from: %s", value.c_str());
				smail.from = value;
			}
			else if (header == "To:")
			{
				alog("SMTP: to: %s", value.c_str());
				smtp_set_to(value);
			}
			else if (smtp_is_end(buf))
				break;
			else
			{
				headers_done = true;
				smail.smtp_body.push_back(strip(buf) + "\r\n");
			}
		}
		else
			smail.smtp_body.push_back(strip(buf) + "\r\n");
	}

	if (!smtp_connect(server, port))
	{
		alog("SMTP: failed to connect to %s:%d", server, port);
		return 0;
	}
	if (!smtp_send_email())
	{
		alog("SMTP: error during sending of mail");
		return 0;
	}
	smtp_disconnect();

	return 1;
}
