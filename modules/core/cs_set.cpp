/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSSet : public Command
{
	typedef std::map<Anope::string, Command *, std::less<ci::string> > subcommand_map;
	subcommand_map subcommands;

 public:
	CommandCSSet() : Command("SET", 2, 3)
	{
	}

	~CommandCSSet()
	{
		this->subcommands.clear();
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		if (readonly)
		{
			notice_lang(Config->s_ChanServ, u, CHAN_SET_DISABLED);
			return MOD_CONT;
		}
		if (!check_access(u, cs_findchan(params[0]), CA_SET))
		{
			notice_lang(Config->s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		Command *c = this->FindCommand(params[1]);

		if (c)
		{
			ChannelInfo *ci = cs_findchan(params[0]);
			Anope::string cmdparams = ci->name;
			for (std::vector<Anope::string>::const_iterator it = params.begin() + 2, it_end = params.end(); it != it_end; ++it)
				cmdparams += " " + *it;
			mod_run_cmd(ChanServ, u, c, params[1], cmdparams);
		}
		else
		{
			notice_lang(Config->s_ChanServ, u, CHAN_SET_UNKNOWN_OPTION, params[1].c_str());
			notice_lang(Config->s_ChanServ, u, MORE_INFO, Config->s_ChanServ.c_str(), "SET");
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (subcommand.empty())
		{
			notice_help(Config->s_ChanServ, u, CHAN_HELP_SET_HEAD);
			for (subcommand_map::iterator it = this->subcommands.begin(), it_end = this->subcommands.end(); it != it_end; ++it)
				it->second->OnServHelp(u);
			notice_help(Config->s_ChanServ, u, CHAN_HELP_SET_TAIL);
			return true;
		}
		else
		{
			Command *c = this->FindCommand(subcommand);

			if (c)
				return c->OnHelp(u, subcommand);
		}

		return false;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_ChanServ, u, CHAN_HELP_CMD_SET);
	}

	bool AddSubcommand(Command *c)
	{
		return this->subcommands.insert(std::make_pair(c->name, c)).second;
	}

	bool DelSubcommand(Command *c)
	{
		return this->subcommands.erase(c->name);
	}

	Command *FindCommand(const Anope::string &subcommand)
	{
		subcommand_map::const_iterator it = this->subcommands.find(subcommand);

		if (it != this->subcommands.end())
			return it->second;

		return NULL;
	}
};

class CSSet : public Module
{
	CommandCSSet commandcsset;

 public:
	CSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsset);
	}
};

MODULE_INIT(CSSet)
