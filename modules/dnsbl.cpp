/*
 * Anope IRC Services
 *
 * Copyright (C) 2013-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/dns.h"

using namespace DNS;

struct Blacklist
{
	struct Reply
	{
		int code;
		Anope::string reason;
		bool allow_account;

		Reply() : code(0), allow_account(false) { }
	};

	Anope::string name;
	time_t bantime;
	Anope::string reason;
	std::vector<Reply> replies;

	Blacklist() : bantime(0) { }

	Reply *Find(int code)
	{
		for (unsigned int i = 0; i < replies.size(); ++i)
			if (replies[i].code == code)
				return &replies[i];
		return NULL;
	}
};

class DNSBLResolver : public Request
{
	ServiceReference<XLineManager> &akills;
	Reference<User> user;
	Blacklist blacklist;
	bool add_to_akill;

 public:
	DNSBLResolver(ServiceReference<XLineManager> &a, Module *c, DNS::Manager *manager, User *u, const Blacklist &b, const Anope::string &host, bool add_akill) : Request(manager, c, host, QUERY_A, true), akills(a), user(u), blacklist(b), add_to_akill(add_akill) { }

	void OnLookupComplete(const Query *record) override
	{
		if (!user || user->Quitting())
			return;

		const ResourceRecord &ans_record = record->answers[0];
		// Replies should be in 127.0.0.0/24
		if (ans_record.rdata.find("127.0.0.") != 0)
			return;

		sockaddrs sresult;
		sresult.pton(AF_INET, ans_record.rdata);
		int result = sresult.sa4.sin_addr.s_addr >> 24;

		Blacklist::Reply *reply = blacklist.Find(result);
		if (!blacklist.replies.empty() && !reply)
			return;

		if (reply && reply->allow_account && user->Account())
			return;

		Anope::string reason = this->blacklist.reason, addr = user->ip.addr();
		reason = reason.replace_all_cs("%n", user->nick);
		reason = reason.replace_all_cs("%u", user->GetIdent());
		reason = reason.replace_all_cs("%g", user->realname);
		reason = reason.replace_all_cs("%h", user->host);
		reason = reason.replace_all_cs("%i", addr);
		reason = reason.replace_all_cs("%r", reply ? reply->reason : "");
		reason = reason.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<Anope::string>("networkname"));

		ServiceBot *OperServ = Config->GetClient("OperServ");
		Log(creator, "dnsbl", OperServ) << user->GetMask() << " (" << addr << ") appears in " << this->blacklist.name;

		XLine *x = Serialize::New<XLine *>();
		x->SetMask("*@" + addr);
		x->SetBy(OperServ ? OperServ->nick : "m_dnsbl");
		x->SetCreated(Anope::CurTime);
		x->SetExpires(Anope::CurTime + this->blacklist.bantime);
		x->SetReason(reason);
		x->SetID(XLineManager::GenerateUID());

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
	, public EventHook<Event::UserConnect>
{
	ServiceReference<DNS::Manager> manager;
	std::vector<Blacklist> blacklists;
	std::set<Anope::string> exempts;
	bool check_on_connect;
	bool check_on_netburst;
	bool add_to_akill;
	ServiceReference<XLineManager> akill;

 public:
	ModuleDNSBL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR | EXTRA)
		, EventHook<Event::UserConnect>(this)
		, akill("sgline")
	{

	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *block = conf->GetModule(this);
		this->check_on_connect = block->Get<bool>("check_on_connect");
		this->check_on_netburst = block->Get<bool>("check_on_netburst");
		this->add_to_akill = block->Get<bool>("add_to_akill", "yes");

		this->blacklists.clear();
		for (int i = 0; i < block->CountBlock("blacklist"); ++i)
		{
			Configuration::Block *bl = block->GetBlock("blacklist", i);
			Blacklist blacklist;

			blacklist.name = bl->Get<Anope::string>("name");
			if (blacklist.name.empty())
				continue;
			blacklist.bantime = bl->Get<time_t>("time", "4h");
			blacklist.reason = bl->Get<Anope::string>("reason");

			for (int j = 0; j < bl->CountBlock("reply"); ++j)
			{
				Configuration::Block *reply = bl->GetBlock("reply", j);
				Blacklist::Reply r;

				r.code = reply->Get<int>("code");
				r.reason = reply->Get<Anope::string>("reason");
				r.allow_account = reply->Get<bool>("allow_account");

				blacklist.replies.push_back(r);
			}

			this->blacklists.push_back(blacklist);
		}

		this->exempts.clear();
		for (int i = 0; i < block->CountBlock("exempt"); ++i)
			this->exempts.insert(block->Get<Anope::string>("ip"));
	}

	void OnUserConnect(User *user, bool &exempt) override
	{
		if (exempt || user->Quitting() || (!this->check_on_connect && !Me->IsSynced()) || !manager)
			return;

		if (!this->check_on_netburst && !user->server->IsSynced())
			return;

		/* At this time we only support IPv4 */
		if (!user->ip.valid() || user->ip.sa.sa_family != AF_INET)
			/* User doesn't have a valid IPv4 IP (ipv6/spoof/etc) */
			return;

		if (this->exempts.count(user->ip.addr()))
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
				res = new DNSBLResolver(akill, this, manager, user, b, dnsbl_host, this->add_to_akill);
				manager->Process(res);
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

