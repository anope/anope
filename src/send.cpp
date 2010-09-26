/* Routines for sending stuff to the network.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"

/*************************************************************************/

/**
 * Send a command to the server.  The two forms here are like
 * printf()/vprintf() and friends.
 * @param source Orgin of the Message (some times NULL)
 * @param fmt Format of the Message
 * @param ... any number of parameters
 * @return void
 */
void send_cmd(const Anope::string &source, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";

	va_start(args, fmt);

	vsnprintf(buf, BUFSIZE - 1, fmt, args);

	if (!UplinkSock)
	{
		if (!source.empty())
			Log(LOG_DEBUG) << "Attemtped to send \"" << source << " " << buf << "\" with UplinkSock NULL";
		else
			Log(LOG_DEBUG) << "Attemtped to send \"" << buf << "\" with UplinkSock NULL";
		return;
	}

	if (!source.empty())
	{
		UplinkSock->Write(":%s %s", source.c_str(), buf);
		Log(LOG_RAWIO) << "Sent: :" << source << " " << buf;
	}
	else
	{
		UplinkSock->Write("%s", buf);
		Log(LOG_RAWIO) << "Sent: "<< buf;
	}

	va_end(args);
}

/*************************************************************************/

/**
 * Send a server notice
 * @param source Orgin of the Message
 * @param s Server Struct
 * @param fmt Format of the Message
 * @param ... any number of parameters
 * @return void
 */
void notice_server(const Anope::string &source, const Server *s, const char *fmt, ...)
{
	if (fmt)
	{
		va_list args;
		char buf[BUFSIZE] = "";

		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);

		if (Config->NSDefFlags.HasFlag(NI_MSG))
			ircdproto->SendGlobalPrivmsg(findbot(source), s, buf);
		else
			ircdproto->SendGlobalNotice(findbot(source), s, buf);
		va_end(args);
	}
}

