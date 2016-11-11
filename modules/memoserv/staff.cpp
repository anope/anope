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
#include "modules/memoserv.h"

class CommandMSStaff : public Command
{
 public:
	CommandMSStaff(Module *creator) : Command(creator, "memoserv/staff", 1, 1)
	{
		this->SetDesc(_("Send a memo to all opers/admins"));
		this->SetSyntax(_("\037memo-text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!MemoServ::service)
			return;

		const Anope::string &text = params[0];

		for (NickServ::Account *nc : NickServ::service->GetAccountList())
			if (source.nc != nc && nc->GetOper())
				MemoServ::service->Send(source.GetNick(), nc->GetDisplay(), text, true);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sends all services staff a memo containing \037memo-text\037."));

		return true;
	}
};

class MSStaff : public Module
{
	CommandMSStaff commandmsstaff;

 public:
	MSStaff(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmsstaff(this)
	{
		if (!MemoServ::service)
			throw ModuleException("No MemoServ!");
	}
};

MODULE_INIT(MSStaff)
