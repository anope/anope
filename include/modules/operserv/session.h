// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

struct Session final
{
	cidr addr;                      /* A cidr (sockaddrs + len) representing this session */
	unsigned count = 1;             /* Number of clients with this host */
	unsigned hits = 0;              /* Number of subsequent kills for a host */

	Session(const sockaddrs &ip, int len) : addr(ip, len) { }
};

struct Exception final
	: Serializable
{
	Anope::string mask;		/* Hosts to which this exception applies */
	unsigned limit;			/* Session limit for exception */
	Anope::string who;		/* Nick of person who added the exception */
	Anope::string reason;		/* Reason for exception's addition */
	time_t time;			/* When this exception was added */
	time_t expires;			/* Time when it expires. 0 == no expiry */

	Exception() : Serializable("Exception") { }
};

class SessionService
	: public Service
{
public:
	typedef std::unordered_map<cidr, Session *, cidr::hash> SessionMap;
	typedef std::vector<Exception *> ExceptionVector;

	SessionService(Module *m) : Service(m, "SessionService", "session") { }

	virtual Exception *CreateException() = 0;

	virtual void AddException(Exception *e) = 0;

	virtual void DelException(Exception *e) = 0;

	virtual Exception *FindException(User *u) = 0;

	virtual Exception *FindException(const Anope::string &host) = 0;

	virtual ExceptionVector &GetExceptions() = 0;

	virtual Session *FindSession(const Anope::string &ip) = 0;

	virtual SessionMap &GetSessions() = 0;
};

static ServiceReference<SessionService> session_service("SessionService", "session");
