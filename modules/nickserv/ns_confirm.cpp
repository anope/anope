// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

class CommandNSConfirm final
	: public Command
{
public:
	CommandNSConfirm(Module *creator)
		: Command(creator, "nickserv/confirm", 1, 3)
	{
		this->AllowUnregistered(true);
		this->SetDesc(_("Confirm a previous action"));
		this->SetSyntax(_("\037type\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"This command is used by several other commands as a way to confirm changes made "
			"to your account. \037type\037 can be one of:"
		));

		auto this_name = source.command;
		auto hide_privileged_commands = Config->GetBlock("options").Get<bool>("hideprivilegedcommands");
		auto hide_registered_commands = Config->GetBlock("options").Get<bool>("hideregisteredcommands");

		HelpWrapper help;
		for (const auto &[c_name, info] : source.service->commands)
		{
			if (c_name.find_ci(this_name + " ") == 0)
			{
				if (info.hide)
					continue;

				ServiceReference<Command> c("Command", info.name);
				if (!c)
					continue;

				else if (hide_registered_commands && !c->AllowUnregistered() && !source.GetAccount())
					continue;

				else if (hide_privileged_commands && !info.permission.empty() && !source.HasCommand(info.permission))
					continue;

				source.command = c_name;
				c->OnServHelp(source, help);
			}
		}
		help.SendTo(source);

		source.Reply(_("Type \002%s\032\037option\037\002 for more information on a specific option."),
			source.service->GetQueryCommand("generic/help", this_name).c_str());

		return true;
	}
};

class NSConfirm final
	: public Module
{
private:
	CommandNSConfirm commandnsconfirm;

public:
	NSConfirm(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnsconfirm(this)
	{
	}
};

MODULE_INIT(NSConfirm)
