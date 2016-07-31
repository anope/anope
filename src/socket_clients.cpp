/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
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
#include "anope.h"
#include "logger.h"
#include "sockets.h"

#include <errno.h>

void ConnectionSocket::Connect(const Anope::string &TargetHost, int Port)
{
	this->io->Connect(this, TargetHost, Port);
}

bool ConnectionSocket::Process()
{
	try
	{
		if (this->flags[SF_CONNECTED])
			return true;
		else if (this->flags[SF_CONNECTING])
			this->flags[this->io->FinishConnect(this)] = true;
		else
			this->flags[SF_DEAD] = true;
	}
	catch (const SocketException &ex)
	{
		Log() << ex.GetReason();
	}
	return false;
}

void ConnectionSocket::ProcessError()
{
	int optval = 0;
	socklen_t optlen = sizeof(optval);
	getsockopt(this->GetFD(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&optval), &optlen);
	errno = optval;
	this->OnError(optval ? Anope::LastError() : "");
}

void ConnectionSocket::OnConnect()
{
}

void ConnectionSocket::OnError(const Anope::string &error)
{
	Log(LOG_DEBUG) << "Socket error: " << error;
}

ClientSocket::ClientSocket(ListenSocket *l, const sockaddrs &addr) : ls(l), clientaddr(addr)
{
}

bool ClientSocket::Process()
{
	try
	{
		if (this->flags[SF_ACCEPTED])
			return true;
		else if (this->flags[SF_ACCEPTING])
			this->flags[this->io->FinishAccept(this)] = true;
		else
			this->flags[SF_DEAD] = true;
	}
	catch (const SocketException &ex)
	{
		Log() << ex.GetReason();
	}
	return false;
}

void ClientSocket::ProcessError()
{
	int optval = 0;
	socklen_t optlen = sizeof(optval);
	getsockopt(this->GetFD(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&optval), &optlen);
	errno = optval;
	this->OnError(optval ? Anope::LastError() : "");
}

void ClientSocket::OnAccept()
{
}

void ClientSocket::OnError(const Anope::string &error)
{
	Log(LOG_DEBUG) << "Socket error: " << error;
}

