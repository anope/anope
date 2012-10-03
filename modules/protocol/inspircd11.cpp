/* inspircd 1.1 beta 6+ functions
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

static bool has_servicesmod = false;
static bool has_globopsmod = false;
static bool has_svsholdmod = false;
static bool has_chghostmod = false;
static bool has_chgidentmod = false;
static bool has_hidechansmod = false;

class InspIRCdProto : public IRCDProto
{
 public:
	InspIRCdProto() : IRCDProto("InspIRCd 1.1")
	{
		DefaultPseudoclientModes = "+I";
		CanSVSNick = true;
		CanSetVHost = true;
		CanSetVIdent = true;
		CanSNLine = true;
		CanSQLine = true;
		CanSZLine = true;
		CanSVSHold = true;
		CanSVSO = true;
		MaxModes = 20;
	}
 private:

	Anope::string current_pass;

	void inspircd_cmd_chgident(const Anope::string &nick, const Anope::string &vIdent)
	{
		if (has_chgidentmod)
		{
			if (nick.empty() || vIdent.empty())
				return;
			UplinkSocket::Message(findbot(Config->OperServ)) << "CHGIDENT " << nick << " " << vIdent;
		}
		else
			Log() << "CHGIDENT not loaded!";
	}

	void inspircd_cmd_chghost(const Anope::string &nick, const Anope::string &vhost)
	{
		if (has_chghostmod)
		{
			if (nick.empty() || vhost.empty())
				return;
			UplinkSocket::Message(findbot(Config->OperServ)) << "CHGHOST " << nick << " " << vhost;
		}
		else
			Log() << "CHGHOST not loaded!";
	}

	void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "NOTICE $" << dest->GetName() << " :" << msg;
	}

	void SendGlobalPrivmsg(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "PRIVMSG $" << dest->GetName() << " :" << msg;
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

		UplinkSocket::Message(findbot(Config->OperServ)) << "GLINE " << x->Mask;
	}

	void SendTopic(BotInfo *bi, Channel *c, const Anope::string &topic, const Anope::string &setter, time_t &ts) anope_override
	{
		UplinkSocket::Message(bi) << "FTOPIC " << c->name << " " << ts << " " << setter << " :" << topic;
	}

	void SendVhostDel(User *u) anope_override
	{
		if (u->HasMode(UMODE_CLOAK))
			inspircd_cmd_chghost(u->nick, u->chost);
		else
			inspircd_cmd_chghost(u->nick, u->host);

		if (has_chgidentmod && u->GetIdent() != u->GetVIdent())
			inspircd_cmd_chgident(u->nick, u->GetIdent());
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
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		UplinkSocket::Message(Me) << "ADDLINE G " << x->Mask << " " << x->By << " " << Anope::CurTime << " " << timeleft << " :" << x->GetReason();
	}

	void SendSVSKillInternal(const BotInfo *source, User *user, const Anope::string &buf) anope_override
	{
		if (source)
		{
			UplinkSocket::Message(source) << "KILL " << user->nick << " :" << buf;
			user->KillInternal(source->nick, buf);
		}
		else
		{
			UplinkSocket::Message(Me) << "KILL " << user->nick << " :" << buf;
			user->KillInternal(Me->GetName(), buf);
		}
	}

	void SendNumericInternal(int numeric, const Anope::string &dest, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message() << "PUSH " << dest << " ::" << Me->GetName() << " " << numeric << " " << dest << " " << buf;
	}

	void SendModeInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf) anope_override
	{
		if (source)
			UplinkSocket::Message(source) << "FMODE " << dest->name << " " << dest->creation_time << " " << buf;
		else
			UplinkSocket::Message(Me) << "FMODE " << dest->name << " " << dest->creation_time << " " << buf;
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf) anope_override
	{
		if (bi)
			UplinkSocket::Message(bi) << "MODE " << u->nick << " " << buf;
		else
			UplinkSocket::Message(Me) << "MODE " << u->nick << " " << buf;
	}

	void SendClientIntroduction(const User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "NICK " << u->timestamp << " " << u->nick << " " << u->host << " " << u->host << " " << u->GetIdent() << " " << modes << " 0.0.0.0 :" << u->realname;
		UplinkSocket::Message(u) << "OPERTYPE Service";
	}

	/* SERVER services-dev.chatspike.net password 0 :Description here */
	void SendServer(const Server *server) anope_override
	{
		UplinkSocket::Message(Me) << "SERVER " << server->GetName() << " " << current_pass << " " << server->GetHops() << " :" << server->GetDescription();
	}

	/* JOIN */
	void SendJoin(const User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		UplinkSocket::Message(user) << "JOIN " << c->name << " " << c->creation_time;
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
		UplinkSocket::Message(findbot(Config->OperServ)) << "QLINE " << x->Mask;
	}

	/* SQLINE */
	void SendSQLine(User *, const XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		UplinkSocket::Message(Me) << "ADDLINE Q " << x->Mask << " " << x->By << " " << Anope::CurTime << " " << timeleft << " :" << x->GetReason();
	}

	/* Functions that use serval cmd functions */

	void SendVhost(User *u, const Anope::string &vIdent, const Anope::string &vhost) anope_override
	{
		if (!vIdent.empty())
			inspircd_cmd_chgident(u->nick, vIdent);
		if (!vhost.empty())
			inspircd_cmd_chghost(u->nick, vhost);
	}

	void SendConnect() anope_override
	{
		current_pass = Config->Uplinks[CurrentUplink]->password;
		SendServer(Me);
		UplinkSocket::Message() << "BURST";
		Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
		UplinkSocket::Message(Me) << "VERSION :Anope-" << Anope::Version() << " " << Me->GetName() << " :" << this->GetProtocolName() << " - (" << (enc ? enc->name : "unknown") << ") -- " << Anope::VersionBuildString();
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message(findbot(Config->OperServ)) << "SVSHOLD " << nick << " " << Config->NSReleaseTimeout << "s :Being held for registered user";
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message(findbot(Config->OperServ)) << "SVSHOLD " << nick;
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(findbot(Config->OperServ)) << "ZLINE " << x->GetHost();
	}

	/* SZLINE */
	void SendSZLine(User *, const XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		UplinkSocket::Message(Me) << "ADDLINE Z " << x->GetHost() << " " << x->By << " " << Anope::CurTime << " " << timeleft << " :" << x->GetReason();
	}

	void SendSVSJoin(const BotInfo *source, const Anope::string &nick, const Anope::string &chan, const Anope::string &) anope_override
	{
		UplinkSocket::Message(source) << "SVSJOIN " << nick << " " << chan;
	}

	void SendBOB() anope_override
	{
		UplinkSocket::Message() << "BURST " << Anope::CurTime;
	}

	void SendEOB() anope_override
	{
		UplinkSocket::Message() << "ENDBURST";
	}

	void SendLogin(User *u) anope_override
	{
		if (!u->Account())
			return;

		Anope::string svidbuf = stringify(u->timestamp);
		u->Account()->Extend("authenticationtoken", new ExtensibleItemClass<Anope::string>(svidbuf));
	}

	void SendLogout(User *u) anope_override
	{
	}
};

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

struct IRCDMessageCapab : IRCDMessage
{
	IRCDMessageCapab() : IRCDMessage("CAPAB", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &, const std::vector<Anope::string> &params) anope_override
	{
		if (params[0].equals_cs("START"))
		{
			/* reset CAPAB */
			has_servicesmod = false;
			has_globopsmod = false;
			has_svsholdmod = false;
			has_chghostmod = false;
			has_chgidentmod = false;
			has_hidechansmod = false;
		}
		else if (params[0].equals_cs("MODULES") && params.size() > 1)
		{
			if (params[1].find("m_globops.so") != Anope::string::npos)
				has_globopsmod = true;
			if (params[1].find("m_services.so") != Anope::string::npos)
			has_servicesmod = true;
			if (params[1].find("m_svshold.so") != Anope::string::npos)
				has_svsholdmod = true;
			if (params[1].find("m_chghost.so") != Anope::string::npos)
				has_chghostmod = true;
			if (params[1].find("m_chgident.so") != Anope::string::npos)
				has_chgidentmod = true;
			if (params[1].find("m_hidechans.so") != Anope::string::npos)
				has_hidechansmod = true;
		}
		else if (params[0].equals_cs("CAPABILITIES") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string capab;
			while (ssep.GetToken(capab))
			{
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
								ModeManager::AddChannelMode(new ChannelModeList(CMODE_BAN, 'b'));
								continue;
							case 'e':
								ModeManager::AddChannelMode(new ChannelModeList(CMODE_EXCEPT, 'e'));
								continue;
							case 'I':
								ModeManager::AddChannelMode(new ChannelModeList(CMODE_INVITEOVERRIDE, 'I'));
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
							default:
								ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, modebuf[t]));
						}
					}

					sep.GetToken(modebuf);
					for (size_t t = 0, end = modebuf.length(); t < end; ++t)
					{
						switch (modebuf[t])
						{
							case 'f':
								ModeManager::AddChannelMode(new ChannelModeFlood('f', false));
								continue;
							case 'l':
								ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, 'l', true));
								continue;
							case 'L':
								ModeManager::AddChannelMode(new ChannelModeParam(CMODE_REDIRECT, 'L', true));
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
							case 'i':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, 'i'));
								continue;
							case 'm':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, 'm'));
								continue;
							case 'n':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, 'n'));
								continue;
							case 'p':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, 'p'));
								continue;
							case 's':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, 's'));
								continue;
							case 't':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, 't'));
								continue;
							case 'r':
								ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
								continue;
							case 'c':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCOLOR, 'c'));
								continue;
							case 'u':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_AUDITORIUM, 'u'));
								continue;
							case 'z':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_SSL, 'z'));
								continue;
							case 'A':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_ALLINVITE, 'A'));
								continue;
							case 'C':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_NOCTCP, 'C'));
								continue;
							case 'G':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_FILTER, 'G'));
								continue;
							case 'K':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKNOCK, 'K'));
								continue;
							case 'N':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_NONICK, 'N'));
								continue;
							case 'O':
								ModeManager::AddChannelMode(new ChannelModeOper('O'));
								continue;
							case 'Q':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKICK, 'Q'));
								continue;
							case 'R':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_REGISTEREDONLY, 'R'));
								continue;
							case 'S':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_STRIPCOLOR, 'S'));
								continue;
							case 'V':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_NOINVITE, 'V'));
								continue;
							default:
								ModeManager::AddChannelMode(new ChannelMode(CMODE_END, modebuf[t]));
						}
					}
				}
				else if (capab.find("PREFIX=(") != Anope::string::npos)
				{
					Anope::string modes(capab.begin() + 8, capab.begin() + capab.find(')'));
					Anope::string chars(capab.begin() + capab.find(')') + 1, capab.end());
					unsigned short level = modes.length() - 1;

					for (size_t t = 0, end = modes.length(); t < end; ++t)
					{
						switch (modes[t])
						{
							case 'q':
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, 'q', '~', level--));
								continue;
							case 'a':
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT, 'a', '&', level--));
								continue;
							case 'o':
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, 'o', '@', level--));
								continue;
							case 'h':
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_HALFOP, 'h', '%', level--));
								continue;
							case 'v':
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, 'v', '+', level--));
								continue;
							default:
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_END, modes[t], chars[t], level--));
						}
					}
				}
				else if (capab.find("MAXMODES=") != Anope::string::npos)
				{
					Anope::string maxmodes(capab.begin() + 9, capab.end());
					ircdproto->MaxModes = maxmodes.is_pos_number_only() ? convertTo<unsigned>(maxmodes) : 3;
				}
			}
		}
		else if (params[0].equals_cs("END"))
		{
			if (!has_globopsmod)
			{
				UplinkSocket::Message() << "ERROR :m_globops is not loaded. This is required by Anope";
				quitmsg = "ERROR: Remote server does not have the m_globops module loaded, and this is required.";
				quitting = true;
				return false;
			}
			if (!has_servicesmod)
			{
				UplinkSocket::Message() << "ERROR :m_services is not loaded. This is required by Anope";
				quitmsg = "ERROR: Remote server does not have the m_services module loaded, and this is required.";
				quitting = true;
				return false;
			}
			if (!has_hidechansmod)
			{
				UplinkSocket::Message() << "ERROR :m_hidechans.so is not loaded. This is required by Anope";
				quitmsg = "ERROR: Remote server deos not have the m_hidechans module loaded, and this is required.";
				quitting = true;
				return false;
			}
			if (!has_svsholdmod)
				Log() << "SVSHOLD missing, Usage disabled until module is loaded.";
			if (!has_chghostmod)
				Log() << "CHGHOST missing, Usage disabled until module is loaded.";
			if (!has_chgidentmod)
				Log() << "CHGIDENT missing, Usage disabled until module is loaded.";
			ircdproto->CanSVSHold = has_svsholdmod;
		}

		return true;
	}
};

struct IRCDMessageChgIdent : IRCDMessage
{
	IRCDMessageChgIdent(const Anope::string &n) : IRCDMessage(n, 2) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = finduser(params[0]);
		if (!u)
		{
			Log(LOG_DEBUG) << "CHGIDENT for nonexistent user " << params[0];
			return true;
		}

		u->SetIdent(params[1]);
		return true;
	}
};

struct IRCDMessageChgName : IRCDMessage
{
	IRCDMessageChgName(const Anope::string &n) : IRCDMessage(n, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->SetRealname(params[0]);
		return true;
	}
};

struct IRCDMessageEndBurst : IRCDMessage
{
	IRCDMessageEndBurst() : IRCDMessage("ENDBURST", 0) { }

	bool Run(MessageSource &, const std::vector<Anope::string> &params) anope_override
	{
		Me->GetLinks().front()->Sync(true);
		return true;
	}
};

struct IRCDMessageFHost : IRCDMessage
{
	IRCDMessageFHost(const Anope::string &n) : IRCDMessage(n, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->SetDisplayedHost(params[0]);
		return true;
	}
};

struct IRCDMessageFJoin : IRCDMessage
{
	IRCDMessageFJoin() : IRCDMessage("FJOIN", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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

		spacesepstream sep(params[params.size() - 1]);
		Anope::string buf;
		while (sep.GetToken(buf))
		{
			std::list<ChannelMode *> Status;
			char ch;

			/* Loop through prefixes */
			while ((ch = ModeManager::GetStatusChar(buf[0])))
			{
				ChannelMode *cm = ModeManager::FindChannelModeByChar(ch);

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
			/* Erase the , */
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

struct IRCDMessageFMode : IRCDMessage
{
	IRCDMessageFMode() : IRCDMessage("FMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* :source FMODE #test 12345678 +nto foo */

		Channel *c = findchan(params[0]);
		if (!c)
			return true;
		time_t ts = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : 0;

		/* TS's are equal now, so we can proceed with parsing */
		// For fun, modes sometimes get sent without a mode prefix
		Anope::string modes = "+" + params[2];
		for (unsigned n = 3; n < params.size(); ++n)
			modes += " " + params[n];

		c->SetModesInternal(source, modes, ts);
		return true;
	}
};

struct IRCDMessageFTopic : IRCDMessage
{
	IRCDMessageFTopic() : IRCDMessage("FTOPIC", 4) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = findchan(params[0]);
		if (c)
			c->ChangeTopicInternal(params[2], params[3], Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime);

		return true;
	}
};

struct IRCDMessageIdle : IRCDMessage
{
	IRCDMessageIdle() : IRCDMessage("IDLE", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const BotInfo *bi = findbot(params[0]);
		UplinkSocket::Message(bi) << "IDLE " << source.GetSource() << " " << start_time << " " << (bi ? Anope::CurTime - bi->lastmsg : 0);
		return true;
	}
};

struct IRCDMessageMode : IRCDMessage
{
	IRCDMessageMode() : IRCDMessage("MODE", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (ircdproto->IsChannelValid(params[0]))
		{
			Channel *c = findchan(params[0]);
			time_t ts;

			try
			{
				ts = convertTo<time_t>(params[2]);
			}
			catch (const ConvertException &)
			{
				ts = Anope::CurTime;
			}

			if (c)
				c->SetModesInternal(source, params[1], ts);
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
	IRCDMessageNick() : IRCDMessage("NICK", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() == 8 && source.GetServer())
		{
			time_t ts = Anope::string(params[0]).is_pos_number_only() ? convertTo<time_t>(params[0]) : Anope::CurTime;

			User *user = new User(params[1], params[4], params[2], params[3], params[6], source.GetServer(), params[7], ts, params[5]);
			if (nickserv)
			{
				NickAlias *na = findnick(user->nick);
				Anope::string *svidbuf = na ? na->nc->GetExt<ExtensibleItemClass<Anope::string> *>("authenticationtoken") : NULL;
				if (na && svidbuf && *svidbuf == params[0])
				{
					NickCore *nc = na->nc;
					user->Login(nc);
					if (!Config->NoNicknameOwnership && na->nc->HasFlag(NI_UNCONFIRMED) == false)
						user->SetMode(findbot(Config->NickServ), UMODE_REGISTERED);
				}
				else if (nickserv)
					nickserv->Validate(user);
			}
		}
		else if (params.size() == 1 && source.GetUser())
			source.GetUser()->ChangeNick(params[0]);

		return true;
	}
};

struct IRCDMessageOperType : IRCDMessage
{
	IRCDMessageOperType() : IRCDMessage("OPERTYPE", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* opertype is equivalent to mode +o because servers
		   dont do this directly */
		User *u = source.GetUser();
		if (!u->HasMode(UMODE_OPER))
			u->SetModesInternal("+o");

		return true;
	}
};

struct IRCDMessageRSQuit : IRCDMessage
{
	IRCDMessageRSQuit() : IRCDMessage("RSQUIT", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.empty() || params.size() > 3)
			return true;

		Server *s;
		/* Horrible workaround to an insp bug (#) in how RSQUITs are sent - mark */
		if (params.size() > 1 && Config->ServerName.equals_cs(params[0]))
			s = Server::Find(params[1]);
		else
			s = Server::Find(params[0]);

		source.GetServer()->Delete(source.GetServer()->GetName() + " " + (s ? s->GetName() : "<unknown>"));

		return true;
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer() : IRCDMessage("SERVER", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		unsigned int hops = Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[2]);
		return true;
	}
};

class ProtoInspIRCd : public Module
{
	InspIRCdProto ircd_proto;

	/* Core message handlers */
	CoreIRCDMessageAway core_message_away;
	CoreIRCDMessageCapab core_message_capab;
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

	/* Our message handlers */
	IRCDMessageCapab message_capab;
	IRCDMessageChgIdent message_chgident, message_setident;
	IRCDMessageChgName message_chgname, message_setname;
	IRCDMessageEndBurst message_endburst;
	IRCDMessageFHost message_fhost, message_sethost;
	IRCDMessageFJoin message_fjoin;
	IRCDMessageFMode message_fmode;
	IRCDMessageFTopic message_ftopic;
	IRCDMessageIdle message_idle;
	IRCDMessageMode message_mode;
	IRCDMessageNick message_nick;
	IRCDMessageOperType message_opertype;
	IRCDMessageRSQuit message_rsquit;
	IRCDMessageServer message_server;

	void AddModes()
	{
		ModeManager::AddUserMode(new UserMode(UMODE_CALLERID, 'g'));
		ModeManager::AddUserMode(new UserMode(UMODE_HELPOP, 'h'));
		ModeManager::AddUserMode(new UserMode(UMODE_INVIS, 'i'));
		ModeManager::AddUserMode(new UserMode(UMODE_OPER, 'o'));
		ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, 'r'));
		ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, 'w'));
		ModeManager::AddUserMode(new UserMode(UMODE_CLOAK, 'x'));
	}

 public:
	ProtoInspIRCd(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL),
		message_chgident("CHGIDENT"), message_setident("SETIDENT"),
		message_chgname("CHGNAME"), message_setname("SETNAME"),
		message_fhost("FHOST"), message_sethost("SETHOST")
	{
		this->SetAuthor("Anope");

		Capab.insert("NOQUIT");

		this->AddModes();

		ModuleManager::Attach(I_OnUserNickChange, this);
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
	}
};

MODULE_INIT(ProtoInspIRCd)
