/* Memory management routines.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id: memory.c,v 1.7 2003/07/20 01:15:49 dane Exp $ 
 *
 */

#include "services.h"

/*************************************************************************/
/*************************************************************************/

/* smalloc, scalloc, srealloc, sstrdup:
 *	Versions of the memory allocation functions which will cause the
 *	program to terminate with an "Out of memory" error if the memory
 *	cannot be allocated.  (Hence, the return value from these functions
 *	is never NULL.)
 */

void *smalloc(long size)
{
    void *buf;

    if (!size) {
        size = 1;
    }
    buf = malloc(size);
    if (!buf)
#if !defined(USE_THREADS) || !defined(LINUX20)
        raise(SIGUSR1);
#else
        abort();
#endif
    return buf;
}

void *scalloc(long elsize, long els)
{
    void *buf;

    if (!elsize || !els) {
        elsize = els = 1;
    }
    buf = calloc(elsize, els);
    if (!buf)
#if !defined(USE_THREADS) || !defined(LINUX20)
        raise(SIGUSR1);
#else
        abort();
#endif
    return buf;
}

void *srealloc(void *oldptr, long newsize)
{
    void *buf;

    if (!newsize) {
        newsize = 1;
    }
    buf = realloc(oldptr, newsize);
    if (!buf)
#if !defined(USE_THREADS) || !defined(LINUX20)
        raise(SIGUSR1);
#else
        abort();
#endif
    return buf;
}

char *sstrdup(const char *s)
{
    char *t = strdup(s);
    if (!t)
#if !defined(USE_THREADS) || !defined(LINUX20)
        raise(SIGUSR1);
#else
        abort();
#endif
    return t;
}

/*************************************************************************/
/*************************************************************************/

/* In the future: malloc() replacements that tell us if we're leaking and
 * maybe do sanity checks too... */

/*************************************************************************/
