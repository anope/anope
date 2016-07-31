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

class CommandCSGetKey : public Command
{
 public:
	CommandCSGetKey(Module *creator) : Command(creator, "chanserv/getkey", 1, 1)
	{
		this->SetDesc(_("Returns the key of the given channel"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("GETKEY") && !source.HasCommand("chanserv/getkey"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "GETKEY", ci->GetName());
			return;
		}

		Anope::string key;
		if (!ci->c || !ci->c->GetParam("KEY", key))
		{
			source.Reply(_("Channel \002{0}\002 does not have a key."), ci->GetName());
			return;
		}

		bool override = !source.AccessFor(ci).HasPriv("GETKEY");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci);

		source.Reply(_("Key for channel \002{0}\002 is \002{1}\002."), ci->GetName(), key);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Returns the key of \037channel\037.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege on \037channel\037."),
		               "GETKEY");
		return true;
	}
};

class CSGetKey : public Module
{
	CommandCSGetKey commandcsgetkey;

 public:
	CSGetKey(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsgetkey(this)
	{

	}
};

MODULE_INIT(CSGetKey)
