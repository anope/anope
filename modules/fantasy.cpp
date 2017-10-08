/*
 * Anope IRC Services
 *
 * Copyright (C) 2013-2017 Anope Team <team@anope.org>
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
#include "modules/fantasy.h"
#include "modules/botserv/info.h"

class CommandBSSetFantasy : public Command
{
 public:
	CommandBSSetFantasy(Module *creator, const Anope::string &sname = "botserv/set/fantasy") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Enable fantaisist commands"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChanServ::Channel *ci = ChanServ::Find(params[0]);
		const Anope::string &value = params[1];

		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), params[0]);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("SET") && !source.HasPriv("botserv/administration"))
		{
			source.Reply(_("Access denied. You do not have the \002{0}\002 privilege on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bot option setting is temporarily disabled."));
			return;
		}

		if (value.equals_ci("ON"))
		{
			logger.Command(source, ci, _("{source} used {command} on {channel} to enable fantasy"));

			ci->SetFantasy(true);
			source.Reply(_("Fantasy mode is now \002on\002 on channel \002{0}\002."), ci->GetName());
		}
		else if (value.equals_ci("OFF"))
		{
			logger.Command(source, ci, _("{source} used {command} on {channel} to disable fantasy"));

			ci->SetFantasy(false);
			source.Reply(_("Fantasy mode is now \002off\002 on channel \002{0}\002."), ci->GetName());
		}
		else
		{
			this->OnSyntaxError(source, source.GetCommand());
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
				"Enables or disables \002fantasy\002 mode on a channel.\n"
				"When it is enabled, users will be able to use\n"
				"fantasy commands on a channel when prefixed\n"
				"with one of the following fantasy characters: \002%s\002\n"
				" \n"
				"Note that users wanting to use fantaisist\n"
				"commands MUST have enough access for both\n"
				"the FANTASIA and the command they are executing."),
				Config->GetModule(this->GetOwner())->Get<Anope::string>("fantasycharacter", "!").c_str());
		return true;
	}
};

class Fantasy : public Module
	, public EventHook<Event::Privmsg>
	, public EventHook<Event::ServiceBotEvent>
{
	CommandBSSetFantasy commandbssetfantasy;

 public:
	Fantasy(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::Privmsg>(this)
		, EventHook<Event::ServiceBotEvent>(this)
		, commandbssetfantasy(this)
	{
	}

	void OnPrivmsg(User *u, Channel *c, Anope::string &msg) override
	{
		if (!u || !c || !c->GetChannel() || !c->GetChannel()->GetBot() || msg.empty() || msg[0] == '\1')
			return;

		ChanServ::Channel *ci = c->GetChannel();

		if (Config->GetClient("BotServ") && !ci->IsFantasy())
			return;

		std::vector<Anope::string> params;
		spacesepstream(msg).GetTokens(params);

		if (params.empty())
			return;

		Anope::string normalized_param0 = Anope::NormalizeBuffer(params[0]);
		Anope::string fantasy_chars = Config->GetModule(this)->Get<Anope::string>("fantasycharacter", "!");

		if (!normalized_param0.find(ci->GetBot()->nick))
		{
			params.erase(params.begin());
		}
		else if (!normalized_param0.find_first_of(fantasy_chars))
		{
			size_t sz = params[0].find_first_of(fantasy_chars);
			if (sz == Anope::string::npos)
				return; /* normalized_param0 is a subset of params[0] so this can't happen */

			params[0].erase(0, sz + 1);
		}
		else
		{
			return;
		}
		
		if (params.empty())
			return;

		CommandInfo::map::const_iterator it = Config->Fantasy.end();
		unsigned count = 0;
		for (unsigned max = params.size(); it == Config->Fantasy.end() && max > 0; --max)
		{
			Anope::string full_command;
			for (unsigned i = 0; i < max; ++i)
				full_command += " " + params[i];
			full_command.erase(full_command.begin());

			++count;
			it = Config->Fantasy.find(Anope::NormalizeBuffer(full_command));
		}

		if (it == Config->Fantasy.end())
			return;

		const CommandInfo &info = it->second;
		ServiceReference<Command> cmd(info.name);
		if (!cmd)
		{
			logger.Debug("Fantasy command {0} exists for non-existent service {1}!", it->first, info.name);
			return;
		}

		for (unsigned i = 0, j = params.size() - (count - 1); i < j; ++i)
			params.erase(params.begin());

		/* Some commands take the channel as a first parameter */
		if (info.prepend_channel)
			params.insert(params.begin(), c->name);

		while (cmd->max_params > 0 && params.size() > cmd->max_params)
		{
			params[cmd->max_params - 1] += " " + params[cmd->max_params];
			params.erase(params.begin() + cmd->max_params);
		}

		// Command requires registered users only
		if (!cmd->AllowUnregistered() && !u->Account())
			return;

		if (params.size() < cmd->min_params)
			return;

		CommandSource source(u->nick, u, u->Account(), u, ci->GetBot());
		source.c = c;
		source.SetCommandInfo(info);

		ChanServ::AccessGroup ag = ci->AccessFor(u);
		bool has_fantasia = ag.HasPriv("FANTASIA") || source.HasPriv("botserv/fantasy");

		EventReturn MOD_RESULT;
		if (has_fantasia)
		{
			MOD_RESULT = EventManager::Get()->Dispatch(&Event::BotFantasy::OnBotFantasy, source, cmd, ci, params);
		}
		else
		{
			MOD_RESULT = EventManager::Get()->Dispatch(&Event::BotNoFantasyAccess::OnBotNoFantasyAccess, source, cmd, ci, params);
		}

		if (MOD_RESULT == EVENT_STOP || !has_fantasia)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !info.permission.empty() && !source.HasCommand(info.permission))
			return;

		MOD_RESULT = EventManager::Get()->Dispatch(&Event::PreCommand::OnPreCommand, source, cmd, params);
		if (MOD_RESULT == EVENT_STOP)
			return;

		Reference<NickServ::Account> nc_reference(u->Account());
		cmd->Execute(source, params);
		if (!nc_reference)
			source.nc = NULL;
		EventManager::Get()->Dispatch(&Event::PostCommand::OnPostCommand, source, cmd, params);
	}

	void OnServiceBot(CommandSource &source, ServiceBot *bi, ChanServ::Channel *ci, InfoFormatter &info) override
	{
		if (ci->IsFantasy())
			info.AddOption(_("Fantasy"));
	}
};

MODULE_INIT(Fantasy)

