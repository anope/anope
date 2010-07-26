/* Various routines to perform simple actions.
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

/*************************************************************************/

/**
 * Note a bad password attempt for the given user.  If they've used up
 * their limit, toss them off.
 * @param u the User to check
 * @return true if the user was killed, otherwise false
 */
bool bad_password(User *u)
{
	time_t now = time(NULL);

	if (!u || !Config.BadPassLimit)
		return false;

	if (Config.BadPassTimeout > 0 && u->invalid_pw_time > 0 && u->invalid_pw_time < now - Config.BadPassTimeout)
		u->invalid_pw_count = 0;
	++u->invalid_pw_count;
	u->invalid_pw_time = now;
	if (u->invalid_pw_count >= Config.BadPassLimit)
	{
		kill_user("", u->nick, "Too many invalid passwords");
		return true;
	}

	return false;
}

/*************************************************************************/

/**
 * Remove a user from the IRC network.
 * @param source is the nick which should generate the kill, or NULL for a server-generated kill.
 * @param user to remove
 * @param reason for the kill
 * @return void
 */
void kill_user(const Anope::string &source, const Anope::string &user, const Anope::string &reason)
{
	if (user.empty())
		return;

	Anope::string real_source = source.empty() ? Config.ServerName : source;

	Anope::string buf = real_source + " (" + reason + ")";

	ircdproto->SendSVSKill(findbot(source), finduser(user), "%s", buf.c_str());

	if (!ircd->quitonkill && finduser(user))
		do_kill(user, buf);
}

/*************************************************************************/

/**
 * Unban the nick from a channel
 * @param ci channel info for the channel
 * @param nick to remove the ban for
 * @return void
 */
void common_unban(ChannelInfo *ci, const Anope::string &nick)
{
	Anope::string host;
	uint32 ip = 0;
	User *u;
	Entry *ban, *next;

	if (!ci || !ci->c || nick.empty())
		return;

	if (!(u = finduser(nick)))
		return;

	if (!ci->c->bans || !ci->c->bans->count)
		return;

	if (u->hostip.empty())
	{
		host = host_resolve(u->host);
		/* we store the just resolved hostname so we don't
		 * need to do this again */
		if (!host.empty())
			u->hostip = host;
	}
	else
		host = u->hostip;
	/* Convert the host to an IP.. */
	if (!host.empty())
		ip = str_is_ip(host);

	if (ircd->svsmode_unban)
		ircdproto->SendBanDel(ci->c, nick);
	else
		for (ban = ci->c->bans->entries; ban; ban = next)
		{
			next = ban->next;
			if (entry_match(ban, u->nick, u->GetIdent(), u->host, ip) || entry_match(ban, u->nick, u->GetIdent(), u->GetDisplayedHost(), ip))
				ci->c->RemoveMode(NULL, CMODE_BAN, ban->mask);
		}
}

/*************************************************************************/
