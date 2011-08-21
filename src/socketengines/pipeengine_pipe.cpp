#include "services.h"

Pipe::Pipe() : Socket(-1), WritePipe(-1)
{
	int fds[2];
	if (pipe(fds))
		throw CoreException("Could not create pipe: " + Anope::LastError());
	int flags = fcntl(fds[0], F_GETFL, 0);
	fcntl(fds[0], F_SETFL, flags | O_NONBLOCK);
	flags = fcntl(fds[1], F_GETFL, 0);
	fcntl(fds[1], F_SETFL, flags | O_NONBLOCK);

	this->~Socket();

	this->Sock = fds[0];
	this->WritePipe = fds[1];

	SocketEngine::AddSocket(this);
}

Pipe::~Pipe()
{
	CloseSocket(this->WritePipe);
}

bool Pipe::ProcessRead()
{
	char dummy[512];
	while (read(this->GetFD(), &dummy, 512) == 512);
	this->OnNotify();
	return true;
}

void Pipe::Notify()
{
	const char dummy = '*';
	write(this->WritePipe, &dummy, 1);
}

void Pipe::OnNotify()
{
}

