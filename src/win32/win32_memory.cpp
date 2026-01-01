// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
// Copyright (C) 2002-2011 InspIRCd Development Team
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

#ifdef _WIN32

#include <windows.h>
#include <exception>
#include <new>
#include <new.h>

/** On windows, all dll files and executables have their own private heap,
 * whereas on POSIX systems, shared objects loaded into an executable share
 * the executable's heap. This means that if we pass an arbitrary pointer to
 * a windows DLL which is not allocated in that dll, without some form of
 * marshalling, we get a page fault. To fix this, these overridden operators
 * new and delete use the windows HeapAlloc and HeapFree functions to claim
 * memory from the windows global heap. This makes windows 'act like' POSIX
 * when it comes to memory usage between dlls and exes.
 */

void *::operator new(size_t iSize)
{
	void *ptr = HeapAlloc(GetProcessHeap(), 0, iSize); /* zero memory for unix compatibility */
	/* This is the correct behaviour according to C++ standards for out of memory,
	 * not returning null -- Brain
	 */
	if (!ptr)
		throw std::bad_alloc();
	else
		return ptr;
}

void ::operator delete(void *ptr)
{
	if (ptr)
		HeapFree(GetProcessHeap(), 0, ptr);
}

void *operator new[](size_t iSize)
{
	void *ptr = HeapAlloc(GetProcessHeap(), 0, iSize); /* Why were we initializing the memory to zeros here? This is just a waste of cpu! */
	if (!ptr)
		throw std::bad_alloc();
	else
		return ptr;
}

void operator delete[](void *ptr)
{
	if (ptr)
		HeapFree(GetProcessHeap(), 0, ptr);
}

#endif
