/* Ratbox IRCD functions
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

static Anope::string UplinkSID;

static ServiceReference<IRCDProto> hybrid("IRCDProto", "hybrid");

class RatboxProto : public IRCDProto
{
 public:
	RatboxProto(Module *creator) : IRCDProto(creator, "Ratbox 3.0+")
	{
		DefaultPseudoclientModes = "+oiS";
		CanSNLine = true;
		CanSQLine = true;
		CanSZLine = true;
		RequiresID = true;
		MaxModes = 4;
	}

	void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { hybrid->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { hybrid->SendGlobalPrivmsg(bi, dest, msg); }
	void SendGlobopsInternal(const BotInfo *source, const Anope::string &buf) anope_override { hybrid->SendGlobopsInternal(source, buf); }
	void SendSQLine(User *u, const XLine *x) anope_override { hybrid->SendSQLine(u, x); }
	void SendSGLine(User *u, const XLine *x) anope_override { hybrid->SendSGLine(u, x); }
	void SendSGLineDel(const XLine *x) anope_override { hybrid->SendSGLineDel(x); }
	void SendAkill(User *u, XLine *x) anope_override { hybrid->SendAkill(u, x); }
	void SendAkillDel(const XLine *x) anope_override { hybrid->SendAkillDel(x); }
	void SendSQLineDel(const XLine *x) anope_override { hybrid->SendSQLineDel(x); }
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override { hybrid->SendJoin(user, c, status); }
	void SendServer(const Server *server) anope_override { hybrid->SendServer(server); }
	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf) anope_override { hybrid->SendModeInternal(bi, u, buf); }
	void SendChannel(Channel *c) anope_override { hybrid->SendChannel(c); }
	void SendTopic(BotInfo *bi, Channel *c) anope_override { hybrid->SendTopic(bi, c); }

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "PASS " << Config->Uplinks[Anope::CurrentUplink]->password << " TS 6 :" << Me->GetSID();
		/*
		  QS     - Can handle quit storm removal
		  EX     - Can do channel +e exemptions
		  CHW    - Can do channel wall @#
		  IE     - Can do invite exceptions
		  GLN    - Can do GLINE message
		  KNOCK  - supports KNOCK
		  TB     - supports topic burst
		  ENCAP  - supports ENCAP
		*/
		UplinkSocket::Message() << "CAPAB :QS EX CHW IE GLN TB ENCAP";
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
		UplinkSocket::Message() << "SVINFO 6 3 0 :" << Anope::CurTime;
	}

	void SendClientIntroduction(const User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "UID " << u->nick << " 1 " << u->timestamp << " " << modes << " " << u->GetIdent() << " " << u->host << " 0 " << u->GetUID() << " :" << u->realname;
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
};

struct IRCDMessageEncap : IRCDMessage
{
	IRCDMessageEncap(Module *creator) : IRCDMessage(creator, "ENCAP", 3) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	// Debug: Received: :00BAAAAAB ENCAP * LOGIN Adam
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params[1] == "LOGIN" || params[1] == "SU")
		{
			User *u = source.GetUser();

			NickCore *nc = NickCore::Find(params[2]);
			if (!nc)
				return;
			u->Login(nc);

			const NickAlias *user_na = NickAlias::Find(u->nick);
			if (!Config->NoNicknameOwnership && user_na && user_na->nc == nc && user_na->nc->HasExt("UNCONFIRMED") == false)
				u->SetMode(NickServ, "REGISTERED");
		}
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

	// SERVER hades.arpa 1 :ircd-ratbox test server
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		// Servers other then our immediate uplink are introduced via SID
		if (params[1] != "1")
			return;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
		IRCD->SendPing(Config->ServerName, params[0]);
	}
};

struct IRCDMessageTBurst : IRCDMessage
{
	IRCDMessageTBurst(Module *creator) : IRCDMessage(creator, "TB", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	/*
	 * params[0] = channel
	 * params[1] = ts
	 * params[2] = topic OR who set the topic
	 * params[3] = topic if params[2] isnt the topic
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		time_t topic_time = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime;
		Channel *c = Channel::Find(params[0]);

		if (!c)
			return;

		const Anope::string &setter = params.size() == 4 ? params[2] : "",
			topic = params.size() == 4 ? params[3] : params[2];

		c->ChangeTopicInternal(setter, topic, topic_time);
	}
};

struct IRCDMessageUID : IRCDMessage
{
	IRCDMessageUID(Module *creator) : IRCDMessage(creator, "UID", 9) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	// :42X UID Adam 1 1348535644 +aow Adam 192.168.0.5 192.168.0.5 42XAAAAAB :Adam
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* Source is always the server */
		new User(params[0], params[4], params[5], "", params[6], source.GetServer(), params[8], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0, params[3], params[7]);
	}
};

class ProtoRatbox : public Module
{
	Module *m_hybrid;

	RatboxProto ircd_proto;

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
	ServiceAlias message_bmask, message_join, message_nick, message_pong, message_sid,
			message_sjoin, message_tmode;

	/* Our message handlers */
	IRCDMessageEncap message_encap;
	IRCDMessagePass message_pass;
	IRCDMessageServer message_server;
	IRCDMessageTBurst message_tburst;
	IRCDMessageUID message_uid;

	void AddModes()
	{
		/* user modes */
		ModeManager::RemoveUserMode(ModeManager::FindUserModeByName("HIDEOPER"));
		ModeManager::RemoveUserMode(ModeManager::FindUserModeByName("REGPRIV"));

		/* v/h/o/a/q */
		ModeManager::RemoveChannelMode(ModeManager::FindChannelModeByName("HALFOP"));

		/* channel modes */
		ModeManager::RemoveChannelMode(ModeManager::FindChannelModeByName("REGISTERED"));
		ModeManager::RemoveChannelMode(ModeManager::FindChannelModeByName("OPERONLY"));
		ModeManager::RemoveChannelMode(ModeManager::FindChannelModeByName("REGISTEREDONLY"));
		ModeManager::RemoveChannelMode(ModeManager::FindChannelModeByName("SSL"));
	}

 public:
	ProtoRatbox(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL),
		ircd_proto(this),
		message_away(this), message_capab(this), message_error(this), message_kick(this), message_kill(this),
		message_mode(this), message_motd(this), message_part(this), message_ping(this), message_privmsg(this),
		message_quit(this), message_squit(this), message_stats(this), message_time(this), message_topic(this),
		message_version(this), message_whois(this),

		message_bmask("IRCDMessage", "ratbox/bmask", "hybrid/bmask"), message_join("IRCDMessage", "ratbox/join", "hybrid/join"),
		message_nick("IRCDMessage", "ratbox/nick", "hybrid/nick"), message_pong("IRCDMessage", "ratbox/pong", "hybrid/pong"),
		message_sid("IRCDMessage", "ratbox/sid", "hybrid/sid"), message_sjoin("IRCDMessage", "ratbox/sjoin", "hybrid/sjoin"),
		message_tmode("IRCDMessage", "ratbox/tmode", "hybrid/tmode"),

		message_encap(this), message_pass(this), message_server(this), message_tburst(this), message_uid(this)
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

	~ProtoRatbox()
	{
		ModuleManager::UnloadModule(m_hybrid, NULL);
	}
};

MODULE_INIT(ProtoRatbox)
