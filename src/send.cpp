/* Routines for sending stuff to the network.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"

/*************************************************************************/

/**
 * Send a command to the server.  The two forms here are like
 * printf()/vprintf() and friends.
 * @param source Orgin of the Message (some times NULL)
 * @param fmt Format of the Message
 * @param ... any number of parameters
 * @return void
 */
void send_cmd(const Anope::string &source, const char *fmt, ...)
{
	va_list args;
	static char buf[BUFSIZE];

	va_start(args, fmt);

	vsnprintf(buf, BUFSIZE - 1, fmt, args);

	if (!UplinkSock)
	{
		if (!source.empty())
			Alog(LOG_DEBUG) << "Attemtped to send \"" << source << " " << buf << "\" with UplinkSock NULL";
		else
			Alog(LOG_DEBUG) << "Attemtped to send \"" << buf << "\" with UplinkSock NULL";
		return;
	}

	if (!source.empty())
	{
		UplinkSock->Write(":%s %s", source.c_str(), buf);
		Alog(LOG_DEBUG) << "Sent: :" << source << " " << buf;
	}
	else
	{
		UplinkSock->Write("%s", buf);
		Alog(LOG_DEBUG) << "Sent: "<< buf;
	}

	va_end(args);
}

/*************************************************************************/

/**
 * Send a server notice
 * @param source Orgin of the Message
 * @param s Server Struct
 * @param fmt Format of the Message
 * @param ... any number of parameters
 * @return void
 */
void notice_server(const Anope::string &source, const Server *s, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";

	if (fmt)
	{
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);

		if (Config.NSDefFlags.HasFlag(NI_MSG))
			ircdproto->SendGlobalPrivmsg(findbot(source), s, buf);
		else
			ircdproto->SendGlobalNotice(findbot(source), s, buf);
		va_end(args);
	}
}

/*************************************************************************/

/**
 * Send a message in the user's selected language to the user using NOTICE.
 * @param source Orgin of the Message
 * @param u User Struct
 * @param int Index of the Message
 * @param ... any number of parameters
 * @return void
 */
void notice_lang(const Anope::string &source, User *dest, int message, ...)
{
	va_list args;
	char buf[4096]; /* because messages can be really big */
	char *s, *t;
	const char *fmt;

	if (!dest || !message)
		return;
	va_start(args, message);
	fmt = getstring(dest, message);

	if (!fmt)
		return;
	memset(buf, 0, 4096);
	vsnprintf(buf, sizeof(buf), fmt, args);
	s = buf;
	while (*s)
	{
		t = s;
		s += strcspn(s, "\n");
		if (*s)
			*s++ = 0;

		dest->SendMessage(source, "%s", *t ? t : " ");
	}
	va_end(args);
}

/*************************************************************************/

/**
 * Like notice_lang(), but replace %S by the source.  This is an ugly hack
 * to simplify letting help messages display the name of the pseudoclient
 * that's sending them.
 * @param source Orgin of the Message
 * @param u User Struct
 * @param int Index of the Message
 * @param ... any number of parameters
 * @return void
 */
void notice_help(const Anope::string &source, User * dest, int message, ...)
{
	va_list args;
	char buf[4096], buf2[4096], outbuf[BUFSIZE];
	char *s, *t;
	const char *fmt;

	if (!dest || !message)
		return;
	va_start(args, message);
	fmt = getstring(dest, message);
	if (!fmt)
		return;
	/* Some sprintf()'s eat %S or turn it into just S, so change all %S's
	 * into \1\1... we assume this doesn't occur anywhere else in the
	 * string. */
	strscpy(buf2, fmt, sizeof(buf2));
	strnrepl(buf2, sizeof(buf2), "%S", "\1\1");
	vsnprintf(buf, sizeof(buf), buf2, args);
	s = buf;
	while (*s)
	{
		t = s;
		s += strcspn(s, "\n");
		if (*s)
			*s++ = 0;
		strscpy(outbuf, t, sizeof(outbuf));
		strnrepl(outbuf, sizeof(outbuf), "\1\1", source.c_str());

		dest->SendMessage(source, "%s", *outbuf ? outbuf : " ");
	}
	va_end(args);
}
