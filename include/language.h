/*
 *
 * (C) 2008-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#include "anope.h"

namespace Language
{

	/* Languages we support as configured in anope.conf. They are
	 * added to this list if we detect a language exists in the correct
	 * location for each language.
	 */
	extern CoreExport std::vector<Anope::string> Languages;

	/* Domains to search when looking for translations other than the
	 * default "anope domain. This is used by modules who add their own
	 * language files (and thus domains) to Anope. If a module is loaded
	 * and we detect a language file exists for at least one of the supported
	 * languages for the module, then we add the module's domain (its name)
	 * here.
	 *
	 * When strings are translated they are checked against all domains.
	 */
	extern std::vector<Anope::string> Domains;

	/** Initialize the language system. Finds valid language files and
	 * populates the Languages list.
	 */
	extern void InitLanguages();

	/** Translates a string to the default language.
	 * @param string A string to translate
	 * @return The translated string if found, else the original string.
	 */
	extern CoreExport const char *Translate(const char *string);

	/** Translates a string to the language of the given user.
	 * @param u The user to transate the string for
	 * @param string A string to translate
	 * @return The translated string if found, else the original string.
	 */
	extern CoreExport const char *Translate(User *u, const char *string);

	/** Translates a string to the language of the given account.
	 * @param nc The account to translate the string for
	 * @param string A string to translate
	 * @return The translated string if count, else the original string
	 */
	extern CoreExport const char *Translate(NickServ::Account *nc, const char *string);

	/** Translatesa string to the given language.
	 * @param lang The language to translate to
	 * @param string The string to translate
	 * @return The translated string if found, else the original string.
	 */
	extern CoreExport const char *Translate(const char *lang, const char *string);

} // namespace Language

