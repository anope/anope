#ifndef OS_SESSION_H
#define OS_SESSION_H

struct Session
{
	Anope::string host;             /* Host of the session */
	unsigned count;                 /* Number of clients with this host */
	unsigned hits;                  /* Number of subsequent kills for a host */
};

struct Exception : Serializable
{
	Anope::string mask;		/* Hosts to which this exception applies */
	unsigned limit;			/* Session limit for exception */
	Anope::string who;		/* Nick of person who added the exception */
	Anope::string reason;		/* Reason for exception's addition */
	time_t time;			/* When this exception was added */
	time_t expires;			/* Time when it expires. 0 == no expiry */

	Anope::string serialize_name() const anope_override { return "Exception"; }
	serialized_data serialize() anope_override;
	static void unserialize(serialized_data &data);
};

class SessionService : public Service
{
 public:
	typedef Anope::map<Session *> SessionMap;
	typedef std::vector<Exception *> ExceptionVector;

	SessionService(Module *m) : Service(m, "SessionService", "session") { }

	virtual void AddException(Exception *e) = 0;

	virtual void DelException(Exception *e) = 0;

	virtual Exception *FindException(User *u) = 0;

	virtual Exception *FindException(const Anope::string &host) = 0; 

	virtual ExceptionVector &GetExceptions() = 0;

	virtual void AddSession(Session *s) = 0;

	virtual void DelSession(Session *s) = 0;

	virtual Session *FindSession(const Anope::string &mask) = 0;

	virtual SessionMap &GetSessions() = 0;
};

static service_reference<SessionService> session_service("SessionService", "session");

Serializable::serialized_data Exception::serialize()
{
	serialized_data data;	

	data["mask"] << this->mask;
	data["limit"] << this->limit;
	data["who"] << this->who;
	data["reason"] << this->reason;
	data["time"] << this->time;
	data["expires"] << this->expires;

	return data;
}

void Exception::unserialize(Serializable::serialized_data &data)
{
	if (!session_service)
		return;

	Exception *ex = new Exception;
	data["mask"] >> ex->mask;
	data["limit"] >> ex->limit;
	data["who"] >> ex->who;
	data["reason"] >> ex->reason;
	data["time"] >> ex->time;
	data["expires"] >> ex->expires;

	session_service->AddException(ex);
}

#endif

