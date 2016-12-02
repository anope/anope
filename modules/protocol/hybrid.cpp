/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 * Copyright (C) 2012-2016 ircd-hybrid development team
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

/* Dependencies: anope_protocol.rfc1459 */

#include "module.h"
#include "modules/protocol/hybrid.h"

static Anope::string UplinkSID;

class HybridProto : public IRCDProto
{
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

	void SendSVSKillInternal(const MessageSource &source, User *u, const Anope::string &buf) override
	{
		IRCDProto::SendSVSKillInternal(source, u, buf);
		u->KillInternal(source, buf);
	}

  public:
	HybridProto(Module *creator) : IRCDProto(creator, "Hybrid 8.2.x")
	{
		DefaultPseudoclientModes = "+oi";
		CanSVSNick = true;
		CanSVSHold = true;
		CanSVSJoin = true;
		CanSNLine = true;
		CanSQLine = true;
		CanSQLineChannel = true;
		CanSZLine = true;
		CanCertFP = true;
		CanSetVHost = true;
		RequiresID = true;
		MaxModes = 6;
	}

	void SendInvite(const MessageSource &source, const Channel *c, User *u) override
	{
		Uplink::Send(source, "INVITE", u->GetUID(), c->name, c->creation_time);
	}

	void SendGlobalNotice(ServiceBot *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "NOTICE", "$$" + dest->GetName(), msg);
	}

	void SendGlobalPrivmsg(ServiceBot *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "PRIVMSG", "$$" + dest->GetName(), msg);
	}

	void SendSQLine(User *, XLine *x) override
	{
		Uplink::Send(FindIntroduced(), "RESV", "*", x->GetExpires() ? x->GetExpires() - Anope::CurTime : 0, x->GetMask(), x->GetReason());
	}

	void SendSGLineDel(XLine *x) override
	{
		Uplink::Send(Config->GetClient("OperServ"), "UNXLINE", "*", x->GetMask());
	}

	void SendSGLine(User *, XLine *x) override
	{
		Uplink::Send(Config->GetClient("OperServ"), "XLINE", "*", x->GetMask(), x->GetExpires() ? x->GetExpires() - Anope::CurTime : 0, x->GetReason());
	}

	void SendSZLineDel(XLine *x) override
	{
		Uplink::Send(Config->GetClient("OperServ"), "UNDLINE", "*", x->GetHost());
	}

	void SendSZLine(User *, XLine *x) override
	{
		/* Calculate the time left before this would expire, capping it at 2 days */
		time_t timeleft = x->GetExpires() - Anope::CurTime;

		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;

		Uplink::Send(Config->GetClient("OperServ"), "DLINE", "*", timeleft, x->GetHost(), x->GetReason());
	}

	void SendAkillDel(XLine *x) override
	{
		if (x->IsRegex() || x->HasNickOrReal())
			return;

		Uplink::Send(Config->GetClient("OperServ"), "UNKLINE", "*", x->GetUser(), x->GetHost());
	}

	void SendSQLineDel(XLine *x) override
	{
		Uplink::Send(Config->GetClient("OperServ"), "UNRESV", "*", x->GetMask());
	}

	void SendJoin(User *u, Channel *c, const ChannelStatus *status) override
	{
		/*
		 * Note that we must send our modes with the SJOIN and can not add them to the
		 * mode stacker because ircd-hybrid does not allow *any* client to op itself
		 */
		Uplink::Send("SJOIN", c->creation_time, c->name, "+" + c->GetModes(true, true), (status != NULL ? status->BuildModePrefixList() : "") + u->GetUID());

		/* And update our internal status for this user since this is not going through our mode handling system */
		if (status)
		{
			ChanUserContainer *uc = c->FindUser(u);

			if (uc)
				uc->status = *status;
		}
	}

	void SendAkill(User *u, XLine *x) override
	{
		if (x->IsRegex() || x->HasNickOrReal())
		{
			if (!u)
			{
				/*
				 * No user (this akill was just added), and contains nick and/or realname.
				 * Find users that match and ban them.
				 */
				for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
					if (x->GetManager()->Check(it->second, x))
						this->SendAkill(it->second, x);

				return;
			}

			XLine *old = x;

			if (old->GetManager()->HasEntry("*@" + u->host))
				return;

			/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
			XLine *xl = Serialize::New<XLine *>();
			xl->SetMask("*@" + u->host);
			xl->SetBy(old->GetBy());
			xl->SetExpires(old->GetExpires());
			xl->SetReason(old->GetReason());
			xl->SetID(old->GetID());

			old->GetManager()->AddXLine(xl);
			x = xl;

			Log(Config->GetClient("OperServ"), "akill") << "AKILL: Added an akill for " << x->GetMask() << " because " << u->GetMask() << "#"
					<< u->realname << " matches " << old->GetMask();
		}

		/* Calculate the time left before this would expire, capping it at 2 days */
		time_t timeleft = x->GetExpires() - Anope::CurTime;

		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;

		Uplink::Send(Config->GetClient("OperServ"), "KLINE", timeleft, x->GetUser(), x->GetHost(), x->GetReason());
	}

	void SendServer(const Server *server) override
	{
		if (server == Me)
			Uplink::Send("SERVER", server->GetName(), server->GetHops() + 1, server->GetDescription());
		else
			Uplink::Send(Me, "SID", server->GetName(), server->GetHops() + 1, server->GetSID(), server->GetDescription());
	}

	void SendConnect() override
	{
		Uplink::Send("PASS", Config->Uplinks[Anope::CurrentUplink].password, "TS", 6, Me->GetSID());

		/*
		 * As of January 13, 2016, ircd-hybrid-8 supports the following capabilities
		 * which are required to work with IRC-services:
		 *
		 * QS     - Can handle quit storm removal
		 * EX     - Can do channel +e exemptions
		 * IE     - Can do invite exceptions
		 * CHW    - Can do channel wall @#
		 * TBURST - Supports topic burst
		 * ENCAP  - Supports ENCAP
		 * HOPS   - Supports HalfOps
		 * SVS    - Supports services
		 * EOB    - Supports End Of Burst message
		 */
		Uplink::Send("CAPAB", "QS EX CHW IE ENCAP TBURST SVS HOPS EOB");

		SendServer(Me);

		Uplink::Send("SVINFO", 6, 6, 0, Anope::CurTime);
	}

	void SendClientIntroduction(User *u) override
	{
		Anope::string modes = "+" + u->GetModes();

		Uplink::Send(Me, "UID", u->nick, 1, u->timestamp, modes, u->GetIdent(), u->host, "0.0.0.0", u->GetUID(), "*", u->realname);
	}

	void SendEOB() override
	{
		Uplink::Send(Me, "EOB");
	}

	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &buf) override
	{
		IRCMessage message(source, "SVSMODE", u->GetUID(), u->timestamp);
		message.TokenizeAndPush(buf);
		Uplink::SendMessage(message);
	}

	void SendLogin(User *u, NickServ::Nick *na) override
	{
		IRCD->SendMode(Config->GetClient("NickServ"), u, "+d %s", na->GetAccount()->GetDisplay().c_str());
	}

	void SendLogout(User *u) override
	{
		IRCD->SendMode(Config->GetClient("NickServ"), u, "+d *");
	}

	void SendChannel(Channel *c) override
	{
		Anope::string modes = c->GetModes(true, true);

		if (modes.empty())
			modes = "+";

		Uplink::Send("SJOIN", c->creation_time, c->name, modes, "");
	}

	void SendTopic(const MessageSource &source, Channel *c) override
	{
		Uplink::Send(source, "TBURST", c->creation_time, c->name, c->topic_ts, c->topic_setter, c->topic);
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) override
	{
		Uplink::Send(Me, "SVSNICK", u->GetUID(), newnick, when);
	}

	void SendSVSJoin(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &) override
	{
		Uplink::Send(source, "SVSJOIN", u->GetUID(), chan);
	}

	void SendSVSPart(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &param) override
	{
		if (!param.empty())
			Uplink::Send(source, "SVSPART", u->GetUID(), chan, param);
		else
			Uplink::Send(source, "SVSPART", u->GetUID(), chan);
	}

	void SendSVSHold(const Anope::string &nick, time_t t) override
	{
#if 0
		XLine x(nick, Me->GetName(), Anope::CurTime + t, "Being held for registered user");
		this->SendSQLine(NULL, &x);
#endif
	}
#warning "xline on stack"

	void SendSVSHoldDel(const Anope::string &nick) override
	{
#if 0
		XLine x(nick);
		this->SendSQLineDel(&x);
#endif
	}

	void SendVhost(User *u, const Anope::string &ident, const Anope::string &host) override
	{
		u->SetMode(Config->GetClient("HostServ"), "CLOAK", host);
	}

	void SendVhostDel(User *u) override
	{
		u->RemoveMode(Config->GetClient("HostServ"), "CLOAK", u->host);
	}

	bool IsIdentValid(const Anope::string &ident) override
	{
		if (ident.empty() || ident.length() > Config->GetBlock("networkinfo")->Get<unsigned>("userlen"))
			return false;

		Anope::string chars = "~}|{ `_^]\\[ .-$";

		for (unsigned i = 0; i < ident.length(); ++i)
		{
			const char &c = ident[i];

			if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
				continue;

			if (chars.find(c) != Anope::string::npos)
				continue;

			return false;
		}

		return true;
	}
};

/*            0          1        2  3              */
/* :0MC BMASK 1350157102 #channel b :*!*@*.test.com */
void hybrid::BMask::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Channel *c = Channel::Find(params[1]);
	ChannelMode *mode = ModeManager::FindChannelModeByChar(params[2][0]);

	if (c && mode)
	{
		spacesepstream bans(params[3]);
		Anope::string token;
		while (bans.GetToken(token))
			c->SetModeInternal(source, mode, token);
	}
}

void hybrid::EOB::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	source.GetServer()->Sync(true);
}

void hybrid::Join::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	if (params.size() < 2)
		return;

	std::vector<Anope::string> p = params;
	p.erase(p.begin());

	return rfc1459::Join::Run(source, p);
}

/*                 0       1          */
/* :0MCAAAAAB NICK newnick 1350157102 */
void hybrid::Nick::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	time_t ts = Anope::CurTime;

	try
	{
		ts = convertTo<time_t>(params[1]);
	}
	catch (const ConvertException &) { }

	source.GetUser()->ChangeNick(params[0], ts);
}

struct IRCDMessagePass : IRCDMessage
{
	IRCDMessagePass(Module *creator) : IRCDMessage(creator, "PASS", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*      0        1  2 3   */
	/* PASS password TS 6 0MC */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		UplinkSID = params[3];
	}
};

void hybrid::Pong::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	source.GetServer()->Sync(false);
}

/*        0          1  2                       */
/* SERVER hades.arpa 1 :ircd-hybrid test server */
void hybrid::ServerMessage::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	/* Servers other than our immediate uplink are introduced via SID */
	if (params[1] != "1")
		return;

	new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);

	IRCD->SendPing(Me->GetName(), params[0]);
}

void hybrid::SID::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	unsigned int hops = 0;

	try
	{
		hops = convertTo<unsigned int>(params[1]);
	}
	catch (const ConvertException &) { }

	new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[3], params[2]);

	IRCD->SendPing(Me->GetName(), params[0]);
}

void hybrid::SJoin::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Anope::string modes;
	if (params.size() >= 3)
		for (unsigned i = 2; i < params.size() - 1; ++i)
			modes += " " + params[i];
	if (!modes.empty())
		modes.erase(modes.begin());

	std::list<rfc1459::Join::SJoinUser> users;

	spacesepstream sep(params[params.size() - 1]);
	Anope::string buf;

	while (sep.GetToken(buf))
	{
		rfc1459::Join::SJoinUser sju;

		/* Get prefixes from the nick */
		for (char ch; !buf.empty() && (ch = ModeManager::GetStatusChar(buf[0]));)
		{
			buf.erase(buf.begin());
			sju.first.AddMode(ch);
		}

		sju.second = User::Find(buf);
		if (!sju.second)
		{
			Log(LOG_DEBUG) << "SJOIN for non-existent user " << buf << " on " << params[1];
			continue;
		}

		users.push_back(sju);
	}

	time_t ts = Anope::CurTime;

	try
	{
		ts = convertTo<time_t>(params[0]);
	}
	catch (const ConvertException &) { }

	rfc1459::Join::SJoin(source, params[1], ts, modes, users);
}

/*
 * parv[0] = nickname
 * parv[1] = TS
 * parv[2] = mode
 * parv[3] = optional argument (services id)
 */
void hybrid::SVSMode::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	User *u = User::Find(params[0]);
	if (!u)
		return;

	time_t ts;

	try
	{
		ts = convertTo<time_t>(params[1]);
	}
	catch (const ConvertException &)
	{
		return;
	}

	if (ts != u->timestamp)
		return;

	u->SetModesInternal(source, "%s", params[2].c_str());
}

void hybrid::TBurst::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Anope::string setter;
	sepstream(params[3], '!').GetToken(setter, 0);
	Channel *c = Channel::Find(params[1]);

	time_t topic_time = Anope::CurTime;

	try
	{
		topic_time = convertTo<time_t>(params[2]);
	}
	catch (const ConvertException &ex) { }

	if (c)
		c->ChangeTopicInternal(NULL, setter, params[4], topic_time);
}

void hybrid::TMode::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	time_t ts = 0;

	try
	{
		ts = convertTo<time_t>(params[0]);
	}
	catch (const ConvertException &) { }

	Channel *c = Channel::Find(params[1]);
	Anope::string modes = params[2];

	for (unsigned i = 3; i < params.size(); ++i)
		modes += " " + params[i];

	if (c)
		c->SetModesInternal(source, modes, ts);
}

/*          0     1 2          3   4      5             6        7         8           9                   */
/* :0MC UID Steve 1 1350157102 +oi ~steve resolved.host 10.0.0.1 0MCAAAAAB Steve      :Mining all the time */
void hybrid::UID::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Anope::string ip = params[6];

	if (ip == "0") /* Can be 0 for spoofed clients */
		ip.clear();

	NickServ::Nick *na = NULL;
	if (params[8] != "0" && params[8] != "*")
		na = NickServ::FindNick(params[8]);

	time_t ts = 0;

	try
	{
		ts = convertTo<time_t>(params[2]);
	}
	catch (const ConvertException &) { }

	/* Source is always the server */
	User::OnIntroduce(params[0], params[4], params[5], "",
			ip, source.GetServer(),
			params[9], ts,
			params[3], params[7], na ? na->GetAccount() : NULL);
}

/*                   0                                                                */
/* :0MCAAAAAB CERTFP 4C62287BA6776A89CD4F8FF10A62FFB35E79319F51AF6C62C674984974FCCB1D */
void hybrid::CertFP::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	User *u = source.GetUser();

	u->fingerprint = params[0];
	EventManager::Get()->Dispatch(&Event::Fingerprint::OnFingerprint, u);
}

class ProtoHybrid : public Module
	, public EventHook<Event::UserNickChange>
{
	HybridProto ircd_proto;

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
	hybrid::CertFP message_certfp;
	hybrid::BMask message_bmask;
	hybrid::EOB message_eob;
	hybrid::Join message_join;
	hybrid::Nick message_nick;
	IRCDMessagePass message_pass;
	hybrid::Pong message_pong;
	hybrid::ServerMessage message_server;
	hybrid::SID message_sid;
	hybrid::SJoin message_sjoin;
	hybrid::SVSMode message_svsmode;
	hybrid::TBurst message_tburst;
	hybrid::TMode message_tmode;
	hybrid::UID message_uid;

public:
	ProtoHybrid(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR)
		, EventHook<Event::UserNickChange>(this)
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
		, message_join(this)
		, message_nick(this)
		, message_pass(this)
		, message_pong(this)
		, message_server(this)
		, message_sid(this)
		, message_sjoin(this)
		, message_svsmode(this)
		, message_tburst(this)
		, message_tmode(this)
		, message_uid(this)
		, message_certfp(this)
	{
	}

	void OnUserNickChange(User *u, const Anope::string &) override
	{
		u->RemoveModeInternal(Me, ModeManager::FindUserModeByName("REGISTERED"));
	}
};

MODULE_INIT(ProtoHybrid)
