/* Session Limiting functions.
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */

#include "services.h"
#include "pseudo.h"

/*************************************************************************/

/* SESSION LIMITING
 *
 * The basic idea of session limiting is to prevent one host from having more
 * than a specified number of sessions (client connections/clones) on the
 * network at any one time. To do this we have a list of sessions and
 * exceptions. Each session structure records information about a single host,
 * including how many clients (sessions) that host has on the network. When a
 * host reaches it's session limit, no more clients from that host will be
 * allowed to connect.
 *
 * When a client connects to the network, we check to see if their host has
 * reached the default session limit per host, and thus whether it is allowed
 * any more. If it has reached the limit, we kill the connecting client; all
 * the other clients are left alone. Otherwise we simply increment the counter
 * within the session structure. When a client disconnects, we decrement the
 * counter. When the counter reaches 0, we free the session.
 *
 * Exceptions allow one to specify custom session limits for a specific host
 * or a range thereof. The first exception that the host matches is the one
 * used.
 *
 * "Session Limiting" is likely to slow down services when there are frequent
 * client connects and disconnects. The size of the exception list can also
 * play a large role in this performance decrease. It is therefore recommened
 * that you keep the number of exceptions to a minimum. A very simple hashing
 * method is currently used to store the list of sessions. I'm sure there is
 * room for improvement and optimisation of this, along with the storage of
 * exceptions. Comments and suggestions are more than welcome!
 *
 * -TheShadow (02 April 1999)
 */

/*************************************************************************/

/* I'm sure there is a better way to hash the list of hosts for which we are
 * storing session information. This should be sufficient for the mean time.
 * -TheShadow */

#define HASH(host)	  (((host)[0]&31)<<5 | ((host)[1]&31))

Session *sessionlist[1024];
int32 nsessions = 0;

Exception *exceptions = NULL;
int16 nexceptions = 0;

/*************************************************************************/
/****************************** Statistics *******************************/
/*************************************************************************/

void get_session_stats(long *nrec, long *memuse)
{
	Session *session;
	long mem;
	int i;

	mem = sizeof(Session) * nsessions;
	for (i = 0; i < 1024; i++) {
		for (session = sessionlist[i]; session; session = session->next) {
			mem += strlen(session->host) + 1;
		}
	}

	*nrec = nsessions;
	*memuse = mem;
}

void get_exception_stats(long *nrec, long *memuse)
{
	long mem;
	int i;

	mem = sizeof(Exception) * nexceptions;
	for (i = 0; i < nexceptions; i++) {
		mem += strlen(exceptions[i].mask) + 1;
		mem += strlen(exceptions[i].reason) + 1;
	}
	*nrec = nexceptions;
	*memuse = mem;
}

/*************************************************************************/
/********************* Internal Session Functions ************************/
/*************************************************************************/

Session *findsession(const char *host)
{
	Session *session;
	int i;

	if (!host) {
		return NULL;
	}

	for (i = 0; i < 1024; i++) {
		for (session = sessionlist[i]; session; session = session->next) {
			if (stricmp(host, session->host) == 0) {
				return session;
			}
		}
	}

	return NULL;
}

/* Attempt to add a host to the session list. If the addition of the new host
 * causes the the session limit to be exceeded, kill the connecting user.
 * Returns 1 if the host was added or 0 if the user was killed.
 */

int add_session(const char *nick, const char *host, char *hostip)
{
	Session *session, **list;
	Exception *exception;
	int sessionlimit = 0;

	session = findsession(host);

	if (session) {
		exception = find_hostip_exception(host, hostip);

		sessionlimit = exception ? exception->limit : Config.DefSessionLimit;

		if (sessionlimit != 0 && session->count >= sessionlimit) {
			if (Config.SessionLimitExceeded)
				ircdproto->SendMessage(findbot(Config.s_OperServ), nick, Config.SessionLimitExceeded, host);
			if (Config.SessionLimitDetailsLoc)
				ircdproto->SendMessage(findbot(Config.s_OperServ), nick, "%s", Config.SessionLimitDetailsLoc);

			/* Previously on IRCds that send a QUIT (InspIRCD) when a user is killed, the session for a host was
			 * decremented in do_quit, which caused problems and fixed here
			 *
			 * Now, we create the user struture before calling this (to fix some user tracking issues..
			 * read users.c), so we must increment this here no matter what because it will either be
			 * decremented in do_kill or in do_quit - Adam
			 */
			session->count++;
			kill_user(Config.s_OperServ, nick, "Session limit exceeded");

			session->hits++;
			if (Config.MaxSessionKill && session->hits >= Config.MaxSessionKill) {
				char akillmask[BUFSIZE];
				snprintf(akillmask, sizeof(akillmask), "*@%s", host);
				add_akill(NULL, akillmask, Config.s_OperServ,
						  time(NULL) + Config.SessionAutoKillExpiry,
						  "Session limit exceeded");
				ircdproto->SendGlobops(findbot(Config.s_OperServ),
								 "Added a temporary AKILL for \2%s\2 due to excessive connections",
								 akillmask);
			}
			return 0;
		} else {
			session->count++;
			return 1;
		}
	}

	nsessions++;
	session = new Session;
	session->host = sstrdup(host);
	list = &sessionlist[HASH(session->host)];
	session->prev = NULL;
	session->next = *list;
	if (*list)
		(*list)->prev = session;
	*list = session;
	session->count = 1;
	session->hits = 0;

	return 1;
}

void del_session(const char *host)
{
	Session *session;

	if (!Config.LimitSessions) {
		if (debug) {
			alog("debug: del_session called when LimitSessions is disabled");
		}
		return;
	}

	if (!host || !*host) {
		if (debug) {
			alog("debug: del_session called with NULL values");
		}
		return;
	}

	if (debug >= 2)
		alog("debug: del_session() called");

	session = findsession(host);

	if (!session) {
		if (debug) {
			ircdproto->SendGlobops(findbot(Config.s_OperServ),
							 "WARNING: Tried to delete non-existant session: \2%s",
							 host);
			alog("session: Tried to delete non-existant session: %s",
				 host);
		}
		return;
	}

	if (session->count > 1) {
		session->count--;
		return;
	}

	if (session->prev)
		session->prev->next = session->next;
	else
		sessionlist[HASH(session->host)] = session->next;
	if (session->next)
		session->next->prev = session->prev;

	if (debug >= 2)
		alog("debug: del_session(): free session structure");

	delete [] session->host;
	delete session;

	nsessions--;

	if (debug >= 2)
		alog("debug: del_session() done");
}


/*************************************************************************/
/********************** Internal Exception Functions *********************/
/*************************************************************************/

void expire_exceptions()
{
	int i;
	time_t now = time(NULL);

	for (i = 0; i < nexceptions; i++) {
		if (exceptions[i].expires == 0 || exceptions[i].expires > now)
			continue;
		if (Config.WallExceptionExpire)
			ircdproto->SendGlobops(findbot(Config.s_OperServ),
							 "Session limit exception for %s has expired.",
							 exceptions[i].mask);
		delete [] exceptions[i].mask;
		delete [] exceptions[i].reason;
		nexceptions--;
		memmove(exceptions + i, exceptions + i + 1,
				sizeof(Exception) * (nexceptions - i));
		exceptions = static_cast<Exception *>(srealloc(exceptions, sizeof(Exception) * nexceptions));
		i--;
	}
}

/* Find the first exception this host matches and return it. */
Exception *find_host_exception(const char *host)
{
	int i;

	for (i = 0; i < nexceptions; i++) {
		if ((Anope::Match(host, exceptions[i].mask, false))) {
			return &exceptions[i];
		}
	}

	return NULL;
}

/* Same as find_host_exception() except
 * this tries to find the exception by IP also. */
Exception *find_hostip_exception(const char *host, const char *hostip)
{
	int i;

	for (i = 0; i < nexceptions; i++) {
		if ((Anope::Match(host, exceptions[i].mask, false))
			|| ((ircd->nickip && hostip)
				&& (Anope::Match(hostip, exceptions[i].mask, false)))) {
			return &exceptions[i];
		}
	}

	return NULL;
}


/*************************************************************************/
/************************ Exception Manipulation *************************/
/*************************************************************************/

int exception_add(User * u, const char *mask, const int limit,
				  const char *reason, const char *who,
				  const time_t expires)
{
	int i;

	/* Check if an exception already exists for this mask */
	for (i = 0; i < nexceptions; i++) {
		if (!stricmp(mask, exceptions[i].mask)) {
			if (exceptions[i].limit != limit) {
				exceptions[i].limit = limit;
				if (u)
					notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_CHANGED,
								mask, exceptions[i].limit);
				return -2;
			} else {
				if (u)
					notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_EXISTS,
								mask);
				return -1;
			}
		}
	}

	nexceptions++;
	exceptions = static_cast<Exception *>(srealloc(exceptions, sizeof(Exception) * nexceptions));

	exceptions[nexceptions - 1].mask = sstrdup(mask);
	exceptions[nexceptions - 1].limit = limit;
	exceptions[nexceptions - 1].reason = sstrdup(reason);
	exceptions[nexceptions - 1].time = time(NULL);
	exceptions[nexceptions - 1].who = who;
	exceptions[nexceptions - 1].expires = expires;
	exceptions[nexceptions - 1].num = nexceptions - 1;

	return 1;
}
