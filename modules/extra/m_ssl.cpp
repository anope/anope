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
	/* -1 if not, 0 if waiting, 1 if true */
	int connected, accepted;

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

	/** Really write something to the socket
 	 * @param s The socket
	 * @param buf What to write
	 * @return Number of bytes written
	 */
	int Send(Socket *s, const Anope::string &buf);

	/** Accept a connection from a socket
	 * @param s The socket
	 * @return The new socket
	 */
	ClientSocket *Accept(ListenSocket *s);

	/** Check if a connection has been accepted
	 * @param s The client socket
	 * @return -1 on error, 0 to wait, 1 on success
	 */
	int Accepted(ClientSocket *cs);

	/** Connect the socket
	 * @param s THe socket
	 * @param target IP to connect to
	 * @param port to connect to
	 */
	void Connect(ConnectionSocket *s, const Anope::string &target, int port);

	/** Check if this socket is connected
	 * @param s The socket
 	 * @return -1 for error, 0 for wait, 1 for connected
	 */
	int Connected(ConnectionSocket *s);

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

SSLSocketIO::SSLSocketIO() : connected(-1), accepted(-1)
{
	this->sslsock = NULL;
}

int SSLSocketIO::Recv(Socket *s, char *buf, size_t sz)
{
	int i = SSL_read(this->sslsock, buf, sz);
	TotalRead += i;
	return i;
}

int SSLSocketIO::Send(Socket *s, const Anope::string &buf)
{
	int i = SSL_write(this->sslsock, buf.c_str(), buf.length());
	TotalWritten += i;
	return i;
}

ClientSocket *SSLSocketIO::Accept(ListenSocket *s)
{
	if (s->IO == &normalSocketIO)
		throw SocketException("Attempting to accept on uninitialized socket with SSL");
	
	ClientSocket *newsocket = normalSocketIO.Accept(s);
	me->service.Init(newsocket);
	SSLSocketIO *IO = debug_cast<SSLSocketIO *>(newsocket->IO);

	IO->sslsock = SSL_new(server_ctx);
	if (!IO->sslsock)
		throw SocketException("Unable to initialize SSL socket");

	SSL_set_accept_state(IO->sslsock);

	if (!SSL_set_fd(IO->sslsock, newsocket->GetFD()))
		throw SocketException("Unable to set SSL fd");

	int ret = SSL_accept(IO->sslsock);
	if (ret <= 0)
	{
		IO->accepted = 0;
		int error = SSL_get_error(IO->sslsock, ret);
		if (ret == -1 && (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE))
		{
			SocketEngine::MarkWritable(newsocket);
			return newsocket;
		}
		
		throw SocketException("Unable to accept new SSL connection: " + Anope::string(ERR_error_string(ERR_get_error(), NULL)));
	}
	
	IO->accepted = 1;
	return newsocket;
}

int SSLSocketIO::Accepted(ClientSocket *cs)
{
	SSLSocketIO *IO = debug_cast<SSLSocketIO *>(cs->IO);

	if (IO->accepted == 0)
	{
		int ret = SSL_accept(IO->sslsock);
		if (ret <= 0)
		{
			int error = SSL_get_error(IO->sslsock, ret);
			if (ret == -1 && (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE))
			{
				SocketEngine::MarkWritable(cs);
				return 0;
			}

			return -1;
		}
		IO->accepted = 1;
		return 0;
	}

	return IO->accepted;
}

void SSLSocketIO::Connect(ConnectionSocket *s, const Anope::string &target, int port)
{
	if (s->IO == &normalSocketIO)
		throw SocketException("Attempting to connect uninitialized socket with SSL");

	normalSocketIO.Connect(s, target, port);

	SSLSocketIO *IO = debug_cast<SSLSocketIO *>(s->IO);

	IO->sslsock = SSL_new(client_ctx);
	if (!IO->sslsock)
		throw SocketException("Unable to initialize SSL socket");

	if (!SSL_set_fd(IO->sslsock, s->GetFD()))
		throw SocketException("Unable to set SSL fd");

	int ret = SSL_connect(IO->sslsock);
	if (ret <= 0)
	{
		IO->connected = 0;
		int error = SSL_get_error(IO->sslsock, ret);
		if (ret == -1 && (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE))
		{
			SocketEngine::MarkWritable(s);
			return;
		}

		s->ProcessError();
	}

	IO->connected = 1;
}

int SSLSocketIO::Connected(ConnectionSocket *s)
{
	if (s->IO == &normalSocketIO)
		throw SocketException("Connected() called for non ssl socket?");
	
	int i = SocketIO::Connected(s);
	if (i != 1)
		return i;

	SSLSocketIO *IO = debug_cast<SSLSocketIO *>(s->IO);

	if (IO->connected == 0)
	{
		int ret = SSL_connect(IO->sslsock);
		if (ret <= 0)
		{
			int error = SSL_get_error(IO->sslsock, ret);
			if (ret == -1 && (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE))
				return 0;

			s->ProcessError();
			return -1;
		}
		IO->connected = 1;
		return 0; // poll for next read/write (which will be real), don't assume ones available
	}
	
	return IO->connected;
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
