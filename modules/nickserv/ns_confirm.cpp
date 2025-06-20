/* NickServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

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
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"This command is used by several other commands as a way to action changes made "
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
