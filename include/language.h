/* Commonly used language strings
 *
 * (C) 2008-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#define MORE_INFO			_("\002%s%s HELP %s\002 for more information.")
#define BAD_USERHOST_MASK		_("Mask must be in the form \037user\037@\037host\037.")
#define BAD_EXPIRY_TIME			_("Invalid expiry time.")
#define USERHOST_MASK_TOO_WIDE		_("%s coverage is too wide; Please use a more specific mask.")
#define READ_ONLY_MODE			_("Services are in read-only mode!")
#define PASSWORD_INCORRECT		_("Password incorrect.")
#define ACCESS_DENIED			_("Access denied.")
#define MORE_OBSCURE_PASSWORD		_("Please try again with a more obscure password. Passwords should be at least\n" \
						"five characters long, should not be something easily guessed\n" \
						"(e.g. your real name or your nick), and cannot contain the space or tab characters.")
#define PASSWORD_TOO_LONG		_("Your password is too long. Please try again with a shorter password.")
#define NICK_NOT_REGISTERED		_("Your nick isn't registered.")
#define NICK_X_NOT_REGISTERED		_("Nick \002%s\002 isn't registered.")
#define NICK_X_NOT_IN_USE		_("Nick \002%s\002 isn't currently in use.")
#define NICK_X_NOT_ON_CHAN		_("\002%s\002 is not currently on channel %s.")
#define NICK_X_SUSPENDED		_("Nick %s is currently suspended.")
#define CHAN_X_SUSPENDED		_("Channel %s is currently suspended.")
#define CHAN_X_NOT_REGISTERED		_("Channel \002%s\002 isn't registered.")
#define CHAN_X_NOT_IN_USE		_("Channel \002%s\002 doesn't exist.")
#define NICK_IDENTIFY_REQUIRED		_("Password authentication required for that command.")
#define MAIL_X_INVALID			_("\002%s\002 is not a valid e-mail address.")
#define NO_REASON			_("No reason")
#define UNKNOWN				_("<unknown>")
#define NO_EXPIRE			_("does not expire")
#define LIST_INCORRECT_RANGE		_("Incorrect range specified. The correct syntax is \002#\037from\037-\037to\037\002.")
#define UNKNOWN_OPTION			_("Unknown option \002%s\002.\n" \
						"Type %s%s HELP %s for more information.")
#define NICK_IS_REGISTERED		_("This nick is owned by someone else.  Please choose another.\n" \
						"(If this is your nick, type \002%s%s IDENTIFY \037password\037\002.)")
#define NICK_IS_SECURE			_("This nickname is registered and protected.  If it is your\n" \
						"nick, type \002%s%s IDENTIFY \037password\037\002.  Otherwise,\n" \
						"please choose a different nick.")
#define FORCENICKCHANGE_NOW		_("This nickname has been registered; you may not use it.")
#define NICK_CANNOT_BE_REGISTERED	_("Nickname \002%s\002 may not be registered.")
#define NICK_ALREADY_REGISTERED		_("Nickname \002%s\002 is already registered!")
#define NICK_SET_SYNTAX			_("SET \037option\037 \037parameters\037")
#define NICK_SET_DISABLED		_("Sorry, nickname option setting is temporarily disabled.")
#define NICK_SET_UNKNOWN_OPTION		_("Unknown SET option \002%s%s\002.")
#define NICK_SET_DISPLAY_CHANGED	_("The new display is now \002%s\002.")
#define NICK_LIST_SYNTAX		_("LIST \037pattern\037")
#define NICK_RECOVERED			_("User claiming your nick has been killed.\n" \
						"\002%s%s RELEASE %s\002 to get it back before %s timeout.")
#define NICK_REQUESTED			_("This nick has already been requested, please check your e-mail address for the pass code")
#define NICK_CONFIRM_INVALID		_("Invalid passcode has been entered, please check the e-mail again, and retry")
#define CHAN_NOT_ALLOWED_TO_JOIN	_("You are not permitted to be on this channel.")
#define CHAN_X_INVALID			_("Channel %s is not a valid channel.")
#define CHAN_REACHED_CHANNEL_LIMIT	_("Sorry, you have already reached your limit of \002%d\002 channels.")
#define CHAN_EXCEEDED_CHANNEL_LIMIT	_("Sorry, you have already exceeded your limit of \002%d\002 channels.")
#define CHAN_SYMBOL_REQUIRED		_("Please use the symbol of \002#\002 when attempting to register")
#define CHAN_SET_DISABLED		_("Sorry, channel option setting is temporarily disabled.")
#define CHAN_SETTING_CHANGED		_("%s for %s set to %s.")
#define CHAN_SETTING_UNSET		_("%s for %s unset.")
#define CHAN_SET_MLOCK_DEPRECATED	_("MLOCK is deprecated. Use \002%s%s HELP MODE\002 instead.")
#define CHAN_ACCESS_LEVEL_RANGE		_("Access level must be between %d and %d inclusive.")
#define CHAN_INFO_HEADER		_("Information for channel \002%s\002:")
#define CHAN_EXCEPTED			_("\002%s\002 matches an except on %s and cannot be banned until the except have been removed.")
#define MEMO_NEW_X_MEMO_ARRIVED		_("There is a new memo on channel %s.\n" \
						"Type \002%s%s READ %s %d\002 to read it.")
#define MEMO_NEW_MEMO_ARRIVED		_("You have a new memo from %s.\n" \
						"Type \002%s%s READ %d\002 to read it.")
#define MEMO_HAVE_NO_MEMOS		_("You have no memos.")
#define MEMO_X_HAS_NO_MEMOS		_("%s has no memos.")
#define MEMO_SEND_SYNTAX		_("SEND {\037nick\037 | \037channel\037} \037memo-text\037")
#define MEMO_SEND_DISABLED		_("Sorry, memo sending is temporarily disabled.")
#define MEMO_HAVE_NO_NEW_MEMOS		_("You have no new memos.")
#define MEMO_X_HAS_NO_NEW_MEMOS		_("%s has no new memos.")
#define BOT_DOES_NOT_EXIST		_("Bot \002%s\002 does not exist.")
#define BOT_NOT_ASSIGNED		_("You must assign a bot to the channel before using this command.")
#define BOT_NOT_ON_CHANNEL		_("Bot is not on channel \002%s\002.")
#define BOT_ASSIGN_READONLY		_("Sorry, bot assignment is temporarily disabled.")
#define ENABLED				_("Enabled")
#define DISABLED			_("Disabled")
#define HOST_SET_ERROR			_("A vhost must be in the format of a valid hostmask.")
#define HOST_SET_IDENT_ERROR		_("A vhost ident must be in the format of a valid ident")
#define HOST_SET_TOOLONG		_("Error! The vhost is too long, please use a host shorter than %d characters.")
#define HOST_SET_IDENTTOOLONG		_("Error! The Ident is too long, please use an ident shorter than %d characters.")
#define HOST_NOT_ASSIGNED		_("Please contact an Operator to get a vhost assigned to this nick.")
#define HOST_NO_VIDENT			_("Your IRCD does not support vIdent's, if this is incorrect, please report this as a possible bug")

