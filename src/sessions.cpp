/* Session Limiting functions.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"

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
 * that you keep the number of exceptions to a minimum. 
 *
 * -TheShadow (02 April 1999)
 */

/*************************************************************************/

session_map SessionList;

std::vector<Exception *> exceptions;

/*************************************************************************/
/****************************** Statistics *******************************/
/*************************************************************************/

void get_session_stats(long &count, long &mem)
{
	count = SessionList.size();
	mem = sizeof(Session) * SessionList.size();

	for (session_map::const_iterator it = SessionList.begin(), it_end = SessionList.end(); it != it_end; ++it)
	{
		Session *session = it->second;

		mem += session->host.length() + 1;
	}
}

void get_exception_stats(long &count, long &mem)
{
	count = exceptions.size();
	mem = sizeof(Exception) * exceptions.size();

	for (std::vector<Exception *>::const_iterator it = exceptions.begin(), it_end = exceptions.end(); it != it_end; ++it)
	{
		Exception *e = *it;
		if (!e->mask.empty())
			mem += e->mask.length() + 1;
		if (!e->reason.empty())
			mem += e->reason.length() + 1;
		if (!e->who.empty())
			mem += e->who.length() + 1;
	}
}

/*************************************************************************/
/********************* Internal Session Functions ************************/
/*************************************************************************/

Session *findsession(const Anope::string &host)
{
	session_map::const_iterator it = SessionList.find(host);

	if (it != SessionList.end())
		return it->second;
	return NULL;
}

/* Attempt to add a host to the session list. If the addition of the new host
 * causes the the session limit to be exceeded, kill the connecting user.
 */

void add_session(const Anope::string &nick, const Anope::string &host, const Anope::string &hostip)
{
	Session *session = findsession(host);

	if (session)
	{
		bool kill = false;
		if (Config->DefSessionLimit && session->count >= Config->DefSessionLimit)
		{
			kill = true;
			Exception *exception = find_hostip_exception(host, hostip);
			if (exception)
			{
				kill = false;
				if (exception->limit && session->count >= exception->limit)
					kill = true;
			}
		}

		if (kill)
		{
			if (!Config->SessionLimitExceeded.empty())
				ircdproto->SendMessage(OperServ, nick, Config->SessionLimitExceeded.c_str(), host.c_str());
			if (!Config->SessionLimitDetailsLoc.empty())
				ircdproto->SendMessage(OperServ, nick, "%s", Config->SessionLimitDetailsLoc.c_str());

			/* Previously on IRCds that send a QUIT (InspIRCD) when a user is killed, the session for a host was
			 * decremented in do_quit, which caused problems and fixed here
			 *
			 * Now, we create the user struture before calling this to fix some user tracking issues,
			 * so we must increment this here no matter what because it will either be
			 * decremented in do_kill or in do_quit - Adam
			 */
			++session->count;
			kill_user(Config->s_OperServ, nick, "Session limit exceeded");

			++session->hits;
			if (Config->MaxSessionKill && session->hits >= Config->MaxSessionKill && SGLine)
			{
				Anope::string akillmask = "*@" + host;
				XLine *x = new XLine(akillmask, Config->s_OperServ, Anope::CurTime + Config->SessionAutoKillExpiry, "Session limit exceeded");
				SGLine->AddXLine(x);
				ircdproto->SendGlobops(OperServ, "Added a temporary AKILL for \2%s\2 due to excessive connections", akillmask.c_str());
			}
		}
		else
		{
			++session->count;
		}
	}
	else
	{
		session = new Session();
		session->host = host;
		session->count = 1;
		session->hits = 0;

		SessionList[session->host] = session;
	}
}

void del_session(const Anope::string &host)
{
	if (!Config->LimitSessions)
	{
		Log(LOG_DEBUG) << "del_session called when LimitSessions is disabled";
		return;
	}

	if (host.empty())
	{
		Log(LOG_DEBUG) << "del_session called with NULL values";
		return;
	}

	Log(LOG_DEBUG_2) << "del_session() called";

	Session *session = findsession(host);

	if (!session)
	{
		if (debug)
		{
			ircdproto->SendGlobops(OperServ, "WARNING: Tried to delete non-existant session: \2%s", host.c_str());
			Log() << "session: Tried to delete non-existant session: " << host;
		}
		return;
	}

	if (session->count > 1)
	{
		--session->count;
		return;
	}

	SessionList.erase(session->host);

	Log(LOG_DEBUG_2) << "del_session(): free session structure";

	delete session;

	Log(LOG_DEBUG_2) << "del_session() done";
}

/*************************************************************************/
/********************** Internal Exception Functions *********************/
/*************************************************************************/

void expire_exceptions()
{
	for (unsigned i = exceptions.size(); i > 0; --i)
	{
		Exception *e = exceptions[i - 1];

		if (!e->expires || e->expires > Anope::CurTime)
			continue;
		if (Config->WallExceptionExpire)
			ircdproto->SendGlobops(OperServ, "Session limit exception for %s has expired.", e->mask.c_str());
		delete e;
		exceptions.erase(exceptions.begin() + i - 1);
	}
}

/* Find the first exception this host matches and return it. */
Exception *find_host_exception(const Anope::string &host)
{
	for (std::vector<Exception *>::const_iterator it = exceptions.begin(), it_end = exceptions.end(); it != it_end; ++it)
	{
		Exception *e = *it;
		if (Anope::Match(host, e->mask))
			return e;
	}

	return NULL;
}

/* Same as find_host_exception() except
 * this tries to find the exception by IP also. */
Exception *find_hostip_exception(const Anope::string &host, const Anope::string &hostip)
{
	for (std::vector<Exception *>::const_iterator it = exceptions.begin(), it_end = exceptions.end(); it != it_end; ++it)
	{
		Exception *e = *it;
		if (Anope::Match(host, e->mask) || (!hostip.empty() && Anope::Match(hostip, e->mask)))
			return e;
	}

	return NULL;
}

/*************************************************************************/
/************************ Exception Manipulation *************************/
/*************************************************************************/

int exception_add(User *u, const Anope::string &mask, unsigned limit, const Anope::string &reason, const Anope::string &who, time_t expires)
{
	/* Check if an exception already exists for this mask */
	for (std::vector<Exception *>::iterator it = exceptions.begin(), it_end = exceptions.end(); it != it_end; ++it)
	{
		Exception *e = *it;
		if (e->mask.equals_ci(mask))
		{
			if (e->limit != limit)
			{
				e->limit = limit;
				if (u)
					u->SendMessage(OperServ, OPER_EXCEPTION_CHANGED, mask.c_str(), e->limit);
				return -2;
			}
			else
			{
				if (u)
					u->SendMessage(OperServ, OPER_EXCEPTION_EXISTS, mask.c_str());
				return -1;
			}
		}
	}

	Exception *exception = new Exception();
	exception->mask = mask;
	exception->limit = limit;
	exception->reason = reason;
	exception->time = Anope::CurTime;
	exception->who = who;
	exception->expires = expires;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnExceptionAdd, OnExceptionAdd(u, exception));
	if (MOD_RESULT == EVENT_STOP)
	{
		delete exception;
		return -3;
	}

	exceptions.push_back(exception);

	return 1;
}
