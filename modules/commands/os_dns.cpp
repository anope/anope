/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "../extra/dns.h"

static ServiceReference<DNS::Manager> dnsmanager("DNS::Manager", "dns/manager");

class DNSServer;
static std::vector<DNSServer *> dns_servers;

static std::map<Anope::string, std::list<time_t> > server_quit_times;

class DNSServer : public Serializable
{
	Anope::string server_name;
	std::vector<Anope::string> ips;
	unsigned limit;
 	bool pooled;

	DNSServer() : Serializable("DNSServer"), limit(0), pooled(false), repool(0) { dns_servers.push_back(this); }
 public:
	time_t repool;

	DNSServer(const Anope::string &sn) : Serializable("DNSServer"), server_name(sn), limit(0), pooled(false), repool(0)
	{
		dns_servers.push_back(this);
	}

	~DNSServer()
	{
		std::vector<DNSServer *>::iterator it = std::find(dns_servers.begin(), dns_servers.end(), this);
		if (it != dns_servers.end())
			dns_servers.erase(it);
	}

	const Anope::string &GetName() const { return server_name; }
	std::vector<Anope::string> &GetIPs() { return ips; }
	unsigned GetLimit() const { return limit; }
	void SetLimit(unsigned l) { limit = l; }
	bool Pooled() const { return pooled; }

	void Pool(bool p)
	{
		pooled = p;
		if (dnsmanager)
			dnsmanager->UpdateSerial();
	}


	void Serialize(Serialize::Data &data) const anope_override
	{
		data["server_name"] << server_name;
		for (unsigned i = 0; i < ips.size(); ++i)
			data["ip" + stringify(i)] << ips[i];
		data["limit"] << limit;
		data["pooled"] << pooled;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data)
	{
		DNSServer *req;

		if (obj)
			req = anope_dynamic_static_cast<DNSServer *>(obj);
		else
			req = new DNSServer();

		data["server_name"] >> req->server_name;

		for (unsigned i = 0; true; ++i)
		{
			Anope::string ip_str;
			data["ip" + stringify(i)] >> ip_str;
			if (ip_str.empty())
				break;
			req->ips.push_back(ip_str);
		}

		data["limit"] >> req->limit;
		data["pooled"] >> req->pooled;

		return req;
	}

	static DNSServer *Find(const Anope::string &s)
	{
		for (unsigned i = 0; i < dns_servers.size(); ++i)
			if (dns_servers[i]->GetName() == s)
				return dns_servers[i];
		return NULL;
	}
};

class CommandOSDNS : public Command
{
	void DisplayPoolState(CommandSource &source)
	{
		if (dns_servers.empty())
		{
			source.Reply(_("There are no configured servers."));
			return;
		}

		ListFormatter lf;
		lf.AddColumn("Server").AddColumn("IP").AddColumn("Limit").AddColumn("State");
		for (unsigned i = 0; i < dns_servers.size(); ++i)
		{
			DNSServer *s = dns_servers[i];
			Server *srv = Server::Find(s->GetName());

			ListFormatter::ListEntry entry;
			entry["Server"] = s->GetName();
			entry["Limit"] = s->GetLimit() ? stringify(s->GetLimit()) : "None";

			Anope::string ip_str;
			for (unsigned j = 0; j < s->GetIPs().size(); ++j)
				ip_str += s->GetIPs()[j] + " ";
			ip_str.trim();
			if (ip_str.empty())
				ip_str = "None";
			entry["IP"] = ip_str;

			if (!srv)
				entry["State"] = "Split";
			else if (s->Pooled())
				entry["State"] = "Pooled";
			else
				entry["State"] = "Unpooled";

			lf.AddEntry(entry);
		}

		std::vector<Anope::string> replies;
		lf.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

	void OnAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServer::Find(params[1]);

		if (s)
		{
			source.Reply(_("Server %s already exists."), params[1].c_str());
			return;
		}

		if (!Server::Find(params[1]))
		{
			source.Reply(_("Server %s is not linked to the network."), params[1].c_str());
			return;
		}

		s = new DNSServer(params[1]);
		source.Reply(_("Added server %s."), s->GetName().c_str());

		Log(LOG_ADMIN, source, this) << "to add server " << s->GetName();
	}

	void OnDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServer::Find(params[1]);
		
		if (!s)
		{
			source.Reply(_("Server %s does not exist."), params[1].c_str());
			return;
		}
		else if (Server::Find(s->GetName()))
		{
			source.Reply(_("Server %s must be quit before it can be deleted."), s->GetName().c_str());
			return;
		}

		Log(LOG_ADMIN, source, this) << "to delete server " << s->GetName();
		source.Reply(_("Removed server %s."), s->GetName().c_str());
		delete s;
	}

	void AddIP(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServer::Find(params[1]);
		
		if (!s)
		{
			source.Reply(_("Server %s does not exist."), params[1].c_str());
			return;
		}

		for (unsigned i = 0; i < s->GetIPs().size(); ++i)
			if (params[2].equals_ci(s->GetIPs()[i]))
			{
				source.Reply(_("IP %s already exists for %s."), s->GetIPs()[i].c_str(), s->GetName().c_str());
				return;
			}

		sockaddrs addr;
		try
		{
			addr.pton(AF_INET, params[2]);
		}
		catch (const SocketException &)
		{
			try
			{
				addr.pton(AF_INET6, params[2]);
			}
			catch (const SocketException &)
			{
				source.Reply(_("%s is not a valid IP address."), params[2].c_str());
				return;
			}
		}

		s->GetIPs().push_back(params[2]);
		source.Reply(_("Added IP %s to %s."), params[2].c_str(), s->GetName().c_str());
		Log(LOG_ADMIN, source, this) << "to add IP " << params[2] << " to " << s->GetName();

		if (s->Pooled() && dnsmanager)
			dnsmanager->UpdateSerial();
	}
	
	void DelIP(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServer::Find(params[1]);
		
		if (!s)
		{
			source.Reply(_("Server %s does not exist."), params[1].c_str());
			return;
		}

		for (unsigned i = 0; i < s->GetIPs().size(); ++i)
			if (params[2].equals_ci(s->GetIPs()[i]))
			{
				s->GetIPs().erase(s->GetIPs().begin() + i);
				source.Reply(_("Removed IP %s from %s."), params[2].c_str(), s->GetName().c_str());
				Log(LOG_ADMIN, source, this) << "to remove IP " << params[2] << " from " << s->GetName();

				if (s->GetIPs().empty())
				{
					s->repool = 0;
					s->Pool(false);
				}

				if (s->Pooled() && dnsmanager)
					dnsmanager->UpdateSerial();

				return;
			}

		source.Reply(_("IP %s does not exist for %s."), params[2].c_str(), s->GetName().c_str());
	}

	void OnSet(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServer::Find(params[1]);
		
		if (!s)
		{
			source.Reply(_("Server %s does not exist."), params[1].c_str());
			return;
		}

		if (params[2].equals_ci("LIMIT"))
		{
			try
			{
				unsigned l = convertTo<unsigned>(params[3]);
				s->SetLimit(l);
				if (l)
					source.Reply(_("User limit for %s set to %d."), s->GetName().c_str(), l);
				else
					source.Reply(_("User limit for %s removed."), s->GetName().c_str());
			}
			catch (const ConvertException &ex)
			{
				source.Reply(_("Invalid value for LIMIT. Must be numerical."));
			}
		}
		else
			source.Reply(_("Unknown SET option"));
	}

	void OnPool(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServer::Find(params[1]);
		
		if (!s)
		{
			source.Reply(_("Server %s does not exist."), params[1].c_str());
			return;
		}
		else if (!Server::Find(s->GetName()))
		{
			source.Reply(_("Server %s is not currently linked."), s->GetName().c_str());
			return;
		}
		else if (s->Pooled())
		{
			source.Reply(_("Server %s is already pooled."), s->GetName().c_str());
			return;
		}
		else if (s->GetIPs().empty())
		{
			source.Reply(_("Server %s has no configured IPs."), s->GetName().c_str());
		}

		s->Pool(true);

		source.Reply(_("Pooled %s."), s->GetName().c_str());
		Log(LOG_ADMIN, source, this) << "to pool " << s->GetName();
	}


	void OnDepool(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServer::Find(params[1]);
		
		if (!s)
		{
			source.Reply(_("Server %s does not exist."), params[1].c_str());
			return;
		}
		else if (!s->Pooled())
		{
			source.Reply(_("Server %s is not pooled."), s->GetName().c_str());
			return;
		}

		s->Pool(false);

		source.Reply(_("Depooled %s."), s->GetName().c_str());
		Log(LOG_ADMIN, source, this) << "to depool " << s->GetName();
	}

 public:
	CommandOSDNS(Module *creator) : Command(creator, "operserv/dns", 0, 3)
	{
		this->SetDesc(_("Manage the DNS zone for this network"));
		this->SetSyntax(_("ADD \037server.name\037"));
		this->SetSyntax(_("DEL \037server.name\037"));
		this->SetSyntax(_("ADDIP \037server.name\037 \037ip\037"));
		this->SetSyntax(_("DELIP \037server.name\037 \037ip\037"));
		this->SetSyntax(_("SET \037server.name\037 \37option\37 \037value\037"));
		this->SetSyntax(_("POOL \037server.name\037"));
		this->SetSyntax(_("DEPOOL \037server.name\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.empty())
			this->DisplayPoolState(source);
		else if (params[0].equals_ci("ADD") && params.size() > 1)
			this->OnAdd(source, params);
		else if (params[0].equals_ci("DEL") && params.size() > 1)
			this->OnDel(source, params);
		else if (params[0].equals_ci("ADDIP") && params.size() > 2)
			this->AddIP(source, params);
		else if (params[0].equals_ci("DELIP") && params.size() > 2)
			this->DelIP(source, params);
		else if (params[0].equals_ci("SET") && params.size() > 3)
			this->OnSet(source, params);
		else if (params[0].equals_ci("POOL") && params.size() > 1)
			this->OnPool(source, params);
		else if (params[0].equals_ci("DEPOOL") && params.size() > 1)
			this->OnDepool(source, params);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command allows managing a DNS zone\n"
				"used for controlling what servers users\n"
				"are directed to when connecting. Omitting\n"
				"all parameters prints out the status of\n"
				"the DNS zone.\n"));
		return true;
	}
};

class ModuleDNS : public Module
{
	Serialize::Type dns_type;
	CommandOSDNS commandosdns;

	time_t ttl;
	int user_drop_mark;
	time_t user_drop_time;
	time_t user_drop_readd_time;
	bool remove_split_servers;
	bool readd_connected_servers;

 public:
	ModuleDNS(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED),
		dns_type("DNSServer", DNSServer::Unserialize), commandosdns(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnNewServer, I_OnServerQuit, I_OnUserConnect, I_OnUserLogoff, I_OnDnsRequest };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	void OnReload() anope_override
	{
		ConfigReader config;

		this->ttl = Anope::DoTime(config.ReadValue("os_dns", "ttl", 0));
		this->user_drop_mark = config.ReadInteger("os_dns", "user_drop_mark", 0, false);
		this->user_drop_time = Anope::DoTime(config.ReadValue("os_dns", "user_drop_time", 0, false));
		this->user_drop_readd_time = Anope::DoTime(config.ReadValue("os_dns", "user_drop_readd_time", 0, false));
		this->remove_split_servers = config.ReadFlag("os_dns", "remove_split_servers", 0);
		this->readd_connected_servers = config.ReadFlag("os_dns", "readd_connected_servers", 0);
	}

	void OnNewServer(Server *s) anope_override
	{
		if (this->readd_connected_servers)
		{
			DNSServer *dns = DNSServer::Find(s->GetName());
			if (dns && !dns->Pooled() && !dns->GetIPs().empty() && dns->GetLimit() < s->users)
			{
				dns->Pool(true);
				Log(this) << "Pooling server " << s->GetName();
			}
		}
	}


	void OnServerQuit(Server *s) anope_override
	{
		DNSServer *dns = DNSServer::Find(s->GetName());
		if (dns && dns->Pooled())
		{
			dns->Pool(false);
			Log(this) << "Depooling delinked server " << s->GetName();
		}
	}

	void OnUserConnect(Reference<User> &u, bool &exempt) anope_override
	{
		if (u && u->server)
		{
			DNSServer *s = DNSServer::Find(u->server->GetName());
			/* Check for user limit reached */
			if (s && s->GetLimit() && s->Pooled() && u->server->users >= s->GetLimit())
			{
				Log(this) << "Depooling full server " << s->GetName() << ": " << u->server->users << " users";
				s->Pool(false);
			}
		}
	}

	void OnUserLogoff(User *u) anope_override
	{
		if (u && u->server)
		{
			DNSServer *s = DNSServer::Find(u->server->GetName());
			if (!s)
				return;

			/* Check for dropping under userlimit */
			if (s->GetLimit() && !s->Pooled() && s->GetLimit() > u->server->users)
			{
				Log(this) << "Pooling server " << s->GetName();
				s->Pool(true);
			}

			if (this->user_drop_mark > 0)
			{
				std::list<time_t>& times = server_quit_times[u->server->GetName()];
				times.push_back(Anope::CurTime);
				if (times.size() > static_cast<unsigned>(this->user_drop_mark))
					times.pop_front();

				if (s->Pooled() && times.size() == static_cast<unsigned>(this->user_drop_mark))
				{
					time_t diff = Anope::CurTime - *times.begin();

					/* Check for very fast user drops */
					if (diff <= this->user_drop_time)
					{
						Log(this) << "Depooling server " << s->GetName() << ": dropped " << this->user_drop_mark << " users in " << diff << " seconds";
						s->repool = Anope::CurTime + this->user_drop_readd_time;
						s->Pool(false);
					}
					/* Check for needing to re-pool a server that dropped users */
					else if (s->repool && s->repool <= Anope::CurTime && !s->Pooled())
					{
						s->Pool(true);
						s->repool = 0;
						Log(this) << "Pooling server " << s->GetName();
					}
				}
			}
		}
	}

	void OnDnsRequest(DNS::Packet &req, DNS::Packet *packet) anope_override
	{
		if (req.questions.empty())
			return;
		/* Currently we reply to any QR for A/AAAA */
		const DNS::Question& q = req.questions[0];
		if (q.type != DNS::QUERY_A && q.type != DNS::QUERY_AAAA && q.type != DNS::QUERY_AXFR)
			return;

		for (unsigned i = 0; i < dns_servers.size(); ++i)
		{
			DNSServer *s = dns_servers[i];
			if (!s->Pooled())
				continue;

			for (unsigned j = 0; j < s->GetIPs().size(); ++j)
			{
				DNS::QueryType q_type = s->GetIPs()[j].find(':') != Anope::string::npos ? DNS::QUERY_AAAA : DNS::QUERY_A;

				if (q.type == DNS::QUERY_AXFR || q_type == q.type)
				{
					DNS::ResourceRecord rr(q.name, q_type);
					rr.ttl = this->ttl;
					rr.rdata = s->GetIPs()[j];
					packet->answers.push_back(rr);
				}
			}
		}

		if (packet->answers.empty())
		{
			static time_t last_warn = 0;
			if (last_warn + 60 < Anope::CurTime)
			{
				last_warn = Anope::CurTime;
				Log(this) << "os_dns: Warning! There are no pooled servers!";
			}

			/* Something messed up, just return them all and hope one is available */
			for (unsigned i = 0; i < dns_servers.size(); ++i)
			{
				DNSServer *s = dns_servers[i];

				for (unsigned j = 0; j < s->GetIPs().size(); ++j)
				{
					DNS::QueryType q_type = s->GetIPs()[j].find(':') != Anope::string::npos ? DNS::QUERY_AAAA : DNS::QUERY_A;

					if (q.type == DNS::QUERY_AXFR || q_type == q.type)
					{
						DNS::ResourceRecord rr(q.name, q_type);
						rr.ttl = this->ttl;
						rr.rdata = s->GetIPs()[j];
						packet->answers.push_back(rr);
					}
				}
			}

			if (packet->answers.empty())
			{
				Log(this) << "os_dns: Error! There are no servers with any IPs. At all.";
				/* Send back an empty answer anyway */
			}
		}
	}
};

MODULE_INIT(ModuleDNS)
