#include "services.h"

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
		static char dummy[512];
		while (read(s->GetFD(), &dummy, 512) == 512);
		return 0;
	}

	/** Write something to the socket
 	 * @param s The socket
	 * @param buf What to write
	 * @return Number of bytes written
	 */
	int Send(Socket *s, const Anope::string &buf) const
	{
		static const char dummy = '*';
		Pipe *pipe = debug_cast<Pipe *>(s);
		return write(pipe->WritePipe, &dummy, 1);
	}
} pipeSocketIO;

Pipe::Pipe() : BufferedSocket()
{
	int fds[2];
	if (pipe(fds))
		throw CoreException(Anope::string("Could not create pipe: ") + Anope::LastError());
	int flags = fcntl(fds[0], F_GETFL, 0);
	fcntl(fds[0], F_SETFL, flags | O_NONBLOCK);
	flags = fcntl(fds[1], F_GETFL, 0);
	fcntl(fds[1], F_SETFL, flags | O_NONBLOCK);

	this->IO = &pipeSocketIO;
	this->Sock = fds[0];
	this->WritePipe = fds[1];
	this->IPv6 = false;

	SocketEngine::AddSocket(this);
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
	this->IO->Send(this, "");
}

void Pipe::OnNotify()
{
}

