/* ircd-hybrid-8 protocol module
 *
 * (C) 2003-2012 Anope Team
 * (C) 2012 by the Hybrid Development Team
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
  public:
	HybridProto(Module *creator) : IRCDProto(creator, "Hybrid 8.0.0")
	{
		DefaultPseudoclientModes = "+oi";
		CanSNLine = true;
		CanSQLine = true;
		CanSZLine = true;
		RequiresID = true;
		MaxModes = 4;
	}

	void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "NOTICE $$" << dest->GetName() << " :" << msg;
	}

	void SendGlobalPrivmsg(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "PRIVMSG $$" << dest->GetName() << " :" << msg;
	}

	void SendGlobopsInternal(const BotInfo *source, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "OPERWALL :" << buf;
	}

	void SendSQLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message(OperServ) << "RESV * " << x->mask << " :" << x->GetReason();
	}

	void SendSGLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(OperServ) << "UNXLINE * " << x->mask;
	}

	void SendSGLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message(OperServ) << "XLINE * " << x->mask << " 0 :" << x->GetReason();
	}

	void SendSZLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(OperServ) << "UNDLINE * " << x->GetHost();
	}

	void SendSZLine(User *, const XLine *x) anope_override
	{
		/* Calculate the time left before this would expire, capping it at 2 days */
		time_t timeleft = x->expires - Anope::CurTime;

		if (timeleft > 172800 || !x->expires)
			timeleft = 172800;

		UplinkSocket::Message(OperServ) << "DLINE * " << timeleft << " " << x->GetHost() << " :" << x->GetReason();
	}

	void SendAkillDel(const XLine *x) anope_override
	{
		if (x->IsRegex() || x->HasNickOrReal())
			return;

		UplinkSocket::Message(OperServ) << "UNKLINE * " << x->GetUser() << " " << x->GetHost();
	}

	void SendSQLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(OperServ) << "UNRESV * " << x->mask;
	}

	void SendJoin(const User *user, Channel *c, const ChannelStatus *status) anope_override
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

			Log(OperServ, "akill") << "AKILL: Added an akill for " << x->mask << " because " << u->GetMask() << "#"
					<< u->realname << " matches " << old->mask;
		}

		/* Calculate the time left before this would expire, capping it at 2 days */
		time_t timeleft = x->expires - Anope::CurTime;

		if (timeleft > 172800 || !x->expires)
			timeleft = 172800;

		UplinkSocket::Message(OperServ) << "KLINE * " << timeleft << " " << x->GetUser() << " " << x->GetHost() << " :" << x->GetReason();
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
		UplinkSocket::Message() << "PASS " << Config->Uplinks[Anope::CurrentUplink]->password << " TS 6 :" << Me->GetSID();

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

	void SendClientIntroduction(const User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();

		UplinkSocket::Message(Me) << "UID " << u->nick << " 1 " << u->timestamp << " " << modes << " "
					 << u->GetIdent() << " " << u->host << " 0 " << u->GetUID() << " 0 :" << u->realname;
	}

	void SendEOB() anope_override
	{
		UplinkSocket::Message(Me) << "EOB";
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf) anope_override
	{
		if (bi)
			UplinkSocket::Message(bi) << "SVSMODE " << u->GetUID() << " " << u->timestamp << " " << buf;
		else
			UplinkSocket::Message(Me) << "SVSMODE " << u->GetUID() << " " << u->timestamp << " " << buf;
	}

	void SendLogin(User *u) anope_override
	{
		IRCD->SendMode(NickServ, u, "+d %s", u->Account()->display.c_str());
	}

	void SendLogout(User *u) anope_override
	{
		IRCD->SendMode(NickServ, u, "+d 0");
	}

	void SendChannel(Channel *c) anope_override
	{
		Anope::string modes = c->GetModes(true, true);

		if (modes.empty())
			modes = "+";

		UplinkSocket::Message() << "SJOIN " << c->creation_time << " " << c->name << " " << modes << " :";
	}

	void SendTopic(BotInfo *bi, Channel *c) anope_override
	{
		bool needjoin = c->FindUser(bi) == NULL;

		if (needjoin)
		{
			ChannelStatus status;

			status.SetFlag(CMODE_OP);
			bi->Join(c, &status);
		}

		IRCDProto::SendTopic(bi, c);

		if (needjoin)
			bi->Part(c);
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

		if (c)
		{
			ChannelMode *ban = ModeManager::FindChannelModeByName(CMODE_BAN),
					*except = ModeManager::FindChannelModeByName(CMODE_EXCEPT),
					*invex = ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE);

			spacesepstream bans(params[3]);
			Anope::string token;
			while (bans.GetToken(token))
			{
				if (ban && params[2].equals_cs("b"))
					c->SetModeInternal(source, ban, token);
				else if (except && params[2].equals_cs("e"))
					c->SetModeInternal(source, except, token);
				else if (invex && params[2].equals_cs("I"))
					c->SetModeInternal(source, invex, token);
			}
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

		IRCD->SendPing(Config->ServerName, params[0]);
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

		IRCD->SendPing(Config->ServerName, params[0]);
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
				ChannelMode *cm = ModeManager::FindChannelModeByChar(ch);

				if (!cm)
				{
					Log() << "Received unknown mode prefix " << ch << " in SJOIN string";
					continue;
				}

				sju.first.SetFlag(cm->name);
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

		u->SetModesInternal("%s", params[2].c_str());
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
	ServiceReference<NickServService> NSService;

	IRCDMessageUID(Module *creator) : IRCDMessage(creator, "UID", 10), NSService("NickServService", "NickServ") { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*          0     1 2          3   4      5             6        7         8           9                   */
	/* :0MC UID Steve 1 1350157102 +oi ~steve resolved.host 10.0.0.1 0MCAAAAAB 1350157108 :Mining all the time */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Anope::string ip = params[6];

		if (ip == "0") /* Can be 0 for spoofed clients */
			ip.clear();

		/* Source is always the server */
		User *user = new User(params[0], params[4], params[5], "",
					ip, source.GetServer(),
					params[9], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0,
					params[3], params[7]);

		if (NSService && params[8] != "0")
		{
			NickAlias *na = NickAlias::Find(params[8]);
			if (na != NULL)
				NSService->Login(user, na);
		}
	}
};

class ProtoHybrid : public Module
{
	HybridProto ircd_proto;

	/* Core message handlers */
	Message::Away message_away;
	Message::Capab message_capab;
	Message::Error message_error;
	Message::Kick message_kick;
	Message::Kill message_kill;
	Message::Mode message_mode;
	Message::MOTD message_motd;
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

	void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserMode(UMODE_ADMIN, 'a'));
		ModeManager::AddUserMode(new UserMode(UMODE_INVIS, 'i'));
		ModeManager::AddUserMode(new UserMode(UMODE_OPER, 'o'));
		ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, 'r'));
		ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, 's'));
		ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, 'w'));
		ModeManager::AddUserMode(new UserMode(UMODE_HIDEOPER, 'H'));
		ModeManager::AddUserMode(new UserMode(UMODE_REGPRIV, 'R'));

		/* b/e/I */
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_BAN, 'b'));
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_EXCEPT, 'e'));
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_INVITEOVERRIDE, 'I'));

		/* v/h/o/a/q */
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_HALFOP, 'h', '%', 1));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, 'o', '@', 2));

		/* l/k */
		ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, 'l'));
		ModeManager::AddChannelMode(new ChannelModeKey('k'));

		/* Add channel modes */
		ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, 'i'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, 'm'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, 'n'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, 'p'));
		ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, 's'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, 't'));
		ModeManager::AddChannelMode(new ChannelModeOper('O'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_REGISTEREDONLY, 'R'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_SSL, 'S'));
	}

public:
	ProtoHybrid(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL),
		ircd_proto(this),
		message_away(this), message_capab(this), message_error(this), message_kick(this), message_kill(this),
		message_mode(this), message_motd(this), message_part(this), message_ping(this), message_privmsg(this),
		message_quit(this), message_squit(this), message_stats(this), message_time(this), message_topic(this),
		message_version(this), message_whois(this),

		message_bmask(this), message_eob(this), message_join(this),
		message_nick(this), message_pass(this), message_pong(this), message_server(this), message_sid(this),
		message_sjoin(this), message_svsmode(this), message_tburst(this), message_tmode(this), message_uid(this)
	{
		this->SetAuthor("Anope");
		this->AddModes();

		ModuleManager::Attach(I_OnUserNickChange, this);

		if (Config->Numeric.empty())
		{
			Anope::string numeric = Servers::TS6_SID_Retrieve();
			Me->SetSID(numeric);
			Config->Numeric = numeric;
		}

		for (botinfo_map::iterator it = BotListByNick->begin(), it_end = BotListByNick->end(); it != it_end; ++it)
			it->second->GenerateUID();
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
	}
};

MODULE_INIT(ProtoHybrid)
