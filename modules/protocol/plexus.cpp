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
#include "modules/protocol/plexus.h"
#include "modules/protocol/hybrid.h"

static Anope::string UplinkSID;

class PlexusProto : public IRCDProto
{
	ServiceReference<IRCDProto> hybrid; // XXX use moddeps + inheritance here
	
 public:
	PlexusProto(Module *creator) : IRCDProto(creator, "hybrid-7.2.3+plexus-3.0.1")
		, hybrid("hybrid")
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

	void SendSVSKillInternal(const MessageSource &source, User *targ, const Anope::string &reason) override { hybrid->SendSVSKillInternal(source, targ, reason); }
	void SendGlobalNotice(ServiceBot *bi, const Server *dest, const Anope::string &msg) override { hybrid->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(ServiceBot *bi, const Server *dest, const Anope::string &msg) override { hybrid->SendGlobalPrivmsg(bi, dest, msg); }
	void SendSQLine(User *u, XLine *x) override { hybrid->SendSQLine(u, x); }
	void SendSQLineDel(XLine *x) override { hybrid->SendSQLineDel(x); }
	void SendSGLineDel(XLine *x) override { hybrid->SendSGLineDel(x); }
	void SendSGLine(User *u, XLine *x) override { hybrid->SendSGLine(u, x); }
	void SendAkillDel(XLine *x) override { hybrid->SendAkillDel(x); }
	void SendAkill(User *u, XLine *x) override { hybrid->SendAkill(u, x); }
	void SendServer(const Server *server) override { hybrid->SendServer(server); }
	void SendChannel(Channel *c) override { hybrid->SendChannel(c); }
	void SendSVSHold(const Anope::string &nick, time_t t) override { hybrid->SendSVSHold(nick, t); }
	void SendSVSHoldDel(const Anope::string &nick) override { hybrid->SendSVSHoldDel(nick); }

	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) override
	{
		Uplink::Send(source, "OPERWALL", buf);
	}

	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override
	{
		Uplink::Send(Me, "SJOIN", c->creation_time, c->name, "+" + c->GetModes(true, true), user->GetUID());
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

			ServiceBot *setter = ServiceBot::Find(user->GetUID());
			for (size_t i = 0; i < cs.Modes().length(); ++i)
				c->SetMode(setter, ModeManager::FindChannelModeByChar(cs.Modes()[i]), user->GetUID(), false);

			if (uc != NULL)
				uc->status = cs;
		}
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) override
	{
		Uplink::Send(Me, "ENCAP", u->server->GetName(), "SVSNICK", u->GetUID(), u->timestamp, newnick, when);
	}

	void SendVhost(User *u, const Anope::string &ident, const Anope::string &host) override
	{
		if (!ident.empty())
			Uplink::Send(Me, "ENCAP", "*", "CHGIDENT", u->GetUID(), ident);
		Uplink::Send(Me, "ENCAP", "*", "CHGHOST", u->GetUID(), host);
		u->SetMode(Config->GetClient("HostServ"), "CLOAK");
	}

	void SendVhostDel(User *u) override
	{
		u->RemoveMode(Config->GetClient("HostServ"), "CLOAK");
	}

	void SendConnect() override
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
		Uplink::Send(Me, "UID", u->nick, 1, u->timestamp, modes, u->GetIdent(), u->host, "255.255.255.255", u->GetUID(), 0, u->host, u->realname);
	}

	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &buf) override
	{
		Uplink::Send(source, "ENCAP", "*", "SVSMODE", u->GetUID(), u->timestamp, buf);
	}

	void SendLogin(User *u, NickServ::Nick *na) override
	{
		Uplink::Send(Me, "ENCAP", "*", "SU", u->GetUID(), na->GetAccount()->GetDisplay());
	}

	void SendLogout(User *u) override
	{
		Uplink::Send(Me, "ENCAP", "*", "SU", u->GetUID(), "");
	}

	void SendTopic(const MessageSource &source, Channel *c) override
	{
		Uplink::Send(source, "ENCAP", "*", "TOPIC", c->name, c->topic_setter, c->topic_ts, c->topic);
	}

	void SendSVSJoin(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) override
	{
		Uplink::Send(source, "ENCAP", user->server->GetName(), "SVSJOIN", user->GetUID(), chan);
	}

	void SendSVSPart(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) override
	{
		Uplink::Send(source, "ENCAP", user->server->GetName(), "SVSPART", user->GetUID(), chan);
	}
};

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
	return;
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
void plexus::Server::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	/* Servers other than our immediate uplink are introduced via SID */
	if (params[1] != "1")
		return;

	new ::Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
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
	Module *m_hybrid;

	PlexusProto ircd_proto;

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
	hybrid::EOB message_eob;
	plexus::Encap message_encap;
	hybrid::Join message_join;
	hybrid::Nick message_nick;
	IRCDMessagePass message_pass;
	plexus::Server message_server;
	hybrid::SID message_sid;
	hybrid::SJoin message_sjoin;
	hybrid::TBurst message_tburst;
	hybrid::TMode message_tmode;
	plexus::UID message_uid;

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
	{
	}
};

template<> void ModuleInfo<ProtoPlexus>(ModuleDef *def)
{
	def->Depends("hybrid");
}

MODULE_INIT(ProtoPlexus)
