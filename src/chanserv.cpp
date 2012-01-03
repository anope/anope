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
#include "modules.h"

registered_channel_map RegisteredChannelList;

/*************************************************************************/

/* Check the current modes on a channel; if they conflict with a mode lock,
 * fix them.
 */

void check_modes(Channel *c)
{
	if (!c)
	{
		Log() << "check_modes called with NULL values";
		return;
	}

	if (c->bouncy_modes)
		return;

	/* Check for mode bouncing */
	if (c->server_modecount >= 3 && c->chanserv_modecount >= 3)
	{
		Log() << "Warning: unable to set modes on channel " << c->name << ". Are your servers' U:lines configured correctly?";
		c->bouncy_modes = 1;
		return;
	}

	if (c->chanserv_modetime != Anope::CurTime)
	{
		c->chanserv_modecount = 0;
		c->chanserv_modetime = Anope::CurTime;
	}
	c->chanserv_modecount++;

	/* Check if the channel is registered; if not remove mode -r */
	ChannelInfo *ci = c->ci;
	if (!ci)
	{
		if (c->HasMode(CMODE_REGISTERED))
			c->RemoveMode(NULL, CMODE_REGISTERED);
		return;
	}

	for (std::multimap<ChannelModeName, ModeLock>::const_iterator it = ci->GetMLock().begin(), it_end = ci->GetMLock().end(); it != it_end; ++it)
	{
		const ModeLock &ml = it->second;
		ChannelMode *cm = ModeManager::FindChannelModeByName(ml.name);
		if (!cm)
			continue;

		if (cm->Type == MODE_REGULAR)
		{
			if (!c->HasMode(cm->Name) && ml.set)
				c->SetMode(NULL, cm);
			else if (c->HasMode(cm->Name) && !ml.set)
				c->RemoveMode(NULL, cm);
		}
		else if (cm->Type == MODE_PARAM)
		{
			Anope::string param;
			c->GetParam(cm->Name, param);

			/* If the channel doesnt have the mode, or it does and it isn't set correctly */
			if (ml.set)
			{
				if (!c->HasMode(cm->Name) || (!param.empty() && !ml.param.empty() && !param.equals_cs(ml.param)))
					c->SetMode(NULL, cm, ml.param);
			}
			else
			{
				if (c->HasMode(cm->Name))
					c->RemoveMode(NULL, cm);
			}

		}
		else if (cm->Type == MODE_LIST)
		{
			if (ml.set)
				c->SetMode(NULL, cm, ml.param);
			else
				c->RemoveMode(NULL, cm, ml.param);
		}
	}
}

/*************************************************************************/

ChannelInfo *cs_findchan(const Anope::string &chan)
{
	FOREACH_MOD(I_OnFindChan, OnFindChan(chan));

	registered_channel_map::const_iterator it = RegisteredChannelList.find(chan);
	if (it != RegisteredChannelList.end())
		return it->second;

	return NULL;
}

/*************************************************************************/

/** Is the user the real founder?
 * @param user The user
 * @param ci The channel
 * @return true or false
 */
bool IsFounder(User *user, ChannelInfo *ci)
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

int get_idealban(ChannelInfo *ci, User *u, Anope::string &ret)
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

ChanServTimer::ChanServTimer(Channel *chan) : Timer(Config->CSInhabit), c(chan)
{
	BotInfo *bi = findbot(Config->ChanServ);
	if (!bi || !c)
		return;
	c->SetFlag(CH_INHABIT);
	if (!c->ci || !c->ci->bi)
		bi->Join(c);
	else if (!c->FindUser(c->ci->bi))
		c->ci->bi->Join(c);
}

void ChanServTimer::Tick(time_t)
{
	if (!c)
		return;

	c->UnsetFlag(CH_INHABIT);

	if (!c->ci || !c->ci->bi)
	{
		BotInfo *bi = findbot(Config->ChanServ);
		if (bi)
			bi->Part(c);
	}
	else if (c->users.size() == 1 || c->users.size() < Config->BSMinUsers)
		c->ci->bi->Part(c);
}

