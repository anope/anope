/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class CommandNSSet : public Command
{
	std::map<ci::string, Command *> subcommands;
 public:
	CommandNSSet(const ci::string &cname) : Command(cname, 1, 3)
	{
	}

	~CommandNSSet()
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
			notice_lang(Config.s_NickServ, u, NICK_SET_DISABLED);
			return MOD_CONT;
		}

		if (u->Account()->HasFlag(NI_SUSPENDED))
		{
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, u->Account()->display);
			return MOD_CONT;
		}

		Command *c = this->FindCommand(params[0]);

		if (c)
		{
			ci::string cmdparams;
			for (std::vector<ci::string>::const_iterator it = params.begin() + 1; it != params.end(); ++it)
				cmdparams += " " + *it;
			if (!cmdparams.empty())
				cmdparams.erase(cmdparams.begin());
			mod_run_cmd(NickServ, u, c, params[0], cmdparams);
		}
		else
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_UNKNOWN_OPTION, params[0].c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (subcommand.empty())
		{
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_HEAD);
			for (std::map<ci::string, Command *>::iterator it = this->subcommands.begin(); it != this->subcommands.end(); ++it)
				it->second->OnServHelp(u);
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_TAIL);
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

class CommandNSSetDisplay : public Command
{
 public:
	CommandNSSetDisplay(const ci::string &cname) : Command(cname, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickAlias *na = findnick(params[0]);

		if (!na || na->nc != u->Account())
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_DISPLAY_INVALID);
			return MOD_CONT;
		}

		change_core_display(u->Account(), params[0].c_str());
		notice_lang(Config.s_NickServ, u, NICK_SET_DISPLAY_CHANGED, u->Account()->display);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_DISPLAY);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
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
	CommandNSSetPassword(const ci::string &cname) : Command(cname, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string param = params[0];

		int len = param.size();

		if (u->Account()->display == param || (Config.StrictPasswords && len < 5))
		{
			notice_lang(Config.s_NickServ, u, MORE_OBSCURE_PASSWORD);
			return MOD_CONT;
		}
		else if (len > Config.PassLen)
		{
			notice_lang(Config.s_NickServ, u, PASSWORD_TOO_LONG);
			return MOD_CONT;
		}

		std::string buf = param.c_str(); /* conversion from ci::string to std::string */
		if (enc_encrypt(buf, u->Account()->pass) < 0)
		{
			Alog() << Config.s_NickServ << ": Failed to encrypt password for " << u->Account()->display << " (set)";
			notice_lang(Config.s_NickServ, u, NICK_SET_PASSWORD_FAILED);
			return MOD_CONT;
		}

		std::string tmp_pass;
		if (enc_decrypt(u->Account()->pass, tmp_pass) == 1)
			notice_lang(Config.s_NickServ, u, NICK_SET_PASSWORD_CHANGED_TO, tmp_pass.c_str());
		else
			notice_lang(Config.s_NickServ, u, NICK_SET_PASSWORD_CHANGED);

		Alog() << Config.s_NickServ << ": " << u->GetMask() << " (e-mail: " << (u->Account()->email ? u->Account()->email : "none") << ") changed its password.";
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_PASSWORD);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
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
	NSSet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		Command *set = new CommandNSSet("SET");
		this->AddCommand(NickServ, set);
		set->AddSubcommand(new CommandNSSetDisplay("DISPLAY"));
		set->AddSubcommand(new CommandNSSetPassword("PASSWORD"));
	}
};

MODULE_INIT(NSSet)
