 /* POSIX emulation layer for Windows.
 *
 * Copyright (C) 2008-2013 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"
#include "anope.h"

void *dlopen(const char *filename, int)
{
	return LoadLibrary(filename);
}

char *dlerror(void)
{
	static Anope::string err = Anope::LastError();
	SetLastError(0);
	return err.empty() ? NULL : const_cast<char *>(err.c_str());
}

void *dlsym(void *handle, const char *symbol)
{
	return GetProcAddress(reinterpret_cast<HMODULE>(handle), symbol);
}

int dlclose(void *handle)
{
	return !FreeLibrary(reinterpret_cast<HMODULE>(handle));
}
