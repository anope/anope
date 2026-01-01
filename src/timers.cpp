// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
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
#include "timers.h"

std::multimap<time_t, Timer *> TimerManager::Timers;

Timer::Timer(time_t time_from_now, bool repeating)
	: trigger(Anope::CurTime + std::abs(time_from_now))
	, secs(time_from_now)
	, repeat(repeating)
{
	if (time_from_now)
		TimerManager::AddTimer(this);
}

Timer::Timer(Module *creator, time_t time_from_now, bool repeating)
	: owner(creator)
	, trigger(Anope::CurTime + std::abs(time_from_now))
	, secs(time_from_now)
	, repeat(repeating)
{
	if (time_from_now)
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
	Timers.emplace(t->GetTimer(), t);
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

void TimerManager::TickTimers()
{
	while (!Timers.empty())
	{
		std::multimap<time_t, Timer *>::iterator it = Timers.begin();
		Timer *t = it->second;

		if (t->GetTimer() > Anope::CurTime)
			break;

		t->Tick();

		if (t->GetRepeat())
			t->SetTimer(Anope::CurTime + t->GetSecs());
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
