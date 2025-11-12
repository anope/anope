// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
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

#include "services.h"
#include "anope.h"

void *dlopen(const char *filename, int)
{
	return LoadLibrary(filename);
}

char *dlerror(void)
{
	static Anope::string err;
	err = Anope::LastError();
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
