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
#cmakedefine HAVE_UMASK 1
#cmakedefine GETTEXT_FOUND 1

#ifdef _WIN32
# define popen _popen
# define pclose _pclose
# ifdef MSVCPP
#  define PATH_MAX MAX_PATH
# endif
# define sleep(x) Sleep(x * 1000)
#endif
