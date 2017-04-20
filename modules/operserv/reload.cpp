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

class CommandOSReload : public Command
{
 public:
	CommandOSReload(Module *creator) : Command(creator, "operserv/reload", 0, 0)
	{
		this->SetDesc(_("Reload services' configuration file"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		try
		{
			logger.Admin(source, _("{source} used {command}"));

			Configuration::Conf *new_config = new Configuration::Conf();
			Configuration::Conf *old = Config;
			Config = new_config;
			Config->Post(old);
			delete old;

			source.Reply(_("Services' configuration has been reloaded."));
		}
		catch (const ConfigException &ex)
		{
			this->GetOwner()->logger.Log(_("Error reloading configuration file: {0}"), ex.GetReason());
			source.Reply(_("Error reloading configuration file: {0}"), ex.GetReason());
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Causes Services to reload the configuration file."));
		return true;
	}
};

class OSReload : public Module
{
	CommandOSReload commandosreload;

 public:
	OSReload(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosreload(this)
	{

	}
};

MODULE_INIT(OSReload)
