/* NickServ core functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandNSSetLanguage
	: public Command
{
public:
	CommandNSSetLanguage(Module *creator, const Anope::string &sname = "nickserv/set/language", size_t min = 1)
		: Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Set the language services will use when messaging you"));
		this->SetSyntax(_("\037language\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetNickOption, MOD_RESULT, (source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param != "en_US")
		{
			for (unsigned j = 0; j < Language::Languages.size(); ++j)
			{
				if (Language::Languages[j] == param)
					break;
				else if (j + 1 == Language::Languages.size())
				{
					this->OnSyntaxError(source, "");
					return;
				}
			}
		}

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the language of " << nc->display << " to " << param;

		nc->language = param;
		if (source.GetAccount() == nc)
			source.Reply(_("Language changed to \002English\002."));
		else
			source.Reply(_("Language for \002%s\002 changed to \002%s\002."), nc->display.c_str(), Language::Translate(param.c_str(), _("English")));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &param) override
	{
		this->Run(source, source.nc->display, param[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the language services uses when sending messages to\n"
				"you (for example, when responding to a command you send).\n"
				"\037language\037 should be chosen from the following list of\n"
				"supported languages:"));

		source.Reply("         en_US (English)");
		for (const auto &language : Language::Languages)
		{
			const Anope::string &langname = Language::Translate(language.c_str(), _("English"));
			if (langname != "English")
				source.Reply("         %s (%s)", language.c_str(), langname.c_str());
		}

		return true;
	}
};

class CommandNSSASetLanguage final
	: public CommandNSSetLanguage
{
public:
	CommandNSSASetLanguage(Module *creator)
		: CommandNSSetLanguage(creator, "nickserv/saset/language", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 \037language\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the language services uses when sending messages to\n"
				"the given user (for example, when responding to a command they send).\n"
				"\037language\037 should be chosen from the following list of\n"
				"supported languages:"));
		source.Reply("         en_US (English)");
		for (const auto &language : Language::Languages)
		{
			const Anope::string &langname = Language::Translate(language.c_str(), _("English"));
			if (langname != "English")
				source.Reply("         %s (%s)", language.c_str(), langname.c_str());
		}
		return true;
	}
};

class NSSetLanguage final
	: public Module
{
private:
	CommandNSSetLanguage commandnssetlanguage;
	CommandNSSASetLanguage commandnssasetlanguage;

public:
	NSSetLanguage(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnssetlanguage(this)
		, commandnssasetlanguage(this)
	{
#ifndef HAVE_LOCALIZATION
		throw ModuleException("Anope was not built with localization support");
#endif
	}
};

MODULE_INIT(NSSetLanguage)
