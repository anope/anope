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

class HelpChannel final
	: public Module
{
public:
	HelpChannel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
	{
	}

	EventReturn OnChannelModeSet(Channel *c, MessageSource &, ChannelMode *mode, const ModeData &data) override
	{
		if (mode->name == "OP" && c && c->ci && c->name.equals_ci(Config->GetModule(this).Get<const Anope::string>("helpchannel")))
		{
			User *u = User::Find(data.value);

			if (u && c->ci->AccessFor(u).HasPriv("OPME"))
				u->SetMode(Config->GetClient("OperServ"), "HELPOP");
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(HelpChannel)
