/* Ratbox IRCD functions
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

	void SendSVSKillInternal(const MessageSource &source, User *targ, const Anope::string &reason) anope_override { hybrid->SendSVSKillInternal(source, targ, reason); }
	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { hybrid->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { hybrid->SendGlobalPrivmsg(bi, dest, msg); }
	void SendSQLine(User *u, const XLine *x) anope_override { hybrid->SendSQLine(u, x); }
	void SendSGLine(User *u, const XLine *x) anope_override { hybrid->SendSGLine(u, x); }
	void SendSGLineDel(const XLine *x) anope_override { hybrid->SendSGLineDel(x); }
	void SendAkill(User *u, XLine *x) anope_override { hybrid->SendAkill(u, x); }
	void SendAkillDel(const XLine *x) anope_override { hybrid->SendAkillDel(x); }
	void SendSQLineDel(const XLine *x) anope_override { hybrid->SendSQLineDel(x); }
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override { hybrid->SendJoin(user, c, status); }
	void SendServer(const Server *server) anope_override { hybrid->SendServer(server); }
	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &buf) anope_override { hybrid->SendModeInternal(source, u, buf); }
	void SendChannel(Channel *c) anope_override { hybrid->SendChannel(c); }
	bool IsIdentValid(const Anope::string &ident) anope_override { return hybrid->IsIdentValid(ident); }

	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "OPERWALL :" << buf;
	}

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "PASS " << Config->Uplinks[Anope::CurrentUplink].password << " TS 6 :" << Me->GetSID();
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

	void SendClientIntroduction(User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "UID " << u->nick << " 1 " << u->timestamp << " " << modes << " " << u->GetIdent() << " " << u->host << " 0 " << u->GetUID() << " :" << u->realname;
	}

	void SendLogin(User *u, NickAlias *na) anope_override
	{
		if (na->nc->HasExt("UNCONFIRMED"))
			return;

		UplinkSocket::Message(Me) << "ENCAP * SU " << u->GetUID() << " " << na->nc->display;
	}

	void SendLogout(User *u) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP * SU " << u->GetUID();
	}

	void SendTopic(const MessageSource &source, Channel *c) anope_override
	{
		BotInfo *bi = source.GetBot();
		bool needjoin = c->FindUser(bi) == NULL;

		if (needjoin)
		{
			ChannelStatus status;

			status.AddMode('o');
			bi->Join(c, &status);
		}

		IRCDProto::SendTopic(source, c);

		if (needjoin)
			bi->Part(c);
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

			/* Sometimes a user connects, we send them the usual "this nickname is registered" mess (if
			 * their server isn't syncing) and then we receive this.. so tell them about it.
			 */
			if (u->server->IsSynced())
				u->SendMessage(Config->GetClient("NickServ"), _("You have been logged in as \002%s\002."), nc->display.c_str());
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
		IRCD->SendPing(Me->GetName(), params[0]);
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
		User::OnIntroduce(params[0], params[4], params[5], "", params[6], source.GetServer(), params[8], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0, params[3], params[7], NULL);
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
		ModeManager::AddUserMode(new UserModeOperOnly("ADMIN", 'a'));
		ModeManager::AddUserMode(new UserModeOperOnly("BOT", 'b'));
		// c/C = con
		// d = debug?
		ModeManager::AddUserMode(new UserMode("DEAF", 'D'));
		// f = full?
		ModeManager::AddUserMode(new UserMode("CALLERID", 'g'));
		ModeManager::AddUserMode(new UserMode("INVIS", 'i'));
		// k = skill?
		ModeManager::AddUserMode(new UserModeOperOnly("LOCOPS", 'l'));
		// n = nchange
		ModeManager::AddUserMode(new UserModeOperOnly("OPER", 'o'));
		// r = rej
		ModeManager::AddUserMode(new UserModeOperOnly("SNOMASK", 's'));
		ModeManager::AddUserMode(new UserModeNoone("PROTECTED", 'S'));
		// u = unauth?
		ModeManager::AddUserMode(new UserMode("WALLOPS", 'w'));
		// x = external?
		// y = spy?
		ModeManager::AddUserMode(new UserModeOperOnly("OPERWALLS", 'z'));
		// Z = spy?

		/* b/e/I */
		ModeManager::AddChannelMode(new ChannelModeList("BAN", 'b'));
		ModeManager::AddChannelMode(new ChannelModeList("EXCEPT", 'e'));
		ModeManager::AddChannelMode(new ChannelModeList("INVITEOVERRIDE", 'I'));

		/* v/h/o/a/q */
		ModeManager::AddChannelMode(new ChannelModeStatus("VOICE", 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus("OP", 'o', '@', 1));

		/* l/k */
		ModeManager::AddChannelMode(new ChannelModeParam("LIMIT", 'l', true));
		ModeManager::AddChannelMode(new ChannelModeKey('k'));

		/* channel modes */
		ModeManager::AddChannelMode(new ChannelMode("INVITE", 'i'));
		ModeManager::AddChannelMode(new ChannelMode("MODERATED", 'm'));
		ModeManager::AddChannelMode(new ChannelMode("NOEXTERNAL", 'n'));
		ModeManager::AddChannelMode(new ChannelMode("PRIVATE", 'p'));
		ModeManager::AddChannelMode(new ChannelMode("REGISTEREDONLY", 'r'));
		ModeManager::AddChannelMode(new ChannelMode("SECRET", 's'));
		ModeManager::AddChannelMode(new ChannelMode("TOPIC", 't'));
		ModeManager::AddChannelMode(new ChannelMode("SSL", 'S'));
	}

 public:
	ProtoRatbox(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR),
		ircd_proto(this),
		message_away(this), message_capab(this), message_error(this), message_invite(this), message_kick(this),
		message_kill(this), message_mode(this), message_motd(this), message_notice(this), message_part(this),
		message_ping(this), message_privmsg(this), message_quit(this), message_squit(this), message_stats(this),
		message_time(this), message_topic(this), message_version(this), message_whois(this),

		message_bmask("IRCDMessage", "ratbox/bmask", "hybrid/bmask"), message_join("IRCDMessage", "ratbox/join", "hybrid/join"),
		message_nick("IRCDMessage", "ratbox/nick", "hybrid/nick"), message_pong("IRCDMessage", "ratbox/pong", "hybrid/pong"),
		message_sid("IRCDMessage", "ratbox/sid", "hybrid/sid"), message_sjoin("IRCDMessage", "ratbox/sjoin", "hybrid/sjoin"),
		message_tmode("IRCDMessage", "ratbox/tmode", "hybrid/tmode"),

		message_encap(this), message_pass(this), message_server(this), message_tburst(this), message_uid(this)
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

	~ProtoRatbox()
	{
		m_hybrid = ModuleManager::FindModule("hybrid");
		ModuleManager::UnloadModule(m_hybrid, NULL);
	}
};

MODULE_INIT(ProtoRatbox)
