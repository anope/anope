/*
 * Anope IRC Services
 *
 * Copyright (C) 2014-2016 Anope Team <team@anope.org>
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
#include "modules/sasl.h"
#include "modules/nickserv/cert.h"

using namespace SASL;

class Plain : public Mechanism
{
 public:
	Plain(SASL::Service *s, Module *o) : Mechanism(s, o, "PLAIN") { }

	void ProcessMessage(Session *sess, const SASL::Message &m) override
	{
		if (m.type == "S")
		{
			GetService()->SendMessage(sess, "C", "+");
		}
		else if (m.type == "C")
		{
			Anope::string decoded;
			Anope::B64Decode(m.data, decoded);

			size_t p = decoded.find('\0');
			if (p == Anope::string::npos)
			{
				GetService()->Fail(sess);
				delete sess;
				return;
			}
			decoded = decoded.substr(p + 1);

			p = decoded.find('\0');
			if (p == Anope::string::npos)
			{
				GetService()->Fail(sess);
				delete sess;
				return;
			}

			Anope::string acc = decoded.substr(0, p),
				pass = decoded.substr(p + 1);

			if (!NickServ::service || acc.empty() || pass.empty() || !IRCD->IsNickValid(acc) || pass.find_first_of("\r\n") != Anope::string::npos)
			{
				GetService()->Fail(sess);
				delete sess;
				return;
			}

			NickServ::IdentifyRequest *req = NickServ::service->CreateIdentifyRequest(new IdentifyRequestListener(GetService(), m.source), this->GetOwner(), acc, pass);
			EventManager::Get()->Dispatch(&Event::CheckAuthentication::OnCheckAuthentication, nullptr, req);
			req->Dispatch();
		}
	}
};

class External : public Mechanism
{
	ServiceReference<CertService> certs;

	struct Session : SASL::Session
	{
		Anope::string cert;

		Session(SASL::Service *s, Mechanism *m, const Anope::string &u) : SASL::Session(s, m, u) { }
	};

 public:
	External(SASL::Service *s, Module *o) : Mechanism(s, o, "EXTERNAL")
		, certs("certs")
	{
		if (!IRCD || !IRCD->CanCertFP)
			throw ModuleException("No CertFP");
	}

	Session* CreateSession(const Anope::string &uid) override
	{
		return new Session(this->GetService(), this, uid);
	}

	void ProcessMessage(SASL::Session *sess, const SASL::Message &m) override
	{
		Session *mysess = anope_dynamic_static_cast<Session *>(sess);

		if (m.type == "S")
		{
			mysess->cert = m.ext;

			GetService()->SendMessage(sess, "C", "+");
		}
		else if (m.type == "C")
		{
			if (!certs)
			{
				GetService()->Fail(sess);
				delete sess;
				return;
			}

			NickServ::Account *nc = certs->FindAccountFromCert(mysess->cert);
			if (!nc || nc->HasFieldS("NS_SUSPENDED"))
			{
				Log(Config->GetClient("NickServ"), "sasl") << "A user failed to identify using certificate " << mysess->cert << " using SASL EXTERNAL";
				GetService()->Fail(sess);
				delete sess;
				return;
			}

			Log(Config->GetClient("NickServ"), "sasl") << "A user identified to account " << nc->GetDisplay() << " using SASL EXTERNAL";
			GetService()->Succeed(sess, nc);
			delete sess;
		}
	}
};

class SASLService : public SASL::Service, public Timer
{
	std::map<Anope::string, SASL::Session *> sessions;

 public:
	SASLService(Module *o) : SASL::Service(o), Timer(o, 60, Anope::CurTime, true) { }

	~SASLService()
	{
		for (std::map<Anope::string, Session *>::iterator it = sessions.begin(); it != sessions.end(); it++)
			delete it->second;
	}

	void ProcessMessage(const SASL::Message &m) override
	{
		if (m.target != "*")
		{
			Server *s = Server::Find(m.target);
			if (s != Me)
			{
				User *u = User::Find(m.target);
				if (!u || u->server != Me)
					return;
			}
		}

		Session* &session = sessions[m.source];

		if (m.type == "S")
		{
			ServiceReference<Mechanism> mech(m.data);
			if (!mech)
			{
				Session tmp(this, NULL, m.source);

				this->SendMechs(&tmp);
				this->Fail(&tmp);
				return;
			}

			if (!session)
				session = mech->CreateSession(m.source);
		}
		else if (m.type == "D")
		{
			delete session;
			sessions.erase(m.source);
			return;
		}

		if (session && session->mech)
			session->mech->ProcessMessage(session, m);
	}

	Anope::string GetAgent() override
	{
		Anope::string agent = Config->GetModule(Service::GetOwner())->Get<Anope::string>("agent", "NickServ");
		ServiceBot *bi = Config->GetClient(agent);
		if (bi)
			agent = bi->GetUID();
		return agent;
	}

	Session* GetSession(const Anope::string &uid) override
	{
		std::map<Anope::string, Session *>::iterator it = sessions.find(uid);
		if (it != sessions.end())
			return it->second;
		return NULL;
	}

	void RemoveSession(Session *sess) override
	{
		sessions.erase(sess->uid);
	}

	void DeleteSessions(Mechanism *mech, bool da) override
	{
		for (std::map<Anope::string, Session *>::iterator it = sessions.begin(); it != sessions.end();)
		{
			std::map<Anope::string, Session *>::iterator del = it++;
			if (*del->second->mech == mech)
			{
				if (da)
					this->SendMessage(del->second, "D", "A");
				delete del->second;
			}
		}
	}

	void SendMessage(Session *session, const Anope::string &mtype, const Anope::string &data) override
	{
		SASL::Message msg;
		msg.source = this->GetAgent();
		msg.target = session->uid;
		msg.type = mtype;
		msg.data = data;

		IRCD->SendSASLMessage(msg);
	}

	void Succeed(Session *session, NickServ::Account *nc) override
	{
		IRCD->SendSVSLogin(session->uid, nc->GetDisplay());
		this->SendMessage(session, "D", "S");
	}

	void Fail(Session *session) override
	{
		this->SendMessage(session, "D", "F");
	}

	void SendMechs(Session *session) override
	{
		std::vector<Mechanism *> mechs = ServiceManager::Get()->FindServices<Mechanism *>();

		Anope::string buf;
		for (unsigned j = 0; j < mechs.size(); ++j)
			buf += "," + mechs[j]->GetName();

		this->SendMessage(session, "M", buf.empty() ? "" : buf.substr(1));
	}

	void Tick(time_t) override
	{
		for (std::map<Anope::string, Session *>::iterator it = sessions.begin(); it != sessions.end();)
		{
			Anope::string key = it->first;
			Session *s = it->second;
			++it;

			if (!s || !s->mech || s->created + 60 < Anope::CurTime)
			{
				delete s;
				sessions.erase(key);
			}
		}
	}
};

void IdentifyRequestListener::OnSuccess(NickServ::IdentifyRequest *req)
{
	NickServ::Nick *na = NickServ::FindNick(req->GetAccount());
	if (!na || na->GetAccount()->HasFieldS("NS_SUSPENDED"))
		return OnFail(req);

	unsigned int maxlogins = Config->GetModule("ns_identify")->Get<unsigned int>("maxlogins");
	if (maxlogins && na->GetAccount()->users.size() >= maxlogins)
		return OnFail(req);

	Session *s = service->GetSession(uid);
	if (s)
	{
		Log(Config->GetClient("NickServ"), "sasl") << "A user identified to account " << req->GetAccount() << " using SASL";
		service->Succeed(s, na->GetAccount());
		delete s;
	}
}

void IdentifyRequestListener::OnFail(NickServ::IdentifyRequest *req)
{
	Session *s = service->GetSession(uid);
	if (s)
	{
		service->Fail(s);
		delete s;
	}

	Anope::string accountstatus;
	NickServ::Nick *na = NickServ::FindNick(req->GetAccount());
	if (!na)
		accountstatus = "nonexistent ";
	else if (na->GetAccount()->HasFieldS("NS_SUSPENDED"))
		accountstatus = "suspended ";

	Log(Config->GetClient("NickServ"), "sasl") << "A user failed to identify for " << accountstatus << "account " << req->GetAccount() << " using SASL";
}

class ModuleSASL : public Module
{
	SASLService sasl;

	Plain plain;
	External *external = nullptr;

 public:
	ModuleSASL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, sasl(this)
		, plain(&sasl, this)
	{
		try
		{
			external = new External(&sasl, this);
		}
		catch (ModuleException &) { }
	}

	~ModuleSASL()
	{
		delete external;
	}
};

MODULE_INIT(ModuleSASL)
