// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

class CommandBSSetFantasy final
	: public Command
{
public:
	CommandBSSetFantasy(Module *creator, const Anope::string &sname = "botserv/set/fantasy") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Enable fantasy commands"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		const Anope::string &value = params[1];

		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!source.HasPriv("botserv/administration") && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		if (value.equals_ci("ON"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enable fantasy";

			ci->Extend<bool>("BS_FANTASY");
			source.Reply(_("Fantasy mode is now \002on\002 on channel %s."), ci->name.c_str());
		}
		else if (value.equals_ci("OFF"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable fantasy";

			ci->Shrink<bool>("BS_FANTASY");
			source.Reply(_("Fantasy mode is now \002off\002 on channel %s."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Enables or disables \002fantasy\002 mode on a channel. "
				"When it is enabled, users will be able to use "
				"fantasy commands on a channel when prefixed "
				"with one of the following fantasy prefixes: \002%s\002"
				"\n\n"
				"Note that users wanting to use fantasy commands "
				"MUST have enough access for both the FANTASY "
				"privilege and the command they are executing."
			),
			Config->GetModule(this->owner).Get<const Anope::string>("prefix", "!").c_str());
		return true;
	}
};

class Fantasy final
	: public Module
{
private:
	SerializableExtensibleItem<bool> fantasy;
	CommandBSSetFantasy commandbssetfantasy;
	std::vector<Anope::string> prefixes;

public:
	Fantasy(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, fantasy(this, "BS_FANTASY")
		, commandbssetfantasy(this)
	{
	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &modconf = conf.GetModule(this);
		const auto &prefix = modconf.Get<const Anope::string>("prefix", "!");
		spacesepstream(prefix).GetTokens(prefixes);
	}

	void OnPrivmsg(User *u, Channel *c, Anope::string &msg, const Anope::map<Anope::string> &tags) override
	{
		if (!u || !c || !c->ci || !c->ci->bi || msg.empty() || msg[0] == '\1')
			return;

		if (Config->GetClient("BotServ") && !fantasy.HasExt(c->ci))
			return;

		std::vector<Anope::string> params;
		spacesepstream(msg).GetTokens(params);

		if (params.empty())
			return;

		Anope::string normalized_param0 = Anope::RemoveFormatting(params[0]);
		if (normalized_param0.find_ci(c->ci->bi->nick) == 0)
		{
			params.erase(params.begin());
		}
		else
		{
			auto fn = [&normalized_param0](const auto &prefix) { return normalized_param0.find(prefix) == 0; };
			auto it = std::find_if(prefixes.begin(), prefixes.end(), fn);
			if (it == prefixes.end())
				return;

			auto sz = params[0].find(*it);
			if (sz == Anope::string::npos)
				return; /* normalized_param0 is a subset of params[0] so this can't happen */

			params[0].erase(0, sz + it->length());
			if (params[0].equals_ci(c->ci->bi->alias))
				params.erase(params.begin());
		}

		if (params.empty())
			return;

		auto it = Config->Fantasy.end();
		unsigned count = 0;
		for (unsigned max = params.size(); it == Config->Fantasy.end() && max > 0; --max)
		{
			Anope::string full_command;
			for (unsigned i = 0; i < max; ++i)
				full_command += " " + params[i];
			full_command.erase(full_command.begin());

			++count;
			it = Config->Fantasy.find(Anope::RemoveFormatting(full_command));
		}

		if (it == Config->Fantasy.end())
			return;

		const CommandInfo &info = it->second;
		ServiceReference<Command> cmd("Command", info.name);
		if (!cmd)
		{
			Log(LOG_DEBUG) << "Fantasy command " << it->first << " exists for nonexistent service " << info.name << "!";
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
		if (!cmd->AllowUnregistered() && !u->IsIdentified())
			return;

		if (params.size() < cmd->min_params)
			return;

		Anope::string msgid;
		auto iter = tags.find("msgid");
		if (iter != tags.end())
			msgid = iter->second;

		CommandSource source(u->nick, u, u->Account(), u, c->ci->bi, msgid);
		source.c = c;
		source.command = it->first;
		source.permission = info.permission;

		AccessGroup ag = c->ci->AccessFor(u);
		bool has_fantasy = !info.require_privilege || ag.HasPriv("FANTASY") || source.HasPriv("botserv/fantasy");

		EventReturn MOD_RESULT;
		if (has_fantasy)
		{
			FOREACH_RESULT(OnBotFantasy, MOD_RESULT, (source, cmd, c->ci, params));
		}
		else
		{
			FOREACH_RESULT(OnBotNoFantasyAccess, MOD_RESULT, (source, cmd, c->ci, params));
		}

		if (MOD_RESULT == EVENT_STOP || !has_fantasy)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !info.permission.empty() && !source.HasCommand(info.permission))
			return;

		FOREACH_RESULT(OnPreCommand, MOD_RESULT, (source, cmd, params));
		if (MOD_RESULT == EVENT_STOP)
			return;

		Reference<NickCore> nc_reference(u->Account());
		cmd->Execute(source, params);
		if (!nc_reference)
			source.nc = NULL;
		FOREACH_MOD(OnPostCommand, (source, cmd, params));
	}

	void OnBotInfo(CommandSource &source, BotInfo *bi, ChannelInfo *ci, InfoFormatter &info) override
	{
		if (fantasy.HasExt(ci))
			info.AddOption(_("Fantasy"));
	}
};

MODULE_INIT(Fantasy)
