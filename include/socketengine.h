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

#pragma once

#include "services.h"
#include "sockets.h"

class CoreExport SocketEngine final
{
	static const int DefaultSize = 2; // Uplink, mode stacker
public:
	/* Map of sockets */
	static std::map<int, Socket *> Sockets;

	/** Called to initialize the socket engine
	 */
	static void Init();

	/** Called to shutdown the socket engine
	 */
	static void Shutdown();

	/** Set a flag on a socket
	 * @param s The socket
	 * @param set Whether setting or unsetting
	 * @param flag The flag to set or unset
	 */
	static void Change(Socket *s, bool set, SocketFlag flag);

	/** Read from sockets and do things
	 */
	static void Process();

	static int GetLastError();
	static void SetLastError(int);

	static bool IgnoreErrno();
};
