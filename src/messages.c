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

int m_kill(const char *nick, const char *msg)
{
	BotInfo *bi;

	/* Recover if someone kills us. */
	/* use nickIsServices() to reduce the number of lines of code  - TSL */
	if (nickIsServices(nick, 0)) {
		introduce_user(nick);
	} else if (s_BotServ && (bi = findbot(nick))) {
		introduce_user(nick);
		bi->RejoinAll();
	} else {
		do_kill(nick, msg);
	}
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
	ircdproto->SendNumeric(ServerName, 391, source, "%s :%s", ServerName, buf);
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

	f = fopen(MOTDFilename, "r");
	if (f) {
		ircdproto->SendNumeric(ServerName, 375, source, ":- %s Message of the Day", ServerName);
		while (fgets(buf, sizeof(buf), f)) {
			buf[strlen(buf) - 1] = 0;
			ircdproto->SendNumeric(ServerName, 372, source, ":- %s", buf);
		}
		fclose(f);
		ircdproto->SendNumeric(ServerName, 376, source, ":End of /MOTD command.");
	} else {
		ircdproto->SendNumeric(ServerName, 422, source, ":- MOTD file not found!  Please contact your IRC administrator.");
	}
	return MOD_CONT;
}

/*************************************************************************/

int m_privmsg(const char *source, const char *receiver, const char *msg)
{
	char *s;
	time_t starttime, stoptime; /* When processing started and finished */

	BotInfo *bi;
	ChannelInfo *ci;
	User *u;

	if (!source || !*source || !*receiver || !receiver || !msg) {
		return MOD_CONT;
	}

	u = finduser(source);

	if (!u) {
		alog("%s: user record for %s not found", msg, source);
		/* Two lookups naughty, however, this won't happen often. -- w00t */
		if (findbot(receiver))
		{
			ircdproto->SendMessage(findbot(receiver), source, "%s", getstring(NULL, USER_RECORD_NOT_FOUND));
		}
		return MOD_CONT;
	}

	if (*receiver == '#') {
		if (s_BotServ && (ci = cs_findchan(receiver))) {
			/* Some paranoia checks */
			if (!(ci->flags & CI_VERBOTEN) && ci->c && ci->bi) {
				botchanmsgs(u, ci, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
			}
		}
	} else {
		/* Check if we should ignore.  Operators always get through. */
		if (allow_ignore && !is_oper(u)) {
			IgnoreData *ign = get_ignore(source);
			if (ign) {
				alog("Ignored message from %s: \"%s\"", source, inbuf);
				return MOD_CONT;
			}
		}

		/* If a server is specified (nick@server format), make sure it matches
		 * us, and strip it off. */
		s = const_cast<char *>(strchr(receiver, '@'));
		if (s) {
			*s++ = 0;
			if (stricmp(s, ServerName) != 0)
				return MOD_CONT;
		} else if (UseStrictPrivMsg) {
			if (debug) {
				alog("Ignored PRIVMSG without @ from %s", source);
			}
			notice_lang(receiver, u, INVALID_TARGET, receiver, receiver,
						ServerName, receiver);
			return MOD_CONT;
		}

		starttime = time(NULL);

		if (!stricmp(receiver, s_OperServ)) {
			if (!is_oper(u) && OSOpersOnly) {
				notice_lang(s_OperServ, u, ACCESS_DENIED);
				if (WallBadOS)
					ircdproto->SendGlobops(s_OperServ,
									 "Denied access to %s from %s!%s@%s (non-oper)",
									 s_OperServ, u->nick, u->GetIdent().c_str(),
									 u->host);
			} else {
				operserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
			}
		} else if (!stricmp(receiver, s_NickServ)) {
			nickserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
		} else if (!stricmp(receiver, s_ChanServ)) {
			if (!is_oper(u) && CSOpersOnly)
				notice_lang(s_ChanServ, u, ACCESS_DENIED);
			else
				chanserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
		} else if (!stricmp(receiver, s_MemoServ)) {
			memoserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
		} else if (s_HostServ && !stricmp(receiver, s_HostServ)) {
			hostserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
		} else if (s_HelpServ && !stricmp(receiver, s_HelpServ)) {
			helpserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
		} else if (s_BotServ && !stricmp(receiver, s_BotServ)) {
			botserv(u, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
		} else if (s_BotServ && (bi = findbot(receiver))) {
			botmsgs(u, bi, const_cast<char *>(msg)); // XXX Unsafe cast, this needs reviewing -- CyberBotX
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
	int i;
	User *u;
	NickCore *nc;

	if (ac < 1)
		return MOD_CONT;

	switch (*av[0]) {
	case 'l':
		u = finduser(source);

		if (u && is_oper(u)) {

			ircdproto->SendNumeric(ServerName, 211, source, "Server SendBuf SentBytes SentMsgs RecvBuf RecvBytes RecvMsgs ConnTime");
			ircdproto->SendNumeric(ServerName, 211, source, "%s %d %d %d %d %d %d %ld", uplink_server->host, write_buffer_len(), total_written, -1, read_buffer_len(),
				total_read, -1, time(NULL) - start_time);
		}

		ircdproto->SendNumeric(ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
		break;
	case 'o':
	case 'O':
/* Check whether the user is an operator */
		u = finduser(source);
		if (u && !is_oper(u) && HideStatsO) {
			ircdproto->SendNumeric(ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
		} else {
			for (i = 0; i < RootNumber; i++)
				ircdproto->SendNumeric(ServerName, 243, source, "O * * %s Root 0", ServicesRoots[i]);
			for (i = 0; i < servadmins.count && (nc = static_cast<NickCore *>(servadmins.list[i]));
				 i++)
				ircdproto->SendNumeric(ServerName, 243, source, "O * * %s Admin 0", nc->display);
			for (i = 0; i < servopers.count && (nc = static_cast<NickCore *>(servopers.list[i]));
				 i++)
				ircdproto->SendNumeric(ServerName, 243, source, "O * * %s Oper 0", nc->display);

			ircdproto->SendNumeric(ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
		}

		break;

	case 'u':{
			int uptime = time(NULL) - start_time;
			ircdproto->SendNumeric(ServerName, 242, source, ":Services up %d day%s, %02d:%02d:%02d", uptime / 86400, uptime / 86400 == 1 ? "" : "s",
				(uptime / 3600) % 24, (uptime / 60) % 60, uptime % 60);
			ircdproto->SendNumeric(ServerName, 250, source, ":Current users: %d (%d ops); maximum %d", usercnt, opcnt, maxusercnt);
			ircdproto->SendNumeric(ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
			break;
		}					   /* case 'u' */

	default:
		ircdproto->SendNumeric(ServerName, 219, source, "%c :End of /STATS report.", *av[0] ? *av[0] : '*');
		break;
	}
	return MOD_CONT;
}

/*************************************************************************/

int m_version(const char *source, int ac, const char **av)
{
	if (source) ircdproto->SendNumeric(ServerName, 351, source, "Anope-%s %s :%s - %s (%s) -- %s", version_number, ServerName, ircd->name, version_flags,
		EncModule, version_build);
	return MOD_CONT;
}


/*************************************************************************/

int m_whois(const char *source, const char *who)
{
	BotInfo *bi;
	NickAlias *na;
	const char *clientdesc;

	if (source && who) {
		if (stricmp(who, s_NickServ) == 0)
			clientdesc = desc_NickServ;
		else if (stricmp(who, s_ChanServ) == 0)
			clientdesc = desc_ChanServ;
		else if (stricmp(who, s_MemoServ) == 0)
			clientdesc = desc_MemoServ;
		else if (s_BotServ && stricmp(who, s_BotServ) == 0)
			clientdesc = desc_BotServ;
		else if (s_HostServ && stricmp(who, s_HostServ) == 0)
			clientdesc = desc_HostServ;
		else if (stricmp(who, s_HelpServ) == 0)
			clientdesc = desc_HelpServ;
		else if (stricmp(who, s_OperServ) == 0)
			clientdesc = desc_OperServ;
		else if (stricmp(who, s_GlobalNoticer) == 0)
			clientdesc = desc_GlobalNoticer;
		else if (s_BotServ && (bi = findbot(who))) {
			/* Bots are handled separately */
			ircdproto->SendNumeric(ServerName, 311, source, "%s %s %s * :%s", bi->nick, bi->user, bi->host, bi->real);
			ircdproto->SendNumeric(ServerName, 307, source, "%s :is a registered nick", bi->nick);
			ircdproto->SendNumeric(ServerName, 312, source, "%s %s :%s", bi->nick, ServerName, ServerDesc);
			ircdproto->SendNumeric(ServerName, 317, source, "%s %ld %ld :seconds idle, signon time", bi->nick, time(NULL) - bi->lastmsg, start_time);
			ircdproto->SendNumeric(ServerName, 318, source, "%s :End of /WHOIS list.", who);
			return MOD_CONT;
		} else if (!ircd->svshold && (na = findnick(who))
				   && (na->status & NS_KILL_HELD)) {
			/* We have a nick enforcer client here that we need to respond to.
			 * We can't just say it doesn't exist here, even tho it does for
			 * other servers :) -GD
			 */
			ircdproto->SendNumeric(ServerName, 311, source, "%s %s %s * :Services Enforcer", na->nick, NSEnforcerUser, NSEnforcerHost);
			ircdproto->SendNumeric(ServerName, 312, source, "%s %s :%s", na->nick, ServerName, ServerDesc);
			ircdproto->SendNumeric(ServerName, 318, source, "%s :End of /WHOIS list.", who);
			return MOD_CONT;
		} else {
			ircdproto->SendNumeric(ServerName, 401, source, "%s :No such service.", who);
			return MOD_CONT;
		}
		ircdproto->SendNumeric(ServerName, 311, source, "%s %s %s * :%s", who, ServiceUser, ServiceHost, clientdesc);
		ircdproto->SendNumeric(ServerName, 312, source, "%s %s :%s", who, ServerName, ServerDesc);
		ircdproto->SendNumeric(ServerName, 317, source, "%s %ld %ld :seconds idle, signon time", who, time(NULL) - start_time, start_time);
		ircdproto->SendNumeric(ServerName, 318, source, "%s :End of /WHOIS list.", who);
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
