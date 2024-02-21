/*
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

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
		SendInternal(tags, source, command, { stringify(args)... });
	}

	template<typename... Args>
	void Send(const Anope::map<Anope::string> &tags, const Anope::string &command, Args &&...args)
	{
		SendInternal(tags, Me, command, { stringify(args)... });
	}

	template<typename... Args>
	void Send(const MessageSource &source, const Anope::string &command, Args &&...args)
	{
		SendInternal({}, source, command, { stringify(args)... });
	}

	template<typename... Args>
	void Send(const Anope::string &command, Args &&...args)
	{
		SendInternal({}, Me, command, { stringify(args)... });
	}
}

/* This is the socket to our uplink */
class UplinkSocket final
	: public ConnectionSocket
	, public BufferedSocket
{
public:
	bool error;
	UplinkSocket();
	~UplinkSocket();
	bool ProcessRead() override;
	void OnConnect() override;
	void OnError(const Anope::string &) override;

	/* A message sent over the uplink socket */
	class CoreExport [[deprecated]] Message final
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
