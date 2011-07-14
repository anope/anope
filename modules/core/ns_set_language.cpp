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
	CommandNSSetLanguage(Module *creator, const Anope::string &sname = "nickserv/set/language", size_t min = 1, const Anope::string &spermission = "") : Command(creator, sname, min, min + 1, spermission)
	{
		this->SetDesc(_("Set the language Services will use when messaging you"));
		this->SetSyntax(_("\037language\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		NickAlias *na = findnick(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		for (unsigned j = 0; j < languages.size(); ++j)
		{
			if (param == "en" || languages[j] == param)
				break;
			else if (j + 1 == languages.size())
			{
				this->OnSyntaxError(source, "");
				return;
			}
		}

		nc->language = param != "en" ? param : "";
		source.Reply(_("Language changed to \002English\002."));

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &param)
	{
		this->Run(source, source.u->Account()->display, param[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the language Services uses when sending messages to\n"
				"you (for example, when responding to a command you send).\n"
				"\037language\037 should be chosen from the following list of\n"
				"supported languages:"));

		source.Reply("         en (English)");
		for (unsigned j = 0; j < languages.size(); ++j)
		{
			const Anope::string &langname = anope_gettext(languages[j].c_str(), _("English"));
			if (langname == "English")
				continue;
			source.Reply("         %s (%s)", languages[j].c_str(), langname.c_str());
		}

		return true;
	}
};

class CommandNSSASetLanguage : public CommandNSSetLanguage
{
 public:
	CommandNSSASetLanguage(Module *creator) : CommandNSSetLanguage(creator, "nickserv/saset/language", 2, "nickserv/saset/language")
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 \037language\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the language Services uses when sending messages to\n"
				"you (for example, when responding to a command you send).\n"
				"\037language\037 should be chosen from the following list of\n"
				"supported languages:"));
		return true;
	}
};

class NSSetLanguage : public Module
{
	CommandNSSetLanguage commandnssetlanguage;
	CommandNSSASetLanguage commandnssasetlanguage;

 public:
	NSSetLanguage(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssetlanguage(this), commandnssasetlanguage(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandnssetlanguage);
		ModuleManager::RegisterService(&commandnssasetlanguage);
	}
};

MODULE_INIT(NSSetLanguage)
