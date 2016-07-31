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

class CommandNSGetPass : public Command
{
 public:
	CommandNSGetPass(Module *creator) : Command(creator, "nickserv/getpass", 1, 1)
	{
		this->SetDesc(_("Retrieve the password for a nickname"));
		this->SetSyntax(_("\037account\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[0];
		Anope::string tmp_pass;
		NickServ::Nick *na = NickServ::FindNick(nick);

		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		if (Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && na->GetAccount()->IsServicesOper())
		{
			source.Reply(_("You may not get the password of other Services Operators."));
			return;
		}

		if (!Anope::Decrypt(na->GetAccount()->GetPassword(), tmp_pass))
		{
			source.Reply(_("The \002{0}\002 command is unavailable because encryption is in use."), source.command);
			return;
		}

		Log(LOG_ADMIN, source, this) << "for " << na->GetNick();
		source.Reply(_("Password of \002{0}\02 is \002%s\002."), na->GetNick(), tmp_pass);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Returns the password for the given account. This command may not be available if password encryption is in use."));
		return true;
	}
};

class NSGetPass : public Module
{
	CommandNSGetPass commandnsgetpass;

 public:
	NSGetPass(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandnsgetpass(this)
	{

		Anope::string tmp_pass = "plain:tmp";
		if (!Anope::Decrypt(tmp_pass, tmp_pass))
			throw ModuleException("Incompatible with the encryption module being used");

	}
};

MODULE_INIT(NSGetPass)
