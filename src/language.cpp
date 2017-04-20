/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "services.h"
#include "modules.h"
#include "commands.h"
#include "config.h"
#include "language.h"
#include "modules/nickserv.h"

#if GETTEXT_FOUND
# include <libintl.h>
#endif

std::vector<Anope::string> Language::Languages;
std::vector<Anope::string> Language::Domains;

void Language::InitLanguages()
{
#if GETTEXT_FOUND
	Anope::Logger.Debug("Initializing Languages...");

	Languages.clear();

	if (!bindtextdomain("anope", Anope::LocaleDir.c_str()))
		Anope::Logger.Log("Error calling bindtextdomain, {0}", Anope::LastError());
	else
		Anope::Logger.Debug("Successfully bound anope to {0}", Anope::LocaleDir);

	setlocale(LC_ALL, "");

	spacesepstream sep(Config->GetBlock("options")->Get<Anope::string>("languages"));
	Anope::string language;
	while (sep.GetToken(language))
	{
		const Anope::string &lang_name = Translate(language.c_str(), _("English"));
		if (lang_name == "English")
		{
			Anope::Logger.Log("Unable to use language {0}", language);
			continue;
		}

		Anope::Logger.Debug("Found language {0}", language);
		Languages.push_back(language);
	}
#else
	Anope::Logger.Log("Unable to initialize languages, gettext is not installed");
#endif
}

const char *Language::Translate(const char *string)
{
	return Translate("", string);
}

const char *Language::Translate(const Anope::string &string)
{
	return Translate("", string.c_str());
}

const char *Language::Translate(User *u, const char *string)
{
	if (u && u->Account())
		return Translate(u->Account(), string);
	else
		return Translate("", string);
}

const char *Language::Translate(User *u, const Anope::string &string)
{
	return Translate(u, string.c_str());
}

const char *Language::Translate(NickServ::Account *nc, const char *string)
{
	return Translate(nc ? nc->GetLanguage().c_str() : "", string);
}

const char *Language::Translate(NickServ::Account *nc, const Anope::string &string)
{
	return Translate(nc, string.c_str());
}

#if GETTEXT_FOUND

#ifdef __USE_GNU_GETTEXT
extern "C" int _nl_msg_cat_cntr;
#endif

const char *Language::Translate(const char *lang, const char *string)
{
	if (!string || !*string)
		return "";

	if (!lang || !*lang)
	{
		if (Config == nullptr)
			return string;

		lang = Config->DefLanguage.c_str();
	}

#ifdef __USE_GNU_GETTEXT
	++_nl_msg_cat_cntr;
#endif

#ifdef _WIN32
	SetThreadLocale(MAKELCID(MAKELANGID(WindowsGetLanguage(lang), SUBLANG_DEFAULT), SORT_DEFAULT));
#else
	/* First, set LANG and LANGUAGE env variables.
	 * Some systems (Debian) don't care about this, so we must setlocale LC_ALL as well.
	 * BUT if this call fails because the LANGUAGE env variable is set, setlocale resets
	 * the locale to "C", which short circuits gettext and causes it to fail on systems that
	 * use the LANGUAGE env variable. We must reset the locale to en_US (or, anything not
	 * C or POSIX) then.
	 */
	setenv("LANG", lang, 1);
	setenv("LANGUAGE", lang, 1);
	if (setlocale(LC_ALL, lang) == NULL)
		setlocale(LC_ALL, "en_US");
#endif
	const char *translated_string = dgettext("anope", string);
	for (unsigned i = 0; translated_string == string && i < Domains.size(); ++i)
		translated_string = dgettext(Domains[i].c_str(), string);
#ifdef _WIN32
	SetThreadLocale(MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), SORT_DEFAULT));
#else
	unsetenv("LANGUAGE");
	unsetenv("LANG");
	setlocale(LC_ALL, "");
#endif

	return translated_string;
}
#else
const char *Language::Translate(const char *lang, const char *string)
{
	return string != NULL ? string : "";
}
#endif

