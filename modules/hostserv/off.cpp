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

class CommandHSOff : public Command
{
 public:
	CommandHSOff(Module *creator) : Command(creator, "hostserv/off", 0, 0)
	{
		this->SetDesc(_("Deactivates your assigned vhost"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		NickServ::Nick *na = NickServ::FindNick(u->nick);

		if (!na || na->GetAccount() != u->Account() || !na->HasVhost())
			na = NickServ::FindNick(u->Account()->GetDisplay());

		if (!na || !na->HasVhost() || na->GetAccount() != source.GetAccount())
		{
			source.Reply(_("There is no vhost assigned to this nickname."));
			return;
		}

		u->vhost.clear();
		IRCD->SendVhostDel(u);
		Log(LOG_COMMAND, source, this) << "to disable their vhost";
		source.Reply(_("Your vhost was removed and the normal cloaking restored."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Deactivates your vhost."));
		return true;
	}
};

class HSOff : public Module
{
	CommandHSOff commandhsoff;

 public:
	HSOff(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhsoff(this)
	{
		if (!IRCD || !IRCD->CanSetVHost)
			throw ModuleException("Your IRCd does not support vhosts");
	}
};

MODULE_INIT(HSOff)
