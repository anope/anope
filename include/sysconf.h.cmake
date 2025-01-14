/*
 *
 * (C) 2003-2025 Anope Team
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

// The extension used for module file extensions.
#define DLL_EXT "@CMAKE_SHARED_LIBRARY_SUFFIX@"

// Whether Anope was built in debug mode.
#cmakedefine01 DEBUG_BUILD

// The default config directory.
#define DEFAULT_CONF_DIR "@CONF_DIR@"

// The default data directory.
#define DEFAULT_DATA_DIR "@DATA_DIR@"

// The default locale directory.
#define DEFAULT_LOCALE_DIR "@LOCALE_DIR@"

// The default log directory.
#define DEFAULT_LOG_DIR "@LOG_DIR@"

// The default module directory.
#define DEFAULT_MODULE_DIR "@MODULE_DIR@"

// Whether the clock_gettime() function is available.
#cmakedefine01 HAVE_CLOCK_GETTIME

// Whether Anope was built with localization support.
#cmakedefine01 HAVE_LOCALIZATION

// Whether the umask() function is available.
#cmakedefine01 HAVE_UMASK

#ifdef _WIN32
# define popen _popen
# define pclose _pclose
#endif

#if defined __GNUC__
# define ATTR_FORMAT(STRINGPOS, FIRSTPOS) __attribute__((format(printf, STRINGPOS, FIRSTPOS)))
# define ATTR_NOT_NULL(...) __attribute__((nonnull(__VA_ARGS__)))
#else
# define ATTR_FORMAT(STRINGPOS, FIRSTPOS)
# define ATTR_NOT_NULL(...)
#endif
