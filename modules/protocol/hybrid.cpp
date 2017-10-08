/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
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

/* Dependencies: anope_protocol.rfc1459,anope_protocol.ts6 */

#include "module.h"
#include "modules/protocol/hybrid.h"

static Anope::string UplinkSID;

void hybrid::senders::Akill::Send(User* u, XLine* x)
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
					this->Send(it->second, x);

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

		Anope::Logger.Bot("OperServ").Category("akill").Log(_("AKILL: Added an akill for {0} because {1}#{2} matches {3}"),
				x->GetMask(), u->GetMask(), u->realname, old->GetMask());
	}

	/* Calculate the time left before this would expire, capping it at 2 days */
	time_t timeleft = x->GetExpires() - Anope::CurTime;

	if (timeleft > 172800 || !x->GetExpires())
		timeleft = 172800;

	Uplink::Send(Config->GetClient("OperServ"), "KLINE", timeleft, x->GetUser(), x->GetHost(), x->GetReason());
}

void hybrid::senders::AkillDel::Send(XLine* x)
{
	if (x->IsRegex() || x->HasNickOrReal())
		return;

	Uplink::Send(Config->GetClient("OperServ"), "UNKLINE", "*", x->GetUser(), x->GetHost());
}

void hybrid::senders::MessageChannel::Send(Channel* c)
{
	Anope::string modes = c->GetModes(true, true);

	if (modes.empty())
		modes = "+";

	Uplink::Send("SJOIN", c->creation_time, c->name, modes, "");
}

void hybrid::senders::GlobalNotice::Send(const MessageSource &source, Server *dest, const Anope::string &msg)
{
	Uplink::Send(source, "NOTICE", "$$" + dest->GetName(), msg);
}

void hybrid::senders::GlobalPrivmsg::Send(const MessageSource& source, Server* dest, const Anope::string& msg)
{
	Uplink::Send(source, "PRIVMSG", "$$" + dest->GetName(), msg);
}

void hybrid::senders::Invite::Send(const MessageSource &source, Channel *chan, User *user)
{
	Uplink::Send(source, "INVITE", user->GetUID(), chan->name, chan->creation_time);
}

void hybrid::senders::Join::Send(User* u, Channel* c, const ChannelStatus* status)
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

void hybrid::senders::Login::Send(User *u, NickServ::Nick *na)
{
	IRCD->SendMode(Config->GetClient("NickServ"), u, "+d {0}", na->GetAccount()->GetDisplay());
}

void hybrid::senders::Logout::Send(User *u)
{
	IRCD->SendMode(Config->GetClient("NickServ"), u, "+d *");
}

void hybrid::senders::ModeUser::Send(const MessageSource &source, User *user, const Anope::string &modes)
{
	IRCMessage message(source, "SVSMODE", user->GetUID(), user->timestamp);
	message.TokenizeAndPush(modes);
	Uplink::SendMessage(message);
}

void hybrid::senders::NickIntroduction::Send(User *user)
{
	Anope::string modes = "+" + user->GetModes();
	Uplink::Send(Me, "UID", user->nick, 1, user->timestamp, modes, user->GetIdent(), user->host, "0.0.0.0", user->GetUID(), "*", user->realname);
}

void hybrid::senders::MessageServer::Send(Server* server)
{
	Uplink::Send(Me, "SID", server->GetName(), server->GetHops() + 1, server->GetSID(), server->GetDescription());
}

void hybrid::senders::SGLine::Send(User*, XLine* x)
{
	Uplink::Send(Config->GetClient("OperServ"), "XLINE", "*", x->GetMask(), x->GetExpires() ? x->GetExpires() - Anope::CurTime : 0, x->GetReason());
}

void hybrid::senders::SGLineDel::Send(XLine* x)
{
	Uplink::Send(Config->GetClient("OperServ"), "UNXLINE", "*", x->GetMask());
}

void hybrid::senders::SQLine::Send(User*, XLine* x)
{
#warning "findintroduced"
	//Uplink::Send(FindIntroduced(), "RESV", "*", x->GetExpires() ? x->GetExpires() - Anope::CurTime : 0, x->GetMask(), x->GetReason());
}

void hybrid::senders::SQLineDel::Send(XLine* x)
{
	Uplink::Send(Config->GetClient("OperServ"), "UNRESV", "*", x->GetMask());
}

void hybrid::senders::SZLine::Send(User*, XLine* x)
{
	/* Calculate the time left before this would expire, capping it at 2 days */
	time_t timeleft = x->GetExpires() - Anope::CurTime;

	if (timeleft > 172800 || !x->GetExpires())
		timeleft = 172800;

	Uplink::Send(Config->GetClient("OperServ"), "DLINE", "*", timeleft, x->GetHost(), x->GetReason());
}

void hybrid::senders::SZLineDel::Send(XLine* x)
{
	Uplink::Send(Config->GetClient("OperServ"), "UNDLINE", "*", x->GetHost());
}

void hybrid::senders::SVSHold::Send(const Anope::string&, time_t)
{
#if 0
	XLine x(nick, Me->GetName(), Anope::CurTime + t, "Being held for registered user");
	this->SendSQLine(NULL, &x);
#endif
}

#warning "xline on stack"
void hybrid::senders::SVSHoldDel::Send(const Anope::string&)
{
#if 0
	XLine x(nick);
	this->SendSQLineDel(&x);
#endif
}

void hybrid::senders::SVSJoin::Send(const MessageSource& source, User* u, const Anope::string& chan, const Anope::string& key)
{
	Uplink::Send(source, "SVSJOIN", u->GetUID(), chan);
}

void hybrid::senders::SVSNick::Send(User* u, const Anope::string& newnick, time_t ts)
{
	Uplink::Send(Me, "SVSNICK", u->GetUID(), newnick, ts);
}

void hybrid::senders::SVSPart::Send(const MessageSource& source, User* u, const Anope::string& chan, const Anope::string& reason)
{
	if (!reason.empty())
		Uplink::Send(source, "SVSPART", u->GetUID(), chan, reason);
	else
		Uplink::Send(source, "SVSPART", u->GetUID(), chan);
}

void hybrid::senders::Topic::Send(const MessageSource &source, Channel *channel, const Anope::string &topic, time_t topic_ts, const Anope::string &topic_setter)
{
	Uplink::Send(source, "TBURST", channel->creation_time, channel->name, topic_ts, topic_setter, topic);
}

void hybrid::senders::VhostDel::Send(User* u)
{
	u->RemoveMode(Config->GetClient("HostServ"), "CLOAK", u->host);
}

void hybrid::senders::VhostSet::Send(User* u, const Anope::string& vident, const Anope::string& vhost)
{
	u->SetMode(Config->GetClient("HostServ"), "CLOAK", vhost);
}

void hybrid::senders::Wallops::Send(const MessageSource &source, const Anope::string &msg)
{
	Uplink::Send(source, "GLOBOPS", msg);
}

ServiceBot *hybrid::Proto::FindIntroduced()
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

hybrid::Proto::Proto(Module *creator) : ts6::Proto(creator, "Hybrid 8.2.x")
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

void hybrid::Proto::Handshake()
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

	Uplink::Send("SERVER", Me->GetName(), Me->GetHops() + 1, Me->GetDescription());

	Uplink::Send("SVINFO", 6, 6, 0, Anope::CurTime);
}

void hybrid::Proto::SendEOB()
{
	Uplink::Send(Me, "EOB");
}

bool hybrid::Proto::IsIdentValid(const Anope::string &ident)
{
	if (ident.empty() || ident.length() > Config->GetBlock("networkinfo")->Get<unsigned int>("userlen"))
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

	IRCD->Send<messages::Ping>(Me->GetName(), params[0]);
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

	IRCD->Send<messages::Ping>(Me->GetName(), params[0]);
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
			Anope::Logger.Debug("SJOIN for non-existent user {0} on {1}", buf, params[1]);
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
	hybrid::Proto ircd_proto;

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

	/* Core message senders */
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
	hybrid::senders::Invite sender_invite;
	hybrid::senders::Join sender_join;
	hybrid::senders::Login sender_login;
	hybrid::senders::Logout sender_logout;
	hybrid::senders::ModeUser sender_mode_user;
	hybrid::senders::NickIntroduction sender_nickintroduction;
	hybrid::senders::MessageServer sender_server;
	hybrid::senders::SGLine sender_sgline;
	hybrid::senders::SGLineDel sender_sgline_del;
	hybrid::senders::SQLine sender_sqline;
	hybrid::senders::SQLineDel sender_sqline_del;
	hybrid::senders::SZLine sender_szline;
	hybrid::senders::SZLineDel sender_szline_del;
	hybrid::senders::SVSHold sender_svshold;
	hybrid::senders::SVSHoldDel sender_svsholddel;
	hybrid::senders::SVSNick sender_svsnick;
	hybrid::senders::Topic sender_topic;
	hybrid::senders::VhostDel sender_vhost_del;
	hybrid::senders::VhostSet sender_vhost_set;
	hybrid::senders::Wallops sender_wallops;

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

		, message_certfp(this)
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
		, sender_invite(this)
		, sender_join(this)
		, sender_login(this)
		, sender_logout(this)
		, sender_mode_user(this)
		, sender_nickintroduction(this)
		, sender_server(this)
		, sender_sgline(this)
		, sender_sgline_del(this)
		, sender_sqline(this)
		, sender_sqline_del(this)
		, sender_szline(this)
		, sender_szline_del(this)
		, sender_svshold(this)
		, sender_svsholddel(this)
		, sender_svsnick(this)
		, sender_topic(this)
		, sender_vhost_del(this)
		, sender_vhost_set(this)
		, sender_wallops(this)
	{
		IRCD = &ircd_proto;
	}

	~ProtoHybrid()
	{
		IRCD = nullptr;
	}

	void OnUserNickChange(User *u, const Anope::string &) override
	{
		u->RemoveModeInternal(Me, ModeManager::FindUserModeByName("REGISTERED"));
	}
};

MODULE_INIT(ProtoHybrid)
