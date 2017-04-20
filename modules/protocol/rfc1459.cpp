/*
 * Anope IRC Services
 *
 * Copyright (C) 2005-2017 Anope Team <team@anope.org>
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

#include "module.h"
#include "modules/protocol/rfc1459.h"
#include "modules/operserv/stats.h"

using namespace rfc1459;

void senders::GlobalNotice::Send(const MessageSource &source, Server *dest, const Anope::string &msg)
{
	Uplink::Send(source, "NOTICE", "$" + dest->GetName(), msg);
}

void senders::GlobalPrivmsg::Send(const MessageSource& source, Server* dest, const Anope::string& msg)
{
	Uplink::Send(source, "PRIVMSG", "$" + dest->GetName(), msg);
}

void senders::Invite::Send(const MessageSource &source, Channel *chan, User *user)
{
	Uplink::Send(source, "INVITE", user->GetUID(), chan->name);
}

void senders::Join::Send(User* user, Channel* c, const ChannelStatus* status)
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

void senders::Kick::Send(const MessageSource &source, Channel *chan, User *user, const Anope::string &reason)
{
	if (!reason.empty())
		Uplink::Send(source, "KICK", chan->name, user->GetUID(), reason);
	else
		Uplink::Send(source, "KICK", chan->name, user->GetUID());
}

void senders::Kill::Send(const MessageSource &source, const Anope::string &target, const Anope::string &reason)
{
	Uplink::Send(source, "KILL", target, reason);
}

void senders::Kill::Send(const MessageSource &source, User *user, const Anope::string &reason)
{
	Uplink::Send(source, "KILL", user->GetUID(), reason);
	user->KillInternal(source, reason);
}

void senders::ModeChannel::Send(const MessageSource &source, Channel *channel, const Anope::string &modes)
{
	IRCMessage message(source, "MODE", channel->name);
	message.TokenizeAndPush(modes);
	Uplink::SendMessage(message);
}

void senders::ModeUser::Send(const MessageSource &source, User *user, const Anope::string &modes)
{
	IRCMessage message(source, "MODE", user->GetUID());
	message.TokenizeAndPush(modes);
	Uplink::SendMessage(message);
}

void senders::NickChange::Send(User *u, const Anope::string &newnick, time_t ts)
{
	/* this actually isn't rfc1459 which says NICK <nickname> <hopcount> */
	Uplink::Send(u, "NICK", newnick, ts);
}

void senders::Notice::Send(const MessageSource &source, const Anope::string &dest, const Anope::string &msg)
{
	Uplink::Send(source, "NOTICE", dest, msg);
}

void senders::Part::Send(User *u, Channel *chan, const Anope::string &reason)
{
	if (!reason.empty())
		Uplink::Send(u, "PART", chan->name, reason);
	else
		Uplink::Send(u, "PART", chan->name);
}

void senders::Ping::Send(const Anope::string &servname, const Anope::string &who)
{
	if (servname.empty())
		Uplink::Send(Me, "PING", who);
	else
		Uplink::Send(Me, "PING", servname, who);
}

void senders::Pong::Send(const Anope::string &servname, const Anope::string &who)
{
	if (servname.empty())
		Uplink::Send(Me, "PONG", who);
	else
		Uplink::Send(Me, "PONG", servname, who);
}

void senders::Privmsg::Send(const MessageSource &source, const Anope::string &dest, const Anope::string &msg)
{
	Uplink::Send(source, "PRIVMSG", dest, msg);
}

void senders::Quit::Send(User *user, const Anope::string &reason)
{
	if (!reason.empty())
		Uplink::Send(user, "QUIT", reason);
	else
		Uplink::Send(user, "QUIT");
}

void senders::MessageServer::Send(Server* server)
{
	Uplink::Send("SERVER", server->GetName(), server->GetHops() + 1, server->GetDescription());
}

void senders::SQuit::Send(Server *s, const Anope::string &message)
{
	Uplink::Send("SQUIT", s->GetSID(), message);
}

void senders::Topic::Send(const MessageSource &source, Channel *channel, const Anope::string &topic, time_t topic_ts, const Anope::string &topic_setter)
{
	Uplink::Send(source, "TOPIC", channel->name, topic);
}

void senders::Wallops::Send(const MessageSource &source, const Anope::string &msg)
{
	Uplink::Send(source, "WALLOPS", msg);
}

void Away::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	User *u = source.GetUser();
	const Anope::string &msg = !params.empty() ? params[0] : "";

	EventManager::Get()->Dispatch(&Event::UserAway::OnUserAway, u, msg);

	if (!msg.empty())
		u->logger.Category("away").Log(_("{0} is now away: {1}"), u->GetMask(), msg);
	else
		u->logger.Category("away").Log(_("{0} is no longer away"), u->GetMask());
}

void Capab::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	if (params.size() == 1)
	{
		spacesepstream sep(params[0]);
		Anope::string token;
		while (sep.GetToken(token))
			Servers::Capab.insert(token);
	}
	else
		for (unsigned i = 0; i < params.size(); ++i)
			Servers::Capab.insert(params[i]);
}

void Error::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Anope::Logger.Terminal("ERROR: {0}", params[0]);
	Anope::QuitReason = "Received ERROR from uplink: " + params[0];
	Anope::Quitting = true;
}

void Invite::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	User *targ = User::Find(params[0]);
	Channel *c = Channel::Find(params[1]);

	if (!targ || targ->server != Me || !c || c->FindUser(targ))
		return;

	EventManager::Get()->Dispatch(&Event::Invite::OnInvite, source.GetUser(), c, targ);
}

void Join::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	User *user = source.GetUser();
	const Anope::string &channels = params[0];

	Anope::string channel;
	commasepstream sep(channels);

	while (sep.GetToken(channel))
	{
		/* Special case for /join 0 */
		if (channel == "0")
		{
			for (User::ChanUserList::iterator it = user->chans.begin(), it_end = user->chans.end(); it != it_end; )
			{
				ChanUserContainer *cc = it->second;
				Channel *c = cc->chan;
				++it;

				EventManager::Get()->Dispatch(&Event::PrePartChannel::OnPrePartChannel, user, c);
				cc->chan->DeleteUser(user);
				EventManager::Get()->Dispatch(&Event::PartChannel::OnPartChannel, user, c, c->name,  "");
			}
			continue;
		}

		std::list<SJoinUser> users;
		users.push_back(std::make_pair(ChannelStatus(), user));

		Channel *chan = Channel::Find(channel);
		SJoin(source, channel, chan ? chan->creation_time : Anope::CurTime, "", users);
	}
}

void Join::SJoin(MessageSource &source, const Anope::string &chan, time_t ts, const Anope::string &modes, const std::list<SJoinUser> &users)
{
	bool created;
	Channel *c = Channel::FindOrCreate(chan, created, ts ? ts : Anope::CurTime);
	bool keep_their_modes = true;

	if (created)
		c->syncing = true;
	/* Some IRCds do not include a TS */
	else if (!ts)
		;
	/* Our creation time is newer than what the server gave us, so reset the channel to the older time */
	else if (c->creation_time > ts)
	{
		c->creation_time = ts;
		c->Reset();
	}
	/* Their TS is newer, don't accept any modes from them */
	else if (ts > c->creation_time)
		keep_their_modes = false;

	/* Update the modes for the channel */
	if (keep_their_modes && !modes.empty())
		/* If we are syncing, mlock is checked later in Channel::Sync. It is important to not check it here
		 * so that Channel::SetCorrectModes can correctly detect the presence of channel mode +r.
		 */
		c->SetModesInternal(source, modes, ts, !c->syncing);

	for (std::list<SJoinUser>::const_iterator it = users.begin(), it_end = users.end(); it != it_end; ++it)
	{
		const ChannelStatus &status = it->first;
		User *u = it->second;
		keep_their_modes = ts <= c->creation_time; // OnJoinChannel can call modules which can modify this channel's ts

		if (c->FindUser(u))
			continue;

		/* Add the user to the channel */
		c->JoinUser(u, keep_their_modes ? &status : NULL);

		/* Check if the user is allowed to join */
		if (c->CheckKick(u))
			continue;

		/* Set whatever modes the user should have, and remove any that
		 * they aren't allowed to have (secureops etc).
		 */
		c->SetCorrectModes(u, true);

		EventManager::Get()->Dispatch(&Event::JoinChannel::OnJoinChannel, u, c);
	}

	/* Channel is done syncing */
	if (c->syncing)
	{
		/* Sync the channel (mode lock, topic, etc) */
		/* the channel is synced when the netmerge is complete */
		Server *src = source.GetServer() ? source.GetServer() : Me;
		if (src && src->IsSynced())
		{
			c->Sync();

			if (c->CheckDelete())
				c->QueueForDeletion();
		}
	}
}

void Kick::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	const Anope::string &channel = params[0];
	const Anope::string &users = params[1];
	const Anope::string &reason = params.size() > 2 ? params[2] : "";

	Channel *c = Channel::Find(channel);
	if (!c)
		return;

	Anope::string user;
	commasepstream sep(users);

	while (sep.GetToken(user))
		c->KickInternal(source, user, reason);
}

void Kill::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	User *u = User::Find(params[0]);
	ServiceBot *bi;

	if (!u)
		return;

	/* Recover if someone kills us. */
	if (u->server == Me && (bi = dynamic_cast<ServiceBot *>(u)))
	{
		static time_t last_time = 0;

		if (last_time == Anope::CurTime)
		{
			Anope::QuitReason = "Kill loop detected. Are Services U:Lined?";
			Anope::Quitting = true;
			return;
		}
		last_time = Anope::CurTime;

		bi->OnKill();
	}
	else
		u->KillInternal(source, params[1]);
}

void rfc1459::Mode::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Anope::string buf;
	for (unsigned i = 1; i < params.size(); ++i)
		buf += " " + params[i];

	if (IRCD->IsChannelValid(params[0]))
	{
		Channel *c = Channel::Find(params[0]);

		if (c)
			c->SetModesInternal(source, buf.substr(1), 0);
	}
	else
	{
		User *u = User::Find(params[0]);

		if (u)
			u->SetModesInternal(source, "%s", buf.substr(1).c_str());
	}
}

void MOTD::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Server *s = Server::Find(params[0]);
	if (s != Me)
		return;

	/* XXX We should cache the file somewhere not open/read/close it on every request */
	FILE *f = fopen(Config->GetBlock("serverinfo")->Get<Anope::string>("motd").c_str(), "r");
	if (f)
	{
		IRCD->SendNumeric(RPL_MOTDSTART, source.GetUser(), Anope::Format("- {0} Message of the Day", s->GetName()));
		char buf[BUFSIZE];
		while (fgets(buf, sizeof(buf), f))
		{
			buf[strlen(buf) - 1] = 0;
			IRCD->SendNumeric(RPL_MOTD, source.GetUser(), Anope::Format("- {0}", buf));
		}
		fclose(f);
		IRCD->SendNumeric(RPL_YOUREOPER, source.GetUser(), "End of /MOTD command.");
	}
	else
		IRCD->SendNumeric(ERR_NOMOTD, source.GetUser(), "- MOTD file not found!  Please contact your IRC administrator.");
}

void Notice::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Anope::string message = params[1];

	User *u = source.GetUser();

	/* ignore channel notices */
	if (!IRCD->IsChannelValid(params[0]))
	{
		ServiceBot *bi = ServiceBot::Find(params[0]);
		if (!bi)
			return;
		EventManager::Get()->Dispatch(&Event::BotNotice::OnBotNotice, u, bi, message);
	}
}

void Part::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	User *u = source.GetUser();
	const Anope::string &reason = params.size() > 1 ? params[1] : "";

	Anope::string channel;
	commasepstream sep(params[0]);

	while (sep.GetToken(channel))
	{
		Channel *c = Channel::Find(channel);

		if (!c || !u->FindChannel(c))
			continue;

		c->logger.User(u).Category("part").Log(_("{0} parted {1}: {2}"), u->GetMask(), c->GetName(), !reason.empty() ? reason : "No reason");

		EventManager::Get()->Dispatch(&Event::PrePartChannel::OnPrePartChannel, u, c);
		c->DeleteUser(u);
		EventManager::Get()->Dispatch(&Event::PartChannel::OnPartChannel, u, c, c->name, reason);
	}
}

void Ping::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	IRCD->Send<messages::Pong>(params.size() > 1 ? params[1] : Me->GetSID(), params[0]);
}

void Privmsg::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	const Anope::string &receiver = params[0];
	Anope::string message = params[1];

	User *u = source.GetUser();

	if (IRCD->IsChannelValid(receiver))
	{
		Channel *c = Channel::Find(receiver);
		if (c)
		{
			EventManager::Get()->Dispatch(&Event::Privmsg::OnPrivmsg, u, c, message);
		}
	}
	else
	{
		/* If a server is specified (nick@server format), make sure it matches
		 * us, and strip it off. */
		Anope::string botname = receiver;
		size_t s = receiver.find('@');
		bool nick_only = false;
		if (s != Anope::string::npos)
		{
			Anope::string servername(receiver.begin() + s + 1, receiver.end());
			botname = botname.substr(0, s);
			nick_only = true;
			if (!servername.equals_ci(Me->GetName()))
				return;
		}
		else if (!IRCD->RequiresID && Config->UseStrictPrivmsg)
		{
			ServiceBot *bi = ServiceBot::Find(receiver);
			if (!bi)
				return;
			bi->logger.Debug("Ignored PRIVMSG without @ from {0}", u->nick);
			u->SendMessage(bi, _("\"/msg %s\" is no longer supported.  Use \"/msg %s@%s\" or \"/%s\" instead."), bi->nick.c_str(), bi->nick.c_str(), Me->GetName().c_str(), bi->nick.c_str());
			return;
		}

		ServiceBot *bi = ServiceBot::Find(botname, nick_only);

		if (bi)
		{
			if (message[0] == '\1' && message[message.length() - 1] == '\1')
			{
				if (message.substr(0, 6).equals_ci("\1PING "))
				{
					Anope::string buf = message;
					buf.erase(buf.begin());
					buf.erase(buf.end() - 1);
					IRCD->SendCTCPReply(bi, u->nick, buf);
				}
				else if (message.substr(0, 9).equals_ci("\1VERSION\1"))
				{
					Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
					IRCD->SendCTCPReply(bi, u->nick, "VERSION Anope-{0}", Anope::Version());
				}
				return;
			}

			EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&Event::BotPrivmsg::OnBotPrivmsg, u, bi, message);
			if (MOD_RESULT == EVENT_STOP)
				return;

			bi->OnMessage(u, message);
		}
	}
}

void Quit::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	const Anope::string &reason = params[0];
	User *user = source.GetUser();

	user->logger.Category("quit").Log(_("{0} quit: {1}"), user->GetMask(), (!reason.empty() ? reason : "no reason"));

	user->Quit(reason);
}

void SQuit::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Server *s = Server::Find(params[0]);

	if (!s)
	{
		Anope::Logger.Debug("SQUIT for nonexistent server {0}", params[0]);
		return;
	}

	if (s == Me)
	{
		if (Me->GetLinks().empty())
			return;

		s = Me->GetLinks().front();
	}

	s->Delete(s->GetName() + " " + s->GetUplink()->GetName());
}

void rfc1459::Stats::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	User *u = source.GetUser();

	switch (params[0][0])
	{
		case 'l':
			if (u->HasMode("OPER"))
			{
				IRCD->SendNumeric(RPL_STATSLINKINFO, u, "Server", "SendBuf", "SentBytes", "SentMsgs", "RecvBuf", "RecvBytes", "RecvMsgs", "ConnTime");
				IRCD->SendNumeric(RPL_STATSLINKINFO, u, Config->Uplinks[Anope::CurrentUplink].host, UplinkSock->WriteBufferLen(), TotalWritten, -1, UplinkSock->ReadBufferLen(), TotalRead, -1, Anope::CurTime - Anope::StartTime);
			}
			break;
		case 'o':
		case 'O':
			/* Check whether the user is an operator */
			if (u->HasMode("OPER") || !Config->GetBlock("options")->Get<bool>("hidestatso"))
			{
				for (Oper *o : Serialize::GetObjects<Oper *>())
					IRCD->SendNumeric(RPL_STATSOLINE, u, "O", "*", "*", o->GetName(), o->GetType()->GetName().replace_all_cs(" ", "_"), "0");
			}
			break;
		case 'u':
		{
			::Stats *s = Serialize::GetObject<::Stats *>();
			long uptime = static_cast<long>(Anope::CurTime - Anope::StartTime);

			IRCD->SendNumeric(RPL_STATSUPTIME, u, Anope::Format("Services up {0} day{1}, {2}",
					uptime / 86400, uptime / 86400 == 1 ? "" : "s",
					Anope::printf("%02d:%02d:%02d", (uptime / 3600) % 24, (uptime / 60) % 60, uptime % 60)));
			IRCD->SendNumeric(RPL_STATSCONN, u, Anope::Format("Current users: {0} ({1} ops); maximum {2}", UserListByNick.size(), OperCount, s ? s->GetMaxUserCount() : 0));
			break;
		}
	}

	IRCD->SendNumeric(RPL_ENDOFSTATS, u, params[0][0], "End of /STATS report.");
}

void Time::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	char buf[64];
	strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y %Z", tm);
	IRCD->SendNumeric(RPL_TIME, source.GetUser(), Me->GetName(), buf);
}

void Topic::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Channel *c = Channel::Find(params[0]);
	if (c)
		c->ChangeTopicInternal(source.GetUser(), source.GetSource(), params[1], Anope::CurTime);
}

void Version::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
	IRCD->SendNumeric(RPL_VERSION, source.GetUser(), "Anope-" + Anope::Version(), Me->GetName(),
			Anope::Format("{0} - {1} - Built: {2} - Flags: {3}",
				IRCD->GetProtocolName(), enc ? enc->name : "(none)", Anope::VersionBuildTime(), Anope::VersionFlags()));
}

void Whois::Run(MessageSource &source, const std::vector<Anope::string> &params)
{
	User *src = source.GetUser();
	User *u = User::Find(params[0]);

	if (u && u->server == Me)
	{
		const ServiceBot *bi = ServiceBot::Find(u->GetUID());
		IRCD->SendNumeric(RPL_WHOISUSER, src, u->nick, u->GetIdent(), u->host, "*", u->realname);
		if (bi)
			IRCD->SendNumeric(RPL_WHOISREGNICK, src, bi->nick, "is a registered nick");
		IRCD->SendNumeric(RPL_WHOISSERVER, src, u->nick, Me->GetName(), Config->GetBlock("serverinfo")->Get<Anope::string>("description"));
		if (bi)
			IRCD->SendNumeric(RPL_WHOISIDLE, src, bi->nick, Anope::CurTime - bi->lastmsg, bi->signon, "seconds idle, signon time");
		IRCD->SendNumeric(RPL_ENDOFWHOIS, src, u->nick, "End of /WHOIS list.");
	}
	else
	{
		IRCD->SendNumeric(ERR_NOSUCHNICK, src, params[0], "No such user.");
	}
}

class ProtoRFC1459 : public Module
{
 public:
	ProtoRFC1459(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL | VENDOR)
	{
	}
};

MODULE_INIT(ProtoRFC1459)
