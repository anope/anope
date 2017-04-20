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

class CommandNSGetEMail : public Command
{
 public:
	CommandNSGetEMail(Module *creator) : Command(creator, "nickserv/getemail", 1, 1)
	{
		this->SetDesc(_("Matches and returns all users that registered using given email"));
		this->SetSyntax(_("\037email\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &email = params[0];
		int j = 0;

		logger.Admin(source, _("{source} used {command} on {0}"), email);

		for (NickServ::Account *nc : NickServ::service->GetAccountList())
			if (!nc->GetEmail().empty() && Anope::Match(nc->GetEmail(), email))
			{
				++j;
				source.Reply(_("Email matched: \002{0}\002 (\002{1}\002) to \002{2}\002."), nc->GetDisplay(), nc->GetEmail(), email);
			}

		if (j <= 0)
		{
			source.Reply(_("There are no accounts with an email that matches \002{0}\002."), email);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Returns the matching accounts whose email address is \037email\037."));
		return true;
	}
};

class NSGetEMail : public Module
{
	CommandNSGetEMail commandnsgetemail;

 public:
	NSGetEMail(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandnsgetemail(this)
	{

	}
};

MODULE_INIT(NSGetEMail)
