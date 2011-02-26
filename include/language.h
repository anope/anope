/* Commonly used language strings
 *
 * (C) 2008-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#define MORE_INFO			"\002%R%s HELP %s\002 for more information."
#define BAD_USERHOST_MASK		"Mask must be in the form \037user\037@\037host\037."
#define BAD_EXPIRY_TIME			"Invalid expiry time."
#define USERHOST_MASK_TOO_WIDE		"%s coverage is too wide; Please use a more specific mask."
#define READ_ONLY_MODE			"\002Notice:\002 Services is in read-only mode; changes will not be saved!"
#define PASSWORD_INCORRECT			"Password incorrect."
#define ACCESS_DENIED			"Access denied."
#define MORE_OBSCURE_PASSWORD		"Please try again with a more obscure password. Passwords should be at least five characters long, should not be something easily guessed (e.g. your real name or your nick), and cannot contain the space or tab characters."
#define PASSWORD_TOO_LONG		"Your password is too long. Please try again with a shorter password."
#define NICK_NOT_REGISTERED		"Your nick isn't registered."
#define NICK_X_NOT_REGISTERED		"Nick \002%s\002 isn't registered."
#define NICK_X_NOT_IN_USE		"Nick \002%s\002 isn't currently in use."
#define NICK_X_NOT_ON_CHAN		"\002%s\002 is not currently on channel %s."
#define NICK_X_FORBIDDEN		"Nick \002%s\002 may not be registered or used."
#define NICK_X_FORBIDDEN_OPER		"Nick \002%s\002 has been forbidden by %s:\n" \
						"%s"
#define NICK_X_SUSPENDED			"Nick %s is currently suspended."
#define CHAN_X_NOT_REGISTERED		"Channel \002%s\002 isn't registered."
#define CHAN_X_NOT_IN_USE		"Channel \002%s\002 doesn't exist."
#define CHAN_X_FORBIDDEN		"Channel \002%s\002 may not be registered or used."
#define CHAN_X_FORBIDDEN_OPER		"Channel \002%s\002 has been forbidden by %s:\n" \
						"%s"
#define NICK_IDENTIFY_REQUIRED		"Password authentication required for that command.\n" \
						"Retry after typing \002%R%s IDENTIFY \037password\037\002."
#define MAIL_X_INVALID			"\002%s\002 is not a valid e-mail address."
#define NO_REASON			"No reason"
#define UNKNOWN				"<unknown>"
#define NO_EXPIRE			"does not expire"
#define END_OF_ANY_LIST			"End of \002%s\002 list."
#define LIST_INCORRECT_RANGE		"Incorrect range specified. The correct syntax is \002#\037from\037-\037to\037\002."
#define UNKNOWN_OPTION			"Unknown option \002%s\002.\n" \
						"Type %R%S HELP %s for more information."
#define NICK_IS_REGISTERED		"This nick is owned by someone else.  Please choose another.\n" \
						"(If this is your nick, type \002%R%s IDENTIFY \037password\037\002.)"
#define NICK_IS_SECURE			"This nickname is registered and protected.  If it is your\n" \
						"nick, type \002%R%s IDENTIFY \037password\037\002.  Otherwise,\n" \
						"please choose a different nick."
#define FORCENICKCHANGE_NOW		"This nickname has been registered; you may not use it."
#define NICK_CANNOT_BE_REGISTERED	"Nickname \002%s\002 may not be registered."
#define NICK_ALREADY_REGISTERED		"Nickname \002%s\002 is already registered!"
#define NICK_SET_SYNTAX			"SET \037option\037 \037parameters\037"
#define NICK_SET_DISABLED		"Sorry, nickname option setting is temporarily disabled."
#define NICK_SET_UNKNOWN_OPTION		"Unknown SET option \002%s\002."
#define NICK_SET_DISPLAY_CHANGED	"The new display is now \002%s\002."
#define NICK_SASET_SYNTAX		"SASET \037nickname\037 \037option\037 \037parameters\037"
#define NICK_SASET_DISPLAY_INVALID	"The new display for \002%s\002 MUST be a nickname of the nickname group!"
#define NICK_SASET_PASSWORD_FAILED	"Sorry, couldn't change password for \002%s\002."
#define NICK_SASET_PASSWORD_CHANGED	"Password for \002%s\002 changed."
#define NICK_SASET_PASSWORD_CHANGED_TO	"Password for \002%s\002 changed to \002%s\002."
#define NICK_INFO_OPTIONS		"          Options: %s"
#define NICK_LIST_SYNTAX		"LIST \037pattern\037"
#define LIST_HEADER			"List of entries matching \002%s\002:"
#define NICK_RECOVERED			"User claiming your nick has been killed.\n" \
						"\002%R%s RELEASE %s\002 to get it back before %s timeout."
#define NICK_REQUESTED			"This nick has already been requested, please check your e-mail address for the pass code"
#define NICK_IS_PREREG			"This nick is awaiting an e-mail verification code before completing registration."
#define NICK_CONFIRM_INVALID		"Invalid passcode has been entered, please check the e-mail again, and retry"
#define CHAN_NOT_ALLOWED_TO_JOIN	"You are not permitted to be on this channel."
#define CHAN_X_INVALID			"Channel %s is not a valid channel."
#define CHAN_REACHED_CHANNEL_LIMIT	"Sorry, you have already reached your limit of \002%d\002 channels."
#define CHAN_EXCEEDED_CHANNEL_LIMIT	"Sorry, you have already exceeded your limit of \002%d\002 channels."
#define CHAN_SYMBOL_REQUIRED		"Please use the symbol of \002#\002 when attempting to register"
#define CHAN_SASET_SYNTAX		"SASET \002channel\002 \037option\037 \037parameters\037"
#define CHAN_SET_SYNTAX			"SET \037channel\037 \037option\037 \037parameters\037"
#define CHAN_SET_DISABLED		"Sorry, channel option setting is temporarily disabled."
#define CHAN_SETTING_CHANGED		"%s for %s set to %s."
#define CHAN_SETTING_UNSET		"%s for %s unset."
#define CHAN_SET_MLOCK_DEPRECATED	"MLOCK is deprecated. Use \002%R%s HELP MODE\002 instead."
#define CHAN_ACCESS_LEVEL_RANGE		"Access level must be between %d and %d inclusive."
#define CHAN_ACCESS_LIST_HEADER		"Access list for %s:\n" \
						"  Num   Lev  Mask"
#define CHAN_ACCESS_VIEW_XOP_FORMAT	"  %3d   %s   %s\n" \
						"    by %s, last seen %s"
#define CHAN_ACCESS_VIEW_AXS_FORMAT	"  %3d  %4d  %s\n" \
						"    by %s, last seen %s"
#define CHAN_AKICK_VIEW_FORMAT		"%3d %s (by %s on %s)\n" \
						"    %s"
#define CHAN_INFO_HEADER		"Information for channel \002%s\002:"
#define CHAN_EXCEPTED			"\002%s\002 matches an except on %s and cannot be banned until the except have been removed."
#define CHAN_LIST_ENTRY			"%3d    %s\n" \
						"  Added by %s on %s"
#define MEMO_NEW_X_MEMO_ARRIVED		"There is a new memo on channel %s.\n" \
						"Type \002%R%s READ %s %d\002 to read it."
#define MEMO_NEW_MEMO_ARRIVED		"You have a new memo from %s.\n" \
						"Type \002%R%s READ %d\002 to read it."
#define MEMO_HAVE_NO_MEMOS		"You have no memos."
#define MEMO_X_HAS_NO_MEMOS		"%s has no memos."
#define MEMO_SEND_SYNTAX		"SEND {\037nick\037 | \037channel\037} \037memo-text\037"
#define MEMO_SEND_DISABLED		"Sorry, memo sending is temporarily disabled."
#define MEMO_HAVE_NO_NEW_MEMOS		"You have no new memos."
#define MEMO_X_HAS_NO_NEW_MEMOS		"%s has no new memos."
#define BOT_DOES_NOT_EXIST		"Bot \002%s\002 does not exist."
#define BOT_NOT_ASSIGNED		"You must assign a bot to the channel before using this command.\n" \
						"Type %R%S HELP ASSIGN for more information."
#define BOT_NOT_ON_CHANNEL		"Bot is not on channel \002%s\002."
#define BOT_ASSIGN_READONLY		"Sorry, bot assignment is temporarily disabled."
#define ENABLED				"Enabled"
#define DISABLED			"Disabled"
#define OPER_LIST_FORMAT		"  %3d   %-32s  %s"
#define OPER_VIEW_FORMAT		"%3d  %s (by %s on %s; %s)\n" \
						"      %s"
#define HOST_SET_ERROR			"A vhost must be in the format of a valid hostmask."
#define HOST_SET_IDENT_ERROR		"A vhost ident must be in the format of a valid ident"
#define HOST_SET_TOOLONG		"Error! The vhost is too long, please use a host shorter than %d characters."
#define HOST_SET_IDENTTOOLONG		"Error! The Ident is too long, please use an ident shorter than %d characters."
#define HOST_NOT_ASSIGNED		"Please contact an Operator to get a vhost assigned to this nick."
#define HOST_NO_VIDENT			"Your IRCD does not support vIdent's, if this is incorrect, please report this as a possible bug"

