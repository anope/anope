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

/** A sockaddr union used to combine IPv4 and IPv6 sockaddrs
 */
union CoreExport sockaddrs
{
	sockaddr sa;
	sockaddr_in sa4;
	sockaddr_in6 sa6;

	/** Get the size of the sockaddr we represent
	 * @return The size
	 */
	size_t size() const;

	/** Get the port represented by this addr
	 * @return The port, or -1 on fail
	 */
	int port() const;

	/** Get the address represented by this addr
	 * @return The address
	 */
	Anope::string addr() const;

	/** Construct the object, sets everything to 0
	 */
	sockaddrs();

	/** Check if this sockaddr has data in it
	 */
	bool operator()() const;

	/** Compares with sockaddr with another. Compares address type, port, and address
	 * @return true if they are the same
	 */
	bool operator==(const sockaddrs &other) const;
	/* The same as above but not */
	inline bool operator!=(const sockaddrs &other) const { return !(*this == other); }

	void pton(int type, const Anope::string &address, int pport = 0);

	void ntop(int type, const void *src);
};

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
	SF_DEAD,
	SF_WRITABLE
};

class CoreExport Socket : public Flags<SocketFlag, 2>
{
 protected:
	/** Really recieve something from the buffer
	 * @param buf The buf to read to
	 * @param sz How much to read
	 * @return Number of bytes recieved
	 */
	virtual int RecvInternal(char *buf, size_t sz) const;

	/** Really write something to the socket
	 * @param buf What to write
	 * @return Number of bytes written
	 */
	virtual int SendInternal(const Anope::string &buf) const;

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

	/** Empty constructor, used for things such as the pipe socket
	 */
	Socket();

	/** Default constructor
	 * @param nsock The socket to use, 0 if we need to create our own
	 * @param nIPv6 true if using ipv6
	 * @param type The socket type, defaults to SOCK_STREAM
	 */
 	Socket(int nsock, bool nIPv6, int type = SOCK_STREAM);

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

	/** Called when the socket is ready to be written to
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

class CoreExport Pipe : public Socket
{
 private:
 	/** The FD of the write pipe (if this isn't evenfd)
	 * this->Sock is the readfd
	 */
 	int WritePipe;

	/** Our overloaded RecvInternal call
	 */
	int RecvInternal(char *buf, size_t sz) const;

	/** Our overloaded SendInternal call
	 */
	int SendInternal(const Anope::string &buf) const;
 public:
 	/** Constructor
	 */
	Pipe();

	/** Called when data is to be read
	 */
	bool ProcessRead();

	/** Function that calls OnNotify
	 */
	bool Read(const Anope::string &);

	/** Called when this pipe needs to be woken up
	 */
	void Notify();

	/** Should be overloaded to do something useful
	 */
	virtual void OnNotify();
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
	 * @param type The socket type, defaults to SOCK_STREAM
	 */
	ClientSocket(const Anope::string &nTargetHost, int nPort, const Anope::string &nBindHost = "", bool nIPv6 = false, int type = SOCK_STREAM);

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
	int GetPort() const;
};

#endif // SOCKET_H
