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

class CommandOSKick : public Command
{
 public:
	CommandOSKick(Module *creator) : Command(creator, "operserv/kick", 3, 3)
	{
		this->SetDesc(_("Kick a user from a channel"));
		this->SetSyntax(_("\037channel\037 \037user\037 \037reason\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &nick = params[1];
		const Anope::string &s = params[2];
		Channel *c;
		User *u2;

		if (!(c = Channel::Find(chan)))
		{
			source.Reply(_("Channel \002%s\002 doesn't exist."), chan);
			return;
		}

		if (c->bouncy_modes)
		{
			source.Reply(_("Services is unable to change modes. Are your servers' U:lines configured correctly?"));
			return;
		}

		if (!(u2 = User::Find(nick, true)))
		{
			source.Reply(_("\002{0}\002 isn't currently online."), nick);
			return;
		}

		if (!c->Kick(source.service, u2, "{0} ({1})", source.GetNick(), s))
		{
			source.Reply(_("Access denied."));
			return;
		}

		Log(LOG_ADMIN, source, this) << "on " << u2->nick << " in " << c->name << " (" << s << ")";
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows you to kick a user from any channel. Parameters are the same as for the standard /KICK command."
		               " The kick message will have the nickname of the IRCop sending the KICK command prepended; for example:\n"
		               "\n"
		               "*** SpamMan has been kicked off channel #my_channel by {0} (Alcan (Flood))"),
		               source.service->nick);
		return true;
	}
};

class OSKick : public Module
{
	CommandOSKick commandoskick;

 public:
	OSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandoskick(this)
	{

	}
};

MODULE_INIT(OSKick)
