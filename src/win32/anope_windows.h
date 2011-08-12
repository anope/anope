 /* POSIX emulation layer for Windows.
 *
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2011 Anope Team <info@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */
 
 #ifndef WINDOWS_H
 #define WINDOWS_H
 #ifdef _WIN32
 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <sys/timeb.h>
#include <direct.h>
#include <io.h>
#ifdef MODULE_COMPILE
# define CoreExport __declspec(dllimport)
# define DllExport __declspec(dllexport)
#else
# define CoreExport __declspec(dllexport)
# define DllExport __declspec(dllimport)
#endif
/* We have our own inet_pton and inet_ntop (Windows XP doesn't have its own) */
#define inet_pton inet_pton_
#define inet_ntop inet_ntop_
#define setenv(x, y, z) SetEnvironmentVariable(x, y)
#define unsetenv(x) SetEnvironmentVariable(x, NULL)
#define MARK_DEPRECATED
#if GETTEXT_FOUND
/* Undefine some functions libintl defines */
# undef snprintf
# undef vsnprintf
# undef printf
#endif
#define snprintf _snprintf
/* VS2008 hates having this define before its own */
#define vsnprintf _vsnprintf
#define stat _stat
#define S_ISREG(x) ((x) & _S_IFREG)
#ifdef EINPROGRESS
# undef EINPROGRESS
#endif
#define EINPROGRESS WSAEWOULDBLOCK

#include "sigaction/sigaction.h"

namespace Anope
{
	class string;
}
 
extern CoreExport void OnStartup();
extern CoreExport void OnShutdown();
extern CoreExport USHORT WindowsGetLanguage(const char *lang);
extern CoreExport int inet_pton(int af, const char *src, void *dst);
extern CoreExport const char *inet_ntop(int af, const void *src, char *dst, size_t size);
extern CoreExport int gettimeofday(timeval *tv, void *);
extern CoreExport Anope::string GetWindowsVersion();
extern CoreExport bool SupportedWindowsVersion();
extern int mkstemp(char *input);
 
 #endif // _WIN32
 #endif // WINDOWS_H
