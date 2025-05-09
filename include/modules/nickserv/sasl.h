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
	class Mechanism;
	struct Message;
	class ProtocolInterface;
	struct Service;
	struct Session;

	/** The SASL interface implemented by the protocol modules. */
	static ServiceReference<SASL::ProtocolInterface> protocol_interface("SASL::ProtocolInterface", "sasl");

	/** The SASL interface implemented by ns_sasl. */
	static ServiceReference<SASL::Service> service("SASL::Service", "sasl");
}

/** Represents a single SASL message. */
struct SASL::Message final
{
	/** The source UID or name. */
	Anope::string source;

	/** The target UID or name. */
	Anope::string target;

	/** The type of message. */
	Anope::string type;

	/** One or more data parameters. */
	std::vector<Anope::string> data;
};

/** Sends IRCd messages used by the SASL module. */
class SASL::ProtocolInterface
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

/** SASL service interface. */
struct SASL::Service
	: ::Service
{
	Service(Module *o)
		: ::Service(o, "SASL::Service", "sasl")
	{
	}

	virtual void DeleteSessions(Mechanism *mech, bool da = false) = 0;

	/** Fails a SASL session. This notifies the client and increments the
	 * failed attempt counter which may kill the client if the bad password
	 * limit has been reached.
	 *
	 * @param sess The session to fail.
	 */
	virtual void Fail(Session *sess) = 0;

	/** Retrieves the session for the specified user identifier. */
	virtual Session *GetSession(const Anope::string &uid) = 0;

	/** Processes a SASL authentication message from the uplink.
	 * @param m The message to process.
	 */
	virtual void ProcessMessage(const Message &m) = 0;

	/** Removes the specified SASL session from the service.
	 * @param sess The session to remove.
	 */
	virtual void RemoveSession(Session *sess) = 0;

	/** Sends a SASL message for the specified session.
	 * @param type The type of message to send.
	 * @param data The contents of the SASL message.
	 */
	virtual void SendMessage(SASL::Session *sess, const Anope::string &type, const Anope::string &data) = 0;

	/** Completes a successful authentication.
	 * @param sess The session which finished authentication.
	 * @param nc The account which has been logged into.
	 */
	virtual void Succeed(Session *sess, NickCore *nc) = 0;
};

/** Represents a single SASL session. */
struct SASL::Session
{
	/** The time at which the session was created. */
	const time_t created;

	/** The hostname and IP address of the authenticating user. */
	Anope::string hostname, ip;

	/** A reference to the mechanism that the session is authenticating using. */
	Reference<Mechanism> mech;

	/** The unique identifier of the authenticating user. */
	const Anope::string uid;

	/** Creates a new authentication session.
	 * @param m The mechanism that the session is authenticating with.
	 * @param u The unique identifier of the authenticating user.
	 */
	Session(Mechanism *m, const Anope::string &u)
		: created(Anope::CurTime)
		, mech(m)
		, uid(u)
	{
	}

	virtual ~Session()
	{
		if (service)
			service->RemoveSession(this);
	}

	/** Retrieves a description of the user this session is associated with. */
	inline Anope::string GetUserInfo()
	{
		auto *u = User::Find(uid);
		if (u)
			return u->GetMask();
		if (!hostname.empty() && !ip.empty())
			return Anope::printf("%s (%s)", hostname.c_str(), ip.c_str());
		return "A user";
	};
};

/* Represents an authentication mechanism. */
class SASL::Mechanism
	: public ::Service
{
protected:
	/** Creates a new authentication mechanism.
	 * @param o The module that owns this instance.
	 * @param sname The name of the authentication mechanism (e.g. PLAIN).
	 */
	Mechanism(Module *o, const Anope::string &sname)
		: Service(o, "SASL::Mechanism", sname)
	{
	}

public:
	virtual ~Mechanism()
	{
		if (service)
			service->DeleteSessions(this, true);
	}

	/** Creates a new SASL session.
	 * @param uid The unique identifier of the authenticating user.
	 */
	virtual Session *CreateSession(const Anope::string &uid)
	{
		return new Session(this, uid);
	}

	/** Processes an authentication message from the authenticating user.
	 * @param session The session for which the message applies.
	 * @param m The message sent by the user.
	 * @return True if the message was processed successfully or false if
	 *         the session should be aborted.
	 */
	virtual bool ProcessMessage(Session *sess, const Message &m) = 0;
};
