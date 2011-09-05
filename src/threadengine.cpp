#include "services.h"

/** Threads constructor
 */
Thread::Thread() : exit(false)
{
}

/** Threads destructor
 */
Thread::~Thread()
{
}

/** Sets the exit state as true informing the thread we want it to shut down
 */
void Thread::SetExitState()
{
	this->Notify();
	exit = true;
}

/** Returns the exit state of the thread
 * @return true if we want to exit
 */
bool Thread::GetExitState() const
{
	return exit;
}

/** Called when this thread should be joined to
 */
void Thread::OnNotify()
{
	this->Join();
	this->SetFlag(SF_DEAD);
}

