#include "module.h"

class SocketEngineSelect : public SocketEngineBase
{
 private:
 	/* Max Read FD */
	int MaxFD;
	/* Read FDs */
	fd_set ReadFDs;
	/* Write FDs */
	fd_set WriteFDs;

 public:
	SocketEngineSelect()
	{
		MaxFD = 0;
		FD_ZERO(&ReadFDs);
		FD_ZERO(&WriteFDs);
	}

	~SocketEngineSelect()
	{
		FD_ZERO(&ReadFDs);
		FD_ZERO(&WriteFDs);
	}

	void AddSocket(Socket *s)
	{
		if (s->GetSock() > MaxFD)
			MaxFD = s->GetSock();
		FD_SET(s->GetSock(), &ReadFDs);
		Sockets.insert(std::make_pair(s->GetSock(), s));
	}

	void DelSocket(Socket *s)
	{
		if (s->GetSock() == MaxFD)
			--MaxFD;
		FD_CLR(s->GetSock(), &ReadFDs);
		FD_CLR(s->GetSock(), &WriteFDs);
		Sockets.erase(s->GetSock());
	}

	void MarkWriteable(Socket *s)
	{
		FD_SET(s->GetSock(), &WriteFDs);
	}

	void ClearWriteable(Socket *s)
	{
		FD_CLR(s->GetSock(), &WriteFDs);
	}

	void Process()
	{
		fd_set rfdset = ReadFDs, wfdset = WriteFDs, efdset = ReadFDs;
		timeval tval;
		tval.tv_sec = Config->ReadTimeout;
		tval.tv_usec = 0;

		int sresult = select(MaxFD + 1, &rfdset, &wfdset, &efdset, &tval);

		if (sresult == -1)
		{
#ifdef WIN32
			errno = WSAGetLastError();
#endif
			Log() << "SockEngine::Process(): error: " << strerror(errno);
		}
		else if (sresult)
		{
			for (std::map<int, Socket *>::const_iterator it = Sockets.begin(), it_end = Sockets.end(); it != it_end; ++it)
			{
				Socket *s = it->second;

				if (s->HasFlag(SF_DEAD))
					continue;
				if (FD_ISSET(s->GetSock(), &efdset))
				{
					s->ProcessError();
					s->SetFlag(SF_DEAD);
					continue;
				}
				if (FD_ISSET(s->GetSock(), &rfdset) && !s->ProcessRead())
					s->SetFlag(SF_DEAD);
				if (FD_ISSET(s->GetSock(), &wfdset) && !s->ProcessWrite())
					s->SetFlag(SF_DEAD);
			}

			for (std::map<int, Socket *>::iterator it = Sockets.begin(), it_end = Sockets.end(); it != it_end; )
			{
				Socket *s = it->second;
				++it;

				if (s->HasFlag(SF_DEAD))
					delete s;
			}
		}
	}
};

class ModuleSocketEngineSelect : public Module
{
	SocketEngineSelect *engine;

 public:
	ModuleSocketEngineSelect(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetPermanent(true);
		this->SetType(SOCKETENGINE);

		engine = new SocketEngineSelect();
		SocketEngine = engine;
	}

	~ModuleSocketEngineSelect()
	{
		delete engine;
		SocketEngine = NULL;
	}
};

MODULE_INIT(ModuleSocketEngineSelect)
