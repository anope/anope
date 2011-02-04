/* ChanServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSSASet : public Command
{
	typedef std::map<Anope::string, Command *, std::less<ci::string> > subcommand_map;
	subcommand_map subcommands;

 public:
	CommandCSSASet() : Command("SASET", 2, 3)
	{
	}

	~CommandCSSASet()
	{
		this->subcommands.clear();
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		if (readonly)
		{
			source.Reply(LanguageString::CHAN_SET_DISABLED);
			return MOD_CONT;
		}

		// XXX Remove after 1.9.4 release
		if (params[1].equals_ci("MLOCK"))
		{
			source.Reply(LanguageString::CHAN_SET_MLOCK_DEPRECATED);
			return MOD_CONT;
		}

		Command *c = this->FindCommand(params[1]);

		if (c)
		{
			ChannelInfo *ci = source.ci;
			Anope::string cmdparams = ci->name;
			for (std::vector<Anope::string>::const_iterator it = params.begin() + 2, it_end = params.end(); it != it_end; ++it)
				cmdparams += " " + *it;
			Log(LOG_ADMIN, u, this, ci) << params[1] << " " << cmdparams;
			mod_run_cmd(ChanServ, u, c, params[1], cmdparams, false);
		}
		else
		{
			source.Reply(_("Unknown SASET option \002%s\002."), params[1].c_str());
			source.Reply(LanguageString::MORE_INFO, Config->s_ChanServ.c_str(), "SET");
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.empty())
		{
			source.Reply(_("Syntax: SASET \002channel\002 \037option\037 \037parameters\037\n"
					" \n"
					"Allows Services Operators to forcefully change settings\n"
					"on channels.\n"
					" \n"
					"Available options:"));
			for (subcommand_map::iterator it = this->subcommands.begin(), it_end = this->subcommands.end(); it != it_end; ++it)
				it->second->OnServHelp(source);
			source.Reply(_("Type \002%R%S HELP SASET \037option\037\002 for more information on a\n"
				"particular option."));
			return true;
		}
		else
		{
			Command *c = this->FindCommand(subcommand);

			if (c)
				return c->OnHelp(source, subcommand);
		}

		return false;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SASET", LanguageString::CHAN_SASET_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    SASET      Forcefully set channel options and information"));
	}

	bool AddSubcommand(Module *creator, Command *c)
	{
		c->module = creator;
		c->service = this->service;
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

class CSSASet : public Module
{
	CommandCSSASet commandcssaset;

 public:
	CSSASet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcssaset);
	}
};

MODULE_INIT(CSSASet)
