/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"

class BSAutoAssign : public Module
	, public EventHook<Event::ChanRegistered>
{
 public:
	BSAutoAssign(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ChanRegistered>(this)
	{
	}

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		const Anope::string &bot = Config->GetModule(this)->Get<Anope::string>("bot");
		if (bot.empty())
			return;

		ServiceBot *bi = ServiceBot::Find(bot, true);
		if (bi == NULL)
		{
			logger.Log("bs_autoassign is configured to assign bot {0}, but it does not exist?", bot);
			return;
		}

		bi->Assign(NULL, ci);
	}
};

MODULE_INIT(BSAutoAssign)
