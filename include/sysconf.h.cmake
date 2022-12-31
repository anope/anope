/*
 *
 * (C) 2003-2023 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

// The default umask to use for files.
#cmakedefine DEFUMASK @DEFUMASK@

// Whether Anope was built in debug mode.
#cmakedefine01 DEBUG_BUILD

// Whether Anope was built with localization support.
#cmakedefine01 HAVE_LOCALIZATION

// Whether the umask() function is available.
#cmakedefine01 HAVE_UMASK

#ifdef _WIN32
# define popen _popen
# define pclose _pclose
# ifdef _MSC_VER
#  define PATH_MAX MAX_PATH
# endif
# define sleep(x) Sleep(x * 1000)
#endif
