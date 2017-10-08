/*
 * Anope IRC Services
 *
 * Copyright (C) 2005-2017 Anope Team <team@anope.org>
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

void ratbox::senders::NickIntroduction::Send(User *user)
{
	Anope::string modes = "+" + user->GetModes();
	Uplink::Send(Me, "UID", user->nick, 1, user->timestamp, modes, user->GetIdent(), user->host, 0, user->GetUID(), user->realname);
}

void ratbox::senders::Login::Send(User *u, NickServ::Nick *na)
{
	if (na->GetAccount()->IsUnconfirmed())
		return;

	Uplink::Send(Me, "ENCAP", "*", "SU", u->GetUID(), na->GetAccount()->GetDisplay());
}

void ratbox::senders::Logout::Send(User *u)
{
	Uplink::Send(Me, "ENCAP", "*", "SU", u->GetUID());
}

void ratbox::senders::SQLine::Send(User*, XLine* x)
{
	if (x->IsExpired())
		return;

	/* Calculate the time left before this would expire */
	time_t timeleft = x->GetExpires() > 0 ? x->GetExpires() - Anope::CurTime : 0;
#warning "find introduced"
//	Uplink::Send(FindIntroduced(), "ENCAP", "*", "RESV", timeleft, x->GetMask(), 0, x->GetReason());
}

void ratbox::senders::SQLineDel::Send(XLine* x)
{
	Uplink::Send(Config->GetClient("OperServ"), "ENCAP", "*", "UNRESV", x->GetMask());
}

void ratbox::senders::SVSNick::Send(User* u, const Anope::string& newnick, time_t ts)
{
	Uplink::Send(Me, "ENCAP", u->server->GetName(), "RSFNC", u->GetUID(),
			newnick, ts, u->timestamp);
}

void ratbox::senders::Topic::Send(const MessageSource &source, Channel *channel, const Anope::string &topic, time_t topic_ts, const Anope::string &topic_setter)
{
	ServiceBot *bi = source.GetBot();
	bool needjoin = channel->FindUser(bi) == NULL;

	if (needjoin)
	{
		ChannelStatus status;

		status.AddMode('o');
		bi->Join(channel, &status);
	}

	rfc1459::senders::Topic::Send(source, channel, topic, topic_ts, topic_setter);

	if (needjoin)
		bi->Part(channel);
}

void ratbox::senders::Wallops::Send(const MessageSource &source, const Anope::string &msg)
{
	Uplink::Send(source, "OPERWALL", msg);
}

ServiceBot *ratbox::Proto::FindIntroduced()
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

ratbox::Proto::Proto(Module *creator) : ts6::Proto(creator, "Ratbox 3.0+")
	, hybrid(creator)
{
	DefaultPseudoclientModes = "+oiS";
	CanSVSNick = true;
	CanSNLine = true;
	CanSQLine = true;
	CanSQLineChannel = true;
	CanSZLine = true;
	RequiresID = true;
	MaxModes = 4;
}

void ratbox::Proto::Handshake()
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
	Uplink::Send("SERVER", Me->GetName(), Me->GetHops() + 1, Me->GetDescription());

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
void ratbox::ServerMessage::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	// Servers other then our immediate uplink are introduced via SID
	if (params[1] != "1")
		return;
	new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
	IRCD->Send<messages::Ping>(Me->GetName(), params[0]);
}

/*
 * params[0] = channel
 * params[1] = ts
 * params[2] = topic OR who set the topic
 * params[3] = topic if params[2] isn't the topic
 */
void ratbox::TB::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Channel *c = Channel::Find(params[0]);
	time_t topic_time = Anope::CurTime;

	if (!c)
		return;
	try
	{
		topic_time = convertTo<time_t>(params[1]);
	}
	catch (const ConvertException &) { }

	const Anope::string &setter = params.size() == 4 ? params[2] : "",
		&topic = params.size() == 4 ? params[3] : params[2];

	c->ChangeTopicInternal(NULL, setter, topic, topic_time);
}

// :42X UID Adam 1 1348535644 +aow Adam 192.168.0.5 192.168.0.5 42XAAAAAB :Adam
void ratbox::UID::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	time_t ts = 0;

	try
	{
		ts = convertTo<time_t>(params[2]);
	}
	catch (const ConvertException &) { }

	/* Source is always the server */
	User::OnIntroduce(params[0], params[4], params[5], "", params[6], source.GetServer(), params[8], ts, params[3], params[7], NULL);
}

class ProtoRatbox : public Module
{
	ratbox::Proto ircd_proto;

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
	ratbox::ServerMessage message_server;
	hybrid::SID message_sid;
	hybrid::SJoin message_sjoin;
	ratbox::TB message_tb;
	hybrid::TMode message_tmode;
	ratbox::UID message_uid;

	/* Core message senders */
	rfc1459::senders::Invite sender_invite;
	rfc1459::senders::Kick sender_kick;
	rfc1459::senders::Kill sender_svskill;
	rfc1459::senders::ModeChannel sender_mode_chan;
	rfc1459::senders::NickChange sender_nickchange;
	rfc1459::senders::Notice sender_notice;
	rfc1459::senders::Part sender_part;
	rfc1459::senders::Ping sender_ping;
	rfc1459::senders::Pong sender_pong;
	rfc1459::senders::Privmsg sender_privmsg;
	rfc1459::senders::Quit sender_quit;
	rfc1459::senders::SQuit sender_squit;

	hybrid::senders::Akill sender_akill;
	hybrid::senders::AkillDel sender_akill_del;
	hybrid::senders::MessageChannel sender_channel;
	hybrid::senders::GlobalNotice sender_global_notice;
	hybrid::senders::GlobalPrivmsg sender_global_privmsg;
	hybrid::senders::Join sender_join;
	hybrid::senders::ModeUser sender_mode_user;
	hybrid::senders::MessageServer sender_server;
	hybrid::senders::SGLine sender_sgline;
	hybrid::senders::SGLineDel sender_sgline_del;

	ratbox::senders::Login sender_login;
	ratbox::senders::Logout sender_logout;
	ratbox::senders::NickIntroduction sender_nickintroduction;
	ratbox::senders::SQLine sender_sqline;
	ratbox::senders::SQLineDel sender_sqline_del;
	ratbox::senders::SVSNick sender_svsnick;
	ratbox::senders::Topic sender_topic;
	ratbox::senders::Wallops sender_wallops;

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

		, sender_invite(this)
		, sender_kick(this)
		, sender_svskill(this)
		, sender_mode_chan(this)
		, sender_nickchange(this)
		, sender_notice(this)
		, sender_part(this)
		, sender_ping(this)
		, sender_pong(this)
		, sender_privmsg(this)
		, sender_quit(this)
		, sender_squit(this)

		, sender_akill(this)
		, sender_akill_del(this)
		, sender_channel(this)
		, sender_global_notice(this)
		, sender_global_privmsg(this)
		, sender_join(this)
		, sender_mode_user(this)
		, sender_server(this)
		, sender_sgline(this)
		, sender_sgline_del(this)

		, sender_login(this)
		, sender_logout(this)
		, sender_nickintroduction(this)
		, sender_sqline(this)
		, sender_sqline_del(this)
		, sender_svsnick(this)
		, sender_topic(this)
		, sender_wallops(this)
	{
		IRCD = &ircd_proto;
	}

	~ProtoRatbox()
	{
		IRCD = nullptr;
	}
};

template<> void ModuleInfo<ProtoRatbox>(ModuleDef *def)
{
	def->Depends("hybrid");
}

MODULE_INIT(ProtoRatbox)
