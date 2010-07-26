/*                                               
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef SOCKETS_H
#define SOCKETS_H

#include "anope.h"

#define NET_BUFSIZE 65535

#ifdef _WIN32
# define CloseSocket closesocket
#else
# define CloseSocket close
#endif

class SocketException : public CoreException
{
 public:
	/** Default constructor for socket exceptions
	 * @param message Error message
	 */
	SocketException(const Anope::string &message) : CoreException(message) { }

	/** Default destructor
	 * @throws Nothing
	 */
	virtual ~SocketException() throw() { }
};

enum SocketType
{
	SOCKTYPE_CLIENT,
	SOCKTYPE_LISTEN
};

enum SocketFlag
{
	SF_DEAD
};

class CoreExport Socket : public Flags<SocketFlag, 1>
{
 private:
	/** Really recieve something from the buffer
	 * @param buf The buf to read to
	 * @param sz How much to read
	 * @return Number of bytes recieved
	 */
	virtual const int RecvInternal(char *buf, size_t sz) const;

	/** Really write something to the socket
	 * @param buf What to write
	 * @return Number of bytes written
	 */
	virtual const int SendInternal(const Anope::string &buf) const;

 protected:
	/* Socket FD */
	int Sock;
	/* Port we're connected to */
	int Port;
	/* Is this an IPv6 socket? */
	bool IPv6;
	/* Things to be written to the socket */
	std::string WriteBuffer;
	/* Part of a message sent from the server, but not totally recieved */
	std::string extrabuf;
	/* How much data was received from this socket */
	size_t RecvLen;

 public:
	/* Type this socket is */
	SocketType Type;

	/** Default constructor
	 * @param nsock The socket to use, 0 if we need to create our own
	 * @param nIPv6 true if using ipv6
	 */
 	Socket(int nsock, bool nIPv6);
	
	/** Default destructor
	 */
	virtual ~Socket();

	/** Get the socket FD for this socket
	 * @return the fd
	 */
	int GetSock() const;

	/** Mark a socket as blockig
	 * @return true if the socket is now blocking
	 */
	bool SetBlocking();

	/** Mark a socket as non-blocking
	 * @return true if the socket is now non-blocking
	 */
	bool SetNonBlocking();

	/** Check if this socket is IPv6
	 * @return true or false
	 */
	bool IsIPv6() const;

	/** Get the length of the read buffer
	 * @return The length of the read buffer
	 */
	size_t ReadBufferLen() const;

	/** Get the length of the write buffer
	 * @return The length of the write buffer
	 */
	size_t WriteBufferLen() const;

	/** Called when there is something to be recieved for this socket
	 * @return true on success, false to drop this socket
	 */
	virtual bool ProcessRead();

	/** Called when there is something to be written to this socket
	 * @return true on success, false to drop this socket
	 */
	virtual bool ProcessWrite();

	/** Called when there is an error for this socket
	 * @return true on success, false to drop this socket
	 */
	virtual void ProcessError();

	/** Called with a line recieved from the socket
	 * @param buf The line
	 * @return true to continue reading, false to drop the socket
	 */
	virtual bool Read(const Anope::string &buf);

	/** Write to the socket
	 * @param message The message
	 */
	void Write(const char *message, ...);
	void Write(const Anope::string &message);
};

class CoreExport ClientSocket : public Socket
{
 protected:
	/* Target host we're connected to */
	Anope::string TargetHost;
	/* Target port we're connected to */
	int Port;
	/* The host to bind to */
	Anope::string BindHost;

 public:

       /** Constructor
	* @param nTargetHost The target host to connect to
	* @param nPort The target port to connect to
	* @param nBindHost The host to bind to for connecting
	* @param nIPv6 true to use IPv6
	*/
       ClientSocket(const Anope::string &nTargetHost, int nPort, const Anope::string &nBindHost, bool nIPv6);

       /** Default destructor
	*/
       virtual ~ClientSocket();

	/** Called with a line recieved from the socket
	* @param buf The line
	* @return true to continue reading, false to drop the socket
	*/
	virtual bool Read(const Anope::string &buf);
};

class CoreExport ListenSocket : public Socket
{
 protected:
	/* Bind IP */
	Anope::string BindIP;
	/* Port to bind to */
	int Port;

 public:
	/** Constructor
	 * @param bind The IP to bind to
	 * @param port The port to listen on
	 */
	ListenSocket(const Anope::string &bind, int port);

	/** Destructor
	 */
	virtual ~ListenSocket();

	/** Process what has come in from the connection
	 * @return false to destory this socket
	 */
	bool ProcessRead();

	/** Called when a connection is accepted
	 * @param s The socket for the new connection
	 * @return true if the listen socket should remain alive
	 */
	virtual bool OnAccept(Socket *s);

	/** Get the bind IP for this socket
	 * @return the bind ip
	 */
	const Anope::string &GetBindIP() const;

	/** Get the port this socket is bound to
	 * @return The port
	*/
	const int GetPort() const;
};

#endif // SOCKET_H
