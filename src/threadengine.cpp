#include "services.h"

ThreadEngine threadEngine;

/** Check for finished threads
 */
void ThreadEngine::Process()
{
	for (unsigned i = this->threads.size(); i > 0; --i)
	{
		Thread *t = this->threads[i - 1];

		if (t->GetExitState())
		{
			t->Join();
			delete t;
		}
	}
}

/** Threads constructor
 */
Thread::Thread() : exit(false)
{
	threadEngine.threads.push_back(this);
}

/** Threads destructor
 */
Thread::~Thread()
{
	std::vector<Thread *>::iterator it = std::find(threadEngine.threads.begin(), threadEngine.threads.end(), this);

	if (it != threadEngine.threads.end())
	{
		threadEngine.threads.erase(it);
	}
}

/** Sets the exit state as true informing the thread we want it to shut down
 */
void Thread::SetExitState()
{
	exit = true;
}

/** Returns the exit state of the thread
 * @return true if we want to exit
 */
bool Thread::GetExitState() const
{
	return exit;
}

/** Called to run the thread, should be overloaded
 */
void Thread::Run()
{
}
