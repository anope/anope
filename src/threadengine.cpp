/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "threadengine.h"
#include "anope.h"

#include <pthread.h>

static inline pthread_attr_t *get_engine_attr()
{
	/* Threadengine attributes used by this thread engine */
	static pthread_attr_t attr;
	static bool inited = false;

	if (inited == false)
	{
		if (pthread_attr_init(&attr))
			throw CoreException("Error calling pthread_attr_init");
		if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE))
			throw CoreException("Unable to mark threads as joinable");
		inited = true;
	}

	return &attr;
}

/** Entry point used for the threads
 * @param parameter A Thread* cast to a void*
 */
static void *entry_point(void *parameter)
{
	Thread *thread = static_cast<Thread *>(parameter);
	thread->Run();
	thread->SetExitState();
	pthread_exit(0);
	return NULL;
}

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

/** Join to the thread, sets the exit state to true
 */
void Thread::Join()
{
	this->SetExitState();
	pthread_join(Handle, NULL);
}

/** Sets the exit state as true informing the thread we want it to shut down
 */
void Thread::SetExitState()
{
	this->Notify();
	exit = true;
}

/** Exit the thread. Note that the thread still must be joined to free resources!
 */
void Thread::Exit()
{
	this->SetExitState();
	pthread_exit(0);
}

/** Launch the thread
 */
void Thread::Start()
{
	if (pthread_create(&this->Handle, get_engine_attr(), entry_point, this))
	{
		this->SetFlag(SF_DEAD);
		throw CoreException("Unable to create thread: " + Anope::LastError());
	}
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

/** Constructor
 */
Mutex::Mutex()
{
	pthread_mutex_init(&mutex, NULL);
}

/** Destructor
 */
Mutex::~Mutex()
{
	pthread_mutex_destroy(&mutex);
}

/** Attempt to lock the mutex, will hang until a lock can be achieved
 */
void Mutex::Lock()
{
	pthread_mutex_lock(&mutex);
}

/** Unlock the mutex, it must be locked first
 */
void Mutex::Unlock()
{
	pthread_mutex_unlock(&mutex);
}

/** Attempt to lock the mutex, will return true on success and false on fail
 * Does not block
 * @return true or false
 */
bool Mutex::TryLock()
{
	return pthread_mutex_trylock(&mutex) == 0;
}

/** Constructor
 */
Condition::Condition() : Mutex()
{
	pthread_cond_init(&cond, NULL);
}

/** Destructor
 */
Condition::~Condition()
{
	pthread_cond_destroy(&cond);
}

/** Called to wakeup the waiter
 */
void Condition::Wakeup()
{
	pthread_cond_signal(&cond);
}

/** Called to wait for a Wakeup() call
 */
void Condition::Wait()
{
	pthread_cond_wait(&cond, &mutex);
}
