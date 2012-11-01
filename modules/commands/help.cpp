/* Core functions
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

class CommandHelp : public Command
{
 public:
	CommandHelp(Module *creator) : Command(creator, "generic/help", 0)
	{
		this->SetDesc(_("Displays this list and give information about commands"));
		this->SetFlag(CFLAG_STRIP_CHANNEL);
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnPreHelp, OnPreHelp(source, params));
		if (MOD_RESULT == EVENT_STOP)
			return;
	
		const BotInfo *bi = source.service;
		const CommandInfo::map &map = source.c ? Config->Fantasy : bi->commands;

		if (params.empty())
		{
			for (CommandInfo::map::const_iterator it = map.begin(), it_end = map.end(); it != it_end; ++it)
			{
				const Anope::string &c_name = it->first;
				const CommandInfo &info = it->second;

				// Smaller command exists
				Anope::string cmd = myStrGetToken(c_name, ' ', 0);
				if (cmd != it->first && map.count(cmd))
					continue;

				service_reference<Command> c("Command", info.name);
				if (!c)
					continue;
				else if (!Config->HidePrivilegedCommands)
					; // Always show with HidePrivilegedCommands disabled
				else if (!c->HasFlag(CFLAG_ALLOW_UNREGISTERED) && !source.GetAccount())
					continue;
				else if (!info.permission.empty() && !source.HasCommand(info.permission))
					continue;

				source.command = c_name;
				c->OnServHelp(source);

			}
		}
		else
		{
			bool helped = false;
			for (unsigned max = params.size(); max > 0; --max)
			{
				Anope::string full_command;
				for (unsigned i = 0; i < max; ++i)
					full_command += " " + params[i];
				full_command.erase(full_command.begin());

				CommandInfo::map::const_iterator it = map.find(full_command);
				if (it == map.end())
					continue;

				const CommandInfo &info = it->second;

				service_reference<Command> c("Command", info.name);
				if (!c)
					continue;
				else if (!Config->HidePrivilegedCommands)
					; // Always show with HidePrivilegedCommands disabled
				else if (!info.permission.empty() && !source.HasCommand(info.permission))
					continue;
				
				// Allow unregistered users to see help for commands that they explicitly request help for

				const Anope::string &subcommand = params.size() > max ? params[max] : "";
				source.command = full_command;
				if (!c->OnHelp(source, subcommand))
					continue;

				helped = true;

				/* Inform the user what permission is required to use the command */
				if (!info.permission.empty())
				{
					source.Reply(" ");
					source.Reply(_("Access to this command requires the permission \002%s\002 to be present in your opertype."), info.permission.c_str());
				}
				if (!c->HasFlag(CFLAG_ALLOW_UNREGISTERED) && !source.nc)
				{
					if (info.permission.empty())
						source.Reply(" ");
					source.Reply( _("You need to be identified to use this command."));
				}
				/* User doesn't have the proper permission to use this command */
				else if (!info.permission.empty() && !source.HasCommand(info.permission))
				{
					source.Reply(_("You cannot use this command."));
				}

				break;
			}

			if (helped == false)
				source.Reply(_("No help available for \002%s\002."), params[0].c_str());
		}
	
		FOREACH_MOD(I_OnPostHelp, OnPostHelp(source, params));

		return;
	}
};

class Help : public Module
{
	CommandHelp commandhelp;

 public:
	Help(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandhelp(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(Help)
