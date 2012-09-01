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

BufferedSocket::BufferedSocket()
{
}

BufferedSocket::~BufferedSocket()
{
}

bool BufferedSocket::ProcessRead()
{
	char tbuffer[NET_BUFSIZE];

	this->RecvLen = 0;

	int len = this->IO->Recv(this, tbuffer, sizeof(tbuffer) - 1);
	if (len <= 0)
		return false;
	
	tbuffer[len] = 0;
	this->RecvLen = len;

	Anope::string sbuffer = this->extrabuf;
	sbuffer += tbuffer;
	this->extrabuf.clear();
	size_t lastnewline = sbuffer.rfind('\n');
	if (lastnewline == Anope::string::npos)
	{
		this->extrabuf = sbuffer;
		return true;
	}
	if (lastnewline < sbuffer.length() - 1)
	{
		this->extrabuf = sbuffer.substr(lastnewline);
		this->extrabuf.trim();
		sbuffer = sbuffer.substr(0, lastnewline);
	}

	sepstream stream(sbuffer, '\n');

	Anope::string tbuf;
	while (stream.GetToken(tbuf))
	{
		tbuf.trim();
		if (!Read(tbuf))
			return false;
	}

	return true;
}

bool BufferedSocket::ProcessWrite()
{
	int count = this->IO->Send(this, this->WriteBuffer);
	if (count <= -1)
		return false;
	this->WriteBuffer = this->WriteBuffer.substr(count);
	if (this->WriteBuffer.empty())
		SocketEngine::ClearWritable(this);

	return true;
}

bool BufferedSocket::Read(const Anope::string &buf)
{
	return false;
}

void BufferedSocket::Write(const char *buffer, size_t l)
{
	this->WriteBuffer += buffer + Anope::string("\r\n");
	SocketEngine::MarkWritable(this);
}

void BufferedSocket::Write(const char *message, ...)
{
	va_list vi;
	char tbuffer[BUFSIZE];

	if (!message)
		return;

	va_start(vi, message);
	int len = vsnprintf(tbuffer, sizeof(tbuffer), message, vi);
	va_end(vi);

	this->Write(tbuffer, std::min(len, static_cast<int>(sizeof(tbuffer))));
}

void BufferedSocket::Write(const Anope::string &message)
{
	this->Write(message.c_str(), message.length());
}

int BufferedSocket::ReadBufferLen() const
{
	return RecvLen;
}

int BufferedSocket::WriteBufferLen() const
{
	return this->WriteBuffer.length();
}


BinarySocket::DataBlock::DataBlock(const char *b, size_t l)
{
	this->orig = this->buf = new char[l];
	memcpy(this->buf, b, l);
	this->len = l;
}

BinarySocket::DataBlock::~DataBlock()
{
	delete [] this->orig;
}

BinarySocket::BinarySocket()
{
}

BinarySocket::~BinarySocket()
{
}

bool BinarySocket::ProcessRead()
{
	char tbuffer[NET_BUFSIZE];

	int len = this->IO->Recv(this, tbuffer, sizeof(tbuffer));
	if (len <= 0)
		return false;
	
	return this->Read(tbuffer, len);
}

bool BinarySocket::ProcessWrite()
{
	if (this->WriteBuffer.empty())
	{
		SocketEngine::ClearWritable(this);
		return true;
	}

	DataBlock *d = this->WriteBuffer.front();

	int len = this->IO->Send(this, d->buf, d->len);
	if (len <= -1)
		return false;
	else if (static_cast<size_t>(len) == d->len)
	{
		delete d;
		this->WriteBuffer.pop_front();
	}
	else
	{
		d->buf += len;
		d->len -= len;
	}

	if (this->WriteBuffer.empty())
		SocketEngine::ClearWritable(this);

	return true;
}

void BinarySocket::Write(const char *buffer, size_t l)
{
	this->WriteBuffer.push_back(new DataBlock(buffer, l));
	SocketEngine::MarkWritable(this);
}

void BinarySocket::Write(const char *message, ...)
{
	va_list vi;
	char tbuffer[BUFSIZE];

	if (!message)
		return;

	va_start(vi, message);
	int len = vsnprintf(tbuffer, sizeof(tbuffer), message, vi);
	va_end(vi);

	this->Write(tbuffer, std::min(len, static_cast<int>(sizeof(tbuffer))));
}

void BinarySocket::Write(const Anope::string &message)
{
	this->Write(message.c_str(), message.length());
}

bool BinarySocket::Read(const char *buffer, size_t l)
{
	return true;
}

