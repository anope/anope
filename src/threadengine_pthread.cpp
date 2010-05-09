#include "services.h"

/** Join to the thread, sets the exit state to true
 */
void Thread::Join()
{
	SetExitState();
	pthread_join(Handle, NULL);
	delete this;
}

/* Threadengine attributes used by this thread engine */
static pthread_attr_t threadengine_attr;

/** Entry point used for the threads
 * @param parameter A Thread* cast to a void*
 */
static void *entry_point(void *parameter)
{
	Thread *thread = static_cast<Thread *>(parameter);
	thread->Run();
	pthread_exit(0);
}

/** Threadengines constructor
 */
ThreadEngine::ThreadEngine()
{
	if (pthread_attr_init(&threadengine_attr))
	{
		throw CoreException("ThreadEngine: Error calling pthread_attr_init");
	}
}

/** Threadengines destructor
 */
ThreadEngine::~ThreadEngine()
{
	pthread_attr_destroy(&threadengine_attr);
}

/** Start a new thread
 * @param thread A pointer to a newley allocated thread
 */
void ThreadEngine::Start(Thread *thread)
{
	if (pthread_create(&thread->Handle, &threadengine_attr, entry_point, thread))
	{
		delete thread;
		throw CoreException("Unable to create thread");
	}
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

