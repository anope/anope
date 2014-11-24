
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

static Serialize::TypeReference<Exception> exception("Exception");

class SessionService : public Service
{
 public:
 	typedef std::unordered_map<cidr, Session *, cidr::hash> SessionMap;

	SessionService(Module *m) : Service(m, "SessionService", "session") { }

	virtual Exception *FindException(User *u) anope_abstract;

	virtual Exception *FindException(const Anope::string &host) anope_abstract;

	virtual Session *FindSession(const Anope::string &ip) anope_abstract;

	virtual SessionMap &GetSessions() anope_abstract;
};

static ServiceReference<SessionService> session_service("SessionService", "session");

namespace Event
{
	struct CoreExport Exception : Events
	{
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

