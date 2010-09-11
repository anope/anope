/*
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

struct Blacklist
{
	Anope::string name;
	time_t bantime;
	Anope::string reason;

	Blacklist(const Anope::string &n, time_t b, const Anope::string &r) : name(n), bantime(b), reason(r) { }
};

class DNSBLResolver : public DNSRequest
{
	dynamic_reference<User> user;
	Blacklist blacklist;
 public:
	DNSBLResolver(User *u, const Blacklist &b, const Anope::string &host) : DNSRequest(host, DNS_QUERY_A, true), user(u), blacklist(b) { }

	void OnLookupComplete(const DNSRecord *)
	{
		if (!user || !SGLine)
			return;

		Anope::string reason = this->blacklist.reason;
		reason = reason.replace_all_ci("%i", user->ip.addr());
		reason = reason.replace_all_ci("%h", user->host);

		XLine *x = SGLine->Add(NULL, NULL, Anope::string("*@") + user->host, Anope::CurTime + this->blacklist.bantime, reason);
		if (x)
		{
			static Command command_akill("AKILL", 0);
			command_akill.service = OperServ;
			Log(LOG_COMMAND, OperServ, &command_akill) << "for " << user->GetMask() << " (Listed in " << this->blacklist.name << ")";
			/* If AkillOnAdd is disabled send it anyway, noone wants bots around... */
			if (!Config->AkillOnAdd)
				ircdproto->SendAkill(x);
		}
	}
};

class ModuleDNSBL : public Module
{
	std::vector<Blacklist> blacklists;
	bool check_on_connect;
	bool check_on_netburst;

 public:
	ModuleDNSBL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(SUPPORTED);

		OnReload(false);

		Implementation i[] = { I_OnReload, I_OnPreUserConnect };
		ModuleManager::Attach(i, this, 2);
	}

	void OnReload(bool)
	{
		ConfigReader config;

		this->check_on_connect = config.ReadFlag("m_dnsbl", "check_on_connect", "no", 0);
		this->check_on_netburst = config.ReadFlag("m_dnsbl", "check_on_netburst", "no", 0);

		this->blacklists.clear();
		for (int i = 0, num = config.Enumerate("blacklist"); i < num; ++i)
		{
			Anope::string bname = config.ReadValue("blacklist", "name", "", i);
			if (bname.empty())
				continue;
			Anope::string sbantime = config.ReadValue("blacklist", "time", "4h", i);
			time_t bantime = dotime(sbantime);
			if (bantime < 0)
				bantime = 9000;
			Anope::string reason = config.ReadValue("blacklist", "reason", "", i);

			this->blacklists.push_back(Blacklist(bname, bantime, reason));
		}
	}

	EventReturn OnPreUserConnect(User *u)
	{
		if (!this->check_on_connect && !Me->IsSynced())
			return EVENT_CONTINUE;
		if (!this->check_on_netburst && !u->server->IsSynced())
			return EVENT_CONTINUE;
		/* At this time we only support IPv4 */
		if (u->ip.sa.sa_family != AF_INET)
			return EVENT_CONTINUE;

		unsigned long ip = u->ip.sa4.sin_addr.s_addr;
		unsigned long reverse_ip = ((ip & 0xFF) << 24) | ((ip & 0xFF00) << 8) | ((ip & 0xFF0000) >> 8) | ((ip & 0xFF000000) >> 24);

		sockaddrs user_ip;
		user_ip.sa4.sin_family = AF_INET;
		user_ip.sa4.sin_addr.s_addr = reverse_ip;

		for (unsigned i = 0; i < this->blacklists.size(); ++i)
		{
			const Blacklist &b = this->blacklists[i];

			try
			{
				Anope::string dnsbl_host = user_ip.addr() + "." + b.name;
				new DNSBLResolver(u, b, dnsbl_host);
			}
			catch (const SocketException &ex)
			{
				Log() << "Resolver: " << ex.GetReason();
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ModuleDNSBL)

