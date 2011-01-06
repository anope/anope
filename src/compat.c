/* Compatibility routines.
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

#if !HAVE_SNPRINTF

/* [v]snprintf: Like [v]sprintf, but don't write more than len bytes
 *              (including null terminator).  Return the number of bytes
 *              written.
 */

#if BAD_SNPRINTF
int vsnprintf(char *buf, size_t len, const char *fmt, va_list args)
{
    if (len <= 0)
        return 0;
    *buf = 0;
    vsnprintf(buf, len, fmt, args);
    buf[len - 1] = 0;
    return strlen(buf);
}
#endif                          /* BAD_SNPRINTF */

int snprintf(char *buf, size_t len, const char *fmt, ...)
{
    va_list args;
    int ret = 0;

    va_start(args, fmt);
    ret = vsnprintf(buf, len, fmt, args);
    va_end(args);
    return ret;
}

#endif                          /* !HAVE_SNPRINTF */

/*************************************************************************/

#if !HAVE_STRICMP && !HAVE_STRCASECMP

/* stricmp, strnicmp:  Case-insensitive versions of strcmp() and
 *                     strncmp().
 */

int stricmp(const char *s1, const char *s2)
{
    register int c;

    while ((c = tolower(*s1)) == tolower(*s2)) {
        if (c == 0)
            return 0;
        s1++;
        s2++;
    }
    if (c < tolower(*s2))
        return -1;
    return 1;
}

int strnicmp(const char *s1, const char *s2, size_t len)
{
    register int c;

    if (!len)
        return 0;
    while ((c = tolower(*s1)) == tolower(*s2) && len > 0) {
        if (c == 0 || --len == 0)
            return 0;
        s1++;
        s2++;
    }
    if (c < tolower(*s2))
        return -1;
    return 1;
}
#endif

/*************************************************************************/

#if !HAVE_STRDUP
char *strdup(const char *s)
{
    char *new = calloc(strlen(s) + 1, 1);
    if (new)
        strcpy(new, s);
    return new;
}
#endif

/*************************************************************************/

#if !HAVE_STRSPN
size_t strspn(const char *s, const char *accept)
{
    size_t i = 0;

    while (*s && strchr(accept, *s))
        ++i, ++s;
    return i;
}
#endif

/*************************************************************************/

#if !HAVE_STRERROR
# if HAVE_SYS_ERRLIST
extern char *sys_errlist[];
# endif

char *strerror(int errnum)
{
# if HAVE_SYS_ERRLIST
    return sys_errlist[errnum];
# else
    static char buf[20];
    snprintf(buf, sizeof(buf), "Error %d", errnum);
    return buf;
# endif
}
#endif

/*************************************************************************/

#if !HAVE_STRSIGNAL
/* Windows only supports 6 signals:
 * SIGINT, SIGILL, SIGABRT, SIGFPE, SIGSEGV, SIGTERM
 * -- codemastr
 */
char *strsignal(int signum)
{
    static char buf[32];
    switch (signum) {
#ifndef _WIN32
    case SIGHUP:
        strscpy(buf, "Hangup", sizeof(buf));
        break;
#endif
    case SIGINT:
        strscpy(buf, "Interrupt", sizeof(buf));
        break;
#ifndef _WIN32
    case SIGQUIT:
        strscpy(buf, "Quit", sizeof(buf));
        break;
#endif
#ifdef SIGILL
    case SIGILL:
        strscpy(buf, "Illegal instruction", sizeof(buf));
        break;
#endif
#ifdef SIGABRT
    case SIGABRT:
        strscpy(buf, "Abort", sizeof(buf));
        break;
#endif
#if defined(SIGIOT) && (!defined(SIGABRT) || SIGIOT != SIGABRT)
    case SIGIOT:
        strscpy(buf, "IOT trap", sizeof(buf));
        break;
#endif
#ifdef SIGBUS
    case SIGBUS:
        strscpy(buf, "Bus error", sizeof(buf));
        break;
#endif
    case SIGFPE:
        strscpy(buf, "Floating point exception", sizeof(buf));
        break;
#ifndef _WIN32
    case SIGKILL:
        strscpy(buf, "Killed", sizeof(buf));
        break;
    case SIGUSR1:
        strscpy(buf, "User signal 1", sizeof(buf));
        break;
#endif
    case SIGSEGV:
        strscpy(buf, "Segmentation fault", sizeof(buf));
        break;
#ifndef _WIN32
    case SIGUSR2:
        strscpy(buf, "User signal 2", sizeof(buf));
        break;
    case SIGPIPE:
        strscpy(buf, "Broken pipe", sizeof(buf));
        break;
    case SIGALRM:
        strscpy(buf, "Alarm clock", sizeof(buf));
        break;
#endif
    case SIGTERM:
        strscpy(buf, "Terminated", sizeof(buf));
        break;
#ifndef _WIN32
    case SIGSTOP:
        strscpy(buf, "Suspended (signal)", sizeof(buf));
        break;
    case SIGTSTP:
        strscpy(buf, "Suspended", sizeof(buf));
        break;
    case SIGIO:
        strscpy(buf, "I/O error", sizeof(buf));
        break;
#endif
    default:
        snprintf(buf, sizeof(buf), "Signal %d\n", signum);
        break;
    }
    return buf;
}
#endif


/*************************************************************************/
