// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include "sockets.h"
#include "protocol.h"
#include "servers.h"

namespace Uplink
{
	extern void Connect();
	extern CoreExport void SendInternal(const Anope::map<Anope::string> &, const MessageSource &, const Anope::string &, const std::vector<Anope::string> &);

	template<typename... Args>
	void Send(const Anope::map<Anope::string> &tags, const MessageSource &source, const Anope::string &command, Args &&...args)
	{
		SendInternal(tags, source, command, { Anope::ToString(args)... });
	}

	template<typename... Args>
	void Send(const Anope::map<Anope::string> &tags, const Anope::string &command, Args &&...args)
	{
		SendInternal(tags, Me, command, { Anope::ToString(args)... });
	}

	template<typename... Args>
	void Send(const MessageSource &source, const Anope::string &command, Args &&...args)
	{
		SendInternal({}, source, command, { Anope::ToString(args)... });
	}

	template<typename... Args>
	void Send(const Anope::string &command, Args &&...args)
	{
		SendInternal({}, Me, command, { Anope::ToString(args)... });
	}
}

/* This is the socket to our uplink */
class UplinkSocket final
	: public ConnectionSocket
	, public BufferedSocket
{
public:
	bool error = false;
	size_t recv_msgs = 0;
	size_t sent_msgs = 0;
	UplinkSocket();
	~UplinkSocket();
	bool ProcessRead() override;
	void OnConnect() override;
	void OnError(const Anope::string &) override;
};
extern CoreExport UplinkSocket *UplinkSock;
