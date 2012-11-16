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
	HybridProto() : IRCDProto("Hybrid 8.0.0")
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

	void SendSQLine(User *, const XLine *x) anope_override
	{
		const BotInfo *bi = findbot(Config->OperServ);

		UplinkSocket::Message(bi) << "RESV * " << x->Mask << " :" << x->GetReason();
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

	void SendSZLineDel(const XLine *x) anope_override
	{
		const BotInfo *bi = findbot(Config->OperServ);

		UplinkSocket::Message(bi) << "UNDLINE * " << x->GetHost();
	}

	void SendSZLine(User *, const XLine *x) anope_override
	{
		const BotInfo *bi = findbot(Config->OperServ);
		/* Calculate the time left before this would expire, capping it at 2 days */
		time_t timeleft = x->Expires - Anope::CurTime;

		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;

		UplinkSocket::Message(bi) << "DLINE * " << timeleft << " " << x->GetHost() << " :" << x->GetReason();
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
		const BotInfo *bi = findbot(Config->OperServ);

		UplinkSocket::Message(bi) << "UNRESV * " << x->Mask;
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
			XLine *xline = new XLine("*@" + u->host, old->By, old->Expires, old->Reason, old->UID);

			old->manager->AddXLine(xline);
			x = xline;

			Log(bi, "akill") << "AKILL: Added an akill for " << x->Mask << " because " << u->GetMask() << "#"
					<< u->realname << " matches " << old->Mask;
		}

		/* Calculate the time left before this would expire, capping it at 2 days */
		time_t timeleft = x->Expires - Anope::CurTime;

		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;

		UplinkSocket::Message(bi) << "KLINE * " << timeleft << " " << x->GetUser() << " " << x->GetHost() << " :" << x->GetReason();
	}

	void SendServer(const Server *server) anope_override
	{
		UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :" << server->GetDescription();
	}

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "PASS " << Config->Uplinks[CurrentUplink]->password << " TS 6 :" << Me->GetSID();

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
		const BotInfo *ns = findbot(Config->NickServ);

		ircdproto->SendMode(ns, u, "+d %s", u->Account()->display.c_str());
	}

	void SendLogout(User *u) anope_override
	{
		const BotInfo *ns = findbot(Config->NickServ);

		ircdproto->SendMode(ns, u, "+d 0");
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
	IRCDMessageBMask() : IRCDMessage("BMASK", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*            0          1        2  3              */
	/* :0MC BMASK 1350157102 #channel b :*!*@*.test.com */
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
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
				else if (invex && params[2].equals_cs("I"))
					c->SetModeInternal(source, invex, b);
			}
		}

		return true;
	}
};

struct IRCDMessageJoin : CoreIRCDMessageJoin
{
	IRCDMessageJoin() : CoreIRCDMessageJoin("JOIN") { }

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
	IRCDMessageMode(const Anope::string &n) : IRCDMessage(n, 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() > 2 && ircdproto->IsChannelValid(params[0]))
		{
			Channel *c = findchan(params[0]);
			time_t ts = 0;

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
	IRCDMessageNick() : IRCDMessage("NICK", 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	/*                 0       1          */
	/* :0MCAAAAAB NICK newnick 1350157102 */
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->ChangeNick(params[0], convertTo<time_t>(params[1]));
		return true;
	}
};

struct IRCDMessagePass : IRCDMessage
{
	IRCDMessagePass() : IRCDMessage("PASS", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*      0        1  2 3   */
	/* PASS password TS 6 0MC */
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

	/*        0          1  2                       */
	/* SERVER hades.arpa 1 :ircd-hybrid test server */
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* Servers other than our immediate uplink are introduced via SID */
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

	/*          0          1 2    3                       */
	/* :0MC SID hades.arpa 2 4XY :ircd-hybrid test server */
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
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

			/*
			 * Update their status internally on the channel
			 * This will enforce secureops etc on the user
			 */
			for (std::list<ChannelMode *>::iterator it = Status.begin(), it_end = Status.end(); it != it_end; ++it)
				c->SetModeInternal(source, *it, buf);

			/* Now set whatever modes this user is allowed to have on the channel */
			chan_set_correct_modes(u, c, 1, true);

			/*
			 * Check to see if modules want the user to join, if they do
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
	IRCDMessageTBurst() : IRCDMessage("TBURST", 5) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Anope::string setter = myStrGetToken(params[3], '!', 0);
		time_t topic_time = Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime;
		Channel *c = findchan(params[1]);

		if (c)
			c->ChangeTopicInternal(setter, params[4], topic_time);

		return true;
	}
};

struct IRCDMessageTMode : IRCDMessage
{
	IRCDMessageTMode() : IRCDMessage("TMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		time_t ts = 0;

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
	IRCDMessageUID() : IRCDMessage("UID", 10) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*          0     1 2          3   4      5             6        7         8           9                   */
	/* :0MC UID Steve 1 1350157102 +oi ~steve resolved.host 10.0.0.1 0MCAAAAAB 1350157108 :Mining all the time */
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Anope::string ip = params[6];

		if (ip == "0") /* Can be 0 for spoofed clients */
			ip.clear();

		/* Source is always the server */
		User *user = new User(params[0], params[4], params[5], "",
					ip, source.GetServer(),
					params[9], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0,
					params[3], params[7]);

		if (user && nickserv)
		{
			const NickAlias *na = NULL;

			if (params[8] != "0")
				na = findnick(params[8]);

			if (na)
			{
				user->Login(na->nc);

				if (!Config->NoNicknameOwnership && na->nc->HasFlag(NI_UNCONFIRMED) == false)
					user->SetMode(findbot(Config->NickServ), UMODE_REGISTERED);
			}
			else
				nickserv->Validate(user);
		}

		return true;
	}
};

class ProtoHybrid : public Module
{
	HybridProto ircd_proto;

	/* Core message handlers */
	CoreIRCDMessageAway core_message_away;
	CoreIRCDMessageCapab core_message_capab;
	CoreIRCDMessageError core_message_error;
	CoreIRCDMessageKick core_message_kick;
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
	CoreIRCDMessageWhois core_message_whois;

	/* Our message handlers */
	IRCDMessageBMask message_bmask;
	IRCDMessageJoin message_join;
	IRCDMessageMode message_mode, message_svsmode;
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
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, 'o', '@', 1));

		/* Add channel modes */
		ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, 'i'));
		ModeManager::AddChannelMode(new ChannelModeKey('k'));
		ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, 'l'));
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
	ProtoHybrid(const Anope::string &modname, const Anope::string &creator) :
		Module(modname, creator, PROTOCOL), message_mode("MODE"), message_svsmode("SVSMODE")
	{
		this->SetAuthor("Anope");
		this->AddModes();

		ModuleManager::Attach(I_OnUserNickChange, this);

		if (Config->Numeric.empty())
		{
			Anope::string numeric = ts6_sid_retrieve();
			Me->SetSID(numeric);
			Config->Numeric = numeric;
		}

		for (botinfo_map::iterator it = BotListByNick->begin(), it_end = BotListByNick->end(); it != it_end; ++it)
			it->second->GenerateUID();
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
		ircdproto->SendLogout(u);
	}
};

MODULE_INIT(ProtoHybrid)
