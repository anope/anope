/* Inspircd 2.0 functions
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
#include "modules/cs_mode.h"
#include "modules/cs_set.h"

struct SASLUser
{
	Anope::string uid;
	Anope::string acc;
	time_t created;
};

static std::list<SASLUser> saslusers;
static Anope::string rsquit_server, rsquit_id;
static unsigned int spanningtree_proto_ver = 0;

class InspIRCd20Proto : public IRCDProto
{
 private:
	void SendSVSKillInternal(const MessageSource &source, User *user, const Anope::string &buf) override
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
	InspIRCd20Proto(Module *creator) : IRCDProto(creator, "InspIRCd 2.0")
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

	void SendConnect() override
	{
		UplinkSocket::Message() << "CAPAB START 1202";
		UplinkSocket::Message() << "CAPAB CAPABILITIES :PROTOCOL=1202";
		UplinkSocket::Message() << "CAPAB END";
		SendServer(Me);
	}

	void SendGlobalNotice(ServiceBot *bi, const Server *dest, const Anope::string &msg) override
	{
		UplinkSocket::Message(bi) << "NOTICE $" << dest->GetName() << " :" << msg;
	}

	void SendGlobalPrivmsg(ServiceBot *bi, const Server *dest, const Anope::string &msg) override
	{
		UplinkSocket::Message(bi) << "PRIVMSG $" << dest->GetName() << " :" << msg;
	}

	void SendAkillDel(XLine *x) override
	{
		/* InspIRCd may support regex bans */
		if (x->IsRegex() && Servers::Capab.count("RLINE"))
		{
			Anope::string mask = x->GetMask();
			size_t h = mask.find('#');
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

	void SendTopic(const MessageSource &source, Channel *c) override
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

	void SendVhostDel(User *u) override
	{
		if (u->HasMode("CLOAK"))
			this->SendChgHostInternal(u->nick, u->chost);
		else
			this->SendChgHostInternal(u->nick, u->host);

		if (Servers::Capab.count("CHGIDENT") && u->GetIdent() != u->GetVIdent())
			this->SendChgIdentInternal(u->nick, u->GetIdent());
	}

	void SendAkill(User *u, XLine *x) override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->GetExpires() - Anope::CurTime;
		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;

		/* InspIRCd may support regex bans, if they do we can send this and forget about it */
		if (x->IsRegex() && Servers::Capab.count("RLINE"))
		{
			Anope::string mask = x->GetMask();
			size_t h = mask.find('#');
			if (h != Anope::string::npos)
				mask = mask.replace(h, 1, ' ');
			SendAddLine("R", mask, timeleft, x->GetBy(), x->GetReason());
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

			XLine *old = x;

			if (old->manager->HasEntry("*@" + u->host))
				return;

			/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
			x = new XLine("*@" + u->host, old->GetBy(), old->GetExpires(), old->GetReason(), old->GetID());
			old->manager->AddXLine(x);

			Log(Config->GetClient("OperServ"), "akill") << "AKILL: Added an akill for " << x->GetMask() << " because " << u->GetMask() << "#" << u->realname << " matches " << old->GetMask();
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

		SendAddLine("G", x->GetUser() + "@" + x->GetHost(), timeleft, x->GetBy(), x->GetReason());
	}

	void SendNumericInternal(int numeric, const Anope::string &dest, const Anope::string &buf) override
	{
		UplinkSocket::Message() << "PUSH " << dest << " ::" << Me->GetName() << " " << numeric << " " << dest << " " << buf;
	}

	void SendModeInternal(const MessageSource &source, const Channel *dest, const Anope::string &buf) override
	{
		UplinkSocket::Message(source) << "FMODE " << dest->name << " " << dest->creation_time << " " << buf;
	}

	void SendClientIntroduction(User *u) override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "UID " << u->GetUID() << " " << u->timestamp << " " << u->nick << " " << u->host << " " << u->host << " " << u->GetIdent() << " 0.0.0.0 " << u->timestamp << " " << modes << " :" << u->realname;
	}

	/* SERVER services-dev.chatspike.net password 0 :Description here */
	void SendServer(const Server *server) override
	{
		/* if rsquit is set then we are waiting on a squit */
		if (rsquit_id.empty() && rsquit_server.empty())
			UplinkSocket::Message() << "SERVER " << server->GetName() << " " << Config->Uplinks[Anope::CurrentUplink].password << " " << server->GetHops() << " " << server->GetSID() << " :" << server->GetDescription();
	}

	void SendSquit(Server *s, const Anope::string &message) override
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
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override
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

			ServiceBot *setter = ServiceBot::Find(user->nick);
			for (size_t i = 0; i < cs.Modes().length(); ++i)
				c->SetMode(setter, ModeManager::FindChannelModeByChar(cs.Modes()[i]), user->GetUID(), false);

			if (uc != NULL)
				uc->status = cs;
		}
	}

	/* UNSQLINE */
	void SendSQLineDel(XLine *x) override
	{
		SendDelLine("Q", x->GetMask());
	}

	/* SQLINE */
	void SendSQLine(User *, XLine *x) override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->GetExpires() - Anope::CurTime;
		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;
		SendAddLine("Q", x->GetMask(), timeleft, x->GetBy(), x->GetReason());
	}

	void SendVhost(User *u, const Anope::string &vIdent, const Anope::string &vhost) override
	{
		if (!vIdent.empty())
			this->SendChgIdentInternal(u->nick, vIdent);
		if (!vhost.empty())
			this->SendChgHostInternal(u->nick, vhost);
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick, time_t t) override
	{
		UplinkSocket::Message(Config->GetClient("NickServ")) << "SVSHOLD " << nick << " " << t << " :Being held for registered user";
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) override
	{
		UplinkSocket::Message(Config->GetClient("NickServ")) << "SVSHOLD " << nick;
	}

	/* UNSZLINE */
	void SendSZLineDel(XLine *x) override
	{
		SendDelLine("Z", x->GetHost());
	}

	/* SZLINE */
	void SendSZLine(User *, XLine *x) override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->GetExpires() - Anope::CurTime;
		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;
		SendAddLine("Z", x->GetHost(), timeleft, x->GetBy(), x->GetReason());
	}

	void SendSVSJoin(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &) override
	{
		UplinkSocket::Message(source) << "SVSJOIN " << u->GetUID() << " " << chan;
	}

	void SendSVSPart(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &param) override
	{
		if (!param.empty())
			UplinkSocket::Message(source) << "SVSPART " << u->GetUID() << " " << chan << " :" << param;
		else
			UplinkSocket::Message(source) << "SVSPART " << u->GetUID() << " " << chan;
	}

	void SendSWhois(const MessageSource &, const Anope::string &who, const Anope::string &mask) override
	{
		User *u = User::Find(who);

		UplinkSocket::Message(Me) << "METADATA " << u->GetUID() << " swhois :" << mask;
	}

	void SendBOB() override
	{
		UplinkSocket::Message(Me) << "BURST " << Anope::CurTime;
		Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
		UplinkSocket::Message(Me) << "VERSION :Anope-" << Anope::Version() << " " << Me->GetName() << " :" << IRCD->GetProtocolName() << " - (" << (enc ? enc->name : "none") << ") -- " << Anope::VersionBuildString();
	}

	void SendEOB() override
	{
		UplinkSocket::Message(Me) << "ENDBURST";
	}

	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) override
	{
		if (Servers::Capab.count("GLOBOPS"))
			UplinkSocket::Message(source) << "SNONOTICE g :" << buf;
		else
			UplinkSocket::Message(source) << "SNONOTICE A :" << buf;
	}

	void SendLogin(User *u, NickServ::Nick *na) override
	{
		/* InspIRCd uses an account to bypass chmode +R, not umode +r, so we can't send this here */
		if (na->GetAccount()->HasFieldS("UNCONFIRMED"))
			return;

		UplinkSocket::Message(Me) << "METADATA " << u->GetUID() << " accountname :" << na->GetAccount()->GetDisplay();
	}

	void SendLogout(User *u) override
	{
		UplinkSocket::Message(Me) << "METADATA " << u->GetUID() << " accountname :";
	}

	void SendChannel(Channel *c) override
	{
		UplinkSocket::Message(Me) << "FJOIN " << c->name << " " << c->creation_time << " +" << c->GetModes(true, true) << " :";
	}

	void SendSASLMessage(const SASL::Message &message) override
	{
		UplinkSocket::Message(Me) << "ENCAP " << message.target.substr(0, 3) << " SASL " << message.source << " " << message.target << " " << message.type << " " << message.data << (message.ext.empty() ? "" : (" " + message.ext));
	}

	void SendSVSLogin(const Anope::string &uid, const Anope::string &acc) override
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

	bool IsExtbanValid(const Anope::string &mask) override
	{
		return mask.length() >= 3 && mask[1] == ':';
	}

	bool IsIdentValid(const Anope::string &ident) override
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

class InspIRCdExtBan : public ChannelModeVirtual<ChannelModeList>
{
	char ext;

 public:
	InspIRCdExtBan(const Anope::string &mname, const Anope::string &basename, char extban) : ChannelModeVirtual<ChannelModeList>(mname, basename)
		, ext(extban)
	{
	}

	ChannelMode *Wrap(Anope::string &param) override
	{
		param = Anope::string(ext) + ":" + param;
		return ChannelModeVirtual<ChannelModeList>::Wrap(param);
	}

	ChannelMode *Unwrap(ChannelMode *cm, Anope::string &param) override
	{
		if (cm->type != MODE_LIST || param.length() < 3 || param[0] != ext || param[1] != ':')
			return cm;

		param = param.substr(2);
		return this;
	}
};

namespace InspIRCdExtban
{
	class EntryMatcher : public InspIRCdExtBan
	{
	 public:
		EntryMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(3);

			return Entry(this->name, real_mask).Matches(u);
		}
	};

	class ChannelMatcher : public InspIRCdExtBan
	{
	 public:
		ChannelMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
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

	class AccountMatcher : public InspIRCdExtBan
	{
	 public:
	 	AccountMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
	 	{
	 	}

		bool Matches(User *u, const Entry *e) override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(2);

			return u->IsIdentified() && real_mask.equals_ci(u->Account()->GetDisplay());
		}
	};

	class RealnameMatcher : public InspIRCdExtBan
	{
	 public:
	 	RealnameMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
	 	{
	 	}

	 	bool Matches(User *u, const Entry *e) override
	 	{
	 		const Anope::string &mask = e->GetMask();
	 		Anope::string real_mask = mask.substr(2);
	 		return Anope::Match(u->realname, real_mask);
	 	}
	};

	class ServerMatcher : public InspIRCdExtBan
	{
	 public:
	 	ServerMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
	 	{
	 	}

	 	bool Matches(User *u, const Entry *e) override
	 	{
	 		const Anope::string &mask = e->GetMask();
	 		Anope::string real_mask = mask.substr(2);
	 		return Anope::Match(u->server->GetName(), real_mask);
	 	}
	};

	class FinerprintMatcher : public InspIRCdExtBan
	{
	 public:
	 	FinerprintMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
	 	{
	 	}

	 	bool Matches(User *u, const Entry *e) override
	 	{
	 		const Anope::string &mask = e->GetMask();
	 		Anope::string real_mask = mask.substr(2);
	 		return !u->fingerprint.empty() && Anope::Match(u->fingerprint, real_mask);
	 	}
	};

	class UnidentifiedMatcher : public InspIRCdExtBan
	{
	 public:
		UnidentifiedMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
	 		const Anope::string &mask = e->GetMask();
	 		Anope::string real_mask = mask.substr(2);
			return !u->Account() && Entry("BAN", real_mask).Matches(u);
		}
	};
}

class ColonDelimitedParamMode : public ChannelModeParam
{
 public:
	ColonDelimitedParamMode(const Anope::string &modename, char modeChar) : ChannelModeParam(modename, modeChar, true) { }

	bool IsValid(Anope::string &value) const override
	{
		return IsValid(value, false);
	}

	bool IsValid(const Anope::string &value, bool historymode) const
	{
		if (value.empty())
			return false; // empty param is never valid

		Anope::string::size_type pos = value.find(':');
		if ((pos == Anope::string::npos) || (pos == 0))
			return false; // no ':' or it's the first char, both are invalid

		Anope::string rest;
		try
		{
			if (convertTo<int>(value, rest, false) <= 0)
				return false; // negative numbers and zero are invalid

			rest = rest.substr(1);
			int n;
			if (historymode)
			{
				// For the history mode, the part after the ':' is a duration and it
				// can be in the user friendly "1d3h20m" format, make sure we accept that
				n = Anope::DoTime(rest);
			}
			else
				n = convertTo<int>(rest);

			if (n <= 0)
				return false;
		}
		catch (const ConvertException &e)
		{
			// conversion error, invalid
			return false;
		}

		return true;
	}
};

class SimpleNumberParamMode : public ChannelModeParam
{
 public:
	SimpleNumberParamMode(const Anope::string &modename, char modeChar) : ChannelModeParam(modename, modeChar, true) { }

	bool IsValid(Anope::string &value) const override
	{
		if (value.empty())
			return false; // empty param is never valid

		try
		{
			if (convertTo<int>(value) <= 0)
				return false;
		}
		catch (const ConvertException &e)
		{
			// conversion error, invalid
			return false;
		}

		return true;
	}
};

class ChannelModeFlood : public ColonDelimitedParamMode
{
 public:
	ChannelModeFlood(char modeChar) : ColonDelimitedParamMode("FLOOD", modeChar) { }

	bool IsValid(Anope::string &value) const override
	{
		// The parameter of this mode is a bit different, it may begin with a '*',
		// ignore it if that's the case
		Anope::string v = value[0] == '*' ? value.substr(1) : value;
		return ((!value.empty()) && (ColonDelimitedParamMode::IsValid(v)));
	}
};

class ChannelModeHistory : public ColonDelimitedParamMode
{
 public:
	ChannelModeHistory(char modeChar) : ColonDelimitedParamMode("HISTORY", modeChar) { }

	bool IsValid(Anope::string &value) const override
	{
		return (ColonDelimitedParamMode::IsValid(value, true));
	}
};

class ChannelModeRedirect : public ChannelModeParam
{
 public:
	ChannelModeRedirect(char modeChar) : ChannelModeParam("REDIRECT", modeChar, true) { }

	bool IsValid(Anope::string &value) const override
	{
		// The parameter of this mode is a channel, and channel names start with '#'
		return ((!value.empty()) && (value[0] == '#'));
	}
};

struct IRCDMessageCapab : Message::Capab
{
	std::map<char, Anope::string> chmodes, umodes;

	IRCDMessageCapab(Module *creator) : Message::Capab(creator, "CAPAB") { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (params[0].equals_cs("START"))
		{
			if (params.size() >= 2)
				spanningtree_proto_ver = (Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0);

			if (spanningtree_proto_ver < 1202)
			{
				UplinkSocket::Message() << "ERROR :Protocol mismatch, no or invalid protocol version given in CAPAB START";
				Anope::QuitReason = "Protocol mismatch, no or invalid protocol version given in CAPAB START";
				Anope::Quitting = true;
				return;
			}

			/* reset CAPAB */
			chmodes.clear();
			umodes.clear();
			Servers::Capab.insert("SERVERS");
			Servers::Capab.insert("CHGHOST");
			Servers::Capab.insert("CHGIDENT");
			Servers::Capab.insert("TOPICLOCK");
			IRCD->CanSVSHold = false;
		}
		else if (params[0].equals_cs("CHANMODES") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string capab;

			while (ssep.GetToken(capab))
			{
				Anope::string modename = capab.substr(0, capab.find('='));
				Anope::string modechar = capab.substr(capab.find('=') + 1);
				ChannelMode *cm = NULL;

				if (modename.equals_cs("admin"))
					cm = new ChannelModeStatus("PROTECT", modechar.length() > 1 ? modechar[1] : modechar[0], modechar.length() > 1 ? modechar[0] : 0, 3);
				else if (modename.equals_cs("allowinvite"))
				{
					cm = new ChannelMode("ALLINVITE", modechar[0]);
					ModeManager::AddChannelMode(new InspIRCdExtban::EntryMatcher("INVITEBAN", "BAN", 'A'));
				}
				else if (modename.equals_cs("auditorium"))
					cm = new ChannelMode("AUDITORIUM", modechar[0]);
				else if (modename.equals_cs("ban"))
					cm = new ChannelModeList("BAN", modechar[0]);
				else if (modename.equals_cs("banexception"))
					cm = new ChannelModeList("EXCEPT", modechar[0]);
				else if (modename.equals_cs("blockcaps"))
				{
					cm = new ChannelMode("BLOCKCAPS", modechar[0]);
					ModeManager::AddChannelMode(new InspIRCdExtban::EntryMatcher("BLOCKCAPSBAN", "BAN", 'B'));
				}
				else if (modename.equals_cs("blockcolor"))
				{
					cm = new ChannelMode("BLOCKCOLOR", modechar[0]);
					ModeManager::AddChannelMode(new InspIRCdExtban::EntryMatcher("BLOCKCOLORBAN", "BAN", 'c'));
				}
				else if (modename.equals_cs("c_registered"))
					cm = new ChannelModeNoone("REGISTERED", modechar[0]);
				else if (modename.equals_cs("censor"))
					cm = new ChannelMode("CENSOR", modechar[0]);
				else if (modename.equals_cs("delayjoin"))
					cm = new ChannelMode("DELAYEDJOIN", modechar[0]);
				else if (modename.equals_cs("delaymsg"))
					cm = new SimpleNumberParamMode("DELAYMSG", modechar[0]);
				else if (modename.equals_cs("filter"))
					cm = new ChannelModeList("FILTER", modechar[0]);
				else if (modename.equals_cs("flood"))
					cm = new ChannelModeFlood(modechar[0]);
				else if (modename.equals_cs("founder"))
					cm = new ChannelModeStatus("OWNER", modechar.length() > 1 ? modechar[1] : modechar[0], modechar.length() > 1 ? modechar[0] : 0, 4);
				else if (modename.equals_cs("halfop"))
					cm = new ChannelModeStatus("HALFOP", modechar.length() > 1 ? modechar[1] : modechar[0], modechar.length() > 1 ? modechar[0] : 0, 1);
				else if (modename.equals_cs("history"))
					cm = new ChannelModeHistory(modechar[0]);
				else if (modename.equals_cs("invex"))
					cm = new ChannelModeList("INVITEOVERRIDE", modechar[0]);
				else if (modename.equals_cs("inviteonly"))
					cm = new ChannelMode("INVITE", modechar[0]);
				else if (modename.equals_cs("joinflood"))
					cm = new ColonDelimitedParamMode("JOINFLOOD", modechar[0]);
				else if (modename.equals_cs("key"))
					cm = new ChannelModeKey(modechar[0]);
				else if (modename.equals_cs("kicknorejoin"))
					cm = new SimpleNumberParamMode("NOREJOIN", modechar[0]);
				else if (modename.equals_cs("limit"))
					cm = new ChannelModeParam("LIMIT", modechar[0], true);
				else if (modename.equals_cs("moderated"))
					cm = new ChannelMode("MODERATED", modechar[0]);
				else if (modename.equals_cs("nickflood"))
					cm = new ColonDelimitedParamMode("NICKFLOOD", modechar[0]);
				else if (modename.equals_cs("noctcp"))
				{
					cm = new ChannelMode("NOCTCP", modechar[0]);
					ModeManager::AddChannelMode(new InspIRCdExtban::EntryMatcher("NOCTCPBAN", "BAN", 'C'));
				}
				else if (modename.equals_cs("noextmsg"))
					cm = new ChannelMode("NOEXTERNAL", modechar[0]);
				else if (modename.equals_cs("nokick"))
				{
					cm = new ChannelMode("NOKICK", modechar[0]);
					ModeManager::AddChannelMode(new InspIRCdExtban::EntryMatcher("NOKICKBAN", "BAN", 'Q'));
				}
				else if (modename.equals_cs("noknock"))
					cm = new ChannelMode("NOKNOCK", modechar[0]);
				else if (modename.equals_cs("nonick"))
				{
					cm = new ChannelMode("NONICK", modechar[0]);
					ModeManager::AddChannelMode(new InspIRCdExtban::EntryMatcher("NONICKBAN", "BAN", 'N'));
				}
				else if (modename.equals_cs("nonotice"))
				{
					cm = new ChannelMode("NONOTICE", modechar[0]);
					ModeManager::AddChannelMode(new InspIRCdExtban::EntryMatcher("NONOTICEBAN", "BAN", 'T'));
				}
				else if (modename.equals_cs("op"))
					cm = new ChannelModeStatus("OP", modechar.length() > 1 ? modechar[1] : modechar[0], modechar.length() > 1 ? modechar[0] : 0, 2);
				else if (modename.equals_cs("operonly"))
					cm = new ChannelModeOperOnly("OPERONLY", modechar[0]);
				else if (modename.equals_cs("permanent"))
					cm = new ChannelMode("PERM", modechar[0]);
				else if (modename.equals_cs("private"))
					cm = new ChannelMode("PRIVATE", modechar[0]);
				else if (modename.equals_cs("redirect"))
					cm = new ChannelModeRedirect(modechar[0]);
				else if (modename.equals_cs("reginvite"))
					cm = new ChannelMode("REGISTEREDONLY", modechar[0]);
				else if (modename.equals_cs("regmoderated"))
					cm = new ChannelMode("REGMODERATED", modechar[0]);
				else if (modename.equals_cs("secret"))
					cm = new ChannelMode("SECRET", modechar[0]);
				else if (modename.equals_cs("sslonly"))
				{
					cm = new ChannelMode("SSL", modechar[0]);
					ModeManager::AddChannelMode(new InspIRCdExtban::FinerprintMatcher("SSLBAN", "BAN", 'z'));
				}
				else if (modename.equals_cs("stripcolor"))
				{
					cm = new ChannelMode("STRIPCOLOR", modechar[0]);
					ModeManager::AddChannelMode(new InspIRCdExtban::EntryMatcher("STRIPCOLORBAN", "BAN", 'S'));
				}
				else if (modename.equals_cs("topiclock"))
					cm = new ChannelMode("TOPIC", modechar[0]);
				else if (modename.equals_cs("voice"))
					cm = new ChannelModeStatus("VOICE", modechar.length() > 1 ? modechar[1] : modechar[0], modechar.length() > 1 ? modechar[0] : 0, 0);
				/* Unknown status mode, (customprefix) - add it */
				else if (modechar.length() == 2)
					cm = new ChannelModeStatus(modename.upper(), modechar[1], modechar[0], -1);
				/* Unknown non status mode, add it to our list for later */
				else
					chmodes[modechar[0]] = modename.upper();

				if (cm)
					ModeManager::AddChannelMode(cm);
			}
		}
		if (params[0].equals_cs("USERMODES") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string capab;

			while (ssep.GetToken(capab))
			{
				Anope::string modename = capab.substr(0, capab.find('='));
				Anope::string modechar = capab.substr(capab.find('=') + 1);
				UserMode *um = NULL;

				if (modename.equals_cs("bot"))
					um = new UserMode("BOT", modechar[0]);
				else if (modename.equals_cs("callerid"))
					um = new UserMode("CALLERID", modechar[0]);
				else if (modename.equals_cs("cloak"))
					um = new UserMode("CLOAK", modechar[0]);
				else if (modename.equals_cs("deaf"))
					um = new UserMode("DEAF", modechar[0]);
				else if (modename.equals_cs("deaf_commonchan"))
					um = new UserMode("COMMONCHANS", modechar[0]);
				else if (modename.equals_cs("helpop"))
					um = new UserModeOperOnly("HELPOP", modechar[0]);
				else if (modename.equals_cs("hidechans"))
					um = new UserMode("PRIV", modechar[0]);
				else if (modename.equals_cs("hideoper"))
					um = new UserModeOperOnly("HIDEOPER", modechar[0]);
				else if (modename.equals_cs("invisible"))
					um = new UserMode("INVIS", modechar[0]);
				else if (modename.equals_cs("invis-oper"))
					um = new UserModeOperOnly("INVISIBLE_OPER", modechar[0]);
				else if (modename.equals_cs("oper"))
					um = new UserModeOperOnly("OPER", modechar[0]);
				else if (modename.equals_cs("regdeaf"))
					um = new UserMode("REGPRIV", modechar[0]);
				else if (modename.equals_cs("servprotect"))
				{
					um = new UserModeNoone("PROTECTED", modechar[0]);
					IRCD->DefaultPseudoclientModes += modechar;
				}
				else if (modename.equals_cs("showwhois"))
					um = new UserMode("WHOIS", modechar[0]);
				else if (modename.equals_cs("u_censor"))
					um = new UserMode("CENSOR", modechar[0]);
				else if (modename.equals_cs("u_registered"))
					um = new UserModeNoone("REGISTERED", modechar[0]);
				else if (modename.equals_cs("u_stripcolor"))
					um = new UserMode("STRIPCOLOR", modechar[0]);
				else if (modename.equals_cs("wallops"))
					um = new UserMode("WALLOPS", modechar[0]);
				else
					umodes[modechar[0]] = modename.upper();

				if (um)
					ModeManager::AddUserMode(um);
			}
		}
		else if (params[0].equals_cs("MODULES") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string module;

			while (ssep.GetToken(module))
			{
				if (module.equals_cs("m_svshold.so"))
					IRCD->CanSVSHold = true;
				else if (module.find("m_rline.so") == 0)
				{
					Servers::Capab.insert("RLINE");
					const Anope::string &regexengine = Config->GetBlock("options")->Get<Anope::string>("regexengine");
					if (!regexengine.empty() && module.length() > 11 && regexengine != module.substr(11))
						Log() << "Warning: InspIRCd is using regex engine " << module.substr(11) << ", but we have " << regexengine << ". This may cause inconsistencies.";
				}
				else if (module.equals_cs("m_topiclock.so"))
					Servers::Capab.insert("TOPICLOCK");
			}
		}
		else if (params[0].equals_cs("MODSUPPORT") && params.size() > 1)
		{
			spacesepstream ssep(params[1]);
			Anope::string module;

			while (ssep.GetToken(module))
			{
				if (module.equals_cs("m_services_account.so"))
				{
					Servers::Capab.insert("SERVICES");
					ModeManager::AddChannelMode(new InspIRCdExtban::AccountMatcher("ACCOUNTBAN", "BAN", 'R'));
					ModeManager::AddChannelMode(new InspIRCdExtban::UnidentifiedMatcher("UNREGISTEREDBAN", "BAN", 'U'));
				}
				else if (module.equals_cs("m_chghost.so"))
					Servers::Capab.insert("CHGHOST");
				else if (module.equals_cs("m_chgident.so"))
					Servers::Capab.insert("CHGIDENT");
				else if (module == "m_channelban.so")
					ModeManager::AddChannelMode(new InspIRCdExtban::ChannelMatcher("CHANNELBAN", "BAN", 'j'));
				else if (module == "m_gecosban.so")
					ModeManager::AddChannelMode(new InspIRCdExtban::RealnameMatcher("REALNAMEBAN", "BAN", 'r'));
				else if (module == "m_nopartmessage.so")
					ModeManager::AddChannelMode(new InspIRCdExtban::EntryMatcher("PARTMESSAGEBAN", "BAN", 'p'));
				else if (module == "m_serverban.so")
					ModeManager::AddChannelMode(new InspIRCdExtban::ServerMatcher("SERVERBAN", "BAN", 's'));
				else if (module == "m_muteban.so")
					ModeManager::AddChannelMode(new InspIRCdExtban::EntryMatcher("QUIET", "BAN", 'm'));
			}
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
						if (ModeManager::FindChannelModeByChar(modebuf[t]))
							continue;
						ModeManager::AddChannelMode(new ChannelModeList(chmodes[modebuf[t]], modebuf[t]));
					}

					sep.GetToken(modebuf);
					for (size_t t = 0, end = modebuf.length(); t < end; ++t)
					{
						if (ModeManager::FindChannelModeByChar(modebuf[t]))
							continue;
						ModeManager::AddChannelMode(new ChannelModeParam(chmodes[modebuf[t]], modebuf[t]));
					}

					sep.GetToken(modebuf);
					for (size_t t = 0, end = modebuf.length(); t < end; ++t)
					{
						if (ModeManager::FindChannelModeByChar(modebuf[t]))
							continue;
						ModeManager::AddChannelMode(new ChannelModeParam(chmodes[modebuf[t]], modebuf[t], true));
					}

					sep.GetToken(modebuf);
					for (size_t t = 0, end = modebuf.length(); t < end; ++t)
					{
						if (ModeManager::FindChannelModeByChar(modebuf[t]))
							continue;
						ModeManager::AddChannelMode(new ChannelMode(chmodes[modebuf[t]], modebuf[t]));
					}
				}
				else if (capab.find("USERMODES") != Anope::string::npos)
				{
					Anope::string modes(capab.begin() + 10, capab.end());
					commasepstream sep(modes);
					Anope::string modebuf;

					sep.GetToken(modebuf);
					sep.GetToken(modebuf);

					if (sep.GetToken(modebuf))
						for (size_t t = 0, end = modebuf.length(); t < end; ++t)
							ModeManager::AddUserMode(new UserModeParam(umodes[modebuf[t]], modebuf[t]));

					if (sep.GetToken(modebuf))
						for (size_t t = 0, end = modebuf.length(); t < end; ++t)
							ModeManager::AddUserMode(new UserMode(umodes[modebuf[t]], modebuf[t]));
				}
				else if (capab.find("MAXMODES=") != Anope::string::npos)
				{
					Anope::string maxmodes(capab.begin() + 9, capab.end());
					IRCD->MaxModes = maxmodes.is_pos_number_only() ? convertTo<unsigned>(maxmodes) : 3;
				}
				else if (capab.find("PREFIX=") != Anope::string::npos)
				{
					Anope::string modes(capab.begin() + 8, capab.begin() + capab.find(')'));
					Anope::string chars(capab.begin() + capab.find(')') + 1, capab.end());
					short level = modes.length() - 1;

					for (size_t t = 0, end = modes.length(); t < end; ++t)
					{
						ChannelMode *cm = ModeManager::FindChannelModeByChar(modes[t]);
						if (cm == NULL || cm->type != MODE_STATUS)
						{
							Log() << "CAPAB PREFIX gave unknown channel status mode " << modes[t];
							continue;
						}

						ChannelModeStatus *cms = anope_dynamic_static_cast<ChannelModeStatus *>(cm);
						cms->level = level--;

						Log(LOG_DEBUG) << cms->name << " is now level " << cms->level;
					}

					ModeManager::RebuildStatusModes();
				}
				else if (capab == "GLOBOPS=1")
					Servers::Capab.insert("GLOBOPS");
			}
		}
		else if (params[0].equals_cs("END"))
		{
			if (!Servers::Capab.count("SERVICES"))
			{
				UplinkSocket::Message() << "ERROR :m_services_account.so is not loaded. This is required by Anope";
				Anope::QuitReason = "ERROR: Remote server does not have the m_services_account module loaded, and this is required.";
				Anope::Quitting = true;
				return;
			}
			if (!ModeManager::FindUserModeByName("PRIV"))
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

			chmodes.clear();
			umodes.clear();
		}

		Message::Capab::Run(source, params);
	}
};

struct IRCDMessageEncap : IRCDMessage
{
	IRCDMessageEncap(Module *creator) : IRCDMessage(creator, "ENCAP", 4) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::Match(Me->GetSID(), params[0]) == false)
			return;

		if (params[1] == "CHGIDENT")
		{
			User *u = User::Find(params[2]);
			if (!u || u->server != Me)
				return;

			u->SetIdent(params[3]);
			UplinkSocket::Message(u) << "FIDENT " << params[3];
		}
		else if (params[1] == "CHGHOST")
		{
			User *u = User::Find(params[2]);
			if (!u || u->server != Me)
				return;

			u->SetDisplayedHost(params[3]);
			UplinkSocket::Message(u) << "FHOST " << params[3];
		}
		else if (params[1] == "CHGNAME")
		{
			User *u = User::Find(params[2]);
			if (!u || u->server != Me)
				return;

			u->SetRealname(params[3]);
			UplinkSocket::Message(u) << "FNAME " << params[3];
		}
	}
};

struct IRCDMessageEndburst : IRCDMessage
{
	IRCDMessageEndburst(Module *creator) : IRCDMessage(creator, "ENDBURST", 0) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		Server *s = source.GetServer();

		Log(LOG_DEBUG) << "Processed ENDBURST for " << s->GetName();

		s->Sync(true);
	}
};

struct IRCDMessageFHost : IRCDMessage
{
	IRCDMessageFHost(Module *creator) : IRCDMessage(creator, "FHOST", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		source.GetUser()->SetDisplayedHost(params[0]);
	}
};

struct IRCDMessageFIdent : IRCDMessage
{
	IRCDMessageFIdent(Module *creator) : IRCDMessage(creator, "FIDENT", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		source.GetUser()->SetIdent(params[0]);
	}
};

struct IRCDMessageFJoin : IRCDMessage
{
	IRCDMessageFJoin(Module *creator) : IRCDMessage(creator, "FJOIN", 2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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
				Log(LOG_DEBUG) << "FJOIN for nonexistant user " << buf << " on " << params[0];
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		/* :source FTOPIC channel topicts setby :topic */

		Channel *c = Channel::Find(params[0]);
		if (c)
			c->ChangeTopicInternal(params[2], params[3], Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime);
	}
};

struct IRCDMessageIdle : IRCDMessage
{
	IRCDMessageIdle(Module *creator) : IRCDMessage(creator, "IDLE", 1) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		ServiceBot *bi = ServiceBot::Find(params[0]);
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (isdigit(params[0][0]))
		{
			if (params[1].equals_cs("accountname"))
			{
				User *u = User::Find(params[0]);
				NickServ::Account *nc = NickServ::FindAccount(params[2]);
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
				u->Extend<bool>("ssl", true);
				Anope::string data = params[2].c_str();
				size_t pos1 = data.find(' ') + 1;
				size_t pos2 = data.find(' ', pos1);
				if ((pos2 - pos1) >= 32) // inspircd supports md5 and sha1 fingerprint hashes -> size 32 or 40 bytes.
				{
					u->fingerprint = data.substr(pos1, pos2 - pos1);
				}
				Event::OnFingerprint(&Event::Fingerprint::OnFingerprint, u);
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		source.GetUser()->ChangeNick(params[0]);
	}
};

struct IRCDMessageOperType : IRCDMessage
{
	IRCDMessageOperType(Module *creator) : IRCDMessage(creator, "OPERTYPE", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		/* opertype is equivalent to mode +o because servers
		   dont do this directly */
		User *u = source.GetUser();
		if (!u->HasMode("OPER"))
			u->SetModesInternal(source, "+o");
	}
};

struct IRCDMessageRSQuit : IRCDMessage
{
	IRCDMessageRSQuit(Module *creator) : IRCDMessage(creator, "RSQUIT", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		Server *s = Server::Find(params[0]);
		const Anope::string &reason = params.size() > 1 ? params[1] : "";
		if (!s)
			return;

		UplinkSocket::Message(Me) << "SQUIT " << s->GetSID() << " :" << reason;
		s->Delete(s->GetName() + " " + s->GetUplink()->GetName());
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		unsigned int hops = Anope::string(params[2]).is_pos_number_only() ? convertTo<unsigned>(params[2]) : 0;
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params[4], params[3]);
	}
};

struct IRCDMessageSQuit : Message::SQuit
{
	IRCDMessageSQuit(Module *creator) : Message::SQuit(creator) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		time_t ts = convertTo<time_t>(params[1]);

		Anope::string modes = params[8];
		for (unsigned i = 9; i < params.size() - 1; ++i)
			modes += " " + params[i];

		NickServ::Nick *na = NULL;
		if (SASL::sasl)
			for (std::list<SASLUser>::iterator it = saslusers.begin(); it != saslusers.end();)
			{
				SASLUser &u = *it;

				if (u.created + 30 < Anope::CurTime)
					it = saslusers.erase(it);
				else if (u.uid == params[0])
				{
					na = NickServ::FindNick(u.acc);
					it = saslusers.erase(it);
				}
				else
					++it;
			}

		User *u = User::OnIntroduce(params[2], params[5], params[3], params[4], params[6], source.GetServer(), params[params.size() - 1], ts, modes, params[0], na ? na->GetAccount() : NULL);
		if (u)
			u->signon = convertTo<time_t>(params[7]);
	}
};

class ProtoInspIRCd20 : public Module
	, public EventHook<Event::UserNickChange>
	, public EventHook<Event::ChannelSync>
	, public EventHook<Event::ChanRegistered>
	, public EventHook<Event::DelChan>
	, public EventHook<Event::MLockEvents>
	, public EventHook<Event::SetChannelOption>
	, public EventHook<Event::ChannelModeUnset>
{
	InspIRCd20Proto ircd_proto;
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
	IRCDMessageCapab message_capab;
	IRCDMessageEncap message_encap;
	IRCDMessageEndburst message_endburst;
	IRCDMessageFHost message_fhost;
	IRCDMessageFIdent message_fident;
	IRCDMessageFJoin message_fjoin;
	IRCDMessageFMode message_fmode;
	IRCDMessageFTopic message_ftopic;
	IRCDMessageIdle message_idle;
	IRCDMessageMetadata message_metadata;
	IRCDMessageMode message_mode;
	IRCDMessageNick message_nick;
	IRCDMessageOperType message_opertype;
	IRCDMessageRSQuit message_rsquit;
	IRCDMessageServer message_server;
	IRCDMessageSQuit message_squit;
	IRCDMessageTime message_time;
	IRCDMessageUID message_uid;

	bool use_server_side_topiclock, use_server_side_mlock;

	void SendChannelMetadata(Channel *c, const Anope::string &metadataname, const Anope::string &value)
	{
		UplinkSocket::Message(Me) << "METADATA " << c->name << " " << metadataname << " :" << value;
	}

 public:
	ProtoInspIRCd20(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR)
		, EventHook<Event::UserNickChange>()
		, EventHook<Event::ChannelSync>()
		, EventHook<Event::ChanRegistered>()
		, EventHook<Event::DelChan>()
		, EventHook<Event::MLockEvents>()
		, EventHook<Event::SetChannelOption>()
		, EventHook<Event::ChannelModeUnset>()
		, ircd_proto(this)
		, ssl(this, "ssl")
		, message_away(this)
		, message_error(this)
		, message_invite(this)
		, message_join(this)
		, message_kick(this)
		, message_kill(this)
		, message_motd(this)
		, message_notice(this)
		, message_part(this)
		, message_ping(this)
		, message_privmsg(this)
		, message_quit(this)
		, message_stats(this)
		, message_topic(this)

		, message_capab(this)
		, message_encap(this)
		, message_endburst(this)
		, message_fhost(this)
		, message_fident(this)
		, message_fjoin(this)
		, message_fmode(this)
		, message_ftopic(this)
		, message_idle(this)
		, message_metadata(this)
		, message_mode(this)
		, message_nick(this)
		, message_opertype(this)
		, message_rsquit(this)
		, message_server(this)
		, message_squit(this)
		, message_time(this)
		, message_uid(this)
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		use_server_side_topiclock = conf->GetModule(this)->Get<bool>("use_server_side_topiclock");
		use_server_side_mlock = conf->GetModule(this)->Get<bool>("use_server_side_mlock");
	}

	void OnUserNickChange(User *u, const Anope::string &) override
	{
		u->RemoveModeInternal(Me, ModeManager::FindUserModeByName("REGISTERED"));
	}

	void OnChannelSync(Channel *c) override
	{
		if (c->ci)
			this->OnChanRegistered(c->ci);
	}

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		if (use_server_side_mlock && ci->c && mlocks && !mlocks->GetMLockAsString(ci, false).empty())
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "");
			SendChannelMetadata(ci->c, "mlock", modes);
		}

		if (use_server_side_topiclock && Servers::Capab.count("TOPICLOCK") && ci->c)
		{
			if (ci->HasFieldS("TOPICLOCK"))
				SendChannelMetadata(ci->c, "topiclock", "1");
		}
	}

	void OnDelChan(ChanServ::Channel *ci) override
	{
		if (use_server_side_mlock && ci->c)
			SendChannelMetadata(ci->c, "mlock", "");

		if (use_server_side_topiclock && Servers::Capab.count("TOPICLOCK") && ci->c)
			SendChannelMetadata(ci->c, "topiclock", "");
	}

	EventReturn OnMLock(ChanServ::Channel *ci, ModeLock *lock) override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->GetName());
		if (use_server_side_mlock && cm && ci->c && mlocks && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM))
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->mchar;
			SendChannelMetadata(ci->c, "mlock", modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChanServ::Channel *ci, ModeLock *lock) override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->GetName());
		if (use_server_side_mlock && cm && ci->c && mlocks && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM))
		{
			Anope::string modes = mlocks->GetMLockAsString(ci, false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->mchar, "");
			SendChannelMetadata(ci->c, "mlock", modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnSetChannelOption(CommandSource &source, Command *cmd, ChanServ::Channel *ci, const Anope::string &setting) override
	{
		if (cmd->name == "chanserv/topic" && ci->c)
		{
			if (setting == "topiclock on")
				SendChannelMetadata(ci->c, "topiclock", "1");
			else if (setting == "topiclock off")
				SendChannelMetadata(ci->c, "topiclock", "0");
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnChannelModeUnset(Channel *c, const MessageSource &setter, ChannelMode *mode, const Anope::string &param) override
	{
		if ((setter.GetUser() && setter.GetUser()->server == Me) || setter.GetServer() == Me || !setter.GetServer())
			if (mode->name == "OPERPREFIX")
				c->SetMode(c->ci->WhoSends(), mode, param, false);

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ProtoInspIRCd20)
