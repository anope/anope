#include "services.h"

Pipe::Pipe() : Socket(-1)
{
	sockaddrs localhost;

	localhost.pton(AF_INET, "127.0.0.1");

	int cfd = socket(AF_INET, SOCK_STREAM, 0), lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (cfd == -1)
		throw CoreException("Error accepting new socket for Pipe");
	
	if (bind(lfd, &localhost.sa, localhost.size()) == -1)
		throw CoreException("Error accepting new socket for Pipe");
	if (listen(lfd, 1) == -1)
		throw CoreException("Error accepting new socket for Pipe");

	sockaddrs lfd_addr;
	socklen_t sz = sizeof(lfd_addr);
	getsockname(lfd, &lfd_addr.sa, &sz);

	if (connect(cfd, &lfd_addr.sa, lfd_addr.size()))
		throw CoreException("Error accepting new socket for Pipe");
	CloseSocket(lfd);
	
	this->WritePipe = cfd;

	SocketEngine::AddSocket(this);
}

Pipe::~Pipe()
{
	CloseSocket(this->WritePipe);
}

bool Pipe::ProcessRead()
{
	char dummy[512];
	while (recv(this->GetFD(), dummy, 512, 0) == 512);
	this->OnNotify();
	return true;
}

void Pipe::Notify()
{
	const char dummy = '*';
	send(this->WritePipe, &dummy, 1, 0);
}

void Pipe::OnNotify()
{
}

