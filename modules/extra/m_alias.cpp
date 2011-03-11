/*
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

struct CommandAlias
{
	bool fantasy;
	bool operonly;
	Anope::string source_client;
	Anope::string source_command;
	Anope::string target_client;
	Anope::string target_command;
};

class ModuleAlias : public Module
{
	std::multimap<Anope::string, CommandAlias, std::less<ci::string> > aliases;
 public:
	ModuleAlias(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		Implementation i[] = { I_OnReload, I_OnPreCommandRun, I_OnBotFantasy };
		ModuleManager::Attach(i, this, 3);

		OnReload(false);
	}

	void OnReload(bool)
	{
		ConfigReader config;

		this->aliases.clear();

		for (int i = 0; i < config.Enumerate("alias"); ++i)
		{
			bool fantasy = config.ReadFlag("alias", "fantasy", "no", i);
			bool operonly = config.ReadFlag("alias", "operonly", "no", i);
			Anope::string source_client = config.ReadValue("alias", "source_client", "", i);
			Anope::string source_command = config.ReadValue("alias", "source_command", "", i);
			Anope::string target_client = config.ReadValue("alias", "target_client", "", i);
			Anope::string target_command = config.ReadValue("alias", "target_command", "", i);
			
			if ((!fantasy && source_client.empty()) || source_command.empty() || target_client.empty() || target_command.empty())
				continue;

			CommandAlias alias;
			alias.fantasy = fantasy;
			alias.operonly = operonly;
			alias.source_client = source_client;
			alias.source_command = source_command;
			alias.target_client = target_client;
			alias.target_command = target_command;
			
			this->aliases.insert(std::make_pair(source_command, alias));
		}
	}

	EventReturn OnPreCommandRun(User *&u, BotInfo *&bi, Anope::string &command, Anope::string &message, ChannelInfo *&ci)
	{
		bool fantasy = ci != NULL;
		std::multimap<Anope::string, CommandAlias, std::less<ci::string> >::const_iterator it = aliases.find(command), it_end = it;
		if (it_end != aliases.end())
			it_end = aliases.upper_bound(command);
		for (; it != it_end; ++it)
		{
			const CommandAlias &alias = it->second;
			
			if (!u->HasMode(UMODE_OPER) && alias.operonly)
				continue;
			else if (fantasy != alias.fantasy)
				continue;
			else if (fantasy && alias.fantasy) // OnBotFantasy gets this!
				return EVENT_STOP;
			else if (!bi->nick.equals_ci(alias.source_client))
				continue;

			BotInfo *target = findbot(alias.target_client);
			if (target)
				bi = target;
			command = alias.target_command;
			break;
		}

		return EVENT_CONTINUE;
	}

	void OnBotFantasy(const Anope::string &command, User *u, ChannelInfo *ci, const Anope::string &params)
	{
		std::multimap<Anope::string, CommandAlias, std::less<ci::string> >::const_iterator it = aliases.find(command), it_end = it;
		if (it_end != aliases.end())
			it_end = aliases.upper_bound(command);
		for (; it != it_end; ++it)
		{
			const CommandAlias &alias = it->second;

			if (!u->HasMode(UMODE_OPER) && alias.operonly)
				continue;

			BotInfo *target = findbot(alias.target_client);
			if (!target)
				target = ChanServ;

			Anope::string full_message = alias.target_command;
			if (target == ChanServ || target == BotServ)
			{
				Command *target_c = FindCommand(target, alias.target_command);
				if (target_c && !target_c->HasFlag(CFLAG_STRIP_CHANNEL))
					full_message += " " + ci->name;
			}
			if (!params.empty())
				full_message += + " " + params;

			mod_run_cmd(target, u, ci, full_message);
			break;
		}
	}
};

MODULE_INIT(ModuleAlias)
