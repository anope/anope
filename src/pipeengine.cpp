/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
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
	int sflags = fcntl(fds[0], F_GETFL, 0);
	fcntl(fds[0], F_SETFL, sflags | O_NONBLOCK);
	sflags = fcntl(fds[1], F_GETFL, 0);
	fcntl(fds[1], F_SETFL, sflags | O_NONBLOCK);

	SocketEngine::Change(this, false, SF_READABLE);
	SocketEngine::Change(this, false, SF_WRITABLE);
	anope_close(this->sock);
	this->io->Destroy();
	SocketEngine::Sockets.erase(this->sock);

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
	while (read(this->GetFD(), dummy, sizeof(dummy)) == sizeof(dummy));

	this->OnNotify();

	return true;
}

void Pipe::Write(const char *data, size_t sz)
{
	write(this->write_pipe, data, sz);
}

int Pipe::Read(char *data, size_t sz)
{
	return read(this->GetFD(), data, sz);
}

bool Pipe::SetWriteBlocking(bool state)
{
	int f = fcntl(this->write_pipe, F_GETFL, 0);
	if (state)
		return !fcntl(this->write_pipe, F_SETFL, f & ~O_NONBLOCK);
	else
		return !fcntl(this->write_pipe, F_SETFL, f | O_NONBLOCK);
}

void Pipe::Notify()
{
	this->Write("", 1);
}

