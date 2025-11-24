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

#define OPERSERV_SESSION_EXCEPTION_TYPE "Exception"
#define OPERSERV_SESSION_SERVICE "OperServ::SessionService"

namespace OperServ
{
	struct Exception;
	struct Session;
	class SessionService;

	using ExceptionVector = std::vector<Exception *>;
	using SessionMap = std::unordered_map<cidr, Session *, cidr::hash>;

	ServiceReference<SessionService> session_service(OPERSERV_SESSION_SERVICE, OPERSERV_SESSION_SERVICE);
}

struct OperServ::Session final
{
	cidr addr;                      /* A cidr (sockaddrs + len) representing this session */
	unsigned count = 1;             /* Number of clients with this host */
	unsigned hits = 0;              /* Number of subsequent kills for a host */

	Session(const sockaddrs &ip, int len)
		: addr(ip, len)
	{
	}
};

struct OperServ::Exception final
	: Serializable
{
	Anope::string mask;		/* Hosts to which this exception applies */
	unsigned limit;			/* Session limit for exception */
	Anope::string who;		/* Nick of person who added the exception */
	Anope::string reason;		/* Reason for exception's addition */
	time_t time;			/* When this exception was added */
	time_t expires;			/* Time when it expires. 0 == no expiry */

	Exception()
		: Serializable(OPERSERV_SESSION_EXCEPTION_TYPE)
	{
	}
};

class OperServ::SessionService
	: public Service
{
public:
	SessionService(Module *m)
		: Service(m, OPERSERV_SESSION_SERVICE, OPERSERV_SESSION_SERVICE)
	{
	}

	virtual OperServ::Exception *CreateException() = 0;

	virtual void AddException(OperServ::Exception *e) = 0;

	virtual void DelException(OperServ::Exception *e) = 0;

	virtual OperServ::Exception *FindException(User *u) = 0;

	virtual OperServ::Exception *FindException(const Anope::string &host) = 0;

	virtual OperServ::ExceptionVector &GetExceptions() = 0;

	virtual OperServ::Session *FindSession(const Anope::string &ip) = 0;

	virtual OperServ::SessionMap &GetSessions() = 0;
};
