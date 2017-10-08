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

class CommandCSSync : public Command
{
 public:
	CommandCSSync(Module *creator) : Command(creator, "chanserv/sync", 1, 1)
	{
		this->SetDesc(_("Sync users channel modes"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		Channel *c = ci->GetChannel();
		if (c == nullptr)
		{
			source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->GetName());
			return;
		}

		if (!source.AccessFor(ci).HasPriv("ACCESS_CHANGE") && !source.HasOverridePriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "ACCESS_CHANGE", ci->GetName());
			return;
		}

		logger.Command(source, ci, _("{source} used {command} on {channel}"));

		for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
			c->SetCorrectModes(it->second->user, true);

		source.Reply(_("All user modes on \002{0}\002 have been synced."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &params) override
	{
		source.Reply(_("Syncs all channel status modes on all users on \037channel\037 with the modes they should have based on the channel access list.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege on \037channel\037."),
		               "ACCESS_CHNAGE");
		return true;
	}
};

class CSSync : public Module
{
	CommandCSSync commandcssync;

 public:
	CSSync(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcssync(this)
	{

	}
};

MODULE_INIT(CSSync)
