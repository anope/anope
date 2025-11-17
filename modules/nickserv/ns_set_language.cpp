// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

#define ENGLISH _("English")

class CommandNSSetLanguage
	: public Command
{
protected:
	Anope::map<Anope::string> &languages;

	const char *GetDefaultLanguage() const
	{
		auto it = languages.find(Config->DefLanguage);
		if (it != languages.end())
			return it->second.c_str();
		return ENGLISH;
	}

public:
	CommandNSSetLanguage(Module *creator, Anope::map<Anope::string> &langs, const Anope::string &sname = "nickserv/set/language", size_t min = 0)
		: Command(creator, sname, min, min + 1)
		, languages(langs)
	{
		this->SetDesc(_("Set the language services will use when messaging you"));
		this->SetSyntax(_("[\037language\037]"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const auto *na = NickAlias::Find(user);
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

		Log() << "PARAM-EMPTY: " << param;

		Anope::string langname;
		if (param.empty())
		{
			langname = Language::Translate(ENGLISH);
			nc->language.clear();
		}
		else
		{
			auto lang = languages.end();
			for (auto it = languages.begin(); it != languages.end(); ++it)
			{
				auto &[langcode, langname] = *it;
				if (langcode.find_ci(param) != 0)
					continue; // Language does not match.

				if (lang != languages.end())
				{
					source.Reply(_("Multiple languages matched \002%s\002. Please be more specific."), param.c_str());
					return;
				}

				lang = it;
			}

			if (lang == languages.end())
			{
				this->OnSyntaxError(source, "");
				return;
			}

			langname = lang->second;
			nc->language = lang->first;
		}

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the language of " << nc->display << " to " << langname;
		if (source.GetAccount() == nc)
			source.Reply(_("Language changed to \002%s\002."), langname.c_str());
		else
			source.Reply(_("Language for \002%s\002 changed to \002%s\002."), nc->display.c_str(), langname.c_str());
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params.empty() ? "" : params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Changes the language services uses when sending messages to "
				"you (for example, when responding to a command you send). If "
				"\037language\037 is not specified the default (%s) will be "
				"used. Otherwise, \037language\037 should be chosen from the "
				"following list of supported languages:"
			),
			GetDefaultLanguage());

		for (const auto &[langcode, langname] : languages)
			source.Reply("    %s (%s)", langcode.c_str(), langname.c_str());

		return true;
	}
};

class CommandNSSASetLanguage final
	: public CommandNSSetLanguage
{
public:
	CommandNSSASetLanguage(Module *creator, Anope::map<Anope::string> &langs)
		: CommandNSSetLanguage(creator, langs, "nickserv/saset/language", 1)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 [\037language\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Changes the language services uses when sending messages to "
				"the given user (for example, when responding to a command they "
				"send). If \037language\037 is not specified the default (%s) "
				"will be used. Otherwise, \037language\037 should be chosen from "
				"the following list of supported languages:"
			),
			GetDefaultLanguage());

		for (const auto &[langcode, langname] : languages)
			source.Reply("    %s (%s)", langcode.c_str(), langname.c_str());

		return true;
	}
};

class NSSetLanguage final
	: public Module
{
private:
	CommandNSSetLanguage commandnssetlanguage;
	CommandNSSASetLanguage commandnssasetlanguage;
	Anope::map<Anope::string> languages;

public:
	NSSetLanguage(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnssetlanguage(this, languages)
		, commandnssasetlanguage(this, languages)
	{
#if !HAVE_LOCALIZATION
		throw ModuleException("Anope was not built with localization support");
#endif

		// Build a list of languages. This only needs to happen once as we
		// only load the languages on boot.
		languages.emplace("en_US.UTF-8", ENGLISH);
		for (const auto &language : Language::Languages)
			languages.emplace(language, Language::Translate(language.c_str(), ENGLISH));
	}
};

MODULE_INIT(NSSetLanguage)
