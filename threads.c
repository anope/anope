/* Threads handling.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$ 
 *
 */

#include "services.h"

#ifdef USE_THREADS

/*************************************************************************/

static Thread *threads;

static int thread_cancel(Thread * thr);

/*************************************************************************/

static int thread_cancel(Thread * thr)
{
    if (pthread_cancel(thr->th))
        return 0;

    if (thr->next)
        thr->next->prev = thr->prev;
    if (thr->prev)
        thr->prev->next = thr->next;
    else
        threads = thr->next;

    return 1;
}

/*************************************************************************/

int thread_create(pthread_t * th, void *(*start_routine) (void *),
                  void *arg)
{
    Thread *thr;

    if (pthread_create(th, NULL, start_routine, arg))
        return 0;
    if (pthread_detach(*th))
        return 0;

    /* Add the thread to our internal list */
    thr = scalloc(sizeof(Thread), 1);
    thr->th = *th;
    thr->next = threads;
    if (thr->next)
        thr->next->prev = thr;
    threads = thr;

    return 1;
}

/*************************************************************************/

int thread_killall(void)
{
    Thread *thr, *next;

    for (thr = threads; thr; thr = next) {
        next = thr;
        if (!thread_cancel(thr))
            return 0;
    }

    return 1;
}

/*************************************************************************/

#endif
