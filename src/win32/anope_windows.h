/* POSIX emulation layer for Windows.
 *
 * (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * (C) 2008-2025 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef WINDOWS_H
#define WINDOWS_H
#ifdef _WIN32

#define NOMINMAX

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <sys/timeb.h>
#include <direct.h>

#ifdef MODULE_COMPILE
# define CoreExport __declspec(dllimport)
# define DllExport __declspec(dllexport)
#else
# define CoreExport __declspec(dllexport)
# define DllExport __declspec(dllimport)
#endif

#if HAVE_LOCALIZATION
/* Undefine some functions libintl defines */
# undef snprintf
# undef vsnprintf
# undef printf
#endif

#define snprintf _snprintf
/* VS2008 hates having this define before its own */
#define vsnprintf _vsnprintf

#define anope_close windows_close

#define stat _stat
#define S_ISREG(x) ((x) & _S_IFREG)

#ifdef EINPROGRESS
# undef EINPROGRESS
#endif
#define EINPROGRESS WSAEWOULDBLOCK

#include "socket.h"
#include "dl/dl.h"
#include "pipe/pipe.h"
#include "sigaction/sigaction.h"

typedef SSIZE_T ssize_t;

namespace Anope
{
	class string;
}

extern CoreExport void OnStartup();
extern CoreExport void OnShutdown();
extern CoreExport USHORT WindowsGetLanguage(const Anope::string &lang);
extern int setenv(const char *name, const char *value, int overwrite);
extern int unsetenv(const char *name);
extern int mkstemp(char *input);

#endif // _WIN32
#endif // WINDOWS_H
