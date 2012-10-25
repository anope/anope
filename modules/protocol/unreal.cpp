/* Unreal IRCD 3.2.x functions
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

class UnrealIRCdProto : public IRCDProto
{
 public:
	UnrealIRCdProto() : IRCDProto("UnrealIRCd 3.2.x")
	{
		DefaultPseudoclientModes = "+Soiq";
		CanSVSNick = true;
		CanSetVHost = true;
		CanSetVIdent = true;
		CanSNLine = true;
		CanSQLine = true;
		CanSZLine = true;
		CanSVSHold = true;
		CanSVSO = true;
		MaxModes = 12;
	}

 private:
	/* SVSNOOP */
	void SendSVSNOOP(const Server *server, bool set) anope_override
	{
		UplinkSocket::Message() << "f " << server->GetName() << " " << (set ? "+" : "-");
	}

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

		UplinkSocket::Message() << "BD - G " << x->GetUser() << " " << x->GetHost() << " " << Config->OperServ;
	}

	void SendTopic(BotInfo *whosets, Channel *c) anope_override
	{
		UplinkSocket::Message(whosets) << ") " << c->name << " " << c->topic_setter << " " << c->topic_ts << " :" << c->topic;
	}

	void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "NOTICE $" << dest->GetName() << " :" << msg;
	}

	void SendGlobalPrivmsg(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "PRIVMSG $" << dest->GetName() << " :" << msg;
	}

	void SendVhostDel(User *u) anope_override
	{
		const BotInfo *bi = findbot(Config->HostServ);
		u->RemoveMode(bi, UMODE_CLOAK);
		u->RemoveMode(bi, UMODE_VHOST);
		ModeManager::ProcessModes();
		u->SetMode(bi, UMODE_CLOAK);
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
			XLine *xline = new XLine("*@" + u->host, old->By, old->Expires, old->Reason, old->UID);
			old->manager->AddXLine(xline);
			x = xline;

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
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		UplinkSocket::Message() << "BD + G " << x->GetUser() << " " << x->GetHost() << " " << x->By << " " << Anope::CurTime + timeleft << " " << x->Created << " :" << x->GetReason();
	}

	void SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "h " << user->nick << " :" << buf;
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(bi) << "v " << u->nick <<" " << buf;
	}

	void SendClientIntroduction(const User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message() << "& " << u->nick << " 1 " << u->timestamp << " " << u->GetIdent() << " " << u->host << " " << u->server->GetName() << " 0 " << modes << " " << u->host << " * :" << u->realname;
	}

	/* SERVER name hop descript */
	/* Unreal 3.2 actually sends some info about itself in the descript area */
	void SendServer(const Server *server) anope_override
	{
		if (!Config->Numeric.empty())
			UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :U0-*-" << Config->Numeric << " " << server->GetDescription();
		else
			UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :" << server->GetDescription();
	}

	/* JOIN */
	void SendJoin(const User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		UplinkSocket::Message(Me) << "~ " << c->creation_time << " " << c->name << " :" << user->nick;
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

	/* unsqline
	*/
	void SendSQLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message() << "d " << x->Mask;
	}

	/* SQLINE */
	/*
	** - Unreal will translate this to TKL for us
	**
	*/
	void SendSQLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message() << "c " << x->Mask << " :" << x->GetReason();
	}

	/*
	** svso
	**	  parv[0] = sender prefix
	**	  parv[1] = nick
	**	  parv[2] = options
	*/
	void SendSVSO(const BotInfo *source, const Anope::string &nick, const Anope::string &flag) anope_override
	{
		UplinkSocket::Message(source) << "BB " << nick << " " << flag;
	}

	/* NICK <newnick>  */
	void SendChangeBotNick(const BotInfo *oldnick, const Anope::string &newnick) anope_override
	{
		UplinkSocket::Message(oldnick) << "& " << newnick << " " << Anope::CurTime;
	}

	/* Functions that use serval cmd functions */

	void SendVhost(User *u, const Anope::string &vIdent, const Anope::string &vhost) anope_override
	{
		if (!vIdent.empty())
			UplinkSocket::Message(Me) << "AZ " << u->nick << " " << vIdent;
		if (!vhost.empty())
			UplinkSocket::Message(Me) << "AL " << u->nick << " " << vhost;
	}

	void SendConnect() anope_override
	{
		/*
		   NICKv2 = Nick Version 2
		   VHP    = Sends hidden host
		   UMODE2 = sends UMODE2 on user modes
		   NICKIP = Sends IP on NICK
		   TOKEN  = Use tokens to talk
		   SJ3    = Supports SJOIN
		   NOQUIT = No Quit
		   TKLEXT = Extended TKL we don't use it but best to have it
		   SJB64  = Base64 encoded time stamps
		   ESVID  = Allows storing account names as services stamp
		   MLOCK  = Supports the MLOCK server command
		   VL     = Version Info
		   NS     = Config->Numeric Server
		*/
		if (!Config->Numeric.empty())
			UplinkSocket::Message() << "PROTOCTL NICKv2 VHP UMODE2 NICKIP SJOIN SJOIN2 SJ3 NOQUIT TKLEXT ESVID MLOCK VL";
		else
			UplinkSocket::Message() << "PROTOCTL NICKv2 VHP UMODE2 NICKIP SJOIN SJOIN2 SJ3 NOQUIT TKLEXT ESVID MLOCK";
		UplinkSocket::Message() << "PASS :" << Config->Uplinks[CurrentUplink]->password;
		SendServer(Me);
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message() << "BD + Q H " << nick << " " << Config->ServerName << " " << Anope::CurTime + Config->NSReleaseTimeout << " " << Anope::CurTime << " :Being held for registered user";
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message() << "BD - Q * " << nick << " " << Config->ServerName;
	}

	/* UNSGLINE */
	/*
	 * SVSNLINE - :realname mask
	*/
	void SendSGLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message() << "BR - :" << x->Mask;
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message() << "BD - Z * " << x->GetHost() << " " << Config->OperServ;
	}

	/* SZLINE */
	void SendSZLine(User *, const XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		UplinkSocket::Message() << "BD + Z * " << x->GetHost() << " " << x->By << " " << Anope::CurTime + timeleft << " " << x->Created << " :" << x->GetReason();
	}

	/* SGLINE */
	/*
	 * SVSNLINE + reason_where_is_space :realname mask with spaces
	*/
	void SendSGLine(User *, const XLine *x) anope_override
	{
		Anope::string edited_reason = x->GetReason();
		edited_reason = edited_reason.replace_all_cs(" ", "_");
		UplinkSocket::Message() << "BR + " << edited_reason << " :" << x->Mask;
	}

	/* svsjoin
		parv[0] - sender
		parv[1] - nick to make join
		parv[2] - channel to join
		parv[3] - (optional) channel key(s)
	*/
	/* In older Unreal SVSJOIN and SVSNLINE tokens were mixed so SVSJOIN and SVSNLINE are broken
	   when coming from a none TOKEN'd server
	*/
	void SendSVSJoin(const BotInfo *source, const Anope::string &nick, const Anope::string &chan, const Anope::string &param) anope_override
	{
		if (!param.empty())
			UplinkSocket::Message(source) << "BX " << nick << " " << chan << " :" << param;
		else
			UplinkSocket::Message(source) << "BX " << nick << " :" << chan;
	}

	void SendSWhois(const BotInfo *source, const Anope::string &who, const Anope::string &mask) anope_override
	{
		UplinkSocket::Message(source) << "BA " << who << " :" << mask;
	}

	void SendEOB() anope_override
	{
		UplinkSocket::Message(Me) << "ES";
	}

	bool IsNickValid(const Anope::string &nick) anope_override
	{
		if (nick.equals_ci("ircd") || nick.equals_ci("irc"))
			return false;

		return true;
	}

	bool IsChannelValid(const Anope::string &chan) anope_override
	{
		if (chan.find(':') != Anope::string::npos)
			return false;

		return IRCDProto::IsChannelValid(chan);
	}

	void SendLogin(User *u) anope_override
	{
		if (!u->Account())
			return;

		const BotInfo *ns = findbot(Config->NickServ);
		if (Capab.count("ESVID") > 0)
			ircdproto->SendMode(ns, u, "+d %s", u->Account()->display.c_str());
		else
			ircdproto->SendMode(ns, u, "+d %d", u->timestamp);
	}

	void SendLogout(User *u) anope_override
	{
		const BotInfo *ns = findbot(Config->NickServ);
		ircdproto->SendMode(ns, u, "+d 0");
	}

	void SendChannel(Channel *c) anope_override
	{
		/* Unreal does not support updating a channels TS without actually joining a user,
		 * so we will join and part us now
		 */
		BotInfo *bi = c->ci->WhoSends();
		if (!bi)
			;
		else if (c->FindUser(bi) == NULL)
		{
			bi->Join(c);
			bi->Part(c);
		}
		else
		{
			bi->Part(c);
			bi->Join(c);
		}
	}
};

class UnrealExtBan : public ChannelModeList
{
 public:
	UnrealExtBan(ChannelModeName mName, char modeChar) : ChannelModeList(mName, modeChar) { }

	bool Matches(const User *u, const Entry *e) anope_override
	{
		const Anope::string &mask = e->mask;

		if (mask.find("~c:") == 0)
		{
			Anope::string channel = mask.substr(3);

			ChannelMode *cm = NULL;
			if (channel[0] != '#')
			{
				char modeChar = ModeManager::GetStatusChar(channel[0]);
				channel.erase(channel.begin());
				cm = ModeManager::FindChannelModeByChar(modeChar);
				if (cm != NULL && cm->Type != MODE_STATUS)
					cm = NULL;
			}

			Channel *c = findchan(channel);
			if (c != NULL)
			{
				UserContainer *uc = c->FindUser(u);
				if (uc != NULL)
					if (cm == NULL || uc->Status->HasFlag(cm->Name))
						return true;
			}
		}
		else if (mask.find("~j:") == 0 || mask.find("~n:") == 0 || mask.find("~q:") == 0)
		{
			Anope::string real_mask = mask.substr(3);

			Entry en(this->Name, real_mask);
			if (en.Matches(u))
				return true;
		}
		else if (mask.find("~r:") == 0)
		{
			Anope::string real_mask = mask.substr(3);

			if (Anope::Match(u->realname, real_mask))
				return true;
		}
		else if (mask.find("~R:") == 0)
		{
			if (u->HasMode(UMODE_REGISTERED) && mask.equals_ci(u->nick))
				return true;
		}
		else if (mask.find("~a:") == 0)
		{
			Anope::string real_mask = mask.substr(3);

			if (u->Account() && Anope::Match(u->Account()->display, real_mask))
				return true;
		}
	
		return false;
	}
};

class ChannelModeFlood : public ChannelModeParam
{
 public:
	ChannelModeFlood(char modeChar, bool minusNoArg) : ChannelModeParam(CMODE_FLOOD, modeChar, minusNoArg) { }

	/* Borrowed part of this check from UnrealIRCd */
	bool IsValid(const Anope::string &value) const anope_override
	{
		if (value.empty())
			return false;
		try
		{
			Anope::string rest;
			if (value[0] != ':' && convertTo<unsigned>(value[0] == '*' ? value.substr(1) : value, rest, false) > 0 && rest[0] == ':' && rest.length() > 1 && convertTo<unsigned>(rest.substr(1), rest, false) > 0 && rest.empty())
				return true;
		}
		catch (const ConvertException &) { }
	
		/* '['<number><1 letter>[optional: '#'+1 letter],[next..]']'':'<number> */
		size_t end_bracket = value.find(']', 1);
		if (end_bracket == Anope::string::npos)
			return false;
		Anope::string xbuf = value.substr(0, end_bracket);
		if (value[end_bracket + 1] != ':')
			return false;
		commasepstream args(xbuf.substr(1));
		Anope::string arg;
		while (args.GetToken(arg))
		{
			/* <number><1 letter>[optional: '#'+1 letter] */
			size_t p = 0;
			while (p < arg.length() && isdigit(arg[p]))
				++p;
			if (p == arg.length() || !(arg[p] == 'c' || arg[p] == 'j' || arg[p] == 'k' || arg[p] == 'm' || arg[p] == 'n' || arg[p] == 't'))
				continue; /* continue instead of break for forward compatability. */
			int v = arg.substr(0, p).is_number_only() ? convertTo<int>(arg.substr(0, p)) : 0;
			if (v < 1 || v > 999)
				return false;
		}

		return true;
	}
};

class ChannelModeUnrealSSL : public ChannelMode
{
 public:
	ChannelModeUnrealSSL(ChannelModeName n, char c) : ChannelMode(n, c)
	{
	}

	bool CanSet(User *u) const anope_override
	{
		return false;
	}
};

struct IRCDMessageCapab : CoreIRCDMessageCapab
{
	IRCDMessageCapab() : CoreIRCDMessageCapab("PROTOCTL") { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		for (unsigned i = 0; i < params.size(); ++i)
		{
			Anope::string capab = params[i];

			if (capab.find("CHANMODES") != Anope::string::npos)
			{
				Anope::string modes(capab.begin() + 10, capab.end());
				commasepstream sep(modes);
				Anope::string modebuf;

				sep.GetToken(modebuf);
				for (size_t t = 0, end = modebuf.length(); t < end; ++t)
				{
					switch (modebuf[t])
					{
						case 'b':
							ModeManager::AddChannelMode(new UnrealExtBan(CMODE_BAN, 'b'));
							continue;
						case 'e':
							ModeManager::AddChannelMode(new UnrealExtBan(CMODE_EXCEPT, 'e'));
							continue;
						case 'I':
							ModeManager::AddChannelMode(new UnrealExtBan(CMODE_INVITEOVERRIDE, 'I'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelModeList(CMODE_END, modebuf[t]));
					}
				}

				sep.GetToken(modebuf);
				for (size_t t = 0, end = modebuf.length(); t < end; ++t)
				{
					switch (modebuf[t])
					{
						case 'k':
							ModeManager::AddChannelMode(new ChannelModeKey('k'));
							continue;
						case 'f':
							ModeManager::AddChannelMode(new ChannelModeFlood('f', false));
							continue;
						case 'L':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_REDIRECT, 'L'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, modebuf[t]));
					}
				}

				sep.GetToken(modebuf);
				for (size_t t = 0, end = modebuf.length(); t < end; ++t)
				{
					switch (modebuf[t])
					{
						case 'l':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, 'l', true));
							continue;
						case 'j':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_JOINFLOOD, 'j', true));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, modebuf[t], true));
					}
				}

				sep.GetToken(modebuf);
				for (size_t t = 0, end = modebuf.length(); t < end; ++t)
				{
					switch (modebuf[t])
					{
						case 'p':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, 'p'));
							continue;
						case 's':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, 's'));
							continue;
						case 'm':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, 'm'));
							continue;
						case 'n':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, 'n'));
							continue;
						case 't':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, 't'));
							continue;
						case 'i':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, 'i'));
							continue;
						case 'r':
							ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
							continue;
						case 'R':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_REGISTEREDONLY, 'R'));
							continue;
						case 'c':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCOLOR, 'c'));
							continue;
						case 'O':
							ModeManager::AddChannelMode(new ChannelModeOper('O'));
							continue;
						case 'A':
							ModeManager::AddChannelMode(new ChannelModeAdmin('A'));
							continue;
						case 'Q':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKICK, 'Q'));
							continue;
						case 'K':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKNOCK, 'K'));
							continue;
						case 'V':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOINVITE, 'V'));
							continue;
						case 'C':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOCTCP, 'C'));
							continue;
						case 'u':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_AUDITORIUM, 'u'));
							continue;
						case 'z':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_SSL, 'z'));
							continue;
						case 'N':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NONICK, 'N'));
							continue;
						case 'S':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_STRIPCOLOR, 'S'));
							continue;
						case 'M':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_REGMODERATED, 'M'));
							continue;
						case 'T':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NONOTICE, 'T'));
							continue;
						case 'G':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_FILTER, 'G'));
							continue;
						case 'Z':
							ModeManager::AddChannelMode(new ChannelModeUnrealSSL(CMODE_END, 'Z'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelMode(CMODE_END, modebuf[t]));
					}
				}
			}
		}

		return CoreIRCDMessageCapab::Run(source, params);
	}
};

struct IRCDMessageChgHost : IRCDMessage
{
	IRCDMessageChgHost() : IRCDMessage("CHGHOST", 2) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = finduser(params[0]);
		if (u)
			u->SetDisplayedHost(params[1]);
		return true;
	}
};

struct IRCDMessageChgIdent : IRCDMessage
{
	IRCDMessageChgIdent() : IRCDMessage("CHGIDENT", 2) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = finduser(params[0]);
		if (u)
			u->SetVIdent(params[1]);
		return true;
	}
};

struct IRCDMessageChgName : IRCDMessage
{
	IRCDMessageChgName() : IRCDMessage("CHGNAME", 2) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = finduser(params[0]);
		if (u)
			u->SetRealname(params[1]);
		return true;
	}
};

struct IRCDMessageMode : IRCDMessage
{
	IRCDMessageMode(const Anope::string &mname) : IRCDMessage(mname, 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		bool server_source = source.GetServer() != NULL;
		Anope::string modes = params[1];
		for (unsigned i = 2; i < params.size() - (server_source ? 1 : 0); ++i)
			modes += " " + params[i];

		if (ircdproto->IsChannelValid(params[0]))
		{
			Channel *c = findchan(params[0]);
			time_t ts = Anope::CurTime;
			try
			{
				if (server_source)
					ts = convertTo<time_t>(params[params.size() - 1]);
			}
			catch (const ConvertException &) { }

			if (c)
				c->SetModesInternal(source, modes, ts);
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

/* netinfo
 *  argv[0] = max global count
 *  argv[1] = time of end sync
 *  argv[2] = unreal protocol using (numeric)
 *  argv[3] = cloak-crc (> u2302)
 *  argv[4] = free(**)
 *  argv[5] = free(**)
 *  argv[6] = free(**)
 *  argv[7] = ircnet
 */
struct IRCDMessageNetInfo : IRCDMessage
{
	IRCDMessageNetInfo() : IRCDMessage("NETINFO", 8) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		UplinkSocket::Message() << "AO " << maxusercnt << " " << Anope::CurTime << " " << convertTo<int>(params[2]) << " " << params[3] << " 0 0 0 :" << params[7];
		return true;
	}
};

struct IRCDMessageNick : IRCDMessage
{
	IRCDMessageNick() : IRCDMessage("NICK", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	/*
	** NICK - new
	**	  source  = NULL
	**	  parv[0] = nickname
	**	  parv[1] = hopcount
	**	  parv[2] = timestamp
	**	  parv[3] = username
	**	  parv[4] = hostname
	**	  parv[5] = servername
	**	  parv[6] = servicestamp
	**	  parv[7] = umodes
	**	  parv[8] = virthost, * if none
	**	  parv[9] = ip
	**	  parv[10] = info
	**
	** NICK - change
	**	  source  = oldnick
	**	  parv[0] = new nickname
	**	  parv[1] = hopcount
	*/
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() == 11)
		{
			Anope::string decoded_ip;
			Anope::B64Decode(params[9], decoded_ip);

			Anope::string ip;
			try
			{
				sockaddrs ip_addr;
				ip_addr.ntop(params[9].length() == 8 ? AF_INET : AF_INET6, decoded_ip.c_str());
				ip = ip_addr.addr();
			}
			catch (const SocketException &ex) { }

			Anope::string vhost = params[8];
			if (vhost.equals_cs("*"))
				vhost.clear();

			time_t user_ts = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime;

			Server *s = Server::Find(params[5]);
			if (s == NULL)
			{
				Log(LOG_DEBUG) << "User " << params[0] << " introduced from nonexistant server " << params[5] << "?";
				return true;
			}
		
			User *user = new User(params[0], params[3], params[4], vhost, ip, s, params[10], user_ts, params[7]);

			const NickAlias *na = NULL;

			if (params[6] == "0")
				;
			else if (params[6].is_pos_number_only())
			{
				if (convertTo<time_t>(params[6]) == user->timestamp)
					na = findnick(user->nick);
			}
			else
			{
				na = findnick(params[6]);
			}

			if (na)
			{
				user->Login(na->nc);
				if (!Config->NoNicknameOwnership && na->nc->HasFlag(NI_UNCONFIRMED) == false && nickserv)
					user->SetMode(findbot(Config->NickServ), UMODE_REGISTERED);
			}
			else if (nickserv)
				nickserv->Validate(user);
		}
		else
			source.GetUser()->ChangeNick(params[0]);

		return true;
	}
};

/** This is here because:
 *
 * If we had three servers, A, B & C linked like so: A<->B<->C
 * If Anope is linked to A and B splits from A and then reconnects
 * B introduces itself, introduces C, sends EOS for C, introduces Bs clients
 * introduces Cs clients, sends EOS for B. This causes all of Cs clients to be introduced
 * with their server "not syncing". We now send a PING immediately when receiving a new server
 * and then finish sync once we get a pong back from that server.
 */
struct IRCDMessagePong : IRCDMessage
{
	IRCDMessagePong() : IRCDMessage("PONG", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!source.GetServer()->IsSynced())
			source.GetServer()->Sync(false);
		return true;
	}
};

struct IRCDMessageSDesc : IRCDMessage
{
	IRCDMessageSDesc() : IRCDMessage("SDESC", 1) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetServer()->SetDescription(params[0]);
		return true;
	}
};

struct IRCDMessageSetHost : IRCDMessage
{
	IRCDMessageSetHost() : IRCDMessage("SETHOST", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.GetUser();

		/* When a user sets +x we recieve the new host and then the mode change */
		if (u->HasMode(UMODE_CLOAK))
			u->SetDisplayedHost(params[0]);
		else
			u->SetCloakedHost(params[0]);

		return true;
	}
};

struct IRCDMessageSetIdent : IRCDMessage
{
	IRCDMessageSetIdent() : IRCDMessage("SETIDENT", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.GetUser();
		u->SetVIdent(params[0]);
		return true;
	}
};

struct IRCDMessageSetName : IRCDMessage
{
	IRCDMessageSetName() : IRCDMessage("STENAME", 1) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = finduser(params[0]);
		if (u)
			u->SetRealname(params[1]);
		return true;
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer() : IRCDMessage("SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		unsigned int hops = Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0;

		if (params[1].equals_cs("1"))
		{
			Anope::string vl = myStrGetToken(params[2], ' ', 0);
			Anope::string upnumeric = myStrGetToken(vl, '-', 2);
			Anope::string desc = myStrGetTokenRemainder(params[2], ' ', 1);

			new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, desc);
		}
		else
			new Server(source.GetServer(), params[0], hops, params[2]);

		ircdproto->SendPing(Config->ServerName, params[0]);

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
		if (keep_their_modes && params.size() >= 4)
		{
			Anope::string modes;
			for (unsigned i = 2; i < params.size() - 1; ++i)
				modes += " " + params[i];
			if (!modes.empty())
				modes.erase(modes.begin());
			/* Set the modes internally */
			c->SetModesInternal(source, modes);
		}

		ChannelMode *ban = ModeManager::FindChannelModeByName(CMODE_BAN),
			*except = ModeManager::FindChannelModeByName(CMODE_EXCEPT),
			*invex = ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE);
		spacesepstream sep(params[params.size() - 1]);
		Anope::string buf;
		while (sep.GetToken(buf))
		{
			/* Ban */
			if (buf[0] == '&')
			{
				buf.erase(buf.begin());
				if (keep_their_modes && ban)
					c->SetModeInternal(source, ban, buf);
			}
			/* Except */
			else if (buf[0] == '"')
			{
				buf.erase(buf.begin());
				if (keep_their_modes && except)
					c->SetModeInternal(source, except, buf);
			}
			/* Invex */
			else if (buf[0] == '\'')
			{
				buf.erase(buf.begin());
				if (keep_their_modes && invex)
					c->SetModeInternal(source, invex, buf);
			}
			else
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

	/*
	**	source = sender prefix
	**	parv[0] = channel name
	**	parv[1] = topic nickname
	**	parv[2] = topic time
	**	parv[3] = topic text
	*/
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = findchan(params[0]);
		if (c)
			c->ChangeTopicInternal(params[1], params[3], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime);

		return true;
	}
};


struct IRCDMessageUmode2 : IRCDMessage
{
	IRCDMessageUmode2() : IRCDMessage("UMODE2", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->SetModesInternal("%s", params[0].c_str());
		return true;
	}
};

class ProtoUnreal : public Module
{
	UnrealIRCdProto ircd_proto;

	/* Core message handlers */
	CoreIRCDMessageAway core_message_away;
	CoreIRCDMessageError core_message_error;
	CoreIRCDMessageJoin core_message_join;
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
	IRCDMessageCapab message_capab;
	IRCDMessageChgHost message_chghost;
	IRCDMessageChgIdent message_chgident;
	IRCDMessageChgName message_chgname;
	IRCDMessageMode message_mode, message_svsmode, message_svs2mode;
	IRCDMessageNetInfo message_netinfo;
	IRCDMessageNick message_nick;
	IRCDMessagePong message_pong;
	IRCDMessageSDesc message_sdesc;
	IRCDMessageSetHost message_sethost;
	IRCDMessageSetIdent message_setident;
	IRCDMessageSetName message_setname;
	IRCDMessageServer message_server;
	IRCDMessageSJoin message_sjoin;
	IRCDMessageTopic message_topic;
	IRCDMessageUmode2 message_umode2;

	void AddModes()
	{
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_HALFOP, 'h', '%', 1));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, 'o', '@', 2));
		/* Unreal sends +q as * and +a as ~ */
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT, 'a', '~', 3));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, 'q', '*', 4));

		/* Add user modes */
		ModeManager::AddUserMode(new UserMode(UMODE_SERV_ADMIN, 'A'));
		ModeManager::AddUserMode(new UserMode(UMODE_BOT, 'B'));
		ModeManager::AddUserMode(new UserMode(UMODE_CO_ADMIN, 'C'));
		ModeManager::AddUserMode(new UserMode(UMODE_FILTER, 'G'));
		ModeManager::AddUserMode(new UserMode(UMODE_HIDEOPER, 'H'));
		ModeManager::AddUserMode(new UserMode(UMODE_HIDEIDLE, 'I'));
		ModeManager::AddUserMode(new UserMode(UMODE_NETADMIN, 'N'));
		ModeManager::AddUserMode(new UserMode(UMODE_REGPRIV, 'R'));
		ModeManager::AddUserMode(new UserMode(UMODE_PROTECTED, 'S'));
		ModeManager::AddUserMode(new UserMode(UMODE_NOCTCP, 'T'));
		ModeManager::AddUserMode(new UserMode(UMODE_WEBTV, 'V'));
		ModeManager::AddUserMode(new UserMode(UMODE_WHOIS, 'W'));
		ModeManager::AddUserMode(new UserMode(UMODE_ADMIN, 'a'));
		ModeManager::AddUserMode(new UserMode(UMODE_DEAF, 'd'));
		ModeManager::AddUserMode(new UserMode(UMODE_GLOBOPS, 'g'));
		ModeManager::AddUserMode(new UserMode(UMODE_HELPOP, 'h'));
		ModeManager::AddUserMode(new UserMode(UMODE_INVIS, 'i'));
		ModeManager::AddUserMode(new UserMode(UMODE_OPER, 'o'));
		ModeManager::AddUserMode(new UserMode(UMODE_PRIV, 'p'));
		ModeManager::AddUserMode(new UserMode(UMODE_GOD, 'q'));
		ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, 'r'));
		ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, 's'));
		ModeManager::AddUserMode(new UserMode(UMODE_VHOST, 't'));
		ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, 'w'));
		ModeManager::AddUserMode(new UserMode(UMODE_CLOAK, 'x'));
		ModeManager::AddUserMode(new UserMode(UMODE_SSL, 'z'));
	}

 public:
	ProtoUnreal(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL),
		message_mode("MODE"), message_svsmode("SVSMODE"), message_svs2mode("SVS2MODE")
	{
		this->SetAuthor("Anope");

		this->AddModes();

		Implementation i[] = { I_OnUserNickChange, I_OnChannelCreate, I_OnChanRegistered, I_OnDelChan, I_OnMLock, I_OnUnMLock };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
		ModuleManager::SetPriority(this, PRIORITY_FIRST);
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
		ircdproto->SendLogout(u);
	}

	void OnChannelCreate(Channel *c) anope_override
	{
		if (Config->UseServerSideMLock && Capab.count("MLOCK") > 0 && c->ci)
		{
			Anope::string modes = c->ci->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(c->creation_time) << " " << c->ci->name << " " << modes;
		}
	}

	void OnChanRegistered(ChannelInfo *ci) anope_override
	{
		if (!ci->c || !Config->UseServerSideMLock)
			return;
		Anope::string modes = ci->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");
		UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->name << " " << modes;
	}

	void OnDelChan(ChannelInfo *ci) anope_override
	{
		if (!ci->c || !Config->UseServerSideMLock)
			return;
		UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->name << " :";
	}

	EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (cm && ci->c && (cm->Type == MODE_REGULAR || cm->Type == MODE_PARAM) && Capab.count("MLOCK") > 0 && Config->UseServerSideMLock)
		{
			Anope::string modes = ci->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->ModeChar;
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->name << " " << modes;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChannelInfo *ci, ModeLock *lock) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (cm && ci->c && (cm->Type == MODE_REGULAR || cm->Type == MODE_PARAM) && Capab.count("MLOCK") > 0 && Config->UseServerSideMLock)
		{
			Anope::string modes = ci->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->ModeChar, "");
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->name << " " << modes;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ProtoUnreal)
