/*
 *
 * (C) 2004 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

#ifndef THREADS_H
#define THREADS_H

#ifdef _WIN32
typedef long ano_thread_t;
typedef HANDLE ano_mutex_t;
typedef HANDLE ano_cond_t;
typedef unsigned 	(__stdcall *ano_thread_start) (void *);
typedef struct 
{
	ano_thread_start func;
	void *arg;
} ano_cleanup_t;

extern ano_thread_start __declspec(thread) cleanup_func;

#define ano_thread_create(thread,start,arg)  	!_beginthreadex(NULL, 0, (ano_thread_start)start, arg, 0, &thread)
#define ano_thread_self()			GetCurrentThreadId()
#define ano_thread_detach(thread)		0
#define ano_mutex_lock(mutex)			WaitForSingleObject(mutex, INFINITE)
#define ano_mutex_unlock(mutex)			ReleaseMutex(mutex)

/* ano_cond_wait is in compat.c */
#define ano_cond_signal(cond)			SetEvent(cond)

/* very minimalistic implementation */
#define ano_cleanup_push(func, arg)		cleanup_func = (ano_thread_start)func
#define ano_cleanup_pop(execute)		cleanup_func(NULL)

#else

typedef pthread_t ano_thread_t;
typedef pthread_mutex_t ano_mutex_t;
typedef pthread_cond_t ano_cond_t;
typedef void 		*(*ano_thread_start) (void *);

#define ano_thread_create(thread,start,arg)	pthread_create(&thread, NULL, start, arg)
#define ano_thread_self()				pthread_self()
#define ano_thread_detach(thread)		pthread_detach(thread)

#define ano_mutex_lock(mutex)			pthread_mutex_lock(&mutex)
#define ano_mutex_unlock(mutex)			pthread_mutex_unlock(&mutex)

#define ano_cond_wait(cond, mutex)		pthread_cond_wait(&cond, &mutex)
#define ano_cond_signal(cond)			pthread_cond_signal(&cond)

#define ano_cleanup_push(func, arg)		pthread_cleanup_push(func, arg)
#define ano_cleanup_pop(execute)		pthread_cleanup_pop(execute)

#define ano_thread_cancel(thread)		pthread_cancel(thread)

#endif

#endif
