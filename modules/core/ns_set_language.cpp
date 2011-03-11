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

class CommandNSSetLanguage : public Command
{
 public:
	CommandNSSetLanguage(const Anope::string &spermission = "") : Command("LANGUAGE", 2, 2, spermission)
	{
		this->SetDesc(_("Set the language Services will use when messaging you"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
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
				this->OnSyntaxError(source, "");
				return MOD_CONT;
			}
		}

		nc->language = param != "en" ? param : "";
		PopLanguage();
		PushLanguage("anope", nc->language);
		source.Reply(_("Language changed to \002English\002."));

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET LANGUAGE \037language\037\002\n"
				" \n"
				"Changes the language Services uses when sending messages to\n"
				"you (for example, when responding to a command you send).\n"
				"\037language\037 should be chosen from the following list of\n"
				"supported languages:"));

		source.Reply("         en (English)");
		for (unsigned j = 0; j < languages.size(); ++j)
		{
			PushLanguage("anope", languages[j]);
			const Anope::string &langname = _("English");
			PopLanguage();
			if (langname == "English")
				continue;
			source.Reply("         %s (%s)", languages[j].c_str(), langname.c_str());
		}

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET LANGUAGE", _("SET LANGUAGE \037language\037"));
	}
};

class CommandNSSASetLanguage : public CommandNSSetLanguage
{
 public:
	CommandNSSASetLanguage() : CommandNSSetLanguage("nickserv/saset/language")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET LANGUAGE \037language\037\002\n"
				" \n"
				"Changes the language Services uses when sending messages to\n"
				"you (for example, when responding to a command you send).\n"
				"\037language\037 should be chosen from the following list of\n"
				"supported languages:"));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SASET LANGUAGE", _("SASET \037nickname\037 LANGUAGE \037number\037"));
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
			c->AddSubcommand(this, &commandnssetlanguage);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandnssasetlanguage);
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
