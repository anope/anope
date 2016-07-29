/* ircd-hybrid-8 protocol module
 *
 * (C) 2003-2014 Anope Team
 * (C) 2012-2015 ircd-hybrid development team
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#include "module.h"

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

	void SendSVSKillInternal(const MessageSource &source, User *user, const Anope::string &buf) override
	{
		IRCDProto::SendSVSKillInternal(source, user, buf);
		user->KillInternal(source, buf);
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
		CanSZLine = true;
		CanCertFP = true;
		CanSetVHost = true;
		RequiresID = true;
		MaxModes = 6;
	}

	void SendInvite(const MessageSource &source, const Channel *c, User *u) override
	{
		UplinkSocket::Message(source) << "INVITE " << u->GetUID() << " " << c->name << " " << c->creation_time;
	}

	void SendGlobalNotice(ServiceBot *bi, const Server *dest, const Anope::string &msg) override
	{
		UplinkSocket::Message(bi) << "NOTICE $$" << dest->GetName() << " :" << msg;
	}

	void SendGlobalPrivmsg(ServiceBot *bi, const Server *dest, const Anope::string &msg) override
	{
		UplinkSocket::Message(bi) << "PRIVMSG $$" << dest->GetName() << " :" << msg;
	}

	void SendSQLine(User *, XLine *x) override
	{
		UplinkSocket::Message(FindIntroduced()) << "ENCAP * RESV " << (x->GetExpires() ? x->GetExpires() - Anope::CurTime : 0) << " " << x->GetMask() << " 0 :" << x->GetReason();
	}

	void SendSGLineDel(XLine *x) override
	{
		UplinkSocket::Message(Config->GetClient("OperServ")) << "UNXLINE * " << x->GetMask();
	}

	void SendSGLine(User *, XLine *x) override
	{
		UplinkSocket::Message(Config->GetClient("OperServ")) << "XLINE * " << x->GetMask() << " 0 :" << x->GetReason();
	}

	void SendSZLineDel(XLine *x) override
	{
		UplinkSocket::Message(Config->GetClient("OperServ")) << "UNDLINE * " << x->GetHost();
	}

	void SendSZLine(User *, XLine *x) override
	{
		/* Calculate the time left before this would expire, capping it at 2 days */
		time_t timeleft = x->GetExpires() - Anope::CurTime;

		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;

		UplinkSocket::Message(Config->GetClient("OperServ")) << "DLINE * " << timeleft << " " << x->GetHost() << " :" << x->GetReason();
	}

	void SendAkillDel(XLine *x) override
	{
		if (x->IsRegex() || x->HasNickOrReal())
			return;

		UplinkSocket::Message(Config->GetClient("OperServ")) << "UNKLINE * " << x->GetUser() << " " << x->GetHost();
	}

	void SendSQLineDel(XLine *x) override
	{
		UplinkSocket::Message(Config->GetClient("OperServ")) << "UNRESV * " << x->GetMask();
	}

	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override
	{
		/*
		 * Note that we must send our modes with the SJOIN and
		 * can not add them to the mode stacker because ircd-hybrid
		 * does not allow *any* client to op itself
		 */
		UplinkSocket::Message() << "SJOIN " << c->creation_time << " " << c->name << " +"
					<< c->GetModes(true, true) << " :"
					<< (status != NULL ? status->BuildModePrefixList() : "") << user->GetUID();

		/* And update our internal status for this user since this is not going through our mode handling system */
		if (status != NULL)
		{
			ChanUserContainer *uc = c->FindUser(user);

			if (uc != NULL)
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

		UplinkSocket::Message(Config->GetClient("OperServ")) << "KLINE * " << timeleft << " " << x->GetUser() << " " << x->GetHost() << " :" << x->GetReason();
	}

	void SendServer(const Server *server) override
	{
		if (server == Me)
			UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() + 1 << " :" << server->GetDescription();
		else
			UplinkSocket::Message(Me) << "SID " << server->GetName() << " " << server->GetHops() + 1 << " " << server->GetSID() << " :" << server->GetDescription();
	}

	void SendConnect() override
	{
		UplinkSocket::Message() << "PASS " << Config->Uplinks[Anope::CurrentUplink].password << " TS 6 :" << Me->GetSID();

		/*
		 * As of October 13, 2012, ircd-hybrid-8 does support the following capabilities
		 * which are required to work with IRC-services:
		 *
		 * QS     - Can handle quit storm removal
		 * EX     - Can do channel +e exemptions
		 * CHW    - Can do channel wall @#
		 * IE     - Can do invite exceptions
		 * KNOCK  - Supports KNOCK
		 * TBURST - Supports topic burst
		 * ENCAP  - Supports ENCAP
		 * HOPS   - Supports HalfOps
		 * SVS    - Supports services
		 * EOB    - Supports End Of Burst message
		 * TS6    - Capable of TS6 support
		 */
		UplinkSocket::Message() << "CAPAB :QS EX CHW IE ENCAP TBURST SVS HOPS EOB TS6";

		SendServer(Me);

		UplinkSocket::Message() << "SVINFO 6 6 0 :" << Anope::CurTime;
	}

	void SendClientIntroduction(User *u) override
	{
		Anope::string modes = "+" + u->GetModes();

		UplinkSocket::Message(Me) << "UID " << u->nick << " 1 " << u->timestamp << " " << modes << " "
					 << u->GetIdent() << " " << u->host << " 0 " << u->GetUID() << " * :" << u->realname;
	}

	void SendEOB() override
	{
		UplinkSocket::Message(Me) << "EOB";
	}

	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &buf) override
	{
		UplinkSocket::Message(source) << "SVSMODE " << u->GetUID() << " " << u->timestamp << " " << buf;
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

		UplinkSocket::Message() << "SJOIN " << c->creation_time << " " << c->name << " " << modes << " :";
	}

	void SendTopic(const MessageSource &source, Channel *c) override
	{
		UplinkSocket::Message(source) << "TBURST " << c->creation_time << " " << c->name << " " << c->topic_ts << " " << c->topic_setter << " :" << c->topic;
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) override
	{
		UplinkSocket::Message(Me) << "SVSNICK " << u->GetUID() << " " << newnick << " " << when;
	}

	void SendSVSJoin(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &) override
	{
		UplinkSocket::Message(source) << "SVSJOIN " << u->GetUID() << " " << chan;
	}

	void SendSVSPart(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &param) override
	{
		if (!param.empty())
			UplinkSocket::Message(source) << "SVSPART " << u->GetUID() << " " << chan << " :" << param;
		else
			UplinkSocket::Message(source) << "SVSPART " << u->GetUID() << " " << chan;
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

struct IRCDMessageBMask : IRCDMessage
{
	IRCDMessageBMask(Module *creator) : IRCDMessage(creator, "BMASK", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*            0          1        2  3              */
	/* :0MC BMASK 1350157102 #channel b :*!*@*.test.com */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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
};

struct IRCDMessageEOB : IRCDMessage
{
	IRCDMessageEOB(Module *craetor) : IRCDMessage(craetor, "EOB", 0) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		source.GetServer()->Sync(true);
	}
};

struct IRCDMessageJoin : Message::Join
{
	IRCDMessageJoin(Module *creator) : Message::Join(creator, "JOIN") { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (params.size() < 2)
			return;

		std::vector<Anope::string> p = params;
		p.erase(p.begin());

		return Message::Join::Run(source, p);
	}
};

struct IRCDMessageNick : IRCDMessage
{
	IRCDMessageNick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	/*                 0       1          */
	/* :0MCAAAAAB NICK newnick 1350157102 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		source.GetUser()->ChangeNick(params[0], convertTo<time_t>(params[1]));
	}
};

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

struct IRCDMessagePong : IRCDMessage
{
	IRCDMessagePong(Module *creator) : IRCDMessage(creator, "PONG", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		source.GetServer()->Sync(false);
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*        0          1  2                       */
	/* SERVER hades.arpa 1 :ircd-hybrid test server */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		/* Servers other than our immediate uplink are introduced via SID */
		if (params[1] != "1")
			return;

		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);

		IRCD->SendPing(Me->GetName(), params[0]);
	}
};

struct IRCDMessageSID : IRCDMessage
{
	IRCDMessageSID(Module *creator) : IRCDMessage(creator, "SID", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*          0          1 2    3                       */
	/* :0MC SID hades.arpa 2 4XY :ircd-hybrid test server */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		unsigned int hops = params[1].is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[3], params[2]);

		IRCD->SendPing(Me->GetName(), params[0]);
	}
};

struct IRCDMessageSJoin : IRCDMessage
{
	IRCDMessageSJoin(Module *creator) : IRCDMessage(creator, "SJOIN", 2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		Anope::string modes;
		if (params.size() >= 3)
			for (unsigned i = 2; i < params.size() - 1; ++i)
				modes += " " + params[i];
		if (!modes.empty())
			modes.erase(modes.begin());

		std::list<Message::Join::SJoinUser> users;

		spacesepstream sep(params[params.size() - 1]);
		Anope::string buf;

		while (sep.GetToken(buf))
		{
			Message::Join::SJoinUser sju;

			/* Get prefixes from the nick */
			for (char ch; (ch = ModeManager::GetStatusChar(buf[0]));)
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

		time_t ts = Anope::string(params[0]).is_pos_number_only() ? convertTo<time_t>(params[0]) : Anope::CurTime;
		Message::Join::SJoin(source, params[1], ts, modes, users);
	}
};

struct IRCDMessageSVSMode : IRCDMessage
{
	IRCDMessageSVSMode(Module *creator) : IRCDMessage(creator, "SVSMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	/*
	 * parv[0] = nickname
	 * parv[1] = TS
	 * parv[2] = mode
	 * parv[3] = optional argument (services id)
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = User::Find(params[0]);
		if (!u)
			return;

		if (!params[1].is_pos_number_only() || convertTo<time_t>(params[1]) != u->timestamp)
			return;

		u->SetModesInternal(source, "%s", params[2].c_str());
	}
};

struct IRCDMessageTBurst : IRCDMessage
{
	IRCDMessageTBurst(Module *creator) : IRCDMessage(creator, "TBURST", 5) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		Anope::string setter;
		sepstream(params[3], '!').GetToken(setter, 0);
		time_t topic_time = Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime;
		Channel *c = Channel::Find(params[1]);

		if (c)
			c->ChangeTopicInternal(NULL, setter, params[4], topic_time);
	}
};

struct IRCDMessageTMode : IRCDMessage
{
	IRCDMessageTMode(Module *creator) : IRCDMessage(creator, "TMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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
};

struct IRCDMessageUID : IRCDMessage
{
	IRCDMessageUID(Module *creator) : IRCDMessage(creator, "UID", 10) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*          0     1 2          3   4      5             6        7         8           9                   */
	/* :0MC UID Steve 1 1350157102 +oi ~steve resolved.host 10.0.0.1 0MCAAAAAB Steve      :Mining all the time */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		Anope::string ip = params[6];

		if (ip == "0") /* Can be 0 for spoofed clients */
			ip.clear();

		NickServ::Nick *na = NULL;
		if (params[8] != "0" && params[8] != "*")
			na = NickServ::FindNick(params[8]);

		/* Source is always the server */
		User::OnIntroduce(params[0], params[4], params[5], "",
				ip, source.GetServer(),
				params[9], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0,
				params[3], params[7], na ? na->GetAccount() : NULL);
	}
};

struct IRCDMessageCertFP: IRCDMessage
{
	IRCDMessageCertFP(Module *creator) : IRCDMessage(creator, "CERTFP", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	/*                   0                                                                */
	/* :0MCAAAAAB CERTFP 4C62287BA6776A89CD4F8FF10A62FFB35E79319F51AF6C62C674984974FCCB1D */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();

		u->fingerprint = params[0];
		EventManager::Get()->Dispatch(&Event::Fingerprint::OnFingerprint, u);
	}
};

class ProtoHybrid : public Module
	, public EventHook<Event::UserNickChange>
{
	HybridProto ircd_proto;

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

	/* Our message handlers */
	IRCDMessageBMask message_bmask;
	IRCDMessageEOB message_eob;
	IRCDMessageJoin message_join;
	IRCDMessageNick message_nick;
	IRCDMessagePass message_pass;
	IRCDMessagePong message_pong;
	IRCDMessageServer message_server;
	IRCDMessageSID message_sid;
	IRCDMessageSJoin message_sjoin;
	IRCDMessageSVSMode message_svsmode;
	IRCDMessageTBurst message_tburst;
	IRCDMessageTMode message_tmode;
	IRCDMessageUID message_uid;
	IRCDMessageCertFP message_certfp;

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
