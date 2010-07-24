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

/** Constructor
 * @param nsock The socket
 * @param nIPv6 IPv6?
 */
Socket::Socket(int nsock, bool nIPv6)
{
	Type = SOCKTYPE_CLIENT;
	IPv6 = nIPv6;
	if (nsock == 0)
		sock = socket(IPv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
	else
		sock = nsock;
	SocketEngine->AddSocket(this);
}

/** Default destructor
*/
Socket::~Socket()
{
	SocketEngine->DelSocket(this);
	CloseSocket(sock);
}

/** Really recieve something from the buffer
 * @param buf The buf to read to
 * @param sz How much to read
 * @return Number of bytes recieved
 */
const int Socket::RecvInternal(char *buf, size_t sz) const
{
	return recv(GetSock(), buf, sz, 0);
}

/** Really write something to the socket
 * @param buf What to write
 * @return Number of bytes written
 */
const int Socket::SendInternal(const std::string &buf) const
{
	return send(GetSock(), buf.c_str(), buf.length(), 0);
}

/** Get the socket FD for this socket
 * @return the fd
 */
int Socket::GetSock() const
{
	return sock;
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
	char tbuffer[NET_BUFSIZE];
	memset(&tbuffer, '\0', sizeof(tbuffer));

	RecvLen = RecvInternal(tbuffer, sizeof(tbuffer) - 1);
	if (RecvLen <= 0)
		return false;

	std::string sbuffer = extrabuf;
	sbuffer.append(tbuffer);
	extrabuf.clear();
	size_t lastnewline = sbuffer.find_last_of('\n');
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

	std::string tbuf;
	while (stream.GetToken(tbuf))
	{
		TrimBuf(tbuf);

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
bool Socket::Read(const std::string &buf)
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
	std::string sbuf;
	
	if (!message)
		return;

	va_start(vi, message);
	vsnprintf(tbuffer, sizeof(tbuffer), message, vi);
	va_end(vi);

	sbuf = tbuffer;
	Write(sbuf);
}

/** Write to the socket
 * @param message The message
 */
void Socket::Write(const std::string &message)
{
	WriteBuffer.append(message + "\r\n");
	SocketEngine->MarkWriteable(this);
}

/** Constructor
 * @param nLS The listen socket this connection came from
 * @param nu The user using this socket
 * @param nsock The socket
 * @param nIPv6 IPv6
 */
ClientSocket::ClientSocket(const std::string &nTargetHost, int nPort, const std::string &nBindHost, bool nIPv6) : Socket(0, nIPv6), TargetHost(nTargetHost), Port(nPort), BindHost(nBindHost)
{
	if (!IPv6 && (TargetHost.find(':') != std::string::npos || BindHost.find(':') != std::string::npos))
		IPv6 = true;
	
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

		if (getaddrinfo(BindHost.c_str(), NULL, &hints, &bindar) == 0)
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
					throw SocketException("Invalid bind host: " + std::string(strerror(errno)));
			}
			else
			{
				bindaddr.sin_family = AF_INET;

				if (inet_pton(AF_INET, BindHost.c_str(), &bindaddr.sin_addr) < 1)
					throw SocketException("Invalid bind host: " + std::string(strerror(errno)));
			}
		}

		if (IPv6)
		{
			if (bind(sock, reinterpret_cast<sockaddr *>(&bindaddr6), sizeof(bindaddr6)) == -1)
				throw SocketException("Unable to bind to address: " + std::string(strerror(errno)));
		}
		else
		{
			if (bind(sock, reinterpret_cast<sockaddr *>(&bindaddr), sizeof(bindaddr)) == -1)
				throw SocketException("Unable to bind to address: " + std::string(strerror(errno)));
		}
	}

	addrinfo *conar;
	sockaddr_in6 addr6;
	sockaddr_in addr;

	if (getaddrinfo(TargetHost.c_str(), NULL, &hints, &conar) == 0)
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
				throw SocketException("Invalid server host: " + std::string(strerror(errno)));
		}
		else
		{
			if (inet_pton(AF_INET, TargetHost.c_str(), &addr.sin_addr) < 1)
				throw SocketException("Invalid server host: " + std::string(strerror(errno)));
		}
	}

	if (IPv6)
	{
		addr6.sin6_family = AF_INET6;
		addr6.sin6_port = htons(nPort);

		if (connect(sock, reinterpret_cast<sockaddr *>(&addr6), sizeof(addr6)) == -1)
		{
			throw SocketException("Error connecting to server: " + std::string(strerror(errno)));
		}
	}
	else
	{
		addr.sin_family = AF_INET;
		addr.sin_port = htons(nPort);

		if (connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1)
		{
			throw SocketException("Error connecting to server: " + std::string(strerror(errno)));
		}
	}
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
bool ClientSocket::Read(const std::string &buf)
{
	return true;
}

/** Constructor
 * @param bind The IP to bind to
 * @param port The port to listen on
 */
ListenSocket::ListenSocket(const std::string &bindip, int port) : Socket(0, (bindip.find(':') != std::string::npos ? true : false))
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
		{
			throw SocketException("Invalid bind host: " + std::string(strerror(errno)));
		}
	}
	else
	{
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_port = htons(port);

		if (inet_pton(AF_INET, bindip.c_str(), &sock_addr.sin_addr) < 1)
		{
			throw SocketException("Invalid bind host: " + std::string(strerror(errno)));
		}
	}

	if (IPv6)
	{
		if (bind(sock, reinterpret_cast<sockaddr *>(&sock_addr6), sizeof(sock_addr6)) == -1)
		{
			throw SocketException("Unable to bind to address: " + std::string(strerror(errno)));
		}
	}
	else
	{
		if (bind(sock, reinterpret_cast<sockaddr *>(&sock_addr), sizeof(sock_addr)) == -1)
		{
			throw SocketException("Unable to bind to address: " + std::string(strerror(errno)));
		}
	}

	if (listen(sock, 5) == -1)
	{
		throw SocketException("Unable to listen: " + std::string(strerror(errno)));
	}
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
	int newsock = accept(sock, NULL, NULL);

#ifndef _WIN32
# define INVALID_SOCKET 0
#endif

	if (newsock > 0 && newsock != INVALID_SOCKET)
	{
		return this->OnAccept(new Socket(newsock, IPv6));
	}

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
const std::string &ListenSocket::GetBindIP() const
{
        return BindIP;
}

/** Get the port this socket is bound to
 * @return The port
 */
const int ListenSocket::GetPort() const
{
	return Port;
}

