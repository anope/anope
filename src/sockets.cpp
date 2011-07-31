#include "services.h"

std::map<int, Socket *> SocketEngine::Sockets;

int32 TotalRead = 0;
int32 TotalWritten = 0;

SocketIO normalSocketIO;

/** Construct the object, sets everything to 0
 */
sockaddrs::sockaddrs()
{
	this->clear();
}

/** Memset the object to 0
 */
void sockaddrs::clear()
{
	memset(this, 0, sizeof(*this));
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
				throw SocketException(Anope::LastError());
			return address;
		case AF_INET6:
			if (!inet_ntop(AF_INET6, &sa6.sin6_addr, address, sizeof(address)))
				throw SocketException(Anope::LastError());
			return address;
		default:
			break;
	}

	return address;
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
		{
			int i = inet_pton(type, address.c_str(), &sa4.sin_addr);
			if (i == 0)
				throw SocketException("Invalid host");
			else if (i <= -1)
				throw SocketException("Invalid host: "  + Anope::LastError());
			sa4.sin_family = type;
			sa4.sin_port = htons(pport);
			return;
		}
		case AF_INET6:
		{
			int i = inet_pton(type, address.c_str(), &sa6.sin6_addr);
			if (i == 0)
				throw SocketException("Invalid host");
			else if (i <= -1)
				throw SocketException("Invalid host: " + Anope::LastError());
			sa6.sin6_family = type;
			sa6.sin6_port = htons(pport);
			return;
		}
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

cidr::cidr(const Anope::string &ip)
{
	if (ip.find_first_not_of("01234567890:./") != Anope::string::npos)
		throw SocketException("Invalid IP");

	bool ipv6 = ip.find(':') != Anope::string::npos;
	size_t sl = ip.find_last_of('/');
	if (sl == Anope::string::npos)
	{
		this->cidr_ip = ip;
		this->cidr_len = ipv6 ? 128 : 32;
		this->addr.pton(ipv6 ? AF_INET6 : AF_INET, ip);
	}
	else
	{
		Anope::string real_ip = ip.substr(0, sl);
		Anope::string cidr_range = ip.substr(sl + 1);
		if (!cidr_range.is_pos_number_only())
			throw SocketException("Invalid CIDR range");

		this->cidr_ip = real_ip;
		this->cidr_len = convertTo<unsigned int>(cidr_range);
		this->addr.pton(ipv6 ? AF_INET6 : AF_INET, real_ip);
	}
}

cidr::cidr(const Anope::string &ip, unsigned char len)
{
	bool ipv6 = ip.find(':') != Anope::string::npos;
	this->addr.pton(ipv6 ? AF_INET6 : AF_INET, ip);
	this->cidr_ip = ip;
	this->cidr_len = len;
}

Anope::string cidr::mask() const
{
	return this->cidr_ip + "/" + this->cidr_len;
}

bool cidr::match(sockaddrs &other)
{
	if (this->addr.sa.sa_family != other.sa.sa_family)
		return false;

	unsigned char *ip, *their_ip, byte;

	switch (this->addr.sa.sa_family)
	{
		case AF_INET:
			ip = reinterpret_cast<unsigned char *>(&this->addr.sa4.sin_addr);
			byte = this->cidr_len / 8;
			their_ip = reinterpret_cast<unsigned char *>(&other.sa4.sin_addr);
			break;
		case AF_INET6:
			ip = reinterpret_cast<unsigned char *>(&this->addr.sa6.sin6_addr);
			byte = this->cidr_len / 8;
			their_ip = reinterpret_cast<unsigned char *>(&other.sa6.sin6_addr);
			break;
		default:
			throw SocketException("Invalid address type");
	}
	
	if (memcmp(ip, their_ip, byte))
		return false;

	ip += byte;
	their_ip += byte;
	byte = this->cidr_len % 8;
	if ((*ip & byte) != (*their_ip & byte))
		return false;
	
	return true;
}

/** Receive something from the buffer
 * @param s The socket
 * @param buf The buf to read to
 * @param sz How much to read
 * @return Number of bytes received
 */
int SocketIO::Recv(Socket *s, char *buf, size_t sz)
{
	size_t i = recv(s->GetFD(), buf, sz, 0);
	TotalRead += i;
	return i;
}

/** Write something to the socket
 * @param s The socket
 * @param buf What to write
 * @return Number of bytes written
 */
int SocketIO::Send(Socket *s, const Anope::string &buf)
{
	size_t i = send(s->GetFD(), buf.c_str(), buf.length(), 0);
	TotalWritten += i;
	return i;
}

/** Accept a connection from a socket
 * @param s The socket
 * @return The new client socket
 */
ClientSocket *SocketIO::Accept(ListenSocket *s)
{
	sockaddrs conaddr;

	socklen_t size = sizeof(conaddr);
	int newsock = accept(s->GetFD(), &conaddr.sa, &size);

#ifndef INVALID_SOCKET
# define INVALID_SOCKET -1
#endif

	if (newsock >= 0 && newsock != INVALID_SOCKET)
		return s->OnAccept(newsock, conaddr);
	else
		throw SocketException("Unable to accept connection: " + Anope::LastError());
}

/** Check if a connection has been accepted
 * @param s The client socket
 * @return -1 on error, 0 to wait, 1 on success
 */
int SocketIO::Accepted(ClientSocket *cs)
{
	return 1;
}

/** Bind a socket
 * @param s The socket
 * @param ip The IP to bind to
 * @param port The optional port to bind to
 */
void SocketIO::Bind(Socket *s, const Anope::string &ip, int port)
{
	s->bindaddr.pton(s->IsIPv6() ? AF_INET6 : AF_INET, ip, port);
	if (bind(s->GetFD(), &s->bindaddr.sa, s->bindaddr.size()) == -1)
		throw SocketException("Unable to bind to address: " + Anope::LastError());
}

/** Connect the socket
  * @param s THe socket
  * @param target IP to connect to
  * @param port to connect to
  */
void SocketIO::Connect(ConnectionSocket *s, const Anope::string &target, int port)
{
	s->conaddr.pton(s->IsIPv6() ? AF_INET6 : AF_INET, target, port);
	int c = connect(s->GetFD(), &s->conaddr.sa, s->conaddr.size());
	if (c == -1)
	{
		if (Anope::LastErrorCode() != EINPROGRESS)
			throw SocketException("Error connecting to server: " + Anope::LastError());
		else
			SocketEngine::MarkWritable(s);
	}
	else
	{
		s->connected = true;
		s->OnConnect();
	}
}

/** Check if this socket is connected
 * @param s The socket
 * @return -1 for error, 0 for wait, 1 for connected
 */
int SocketIO::Connected(ConnectionSocket *s)
{
	return s->connected == true ? 1 : -1;
}

/** Empty constructor, used for things such as the pipe socket
 */
Socket::Socket() : Flags<SocketFlag, 2>(SocketFlagStrings)
{
	this->Type = SOCKTYPE_BASE;
	this->IO = &normalSocketIO;
}

/** Constructor
 * @param sock The socket
 * @param ipv6 IPv6?
 * @param type The socket type, defaults to SOCK_STREAM
 */
Socket::Socket(int sock, bool ipv6, int type) : Flags<SocketFlag, 2>(SocketFlagStrings)
{
	this->Type = SOCKTYPE_BASE;
	this->IO = &normalSocketIO;
	this->IPv6 = ipv6;
	if (sock == 0)
		this->Sock = socket(this->IPv6 ? AF_INET6 : AF_INET, type, 0);
	else
		this->Sock = sock;
	this->SetNonBlocking();
	SocketEngine::AddSocket(this);
}

/** Default destructor
*/
Socket::~Socket()
{
	SocketEngine::DelSocket(this);
	CloseSocket(this->Sock);
	this->IO->Destroy();
}

/** Get the socket FD for this socket
 * @return the fd
 */
int Socket::GetFD() const
{
	return Sock;
}

/** Check if this socket is IPv6
 * @return true or false
 */
bool Socket::IsIPv6() const
{
	return IPv6;
}

/** Mark a socket as blockig
 * @return true if the socket is now blocking
 */
bool Socket::SetBlocking()
{
#ifdef _WIN32
	unsigned long opt = 0;
	return !ioctlsocket(this->GetFD(), FIONBIO, &opt);
#else
	int flags = fcntl(this->GetFD(), F_GETFL, 0);
	return !fcntl(this->GetFD(), F_SETFL, flags & ~O_NONBLOCK);
#endif
}

/** Mark a socket as non-blocking
 * @return true if the socket is now non-blocking
 */
bool Socket::SetNonBlocking()
{
#ifdef _WIN32
	unsigned long opt = 1;
	return !ioctlsocket(this->GetFD(), FIONBIO, &opt);
#else
	int flags = fcntl(this->GetFD(), F_GETFL, 0);
	return !fcntl(this->GetFD(), F_SETFL, flags | O_NONBLOCK);
#endif
}

/** Bind the socket to an ip and port
 * @param ip The ip
 * @param port The port
 */
void Socket::Bind(const Anope::string &ip, int port)
{
	this->IO->Bind(this, ip, port);
}

/** Called when there is something to be received for this socket
 * @return true on success, false to drop this socket
 */
bool Socket::ProcessRead()
{
	return true;
}

/** Called when the socket is ready to be written to
 * @return true on success, false to drop this socket
 */
bool Socket::ProcessWrite()
{
	return true;
}

/** Called when there is an error for this socket
 * @return true on success, false to drop this socket
 */
void Socket::ProcessError()
{
}

/** Constructor for pipe socket
 */
BufferedSocket::BufferedSocket() : Socket()
{
	this->Type = SOCKTYPE_BUFFERED;
}

/** Constructor
 * @param fd FD to use
 * @param ipv6 true for ipv6
 * @param type socket type, defaults to SOCK_STREAM
 */
BufferedSocket::BufferedSocket(int fd, bool ipv6, int type) : Socket(fd, ipv6, type)
{
	this->Type = SOCKTYPE_BUFFERED;
}

/** Default destructor
 */
BufferedSocket::~BufferedSocket()
{
}

/** Called when there is something to be received for this socket
 * @return true on success, false to drop this socket
 */
bool BufferedSocket::ProcessRead()
{
	char tbuffer[NET_BUFSIZE];

	this->RecvLen = 0;

	int len = this->IO->Recv(this, tbuffer, sizeof(tbuffer) - 1);
	if (len <= 0)
		return false;
	
	tbuffer[len] = 0;
	this->RecvLen = len;

	Anope::string sbuffer = this->extrabuf;
	sbuffer += tbuffer;
	this->extrabuf.clear();
	size_t lastnewline = sbuffer.rfind('\n');
	if (lastnewline == Anope::string::npos)
	{
		this->extrabuf = sbuffer;
		return true;
	}
	if (lastnewline < sbuffer.length() - 1)
	{
		this->extrabuf = sbuffer.substr(lastnewline);
		this->extrabuf.trim();
		sbuffer = sbuffer.substr(0, lastnewline);
	}

	sepstream stream(sbuffer, '\n');

	Anope::string tbuf;
	while (stream.GetToken(tbuf))
	{
		tbuf.trim();
		if (!tbuf.empty() && !Read(tbuf))
			return false;
	}

	return true;
}

/** Called when the socket is ready to be written to
 * @return true on success, false to drop this socket
 */
bool BufferedSocket::ProcessWrite()
{
	int count = this->IO->Send(this, this->WriteBuffer);
	if (count <= -1)
		return false;
	this->WriteBuffer = this->WriteBuffer.substr(count);
	if (this->WriteBuffer.empty())
		SocketEngine::ClearWritable(this);

	return true;
}

/** Called with a line received from the socket
 * @param buf The line
 * @return true to continue reading, false to drop the socket
 */
bool BufferedSocket::Read(const Anope::string &buf)
{
	return false;
}

/** Write to the socket
 * @param message The message
 */
void BufferedSocket::Write(const char *message, ...)
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
void BufferedSocket::Write(const Anope::string &message)
{
	this->WriteBuffer += message + "\r\n";
	SocketEngine::MarkWritable(this);
}

/** Get the length of the read buffer
 * @return The length of the read buffer
 */
int BufferedSocket::ReadBufferLen() const
{
	return RecvLen;
}

/** Get the length of the write buffer
 * @return The length of the write buffer
 */
int BufferedSocket::WriteBufferLen() const
{
	return this->WriteBuffer.length();
}

/** Constructor
 * @param bindip The IP to bind to
 * @param port The port to listen on
 * @param ipv6 true for ipv6
 */
ListenSocket::ListenSocket(const Anope::string &bindip, int port, bool ipv6) : Socket(0, ipv6)
{
	this->Type = SOCKTYPE_LISTEN;
	this->SetNonBlocking();

	this->bindaddr.pton(IPv6 ? AF_INET6 : AF_INET, bindip, port);
	this->IO->Bind(this, bindip, port);

	if (listen(Sock, SOMAXCONN) == -1)
		throw SocketException(Anope::string("Unable to listen: ") + Anope::LastError());
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
	try
	{
		this->IO->Accept(this);
	}
	catch (const SocketException &ex)
	{
		Log() << ex.GetReason();
	}
	return true;
}

/** Called when a connection is accepted
 * @param fd The FD for the new connection
 * @param addr The sockaddr for where the connection came from
 * @return The new socket
 */
ClientSocket *ListenSocket::OnAccept(int fd, const sockaddrs &addr)
{
	return new ClientSocket(this, fd, addr);
}

/** Constructor
 * @param ipv6 true to use IPv6
 * @param type The socket type, defaults to SOCK_STREAM
 */
ConnectionSocket::ConnectionSocket(bool ipv6, int type) : BufferedSocket(0, ipv6, type), connected(false)
{
	this->Type = SOCKTYPE_CONNECTION;
}

/** Connect the socket
 * @param TargetHost The target host to connect to
 * @param Port The target port to connect to
 */
void ConnectionSocket::Connect(const Anope::string &TargetHost, int Port)
{
	this->IO->Connect(this, TargetHost, Port);
}

/** Called when there is something to be received for this socket
 * @return true on success, false to drop this socket
 */
bool ConnectionSocket::ProcessRead()
{
	if (!this->connected)
	{
		int optval = 0;
		socklen_t optlen = sizeof(optval);
		if (!getsockopt(this->GetFD(), SOL_SOCKET, SO_ERROR, &optval, &optlen) && !optval)
		{
			this->connected = true;
			this->OnConnect();
		}
		else
			errno = optval;
	}

	int i = this->IO->Connected(this);
	if (i == 1)
		return BufferedSocket::ProcessRead();
	else if (i == 0)
		return true;

	this->OnError(Anope::LastError());
	return false;
}

/** Called when the socket is ready to be written to
 * @return true on success, false to drop this socket
 */
bool ConnectionSocket::ProcessWrite()
{
	if (!this->connected)
	{
		int optval = 0;
		socklen_t optlen = sizeof(optval);
		if (!getsockopt(this->GetFD(), SOL_SOCKET, SO_ERROR, &optval, &optlen) && !optval)
		{
			this->connected = true;
			this->OnConnect();
		}
		else
			errno = optval;
	}

	int i = this->IO->Connected(this);
	if (i == 1)
		return BufferedSocket::ProcessWrite();
	else if (i == 0)
		return true;
	
	this->OnError(Anope::LastError());
	return false;
}

/** Called when there is an error for this socket
 * @return true on success, false to drop this socket
 */
void ConnectionSocket::ProcessError()
{
}

/** Called on a successful connect
 */
void ConnectionSocket::OnConnect()
{
}

/** Called when a connection is not successful
 * @param error The error
 */
void ConnectionSocket::OnError(const Anope::string &error)
{
}

/** Constructor
 * @param ls Listen socket this connection is from
 * @param fd New FD for this socket
 * @param addr Address the connection came from
 */
ClientSocket::ClientSocket(ListenSocket *ls, int fd, const sockaddrs &addr) : BufferedSocket(fd, ls->IsIPv6()), LS(ls), clientaddr(addr)
{
	this->Type = SOCKTYPE_CLIENT;
}

/** Called when there is something to be received for this socket
 * @return true on success, false to drop this socket
 */
bool ClientSocket::ProcessRead()
{
	int i = this->IO->Accepted(this);
	if (i == 1)
		return BufferedSocket::ProcessRead();
	else if (i == 0)
		return true;
	return false;
}

/** Called when the socket is ready to be written to
 * @return true on success, false to drop this socket
 */
bool ClientSocket::ProcessWrite()
{
	int i = this->IO->Accepted(this);
	if (i == 1)
		return BufferedSocket::ProcessWrite();
	else if (i == 0)
		return true;
	return false;
}

