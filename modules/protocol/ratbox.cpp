/*
 * Anope IRC Services
 *
 * Copyright (C) 2005-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"

static Anope::string UplinkSID;

class RatboxProto : public IRCDProto
{
	ServiceReference<IRCDProto> hybrid; // XXX
 public:
	RatboxProto(Module *creator) : IRCDProto(creator, "Ratbox 3.0+")
		, hybrid("hybrid")
	{
		DefaultPseudoclientModes = "+oiS";
		CanSNLine = true;
		CanSQLine = true;
		CanSZLine = true;
		RequiresID = true;
		MaxModes = 4;
	}

	void SendSVSKillInternal(const MessageSource &source, User *targ, const Anope::string &reason) override { hybrid->SendSVSKillInternal(source, targ, reason); }
	void SendGlobalNotice(ServiceBot *bi, const Server *dest, const Anope::string &msg) override { hybrid->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(ServiceBot *bi, const Server *dest, const Anope::string &msg) override { hybrid->SendGlobalPrivmsg(bi, dest, msg); }
	void SendSQLine(User *u, XLine *x) override { hybrid->SendSQLine(u, x); }
	void SendSGLine(User *u, XLine *x) override { hybrid->SendSGLine(u, x); }
	void SendSGLineDel(XLine *x) override { hybrid->SendSGLineDel(x); }
	void SendAkill(User *u, XLine *x) override { hybrid->SendAkill(u, x); }
	void SendAkillDel(XLine *x) override { hybrid->SendAkillDel(x); }
	void SendSQLineDel(XLine *x) override { hybrid->SendSQLineDel(x); }
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override { hybrid->SendJoin(user, c, status); }
	void SendServer(const Server *server) override { hybrid->SendServer(server); }
	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &buf) override { hybrid->SendModeInternal(source, u, buf); }
	void SendChannel(Channel *c) override { hybrid->SendChannel(c); }
	bool IsIdentValid(const Anope::string &ident) override { return hybrid->IsIdentValid(ident); }

	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) override
	{
		Uplink::Send(source, "OPERWALL", buf);
	}

	void SendConnect() override
	{
		Uplink::Send("PASS", Config->Uplinks[Anope::CurrentUplink].password, "TS", 6, Me->GetSID());
		
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
		Uplink::Send("CAPAB", "QS EX CHW IE GLN TB ENCAP");
		
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
		Uplink::Send("SVINFO", 6, 6, 0, Anope::CurTime);
	}

	void SendClientIntroduction(User *u) override
	{
		Anope::string modes = "+" + u->GetModes();
		Uplink::Send(Me, "UID", u->nick, 1, u->timestamp, modes, u->GetIdent(), u->host, 0, u->GetUID(), u->realname);
	}

	void SendLogin(User *u, NickServ::Nick *na) override
	{
		if (na->GetAccount()->HasFieldS("UNCONFIRMED"))
			return;

		Uplink::Send(Me, "ENCAP", "*", "SU", u->GetUID(), na->GetAccount()->GetDisplay());
	}

	void SendLogout(User *u) override
	{
		Uplink::Send(Me, "ENCAP", "*", "SU", u->GetUID());
	}

	void SendTopic(const MessageSource &source, Channel *c) override
	{
		ServiceBot *bi = source.GetBot();
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (params[1] == "LOGIN" || params[1] == "SU")
		{
			User *u = source.GetUser();

			NickServ::Account *nc = NickServ::FindAccount(params[2]);
			if (!nc)
				return;
			u->Login(nc);

			/* Sometimes a user connects, we send them the usual "this nickname is registered" mess (if
			 * their server isn't syncing) and then we receive this.. so tell them about it.
			 */
			if (u->server->IsSynced())
				u->SendMessage(Config->GetClient("NickServ"), _("You have been logged in as \002%s\002."), nc->GetDisplay().c_str());
		}
	}
};

struct IRCDMessagePass : IRCDMessage
{
	IRCDMessagePass(Module *creator) : IRCDMessage(creator, "PASS", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		UplinkSID = params[3];
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	// SERVER hades.arpa 1 :ircd-ratbox test server
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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
	 * params[3] = topic if params[2] isn't the topic
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		time_t topic_time = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime;
		Channel *c = Channel::Find(params[0]);

		if (!c)
			return;

		const Anope::string &setter = params.size() == 4 ? params[2] : "",
			topic = params.size() == 4 ? params[3] : params[2];

		c->ChangeTopicInternal(NULL, setter, topic, topic_time);
	}
};

struct IRCDMessageUID : IRCDMessage
{
	IRCDMessageUID(Module *creator) : IRCDMessage(creator, "UID", 9) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	// :42X UID Adam 1 1348535644 +aow Adam 192.168.0.5 192.168.0.5 42XAAAAAB :Adam
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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

 public:
	ProtoRatbox(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR)
		, ircd_proto(this)
		, message_away(this)
		, message_capab(this)
		, message_error(this)
		, message_invite(this)
		, message_kick(this)
		, message_kill(this)
		, message_mode(this)
		, message_motd(this)
		, message_notice(this)
		, message_part(this)
		, message_ping(this)
		, message_privmsg(this)
		, message_quit(this)
		, message_squit(this)
		, message_stats(this)
		, message_time(this)
		, message_topic(this)
		, message_version(this)
		, message_whois(this)

		, message_bmask("IRCDMessage", "ratbox/bmask", "hybrid/bmask")
		, message_join("IRCDMessage", "ratbox/join", "hybrid/join")
		, message_nick("IRCDMessage", "ratbox/nick", "hybrid/nick")
		, message_pong("IRCDMessage", "ratbox/pong", "hybrid/pong")
		, message_sid("IRCDMessage", "ratbox/sid", "hybrid/sid")
		, message_sjoin("IRCDMessage", "ratbox/sjoin", "hybrid/sjoin")
		, message_tmode("IRCDMessage", "ratbox/tmode", "hybrid/tmode")

		, message_encap(this)
		, message_pass(this)
		, message_server(this)
		, message_tburst(this)
		, message_uid(this)
	{

		if (ModuleManager::LoadModule("hybrid", User::Find(creator)) != MOD_ERR_OK)
			throw ModuleException("Unable to load hybrid");
		m_hybrid = ModuleManager::FindModule("hybrid");
		if (!m_hybrid)
			throw ModuleException("Unable to find hybrid");
#warning ""
//		if (!hybrid)
//			throw ModuleException("No protocol interface for hybrid");
	}

	~ProtoRatbox()
	{
		m_hybrid = ModuleManager::FindModule("hybrid");
		ModuleManager::UnloadModule(m_hybrid, NULL);
	}
};

MODULE_INIT(ProtoRatbox)
