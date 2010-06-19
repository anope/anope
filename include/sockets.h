/*
 *
 * (C) 2004-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

#ifndef SOCKETS_H
#define SOCKETS_H

#ifdef _WIN32
#define CloseSocket closesocket
#else
#define CloseSocket close
#endif

#define NET_BUFSIZE 65536

class SocketException : public CoreException
{
 public:
 	/** Default constructor for socket exceptions
	 * @param message Error message
	 */
	SocketException(const std::string &message) : CoreException(message) { }

	/** Default destructor
	 * @throws Nothing
	 */
	virtual ~SocketException() throw() { }
};

class CoreExport Socket
{
 private:
	/** Read from the socket
	 * @param buf Buffer to read to
	 * @param sz How much to read
	 * @return Number of bytes recieved
	 */
	virtual int RecvInternal(char *buf, size_t sz) const;

	/** Write to the socket
	 * @param buf What to write
	 * @return Number of bytes sent, -1 on error
	 */
	virtual int SendInternal(const std::string &buf) const;

 protected:
	/* Socket FD */
	int Sock;
	/* Host this socket is connected to */
	std::string TargetHost;
	/* Port we're connected to */
	int Port;
	/* IP this socket is bound to */
	std::string BindHost;
	/* Is this an IPv6 socket? */
	bool IPv6;

	/* Messages to be written to the socket */
	std::string WriteBuffer;
	/* Part of a message not totally yet recieved */
	std::string extrabuf;
	/* How much data was recieved from the socket */
	size_t RecvLen;

 public:
 	/** Default constructor
	 * @param nTargetHost Hostname to connect to
	 * @param nPort Port to connect to
	 * @param nBindHos Host to bind to when connecting
	 * @param nIPv6 true to use IPv6
	 */
	Socket(const std::string &nTargetHost, int nPort, const std::string &nBindHost = "", bool nIPv6 = false);

	/** Default destructor
	 */
	virtual ~Socket();

	/** Get the socket FD for this socket
	 * @return The fd
	 */
	virtual int GetSock() const;

	/** Check if this socket is IPv6
	 * @return true or false
	 */
	bool IsIPv6() const;

	/** Called when there is something to be read from thie socket
	 * @return true on success, false to kill this socket
	 */
	virtual bool ProcessRead();

	/** Called when this socket becomes writeable
	 * @return true on success, false to drop this socket
	 */
	virtual bool ProcessWrite();

	/** Called when there is an error on this socket
	 */
	virtual void ProcessError();

	/** Called with a message recieved from the socket
	 * @param buf The message
	 * @return true on success, false to kill this socket
	 */
	virtual bool Read(const std::string &buf);

	/** Write to the socket
	 * @param message The message to write
	 */
	void Write(const char *message, ...);
	void Write(std::string &message);

	/** Get the length of the read buffer
	 * @return The length of the read buffer
	 */
	size_t ReadBufferLen() const;

	/** Get the length of the write buffer
	 * @return The length of the write buffer
	 */
	size_t WriteBufferLen() const;
};

class CoreExport SocketEngine
{
 private:
 	/* List of sockets that need to be deleted */
	std::set<Socket *> OldSockets;
 	/* FDs to read */
	fd_set ReadFDs;
	/* FDs that want writing */
	fd_set WriteFDs;
	/* Max FD */
	int MaxFD;

	/** Unmark a socket as writeable
	 * @param s The socket
	 */
	void ClearWriteable(Socket *s);
 public:
	/* Set of sockets */
	std::set<Socket *> Sockets;

	/** Constructor
	 */
	SocketEngine();

	/** Destructor
	 */
	virtual ~SocketEngine();

	/** Add a socket to the socket engine
	 * @param s The socket
	 */
	void AddSocket(Socket *s);

	/** Delete a socket from the socket engine
	 * @param s The socket
	 */
	void DelSocket(Socket *s);

	/** Mark a socket as wanting to be written to
	 * @param s The socket
	 */
	void MarkWriteable(Socket *s);

	/** Called to iterate through each socket and check for activity
	 */
	void Process();

	/** Get the last socket error
	 * @return The error
	 */
	const std::string GetError() const;
};

#endif
