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

IRCDVar myIrcd[] = {
	{"Bahamut 1.8.x",	/* ircd name */
	"+",					/* Modes used by pseudoclients */
	 1,					/* SVSNICK */
	 0,					/* Vhost */
	 1,					/* Supports SNlines */
	 1,					/* Supports SQlines */
	 1,					/* Supports SZlines */
	 0,					/* Join 2 Message */
	 1,					/* Chan SQlines */
	 1,					/* Quit on Kill */
	 0,					/* vidents */
	 1,					/* svshold */
	 1,					/* time stamp on mode */
	 0,					/* O:LINE */
	 1,					/* UMODE */
	 1,					/* No Knock requires +i */
	 0,					/* Can remove User Channel Modes with SVSMODE */
	 0,					/* Sglines are not enforced until user reconnects */
	 0,					/* ts6 */
	 "$",				/* TLD Prefix for Global */
	 6,					/* Max number of modes we can send per line */
	 0,					/* IRCd sends a SSL users certificate fingerprint */
	 }
	,
	{NULL}
};


class BahamutIRCdProto : public IRCDProto
{
	void SendModeInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf) anope_override
	{
		if (Capab.count("TSMODE") > 0)
			UplinkSocket::Message(source ? source->nick : Config->ServerName) << "MODE " << dest->name << " " << dest->creation_time << " " << buf;
		else
			UplinkSocket::Message(source ? source->nick : Config->ServerName) << "MODE " << dest->name << " " << buf;
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(bi ? bi->nick : Config->ServerName) << "SVSMODE " << u->nick << " " << u->timestamp << " " << buf;
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message(Config->ServerName) << "SVSHOLD " << nick << " " << Config->NSReleaseTimeout << " :Being held for registered user";
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message(Config->ServerName) << "SVSHOLD " << nick << " 0";
	}

	/* SQLINE */
	void SendSQLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message() << "SQLINE " << x->Mask << " :" << x->Reason;
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
		UplinkSocket::Message() << "SZLINE " << x->GetHost() << " :" << x->Reason;
		/* this is how we are supposed to deal with it */
		UplinkSocket::Message() << "AKILL " << x->GetHost() << " * " << timeleft << " " << x->By << " " << Anope::CurTime << " :" << x->Reason;
	}

	/* SVSNOOP */
	void SendSVSNOOP(const Server *server, bool set) anope_override
	{
		UplinkSocket::Message() << "SVSNOOP " << server->GetName() << " " << (set ? "+" : "-");
	}

	/* SGLINE */
	void SendSGLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message() << "SGLINE " << x->Mask.length() << " :" << x->Mask << ":" << x->Reason;
	}

	/* RAKILL */
	void SendAkillDel(const XLine *x) anope_override
	{
		UplinkSocket::Message() << "RAKILL " << x->GetHost() << " " << x->GetUser();
	}

	/* TOPIC */
	void SendTopic(BotInfo *whosets, Channel *c) anope_override
	{
		UplinkSocket::Message(whosets->nick) << "TOPIC " << c->name << " " << c->topic_setter << " " << c->topic_time << " :" << c->topic;
	}

	/* UNSQLINE */
	void SendSQLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message() << "UNSQLINE " << x->Mask;
	}

	/* JOIN - SJOIN */
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		UplinkSocket::Message(user->nick) << "SJOIN " << c->creation_time << " " << c->name;
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

	void SendAkill(User *, const XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800)
			timeleft = 172800;
		UplinkSocket::Message() << "AKILL " << x->GetHost() << " " << x->GetUser() << " " << timeleft << " " << x->By << " " << Anope::CurTime << " :" << x->Reason;
	}

	/*
	  Note: if the stamp is null 0, the below usage is correct of Bahamut
	*/
	void SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source ? source->nick : "") << "SVSKILL " << user->nick << " :" << buf;
	}

	void SendBOB() anope_override
	{
		UplinkSocket::Message() << "BURST";
	}

	void SendEOB() anope_override
	{
		UplinkSocket::Message() << "BURST 0";
	}

	void SendKickInternal(const BotInfo *source, const Channel *chan, const User *user, const Anope::string &buf) anope_override
	{
		if (!buf.empty())
			UplinkSocket::Message(source->nick) << "KICK " << chan->name << " " << user->nick << " :" << buf;
		else
			UplinkSocket::Message(source->nick) << "KICK " << chan->name << " " << user->nick;
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
		BotInfo *ns = findbot(Config->NickServ);
		ircdproto->SendMode(ns, u, "+d %d", u->timestamp);
	}

	void SendLogout(User *u) anope_override
	{
		BotInfo *ns = findbot(Config->NickServ);
		ircdproto->SendMode(ns, u, "+d 1");
	}
};

class BahamutIRCdMessage : public IRCdMessage
{
 public:
	bool OnMode(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() > 2 && (params[0][0] == '#' || params[0][0] == '&'))
			do_cmode(source, params[0], params[2], params[1]);
		else if (params.size() > 1)
			do_umode(params[0], params[1]);

		return true;
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
	bool OnNick(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() != 2)
		{
			/* Currently bahamut has no ipv6 support */
			sockaddrs ip;
			ip.ntop(AF_INET, params[8].c_str());

			User *user = do_nick(source, params[0], params[4], params[5], params[6], params[9], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : 0, ip.addr(), "", "", params[3]);
			if (user && nickserv)
			{
				NickAlias *na;
				if (user->timestamp == convertTo<time_t>(params[7]) && (na = findnick(user->nick)))
				{
					user->Login(na->nc);
					if (!Config->NoNicknameOwnership && na->nc->HasFlag(NI_UNCONFIRMED) == false)
						user->SetMode(findbot(Config->NickServ), UMODE_REGISTERED);
				}
				else
					nickserv->Validate(user);
			}
		}
		else
			do_nick(source, params[0], "", "", "", "", Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : 0, "", "", "", "");

		return true;
	}

	bool OnServer(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		do_server(source, params[0], Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0, params[2], "");
		return true;
	}

	bool OnTopic(const Anope::string &, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() < 4)
			return true;

		Channel *c = findchan(params[0]);
		if (!c)
		{
			Log() << "TOPIC for nonexistant channel " << params[0];
			return true;
		}

		c->ChangeTopicInternal(params[1], params[3], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime);

		return true;
	}

	bool OnSJoin(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
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
			c->SetModesInternal(NULL, modes);
		}

		/* For some reason, bahamut will send a SJOIN from the user joining a channel
		 * if the channel already existed
		 */
		if (!c->HasFlag(CH_SYNCING) && params.size() == 2)
		{
			User *u = finduser(source);
			if (!u)
				Log(LOG_DEBUG) << "SJOIN for nonexistant user " << source << " on " << c->name;
			else
			{
				EventReturn MOD_RESULT;
				FOREACH_RESULT(I_OnPreJoinChannel, OnPreJoinChannel(u, c));

				/* Add the user to the channel */
				c->JoinUser(u);

				/* Now set whatever modes this user is allowed to have on the channel */
				chan_set_correct_modes(u, c, 1);

				/* Check to see if modules want the user to join, if they do
				 * check to see if they are allowed to join (CheckKick will kick/ban them)
				 * Don't trigger OnJoinChannel event then as the user will be destroyed
				 */
				if (MOD_RESULT == EVENT_STOP && (!c->ci || !c->ci->CheckKick(u)))
				{
					FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(u, c));
				}
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
					c->SetModeInternal(*it, buf);

				/* Now set whatever modes this user is allowed to have on the channel */
				chan_set_correct_modes(u, c, 1);

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

bool event_burst(const Anope::string &source, const std::vector<Anope::string> &params)
{
	Server *s = Server::Find(source);

	if (params.empty())
	{
		/* for future use  - start burst */
	}
	else
	{
		/* If we found a server with the given source, that one just
		 * finished bursting. If there was no source, then our uplink
		 * server finished bursting. -GD
		 */
		if (!s)
			s = Me->GetLinks().front();
		if (s)
			s->Sync(true);
	}
	return true;
}

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

class ProtoBahamut : public Module
{
	Message message_svsmode, message_burst;
	
	BahamutIRCdProto ircd_proto;
	BahamutIRCdMessage ircd_message;

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
		message_svsmode("SVSMODE", OnMode), message_burst("BURST", event_burst)
	{
		this->SetAuthor("Anope");

		pmodule_ircd_var(myIrcd);
		pmodule_ircd_proto(&this->ircd_proto);
		pmodule_ircd_message(&this->ircd_message);

		this->AddModes();

		ModuleManager::Attach(I_OnUserNickChange, this);
	}

	~ProtoBahamut()
	{
		pmodule_ircd_var(NULL);
		pmodule_ircd_proto(NULL);
		pmodule_ircd_message(NULL);
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
	}
};

MODULE_INIT(ProtoBahamut)
