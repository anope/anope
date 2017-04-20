/*
 * Anope IRC Services
 *
 * Copyright (C) 2013-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#pragma once

struct Session
{
	cidr addr;                      /* A cidr (sockaddrs + len) representing this session */
	unsigned int count = 1;         /* Number of clients with this host */
	unsigned int hits = 0;          /* Number of subsequent kills for a host */

	Session(const sockaddrs &ip, int len) : addr(ip, len) { }
};

class Exception : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	static constexpr const char *NAME = "exception";

	virtual Anope::string GetMask() anope_abstract;
	virtual void SetMask(const Anope::string &) anope_abstract;

	virtual unsigned int GetLimit() anope_abstract;
	virtual void SetLimit(unsigned int) anope_abstract;
	
	virtual Anope::string GetWho() anope_abstract;
	virtual void SetWho(const Anope::string &) anope_abstract;

	virtual Anope::string GetReason() anope_abstract;
	virtual void SetReason(const Anope::string &) anope_abstract;

	virtual time_t GetTime() anope_abstract;
	virtual void SetTime(const time_t &) anope_abstract;

	virtual time_t GetExpires() anope_abstract;
	virtual void SetExpires(const time_t &) anope_abstract;
};

class SessionService : public Service
{
 public:
	static constexpr const char *NAME = "session";
	
 	typedef std::unordered_map<cidr, Session *, cidr::hash> SessionMap;

	SessionService(Module *m) : Service(m, NAME) { }

	virtual Exception *FindException(User *u) anope_abstract;

	virtual Exception *FindException(const Anope::string &host) anope_abstract;

	virtual Session *FindSession(const Anope::string &ip) anope_abstract;

	virtual SessionMap &GetSessions() anope_abstract;
};

namespace Event
{
	struct CoreExport Exception : Events
	{
		static constexpr const char *NAME = "exception";

		using Events::Events;

		/** Called after an exception has been added
		 * @param ex The exception
		 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
		 */
		virtual EventReturn OnExceptionAdd(::Exception *ex) anope_abstract;

		/** Called before an exception is deleted
		 * @param source The source deleting it
		 * @param ex The exceotion
		 */
		virtual void OnExceptionDel(CommandSource &source, ::Exception *ex) anope_abstract;
	};
}

