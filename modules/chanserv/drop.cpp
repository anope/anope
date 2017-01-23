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
#include "modules/chanserv/drop.h"

class CommandCSDrop : public Command
{
 public:
	CommandCSDrop(Module *creator) : Command(creator, "chanserv/drop", 1, 2)
	{
		this->SetDesc(_("Cancel the registration of a channel"));
		this->SetSyntax(_("\037channel\037 \037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		if (Anope::ReadOnly && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Sorry, channel de-registration is temporarily disabled."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (params.size() < 2 || !chan.equals_ci(params[1]))
		{
			source.Reply(_("You must enter the channel name twice as a confirmation that you wish to drop \002{0}\002."), ci->GetName());
			return;
		}

		if ((ci->IsSecureFounder() ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")) && !source.HasCommand("chanserv/drop"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->GetName());
			return;
		}

		EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&Event::ChanDrop::OnChanDrop, source, ci);
		if (MOD_RESULT == EVENT_STOP)
			return;

		bool override = (ci->IsSecureFounder() ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER"));
		logger.Command(override ? LogType::OVERRIDE : LogType::COMMAND, source, ci, _("{source} used {command} on {channel} (founder was: {0})"),
				ci->GetFounder() ? ci->GetFounder()->GetDisplay() : "none");

		Reference<Channel> c = ci->c;
		ci->Delete();

		source.Reply(_("Channel \002{0}\002 has been dropped."), chan);

		if (c)
			c->CheckModes();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Unregisters \037channel\037. All channel settings will be deleted. As a precaution, you must give the \037channel\037 name twice.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege on \037channel\037."),
		               "FOUNDER");

		if (source.HasCommand("chanserv/drop"))
			source.Reply(_("\n"
			               "As a Services Operator with the command \002{0}\002, you may drop any channel."),
			               "chanserv/drop");

		return true;
	}
};

class CSDrop : public Module
{
	CommandCSDrop commandcsdrop;

 public:
	CSDrop(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsdrop(this)
	{

	}
};

MODULE_INIT(CSDrop)
