/* OperServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandOSConfig : public Command
{
 public:
	CommandOSConfig(Module *creator) : Command(creator, "operserv/config", 1, 4)
	{
		this->SetDesc(_("View and change configuration file settings"));
		this->SetSyntax(_("{\037MODIFY\037|\037VIEW\037} [\037block name\037 \037item name\037 \037item value\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &what = params[0];

		if (what.equals_ci("MODIFY") && params.size() > 3)
		{
			Configuration::Block *block = Config->GetBlock(params[1]);
			if (!block)
			{
				source.Reply(_("There is no such configuration block %s."), params[1].c_str());
				return;
			}

			block->Set(params[2], params[3]);

			Log(LOG_ADMIN, source, this) << "to change the configuration value of " << params[1] << ":" << params[2] << " to " << params[3];
			source.Reply(_("Value of %s:%s changed to %s"), params[1].c_str(), params[2].c_str(), params[3].c_str());
		}
		else if (what.equals_ci("VIEW"))
		{
			/* Blocks we should show */
			const Anope::string show_blocks[] = { "botserv", "chanserv", "defcon", "global", "memoserv", "nickserv", "networkinfo", "operserv", "options", "" };

			Log(LOG_ADMIN, source, this) << "VIEW";
			
			for (unsigned i = 0; !show_blocks[i].empty(); ++i)
			{
				Configuration::Block *block = Config->GetBlock(show_blocks[i]);
				const Configuration::Block::item_map *items = block->GetItems();

				if (!items)
					continue;

				ListFormatter lflist;
				lflist.AddColumn("Name").AddColumn("Value");

				for (Configuration::Block::item_map::const_iterator it = items->begin(), it_end = items->end(); it != it_end; ++it)
				{
					ListFormatter::ListEntry entry;
					entry["Name"] = it->first;
					entry["Value"] = it->second;
					lflist.AddEntry(entry);
				}

				std::vector<Anope::string> replies;
				lflist.Process(replies);

				source.Reply(_("%s settings:"), block->GetName().c_str());

				for (unsigned i = 0; i < replies.size(); ++i)
					source.Reply(replies[i]);
			}

			source.Reply(_("End of configuration."));
		}
		else
			this->OnSyntaxError(source, what);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to change and view configuration settings.\n"
				"Settings changed by this command are temporary and will not be reflected\n"
				"back into the configuration file, and will be lost if Anope is shut down,\n"
				"restarted, or the RELOAD command is used.\n"
				" \n"
				"Example:\n"
				"     \002MODIFY nickserv forcemail no\002"));
		return true;
	}
};

class OSConfig : public Module
{
	CommandOSConfig commandosconfig;

 public:
	OSConfig(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandosconfig(this)
	{

	}
};

MODULE_INIT(OSConfig)
