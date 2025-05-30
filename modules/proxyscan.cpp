/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

struct ProxyCheck final
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

class ProxyCallbackListener final
	: public ListenSocket
{
	class ProxyCallbackClient final
		: public ClientSocket
		, public BufferedSocket
	{
	public:
		ProxyCallbackClient(ListenSocket *l, int f, const sockaddrs &a) : Socket(f, l->GetFamily()), ClientSocket(l, a), BufferedSocket()
		{
		}

		void OnAccept() override
		{
			this->Write(ProxyCheckString);
		}

		bool ProcessWrite() override
		{
			return !(!BufferedSocket::ProcessWrite() || this->write_buffer.empty());
		}
	};

public:
	ProxyCallbackListener(const Anope::string &b, int p) : Socket(-1, b.find(':') == Anope::string::npos ? AF_INET : AF_INET6), ListenSocket(b, p, false)
	{
	}

	ClientSocket *OnAccept(int fd, const sockaddrs &addr) override
	{
		return new ProxyCallbackClient(this, fd, addr);
	}
};

class ProxyConnect
	: public ConnectionSocket
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

	~ProxyConnect() override
	{
		proxies.erase(this);
	}

	void OnConnect() override = 0;
	virtual const Anope::string GetType() const = 0;

protected:
	void Ban()
	{
		auto reason = Anope::Template(this->proxy.reason, {
			{ "ip",    this->conaddr.addr()                  },
			{ "port",  Anope::ToString(this->conaddr.port()) },
			{ "type",  this->GetType()                       },
		});

		BotInfo *OperServ = Config->GetClient("OperServ");
		Log(OperServ) << "PROXYSCAN: Open " << this->GetType() << " proxy found on " << this->conaddr.str() << " (" << reason << ")";
		auto *x = new XLine("*@" + this->conaddr.addr(), OperServ ? OperServ->nick : "", Anope::CurTime + this->proxy.duration, reason, XLineManager::GenerateUID());
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

class HTTPProxyConnect final
	: public ProxyConnect
	, public BufferedSocket
{
public:
	HTTPProxyConnect(ProxyCheck &p, unsigned short po) : Socket(-1), ProxyConnect(p, po), BufferedSocket()
	{
	}

	void OnConnect() override
	{
		this->Write("CONNECT %s:%d HTTP/1.0", target_ip.c_str(), target_port);
		this->Write("Content-Length: 0");
		this->Write("Connection: close");
		this->Write("");
	}

	const Anope::string GetType() const override
	{
		return "HTTP";
	}

	bool ProcessRead() override
	{
		bool b = BufferedSocket::ProcessRead();
		if (this->GetLine() == ProxyCheckString)
		{
			this->Ban();
			return false;
		}
		return b;
	}
};

class SOCKS5ProxyConnect final
	: public ProxyConnect
	, public BinarySocket
{
public:
	SOCKS5ProxyConnect(ProxyCheck &p, unsigned short po) : Socket(-1), ProxyConnect(p, po), BinarySocket()
	{
	}

	void OnConnect() override
	{
		sockaddrs target_addr;
		char buf[4 + sizeof(target_addr.sa4.sin_addr.s_addr) + sizeof(target_addr.sa4.sin_port)];
		int ptr = 0;
		target_addr.pton(AF_INET, target_ip, target_port);
		if (!target_addr.valid())
			return;

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

	const Anope::string GetType() const override
	{
		return "SOCKS5";
	}

	bool Read(const char *buffer, size_t l) override
	{
		if (l >= ProxyCheckString.length() && !strncmp(buffer, ProxyCheckString.c_str(), ProxyCheckString.length()))
		{
			this->Ban();
			return false;
		}
		return true;
	}
};

class ModuleProxyScan final
	: public Module
{
	Anope::string listen_ip;
	unsigned short listen_port;
	Anope::string con_notice, con_source;
	std::vector<ProxyCheck> proxyscans;

	ProxyCallbackListener *listener;

	class ConnectionTimeout final
		: public Timer
	{
	public:
		ConnectionTimeout(Module *c, time_t timeout)
			: Timer(c, timeout, true)
		{
		}

		void Tick() override
		{
			for (std::set<ProxyConnect *>::iterator it = ProxyConnect::proxies.begin(), it_end = ProxyConnect::proxies.end(); it != it_end;)
			{
				ProxyConnect *p = *it;
				++it;

				if (p->created + this->GetSecs() < Anope::CurTime)
					delete p;
			}
		}
	} connectionTimeout;

public:
	ModuleProxyScan(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR),
		connectionTimeout(this, 5)
	{


		this->listener = NULL;
	}

	~ModuleProxyScan() override
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

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &config = Config->GetModule(this);

		Anope::string s_target_ip = config.Get<const Anope::string>("target_ip");
		if (s_target_ip.empty())
			throw ConfigException(this->name + " target_ip may not be empty");

		int s_target_port = config.Get<int>("target_port", "-1");
		if (s_target_port <= 0)
			throw ConfigException(this->name + " target_port may not be empty and must be a positive number");

		Anope::string s_listen_ip = config.Get<const Anope::string>("listen_ip");
		if (s_listen_ip.empty())
			throw ConfigException(this->name + " listen_ip may not be empty");

		int s_listen_port = config.Get<int>("listen_port", "-1");
		if (s_listen_port <= 0)
			throw ConfigException(this->name + " listen_port may not be empty and must be a positive number");

		target_ip = s_target_ip;
		target_port = s_target_port;
		this->listen_ip = s_listen_ip;
		this->listen_port = s_listen_port;
		this->con_notice = config.Get<const Anope::string>("connect_notice");
		this->con_source = config.Get<const Anope::string>("connect_source");
		add_to_akill = config.Get<bool>("add_to_akill", "true");
		this->connectionTimeout.SetSecs(config.Get<time_t>("timeout", "5s"));

		ProxyCheckString = Config->GetBlock("networkinfo").Get<const Anope::string>("networkname") + " proxy check";
		delete this->listener;
		this->listener = NULL;
		try
		{
			this->listener = new ProxyCallbackListener(this->listen_ip, this->listen_port);
		}
		catch (const SocketException &ex)
		{
			throw ConfigException("proxyscan: " + ex.GetReason());
		}

		this->proxyscans.clear();
		for (int i = 0; i < config.CountBlock("proxyscan"); ++i)
		{
			const auto &block = config.GetBlock("proxyscan", i);
			ProxyCheck p;
			Anope::string token;

			commasepstream sep(block.Get<const Anope::string>("type"));
			while (sep.GetToken(token))
			{
				if (!token.equals_ci("HTTP") && !token.equals_ci("SOCKS5"))
					continue;
				p.types.insert(token);
			}
			if (p.types.empty())
				continue;

			commasepstream sep2(block.Get<const Anope::string>("port"));
			while (sep2.GetToken(token))
			{
				if (auto port = Anope::TryConvert<unsigned short>(token))
					p.ports.push_back(port.value());
			}
			if (p.ports.empty())
				continue;

			p.duration = block.Get<time_t>("time", "4h");
			p.reason = block.Get<const Anope::string>("reason");
			if (p.reason.empty())
				continue;

			this->proxyscans.push_back(p);
		}
	}

	void OnUserConnect(User *user, bool &exempt) override
	{
		if (exempt || user->Quitting() || !Me->IsSynced() || !user->server->IsSynced())
			return;

		/* At this time we only support IPv4 */
		if (!user->ip.valid() || user->ip.sa.sa_family != AF_INET)
			/* User doesn't have a valid IPv4 IP (ipv6/spoof/etc) */
			return;

		if (!this->con_notice.empty() && !this->con_source.empty())
		{
			BotInfo *bi = BotInfo::Find(this->con_source, true);
			if (bi)
				user->SendMessage(bi, this->con_notice);
		}

		for (unsigned i = this->proxyscans.size(); i > 0; --i)
		{
			ProxyCheck &p = this->proxyscans[i - 1];

			for (const auto &type : p.types)
			{
				for (const auto port : p.ports)
				{
					try
					{
						ProxyConnect *con = NULL;
						if (type.equals_ci("HTTP"))
							con = new HTTPProxyConnect(p, port);
						else if (type.equals_ci("SOCKS5"))
							con = new SOCKS5ProxyConnect(p, port);
						else
							continue;
						con->Connect(user->ip.addr(), port);
					}
					catch (const SocketException &ex)
					{
						Log(LOG_DEBUG) << "proxyscan: " << ex.GetReason();
					}
				}
			}
		}
	}
};

MODULE_INIT(ModuleProxyScan)
