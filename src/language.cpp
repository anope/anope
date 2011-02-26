#include "services.h"
#include <stack>

std::vector<Anope::string> languages;

void InitLanguages()
{
#if GETTEXT_FOUND
	languages.clear();
	spacesepstream sep(Config->Languages);
	Anope::string language;

	while (sep.GetToken(language))
	{
		if (!IsFile("languages/" + language + "/LC_MESSAGES/anope.mo"))
		{
			Log() << "Error loading language " << language << ", file does not exist!";
		}
		else
		{
			Log(LOG_DEBUG) << "Found language file " << language;
			languages.push_back(language);
		}
	}

	if (!bindtextdomain("anope", (services_dir + "/languages/").c_str()))
		Log() << "Error calling bindtextdomain, " << Anope::LastError();
	else
		Log(LOG_DEBUG) << "Successfully bound anope to " << services_dir << "/languages/";
#else
	Log() << "Can not load languages, gettext is not installed";
#endif
}

const Anope::string GetString(NickCore *nc, const char *string)
{
	return GetString("anope", nc ? nc->language : "", string);
}

const Anope::string GetString(const Anope::string &domain, const Anope::string &lang, const char *string)
{
	PushLanguage(domain, lang);
	const char *t_string = anope_gettext(string);
	PopLanguage();
	return t_string;
}

#if GETTEXT_FOUND
static std::stack<std::pair<Anope::string, Anope::string > > language_stack;

void PushLanguage(const Anope::string &domain, Anope::string language)
{
	if (language.empty() && !Config->NSDefLanguage.empty())
		language = Config->NSDefLanguage;
	
	language_stack.push(std::make_pair(domain, language));
}

void PopLanguage()
{
	language_stack.pop();
}

/* Used by gettext to make it always dynamically load language strings (so we can drop them in while Anope is running) */
extern "C" int _nl_msg_cat_cntr;

const char *anope_gettext(const char *string)
{
	std::pair<Anope::string, Anope::string> lang_info;
	if (!language_stack.empty())
		lang_info = language_stack.top();
	if (*string == 0 || lang_info.first.empty() || lang_info.second.empty())
		return string;

	++_nl_msg_cat_cntr;
#ifdef _WIN32
	SetThreadLocale(MAKELCID(MAKELANGID(WindowsGetLanguage(lang_info.second.c_str()), SUBLANG_DEFAULT), SORT_DEFAULT));
#else
	/* First, set LANGUAGE env variable.
	 * Some systems (Debian) don't care about this, so we must setlocale LC_ALL aswell.
	 * BUT if this call fails because the LANGUAGE env variable is set, setlocale resets
	 * the locale to "C", which short circuits gettext and causes it to fail on systems that
	 * use the LANGUAGE env variable. We must reset the locale to en_US (or, anything not
	 * C or POSIX) then.
	 */
	setenv("LANGUAGE", lang_info.second.c_str(), 1);
	if (setlocale(LC_ALL, lang_info.second.c_str()) == NULL)
		setlocale(LC_ALL, "en_US");
#endif
	const char *translated_string = dgettext(lang_info.first.c_str(), string);
#ifdef _WIN32
	SetThreadLocale(MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), SORT_DEFAULT));
#else
	unsetenv("LANGUAGE");
	setlocale(LC_ALL, "");
#endif

	return translated_string != NULL ? translated_string : "";
}
#else
void PushLanguage(const Anope::string &, Anope::string)
{
}

void PopLanguage()
{
}

const char *anope_gettext(const char *string)
{
	return string != NULL ? string : "";
}
#endif

void SyntaxError(CommandSource &source, const Anope::string &command, const Anope::string &message)
{
	source.Reply(_("Syntax: \002%s\002"), message.c_str());
	source.Reply(_(_(MORE_INFO)), source.owner->nick.c_str(), command.c_str());
}

