/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
 * Copyright (C) 2011-2012, 2014 Alexander Barton <alex@barton.de>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

/* Dependencies: anope_protocol.rfc1459 */

#include "module.h"
#include "modules/protocol/rfc1459.h"

class ngIRCdProto : public IRCDProto
{
	void SendSVSKillInternal(const MessageSource &source, User *user, const Anope::string &buf) override
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

	void SendAkill(User *u, XLine *x) override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->GetExpires() - Anope::CurTime;
		if (timeleft > 172800 || !x->GetExpires())
			timeleft = 172800;
		Uplink::Send(Me, "GLINE", x->GetMask(), timeleft, x->GetReason() + " (" + x->GetBy() + ")");
	}

	void SendAkillDel(XLine *x) override
	{
		Uplink::Send(Me, "GLINE", x->GetMask());
	}

	void SendChannel(Channel *c) override
	{
		Uplink::Send(Me, "CHANINFO", c->name, "+" + c->GetModes(true, true));
	}

	// Received: :dev.anope.de NICK DukeP 1 ~DukePyro p57ABF9C9.dip.t-dialin.net 1 +i :DukePyrolator
	void SendClientIntroduction(User *u) override
	{
		Anope::string modes = "+" + u->GetModes();
		Uplink::Send(Me, "NICK", u->nick, 1, u->GetIdent(), u->host, 1, modes, u->realname);
	}

	void SendConnect() override
	{
		Uplink::Send("PASS", Config->Uplinks[Anope::CurrentUplink].password, "0210-IRC+", "Anope|" + Anope::VersionShort(), "CLHMSo P");
		/* Make myself known to myself in the serverlist */
		SendServer(Me);
		/* finish the enhanced server handshake and register the connection */
		this->SendNumeric(376, "*", ":End of MOTD command");
	}

	void SendForceNickChange(User *u, const Anope::string &newnick, time_t when) override
	{
		Uplink::Send(Me, "SVSNICK", u->nick, newnick);
	}

	void SendGlobalNotice(ServiceBot *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "NOTICE", "$" + dest->GetName(), msg);
	}

	void SendGlobalPrivmsg(ServiceBot *bi, const Server *dest, const Anope::string &msg) override
	{
		Uplink::Send(bi, "PRIVMSG", "$" + dest->GetName(), msg);
	}

	void SendGlobopsInternal(const MessageSource &source, const Anope::string &buf) override
	{
		Uplink::Send(source, "WALLOPS", buf);
	}

	void SendJoin(User *user, Channel *c, const ChannelStatus *status) override
	{
		Uplink::Send(user, "JOIN", c->name);
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

	void SendKickInternal(const MessageSource &source, const Channel *chan, User *user, const Anope::string &buf) override
	{
		if (!buf.empty())
			Uplink::Send(source, "KICK", chan->name, user->nick, buf);
		else
			Uplink::Send(source, "KICK", chan->name, user->nick);
	}

	void SendLogin(User *u, NickServ::Nick *na) override
	{
		Uplink::Send(Me, "METADATA", u->GetUID(), "accountname", na->GetAccount()->GetDisplay());
	}

	void SendLogout(User *u) override
	{
		Uplink::Send(Me, "METADATA", u->GetUID(), "accountname", "");
	}

	void SendModeInternal(const MessageSource &source, const Channel *dest, const Anope::string &buf) override
	{
		IRCMessage message(source, "MODE", dest->name);
		message.TokenizeAndPush(buf);
		Uplink::SendMessage(message);
	}

	void SendPartInternal(User *u, const Channel *chan, const Anope::string &buf) override
	{
		if (!buf.empty())
			Uplink::Send(u, "PART", chan->name, buf);
		else
			Uplink::Send(u, "PART", chan->name);
	}

	/* SERVER name hop descript */
	void SendServer(const Server *server) override
	{
		Uplink::Send("SERVER", server->GetName(), server->GetHops(), server->GetDescription());
	}

	void SendTopic(const MessageSource &source, Channel *c) override
	{
		Uplink::Send(source, "TOPIC", c->name, c->topic);
	}

	void SendVhost(User *u, const Anope::string &vIdent, const Anope::string &vhost) override
	{
		if (!vIdent.empty())
			Uplink::Send(Me, "METADATA", u->nick, "user", vIdent);

		Uplink::Send(Me, "METADATA", u->nick, "cloakhost", vhost);
		if (!u->HasMode("CLOAK"))
		{
			u->SetMode(Config->GetClient("HostServ"), "CLOAK");
			ModeManager::ProcessModes();
		}
	}

	void SendVhostDel(User *u) override
	{
		this->SendVhost(u, u->GetIdent(), "");
	}

	Anope::string Format(IRCMessage &message)
	{
		if (message.GetSource().GetSource().empty())
			message.SetSource(Me);
		return IRCDProto::Format(message);
	}
};

struct IRCDMessage005 : IRCDMessage
{
	IRCDMessage005(Module *creator) : IRCDMessage(creator, "005", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	// Please see <http://www.irc.org/tech_docs/005.html> for details.
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		bool created;
		Channel *c = Channel::FindOrCreate(params[0], created);

		Anope::string modes = params[1];

		if (params.size() == 3)
		{
			c->ChangeTopicInternal(NULL, source.GetName(), params[2], Anope::CurTime);
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
			c->ChangeTopicInternal(NULL, source.GetName(), params[4], Anope::CurTime);
		}

		c->SetModesInternal(source, modes);
	}
};

struct IRCDMessageJoin : rfc1459::Join
{
	IRCDMessageJoin(Module *creator) : rfc1459::Join(creator, "JOIN") { }

	/*
	 * <@po||ux> DukeP: RFC 2813, 4.2.1: the JOIN command on server-server links
	 * separates the modes ("o") with ASCII 7, not space. And you can't see ASCII 7.
	 *
	 * if a user joins a new channel, the ircd sends <channelname>\7<umode>
	 */
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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

		rfc1459::Join::Run(source, new_params);

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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = User::Find(params[0]);
		if (!u)
		{
			Log() << "received METADATA for non-existent user " << params[0];
			return;
		}
		if (params[1].equals_cs("accountname"))
		{
			NickServ::Account *nc = NickServ::FindAccount(params[2]);
			if (nc)
				u->Login(nc);
		}
		else if (params[1].equals_cs("certfp"))
		{
			u->fingerprint = params[2];
			EventManager::Get()->Dispatch(&Event::Fingerprint::OnFingerprint, u);
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (params.size() == 1)
		{
			// we have a nickchange
			source.GetUser()->ChangeNick(params[0]);
		}
		else if (params.size() == 7)
		{
			// a new user is connecting to the network
			Server *s = Server::Find(params[4]);
			if (s == NULL)
			{
				Log(LOG_DEBUG) << "User " << params[0] << " introduced from non-existent server " << params[4] << "?";
				return;
			}
			User::OnIntroduce(params[0], params[2], params[3], "", "", s, params[6], Anope::CurTime, params[5], "", NULL);
			Log(LOG_DEBUG) << "Registered nick \"" << params[0] << "\" on server " << s->GetName() << ".";
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		std::list<rfc1459::Join::SJoinUser> users;

		commasepstream sep(params[1]);
		Anope::string buf;
		while (sep.GetToken(buf))
		{

			rfc1459::Join::SJoinUser sju;

			/* Get prefixes from the nick */
			for (char ch; (ch = ModeManager::GetStatusChar(buf[0]));)
			{
				buf.erase(buf.begin());
				sju.first.AddMode(ch);
			}

			sju.second = User::Find(buf);
			if (!sju.second)
			{
				Log(LOG_DEBUG) << "NJOIN for non-existent user " << buf << " on " << params[0];
				continue;
			}
			users.push_back(sju);
		}

		rfc1459::Join::SJoin(source, params[0], 0, "", users);
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (!source.GetServer()->IsSynced())
			source.GetServer()->Sync(false);
	}
};

struct IRCDMessageServer : IRCDMessage
{
	IRCDMessageServer(Module *creator) : IRCDMessage(creator, "SERVER", 3) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	/*
	 * New directly linked server:
	 *
	 * SERVER tolsun.oulu.fi 1 :Experimental server
	 * 	New server tolsun.oulu.fi introducing itself
	 * 	and attempting to register.
	 *
	 * params[0] = servername
	 * params[1] = hop count
	 * params[2] = server description
	 *
	 * New remote server in the network:
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

	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		if (params.size() == 3)
		{
			// our uplink is introducing itself
			new Server(Me, params[0], 1, params[2], "1");
		}
		else
		{
			// our uplink is introducing a new server
			unsigned int hops = params[1].is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0;
			new Server(source.GetServer(), params[0], hops, params[3], params[2]);
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
	void Run(MessageSource &source, const std::vector<Anope::string> &params) override
	{
		Channel *c = Channel::Find(params[0]);
		if (!c)
		{
			Log(LOG_DEBUG) << "TOPIC for non-existent channel " << params[0];
			return;
		}
		c->ChangeTopicInternal(source.GetUser(), source.GetName(), params[1], Anope::CurTime);
	}
};



class ProtongIRCd : public Module
	, public EventHook<Event::UserNickChange>
{
	ngIRCdProto ircd_proto;

	/* Core message handlers */
	rfc1459::Capab message_capab;
	rfc1459::Error message_error;
	rfc1459::Invite message_invite;
	rfc1459::Kick message_kick;
	rfc1459::Kill message_kill;
	rfc1459::MOTD message_motd;
	rfc1459::Notice message_notice;
	rfc1459::Part message_part;
	rfc1459::Ping message_ping;
	rfc1459::Privmsg message_privmsg, message_squery;
	rfc1459::Quit message_quit;
	rfc1459::SQuit message_squit;
	rfc1459::Stats message_stats;
	rfc1459::Time message_time;
	rfc1459::Version message_version;
	rfc1459::Whois message_whois;

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

 public:
	ProtongIRCd(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR)
		, EventHook<Event::UserNickChange>(this)
		, ircd_proto(this)
		, message_capab(this)
		, message_error(this)
		, message_invite(this)
		, message_kick(this)
		, message_kill(this)
		, message_motd(this)
		, message_notice(this)
		, message_part(this)
		, message_ping(this)
		, message_privmsg(this)
		, message_squery(this, "SQUERY")
		, message_quit(this)
		, message_squit(this)
		, message_stats(this)
		, message_time(this)
		, message_version(this)
		, message_whois(this)

		, message_005(this)
		, message_376(this)
		, message_chaninfo(this)
		, message_join(this)
		, message_metadata(this)
		, message_mode(this)
		, message_nick(this)
		, message_njoin(this)
		, message_pong(this)
		, message_server(this)
		, message_topic(this)
	{

		Servers::Capab.insert("QS");

	}

	void OnUserNickChange(User *u, const Anope::string &) override
	{
		u->RemoveMode(Config->GetClient("NickServ"), "REGISTERED");
	}
};

MODULE_INIT(ProtongIRCd)
