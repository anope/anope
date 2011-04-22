#ifndef OS_SESSION_H
#define OS_SESSION_H

class SessionService : public Service
{
 public:
	typedef Anope::map<Session *> SessionMap;
	typedef std::vector<Exception *> ExceptionVector;
 private:
	SessionMap Sessions;
	ExceptionVector Exceptions;
 public:
	SessionService(Module *m) : Service(m, "session") { }

	void AddException(Exception *e)
	{
		this->Exceptions.push_back(e);
	}

	void DelException(Exception *e)
	{
		ExceptionVector::iterator it = std::find(this->Exceptions.begin(), this->Exceptions.end(), e);
		if (it != this->Exceptions.end())
			this->Exceptions.erase(it);
	}

	Exception *FindException(User *u)
	{
		for (std::vector<Exception *>::const_iterator it = this->Exceptions.begin(), it_end = this->Exceptions.end(); it != it_end; ++it)
		{
			Exception *e = *it;
			if (Anope::Match(u->host, e->mask) || (u->ip() && Anope::Match(u->ip.addr(), e->mask)))
				return e;
		}
		return NULL;
	}

	Exception *FindException(const Anope::string &host)
	{
		for (std::vector<Exception *>::const_iterator it = this->Exceptions.begin(), it_end = this->Exceptions.end(); it != it_end; ++it)
		{
			Exception *e = *it;
			if (Anope::Match(host, e->mask))
				return e;
		}

		return NULL;
	}

	ExceptionVector &GetExceptions()
	{
		return this->Exceptions;
	}

	void AddSession(Session *s)
	{
		this->Sessions[s->host] = s;
	}

	void DelSession(Session *s)
	{
		this->Sessions.erase(s->host);
	}

	Session *FindSession(const Anope::string &mask)
	{
		SessionMap::iterator it = this->Sessions.find(mask);
		if (it != this->Sessions.end())
			return it->second;
		return NULL;
	}

	SessionMap &GetSessions()
	{
		return this->Sessions;
	}
};

#endif

