#include "services.h"

std::map<int, Socket *> SocketEngine::Sockets;

int32_t TotalRead = 0;
int32_t TotalWritten = 0;

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
 * @param buf The data to write
 * @param size The length of the data
 */
int SocketIO::Send(Socket *s, const char *buf, size_t sz)
{
	size_t i = send(s->GetFD(), buf, sz, 0);
	TotalWritten += i;
	return i;
}
int SocketIO::Send(Socket *s, const Anope::string &buf)
{
	return this->Send(s, buf.c_str(), buf.length());
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

	if (newsock >= 0)
	{
		ClientSocket *ns = s->OnAccept(newsock, conaddr);
		ns->SetFlag(SF_ACCEPTED);
		ns->OnAccept();
		return ns;
	}
	else
		throw SocketException("Unable to accept connection: " + Anope::LastError());
}

/** Finished accepting a connection from a socket
 * @param s The socket
 * @return SF_ACCEPTED if accepted, SF_ACCEPTING if still in process, SF_DEAD on error
 */
SocketFlag SocketIO::FinishAccept(ClientSocket *cs)
{
	return SF_ACCEPTED;
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
	s->UnsetFlag(SF_CONNECTING);
	s->UnsetFlag(SF_CONNECTED);
	s->conaddr.pton(s->IsIPv6() ? AF_INET6 : AF_INET, target, port);
	int c = connect(s->GetFD(), &s->conaddr.sa, s->conaddr.size());
	if (c == -1)
	{
		if (Anope::LastErrorCode() != EINPROGRESS)
			s->OnError(Anope::LastError());
		else
		{
			SocketEngine::MarkWritable(s);
			s->SetFlag(SF_CONNECTING);
		}
	}
	else
	{
		s->SetFlag(SF_CONNECTED);
		s->OnConnect();
	}
}

/** Called to potentially finish a pending connection
 * @param s The socket
 * @return SF_CONNECTED on success, SF_CONNECTING if still pending, and SF_DEAD on error.
 */
SocketFlag SocketIO::FinishConnect(ConnectionSocket *s)
{
	if (s->HasFlag(SF_CONNECTED))
		return SF_CONNECTED;
	else if (!s->HasFlag(SF_CONNECTING))
		throw SocketException("SocketIO::FinishConnect called for a socket not connected nor connecting?");

	int optval = 0;
	socklen_t optlen = sizeof(optval);
	if (!getsockopt(s->GetFD(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&optval), &optlen) && !optval)
	{
		s->SetFlag(SF_CONNECTED);
		s->UnsetFlag(SF_CONNECTING);
		s->OnConnect();
		return SF_CONNECTED;
	}
	else
	{
		errno = optval;
		s->OnError(optval ? Anope::LastError() : "");
		return SF_DEAD;
	}
}

/** Empty constructor, should not be called.
 */
Socket::Socket() : Flags<SocketFlag>(SocketFlagStrings)
{
	throw CoreException("Socket::Socket() ?");
}

/** Constructor
 * @param sock The socket
 * @param ipv6 IPv6?
 * @param type The socket type, defaults to SOCK_STREAM
 */
Socket::Socket(int sock, bool ipv6, int type) : Flags<SocketFlag>(SocketFlagStrings)
{
	this->IO = &normalSocketIO;
	this->IPv6 = ipv6;
	if (sock == -1)
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
	close(this->Sock);
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
	int flags = fcntl(this->GetFD(), F_GETFL, 0);
	return !fcntl(this->GetFD(), F_SETFL, flags & ~O_NONBLOCK);
}

/** Mark a socket as non-blocking
 * @return true if the socket is now non-blocking
 */
bool Socket::SetNonBlocking()
{
	int flags = fcntl(this->GetFD(), F_GETFL, 0);
	return !fcntl(this->GetFD(), F_SETFL, flags | O_NONBLOCK);
}

/** Bind the socket to an ip and port
 * @param ip The ip
 * @param port The port
 */
void Socket::Bind(const Anope::string &ip, int port)
{
	this->IO->Bind(this, ip, port);
}

/** Called when there either is a read or write event.
 * @return true to continue to call ProcessRead/ProcessWrite, false to not continue
 */
bool Socket::Process()
{
	return true;
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

/** Constructor
 * @param bindip The IP to bind to
 * @param port The port to listen on
 * @param ipv6 true for ipv6
 */
ListenSocket::ListenSocket(const Anope::string &bindip, int port, bool ipv6) : Socket(-1, ipv6)
{
	this->SetNonBlocking();

	const char op = 1;
	setsockopt(this->GetFD(), SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));

	this->bindaddr.pton(IPv6 ? AF_INET6 : AF_INET, bindip, port);
	this->IO->Bind(this, bindip, port);

	if (listen(Sock, SOMAXCONN) == -1)
		throw SocketException("Unable to listen: " + Anope::LastError());
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

