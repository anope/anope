/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
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

class CommandCSUnban : public Command
{
 public:
	CommandCSUnban(Module *creator) : Command(creator, "chanserv/unban", 0, 2)
	{
		this->SetDesc(_("Remove all bans preventing a user from entering a channel"));
		this->SetSyntax(_("\037channel\037 [\037nick\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName("BAN");
		if (!cm)
			return;

		std::vector<ChannelMode *> modes = cm->listeners;
		modes.push_back(cm);

		if (params.empty())
		{
			if (!source.GetUser())
				return;

			unsigned int count = 0;

			for (ChanServ::Channel *ci : source.GetAccount()->GetRefs<ChanServ::Channel *>())
			{
				if (!ci->c || !source.AccessFor(ci).HasPriv("UNBAN"))
					continue;

				for (unsigned int j = 0; j < modes.size(); ++j)
					if (ci->c->Unban(source.GetUser(), modes[j]->name, true))
						++count;
			}

			logger.Command(LogType::COMMAND, source, _("{source} used {command} on all channels"));
			source.Reply(_("You have been unbanned from \002{0}\002 channels."), count);

			return;
		}

		const Anope::string &chan = params[0];
		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (ci->c == NULL)
		{
			source.Reply(_("Channel \002{0}\002 doesn't exist."), ci->GetName());
			return;
		}

		if (!source.AccessFor(ci).HasPriv("UNBAN") && !source.HasPriv("chanserv/kick"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "UNBAN", ci->GetName());
			return;
		}

		User *u2 = source.GetUser();
		if (params.size() > 1)
			u2 = User::Find(params[1], true);

		if (!u2)
		{
			if (params.size() > 1)
				source.Reply(_("User \002{0}\002 isn't currently online."), params[1]);
			return;
		}

		bool override = !source.AccessFor(ci).HasPriv("UNBAN") && source.HasPriv("chanserv/kick");
		logger.Command(override ? LogType::OVERRIDE : LogType::COMMAND, source, _("{source} used {command} on {channel} to unban {0}"), u2->nick);

		for (unsigned i = 0; i < modes.size(); ++i)
			ci->c->Unban(u2, modes[i]->name, source.GetUser() == u2);

		if (u2 == source.GetUser())
			source.Reply(_("You have been unbanned from \002{0}\002."), ci->c->name);
		else
			source.Reply(_("\002{0}\002 has been unbanned from \002{1}\002."), u2->nick, ci->c->name);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Tells {0} to remove all bans preventing you or the given \037user\037 from entering \037channel\037."
		               " If no channel is given, all bans affecting you in channels you have access in are removed.\n"
				"\n"
				"Use of this command requires the \002{1}\002 privilege on \037channel\037."),
				source.service->nick, "UNBAN");
		return true;
	}
};

class CSUnban : public Module
{
	CommandCSUnban commandcsunban;

 public:
	CSUnban(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsunban(this)
	{

	}
};

MODULE_INIT(CSUnban)
