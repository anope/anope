#include "services.h"

static ClientSocket *newsocket = NULL;

class LSocket : public ListenSocket
{
 public:
	LSocket(const Anope::string &host, int port) : ListenSocket(host, port, false) { }

	ClientSocket *OnAccept(int fd, const sockaddrs &addr)
	{
		newsocket = new ClientSocket(this, fd, addr);
		return newsocket;
	}
};

class PipeIO : public SocketIO
{
 public:
	/** Receive something from the buffer
 	 * @param s The socket
	 * @param buf The buf to read to
	 * @param sz How much to read
	 * @return Number of bytes received
	 */
	int Recv(Socket *s, char *buf, size_t sz)
	{
		static char dummy[512];
		return recv(s->GetFD(), dummy, 512, 0);
	}

	/** Write something to the socket
 	 * @param s The socket
	 * @param buf What to write
	 * @return Number of bytes written
	 */
	int Send(Socket *s, const Anope::string &buf)
	{
		static const char dummy = '*';
		Pipe *pipe = debug_cast<Pipe *>(s);
		return send(pipe->WritePipe, &dummy, 1, 0);
	}
} pipeSocketIO;

Pipe::Pipe() : BufferedSocket()
{
	LSocket lfs("127.0.0.1", 0);

	int cfd = socket(AF_INET, SOCK_STREAM, 0);
	if (cfd == -1)
		throw CoreException("Error accepting new socket for Pipe");

	sockaddr_in addr;
	socklen_t sz = sizeof(addr);
	getsockname(lfs.GetFD(), reinterpret_cast<sockaddr *>(&addr), &sz);

	if (connect(cfd, reinterpret_cast<sockaddr *>(&addr), sz))
		throw CoreException("Error accepting new socket for Pipe");
	lfs.ProcessRead();
	if (!newsocket)
		throw CoreException("Error accepting new socket for Pipe");
	
	this->IO = &pipeSocketIO;
	this->Sock = cfd;
	this->WritePipe = newsocket->GetFD();
	this->IPv6 = false;

	SocketEngine::AddSocket(this);
	newsocket = NULL;
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

