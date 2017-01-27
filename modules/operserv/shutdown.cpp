/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
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

class CommandOSRestart : public Command
{
 public:
	CommandOSRestart(Module *creator) : Command(creator, "operserv/restart", 0, 0)
	{
		this->SetDesc(_("Restart Anope"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		logger.Command(LogType::ADMIN, source, _("{source} used {command}"));
		Anope::QuitReason = source.GetCommand() + " command received from " + source.GetNick();
		Anope::Quitting = Anope::Restarting = true;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Causes Anope to restart."));
		return true;
	}
};

class CommandOSShutdown : public Command
{
 public:
	CommandOSShutdown(Module *creator) : Command(creator, "operserv/shutdown", 0, 0)
	{
		this->SetDesc(_("Shutdown Anope"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		logger.Command(LogType::ADMIN, source, _("{source} used {command}"));
		Anope::QuitReason = source.GetCommand() + " command received from " + source.GetNick();
		Anope::Quitting = true;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Causes Anope to shut down"));
		return true;
	}
};

class OSShutdown : public Module
{
	CommandOSRestart commandosrestart;
	CommandOSShutdown commandosshutdown;

 public:
	OSShutdown(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosrestart(this)
		, commandosshutdown(this)
	{
	}
};

MODULE_INIT(OSShutdown)
