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
#include "language.h"

/*************************************************************************/

int m_nickcoll(const char *user)
{
	introduce_user(user);
	return MOD_CONT;
}

/*************************************************************************/

int m_away(const char *source, const char *msg)
{
	User *u;

	u = finduser(source);

	if (u && !msg) /* un-away */
		check_memos(u);
	return MOD_CONT;
}

/*************************************************************************/

int m_kill(const std::string &nick, const char *msg)
{
	BotInfo *bi;

	/* Recover if someone kills us. */
	if (Config.s_BotServ && (bi = findbot(nick)))
	{
		introduce_user(nick);
		bi->RejoinAll();
	}
	else
		do_kill(nick, msg);

	return MOD_CONT;
}

/*************************************************************************/

int m_time(const char *source, int ac, const char **av)
{
	time_t t;
	struct tm *tm;
	char buf[64];

	if (!source)
		return MOD_CONT;

	time(&t);
	tm = localtime(&t);
	strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y %Z", tm);
	ircdproto->SendNumeric(Config.ServerName, 391, source, "%s :%s", Config.ServerName, buf);
	return MOD_CONT;
}

/*************************************************************************/

int m_motd(const char *source)
{
	FILE *f;
	char buf[BUFSIZE];

	if (!source)
		return MOD_CONT;

	f = fopen(Config.MOTDFilename, "r");
	if (f)
	{
		ircdproto->SendNumeric(Config.ServerName, 375, source, ":- %s Message of the Day", Config.ServerName);
		while (fgets(buf, sizeof(buf), f))
		{
			buf[strlen(buf) - 1] = 0;
			ircdproto->SendNumeric(Config.ServerName, 372, source, ":- %s", buf);
		}
		fclose(f);
		ircdproto->SendNumeric(Config.ServerName, 376, source, ":End of /MOTD command.");
	}
	else
		ircdproto->SendNumeric(Config.ServerName, 422, source, ":- MOTD file not found!  Please contact your IRC administrator.");
	return MOD_CONT;
}

/*************************************************************************/

int m_privmsg(const std::string &source, const std::string &receiver, const std::string &message)
{
	char *target;
	time_t starttime, stoptime; /* When processing started and finished */

	if (source.empty() || receiver.empty() || message.empty())
		return MOD_CONT;

	User *u = finduser(source);

	if (!u)
	{
		Alog() << message << ": user record for " << source << " not found";

		BotInfo *bi = findbot(receiver);
		if (bi)
			ircdproto->SendMessage(bi, source.c_str(), "%s", getstring(USER_RECORD_NOT_FOUND));

		return MOD_CONT;
	}

	if (receiver[0] == '#' && Config.s_BotServ)
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
			if (get_ignore(source.c_str()))
			{
				target = myStrGetToken(message.c_str(), ' ', 0);
				Alog() << "Ignored message from " << source << " to " << receiver << " using command " << target;
				delete [] target;
				return MOD_CONT;
			}
		}

		/* If a server is specified (nick@server format), make sure it matches
		 * us, and strip it off. */
		std::string botname = receiver;
		size_t s = receiver.find('@');
		if (s != std::string::npos)
		{
			ci::string servername(receiver.begin() + s + 1, receiver.end());
			botname = botname.erase(s);
			if (servername != Config.ServerName)
				return MOD_CONT;
		}
		else if (Config.UseStrictPrivMsg)
		{
			Alog(LOG_DEBUG) << "Ignored PRIVMSG without @ from " << source;
			notice_lang(receiver, u, INVALID_TARGET, receiver.c_str(), receiver.c_str(), Config.ServerName, receiver.c_str());
			return MOD_CONT;
		}

		if (allow_ignore)
			starttime = time(NULL);

		BotInfo *bi = findbot(botname);

		if (bi)
		{
			ci::string ci_bi_nick(bi->nick.c_str());
			if (ci_bi_nick == Config.s_OperServ)
			{
				if (!is_oper(u) && Config.OSOpersOnly)
				{
					notice_lang(Config.s_OperServ, u, ACCESS_DENIED);
					if (Config.WallBadOS)
						ircdproto->SendGlobops(OperServ, "Denied access to %s from %s!%s@%s (non-oper)", Config.s_OperServ, u->nick.c_str(), u->GetIdent().c_str(), u->host);
				}
				else
					operserv(u, message);
			}
			else if (ci_bi_nick == Config.s_NickServ)
				nickserv(u, message);
			else if (ci_bi_nick == Config.s_ChanServ)
			{
				if (!is_oper(u) && Config.CSOpersOnly)
					notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
				else
					chanserv(u, message);
			}
			else if (ci_bi_nick == Config.s_MemoServ)
				memoserv(u, message);
			else if (Config.s_HostServ && ci_bi_nick == Config.s_HostServ)
				hostserv(u, message);
			else if (Config.s_BotServ)
				botserv(u, bi, message);
		}

		/* Add to ignore list if the command took a significant amount of time. */
		if (allow_ignore)
		{
			stoptime = time(NULL);
			if (stoptime > starttime && source.find('.') == std::string::npos)
				add_ignore(source.c_str(), stoptime - starttime);
		}
	}

	return MOD_CONT;
}

/*************************************************************************/

int m_stats(const char *source, int ac, const char **av)
{
	User *u;

	if (ac < 1)
		return MOD_CONT;

	switch (*av[0])
	{
		case 'l':
			u = finduser(source);

			if (u && is_oper(u))
			{
				ircdproto->SendNumeric(Config.ServerName, 211, source, "Server SendBuf SentBytes SentMsgs RecvBuf RecvBytes RecvMsgs ConnTime");
				ircdproto->SendNumeric(Config.ServerName, 211, source, "%s %d %d %d %d %d %d %ld", uplink_server->host, UplinkSock->WriteBufferLen(), TotalWritten, -1, UplinkSock->ReadBufferLen(), TotalRead, -1, time(NULL) - start_time);
			}

			ircdproto->SendNumeric(Config.ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
			break;
		case 'o':
		case 'O':
			/* Check whether the user is an operator */
			u = finduser(source);
			if (u && !is_oper(u) && Config.HideStatsO)
				ircdproto->SendNumeric(Config.ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
			else
			{
				std::list<std::pair<ci::string, ci::string> >::iterator it, it_end;

				for (it = Config.Opers.begin(), it_end = Config.Opers.end(); it != it_end; ++it)
				{
					ci::string nick = it->first, type = it->second;

					NickCore *nc = findcore(nick);
					if (nc)
						ircdproto->SendNumeric(Config.ServerName, 243, source, "O * * %s %s 0", nick.c_str(), type.c_str());
				}

				ircdproto->SendNumeric(Config.ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
			}

			break;

		case 'u':
		{
			int uptime = time(NULL) - start_time;
			ircdproto->SendNumeric(Config.ServerName, 242, source, ":Services up %d day%s, %02d:%02d:%02d", uptime / 86400, uptime / 86400 == 1 ? "" : "s", (uptime / 3600) % 24, (uptime / 60) % 60, uptime % 60);
			ircdproto->SendNumeric(Config.ServerName, 250, source, ":Current users: %d (%d ops); maximum %d", usercnt, opcnt, maxusercnt);
			ircdproto->SendNumeric(Config.ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
			break;
		} /* case 'u' */

		default:
			ircdproto->SendNumeric(Config.ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
	}
	return MOD_CONT;
}

/*************************************************************************/

int m_version(const char *source, int ac, const char **av)
{
	if (source)
		ircdproto->SendNumeric(Config.ServerName, 351, source, "Anope-%s %s :%s -(%s) -- %s", version_number, Config.ServerName, ircd->name, Config.EncModuleList.begin()->c_str(), version_build);
	return MOD_CONT;
}

/*************************************************************************/

int m_whois(const char *source, const char *who)
{
	if (source && who)
	{
		NickAlias *na;
		BotInfo *bi = findbot(who);
		if (bi)
		{
			ircdproto->SendNumeric(Config.ServerName, 311, source, "%s %s %s * :%s", bi->nick.c_str(), bi->user.c_str(), bi->host.c_str(), bi->real.c_str());
			ircdproto->SendNumeric(Config.ServerName, 307, source, "%s :is a registered nick", bi->nick.c_str());
			ircdproto->SendNumeric(Config.ServerName, 312, source, "%s %s :%s", bi->nick.c_str(), Config.ServerName, Config.ServerDesc);
			ircdproto->SendNumeric(Config.ServerName, 317, source, "%s %ld %ld :seconds idle, signon time", bi->nick.c_str(), time(NULL) - bi->lastmsg, start_time);
			ircdproto->SendNumeric(Config.ServerName, 318, source, "%s :End of /WHOIS list.", who);
		}
		else if (!ircd->svshold && (na = findnick(who)) && na->HasFlag(NS_HELD))
		{
			/* We have a nick enforcer client here that we need to respond to.
			 * We can't just say it doesn't exist here, even tho it does for
			 * other servers :) -GD
			 */
			ircdproto->SendNumeric(Config.ServerName, 311, source, "%s %s %s * :Services Enforcer", na->nick, Config.NSEnforcerUser, Config.NSEnforcerHost);
			ircdproto->SendNumeric(Config.ServerName, 312, source, "%s %s :%s", na->nick, Config.ServerName, Config.ServerDesc);
			ircdproto->SendNumeric(Config.ServerName, 318, source, "%s :End of /WHOIS list.", who);
		}
		else
			ircdproto->SendNumeric(Config.ServerName, 401, source, "%s :No such service.", who);
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
