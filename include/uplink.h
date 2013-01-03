/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#ifndef UPLINK_H
#define UPLINK_H

#include "sockets.h"

namespace Uplink
{
	extern void Connect();
}

/* This is the socket to our uplink */
class UplinkSocket : public ConnectionSocket, public BufferedSocket
{
 public:
	UplinkSocket();
	~UplinkSocket();
	bool ProcessRead() anope_override;
	void OnConnect() anope_override;
	void OnError(const Anope::string &) anope_override;

	/* A message sent over the uplink socket */
	class CoreExport Message
	{
	 private:
	 	/* The source of the message, can be a server (Me), or any user (one of our bots) */
		const Server *server;
		const User *user;
		std::stringstream buffer;

	 public:
	 	Message();
	 	explicit Message(const Server *);
		explicit Message(const User *);
	 	~Message();
		template<typename T> Message &operator<<(const T &val)
		{
			this->buffer << val;
			return *this;
		}
	};
};
extern CoreExport UplinkSocket *UplinkSock;

#endif // UPLINK_H

