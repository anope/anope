/* Solanum functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/cs_mode.h"
#include "modules/sasl.h"

static Anope::string UplinkSID;

static ServiceReference<IRCDProto> ratbox("IRCDProto", "ratbox");

class ChannelModeLargeBan final
	: public ChannelMode
{
public:
	ChannelModeLargeBan(const Anope::string &mname, char modeChar) : ChannelMode(mname, modeChar) { }

	bool CanSet(User *u) const override
	{
		return u && u->HasMode("OPER");
	}
};


class SolanumProto final
	: public IRCDProto
	, public SASL::ProtocolInterface
{
public:
	SolanumProto(Module *creator)
		: IRCDProto(creator, "Solanum")
		, SASL::ProtocolInterface(creator)
	{
		DefaultPseudoclientModes = "+oiS";
		CanCertFP = true;
		CanSNLine = true;
		CanSQLine = true;
		CanSQLineChannel = true;
		CanSZLine = true;
		CanSVSNick = true;
		CanSVSHold = true;
		CanSetVHost = true;
		RequiresID = true;
		MaxModes = 4;
	}

	void SendSVSKill(const MessageSource &source, User *targ, const Anope::string &reason) override { ratbox->SendSVSKill(source, targ, reason); }
	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) override { ratbox->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) override { ratbox->SendGlobalPrivmsg(bi, dest, msg); }
	void SendGlobops(const MessageSource &source, const Anope::string &buf) override { ratbox->SendGlobops(source, buf); }
	void SendSGLine(User *u, const XLine *x) override { ratbox->SendSGLine(u, x); }
	void SendSGLineDel(const XLine *x) override { ratbox->SendSGLineDel(x); }
	void SendAkill(User *u, XLine *x) override { ratbox->SendAkill(u, x); }
	void SendAkillDel(const XLine *x) override { ratbox->SendAkillDel(x); }
	void SendSQLine(User *u, const XLine *x) override { ratbox->SendSQLine(u, x); }
	void SendSQLineDel(const XLine *x) override { ratbox->SendSQLineDel(x); }
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override { ratbox->SendJoin(user, c, status); }
	void SendServer(const Server *server) override { ratbox->SendServer(server); }
	void SendChannel(Channel *c) override { ratbox->SendChannel(c); }
	void SendTopic(const MessageSource &source, Channel *c) override { ratbox->SendTopic(source, c); }
	bool IsIdentValid(const Anope::string &ident) override { return ratbox->IsIdentValid(ident); }
	void SendLogin(User *u, NickAlias *na) override { ratbox->SendLogin(u, na); }
	void SendLogout(User *u) override { ratbox->SendLogout(u); }

	void SendSASLMechanisms(std::vector<Anope::string> &mechanisms) override
	{
		Anope::string mechlist;
		for (const auto &mechanism : mechanisms)
			mechlist += "," + mechanism;

		Uplink::Send("ENCAP", '*', "MECHLIST", mechanisms.empty() ? "" : mechlist.substr(1));
	}

	void SendConnect() override
	{
		Uplink::Send("PASS", Config->Uplinks[Anope::CurrentUplink].password, "TS", 6, Me->GetSID());

		/*
		 * Received: CAPAB :BAN CHW CLUSTER ENCAP EOPMOD EUID EX IE KLN
		 *           KNOCK MLOCK QS RSFNC SAVE SERVICES TB UNKLN
		 *
		 * BAN      - Can do BAN message
		 * CHW      - Can do channel wall @#
		 * CLUSTER  - Supports umode +l, can send LOCOPS (encap only)
		 * ECHO     - Supports sending echoed messages
		 * ENCAP    - Can do ENCAP message
		 * EOPMOD   - Can do channel wall =# (for cmode +z)
		 * EUID     - Can do EUID (its similar to UID but includes the ENCAP REALHOST and ENCAP LOGIN information)
		 * EX       - Can do channel +e exemptions
		 * GLN      - Can set G:Lines
		 * IE       - Can do invite exceptions
		 * KLN      - Can set K:Lines (encap only)
		 * KNOCK    - Supports KNOCK
		 * MLOCK    - Supports MLOCK
		 * RSFNC    - Forces a nickname change and propagates it (encap only)
		 * SERVICES - Support channel mode +r (only registered users may join)
		 * SAVE     - Resolve a nick collision by changing a nickname to the UID.
		 * TB       - Supports topic burst
		 * UNKLN    - Can do UNKLINE (encap only)
		 * QS       - Can handle quit storm removal
		*/
		Uplink::Send("CAPAB", "BAN CHW CLUSTER ECHO ENCAP EOPMOD EUID EX IE KLN KNOCK MLOCK QS RSFNC SERVICES TB UNKLN");

		/* Make myself known to myself in the serverlist */
		SendServer(Me);

		/*
		 * Received: SVINFO 6 6 0 :1353235537
		 *  arg[0] = current TS version
		 *  arg[1] = minimum required TS version
		 *  arg[2] = '0'
		 *  arg[3] = server's idea of UTC time
		 */
		Uplink::Send("SVINFO", 6, 6, 0, Anope::CurTime);
	}

	void SendClientIntroduction(User *u) override
	{
		Uplink::Send("EUID", u->nick, 1, u->timestamp, "+" + u->GetModes(), u->GetIdent(), u->host, 0, u->GetUID(), '*', '*', u->realname);
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) override
	{
		Uplink::Send("ENCAP", u->server->GetName(), "RSFNC", u->GetUID(), newnick, when, u->timestamp);
	}

	void SendSVSHold(const Anope::string &nick, time_t delay) override
	{
		Uplink::Send("ENCAP", '*', "NICKDELAY", delay, nick);
	}

	void SendSVSHoldDel(const Anope::string &nick) override
	{
		Uplink::Send("ENCAP", '*', "NICKDELAY", 0, nick);
	}

	void SendVHost(User *u, const Anope::string &ident, const Anope::string &host) override
	{
		Uplink::Send("ENCAP", '*', "CHGHOST", u->GetUID(), host);
	}

	void SendVHostDel(User *u) override
	{
		this->SendVHost(u, "", u->host);
	}

	void SendSASLMessage(const SASL::Message &message) override
	{
		Server *s = Server::Find(message.target.substr(0, 3));
		auto target = s ? s->GetName() : message.target.substr(0, 3);

		auto newparams = message.data;
		newparams.insert(newparams.begin(), { target, "SASL", message.source, message.target, message.type });
		Uplink::SendInternal({}, Me, "ENCAP", newparams);
	}

	void SendSVSLogin(const Anope::string &uid, NickAlias *na) override
	{
		Server *s = Server::Find(uid.substr(0, 3));

		Uplink::Send("ENCAP", s ? s->GetName() : uid.substr(0, 3), "SVSLOGIN", uid, '*',
			na && !na->GetVHostIdent().empty() ? na->GetVHostIdent() : '*',
			na && !na->GetVHostHost().empty() ? na->GetVHostHost() : '*',
			na ? na->nc->display : "0");
	}
};


struct IRCDMessageEncap final
	: IRCDMessage
{
	IRCDMessageEncap(Module *creator) : IRCDMessage(creator, "ENCAP", 3)
	{
		SetFlag(FLAG_SOFT_LIMIT);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		// In a burst, states that the source user is logged in as the account.
		if (params[1] == "LOGIN" || params[1] == "SU")
		{
			User *u = source.GetUser();
			NickCore *nc = NickCore::Find(params[2]);

			if (!u || !nc)
				return;

			u->Login(nc);
		}
		// Received: :42XAAAAAE ENCAP * CERTFP :3f122a9cc7811dbad3566bf2cec3009007c0868f
		else if (params[1] == "CERTFP")
		{
			User *u = source.GetUser();
			if (!u)
				return;

			u->fingerprint = params[2];

			FOREACH_MOD(OnFingerprint, (u));
		}
		/*
		 * Received: :42X ENCAP * SASL 42XAAAAAH * S PLAIN
		 * Received: :42X ENCAP * SASL 42XAAAAAC * D A
		 *
		 * Part of a SASL authentication exchange. The mode is 'C' to send some data
		 * (base64 encoded), or 'S' to end the exchange (data indicates type of
		 * termination: 'A' for abort, 'F' for authentication failure, 'S' for
		 * authentication success).
		 *
		 * Solanum only accepts messages from SASL agents; these must have umode +S
		 */
		else if (params[1] == "SASL" && SASL::sasl && params.size() >= 6)
		{
			SASL::Message m;
			m.source = params[2];
			m.target = params[3];
			m.type = params[4];
			m.data.assign(params.begin() + 5, params.end());
			SASL::sasl->ProcessMessage(m);
		}
	}
};

struct IRCDMessageEUID final
	: IRCDMessage
{
	IRCDMessageEUID(Module *creator) : IRCDMessage(creator, "EUID", 11) { SetFlag(FLAG_REQUIRE_SERVER); }

	/*
	 * :42X EUID DukePyrolator 1 1353240577 +Zi ~jens erft-5d80b00b.pool.mediaWays.net 93.128.176.11 42XAAAAAD * * :jens
	 * :<SID> EUID <NICK> <HOPS> <TS> +<UMODE> <USERNAME> <VHOST> <IP> <UID> <REALHOST> <ACCOUNT> :<GECOS>
	 *               0      1     2      3         4         5     6     7       8         9         10
	 *
	 * Introduces a user. The hostname field is now always the visible host.
	 * The realhost field is * if the real host is equal to the visible host.
	 * The account field is * if the login is not set.
	 * Note that even if both new fields are *, an EUID command still carries more
	 * information than a UID command (namely that real host is visible host and the
	 * user is not logged in with services). Hence a NICK or UID command received
	 * from a remote server should not be sent in EUID form to other servers.
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		NickAlias *na = NULL;
		if (params[9] != "*")
			na = NickAlias::Find(params[9]);

		User::OnIntroduce(params[0], params[4], (params[8] != "*" ? params[8] : params[5]), params[5], params[6], source.GetServer(),
			params[10], IRCD->ExtractTimestamp(params[2]), params[3], params[7], na ? *na->nc : NULL);
	}
};

// we can't use this function from ratbox because we set a local variable here
struct IRCDMessageServer final
	: IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(FLAG_REQUIRE_SERVER); }

	// SERVER dev.anope.de 1 :solanum test server
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		// Servers other then our immediate uplink are introduced via SID
		if (params[1] != "1")
			return;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
		IRCD->SendPing(Me->GetName(), params[0]);
	}
};

// we can't use this function from ratbox because we set a local variable here
struct IRCDMessagePass final
	: IRCDMessage
{
	IRCDMessagePass(Module *creator) : IRCDMessage(creator, "PASS", 4) { SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		// UplinkSID is used in IRCDMessageServer
		UplinkSID = params[3];
	}
};

struct IRCDMessageNotice final
	: Message::Notice
{
	IRCDMessageNotice(Module *creator) : Message::Notice(creator) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (Servers::Capab.count("ECHO"))
			Uplink::Send("ECHO", 'N', source.GetSource(), params[1]);

		Message::Notice::Run(source, params, tags);
	}
};

struct IRCDMessagePrivmsg final
	: Message::Privmsg
{
	IRCDMessagePrivmsg(Module *creator) : Message::Privmsg(creator) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (Servers::Capab.count("ECHO"))
			Uplink::Send("ECHO", 'P', source.GetSource(), params[1]);

		Message::Privmsg::Run(source, params, tags);
	}
};

class ProtoSolanum final
	: public Module
{
	Module *m_ratbox;

	SolanumProto ircd_proto;

	/* Core message handlers */
	Message::Away message_away;
	Message::Capab message_capab;
	Message::Error message_error;
	Message::Invite message_invite;
	Message::Kick message_kick;
	Message::Kill message_kill;
	Message::Mode message_mode;
	Message::MOTD message_motd;
	Message::Part message_part;
	Message::Ping message_ping;
	Message::Quit message_quit;
	Message::SQuit message_squit;
	Message::Stats message_stats;
	Message::Time message_time;
	Message::Topic message_topic;
	Message::Version message_version;
	Message::Whois message_whois;

	/* Ratbox Message Handlers */
	ServiceAlias message_bmask, message_join, message_nick, message_pong, message_sid, message_sjoin,
		message_tb, message_tmode, message_uid;

	/* Our message handlers */
	IRCDMessageEncap message_encap;
	IRCDMessageEUID message_euid;
	IRCDMessageNotice message_notice;
	IRCDMessagePass message_pass;
	IRCDMessagePrivmsg message_privmsg;
	IRCDMessageServer message_server;

	static void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserMode("NOFORWARD", 'Q'));
		ModeManager::AddUserMode(new UserMode("REGPRIV", 'R'));
		ModeManager::AddUserMode(new UserModeOperOnly("OPERWALLS", 'z'));
		ModeManager::AddUserMode(new UserModeNoone("SSL", 'Z'));

		/* b/e/I */
		ModeManager::AddChannelMode(new ChannelModeList("QUIET", 'q'));

		/* Add channel modes */
		ModeManager::AddChannelMode(new ChannelMode("BLOCKCOLOR", 'c'));
		ModeManager::AddChannelMode(new ChannelMode("NOCTCP", 'C'));
		ModeManager::AddChannelMode(new ChannelModeParam("REDIRECT", 'f'));
		ModeManager::AddChannelMode(new ChannelMode("ALLOWFORWARD", 'F'));
		ModeManager::AddChannelMode(new ChannelMode("ALLINVITE", 'g'));
		ModeManager::AddChannelMode(new ChannelModeParam("JOINFLOOD", 'j'));
		ModeManager::AddChannelMode(new ChannelModeLargeBan("LBAN", 'L'));
		ModeManager::AddChannelMode(new ChannelMode("PERM", 'P'));
		ModeManager::AddChannelMode(new ChannelMode("NOFORWARD", 'Q'));
		ModeManager::AddChannelMode(new ChannelMode("OPMODERATED", 'z'));
	}

public:
	ProtoSolanum(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR),
		ircd_proto(this),
		message_away(this), message_capab(this), message_error(this), message_invite(this), message_kick(this),
		message_kill(this), message_mode(this), message_motd(this), message_part(this), message_ping(this),
		message_quit(this), message_squit(this), message_stats(this), message_time(this), message_topic(this),
		message_version(this), message_whois(this),

		message_bmask("IRCDMessage", "solanum/bmask", "ratbox/bmask"),
		message_join("IRCDMessage", "solanum/join", "ratbox/join"),
		message_nick("IRCDMessage", "solanum/nick", "ratbox/nick"),
		message_pong("IRCDMessage", "solanum/pong", "ratbox/pong"),
		message_sid("IRCDMessage", "solanum/sid", "ratbox/sid"),
		message_sjoin("IRCDMessage", "solanum/sjoin", "ratbox/sjoin"),
		message_tb("IRCDMessage", "solanum/tb", "ratbox/tb"),
		message_tmode("IRCDMessage", "solanum/tmode", "ratbox/tmode"),
		message_uid("IRCDMessage", "solanum/uid", "ratbox/uid"),

		message_encap(this), message_euid(this), message_notice(this), message_pass(this),
		message_privmsg(this), message_server(this)
	{


		if (ModuleManager::LoadModule("ratbox", User::Find(creator)) != MOD_ERR_OK)
			throw ModuleException("Unable to load ratbox");
		m_ratbox = ModuleManager::FindModule("ratbox");
		if (!m_ratbox)
			throw ModuleException("Unable to find ratbox");
		if (!ratbox)
			throw ModuleException("No protocol interface for ratbox");

		this->AddModes();
	}

	~ProtoSolanum() override
	{
		m_ratbox = ModuleManager::FindModule("ratbox");
		ModuleManager::UnloadModule(m_ratbox, NULL);
	}

	void OnUserLogin(User *u) override
	{
		// If the user has logged into their current nickname then mark them as such.
		NickAlias *na = NickAlias::Find(u->nick);
		if (na && na->nc == u->Account())
			Uplink::Send("ENCAP", '*', "IDENTIFIED", u->GetUID(), u->nick);
		else
			Uplink::Send("ENCAP", '*', "IDENTIFIED", u->GetUID(), u->nick, "OFF");
	}

	void OnNickLogout(User *u) override
	{
		// We don't know what account the user was logged into so send in all cases.
		Uplink::Send("ENCAP", '*', "IDENTIFIED", u->GetUID(), u->nick, "OFF");
	}

	void OnUserNickChange(User *u, const Anope::string &) override
	{
		// If the user is logged into an account check if we need to mark them
		// as not identified to their nick.
		if (u->Account())
			OnUserLogin(u);
	}

	void OnChannelSync(Channel *c) override
	{
		if (!c->ci)
			return;

		ModeLocks *modelocks = c->ci->GetExt<ModeLocks>("modelocks");
		if (modelocks && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");
			Uplink::Send("MLOCK", c->creation_time, c->ci->name, modes);
		}
	}

	EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock) override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (cm && ci->c && modelocks && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->mchar;
			Uplink::Send("MLOCK", ci->c->creation_time, ci->name, modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChannelInfo *ci, ModeLock *lock) override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (cm && modelocks && ci->c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->mchar, "");
			Uplink::Send("MLOCK", ci->c->creation_time, ci->name, modes);
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ProtoSolanum)
