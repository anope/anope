/* Timer include stuff.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

#ifndef TIMERS_H
#define TIMERS_H

#include "services.h"
#include <time.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

class CoreExport Timer : public Extensible
{
	private:
		/** The time this was created
		 */
		time_t settime;
		
		/** The triggering time
		 */
		time_t trigger;
		
		/** Numer of seconds between triggers
		 */
		long secs;
		
		/** True if this is a repeating timer
		 */
		bool repeat;
		
	public:
		/** Default constructor, initializes the triggering time
		 * @param time_from_now The number of seconds from now to trigger the timer
		 * @param now The time now
		 * @param repeating Repeat this timer every time_from_now if this is true
		 */
		Timer(long time_from_now, time_t now = time(NULL), bool repeating = false);
		
		/** Default destructor, removes the timer from the list
		 */
		virtual ~Timer();
		
		/** Set the trigger time to a new value
		 * @param t The new time
		 */
		void SetTimer(time_t t);
		
		/** Retrieve the triggering time
		 * @return The trigger time
		 */
		const time_t GetTimer();
		
		/** Returns true if the timer is set to repeat
		 * @return Returns true if the timer is set to repeat
		 */
		const bool GetRepeat();
		
		/** Returns the interval between ticks
		 * @return The interval
		 */
		const long GetSecs();
		
		/** Returns the time this timer was created
		 * @return The time this timer was created
		 */
		const time_t GetSetTime();
		
		/** Called when the timer ticks
		 * This should be overridden with something useful
		 */
		virtual void Tick(time_t ctime) = 0;
};

/** This class manages sets of Timers, and triggers them at their defined times.
 * This will ensure timers are not missed, as well as removing timers that have
 * expired and allowing the addition of new ones.
 */
class CoreExport TimerManager : public Extensible
{
	protected:
		/** A list of timers
		 */
		static std::vector<Timer *> Timers;
	public:
		/** Add a timer to the list
		 * @param T A Timer derived class to add
		 */
		static void AddTimer(Timer *T);
		
		/** Deletes a timer
		 * @param T A Timer derived class to delete
		 */
		static void DelTimer(Timer *T);
		
		/** Tick all pending timers
		 * @param ctime The current time
		 */
		static void TickTimers(time_t ctime = time(NULL));
		
		/** Compares two timers
		 */
		static bool TimerComparison(Timer *one, Timer *two);
};

#endif
