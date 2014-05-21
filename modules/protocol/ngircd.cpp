/*
 * ngIRCd Protocol module for Anope IRC Services
 *
 * (C) 2012 Anope Team <team@anope.org>
 * (C) 2011-2012, 2014 Alexander Barton <alex@barton.de>
 * (C) 2011 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class ngIRCdProto : public IRCDProto
{
	void SendSVSKillInternal(const MessageSource &source, User *user, const Anope::string &buf) anope_override
	{
		IRCDProto::SendSVSKillInternal(source, user, buf);
		user->KillInternal(source, buf);
	}

 public:
	ngIRCdProto(Module *creator) : IRCDProto(creator, "ngIRCd")
	{
		DefaultPseudoclientModes = "+oi";
		CanCertFP = true;
		CanSVSNick = true;
		CanSetVHost = true;
		CanSetVIdent = true;
		MaxModes = 5;
	}

	void SendAkill(User *u, XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->expires - Anope::CurTime;
		if (timeleft > 172800 || !x->expires)
			timeleft = 172800;
		UplinkSocket::Message(Me) << "GLINE " << x->mask << " " << timeleft << " :" << x->GetReason() << " (" << x->by << ")";
	}

	void SendAkillDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(Me) << "GLINE " << x->mask;
	}

	void SendChannel(Channel *c) anope_override
	{
		UplinkSocket::Message(Me) << "CHANINFO " << c->name << " +" << c->GetModes(true, true);
	}

	// Received: :dev.anope.de NICK DukeP 1 ~DukePyro p57ABF9C9.dip.t-dialin.net 1 +i :DukePyrolator
	void SendClientIntroduction(User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "NICK " << u->nick << " 1 " << u->GetIdent() << " " << u->host << " 1 " << modes << " :" << u->realname;
	}

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "PASS " << Config->Uplinks[Anope::CurrentUplink].password << " 0210-IRC+ Anope|" << Anope::VersionShort() << ":CLHMSo P";
		/* Make myself known to myself in the serverlist */
		SendServer(Me);
		/* finish the enhanced server handshake and register the connection */
		this->SendNumeric(376, "*", ":End of MOTD command");
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) anope_override
	{
		UplinkSocket::Message(Me) << "SVSNICK " << u->nick << " " << newnick;
	}

	void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "NOTICE $" << dest->GetName() << " :" << msg;
	}

	void SendGlobalPrivmsg(BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "PRIVMSG $" << dest->GetName() << " :" << msg;
	}

	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "WALLOPS :" << buf;
	}

	void SendJoin(User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		UplinkSocket::Message(user) << "JOIN " << c->name;
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

	void SendKickInternal(const MessageSource &source, const Channel *chan, User *user, const Anope::string &buf) anope_override
	{
		if (!buf.empty())
			UplinkSocket::Message(source) << "KICK " << chan->name << " " << user->nick << " :" << buf;
		else
			UplinkSocket::Message(source) << "KICK " << chan->name << " " << user->nick;
	}

	void SendLogin(User *u, NickAlias *na) anope_override
	{
		UplinkSocket::Message(Me) << "METADATA " << u->GetUID() << " accountname :" << na->nc->display;
	}

	void SendLogout(User *u) anope_override
	{
		UplinkSocket::Message(Me) << "METADATA " << u->GetUID() << " accountname :";
	} 

	void SendModeInternal(const MessageSource &source, const Channel *dest, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "MODE " << dest->name << " " << buf;
	}

	void SendPartInternal(User *u, const Channel *chan, const Anope::string &buf) anope_override
	{
		if (!buf.empty())
			UplinkSocket::Message(u) << "PART " << chan->name << " :" << buf;
		else
			UplinkSocket::Message(u) << "PART " << chan->name;
	}

	/* SERVER name hop descript */
	void SendServer(const Server *server) anope_override
	{
		UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :" << server->GetDescription();
	}

	void SendTopic(const MessageSource &source, Channel *c) anope_override
	{
		UplinkSocket::Message(source) << "TOPIC " << c->name << " :" << c->topic;
	}

	void SendVhost(User *u, const Anope::string &vIdent, const Anope::string &vhost) anope_override
	{
		if (!vIdent.empty())
			UplinkSocket::Message(Me) << "METADATA " << u->nick << " user :" << vIdent;

		UplinkSocket::Message(Me) << "METADATA " << u->nick << " cloakhost :" << vhost;
		if (!u->HasMode("CLOAK"))
		{
			u->SetMode(Config->GetClient("HostServ"), "CLOAK");
			ModeManager::ProcessModes();
		}
	}

	void SendVhostDel(User *u) anope_override
	{
		this->SendVhost(u, u->GetIdent(), "");
	}
};

struct IRCDMessage005 : IRCDMessage
{
	IRCDMessage005(Module *creator) : IRCDMessage(creator, "005", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	// Please see <http://www.irc.org/tech_docs/005.html> for details.
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		size_t pos;
		Anope::string parameter, data;
		for (unsigned i = 0, end = params.size(); i < end; ++i)
		{
			pos = params[i].find('=');
			if (pos != Anope::string::npos)
			{
				parameter = params[i].substr(0, pos);
				data = params[i].substr(pos+1, params[i].length());
				if (parameter == "MODES")
				{
					unsigned maxmodes = convertTo<unsigned>(data);
					IRCD->MaxModes = maxmodes;
				}
				else if (parameter == "NICKLEN")
				{
					unsigned newlen = convertTo<unsigned>(data), len = Config->GetBlock("networkinfo")->Get<unsigned>("nicklen");
					if (len != newlen)
					{
						Log() << "Warning: NICKLEN is " << newlen << " but networkinfo:nicklen is " << len;
					}
				}
			}
		}
	}
};

struct IRCDMessage376 : IRCDMessage
{
	IRCDMessage376(Module *creator) : IRCDMessage(creator, "376", 2) { }

	/*
	 * :ngircd.dev.anope.de 376 services.anope.de :End of MOTD command
	 *
	 * we do nothing here, this function exists only to
	 * avoid the "unknown message from server" message.
	 *
	 */

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
	}
};

struct IRCDMessageChaninfo : IRCDMessage
{
	IRCDMessageChaninfo(Module *creator) : IRCDMessage(creator, "CHANINFO", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*
	 * CHANINFO is used by servers to inform each other about a channel: its
	 * modes, channel key, user limits and its topic. The parameter combination
	 * <key> and <limit> is optional, as well as the <topic> parameter, so that
	 * there are three possible forms of this command:
	 *
	 * CHANINFO <chan> +<modes>
	 * CHANINFO <chan> +<modes> :<topic>
	 * CHANINFO <chan> +<modes> <key> <limit> :<topic>
	 *
	 * The parameter <key> must be ignored if a channel has no key (the parameter
	 * <modes> doesn't list the "k" channel mode). In this case <key> should
	 * contain "*" because the parameter <key> is required by the CHANINFO syntax
	 * and therefore can't be omitted. The parameter <limit> must be ignored when
	 * a channel has no user limit (the parameter <modes> doesn't list the "l"
	 * channel mode). In this case <limit> should be "0".
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		bool created;
		Channel *c = Channel::FindOrCreate(params[0], created);

		Anope::string modes = params[1];

		if (params.size() == 3)
		{
			c->ChangeTopicInternal(source.GetName(), params[2], Anope::CurTime);
		}
		else if (params.size() == 5)
		{
			for (size_t i = 0, end = params[1].length(); i < end; ++i)
			{
				switch(params[1][i])
				{
					case 'k':
						modes += " " + params[2];
						continue;
					case 'l':
						modes += " " + params[3];
						continue;
			}
		}
			c->ChangeTopicInternal(source.GetName(), params[4], Anope::CurTime);
		}

		c->SetModesInternal(source, modes);
	}
};

struct IRCDMessageJoin : Message::Join
{
	IRCDMessageJoin(Module *creator) : Message::Join(creator, "JOIN") { }

	/*
	 * <@po||ux> DukeP: RFC 2813, 4.2.1: the JOIN command on server-server links
	 * separates the modes ("o") with ASCII 7, not space. And you can't see ASCII 7.
	 *
	 * if a user joins a new channel, the ircd sends <channelname>\7<umode>
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *user = source.GetUser();
		size_t pos = params[0].find('\7');
		Anope::string channel, modes;

		if (pos != Anope::string::npos)
		{
			channel = params[0].substr(0, pos);
			modes = '+' + params[0].substr(pos+1, params[0].length()) + " " + user->nick;
		}
		else
		{
			channel = params[0];
		}

		std::vector<Anope::string> new_params;
		new_params.push_back(channel);

		Message::Join::Run(source, new_params);

		if (!modes.empty())
		{
			Channel *c = Channel::Find(channel);
			if (c)
				c->SetModesInternal(source, modes);
		}
	}
};

struct IRCDMessageMetadata : IRCDMessage
{
	IRCDMessageMetadata(Module *creator) : IRCDMessage(creator, "METADATA", 3) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*
	 * Received: :ngircd.dev.anope.de METADATA DukePyrolator host :anope-e2ee5c7d
	 *
	 * params[0] = nick of the user
	 * params[1] = command
	 * params[2] = data
	 *
	 * following commands are supported:
	 *  - "accountname": the account name of a client (can't be empty)
	 *  - "certfp": the certificate fingerprint of a client (can't be empty)
	 *  - "cloakhost" : the cloaked hostname of a client
	 *  - "host": the hostname of a client (can't be empty)
	 *  - "info": info text ("real name") of a client
	 *  - "user": the user name (ident) of a client (can't be empty)
	 */

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = User::Find(params[0]);
		if (!u)
		{
			Log() << "received METADATA for non-existent user " << params[0];
			return;
		}
		if (params[1].equals_cs("accountname"))
		{
			NickCore *nc = NickCore::Find(params[2]);
			if (nc)
				u->Login(nc);
		}
		else if (params[1].equals_cs("certfp"))
		{
			u->fingerprint = params[2];
			FOREACH_MOD(OnFingerprint, (u));
		}
		else if (params[1].equals_cs("cloakhost"))
		{
			if (!params[2].empty())
				u->SetDisplayedHost(params[2]);
		}
		else if (params[1].equals_cs("host"))
		{
			u->SetCloakedHost(params[2]);
		}
		else if (params[1].equals_cs("info"))
		{
			u->SetRealname(params[2]);
		}
		else if (params[1].equals_cs("user"))
		{
			u->SetVIdent(params[2]);
		}
	}
};

struct IRCDMessageMode : IRCDMessage
{
	IRCDMessageMode(Module *creator) : IRCDMessage(creator, "MODE", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	/*
	 * Received: :DukeP MODE #anope +b *!*@*.aol.com
	 * Received: :DukeP MODE #anope +h DukeP
	 * params[0] = channel or nick
	 * params[1] = modes
	 * params[n] = parameters
	 */

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Anope::string modes = params[1];

		for (size_t i = 2; i < params.size(); ++i)
			modes += " " + params[i];

		if (IRCD->IsChannelValid(params[0]))
		{
			Channel *c = Channel::Find(params[0]);

			if (c)
				c->SetModesInternal(source, modes);
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
	 * NICK - NEW
	 * Received: :dev.anope.de NICK DukeP_ 1 ~DukePyro ip-2-201-236-154.web.vodafone.de 1 + :DukePyrolator
	 * Parameters: <nickname> <hopcount> <username> <host> <servertoken> <umode> :<realname>
	 * source = server
	 * params[0] = nick
	 * params[1] = hopcount
	 * params[2] = username/ident
	 * params[3] = host
	 * params[4] = servertoken
	 * params[5] = modes
	 * params[6] = info
	 *
	 * NICK - change
	 * Received: :DukeP_ NICK :test2
	 * source    = oldnick
	 * params[0] = newnick
	 *
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() == 1)
		{
			// we have a nickchange
			source.GetUser()->ChangeNick(params[0]);
		}
		else if (params.size() == 7)
		{
			// a new user is connecting to the network
			User::OnIntroduce(params[0], params[2], params[3], "", "", source.GetServer(), params[6], Anope::CurTime, params[5], "", NULL);
		}
		else
		{
			Log(LOG_DEBUG) << "Received NICK with invalid number of parameters. source = " << source.GetName() << "params[0] = " << params[0] << "params.size() = " << params.size();
		}
	}
};

struct IRCDMessageNJoin : IRCDMessage
{
	IRCDMessageNJoin(Module *creator) : IRCDMessage(creator, "NJOIN",2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); };

	/*
	 * RFC 2813, 4.2.2: Njoin Message:
	 * The NJOIN message is used between servers only.
	 * It is used when two servers connect to each other to exchange
	 * the list of channel members for each channel.
	 *
	 * Even though the same function can be performed by using a succession
	 * of JOIN, this message SHOULD be used instead as it is more efficient.
	 *
	 * Received: :dev.anope.de NJOIN #test :DukeP2,@DukeP,%test,+test2
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		std::list<Message::Join::SJoinUser> users;

		commasepstream sep(params[1]);
		Anope::string buf;
		while (sep.GetToken(buf))
		{

			Message::Join::SJoinUser sju;

			/* Get prefixes from the nick */
			for (char ch; (ch = ModeManager::GetStatusChar(buf[0]));)
			{
				buf.erase(buf.begin());
				sju.first.AddMode(ch);
			}

			sju.second = User::Find(buf);
			if (!sju.second)
			{
				Log(LOG_DEBUG) << "NJOIN for nonexistant user " << buf << " on " << params[0];
				continue;
			}
			users.push_back(sju);
		} 

		Message::Join::SJoin(source, params[0], 0, "", users);
	}
};

struct IRCDMessagePong : IRCDMessage
{
	IRCDMessagePong(Module *creator) : IRCDMessage(creator, "PONG", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*
	 * ngIRCd does not send an EOB, so we send a PING immediately
	 * when receiving a new server and then finish sync once we
	 * get a pong back from that server.
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!source.GetServer()->IsSynced())
			source.GetServer()->Sync(false);
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	/*
	 * SERVER tolsun.oulu.fi 1 :Experimental server
	 * 	New server tolsun.oulu.fi introducing itself
	 * 	and attempting to register.
	 *
	 * RFC 2813 says the server has to send a hopcount
	 * AND a servertoken. Not quite sure what ngIRCd is
	 * sending here.
	 *
	 * params[0] = servername
	 * params[1] = hop count (or servertoken?)
	 * params[2] = server description
	 *
	 *
	 * :tolsun.oulu.fi SERVER csd.bu.edu 5 34 :BU Central Server
	 *	Server tolsun.oulu.fi is our uplink for csd.bu.edu
	 *	which is 5 hops away. The token "34" will be used
	 *	by tolsun.oulu.fi when introducing new users or
	 *	services connected to csd.bu.edu.
	 *
	 * params[0] = servername
	 * params[1] = hop count
	 * params[2] = server numeric
	 * params[3] = server description
	 */

	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() == 3)
		{
			// our uplink is introducing itself
			new Server(Me, params[0], 1, params[2], "");
		}
		else
		{
			// our uplink is introducing a new server
			unsigned int hops = params[1].is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0;
			new Server(source.GetServer(), params[0], hops, params[2], params[3]);
		}
		/*
		 * ngIRCd does not send an EOB, so we send a PING immediately
		 * when receiving a new server and then finish sync once we
		 * get a pong back from that server.
		 */
		IRCD->SendPing(Me->GetName(), params[0]);
	}
};

struct IRCDMessageTopic : IRCDMessage
{
	IRCDMessageTopic(Module *creator) : IRCDMessage(creator, "TOPIC", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	// Received: :DukeP TOPIC #anope :test
	void Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = Channel::Find(params[0]);
		if (!c)
		{
			Log(LOG_DEBUG) << "TOPIC for nonexistant channel " << params[0];
			return;
		}
		c->ChangeTopicInternal(source.GetName(), params[1], Anope::CurTime);
	}
};



class ProtongIRCd : public Module
{
	ngIRCdProto ircd_proto;

	/* Core message handlers */
	Message::Capab message_capab;
	Message::Error message_error;
	Message::Invite message_invite;
	Message::Kick message_kick;
	Message::Kill message_kill;
	Message::MOTD message_motd;
	Message::Notice message_notice;
	Message::Part message_part;
	Message::Ping message_ping;
	Message::Privmsg message_privmsg, message_squery;
	Message::Quit message_quit;
	Message::SQuit message_squit;
	Message::Stats message_stats;
	Message::Time message_time;
	Message::Version message_version;
	Message::Whois message_whois;

	/* Our message handlers */
	IRCDMessage005 message_005;
	IRCDMessage376 message_376;
	IRCDMessageChaninfo message_chaninfo;
	IRCDMessageJoin message_join;
	IRCDMessageMetadata message_metadata;
	IRCDMessageMode message_mode;
	IRCDMessageNick message_nick;
	IRCDMessageNJoin message_njoin;
	IRCDMessagePong message_pong;
	IRCDMessageServer message_server;
	IRCDMessageTopic message_topic;

	void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserMode("NOCTCP", 'b'));
		ModeManager::AddUserMode(new UserMode("BOT", 'B'));
		ModeManager::AddUserMode(new UserMode("COMMONCHANS", 'C'));
		ModeManager::AddUserMode(new UserMode("INVIS", 'i'));
		ModeManager::AddUserMode(new UserModeOperOnly("OPER", 'o'));
		ModeManager::AddUserMode(new UserModeOperOnly("PROTECTED", 'q'));
		ModeManager::AddUserMode(new UserModeOperOnly("RESTRICTED", 'r'));
		ModeManager::AddUserMode(new UserModeNoone("REGISTERED", 'R'));
		ModeManager::AddUserMode(new UserModeOperOnly("SNOMASK", 's'));
		ModeManager::AddUserMode(new UserMode("WALLOPS", 'w'));
		ModeManager::AddUserMode(new UserMode("CLOAK", 'x'));

		/* Add modes for ban, exception, and invite lists */
		ModeManager::AddChannelMode(new ChannelModeList("BAN", 'b'));
		ModeManager::AddChannelMode(new ChannelModeList("EXCEPT", 'e'));
		ModeManager::AddChannelMode(new ChannelModeList("INVITEOVERRIDE", 'I'));

		/* Add channel user modes */
		ModeManager::AddChannelMode(new ChannelModeStatus("VOICE", 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus("HALFOP", 'h', '%', 1));
		ModeManager::AddChannelMode(new ChannelModeStatus("OP", 'o', '@', 2));
		ModeManager::AddChannelMode(new ChannelModeStatus("PROTECT", 'a', '&', 3));
		ModeManager::AddChannelMode(new ChannelModeStatus("OWNER", 'q', '~', 4));

		/* Add channel modes */
		ModeManager::AddChannelMode(new ChannelMode("INVITE", 'i'));
		ModeManager::AddChannelMode(new ChannelModeKey('k'));
		ModeManager::AddChannelMode(new ChannelModeParam("LIMIT", 'l', true));
		ModeManager::AddChannelMode(new ChannelMode("MODERATED", 'm'));
		ModeManager::AddChannelMode(new ChannelMode("REGMODERATED", 'M'));
		ModeManager::AddChannelMode(new ChannelMode("NOEXTERNAL", 'n'));
		ModeManager::AddChannelMode(new ChannelMode("OPERONLY", 'O'));
		ModeManager::AddChannelMode(new ChannelMode("PERM", 'P'));
		ModeManager::AddChannelMode(new ChannelMode("NOKICK", 'Q'));
		ModeManager::AddChannelMode(new ChannelModeNoone("REGISTERED", 'r'));
		ModeManager::AddChannelMode(new ChannelMode("REGISTEREDONLY", 'R'));
		ModeManager::AddChannelMode(new ChannelMode("SECRET", 's'));
		ModeManager::AddChannelMode(new ChannelMode("TOPIC", 't'));
		ModeManager::AddChannelMode(new ChannelMode("NOINVITE", 'V'));
		ModeManager::AddChannelMode(new ChannelMode("SSL", 'z'));
	}

 public:
	ProtongIRCd(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR),
		ircd_proto(this),
		message_capab(this), message_error(this), message_invite(this), message_kick(this), message_kill(this),
		message_motd(this), message_notice(this), message_part(this), message_ping(this), message_privmsg(this),
		message_squery(this, "SQUERY"), message_quit(this), message_squit(this), message_stats(this), message_time(this),
		message_version(this), message_whois(this),

		message_005(this), message_376(this), message_chaninfo(this), message_join(this), message_metadata(this),
		message_mode(this), message_nick(this), message_njoin(this), message_pong(this), message_server(this),
		message_topic(this)
	{

		Servers::Capab.insert("QS");

		this->AddModes();

	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveMode(Config->GetClient("NickServ"), "REGISTERED");
	}
};

MODULE_INIT(ProtongIRCd)
