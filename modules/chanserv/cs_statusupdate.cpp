// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

class StatusUpdate final
	: public Module
{
private:
	void OnAccessChange(ChannelInfo *ci, ChanAccess *access, bool migrated, bool adding)
	{
		if (!ci->c || migrated)
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

	void OnAccessAdd(ChannelInfo *ci, CommandSource &, ChanAccess *access, bool migrated) override
	{
		OnAccessChange(ci, access, migrated, true);
	}

	void OnAccessDel(ChannelInfo *ci, CommandSource &, ChanAccess *access, bool migrated) override
	{
		OnAccessChange(ci, access, migrated, false);
	}
};

MODULE_INIT(StatusUpdate)
