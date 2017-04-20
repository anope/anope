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
#include "modules/nickserv/update.h"

class CommandNSUpdate : public Command
{
 public:
	CommandNSUpdate(Module *creator) : Command(creator, "nickserv/update", 0, 0)
	{
		this->SetDesc(_("Updates your current status, i.e. it checks for new memos"));
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		NickServ::Nick *na = NickServ::FindNick(u->nick);

		if (na && na->GetAccount() == source.GetAccount())
		{
			na->SetLastRealname(u->realname);
			na->SetLastSeen(Anope::CurTime);
		}

		EventManager::Get()->Dispatch(&Event::NickUpdate::OnNickUpdate, u);

		source.Reply(_("Status updated (memos, vhost, chmodes, flags)."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Updates your current status, i.e. it checks for new memos,\n"
				"sets needed channel modes and updates your vhost and\n"
				"your userflags (lastseentime, etc)."));
		return true;
	}
};

class NSUpdate : public Module
{
	CommandNSUpdate commandnsupdate;

 public:
	NSUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandnsupdate(this)
	{

	}
};

MODULE_INIT(NSUpdate)
