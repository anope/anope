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
#include "modules/nickserv/drop.h"

class CommandNSDrop : public Command
{
 public:
	CommandNSDrop(Module *creator) : Command(creator, "nickserv/drop", 1, 1)
	{
		this->SetSyntax(_("\037nickname\037"));
		this->SetDesc(_("Cancel the registration of a nickname"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[0];

		if (Anope::ReadOnly && !source.HasPriv("nickserv/drop"))
		{
			source.Reply(_("Sorry, nickname de-registration is temporarily disabled."));
			return;
		}

		NickServ::Nick *na = NickServ::FindNick(nick);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		bool is_mine = source.GetAccount() == na->GetAccount();

		if (!is_mine && !source.HasPriv("nickserv/drop"))
		{
			source.Reply(_("Access denied. You do not have the correct operator privileges to drop other user's nicknames."));
			return;
		}

		if (Config->GetModule("nickserv/main")->Get<bool>("secureadmins", "yes") && !is_mine && na->GetAccount()->GetOper())
		{
			source.Reply(_("You may not drop other Services Operators' nicknames."));
			return;
		}

		EventManager::Get()->Dispatch(&Event::NickDrop::OnNickDrop, source, na);

		logger.Command(!is_mine ? LogType::ADMIN : LogType::COMMAND, source, _("{source} used {command} to drop nickname {0} (account: {1}) (e-mail: {2})"),
				na->GetNick(), na->GetAccount()->GetDisplay(),
				!na->GetAccount()->GetEmail().empty() ? na->GetAccount()->GetEmail() : "none");

		na->Delete();

		source.Reply(_("\002{0}\002 has been dropped."), nick);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Unregisters \037nickname\037. Once your nickname is dropped you may lose all of your access and channels that you may own. Any other user will be free to register \037nickname\037."));
		if (!source.HasPriv("nickserv/drop"))
			source.Reply(_("You may drop any nickname within your group."));
		else
			 source.Reply(_("As a Services Operator, you may drop any nick."));

		return true;
	}
};

class NSDrop : public Module
{
	CommandNSDrop commandnsdrop;

 public:
	NSDrop(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandnsdrop(this)
	{

	}
};

MODULE_INIT(NSDrop)
