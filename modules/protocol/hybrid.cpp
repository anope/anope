/* ircd-hybrid protocol module. Minimum supported version of ircd-hybrid is 8.2.23.
 *
 * (C) 2003-2025 Anope Team <team@anope.org>
 * (C) 2012-2022 ircd-hybrid development team
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/chanserv/mode.h"

static Anope::string UplinkSID;
static bool UseSVSAccount = false;  // Temporary backwards compatibility hack until old proto is deprecated

class HybridProto final
	: public IRCDProto
{
	void SendSVSKill(const MessageSource &source, User *u, const Anope::string &buf) override
	{
		IRCDProto::SendSVSKill(source, u, buf);
		u->KillInternal(source, buf);
	}

public:
	HybridProto(Module *creator) : IRCDProto(creator, "ircd-hybrid 8.2.23+")
	{
		DefaultPseudoclientModes = "+oi";
		CanSVSNick = true;
		CanSVSHold = true;
		CanSVSJoin = true;
		CanSNLine = true;
		CanSQLine = true;
		CanSQLineChannel = true;
		CanSZLine = true;
		CanCertFP = true;
		CanSetVHost = true;
		RequiresID = true;
		MaxModes = 6;
	}

	void SendInvite(const MessageSource &source, const Channel *c, User *u) override
	{
		Uplink::Send(source, "INVITE", u->GetUID(), c->name, c->created);
	}

	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "NOTICE", "$$" + dest->GetName(), msg);
	}

	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "PRIVMSG", "$$" + dest->GetName(), msg);
	}

	void SendSQLine(User *, const XLine *x) override
	{
		Uplink::Send("RESV", '*', x->expires ? x->expires - Anope::CurTime : 0, x->mask, x->reason);
	}

	void SendSGLineDel(const XLine *x) override
	{
		Uplink::Send("UNXLINE", '*', x->mask);
	}

	void SendSGLine(User *, const XLine *x) override
	{
		Uplink::Send("XLINE", '*', x->mask, x->expires ? x->expires - Anope::CurTime : 0, x->GetReason());
	}

	void SendSZLineDel(const XLine *x) override
	{
		Uplink::Send("UNDLINE", '*', x->GetHost());
	}

	void SendSZLine(User *, const XLine *x) override
	{
		/* Calculate the time left before this would expire */
		time_t timeleft = x->expires ? x->expires - Anope::CurTime : x->expires;
		Uplink::Send("DLINE", '*', timeleft, x->GetHost(), x->GetReason());
	}

	void SendAkillDel(const XLine *x) override
	{
		if (x->IsRegex() || x->HasNickOrReal())
			return;

		Uplink::Send("UNKLINE", '*', x->GetUser(), x->GetHost());
	}

	void SendSQLineDel(const XLine *x) override
	{
		Uplink::Send("UNRESV", '*', x->mask);
	}

	void SendJoin(User *u, Channel *c, const ChannelStatus *status) override
	{
		Uplink::Send("SJOIN", c->created, c->name, "+" + c->GetModes(true, true), u->GetUID());

		/*
		 * Note that we can send this with the SJOIN but choose not to
		 * because the mode stacker will handle this and probably will
		 * merge these modes with +nrt and other mlocked modes.
		 */
		if (status)
		{
			/* First save the channel status in case uc->status == status */
			ChannelStatus cs = *status;

			/*
			 * If the user is internally on the channel with flags, kill them so that
			 * the stacker will allow this.
			 */
			ChanUserContainer *uc = c->FindUser(u);
			if (uc)
				uc->status.Clear();

			BotInfo *setter = BotInfo::Find(u->GetUID());
			for (auto mode : cs.Modes())
				c->SetMode(setter, ModeManager::FindChannelModeByChar(mode), u->GetUID(), false);

			if (uc)
				uc->status = cs;
		}
	}

	void SendAkill(User *u, XLine *x) override
	{
		if (x->IsRegex() || x->HasNickOrReal())
		{
			if (!u)
			{
				/*
				 * No user (this akill was just added), and contains nick and/or realname.
				 * Find users that match and ban them.
				 */
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

			Log(Config->GetClient("OperServ"), "akill") << "AKILL: Added an akill for " << x->mask << " because " << u->GetMask() << "#"
					<< u->realname << " matches " << old->mask;
		}

		/* Calculate the time left before this would expire */
		time_t timeleft = x->expires ? x->expires - Anope::CurTime : x->expires;
		Uplink::Send("KLINE", '*', timeleft, x->GetUser(), x->GetHost(), x->GetReason());
	}

	void SendServer(const Server *server) override
	{
		if (server == Me)
			Uplink::Send("SERVER", server->GetName(), server->GetHops() + 1, server->GetSID(), '+', server->GetDescription());
		else
			Uplink::Send("SID", server->GetName(), server->GetHops() + 1, server->GetSID(), '+', server->GetDescription());
	}

	void SendConnect() override
	{
		Uplink::Send("PASS", Config->Uplinks[Anope::CurrentUplink].password);

		/*
		 * TBURST - Supports topic burst
		 * ENCAP  - Supports ENCAP
		 * EOB    - Supports End Of Burst message
		 * RHOST  - Supports UID message with realhost information
		 * MLOCK  - Supports MLOCK
		 */
		Uplink::Send("CAPAB", "ENCAP", "TBURST", "EOB", "RHOST", "MLOCK");

		SendServer(Me);

		Uplink::Send("SVINFO", 6, 6, 0, Anope::CurTime);
	}

	void SendClientIntroduction(User *u) override
	{
		Uplink::Send("UID", u->nick, 1, u->timestamp, "+" + u->GetModes(), u->GetIdent(), u->host, u->host, "0.0.0.0", u->GetUID(), '*', u->realname);
	}

	void SendEOB() override
	{
		Uplink::Send("EOB");
	}

	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &modes, const std::vector<Anope::string> &values) override
	{
		auto params = values;
		params.insert(params.begin(), { u->GetUID(), Anope::ToString(u->timestamp), modes });
		Uplink::SendInternal({}, source, "SVSMODE", params);
	}

	void SendLogin(User *u, NickAlias *na) override
	{
		if (!UseSVSAccount)
			IRCD->SendMode(Config->GetClient("NickServ"), u, "+d", na->nc->display);
		else
			Uplink::Send("SVSACCOUNT", u->GetUID(), u->timestamp, na->nc->display);
	}

	void SendLogout(User *u) override
	{
		if (!UseSVSAccount)
			IRCD->SendMode(Config->GetClient("NickServ"), u, "+d", '*');
		else
			Uplink::Send("SVSACCOUNT", u->GetUID(), u->timestamp, '*');
	}

	void SendChannel(Channel *c) override
	{
		Uplink::Send("SJOIN", c->created, c->name, "+" + c->GetModes(true, true), "");
	}

	void SendTopic(const MessageSource &source, Channel *c) override
	{
		Uplink::Send(source, "TBURST", c->created, c->name, c->topic_ts, c->topic_setter, c->topic);
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) override
	{
		Uplink::Send("SVSNICK", u->GetUID(), u->timestamp, newnick, when);
	}

	void SendSVSJoin(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &key) override
	{
		Uplink::Send(source, "SVSJOIN", u->GetUID(), chan);
	}

	void SendSVSPart(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &param) override
	{
		if (!param.empty())
			Uplink::Send(source, "SVSPART", u->GetUID(), chan, param);
		else
			Uplink::Send(source, "SVSPART", u->GetUID(), chan);
	}

	void SendSVSHold(const Anope::string &nick, time_t t) override
	{
		XLine x(nick, Me->GetName(), Anope::CurTime + t, "Being held for a registered user");
		this->SendSQLine(NULL, &x);
	}

	void SendSVSHoldDel(const Anope::string &nick) override
	{
		XLine x(nick);
		this->SendSQLineDel(&x);
	}

	void SendVHost(User *u, const Anope::string &ident, const Anope::string &host) override
	{
		Uplink::Send("SVSHOST", u->GetUID(), u->timestamp, host);
	}

	void SendVHostDel(User *u) override
	{
		Uplink::Send("SVSHOST", u->GetUID(), u->timestamp, u->host);
	}

	bool IsExtbanValid(const Anope::string &mask) override
	{
		return mask.length() >= 4 && mask[0] == '$' && mask[2] == ':';
	}

	bool IsIdentValid(const Anope::string &ident) override
	{
		if (ident.empty() || ident.length() > IRCD->MaxUser)
			return false;

		/*
		 * If the user name begins with a tilde (~), make sure there is at least
		 * one succeeding character.
		 */
		unsigned i = ident[0] == '~';
		if (i >= ident.length())
			return false;

		/* User names may not start with a '-', '_', or '.'. */
		const char &a = ident[i];
		if (a == '-' || a == '_' || a == '.')
			return false;

		for (const auto c : ident)
		{
			/* A tilde can only be used as the first character of a user name. */
			if (c == '~' && i == 0)
				continue;

			if ((c >= 'A' && c <= 'Z') ||
				(c >= 'a' && c <= 'z') ||
				(c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.')
				continue;

			return false;
		}

		return true;
	}
};

struct IRCDMessageBMask final
	: IRCDMessage
{
	IRCDMessageBMask(Module *creator) : IRCDMessage(creator, "BMASK", 4) { SetFlag(FLAG_REQUIRE_SERVER); }

	/*            0          1        2 3               */
	/* :0MC BMASK 1350157102 #channel b :*!*@*.test.com */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		Channel *c = Channel::Find(params[1]);
		ChannelMode *mode = ModeManager::FindChannelModeByChar(params[2][0]);

		if (c && mode)
		{
			spacesepstream bans(params[3]);
			Anope::string token;

			while (bans.GetToken(token))
				c->SetModeInternal(source, mode, token);
		}
	}
};

struct IRCDMessageCapab final
	: Message::Capab
{
	IRCDMessageCapab(Module *creator) : Message::Capab(creator, "CAPAB") { SetFlag(FLAG_SOFT_LIMIT); }

	/*       0                 */
	/* CAPAB :TBURST EOB MLOCK */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		spacesepstream sep(params[0]);
		Anope::string capab;

		while (sep.GetToken(capab))
		{
			if (capab.find("HOP") != Anope::string::npos || capab.find("RHOST") != Anope::string::npos)
				ModeManager::AddChannelMode(new ChannelModeStatus("HALFOP", 'h', '%', 1));
			if (capab.find("AOP") != Anope::string::npos)
				ModeManager::AddChannelMode(new ChannelModeStatus("PROTECT", 'a', '&', 3));
			if (capab.find("QOP") != Anope::string::npos)
				ModeManager::AddChannelMode(new ChannelModeStatus("OWNER", 'q', '~', 4));
		}

		Message::Capab::Run(source, params, tags);
	}
};

struct IRCDMessageCertFP: IRCDMessage
{
	IRCDMessageCertFP(Module *creator) : IRCDMessage(creator, "CERTFP", 1) { SetFlag(FLAG_REQUIRE_USER); }

	/*                   0                                                                */
	/* :0MCAAAAAB CERTFP 4C62287BA6776A89CD4F8FF10A62FFB35E79319F51AF6C62C674984974FCCB1D */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		User *u = source.GetUser();

		u->fingerprint = params[0];
		FOREACH_MOD(OnFingerprint, (u));
	}
};

struct IRCDMessageEOB final
	: IRCDMessage
{
	IRCDMessageEOB(Module *creator) : IRCDMessage(creator, "EOB", 0) { SetFlag(FLAG_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		source.GetServer()->Sync(true);
	}
};

struct IRCDMessageJoin final
	: Message::Join
{
	IRCDMessageJoin(Module *creator) : Message::Join(creator, "JOIN") { }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (params.size() < 2)
			return;

		std::vector<Anope::string> p = params;
		p.erase(p.begin());

		Message::Join::Run(source, p, tags);
	}
};

struct IRCDMessageMetadata final
	: IRCDMessage
{
	IRCDMessageMetadata(Module *creator) : IRCDMessage(creator, "METADATA", 3) { SetFlag(FLAG_REQUIRE_SERVER); }

	/*               0      1         2      3                                                                 */
	/* :0MC METADATA client 0MCAAAAAB certfp :4C62287BA6776A89CD4F8FF10A62FFB35E79319F51AF6C62C674984974FCCB1D */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (params[0].equals_cs("client"))
		{
			User *u = User::Find(params[1]);
			if (!u)
			{
				Log(LOG_DEBUG) << "METADATA for nonexistent user " << params[1];
				return;
			}

			if (params[2].equals_cs("certfp"))
			{
				u->fingerprint = params[3];
				FOREACH_MOD(OnFingerprint, (u));
			}
		}
	}
};

struct IRCDMessageMLock final
	: IRCDMessage
{
	IRCDMessageMLock(Module *creator) : IRCDMessage(creator, "MLOCK", 4) { SetFlag(FLAG_REQUIRE_SERVER); }

	/*            0          1        2          3   */
	/* :0MC MLOCK 1350157102 #channel 1350158923 :nt */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		Channel *c = Channel::Find(params[1]);

		if (c && c->ci)
		{
			ModeLocks *modelocks = c->ci->GetExt<ModeLocks>("modelocks");
			Anope::string modes;

			if (modelocks)
				modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");

			// Mode lock string is not what we say it is?
			if (modes != params[3])
				Uplink::Send("MLOCK", c->created, c->name, Anope::CurTime, modes);
		}
	}
};

struct IRCDMessageNick final
	: IRCDMessage
{
	IRCDMessageNick(Module *creator) : IRCDMessage(creator, "NICK", 2) { SetFlag(FLAG_REQUIRE_USER); }

	/*                 0       1          */
	/* :0MCAAAAAB NICK newnick 1350157102 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		source.GetUser()->ChangeNick(params[0], IRCD->ExtractTimestamp(params[1]));
	}
};

struct IRCDMessagePass final
	: IRCDMessage
{
	IRCDMessagePass(Module *creator) : IRCDMessage(creator, "PASS", 1) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	/*      0        */
	/* PASS password */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		if (params.size() == 4)
			UplinkSID = params[3];
	}
};

struct IRCDMessagePong final
	: IRCDMessage
{
	IRCDMessagePong(Module *creator) : IRCDMessage(creator, "PONG", 0) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		source.GetServer()->Sync(false);
	}
};

struct IRCDMessageServer final
	: IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	/*        0          1 2   3 4                        */
	/* SERVER hades.arpa 1 4XY + :ircd-hybrid test server */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		/* Servers other than our immediate uplink are introduced via SID */
		if (params[1] != "1")
			return;

		if (params.size() == 5)
		{
			UplinkSID = params[2];
			UseSVSAccount = true;
		}

		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], 1, params.back(), UplinkSID);

		IRCD->SendPing(Me->GetName(), params[0]);
	}
};

struct IRCDMessageSID final
	: IRCDMessage
{
	IRCDMessageSID(Module *creator) : IRCDMessage(creator, "SID", 5) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	/*          0          1 2   3 4                        */
	/* :0MC SID hades.arpa 2 4XY + :ircd-hybrid test server */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		auto hops = Anope::Convert(params[1], 0);
		new Server(source.GetServer() == NULL ? Me : source.GetServer(), params[0], hops, params.back(), params[2]);

		IRCD->SendPing(Me->GetName(), params[0]);
	}
};

struct IRCDMessageSJoin final
	: IRCDMessage
{
	IRCDMessageSJoin(Module *creator) : IRCDMessage(creator, "SJOIN", 4) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	/*            0          1       2   3                      */
	/* :0MC SJOIN 1654877335 #nether +nt :@0MCAAAAAB +0MCAAAAAC */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		std::list<Message::Join::SJoinUser> users;

		spacesepstream sep(params[params.size() - 1]);
		Anope::string buf;

		while (sep.GetToken(buf))
		{
			Message::Join::SJoinUser sju;

			/* Get prefixes from the nick */
			for (char ch; (ch = ModeManager::GetStatusChar(buf[0])); )
			{
				buf.erase(buf.begin());
				sju.first.AddMode(ch);
			}

			sju.second = User::Find(buf);
			if (!sju.second)
			{
				Log(LOG_DEBUG) << "SJOIN for nonexistent user " << buf << " on " << params[1];
				continue;
			}

			users.push_back(sju);
		}

		auto ts = IRCD->ExtractTimestamp(params[0]);
		Message::Join::SJoin(source, params[1], ts, params[2], { params.begin() + 3, params.end() - 1 }, users);
	}
};

struct IRCDMessageSVSMode final
	: IRCDMessage
{
	IRCDMessageSVSMode(Module *creator) : IRCDMessage(creator, "SVSMODE", 3) { SetFlag(FLAG_SOFT_LIMIT); }

	/*              0         1          2  */
	/* :0MC SVSMODE 0MCAAAAAB 1350157102 +r */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		User *u = User::Find(params[0]);

		if (!u)
			return;

		if (IRCD->ExtractTimestamp(params[1]) != u->timestamp)
			return;

		u->SetModesInternal(source, params[2]);
	}
};

struct IRCDMessageTBurst final
	: IRCDMessage
{
	IRCDMessageTBurst(Module *creator) : IRCDMessage(creator, "TBURST", 5) { }

	/*             0          1       2          3                      4                      */
	/* :0MC TBURST 1654867975 #nether 1654877335 Steve!~steve@the.mines :Join the ghast nation */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		Anope::string setter;
		sepstream(params[3], '!').GetToken(setter, 0);
		auto topic_time = IRCD->ExtractTimestamp(params[2]);
		Channel *c = Channel::Find(params[1]);

		if (c)
			c->ChangeTopicInternal(NULL, setter, params[4], topic_time);
	}
};

struct IRCDMessageTMode final
	: IRCDMessage
{
	IRCDMessageTMode(Module *creator) : IRCDMessage(creator, "TMODE", 3) { SetFlag(FLAG_SOFT_LIMIT); }

	/*            0          1       2    */
	/* :0MC TMODE 1654867975 #nether +ntR */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		auto ts = IRCD->ExtractTimestamp(params[0]);
		Channel *c = Channel::Find(params[1]);

		if (c)
			c->SetModesInternal(source, params[2], { params.begin() + 3, params.end() }, ts);
	}
};

struct IRCDMessageUID final
	: IRCDMessage
{
	IRCDMessageUID(Module *creator) : IRCDMessage(creator, "UID", 11) { SetFlag(FLAG_REQUIRE_SERVER); SetFlag(FLAG_SOFT_LIMIT); }

	/*          0     1 2          3   4      5            6         7        8         9     10                   */
	/* :0MC UID Steve 1 1350157102 +oi ~steve virtual.host real.host 10.0.0.1 0MCAAAAAB Steve :Mining all the time */
	void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) override
	{
		NickAlias *na = NULL;

		if (params[9] != "*")
			na = NickAlias::Find(params[9]);

		/* Source is always the server */
		User::OnIntroduce(params[0], params[4], params[6], params[5], params[7], source.GetServer(), params[10],
				IRCD->ExtractTimestamp(params[2]), params[3], params[8], na ? *na->nc : NULL);
	}
};

class ProtoHybrid final
	: public Module
{
	HybridProto ircd_proto;

	/* Core message handlers */
	Message::Away message_away;
	Message::Error message_error;
	Message::Invite message_invite;
	Message::Kick message_kick;
	Message::Kill message_kill;
	Message::Mode message_mode;
	Message::MOTD message_motd;
	Message::Notice message_notice;
	Message::Part message_part;
	Message::Ping message_ping;
	Message::Privmsg message_privmsg;
	Message::Quit message_quit;
	Message::SQuit message_squit;
	Message::Stats message_stats;
	Message::Time message_time;
	Message::Topic message_topic;
	Message::Version message_version;
	Message::Whois message_whois;

	/* Our message handlers */
	IRCDMessageBMask message_bmask;
	IRCDMessageCapab message_capab;
	IRCDMessageCertFP message_certfp;
	IRCDMessageEOB message_eob;
	IRCDMessageJoin message_join;
	IRCDMessageMetadata message_metadata;
	IRCDMessageMLock message_mlock;
	IRCDMessageNick message_nick;
	IRCDMessagePass message_pass;
	IRCDMessagePong message_pong;
	IRCDMessageServer message_server;
	IRCDMessageSID message_sid;
	IRCDMessageSJoin message_sjoin;
	IRCDMessageSVSMode message_svsmode;
	IRCDMessageTBurst message_tburst;
	IRCDMessageTMode message_tmode;
	IRCDMessageUID message_uid;

	static void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserModeOperOnly("ADMIN", 'a'));
		ModeManager::AddUserMode(new UserMode("CALLERID", 'g'));
		ModeManager::AddUserMode(new UserMode("INVIS", 'i'));
		ModeManager::AddUserMode(new UserModeOperOnly("LOCOPS", 'l'));
		ModeManager::AddUserMode(new UserModeOperOnly("OPER", 'o'));
		ModeManager::AddUserMode(new UserMode("HIDECHANS", 'p'));
		ModeManager::AddUserMode(new UserMode("HIDEIDLE", 'q'));
		ModeManager::AddUserMode(new UserModeNoone("REGISTERED", 'r'));
		ModeManager::AddUserMode(new UserModeOperOnly("SNOMASK", 's'));
		ModeManager::AddUserMode(new UserMode("WALLOPS", 'w'));
		ModeManager::AddUserMode(new UserMode("BOT", 'B'));
		ModeManager::AddUserMode(new UserMode("DEAF", 'D'));
		ModeManager::AddUserMode(new UserMode("SOFTCALLERID", 'G'));
		ModeManager::AddUserMode(new UserModeOperOnly("HIDEOPER", 'H'));
		ModeManager::AddUserMode(new UserMode("REGPRIV", 'R'));
		ModeManager::AddUserMode(new UserModeNoone("SSL", 'S'));
		ModeManager::AddUserMode(new UserModeNoone("WEBIRC", 'W'));
		ModeManager::AddUserMode(new UserMode("SECUREONLY", 'Z'));

		/* b/e/I */
		ModeManager::AddChannelMode(new ChannelModeList("BAN", 'b'));
		ModeManager::AddChannelMode(new ChannelModeList("EXCEPT", 'e'));
		ModeManager::AddChannelMode(new ChannelModeList("INVITEOVERRIDE", 'I'));

		/* v/o */
		ModeManager::AddChannelMode(new ChannelModeStatus("VOICE", 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus("OP", 'o', '@', 2));

		/* l/k */
		ModeManager::AddChannelMode(new ChannelModeParam("LIMIT", 'l', true));
		ModeManager::AddChannelMode(new ChannelModeKey('k'));

		/* Add channel modes */
		ModeManager::AddChannelMode(new ChannelMode("BLOCKCOLOR", 'c'));
		ModeManager::AddChannelMode(new ChannelMode("INVITE", 'i'));
		ModeManager::AddChannelMode(new ChannelMode("MODERATED", 'm'));
		ModeManager::AddChannelMode(new ChannelMode("NOEXTERNAL", 'n'));
		ModeManager::AddChannelMode(new ChannelMode("PRIVATE", 'p'));
		ModeManager::AddChannelMode(new ChannelModeNoone("REGISTERED", 'r'));
		ModeManager::AddChannelMode(new ChannelMode("SECRET", 's'));
		ModeManager::AddChannelMode(new ChannelMode("TOPIC", 't'));
		ModeManager::AddChannelMode(new ChannelMode("NOCTCP", 'C'));
		ModeManager::AddChannelMode(new ChannelMode("NOKNOCK", 'K'));
		ModeManager::AddChannelMode(new ChannelModeOperOnly("LBAN", 'L'));
		ModeManager::AddChannelMode(new ChannelMode("REGMODERATED", 'M'));
		ModeManager::AddChannelMode(new ChannelMode("NONICK", 'N'));
		ModeManager::AddChannelMode(new ChannelModeOperOnly("OPERONLY", 'O'));
		ModeManager::AddChannelMode(new ChannelMode("NOKICK", 'Q'));
		ModeManager::AddChannelMode(new ChannelMode("REGISTEREDONLY", 'R'));
		ModeManager::AddChannelMode(new ChannelMode("SSL", 'S'));
		ModeManager::AddChannelMode(new ChannelMode("NONOTICE", 'T'));
		ModeManager::AddChannelMode(new ChannelMode("NOINVITE", 'V'));
		ModeManager::AddChannelMode(new ChannelModeNoone("ISSECURE", 'Z'));
	}

public:
	ProtoHybrid(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR),
		ircd_proto(this),
		message_away(this),
		message_error(this),
		message_invite(this),
		message_kick(this),
		message_kill(this),
		message_mode(this),
		message_motd(this),
		message_notice(this),
		message_part(this),
		message_ping(this),
		message_privmsg(this),
		message_quit(this),
		message_squit(this),
		message_stats(this),
		message_time(this),
		message_topic(this),
		message_version(this),
		message_whois(this),
		message_bmask(this),
		message_capab(this),
		message_certfp(this),
		message_eob(this),
		message_join(this),
		message_metadata(this),
		message_mlock(this),
		message_nick(this),
		message_pass(this),
		message_pong(this),
		message_server(this),
		message_sid(this),
		message_sjoin(this),
		message_svsmode(this),
		message_tburst(this),
		message_tmode(this),
		message_uid(this)
	{
		if (IRCD == &ircd_proto)
			this->AddModes();
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
		if (modelocks && Servers::Capab.count("MLOCK"))
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");
			Uplink::Send("MLOCK", c->created, c->ci->name, Anope::CurTime, modes);
		}
	}

	void OnDelChan(ChannelInfo *ci) override
	{
		if (ci->c && Servers::Capab.count("MLOCK"))
			Uplink::Send("MLOCK", ci->c->created, ci->name, Anope::CurTime, "");
	}

	EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock) override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (cm && ci->c && modelocks && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK"))
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->mchar;
			Uplink::Send("MLOCK", ci->c->created, ci->name, Anope::CurTime, modes);
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChannelInfo *ci, ModeLock *lock) override
	{
		ModeLocks *modelocks = ci->GetExt<ModeLocks>("modelocks");
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (cm && modelocks && ci->c && (cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && Servers::Capab.count("MLOCK"))
		{
			Anope::string modes = modelocks->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->mchar, "");
			Uplink::Send("MLOCK", ci->c->created, ci->name, Anope::CurTime, modes);
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ProtoHybrid)
