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

class CommandOSMode : public Command
{
 public:
	CommandOSMode(Module *creator) : Command(creator, "operserv/mode", 2, 3)
	{
		this->SetDesc(_("Change channel modes"));
		this->SetSyntax(_("\037channel\037 \037modes\037"));
		this->SetSyntax(_("\037channel\037 CLEAR [ALL]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &target = params[0];
		const Anope::string &modes = params[1];

		Reference<Channel> c = Channel::Find(target);
		if (!c)
			source.Reply(_("Channel \002{0}\002 doesn't exist."), target);
		else if (c->bouncy_modes)
			source.Reply(_("Services is unable to change modes. Are your servers' U:lines configured correctly?"));
		else if (modes.equals_ci("CLEAR"))
		{
			bool all = params.size() > 2 && params[2].equals_ci("ALL");

			const Channel::ModeList chmodes = c->GetModes();
			for (Channel::ModeList::const_iterator it = chmodes.begin(), it_end = chmodes.end(); it != it_end && c; ++it)
				c->RemoveMode(c->ci->WhoSends(), it->first, it->second, false);

			if (!c)
			{
				source.Reply(_("Modes cleared on %s and the channel destroyed."), target.c_str());
				return;
			}

			if (all)
			{
				for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
				{
					ChanUserContainer *uc = it->second;

					if (uc->user->HasMode("OPER"))
						continue;

					for (size_t i = uc->status.Modes().length(); i > 0; --i)
						c->RemoveMode(c->ci->WhoSends(), ModeManager::FindChannelModeByChar(uc->status.Modes()[i - 1]), uc->user->GetUID(), false);
				}

				source.Reply(_("All modes cleared on \002{0}\002."), c->name);
			}
			else
				source.Reply(_("Non-status modes cleared on \002{0}\002."), c->name);
		}
		else
		{
			spacesepstream sep(modes + (params.size() > 2 ? " " + params[2] : ""));
			Anope::string mode;
			int add = 1;
			Anope::string log_modes, log_params;

			sep.GetToken(mode);
			for (unsigned i = 0; i < mode.length() && c; ++i)
			{
				char ch = mode[i];

				if (ch == '+')
				{
					add = 1;
					log_modes += "+";
					continue;
				}
				else if (ch == '-')
				{
					add = 0;
					log_modes += "-";
					continue;
				}

				ChannelMode *cm = ModeManager::FindChannelModeByChar(ch);
				if (!cm)
					continue;

				Anope::string param, param_log;
				if (cm->type != MODE_REGULAR)
				{
					if (cm->type == MODE_PARAM && !add && anope_dynamic_static_cast<ChannelModeParam *>(cm)->minus_no_arg)
						;
					else if (!sep.GetToken(param))
						continue;

					param_log = param;

					if (cm->type == MODE_STATUS)
					{
						User *targ = User::Find(param, true);
						if (targ == NULL || c->FindUser(targ) == NULL)
							continue;
						param = targ->GetUID();
					}
				}

				log_modes += cm->mchar;
				if (!param.empty())
					log_params += " " + param_log;

				if (add)
					c->SetMode(source.service, cm, param, false);
				else
					c->RemoveMode(source.service, cm, param, false);
			}

			if (!log_modes.replace_all_cs("+", "").replace_all_cs("-", "").empty())
				Log(LOG_ADMIN, source, this) << log_modes << log_params << " on " << (c ? c->name : target);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows Services Operators to change modes for any channel. Parameters are the same as for the standard /MODE command.\n"
				"Alternatively, CLEAR may be given to clear all modes on the channel.\n"
				"If CLEAR ALL is given then all modes, including user status, is removed."));
		return true;
	}
};

class CommandOSUMode : public Command
{
 public:
	CommandOSUMode(Module *creator) : Command(creator, "operserv/umode", 2, 2)
	{
		this->SetDesc(_("Change user modes"));
		this->SetSyntax(_("\037user\037 \037modes\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &target = params[0];
		const Anope::string &modes = params[1];

		User *u2 = User::Find(target, true);
		if (!u2)
			source.Reply(_("\002{0}\002 isn't currently online."), target);
		else
		{
			u2->SetModes(source.service, "%s", modes.c_str());
			source.Reply(_("Changed usermodes of \002{0}\002 to \002{1}\002."), u2->nick.c_str(), modes.c_str());

			u2->SendMessage(*source.service, _("\002{0}\002 changed your usermodes to \002{1}\002."), source.GetNick(), modes);

			Log(LOG_ADMIN, source, this) << modes << " on " << target;
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows Services Operators to change modes for any user. Parameters are the same as for the standard /MODE command."));
		return true;
	}
};

class OSMode : public Module
{
	CommandOSMode commandosmode;
	CommandOSUMode commandosumode;

 public:
	OSMode(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosmode(this)
		, commandosumode(this)
	{

	}
};

MODULE_INIT(OSMode)
