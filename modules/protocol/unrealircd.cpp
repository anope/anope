/* UnrealIRCd functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/cs_mode.h"
#include "modules/sasl.h"

typedef Anope::map<Anope::string> ModData;
static Anope::string UplinkSID;

class UnrealIRCdProto final
	: public IRCDProto
{
public:
	PrimitiveExtensibleItem<ModData> ClientModData;
	PrimitiveExtensibleItem<ModData> ChannelModData;

	UnrealIRCdProto(Module *creator)
		: IRCDProto(creator, "UnrealIRCd 6+")
		, ClientModData(creator, "ClientModData")
		, ChannelModData(creator, "ChannelModData")
	{
		DefaultPseudoclientModes = "+BioqS";
		CanSVSNick = true;
		CanSVSJoin = true;
		CanSVSNOOP = true;
		CanSetVHost = true;
		CanSetVIdent = true;
		CanSNLine = true;
		CanSQLine = true;
		CanSQLineChannel = true;
		CanSZLine = true;
		CanSVSHold = true;
		CanClearBans = true;
		CanCertFP = true;
		CanTagMessage = true;
		RequiresID = true;
		MaxModes = 12;
	}

private:
	/* SVSNOOP */
	void SendSVSNOOP(const Server *server, bool set) override
	{
		Uplink::Send("SVSNOOP", server->GetSID(), set ? '+' : '-');
	}

	void SendAkillDel(const XLine *x) override
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

		Uplink::Send("TKL", '-', 'G', x->GetUser(), x->GetHost(), x->by);
	}

	void SendTopic(const MessageSource &source, Channel *c) override
	{
		Uplink::Send(source, "TOPIC", c->name, c->topic_setter, c->topic_ts, c->topic);
	}

	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "NOTICE", "$" + dest->GetName(), msg);
	}

	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "PRIVMSG", "$" + dest->GetName(), msg);
	}

	void SendVHostDel(User *u) override
	{
		BotInfo *HostServ = Config->GetClient("HostServ");
		u->RemoveMode(HostServ, "VHOST");
	}

	void SendAkill(User *u, XLine *x) override
	{
		if (x->IsRegex() || x->HasNickOrReal())
		{
			if (!u)
			{
				/* No user (this akill was just added), and contains nick and/or realname. Find users that match and ban them */
				for (const auto &[_, user] : UserListByNick)
					if (x->manager->Check(user, x))
						this->SendAkill(user, x);
				return;
			}

			const XLine *old = x;

			if (old->manager->HasEntry("*@" + u->host))
				return;

			/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
			auto *xline = new XLine("*@" + u->host, old->by, old->expires, old->reason, old->id);
			old->manager->AddXLine(xline);
			x = xline;

			Log(Config->GetClient("OperServ"), "akill") << "AKILL: Added an akill for " << x->mask << " because " << u->GetMask() << "#" << u->realname << " matches " << old->mask;
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

		Uplink::Send("TKL", '+', 'G', x->GetUser(), x->GetHost(), x->by, x->expires, x->created, x->GetReason());
	}

	void SendSVSKill(const MessageSource &source, User *user, const Anope::string &buf) override
	{
		Uplink::Send(source, "SVSKILL", user->GetUID(), buf);
		user->KillInternal(source, buf);
	}

	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &modes, const std::vector<Anope::string> &values) override
	{
		auto params = values;
		params.insert(params.begin(), { u->GetUID(), modes });
		Uplink::SendInternal({}, source, "SVS2MODE", params);
}

	void SendClientIntroduction(User *u) override
	{
		Uplink::Send(u->server, "UID", u->nick, 1, u->timestamp, u->GetIdent(), u->host, u->GetUID(), '*', "+" + u->GetModes(),
			u->vhost.empty() ? "*" : u->vhost, u->chost.empty() ? "*" : u->chost, "*", u->realname);
	}

	void SendServer(const Server *server) override
	{
		if (server == Me)
			Uplink::Send("SERVER", server->GetName(), server->GetHops() + 1, server->GetDescription());
		else
			Uplink::Send("SID", server->GetName(), server->GetHops() + 1, server->GetSID(), server->GetDescription());
	}

	/* JOIN */
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override
	{
		Uplink::Send("SJOIN", c->creation_time, c->name, "+" + c->GetModes(true, true), user->GetUID());
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
			for (auto mode : cs.Modes())
				c->SetMode(setter, ModeManager::FindChannelModeByChar(mode), user->GetUID(), false);

			if (uc != NULL)
				uc->status = cs;
		}
	}

	/* unsqline
	*/
	void SendSQLineDel(const XLine *x) override
	{
		Uplink::Send("UNSQLINE", x->mask);
	}

	/* SQLINE */
	/*
	** - Unreal will translate this to TKL for us
	**
	*/
	void SendSQLine(User *, const XLine *x) override
	{
		Uplink::Send("TKL", '+', 'Q', "*", x->mask, x->by, x->expires, x->created, x->GetReason());
	}

	/* Functions that use serval cmd functions */

	void SendVHost(User *u, const Anope::string &vident, const Anope::string &vhost) override
	{
		if (!vident.empty())
			Uplink::Send("CHGIDENT", u->GetUID(), vident);

		if (!vhost.empty())
			Uplink::Send("CHGHOST", u->GetUID(), vhost);

		// Internally unreal sets +xt on chghost
		BotInfo *bi = Config->GetClient("HostServ");
		u->SetMode(bi, "CLOAK");
		u->SetMode(bi, "VHOST");
	}

	void SendConnect() override
	{
		Uplink::Send("PASS", Config->Uplinks[Anope::CurrentUplink].password);

		// BIGLINES: enable sending lines up to 16384 characters in length.
		// EAUTH: communicates information about the local server.
		// MLOCK: enable receiving the MLOCK message when a mode lock changes.
		// MTAGS: enable receiving IRCv3 message tags.
		// NEXTBANS: enables receiving named extended bans.
		// SID: communicates the unique identifier of the local server.
		// VHP: enable receiving the vhost in UID.
		Uplink::Send("PROTOCTL", "BIGLINES", "MLOCK", "MTAGS", "NEXTBANS", "VHP");
		Uplink::Send("PROTOCTL", "EAUTH=" + Me->GetName() + ",,,Anope-" + Anope::VersionShort());
		Uplink::Send("PROTOCTL", "SID=" + Me->GetSID());

		SendServer(Me);
	}

	void SendSASLMechanisms(std::vector<Anope::string> &mechanisms) override
	{
		Anope::string mechlist;
		for (const auto &mechanism : mechanisms)
			mechlist += "," + mechanism;

		Uplink::Send("MD", "client", Me->GetName(), "saslmechlist", mechanisms.empty() ? "" : mechlist.substr(1));
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick, time_t t) override
	{
		Uplink::Send("TKL", '+', 'Q', 'H', nick, Me->GetName(), Anope::CurTime + t, Anope::CurTime, "Being held for a registered user");
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) override
	{
		Uplink::Send("TKL", '-', 'Q', '*', nick, Me->GetName());
	}

	/* UNSGLINE */
	/*
	 * SVSNLINE - :realname mask
	*/
	void SendSGLineDel(const XLine *x) override
	{
		Uplink::Send("SVSNLINE", '-', x->mask);
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x) override
	{
		Uplink::Send("TKL", '-', 'Z', '*', x->GetHost(), x->by);
	}

	/* SZLINE */
	void SendSZLine(User *, const XLine *x) override
	{
		Uplink::Send("TKL", '+', 'Z', '*', x->GetHost(), x->by, x->expires, x->created, x->GetReason());
	}

	/* SGLINE */
	/*
	 * SVSNLINE + reason_where_is_space :realname mask with spaces
	*/
	void SendSGLine(User *, const XLine *x) override
	{
		Anope::string edited_reason = x->GetReason();
		edited_reason = edited_reason.replace_all_cs(" ", "_");
		Uplink::Send("SVSNLINE", '+', edited_reason, x->mask);
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
	void SendSVSJoin(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &key) override
	{
		if (key.empty())
			Uplink::Send("SVSJOIN", user->GetUID(), chan);
		else
			Uplink::Send("SVSJOIN", user->GetUID(), chan, key);
	}

	void SendSVSPart(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) override
	{
		if (!param.empty())
			Uplink::Send("SVSPART", user->GetUID(), chan, param);
		else
			Uplink::Send("SVSPART", user->GetUID(), chan);
	}

	void SendGlobops(const MessageSource &source, const Anope::string &buf) override
	{
		Uplink::Send("SENDUMODE", 'o', "From " + source.GetName() + ": " + buf);
	}

	void SendSWhois(const MessageSource &source, const Anope::string &who, const Anope::string &mask) override
	{
		Uplink::Send("SWHOIS", who, mask);
	}

	void SendEOB() override
	{
		Uplink::Send("EOS");
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

	void SendLogin(User *u, NickAlias *na) override
	{
		if (!na->nc->HasExt("UNCONFIRMED"))
			IRCD->SendMode(Config->GetClient("NickServ"), u, "+d", na->nc->display);
	}

	void SendLogout(User *u) override
	{
		IRCD->SendMode(Config->GetClient("NickServ"), u, "+d", 0);
	}

	void SendChannel(Channel *c) override
	{
		Uplink::Send("SJOIN", c->creation_time, c->name, "+" + c->GetModes(true, true), "");
	}

	void SendSASLMessage(const SASL::Message &message) override
	{
		size_t p = message.target.find('!');
		Anope::string distmask;

		if (p == Anope::string::npos)
		{
			Server *s = Server::Find(message.target.substr(0, 3));
			if (!s)
				return;
			distmask = s->GetName();
		}
		else
		{
			distmask = message.target.substr(0, p);
		}

		if (message.ext.empty())
			Uplink::Send(BotInfo::Find(message.source), "SASL", distmask, message.target, message.type, message.data);
		else
			Uplink::Send(BotInfo::Find(message.source), "SASL", distmask, message.target, message.type, message.data, message.ext);
	}

	void SendSVSLogin(const Anope::string &uid, NickAlias *na) override
	{
		size_t p = uid.find('!');
		Anope::string distmask;

		if (p == Anope::string::npos)
		{
			Server *s = Server::Find(uid.substr(0, 3));
			if (!s)
				return;
			distmask = s->GetName();
		}
		else
		{
			distmask = uid.substr(0, p);
		}

		if (na)
		{
			if (!na->GetVHostIdent().empty())
				Uplink::Send("CHGIDENT", uid, na->GetVHostIdent());

			if (!na->GetVHostHost().empty())
				Uplink::Send("CHGHOST", uid, na->GetVHostHost());
		}

		Uplink::Send("SVSLOGIN", distmask, uid, na ? na->nc->display : "0");
	}

	bool IsIdentValid(const Anope::string &ident) override
	{
		if (ident.empty() || ident.length() > IRCD->MaxUser)
			return false;

		for (auto c : ident)
		{
			if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-')
				continue;

			if (c == '-' || c == '.' || c == '_')
				continue;

			return false;
		}

		return true;
	}

	void SendClearBans(const MessageSource &user, Channel *c, User* u) override
	{
		Uplink::Send(user, "SVS2MODE", c->name, "-b", u->GetUID());
	}

	bool IsTagValid(const Anope::string &tname, const Anope::string &tvalue) override
	{
		return !!Servers::Capab.count("MTAGS");
	}
};

namespace UnrealExtBan
{
	bool IsExtBan(const Anope::string &str, Anope::string &name, Anope::string &value)
	{
		if (str[0] != '~')
			return false;

		auto endpos = str.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 1);
		if (endpos == Anope::string::npos || str[endpos] != ':' || endpos+1 == str.length())
			return false;

		name = str.substr(1, endpos - 1);
		value = str.substr(endpos + 1);
		return true;
	}

	class Base
		: public ChannelModeVirtual<ChannelModeList>
	{
	private:
		char xbchar;
		Anope::string xbname;

	public:
		Base(const Anope::string &mname, const Anope::string& uname, char uchar)
			: ChannelModeVirtual<ChannelModeList>(mname, "BAN")
			, xbchar(uchar)
			, xbname(uname)
		{
		}

		ChannelMode *Wrap(Anope::string &param) override
		{
			auto prefix = Servers::Capab.count("NEXTBANS") ? xbname : Anope::string(xbchar);
			param = Anope::printf("~%s:%s", prefix.c_str(), param.c_str());
			return ChannelModeVirtual<ChannelModeList>::Wrap(param);
		}

		ChannelMode *Unwrap(ChannelMode *cm, Anope::string &param) override
		{
			// The mask must be in the format ~<letter>:<value> or ~<name>:<value>.
			if (cm->type != MODE_LIST)
				return cm;

			Anope::string name, value;
			if (!IsExtBan(param, name, value))
				return cm;

			if (name.length() == 1 ? name[0] != xbchar : name != xbname)
				return cm;

			param = value;
			return this;

		}
	};

	class ChannelMatcher final
		: public Base
	{
	public:
		ChannelMatcher()
			: Base("CHANNELBAN", "channel", 'c')
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			auto channel = e->GetMask();
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

	class EntryMatcher final
		: public Base
	{
	public:
		EntryMatcher(const Anope::string &mname, const Anope::string &uname, char uchar)
			: Base(mname, uname, uchar)
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			return Entry(this->name, e->GetMask()).Matches(u);
		}
	};

	class RealnameMatcher final
		: public Base
	{
	public:
		RealnameMatcher()
			: Base("REALNAMEBAN", "realname", 'r')
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			return Anope::Match(u->realname, e->GetMask());
		}
	};

	class AccountMatcher final
		: public Base
	{
	public:
		AccountMatcher()
			: Base("ACCOUNTBAN", "account", 'a')
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			if (e->GetMask() == "0" && !u->Account()) /* ~a:0 is special and matches all unauthenticated users */
				return true;

			return u->Account() && Anope::Match(u->Account()->display, e->GetMask());
		}
	};

	class FingerprintMatcher final
		: public Base
	{
	public:
		FingerprintMatcher()
			: Base("SSLBAN", "certfp", 'S')
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			return !u->fingerprint.empty() && Anope::Match(u->fingerprint, e->GetMask());
		}
	};

	class OperclassMatcher final
		: public Base
	{
	public:
		OperclassMatcher()
			: Base("OPERCLASSBAN", "operclass", 'O')
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			ModData *moddata = u->GetExt<ModData>("ClientModData");
			return moddata != NULL && moddata->find("operclass") != moddata->end() && Anope::Match((*moddata)["operclass"], e->GetMask());
		}
	};

	class TimedBanMatcher final
		: public Base
	{
	public:
		TimedBanMatcher()
			: Base("TIMEDBAN", "time", 't')
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			/* strip down the time (~t:1234:) and call other matchers */
			auto real_mask = e->GetMask();
			real_mask = real_mask.substr(real_mask.find(":") + 1);
			return Entry("BAN", real_mask).Matches(u);
		}
	};

	class CountryMatcher final
		: public Base
	{
	public:
	 	CountryMatcher()
	 		: Base("COUNTRYBAN", "country", 'C')
		{
		}

		bool Matches(User *u, const Entry *e) override
		{
			ModData *moddata = u->GetExt<ModData>("ClientModData");
			if (moddata == NULL || moddata->find("geoip") == moddata->end())
				return false;

			sepstream sep((*moddata)["geoip"], '|');/* "cc=PL|cd=Poland" */
			Anope::string tokenbuf;
			while (sep.GetToken(tokenbuf))
			{
				if (tokenbuf.rfind("cc=", 0) == 0)
					return (tokenbuf.substr(3, 2) == e->GetMask());
			}
			return false;
		}
	};
}

class ChannelModeFlood final
	: public ChannelModeParam
{
public:
	ChannelModeFlood(char modeChar, bool minusNoArg) : ChannelModeParam("FLOOD", modeChar, minusNoArg) { }

	/* Borrowed part of this check from UnrealIRCd */
	bool IsValid(Anope::string &value) const override
	{
		if (value.empty() || value[0] != '[')
			return false;

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
			if (p == arg.length() || (arg[p] != 'c' && arg[p] != 'j' && arg[p] != 'k' && arg[p] != 'm' && arg[p] != 'n' && arg[p] != 't'))
				continue; /* continue instead of break for forward compatibility. */

			auto v = Anope::Convert<int>(arg.substr(0, p), 0);
			if (v < 1 || v > 999)
				return false;
		}

		return true;
	}
};

class ChannelModeHistory final
	: public ChannelModeParam
{
public:
	ChannelModeHistory(char modeChar) : ChannelModeParam("HISTORY", modeChar, true) { }

	bool IsValid(Anope::string &value) const override
	{
		if (value.empty())
			return false; // empty param is never valid

		Anope::string::size_type pos = value.find(':');
		if ((pos == Anope::string::npos) || (pos == 0))
			return false; // no ':' or it's the first char, both are invalid

		Anope::string rest;
		if (Anope::Convert<int>(value, 0, &rest) <= 0)
			return false; // negative numbers and zero are invalid

		// The part after the ':' is a duration and it
		// can be in the user friendly "1d3h20m" format, make sure we accept that
		auto n = Anope::DoTime(rest.substr(1));
		return n > 0;
	}
};

class ChannelModeUnrealSSL final
	: public ChannelMode
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

struct IRCDMessageCapab final
	: Message::Capab
{
	IRCDMessageCapab(Module *creator) : Message::Capab(creator, "PROTOCTL") { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		for (const auto &capab : params)
		{
			if (capab.find("USERMODES=") != Anope::string::npos)
			{
				Anope::string modebuf(capab.begin() + 10, capab.end());
				for (auto mode : modebuf)
				{
					switch (mode)
					{
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
							ModeManager::AddUserMode(new UserModeOperOnly("HIDEIDLE", 'I'));
							continue;
						case 'R':
							ModeManager::AddUserMode(new UserMode("REGPRIV", 'R'));
							continue;
						case 'S':
							ModeManager::AddUserMode(new UserModeOperOnly("PROTECTED", 'S'));
							continue;
						case 'T':
							ModeManager::AddUserMode(new UserMode("NOCTCP", 'T'));
							continue;
						case 'W':
							ModeManager::AddUserMode(new UserModeOperOnly("WHOIS", 'W'));
							continue;
						case 'd':
							ModeManager::AddUserMode(new UserMode("DEAF", 'd'));
							continue;
						case 'D':
							ModeManager::AddUserMode(new UserMode("PRIVDEAF", 'D'));
							continue;
						case 'i':
							ModeManager::AddUserMode(new UserMode("INVIS", 'i'));
							continue;
						case 'o':
							ModeManager::AddUserMode(new UserModeOperOnly("OPER", 'o'));
							continue;
						case 'p':
							ModeManager::AddUserMode(new UserMode("PRIV", 'p'));
							continue;
						case 'q':
							ModeManager::AddUserMode(new UserModeOperOnly("GOD", 'q'));
							continue;
						case 'r':
							ModeManager::AddUserMode(new UserModeNoone("REGISTERED", 'r'));
							continue;
						case 's':
							ModeManager::AddUserMode(new UserModeOperOnly("SNOMASK", 's'));
							continue;
						case 't':
							ModeManager::AddUserMode(new UserModeNoone("VHOST", 't'));
							continue;
						case 'w':
							ModeManager::AddUserMode(new UserMode("WALLOPS", 'w'));
							continue;
						case 'x':
							ModeManager::AddUserMode(new UserMode("CLOAK", 'x'));
							continue;
						case 'z':
							ModeManager::AddUserMode(new UserModeNoone("SSL", 'z'));
							continue;
						case 'Z':
							ModeManager::AddUserMode(new UserMode("SSLPRIV", 'Z'));
							continue;
						default:
							ModeManager::AddUserMode(new UserMode("", mode));
					}
				}
			}
			else if (capab.find("CHANMODES=") != Anope::string::npos)
			{
				Anope::string modes(capab.begin() + 10, capab.end());
				commasepstream sep(modes);
				Anope::string modebuf;

				sep.GetToken(modebuf);
				for (auto mode : modebuf)
				{
					switch (mode)
					{
						case 'b':
							ModeManager::AddChannelMode(new ChannelModeList("BAN", 'b'));

							ModeManager::AddChannelMode(new UnrealExtBan::ChannelMatcher());
							ModeManager::AddChannelMode(new UnrealExtBan::EntryMatcher("JOINBAN", "join", 'j'));
							ModeManager::AddChannelMode(new UnrealExtBan::EntryMatcher("NONICKBAN", "nickchange", 'n'));
							ModeManager::AddChannelMode(new UnrealExtBan::EntryMatcher("QUIET", "quiet", 'q'));
							ModeManager::AddChannelMode(new UnrealExtBan::RealnameMatcher());
							ModeManager::AddChannelMode(new UnrealExtBan::AccountMatcher());
							ModeManager::AddChannelMode(new UnrealExtBan::FingerprintMatcher());
							ModeManager::AddChannelMode(new UnrealExtBan::TimedBanMatcher());
							ModeManager::AddChannelMode(new UnrealExtBan::OperclassMatcher());
							ModeManager::AddChannelMode(new UnrealExtBan::CountryMatcher());
							continue;
						case 'e':
							ModeManager::AddChannelMode(new ChannelModeList("EXCEPT", 'e'));
							continue;
						case 'I':
							ModeManager::AddChannelMode(new ChannelModeList("INVITEOVERRIDE", 'I'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelModeList("", mode));
					}
				}

				sep.GetToken(modebuf);
				for (auto mode : modebuf)
				{
					switch (mode)
					{
						case 'k':
							ModeManager::AddChannelMode(new ChannelModeKey('k'));
							continue;
						case 'f':
							ModeManager::AddChannelMode(new ChannelModeFlood('f', false));
							continue;
						case 'L':
							ModeManager::AddChannelMode(new ChannelModeParam("REDIRECT", 'L'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelModeParam("", mode));
					}
				}

				sep.GetToken(modebuf);
				for (auto mode : modebuf)
				{
					switch (mode)
					{
						case 'l':
							ModeManager::AddChannelMode(new ChannelModeParam("LIMIT", 'l', true));
							continue;
						case 'H':
							ModeManager::AddChannelMode(new ChannelModeHistory('H'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelModeParam("", mode, true));
					}
				}

				sep.GetToken(modebuf);
				for (auto mode : modebuf)
				{
					switch (mode)
					{
						case 'p':
							ModeManager::AddChannelMode(new ChannelMode("PRIVATE", 'p'));
							continue;
						case 's':
							ModeManager::AddChannelMode(new ChannelMode("SECRET", 's'));
							continue;
						case 'm':
							ModeManager::AddChannelMode(new ChannelMode("MODERATED", 'm'));
							continue;
						case 'n':
							ModeManager::AddChannelMode(new ChannelMode("NOEXTERNAL", 'n'));
							continue;
						case 't':
							ModeManager::AddChannelMode(new ChannelMode("TOPIC", 't'));
							continue;
						case 'i':
							ModeManager::AddChannelMode(new ChannelMode("INVITE", 'i'));
							continue;
						case 'r':
							ModeManager::AddChannelMode(new ChannelModeNoone("REGISTERED", 'r'));
							continue;
						case 'R':
							ModeManager::AddChannelMode(new ChannelMode("REGISTEREDONLY", 'R'));
							continue;
						case 'c':
							ModeManager::AddChannelMode(new ChannelMode("BLOCKCOLOR", 'c'));
							continue;
						case 'O':
							ModeManager::AddChannelMode(new ChannelModeOperOnly("OPERONLY", 'O'));
							continue;
						case 'Q':
							ModeManager::AddChannelMode(new ChannelMode("NOKICK", 'Q'));
							continue;
						case 'K':
							ModeManager::AddChannelMode(new ChannelMode("NOKNOCK", 'K'));
							continue;
						case 'V':
							ModeManager::AddChannelMode(new ChannelMode("NOINVITE", 'V'));
							continue;
						case 'C':
							ModeManager::AddChannelMode(new ChannelMode("NOCTCP", 'C'));
							continue;
						case 'z':
							ModeManager::AddChannelMode(new ChannelMode("SSL", 'z'));
							continue;
						case 'N':
							ModeManager::AddChannelMode(new ChannelMode("NONICK", 'N'));
							continue;
						case 'S':
							ModeManager::AddChannelMode(new ChannelMode("STRIPCOLOR", 'S'));
							continue;
						case 'M':
							ModeManager::AddChannelMode(new ChannelMode("REGMODERATED", 'M'));
							continue;
						case 'T':
							ModeManager::AddChannelMode(new ChannelMode("NONOTICE", 'T'));
							continue;
						case 'G':
							ModeManager::AddChannelMode(new ChannelMode("CENSOR", 'G'));
							continue;
						case 'Z':
							ModeManager::AddChannelMode(new ChannelModeUnrealSSL("ALLSSL", 'Z'));
							continue;
						case 'd':
							// post delayed. means that channel is -D but invisible users still exist.
							continue;
						case 'D':
							ModeManager::AddChannelMode(new ChannelMode("DELAYEDJOIN", 'D'));
							continue;
						case 'P':
							ModeManager::AddChannelMode(new ChannelModeOperOnly("PERM", 'P'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelMode("", mode));
					}
				}
			}
			else if (!capab.find("SID="))
			{
				UplinkSID = capab.substr(4);
			}
			else if (!capab.find("PREFIX=")) /* PREFIX=(qaohv)~&@%+ */
			{
				Anope::string modes(capab.begin() + 7, capab.end());
				reverse(modes.begin(), modes.end()); /* +%@&!)vhoaq( */
				std::size_t mode_count = modes.find(')');
				Anope::string mode_prefixes = modes.substr(0, mode_count);
				Anope::string mode_chars = modes.substr(mode_count+1, mode_count);

				for (size_t t = 0, end = mode_chars.length(); t < end; ++t)
				{
					Anope::string mode_name;
					switch (mode_chars[t])
					{

						case 'v':
							mode_name = "VOICE";
							break;
						case 'h':
							mode_name = "HALFOP";
							break;
						case 'o':
							mode_name = "OP";
							break;
						case 'a':
							mode_name = "PROTECT";
							break;
						case 'q':
							mode_name = "OWNER";
							break;
						default:
							mode_name = "";
							break;
					}
					ModeManager::AddChannelMode(new ChannelModeStatus(mode_name, mode_chars[t], mode_prefixes[t], t));
				}
			}
			else if (capab.equals_ci("BIGLINES"))
				IRCD->MaxLine = 16384;
		}

		Message::Capab::Run(source, params, tags);
	}
};

struct IRCDMessageChgHost final
	: IRCDMessage
{
	IRCDMessageChgHost(Module *creator) : IRCDMessage(creator, "CHGHOST", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		User *u = User::Find(params[0]);
		if (u)
			u->SetDisplayedHost(params[1]);
	}
};

struct IRCDMessageChgIdent final
	: IRCDMessage
{
	IRCDMessageChgIdent(Module *creator) : IRCDMessage(creator, "CHGIDENT", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		User *u = User::Find(params[0]);
		if (u)
			u->SetVIdent(params[1]);
	}
};

struct IRCDMessageChgName final
	: IRCDMessage
{
	IRCDMessageChgName(Module *creator) : IRCDMessage(creator, "CHGNAME", 2) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		User *u = User::Find(params[0]);
		if (u)
			u->SetRealname(params[1]);
	}
};

struct IRCDMessageMD final
	: IRCDMessage
{
	PrimitiveExtensibleItem<ModData> &ClientModData;
	PrimitiveExtensibleItem<ModData> &ChannelModData;

	IRCDMessageMD(Module *creator, PrimitiveExtensibleItem<ModData> &clmoddata, PrimitiveExtensibleItem<ModData> &chmoddata) : IRCDMessage(creator, "MD", 3), ClientModData(clmoddata), ChannelModData(chmoddata)
	{
		SetFlag(FLAG_SOFT_LIMIT);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		const Anope::string &mdtype = params[0],
				    &obj = params[1],
				    &var = params[2],
				    &value = params.size() > 3 ? params[3] : "";

		if (mdtype == "client") /* can be a server too! */
		{
			User *u = User::Find(obj);

			if (u == NULL)
				return;

			ModData &clientmd = *ClientModData.Require(u);

			if (value.empty())
			{
				clientmd.erase(var);
				Log(LOG_DEBUG) << "Erased client moddata " << var << " from " << u->nick;
			}
			else
			{
				clientmd[var] = value;
				Log(LOG_DEBUG) << "Set client moddata " << var << "=\"" << value << "\" to " << u->nick;
			}
			if (var == "certfp" && !value.empty())
			{
				u->Extend<bool>("ssl");
				u->fingerprint = value;
				FOREACH_MOD(OnFingerprint, (u));
			}
		}
		else if (mdtype == "channel")
		{
			Channel *c = Channel::Find(obj);

			if (c == NULL)
				return;

			ModData &channelmd = *ChannelModData.Require(c);

			if (value.empty())
			{
				channelmd.erase(var);
				Log(LOG_DEBUG) << "Erased channel moddata " << var << " from " << c->name;
			}
			else
			{
				channelmd[var] = value;
				Log(LOG_DEBUG) << "Set channel moddata " << var << "=\"" << value << "\" to " << c->name;
			}
		}
	}
};

struct IRCDMessageMode final
	: IRCDMessage
{
	bool server_ts;

	IRCDMessageMode(Module *creator, const Anope::string &mname, bool sts)
		: IRCDMessage(creator, mname, 2)
		, server_ts(sts)
	{
		SetFlag(FLAG_SOFT_LIMIT);
	}

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		auto final_is_ts = server_ts && source.GetServer() != NULL;
		auto last_param = params.end() - (params.size() > 3 && final_is_ts ? 1 : 0);

		if (IRCD->IsChannelValid(params[0]))
		{
			Channel *c = Channel::Find(params[0]);
			auto ts = final_is_ts ? IRCD->ExtractTimestamp(params.back()) : 0;

			if (c)
				c->SetModesInternal(source, params[2], { params.begin() + 3, last_param }, ts);
		}
		else
		{
			User *u = User::Find(params[0]);
			if (u)
				u->SetModesInternal(source, params[1]);
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
struct IRCDMessageNetInfo final
	: IRCDMessage
{
	IRCDMessageNetInfo(Module *creator) : IRCDMessage(creator, "NETINFO", 8) { SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		Uplink::Send("NETINFO", MaxUserCount, Anope::CurTime, params[2], params[3], 0, 0, 0, params[7]);
	}
};

struct IRCDMessageNick final
	: IRCDMessage
{
	IRCDMessageNick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(FLAG_SOFT_LIMIT); }

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
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
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

			auto user_ts = IRCD->ExtractTimestamp(params[2]);
			Server *s = Server::Find(params[5]);
			if (s == NULL)
			{
				Log(LOG_DEBUG) << "User " << params[0] << " introduced from nonexistent server " << params[5] << "?";
				return;
			}

			NickAlias *na = NULL;

			if (params[6] == "0")
				;
			else if (params[6].is_pos_number_only())
			{
				if (IRCD->ExtractTimestamp(params[6]) == user_ts)
					na = NickAlias::Find(params[0]);
			}
			else
			{
				na = NickAlias::Find(params[6]);
			}

			User::OnIntroduce(params[0], params[3], params[4], vhost, ip, s, params[10], user_ts, params[7], "", na ? *na->nc : NULL);
		}
		else
		{
			User *u = source.GetUser();
			if (u)
				u->ChangeNick(params[0]);
		}
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
struct IRCDMessagePong final
	: IRCDMessage
{
	IRCDMessagePong(Module *creator) : IRCDMessage(creator, "PONG", 0) { SetFlag(FLAG_SOFT_LIMIT); SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (!source.GetServer()->IsSynced())
			source.GetServer()->Sync(false);
	}
};

struct IRCDMessageSASL final
	: IRCDMessage
{
	IRCDMessageSASL(Module *creator) : IRCDMessage(creator, "SASL", 4) { SetFlag(FLAG_SOFT_LIMIT); SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (!SASL::sasl)
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

struct IRCDMessageSDesc final
	: IRCDMessage
{
	IRCDMessageSDesc(Module *creator) : IRCDMessage(creator, "SDESC", 1) { SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		source.GetServer()->SetDescription(params[0]);
	}
};

struct IRCDMessageSetHost final
	: IRCDMessage
{
	IRCDMessageSetHost(Module *creator) : IRCDMessage(creator, "SETHOST", 1) { SetFlag(FLAG_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		User *u = source.GetUser();

		/* When a user sets +x we receive the new host and then the mode change */
		if (u->HasMode("CLOAK"))
			u->SetDisplayedHost(params[0]);
		else
			u->SetCloakedHost(params[0]);
	}
};

struct IRCDMessageSetIdent final
	: IRCDMessage
{
	IRCDMessageSetIdent(Module *creator) : IRCDMessage(creator, "SETIDENT", 1) { SetFlag(FLAG_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		User *u = source.GetUser();
		u->SetVIdent(params[0]);
	}
};

struct IRCDMessageSetName final
	: IRCDMessage
{
	IRCDMessageSetName(Module *creator) : IRCDMessage(creator, "SETNAME", 1) { SetFlag(FLAG_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		User *u = source.GetUser();
		u->SetRealname(params[0]);
	}
};

struct IRCDMessageServer final
	: IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		auto hops = Anope::Convert<unsigned>(params[1], 0);

		if (params[1].equals_cs("1"))
		{
			Anope::string desc;
			spacesepstream(params[2]).GetTokenRemainder(desc, 1);

			new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, desc, UplinkSID);
		}
		else
			new Server(source.GetServer(), params[0], hops, params[2]);

		IRCD->SendPing(Me->GetName(), params[0]);
	}
};

struct IRCDMessageSID final
	: IRCDMessage
{
	IRCDMessageSID(Module *creator) : IRCDMessage(creator, "SID", 4) { SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		auto hops = Anope::Convert<unsigned>(params[1], 0);

		new Server(source.GetServer(), params[0], hops, params[3], params[2]);

		IRCD->SendPing(Me->GetName(), params[0]);
	}
};

static char UnrealSjoinPrefixToModeChar(char sjoin_prefix)
{
	switch(sjoin_prefix)
	{
		case '*':
			return ModeManager::GetStatusChar('~');
		case '~':
			return ModeManager::GetStatusChar('&');
		default:
			return ModeManager::GetStatusChar(sjoin_prefix); /* remaining are regular */
	}
}

struct IRCDMessageSJoin final
	: IRCDMessage
{
	IRCDMessageSJoin(Module *creator) : IRCDMessage(creator, "SJOIN", 3) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		Anope::string modes;
		std::vector<Anope::string> modeparams;
		if (params.size() >= 4)
		{
			modes = params[2];
			modeparams = { params.begin() + 3, params.end() };
		}

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
				for (char ch; (ch = UnrealSjoinPrefixToModeChar(buf[0]));)
				{
					sju.first.AddMode(ch);
					buf.erase(buf.begin());
				}

				sju.second = User::Find(buf);
				if (!sju.second)
				{
					Log(LOG_DEBUG) << "SJOIN for nonexistent user " << buf << " on " << params[1];
					continue;
				}

				users.push_back(sju);
			}
		}

		auto ts = IRCD->ExtractTimestamp(params[0]);
		Message::Join::SJoin(source, params[1], ts, modes, modeparams, users);

		if (!bans.empty() || !excepts.empty() || !invites.empty())
		{
			Channel *c = Channel::Find(params[1]);

			if (!c || c->creation_time != ts)
				return;

			ChannelMode *ban = ModeManager::FindChannelModeByName("BAN"),
				*except = ModeManager::FindChannelModeByName("EXCEPT"),
				*invex = ModeManager::FindChannelModeByName("INVITEOVERRIDE");

			if (ban)
			{
				for (const auto &entry : bans)
					c->SetModeInternal(source, ban, entry);
			}
			if (except)
			{
				for (const auto &entry : excepts)
					c->SetModeInternal(source, except, entry);
			}
			if (invex)
			{
				for (const auto &entry : invites)
					c->SetModeInternal(source, invex, entry);
			}
		}
	}
};

class IRCDMessageSVSLogin final
	: IRCDMessage
{
public:
	IRCDMessageSVSLogin(Module *creator) : IRCDMessage(creator, "SVSLOGIN", 3) { SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		// :irc.example.com SVSLOGIN <mask> <nick> <account>
		// :irc.example.com SVSLOGIN <mask> <nick> 0
		User *u = User::Find(params[1]);
		if (!u)
			return; // Should never happen.

		if (params[2] == "0")
		{
			// The user has been logged out by the IRC server.
			u->Logout();
		}
		else
		{
			// If we're bursting then then the user was probably logged
			// in during a previous connection.
			NickCore *nc = NickCore::Find(params[2]);
			if (nc)
				u->Login(nc);
		}
	}
};

struct IRCDMessageTopic final
	: IRCDMessage
{
	IRCDMessageTopic(Module *creator) : IRCDMessage(creator, "TOPIC", 4) { }

	/*
	**	source = sender prefix
	**	parv[0] = channel name
	**	parv[1] = topic nickname
	**	parv[2] = topic time
	**	parv[3] = topic text
	*/
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		Channel *c = Channel::Find(params[0]);
		if (c)
			c->ChangeTopicInternal(source.GetUser(), params[1], params[3], IRCD->ExtractTimestamp(params[2]));
	}
};

/*
 *      parv[0] = nickname
 *      parv[1] = hopcount
 *      parv[2] = timestamp
 *      parv[3] = username
 *      parv[4] = hostname
 *      parv[5] = UID
 *      parv[6] = servicestamp
 *      parv[7] = umodes
 *      parv[8] = virthost, * if none
 *      parv[9] = cloaked host, * if none
 *      parv[10] = ip
 *      parv[11] = info
 */
struct IRCDMessageUID final
	: IRCDMessage
{
	IRCDMessageUID(Module *creator) : IRCDMessage(creator, "UID", 12) { SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		Anope::string
			nickname  = params[0],
			hopcount  = params[1],
			timestamp = params[2],
			username  = params[3],
			hostname  = params[4],
			uid       = params[5],
			account   = params[6],
			umodes    = params[7],
			vhost     = params[8],
			chost     = params[9],
			ip        = params[10],
			info      = params[11];

		if (ip != "*")
		{
			Anope::string decoded_ip;
			Anope::B64Decode(ip, decoded_ip);

			sockaddrs ip_addr;
			ip_addr.ntop(ip.length() == 8 ? AF_INET : AF_INET6, decoded_ip.c_str());
			ip = ip_addr.addr();
		}

		if (vhost == "*")
			vhost.clear();

		if (chost == "*")
			chost.clear();

		auto user_ts = IRCD->ExtractTimestamp(timestamp);
		NickAlias *na = NULL;

		if (account == "0")
		{
			;
		}
		else if (account.is_pos_number_only())
		{
			if (IRCD->ExtractTimestamp(account) == user_ts)
				na = NickAlias::Find(nickname);
		}
		else
		{
			na = NickAlias::Find(account);
		}

		User *u = User::OnIntroduce(nickname, username, hostname, vhost, ip, source.GetServer(), info, user_ts, umodes, uid, na ? *na->nc : NULL);

		if (u && !chost.empty() && chost != u->GetCloakedHost())
			u->SetCloakedHost(chost);
	}
};

struct IRCDMessageUmode2 final
	: IRCDMessage
{
	IRCDMessageUmode2(Module *creator) : IRCDMessage(creator, "UMODE2", 1) { SetFlag(FLAG_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		source.GetUser()->SetModesInternal(source, params[0]);
	}
};

class ProtoUnreal final
	: public Module
{
	UnrealIRCdProto ircd_proto;

	/* Core message handlers */
	Message::Away message_away;
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
	IRCDMessageCapab message_capab;
	IRCDMessageChgHost message_chghost;
	IRCDMessageChgIdent message_chgident;
	IRCDMessageChgName message_chgname;
	IRCDMessageMD message_md;
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
	IRCDMessageSID message_sid;
	IRCDMessageSJoin message_sjoin;
	IRCDMessageSVSLogin message_svslogin;
	IRCDMessageTopic message_topic;
	IRCDMessageUID message_uid;
	IRCDMessageUmode2 message_umode2;

public:
	ProtoUnreal(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, PROTOCOL | VENDOR)
		, ircd_proto(this)
		, message_away(this)
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
		, message_capab(this)
		, message_chghost(this)
		, message_chgident(this)
		, message_chgname(this)
		, message_md(this, ircd_proto.ClientModData, ircd_proto.ChannelModData)
		, message_mode(this, "MODE", true)
		, message_svsmode(this, "SVSMODE", false)
		, message_svs2mode(this, "SVS2MODE", false)
		, message_netinfo(this)
		, message_nick(this)
		, message_pong(this)
		, message_sasl(this)
		, message_sdesc(this)
		, message_sethost(this)
		, message_setident(this)
		, message_setname(this)
		, message_server(this)
		, message_sid(this)
		, message_sjoin(this)
		, message_svslogin(this)
		, message_topic(this)
		, message_uid(this)
		, message_umode2(this)
	{
	}

	void Prioritize() override
	{
		ModuleManager::SetPriority(this, PRIORITY_FIRST);
	}

	void OnUserNickChange(User *u, const Anope::string &) override
	{
		u->RemoveModeInternal(Me, ModeManager::FindUserModeByName("REGISTERED"));
	}

	void OnChannelSync(Channel *c) override
	{
		if (!c->ci)
			return;

		ModeLocks *modelocks = c->ci->GetExt<ModeLocks>("modelocks");
		if (Servers::Capab.count("MLOCK") > 0 && modelocks)
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");
			Uplink::Send("MLOCK", c->creation_time, c->ci->name, modes);
		}
	}

	void OnChanRegistered(ChannelInfo *ci) override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		if (!ci->c || !modelocks || !Servers::Capab.count("MLOCK"))
			return;
		Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");
		Uplink::Send("MLOCK", ci->c->creation_time, ci->name, modes);
	}

	void OnDelChan(ChannelInfo *ci) override
	{
		if (!ci->c || !Servers::Capab.count("MLOCK"))
			return;
		Uplink::Send("MLOCK", ci->c->creation_time, ci->name, "");
	}

	EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock) override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (cm && modelocks && ci->c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->mchar;
			Uplink::Send("MLOCK", ci->c->creation_time, ci->name, modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChannelInfo *ci, ModeLock *lock) override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (cm && modelocks && ci->c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK") > 0)
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->mchar, "");
			Uplink::Send("MLOCK", ci->c->creation_time, ci->name, modes);
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ProtoUnreal)
