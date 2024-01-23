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

namespace Uplink
{
	extern void Connect();
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
	class CoreExport Message final
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
