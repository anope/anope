/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "commands.h"
#include "config.h"
#include "extern.h"

#if GETTEXT_FOUND
# include <libintl.h>
#endif

std::vector<Anope::string> languages;
std::vector<Anope::string> domains;

void InitLanguages()
{
#if GETTEXT_FOUND
	languages.clear();
	spacesepstream sep(Config->Languages);
	Anope::string language;

	while (sep.GetToken(language))
	{
		if (!IsFile(locale_dir + "/" + language + "/LC_MESSAGES/anope.mo"))
		{
			Log() << "Error loading language " << language << ", file does not exist!";
		}
		else
		{
			Log(LOG_DEBUG) << "Found language file " << language;
			languages.push_back(language);
		}
	}

	if (!bindtextdomain("anope", locale_dir.c_str()))
		Log() << "Error calling bindtextdomain, " << Anope::LastError();
	else
		Log(LOG_DEBUG) << "Successfully bound anope to " << locale_dir;
	
	setlocale(LC_ALL, "");
#else
	Log() << "Can not load languages, gettext is not installed";
#endif
}

const char *translate(const char *string)
{
	return anope_gettext(Config->NSDefLanguage.c_str(), string);
}

const char *translate(User *u, const char *string)
{
	return translate(u ? u->Account() : NULL, string);
}

const char *translate(const NickCore *nc, const char *string)
{
	return anope_gettext(nc ? nc->language.c_str() : Config->NSDefLanguage.c_str(), string);
}

#if GETTEXT_FOUND

/* Used by gettext to make it always dynamically load language strings (so we can drop them in while Anope is running) */
extern "C" int _nl_msg_cat_cntr;

const char *anope_gettext(const char *lang, const char *string)
{
	if (!string || !*string || !lang || !*lang)
		return string ? string : "";

	++_nl_msg_cat_cntr;
#ifdef _WIN32
	SetThreadLocale(MAKELCID(MAKELANGID(WindowsGetLanguage(lang), SUBLANG_DEFAULT), SORT_DEFAULT));
#else
	/* First, set LANGUAGE env variable.
	 * Some systems (Debian) don't care about this, so we must setlocale LC_ALL aswell.
	 * BUT if this call fails because the LANGUAGE env variable is set, setlocale resets
	 * the locale to "C", which short circuits gettext and causes it to fail on systems that
	 * use the LANGUAGE env variable. We must reset the locale to en_US (or, anything not
	 * C or POSIX) then.
	 */
	setenv("LANGUAGE", lang, 1);
	if (setlocale(LC_ALL, lang) == NULL)
		setlocale(LC_ALL, "en_US");
#endif
	const char *translated_string = dgettext("anope", string);
	for (unsigned i = 0; translated_string == string && i < domains.size(); ++i)
		translated_string = dgettext(domains[i].c_str(), string);
#ifdef _WIN32
	SetThreadLocale(MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), SORT_DEFAULT));
#else
	unsetenv("LANGUAGE");
	setlocale(LC_ALL, "");
#endif

	return translated_string;
}
#else
const char *anope_gettext(const char *lang, const char *string)
{
	return string != NULL ? string : "";
}
#endif

