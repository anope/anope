/*
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef SOCKETENGINE_H
#define SOCKETENGINE_H

class CoreExport SocketEngine
{
#ifdef _WIN32
	/* Windows crap */
	static WSADATA wsa;
#endif
 public:
	/* Map of sockets */
	static std::map<int, Socket *> Sockets;

	/** Called to initialize the socket engine
	 */
	static void Init();

	/** Called to shutdown the socket engine
	 */
	static void Shutdown();

	/** Add a socket to the internal list
	 * @param s The socket
	 */
	static void AddSocket(Socket *s);

	/** Delete a socket from the internal list
	 * @param s The socket
	 */
	static void DelSocket(Socket *s);

	/** Mark a socket as writeable
	 * @param s The socket
	 */
	static void MarkWritable(Socket *s);

	/** Unmark a socket as writeable
	 * @param s The socket
	 */
	static void ClearWritable(Socket *s);

	/** Read from sockets and do things
	 */
	static void Process();
};

#endif // SOCKETENGINE_H
