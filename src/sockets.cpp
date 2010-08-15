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

SocketEngineBase::SocketEngineBase()
{
#ifdef _WIN32
	if (WSAStartup(MAKEWORD(2, 0), &wsa))
		Alog() << "Failed to initialize WinSock library";
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
 */
Socket::Socket(int nsock, bool nIPv6)
{
	Type = SOCKTYPE_CLIENT;
	IPv6 = nIPv6;
	if (nsock == 0)
		Sock = socket(IPv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
	else
		Sock = nsock;
	SocketEngine->AddSocket(this);
}

/** Default destructor
*/
Socket::~Socket()
{
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

/** Called when there is something to be written to this socket
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
	SocketEngine->ClearWriteable(this);

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
	SocketEngine->MarkWriteable(this);
}

/** Constructor
 * @param nLS The listen socket this connection came from
 * @param nu The user using this socket
 * @param nsock The socket
 * @param nIPv6 IPv6
 */
ClientSocket::ClientSocket(const Anope::string &nTargetHost, int nPort, const Anope::string &nBindHost, bool nIPv6) : Socket(0, nIPv6), TargetHost(nTargetHost), Port(nPort), BindHost(nBindHost)
{
	addrinfo hints;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_family = IPv6 ? AF_INET6 : AF_INET;

	if (!BindHost.empty())
	{
		addrinfo *bindar;
		sockaddr_in bindaddr;
		sockaddr_in6 bindaddr6;

		if (!getaddrinfo(BindHost.c_str(), NULL, &hints, &bindar))
		{
			if (IPv6)
				memcpy(&bindaddr6, bindar->ai_addr, bindar->ai_addrlen);
			else
				memcpy(&bindaddr, bindar->ai_addr, bindar->ai_addrlen);

			freeaddrinfo(bindar);
		}
		else
		{
			if (IPv6)
			{
				bindaddr6.sin6_family = AF_INET6;

				if (inet_pton(AF_INET6, BindHost.c_str(), &bindaddr6.sin6_addr) < 1)
					throw SocketException(Anope::string("Invalid bind host: ") + strerror(errno));
			}
			else
			{
				bindaddr.sin_family = AF_INET;

				if (inet_pton(AF_INET, BindHost.c_str(), &bindaddr.sin_addr) < 1)
					throw SocketException(Anope::string("Invalid bind host: ") + strerror(errno));
			}
		}

		if (IPv6)
		{
			if (bind(Sock, reinterpret_cast<sockaddr *>(&bindaddr6), sizeof(bindaddr6)) == -1)
				throw SocketException(Anope::string("Unable to bind to address: ") + strerror(errno));
		}
		else
		{
			if (bind(Sock, reinterpret_cast<sockaddr *>(&bindaddr), sizeof(bindaddr)) == -1)
				throw SocketException(Anope::string("Unable to bind to address: ") + strerror(errno));
		}
	}

	addrinfo *conar;
	sockaddr_in6 addr6;
	sockaddr_in addr;

	if (!getaddrinfo(TargetHost.c_str(), NULL, &hints, &conar))
	{
		if (IPv6)
			memcpy(&addr6, conar->ai_addr, conar->ai_addrlen);
		else
			memcpy(&addr, conar->ai_addr, conar->ai_addrlen);

		freeaddrinfo(conar);
	}
	else
	{
		if (IPv6)
		{
			if (inet_pton(AF_INET6, TargetHost.c_str(), &addr6.sin6_addr) < 1)
				throw SocketException(Anope::string("Invalid server host: ") + strerror(errno));
		}
		else
		{
			if (inet_pton(AF_INET, TargetHost.c_str(), &addr.sin_addr) < 1)
				throw SocketException(Anope::string("Invalid server host: ") + strerror(errno));
		}
	}

	if (IPv6)
	{
		addr6.sin6_family = AF_INET6;
		addr6.sin6_port = htons(nPort);

		if (connect(Sock, reinterpret_cast<sockaddr *>(&addr6), sizeof(addr6)) == -1)
			throw SocketException(Anope::string("Error connecting to server: ") + strerror(errno));
	}
	else
	{
		addr.sin_family = AF_INET;
		addr.sin_port = htons(nPort);

		if (connect(Sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1)
			throw SocketException(Anope::string("Error connecting to server: ") + strerror(errno));
	}

	this->SetNonBlocking();
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

	sockaddr_in sock_addr;
	sockaddr_in6 sock_addr6;

	if (IPv6)
	{
		sock_addr6.sin6_family = AF_INET6;
		sock_addr6.sin6_port = htons(port);

		if (inet_pton(AF_INET6, bindip.c_str(), &sock_addr6.sin6_addr) < 1)
			throw SocketException(Anope::string("Invalid bind host: ") + strerror(errno));
	}
	else
	{
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_port = htons(port);

		if (inet_pton(AF_INET, bindip.c_str(), &sock_addr.sin_addr) < 1)
			throw SocketException(Anope::string("Invalid bind host: ") + strerror(errno));
	}

	if (IPv6)
	{
		if (bind(Sock, reinterpret_cast<sockaddr *>(&sock_addr6), sizeof(sock_addr6)) == -1)
			throw SocketException(Anope::string("Unable to bind to address: ") + strerror(errno));
	}
	else
	{
		if (bind(Sock, reinterpret_cast<sockaddr *>(&sock_addr), sizeof(sock_addr)) == -1)
			throw SocketException(Anope::string("Unable to bind to address: ") + strerror(errno));
	}

	if (listen(Sock, 5) == -1)
		throw SocketException(Anope::string("Unable to listen: ") + strerror(errno));

	this->SetNonBlocking();
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

#ifndef _WIN32
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
