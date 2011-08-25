#ifndef OS_SESSION_H
#define OS_SESSION_H

class SessionService : public Service<SessionService>
{
 public:
	typedef Anope::map<Session *> SessionMap;
	typedef std::vector<Exception *> ExceptionVector;

	SessionService(Module *m) : Service<SessionService>(m, "session") { }

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

#endif

