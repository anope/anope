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

/* Dependencies: anope_protocol.hybrid */

#include "module.h"
#include "modules/protocol/hybrid.h"
#include "modules/protocol/ratbox.h"

static Anope::string UplinkSID;

class RatboxProto : public IRCDProto
{
	ServiceReference<IRCDProto> hybrid; // XXX

	ServiceBot *FindIntroduced()
	{
		ServiceBot *bi = Config->GetClient("OperServ");
		if (bi && bi->introduced)
			return bi;

		for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
		{
			User *u = it->second;
			if (u->type == UserType::BOT)
			{
				bi = anope_dynamic_static_cast<ServiceBot *>(u);
				if (bi->introduced)
					return bi;
			}
		}

		return NULL;
	}

 public:
	RatboxProto(Module *creator) : IRCDProto(creator, "Ratbox 3.0+")
		, hybrid("hybrid")
	{
		DefaultPseudoclientModes = "+oiS";
		CanSNLine = true;
		CanSQLine = true;
		CanSQLineChannel = true;
		CanSZLine = true;
		RequiresID = true;
		MaxModes = 4;
	}

	void SendSVSKillInternal(const MessageSource &source, User *targ, const Anope::string &reason) override { hybrid->SendSVSKillInternal(source, targ, reason); }
	void SendGlobalNotice(ServiceBot *bi, const Server *dest, const Anope::string &msg) override { hybrid->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(ServiceBot *bi, const Server *dest, const Anope::string &msg) override { hybrid->SendGlobalPrivmsg(bi, dest, msg); }
	void SendSGLine(User *u, XLine *x) override { hybrid->SendSGLine(u, x); }
	void SendSGLineDel(XLine *x) override { hybrid->SendSGLineDel(x); }
	void SendAkill(User *u, XLine *x) override { hybrid->SendAkill(u, x); }
	void SendAkillDel(XLine *x) override { hybrid->SendAkillDel(x); }
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
		if (na->GetAccount()->IsUnconfirmed())
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

	void SendSQLine(User *, XLine *x) override
	{
		/* Calculate the time left before this would expire, capping it at 2 days */
		time_t timeleft = x->GetExpires() - Anope::CurTime;

		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;

		Uplink::Send(FindIntroduced(), "ENCAP", "*", "RESV", timeleft, x->GetMask(), 0, x->GetReason());
	}

	void SendSQLineDel(XLine *x) override
	{
		Uplink::Send(Config->GetClient("OperServ"), "ENCAP", "*", "UNRESV", x->GetMask());
	}

};

// Debug: Received: :00BAAAAAB ENCAP * LOGIN Adam
void ratbox::Encap::Run(MessageSource &source, const std::vector<Anope::string> &params)
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

void ratbox::Join::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	if (params.size() == 1 && params[0] == "0")
		return rfc1459::Join::Run(source, params);

	if (params.size() < 2)
		return;

	std::vector<Anope::string> p = params;
	p.erase(p.begin());

	return rfc1459::Join::Run(source, p);
}

struct IRCDMessagePass : IRCDMessage
{
	IRCDMessagePass(Module *creator) : IRCDMessage(creator, "PASS", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		UplinkSID = params[3];
	}
};

// SERVER hades.arpa 1 :ircd-ratbox test server
void ratbox::Server::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	// Servers other then our immediate uplink are introduced via SID
	if (params[1] != "1")
		return;
	new ::Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
	IRCD->SendPing(Me->GetName(), params[0]);
}

/*
 * params[0] = channel
 * params[1] = ts
 * params[2] = topic OR who set the topic
 * params[3] = topic if params[2] isn't the topic
 */
void ratbox::TB::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	time_t topic_time = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime;
	Channel *c = Channel::Find(params[0]);

	if (!c)
		return;

	const Anope::string &setter = params.size() == 4 ? params[2] : "",
		topic = params.size() == 4 ? params[3] : params[2];

	c->ChangeTopicInternal(NULL, setter, topic, topic_time);
}

// :42X UID Adam 1 1348535644 +aow Adam 192.168.0.5 192.168.0.5 42XAAAAAB :Adam
void ratbox::UID::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	/* Source is always the server */
	User::OnIntroduce(params[0], params[4], params[5], "", params[6], source.GetServer(), params[8], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0, params[3], params[7], NULL);
}

class ProtoRatbox : public Module
{
	RatboxProto ircd_proto;

	/* Core message handlers */
	rfc1459::Away message_away;
	rfc1459::Capab message_capab;
	rfc1459::Error message_error;
	rfc1459::Invite message_invite;
	rfc1459::Kick message_kick;
	rfc1459::Kill message_kill;
	rfc1459::Mode message_mode;
	rfc1459::MOTD message_motd;
	rfc1459::Notice message_notice;
	rfc1459::Part message_part;
	rfc1459::Ping message_ping;
	rfc1459::Privmsg message_privmsg;
	rfc1459::Quit message_quit;
	rfc1459::SQuit message_squit;
	rfc1459::Stats message_stats;
	rfc1459::Time message_time;
	rfc1459::Topic message_topic;
	rfc1459::Version message_version;
	rfc1459::Whois message_whois;

	/* Our message handlers */
	hybrid::BMask message_bmask;
	ratbox::Encap message_encap;
	ratbox::Join message_join;
	hybrid::Nick message_nick;
	IRCDMessagePass message_pass;
	hybrid::Pong message_pong;
	ratbox::Server message_server;
	hybrid::SID message_sid;
	hybrid::SJoin message_sjoin;
	ratbox::TB message_tb;
	hybrid::TMode message_tmode;
	ratbox::UID message_uid;

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

		, message_bmask(this)
		, message_encap(this)
		, message_join(this)
		, message_nick(this)
		, message_pass(this)
		, message_pong(this)
		, message_server(this)
		, message_sid(this)
		, message_sjoin(this)
		, message_tb(this)
		, message_tmode(this)
		, message_uid(this)
	{
	}
};

template<> void ModuleInfo<ProtoRatbox>(ModuleDef *def)
{
	def->Depends("hybrid");
}

MODULE_INIT(ProtoRatbox)
