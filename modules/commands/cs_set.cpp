/* ChanServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSSet : public Command
{
 public:
	CommandCSSet(Module *creator) : Command(creator, "chanserv/set", 2, 3)
	{
		this->SetDesc(_("Set channel options and information"));
		this->SetSyntax(_("\037option\037 \037channel\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->OnSyntaxError(source, "");
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows the channel founder to set various channel options\n"
			"and other information.\n"
			" \n"
			"Available options:"));
		Anope::string this_name = source.command;
		for (BotInfo::command_map::iterator it = source.owner->commands.begin(), it_end = source.owner->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			CommandInfo &info = it->second;
			if (c_name.find_ci(this_name + " ") == 0)
			{
				service_reference<Command> command(info.name);
				if (command)
				{
					source.command = it->first;
					command->OnServHelp(source);
				}
			}
		}
		source.Reply(_("Type \002%s%s HELP SET \037option\037\002 for more information on a\n"
				"particular option."), Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str());
		return true;
	}
};

class CSSet : public Module
{
	CommandCSSet commandcsset;

 public:
	CSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsset(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSet)
