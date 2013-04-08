/* inspircd 1.1 beta 6+ functions
 *
 * (C) 2003-2013 Anope Team
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
static bool has_chghostmod = false;
static bool has_chgidentmod = false;
static bool has_hidechansmod = false;

class InspIRCdProto : public IRCDProto
{
 public:
	InspIRCdProto(Module *creator) : IRCDProto(creator, "InspIRCd 1.1")
	{
		DefaultPseudoclientModes = "+I";
		CanSVSNick = true;
		CanSVSJoin = true;
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
			UplinkSocket::Message(OperServ) << "CHGIDENT " << nick << " " << vIdent;
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
			UplinkSocket::Message(OperServ) << "CHGHOST " << nick << " " << vhost;
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
		if (x->GetUser() == "*" && x->GetHost().find_first_not_of("0123456789:.") == Anope::string::npos)
			try
			{
				sockaddrs(x->GetHost());
				IRCD->SendSZLineDel(x);
				return;
			}
			catch (const SocketException &) { }

		UplinkSocket::Message(OperServ) << "GLINE " << x->mask;
	}

	void SendTopic(BotInfo *whosets, Channel *c) anope_override
	{
		UplinkSocket::Message(whosets) << "FTOPIC " << c->name << " " << c->topic_time << " " << c->topic_ts <<" :" << c->topic;
	}

	void SendVhostDel(User *u) anope_override
	{
		if (u->HasMode("CLOAK"))
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
				for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
					if (x->manager->Check(it->second, x))
						this->SendAkill(it->second, x);
				return;
			}

			const XLine *old = x;

			if (old->manager->HasEntry("*@" + u->host))
				return;

			/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
			x = new XLine("*@" + u->host, old->by, old->expires, old->reason, old->id);
			old->manager->AddXLine(x);

			Log(OperServ, "akill") << "AKILL: Added an akill for " << x->mask << " because " << u->GetMask() << "#" << u->realname << " matches " << old->mask;
		}

		/* ZLine if we can instead */
		if (x->GetUser() == "*" && x->GetHost().find_first_not_of("0123456789:.") == Anope::string::npos)
			try
			{
				sockaddrs(x->GetHost());
				IRCD->SendSZLine(u, x);
				return;
			}
			catch (const SocketException &) { }

		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->expires - Anope::CurTime;
		if (timeleft > 172800 || !x->expires)
			timeleft = 172800;
		UplinkSocket::Message(Me) << "ADDLINE G " << x->mask << " " << x->by << " " << Anope::CurTime << " " << timeleft << " :" << x->GetReason();
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
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		UplinkSocket::Message(user) << "JOIN " << c->name << " " << c->creation_time;
		if (status)
		{
			/* First save the channel status incase uc->Status == status */
			ChannelStatus cs = *status;
			/* If the user is internally on the channel with flags, kill them so that
			 * the stacker will allow this.
			 */
			ChanUserContainer *uc = c->FindUser(user);
			if (uc != NULL)
				uc->status.Clear();

			BotInfo *setter = BotInfo::Find(user->nick);
			for (size_t i = 0; i < cs.Modes().length(); ++i)
				c->SetMode(setter, ModeManager::FindChannelModeByChar(cs.Modes()[i]), user->GetUID(), false);
		}
	}

	/* UNSQLINE */
	void SendSQLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(OperServ) << "QLINE " << x->mask;
	}

	/* SQLINE */
	void SendSQLine(User *, const XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->expires - Anope::CurTime;
		if (timeleft > 172800 || !x->expires)
			timeleft = 172800;
		UplinkSocket::Message(Me) << "ADDLINE Q " << x->mask << " " << x->by << " " << Anope::CurTime << " " << timeleft << " :" << x->GetReason();
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
		current_pass = Config->Uplinks[Anope::CurrentUplink]->password;
		SendServer(Me);
		UplinkSocket::Message() << "BURST";
		Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
		UplinkSocket::Message(Me) << "VERSION :Anope-" << Anope::Version() << " " << Me->GetName() << " :" << this->GetProtocolName() << " - (" << (enc ? enc->name : "none") << ") -- " << Anope::VersionBuildString();
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message(OperServ) << "SVSHOLD " << nick << " " << Config->NSReleaseTimeout << "s :Being held for registered user";
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message(OperServ) << "SVSHOLD " << nick;
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(OperServ) << "ZLINE " << x->GetHost();
	}

	/* SZLINE */
	void SendSZLine(User *, const XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->expires - Anope::CurTime;
		if (timeleft > 172800 || !x->expires)
			timeleft = 172800;
		UplinkSocket::Message(Me) << "ADDLINE Z " << x->GetHost() << " " << x->by << " " << Anope::CurTime << " " << timeleft << " :" << x->GetReason();
	}

	void SendSVSJoin(const BotInfo *source, const User *u, const Anope::string &chan, const Anope::string &) anope_override
	{
		UplinkSocket::Message(source) << "SVSJOIN " << u->GetUID() << " " << chan;
	}

	void SendSVSPart(const BotInfo *source, const User *u, const Anope::string &chan, const Anope::string &param) anope_override
	{
		if (!param.empty())
			UplinkSocket::Message(source) << "SVSPART " << u->GetUID() << " " << chan << " :" << param;
		else
			UplinkSocket::Message(source) << "SVSPART " << u->GetUID() << " " << chan;
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
	ChannelModeFlood(char modeChar, bool minusNoArg) : ChannelModeParam("FLOOD", modeChar, minusNoArg) { }

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

struct IRCDMessageCapab : Message::Capab
{
	IRCDMessageCapab(Module *creator) : Message::Capab(creator, "CAPAB") { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params[0].equals_cs("START"))
		{
			/* reset CAPAB */
			has_servicesmod = false;
			has_globopsmod = false;
			has_chghostmod = false;
			has_chgidentmod = false;
			has_hidechansmod = false;
			IRCD->CanSVSHold = false;
		}
		else if (params[0].equals_cs("MODULES") && params.size() > 1)
		{
			if (params[1].find("m_globops.so") != Anope::string::npos)
				has_globopsmod = true;
			if (params[1].find("m_services.so") != Anope::string::npos)
			has_servicesmod = true;
			if (params[1].find("m_svshold.so") != Anope::string::npos)
				IRCD->CanSVSHold = true;
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
								ModeManager::AddChannelMode(new ChannelModeList("BAN", 'b'));
								continue;
							case 'e':
								ModeManager::AddChannelMode(new ChannelModeList("EXCEPT", 'e'));
								continue;
							case 'I':
								ModeManager::AddChannelMode(new ChannelModeList("INVITEOVERRIDE", 'I'));
								continue;
							default:
								ModeManager::AddChannelMode(new ChannelModeList("", modebuf[t]));
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
								ModeManager::AddChannelMode(new ChannelModeParam("", modebuf[t]));
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
								ModeManager::AddChannelMode(new ChannelModeParam("LIMIT", 'l', true));
								continue;
							case 'L':
								ModeManager::AddChannelMode(new ChannelModeParam("REDIRECT", 'L', true));
								continue;
							default:
								ModeManager::AddChannelMode(new ChannelModeParam("", modebuf[t], true));
						}
					}

					sep.GetToken(modebuf);
					for (size_t t = 0, end = modebuf.length(); t < end; ++t)
					{
						switch (modebuf[t])
						{
							case 'i':
								ModeManager::AddChannelMode(new ChannelMode("INVITE", 'i'));
								continue;
							case 'm':
								ModeManager::AddChannelMode(new ChannelMode("MODERATED", 'm'));
								continue;
							case 'n':
								ModeManager::AddChannelMode(new ChannelMode("NOEXTERNAL", 'n'));
								continue;
							case 'p':
								ModeManager::AddChannelMode(new ChannelMode("PRIVATE", 'p'));
								continue;
							case 's':
								ModeManager::AddChannelMode(new ChannelMode("SECRET", 's'));
								continue;
							case 't':
								ModeManager::AddChannelMode(new ChannelMode("TOPIC", 't'));
								continue;
							case 'r':
								ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
								continue;
							case 'c':
								ModeManager::AddChannelMode(new ChannelMode("BLOCKCOLOR", 'c'));
								continue;
							case 'u':
								ModeManager::AddChannelMode(new ChannelMode("AUDITORIUM", 'u'));
								continue;
							case 'z':
								ModeManager::AddChannelMode(new ChannelMode("SSL", 'z'));
								continue;
							case 'A':
								ModeManager::AddChannelMode(new ChannelMode("ALLINVITE", 'A'));
								continue;
							case 'C':
								ModeManager::AddChannelMode(new ChannelMode("NOCTCP", 'C'));
								continue;
							case 'G':
								ModeManager::AddChannelMode(new ChannelMode("FILTER", 'G'));
								continue;
							case 'K':
								ModeManager::AddChannelMode(new ChannelMode("NOKNOCK", 'K'));
								continue;
							case 'N':
								ModeManager::AddChannelMode(new ChannelMode("NONICK", 'N'));
								continue;
							case 'O':
								ModeManager::AddChannelMode(new ChannelModeOper('O'));
								continue;
							case 'Q':
								ModeManager::AddChannelMode(new ChannelMode("NOKICK", 'Q'));
								continue;
							case 'R':
								ModeManager::AddChannelMode(new ChannelMode("REGISTEREDONLY", 'R'));
								continue;
							case 'S':
								ModeManager::AddChannelMode(new ChannelMode("STRIPCOLOR", 'S'));
								continue;
							case 'V':
								ModeManager::AddChannelMode(new ChannelMode("NOINVITE", 'V'));
								continue;
							default:
								ModeManager::AddChannelMode(new ChannelMode("", modebuf[t]));
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
								ModeManager::AddChannelMode(new ChannelModeStatus("OWNER", 'q', '~', level--));
								continue;
							case 'a':
								ModeManager::AddChannelMode(new ChannelModeStatus("PROTECT", 'a', '&', level--));
								continue;
							case 'o':
								ModeManager::AddChannelMode(new ChannelModeStatus("OP", 'o', '@', level--));
								continue;
							case 'h':
								ModeManager::AddChannelMode(new ChannelModeStatus("HALFOP", 'h', '%', level--));
								continue;
							case 'v':
								ModeManager::AddChannelMode(new ChannelModeStatus("VOICE", 'v', '+', level--));
								continue;
							default:
								ModeManager::AddChannelMode(new ChannelModeStatus("", modes[t], chars[t], level--));
						}
					}

					ModeManager::RebuildStatusModes();
				}
				else if (capab.find("MAXMODES=") != Anope::string::npos)
				{
					Anope::string maxmodes(capab.begin() + 9, capab.end());
					IRCD->MaxModes = maxmodes.is_pos_number_only() ? convertTo<unsigned>(maxmodes) : 3;
				}
			}
		}
		else if (params[0].equals_cs("END"))
		{
			if (!has_globopsmod)
			{
				UplinkSocket::Message() << "ERROR :m_globops is not loaded. This is required by Anope";
				Anope::QuitReason = "ERROR: Remote server does not have the m_globops module loaded, and this is required.";
				Anope::Quitting = true;
				return;
			}
			if (!has_servicesmod)
			{
				UplinkSocket::Message() << "ERROR :m_services is not loaded. This is required by Anope";
				Anope::QuitReason = "ERROR: Remote server does not have the m_services module loaded, and this is required.";
				Anope::Quitting = true;
				return;
			}
			if (!has_hidechansmod)
			{
				UplinkSocket::Message() << "ERROR :m_hidechans.so is not loaded. This is required by Anope";
				Anope::QuitReason = "ERROR: Remote server deos not have the m_hidechans module loaded, and this is required.";
				Anope::Quitting = true;
				return;
			}
			if (!IRCD->CanSVSHold)
				Log() << "SVSHOLD missing, Usage disabled until module is loaded.";
			if (!has_chghostmod)
				Log() << "CHGHOST missing, Usage disabled until module is loaded.";
			if (!has_chgidentmod)
				Log() << "CHGIDENT missing, Usage disabled until module is loaded.";
		}

		Message::Capab::Run(source, params);
	}
};

struct IRCDMessageChgIdent : IRCDMessage
{
	IRCDMessageChgIdent(Module *creator, const Anope::string &n) : IRCDMessage(creator, n, 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = User::Find(params[0]);
		if (!u)
		{
			Log(LOG_DEBUG) << "CHGIDENT for nonexistent user " << params[0];
			return;
		}

		u->SetIdent(params[1]);
	}
};

struct IRCDMessageChgName : IRCDMessage
{
	IRCDMessageChgName(Module *creator, const Anope::string &n) : IRCDMessage(creator, n, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->SetRealname(params[0]);
	}
};

struct IRCDMessageEndBurst : IRCDMessage
{
	IRCDMessageEndBurst(Module *creator) : IRCDMessage(creator, "ENDBURST", 0) { }

	void Run(MessageSource &, const std::vector<Anope::string> &params) anope_override
	{
		Me->GetLinks().front()->Sync(true);
	}
};

struct IRCDMessageFHost : IRCDMessage
{
	IRCDMessageFHost(Module *creator, const Anope::string &n) : IRCDMessage(creator, n, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->SetDisplayedHost(params[0]);
	}
};

struct IRCDMessageFJoin : IRCDMessage
{
	IRCDMessageFJoin(Module *creator) : IRCDMessage(creator, "FJOIN", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		std::list<Message::Join::SJoinUser> users;

		spacesepstream sep(params[params.size() - 1]);
		Anope::string buf;

		while (sep.GetToken(buf))
		{
			Message::Join::SJoinUser sju;

			/* Loop through prefixes */
			for (char ch; (ch = ModeManager::GetStatusChar(buf[0]));)
			{
				ChannelMode *cm = ModeManager::FindChannelModeByChar(ch);
				buf.erase(buf.begin());

				if (!cm)
				{
					Log() << "Received unknown mode prefix " << ch << " in FJOIN string";
					continue;
				}

				sju.first.AddMode(cm->mchar);
			}
			/* Erase the , */
			buf.erase(buf.begin());

			sju.second = User::Find(buf);
			if (!sju.second)
			{
				Log(LOG_DEBUG) << "FJOIN for nonexistant user " << buf << " on " << params[0];
				continue;
			}

		}

		time_t ts = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime;
		Message::Join::SJoin(source, params[0], ts, "", users);
	}
};

struct IRCDMessageFMode : IRCDMessage
{
	IRCDMessageFMode(Module *creator) : IRCDMessage(creator, "FMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* :source FMODE #test 12345678 +nto foo */

		Channel *c = Channel::Find(params[0]);
		if (!c)
			return;
		time_t ts = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : 0;

		/* TS's are equal now, so we can proceed with parsing */
		// For fun, modes sometimes get sent without a mode prefix
		Anope::string modes = "+" + params[2];
		for (unsigned n = 3; n < params.size(); ++n)
			modes += " " + params[n];

		c->SetModesInternal(source, modes, ts);
	}
};

struct IRCDMessageFTopic : IRCDMessage
{
	IRCDMessageFTopic(Module *creator) : IRCDMessage(creator, "FTOPIC", 4) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = Channel::Find(params[0]);
		if (c)
			c->ChangeTopicInternal(params[2], params[3], Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime);
	}
};

struct IRCDMessageIdle : IRCDMessage
{
	IRCDMessageIdle(Module *creator) : IRCDMessage(creator, "IDLE", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const BotInfo *bi = BotInfo::Find(params[0]);
		UplinkSocket::Message(bi) << "IDLE " << source.GetSource() << " " << Anope::StartTime << " " << (bi ? Anope::CurTime - bi->lastmsg : 0);
	}
};

struct IRCDMessageMode : IRCDMessage
{
	IRCDMessageMode(Module *creator) : IRCDMessage(creator, "MODE", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (IRCD->IsChannelValid(params[0]))
		{
			Channel *c = Channel::Find(params[0]);
			time_t ts;

			try
			{
				ts = convertTo<time_t>(params[2]);
			}
			catch (const ConvertException &)
			{
				ts = 0;
			}

			if (c)
				c->SetModesInternal(source, params[1], ts);
		}
		else
		{
			User *u = User::Find(params[0]);
			if (u)
				u->SetModesInternal("%s", params[1].c_str());
		}
	}
};

struct IRCDMessageNick : IRCDMessage
{
	ServiceReference<NickServService> NSService;

	IRCDMessageNick(Module *creator) : IRCDMessage(creator, "NICK", 1), NSService("NickServService", "NickServ") { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() == 8 && source.GetServer())
		{
			time_t ts = Anope::string(params[0]).is_pos_number_only() ? convertTo<time_t>(params[0]) : Anope::CurTime;

			User *user = new User(params[1], params[4], params[2], params[3], params[6], source.GetServer(), params[7], ts, params[5]);
			if (NSService)
			{
				NickAlias *na = NickAlias::Find(user->nick);
				Anope::string *svidbuf = na ? na->nc->GetExt<ExtensibleItemClass<Anope::string> *>("authenticationtoken") : NULL;
				if (na && svidbuf && *svidbuf == params[0])
					NSService->Login(user, na);
			}
		}
		else if (params.size() == 1 && source.GetUser())
			source.GetUser()->ChangeNick(params[0]);
	}
};

struct IRCDMessageOperType : IRCDMessage
{
	IRCDMessageOperType(Module *creator) : IRCDMessage(creator, "OPERTYPE", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* opertype is equivalent to mode +o because servers
		   dont do this directly */
		User *u = source.GetUser();
		if (!u->HasMode("OPER"))
			u->SetModesInternal("+o");
	}
};

struct IRCDMessageRSQuit : IRCDMessage
{
	IRCDMessageRSQuit(Module *creator) : IRCDMessage(creator, "RSQUIT", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.empty() || params.size() > 3)
			return;

		Server *s;
		/* Horrible workaround to an insp bug (#) in how RSQUITs are sent - mark */
		if (params.size() > 1 && Config->ServerName.equals_cs(params[0]))
			s = Server::Find(params[1]);
		else
			s = Server::Find(params[0]);

		source.GetServer()->Delete(source.GetServer()->GetName() + " " + (s ? s->GetName() : "<unknown>"));
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 4) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		unsigned int hops = Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[2]);
	}
};

class ProtoInspIRCd : public Module
{
	InspIRCdProto ircd_proto;

	/* Core message handlers */
	Message::Away message_away;
	Message::Error message_error;
	Message::Join message_join;
	Message::Kick message_kick;
	Message::Kill message_kill;
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
		ModeManager::AddUserMode(new UserMode("CALLERID", 'g'));
		ModeManager::AddUserMode(new UserMode("HELPOP", 'h'));
		ModeManager::AddUserMode(new UserMode("INVIS", 'i'));
		ModeManager::AddUserMode(new UserMode("OPER", 'o'));
		ModeManager::AddUserMode(new UserMode("REGISTERED", 'r'));
		ModeManager::AddUserMode(new UserMode("WALLOPS", 'w'));
		ModeManager::AddUserMode(new UserMode("CLOAK", 'x'));
	}

 public:
	ProtoInspIRCd(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL),
		ircd_proto(this),
		message_away(this), message_error(this), message_join(this), message_kick(this), message_kill(this),
		message_motd(this), message_part(this), message_ping(this), message_privmsg(this), message_quit(this),
		message_squit(this), message_stats(this), message_time(this), message_topic(this), message_version(this),

		message_capab(this), message_chgident(this, "CHGIDENT"), message_setident(this, "SETIDENT"),
		message_chgname(this, "CHGNAME"), message_setname(this, "SETNAME"), message_endburst(this),
		message_fhost(this, "FHOST"), message_sethost(this, "SETHOST"), message_fjoin(this),
		message_fmode(this), message_ftopic(this), message_idle(this), message_mode(this),
		message_nick(this), message_opertype(this), message_rsquit(this), message_server(this)
	{
		this->SetAuthor("Anope");

		Servers::Capab.insert("NOQUIT");

		this->AddModes();

		ModuleManager::Attach(I_OnUserNickChange, this);
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName("REGISTERED"));
	}
};

MODULE_INIT(ProtoInspIRCd)
