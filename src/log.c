/* Logging routines.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
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
#include "pseudo.h"

static FILE *logfile;

static int curday = 0;

/*************************************************************************/

static int get_logname(char *name, int count, struct tm *tm)
{
	char timestamp[32];
	time_t t;


	if (!tm) {
		time(&t);
		tm = localtime(&t);
	}

	/* fix bug 577 */
	strftime(timestamp, sizeof(timestamp), "%Y%m%d", tm);
	snprintf(name, count, "logs/%s.%s", log_filename.c_str(), timestamp);
	curday = tm->tm_yday;

	return 1;
}

/*************************************************************************/

static void remove_log()
{
	time_t t;
	struct tm tm;

	char name[PATH_MAX];

	if (!Config.KeepLogs)
		return;

	time(&t);
	t -= (60 * 60 * 24 * Config.KeepLogs);
	tm = *localtime(&t);

	/* removed if from here cause get_logchan is always 1 */
	get_logname(name, sizeof(name), &tm);
	DeleteFile(name);
}

/*************************************************************************/

static void checkday()
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

int open_log()
{
	char name[PATH_MAX];

	if (logfile)
		return 0;

	/* if removed again.. get_logname is always 1 */
	get_logname(name, sizeof(name), NULL);
	logfile = fopen(name, "a");

	if (logfile)
		setbuf(logfile, NULL);
	return logfile != NULL ? 0 : -1;
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

/* added cause this is used over and over in the code */
char *log_gettimestamp()
{
	time_t t;
	struct tm tm;
	static char tbuf[256];

	time(&t);
	tm = *localtime(&t);
#if HAVE_GETTIMEOFDAY
	if (debug) {
		char *s;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		strftime(tbuf, sizeof(tbuf) - 1, "[%b %d %H:%M:%S", &tm);
		s = tbuf + strlen(tbuf);
		s += snprintf(s, sizeof(tbuf) - (s - tbuf), ".%06d",
					  static_cast<int>(tv.tv_usec));
		strftime(s, sizeof(tbuf) - (s - tbuf) - 1, " %Y]", &tm);
	} else {
#endif
		strftime(tbuf, sizeof(tbuf) - 1, "[%b %d %H:%M:%S %Y]", &tm);
#if HAVE_GETTIMEOFDAY
	}
#endif
	return tbuf;
}

/*************************************************************************/

/* Like alog(), but tack a ": " and a system error message (as returned by
 * strerror()) onto the end.
 */

void log_perror(const char *fmt, ...)
{
	va_list args;
	char *buf;
	int errno_save = errno;
	char str[BUFSIZE];

	checkday();

	if (!fmt) {
		return;
	}

	va_start(args, fmt);
	vsnprintf(str, sizeof(str), fmt, args);
	va_end(args);

	buf = log_gettimestamp();

	if (logfile) {
		fprintf(logfile, "%s %s : %s\n", buf, str, strerror(errno_save));
	}
	if (nofork) {
		fprintf(stderr, "%s %s : %s\n", buf, str, strerror(errno_save));
	}
	errno = errno_save;
}

/*************************************************************************/

/* We've hit something we can't recover from.  Let people know what
 * happened, then go down.
 */

void fatal(const char *fmt, ...)
{
	va_list args;
	char *buf;
	char buf2[4096];

	checkday();

	if (!fmt) {
		return;
	}

	va_start(args, fmt);
	vsnprintf(buf2, sizeof(buf2), fmt, args);
	va_end(args);

	buf = log_gettimestamp();

	if (logfile)
		fprintf(logfile, "%s FATAL: %s\n", buf, buf2);
	if (nofork)
		fprintf(stderr, "%s FATAL: %s\n", buf, buf2);
	if (UplinkSock)
		ircdproto->SendGlobops(NULL, "FATAL ERROR!  %s", buf2);

	/* one of the many places this needs to be called from */
	ModuleRunTimeDirCleanUp();

	exit(1);
}

/*************************************************************************/

/* Same thing, but do it like perror(). */

void fatal_perror(const char *fmt, ...)
{
	va_list args;
	char *buf;
	char buf2[4096];
	int errno_save = errno;

	checkday();

	if (!fmt) {
		return;
	}

	va_start(args, fmt);
	vsnprintf(buf2, sizeof(buf2), fmt, args);
	va_end(args);

	buf = log_gettimestamp();

	if (logfile)
		fprintf(logfile, "%s FATAL: %s: %s\n", buf, buf2,
				strerror(errno_save));
	if (nofork)
		fprintf(stderr, "%s FATAL: %s: %s\n", buf, buf2,
				strerror(errno_save));
	if (UplinkSock)
		ircdproto->SendGlobops(NULL, "FATAL ERROR!  %s: %s", buf2,
						 strerror(errno_save));

	/* one of the many places this needs to be called from */
	ModuleRunTimeDirCleanUp();

	exit(1);
}

Alog::Alog(LogLevel val) : Level(val)
{
	if (Level >= LOG_DEBUG)
		buf << "Debug: ";
}

Alog::~Alog()
{
	if (Level >= LOG_DEBUG && (Level - LOG_DEBUG + 1) > debug)
		return;

	char *tbuf;
	int errno_save = errno;

	checkday();

	tbuf = log_gettimestamp();

	if (logfile) {
		fprintf(logfile, "%s %s\n", tbuf, buf.str().c_str());
	}
	if (nofork)
		std::cout << tbuf << " " << buf.str() << std::endl;
	else if (Level == LOG_TERMINAL) // XXX dont use this yet unless you know we're at terminal and not daemonized
		std::cout << buf.str() << std::endl;
	if (Config.LogChannel && LogChan && !debug && findchan(Config.LogChannel)) {
		ircdproto->SendPrivmsg(findbot(Config.s_GlobalNoticer), Config.LogChannel, "%s", buf.str().c_str());
	}
	errno = errno_save;
}
