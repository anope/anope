/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

struct ProxyCheck
{
	std::set<Anope::string, ci::less> types;
	std::vector<unsigned short> ports;
	time_t duration;
	Anope::string reason;
};

static Anope::string ProxyCheckString;
static Anope::string target_ip;
static unsigned short target_port;
static bool add_to_akill;

class ProxyCallbackListener : public ListenSocket
{
	class ProxyCallbackClient : public ClientSocket, public BufferedSocket
	{
	 public:
		ProxyCallbackClient(ListenSocket *l, int f, const sockaddrs &a) : Socket(f, l->IsIPv6()), ClientSocket(l, a), BufferedSocket()
		{
		}

		void OnAccept() anope_override
		{
			this->Write(ProxyCheckString);
		}

		bool ProcessWrite() anope_override
		{
			return !BufferedSocket::ProcessWrite() || this->write_buffer.empty() ? false : true;
		}
	};

 public:
	ProxyCallbackListener(const Anope::string &b, int p) : Socket(-1, b.find(':') != Anope::string::npos), ListenSocket(b, p, false)
	{
	}

	ClientSocket *OnAccept(int fd, const sockaddrs &addr) anope_override
	{
		return new ProxyCallbackClient(this, fd, addr);
	}
};

class ProxyConnect : public ConnectionSocket
{
	static ServiceReference<XLineManager> akills;

 public:
 	static std::set<ProxyConnect *> proxies;

 	ProxyCheck proxy;
	unsigned short port;
 	time_t created;

	ProxyConnect(ProxyCheck &p, unsigned short po) : Socket(-1), ConnectionSocket(), proxy(p),
		port(po), created(Anope::CurTime)
	{
		proxies.insert(this);
	}

	~ProxyConnect()
	{
		proxies.erase(this);
	}

	virtual void OnConnect() anope_override = 0;
	virtual const Anope::string GetType() const = 0;

 protected:
	void Ban()
	{
		Anope::string reason = this->proxy.reason;

		reason = reason.replace_all_cs("%t", this->GetType());
		reason = reason.replace_all_cs("%i", this->conaddr.addr());
		reason = reason.replace_all_cs("%p", stringify(this->conaddr.port()));

		Log(OperServ) << "PROXYSCAN: Open " << this->GetType() << " proxy found on " << this->conaddr.addr() << ":" << this->conaddr.port() << " (" << reason << ")";
		XLine *x = new XLine("*@" + this->conaddr.addr(), Config->OperServ, Anope::CurTime + this->proxy.duration, reason, XLineManager::GenerateUID());
		if (add_to_akill && akills)
		{
			akills->AddXLine(x);
			akills->Send(NULL, x);
		}
		else
		{
			if (IRCD->CanSZLine)
				IRCD->SendSZLine(NULL, x);
			else
				IRCD->SendAkill(NULL, x);
			delete x;
		}
	}
};
ServiceReference<XLineManager> ProxyConnect::akills("XLineManager", "xlinemanager/sgline");
std::set<ProxyConnect *> ProxyConnect::proxies;

class HTTPProxyConnect : public ProxyConnect, public BufferedSocket
{
 public:
	HTTPProxyConnect(ProxyCheck &p, unsigned short po) : Socket(-1), ProxyConnect(p, po), BufferedSocket()
	{
	}

	void OnConnect() anope_override
	{
		this->Write("CONNECT %s:%d HTTP/1.0", target_ip.c_str(), target_port);
		this->Write("Content-length: 0");
		this->Write("Connection: close");
		this->Write("");
	}

	const Anope::string GetType() const anope_override
	{
		return "HTTP";
	}

	bool ProcessRead() anope_override
	{
		BufferedSocket::ProcessRead();
		if (this->GetLine() == ProxyCheckString)
		{
			this->Ban();
			return false;
		}
		return true;
	}
};

class SOCKS5ProxyConnect : public ProxyConnect, public BinarySocket
{
 public:
	SOCKS5ProxyConnect(ProxyCheck &p, unsigned short po) : Socket(-1), ProxyConnect(p, po), BinarySocket()
	{
	}

	void OnConnect() anope_override
	{
		sockaddrs target_addr;
		char buf[4 + sizeof(target_addr.sa4.sin_addr.s_addr) + sizeof(target_addr.sa4.sin_port)];
		int ptr = 0;
		try
		{
			target_addr.pton(AF_INET, target_ip, target_port);
		}
		catch (const SocketException &)
		{
			return;
		}

		buf[ptr++] = 5; // Version
		buf[ptr++] = 1; // # of methods
		buf[ptr++] = 0; // No authentication

		this->Write(buf, ptr);

		ptr = 1;
		buf[ptr++] = 1; // Connect
		buf[ptr++] = 0; // Reserved
		buf[ptr++] = 1; // IPv4
		memcpy(buf + ptr, &target_addr.sa4.sin_addr.s_addr, sizeof(target_addr.sa4.sin_addr.s_addr));
		ptr += sizeof(target_addr.sa4.sin_addr.s_addr);
		memcpy(buf + ptr, &target_addr.sa4.sin_port, sizeof(target_addr.sa4.sin_port));
		ptr += sizeof(target_addr.sa4.sin_port);

		this->Write(buf, ptr);
	}

	const Anope::string GetType() const anope_override
	{
		return "SOCKS5";
	}

	bool Read(const char *buffer, size_t l) anope_override
	{
		if (l >= ProxyCheckString.length() && !strncmp(buffer, ProxyCheckString.c_str(), ProxyCheckString.length()))
		{
			this->Ban();
			return false;
		}
		return true;
	}
};

class ModuleProxyScan : public Module
{
	Anope::string listen_ip;
	unsigned short listen_port;
	Anope::string con_notice, con_source;
	std::vector<ProxyCheck> proxyscans;

	ProxyCallbackListener *listener;

	class ConnectionTimeout : public Timer
	{
	 public:
		ConnectionTimeout(Module *c, long timeout) : Timer(c, timeout, Anope::CurTime, true)
		{
		}

		void Tick(time_t) anope_override
		{
			for (std::set<ProxyConnect *>::iterator it = ProxyConnect::proxies.begin(), it_end = ProxyConnect::proxies.end(); it != it_end; ++it)
			{
				ProxyConnect *p = *it;

				if (p->created + this->GetSecs() < Anope::CurTime)
					p->flags[SF_DEAD] = true;
			}
		}
	} connectionTimeout;

 public:
	ModuleProxyScan(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED),
		connectionTimeout(this, 5)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnUserConnect };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->listener = NULL;

		try
		{
			OnReload();
		}
		catch (const ConfigException &ex)
		{
			throw ModuleException(ex.GetReason());
		}
	}

	~ModuleProxyScan()
	{
		for (std::set<ProxyConnect *>::iterator it = ProxyConnect::proxies.begin(), it_end = ProxyConnect::proxies.end(); it != it_end;)
		{
			ProxyConnect *p = *it;
			++it;
			delete p;
		}

		for (std::map<int, Socket *>::const_iterator it = SocketEngine::Sockets.begin(), it_end = SocketEngine::Sockets.end(); it != it_end;)
		{
			Socket *s = it->second;
			++it;

			ClientSocket *cs = dynamic_cast<ClientSocket *>(s);
			if (cs != NULL && cs->ls == this->listener)
				delete s;
		}

		delete this->listener;
	}

	void OnReload() anope_override
	{
		ConfigReader config;

		Anope::string s_target_ip = config.ReadValue("m_proxyscan", "target_ip", "", 0);
		if (s_target_ip.empty())
			throw ConfigException("m_proxyscan:target_ip may not be empty");

		int s_target_port = config.ReadInteger("m_proxyscan", "target_port", "-1", 0, true);
		if (s_target_port <= 0)
			throw ConfigException("m_proxyscan:target_port may not be empty and must be a positive number");

		Anope::string s_listen_ip = config.ReadValue("m_proxyscan", "listen_ip", "", 0);
		if (s_listen_ip.empty())
			throw ConfigException("m_proxyscan:listen_ip may not be empty");

		int s_listen_port = config.ReadInteger("m_proxyscan", "listen_port", "-1", 0, true);
		if (s_listen_port <= 0)
			throw ConfigException("m_proxyscan:listen_port may not be empty and must be a positive number");

		target_ip = s_target_ip;
		target_port = s_target_port;
		this->listen_ip = s_listen_ip;
		this->listen_port = s_listen_port;
		this->con_notice = config.ReadValue("m_proxyscan", "connect_notice", "", 0);
		this->con_source = config.ReadValue("m_proxyscan", "connect_source", "", 0);
		add_to_akill = config.ReadFlag("m_proxyscan", "add_to_akill", "true", 0);
		this->connectionTimeout.SetSecs(config.ReadInteger("m_proxyscan", "timeout", "5", 0, true));

		ProxyCheckString = Config->NetworkName + " proxy check";
		delete this->listener;
		this->listener = NULL;
		try
		{
			this->listener = new ProxyCallbackListener(this->listen_ip, this->listen_port);
		}
		catch (const SocketException &ex)
		{
			throw ConfigException("m_proxyscan: " + ex.GetReason());
		}

		this->proxyscans.clear();
		for (int i = 0; i < config.Enumerate("proxyscan"); ++i)
		{
			ProxyCheck p;
			Anope::string token;

			commasepstream sep(config.ReadValue("proxyscan", "type", "", i));
			while (sep.GetToken(token))
			{
				if (!token.equals_ci("HTTP") && !token.equals_ci("SOCKS5"))
					continue;
				p.types.insert(token);
			}
			if (p.types.empty())
				continue;

			commasepstream sep2(config.ReadValue("proxyscan", "port", "", i));
			while (sep2.GetToken(token))
			{
				try
				{
					unsigned short port = convertTo<unsigned short>(token);
					p.ports.push_back(port);
				}
				catch (const ConvertException &) { }
			}
			if (p.ports.empty())
				continue;

			p.duration = Anope::DoTime(config.ReadValue("proxyscan", "time", "4h", i));
			p.reason = config.ReadValue("proxyscan", "reason", "", i);
			if (p.reason.empty())
				continue;

			this->proxyscans.push_back(p);
		}
	}

	void OnUserConnect(User *user, bool &exempt) anope_override
	{
		if (exempt || user->Quitting() || !Me->IsSynced() || !user->server->IsSynced())
			return;

		/* At this time we only support IPv4 */
		sockaddrs user_ip;
		try
		{
			user_ip.pton(AF_INET, user->ip);
		}
		catch (const SocketException &)
		{
			/* User doesn't have a valid IPv4 IP (ipv6/spoof/etc) */
			return;
		}

		if (!this->con_notice.empty() && !this->con_source.empty())
		{
			const BotInfo *bi = BotInfo::Find(this->con_source);
			if (bi)
				user->SendMessage(bi, this->con_notice);
		}

		for (unsigned i = this->proxyscans.size(); i > 0; --i)
		{
			ProxyCheck &p = this->proxyscans[i - 1];

			for (std::set<Anope::string, ci::less>::iterator it = p.types.begin(), it_end = p.types.end(); it != it_end; ++it)
			{
				for (unsigned k = 0; k < p.ports.size(); ++k)
				{
					try
					{
						ProxyConnect *con = NULL;
						if (it->equals_ci("HTTP"))
							con = new HTTPProxyConnect(p, p.ports[k]);
						else if (it->equals_ci("SOCKS5"))
							con = new SOCKS5ProxyConnect(p, p.ports[k]);
						else
							continue;
						con->Connect(user->ip, p.ports[k]);
					}
					catch (const SocketException &ex)
					{
						Log(LOG_DEBUG) << "m_proxyscan: " << ex.GetReason();
					}
				}
			}
		}
	}
};

MODULE_INIT(ModuleProxyScan)

