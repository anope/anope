/* Charybdis IRCD functions
 *
 * (C) 2003-2016 Anope Team
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

class ChannelModeLargeBan : public ChannelMode
{
 public:
	ChannelModeLargeBan(const Anope::string &mname, char modeChar) : ChannelMode(mname, modeChar) { }

	bool CanSet(User *u) const anope_override
	{
		return u && u->HasMode("OPER");
	}
};


class CharybdisProto : public IRCDProto
{
 public:
	CharybdisProto(Module *creator) : IRCDProto(creator, "Charybdis 3.4+")
	{
		DefaultPseudoclientModes = "+oiS";
		CanCertFP = true;
		CanSNLine = true;
		CanSQLine = true;
		CanSZLine = true;
		CanSVSNick = true;
		CanSVSHold = true;
		CanSetVHost = true;
		RequiresID = true;
		MaxModes = 4;
	}

	void SendSVSKillInternal(const MessageSource &source, User *targ, const Anope::string &reason) anope_override { ratbox->SendSVSKillInternal(source, targ, reason); }
	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { ratbox->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { ratbox->SendGlobalPrivmsg(bi, dest, msg); }
	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) anope_override { ratbox->SendGlobopsInternal(source, buf); }
	void SendSGLine(User *u, const XLine *x) anope_override { ratbox->SendSGLine(u, x); }
	void SendSGLineDel(const XLine *x) anope_override { ratbox->SendSGLineDel(x); }
	void SendAkill(User *u, XLine *x) anope_override { ratbox->SendAkill(u, x); }
	void SendAkillDel(const XLine *x) anope_override { ratbox->SendAkillDel(x); }
	void SendSQLineDel(const XLine *x) anope_override { ratbox->SendSQLineDel(x); }
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override { ratbox->SendJoin(user, c, status); }
	void SendServer(const Server *server) anope_override { ratbox->SendServer(server); }
	void SendChannel(Channel *c) anope_override { ratbox->SendChannel(c); }
	void SendTopic(const MessageSource &source, Channel *c) anope_override { ratbox->SendTopic(source, c); }
	bool IsIdentValid(const Anope::string &ident) anope_override { return ratbox->IsIdentValid(ident); }
	void SendLogin(User *u, NickAlias *na) anope_override { ratbox->SendLogin(u, na); }
	void SendLogout(User *u) anope_override { ratbox->SendLogout(u); }

	void SendSQLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message(Me) << "RESV * " << x->mask << " :" << x->GetReason();
	}

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "PASS " << Config->Uplinks[Anope::CurrentUplink].password << " TS 6 :" << Me->GetSID();
		/*
		 * Received: CAPAB :BAN CHW CLUSTER ENCAP EOPMOD EUID EX IE KLN
		 *           KNOCK MLOCK QS RSFNC SAVE SERVICES TB UNKLN
		 *
		 * BAN      - Can do BAN message
		 * CHW      - Can do channel wall @#
		 * CLUSTER  - Supports umode +l, can send LOCOPS (encap only)
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
		UplinkSocket::Message() << "CAPAB :BAN CHW CLUSTER ENCAP EOPMOD EUID EX IE KLN KNOCK MLOCK QS RSFNC SERVICES TB UNKLN";

		/* Make myself known to myself in the serverlist */
		SendServer(Me);

		/*
		 * Received: SVINFO 6 6 0 :1353235537
		 *  arg[0] = current TS version
		 *  arg[1] = minimum required TS version
		 *  arg[2] = '0'
		 *  arg[3] = server's idea of UTC time
		 */
		UplinkSocket::Message() << "SVINFO 6 6 0 :" << Anope::CurTime;
	}

	void SendClientIntroduction(User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "EUID " << u->nick << " 1 " << u->timestamp << " " << modes << " " << u->GetIdent() << " " << u->host << " 0 " << u->GetUID() << " * * :" << u->realname;
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP " << u->server->GetName() << " RSFNC " << u->GetUID()
						<< " " << newnick << " " << when << " " << u->timestamp;
	}

	void SendSVSHold(const Anope::string &nick, time_t delay) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP * NICKDELAY " << delay << " " << nick;
	}

	void SendSVSHoldDel(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP * NICKDELAY 0 " << nick;
	}

	void SendVhost(User *u, const Anope::string &ident, const Anope::string &host) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP * CHGHOST " << u->GetUID() << " :" << host;
	}

	void SendVhostDel(User *u) anope_override
	{
		this->SendVhost(u, "", u->host);
	}

	void SendSASLMessage(const SASL::Message &message) anope_override
	{
		Server *s = Server::Find(message.target.substr(0, 3));
		UplinkSocket::Message(Me) << "ENCAP " << (s ? s->GetName() : message.target.substr(0, 3)) << " SASL " << message.source << " " << message.target << " " << message.type << " " << message.data << (message.ext.empty() ? "" : (" " + message.ext));
	}

	void SendSVSLogin(const Anope::string &uid, const Anope::string &acc) anope_override
	{
		Server *s = Server::Find(uid.substr(0, 3));
		UplinkSocket::Message(Me) << "ENCAP " << (s ? s->GetName() : uid.substr(0, 3)) << " SVSLOGIN " << uid << " * * * " << acc;
	}
};


struct IRCDMessageEncap : IRCDMessage
{
	IRCDMessageEncap(Module *creator) : IRCDMessage(creator, "ENCAP", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT);}

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.GetUser();

		// In a burst, states that the source user is logged in as the account.
		if (params[1] == "LOGIN" || params[1] == "SU")
		{
			NickCore *nc = NickCore::Find(params[2]);
			if (!nc)
				return;
			u->Login(nc);
		}
		// Received: :42XAAAAAE ENCAP * CERTFP :3f122a9cc7811dbad3566bf2cec3009007c0868f
		if (params[1] == "CERTFP")
		{
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
		 * Charybdis only accepts messages from SASL agents; these must have umode +S
		 */
		if (params[1] == "SASL" && SASL::sasl && params.size() >= 6)
		{
			SASL::Message m;
			m.source = params[2];
			m.target = params[3];
			m.type = params[4];
			m.data = params[5];
			m.ext = params.size() > 6 ? params[6] : "";

			SASL::sasl->ProcessMessage(m);
		}
	}
};

struct IRCDMessageEUID : IRCDMessage
{
	IRCDMessageEUID(Module *creator) : IRCDMessage(creator, "EUID", 11) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		NickAlias *na = NULL;
		if (params[9] != "*")
			na = NickAlias::Find(params[9]);

		User::OnIntroduce(params[0], params[4], params[8], params[5], params[6], source.GetServer(), params[10], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime, params[3], params[7], na ? *na->nc : NULL);
	}
};

// we can't use this function from ratbox because we set a local variable here
struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	// SERVER dev.anope.de 1 :charybdis test server
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		// Servers other then our immediate uplink are introduced via SID
		if (params[1] != "1")
			return;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
		IRCD->SendPing(Me->GetName(), params[0]);
	}
};

// we can't use this function from ratbox because we set a local variable here
struct IRCDMessagePass : IRCDMessage
{
	IRCDMessagePass(Module *creator) : IRCDMessage(creator, "PASS", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		// UplinkSID is used in IRCDMessageServer
		UplinkSID = params[3];
	}
};

class ProtoCharybdis : public Module
{
	Module *m_ratbox;

	CharybdisProto ircd_proto;

	/* Core message handlers */
	Message::Away message_away;
	Message::Capab message_capab;
	Message::Error message_error;
	Message::Invite message_invite;
	Message::Kick message_kick;
	Message::Kill message_kill;
	Message::Mode message_mode;
	Message::MOTD message_motd;
	Message::Notice message_notice;
	Message::Part message_part;
	Message::Ping message_ping;
	Message::Privmsg message_privmsg;
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
	IRCDMessagePass message_pass;
	IRCDMessageServer message_server;

	bool use_server_side_mlock;

	void AddModes()
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
	ProtoCharybdis(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR),
		ircd_proto(this),
		message_away(this), message_capab(this), message_error(this), message_invite(this), message_kick(this),
		message_kill(this), message_mode(this), message_motd(this), message_notice(this), message_part(this),
		message_ping(this), message_privmsg(this), message_quit(this), message_squit(this), message_stats(this),
		message_time(this), message_topic(this), message_version(this), message_whois(this),

		message_bmask("IRCDMessage", "charybdis/bmask", "ratbox/bmask"),
		message_join("IRCDMessage", "charybdis/join", "ratbox/join"),
		message_nick("IRCDMessage", "charybdis/nick", "ratbox/nick"),
		message_pong("IRCDMessage", "charybdis/pong", "ratbox/pong"),
		message_sid("IRCDMessage", "charybdis/sid", "ratbox/sid"),
		message_sjoin("IRCDMessage", "charybdis/sjoin", "ratbox/sjoin"),
		message_tb("IRCDMessage", "charybdis/tb", "ratbox/tb"),
		message_tmode("IRCDMessage", "charybdis/tmode", "ratbox/tmode"),
		message_uid("IRCDMessage", "charybdis/uid", "ratbox/uid"),

		message_encap(this), message_euid(this), message_pass(this), message_server(this)

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

	~ProtoCharybdis()
	{
		m_ratbox = ModuleManager::FindModule("ratbox");
		ModuleManager::UnloadModule(m_ratbox, NULL);
	}

	void OnReload(Configuration::Conf *conf) anope_override
	{
		use_server_side_mlock = conf->GetModule(this)->Get<bool>("use_server_side_mlock");
	}

	void OnChannelSync(Channel *c) anope_override
	{
		if (!c->ci)
			return;

		ModeLocks *modelocks = c->ci->GetExt<ModeLocks>("modelocks");
		if (use_server_side_mlock && modelocks && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(c->creation_time) << " " << c->ci->name << " " << modes;
		}
	}

	EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock) anope_override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (use_server_side_mlock && cm && ci->c && modelocks && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->mchar;
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->name << " " << modes;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChannelInfo *ci, ModeLock *lock) anope_override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (use_server_side_mlock && cm && modelocks && ci->c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->mchar, "");
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->name << " " << modes;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ProtoCharybdis)
