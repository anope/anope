/* Various routines to perform simple actions.
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

/*************************************************************************/

/**
 * Note a bad password attempt for the given user.  If they've used up
 * their limit, toss them off.
 * @param u the User to check
 * @return void
 */
void bad_password(User *u)
{
	time_t now = time(NULL);

	if (!u || !Config.BadPassLimit)
		return;

	if (Config.BadPassTimeout > 0 && u->invalid_pw_time > 0 && u->invalid_pw_time < now - Config.BadPassTimeout)
		u->invalid_pw_count = 0;
	++u->invalid_pw_count;
	u->invalid_pw_time = now;
	if (u->invalid_pw_count >= Config.BadPassLimit)
		kill_user("", u->nick, "Too many invalid passwords");
}

/*************************************************************************/

/**
 * Remove a user from the IRC network.
 * @param source is the nick which should generate the kill, or NULL for a server-generated kill.
 * @param user to remove
 * @param reason for the kill
 * @return void
 */
void kill_user(const std::string &source, const std::string &user, const std::string &reason)
{
	char buf[BUFSIZE];

	if (user.empty())
		return;

	std::string real_source = source.empty() ? Config.ServerName : source;

	snprintf(buf, sizeof(buf), "%s (%s)", source.c_str(), reason.c_str());

	ircdproto->SendSVSKill(findbot(source), finduser(user), buf);

	if (!ircd->quitonkill && finduser(user))
		do_kill(user, buf);
}

/*************************************************************************/

/**
 * Check and enforce SQlines
 * @param mask of the sqline
 * @param reason for the sqline
 * @return void
 */
void sqline(const std::string &mask, const std::string &reason)
{
	int i;
	Channel *c, *next;
	const char *av[3];
	struct c_userlist *cu, *cunext;

	if (ircd->chansqline)
	{
		if (mask[0] == '#')
		{
			ircdproto->SendSQLine(mask, reason);

			for (i = 0; i < 1024; ++i)
			{
				for (c = chanlist[i]; c; c = next)
				{
					next = c->next;

					if (!Anope::Match(c->name, mask, false))
						continue;
					for (cu = c->users; cu; cu = cunext)
					{
						cunext = cu->next;
						if (is_oper(cu->user))
							continue;
						av[0] = c->name;
						av[1] = cu->user->nick;
						av[2] = reason.c_str();
						ircdproto->SendKick(findbot(Config.s_OperServ), c, cu->user, "Q-Lined: %s", av[2]);
						do_kick(Config.s_ChanServ, 3, av);
					}
				}
			}
		}
		else
			ircdproto->SendSQLine(mask, reason);
	}
	else
		ircdproto->SendSQLine(mask, reason);
}

/*************************************************************************/

/**
 * Unban the nick from a channel
 * @param ci channel info for the channel
 * @param nick to remove the ban for
 * @return void
 */
void common_unban(ChannelInfo *ci, const std::string &nick)
{
	char *host = NULL;
	uint32 ip = 0;
	User *u;
	Entry *ban, *next;

	if (!ci || !ci->c || nick.empty())
		return;

	if (!(u = finduser(nick)))
		return;

	if (!ci->c->bans || !ci->c->bans->count)
		return;

	if (u->hostip == NULL)
	{
		host = host_resolve(u->host);
		/* we store the just resolved hostname so we don't
		 * need to do this again */
		if (host)
			u->hostip = sstrdup(host);
	}
	else
		host = sstrdup(u->hostip);
	/* Convert the host to an IP.. */
	if (host)
		ip = str_is_ip(host);

	if (ircd->svsmode_unban)
		ircdproto->SendBanDel(ci->c, nick);
	else
	{
		for (ban = ci->c->bans->entries; ban; ban = next)
		{
			next = ban->next;
			if (entry_match(ban, u->nick, u->GetIdent().c_str(), u->host, ip) || entry_match(ban, u->nick, u->GetIdent().c_str(), u->GetDisplayedHost().c_str(), ip))
				ci->c->RemoveMode(NULL, CMODE_BAN, ban->mask);
		}
	}
	/* host_resolve() sstrdup us this info so we gotta free it */
	if (host)
		delete [] host;
}

/*************************************************************************/
