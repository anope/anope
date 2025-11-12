// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
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

class SASLService final
	: public SASL::Service
	, public Timer
{
private:
	Anope::map<std::pair<time_t, unsigned short>> badpasswords;
	Anope::map<SASL::Session *> sessions;

public:
	unsigned badpasslimit;
	unsigned badpasstimeout;

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

		auto *session = GetSession(m.source);

		if (m.type == "S")
		{
			ServiceReference<SASL::Mechanism> mech("SASL::Mechanism", m.data[0]);
			if (!mech)
			{
				SASL::Session tmp(NULL, m.source);

				this->SendMechs(&tmp);
				this->Fail(&tmp);
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
				session = new SASL::Session(NULL, m.source);
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

	Anope::string GetAgent()
	{
		Anope::string agent = Config->GetModule(Service::owner).Get<Anope::string>("agent", "NickServ");
		BotInfo *bi = Config->GetClient(agent);
		if (bi)
			agent = bi->GetUID();
		return agent;
	}

	SASL::Session *GetSession(const Anope::string &uid) override
	{
		auto it = sessions.find(uid);
		if (it != sessions.end())
			return it->second;
		return NULL;
	}

	void RemoveSession(SASL::Session *sess) override
	{
		sessions.erase(sess->uid);
	}

	void DeleteSessions(SASL::Mechanism *mech, bool da) override
	{
		for (auto it = sessions.begin(); it != sessions.end();)
		{
			auto del = it++;
			if (*del->second->mech == mech)
			{
				if (da)
					this->SendMessage(del->second, "D", "A");
				delete del->second;
			}
		}
	}

	void SendMessage(SASL::Session *session, const Anope::string &mtype, const Anope::string &data) override
	{
		SASL::Message msg;
		msg.source = this->GetAgent();
		msg.target = session->uid;
		msg.type = mtype;
		msg.data.push_back(data);

		SASL::protocol_interface->SendSASLMessage(msg);
	}

	void Succeed(SASL::Session *session, NickCore *nc) override
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
			SASL::protocol_interface->SendSVSLogin(session->uid, na);
		}
		this->SendMessage(session, "D", "S");
	}

	void Fail(SASL::Session *session) override
	{
		this->SendMessage(session, "D", "F");

		auto *u = User::Find(session->uid);
		if (u)
		{
			u->BadPassword();
			return;
		}

		if (!badpasslimit)
			return;

		auto it = badpasswords.find(session->uid);
		if (it == badpasswords.end())
			it = badpasswords.emplace(session->uid, std::make_pair(0, 0)).first;
		auto &[invalid_pw_time, invalid_pw_count] = it->second;

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

	void SendMechs(SASL::Session *session)
	{
		std::vector<Anope::string> mechs = Service::GetServiceKeys("SASL::Mechanism");
		Anope::string buf;
		for (const auto &mech : mechs)
			buf += "," + mech;

		this->SendMessage(session, "M", buf.empty() ? "" : buf.substr(1));
	}

	void Tick() override
	{
		for (auto it = badpasswords.begin(); it != badpasswords.end(); )
		{
			if (it->second.first + badpasstimeout < Anope::CurTime)
				it = badpasswords.erase(it);
			else
				it++;
		}

		for (auto it = sessions.begin(); it != sessions.end(); )
		{
			const auto [uid, sess] = *it++;
			if (!sess || sess->created + 60 < Anope::CurTime)
			{
				delete sess;
				sessions.erase(uid);
			}
		}
	}
};

class ModuleSASL final
	: public Module
{
private:
	SASLService sasl;
	std::vector<Anope::string> mechs;

	void CheckMechs()
	{
		std::vector<Anope::string> newmechs = ::Service::GetServiceKeys("SASL::Mechanism");
		if (newmechs == mechs || !SASL::protocol_interface)
			return;

		mechs = newmechs;

		// If we are connected to the network then broadcast the mechlist.
		if (Me && Me->IsSynced())
			SASL::protocol_interface->SendSASLMechanisms(mechs);
	}

public:
	ModuleSASL(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, sasl(this)
	{
		if (!SASL::protocol_interface)
			throw ModuleException("Your IRCd does not support SASL");
	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &modconf = conf.GetModule(this);
		const auto &options = conf.GetBlock("options");

		sasl.badpasslimit = modconf.Get<unsigned>("badpasslimit");
		if(!sasl.badpasslimit)
			sasl.badpasslimit = options.Get<unsigned>("badpasslimit");

		sasl.badpasstimeout = modconf.Get<time_t>("badpasstimeout");
		if (!sasl.badpasstimeout)
			sasl.badpasstimeout = options.Get<unsigned>("badpasstimeout");
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
		if (SASL::protocol_interface)
			SASL::protocol_interface->SendSASLMechanisms(mechs);
	}
};

MODULE_INIT(ModuleSASL)
