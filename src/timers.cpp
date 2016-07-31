/*
 * Anope IRC Services
 *
 * Copyright (C) 2009-2016 Anope Team <team@anope.org>
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
#include "timers.h"

std::multimap<time_t, Timer *> TimerManager::Timers;

Timer::Timer(long time_from_now, time_t now, bool repeating)
{
	owner = NULL;
	trigger = now + time_from_now;
	secs = time_from_now;
	repeat = repeating;
	settime = now;

	TimerManager::AddTimer(this);
}

Timer::Timer(Module *creator, long time_from_now, time_t now, bool repeating)
{
	owner = creator;
	trigger = now + time_from_now;
	secs = time_from_now;
	repeat = repeating;
	settime = now;

	TimerManager::AddTimer(this);
}

Timer::~Timer()
{
	TimerManager::DelTimer(this);
}

void Timer::SetTimer(time_t t)
{
	TimerManager::DelTimer(this);
	trigger = t;
	TimerManager::AddTimer(this);
}

time_t Timer::GetTimer() const
{
	return trigger;
}

bool Timer::GetRepeat() const
{
	return repeat;
}

time_t Timer::GetSetTime() const
{
	return settime;
}

void Timer::SetSecs(time_t t)
{
	TimerManager::DelTimer(this);
	secs = t;
	trigger = Anope::CurTime + t;
	TimerManager::AddTimer(this);
}

long Timer::GetSecs() const
{
	return secs;
}

Module *Timer::GetOwner() const
{
	return owner;
}

void TimerManager::AddTimer(Timer *t)
{
	Timers.insert(std::make_pair(t->GetTimer(), t));
}

void TimerManager::DelTimer(Timer *t)
{
	std::pair<std::multimap<time_t, Timer *>::iterator, std::multimap<time_t, Timer *>::iterator> itpair = Timers.equal_range(t->GetTimer());
	for (std::multimap<time_t, Timer *>::iterator i = itpair.first; i != itpair.second; ++i)
	{
		if (i->second == t)
		{
			Timers.erase(i);
			break;
		}
	}
}

void TimerManager::TickTimers(time_t ctime)
{
	while (!Timers.empty())
	{
		std::multimap<time_t, Timer *>::iterator it = Timers.begin();
		Timer *t = it->second;

		if (t->GetTimer() > ctime)
			break;

		t->Tick(ctime);

		if (t->GetRepeat())
			t->SetTimer(ctime + t->GetSecs());
		else
			delete t;
	}
}

void TimerManager::DeleteTimersFor(Module *m)
{
	for (std::multimap<time_t, Timer *>::iterator it = Timers.begin(), it_next = it; it != Timers.end(); it = it_next)
	{
		++it_next;
		if (it->second->GetOwner() == m)
			delete it->second;
	}
}

