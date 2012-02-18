/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#ifndef SIGNAL_H
#define SIGNAL_H

#include <signal.h>

#include "sockets.h"

/** Represents a signal handler
 */
class Signal : public Pipe
{
	static std::vector<Signal *> SignalHandlers;
	static void SignalHandler(int signal);

	struct sigaction action, old;
 public:
	int signal;

	/** Constructor
	 * @param s The signal to listen for
	 */
	Signal(int s);
	~Signal();

	/**
	 * Called when the signal is received.
	 * Note this is not *immediatly* called when the signal is received,
	 * but it is saved and called at a later time when we are not doing something
	 * important. This is always called on the main thread, even on systems that
	 * spawn threads for signals, like Windows.
	 */
	virtual void OnNotify() anope_override = 0;
};

#endif

