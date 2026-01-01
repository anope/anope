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

class CommandOSReload final
	: public Command
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
			Log(LOG_ADMIN, source, this);

			auto *new_config = new Configuration::Conf();
			Configuration::Conf *old = Config;
			Config = new_config;
			Config->Post(old);
			delete old;

			source.Reply(_("Services' configuration has been reloaded."));
		}
		catch (const ConfigException &ex)
		{
			Log(this->owner) << "Error reloading configuration file: " << ex.GetReason();
			source.Reply(_("Error reloading configuration file: %s"), ex.GetReason().c_str());
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Causes services to reload the configuration file. Note that "
			"some directives still need the restart of the services to "
			"take effect (such as services' nicknames, activation of the "
			"session limitation, etc.)."
		));
		return true;
	}
};

class OSReload final
	: public Module
{
	CommandOSReload commandosreload;

public:
	OSReload(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandosreload(this)
	{

	}
};

MODULE_INIT(OSReload)
