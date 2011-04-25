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
	int Recv(Socket *s, char *buf, size_t sz) const;

	/** Really write something to the socket
 	 * @param s The socket
	 * @param buf What to write
	 * @return Number of bytes written
	 */
	int Send(Socket *s, const Anope::string &buf) const;

	/** Accept a connection from a socket
	 * @param s The socket
	 */
	void Accept(ListenSocket *s);

	/** Connect the socket
	 * @param s THe socket
	 * @param target IP to connect to
	 * @param port to connect to
	 * @param bindip IP to bind to, if any
	 */
	void Connect(ConnectionSocket *s, const Anope::string &target, int port, const Anope::string &bindip = "");

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

	SSLModule(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), service(this, "ssl")
	{
		me = this;

		this->SetAuthor("Anope");
		this->SetType(SUPPORTED);
		this->SetPermanent(true);

		SSL_library_init();
		SSL_load_error_strings();
		SSLeay_add_ssl_algorithms();

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

		ModuleManager::RegisterService(&this->service);

		ModuleManager::Attach(I_OnPreServerConnect, this);
	}

	~SSLModule()
	{
		SSL_CTX_free(client_ctx);
		SSL_CTX_free(server_ctx);
	}

	EventReturn OnPreServerConnect(Uplink *u, int Number)
	{
		ConfigReader config;

		if (config.ReadFlag("uplink", "ssl", "no", Number - 1))
		{
			DNSRecord req = DNSManager::BlockingQuery(uplink_server->host, uplink_server->ipv6 ? DNS_QUERY_AAAA : DNS_QUERY_A);

			if (!req)
				Log() << "Unable to connect to server " << uplink_server->host << ":" << uplink_server->port << " using SSL: Invalid hostname/IP";
			else
			{
				try
				{
					new UplinkSocket(uplink_server->ipv6);
					this->service.Init(UplinkSock);
					UplinkSock->Connect(req.result, uplink_server->port, Config->LocalHost);

					Log() << "Connected to server " << Number << " (" << u->host << ":" << u->port << ") with SSL";
					return EVENT_ALLOW;
				}
				catch (const SocketException &ex)
				{
					Log() << "Unable to connect with SSL to server " << Number << " (" << u->host << ":" << u->port << "), " << ex.GetReason();
				}
			}

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
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

int SSLSocketIO::Recv(Socket *s, char *buf, size_t sz) const
{
	size_t i = SSL_read(this->sslsock, buf, sz);
	TotalRead += i;
	return i;
}

int SSLSocketIO::Send(Socket *s, const Anope::string &buf) const
{
	size_t i = SSL_write(this->sslsock, buf.c_str(), buf.length());
	TotalWritten += i;
	return i;
}

void SSLSocketIO::Accept(ListenSocket *s)
{
	sockaddrs conaddr;

	socklen_t size = conaddr.size();
	int newsock = accept(s->GetFD(), &conaddr.sa, &size);

#ifndef INVALID_SOCKET
# define INVALID_SOCKET -1
#endif
	if (newsock <= 0 || newsock == INVALID_SOCKET)
		throw SocketException("Unable to accept SSL socket: " + Anope::LastError());

	ClientSocket *newsocket = s->OnAccept(newsock, conaddr);
	me->service.Init(newsocket);
	SSLSocketIO *IO = debug_cast<SSLSocketIO *>(newsocket->IO);

	IO->sslsock = SSL_new(server_ctx);
	if (!IO->sslsock)
		throw SocketException("Unable to initialize SSL socket");

	SSL_set_accept_state(IO->sslsock);

	if (!SSL_set_fd(IO->sslsock, newsock))
		throw SocketException("Unable to set SSL fd");

	int ret = SSL_accept(IO->sslsock);
	if (ret <= 0)
	{
		int error = SSL_get_error(IO->sslsock, ret);

		if (ret != -1 || (error != SSL_ERROR_WANT_READ && error != SSL_ERROR_WANT_READ))
			throw SocketException("Unable to accept new SSL connection: " + Anope::string(ERR_error_string(ERR_get_error(), NULL)));
	}
}

void SSLSocketIO::Connect(ConnectionSocket *s, const Anope::string &TargetHost, int Port, const Anope::string &BindHost)
{
	if (s->IO == &normalSocketIO)
		throw SocketException("Attempting to connect uninitialized socket with SQL");

	normalSocketIO.Connect(s, TargetHost, Port, BindHost);

	SSLSocketIO *IO = debug_cast<SSLSocketIO *>(s->IO);

	IO->sslsock = SSL_new(client_ctx);
	if (!IO->sslsock)
		throw SocketException("Unable to initialize SSL socket");

	if (!SSL_set_fd(IO->sslsock, s->GetFD()))
		throw SocketException("Unable to set SSL fd");

	int ret = SSL_connect(IO->sslsock);
		
	if (ret <= 0)
	{
		int error = SSL_get_error(IO->sslsock, ret);

		if (ret != -1 || (error != SSL_ERROR_WANT_READ && error != SSL_ERROR_WANT_READ))
			throw SocketException("Unable to connect to server: " + Anope::string(ERR_error_string(ERR_get_error(), NULL)));
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
