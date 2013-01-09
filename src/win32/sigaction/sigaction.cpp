 /* POSIX emulation layer for Windows.
 *
 * Copyright (C) 2008-2013 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */
 
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
 