#include "services.h"

static Socket *newsocket = NULL;

class LSocket : public ListenSocket
{
 public:
	LSocket(const Anope::string &host, int port) : ListenSocket(host, port) { }

	bool OnAccept(Socket *s)
	{
		newsocket = s;
		return true;
	}
};

int Pipe::RecvInternal(char *buf, size_t sz) const
{
	static char dummy[512];
	return read(this->Sock, &dummy, 512);
}

int Pipe::SendInternal(const Anope::string &) const
{
	static const char dummy = '*';
	return write(this->WritePipe, &dummy, 1);
}

Pipe::Pipe() : Socket()
{
	LSocket lfs("127.0.0.1", 0);

	int cfd = socket(AF_INET, SOCK_STREAM, 0);
	if (cfd == -1)
		throw CoreException("Error accepting new socket for Pipe");

	sockaddr_in addr;
	socklen_t sz = sizeof(addr);
	getsockname(lfs.GetSock(), reinterpret_cast<sockaddr *>(&addr), &sz);

	if (connect(cfd, reinterpret_cast<sockaddr *>(&addr), sz))
		throw CoreException("Error accepting new socket for Pipe");
	lfs.ProcessRead();
	if (!newsocket)
		throw CoreException("Error accepting new socket for Pipe");
	
	this->Sock = cfd;
	this->WritePipe = newsocket->GetSock();
	this->IPv6 = false;
	this->Type = SOCKTYPE_CLIENT;

	SocketEngine->AddSocket(this);
	newsocket = NULL;
}

bool Pipe::ProcessRead()
{
	this->RecvInternal(NULL, 0);
	return this->Read("");
}

bool Pipe::Read(const Anope::string &)
{
	this->OnNotify();
	return true;
}

void Pipe::Notify()
{
	this->SendInternal("");
}

void Pipe::OnNotify()
{
}

