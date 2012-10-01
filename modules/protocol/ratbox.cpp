/* Ratbox IRCD functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

static Anope::string UplinkSID;

class RatboxProto : public IRCDProto
{
 public:
	RatboxProto() : IRCDProto("Ratbox 3.0+")
	{
		DefaultPseudoclientModes = "+oiS";
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
		UplinkSocket::Message(Me) << "RESV * " << x->Mask << " :" << x->GetReason();
	}

	void SendSGLineDel(const XLine *x) anope_override
	{
		const BotInfo *bi = findbot(Config->OperServ);
		UplinkSocket::Message(bi) << "UNXLINE * " << x->Mask;
	}

	void SendSGLine(User *, const XLine *x) anope_override
	{
		const BotInfo *bi = findbot(Config->OperServ);
		UplinkSocket::Message(bi) << "XLINE * " << x->Mask << " 0 :" << x->GetReason();
	}

	void SendAkillDel(const XLine *x) anope_override
	{
		if (x->IsRegex() || x->HasNickOrReal())
			return;

		const BotInfo *bi = findbot(Config->OperServ);
		UplinkSocket::Message(bi) << "UNKLINE * " << x->GetUser() << " " << x->GetHost();
	}

	void SendSQLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(Me) << "UNRESV * " << x->Mask;
	}

	void SendJoin(const User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		/* Note that we must send our modes with the SJOIN and
		 * can not add them to the mode stacker because ratbox
		 * does not allow *any* client to op itself
		 */
		UplinkSocket::Message() << "SJOIN " << c->creation_time << " " << c->name << " +" << c->GetModes(true, true) << " :" << (status != NULL ? status->BuildModePrefixList() : "") << user->GetUID();
		/* And update our internal status for this user since this is not going through our mode handling system */
		if (status != NULL)
		{
			UserContainer *uc = c->FindUser(user);
			if (uc != NULL)
				*uc->Status = *status;
		}
	}

	void SendAkill(User *u, XLine *x) anope_override
	{
		const BotInfo *bi = findbot(Config->OperServ);

		if (x->IsRegex() || x->HasNickOrReal())
		{
			if (!u)
			{
				/* No user (this akill was just added), and contains nick and/or realname. Find users that match and ban them */
				for (Anope::insensitive_map<User *>::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
					if (x->manager->Check(it->second, x))
						this->SendAkill(it->second, x);
				return;
			}

			const XLine *old = x;

			if (old->manager->HasEntry("*@" + u->host))
				return;

			/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
			XLine *xline = new XLine("*@" + u->host, old->By, old->Expires, old->Reason, old->UID);
			old->manager->AddXLine(xline);
			x = xline;

			Log(bi, "akill") << "AKILL: Added an akill for " << x->Mask << " because " << u->GetMask() << "#" << u->realname << " matches " << old->Mask;
		}

		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		UplinkSocket::Message(bi) << "KLINE * " << timeleft << " " << x->GetUser() << " " << x->GetHost() << " :" << x->GetReason();
	}

	/* SERVER name hop descript */
	void SendServer(const Server *server) anope_override
	{
		UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :" << server->GetDescription();
	}

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "PASS " << Config->Uplinks[CurrentUplink]->password << " TS 6 :" << Me->GetSID();
		/*
		  QS     - Can handle quit storm removal
		  EX     - Can do channel +e exemptions
		  CHW    - Can do channel wall @#
		  IE     - Can do invite exceptions
		  GLN    - Can do GLINE message
		  KNOCK  - supports KNOCK
		  TB     - supports topic burst
		  ENCAP  - supports ENCAP
		*/
		UplinkSocket::Message() << "CAPAB :QS EX CHW IE GLN TB ENCAP";
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
		UplinkSocket::Message() << "SVINFO 6 3 0 :" << Anope::CurTime;
	}

	void SendClientIntroduction(const User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "UID " << u->nick << " 1 " << u->timestamp << " " << modes << " " << u->GetIdent() << " " << u->host << " 0 " << u->GetUID() << " :" << u->realname;
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(bi) << "SVSMODE " << u->nick << " " << buf;
	}

	void SendLogin(User *u) anope_override
	{
		if (!u->Account())
			return;

		UplinkSocket::Message(Me) << "ENCAP * SU " << u->GetUID() << " " << u->Account()->display;
	}

	void SendLogout(User *u) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP * SU " << u->GetUID();
	}

	void SendChannel(Channel *c) anope_override
	{
		Anope::string modes = c->GetModes(true, true);
		if (modes.empty())
			modes = "+";
		UplinkSocket::Message() << "SJOIN " << c->creation_time << " " << c->name << " " << modes << " :";
	}

	bool IsNickValid(const Anope::string &nick) anope_override
	{
		/* TS6 Save extension -Certus */
		if (isdigit(nick[0]))
			return false;

		return true;
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
	IRCDMessageBMask() : IRCDMessage("BMASK", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* :42X BMASK 1106409026 #ircops b :*!*@*.aol.com */
		/*            0          1       2  3             */
		Channel *c = findchan(params[1]);

		if (c)
		{
			ChannelMode *ban = ModeManager::FindChannelModeByName(CMODE_BAN),
				*except = ModeManager::FindChannelModeByName(CMODE_EXCEPT),
				*invex = ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE);

			Anope::string bans = params[3];
			int count = myNumToken(bans, ' '), i;
			for (i = 0; i < count; ++i)
			{
				Anope::string b = myStrGetToken(bans, ' ', i);
				if (ban && params[2].equals_cs("b"))
					c->SetModeInternal(source, ban, b);
				else if (except && params[2].equals_cs("e"))
					c->SetModeInternal(source, except, b);
				if (invex && params[2].equals_cs("I"))
					c->SetModeInternal(source, invex, b);
			}
		}

		return true;
	}
};

struct IRCDMessageEncap : IRCDMessage
{
	IRCDMessageEncap() : IRCDMessage("ENCAP", 3) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	// Debug: Received: :00BAAAAAB ENCAP * LOGIN Adam
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params[1] == "LOGIN")
		{
			User *u = source.GetUser();

			NickCore *nc = findcore(params[2]);
			if (!nc)
				return true;
			u->Login(nc);

			const NickAlias *user_na = findnick(u->nick);
			if (!Config->NoNicknameOwnership && user_na && user_na->nc == nc && user_na->nc->HasFlag(NI_UNCONFIRMED) == false)
				u->SetMode(findbot(Config->NickServ), UMODE_REGISTERED);
		}

		return true;
	}
};

struct IRCDMessageJoin : CoreIRCDMessageJoin
{
	IRCDMessageJoin() : CoreIRCDMessageJoin("JOIN") { }

 	/*
	 * params[0] = ts
	 * params[1] = channel
	 * params[2] = modes
	 */
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() < 2)
			return true;

		std::vector<Anope::string> p = params;
		p.erase(p.begin());

		return CoreIRCDMessageJoin::Run(source, p);
	}
};

struct IRCDMessageMode : IRCDMessage
{
	IRCDMessageMode() : IRCDMessage("MODE", 3) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (ircdproto->IsChannelValid(params[0]))
		{
			// 0 = channel, 1 = ts, 2 = modes
			Channel *c = findchan(params[0]);
			time_t ts = Anope::CurTime;
			try
			{
				ts = convertTo<time_t>(params[1]);
			}
			catch (const ConvertException &) { }

			if (c)
				c->SetModesInternal(source, params[2], ts);
		}
		else
		{
			User *u = finduser(params[0]);
			if (u)
				u->SetModesInternal("%s", params[1].c_str());
		}

		return true;
	}
};

struct IRCDMessageNick : IRCDMessage
{
	IRCDMessageNick() : IRCDMessage("NICK", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->ChangeNick(params[0]);
		return true;
	}
};

struct IRCDMessagePass : IRCDMessage
{
	IRCDMessagePass() : IRCDMessage("PASS", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		UplinkSID = params[3];
		return true;
	}
};

struct IRCDMessagePong : IRCDMessage
{
	IRCDMessagePong() : IRCDMessage("PONG", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetServer()->Sync(false);
		return true;
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer() : IRCDMessage("SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	// SERVER hades.arpa 1 :ircd-ratbox test server
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		// Servers other then our immediate uplink are introduced via SID
		if (params[1] != "1")
			return true;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
		ircdproto->SendPing(Config->ServerName, params[0]);
		return true;
	}
};

struct IRCDMessageSID : IRCDMessage
{
	IRCDMessageSID() : IRCDMessage("SID", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* :42X SID trystan.nomadirc.net 2 43X :ircd-ratbox test server */
		unsigned int hops = params[1].is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[3], params[2]);
		ircdproto->SendPing(Config->ServerName, params[0]);
		return true;
	}
};

struct IRCDMessageSjoin : IRCDMessage
{
	IRCDMessageSjoin() : IRCDMessage("SJOIN", 2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = findchan(params[1]);
		time_t ts = Anope::string(params[0]).is_pos_number_only() ? convertTo<time_t>(params[0]) : 0;
		bool keep_their_modes = true;

		if (!c)
		{
			c = new Channel(params[1], ts);
			c->SetFlag(CH_SYNCING);
		}
		/* Our creation time is newer than what the server gave us */
		else if (c->creation_time > ts)
		{
			c->creation_time = ts;
			c->Reset();
		}
		/* Their TS is newer than ours, our modes > theirs, unset their modes if need be */
		else if (ts > c->creation_time)
			keep_their_modes = false;

		/* If we need to keep their modes, and this SJOIN string contains modes */
		if (keep_their_modes && params.size() >= 3)
		{
			Anope::string modes;
			for (unsigned i = 2; i < params.size() - 1; ++i)
				modes += " " + params[i];
			if (!modes.empty())
				modes.erase(modes.begin());
			/* Set the modes internally */
			c->SetModesInternal(source, modes);
		}

		spacesepstream sep(params[params.size() - 1]);
		Anope::string buf;
		while (sep.GetToken(buf))
		{
			std::list<ChannelMode *> Status;
			char ch;

			/* Get prefixes from the nick */
			while ((ch = ModeManager::GetStatusChar(buf[0])))
			{
				buf.erase(buf.begin());
				ChannelMode *cm = ModeManager::FindChannelModeByChar(ch);
				if (!cm)
				{
					Log() << "Received unknown mode prefix " << ch << " in SJOIN string";
					continue;
				}

				if (keep_their_modes)
					Status.push_back(cm);
			}
	
			User *u = finduser(buf);
			if (!u)
			{
				Log(LOG_DEBUG) << "SJOIN for nonexistant user " << buf << " on " << c->name;
				continue;
			}

			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnPreJoinChannel, OnPreJoinChannel(u, c));

			/* Add the user to the channel */
			c->JoinUser(u);

			/* Update their status internally on the channel
			 * This will enforce secureops etc on the user
			 */
			for (std::list<ChannelMode *>::iterator it = Status.begin(), it_end = Status.end(); it != it_end; ++it)
				c->SetModeInternal(source, *it, buf);

			/* Now set whatever modes this user is allowed to have on the channel */
			chan_set_correct_modes(u, c, 1, true);

			/* Check to see if modules want the user to join, if they do
			 * check to see if they are allowed to join (CheckKick will kick/ban them)
			 * Don't trigger OnJoinChannel event then as the user will be destroyed
			 */
			if (MOD_RESULT != EVENT_STOP && c->ci && c->ci->CheckKick(u))
				continue;

			FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(u, c));
		}

		/* Channel is done syncing */
		if (c->HasFlag(CH_SYNCING))
		{
			/* Unset the syncing flag */
			c->UnsetFlag(CH_SYNCING);
			c->Sync();
		}

		return true;
	}
};

struct IRCDMessageTBurst : IRCDMessage
{
	IRCDMessageTBurst() : IRCDMessage("TBURST", 4) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Anope::string setter = myStrGetToken(params[2], '!', 0);
		time_t topic_time = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime;
		Channel *c = findchan(params[0]);

		if (!c)
			return true;

		c->ChangeTopicInternal(setter, params[3], topic_time);

		return true;
	}
};

struct IRCDMessageTMode : IRCDMessage
{
	IRCDMessageTMode() : IRCDMessage("TMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		time_t ts = Anope::CurTime;
		try
		{
			ts = convertTo<time_t>(params[0]);
		}
		catch (const ConvertException &) { }
		Channel *c = findchan(params[1]);
		Anope::string modes = params[2];
		for (unsigned i = 3; i < params.size(); ++i)
			modes += " " + params[i];

		if (c)
			c->SetModesInternal(source, modes, ts);

		return true;
	}
};

struct IRCDMessageUID : IRCDMessage
{
	IRCDMessageUID() : IRCDMessage("UID", 9) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	// :42X UID Adam 1 1348535644 +aow Adam 192.168.0.5 192.168.0.5 42XAAAAAB :Adam
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* Source is always the server */
		User *user = new User(params[0], params[4], params[5], "", params[6], source.GetServer(), params[8], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0, params[3], params[7]);
		if (user && user->server->IsSynced() && nickserv)
			nickserv->Validate(user);

		return true;
	}
};

class ProtoRatbox : public Module
{
	RatboxProto ircd_proto;

	/* Core message handlers */
	CoreIRCDMessageAway core_message_away;
	CoreIRCDMessageCapab core_message_capab;
	CoreIRCDMessageError core_message_error;
	CoreIRCDMessageKill core_message_kill;
	CoreIRCDMessageMOTD core_message_motd;
	CoreIRCDMessagePart core_message_part;
	CoreIRCDMessagePing core_message_ping;
	CoreIRCDMessagePrivmsg core_message_privmsg;
	CoreIRCDMessageQuit core_message_quit;
	CoreIRCDMessageSQuit core_message_squit;
	CoreIRCDMessageStats core_message_stats;
	CoreIRCDMessageTime core_message_time;
	CoreIRCDMessageTopic core_message_topic;
	CoreIRCDMessageVersion core_message_version;

	/* Our message handlers */
	IRCDMessageBMask message_bmask;
	IRCDMessageEncap message_encap;
	IRCDMessageJoin message_join;
	IRCDMessageMode message_mode;
	IRCDMessageNick message_nick;
	IRCDMessagePass message_pass;
	IRCDMessagePong message_pong;
	IRCDMessageServer message_server;
	IRCDMessageSID message_sid;
	IRCDMessageSjoin message_sjoin;
	IRCDMessageTBurst message_tburst;
	IRCDMessageTMode message_tmode;
	IRCDMessageUID message_uid;

	void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserMode(UMODE_ADMIN, 'a'));
		ModeManager::AddUserMode(new UserMode(UMODE_INVIS, 'i'));
		ModeManager::AddUserMode(new UserMode(UMODE_OPER, 'o'));
		ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, 's'));
		ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, 'w'));

		/* b/e/I */
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_BAN, 'b'));
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_EXCEPT, 'e'));
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_INVITEOVERRIDE, 'I'));

		/* v/h/o/a/q */
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, 'o', '@', 1));

		/* Add channel modes */
		ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, 'i'));
		ModeManager::AddChannelMode(new ChannelModeKey('k'));
		ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, 'l'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, 'm'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, 'n'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, 'p'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, 's'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, 't'));
	}

 public:
	ProtoRatbox(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL)
	{
		this->SetAuthor("Anope");

		this->AddModes();

		Implementation i[] = { I_OnServerSync };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		if (Config->Numeric.empty())
		{
			Anope::string numeric = ts6_sid_retrieve();
			Me->SetSID(numeric);
			Config->Numeric = numeric;
		}

		for (botinfo_map::iterator it = BotListByNick->begin(), it_end = BotListByNick->end(); it != it_end; ++it)
			it->second->GenerateUID();
	}

	void OnServerSync(Server *s) anope_override
	{
		if (nickserv)
			for (Anope::insensitive_map<User *>::iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			{
				User *u = it->second;
				if (u->server == s && !u->IsIdentified())
					nickserv->Validate(u);
			}
	}
};

MODULE_INIT(ProtoRatbox)
