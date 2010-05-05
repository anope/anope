#ifdef _WIN32
typedef HANDLE ThreadHandle;
typedef CRITICAL_SECTION MutexHandle;
typedef HANDLE CondHandle;
#else
# include <pthread.h>
typedef pthread_t ThreadHandle;
typedef pthread_mutex_t MutexHandle;
typedef pthread_cond_t CondHandle;
#endif

class ThreadEngine;
class Thread;

extern CoreExport ThreadEngine threadEngine;

class ThreadEngine
{
 public:
	/** Threadengines constructor
	 */
	ThreadEngine();

	/** Threadengines destructor
	 */
	~ThreadEngine();

	/** Start a new thread
	 * @param thread A pointer to a newley allocated thread
	 */
	void Start(Thread *thread);
};

class Thread : public Extensible
{
 private:
	/* Set to true to tell the thread to finish and we are waiting for it */
	bool Exit; 

	/** Join to the thread, sets the exit state to true
	 */
	void Join();
 public:
 	/* Handle for this thread */
	ThreadHandle Handle;

	/** Threads constructor
	 */
	Thread();

	/** Threads destructor
	 */
	virtual ~Thread();

	/** Sets the exit state as true informing the thread we want it to shut down
	 */
	void SetExitState();

	/** Returns the exit state of the thread
	 * @return true if we want to exit
	 */
	bool GetExitState() const;

	/** Called to run the thread, should be overloaded
	 */
	virtual void Run();
};

class Mutex
{
 protected:
	/* A mutex, used to keep threads in sync */
	MutexHandle mutex;

 public:
	/** Constructor
	 */
 	Mutex();

	/** Destructor
	 */
	~Mutex();

	/** Attempt to lock the mutex, will hang until a lock can be achieved
	 */
	void Lock();

	/** Unlock the mutex, it must be locked first
	 */
	void Unlock();
};

class Condition : public Mutex
{
 private:
	/* A condition */
	CondHandle cond;

 public:
	/** Constructor
	 */
	Condition();

	/** Destructor
	 */
	~Condition();

	/** Called to wakeup the waiter
	 */
	void Wakeup();

	/** Called to wait for a Wakeup() call
	 */
	void Wait();
};

