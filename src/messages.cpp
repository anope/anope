/* Definitions of IRC message functions and list of messages.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"

/*************************************************************************/

int m_nickcoll(const Anope::string &user)
{
	introduce_user(user);
	return MOD_CONT;
}

/*************************************************************************/

int m_away(const Anope::string &source, const Anope::string &msg)
{
	User *u = finduser(source);

	if (u && msg.empty()) /* un-away */
		check_memos(u);
	return MOD_CONT;
}

/*************************************************************************/

int m_kill(const Anope::string &nick, const Anope::string &msg)
{
	BotInfo *bi;

	/* Recover if someone kills us. */
	if (!Config->s_BotServ.empty() && (bi = findbot(nick)))
	{
		introduce_user(nick);
		bi->RejoinAll();
	}
	else
		do_kill(nick, msg);

	return MOD_CONT;
}

/*************************************************************************/

int m_time(const Anope::string &source, int ac, const char **av)
{
	if (source.empty())
		return MOD_CONT;

	time_t *t;
	time(t);
	struct tm *tm = localtime(t);
	char buf[64];
	strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y %Z", tm);
	ircdproto->SendNumeric(Config->ServerName, 391, source, "%s :%s", Config->ServerName.c_str(), buf);
	return MOD_CONT;
}

/*************************************************************************/

int m_motd(const Anope::string &source)
{
	if (source.empty())
		return MOD_CONT;

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
	return MOD_CONT;
}

/*************************************************************************/

int m_privmsg(const Anope::string &source, const Anope::string &receiver, const Anope::string &message)
{
	if (source.empty() || receiver.empty() || message.empty())
		return MOD_CONT;

	User *u = finduser(source);

	if (!u)
	{
		Log() << message << ": user record for " << source << " not found";

		BotInfo *bi = findbot(receiver);
		if (bi)
			ircdproto->SendMessage(bi, source, "%s", GetString(USER_RECORD_NOT_FOUND).c_str());

		return MOD_CONT;
	}

	if (receiver[0] == '#' && !Config->s_BotServ.empty())
	{
		ChannelInfo *ci = cs_findchan(receiver);
		if (ci)
		{
			/* Some paranoia checks */
			if (!ci->HasFlag(CI_FORBIDDEN) && ci->c && ci->bi)
				botchanmsgs(u, ci, message);
		}
	}
	else
	{
		/* Check if we should ignore.  Operators always get through. */
		if (allow_ignore && !is_oper(u))
		{
			if (get_ignore(source))
			{
				Anope::string target = myStrGetToken(message, ' ', 0);
				BotInfo *bi = findbot(target);
				if (bi)
					Log(bi) << "Ignored message from " << source << " using command " << target;
				return MOD_CONT;
			}
		}

		/* If a server is specified (nick@server format), make sure it matches
		 * us, and strip it off. */
		Anope::string botname = receiver;
		size_t s = receiver.find('@');
		if (s != Anope::string::npos)
		{
			Anope::string servername(receiver.begin() + s + 1, receiver.end());
			botname = botname.substr(0, s);
			if (!servername.equals_ci(Config->ServerName))
				return MOD_CONT;
		}
		else if (Config->UseStrictPrivMsg)
		{
			BotInfo *bi = findbot(receiver);
			if (!bi)
				return MOD_CONT;
			Log(LOG_DEBUG) << "Ignored PRIVMSG without @ from " << source;
			u->SendMessage(bi, INVALID_TARGET, receiver.c_str(), receiver.c_str(), Config->ServerName.c_str(), receiver.c_str());
			return MOD_CONT;
		}

		BotInfo *bi = findbot(botname);

		if (bi)
		{
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
					ircdproto->SendCTCP(bi, u->nick, "VERSION Anope-%s %s :%s - (%s) -- %s", Anope::Version().c_str(), Config->ServerName.c_str(), ircd->name, Config->EncModuleList.begin()->c_str(), Anope::Build().c_str());
				}
			}
			if (bi == NickServ || bi == MemoServ || bi == BotServ)
				mod_run_cmd(bi, u, message);
			else if (bi == ChanServ)
			{
				if (!is_oper(u) && Config->CSOpersOnly)
					u->SendMessage(ChanServ, ACCESS_DENIED);
				else
					mod_run_cmd(bi, u, message);
			}
			else if (bi == HostServ)
			{
				if (!ircd->vhost)
					u->SendMessage(HostServ, SERVICE_OFFLINE, Config->s_HostServ.c_str());
				else
					mod_run_cmd(bi, u, message);
			}
			else if (bi == OperServ)
			{
				if (!is_oper(u) && Config->OSOpersOnly)
				{
					u->SendMessage(OperServ, ACCESS_DENIED);
					if (Config->WallBadOS)
						ircdproto->SendGlobops(OperServ, "Denied access to %s from %s!%s@%s (non-oper)", Config->s_OperServ.c_str(), u->nick.c_str(), u->GetIdent().c_str(), u->host.c_str());
				}
				else
				{
					Log(OperServ) << u->nick << ": " <<  message;
					mod_run_cmd(bi, u, message);
				}
			}
		}
	}

	return MOD_CONT;
}

/*************************************************************************/

int m_stats(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;

	User *u = finduser(source);

	switch (*av[0])
	{
		case 'l':
			if (u && is_oper(u))
			{
				ircdproto->SendNumeric(Config->ServerName, 211, source, "Server SendBuf SentBytes SentMsgs RecvBuf RecvBytes RecvMsgs ConnTime");
				ircdproto->SendNumeric(Config->ServerName, 211, source, "%s %d %d %d %d %d %d %ld", uplink_server->host.c_str(), UplinkSock->WriteBufferLen(), TotalWritten, -1, UplinkSock->ReadBufferLen(), TotalRead, -1, Anope::CurTime - start_time);
			}

			ircdproto->SendNumeric(Config->ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
			break;
		case 'o':
		case 'O':
			/* Check whether the user is an operator */
			if (u && !is_oper(u) && Config->HideStatsO)
				ircdproto->SendNumeric(Config->ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
			else
			{
				std::list<std::pair<Anope::string, Anope::string> >::iterator it, it_end;

				for (it = Config->Opers.begin(), it_end = Config->Opers.end(); it != it_end; ++it)
				{
					Anope::string nick = it->first, type = it->second;

					NickCore *nc = findcore(nick);
					if (nc)
						ircdproto->SendNumeric(Config->ServerName, 243, source, "O * * %s %s 0", nick.c_str(), type.c_str());
				}

				ircdproto->SendNumeric(Config->ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
			}

			break;

		case 'u':
		{
			time_t uptime = Anope::CurTime - start_time;
			ircdproto->SendNumeric(Config->ServerName, 242, source, ":Services up %d day%s, %02d:%02d:%02d", uptime / 86400, uptime / 86400 == 1 ? "" : "s", (uptime / 3600) % 24, (uptime / 60) % 60, uptime % 60);
			ircdproto->SendNumeric(Config->ServerName, 250, source, ":Current users: %d (%d ops); maximum %d", usercnt, opcnt, maxusercnt);
			ircdproto->SendNumeric(Config->ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
			break;
		} /* case 'u' */

		default:
			ircdproto->SendNumeric(Config->ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
	}
	return MOD_CONT;
}

/*************************************************************************/

int m_version(const Anope::string &source, int ac, const char **av)
{
	if (!source.empty())
		ircdproto->SendNumeric(Config->ServerName, 351, source, "Anope-%s %s :%s -(%s) -- %s", Anope::Version().c_str(), Config->ServerName.c_str(), ircd->name, Config->EncModuleList.begin()->c_str(), Anope::Build().c_str());
	return MOD_CONT;
}

/*************************************************************************/

int m_whois(const Anope::string &source, const Anope::string &who)
{
	if (!source.empty() && !who.empty())
	{
		User *u;
		BotInfo *bi = findbot(who);
		if (bi)
		{
			ircdproto->SendNumeric(Config->ServerName, 311, source, "%s %s %s * :%s", bi->nick.c_str(), bi->GetIdent().c_str(), bi->host.c_str(), bi->realname.c_str());
			ircdproto->SendNumeric(Config->ServerName, 307, source, "%s :is a registered nick", bi->nick.c_str());
			ircdproto->SendNumeric(Config->ServerName, 312, source, "%s %s :%s", bi->nick.c_str(), Config->ServerName.c_str(), Config->ServerDesc.c_str());
			ircdproto->SendNumeric(Config->ServerName, 317, source, "%s %ld %ld :seconds idle, signon time", bi->nick.c_str(), Anope::CurTime - bi->lastmsg, start_time);
			ircdproto->SendNumeric(Config->ServerName, 318, source, "%s :End of /WHOIS list.", who.c_str());
		}
		else if (!ircd->svshold && (u = finduser(who)) && u->server == Me)
		{
			ircdproto->SendNumeric(Config->ServerName, 311, source, "%s %s %s * :%s", u->nick.c_str(), u->GetIdent().c_str(), u->host.c_str(), u->realname.c_str());
			ircdproto->SendNumeric(Config->ServerName, 312, source, "%s %s :%s", u->nick.c_str(), Config->ServerName.c_str(), Config->ServerDesc.c_str());
			ircdproto->SendNumeric(Config->ServerName, 318, source, "%s :End of /WHOIS list.", u->nick.c_str());
		}
		else
			ircdproto->SendNumeric(Config->ServerName, 401, source, "%s :No such service.", who.c_str());
	}
	return MOD_CONT;
}

/* *INDENT-OFF* */
void moduleAddMsgs()
{
	Anope::AddMessage("STATS", m_stats);
	Anope::AddMessage("TIME", m_time);
	Anope::AddMessage("VERSION", m_version);
}
