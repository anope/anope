/* Timer stuff.
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#include "services.h"
#include "timers.h"

std::vector<Timer *> TimerManager::Timers;

Timer::Timer(long time_from_now, time_t now, bool repeating)
{
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
	trigger = t;
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
	secs = t;
	trigger = Anope::CurTime + t;

	TimerManager::DelTimer(this);
	TimerManager::AddTimer(this);
}

long Timer::GetSecs() const
{
	return secs;
}

void TimerManager::AddTimer(Timer *t)
{
	Timers.push_back(t);
	sort(Timers.begin(), Timers.end(), TimerManager::TimerComparison);
}

void TimerManager::DelTimer(Timer *t)
{
	std::vector<Timer *>::iterator i = std::find(Timers.begin(), Timers.end(), t);

	if (i != Timers.end())
		Timers.erase(i);
}

void TimerManager::TickTimers(time_t ctime)
{
	while (Timers.size() && ctime > Timers.front()->GetTimer())
	{
		Timer *t = Timers.front();

		t->Tick(ctime);

		if (t->GetRepeat())
		{
			t->SetTimer(ctime + t->GetSecs());
			sort(Timers.begin(), Timers.end(), TimerManager::TimerComparison);
		}
		else
			delete t;
	}
}

bool TimerManager::TimerComparison(Timer *one, Timer *two)
{
	return one->GetTimer() < two->GetTimer();
}
