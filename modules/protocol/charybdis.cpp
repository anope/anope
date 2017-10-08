/*
 * Anope IRC Services
 *
 * Copyright (C) 2006-2017 Anope Team <team@anope.org>
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
#include "modules/protocol/hybrid.h"
#include "modules/protocol/charybdis.h"
#include "modules/chanserv/mode.h"

static Anope::string UplinkSID;

class ChannelModeLargeBan : public ChannelMode
{
 public:
	ChannelModeLargeBan(const Anope::string &mname, char modeChar) : ChannelMode(mname, modeChar) { }

	bool CanSet(User *u) const override
	{
		return u && u->HasMode("OPER");
	}
};

void charybdis::senders::NickIntroduction::Send(User *user)
{
	Anope::string modes = "+" + user->GetModes();
	Uplink::Send(Me, "EUID", user->nick, 1, user->timestamp, modes, user->GetIdent(), user->host, 0, user->GetUID(), "*", "*", user->realname);
}

void charybdis::senders::SASL::Send(const ::SASL::Message& message)
{
	Server *s = Server::Find(message.target.substr(0, 3));
	Uplink::Send(Me, "ENCAP", s ? s->GetName() : message.target.substr(0, 3), "SASL", message.source, message.target, message.type, message.data, message.ext.empty() ? "" : message.ext);
}

void charybdis::senders::SASLMechanisms::Send(const std::vector<Anope::string>& mechanisms)
{
	Anope::string mechlist;

	for (unsigned int i = 0; i < mechanisms.size(); ++i)
	{
		mechlist += "," + mechanisms[i];
	}

	Uplink::Send(Me, "ENCAP", "*", "MECHLIST", mechlist.empty() ? "" : mechlist.substr(1));
}

void charybdis::senders::SVSHold::Send(const Anope::string& nick, time_t delay)
{
	Uplink::Send(Me, "ENCAP", "*", "NICKDELAY", delay, nick);
}

void charybdis::senders::SVSHoldDel::Send(const Anope::string& nick)
{
	Uplink::Send(Me, "ENCAP", "*", "NICKDELAY", 0, nick);
}

void charybdis::senders::SVSLogin::Send(const Anope::string& uid, const Anope::string& acc, const Anope::string& vident, const Anope::string& vhost)
{
	Server *s = Server::Find(uid.substr(0, 3));
	Uplink::Send(Me, "ENCAP", s ? s->GetName() : uid.substr(0, 3), "SVSLOGIN", uid, "*", vident.empty() ? "*" : vident, vhost.empty() ? "*" : vhost, acc);
}

void charybdis::senders::VhostDel::Send(User* u)
{
	IRCD->Send<messages::VhostSet>(u, "", u->host);
}

void charybdis::senders::VhostSet::Send(User* u, const Anope::string& vident, const Anope::string& vhost)
{
	Uplink::Send(Me, "ENCAP", "*", "CHGHOST", u->GetUID(), vhost);
}

charybdis::Proto::Proto(Module *creator) : ts6::Proto(creator, "Charybdis 3.4+")
	, ratbox(creator)
{
	DefaultPseudoclientModes = "+oiS";
	CanCertFP = true;
	CanSNLine = true;
	CanSQLine = true;
	CanSQLineChannel = true;
	CanSZLine = true;
	CanSVSNick = true;
	CanSVSHold = true;
	CanSetVHost = true;
	RequiresID = true;
	MaxModes = 4;
}

void charybdis::Proto::Handshake()
{
	Uplink::Send("PASS", Config->Uplinks[Anope::CurrentUplink].password, "TS", 6, Me->GetSID());

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
	Uplink::Send("CAPAB", "BAN CHW CLUSTER ENCAP EOPMOD EUID EX IE KLN KNOCK MLOCK QS RSFNC SERVICES TB UNKLN");

	/* Make myself known to myself in the serverlist */
	Uplink::Send("SERVER", Me->GetName(), Me->GetHops() + 1, Me->GetDescription());

	/*
	 * Received: SVINFO 6 6 0 :1353235537
	 *  arg[0] = current TS version
	 *  arg[1] = minimum required TS version
	 *  arg[2] = '0'
	 *  arg[3] = server's idea of UTC time
	 */
	Uplink::Send("SVINFO", 6, 6, 0, Anope::CurTime);
}

void charybdis::Encap::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	// In a burst, states that the source user is logged in as the account.
	if (params[1] == "LOGIN" || params[1] == "SU")
	{
		User *u = source.GetUser();
		NickServ::Account *nc = NickServ::FindAccount(params[2]);

		if (!u || !nc)
			return;

		u->Login(nc);
	}
	// Received: :42XAAAAAE ENCAP * CERTFP :3f122a9cc7811dbad3566bf2cec3009007c0868f
	else if (params[1] == "CERTFP")
	{
		User *u = source.GetUser();

		if (!u)
			return;

		u->fingerprint = params[2];
		EventManager::Get()->Dispatch(&Event::Fingerprint::OnFingerprint, u);
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

void charybdis::EUID::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	NickServ::Nick *na = NULL;
	if (params[9] != "*")
		na = NickServ::FindNick(params[9]);

	time_t ts = Anope::CurTime;

	try
	{
		ts = convertTo<time_t>(params[2]);
	}
	catch (const ConvertException &) { }

	User::OnIntroduce(params[0], params[4], (params[8] != "*" ? params[8] : params[5]), params[5], params[6], source.GetServer(), params[10], ts, params[3], params[7], na ? na->GetAccount() : NULL);
}

// we can't use this function from ratbox because we set a local variable here
// SERVER dev.anope.de 1 :charybdis test server
void charybdis::ServerMessage::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	// Servers other then our immediate uplink are introduced via SID
	if (params[1] != "1")
		return;
	new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
	IRCD->Send<messages::Ping>(Me->GetName(), params[0]);
}

// we can't use this function from ratbox because we set a local variable here
void charybdis::Pass::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	// UplinkSID is used in server handler
	UplinkSID = params[3];
}

class ProtoCharybdis : public Module
	, public EventHook<Event::ChannelSync>
	, public EventHook<Event::MLockEvents>
{
	ServiceReference<ModeLocks> mlocks;

	charybdis::Proto ircd_proto;

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
	charybdis::Encap message_encap;
	charybdis::EUID message_euid;
	ratbox::Join message_join;
	hybrid::Nick message_nick;
	charybdis::Pass message_pass;
	hybrid::Pong message_pong;
	charybdis::ServerMessage message_server;
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
	rfc1459::senders::ModeUser sender_mode_user;
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
	hybrid::senders::SGLine sender_sgline;
	hybrid::senders::SGLineDel sender_sgline_del;
	hybrid::senders::SVSHold sender_svshold;
	hybrid::senders::SVSHoldDel sender_svsholddel;

	ratbox::senders::Login sender_login;
	ratbox::senders::Logout sender_logout;
	ratbox::senders::SQLine sender_sqline;
	ratbox::senders::SQLineDel sender_sqline_del;
	ratbox::senders::SVSNick sender_svsnick;
	ratbox::senders::Topic sender_topic;
	ratbox::senders::Wallops sender_wallops;

	charybdis::senders::NickIntroduction sender_nickintroduction;
	charybdis::senders::SASL sender_sasl;
	charybdis::senders::SASLMechanisms sender_sasl_mechs;
	charybdis::senders::SVSLogin sender_svslogin;
	charybdis::senders::VhostDel sender_vhost_del;
	charybdis::senders::VhostSet sender_vhost_set;

	bool use_server_side_mlock;

 public:
	ProtoCharybdis(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR)
		, EventHook<Event::ChannelSync>(this)
		, EventHook<Event::MLockEvents>(this)
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
		, message_euid(this)
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

		, sender_akill(this)
		, sender_akill_del(this)
		, sender_channel(this)
		, sender_global_notice(this)
		, sender_global_privmsg(this)
		, sender_invite(this)
		, sender_join(this)
		, sender_kick(this)
		, sender_svskill(this)
		, sender_login(this)
		, sender_logout(this)
		, sender_mode_chan(this)
		, sender_mode_user(this)
		, sender_nickchange(this)
		, sender_nickintroduction(this)
		, sender_notice(this)
		, sender_part(this)
		, sender_ping(this)
		, sender_pong(this)
		, sender_privmsg(this)
		, sender_quit(this)
		, sender_server(this)
		, sender_sasl(this)
		, sender_sasl_mechs(this)
		, sender_sgline(this)
		, sender_sgline_del(this)
		, sender_sqline(this)
		, sender_sqline_del(this)
		, sender_squit(this)
		, sender_svshold(this)
		, sender_svsholddel(this)
		, sender_svslogin(this)
		, sender_svsnick(this)
		, sender_topic(this)
		, sender_vhost_del(this)
		, sender_vhost_set(this)
		, sender_wallops(this)
	{
		IRCD = &ircd_proto;
	}

	~ProtoCharybdis()
	{
		IRCD = nullptr;
	}

	void OnReload(Configuration::Conf *conf) override
	{
		use_server_side_mlock = conf->GetModule(this)->Get<bool>("use_server_side_mlock");
	}

	void OnChannelSync(Channel *c) override
	{
		ChanServ::Channel *ci = c->GetChannel();
		if (!ci || !mlocks)
			return;

		if (use_server_side_mlock && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "");
			Uplink::Send(Me, "MLOCK", c->creation_time, ci->GetName(), modes);
		}
	}

	EventReturn OnMLock(ChanServ::Channel *ci, ModeLock *lock) override
	{
		if (!mlocks)
			return EVENT_CONTINUE;

		Channel *c = ci->GetChannel();
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->GetName());
		if (use_server_side_mlock && cm && c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->mchar;
			Uplink::Send(Me, "MLOCK", c->creation_time, ci->GetName(), modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChanServ::Channel *ci, ModeLock *lock) override
	{
		if (!mlocks)
			return EVENT_CONTINUE;

		Channel *c = ci->GetChannel();
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->GetName());
		if (use_server_side_mlock && cm && c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->mchar, "");
			Uplink::Send(Me, "MLOCK", c->creation_time, ci->GetName(), modes);
		}

		return EVENT_CONTINUE;
	}
};

template<> void ModuleInfo<ProtoCharybdis>(ModuleDef *def)
{
	def->Depends("ratbox");
}

MODULE_INIT(ProtoCharybdis)
