/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2016 Anope Team <team@anope.org>
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
	void OnNotify() override;

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

