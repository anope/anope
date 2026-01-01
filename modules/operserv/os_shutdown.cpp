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

namespace
{
	bool NetworkNameGiven(Command *cmd, CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!Config->GetModule(cmd->owner).Get<bool>("requirename"))
			return true; // Not necesary.

		if (params.empty())
			cmd->OnSyntaxError(source, source.command);
		else if (!params[0].equals_cs(Config->GetBlock("networkinfo").Get<Anope::string>("networkname")))
			source.Reply(_("The network name you specified is incorrect. Did you mean to run %s on a different network?"), source.command.nobreak().c_str());
		else
			return true; // Name was specified correctly
		return false; // Network name was wrong or not specified.
	}
}

class CommandOSQuit final
	: public Command
{
public:
	CommandOSQuit(Module *creator) : Command(creator, "operserv/quit", 0, 1)
	{
		this->SetDesc(_("Terminate services WITHOUT saving"));
		if (Config->GetModule(this->owner).Get<bool>("requirename"))
			this->SetSyntax(_("\037network-name\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!NetworkNameGiven(this, source, params))
			return;

		Log(LOG_ADMIN, source, this);
		Anope::QuitReason = source.command + " command received from " + source.GetNick();
		Anope::Quitting = true;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Causes services to do an immediate shutdown; databases are "
			"\002not\002 saved. This command should not be used unless "
			"damage to the in-memory copies of the databases is feared "
			"and they should not be saved."
		));
		return true;
	}
};

class CommandOSRestart final
	: public Command
{
public:
	CommandOSRestart(Module *creator) : Command(creator, "operserv/restart", 0, 1)
	{
		this->SetDesc(_("Save databases and restart services"));
		if (Config->GetModule(this->owner).Get<bool>("requirename"))
			this->SetSyntax(_("\037network-name\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!NetworkNameGiven(this, source, params))
			return;

		Log(LOG_ADMIN, source, this);
		Anope::QuitReason = source.command + " command received from " + source.GetNick();
		Anope::Quitting = Anope::Restarting = true;
		Anope::SaveDatabases();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(_(
			"Causes services to save all databases and then restart "
			"(i.e. exit and immediately re-run the executable)."
		));
		return true;
	}
};

class CommandOSShutdown final
	: public Command
{
public:
	CommandOSShutdown(Module *creator) : Command(creator, "operserv/shutdown", 0, 1)
	{
		this->SetDesc(_("Terminate services with save"));
		if (Config->GetModule(this->owner).Get<bool>("requirename"))
			this->SetSyntax(_("\037network-name\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!NetworkNameGiven(this, source, params))
			return;

		Log(LOG_ADMIN, source, this);
		Anope::QuitReason = source.command + " command received from " + source.GetNick();
		Anope::Quitting = true;
		Anope::SaveDatabases();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Causes services to save all databases and then shut down."));
		return true;
	}
};

class OSShutdown final
	: public Module
{
	CommandOSQuit commandosquit;
	CommandOSRestart commandosrestart;
	CommandOSShutdown commandosshutdown;

public:
	OSShutdown(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandosquit(this), commandosrestart(this), commandosshutdown(this)
	{

	}
};

MODULE_INIT(OSShutdown)
