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

#define sigemptyset(x) memset((x), 0, sizeof(*(x)))

#ifndef SIGHUP
# define SIGHUP -1
#endif
#ifndef SIGPIPE
# define SIGPIPE -1
#endif

struct sigaction final
{
	void (*sa_handler)(int);
	int sa_flags;
	int sa_mask;
};

extern int sigaction(int, struct sigaction *, struct sigaction *);
