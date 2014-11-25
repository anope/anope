/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/dns.h"

using namespace DNS;

static ServiceReference<XLineManager> akills("XLineManager", "xlinemanager/sgline");
static ServiceReference<Manager> dnsmanager("DNS::Manager", "dns/manager");

struct Blacklist
{
	Anope::string name;
	time_t bantime;
	Anope::string reason;
	std::map<int, Anope::string> replies;

	Blacklist(const Anope::string &n, time_t b, const Anope::string &r, const std::map<int, Anope::string> &re) : name(n), bantime(b), reason(r), replies(re) { }
};

class DNSBLResolver : public Request
{
	Reference<User> user;
	Blacklist blacklist;
	bool add_to_akill;

 public:
	DNSBLResolver(Module *c, User *u, const Blacklist &b, const Anope::string &host, bool add_akill) : Request(dnsmanager, c, host, QUERY_A, true), user(u), blacklist(b), add_to_akill(add_akill) { }

	void OnLookupComplete(const Query *record) anope_override
	{
		if (!user || user->Quitting())
			return;

		const ResourceRecord &ans_record = record->answers[0];
		// Replies should be in 127.0.0.0/24
		if (ans_record.rdata.find("127.0.0.") != 0)
			return;

		Anope::string record_reason;
		if (!this->blacklist.replies.empty())
		{
			sockaddrs sresult;
			sresult.pton(AF_INET, ans_record.rdata);
			int result = sresult.sa4.sin_addr.s_addr >> 24;

			if (!this->blacklist.replies.count(result))
				return;
			record_reason = this->blacklist.replies[result];
		}

		Anope::string reason = this->blacklist.reason, addr = user->ip.addr();
		reason = reason.replace_all_cs("%n", user->nick);
		reason = reason.replace_all_cs("%u", user->GetIdent());
		reason = reason.replace_all_cs("%g", user->realname);
		reason = reason.replace_all_cs("%h", user->host);
		reason = reason.replace_all_cs("%i", addr);
		reason = reason.replace_all_cs("%r", record_reason);
		reason = reason.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<const Anope::string>("networkname"));

		BotInfo *OperServ = Config->GetClient("OperServ");
		Log(creator, "dnsbl", OperServ) << user->GetMask() << " (" << addr << ") appears in " << this->blacklist.name;
		XLine *x = new XLine("*@" + addr, OperServ ? OperServ->nick : "m_dnsbl", Anope::CurTime + this->blacklist.bantime, reason, XLineManager::GenerateUID());
		if (this->add_to_akill && akills)
		{
			akills->AddXLine(x);
			akills->Send(NULL, x);
		}
		else
		{
			IRCD->SendAkill(NULL, x);
			delete x;
		}
	}
};

class ModuleDNSBL : public Module
{
	std::vector<Blacklist> blacklists;
	bool check_on_connect;
	bool check_on_netburst;
	bool add_to_akill;

 public:
	ModuleDNSBL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR | EXTRA)
	{

	}

	void OnReload(Configuration::Conf *conf) anope_override
	{
		Configuration::Block *block = conf->GetModule(this);
		this->check_on_connect = block->Get<bool>("check_on_connect");
		this->check_on_netburst = block->Get<bool>("check_on_netburst");
		this->add_to_akill = block->Get<bool>("add_to_akill", "yes");

		this->blacklists.clear();
		for (int i = 0, num = block->CountBlock("blacklist"); i < num; ++i)
		{
			Configuration::Block *bl = block->GetBlock("blacklist", i);

			Anope::string bname = bl->Get<const Anope::string>("name");
			if (bname.empty())
				continue;
			time_t bantime = bl->Get<time_t>("time", "4h");
			Anope::string reason = bl->Get<const Anope::string>("reason");
			std::map<int, Anope::string> replies;
			for (int j = 0; j < 256; ++j)
			{
				Anope::string k = bl->Get<const Anope::string>(stringify(j));
				if (!k.empty())
					replies[j] = k;
			}

			this->blacklists.push_back(Blacklist(bname, bantime, reason, replies));
		}
	}

	void OnUserConnect(User *user, bool &exempt) anope_override
	{
		if (exempt || user->Quitting() || (!this->check_on_connect && !Me->IsSynced()) || !dnsmanager)
			return;

		if (!this->check_on_netburst && !user->server->IsSynced())
			return;

		/* At this time we only support IPv4 */
		if (!user->ip.valid() || user->ip.sa.sa_family != AF_INET)
			/* User doesn't have a valid IPv4 IP (ipv6/spoof/etc) */
			return;

		const unsigned long &ip = user->ip.sa4.sin_addr.s_addr;
		unsigned long reverse_ip = (ip << 24) | ((ip & 0xFF00) << 8) | ((ip & 0xFF0000) >> 8) | (ip >> 24);

		sockaddrs reverse = user->ip;
		reverse.sa4.sin_addr.s_addr = reverse_ip;

		for (unsigned i = 0; i < this->blacklists.size(); ++i)
		{
			const Blacklist &b = this->blacklists[i];

			Anope::string dnsbl_host = reverse.addr() + "." + b.name;
			DNSBLResolver *res = NULL;
			try
			{
				res = new DNSBLResolver(this, user, b, dnsbl_host, this->add_to_akill);
				dnsmanager->Process(res);
			}
			catch (const SocketException &ex)
			{
				delete res;
				Log(this) << ex.GetReason();
			}
		}
	}
};

MODULE_INIT(ModuleDNSBL)

