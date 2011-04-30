/*
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "chanserv.h"

struct CommandAlias
{
	bool fantasy;
	bool operonly;
	Anope::string source_client;
	Anope::string source_command;
	Anope::string source_desc;
	Anope::string target_client;
	Anope::string target_command;
	Anope::string target_rewrite;
};

class AliasCommand : public Command
{
	CommandAlias alias;
	dynamic_reference<Command> target_command_c;
 public:
	AliasCommand(CommandAlias &a) : Command(a.source_command, 0, 0), alias(a), target_command_c(NULL) { }
	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params) { return MOD_CONT; }

	void OnServHelp(CommandSource &source)
	{
		Anope::string cdesc = this->alias.source_desc;
		if (!this->target_command_c)
			this->target_command_c = FindCommand(findbot(this->alias.target_client), this->alias.target_command);
		if (this->target_command_c && cdesc.empty())
			cdesc = this->target_command_c->GetDesc();
		if (this->target_command_c)
			source.Reply("    %-14s %s", this->name.c_str(), _(cdesc.c_str()));
	}
	
	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (!this->alias.target_rewrite.empty())
			return false;

		if (!this->target_command_c)
			this->target_command_c = FindCommand(findbot(this->alias.target_client), this->alias.target_command);
		if (!this->target_command_c)
			return false;

		mod_help_cmd(this->target_command_c->service, source.u, source.ci, this->target_command_c->name);
		return true;
	}
};

class ModuleAlias : public Module
{
	std::multimap<Anope::string, CommandAlias, std::less<ci::string> > aliases;
	std::vector<AliasCommand *> commands;

	Anope::string RewriteCommand(Anope::string &message, const Anope::string &rewrite)
	{
		if (rewrite.empty())
			return message;

		std::vector<Anope::string> tokens = BuildStringVector(message);
		spacesepstream sep(rewrite);
		Anope::string token, final_message;
		while (sep.GetToken(token))
		{
			if (token[0] == '$' && token.length() > 1)
			{
				Anope::string number = token.substr(1);
				bool all = false;
				if (number[number.length() - 1] == '-')
				{
					number.erase(number.length() - 1);
					all = true;
				}
				if (number.empty())
					continue;

				int index;
				try
				{
					index = convertTo<int>(number);
				}
				catch (const ConvertException &ex)
				{
					continue;
				}

				if (index < 0 || static_cast<unsigned>(index) >= tokens.size())
					continue;

				final_message += tokens[index] + " ";
				if (all)
					for (unsigned i = index + i; i < tokens.size(); ++i)
						final_message += tokens[i] + " ";
			}
			else
				final_message += token + " ";
		}

		final_message.trim();
		return final_message;
	}

 public:
	ModuleAlias(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnPreCommandRun, I_OnBotFantasy };
		ModuleManager::Attach(i, this, 3);

		OnReload();
	}

	void OnReload()
	{
		ConfigReader config;

		this->aliases.clear();
		for (unsigned i = 0; i < this->commands.size(); ++i)
			delete this->commands[i];
		this->commands.clear();

		for (int i = 0; i < config.Enumerate("alias"); ++i)
		{
			bool fantasy = config.ReadFlag("alias", "fantasy", "no", i);
			bool operonly = config.ReadFlag("alias", "operonly", "no", i);
			bool hide = config.ReadFlag("alias", "hide", "no", i);
			Anope::string source_client = config.ReadValue("alias", "source_client", "", i);
			Anope::string source_command = config.ReadValue("alias", "source_command", "", i);
			Anope::string source_desc = config.ReadValue("alias", "source_desc", "", i);
			Anope::string target_client = config.ReadValue("alias", "target_client", "", i);
			Anope::string target_command = config.ReadValue("alias", "target_command", "", i);
			Anope::string target_rewrite = config.ReadValue("alias", "target_rewrite", "", i);
			
			if ((!fantasy && source_client.empty()) || source_command.empty() || target_client.empty() || target_command.empty())
				continue;

			CommandAlias alias;
			alias.fantasy = fantasy;
			alias.operonly = operonly;
			alias.source_client = source_client;
			alias.source_command = source_command;
			alias.source_desc = source_desc;
			alias.target_client = target_client;
			alias.target_command = target_command;
			alias.target_rewrite = target_rewrite;
			
			this->aliases.insert(std::make_pair(source_command, alias));
			if (hide == false)
			{
				AliasCommand *cmd = new AliasCommand(alias);
				this->commands.push_back(cmd);
				BotInfo *bi = findbot(alias.source_client);
				if (bi != NULL)
					this->AddCommand(bi, cmd);
			}
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
				continue;
			else if (!bi->nick.equals_ci(alias.source_client))
				continue;

			BotInfo *target = findbot(alias.target_client);
			if (target)
				bi = target;
			command = alias.target_command;
			message = this->RewriteCommand(message, alias.target_rewrite);
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
				target = chanserv->Bot();

			Anope::string full_message = alias.target_command;
			if (target == chanserv->Bot() || target->nick == Config->s_BotServ)
			{
				Command *target_c = FindCommand(target, alias.target_command);
				if (target_c && !target_c->HasFlag(CFLAG_STRIP_CHANNEL))
					full_message += " " + ci->name;
			}
			if (!params.empty())
				full_message += + " " + params;

			full_message = this->RewriteCommand(full_message, alias.target_rewrite);
			mod_run_cmd(target, u, ci, full_message);
			break;
		}
	}
};

MODULE_INIT(ModuleAlias)
