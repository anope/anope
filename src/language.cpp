/*
 *
 * (C) 2003-2025 Anope Team
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
#include "language.h"

#if HAVE_LOCALIZATION
# include <libintl.h>
#endif

std::vector<Anope::string> Language::Languages;
std::vector<Anope::string> Language::Domains;

void Language::InitLanguages()
{
#if HAVE_LOCALIZATION
	Log(LOG_DEBUG) << "Initializing Languages...";

	Languages.clear();

	if (!bindtextdomain("anope", Anope::LocaleDir.c_str()))
		Log() << "Error calling bindtextdomain, " << Anope::LastError();
	else
		Log(LOG_DEBUG) << "Successfully bound anope to " << Anope::LocaleDir;

	setlocale(LC_ALL, "");

	spacesepstream sep(Config->GetBlock("options").Get<const Anope::string>("languages"));
	Anope::string language;
	while (sep.GetToken(language))
	{
		const Anope::string &lang_name = Translate(language.c_str(), _("English"));
		if (lang_name == "English")
		{
			Log() << "Unable to use language " << language;
			continue;
		}

		Log(LOG_DEBUG) << "Found language " << language;
		Languages.push_back(language);
	}
#else
	Log() << "Unable to initialize languages, gettext is not installed";
#endif
}

const char *Language::Translate(const char *string)
{
	return Translate("", string);
}

const char *Language::Translate(User *u, const char *string)
{
	if (u && u->IsIdentified())
		return Translate(u->Account(), string);
	else
		return Translate("", string);
}

const char *Language::Translate(const NickCore *nc, const char *string)
{
	return Translate(nc ? nc->language.c_str() : "", string);
}

const char *Language::Translate(int count, const char *singular, const char *plural)
{
	return Translate("", count, singular, plural);
}

const char *Language::Translate(User *u, int count, const char *singular, const char *plural)
{
	if (u && u->IsIdentified())
		return Translate(u->Account(), count, singular, plural);
	else
		return Translate("", count, singular, plural);
}

const char *Language::Translate(const NickCore *nc, int count, const char *singular, const char *plural)
{
	return Translate(nc ? nc->language.c_str() : "", count, singular, plural);
}

#if HAVE_LOCALIZATION

#if defined(__GLIBC__) && defined(__USE_GNU_GETTEXT)
extern "C" int _nl_msg_cat_cntr;
#endif

namespace
{
	void PreTranslate(const char* lang)
	{
#if defined(__GLIBC__) && defined(__USE_GNU_GETTEXT)
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
	}

	void PostTranslate()
	{
#ifdef _WIN32
		SetThreadLocale(MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), SORT_DEFAULT));
#else
		unsetenv("LANGUAGE");
		unsetenv("LANG");
		setlocale(LC_ALL, "");
#endif
	}
}

const char *Language::Translate(const char *lang, const char *string)
{
	if (!string || !*string)
		return "";

	if (!lang || !*lang)
		lang = Config->DefLanguage.c_str();

	PreTranslate(lang);

	const char *translated_string = dgettext("anope", string);
	for (unsigned i = 0; translated_string == string && i < Domains.size(); ++i)
		translated_string = dgettext(Domains[i].c_str(), string);

	PostTranslate();
	return translated_string;
}

const char *Language::Translate(const char *lang, int count, const char *singular, const char *plural)
{
	if (!singular || !*singular || !plural || !*plural)
		return "";

	if (!lang || !*lang)
		lang = Config->DefLanguage.c_str();

	PreTranslate(lang);

	const char *translated_string = dngettext("anope", singular, plural, count);
	for (unsigned i = 0; (translated_string == singular || translated_string == plural) && i < Domains.size(); ++i)
		translated_string = dngettext(Domains[i].c_str(), singular, plural, count);

	PostTranslate();
	return translated_string;
}
#else
const char *Language::Translate(const char *lang, const char *string)
{
	return string != NULL ? string : "";
}
const char *Language::Translate(const char *lang, int count, const char *singular, const char *plural)
{
	return Language::Translate("", count == 1 ? singular : plural);
}
#endif


