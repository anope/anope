/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

class StatusUpdate final
	: public Module
{
private:
	void OnAccessChange(ChannelInfo *ci, ChanAccess *access, bool adding)
	{
		if (!ci->c)
			return;

		for (const auto &[_, uc] : ci->c->users)
		{
			auto *user = uc->user;

			ChannelInfo *next;
			if (user->server != Me && access->Matches(user, user->Account(), next))
			{
				auto ag = ci->AccessFor(user);

				for (auto *cms : ModeManager::GetStatusChannelModesByRank())
				{
					if (!ag.HasPriv("AUTO" + cms->name))
						ci->c->RemoveMode(NULL, cms, user->GetUID());
				}

				if (adding)
					ci->c->SetCorrectModes(user, true);
			}
		}
	}

public:
	StatusUpdate(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
	{
	}

	void OnAccessAdd(ChannelInfo *ci, CommandSource &, ChanAccess *access) override
	{
		OnAccessChange(ci, access, true);
	}

	void OnAccessDel(ChannelInfo *ci, CommandSource &, ChanAccess *access) override
	{
		OnAccessChange(ci, access, false);
	}
};

MODULE_INIT(StatusUpdate)
