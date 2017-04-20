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

