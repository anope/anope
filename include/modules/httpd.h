/*
 *
 * (C) 2012-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

namespace HTTP
{
	struct Reply;
	struct Message;
	class Page;
	class Client;
	class Provider;

	enum Error
	{
		OK = 200,
		FOUND = 302,
		BAD_REQUEST = 400,
		PAGE_NOT_FOUND = 404,
		NOT_SUPPORTED = 505,
	};
}

/* A message to someone */
struct HTTP::Reply final
{
	HTTP::Error error = HTTP::OK;
	Anope::string content_type;
	std::map<Anope::string, Anope::string, ci::less> headers;
	typedef std::list<std::pair<Anope::string, Anope::string> > cookie;
	std::vector<cookie> cookies;

	Reply() = default;
	Reply &operator=(const HTTP::Reply &) = default;

	Reply(const HTTP::Reply &other)
		: error(other.error)
		, length(other.length)
	{
		content_type = other.content_type;
		headers = other.headers;
		cookies = other.cookies;

		for (const auto &datum : other.out)
			out.push_back(new Data(datum->buf, datum->len));
	}

	~Reply()
	{
		for (const auto *datum : out)
			delete datum;
		out.clear();
	}

	struct Data final
	{
		char *buf;
		size_t len;

		Data(const char *b, size_t l)
		{
			this->buf = new char[l];
			memcpy(this->buf, b, l);
			this->len = l;
		}

		~Data()
		{
			delete [] buf;
		}
	};

	std::deque<Data *> out;
	size_t length = 0;

	void Write(const Anope::string &message)
	{
		this->out.push_back(new Data(message.c_str(), message.length()));
		this->length += message.length();
	}

	void Write(const char *b, size_t l)
	{
		this->out.push_back(new Data(b, l));
		this->length += l;
	}
};

/* A message from someone */
struct HTTP::Message final
{
	std::map<Anope::string, Anope::string, ci::less> headers;
	std::map<Anope::string, Anope::string, ci::less> cookies;
	std::map<Anope::string, Anope::string, ci::less> get_data;
	std::map<Anope::string, Anope::string, ci::less> post_data;
	Anope::string content;
};

class HTTP::Page
	: public virtual Base
{
	Anope::string url;
	Anope::string content_type;

public:
	Page(const Anope::string &u, const Anope::string &ct = "text/html")
		: url(u)
		, content_type(ct)
	{
	}

	const Anope::string &GetURL() const { return this->url; }

	const Anope::string &GetContentType() const { return this->content_type; }

	/** Called when this page is requested
	 * @param The server this page is on
	 * @param The page name
	 * @param The client requesting the page
	 * @param The HTTP header sent from the client to request the page
	 * @param The HTTP header that will be sent back to the client
	 */
	virtual bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &) = 0;
};

class HTTP::Client
	: public ClientSocket
	, public BinarySocket
	, public Base
{
protected:
	void WriteClient(const Anope::string &message)
	{
		BinarySocket::Write(message + "\r\n");
	}

public:
	Client(ListenSocket *l, int f, const sockaddrs &a)
		: ClientSocket(l, a)
		, BinarySocket()
	{
	}

	virtual const Anope::string GetIP()
	{
		return this->clientaddr.addr();
	}

	virtual void SendError(HTTP::Error err, const Anope::string &msg) = 0;
	virtual void SendReply(HTTP::Reply *) = 0;
};

#define HTTP_PROVIDER "HTTP::Provider"

class HTTP::Provider
	: public ListenSocket
	, public Service
{
	Anope::string ip;
	unsigned short port;
	bool ssl;
public:
	std::vector<Anope::string> ext_ips;
	std::vector<Anope::string> ext_headers;

	Provider(Module *c, const Anope::string &n, const Anope::string &i, const unsigned short p, bool s)
		: ListenSocket(i, p, i.find(':') != Anope::string::npos)
		, Service(c, HTTP_PROVIDER, n)
		, ip(i)
		, port(p)
		, ssl(s)
	{
	}

	const Anope::string &GetIP() const
	{
		return this->ip;
	}

	unsigned short GetPort() const
	{
		return this->port;
	}

	bool IsSSL() const
	{
		return this->ssl;
	}

	virtual bool RegisterPage(HTTP::Page *page) = 0;
	virtual void UnregisterPage(HTTP::Page *page) = 0;
	virtual HTTP::Page *FindPage(const Anope::string &name) = 0;
};

namespace HTTP
{
	inline Anope::string URLDecode(const Anope::string &url)
	{
		Anope::string decoded;

		for (unsigned i = 0; i < url.length(); ++i)
		{
			const char &c = url[i];

			if (c == '%' && i + 2 < url.length())
			{
				Anope::string dest;
				Anope::Unhex(url.substr(i + 1, 2), dest);
				decoded += dest;
				i += 2;
			}
			else if (c == '+')
				decoded += ' ';
			else
				decoded += c;
		}

		return decoded;
	}

	inline Anope::string URLEncode(const Anope::string &url)
	{
		Anope::string encoded;

		for (const auto c : url)
		{
			if (isalnum(c) || c == '.' || c == '-' || c == '*' || c == '_')
				encoded += c;
			else if (c == ' ')
				encoded += '+';
			else
				encoded += "%" + Anope::Hex(c);
		}

		return encoded;
	}

	inline Anope::string Escape(const Anope::string &src)
	{
		Anope::string dst;

		for (const auto c : src)
		{
			switch (c)
			{
				case '<':
					dst += "&lt;";
					break;
				case '>':
					dst += "&gt;";
					break;
				case '"':
					dst += "&quot;";
					break;
				case '&':
					dst += "&amp;";
					break;
				default:
					dst += c;
			}
		}

		return dst;
	}
}
