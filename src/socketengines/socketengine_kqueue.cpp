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
#include "socketengine.h"
#include "sockets.h"
#include "logger.h"
#include "config.h"

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

static int kq_fd, max_fds;
static struct kevent *change_events, *event_events;
static int change_count;

static struct kevent *GetChangeEvent()
{
	if (change_count == max_fds)
	{
		timespec zero_timespec = { 0, 0 };
		for (int i = 0; i < change_count; ++i)
			kevent(kq_fd, &change_events[i], 1, NULL, 0, &zero_timespec);
		change_count = 0;
	}

	return &change_events[change_count++];
}

void SocketEngine::Init()
{
	kq_fd = kqueue();
	max_fds = getdtablesize();

	if (kq_fd < 0)
		throw SocketException("Unable to create kqueue engine: " + Anope::LastError());
	else if (max_fds <= 0)
		throw SocketException("Can't determine maximum number of open sockets");

	change_events = new struct kevent[max_fds];
	event_events = new struct kevent[max_fds];

	change_count = 0;
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

	delete [] change_events;
	delete [] event_events;
}

void SocketEngine::AddSocket(Socket *s)
{
	struct kevent *event = GetChangeEvent();
	EV_SET(event, s->GetFD(), EVFILT_READ, EV_ADD, 0, 0, NULL);

	Sockets.insert(std::make_pair(s->GetFD(), s));
}

void SocketEngine::DelSocket(Socket *s)
{
	struct kevent *event = GetChangeEvent();
	EV_SET(event, s->GetFD(), EVFILT_READ, EV_DELETE, 0, 0, NULL);

	event = GetChangeEvent();
	EV_SET(event, s->GetFD(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);

	Sockets.erase(s->GetFD());
}

void SocketEngine::MarkWritable(Socket *s)
{
	if (s->HasFlag(SF_WRITABLE))
		return;

	struct kevent *event = GetChangeEvent();
	EV_SET(event, s->GetFD(), EVFILT_WRITE, EV_ADD, 0, 0, NULL);

	s->SetFlag(SF_WRITABLE);
}

void SocketEngine::ClearWritable(Socket *s)
{
	if (!s->HasFlag(SF_WRITABLE))
		return;

	struct kevent *event = GetChangeEvent();
	EV_SET(event, s->GetFD(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);

	s->UnsetFlag(SF_WRITABLE);
}

void SocketEngine::Process()
{
	static timespec kq_timespec = { Config->ReadTimeout, 0 };
	int total = kevent(kq_fd, change_events, change_count, event_events, max_fds, &kq_timespec);
	change_count = 0;
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
		struct kevent *event = &event_events[i];
		if (event->flags & EV_ERROR)
			continue;

		std::map<int, Socket *>::iterator it = Sockets.find(event->ident);
		if (it == Sockets.end())
			continue;
		Socket *s = it->second;

		if (event->flags & EV_EOF)
		{
			s->ProcessError();
			s->SetFlag(SF_DEAD);
			delete s;
			continue;
		}

		if (!s->Process())
			continue;

		if (event->filter == EVFILT_READ && !s->ProcessRead())
			s->SetFlag(SF_DEAD);
		else if (event->filter == EVFILT_WRITE && !s->ProcessWrite())
			s->SetFlag(SF_DEAD);

		if (s->HasFlag(SF_DEAD))
			delete s;
	}
}

