#include "services.h"

/** Entry point for the thread
 * @param paramter A Thread* cast to a void*
 */
static DWORD WINAPI entry_point(void *parameter)
{
	Thread *thread = static_cast<Thread *>(parameter);
	thread->Run();
	if (!thread->GetExitState())
		thread->Join();
	delete thread;
	return 0;
}

/** Threadengines constructor
 */
ThreadEngine::ThreadEngine()
{
}

/** Threadengines destructor
 */
ThreadEngine::~ThreadEngine()
{
}

/** Join to the thread, sets the exit state to true
 */
void Thread::Join()
{
	SetExitState();
	WaitForSingleObject(Handle, INFINITE);
}

/** Start a new thread
 * @param thread A pointer to a newley allocated thread
 */
void ThreadEngine::Start(Thread *thread)
{
	thread->Handle = CreateThread(NULL, 0, entry_point, thread, 0, NULL);

	if (!thread->Handle)
	{
		delete thread;
		throw CoreException(Anope::string("Unable to create thread: ") + dlerror());
	}
}

/** Constructor
 */
Mutex::Mutex()
{
	InitializeCriticalSection(&mutex);
}

/** Destructor
 */
Mutex::~Mutex()
{
	DeleteCriticalSection(&mutex);
}

/** Attempt to lock the mutex, will hang until a lock can be achieved
 */
void Mutex::Lock()
{
	EnterCriticalSection(&mutex);
}

/** Unlock the mutex, it must be locked first
 */
void Mutex::Unlock()
{
	LeaveCriticalSection(&mutex);
}

/** Constructor
 */
Condition::Condition() : Mutex()
{
	cond = CreateEvent(NULL, false, false, NULL);
}

/** Destructor
 */
Condition::~Condition()
{
	CloseHandle(cond);
}

/** Called to wakeup the waiter
 */
void Condition::Wakeup()
{
	PulseEvent(cond);
}

/** Called to wait for a Wakeup() call
 */
void Condition::Wait()
{
	LeaveCriticalSection(&mutex);
	WaitForSingleObject(cond, INFINITE);
	EnterCriticalSection(&mutex);
}
