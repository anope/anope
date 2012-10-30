/*
 * ngIRCd Protocol module for Anope IRC Services
 *
 * (C) 2012 Anope Team <team@anope.org>
 * (C) 2011-2012 Alexander Barton <alex@barton.de>
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
 public:
	ngIRCdProto() : IRCDProto("ngIRCd")
	{
		DefaultPseudoclientModes = "+oi";
		MaxModes = 5;
	}

	void SendAkill(User *u, XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		UplinkSocket::Message(Me) << "GLINE " << x->Mask << " " << timeleft << " :" << x->GetReason() << " (" << x->By << ")";
	}

	void SendAkillDel(const XLine *x) anope_override
	{
		UplinkSocket::Message(Me) << "GLINE " << x->Mask;
	}

	void SendChannel(Channel *c) anope_override
	{
		UplinkSocket::Message(Me) << "CHANINFO " << c->name << " +" << c->GetModes(true, true);
	}

	// Received: :dev.anope.de NICK DukeP 1 ~DukePyro p57ABF9C9.dip.t-dialin.net 1 +i :DukePyrolator
	void SendClientIntroduction(const User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message(Me) << "NICK " << u->nick << " 1 " << u->GetIdent() << " " << u->host << " 1 " << modes << " :" << u->realname;
	}

	void SendConnect() anope_override
	{
		UplinkSocket::Message() << "PASS " << Config->Uplinks[CurrentUplink]->password << " 0210-IRC+ Anope|" << Anope::VersionShort() << ":CLHSo P";
		/* Make myself known to myself in the serverlist */
		SendServer(Me);
		/* finish the enhanced server handshake and register the connection */
		this->SendNumeric(376, "*", ":End of MOTD command");
	}

	void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "NOTICE $" << dest->GetName() << " :" << msg;
	}

	void SendGlobalPrivmsg(const BotInfo *bi, const Server *dest, const Anope::string &msg) anope_override
	{
		UplinkSocket::Message(bi) << "PRIVMSG $" << dest->GetName() << " :" << msg;
	}

	void SendGlobopsInternal(const BotInfo *source, const Anope::string &buf) anope_override
	{
		if (source)
			UplinkSocket::Message(source) << "WALLOPS :" << buf;
		else
			UplinkSocket::Message(Me) << "WALLOPS :" << buf;
	}

	void SendJoin(const User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		UplinkSocket::Message(user) << "JOIN " << c->name;
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

	void SendKickInternal(const BotInfo *bi, const Channel *chan, const User *user, const Anope::string &buf) anope_override
	{
		if (!buf.empty())
			UplinkSocket::Message(bi) << "KICK " << chan->name << " " << user->nick << " :" << buf;
		else
			UplinkSocket::Message(bi) << "KICK " << chan->name << " " << user->nick;
	}

	void SendLogin(User *u) anope_override { }

	void SendLogout(User *u) anope_override { } 

	void SendModeInternal(const BotInfo *bi, const Channel *dest, const Anope::string &buf) anope_override
	{
		if (bi)
			UplinkSocket::Message(bi) << "MODE " << dest->name << " " << buf;
		else
			UplinkSocket::Message(Me) << "MODE " << dest->name << " " << buf;
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf) anope_override
	{
		if (bi)
			UplinkSocket::Message(bi) << "MODE " << u->nick << " " << buf;
		else
			UplinkSocket::Message(Me) << "MODE " << u->nick << " " << buf;
	}

	void SendPartInternal(const BotInfo *bi, const Channel *chan, const Anope::string &buf) anope_override
	{
		if (!buf.empty())
			UplinkSocket::Message(bi) << "PART " << chan->name << " :" << buf;
		else
			UplinkSocket::Message(bi) << "PART " << chan->name;
	}

	/* SERVER name hop descript */
	void SendServer(const Server *server) anope_override
	{
		UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :" << server->GetDescription();
	}

	void SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf) anope_override
	{
		if (source)
			UplinkSocket::Message(source) << "KILL " << user->nick << " :" << buf;
		else
			UplinkSocket::Message(Me) << "KILL " << user->nick << " :" << buf;
	}

	void SendTopic(BotInfo *bi, Channel *c) anope_override
	{
		UplinkSocket::Message(bi) << "TOPIC " << c->name << " :" << c->topic;
	}
};

struct IRCDMessage005 : IRCDMessage
{
	IRCDMessage005() : IRCDMessage("005", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	// Please see <http://www.irc.org/tech_docs/005.html> for details.
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
					ircdproto->MaxModes = maxmodes;
				}
				else if (parameter == "NICKLEN")
				{
					unsigned newlen = convertTo<unsigned>(data);
					if (Config->NickLen != newlen)
					{
						Log() << "NickLen changed from " << Config->NickLen << " to " << newlen;
						Config->NickLen = newlen;
					}
				}
			}
		}
		return true;
	}
};

struct IRCDMessage376 : IRCDMessage
{
	IRCDMessage376() : IRCDMessage("376", 2) { }

	/*
	 * :ngircd.dev.anope.de 376 services.anope.de :End of MOTD command
	 *
	 * we do nothing here, this function exists only to
	 * avoid the "unknown message from server" message.
	 *
	 */

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		return true;
	}
};

struct IRCDMessageChaninfo : IRCDMessage
{
	IRCDMessageChaninfo() : IRCDMessage("CHANINFO", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

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
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{

		Channel *c = findchan(params[0]);
		if (!c)
			c = new Channel(params[0]);

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

		c->SetModesInternal(source, modes, c->creation_time, true);
		return true;
	}
};

struct IRCDMessageJoin : IRCDMessage
{
	IRCDMessageJoin() : IRCDMessage("JOIN", 1) { }

	/*
	 * <@po||ux> DukeP: RFC 2813, 4.2.1: the JOIN command on server-server links
	 * separates the modes ("o") with ASCII 7, not space. And you can't see ASCII 7.
	 *
	 * if a user joins a new channel, the ircd sends <channelname>\7<umode>
	 */
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *chan;
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

		chan = findchan(channel);
		/* Channel doesn't exist, create it */
		if (!chan)
		{
			chan = new Channel(channel, Anope::CurTime);
			chan->SetFlag(CH_SYNCING);
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnPreJoinChannel, OnPreJoinChannel(user, chan));

		/* Join the user to the channel */
		chan->JoinUser(user);

		if (!modes.empty())
			chan->SetModesInternal(source, modes, chan->creation_time);

		/* Set the proper modes on the user */
		chan_set_correct_modes(user, chan, 1, true);

		/* Modules may want to allow this user in the channel, check.
		 * If not, CheckKick will kick/ban them, don't call OnJoinChannel after this as the user will have
		 * been destroyed
		 */
		if (MOD_RESULT != EVENT_STOP && (!chan->ci || !chan->ci->CheckKick(user)))
		{
			FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(user, chan));
		}

		/* Channel is done syncing */
		if (chan->HasFlag(CH_SYNCING))
		{
			/* Unset the syncing flag */
			chan->UnsetFlag(CH_SYNCING);
			chan->Sync();
		}

		return true;
	}
};


struct IRCDMessageMode : IRCDMessage
{
	IRCDMessageMode() : IRCDMessage("MODE", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	/*
	 * Received: :DukeP MODE #anope +b *!*@*.aol.com
	 * Received: :DukeP MODE #anope +h DukeP
	 * params[0] = channel or nick
	 * params[1] = modes
	 * params[n] = parameters
	 */

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Anope::string modes = params[1];

		for (size_t i = 2; i < params.size(); ++i)
			modes += " " + params[i];

		if (ircdproto->IsChannelValid(params[0]))
		{
			Channel *c = findchan(params[0]);

			if (c)
				c->SetModesInternal(source, modes, c->creation_time);
		}
		else
		{
			User *u = finduser(params[0]);

			if (u)
				u->SetModesInternal("%s", params[1].c_str());
		}
		return true;
	}
};

struct IRCDMessageNick : IRCDMessage
{
	IRCDMessageNick() : IRCDMessage("NICK", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

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
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() == 1)
		{
			// we have a nickchange
			source.GetUser()->ChangeNick(params[0]);
		}
		else if (params.size() == 7)
		{
			// a new user is connecting to the network
			User *user = new User(params[0], params[2], params[3], "", "", source.GetServer(), params[6], Anope::CurTime, params[5], "");
			if (user && nickserv)
				nickserv->Validate(user);
		}
		else
		{
			Log(LOG_DEBUG) << "Received NICK with invalid number of parameters. source = " << source.GetName() << "params[0] = " << params[0] << "params.size() = " << params.size();
		}
		return true;
	}
};

struct IRCDMessageNJoin : IRCDMessage
{
	IRCDMessageNJoin() : IRCDMessage("NJOIN",2) { SetFlag(IRCDMESSAGE_REQUIRE_SERVER); };

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
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = findchan(params[0]);
		commasepstream sep(params[1]);
		Anope::string buf;

		if (!c)
		{
			c = new Channel(params[0], Anope::CurTime);
			c->SetFlag(CH_SYNCING);
		}

		while (sep.GetToken(buf))
		{
			std::list<ChannelMode *> Status;
			char ch;

			/* Get prefixes from the nick */
			while ((ch = ModeManager::GetStatusChar(buf[0])))
			{
				buf.erase(buf.begin());
				ChannelMode *cm = ModeManager::FindChannelModeByChar(ch);
				if (!cm)
				{
					Log(LOG_DEBUG) << "Received unknown mode prefix " << ch << " in NJOIN string.";
					continue;
				}
				Status.push_back(cm);
			}
			User *u = finduser(buf);
			if (!u)
			{
				Log(LOG_DEBUG) << "NJOIN for nonexistant user " << buf << " on " << c->name;
				continue;
			}

			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnPreJoinChannel, OnPreJoinChannel(u, c));

			/* Add the user to the Channel */
			c->JoinUser(u);

			/* Update their status internally on the channel
			 * This will enforce secureops etc on the user
			 */
			for (std::list<ChannelMode *>::iterator it = Status.begin(), it_end = Status.end(); it != it_end; ++it)
				c->SetModeInternal(source, *it, buf);
			/* Now set whatever modes this user is allowed to have on the channel */
			chan_set_correct_modes(u, c, 1, true);

			/* Check to see if modules want the user to join, if they do
			 * check to see if they are allowed to join (CheckKick will kick/ban them)
			 * Don't trigger OnJoinChannel event then as the user will be destroyed
			 */
			if (MOD_RESULT != EVENT_STOP && c->ci && c->ci->CheckKick(u))
				continue;

			FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(u, c));
		} /* while */

		if (c->HasFlag(CH_SYNCING))
		{
			c->UnsetFlag(CH_SYNCING);
			c->Sync();
		}
		return true;
	}
};

struct IRCDMessagePong : IRCDMessage
{
	IRCDMessagePong() : IRCDMessage("PONG", 0) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); SetFlag(IRCDMESSAGE_REQUIRE_SERVER); }

	/*
	 * ngIRCd does not send an EOB, so we send a PING immediately
	 * when receiving a new server and then finish sync once we
	 * get a pong back from that server.
	 */
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!source.GetServer()->IsSynced())
			source.GetServer()->Sync(false);
		return true;
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer() : IRCDMessage("SERVER", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

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

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
		ircdproto->SendPing(Config->ServerName, params[0]);
		return true;
	}
};

struct IRCDMessageTopic : IRCDMessage
{
	IRCDMessageTopic() : IRCDMessage("TOPIC", 2) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	// Received: :DukeP TOPIC #anope :test
	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = findchan(params[0]);
		if (!c)
		{
			Log(LOG_DEBUG) << "TOPIC for nonexistant channel " << params[0];
			return true;
		}
		c->ChangeTopicInternal(source.GetName(), params[1], Anope::CurTime);
		return true;
	}
};



class ProtongIRCd : public Module
{
	ngIRCdProto ircd_proto;

	/* Core message handlers */
	CoreIRCDMessageCapab core_message_capab;
	CoreIRCDMessageError core_message_error;
	CoreIRCDMessageKick core_message_kick;
	CoreIRCDMessageKill core_message_kill;
	CoreIRCDMessageMOTD core_message_motd;
	CoreIRCDMessagePart core_message_part;
	CoreIRCDMessagePing core_message_ping;
	CoreIRCDMessagePrivmsg core_message_privmsg;
	CoreIRCDMessageQuit core_message_quit;
	CoreIRCDMessageSQuit core_message_squit;
	CoreIRCDMessageStats core_message_stats;
	CoreIRCDMessageTime core_message_time;
	CoreIRCDMessageVersion core_message_version;

	/* Our message handlers */
	IRCDMessage005 message_005;
	IRCDMessage376 message_376;
	IRCDMessageChaninfo message_chaninfo;
	IRCDMessageJoin message_join;
	IRCDMessageMode message_mode;
	IRCDMessageNick message_nick;
	IRCDMessageNJoin message_njoin;
	IRCDMessagePong message_pong;
	IRCDMessageServer message_server;
	IRCDMessageTopic message_topic;

	void AddModes()
	{
		/* Add user modes */
		ModeManager::AddUserMode(new UserMode(UMODE_NOCTCP, 'b'));
		ModeManager::AddUserMode(new UserMode(UMODE_BOT, 'B'));
		ModeManager::AddUserMode(new UserMode(UMODE_COMMONCHANS, 'C'));
		ModeManager::AddUserMode(new UserMode(UMODE_INVIS, 'i'));
		ModeManager::AddUserMode(new UserMode(UMODE_OPER, 'o'));
		ModeManager::AddUserMode(new UserMode(UMODE_PROTECTED, 'q'));
		ModeManager::AddUserMode(new UserMode(UMODE_RESTRICTED, 'r'));
		ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, 'R'));
		ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, 's'));
		ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, 'w'));
		ModeManager::AddUserMode(new UserMode(UMODE_CLOAK, 'x'));

		/* Add modes for ban, exception, and invite lists */
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_BAN, 'b'));
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_EXCEPT, 'e'));
		ModeManager::AddChannelMode(new ChannelModeList(CMODE_INVITEOVERRIDE, 'I'));

		/* Add channel user modes */
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, 'v', '+'));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_HALFOP, 'h', '%'));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, 'o', '@'));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT, 'a', '&'));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, 'q','~'));

		/* Add channel modes */
		ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, 'i'));
		ModeManager::AddChannelMode(new ChannelModeKey('k'));
		ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, 'l'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, 'm'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_REGMODERATED, 'M'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, 'n'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_OPERONLY, 'O'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_PERM, 'P'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKICK, 'Q'));
		ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_REGISTEREDONLY, 'R'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, 's'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, 't'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_NOINVITE, 'V'));
		ModeManager::AddChannelMode(new ChannelMode(CMODE_SSL, 'z'));
	}

 public:
	ProtongIRCd(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL)
	{
		this->SetAuthor("Anope");

		Capab.insert("QS");

		this->AddModes();

		Implementation i[] = { I_OnUserNickChange };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
	}
};

MODULE_INIT(ProtongIRCd)
