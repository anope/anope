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

#include "services.h"
#include "sockets.h"

int pipe(int fds[2])
{
	sockaddrs localhost("127.0.0.1");

	int cfd = socket(AF_INET, SOCK_STREAM, 0), lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (cfd == -1 || lfd == -1)
	{
		anope_close(cfd);
		anope_close(lfd);
		return -1;
	}

	if (bind(lfd, &localhost.sa, localhost.size()) == -1)
	{
		anope_close(cfd);
		anope_close(lfd);
		return -1;
	}

	if (listen(lfd, 1) == -1)
	{
		anope_close(cfd);
		anope_close(lfd);
		return -1;
	}

	sockaddrs lfd_addr;
	socklen_t sz = sizeof(lfd_addr);
	getsockname(lfd, &lfd_addr.sa, &sz);

	if (connect(cfd, &lfd_addr.sa, lfd_addr.size()))
	{
		anope_close(cfd);
		anope_close(lfd);
		return -1;
	}

	int afd = accept(lfd, NULL, NULL);
	anope_close(lfd);
	if (afd == -1)
	{
		anope_close(cfd);
		return -1;
	}

	fds[0] = cfd;
	fds[1] = afd;

	return 0;
}
