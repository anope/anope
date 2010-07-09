#include "module.h"
#include <sys/epoll.h>
#include <ulimit.h>

class SocketEngineEPoll : public SocketEngineBase
{
 private:
	long max;
	int EngineHandle;
	epoll_event *events;
	unsigned SocketCount;

 public:
	SocketEngineEPoll()
	{
		SocketCount = 0;
		max = ulimit(4, 0);
		
		if (max <= 0)
		{
			Alog() << "Can't determine maximum number of open sockets";
			throw ModuleException("Can't determine maximum number of open sockets");
		}

		EngineHandle = epoll_create(max / 4);

		if (EngineHandle == -1)
		{
			Alog() << "Could not initialize epoll socket engine: " << strerror(errno);
			throw ModuleException("Could not initialize epoll socket engine: " + std::string(strerror(errno)));
		}

		events = new epoll_event[max];
		memset(events, 0, sizeof(epoll_event) * max);
	}

	~SocketEngineEPoll()
	{
		delete [] events;
	}

	void AddSocket(Socket *s)
	{
		epoll_event ev;
		
		memset(&ev, 0, sizeof(ev));

		ev.events = EPOLLIN | EPOLLOUT;
		ev.data.fd = s->GetSock();

		if (epoll_ctl(EngineHandle, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
		{
			Alog() << "Unable to add fd " << ev.data.fd << " to socketengine epoll: " << strerror(errno);
			return;
		}

		Sockets.insert(std::make_pair(ev.data.fd, s));

		++SocketCount;
	}

	void DelSocket(Socket *s)
	{
		epoll_event ev;

		memset(&ev, 0, sizeof(ev));

		ev.data.fd = s->GetSock();

		if (epoll_ctl(EngineHandle, EPOLL_CTL_DEL, ev.data.fd, &ev) == -1)
		{
			Alog() << "Unable to delete fd " << ev.data.fd << " from socketengine epoll: " << strerror(errno);
			return;
		}

		Sockets.erase(ev.data.fd);

		--SocketCount;
	}

	void Process()
	{
		int total = epoll_wait(EngineHandle, events, max - 1, (Config.ReadTimeout * 1000));

		if (total == -1)
		{
			Alog() << "SockEngine::Process(): error " << strerror(errno);
			return;
		}

		for (int i = 0; i < total; ++i)
		{
			epoll_event *ev = &events[i];
			Socket *s = Sockets[ev->data.fd];

			if (ev->events & (EPOLLHUP | EPOLLERR))
			{
				s->ProcessError();
				s->SetFlag(SF_DEAD);
				continue;
			}

			if (ev->events & EPOLLIN)
			{
				if (!s->ProcessRead())
				{
					s->SetFlag(SF_DEAD);
				}
			}

			if (ev->events & EPOLLOUT)
			{
				if (!s->ProcessWrite())
				{
					s->SetFlag(SF_DEAD);
				}
			}
		}

		for (std::map<int, Socket *>::iterator it = Sockets.begin(), it_end = Sockets.end(); it != it_end;)
		{
			Socket *s = it->second;
			++it;

			if (s->HasFlag(SF_DEAD))
			{
				delete s;
			}
		}
	}
};

class ModuleSocketEngineEPoll : public Module
{
	SocketEngineEPoll *engine;

 public:
	ModuleSocketEngineEPoll(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetPermanent(true);
		this->SetType(SOCKETENGINE);

		engine = new SocketEngineEPoll();
		SocketEngine = engine;
	}

	~ModuleSocketEngineEPoll()
	{
		delete engine;
		SocketEngine = NULL;
	}
};

MODULE_INIT(ModuleSocketEngineEPoll)

