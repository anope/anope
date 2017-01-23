/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
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
#include "modules/protocol/rfc1459.h"
#include "modules/protocol/bahamut.h"

void bahamut::senders::Akill::Send(User* u, XLine* x)
{
	if (x->IsRegex() || x->HasNickOrReal())
	{
		if (!u)
		{
			/* No user (this akill was just added), and contains nick and/or realname. Find users that match and ban them */
			for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
				if (x->GetManager()->Check(it->second, x))
					this->Send(it->second, x);
			return;
		}

		XLine *old = x;

		if (old->GetManager()->HasEntry("*@" + u->host))
			return;

		/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
		x = Serialize::New<XLine *>();
		x->SetMask("*@" + u->host);
		x->SetBy(old->GetBy());
		x->SetExpires(old->GetExpires());
		x->SetReason(old->GetReason());
		x->SetID(old->GetID());
		old->GetManager()->AddXLine(x);

		Anope::Logger.Bot("OperServ").Category("akill").Log(_("AKILL: Added an akill for {0} because {1}#{2} matches {3}"),
				x->GetMask(), u->GetMask(), u->realname, old->GetMask());
	}

	/* ZLine if we can instead */
	if (x->GetUser() == "*")
	{
		cidr a(x->GetHost());
		if (a.valid())
		{
			IRCD->Send<messages::SZLine>(u, x);
			return;
		}
	}

	// Calculate the time left before this would expire, capping it at 2 days
	time_t timeleft = x->GetExpires() - Anope::CurTime;
	if (timeleft > 172800)
		timeleft = 172800;

	Uplink::Send("AKILL", x->GetHost(), x->GetUser(), timeleft, x->GetBy(), Anope::CurTime, x->GetReason());
}

void bahamut::senders::AkillDel::Send(XLine* x)
{
	if (x->IsRegex() || x->HasNickOrReal())
		return;

	/* ZLine if we can instead */
	if (x->GetUser() == "*")
	{
		cidr a(x->GetHost());
		if (a.valid())
		{
			IRCD->Send<messages::SZLineDel>(x);
			return;
		}
	}

	Uplink::Send("RAKILL", x->GetHost(), x->GetUser());
}

void bahamut::senders::MessageChannel::Send(Channel* c)
{
	Anope::string modes = c->GetModes(true, true);
	if (modes.empty())
		modes = "+";
	Uplink::Send("SJOIN", c->creation_time, c->name, modes, "");
}

void bahamut::senders::Join::Send(User* user, Channel* c, const ChannelStatus* status)
{
	Uplink::Send(user, "SJOIN", c->creation_time, c->name);

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

void bahamut::senders::Kill::Send(const MessageSource &source, const Anope::string &target, const Anope::string &reason)
{
	Uplink::Send(source, "SVSKILL", target, reason);
}

/*
  Note: if the stamp is null 0, the below usage is correct of Bahamut
*/
void bahamut::senders::Kill::Send(const MessageSource &source, User *user, const Anope::string &reason)
{
	Uplink::Send(source, "SVSKILL", user->nick, reason);
}

void bahamut::senders::Login::Send(User *u, NickServ::Nick *na)
{
	IRCD->SendMode(Config->GetClient("NickServ"), u, "+d {0}", u->signon);
}

void bahamut::senders::Logout::Send(User *u)
{
	IRCD->SendMode(Config->GetClient("NickServ"), u, "+d 1");
}

void bahamut::senders::ModeChannel::Send(const MessageSource &source, Channel *channel, const Anope::string &modes)
{
	IRCMessage message(source, "MODE", channel->name, channel->creation_time);
	message.TokenizeAndPush(modes);
	Uplink::SendMessage(message);
}

void bahamut::senders::ModeUser::Send(const MessageSource &source, User *user, const Anope::string &modes)
{
	IRCMessage message(source, "SVSMODE", user->nick, user->timestamp);
	message.TokenizeAndPush(modes);
	Uplink::SendMessage(message);
}

void bahamut::senders::NickIntroduction::Send(User *user)
{
	Anope::string modes = "+" + user->GetModes();
	Uplink::Send("NICK", user->nick, 1, user->timestamp, modes, user->GetIdent(), user->host, user->server->GetName(), 0, 0, user->realname);
}

void bahamut::senders::NOOP::Send(Server* server, bool set)
{
	Uplink::Send("SVSNOOP", server->GetSID(), set ? "+" : "-");
}

void bahamut::senders::SGLine::Send(User*, XLine* x)
{
	Uplink::Send("SGLINE", x->GetMask().length(), x->GetMask() + ":" + x->GetReason());
}

void bahamut::senders::SGLineDel::Send(XLine* x)
{
	Uplink::Send("UNSGLINE", 0, x->GetMask());
}

void bahamut::senders::SQLine::Send(User*, XLine* x)
{
	Uplink::Send(Me, "SQLINE", x->GetMask(), x->GetReason());
}

void bahamut::senders::SQLineDel::Send(XLine* x)
{
	Uplink::Send("UNSQLINE", x->GetMask());
}

void bahamut::senders::SZLine::Send(User*, XLine* x)
{
	// Calculate the time left before this would expire, capping it at 2 days
	time_t timeleft = x->GetExpires() - Anope::CurTime;
	if (timeleft > 172800 || !x->GetExpires())
		timeleft = 172800;
	/* this will likely fail so its only here for legacy */
	Uplink::Send("SZLINE", x->GetHost(), x->GetReason());
	/* this is how we are supposed to deal with it */
	Uplink::Send("AKILL", x->GetHost(), "*", timeleft, x->GetBy(), Anope::CurTime, x->GetReason());
}

void bahamut::senders::SZLineDel::Send(XLine* x)
{
	/* this will likely fail so its only here for legacy */
	Uplink::Send("UNSZLINE", 0, x->GetHost());
	/* this is how we are supposed to deal with it */
	Uplink::Send("RAKILL", x->GetHost(), "*");
}

void bahamut::senders::SVSHold::Send(const Anope::string& nick, time_t t)
{
	Uplink::Send(Me, "SVSHOLD", nick, t, "Being held for registered user");
}

void bahamut::senders::SVSHoldDel::Send(const Anope::string& nick)
{
	Uplink::Send(Me, "SVSHOLD", nick, 0);
}

void bahamut::senders::SVSNick::Send(User* u, const Anope::string& newnick, time_t ts)
{
	Uplink::Send("SVSNICK", u->GetUID(), newnick, ts);
}

void bahamut::senders::Topic::Send(const MessageSource &source, Channel *channel, const Anope::string &topic, time_t topic_ts, const Anope::string &topic_setter)
{
	Uplink::Send(source, "TOPIC", channel->name, topic_setter, topic_ts, topic);
}

void bahamut::senders::Wallops::Send(const MessageSource &source, const Anope::string &msg)
{
	Uplink::Send(source, "GLOBOPS", msg);
}

#warning "not used"
class ChannelModeFlood : public ChannelModeParam
{
 public:
	ChannelModeFlood(char modeChar, bool minusNoArg) : ChannelModeParam("FLOOD", modeChar, minusNoArg) { }

	bool IsValid(Anope::string &value) const override
	{
		try
		{
			Anope::string rest;
			if (!value.empty() && value[0] != ':' && convertTo<int>(value[0] == '*' ? value.substr(1) : value, rest, false) > 0 && rest[0] == ':' && rest.length() > 1 && convertTo<int>(rest.substr(1), rest, false) > 0 && rest.empty())
				return true;
		}
		catch (const ConvertException &) { }

		return false;
	}
};

bahamut::Proto::Proto(Module *creator) : IRCDProto(creator, "Bahamut 1.8.x")
{
	DefaultPseudoclientModes = "+";
	CanSVSNick = true;
	CanSNLine = true;
	CanSQLine = true;
	CanSQLineChannel = true;
	CanSZLine = true;
	CanSVSHold = true;
	MaxModes = 60;
}

void bahamut::Proto::SendBOB()
{
	Uplink::Send("BURST");
}

void bahamut::Proto::SendEOB()
{
	Uplink::Send("BURST", 0);
}

void bahamut::Proto::Handshake()
{
	Uplink::Send("PASS", Config->Uplinks[Anope::CurrentUplink].password, "TS");
	Uplink::Send("CAPAB", "SSJOIN NOQUIT BURST UNCONNECT NICKIP TSMODE TS3");
	IRCD->Send<messages::MessageServer>(Me);
	/*
	 * SVINFO
	 *	   parv[0] = sender prefix
	 *	   parv[1] = TS_CURRENT for the server
	 *	   parv[2] = TS_MIN for the server
	 *	   parv[3] = server is standalone or connected to non-TS only
	 *	   parv[4] = server's idea of UTC time
	 */
	Uplink::Send("SVINFO", 3, 1, 0, Anope::CurTime);
	this->SendBOB();
}

void bahamut::Burst::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Server *s = source.GetServer();
	s->Sync(true);
}

void bahamut::Mode::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	if (params.size() > 2 && IRCD->IsChannelValid(params[0]))
	{
		Channel *c = Channel::Find(params[0]);
		time_t ts = 0;

		try
		{
			ts = convertTo<time_t>(params[1]);
		}
		catch (const ConvertException &) { }

		Anope::string modes = params[2];
		for (unsigned int i = 3; i < params.size(); ++i)
			modes += " " + params[i];

		if (c)
			c->SetModesInternal(source, modes, ts);
	}
	else
	{
		User *u = User::Find(params[0]);
		if (u)
			u->SetModesInternal(source, "%s", params[1].c_str());
	}
}

/*
 ** NICK - new
 **	  source  = NULL
 **	  parv[0] = nickname
 **	  parv[1] = hopcount
 **	  parv[2] = timestamp
 **	  parv[3] = modes
 **	  parv[4] = username
 **	  parv[5] = hostname
 **	  parv[6] = server
 **	  parv[7] = servicestamp
 **	  parv[8] = IP
 **	  parv[9] = info
 ** NICK - change
 **	  source  = oldnick
 **	  parv[0] = new nickname
 **	  parv[1] = hopcount
 */
void bahamut::Nick::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	if (params.size() == 10)
	{
		Server *s = Server::Find(params[6]);
		if (s == nullptr)
		{
			Anope::Logger.Debug("User {0} introduced from non-existent server {1}", params[0], params[6]);
			return;
		}

		NickServ::Nick *na = nullptr;
		time_t signon = 0, stamp = 0;

		try
		{
			signon = convertTo<time_t>(params[2]);
			stamp = convertTo<time_t>(params[7]);
		}
		catch (const ConvertException &)
		{
		}

		if (signon && signon == stamp && NickServ::service)
			na = NickServ::service->FindNick(params[0]);

		User::OnIntroduce(params[0], params[4], params[5], "", params[8], s, params[9], signon, params[3], "", na ? na->GetAccount() : nullptr);
	}
	else
	{
		User *u = source.GetUser();

		if (u)
			u->ChangeNick(params[0]);
	}
}

void bahamut::ServerMessage::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	unsigned int hops = 0;

	try
	{
		hops = convertTo<unsigned>(params[1]);
	}
	catch (const ConvertException &) { }

	new Server(source.GetServer(), params[0], hops, params[2]);
}

void bahamut::SJoin::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Anope::string modes;
	if (params.size() >= 4)
		for (unsigned i = 2; i < params.size(); ++i)
			modes += " " + params[i];
	if (!modes.empty())
		modes.erase(modes.begin());

	std::list<rfc1459::Join::SJoinUser> users;

	/* For some reason, bahamut will send a SJOIN from the user joining a channel
	 * if the channel already existed
	 */
	if (source.GetUser())
	{
		rfc1459::Join::SJoinUser sju;
		sju.second = source.GetUser();
		users.push_back(sju);
	}
	else
	{
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
				Anope::Logger.Log("SJOIN for non-existent user {0} on {1}", buf, params[1]);
				continue;
			}

			users.push_back(sju);
		}
	}

	time_t ts = Anope::CurTime;

	try
	{
		ts = convertTo<time_t>(params[0]);
	}
	catch (const ConvertException &) { }

	rfc1459::Join::SJoin(source, params[1], ts, modes, users);
}

void bahamut::Topic::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Channel *c = Channel::Find(params[0]);
	time_t ts = Anope::CurTime;

	try
	{
		ts = convertTo<time_t>(params[2]);
	}
	catch (const ConvertException &) { }

	if (c)
		c->ChangeTopicInternal(source.GetUser(), params[1], params[3], ts);
}

class ProtoBahamut : public Module
	, public EventHook<Event::UserNickChange>
{
	bahamut::Proto ircd_proto;

	/* Core message handlers */
	rfc1459::Away message_away;
	rfc1459::Capab message_capab;
	rfc1459::Error message_error;
	rfc1459::Invite message_invite;
	rfc1459::Join message_join;
	rfc1459::Kick message_kick;
	rfc1459::Kill message_kill;
	rfc1459::MOTD message_motd;
	rfc1459::Notice message_notice;
	rfc1459::Part message_part;
	rfc1459::Ping message_ping;
	rfc1459::Privmsg message_privmsg;
	rfc1459::Quit message_quit;
	rfc1459::SQuit message_squit;
	rfc1459::Stats message_stats;
	rfc1459::Time message_time;
	rfc1459::Version message_version;
	rfc1459::Whois message_whois;

	/* Our message handlers */
	bahamut::Burst message_burst;
	bahamut::Mode message_mode, message_svsmode;
	bahamut::Nick message_nick;
	bahamut::ServerMessage message_server;
	bahamut::SJoin message_sjoin;
	bahamut::Topic message_topic;

	/* Core message senders */
	rfc1459::senders::GlobalNotice sender_global_notice;
	rfc1459::senders::GlobalPrivmsg sender_global_privmsg;
	rfc1459::senders::Invite sender_invite;
	rfc1459::senders::Kick sender_kick;
	rfc1459::senders::NickChange sender_nickchange;
	rfc1459::senders::Notice sender_notice;
	rfc1459::senders::Part sender_part;
	rfc1459::senders::Ping sender_ping;
	rfc1459::senders::Pong sender_pong;
	rfc1459::senders::Privmsg sender_privmsg;
	rfc1459::senders::Quit sender_quit;
	rfc1459::senders::MessageServer sender_server;
	rfc1459::senders::SQuit sender_squit;

	/* Our message senders */
	bahamut::senders::Akill sender_akill;
	bahamut::senders::AkillDel sender_akill_del;
	bahamut::senders::MessageChannel sender_channel;
	bahamut::senders::Join sender_join;
	bahamut::senders::Kill sender_kill;
	bahamut::senders::Login sender_login;
	bahamut::senders::Logout sender_logout;
	bahamut::senders::ModeChannel sender_mode_channel;
	bahamut::senders::ModeUser sender_mode_user;
	bahamut::senders::NickIntroduction sender_nickintroduction;
	bahamut::senders::NOOP sender_noop;
	bahamut::senders::SGLine sender_sgline;
	bahamut::senders::SGLineDel sender_sgline_del;
	bahamut::senders::SQLine sender_sqline;
	bahamut::senders::SQLineDel sender_sqline_del;
	bahamut::senders::SZLine sender_szline;
	bahamut::senders::SZLineDel sender_szline_del;
	bahamut::senders::SVSHold sender_svshold;
	bahamut::senders::SVSHoldDel sender_svsholddel;
	bahamut::senders::SVSNick sender_svsnick;
	bahamut::senders::Topic sender_topic;
	bahamut::senders::Wallops sender_wallops;

 public:
	ProtoBahamut(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR)
		, EventHook<Event::UserNickChange>(this)
		, ircd_proto(this)
		, message_away(this)
		, message_capab(this)
		, message_error(this)
		, message_invite(this)
		, message_join(this)
		, message_kick(this)
		, message_kill(this)
		, message_motd(this)
		, message_notice(this)
		, message_part(this)
		, message_ping(this)
		, message_privmsg(this)
		, message_quit(this)
		, message_squit(this)
		, message_stats(this)
		, message_time(this)
		, message_version(this)
		, message_whois(this)

		, message_burst(this)
		, message_mode(this, "MODE")
		, message_svsmode(this, "SVSMODE")
		, message_nick(this)
		, message_server(this)
		, message_sjoin(this)
		, message_topic(this)

		, sender_akill(this)
		, sender_akill_del(this)
		, sender_channel(this)
		, sender_global_notice(this)
		, sender_global_privmsg(this)
		, sender_invite(this)
		, sender_join(this)
		, sender_kick(this)
		, sender_kill(this)
		, sender_login(this)
		, sender_logout(this)
		, sender_mode_channel(this)
		, sender_mode_user(this)
		, sender_nickchange(this)
		, sender_nickintroduction(this)
		, sender_noop(this)
		, sender_topic(this)
		, sender_notice(this)
		, sender_part(this)
		, sender_ping(this)
		, sender_pong(this)
		, sender_privmsg(this)
		, sender_quit(this)
		, sender_server(this)
		, sender_sgline(this)
		, sender_sgline_del(this)
		, sender_sqline(this)
		, sender_sqline_del(this)
		, sender_szline(this)
		, sender_szline_del(this)
		, sender_squit(this)
		, sender_svshold(this)
		, sender_svsholddel(this)
		, sender_svsnick(this)
		, sender_wallops(this)
	{
		IRCD = &ircd_proto;
	}

	~ProtoBahamut()
	{
		IRCD = nullptr;
	}

	void OnUserNickChange(User *u, const Anope::string &) override
	{
		u->RemoveModeInternal(Me, ModeManager::FindUserModeByName("REGISTERED"));
		sender_logout.Send(u);
	}
};

MODULE_INIT(ProtoBahamut)
