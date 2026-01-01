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

class BSAutoAssign final
	: public Module
{
public:
	BSAutoAssign(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
	{
	}

	void OnChanRegistered(ChannelInfo *ci) override
	{
		const Anope::string &bot = Config->GetModule(this).Get<const Anope::string>("bot");
		if (bot.empty())
			return;

		BotInfo *bi = BotInfo::Find(bot, true);
		if (bi == NULL)
		{
			Log(this) << "bs_autoassign is configured to assign bot " << bot << ", but it does not exist?";
			return;
		}

		bi->Assign(NULL, ci);
	}
};

MODULE_INIT(BSAutoAssign)
