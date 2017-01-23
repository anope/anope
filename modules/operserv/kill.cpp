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

class CommandOSKill : public Command
{
 public:
	CommandOSKill(Module *creator) : Command(creator, "operserv/kill", 1, 2)
	{
		this->SetDesc(_("Kill a user"));
		this->SetSyntax(_("\037user\037 [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[0];
		Anope::string reason = params.size() > 1 ? params[1] : "";

		User *u2 = User::Find(nick, true);
		if (u2 == NULL)
		{
			source.Reply(_("\002{0}\002 isn't currently online."), nick);
			return;
		}

		if (u2->IsProtected() || u2->server == Me)
		{
			source.Reply(_("\002{0}\002 is protected and cannot be killed."), u2->nick);
			return;
		}

		if (reason.empty())
			reason = "No reason specified";
		if (Config->GetModule("operserv/main")->Get<bool>("addakiller"))
			reason = "(" + source.GetNick() + ") " + reason;

		logger.Command(LogType::ADMIN, source, _("{source} used {command} on {0} for {1}"), u2->nick, reason);

		u2->Kill(*source.service, reason);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows you to kill a user from the network. Parameters are the same as for the standard /KILL command."));
		return true;
	}
};

class OSKill : public Module
{
	CommandOSKill commandoskill;

 public:
	OSKill(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandoskill(this)
	{

	}
};

MODULE_INIT(OSKill)
