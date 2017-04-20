/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
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
#include "modules/chanserv/info.h"

class CommandCSInfo : public Command
{
 public:
	CommandCSInfo(Module *creator) : Command(creator, "chanserv/info", 1, 2)
	{
		this->SetDesc(_("Lists information about the specified registered channel"));
		this->SetSyntax(_("\037channel\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		NickServ::Account *nc = source.nc;
		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		bool has_auspex = source.HasPriv("chanserv/auspex");
		bool show_all = false;

		/* Should we show all fields? Only for sadmins and identified users */
		if (source.AccessFor(ci).HasPriv("INFO") || has_auspex)
			show_all = true;

		InfoFormatter info(nc);

		source.Reply(_("Information for channel \002{0}\002:"), ci->GetName());
		if (ci->GetFounder())
			info[_("Founder")] = ci->GetFounder()->GetDisplay();

		if (show_all && ci->GetSuccessor())
			info[_("Successor")] = ci->GetSuccessor()->GetDisplay();

		if (!ci->GetDesc().empty())
			info[_("Description")] = ci->GetDesc();

		info[_("Registered")] = Anope::strftime(ci->GetTimeRegistered(), source.GetAccount());
		info[_("Last used")] = Anope::strftime(ci->GetLastUsed(), source.GetAccount());

		if (show_all)
		{
			switch (ci->GetBanType())
			{
				case 0:
					info[_("Ban type")] = stringify("0: *!user@host");
					break;
				case 1:
					info[_("Ban type")] = stringify("1: *!*user@host");
					break;
				case 2:
					info[_("Ban type")] = stringify("2: *!*@host");
					break;
				case 3:
					info[_("Ban type")] = stringify("3: *!*user@*.domain");
					break;
			}
		}

		EventManager::Get()->Dispatch(&Event::ChanInfo::OnChanInfo, source, ci, info, show_all);

		std::vector<Anope::string> replies;
		info.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Shows information about \037channel\037, including its founder, description, time of registration, and last time used."
		               " If the user issuing the command has \002{0}\002 privilege on \037channel\037, then the successor, last topic set, settings and expiration time will also be displayed when applicable."));
		return true;
	}
};

class CSInfo : public Module
{
	CommandCSInfo commandcsinfo;

 public:
	CSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsinfo(this)
	{

	}
};

MODULE_INIT(CSInfo)
