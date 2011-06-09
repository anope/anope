/* Timer stuff.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"

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
	settime = now;

	TimerManager::AddTimer(this);
}

/** Default destructor, removes the timer from the list
 */
Timer::~Timer()
{
	TimerManager::DelTimer(this);
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
time_t Timer::GetTimer() const
{
	return trigger;
}

/** Returns true if the timer is set to repeat
 * @return Returns true if the timer is set to repeat
 */
bool Timer::GetRepeat() const
{
	return repeat;
}

/** Returns the time this timer was created
* @return The time this timer was created
*/
time_t Timer::GetSetTime() const
{
	return settime;
}

/** Sets the interval between ticks
 * @param t The new interval
 */
void Timer::SetSecs(time_t t)
{
	secs = t;
	trigger = Anope::CurTime + t;
	sort(TimerManager::Timers.begin(), TimerManager::Timers.end(), TimerManager::TimerComparison);
}

/** Returns the interval between ticks
* @return The interval
*/
long Timer::GetSecs() const
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
		Timers.erase(i);
}

/** Tick all pending timers
 * @param ctime The current time
 */
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

/** Compares two timers
 */
bool TimerManager::TimerComparison(Timer *one, Timer *two)
{
	return one->GetTimer() < two->GetTimer();
}
