/* Definitions of IRC message functions and list of messages.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"

bool OnStats(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() < 1)
		return true;

	User *u = finduser(source);

	switch (params[0][0])
	{
		case 'l':
			if (u && u->HasMode(UMODE_OPER))
			{
				ircdproto->SendNumeric(Config->ServerName, 211, source, "Server SendBuf SentBytes SentMsgs RecvBuf RecvBytes RecvMsgs ConnTime");
				ircdproto->SendNumeric(Config->ServerName, 211, source, "%s %d %d %d %d %d %d %ld", uplink_server->host.c_str(), UplinkSock->WriteBufferLen(), TotalWritten, -1, UplinkSock->ReadBufferLen(), TotalRead, -1, static_cast<long>(Anope::CurTime - start_time));
			}

			ircdproto->SendNumeric(Config->ServerName, 219, source, "%c :End of /STATS report.", params[0][0]);
			break;
		case 'o':
		case 'O':
			/* Check whether the user is an operator */
			if (u && !u->HasMode(UMODE_OPER) && Config->HideStatsO)
				ircdproto->SendNumeric(Config->ServerName, 219, source, "%c :End of /STATS report.", params[0][0]);
			else
			{
				for (unsigned i = 0; i < Config->Opers.size(); ++i)
				{
					Oper *o = Config->Opers[i];

					NickAlias *na = findnick(o->name);
					if (na)
						ircdproto->SendNumeric(Config->ServerName, 243, source, "O * * %s %s 0", o->name.c_str(), o->ot->GetName().c_str());
				}

				ircdproto->SendNumeric(Config->ServerName, 219, source, "%c :End of /STATS report.", params[0][0]);
			}

			break;

		case 'u':
		{
			time_t uptime = Anope::CurTime - start_time;
			ircdproto->SendNumeric(Config->ServerName, 242, source, ":Services up %d day%s, %02d:%02d:%02d", uptime / 86400, uptime / 86400 == 1 ? "" : "s", (uptime / 3600) % 24, (uptime / 60) % 60, uptime % 60);
			ircdproto->SendNumeric(Config->ServerName, 250, source, ":Current users: %d (%d ops); maximum %d", usercnt, opcnt, maxusercnt);
			ircdproto->SendNumeric(Config->ServerName, 219, source, "%c :End of /STATS report.", params[0][0]);
			break;
		} /* case 'u' */

		default:
			ircdproto->SendNumeric(Config->ServerName, 219, source, "%c :End of /STATS report.", params[0][0]);
	}

	return true;
}

bool OnTime(const Anope::string &source, const std::vector<Anope::string> &)
{
	if (source.empty())
		return true;

	time_t t;
	time(&t);
	struct tm *tm = localtime(&t);
	char buf[64];
	strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y %Z", tm);
	ircdproto->SendNumeric(Config->ServerName, 391, source, "%s :%s", Config->ServerName.c_str(), buf);
	return true;
}

bool OnVersion(const Anope::string &source, const std::vector<Anope::string> &)
{
	Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
	ircdproto->SendNumeric(Config->ServerName, 351, source, "Anope-%s %s :%s -(%s) -- %s", Anope::Version().c_str(), Config->ServerName.c_str(), ircd->name, enc ? enc->name.c_str() : "unknown", Anope::VersionBuildString().c_str());
	return true;
}

/* XXX We should cache the file somewhere not open/read/close it on every request */
bool OnMotd(const Anope::string &source, const std::vector<Anope::string> &)
{
	if (source.empty())
		return true;

	FILE *f = fopen(Config->MOTDFilename.c_str(), "r");
	if (f)
	{
		ircdproto->SendNumeric(Config->ServerName, 375, source, ":- %s Message of the Day", Config->ServerName.c_str());
		char buf[BUFSIZE];
		while (fgets(buf, sizeof(buf), f))
		{
			buf[strlen(buf) - 1] = 0;
			ircdproto->SendNumeric(Config->ServerName, 372, source, ":- %s", buf);
		}
		fclose(f);
		ircdproto->SendNumeric(Config->ServerName, 376, source, ":End of /MOTD command.");
	}
	else
		ircdproto->SendNumeric(Config->ServerName, 422, source, ":- MOTD file not found!  Please contact your IRC administrator.");

	return true;
}

#define ProtocolFunc(x) \
	inline bool x(const Anope::string &source, const std::vector<Anope::string> &params) \
	{ \
		return ircdmessage->x(source, params); \
	}

ProtocolFunc(On436)
ProtocolFunc(OnAway)
ProtocolFunc(OnJoin)
ProtocolFunc(OnKick)
ProtocolFunc(OnKill)
ProtocolFunc(OnMode)
ProtocolFunc(OnNick)
ProtocolFunc(OnUID)
ProtocolFunc(OnPart)
ProtocolFunc(OnPing)
ProtocolFunc(OnPrivmsg)
ProtocolFunc(OnQuit)
ProtocolFunc(OnServer)
ProtocolFunc(OnSQuit)
ProtocolFunc(OnTopic)
ProtocolFunc(OnWhois)
ProtocolFunc(OnCapab)
ProtocolFunc(OnSJoin)
ProtocolFunc(OnError)

void init_core_messages()
{
	static Message message_stats("STATS", OnStats);
	static Message message_time("TIME", OnTime);
	static Message message_verssion("VERSION", OnVersion);
	static Message message_motd("MOTD", OnMotd);

	static Message message_436("436", On436);
	static Message message_away("AWAY", OnAway);
	static Message message_join("JOIN", OnJoin);
	static Message message_kick("KICK", OnKick);
	static Message message_kill("KILL", OnKill);
	static Message message_mode("MODE", OnMode);
	static Message message_nick("NICK", OnNick);
	static Message message_uid("UID", OnUID);
	static Message message_part("PART", OnPart);
	static Message message_ping("PING", OnPing);
	static Message message_privmsg("PRIVMSG", OnPrivmsg);
	static Message message_quit("QUIT", OnQuit);
	static Message message_server("SERVER", OnServer);
	static Message message_squit("SQUIT", OnSQuit);
	static Message message_topic("TOPIC", OnTopic);
	static Message message_whois("WHOIS", OnWhois);
	static Message message_capab("CAPAB", OnCapab);
	static Message message_sjoin("SJOIN", OnSJoin);
	static Message message_error("ERROR", OnError);
}

