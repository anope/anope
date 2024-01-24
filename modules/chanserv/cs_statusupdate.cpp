/*
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

class StatusUpdate final
	: public Module
{
public:
	StatusUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
	{

	}

	void OnAccessAdd(ChannelInfo *ci, CommandSource &, ChanAccess *access) override
	{
		if (ci->c)
		{
			for (const auto &[_, uc] : ci->c->users)
			{
				User *user = uc->user;

				ChannelInfo *next;
				if (user->server != Me && access->Matches(user, user->Account(), next))
				{
					AccessGroup ag = ci->AccessFor(user);

					for (auto *cms : ModeManager::GetStatusChannelModesByRank())
					{
						if (!ag.HasPriv("AUTO" + cms->name))
							ci->c->RemoveMode(NULL, cms, user->GetUID());
					}
					ci->c->SetCorrectModes(user, true);
				}
			}
		}
	}

	void OnAccessDel(ChannelInfo *ci, CommandSource &, ChanAccess *access) override
	{
		if (ci->c)
		{
			for (const auto &[_, uc] : ci->c->users)
			{
				User *user = uc->user;

				ChannelInfo *next;
				if (user->server != Me && access->Matches(user, user->Account(), next))
				{
					AccessGroup ag = ci->AccessFor(user);

					for (auto *cms : ModeManager::GetStatusChannelModesByRank())
					{
							if (!ag.HasPriv("AUTO" + cms->name))
							ci->c->RemoveMode(NULL, cms, user->GetUID());
					}
				}
			}
		}
	}
};

MODULE_INIT(StatusUpdate)
