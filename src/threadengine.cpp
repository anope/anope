/*
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

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
