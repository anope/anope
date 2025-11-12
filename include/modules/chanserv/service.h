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

#pragma once

class ChanServService
	: public Service
{
public:
	ChanServService(Module *m) : Service(m, "ChanServService", "ChanServ")
	{
	}

	/* Have ChanServ hold the channel, that is, join and set +nsti and wait
	 * for a few minutes so no one can join or rejoin.
	 */
	virtual void Hold(Channel *c) = 0;
};
