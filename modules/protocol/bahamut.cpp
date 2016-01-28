/* Bahamut functions
 *
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class ChannelModeFlood : public ChannelModeParam
{
 public:
	ChannelModeFlood(char modeChar, bool minusNoArg) : ChannelModeParam("FLOOD", modeChar, minusNoArg) { }

	bool IsValid(Anope::string &value) const anope_override
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

class BahamutIRCdProto : public IRCDProto
{
 public:
	BahamutIRCdProto(Module *creator) : IRCDProto(creator, "Bahamut 1.8.x")
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

	void SendModeInternal(const MessageSource &source, const Channel *dest, const Anope::string &buf) anope_override
	{
		if (Servers::Capab.count("TSMODE") > 0)
		{
			UplinkSocket::Message(source) << "MODE " << dest->name << " " << dest->creation_time << " " << buf;
		}
		else
			IRCDProto::SendModeInternal(source, dest, buf);
	}

	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "SVSMODE " << u->nick << " " << u->timestamp << " " << buf;
	}

	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "NOTICE $" << dest->GetName() << " :" << msg;
	}

	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "PRIVMSG $" << dest->GetName() << " :" << msg;
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick, time_t time) anope_override
	{
		UplinkSocket::Message(Me) << "SVSHOLD " << nick << " " << time << " :Being held for registered user";
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message(Me) << "SVSHOLD " << nick << " 0";
	}

	/* SQLINE */
	void SendSQLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message() << "SQLINE " << x->mask << " :" << x->GetReason();
	}

	/* UNSLINE */
	void SendSGLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message() << "UNSGLINE 0 :" << x->mask;
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x) anope_override
	{
		/* this will likely fail so its only here for legacy */
		UplinkSocket::Message() << "UNSZLINE 0 " << x->GetHost();
		/* this is how we are supposed to deal with it */
		UplinkSocket::Message() << "RAKILL " << x->GetHost() << " *";
	}

	/* SZLINE */
	void SendSZLine(User *, const XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->expires - Anope::CurTime;
		if (timeleft > 172800 || !x->expires)
			timeleft = 172800;
		/* this will likely fail so its only here for legacy */
		UplinkSocket::Message() << "SZLINE " << x->GetHost() << " :" << x->GetReason();
		/* this is how we are supposed to deal with it */
		UplinkSocket::Message() << "AKILL " << x->GetHost() << " * " << timeleft << " " << x->by << " " << Anope::CurTime << " :" << x->GetReason();
	}

	/* SVSNOOP */
	void SendSVSNOOP(const Server *server, bool set) anope_override
	{
		UplinkSocket::Message() << "SVSNOOP " << server->GetName() << " " << (set ? "+" : "-");
	}

	/* SGLINE */
	void SendSGLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message() << "SGLINE " << x->mask.length() << " :" << x->mask << ":" << x->GetReason();
	}

	/* RAKILL */
	void SendAkillDel(const XLine *x) anope_override
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

		UplinkSocket::Message() << "RAKILL " << x->GetHost() << " " << x->GetUser();
	}

	/* TOPIC */
	void SendTopic(const MessageSource &source, Channel *c) anope_override
	{
		UplinkSocket::Message(source) << "TOPIC " << c->name << " " << c->topic_setter << " " << c->topic_ts << " :" << c->topic;
	}

	/* UNSQLINE */
	void SendSQLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message() << "UNSQLINE " << x->mask;
	}

	/* JOIN - SJOIN */
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		UplinkSocket::Message(user) << "SJOIN " << c->creation_time << " " << c->name;
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
			for (size_t i = 0; i < cs.Modes().length(); ++i)
				c->SetMode(setter, ModeManager::FindChannelModeByChar(cs.Modes()[i]), user->GetUID(), false);

			if (uc != NULL)
				uc->status = cs;
		}
	}

	void SendAkill(User *u, XLine *x) anope_override
	{
		if (x->IsRegex() || x->HasNickOrReal())
		{
			if (!u)
			{
				/* No user (this akill was just added), and contains nick and/or realname. Find users that match and ban them */
				for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
					if (x->manager->Check(it->second, x))
						this->SendAkill(it->second, x);
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

		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->expires - Anope::CurTime;
		if (timeleft > 172800)
			timeleft = 172800;
		UplinkSocket::Message() << "AKILL " << x->GetHost() << " " << x->GetUser() << " " << timeleft << " " << x->by << " " << Anope::CurTime << " :" << x->GetReason();
	}

	/*
	  Note: if the stamp is null 0, the below usage is correct of Bahamut
	*/
	void SendSVSKillInternal(const MessageSource &source, User *user, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "SVSKILL " << user->nick << " :" << buf;
	}

	void SendBOB() anope_override
	{
		UplinkSocket::Message() << "BURST";
	}

	void SendEOB() anope_override
	{
		UplinkSocket::Message() << "BURST 0";
	}

	void SendClientIntroduction(User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message() << "NICK " << u->nick << " 1 " << u->timestamp << " " << modes << " " << u->GetIdent() << " " << u->host << " " << u->server->GetName() << " 0 0 :" << u->realname;
	}

	/* SERVER */
	void SendServer(const Server *server) anope_override
	{
		UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :" << server->GetDescription();
	}

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "PASS " << Config->Uplinks[Anope::CurrentUplink].password << " :TS";
		UplinkSocket::Message() << "CAPAB SSJOIN NOQUIT BURST UNCONNECT NICKIP TSMODE TS3";
		SendServer(Me);
		/*
		 * SVINFO
		 *	   parv[0] = sender prefix
		 *	   parv[1] = TS_CURRENT for the server
		 *	   parv[2] = TS_MIN for the server
		 *	   parv[3] = server is standalone or connected to non-TS only
		 *	   parv[4] = server's idea of UTC time
		 */
		UplinkSocket::Message() << "SVINFO 3 1 0 :" << Anope::CurTime;
		this->SendBOB();
	}

	void SendChannel(Channel *c) anope_override
	{
		Anope::string modes = c->GetModes(true, true);
		if (modes.empty())
			modes = "+";
		UplinkSocket::Message() << "SJOIN " << c->creation_time << " " << c->name << " " << modes << " :";
	}

	void SendLogin(User *u, NickAlias *) anope_override
	{
		IRCD->SendMode(Config->GetClient("NickServ"), u, "+d %d", u->signon);
	}

	void SendLogout(User *u) anope_override
	{
		IRCD->SendMode(Config->GetClient("NickServ"), u, "+d 1");
	}
};

struct IRCDMessageBurst : IRCDMessage
{
	IRCDMessageBurst(Module *creator) : IRCDMessage(creator, "BURST", 0) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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

struct IRCDMessageMode : IRCDMessage
{
	IRCDMessageMode(Module *creator, const Anope::string &sname) : IRCDMessage(creator, sname, 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
struct IRCDMessageNick : IRCDMessage
{
	IRCDMessageNick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() == 10)
		{
			Server *s = Server::Find(params[6]);
			if (s == NULL)
			{
				Log(LOG_DEBUG) << "User " << params[0] << " introduced from non-existent server " << params[6] << "?";
				return;
			}

			NickAlias *na = NULL;
			time_t signon = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0,
				stamp = params[7].is_pos_number_only() ? convertTo<time_t>(params[7]) : 0;
			if (signon && signon == stamp)
				na = NickAlias::Find(params[0]);

			User::OnIntroduce(params[0], params[4], params[5], "", params[8], s, params[9], signon, params[3], "", na ? *na->nc : NULL);
		}
		else
			source.GetUser()->ChangeNick(params[0]);
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		unsigned int hops = Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[2]);
	}
};

struct IRCDMessageSJoin : IRCDMessage
{
	IRCDMessageSJoin(Module *creator) : IRCDMessage(creator, "SJOIN", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
					Log(LOG_DEBUG) << "SJOIN for non-existent user " << buf << " on " << params[1];
					continue;
				}

				users.push_back(sju);
			}
		}

		time_t ts = Anope::string(params[0]).is_pos_number_only() ? convertTo<time_t>(params[0]) : Anope::CurTime;
		Message::Join::SJoin(source, params[1], ts, modes, users);
	}
};

struct IRCDMessageTopic : IRCDMessage
{
	IRCDMessageTopic(Module *creator) : IRCDMessage(creator, "TOPIC", 4) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = Channel::Find(params[0]);
		if (c)
			c->ChangeTopicInternal(source.GetUser(), params[1], params[3], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime);
	}
};

class ProtoBahamut : public Module
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

	void AddModes()
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

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveModeInternal(Me, ModeManager::FindUserModeByName("REGISTERED"));
		IRCD->SendLogout(u);
	}
};

MODULE_INIT(ProtoBahamut)
