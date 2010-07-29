/* NickServ core functions
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

class CommandNSSet : public Command
{
	typedef std::map<Anope::string, Command *, std::less<ci::string> > subcommand_map;
	subcommand_map subcommands;

 public:
	CommandNSSet(const Anope::string &cname) : Command(cname, 1, 3)
	{
	}

	~CommandNSSet()
	{
		for (subcommand_map::const_iterator it = this->subcommands.begin(), it_end = this->subcommands.end(); it != it_end; ++it)
			delete it->second;
		this->subcommands.clear();
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		if (readonly)
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_DISABLED);
			return MOD_CONT;
		}

		if (u->Account()->HasFlag(NI_SUSPENDED))
		{
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, u->Account()->display.c_str());
			return MOD_CONT;
		}

		Command *c = this->FindCommand(params[0]);

		if (c)
		{
			Anope::string cmdparams;
			for (std::vector<Anope::string>::const_iterator it = params.begin() + 1, it_end = params.end(); it != it_end; ++it)
				cmdparams += " " + *it;
			if (!cmdparams.empty())
				cmdparams.erase(cmdparams.begin());
			mod_run_cmd(NickServ, u, c, params[0], cmdparams);
		}
		else
			notice_lang(Config.s_NickServ, u, NICK_SET_UNKNOWN_OPTION, params[0].c_str());

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (subcommand.empty())
		{
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_HEAD);
			for (subcommand_map::iterator it = this->subcommands.begin(), it_end = this->subcommands.end(); it != it_end; ++it)
				it->second->OnServHelp(u);
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_TAIL);
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
		syntax_error(Config.s_NickServ, u, "SET", NICK_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET);
	}

	bool AddSubcommand(Command *c)
	{
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
	CommandNSSetDisplay(const Anope::string &cname) : Command(cname, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickAlias *na = findnick(params[0]);

		if (!na || na->nc != u->Account())
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_DISPLAY_INVALID);
			return MOD_CONT;
		}

		change_core_display(u->Account(), params[0]);
		notice_lang(Config.s_NickServ, u, NICK_SET_DISPLAY_CHANGED, u->Account()->display.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_DISPLAY);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		// XXX
		syntax_error(Config.s_NickServ, u, "SET", NICK_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET_DISPLAY);
	}
};

class CommandNSSetPassword : public Command
{
 public:
	CommandNSSetPassword(const Anope::string &cname) : Command(cname, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string param = params[0];

		int len = param.length();

		if (u->Account()->display.equals_ci(param) || (Config.StrictPasswords && len < 5))
		{
			notice_lang(Config.s_NickServ, u, MORE_OBSCURE_PASSWORD);
			return MOD_CONT;
		}
		else if (len > Config.PassLen)
		{
			notice_lang(Config.s_NickServ, u, PASSWORD_TOO_LONG);
			return MOD_CONT;
		}

		if (enc_encrypt(param, u->Account()->pass) < 0)
		{
			Alog() << Config.s_NickServ << ": Failed to encrypt password for " << u->Account()->display << " (set)";
			notice_lang(Config.s_NickServ, u, NICK_SET_PASSWORD_FAILED);
			return MOD_CONT;
		}

		Anope::string tmp_pass;
		if (enc_decrypt(u->Account()->pass, tmp_pass) == 1)
			notice_lang(Config.s_NickServ, u, NICK_SET_PASSWORD_CHANGED_TO, tmp_pass.c_str());
		else
			notice_lang(Config.s_NickServ, u, NICK_SET_PASSWORD_CHANGED);

		Alog() << Config.s_NickServ << ": " << u->GetMask() << " (e-mail: " << (!u->Account()->email.empty() ? u->Account()->email : "none") << ") changed its password.";
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_PASSWORD);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		// XXX
		syntax_error(Config.s_NickServ, u, "SET", NICK_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET_PASSWORD);
	}
};

class NSSet : public Module
{
 public:
	NSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *set = new CommandNSSet("SET");
		this->AddCommand(NickServ, set);
		set->AddSubcommand(new CommandNSSetDisplay("DISPLAY"));
		set->AddSubcommand(new CommandNSSetPassword("PASSWORD"));
	}
};

MODULE_INIT(NSSet)
