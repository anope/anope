#include "services.h"

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
}

/** Join to the thread, sets the exit state to true
 */
void Thread::Join()
{
	this->SetExitState();
	pthread_join(Handle, NULL);
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
