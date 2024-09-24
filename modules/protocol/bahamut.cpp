/* Bahamut functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class ChannelModeFlood final
	: public ChannelModeParam
{
public:
	ChannelModeFlood(char modeChar, bool minusNoArg) : ChannelModeParam("FLOOD", modeChar, minusNoArg) { }

	bool IsValid(Anope::string &value) const override
	{
		if (value.empty() || value[0] == ':')
			return false;

		Anope::string rest;
		auto num1 = Anope::Convert<int>(value[0] == '*' ? value.substr(1) : value, 0, &rest);
		if (num1 <= 0 || rest[0] != ':' || rest.length() <= 1)
			return false;

		return Anope::Convert<int>(rest.substr(1), 0, &rest) > 0 && rest.empty();
	}
};

class BahamutIRCdProto final
	: public IRCDProto
{
public:
	BahamutIRCdProto(Module *creator) : IRCDProto(creator, "Bahamut 2+")
	{
		DefaultPseudoclientModes = "+";
		CanSVSNick = true;
		CanSVSNOOP = true;
		CanSNLine = true;
		CanSQLine = true;
		CanSQLineChannel = true;
		CanSZLine = true;
		CanSVSHold = true;
		MaxModes = 60;
	}

	void SendModeInternal(const MessageSource &source, Channel *chan, const Anope::string &modes, const std::vector<Anope::string> &values) override
	{
		auto params = values;
		params.insert(params.begin(), { chan->name, Anope::ToString(chan->creation_time), modes });
		Uplink::SendInternal({}, source, "MODE", params);
	}

	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &modes, const std::vector<Anope::string> &values) override
	{
		auto params = values;
		params.insert(params.begin(), { u->nick, Anope::ToString(u->timestamp), modes });
		Uplink::SendInternal({}, source, "SVSMODE", params);
	}

	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "NOTICE", "$" + dest->GetName(), msg);
	}

	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "PRIVMSG", "$" + dest->GetName(), msg);
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick, time_t time) override
	{
		Uplink::Send("SVSHOLD", nick, time, "Being held for a registered user");
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) override
	{
		Uplink::Send("SVSHOLD", nick, 0);
	}

	/* SQLINE */
	void SendSQLine(User *, const XLine *x) override
	{
		Uplink::Send("SQLINE", x->mask, x->reason);
	}

	/* UNSLINE */
	void SendSGLineDel(const XLine *x) override
	{
		Uplink::Send("UNSGLINE", 0, x->mask);
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x) override
	{
		Uplink::Send("RAKILL", x->GetHost(), '*');
	}

	/* SZLINE */
	void SendSZLine(User *, const XLine *x) override
	{
		// Calculate the time left before this would expire
		time_t timeleft = x->expires ? x->expires - Anope::CurTime : x->expires;
		Uplink::Send("AKILL", x->GetHost(), '*', timeleft, x->by, Anope::CurTime, x->GetReason());
	}

	/* SVSNOOP */
	void SendSVSNOOP(const Server *server, bool set) override
	{
		Uplink::Send("SVSNOOP", server->GetName(), set ? '+' : '-');
	}

	/* SGLINE */
	void SendSGLine(User *, const XLine *x) override
	{
		Uplink::Send("SGLINE", x->mask.length(), x->mask, x->GetReason());
	}

	/* RAKILL */
	void SendAkillDel(const XLine *x) override
	{
		if (x->IsRegex() || x->HasNickOrReal())
			return;

		/* ZLine if we can instead */
		if (x->GetUser() == "*")
		{
			cidr a(x->GetHost());
			if (a.valid())
			{
				IRCD->SendSZLineDel(x);
				return;
			}
		}

		Uplink::Send("RAKILL", x->GetHost(), x->GetUser());
	}

	/* TOPIC */
	void SendTopic(const MessageSource &source, Channel *c) override
	{
		Uplink::Send(source, "TOPIC", c->name, c->topic_setter, c->topic_ts, c->topic);
	}

	/* UNSQLINE */
	void SendSQLineDel(const XLine *x) override
	{
		Uplink::Send("UNSQLINE", x->mask);
	}

	/* JOIN - SJOIN */
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override
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

			BotInfo *setter = BotInfo::Find(user->GetUID());
			for (auto mode : cs.Modes())
				c->SetMode(setter, ModeManager::FindChannelModeByChar(mode), user->GetUID(), false);

			if (uc != NULL)
				uc->status = cs;
		}
	}

	void SendAkill(User *u, XLine *x) override
	{
		if (x->IsRegex() || x->HasNickOrReal())
		{
			if (!u)
			{
				/* No user (this akill was just added), and contains nick and/or realname. Find users that match and ban them */
				for (const auto &[_, user] : UserListByNick)
					if (x->manager->Check(user, x))
						this->SendAkill(user, x);
				return;
			}

			const XLine *old = x;

			if (old->manager->HasEntry("*@" + u->host))
				return;

			/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
			x = new XLine("*@" + u->host, old->by, old->expires, old->reason, old->id);
			old->manager->AddXLine(x);

			Log(Config->GetClient("OperServ"), "akill") << "AKILL: Added an akill for " << x->mask << " because " << u->GetMask() << "#" << u->realname << " matches " << old->mask;
		}

		/* ZLine if we can instead */
		if (x->GetUser() == "*")
		{
			cidr a(x->GetHost());
			if (a.valid())
			{
				IRCD->SendSZLine(u, x);
				return;
			}
		}

		// Calculate the time left before this would expire
		time_t timeleft = x->expires ? x->expires - Anope::CurTime : x->expires;
		Uplink::Send("AKILL", x->GetHost(), x->GetUser(), timeleft, x->by, Anope::CurTime, x->reason);
	}

	/*
	  Note: if the stamp is null 0, the below usage is correct of Bahamut
	*/
	void SendSVSKill(const MessageSource &source, User *user, const Anope::string &buf) override
	{
		Uplink::Send(source, "SVSKILL", user->nick, buf);
	}

	void SendBOB() override
	{
		Uplink::Send("BURST");
	}

	void SendEOB() override
	{
		Uplink::Send("BURST", 0);
	}

	void SendClientIntroduction(User *u) override
	{
		Uplink::Send("NICK", u->nick, 1, u->timestamp, "+" + u->GetModes(), u->GetIdent(), u->host, u->server->GetName(), 0, "0.0.0.0", u->realname);
	}

	/* SERVER */
	void SendServer(const Server *server) override
	{
		Uplink::Send("SERVER", server->GetName(), server->GetHops(), server->GetDescription());
	}

	void SendConnect() override
	{
		Uplink::Send("PASS", Config->Uplinks[Anope::CurrentUplink].password, "TS");
		Uplink::Send("CAPAB", "BURST", "NICKIPSTR", "SSJOIN", "UNCONNECT");
		SendServer(Me);
		/*
		 * SVINFO
		 *	   parv[0] = sender prefix
		 *	   parv[1] = TS_CURRENT for the server
		 *	   parv[2] = TS_MIN for the server
		 *	   parv[3] = server is standalone or connected to non-TS only
		 *	   parv[4] = server's idea of UTC time
		 */
		Uplink::Send("SVINFO", 5, 5, 0, Anope::CurTime);
		this->SendBOB();
	}

	void SendChannel(Channel *c) override
	{
		Uplink::Send("SJOIN", c->creation_time, c->name, "+" + c->GetModes(true, true), "");
	}

	void SendLogin(User *u, NickAlias *) override
	{
		IRCD->SendMode(Config->GetClient("NickServ"), u, "+d", u->signon);
	}

	void SendLogout(User *u) override
	{
		IRCD->SendMode(Config->GetClient("NickServ"), u, "+d", 1);
	}
};

struct IRCDMessageBurst final
	: IRCDMessage
{
	IRCDMessageBurst(Module *creator) : IRCDMessage(creator, "BURST", 0) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		/* If we found a server with the given source, that one just
		 * finished bursting. If there was no source, then our uplink
		 * server finished bursting. -GD
		 */
		Server *s = source.GetServer();
		if (!s)
			s = Me->GetLinks().front();
		if (s)
			s->Sync(true);
	}
};

struct IRCDMessageMode final
	: IRCDMessage
{
	IRCDMessageMode(Module *creator, const Anope::string &sname) : IRCDMessage(creator, sname, 2) { SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (params.size() > 2 && IRCD->IsChannelValid(params[0]))
		{
			Channel *c = Channel::Find(params[0]);
			auto ts = IRCD->ExtractTimestamp(params[1]);

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
				u->SetModesInternal(source, params[1]);
		}
	}
};

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
struct IRCDMessageNick final
	: IRCDMessage
{
	IRCDMessageNick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (params.size() == 10)
		{
			Server *s = Server::Find(params[6]);
			if (s == NULL)
			{
				Log(LOG_DEBUG) << "User " << params[0] << " introduced from nonexistent server " << params[6] << "?";
				return;
			}

			NickAlias *na = NULL;
			auto signon = IRCD->ExtractTimestamp(params[2]);
			auto stamp = IRCD->ExtractTimestamp(params[7]);
			if (signon && signon == stamp)
				na = NickAlias::Find(params[0]);

			User::OnIntroduce(params[0], params[4], params[5], "", params[8], s, params[9], signon, params[3], "", na ? *na->nc : NULL);
		}
		else
		{
			User *u = source.GetUser();

			if (u)
				u->ChangeNick(params[0]);
		}
	}
};

struct IRCDMessageServer final
	: IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		auto hops = Anope::Convert<unsigned>(params[1], 0);
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[2]);
	}
};

struct IRCDMessageSJoin final
	: IRCDMessage
{
	IRCDMessageSJoin(Module *creator) : IRCDMessage(creator, "SJOIN", 2) { SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		Anope::string modes;
		if (params.size() >= 4)
			for (unsigned i = 2; i < params.size(); ++i)
				modes += " " + params[i];
		if (!modes.empty())
			modes.erase(modes.begin());

		std::list<Message::Join::SJoinUser> users;

		/* For some reason, bahamut will send a SJOIN from the user joining a channel
		 * if the channel already existed
		 */
		if (source.GetUser())
		{
			Message::Join::SJoinUser sju;
			sju.second = source.GetUser();
			users.push_back(sju);
		}
		else
		{
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
					Log(LOG_DEBUG) << "SJOIN for nonexistent user " << buf << " on " << params[1];
					continue;
				}

				users.push_back(sju);
			}
		}

		auto ts = IRCD->ExtractTimestamp(params[0]);
		Message::Join::SJoin(source, params[1], ts, modes, users);
	}
};

struct IRCDMessageTopic final
	: IRCDMessage
{
	IRCDMessageTopic(Module *creator) : IRCDMessage(creator, "TOPIC", 4) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		Channel *c = Channel::Find(params[0]);
		if (c)
			c->ChangeTopicInternal(source.GetUser(), params[1], params[3], IRCD->ExtractTimestamp(params[2]));
	}
};

class ProtoBahamut final
	: public Module
{
	BahamutIRCdProto ircd_proto;

	/* Core message handlers */
	Message::Away message_away;
	Message::Capab message_capab;
	Message::Error message_error;
	Message::Invite message_invite;
	Message::Join message_join;
	Message::Kick message_kick;
	Message::Kill message_kill;
	Message::MOTD message_motd;
	Message::Notice message_notice;
	Message::Part message_part;
	Message::Ping message_ping;
	Message::Privmsg message_privmsg;
	Message::Quit message_quit;
	Message::SQuit message_squit;
	Message::Stats message_stats;
	Message::Time message_time;
	Message::Version message_version;
	Message::Whois message_whois;

	/* Our message handlers */
	IRCDMessageBurst message_burst;
	IRCDMessageMode message_mode, message_svsmode;
	IRCDMessageNick message_nick;
	IRCDMessageServer message_server;
	IRCDMessageSJoin message_sjoin;
	IRCDMessageTopic message_topic;

	static void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserModeOperOnly("SERV_ADMIN", 'A'));
		ModeManager::AddUserMode(new UserMode("REGPRIV", 'R'));
		ModeManager::AddUserMode(new UserModeOperOnly("ADMIN", 'a'));
		ModeManager::AddUserMode(new UserMode("INVIS", 'i'));
		ModeManager::AddUserMode(new UserModeOperOnly("OPER", 'o'));
		ModeManager::AddUserMode(new UserModeNoone("REGISTERED", 'r'));
		ModeManager::AddUserMode(new UserModeOperOnly("SNOMASK", 's'));
		ModeManager::AddUserMode(new UserModeOperOnly("WALLOPS", 'w'));
		ModeManager::AddUserMode(new UserMode("DEAF", 'd'));

		/* b/e/I */
		ModeManager::AddChannelMode(new ChannelModeList("BAN", 'b'));

		/* v/h/o/a/q */
		ModeManager::AddChannelMode(new ChannelModeStatus("VOICE", 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus("HALFOP", 'h', '%', 1));
		ModeManager::AddChannelMode(new ChannelModeStatus("OP", 'o', '@', 1));

		/* Add channel modes */
		ModeManager::AddChannelMode(new ChannelMode("BLOCKCOLOR", 'c'));
		ModeManager::AddChannelMode(new ChannelMode("INVITE", 'i'));
		ModeManager::AddChannelMode(new ChannelModeFlood('f', false));
		ModeManager::AddChannelMode(new ChannelModeKey('k'));
		ModeManager::AddChannelMode(new ChannelModeParam("LIMIT", 'l', true));
		ModeManager::AddChannelMode(new ChannelMode("MODERATED", 'm'));
		ModeManager::AddChannelMode(new ChannelMode("NOEXTERNAL", 'n'));
		ModeManager::AddChannelMode(new ChannelMode("PRIVATE", 'p'));
		ModeManager::AddChannelMode(new ChannelModeNoone("REGISTERED", 'r'));
		ModeManager::AddChannelMode(new ChannelMode("SECRET", 's'));
		ModeManager::AddChannelMode(new ChannelMode("TOPIC", 't'));
		ModeManager::AddChannelMode(new ChannelMode("REGMODERATED", 'M'));
		ModeManager::AddChannelMode(new ChannelModeOperOnly("OPERONLY", 'O'));
		ModeManager::AddChannelMode(new ChannelMode("REGISTEREDONLY", 'R'));
	}

public:
	ProtoBahamut(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR),
		ircd_proto(this),
		message_away(this), message_capab(this), message_error(this), message_invite(this),
		message_join(this), message_kick(this), message_kill(this), message_motd(this), message_notice(this),
		message_part(this), message_ping(this), message_privmsg(this), message_quit(this),
		message_squit(this), message_stats(this), message_time(this), message_version(this), message_whois(this),

		message_burst(this), message_mode(this, "MODE"), message_svsmode(this, "SVSMODE"),
		message_nick(this), message_server(this), message_sjoin(this), message_topic(this)
	{
		this->AddModes();
	}

	void OnUserNickChange(User *u, const Anope::string &) override
	{
		u->RemoveModeInternal(Me, ModeManager::FindUserModeByName("REGISTERED"));
		IRCD->SendLogout(u);
	}
};

MODULE_INIT(ProtoBahamut)
