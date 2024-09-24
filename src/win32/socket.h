/* POSIX emulation layer for Windows.
 *
 * (C) 2008-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

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
