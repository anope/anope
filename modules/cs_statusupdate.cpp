/*
 *
 * (C) 2003-2022 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

class StatusUpdate : public Module
{
 public:
	StatusUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
	{

	}

	void OnAccessAdd(ChannelInfo *ci, CommandSource &, ChanAccess *access) override
	{
		if (ci->c)
			for (const auto& [key, value] : ci->c->users)
			{
				User *user = value->user;

				ChannelInfo *next;
				if (user->server != Me && access->Matches(user, user->Account(), next))
				{
					AccessGroup ag = ci->AccessFor(user);

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

	void OnAccessDel(ChannelInfo *ci, CommandSource &, ChanAccess *access) override
	{
		if (ci->c)
			for (const auto& [key, value] : ci->c->users)
			{
				User *user = value->user;

				ChannelInfo *next;
				if (user->server != Me && access->Matches(user, user->Account(), next))
				{
					AccessGroup ag = ci->AccessFor(user);

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
