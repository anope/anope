// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"
#include "modules/nickserv/sasl.h"

class SASLIdentifyRequest final
	: public IdentifyRequest
{
private:
	Anope::string uid;
	Anope::string hostname;

	inline Anope::string GetUserInfo()
	{
		auto *u = User::Find(uid);
		if (u)
			return u->GetMask();
		if (!hostname.empty() && !GetAddress().empty())
			return Anope::Format("%s (%s)", hostname.c_str(), GetAddress().c_str());
		return "A user";
	};

public:
	SASLIdentifyRequest(Module *m, const Anope::string &id, const Anope::string &acc, const Anope::string &pass, const Anope::string &h, const Anope::string &i)
		: IdentifyRequest(m, acc, pass, i)
		, uid(id)
		, hostname(h)
	{
	}

	void OnSuccess(NickAlias *na) override
	{
		if (!SASL::service)
			return;

		NickCore *nc = na->nc;
		if (nc->HasExt("NS_SUSPENDED") || nc->HasExt("UNCONFIRMED"))
			return OnFail();

		auto maxlogins = Config->GetModule("ns_identify").Get<unsigned int>("maxlogins");
		if (maxlogins && nc->users.size() >= maxlogins)
			return OnFail();

		auto *s = SASL::service->GetSession(uid);
		if (s)
		{
			Log(this->GetOwner(), "sasl", Config->GetClient("NickServ")) << GetUserInfo() << " identified to account " << nc->display << " using SASL";
			SASL::service->Succeed(s, nc);
			delete s;
		}
	}

	void OnFail() override
	{
		if (!SASL::service)
			return;

		auto *s = SASL::service->GetSession(uid);
		if (s)
		{
			SASL::service->Fail(s);
			delete s;
		}

		Anope::string accountstatus;
		auto *na = NickAlias::Find(GetAccount());
		if (!na)
			accountstatus = "nonexistent ";
		else if (na->nc->HasExt("NS_SUSPENDED"))
			accountstatus = "suspended ";
		else if (na->nc->HasExt("UNCONFIRMED"))
			accountstatus = "unconfirmed ";

		Log(this->GetOwner(), "sasl", Config->GetClient("NickServ")) << GetUserInfo() << " failed to identify for " << accountstatus << "account " << this->GetAccount() << " using SASL";
	}
};

class Plain final
	: public SASL::Mechanism
{
public:
	Plain(Module *o)
		: SASL::Mechanism(o, "PLAIN")
	{
	}

	bool ProcessMessage(SASL::Session *sess, const SASL::Message &m) override
	{
		if (m.type == "S")
		{
			SASL::service->SendMessage(sess, "C", "+");
		}
		else if (m.type == "C")
		{
			// message = [authzid] UTF8NUL authcid UTF8NUL passwd
			const auto message = Anope::B64Decode(m.data[0]);

			const auto zcsep = message.find('\0');
			if (zcsep == Anope::string::npos)
				return false;

			const auto cpsep = message.find('\0', zcsep + 1);
			if (cpsep == Anope::string::npos)
				return false;

			const auto authzid = message.substr(0, zcsep);
			const auto authcid = message.substr(zcsep + 1, cpsep - zcsep - 1);

			// We don't support having an authcid that is different to the authzid.
			if (!authzid.empty() && authzid != authcid)
				return false;

			const auto passwd = message.substr(cpsep + 1);
			if (authcid.empty() || passwd.empty() || !IRCD->IsNickValid(authcid) || passwd.find_first_of("\r\n\0") != Anope::string::npos)
				return false;

			auto *req = new SASLIdentifyRequest(this->owner, m.source, authcid, passwd, sess->hostname, sess->ip);
			FOREACH_MOD(OnCheckAuthentication, (NULL, req));
			req->Dispatch();
		}
		return true;
	}
};

class ModuleSASLPlain final
	: public Module
{
private:
	Plain plain;

public:
	ModuleSASLPlain(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, plain(this)
	{
		if (!SASL::protocol_interface)
			throw ModuleException("Your IRCd does not support SASL");
	}
};

MODULE_INIT(ModuleSASLPlain)
