/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
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

#pragma once

#include "anope.h"

class CoreExport Timer
{
 private:
 	/** The owner of the timer, if any
	 */
	Module *owner;

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
	/** Constructor, initializes the triggering time
	 * @param time_from_now The number of seconds from now to trigger the timer
	 * @param now The time now
	 * @param repeating Repeat this timer every time_from_now if this is true
	 */
	Timer(long time_from_now, time_t now = Anope::CurTime, bool repeating = false);

	/** Constructor, initializes the triggering time
	 * @param creator The creator of the timer
	 * @param time_from_now The number of seconds from now to trigger the timer
	 * @param now The time now
	 * @param repeating Repeat this timer every time_from_now if this is true
	 */
	Timer(Module *creator, long time_from_now, time_t now = Anope::CurTime, bool repeating = false);

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

	/** Returns the time this timer was created
	 * @return The time this timer was created
	 */
	time_t GetSetTime() const;

	/** Returns the owner of this timer, if any
	 * @return The owner of the timer
	 */
	Module *GetOwner() const;

	/** Called when the timer ticks
	 * This should be overridden with something useful
	 */
	virtual void Tick(time_t ctime) anope_abstract;
};

/** This class manages sets of Timers, and triggers them at their defined times.
 * This will ensure timers are not missed, as well as removing timers that have
 * expired and allowing the addition of new ones.
 */
class CoreExport TimerManager
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
	static void TickTimers(time_t ctime = Anope::CurTime);

	/** Deletes all timers owned by the given module
	 */
	static void DeleteTimersFor(Module *m);
};

