/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/dns.h"

static ServiceReference<DNS::Manager> dnsmanager("DNS::Manager", "dns/manager");

struct DNSZone;
class DNSServer;

static Serialize::Checker<std::vector<DNSZone *> > zones("DNSZone");
static Serialize::Checker<std::vector<DNSServer *> > dns_servers("DNSServer");

static std::map<Anope::string, std::list<time_t> > server_quit_times;

struct DNSZone final
	: Serializable
{
	Anope::string name;
	std::set<Anope::string, ci::less> servers;

	DNSZone(const Anope::string &n) : Serializable("DNSZone"), name(n)
	{
		zones->push_back(this);
	}

	~DNSZone() override
	{
		std::vector<DNSZone *>::iterator it = std::find(zones->begin(), zones->end(), this);
		if (it != zones->end())
			zones->erase(it);
	}

	static DNSZone *Find(const Anope::string &name)
	{
		for (auto *zone : *zones)
		{
			if (zone->name.equals_ci(name))
			{
				zone->QueueUpdate();
				return zone;
			}
		}
		return NULL;
	}
};

struct DNSZoneType final
	: Serialize::Type
{
	DNSZoneType()
		: Serialize::Type("DNSZone")
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *zone = static_cast<const DNSZone *>(obj);
		data.Store("name", zone->name);
		unsigned count = 0;
		for (const auto &server : zone->servers)
			data.Store("server" + Anope::ToString(count++), server);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		DNSZone *zone;
		Anope::string zone_name;

		data["name"] >> zone_name;

		if (obj)
		{
			zone = anope_dynamic_static_cast<DNSZone *>(obj);
			data["name"] >> zone->name;
		}
		else
			zone = new DNSZone(zone_name);

		zone->servers.clear();
		for (unsigned count = 0; true; ++count)
		{
			Anope::string server_str;
			data["server" + Anope::ToString(count)] >> server_str;
			if (server_str.empty())
				break;
			zone->servers.insert(server_str);
		}

		return zone;
	}
};

class DNSServer final
	: public Serializable
{
	Anope::string server_name;
	std::vector<Anope::string> ips;
	unsigned limit = 0;
	/* wants to be in the pool */
	bool pooled = false;
	/* is actually in the pool */
	bool active = false;

public:
	friend struct DNSServerType;

	std::set<Anope::string, ci::less> zones;
	time_t repool = 0;

	DNSServer(const Anope::string &sn) : Serializable("DNSServer"), server_name(sn)
	{
		dns_servers->push_back(this);
	}

	~DNSServer() override
	{
		std::vector<DNSServer *>::iterator it = std::find(dns_servers->begin(), dns_servers->end(), this);
		if (it != dns_servers->end())
			dns_servers->erase(it);
	}

	const Anope::string &GetName() const { return server_name; }
	std::vector<Anope::string> &GetIPs() { return ips; }
	unsigned GetLimit() const { return limit; }
	void SetLimit(unsigned l) { limit = l; }

	bool Pooled() const { return pooled; }
	void Pool(bool p)
	{
		if (!p)
			this->SetActive(p);
		pooled = p;
	}

	bool Active() const { return pooled && active; }
	void SetActive(bool p)
	{
		if (p)
			this->Pool(p);
		active = p;

		if (dnsmanager)
		{
			dnsmanager->UpdateSerial();
			for (const auto &zone : zones)
				dnsmanager->Notify(zone);
		}
	}

	static DNSServer *Find(const Anope::string &s)
	{
		for (auto *serv : *dns_servers)
			if (serv->GetName().equals_ci(s))
			{
				serv->QueueUpdate();
				return serv;
			}
		return NULL;
	}
};

struct DNSServerType final
	: Serialize::Type
{
	DNSServerType()
		: Serialize::Type("DNSServer")
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *req = static_cast<const DNSServer *>(obj);
		data.Store("server_name", req->server_name);
		for (unsigned i = 0; i < req->ips.size(); ++i)
			data.Store("ip" + Anope::ToString(i), req->ips[i]);
		data.Store("limit", req->limit);
		data.Store("pooled", req->pooled);
		unsigned count = 0;
		for (const auto &zone : req->zones)
			data.Store("zone" + Anope::ToString(count++), zone);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		DNSServer *req;
		Anope::string server_name;

		data["server_name"] >> server_name;

		if (obj)
		{
			req = anope_dynamic_static_cast<DNSServer *>(obj);
			req->server_name = server_name;
		}
		else
			req = new DNSServer(server_name);

		for (unsigned i = 0; true; ++i)
		{
			Anope::string ip_str;
			data["ip" + Anope::ToString(i)] >> ip_str;
			if (ip_str.empty())
				break;
			req->ips.push_back(ip_str);
		}

		data["limit"] >> req->limit;
		data["pooled"] >> req->pooled;

		req->zones.clear();
		for (unsigned i = 0; true; ++i)
		{
			Anope::string zone_str;
			data["zone" + Anope::ToString(i)] >> zone_str;
			if (zone_str.empty())
				break;
			req->zones.insert(zone_str);
		}

		return req;
	}
};

class CommandOSDNS final
	: public Command
{
	static void DisplayPoolState(CommandSource &source)
	{
		if (dns_servers->empty())
		{
			source.Reply(_("There are no configured servers."));
			return;
		}

		ListFormatter lf(source.GetAccount());
		lf.AddColumn(_("Server")).AddColumn(_("IP")).AddColumn(_("Limit")).AddColumn(_("State"));
		for (auto *s : *dns_servers)
		{
			Server *srv = Server::Find(s->GetName(), true);

			ListFormatter::ListEntry entry;
			entry["Server"] = s->GetName();
			entry["Limit"] = s->GetLimit() ? Anope::ToString(s->GetLimit()) : Language::Translate(source.GetAccount(), _("None"));

			Anope::string ip_str;
			for (const auto &ip : s->GetIPs())
				ip_str += ip + " ";
			ip_str.trim();
			if (ip_str.empty())
				ip_str = "None";
			entry["IP"] = ip_str;

			if (s->Active())
				entry["State"] = Language::Translate(source.GetAccount(), _("Pooled/Active"));
			else if (s->Pooled())
				entry["State"] = Language::Translate(source.GetAccount(), _("Pooled/Not Active"));
			else
				entry["State"] = Language::Translate(source.GetAccount(), _("Unpooled"));

			if (!srv)
				entry["State"] += Anope::string(" ") + Language::Translate(source.GetAccount(), _("(Split)"));

			lf.AddEntry(entry);
		}

		std::vector<Anope::string> replies;
		lf.Process(replies);

		if (!zones->empty())
		{
			ListFormatter lf2(source.GetAccount());
			lf2.AddColumn(_("Zone")).AddColumn(_("Servers"));

			for (auto *z : *zones)
			{
				ListFormatter::ListEntry entry;
				entry["Zone"] = z->name;

				Anope::string server_str;
				for (const auto &server : z->servers)
					server_str += server + " ";
				server_str.trim();

				if (server_str.empty())
					server_str = "None";

				entry["Servers"] = server_str;

				lf2.AddEntry(entry);
			}

			lf2.Process(replies);
		}

		for (const auto &reply : replies)
			source.Reply(reply);
	}

	void AddZone(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &zone = params[1];

		if (DNSZone::Find(zone))
		{
			source.Reply(_("Zone %s already exists."), zone.c_str());
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

		Log(LOG_ADMIN, source, this) << "to add zone " << zone;

		new DNSZone(zone);
		source.Reply(_("Added zone %s."), zone.c_str());
	}

	void DelZone(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &zone = params[1];

		DNSZone *z = DNSZone::Find(zone);
		if (!z)
		{
			source.Reply(_("Zone %s does not exist."), zone.c_str());
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

		Log(LOG_ADMIN, source, this) << "to delete zone " << z->name;

		for (const auto &server : z->servers)
		{
			DNSServer *s = DNSServer::Find(server);
			if (s)
				s->zones.erase(z->name);
		}

		if (dnsmanager)
		{
			dnsmanager->UpdateSerial();
			dnsmanager->Notify(z->name);
		}

		source.Reply(_("Zone %s removed."), z->name.c_str());
		delete z;
	}

	void AddServer(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServer::Find(params[1]);
		const Anope::string &zone = params.size() > 2 ? params[2] : "";

		if (s)
		{
			if (zone.empty())
			{
				source.Reply(_("Server %s already exists."), s->GetName().c_str());
			}
			else
			{
				DNSZone *z = DNSZone::Find(zone);
				if (!z)
				{
					source.Reply(_("Zone %s does not exist."), zone.c_str());
					return;
				}
				else if (z->servers.count(s->GetName()))
				{
					source.Reply(_("Server %s is already in zone %s."), s->GetName().c_str(), z->name.c_str());
					return;
				}

				if (Anope::ReadOnly)
					source.Reply(READ_ONLY_MODE);

				z->servers.insert(s->GetName());
				s->zones.insert(zone);

				if (dnsmanager)
				{
					dnsmanager->UpdateSerial();
					dnsmanager->Notify(zone);
				}

				Log(LOG_ADMIN, source, this) << "to add server " << s->GetName() << " to zone " << z->name;

				source.Reply(_("Server %s added to zone %s."), s->GetName().c_str(), z->name.c_str());
			}

			return;
		}

		Server *serv = Server::Find(params[1], true);
		if (!serv || serv == Me || serv->IsJuped())
		{
			source.Reply(_("Server %s is not linked to the network."), params[1].c_str());
			return;
		}

		s = new DNSServer(params[1]);
		if (zone.empty())
		{
			if (Anope::ReadOnly)
				source.Reply(READ_ONLY_MODE);

			Log(LOG_ADMIN, source, this) << "to add server " << s->GetName();
			source.Reply(_("Added server %s."), s->GetName().c_str());
		}
		else
		{
			DNSZone *z = DNSZone::Find(zone);
			if (!z)
			{
				source.Reply(_("Zone %s does not exist."), zone.c_str());
				delete s;
				return;
			}

			if (Anope::ReadOnly)
				source.Reply(READ_ONLY_MODE);

			Log(LOG_ADMIN, source, this) << "to add server " << s->GetName() << " to zone " << zone;

			z->servers.insert(s->GetName());
			s->zones.insert(z->name);

			if (dnsmanager)
			{
				dnsmanager->UpdateSerial();
				dnsmanager->Notify(z->name);
			}
		}
	}

	void DelServer(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServer::Find(params[1]);
		const Anope::string &zone = params.size() > 2 ? params[2] : "";

		if (!s)
		{
			source.Reply(_("Server %s does not exist."), params[1].c_str());
			return;
		}
		else if (!zone.empty())
		{
			DNSZone *z = DNSZone::Find(zone);
			if (!z)
			{
				source.Reply(_("Zone %s does not exist."), zone.c_str());
				return;
			}
			else if (!z->servers.count(s->GetName()))
			{
				source.Reply(_("Server %s is not in zone %s."), s->GetName().c_str(), z->name.c_str());
				return;
			}

			if (Anope::ReadOnly)
				source.Reply(READ_ONLY_MODE);

			Log(LOG_ADMIN, source, this) << "to remove server " << s->GetName() << " from zone " << z->name;

			if (dnsmanager)
			{
				dnsmanager->UpdateSerial();
				dnsmanager->Notify(z->name);
			}

			z->servers.erase(s->GetName());
			s->zones.erase(z->name);
			source.Reply(_("Removed server %s from zone %s."), s->GetName().c_str(), z->name.c_str());
			return;
		}
		else if (Server::Find(s->GetName(), true))
		{
			source.Reply(_("Server %s must be quit before it can be deleted."), s->GetName().c_str());
			return;
		}

		for (const auto &zone : s->zones)
		{
			DNSZone *z = DNSZone::Find(zone);
			if (z)
				z->servers.erase(s->GetName());
		}

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

		if (dnsmanager)
			dnsmanager->UpdateSerial();

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

		for (const auto &ip : s->GetIPs())
		{
			if (params[2].equals_ci(ip))
			{
				source.Reply(_("IP %s already exists for %s."), ip.c_str(), s->GetName().c_str());
				return;
			}
		}

		sockaddrs addr(params[2]);
		if (!addr.valid())
		{
			source.Reply(_("%s is not a valid IP address."), params[2].c_str());
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

		s->GetIPs().push_back(params[2]);
		source.Reply(_("Added IP %s to %s."), params[2].c_str(), s->GetName().c_str());
		Log(LOG_ADMIN, source, this) << "to add IP " << params[2] << " to " << s->GetName();

		if (s->Active() && dnsmanager)
		{
			dnsmanager->UpdateSerial();
			for (const auto &zone : s->zones)
				dnsmanager->Notify(zone);
		}
	}

	void DelIP(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServer::Find(params[1]);

		if (!s)
		{
			source.Reply(_("Server %s does not exist."), params[1].c_str());
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

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

				if (s->Active() && dnsmanager)
				{
					dnsmanager->UpdateSerial();
					for (const auto &zone : s->zones)
						dnsmanager->Notify(zone);
				}

				return;
			}

		source.Reply(_("IP %s does not exist for %s."), params[2].c_str(), s->GetName().c_str());
	}

	static void OnSet(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServer::Find(params[1]);

		if (!s)
		{
			source.Reply(_("Server %s does not exist."), params[1].c_str());
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

		if (params[2].equals_ci("LIMIT"))
		{
			if (auto l = Anope::TryConvert<unsigned>(params[3]))
			{
				s->SetLimit(l.value());
				if (s->GetLimit())
					source.Reply(_("User limit for %s set to %d."), s->GetName().c_str(), s->GetLimit());
				else
					source.Reply(_("User limit for %s removed."), s->GetName().c_str());
			}
			else
			{
				source.Reply(_("Invalid value for LIMIT. Must be numerical."));
			}
		}
		else
			source.Reply(_("Unknown SET option."));
	}

	void OnPool(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServer::Find(params[1]);

		if (!s)
		{
			source.Reply(_("Server %s does not exist."), params[1].c_str());
			return;
		}
		else if (!Server::Find(s->GetName(), true))
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
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

		s->SetActive(true);

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

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

		s->Pool(false);

		source.Reply(_("Depooled %s."), s->GetName().c_str());
		Log(LOG_ADMIN, source, this) << "to depool " << s->GetName();
	}

public:
	CommandOSDNS(Module *creator) : Command(creator, "operserv/dns", 0, 4)
	{
		this->SetDesc(_("Manage DNS zones for this network"));
		this->SetSyntax(_("ADDZONE \037zone.name\037"));
		this->SetSyntax(_("DELZONE \037zone.name\037"));
		this->SetSyntax(_("ADDSERVER \037server.name\037 [\037zone.name\037]"));
		this->SetSyntax(_("DELSERVER \037server.name\037 [\037zone.name\037]"));
		this->SetSyntax(_("ADDIP \037server.name\037 \037ip\037"));
		this->SetSyntax(_("DELIP \037server.name\037 \037ip\037"));
		this->SetSyntax(_("SET \037server.name\037 \037option\037 \037value\037"));
		this->SetSyntax(_("POOL \037server.name\037"));
		this->SetSyntax(_("DEPOOL \037server.name\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (params.empty())
			this->DisplayPoolState(source);
		else if (params[0].equals_ci("ADDZONE") && params.size() > 1)
			this->AddZone(source, params);
		else if (params[0].equals_ci("DELZONE") && params.size() > 1)
			this->DelZone(source, params);
		else if (params[0].equals_ci("ADDSERVER") && params.size() > 1)
			this->AddServer(source, params);
		else if (params[0].equals_ci("DELSERVER") && params.size() > 1)
			this->DelServer(source, params);
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

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"This command allows managing DNS zones used for controlling what servers users "
			"are directed to when connecting. Omitting all parameters prints out the status of "
			"the DNS zone."
			"\n\n"
			"\002ADDZONE\002 adds a zone, eg us.yournetwork.tld. Servers can then be added to this "
			"zone with the \002ADDSERVER\002 command."
			"\n\n"
			"The \002ADDSERVER\002 command adds a server to the given zone. When a query is done, the "
			"zone in question is served if it exists, else all servers in all zones are served. "
			"A server may be in more than one zone."
			"\n\n"
			"The \002ADDIP\002 command associates an IP with a server."
			"\n\n"
			"The \002POOL\002 and \002DEPOOL\002 commands actually add and remove servers to their given zones."
		));
		return true;
	}
};

class ModuleDNS final
	: public Module
{
	DNSZoneType zone_type;
	DNSServerType dns_type;
	CommandOSDNS commandosdns;

	time_t ttl;
	int user_drop_mark;
	time_t user_drop_time;
	time_t user_drop_readd_time;
	bool remove_split_servers;
	bool readd_connected_servers;

	time_t last_warn = 0;

public:
	ModuleDNS(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, EXTRA | VENDOR)
		, commandosdns(this)
	{
		for (auto *s : *dns_servers)
		{
			if (s->Pooled() && Server::Find(s->GetName(), true))
				s->SetActive(true);
		}
	}

	~ModuleDNS() override
	{
		for (unsigned i = zones->size(); i > 0; --i)
			delete zones->at(i - 1);
		for (unsigned i = dns_servers->size(); i > 0; --i)
			delete dns_servers->at(i - 1);
	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &block = conf.GetModule(this);
		this->ttl = block.Get<time_t>("ttl");
		this->user_drop_mark =  block.Get<int>("user_drop_mark");
		this->user_drop_time = block.Get<time_t>("user_drop_time");
		this->user_drop_readd_time = block.Get<time_t>("user_drop_readd_time");
		this->remove_split_servers = block.Get<bool>("remove_split_servers");
		this->readd_connected_servers = block.Get<bool>("readd_connected_servers");
	}

	void OnNewServer(Server *s) override
	{
		if (s == Me || s->IsJuped())
			return;
		if (!Me->IsSynced() || this->readd_connected_servers)
		{
			DNSServer *dns = DNSServer::Find(s->GetName());
			if (dns && dns->Pooled() && !dns->Active() && !dns->GetIPs().empty())
			{
				dns->SetActive(true);
				Log(this) << "Pooling server " << s->GetName();
			}
		}
	}

	void OnServerQuit(Server *s) override
	{
		DNSServer *dns = DNSServer::Find(s->GetName());
		if (remove_split_servers && dns && dns->Pooled() && dns->Active())
		{
			if (readd_connected_servers)
				dns->SetActive(false); // Will be reactivated when it comes back
			else
				dns->Pool(false); // Otherwise permanently pull this
			Log(this) << "Depooling delinked server " << s->GetName();
		}
	}

	void OnUserConnect(User *u, bool &exempt) override
	{
		if (!u->Quitting() && u->server)
		{
			DNSServer *s = DNSServer::Find(u->server->GetName());
			/* Check for user limit reached */
			if (s && s->Pooled() && s->Active() && s->GetLimit() && u->server->users >= s->GetLimit())
			{
				Log(this) << "Depooling full server " << s->GetName() << ": " << u->server->users << " users";
				s->SetActive(false);
			}
		}
	}

	void OnPreUserLogoff(User *u) override
	{
		if (u && u->server)
		{
			DNSServer *s = DNSServer::Find(u->server->GetName());
			if (!s || !s->Pooled())
				return;

			/* Check for dropping under userlimit */
			if (s->GetLimit() && !s->Active() && s->GetLimit() > u->server->users)
			{
				Log(this) << "Pooling server " << s->GetName();
				s->SetActive(true);
			}

			if (this->user_drop_mark > 0)
			{
				std::list<time_t>& times = server_quit_times[u->server->GetName()];
				times.push_back(Anope::CurTime);
				if (times.size() > static_cast<unsigned>(this->user_drop_mark))
					times.pop_front();

				if (times.size() == static_cast<unsigned>(this->user_drop_mark))
				{
					time_t diff = Anope::CurTime - *times.begin();

					/* Check for very fast user drops */
					if (s->Active() && diff <= this->user_drop_time)
					{
						Log(this) << "Depooling server " << s->GetName() << ": dropped " << this->user_drop_mark << " users in " << diff << " seconds";
						s->repool = Anope::CurTime + this->user_drop_readd_time;
						s->SetActive(false);
					}
					/* Check for needing to re-pool a server that dropped users */
					else if (!s->Active() && s->repool && s->repool <= Anope::CurTime)
					{
						s->SetActive(true);
						s->repool = 0;
						Log(this) << "Pooling server " << s->GetName();
					}
				}
			}
		}
	}

	void OnDnsRequest(DNS::Query &req, DNS::Query *packet) override
	{
		if (req.questions.empty())
			return;
		/* Currently we reply to any QR for A/AAAA */
		const DNS::Question &q = req.questions[0];
		if (q.type != DNS::QUERY_A && q.type != DNS::QUERY_AAAA && q.type != DNS::QUERY_AXFR && q.type != DNS::QUERY_ANY)
			return;

		DNSZone *zone = DNSZone::Find(q.name);
		size_t answer_size = packet->answers.size();
		if (zone)
		{
			for (const auto &server : zone->servers)
			{
				DNSServer *s = DNSServer::Find(server);
				if (!s || !s->Active())
					continue;

				for (const auto &ip : s->GetIPs())
				{
					DNS::QueryType q_type = ip.find(':') != Anope::string::npos ? DNS::QUERY_AAAA : DNS::QUERY_A;

					if (q.type == DNS::QUERY_AXFR || q.type == DNS::QUERY_ANY || q_type == q.type)
					{
						DNS::ResourceRecord rr(q.name, q_type);
						rr.ttl = this->ttl;
						rr.rdata = ip;
						packet->answers.push_back(rr);
					}
				}
			}
		}

		if (packet->answers.size() == answer_size)
		{
			/* Default zone */
			for (auto *s : *dns_servers)
			{
				if (!s->Active())
					continue;

				for (const auto &ip : s->GetIPs())
				{
					DNS::QueryType q_type = ip.find(':') != Anope::string::npos ? DNS::QUERY_AAAA : DNS::QUERY_A;

					if (q.type == DNS::QUERY_AXFR || q.type == DNS::QUERY_ANY || q_type == q.type)
					{
						DNS::ResourceRecord rr(q.name, q_type);
						rr.ttl = this->ttl;
						rr.rdata = ip;
						packet->answers.push_back(rr);
					}
				}
			}
		}

		if (packet->answers.size() == answer_size)
		{
			if (last_warn + 60 < Anope::CurTime)
			{
				last_warn = Anope::CurTime;
				Log(this) << "Warning! There are no pooled servers!";
			}

			/* Something messed up, just return them all and hope one is available */
			for (auto *s : *dns_servers)
			{
				for (const auto &ip : s->GetIPs())
				{
					DNS::QueryType q_type = ip.find(':') != Anope::string::npos ? DNS::QUERY_AAAA : DNS::QUERY_A;

					if (q.type == DNS::QUERY_AXFR || q.type == DNS::QUERY_ANY || q_type == q.type)
					{
						DNS::ResourceRecord rr(q.name, q_type);
						rr.ttl = this->ttl;
						rr.rdata = ip;
						packet->answers.push_back(rr);
					}
				}
			}

			if (packet->answers.size() == answer_size)
			{
				Log(this) << "Error! There are no servers with any IPs of type " << q.type;
			}
		}
	}
};

MODULE_INIT(ModuleDNS)
