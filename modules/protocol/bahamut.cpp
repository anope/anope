/* Bahamut functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class ChannelModeFlood : public ChannelModeParam
{
 public:
	ChannelModeFlood(char modeChar, bool minusNoArg) : ChannelModeParam(CMODE_FLOOD, modeChar, minusNoArg) { }

	bool IsValid(const Anope::string &value) const anope_override
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
	BahamutIRCdProto() : IRCDProto("Bahamut 1.8.x")
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

	void SendModeInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf) anope_override
	{
		if (Capab.count("TSMODE") > 0)
		{
			if (source)
				UplinkSocket::Message(source) << "MODE " << dest->name << " " << dest->creation_time << " " << buf;
			else
				UplinkSocket::Message(Me) << "MODE " << dest->name << " " << dest->creation_time << " " << buf;
		}
		else
			IRCDProto::SendModeInternal(source, dest, buf);
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf) anope_override
	{
		if (bi)
			UplinkSocket::Message(bi) << "SVSMODE " << u->nick << " " << u->timestamp << " " << buf;
		else
			UplinkSocket::Message(Me) << "SVSMODE " << u->nick << " " << u->timestamp << " " << buf;
	}

	void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "NOTICE $" << dest->GetName() << " :" << msg;
	}

	void SendGlobalPrivmsg(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "PRIVMSG $" << dest->GetName() << " :" << msg;
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message(Me) << "SVSHOLD " << nick << " " << Config->NSReleaseTimeout << " :Being held for registered user";
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message(Me) << "SVSHOLD " << nick << " 0";
	}

	/* SQLINE */
	void SendSQLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message() << "SQLINE " << x->Mask << " :" << x->GetReason();
	}

	/* UNSLINE */
	void SendSGLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message() << "UNSGLINE 0 :" << x->Mask;
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
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		/* this will likely fail so its only here for legacy */
		UplinkSocket::Message() << "SZLINE " << x->GetHost() << " :" << x->GetReason();
		/* this is how we are supposed to deal with it */
		UplinkSocket::Message() << "AKILL " << x->GetHost() << " * " << timeleft << " " << x->By << " " << Anope::CurTime << " :" << x->GetReason();
	}

	/* SVSNOOP */
	void SendSVSNOOP(const Server *server, bool set) anope_override
	{
		UplinkSocket::Message() << "SVSNOOP " << server->GetName() << " " << (set ? "+" : "-");
	}

	/* SGLINE */
	void SendSGLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message() << "SGLINE " << x->Mask.length() << " :" << x->Mask << ":" << x->GetReason();
	}

	/* RAKILL */
	void SendAkillDel(const XLine *x) anope_override
	{
		if (x->IsRegex() || x->HasNickOrReal())
			return;

		/* ZLine if we can instead */
		try
		{
			if (x->GetUser() == "*")
			{
				sockaddrs(x->GetHost());
				ircdproto->SendSZLineDel(x);
				return;
			}
		}
		catch (const SocketException &) { }

		UplinkSocket::Message() << "RAKILL " << x->GetHost() << " " << x->GetUser();
	}

	/* TOPIC */
	void SendTopic(BotInfo *whosets, Channel *c) anope_override
	{
		UplinkSocket::Message(whosets) << "TOPIC " << c->name << " " << c->topic_setter << " " << c->topic_time << " :" << c->topic;
	}

	/* UNSQLINE */
	void SendSQLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message() << "UNSQLINE " << x->Mask;
	}

	/* JOIN - SJOIN */
	void SendJoin(const User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		UplinkSocket::Message(user) << "SJOIN " << c->creation_time << " " << c->name;
		if (status)
		{
			/* First save the channel status incase uc->Status == status */
			ChannelStatus cs = *status;
			/* If the user is internally on the channel with flags, kill them so that
			 * the stacker will allow this.
			 */
			UserContainer *uc = c->FindUser(user);
			if (uc != NULL)
				uc->Status->ClearFlags();

			BotInfo *setter = findbot(user->nick);
			for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
				if (cs.HasFlag(ModeManager::ChannelModes[i]->Name))
					c->SetMode(setter, ModeManager::ChannelModes[i], user->nick, false);
		}
	}

	void SendAkill(User *u, XLine *x) anope_override
	{
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
			x = new XLine("*@" + u->host, old->By, old->Expires, old->Reason, old->UID);
			old->manager->AddXLine(x);

			Log(findbot(Config->OperServ), "akill") << "AKILL: Added an akill for " << x->Mask << " because " << u->GetMask() << "#" << u->realname << " matches " << old->Mask;
		}

		/* ZLine if we can instead */
		try
		{
			if (x->GetUser() == "*")
			{
				sockaddrs(x->GetHost());
				ircdproto->SendSZLine(u, x);
				return;
			}
		}
		catch (const SocketException &) { }

		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800)
			timeleft = 172800;
		UplinkSocket::Message() << "AKILL " << x->GetHost() << " " << x->GetUser() << " " << timeleft << " " << x->By << " " << Anope::CurTime << " :" << x->GetReason();
	}

	/*
	  Note: if the stamp is null 0, the below usage is correct of Bahamut
	*/
	void SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf) anope_override
	{
		if (source)
			UplinkSocket::Message(source) << "SVSKILL " << user->nick << " :" << buf;
		else
			UplinkSocket::Message() << "SVSKILL " << user->nick << " :" << buf;
	}

	void SendBOB() anope_override
	{
		UplinkSocket::Message() << "BURST";
	}

	void SendEOB() anope_override
	{
		UplinkSocket::Message() << "BURST 0";
	}

	void SendClientIntroduction(const User *u) anope_override
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
		UplinkSocket::Message() << "PASS " << Config->Uplinks[CurrentUplink]->password << " :TS";
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

	void SendLogin(User *u) anope_override
	{
		const BotInfo *ns = findbot(Config->NickServ);
		ircdproto->SendMode(ns, u, "+d %d", u->timestamp);
	}

	void SendLogout(User *u) anope_override
	{
		const BotInfo *ns = findbot(Config->NickServ);
		ircdproto->SendMode(ns, u, "+d 1");
	}
};

struct IRCDMessageBurst : IRCDMessage
{
	IRCDMessageBurst() : IRCDMessage("BURST", 0) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
		return true;
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
	IRCDMessageNick() : IRCDMessage("NICK", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() == 10)
		{
			User *user = new User(params[0], params[4], params[5], "", params[8], source.GetServer(), params[9], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0, params[3]);
			if (user && nickserv)
			{
				const NickAlias *na;
				if (user->timestamp == convertTo<time_t>(params[7]) && (na = findnick(user->nick)))
				{
					NickCore *nc = na->nc;
					user->Login(nc);
					if (!Config->NoNicknameOwnership && na->nc->HasFlag(NI_UNCONFIRMED) == false)
						user->SetMode(findbot(Config->NickServ), UMODE_REGISTERED);
				}
				else
					nickserv->Validate(user);
			}
		}
		else
			source.GetUser()->ChangeNick(params[0]);

		return true;
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer() : IRCDMessage("SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		unsigned int hops = Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[2]);
		return true;
	}
};

struct IRCDMessageSJoin : IRCDMessage
{
	IRCDMessageSJoin() : IRCDMessage("SJOIN", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

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
		if (keep_their_modes && params.size() >= 4)
		{
			/* Set the modes internally */
			Anope::string modes;
			for (unsigned i = 2; i < params.size(); ++i)
				modes += " " + params[i];
			if (!modes.empty())
				modes.erase(modes.begin());
			c->SetModesInternal(source, modes);
		}

		/* For some reason, bahamut will send a SJOIN from the user joining a channel
		 * if the channel already existed
		 */
		if (!c->HasFlag(CH_SYNCING) && params.size() == 2)
		{
			User *u = source.GetUser();

			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnPreJoinChannel, OnPreJoinChannel(u, c));

			/* Add the user to the channel */
			c->JoinUser(u);

			/* Now set whatever modes this user is allowed to have on the channel */
			chan_set_correct_modes(u, c, 1, true);

			/* Check to see if modules want the user to join, if they do
			 * check to see if they are allowed to join (CheckKick will kick/ban them)
			 * Don't trigger OnJoinChannel event then as the user will be destroyed
			 */
			if (MOD_RESULT == EVENT_STOP && (!c->ci || !c->ci->CheckKick(u)))
			{
				FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(u, c));
			}
		}
		else
		{
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
						Log() << "Received unknown mode prefix " << cm << " in SJOIN string";
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

struct IRCDMessageTopic : IRCDMessage
{
	IRCDMessageTopic() : IRCDMessage("TOPIC", 4) { }

	bool Run(MessageSource &, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = findchan(params[0]);
		if (c)
			c->ChangeTopicInternal(params[1], params[3], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime);
		return true;
	}
};

class ProtoBahamut : public Module
{
	BahamutIRCdProto ircd_proto;

	/* Core message handlers */
	CoreIRCDMessageAway core_message_away;
	CoreIRCDMessageCapab core_message_capab;
	CoreIRCDMessageError core_message_error;
	CoreIRCDMessageJoin core_message_join;
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
	IRCDMessageBurst message_burst;
	IRCDMessageMode message_mode, message_svsmode;
	IRCDMessageNick message_nick;
	IRCDMessageServer message_server;
	IRCDMessageSJoin message_sjoin;
	IRCDMessageTopic message_topic;

	void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserMode(UMODE_SERV_ADMIN, 'A'));
		ModeManager::AddUserMode(new UserMode(UMODE_REGPRIV, 'R'));
		ModeManager::AddUserMode(new UserMode(UMODE_ADMIN, 'a'));
		ModeManager::AddUserMode(new UserMode(UMODE_INVIS, 'i'));
		ModeManager::AddUserMode(new UserMode(UMODE_OPER, 'o'));
		ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, 'r'));
		ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, 's'));
		ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, 'w'));
		ModeManager::AddUserMode(new UserMode(UMODE_DEAF, 'd'));

		/* b/e/I */
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_BAN, 'b'));

		/* v/h/o/a/q */
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, 'o', '@', 1));

		/* Add channel modes */
		ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCOLOR, 'c'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, 'i'));
		ModeManager::AddChannelMode(new ChannelModeFlood('f', false));
		ModeManager::AddChannelMode(new ChannelModeKey('k'));
		ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, 'l'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, 'm'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, 'n'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, 'p'));
		ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, 's'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, 't'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_REGMODERATED, 'M'));
		ModeManager::AddChannelMode(new ChannelModeOper('O'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_REGISTEREDONLY, 'R'));
	}

 public:
	ProtoBahamut(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL),
		message_mode("MODE"), message_svsmode("SVSMODE")
	{
		this->SetAuthor("Anope");

		this->AddModes();

		ModuleManager::Attach(I_OnUserNickChange, this);
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
		ircdproto->SendLogout(u);
	}
};

MODULE_INIT(ProtoBahamut)
