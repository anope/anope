/* POSIX emulation layer for Windows.
 *
 * (C) 2008-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#define RTLD_NOW 0

extern void *dlopen(const char *filename, int);
extern char *dlerror(void);
extern void *dlsym(void *handle, const char *symbol);
extern int dlclose(void *handle);
