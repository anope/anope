/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2017 Anope Team <team@anope.org>
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

class HelpChannel : public Module
	, public EventHook<Event::ChannelModeSet>
{
 public:
	HelpChannel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ChannelModeSet>(this)
	{
	}

	EventReturn OnChannelModeSet(Channel *c, const MessageSource &, ChannelMode *mode, const Anope::string &param) override
	{
		if (mode->name == "OP" && c && c->GetChannel() && c->name.equals_ci(Config->GetModule(this)->Get<Anope::string>("helpchannel")))
		{
			User *u = User::Find(param);

			if (u && c->GetChannel()->AccessFor(u).HasPriv("OPME"))
				u->SetMode(Config->GetClient("OperServ"), "HELPOP");
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(HelpChannel)
