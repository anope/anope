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

#include <windows.h>
#include "sigaction.h"
#include <signal.h>

int sigaction(int sig, struct sigaction *action, struct sigaction *old)
{
	if (sig == -1)
		return 0;
	if (old == NULL)
	{
		if (signal(sig, SIG_DFL) == SIG_ERR)
			return -1;
	}
	else
	{
		if (signal(sig, action->sa_handler) == SIG_ERR)
			return -1;
	}
	return 0;
}
