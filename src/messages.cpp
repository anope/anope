/* Common message handlers
 *
 * (C) 2003-2012 Anope Team
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
#include "extern.h"
#include "uplink.h"
#include "opertype.h"
#include "messages.h"
#include "servers.h"
#include "channels.h"

bool CoreIRCDMessageAway::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	FOREACH_MOD(I_OnUserAway, OnUserAway(source.GetUser(), params.empty() ? "" : params[0]));
	return true;
}

bool CoreIRCDMessageCapab::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	for (unsigned i = 0; i < params.size(); ++i)
		Capab.insert(params[i]);
	return true;
}

bool CoreIRCDMessageError::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	Log(LOG_TERMINAL) << "ERROR: " << params[0];
	quitmsg = "Received ERROR from uplink: " + params[0];

	return true;
}

bool CoreIRCDMessageJoin::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
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
			for (UChannelList::iterator it = user->chans.begin(), it_end = user->chans.end(); it != it_end; )
			{
				ChannelContainer *cc = *it++;

				Anope::string channame = cc->chan->name;
				FOREACH_MOD(I_OnPrePartChannel, OnPrePartChannel(user, cc->chan));
				cc->chan->DeleteUser(user);
				FOREACH_MOD(I_OnPartChannel, OnPartChannel(user, findchan(channame), channame, ""));
			}
			user->chans.clear();
			continue;
		}

		Channel *chan = findchan(channel);
		/* Channel doesn't exist, create it */
		if (!chan)
			chan = new Channel(channel, Anope::CurTime);

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnPreJoinChannel, OnPreJoinChannel(user, chan));

		/* Join the user to the channel */
		chan->JoinUser(user);
		/* Set the proper modes on the user */
		chan_set_correct_modes(user, chan, 1, true);

		/* Modules may want to allow this user in the channel, check.
		 * If not, CheckKick will kick/ban them, don't call OnJoinChannel after this as the user will have
		 * been destroyed
		 */
		if (MOD_RESULT != EVENT_STOP && chan && chan->ci && chan->ci->CheckKick(user))
			continue;

		FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(user, chan));
	}

	return true;
}


bool CoreIRCDMessageKick::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	const Anope::string &channel = params[0];
	const Anope::string &users = params[1];
	const Anope::string &reason = params.size() > 2 ? params[2] : "";

	Channel *c = findchan(channel);
	if (!c)
		return true;

	Anope::string user;
	commasepstream sep(users);

	while (sep.GetToken(user))
		c->KickInternal(source, user, reason);
	return true;
}

bool CoreIRCDMessageKill::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	User *u = finduser(params[0]);
	BotInfo *bi;

	if (!u)
		return true;

	/* Recover if someone kills us. */
	if (u->server == Me && (bi = dynamic_cast<BotInfo *>(u)))
	{
		bi->introduced = false;
		introduce_user(bi->nick);
		bi->RejoinAll();
	}
	else
		u->KillInternal(source.GetSource(), params[1]);
	
	return true;
}

/* XXX We should cache the file somewhere not open/read/close it on every request */
bool CoreIRCDMessageMOTD::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	Server *s = Server::Find(params[0]);
	if (s != Me)
		return true;

	FILE *f = fopen(Config->MOTDFilename.c_str(), "r");
	if (f)
	{
		ircdproto->SendNumeric(375, source.GetSource(), ":- %s Message of the Day", Config->ServerName.c_str());
		char buf[BUFSIZE];
		while (fgets(buf, sizeof(buf), f))
		{
			buf[strlen(buf) - 1] = 0;
			ircdproto->SendNumeric(372, source.GetSource(), ":- %s", buf);
		}
		fclose(f);
		ircdproto->SendNumeric(376, source.GetSource(), ":End of /MOTD command.");
	}
	else
		ircdproto->SendNumeric(422, source.GetSource(), ":- MOTD file not found!  Please contact your IRC administrator.");

	return true;
}

bool CoreIRCDMessagePart::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	User *u = source.GetUser();
	const Anope::string &reason = params.size() > 1 ? params[1] : "";

	Anope::string channel;
	commasepstream sep(params[0]);

	while (sep.GetToken(channel))
	{
		dynamic_reference<Channel> c = findchan(channel);

		if (!c || !u->FindChannel(c))
			continue;

		Log(u, c, "part") << "Reason: " << (!reason.empty() ? reason : "No reason");
		FOREACH_MOD(I_OnPrePartChannel, OnPrePartChannel(u, c));
		Anope::string ChannelName = c->name;
		c->DeleteUser(u);
		FOREACH_MOD(I_OnPartChannel, OnPartChannel(u, c, ChannelName, !reason.empty() ? reason : ""));
	}

	return true;
}

bool CoreIRCDMessagePing::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	ircdproto->SendPong(params.size() > 1 ? params[1] : Me->GetSID(), params[0]);
	return true;
}

bool CoreIRCDMessagePrivmsg::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	const Anope::string &receiver = params[0];
	Anope::string message = params[1];

	User *u = source.GetUser();

	if (ircdproto->IsChannelValid(receiver))
	{
		Channel *c = findchan(receiver);
		if (c)
		{
			FOREACH_MOD(I_OnPrivmsg, OnPrivmsg(u, c, message));
		}
	}
	else
	{
		/* If a server is specified (nick@server format), make sure it matches
		 * us, and strip it off. */
		Anope::string botname = receiver;
		size_t s = receiver.find('@');
		if (s != Anope::string::npos)
		{
			Anope::string servername(receiver.begin() + s + 1, receiver.end());
			botname = botname.substr(0, s);
			if (!servername.equals_ci(Config->ServerName))
				return true;
		}
		else if (Config->UseStrictPrivMsg)
		{
			const BotInfo *bi = findbot(receiver);
			if (!bi)
				return true;
			Log(LOG_DEBUG) << "Ignored PRIVMSG without @ from " << u->nick;
			u->SendMessage(bi, _("\"/msg %s\" is no longer supported.  Use \"/msg %s@%s\" or \"/%s\" instead."), receiver.c_str(), receiver.c_str(), Config->ServerName.c_str(), receiver.c_str());
			return true;
		}

		BotInfo *bi = findbot(botname);

		if (bi)
		{
			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnBotPrivmsg, OnBotPrivmsg(u, bi, message));
			if (MOD_RESULT == EVENT_STOP)
				return true;

			if (message[0] == '\1' && message[message.length() - 1] == '\1')
			{
				if (message.substr(0, 6).equals_ci("\1PING "))
				{
					Anope::string buf = message;
					buf.erase(buf.begin());
					buf.erase(buf.end() - 1);
					ircdproto->SendCTCP(bi, u->nick, "%s", buf.c_str());
				}
				else if (message.substr(0, 9).equals_ci("\1VERSION\1"))
				{
					Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
					ircdproto->SendCTCP(bi, u->nick, "VERSION Anope-%s %s :%s - (%s) -- %s", Anope::Version().c_str(), Config->ServerName.c_str(), ircdproto->GetProtocolName().c_str(), enc ? enc->name.c_str() : "unknown", Anope::VersionBuildString().c_str());
				}
				return true;
			}
			
			bi->OnMessage(u, message);
		}
	}

	return true;
}

bool CoreIRCDMessageQuit::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	const Anope::string &reason = params[0];
	User *user = source.GetUser();

	Log(user, "quit") << "quit (Reason: " << (!reason.empty() ? reason : "no reason") << ")";

	NickAlias *na = findnick(user->nick);
	if (na && !na->nc->HasFlag(NI_SUSPENDED) && (user->IsRecognized() || user->IsIdentified(true)))
	{
		na->last_seen = Anope::CurTime;
		na->last_quit = reason;
	}
	FOREACH_MOD(I_OnUserQuit, OnUserQuit(user, reason));
	delete user;

	return true;
}

bool CoreIRCDMessageSQuit::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	Server *s = Server::Find(params[0]);

	if (!s)
	{
		Log() << "SQUIT for nonexistent server " << params[0];
		return true;
	}

	FOREACH_MOD(I_OnServerQuit, OnServerQuit(s));

	s->Delete(s->GetName() + " " + s->GetUplink()->GetName());

	return true;
}

bool CoreIRCDMessageStats::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	User *u = source.GetUser();

	switch (params[0][0])
	{
		case 'l':
			if (u->HasMode(UMODE_OPER))
			{
				ircdproto->SendNumeric(211, source.GetSource(), "Server SendBuf SentBytes SentMsgs RecvBuf RecvBytes RecvMsgs ConnTime");
				ircdproto->SendNumeric(211, source.GetSource(), "%s %d %d %d %d %d %d %ld", Config->Uplinks[CurrentUplink]->host.c_str(), UplinkSock->WriteBufferLen(), TotalWritten, -1, UplinkSock->ReadBufferLen(), TotalRead, -1, static_cast<long>(Anope::CurTime - start_time));
			}

			ircdproto->SendNumeric(219, source.GetSource(), "%c :End of /STATS report.", params[0][0]);
			break;
		case 'o':
		case 'O':
			/* Check whether the user is an operator */
			if (!u->HasMode(UMODE_OPER) && Config->HideStatsO)
				ircdproto->SendNumeric(219, source.GetSource(), "%c :End of /STATS report.", params[0][0]);
			else
			{
				for (unsigned i = 0; i < Config->Opers.size(); ++i)
				{
					Oper *o = Config->Opers[i];

					const NickAlias *na = findnick(o->name);
					if (na)
						ircdproto->SendNumeric(243, source.GetSource(), "O * * %s %s 0", o->name.c_str(), o->ot->GetName().c_str());
				}

				ircdproto->SendNumeric(219, source.GetSource(), "%c :End of /STATS report.", params[0][0]);
			}

			break;
		case 'u':
		{
			time_t uptime = Anope::CurTime - start_time;
			ircdproto->SendNumeric(242, source.GetSource(), ":Services up %d day%s, %02d:%02d:%02d", uptime / 86400, uptime / 86400 == 1 ? "" : "s", (uptime / 3600) % 24, (uptime / 60) % 60, uptime % 60);
			ircdproto->SendNumeric(250, source.GetSource(), ":Current users: %d (%d ops); maximum %d", usercnt, opcnt, maxusercnt);
			ircdproto->SendNumeric(219, source.GetSource(), "%c :End of /STATS report.", params[0][0]);
			break;
		} /* case 'u' */

		default:
			ircdproto->SendNumeric(219, source.GetSource(), "%c :End of /STATS report.", params[0][0]);
	}

	return true;
}

bool CoreIRCDMessageTime::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	time_t t;
	time(&t);
	struct tm *tm = localtime(&t);
	char buf[64];
	strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y %Z", tm);
	ircdproto->SendNumeric(391, source.GetSource(), "%s :%s", Config->ServerName.c_str(), buf);
	return true;
}

bool CoreIRCDMessageTopic::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	Channel *c = findchan(params[0]);
	if (c)
		c->ChangeTopicInternal(source.GetSource(), params[1], Anope::CurTime);

	return true;
}

bool CoreIRCDMessageVersion::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
	ircdproto->SendNumeric(351, source.GetSource(), "Anope-%s %s :%s -(%s) -- %s", Anope::Version().c_str(), Config->ServerName.c_str(), ircdproto->GetProtocolName().c_str(), enc ? enc->name.c_str() : "unknown", Anope::VersionBuildString().c_str());
	return true;
}

bool CoreIRCDMessageWhois::Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
{
	User *u = finduser(params[0]);

	if (u && u->server == Me)
	{
		const BotInfo *bi = findbot(u->nick);
		ircdproto->SendNumeric(311, source.GetSource(), "%s %s %s * :%s", u->nick.c_str(), u->GetIdent().c_str(), u->host.c_str(), u->realname.c_str());
		if (bi)
			ircdproto->SendNumeric(307, source.GetSource(), "%s :is a registered nick", bi->nick.c_str());
		ircdproto->SendNumeric(312, source.GetSource(), "%s %s :%s", u->nick.c_str(), Config->ServerName.c_str(), Config->ServerDesc.c_str());
		if (bi)
			ircdproto->SendNumeric(317, source.GetSource(), "%s %ld %ld :seconds idle, signon time", bi->nick.c_str(), static_cast<long>(Anope::CurTime - bi->lastmsg), static_cast<long>(start_time));
		ircdproto->SendNumeric(318, source.GetSource(), "%s :End of /WHOIS list.", params[0].c_str());
	}
	else
		ircdproto->SendNumeric(401, source.GetSource(), "%s :No such user.", params[0].c_str());

	return true;
}

