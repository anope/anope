/*
 * Anope IRC Services
 *
 * Copyright (C) 2016 Anope Team <team@anope.org>
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

class CommandHSSet : public Command
{
 public:
	CommandHSSet(Module *creator) : Command(creator, "hostserv/set", 2, 3)
	{
		this->SetDesc(_("Set vhost options"));
		this->SetSyntax(_("\037option\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Available options:"));
		// XXX this entire thing is dup
		Anope::string this_name = source.GetCommand();
		bool hide_privileged_commands = Config->GetBlock("options")->Get<bool>("hideprivilegedcommands"),
		     hide_registered_commands = Config->GetBlock("options")->Get<bool>("hideregisteredcommands");
		for (CommandInfo::map::const_iterator it = source.service->commands.begin(), it_end = source.service->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;
			if (c_name.find_ci(this_name + " ") == 0)
			{
				ServiceReference<Command> c(info.name);

				// XXX dup
				if (!c)
					continue;
				else if (hide_registered_commands && !c->AllowUnregistered() && !source.GetAccount())
					continue;
				else if (hide_privileged_commands && !info.permission.empty() && !source.HasCommand(info.permission))
					continue;

				source.SetCommand(it->first);
				c->OnServHelp(source);
			}
		}

		CommandInfo *help = source.service->FindCommand("generic/help");
		if (help)
			source.Reply(_("Type \002{0}{1} {2} {3} \037option\037\002 for more information on a particular option."),
			               Config->StrictPrivmsg, source.service->nick, help->cname, this_name);

		return true;
	}
};

class CommandHSSetDefault : public Command
{
 public:
	CommandHSSetDefault(Module *creator, const Anope::string &cname = "hostserv/set/default") : Command(creator, cname, 1)
	{
		this->SetDesc(_("Sets your default vhost"));
		this->SetSyntax(_("\037vhost\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const Anope::string &mask = params[0];
		std::vector<HostServ::VHost *> vhosts = source.GetAccount()->GetRefs<HostServ::VHost *>();

		if (vhosts.empty())
		{
			source.Reply(_("You do not have any vhosts associated with your account."));
			return;
		}

		HostServ::VHost *vhost = HostServ::FindVHost(source.GetAccount(), mask);
		if (vhost == nullptr)
		{
			source.Reply(_("You do not have the vhost \002{0}\002."), mask);
			return;
		}

		/* Disable default on all vhosts */
		for (HostServ::VHost *v : vhosts)
			v->SetDefault(false);

		/* Set default on chose vhost */
		vhost->SetDefault(true);

		source.Reply(_("Your default vhost is now \002{0}\002."), vhost->Mask());

		logger.Command(LogType::COMMAND, source, _("{source} used {command} set their default vhost to {0}"), vhost->Mask());
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Sets your default vhost. Your default vhost is the vhost which is applied when you first login."));
		return true;
	}
};

class HSSet : public Module
{
	CommandHSSet commandhsset;
	CommandHSSetDefault commandhssetdefault;

 public:
	HSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandhsset(this)
		, commandhssetdefault(this)
	{
	}
};

MODULE_INIT(HSSet)
