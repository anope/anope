/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
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

class StatusUpdate : public Module
	, public EventHook<Event::AccessAdd>
	, public EventHook<Event::AccessDel>
{
	void ApplyModes(ChanServ::Channel *ci, ChanServ::ChanAccess *access, bool set)
	{
		if (ci->c == nullptr)
			return;

		for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
		{
			User *user = it->second->user;

			if (user->server != Me && access->Matches(user, user->Account()))
			{
				ChanServ::AccessGroup ag = ci->AccessFor(user);

				for (unsigned i = 0; i < ModeManager::GetStatusChannelModesByRank().size(); ++i)
				{
					ChannelModeStatus *cms = ModeManager::GetStatusChannelModesByRank()[i];
					if (!ag.HasPriv("AUTO" + cms->name))
						ci->c->RemoveMode(NULL, cms, user->GetUID());
				}

				if (set)
					ci->c->SetCorrectModes(user, true);
			}
		}
	}

 public:
	StatusUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::AccessAdd>(this)
		, EventHook<Event::AccessDel>(this)
	{

	}

	void OnAccessAdd(ChanServ::Channel *ci, CommandSource &, ChanServ::ChanAccess *access) override
	{
		ApplyModes(ci, access, true);
	}

	// XXX this relies on the access entry already being removed from the list?
	void OnAccessDel(ChanServ::Channel *ci, CommandSource &, ChanServ::ChanAccess *access) override
	{
		ApplyModes(ci, access, false);
	}
};

MODULE_INIT(StatusUpdate)
