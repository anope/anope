/* Commonly used language strings
 *
 * (C) 2008-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

namespace LanguageString
{
	const char *const MORE_INFO =				_("\002%R%s HELP %s\002 for more information.");
	const char *const BAD_USERHOST_MASK =			_("Mask must be in the form \037user\037@\037host\037.");
	const char *const BAD_EXPIRY_TIME = 			_("Invalid expiry time.");
	const char *const USERHOST_MASK_TOO_WIDE =		_("%s coverage is too wide; Please use a more specific mask.");
	const char *const READ_ONLY_MODE =			_("\002Notice:\002 Services is in read-only mode; changes will not be saved!");
	const char *const PASSWORD_INCORRECT =			_("Password incorrect.");
	const char *const ACCESS_DENIED =			_("Access denied.");
	const char *const MORE_OBSCURE_PASSWORD =		_("Please try again with a more obscure password. Passwords should be at least five characters long, should not be something easily guessed (e.g. your real name or your nick), and cannot contain the space or tab characters.");
	const char *const PASSWORD_TOO_LONG =			_("Your password is too long. Please try again with a shorter password.");
	const char *const NICK_NOT_REGISTERED =			_("Your nick isn't registered.");
	const char *const NICK_X_NOT_REGISTERED =		_("Nick \002%s\002 isn't registered.");
	const char *const NICK_X_NOT_IN_USE =			_("Nick \002%s\002 isn't currently in use.");
	const char *const NICK_X_NOT_ON_CHAN =			_("\002%s\002 is not currently on channel %s.");
	const char *const NICK_X_FORBIDDEN =			_("Nick \002%s\002 may not be registered or used.");
	const char *const NICK_X_FORBIDDEN_OPER =		_("Nick \002%s\002 has been forbidden by %s:\n"
								"%s");
	const char *const NICK_X_SUSPENDED =			_("Nick %s is currently suspended.");
	const char *const CHAN_X_NOT_REGISTERED =		_("Channel \002%s\002 isn't registered.");
	const char *const CHAN_X_NOT_IN_USE =			_("Channel \002%s\002 doesn't exist.");
	const char *const CHAN_X_FORBIDDEN =			_("Channel \002%s\002 may not be registered or used.");
	const char *const CHAN_X_FORBIDDEN_OPER = 		_("Channel \002%s\002 has been forbidden by %s:\n" \
								"%s");
	const char *const NICK_IDENTIFY_REQUIRED =		_("Password authentication required for that command.\n" \
								"Retry after typing \002%R%s IDENTIFY \037password\037\002.");
	const char *const MAIL_X_INVALID =			_("\002%s\002 is not a valid e-mail address.");
	const char *const NO_REASON =				_("No reason");
	const char *const UNKNOWN =				_("<unknown>");
	const char *const NO_EXPIRE =				_("does not expire");
	const char *const END_OF_ANY_LIST =			_("End of \002%s\002 list.");
	const char *const LIST_INCORRECT_RANGE =		_("Incorrect range specified. The correct syntax is \002#\037from\037-\037to\037\002.");
	const char *const UNKNOWN_OPTION =			_("Unknown option \002%s\002.\n"
									"Type %R%S HELP %s for more information.");
	/* move these */
	const char *const NICK_IS_REGISTERED =			_("This nick is owned by someone else.  Please choose another.\n"
									"(If this is your nick, type \002%R%s IDENTIFY \037password\037\002.)");
	const char *const NICK_IS_SECURE =			_("This nickname is registered and protected.  If it is your\n"
									"nick, type \002%R%s IDENTIFY \037password\037\002.  Otherwise,\n"
									"please choose a different nick.");
	const char *const FORCENICKCHANGE_NOW =			_("This nickname has been registered; you may not use it.");
	const char *const NICK_CANNOT_BE_REGISTERED =		_("Nickname \002%s\002 may not be registered.");
	const char *const NICK_ALREADY_REGISTERED =		_("Nickname \002%s\002 is already registered!");
	const char *const NICK_SET_SYNTAX =			_("SET \037option\037 \037parameters\037");
	const char *const NICK_SET_DISABLED =			_("Sorry, nickname option setting is temporarily disabled.");
	const char *const NICK_SET_UNKNOWN_OPTION =		_("Unknown SET option \002%s\002.");
	const char *const NICK_SET_DISPLAY_CHANGED =		_("The new display is now \002%s\002.");
	const char *const NICK_SASET_SYNTAX =			_("SASET \037nickname\037 \037option\037 \037parameters\037");
	const char *const NICK_SASET_DISPLAY_INVALID =		_("The new display for \002%s\002 MUST be a nickname of the nickname group!");
	const char *const NICK_SASET_PASSWORD_FAILED =		_("Sorry, couldn't change password for \002%s\002.");
	const char *const NICK_SASET_PASSWORD_CHANGED =		_("Password for \002%s\002 changed.");
	const char *const NICK_SASET_PASSWORD_CHANGED_TO =	_("Password for \002%s\002 changed to \002%s\002.");
	const char *const NICK_INFO_OPTIONS =			_("          Options: %s");
	const char *const NICK_LIST_SYNTAX =			_("LIST \037pattern\037");
	const char *const LIST_HEADER =				_("List of entries matching \002%s\002:");
	const char *const NICK_RECOVERED =			_("User claiming your nick has been killed.\n"
									"\002%R%s RELEASE %s\002 to get it back before %s timeout.");
	const char *const NICK_REQUESTED =			_("This nick has already been requested, please check your e-mail address for the pass code");
	const char *const NICK_IS_PREREG =			_("This nick is awaiting an e-mail verification code before completing registration.");
	const char *const NICK_CONFIRM_INVALID =		_("Invalid passcode has been entered, please check the e-mail again, and retry");
	const char *const CHAN_NOT_ALLOWED_TO_JOIN =		_("You are not permitted to be on this channel.");
	const char *const CHAN_X_INVALID =			_("Channel %s is not a valid channel.");
	const char *const CHAN_REACHED_CHANNEL_LIMIT =		_("Sorry, you have already reached your limit of \002%d\002 channels.");
	const char *const CHAN_EXCEEDED_CHANNEL_LIMIT =		_("Sorry, you have already exceeded your limit of \002%d\002 channels.");
	const char *const CHAN_SYMBOL_REQUIRED =		_("Please use the symbol of \002#\002 when attempting to register");
	const char *const CHAN_SASET_SYNTAX =			_("SASET \002channel\002 \037option\037 \037parameters\037");
	const char *const CHAN_SET_SYNTAX =			_("SET \037channel\037 \037option\037 \037parameters\037");
	const char *const CHAN_SET_DISABLED =			_("Sorry, channel option setting is temporarily disabled.");
	const char *const CHAN_SETTING_CHANGED =		_("%s for %s set to %s.");
	const char *const CHAN_SETTING_UNSET =			_("%s for %s unset.");
	const char *const CHAN_SET_MLOCK_DEPRECATED =		_("MLOCK is deprecated. Use \002%R%s HELP MODE\002 instead.");
	const char *const CHAN_ACCESS_LEVEL_RANGE =		_("Access level must be between %d and %d inclusive.");
	const char *const CHAN_ACCESS_LIST_HEADER =		_("Access list for %s:\n"
									"  Num   Lev  Mask");
	const char *const CHAN_ACCESS_VIEW_XOP_FORMAT =		_("  %3d   %s   %s\n"
									"    by %s, last seen %s");
	const char *const CHAN_ACCESS_VIEW_AXS_FORMAT =		_("  %3d  %4d  %s\n"
									"    by %s, last seen %s");
	const char *const CHAN_AKICK_VIEW_FORMAT =		_("%3d %s (by %s on %s)\n"
									"    %s");
	const char *const CHAN_INFO_HEADER =			_("Information for channel \002%s\002:");
	const char *const CHAN_EXCEPTED =			_("\002%s\002 matches an except on %s and cannot be banned until the except have been removed.");
	const char *const CHAN_LIST_ENTRY =		_("%3d    %s\n"
									"  Added by %s on %s");
	const char *const MEMO_NEW_X_MEMO_ARRIVED =		_("There is a new memo on channel %s.\n"
									"Type \002%R%s READ %s %d\002 to read it.");
	const char *const MEMO_NEW_MEMO_ARRIVED =		_("You have a new memo from %s.\n"
									"Type \002%R%s READ %d\002 to read it.");
	const char *const MEMO_HAVE_NO_MEMOS =			_("You have no memos.");
	const char *const MEMO_X_HAS_NO_MEMOS =			_("%s has no memos.");
	const char *const MEMO_SEND_SYNTAX =			_("SEND {\037nick\037 | \037channel\037} \037memo-text\037");
	const char *const MEMO_SEND_DISABLED =			_("Sorry, memo sending is temporarily disabled.");
	const char *const MEMO_HAVE_NO_NEW_MEMOS =		_("You have no new memos.");
	const char *const MEMO_X_HAS_NO_NEW_MEMOS =		_("%s has no new memos.");
	const char *const BOT_DOES_NOT_EXIST =			_("Bot \002%s\002 does not exist.");
	const char *const BOT_NOT_ASSIGNED =			_("You must assign a bot to the channel before using this command.\n"
									"Type %R%S HELP ASSIGN for more information.");
	const char *const BOT_NOT_ON_CHANNEL =			_("Bot is not on channel \002%s\002.");
	const char *const BOT_ASSIGN_READONLY =			_("Sorry, bot assignment is temporarily disabled.");
	const char *const ENABLED =				_("Enabled");
	const char *const DISABLED =				_("Disabled");
	const char *const OPER_LIST_FORMAT =			_("  %3d   %-32s  %s");
	const char *const OPER_VIEW_FORMAT =			_("%3d  %s (by %s on %s; %s)\n"
									"      %s");
	const char *const HOST_SET_ERROR =			_("A vhost must be in the format of a valid hostmask.");
	const char *const HOST_SET_IDENT_ERROR =		_("A vhost ident must be in the format of a valid ident");
	const char *const HOST_SET_TOOLONG =			_("Error! The vhost is too long, please use a host shorter than %d characters.");
	const char *const HOST_SET_IDENTTOOLONG =		_("Error! The Ident is too long, please use an ident shorter than %d characters.");
	const char *const HOST_NOT_ASSIGNED =			_("Please contact an Operator to get a vhost assigned to this nick.");
	const char *const HOST_NO_VIDENT =			_("Your IRCD does not support vIdent's, if this is incorrect, please report this as a possible bug");
}

