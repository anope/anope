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

class SocketEnginePoll : public SocketEngineBase
{
 private:
	long max;
	pollfd *events;
	int SocketCount;
	std::map<int, int> socket_positions;

 public:
	SocketEnginePoll()
	{
		SocketCount = 0;

#ifndef _WIN32
		rlimit fd_limit;
		if (getrlimit(RLIMIT_NOFILE, &fd_limit) == -1)
			throw CoreException(Anope::LastError());

		max = fd_limit.rlim_cur;
#else
		max = 1024;
#endif

		events = new pollfd[max];
	}

	~SocketEnginePoll()
	{
		delete [] events;
	}

	void AddSocket(Socket *s)
	{
		if (SocketCount == max)
		{
			Log() << "Unable to add fd " << s->GetFD() << " to socketengine poll, engine is full";
			return;
		}

		pollfd *ev = &this->events[SocketCount];
		ev->fd = s->GetFD();
		ev->events = POLLIN;
		ev->revents = 0;

		Sockets.insert(std::make_pair(ev->fd, s));
		socket_positions.insert(std::make_pair(ev->fd, SocketCount));

		++SocketCount;
	}

	void DelSocket(Socket *s)
	{
		std::map<int, int>::iterator pos = socket_positions.find(s->GetFD());
		if (pos == socket_positions.end())
		{
			Log() << "Unable to delete unknown fd " << s->GetFD() << " from socketengine poll";
			return;
		}

		if (pos->second != SocketCount - 1)
		{
			pollfd *ev = &this->events[pos->second],
				*last_ev = &this->events[SocketCount - 1];

			ev->fd = last_ev->fd;
			ev->events = last_ev->events;
			ev->revents = last_ev->revents;

			socket_positions[ev->fd] = pos->second;
		}

		Sockets.erase(s->GetFD());
		this->socket_positions.erase(pos);

		--SocketCount;
	}

	void MarkWritable(Socket *s)
	{
		if (s->HasFlag(SF_WRITABLE))
			return;

		std::map<int, int>::iterator pos = socket_positions.find(s->GetFD());
		if (pos == socket_positions.end())
		{
			Log() << "Unable to mark unknown fd " << s->GetFD() << " as writable";
			return;
		}

		pollfd *ev = &this->events[pos->second];
		ev->events |= POLLOUT;
		
		s->SetFlag(SF_WRITABLE);
	}

	void ClearWritable(Socket *s)
	{
		if (!s->HasFlag(SF_WRITABLE))
			return;

		std::map<int, int>::iterator pos = socket_positions.find(s->GetFD());
		if (pos == socket_positions.end())
		{
			Log() << "Unable to mark unknown fd " << s->GetFD() << " as writable";
			return;
		}

		pollfd *ev = &this->events[pos->second];
		ev->events &= ~POLLOUT;

		s->UnsetFlag(SF_WRITABLE);
	}

	void Process()
	{
		int total = poll(this->events, this->SocketCount, Config->ReadTimeout * 1000);
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
			pollfd *ev = &this->events[i];
			
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
			pollfd *ev = &this->events[i];
			Socket *s = Sockets[ev->fd];

			if (s->HasFlag(SF_DEAD))
				delete s;
		}
	}
};

class ModuleSocketEnginePoll : public Module
{
	SocketEnginePoll engine;

 public:
	ModuleSocketEnginePoll(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetPermanent(true);
		this->SetType(SOCKETENGINE);

		SocketEngine = &engine;
	}

	~ModuleSocketEnginePoll()
	{
		SocketEngine = NULL;
	}
};

MODULE_INIT(ModuleSocketEnginePoll)
