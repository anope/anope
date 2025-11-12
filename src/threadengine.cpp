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

#include "services.h"
#include "threadengine.h"
#include "anope.h"

#include <stdexcept>

static void *entry_point(void *parameter)
{
	Thread *thread = static_cast<Thread *>(parameter);
	thread->Run();
	thread->SetExitState();
	return NULL;
}

void Thread::Join()
{
	this->SetExitState();
	if (this->handle)
		this->handle->join();
}

void Thread::SetExitState()
{
	this->Notify();
	exit = true;
}

void Thread::Exit()
{
	this->SetExitState();
}

void Thread::Start()
{
	try
	{
		if (!this->handle)
			this->handle = std::make_unique<std::thread>(entry_point, this);
	}
	catch (const std::system_error &err)
	{
		this->flags[SF_DEAD] = true;
		throw CoreException("Unable to create thread: " + Anope::string(err.what()));
	}
}

bool Thread::GetExitState() const
{
	return exit;
}

void Thread::OnNotify()
{
	this->Join();
	this->flags[SF_DEAD] = true;
}
