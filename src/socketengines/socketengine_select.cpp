#include "module.h"

static int MaxFD;
static fd_set ReadFDs;
static fd_set WriteFDs;

void SocketEngine::Init()
{
	MaxFD = 0;
	FD_ZERO(&ReadFDs);
	FD_ZERO(&WriteFDs);
}

void SocketEngine::Shutdown()
{
	Process();

	for (std::map<int, Socket *>::const_iterator it = Sockets.begin(), it_end = Sockets.end(); it != it_end; ++it)
		delete it->second;
	Sockets.clear();

	MaxFD = 0;
	FD_ZERO(&ReadFDs);
	FD_ZERO(&WriteFDs);
}

void SocketEngine::AddSocket(Socket *s)
{
	if (s->GetFD() > MaxFD)
		MaxFD = s->GetFD();
	FD_SET(s->GetFD(), &ReadFDs);
	Sockets.insert(std::make_pair(s->GetFD(), s));
}

void SocketEngine::DelSocket(Socket *s)
{
	if (s->GetFD() == MaxFD)
		--MaxFD;
	FD_CLR(s->GetFD(), &ReadFDs);
	FD_CLR(s->GetFD(), &WriteFDs);
	Sockets.erase(s->GetFD());
}

void SocketEngine::MarkWritable(Socket *s)
{
	if (s->HasFlag(SF_WRITABLE))
		return;
	FD_SET(s->GetFD(), &WriteFDs);
	s->SetFlag(SF_WRITABLE);
}

void SocketEngine::ClearWritable(Socket *s)
{
	if (!s->HasFlag(SF_WRITABLE))
		return;
	FD_CLR(s->GetFD(), &WriteFDs);
	s->UnsetFlag(SF_WRITABLE);
}

void SocketEngine::Process()
{
	fd_set rfdset = ReadFDs, wfdset = WriteFDs, efdset = ReadFDs;
	timeval tval;
	tval.tv_sec = Config->ReadTimeout;
	tval.tv_usec = 0;

	int sresult = select(MaxFD + 1, &rfdset, &wfdset, &efdset, &tval);
	Anope::CurTime = time(NULL);

	if (sresult == -1)
	{
		Log() << "SockEngine::Process(): error: " << Anope::LastError();
	}
	else if (sresult)
	{
		int processed = 0;
		for (std::map<int, Socket *>::const_iterator it = Sockets.begin(), it_end = Sockets.end(); it != it_end && processed != sresult; ++it)
		{
			Socket *s = it->second;

			if (FD_ISSET(s->GetFD(), &efdset) || FD_ISSET(s->GetFD(), &rfdset) || FD_ISSET(s->GetFD(), &wfdset))
				++processed;
			if (s->HasFlag(SF_DEAD))
				continue;
			if (FD_ISSET(s->GetFD(), &efdset))
			{
				s->ProcessError();
				s->SetFlag(SF_DEAD);
				continue;
			}
			if (FD_ISSET(s->GetFD(), &rfdset) && !s->ProcessRead())
				s->SetFlag(SF_DEAD);
			if (FD_ISSET(s->GetFD(), &wfdset) && !s->ProcessWrite())
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

