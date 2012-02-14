/* RequiredLibraries: ssl,crypt */

#include "module.h"
#include "ssl.h"

#define OPENSSL_NO_SHA512
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>

#define CERTFILE "anope.cert"
#define KEYFILE "anope.key"

static SSL_CTX *server_ctx, *client_ctx;

class MySSLService : public SSLService
{
 public:
 	MySSLService(Module *o, const Anope::string &n);

	/** Initialize a socket to use SSL
	 * @param s The socket
	 */
	void Init(Socket *s);
};

class SSLSocketIO : public SocketIO
{
 public:
	/* The SSL socket for this socket */
	SSL *sslsock;

	/** Constructor
	 */
	SSLSocketIO();

	/** Really receive something from the buffer
 	 * @param s The socket
	 * @param buf The buf to read to
	 * @param sz How much to read
	 * @return Number of bytes received
	 */
	int Recv(Socket *s, char *buf, size_t sz);

	/** Write something to the socket
	 * @param s The socket
	 * @param buf The data to write
	 * @param size The length of the data
	 */
	int Send(Socket *s, const char *buf, size_t sz);

	/** Accept a connection from a socket
	 * @param s The socket
	 * @return The new socket
	 */
	ClientSocket *Accept(ListenSocket *s);

	/** Finished accepting a connection from a socket
	 * @param s The socket
	 * @return SF_ACCEPTED if accepted, SF_ACCEPTING if still in process, SF_DEAD on error
	 */
	SocketFlag FinishAccept(ClientSocket *cs);

	/** Connect the socket
	 * @param s THe socket
	 * @param target IP to connect to
	 * @param port to connect to
	 */
	void Connect(ConnectionSocket *s, const Anope::string &target, int port);

	/** Called to potentially finish a pending connection
	 * @param s The socket
	 * @return SF_CONNECTED on success, SF_CONNECTING if still pending, and SF_DEAD on error.
	 */
	SocketFlag FinishConnect(ConnectionSocket *s);

	/** Called when the socket is destructing
	 */
	void Destroy();
};

class SSLModule;
static SSLModule *me;
class SSLModule : public Module
{
	static int AlwaysAccept(int, X509_STORE_CTX *)
	{
		return 1;
	}

 public:
	MySSLService service;

	SSLModule(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED), service(this, "ssl")
	{
		me = this;

		this->SetAuthor("Anope");
		this->SetPermanent(true);

		SSL_library_init();
		SSL_load_error_strings();

		client_ctx = SSL_CTX_new(SSLv23_client_method());
		server_ctx = SSL_CTX_new(SSLv23_server_method());

		if (!client_ctx || !server_ctx)
			throw ModuleException("Error initializing SSL CTX");

		if (IsFile(CERTFILE))
		{
			if (!SSL_CTX_use_certificate_file(client_ctx, CERTFILE, SSL_FILETYPE_PEM) || !SSL_CTX_use_certificate_file(server_ctx, CERTFILE, SSL_FILETYPE_PEM))
			{
				SSL_CTX_free(client_ctx);
				SSL_CTX_free(server_ctx);
				throw ModuleException("Error loading certificate");
			}
		}
		else
			Log() << "m_ssl: No certificate file found";

		if (IsFile(KEYFILE))
		{
			if (!SSL_CTX_use_PrivateKey_file(client_ctx, KEYFILE, SSL_FILETYPE_PEM) || !SSL_CTX_use_PrivateKey_file(server_ctx, KEYFILE, SSL_FILETYPE_PEM))
			{
				SSL_CTX_free(client_ctx);
				SSL_CTX_free(server_ctx);
				throw ModuleException("Error loading private key");
			}
		}
		else
		{
			if (IsFile(CERTFILE))
			{
				SSL_CTX_free(client_ctx);
				SSL_CTX_free(server_ctx);
				throw ModuleException("Error loading private key - file not found");
			}
			else
				Log() << "m_ssl: No private key found";
		}

		SSL_CTX_set_mode(client_ctx, SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
		SSL_CTX_set_mode(server_ctx, SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

		SSL_CTX_set_verify(client_ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, SSLModule::AlwaysAccept);
		SSL_CTX_set_verify(server_ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, SSLModule::AlwaysAccept);


		ModuleManager::Attach(I_OnPreServerConnect, this);
	}

	~SSLModule()
	{
		for (std::map<int, Socket *>::const_iterator it = SocketEngine::Sockets.begin(), it_end = SocketEngine::Sockets.end(); it != it_end;)
		{
			Socket *s = it->second;
			++it;

			if (dynamic_cast<SSLSocketIO *>(s->IO))
				delete s;
		}

		SSL_CTX_free(client_ctx);
		SSL_CTX_free(server_ctx);
	}

	void OnPreServerConnect()
	{
		ConfigReader config;

		if (config.ReadFlag("uplink", "ssl", "no", CurrentUplink))
		{
			this->service.Init(UplinkSock);
		}
	}
};

MySSLService::MySSLService(Module *o, const Anope::string &n) : SSLService(o, n)
{
}

void MySSLService::Init(Socket *s)
{
	if (s->IO != &normalSocketIO)
		throw CoreException("Socket initializing SSL twice");
	
	s->IO = new SSLSocketIO();
}

SSLSocketIO::SSLSocketIO()
{
	this->sslsock = NULL;
}

int SSLSocketIO::Recv(Socket *s, char *buf, size_t sz)
{
	int i = SSL_read(this->sslsock, buf, sz);
	TotalRead += i;
	return i;
}

int SSLSocketIO::Send(Socket *s, const char *buf, size_t sz)
{
	int i = SSL_write(this->sslsock, buf, sz);
	TotalWritten += i;
	return i;
}

ClientSocket *SSLSocketIO::Accept(ListenSocket *s)
{
	if (s->IO == &normalSocketIO)
		throw SocketException("Attempting to accept on uninitialized socket with SSL");

	sockaddrs conaddr;

	socklen_t size = sizeof(conaddr);
	int newsock = accept(s->GetFD(), &conaddr.sa, &size);

#ifndef INVALID_SOCKET
	const int INVALID_SOCKET = -1;
#endif

	if (newsock < 0 || newsock == INVALID_SOCKET)
		throw SocketException("Unable to accept connection: " + Anope::LastError());

	ClientSocket *newsocket = s->OnAccept(newsock, conaddr);
	me->service.Init(newsocket);
	SSLSocketIO *IO = debug_cast<SSLSocketIO *>(newsocket->IO);

	IO->sslsock = SSL_new(server_ctx);
	if (!IO->sslsock)
		throw SocketException("Unable to initialize SSL socket");

	SSL_set_accept_state(IO->sslsock);

	if (!SSL_set_fd(IO->sslsock, newsocket->GetFD()))
		throw SocketException("Unable to set SSL fd");

	newsocket->SetFlag(SF_ACCEPTING);
	this->FinishAccept(newsocket);
	
	return newsocket;
}

SocketFlag SSLSocketIO::FinishAccept(ClientSocket *cs)
{
	if (cs->IO == &normalSocketIO)
		throw SocketException("Attempting to finish connect uninitialized socket with SSL");
	else if (cs->HasFlag(SF_ACCEPTED))
		return SF_ACCEPTED;
	else if (!cs->HasFlag(SF_ACCEPTING))
		throw SocketException("SSLSocketIO::FinishAccept called for a socket not accepted nor accepting?");

	SSLSocketIO *IO = debug_cast<SSLSocketIO *>(cs->IO);
	
	int ret = SSL_accept(IO->sslsock);
	if (ret <= 0)
	{
		int error = SSL_get_error(IO->sslsock, ret);
		if (ret == -1 && (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE))
		{
			SocketEngine::MarkWritable(cs);
			return SF_ACCEPTING;
		}
		else
		{
			cs->OnError(ERR_error_string(ERR_get_error(), NULL));
			cs->SetFlag(SF_DEAD);
			cs->UnsetFlag(SF_ACCEPTING);
			return SF_DEAD;
		}
	}
	else
	{
		cs->SetFlag(SF_ACCEPTED);
		cs->UnsetFlag(SF_ACCEPTING);
		cs->OnAccept();
		return SF_ACCEPTED;
	}
}

void SSLSocketIO::Connect(ConnectionSocket *s, const Anope::string &target, int port)
{
	if (s->IO == &normalSocketIO)
		throw SocketException("Attempting to connect uninitialized socket with SSL");

	s->UnsetFlag(SF_CONNECTING);
	s->UnsetFlag(SF_CONNECTED);

	s->conaddr.pton(s->IsIPv6() ? AF_INET6 : AF_INET, target, port);
	int c = connect(s->GetFD(), &s->conaddr.sa, s->conaddr.size());
	if (c == -1)
	{
		if (Anope::LastErrorCode() != EINPROGRESS)
		{
			s->OnError(Anope::LastError());
			s->SetFlag(SF_DEAD);
			return;
		}
		else
		{
			SocketEngine::MarkWritable(s);
			s->SetFlag(SF_CONNECTING);
			return;
		}
	}
	else
	{
		s->SetFlag(SF_CONNECTING);
		this->FinishConnect(s);
	}
}

SocketFlag SSLSocketIO::FinishConnect(ConnectionSocket *s)
{
	if (s->IO == &normalSocketIO)
		throw SocketException("Attempting to finish connect uninitialized socket with SSL");
	else if (s->HasFlag(SF_CONNECTED))
		return SF_CONNECTED;
	else if (!s->HasFlag(SF_CONNECTING))
		throw SocketException("SSLSocketIO::FinishConnect called for a socket not connected nor connecting?");

	SSLSocketIO *IO = debug_cast<SSLSocketIO *>(s->IO);

	if (IO->sslsock == NULL)
	{
		IO->sslsock = SSL_new(client_ctx);
		if (!IO->sslsock)
			throw SocketException("Unable to initialize SSL socket");

		if (!SSL_set_fd(IO->sslsock, s->GetFD()))
			throw SocketException("Unable to set SSL fd");
	}
	
	int ret = SSL_connect(IO->sslsock);
	if (ret <= 0)
	{
		int error = SSL_get_error(IO->sslsock, ret);
		if (ret == -1 && (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE))
		{
			SocketEngine::MarkWritable(s);
			return SF_CONNECTING;
		}
		else
		{
			s->OnError(ERR_error_string(ERR_get_error(), NULL));
			s->UnsetFlag(SF_CONNECTING);
			s->SetFlag(SF_DEAD);
			return SF_DEAD;
		}
	}
	else
	{
		s->UnsetFlag(SF_CONNECTING);
		s->SetFlag(SF_CONNECTED);
		s->OnConnect();
		return SF_CONNECTED;
	}
}

void SSLSocketIO::Destroy()
{
	if (this->sslsock)
	{
		SSL_shutdown(this->sslsock);
		SSL_free(this->sslsock);
	}
}

MODULE_INIT(SSLModule)
