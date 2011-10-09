#include "module.h"

#ifndef _WIN32
# include <sys/poll.h>
# include <poll.h>
# include <sys/types.h>
# include <sys/time.h>
# include <sys/resource.h>
# ifndef POLLRDHUP
#  define POLLRDHUP 0
# endif
#else
# define poll WSAPoll
# define POLLRDHUP POLLHUP
#endif

static long max;
static pollfd *events;
static int SocketCount;
static std::map<int, int> socket_positions;

void SocketEngine::Init()
{
	SocketCount = 0;

	rlimit fd_limit;
	if (getrlimit(RLIMIT_NOFILE, &fd_limit) == -1)
		throw CoreException(Anope::LastError());
	
	max = fd_limit.rlim_cur;

	events = new pollfd[max];
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
	if (SocketCount == max)
		throw SocketException("Unable to add fd " + stringify(s->GetFD()) + " to poll: " + Anope::LastError());

	pollfd *ev = &events[SocketCount];
	ev->fd = s->GetFD();
	ev->events = POLLIN;
	ev->revents = 0;

	Sockets.insert(std::make_pair(ev->fd, s));
	socket_positions.insert(std::make_pair(ev->fd, SocketCount));

	++SocketCount;
}

void SocketEngine::DelSocket(Socket *s)
{
	std::map<int, int>::iterator pos = socket_positions.find(s->GetFD());
	if (pos == socket_positions.end())
		throw SocketException("Unable to remove fd " + stringify(s->GetFD()) + " from poll");

	if (pos->second != SocketCount - 1)
	{
		pollfd *ev = &events[pos->second],
			*last_ev = &events[SocketCount - 1];

		ev->fd = last_ev->fd;
		ev->events = last_ev->events;
		ev->revents = last_ev->revents;

		socket_positions[ev->fd] = pos->second;
	}

	Sockets.erase(s->GetFD());
	socket_positions.erase(pos);

	--SocketCount;
}

void SocketEngine::MarkWritable(Socket *s)
{
	if (s->HasFlag(SF_WRITABLE))
		return;

	std::map<int, int>::iterator pos = socket_positions.find(s->GetFD());
	if (pos == socket_positions.end())
		throw SocketException("Unable to mark fd " + stringify(s->GetFD()) + " as writable in poll");

	pollfd *ev = &events[pos->second];
	ev->events |= POLLOUT;
	
	s->SetFlag(SF_WRITABLE);
}

void SocketEngine::ClearWritable(Socket *s)
{
	if (!s->HasFlag(SF_WRITABLE))
		return;

	std::map<int, int>::iterator pos = socket_positions.find(s->GetFD());
	if (pos == socket_positions.end())
		throw SocketException("Unable clear mark fd " + stringify(s->GetFD()) + " as writable in poll");

	pollfd *ev = &events[pos->second];
	ev->events &= ~POLLOUT;

	s->UnsetFlag(SF_WRITABLE);
}

void SocketEngine::Process()
{
	int total = poll(events, SocketCount, Config->ReadTimeout * 1000);
	Anope::CurTime = time(NULL);

	/* EINTR can be given if the read timeout expires */
	if (total == -1)
	{
		if (errno != EINTR)
			Log() << "SockEngine::Process(): error: " << Anope::LastError();
		return;
	}

	for (int i = 0, processed = 0; i < SocketCount && processed != total; ++i)
	{
		pollfd *ev = &events[i];
		
		if (ev->revents != 0)
			++processed;

		std::map<int, Socket *>::iterator it = Sockets.find(ev->fd);
		if (it == Sockets.end())
			continue;
		Socket *s = it->second;

		if (ev->revents & (POLLERR | POLLRDHUP))
		{
			s->ProcessError();
			s->SetFlag(SF_DEAD);
			delete s;
			continue;
		}

		if (!s->Process())
			continue;

		if ((ev->revents & POLLIN) && !s->ProcessRead())
			s->SetFlag(SF_DEAD);

		if ((ev->revents & POLLOUT) && !s->ProcessWrite())
			s->SetFlag(SF_DEAD);

		if (s->HasFlag(SF_DEAD))
			delete s;
	}
}

