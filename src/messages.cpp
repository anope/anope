/* Common message handlers
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "users.h"
#include "protocol.h"
#include "config.h"
#include "uplink.h"
#include "opertype.h"
#include "messages.h"
#include "servers.h"
#include "channels.h"
#include "numeric.h"

using namespace Message;

void Away::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	const Anope::string &msg = !params.empty() ? params[0] : "";

	FOREACH_MOD(OnUserAway, (source.GetUser(), msg));
	if (!msg.empty())
		Log(source.GetUser(), "away") << "is now away: " << msg;
	else
		Log(source.GetUser(), "away") << "is no longer away";
}

void Capab::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	if (params.size() == 1)
	{
		spacesepstream sep(params[0]);
		Anope::string token;
		while (sep.GetToken(token))
			Servers::Capab.insert(token);
	}
	else
	{
		for (const auto &param : params)
			Servers::Capab.insert(param);
	}
}

void Error::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	Log(LOG_TERMINAL) << "ERROR: " << params[0];
	Anope::QuitReason = "Received ERROR from uplink: " + params[0];
	Anope::Quitting = true;
}

void Invite::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	User *targ = User::Find(params[0]);
	Channel *c = Channel::Find(params[1]);

	if (!targ || targ->server != Me || !c || c->FindUser(targ))
		return;

	FOREACH_MOD(OnInvite, (source.GetUser(), c, targ));
}

void Join::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
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

				FOREACH_MOD(OnPrePartChannel, (user, c));
				cc->chan->DeleteUser(user);
				FOREACH_MOD(OnPartChannel, (user, c, c->name, ""));
			}
			continue;
		}

		std::list<SJoinUser> users;
		users.emplace_back(ChannelStatus(), user);

		Channel *chan = Channel::Find(channel);
		SJoin(source, channel, chan ? chan->creation_time : Anope::CurTime, "", {}, users);
	}
}

void Join::SJoin(MessageSource &source, const Anope::string &chan, time_t ts, const Anope::string &modes, const std::vector<Anope::string> &modeparams, const std::list<SJoinUser> &users)
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
		c->SetModesInternal(source, modes, modeparams, ts, !c->syncing);

	for (const auto &[status, u] : users)
	{
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

		FOREACH_MOD(OnJoinChannel, (u, c));
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

void Kick::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
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

void Kill::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	User *u = User::Find(params[0]);
	BotInfo *bi;

	if (!u)
		return;

	/* Recover if someone kills us. */
	if (u->server == Me && (bi = dynamic_cast<BotInfo *>(u)))
	{
		static time_t last_time = 0;

		if (last_time == Anope::CurTime)
		{
			Anope::QuitReason = "Kill loop detected. Is Anope U:Lined?";
			Anope::Quitting = true;
			return;
		}
		last_time = Anope::CurTime;

		bi->OnKill();
	}
	else
		u->KillInternal(source, params[1]);
}

void Message::Mode::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	if (IRCD->IsChannelValid(params[0]))
	{
		Channel *c = Channel::Find(params[0]);

		if (c)
			c->SetModesInternal(source, params[1], { params.begin() + 2, params.end() });
	}
	else
	{
		User *u = User::Find(params[0]);

		if (u)
			u->SetModesInternal(source, params[1], { params.begin() + 2, params.end() });
	}
}

/* XXX We should cache the file somewhere not open/read/close it on every request */
void MOTD::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	Server *s = Server::Find(params[0]);
	if (s != Me)
		return;

	auto motdfile = Anope::ExpandConfig(Config->GetBlock("serverinfo")->Get<const Anope::string>("motd"));
	std::ifstream stream(motdfile.str());
	if (!stream.is_open())
	{
		IRCD->SendNumeric(ERR_NOSUCHNICK, source.GetSource(), "- MOTD file not readable!  Please contact your IRC administrator.");
		return;
	}

	IRCD->SendNumeric(RPL_MOTDSTART, source.GetSource(), "- " + s->GetName() + " Message of the Day");
	for (Anope::string line; std::getline(stream, line.str()); )
		IRCD->SendNumeric(RPL_MOTD, source.GetSource(), Anope::printf("- %s", line.c_str()));
	IRCD->SendNumeric(RPL_ENDOFMOTD, source.GetSource(), "End of /MOTD command.");
}

void Notice::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	Anope::string message = params[1];

	User *u = source.GetUser();

	/* ignore channel notices */
	if (!IRCD->IsChannelValid(params[0]))
	{
		BotInfo *bi = BotInfo::Find(params[0]);
		if (!bi)
			return;
		FOREACH_MOD(OnBotNotice, (u, bi, message, tags));
	}
}

void Part::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
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

		Log(u, c, "part") << "Reason: " << (!reason.empty() ? reason : "No reason");
		FOREACH_MOD(OnPrePartChannel, (u, c));
		c->DeleteUser(u);
		FOREACH_MOD(OnPartChannel, (u, c, c->name, !reason.empty() ? reason : ""));
	}
}

void Ping::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	IRCD->SendPong(params.size() > 1 ? params[1] : Me->GetSID(), params[0]);
}

void Privmsg::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	const Anope::string &receiver = params[0];
	Anope::string message = params[1];

	User *u = source.GetUser();

	if (IRCD->IsChannelValid(receiver))
	{
		Channel *c = Channel::Find(receiver);
		if (c)
		{
			FOREACH_MOD(OnPrivmsg, (u, c, message, tags));
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

		BotInfo *bi = BotInfo::Find(botname, nick_only);
		if (bi)
		{
			Anope::string ctcpname, ctcpbody;
			if (Anope::ParseCTCP(message, ctcpname, ctcpbody))
			{
				if (ctcpname.equals_ci("PING"))
				{
					IRCD->SendNotice(bi, u->nick, Anope::FormatCTCP("PING", ctcpbody));
				}
				else if (ctcpname.equals_ci("VERSION"))
				{
					Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
					IRCD->SendNotice(bi, u->nick, Anope::FormatCTCP("VERSION", Anope::printf("Anope-%s %s -- %s -- %s", Anope::Version().c_str(),
						Anope::VersionBuildString().c_str(), IRCD->GetProtocolName().c_str(), enc ? enc->name.c_str() : "(none)")));
				}
				return;
			}

			EventReturn MOD_RESULT;
			FOREACH_RESULT(OnBotPrivmsg, MOD_RESULT, (u, bi, message, tags));
			if (MOD_RESULT == EVENT_STOP)
				return;

			bi->OnMessage(u, message, tags);
		}
	}

	return;
}

void Quit::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	const Anope::string &reason = params[0];
	User *user = source.GetUser();

	Log(user, "quit") << "quit (Reason: " << (!reason.empty() ? reason : "no reason") << ")";

	user->Quit(reason);
}

void SQuit::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	Server *s = Server::Find(params[0]);

	if (!s)
	{
		Log(LOG_DEBUG) << "SQUIT for nonexistent server " << params[0];
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

void Stats::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	User *u = source.GetUser();

	switch (params[0][0])
	{
		case 'l':
			if (u->HasMode("OPER"))
			{
				IRCD->SendNumeric(RPL_STATSLINKINFO, source.GetSource(), "Server SendBuf SentBytes SentMsgs RecvBuf RecvBytes RecvMsgs ConnTime");
				IRCD->SendNumeric(RPL_STATSLINKINFO, source.GetSource(), Config->Uplinks[Anope::CurrentUplink].host, UplinkSock->WriteBufferLen(), TotalWritten, -1, UplinkSock->ReadBufferLen(), TotalRead, -1, Anope::CurTime - Anope::StartTime);
			}

			IRCD->SendNumeric(RPL_STATSLINKINFO, source.GetSource(), params[0][0], "End of /STATS report.");
			break;
		case 'o':
		case 'O':
			/* Check whether the user is an operator */
			if (!u->HasMode("OPER") && Config->GetBlock("options")->Get<bool>("hidestatso"))
				IRCD->SendNumeric(RPL_STATSLINKINFO, source.GetSource(), params[0][0], "End of /STATS report.");
			else
			{
				for (auto *o : Oper::opers)
				{
					const NickAlias *na = NickAlias::Find(o->name);
					if (na)
						IRCD->SendNumeric(RPL_STATSOLINE, source.GetSource(), 'O', '*', '*', o->name, o->ot->GetName().replace_all_cs(" ", "_"), '0');
				}

				IRCD->SendNumeric(RPL_STATSLINKINFO, source.GetSource(), params[0][0], "End of /STATS report.");
			}

			break;
		case 'u':
		{
			long uptime = static_cast<long>(Anope::CurTime - Anope::StartTime);
			IRCD->SendNumeric(RPL_STATSUPTIME, source.GetSource(), Anope::printf("Services up %ld day%s, %02ld:%02ld:%02ld", uptime / 86400, uptime / 86400 == 1 ? "" : "s", (uptime / 3600) % 24, (uptime / 60) % 60, uptime % 60));
			IRCD->SendNumeric(RPL_STATSCONN, source.GetSource(), Anope::printf("Current users: %zu (%d ops); maximum %u", UserListByNick.size(), OperCount, MaxUserCount));
			IRCD->SendNumeric(RPL_STATSLINKINFO, source.GetSource(), params[0][0], "End of /STATS report.");
			break;
		} /* case 'u' */

		default:
			IRCD->SendNumeric(RPL_STATSLINKINFO, source.GetSource(), params[0][0], "End of /STATS report.");
	}

	return;
}

void Time::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	time_t t;
	time(&t);
	struct tm *tm = localtime(&t);
	char buf[64];
	strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y %Z", tm);
	IRCD->SendNumeric(RPL_TIME, source.GetSource(), Me->GetName(), buf);
	return;
}

void Topic::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	Channel *c = Channel::Find(params[0]);
	if (c)
		c->ChangeTopicInternal(source.GetUser(), source.GetSource(), params[1], Anope::CurTime);

	return;
}

void Version::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
	IRCD->SendNumeric(RPL_VERSION, source.GetSource(), "Anope-" + Anope::Version(), Me->GetName(), Anope::printf("%s -(%s) -- %s",
		IRCD->GetProtocolName().c_str(), enc ? enc->name.c_str() : "(none)", Anope::VersionBuildString().c_str()));
}

void Whois::Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags)
{
	User *u = User::Find(params[0]);

	if (u && u->server == Me)
	{
		const BotInfo *bi = BotInfo::Find(u->GetUID());
		IRCD->SendNumeric(RPL_WHOISUSER, source.GetSource(), u->nick, u->GetIdent(), u->host, '*', u->realname);
		if (bi)
			IRCD->SendNumeric(RPL_WHOISREGNICK, source.GetSource(), bi->nick, "is a registered nick");
		IRCD->SendNumeric(RPL_WHOISSERVER, source.GetSource(), u->nick, Me->GetName(), Config->GetBlock("serverinfo")->Get<const Anope::string>("description"));
		if (bi)
			IRCD->SendNumeric(RPL_WHOISIDLE, source.GetSource(), bi->nick, Anope::CurTime - bi->lastmsg, bi->signon, "seconds idle, signon time");
		IRCD->SendNumeric(RPL_WHOISOPERATOR, source.GetSource(), u->nick, "is a Network Service");
		IRCD->SendNumeric(RPL_ENDOFWHOIS, source.GetSource(), u->nick, "End of /WHOIS list.");
	}
	else
		IRCD->SendNumeric(ERR_NOSUCHNICK, source.GetSource(), params[0], "No such user.");
}
