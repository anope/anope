/*
 *
 * (C) 2008-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

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
	 * @param u The user to translate the string for
	 * @param string A string to translate
	 * @return The translated string if found, else the original string.
	 */
	extern CoreExport const char *Translate(User *u, const char *string);

	/** Translates a string to the language of the given account.
	 * @param nc The account to translate the string for
	 * @param string A string to translate
	 * @return The translated string if count, else the original string
	 */
	extern CoreExport const char *Translate(const NickCore *nc, const char *string);

	/** Translates a string to the given language.
	 * @param lang The language to translate to
	 * @param string The string to translate
	 * @return The translated string if found, else the original string.
	 */
	extern CoreExport const char *Translate(const char *lang, const char *string);

	/** Translates a plural string to the default language.
	 * @param count The number of items the string is counting.
	 * @param singular The string to translate if there is one of \p count
	 * @param plural The string to translate if there is multiple of \p count
	 * @return The translated string if found, else the original string.
	 */
	extern CoreExport const char *Translate(int count, const char *singular, const char *plural);

	/** Translates a plural string to the language of the given user.
	 * @param u The user to translate the string for
	 * @param count The number of items the string is counting.
	 * @param singular The string to translate if there is one of \p count
	 * @param plural The string to translate if there is multiple of \p count
	 * @return The translated string if found, else the original string.
	 */
	extern CoreExport const char *Translate(User *u, int count, const char *singular, const char *plural);

	/** Translates a plural string to the language of the given account.
	 * @param nc The account to translate the string for
	 * @param count The number of items the string is counting.
	 * @param singular The string to translate if there is one of \p count
	 * @param plural The string to translate if there is multiple of \p count
	 * @return The translated string if count, else the original string
	 */
	extern CoreExport const char *Translate(const NickCore *nc, int count, const char *singular, const char *plural);

	/** Translates a plural string to the given language.
	 * @param lang The language to translate to
	 * @param count The number of items the string is counting.
	 * @param singular The string to translate if there is one of \p count
	 * @param plural The string to translate if there is multiple of \p count
	 * @return The translated string if found, else the original string.
	 */
	extern CoreExport const char *Translate(const char *lang, int count, const char *singular, const char *plural);

} // namespace Language

/* Commonly used language strings */
#define CONFIRM_DROP			_("Please confirm that you want to drop \002%s\002 with \002%s\032%s\032%s\002")
#define SERVICE_UNAVAILABLE		_("Sorry, %s is temporarily unavailable.")
#define MORE_INFO			_("Type \002%s\002 for more information.")
#define BAD_USERHOST_MASK		_("Mask must be in the form \037user\037@\037host\037.")
#define BAD_EXPIRY_TIME			_("Invalid expiry time.")
#define USERHOST_MASK_TOO_WIDE		_("%s coverage is too wide; Please use a more specific mask.")
#define READ_ONLY_MODE			_("Services are temporarily in read-only mode.")
#define PASSWORD_INCORRECT		_("Password incorrect.")
#define ACCESS_DENIED			_("Access denied.")
#define MORE_OBSCURE_PASSWORD		_("Please try again with a more obscure password. Passwords should not be " \
						"something that could be easily guessed (e.g. your real name or your nick) and " \
						"cannot contain the space or tab characters.")
#define PASSWORD_TOO_SHORT		_("Your password is too short. It must be longer than %u characters.")
#define PASSWORD_TOO_LONG		_("Your password is too long. It must be shorter than %u characters.")
#define NICK_NOT_REGISTERED		_("Your nick isn't registered.")
#define NICK_X_NOT_REGISTERED		_("Nick \002%s\002 isn't registered.")
#define NICK_X_NOT_IN_USE		_("Nick \002%s\002 isn't currently in use.")
#define NICK_X_NOT_ON_CHAN		_("\002%s\002 is not currently on channel %s.")
#define NICK_X_SUSPENDED		_("Nick %s is currently suspended.")
#define CHAN_X_SUSPENDED		_("Channel %s is currently suspended.")
#define CHAN_X_NOT_REGISTERED		_("Channel \002%s\002 isn't registered.")
#define CHAN_X_NOT_IN_USE		_("Channel \002%s\002 doesn't exist.")
#define NICK_IDENTIFY_REQUIRED		_("You must be logged into an account to use that command.")
#define MAIL_X_INVALID			_("\002%s\002 is not a valid email address.")
#define UNKNOWN				_("<unknown>")
#define NO_EXPIRE			_("does not expire")
#define LIST_INCORRECT_RANGE		_("Incorrect range specified. The correct syntax is \002#\037from\037-\037to\037\002.")
#define NICK_IS_SECURE			_("This nickname is registered and protected.  If it is your " \
						"nick, type \002%s\032\037password\037\002.  Otherwise, " \
						"please choose a different nick.")
#define FORCENICKCHANGE_NOW		_("This nickname has been registered; you may not use it.")
#define NICK_CANNOT_BE_REGISTERED	_("Nickname \002%s\002 may not be registered.")
#define NICK_ALREADY_REGISTERED		_("Nickname \002%s\002 is already registered!")
#define NICK_SET_DISPLAY_CHANGED	_("The new display is now \002%s\002.")
#define NICK_CONFIRM_INVALID		_("Invalid passcode has been entered, please check the email again, and retry.")
#define CHAN_NOT_ALLOWED_TO_JOIN	_("You are not permitted to be on this channel.")
#define CHAN_X_INVALID			_("Channel %s is not a valid channel.")
#define CHAN_REACHED_CHANNEL_LIMIT	_("Sorry, you have already reached your limit of \002%d\002 channels.")
#define CHAN_EXCEEDED_CHANNEL_LIMIT	_("Sorry, you have already exceeded your limit of \002%d\002 channels.")
#define CHAN_SYMBOL_REQUIRED		_("Please use the symbol of \002#\002 when attempting to register.")
#define CHAN_SETTING_CHANGED		_("%s for %s set to %s.")
#define CHAN_SETTING_UNSET		_("%s for %s unset.")
#define CHAN_ACCESS_LEVEL_RANGE		_("Access level must be between %d and %d inclusive.")
#define CHAN_INFO_HEADER		_("Information about channel \002%s\002:")
#define CHAN_EXCEPTED			_("\002%s\002 matches an except on %s and cannot be banned until the except has been removed.")
#define MEMO_NEW_X_MEMO_ARRIVED		_("There is a new memo on channel %s. " \
						"Type \002%s\032%s\032%zu\002 to read it.")
#define MEMO_NEW_MEMO_ARRIVED		_("You have a new memo from %s. " \
						"Type \002%s\032%zu\002 to read it.")
#define MEMO_HAVE_NO_MEMOS		_("You have no memos.")
#define MEMO_X_HAS_NO_MEMOS		_("%s has no memos.")
#define MEMO_HAVE_NO_NEW_MEMOS		_("You have no new memos.")
#define MEMO_X_HAS_NO_NEW_MEMOS		_("%s has no new memos.")
#define BOT_DOES_NOT_EXIST		_("Bot \002%s\002 does not exist.")
#define BOT_NOT_ASSIGNED		_("You must assign a bot to the channel before using this command.")
#define BOT_NOT_ON_CHANNEL		_("Bot is not on channel \002%s\002.")
#define HOST_SET_ERROR			_("A vhost must be in the format of a valid hostname.")
#define HOST_SET_IDENT_ERROR		_("A vident must be in the format of a valid ident.")
#define HOST_SET_TOOLONG		_("Error! The vhost is too long, please use a hostname shorter than %zu characters.")
#define HOST_SET_IDENTTOOLONG		_("Error! The vident is too long, please use an ident shorter than %zu characters.")
#define HOST_NOT_ASSIGNED		_("Please contact an Operator to get a vhost assigned to this nick.")
#define HOST_NO_VIDENT			_("Your IRCd does not support vidents. If this is incorrect please report this as a possible bug.")
