/* POSIX emulation layer for Windows.
 *
 * (C) 2008-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#define sigemptyset(x) memset((x), 0, sizeof(*(x)))

#ifndef SIGHUP
# define SIGHUP -1
#endif
#ifndef SIGPIPE
# define SIGPIPE -1
#endif

struct sigaction
{
	void (*sa_handler)(int);
	int sa_flags;
	int sa_mask;
};

extern int sigaction(int, struct sigaction *, struct sigaction *);
