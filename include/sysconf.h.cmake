/*
 *
 * (C) 2003-2021 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

#cmakedefine DEBUG_BUILD

#cmakedefine DEFUMASK @DEFUMASK@
#cmakedefine HAVE_STRINGS_H 1
#cmakedefine HAVE_UMASK 1
#cmakedefine HAVE_EVENTFD 1
#cmakedefine HAVE_EPOLL 1
#cmakedefine HAVE_POLL 1
#cmakedefine GETTEXT_FOUND 1

#ifdef _WIN32
# define popen _popen
# define pclose _pclose
# define ftruncate _chsize
# ifdef MSVCPP
#  define PATH_MAX MAX_PATH
# endif
# define MAXPATHLEN MAX_PATH
# define bzero(buf, size) memset(buf, 0, size)
# define sleep(x) Sleep(x * 1000)
#endif
