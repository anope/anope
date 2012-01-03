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

ConnectionSocket::ConnectionSocket() : Socket()
{
}

void ConnectionSocket::Connect(const Anope::string &TargetHost, int Port)
{
	this->IO->Connect(this, TargetHost, Port);
}

bool ConnectionSocket::Process()
{
	try
	{
		if (this->HasFlag(SF_CONNECTED))
			return true;
		else if (this->HasFlag(SF_CONNECTING))
			this->SetFlag(this->IO->FinishConnect(this));
		else
			this->SetFlag(SF_DEAD);
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

void ConnectionSocket::OnError(const Anope::string &)
{
}

ClientSocket::ClientSocket(ListenSocket *ls, const sockaddrs &addr) : Socket(), LS(ls), clientaddr(addr)
{
}

bool ClientSocket::Process()
{
	try
	{
		if (this->HasFlag(SF_ACCEPTED))
			return true;
		else if (this->HasFlag(SF_ACCEPTING))
			this->SetFlag(this->IO->FinishAccept(this));
		else
			this->SetFlag(SF_DEAD);
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
}

