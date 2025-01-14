/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

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
