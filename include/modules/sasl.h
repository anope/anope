/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

namespace SASL
{
	struct Message
	{
		Anope::string source;
		Anope::string target;
		Anope::string type;
		Anope::string data;
		Anope::string ext;
	};

	class Mechanism;

	struct Session
	{
		time_t created;
		Anope::string uid;
		Reference<Mechanism> mech;

		Session(Mechanism *m, const Anope::string &u) : created(Anope::CurTime), uid(u), mech(m) { }
		virtual ~Session() { }
	};

	/* PLAIN, EXTERNAL, etc */
	class Mechanism : public Service
	{
	 public:
		Mechanism(Module *o, const Anope::string &sname) : Service(o, "SASL::Mechanism", sname) { }

		virtual Session* CreateSession(const Anope::string &uid) { return new Session(this, uid); }

		virtual void ProcessMessage(Session *session, const Message &) = 0;
	};

	class Service : public ::Service
	{
	 public:
		Service(Module *o) : ::Service(o, "SASL::Service", "sasl") { }

		virtual void ProcessMessage(const Message &) = 0;

		virtual Anope::string GetAgent() = 0;

		virtual Session* GetSession(const Anope::string &uid) = 0;

		virtual void SendMessage(SASL::Session *session, const Anope::string &type, const Anope::string &data) = 0;

		virtual void Succeed(Session *, NickCore *) = 0;
		virtual void Fail(Session *) = 0;
		virtual void SendMechs(Session *) = 0;
	};
}

static ServiceReference<SASL::Service> sasl("SASL::Service", "sasl");

