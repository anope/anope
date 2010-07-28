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

class CommandNSSetLanguage : public Command
{
 public:
	CommandNSSetLanguage(const Anope::string &cname) : Command(cname, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string param = params[0];

		if (param.find_first_not_of("0123456789") != Anope::string::npos) /* i.e. not a number */
		{
			this->OnSyntaxError(u, "");
			return MOD_CONT;
		}

		int langnum = convertTo<int>(param) - 1;
		if (langnum < 0 || langnum >= NUM_LANGS || langlist[langnum] < 0)
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_LANGUAGE_UNKNOWN, langnum + 1, Config.s_NickServ.c_str());
			return MOD_CONT;
		}

		u->Account()->language = langlist[langnum];
		notice_lang(Config.s_NickServ, u, NICK_SET_LANGUAGE_CHANGED);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_LANGUAGE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_NickServ, u, "SET LANGUAGE", NICK_SET_LANGUAGE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET_LANGUAGE);
	}
};

class CommandNSSASetLanguage : public Command
{
 public:
	CommandNSSASetLanguage(const Anope::string &cname) : Command(cname, 2, 2, "nickserv/saset/language")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		if (!nc)
			throw CoreException("NULL nc in CommandNSSASetLanguage");

		Anope::string param = params[1];

		if (param.find_first_not_of("0123456789") != Anope::string::npos) /* i.e. not a number */
		{
			this->OnSyntaxError(u, "LANGUAGE");
			return MOD_CONT;
		}
		int langnum = convertTo<int>(param) - 1;
		if (langnum < 0 || langnum >= NUM_LANGS || langlist[langnum] < 0)
		{
			notice_lang(Config.s_NickServ, u, NICK_SASET_LANGUAGE_UNKNOWN, langnum + 1, Config.s_NickServ.c_str());
			return MOD_CONT;
		}
		nc->language = langlist[langnum];
		notice_lang(Config.s_NickServ, u, NICK_SASET_LANGUAGE_CHANGED);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_LANGUAGE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_NickServ, u, "SASET LANGUAGE", NICK_SASET_LANGUAGE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET_LANGUAGE);
	}
};

class NSSetLanguage : public Module
{
 public:
	NSSetLanguage(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(new CommandNSSetLanguage("LANGUAGE"));

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandNSSASetLanguage("LANGUAGE"));
	}

	~NSSetLanguage()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand("LANGUAGE");

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand("LANGUAGE");
	}
};

MODULE_INIT(NSSetLanguage)
