/* Plexus 3+ IRCD functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

static Anope::string UplinkSID;

static ServiceReference<IRCDProto> hybrid("IRCDProto", "hybrid");

class PlexusProto : public IRCDProto
{
 public:
	PlexusProto(Module *creator) : IRCDProto(creator, "hybrid-7.2.3+plexus-3.0.1")
	{
		DefaultPseudoclientModes = "+oiU";
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

	void SendSVSKillInternal(const MessageSource &source, User *targ, const Anope::string &reason) anope_override { hybrid->SendSVSKillInternal(source, targ, reason); }
	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { hybrid->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { hybrid->SendGlobalPrivmsg(bi, dest, msg); }
	void SendSQLine(User *u, const XLine *x) anope_override { hybrid->SendSQLine(u, x); }
	void SendSQLineDel(const XLine *x) anope_override { hybrid->SendSQLineDel(x); }
	void SendSGLineDel(const XLine *x) anope_override { hybrid->SendSGLineDel(x); }
	void SendSGLine(User *u, const XLine *x) anope_override { hybrid->SendSGLine(u, x); }
	void SendAkillDel(const XLine *x) anope_override { hybrid->SendAkillDel(x); }
	void SendAkill(User *u, XLine *x) anope_override { hybrid->SendAkill(u, x); }
	void SendServer(const Server *server) anope_override { hybrid->SendServer(server); }
	void SendChannel(Channel *c) anope_override { hybrid->SendChannel(c); }
	void SendSVSHold(const Anope::string &nick, time_t t) anope_override { hybrid->SendSVSHold(nick, t); }
	void SendSVSHoldDel(const Anope::string &nick) anope_override { hybrid->SendSVSHoldDel(nick); }

	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "OPERWALL :" << buf;
	}

	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		UplinkSocket::Message(Me) << "SJOIN " << c->creation_time << " " << c->name << " +" << c->GetModes(true, true) << " :" << user->GetUID();
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
			for (size_t i = 0; i < cs.Modes().length(); ++i)
				c->SetMode(setter, ModeManager::FindChannelModeByChar(cs.Modes()[i]), user->GetUID(), false);

			if (uc != NULL)
				uc->status = cs;
		}
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP " << u->server->GetName() << " SVSNICK " << u->GetUID() << " " << u->timestamp << " " << newnick << " " << when;
	}

	void SendVhost(User *u, const Anope::string &ident, const Anope::string &host) anope_override
	{
		if (!ident.empty())
			UplinkSocket::Message(Me) << "ENCAP * CHGIDENT " << u->GetUID() << " " << ident;
		UplinkSocket::Message(Me) << "ENCAP * CHGHOST " << u->GetUID() << " " << host;
		u->SetMode(Config->GetClient("HostServ"), "CLOAK");
	}

	void SendVhostDel(User *u) anope_override
	{
		u->RemoveMode(Config->GetClient("HostServ"), "CLOAK");
	}

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "PASS " << Config->Uplinks[Anope::CurrentUplink].password << " TS 6 :" << Me->GetSID();
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
		 * ENCAP  - Supports encapsulization of protocol messages
		 * SVS    - Supports services protocol extensions
		 */
		UplinkSocket::Message() << "CAPAB :QS EX CHW IE EOB KLN UNKLN GLN HUB KNOCK TBURST PARA ENCAP SVS";
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
		UplinkSocket::Message() << "SVINFO 6 5 0 :" << Anope::CurTime;
	}

	void SendClientIntroduction(User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "UID " << u->nick << " 1 " << u->timestamp << " " << modes << " " << u->GetIdent() << " " << u->host << " 255.255.255.255 " << u->GetUID() << " 0 " << u->host << " :" << u->realname;
	}

	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "ENCAP * SVSMODE " << u->GetUID() << " " << u->timestamp << " " << buf;
	}

	void SendLogin(User *u, NickAlias *na) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP * SU " << u->GetUID() << " " << na->nc->display;
	}

	void SendLogout(User *u) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP * SU " << u->GetUID();
	}

	void SendTopic(const MessageSource &source, Channel *c) anope_override
	{
		UplinkSocket::Message(source) << "ENCAP * TOPIC " << c->name << " " << c->topic_setter << " " << c->topic_ts << " :" << c->topic;
	}

	void SendSVSJoin(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) anope_override
	{
		UplinkSocket::Message(source) << "ENCAP " << user->server->GetName() << " SVSJOIN " << user->GetUID() << " " << chan;
	}

	void SendSVSPart(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) anope_override
	{
		UplinkSocket::Message(source) << "ENCAP " << user->server->GetName() << " SVSPART " << user->GetUID() << " " << chan;
	}
};

struct IRCDMessageEncap : IRCDMessage
{
	IRCDMessageEncap(Module *creator) : IRCDMessage(creator, "ENCAP", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
		return;
	}
};

struct IRCDMessagePass : IRCDMessage
{
	IRCDMessagePass(Module *creator) : IRCDMessage(creator, "PASS", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		UplinkSID = params[3];
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*        0          1  2                       */
	/* SERVER hades.arpa 1 :ircd-hybrid test server */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* Servers other than our immediate uplink are introduced via SID */
		if (params[1] != "1")
			return;

		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
	}
};

struct IRCDMessageUID : IRCDMessage
{
	IRCDMessageUID(Module *creator) : IRCDMessage(creator, "UID", 11) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* An IP of 0 means the user is spoofed */
		Anope::string ip = params[6];
		if (ip == "0")
			ip.clear();

		time_t ts;
		try
		{
			ts = convertTo<time_t>(params[2]);
		}
		catch (const ConvertException &)
		{
			ts = Anope::CurTime;
		}

		NickAlias *na = NULL;
		try
		{
			if (params[8].is_pos_number_only() && convertTo<time_t>(params[8]) == ts)
				na = NickAlias::Find(params[0]);
		}
		catch (const ConvertException &) { }
		if (params[8] != "0" && !na)
			na = NickAlias::Find(params[8]);

		User::OnIntroduce(params[0], params[4], params[9], params[5], ip, source.GetServer(), params[10], ts, params[3], params[7], na ? *na->nc : NULL);
	}
};

class ProtoPlexus : public Module
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

	void AddModes()
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
		ModeManager::AddUserMode(new UserMode("WALLOPS", 'w'));
		ModeManager::AddUserMode(new UserModeNoone("WEBIRC", 'W'));
		ModeManager::AddUserMode(new UserMode("CLOAK", 'x'));
		ModeManager::AddUserMode(new UserModeOperOnly("OPERWALLS", 'z'));

		/* b/e/I */
		ModeManager::AddChannelMode(new ChannelModeList("BAN", 'b'));
		ModeManager::AddChannelMode(new ChannelModeList("EXCEPT", 'e'));
		ModeManager::AddChannelMode(new ChannelModeList("INVITEOVERRIDE", 'I'));
		ModeManager::AddUserMode(new UserModeNoone("PROTECTED", 'U'));

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
		ModeManager::AddChannelMode(new ChannelMode("REGMODERATED", 'M'));
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

	~ProtoPlexus()
	{
		m_hybrid = ModuleManager::FindModule("hybrid");
		ModuleManager::UnloadModule(m_hybrid, NULL);
	}
};

MODULE_INIT(ProtoPlexus)
