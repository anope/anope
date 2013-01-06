/* Plexus 3+ IRCD functions
 *
 * (C) 2003-2012 Anope Team
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

	void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { hybrid->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { hybrid->SendGlobalPrivmsg(bi, dest, msg); }
	void SendGlobopsInternal(const BotInfo *source, const Anope::string &buf) anope_override { hybrid->SendGlobopsInternal(source, buf); }
	void SendSQLine(User *u, const XLine *x) anope_override { hybrid->SendSQLine(u, x); }
	void SendSQLineDel(const XLine *x) anope_override { hybrid->SendSQLineDel(x); }
	void SendSGLineDel(const XLine *x) anope_override { hybrid->SendSGLineDel(x); }
	void SendSGLine(User *u, const XLine *x) anope_override { hybrid->SendSGLine(u, x); }
	void SendAkillDel(const XLine *x) anope_override { hybrid->SendAkillDel(x); }
	void SendAkill(User *u, XLine *x) anope_override { hybrid->SendAkill(u, x); }
	void SendServer(const Server *server) anope_override { hybrid->SendServer(server); }
	void SendChannel(Channel *c) anope_override { hybrid->SendChannel(c); }

	void SendJoin(const User *user, Channel *c, const ChannelStatus *status) anope_override
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
				uc->status.ClearFlags();

			BotInfo *setter = BotInfo::Find(user->nick);
			for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
				if (cs.HasFlag(ModeManager::ChannelModes[i]->name))
					c->SetMode(setter, ModeManager::ChannelModes[i], user->GetUID(), false);
		}
	}

	void SendForceNickChange(const User *u, const Anope::string &newnick, time_t when) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP " << u->server->GetName() << " SVSNICK " << u->GetUID() << " " << u->timestamp << " " << newnick << " " << when;
	}

	void SendVhost(User *u, const Anope::string &ident, const Anope::string &host) anope_override
	{
		if (!ident.empty())
			UplinkSocket::Message(Me) << "ENCAP * CHGIDENT " << u->GetUID() << " " << ident;
		UplinkSocket::Message(Me) << "ENCAP * CHGHOST " << u->GetUID() << " " << host;
	}

	void SendVhostDel(User *u) anope_override
	{
		if (u->HasMode(UMODE_CLOAK))
			u->RemoveMode(HostServ, UMODE_CLOAK);
		else
			this->SendVhost(u, u->GetIdent(), u->chost);
	}

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "PASS " << Config->Uplinks[Anope::CurrentUplink]->password << " TS 6 :" << Me->GetSID();
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

	void SendClientIntroduction(const User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "UID " << u->nick << " 1 " << u->timestamp << " " << modes << " " << u->GetIdent() << " " << u->host << " 255.255.255.255 " << u->GetUID() << " 0 " << u->host << " :" << u->realname;
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(bi) << "ENCAP * SVSMODE " << u->GetUID() << " " << u->timestamp << " " << buf;
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

	void SendTopic(BotInfo *bi, Channel *c) anope_override
	{
		UplinkSocket::Message(bi) << "ENCAP * TOPIC " << c->name << " " << c->topic_setter << " " << c->topic_ts << " :" << c->topic;
	}

	void SendSVSJoin(const BotInfo *source, const User *user, const Anope::string &chan, const Anope::string &param) anope_override
	{
		UplinkSocket::Message(source) << "ENCAP " << user->server->GetSID() << " SVSJOIN " << user->GetUID() << " " << chan;
	}

	void SendSVSPart(const BotInfo *source, const User *user, const Anope::string &chan, const Anope::string &param) anope_override
	{
		UplinkSocket::Message(source) << "ENCAP " << user->server->GetSID() << " SVSPART " << user->GetUID() << " " << chan;
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
			const NickAlias *user_na = NickAlias::Find(params[2]);
			NickCore *nc = NickCore::Find(params[3]);
			if (u && nc)
			{
				u->Login(nc);
				if (!Config->NoNicknameOwnership && user_na && user_na->nc == nc && user_na->nc->HasFlag(NI_UNCONFIRMED) == false)
					u->SetMode(NickServ, UMODE_REGISTERED);
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
				FOREACH_MOD(I_OnFingerprint, OnFingerprint(u));
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
	ServiceReference<NickServService> NSService;

	IRCDMessageUID(Module *creator) : IRCDMessage(creator, "UID", 11), NSService("NickServService", "NickServ") { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

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

		User *user = new User(params[0], params[4], params[9], params[5], ip, source.GetServer(), params[10], ts, params[3], params[7]);
		try
		{
			if (NSService && params[8].is_pos_number_only() && convertTo<time_t>(params[8]) == user->timestamp)
			{
				NickAlias *na = NickAlias::Find(user->nick);
				if (na)
					NSService->Login(user, na);
			}
		}
		catch (const ConvertException &) { }
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
		ModeManager::RemoveUserMode(ModeManager::FindUserModeByName(UMODE_HIDEOPER));
		ModeManager::AddUserMode(new UserMode(UMODE_NOCTCP, 'C'));
		ModeManager::AddUserMode(new UserMode(UMODE_DEAF, 'D'));
		ModeManager::AddUserMode(new UserMode(UMODE_SOFTCALLERID, 'G'));
		ModeManager::AddUserMode(new UserMode(UMODE_NETADMIN, 'N'));
		ModeManager::AddUserMode(new UserMode(UMODE_SSL, 'S'));
		ModeManager::AddUserMode(new UserMode(UMODE_WEBIRC, 'W'));
		ModeManager::AddUserMode(new UserMode(UMODE_CALLERID, 'g'));
		ModeManager::AddUserMode(new UserMode(UMODE_PRIV, 'p'));
		ModeManager::AddUserMode(new UserMode(UMODE_CLOAK, 'x'));
		ModeManager::AddUserMode(new UserMode(UMODE_PROTECTED, 'U'));

		/* v/h/o/a/q */
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT, 'a', '&', 3));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, 'q', '~', 4));

		/* Add channel modes */
		ModeManager::RemoveChannelMode(ModeManager::FindChannelModeByName(CMODE_REGISTERED));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_BANDWIDTH, 'B'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_NOCTCP, 'C'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_REGMODERATED, 'M'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_NONOTICE, 'N'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCOLOR, 'c'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_PERM, 'z'));
	}

 public:
	ProtoPlexus(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL),
		ircd_proto(this),
		message_away(this), message_capab(this), message_error(this), message_kick(this), message_kill(this),
		message_mode(this), message_motd(this), message_part(this), message_ping(this), message_privmsg(this), message_quit(this),
		message_squit(this), message_stats(this), message_time(this), message_topic(this), message_version(this), message_whois(this),

		message_bmask("IRCDMessage", "plexus/bmask", "hybrid/bmask"), message_eob("IRCDMessage", "plexus/eob", "hybrid/eob"),
		message_join("IRCDMessage", "plexus/join", "hybrid/join"), message_nick("IRCDMessage", "plexus/nick", "hybrid/nick"),
		message_sid("IRCDMessage", "plexus/sid", "hybrid/sid"),
		message_sjoin("IRCDMessage", "plexus/sjoin", "hybrid/sjoin"), message_tburst("IRCDMessage", "plexus/tburst", "hybrid/tburst"),
		message_tmode("IRCDMessage", "plexus/tmode", "hybrid/tmode"),

		message_encap(this), message_pass(this), message_server(this), message_uid(this)
	{
		this->SetAuthor("Anope");

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
		ModuleManager::UnloadModule(m_hybrid, NULL);
	}
};

MODULE_INIT(ProtoPlexus)
