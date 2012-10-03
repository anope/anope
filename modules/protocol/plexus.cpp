/* Plexus 3+ IRCD functions
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

class PlexusProto : public IRCDProto
{
 public:
	PlexusProto() : IRCDProto("hybrid-7.2.3+plexus-3.0.1")
	{
		DefaultPseudoclientModes = "+oiU";
		CanSVSNick = true;
		CanSetVHost = true;
		CanSetVIdent = true;
		CanSNLine = true;
		CanSQLine = true;
		CanSQLineChannel = true;
		CanSVSHold = true;
		CanCertFP = true;
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
		UplinkSocket::Message(Me) <<  "UNRESV * " << x->Mask;
	}

	void SendJoin(const User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		UplinkSocket::Message(Me) << "SJOIN " << c->creation_time << " " << c->name << " +" << c->GetModes(true, true) << " :" << user->GetUID();
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

	void SendServer(const Server *server) anope_override
	{
		if (server == Me)
			UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :" << server->GetDescription();
		else
			UplinkSocket::Message(Me) << "SID " << server->GetName() << " " << server->GetHops() << " " << server->GetSID() << " :" << server->GetDescription();
	}

	void SendForceNickChange(const User *u, const Anope::string &newnick, time_t when) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP " << u->server->GetName() << " SVSNICK " << u->GetUID() << " " << u->timestamp << " " << newnick << " " << when;
	}

	void SendVhostDel(User *u) anope_override
	{
		const BotInfo *bi = findbot(Config->HostServ);
		if (u->HasMode(UMODE_CLOAK))
			u->RemoveMode(bi, UMODE_CLOAK);
		else
			this->SendVhost(u, u->GetIdent(), u->chost);
	}

	void SendVhost(User *u, const Anope::string &ident, const Anope::string &host) anope_override
	{
		if (!ident.empty())
			UplinkSocket::Message(Me) << "ENCAP * CHGIDENT " << u->GetUID() << " " << ident;
		UplinkSocket::Message(Me) << "ENCAP * CHGHOST " << u->GetUID() << " " << host;
	}

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "PASS " << Config->Uplinks[CurrentUplink]->password << " TS 6 :" << Me->GetSID();
		/* CAPAB
		 * QS     - Can handle quit storm removal
		 * EX     - Can do channel +e exemptions
		 * CHW    - Can do channel wall @#
		 * LL     - Can do lazy links
		 * IE     - Can do invite exceptions
		 * EOB    - Can do EOB message
		 * KLN    - Can do KLINE message
		 * GLN    - Can do GLINE message
		 * HUB    - This server is a HUB
		 * AOPS   - Can do anon ops (+a)
		 * UID    - Can do UIDs
		 * ZIP    - Can do ZIPlinks
		 * ENC    - Can do ENCrypted links
		 * KNOCK  - Supports KNOCK
		 * TBURST - Supports TBURST
		 * PARA   - Supports invite broadcasting for +p
		 * ENCAP  - Supports encapsulization of protocol messages
		 * SVS    - Supports services protocol extensions
		 */
		UplinkSocket::Message() << "CAPAB :QS EX CHW IE EOB KLN UNKLN GLN HUB KNOCK TBURST PARA ENCAP SVS";
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
		UplinkSocket::Message() << "SVINFO 6 5 0 :" << Anope::CurTime;
	}

	void SendClientIntroduction(const User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "UID " << u->nick << " 1 " << u->timestamp << " " << modes << " " << u->GetIdent() << " " << u->host << " 255.255.255.255 " << u->GetUID() << " 0 " << u->host << " :" << u->realname;
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(bi) << "ENCAP * SVSMODE " << u->GetUID() << " " << u->timestamp << " " << buf;
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

	void SendTopic(BotInfo *bi, Channel *c) anope_override
	{
		UplinkSocket::Message(bi) << "ENCAP * TOPIC " << c->name << " " << c->topic_setter << " " << c->topic_ts << " :" << c->topic;
	}

	void SendChannel(Channel *c) anope_override
	{
		Anope::string modes = c->GetModes(true, true);
		if (modes.empty())
			modes = "+";
		UplinkSocket::Message(Me) << "SJOIN " << c->creation_time << " " << c->name << " " << modes << " :";
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
	IRCDMessageEncap() : IRCDMessage("ENCAP", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/*
		 * Received: :dev.anope.de ENCAP * SU DukePyrolator DukePyrolator
		 * params[0] = *
		 * params[1] = SU
		 * params[2] = nickname
		 * params[3] = account
		 */
		if (params[1].equals_cs("SU"))
		{
			User *u = finduser(params[2]);
			const NickAlias *user_na = findnick(params[2]);
			NickCore *nc = findcore(params[3]);
			if (u && nc)
			{
				u->Login(nc);
				if (!Config->NoNicknameOwnership && user_na && user_na->nc == nc && user_na->nc->HasFlag(NI_UNCONFIRMED) == false)
					u->SetMode(findbot(Config->NickServ), UMODE_REGISTERED);
			}
		}

		/*
		 * Received: :dev.anope.de ENCAP * CERTFP DukePyrolator :3F122A9CC7811DBAD3566BF2CEC3009007C0868F
		 * params[0] = *
		 * params[1] = CERTFP
		 * params[2] = nickname
		 * params[3] = fingerprint
		 */
		else if (params[1].equals_cs("CERTFP"))
		{
			User *u = finduser(params[2]);
			if (u)
			{
				u->fingerprint = params[3];
				FOREACH_MOD(I_OnFingerprint, OnFingerprint(u));
			}
		}
		return true;
	}
};

struct IRCDMessageEOB : IRCDMessage
{
	IRCDMessageEOB() : IRCDMessage("EOB", 0) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetServer()->Sync(true);
		return true;
	}
};

struct IRCDMessageJoin : CoreIRCDMessageJoin
{
	IRCDMessageJoin() : CoreIRCDMessageJoin("JOIN") { }

 	/*
	 * params[0] = ts
	 * params[1] = channel
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
	IRCDMessageNick() : IRCDMessage("NICK", 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

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

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer() : IRCDMessage("SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		// Servers other then our immediate uplink are introduced via SID
		if (params[1] != "1")
			return true;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params[2], UplinkSID);
		return true;
	}
};

struct IRCDMessageSID : IRCDMessage
{
	IRCDMessageSID() : IRCDMessage("SID", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/* :42X SID trystan.nomadirc.net 2 43X :ircd-plexus test server */
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		unsigned int hops = params[1].is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[3], params[2]);
		return true;
	}
};

struct IRCDMessageSJoin : IRCDMessage
{
	IRCDMessageSJoin() : IRCDMessage("SJOIN", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

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
	IRCDMessageTBurst() : IRCDMessage("TBURST", 5) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		//                      0          1    2          3                        4
		// :rizon.server TBURST 1298674830 #lol 1298674833 Adam!Adam@i.has.a.spoof :lol
		//                      chan ts         topic ts

		Channel *c = findchan(params[1]);

		try
		{
			if (!c || c->creation_time < convertTo<time_t>(params[0]))
				return true;
		}
		catch (const ConvertException &)
		{
			return true;
		}

		Anope::string setter = myStrGetToken(params[3], '!', 0);
		time_t topic_time = Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime;
		c->ChangeTopicInternal(setter, params.size() > 4 ? params[4] : "", topic_time);

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

struct IRCDMessageTopic : IRCDMessage
{
	IRCDMessageTopic() : IRCDMessage("TOPIC", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = findchan(params[0]);
		if (!c)
			return true;

		c->ChangeTopicInternal(params[1], params[3], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime);
		return true;
	}
};

struct IRCDMessageUID : IRCDMessage
{
	IRCDMessageUID() : IRCDMessage("UID", 11) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*
	   params[0] = nick
	   params[1] = hop
	   params[2] = ts
	   params[3] = modes
	   params[4] = user
	   params[5] = host
	   params[6] = IP
	   params[7] = UID
	   params[8] = services stamp
	   params[9] = realhost
	   params[10] = info
	*/
	// :42X UID Adam 1 1348535644 +aow Adam 192.168.0.5 192.168.0.5 42XAAAAAB 0 192.168.0.5 :Adam
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* An IP of 0 means the user is spoofed */
		Anope::string ip = params[6];
		if (ip == "0")
			ip.clear();

		User *user = new User(params[0], params[4], params[9], params[5], ip, source.GetServer(), params[10], params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : 0, params[3], params[7]);
		if (user && user->server->IsSynced() && nickserv)
			nickserv->Validate(user);

		return true;
	}
};

class ProtoPlexus : public Module
{
	PlexusProto ircd_proto;

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
	IRCDMessageEncap message_encap;
	IRCDMessageEOB message_eob;
	IRCDMessageJoin message_join;
	IRCDMessageMode message_mode;
	IRCDMessageNick message_nick;
	IRCDMessagePass message_pass;
	IRCDMessageServer message_server;
	IRCDMessageSID message_sid;
	IRCDMessageSJoin message_sjoin;
	IRCDMessageTBurst message_tburst;
	IRCDMessageTMode message_tmode;
	IRCDMessageTopic message_topic;
	IRCDMessageUID message_uid;

	void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserMode(UMODE_ADMIN, 'a'));
		ModeManager::AddUserMode(new UserMode(UMODE_INVIS, 'i'));
		ModeManager::AddUserMode(new UserMode(UMODE_OPER, 'o'));
		ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, 's'));
		ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, 'w'));
		ModeManager::AddUserMode(new UserMode(UMODE_NOCTCP, 'C'));
		ModeManager::AddUserMode(new UserMode(UMODE_DEAF, 'D'));
		ModeManager::AddUserMode(new UserMode(UMODE_SOFTCALLERID, 'G'));
		ModeManager::AddUserMode(new UserMode(UMODE_NETADMIN, 'N'));
		ModeManager::AddUserMode(new UserMode(UMODE_REGPRIV, 'R'));
		ModeManager::AddUserMode(new UserMode(UMODE_SSL, 'S'));
		ModeManager::AddUserMode(new UserMode(UMODE_WEBIRC, 'W'));
		ModeManager::AddUserMode(new UserMode(UMODE_CALLERID, 'g'));
		ModeManager::AddUserMode(new UserMode(UMODE_PRIV, 'p'));
		ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, 'r'));
		ModeManager::AddUserMode(new UserMode(UMODE_CLOAK, 'x'));
		ModeManager::AddUserMode(new UserMode(UMODE_PROTECTED, 'U'));

		/* b/e/I */
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_BAN, 'b'));
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_EXCEPT, 'e'));
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_INVITEOVERRIDE, 'I'));
	
		/* l/k */
		ModeManager::AddChannelMode(new ChannelModeKey('k'));
		ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, 'l'));

		/* v/h/o/a/q */
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_HALFOP, 'h', '%', 1));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, 'o', '@', 2));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT, 'a', '&', 3));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, 'q', '~', 4));

		/* Add channel modes */
		ModeManager::AddChannelMode(new ChannelMode(CMODE_BANDWIDTH, 'B'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_NOCTCP, 'C'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_REGMODERATED, 'M'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_NONOTICE, 'N'));
		ModeManager::AddChannelMode(new ChannelModeOper('O'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_REGISTEREDONLY, 'R'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_SSL, 'S'));

		ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCOLOR, 'c'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, 'i'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, 'm'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, 'n'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, 'p'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, 's'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, 't'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_PERM, 'z'));
	}

 public:
	ProtoPlexus(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL)
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

MODULE_INIT(ProtoPlexus)
