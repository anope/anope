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

class CoreExport SocketEngineBase
{
 public:
#ifdef _WIN32
	/* Windows crap */
	WSADATA wsa;
#endif
	/* Map of sockets */
	std::map<int, Socket *> Sockets;

	/** Default constructor
	 */
	SocketEngineBase();

	/** Default destructor
	 */
	virtual ~SocketEngineBase();

	/** Add a socket to the internal list
	 * @param s The socket
	 */
	virtual void AddSocket(Socket *s) { }

	/** Delete a socket from the internal list
	 * @param s The socket
	 */
	virtual void DelSocket(Socket *s) { }

	/** Mark a socket as writeable
	 * @param s The socket
	 */
	virtual void MarkWritable(Socket *s) { }

	/** Unmark a socket as writeable
	 * @param s The socket
	 */
	virtual void ClearWritable(Socket *s) { }

	/** Read from sockets and do things
	 */
	virtual void Process() { }
};

#endif // SOCKETENGINE_H
