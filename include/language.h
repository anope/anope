// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
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

	/** Sets the locale to the specified language.
	 * @param lang The language to translate to.
	 */
	extern CoreExport void SetLocale(const char* lang);

	/** Sets the locale back to the default. */
	extern CoreExport void ResetLocale();

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
#define ACCESS_DENIED                _("Access denied.")
#define BAD_EXPIRY_TIME              _("Invalid expiry time.")
#define BAD_USERHOST_MASK            _("Mask must be in the form \037user\037@\037host\037.")
#define CONFIRM_DROP                 _("Please confirm that you want to drop \002%s\002 with \002%s\033%s\033%s\002")
#define CONFIRM_REGISTER_ADMIN       _("All new accounts must be confirmed by an administrator. Please wait for your registration to be confirmed.")
#define CONFIRM_REGISTER_CODE        _("All new accounts must be confirmed. To confirm your account, type \002%s\002.")
#define CONFIRM_REGISTER_MAIL        _("All new accounts must be confirmed. To confirm your account, follow the instructions that were emailed to you.")
#define LIST_INCORRECT_RANGE         _("Incorrect range specified. The correct syntax is \002#\037from\037-\037to\037\002.")
#define MAIL_X_INVALID               _("\002%s\002 is not a valid email address.")
#define MORE_INFO                    _("Type \002%s\002 for more information.")
#define NO_EXPIRE                    _("does not expire")
#define PASSWORD_INCORRECT           _("Password incorrect.")
#define READ_ONLY_MODE               _("Services are temporarily in read-only mode.")
#define TIME_NEVER                   _("Never")
#define TIME_NOW                     _("Now")
#define TIME_UNKNOWN                 _("Unknown")
#define TRY_AGAIN_LATER              _("The \002%s\002 command is temporarily unavailable. Please try again later.")
#define USERHOST_MASK_TOO_WIDE       _("%s coverage is too wide; Please use a more specific mask.")

#define BOT_DOES_NOT_EXIST           _("Bot \002%s\002 does not exist.")
#define BOT_NOT_ASSIGNED             _("You must assign a bot to the channel before using this command.")
#define BOT_NOT_ON_CHANNEL           _("Bot is not on channel \002%s\002.")

#define CHAN_ACCESS_LIMIT           N_("You can only have %u access entry on a channel.", "You can only have %u access entries on a channel.")
#define CHAN_ACCESS_LIMIT_DEEP      N_("You can only have %u access entry on a channel, including access entries from other channels.", "You can only have %u access entries on a channel, including access entries from other channels.")
#define CHAN_ACCESS_LEVEL_RANGE      _("Access level must be between %d and %d inclusive.")
#define CHAN_ACCESS_MALFORMED        _("You cannot add a malformed mask to an access list. Did you mean to add %s instead?")
#define CHAN_ACCESS_FOREIGN         N_("%u access entry from other access systems not shown; use \002%s\033ALL\002 to view all access entries.", "%u access entries from other access systems not shown; use \002%s\033ALL\002 to view all access entries.")
#define CHAN_ACCESS_MIGRATED_1       _("\002%s\002 has been migrated to the %s access system.")
#define CHAN_ACCESS_MIGRATED_N      N_("\002%u\002 entry has been migrated to the %s access system.", "\002%u\002 entries have been migrated to the %s access system.")
#define CHAN_ACCESS_NOT_MIGRATED_1   _("\002%s\002 can not be migrated to the %s access system because they have privileges that you do not.")
#define CHAN_ACCESS_NOT_MIGRATED_N  N_("\002%u\002 entry can not be migrated to the %s access system because they have privileges that you do not.", "\002%u\002 entries can not be migrated to the %s access system because they have privileges that you do not.")
#define CHAN_EXCEPTED                _("\002%s\002 matches an except on %s and cannot be banned until the except has been removed.")
#define CHAN_INFO_HEADER             _("Information about channel \002%s\002:")
#define CHAN_LIMIT_EXCEEDED          _("You have already exceeded your limit of \002%d\002 channels.")
#define CHAN_LIMIT_REACHED           _("You have already reached your limit of \002%d\002 channels.")
#define CHAN_NOT_ALLOWED_TO_JOIN     _("You are not permitted to be on this channel.")
#define CHAN_SETTING_CHANGED         _("%s for %s set to %s.")
#define CHAN_SETTING_INVALID         _("%s syntax is invalid.")
#define CHAN_SETTING_UNSET           _("%s for %s unset.")
#define CHAN_SYMBOL_REQUIRED         _("Please use the symbol of \002#\002 when attempting to register.")
#define CHAN_X_INVALID               _("Channel %s is not a valid channel.")
#define CHAN_X_NOT_IN_USE            _("Channel \002%s\002 doesn't exist.")
#define CHAN_X_NOT_REGISTERED        _("Channel \002%s\002 isn't registered.")
#define CHAN_X_SUSPENDED             _("Channel %s is currently suspended.")

#define HOST_NO_VIDENT               _("Your IRCd does not support vidents. If this is incorrect please report this as a possible bug.")
#define HOST_NOT_ASSIGNED            _("Please contact an Operator to get a vhost assigned to this nick.")
#define HOST_SET_VHOST_ERROR         _("A vhost must be in the format of a valid hostname.")
#define HOST_SET_VHOST_TOO_LONG      _("Error! The vhost is too long, please use a hostname shorter than %zu characters.")
#define HOST_SET_VIDENT_ERROR        _("A vident must be in the format of a valid ident.")
#define HOST_SET_VIDENT_TOO_LONG     _("Error! The vident is too long, please use an ident shorter than %zu characters.")

#define MEMO_HAVE_NO_MEMOS           _("You have no memos.")
#define MEMO_HAVE_NO_NEW_MEMOS       _("You have no new memos.")
#define MEMO_NEW_MEMO_ARRIVED        _("You have a new memo from %s. Type \002%s\033%zu\002 to read it.")
#define MEMO_NEW_X_MEMO_ARRIVED      _("There is a new memo on channel %s. Type \002%s\033%s\033%zu\002 to read it.")
#define MEMO_X_HAS_NO_MEMOS          _("%s has no memos.")
#define MEMO_X_HAS_NO_NEW_MEMOS      _("%s has no new memos.")

#define NICK_ALREADY_REGISTERED      _("Nickname \002%s\002 is already registered!")
#define NICK_CANNOT_BE_REGISTERED    _("Nickname \002%s\002 may not be registered.")
#define NICK_IDENTIFY_REQUIRED       _("You must be logged into an account to use that command.")
#define NICK_SET_DISPLAY_CHANGED     _("The new display is now \002%s\002.")
#define NICK_X_NOT_IN_USE            _("Nick \002%s\002 isn't currently in use.")
#define NICK_X_NOT_ON_CHAN           _("\002%s\002 is not currently on channel %s.")
#define NICK_X_NOT_REGISTERED        _("Nick \002%s\002 isn't registered.")
#define NICK_X_SUSPENDED             _("Nick %s is currently suspended.")
