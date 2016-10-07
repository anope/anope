/*
 * Anope IRC Services
 *
 * Copyright (C) 2006-2016 Anope Team <team@anope.org>
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
#include "modules/protocol/ratbox.h"
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


class CharybdisProto : public IRCDProto
{
	ServiceReference<IRCDProto> ratbox; // XXX

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
	CharybdisProto(Module *creator) : IRCDProto(creator, "Charybdis 3.4+")
		, ratbox("ratbox")
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

	void SendSVSKillInternal(const MessageSource &source, User *targ, const Anope::string &reason) override { ratbox->SendSVSKillInternal(source, targ, reason); }
	void SendGlobalNotice(ServiceBot *bi, const Server *dest, const Anope::string &msg) override { ratbox->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(ServiceBot *bi, const Server *dest, const Anope::string &msg) override { ratbox->SendGlobalPrivmsg(bi, dest, msg); }
	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) override { ratbox->SendGlobopsInternal(source, buf); }
	void SendSGLine(User *u, XLine *x) override { ratbox->SendSGLine(u, x); }
	void SendSGLineDel(XLine *x) override { ratbox->SendSGLineDel(x); }
	void SendAkill(User *u, XLine *x) override { ratbox->SendAkill(u, x); }
	void SendAkillDel(XLine *x) override { ratbox->SendAkillDel(x); }
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override { ratbox->SendJoin(user, c, status); }
	void SendServer(const Server *server) override { ratbox->SendServer(server); }
	void SendChannel(Channel *c) override { ratbox->SendChannel(c); }
	void SendTopic(const MessageSource &source, Channel *c) override { ratbox->SendTopic(source, c); }
	bool IsIdentValid(const Anope::string &ident) override { return ratbox->IsIdentValid(ident); }
	void SendLogin(User *u, NickServ::Nick *na) override { ratbox->SendLogin(u, na); }
	void SendLogout(User *u) override { ratbox->SendLogout(u); }

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

	void SendConnect() override
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
		SendServer(Me);

		/*
		 * Received: SVINFO 6 6 0 :1353235537
		 *  arg[0] = current TS version
		 *  arg[1] = minimum required TS version
		 *  arg[2] = '0'
		 *  arg[3] = server's idea of UTC time
		 */
		Uplink::Send("SVINFO", 6, 6, Anope::CurTime);
	}

	void SendClientIntroduction(User *u) override
	{
		Anope::string modes = "+" + u->GetModes();
		Uplink::Send(Me, "EUID", u->nick, 1, u->timestamp, modes, u->GetIdent(), u->host, 0, u->GetUID(), "*", "*", u->realname);
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) override
	{
		Uplink::Send(Me, "ENCAP", u->server->GetName(), "RSFNC", u->GetUID(),
				newnick, when, u->timestamp);
	}

	void SendSVSHold(const Anope::string &nick, time_t delay) override
	{
		Uplink::Send(Me, "ENCAP", "*", "NICKDELAY", delay, nick);
	}

	void SendSVSHoldDel(const Anope::string &nick) override
	{
		Uplink::Send(Me, "ENCAP", "*", "NICKDELAY", 0, nick);
	}

	void SendVhost(User *u, const Anope::string &ident, const Anope::string &host) override
	{
		Uplink::Send(Me, "ENCAP", "*", "CHGHOST", u->GetUID(), host);
	}

	void SendVhostDel(User *u) override
	{
		this->SendVhost(u, "", u->host);
	}

	void SendSASLMechanisms(std::vector<Anope::string> &mechanisms) override
	{
		Anope::string mechlist;

		for (unsigned i = 0; i < mechanisms.size(); ++i)
		{
			mechlist += "," + mechanisms[i];
		}

		Uplink::Send(Me, "ENCAP", "*", "MECHLIST", mechlist.empty() ? "" : mechlist.substr(1));
	}

	void SendSASLMessage(const SASL::Message &message) override
	{
		Server *s = Server::Find(message.target.substr(0, 3));
		Uplink::Send(Me, "ENCAP", s ? s->GetName() : message.target.substr(0, 3), "SASL", message.source, message.target, message.type, message.data, message.ext.empty() ? "" : message.ext);
	}

	void SendSVSLogin(const Anope::string &uid, const Anope::string &acc, const Anope::string &vident, const Anope::string &vhost) override
	{
		Server *s = Server::Find(uid.substr(0, 3));
		Uplink::Send(Me, "ENCAP", s ? s->GetName() : uid.substr(0, 3), "SVSLOGIN", uid, "*", vident.empty() ? "*" : vident, vhost.empty() ? "*" : vhost, acc);
	}
};


void charybdis::Encap::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	User *u = source.GetUser();

	// In a burst, states that the source user is logged in as the account.
	if (params[1] == "LOGIN" || params[1] == "SU")
	{
		NickServ::Account *nc = NickServ::FindAccount(params[2]);
		if (!nc)
			return;
		u->Login(nc);
	}
	// Received: :42XAAAAAE ENCAP * CERTFP :3f122a9cc7811dbad3566bf2cec3009007c0868f
	if (params[1] == "CERTFP")
	{
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
	if (params[1] == "SASL" && sasl && params.size() >= 6)
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

	User::OnIntroduce(params[0], params[4], (params[8] != "*" ? params[8] : params[5]), params[5], params[6], source.GetServer(), params[10], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime, params[3], params[7], na ? na->GetAccount() : NULL);
}

// we can't use this function from ratbox because we set a local variable here
// SERVER dev.anope.de 1 :charybdis test server
void charybdis::Server::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	// Servers other then our immediate uplink are introduced via SID
	if (params[1] != "1")
		return;
	new ::Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
	IRCD->SendPing(Me->GetName(), params[0]);
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

	CharybdisProto ircd_proto;

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
	charybdis::Server message_server;
	hybrid::SID message_sid;
	hybrid::SJoin message_sjoin;
	ratbox::TB message_tb;
	hybrid::TMode message_tmode;
	ratbox::UID message_uid;

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
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		use_server_side_mlock = conf->GetModule(this)->Get<bool>("use_server_side_mlock");
	}

	void OnChannelSync(Channel *c) override
	{
		if (!c->ci || !mlocks)
			return;

		if (use_server_side_mlock && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = mlocks->GetMLockAsString(c->ci, false).replace_all_cs("+", "").replace_all_cs("-", "");
			Uplink::Send(Me, "MLOCK", c->creation_time, c->ci->GetName(), modes);
		}
	}

	EventReturn OnMLock(ChanServ::Channel *ci, ModeLock *lock) override
	{
		if (!mlocks)
			return EVENT_CONTINUE;

		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->GetName());
		if (use_server_side_mlock && cm && ci->c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->mchar;
			Uplink::Send(Me, "MLOCK", ci->c->creation_time, ci->GetName(), modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChanServ::Channel *ci, ModeLock *lock) override
	{
		if (!mlocks)
			return EVENT_CONTINUE;

		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->GetName());
		if (use_server_side_mlock && cm && ci->c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->mchar, "");
			Uplink::Send(Me, "MLOCK", ci->c->creation_time, ci->GetName(), modes);
		}

		return EVENT_CONTINUE;
	}
};

template<> void ModuleInfo<ProtoCharybdis>(ModuleDef *def)
{
	def->Depends("ratbox");
}

MODULE_INIT(ProtoCharybdis)
