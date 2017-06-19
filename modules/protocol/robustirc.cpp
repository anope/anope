/* RobustIRC
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the UnrealIRCD module.
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/cs_mode.h"
#include "modules/sasl.h"

class RobustIRCProto : public IRCDProto
{
 public:
	RobustIRCProto(Module *creator) : IRCDProto(creator, "RobustIRC v1")
	{
		DefaultPseudoclientModes = "+";
		CanSVSNick = true;
		CanSVSHold = true;
		CanSVSJoin = true;
		MaxModes = 1;
	}

 private:
	void SendTopic(const MessageSource &source, Channel *c) anope_override
	{
		UplinkSocket::Message(source) << "TOPIC " << c->name << " " << c->topic_setter << " " << c->topic_ts << " :" << c->topic;
	}

	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		// not supported
	}

	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		// not supported
	}

	void SendAkillDel(const XLine *x) anope_override
	{
		// not supported
	}

	void SendAkill(User *u, XLine *x) anope_override
	{
		// not supported
	}

	void Parse(const Anope::string &buffer, Anope::string &source, Anope::string &command, std::vector<Anope::string> &params)
	{
		IRCDProto::Parse(buffer, source, command, params);
		// RobustIRC often sends messages that have the full prefix,
		// even for server-to-server messages. To make anope understand
		// them, we just strip the prefix and only leave the nickname.
		if (source.find("!") != Anope::string::npos) {
			source.erase(source.find("!"));
		}
	}

	void SendModeInternal(const MessageSource &source, User *u, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "SVSMODE " << u->nick << " " << buf;
	}

	void SendClientIntroduction(User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message() << "NICK " << u->nick << " 1 " << u->timestamp << " " << u->GetIdent() << " " << u->host << " " << u->server->GetName() << " 0 :" << u->realname;
		UplinkSocket::Message() << "MODE " << u->nick << " " << modes;
	}

	/* SERVER name hop descript */
	/* Unreal 3.2 actually sends some info about itself in the descript area */
	void SendServer(const Server *server) anope_override
	{
		UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :" << server->GetDescription();
	}

	/* JOIN */
	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override
	{

		UplinkSocket::Message(user) << "JOIN " << c->name;
		// TODO: do we need this?
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

	void SendSQLineDel(const XLine *x) anope_override
	{
		// not supported
	}

	void SendSQLine(User *, const XLine *x) anope_override
	{
		// not supported
	}

	void SendSVSO(BotInfo *source, const Anope::string &nick, const Anope::string &flag) anope_override
	{
		// not supported
	}

	/* Functions that use serval cmd functions */

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "PASS :" << Config->Uplinks[Anope::CurrentUplink].password;
		SendServer(Me);
	}

	void SendSVSHold(const Anope::string &nick, time_t t) anope_override
	{
		UplinkSocket::Message(Config->GetClient("NickServ")) << "SVSHOLD " << nick << " " << t << " :Being held for registered user";
	}

	void SendSVSHoldDel(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message(Config->GetClient("NickServ")) << "SVSHOLD " << nick;
	}

	void SendSGLineDel(const XLine *x) anope_override
	{
		// not supported
	}

	void SendSZLineDel(const XLine *x) anope_override
	{
		// not supported
	}

	void SendSZLine(User *, const XLine *x) anope_override
	{
		// not supported
	}

	void SendSGLine(User *, const XLine *x) anope_override
	{
		// not supported
	}

	void SendSVSJoin(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) anope_override
	{
		UplinkSocket::Message(source) << "SVSJOIN " << user->GetUID() << " " << chan;
	}

	void SendSVSPart(const MessageSource &source, User *user, const Anope::string &chan, const Anope::string &param) anope_override
	{
		UplinkSocket::Message(source) << "SVSPART " << user->GetUID() << " " << chan;
	}

	void SendSWhois(const MessageSource &source, const Anope::string &who, const Anope::string &mask) anope_override
	{
		// not supported
	}

	void SendEOB() anope_override
	{
	}

	bool IsNickValid(const Anope::string &nick) anope_override
	{
		return IRCDProto::IsNickValid(nick);
	}

	bool IsChannelValid(const Anope::string &chan) anope_override
	{
		if (chan.find(':') != Anope::string::npos)
			return false;

		return IRCDProto::IsChannelValid(chan);
	}

	void SendLogin(User *u, NickAlias *na) anope_override
	{
		IRCD->SendMode(Config->GetClient("NickServ"), u, "+d %d", u->signon);
	}

	void SendLogout(User *u) anope_override
	{
		IRCD->SendMode(Config->GetClient("NickServ"), u, "+d 0");
	}

	bool IsIdentValid(const Anope::string &ident) anope_override
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

struct IRCDMessageMode : IRCDMessage
{
	IRCDMessageMode(Module *creator, const Anope::string &mname) : IRCDMessage(creator, mname, 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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

struct IRCDMessageNick : IRCDMessage
{
	IRCDMessageNick(Module *creator) : IRCDMessage(creator, "NICK", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

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
	**	  parv[8] = info
	**
	** NICK - change
	**	  source  = oldnick
	**	  parv[0] = new nickname
	*/
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() == 9)
		{
			Anope::string ip;
			Anope::string vhost;

			time_t user_ts = params[2].is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime;

			Server *s = Server::Find(params[5]);
			if (s == NULL)
			{
				Log(LOG_DEBUG) << "User " << params[0] << " introduced from nonexistant server " << params[5] << "?";
				return;
			}
		
			NickAlias *na = NULL;

			if (params[6] == "0")
				;
			else if (params[6].is_pos_number_only())
			{
				if (convertTo<time_t>(params[6]) == user_ts)
					na = NickAlias::Find(params[0]);
			}
			else
			{
				na = NickAlias::Find(params[6]);
			}

			User::OnIntroduce(params[0], params[3], params[4], vhost, ip, s, params[8], user_ts, params[7], "", na ? *na->nc : NULL);
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!source.GetServer()->IsSynced()) {
			source.GetServer()->Sync(false);
		}
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
					Log(LOG_DEBUG) << "SJOIN for nonexistant user " << buf << " on " << params[1];
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
	  	Channel *c = Channel::Find(params[0]);
		if (c)
			c->ChangeTopicInternal(source.GetUser(), params[1], params[3], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime);
	}
};

class ProtoRobustIRC : public Module
{
	RobustIRCProto ircd_proto;

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
	Message::SQuit message_squit;
	Message::Stats message_stats;
	Message::Time message_time;
	Message::Version message_version;
	Message::Whois message_whois;

	/* Our message handlers */
	IRCDMessageMode message_mode, message_svsmode;
	IRCDMessageNick message_nick;
	IRCDMessagePong message_pong;
	IRCDMessageServer message_server;
	IRCDMessageSJoin message_sjoin;
	IRCDMessageTopic message_topic;

	bool use_server_side_mlock;

	void AddModes()
	{
		ModeManager::AddChannelMode(new ChannelModeStatus("VOICE", 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus("OP", 'o', '@', 2));
		ModeManager::AddChannelMode(new ChannelMode("INVITE", 'i'));
		ModeManager::AddChannelMode(new ChannelMode("TOPIC", 't'));
		ModeManager::AddChannelMode(new ChannelMode("NOEXTERNAL", 'n'));
		ModeManager::AddChannelMode(new ChannelMode("SECRET", 's'));
		ModeManager::AddChannelMode(new ChannelModeNoone("REGISTERED", 'r'));

		/* Add user modes */
		ModeManager::AddUserMode(new UserMode("INVIS", 'i'));
		ModeManager::AddUserMode(new UserModeOperOnly("OPER", 'o'));
		ModeManager::AddUserMode(new UserModeNoone("REGISTERED", 'r'));
	}

 public:
	ProtoRobustIRC(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR),
		ircd_proto(this),
		message_away(this), message_error(this), message_invite(this), message_join(this), message_kick(this),
		message_kill(this), message_motd(this), message_notice(this), message_part(this), message_ping(this),
		message_privmsg(this), message_quit(this), message_squit(this), message_stats(this), message_time(this),
		message_version(this), message_whois(this),

		message_mode(this, "MODE"), message_svsmode(this, "SVSMODE"), message_nick(this), message_pong(this),
		message_server(this), message_sjoin(this), message_topic(this)
	{

		this->AddModes();

		ModuleManager::SetPriority(this, PRIORITY_FIRST);
	}
};

MODULE_INIT(ProtoRobustIRC)
