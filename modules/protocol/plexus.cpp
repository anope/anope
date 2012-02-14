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

static Anope::string TS6UPLINK;

IRCDVar myIrcd[] = {
	{"hybrid-7.2.3+plexus-3.0.1",	/* ircd name */
	 "+oi",				/* Modes used by pseudoclients */
	 1,				/* SVSNICK */
	 1,				/* Vhost */
	 1,				/* Supports SNlines */
	 1,				/* Supports SQlines */
	 0,				/* Supports SZlines */
	 0,				/* Join 2 Message */
	 1,				/* Chan SQlines */
	 0,				/* Quit on Kill */
	 1,				/* vidents */
	 1,				/* svshold */
	 1,				/* time stamp on mode */
	 1,				/* UMODE */
	 0,				/* O:LINE */
	 0,				/* No Knock requires +i */
	 0,				/* Can remove User Channel Modes with SVSMODE */
	 0,				/* Sglines are not enforced until user reconnects */
	 1,				/* ts6 */
	 "$$",			/* TLD Prefix for Global */
	 4,				/* Max number of modes we can send per line */
	 1,					/* IRCd sends a SSL users certificate fingerprint */
	 }
	,
	{NULL}
};

class PlexusProto : public IRCDProto
{
	void SendGlobopsInternal(const BotInfo *source, const Anope::string &buf)
	{
		UplinkSocket::Message(source ? source->GetUID() : Me->GetSID()) << "OPERWALL :" << buf;
	}

	void SendSQLine(User *, const XLine *x)
	{
		UplinkSocket::Message(Me->GetSID()) << "RESV * " << x->Mask << " :" << x->Reason;
	}

	void SendSGLineDel(const XLine *x)
	{
		BotInfo *bi = findbot(Config->OperServ);
		UplinkSocket::Message(bi ? bi->GetUID() : Config->OperServ) << "UNXLINE * " << x->Mask;
	}

	void SendSGLine(User *, const XLine *x)
	{
		BotInfo *bi = findbot(Config->OperServ);
		UplinkSocket::Message(bi ? bi->GetUID() : Config->OperServ) << "XLINE * " << x->Mask << " 0 :" << x->Reason;
	}

	void SendAkillDel(const XLine *x)
	{
		BotInfo *bi = findbot(Config->OperServ);
		UplinkSocket::Message(bi ? bi->GetUID() : Config->OperServ) << "UNKLINE * " << x->GetUser() << " " << x->GetHost();
	}

	void SendSQLineDel(const XLine *x)
	{
		UplinkSocket::Message(Me->GetSID()) <<  "UNRESV * " << x->Mask;
	}

	void SendJoin(User *user, Channel *c, const ChannelStatus *status)
	{
		UplinkSocket::Message(Me->GetSID()) << "SJOIN " << c->creation_time << " " << c->name << " +" << c->GetModes(true, true) << " :" << user->GetUID();
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

	void SendAkill(User *, const XLine *x)
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		BotInfo *bi = findbot(Config->OperServ);
		UplinkSocket::Message(bi ? bi->GetUID() : Config->OperServ) << "KLINE * " << timeleft << " " << x->GetUser() << " " << x->GetHost() << " :" << x->Reason;
	}

	void SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf)
	{
		UplinkSocket::Message(source ? source->GetUID() : Me->GetSID()) << "KILL " << user->GetUID() << " :" << buf;
	}

	void SendServer(const Server *server)
	{
		if (server == Me)
			UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :" << server->GetDescription();
		else
			UplinkSocket::Message(Me->GetSID()) << "SID " << server->GetName() << " " << server->GetHops() << " " << server->GetSID() << " :" << server->GetDescription();
	}

	void SendForceNickChange(const User *u, const Anope::string &newnick, time_t when)
	{
		UplinkSocket::Message(Me->GetSID()) << "ENCAP " << u->server->GetName() << " SVSNICK " << u->GetUID() << " " << u->timestamp << " " << newnick << " " << when;
	}

	void SendVhostDel(User *u)
	{
		BotInfo *bi = findbot(Config->HostServ);
		if (u->HasMode(UMODE_CLOAK))
			u->RemoveMode(bi, UMODE_CLOAK);
		else
			this->SendVhost(u, u->GetIdent(), u->chost);
	}

	void SendVhost(User *u, const Anope::string &ident, const Anope::string &host)
	{
		if (!ident.empty())
			UplinkSocket::Message(Me->GetSID()) << "ENCAP * CHGIDENT " << u->GetUID() << " " << ident;
		UplinkSocket::Message(Me->GetSID()) << "ENCAP * CHGHOST " << u->GetUID() << " " << host;
	}

	void SendConnect()
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

	void SendClientIntroduction(const User *u)
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me->GetSID()) << "UID " << u->nick << " 1 " << u->timestamp << " " << modes << " " << u->GetIdent() << " " << u->host << " 255.255.255.255 " << u->GetUID() << " 0 " << u->host << " :" << u->realname;
	}

	void SendPartInternal(const BotInfo *bi, const Channel *chan, const Anope::string &buf)
	{
		if (!buf.empty())
			UplinkSocket::Message(bi->GetUID()) << "PART " << chan->name << " :" << buf;
		else
			UplinkSocket::Message(bi->GetUID()) << "PART " << chan->name;
	}

	void SendModeInternal(const BotInfo *bi, const Channel *dest, const Anope::string &buf)
	{
		UplinkSocket::Message(bi ? bi->GetUID() : Me->GetSID()) << "MODE " << dest->name << " " << buf;
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf)
	{
		UplinkSocket::Message(bi ? bi->GetUID() : Me->GetSID()) << "ENCAP * SVSMODE " << u->GetUID() << " " << u->timestamp << " " << buf;
	}

	void SendKickInternal(const BotInfo *bi, const Channel *chan, const User *user, const Anope::string &buf)
	{
		if (!buf.empty())
			UplinkSocket::Message(bi->GetUID()) << "KICK " << chan->name << " " << user->GetUID() << " :" << buf;
		else
			UplinkSocket::Message(bi->GetUID()) << "KICK " << chan->name << " " << user->GetUID();
	}

	/* INVITE */
	void SendInvite(BotInfo *source, const Anope::string &chan, const Anope::string &nick)
	{
		User *u = finduser(nick);
		UplinkSocket::Message(source->GetUID()) << "INVITE " << (u ? u->GetUID() : nick) << " " <<  chan;
	}

	void SendLogin(User *u)
	{
		if (!u->Account())
			return;

		UplinkSocket::Message(Me->GetSID()) << "ENCAP * SU " << u->GetUID() << " " << u->Account()->display;
	}

	void SendLogout(User *u)
	{
		UplinkSocket::Message(Me->GetSID()) << "ENCAP * SU " << u->GetUID();
	}

	void SendTopic(BotInfo *bi, Channel *c)
	{
		UplinkSocket::Message(bi->GetUID()) << "ENCAP * TOPIC " << c->name << " " << c->topic_setter << " " << c->topic_time + 1 << " :" << c->topic;
	}

	void SendChannel(Channel *c)
	{
		Anope::string modes = c->GetModes(true, true);
		if (modes.empty())
			modes = "+";
		UplinkSocket::Message(Me->GetSID()) << "SJOIN " << c->creation_time << " " << c->name << " " << modes << "%s :";
	}
};

class PlexusIRCdMessage : public IRCdMessage
{
 public:
 	/*
	 * params[0] = ts
	 * params[1] = channel
	 */
	bool OnJoin(const Anope::string &source, const std::vector<Anope::string> &params)
	{
		if (params.size() < 2)
			return IRCdMessage::OnJoin(source, params);
		do_join(source, params[1], params[0]);
		return true;
	}

	bool OnMode(const Anope::string &source, const std::vector<Anope::string> &params)
	{
		if (params.size() < 2)
			return true;

		if (params[0][0] == '#' || params[0][0] == '&')
			do_cmode(source, params[0], params[2], params[1]);
		else
			do_umode(params[0], params[1]);

		return true;
	}

	/*
	   TS6
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
	bool OnUID(const Anope::string &source, const std::vector<Anope::string> &params)
	{
		/* An IP of 0 means the user is spoofed */
		Anope::string ip = params[6];
		if (ip == "0")
			ip.clear();
		User *user = do_nick("", params[0], params[4], params[9], source, params[10], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : 0, ip, params[5], params[7], params[3]);
		if (nickserv && user && user->server->IsSynced())
			nickserv->Validate(user);

		return true;
	}

	bool OnNick(const Anope::string &source, const std::vector<Anope::string> &params)
	{
		do_nick(source, params[0], "", "", "", "", Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : 0, "", "", "", "");
		return true;
	}

	bool OnServer(const Anope::string &source, const std::vector<Anope::string> &params)
	{
		if (params[1].equals_cs("1"))
			do_server(source, params[0], Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0, params[2], TS6UPLINK);
		else
			do_server(source, params[0], Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0, params[2], "");
		return true;
	}

	/*
	   TS6
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
	bool OnTopic(const Anope::string &source, const std::vector<Anope::string> &params)
	{
		Channel *c = findchan(params[0]);
		if (!c)
		{
			Log() << "TOPIC for nonexistant channel " << params[0];
			return true;
		}

		if (params.size() == 4)
			c->ChangeTopicInternal(params[1], params[3], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime);
		else
			c->ChangeTopicInternal(source, (params.size() > 1 ? params[1] : ""));

		return true;
	}

	bool OnSJoin(const Anope::string &source, const std::vector<Anope::string> &params)
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
			c->SetModesInternal(NULL, modes);
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

bool event_tburst(const Anope::string &source, const std::vector<Anope::string> &params)
{
	// :rizon.server TBURST 1298674830 #lol 1298674833 Adam!Adam@i.has.a.spoof :lol
	if (params.size() < 4)
		return true;

	Channel *c = findchan(params[1]);

	if (!c)
	{
		Log() << "TOPIC " << params[3] << " for nonexistent channel " << params[0];
		return true;
	}
	else if (c->creation_time < convertTo<time_t>(params[0]))
		return true;

	Anope::string setter = myStrGetToken(params[3], '!', 0);
	time_t topic_time = Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime;
	c->ChangeTopicInternal(setter, params.size() > 4 ? params[4] : "", topic_time);

	return true;
}

bool event_sid(const Anope::string &source, const std::vector<Anope::string> &params)
{
	/* :42X SID trystan.nomadirc.net 2 43X :ircd-plexus test server */
	Server *s = Server::Find(source);

	do_server(s->GetName(), params[0], Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0, params[3], params[2]);
	return true;
}

bool event_mode(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() < 2)
		return true;

	if (params[0][0] == '#' || params[0][0] == '&')
		do_cmode(source, params[0], params[2], params[1]);
	else
		do_umode(params[0], params[1]);
	return true;
}

bool event_tmode(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params[1][0] == '#')
	{
		Anope::string modes = params[2];
		for (unsigned i = 3; i < params.size(); ++i)
			modes += " " + params[i];
		do_cmode(source, params[1], modes, params[0]);
	}
	return true;
}

bool event_pass(const Anope::string &source, const std::vector<Anope::string> &params)
{
	TS6UPLINK = params[3];
	return true;
}

bool event_bmask(const Anope::string &source, const std::vector<Anope::string> &params)
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
		for (i = 0; i <= count - 1; ++i)
		{
			Anope::string b = myStrGetToken(bans, ' ', i);
			if (ban && params[2].equals_cs("b"))
				c->SetModeInternal(ban, b);
			else if (except && params[2].equals_cs("e"))
				c->SetModeInternal(except, b);
			if (invex && params[2].equals_cs("I"))
				c->SetModeInternal(invex, b);
		}
	}
	return true;
}

bool event_encap(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() < 4)
		return true;
	/*
	 * Received: :dev.anope.de ENCAP * SU DukePyrolator DukePyrolator
	 * params[0] = *
	 * params[1] = SU
	 * params[2] = nickname
	 * params[3] = account
	 */
	else if (params[1].equals_cs("SU"))
	{
		User *u = finduser(params[2]);
		NickAlias *user_na = findnick(params[2]);
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

bool event_eob(const Anope::string &source, const std::vector<Anope::string> &params)
{
	Server *s = Server::Find(source);
	if (s)
		s->Sync(true);

	return true;
}

class ProtoPlexus : public Module
{
	Message message_tmode, message_bmask, message_pass,
		message_tb, message_sid, message_encap,
		message_eob;

	PlexusProto ircd_proto;
	PlexusIRCdMessage ircd_message;

	void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserMode(UMODE_ADMIN, 'a'));
		ModeManager::AddUserMode(new UserMode(UMODE_INVIS, 'i'));
		ModeManager::AddUserMode(new UserMode(UMODE_OPER, 'o'));
		ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, 's'));
		ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, 'w'));
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
	ProtoPlexus(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL),
		message_tmode("TMODE", event_tmode), message_bmask("BMASK", event_bmask),
		message_pass("PASS", event_pass), message_tb("TBURST", event_tburst),
		message_sid("SID", event_sid), message_encap("ENCAP", event_encap),
		message_eob("EOB", event_eob)
	{
		this->SetAuthor("Anope");

		pmodule_ircd_var(myIrcd);
		pmodule_ircd_proto(&this->ircd_proto);
		pmodule_ircd_message(&this->ircd_message);

		this->AddModes();

		Implementation i[] = { I_OnServerSync };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		if (Config->Numeric.empty())
		{
			Anope::string numeric = ts6_sid_retrieve();
			Me->SetSID(numeric);
			Config->Numeric = numeric;
		}

		for (botinfo_map::iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
			it->second->GenerateUID();
	}

	~ProtoPlexus()
	{
		pmodule_ircd_var(NULL);
		pmodule_ircd_proto(NULL);
		pmodule_ircd_message(NULL);
	}

	void OnServerSync(Server *s)
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
