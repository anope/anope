/* Logging routines.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$ 
 *
 */

#include "services.h"
#include "pseudo.h"

static FILE *logfile;

static int curday = 0;

/*************************************************************************/

static int get_logname(char *name, int count, struct tm *tm)
{

    char timestamp[32];

    if (!tm) {
        time_t t;

        time(&t);
        tm = localtime(&t);
    }

    strftime(timestamp, count, "%Y%m%d", tm);
    snprintf(name, count, "logs/%s.%s", log_filename, timestamp);
    curday = tm->tm_yday;

    return 1;
}

/*************************************************************************/

static void remove_log(void)
{
    time_t t;
    struct tm tm;

    char name[PATH_MAX];

    if (!KeepLogs)
        return;

    time(&t);
    t -= (60 * 60 * 24 * KeepLogs);
    tm = *localtime(&t);

    if (!get_logname(name, sizeof(name), &tm))
        return;
    unlink(name);
}

/*************************************************************************/

static void checkday(void)
{
    time_t t;
    struct tm tm;

    time(&t);
    tm = *localtime(&t);

    if (curday != tm.tm_yday) {
        close_log();
        remove_log();
        open_log();
    }
}

/*************************************************************************/

/* Open the log file.  Return -1 if the log file could not be opened, else
 * return 0. */

int open_log(void)
{
    char name[PATH_MAX];

    if (logfile)
        return 0;

    if (!get_logname(name, sizeof(name), NULL))
        return 0;
    logfile = fopen(name, "a");

    if (logfile)
        setbuf(logfile, NULL);
    return logfile != NULL ? 0 : -1;
}

/* Close the log file. */

void close_log(void)
{
    if (!logfile)
        return;
    fclose(logfile);
    logfile = NULL;
}

/*************************************************************************/

/* Log stuff to the log file with a datestamp.  Note that errno is
 * preserved by this routine and log_perror().
 */

void alog(const char *fmt, ...)
{
    va_list args;
    time_t t;
    struct tm tm;
    char buf[256];
    int errno_save = errno;

    checkday();

    if (!fmt) {
        return;
    }

    va_start(args, fmt);
    time(&t);
    tm = *localtime(&t);
#if HAVE_GETTIMEOFDAY
    if (debug) {
        char *s;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        strftime(buf, sizeof(buf) - 1, "[%b %d %H:%M:%S", &tm);
        s = buf + strlen(buf);
        s += snprintf(s, sizeof(buf) - (s - buf), ".%06d", tv.tv_usec);
        strftime(s, sizeof(buf) - (s - buf) - 1, " %Y] ", &tm);
    } else {
#endif
        strftime(buf, sizeof(buf) - 1, "[%b %d %H:%M:%S %Y] ", &tm);
#if HAVE_GETTIMEOFDAY
    }
#endif
    if (logfile) {
        fputs(buf, logfile);
        vfprintf(logfile, fmt, args);
        fputc('\n', logfile);
    }
    if (nofork) {
        fputs(buf, stderr);
        vfprintf(stderr, fmt, args);
        fputc('\n', stderr);
    }

    if (LogChannel && logchan && !debug && findchan(LogChannel)) {
        char str[BUFSIZE];
        vsnprintf(str, sizeof(str), fmt, args);
        privmsg(s_GlobalNoticer, LogChannel, "%s", str);
    }

    va_end(args);
    errno = errno_save;
}


/* Like alog(), but tack a ": " and a system error message (as returned by
 * strerror()) onto the end.
 */

void log_perror(const char *fmt, ...)
{
    va_list args;
    time_t t;
    struct tm tm;
    char buf[256];
    int errno_save = errno;

    checkday();

    if (!fmt) {
        return;
    }

    va_start(args, fmt);
    time(&t);
    tm = *localtime(&t);
#if HAVE_GETTIMEOFDAY
    if (debug) {
        char *s;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        strftime(buf, sizeof(buf) - 1, "[%b %d %H:%M:%S", &tm);
        s = buf + strlen(buf);
        s += snprintf(s, sizeof(buf) - (s - buf), ".%06d", tv.tv_usec);
        strftime(s, sizeof(buf) - (s - buf) - 1, " %Y] ", &tm);
    } else {
#endif
        strftime(buf, sizeof(buf) - 1, "[%b %d %H:%M:%S %Y] ", &tm);
#if HAVE_GETTIMEOFDAY
    }
#endif
    if (logfile) {
        fputs(buf, logfile);
        vfprintf(logfile, fmt, args);
        fprintf(logfile, ": %s\n", strerror(errno_save));
    }
    if (nofork) {
        fputs(buf, stderr);
        vfprintf(stderr, fmt, args);
        fprintf(stderr, ": %s\n", strerror(errno_save));
    }
    errno = errno_save;
    va_end(args);
}

/*************************************************************************/

/* We've hit something we can't recover from.  Let people know what
 * happened, then go down.
 */

void fatal(const char *fmt, ...)
{
    va_list args;
    time_t t;
    struct tm tm;
    char buf[256], buf2[4096];

    checkday();

    va_start(args, fmt);
    time(&t);
    tm = *localtime(&t);
#if HAVE_GETTIMEOFDAY
    if (debug) {
        char *s;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        strftime(buf, sizeof(buf) - 1, "[%b %d %H:%M:%S", &tm);
        s = buf + strlen(buf);
        s += snprintf(s, sizeof(buf) - (s - buf), ".%06d", tv.tv_usec);
        strftime(s, sizeof(buf) - (s - buf) - 1, " %Y] ", &tm);
    } else {
#endif
        strftime(buf, sizeof(buf) - 1, "[%b %d %H:%M:%S %Y] ", &tm);
#if HAVE_GETTIMEOFDAY
    }
#endif
    vsnprintf(buf2, sizeof(buf2), fmt, args);
    if (logfile)
        fprintf(logfile, "%sFATAL: %s\n", buf, buf2);
    if (nofork)
        fprintf(stderr, "%sFATAL: %s\n", buf, buf2);
    if (servsock >= 0)
        anope_cmd_global(NULL, "FATAL ERROR!  %s", buf2);

    va_end(args);
    exit(1);
}


/* Same thing, but do it like perror(). */

void fatal_perror(const char *fmt, ...)
{
    va_list args;
    time_t t;
    struct tm tm;
    char buf[256], buf2[4096];
    int errno_save = errno;

    checkday();

    va_start(args, fmt);
    time(&t);
    tm = *localtime(&t);
#if HAVE_GETTIMEOFDAY
    if (debug) {
        char *s;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        strftime(buf, sizeof(buf) - 1, "[%b %d %H:%M:%S", &tm);
        s = buf + strlen(buf);
        s += snprintf(s, sizeof(buf) - (s - buf), ".%06d", tv.tv_usec);
        strftime(s, sizeof(buf) - (s - buf) - 1, " %Y] ", &tm);
    } else {
#endif
        strftime(buf, sizeof(buf) - 1, "[%b %d %H:%M:%S %Y] ", &tm);
#if HAVE_GETTIMEOFDAY
    }
#endif
    vsnprintf(buf2, sizeof(buf2), fmt, args);
    if (logfile)
        fprintf(logfile, "%sFATAL: %s: %s\n", buf, buf2,
                strerror(errno_save));
    if (stderr)
        fprintf(stderr, "%sFATAL: %s: %s\n", buf, buf2,
                strerror(errno_save));
    if (servsock >= 0)
        anope_cmd_global(NULL, "FATAL ERROR!  %s: %s", buf2,
                         strerror(errno_save));
    va_end(args);
    exit(1);
}

/*************************************************************************/
