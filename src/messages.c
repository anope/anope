/* Definitions of IRC message functions and list of messages.
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */

#include "services.h"
#include "messages.h"
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

	if (u && msg == 0)		  /* un-away */
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

	if (!source) {
		return MOD_CONT;
	}

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

	if (!source) {
		return MOD_CONT;
	}

	f = fopen(Config.MOTDFilename, "r");
	if (f) {
		ircdproto->SendNumeric(Config.ServerName, 375, source, ":- %s Message of the Day", Config.ServerName);
		while (fgets(buf, sizeof(buf), f)) {
			buf[strlen(buf) - 1] = 0;
			ircdproto->SendNumeric(Config.ServerName, 372, source, ":- %s", buf);
		}
		fclose(f);
		ircdproto->SendNumeric(Config.ServerName, 376, source, ":End of /MOTD command.");
	} else {
		ircdproto->SendNumeric(Config.ServerName, 422, source, ":- MOTD file not found!  Please contact your IRC administrator.");
	}
	return MOD_CONT;
}

/*************************************************************************/

int m_privmsg(const char *source, const std::string &receiver, const char *msg)
{
	char *target;
	time_t starttime, stoptime; /* When processing started and finished */

	BotInfo *bi;
	ChannelInfo *ci;
	User *u;

	if (!source || !*source || receiver.empty() || !msg) {
		return MOD_CONT;
	}

	u = finduser(source);

	if (!u) {
		alog("%s: user record for %s not found", msg, source);
		/* Two lookups naughty, however, this won't happen often. -- w00t */
		if (findbot(receiver))
		{
			ircdproto->SendMessage(findbot(receiver), source, "%s", getstring(USER_RECORD_NOT_FOUND));
		}
		return MOD_CONT;
	}

	if (receiver[0] == '#') {
		if (Config.s_BotServ && (ci = cs_findchan(receiver))) {
			/* Some paranoia checks */
			if (!ci->HasFlag(CI_FORBIDDEN) && ci->c && ci->bi) {
				botchanmsgs(u, ci, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
			}
		}
	} else {
		/* Check if we should ignore.  Operators always get through. */
		if (allow_ignore && !is_oper(u)) {
			IgnoreData *ign = get_ignore(source);
			if (ign) {
				target = myStrGetToken(msg, ' ', 0);
				alog("Ignored message from %s to %s using command %s", source, receiver.c_str(), target);
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
		else if (Config.UseStrictPrivMsg) {
			if (debug) {
				alog("Ignored PRIVMSG without @ from %s", source);
			}
			notice_lang(receiver, u, INVALID_TARGET, receiver.c_str(), receiver.c_str(),
						Config.ServerName, receiver.c_str());
			return MOD_CONT;
		}

		starttime = time(NULL);

		bi = findbot(botname);

		if (bi)
		{
			ci::string ci_bi_nick(bi->nick.c_str());
			if (ci_bi_nick == Config.s_OperServ)
			{
				if (!is_oper(u) && Config.OSOpersOnly)
				{
					notice_lang(Config.s_OperServ, u, ACCESS_DENIED);
					if (Config.WallBadOS)
						ircdproto->SendGlobops(findbot(Config.s_OperServ), "Denied access to %s from %s!%s@%s (non-oper)", Config.s_OperServ, u->nick.c_str(), u->GetIdent().c_str(), u->host);
				}
				else
					operserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
			}
			else if (ci_bi_nick == Config.s_NickServ)
				nickserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
			else if (ci_bi_nick== Config.s_ChanServ)
			{
				if (!is_oper(u) && Config.CSOpersOnly)
					notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
				else
					chanserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
			}
			else if (ci_bi_nick == Config.s_MemoServ)
				memoserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
			else if (Config.s_HostServ && ci_bi_nick == Config.s_HostServ)
				hostserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
			else if (Config.s_BotServ)
			{
				if (ci_bi_nick == Config.s_BotServ)
					botserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
				else
					botmsgs(u, bi, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
			}
		}

		/* Add to ignore list if the command took a significant amount of time. */
		if (allow_ignore) {
			stoptime = time(NULL);
			if (stoptime > starttime && *source && !strchr(source, '.'))
				add_ignore(source, stoptime - starttime);
		}
	}
	return MOD_CONT;
}

/*************************************************************************/

int m_stats(const char *source, int ac, const char **av)
{
	User *u;
	NickCore *nc;

	if (ac < 1)
		return MOD_CONT;

	switch (*av[0]) {
	case 'l':
		u = finduser(source);

		if (u && is_oper(u)) {

			ircdproto->SendNumeric(Config.ServerName, 211, source, "Server SendBuf SentBytes SentMsgs RecvBuf RecvBytes RecvMsgs ConnTime");
			ircdproto->SendNumeric(Config.ServerName, 211, source, "%s %d %d %d %d %d %d %ld", uplink_server->host, write_buffer_len(), total_written, -1, read_buffer_len(),
				total_read, -1, time(NULL) - start_time);
		}

		ircdproto->SendNumeric(Config.ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
		break;
	case 'o':
	case 'O':
/* Check whether the user is an operator */
		u = finduser(source);
		if (u && !is_oper(u) && Config.HideStatsO) {
			ircdproto->SendNumeric(Config.ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
		} else {
			std::list<std::pair<std::string, std::string> >::iterator it;

			for (it = Config.Opers.begin(); it != Config.Opers.end(); ++it)
			{
				std::string nick = it->first, type = it->second;

				if ((nc = findcore(nick.c_str())))
					ircdproto->SendNumeric(Config.ServerName, 243, source, "O * * %s %s 0", nick.c_str(), type.c_str());
			}

			ircdproto->SendNumeric(Config.ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
		}

		break;

	case 'u':{
			int uptime = time(NULL) - start_time;
			ircdproto->SendNumeric(Config.ServerName, 242, source, ":Services up %d day%s, %02d:%02d:%02d", uptime / 86400, uptime / 86400 == 1 ? "" : "s",
				(uptime / 3600) % 24, (uptime / 60) % 60, uptime % 60);
			ircdproto->SendNumeric(Config.ServerName, 250, source, ":Current users: %d (%d ops); maximum %d", usercnt, opcnt, maxusercnt);
			ircdproto->SendNumeric(Config.ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
			break;
		}					   /* case 'u' */

	default:
		ircdproto->SendNumeric(Config.ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
		break;
	}
	return MOD_CONT;
}

/*************************************************************************/

int m_version(const char *source, int ac, const char **av)
{
	if (source) ircdproto->SendNumeric(Config.ServerName, 351, source, "Anope-%s %s :%s - %s (%s) -- %s", version_number, Config.ServerName, ircd->name, version_flags,
		Config.EncModuleList.begin()->c_str(), version_build);
	return MOD_CONT;
}


/*************************************************************************/

int m_whois(const char *source, const char *who)
{
	BotInfo *bi;
	NickAlias *na;
	const char *clientdesc;

	if (source && who) {
		if (stricmp(who, Config.s_NickServ) == 0)
			clientdesc = Config.desc_NickServ;
		else if (stricmp(who, Config.s_ChanServ) == 0)
			clientdesc = Config.desc_ChanServ;
		else if (stricmp(who, Config.s_MemoServ) == 0)
			clientdesc = Config.desc_MemoServ;
		else if (Config.s_BotServ && stricmp(who, Config.s_BotServ) == 0)
			clientdesc = Config.desc_BotServ;
		else if (Config.s_HostServ && stricmp(who, Config.s_HostServ) == 0)
			clientdesc = Config.desc_HostServ;
		else if (stricmp(who, Config.s_OperServ) == 0)
			clientdesc = Config.desc_OperServ;
		else if (stricmp(who, Config.s_GlobalNoticer) == 0)
			clientdesc = Config.desc_GlobalNoticer;
		else if (Config.s_BotServ && (bi = findbot(who))) {
			/* Bots are handled separately */
			ircdproto->SendNumeric(Config.ServerName, 311, source, "%s %s %s * :%s", bi->nick.c_str(), bi->user.c_str(), bi->host.c_str(), bi->real.c_str());
			ircdproto->SendNumeric(Config.ServerName, 307, source, "%s :is a registered nick", bi->nick.c_str());
			ircdproto->SendNumeric(Config.ServerName, 312, source, "%s %s :%s", bi->nick.c_str(), Config.ServerName, Config.ServerDesc);
			ircdproto->SendNumeric(Config.ServerName, 317, source, "%s %ld %ld :seconds idle, signon time", bi->nick.c_str(), time(NULL) - bi->lastmsg, start_time);
			ircdproto->SendNumeric(Config.ServerName, 318, source, "%s :End of /WHOIS list.", who);
			return MOD_CONT;
		} else if (!ircd->svshold && (na = findnick(who)) && na->HasFlag(NS_KILL_HELD)) {
			/* We have a nick enforcer client here that we need to respond to.
			 * We can't just say it doesn't exist here, even tho it does for
			 * other servers :) -GD
			 */
			ircdproto->SendNumeric(Config.ServerName, 311, source, "%s %s %s * :Services Enforcer", na->nick, Config.NSEnforcerUser, Config.NSEnforcerHost);
			ircdproto->SendNumeric(Config.ServerName, 312, source, "%s %s :%s", na->nick, Config.ServerName, Config.ServerDesc);
			ircdproto->SendNumeric(Config.ServerName, 318, source, "%s :End of /WHOIS list.", who);
			return MOD_CONT;
		} else {
			ircdproto->SendNumeric(Config.ServerName, 401, source, "%s :No such service.", who);
			return MOD_CONT;
		}
		ircdproto->SendNumeric(Config.ServerName, 311, source, "%s %s %s * :%s", who, Config.ServiceUser, Config.ServiceHost, clientdesc);
		ircdproto->SendNumeric(Config.ServerName, 312, source, "%s %s :%s", who, Config.ServerName, Config.ServerDesc);
		ircdproto->SendNumeric(Config.ServerName, 317, source, "%s %ld %ld :seconds idle, signon time", who, time(NULL) - start_time, start_time);
		ircdproto->SendNumeric(Config.ServerName, 318, source, "%s :End of /WHOIS list.", who);
	}
	return MOD_CONT;
}

/* *INDENT-OFF* */
void moduleAddMsgs() {
	Message *m;
	m = createMessage("STATS",	 m_stats); addCoreMessage(IRCD,m);
	m = createMessage("TIME",	  m_time); addCoreMessage(IRCD,m);
	m = createMessage("VERSION",   m_version); addCoreMessage(IRCD,m);
}

/*************************************************************************/

Message *find_message(const char *name)
{
	Message *m;
	m = findMessage(IRCD, name);
	return m;
}

/*************************************************************************/
