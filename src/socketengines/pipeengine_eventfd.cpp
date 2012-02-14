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

#include <sys/eventfd.h>

Pipe::Pipe() : Socket(eventfd(0, EFD_NONBLOCK))
{
	if (this->Sock < 0)
		throw CoreException("Could not create pipe: " + Anope::LastError());
}

Pipe::~Pipe()
{
}

bool Pipe::ProcessRead()
{
	eventfd_t dummy;
	eventfd_read(this->GetFD(), &dummy);
	this->OnNotify();
	return true;
}

void Pipe::Notify()
{
	eventfd_write(this->GetFD(), 1);
}

void Pipe::OnNotify()
{
}

