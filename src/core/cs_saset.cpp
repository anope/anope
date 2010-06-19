/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandCSSASet : public Command
{
	std::map<ci::string, Command *> subcommands;

 public:
	CommandCSSASet(const ci::string &cname) : Command(cname, 2, 3)
	{
	}

	~CommandCSSASet()
	{
		for (std::map<ci::string, Command *>::const_iterator it = this->subcommands.begin(); it != this->subcommands.end(); ++it)
		{
			delete it->second;
		}
		this->subcommands.clear();
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		if (readonly)
		{
			notice_lang(Config.s_ChanServ, u, CHAN_SET_DISABLED);
			return MOD_CONT;
		}

		Command *c = this->FindCommand(params[1]);

		if (c)
		{
			ci::string cmdparams = cs_findchan(params[0])->name.c_str();
			for (std::vector<ci::string>::const_iterator it = params.begin() + 2; it != params.end(); ++it)
				cmdparams += " " + *it;
			mod_run_cmd(ChanServ, u, c, params[1], cmdparams);
		}
		else
		{
			notice_lang(Config.s_ChanServ, u, CHAN_SET_UNKNOWN_OPTION, params[1].c_str());
			notice_lang(Config.s_ChanServ, u, MORE_INFO, Config.s_ChanServ, "SET");
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (subcommand.empty())
		{
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SASET_HEAD);
			for (std::map<ci::string, Command *>::iterator it = this->subcommands.begin(); it != this->subcommands.end(); ++it)
				it->second->OnServHelp(u);
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_TAIL);
			return true;
		}
		else
		{
			Command *c = this->FindCommand(subcommand);

			if (c)
			{
				return c->OnHelp(u, subcommand);
			}
		}

		return false;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "SASET", CHAN_SASET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SASET);
	}

	bool AddSubcommand(Command *c)
	{
		return this->subcommands.insert(std::make_pair(c->name, c)).second;
	}

	bool DelSubcommand(const ci::string &command)
	{
		return this->subcommands.erase(command);
	}

	Command *FindCommand(const ci::string &subcommand)
	{
		std::map<ci::string, Command *>::const_iterator it = this->subcommands.find(subcommand);

		if (it != this->subcommands.end())
		{
			return it->second;
		}

		return NULL;
	}
};

class CSSASet : public Module
{
 public:
	CSSASet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(ChanServ, new CommandCSSASet("SASET"));
	}
};

MODULE_INIT(CSSASet)
