/*
 *
 * (C) 2014-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

namespace SASL
{
	struct Message final
	{
		Anope::string source;
		Anope::string target;
		Anope::string type;
		std::vector<Anope::string> data;
	};

	class Mechanism;
	struct Session;

	class Service
		: public ::Service
	{
	public:
		Service(Module *o) : ::Service(o, "SASL::Service", "sasl") { }

		virtual void ProcessMessage(const Message &) = 0;

		virtual Session *GetSession(const Anope::string &uid) = 0;

		virtual void SendMessage(SASL::Session *session, const Anope::string &type, const Anope::string &data) = 0;

		virtual void Succeed(Session *, NickCore *) = 0;
		virtual void Fail(Session *) = 0;
		virtual void DeleteSessions(Mechanism *, bool = false) = 0;
		virtual void RemoveSession(Session *) = 0;
	};

	static ServiceReference<SASL::Service> service("SASL::Service", "sasl");

	struct Session
	{
		time_t created;
		Anope::string uid;
		Anope::string hostname, ip;
		Reference<Mechanism> mech;

		Session(Mechanism *m, const Anope::string &u) : created(Anope::CurTime), uid(u), mech(m) { }

		inline Anope::string GetUserInfo()
		{
			auto *u = User::Find(uid);
			if (u)
				return u->GetMask();
			if (!hostname.empty() && !ip.empty())
				return Anope::printf("%s (%s)", hostname.c_str(), ip.c_str());
			return "A user";
		};

		virtual ~Session()
		{
			if (service)
				service->RemoveSession(this);
		}
	};

	/* PLAIN, EXTERNAL, etc */
	class Mechanism
		: public ::Service
	{
	public:
		Mechanism(Module *o, const Anope::string &sname) : Service(o, "SASL::Mechanism", sname) { }

		virtual Session *CreateSession(const Anope::string &uid) { return new Session(this, uid); }

		virtual bool ProcessMessage(Session *session, const Message &) = 0;

		virtual ~Mechanism()
		{
			if (service)
				service->DeleteSessions(this, true);
		}
	};

	class IdentifyRequest
		: public ::IdentifyRequest
	{
		Anope::string uid;
		Anope::string hostname;

		inline Anope::string GetUserInfo()
		{
			auto *u = User::Find(uid);
			if (u)
				return u->GetMask();
			if (!hostname.empty() && !GetAddress().empty())
				return Anope::printf("%s (%s)", hostname.c_str(), GetAddress().c_str());
			return "A user";
		};

	public:
		IdentifyRequest(Module *m, const Anope::string &id, const Anope::string &acc, const Anope::string &pass, const Anope::string &h, const Anope::string &i)
			: ::IdentifyRequest(m, acc, pass, i)
			, uid(id)
			, hostname(h)
		{
		}

		void OnSuccess() override
		{
			if (!service)
				return;

			NickAlias *na = NickAlias::Find(GetAccount());
			if (!na || na->nc->HasExt("NS_SUSPENDED") || na->nc->HasExt("UNCONFIRMED"))
				return OnFail();

			unsigned int maxlogins = Config->GetModule("ns_identify").Get<unsigned int>("maxlogins");
			if (maxlogins && na->nc->users.size() >= maxlogins)
				return OnFail();

			Session *s = service->GetSession(uid);
			if (s)
			{
				Log(this->GetOwner(), "sasl", Config->GetClient("NickServ")) << GetUserInfo() << " identified to account " << this->GetAccount() << " using SASL";
				service->Succeed(s, na->nc);
				delete s;
			}
		}

		void OnFail() override
		{
			if (!service)
				return;

			Session *s = service->GetSession(uid);
			if (s)
			{
				service->Fail(s);
				delete s;
			}

			Anope::string accountstatus;
			NickAlias *na = NickAlias::Find(GetAccount());
			if (!na)
				accountstatus = "nonexistent ";
			else if (na->nc->HasExt("NS_SUSPENDED"))
				accountstatus = "suspended ";
			else if (na->nc->HasExt("UNCONFIRMED"))
				accountstatus = "unconfirmed ";

			Log(this->GetOwner(), "sasl", Config->GetClient("NickServ")) << GetUserInfo() << " failed to identify for " << accountstatus << "account " << this->GetAccount() << " using SASL";
		}
	};

	/** Sends IRCd messages used by the SASL module. */
	class ProtocolInterface
		: public ::Service
	{
	protected:
		ProtocolInterface(Module *o)
			: ::Service(o, "SASL::ProtocolInterface", "sasl")
		{
		}

	public:
		/** Sends the list of SASL mechanisms to the IRCd
		 * @param mechs The list of SASL mechanisms.
		 */
		virtual void SendSASLMechanisms(std::vector<Anope::string> &mechs) { };

		/** Sends a SASL message to the IRCd.
		 * @param message The SASL message to send.
		 */
		virtual void SendSASLMessage(const SASL::Message &message) = 0;

		/** Sends a login or logout for \p uid to \p na.
		 * @param uid The uid of the user to log in.
		 * @param na The nick alias to log the user in as or logout if nullptr.
		 */
		virtual void SendSVSLogin(const Anope::string &uid, NickAlias *na) = 0;
	};

	static ServiceReference<SASL::ProtocolInterface> protocol_interface("SASL::ProtocolInterface", "sasl");
}
