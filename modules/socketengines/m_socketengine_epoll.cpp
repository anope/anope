/* RequiredFunctions: epoll_wait */

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
			Log() << "Can't determine maximum number of open sockets";
			throw ModuleException("Can't determine maximum number of open sockets");
		}

		EngineHandle = epoll_create(max / 4);

		if (EngineHandle == -1)
		{
			Log() << "Could not initialize epoll socket engine: " << Anope::LastError();
			throw ModuleException(Anope::string("Could not initialize epoll socket engine: ") + Anope::LastError());
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

		ev.events = EPOLLIN;
		ev.data.fd = s->GetFD();

		if (epoll_ctl(EngineHandle, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
		{
			Log() << "Unable to add fd " << ev.data.fd << " to socketengine epoll: " << Anope::LastError();
			return;
		}

		Sockets.insert(std::make_pair(ev.data.fd, s));

		++SocketCount;
	}

	void DelSocket(Socket *s)
	{
		epoll_event ev;

		memset(&ev, 0, sizeof(ev));

		ev.data.fd = s->GetFD();

		if (epoll_ctl(EngineHandle, EPOLL_CTL_DEL, ev.data.fd, &ev) == -1)
		{
			Log() << "Unable to delete fd " << ev.data.fd << " from socketengine epoll: " << Anope::LastError();
			return;
		}

		Sockets.erase(ev.data.fd);

		--SocketCount;
	}

	void MarkWritable(Socket *s)
	{
		if (s->HasFlag(SF_WRITABLE))
			return;

		epoll_event ev;

		memset(&ev, 0, sizeof(ev));

		ev.events = EPOLLIN | EPOLLOUT;
		ev.data.fd = s->GetFD();

		if (epoll_ctl(EngineHandle, EPOLL_CTL_MOD, ev.data.fd, &ev) == -1)
			Log() << "Unable to mark fd " << ev.data.fd << " as writable in socketengine epoll: " << Anope::LastError();
		else
			s->SetFlag(SF_WRITABLE);
	}

	void ClearWritable(Socket *s)
	{
		if (!s->HasFlag(SF_WRITABLE))
			return;

		epoll_event ev;

		memset(&ev, 0, sizeof(ev));

		ev.events = EPOLLIN;
		ev.data.fd = s->GetFD();

		if (epoll_ctl(EngineHandle, EPOLL_CTL_MOD, ev.data.fd, &ev) == -1)
			Log() << "Unable to mark fd " << ev.data.fd << " as unwritable in socketengine epoll: " << Anope::LastError();
		else
			s->UnsetFlag(SF_WRITABLE);
	}

	void Process()
	{
		int total = epoll_wait(EngineHandle, events, max - 1, Config->ReadTimeout * 1000);
		Anope::CurTime = time(NULL);

		if (total == -1)
		{
			Log() << "SockEngine::Process(): error: " << Anope::LastError();
			return;
		}

		for (int i = 0; i < total; ++i)
		{
			epoll_event *ev = &events[i];
			Socket *s = Sockets[ev->data.fd];

			if (s->HasFlag(SF_DEAD))
				continue;
			if (ev->events & (EPOLLHUP | EPOLLERR))
			{
				s->ProcessError();
				s->SetFlag(SF_DEAD);
				continue;
			}

			if ((ev->events & EPOLLIN) && !s->ProcessRead())
				s->SetFlag(SF_DEAD);

			if ((ev->events & EPOLLOUT) && !s->ProcessWrite())
				s->SetFlag(SF_DEAD);
		}

		for (int i = 0; i < total; ++i)
		{
			epoll_event *ev = &events[i];
			Socket *s = Sockets[ev->data.fd];

			if (s->HasFlag(SF_DEAD))
				delete s;
		}
	}
};

class ModuleSocketEngineEPoll : public Module
{
	SocketEngineEPoll engine;

 public:
	ModuleSocketEngineEPoll(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetPermanent(true);
		this->SetType(SOCKETENGINE);

		SocketEngine = &engine;
	}

	~ModuleSocketEngineEPoll()
	{
		SocketEngine = NULL;
	}
};

MODULE_INIT(ModuleSocketEngineEPoll)
