/* Routines for sending stuff to the network.
 *
 * (C) 2003 Anope Team
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

/* Send a command to the server.  The two forms here are like
 * printf()/vprintf() and friends. */

void send_cmd(const char *source, const char *fmt, ...)
{
    va_list args;

    if (fmt) {
        va_start(args, fmt);
        vsend_cmd(source, fmt, args);
        va_end(args);
    }
}

void vsend_cmd(const char *source, const char *fmt, va_list args)
{
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        vsnprintf(buf, BUFSIZE - 1, fmt, args);

        if (!buf) {
            return;
        }

        if (source) {
            sockprintf(servsock, ":%s %s\r\n", source, buf);
            if (debug)
                alog("debug: Sent: :%s %s", source, buf);
        } else {
            sockprintf(servsock, "%s\r\n", buf);
            if (debug)
                alog("debug: Sent: %s", buf);
        }
    }
}

/*************************************************************************/

void notice_server(char *source, Server * s, char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);

        if (!buf) {
            return;
        }

        if (UsePrivmsg) {
            anope_cmd_serv_privmsg(source, s->name, buf);
        } else {
            anope_cmd_serv_notice(source, s->name, buf);
        }
        va_end(args);
    }
}

/*************************************************************************/

void notice_user(char *source, User * u, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);

        if (!buf) {
            return;
        }

        if (UsePrivmsg && (!u->na || (u->na->nc->flags & NI_MSG))) {
            anope_cmd_privmsg2(source, u->nick, buf);
        } else {
            anope_cmd_notice2(source, u->nick, buf);
        }
        va_end(args);
    }
}

/*************************************************************************/

/* Send a NULL-terminated array of text as NOTICEs. */
void notice_list(char *source, char *dest, char **text)
{
    while (*text) {
        /* Have to kludge around an ircII bug here: if a notice includes
         * no text, it is ignored, so we replace blank lines by lines
         * with a single space.
         */
        if (**text)
            anope_cmd_notice2(source, dest, *text);
        else
            anope_cmd_notice2(source, dest, " ");
        text++;
    }
}

/*************************************************************************/

/* Send a message in the user's selected language to the user using NOTICE. */
void notice_lang(char *source, User * dest, int message, ...)
{
    va_list args;
    char buf[4096];             /* because messages can be really big */
    char *s, *t;
    const char *fmt;
    if (!dest || !message)
        return;
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
        if (UsePrivmsg && (!dest->na || (dest->na->nc->flags & NI_MSG))) {
            anope_cmd_privmsg2(source, dest->nick, *t ? t : " ");
        } else {
            anope_cmd_notice2(source, dest->nick, *t ? t : " ");
        }
    }
    va_end(args);
}

/*************************************************************************/

/* Like notice_lang(), but replace %S by the source.  This is an ugly hack
 * to simplify letting help messages display the name of the pseudoclient
 * that's sending them.
 */
void notice_help(char *source, User * dest, int message, ...)
{
    va_list args;
    char buf[4096], buf2[4096], outbuf[BUFSIZE];
    char *s, *t;
    const char *fmt;

    if (!dest || !message)
        return;
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
        if (UsePrivmsg && (!dest->na || (dest->na->nc->flags & NI_MSG))) {
            anope_cmd_privmsg2(source, dest->nick, *outbuf ? outbuf : " ");
        } else {
            anope_cmd_notice2(source, dest->nick, *outbuf ? outbuf : " ");
        }
    }
    va_end(args);
}

/*************************************************************************/

/* Send a NOTICE from the given source to the given nick. */
void notice(char *source, char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';

    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);

        if (!buf) {
            return;
        }

        if (UsePrivmsg) {
            anope_cmd_privmsg2(source, dest, buf);
        } else {
            anope_cmd_notice2(source, dest, buf);
        }
        va_end(args);
    }
}

/*************************************************************************/

/* Send a PRIVMSG from the given source to the given nick. */
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

    if (!buf) {
        return;
    }
    anope_cmd_privmsg2(source, dest, buf);
}

/* cause #defines just bitched to much, its back and hooks to 
   a legacy in the ircd protocol files  - TSL */
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
    if (!buf) {
        return;
    }

    anope_cmd_global_legacy(source, buf);
}
