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

	this->recv_len = 0;

	int len = this->io->Recv(this, tbuffer, sizeof(tbuffer) - 1);
	if (len == 0)
		return false;
	if (len < 0)
		return SocketEngine::IgnoreErrno();

	tbuffer[len] = 0;
	this->read_buffer.append(tbuffer);
	this->recv_len = len;

	return true;
}

bool BufferedSocket::ProcessWrite()
{
	int count = this->io->Send(this, this->write_buffer);
	if (count == 0)
		return false;
	if (count < 0)
		return SocketEngine::IgnoreErrno();

	this->write_buffer = this->write_buffer.substr(count);
	if (this->write_buffer.empty())
		SocketEngine::Change(this, false, SF_WRITABLE);

	return true;
}

const Anope::string BufferedSocket::GetLine()
{
	size_t s = this->read_buffer.find('\n');
	if (s == Anope::string::npos)
		return "";
	Anope::string str = this->read_buffer.substr(0, s + 1);
	this->read_buffer.erase(0, s + 1);
	this->read_buffer.ltrim("\r\n");
	return str.trim("\r\n");
}

void BufferedSocket::Write(const char *buffer, size_t l)
{
	this->write_buffer += buffer + Anope::string("\r\n");
	SocketEngine::Change(this, true, SF_WRITABLE);
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
	return recv_len;
}

int BufferedSocket::WriteBufferLen() const
{
	return this->write_buffer.length();
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

	int len = this->io->Recv(this, tbuffer, sizeof(tbuffer));
	if (len <= 0)
		return false;

	return this->Read(tbuffer, len);
}

bool BinarySocket::ProcessWrite()
{
	if (this->write_buffer.empty())
	{
		SocketEngine::Change(this, false, SF_WRITABLE);
		return true;
	}

	DataBlock *d = this->write_buffer.front();

	int len = this->io->Send(this, d->buf, d->len);
	if (len <= -1)
		return false;
	else if (static_cast<size_t>(len) == d->len)
	{
		delete d;
		this->write_buffer.pop_front();
	}
	else
	{
		d->buf += len;
		d->len -= len;
	}

	if (this->write_buffer.empty())
		SocketEngine::Change(this, false, SF_WRITABLE);

	return true;
}

void BinarySocket::Write(const char *buffer, size_t l)
{
	if (l == 0)
		return;
	this->write_buffer.push_back(new DataBlock(buffer, l));
	SocketEngine::Change(this, true, SF_WRITABLE);
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

