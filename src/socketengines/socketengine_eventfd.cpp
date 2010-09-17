#include "services.h"
#include <sys/eventfd.h>

int Pipe::RecvInternal(char *buf, size_t sz) const
{
	static eventfd_t dummy;
	return !eventfd_read(this->Sock, &dummy);
}

int Pipe::SendInternal(const Anope::string &) const
{
	return !eventfd_write(this->Sock, 1);
}

Pipe::Pipe() : Socket()
{
	this->Sock = eventfd(0, EFD_NONBLOCK);
	if (this->Sock < 0)
		throw CoreException(Anope::string("Could not create pipe: ") + Anope::LastError());

	this->IPv6 = false;
	this->Type = SOCKTYPE_CLIENT;

	SocketEngine->AddSocket(this);
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
	this->Write("*");
}

void Pipe::OnNotify()
{
}

