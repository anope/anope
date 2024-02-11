/* Timer include stuff.
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

#include "anope.h"

class CoreExport Timer
{
private:
	/** The owner of the timer, if any
	 */
	Module *owner;

	/** The triggering time
	 */
	time_t trigger;

	/** Number of seconds between triggers
	 */
	time_t secs;

	/** True if this is a repeating timer
	 */
	bool repeat;

public:
	/** Constructor, initializes the triggering time
	 * @param time_from_now The number of seconds from now to trigger the timer
	 * @param repeating Repeat this timer every time_from_now if this is true
	 */
	Timer(time_t time_from_now, bool repeating = false);

	/** Constructor, initializes the triggering time
	 * @param creator The creator of the timer
	 * @param time_from_now The number of seconds from now to trigger the timer
	 * @param repeating Repeat this timer every time_from_now if this is true
	 */
	Timer(Module *creator, time_t time_from_now, bool repeating = false);

	/** Destructor, removes the timer from the list
	 */
	virtual ~Timer();

	/** Set the trigger time to a new value
	 * @param t The new time
	 */
	void SetTimer(time_t t);

	/** Retrieve the triggering time
	 * @return The trigger time
	 */
	time_t GetTimer() const;

	/** Returns true if the timer is set to repeat
	 * @return Returns true if the timer is set to repeat
	 */
	bool GetRepeat() const;

	/** Set the interval between ticks
	 * @paramt t The new interval
	 */
	void SetSecs(time_t t);

	/** Returns the interval between ticks
	 * @return The interval
	 */
	long GetSecs() const;

	/** Returns the owner of this timer, if any
	 * @return The owner of the timer
	 */
	Module *GetOwner() const;

	/** Called when the timer ticks
	 * This should be overridden with something useful
	 */
	virtual void Tick() = 0;
};

/** This class manages sets of Timers, and triggers them at their defined times.
 * This will ensure timers are not missed, as well as removing timers that have
 * expired and allowing the addition of new ones.
 */
class CoreExport TimerManager final
{
	/** A list of timers
	 */
	static std::multimap<time_t, Timer *> Timers;
public:
	/** Add a timer to the list
	 * @param t A Timer derived class to add
	 */
	static void AddTimer(Timer *t);

	/** Deletes a timer
	 * @param t A Timer derived class to delete
	 */
	static void DelTimer(Timer *t);

	/** Tick all pending timers
	 * @param ctime The current time
	 */
	static void TickTimers();

	/** Deletes all timers owned by the given module
	 */
	static void DeleteTimersFor(Module *m);
};
