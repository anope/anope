/* Various routines to perform simple actions.
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
#include "users.h"
#include "config.h"
#include "regchannel.h"
#include "channels.h"

/*************************************************************************/

/**
 * Note a bad password attempt for the given user.  If they've used up
 * their limit, toss them off.
 * @param u the User to check
 * @return true if the user was killed, otherwise false
 */
bool bad_password(User *u)
{
	if (!u || !Config->BadPassLimit)
		return false;

	if (Config->BadPassTimeout > 0 && u->invalid_pw_time > 0 && u->invalid_pw_time < Anope::CurTime - Config->BadPassTimeout)
		u->invalid_pw_count = 0;
	++u->invalid_pw_count;
	u->invalid_pw_time = Anope::CurTime;
	if (u->invalid_pw_count >= Config->BadPassLimit)
	{
		u->Kill(Config->ServerName, "Too many invalid passwords");
		return true;
	}

	return false;
}

/*************************************************************************/


/**
 * Unban the user from a channel
 * @param ci channel info for the channel
 * @param u The user to unban
 * @param full True to match against the users real host and IP
 * @return void
 */
void common_unban(ChannelInfo *ci, User *u, bool full)
{
	if (!u || !ci || !ci->c || !ci->c->HasMode(CMODE_BAN))
		return;

	std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> bans = ci->c->GetModeList(CMODE_BAN);
	for (; bans.first != bans.second;)
	{
		Entry ban(CMODE_BAN, bans.first->second);
		++bans.first;
		if (ban.Matches(u, full))
			ci->c->RemoveMode(NULL, CMODE_BAN, ban.GetMask());
	}
}

/*************************************************************************/
