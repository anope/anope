/* ircd-hybrid-8 protocol module
 *
 * (C) 2003-2014 Anope Team
 * (C) 2012-2013 by the Hybrid Development Team
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
	BotInfo *FindIntroduced()
	{
		BotInfo *bi = Config->GetClient("OperServ");
		if (bi && bi->introduced)
			return bi;

		for (botinfo_map::iterator it = BotListByNick->begin(), it_end = BotListByNick->end(); it != it_end; ++it)
			if (it->second->introduced)
				return it->second;

		return NULL;
	}

	void SendSVSKillInternal(const MessageSource &source, User *user, const Anope::string &buf) anope_override
	{
		IRCDProto::SendSVSKillInternal(source, user, buf);
		user->KillInternal(source, buf);
	}

  public:
	HybridProto(Module *creator) : IRCDProto(creator, "Hybrid 8.1.x")
	{
		DefaultPseudoclientModes = "+oi";
		CanSVSNick = true;
		CanSNLine = true;
		CanSQLine = true;
		CanSZLine = true;
		CanSVSHold = true;
		CanCertFP = true;
		CanSetVHost = true;
		RequiresID = true;
		MaxModes = 4;
	}

	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "NOTICE $$" << dest->GetName() << " :" << msg;
	}

	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "PRIVMSG $$" << dest->GetName() << " :" << msg;
	}

	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "GLOBOPS :" << buf;
	}

	void SendSQLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message(FindIntroduced()) << "ENCAP * RESV " << (x->expires ? x->expires - Anope::CurTime : 0) << " " << x->mask << " 0 :" << x->reason;
	}

	void SendSGLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(Config->GetClient("OperServ")) << "UNXLINE * " << x->mask;
	}

	void SendSGLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message(Config->GetClient("OperServ")) << "XLINE * " << x->mask << " 0 :" << x->GetReason();
	}

	void SendSZLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(Config->GetClient("OperServ")) << "UNDLINE * " << x->GetHost();
	}

	void SendSZLine(User *, const XLine *x) anope_override
	{
		/* Calculate the time left before this would expire, capping it at 2 days */
		time_t timeleft = x->expires - Anope::CurTime;

		if (timeleft > 172800 || !x->expires)
			timeleft = 172800;

		UplinkSocket::Message(Config->GetClient("OperServ")) << "DLINE * " << timeleft << " " << x->GetHost() << " :" << x->GetReason();
	}

	void SendAkillDel(const XLine *x) anope_override
	{
		if (x->IsRegex() || x->HasNickOrReal())
			return;

		UplinkSocket::Message(Config->GetClient("OperServ")) << "UNKLINE * " << x->GetUser() << " " << x->GetHost();
	}

	void SendSQLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(Config->GetClient("OperServ")) << "UNRESV * " << x->mask;
	}

	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override
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

	void SendAkill(User *u, XLine *x) anope_override
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
					if (x->manager->Check(it->second, x))
						this->SendAkill(it->second, x);

				return;
			}

			const XLine *old = x;

			if (old->manager->HasEntry("*@" + u->host))
				return;

			/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
			XLine *xline = new XLine("*@" + u->host, old->by, old->expires, old->reason, old->id);

			old->manager->AddXLine(xline);
			x = xline;

			Log(Config->GetClient("OperServ"), "akill") << "AKILL: Added an akill for " << x->mask << " because " << u->GetMask() << "#"
					<< u->realname << " matches " << old->mask;
		}

		/* Calculate the time left before this would expire, capping it at 2 days */
		time_t timeleft = x->expires - Anope::CurTime;

		if (timeleft > 172800 || !x->expires)
			timeleft = 172800;

		UplinkSocket::Message(Config->GetClient("OperServ")) << "KLINE * " << timeleft << " " << x->GetUser() << " " << x->GetHost() << " :" << x->GetReason();
	}

	void SendServer(const Server *server) anope_override
	{
		if (server == Me)
			UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :" << server->GetDescription();
		else
			UplinkSocket::Message(Me) << "SID " << server->GetName() << " " << server->GetHops() << " " << server->GetSID() << " :" << server->GetDescription();
	}

	void SendConnect() anope_override
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

		UplinkSocket::Message() << "SVINFO 6 5 0 :" << Anope::CurTime;
	}

	void SendClientIntroduction(User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();

		UplinkSocket::Message(Me) << "UID " << u->nick << " 1 " << u->timestamp << " " << modes << " "
					 << u->GetIdent() << " " << u->host << " 0 " << u->GetUID() << " 0 :" << u->realname;
	}

	void SendEOB() anope_override
	{
		UplinkSocket::Message(Me) << "EOB";
	}

	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "SVSMODE " << u->GetUID() << " " << u->timestamp << " " << buf;
	}

	void SendLogin(User *u, NickAlias *na) anope_override
	{
		IRCD->SendMode(Config->GetClient("NickServ"), u, "+d %s", na->nc->display.c_str());
	}

	void SendLogout(User *u) anope_override
	{
		IRCD->SendMode(Config->GetClient("NickServ"), u, "+d 0");
	}

	void SendChannel(Channel *c) anope_override
	{
		Anope::string modes = c->GetModes(true, true);

		if (modes.empty())
			modes = "+";

		UplinkSocket::Message() << "SJOIN " << c->creation_time << " " << c->name << " " << modes << " :";
	}

	void SendTopic(const MessageSource &source, Channel *c) anope_override
	{
		UplinkSocket::Message(source) << "TBURST " << c->creation_time << " " << c->name << " " << c->topic_ts << " " << c->topic_setter << " :" << c->topic;
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) anope_override
	{
		UplinkSocket::Message(Me) << "SVSNICK " << u->nick << " " << newnick << " " << when;
	}

	void SendSVSHold(const Anope::string &nick, time_t t) anope_override
	{
		XLine x(nick, Me->GetName(), Anope::CurTime + t, "Being held for registered user");
		this->SendSQLine(NULL, &x);
	}

	void SendSVSHoldDel(const Anope::string &nick) anope_override
	{
		XLine x(nick);
		this->SendSQLineDel(&x);
	}

	void SendVhost(User *u, const Anope::string &ident, const Anope::string &host) anope_override
	{
		u->SetMode(Config->GetClient("HostServ"), "CLOAK", host);
	}

	void SendVhostDel(User *u) anope_override
	{
		u->RemoveMode(Config->GetClient("HostServ"), "CLOAK", u->host);
	}

	bool IsIdentValid(const Anope::string &ident) anope_override
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetServer()->Sync(true);
	}
};

struct IRCDMessageJoin : Message::Join
{
	IRCDMessageJoin(Module *creator) : Message::Join(creator, "JOIN") { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->ChangeNick(params[0], convertTo<time_t>(params[1]));
	}
};

struct IRCDMessagePass : IRCDMessage
{
	IRCDMessagePass(Module *creator) : IRCDMessage(creator, "PASS", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*      0        1  2 3   */
	/* PASS password TS 6 0MC */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		UplinkSID = params[3];
	}
};

struct IRCDMessagePong : IRCDMessage
{
	IRCDMessagePong(Module *creator) : IRCDMessage(creator, "PONG", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetServer()->Sync(false);
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*        0          1  2                       */
	/* SERVER hades.arpa 1 :ircd-hybrid test server */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		unsigned int hops = params[1].is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[3], params[2]);

		IRCD->SendPing(Me->GetName(), params[0]);
	}
};

struct IRCDMessageSJoin : IRCDMessage
{
	IRCDMessageSJoin(Module *creator) : IRCDMessage(creator, "SJOIN", 2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
				Log(LOG_DEBUG) << "SJOIN for nonexistant user " << buf << " on " << params[1];
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Anope::string setter;
		sepstream(params[3], '!').GetToken(setter, 0);
		time_t topic_time = Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime;
		Channel *c = Channel::Find(params[1]);

		if (c)
			c->ChangeTopicInternal(setter, params[4], topic_time);
	}
};

struct IRCDMessageTMode : IRCDMessage
{
	IRCDMessageTMode(Module *creator) : IRCDMessage(creator, "TMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
	/* :0MC UID Steve 1 1350157102 +oi ~steve resolved.host 10.0.0.1 0MCAAAAAB 1350157108 :Mining all the time */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Anope::string ip = params[6];

		if (ip == "0") /* Can be 0 for spoofed clients */
			ip.clear();

		NickAlias *na = NULL;
		if (params[8] != "0")
			na = NickAlias::Find(params[8]);

		/* Source is always the server */
		User::OnIntroduce(params[0], params[4], params[5], "",
				ip, source.GetServer(),
				params[9], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0,
				params[3], params[7], na ? *na->nc : NULL);
	}
};

struct IRCDMessageCertFP: IRCDMessage
{
	IRCDMessageCertFP(Module *creator) : IRCDMessage(creator, "CERTFP", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	/*                   0                                                                */
	/* :0MCAAAAAB CERTFP 4C62287BA6776A89CD4F8FF10A62FFB35E79319F51AF6C62C674984974FCCB1D */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.GetUser();

		u->fingerprint = params[0];
		FOREACH_MOD(OnFingerprint, (u));
	}
};

class ProtoHybrid : public Module
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

	void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserModeOperOnly("ADMIN", 'a'));
		ModeManager::AddUserMode(new UserModeOperOnly("CALLERID", 'g'));
		ModeManager::AddUserMode(new UserMode("INVIS", 'i'));
		ModeManager::AddUserMode(new UserModeOperOnly("LOCOPS", 'l'));
		ModeManager::AddUserMode(new UserModeOperOnly("OPER", 'o'));
		ModeManager::AddUserMode(new UserModeNoone("REGISTERED", 'r'));
		ModeManager::AddUserMode(new UserModeOperOnly("SNOMASK", 's'));
		ModeManager::AddUserMode(new UserMode("WALLOPS", 'w'));
		ModeManager::AddUserMode(new UserModeOperOnly("OPERWALLS", 'z'));
		ModeManager::AddUserMode(new UserMode("DEAF", 'D'));
		ModeManager::AddUserMode(new UserModeOperOnly("HIDEOPER", 'H'));
		ModeManager::AddUserMode(new UserMode("REGPRIV", 'R'));
		ModeManager::AddUserMode(new UserModeNoone("SSL", 'S'));
		ModeManager::AddUserMode(new UserMode("CLOAK", 'x'));

		/* b/e/I */
		ModeManager::AddChannelMode(new ChannelModeList("BAN", 'b'));
		ModeManager::AddChannelMode(new ChannelModeList("EXCEPT", 'e'));
		ModeManager::AddChannelMode(new ChannelModeList("INVITEOVERRIDE", 'I'));

		/* v/h/o/a/q */
		ModeManager::AddChannelMode(new ChannelModeStatus("VOICE", 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus("HALFOP", 'h', '%', 1));
		ModeManager::AddChannelMode(new ChannelModeStatus("OP", 'o', '@', 2));

		/* l/k */
		ModeManager::AddChannelMode(new ChannelModeParam("LIMIT", 'l', true));
		ModeManager::AddChannelMode(new ChannelModeKey('k'));

		/* Add channel modes */
		ModeManager::AddChannelMode(new ChannelMode("BLOCKCOLOR", 'c'));
		ModeManager::AddChannelMode(new ChannelMode("INVITE", 'i'));
		ModeManager::AddChannelMode(new ChannelMode("MODERATED", 'm'));
		ModeManager::AddChannelMode(new ChannelMode("NOEXTERNAL", 'n'));
		ModeManager::AddChannelMode(new ChannelMode("PRIVATE", 'p'));
		ModeManager::AddChannelMode(new ChannelModeNoone("REGISTERED", 'r'));
		ModeManager::AddChannelMode(new ChannelMode("SECRET", 's'));
		ModeManager::AddChannelMode(new ChannelMode("TOPIC", 't'));
		ModeManager::AddChannelMode(new ChannelModeOperOnly("OPERONLY", 'O'));
		ModeManager::AddChannelMode(new ChannelMode("REGMODERATED", 'M'));
		ModeManager::AddChannelMode(new ChannelMode("REGISTEREDONLY", 'R'));
		ModeManager::AddChannelMode(new ChannelMode("SSL", 'S'));
	}

public:
	ProtoHybrid(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR),
		ircd_proto(this),
		message_away(this), message_capab(this), message_error(this), message_invite(this), message_kick(this),
		message_kill(this), message_mode(this), message_motd(this), message_notice(this), message_part(this),
		message_ping(this), message_privmsg(this), message_quit(this), message_squit(this), message_stats(this),
		message_time(this), message_topic(this), message_version(this), message_whois(this),
		message_bmask(this), message_eob(this), message_join(this),
		message_nick(this), message_pass(this), message_pong(this), message_server(this), message_sid(this),
		message_sjoin(this), message_svsmode(this), message_tburst(this), message_tmode(this), message_uid(this),
		message_certfp(this)
	{
		if (Config->GetModule(this))
			this->AddModes();
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveModeInternal(Me, ModeManager::FindUserModeByName("REGISTERED"));
	}
};

MODULE_INIT(ProtoHybrid)
