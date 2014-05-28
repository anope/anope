/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#pragma once

#include "sockets.h"
#include "extensible.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class CoreExport Thread : public Pipe
{
 private:
	/* Set to true to tell the thread to finish and we are waiting for it */
	bool exit;

 public:
	/* Handle for this thread */
	std::thread handle;

	/** Threads constructor
	 */
	Thread();

	/** Threads destructor
	 */
	virtual ~Thread();

	/** Join to the thread, sets the exit state to true
	 */
	void Join();

	/** Sets the exit state as true informing the thread we want it to shut down
	 */
	void SetExitState();

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
	virtual void Run() anope_abstract;
};

class Mutex
{
 protected:
	std::mutex m;

 public:
	void Lock();

	bool TryLock();

	void Unlock();
};

class Condition : public Mutex
{
	std::condition_variable_any cv;

 public:
	void Wakeup();

	void Wait();
};

