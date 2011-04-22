/* BotServ functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "services.h"
#include "modules.h"

BotInfo *findbot(const Anope::string &nick)
{
	BotInfo *bi = NULL;
	if (isdigit(nick[0]) && ircd->ts6)
	{
		Anope::map<BotInfo *>::iterator it = BotListByUID.find(nick);
		if (it != BotListByUID.end())
			bi = it->second;
	}
	else
	{
		Anope::insensitive_map<BotInfo *>::iterator it = BotListByNick.find(nick);
		if (it != BotListByNick.end())
			bi = it->second;
	}
	
	FOREACH_MOD(I_OnFindBot, OnFindBot(nick));
	
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
		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", GetString(requester->Account(), _(ACCESS_DENIED)).c_str());
		return;
	}

	ChanAccess *u_access = ci->GetAccess(u), *req_access = ci->GetAccess(requester);
	int16 u_level = u_access ? u_access->level : 0, req_level = req_access ? req_access->level : 0;
	if (ci->HasFlag(CI_PEACE) && !requester->nick.equals_ci(u->nick) && u_level >= req_level)
		return;

	if (matches_list(ci->c, u, CMODE_EXCEPT))
	{
		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", GetString(requester->Account(), _("User matches channel except.")).c_str());
		return;
	}

	Anope::string mask;
	get_idealban(ci, u, mask);

	ci->c->SetMode(NULL, CMODE_BAN, mask);

	/* Check if we need to do a signkick or not -GD */
	if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !check_access(requester, ci, CA_SIGNKICK)))
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
		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", GetString(requester->Account(), _(ACCESS_DENIED)).c_str());
		return;
	}

	ChanAccess *u_access = ci->GetAccess(u), *req_access = ci->GetAccess(requester);
	int16 u_level = u_access ? u_access->level : 0, req_level = req_access ? req_access->level : 0;
	if (ci->HasFlag(CI_PEACE) && !requester->nick.equals_ci(u->nick) && u_level >= req_level)
		return;

	if (ci->HasFlag(CI_SIGNKICK) || (ci->HasFlag(CI_SIGNKICK_LEVEL) && !check_access(requester, ci, CA_SIGNKICK)))
		ci->c->Kick(ci->bi, u, "%s (%s)", !reason.empty() ? reason.c_str() : ci->bi->nick.c_str(), requester->nick.c_str());
	else
		ci->c->Kick(ci->bi, u, "%s", !reason.empty() ? reason.c_str() : ci->bi->nick.c_str());
}

