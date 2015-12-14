/* inspircd 1.2 functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/sasl.h"

struct SASLUser
{
	Anope::string uid;
	Anope::string acc;
	time_t created;
};

static std::list<SASLUser> saslusers;

static Anope::string rsquit_server, rsquit_id;

class ChannelModeFlood : public ChannelModeParam
{
 public:
	ChannelModeFlood(char modeChar, bool minusNoArg) : ChannelModeParam("FLOOD", modeChar, minusNoArg) { }

	bool IsValid(Anope::string &value) const anope_override
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

class InspIRCd12Proto : public IRCDProto
{
 private:
	void SendSVSKillInternal(const MessageSource &source, User *user, const Anope::string &buf) anope_override
	{
		IRCDProto::SendSVSKillInternal(source, user, buf);
		user->KillInternal(source, buf);
	}

	void SendChgIdentInternal(const Anope::string &nick, const Anope::string &vIdent)
	{
		if (!Servers::Capab.count("CHGIDENT"))
			Log() << "CHGIDENT not loaded!";
		else
			UplinkSocket::Message(Me) << "CHGIDENT " << nick << " " << vIdent;
	}

	void SendChgHostInternal(const Anope::string &nick, const Anope::string &vhost)
	{
		if (!Servers::Capab.count("CHGHOST"))
			Log() << "CHGHOST not loaded!";
		else
			UplinkSocket::Message(Me) << "CHGHOST " << nick << " " << vhost;
	}

	void SendAddLine(const Anope::string &xtype, const Anope::string &mask, time_t duration, const Anope::string &addedby, const Anope::string &reason)
	{
		UplinkSocket::Message(Me) << "ADDLINE " << xtype << " " << mask << " " << addedby << " " << Anope::CurTime << " " << duration << " :" << reason;
	}

	void SendDelLine(const Anope::string &xtype, const Anope::string &mask)
	{
		UplinkSocket::Message(Me) << "DELLINE " << xtype << " " << mask;
	}

 public:
	InspIRCd12Proto(Module *creator) : IRCDProto(creator, "InspIRCd 1.2")
	{
		DefaultPseudoclientModes = "+I";
		CanSVSNick = true;
		CanSVSJoin = true;
		CanSetVHost = true;
		CanSetVIdent = true;
		CanSQLine = true;
		CanSZLine = true;
		CanSVSHold = true;
		CanCertFP = true;
		RequiresID = true;
		MaxModes = 20;
	}

	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "NOTICE $" << dest->GetName() << " :" << msg;
	}

	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "PRIVMSG $" << dest->GetName() << " :" << msg;
	}

	void SendAkillDel(const XLine *x) anope_override
	{
		/* InspIRCd may support regex bans */
		if (x->IsRegex() && Servers::Capab.count("RLINE"))
		{
			Anope::string mask = x->mask;
			size_t h = x->mask.find('#');
			if (h != Anope::string::npos)
				mask = mask.replace(h, 1, ' ');
			SendDelLine("R", mask);
			return;
		}
		else if (x->IsRegex() || x->HasNickOrReal())
			return;

		/* ZLine if we can instead */
		if (x->GetUser() == "*")
		{
			cidr addr(x->GetHost());
			if (addr.valid())
			{
				IRCD->SendSZLineDel(x);
				return;
			}
		}

		SendDelLine("G", x->GetUser() + "@" + x->GetHost());
	}

	void SendTopic(const MessageSource &source, Channel *c) anope_override
	{
		if (Servers::Capab.count("SVSTOPIC"))
		{
			UplinkSocket::Message(c->ci->WhoSends()) << "SVSTOPIC " << c->name << " " << c->topic_ts << " " << c->topic_setter << " :" << c->topic;
		}
		else
		{
			/* If the last time a topic was set is after the TS we want for this topic we must bump this topic's timestamp to now */
			time_t ts = c->topic_ts;
			if (c->topic_time > ts)
				ts = Anope::CurTime;
			/* But don't modify c->topic_ts, it should remain set to the real TS we want as ci->last_topic_time pulls from it */
			UplinkSocket::Message(source) << "FTOPIC " << c->name << " " << ts << " " << c->topic_setter << " :" << c->topic;
		}
	}

	void SendVhostDel(User *u) anope_override
	{
		if (u->HasMode("CLOAK"))
			this->SendChgHostInternal(u->nick, u->chost);
		else
			this->SendChgHostInternal(u->nick, u->host);

		if (Servers::Capab.count("CHGIDENT") && u->GetIdent() != u->GetVIdent())
			this->SendChgIdentInternal(u->nick, u->GetIdent());
	}

	void SendAkill(User *u, XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->expires - Anope::CurTime;
		if (timeleft > 172800 || !x->expires)
			timeleft = 172800;

		/* InspIRCd may support regex bans, if they do we can send this and forget about it */
		if (x->IsRegex() && Servers::Capab.count("RLINE"))
		{
			Anope::string mask = x->mask;
			size_t h = x->mask.find('#');
			if (h != Anope::string::npos)
				mask = mask.replace(h, 1, ' ');
			SendAddLine("R", mask, timeleft, x->by, x->GetReason());
			return;
		}
		else if (x->IsRegex() || x->HasNickOrReal())
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

			Log(Config->GetClient("OperServ"), "akill") << "AKILL: Added an akill for " << x->mask << " because " << u->GetMask() << "#" << u->realname << " matches " << old->mask;
		}

		/* ZLine if we can instead */
		if (x->GetUser() == "*")
		{
			cidr addr(x->GetHost());
			if (addr.valid())
			{
				IRCD->SendSZLine(u, x);
				return;
			}
		}

		SendAddLine("G", x->GetUser() + "@" + x->GetHost(), timeleft, x->by, x->GetReason());
	}

	void SendNumericInternal(int numeric, const Anope::string &dest, const Anope::string &buf) anope_override
	{
		User *u = User::Find(dest);
		UplinkSocket::Message() << "PUSH " << dest << " ::" << Me->GetName() << " " << numeric << " " << (u ? u->nick : dest) << " " << buf;
	}

	void SendModeInternal(const MessageSource &source, const Channel *dest, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "FMODE " << dest->name << " " << dest->creation_time << " " << buf;
	}

	void SendClientIntroduction(User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "UID " << u->GetUID() << " " << u->timestamp << " " << u->nick << " " << u->host << " " << u->host << " " << u->GetIdent() << " 0.0.0.0 " << u->timestamp << " " << modes << " :" << u->realname;
		if (modes.find('o') != Anope::string::npos)
			UplinkSocket::Message(u) << "OPERTYPE :services";
	}

	/* SERVER services-dev.chatspike.net password 0 :Description here */
	void SendServer(const Server *server) anope_override
	{
		/* if rsquit is set then we are waiting on a squit */
		if (rsquit_id.empty() && rsquit_server.empty())
			UplinkSocket::Message() << "SERVER " << server->GetName() << " " << Config->Uplinks[Anope::CurrentUplink].password << " " << server->GetHops() << " " << server->GetSID() << " :" << server->GetDescription();
	}

	void SendSquit(Server *s, const Anope::string &message) anope_override
	{
		if (s != Me)
		{
			rsquit_id = s->GetSID();
			rsquit_server = s->GetName();
			UplinkSocket::Message() << "RSQUIT " << s->GetName() << " :" << message;
		}
		else
			UplinkSocket::Message() << "SQUIT " << s->GetName() << " :" << message;
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
			ChanUserContainer *uc = c->FindUser(user);
			if (uc != NULL)
				uc->status.Clear();

			BotInfo *setter = BotInfo::Find(user->GetUID());
			for (size_t i = 0; i < cs.Modes().length(); ++i)
				c->SetMode(setter, ModeManager::FindChannelModeByChar(cs.Modes()[i]), user->GetUID(), false);

			if (uc != NULL)
				uc->status = cs;
		}
	}

	/* UNSQLINE */
	void SendSQLineDel(const XLine *x) anope_override
	{
		SendDelLine("Q", x->mask);
	}

	/* SQLINE */
	void SendSQLine(User *, const XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->expires - Anope::CurTime;
		if (timeleft > 172800 || !x->expires)
			timeleft = 172800;
		SendAddLine("Q", x->mask, timeleft, x->by, x->GetReason());
	}

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
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick, time_t t) anope_override
	{
		UplinkSocket::Message(Config->GetClient("NickServ")) << "SVSHOLD " << nick << " " << t << " :Being held for registered user";
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message(Config->GetClient("NickServ")) << "SVSHOLD " << nick;
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x) anope_override
	{
		SendDelLine("Z", x->GetHost());
	}

	/* SZLINE */
	void SendSZLine(User *, const XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->expires - Anope::CurTime;
		if (timeleft > 172800 || !x->expires)
			timeleft = 172800;
		SendAddLine("Z", x->GetHost(), timeleft, x->by, x->GetReason());
	}

	void SendSVSJoin(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &) anope_override
	{
		UplinkSocket::Message(source) << "SVSJOIN " << u->GetUID() << " " << chan;
	}

	void SendSVSPart(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &param) anope_override
	{
		if (!param.empty())
			UplinkSocket::Message(source) << "SVSPART " << u->GetUID() << " " << chan << " :" << param;
		else
			UplinkSocket::Message(source) << "SVSPART " << u->GetUID() << " " << chan;
	}

	void SendSWhois(const MessageSource &, const Anope::string &who, const Anope::string &mask) anope_override
	{
		User *u = User::Find(who);

		UplinkSocket::Message(Me) << "METADATA " << u->GetUID() << " swhois :" << mask;
	}

	void SendBOB() anope_override
	{
		UplinkSocket::Message(Me) << "BURST " << Anope::CurTime;
		Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
		UplinkSocket::Message(Me) << "VERSION :Anope-" << Anope::Version() << " " << Me->GetName() << " :" << IRCD->GetProtocolName() << " - (" << (enc ? enc->name : "none") << ") -- " << Anope::VersionBuildString();
	}

	void SendEOB() anope_override
	{
		UplinkSocket::Message(Me) << "ENDBURST";
	}

	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) anope_override
	{
		if (Servers::Capab.count("GLOBOPS"))
			UplinkSocket::Message(source) << "SNONOTICE g :" << buf;
		else
			UplinkSocket::Message(source) << "SNONOTICE A :" << buf;
	}

	void SendLogin(User *u, NickAlias *na) anope_override
	{
		/* InspIRCd uses an account to bypass chmode +R, not umode +r, so we can't send this here */
		if (na->nc->HasExt("UNCONFIRMED"))
			return;

		UplinkSocket::Message(Me) << "METADATA " << u->GetUID() << " accountname :" << na->nc->display;
	}

	void SendLogout(User *u) anope_override
	{
		UplinkSocket::Message(Me) << "METADATA " << u->GetUID() << " accountname :";
	}

	void SendChannel(Channel *c) anope_override
	{
		UplinkSocket::Message(Me) << "FJOIN " << c->name << " " << c->creation_time << " +" << c->GetModes(true, true) << " :";
	}

	void SendOper(User *u) anope_override
	{
	}

	void SendSASLMessage(const SASL::Message &message) anope_override
	{
		UplinkSocket::Message(Me) << "ENCAP " << message.target.substr(0, 3) << " SASL " << message.source << " " << message.target << " " << message.type << " " << message.data << (message.ext.empty() ? "" : (" " + message.ext));
	}

	void SendSVSLogin(const Anope::string &uid, const Anope::string &acc) anope_override
	{
		UplinkSocket::Message(Me) << "METADATA " << uid << " accountname :" << acc;

		SASLUser su;
		su.uid = uid;
		su.acc = acc;
		su.created = Anope::CurTime;

		for (std::list<SASLUser>::iterator it = saslusers.begin(); it != saslusers.end();)
		{
			SASLUser &u = *it;

			if (u.created + 30 < Anope::CurTime || u.uid == uid)
				it = saslusers.erase(it);
			else
				++it;
		}

		saslusers.push_back(su);
	}

	bool IsExtbanValid(const Anope::string &mask) anope_override
	{
		return mask.length() >= 3 && mask[1] == ':';
	}

	bool IsIdentValid(const Anope::string &ident) anope_override
	{
		if (ident.empty() || ident.length() > Config->GetBlock("networkinfo")->Get<unsigned>("userlen"))
			return false;

		for (unsigned i = 0; i < ident.length(); ++i)
		{
			const char &c = ident[i];

			if (c >= 'A' && c <= '}')
				continue;

			if ((c >= '0' && c <= '9') || c == '-' || c == '.')
				continue;

			return false;
		}

		return true;
	}
};

class InspIRCdExtBan : public ChannelModeList
{
 public:
	InspIRCdExtBan(const Anope::string &mname, char modeChar) : ChannelModeList(mname, modeChar) { }

	bool Matches(User *u, const Entry *e) anope_override
	{
		const Anope::string &mask = e->GetMask();

		if (mask.find("m:") == 0 || mask.find("N:") == 0)
		{
			Anope::string real_mask = mask.substr(2);

			Entry en(this->name, real_mask);
			if (en.Matches(u))
				return true;
		}
		else if (mask.find("j:") == 0)
		{
			Anope::string real_mask = mask.substr(2);

			Channel *c = Channel::Find(real_mask);
			if (c != NULL && c->FindUser(u) != NULL)
				return true;
		}
		else if (mask.find("M:") == 0 || mask.find("R:") == 0)
		{
			Anope::string real_mask = mask.substr(2);

			if (u->IsIdentified() && real_mask.equals_ci(u->Account()->display))
				return true;
		}
		else if (mask.find("r:") == 0)
		{
			Anope::string real_mask = mask.substr(2);

			if (Anope::Match(u->realname, real_mask))
				return true;
		}
		else if (mask.find("s:") == 0)
		{
			Anope::string real_mask = mask.substr(2);

			if (Anope::Match(u->server->GetName(), real_mask))
				return true;
		}

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
			Servers::Capab.clear();
			Servers::Capab.insert("NOQUIT");
			IRCD->CanSVSHold = false;
		}
		else if (params[0].equals_cs("MODULES") && params.size() > 1)
		{
			if (params[1].find("m_globops.so") != Anope::string::npos)
				Servers::Capab.insert("GLOBOPS");
			if (params[1].find("m_services_account.so") != Anope::string::npos)
				Servers::Capab.insert("SERVICES");
			if (params[1].find("m_svshold.so") != Anope::string::npos)
				IRCD->CanSVSHold = true;
			if (params[1].find("m_chghost.so") != Anope::string::npos)
				Servers::Capab.insert("CHGHOST");
			if (params[1].find("m_chgident.so") != Anope::string::npos)
				Servers::Capab.insert("CHGIDENT");
			if (params[1].find("m_hidechans.so") != Anope::string::npos)
				Servers::Capab.insert("HIDECHANS");
			if (params[1].find("m_servprotect.so") != Anope::string::npos)
				IRCD->DefaultPseudoclientModes = "+Ik";
			if (params[1].find("m_rline.so") != Anope::string::npos)
				Servers::Capab.insert("RLINE");
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
								ModeManager::AddChannelMode(new InspIRCdExtBan("BAN", 'b'));
								continue;
							case 'e':
								ModeManager::AddChannelMode(new InspIRCdExtBan("EXCEPT", 'e'));
								continue;
							case 'I':
								ModeManager::AddChannelMode(new InspIRCdExtBan("INVITEOVERRIDE", 'I'));
								continue;
							/* InspIRCd sends q and a here if they have no prefixes */
							case 'q':
								ModeManager::AddChannelMode(new ChannelModeStatus("OWNER", 'q', '@', 4));
								continue;
							case 'a':
								ModeManager::AddChannelMode(new ChannelModeStatus("PROTECT" , 'a', '@', 3));
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
							case 'F':
								ModeManager::AddChannelMode(new ChannelModeParam("NICKFLOOD", 'F', true));
								continue;
							case 'J':
								ModeManager::AddChannelMode(new ChannelModeParam("NOREJOIN", 'J', true));
								continue;
							case 'L':
								ModeManager::AddChannelMode(new ChannelModeParam("REDIRECT", 'L', true));
								continue;
							case 'f':
								ModeManager::AddChannelMode(new ChannelModeFlood('f', true));
								continue;
							case 'j':
								ModeManager::AddChannelMode(new ChannelModeParam("JOINFLOOD", 'j', true));
								continue;
							case 'l':
								ModeManager::AddChannelMode(new ChannelModeParam("LIMIT", 'l', true));
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
							case 'A':
								ModeManager::AddChannelMode(new ChannelMode("ALLINVITE", 'A'));
								continue;
							case 'B':
								ModeManager::AddChannelMode(new ChannelMode("BLOCKCAPS", 'B'));
								continue;
							case 'C':
								ModeManager::AddChannelMode(new ChannelMode("NOCTCP", 'C'));
								continue;
							case 'D':
								ModeManager::AddChannelMode(new ChannelMode("DELAYEDJOIN", 'D'));
								continue;
							case 'G':
								ModeManager::AddChannelMode(new ChannelMode("CENSOR", 'G'));
								continue;
							case 'K':
								ModeManager::AddChannelMode(new ChannelMode("NOKNOCK", 'K'));
								continue;
							case 'M':
								ModeManager::AddChannelMode(new ChannelMode("REGMODERATED", 'M'));
								continue;
							case 'N':
								ModeManager::AddChannelMode(new ChannelMode("NONICK", 'N'));
								continue;
							case 'O':
								ModeManager::AddChannelMode(new ChannelModeOperOnly("OPERONLY", 'O'));
								continue;
							case 'P':
								ModeManager::AddChannelMode(new ChannelMode("PERM", 'P'));
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
							case 'T':
								ModeManager::AddChannelMode(new ChannelMode("NONOTICE", 'T'));
								continue;
							case 'c':
								ModeManager::AddChannelMode(new ChannelMode("BLOCKCOLOR", 'c'));
								continue;
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
							case 'r':
								ModeManager::AddChannelMode(new ChannelModeNoone("REGISTERED", 'r'));
								continue;
							case 's':
								ModeManager::AddChannelMode(new ChannelMode("SECRET", 's'));
								continue;
							case 't':
								ModeManager::AddChannelMode(new ChannelMode("TOPIC", 't'));
								continue;
							case 'u':
								ModeManager::AddChannelMode(new ChannelMode("AUDITORIUM", 'u'));
								continue;
							case 'z':
								ModeManager::AddChannelMode(new ChannelMode("SSL", 'z'));
								continue;
							default:
								ModeManager::AddChannelMode(new ChannelMode("", modebuf[t]));
						}
					}
				}
				else if (capab.find("USERMODES") != Anope::string::npos)
				{
					Anope::string modes(capab.begin() + 10, capab.end());
					commasepstream sep(modes);
					Anope::string modebuf;

					while (sep.GetToken(modebuf))
					{
						for (size_t t = 0, end = modebuf.length(); t < end; ++t)
						{
							switch (modebuf[t])
							{
								case 'h':
									ModeManager::AddUserMode(new UserModeOperOnly("HELPOP", 'h'));
									continue;
								case 'B':
									ModeManager::AddUserMode(new UserMode("BOT", 'B'));
									continue;
								case 'G':
									ModeManager::AddUserMode(new UserMode("CENSOR", 'G'));
									continue;
								case 'H':
									ModeManager::AddUserMode(new UserModeOperOnly("HIDEOPER", 'H'));
									continue;
								case 'I':
									ModeManager::AddUserMode(new UserMode("PRIV", 'I'));
									continue;
								case 'Q':
									ModeManager::AddUserMode(new UserModeOperOnly("HIDDEN", 'Q'));
									continue;
								case 'R':
									ModeManager::AddUserMode(new UserMode("REGPRIV", 'R'));
									continue;
								case 'S':
									ModeManager::AddUserMode(new UserMode("STRIPCOLOR", 'S'));
									continue;
								case 'W':
									ModeManager::AddUserMode(new UserMode("WHOIS", 'W'));
									continue;
								case 'c':
									ModeManager::AddUserMode(new UserMode("COMMONCHANS", 'c'));
									continue;
								case 'g':
									ModeManager::AddUserMode(new UserMode("CALLERID", 'g'));
									continue;
								case 'i':
									ModeManager::AddUserMode(new UserMode("INVIS", 'i'));
									continue;
								case 'k':
									ModeManager::AddUserMode(new UserModeNoone("PROTECTED", 'k'));
									continue;
								case 'o':
									ModeManager::AddUserMode(new UserModeOperOnly("OPER", 'o'));
									continue;
								case 'r':
									ModeManager::AddUserMode(new UserModeNoone("REGISTERED", 'r'));
									continue;
								case 'w':
									ModeManager::AddUserMode(new UserMode("WALLOPS", 'w'));
									continue;
								case 'x':
									ModeManager::AddUserMode(new UserMode("CLOAK", 'x'));
									continue;
								case 'd':
									ModeManager::AddUserMode(new UserMode("DEAF", 'd'));
									continue;
								default:
									ModeManager::AddUserMode(new UserMode("", modebuf[t]));
							}
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
								ModeManager::AddChannelMode(new ChannelModeStatus("OWNER", 'q', chars[t], level--));
								continue;
							case 'a':
								ModeManager::AddChannelMode(new ChannelModeStatus("PROTECT", 'a', chars[t], level--));
								continue;
							case 'o':
								ModeManager::AddChannelMode(new ChannelModeStatus("OP", 'o', chars[t], level--));
								continue;
							case 'h':
								ModeManager::AddChannelMode(new ChannelModeStatus("HALFOP", 'h', chars[t], level--));
								continue;
							case 'v':
								ModeManager::AddChannelMode(new ChannelModeStatus("VOICE", 'v', chars[t], level--));
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
			if (!Servers::Capab.count("GLOBOPS"))
			{
				UplinkSocket::Message() << "ERROR :m_globops is not loaded. This is required by Anope";
				Anope::QuitReason = "Remote server does not have the m_globops module loaded, and this is required.";
				Anope::Quitting = true;
				return;
			}
			if (!Servers::Capab.count("SERVICES"))
			{
				UplinkSocket::Message() << "ERROR :m_services_account.so is not loaded. This is required by Anope";
				Anope::QuitReason = "ERROR: Remote server does not have the m_services_account module loaded, and this is required.";
				Anope::Quitting = true;
				return;
			}
			if (!Servers::Capab.count("HIDECHANS"))
			{
				UplinkSocket::Message() << "ERROR :m_hidechans.so is not loaded. This is required by Anope";
				Anope::QuitReason = "ERROR: Remote server does not have the m_hidechans module loaded, and this is required.";
				Anope::Quitting = true;
				return;
			}
			if (!IRCD->CanSVSHold)
				Log() << "SVSHOLD missing, Usage disabled until module is loaded.";
			if (!Servers::Capab.count("CHGHOST"))
				Log() << "CHGHOST missing, Usage disabled until module is loaded.";
			if (!Servers::Capab.count("CHGIDENT"))
				Log() << "CHGIDENT missing, Usage disabled until module is loaded.";
		}

		Message::Capab::Run(source, params);
	}
};

struct IRCDMessageChgIdent : IRCDMessage
{
	IRCDMessageChgIdent(Module *creator) : IRCDMessage(creator, "CHGIDENT", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = User::Find(params[0]);
		if (u)
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

struct IRCDMessageEncap : IRCDMessage
{
	IRCDMessageEncap(Module *creator) : IRCDMessage(creator, "ENCAP", 4) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (Anope::Match(Me->GetSID(), params[0]) == false)
			return;

		if (SASL::sasl && params[1] == "SASL" && params.size() >= 6)
		{
			SASL::Message m;
			m.source = params[2];
			m.target = params[3];
			m.type = params[4];
			m.data = params[5];
			m.ext = params.size() > 6 ? params[6] : "";

			SASL::sasl->ProcessMessage(m);
		}
	}
};

struct IRCDMessageEndburst : IRCDMessage
{
	IRCDMessageEndburst(Module *creator) : IRCDMessage(creator, "ENDBURST", 0) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Server *s = source.GetServer();

		Log(LOG_DEBUG) << "Processed ENDBURST for " << s->GetName();

		s->Sync(true);
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
	IRCDMessageFJoin(Module *creator) : IRCDMessage(creator, "FJOIN", 2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Anope::string modes;
		if (params.size() >= 3)
		{
			for (unsigned i = 2; i < params.size() - 1; ++i)
				modes += " " + params[i];
			if (!modes.empty())
				modes.erase(modes.begin());
		}

		std::list<Message::Join::SJoinUser> users;

		spacesepstream sep(params[params.size() - 1]);
		Anope::string buf;
		while (sep.GetToken(buf))
		{
			Message::Join::SJoinUser sju;

			/* Loop through prefixes and find modes for them */
			for (char c; (c = buf[0]) != ',' && c;)
			{
				buf.erase(buf.begin());
				sju.first.AddMode(c);
			}
			/* Erase the , */
			if (!buf.empty())
				buf.erase(buf.begin());

			sju.second = User::Find(buf);
			if (!sju.second)
			{
				Log(LOG_DEBUG) << "FJOIN for non-existent user " << buf << " on " << params[0];
				continue;
			}

			users.push_back(sju);
		}

		time_t ts = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime;
		Message::Join::SJoin(source, params[0], ts, modes, users);
	}
};

struct IRCDMessageFMode : IRCDMessage
{
	IRCDMessageFMode(Module *creator) : IRCDMessage(creator, "FMODE", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* :source FMODE #test 12345678 +nto foo */

		Anope::string modes = params[2];
		for (unsigned n = 3; n < params.size(); ++n)
			modes += " " + params[n];

		Channel *c = Channel::Find(params[0]);
		time_t ts;

		try
		{
			ts = convertTo<time_t>(params[1]);
		}
		catch (const ConvertException &)
		{
			ts = 0;
		}

		if (c)
			c->SetModesInternal(source, modes, ts);
	}
};

struct IRCDMessageFTopic : IRCDMessage
{
	IRCDMessageFTopic(Module *creator) : IRCDMessage(creator, "FTOPIC", 4) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* :source FTOPIC channel topicts setby :topic */

		Channel *c = Channel::Find(params[0]);
		if (c)
			c->ChangeTopicInternal(NULL, params[2], params[3], Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime);
	}
};

struct IRCDMessageIdle : IRCDMessage
{
	IRCDMessageIdle(Module *creator) : IRCDMessage(creator, "IDLE", 1) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		BotInfo *bi = BotInfo::Find(params[0]);
		if (bi)
			UplinkSocket::Message(bi) << "IDLE " << source.GetSource() << " " << Anope::StartTime << " " << (Anope::CurTime - bi->lastmsg);
		else
		{
			User *u = User::Find(params[0]);
			if (u && u->server == Me)
				UplinkSocket::Message(u) <<  "IDLE " << source.GetSource() << " " << Anope::StartTime << " 0";
		}
	}
};

/*
 *   source     = numeric of the sending server
 *   params[0]  = uuid
 *   params[1]  = metadata name
 *   params[2]  = data
 */
struct IRCDMessageMetadata : IRCDMessage
{
	IRCDMessageMetadata(Module *creator) : IRCDMessage(creator, "METADATA", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (isdigit(params[0][0]))
		{
			if (params[1].equals_cs("accountname"))
			{
				User *u = User::Find(params[0]);
				NickCore *nc = NickCore::Find(params[2]);
				if (u && nc)
					u->Login(nc);
			}

			/*
			 *   possible incoming ssl_cert messages:
			 *   Received: :409 METADATA 409AAAAAA ssl_cert :vTrSe c38070ce96e41cc144ed6590a68d45a6 <...> <...>
			 *   Received: :409 METADATA 409AAAAAC ssl_cert :vTrSE Could not get peer certificate: error:00000000:lib(0):func(0):reason(0)
			 */
			else if (params[1].equals_cs("ssl_cert"))
			{
				User *u = User::Find(params[0]);
				if (!u)
					return;
				u->Extend<bool>("ssl");
				Anope::string data = params[2].c_str();
				size_t pos1 = data.find(' ') + 1;
				size_t pos2 = data.find(' ', pos1);
				if ((pos2 - pos1) >= 32) // inspircd supports md5 and sha1 fingerprint hashes -> size 32 or 40 bytes.
				{
					u->fingerprint = data.substr(pos1, pos2 - pos1);
				}
				FOREACH_MOD(OnFingerprint, (u));
			}
		}
		else if (params[0][0] == '#')
		{
		}
		else if (params[0] == "*")
		{
			// Wed Oct  3 15:40:27 2012: S[14] O :20D METADATA * modules :-m_svstopic.so

			if (params[1].equals_cs("modules") && !params[2].empty())
			{
				// only interested when it comes from our uplink
				Server* server = source.GetServer();
				if (!server || server->GetUplink() != Me)
					return;

				bool plus = (params[2][0] == '+');
				if (!plus && params[2][0] != '-')
					return;

				bool required = false;
				Anope::string capab, module = params[2].substr(1);

				if (module.equals_cs("m_services_account.so"))
					required = true;
				else if (module.equals_cs("m_hidechans.so"))
					required = true;
				else if (module.equals_cs("m_chghost.so"))
					capab = "CHGHOST";
				else if (module.equals_cs("m_chgident.so"))
					capab = "CHGIDENT";
				else if (module.equals_cs("m_svshold.so"))
					capab = "SVSHOLD";
				else if (module.equals_cs("m_rline.so"))
					capab = "RLINE";
				else if (module.equals_cs("m_topiclock.so"))
					capab = "TOPICLOCK";
				else
					return;

				if (required)
				{
					if (!plus)
						Log() << "Warning: InspIRCd unloaded module " << module << ", Anope won't function correctly without it";
				}
				else
				{
					if (plus)
						Servers::Capab.insert(capab);
					else
						Servers::Capab.erase(capab);

					Log() << "InspIRCd " << (plus ? "loaded" : "unloaded") << " module " << module << ", adjusted functionality";
				}

			}
		}
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

			Anope::string modes = params[1];
			for (unsigned n = 2; n < params.size(); ++n)
				modes += " " + params[n];

			if (c)
				c->SetModesInternal(source, modes);
		}
		else
		{
			/* InspIRCd lets opers change another
			   users modes, we have to kludge this
			   as it slightly breaks RFC1459
			 */
			User *u = source.GetUser();
			// This can happen with server-origin modes.
			if (!u)
				u = User::Find(params[0]);
			// if it's still null, drop it like fire.
			// most likely situation was that server introduced a nick which we subsequently akilled
			if (u)
				u->SetModesInternal(source, "%s", params[1].c_str());
		}
	}
};

struct IRCDMessageNick : IRCDMessage
{
	IRCDMessageNick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->ChangeNick(params[0]);
	}
};

struct IRCDMessageOperType : IRCDMessage
{
	IRCDMessageOperType(Module *creator) : IRCDMessage(creator, "OPERTYPE", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		/* opertype is equivalent to mode +o because servers
		   don't do this directly */
		User *u = source.GetUser();
		if (!u->HasMode("OPER"))
			u->SetModesInternal(source, "+o");
	}
};

struct IRCDMessageRSQuit : IRCDMessage
{
	IRCDMessageRSQuit(Module *creator) : IRCDMessage(creator, "RSQUIT", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Server *s = Server::Find(params[0]);
		const Anope::string &reason = params.size() > 1 ? params[1] : "";
		if (!s)
			return;

		UplinkSocket::Message(Me) << "SQUIT " << s->GetSID() << " :" << reason;
		s->Delete(s->GetName() + " " + s->GetUplink()->GetName());
	}
};

struct IRCDMessageSetIdent : IRCDMessage
{
	IRCDMessageSetIdent(Module *creator) : IRCDMessage(creator, "SETIDENT", 0) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->SetIdent(params[0]);
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 5) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*
	 * [Nov 04 00:08:46.308435 2009] debug: Received: SERVER irc.inspircd.com pass 0 964 :Testnet Central!
	 * 0: name
	 * 1: pass
	 * 2: hops
	 * 3: numeric
	 * 4: desc
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		unsigned int hops = Anope::string(params[2]).is_pos_number_only() ? convertTo<unsigned>(params[2]) : 0;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[4], params[3]);
	}
};

struct IRCDMessageSQuit : Message::SQuit
{
	IRCDMessageSQuit(Module *creator) : Message::SQuit(creator) { }
	
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params[0] == rsquit_id || params[0] == rsquit_server)
		{
			/* squit for a recently squit server, introduce the juped server now */
			Server *s = Server::Find(rsquit_server);

			rsquit_id.clear();
			rsquit_server.clear();

			if (s && s->IsJuped())
				IRCD->SendServer(s);
		}
		else
			Message::SQuit::Run(source, params);
	}
};

struct IRCDMessageTime : IRCDMessage
{
	IRCDMessageTime(Module *creator) : IRCDMessage(creator, "TIME", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		UplinkSocket::Message(Me) << "TIME " << source.GetSource() << " " << params[1] << " " << Anope::CurTime;
	}
};

struct IRCDMessageUID : IRCDMessage
{
	IRCDMessageUID(Module *creator) : IRCDMessage(creator, "UID", 8) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	/*
	 * [Nov 03 22:09:58.176252 2009] debug: Received: :964 UID 964AAAAAC 1225746297 w00t2 localhost testnet.user w00t 127.0.0.1 1225746302 +iosw +ACGJKLNOQcdfgjklnoqtx :Robin Burchell <w00t@inspircd.org>
	 * 0: uid
	 * 1: ts
	 * 2: nick
	 * 3: host
	 * 4: dhost
	 * 5: ident
	 * 6: ip
	 * 7: signon
	 * 8+: modes and params -- IMPORTANT, some modes (e.g. +s) may have parameters. So don't assume a fixed position of realname!
	 * last: realname
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		time_t ts = convertTo<time_t>(params[1]);

		Anope::string modes = params[8];
		for (unsigned i = 9; i < params.size() - 1; ++i)
			modes += " " + params[i];

		NickAlias *na = NULL;
		if (SASL::sasl)
			for (std::list<SASLUser>::iterator it = saslusers.begin(); it != saslusers.end();)
			{
				SASLUser &u = *it;

				if (u.created + 30 < Anope::CurTime)
					it = saslusers.erase(it);
				else if (u.uid == params[0])
				{
					na = NickAlias::Find(u.acc);
					it = saslusers.erase(it);
				}
				else
					++it;
			}

		User *u = User::OnIntroduce(params[2], params[5], params[3], params[4], params[6], source.GetServer(), params[params.size() - 1], ts, modes, params[0], na ? *na->nc : NULL);
		if (u)
			u->signon = convertTo<time_t>(params[7]);
	}
};

class ProtoInspIRCd12 : public Module
{
	InspIRCd12Proto ircd_proto;
	ExtensibleItem<bool> ssl;

	/* Core message handlers */
	Message::Away message_away;
	Message::Error message_error;
	Message::Invite message_invite;
	Message::Join message_join;
	Message::Kick message_kick;
	Message::Kill message_kill;
	Message::MOTD message_motd;
	Message::Notice message_notice;
	Message::Part message_part;
	Message::Ping message_ping;
	Message::Privmsg message_privmsg;
	Message::Quit message_quit;
	Message::Stats message_stats;
	Message::Topic message_topic;

	/* Our message handlers */
	IRCDMessageChgIdent message_chgident;
	IRCDMessageChgName message_setname, message_chgname;
	IRCDMessageCapab message_capab;
	IRCDMessageEncap message_encap;
	IRCDMessageEndburst message_endburst;
	IRCDMessageFHost message_fhost, message_sethost;
	IRCDMessageFJoin message_fjoin;
	IRCDMessageFMode message_fmode;
	IRCDMessageFTopic message_ftopic;
	IRCDMessageIdle message_idle;
	IRCDMessageMetadata message_metadata;
	IRCDMessageMode message_mode;
	IRCDMessageNick message_nick;
	IRCDMessageOperType message_opertype;
	IRCDMessageRSQuit message_rsquit;
	IRCDMessageSetIdent message_setident;
	IRCDMessageServer message_server;
	IRCDMessageSQuit message_squit;
	IRCDMessageTime message_time;
	IRCDMessageUID message_uid;

 public:
	ProtoInspIRCd12(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR),
		ircd_proto(this), ssl(this, "ssl"),
		message_away(this), message_error(this), message_invite(this), message_join(this), message_kick(this), message_kill(this),
		message_motd(this), message_notice(this), message_part(this), message_ping(this), message_privmsg(this), message_quit(this),
		message_stats(this), message_topic(this),

		message_chgident(this), message_setname(this, "SETNAME"), message_chgname(this, "FNAME"), message_capab(this), message_encap(this),
		message_endburst(this),
		message_fhost(this, "FHOST"), message_sethost(this, "SETHOST"), message_fjoin(this), message_fmode(this), message_ftopic(this),
		message_idle(this), message_metadata(this), message_mode(this), message_nick(this), message_opertype(this), message_rsquit(this),
		message_setident(this), message_server(this), message_squit(this), message_time(this), message_uid(this)
	{
		Servers::Capab.insert("NOQUIT");
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		/* InspIRCd 1.2 doesn't set -r on nick change, remove -r here. Note that if we have to set +r later
		 * this will cancel out this -r, resulting in no mode changes.
		 *
		 * Do not set -r if we don't have a NickServ loaded - DP
		 */
		BotInfo *NickServ = Config->GetClient("NickServ");
		if (NickServ)
			u->RemoveMode(NickServ, "REGISTERED");
	}
};

MODULE_INIT(ProtoInspIRCd12)
