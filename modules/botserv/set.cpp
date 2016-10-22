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
#include "modules/botserv/kick.h"

class CommandBSSet : public Command
{
 public:
	CommandBSSet(Module *creator) : Command(creator, "botserv/set", 3, 3)
	{
		this->SetDesc(_("Configures bot options"));
		this->SetSyntax(_("\037option\037 \037(channel | bot)\037 \037settings\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Configures bot options.\n"
			"\n"
			"Available options:"));
		bool hide_privileged_commands = Config->GetBlock("options")->Get<bool>("hideprivilegedcommands"),
		     hide_registered_commands = Config->GetBlock("options")->Get<bool>("hideregisteredcommands");
		Anope::string this_name = source.command;
		for (CommandInfo::map::const_iterator it = source.service->commands.begin(), it_end = source.service->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;
			if (c_name.find_ci(this_name + " ") == 0)
			{
				ServiceReference<Command> command(info.name);
				if (command)
				{
					// XXX dup
					if (hide_registered_commands && !command->AllowUnregistered() && !source.GetAccount())
						continue;

					if (hide_privileged_commands && !info.permission.empty() && !source.HasCommand(info.permission))
						continue;

					source.command = it->first;
					command->OnServHelp(source);
				}
			}
		}

		CommandInfo *help = source.service->FindCommand("generic/help");
		if (help)
			source.Reply(_("Type \002{0}{1} {2} {3} \037option\037\002 for more information on a particular option."),
			               Config->StrictPrivmsg, source.service->nick, help->cname, this_name);

		return true;
	}
};

class CommandBSSetBanExpire : public Command
{
 public:
 	class UnbanTimer : public Timer
	{
		Anope::string chname;
		Anope::string mask;

	 public:
		UnbanTimer(Module *creator, const Anope::string &ch, const Anope::string &bmask, time_t t) : Timer(creator, t), chname(ch), mask(bmask) { }

		void Tick(time_t) override
		{
			Channel *c = Channel::Find(chname);
			if (c)
				c->RemoveMode(NULL, "BAN", mask);
		}
	};

	CommandBSSetBanExpire(Module *creator, const Anope::string &sname = "botserv/set/banexpire") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Configures the time bot bans expire in"));
		this->SetSyntax(_("\037channel\037 \037time\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &arg = params[1];

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		ChanServ::AccessGroup access = source.AccessFor(ci);
		if (!source.HasPriv("botserv/administration") && !access.HasPriv("SET"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, changing bot options is temporarily disabled."));
			return;
		}

		time_t t = Anope::DoTime(arg);
		if (t == -1)
		{
			source.Reply(_("Invalid expiry time \002{0}\002."), arg);
			return;
		}

		/* cap at 1 day */
		if (t > 86400)
		{
			source.Reply(_("Ban expiry may not be longer than 1 day."));
			return;
		}

		ci->SetBanExpire(t);

		bool override = !access.HasPriv("SET");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to change banexpire to " << arg;

		if (!t)
			source.Reply(_("Bot bans will no longer automatically expire."));
		else
			source.Reply(_("Bot bans will automatically expire after \002{0}\002."), Anope::Duration(t, source.GetAccount()));
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Sets the time bot bans expire in. If enabled, any bans placed by bots, such as by the flood kicker, badwords kicker, etc. will automatically be removed after the given time."
		               " Set to 0 to disable bans from automatically expiring."));
		return true;
	}
};

class CommandBSSetPrivate : public Command
{
 public:
	CommandBSSetPrivate(Module *creator, const Anope::string &sname = "botserv/set/private") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Prevent a bot from being assigned by non Services Operators"));
		this->SetSyntax(_("\037botname\037 {\037ON|OFF\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[9];
		const Anope::string &value = params[1];

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		ServiceBot *bi = ServiceBot::Find(nick, true);
		if (bi == NULL || !bi->bi)
		{
			source.Reply(_("Bot \002{0}\002 does not exist."), nick);
			return;
		}

		if (value.equals_ci("ON"))
		{
			bi->bi->SetOperOnly(true);
			source.Reply(_("Private mode of bot \002{0}\002 is now \002on\002."), bi->nick);
		}
		else if (value.equals_ci("OFF"))
		{
			bi->bi->SetOperOnly(false);
			source.Reply(_("Private mode of bot \002{0}\002 is now \002off\002."), bi->nick);
		}
		else
		{
			this->OnSyntaxError(source, source.command);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("This option prevents a bot from being assigned to channels by users who do not have the \002{0}\002 privilege."),
		               "botserv/administration");
		return true;
	}
};

class BSSet : public Module
	, public EventHook<Event::BotBan>
{
	CommandBSSet commandbsset;
	CommandBSSetBanExpire commandbssetbanexpire;
	CommandBSSetPrivate commandbssetprivate;

 public:
	BSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::BotBan>(this)
		, commandbsset(this)
		, commandbssetbanexpire(this)
		, commandbssetprivate(this)
	{
	}

	void OnBotBan(User *u, ChanServ::Channel *ci, const Anope::string &mask) override
	{
		if (!ci->GetBanExpire())
			return;

		new CommandBSSetBanExpire::UnbanTimer(this, ci->GetName(), mask, ci->GetBanExpire());
	}
};

MODULE_INIT(BSSet)
