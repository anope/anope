/* Time-delay routine include stuff.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <time.h>


/* Definitions for timeouts: */
typedef struct timeout_ Timeout;
struct timeout_ {
    Timeout *next, *prev;
    time_t settime, timeout;
    int repeat;			/* Does this timeout repeat indefinitely? */
    void (*code)(Timeout *);	/* This structure is passed to the code */
    void *data;			/* Can be anything */
};


/* Check the timeout list for any pending actions. */
extern void check_timeouts(void);

/* Add a timeout to the list to be triggered in `delay' seconds.  Any
 * timeout added from within a timeout routine will not be checked during
 * that run through the timeout list.
 */
extern Timeout *add_timeout(int delay, void (*code)(Timeout *), int repeat);

/* Remove a timeout from the list (if it's there). */
extern void del_timeout(Timeout *t);

#ifdef DEBUG_COMMANDS
/* Send the list of timeouts to the given user. */
extern int send_timeout_list(User *u);
#endif


#endif	/* TIMEOUT_H */
