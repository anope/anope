#include "services.h"
#include <sys/eventfd.h>

class PipeIO : public SocketIO
{
 public:
	/** Receive something from the buffer
 	 * @param s The socket
	 * @param buf The buf to read to
	 * @param sz How much to read
	 * @return Number of bytes received
	 */
	int Recv(Socket *s, char *buf, size_t sz) const
	{
		static eventfd_t dummy;
		return !eventfd_read(s->GetFD(), &dummy);
	}

	/** Write something to the socket
 	 * @param s The socket
	 * @param buf What to write
	 * @return Number of bytes written
	 */
	int Send(Socket *s, const Anope::string &buf) const
	{
		return !eventfd_write(s->GetFD(), 1);
	}
} pipeSocketIO;

Pipe::Pipe() : BufferedSocket()
{
	this->IO = &pipeSocketIO;
	this->Sock = eventfd(0, EFD_NONBLOCK);
	if (this->Sock < 0)
		throw CoreException(Anope::string("Could not create pipe: ") + Anope::LastError());

	this->IPv6 = false;

	SocketEngine->AddSocket(this);
}

bool Pipe::ProcessRead()
{
	this->IO->Recv(this, NULL, 0);
	return this->Read("");
}

bool Pipe::Read(const Anope::string &)
{
	this->OnNotify();
	return true;
}

void Pipe::Notify()
{
	/* Note we send this immediatly. If use use Socket::Write and if this functions is called
	 * from a thread, only epoll is able to pick up the change to this sockets want flags immediately
	 * Other engines time out then pick up and write the change then read it back, which
	 * is too slow for most things.
	 */
	this->IO->Send(this, "");
}

void Pipe::OnNotify()
{
}

