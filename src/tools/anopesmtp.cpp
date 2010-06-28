/* smtp stuff handler for win32.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * Written by Dominick Meglio <codemastr@unrealircd.com>
 * *nix port by Trystan Scott Lee <trystan@nomadirc.net>
 */

#include "smtp.h"

static FILE *logfile;
static int curday = 0;

/*************************************************************************/

static int get_logname(std::string &name, struct tm *tm = NULL)
{
	char timestamp[32];

	if (!tm)
	{
		time_t t = time(NULL);
		tm = localtime(&t);
	}

	strftime(timestamp, sizeof(timestamp), "%Y%m%d", tm);
	name = std::string("logs/anopesmtp.") + timestamp;
	curday = tm->tm_yday;

	return 1;
}

/*************************************************************************/

/* Close the log file. */

void close_log()
{
	if (!logfile)
		return;
	fclose(logfile);
	logfile = NULL;
}

/*************************************************************************/

static void remove_log()
{
	time_t t = time(NULL);
	t -= 2592000; // 30 days ago
	struct tm *tm = localtime(&t);

	std::string name;
	if (!get_logname(name, tm))
		return;
	unlink(name.c_str());
}

/*************************************************************************/

/* Open the log file.  Return -1 if the log file could not be opened, else
 * return 0. */

int open_log()
{
	if (logfile)
		return 0;

	std::string name;
	if (!get_logname(name))
		return 0;
	logfile = fopen(name.c_str(), "w");
	return logfile ? 0 : -1;
}

/*************************************************************************/

static void checkday()
{
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	if (curday != tm->tm_yday)
	{
		close_log();
		remove_log();
		open_log();
	}
}

/*************************************************************************/

/* Log stuff to the log file with a datestamp.  Note that errno is
 * preserved by this routine and log_perror().
 */

void alog(const char *fmt, ...)
{
	int errno_save = errno;

	if (!smtp_debug)
		return;

	checkday();

	if (!fmt)
		return;

	va_list args;
	va_start(args, fmt);

	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	char buf[256];
	strftime(buf, sizeof(buf) - 1, "[%b %d %H:%M:%S %Y] ", tm);
	if (logfile)
	{
		fputs(buf, logfile);
		vfprintf(logfile, fmt, args);
		fputc('\n', logfile);
	}
	va_end(args);
	errno = errno_save;
}

/*************************************************************************/

/* Remove a trailing \r\n */
ci::string strip(const ci::string &buf)
{
	ci::string newbuf = buf;
	char c = newbuf[newbuf.size() - 1];
	while (c == '\n' || c == '\r')
	{
		newbuf.erase(newbuf.end() - 1);
		c = newbuf[newbuf.size() - 1];
	}
	return newbuf;
}

/*************************************************************************/

/* Is the buffer a header? */
bool smtp_is_header(const ci::string &buf)
{
	size_t tmp = buf.find(' ');

	if (tmp == ci::string::npos)
		return false;

	if (buf[tmp + 1] == ':')
		return true;
	return false;
}

/*************************************************************************/

/* Parse a header into a name and value */
void smtp_parse_header(const ci::string &buf, ci::string &header, ci::string &value)
{
	ci::string newbuf = strip(buf);

	size_t space = newbuf.find(' ');
	if (space != ci::string::npos)
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

/*************************************************************************/

/* Have we reached the end of input? */
bool smtp_is_end(const ci::string &buf)
{
	if (buf[0] == '.')
		if (buf[1] == '\r' || buf[1] == '\n')
			return true;

	return false;
}

/*************************************************************************/

/* Set who the email is to */
void smtp_set_to(const ci::string &to)
{
	mail.to = to;
	size_t c = mail.to.rfind('<');
	if (c != ci::string::npos && c + 1 < mail.to.size())
	{
		mail.to = mail.to.substr(c + 1);
		mail.to.erase(mail.to.end() - 1);
	}
}

/*************************************************************************/

/* Establish a connection to the SMTP server */
int smtp_connect(const char *host, unsigned short port)
{
	struct sockaddr_in addr;

	if ((mail.sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
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
	if (connect(mail.sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		ano_sockclose(mail.sock);
		return 0;
	}

	return 1;
}

/*************************************************************************/

/* Send a line of text */
int smtp_send(const char *text)
{
	int result = ano_sockwrite(mail.sock, text, strlen(text));

	alog("SMTP: sent %s",text);

	if (result == SOCKET_ERROR)
		ano_sockclose(mail.sock);

	return result;
}

/*************************************************************************/

/* Read a line of text */
int smtp_read(char *buf, int len)
{
	int result;

	memset(buf, 0, len);
	result = ano_sockread(mail.sock, buf, len);

	if (result == SOCKET_ERROR)
		ano_sockclose(mail.sock);

	return result;
}

/*************************************************************************/

/* Retrieve a response code */
int smtp_get_code(const std::string &text)
{
	size_t tmp = text.find(' ');

	if (tmp == ci::string::npos)
		return 0;

	return atol(text.c_str());
}

/*************************************************************************/

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
	strcat(buf, mail.from.c_str());
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
	strcat(buf, mail.to.c_str());
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

	for (std::vector<ci::string>::const_iterator it = mail.smtp_headers.begin(), it_end = mail.smtp_headers.end(); it != it_end; ++it)
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
	for (std::vector<ci::string>::const_iterator it = mail.smtp_body.begin(), it_end = mail.smtp_body.end(); it != it_end; ++it)
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

/*************************************************************************/

void smtp_disconnect()
{
	smtp_send("QUIT\r\n");
	ano_sockclose(mail.sock);
}

/*************************************************************************/

void mail_cleanup()
{
	mail.from.clear();
	mail.to.clear();

	mail.smtp_headers.clear();

	mail.smtp_body.clear();
}

/*************************************************************************/

int main(int argc, char *argv[])
{
	/* Win32 stuff */
#ifdef _WIN32
	WSADATA wsa;
#endif

	if (argc == 1)
		return 0;

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

	memset(&mail, 0, sizeof(mail));

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
			mail.smtp_headers.push_back(strip(buf) + "\r\n");
			ci::string header, value;
			smtp_parse_header(buf, header, value);
			if (header == "from")
			{
				alog("SMTP: from: %s", value.c_str());
				mail.from = value;
			}
			else if (header == "to")
			{
				alog("SMTP: to: %s", value.c_str());
				smtp_set_to(value);
			}
			else if (smtp_is_end(buf))
				break;
			else
			{
				headers_done = true;
				mail.smtp_body.push_back(strip(buf) + "\r\n");
			}
		}
		else
			mail.smtp_body.push_back(strip(buf) + "\r\n");
	}

	if (!smtp_connect(server, port))
	{
		alog("SMTP: failed to connect to %s:%d", server, port);
		mail_cleanup();
		return 0;
	}
	if (!smtp_send_email())
	{
		alog("SMTP: error during sending of mail");
		mail_cleanup();
		return 0;
	}
	smtp_disconnect();
	mail_cleanup();

	return 1;
}
