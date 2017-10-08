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

/* Dependencies: anope_protocol.ratbox */

#include "module.h"
#include "modules/sasl.h"
#include "modules/protocol/plexus.h"
#include "modules/protocol/hybrid.h"
#include "modules/protocol/ratbox.h"

static Anope::string UplinkSID;

void plexus::senders::ModeUser::Send(const MessageSource &source, User *user, const Anope::string &modes)
{
	IRCMessage message(source, "ENCAP", "*", "SVSMODE", user->GetUID(), user->timestamp);
	message.TokenizeAndPush(modes);
	Uplink::SendMessage(message);
}

void plexus::senders::NickIntroduction::Send(User *user)
{
	Anope::string modes = "+" + user->GetModes();
	Uplink::Send(Me, "UID", user->nick, 1, user->timestamp, modes, user->GetIdent(), user->host, "255.255.255.255", user->GetUID(), 0, user->host, user->realname);
}

void plexus::senders::NOOP::Send(Server* server, bool mode)
{
	Uplink::Send("ENCAP", server->GetName(), "SVSNOOP", (mode ? "+" : "-"));
}

void plexus::senders::SASL::Send(const ::SASL::Message& message)
{
	Anope::string sid = message.target.substr(0, 3);
	Server *s = Server::Find(sid);
	Uplink::Send(Me, "ENCAP", s ? s->GetName() : sid, "SASL",
			message.source, message.target, message.type, message.data, message.ext.empty() ? "" : message.ext);
}

void plexus::senders::Topic::Send(const MessageSource &source, Channel *channel, const Anope::string &topic, time_t topic_ts, const Anope::string &topic_setter)
{
	Uplink::Send(source, "ENCAP", "*", "TOPIC", channel->name, topic_setter, topic_ts, topic);
}

void plexus::senders::SVSJoin::Send(const MessageSource& source, User* user, const Anope::string& chan, const Anope::string& key)
{
	Uplink::Send(source, "ENCAP", user->server->GetName(), "SVSJOIN", user->GetUID(), chan);
}

void plexus::senders::SVSLogin::Send(const Anope::string& uid, const Anope::string& acc, const Anope::string& vident, const Anope::string& vhost)
{
	Anope::string sid = uid.substr(0, 3);
	Server *s = Server::Find(sid);

	Uplink::Send(Me, "ENCAP", s ? s->GetName() : sid, "SVSLOGIN", uid, "*", vident.empty() ? "*" : vident, vhost.empty() ? "*" : vhost, acc);
}

void plexus::senders::SVSNick::Send(User* u, const Anope::string& newnick, time_t ts)
{
	Uplink::Send(Me, "ENCAP", u->server->GetName(), "SVSNICK", u->GetUID(), u->timestamp, newnick, ts);
}

void plexus::senders::SVSPart::Send(const MessageSource& source, User* user, const Anope::string& chan, const Anope::string& reason)
{
	Uplink::Send(source, "ENCAP", user->server->GetName(), "SVSPART", user->GetUID(), chan);
}

void plexus::senders::VhostDel::Send(User* u)
{
	u->RemoveMode(Config->GetClient("HostServ"), "CLOAK");
}

void plexus::senders::VhostSet::Send(User* u, const Anope::string& vident, const Anope::string& vhost)
{
	if (!vident.empty())
		Uplink::Send(Me, "ENCAP", "*", "CHGIDENT", u->GetUID(), vident);
	Uplink::Send(Me, "ENCAP", "*", "CHGHOST", u->GetUID(), vhost);
	u->SetMode(Config->GetClient("HostServ"), "CLOAK");
}

plexus::Proto::Proto(Module *creator) : ts6::Proto(creator, "Plexus 4")
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

void plexus::Proto::Handshake()
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
	 * ENCAP  - Supports encapsulization of protocol messages
	 * SVS    - Supports services protocol extensions
	 */
	Uplink::Send("CAPAB", "QS EX CHW IE EOB KLN UNKLN GLN HUB KNOCK TBURST PARA ENCAP SVS");

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

void plexus::Encap::Run(MessageSource &source, const std::vector<Anope::string> &params)
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
		NickServ::Account *nc = NickServ::FindAccount(params[3]);
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
			EventManager::Get()->Dispatch(&Event::Fingerprint::OnFingerprint, u);
		}
	}

	else if (params[1] == "SASL" && sasl && params.size() >= 6)
	{
		SASL::Message m;
		m.source = params[2];
		m.target = params[3];
		m.type = params[4];
		m.data = params[5];
		m.ext = params.size() > 6 ? params[6] : "";

		sasl->ProcessMessage(m);
	}
}

struct IRCDMessagePass : IRCDMessage
{
	IRCDMessagePass(Module *creator) : IRCDMessage(creator, "PASS", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		UplinkSID = params[3];
	}
};

/*        0          1  2                       */
/* SERVER hades.arpa 1 :ircd-hybrid test server */
void plexus::ServerMessage::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	/* Servers other than our immediate uplink are introduced via SID */
	if (params[1] != "1")
		return;

	new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
}

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
void plexus::UID::Run(MessageSource &source, const std::vector<Anope::string> &params)
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

	NickServ::Nick *na = NULL;
	try
	{
		if (params[8].is_pos_number_only() && convertTo<time_t>(params[8]) == ts)
			na = NickServ::FindNick(params[0]);
	}
	catch (const ConvertException &) { }
	if (params[8] != "0" && !na)
		na = NickServ::FindNick(params[8]);

	User::OnIntroduce(params[0], params[4], params[9], params[5], ip, source.GetServer(), params[10], ts, params[3], params[7], na ? na->GetAccount() : NULL);
}

class ProtoPlexus : public Module
{
	plexus::Proto ircd_proto;

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
	rfc1459::Pong message_pong;
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
	hybrid::EOB message_eob;
	plexus::Encap message_encap;
	hybrid::Join message_join;
	hybrid::Nick message_nick;
	IRCDMessagePass message_pass;
	plexus::ServerMessage message_server;
	hybrid::SID message_sid;
	hybrid::SJoin message_sjoin;
	hybrid::TBurst message_tburst;
	hybrid::TMode message_tmode;
	plexus::UID message_uid;

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
	hybrid::senders::MessageServer sender_server;
	hybrid::senders::SQLine sender_sqline;
	hybrid::senders::SQLineDel sender_sqline_del;
	hybrid::senders::SVSHold sender_svshold;
	hybrid::senders::SVSHoldDel sender_svsholddel;

	ratbox::senders::Login sender_login;
	ratbox::senders::Logout sender_logout;
	ratbox::senders::Wallops sender_wallops;

	plexus::senders::ModeUser sender_mode_user;
	plexus::senders::NickIntroduction sender_nickintroduction;
	plexus::senders::NOOP sender_noop;
	plexus::senders::SASL sender_sasl;
	plexus::senders::SVSJoin sender_svsjoin;
	plexus::senders::SVSLogin sender_svslogin;
	plexus::senders::SVSNick sender_svsnick;
	plexus::senders::SVSPart sender_svspart;
	plexus::senders::Topic sender_topic;
	plexus::senders::VhostDel sender_vhost_del;
	plexus::senders::VhostSet sender_vhost_set;

 public:
	ProtoPlexus(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR)
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
		, message_pong(this)
		, message_privmsg(this)
		, message_quit(this)
		, message_squit(this)
		, message_stats(this)
		, message_time(this)
		, message_topic(this)
		, message_version(this)
		, message_whois(this)

		, message_bmask(this)
		, message_eob(this)
		, message_encap(this)
		, message_join(this)
		, message_nick(this)
		, message_pass(this)
		, message_server(this)
		, message_sid(this)
		, message_sjoin(this)
		, message_tburst(this)
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
		, sender_server(this)
		, sender_sqline(this)
		, sender_sqline_del(this)
		, sender_svshold(this)
		, sender_svsholddel(this)

		, sender_login(this)
		, sender_logout(this)
		, sender_wallops(this)

		, sender_mode_user(this)
		, sender_nickintroduction(this)
		, sender_noop(this)
		, sender_sasl(this)
		, sender_svsjoin(this)
		, sender_svslogin(this)
		, sender_svsnick(this)
		, sender_svspart(this)
		, sender_topic(this)
		, sender_vhost_del(this)
		, sender_vhost_set(this)
	{
		IRCD = &ircd_proto;
	}

	~ProtoPlexus()
	{
		IRCD = nullptr;
	}
};

template<> void ModuleInfo<ProtoPlexus>(ModuleDef *def)
{
	def->Depends("hybrid");
}

MODULE_INIT(ProtoPlexus)
