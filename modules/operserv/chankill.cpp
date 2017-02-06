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

class CommandOSChanKill : public Command
{
	ServiceReference<XLineManager> akills;
	
 public:
	CommandOSChanKill(Module *creator) : Command(creator, "operserv/chankill", 2, 3)
		, akills("xlinemanager/sgline")
	{
		this->SetDesc(_("AKILL all users on a specific channel"));
		this->SetSyntax(_("[+\037expiry\037] \037channel\037 \037reason\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!akills)
			return;

		Anope::string expiry, channel;
		unsigned last_param = 1;

		channel = params[0];
		if (!channel.empty() && channel[0] == '+')
		{
			expiry = channel;
			channel = params[1];
			last_param = 2;
		}

		time_t expires = !expiry.empty() ? Anope::DoTime(expiry) : Config->GetModule("operserv/main")->Get<time_t>("autokillexpiry", "30d");
		if (!expiry.empty() && isdigit(expiry[expiry.length() - 1]))
			expires *= 86400;

		if (expires)
		{
			if (expires < 60)
			{
				source.Reply(_("Invalid expiry time \002{0}\002."), expiry);
				return;
			}

			expires += Anope::CurTime;
		}

		if (params.size() <= last_param)
		{
			this->OnSyntaxError(source, "");
			return;
		}

		Anope::string reason = params[last_param];
		if (params.size() > last_param + 1)
			reason += params[last_param + 1];

		Anope::string realreason;
		if (Config->GetModule("operserv/main")->Get<bool>("addakiller") && !source.GetNick().empty())
			realreason = "[" + source.GetNick() + "] " + reason;
		else
			realreason = reason;

		Channel *c = Channel::Find(channel);
		if (!c)
		{
			source.Reply(_("Channel \002{0}\002 doesn't exist."), channel);
			return;
		}

		for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
		{
			ChanUserContainer *uc = it->second;

			if (uc->user->server == Me || uc->user->HasMode("OPER"))
				continue;

			Anope::string akillmask = "*@" + uc->user->host;

			if (akills->HasEntry(akillmask))
				continue;

			XLine *x = Serialize::New<XLine *>();
			x->SetMask(akillmask);
			x->SetBy(source.GetNick());
			x->SetExpires(expires);
			x->SetReason(realreason);
			x->SetID(XLineManager::GenerateUID());

			akills->AddXLine(x);
			akills->OnMatch(uc->user, x);
		}

		logger.Admin(source, _("{source} used {command} to {0} ({1})"), c->name, realreason);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Adds an auto kill for every user on \037channel\037, except for IRC operators."));
		return true;
	}
};

class OSChanKill : public Module
{
	CommandOSChanKill commandoschankill;

 public:
	OSChanKill(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandoschankill(this)
	{

	}
};

MODULE_INIT(OSChanKill)
