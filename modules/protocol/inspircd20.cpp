/* Inspircd 2.0 functions
 *
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/cs_mode.h"

static unsigned int spanningtree_proto_ver = 0;

static ServiceReference<IRCDProto> insp12("IRCDProto", "inspircd12");

class InspIRCd20Proto : public IRCDProto
{
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

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "CAPAB START 1202";
		UplinkSocket::Message() << "CAPAB CAPABILITIES :PROTOCOL=1202";
		UplinkSocket::Message() << "CAPAB END";
		insp12->SendConnect();
	}

	void SendSASLMechanisms(std::vector<Anope::string> &mechanisms) anope_override
	{
		Anope::string mechlist;
		for (unsigned i = 0; i < mechanisms.size(); ++i)
			mechlist += "," + mechanisms[i];

		UplinkSocket::Message(Me) << "METADATA * saslmechlist :" << (mechanisms.empty() ? "" : mechlist.substr(1));
	}

	void SendSVSKillInternal(const MessageSource &source, User *user, const Anope::string &buf) anope_override { insp12->SendSVSKillInternal(source, user, buf); }
	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { insp12->SendGlobalNotice(bi, dest, msg); }
	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override { insp12->SendGlobalPrivmsg(bi, dest, msg); }
	void SendAkillDel(const XLine *x) anope_override { insp12->SendAkillDel(x); }
	void SendTopic(const MessageSource &whosets, Channel *c) anope_override { insp12->SendTopic(whosets, c); };
	void SendVhostDel(User *u) anope_override { insp12->SendVhostDel(u); }
	void SendAkill(User *u, XLine *x) anope_override { insp12->SendAkill(u, x); }
	void SendNumericInternal(int numeric, const Anope::string &dest, const Anope::string &buf) anope_override { insp12->SendNumericInternal(numeric, dest, buf); }
	void SendModeInternal(const MessageSource &source, const Channel *dest, const Anope::string &buf) anope_override { insp12->SendModeInternal(source, dest, buf); }
	void SendClientIntroduction(User *u) anope_override { insp12->SendClientIntroduction(u); }
	void SendServer(const Server *server) anope_override { insp12->SendServer(server); }
	void SendSquit(Server *s, const Anope::string &message) anope_override { insp12->SendSquit(s, message); }
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override { insp12->SendJoin(user, c, status); }
	void SendSQLineDel(const XLine *x) anope_override { insp12->SendSQLineDel(x); }
	void SendSQLine(User *u, const XLine *x) anope_override { insp12->SendSQLine(u, x); }
	void SendVhost(User *u, const Anope::string &vident, const Anope::string &vhost) anope_override { insp12->SendVhost(u, vident, vhost); }
	void SendSVSHold(const Anope::string &nick, time_t t) anope_override { insp12->SendSVSHold(nick, t); }
	void SendSVSHoldDel(const Anope::string &nick) anope_override { insp12->SendSVSHoldDel(nick); }
	void SendSZLineDel(const XLine *x) anope_override { insp12->SendSZLineDel(x); }
	void SendSZLine(User *u, const XLine *x) anope_override { insp12->SendSZLine(u, x); }
	void SendSVSJoin(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &other) anope_override { insp12->SendSVSJoin(source, u, chan, other); }
	void SendSVSPart(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &param) anope_override { insp12->SendSVSPart(source, u, chan, param); }
	void SendSWhois(const MessageSource &bi, const Anope::string &who, const Anope::string &mask) anope_override { insp12->SendSWhois(bi, who, mask); }
	void SendBOB() anope_override { insp12->SendBOB(); }
	void SendEOB() anope_override { insp12->SendEOB(); }
	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) { insp12->SendGlobopsInternal(source, buf); }
	void SendLogin(User *u, NickAlias *na) anope_override { insp12->SendLogin(u, na); }
	void SendLogout(User *u) anope_override { insp12->SendLogout(u); }
	void SendChannel(Channel *c) anope_override { insp12->SendChannel(c); }
	void SendSASLMessage(const SASL::Message &message) anope_override { insp12->SendSASLMessage(message); }
	void SendSVSLogin(const Anope::string &uid, const Anope::string &acc, const Anope::string &vident, const Anope::string &vhost) anope_override { insp12->SendSVSLogin(uid, acc, vident, vhost); }
	bool IsExtbanValid(const Anope::string &mask) anope_override { return insp12->IsExtbanValid(mask); }
	bool IsIdentValid(const Anope::string &ident) anope_override { return insp12->IsIdentValid(ident); }
};

class InspIRCdAutoOpMode : public ChannelModeList
{
 public:
	InspIRCdAutoOpMode(char mode) : ChannelModeList("AUTOOP", mode)
	{
	}

	bool IsValid(Anope::string &mask) const anope_override
	{
		// We can not validate this because we don't know about the
		// privileges of the setter so just reject attempts to set it.
		return false;
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

	ChannelMode *Wrap(Anope::string &param) anope_override
	{
		param = Anope::string(ext) + ":" + param;
		return ChannelModeVirtual<ChannelModeList>::Wrap(param);
	}

	ChannelMode *Unwrap(ChannelMode *cm, Anope::string &param) anope_override
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

		bool Matches(User *u, const Entry *e) anope_override
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

		bool Matches(User *u, const Entry *e) anope_override
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

		bool Matches(User *u, const Entry *e) anope_override
		{
			const Anope::string &mask = e->GetMask();
			Anope::string real_mask = mask.substr(2);

			return u->IsIdentified() && real_mask.equals_ci(u->Account()->display);
		}
	};

	class RealnameMatcher : public InspIRCdExtBan
	{
	 public:
	 	RealnameMatcher(const Anope::string &mname, const Anope::string &mbase, char c) : InspIRCdExtBan(mname, mbase, c)
	 	{
	 	}

	 	bool Matches(User *u, const Entry *e) anope_override
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

	 	bool Matches(User *u, const Entry *e) anope_override
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

	 	bool Matches(User *u, const Entry *e) anope_override
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

		bool Matches(User *u, const Entry *e) anope_override
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

	bool IsValid(Anope::string &value) const anope_override
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

	bool IsValid(Anope::string &value) const anope_override
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

	bool IsValid(Anope::string &value) const anope_override
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

	bool IsValid(Anope::string &value) const anope_override
	{
		return (ColonDelimitedParamMode::IsValid(value, true));
	}
};

class ChannelModeRedirect : public ChannelModeParam
{
 public:
	ChannelModeRedirect(char modeChar) : ChannelModeParam("REDIRECT", modeChar, true) { }

	bool IsValid(Anope::string &value) const anope_override
	{
		// The parameter of this mode is a channel, and channel names start with '#'
		return ((!value.empty()) && (value[0] == '#'));
	}
};

struct IRCDMessageCapab : Message::Capab
{
	std::map<char, Anope::string> chmodes, umodes;

	IRCDMessageCapab(Module *creator) : Message::Capab(creator, "CAPAB") { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
				else if (modename.equals_cs("autoop"))
					cm = new InspIRCdAutoOpMode(modechar[0]);
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
					const Anope::string &regexengine = Config->GetBlock("options")->Get<const Anope::string>("regexengine");
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
	ServiceReference<IRCDMessage> insp12_encap;

	IRCDMessageEncap(Module *creator) : IRCDMessage(creator, "ENCAP", 4), insp12_encap("IRCDMessage", "inspircd12/encap") { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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

		if (insp12_encap)
			insp12_encap->Run(source, params);
	}
};

struct IRCDMessageFHost : IRCDMessage
{
	IRCDMessageFHost(Module *creator) : IRCDMessage(creator, "FHOST", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.GetUser();
		if (u->HasMode("CLOAK"))
			u->RemoveModeInternal(source, ModeManager::FindUserModeByName("CLOAK"));
		u->SetDisplayedHost(params[0]);
	}
};

struct IRCDMessageFIdent : IRCDMessage
{
	IRCDMessageFIdent(Module *creator) : IRCDMessage(creator, "FIDENT", 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->SetIdent(params[0]);
	}
};

struct IRCDMessageSave : IRCDMessage
{
	time_t last_collide;

	IRCDMessageSave(Module *creator) : IRCDMessage(creator, "SAVE", 2), last_collide(0) { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *targ = User::Find(params[0]);
		time_t ts;

		try
		{
			ts = convertTo<time_t>(params[1]);
		}
		catch (const ConvertException &)
		{
			return;
		}

		if (!targ || targ->timestamp != ts)
			return;

		BotInfo *bi;
		if (targ->server == Me && (bi = dynamic_cast<BotInfo *>(targ)))
		{
			if (last_collide == Anope::CurTime)
			{
				Anope::QuitReason = "Nick collision fight on " + targ->nick;
				Anope::Quitting = true;
				return;
			}

			IRCD->SendKill(Me, targ->nick, "Nick collision");
			IRCD->SendNickChange(targ, targ->nick);
			last_collide = Anope::CurTime;
		}
		else
			targ->ChangeNick(targ->GetUID());
	}
};

class IRCDMessageMetadata : IRCDMessage
{
	ServiceReference<IRCDMessage> insp12_metadata;
	const bool &do_topiclock;
	const bool &do_mlock;

 public:
	IRCDMessageMetadata(Module *creator, const bool &handle_topiclock, const bool &handle_mlock) : IRCDMessage(creator, "METADATA", 3), insp12_metadata("IRCDMessage", "inspircd12/metadata"), do_topiclock(handle_topiclock), do_mlock(handle_mlock) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		// We deliberately ignore non-bursting servers to avoid pseudoserver fights
		if ((params[0][0] == '#') && (!source.GetServer()->IsSynced()))
		{
			Channel *c = Channel::Find(params[0]);
			if (c && c->ci)
			{
				if ((do_mlock) && (params[1] == "mlock"))
				{
					ModeLocks *modelocks = c->ci->GetExt<ModeLocks>("modelocks");
					Anope::string modes;
					if (modelocks)
						modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");

					// Mode lock string is not what we say it is?
					if (modes != params[2])
						UplinkSocket::Message(Me) << "METADATA " << c->name << " mlock :" << modes;
				}
				else if ((do_topiclock) && (params[1] == "topiclock"))
				{
					bool mystate = c->ci->GetExt<bool>("TOPICLOCK");
					bool serverstate = (params[2] == "1");
					if (mystate != serverstate)
						UplinkSocket::Message(Me) << "METADATA " << c->name << " topiclock :" << (mystate ? "1" : "");
				}
			}
		}

		if (insp12_metadata)
			insp12_metadata->Run(source, params);
	}
};

class ProtoInspIRCd20 : public Module
{
	Module *m_insp12;

	InspIRCd20Proto ircd_proto;

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

	/* InspIRCd 1.2 message handlers */
	ServiceAlias message_endburst, message_fjoin, message_fmode,
				message_ftopic, message_idle, message_mode,
				message_nick, message_opertype, message_rsquit, message_server,
				message_squit, message_time, message_uid;

	/* Our message handlers */
	IRCDMessageCapab message_capab;
	IRCDMessageEncap message_encap;
	IRCDMessageFHost message_fhost;
	IRCDMessageFIdent message_fident;
	IRCDMessageMetadata message_metadata;
	IRCDMessageSave message_save;

	bool use_server_side_topiclock, use_server_side_mlock;

	void SendChannelMetadata(Channel *c, const Anope::string &metadataname, const Anope::string &value)
	{
		UplinkSocket::Message(Me) << "METADATA " << c->name << " " << metadataname << " :" << value;
	}

 public:
	ProtoInspIRCd20(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR),
		ircd_proto(this),
		message_away(this), message_error(this), message_invite(this), message_join(this), message_kick(this),
		message_kill(this), message_motd(this), message_notice(this), message_part(this), message_ping(this),
		message_privmsg(this), message_quit(this), message_stats(this), message_topic(this),

		message_endburst("IRCDMessage", "inspircd20/endburst", "inspircd12/endburst"),
		message_fjoin("IRCDMessage", "inspircd20/fjoin", "inspircd12/fjoin"),
		message_fmode("IRCDMessage", "inspircd20/fmode", "inspircd12/fmode"),
		message_ftopic("IRCDMessage", "inspircd20/ftopic", "inspircd12/ftopic"),
		message_idle("IRCDMessage", "inspircd20/idle", "inspircd12/idle"),
		message_mode("IRCDMessage", "inspircd20/mode", "inspircd12/mode"),
		message_nick("IRCDMessage", "inspircd20/nick", "inspircd12/nick"),
		message_opertype("IRCDMessage", "inspircd20/opertype", "inspircd12/opertype"),
		message_rsquit("IRCDMessage", "inspircd20/rsquit", "inspircd12/rsquit"),
		message_server("IRCDMessage", "inspircd20/server", "inspircd12/server"),
		message_squit("IRCDMessage", "inspircd20/squit", "inspircd12/squit"),
		message_time("IRCDMessage", "inspircd20/time", "inspircd12/time"),
		message_uid("IRCDMessage", "inspircd20/uid", "inspircd12/uid"),

		message_capab(this), message_encap(this), message_fhost(this), message_fident(this), message_metadata(this, use_server_side_topiclock, use_server_side_mlock),
		message_save(this)
	{

		if (ModuleManager::LoadModule("inspircd12", User::Find(creator)) != MOD_ERR_OK)
			throw ModuleException("Unable to load inspircd12");
		m_insp12 = ModuleManager::FindModule("inspircd12");
		if (!m_insp12)
			throw ModuleException("Unable to find inspircd12");
		if (!insp12)
			throw ModuleException("No protocol interface for insp12");
		ModuleManager::DetachAll(m_insp12);

	}

	~ProtoInspIRCd20()
	{
		m_insp12 = ModuleManager::FindModule("inspircd12");
		ModuleManager::UnloadModule(m_insp12, NULL);
	}

	void OnReload(Configuration::Conf *conf) anope_override
	{
		use_server_side_topiclock = conf->GetModule(this)->Get<bool>("use_server_side_topiclock");
		use_server_side_mlock = conf->GetModule(this)->Get<bool>("use_server_side_mlock");
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveModeInternal(Me, ModeManager::FindUserModeByName("REGISTERED"));
	}

	void OnChannelSync(Channel *c) anope_override
	{
		if (c->ci)
			this->OnChanRegistered(c->ci);
	}

	void OnChanRegistered(ChannelInfo *ci) anope_override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		if (use_server_side_mlock && ci->c && modelocks && !modelocks->GetMLockAsString(false).empty())
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");
			SendChannelMetadata(ci->c, "mlock", modes);
		}

		if (use_server_side_topiclock && Servers::Capab.count("TOPICLOCK") && ci->c)
		{
			if (ci->HasExt("TOPICLOCK"))
				SendChannelMetadata(ci->c, "topiclock", "1");
		}
	}

	void OnDelChan(ChannelInfo *ci) anope_override
	{
		if (use_server_side_mlock && ci->c)
			SendChannelMetadata(ci->c, "mlock", "");

		if (use_server_side_topiclock && Servers::Capab.count("TOPICLOCK") && ci->c)
			SendChannelMetadata(ci->c, "topiclock", "");
	}

	EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock) anope_override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (use_server_side_mlock && cm && ci->c && modelocks && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM))
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->mchar;
			SendChannelMetadata(ci->c, "mlock", modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChannelInfo *ci, ModeLock *lock) anope_override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (use_server_side_mlock && cm && ci->c && modelocks && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM))
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->mchar, "");
			SendChannelMetadata(ci->c, "mlock", modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnSetChannelOption(CommandSource &source, Command *cmd, ChannelInfo *ci, const Anope::string &setting) anope_override
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
};

MODULE_INIT(ProtoInspIRCd20)
