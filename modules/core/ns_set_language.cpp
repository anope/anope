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
	CommandNSSetLanguage(const Anope::string &spermission = "") : Command("LANGUAGE", 2, 2, spermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *na = findnick(params[0]);
		if (!na)
			throw CoreException("NULL na in CommandNSSetLanguage");
		NickCore *nc = na->nc;

		Anope::string param = params[1];

		for (unsigned j = 0; j < languages.size(); ++j)
		{
			if (param == "en" || languages[j] == param)
				break;
			else if (j + 1 == languages.size())
			{
				this->OnSyntaxError(u, "");
				return MOD_CONT;
			}
		}

		nc->language = param != "en" ? param : "";
		u->SendMessage(NickServ, NICK_SET_LANGUAGE_CHANGED);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(NickServ, NICK_HELP_SET_LANGUAGE);
		u->SendMessage(Config->s_NickServ, "         en (English)");
		for (unsigned j = 0; j < languages.size(); ++j)
		{
			const Anope::string &langname = GetString(languages[j], LANGUAGE_NAME);
			if (langname == "English")
				continue;
			u->SendMessage("         %s (%s)", languages[j].c_str(), langname.c_str());
		}

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		SyntaxError(NickServ, u, "SET LANGUAGE", NICK_SET_LANGUAGE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_SET_LANGUAGE);
	}
};

class CommandNSSASetLanguage : public CommandNSSetLanguage
{
 public:
	CommandNSSASetLanguage() : CommandNSSetLanguage("nickserv/saset/language")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		u->SendMessage(NickServ, NICK_HELP_SASET_LANGUAGE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		SyntaxError(NickServ, u, "SASET LANGUAGE", NICK_SASET_LANGUAGE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_SASET_LANGUAGE);
	}
};

class NSSetLanguage : public Module
{
	CommandNSSetLanguage commandnssetlanguage;
	CommandNSSASetLanguage commandnssasetlanguage;

 public:
	NSSetLanguage(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(&commandnssetlanguage);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(&commandnssasetlanguage);
	}

	~NSSetLanguage()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand(&commandnssetlanguage);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand(&commandnssasetlanguage);
	}
};

MODULE_INIT(NSSetLanguage)
