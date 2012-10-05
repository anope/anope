/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "anope.h"
#include "sockets.h"
#include "socketengine.h"
#include "config.h"

#include <sys/epoll.h>
#include <ulimit.h>
#include <errno.h>

static long max;
static int EngineHandle;
static epoll_event *events;

void SocketEngine::Init()
{
	max = ulimit(4, 0);

	if (max <= 0)
		throw SocketException("Can't determine maximum number of open sockets");

	EngineHandle = epoll_create(max / 4);

	if (EngineHandle == -1)
		throw SocketException("Could not initialize epoll socket engine: " + Anope::LastError());

	events = new epoll_event[max];
	memset(events, 0, sizeof(epoll_event) * max);
}

void SocketEngine::Shutdown()
{
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
		throw SocketException("Unable to add fd " + stringify(ev.data.fd) + " to epoll: " + Anope::LastError());

	Sockets[ev.data.fd] = s;
}

void SocketEngine::DelSocket(Socket *s)
{
	epoll_event ev;

	memset(&ev, 0, sizeof(ev));

	ev.data.fd = s->GetFD();

	if (epoll_ctl(EngineHandle, EPOLL_CTL_DEL, ev.data.fd, &ev) == -1)
		throw SocketException("Unable to remove fd " + stringify(ev.data.fd) + " from epoll: " + Anope::LastError());

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
		throw SocketException("Unable to mark fd " + stringify(ev.data.fd) + " as writable in epoll: " + Anope::LastError());

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
		throw SocketException("Unable clear mark fd " + stringify(ev.data.fd) + " as writable in epoll: " + Anope::LastError());

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

		std::map<int, Socket *>::iterator it = Sockets.find(ev->data.fd);
		if (it == Sockets.end())
			continue;
		Socket *s = it->second;

		if (ev->events & (EPOLLHUP | EPOLLERR))
		{
			s->ProcessError();
			s->SetFlag(SF_DEAD);
			delete s;
			continue;
		}

		if (!s->Process())
			continue;

		if ((ev->events & EPOLLIN) && !s->ProcessRead())
			s->SetFlag(SF_DEAD);

		if ((ev->events & EPOLLOUT) && !s->ProcessWrite())
			s->SetFlag(SF_DEAD);

		if (s->HasFlag(SF_DEAD))
			delete s;
	}
}

