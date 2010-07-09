/* RequiredLibraries: ssl,crypt */

#include "module.h"

#define OPENSSL_NO_SHA512
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>

#define CERTFILE "anope.cert"
#define KEYFILE "anope.key"

static SSL_CTX *ctx;

class SSLSocket : public ClientSocket
{
 private:
	SSL *sslsock;

	const int RecvInternal(char *buf, size_t sz) const
	{
		return SSL_read(sslsock, buf, sz);
	}

	const int SendInternal(const std::string &buf) const
	{
		return SSL_write(sslsock, buf.c_str(), buf.size());
	}
 public:
	SSLSocket(const std::string &nTargetHost, int nPort, const std::string &nBindHost = "", bool nIPv6 = false) : ClientSocket(nTargetHost, nPort, nBindHost, nIPv6)
	{
		sslsock = SSL_new(ctx);

		if (!sslsock)
			throw CoreException("Unable to initialize SSL socket");

		SSL_set_connect_state(sslsock);
		SSL_set_fd(sslsock, sock);
		SSL_connect(sslsock);

		UplinkSock = this;
	}

	~SSLSocket()
	{
		SSL_shutdown(sslsock);
		SSL_free(sslsock);

		UplinkSock = NULL;
	}

	bool Read(const std::string &buf)
	{
		process(buf);
		return true;
	}
};

class SSLModule : public Module
{
 public:
	SSLModule(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		SSL_load_error_strings();
		SSLeay_add_ssl_algorithms();

		ctx = SSL_CTX_new(SSLv23_client_method());

		if (!ctx)
			throw ModuleException("Error initializing SSL CTX");

		if (IsFile(CERTFILE))
		{
			if (!SSL_CTX_use_certificate_file(ctx, CERTFILE, SSL_FILETYPE_PEM))
			{
				SSL_CTX_free(ctx);
				throw ModuleException("Error loading certificate");
			}
		}
		else
			Alog() << "m_ssl: No certificate file found";

		if (IsFile(KEYFILE))
		{
			if (!SSL_CTX_use_PrivateKey_file(ctx, KEYFILE, SSL_FILETYPE_PEM))
			{
				SSL_CTX_free(ctx);
				throw ModuleException("Error loading private key");
			}
		}
		else
		{
			if (IsFile(CERTFILE))
			{
				SSL_CTX_free(ctx);
				throw ModuleException("Error loading private key - file not found");
			}
			else
				Alog() << "m_ssl: No private key found";
		}

		this->SetAuthor("Anope");
		this->SetType(SUPPORTED);
		this->SetPermanent(true);

		SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
		SSL_CTX_set_options(ctx, SSL_OP_TLS_ROLLBACK_BUG | SSL_OP_ALL);

		ModuleManager::Attach(I_OnPreServerConnect, this);
	}

	~SSLModule()
	{
		SSL_CTX_free(ctx);
	}

	EventReturn OnPreServerConnect(Uplink *u, int Number)
	{
		ConfigReader config;

		if (config.ReadFlag("uplink", "ssl", "no", Number - 1))
		{
			try
			{
				new SSLSocket(u->host, u->port, Config.LocalHost ? Config.LocalHost : "", u->ipv6);
				Alog() << "Connected to Server " << Number << " (" << u->host << ":" << u->port << ")";
			}
			catch (SocketException& ex)
			{
				Alog() << "Unable to connect with SSL to server" << Number << " (" << u->host << ":" << u->port << "), " << ex.GetReason();
			}

			return EVENT_ALLOW;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(SSLModule)
