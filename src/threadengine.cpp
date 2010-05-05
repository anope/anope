#include "services.h"

ThreadEngine threadEngine;

/** Threads constructor
 */
Thread::Thread() : Exit(false)
{
}

/** Threads destructor
 */
Thread::~Thread()
{
	Join();
}

/** Sets the exit state as true informing the thread we want it to shut down
 */
void Thread::SetExitState()
{
	Exit = true;
}

/** Returns the exit state of the thread
 * @return true if we want to exit
 */
bool Thread::GetExitState() const
{
	return Exit;
}

/** Called to run the thread, should be overloaded
 */
void Thread::Run()
{
}

