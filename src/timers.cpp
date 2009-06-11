/* Timer stuff.
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$ 
 *
 */

#include "timers.h"

std::vector<Timer *> TimerManager::Timers;

/** Default constructor, initializes the triggering time
 * @param time_from_now The number of seconds from now to trigger the timer
 * @param now The time now
 * @param repeating Repeat this timer every time_from_now if this is true
 */
Timer::Timer(long time_from_now, time_t now, bool repeating)
{
	trigger = now + time_from_now;
	secs = time_from_now;
	repeat = repeating;

	TimerManager::AddTimer(this);
}

/** Default destructor, does nothing
 */
Timer::~Timer()
{
}

/** Set the trigger time to a new value
 * @param t The new time
*/
void Timer::SetTimer(time_t t)
{
	trigger = t;
}

/** Retrieve the triggering time
 * @return The trigger time
 */
const time_t Timer::GetTimer()
{
	return trigger;
}

/** Returns true if the timer is set to repeat
 * @return Returns true if the timer is set to repeat
 */
const bool Timer::GetRepeat()
{
	return repeat;
}

/** Returns the time this timer was created
* @return The time this timer was created
*/
const time_t Timer::GetSetTime()
{
	return settime;
}

/** Returns the interval between ticks
* @return The interval
*/
const long Timer::GetSecs()
{
	return secs;
}

/** Add a timer to the list
 * @param T A Timer derived class to add
 */
void TimerManager::AddTimer(Timer *T)
{
	Timers.push_back(T);
	sort(Timers.begin(), Timers.end(), TimerManager::TimerComparison);
}

/** Deletes a timer
 * @param T A Timer derived class to delete
 */
void TimerManager::DelTimer(Timer *T)
{
	std::vector<Timer *>::iterator i = std::find(Timers.begin(), Timers.end(), T);
		
	if (i != Timers.end())
	{
		delete (*i);
		Timers.erase(i);
	}
}

/** Tick all pending timers
 * @param time The current time
 */
void TimerManager::TickTimers(time_t ctime)
{
	std::vector<Timer *>::iterator i;
	Timer *t;

	while ((Timers.size()) && (ctime > (*Timers.begin())->GetTimer()))
	{
		i = Timers.begin();
		
		t = *i;

		t->Tick(ctime);
		
		Timers.erase(i);
		
		if (t->GetRepeat())
		{
			t->SetTimer(ctime + t->GetSecs());
			AddTimer(t);
		}
		else
			delete t;
	}
}

/** Compares two timers
 */
bool TimerManager::TimerComparison(Timer *one, Timer *two)
{
	return (one->GetTimer() < two->GetTimer());
}
