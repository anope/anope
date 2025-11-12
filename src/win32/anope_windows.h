// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
// Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

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

#endif // _WIN32
#endif // WINDOWS_H
