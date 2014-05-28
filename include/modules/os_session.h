#ifndef OS_SESSION_H
#define OS_SESSION_H

struct Session
{
	cidr addr;                      /* A cidr (sockaddrs + len) representing this session */
	unsigned count;                 /* Number of clients with this host */
	unsigned hits;                  /* Number of subsequent kills for a host */

	Session(const Anope::string &ip, int len) : addr(ip, len), count(1), hits(0) { }
};

struct Exception : Serializable
{
	Anope::string mask;		/* Hosts to which this exception applies */
	unsigned limit;			/* Session limit for exception */
	Anope::string who;		/* Nick of person who added the exception */
	Anope::string reason;		/* Reason for exception's addition */
	time_t time;			/* When this exception was added */
	time_t expires;			/* Time when it expires. 0 == no expiry */

	Exception() : Serializable("Exception") { }
	void Serialize(Serialize::Data &data) const override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data);
};

class SessionService : public Service
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

void Exception::Serialize(Serialize::Data &data) const
{
	data["mask"] << this->mask;
	data["limit"] << this->limit;
	data["who"] << this->who;
	data["reason"] << this->reason;
	data["time"] << this->time;
	data["expires"] << this->expires;
}

Serializable* Exception::Unserialize(Serializable *obj, Serialize::Data &data)
{
	if (!session_service)
		return NULL;

	Exception *ex;
	if (obj)
		ex = anope_dynamic_static_cast<Exception *>(obj);
	else
		ex = new Exception;
	data["mask"] >> ex->mask;
	data["limit"] >> ex->limit;
	data["who"] >> ex->who;
	data["reason"] >> ex->reason;
	data["time"] >> ex->time;
	data["expires"] >> ex->expires;

	if (!obj)
		session_service->AddException(ex);
	return ex;
}

#endif

