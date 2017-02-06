/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
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

class CommandOSConfig : public Command
{
 public:
	CommandOSConfig(Module *creator) : Command(creator, "operserv/config", 1, 4)
	{
		this->SetDesc(_("View and change configuration file settings"));
		this->SetSyntax(_("{\037MODIFY\037|\037VIEW\037} [\037block name\037 \037item name\037 \037item value\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &what = params[0];

		if (what.equals_ci("MODIFY") && params.size() > 3)
		{
			if (!source.HasPriv("operserv/config"))
			{
				source.Reply(_("Access denied. You do not have the operator privilege \002{0}\002."), "operserv/config");
				return;
			}

			Configuration::Block *block = Config->GetBlock(params[1]);
			if (!block)
				block = Config->GetModule(params[1]);

			if (!block)
			{
				source.Reply(_("There is no such configuration block \002{0}\002."), params[1]);
				return;
			}

			block->Set(params[2], params[3]);

			logger.Admin(source, _("{source} used {command} to change the configuration value of {0}:{1} to {2}"),
					params[1], params[2], params[3]);
			source.Reply(_("Value of \002{0}:{1}\002 changed to \002{2}\002."), params[1], params[2], params[3]);
		}
		else if (what.equals_ci("VIEW"))
		{
			/* Blocks we should show */
			const Anope::string show_blocks[] = { "serverinfo", "networkinfo", "options", "" };

			logger.Admin(source, _("{source} used {command} to view the configuration"));

			for (unsigned i = 0; !show_blocks[i].empty(); ++i)
			{
				Configuration::Block *block = Config->GetBlock(show_blocks[i]);
				const Configuration::Block::item_map *items = block->GetItems();

				if (!items)
					continue;

				ListFormatter lflist(source.GetAccount());
				lflist.AddColumn(_("Name")).AddColumn(_("Value"));

				for (Configuration::Block::item_map::const_iterator it = items->begin(), it_end = items->end(); it != it_end; ++it)
				{
					ListFormatter::ListEntry entry;
					entry["Name"] = it->first;
					entry["Value"] = it->second;
					lflist.AddEntry(entry);
				}

				std::vector<Anope::string> replies;
				lflist.Process(replies);

				source.Reply(_("%s settings:"), block->GetName());

				for (unsigned j = 0; j < replies.size(); ++j)
					source.Reply(replies[j]);

				source.Reply(" ");
			}

			ListFormatter lflist(source.GetAccount());
			lflist.AddColumn(_("Module Name")).AddColumn(_("Name")).AddColumn(_("Value"));

			for (int i = 0; i < Config->CountBlock("module"); ++i)
			{
				Configuration::Block *block = Config->GetBlock("module", i);
				const Configuration::Block::item_map *items = block->GetItems();

				if (!items || items->size() <= 1)
					continue;

				ListFormatter::ListEntry entry;
				entry["Module Name"] = block->Get<Anope::string>("name");

				for (Configuration::Block::item_map::const_iterator it = items->begin(), it_end = items->end(); it != it_end; ++it)
				{
					entry["Name"] = it->first;
					entry["Value"] = it->second;
					lflist.AddEntry(entry);
				}
			}

			std::vector<Anope::string> replies;
			lflist.Process(replies);

			source.Reply(_("Module settings:"));

			for (unsigned j = 0; j < replies.size(); ++j)
				source.Reply(replies[j]);

			source.Reply(_("End of configuration."));
		}
		else
		{
			this->OnSyntaxError(source, what);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows you to change and view configuration settings. Settings changed by this command are temporary and will not be reflected back into the configuration file, and will be lost if Anope is shut down, restarted, or the configuration is reloaded.\n"
		               "Use of the \002MODIFY\002 command requires the \002{0}\002 operator privilege.\n"
		               "\n"
		               "Example:\n"
		               "         {0} MODIFY nickserv forcemail no"
		               "         Changes the \"forceemail\" setting of the module configuration for \"nickserv\" to \"no\""));
		return true;
	}
};

class OSConfig : public Module
{
	CommandOSConfig commandosconfig;

 public:
	OSConfig(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosconfig(this)
	{

	}
};

MODULE_INIT(OSConfig)
