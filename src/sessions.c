/* Session Limiting functions.
 *
 * (C) 2003-2005 Anope Team
 * Contact us at info@anope.org
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
 *
 * The whole exception system has been rewritten to use linked list instead of
 * an array due to a bug we couldn't track down. We don't need the numerical
 * system anymore, it's not necessary to delete ranges.
 *
 * -Certus (04 July 2006)
 */

/*************************************************************************/

/* I'm sure there is a better way to hash the list of hosts for which we are
 * storing session information. This should be sufficient for the mean time.
 * -TheShadow */

#define HASH(host)      (((host)[0]&31)<<5 | ((host)[1]&31))

Session *sessionlist[1024];
int32 nsessions = 0;

Exception *exceptionlist[1024];
int16 nexceptions = 0;

/*************************************************************************/
/************************ Exception Manipulation *************************/
/*************************************************************************/

Exception *exception_add(User * u, const char *mask, const int limit,
                  const char *reason, const char *who,
                  const time_t expires)
{
    int i, index;
    Exception *exception, *ptr, *prev;

    /* Check if an exception already exists for this mask */
    for (i = 0; i < 1024; i++) {
        for (exception = exceptionlist[i]; exception; exception = exception->next) {
            if (!stricmp(mask, exception->mask)) {
                if (exception->limit != limit) {
                    exception->limit = limit;
                    if (u)
                        notice_lang(s_OperServ, u, OPER_EXCEPTION_CHANGED,
                                    mask, exception->limit);
                    return NULL;
                } else {
                    if (u)
                        notice_lang(s_OperServ, u, OPER_EXCEPTION_EXISTS,
                                    mask);
                    return NULL;
                }
            }
        }
    }

    nexceptions++;
    exception = scalloc(sizeof(Exception), 1);

    exception->mask = sstrdup(mask);
    exception->limit = limit;
    exception->reason = sstrdup(reason);
    exception->time = time(NULL);
    strscpy(exception->who, who, NICKMAX);
    exception->expires = expires;

    index = HASH(mask); /* bang! into the list! */

    exception->prev = NULL;
    exception->next = exceptionlist[index];
    if (exception->next)
        exception->next->prev = exception;
    exceptionlist[index] = exception;

    return exception;
}

/*************************************************************************/

static int exception_del(Exception *exception)
{
    /* Remove the exception from the list */
    if (exception->next)
        exception->next->prev = exception->prev;
    if (exception->prev)
        exception->prev->next = exception->next;
    else
        exceptionlist[HASH(exception->mask)] = exception->next;

    free(exception->mask);
    free(exception->reason);
    free(exception);
    nexceptions--;
    
    return 1;
}

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
    Exception *exception;
    long mem;
    int i;

    mem = sizeof(Exception) * nexceptions;
    for (i = 0; i < 1024; i++) {
        for (exception = exceptionlist[i]; exception; exception = exception->next) {
            mem += strlen(exception->mask) + 1;
            mem += strlen(exception->reason) + 1;
        }
    }

    *nrec = nexceptions;
    *memuse = mem;
}

/*************************************************************************/
/************************* Session List Display **************************/
/*************************************************************************/

/* Syntax: SESSION LIST threshold
 *	Lists all sessions with atleast threshold clients.
 *	The threshold value must be greater than 1. This is to prevent 
 * 	accidental listing of the large number of single client sessions.
 *
 * Syntax: SESSION VIEW host
 *	Displays detailed session information about the supplied host.
 */

int do_session(User * u)
{
    Session *session;
    Exception *exception;
    char *cmd = strtok(NULL, " ");
    char *param1 = strtok(NULL, " ");
    int mincount;
    int i;

    if (!LimitSessions) {
        notice_lang(s_OperServ, u, OPER_SESSION_DISABLED);
        return MOD_CONT;
    }

    if (!cmd)
        cmd = "";

    if (stricmp(cmd, "LIST") == 0) {
        if (!param1) {
            syntax_error(s_OperServ, u, "SESSION",
                         OPER_SESSION_LIST_SYNTAX);

        } else if ((mincount = atoi(param1)) <= 1) {
            notice_lang(s_OperServ, u, OPER_SESSION_INVALID_THRESHOLD);

        } else {
            notice_lang(s_OperServ, u, OPER_SESSION_LIST_HEADER, mincount);
            notice_lang(s_OperServ, u, OPER_SESSION_LIST_COLHEAD);
            for (i = 0; i < 1024; i++) {
                for (session = sessionlist[i]; session;
                     session = session->next) {
                    if (session->count >= mincount)
                        notice_lang(s_OperServ, u,
                                    OPER_SESSION_LIST_FORMAT,
                                    session->count, session->host);
                }
            }
        }
    } else if (stricmp(cmd, "VIEW") == 0) {
        if (!param1) {
            syntax_error(s_OperServ, u, "SESSION",
                         OPER_SESSION_VIEW_SYNTAX);

        } else {
            session = findsession(param1);
            if (!session) {
                notice_lang(s_OperServ, u, OPER_SESSION_NOT_FOUND, param1);
            } else {
                exception = find_host_exception(param1);

                notice_lang(s_OperServ, u, OPER_SESSION_VIEW_FORMAT,
                            param1, session->count,
                            exception ? exception->
                            limit : DefSessionLimit);
            }
        }

    } else {
        syntax_error(s_OperServ, u, "SESSION", OPER_SESSION_SYNTAX);
    }
    return MOD_CONT;
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

int add_session(char *nick, char *host)
{
    Session *session, **list;
    Exception *exception;
    int sessionlimit = 0;

    session = findsession(host);

    if (session) {
        exception = find_host_exception(host);
        if (checkDefCon(DEFCON_REDUCE_SESSION)) {
            sessionlimit =
                exception ? exception->limit : DefConSessionLimit;
        } else {
            sessionlimit = exception ? exception->limit : DefSessionLimit;
        }

        if (sessionlimit != 0 && session->count >= sessionlimit) {
            if (SessionLimitExceeded)
                notice(s_OperServ, nick, SessionLimitExceeded, host);
            if (SessionLimitDetailsLoc)
                notice(s_OperServ, nick, "%s", SessionLimitDetailsLoc);

            /* We don't use kill_user() because a user stucture has not yet
             * been created. Simply kill the user. -TheShadow
             */
            kill_user(s_OperServ, nick, "Session limit exceeded");

            session->hits++;
            if (MaxSessionKill && session->hits >= MaxSessionKill) {
                char akillmask[BUFSIZE];
                snprintf(akillmask, sizeof(akillmask), "*@%s", host);
                add_akill(NULL, akillmask, s_OperServ,
                          time(NULL) + SessionAutoKillExpiry,
                          "Session limit exceeded");
                anope_cmd_global(s_OperServ,
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
    session = scalloc(sizeof(Session), 1);
    session->host = sstrdup(host);
    list = &sessionlist[HASH(session->host)];
    session->next = *list;
    if (*list)
        (*list)->prev = session;
    *list = session;
    session->count = 1;

    return 1;
}

void del_session(const char *host)
{
    Session *session;

    if (!LimitSessions) {
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
            anope_cmd_global(s_OperServ,
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

    free(session->host);
    free(session);

    nsessions--;

    if (debug >= 2)
        alog("debug: del_session() done");
}


/*************************************************************************/
/********************** Internal Exception Functions *********************/
/*************************************************************************/

void expire_exceptions(void)
{
    int i;
    Exception *exception, *tmpexc;
    time_t now = time(NULL);

    for (i = 0; i < 1024; i++) {
        for (exception = exceptionlist[i]; exception; exception = tmpexc) {
            tmpexc = exception->next;
            if (exception->expires == 0 || exception->expires > now)
                continue;
            if (WallExceptionExpire)
                anope_cmd_global(s_OperServ,
                                 "Session limit exception for %s has expired.",
                                 exception->mask);
            exception_del(exception);
        }
    }
}

/* Find the first exception this host matches and return it. */

Exception *find_host_exception(const char *host)
{
    int i;
    Exception *exception;

    for (i = 0; i < 1024; i++) {
        for (exception = exceptionlist[i]; exception; exception = exception->next) {
            if (match_wild_nocase(exception->mask, host))
                return exception;
        }
    }

    return NULL;
}

/*************************************************************************/
/*********************** Exception Load/Save *****************************/
/*************************************************************************/

#define SAFE(x) do {                                    \
    if ((x) < 0) {                                      \
        if (!forceload)                                 \
            fatal("Read error on %s", ExceptionDBName); \
        nexceptions = i;                                \
        break;                                          \
    }                                                   \
} while (0)

void load_exceptions()
{
    dbFILE *f;
    int i, index;
    uint16 n;
    uint16 tmp16;
    uint32 tmp32;

    if (!
        (f = open_db(s_OperServ, ExceptionDBName, "r", EXCEPTION_VERSION)))
        return;
    switch (i = get_file_version(f)) {
    case 9:
    case 8:
    case 7:
        SAFE(read_int16(&n, f));
        nexceptions = n;
        if (!nexceptions) {
            close_db(f);
            return;
        }
        for (i = 0; i < nexceptions; i++) {

            Exception *exception = scalloc(sizeof(Exception), 1);

            SAFE(read_string(&exception->mask, f));
            SAFE(read_int16(&tmp16, f));
            exception->limit = tmp16;
            SAFE(read_buffer(exception->who, f));
            SAFE(read_string(&exception->reason, f));
            SAFE(read_int32(&tmp32, f));
            exception->time = tmp32;
            SAFE(read_int32(&tmp32, f));
            exception->expires = tmp32;

            index = HASH(exception->mask);

            exception->prev = NULL;
            exception->next = exceptionlist[index];
            if (exception->next)
                exception->next->prev = exception;
            exceptionlist[index] = exception;
        }
        break;

    default:
        fatal("Unsupported version (%d) on %s", i, ExceptionDBName);
    }                           /* switch (ver) */

    close_db(f);
}

#undef SAFE

/*************************************************************************/

#define SAFE(x) do {                                            \
    if ((x) < 0) {                                              \
        restore_db(f);                                          \
        log_perror("Write error on %s", ExceptionDBName);       \
        if (time(NULL) - lastwarn > WarningTimeout) {           \
            anope_cmd_global(NULL, "Write error on %s: %s", ExceptionDBName,  \
                        strerror(errno));                       \
            lastwarn = time(NULL);                              \
        }                                                       \
        return;                                                 \
    }                                                           \
} while (0)

void save_exceptions()
{
    dbFILE *f;
    int i;
    Exception *exception;
    static time_t lastwarn = 0;

    if (!
        (f = open_db(s_OperServ, ExceptionDBName, "w", EXCEPTION_VERSION)))
        return;
    SAFE(write_int16(nexceptions, f));
    for (i = 0; i < 1024; i++) {
        for (exception = exceptionlist[i]; exception; exception = exception->next) {
            SAFE(write_string(exception->mask, f));
            SAFE(write_int16(exception->limit, f));
            SAFE(write_buffer(exception->who, f));
            SAFE(write_string(exception->reason, f));
            SAFE(write_int32(exception->time, f));
            SAFE(write_int32(exception->expires, f));
        }
    }
    close_db(f);
}

#undef SAFE

/*************************************************************************/

void save_rdb_exceptions()
{
#ifdef USE_RDB
    int i;
    Exception *exception;

    if (!rdb_open())
        return;
    rdb_clear_table("anope_os_exceptions");
    for (i = 0; i < 1024; i++) {
        for (exception = exceptionlist[i]; exception; exception = exception->next) {
            rdb_save_exceptions(exception);
        }
    }
    rdb_close();
#endif
}


/*************************************************************************/

/* Syntax: EXCEPTION ADD [+expiry] mask limit reason
 *	Adds mask to the exception list with limit as the maximum session
 *	limit and +expiry as an optional expiry time.
 *
 * Syntax: EXCEPTION DEL mask
 *	Deletes the first exception that matches mask exactly.
 *
 * Syntax: EXCEPTION LIST [mask]
 *	Lists all exceptions or those matching mask.
 *
 * Syntax: EXCEPTION VIEW [mask]
 *	Displays detailed information about each exception or those matching
 *	mask.
 */

int do_exception(User * u)
{
    char *cmd = strtok(NULL, " ");
    char *mask, *reason, *expiry, *limitstr;
    int limit, expires;
    int i;
    int x;

    if (!LimitSessions) {
        notice_lang(s_OperServ, u, OPER_EXCEPTION_DISABLED);
        return MOD_CONT;
    }

    if (!cmd)
        cmd = "";

    if (stricmp(cmd, "ADD") == 0) {
        /* gotta keep this check because of the dbs :( -Certus */
        if (nexceptions >= 32767) {
            notice_lang(s_OperServ, u, OPER_EXCEPTION_TOO_MANY);
            return MOD_CONT;
        }
        mask = strtok(NULL, " ");
        if (mask && *mask == '+') {
            expiry = mask;
            mask = strtok(NULL, " ");
        } else {
            expiry = NULL;
        }
        limitstr = strtok(NULL, " ");
        reason = strtok(NULL, "");

        if (!reason) {
            syntax_error(s_OperServ, u, "EXCEPTION",
                         OPER_EXCEPTION_ADD_SYNTAX);
            return MOD_CONT;
        }

        expires = expiry ? dotime(expiry) : ExceptionExpiry;
        if (expires < 0) {
            notice_lang(s_OperServ, u, BAD_EXPIRY_TIME);
            return MOD_CONT;
        } else if (expires > 0) {
            expires += time(NULL);
        }

        limit = (limitstr && isdigit(*limitstr)) ? atoi(limitstr) : -1;

        if (limit < 0 || limit > MaxSessionLimit) {
            notice_lang(s_OperServ, u, OPER_EXCEPTION_INVALID_LIMIT,
                        MaxSessionLimit);
            return MOD_CONT;

        } else { /* finally when can add it */
            Exception *exception;

            if (strchr(mask, '!') || strchr(mask, '@')) {
                notice_lang(s_OperServ, u,
                            OPER_EXCEPTION_INVALID_HOSTMASK);
                return MOD_CONT;
            }

            exception = exception_add(u, mask, limit, reason, u->nick, expires);

            if (exception)
                notice_lang(s_OperServ, u, OPER_EXCEPTION_ADDED, mask, limit);

            if (readonly)
                notice_lang(s_OperServ, u, READ_ONLY_MODE);
        }
    } else if (stricmp(cmd, "DEL") == 0) {
        Exception *exception;
        int deleted = 0;

        mask = strtok(NULL, " ");

        if (!mask) {
            syntax_error(s_OperServ, u, "EXCEPTION",
                         OPER_EXCEPTION_DEL_SYNTAX);
            return MOD_CONT;
        }

        for (i = 0; i < 1024; i++) {
            for (exception = exceptionlist[i]; exception; exception = exception->next) {
                if (!stricmp(mask, exception->mask)) {
                    exception_del(exception);
                    notice_lang(s_OperServ, u, OPER_EXCEPTION_DELETED, mask);
                    deleted = 1;
                    break;
                }
            }
        }

        if (!deleted)
            notice_lang(s_OperServ, u, OPER_EXCEPTION_NOT_FOUND, mask);

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);


    } else if (stricmp(cmd, "LIST") == 0) {
        int sent_header = 0;
        Exception *exception;

        expire_exceptions();
        mask = strtok(NULL, " ");

        for (i = 0; i < 1024; i++) {
            for (exception = exceptionlist[i]; exception; exception = exception->next) {
                if (!mask || match_wild_nocase(mask, exception->mask)) {
                    if (!sent_header) {
                        notice_lang(s_OperServ, u, OPER_EXCEPTION_LIST_HEADER);
                        notice_lang(s_OperServ, u, OPER_EXCEPTION_LIST_COLHEAD);
                        sent_header = 1;
                    }
                    notice_lang(s_OperServ, u, OPER_EXCEPTION_LIST_FORMAT,
                        exception->limit, exception->mask);
                }
            }
        }

        if (!sent_header)
            notice_lang(s_OperServ, u, OPER_EXCEPTION_NO_MATCH);

    } else if (stricmp(cmd, "VIEW") == 0) {
        int sent_header = 0;
        Exception *exception;
        char timebuf[32], expirebuf[256];
        struct tm tm;
        time_t t = time(NULL);

        expire_exceptions();
        mask = strtok(NULL, " ");

        for (i = 0; i < 1024; i++) {
            for (exception = exceptionlist[i]; exception; exception = exception->next) {
                if (!mask || match_wild_nocase(mask, exception->mask)) {
                    if (!sent_header) {
                        notice_lang(s_OperServ, u, OPER_EXCEPTION_LIST_HEADER);
                        sent_header = 1;
                    }
                    tm = *localtime(exception->time ? &exception->time : &t);
                    strftime_lang(timebuf, sizeof(timebuf), u,
                        STRFTIME_SHORT_DATE_FORMAT, &tm);
                    expire_left(u->na, expirebuf, sizeof(expirebuf), exception->expires);
                    notice_lang(s_OperServ, u, OPER_EXCEPTION_VIEW_FORMAT, exception->mask,
                        exception->who ? exception->who : "<unknown>", timebuf, expirebuf,
                        exception->limit, exception->reason);
                }
            }
        }

        if (!sent_header)
            notice_lang(s_OperServ, u, OPER_EXCEPTION_NO_MATCH);

    } else {
        syntax_error(s_OperServ, u, "EXCEPTION", OPER_EXCEPTION_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/
