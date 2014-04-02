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

#include "services.h"
#include "threadengine.h"
#include "anope.h"
#include <system_error>

Thread::Thread() : exit(false)
{
}

Thread::~Thread()
{
}

void Thread::Join()
{
	this->SetExitState();
	try
	{
		if (this->handle.joinable())
			this->handle.join();
	}
	catch (const std::system_error &error)
	{
		throw CoreException("Unable to join thread: " + Anope::string(error.what()));
	}
}

void Thread::SetExitState()
{
	this->Notify();
	exit = true;
}

void Thread::Start()
{
	try
	{
		this->handle = std::thread([this]()
		{
			try
			{
				this->Run();
			}
			catch (...)
			{
				this->SetExitState();
				throw;
			}
			this->SetExitState();
		});
	}
	catch (const std::system_error &error)
	{
		this->flags[SF_DEAD] = true;
		throw CoreException("Unable to create thread: " + Anope::string(error.what()));
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

void Mutex::Lock()
{
	this->m.lock();
}

bool Mutex::TryLock()
{
	return this->m.try_lock();
}

void Mutex::Unlock()
{
	this->m.unlock();
}

void Condition::Wait()
{
	this->cv.wait(this->m);
}

void Condition::Wakeup()
{
	this->cv.notify_one();
}

