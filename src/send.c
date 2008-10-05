/* Routines for sending stuff to the network.
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
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
void send_cmd(const char *source, const char *fmt, ...)
{
	va_list args;
	static char buf[BUFSIZE];

	va_start(args, fmt);

	vsnprintf(buf, BUFSIZE - 1, fmt, args);

	if (source)
	{
		sockprintf(servsock, ":%s %s\r\n", source, buf);
		eventprintf(":%s %s", source, buf);
		if (debug)
			alog("debug: Sent: :%s %s", source, buf);
	}
	else
	{
		sockprintf(servsock, "%s\r\n", buf);
		eventprintf("%s", buf);
		if (debug)
			alog("debug: Sent: %s", buf);
	}

	va_end(args);
}


/*
 * Copypasta version that accepts std::string source.
 */
void send_cmd(const std::string &source, const char *fmt, ...)
{
	va_list args;
	static char buf[BUFSIZE];

	va_start(args, fmt);

	vsnprintf(buf, BUFSIZE - 1, fmt, args);

	if (!source.empty())
	{
		sockprintf(servsock, ":%s %s\r\n", source.c_str(), buf);
		eventprintf(":%s %s", source.c_str(), buf);
		if (debug)
			alog("debug: Sent: :%s %s", source.c_str(), buf);
	}
	else
	{
		sockprintf(servsock, "%s\r\n", buf);
		eventprintf("%s", buf);
		if (debug)
			alog("debug: Sent: %s", buf);
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
void notice_server(char *source, Server * s, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);

        if (NSDefFlags & NI_MSG) {
            ircdproto->SendGlobalPrivmsg(findbot(source), s->name, buf);
        } else {
            ircdproto->SendGlobalNotice(findbot(source), s->name, buf);
        }
        va_end(args);
    }
}

/*************************************************************************/

/**
 * Send a notice to a user
 * @param source Orgin of the Message
 * @param u User Struct
 * @param fmt Format of the Message
 * @param ... any number of parameters
 * @return void
 */
void notice_user(char *source, User * u, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);

		u->SendMessage(source, buf);

        va_end(args);
    }
}

/*************************************************************************/

/**
 * Send a NULL-terminated array of text as NOTICEs.
 * @param source Orgin of the Message
 * @param dest Destination of the Notice
 * @param text Array of text to send
 * @return void
 */
void notice_list(char *source, char *dest, char **text)
{
    while (*text) {
        /* Have to kludge around an ircII bug here: if a notice includes
         * no text, it is ignored, so we replace blank lines by lines
         * with a single space.
         */
        if (**text) {
            ircdproto->SendNotice(findbot(source), dest, *text);
        } else {
            ircdproto->SendNotice(findbot(source), dest, " ");
        }
        text++;
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
void notice_lang(const char *source, User * dest, int message, ...)
{
    va_list args;
    char buf[4096];             /* because messages can be really big */
    char *s, *t;
    const char *fmt;

    if (!dest || !message) {
        return;
    }
    va_start(args, message);
    fmt = getstring(dest->na, message);

    if (!fmt)
        return;
    memset(buf, 0, 4096);
    vsnprintf(buf, sizeof(buf), fmt, args);
    s = buf;
    while (*s) {
        t = s;
        s += strcspn(s, "\n");
        if (*s)
            *s++ = 0;
        /* Send privmsg instead of notice if:
         * - UsePrivmsg is enabled
         * - The user is not registered and NSDefMsg is enabled
         * - The user is registered and has set /ns set msg on
         */
        if (UsePrivmsg && ((!dest->na && (NSDefFlags & NI_MSG))
                           || (dest->na
                               && (dest->na->nc->flags & NI_MSG)))) {
            ircdproto->SendPrivmsg(findbot(source), dest->nick, *t ? t : " ");
        } else {
            ircdproto->SendNotice(findbot(source), dest->nick, *t ? t : " ");
        }
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
void notice_help(const char *source, User * dest, int message, ...)
{
    va_list args;
    char buf[4096], buf2[4096], outbuf[BUFSIZE];
    char *s, *t;
    const char *fmt;

    if (!dest || !message) {
        return;
    }
    va_start(args, message);
    fmt = getstring(dest->na, message);
    if (!fmt)
        return;
    /* Some sprintf()'s eat %S or turn it into just S, so change all %S's
     * into \1\1... we assume this doesn't occur anywhere else in the
     * string. */
    strscpy(buf2, fmt, sizeof(buf2));
    strnrepl(buf2, sizeof(buf2), "%S", "\1\1");
    vsnprintf(buf, sizeof(buf), buf2, args);
    s = buf;
    while (*s) {
        t = s;
        s += strcspn(s, "\n");
        if (*s)
            *s++ = 0;
        strscpy(outbuf, t, sizeof(outbuf));
        strnrepl(outbuf, sizeof(outbuf), "\1\1", source);
        /* Send privmsg instead of notice if:
         * - UsePrivmsg is enabled
         * - The user is not registered and NSDefMsg is enabled
         * - The user is registered and has set /ns set msg on
         */
        if (UsePrivmsg && ((!dest->na && (NSDefFlags & NI_MSG))
                           || (dest->na
                               && (dest->na->nc->flags & NI_MSG)))) {
            ircdproto->SendPrivmsg(findbot(source), dest->nick, *outbuf ? outbuf : " ");
        } else {
            ircdproto->SendNotice(findbot(source), dest->nick, *outbuf ? outbuf : " ");
        }
    }
    va_end(args);
}
