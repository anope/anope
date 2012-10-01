/* BotServ functions
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
#include "protocol.h"
#include "bots.h"
#include "regchannel.h"
#include "language.h"
#include "extern.h"
#include "access.h"
#include "channels.h"
#include "account.h"

BotInfo* findbot(const Anope::string &nick)
{
	BotInfo *bi = NULL;
	if (isdigit(nick[0]) && ircdproto->RequiresID)
	{
		botinfouid_map::iterator it = BotListByUID->find(nick);
		if (it != BotListByUID->end())
			bi = it->second;
	}
	else
	{
		botinfo_map::iterator it = BotListByNick->find(nick);
		if (it != BotListByNick->end())
			bi = it->second;
	}

	if (bi)
		bi->QueueUpdate();
	return bi;
}


/*************************************************************************/

/* Makes a simple ban and kicks the target
 * @param requester The user requesting the kickban
 * @param ci The channel
 * @param u The user being kicked
 * @param reason The reason
 */
void bot_raw_ban(User *requester, ChannelInfo *ci, User *u, const Anope::string &reason)
{
	if (!u || !ci)
		return;

	if (ModeManager::FindUserModeByName(UMODE_PROTECTED) && u->IsProtected() && requester != u)
	{
		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", translate(requester, ACCESS_DENIED));
		return;
	}

	AccessGroup u_access = ci->AccessFor(u), req_access = ci->AccessFor(requester);
	if (ci->HasFlag(CI_PEACE) && u != requester && u_access >= req_access)
		return;

	if (matches_list(ci->c, u, CMODE_EXCEPT))
	{
		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", translate(requester, _("User matches channel except.")));
		return;
	}

	Anope::string mask;
	get_idealban(ci, u, mask);

	ci->c->SetMode(NULL, CMODE_BAN, mask);

	/* Check if we need to do a signkick or not -GD */
	if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !req_access.HasPriv("SIGNKICK")))
		ci->c->Kick(ci->bi, u, "%s (%s)", !reason.empty() ? reason.c_str() : ci->bi->nick.c_str(), requester->nick.c_str());
	else
		ci->c->Kick(ci->bi, u, "%s", !reason.empty() ? reason.c_str() : ci->bi->nick.c_str());
}

/*************************************************************************/

/* Makes a kick with a "dynamic" reason ;)
 * @param requester The user requesting the kick
 * @param ci The channel
 * @param u The user being kicked
 * @param reason The reason for the kick
 */
void bot_raw_kick(User *requester, ChannelInfo *ci, User *u, const Anope::string &reason)
{
	if (!u || !ci || !ci->c || !ci->c->FindUser(u))
		return;

	if (ModeManager::FindUserModeByName(UMODE_PROTECTED) && u->IsProtected() && requester != u)
	{
		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", translate(requester, ACCESS_DENIED));
		return;
	}

	AccessGroup u_access = ci->AccessFor(u), req_access = ci->AccessFor(requester);
	if (ci->HasFlag(CI_PEACE) && requester != u && u_access >= req_access)
		return;

	if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !req_access.HasPriv("SIGNKICK")))
		ci->c->Kick(ci->bi, u, "%s (%s)", !reason.empty() ? reason.c_str() : ci->bi->nick.c_str(), requester->nick.c_str());
	else
		ci->c->Kick(ci->bi, u, "%s", !reason.empty() ? reason.c_str() : ci->bi->nick.c_str());
}

