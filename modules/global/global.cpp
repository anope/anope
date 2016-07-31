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
#include "modules/global.h"

class CommandGLGlobal : public Command
{
	ServiceReference<Global::GlobalService> service;

 public:
	CommandGLGlobal(Module *creator) : Command(creator, "global/global", 1, 1)
	{
		this->SetDesc(_("Send a message to all users"));
		this->SetSyntax(_("\037message\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &msg = params[0];

		if (!service)
		{
			source.Reply("No global reference, is global loaded?");
			return;
		}

		Log(LOG_ADMIN, source, this);
		service->SendGlobal(NULL, source.GetNick(), msg);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows Services Operators to send a message to all users on the network."
		               " The message will be sent from \002{0}\002."), source.service->nick);
		return true;
	}
};

class GLGlobal : public Module
{
	CommandGLGlobal commandglglobal;

 public:
	GLGlobal(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandglglobal(this)
	{

	}
};

MODULE_INIT(GLGlobal)
