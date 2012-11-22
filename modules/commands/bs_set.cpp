/* BotServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandBSSet : public Command
{
 public:
	CommandBSSet(Module *creator) : Command(creator, "botserv/set", 3, 3)
	{
		this->SetDesc(_("Configures bot options"));
		this->SetSyntax(_("\037option\037 \037(channel | bot)\037 \037settings\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Configures bot options.\n"
			" \n"
			"Available options:"));
		Anope::string this_name = source.command;
		for (CommandInfo::map::const_iterator it = source.service->commands.begin(), it_end = source.service->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;
			if (c_name.find_ci(this_name + " ") == 0)
			{
				ServiceReference<Command> command("Command", info.name);
				if (command)
				{
					source.command = it->first;
					command->OnServHelp(source);
				}
			}
		}
		source.Reply(_("Type \002%s%s HELP SET \037option\037\002 for more information on a\n"
				"particular option."), Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());

		return true;
	}
};

class BSSet : public Module
{
	CommandBSSet commandbsset;

 public:
	BSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbsset(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(BSSet)
