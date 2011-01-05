/* NickServ core functions
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

class CommandNSSet : public Command
{
	typedef std::map<Anope::string, Command *, std::less<ci::string> > subcommand_map;
	subcommand_map subcommands;

 public:
	CommandNSSet() : Command("SET", 1, 3)
	{
	}

	~CommandNSSet()
	{
		this->subcommands.clear();
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		if (readonly)
		{
			source.Reply(NICK_SET_DISABLED);
			return MOD_CONT;
		}

		if (u->Account()->HasFlag(NI_SUSPENDED))
		{
			source.Reply(NICK_X_SUSPENDED, u->Account()->display.c_str());
			return MOD_CONT;
		}

		Command *c = this->FindCommand(params[0]);

		if (c)
		{
			Anope::string cmdparams = u->Account()->display;
			for (std::vector<Anope::string>::const_iterator it = params.begin() + 1, it_end = params.end(); it != it_end; ++it)
				cmdparams += " " + *it;
			/* Don't log the whole message for set password */
			if (c->name != "PASSWORD")
				Log(LOG_COMMAND, u, this) << params[0] << " " << cmdparams;
			else
				Log(LOG_COMMAND, u, this) << params[0];
			mod_run_cmd(NickServ, u, c, params[0], cmdparams, false);
		}
		else
			source.Reply(NICK_SET_UNKNOWN_OPTION, params[0].c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (subcommand.empty())
		{
			source.Reply(NICK_HELP_SET_HEAD);
			for (subcommand_map::iterator it = this->subcommands.begin(), it_end = this->subcommands.end(); it != it_end; ++it)
				it->second->OnServHelp(source);
			source.Reply(NICK_HELP_SET_TAIL);
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
		SyntaxError(source, "SET", NICK_SET_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_SET);
	}

	bool AddSubcommand(Module *creator, Command *c)
	{
		c->module = creator;
		c->service = this->service;
		return this->subcommands.insert(std::make_pair(c->name, c)).second;
	}

	bool DelSubcommand(const Anope::string &command)
	{
		return this->subcommands.erase(command);
	}

	Command *FindCommand(const Anope::string &subcommand)
	{
		subcommand_map::const_iterator it = this->subcommands.find(subcommand);

		if (it != this->subcommands.end())
			return it->second;

		return NULL;
	}
};

class CommandNSSetDisplay : public Command
{
 public:
	CommandNSSetDisplay() : Command("DISPLAY", 1)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *na = findnick(params[1]);

		if (!na || na->nc != u->Account())
		{
			source.Reply(NICK_SASET_DISPLAY_INVALID, u->Account()->display.c_str());
			return MOD_CONT;
		}

		change_core_display(u->Account(), params[1]);
		source.Reply(NICK_SET_DISPLAY_CHANGED, u->Account()->display.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(NICK_HELP_SET_DISPLAY);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SET", NICK_SET_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_SET_DISPLAY);
	}
};

class CommandNSSetPassword : public Command
{
 public:
	CommandNSSetPassword() : Command("PASSWORD", 1)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &param = params[1];
		unsigned len = param.length();

		if (u->Account()->display.equals_ci(param) || (Config->StrictPasswords && len < 5))
		{
			source.Reply(MORE_OBSCURE_PASSWORD);
			return MOD_CONT;
		}
		else if (len > Config->PassLen)
		{
			source.Reply(PASSWORD_TOO_LONG);
			return MOD_CONT;
		}

		if (enc_encrypt(param, u->Account()->pass) < 0)
		{
			Log(NickServ) << "Failed to encrypt password for " << u->Account()->display << " (set)";
			source.Reply(NICK_SASET_PASSWORD_FAILED);
			return MOD_CONT;
		}

		Anope::string tmp_pass;
		if (enc_decrypt(u->Account()->pass, tmp_pass) == 1)
			source.Reply(NICK_SASET_PASSWORD_CHANGED_TO, u->Account()->display.c_str(), tmp_pass.c_str());
		else
			source.Reply(NICK_SASET_PASSWORD_CHANGED, u->Account()->display.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(NICK_HELP_SET_PASSWORD);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SET", NICK_SET_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_SET_PASSWORD);
	}
};

class NSSet : public Module
{
	CommandNSSet commandnsset;
	CommandNSSetDisplay commandnssetdisplay;
	CommandNSSetPassword commandnssetpassword;

 public:
	NSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsset);

		commandnsset.AddSubcommand(this, &commandnssetdisplay);
		commandnsset.AddSubcommand(this, &commandnssetpassword);
	}
};

MODULE_INIT(NSSet)
