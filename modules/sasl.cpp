/*
 *
 * (C) 2014-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/sasl.h"
#include "modules/ns_cert.h"

using namespace SASL;

class Plain final
	: public Mechanism
{
public:
	Plain(Module *o) : Mechanism(o, "PLAIN") { }

	bool ProcessMessage(Session *sess, const SASL::Message &m) override
	{
		if (m.type == "S")
		{
			sasl->SendMessage(sess, "C", "+");
		}
		else if (m.type == "C")
		{
			// message = [authzid] UTF8NUL authcid UTF8NUL passwd
			Anope::string message;
			Anope::B64Decode(m.data[0], message);

			size_t zcsep = message.find('\0');
			if (zcsep == Anope::string::npos)
				return false;

			size_t cpsep = message.find('\0', zcsep + 1);
			if (cpsep == Anope::string::npos)
				return false;

			Anope::string authzid = message.substr(0, zcsep);
			Anope::string authcid = message.substr(zcsep + 1, cpsep - zcsep - 1);

			// We don't support having an authcid that is different to the authzid.
			if (!authzid.empty() && authzid != authcid)
				return false;

			Anope::string passwd = message.substr(cpsep + 1);

			if (authcid.empty() || passwd.empty() || !IRCD->IsNickValid(authcid) || passwd.find_first_of("\r\n\0") != Anope::string::npos)
				return false;

			SASL::IdentifyRequest *req = new SASL::IdentifyRequest(this->owner, m.source, authcid, passwd, sess->hostname, sess->ip);
			FOREACH_MOD(OnCheckAuthentication, (NULL, req));
			req->Dispatch();
		}
		return true;
	}
};

class External final
	: public Mechanism
{
	ServiceReference<CertService> certs;

	struct Session final
		: SASL::Session
	{
		std::vector<Anope::string> certs;

		Session(Mechanism *m, const Anope::string &u) : SASL::Session(m, u) { }
	};

public:
	External(Module *o) : Mechanism(o, "EXTERNAL"), certs("CertService", "certs")
	{
		if (!IRCD || !IRCD->CanCertFP)
			throw ModuleException("No CertFP");
	}

	Session *CreateSession(const Anope::string &uid) override
	{
		return new Session(this, uid);
	}

	bool ProcessMessage(SASL::Session *sess, const SASL::Message &m) override
	{
		Session *mysess = anope_dynamic_static_cast<Session *>(sess);

		if (m.type == "S")
		{
			mysess->certs.assign(m.data.begin() + 1, m.data.end());

			sasl->SendMessage(sess, "C", "+");
		}
		else if (m.type == "C")
		{
			if (!certs || mysess->certs.empty())
				return false;

			Anope::string user = "A user";
			auto *u = User::Find(sess->uid);
			if (u)
				user = u->GetMask();
			else if (!mysess->hostname.empty() && !mysess->ip.empty())
				user = mysess->hostname + " (" + mysess->ip + ")";

			for (auto it = mysess->certs.begin(); it != mysess->certs.end(); ++it)
			{
				auto *nc = certs->FindAccountFromCert(*it);
				if (nc && !nc->HasExt("NS_SUSPENDED") && !nc->HasExt("UNCONFIRMED"))
				{
					// If we are using a fallback cert then upgrade it.
					if (it != mysess->certs.begin())
					{
						auto *cl = nc->GetExt<NSCertList>("certificates");
						if (cl)
							cl->ReplaceCert(*it, mysess->certs[0]);
					}

					Log(this->owner, "sasl", Config->GetClient("NickServ")) << user << " identified to account " << nc->display << " using SASL EXTERNAL";
					sasl->Succeed(sess, nc);
					delete sess;
					return true;
				}
			}

			Log(this->owner, "sasl", Config->GetClient("NickServ")) << user << " failed to identify using certificate " << mysess->certs.front() << " using SASL EXTERNAL";
			return false;
		}
		return true;
	}
};

class Anonymous final
	: public Mechanism
{
public:
	Anonymous(Module *o) : Mechanism(o, "ANONYMOUS") { }

	bool ProcessMessage(Session *sess, const SASL::Message &m) override
	{
		if (m.type == "S")
		{
			sasl->SendMessage(sess, "C", "+");
		}
		else if (m.type == "C")
		{
			Anope::string decoded;
			Anope::B64Decode(m.data[0], decoded);

			Anope::string user = "A user";
			if (!sess->hostname.empty() && !sess->ip.empty())
				user = sess->hostname + " (" + sess->ip + ")";
			if (!decoded.empty())
				user += " [" + decoded + "]";

			Log(this->owner, "sasl", Config->GetClient("NickServ")) << user << " unidentified using SASL ANONYMOUS";
			sasl->Succeed(sess, nullptr);
		}
		return true;
	}
};

class SASLService final
	: public SASL::Service
	, public Timer
{
private:
	Anope::map<std::pair<time_t, unsigned short>> badpasswords;
	Anope::map<SASL::Session *> sessions;

public:
	SASLService(Module *o)
		: SASL::Service(o)
		, Timer(o, 60, true)
		{
		}

	~SASLService() override
	{
		for (const auto &[_, session] : sessions)
			delete session;
	}

	void ProcessMessage(const SASL::Message &m) override
	{
		if (m.data.empty())
			return; // Malformed.

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

		Session *session = GetSession(m.source);

		if (m.type == "S")
		{
			ServiceReference<Mechanism> mech("SASL::Mechanism", m.data[0]);
			if (!mech)
			{
				Session tmp(NULL, m.source);

				sasl->SendMechs(&tmp);
				sasl->Fail(&tmp);
				return;
			}

			Anope::string hostname, ip;
			if (session)
			{
				// Copy over host/ip to mech-specific session
				hostname = session->hostname;
				ip = session->ip;
				delete session;
			}

			session = mech->CreateSession(m.source);
			if (session)
			{
				session->hostname = hostname;
				session->ip = ip;

				sessions[m.source] = session;
			}
		}
		else if (m.type == "D")
		{
			delete session;
			return;
		}
		else if (m.type == "H")
		{
			if (!session)
			{
				session = new Session(NULL, m.source);
				sessions[m.source] = session;
			}
			session->hostname = m.data[0];
			session->ip = m.data.size() > 1 ? m.data[1] : "";
		}

		if (session && session->mech)
		{
			if (!session->mech->ProcessMessage(session, m))
			{
				Fail(session);
				delete session;
			}
		}
	}

	Anope::string GetAgent() override
	{
		Anope::string agent = Config->GetModule(Service::owner)->Get<Anope::string>("agent", "NickServ");
		BotInfo *bi = Config->GetClient(agent);
		if (bi)
			agent = bi->GetUID();
		return agent;
	}

	Session *GetSession(const Anope::string &uid) override
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
		msg.data.push_back(data);

		IRCD->SendSASLMessage(msg);
	}

	void Succeed(Session *session, NickCore *nc) override
	{
		// If the user is already introduced then we log them in now.
		// Otherwise, we send an SVSLOGIN to log them in later.
		User *user = User::Find(session->uid);
		NickAlias *na = nc ? nc->na : nullptr;
		if (user)
		{
			if (na)
				user->Identify(na);
			else
				user->Logout();
		}
		else
		{
			IRCD->SendSVSLogin(session->uid, na);
		}
		this->SendMessage(session, "D", "S");
	}

	void Fail(Session *session) override
	{
		this->SendMessage(session, "D", "F");

		const auto badpasslimit = Config->GetBlock("options")->Get<int>("badpasslimit");
		if (!badpasslimit)
			return;

		auto it = badpasswords.find(session->uid);
		if (it == badpasswords.end())
			it = badpasswords.emplace(session->uid, std::make_pair(0, 0)).first;
		auto &[invalid_pw_time, invalid_pw_count] = it->second;

		const auto badpasstimeout = Config->GetBlock("options")->Get<time_t>("badpasstimeout");
		if (badpasstimeout > 0 && invalid_pw_time > 0 && invalid_pw_time < Anope::CurTime - badpasstimeout)
			invalid_pw_count = 0;

		invalid_pw_count++;
		invalid_pw_time = Anope::CurTime;
		if (invalid_pw_count >= badpasslimit)
		{
			IRCD->SendKill(BotInfo::Find(GetAgent()), session->uid, "Too many invalid passwords");
			badpasswords.erase(it);
		}
	}

	void SendMechs(Session *session) override
	{
		std::vector<Anope::string> mechs = Service::GetServiceKeys("SASL::Mechanism");
		Anope::string buf;
		for (const auto &mech : mechs)
			buf += "," + mech;

		this->SendMessage(session, "M", buf.empty() ? "" : buf.substr(1));
	}

	void Tick() override
	{
		const auto badpasstimeout = Config->GetBlock("options")->Get<time_t>("badpasstimeout");
		for (auto it = badpasswords.begin(); it != badpasswords.end(); )
		{
			if (it->second.first + badpasstimeout < Anope::CurTime)
				it = badpasswords.erase(it);
			else
				it++;
		}

		for (auto it = sessions.begin(); it != sessions.end(); )
		{
			const auto *sess = it->second;
			if (!sess || sess->created + 60 < Anope::CurTime)
			{
				delete sess;
				it = sessions.erase(it);
			}
			else
				it++;
		}
	}
};

class ModuleSASL final
	: public Module
{
	SASLService sasl;

	Anonymous anonymous;
	Plain plain;
	External *external = nullptr;

	std::vector<Anope::string> mechs;

	void CheckMechs()
	{
		std::vector<Anope::string> newmechs = ::Service::GetServiceKeys("SASL::Mechanism");
		if (newmechs == mechs)
			return;

		mechs = newmechs;

		// If we are connected to the network then broadcast the mechlist.
		if (Me && Me->IsSynced())
			IRCD->SendSASLMechanisms(mechs);
	}

public:
	ModuleSASL(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		sasl(this), anonymous(this), plain(this)
	{
		try
		{
			external = new External(this);
			CheckMechs();
		}
		catch (ModuleException &) { }
	}

	~ModuleSASL() override
	{
		delete external;
	}

	void OnModuleLoad(User *, Module *) override
	{
		CheckMechs();
	}

	void OnModuleUnload(User *, Module *) override
	{
		CheckMechs();
	}

	void OnPreUplinkSync(Server *) override
	{
		// We have not yet sent a mechanism list so always do it here.
		IRCD->SendSASLMechanisms(mechs);
	}
};

MODULE_INIT(ModuleSASL)
