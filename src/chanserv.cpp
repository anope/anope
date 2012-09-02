/* ChanServ functions.
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "services.h"
#include "anope.h"
#include "regchannel.h"
#include "users.h"
#include "channels.h"
#include "access.h"
#include "account.h"

ChannelInfo* cs_findchan(const Anope::string &chan)
{
	registered_channel_map::const_iterator it = RegisteredChannelList->find(chan);
	if (it != RegisteredChannelList->end())
	{
		it->second->QueueUpdate();
		return it->second;
	}

	return NULL;
}

/*************************************************************************/

/** Is the user the real founder?
 * @param user The user
 * @param ci The channel
 * @return true or false
 */
bool IsFounder(const User *user, const ChannelInfo *ci)
{
	if (!user || !ci)
		return false;

	if (user->SuperAdmin)
		return true;

	if (user->Account() && user->Account() == ci->GetFounder())
		return true;

	return false;
}

/*************************************************************************/

void update_cs_lastseen(User *user, ChannelInfo *ci)
{
	if (!ci || !user)
		return;
	
	AccessGroup u_access = ci->AccessFor(user);
	for (unsigned i = u_access.size(); i > 0; --i)
		u_access[i - 1]->last_seen = Anope::CurTime;
}

/*************************************************************************/

/* Returns the best ban possible for a user depending of the bantype
   value. */

int get_idealban(const ChannelInfo *ci, User *u, Anope::string &ret)
{
	Anope::string mask;

	if (!ci || !u)
		return 0;

	Anope::string vident = u->GetIdent();

	switch (ci->bantype)
	{
		case 0:
			ret = "*!" + vident + "@" + u->GetDisplayedHost();
			return 1;
		case 1:
			if (vident[0] == '~')
				ret = "*!*" + vident + "@" + u->GetDisplayedHost();
			else
				ret = "*!" + vident + "@" + u->GetDisplayedHost();

			return 1;
		case 2:
			ret = "*!*@" + u->GetDisplayedHost();
			return 1;
		case 3:
			mask = create_mask(u);
			ret = "*!" + mask;
			return 1;

		default:
			return 0;
	}
}

