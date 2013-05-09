/* Charybdis IRCD functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

static bool sasl = true;
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

	void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { ratbox->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { ratbox->SendGlobalPrivmsg(bi, dest, msg); }
	void SendGlobopsInternal(const BotInfo *source, const Anope::string &buf) anope_override { ratbox->SendGlobopsInternal(source, buf); }
	void SendSGLine(User *u, const XLine *x) anope_override { ratbox->SendSGLine(u, x); }
	void SendSGLineDel(const XLine *x) anope_override { ratbox->SendSGLineDel(x); }
	void SendAkill(User *u, XLine *x) anope_override { ratbox->SendAkill(u, x); }
	void SendAkillDel(const XLine *x) anope_override { ratbox->SendAkillDel(x); }
	void SendSQLineDel(const XLine *x) anope_override { ratbox->SendSQLineDel(x); }
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override { ratbox->SendJoin(user, c, status); }
	void SendServer(const Server *server) anope_override { ratbox->SendServer(server); }
	void SendChannel(Channel *c) anope_override { ratbox->SendChannel(c); }
	void SendTopic(BotInfo *bi, Channel *c) anope_override { ratbox->SendTopic(bi, c); }

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
		UplinkSocket::Message() << "CAPAB :BAN CHW CLUSTER ENCAP EOPMOD EUID EX IE KLN KNOCK MLOCK QS RSFNC SAVE SERVICES TB UNKLN";

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

	void SendClientIntroduction(const User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "EUID " << u->nick << " 1 " << u->timestamp << " " << modes << " " << u->GetIdent() << " " << u->host << " 0 " << u->GetUID() << " * * :" << u->realname;
	}

	void SendLogin(User *u) anope_override
	{
		if (!u->Account())
			return;

		UplinkSocket::Message(Me) << "ENCAP * SU " << u->GetUID() << " " << u->Account()->display;
	}

	void SendLogout(User *u) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP * SU " << u->GetUID();
	}

	void SendForceNickChange(const User *u, const Anope::string &newnick, time_t when) anope_override
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
		UplinkSocket::Message(Me) << "ENCAP * CHGHOST " << u->GetUID() << ": " << host;
	}

	void SendVhostDel(User *u) anope_override
	{
		this->SendVhost(u, "", u->host);
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
			FOREACH_MOD(I_OnFingerprint, OnFingerprint(u));
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
		if (params[1] == "SASL" && sasl && params.size() == 6)
		{
			class CharybdisSASLIdentifyRequest : public IdentifyRequest
			{
				Anope::string uid;
				MessageSource msource;

			 public:
				CharybdisSASLIdentifyRequest(Module *m, MessageSource &source_, const Anope::string &id, const Anope::string &acc, const Anope::string &pass) : IdentifyRequest(m, acc, pass), uid(id), msource(source_) { }

				void OnSuccess() anope_override
				{
					/* SVSLOGIN
					 * parameters: target, new nick, new username, new visible hostname, new login name
					 * Sent after successful SASL authentication.
					 * The target is a UID, typically an unregistered one.
					 * Any of the "new" parameters can be '*' to leave the corresponding field
					 * unchanged. The new login name can be '0' to log the user out.
					 * If the UID is registered on the network, a SIGNON with the changes will be
					 * broadcast, otherwise the changes will be stored, to be used when registration
					 * completes.
					 */
					UplinkSocket::Message(Me) << "ENCAP " << msource.GetName() << " SVSLOGIN " << this->uid << " * * * " << this->GetAccount();
					UplinkSocket::Message(Me) << "ENCAP " << msource.GetName() << " SASL " << NickServ->GetUID() << " " << this->uid << " D S";
				}

				void OnFail() anope_override
				{
					UplinkSocket::Message(Me) << "ENCAP " << msource.GetName() << " SASL " << NickServ->GetUID() << " " << this->uid << " " << " D F";

					Log(NickServ) << "A user failed to identify for account " << this->GetAccount() << " using SASL";
				}
			};
			if (params[4] == "S")
			{
				if (params[5] == "PLAIN")
					UplinkSocket::Message(Me) << "ENCAP " << source.GetName() << " SASL " << NickServ->GetUID() << " " << params[2] << " C +";
				else
					UplinkSocket::Message(Me) << "ENCAP " << source.GetName() << " SASL " << NickServ->GetUID() << " " << params[2] << " D F";
			}
			else if (params[4] == "C")
			{
				Anope::string decoded;
				Anope::B64Decode(params[5], decoded);

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

				IdentifyRequest *req = new CharybdisSASLIdentifyRequest(this->owner, source, params[2], acc, pass);
				FOREACH_MOD(I_OnCheckAuthentication, OnCheckAuthentication(NULL, req));
				req->Dispatch();
			}
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
		/* Source is always the server */
		User *u = new User(params[0], params[4], params[8], params[5], params[6], source.GetServer(), params[10], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime, params[3], params[7]);
		if (params[9] != "*")
		{
			NickAlias *na = NickAlias::Find(params[9]);
			if (na)
			{
				u->Login(na->nc);

				if (u->server->IsSynced() && NickServ)
					u->SendMessage(NickServ, _("You have been logged in as \2%s\2."), na->nc->display.c_str());
			}
		}
	}
};

// we cant use this function from ratbox because we set a local variable here
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

// we cant use this function from ratbox because we set a local variable here
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
		message_tb, message_tmode;

	/* Our message handlers */
	IRCDMessageEncap message_encap;
	IRCDMessageEUID message_euid;
	IRCDMessagePass message_pass;
	IRCDMessageServer message_server;

	bool use_server_side_mlock;

	void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserMode("DEAF", 'D'));
		ModeManager::AddUserMode(new UserMode("CALLERID", 'g'));
		ModeManager::AddUserMode(new UserMode("REGPRIV", 'R'));
		ModeManager::AddUserMode(new UserMode("SSL", 'Z')); 
		ModeManager::AddUserMode(new UserMode("LOCOPS", 'l'));
		ModeManager::AddUserMode(new UserMode("OPERWALLS", 'z'));
		ModeManager::AddUserMode(new UserMode("PROTECTED", 'S'));
		ModeManager::AddUserMode(new UserMode("NOFORWARD", 'Q'));

		// charybdis has no usermode for registered users
		ModeManager::RemoveUserMode(ModeManager::FindUserModeByName("REGISTERED"));

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
		ModeManager::AddChannelMode(new ChannelMode("REGISTEREDONLY", 'r'));
		ModeManager::AddChannelMode(new ChannelMode("OPMODERATED", 'z'));
	}

 public:
	ProtoCharybdis(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR),
		ircd_proto(this),
		message_away(this), message_capab(this), message_error(this), message_invite(this), message_kick(this),
		message_kill(this), message_mode(this), message_motd(this), message_part(this), message_ping(this),
		message_privmsg(this), message_quit(this), message_squit(this), message_stats(this), message_time(this),
		message_topic(this), message_version(this), message_whois(this),

		message_bmask("IRCDMessage", "charybdis/bmask", "ratbox/bmask"),
		message_join("IRCDMessage", "charybdis/join", "ratbox/join"),
		message_nick("IRCDMessage", "charybdis/nick", "ratbox/nick"),
		message_pong("IRCDMessage", "charybdis/pong", "ratbox/pong"),
		message_sid("IRCDMessage", "charybdis/sid", "ratbox/sid"),
		message_sjoin("IRCDMessage", "charybdis/sjoin", "ratbox/sjoin"),
		message_tb("IRCDMessage", "charybdis/tb", "ratbox/tb"),
		message_tmode("IRCDMessage", "charybdis/tmode", "ratbox/tmode"),

		message_encap(this), message_euid(this), message_pass(this), message_server(this)

	{

		Implementation i[] = { I_OnReload, I_OnChannelSync, I_OnMLock, I_OnUnMLock };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

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
		ModuleManager::UnloadModule(m_ratbox, NULL);
	}

	void OnReload(Configuration::Conf *conf) anope_override
	{
		use_server_side_mlock = conf->GetModule(this)->Get<bool>("use_server_side_mlock");
		sasl = conf->GetModule(this)->Get<bool>("sasl");
	}

	void OnChannelSync(Channel *c) anope_override
	{
		if (use_server_side_mlock && c->ci && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = c->ci->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(c->creation_time) << " " << c->ci->name << " " << modes;
		}
	}

	EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (use_server_side_mlock && cm && ci->c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = ci->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->mchar;
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->name << " " << modes;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChannelInfo *ci, ModeLock *lock) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (use_server_side_mlock && cm && ci->c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = ci->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->mchar, "");
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->name << " " << modes;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ProtoCharybdis)
