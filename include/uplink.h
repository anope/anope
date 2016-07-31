/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
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

#pragma once

#include "sockets.h"
#include "protocol.h"

namespace Uplink
{
	extern void Connect();
}

/* This is the socket to our uplink */
class UplinkSocket : public ConnectionSocket, public BufferedSocket
{
 public:
	bool error;
	UplinkSocket();
	~UplinkSocket();
	bool ProcessRead() override;
	void OnConnect() override;
	void OnError(const Anope::string &) override;

	/* A message sent over the uplink socket */
	class CoreExport Message
	{
		MessageSource source;
		std::stringstream buffer;

	 public:
	 	Message();
	 	Message(const MessageSource &);
	 	~Message();
		template<typename T> Message &operator<<(const T &val)
		{
			this->buffer << val;
			return *this;
		}
	};
};
extern CoreExport UplinkSocket *UplinkSock;

