/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "httpd.h"

static Anope::string BuildDate()
{
	char timebuf[64];
	struct tm *tm = localtime(&Anope::CurTime);
	strftime(timebuf, sizeof(timebuf), "%a, %d %b %Y %T %Z", tm);
	return timebuf;
}

static Anope::string GetStatusFromCode(HTTPError err)
{
	switch (err)
	{
		case HTTP_ERROR_OK:
			return "200 OK";
		case HTTP_FOUND:
			return "302 Found";
		case HTTP_BAD_REQUEST:
			return "400 Bad Request";
		case HTTP_PAGE_NOT_FOUND:
			return "404 Not Found";
		case HTTP_NOT_SUPPORTED:
			return "505 HTTP Version Not Supported";
	}

	return "501 Not Implemented";
}

class MyHTTPClient : public HTTPClient, public Base
{
	HTTPProvider *provider;
	HTTPMessage header;
	bool header_done;
	Anope::string page_name;
	dynamic_reference<HTTPPage> page;
	Anope::string ip;

	enum
	{
		ACTION_NONE,
		ACTION_GET,
		ACTION_POST
	} action;

	void Serve()
	{
		if (!this->page)
		{
			this->SendError(HTTP_PAGE_NOT_FOUND, "Page not found");
			return;
		}

		if (this->ip == this->provider->ext_ip)
		{
			for (unsigned i = 0; i < this->provider->ext_headers.size(); ++i)
			{
				const Anope::string &token = this->provider->ext_headers[i];

				if (this->header.headers.count(token))
				{
					Log(LOG_DEBUG, "httpd") << "m_httpd: IP for connection " << this->GetFD() << " changed to " << this->ip;
					this->ip = this->header.headers[token];
					break;
				}
			}
		}

		Log(LOG_DEBUG, "httpd") << "m_httpd: Serving page " << this->page_name << " to " << this->ip;

		HTTPReply reply;

		this->page->OnRequest(this->provider, this->page_name, this, this->header, reply);

		this->SendReply(&reply);
	}

 public:
	time_t created;

	MyHTTPClient(HTTPProvider *l, int f, const sockaddrs &a) : Socket(f, l->IsIPv6()), HTTPClient(l, f, a), provider(l), header_done(false), ip(a.addr()), action(ACTION_NONE), created(Anope::CurTime)
	{
		Log(LOG_DEBUG, "httpd") << "Accepted connection " << f << " from " << a.addr();
	}

	~MyHTTPClient()
	{
		Log(LOG_DEBUG, "httpd") << "Closing connection " << this->GetFD() << " from " << this->ip;
	}

	const Anope::string GetIP() anope_override
	{
		return this->ip;
	}

	bool Read(const Anope::string &buf) anope_override
	{
		Log(LOG_DEBUG_2) << "HTTP from " << this->clientaddr.addr() << ": " << buf;

		if (!this->header_done)
		{
			if (this->action == ACTION_NONE)
			{
				std::vector<Anope::string> params = BuildStringVector(buf);

				if (params.empty() || (params[0] != "GET" && params[0] != "POST"))
				{
					this->SendError(HTTP_BAD_REQUEST, "Unknown operation");
					return true;
				}

				if (params.size() != 3)
				{
					this->SendError(HTTP_BAD_REQUEST, "Invalid parameters");
					return true;
				}

				if (params[0] == "GET")
					this->action = ACTION_GET;
				else if (params[0] == "POST")
					this->action = ACTION_POST;

				Anope::string targ = params[1];
				size_t q = targ.find('?');
				if (q != Anope::string::npos)
				{
					sepstream sep(targ.substr(q + 1), '&');
					targ = targ.substr(0, q);

					Anope::string token;
					while (sep.GetToken(token))
					{
						size_t sz = token.find('=');
						if (sz == Anope::string::npos || !sz || sz + 1 >= token.length())
							continue;
						this->header.get_data[token.substr(0, sz)] = HTTPUtils::URLDecode(token.substr(sz + 1));
					}
				}

				this->page = this->provider->FindPage(targ);
				this->page_name = targ;
			}
			else if (buf.find("Cookie: ") == 0)
			{
				spacesepstream sep(buf.substr(8));
				Anope::string token;

				while (sep.GetToken(token))
				{
					size_t sz = token.find('=');
					if (sz == Anope::string::npos || !sz || sz + 1 >= token.length())
						continue;
					size_t end = token.length() - (sz + 1);
					if (!sep.StreamEnd())
						--end; // Remove trailing ;
					this->header.cookies[token.substr(0, sz)] = token.substr(sz + 1, end);
				}
			}
			else if (buf.find(':') != Anope::string::npos)
			{
				size_t sz = buf.find(':');
				if (sz + 2 < buf.length())
					this->header.headers[buf.substr(0, sz)] = buf.substr(sz + 2);
			}
			else if (buf.empty())
			{
				this->header_done = true;

				if (this->action == ACTION_POST)
				{
					Log(LOG_DEBUG_2) << "HTTP POST from " << this->clientaddr.addr() << ": " << this->extrabuf;
					
					sepstream sep(this->extrabuf, '&');
					Anope::string token;

					while (sep.GetToken(token))
					{
						size_t sz = token.find('=');
						if (sz == Anope::string::npos || !sz || sz + 1 >= token.length())
							continue;
						this->header.post_data[token.substr(0, sz)] = HTTPUtils::URLDecode(token.substr(sz + 1));
					}
				}

				this->Serve();
			}
		}

		return true;
	}

	void SendError(HTTPError err, const Anope::string &msg) anope_override
	{
		HTTPReply h;

		h.error = err;

		h.Write(msg);

		this->SendReply(&h);
	}

	void SendReply(HTTPReply *message) anope_override
	{
		this->WriteClient("HTTP/1.1 " + GetStatusFromCode(message->error));
		this->WriteClient("Date: " + BuildDate());
		this->WriteClient("Server: Anope-" + Anope::VersionShort());
		if (message->content_type.empty())
			this->WriteClient("Content-Type: text/html");
		else
			this->WriteClient("Content-Type: " + message->content_type);
		this->WriteClient("Content-Length: " + stringify(message->length));

		for (unsigned i = 0; i < message->cookies.size(); ++i)
		{
			Anope::string buf = "Set-Cookie:";

			for (HTTPReply::cookie::iterator it = message->cookies[i].begin(), it_end = message->cookies[i].end(); it != it_end; ++it)
				buf += " " + it->first + "=" + it->second + ";";

			buf.erase(buf.length() - 1);

			this->WriteClient(buf);
		}

		typedef std::map<Anope::string, Anope::string> map;
		for (map::iterator it = message->headers.begin(), it_end = message->headers.end(); it != it_end; ++it)
			this->WriteClient(it->first + ": " + it->second);

		this->WriteClient("Connection: Close");
		this->WriteClient("");

		for (unsigned i = 0; i < message->out.size(); ++i)
		{
			HTTPReply::Data* d = message->out[i];

			this->Write(d->buf, d->len);

			delete d;
		}

		message->out.clear();
	}
};

class MyHTTPProvider : public HTTPProvider, public CallBack
{
	int timeout;
	std::map<Anope::string, HTTPPage *> pages;
	std::list<dynamic_reference<MyHTTPClient> > clients;

 public:
	MyHTTPProvider(Module *c, const Anope::string &n, const Anope::string &i, const unsigned short p, const int t) : HTTPProvider(c, n, i, p), CallBack(c, 10, Anope::CurTime, true), timeout(t) { }

	void Tick(time_t) anope_override
	{
		while (!this->clients.empty())
		{
			dynamic_reference<MyHTTPClient>& c = this->clients.front();
			if (c && c->created + this->timeout >= Anope::CurTime)
				break;

			delete c;
			this->clients.pop_front();
		}
	}

	ClientSocket* OnAccept(int fd, const sockaddrs &addr) anope_override
	{
		MyHTTPClient *c = new MyHTTPClient(this, fd, addr);
		this->clients.push_back(c);
		return c;
	}

	bool RegisterPage(HTTPPage *page) anope_override
	{
		return this->pages.insert(std::make_pair(page->GetURL(), page)).second;
	}

	void UnregisterPage(HTTPPage *page) anope_override
	{
		this->pages.erase(page->GetURL());
	}

	HTTPPage* FindPage(const Anope::string &pname)
	{
		if (this->pages.count(pname) == 0)
			return NULL;
		return this->pages[pname];
	}
};

class HTTPD : public Module
{
	std::map<Anope::string, HTTPProvider *> providers;
 public:
	HTTPD(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	~HTTPD()
	{
		for (std::map<int, Socket *>::const_iterator it = SocketEngine::Sockets.begin(), it_end = SocketEngine::Sockets.end(); it != it_end;)
		{
			Socket *s = it->second;
			++it;

			if (dynamic_cast<MyHTTPProvider *>(s) || dynamic_cast<MyHTTPClient *>(s))
				delete s;
		}

		this->providers.clear();
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		std::set<Anope::string> existing;

		for (int i = 0, num = config.Enumerate("httpd"); i < num; ++i)
		{
			Anope::string hname = config.ReadValue("httpd", "name", "httpd/main", i);
			existing.insert(hname);

			Anope::string ip = config.ReadValue("httpd", "ip", "", i);
			int port = config.ReadInteger("httpd", "port", "8080", i, true);
			int timeout = config.ReadInteger("httpd", "timeout", "30", i, true);
			Anope::string ext_ip = config.ReadValue("httpd", "extforward_ip", "", i);
			Anope::string ext_header = config.ReadValue("httpd", "extforward_header", "", i);

			if (ip.empty())
			{
				Log(LOG_NORMAL, "httpd") << "You must configure a bind IP for HTTP server " << hname;
				continue;
			}
			else if (port <= 0 || port > 65535)
			{
				Log(LOG_NORMAL, "httpd") << "You must configure a (valid) listen port for HTTP server " << hname;
				continue;
			}

			HTTPProvider *p;
			if (this->providers.count(hname) == 0)
			{
				try
				{
					p = new MyHTTPProvider(this, hname, ip, port, timeout);
				}
				catch (const SocketException &ex)
				{
					Log(LOG_NORMAL, "httpd") << "Unable to create HTTP server " << hname << ": " << ex.GetReason();
					continue;
				}
				this->providers[hname] = p;

				Log(LOG_NORMAL, "httpd") << "Created HTTP server " << hname;
			}
			else
			{
				p = this->providers[hname];

				if (p->GetIP() != ip || p->GetPort() != port)
				{
					delete p;
					this->providers.erase(hname);

					try
					{
						p = new MyHTTPProvider(this, hname, ip, port, timeout);
					}
					catch (const SocketException &ex)
					{
						Log(LOG_NORMAL, "httpd") << "Unable to create HTTP server " << hname << ": " << ex.GetReason();
						continue;
					}

					this->providers[hname] = p;
				}
			}


			p->ext_ip = ext_ip;
			p->ext_headers = BuildStringVector(ext_header);
		}

		for (std::map<Anope::string, HTTPProvider *>::iterator it = this->providers.begin(), it_end = this->providers.end(); it != it_end;)
		{
			HTTPProvider *p = it->second;
			++it;

			if (existing.count(p->name) == 0)
			{
				Log(LOG_NORMAL, "httpd") << "Removing HTTP server " << p->name;
				this->providers.erase(p->name);
				delete p;
			}
		}
	}
};

MODULE_INIT(HTTPD)
