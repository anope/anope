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

struct ExtensibleString : Anope::string, ExtensibleItem
{
	ExtensibleString(const Anope::string &s) : Anope::string(s) { }
};

IRCDVar myIrcd = {
	"InspIRCd 1.1",	/* ircd name */
	 "+I",				/* Modes used by pseudoclients */
	 1,					/* SVSNICK */
	 1,					/* Vhost */
	 1,					/* Supports SNlines */
	 1,					/* Supports SQlines */
	 1,					/* Supports SZlines */
	 1,					/* Join 2 Message */
	 0,					/* Chan SQlines */
	 0,					/* Quit on Kill */
	 1,					/* vidents */
	 1,					/* svshold */
	 0,					/* time stamp on mode */
	 1,					/* O:LINE */
	 1,					/* UMODE */
	 1,					/* No Knock requires +i */
	 0,					/* Can remove User Channel Modes with SVSMODE */
	 0,					/* Sglines are not enforced until user reconnects */
	 0,					/* ts6 */
	 "$",				/* TLD Prefix for Global */
	 20,				/* Max number of modes we can send per line */
	 0					/* IRCd sends a SSL users certificate fingerprint */
};

static bool has_servicesmod = false;
static bool has_globopsmod = false;
static bool has_svsholdmod = false;
static bool has_chghostmod = false;
static bool has_chgidentmod = false;
static bool has_hidechansmod = false;

/* CHGHOST */
void inspircd_cmd_chghost(const Anope::string &nick, const Anope::string &vhost)
{
	const BotInfo *bi = findbot(Config->OperServ);
	if (has_chghostmod)
	{
		if (nick.empty() || vhost.empty())
			return;
		UplinkSocket::Message(bi) << "CHGHOST " << nick << " " << vhost;
	}
	else
	{
		if (bi)
			ircdproto->SendGlobops(bi, "CHGHOST not loaded!");
	}
}

bool event_idle(const Anope::string &source, const std::vector<Anope::string> &params)
{
	const BotInfo *bi = findbot(params[0]);
	UplinkSocket::Message(bi) << "IDLE " << source << " " << start_time << " " << (bi ? Anope::CurTime - bi->lastmsg : 0);
	return true;
}

static Anope::string currentpass;

/* PASS */
void inspircd_cmd_pass(const Anope::string &pass)
{
	currentpass = pass;
}

class InspIRCdProto : public IRCDProto
{
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

	void SendTopic(BotInfo *whosets, Channel *c) anope_override
	{
		UplinkSocket::Message(whosets) << "FTOPIC " << c->name << " " << c->topic_time << " " << c->topic_setter <<" :" << c->topic;
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

	void SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf) anope_override
	{
		if (source)
			UplinkSocket::Message(source) << "KILL " << user->nick << " :" << buf;
		else
			UplinkSocket::Message(Me) << "KILL " << user->nick << " :" << buf;
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

	void SendKickInternal(const BotInfo *source, const Channel *chan, const User *user, const Anope::string &buf) anope_override
	{
		if (!buf.empty())
			UplinkSocket::Message(source) << "KICK " << chan->name << " " << user->nick << " :" << buf;
		else
			UplinkSocket::Message(source) << "KICK " << chan->name << " " << user->nick << " :" << user->nick;
	}

	/* SERVER services-dev.chatspike.net password 0 :Description here */
	void SendServer(const Server *server) anope_override
	{
		UplinkSocket::Message(Me) << "SERVER " << server->GetName() << " " << currentpass << " " << server->GetHops() << " :" << server->GetDescription();
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
		inspircd_cmd_pass(Config->Uplinks[CurrentUplink]->password);
		SendServer(Me);
		UplinkSocket::Message() << "BURST";
		Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
		UplinkSocket::Message(Me) << "VERSION :Anope-" << Anope::Version() << " " << Me->GetName() << " :" << ircd->name << " - (" << (enc ? enc->name : "unknown") << ") -- " << Anope::VersionBuildString();
	}

	/* CHGIDENT */
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
		u->Account()->Extend("authenticationtoken", new ExtensibleString(svidbuf));
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

class InspircdIRCdMessage : public IRCdMessage
{
 public:
	bool OnMode(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() < 2)
			return true;

		if (params[0][0] == '#' || params[0][0] == '&')
			do_cmode(source, params[0], params[1], params[2]);
		else
			do_umode(params[0], params[1]);

		return true;
	}

	bool OnNick(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() == 8)
		{
			time_t ts = Anope::string(params[0]).is_pos_number_only() ? convertTo<time_t>(params[0]) : 0;

			User *user = do_nick("", params[1], params[4], params[2], source, params[7], ts, params[6], params[3], "", params[5]);
			if (user && nickserv)
			{
				user->SetCloakedHost(params[3]);

				NickAlias *na = findnick(user->nick);
				Anope::string *svidbuf = na ? na->nc->GetExt<ExtensibleString *>("authenticationtoken") : NULL;
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
		else if (params.size() == 1)
			do_nick(source, params[0], "", "", "", "", 0, "", "", "", "");
		return true;
	}

	bool OnServer(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		do_server(source, params[0], Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0, params[2], "");
		return true;
	}

	bool OnTopic(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = findchan(params[0]);

		if (!c)
		{
			Log() << "TOPIC " << (params.size() > 1 ? params[1] : "") << " for nonexistent channel " << params[0];
			return true;
		}

		c->ChangeTopicInternal(source, (params.size() > 1 ? params[1] : ""), Anope::CurTime);

		return true;
	}

	bool OnCapab(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
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
		else if (params[0].equals_cs("MODULES"))
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
		else if (params[0].equals_cs("CAPABILITIES"))
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
					ircd->maxmodes = maxmodes.is_pos_number_only() ? convertTo<unsigned>(maxmodes) : 3;
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
			ircd->svshold = has_svsholdmod;
		}

		IRCdMessage::OnCapab(source, params);

		return true;
	}

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
				c->SetModeInternal(NULL, *it, buf);

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

bool event_ftopic(const Anope::string &source, const std::vector<Anope::string> &params)
{
	/* :source FTOPIC channel ts setby :topic */
	if (params.size() < 4)
		return true;
	
	Channel *c = findchan(params[0]);
	if (!c)
	{
		Log() << "TOPIC for nonexistant channel " << params[0];
		return true;
	}

	c->ChangeTopicInternal(params[2], params[3], Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime);

	return true;
}

bool event_opertype(const Anope::string &source, const std::vector<Anope::string> &params)
{
	/* opertype is equivalent to mode +o because servers
	   dont do this directly */
	User *u = finduser(source);
	if (u && !u->HasMode(UMODE_OPER))
	{
		std::vector<Anope::string> newparams;
		newparams.push_back(source);
		newparams.push_back("+o");
		return ircdmessage->OnMode(source, newparams);
	}
	else
		return true;
}

bool event_fmode(const Anope::string &source, const std::vector<Anope::string> &params)
{
	/* :source FMODE #test 12345678 +nto foo */
	if (params.size() < 3)
		return true;

	Channel *c = findchan(params[0]);
	/* Checking the TS for validity to avoid desyncs */
	if (c)
	{
		time_t ts = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : 0;
		if (c->creation_time > ts)
		{
			/* Our TS is bigger, we should lower it */
			c->creation_time = ts;
			c->Reset();
		}
		else if (c->creation_time < ts)
			/* The TS we got is bigger, we should ignore this message. */
			return true;
	}
	else
		/* Got FMODE for a non-existing channel */
		return true;

	/* TS's are equal now, so we can proceed with parsing */
	std::vector<Anope::string> newparams;
	for (unsigned n = 0; n < params.size(); ++n)
	{
		if (n != 1)
		{
			newparams.push_back(params[n]);
			Log(LOG_DEBUG) << "Param: " << params[n];
		}
	}

	return ircdmessage->OnMode(source, newparams);
}

bool event_rsquit(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.empty() || params.size() > 3)
		return true;

	std::vector<Anope::string> p;
	/* Horrible workaround to an insp bug (#) in how RSQUITs are sent - mark */
	if (params.size() > 1 && Config->ServerName.equals_cs(params[0]))
		p.push_back(params[1]);
	else
		p.push_back(params[0]);

	ircdmessage->OnSQuit(source, p);

	return true;
}

bool event_setname(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.empty())
		return true;

	User *u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "SETNAME for nonexistent user " << source;
		return true;
	}

	u->SetRealname(params[0]);
	return true;
}

bool event_chgname(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() < 2)
		return true;

	User *u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "FNAME for nonexistent user " << source;
		return true;
	}

	u->SetRealname(params[0]);
	return true;
}

bool event_setident(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (!params.empty())
		return true;

	User *u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "SETIDENT for nonexistent user " << source;
		return true;
	}

	u->SetIdent(params[0]);
	return true;
}

bool event_chgident(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() < 2)
		return true;

	User *u = finduser(params[0]);
	if (!u)
	{
		Log(LOG_DEBUG) << "CHGIDENT for nonexistent user " << params[0];
		return true;
	}

	u->SetIdent(params[1]);
	return true;
}

bool event_sethost(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.empty())
		return true;

	User *u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "SETHOST for nonexistent user " << source;
		return true;
	}

	u->SetDisplayedHost(params[0]);
	return true;
}


bool event_chghost(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.empty())
		return true;

	User *u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "FHOST for nonexistent user " << source;
		return true;
	}

	u->SetDisplayedHost(params[0]);
	return true;
}

bool event_endburst(const Anope::string &source, const std::vector<Anope::string> &params)
{
	Me->GetLinks().front()->Sync(true);
	return true;
}

class ProtoInspIRCd : public Module
{
	Message message_endburst, message_rsquit, message_svsmode, message_chghost, message_chgident, message_chgname,
		message_sethost, message_setident, message_setname, message_fmode, message_ftopic, message_opertype,
		message_idle, message_fjoin;

	InspIRCdProto ircd_proto;
	InspircdIRCdMessage ircd_message;

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
		message_endburst("ENDBURST", event_endburst), message_rsquit("RSQUIT", event_rsquit),
		message_svsmode("SVSMODE", OnMode), message_chghost("CHGHOST", event_chghost),
		message_chgident("CHGIDENT", event_chgident), message_chgname("CHGNAME", event_chgname),
		message_sethost("SETHOST", event_sethost), message_setident("SETIDENT", event_setident),
		message_setname("SETNAME", event_setname), message_fmode("FMODE", event_fmode),
		message_ftopic("FTOPIC", event_ftopic), message_opertype("OPERTYPE", event_opertype),
		message_idle("IDLE", event_idle), message_fjoin("FJOIN", OnSJoin)
	{
		this->SetAuthor("Anope");

		pmodule_ircd_var(&myIrcd);
		pmodule_ircd_proto(&this->ircd_proto);
		pmodule_ircd_message(&this->ircd_message);

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
