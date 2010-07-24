#include "services.h"

SocketEngine socketEngine;
int32 TotalRead = 0;
int32 TotalWritten = 0;

/** Trims all the \r and \ns from the begining and end of a string
 * @return A string without trailing \r and \ns
 */
static void TrimBuf(std::string &buffer)
{
	while (!buffer.empty() && (buffer[0] == '\r' || buffer[0] == '\n'))
		buffer.erase(buffer.begin());
	while (!buffer.empty() && (buffer[buffer.length() - 1] == '\r' || buffer[buffer.length() - 1] == '\n'))
		buffer.erase(buffer.length() - 1);
}

/** Default constructor
 * @param nTargetHost Hostname to connect to
 * @param nPort Port to connect to
 * @param nBindHos Host to bind to when connecting
 * @param nIPv6 true to use IPv6
 */
Socket::Socket(const std::string &nTargetHost, int nPort, const std::string &nBindHost, bool nIPv6) : TargetHost(nTargetHost), Port(nPort), BindHost(nBindHost), IPv6(nIPv6)
{
	if (!IPv6 && (TargetHost.find(':') != std::string::npos || BindHost.find(':') != std::string::npos))
		IPv6 = true;
	
	Sock = socket(IPv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);

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

		int Bound = -1;
		if (getaddrinfo(BindHost.c_str(), NULL, &hints, &bindar) == 0)
		{
			if (IPv6)
				memcpy(&bindaddr6, bindar->ai_addr, bindar->ai_addrlen);
			else
				memcpy(&bindaddr, bindar->ai_addr, bindar->ai_addrlen);

			freeaddrinfo(bindar);

			Bound = bind(Sock, reinterpret_cast<sockaddr *>(&bindaddr), sizeof(bindaddr));
		}
		if (Bound < 0)
		{
			if (IPv6)
			{
				bindaddr6.sin6_family = AF_INET6;

				if (inet_pton(AF_INET6, BindHost.c_str(), &bindaddr6.sin6_addr) < 1)
				{
					throw SocketException("Invalid bind host");
				}

				if (bind(Sock, reinterpret_cast<sockaddr *>(&bindaddr6), sizeof(bindaddr6)) == -1)
				{
					throw SocketException("Unable to bind to address");
				}
			}
			else
			{
				bindaddr.sin_family = AF_INET;

				if (inet_pton(bindaddr.sin_family, BindHost.c_str(), &bindaddr.sin_addr) < 1)
				{
					throw SocketException("Invalid bind host");
				}

				if (bind(Sock, reinterpret_cast<sockaddr *>(&bindaddr), sizeof(bindaddr)) == -1)
				{
					throw SocketException("Unable to bind to address");
				}
			}
		}
	}

	addrinfo *conar;
	sockaddr_in conaddr;
	sockaddr_in6 conaddr6;
	if (getaddrinfo(TargetHost.c_str(), NULL, &hints, &conar) == 0)
	{
		if (IPv6)
			memcpy(&conaddr6, conar->ai_addr, conar->ai_addrlen);
		else
			memcpy(&conaddr, conar->ai_addr, conar->ai_addrlen);

		freeaddrinfo(conar);
	}
	else
	{
		if (IPv6)
		{
			if (inet_pton(AF_INET6, TargetHost.c_str(), &conaddr6.sin6_addr) < 1)
			{
				throw SocketException("Invalid server address");
			}
		}
		else
		{
			if (inet_pton(AF_INET, TargetHost.c_str(), &conaddr.sin_addr) < 1)
			{
				throw SocketException("Invalid server address");
			}
		}
	}

	if (IPv6)
	{
		conaddr6.sin6_family = AF_INET6;
		conaddr6.sin6_port = htons(Port);

		if (connect(Sock, reinterpret_cast<sockaddr *>(&conaddr6), sizeof(conaddr6)) < 0)
		{
			throw SocketException("Error connecting to server");
		}
	}
	else
	{
		conaddr.sin_family = AF_INET;
		conaddr.sin_port = htons(Port);

		if (connect(Sock, reinterpret_cast<sockaddr *>(&conaddr), sizeof(conaddr)) < 0)
		{
			throw SocketException("Error connecting to server");
		}
	}

	socketEngine.AddSocket(this);
}

/** Default destructor
 */
Socket::~Socket()
{
	CloseSocket(Sock);

	socketEngine.DelSocket(this);
}

/** Read from the socket
 * @param buf Buffer to read to
 * @param sz How much to read
 * @return Number of bytes recieved
 */
int Socket::RecvInternal(char *buf, size_t sz) const
{
	return recv(GetSock(), buf, sz, 0);
}

/** Write to the socket
 * @param buf What to write
 * @return Number of bytes sent, -1 on error
 */
int Socket::SendInternal(const std::string &buf) const
{
	return send(GetSock(), buf.c_str(), buf.length(), 0);
}

/** Get the socket FD for this socket
 * @return The fd
 */
int Socket::GetSock() const
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

/** Called when there is something to be read from thie socket
 * @return true on success, false to kill this socket
 */
bool Socket::ProcessRead()
{
	char buffer[NET_BUFSIZE];
	memset(&buffer, 0, sizeof(buffer));

	RecvLen = RecvInternal(buffer, sizeof(buffer) - 1);
	if (RecvLen <= 0)
	{
		return false;
	}
	TotalRead += RecvLen;

	std::string sbuffer = extrabuf;
	sbuffer.append(buffer);
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
	std::string buf;

	while (stream.GetToken(buf))
	{
		TrimBuf(buf);

		if (!buf.empty())
		{
			if (!Read(buf))
			{
				return false;
			}
		}
	}

	return true;
}

/** Called when this socket becomes writeable
 * @return true on success, false to drop this socket
 */
bool Socket::ProcessWrite()
{
	int Written = SendInternal(WriteBuffer);
	if (Written == -1)
	{
		return false;
	}
	TotalWritten += Written;

	WriteBuffer.clear();
	return true;
}

/** Called when there is an error on this socket
 */
void Socket::ProcessError()
{
}

/** Called with a message recieved from the socket
 * @param buf The message
 * @return true on success, false to kill this socket
 */
bool Socket::Read(const std::string &buf)
{
	return true;
}

/** Write to the socket
 * @param message The message to write
 */
void Socket::Write(const char *message, ...)
{
	char buf[BUFSIZE];
	va_list vi;
	va_start(vi, message);
	vsnprintf(buf, sizeof(buf), message, vi);
	va_end(vi);

	std::string sbuf = buf;
	Write(sbuf);
}

/** Write to the socket
 * @param message The message to write
 */
void Socket::Write(std::string &message)
{
	WriteBuffer.append(message + "\r\n");
	socketEngine.MarkWriteable(this);
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
	return WriteBuffer.size();
}

/** Constructor
 */
SocketEngine::SocketEngine()
{
	FD_ZERO(&ReadFDs);
	FD_ZERO(&WriteFDs);
	MaxFD = 0;

#ifdef _WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 0), &wsa))
	{
		Alog() << "Failed to initialize WinSock library";
	}
#endif
}

/** Destructor
 */
SocketEngine::~SocketEngine()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

/** Add a socket to the socket engine
 * @param s The socket
 */
void SocketEngine::AddSocket(Socket *s)
{
	if (s->GetSock() > MaxFD)
		MaxFD = s->GetSock();
	FD_SET(s->GetSock(), &ReadFDs);
	Sockets.insert(s);
}

/** Delete a socket from the socket engine
 * @param s The socket
 */
void SocketEngine::DelSocket(Socket *s)
{
	if (s->GetSock() == MaxFD)
		--MaxFD;
	FD_CLR(s->GetSock(), &ReadFDs);
	FD_CLR(s->GetSock(), &WriteFDs);
	Sockets.erase(s);
}

/** Mark a socket as wanting to be written to
 * @param s The socket
 */
void SocketEngine::MarkWriteable(Socket *s)
{
	FD_SET(s->GetSock(), &WriteFDs);
}

/** Unmark a socket as writeable
 * @param s The socket
 */
void SocketEngine::ClearWriteable(Socket *s)
{
	FD_CLR(s->GetSock(), &WriteFDs);
}

/** Called to iterate through each socket and check for activity
 */
void SocketEngine::Process()
{
	fd_set rfdset = ReadFDs, wfdset = WriteFDs, efdset = ReadFDs;
	timeval tval;

	tval.tv_sec = Config.ReadTimeout;
	tval.tv_usec = 0;

	int sresult = select(MaxFD + 1, &rfdset, &wfdset, &efdset, &tval);

	if (sresult == -1)
	{
		Alog() << "SocketEngine::Process error, " << GetError();
	}
	else if (sresult)
	{
		for (std::set<Socket *>::iterator it = Sockets.begin(); it != Sockets.end(); ++it)
		{
			Socket *s = *it;

			if (FD_ISSET(s->GetSock(), &efdset))
			{
				s->ProcessError();
				OldSockets.insert(s);
				continue;
			}
			if (FD_ISSET(s->GetSock(), &rfdset))
			{
				if (!s->ProcessRead())
					OldSockets.insert(s);
			}
			if (FD_ISSET(s->GetSock(), &wfdset))
			{
				ClearWriteable(s);
				if (!s->ProcessWrite())
					OldSockets.insert(s);
			}
		}
	}

	while (!OldSockets.empty())
	{
		delete (*OldSockets.begin());
		OldSockets.erase(OldSockets.begin());
	}
}

/** Get the last socket error
 * @return The error
 */
const std::string SocketEngine::GetError() const
{
#ifdef _WIN32
	errno = WSAGetLastError();
#endif
	switch (errno)
	{
		case EBADF:
			return "Socket error, invalid file descriptor given to select()";
			break;
		case EINTR:
			return "Socket engine caught signal";
			break;
#ifdef WIN32
		case WSANOTINITIALISED:
			return "A successful WSAStartup call must occur before using this function.";
			break;
		case WSAEFAULT:
			return "The Windows Sockets implementation was unable to allocate needed resources for its internal operations, or the readfds, writefds, exceptfds, or timeval parameters are not part of the user address space.";
			break;
		case WSAENETDOWN:
			return "The network subsystem has failed.";
			break;
		case WSAEINVAL:
			return "The time-out value is not valid, or all three descriptor parameters were null.";
			break;
		case WSAEINTR:
			return "A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.";
			break;
		case WSAEINPROGRESS:
			return "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
			break;
		case WSAENOTSOCK:
			return "One of the descriptor sets contains an entry that is not a socket.";
			break;
#endif
		default:
			return "Socket engine caught unknown error";
	}
}

