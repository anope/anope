/* Routines for sending stuff to the network.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
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

    if (fmt) {
        va_start(args, fmt);
        vsend_cmd(source, fmt, args);
        va_end(args);
    }
}

/*************************************************************************/

/**
 * actually Send a command to the server.
 * @param source Orgin of the Message (some times NULL)
 * @param fmt Format of the Message
 * @param args List of the arguments
 * @return void
 */
void vsend_cmd(const char *source, const char *fmt, va_list args)
{
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        vsnprintf(buf, BUFSIZE - 1, fmt, args);

        if (source) {
            sockprintf(servsock, ":%s %s\r\n", source, buf);
            eventprintf(":%s %s", source, buf);
            if (debug) {
                alog("debug: Sent: :%s %s", source, buf);
            }
        } else {
            sockprintf(servsock, "%s\r\n", buf);
            eventprintf("%s", buf);
            if (debug) {
                alog("debug: Sent: %s", buf);
            }
        }
    }
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
void notice_server(char *source, Server * s, char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);

        if (NSDefFlags & NI_MSG) {
            anope_cmd_serv_privmsg(source, s->name, buf);
        } else {
            anope_cmd_serv_notice(source, s->name, buf);
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

        /* Send privmsg instead of notice if:
         * - UsePrivmsg is enabled
         * - The user is not registered and NSDefMsg is enabled
         * - The user is registered and has set /ns set msg on
         */
        if (UsePrivmsg && ((!u->na && (NSDefFlags & NI_MSG))
                           || (u->na && (u->na->nc->flags & NI_MSG)))) {
            anope_cmd_privmsg2(source, u->nick, buf);
        } else {
            anope_cmd_notice2(source, u->nick, buf);
        }
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
            anope_cmd_notice2(source, dest, *text);
        } else {
            anope_cmd_notice2(source, dest, " ");
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
void notice_lang(char *source, User * dest, int message, ...)
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
            anope_cmd_privmsg2(source, dest->nick, *t ? t : " ");
        } else {
            anope_cmd_notice2(source, dest->nick, *t ? t : " ");
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
void notice_help(char *source, User * dest, int message, ...)
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
            anope_cmd_privmsg2(source, dest->nick, *outbuf ? outbuf : " ");
        } else {
            anope_cmd_notice2(source, dest->nick, *outbuf ? outbuf : " ");
        }
    }
    va_end(args);
}

/*************************************************************************/

/**
 * Send a NOTICE from the given source to the given nick.
 * @param source Orgin of the Message
 * @param dest Destination of the Message
 * @param fmt Format of the Message
 * @param ... any number of parameters
 * @return void
 */
void notice(char *source, char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);

        if (NSDefFlags & NI_MSG) {
            anope_cmd_privmsg2(source, dest, buf);
        } else {
            anope_cmd_notice2(source, dest, buf);
        }
        va_end(args);
    }
}

/*************************************************************************/

/**
 * Send a PRIVMSG from the given source to the given nick.
 * @param source Orgin of the Message
 * @param dest Destination of the Message
 * @param fmt Format of the Message
 * @param ... any number of parameters
 * @return void
 */
void privmsg(char *source, char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }

    anope_cmd_privmsg2(source, dest, buf);
}

/*************************************************************************/

/**
 * Send out a WALLOP, this is here for legacy only, same day we will pull it out
 * @param source Orgin of the Message
 * @param fmt Format of the Message
 * @param ... any number of parameters
 * @return void
 */
void wallops(char *source, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }

    anope_cmd_global_legacy(source, buf);
}
