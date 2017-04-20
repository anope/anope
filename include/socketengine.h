/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "services.h"
#include "sockets.h"

class CoreExport SocketEngine
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

