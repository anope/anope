/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/dns.h"
#include "modules/operserv/dns.h"

static std::map<Anope::string, std::list<time_t> > server_quit_times;

class DNSZoneImpl : public DNSZone
{
 public:
	DNSZoneImpl(Serialize::TypeBase *type) : DNSZone(type) { }
	DNSZoneImpl(Serialize::TypeBase *type, Serialize::ID id) : DNSZone(type, id) { }

	Anope::string GetName() override;
	void SetName(const Anope::string &) override;

	static DNSZone *Find(const Anope::string &name)
	{
		for (DNSZone *zone : Serialize::GetObjects<DNSZone *>(dnszone))
			if (zone->GetName().equals_ci(name))
				return zone;
		return nullptr;
	}
};

class DNSZoneType : public Serialize::Type<DNSZoneImpl>
{
 public:
	Serialize::Field<DNSZoneImpl, Anope::string> name;

	DNSZoneType(Module *creator) : Serialize::Type<DNSZoneImpl>(creator, "DNSZone")
		, name(this, "name")
	{
	}
};

Anope::string DNSZoneImpl::GetName()
{
	return Get(&DNSZoneType::name);
}

void DNSZoneImpl::SetName(const Anope::string &name)
{
	Set(&DNSZoneType::name, name);
}

class DNSServerImpl : public DNSServer
{
	/* is actually in the pool */
	bool active = false;

 public:
 	std::set<Anope::string, ci::less> zones;
	time_t repool = 0;

	DNSServerImpl(Serialize::TypeBase *type) : DNSServer(type) { }
	DNSServerImpl(Serialize::TypeBase *type, Serialize::ID id) : DNSServer(type, id) { }

	DNSZone *GetZone() override;
	void SetZone(DNSZone *) override;

	Anope::string GetName() override;
	void SetName(const Anope::string &) override;

	unsigned int GetLimit() override;
	void SetLimit(const unsigned int &) override;

	bool GetPooled() override;
	void SetPool(const bool &) override;

	bool Active()
	{
		return GetPooled() && active;
	}

	void SetActive(bool p)
	{
		if (p)
			this->SetPool(p);
		active = p;

		if (DNS::manager)
		{
			DNS::manager->UpdateSerial();
			for (std::set<Anope::string, ci::less>::iterator it = zones.begin(), it_end = zones.end(); it != it_end; ++it)
				DNS::manager->Notify(*it);
		}
	}

	static DNSServerImpl *Find(const Anope::string &s)
	{
		for (DNSServerImpl *server : Serialize::GetObjects<DNSServerImpl *>(dnsserver))
			if (server->GetName().equals_ci(s))
				return server;
		return nullptr;
	}
};

class DNSServerType : public Serialize::Type<DNSServerImpl>
{
 public:
	Serialize::ObjectField<DNSServerImpl, DNSZone *> zone;
	Serialize::Field<DNSServerImpl, Anope::string> name;
	Serialize::Field<DNSServerImpl, unsigned int> limit;
	Serialize::Field<DNSServerImpl, bool> pooled;

	DNSServerType(Module *creator) : Serialize::Type<DNSServerImpl>(creator, "DNSServer")
		, zone(this, "zone")
		, name(this, "name")
		, limit(this, "limit")
		, pooled(this, "pooled")
	{
	}
};

DNSZone *DNSServerImpl::GetZone()
{
	return Get(&DNSServerType::zone);
}

void DNSServerImpl::SetZone(DNSZone *z)
{
	Set(&DNSServerType::zone, z);
}

Anope::string DNSServerImpl::GetName()
{
	return Get(&DNSServerType::name);
}

void DNSServerImpl::SetName(const Anope::string &n)
{
	Set(&DNSServerType::name, n);
}

unsigned int DNSServerImpl::GetLimit()
{
	return Get(&DNSServerType::limit);
}

void DNSServerImpl::SetLimit(const unsigned int &l)
{
	Set(&DNSServerType::limit, l);
}

bool DNSServerImpl::GetPooled()
{
	return Get(&DNSServerType::pooled);
}

void DNSServerImpl::SetPool(const bool &p)
{
	Set(&DNSServerType::pooled, p);
}

class DNSZoneMembershipImpl : public DNSZoneMembership
{
 public:
	DNSZoneMembershipImpl(Serialize::TypeBase *type) : DNSZoneMembership(type) { }
	DNSZoneMembershipImpl(Serialize::TypeBase *type, Serialize::ID id) : DNSZoneMembership(type, id) { }

	DNSServer *GetServer() override;
	void SetServer(DNSServer *) override;

	DNSZone *GetZone() override;
	void SetZone(DNSZone *) override;

	static DNSZoneMembership *Find(DNSServer *server, DNSZone *zone)
	{
		for (DNSZoneMembership *mem : Serialize::GetObjects<DNSZoneMembership *>(dnszonemembership))
			if (mem->GetServer() == server && mem->GetZone() == zone)
				return mem;
		return nullptr;
	}
};

class DNSZoneMembershipType : public Serialize::Type<DNSZoneMembershipImpl>
{
 public:
	Serialize::ObjectField<DNSZoneMembershipImpl, DNSServer *> server;
	Serialize::ObjectField<DNSZoneMembershipImpl, DNSZone *> zone;

	DNSZoneMembershipType(Module *creator) : Serialize::Type<DNSZoneMembershipImpl>(creator, "DNSZoneMembership")
		, server(this, "server")
		, zone(this, "zone")
	{
	}
};

DNSServer *DNSZoneMembershipImpl::GetServer()
{
	return Get(&DNSZoneMembershipType::server);
}

void DNSZoneMembershipImpl::SetServer(DNSServer *s)
{
	Set(&DNSZoneMembershipType::server, s);
}

DNSZone *DNSZoneMembershipImpl::GetZone()
{
	return Get(&DNSZoneMembershipType::zone);
}

void DNSZoneMembershipImpl::SetZone(DNSZone *z)
{
	Set(&DNSZoneMembershipType::zone, z);
}

class DNSIPImpl : public DNSIP
{
 public:
	DNSIPImpl(Serialize::TypeBase *type) : DNSIP(type) { }
	DNSIPImpl(Serialize::TypeBase *type, Serialize::ID id) : DNSIP(type, id) { }

	DNSServer *GetServer() override;
	void SetServer(DNSServer *) override;

	Anope::string GetIP() override;
	void SetIP(const Anope::string &) override;
};

class DNSIPType : public Serialize::Type<DNSIPImpl>
{
 public:
	Serialize::ObjectField<DNSServerImpl, DNSServer *> server;
	Serialize::Field<DNSServerImpl, Anope::string> ip;

	DNSIPType(Module *creator) : Serialize::Type<DNSIPImpl>(creator, "DNSIP")
		, server(this, "server")
		, ip(this, "ip")
	{
	}
};

DNSServer *DNSIPImpl::GetServer()
{
	return Get(&DNSIPType::server);
}

void DNSIPImpl::SetServer(DNSServer *s)
{
	Set(&DNSIPType::server, s);
}

Anope::string DNSIPImpl::GetIP()
{
	return Get(&DNSIPType::ip);
}

void DNSIPImpl::SetIP(const Anope::string &ip)
{
	Set(&DNSIPType::ip, ip);
}

class CommandOSDNS : public Command
{
	void DisplayPoolState(CommandSource &source)
	{
		std::vector<DNSServerImpl *> servers = Serialize::GetObjects<DNSServerImpl *>(dnsserver);

		if (servers.empty())
		{
			source.Reply(_("There are no configured servers."));
			return;
		}

		ListFormatter lf(source.GetAccount());
		lf.AddColumn(_("Server")).AddColumn(_("IP")).AddColumn(_("Limit")).AddColumn(_("State"));
		for (DNSServerImpl *s : servers)
		{
			Server *srv = Server::Find(s->GetName(), true);

			ListFormatter::ListEntry entry;
			entry["Server"] = s->GetName();
			entry["Limit"] = s->GetLimit() ? stringify(s->GetLimit()) : Language::Translate(source.GetAccount(), _("None"));

			Anope::string ip_str;
			for (DNSIP *ip : s->GetRefs<DNSIP *>(dnsip))
				ip_str += ip->GetIP() + " ";
			ip_str.trim();
			if (ip_str.empty())
				ip_str = "None";
			entry["IP"] = ip_str;

			if (s->Active())
				entry["State"] = Language::Translate(source.GetAccount(), _("Pooled/Active"));
			else if (s->GetPooled())
				entry["State"] = Language::Translate(source.GetAccount(), _("Pooled/Not Active"));
			else
				entry["State"] = Language::Translate(source.GetAccount(), _("Unpooled"));

			if (!srv)
				entry["State"] += Anope::string(" ") + Language::Translate(source.GetAccount(), _("(Split)"));

			lf.AddEntry(entry);
		}

		std::vector<Anope::string> replies;
		lf.Process(replies);

		std::vector<DNSZone *> zones = Serialize::GetObjects<DNSZone *>(dnszone);
		if (!zones.empty())
		{
			ListFormatter lf2(source.GetAccount());
			lf2.AddColumn(_("Zone")).AddColumn(_("Servers"));

			for (DNSZone *z : zones)
			{
				ListFormatter::ListEntry entry;
				entry["Zone"] = z->GetName();

				Anope::string server_str;
				for (DNSServer *s : z->GetRefs<DNSServer *>(dnsserver))
					server_str += s->GetName() + " ";
				server_str.trim();

				if (server_str.empty())
					server_str = "None";

				entry["Servers"] = server_str;

				lf2.AddEntry(entry);
			}

			lf2.Process(replies);
		}

		for (const Anope::string &r : replies)
			source.Reply(r);
	}

	void AddZone(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &zone = params[1];

		if (DNSZoneImpl::Find(zone))
		{
			source.Reply(_("Zone \002{0}\002 already exists."), zone);
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		Log(LOG_ADMIN, source, this) << "to add zone " << zone;

		DNSZone *z = dnszone.Create();
		z->SetName(zone);
		source.Reply(_("Added zone \002{0}\002."), zone);
	}

	void DelZone(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &zone = params[1];

		DNSZone *z = DNSZoneImpl::Find(zone);
		if (!z)
		{
			source.Reply(_("Zone \002{0}\002 does not exist."), zone);
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		Log(LOG_ADMIN, source, this) << "to delete zone " << z->GetName();

		for (DNSZoneMembership *mem : z->GetRefs<DNSZoneMembership *>(dnszonemembership))
			mem->Delete();

		if (DNS::manager)
		{
			DNS::manager->UpdateSerial();
			DNS::manager->Notify(z->GetName());
		}

		source.Reply(_("Zone \002{0}\002 removed."), z->GetName());
		z->Delete();
	}

	void AddServer(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServerImpl::Find(params[1]);
		const Anope::string &zone = params.size() > 2 ? params[2] : "";

		if (s)
		{
			if (zone.empty())
			{
				source.Reply(_("Server \002{0}\002 already exists."), s->GetName());
			}
			else
			{
				DNSZone *z = DNSZoneImpl::Find(zone);
				if (!z)
				{
					source.Reply(_("Zone \002{0}\002 does not exist."), zone);
					return;
				}

				DNSZoneMembership *mem = DNSZoneMembershipImpl::Find(s, z);
				if (mem)
				{
					source.Reply(_("Server \002{0}\002 is already in zone \002{1}\002."), s->GetName(), z->GetName());
					return;
				}

				if (Anope::ReadOnly)
					source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

				mem = dnszonemembership.Create();
				mem->SetZone(z);
				mem->SetServer(s);

				if (DNS::manager)
				{
					DNS::manager->UpdateSerial();
					DNS::manager->Notify(zone);
				}

				Log(LOG_ADMIN, source, this) << "to add server " << s->GetName() << " to zone " << z->GetName();

				source.Reply(_("Server \002{0}\002 added to zone \002{1}\002."), s->GetName(), z->GetName());
			}

			return;
		}

		Server *serv = Server::Find(params[1], true);
		if (!serv || serv == Me || serv->IsJuped())
		{
			source.Reply(_("Server \002{0}\002 is not linked to the network."), params[1]);
			return;
		}

		s = dnsserver.Create();
		s->SetName(params[1]);
		if (zone.empty())
		{
			if (Anope::ReadOnly)
				source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

			Log(LOG_ADMIN, source, this) << "to add server " << s->GetName();
			source.Reply(_("Added server \002{0}\002."), s->GetName());
		}
		else
		{
			DNSZone *z = DNSZoneImpl::Find(zone);
			if (!z)
			{
				source.Reply(_("Zone \002{0}\002 does not exist."), zone);
				s->Delete();
				return;
			}

			if (Anope::ReadOnly)
				source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

			Log(LOG_ADMIN, source, this) << "to add server " << s->GetName() << " to zone " << zone;

			DNSZoneMembership *mem = dnszonemembership.Create();
			mem->SetServer(s);
			mem->SetZone(z);

			if (DNS::manager)
			{
				DNS::manager->UpdateSerial();
				DNS::manager->Notify(z->GetName());
			}
		}
	}

	void DelServer(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServerImpl::Find(params[1]);
		const Anope::string &zone = params.size() > 2 ? params[2] : "";

		if (!s)
		{
			source.Reply(_("Server \002{0}\002 does not exist."), params[1]);
			return;
		}

		if (!zone.empty())
		{
			DNSZone *z = DNSZoneImpl::Find(zone);
			if (!z)
			{
				source.Reply(_("Zone \002{0}\002 does not exist."), zone);
				return;
			}

			DNSZoneMembership *mem = DNSZoneMembershipImpl::Find(s, z);
			if (!mem)
			{
				source.Reply(_("Server \002{0}\002 is not in zone \002{1}\002."), s->GetName(), z->GetName());
				return;
			}

			if (Anope::ReadOnly)
				source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

			Log(LOG_ADMIN, source, this) << "to remove server " << s->GetName() << " from zone " << z->GetName();

			if (DNS::manager)
			{
				DNS::manager->UpdateSerial();
				DNS::manager->Notify(z->GetName());
			}

			mem->Delete();
			source.Reply(_("Removed server \002{0}\002 from zone \002{1}\002."), s->GetName(), z->GetName());
			return;
		}

		if (Server::Find(s->GetName(), true))
		{
			source.Reply(_("Server \002{0}\002 must be quit before it can be deleted."), s->GetName());
			return;
		}

		for (DNSZoneMembership *mem : s->GetRefs<DNSZoneMembership *>(dnszonemembership))
			mem->Delete();

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		if (DNS::manager)
			DNS::manager->UpdateSerial();

		Log(LOG_ADMIN, source, this) << "to delete server " << s->GetName();
		source.Reply(_("Removed server \002{0}\002."), s->GetName());
		s->Delete();
	}

	void AddIP(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServerImpl *s = DNSServerImpl::Find(params[1]);

		if (!s)
		{
			source.Reply(_("Server \002{0}\002 does not exist."), params[1]);
			return;
		}

		for (DNSIP *ip : s->GetRefs<DNSIP *>(dnsip))
			if (params[2].equals_ci(ip->GetIP()))
			{
				source.Reply(_("IP \002{0}\002 already exists for \002{1}\002."), ip->GetIP(), s->GetName());
				return;
			}

		sockaddrs addr(params[2]);
		if (!addr.valid())
		{
			source.Reply(_("\002{0}\002 is not a valid IP address."), params[2]);
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		DNSIP *ip = dnsip.Create();
		ip->SetServer(s);
		ip->SetIP(params[2]);

		source.Reply(_("Added IP \002{0}\002 to \002{1}\002."), params[2], s->GetName());
		Log(LOG_ADMIN, source, this) << "to add IP " << params[2] << " to " << s->GetName();

		if (s->Active() && DNS::manager)
		{
			DNS::manager->UpdateSerial();
			for (DNSZone *zone : s->GetRefs<DNSZone *>(dnszone))
				DNS::manager->Notify(zone->GetName());
		}
	}

	void DelIP(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServerImpl *s = DNSServerImpl::Find(params[1]);

		if (!s)
		{
			source.Reply(_("Server \002{0}\002 does not exist."), params[1]);
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		for (DNSIP *ip : s->GetRefs<DNSIP *>(dnsip))
			if (params[2].equals_ci(ip->GetIP()))
			{
				ip->Delete();

				source.Reply(_("Removed IP \002{0}\002 from \002{1}\002."), params[2], s->GetName());
				Log(LOG_ADMIN, source, this) << "to remove IP " << params[2] << " from " << s->GetName();

				if (s->GetRefs<DNSIP *>(dnsip).empty())
				{
					s->repool = 0;
					s->SetPool(false);
				}

				if (s->Active() && DNS::manager)
				{
					DNS::manager->UpdateSerial();
					for (DNSZone *zone : s->GetRefs<DNSZone *>(dnszone))
						DNS::manager->Notify(zone->GetName());
				}

				return;
			}

		source.Reply(_("IP \002{0}\002 does not exist for \002{1}\002."), params[2], s->GetName());
	}

	void OnSet(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServerImpl::Find(params[1]);

		if (!s)
		{
			source.Reply(_("Server \002{0}\002 does not exist."), params[1]);
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		if (params[2].equals_ci("LIMIT"))
		{
			try
			{
				unsigned l = convertTo<unsigned>(params[3]);
				s->SetLimit(l);
				if (l)
					source.Reply(_("User limit for \002{0}\002 set to \002{1}\002."), s->GetName(), l);
				else
					source.Reply(_("User limit for \002{0}\002 removed."), s->GetName());
			}
			catch (const ConvertException &ex)
			{
				source.Reply(_("Invalid value for limit. Must be numerical."));
			}
		}
		else
			source.Reply(_("Unknown SET option."));
	}

	void OnPool(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServerImpl *s = DNSServerImpl::Find(params[1]);

		if (!s)
		{
			source.Reply(_("Server \002{0}\002 does not exist."), params[1]);
			return;
		}

		if (!Server::Find(s->GetName(), true))
		{
			source.Reply(_("Server \002{0}\002 is not currently linked."), s->GetName());
			return;
		}

		if (s->GetPooled())
		{
			source.Reply(_("Server \002{0}\002 is already pooled."), s->GetName());
			return;
		}

		if (s->GetRefs<DNSIP *>(dnsip).empty())
		{
			source.Reply(_("Server \002{0}\002 has no configured IPs."), s->GetName());
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		s->SetActive(true);

		source.Reply(_("Pooled \002{0}\002."), s->GetName());
		Log(LOG_ADMIN, source, this) << "to pool " << s->GetName();
	}


	void OnDepool(CommandSource &source, const std::vector<Anope::string> &params)
	{
		DNSServer *s = DNSServerImpl::Find(params[1]);

		if (!s)
		{
			source.Reply(_("Server \002{0}\002 does not exist."), params[1]);
			return;
		}

		if (!s->GetPooled())
		{
			source.Reply(_("Server \002{0}\002 is not pooled."), s->GetName());
			return;
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		s->SetPool(false);

		source.Reply(_("Depooled \002{0}\002."), s->GetName());
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
		source.Reply(_("This command allows managing DNS zones used for controlling what servers users are directed to when connecting."
		               " Omitting all parameters prints out the status of the DNS zone.\n"
		               "\n"
		               "\002{0} ADDZONE\002 adds a zone, eg. us.example.com. Servers can then be added to this zone with the \002ADDSERVER\002 command.\n"
			       "\n"
		               "The \002{0} ADDSERVER\002 command adds a server to the given zone."
		               " When a DNS query is done, the zone in question is served if it exists, otherwise all servers in all zones are served."
		               " A server may be in more than one zone.\n"
		               "\n"
		               "The \002{0} ADDIP\002 command associates an IP with a server.\n"
		               " \n"
		               "The \002{0} POOL\002 and \002{0} DEPOOL\002 commands actually add and remove servers to their given zones."),
		               source.command);
		return true;
	}
};

class ModuleDNS : public Module
	, public EventHook<Event::NewServer>
	, public EventHook<Event::ServerQuit>
	, public EventHook<Event::UserConnect>
	, public EventHook<Event::PreUserLogoff>
	, public EventHook<Event::DnsRequest>
{
	DNSZoneType zonetype;
	DNSServerType servertype;
	DNSZoneMembershipType zonemembtype;
	DNSIPType iptype;
	CommandOSDNS commandosdns;

	time_t ttl;
	int user_drop_mark;
	time_t user_drop_time;
	time_t user_drop_readd_time;
	bool remove_split_servers;
	bool readd_connected_servers;

	time_t last_warn;

 public:
	ModuleDNS(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR)
		, zonetype(this)
		, servertype(this)
		, zonemembtype(this)
		, iptype(this)
		, commandosdns(this)
		, last_warn(0)
	{
#if 0
		for (unsigned j = 0; j < dns_servers->size(); ++j)
		{
			DNSServer *s = dns_servers->at(j);
			if (s->Pooled() && Server::Find(s->GetName(), true))
				s->SetActive(true);
		}
#endif
	}

	~ModuleDNS()
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *block = conf->GetModule(this);
		this->ttl = block->Get<time_t>("ttl");
		this->user_drop_mark =  block->Get<int>("user_drop_mark");
		this->user_drop_time = block->Get<time_t>("user_drop_time");
		this->user_drop_readd_time = block->Get<time_t>("user_drop_readd_time");
		this->remove_split_servers = block->Get<bool>("remove_split_servers");
		this->readd_connected_servers = block->Get<bool>("readd_connected_servers");
	}

	void OnNewServer(Server *s) override
	{
		if (s == Me || s->IsJuped())
			return;
		if (!Me->IsSynced() || this->readd_connected_servers)
		{
			DNSServerImpl *dns = DNSServerImpl::Find(s->GetName());
			if (dns && dns->GetPooled() && !dns->Active() && !dns->GetRefs<DNSIP *>(dnsip).empty())
			{
				dns->SetActive(true);
				Log(this) << "Pooling server " << s->GetName();
			}
		}
	}

	void OnServerQuit(Server *s) override
	{
		DNSServerImpl *dns = DNSServerImpl::Find(s->GetName());
		if (remove_split_servers && dns && dns->GetPooled() && dns->Active())
		{
			if (readd_connected_servers)
				dns->SetActive(false); // Will be reactivated when it comes back
			else
				dns->SetPool(false); // Otherwise permanently pull this
			Log(this) << "Depooling delinked server " << s->GetName();
		}
	}

	void OnUserConnect(User *u, bool &exempt) override
	{
		if (!u->Quitting() && u->server)
		{
			DNSServerImpl *s = DNSServerImpl::Find(u->server->GetName());
			/* Check for user limit reached */
			if (s && s->GetPooled() && s->Active() && s->GetLimit() && u->server->users >= s->GetLimit())
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
			DNSServerImpl *s = DNSServerImpl::Find(u->server->GetName());
			if (!s || !s->GetPooled())
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
		const DNS::Question& q = req.questions[0];
		if (q.type != DNS::QUERY_A && q.type != DNS::QUERY_AAAA && q.type != DNS::QUERY_AXFR && q.type != DNS::QUERY_ANY)
			return;

		DNSZone *zone = DNSZoneImpl::Find(q.name);
		size_t answer_size = packet->answers.size();
		if (zone)
		{
			for (DNSServerImpl *s : zone->GetRefs<DNSServerImpl *>(dnsserver))
			{
				if (!s->Active())
					continue;

				for (DNSIP *ip : s->GetRefs<DNSIP *>(dnsip))
				{
					DNS::QueryType q_type = ip->GetIP().find(':') != Anope::string::npos ? DNS::QUERY_AAAA : DNS::QUERY_A;

					if (q.type == DNS::QUERY_AXFR || q.type == DNS::QUERY_ANY || q_type == q.type)
					{
						DNS::ResourceRecord rr(q.name, q_type);
						rr.ttl = this->ttl;
						rr.rdata = ip->GetIP();
						packet->answers.push_back(rr);
					}
				}
			}
		}

		if (packet->answers.size() == answer_size)
		{
			/* Default zone */
			for (DNSServerImpl *s : Serialize::GetObjects<DNSServerImpl *>(dnsserver))
			{
				if (!s->Active())
					continue;

				for (DNSIP *ip : s->GetRefs<DNSIP *>(dnsip))
				{
					DNS::QueryType q_type = ip->GetIP().find(':') != Anope::string::npos ? DNS::QUERY_AAAA : DNS::QUERY_A;

					if (q.type == DNS::QUERY_AXFR || q.type == DNS::QUERY_ANY || q_type == q.type)
					{
						DNS::ResourceRecord rr(q.name, q_type);
						rr.ttl = this->ttl;
						rr.rdata = ip->GetIP();
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
			for (DNSServer *s : Serialize::GetObjects<DNSServer *>(dnsserver))
				for (DNSIP *ip : s->GetRefs<DNSIP *>(dnsip))
				{
					DNS::QueryType q_type = ip->GetIP().find(':') != Anope::string::npos ? DNS::QUERY_AAAA : DNS::QUERY_A;

					if (q.type == DNS::QUERY_AXFR || q.type == DNS::QUERY_ANY || q_type == q.type)
					{
						DNS::ResourceRecord rr(q.name, q_type);
						rr.ttl = this->ttl;
						rr.rdata = ip->GetIP();
						packet->answers.push_back(rr);
					}
				}

			if (packet->answers.size() == answer_size)
			{
				Log(this) << "Error! There are no servers with any IPs of type " << q.type;
				/* Send back an empty answer anyway */
			}
		}
	}
};

MODULE_INIT(ModuleDNS)
