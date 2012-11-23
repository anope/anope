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

#ifndef _WIN32
#include <fcntl.h>
#endif

Pipe::Pipe() : Socket(-1), write_pipe(-1)
{
	int fds[2];
	if (pipe(fds))
		throw CoreException("Could not create pipe: " + Anope::LastError());
	int flags = fcntl(fds[0], F_GETFL, 0);
	fcntl(fds[0], F_SETFL, flags | O_NONBLOCK);
	flags = fcntl(fds[1], F_GETFL, 0);
	fcntl(fds[1], F_SETFL, flags | O_NONBLOCK);

	this->~Pipe();

	this->sock = fds[0];
	this->write_pipe = fds[1];

	SocketEngine::Sockets[this->sock] = this;
	SocketEngine::Change(this, true, SF_READABLE);
}

Pipe::~Pipe()
{
	if (this->write_pipe >= 0)
		anope_close(this->write_pipe);
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
	write(this->write_pipe, &dummy, 1);
}

