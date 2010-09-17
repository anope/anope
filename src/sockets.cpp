#include "services.h"

SocketEngineBase *SocketEngine;
int32 TotalRead = 0;
int32 TotalWritten = 0;

/** Trims all the \r and \ns from the begining and end of a string
 * @param buffer The buffer to trim
 */
static void TrimBuf(std::string &buffer)
{
	while (!buffer.empty() && (buffer[0] == '\r' || buffer[0] == '\n'))
		buffer.erase(buffer.begin());
	while (!buffer.empty() && (buffer[buffer.length() - 1] == '\r' || buffer[buffer.length() - 1] == '\n'))
		buffer.erase(buffer.length() - 1);
}

/** Get the size of the sockaddr we represent
 * @return The size
 */
size_t sockaddrs::size() const
{
	switch (sa.sa_family)
	{
		case AF_INET:
			return sizeof(sa4);
		case AF_INET6:
			return sizeof(sa6);
		default:
			break;
	}

	return 0;
}

/** Get the port represented by this addr
 * @return The port, or -1 on fail
 */
int sockaddrs::port() const
{
	switch (sa.sa_family)
	{
		case AF_INET:
			return ntohs(sa4.sin_port);
		case AF_INET6:
			return ntohs(sa6.sin6_port);
		default:
			break;
	}

	return -1;
}

/** Get the address represented by this addr
 * @return The address
 */
Anope::string sockaddrs::addr() const
{
	char address[INET6_ADDRSTRLEN + 1] = "";

	switch (sa.sa_family)
	{
		case AF_INET:
			if (!inet_ntop(AF_INET, &sa4.sin_addr, address, sizeof(address)))
				throw SocketException(strerror(errno));
			return address;
		case AF_INET6:
			if (!inet_ntop(AF_INET6, &sa6.sin6_addr, address, sizeof(address)))
				throw SocketException(strerror(errno));
			return address;
		default:
			break;
	}

	return address;
}

/** Construct the object, sets everything to 0
 */
sockaddrs::sockaddrs()
{
	memset(this, 0, sizeof(*this));
}

/** Check if this sockaddr has data in it
 */
bool sockaddrs::operator()() const
{
	return this->sa.sa_family != 0;
}

/** Compares with sockaddr with another. Compares address type, port, and address
 * @return true if they are the same
 */
bool sockaddrs::operator==(const sockaddrs &other) const
{
	if (sa.sa_family != other.sa.sa_family)
		return false;
	switch (sa.sa_family)
	{
		case AF_INET:
			return (sa4.sin_port == other.sa4.sin_port) && (sa4.sin_addr.s_addr == other.sa4.sin_addr.s_addr);
		case AF_INET6:
			return (sa6.sin6_port == other.sa6.sin6_port) && !memcmp(sa6.sin6_addr.s6_addr, other.sa6.sin6_addr.s6_addr, 16);
		default:
			return !memcmp(this, &other, sizeof(*this));
	}

	return false;
}

/** The equivalent of inet_pton
 * @param type AF_INET or AF_INET6
 * @param address The address to place in the sockaddr structures
 * @param pport An option port to include in the  sockaddr structures
 * @throws A socket exception if given invalid IPs
 */
void sockaddrs::pton(int type, const Anope::string &address, int pport)
{
	switch (type)
	{
		case AF_INET:
			if (inet_pton(type, address.c_str(), &sa4.sin_addr) < 1)
				throw SocketException(Anope::string("Invalid host: ") + strerror(errno));
			sa4.sin_family = type;
			sa4.sin_port = htons(pport);
			return;
		case AF_INET6:
			if (inet_pton(type, address.c_str(), &sa6.sin6_addr) < 1)
				throw SocketException(Anope::string("Invalid host: ") + strerror(errno));
			sa6.sin6_family = type;
			sa6.sin6_port = htons(pport);
			return;
		default:
			break;
	}

	throw CoreException("Invalid socket type");
}

/** The equivalent of inet_ntop
 * @param type AF_INET or AF_INET6
 * @param address The in_addr or in_addr6 structure
 * @throws A socket exception if given an invalid structure
 */
void sockaddrs::ntop(int type, const void *src)
{
	switch (type)
	{
		case AF_INET:
			sa4.sin_addr = *reinterpret_cast<const in_addr *>(src);
			sa4.sin_family = type;
			return;
		case AF_INET6:
			sa6.sin6_addr = *reinterpret_cast<const in6_addr *>(src);
			sa6.sin6_family = type;
			return;
		default:
			break;
	}

	throw CoreException("Invalid socket type");
}

SocketEngineBase::SocketEngineBase()
{
#ifdef _WIN32
	if (WSAStartup(MAKEWORD(2, 0), &wsa))
		throw FatalException("Failed to initialize WinSock library");
#endif
}

SocketEngineBase::~SocketEngineBase()
{
	for (std::map<int, Socket *>::const_iterator it = this->Sockets.begin(), it_end = this->Sockets.end(); it != it_end; ++it)
		delete it->second;
	this->Sockets.clear();
#ifdef _WIN32
	WSACleanup();
#endif
}

Socket::Socket()
{
}

/** Constructor
 * @param nsock The socket
 * @param nIPv6 IPv6?
 * @param type The socket type, defaults to SOCK_STREAM
 */
Socket::Socket(int nsock, bool nIPv6, int type)
{
	Type = SOCKTYPE_CLIENT;
	IPv6 = nIPv6;
	if (nsock == 0)
		Sock = socket(IPv6 ? AF_INET6 : AF_INET, type, 0);
	else
		Sock = nsock;
	SocketEngine->AddSocket(this);
}

/** Default destructor
*/
Socket::~Socket()
{
	if (SocketEngine)
		SocketEngine->DelSocket(this);
	CloseSocket(Sock);
}

/** Really recieve something from the buffer
 * @param buf The buf to read to
 * @param sz How much to read
 * @return Number of bytes recieved
 */
int Socket::RecvInternal(char *buf, size_t sz) const
{
	return recv(GetSock(), buf, sz, 0);
}

/** Really write something to the socket
 * @param buf What to write
 * @return Number of bytes written
 */
int Socket::SendInternal(const Anope::string &buf) const
{
	return send(GetSock(), buf.c_str(), buf.length(), 0);
}

/** Get the socket FD for this socket
 * @return the fd
 */
int Socket::GetSock() const
{
	return Sock;
}

/** Mark a socket as blockig
 * @return true if the socket is now blocking
 */
bool Socket::SetBlocking()
{
#ifdef _WIN32
	unsigned long opt = 0;
	return !ioctlsocket(this->GetSock(), FIONBIO, &opt);
#else
	int flags = fcntl(this->GetSock(), F_GETFL, 0);
	return !fcntl(this->GetSock(), F_SETFL, flags & ~O_NONBLOCK);
#endif
}

/** Mark a socket as non-blocking
 * @return true if the socket is now non-blocking
 */
bool Socket::SetNonBlocking()
{
#ifdef _WIN32
	unsigned long opt = 0;
	return !ioctlsocket(this->GetSock(), FIONBIO, &opt);
#else
	int flags = fcntl(this->GetSock(), F_GETFL, 0);
	return !fcntl(this->GetSock(), F_SETFL, flags | O_NONBLOCK);
#endif
}

/** Check if this socket is IPv6
 * @return true or false
 */
bool Socket::IsIPv6() const
{
	return IPv6;
}

/** Get the length of the read buffer
 * @return The length of the read buffer
 */
size_t Socket::ReadBufferLen() const
{
	return RecvLen;
}

/** Get the length of the write buffer
 * @return The length of the write buffer
 */
size_t Socket::WriteBufferLen() const
{
	return WriteBuffer.length();
}

/** Called when there is something to be recieved for this socket
 * @return true on success, false to drop this socket
 */
bool Socket::ProcessRead()
{
	char tbuffer[NET_BUFSIZE] = "";

	RecvLen = RecvInternal(tbuffer, sizeof(tbuffer) - 1);
	if (RecvLen <= 0)
		return false;

	std::string sbuffer = extrabuf;
	sbuffer.append(tbuffer);
	extrabuf.clear();
	size_t lastnewline = sbuffer.rfind('\n');
	if (lastnewline == std::string::npos)
	{
		extrabuf = sbuffer;
		return true;
	}
	if (lastnewline < sbuffer.size() - 1)
	{
		extrabuf = sbuffer.substr(lastnewline);
		TrimBuf(extrabuf);
		sbuffer = sbuffer.substr(0, lastnewline);
	}

	sepstream stream(sbuffer, '\n');

	Anope::string tbuf;
	while (stream.GetToken(tbuf))
	{
		std::string tmp_tbuf = tbuf.str();
		TrimBuf(tmp_tbuf);
		tbuf = tmp_tbuf;

		if (!tbuf.empty())
			if (!Read(tbuf))
				return false;
	}

	return true;
}

/** Called when the socket is ready to be written to
 * @return true on success, false to drop this socket
 */
bool Socket::ProcessWrite()
{
	if (WriteBuffer.empty())
	{
		return true;
	}
	if (SendInternal(WriteBuffer) == -1)
	{
		return false;
	}
	WriteBuffer.clear();
	SocketEngine->ClearWritable(this);

	return true;
}

/** Called when there is an error for this socket
 * @return true on success, false to drop this socket
 */
void Socket::ProcessError()
{
}

/** Called with a line recieved from the socket
 * @param buf The line
 * @return true to continue reading, false to drop the socket
 */
bool Socket::Read(const Anope::string &buf)
{
	return false;
}

/** Write to the socket
 * @param message The message
 */
void Socket::Write(const char *message, ...)
{
	va_list vi;
	char tbuffer[BUFSIZE];

	if (!message)
		return;

	va_start(vi, message);
	vsnprintf(tbuffer, sizeof(tbuffer), message, vi);
	va_end(vi);

	Anope::string sbuf = tbuffer;
	Write(sbuf);
}

/** Write to the socket
 * @param message The message
 */
void Socket::Write(const Anope::string &message)
{
	WriteBuffer.append(message.str() + "\r\n");
	SocketEngine->MarkWritable(this);
}

/** Constructor
 * @param nLS The listen socket this connection came from
 * @param nu The user using this socket
 * @param nsock The socket
 * @param nIPv6 IPv6
 * @param type The socket type, defaults to SOCK_STREAM
 */
ClientSocket::ClientSocket(const Anope::string &nTargetHost, int nPort, const Anope::string &nBindHost, bool nIPv6, int type) : Socket(0, nIPv6, type), TargetHost(nTargetHost), Port(nPort), BindHost(nBindHost)
{
	this->SetNonBlocking();

	if (!BindHost.empty())
	{
		sockaddrs bindaddr;
		bindaddr.pton(IPv6 ? AF_INET6 : AF_INET, BindHost, 0);
		if (bind(Sock, &bindaddr.sa, bindaddr.size()) == -1)
			throw SocketException(Anope::string("Unable to bind to address: ") + strerror(errno));
	}

	sockaddrs conaddr;
	conaddr.pton(IPv6 ? AF_INET6 : AF_INET, TargetHost, Port);
	if (connect(Sock, &conaddr.sa, conaddr.size()) == -1 && errno != EINPROGRESS)
		throw SocketException(Anope::string("Error connecting to server: ") + strerror(errno));
}

/** Default destructor
 */
ClientSocket::~ClientSocket()
{
}

/** Called with a line recieved from the socket
 * @param buf The line
 * @return true to continue reading, false to drop the socket
 */
bool ClientSocket::Read(const Anope::string &buf)
{
	return true;
}

/** Constructor
 * @param bind The IP to bind to
 * @param port The port to listen on
 */
ListenSocket::ListenSocket(const Anope::string &bindip, int port) : Socket(0, (bindip.find(':') != Anope::string::npos ? true : false))
{
	Type = SOCKTYPE_LISTEN;
	BindIP = bindip;
	Port = port;

	this->SetNonBlocking();

	sockaddrs sockaddr;
	sockaddr.pton(IPv6 ? AF_INET6 : AF_INET, BindIP, Port);

	if (bind(Sock, &sockaddr.sa, sockaddr.size()) == -1)
		throw SocketException(Anope::string("Unable to bind to address: ") + strerror(errno));

	if (listen(Sock, 5) == -1)
		throw SocketException(Anope::string("Unable to listen: ") + strerror(errno));
}

/** Destructor
 */
ListenSocket::~ListenSocket()
{
}

/** Accept a connection in this sockets queue
 */
bool ListenSocket::ProcessRead()
{
	int newsock = accept(Sock, NULL, NULL);

#ifndef INVALID_SOCKET
# define INVALID_SOCKET 0
#endif

	if (newsock > 0 && newsock != INVALID_SOCKET)
		return this->OnAccept(new Socket(newsock, IPv6));

	return true;
}

/** Called when a connection is accepted
 * @param s The socket for the new connection
 * @return true if the listen socket should remain alive
 */
bool ListenSocket::OnAccept(Socket *s)
{
	return true;
}

/** Get the bind IP for this socket
 * @return the bind ip
 */
const Anope::string &ListenSocket::GetBindIP() const
{
	return BindIP;
}

/** Get the port this socket is bound to
 * @return The port
 */
int ListenSocket::GetPort() const
{
	return Port;
}
