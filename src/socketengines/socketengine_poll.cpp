#include "module.h"

#ifndef _WIN32
# include <ulimit.h>
# include <sys/poll.h>
# include <poll.h>
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
#ifndef _WIN32
	max = ulimit(4, 0);
#else
	max = 1024;
#endif

	if (max <= 0)
	{
		Log() << "Can't determine maximum number of open sockets";
		throw CoreException("Can't determine maximum number of open sockets");
	}

	events = new pollfd[max];
}

void SocketEngine::Shutdown()
{
	Process();

	for (std::map<int, Socket *>::const_iterator it = Sockets.begin(), it_end = Sockets.end(); it != it_end; ++it)
		delete it->second;
	Sockets.clear();

	delete [] events;
}

void SocketEngine::AddSocket(Socket *s)
{
	if (SocketCount == max)
	{
		Log() << "Unable to add fd " << s->GetFD() << " to socketengine poll, engine is full";
		return;
	}

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
	{
		Log() << "Unable to delete unknown fd " << s->GetFD() << " from socketengine poll";
		return;
	}

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
	{
		Log() << "Unable to mark unknown fd " << s->GetFD() << " as writable";
		return;
	}

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
	{
		Log() << "Unable to mark unknown fd " << s->GetFD() << " as writable";
		return;
	}

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

		Socket *s = Sockets[ev->fd];

		if (s->HasFlag(SF_DEAD))
			continue;
		if (ev->revents & (POLLERR | POLLRDHUP))
		{
			s->ProcessError();
			s->SetFlag(SF_DEAD);
			continue;
		}

		if ((ev->revents & POLLIN) && !s->ProcessRead())
			s->SetFlag(SF_DEAD);

		if ((ev->revents & POLLOUT) && !s->ProcessWrite())
			s->SetFlag(SF_DEAD);
	}

	for (int i = 0; i < SocketCount; ++i)
	{
		pollfd *ev = &events[i];
		Socket *s = Sockets[ev->fd];

		if (s->HasFlag(SF_DEAD))
			delete s;
	}
}

