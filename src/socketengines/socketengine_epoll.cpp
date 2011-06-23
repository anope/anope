#include "module.h"
#include <sys/epoll.h>
#include <ulimit.h>

static long max;
static int EngineHandle;
static epoll_event *events;

void SocketEngine::Init()
{
	max = ulimit(4, 0);

	if (max <= 0)
	{
		Log() << "Can't determine maximum number of open sockets";
		throw CoreException("Can't determine maximum number of open sockets");
	}

	EngineHandle = epoll_create(max / 4);

	if (EngineHandle == -1)
	{
		Log() << "Could not initialize epoll socket engine: " << Anope::LastError();
		throw CoreException(Anope::string("Could not initialize epoll socket engine: ") + Anope::LastError());
	}

	events = new epoll_event[max];
	memset(events, 0, sizeof(epoll_event) * max);
}

void SocketEngine::Shutdown()
{
	Process();

	for (std::map<int, Socket *>::const_iterator it = Sockets.begin(), it_end = Sockets.end(); it != it_end;)
	{
		Socket *s = it->second;
		++it;
		delete s;
	}
	Sockets.clear();

	delete [] events;
}

void SocketEngine::AddSocket(Socket *s)
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
}

void SocketEngine::DelSocket(Socket *s)
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
}

void SocketEngine::MarkWritable(Socket *s)
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

void SocketEngine::ClearWritable(Socket *s)
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

void SocketEngine::Process()
{
	int total = epoll_wait(EngineHandle, events, max - 1, Config->ReadTimeout * 1000);
	Anope::CurTime = time(NULL);

	/* EINTR can be given if the read timeout expires */
	if (total == -1)
	{
		if (errno != EINTR)
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

