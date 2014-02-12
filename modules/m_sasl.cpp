/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/sasl.h"
#include "modules/ns_cert.h"

class Plain : public SASL::Mechanism
{
	class IdentifyRequest : public ::IdentifyRequest
	{
		Anope::string uid;

	 public:
		IdentifyRequest(Module *m, const Anope::string &id, const Anope::string &acc, const Anope::string &pass) : ::IdentifyRequest(m, acc, pass), uid(id) { }

		void OnSuccess() anope_override
		{
			if (!sasl)
				return;

			NickAlias *na = NickAlias::Find(GetAccount());
			if (!na)
				return OnFail();

			SASL::Session *s = sasl->GetSession(uid);
			if (s)
				sasl->Succeed(s, na->nc);
		}

		void OnFail() anope_override
		{
			if (!sasl)
				return;

			SASL::Session *s = sasl->GetSession(uid);
			if (s)
				sasl->Fail(s);

			Log(Config->GetClient("NickServ")) << "A user failed to identify for account " << this->GetAccount() << " using SASL";
		}
	};

 public:
	Plain(Module *o) : SASL::Mechanism(o, "PLAIN") { }

	void ProcessMessage(SASL::Session *sess, const SASL::Message &m) anope_override
	{
		if (m.type == "S")
		{
			sasl->SendMessage(sess, "C", "+");
		}
		else if (m.type == "C")
		{
			Anope::string decoded;
			Anope::B64Decode(m.data, decoded);

			size_t p = decoded.find('\0');
			if (p == Anope::string::npos)
				return;
			decoded = decoded.substr(p + 1);

			p = decoded.find('\0');
			if (p == Anope::string::npos)
				return;

			Anope::string acc = decoded.substr(0, p),
				pass = decoded.substr(p + 1);

			if (acc.empty() || pass.empty())
				return;

			IdentifyRequest *req = new IdentifyRequest(this->owner, m.source, acc, pass);
			FOREACH_MOD(OnCheckAuthentication, (NULL, req));
			req->Dispatch();
		}
	}
};

class External : public SASL::Mechanism
{
	struct Session : SASL::Session
	{
		Anope::string cert;

		Session(Mechanism *m, const Anope::string &u) : SASL::Session(m, u) { }
	};

 public:
	External(Module *o) : SASL::Mechanism(o, "EXTERNAL")
	{
		if (!IRCD || !IRCD->CanCertFP)
			throw ModuleException("No CertFP");
	}

	SASL::Session* CreateSession(const Anope::string &uid) anope_override
	{
		return new Session(this, uid);
	}

	void ProcessMessage(SASL::Session *sess, const SASL::Message &m) anope_override
	{
		Session *mysess = anope_dynamic_static_cast<Session *>(sess);

		if (m.type == "S")
		{
			mysess->cert = m.ext;

			sasl->SendMessage(sess, "C", "+");
		}
		else if (m.type == "C")
		{
			Anope::string account;
			Anope::B64Decode(m.data, account);

			NickAlias *na = NickAlias::Find(account);
			if (!na)
			{
				sasl->Fail(sess);
				return;
			}

			NSCertList *cl = na->nc->GetExt<NSCertList>("certificates");
			if (cl == NULL || !cl->FindCert(mysess->cert))
			{
				sasl->Fail(sess);
				return;
			}

			sasl->Succeed(sess, na->nc);
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
		for (std::map<Anope::string, SASL::Session *>::iterator it = sessions.begin(); it != sessions.end();)
			delete it->second;
	}

	void ProcessMessage(const SASL::Message &m) anope_override
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

		SASL::Session* &session = sessions[m.source];

		if (m.type == "S")
		{
			ServiceReference<SASL::Mechanism> mech("SASL::Mechanism", m.data);
			if (!mech)
			{
				SASL::Session tmp(NULL, m.source);

				sasl->SendMechs(&tmp);
				sasl->Fail(&tmp);
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

	Anope::string GetAgent() anope_override
	{
		Anope::string agent = Config->GetModule(Service::owner)->Get<Anope::string>("agent", "NickServ");
		BotInfo *bi = Config->GetClient(agent);
		if (bi)
			agent = bi->GetUID();
		return agent;
	}

	SASL::Session* GetSession(const Anope::string &uid) anope_override
	{
		std::map<Anope::string, SASL::Session *>::iterator it = sessions.find(uid);
		if (it != sessions.end())
			return it->second;
		return NULL;
	}

	void SendMessage(SASL::Session *session, const Anope::string &mtype, const Anope::string &data) anope_override
	{
		SASL::Message msg;
		msg.source = this->GetAgent();
		msg.target = session->uid;
		msg.type = mtype;
		msg.data = data;

		IRCD->SendSASLMessage(msg);
	}

	void Succeed(SASL::Session *session, NickCore *nc) anope_override
	{
		IRCD->SendSVSLogin(session->uid, nc->display);
		this->SendMessage(session, "D", "S");
	}

	void Fail(SASL::Session *session) anope_override
	{
		this->SendMessage(session, "D", "F");
	}

	void SendMechs(SASL::Session *session) anope_override
	{
		std::vector<Anope::string> mechs = Service::GetServiceKeys("SASL::Mechanism");
		Anope::string buf;
		for (unsigned j = 0; j < mechs.size(); ++j)
			buf += "," + mechs[j];

		this->SendMessage(session, "M", buf.empty() ? "" : buf.substr(1));
	}

	void Tick(time_t) anope_override
	{
		for (std::map<Anope::string, SASL::Session *>::iterator it = sessions.begin(); it != sessions.end();)
		{
			Anope::string key = it->first;
			SASL::Session *s = it->second;
			++it;

			if (!s || !s->mech || s->created + 60 < Anope::CurTime)
			{
				delete s;
				sessions.erase(key);
			}
		}
	}
};

class ModuleSASL : public Module
{
	SASLService sasl;

	Plain plain;
	External *external;

 public:
	ModuleSASL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		sasl(this), plain(this), external(NULL)
	{
		try
		{
			external = new External(this);
		}
		catch (ModuleException &) { }
	}

	~ModuleSASL()
	{
		delete external;
	}
};

MODULE_INIT(ModuleSASL)
