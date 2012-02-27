/* Inspircd 1.2+ generic TS6 functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

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

class InspIRCdTS6Proto : public IRCDProto
{
 private:
	void SendChgIdentInternal(const Anope::string &nick, const Anope::string &vIdent)
	{
		if (!has_chgidentmod)
			Log() << "CHGIDENT not loaded!";
		else
			UplinkSocket::Message(findbot(Config->HostServ)) << "CHGIDENT " << nick << " " << vIdent;
	}

	void SendChgHostInternal(const Anope::string &nick, const Anope::string &vhost)
	{
		if (!has_chghostmod)
			Log() << "CHGHOST not loaded!";
		else
			UplinkSocket::Message(Me) << "CHGHOST " << nick << " " << vhost;
	}

 public:

	void SendAkillDel(const XLine *x) anope_override
	{
		BotInfo *bi = findbot(Config->OperServ);

		/* InspIRCd may support regex bans */
		if (x->IsRegex() && has_rlinemod)
		{
			Anope::string mask = x->Mask;
			size_t h = x->Mask.find('#');
			if (h != Anope::string::npos)
				mask = mask.replace(h, 1, ' ');
			UplinkSocket::Message(bi) << "RLINE " << mask;
			return;
		}
		else if (x->IsRegex() || x->HasNickOrReal())
			return;

		UplinkSocket::Message(bi) << "GLINE " << x->Mask;
	}

	void SendTopic(BotInfo *whosets, Channel *c) anope_override
	{
		UplinkSocket::Message(whosets) << "FTOPIC " << c->name << " " << Anope::CurTime << " " << c->topic_setter << " :" << c->topic;
	}

	void SendVhostDel(User *u) anope_override
	{
		if (u->HasMode(UMODE_CLOAK))
			this->SendChgHostInternal(u->nick, u->chost);
		else
			this->SendChgHostInternal(u->nick, u->host);

		if (has_chgidentmod && u->GetIdent() != u->GetVIdent())
			this->SendChgIdentInternal(u->nick, u->GetIdent());
	}

	void SendAkill(User *u, XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;

		BotInfo *bi = findbot(Config->OperServ);
		/* InspIRCd may support regex bans, if they do we can send this and forget about it */
		if (x->IsRegex() && has_rlinemod)
		{
			Anope::string mask = x->Mask;
			size_t h = x->Mask.find('#');
			if (h != Anope::string::npos)
				mask = mask.replace(h, 1, ' ');
			UplinkSocket::Message(bi) << "RLINE " << mask << " " << timeleft << " :" << x->GetReason();
			return;
		}
		else if (x->IsRegex() || x->HasNickOrReal())
		{
			if (!u)
			{
				/* No user (this akill was just added), and contains nick and/or realname. Find users that match and ban them */
				for (Anope::insensitive_map<User *>::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
					if (x->manager->Check(it->second, x))
						this->SendAkill(it->second, x);
				return;
			}

			XLine *old = x;

			if (old->manager->HasEntry("*@" + u->host))
				return;

			/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
			x = new XLine("*@" + u->host, old->By, old->Expires, old->Reason, old->UID);
			old->manager->AddXLine(x);

			Log(bi, "akill") << "AKILL: Added an akill for " << x->Mask << " because " << u->GetMask() << "#" << u->realname << " matches " << old->Mask;
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

		UplinkSocket::Message(bi) << "ADDLINE G " << x->GetUser() << "@" << x->GetHost() << " " << x->By << " " << Anope::CurTime << " " << timeleft << " :" << x->GetReason();
	}

	void SendNumericInternal(int numeric, const Anope::string &dest, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message() << "PUSH " << dest << " ::" << Me->GetName() << " " << numeric << " " << dest << " " << buf;
	}

	void SendModeInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "FMODE " << dest->name << " " << dest->creation_time << " " << buf;
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(bi) << "MODE " << u->GetUID() << " " << buf;
	}

	void SendClientIntroduction(const User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "UID " << u->GetUID() << " " << u->timestamp << " " << u->nick << " " << u->host << " " << u->host << " " << u->GetIdent() << " 0.0.0.0 " << u->my_signon << " " << modes << " :" << u->realname;
	}

	/* SERVER services-dev.chatspike.net password 0 :Description here */
	void SendServer(const Server *server) anope_override
	{
		UplinkSocket::Message() << "SERVER " << server->GetName() << " " << Config->Uplinks[CurrentUplink]->password << " " << server->GetHops() << " " << server->GetSID() << " :" << server->GetDescription();
	}

	/* JOIN */
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		UplinkSocket::Message(Me) << "FJOIN " << c->name << " " << c->creation_time << " +" << c->GetModes(true, true) << " :," << user->GetUID();
		/* Note that we can send this with the FJOIN but choose not to
		 * because the mode stacker will handle this and probably will
		 * merge these modes with +nrt and other mlocked modes
		 */
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

	/* UNSQLINE */
	void SendSQLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(Me) << "DELLINE Q " << x->Mask;
	}

	/* SQLINE */
	void SendSQLine(User *, const XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		UplinkSocket::Message(Me) << "ADDLINE Q " << x->Mask << " " << Config->OperServ << " " << Anope::CurTime << " " << timeleft << " :" << x->GetReason();
	}

	/* Functions that use serval cmd functions */

	void SendVhost(User *u, const Anope::string &vIdent, const Anope::string &vhost) anope_override
	{
		if (!vIdent.empty())
			this->SendChgIdentInternal(u->nick, vIdent);
		if (!vhost.empty())
			this->SendChgHostInternal(u->nick, vhost);
	}

	void SendConnect() anope_override
	{
		SendServer(Me);
		UplinkSocket::Message(Me) << "BURST";
		Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
		UplinkSocket::Message(Me) << "VERSION :Anope-" << Anope::Version() << " " << Config->ServerName << " :" << ircd->name << " - (" << (enc ? enc->name : "unknown") << ") -- " << Anope::VersionBuildString();
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick) anope_override
	{
		BotInfo *bi = findbot(Config->NickServ);
		if (bi)
			UplinkSocket::Message(bi) << "SVSHOLD " << nick << " " << Config->NSReleaseTimeout << " :Being held for registered user";
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) anope_override
	{
		BotInfo *bi = findbot(Config->NickServ);
		if (bi)
			UplinkSocket::Message(bi) << "SVSHOLD " << nick;
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(Me) << "DELLINE Z " << x->GetHost();
	}

	/* SZLINE */
	void SendSZLine(User *, const XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		UplinkSocket::Message(Me) << "ADDLINE Z " << x->GetHost() << " " << x->By << " " << Anope::CurTime << " " << timeleft <<" :" << x->GetReason();
	}

	void SendSVSJoin(const BotInfo *source, const Anope::string &nick, const Anope::string &chan, const Anope::string &) anope_override
	{
		User *u = finduser(nick);
		UplinkSocket::Message(source) << "SVSJOIN " << u->GetUID() << " " << chan;
	}

	void SendSWhois(const BotInfo *, const Anope::string &who, const Anope::string &mask) anope_override
	{
		User *u = finduser(who);

		UplinkSocket::Message(Me) << "METADATA " << u->GetUID() << " swhois :" << mask;
	}

	void SendBOB() anope_override
	{
		UplinkSocket::Message(Me) << "BURST " << Anope::CurTime;
	}

	void SendEOB() anope_override
	{
		UplinkSocket::Message(Me) << "ENDBURST";
	}

	void SendGlobopsInternal(BotInfo *source, const Anope::string &buf)
	{
		if (has_globopsmod)
			UplinkSocket::Message(source) << "SNONOTICE g :" << buf;
		else
			UplinkSocket::Message(source) << "SNONOTICE A :" << buf;
	}

	void SendLogin(User *u) anope_override
	{
		if (!u->Account() || u->Account()->HasFlag(NI_UNCONFIRMED))
			return;

		UplinkSocket::Message(Me) << "METADATA " << u->GetUID() << " accountname :" << u->Account()->display;
	}

	void SendLogout(User *u) anope_override
	{
		UplinkSocket::Message(Me) << "METADATA " << u->GetUID() << " accountname :";
	}

	void SendChannel(Channel *c) anope_override
	{
		UplinkSocket::Message(Me) << "FJOIN " << c->name << " " << c->creation_time << " +" << c->GetModes(true, true) << " :";
	}

	bool IsNickValid(const Anope::string &nick) anope_override
	{
		/* InspIRCd, like TS6, uses UIDs on collision, so... */
		if (isdigit(nick[0]))
			return false;

		return true;
	}
};

class InspircdIRCdMessage : public IRCdMessage
{
 public:
	bool OnMode(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params[0][0] == '#' || params[0][0] == '&')
			do_cmode(source, params[0], params[2], params[1]);
		else
		{
			/* InspIRCd lets opers change another
			   users modes, we have to kludge this
			   as it slightly breaks RFC1459
			 */
			User *u = finduser(source);
			// This can happen with server-origin modes.
			if (!u)
				u = finduser(params[0]);
			// if it's still null, drop it like fire.
			// most likely situation was that server introduced a nick which we subsequently akilled
			if (!u)
				return true;

			do_umode(u->nick, params[1]);
		}

		return true;
	}

	virtual bool OnUID(const Anope::string &source, const std::vector<Anope::string> &params) anope_override = 0;

	bool OnNick(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		do_nick(source, params[0], "", "", "", "", 0, "", "", "", "");
		return true;
	}

	bool OnPrivmsg(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		/* Ignore privmsgs from the server, which can happen. */
		if (Server::Find(source) != NULL)
			return true;

		return IRCdMessage::OnPrivmsg(source, params);
	}

	/*
	 * [Nov 04 00:08:46.308435 2009] debug: Received: SERVER irc.inspircd.com pass 0 964 :Testnet Central!
	 * 0: name
	 * 1: pass
	 * 2: hops
	 * 3: numeric
	 * 4: desc
	 */
	bool OnServer(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		do_server(source, params[0], Anope::string(params[2]).is_pos_number_only() ? convertTo<unsigned>(params[2]) : 0, params[4], params[3]);
		return true;
	}

	bool OnTopic(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = findchan(params[0]);

		if (!c)
		{
			Log() << "TOPIC " << params[1] << " for nonexistent channel " << params[0];
			return true;
		}

		c->ChangeTopicInternal(source, (params.size() > 1 ? params[1] : ""), Anope::CurTime);

		return true;
	}

	virtual bool OnCapab(const Anope::string &, const std::vector<Anope::string> &) anope_override = 0;

	bool OnSJoin(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = findchan(params[0]);
		time_t ts = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : 0;
		bool keep_their_modes = true;

		if (!c)
		{
			c = new Channel(params[0], ts);
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

		/* If we need to keep their modes, and this FJOIN string contains modes */
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

			/* Loop through prefixes and find modes for them */
			while (buf[0] != ',')
			{
				ChannelMode *cm = ModeManager::FindChannelModeByChar(buf[0]);
				if (!cm)
				{
					Log() << "Received unknown mode prefix " << buf[0] << " in FJOIN string";
					buf.erase(buf.begin());
					continue;
				}

				buf.erase(buf.begin());
				if (keep_their_modes)
					Status.push_back(cm);
			}
			buf.erase(buf.begin());

			User *u = finduser(buf);
			if (!u)
			{
				Log(LOG_DEBUG) << "FJOIN for nonexistant user " << buf << " on " << c->name;
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

bool event_idle(const Anope::string &source, const std::vector<Anope::string> &params)
{
	BotInfo *bi = findbot(params[0]);
	if (bi)
		UplinkSocket::Message(bi) << "IDLE " << source << " " << start_time << " " << (Anope::CurTime - bi->lastmsg);
	return true;
}

bool event_time(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() < 2)
		return true;

	UplinkSocket::Message(Me) << "TIME " << source << " " << params[1] << " " << Anope::CurTime;
	return true;
}

bool event_rsquit(const Anope::string &source, const std::vector<Anope::string> &params)
{
	/* On InspIRCd we must send a SQUIT when we recieve RSQUIT for a server we have juped */
	Server *s = Server::Find(params[0]);
	if (s && s->HasFlag(SERVER_JUPED))
		UplinkSocket::Message(Me) << "SQUIT " << s->GetSID() << " :" << (params.size() > 1 ? params[1].c_str() : "");

	ircdmessage->OnSQuit(source, params);

	return true;
}


