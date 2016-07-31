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

class CommandOSUpdate : public Command
{
 public:
	CommandOSUpdate(Module *creator) : Command(creator, "operserv/update", 0, 0)
	{
		this->SetDesc(_("Force the Services databases to be updated immediately"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Log(LOG_ADMIN, source, this);
		source.Reply(_("Updating databases."));
		Anope::SaveDatabases();
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Causes Services to update all database files."));
		return true;
	}
};

class OSUpdate : public Module
{
	CommandOSUpdate commandosupdate;

 public:
	OSUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosupdate(this)
	{

	}
};

MODULE_INIT(OSUpdate)
