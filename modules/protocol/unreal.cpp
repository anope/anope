/* Unreal IRCD 3.2.x functions
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
#include "modules/chanserv/mode.h"
#include "modules/sasl.h"

class UnrealIRCdProto : public IRCDProto
{
 public:
	UnrealIRCdProto(Module *creator) : IRCDProto(creator, "UnrealIRCd 3.2.x")
	{
		DefaultPseudoclientModes = "+Soiq";
		CanSVSNick = true;
		CanSVSJoin = true;
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
	void SendSVSNOOP(const Server *server, bool set) override
	{
		UplinkSocket::Message() << "SVSNOOP " << server->GetName() << " " << (set ? "+" : "-");
	}

	void SendAkillDel(XLine *x) override
	{
		if (x->IsRegex() || x->HasNickOrReal())
			return;

		/* ZLine if we can instead */
		if (x->GetUser() == "*")
		{
			cidr a(x->GetHost());
			if (a.valid())
			{
				IRCD->SendSZLineDel(x);
				return;
			}
		}

		UplinkSocket::Message() << "TKL - G " << x->GetUser() << " " << x->GetHost() << " " << x->GetBy();
	}

	void SendTopic(const MessageSource &source, Channel *c) override
	{
		UplinkSocket::Message(source) << "TOPIC " << c->name << " " << c->topic_setter << " " << c->topic_ts << " :" << c->topic;
	}

	void SendGlobalNotice(ServiceBot *bi, const Server *dest, const Anope::string &msg) override
	{
		UplinkSocket::Message(bi) << "NOTICE $" << dest->GetName() << " :" << msg;
	}

	void SendGlobalPrivmsg(ServiceBot *bi, const Server *dest, const Anope::string &msg) override
	{
		UplinkSocket::Message(bi) << "PRIVMSG $" << dest->GetName() << " :" << msg;
	}

	void SendVhostDel(User *u) override
	{
		ServiceBot *HostServ = Config->GetClient("HostServ");
		u->RemoveMode(HostServ, "CLOAK");
		u->RemoveMode(HostServ, "VHOST");
		ModeManager::ProcessModes();
		u->SetMode(HostServ, "CLOAK");
	}

	void SendAkill(User *u, XLine *x) override
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

			XLine *old = x;

			if (old->manager->HasEntry("*@" + u->host))
				return;

			/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
			XLine *xl = new XLine("*@" + u->host, old->GetBy(), old->GetExpires(), old->GetReason(), old->GetID());
			old->manager->AddXLine(xl);
			x = xl;

			Log(Config->GetClient("OperServ"), "akill") << "AKILL: Added an akill for " << x->GetMask() << " because " << u->GetMask() << "#" << u->realname << " matches " << old->GetMask();
		}

		/* ZLine if we can instead */
		if (x->GetUser() == "*")
		{
			cidr a(x->GetHost());
			if (a.valid())
			{
				IRCD->SendSZLine(u, x);
				return;
			}
		}

		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->GetExpires() - Anope::CurTime;
		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;
		UplinkSocket::Message() << "TKL + G " << x->GetUser() << " " << x->GetHost() << " " << x->GetBy() << " " << Anope::CurTime + timeleft << " " << x->GetCreated() << " :" << x->GetReason();
	}

	void SendSVSKillInternal(const MessageSource &source, User *user, const Anope::string &buf) override
	{
		UplinkSocket::Message(source) << "SVSKILL " << user->nick << " :" << buf;
		user->KillInternal(source, buf);
	}

	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &buf) override
	{
		UplinkSocket::Message(source) << "SVS2MODE " << u->nick <<" " << buf;
	}

	void SendClientIntroduction(User *u) override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message() << "NICK " << u->nick << " 1 " << u->timestamp << " " << u->GetIdent() << " " << u->host << " " << u->server->GetName() << " 0 " << modes << " " << u->host << " * :" << u->realname;
	}

	/* SERVER name hop descript */
	/* Unreal 3.2 actually sends some info about itself in the descript area */
	void SendServer(const Server *server) override
	{
		if (!server->GetSID().empty() && server == Me)
			UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :U0-*-" << server->GetSID() << " " << server->GetDescription();
		else
			UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :" << server->GetDescription();
	}

	/* JOIN */
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override
	{
		UplinkSocket::Message(Me) << "SJOIN " << c->creation_time << " " << c->name << " :" << user->nick;
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

			ServiceBot *setter = ServiceBot::Find(user->GetUID());
			for (size_t i = 0; i < cs.Modes().length(); ++i)
				c->SetMode(setter, ModeManager::FindChannelModeByChar(cs.Modes()[i]), user->GetUID(), false);

			if (uc != NULL)
				uc->status = cs;
		}
	}

	/* unsqline
	*/
	void SendSQLineDel(XLine *x) override
	{
		UplinkSocket::Message() << "UNSQLINE " << x->GetMask();
	}

	/* SQLINE */
	/*
	** - Unreal will translate this to TKL for us
	**
	*/
	void SendSQLine(User *, XLine *x) override
	{
		UplinkSocket::Message() << "SQLINE " << x->GetMask() << " :" << x->GetReason();
	}

	/*
	** svso
	**	  parv[0] = sender prefix
	**	  parv[1] = nick
	**	  parv[2] = options
	*/
	void SendSVSO(ServiceBot *source, const Anope::string &nick, const Anope::string &flag) override
	{
		UplinkSocket::Message(source) << "SVSO " << nick << " " << flag;
	}

	/* Functions that use serval cmd functions */

	void SendVhost(User *u, const Anope::string &vIdent, const Anope::string &vhost) override
	{
		if (!vIdent.empty())
			UplinkSocket::Message(Me) << "CHGIDENT " << u->nick << " " << vIdent;
		if (!vhost.empty())
			UplinkSocket::Message(Me) << "CHGHOST " << u->nick << " " << vhost;
	}

	void SendConnect() override
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
		Anope::string protoctl = "NICKv2 VHP UMODE2 NICKIP SJOIN SJOIN2 SJ3 NOQUIT TKLEXT ESVID MLOCK VL";
		if (!Me->GetSID().empty())
			protoctl += " VL";
		UplinkSocket::Message() << "PROTOCTL " << protoctl;
		UplinkSocket::Message() << "PASS :" << Config->Uplinks[Anope::CurrentUplink].password;
		SendServer(Me);
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick, time_t t) override
	{
		UplinkSocket::Message() << "TKL + Q H " << nick << " " << Me->GetName() << " " << Anope::CurTime + t << " " << Anope::CurTime << " :Being held for registered user";
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) override
	{
		UplinkSocket::Message() << "TKL - Q * " << nick << " " << Me->GetName();
	}

	/* UNSGLINE */
	/*
	 * SVSNLINE - :realname mask
	*/
	void SendSGLineDel(XLine *x) override
	{
		UplinkSocket::Message() << "SVSNLINE - :" << x->GetMask();
	}

	/* UNSZLINE */
	void SendSZLineDel(XLine *x) override
	{
		UplinkSocket::Message() << "TKL - Z * " << x->GetHost() << " " << x->GetBy();
	}

	/* SZLINE */
	void SendSZLine(User *, XLine *x) override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->GetExpires() - Anope::CurTime;
		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;
		UplinkSocket::Message() << "TKL + Z * " << x->GetHost() << " " << x->GetBy() << " " << Anope::CurTime + timeleft << " " << x->GetCreated() << " :" << x->GetReason();
	}

	/* SGLINE */
	/*
	 * SVSNLINE + reason_where_is_space :realname mask with spaces
	*/
	void SendSGLine(User *, XLine *x) override
	{
		Anope::string edited_reason = x->GetReason();
		edited_reason = edited_reason.replace_all_cs(" ", "_");
		UplinkSocket::Message() << "SVSNLINE + " << edited_reason << " :" << x->GetMask();
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
	void SendSVSJoin(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) override
	{
		if (!param.empty())
			UplinkSocket::Message(source) << "SVSJOIN " << user->GetUID() << " " << chan << " :" << param;
		else
			UplinkSocket::Message(source) << "SVSJOIN " << user->GetUID() << " " << chan;
	}

	void SendSVSPart(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) override
	{
		if (!param.empty())
			UplinkSocket::Message(source) << "SVSPART " << user->GetUID() << " " << chan << " :" << param;
		else
			UplinkSocket::Message(source) << "SVSPART " << user->GetUID() << " " << chan;
	}

	void SendSWhois(const MessageSource &source, const Anope::string &who, const Anope::string &mask) override
	{
		UplinkSocket::Message(source) << "SWHOIS " << who << " :" << mask;
	}

	void SendEOB() override
	{
		UplinkSocket::Message(Me) << "EOS";
	}

	bool IsNickValid(const Anope::string &nick) override
	{
		if (nick.equals_ci("ircd") || nick.equals_ci("irc"))
			return false;

		return IRCDProto::IsNickValid(nick);
	}

	bool IsChannelValid(const Anope::string &chan) override
	{
		if (chan.find(':') != Anope::string::npos)
			return false;

		return IRCDProto::IsChannelValid(chan);
	}

	bool IsExtbanValid(const Anope::string &mask) override
	{
		return mask.length() >= 4 && mask[0] == '~' && mask[2] == ':';
	}

	void SendLogin(User *u, NickServ::Nick *na) override
	{
		/* 3.2.10.4+ treats users logged in with accounts as fully registered, even if -r, so we can not set this here. Just use the timestamp. */
		if (Servers::Capab.count("ESVID") > 0 && !na->GetAccount()->HasFieldS("UNCONFIRMED"))
			IRCD->SendMode(Config->GetClient("NickServ"), u, "+d %s", na->GetAccount()->GetDisplay().c_str());
		else
			IRCD->SendMode(Config->GetClient("NickServ"), u, "+d %d", u->signon);
	}

	void SendLogout(User *u) override
	{
		IRCD->SendMode(Config->GetClient("NickServ"), u, "+d 0");
	}

	void SendChannel(Channel *c) override
	{
		/* Unreal does not support updating a channels TS without actually joining a user,
		 * so we will join and part us now
		 */
		ServiceBot *bi = c->ci->WhoSends();
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

	void SendSASLMessage(const SASL::Message &message) override
	{
		size_t p = message.target.find('!');
		if (p == Anope::string::npos)
			return;

		UplinkSocket::Message(ServiceBot::Find(message.source)) << "SASL " << message.target.substr(0, p) << " " << message.target << " " << message.type << " " << message.data << (message.ext.empty() ? "" : " " + message.ext);
	}

	void SendSVSLogin(const Anope::string &uid, const Anope::string &acc) override
	{
		size_t p = uid.find('!');
		if (p == Anope::string::npos)
			return;
		UplinkSocket::Message(Me) << "SVSLOGIN " << uid.substr(0, p) << " " << uid << " " << acc;
	}

	bool IsIdentValid(const Anope::string &ident) override
	{
		if (ident.empty() || ident.length() > Config->GetBlock("networkinfo")->Get<unsigned>("userlen"))
			return false;

		for (unsigned i = 0; i < ident.length(); ++i)
		{
			const char &c = ident[i];

			if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-')
				continue;

			if (c == '-' || c == '.' || c == '_')
				continue;

			return false;
		}

		return true;
	}
};

class UnrealExtBan : public ChannelModeVirtual<ChannelModeList>
{
	char ext;

 public:
	UnrealExtBan(const Anope::string &mname, const Anope::string &basename, char extban) : ChannelModeVirtual<ChannelModeList>(mname, basename)
		, ext(extban)
	{
	}

	ChannelMode *Wrap(Anope::string &param) override
	{
		param = "~" + Anope::string(ext) + ":" + param;
		return ChannelModeVirtual<ChannelModeList>::Wrap(param);
	}

	ChannelMode *Unwrap(ChannelMode *cm, Anope::string &param) override
	{
		if (cm->type != MODE_LIST || param.length() < 4 || param[0] != '~' || param[1] != ext || param[2] != ':')
			return cm;

		param = param.substr(3);
		return this;
	}
};

namespace UnrealExtban
{
	class ChannelMatcher : public UnrealExtBan
	{
	 public:
		ChannelMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : UnrealExtBan(mname, mbase, c)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string channel = mask.substr(3);

			ChannelMode *cm = NULL;
			if (channel[0] != '#')
			{
				char modeChar = ModeManager::GetStatusChar(channel[0]);
				channel.erase(channel.begin());
				cm = ModeManager::FindChannelModeByChar(modeChar);
				if (cm != NULL && cm->type != MODE_STATUS)
					cm = NULL;
			}

			Channel *c = Channel::Find(channel);
			if (c != NULL)
			{
				ChanUserContainer *uc = c->FindUser(u);
				if (uc != NULL)
					if (cm == NULL || uc->status.HasMode(cm->mchar))
						return true;
			}

			return false;
		}
	};

	class EntryMatcher : public UnrealExtBan
	{
	 public:
	 	EntryMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : UnrealExtBan(mname, mbase, c)
	 	{
	 	}

	 	bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(3);

			return Entry(this->name, real_mask).Matches(u);
		}
	};

	class RealnameMatcher : public UnrealExtBan
	{
	 public:
	 	RealnameMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : UnrealExtBan(mname, mbase, c)
	 	{
	 	}

	 	bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(3);

			return Anope::Match(u->realname, real_mask);
		}
	};

	class RegisteredMatcher : public UnrealExtBan
	{
	 public:
	 	RegisteredMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : UnrealExtBan(mname, mbase, c)
	 	{
	 	}

	 	bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			return u->HasMode("REGISTERED") && mask.equals_ci(u->nick);
		}
	};

	class AccountMatcher : public UnrealExtBan
	{
	 public:
	 	AccountMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : UnrealExtBan(mname, mbase, c)
	 	{
	 	}

	 	bool Matches(User *u, const Entry *e) override
	 	{
	 		const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(3);

	 		return u->Account() && Anope::Match(u->Account()->GetDisplay(), real_mask);
	 	}
	};
}

class ChannelModeFlood : public ChannelModeParam
{
 public:
	ChannelModeFlood(char modeChar, bool minusNoArg) : ChannelModeParam("FLOOD", modeChar, minusNoArg) { }

	/* Borrowed part of this check from UnrealIRCd */
	bool IsValid(Anope::string &value) const override
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
				continue; /* continue instead of break for forward compatibility. */
			try
			{
				int v = arg.substr(0, p).is_number_only() ? convertTo<int>(arg.substr(0, p)) : 0;
				if (v < 1 || v > 999)
					return false;
			}
			catch (const ConvertException &)
			{
				return false;
			}
		}

		return true;
	}
};

class ChannelModeUnrealSSL : public ChannelMode
{
 public:
	ChannelModeUnrealSSL(const Anope::string &n, char c) : ChannelMode(n, c)
	{
	}

	bool CanSet(User *u) const override
	{
		return false;
	}
};

struct IRCDMessageChgHost : IRCDMessage
{
	IRCDMessageChgHost(Module *creator) : IRCDMessage(creator, "CHGHOST", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = User::Find(params[0]);
		if (u)
			u->SetDisplayedHost(params[1]);
	}
};

struct IRCDMessageChgIdent : IRCDMessage
{
	IRCDMessageChgIdent(Module *creator) : IRCDMessage(creator, "CHGIDENT", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = User::Find(params[0]);
		if (u)
			u->SetVIdent(params[1]);
	}
};

struct IRCDMessageChgName : IRCDMessage
{
	IRCDMessageChgName(Module *creator) : IRCDMessage(creator, "CHGNAME", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = User::Find(params[0]);
		if (u)
			u->SetRealname(params[1]);
	}
};

struct IRCDMessageMode : IRCDMessage
{
	IRCDMessageMode(Module *creator, const Anope::string &mname) : IRCDMessage(creator, mname, 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		bool server_source = source.GetServer() != NULL;
		Anope::string modes = params[1];
		for (unsigned i = 2; i < params.size() - (server_source ? 1 : 0); ++i)
			modes += " " + params[i];

		if (IRCD->IsChannelValid(params[0]))
		{
			Channel *c = Channel::Find(params[0]);
			time_t ts = 0;

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
			User *u = User::Find(params[0]);
			if (u)
				u->SetModesInternal(source, "%s", params[1].c_str());
		}
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
	IRCDMessageNetInfo(Module *creator) : IRCDMessage(creator, "NETINFO", 8) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		UplinkSocket::Message() << "NETINFO " << MaxUserCount << " " << Anope::CurTime << " " << convertTo<int>(params[2]) << " " << params[3] << " 0 0 0 :" << params[7];
	}
};

struct IRCDMessageNick : IRCDMessage
{
	IRCDMessageNick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (params.size() == 11)
		{
			Anope::string ip;
			if (params[9] != "*")
			{
				Anope::string decoded_ip;
				Anope::B64Decode(params[9], decoded_ip);

				sockaddrs ip_addr;
				ip_addr.ntop(params[9].length() == 8 ? AF_INET : AF_INET6, decoded_ip.c_str());
				ip = ip_addr.addr();
			}

			Anope::string vhost = params[8];
			if (vhost.equals_cs("*"))
				vhost.clear();

			time_t user_ts = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime;

			Server *s = Server::Find(params[5]);
			if (s == NULL)
			{
				Log(LOG_DEBUG) << "User " << params[0] << " introduced from non-existent server " << params[5] << "?";
				return;
			}

			NickServ::Nick *na = NULL;

			if (params[6] == "0")
				;
			else if (params[6].is_pos_number_only())
			{
				if (convertTo<time_t>(params[6]) == user_ts)
					na = NickServ::FindNick(params[0]);
			}
			else
			{
				na = NickServ::FindNick(params[6]);
			}

			User::OnIntroduce(params[0], params[3], params[4], vhost, ip, s, params[10], user_ts, params[7], "", na ? na->GetAccount() : NULL);
		}
		else
			source.GetUser()->ChangeNick(params[0]);
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
	IRCDMessagePong(Module *creator) : IRCDMessage(creator, "PONG", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (!source.GetServer()->IsSynced())
			source.GetServer()->Sync(false);
	}
};

struct IRCDMessageSASL : IRCDMessage
{
	IRCDMessageSASL(Module *creator) : IRCDMessage(creator, "SASL", 4) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		size_t p = params[1].find('!');
		if (!SASL::sasl || p == Anope::string::npos)
			return;

		SASL::Message m;
		m.source = params[1];
		m.target = params[0];
		m.type = params[2];
		m.data = params[3];
		m.ext = params.size() > 4 ? params[4] : "";

		SASL::sasl->ProcessMessage(m);
	}
};

struct IRCDMessageSDesc : IRCDMessage
{
	IRCDMessageSDesc(Module *creator) : IRCDMessage(creator, "SDESC", 1) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		source.GetServer()->SetDescription(params[0]);
	}
};

struct IRCDMessageSetHost : IRCDMessage
{
	IRCDMessageSetHost(Module *creator) : IRCDMessage(creator, "SETHOST", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();

		/* When a user sets +x we receive the new host and then the mode change */
		if (u->HasMode("CLOAK"))
			u->SetDisplayedHost(params[0]);
		else
			u->SetCloakedHost(params[0]);
	}
};

struct IRCDMessageSetIdent : IRCDMessage
{
	IRCDMessageSetIdent(Module *creator) : IRCDMessage(creator, "SETIDENT", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		u->SetVIdent(params[0]);
	}
};

struct IRCDMessageSetName : IRCDMessage
{
	IRCDMessageSetName(Module *creator) : IRCDMessage(creator, "SETNAME", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		u->SetRealname(params[0]);
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		unsigned int hops = Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0;

		if (params[1].equals_cs("1"))
		{
			Anope::string desc;
			spacesepstream(params[2]).GetTokenRemainder(desc, 1);

			new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, desc);
		}
		else
			new Server(source.GetServer(), params[0], hops, params[2]);

		IRCD->SendPing(Me->GetName(), params[0]);
	}
};

struct IRCDMessageSJoin : IRCDMessage
{
	IRCDMessageSJoin(Module *creator) : IRCDMessage(creator, "SJOIN", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		Anope::string modes;
		if (params.size() >= 4)
			for (unsigned i = 2; i < params.size() - 1; ++i)
				modes += " " + params[i];
		if (!modes.empty())
			modes.erase(modes.begin());

		std::list<Anope::string> bans, excepts, invites;
		std::list<Message::Join::SJoinUser> users;

		spacesepstream sep(params[params.size() - 1]);
		Anope::string buf;
		while (sep.GetToken(buf))
		{
			/* Ban */
			if (buf[0] == '&')
			{
				buf.erase(buf.begin());
				bans.push_back(buf);
			}
			/* Except */
			else if (buf[0] == '"')
			{
				buf.erase(buf.begin());
				excepts.push_back(buf);
			}
			/* Invex */
			else if (buf[0] == '\'')
			{
				buf.erase(buf.begin());
				invites.push_back(buf);
			}
			else
			{
				Message::Join::SJoinUser sju;

				/* Get prefixes from the nick */
				for (char ch; (ch = ModeManager::GetStatusChar(buf[0]));)
				{
					sju.first.AddMode(ch);
					buf.erase(buf.begin());
				}

				sju.second = User::Find(buf);
				if (!sju.second)
				{
					Log(LOG_DEBUG) << "SJOIN for non-existent user " << buf << " on " << params[1];
					continue;
				}

				users.push_back(sju);
			}
		}

		time_t ts = Anope::string(params[0]).is_pos_number_only() ? convertTo<time_t>(params[0]) : Anope::CurTime;
		Message::Join::SJoin(source, params[1], ts, modes, users);

		if (!bans.empty() || !excepts.empty() || !invites.empty())
		{
			Channel *c = Channel::Find(params[1]);

			if (!c || c->creation_time != ts)
				return;

			ChannelMode *ban = ModeManager::FindChannelModeByName("BAN"),
				*except = ModeManager::FindChannelModeByName("EXCEPT"),
				*invex = ModeManager::FindChannelModeByName("INVITEOVERRIDE");

			if (ban)
				for (std::list<Anope::string>::iterator it = bans.begin(), it_end = bans.end(); it != it_end; ++it)
					c->SetModeInternal(source, ban, *it);
			if (except)
				for (std::list<Anope::string>::iterator it = excepts.begin(), it_end = excepts.end(); it != it_end; ++it)
					c->SetModeInternal(source, except, *it);
			if (invex)
				for (std::list<Anope::string>::iterator it = invites.begin(), it_end = invites.end(); it != it_end; ++it)
					c->SetModeInternal(source, invex, *it);
		}
	}
};

struct IRCDMessageTopic : IRCDMessage
{
	IRCDMessageTopic(Module *creator) : IRCDMessage(creator, "TOPIC", 4) { }

	/*
	**	source = sender prefix
	**	parv[0] = channel name
	**	parv[1] = topic nickname
	**	parv[2] = topic time
	**	parv[3] = topic text
	*/
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		Channel *c = Channel::Find(params[0]);
		if (c)
			c->ChangeTopicInternal(source.GetUser(), params[1], params[3], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime);
	}
};


struct IRCDMessageUmode2 : IRCDMessage
{
	IRCDMessageUmode2(Module *creator) : IRCDMessage(creator, "UMODE2", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		source.GetUser()->SetModesInternal(source, "%s", params[0].c_str());
	}
};

class ProtoUnreal : public Module
	, public EventHook<Event::UserNickChange>
	, public EventHook<Event::ChannelSync>
	, public EventHook<Event::ChanRegistered>
	, public EventHook<Event::DelChan>
	, public EventHook<Event::MLockEvents>
{
	UnrealIRCdProto ircd_proto;

	/* Core message handlers */
	Message::Away message_away;
	Message::Capab message_capab;
	Message::Error message_error;
	Message::Invite message_invite;
	Message::Join message_join;
	Message::Kick message_kick;
	Message::Kill message_kill, message_svskill;
	Message::MOTD message_motd;
	Message::Notice message_notice;
	Message::Part message_part;
	Message::Ping message_ping;
	Message::Privmsg message_privmsg;
	Message::Quit message_quit;
	Message::SQuit message_squit;
	Message::Stats message_stats;
	Message::Time message_time;
	Message::Version message_version;
	Message::Whois message_whois;

	/* Our message handlers */
	IRCDMessageChgHost message_chghost;
	IRCDMessageChgIdent message_chgident;
	IRCDMessageChgName message_chgname;
	IRCDMessageMode message_mode, message_svsmode, message_svs2mode;
	IRCDMessageNetInfo message_netinfo;
	IRCDMessageNick message_nick;
	IRCDMessagePong message_pong;
	IRCDMessageSASL message_sasl;
	IRCDMessageSDesc message_sdesc;
	IRCDMessageSetHost message_sethost;
	IRCDMessageSetIdent message_setident;
	IRCDMessageSetName message_setname;
	IRCDMessageServer message_server;
	IRCDMessageSJoin message_sjoin;
	IRCDMessageTopic message_topic;
	IRCDMessageUmode2 message_umode2;

	bool use_server_side_mlock;

 public:
	ProtoUnreal(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR)
		, EventHook<Event::UserNickChange>(EventHook<Event::UserNickChange>::Priority::FIRST)
		, EventHook<Event::ChannelSync>(EventHook<Event::ChannelSync>::Priority::FIRST)
		, EventHook<Event::ChanRegistered>(EventHook<Event::ChanRegistered>::Priority::FIRST)
		, EventHook<Event::DelChan>(EventHook<Event::DelChan>::Priority::FIRST)
		, EventHook<Event::MLockEvents>(EventHook<Event::MLockEvents>::Priority::FIRST)
		, ircd_proto(this)
		, message_away(this)
		, message_capab(this, "PROTOCTL")
		, message_error(this)
		, message_invite(this)
		, message_join(this)
		, message_kick(this)
		, message_kill(this)
		, message_svskill(this, "SVSKILL")
		, message_motd(this)
		, message_notice(this)
		, message_part(this)
		, message_ping(this)
		, message_privmsg(this)
		, message_quit(this)
		, message_squit(this)
		, message_stats(this)
		, message_time(this)
		, message_version(this)
		, message_whois(this)

		, message_chghost(this)
		, message_chgident(this)
		, message_chgname(this)
		, message_mode(this, "MODE")
		, message_svsmode(this, "SVSMODE")
		, message_svs2mode(this, "SVS2MODE")
		, message_netinfo(this)
		, message_nick(this)
		, message_pong(this)
		, message_sasl(this)
		, message_sdesc(this)
		, message_sethost(this)
		, message_setident(this)
		, message_setname(this)
		, message_server(this)
		, message_sjoin(this)
		, message_topic(this)
		, message_umode2(this)
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		use_server_side_mlock = conf->GetModule(this)->Get<bool>("use_server_side_mlock");

		for (int i = 0; i < conf->CountBlock("extban"); ++i)
		{
			Configuration::Block *extban = conf->GetBlock("extban", i);
			Anope::string name = extban->Get<Anope::string>("name"),
					type = extban->Get<Anope::string>("type"),
					base = extban->Get<Anope::string>("base"),
					character = extban->Get<Anope::string>("character");

			ChannelMode *cm;

			if (character.empty())
				continue;

			if (type == "channel")
				cm = new UnrealExtban::ChannelMatcher(name, base, character[0]);
			else if (type == "entry")
				cm = new UnrealExtban::EntryMatcher(name, base, character[0]);
			else if (type == "realname")
				cm = new UnrealExtban::RealnameMatcher(name, base, character[0]);
			else if (type == "registered")
				cm = new UnrealExtban::RegisteredMatcher(name, base, character[0]);
			else if (type == "account")
				cm = new UnrealExtban::AccountMatcher(name, base, character[0]);
			else
				continue;

			if (!ModeManager::AddChannelMode(cm))
				delete cm;
		}
	}

	void OnUserNickChange(User *u, const Anope::string &) override
	{
		u->RemoveModeInternal(Me, ModeManager::FindUserModeByName("REGISTERED"));
		if (Servers::Capab.count("ESVID") == 0)
			IRCD->SendLogout(u);
	}

	void OnChannelSync(Channel *c) override
	{
		if (!c->ci)
			return;

		if (use_server_side_mlock && Servers::Capab.count("MLOCK") > 0 && mlocks)
		{
			Anope::string modes = mlocks->GetMLockAsString(c->ci, false).replace_all_cs("+", "").replace_all_cs("-", "");
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(c->creation_time) << " " << c->ci->GetName() << " " << modes;
		}
	}

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		if (!ci->c || !use_server_side_mlock || !mlocks || !Servers::Capab.count("MLOCK"))
			return;
		Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "");
		UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->GetName() << " " << modes;
	}

	void OnDelChan(ChanServ::Channel *ci) override
	{
		if (!ci->c || !use_server_side_mlock || !Servers::Capab.count("MLOCK"))
			return;
		UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->GetName() << " :";
	}

	EventReturn OnMLock(ChanServ::Channel *ci, ModeLock *lock) override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->GetName());
		if (use_server_side_mlock && cm && mlocks && ci->c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->mchar;
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->GetName() << " " << modes;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChanServ::Channel *ci, ModeLock *lock) override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->GetName());
		if (use_server_side_mlock && cm && mlocks && ci->c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->mchar, "");
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->GetName() << " " << modes;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ProtoUnreal)
