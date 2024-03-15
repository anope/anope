/* Plexus 3+ IRCD functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/sasl.h"

static Anope::string UplinkSID;

static ServiceReference<IRCDProto> hybrid("IRCDProto", "hybrid");

class PlexusProto final
	: public IRCDProto
{
public:
	PlexusProto(Module *creator) : IRCDProto(creator, "hybrid-7.2.3+plexus-3.0.1")
	{
		DefaultPseudoclientModes = "+iU";
		CanSVSNick = true;
		CanSVSJoin = true;
		CanSetVHost = true;
		CanSetVIdent = true;
		CanSNLine = true;
		CanSQLine = true;
		CanSQLineChannel = true;
		CanSVSHold = true;
		CanCertFP = true;
		RequiresID = true;
		MaxModes = 4;
	}

	void SendSVSKill(const MessageSource &source, User *targ, const Anope::string &reason) override { hybrid->SendSVSKill(source, targ, reason); }
	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) override { hybrid->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) override { hybrid->SendGlobalPrivmsg(bi, dest, msg); }
	void SendSQLine(User *u, const XLine *x) override { hybrid->SendSQLine(u, x); }
	void SendSQLineDel(const XLine *x) override { hybrid->SendSQLineDel(x); }
	void SendSGLineDel(const XLine *x) override { hybrid->SendSGLineDel(x); }
	void SendSGLine(User *u, const XLine *x) override { hybrid->SendSGLine(u, x); }
	void SendAkillDel(const XLine *x) override { hybrid->SendAkillDel(x); }
	void SendAkill(User *u, XLine *x) override { hybrid->SendAkill(u, x); }
	void SendServer(const Server *server) override { hybrid->SendServer(server); }
	void SendChannel(Channel *c) override { hybrid->SendChannel(c); }
	void SendSVSHold(const Anope::string &nick, time_t t) override { hybrid->SendSVSHold(nick, t); }
	void SendSVSHoldDel(const Anope::string &nick) override { hybrid->SendSVSHoldDel(nick); }

	void SendGlobops(const MessageSource &source, const Anope::string &buf) override
	{
		Uplink::Send(source, "OPERWALL", buf);
	}

	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override
	{
		Uplink::Send("SJOIN", c->creation_time, c->name, "+" + c->GetModes(true, true), user->GetUID());
		if (status)
		{
			/* First save the channel status incase uc->Status == status */
			ChannelStatus cs = *status;
			/* If the user is internally on the channel with flags, kill them so that
			 * the stacker will allow this.
			 */
			ChanUserContainer *uc = c->FindUser(user);
			if (uc != NULL)
				uc->status.Clear();

			BotInfo *setter = BotInfo::Find(user->GetUID());
			for (auto mode : cs.Modes())
				c->SetMode(setter, ModeManager::FindChannelModeByChar(mode), user->GetUID(), false);

			if (uc != NULL)
				uc->status = cs;
		}
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) override
	{
		Uplink::Send("ENCAP", u->server->GetName(), "SVSNICK", u->GetUID(), u->timestamp, newnick, when);
	}

	void SendVHost(User *u, const Anope::string &ident, const Anope::string &host) override
	{
		if (!ident.empty())
			Uplink::Send("ENCAP", '*', "CHGIDENT", u->GetUID(), ident);

		Uplink::Send("ENCAP", '*', "CHGHOST", u->GetUID(), host);
		u->SetMode(Config->GetClient("HostServ"), "CLOAK");
	}

	void SendVHostDel(User *u) override
	{
		u->RemoveMode(Config->GetClient("HostServ"), "CLOAK");
	}

	void SendConnect() override
	{
		Uplink::Send("PASS", Config->Uplinks[Anope::CurrentUplink].password, "TS", 6, Me->GetSID());

		/* CAPAB
		 * QS     - Can handle quit storm removal
		 * EX     - Can do channel +e exemptions
		 * CHW    - Can do channel wall @#
		 * LL     - Can do lazy links
		 * IE     - Can do invite exceptions
		 * EOB    - Can do EOB message
		 * KLN    - Can do KLINE message
		 * GLN    - Can do GLINE message
		 * HUB    - This server is a HUB
		 * AOPS   - Can do anon ops (+a)
		 * UID    - Can do UIDs
		 * ZIP    - Can do ZIPlinks
		 * ENC    - Can do ENCrypted links
		 * KNOCK  - Supports KNOCK
		 * TBURST - Supports TBURST
		 * PARA   - Supports invite broadcasting for +p
		 * ENCAP  - Supports encapsulation of protocol messages
		 * SVS    - Supports services protocol extensions
		 */
		Uplink::Send("CAPAB", "QS EX CHW IE EOB KLN UNKLN GLN HUB KNOCK TBURST PARA ENCAP SVS");

		/* Make myself known to myself in the serverlist */
		SendServer(Me);
		/*
		 * SVINFO
		 *	  parv[0] = sender prefix
		 *	  parv[1] = TS_CURRENT for the server
		 *	  parv[2] = TS_MIN for the server
		 *	  parv[3] = server is standalone or connected to non-TS only
		 *	  parv[4] = server's idea of UTC time
		 */
		Uplink::Send("SVINFO", 6, 5, 0, Anope::CurTime);
	}

	void SendClientIntroduction(User *u) override
	{
		Uplink::Send("UID", u->nick, 1, u->timestamp, "+" + u->GetModes(), u->GetIdent(), u->host, "255.255.255.255", u->GetUID(), 0, u->host, u->realname);
	}

	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &modes, const std::vector<Anope::string> &values) override
	{
		auto params = values;
		params.insert(params.begin(), { "*", "SVSMODE", u->GetUID(), Anope::ToString(u->timestamp), modes });
		Uplink::SendInternal({}, source, "ENCAP", params);
	}

	void SendLogin(User *u, NickAlias *na) override
	{
		Uplink::Send("ENCAP", '*', "SU", u->GetUID(), na->nc->display);
	}

	void SendLogout(User *u) override
	{
		Uplink::Send("ENCAP", '*', "SU", u->GetUID());
	}

	void SendTopic(const MessageSource &source, Channel *c) override
	{
		Uplink::Send(source, "ENCAP", '*', "TOPIC", c->name, c->topic_setter, c->topic_ts, c->topic);
	}

	void SendSVSJoin(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) override
	{
		Uplink::Send(source, "ENCAP", '*', "SVSJOIN", user->GetUID(), chan);
	}

	void SendSVSPart(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) override
	{
		Uplink::Send(source, "ENCAP", '*', "SVSPART", user->GetUID(), chan);
	}

	void SendSASLMessage(const SASL::Message &message) override
	{
		Server *s = Server::Find(message.target.substr(0, 3));
		auto target = s ? s->GetName() : message.target.substr(0, 3);
		if (message.ext.empty())
			Uplink::Send("ENCAP", target, "SASL", message.source, message.target, message.type, message.data);
		else
			Uplink::Send("ENCAP", target, "SASL", message.source, message.target, message.type, message.data, message.ext);
	}

	void SendSVSLogin(const Anope::string &uid, NickAlias *na) override
	{
		Server *s = Server::Find(uid.substr(0, 3));
		Uplink::Send("ENCAP", s ? s->GetName() : uid.substr(0, 3), "SVSLOGIN", uid, '*', '*', na->GetVHostHost().empty() ? "*" : na->GetVHostHost(), na->nc->display);
	}

	void SendSVSNOOP(const Server *server, bool set) override
	{
		Uplink::Send("ENCAP", '*', "SVSNOOP", set ? '+' : '-');
	}
};

struct IRCDMessageEncap final
	: IRCDMessage
{
	IRCDMessageEncap(Module *creator) : IRCDMessage(creator, "ENCAP", 4) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		/*
		 * Received: :dev.anope.de ENCAP * SU DukePyrolator DukePyrolator
		 * params[0] = *
		 * params[1] = SU
		 * params[2] = nickname
		 * params[3] = account
		 */
		if (params[1].equals_cs("SU"))
		{
			User *u = User::Find(params[2]);
			NickCore *nc = NickCore::Find(params[3]);
			if (u && nc)
			{
				u->Login(nc);
			}
		}

		/*
		 * Received: :dev.anope.de ENCAP * CERTFP DukePyrolator :3F122A9CC7811DBAD3566BF2CEC3009007C0868F
		 * params[0] = *
		 * params[1] = CERTFP
		 * params[2] = nickname
		 * params[3] = fingerprint
		 */
		else if (params[1].equals_cs("CERTFP"))
		{
			User *u = User::Find(params[2]);
			if (u)
			{
				u->fingerprint = params[3];
				FOREACH_MOD(OnFingerprint, (u));
			}
		}

		else if (params[1] == "SASL" && SASL::sasl && params.size() >= 6)
		{
			SASL::Message m;
			m.source = params[2];
			m.target = params[3];
			m.type = params[4];
			m.data = params[5];
			m.ext = params.size() > 6 ? params[6] : "";

			SASL::sasl->ProcessMessage(m);
		}

		return;
	}
};

struct IRCDMessagePass final
	: IRCDMessage
{
	IRCDMessagePass(Module *creator) : IRCDMessage(creator, "PASS", 4) { SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		UplinkSID = params[3];
	}
};

struct IRCDMessageServer final
	: IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(FLAG_REQUIRE_SERVER); }

	/*        0          1  2                       */
	/* SERVER hades.arpa 1 :ircd-hybrid test server */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		/* Servers other than our immediate uplink are introduced via SID */
		if (params[1] != "1")
			return;

		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
	}
};

struct IRCDMessageUID final
	: IRCDMessage
{
	IRCDMessageUID(Module *creator) : IRCDMessage(creator, "UID", 11) { SetFlag(FLAG_REQUIRE_SERVER); }

	/*
	   params[0] = nick
	   params[1] = hop
	   params[2] = ts
	   params[3] = modes
	   params[4] = user
	   params[5] = host
	   params[6] = IP
	   params[7] = UID
	   params[8] = services stamp
	   params[9] = realhost
	   params[10] = info
	*/
	// :42X UID Adam 1 1348535644 +aow Adam 192.168.0.5 192.168.0.5 42XAAAAAB 0 192.168.0.5 :Adam
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		/* An IP of 0 means the user is spoofed */
		Anope::string ip = params[6];
		if (ip == "0")
			ip.clear();

		auto ts = IRCD->ExtractTimestamp(params[2]);
		NickAlias *na = NULL;
		if (IRCD->ExtractTimestamp(params[8]) == ts)
			na = NickAlias::Find(params[0]);

		if (params[8] != "0" && !na)
			na = NickAlias::Find(params[8]);

		User::OnIntroduce(params[0], params[4], params[9], params[5], ip, source.GetServer(), params[10], ts, params[3], params[7], na ? *na->nc : NULL);
	}
};

class ProtoPlexus final
	: public Module
{
	Module *m_hybrid;

	PlexusProto ircd_proto;

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

	/* Hybrid message handlers */
	ServiceAlias message_bmask, message_eob, message_join, message_nick, message_sid, message_sjoin,
			message_tburst, message_tmode;

	/* Our message handlers */
	IRCDMessageEncap message_encap;
	IRCDMessagePass message_pass;
	IRCDMessageServer message_server;
	IRCDMessageUID message_uid;

	static void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserModeOperOnly("ADMIN", 'a'));
		ModeManager::AddUserMode(new UserMode("NOCTCP", 'C'));
		ModeManager::AddUserMode(new UserMode("DEAF", 'D'));
		ModeManager::AddUserMode(new UserMode("SOFTCALLERID", 'G'));
		ModeManager::AddUserMode(new UserMode("CALLERID", 'g'));
		ModeManager::AddUserMode(new UserMode("INVIS", 'i'));
		ModeManager::AddUserMode(new UserModeOperOnly("LOCOPS", 'l'));
		ModeManager::AddUserMode(new UserModeOperOnly("OPER", 'o'));
		ModeManager::AddUserMode(new UserModeOperOnly("NETADMIN", 'N'));
		ModeManager::AddUserMode(new UserMode("PRIV", 'p'));
		ModeManager::AddUserMode(new UserModeOperOnly("ROUTING", 'q'));
		ModeManager::AddUserMode(new UserModeNoone("REGISTERED", 'r'));
		ModeManager::AddUserMode(new UserMode("REGPRIV", 'R'));
		ModeManager::AddUserMode(new UserModeOperOnly("SNOMASK", 's'));
		ModeManager::AddUserMode(new UserModeNoone("SSL", 'S'));
		ModeManager::AddUserMode(new UserModeNoone("PROTECTED", 'U'));
		ModeManager::AddUserMode(new UserMode("WALLOPS", 'w'));
		ModeManager::AddUserMode(new UserModeNoone("WEBIRC", 'W'));
		ModeManager::AddUserMode(new UserMode("CLOAK", 'x'));
		ModeManager::AddUserMode(new UserModeOperOnly("OPERWALLS", 'z'));

		/* b/e/I */
		ModeManager::AddChannelMode(new ChannelModeList("BAN", 'b'));
		ModeManager::AddChannelMode(new ChannelModeList("EXCEPT", 'e'));
		ModeManager::AddChannelMode(new ChannelModeList("INVITEOVERRIDE", 'I'));

		/* v/h/o/a/q */
		ModeManager::AddChannelMode(new ChannelModeStatus("VOICE", 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus("HALFOP", 'h', '%', 1));
		ModeManager::AddChannelMode(new ChannelModeStatus("OP", 'o', '@', 2));
		ModeManager::AddChannelMode(new ChannelModeStatus("PROTECT", 'a', '&', 3));
		ModeManager::AddChannelMode(new ChannelModeStatus("OWNER", 'q', '~', 4));

		/* l/k */
		ModeManager::AddChannelMode(new ChannelModeParam("LIMIT", 'l', true));
		ModeManager::AddChannelMode(new ChannelModeKey('k'));

		/* Add channel modes */
		ModeManager::AddChannelMode(new ChannelMode("BANDWIDTH", 'B'));
		ModeManager::AddChannelMode(new ChannelMode("NOCTCP", 'C'));
		ModeManager::AddChannelMode(new ChannelMode("BLOCKCOLOR", 'c'));
		ModeManager::AddChannelMode(new ChannelMode("INVITE", 'i'));
		ModeManager::AddChannelMode(new ChannelMode("MODERATED", 'm'));
		ModeManager::AddChannelMode(new ChannelMode("REGMODERATED", 'M'));
		ModeManager::AddChannelMode(new ChannelMode("NOEXTERNAL", 'n'));
		ModeManager::AddChannelMode(new ChannelMode("NONOTICE", 'N'));
		ModeManager::AddChannelMode(new ChannelMode("PRIVATE", 'p'));
		ModeManager::AddChannelMode(new ChannelMode("SECRET", 's'));
		ModeManager::AddChannelMode(new ChannelMode("TOPIC", 't'));
		ModeManager::AddChannelMode(new ChannelModeOperOnly("OPERONLY", 'O'));
		ModeManager::AddChannelMode(new ChannelMode("REGISTEREDONLY", 'R'));
		ModeManager::AddChannelMode(new ChannelMode("SSL", 'S'));
		ModeManager::AddChannelMode(new ChannelMode("PERM", 'z'));
	}

public:
	ProtoPlexus(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR),
		ircd_proto(this),
		message_away(this), message_capab(this), message_error(this), message_invite(this), message_kick(this), message_kill(this),
		message_mode(this), message_motd(this), message_notice(this), message_part(this), message_ping(this), message_privmsg(this),
		message_quit(this), message_squit(this), message_stats(this), message_time(this), message_topic(this), message_version(this),
		message_whois(this),

		message_bmask("IRCDMessage", "plexus/bmask", "hybrid/bmask"), message_eob("IRCDMessage", "plexus/eob", "hybrid/eob"),
		message_join("IRCDMessage", "plexus/join", "hybrid/join"), message_nick("IRCDMessage", "plexus/nick", "hybrid/nick"),
		message_sid("IRCDMessage", "plexus/sid", "hybrid/sid"),
		message_sjoin("IRCDMessage", "plexus/sjoin", "hybrid/sjoin"), message_tburst("IRCDMessage", "plexus/tburst", "hybrid/tburst"),
		message_tmode("IRCDMessage", "plexus/tmode", "hybrid/tmode"),

		message_encap(this), message_pass(this), message_server(this), message_uid(this)
	{

		if (ModuleManager::LoadModule("hybrid", User::Find(creator)) != MOD_ERR_OK)
			throw ModuleException("Unable to load hybrid");
		m_hybrid = ModuleManager::FindModule("hybrid");
		if (!m_hybrid)
			throw ModuleException("Unable to find hybrid");
		if (!hybrid)
			throw ModuleException("No protocol interface for hybrid");

		this->AddModes();
	}

	~ProtoPlexus() override
	{
		m_hybrid = ModuleManager::FindModule("hybrid");
		ModuleManager::UnloadModule(m_hybrid, NULL);
	}
};

MODULE_INIT(ProtoPlexus)
