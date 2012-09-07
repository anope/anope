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

	const Anope::string serialize_name() const anope_override { return "Exception"; }
	Serialize::Data serialize() const anope_override;
	static Serializable* unserialize(Serializable *obj, Serialize::Data &data);
};

class SessionService : public Service
{
 public:
 	typedef std::map<cidr, Session *> SessionMap;
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

Serialize::Data Exception::serialize() const
{
	Serialize::Data data;	

	data["mask"] << this->mask;
	data["limit"] << this->limit;
	data["who"] << this->who;
	data["reason"] << this->reason;
	data["time"] << this->time;
	data["expires"] << this->expires;

	return data;
}

Serializable* Exception::unserialize(Serializable *obj, Serialize::Data &data)
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

