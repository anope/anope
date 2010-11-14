/*
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

struct CommandAlias
{
	bool fantasy;
	bool operonly;
	Anope::string client;
	Anope::string alias;
	Anope::string command;
};

class ModuleAlias : public Module
{
	std::map<Anope::string, CommandAlias, std::less<ci::string> > aliases;
 public:
	ModuleAlias(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		Implementation i[] = { I_OnReload, I_OnPreCommandRun };
		ModuleManager::Attach(i, this, 2);

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
			Anope::string client = config.ReadValue("alias", "client", "", i);
			Anope::string aliasname = config.ReadValue("alias", "alias", "", i);
			Anope::string command = config.ReadValue("alias", "command", "", i);
			
			if (aliasname.empty() || command.empty())
				continue;

			CommandAlias alias;
			alias.fantasy = fantasy;
			alias.operonly = operonly;
			alias.client = client;
			alias.alias = aliasname;
			alias.command = command;
			
			this->aliases.insert(std::make_pair(aliasname, alias));
		}
	}

	EventReturn OnPreCommandRun(User *u, BotInfo *bi, Anope::string &command, Anope::string &message, bool fantasy)
	{
		std::map<Anope::string, CommandAlias, std::less<ci::string> >::const_iterator it = aliases.find(command);
		if (it != aliases.end())
		{
			const CommandAlias &alias = it->second;

			if (fantasy != alias.fantasy)
				return EVENT_CONTINUE;
			else if (!is_oper(u) && alias.operonly)
				return EVENT_CONTINUE;
			else if (!fantasy && !bi->nick.equals_ci(alias.client))
				return EVENT_CONTINUE;

			command = alias.command;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ModuleAlias)
