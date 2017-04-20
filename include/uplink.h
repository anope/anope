/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2017 Anope Team <team@anope.org>
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
	extern void SendMessage(IRCMessage &);
	
	template<typename... Args>
	void Send(const MessageSource &source, const Anope::string &command, Args&&... args)
	{
		IRCMessage message(source, command, std::forward<Args>(args)...);
		SendMessage(message);
	}
	
	template<typename... Args>
	void Send(const Anope::string &command, Args&&... args)
	{
		IRCMessage message(Me, command, std::forward<Args>(args)...);
		SendMessage(message);
	}
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
};

extern CoreExport UplinkSocket *UplinkSock;

