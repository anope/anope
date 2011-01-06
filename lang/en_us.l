# United States English language file.
#
# (C) 2003-2011 Anope Team
# Contact us at team@anope.org
#   
# Please read COPYING and README for further details.
#     
# Based on the original code of Epona by Lara.
# Based on the original code of Services by Andy Church. 
#
# When translating this file to another language, keep in mind that the
# order of parameters for sprintf() is fixed in the source code, so your
# messages need to take the same parameters in the same order as the
# English messages do.  (Obviously, this doesn't hold for the strftime()
# format lines immediately below.)  If you can't get a natural translation
# of a message without changing the order of the parameters, let us know
# (team@anope.org) which message is causing a problem and I'll see
# what I can do.
#
# In help messages, "%S" (capital S, not lowercase) refers to the name of
# the service sending the message; for example, in NickServ help messages,
# "%S" is replaced by "NickServ" (or whatever it is renamed to in
# services.conf).  The %S's do not count as sprintf() parameters, so they can be
# rearranged, removed, or added as necessary.
#
# Also in help messages, please try to limit line lengths to 60 characters
# of text (not including the leading tab).  This length was chosen because
# it does not cause line wrap under default settings on most current IRC
# clients.  Remember that format characters (control-B, control-_) are not
# included in that 60-character limit (since they don't show on the user's
# screen).  Also remember that format specifiers (%S, etc.) will expand
# when displayed, so remember to take this into account; you can assume
# that the length of a pseudoclient name (%S replacement) will be eight
# characters, so reduce the maximum line length by 6 for every %S on a
# line.
#
# Finally, remember to put a tab at the beginning of every line of text
# (including empty lines).  This has to be a tab, not spaces.

###########################################################################
#
# Name of this language
#
###########################################################################

# For languages other than English, this string should have the following
# format:
#     language-name-in-language (language-name-in-English)
# For example, "Espa�l (Spanish)" or "Fran�is (French)".

LANG_NAME
	English

###########################################################################
#
# General messages
#
###########################################################################

# strftime() format strings.  man 3 strftime for information on the
# meanings of the format specifiers.  Short version:
#	%a = weekday name (short)	%H = hour
#	%b = month name (short)		%M = minute
#	%d = day of month		%S = second
#	%Y = year			%Z = time zone

# This is used as the format string for strftime() for a date and time
# together.
STRFTIME_DATE_TIME_FORMAT
	%b %d %H:%M:%S %Y %Z
# This is used as the format string for strftime() for a date alone in long
# format (English: including weekday).
STRFTIME_LONG_DATE_FORMAT
	%a %b %d %Y
# This is used as the format string for strftime() for a date alone in
# short format (English: without weekday).
STRFTIME_SHORT_DATE_FORMAT
	%b %d %Y

# These tell our strftime() what the names of months and days are.  If you
# don't use %a, %A, %b, or %B in your strftime() strings above, you can
# leave these empty.  However, if you enter names, they MUST stay in order,
# one per line, and the list MUST be complete!

# %a
STRFTIME_DAYS_SHORT
	Sun
	Mon
	Tue
	Wed
	Thu
	Fri
	Sat
# %A
STRFTIME_DAYS_LONG
	Sunday
	Monday
	Tuesday
	Wednesday
	Thursday
	Friday
	Saturday
# %b
STRFTIME_MONTHS_SHORT
	Jan
	Feb
	Mar
	Apr
	May
	Jun
	Jul
	Aug
	Sep
	Oct
	Nov
	Dec
# %B
STRFTIME_MONTHS_LONG
	January
	February
	March
	April
	May
	June
	July
	August
	September
	October
	November
	December


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	, 


# Various error messages.
USER_RECORD_NOT_FOUND
	Internal error - unable to process request.
UNKNOWN_COMMAND
	Unknown command %s.
UNKNOWN_COMMAND_HELP
	Unknown command %s.  "%R%s HELP" for help.
SYNTAX_ERROR
	Syntax: %s
MORE_INFO
	%R%s HELP %s for more information.
NO_HELP_AVAILABLE
	No help available for %s.
OBSOLETE_COMMAND
	This command is obsolete; use %s instead.

BAD_USERHOST_MASK
	Mask must be in the form user@host.
BAD_EXPIRY_TIME
	Invalid expiry time.
USERHOST_MASK_TOO_WIDE
	%s coverage is too wide; Please use a more specific mask.

SERVICE_OFFLINE
	%s is currently offline.
READ_ONLY_MODE
	Notice: Services is in read-only mode; changes will not be saved!
PASSWORD_INCORRECT
	Password incorrect.
INVALID_TARGET
	"/msg %s" is no longer supported.  Use "/msg %s@%s" or "/%s" instead.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SASET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Access denied.
PERMISSION_DENIED
	Permission denied.
RAW_DISABLED
	The RAW option has been disabled. If you must use it, configure the DisableRaw directive in Services configuration.

MORE_OBSCURE_PASSWORD
	Please try again with a more obscure password.  Passwords should be at least five characters long, should not be something easily guessed (e.g. your real name or your nick), and cannot contain the space or tab characters.
PASSWORD_TOO_LONG
	Your password is too long. Please try again with a shorter password.

NICK_NOT_REGISTERED
	Your nick isn't registered.
NICK_NOT_REGISTERED_HELP
	Your nick isn't registered.  Type %R%s HELP for information on registering your nickname.
NICK_X_IS_SERVICES
	Nick %s is part of this Network's Services.
NICK_X_NOT_REGISTERED
	Nick %s isn't registered.
NICK_X_IN_USE
	Nick %s is currently in use.
NICK_X_NOT_IN_USE
	Nick %s isn't currently in use.
NICK_X_NOT_ON_CHAN
	%s is not currently on channel %s.
NICK_X_FORBIDDEN
	Nick %s may not be registered or used.
NICK_X_FORBIDDEN_OPER
	Nick %s has been forbidden by %s:
	%s
NICK_X_ILLEGAL
	Nick %s is an illegal nickname and cannot be used.
NICK_X_TRUNCATED
	Nick %s was truncated to %d characters.
NICK_X_SUSPENDED
	Nick %s is currently suspended.
CHAN_X_NOT_REGISTERED
	Channel %s isn't registered.
CHAN_X_NOT_IN_USE
	Channel %s doesn't exist.
CHAN_X_FORBIDDEN
	Channel %s may not be registered or used.
CHAN_X_FORBIDDEN_OPER
	Channel %s has been forbidden by %s:
	%s
CHAN_X_SUSPENDED
	      Suspended: [%s] %s
NICK_IDENTIFY_REQUIRED
	Password authentication required for that command.
	Retry after typing %R%s IDENTIFY password.
CHAN_IDENTIFY_REQUIRED
	Password authentication required for that command.
	Retry after typing %R%s IDENTIFY %s password.
	
MAIL_DISABLED
	Services have been configured to not send mail.
MAIL_INVALID
	E-mail for %s is invalid.
MAIL_X_INVALID
	%s is not a valid e-mail address.
MAIL_LATER
	Cannot send mail now; please retry a little later.
MAIL_DELAYED
	Please wait %d seconds and retry.
	
NO_REASON
	No reason
UNKNOWN
	<unknown>

# Duration system
DURATION_DAY
	1 day
DURATION_DAYS
	%d days
DURATION_HOUR
	1 hour
DURATION_HOURS
	%d hours
DURATION_MINUTE
	1 minute
DURATION_MINUTES
	%d minutes
DURATION_SECOND
	1 second
DURATION_SECONDS
	%d seconds

# Human readable expiration
NO_EXPIRE
	does not expire
EXPIRES_SOON
	expires at next database update
EXPIRES_M
	expires in %d minutes
EXPIRES_1M
	expires in %d minute
EXPIRES_HM
	expires in %d hours, %d minutes
EXPIRES_H1M
	expires in %d hours, %d minute
EXPIRES_1HM
	expires in %d hour, %d minutes
EXPIRES_1H1M
	expires in %d hour, %d minute
EXPIRES_D
	expires in %d days
EXPIRES_1D
	expires in %d day

# Generic Footer message
END_OF_ANY_LIST
	End of %s list.

# Generic List error messages
LIST_INCORRECT_RANGE
	Incorrect range specified. The correct syntax is #from-to.
CS_LIST_INCORRECT_RANGE
	To search for channels starting with #, search for the channel
	name without the #-sign prepended (anope instead of #anope).

# Generic help limited to messages
HELP_LIMIT_SERV_OPER
	
	Limited to Services Operators.
HELP_LIMIT_SERV_ADMIN
	
	Limited to Services Administrators.
HELP_LIMIT_SERV_ROOT
	
	Limited to Services Roots.
HELP_LIMIT_IRC_OPER
	
	Limited to IRC Operators.
HELP_LIMIT_HOST_SETTER
	
	Limited to Host Setters.
HELP_LIMIT_HOST_REMOVER
	
	Limited to Host Removers.


###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	This nick is owned by someone else.  Please choose another.
	(If this is your nick, type %R%s IDENTIFY password.)
NICK_IS_SECURE
	This nickname is registered and protected.  If it is your
	nick, type %R%s IDENTIFY password.  Otherwise,
	please choose a different nick.
NICK_MAY_NOT_BE_USED
	This nickname may not be used.  Please choose another one.
FORCENICKCHANGE_IN_1_MINUTE
	If you do not change within one minute, I will change your nick.
FORCENICKCHANGE_IN_20_SECONDS
	If you do not change within 20 seconds, I will change your nick.
FORCENICKCHANGE_NOW
	This nickname has been registered; you may not use it.
FORCENICKCHANGE_CHANGING
	Your nickname is now being changed to %s

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER password [email]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER password email
NICK_REGISTRATION_DISABLED
	Sorry, nickname registration is temporarily disabled.
NICK_REGISTRATION_FAILED
	Sorry, registration failed.
NICK_REG_PLEASE_WAIT
	Please wait %d seconds before using the REGISTER command again.
NICK_CANNOT_BE_REGISTERED
	Nickname %s may not be registered.
NICK_ALREADY_REGISTERED
	Nickname %s is already registered!
NICK_REGISTERED
	Nickname %s registered under your account: %s
NICK_REGISTERED_NO_MASK
	Nickname %s registered.
NICK_PASSWORD_IS
	Your password is %s - remember this for later use.
NICK_REG_DELAY
	You must have been using this nick for at least %d seconds to register.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP target password
NICK_GROUP_DISABLED
	Sorry, nickname grouping is temporarily disabled.
NICK_GROUP_FAILED
	Sorry, grouping failed.
NICK_GROUP_PLEASE_WAIT
	Please wait %d seconds before using the GROUP command again.
NICK_GROUP_CHANGE_DISABLED
	Your nick is already registered; type %R%s DROP first.
NICK_GROUP_SAME
	You are already a member of the group of %s.
NICK_GROUP_TOO_MANY
	There are too many nicks in %s's group; list them and drop some.
	Type %R%s HELP GLIST and %R%s HELP DROP
	for more information.
NICK_GROUP_JOINED
	You are now in the group of %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY password
NICK_IDENTIFY_FAILED
	Sorry, identification failed.
NICK_IDENTIFY_SUCCEEDED
	Password accepted - you are now recognized.
NICK_IDENTIFY_EMAIL_REQUIRED
	You must now supply an e-mail for your nick.
	This e-mail will allow you to retrieve your password in
	case you forget it.
NICK_IDENTIFY_EMAIL_HOWTO
	Type %R%S SET EMAIL e-mail in order to set your e-mail.
	Your privacy is respected; this e-mail won't be given to
	any third-party person.
NICK_ALREADY_IDENTIFIED
	You are already identified.

# UPDATE responses
NICK_UPDATE_SUCCESS
	Status updated (memos, vhost, chmodes, flags).
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	Your nick has been logged out.
NICK_LOGOUT_X_SUCCEEDED
	Nick %s has been logged out.
NICK_LOGOUT_SERVICESADMIN
	Can't logout %s because he's a services administrator.

# DROP responses
NICK_DROP_DISABLED
	Sorry, nickname de-registration is temporarily disabled.
NICK_DROPPED
	Your nickname has been dropped.
NICK_X_DROPPED
	Nickname %s has been dropped.

# SET responses
NICK_SET_SYNTAX
	SET option parameters
NICK_SET_SERVADMIN_SYNTAX
	SET [nick] option parameters
NICK_SET_DISABLED
	Sorry, nickname option setting is temporarily disabled.
NICK_SET_UNKNOWN_OPTION
	Unknown SET option %s.
NICK_SET_OPTION_DISABLED
	Option %s cannot be set on this network.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	The new display MUST be a nickname of your nickname group!
NICK_SET_DISPLAY_CHANGED
	The new display is now %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Sorry, couldn't change password.
NICK_SET_PASSWORD_CHANGED
	Password changed.
NICK_SET_PASSWORD_CHANGED_TO
	Password changed to %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE number
NICK_SET_LANGUAGE_UNKNOWN
	Unknown language number %d.  Type %R%s HELP SET LANGUAGE for a list of languages.
NICK_SET_LANGUAGE_CHANGED
	Language changed to English.

# SET URL responses
NICK_SET_URL_CHANGED
	URL changed to %s.
NICK_SET_URL_UNSET
	URL unset.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	E-mail address changed to %s.
NICK_SET_EMAIL_UNSET
	E-mail address unset.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	You cannot unset the e-mail on this network.
	
# SET ICQ responses
NICK_SET_ICQ_CHANGED
	ICQ number set to %s.
NICK_SET_ICQ_UNSET
	ICQ number unset.
NICK_SET_ICQ_INVALID
	%s is not a valid number.
	
# SET GREET responses
NICK_SET_GREET_CHANGED
	Greet message changed to %s.
NICK_SET_GREET_UNSET
	Greet message unset.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Protection is now ON.
NICK_SET_KILL_QUICK
	Protection is now ON, with a reduced delay.
NICK_SET_KILL_IMMED
	Protection is now ON, with no delay.
NICK_SET_KILL_IMMED_DISABLED
	The IMMED option is not available on this network.
NICK_SET_KILL_OFF
	Protection is now OFF.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Secure option is now ON.
NICK_SET_SECURE_OFF
	Secure option is now OFF.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Private option is now ON.
NICK_SET_PRIVATE_OFF
	Private option is now OFF.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Your E-mail address will now be hidden from %s INFO displays.
NICK_SET_HIDE_EMAIL_OFF
	Your E-mail address will now be shown in %s INFO displays.
NICK_SET_HIDE_MASK_ON
	Your last seen user@host mask will now be hidden from %s INFO displays.
NICK_SET_HIDE_MASK_OFF
	Your last seen user@host mask will now be shown in %s INFO displays.
NICK_SET_HIDE_QUIT_ON
	Your last quit message will now be hidden from %s INFO displays.
NICK_SET_HIDE_QUIT_OFF
	Your last quit message will now be shown in %s INFO displays.
NICK_SET_HIDE_STATUS_ON
	Your services access status will now be hidden from %s INFO displays.
NICK_SET_HIDE_STATUS_OFF
	Your services access status will now be shown in %s INFO displays.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET MSG {ON | OFF}
NICK_SET_MSG_ON
	Services will now reply to you with messages.
NICK_SET_MSG_OFF
	Services will now reply to you with notices.

# SET AUTOOP responses
NICK_SET_AUTOOP_SYNTAX
	SET AUTOOP {ON | OFF}
NICK_SET_AUTOOP_ON
	Services will now autoop you in channels.
NICK_SET_AUTOOP_OFF
	Services will no longer autoop you in channels.


# SASET responses
NICK_SASET_SYNTAX
	SASET nickname option parameters
NICK_SASET_DISABLED
	Sorry, nickname option setting is temporarily disabled.
NICK_SASET_UNKNOWN_OPTION
	Unknown SASET option %s.
NICK_SASET_BAD_NICK
	Nickname %s not registered.
NICK_SASET_OPTION_DISABLED
	Option %s cannot be set on this network.

# SASET DISPLAY responses
NICK_SASET_DISPLAY_INVALID
	The new display for %s MUST be a nickname of the nickname group!
NICK_SASET_DISPLAY_CHANGED
	The new display is now %s.

# SASET PASSWORD responses
NICK_SASET_PASSWORD_FAILED
	Sorry, couldn't change password for %s.
NICK_SASET_PASSWORD_CHANGED
	Password for %s changed.
NICK_SASET_PASSWORD_CHANGED_TO
	Password for %s changed to %s.

# SASET URL responses
NICK_SASET_URL_CHANGED
	URL for %s changed to %s.
NICK_SASET_URL_UNSET
	URL %s unset.

# SASET EMAIL responses
NICK_SASET_EMAIL_CHANGED
	E-mail address for %s changed to %s.
NICK_SASET_EMAIL_UNSET
	E-mail address for %s unset.
NICK_SASET_EMAIL_UNSET_IMPOSSIBLE
	You cannot unset the e-mail on this network.
	
# SASET ICQ responses
NICK_SASET_ICQ_CHANGED
	ICQ number for %s set to %s.
NICK_SASET_ICQ_UNSET
	ICQ number for %s unset.
NICK_SASET_ICQ_INVALID
	%s is not a valid number.
	
# SASET GREET responses
NICK_SASET_GREET_CHANGED
	Greet message for %s changed to %s.
NICK_SASET_GREET_UNSET
	Greet message for %s unset.

# SASET PROTECT responses
NICK_SASET_KILL_SYNTAX
	SASET nickname KILL {ON | QUICK | OFF}
NICK_SASET_KILL_IMMED_SYNTAX
	SASET nickname KILL {ON | QUICK | IMMED | OFF}
NICK_SASET_KILL_ON
	Protection is now ON for %s.
NICK_SASET_KILL_QUICK
	Protection is now ON for %s, with a reduced delay.
NICK_SASET_KILL_IMMED
	Protection is now ON for %s, with no delay.
NICK_SASET_KILL_IMMED_DISABLED
	The IMMED option is not available on this network.
NICK_SASET_KILL_OFF
	Protection is now OFF for %s.

# SASET SECURE responses
NICK_SASET_SECURE_SYNTAX
	SASET nickname SECURE {ON | OFF}
NICK_SASET_SECURE_ON
	Secure option is now ON for %s.
NICK_SASET_SECURE_OFF
	Secure option is now OFF for %s.

# SASET PRIVATE responses
NICK_SASET_PRIVATE_SYNTAX
	SASET nickname PRIVATE {ON | OFF}
NICK_SASET_PRIVATE_ON
	Private option is now ON for %s.
NICK_SASET_PRIVATE_OFF
	Private option is now OFF for %s.

# SASET HIDE responses
NICK_SASET_HIDE_SYNTAX
	SASET nickname HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SASET_HIDE_EMAIL_ON
	The E-mail address of %s will now be hidden from %s INFO displays.
NICK_SASET_HIDE_EMAIL_OFF
	The E-mail address of %s will now be shown in %s INFO displays.
NICK_SASET_HIDE_MASK_ON
	The last seen user@host mask of %s will now be hidden from %s INFO displays.
NICK_SASET_HIDE_MASK_OFF
	The last seen user@host mask of %s will now be shown in %s INFO displays.
NICK_SASET_HIDE_QUIT_ON
	The last quit message of %s will now be hidden from %s INFO displays.
NICK_SASET_HIDE_QUIT_OFF
	The last quit message of %s will now be shown in %s INFO displays.
NICK_SASET_HIDE_STATUS_ON
	The services access status of %s will now be hidden from %s INFO displays.
NICK_SASET_HIDE_STATUS_OFF
	The services access status of %s will now be shown in %s INFO displays.
	
# SASET MSG responses
NICK_SASET_MSG_SYNTAX
	SASAET nickname MSG {ON | OFF}
NICK_SASET_MSG_ON
	Services will now reply to %s with messages.
NICK_SASET_MSG_OFF
	Services will now reply to %s with notices.

# SASET NOEXPIRE responses
NICK_SASET_NOEXPIRE_SYNTAX
	SASET nickname NOEXPIRE {ON | OFF}
NICK_SASET_NOEXPIRE_ON
	Nick %s will not expire.
NICK_SASET_NOEXPIRE_OFF
	Nick %s will expire.

# SASET AUTOOP responses
NICK_SASET_AUTOOP_SYNTAX
	SASET nickname AUTOOP {ON | OFF}
NICK_SASET_AUTOOP_ON
	Services will now autoop %s in channels.
NICK_SASET_AUTOOP_OFF
	Services will no longer autoop %s in channels.

# SASET LANGUAGE responses
NICK_SASET_LANGUAGE_SYNTAX
	SASET nickname LANGUAGE number
NICK_SASET_LANGUAGE_UNKNOWN
	Unknown language number %d.  Type %R%s HELP SET LANGUAGE for a list of languages.
NICK_SASET_LANGUAGE_CHANGED
	Language for %s changed to %s.


# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [mask]
NICK_ACCESS_ALREADY_PRESENT
	Mask %s already present on your access list.
NICK_ACCESS_REACHED_LIMIT
	Sorry, you can only have %d access entries for a nickname.
NICK_ACCESS_ADDED
	%s added to your access list.
NICK_ACCESS_NOT_FOUND
	%s not found on your access list.
NICK_ACCESS_DELETED
	%s deleted from your access list.
NICK_ACCESS_LIST
	Access list:
NICK_ACCESS_LIST_X
	Access list for %s:
NICK_ACCESS_LIST_EMPTY
	Your access list is empty.
NICK_ACCESS_LIST_X_EMPTY
	Access list for %s is empty.

# Status messages
NICK_STATUS_0
	STATUS %s 0
NICK_STATUS_1
	STATUS %s 1
NICK_STATUS_2
	STATUS %s 2
NICK_STATUS_3
	STATUS %s 3

# INFO responses
NICK_INFO_SYNTAX
	INFO nick [ALL]
NICK_INFO_REALNAME
	%s is %s
NICK_INFO_SERVICES_OPER
	%s is a services operator.
NICK_INFO_SERVICES_ADMIN
	%s is a services admin.
NICK_INFO_SERVICES_ROOT
	%s is a services root administrator.
NICK_INFO_ADDRESS
	Last seen address: %s
NICK_INFO_ADDRESS_ONLINE
	   Is online from: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s is currently online.
NICK_INFO_TIME_REGGED
	  Time registered: %s
NICK_INFO_LAST_SEEN
	   Last seen time: %s
NICK_INFO_LAST_QUIT
	Last quit message: %s
NICK_INFO_URL
	              URL: %s
NICK_INFO_EMAIL
	   E-mail address: %s
NICK_INFO_VHOST
	            vhost: %s
NICK_INFO_VHOST2
	            vhost: %s@%s
NICK_INFO_ICQ
	            ICQ #: %d
NICK_INFO_GREET
	    Greet message: %s
NICK_INFO_OPTIONS
	          Options: %s
NICK_INFO_EXPIRE
	Expires on: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Protection
NICK_INFO_OPT_SECURE
	Security
NICK_INFO_OPT_PRIVATE
	Private
NICK_INFO_OPT_MSG
	Message mode
NICK_INFO_OPT_AUTOOP
	Auto-op
NICK_INFO_OPT_NONE
	None
NICK_INFO_NO_EXPIRE
	This nickname will not expire.
NICK_INFO_FOR_MORE
	For more verbose information, type %R%s INFO %s ALL.
NICK_INFO_SUSPENDED
	This nickname is currently suspended, reason: %s
NICK_INFO_SUSPENDED_NO_REASON
	This nickname is currently suspended

# LIST responses
NICK_LIST_SYNTAX
	LIST pattern
NICK_LIST_SERVADMIN_SYNTAX
	LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	List of entries matching %s:
NICK_LIST_RESULTS
	End of list - %d/%d matches shown.

# ALIST responses
NICK_ALIST_SYNTAX
	ALIST nickname
NICK_ALIST_HEADER
	Channels that you have access on:
	  Num  Channel              Level    Description 
NICK_ALIST_HEADER_X
	Channels that %s has access on:
	  Num  Channel              Level    Description
NICK_ALIST_XOP_FORMAT
	  %3d %c%-20s %-8s %s
NICK_ALIST_ACCESS_FORMAT
	  %3d %c%-20s %-8d %s
NICK_ALIST_FOOTER
	End of list - %d/%d channels shown.

# GLIST responses
NICK_GLIST_HEADER
	List of nicknames in your group:
NICK_GLIST_HEADER_X
	List of nicknames in the group of %s:
NICK_GLIST_FOOTER
	%d nicknames in the group.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (expires in %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nickname [password]
NICK_NO_RECOVER_SELF
	You can't recover yourself!
NICK_RECOVERED
	User claiming your nick has been killed.
	%R%s RELEASE %s to get it back before %s timeout.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nickname [password]
NICK_RELEASE_NOT_HELD
	Nick %s isn't being held.
NICK_RELEASED
	Services' hold on your nick has been released.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nickname [password]
NICK_NO_GHOST_SELF
	You can't ghost yourself!
NICK_GHOST_KILLED
	Ghost with your nick has been killed.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nickname
NICK_GETPASS_UNAVAILABLE
	GETPASS command unavailable because encryption is in use.
NICK_GETPASS_PASSWORD_IS
	Password for %s is %s.

# GETEMAIL responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host No WildCards!!
NICK_GETEMAIL_EMAILS_ARE
	Emails Match %s to %s.
NICK_GETEMAIL_NOT_USED
	No Emails listed for %s.
	
# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS nickname
NICK_SENDPASS_UNAVAILABLE
	SENDPASS command unavailable because encryption is in use.
NICK_SENDPASS_SUBJECT
	Nickname password (%s)
NICK_SENDPASS_HEAD
	Hi,
NICK_SENDPASS_LINE_1
	You have requested to receive the password of nickname %s by e-mail.
NICK_SENDPASS_LINE_2
	The password is %s. For security reasons, you should change it as soon as possible.
NICK_SENDPASS_LINE_3
	If you don't know why this email has been sent to you, please ignore it.
NICK_SENDPASS_LINE_4
	This mailbox is not monitored. Replies to this e-mail will NOT be responded to.
NICK_SENDPASS_LINE_5
	Thanks for using %s
NICK_SENDPASS_OK
	Password of %s has been sent.

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND nickname reason
NICK_SUSPEND_SUCCEEDED
	Nick %s is now suspended.
NICK_SUSPEND_FAILED
	Couldn't suspend nick %s!

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND nickname
NICK_UNSUSPEND_SUCCEEDED
	Nick %s is now released.
NICK_UNSUSPEND_FAILED
	Couldn't release nick %s!

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID nickname [reason]
NICK_FORBID_SYNTAX_REASON
	FORBID nickname reason
NICK_FORBID_SUCCEEDED
	Nick %s is now forbidden.
NICK_FORBID_FAILED
	Couldn't forbid nick %s!

# Nick Registraion responses
NICK_REQUESTED
	This nick has already been requested, please check your e-mail address for the pass code
NICK_REG_RESENT
	Your passcode has been re-sent to %s.
NICK_REG_UNABLE
	Nick NOT registered, please try again later.
NICK_IS_PREREG
	This nick is awaiting an e-mail verification code before completing registration.
NICK_ENTER_REG_CODE
	A passcode has been sent to %s, please type %R%s confirm <passcode> to complete registration
NICK_CONFIRM_NOT_FOUND
	Registration step 1 may have expired, please use "%R%s register <password> <email>" first.
NICK_CONFIRM_INVALID
	Invalid passcode has been entered, please check the e-mail again, and retry
NICK_REG_MAIL_SUBJECT
	Nickname Registration (%s)
NICK_REG_MAIL_HEAD
	Hi,
NICK_REG_MAIL_LINE_1
	You have requested to register the nickname "%s".
NICK_REG_MAIL_LINE_2
	Please type " %R%s confirm %s " into your IRC client to complete the registration.
NICK_REG_MAIL_LINE_3
	If you don't know why this mail has been sent to you, please ignore it.
NICK_REG_MAIL_LINE_4
	This mailbox is not monitored. Replies to this e-mail will NOT get responded to.
NICK_REG_MAIL_LINE_5
	Thanks for using %s
NICK_GETPASS_PASSCODE_IS
	Passcode for %s is %s.
NICK_FORCE_REG
	Nickname %s confirmed



###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Automatic channel operator status
CHAN_LEVEL_AUTOVOICE
	Automatic mode +v
CHAN_LEVEL_AUTOHALFOP
	Automatic mode +h
CHAN_LEVEL_AUTOPROTECT
	Automatic mode +a
CHAN_LEVEL_AUTODEOP
	Channel operator status disallowed
CHAN_LEVEL_NOJOIN
	Not allowed to join channel
CHAN_LEVEL_INVITE
	Allowed to use INVITE command
CHAN_LEVEL_AKICK
	Allowed to use AKICK command
CHAN_LEVEL_SET
	Allowed to use SET command (not FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Allowed to use CLEAR command
CHAN_LEVEL_UNBAN
	Allowed to use UNBAN command
CHAN_LEVEL_OPDEOP
	Allowed to use OP/DEOP commands
CHAN_LEVEL_ACCESS_LIST
	Allowed to view the access list
CHAN_LEVEL_ACCESS_CHANGE
	Allowed to modify the access list
CHAN_LEVEL_MEMO
	Allowed to list/read channel memos
CHAN_LEVEL_ASSIGN
	Allowed to assign/unassign a bot
CHAN_LEVEL_BADWORDS
	Allowed to use BADWORDS command
CHAN_LEVEL_NOKICK
	Never kicked by the bot's kickers
CHAN_LEVEL_FANTASIA
	Allowed to use fantaisist commands
CHAN_LEVEL_SAY
	Allowed to use SAY and ACT commands
CHAN_LEVEL_GREET
	Greet message displayed
CHAN_LEVEL_VOICEME
	Allowed to (de)voice him/herself
CHAN_LEVEL_VOICE
	Allowed to use VOICE/DEVOICE commands
CHAN_LEVEL_GETKEY
	Allowed to use GETKEY command
CHAN_LEVEL_OPDEOPME
	Allowed to (de)op him/herself
CHAN_LEVEL_HALFOPME
	Allowed to (de)halfop him/herself
CHAN_LEVEL_HALFOP
	Allowed to use HALFOP/DEHALFOP commands
CHAN_LEVEL_PROTECTME
	Allowed to (de)protect him/herself
CHAN_LEVEL_PROTECT
	Allowed to use PROTECT/DEPROTECT commands
CHAN_LEVEL_KICKME
	Allowed to kick him/herself
CHAN_LEVEL_KICK
	Allowed to use KICK command
CHAN_LEVEL_SIGNKICK
	No signed kick when SIGNKICK LEVEL is used
CHAN_LEVEL_BANME
	Allowed to ban him/herself
CHAN_LEVEL_BAN
	Allowed to use BAN command
CHAN_LEVEL_TOPIC
	Allowed to use TOPIC command
CHAN_LEVEL_INFO
	Allowed to use INFO command with ALL option

# Automatic responses
CHAN_IS_REGISTERED
	This channel has been registered with %s.
CHAN_NOT_ALLOWED_OP
	You are not allowed chanop status on channel %s.
CHAN_MAY_NOT_BE_USED
	This channel may not be used.
CHAN_NOT_ALLOWED_TO_JOIN
	You are not permitted to be on this channel.
CHAN_X_INVALID
	Channel %s is not a valid channel.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER channel password description
CHAN_REGISTER_DISABLED
	Sorry, channel registration is temporarily disabled.
CHAN_REGISTER_NOT_LOCAL
	Local channels cannot be registered.
CHAN_MUST_REGISTER_NICK
	You must register your nickname first.  Type %R%s HELP for information on registering nicknames.
CHAN_MUST_IDENTIFY_NICK
	Please identify with %s first, using the command:
	%R%s IDENTIFY password
CHAN_MAY_NOT_BE_REGISTERED
	Channel %s may not be registered.
CHAN_ALREADY_REGISTERED
	Channel %s is already registered!
CHAN_MUST_BE_CHANOP
	You must be a channel operator to register the channel.
CHAN_REACHED_CHANNEL_LIMIT
	Sorry, you have already reached your limit of %d channels.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Sorry, you have already exceeded your limit of %d channels.
CHAN_REGISTRATION_FAILED
	Sorry, registration failed.
CHAN_REGISTERED
	Channel %s registered under your nickname: %s
CHAN_PASSWORD_IS
	Your channel password is %s - remember it for later use.
CHAN_REGISTER_NONE_CHANNEL
	You have attempted to register a nonexistent channel %s
CHAN_SYMBOL_REQUIRED
	Please use the symbol of # when attempting to register


# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY channel password
CHAN_IDENTIFY_FAILED
	Sorry, identification failed.
CHAN_IDENTIFY_SUCCEEDED
	Password accepted - you now have founder-level access to %s.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT channel nickname
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT channel [nickname]
CHAN_LOGOUT_SUCCEEDED
	User %s has been logged out of channel %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	All users identified have been logged out of channel %s (except the channel founder).
CHAN_LOGOUT_FOUNDER_FAILED
	You may not log out of %s (you are founder).

# DROP responses
CHAN_DROP_SYNTAX
	DROP channel
CHAN_DROP_DISABLED
	Sorry, channel de-registration is temporarily disabled.
CHAN_DROPPED
	Channel %s has been dropped.

# SET responses
CHAN_SET_SYNTAX
	SET channel option parameters
CHAN_SET_DISABLED
	Sorry, channel option setting is temporarily disabled.
CHAN_SET_UNKNOWN_OPTION
	Unknown SET option %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s has too many channels registered.
CHAN_FOUNDER_CHANGED
	Founder of %s changed to %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Successor for %s changed to %s.
CHAN_SUCCESSOR_UNSET
	Successor for %s unset.
CHAN_SUCCESSOR_IS_FOUNDER
	%s cannot be the successor on channel %s because he is its founder.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Sorry, couldn't set password.
CHAN_PASSWORD_CHANGED
	%s password changed.
CHAN_PASSWORD_CHANGED_TO
	%s password changed to %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Description of %s changed to %s.

# SET URL responses
CHAN_URL_CHANGED
	URL for %s changed to %s.
CHAN_URL_UNSET
	URL for %s unset.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	E-mail address for %s changed to %s.
CHAN_EMAIL_UNSET
	E-mail address for %s unset.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Entry message for %s changed.
CHAN_ENTRY_MSG_UNSET
	Entry message for %s unset.
	
# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s is not a valid ban type.
CHAN_SET_BANTYPE_CHANGED
	Ban type for channel %s is now #%d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Unknown mode character %c ignored.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Mode %c ignored because you can't lock it.
CHAN_SET_MLOCK_L_REQUIRED
	You must lock mode +l as well to lock mode +L.
CHAN_SET_MLOCK_K_REQUIRED
	You must lock mode +i as well to lock mode +K.
CHAN_MLOCK_CHANGED
	Mode lock on channel %s changed to %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET channel KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Topic retention option for %s is now ON.
CHAN_SET_KEEPTOPIC_OFF
	Topic retention option for %s is now OFF.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET channel TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Topic lock option for %s is now ON.
CHAN_SET_TOPICLOCK_OFF
	Topic lock option for %s is now OFF.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET channel PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Peace option for %s is now ON.
CHAN_SET_PEACE_OFF
	Peace option for %s is now OFF.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET channel PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Private option for %s is now ON.
CHAN_SET_PRIVATE_OFF
	Private option for %s is now OFF.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET channel SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Secure ops option for %s is now ON.
CHAN_SET_SECUREOPS_OFF
	Secure ops option for %s is now OFF.

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET channel SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Secure founder option for %s is now ON.
CHAN_SET_SECUREFOUNDER_OFF
	Secure founder option for %s is now OFF.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET channel RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Restricted access option for %s is now ON.
CHAN_SET_RESTRICTED_OFF
	Restricted access option for %s is now OFF.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET channel SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Secure option for %s is now ON.
CHAN_SET_SECURE_OFF
	Secure option for %s is now OFF.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET channel SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Signed kick option for %s is now ON.
CHAN_SET_SIGNKICK_LEVEL
	Signed kick option for %s is now ON, but depends of the
	level of the user that is using the command.
CHAN_SET_SIGNKICK_OFF
	Signed kick option for %s is now OFF.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET channel OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice option for %s is now ON.
CHAN_SET_OPNOTICE_OFF
	Op-notice option for %s is now OFF.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET channel XOP {ON | OFF}
CHAN_SET_XOP_ON
	xOP lists system for %s is now ON.
CHAN_SET_XOP_OFF
	xOP lists system for %s is now OFF.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET channel NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Channel %s will not expire.
CHAN_SET_NOEXPIRE_OFF
	Channel %s will expire.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Sorry, you can only have %d AOP/SOP/VOP entries on a channel.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	You can't use this command. Use the ACCESS command instead.
	Type %R%s HELP ACCESS for more information.
CHAN_XOP_NOT_AVAILABLE
	xOP system is not available.

# AOP messages
CHAN_AOP_SYNTAX
	AOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]
CHAN_AOP_DISABLED
	Sorry, channel AOP list modification is temporarily disabled.
CHAN_AOP_NICKS_ONLY
	Channel AOP lists may only contain registered nicknames.
CHAN_AOP_ADDED
	%s added to %s AOP list.
CHAN_AOP_MOVED
	%s moved to %s AOP list.
CHAN_AOP_NO_SUCH_ENTRY
	No such entry (#%d) on %s AOP list.
CHAN_AOP_NOT_FOUND
	%s not found on %s AOP list.
CHAN_AOP_NO_MATCH
	No matching entries on %s AOP list.
CHAN_AOP_DELETED
	%s deleted from %s AOP list.
CHAN_AOP_DELETED_ONE
	Deleted 1 entry from %s AOP list.
CHAN_AOP_DELETED_SEVERAL
	Deleted %d entries from %s AOP list.
CHAN_AOP_LIST_EMPTY
	%s AOP list is empty.
CHAN_AOP_LIST_HEADER
	AOP list for %s:
	  Num  Nick
CHAN_AOP_CLEAR
	Channel %s AOP list has been cleared.

# HOP messages
CHAN_HOP_SYNTAX
	HOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]
CHAN_HOP_DISABLED
	Sorry, channel HOP list modification is temporarily disabled.
CHAN_HOP_NICKS_ONLY
	Channel HOP lists may only contain registered nicknames.
CHAN_HOP_ADDED
	%s added to %s HOP list.
CHAN_HOP_MOVED
	%s moved to %s HOP list.
CHAN_HOP_NO_SUCH_ENTRY
	No such entry (#%d) on %s HOP list.
CHAN_HOP_NOT_FOUND
	%s not found on %s HOP list.
CHAN_HOP_NO_MATCH
	No matching entries on %s HOP list.
CHAN_HOP_DELETED
	%s deleted from %s HOP list.
CHAN_HOP_DELETED_ONE
	Deleted 1 entry from %s HOP list.
CHAN_HOP_DELETED_SEVERAL
	Deleted %d entries from %s HOP list.
CHAN_HOP_LIST_EMPTY
	%s HOP list is empty.
CHAN_HOP_LIST_HEADER
	HOP list for %s:
	  Num  Nick
CHAN_HOP_CLEAR
	Channel %s HOP list has been cleared.

# SOP messages
CHAN_SOP_SYNTAX
	SOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]
CHAN_SOP_DISABLED
	Sorry, channel SOP list modification is temporarily disabled.
CHAN_SOP_NICKS_ONLY
	Channel SOP lists may only contain registered nicknames.
CHAN_SOP_ADDED
	%s added to %s SOP list.
CHAN_SOP_MOVED
	%s moved to %s SOP list.
CHAN_SOP_NO_SUCH_ENTRY
	No such entry (#%d) on %s SOP list.
CHAN_SOP_NOT_FOUND
	%s not found on %s SOP list.
CHAN_SOP_NO_MATCH
	No matching entries on %s SOP list.
CHAN_SOP_DELETED
	%s deleted from %s SOP list.
CHAN_SOP_DELETED_ONE
	Deleted 1 entry from %s SOP list.
CHAN_SOP_DELETED_SEVERAL
	Deleted %d entries from %s SOP list.
CHAN_SOP_LIST_EMPTY
	%s SOP list is empty.
CHAN_SOP_LIST_HEADER
	SOP list for %s:
	  Num  Nick
CHAN_SOP_CLEAR
	Channel %s SOP list has been cleared.

# VOP messages
CHAN_VOP_SYNTAX
	VOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]
CHAN_VOP_DISABLED
	Sorry, channel VOP list modification is temporarily disabled.
CHAN_VOP_NICKS_ONLY
	Channel VOP lists may only contain registered nicknames.
CHAN_VOP_ADDED
	%s added to %s VOP list.
CHAN_VOP_MOVED
	%s moved to %s VOP list.
CHAN_VOP_NO_SUCH_ENTRY
	No such entry (#%d) on %s VOP list.
CHAN_VOP_NOT_FOUND
	%s not found on %s VOP list.
CHAN_VOP_NO_MATCH
	No matching entries on %s VOP list.
CHAN_VOP_DELETED
	%s deleted from %s VOP list.
CHAN_VOP_DELETED_ONE
	Deleted 1 entry from %s VOP list.
CHAN_VOP_DELETED_SEVERAL
	Deleted %d entries from %s VOP list.
CHAN_VOP_LIST_EMPTY
	%s VOP list is empty.
CHAN_VOP_LIST_HEADER
	VOP list for %s:
	  Num  Nick
CHAN_VOP_CLEAR
	Channel %s VOP list has been cleared.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS channel {ADD|DEL|LIST|CLEAR} [nick [level] | entry-list]
CHAN_ACCESS_XOP
	You can't use this command. 
	Use the AOP, SOP and VOP commands instead.
	Type %R%s HELP command for more information.
CHAN_ACCESS_XOP_HOP
	You can't use this command. 
	Use the AOP, SOP, HOP and VOP commands instead.
	Type %R%s HELP command for more information.
CHAN_ACCESS_DISABLED
	Sorry, channel access list modification is temporarily disabled.
CHAN_ACCESS_LEVEL_NONZERO
	Access level must be non-zero.
CHAN_ACCESS_LEVEL_RANGE
	Access level must be between %d and %d inclusive.
CHAN_ACCESS_NICKS_ONLY
	Channel access lists may only contain registered nicknames.
CHAN_ACCESS_REACHED_LIMIT
	Sorry, you can only have %d access entries on a channel.
CHAN_ACCESS_LEVEL_UNCHANGED
	Access level for %s on %s unchanged from %d.
CHAN_ACCESS_LEVEL_CHANGED
	Access level for %s on %s changed to %d.
CHAN_ACCESS_ADDED
	%s added to %s access list at level %d.
CHAN_ACCESS_NO_SUCH_ENTRY
	No such entry (#%d) on %s access list.
CHAN_ACCESS_NOT_FOUND
	%s not found on %s access list.
CHAN_ACCESS_NO_MATCH
	No matching entries on %s access list.
CHAN_ACCESS_DELETED
	%s deleted from %s access list.
CHAN_ACCESS_DELETED_ONE
	Deleted 1 entry from %s access list.
CHAN_ACCESS_DELETED_SEVERAL
	Deleted %d entries from %s access list.
CHAN_ACCESS_LIST_EMPTY
	%s access list is empty.
CHAN_ACCESS_LIST_HEADER
	Access list for %s:
	  Num   Lev  Nick
CHAN_ACCESS_LIST_FOOTER
	End of access list.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	Channel %s access list has been cleared.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK channel {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [nick-or-usermask] [reason]
CHAN_AKICK_DISABLED
	Sorry, channel autokick list modification is temporarily disabled.
CHAN_AKICK_ALREADY_EXISTS
	%s already exists on %s autokick list.
CHAN_AKICK_REACHED_LIMIT
	Sorry, you can only have %d autokick masks on a channel.
CHAN_AKICK_ADDED
	%s added to %s autokick list.
CHAN_AKICK_NO_SUCH_ENTRY
	No such entry (#%d) on %s autokick list.
CHAN_AKICK_NOT_FOUND
	%s not found on %s autokick list.
CHAN_AKICK_NO_MATCH
	No matching entries on %s autokick list.
CHAN_AKICK_STUCK
	%s is now always active on channel %s.
CHAN_AKICK_UNSTUCK
	%s is not always active anymore on channel %s.
CHAN_AKICK_DELETED
	%s deleted from %s autokick list.
CHAN_AKICK_DELETED_ONE
	Deleted 1 entry from %s autokick list.
CHAN_AKICK_DELETED_SEVERAL
	Deleted %d entries from %s autokick list.
CHAN_AKICK_LIST_EMPTY
	%s autokick list is empty.
CHAN_AKICK_LIST_HEADER
	Autokick list for %s:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (by %s on %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (stuck) (by %s on %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE for %s complete; %d users were affected.
CHAN_AKICK_CLEAR
	Channel %s akick list has been cleared.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS channel {SET | DIS[ABLE] | LIST | RESET} [item [level]]
CHAN_LEVELS_XOP
	Levels are not available as xOP is enabled on this channel.
CHAN_LEVELS_RANGE
	Level must be between %d and %d inclusive.
CHAN_LEVELS_CHANGED
	Level for %s on channel %s changed to %d.
CHAN_LEVELS_UNKNOWN
	Setting %s not known.  Type %R%s HELP LEVELS DESC for a list of valid settings.
CHAN_LEVELS_DISABLED
	%s disabled on channel %s.
CHAN_LEVELS_LIST_HEADER
	Access level settings for channel %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (disabled)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (founder only)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	Access levels for %s reset to defaults.

# Status Messages
CHAN_STATUS_SYNTAX
	STATUS ERROR Syntax error
CHAN_STATUS_NOT_REGGED
	STATUS ERROR Channel %s not registered
CHAN_STATUS_FORBIDDEN
	STATUS ERROR Channel %s forbidden
CHAN_STATUS_NOTONLINE
	STATUS ERROR Nick %s not online
CHAN_STATUS_INFO
	STATUS %s %s %d

# INFO responses
CHAN_INFO_SYNTAX
	INFO channel [ALL]
CHAN_INFO_HEADER
	Information for channel %s:
CHAN_INFO_FOUNDER
	        Founder: %s (%s)
CHAN_INFO_NO_FOUNDER
	        Founder: %s
CHAN_INFO_SUCCESSOR
	      Successor: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	      Successor: %s
CHAN_INFO_DESCRIPTION
	    Description: %s
CHAN_INFO_ENTRYMSG
	  Entry message: %s
CHAN_INFO_TIME_REGGED
	     Registered: %s
CHAN_INFO_LAST_USED
	      Last used: %s
CHAN_INFO_LAST_TOPIC
	     Last topic: %s
CHAN_INFO_TOPIC_SET_BY
	   Topic set by: %s
CHAN_INFO_URL
	            URL: %s
CHAN_INFO_EMAIL
	 E-mail address: %s
CHAN_INFO_BANTYPE
	       Ban type: %d
CHAN_INFO_OPTIONS
	        Options: %s
CHAN_INFO_OPT_KEEPTOPIC
	Topic Retention
CHAN_INFO_OPT_OPNOTICE
	OP Notice
CHAN_INFO_OPT_PEACE
	Peace
CHAN_INFO_OPT_PRIVATE
	Private
CHAN_INFO_OPT_RESTRICTED
	Restricted Access
CHAN_INFO_OPT_SECURE
	Secure
CHAN_INFO_OPT_SECUREOPS
	Secure Ops
CHAN_INFO_OPT_SECUREFOUNDER
	Secure Founder
CHAN_INFO_OPT_SIGNKICK
	Signed kicks
CHAN_INFO_OPT_TOPICLOCK
	Topic Lock
CHAN_INFO_OPT_XOP
	xOP lists system
CHAN_INFO_OPT_NONE
	None
CHAN_INFO_MODE_LOCK
	      Mode lock: %s
CHAN_INFO_EXPIRE
	      Expires on: %s
CHAN_INFO_NO_EXPIRE
	This channel will not expire.

# LIST responses
CHAN_LIST_SYNTAX
	LIST pattern
CHAN_LIST_SERVADMIN_SYNTAX
	LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
CHAN_LIST_HEADER
	List of entries matching %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	End of list - %d/%d matches shown.
	
# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE channel

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN channel
CHAN_UNBANNED
	You have been unbanned from %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC channel [topic]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR channel what
CHAN_CLEARED_BANS
	All bans on channel %s have been removed.
CHAN_CLEARED_EXCEPTS
	All excepts on channel %s have been removed.
CHAN_CLEARED_MODES
	All modes on channel %s have been reset.
CHAN_CLEARED_OPS
	Mode +o has been cleared from channel %s.
CHAN_CLEARED_HOPS
	Mode +h has been cleared from channel %s.
CHAN_CLEARED_VOICES
	Mode +v has been cleared from channel %s.
CHAN_CLEARED_USERS
	All users have been kicked from channel %s.
CHAN_CLEARED_INVITES
	All invites on channel %s have been removed.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS channel
CHAN_GETPASS_UNAVAILABLE
	GETPASS command unavailable because encryption is in use.
CHAN_GETPASS_PASSWORD_IS
	Password for channel %s is %s.

# GETKEY responses
CHAN_GETKEY_SYNTAX
	GETKEY channel
CHAN_GETKEY_NOKEY
	The channel %s has no key.
CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS channel
CHAN_SENDPASS_UNAVAILABLE
	SENDPASS command unavailable because encryption is in use.
CHAN_SENDPASS_SUBJECT
	Channel password (%s)
CHAN_SENDPASS_HEAD
	Hi,
CHAN_SENDPASS_LINE_1
	You have requested to receive the password of channel %s by e-mail.
CHAN_SENDPASS_LINE_2
	The password is %s. For security reasons, you should change it as soon as possible.
CHAN_SENDPASS_LINE_3
	If you don't know why this mail has been sent to you, please ignore it.
CHAN_SENDPASS_LINE_4
	This mailbox is not monitored. Replies to this e-mail will NOT be responded to.
CHAN_SENDPASS_LINE_5
	Thanks for using %s
CHAN_SENDPASS_OK
	Password of %s has been sent.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID channel [reason]
CHAN_FORBID_SYNTAX_REASON
	FORBID channel reason
CHAN_FORBID_SUCCEEDED
	Channel %s is now forbidden.
CHAN_FORBID_FAILED
	Couldn't forbid channel %s!
CHAN_FORBID_REASON
	This channel has been forbidden.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND channel [reason]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND channel reason
CHAN_SUSPEND_SUCCEEDED
	Channel %s is now suspended.
CHAN_SUSPEND_FAILED
	Couldn't suspended channel %s!
CHAN_SUSPEND_REASON
	This channel has been suspended.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND channel
CHAN_UNSUSPEND_ERROR
	No # found in front of channel name.
CHAN_UNSUSPEND_SUCCEEDED
	Channel %s is now released.
CHAN_UNSUSPEND_FAILED
	Couldn't release channel %s!

# Misc responses
CHAN_EXCEPTED
	%s matches an except on %s and cannot be banned until the except have been removed.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	You have 1 new memo.
MEMO_HAVE_NEW_MEMOS
	You have %d new memos.
MEMO_TYPE_READ_LAST
	Type %R%s READ LAST to read it.
MEMO_TYPE_READ_NUM
	Type %R%s READ %d to read it.
MEMO_TYPE_LIST_NEW
	Type %R%s LIST NEW to list them.
MEMO_AT_LIMIT
	Warning: You have reached your maximum number of memos (%d).  You will be unable to receive any new memos until you delete some of your current ones.
MEMO_OVER_LIMIT
	Warning: You are over your maximum number of memos (%d).  You will be unable to receive any new memos until you delete some of your current ones.
MEMO_X_MANY_NOTICE
	There are %d memos on channel %s.
MEMO_X_ONE_NOTICE
	There is %d memo on channel %s.
MEMO_NEW_X_MEMO_ARRIVED
	There is a new memo on channel %s.
	Type %R%s READ %s %d to read it.
MEMO_NEW_MEMO_ARRIVED
	You have a new memo from %s.
	Type %R%s READ %d to read it.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	You have no memos.
MEMO_X_HAS_NO_MEMOS
	%s has no memos.
MEMO_DOES_NOT_EXIST
	Memo %d does not exist!
MEMO_LIST_NOT_FOUND
	No matching memos found.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {nick | channel} memo-text
MEMO_SEND_DISABLED
	Sorry, memo sending is temporarily disabled.
MEMO_SEND_PLEASE_WAIT
	Please wait %d seconds before using the SEND command again.
MEMO_X_GETS_NO_MEMOS
	%s cannot receive memos.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s currently has too many memos and cannot receive more.
MEMO_SENT
	Memo sent to %s.
MEMO_MASS_SENT
	A massmemo has been sent to all registered users.
	
# STAFF responses
MEMO_STAFF_SYNTAX
	STAFF memo-text
	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {nick | channel}
MEMO_CANCEL_DISABLED
	Sorry, memo canceling is temporarily disabled.
MEMO_CANCEL_NONE
	No memo was cancelable.
MEMO_CANCELLED
	Last memo to %s has been cancelled.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [channel] [list | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	You have no new memos.
MEMO_X_HAS_NO_NEW_MEMOS
	%s has no new memos.
MEMO_LIST_MEMOS
	Memos for %s.  To read, type: %R%s READ num
MEMO_LIST_NEW_MEMOS
	New memos for %s.  To read, type: %R%s READ num
MEMO_LIST_CHAN_MEMOS
	Memos for %s.  To read, type: %R%s READ %s num
MEMO_LIST_CHAN_NEW_MEMOS
	New memos for %s.  To read, type: %R%s READ %s num
MEMO_LIST_HEADER
	 Num  Sender            Date/Time
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [channel] {list | LAST | NEW}
MEMO_HEADER
	Memo %d from %s (%s).  To delete, type: %R%s DEL %d
MEMO_CHAN_HEADER
	Memo %d from %s (%s).  To delete, type: %R%s DEL %s %d
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [channel] {num | list | ALL}
MEMO_DELETED_NONE
	No memos were deleted.
MEMO_DELETED_ONE
	Memo %d has been deleted.
MEMO_DELETED_SEVERAL
	Memos %s have been deleted.
MEMO_DELETED_ALL
	All of your memos have been deleted.
MEMO_CHAN_DELETED_ALL
	All memos for channel %s have been deleted.

# SET responses
MEMO_SET_SYNTAX
	SET option parameters
MEMO_SET_DISABLED
	Sorry, memo option setting is temporarily disabled.
MEMO_SET_UNKNOWN_OPTION
	Unknown SET option %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
MEMO_SET_NOTIFY_ON
	%s will now notify you of memos when you log on and when they are sent to you.
MEMO_SET_NOTIFY_LOGON
	%s will now notify you of memos when you log on or unset /AWAY.
MEMO_SET_NOTIFY_NEW
	%s will now notify you of memos when they are sent to you.
MEMO_SET_NOTIFY_OFF
	%s will not send you any notification of memos.
MEMO_SET_NOTIFY_MAIL
	You will now be informed about new memos via email.
MEMO_SET_NOTIFY_NOMAIL
	You will no longer be informed via email.
MEMO_SET_NOTIFY_INVALIDMAIL
	There's no email address set for your nick.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [channel] limit
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [user | channel] {limit | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	You are not permitted to change your memo limit.
MEMO_SET_LIMIT_FORBIDDEN
	The memo limit for %s may not be changed.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	You cannot set your memo limit higher than %d.
MEMO_SET_LIMIT_TOO_HIGH
	You cannot set the memo limit for %s higher than %d.
MEMO_SET_LIMIT_OVERFLOW
	Memo limit too large; limiting to %d instead.
MEMO_SET_YOUR_LIMIT
	Your memo limit has been set to %d.
MEMO_SET_YOUR_LIMIT_ZERO
	You will no longer be able to receive memos.
MEMO_UNSET_YOUR_LIMIT
	Your memo limit has been disabled.
MEMO_SET_LIMIT
	Memo limit for %s set to %d.
MEMO_SET_LIMIT_ZERO
	Memo limit for %s set to 0.
MEMO_UNSET_LIMIT
	Memo limit disabled for %s.

# INFO responses
MEMO_INFO_SYNTAX
	INFO [channel]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [nick | channel]
MEMO_INFO_NO_MEMOS
	You currently have no memos.
MEMO_INFO_MEMO
	You currently have 1 memo.
MEMO_INFO_MEMO_UNREAD
	You currently have 1 memo, and it has not yet been read.
MEMO_INFO_MEMOS
	You currently have %d memos.
MEMO_INFO_MEMOS_ONE_UNREAD
	You currently have %d memos, of which 1 is unread.
MEMO_INFO_MEMOS_SOME_UNREAD
	You currently have %d memos, of which %d are unread.
MEMO_INFO_MEMOS_ALL_UNREAD
	You currently have %d memos; all of them are unread.
MEMO_INFO_LIMIT
	Your memo limit is %d.
MEMO_INFO_HARD_LIMIT
	Your memo limit is %d, and may not be changed.
MEMO_INFO_LIMIT_ZERO
	Your memo limit is 0; you will not receive any new memos.
MEMO_INFO_HARD_LIMIT_ZERO
	Your memo limit is 0; you will not receive any new memos.  You cannot change this limit.
MEMO_INFO_NO_LIMIT
	You have no limit on the number of memos you may keep.
MEMO_INFO_NOTIFY_OFF
	You will not be notified of new memos.
MEMO_INFO_NOTIFY_ON
	You will be notified of new memos at logon and when they arrive.
MEMO_INFO_NOTIFY_RECEIVE
	You will be notified when new memos arrive.
MEMO_INFO_NOTIFY_SIGNON
	You will be notified of new memos at logon.
MEMO_INFO_X_NO_MEMOS
	%s currently has no memos.
MEMO_INFO_X_MEMO
	%s currently has 1 memo.
MEMO_INFO_X_MEMO_UNREAD
	%s currently has 1 memo, and it has not yet been read.
MEMO_INFO_X_MEMOS
	%s currently has %d memos.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s currently has %d memos, of which 1 is unread.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s currently has %d memos, of which %d are unread.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s currently has %d memos; all of them are unread.
MEMO_INFO_X_LIMIT
	%s's memo limit is %d.
MEMO_INFO_X_HARD_LIMIT
	%s's memo limit is %d, and may not be changed.
MEMO_INFO_X_NO_LIMIT
	%s has no memo limit.
MEMO_INFO_X_NOTIFY_OFF
	%s is not notified of new memos.
MEMO_INFO_X_NOTIFY_ON
	%s is notified of new memos at logon and when they arrive.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s is notified when new memos arrive.
MEMO_INFO_X_NOTIFY_SIGNON
	%s is notified of news memos at logon.

# Memo2Mail responses
MEMO_MAIL_SUBJECT
	New memo
MEMO_MAIL_TEXT1
	Hi %s
MEMO_MAIL_TEXT2
	You have just received a new memo from %s. This is memo number %d.
MEMO_MAIL_TEXT3
	The text of the memo is:

# RSEND responses
MEMO_RSEND_PLEASE_WAIT
	Please wait %d seconds before using the RSEND command again.
MEMO_RSEND_DISABLED
	Sorry, RSEND has been disabled on this network.
MEMO_RSEND_SYNTAX
	RSEND {nick | channel} memo-text
MEMO_RSEND_NICK_MEMO_TEXT
	[auto-memo] The memo you sent has been viewed.
MEMO_RSEND_CHAN_MEMO_TEXT
	[auto-memo] The memo you sent to %s has been viewed.
MEMO_RSEND_USER_NOTIFICATION
	A notification memo has been sent to %s informing him/her you have
	read his/her memo.

# CHECK responses
MEMO_CHECK_SYNTAX
	CHECK nickname
MEMO_CHECK_NOT_READ
	The last memo you sent to %s (sent on %s) has not yet been read.
MEMO_CHECK_READ
	The last memo you sent to %s (sent on %s) has been read.
MEMO_CHECK_NO_MEMO
	Nick %s doesn't have a memo from you.
MEMO_NO_RSEND_SELF
	You can not request a receipt when sending a memo to yourself.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
BOT_DOES_NOT_EXIST
	Bot %s does not exist.
BOT_NOT_ASSIGNED
	You must assign a bot to the channel before using this command.
	Type %R%S HELP ASSIGN for more information.
BOT_NOT_ON_CHANNEL
	Bot is not on channel %s.
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	Don't use the word "%s" on this channel!
BOT_REASON_BADWORD_GENTLE
	Watch your language!
BOT_REASON_BOLD
	Don't use bolds on this channel!
BOT_REASON_CAPS
	Turn caps lock OFF!
BOT_REASON_COLOR
	Don't use colors on this channel!
BOT_REASON_FLOOD
	Stop flooding!
BOT_REASON_REPEAT
	Stop repeating yourself!
BOT_REASON_REVERSE
	Don't use reverses on this channel!
BOT_REASON_UNDERLINE
	Don't use underlines on this channel!
	
# !seen replies
BOT_SEEN_BOT
	You found me, %s!
BOT_SEEN_YOU
	Looking for yourself, eh %s?
BOT_SEEN_ON_CHANNEL
	%s is on the channel right now!
BOT_SEEN_ON_CHANNEL_AS
	%s is on the channel right now (as %s) ! 
BOT_SEEN_ON
	%s was last seen here %s ago.
BOT_SEEN_NEVER
	I've never seen %s on this channel.
BOT_SEEN_UNKNOWN
	I don't know who %s is.

# BOT responses
BOT_BOT_SYNTAX
	BOT ADD nick user host real
	BOT CHANGE oldnick newnick [user [host [real]]]
	BOT DEL nick
BOT_BOT_ALREADY_EXISTS
	Bot %s already exists.
BOT_BOT_CREATION_FAILED
	Sorry, bot creation failed.
BOT_BOT_READONLY
	Sorry, bot modification is temporarily disabled.
BOT_BOT_ADDED
	%s!%s@%s (%s) added to the bot list.
BOT_BOT_ANY_CHANGES
	Old info is equal to the new one.
BOT_BOT_CHANGED
	Bot %s has been changed to %s!%s@%s (%s)
BOT_BOT_DELETED
	Bot %s has been deleted.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Bot list:
BOT_BOTLIST_PRIVATE_HEADER
	Bots reserved to IRC operators:
BOT_BOTLIST_FOOTER
	%d bots available.
BOT_BOTLIST_EMPTY
	There are no bots available at this time.
	Ask a Services admin to create one!
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN chan nick
BOT_ASSIGN_READONLY
	Sorry, bot assignment is temporarily disabled.
BOT_ASSIGN_ALREADY
	Bot %s is already assigned to channel %s.
BOT_ASSIGN_ASSIGNED
	Bot %s has been assigned to %s.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN chan
BOT_UNASSIGN_UNASSIGNED
	There is no bot assigned to %s anymore.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {chan | nick}
BOT_INFO_NOT_FOUND
	%s is not a valid bot or registered channel.
BOT_INFO_BOT_HEADER
	Information for bot %s:
BOT_INFO_BOT_MASK
	       Mask : %s@%s
BOT_INFO_BOT_REALNAME
	  Real name : %s
BOT_INFO_BOT_CREATED
	    Created : %s
BOT_INFO_BOT_USAGE
	    Used on : %d channel(s)
BOT_INFO_BOT_OPTIONS
	    Options : %s
BOT_INFO_OPT_PRIVATE
	Private
BOT_INFO_CHAN_HEADER
	Information for channel %s:
BOT_INFO_CHAN_BOT
	           Bot nick : %s
BOT_INFO_CHAN_BOT_NONE
	           Bot nick : not assigned yet.
BOT_INFO_CHAN_KICK_BADWORDS
	   Bad words kicker : %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	   Bad words kicker : %s (%d kick(s) to ban)
BOT_INFO_CHAN_KICK_BOLDS
	       Bolds kicker : %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	       Bolds kicker : %s (%d kick(s) to ban)
BOT_INFO_CHAN_KICK_CAPS_ON
	        Caps kicker : %s (minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	        Caps kicker : %s (%d kick(s) to ban; minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	        Caps kicker : %s
BOT_INFO_CHAN_KICK_COLORS
	      Colors kicker : %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	      Colors kicker : %s (%d kick(s) to ban)
BOT_INFO_CHAN_KICK_FLOOD_ON
	       Flood kicker : %s (%d lines in %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	       Flood kicker : %s (%d kick(s) to ban; %d lines in %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	       Flood kicker : %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	      Repeat kicker : %s (%d times)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	      Repeat kicker : %s (%d kick(s) to ban; %d times)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	      Repeat kicker : %s
BOT_INFO_CHAN_KICK_REVERSES
	    Reverses kicker : %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	    Reverses kicker : %s (%d kick(s) to ban)
BOT_INFO_CHAN_KICK_UNDERLINES
	  Underlines kicker : %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	  Underlines kicker : %s (%d kick(s) to ban)
BOT_INFO_ACTIVE
	enabled
BOT_INFO_INACTIVE
	disabled
BOT_INFO_CHAN_OPTIONS
	            Options : %s
BOT_INFO_OPT_DONTKICKOPS
	Ops protection
BOT_INFO_OPT_DONTKICKVOICES
	Voices protection
BOT_INFO_OPT_FANTASY
	Fantasy
BOT_INFO_OPT_GREET
	Greet
BOT_INFO_OPT_NOBOT
	No bot
BOT_INFO_OPT_SYMBIOSIS
	Symbiosis
BOT_INFO_OPT_NONE
	None
	
# SET responses
BOT_SET_SYNTAX
	SET (channel | bot) option settings
BOT_SET_DISABLED
	Sorry, bot option setting is temporarily disabled.
BOT_SET_UNKNOWN
	Unknown option %s.
	Type %R%S HELP SET for more information.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET channel DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Bot won't kick ops on channel %s.
BOT_SET_DONTKICKOPS_OFF
	Bot will kick ops on channel %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET channel DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Bot won't kick voices on channel %s.
BOT_SET_DONTKICKVOICES_OFF
	Bot will kick voices on channel %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET channel FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Fantasy mode is now ON on channel %s.
BOT_SET_FANTASY_OFF
	Fantasy mode is now OFF on channel %s.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET channel GREET {ON|OFF}
BOT_SET_GREET_ON
	Greet mode is now ON on channel %s.
BOT_SET_GREET_OFF
	Greet mode is now OFF on channel %s.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET botname NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	No Bot mode is now ON on channel %s.
BOT_SET_NOBOT_OFF
	No Bot mode is now OFF on channel %s.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET botname PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Private mode of bot %s is now ON.
BOT_SET_PRIVATE_OFF
	Private mode of bot %s is now OFF.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET channel SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Symbiosis mode is now ON on channel %s.
BOT_SET_SYMBIOSIS_OFF
	Symbiosis mode is now OFF on channel %s.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK channel option {ON|OFF} [settings]
BOT_KICK_DISABLED
	Sorry, kicker configuration is temporarily disabled.
BOT_KICK_UNKNOWN
	Unknown option %s.
	Type %R%S HELP KICK for more information.
BOT_KICK_BAD_TTB
	%s cannot be taken as times to ban.

BOT_KICK_BADWORDS_ON
	Bot will now kick bad words. Use the BADWORDS command
	to add or remove a bad word.
BOT_KICK_BADWORDS_ON_BAN
	Bot will now kick bad words, and will place a ban after 
	%d kicks for the same user. Use the BADWORDS command
	to add or remove a bad word.
BOT_KICK_BADWORDS_OFF
	Bot won't kick bad words anymore.
BOT_KICK_BOLDS_ON
	Bot will now kick bolds.
BOT_KICK_BOLDS_ON_BAN
	Bot will now kick bolds, and will place a ban after 
	%d kicks for the same user.
BOT_KICK_BOLDS_OFF
	Bot won't kick bolds anymore.
BOT_KICK_CAPS_ON
	Bot will now kick caps (they must constitute at least
	%d characters and %d%% of the entire message).
BOT_KICK_CAPS_ON_BAN
	Bot will now kick caps (they must constitute at least
	%d characters and %d%% of the entire message), and will 
	place a ban after %d kicks for the same user.
BOT_KICK_CAPS_OFF
	Bot won't kick caps anymore.
BOT_KICK_COLORS_ON
	Bot will now kick colors.
BOT_KICK_COLORS_ON_BAN
	Bot will now kick colors, and will place a ban after %d 
	kicks for the same user.
BOT_KICK_COLORS_OFF
	Bot won't kick colors anymore.
BOT_KICK_FLOOD_ON
	Bot will now kick flood (%d lines in %d seconds).
BOT_KICK_FLOOD_ON_BAN
	Bot will now kick flood (%d lines in %d seconds), and 
	will place a ban after %d kicks for the same user.
BOT_KICK_FLOOD_OFF
	Bot won't kick flood anymore.
BOT_KICK_REPEAT_ON
	Bot will now kick repeats (users that say %d times
	the same thing).
BOT_KICK_REPEAT_ON_BAN
	Bot will now kick repeats (users that say %d times
	the same thing), and will place a ban after %d 
	kicks for the same user.
BOT_KICK_REPEAT_OFF
	Bot won't kick repeats anymore.
BOT_KICK_REVERSES_ON
	Bot will now kick reverses.
BOT_KICK_REVERSES_ON_BAN
	Bot will now kick reverses, and will place a ban after %d 
	kicks for the same user.
BOT_KICK_REVERSES_OFF
	Bot won't kick reverses anymore.
BOT_KICK_UNDERLINES_ON
	Bot will now kick underlines.
BOT_KICK_UNDERLINES_ON_BAN
	Bot will now kick underlines, and will place a ban after %d 
	kicks for the same user.
BOT_KICK_UNDERLINES_OFF
	Bot won't kick underlines anymore.
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS channel {ADD|DEL|LIST|CLEAR} [word | entry-list] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	Sorry, channel bad words list modification is temporarily disabled.
BOT_BADWORDS_REACHED_LIMIT
	Sorry, you can only have %d bad words entries on a channel.
BOT_BADWORDS_ALREADY_EXISTS
	%s already exists in %s bad words list.
BOT_BADWORDS_ADDED
	%s added to %s bad words list.
BOT_BADWORDS_NO_SUCH_ENTRY
	No such entry (#%d) on %s bad words list.
BOT_BADWORDS_NOT_FOUND
	%s not found on %s bad words list.
BOT_BADWORDS_NO_MATCH
	No matching entries on %s bad words list.
BOT_BADWORDS_DELETED
	%s deleted from %s bad words list.
BOT_BADWORDS_DELETED_ONE
	Deleted 1 entry from %s bad words list.
BOT_BADWORDS_DELETED_SEVERAL
	Deleted %d entries from %s bad words list.
BOT_BADWORDS_LIST_EMPTY
	%s bad words list is empty.
BOT_BADWORDS_LIST_HEADER
	Bad words list for %s:
	  Num   Word                           Type
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	Bad words list is now empty.

# SAY responses
BOT_SAY_SYNTAX
	SAY channel text

# ACT responses
BOT_ACT_SYNTAX
	ACT channel text

# Errors
BOT_EXCEPT
	User matches channel except.
BOT_BAD_NICK
	Bot Nicks may only contain valid nick characters.
BOT_BAD_HOST
	Bot Hosts may only contain valid host characters.
BOT_BAD_IDENT
	Bot Idents may only contain valid characters.
BOT_LONG_IDENT
	Bot Idents may only contain %d characters.
BOT_LONG_HOST
	Bot Hosts may only contain %d characters.

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Services is unable to change modes.  Are your servers configured correctly?
OPER_BOUNCY_MODES_U_LINE
	Services is unable to change modes.  Are your servers' U:lines configured correctly?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL message

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Unknown STATS option %s.
OPER_STATS_CURRENT_USERS
	Current users: %d (%d ops)
OPER_STATS_MAX_USERS
	Maximum users: %d (%s)
# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Services up %d days, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Services up %d day, %02d:%02d
OPER_STATS_UPTIME_HMS
	Services up %d hours, %d minutes
OPER_STATS_UPTIME_HM1S
	Services up %d hours, %d minutes
OPER_STATS_UPTIME_H1MS
	Services up %d hours, %d minute
OPER_STATS_UPTIME_H1M1S
	Services up %d hours, %d minute
OPER_STATS_UPTIME_1HMS
	Services up %d hour, %d minutes
OPER_STATS_UPTIME_1HM1S
	Services up %d hour, %d minutes
OPER_STATS_UPTIME_1H1MS
	Services up %d hour, %d minute
OPER_STATS_UPTIME_1H1M1S
	Services up %d hour, %d minute
OPER_STATS_UPTIME_MS
	Services up %d minutes, %d seconds
OPER_STATS_UPTIME_M1S
	Services up %d minutes, %d second
OPER_STATS_UPTIME_1MS
	Services up %d minute, %d seconds
OPER_STATS_UPTIME_1M1S
	Services up %d minute, %d second
OPER_STATS_BYTES_READ
	Bytes read    : %5d kB
OPER_STATS_BYTES_WRITTEN
	Bytes written : %5d kB
OPER_STATS_USER_MEM
	User          : %6d records, %5d kB
OPER_STATS_CHANNEL_MEM
	Channel       : %6d records, %5d kB
OPER_STATS_GROUPS_MEM
	NS Groups     : %6d records, %5d kB
OPER_STATS_ALIASES_MEM
	NS Aliases    : %6d records, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ      : %6d records, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ       : %6d records, %5d kB
OPER_STATS_HOSTSERV_MEM
	HostServ      : %6d records, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ      : %6d records, %5d kB
OPER_STATS_SESSIONS_MEM
	Sessions      : %6d records, %5d kB
OPER_STATS_AKILL_COUNT
	Current number of AKILLs: %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Default AKILL expiry time: %d days
OPER_STATS_AKILL_EXPIRE_DAY
	Default AKILL expiry time: 1 day
OPER_STATS_AKILL_EXPIRE_HOURS
	Default AKILL expiry time: %d hours
OPER_STATS_AKILL_EXPIRE_HOUR
	Default AKILL expiry time: 1 hour
OPER_STATS_AKILL_EXPIRE_MINS
	Default AKILL expiry time: %d minutes
OPER_STATS_AKILL_EXPIRE_MIN
	Default AKILL expiry time: 1 minute
OPER_STATS_AKILL_EXPIRE_NONE
	Default AKILL expiry time: No expiration
OPER_STATS_SGLINE_COUNT
	Current number of SGLINEs: %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Default SGLINE expiry time: %d days
OPER_STATS_SGLINE_EXPIRE_DAY
	Default SGLINE expiry time: 1 day
OPER_STATS_SGLINE_EXPIRE_HOURS
	Default SGLINE expiry time: %d hours
OPER_STATS_SGLINE_EXPIRE_HOUR
	Default SGLINE expiry time: 1 hour
OPER_STATS_SGLINE_EXPIRE_MINS
	Default SGLINE expiry time: %d minutes
OPER_STATS_SGLINE_EXPIRE_MIN
	Default SGLINE expiry time: 1 minute
OPER_STATS_SGLINE_EXPIRE_NONE
	Default SGLINE expiry time: No expiration
OPER_STATS_SQLINE_COUNT
	Current number of SQLINEs: %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Default SQLINE expiry time: %d days
OPER_STATS_SQLINE_EXPIRE_DAY
	Default SQLINE expiry time: 1 day
OPER_STATS_SQLINE_EXPIRE_HOURS
	Default SQLINE expiry time: %d hours
OPER_STATS_SQLINE_EXPIRE_HOUR
	Default SQLINE expiry time: 1 hour
OPER_STATS_SQLINE_EXPIRE_MINS
	Default SQLINE expiry time: %d minutes
OPER_STATS_SQLINE_EXPIRE_MIN
	Default SQLINE expiry time: 1 minute
OPER_STATS_SQLINE_EXPIRE_NONE
	Default SQLINE expiry time: No expiration
OPER_STATS_SZLINE_COUNT
	Current number of SZLINEs: %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Default SZLINE expiry time: %d days
OPER_STATS_SZLINE_EXPIRE_DAY
	Default SZLINE expiry time: 1 day
OPER_STATS_SZLINE_EXPIRE_HOURS
	Default SZLINE expiry time: %d hours
OPER_STATS_SZLINE_EXPIRE_HOUR
	Default SZLINE expiry time: 1 hour
OPER_STATS_SZLINE_EXPIRE_MINS
	Default SZLINE expiry time: %d minutes
OPER_STATS_SZLINE_EXPIRE_MIN
	Default SZLINE expiry time: 1 minute
OPER_STATS_SZLINE_EXPIRE_NONE
	Default SZLINE expiry time: No expiration
OPER_STATS_RESET
	Statistics reset.
OPER_STATS_UPLINK_SERVER
	Uplink server: %s
OPER_STATS_UPLINK_CAPAB
	Uplink capab: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Servers found: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE channel modes

# UMODE respondes
OPER_UMODE_SYNTAX
	UMODE nick modes
OPER_UMODE_SUCCESS
	Changed usermodes of %s.
OPER_UMODE_CHANGED
	%s changed your usermodes.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE nick flags
OPER_OLINE_SUCCESS
	Operflags %s have been added for %s.
OPER_OLINE_IRCOP
	You are now an IRC Operator.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES channel [ALL]
OPER_CLEARMODES_DONE
	Binary modes and bans cleared from channel %s.
OPER_CLEARMODES_ALL_DONE
	All modes cleared from channel %s.


# KICK responses
OPER_KICK_SYNTAX
	KICK channel user reason

# SVSNICK responses
OPER_SVSNICK_SYNTAX
	SVSNICK nick newnick 
OPER_SVSNICK_NEWNICK
	The nick %s is now being changed to %s.
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [nick | entry-list]
OPER_ADMIN_SKELETON
	Services is in skeleton mode; the ADMIN command is unavailable.
OPER_ADMIN_EXISTS
	%s already exists on Services admin list.
OPER_ADMIN_REACHED_LIMIT
	Sorry, you can only have %d Services admins.
OPER_ADMIN_ADDED
	%s added to Services admin list.
OPER_ADMIN_NOT_FOUND
	%s not found on Services admin list.
OPER_ADMIN_NO_MATCH
	No matching entries on Services admin list.
OPER_ADMIN_DELETED
	%s deleted from Services admin list.
OPER_ADMIN_DELETED_ONE
	Deleted 1 entry from Services admin list.
OPER_ADMIN_DELETED_SEVERAL
	Deleted %d entries from Services admin list.
OPER_ADMIN_LIST_EMPTY
	Services admin list is empty.
OPER_ADMIN_LIST_HEADER
	Services admin list:
	  Num   Nick
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	Services admin list has been cleared.
OPER_ADMIN_MOVED
	%s has been moved to Services Administrators list.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | entry-list]
OPER_OPER_SKELETON
	Services is in skeleton mode; the OPER command is unavailable.
OPER_OPER_EXISTS
	%s already exists on Services operator list.
OPER_OPER_REACHED_LIMIT
	Sorry, you can only have %d Services operators.
OPER_OPER_ADDED
	%s added to Services operator list.
OPER_OPER_NOT_FOUND
	%s not found on Services operator list.
OPER_OPER_NO_MATCH
	No matching entries on Services operator list.
OPER_OPER_DELETED
	%s deleted from Services operator list.
OPER_OPER_DELETED_ONE
	Deleted 1 entry from Services operator list.
OPER_OPER_DELETED_SEVERAL
	Deleted %d entries from Services operator list.
OPER_OPER_LIST_EMPTY
	Services operator list is empty.
OPER_OPER_LIST_HEADER
	Services operator list:
	  Num   Nick
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	Services operator list has been cleared.
OPER_OPER_MOVED
	%s has been moved to Services Operators list.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]
OPER_AKILL_EXISTS
	%s already exists on the AKILL list.
OPER_AKILL_ALREADY_COVERED
	%s is already covered by %s.
OPER_AKILL_REACHED_LIMIT
	Sorry, you can only have %d AKILLs.
OPER_AKILL_NO_NICK
	Reminder: AKILL masks cannot contain nicknames; make sure you have not included a nick portion in your mask.
OPER_AKILL_ADDED
	%s added to the AKILL list.
OPER_AKILL_CHANGED
	Expiry time of %s changed.
OPER_AKILL_NOT_FOUND
	%s not found on the AKILL list.
OPER_AKILL_NO_MATCH
	No matching entries on the AKILL list.
OPER_AKILL_DELETED
	%s deleted from the AKILL list.
OPER_AKILL_DELETED_ONE
	Deleted 1 entry from the AKILL list.
OPER_AKILL_DELETED_SEVERAL
	Deleted %d entries from the AKILL list.
OPER_AKILL_LIST_EMPTY
	AKILL list is empty.
OPER_AKILL_LIST_HEADER
	Current AKILL list:
	  Num   Mask                              Reason
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Current AKILL list:
# number, mask, set-by, set-time, expires, reason
OPER_AKILL_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_AKILL_CLEAR
	The AKILL list has been cleared.
OPER_CHANKILL_SYNTAX
	CHANKILL [+expiry] {#channel} [reason]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list}[:reason]]
OPER_SGLINE_UNSUPPORTED
	Sorry, SGLINE is not available on this network.
OPER_SGLINE_EXISTS
	%s already exists on the SGLINE list.
OPER_SGLINE_ALREADY_COVERED
	%s is already covered by %s.
OPER_SGLINE_REACHED_LIMIT
	Sorry, you can only have %d SGLINEs.
OPER_SGLINE_ADDED
	%s added to the SGLINE list.
OPER_SGLINE_CHANGED
	Expiry time of %s changed.
OPER_SGLINE_NOT_FOUND
	%s not found on the SGLINE list.
OPER_SGLINE_NO_MATCH
	No matching entries on the SGLINE list.
OPER_SGLINE_DELETED
	%s deleted from the SGLINE list.
OPER_SGLINE_DELETED_ONE
	Deleted 1 entry from the SGLINE list.
OPER_SGLINE_DELETED_SEVERAL
	Deleted %d entries from the SGLINE list.
OPER_SGLINE_LIST_EMPTY
	SGLINE list is empty.
OPER_SGLINE_LIST_HEADER
	Current SGLINE list:
	  Num   Mask                              Reason
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Current SGLINE list:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SGLINE_CLEAR
	The SGLINE list has been cleared.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	Channel SQLINEs are not supported by your IRCd, so you can't use them.
OPER_SQLINE_EXISTS
	%s already exists on the SQLINE list.
OPER_SQLINE_ALREADY_COVERED
	%s is already covered by %s.
OPER_SQLINE_REACHED_LIMIT
	Sorry, you can only have %d SQLINEs.
OPER_SQLINE_ADDED
	%s added to the SQLINE list.
OPER_SQLINE_CHANGED
	Expiry time of %s changed.
OPER_SQLINE_NOT_FOUND
	%s not found on the SQLINE list.
OPER_SQLINE_NO_MATCH
	No matching entries on the SQLINE list.
OPER_SQLINE_DELETED
	%s deleted from the SQLINE list.
OPER_SQLINE_DELETED_ONE
	Deleted 1 entry from the SQLINE list.
OPER_SQLINE_DELETED_SEVERAL
	Deleted %d entries from the SQLINE list.
OPER_SQLINE_LIST_EMPTY
	SQLINE list is empty.
OPER_SQLINE_LIST_HEADER
	Current SQLINE list:
	  Num   Mask                              Reason
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Current SQLINE list:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SQLINE_CLEAR
	The SQLINE list has been cleared.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]
OPER_SZLINE_UNSUPPORTED
	Sorry, SZLINE is not available on this network.
OPER_SZLINE_EXISTS
	%s already exists on the SZLINE list.
OPER_SZLINE_ALREADY_COVERED
	%s is already covered by %s.
OPER_SZLINE_REACHED_LIMIT
	Sorry, you can only have %d SZLINEs.
OPER_SZLINE_ONLY_IPS
	Reminder: you can only add IP masks to the SZLINE list.
OPER_SZLINE_ADDED
	%s added to the SZLINE list.
OPER_SZLINE_CHANGED
	Expiry time of %s changed.
OPER_SZLINE_NOT_FOUND
	%s not found on the SZLINE list.
OPER_SZLINE_NO_MATCH
	No matching entries on the SZLINE list.
OPER_SZLINE_DELETED
	%s deleted from the SZLINE list.
OPER_SZLINE_DELETED_ONE
	Deleted 1 entry from the SZLINE list.
OPER_SZLINE_DELETED_SEVERAL
	Deleted %d entries from the SZLINE list.
OPER_SZLINE_LIST_EMPTY
	SZLINE list is empty.
OPER_SZLINE_LIST_HEADER
	Current SZLINE list:
	  Num   Mask                              Reason
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Current SZLINE list:
# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SZLINE_CLEAR
	The SZLINE list has been cleared.

# SET responses
OPER_SET_SYNTAX
	SET option setting
OPER_SET_IGNORE_ON
	Ignore code will be used.
OPER_SET_IGNORE_OFF
	Ignore code will not be used.
OPER_SET_IGNORE_ERROR
	Setting for IGNORE must be ON or OFF.
OPER_SET_READONLY_ON
	Services are now in read-only mode.
OPER_SET_READONLY_OFF
	Services are now in read-write mode.
OPER_SET_READONLY_ERROR
	Setting for READONLY must be ON or OFF.
OPER_SET_LOGCHAN_ON
	Services are now reporting log messages to %s.
OPER_SET_LOGCHAN_OFF
	Services are no longer reporting log messages to a channel.
OPER_SET_LOGCHAN_ERROR
	Setting for LOGCHAN must be ON or OFF and LogChannel must be defined.
OPER_SET_DEBUG_ON
	Services are now in debug mode.
OPER_SET_DEBUG_OFF
	Services are now in non-debug mode.
OPER_SET_DEBUG_LEVEL
	Services are now in debug mode (level %d).
OPER_SET_DEBUG_ERROR
	Setting for DEBUG must be ON, OFF, or a positive number.
OPER_SET_NOEXPIRE_ON
	Services are now in no expire mode.
OPER_SET_NOEXPIRE_OFF
	Services are now in expire mode.
OPER_SET_NOEXPIRE_ERROR
	Setting for NOEXPIRE must be ON or OFF.
OPER_SET_UNKNOWN_OPTION
	Unknown option %s.
OPER_SET_SQL_ON
	SQL code will be used.
OPER_SET_SQL_OFF
	SQL code will not be used.
OPER_SET_SQL_ERROR
	Setting for SQL must be ON or OFF.
OPER_SET_SQL_ERROR_INIT
	Error during init of SQL, check your logs to correct.
OPER_SET_SQL_ERROR_DISABLED
	SQL is not configured for this network. Please edit the
	configuration file (services.conf).
OPER_SET_LIST_OPTION_ON
	%s is enabled
OPER_SET_LIST_OPTION_OFF
	%s is disabled

# NOOP responses
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} server
OPER_NOOP_SET
	All O:lines of %s have been removed.
OPER_NOOP_REVOKE
	All O:lines of %s have been reset.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE servername [reason]
OPER_JUPE_HOST_ERROR
	Please use a valid server name when juping
OPER_JUPE_INVALID_SERVER
	You can not jupe your services server or your uplink server.
	
# RAW responses
OPER_RAW_SYNTAX
	RAW text

# UPDATE responses
OPER_UPDATING
	Updating databases.
	
# RELOAD responses
OPER_RELOAD
	Services' configuration file has been reloaded.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN not defined; cannot restart.  Rerun the \2configure\2 script and recompile Services to enable the RESTART command.

# IGNORE respondes
OPER_IGNORE_SYNTAX
	IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick | mask]
OPER_IGNORE_VALID_TIME
	You have to enter a valid number as time.
OPER_IGNORE_TIME_DONE
	%s will now be ignored for %s.
OPER_IGNORE_PERM_DONE
	%s will now permanently be ignored.
OPER_IGNORE_DEL_DONE
	%s will no longer be ignored.
OPER_IGNORE_LIST
	Services ignore list:
OPER_IGNORE_LIST_NOMATCH
	Nick %s not found on ignore list.
OPER_IGNORE_LIST_EMPTY
	Ignore list is empty.
OPER_IGNORE_LIST_CLEARED
	Ignore list has been cleared.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	Could not find user %s.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Channel list:
	Name                 Users Modes   Topic
OPER_CHANLIST_HEADER_USER
	%s channel list:
	Name                 Users Modes   Topic
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	End of channel list.
	
# USERLIST responses
OPER_USERLIST_HEADER
	Users list:
	Nick                 Mask
OPER_USERLIST_HEADER_CHAN
	%s users list:
	Nick                 Mask
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	End of users list.

OPER_SUPER_ADMIN_ON
	You are now a SuperAdmin
OPER_SUPER_ADMIN_OFF
	You are no longer a SuperAdmin
OPER_SUPER_ADMIN_SYNTAX
	Setting for SuperAdmin must be ON or OFF (must be enabled in services.conf)
OPER_SUPER_ADMIN_WALL_ON
	%s is now a Super-Admin
OPER_SUPER_ADMIN_WALL_OFF
	%s is no longer a Super-Admin
OPER_SUPER_ADMIN_ONLY
	Only Super-Admins can use this command.
OPER_STAFF_LIST_HEADER
	On Level Nick
OPER_STAFF_FORMAT
	 %c %s  %s
OPER_STAFF_AFORMAT
	 %c %s  %s [%s]

# DefCon Messages

OPER_DEFCON_SYNTAX
	DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	Services are in Defcon mode, Please try again later.
OPER_DEFCON_NO_CONF
	The Defcon system must be enabled in the services.conf file
OPER_DEFCON_CHANGED
	Services are now at DEFCON %d
OPER_DEFCON_WALL
	%s Changed the DEFCON level to %d
DEFCON_GLOBAL
	The Defcon Level is now at Level: %d

# Module strings

OPER_MODULE_LOADED
	Module %s loaded
OPER_MODULE_UNLOADED
	Module %s unloaded
OPER_MODULE_LOAD_FAIL
	Unable to load module %s
OPER_MODULE_REMOVE_FAIL
	Unable to remove module %s
OPER_MODULE_NO_UNLOAD
	This module can not be unloaded.
OPER_MODULE_LOAD_SYNTAX
	MODLOAD FileName
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD FileName
OPER_MODULE_LIST_HEADER
	Current Module list:
OPER_MODULE_LIST
	Module: %s [%s] [%s]
OPER_MODULE_LIST_FOOTER
	%d Modules loaded.
OPER_MODULE_INFO_LIST
	Module: %s Version: %s Author: %s loaded: %s
OPER_MODULE_CMD_LIST
	Providing command: %R%s %s
OPER_MODULE_MSG_LIST
	Providing IRCD handler for: %s
OPER_MODULE_NO_LIST
	No modules currently loaded
OPER_MODULE_NO_INFO
	No information about module %s is available
OPER_MODULE_INFO_SYNTAX
	MODINFO FileName
MODULE_HELP_HEADER
	The following commands have been loaded by a module:

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [params]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+expiry] mask limit reason
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {mask | list}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE num position
OPER_EXCEPTION_DISABLED
	Session limiting is disabled.
OPER_EXCEPTION_ALREADY_PRESENT
	Mask %s already present on exception list.
OPER_EXCEPTION_TOO_MANY
	Session-limit exception list is full!
OPER_EXCEPTION_ADDED
	Session limit for %s set to %d.
OPER_EXCEPTION_MOVED
	Exception for %s (#%d) moved to position %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	No such entry (#%d) session-limit exception list.
OPER_EXCEPTION_NOT_FOUND
	%s not found on session-limit exception list.
OPER_EXCEPTION_NO_MATCH
	No matching entries on session-limit exception list.
OPER_EXCEPTION_DELETED
	%s deleted from session-limit exception list.
OPER_EXCEPTION_DELETED_ONE
	Deleted 1 entry from session-limit exception list.
OPER_EXCEPTION_DELETED_SEVERAL
	Deleted %d entries from session-limit exception list.
OPER_EXCEPTION_LIST_HEADER
	Current Session Limit Exception list:
# mask, limit
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Num  Limit  Host
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (by %s on %s; %s)
	    Limit: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Invalid session limit. It must be a valid integer greater than or equal to zero and less than %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Invalid hostmask. Only real hostmasks are valid as exceptions are not matched against nicks or usernames.
OPER_EXCEPTION_EXISTS
	%s already exists on the EXCEPTION list.
OPER_EXCEPTION_CHANGED
	Exception for %s has been updated to %d.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST limit | VIEW host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST limit
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW host
OPER_SESSION_DISABLED
	Session limiting is disabled.
OPER_SESSION_INVALID_THRESHOLD
	Invalid threshold value. It must be a valid integer greater than 1.
OPER_SESSION_NOT_FOUND
	%s not found on session list.
OPER_SESSION_LIST_HEADER
	Hosts with at least %d sessions:
OPER_SESSION_LIST_COLHEAD
	Sessions  Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	The host %s currently has %d sessions with a limit of %d.

# EXCEPTION help

OPER_HELP_EXCEPTION
	Syntax: EXCEPTION ADD [+expiry] mask limit reason
	        EXCEPTION DEL {mask | list}
	        EXCEPTION MOVE num position
	        EXCEPTION LIST [mask | list]
	        EXCEPTION VIEW [mask | list]
	
	Allows Services admins to manipulate the list of hosts that
	have specific session limits - allowing certain machines, 
	such as shell servers, to carry more than the default number
	of clients at a time. Once a host reaches it's session limit,
	all clients attempting to connect from that host will be
	killed. Before the user is killed, they are notified, via a
	/NOTICE from %S, of a source of help regarding session
	limiting. The content of this notice is a config setting.
	
	EXCEPTION ADD adds the given host mask to the exception list.
	Note that nick!user@host and user@host masks are invalid!
	Only real host masks, such as box.host.dom and *.host.dom,
	are allowed because sessions limiting does not take nick or
	user names into account. limit must be a number greater than
	or equal to zero. This determines how many sessions this host
	may carry at a time. A value of zero means the host has an
	unlimited session limit. See the AKILL help for details about
	the format of the optional expiry parameter.
	EXCEPTION DEL removes the given mask from the exception list.
	EXCEPTION MOVE moves exception num to position. The
	exceptions inbetween will be shifted up or down to fill the gap.
	EXCEPTION LIST and EXCEPTION VIEW show all current
	exceptions; if the optional mask is given, the list is limited
	to those exceptions matching the mask. The difference is that
	EXCEPTION VIEW is more verbose, displaying the name of the
	person who added the exception, it's session limit, reason, 
	host mask and the expiry date and time.
	
	Note that a connecting client will "use" the first exception
	their host matches. Large exception lists and widely matching
	exception masks are likely to degrade services' performance.

OPER_HELP_SESSION
	Syntax: SESSION LIST threshold
	        SESSION VIEW host
	
	Allows Services admins to view the session list.

	SESSION LIST lists hosts with at least threshold sessions.
	The threshold must be a number greater than 1. This is to 
	prevent accidental listing of the large number of single 
	session hosts.
	SESSION VIEW displays detailed information about a specific
	host - including the current session count and session limit.
	The host value may not include wildcards.

	See the EXCEPTION help for more information about session
	limiting and how to set session limits specific to certain
	hosts and groups thereof.

OPER_HELP_STAFF
	Syntax: STAFF

	Displays all Services Staff nicks along with level
	and on-line status.

OPER_HELP_DEFCON
	Syntax: DEFCON [1|2|3|4|5]

	The defcon system can be used to implement a pre-defined
	set of restrictions to services useful during an attempted
	attack on the network.

OPER_HELP_DEFCON_NO_NEW_CHANNELS
	* No new channel registrations
OPER_HELP_DEFCON_NO_NEW_NICKS
	* No new nick registrations
OPER_HELP_DEFCON_NO_MLOCK_CHANGE
	* No MLOCK changes
OPER_HELP_DEFCON_FORCE_CHAN_MODES
	* Force Chan Modes (%s) to be set on all channels
OPER_HELP_DEFCON_REDUCE_SESSION
	* Use the reduced session limit of %d
OPER_HELP_DEFCON_NO_NEW_CLIENTS
	* Kill any NEW clients connecting
OPER_HELP_DEFCON_OPER_ONLY
	* Ignore any non-opers with message
OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* Silently ignore non-opers
OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* AKILL any new clients connecting
OPER_HELP_DEFCON_NO_NEW_MEMOS
	* No new memos sent
OPER_HELP_CHANKILL
	Syntax: CHANKILL [+expiry] channel reason

	Puts an AKILL for every nick on the specified channel. It
	uses the entire and complete real ident@host for every nick,
	then enforces the AKILL. 

###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Logon News - %s] %s
NEWS_OPER_TEXT
	[Oper News - %s] %s
NEWS_RANDOM_TEXT
	[Random News - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD|DEL|LIST} [text|num]
NEWS_LOGON_LIST_HEADER
	Logon news items:
NEWS_LOGON_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_LOGON_LIST_NONE
	There is no logon news.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD text
NEWS_LOGON_ADD_FULL
	News list is full!
NEWS_LOGON_ADDED
	Added new logon news item (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {num | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Logon news item #%d not found!
NEWS_LOGON_DELETED
	Logon news item #%d deleted.
NEWS_LOGON_DEL_NONE
	No logon news items to delete!
NEWS_LOGON_DELETED_ALL
	All logon news items deleted.

NEWS_OPER_SYNTAX
	OPERNEWS {ADD|DEL|LIST} [text|num]
NEWS_OPER_LIST_HEADER
	Oper news items:
NEWS_OPER_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_OPER_LIST_NONE
	There is no oper news.
NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD text
NEWS_OPER_ADD_FULL
	News list is full!
NEWS_OPER_ADDED
	Added new oper news item (#%d).
NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {num | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Oper news item #%d not found!
NEWS_OPER_DELETED
	Oper news item #%d deleted.
NEWS_OPER_DEL_NONE
	No oper news items to delete!
NEWS_OPER_DELETED_ALL
	All oper news items deleted.

NEWS_RANDOM_SYNTAX
	RANDOMNEWS {ADD|DEL|LIST} [text|num]
NEWS_RANDOM_LIST_HEADER
	Random news items:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_RANDOM_LIST_NONE
	There is no random news.
NEWS_RANDOM_ADD_SYNTAX
	RANDOMNEWS ADD text
NEWS_RANDOM_ADD_FULL
	News list is full!
NEWS_RANDOM_ADDED
	Added new random news item (#%d).
NEWS_RANDOM_DEL_SYNTAX
	RANDOMNEWS DEL {num | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Random news item #%d not found!
NEWS_RANDOM_DELETED
	Random news item #%d deleted.
NEWS_RANDOM_DEL_NONE
	No random news items to delete!
NEWS_RANDOM_DELETED_ALL
	All random news items deleted.

NEWS_HELP_LOGON
	Syntax: LOGONNEWS ADD text
	        LOGONNEWS DEL {num | ALL}
	        LOGONNEWS LIST
	
	Edits or displays the list of logon news messages.  When a
	user connects to the network, these messages will be sent
	to them.  (However, no more than %s messages will be
	sent in order to avoid flooding the user.  If there are
	more news messages, only the most recent will be sent.)
	NewsCount can be configured in services.conf.
	
	LOGONNEWS may only be used by Services admins.

NEWS_HELP_OPER
	Syntax: OPERNEWS ADD text
	        OPERNEWS DEL {num | ALL}
	        OPERNEWS LIST
	
	Edits or displays the list of oper news messages.  When a
	user opers up (with the /OPER command), these messages will
	be sent to them.  (However, no more than %s messages will
	be sent in order to avoid flooding the user.  If there are
	more news messages, only the most recent will be sent.)
	NewsCount can be configured in services.conf.
	
	OPERNEWS may only be used by Services admins.

NEWS_HELP_RANDOM
	Syntax: RANDOMNEWS ADD text
	        RANDOMNEWS DEL {num | ALL}
	        RANDOMNEWS LIST
	
	Edits or displays the list of random news messages.  When a
	user connects to the network, one (and only one) of the
	random news will be randomly chosen and sent to them.
	
	RANDOMNEWS may only be used by Services admins.

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S is a service designed to give out information on
	Services.  Help topics are accessible via 
	the HELP commands of the other Services clients:

	%R%s HELP
	     for information on registering nicknames

	%R%s HELP
	     for information on registering and controlling
	     channels

	%R%s HELP
	     for information on sending messages to off-line users

HELP_HELP_BOT

	%R%s HELP
	     for information on setting up a bot on your channel

HELP_HELP_HOST

	%R%s HELP
	     for information on setting up nick vHosts

###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP_CMD_CONFIRM
	    CONFIRM    Confirm a nickserv auth code
NICK_HELP_CMD_RESEND
	    RESEND     Resend a nickserv auth code
NICK_HELP_CMD_REGISTER
	    REGISTER   Register a nickname
NICK_HELP_CMD_GROUP
	    GROUP      Join a group
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY   Identify yourself with your password
NICK_HELP_CMD_ACCESS
	    ACCESS     Modify the list of authorized addresses
NICK_HELP_CMD_SET
	    SET        Set options, including kill protection
NICK_HELP_CMD_SASET
	    SASET      Set SET-options on another nickname
NICK_HELP_CMD_DROP
	    DROP       Cancel the registration of a nickname
NICK_HELP_CMD_RECOVER
	    RECOVER    Kill another user who has taken your nick
NICK_HELP_CMD_RELEASE
	    RELEASE    Regain custody of your nick after RECOVER
NICK_HELP_CMD_SENDPASS
	    SENDPASS   Forgot your password? Try this
NICK_HELP_CMD_GHOST
	    GHOST      Disconnects a "ghost" IRC session using your nick
NICK_HELP_CMD_ALIST
	    ALIST      List channels you have access on
NICK_HELP_CMD_GLIST
	    GLIST      Lists all nicknames in your group
NICK_HELP_CMD_INFO
	    INFO       Displays information about a given nickname
NICK_HELP_CMD_LIST
	    LIST       List all registered nicknames that match a given pattern
NICK_HELP_CMD_LOGOUT
	    LOGOUT     Reverses the effect of the IDENTIFY command
NICK_HELP_CMD_STATUS
	    STATUS     Returns the owner status of the given nickname
NICK_HELP_CMD_UPDATE
	    UPDATE     Updates your current status, i.e. it checks for new memos
NICK_HELP_CMD_GETPASS
	    GETPASS    Retrieve the password for a nickname
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Matches and returns all users that registered using given email
NICK_HELP_CMD_FORBID
	    FORBID     Prevents a nickname from being registered
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Suspend a given nick
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Unsuspend a given nick

NICK_HELP
	%S allows you to "register" a nickname and
	prevent others from using it. The following
	commands allow for registration and maintenance of
	nicknames; to use them, type %R%S command.
	For more information on a specific command, type
	%R%S HELP command.
	
	
NICK_HELP_FOOTER
	 
	NOTICE: This service is intended to provide a way for
	IRC users to ensure their identity is not compromised.
	It is NOT intended to facilitate "stealing" of
	nicknames or other malicious actions.  Abuse of %S
	will result in, at minimum, loss of the abused
	nickname(s).
	
NICK_HELP_EXPIRES
	
	Nicknames that are not used anymore are subject to 
	the automatic expiration, i.e. they will be deleted
	after %d days if not used.

NICK_HELP_REGISTER
	Syntax: REGISTER password [email]
	
	Registers your nickname in the %S database.  Once
	your nick is registered, you can use the SET and ACCESS
	commands to configure your nick's settings as you like
	them.  Make sure you remember the password you use when
	registering - you'll need it to make changes to your nick
	later.  (Note that case matters!  ANOPE, Anope, and 
	anope are all different passwords!)
	
	Guidelines on choosing passwords:
	
	Passwords should not be easily guessable.  For example,
	using your real name as a password is a bad idea.  Using
	your nickname as a password is a much worse idea ;) and,
	in fact, %S will not allow it.  Also, short
	passwords are vulnerable to trial-and-error searches, so
	you should choose a password at least 5 characters long.
	Finally, the space character cannot be used in passwords.
	
	The parameter email is optional and will set the email
	for your nick immediately. However, it may be required
	on certain networks.
	Your privacy is respected; this e-mail won't be given to
	any third-party person.
	
	This command also creates a new group for your nickname,
	that will allow you to register other nicks later sharing
	the same configuration, the same set of memos and the
	same channel privileges. For more information on this
	feature, type %R%S HELP GROUP.
	
NICK_HELP_GROUP
	Syntax: GROUP target password
	
	This command makes your nickname join the target nickname's 
	group. password is the password of the target nickname.
	
	Joining a group will allow you to share your configuration,
	memos, and channel privileges with all the nicknames in the
	group, and much more!
	 
	A group exists as long as it is useful. This means that even
	if a nick of the group is dropped, you won't lose the
	shared things described above, as long as there is at
	least one nick remaining in the group.
	
	You can use this command even if you have not registered
	your nick yet. If your nick is already registered, you'll
	need to identify yourself before using this command. Type
	%R%S HELP IDENTIFY for more information. This
	last may be not possible on your IRC network.
	
	It is recommended to use this command with a non-registered
	nick because it will be registered automatically when 
	using this command. You may use it with a registered nick (to 
	change your group) only if your network administrators allowed 
	it.
	
	You can only be in one group at a time. Group merging is
	not possible.
	
	Note: all the nicknames of a group have the same password.

NICK_HELP_IDENTIFY
	Syntax: IDENTIFY password
	
	Tells %S that you are really the owner of this
	nick.  Many commands require you to authenticate yourself
	with this command before you use them.  The password
	should be the same one you sent with the REGISTER
	command.

NICK_HELP_UPDATE
	Syntax: UPDATE

	Updates your current status, i.e. it checks for new memos,
	sets needed chanmodes (ModeonID) and updates your vhost and
	your userflags (lastseentime, etc).
	
NICK_HELP_LOGOUT
	Syntax: LOGOUT
	
	This reverses the effect of the IDENTIFY command, i.e.
	make you not recognized as the real owner of the nick
	anymore. Note, however, that you won't be asked to reidentify
	yourself.

NICK_HELP_DROP
	Syntax: DROP [nickname]
	
	Drops your nickname from the %S database.  A nick
	that has been dropped is free for anyone to re-register.
	
	You may drop a nick within your group by passing it
	as the nick parameter.
	
	In order to use this command, you must first identify
	with your password (%R%S HELP IDENTIFY for more
	information).

NICK_HELP_ACCESS
	Syntax: ACCESS ADD mask
	        ACCESS DEL mask
	        ACCESS LIST
	
	Modifies or displays the access list for your nick.  This
	is the list of addresses which will be automatically
	recognized by %S as allowed to use the nick.  If
	you want to use the nick from a different address, you
	need to send an IDENTIFY command to make %S
	recognize you.
	
	Examples:
	
	    ACCESS ADD anyone@*.bepeg.com
	        Allows access to user anyone from any machine in
	        the bepeg.com domain.
	
	    ACCESS DEL anyone@*.bepeg.com
	        Reverses the previous command.
	
	    ACCESS LIST
	        Displays the current access list.

NICK_HELP_SET
	Syntax: SET option parameters
	
	Sets various nickname options.  option can be one of:
	
	    DISPLAY    Set the display of your group in Services
	    PASSWORD   Set your nickname password
	    LANGUAGE   Set the language Services will use when
	                   sending messages to you
	    URL        Associate a URL with your nickname
	    EMAIL      Associate an E-mail address with your nickname
	    ICQ        Associate an ICQ number with your nickname
	    GREET      Associate a greet message with your nickname
	    KILL       Turn protection on or off
	    SECURE     Turn nickname security on or off
	    PRIVATE    Prevent your nickname from appearing in a
	                   %R%S LIST
	    HIDE       Hide certain pieces of nickname information
	    MSG        Change the communication method of Services
	    AUTOOP     Should services op you automatically.    
	
	In order to use this command, you must first identify
	with your password (%R%S HELP IDENTIFY for more
	information).
	
	Type %R%S HELP SET option for more information
	on a specific option.

NICK_HELP_SET_DISPLAY
	Syntax: SET DISPLAY new-display
	
	Changes the display used to refer to your nickname group in 
	Services. The new display MUST be a nick of your group.

NICK_HELP_SET_PASSWORD
	Syntax: SET PASSWORD new-password
	
	Changes the password used to identify you as the nick's
	owner.

NICK_HELP_SET_LANGUAGE
	Syntax: SET LANGUAGE number
	
	Changes the language Services uses when sending messages to
	you (for example, when responding to a command you send).
	number should be chosen from the following list of
	supported languages:
	

NICK_HELP_SET_URL
	Syntax: SET URL url
	
	Associates the given URL with your nickname.  This URL
	will be displayed whenever someone requests information
	on your nick with the INFO command.

NICK_HELP_SET_EMAIL
	Syntax: SET EMAIL address
	
	Associates the given E-mail address with your nickname.
	This address will be displayed whenever someone requests
	information on the nickname with the INFO command.
	
NICK_HELP_SET_ICQ
	Syntax: SET ICQ number
	
	Associates the given ICQ number with your nickname.  This 
	number will be displayed whenever someone requests 
	information on your nick with the INFO command.
	
NICK_HELP_SET_GREET
	Syntax: SET GREET message
	
	Makes the given message the greet of your nickname, that
	will be displayed when joining a channel that has GREET
	option enabled, provided that you have the necessary 
	access on it.

NICK_HELP_SET_KILL
	Syntax: SET KILL {ON | QUICK | IMMED | OFF}
	
	Turns the automatic protection option for your nick
	on or off.  With protection on, if another user
	tries to take your nick, they will be given one minute to
	change to another nick, after which %S will forcibly change
	their nick.
	
	If you select QUICK, the user will be given only 20 seconds
	to change nicks instead of the usual 60.  If you select
	IMMED, user's nick will be changed immediately without being
	warned first or given a chance to change their nick; please
	do not use this option unless necessary.  Also, your
	network's administrators may have disabled this option.

NICK_HELP_SET_SECURE
	Syntax: SET SECURE {ON | OFF}
	
	Turns %S's security features on or off for your
	nick.  With SECURE set, you must enter your password
	before you will be recognized as the owner of the nick,
	regardless of whether your address is on the access
	list.  However, if you are on the access list, %S
	will not auto-kill you regardless of the setting of the
	KILL option.

NICK_HELP_SET_PRIVATE
	Syntax: SET PRIVATE {ON | OFF}
	
	Turns %S's privacy option on or off for your nick.
	With PRIVATE set, your nickname will not appear in
	nickname lists generated with %S's LIST command.
	(However, anyone who knows your nickname can still get
	information on it using the INFO command.)

NICK_HELP_SET_HIDE
	Syntax: SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	Allows you to prevent certain pieces of information from
	being displayed when someone does a %S INFO on your
	nick.  You can hide your E-mail address (EMAIL), last seen
	user@host mask (USERMASK), your services access status
	(STATUS) and  last quit message (QUIT).
	The second parameter specifies whether the information should
	be displayed (OFF) or hidden (ON).
	
NICK_HELP_SET_MSG
	Syntax: SET MSG {ON | OFF}
	
	Allows you to choose the way Services are communicating with 
	you. With MSG set, Services will use messages, else they'll 
	use notices.

NICK_HELP_SET_AUTOOP
	Syntax: SET AUTOOP {ON | OFF}
	
	Sets whether you will be opped automatically. Set to ON to 
	allow ChanServ to op you automatically when entering channels.

NICK_HELP_SASET
	Syntax: SASET nickname option parameters.
	
	Sets various nickname options.  option can be one of:
	
	    DISPLAY    Set the display of the group in Services
	    PASSWORD   Set the nickname password
	    URL        Associate a URL with the nickname
	    EMAIL      Associate an E-mail address with the nickname
	    ICQ        Associate an ICQ number with the nickname
	    GREET      Associate a greet message with the nickname
	    KILL       Turn protection on or off
	    SECURE     Turn nickname security on or off
	    PRIVATE    Prevent the nickname from appearing in a
	                   %R%S LIST
	    HIDE       Hide certain pieces of nickname information
	    MSG        Change the communication method of Services
	    NOEXPIRE   Prevent the nickname from expiring
	    AUTOOP     Turn autoop on or off
	    LANGUAGE   Set the language Services will use when
	               sending messages to nickname
	
	Type %R%S HELP SASET option for more information
	on a specific option. The options will be set on the given
	nickname. 

NICK_HELP_SASET_DISPLAY
	Syntax: SASET nickname DISPLAY new-display
	
	Changes the display used to refer to the nickname group in 
	Services. The new display MUST be a nick of the group.

NICK_HELP_SASET_PASSWORD
	Syntax: SASET nickname PASSWORD new-password
	
	Changes the password used to identify as the nick's	owner.

NICK_HELP_SASET_URL
	Syntax: SASET nickname URL url
	
	Associates the given URL with the nickname.  This URL
	will be displayed whenever someone requests information
	on the nick with the INFO command.

NICK_HELP_SASET_EMAIL
	Syntax: SASET nickname EMAIL address
	
	Associates the given E-mail address with the nickname.
	
NICK_HELP_SASET_ICQ
	Syntax: SASET nickname ICQ number
	
	Associates the given ICQ number with the nickname.  This 
	number will be displayed whenever someone requests 
	information on the nick with the INFO command.
	
NICK_HELP_SASET_GREET
	Syntax: SASET nickname GREET message
	
	Makes the given message the greet of the nickname, that
	will be displayed when joining a channel that has GREET
	option enabled, provided that the user has the necessary 
	access on it.

NICK_HELP_SASET_KILL
	Syntax: SASET nickname KILL {ON | QUICK | IMMED | OFF}
	
	Turns the automatic protection option for the nick
	on or off.  With protection on, if another user
	tries to take the nick, they will be given one minute to
	change to another nick, after which %S will forcibly change
	their nick.
	
	If you select QUICK, the user will be given only 20 seconds
	to change nicks instead of the usual 60.  If you select
	IMMED, user's nick will be changed immediately without being
	warned first or given a chance to change their nick; please
	do not use this option unless necessary.  Also, your
	network's administrators may have disabled this option.

NICK_HELP_SASET_SECURE
	Syntax: SASET nickname SECURE {ON | OFF}
	
	Turns %S's security features on or off for your
	nick.  With SECURE set, you must enter your password
	before you will be recognized as the owner of the nick,
	regardless of whether your address is on the access
	list.  However, if you are on the access list, %S
	will not auto-kill you regardless of the setting of the
	KILL option.

NICK_HELP_SASET_PRIVATE
	Syntax: SASET nickname PRIVATE {ON | OFF}
	
	Turns %S's privacy option on or off for the nick.
	With PRIVATE set, the nickname will not appear in
	nickname lists generated with %S's LIST command.
	(However, anyone who knows the nickname can still get
	information on it using the INFO command.)

NICK_HELP_SASET_HIDE
	Syntax: SASET nickname HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	Allows you to prevent certain pieces of information from
	being displayed when someone does a %S INFO on the
	nick.  You can hide the E-mail address (EMAIL), last seen
	user@host mask (USERMASK), the services access status
	(STATUS) and  last quit message (QUIT).
	The second parameter specifies whether the information should
	be displayed (OFF) or hidden (ON).
	
NICK_HELP_SASET_MSG
	Syntax: SASET nickname MSG {ON | OFF}
	
	Allows you to choose the way Services are communicating with 
	the given user. With MSG set, Services will use messages,
	else they'll use notices.

NICK_HELP_SASET_NOEXPIRE
	Syntax: SASET nickname NOEXPIRE {ON | OFF}
	
	Sets whether the given nickname will expire.  Setting this
	to ON prevents the nickname from expiring.

NICK_HELP_SASET_AUTOOP
	Syntax: SASET nickname AUTOOP {ON | OFF}
	
	Sets whether the given nickname will be opped automatically.
	Set to ON to allow ChanServ to op the given nickname 
	automatically when joining channels.

NICK_HELP_SASET_LANGUAGE
	Syntax: SASET nickname LANGUAGE number
	
	Changes the language Services uses when sending messages to
	nickname (for example, when responding to a command he sends).
	number should be chosen from a list of supported languages
	that you can get by typing %R%S HELP SET LANGUAGE.

NICK_HELP_RECOVER
	Syntax: RECOVER nickname [password]
	
	Allows you to recover your nickname if someone else has
	taken it; this does the same thing that %S does
	automatically if someone tries to use a kill-protected
	nick.
	
	When you give this command, %S will bring a fake
	user online with the same nickname as the user you're
	trying to recover your nick from.  This causes the IRC
	servers to disconnect the other user.  This fake user will
	remain online for %s to ensure that the other
	user does not immediately reconnect; after that time, you
	can reclaim your nick.  Alternatively, use the RELEASE
	command (%R%S HELP RELEASE) to get the nick
	back sooner.
	
	In order to use the RECOVER command for a nick, your
	current address as shown in /WHOIS must be on that nick's
	access list, you must be identified and in the group of
	that nick, or you must supply the correct password for
	the nickname.

NICK_HELP_RELEASE
	Syntax: RELEASE nickname [password]
	
	Instructs %S to remove any hold on your nickname
	caused by automatic kill protection or use of the RECOVER
	command.  This holds lasts for %s;
	this command gets rid of them sooner.
	
	In order to use the RELEASE command for a nick, your
	current address as shown in /WHOIS must be on that nick's
	access list, you must be identified and in the group of
	that nick, or you must supply the correct password for
	the nickname.

NICK_HELP_GHOST
	Syntax: GHOST nickname [password]
	
	Terminates a "ghost" IRC session using your nick.  A
	"ghost" session is one which is not actually connected,
	but which the IRC server believes is still online for one
	reason or another.  Typically, this happens if your
	computer crashes or your Internet or modem connection
	goes down while you're on IRC.
	
	In order to use the GHOST command for a nick, your
	current address as shown in /WHOIS must be on that nick's
	access list, you must be identified and in the group of
	that nick, or you must supply the correct password for
	the nickname.

NICK_HELP_INFO
	Syntax: INFO nickname [ALL]
	
	Displays information about the given nickname, such as
	the nick's owner, last seen address and time, and nick
	options. If you are identified for the nick you're 
	getting information for and ALL is specified, you will
	be shown all the information; regardless of whether 
	it's hidden or not.

NICK_HELP_LIST
	Syntax: LIST pattern
	
	Lists all registered nicknames which match the given
	pattern, in nick!user@host format.  Nicks with the
	PRIVATE option set will not be displayed.
	Note that a pattern preceding with a '#' specifies a range.
	
	Examples:
	
	    LIST *!joeuser@foo.com
	        Lists all nicks owned by joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Lists all registered nicks with Bot in their
	        names (case insensitive).
	
	    LIST *!*@*.bar.org
	        Lists all nicks owned by users in the bar.org
	        domain.
	
	    LIST #50-100
	        Lists nicks numbers 50-100

NICK_HELP_ALIST
	Syntax: ALIST [level]
	
	Lists all channels you have access on. Optionally, you can specify
	a level in XOP or ACCESS format. The resulting list will only
	include channels where you have the given level of access.

	Examples:
	    ALIST Founder
	        Lists all channels where you have Founder
	        access.

	    ALIST AOP
	        Lists all channels where you have AOP
	        access or greater.

	    ALIST 10
	        Lists all channels where you have level 10
	        access or greater.

	Channels that have the NOEXPIRE option set will be
	prefixed by an exclamation mark.

NICK_HELP_GLIST
	Syntax: GLIST
	
	Lists all nicks in your group.

NICK_HELP_STATUS
	Syntax: STATUS nickname...
	
	Returns whether the user using the given nickname is
	recognized as the owner of the nickname.  The response has
	this format:
	
	    nickname status-code
	
	where nickname is the nickname sent with the command, and
	status-code is one of the following:
	
	    0 - no such user online or nickname not registered
	    1 - user not recognized as nickname's owner
	    2 - user recognized as owner via access list only
	    3 - user recognized as owner via password identification
	
	Up to sixteen nicknames may be sent with each command; the
	rest will be ignored. If no nickname is given, your status
	will be returned.

NICK_HELP_SENDPASS
	Syntax: SENDPASS nickname
	
	Send the password of the given nickname to the e-mail address
	set in the nickname record. This command is really useful
	to deal with lost passwords.
	
	May be limited to IRC operators on certain networks.
	
	This command is unavailable when encryption is enabled.

NICK_HELP_CONFIRM
	Syntax: CONFIRM passcode
	
	This is the second step of nickname registration process.
	You must perform this command in order to get your nickname
	registered with %S. The passcode (or called auth code also)
	is sent to your e-mail address in the first step of the
	registration process. For more information about the first
	stage of the registration process, type: %R%S HELP REGISTER

NICK_HELP_RESEND
	Syntax: RESEND
	
	This command will re-send the auth code (also called passcode)
	to the e-mail address of the user whom is performing it.

NICK_SERVADMIN_HELP
	 
	Services admins can also drop any nickname without needing
	to identify for the nick, and may view the access list for
	any nickname (%R%S ACCESS LIST nick).

NICK_SERVADMIN_HELP_LOGOUT
	Syntax: LOGOUT [nickname [REVALIDATE]]
	
	Without a parameter, reverses the effect of the IDENTIFY 
	command, i.e. make you not recognized as the real owner of the nick
	anymore. Note, however, that you won't be asked to reidentify
	yourself.
	
	With a parameter, does the same for the given nick. If you 
	specify REVALIDATE as well, Services will ask the given nick
	to re-identify. This use limited to Services admins.

NICK_SERVADMIN_HELP_DROP
	Syntax: DROP [nickname]
	
	Without a parameter, drops your nickname from the
	%S database.
	
	With a parameter, drops the named nick from the database.
	You may drop any nick within your group without any 
	special privileges. Dropping any nick is limited to 
	Services admins.

NICK_SERVADMIN_HELP_INFO
	
	Services admins may use the ALL parameter with any nick.

NICK_SERVADMIN_HELP_LIST
	Syntax: LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	Lists all registered nicknames which match the given
	pattern, in nick!user@host format.  Nicks with the PRIVATE
	option set will only be displayed to Services admins.  Nicks
	with the NOEXPIRE option set will have a ! appended to
	the nickname for Services admins.
	
	If the FORBIDDEN, SUSPENDED, NOEXPIRE or UNCONFIRMED options are given, only
	nicks which, respectively, are FORBIDDEN, SUSPENDED, UNCONFIRMED or have the
	NOEXPIRE flag set will be displayed. If multiple options are
	given, all nicks matching at least one option will be displayed.
	These options are limited to Services admins.  

	Examples:
	
	    LIST *!joeuser@foo.com
	        Lists all registered nicks owned by joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Lists all registered nicks with Bot in their
	        names (case insensitive).
	
	    LIST * NOEXPIRE
	        Lists all registered nicks which have been set to
	        not expire.

NICK_SERVADMIN_HELP_ALIST
	Syntax: ALIST [nickname] [level]
	
	With no parameters, lists channels you have access on. With
	one parameter, lists channels that nickname has access 
	on. With two parameters lists channels that nickname has 
	level access or greater on.

	This use limited to Services admins.

NICK_SERVADMIN_HELP_GLIST
	Syntax: GLIST [nickname]
	
	Without a parameter, lists all nicknames that are in
	your group.
	
	With a parameter, lists all nicknames that are in the
	group of the given nick.
	This use limited to Services admins.

NICK_SERVADMIN_HELP_GETPASS
	Syntax: GETPASS nickname
	
	Returns the password for the given nickname.  Note that
	whenever this command is used, a message including the
	person who issued the command and the nickname it was used
	on will be logged and sent out as a WALLOPS/GLOBOPS.
	
	This command is unavailable when encryption is enabled.

NICK_SERVADMIN_HELP_GETEMAIL
	Syntax: GETEMAIL user@emailhost

	Returns the matching nicks that used given email. Note that
	you can not use wildcards for either user or emailhost. Whenever
	this command is used, a message including the person who issued
	the command and the email it was used on will be logged.

NICK_SERVADMIN_HELP_FORBID
	Syntax: FORBID nickname [reason]
	
	Disallows a nickname from being registered or used by
	anyone.  May be cancelled by dropping the nick.
	
	On certain networks, reason is required.

NICK_SERVADMIN_HELP_SUSPEND
	Syntax: SUSPEND nickname reason

	SUSPENDs a nickname from being used.

NICK_SERVADMIN_HELP_UNSUSPEND
	Syntax: UNSUSPEND nickname

	UNSUSPENDS a nickname from being used.

###########################################################################
#
# ChanServ help messages
#
###########################################################################

CHAN_HELP_CMD_GETPASS
	    GETPASS    Retrieve the founder password for a channel
CHAN_HELP_CMD_FORBID
	    FORBID     Prevent a channel from being used
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    Prevent a channel from being used preserving
	               channel data and settings
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Releases a suspended channel
CHAN_HELP_CMD_STATUS
	    STATUS     Returns the current access level of a user
	               on a channel
CHAN_HELP_CMD_REGISTER
	    REGISTER   Register a channel
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   Identify yourself with your password
CHAN_HELP_CMD_SET
	    SET        Set channel options and information
CHAN_HELP_CMD_AOP
	    AOP        Modify the list of AOP users
CHAN_HELP_CMD_SOP
	    SOP        Modify the list of SOP users
CHAN_HELP_CMD_ACCESS
	    ACCESS     Modify the list of privileged users
CHAN_HELP_CMD_LEVELS
	    LEVELS     Redefine the meanings of access levels
CHAN_HELP_CMD_AKICK
	    AKICK      Maintain the AutoKick list
CHAN_HELP_CMD_DROP
	    DROP       Cancel the registration of a channel
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Help retrieve lost passwords
CHAN_HELP_CMD_BAN
	    BAN        Bans a selected nick on a channel
CHAN_HELP_CMD_CLEAR
	    CLEAR      Tells ChanServ to clear certain settings on a channel
CHAN_HELP_CMD_DEVOICE
	    DEVOICE    Devoices a selected nick on a channel
CHAN_HELP_CMD_GETKEY
	    GETKEY     Returns the key of the given channel
CHAN_HELP_CMD_INFO
	    INFO       Lists information about the named registered channel
CHAN_HELP_CMD_INVITE
	    INVITE     Tells ChanServ to invite you into a channel
CHAN_HELP_CMD_KICK
	    KICK       Kicks a selected nick from a channel
CHAN_HELP_CMD_LIST
	    LIST       Lists all registered channels matching the given pattern
CHAN_HELP_CMD_LOGOUT
	    LOGOUT     This command will logout the selected nickname
CHAN_HELP_CMD_OP
	    OP         Gives Op status to a selected nick on a channel
CHAN_HELP_CMD_TOPIC
	    TOPIC      Manipulate the topic of the specified channel
CHAN_HELP_CMD_UNBAN
	    UNBAN      Remove all bans preventing you from entering a channel
CHAN_HELP_CMD_VOICE
	    VOICE      Voices a selected nick on a channel
CHAN_HELP_CMD_VOP
	    VOP        Maintains the VOP (VOicePeople) list for a channel
CHAN_HELP_CMD_DEHALFOP
	    DEHALFOP   Dehalfops a selected nick on a channel
CHAN_HELP_CMD_DEOWNER
	    DEOWNER    Removes your owner status on a channel
CHAN_HELP_CMD_DEPROTECT
	    DEPROTECT  Deprotects a selected nick on a channel
CHAN_HELP_CMD_HALFOP
	    HALFOP     Halfops a selected nick on a channel
CHAN_HELP_CMD_HOP
	    HOP        Maintains the HOP (HalfOP) list for a channel
CHAN_HELP_CMD_OWNER
	    OWNER      Gives you owner status on channel
CHAN_HELP_CMD_PROTECT
	    PROTECT    Protects a selected nick on a channel
CHAN_HELP_CMD_ADMIN
	    ADMIN      Protects a selected nick on a channel
CHAN_HELP_CMD_DEADMIN
	    DEADMIN    Deprotects a selected nick on a channel
CHAN_HELP_CMD_DEOP
	    DEOP       Deops a selected nick on a channel

CHAN_HELP
	%S allows you to register and control various
	aspects of channels.  %S can often prevent
	malicious users from "taking over" channels by limiting
	who is allowed channel operator privileges.  Available
	commands are listed below; to use them, type
	%R%S command.  For more information on a
	specific command, type %R%S HELP command.
	 
	
CHAN_HELP_EXPIRES
	
	Note that any channel which is not used for %d days
	(i.e. which no user on the channel's access list enters
	for that period of time) will be automatically dropped.

CHAN_HELP_REGISTER
	Syntax: REGISTER channel password description
	
	Registers a channel in the %S database.  In order
	to use this command, you must first be a channel operator
	on the channel you're trying to register.  The password
	is used with the IDENTIFY command to allow others to
	make changes to the channel settings at a later time.
	The last parameter, which must be included, is a
	general description of the channel's purpose.
	
	When you register a channel, you are recorded as the
	"founder" of the channel.  The channel founder is allowed
	to change all of the channel settings for the channel;
	%S will also automatically give the founder
	channel-operator privileges when s/he enters the channel.
	See the ACCESS command (%R%S HELP ACCESS) for
	information on giving a subset of these privileges to
	other channel users.
	
	NOTICE: In order to register a channel, you must have
	first registered your nickname.  If you haven't,
	%R%s HELP for information on how to do so.

CHAN_HELP_IDENTIFY
	Syntax: IDENTIFY channel password
	
	Authenticates you to %S as the founder of the given
	channel.  Many commands require you to use this command
	before using them.  The password should be the same one
	you sent with the REGISTER command.
	
CHAN_HELP_LOGOUT
	Syntax: LOGOUT channel nickname
	
	This command will log the selected nickname out meaning they 
	would have to reidentify themselves to regain their access.
	
	If you are the founder of the channel, you can log out anybody,
	else you can only log out yourself.

CHAN_HELP_DROP
	Syntax: DROP channel
	
	Unregisters the named channel.  Can only be used by
	channel founder, who must use the IDENTIFY command first.

CHAN_HELP_SET
	Syntax: SET channel option parameters
	
	Allows the channel founder to set various channel options
	and other information.
	
	Available options:
	
	    FOUNDER       Set the founder of a channel
	    SUCCESSOR     Set the successor for a channel
	    PASSWORD      Set the founder password
	    DESC          Set the channel description
	    URL           Associate a URL with the channel
	    EMAIL         Associate an E-mail address with the channel
	    ENTRYMSG      Set a message to be sent to users when they
	                     enter the channel
	    BANTYPE       Set how Services make bans on the channel
	    MLOCK         Lock channel modes on or off
	    KEEPTOPIC     Retain topic when channel is not in use
	    OPNOTICE      Send a notice when OP/DEOP commands are used
	    PEACE         Regulate the use of critical commands
	    PRIVATE       Hide channel from LIST command
	    RESTRICTED    Restrict access to the channel
	    SECURE        Activate %S security features
	    SECUREOPS     Stricter control of chanop status
	    SECUREFOUNDER Stricter control of channel founder status
	    SIGNKICK      Sign kicks that are done with KICK command
	    TOPICLOCK     Topic can only be changed with TOPIC
	    XOP           Toggle the user privilege system
	
	Type %R%S HELP SET option for more information on a
	particular option.

CHAN_HELP_SET_FOUNDER
	Syntax: SET channel FOUNDER nick
	
	Changes the founder of a channel.  The new nickname must
	be a registered one.

CHAN_HELP_SET_SUCCESSOR
	Syntax: SET channel SUCCESSOR nick
	
	Changes the successor of a channel.  If the founder's
	nickname expires or is dropped while the channel is still
	registered, the successor will become the new founder of the
	channel.  However, if the successor already has too many
	channels registered (%d), the channel will be dropped
	instead, just as if no successor had been set.  The new
	nickname must be a registered one.

CHAN_HELP_SET_PASSWORD
	Syntax: SET channel PASSWORD password
	
	Sets the password used to identify as the founder of the
	channel.

CHAN_HELP_SET_DESC
	Syntax: SET channel DESC description
	
	Sets the description for the channel, which shows up with
	the LIST and INFO commands.

CHAN_HELP_SET_URL
	Syntax: SET channel URL [url]
	
	Associates the given URL with the channel.  This URL will
	be displayed whenever someone requests information on the
	channel with the INFO command.  If no parameter is given,
	deletes any current URL for the channel.

CHAN_HELP_SET_EMAIL
	Syntax: SET channel EMAIL [address]
	
	Associates the given E-mail address with the channel.
	This address will be displayed whenever someone requests
	information on the channel with the INFO command.  If no
	parameter is given, deletes any current E-mail address for
	the channel.

CHAN_HELP_SET_ENTRYMSG
	Syntax: SET channel ENTRYMSG [message]
	
	Sets the message which will be sent via /notice to users
	when they enter the channel.  If no parameter is given,
	causes no message to be sent upon entering the channel.

CHAN_HELP_SET_BANTYPE
	Syntax: SET channel BANTYPE bantype
	
	Sets the ban type that will be used by services whenever
	they need to ban someone from your channel.
	
	bantype is a number between 0 and 3 that means:
	
	0: ban in the form *!user@host
	1: ban in the form *!*user@host
	2: ban in the form *!*@host
	3: ban in the form *!*user@*.domain

CHAN_HELP_SET_KEEPTOPIC
	Syntax: SET channel KEEPTOPIC {ON | OFF}
	
	Enables or disables the topic retention option for a
	channel.  When topic retention is set, the topic for the
	channel will be remembered by %S even after the
	last user leaves the channel, and will be restored the
	next time the channel is created.

CHAN_HELP_SET_TOPICLOCK
	Syntax: SET channel TOPICLOCK {ON | OFF}
	
	Enables or disables the topic lock option for a channel.
	When topic lock is set, %S will not allow the
	channel topic to be changed except via the TOPIC
	command.

CHAN_HELP_SET_MLOCK
	Syntax: SET channel MLOCK modes
	
	Sets the mode-lock parameter for the channel. %S
	allows you to define certain channel modes to be always
	on, off or free to be either on or off.
	
	The modes parameter is constructed exactly the same way 
	as a /MODE command; that is, modes followed by a + are 
	locked on, and modes followed by a - are locked off. Note,
	however, that unlike the /MODE command, each use of
	SET MLOCK will remove all modes previously locked before
	setting the new!
	
	Warning:  If you set a mode-locked key, as in the second
	example below, you should also set the RESTRICTED option for
	the channel (see HELP SET RESTRICTED), or anyone entering
	the channel when it is empty will be able to see the key!
	
	Examples:
	
	    SET #channel MLOCK +nt-iklps
	        Forces modes n and t on, and modes i, k, l, p, and
	        s off.  Mode m is left free to be either on or off.
	
	    SET #channel MLOCK +knst-ilmp my-key
	        Forces modes k, n, s, and t on, and modes i, l, m,
	        and p off.  Also forces the channel key to be
	        "my-key".
	
	    SET #channel MLOCK +
	        Removes the mode lock; all channel modes are free
	        to be either on or off.

CHAN_HELP_SET_PEACE
	Syntax: SET channel PEACE {ON | OFF}
	
	Enables or disables the peace option for a channel.
	When peace is set, a user won't be able to kick,
	ban or remove a channel status of a user that has
	a level superior or equal to his via %S commands.

CHAN_HELP_SET_PRIVATE
	Syntax: SET channel PRIVATE {ON | OFF}
	
	Enables or disables the private option for a channel.
	When private is set, a %R%S LIST will not
	include the channel in any lists.

CHAN_HELP_SET_RESTRICTED
	Syntax: SET channel RESTRICTED {ON | OFF}
	
	Enables or disables the restricted access option for a
	channel.  When restricted access is set, users not on
	the access list will instead be kicked and banned from the channel.

CHAN_HELP_SET_SECURE
	Syntax: SET channel SECURE {ON | OFF}
	
	Enables or disables %S's security features for a
	channel.  When SECURE is set, only users who have
	registered their nicknames with %s and IDENTIFY'd
	with their password will be given access to the channel
	as controlled by the access list.

CHAN_HELP_SET_SECUREOPS
	Syntax: SET channel SECUREOPS {ON | OFF}
	
	Enables or disables the secure ops option for a channel.
	When secure ops is set, users who are not on the userlist
	will not be allowed chanop status.
	
CHAN_HELP_SET_SECUREFOUNDER
	Syntax: SET channel SECUREFOUNDER {ON | OFF}
	
	Enables or disables the secure founder option for a channel.
	When secure founder is set, only the real founder will be
	able to drop the channel, change its password, its founder and its
	successor, and not those who are IDENTIFY'd with %S.
	
CHAN_HELP_SET_SIGNKICK
	Syntax: SET channel SIGNKICK {ON | LEVEL | OFF}
	
	Enables or disables signed kicks for a
	channel.  When SIGNKICK is set, kicks issued with
	%S KICK command will have the nick that used the
	command in their reason.
	
	If you use LEVEL, those who have a level that is superior 
	or equal to the SIGNKICK level on the channel won't have their 
	kicks signed. See %R%S HELP LEVELS for more information. 

CHAN_HELP_SET_XOP
	Syntax: SET channel XOP {ON | OFF}
	
	Enables or disables the xOP lists system for a channel.
	When XOP is set, you have to use the AOP/SOP/VOP
	commands in order to give channel privileges to
	users, else you have to use the ACCESS command.
	
	Technical Note: when you switch from access list to xOP 
	lists system, your level definitions and user levels will be
	changed, so you won't find the same values if you
	switch back to access system! 
	
	You should also check that your users are in the good xOP 
	list after the switch from access to xOP lists, because the 
	guess is not always perfect... in fact, it is not recommended 
	to use the xOP lists if you changed level definitions with 
	the LEVELS command.
	
	Switching from xOP lists system to access list system
	causes no problem though.

CHAN_HELP_SET_OPNOTICE
	Syntax: SET channel OPNOTICE {ON | OFF}
	
	Enables or disables the op-notice option for a channel.
	When op-notice is set, %S will send a notice to the
	channel whenever the OP or DEOP commands are used for a user
	in the channel.

CHAN_HELP_AOP
	Syntax: AOP channel ADD nick
	        AOP channel DEL {nick | entry-num | list}
	        AOP channel LIST [mask | list]
	        AOP channel CLEAR
	
	Maintains the AOP (AutoOP) list for a channel. The AOP 
	list gives users the right to be auto-opped on your channel,
	to unban or invite themselves if needed, to have their
	greet message showed on join, and so on.
	
	The AOP ADD command adds the given nickname to the
	AOP list.
	
	The AOP DEL command removes the given nick from the
	AOP list.  If a list of entry numbers is given, those
	entries are deleted.  (See the example for LIST below.)
	
	The AOP LIST command displays the AOP list.  If
	a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   AOP #channel LIST 2-5,7-9
	      Lists AOP entries numbered 2 through 5 and
	      7 through 9.
	      
	The AOP CLEAR command clears all entries of the
	AOP list.
	
	The AOP ADD and AOP DEL commands are limited to
	SOPs or above, while the AOP CLEAR command can only
	be used by the channel founder. However, any user on the
	AOP list may use the AOP LIST command.
	
	This command may have been disabled for your channel, and
	in that case you need to use the access list. See 
	%R%S HELP ACCESS for information about the access list,
	and %R%S HELP SET XOP to know how to toggle between 
	the access list and xOP list systems.

CHAN_HELP_HOP
	Syntax: HOP channel ADD nick
	        HOP channel DEL {nick | entry-num | list}
	        HOP channel LIST [mask | list]
	        HOP channel CLEAR
	
	Maintains the HOP (HalfOP) list for a channel. The HOP 
	list gives users the right to be auto-halfopped on your 
	channel.
	
	The HOP ADD command adds the given nickname to the
	HOP list.
	
	The HOP DEL command removes the given nick from the
	HOP list.  If a list of entry numbers is given, those
	entries are deleted.  (See the example for LIST below.)
	
	The HOP LIST command displays the HOP list.  If
	a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   HOP #channel LIST 2-5,7-9
	      Lists HOP entries numbered 2 through 5 and
	      7 through 9.
	      
	The HOP CLEAR command clears all entries of the
	HOP list.
	
	The HOP ADD, HOP DEL and HOP LIST commands are 
	limited to AOPs or above, while the HOP CLEAR command 
	can only be used by the channel founder.
	
	This command may have been disabled for your channel, and
	in that case you need to use the access list. See 
	%R%S HELP ACCESS for information about the access list,
	and %R%S HELP SET XOP to know how to toggle between 
	the access list and xOP list systems.

CHAN_HELP_SOP
	Syntax: SOP channel ADD nick
	        SOP channel DEL {nick | entry-num | list}
	        SOP channel LIST [mask | list]
	        SOP channel CLEAR
	
	Maintains the SOP (SuperOP) list for a channel. The SOP 
	list gives users all rights given by the AOP list, and adds
	those needed to use the AutoKick and the BadWords lists, 
	to send and read channel memos, and so on.
	
	The SOP ADD command adds the given nickname to the
	SOP list.
	
	The SOP DEL command removes the given nick from the
	SOP list.  If a list of entry numbers is given, those
	entries are deleted.  (See the example for LIST below.)
	
	The SOP LIST command displays the SOP list.  If
	a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   SOP #channel LIST 2-5,7-9
	      Lists AOP entries numbered 2 through 5 and
	      7 through 9.
	      
	The SOP CLEAR command clears all entries of the
	SOP list.
	
	The SOP ADD, SOP DEL and SOP CLEAR commands are 
	limited to the channel founder. However, any user on the
	AOP list may use the SOP LIST command.
	
	This command may have been disabled for your channel, and
	in that case you need to use the access list. See 
	%R%S HELP ACCESS for information about the access list,
	and %R%S HELP SET XOP to know how to toggle between 
	the access list and xOP list systems.

CHAN_HELP_VOP
	Syntax: VOP channel ADD nick
	        VOP channel DEL {nick | entry-num | list}
	        VOP channel LIST [mask | list]
	        VOP channel CLEAR
	
	Maintains the VOP (VOicePeople) list for a channel.  
	The VOP list allows users to be auto-voiced and to voice 
	themselves if they aren't.
	
	The VOP ADD command adds the given nickname to the
	VOP list.
	
	The VOP DEL command removes the given nick from the
	VOP list.  If a list of entry numbers is given, those
	entries are deleted.  (See the example for LIST below.)
	
	The VOP LIST command displays the VOP list.  If
	a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   VOP #channel LIST 2-5,7-9
	      Lists VOP entries numbered 2 through 5 and
	      7 through 9.
	      
	The VOP CLEAR command clears all entries of the
	VOP list.
	
	The VOP ADD, VOP DEL and VOP LIST commands are 
	limited to AOPs or above, while the VOP CLEAR command 
	can only be used by the channel founder.
	
	This command may have been disabled for your channel, and
	in that case you need to use the access list. See 
	%R%S HELP ACCESS for information about the access list,
	and %R%S HELP SET XOP to know how to toggle between 
	the access list and xOP list systems.

CHAN_HELP_ACCESS
	Syntax: ACCESS channel ADD nick level
	        ACCESS channel DEL {nick | entry-num | list}
	        ACCESS channel LIST [mask | list]
	        ACCESS channel CLEAR
	
	Maintains the access list for a channel.  The access
	list specifies which users are allowed chanop status or
	access to %S commands on the channel.  Different
	user levels allow for access to different subsets of
	privileges; %R%S HELP ACCESS LEVELS for more
	specific information.  Any nick not on the access list has
	a user level of 0.
	
	The ACCESS ADD command adds the given nickname to the
	access list with the given user level; if the nick is
	already present on the list, its access level is changed to
	the level specified in the command.  The level specified
	must be less than that of the user giving the command, and
	if the nick is already on the access list, the current
	access level of that nick must be less than the access level
	of the user giving the command.
	
	The ACCESS DEL command removes the given nick from the
	access list.  If a list of entry numbers is given, those
	entries are deleted.  (See the example for LIST below.)
	
	The ACCESS LIST command displays the access list.  If
	a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   ACCESS #channel LIST 2-5,7-9
	      Lists access entries numbered 2 through 5 and
	      7 through 9.
	      
	The ACCESS CLEAR command clears all entries of the
	access list.

CHAN_HELP_ACCESS_LEVELS
	User access levels
	
	By default, the following access levels are defined:
	
	   Founder   Full access to %S functions; automatic
	                     opping upon entering channel.  Note
	                     that only one person may have founder
	                     status (it cannot be given using the
	                     ACCESS command).
	        10   Access to AKICK command; automatic opping.
	         5   Automatic opping.
	         3   Automatic voicing.
	         0   No special privileges; can be opped by other
	                     ops (unless secure-ops is set).
	        <0   May not be opped.
	
	These levels may be changed, or new ones added, using the
	LEVELS command; type %R%S HELP LEVELS for
	information.

CHAN_HELP_AKICK
	Syntax: AKICK channel ADD {nick | mask} [reason]
	        AKICK channel STICK mask
	        AKICK channel UNSTICK mask
	        AKICK channel DEL {nick | mask | entry-num | list}
	        AKICK channel LIST [mask | entry-num | list]
	        AKICK channel VIEW [mask | entry-num | list]
	        AKICK channel ENFORCE
	        AKICK channel CLEAR
	
	Maintains the AutoKick list for a channel.  If a user
	on the AutoKick list attempts to join the channel,
	%S will ban that user from the channel, then kick
	the user.
	
	The AKICK ADD command adds the given nick or usermask
	to the AutoKick list.  If a reason is given with
	the command, that reason will be used when the user is
	kicked; if not, the default reason is "You have been
	banned from the channel".
	When akicking a registered nick the nickserv account
	will be added to the akick list instead of the mask.
	All users within that nickgroup will then be akicked.
	
	The AKICK STICK command permanently bans the given mask 
	on the channel. If someone tries to remove the ban, %S
	will automatically set it again. You can't use it for
	registered nicks.
	
	The AKICK UNSTICK command cancels the effect of the
	AKICK STICK command, so you'll be able to unset the
	ban again on the channel.
	
	The AKICK DEL command removes the given nick or mask
	from the AutoKick list.  It does not, however, remove any
	bans placed by an AutoKick; those must be removed
	manually.
	
	The AKICK LIST command displays the AutoKick list, or
	optionally only those AutoKick entries which match the
	given mask.
	
	The AKICK VIEW command is a more verbose version of
	AKICK LIST command.
	
	The AKICK ENFORCE command causes %S to enforce the
	current AKICK list by removing those users who match an
	AKICK mask.
	
	The AKICK CLEAR command clears all entries of the
	akick list.

CHAN_HELP_LEVELS
	Syntax: LEVELS channel SET type level
	        LEVELS channel {DIS | DISABLE} type
	        LEVELS channel LIST
	        LEVELS channel RESET
	
	The LEVELS command allows fine control over the meaning of
	the numeric access levels used for channels.  With this
	command, you can define the access level required for most
	of %S's functions.  (The SET FOUNDER and SET PASSWORD
	commands, as well as this command, are always restricted to
	the channel founder.)
	
	LEVELS SET allows the access level for a function or group of
	functions to be changed.  LEVELS DISABLE (or DIS for short)
	disables an automatic feature or disallows access to a
	function by anyone other than the channel founder.
	LEVELS LIST shows the current levels for each function or
	group of functions.  LEVELS RESET resets the levels to the
	default levels of a newly-created channel (see
	HELP ACCESS LEVELS).
	
	For a list of the features and functions whose levels can be
	set, see HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	The following feature/function names are understood.  Note
	that the levels for AUTODEOP and NOJOIN are maximum levels,
	while all others are minimum levels.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Syntax: INFO channel [ALL]
	
	Lists information about the named registered channel,
	including its founder, time of registration, last time
	used, description, and mode lock, if any. If ALL is 
	specified, the entry message and successor will also 
	be displayed.
	
	By default, the ALL option is limited to those with
	founder access on the channel.

CHAN_HELP_LIST
	Syntax: LIST pattern
	
	Lists all registered channels matching the given pattern.
	(Channels with the PRIVATE option set are not listed.)
	Note that a preceding '#' specifies a range, channel names
	are to be written without '#'.

CHAN_HELP_OP
	Syntax: OP [#channel [nick]]
	
	Ops a selected nick on a channel. If nick is not given,
	it will op you. If channel and nick are not given,
	it will op you on all channels you're on, provided you
	have the rights to.
	
	By default, limited to AOPs or those with level 5 access 
	and above on the channel.

CHAN_HELP_DEOP
	Syntax: DEOP [#channel [nick]]
	
	Deops a selected nick on a channel. If nick is not given,
	it will deop you. If channel and nick are not given,
	it will deop you on all channels you're on, provided you
	have the rights to.
	
	By default, limited to AOPs or those with level 5 access 
	and above on the channel.
	
CHAN_HELP_VOICE
	Syntax: VOICE [#channel [nick]]
	
	Voices a selected nick on a channel. If nick is not given,
	it will voice you. If channel and nick are not given,
	it will voice you on all channels you're on, provided you
	have the rights to.
	
	By default, limited to AOPs or those with level 5 access 
	and above on the channel, or to VOPs or those with level 3 
	and above for self voicing.

CHAN_HELP_DEVOICE
	Syntax: DEVOICE [#channel [nick]]
	
	Devoices a selected nick on a channel. If nick is not given,
	it will devoice you. If channel and nick are not given,
	it will devoice you on all channels you're on, provided you
	have the rights to.
	
	By default, limited to AOPs or those with level 5 access 
	and above on the channel, or to VOPs or those with level 3 
	and above for self devoicing.
	
CHAN_HELP_HALFOP
	Syntax: HALFOP [#channel [nick]]
	
	Halfops a selected nick on a channel. If nick is not given,
	it will halfop you. If channel and nick are not given,
	it will halfop you on all channels you're on, provided you
	have the rights to.
	
	By default, limited to AOPs and those with level 5 access 
	and above on the channel, or to HOPs or those with level 4 
	and above for self halfopping.

CHAN_HELP_DEHALFOP
	Syntax: DEHALFOP [#channel [nick]]
	
	Dehalfops a selected nick on a channel. If nick is not given,
	it will dehalfop you. If channel and nick are not given,
	it will dehalfop you on all channels you're on, provided you
	have the rights to.
	
	By default, limited to AOPs and those with level 5 access 
	and above on the channel, or to HOPs or those with level 4 
	and above for self dehalfopping.
	
CHAN_HELP_PROTECT
	Syntax: PROTECT [#channel [nick]]
	
	Protects a selected nick on a channel. If nick is not given,
	it will protect you. If channel and nick are not given,
	it will protect you on all channels you're on, provided you
	have the rights to.
	
	By default, limited to the founder, or to SOPs or those with 
	level 10 and above on the channel for self protecting.

CHAN_HELP_DEPROTECT
	Syntax: DEPROTECT [#channel [nick]]
	
	Deprotects a selected nick on a channel. If nick is not given,
	it will deprotect you. If channel and nick are not given,
	it will deprotect you on all channels you're on, provided you
	have the rights to.
	
	By default, limited to the founder, or to SOPs or those with 
	level 10 and above on the channel for self deprotecting.

CHAN_HELP_OWNER
	Syntax: OWNER [#channel]
	
	Gives you owner status on channel. If channel is not 
	given, it will give you owner status on all channels you're 
	on, provided you have the rights to.
	
	Limited to those with founder access on the channel.

CHAN_HELP_DEOWNER
	Syntax: DEOWNER [#channel]
	
	Removes your owner status on channel. If channel is 
	not given, it will remove your owner status on all channels 
	you're on, provided you have the rights to.
	
	Limited to those with founder access on the channel.

CHAN_HELP_INVITE
	Syntax: INVITE channel
	
	Tells %S to invite you into the given channel.  
	
	By default, limited to AOPs or those with level 5 and above
	on the channel.

CHAN_HELP_UNBAN
	Syntax: UNBAN channel
	
	Tells %S to remove all bans preventing you from
	entering the given channel.  
	
	By default, limited to AOPs or those with level 5 and above
	on the channel.
	
CHAN_HELP_KICK
	Syntax: KICK [#channel [nick [reason]]]
	
	Kicks a selected nick on a channel. If nick is not given,
	it will kick you. If channel and nick are not given,
	it will kick you on all channels you're on, provided you
	have the rights to.
	
	By default, limited to AOPs or those with level 5 access 
	and above on the channel.

CHAN_HELP_BAN
	Syntax: BAN [#channel [nick [reason]]]
	
	Bans a selected nick on a channel. If nick is not given,
	it will ban you. If channel and nick are not given,
	it will ban you on all channels you're on, provided you
	have the rights to.
	
	By default, limited to AOPs or those with level 5 access 
	and above on the channel.

CHAN_HELP_TOPIC
	Syntax: TOPIC channel [topic]
	
	Causes %S to set the channel topic to the one
	specified. If topic is not given, then an empty topic
	is set. This command is most useful in conjunction
	with SET TOPICLOCK. See %R%S HELP SET TOPICLOCK
	for more information.
	
	By default, limited to those with founder access on the
	channel. 

CHAN_HELP_CLEAR
	Syntax: CLEAR channel what
	
	Tells %S to clear certain settings on a channel.  what
	can be any of the following:
	
	     MODES    Resets all modes on the channel (i.e. clears
	                  modes i,k,l,m,n,p,s,t).
	     BANS     Clears all bans on the channel.
	     EXCEPTS  Clears all excepts on the channel.
	     INVITES  Clears all invites on the channel.
	     OPS      Removes channel-operator status (mode +o) from
	                  all channel operators.
	     HOPS     Removes channel-halfoperator status (mode +h) from
	                  all channel halfoperators, if supported.
	     VOICES   Removes "voice" status (mode +v) from anyone
	                  with that mode set.
	     USERS    Removes (kicks) all users from the channel.
	
	By default, limited to those with founder access on the
	channel.
	
CHAN_HELP_GETKEY
	Syntax: GETKEY channel
	
	Returns the key of the given channel. This is a command
	mainly intended to be used by bots and/or scripts, so
	the output is in the following way:
	
	KEY <channel> <key>
	
	key is "NO KEY" if no key is set.
	
CHAN_HELP_SENDPASS
	Syntax: SENDPASS channel
	
	Send the password of the given channel to the e-mail address
	set in the founder's nickname record. This command is really 
	useful to deal with lost passwords.
	
	May be limited to IRC operators on certain networks.
	
	This command is unavailable when encryption is enabled.

CHAN_SERVADMIN_HELP
	 
	Services admins can also drop any channel without needing
	to identify via password, and may view the access, AKICK,
	and level setting lists for any channel.
	
CHAN_SERVADMIN_HELP_LOGOUT
	Syntax: LOGOUT channel [nickname]
	
	This command will log the selected nickname out meaning they 
	would have to reidentify themselves to regain their access.
	
	If you are the founder of the channel, you can log out anybody,
	else you can only log out yourself.
	
	If you are a Services admin, you can log out 
	anybody of any channel without having to be the founder
	of the channel. Also, you can omit the nickname parameter;
	this will log out all identified users from the channel.

CHAN_SERVADMIN_HELP_DROP
	Syntax: DROP channel
	
	Unregisters the named channel.  Only Services admins
	can drop a channel for which they have not identified.

CHAN_SERVADMIN_HELP_SET
	
	Services admins can also set the option NOEXPIRE, with
	which channels can be prevented from expiring.
	Additionally, Services admins can set options for any
	channel without identifying by password for the channel.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Syntax: SET channel NOEXPIRE {ON | OFF}
	
	Sets whether the given channel will expire.  Setting this
	to ON prevents the channel from expiring.

CHAN_SERVADMIN_HELP_INFO
	
	Services admins can use the ALL parameter with any channel.

CHAN_SERVADMIN_HELP_LIST
	Syntax: LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
	
	Lists all registered channels matching the given pattern.
	Channels with the PRIVATE option set will only be displayed
	to Services admins. Channels with the NOEXPIRE option set 
	will have a ! appended to the channel name for Services admins.
	
	If the FORBIDDEN, SUSPENDED or NOEXPIRE options are given, only 
	channels which, respectively, are FORBIDden, SUSPENDed or have 
	the NOEXPIRE flag set will be displayed.  If multiple options are 
	given, more types of channels will be displayed. These options are 
	limited to Services admins.

CHAN_SERVADMIN_HELP_GETPASS
	Syntax: GETPASS channel
	
	Returns the password for the given channel.  Note that
	whenever this command is used, a message including the
	person who issued the command and the channel it was used
	on will be logged and sent out as a WALLOPS/GLOBOPS.

CHAN_SERVADMIN_HELP_FORBID
	Syntax: FORBID channel [reason]
	
	Disallows anyone from registering or using the given
	channel.  May be cancelled by dropping the channel.
	
	Reason may be required on certain networks.

CHAN_SERVADMIN_HELP_SUSPEND
	Syntax: SUSPEND channel [reason]
	
	Disallows anyone from registering or using the given
	channel.  May be cancelled by using the UNSUSPEND
	command to preserve all previous channel data/settings.
	
	Reason may be required on certain networks.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Syntax: UNSUSPEND channel
	
	Releases a suspended channel. All data and settings
	are preserved from before the suspension.

CHAN_SERVADMIN_HELP_STATUS
	Syntax: STATUS channel nickname
	
	Returns the current access level of the given nick on the
	given channel.  The reply is of the form:
	
	    STATUS channel nickname access-level
	
	If an error occurs, the reply will be in the form:
	
	    STATUS ERROR error-message

###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP_CMD_SEND
	    SEND   Send a memo to a nick or channel
MEMO_HELP_CMD_CANCEL
	    CANCEL Cancel last memo you sent
MEMO_HELP_CMD_LIST
	    LIST   List your memos
MEMO_HELP_CMD_READ
	    READ   Read a memo or memos
MEMO_HELP_CMD_DEL
	    DEL    Delete a memo or memos
MEMO_HELP_CMD_SET
	    SET    Set options related to memos
MEMO_HELP_CMD_INFO
	    INFO   Displays information about your memos
MEMO_HELP_CMD_RSEND
	    RSEND  Sends a memo and requests a read receipt
MEMO_HELP_CMD_CHECK
	    CHECK  Checks if last memo to a nick was read
MEMO_HELP_CMD_SENDALL
	    SENDALL  Send a memo to all registered users
MEMO_HELP_CMD_STAFF
	    STAFF  Send a memo to all opers/admins

MEMO_HELP_HEADER
	%S is a utility allowing IRC users to send short
	messages to other IRC users, whether they are online at
	the time or not, or to channels(*).  Both the sender's
	nickname and the target nickname or channel must be
	registered in order to send a memo.

	%S's commands include:


MEMO_HELP_ADMIN
	not used.
MEMO_HELP_OPER
	not used.

MEMO_HELP_FOOTER
	
	Type %R%S HELP command for help on any of the
	above commands.

	(*) By default, any user with at least level 10 access on a
	    channel can read that channel's memos.  This can be
	    changed with the %s LEVELS command.

MEMO_HELP_SEND
	Syntax: SEND {nick | channel} memo-text
	
	Sends the named nick or channel a memo containing
	memo-text.  When sending to a nickname, the recipient will
	receive a notice that he/she has a new memo.  The target
	nickname/channel must be registered.
	
MEMO_HELP_CANCEL
	Syntax: CANCEL {nick | channel}
	
	Cancels the last memo you sent to the given nick or channel,
	provided it has not been read at the time you use the command.

MEMO_HELP_LIST
	Syntax: LIST [channel] [list | NEW]
	
	Lists any memos you currently have.  With NEW, lists only
	new (unread) memos.  Unread memos are marked with a "*"
	to the left of the memo number.  You can also specify a list
	of numbers, as in the example below:

	   LIST 2-5,7-9
	      Lists memos numbered 2 through 5 and 7 through 9.

MEMO_HELP_READ
	Syntax: READ [channel] {num | list | LAST | NEW}
	
	Sends you the text of the memos specified.  If LAST is
	given, sends you the memo you most recently received.  If
	NEW is given, sends you all of your new memos.  Otherwise,
	sends you memo number num.  You can also give a list of
	numbers, as in this example:
	
	   READ 2-5,7-9
	      Displays memos numbered 2 through 5 and 7 through 9.

MEMO_HELP_DEL
	Syntax: DEL [channel] {num | list | LAST | ALL}
	
	Deletes the specified memo or memos.  You can supply
	multiple memo numbers or ranges of numbers instead of a
	single number, as in the second example below.
	
	If LAST is given, the last memo will be deleted.
	If ALL is given, deletes all of your memos.
	
	Examples:
	
	   DEL 1
	      Deletes your first memo.
	
	   DEL 2-5,7-9
	      Deletes memos numbered 2 through 5 and 7 through 9.

MEMO_HELP_SET
	Syntax: SET option parameters

	Sets various memo options.  option can be one of:
	
	    NOTIFY      Changes when you will be notified about
	                    new memos (only for nicknames)
	    LIMIT       Sets the maximum number of memos you can
	                    receive
	
	Type %R%S HELP SET option for more information
	on a specific option.

MEMO_HELP_SET_NOTIFY
	Syntax: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}

	Changes when you will be notified about new memos:
	
	    ON      You will be notified of memos when you log on,
	               when you unset /AWAY, and when they are sent
	               to you.
	    LOGON   You will only be notified of memos when you log
	               on or when you unset /AWAY.
	    NEW     You will only be notified of memos when they
	               are sent to you.
	    MAIL    You will be notified of memos by email aswell as
	               any other settings you have.
	    NOMAIL  You will not be notified of memos by email.
	    OFF     You will not receive any notification of memos.
	
	ON is essentially LOGON and NEW combined.

MEMO_HELP_SET_LIMIT
	Syntax: SET LIMIT [channel] limit
	
	Sets the maximum number of memos you (or the given channel)
	are allowed to have. If you set this to 0, no one will be
	able to send any memos to you.  However, you cannot set
	this any higher than %d.

MEMO_HELP_INFO
	Syntax: INFO [channel]
	
	Displays information on the number of memos you have, how
	many of them are unread, and how many total memos you can
	receive.  With a parameter, displays the same information
	for the given channel.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Syntax: SET LIMIT [user | channel] {limit | NONE} [HARD]
	
	Sets the maximum number of memos a user or channel is
	allowed to have.  Setting the limit to 0 prevents the user
	from receiving any memos; setting it to NONE allows the
	user to receive and keep as many memos as they want.  If
	you do not give a nickname or channel, your own limit is
	set.
	
	Adding HARD prevents the user from changing the limit.  Not
	adding HARD has the opposite effect, allowing the user to
	change the limit (even if a previous limit was set with
	HARD).

	This use of the SET LIMIT command is limited to Services
	admins.  Other users may only enter a limit for themselves
	or a channel on which they have such privileges, may not
	remove their limit, may not set a limit above %d, and may
	not set a hard limit.

MEMO_SERVADMIN_HELP_INFO
	Syntax: INFO [nick | channel]

	Without a parameter, displays information on the number of
	memos you have, how many of them are unread, and how many
	total memos you can receive.
	
	With a channel parameter, displays the same information for
	the given channel.
	
	With a nickname parameter, displays the same information
	for the given nickname.  This use limited to Services
	admins.

MEMO_HELP_STAFF
	Syntax: STAFF memo-text

	Sends all services staff a memo containing memo-text.
	Note: If you have opers on both the oper list and the
	admin list they will receive the memo twice.  The same
	applies for oper's on the Root list as well as other
	lists.

MEMO_HELP_SENDALL
	Syntax: SENDALL memo-text

	Sends all registered users a memo containing memo-text.
	
MEMO_HELP_RSEND
	Syntax: RSEND {nick | channel} memo-text
	
	Sends the named nick or channel a memo containing
	memo-text.  When sending to a nickname, the recipient will
	receive a notice that he/she has a new memo.  The target
	nickname/channel must be registered.
	Once the memo is read by its recipient, an automatic notification
	memo will be sent to the sender informing him/her that the memo
	has been read.

MEMO_HELP_CHECK
	Syntax: CHECK nick
	
	Checks whether the _last_ memo you sent to nick has been read
	or not. Note that this does only work with nicks, not with chans.

###########################################################################
#
# OperServ help messages
#
###########################################################################
OPER_HELP_CMD_GLOBAL
	    GLOBAL      Send a message to all users
OPER_HELP_CMD_STATS
	    STATS       Show status of Services and network
OPER_HELP_CMD_OPER
	    OPER        Modify the Services operator list
OPER_HELP_CMD_ADMIN
	    ADMIN       Modify the Services admin list
OPER_HELP_CMD_STAFF
	    STAFF       Display Services staff and online status
OPER_HELP_CMD_MODE
	    MODE        Change a channel's modes
OPER_HELP_CMD_KICK
	    KICK        Kick a user from a channel
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  Clear modes of a channel
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  Kill all users that have a certain host
OPER_HELP_CMD_AKILL
	    AKILL       Manipulate the AKILL list
OPER_HELP_CMD_SGLINE
	    SGLINE      Manipulate the SGLINE list
OPER_HELP_CMD_SQLINE
	    SQLINE      Manipulate the SQLINE list
OPER_HELP_CMD_SZLINE
	    SZLINE      Manipulate the SZLINE list
OPER_HELP_CMD_CHANLIST
	    CHANLIST    Lists all channel records
OPER_HELP_CMD_USERLIST
	    USERLIST    Lists all user records
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   Define messages to be shown to users at logon
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  Define messages to be randomly shown to users 
	                at logon
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    Define messages to be shown to users who oper
OPER_HELP_CMD_SESSION
	    SESSION     View the list of host sessions
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   Modify the session-limit exception list
OPER_HELP_CMD_NOOP
	    NOOP        Temporarily remove all O:lines of a server 
	                remotely

OPER_HELP_CMD_JUPE
	    JUPE        "Jupiter" a server
OPER_HELP_CMD_IGNORE
	    IGNORE      Modify the Services ignore list
OPER_HELP_CMD_SET
	    SET         Set various global Services options
OPER_HELP_CMD_RELOAD
	    RELOAD      Reload services' configuration file
OPER_HELP_CMD_UPDATE
	    UPDATE      Force the Services databases to be
	                updated on disk immediately
OPER_HELP_CMD_RESTART
	    RESTART     Save databases and restart Services
OPER_HELP_CMD_QUIT
	    QUIT        Terminate the Services program with no save
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    Terminate the Services program with save
OPER_HELP_CMD_DEFCON
	    DEFCON      Manipulate the DefCon system
OPER_HELP_CMD_CHANKILL
	    CHANKILL    AKILL all users on a specific channel
OPER_HELP_CMD_OLINE
	    OLINE       Give Operflags to a certain user
OPER_HELP_CMD_UMODE
	    UMODE       Change a user's modes
OPER_HELP_CMD_SVSNICK
	    SVSNICK     Forcefully change a user's nickname
OPER_HELP_CMD_MODLOAD
	    MODLOAD     Load a module
OPER_HELP_CMD_MODUNLOAD
	    MODUNLOAD   Un-Load a module
OPER_HELP_CMD_MODINFO
	    MODINFO     Info about a loaded module
OPER_HELP_CMD_MODLIST
	    MODLIST     List loaded modules

OPER_HELP
	%S commands:

OPER_HELP_LOGGED
	Notice: All commands sent to %S are logged!

OPER_HELP_GLOBAL
	Syntax: GLOBAL message
	
	Allows Administrators to send messages to all users on the 
	network. The message will be sent from the nick %s.

OPER_HELP_STATS
	Syntax: STATS [AKILL | ALL | RESET | MEMORY | UPLINK]
	
	Without any option, shows the current number of users and
	IRCops online (excluding Services), the highest number of
	users online since Services was started, and the length of
	time Services has been running.
	
	With the AKILL option, displays the current size of the
	AKILL list and the current default expiry time.
	
	The RESET option currently resets the maximum user count
	to the number of users currently present on the network.
	
	The MEMORY option displays information on the memory
	usage of Services. Using this option can freeze Services for
	a short period of time on large networks; don't overuse it!
	
	The UPLINK option displays information about the current
	server Anope uses as an uplink to the network.
	
	The ALL displays the user and uptime statistics, and
	everything you'd see with MEMORY and UPLINK options.
	
	UPTIME may be used as a synonym for STATS.

OPER_HELP_OPER
	Syntax: OPER ADD nick
	        OPER DEL {nick | entry-num | list}
	        OPER LIST [mask | list]
	        OPER CLEAR
	
	Allows the Services Root Admins to add or remove nicknames
	to or from the Services operator list.  A user whose nickname
	is on the Services operator list and who has identified to
	%s will be able to access Services operator commands.
	
	The OPER ADD command adds the given nickname to the
	Services operator list.
	
	The OPER DEL command removes the given nick from the
	Services operator list.  If a list of entry numbers is given, 
	those entries are deleted.  (See the example for LIST below.)
	
	The OPER LIST command displays the Services operator list.  
	If a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   OPER LIST 2-5,7-9
	      Lists Services operator entries numbered 2 through 
	      5 and 7 through 9.
	      
	The OPER CLEAR command clears all entries of the
	Services operator list.
	
	Any IRC operator may use the OPER LIST form of the command.

OPER_HELP_ADMIN
	Syntax: ADMIN ADD nick
	        ADMIN DEL {nick | entry-num | list}
	        ADMIN LIST [mask | list]
	        ADMIN CLEAR
	
	Allows the Services root to add or remove nicknames
	to or from the Services admin list.  A user whose nickname
	is on the Services admin list and who has identified to
	%s will be able to access Services admin commands.
	
	The ADMIN ADD command adds the given nickname to the
	Services admin list.
	
	The ADMIN DEL command removes the given nick from the
	Services admin list.  If a list of entry numbers is given,
	those entries are deleted.  (See the example for LIST below.)
	
	The ADMIN LIST command displays the Services admin list.  
	If a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   ADMIN LIST 2-5,7-9
	      Lists Services admin entries numbered 2 through 
	      5 and 7 through 9.
	      
	The ADMIN CLEAR command clears all entries of the
	Services admin list.
	
	Any IRC operator may use the ADMIN LIST form of the command.
	All other use limited to Services root.

OPER_HELP_IGNORE
	Syntax: IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick | mask]
	
	Allows Services Admins to make Services ignore a nick or mask
	for a certain time or until the next restart. The default
	time format is seconds. You can specify it by using units.
	Valid units are: s for seconds, m for minutes, 
	h for hours and d for days. 
	Combinations of these units are not permitted.
	To make Services permanently ignore the	user, type 0 as time.
	When adding a mask, it should be in the format user@host
	or nick!user@host, everything else will be considered a nick.
	Wildcards are permitted.
	
	Ignores will not be enforced on IRC Operators.

OPER_HELP_MODE
	Syntax: MODE channel modes
	
	Allows Services operators to set channel modes for any
	channel.  Parameters are the same as for the standard /MODE
	command.

OPER_HELP_UMODE
	Syntax: UMODE user modes
	
	Allows Super Admins to set user modes for any user.
	Parameters are the same as for the standard /MODE
	command.

OPER_HELP_OLINE
	Syntax: OLINE user flags
	
	Allows Super Admins to give Operflags to any user.
	Flags have to be prefixed with a "+" or a "-". To
	remove all flags simply type a "-" instead of any flags.

OPER_HELP_CLEARMODES
	Syntax: CLEARMODES channel [ALL]
	
	Clears all binary modes (i,k,l,m,n,p,s,t) and bans from a
	channel.  If ALL is given, also clears all ops and
	voices (+o and +v modes) from the channel.

OPER_HELP_KICK
	Syntax: KICK channel user reason
	
	Allows staff to kick a user from any channel.
	Parameters are the same as for the standard /KICK
	command.  The kick message will have the nickname of the
	IRCop sending the KICK command prepended; for example:
	
	*** SpamMan has been kicked off channel #my_channel by %S (Alcan (Flood))

OPER_HELP_SVSNICK
	Syntax: SVSNICK nick newnick
	
	Forcefully changes a user's nickname from nick to newnick.
	Limited to Super Admins.

OPER_HELP_AKILL
	Syntax: AKILL ADD [+expiry] mask reason
	        AKILL DEL {mask | entry-num | list}
	        AKILL LIST [mask | list]
	        AKILL VIEW [mask | list]
	        AKILL CLEAR
	
	Allows Services operators to manipulate the AKILL list.  If
	a user matching an AKILL mask attempts to connect, Services
	will issue a KILL for that user and, on supported server
	types, will instruct all servers to add a ban (K-line) for
	the mask which the user matched.
	
	AKILL ADD adds the given user@host/ip mask to the AKILL
	list for the given reason (which must be given).
	expiry is specified as an integer followed by one of d 
	(days), h (hours), or m (minutes).  Combinations (such as 
	1h30m) are not permitted.  If a unit specifier is not 
	included, the default is days (so +30 by itself means 30 
	days).  To add an AKILL which does not expire, use +0.  If the
	usermask to be added starts with a +, an expiry time must
	be given, even if it is the same as the default.  The
	current AKILL default expiry time can be found with the
	STATS AKILL command.
	
	The AKILL DEL command removes the given mask from the
	AKILL list if it is present.  If a list of entry numbers is 
	given, those entries are deleted.  (See the example for LIST 
	below.)
	
	The AKILL LIST command displays the AKILL list.  
	If a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   AKILL LIST 2-5,7-9
	      Lists AKILL entries numbered 2 through 5 and 7 
	      through 9.
	      
	AKILL VIEW is a more verbose version of AKILL LIST, and 
	will show who added an AKILL, the date it was added, and when 
	it expires, as well as the user@host/ip mask and reason.
	
	AKILL CLEAR clears all entries of the AKILL list.
	
OPER_HELP_SGLINE
	Syntax: SGLINE ADD [+expiry] mask:reason
	    SGLINE DEL {mask | entry-num | list}
	        SGLINE LIST [mask | list]
	        SGLINE VIEW [mask | list]
	        SGLINE CLEAR
	
	Allows Services operators to manipulate the SGLINE list.  If
	a user with a realname matching an SGLINE mask attempts to 
	connect, Services will not allow it to pursue his IRC
	session.
	
	SGLINE ADD adds the given realname mask to the SGLINE
	list for the given reason (which must be given).
	expiry is specified as an integer followed by one of d 
	(days), h (hours), or m (minutes).  Combinations (such as 
	1h30m) are not permitted.  If a unit specifier is not 
	included, the default is days (so +30 by itself means 30 
	days).  To add an SGLINE which does not expire, use +0.  If the
	realname mask to be added starts with a +, an expiry time must
	be given, even if it is the same as the default.  The
	current SGLINE default expiry time can be found with the
	STATS AKILL command.
	Note: because the realname mask may contain spaces, the
	separator between it and the reason is a colon.
	
	The SGLINE DEL command removes the given mask from the
	SGLINE list if it is present.  If a list of entry numbers is 
	given, those entries are deleted.  (See the example for LIST 
	below.)
	
	The SGLINE LIST command displays the SGLINE list.  
	If a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   SGLINE LIST 2-5,7-9
	      Lists SGLINE entries numbered 2 through 5 and 7 
	      through 9.
	      
	SGLINE VIEW is a more verbose version of SGLINE LIST, and 
	will show who added an SGLINE, the date it was added, and when 
	it expires, as well as the realname mask and reason.
	
	SGLINE CLEAR clears all entries of the SGLINE list.

OPER_HELP_SQLINE
	Syntax: SQLINE ADD [+expiry] mask reason
	        SQLINE DEL {mask | entry-num | list}
	        SQLINE LIST [mask | list]
	        SQLINE VIEW [mask | list]
	        SQLINE CLEAR
	
	Allows Services operators to manipulate the SQLINE list.  If
	a user with a nick matching an SQLINE mask attempts to 
	connect, Services will not allow it to pursue his IRC
	session.

	If the first character of the mask is #, services will 
	prevent the use of matching channels (on IRCds that 
	support it).
	
	SQLINE ADD adds the given mask to the SQLINE
	list for the given reason (which must be given).
	expiry is specified as an integer followed by one of d 
	(days), h (hours), or m (minutes).  Combinations (such as 
	1h30m) are not permitted.  If a unit specifier is not 
	included, the default is days (so +30 by itself means 30 
	days).  To add an SQLINE which does not expire, use +0.  
	If the mask to be added starts with a +, an expiry time 
	must be given, even if it is the same as the default. The
	current SQLINE default expiry time can be found with the
	STATS AKILL command.
	
	The SQLINE DEL command removes the given mask from the
	SQLINE list if it is present.  If a list of entry numbers is 
	given, those entries are deleted.  (See the example for LIST 
	below.)
	
	The SQLINE LIST command displays the SQLINE list.  
	If a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   SQLINE LIST 2-5,7-9
	      Lists SQLINE entries numbered 2 through 5 and 7 
	      through 9.
	      
	SQLINE VIEW is a more verbose version of SQLINE LIST, and 
	will show who added an SQLINE, the date it was added, and when 
	it expires, as well as the mask and reason.
	
	SQLINE CLEAR clears all entries of the SQLINE list.

OPER_HELP_SZLINE
	Syntax: SZLINE ADD [+expiry] mask reason
	        SZLINE DEL {mask | entry-num | list}
	        SZLINE LIST [mask | list]
	        SZLINE VIEW [mask | list]
	        SZLINE CLEAR
	
	Allows Services operators to manipulate the SZLINE list.  If
	a user with an IP matching an SZLINE mask attempts to 
	connect, Services will not allow it to pursue his IRC
	session (and this, whether the IP has a PTR RR or not).
	
	SZLINE ADD adds the given IP mask to the SZLINE
	list for the given reason (which must be given).
	expiry is specified as an integer followed by one of d 
	(days), h (hours), or m (minutes).  Combinations (such as 
	1h30m) are not permitted.  If a unit specifier is not 
	included, the default is days (so +30 by itself means 30 
	days).  To add an SZLINE which does not expire, use +0.  If the
	realname mask to be added starts with a +, an expiry time must
	be given, even if it is the same as the default.  The
	current SZLINE default expiry time can be found with the
	STATS AKILL command.
	
	The SZLINE DEL command removes the given mask from the
	SZLINE list if it is present.  If a list of entry numbers is 
	given, those entries are deleted.  (See the example for LIST 
	below.)
	
	The SZLINE LIST command displays the SZLINE list.  
	If a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   SZLINE LIST 2-5,7-9
	      Lists SZLINE entries numbered 2 through 5 and 7 
	      through 9.
	      
	SZLINE VIEW is a more verbose version of SZLINE LIST, and 
	will show who added an SZLINE, the date it was added, and when
	it expires, as well as the IP mask and reason.
	
	SZLINE CLEAR clears all entries of the SZLINE list.

OPER_HELP_SET
	Syntax: SET option setting

	Sets various global Services options.  Option names
	currently defined are:
	    READONLY   Set read-only or read-write mode
	    LOGCHAN    Report log messages to a channel
	    DEBUG      Activate or deactivate debug mode
	    NOEXPIRE   Activate or deactivate no expire mode
	    SUPERADMIN Activate or deactivate super-admin mode
	    SQL        Activate or deactivate sql mode
	    IGNORE     Activate or deactivate ignore mode
	    LIST       List the options

OPER_HELP_SET_READONLY
	Syntax: SET READONLY {ON | OFF}
	
	Sets read-only mode on or off.  In read-only mode, normal
	users will not be allowed to modify any Services data,
	including channel and nickname access lists, etc.  IRCops
	with sufficient Services privileges will be able to modify
	Services' AKILL list and drop or forbid nicknames and
	channels, but any such changes will not be saved unless
	read-only mode is deactivated before Services is terminated
	or restarted.
	
	This option is equivalent to the command-line option
	-readonly.

OPER_HELP_SET_LOGCHAN
	Syntax: SET LOGCHAN {ON | OFF}

	With this setting on, Services will send its logs to a specified
	channel as well as the log file. LogChannel must also be defined
	in the Services configuration file for this setting to be of any
	use.
	
	This option is equivalent to the command-line option -logchan.
	
	Note: This can have strong security implications if your log
	channel is not properly secured.

OPER_HELP_SET_DEBUG
	Syntax: SET DEBUG {ON | OFF | num}
	
	Sets debug mode on or off.  In debug mode, all data sent to
	and from Services as well as a number of other debugging
	messages are written to the log file.  If num is
	given, debug mode is activated, with the debugging level set
	to num.

	This option is equivalent to the command-line option
	-debug.

OPER_HELP_SET_NOEXPIRE
	Syntax: SET NOEXPIRE {ON | OFF}

	Sets no expire mode on or off.  In no expire mode, nicks,
	channels, akills and exceptions won't expire until the
	option is unset.

	This option is equivalent to the command-line option
	-noexpire.

OPER_HELP_SET_SUPERADMIN
	Syntax: SET SUPERADMIN {ON | OFF}

	Setting this will grant you extra privileges such as the
	ability to be "founder" on all channel's etc...

	This option is not persistent, and should only be used when
	needed, and set back to OFF when no longer needed.

OPER_HELP_SET_SQL
	Syntax: SET SQL {ON | OFF}

	Setting this will toggle Anope's usage of SQL, this should
	be used to disable and enable SQL should your SQL server go down
	while services are running.

OPER_HELP_SET_IGNORE
	Syntax: SET IGNORE {ON | OFF}

	Setting this will toggle Anope's usage of the IGNORE system 
	on or off.

OPER_HELP_SET_LIST
	Syntax: SET LIST

	Display the various %S settings

OPER_HELP_NOOP
	Syntax: NOOP SET server
	        NOOP REVOKE server

	NOOP SET remove all O:lines of the given
	server and kill all IRCops currently on it to
	prevent them from rehashing the server (because this
	would just cancel the effect).

	NOOP REVOKE makes all removed O:lines available again
	on the given server.

	Note: The server is not checked at all by the
	Services.

OPER_HELP_JUPE
	Syntax: JUPE server [reason]
	
	Tells Services to jupiter a server -- that is, to create
	a fake "server" connected to Services which prevents
	the real server of that name from connecting.  The jupe
	may be removed using a standard SQUIT.  If a reason is
	given, it is placed in the server information field;
	otherwise, the server information field will contain the
	text "Juped by <nick>", showing the nickname of the
	person who jupitered the server.

OPER_HELP_RAW
	Syntax: RAW text
	
	Sends a string of text directly to the server to which
	Services is connected.  This command has a very limited
	range of uses, and can wreak havoc on a network if used
	improperly.  DO NOT USE THIS COMMAND unless you are
	absolutely certain you know what you are doing!

OPER_HELP_UPDATE
	Syntax: UPDATE
	
	Causes Services to update all database files as soon as you
	send the command.
	
OPER_HELP_RELOAD
	Syntax: RELOAD

	Causes Services to reload the configuration file. Note that
	some directives still need the restart of the Services to
	take effect (such as Services' nicknames, activation of the 
	session limitation, etc.)

OPER_HELP_QUIT
	Syntax: QUIT
	
	Causes Services to do an immediate shutdown; databases are
	not saved.  This command should not be used unless
	damage to the in-memory copies of the databases is feared
	and they should not be saved.  For normal shutdowns, use the
	SHUTDOWN command.

OPER_HELP_SHUTDOWN
	Syntax: SHUTDOWN
	
	Causes Services to save all databases and then shut down.

OPER_HELP_RESTART
	Syntax: RESTART
	
	Causes Services to save all databases and then restart
	(i.e. exit and immediately re-run the executable).

OPER_HELP_CHANLIST
	Syntax: CHANLIST [{pattern | nick} [SECRET]]
	
	Lists all channels currently in use on the IRC network, whether they
	are registered or not.

	If pattern is given, lists only channels that match it. If a nickname
	is given, lists only the channels the user using it is on. If SECRET is
	specified, lists only channels matching pattern that have the +s or
	+p mode.

OPER_HELP_USERLIST
	Syntax: USERLIST [{pattern | channel} [INVISIBLE]]
	
	Lists all users currently online on the IRC network, whether their
	nick is registered or not.
	
	If pattern is given, lists only users that match it (it must be in
	the format nick!user@host). If channel is given, lists only users
	that are on the given channel. If INVISIBLE is specified, only users
	with the +i flag will be listed.

OPER_HELP_MODLOAD
	Syntax: MODLOAD FileName
	
	This command loads the module named FileName from the modules
	directory.

OPER_HELP_MODUNLOAD
	Syntax: MODUNLOAD FileName
	
	This command unloads the module named FileName from the modules
	directory.

OPER_HELP_MODINFO
	Syntax: MODINFO FileName
	
	This command lists information about the specified loaded module

OPER_HELP_MODLIST
	Syntax: MODLIST [Core|3rd|protocol|encryption|supported|qatested]
	
	Lists all currently loaded modules.


###########################################################################
#
# BotServ help messages
#
###########################################################################
BOT_HELP_CMD_BOTLIST
	    BOTLIST        Lists available bots
BOT_HELP_CMD_ASSIGN
	    ASSIGN         Assigns a bot to a channel
BOT_HELP_CMD_SET
	    SET            Configures bot options
BOT_HELP_CMD_KICK
	    KICK           Configures kickers
BOT_HELP_CMD_BADWORDS
	    BADWORDS       Maintains bad words list
BOT_HELP_CMD_ACT
	    ACT            Makes the bot do the equivalent of a "/me" command
BOT_HELP_CMD_INFO
	    INFO           Allows you to see BotServ information about a channel or a bot
BOT_HELP_CMD_SAY
	    SAY            Makes the bot say the given text on the given channel
BOT_HELP_CMD_UNASSIGN
	    UNASSIGN       Unassigns a bot from a channel
BOT_HELP_CMD_BOT
	    BOT            Maintains network bot list

BOT_HELP
	%S allows you to have a bot on your own channel.
	It has been created for users that can't host or
	configure a bot, or for use on networks that don't
	allow users' bot. Available commands are listed 
	below; to use them, type %R%S command.  For 
	more information on a specific command, type %R
	%S HELP command.
	

BOT_HELP_FOOTER
	Bot will join a channel whenever there is at least
	%d user(s) on it.

	
BOT_HELP_BOTLIST
	Syntax: BOTLIST
	
	Lists all available bots on this network.
	
BOT_HELP_ASSIGN
	Syntax: ASSIGN chan nick
	
	Assigns a bot pointed out by nick to the channel chan. You
	can then configure the bot for the channel so it fits
	your needs.  
	
BOT_HELP_UNASSIGN
	Syntax: UNASSIGN chan
	
	Unassigns a bot from a channel. When you use this command,
	the bot won't join the channel anymore. However, bot
	configuration for the channel is kept, so you will always
	be able to reassign a bot later without have to reconfigure
	it entirely.
	
BOT_HELP_INFO
	Syntax: INFO {chan | nick}
	
	Allows you to see %S information about a channel or a bot.
	If the parameter is a channel, then you'll get information
	such as enabled kickers. If the parameter is a nick,
	you'll get information about a bot, such as creation
	time or number of channels it is on.
	
BOT_HELP_SET
	Syntax: SET (channel | bot) option parameters
	
	Configures bot options.  option can be one of:
	
	    DONTKICKOPS      To protect ops against bot kicks
	    DONTKICKVOICES   To protect voices against bot kicks
	    GREET            Enable greet messages
	    FANTASY          Enable fantaisist commands
	    SYMBIOSIS        Allow the bot to act as a real bot
	
	Type %R%S HELP SET option for more information
	on a specific option.

	Note: access to this command is controlled by the
	level SET.
	
BOT_HELP_SET_DONTKICKOPS
	Syntax: SET channel DONTKICKOPS {ON|OFF}
	
	Enables or disables ops protection mode on a channel.
	When it is enabled, ops won't be kicked by the bot
	even if they don't match the NOKICK level.
	
BOT_HELP_SET_DONTKICKVOICES
	Syntax: SET channel DONTKICKVOICES {ON|OFF}
	
	Enables or disables voices protection mode on a channel.
	When it is enabled, voices won't be kicked by the bot
	even if they don't match the NOKICK level.
	
BOT_HELP_SET_FANTASY
	Syntax: SET channel FANTASY {ON|OFF}

	Enables or disables fantasy mode on a channel.
	When it is enabled, users will be able to use
	commands !op, !deop, !voice, !devoice,
	!kick, !kb, !unban, !seen on a channel (find how 
	to use them; try with or without nick for each, 
	and with a reason for some?).
	
	Note that users wanting to use fantaisist
	commands MUST have enough level for both
	the FANTASIA and another level depending
	of the command if required (for example, to use 
	!op, user must have enough access for the OPDEOP
	level).

BOT_HELP_SET_GREET
	Syntax: SET channel GREET {ON|OFF}
	
	Enables or disables greet mode on a channel.
	When it is enabled, the bot will display greet
	messages of users joining the channel, provided
	they have enough access to the channel.

BOT_HELP_SET_SYMBIOSIS
	Syntax: SET channel SYMBIOSIS {ON|OFF}
	
	Enables or disables symbiosis mode on a channel.
	When it is enabled, the bot will do everything
	normally done by %s on channels, such as MODEs,
	KICKs, and even the entry message.

BOT_HELP_KICK
	Syntax: KICK channel option parameters
	
	Configures bot kickers.  option can be one of:
	
	    BOLDS         Sets if the bot kicks bolds
	    BADWORDS      Sets if the bot kicks bad words
	    CAPS          Sets if the bot kicks caps
	    COLORS        Sets if the bot kicks colors
	    FLOOD         Sets if the bot kicks flooding users
	    REPEAT        Sets if the bot kicks users who repeat
	                       themselves
	    REVERSES      Sets if the bot kicks reverses
	    UNDERLINES    Sets if the bot kicks underlines
	
	Type %R%S HELP KICK option for more information
	on a specific option.
	
	Note: access to this command is controlled by the
	level SET.

BOT_HELP_KICK_BOLDS
	Syntax: KICK channel BOLDS {ON|OFF} [ttb]

	Sets the bolds kicker on or off. When enabled, this
	option tells the bot to kick users who use bolds.

	ttb is the number of times a user can be kicked
	before it get banned. Don't give ttb to disable
	the ban system once activated.

BOT_HELP_KICK_COLORS
	Syntax: KICK channel COLORS {ON|OFF} [ttb]

	Sets the colors kicker on or off. When enabled, this
	option tells the bot to kick users who use colors.

	ttb is the number of times a user can be kicked
	before it get banned. Don't give ttb to disable
	the ban system once activated.

BOT_HELP_KICK_REVERSES
	Syntax: KICK channel REVERSES {ON|OFF} [ttb]

	Sets the reverses kicker on or off. When enabled, this
	option tells the bot to kick users who use reverses.

	ttb is the number of times a user can be kicked
	before it get banned. Don't give ttb to disable
	the ban system once activated.

BOT_HELP_KICK_UNDERLINES
	Syntax: KICK channel UNDERLINES {ON|OFF} [ttb]

	Sets the underlines kicker on or off. When enabled, this
	option tells the bot to kick users who use underlines.

	ttb is the number of times a user can be kicked
	before it get banned. Don't give ttb to disable
	the ban system once activated.

BOT_HELP_KICK_CAPS
	Syntax: KICK channel CAPS {ON|OFF} [ttb [min [percent]]]

	Sets the caps kicker on or off. When enabled, this
	option tells the bot to kick users who are talking in
	CAPS.

	The bot kicks only if there are at least min caps
	and they constitute at least percent%% of the total 
	text line (if not given, it defaults to 10 characters
	and 25%%).

	ttb is the number of times a user can be kicked
	before it get banned. Don't give ttb to disable
	the ban system once activated.

BOT_HELP_KICK_FLOOD
	Syntax: KICK channel FLOOD {ON|OFF} [ttb [ln [secs]]]

	Sets the flood kicker on or off. When enabled, this
	option tells the bot to kick users who are flooding
	the channel using at least ln lines in secs seconds
	(if not given, it defaults to 6 lines in 10 seconds).
	
	ttb is the number of times a user can be kicked
	before it get banned. Don't give ttb to disable
	the ban system once activated.

BOT_HELP_KICK_REPEAT
	Syntax: KICK #channel REPEAT {ON|OFF} [ttb [num]]

	Sets the repeat kicker on or off. When enabled, this
	option tells the bot to kick users who are repeating
	themselves num times (if num is not given, it
	defaults to 3).

	ttb is the number of times a user can be kicked
	before it get banned. Don't give ttb to disable
	the ban system once activated.

BOT_HELP_KICK_BADWORDS
	Syntax: KICK #channel BADWORDS {ON|OFF} [ttb]

	Sets the bad words kicker on or off. When enabled, this
	option tells the bot to kick users who say certain words
	on the channels.

	You can define bad words for your channel using the
	BADWORDS command. Type %R%S HELP BADWORDS for
	more information.

	ttb is the number of times a user can be kicked
	before it get banned. Don't give ttb to disable
	the ban system once activated.
	
BOT_HELP_BADWORDS
	Syntax: BADWORDS channel ADD word [SINGLE | START | END]
	        BADWORDS channel DEL {word | entry-num | list}
	        BADWORDS channel LIST [mask | list]
	        BADWORDS channel CLEAR
	
	Maintains the bad words list for a channel. The bad
	words list determines which words are to be kicked
	when the bad words kicker is enabled. For more information,
	type %R%S HELP KICK BADWORDS.
	
	The BADWORDS ADD command adds the given word to the
	badword list. If SINGLE is specified, a kick will be
	done only if a user says the entire word. If START is 
	specified, a kick will be done if a user says a word
	that starts with word. If END is specified, a kick
	will be done if a user says a word that ends with
	word. If you don't specify anything, a kick will
	be issued every time word is said by a user.
	
	The BADWORDS DEL command removes the given word from the
	bad words list.  If a list of entry numbers is given, those
	entries are deleted.  (See the example for LIST below.)
	
	The BADWORDS LIST command displays the bad words list.  If
	a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   BADWORDS #channel LIST 2-5,7-9
	      Lists bad words entries numbered 2 through 5 and
	      7 through 9.
	      
	The BADWORDS CLEAR command clears all entries of the
	bad words list.

BOT_HELP_SAY
	Syntax: SAY channel text
	
	Makes the bot say the given text on the given channel.

BOT_HELP_ACT
	Syntax: ACT channel text
	
	Makes the bot do the equivalent of a "/me" command
	on the given channel using the given text.


BOT_SERVADMIN_HELP_BOT
	Syntax: BOT ADD nick user host real
	        BOT CHANGE oldnick newnick [user [host [real]]]
	        BOT DEL nick
	
	Allows Services admins to create, modify, and delete
	bots that users will be able to use on their own
	channels.
	
	BOT ADD adds a bot with the given nickname, username,
	hostname and realname. Since no integrity checks are done 
	for these settings, be really careful.
	BOT CHANGE allows to change nickname, username, hostname
	or realname of a bot without actually delete it (and all
	the data associated with it).
	BOT DEL removes the given bot from the bot list.  
	
	Note: you cannot create a bot that has a nick that is
	currently registered. If an unregistered user is currently
	using the nick, they will be killed.

BOT_SERVADMIN_HELP_SET
	
	These options are reserved to Services admins:
	
	    NOBOT            Prevent a bot from being assigned to 
	                        a channel
	    PRIVATE          Prevent a bot from being assigned by
	                        non IRC operators

BOT_SERVADMIN_HELP_SET_NOBOT
	Syntax: SET channel NOBOT {ON|OFF}
	
	This option makes a channel be unassignable. If a bot 
	is already assigned to the channel, it is unassigned
	automatically when you enable the option.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Syntax: SET bot-nick PRIVATE {ON|OFF}

	This option prevents a bot from being assigned to a
	channel by users that aren't IRC operators.

###########################################################################
#
# HostServ messages
#
###########################################################################
HOST_EMPTY
	The vhost list is empty.
HOST_ENTRY
	#%d Nick:%s, vhost:%s (%s - %s)
HOST_IDENT_ENTRY
	#%d Nick:%s, vhost:%s@%s (%s - %s)
HOST_SET
	vhost for %s set to %s.
HOST_IDENT_SET
	vhost for %s set to %s@%s.
HOST_SETALL
	vhost for group %s set to %s.
HOST_DELALL
	vhosts for group %s have been removed.
HOST_DELALL_SYNTAX
	DELALL <nick>.
HOST_IDENT_SETALL
	vhost for group %s set to %s@%s.
HOST_SET_ERROR
	A vhost must be in the format of a valid hostmask.
HOST_SET_IDENT_ERROR
	A vhost ident must be in the format of a valid ident
HOST_SET_TOOLONG
	Error! The vhost is too long, please use a host shorter than %d characters.
HOST_SET_IDENTTOOLONG
	Error! The Ident is too long, please use an ident shorter than %d characters.
HOST_NOREG
	User %s not found in the nickserv db.
HOST_SET_SYNTAX
	SET <nick> <hostmask>.
HOST_SETALL_SYNTAX
	SETALL <nick> <hostmask>.
HOST_DENIED
	Access Denied.
HOST_NOT_ASSIGNED
	Please contact an Operator to get a vhost assigned to this nick.
HOST_ACTIVATED
	Your vhost of %s is now activated.
HOST_IDENT_ACTIVATED
	Your vhost of %s@%s is now activated.
HOST_ID
	Please identify to services first.
HOST_NOT_REGED
	You need to register before a vhost can be assigned to you.
HOST_DEL
	vhost for %s removed.
HOST_DEL_SYNTAX
	DEL <nick>.
HOST_OFF
	Your vhost was removed and the normal cloaking restored. 
HOST_OFF_UNREAL
	Your vhost was removed. To re-enable the standard host cloaking, type /mode %s +%s
HOST_NO_VIDENT
	Your IRCD does not support vIdent's, if this is incorrect, please report this as a possible bug
HOST_GROUP
	All vhosts in the group %s have been set to %s
HOST_IDENT_GROUP
	All vhosts in the group %s have been set to %s@%s
HOST_LIST_FOOTER
	Displayed all records (Count: %d)
HOST_LIST_RANGE_FOOTER
	Displayed records from %d to %d
HOST_LIST_KEY_FOOTER
	Displayed records matching key %s (Count: %d)

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_HELP_CMD_ON
	    ON          Activates your assigned vhost

HOST_HELP_CMD_OFF
	    OFF         Deactivates your assigned vhost

HOST_HELP_CMD_GROUP
	    GROUP       Syncs the vhost for all nicks in a group

HOST_HELP_CMD_SET
	    SET         Set the vhost of another user

HOST_HELP_CMD_SETALL
	    SETALL      Set the vhost for all nicks in a group

HOST_HELP_CMD_DEL
	    DEL         Delete the vhost of another user

HOST_HELP_CMD_DELALL
	    DELALL      Delete the vhost for all nicks in a group

HOST_HELP_CMD_LIST
	    LIST        Displays one or more vhost entries.

HOST_OPER_HELP
	not used.

HOST_ADMIN_HELP
	not used.

HOST_HELP
	%S commands:

HOST_HELP_ON
	Syntax: ON

	Activates the vhost currently assigned to the nick in use.
	When you use this command any user who performs a /whois
	on you will see the vhost instead of your real IP address.

HOST_HELP_SET
	Syntax: SET <nick> <hostmask>.

	Sets the vhost for the given nick to that of the given
	hostmask.  If your IRCD supports vIdents, then using
	SET <nick> <ident>@<hostmask> set idents for users as 
	well as vhosts.

HOST_HELP_DELALL
	Syntax: DELALL <nick>.

	Deletes the vhost for all nick's in the same group as
	that of the given nick.


HOST_HELP_SETALL
	Syntax: SETALL <nick> <hostmask>.

	Sets the vhost for all nicks in the same group as that
	of the given nick.  If your IRCD supports vIdents, then
	using SETALL <nick> <ident>@<hostmask> will set idents
	for users as well as vhosts.

	* NOTE, this will not update the vhost for any nick's
	added to the group after this command was used.

HOST_HELP_OFF
	Syntax: OFF

	Deactivates the vhost currently assigned to the nick in use.
	When you use this command any user who performs a /whois
	on you will see your real IP address.

HOST_HELP_DEL
	Syntax: DEL <nick>

	Deletes the vhost assigned to the given nick from the
	database.

HOST_HELP_LIST
	Syntax: LIST [<key>|<#X-Y>]

	This command lists registered vhosts to the operator
	if a Key is specified, only entries whos nick or vhost match
	the pattern given in <key> are displayed e.g. Rob* for all
	entries beginning with "Rob"

	If a #X-Y style is used, only entries between the range of X
	and Y will be displayed, e.g. #1-3 will display the first 3
	nick/vhost entries.

	The list uses the value of NSListMax as a hard limit for the
	number of items to display to a operator at any 1 time.

HOST_HELP_GROUP
	Syntax: GROUP
	
	This command allows users to set the vhost of their
	CURRENT nick to be the vhost for all nicks in the same
	group.

OPER_SVSNICK_UNSUPPORTED
	Sorry, SVSNICK is not available on this network.

OPER_SQLINE_UNSUPPORTED
	Sorry, SQLINE is not available on this network.

OPER_SVSO_UNSUPPORTED
	Sorry, OLINE is not available on this network.

OPER_UMODE_UNSUPPORTED
	Sorry, UMODE is not available on this network.

OPER_SUPER_ADMIN_NOT_ENABLED
	SuperAdmin setting not enabled in services.conf

