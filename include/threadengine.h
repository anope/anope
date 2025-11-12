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

#include "sockets.h"
#include "extensible.h"

#include <thread>

class CoreExport Thread
	: public Pipe
	, public Extensible
{
private:
	/* Set to true to tell the thread to finish and we are waiting for it */
	bool exit = false;

public:
	/* Handle for this thread */
	std::unique_ptr<std::thread> handle;

	/** Threads destructor
	 */
	virtual ~Thread() = default;

	/** Join to the thread, sets the exit state to true
	 */
	void Join();

	/** Sets the exit state as true informing the thread we want it to shut down
	 */
	void SetExitState();

	/** Exit the thread. Note that the thread still must be joined to free resources!
	 */
	void Exit();

	/** Launch the thread
	 */
	void Start();

	/** Returns the exit state of the thread
	 * @return true if we want to exit
	 */
	bool GetExitState() const;

	/** Called when this thread should be joined to
	 */
	void OnNotify();

	/** Called when the thread is run.
	 */
	virtual void Run() = 0;
};
