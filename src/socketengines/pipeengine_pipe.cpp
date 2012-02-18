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
#include "sockets.h"
#include "socketengine.h"

#include <unistd.h>
#include <fcntl.h>

Pipe::Pipe() : Socket(-1), WritePipe(-1)
{
	int fds[2];
	if (pipe(fds))
		throw CoreException("Could not create pipe: " + Anope::LastError());
	int flags = fcntl(fds[0], F_GETFL, 0);
	fcntl(fds[0], F_SETFL, flags | O_NONBLOCK);
	flags = fcntl(fds[1], F_GETFL, 0);
	fcntl(fds[1], F_SETFL, flags | O_NONBLOCK);

	this->~Pipe();

	this->Sock = fds[0];
	this->WritePipe = fds[1];

	SocketEngine::AddSocket(this);
}

Pipe::~Pipe()
{
	if (this->WritePipe >= 0)
		close(this->WritePipe);
}

bool Pipe::ProcessRead()
{
	char dummy[512];
	while (read(this->GetFD(), dummy, 512) == 512);
	this->OnNotify();
	return true;
}

void Pipe::Notify()
{
	const char dummy = '*';
	write(this->WritePipe, &dummy, 1);
}

