 /* POSIX emulation layer for Windows.
 *
 * Copyright (C) 2008-2012 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#define RTLD_NOW 0

extern void *dlopen(const char *filename, int);
extern char *dlerror(void);
extern void *dlsym(void *handle, const char *symbol);
extern int dlclose(void *handle);
