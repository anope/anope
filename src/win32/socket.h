// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
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

#define read read_not_used
#define write write_not_used
#include <io.h>
#undef read
#undef write

#define F_GETFL 0
#define F_SETFL 1

#define O_NONBLOCK 1

extern CoreExport int read(int fd, char *buf, size_t count);
extern CoreExport int write(int fd, const char *buf, size_t count);
extern CoreExport int windows_close(int fd);
extern CoreExport int windows_accept(int fd, struct sockaddr *addr, int *addrlen);
extern CoreExport int fcntl(int fd, int cmd, int arg);

#ifndef WIN32_NO_OVERRIDE
# define accept windows_accept
#endif
