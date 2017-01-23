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
#include "modules/nickserv.h"

class CommandNSLogout : public Command
{
 public:
	CommandNSLogout(Module *creator) : Command(creator, "nickserv/logout", 0, 2)
	{
		this->SetDesc(_("Reverses the effect of the IDENTIFY command"));
		this->SetSyntax(_("[\037nickname\037 [REVALIDATE]]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{

		const Anope::string &nick = !params.empty() ? params[0] : "";
		const Anope::string &param = params.size() > 1 ? params[1] : "";

		if (!source.IsServicesOper() && !nick.empty())
		{
			this->OnSyntaxError(source, "");
			return;
		}

		User *u2 = !nick.empty() ? User::Find(nick, true) : source.GetUser();
		if (!u2)
		{
			source.Reply(_("\002{0}\002 isn't currently online."), !nick.empty() ? nick : source.GetNick());
			return;
		}

		if (!nick.empty() && u2->IsServicesOper())
		{
			source.Reply(_("You can't logout \002{0}\002, they are a Services Operator."), nick);
			return;
		}

#warning "revalidate"
#if 0
		if (!nick.empty() && !param.empty() && param.equals_ci("REVALIDATE") && NickServ::service)
			NickServ::service->Validate(u2);
#endif

		u2->super_admin = false; /* Dont let people logout and remain a SuperAdmin */

		// XXX show account name here?
		logger.Command(LogType::COMMAND, source, _("{source} used {command} to logout {0}"), u2->nick);

		if (!nick.empty())
			source.Reply(_("\002{0}\002 has been logged out."), nick);
		else
			source.Reply(_("You have been logged out."));

		IRCD->Send<messages::Logout>(u2);
		u2->RemoveMode(source.service, "REGISTERED");
		u2->Logout();

		/* Send out an event */
		EventManager::Get()->Dispatch(&Event::NickLogout::OnNickLogout, u2);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Logs you out of your account"));
		#if 0
		source.Reply(_("Without a parameter, reverses the effect of the \002IDENTIFY\002\n"
				"command, i.e. make you not recognized as the real owner of the nick\n"
				"anymore. Note, however, that you won't be asked to reidentify\n"
				"yourself.\n"
				" \n"
				"With a parameter, does the same for the given nick. If you\n"
				"specify \002REVALIDATE\002 as well, Services will ask the given nick\n"
				"to re-identify. This is limited to \002Services Operators\002."));
		#endif

		return true;
	}
};

class NSLogout : public Module
{
	CommandNSLogout commandnslogout;

 public:
	NSLogout(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandnslogout(this)
	{

	}
};

MODULE_INIT(NSLogout)
