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

class CommandOSOLine : public Command
{
 public:
	CommandOSOLine(Module *creator) : Command(creator, "operserv/oline", 2, 2)
	{
		this->SetDesc(_("Give Operflags to a certain user"));
		this->SetSyntax(_("\037nick\037 \037flags\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[0];
		const Anope::string &flag = params[1];
		User *u2 = NULL;

		/* let's check whether the user is online */
		if (!(u2 = User::Find(nick, true)))
			source.Reply(_("\002{0}\002 isn't currently online."), nick);
		else if (u2 && flag[0] == '+')
		{
			IRCD->SendSVSO(source.service, nick, flag);
			u2->SetMode(source.service, "OPER");
			u2->SendMessage(*source.service, _("You are now an IRC Operator."));
			source.Reply(_("Operflags \002{0}\002 have been added for \002{1}\002."), flag, nick);
			Log(LOG_ADMIN, source, this) << "for " << nick;
		}
		else if (u2 && flag[0] == '-')
		{
			IRCD->SendSVSO(source.service, nick, flag);
			source.Reply(_("Operflags \002{0}\002 have been removed from \002{1}\002."), flag, nick);
			Log(LOG_ADMIN, source, this) << "for " << nick;
		}
		else
		{
			this->OnSyntaxError(source, "");
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows Services Operators to give Operflags to any user. Flags have to be prefixed with a \"+\" or a \"-\". To remove all flags simply type a \"-\" instead of any flags."));
		return true;
	}
};

class OSOLine : public Module
{
	CommandOSOLine commandosoline;

 public:
	OSOLine(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosoline(this)
	{

		if (!IRCD || !IRCD->CanSVSO)
			throw ModuleException("Your IRCd does not support OMODE.");

	}
};

MODULE_INIT(OSOLine)
