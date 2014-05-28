/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

class StatusUpdate : public Module
	, public EventHook<Event::AccessAdd>
	, public EventHook<Event::AccessDel>
{
 public:
	StatusUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::AccessAdd>("OnAccessAdd")
		, EventHook<Event::AccessDel>("OnAccessDel")
	{

	}

	void OnAccessAdd(ChanServ::Channel *ci, CommandSource &, ChanServ::ChanAccess *access) override
	{
		if (ci->c)
			for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
			{
				User *user = it->second->user;

				ChanServ::ChanAccess::Path p;
				if (user->server != Me && access->Matches(user, user->Account(), p))
				{
					ChanServ::AccessGroup ag = ci->AccessFor(user);

					for (unsigned i = 0; i < ModeManager::GetStatusChannelModesByRank().size(); ++i)
					{
						ChannelModeStatus *cms = ModeManager::GetStatusChannelModesByRank()[i];
						if (!ag.HasPriv("AUTO" + cms->name))
							ci->c->RemoveMode(NULL, cms, user->GetUID());
					}
					ci->c->SetCorrectModes(user, true);
				}
			}
	}

	void OnAccessDel(ChanServ::Channel *ci, CommandSource &, ChanServ::ChanAccess *access) override
	{
		if (ci->c)
			for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
			{
				User *user = it->second->user;

				ChanServ::ChanAccess::Path p;
				if (user->server != Me && access->Matches(user, user->Account(), p))
				{
					ChanServ::AccessGroup ag = ci->AccessFor(user);

					for (unsigned i = 0; i < ModeManager::GetStatusChannelModesByRank().size(); ++i)
					{
						ChannelModeStatus *cms = ModeManager::GetStatusChannelModesByRank()[i];
						if (!ag.HasPriv("AUTO" + cms->name))
							ci->c->RemoveMode(NULL, cms, user->GetUID());
					}
				}
			}
	}
};

MODULE_INIT(StatusUpdate)
