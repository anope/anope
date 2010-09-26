#include "services.h"

#if HAVE_GETTEXT
# include <libintl.h>
#else
# define gettext(x) x
#endif

std::vector<Anope::string> languages;

void InitLanguages()
{
#if HAVE_GETTEXT
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
	{
		Log() << "Error calling bindtextdomain, " << Anope::LastError();
	}
#else
	Log() << "Can not load languages, gettext is not installed";
#endif
}

const Anope::string GetString(Anope::string language, LanguageString string)
{
#if HAVE_GETTEXT
	/* For older databases */
	if (language == "en")
		language.clear();

	/* Apply def language */
	if (language.empty() && !Config->NSDefLanguage.empty())
		language = Config->NSDefLanguage;

	if (language.empty())
#endif
		return language_strings[string];

#if HAVE_GETTEXT
	setlocale(LC_ALL, language.c_str());
	const char *ret = dgettext("anope", language_strings[string].c_str());
	setlocale(LC_ALL, "");

	return ret ? ret : "";
#endif
}

const Anope::string GetString(LanguageString string)
{
	return GetString("", string);
}

const Anope::string GetString(const NickCore *nc, LanguageString string)
{
	return GetString(nc ? nc->language : "", string);
}

const Anope::string GetString(const User *u, LanguageString string)
{
	if (!u)
		return GetString(string);
	const NickCore *nc = u->Account();
	NickAlias *na = NULL;
	if (!nc)
		na = findnick(u->nick);
	return GetString(nc ? nc : (na ? na->nc : NULL), string);
}

void SyntaxError(BotInfo *bi, User *u, const Anope::string &command, LanguageString message)
{
	if (!bi || !u || command.empty())
		return;

	Anope::string str = GetString(u, message);
	u->SendMessage(bi, SYNTAX_ERROR, str.c_str());
	u->SendMessage(bi, MORE_INFO, bi->nick.c_str(), command.c_str());
}

Anope::string language_strings[LANG_STRING_COUNT] = {
	/* LANG_NAME */
	gettext("English"),
	/* COMMAND_REQUIRES_PERM */
	gettext("Access to this command requires the permission %s to be present in your opertype."),
	/* COMMAND_IDENTIFY_REQUIRED */
	gettext("You need to be identified to use this command."),
	/* COMMAND_CANNOT_USE */
	gettext("You cannot use this command."),
	/* COMMAND_CAN_USE */
	gettext("You can use this command."),
	/* USER_RECORD_NOT_FOUND */
	gettext("Internal error - unable to process request."),
	/* UNKNOWN_COMMAND */
	gettext("Unknown command %s."),
	/* UNKNOWN_COMMAND_HELP */
	gettext("Unknown command %s.  \"%R%s HELP\" for help."),
	/* SYNTAX_ERROR */
	gettext("Syntax: %s"),
	/* MORE_INFO */
	gettext("%R%s HELP %s for more information."),
	/* NO_HELP_AVAILABLE */
	gettext("No help available for %s."),
	/* OBSOLETE_COMMAND */
	gettext("This command is obsolete; use %s instead."),
	/* BAD_USERHOST_MASK */
	gettext("Mask must be in the form user@host."),
	/* BAD_EXPIRY_TIME */
	gettext("Invalid expiry time."),
	/* USERHOST_MASK_TOO_WIDE */
	gettext("%s coverage is too wide; Please use a more specific mask."),
	/* SERVICE_OFFLINE */
	gettext("%s is currently offline."),
	/* READ_ONLY_MODE */
	gettext("Notice: Services is in read-only mode; changes will not be saved!"),
	/* PASSWORD_INCORRECT */
	gettext("Password incorrect."),
	/* INVALID_TARGET */
	gettext("\"/msg %s\" is no longer supported.  Use \"/msg %s@%s\" or \"/%s\" instead."),
	/* ACCESS_DENIED */
	gettext("Access denied."),
	/* MORE_OBSCURE_PASSWORD */
	gettext("Please try again with a more obscure password.  Passwords should be at least five characters long, should not be something easily guessed (e.g. your real name or your nick), and cannot contain the space or tab characters."),
	/* PASSWORD_TOO_LONG */
	gettext("Your password is too long. Please try again with a shorter password."),
	/* NICK_NOT_REGISTERED */
	gettext("Your nick isn't registered."),
	/* NICK_NOT_REGISTERED_HELP */
	gettext("Your nick isn't registered.  Type %R%s HELP for information on registering your nickname."),
	/* NICK_X_IS_SERVICES */
	gettext("Nick %s is part of this Network's Services."),
	/* NICK_X_NOT_REGISTERED */
	gettext("Nick %s isn't registered."),
	/* NICK_X_IN_USE */
	gettext("Nick %s is currently in use."),
	/* NICK_X_NOT_IN_USE */
	gettext("Nick %s isn't currently in use."),
	/* NICK_X_NOT_ON_CHAN */
	gettext("%s is not currently on channel %s."),
	/* NICK_X_FORBIDDEN */
	gettext("Nick %s may not be registered or used."),
	/* NICK_X_FORBIDDEN_OPER */
	gettext("Nick %s has been forbidden by %s:\n"
	"%s"),
	/* NICK_X_ILLEGAL */
	gettext("Nick %s is an illegal nickname and cannot be used."),
	/* NICK_X_TRUNCATED */
	gettext("Nick %s was truncated to %d characters."),
	/* NICK_X_SUSPENDED */
	gettext("Nick %s is currently suspended."),
	/* CHAN_X_NOT_REGISTERED */
	gettext("Channel %s isn't registered."),
	/* CHAN_X_NOT_IN_USE */
	gettext("Channel %s doesn't exist."),
	/* CHAN_X_FORBIDDEN */
	gettext("Channel %s may not be registered or used."),
	/* CHAN_X_FORBIDDEN_OPER */
	gettext("Channel %s has been forbidden by %s:\n"
	"%s"),
	/* CHAN_X_SUSPENDED */
	gettext("      Suspended: [%s] %s"),
	/* NICK_IDENTIFY_REQUIRED */
	gettext("Password authentication required for that command.\n"
	"Retry after typing %R%s IDENTIFY password."),
	/* CHAN_IDENTIFY_REQUIRED */
	gettext("Password authentication required for that command.\n"
	"Retry after typing %R%s IDENTIFY %s password."),
	/* MAIL_DISABLED */
	gettext("Services have been configured to not send mail."),
	/* MAIL_INVALID */
	gettext("E-mail for %s is invalid."),
	/* MAIL_X_INVALID */
	gettext("%s is not a valid e-mail address."),
	/* MAIL_LATER */
	gettext("Cannot send mail now; please retry a little later."),
	/* MAIL_DELAYED */
	gettext("Please wait %d seconds and retry."),
	/* NO_REASON */
	gettext("No reason"),
	/* UNKNOWN */
	gettext("<unknown>"),
	/* DURATION_DAY */
	gettext("1 day"),
	/* DURATION_DAYS */
	gettext("%d days"),
	/* DURATION_HOUR */
	gettext("1 hour"),
	/* DURATION_HOURS */
	gettext("%d hours"),
	/* DURATION_MINUTE */
	gettext("1 minute"),
	/* DURATION_MINUTES */
	gettext("%d minutes"),
	/* DURATION_SECOND */
	gettext("1 second"),
	/* DURATION_SECONDS */
	gettext("%d seconds"),
	/* NO_EXPIRE */
	gettext("does not expire"),
	/* EXPIRES_SOON */
	gettext("expires at next database update"),
	/* EXPIRES_M */
	gettext("expires in %d minutes"),
	/* EXPIRES_1M */
	gettext("expires in %d minute"),
	/* EXPIRES_HM */
	gettext("expires in %d hours, %d minutes"),
	/* EXPIRES_H1M */
	gettext("expires in %d hours, %d minute"),
	/* EXPIRES_1HM */
	gettext("expires in %d hour, %d minutes"),
	/* EXPIRES_1H1M */
	gettext("expires in %d hour, %d minute"),
	/* EXPIRES_D */
	gettext("expires in %d days"),
	/* EXPIRES_1D */
	gettext("expires in %d day"),
	/* END_OF_ANY_LIST */
	gettext("End of %s list."),
	/* LIST_INCORRECT_RANGE */
	gettext("Incorrect range specified. The correct syntax is #from-to."),
	/* CS_LIST_INCORRECT_RANGE */
	gettext("To search for channels starting with #, search for the channel\n"
	"name without the #-sign prepended (anope instead of #anope)."),
	/* NICK_IS_REGISTERED */
	gettext("This nick is owned by someone else.  Please choose another.\n"
	"(If this is your nick, type %R%s IDENTIFY password.)"),
	/* NICK_IS_SECURE */
	gettext("This nickname is registered and protected.  If it is your\n"
	"nick, type %R%s IDENTIFY password.  Otherwise,\n"
	"please choose a different nick."),
	/* NICK_MAY_NOT_BE_USED */
	gettext("This nickname may not be used.  Please choose another one."),
	/* FORCENICKCHANGE_IN_1_MINUTE */
	gettext("If you do not change within one minute, I will change your nick."),
	/* FORCENICKCHANGE_IN_20_SECONDS */
	gettext("If you do not change within 20 seconds, I will change your nick."),
	/* FORCENICKCHANGE_NOW */
	gettext("This nickname has been registered; you may not use it."),
	/* FORCENICKCHANGE_CHANGING */
	gettext("Your nickname is now being changed to %s"),
	/* NICK_REGISTER_SYNTAX */
	gettext("REGISTER password [email]"),
	/* NICK_REGISTER_SYNTAX_EMAIL */
	gettext("REGISTER password email"),
	/* NICK_REGISTRATION_DISABLED */
	gettext("Sorry, nickname registration is temporarily disabled."),
	/* NICK_REGISTRATION_FAILED */
	gettext("Sorry, registration failed."),
	/* NICK_REG_PLEASE_WAIT */
	gettext("Please wait %d seconds before using the REGISTER command again."),
	/* NICK_CANNOT_BE_REGISTERED */
	gettext("Nickname %s may not be registered."),
	/* NICK_ALREADY_REGISTERED */
	gettext("Nickname %s is already registered!"),
	/* NICK_REGISTERED */
	gettext("Nickname %s registered under your account: %s"),
	/* NICK_REGISTERED_NO_MASK */
	gettext("Nickname %s registered."),
	/* NICK_PASSWORD_IS */
	gettext("Your password is %s - remember this for later use."),
	/* NICK_REG_DELAY */
	gettext("You must have been using this nick for at least %d seconds to register."),
	/* NICK_GROUP_SYNTAX */
	gettext("GROUP target password"),
	/* NICK_GROUP_DISABLED */
	gettext("Sorry, nickname grouping is temporarily disabled."),
	/* NICK_GROUP_FAILED */
	gettext("Sorry, grouping failed."),
	/* NICK_GROUP_PLEASE_WAIT */
	gettext("Please wait %d seconds before using the GROUP command again."),
	/* NICK_GROUP_CHANGE_DISABLED */
	gettext("Your nick is already registered; type %R%s DROP first."),
	/* NICK_GROUP_SAME */
	gettext("You are already a member of the group of %s."),
	/* NICK_GROUP_TOO_MANY */
	gettext("There are too many nicks in %s's group; list them and drop some.\n"
	"Type %R%s HELP GLIST and %R%s HELP DROP\n"
	"for more information."),
	/* NICK_GROUP_JOINED */
	gettext("You are now in the group of %s."),
	/* NICK_UNGROUP_ONE_NICK */
	gettext("Your nick is not grouped to anything, you can't ungroup it."),
	/* NICK_UNGROUP_NOT_IN_GROUP */
	gettext("The nick %s is not in your group."),
	/* NICK_UNGROUP_SUCCESSFUL */
	gettext("Nick %s has been ungrouped from %s."),
	/* NICK_IDENTIFY_SYNTAX */
	gettext("IDENTIFY [account] password"),
	/* NICK_IDENTIFY_FAILED */
	gettext("Sorry, identification failed."),
	/* NICK_IDENTIFY_SUCCEEDED */
	gettext("Password accepted - you are now recognized."),
	/* NICK_IDENTIFY_EMAIL_REQUIRED */
	gettext("You must now supply an e-mail for your nick.\n"
	"This e-mail will allow you to retrieve your password in\n"
	"case you forget it."),
	/* NICK_IDENTIFY_EMAIL_HOWTO */
	gettext("Type %R%S SET EMAIL e-mail in order to set your e-mail.\n"
	"Your privacy is respected; this e-mail won't be given to\n"
	"any third-party person."),
	/* NICK_ALREADY_IDENTIFIED */
	gettext("You are already identified."),
	/* NICK_UPDATE_SUCCESS */
	gettext("Status updated (memos, vhost, chmodes, flags)."),
	/* NICK_LOGOUT_SYNTAX */
	gettext("LOGOUT"),
	/* NICK_LOGOUT_SUCCEEDED */
	gettext("Your nick has been logged out."),
	/* NICK_LOGOUT_X_SUCCEEDED */
	gettext("Nick %s has been logged out."),
	/* NICK_LOGOUT_SERVICESADMIN */
	gettext("Can't logout %s because he's a Services Operator."),
	/* NICK_DROP_DISABLED */
	gettext("Sorry, nickname de-registration is temporarily disabled."),
	/* NICK_DROPPED */
	gettext("Your nickname has been dropped."),
	/* NICK_X_DROPPED */
	gettext("Nickname %s has been dropped."),
	/* NICK_SET_SYNTAX */
	gettext("SET option parameters"),
	/* NICK_SET_SERVADMIN_SYNTAX */
	gettext("SET [nick] option parameters"),
	/* NICK_SET_DISABLED */
	gettext("Sorry, nickname option setting is temporarily disabled."),
	/* NICK_SET_UNKNOWN_OPTION */
	gettext("Unknown SET option %s."),
	/* NICK_SET_OPTION_DISABLED */
	gettext("Option %s cannot be set on this network."),
	/* NICK_SET_DISPLAY_INVALID */
	gettext("The new display MUST be a nickname of your nickname group!"),
	/* NICK_SET_DISPLAY_CHANGED */
	gettext("The new display is now %s."),
	/* NICK_SET_PASSWORD_FAILED */
	gettext("Sorry, couldn't change password."),
	/* NICK_SET_PASSWORD_CHANGED */
	gettext("Password changed."),
	/* NICK_SET_PASSWORD_CHANGED_TO */
	gettext("Password changed to %s."),
	/* NICK_SET_LANGUAGE_SYNTAX */
	gettext("SET LANGUAGE language"),
	/* NICK_SET_LANGUAGE_UNKNOWN */
	gettext("Unknown language number %d.  Type %R%s HELP SET LANGUAGE for a list of languages."),
	/* NICK_SET_LANGUAGE_CHANGED */
	gettext("Language changed to English."),
	/* NICK_SET_EMAIL_CHANGED */
	gettext("E-mail address changed to %s."),
	/* NICK_SET_EMAIL_UNSET */
	gettext("E-mail address unset."),
	/* NICK_SET_EMAIL_UNSET_IMPOSSIBLE */
	gettext("You cannot unset the e-mail on this network."),
	/* NICK_SET_GREET_CHANGED */
	gettext("Greet message changed to %s."),
	/* NICK_SET_GREET_UNSET */
	gettext("Greet message unset."),
	/* NICK_SET_KILL_SYNTAX */
	gettext("SET KILL {ON | QUICK | OFF}"),
	/* NICK_SET_KILL_IMMED_SYNTAX */
	gettext("SET KILL {ON | QUICK | IMMED | OFF}"),
	/* NICK_SET_KILL_ON */
	gettext("Protection is now ON."),
	/* NICK_SET_KILL_QUICK */
	gettext("Protection is now ON, with a reduced delay."),
	/* NICK_SET_KILL_IMMED */
	gettext("Protection is now ON, with no delay."),
	/* NICK_SET_KILL_IMMED_DISABLED */
	gettext("The IMMED option is not available on this network."),
	/* NICK_SET_KILL_OFF */
	gettext("Protection is now OFF."),
	/* NICK_SET_SECURE_SYNTAX */
	gettext("SET SECURE {ON | OFF}"),
	/* NICK_SET_SECURE_ON */
	gettext("Secure option is now ON."),
	/* NICK_SET_SECURE_OFF */
	gettext("Secure option is now OFF."),
	/* NICK_SET_PRIVATE_SYNTAX */
	gettext("SET PRIVATE {ON | OFF}"),
	/* NICK_SET_PRIVATE_ON */
	gettext("Private option is now ON."),
	/* NICK_SET_PRIVATE_OFF */
	gettext("Private option is now OFF."),
	/* NICK_SET_HIDE_SYNTAX */
	gettext("SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}"),
	/* NICK_SET_HIDE_EMAIL_ON */
	gettext("Your E-mail address will now be hidden from %s INFO displays."),
	/* NICK_SET_HIDE_EMAIL_OFF */
	gettext("Your E-mail address will now be shown in %s INFO displays."),
	/* NICK_SET_HIDE_MASK_ON */
	gettext("Your last seen user@host mask will now be hidden from %s INFO displays."),
	/* NICK_SET_HIDE_MASK_OFF */
	gettext("Your last seen user@host mask will now be shown in %s INFO displays."),
	/* NICK_SET_HIDE_QUIT_ON */
	gettext("Your last quit message will now be hidden from %s INFO displays."),
	/* NICK_SET_HIDE_QUIT_OFF */
	gettext("Your last quit message will now be shown in %s INFO displays."),
	/* NICK_SET_HIDE_STATUS_ON */
	gettext("Your services access status will now be hidden from %s INFO displays."),
	/* NICK_SET_HIDE_STATUS_OFF */
	gettext("Your services access status will now be shown in %s INFO displays."),
	/* NICK_SET_MSG_SYNTAX */
	gettext("SET MSG {ON | OFF}"),
	/* NICK_SET_MSG_ON */
	gettext("Services will now reply to you with messages."),
	/* NICK_SET_MSG_OFF */
	gettext("Services will now reply to you with notices."),
	/* NICK_SET_AUTOOP_SYNTAX */
	gettext("SET AUTOOP {ON | OFF}"),
	/* NICK_SET_AUTOOP_ON */
	gettext("Services will now autoop you in channels."),
	/* NICK_SET_AUTOOP_OFF */
	gettext("Services will no longer autoop you in channels."),
	/* NICK_SASET_SYNTAX */
	gettext("SASET nickname option parameters"),
	/* NICK_SASET_UNKNOWN_OPTION */
	gettext("Unknown SASET option %s."),
	/* NICK_SASET_BAD_NICK */
	gettext("Nickname %s not registered."),
	/* NICK_SASET_DISPLAY_INVALID */
	gettext("The new display for %s MUST be a nickname of the nickname group!"),
	/* NICK_SASET_PASSWORD_FAILED */
	gettext("Sorry, couldn't change password for %s."),
	/* NICK_SASET_PASSWORD_CHANGED */
	gettext("Password for %s changed."),
	/* NICK_SASET_PASSWORD_CHANGED_TO */
	gettext("Password for %s changed to %s."),
	/* NICK_SASET_EMAIL_CHANGED */
	gettext("E-mail address for %s changed to %s."),
	/* NICK_SASET_EMAIL_UNSET */
	gettext("E-mail address for %s unset."),
	/* NICK_SASET_GREET_CHANGED */
	gettext("Greet message for %s changed to %s."),
	/* NICK_SASET_GREET_UNSET */
	gettext("Greet message for %s unset."),
	/* NICK_SASET_KILL_SYNTAX */
	gettext("SASET nickname KILL {ON | QUICK | OFF}"),
	/* NICK_SASET_KILL_IMMED_SYNTAX */
	gettext("SASET nickname KILL {ON | QUICK | IMMED | OFF}"),
	/* NICK_SASET_KILL_ON */
	gettext("Protection is now ON for %s."),
	/* NICK_SASET_KILL_QUICK */
	gettext("Protection is now ON for %s, with a reduced delay."),
	/* NICK_SASET_KILL_IMMED */
	gettext("Protection is now ON for %s, with no delay."),
	/* NICK_SASET_KILL_OFF */
	gettext("Protection is now OFF for %s."),
	/* NICK_SASET_SECURE_SYNTAX */
	gettext("SASET nickname SECURE {ON | OFF}"),
	/* NICK_SASET_SECURE_ON */
	gettext("Secure option is now ON for %s."),
	/* NICK_SASET_SECURE_OFF */
	gettext("Secure option is now OFF for %s."),
	/* NICK_SASET_PRIVATE_SYNTAX */
	gettext("SASET nickname PRIVATE {ON | OFF}"),
	/* NICK_SASET_PRIVATE_ON */
	gettext("Private option is now ON for %s."),
	/* NICK_SASET_PRIVATE_OFF */
	gettext("Private option is now OFF for %s."),
	/* NICK_SASET_HIDE_SYNTAX */
	gettext("SASET nickname HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}"),
	/* NICK_SASET_HIDE_EMAIL_ON */
	gettext("The E-mail address of %s will now be hidden from %s INFO displays."),
	/* NICK_SASET_HIDE_EMAIL_OFF */
	gettext("The E-mail address of %s will now be shown in %s INFO displays."),
	/* NICK_SASET_HIDE_MASK_ON */
	gettext("The last seen user@host mask of %s will now be hidden from %s INFO displays."),
	/* NICK_SASET_HIDE_MASK_OFF */
	gettext("The last seen user@host mask of %s will now be shown in %s INFO displays."),
	/* NICK_SASET_HIDE_QUIT_ON */
	gettext("The last quit message of %s will now be hidden from %s INFO displays."),
	/* NICK_SASET_HIDE_QUIT_OFF */
	gettext("The last quit message of %s will now be shown in %s INFO displays."),
	/* NICK_SASET_HIDE_STATUS_ON */
	gettext("The services access status of %s will now be hidden from %s INFO displays."),
	/* NICK_SASET_HIDE_STATUS_OFF */
	gettext("The services access status of %s will now be shown in %s INFO displays."),
	/* NICK_SASET_MSG_SYNTAX */
	gettext("SASAET nickname PRIVATE {ON | OFF}"),
	/* NICK_SASET_MSG_ON */
	gettext("Services will now reply to %s with messages."),
	/* NICK_SASET_MSG_OFF */
	gettext("Services will now reply to %s with notices."),
	/* NICK_SASET_NOEXPIRE_SYNTAX */
	gettext("SASET nickname NOEXPIRE {ON | OFF}"),
	/* NICK_SASET_NOEXPIRE_ON */
	gettext("Nick %s will not expire."),
	/* NICK_SASET_NOEXPIRE_OFF */
	gettext("Nick %s will expire."),
	/* NICK_SASET_AUTOOP_SYNTAX */
	gettext("SASET nickname AUTOOP {ON | OFF}"),
	/* NICK_SASET_AUTOOP_ON */
	gettext("Services will now autoop %s in channels."),
	/* NICK_SASET_AUTOOP_OFF */
	gettext("Services will no longer autoop %s in channels."),
	/* NICK_SASET_LANGUAGE_SYNTAX */
	gettext("SASET nickname LANGUAGE number"),
	/* NICK_ACCESS_SYNTAX */
	gettext("ACCESS {ADD | DEL | LIST} [mask]"),
	/* NICK_ACCESS_ALREADY_PRESENT */
	gettext("Mask %s already present on your access list."),
	/* NICK_ACCESS_REACHED_LIMIT */
	gettext("Sorry, you can only have %d access entries for a nickname."),
	/* NICK_ACCESS_ADDED */
	gettext("%s added to your access list."),
	/* NICK_ACCESS_NOT_FOUND */
	gettext("%s not found on your access list."),
	/* NICK_ACCESS_DELETED */
	gettext("%s deleted from your access list."),
	/* NICK_ACCESS_LIST */
	gettext("Access list:"),
	/* NICK_ACCESS_LIST_X */
	gettext("Access list for %s:"),
	/* NICK_ACCESS_LIST_EMPTY */
	gettext("Your access list is empty."),
	/* NICK_ACCESS_LIST_X_EMPTY */
	gettext("Access list for %s is empty."),
	/* NICK_STATUS_REPLY */
	gettext("STATUS %s %d %s"),
	/* NICK_INFO_SYNTAX */
	gettext("INFO nick"),
	/* NICK_INFO_REALNAME */
	gettext("%s is %s"),
	/* NICK_INFO_SERVICES_OPERTYPE */
	gettext("%s is a services operator of type %s."),
	/* NICK_INFO_ADDRESS */
	gettext("Last seen address: %s"),
	/* NICK_INFO_ADDRESS_ONLINE */
	gettext("   Is online from: %s"),
	/* NICK_INFO_ADDRESS_ONLINE_NOHOST */
	gettext("%s is currently online."),
	/* NICK_INFO_TIME_REGGED */
	gettext("  Time registered: %s"),
	/* NICK_INFO_LAST_SEEN */
	gettext("   Last seen time: %s"),
	/* NICK_INFO_LAST_QUIT */
	gettext("Last quit message: %s"),
	/* NICK_INFO_URL */
	gettext("              URL: %s"),
	/* NICK_INFO_EMAIL */
	gettext("   E-mail address: %s"),
	/* NICK_INFO_VHOST */
	gettext("            vhost: %s"),
	/* NICK_INFO_VHOST2 */
	gettext("            vhost: %s@%s"),
	/* NICK_INFO_GREET */
	gettext("    Greet message: %s"),
	/* NICK_INFO_OPTIONS */
	gettext("          Options: %s"),
	/* NICK_INFO_EXPIRE */
	gettext("Expires on: %s"),
	/* NICK_INFO_OPT_KILL */
	gettext("Protection"),
	/* NICK_INFO_OPT_SECURE */
	gettext("Security"),
	/* NICK_INFO_OPT_PRIVATE */
	gettext("Private"),
	/* NICK_INFO_OPT_MSG */
	gettext("Message mode"),
	/* NICK_INFO_OPT_AUTOOP */
	gettext("Auto-op"),
	/* NICK_INFO_OPT_NONE */
	gettext("None"),
	/* NICK_INFO_NO_EXPIRE */
	gettext("This nickname will not expire."),
	/* NICK_INFO_SUSPENDED */
	gettext("This nickname is currently suspended, reason: %s"),
	/* NICK_INFO_SUSPENDED_NO_REASON */
	gettext("This nickname is currently suspended"),
	/* NICK_LIST_SYNTAX */
	gettext("LIST pattern"),
	/* NICK_LIST_SERVADMIN_SYNTAX */
	gettext("LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]"),
	/* NICK_LIST_HEADER */
	gettext("List of entries matching %s:"),
	/* NICK_LIST_RESULTS */
	gettext("End of list - %d/%d matches shown."),
	/* NICK_ALIST_SYNTAX */
	gettext("ALIST nickname"),
	/* NICK_ALIST_HEADER */
	gettext("Channels that you have access on:\n"
	"  Num  Channel              Level    Description"),
	/* NICK_ALIST_HEADER_X */
	gettext("Channels that %s has access on:\n"
	"  Num  Channel              Level    Description"),
	/* NICK_ALIST_XOP_FORMAT */
	gettext("  %3d %c%-20s %-8s %s"),
	/* NICK_ALIST_ACCESS_FORMAT */
	gettext("  %3d %c%-20s %-8d %s"),
	/* NICK_ALIST_FOOTER */
	gettext("End of list - %d/%d channels shown."),
	/* NICK_GLIST_HEADER */
	gettext("List of nicknames in your group:"),
	/* NICK_GLIST_HEADER_X */
	gettext("List of nicknames in the group of %s:"),
	/* NICK_GLIST_FOOTER */
	gettext("%d nicknames in the group."),
	/* NICK_GLIST_REPLY */
	gettext("   %s (expires in %s)"),
	/* NICK_GLIST_REPLY_NOEXPIRE */
	gettext("   %s (does not expire)"),
	/* NICK_RECOVER_SYNTAX */
	gettext("RECOVER nickname [password]"),
	/* NICK_NO_RECOVER_SELF */
	gettext("You can't recover yourself!"),
	/* NICK_RECOVERED */
	gettext("User claiming your nick has been killed.\n"
	"%R%s RELEASE %s to get it back before %s timeout."),
	/* NICK_RELEASE_SYNTAX */
	gettext("RELEASE nickname [password]"),
	/* NICK_RELEASE_NOT_HELD */
	gettext("Nick %s isn't being held."),
	/* NICK_RELEASED */
	gettext("Services' hold on your nick has been released."),
	/* NICK_GHOST_SYNTAX */
	gettext("GHOST nickname [password]"),
	/* NICK_NO_GHOST_SELF */
	gettext("You can't ghost yourself!"),
	/* NICK_GHOST_KILLED */
	gettext("Ghost with your nick has been killed."),
	/* NICK_GETPASS_SYNTAX */
	gettext("GETPASS nickname"),
	/* NICK_GETPASS_UNAVAILABLE */
	gettext("GETPASS command unavailable because encryption is in use."),
	/* NICK_GETPASS_PASSWORD_IS */
	gettext("Password for %s is %s."),
	/* NICK_GETEMAIL_SYNTAX */
	gettext("GETEMAIL user@email-host No WildCards!!"),
	/* NICK_GETEMAIL_EMAILS_ARE */
	gettext("Emails Match %s to %s."),
	/* NICK_GETEMAIL_NOT_USED */
	gettext("No Emails listed for %s."),
	/* NICK_SENDPASS_SYNTAX */
	gettext("SENDPASS nickname"),
	/* NICK_SENDPASS_UNAVAILABLE */
	gettext("SENDPASS command unavailable because encryption is in use."),
	/* NICK_SENDPASS_SUBJECT */
	gettext("Nickname password (%s)"),
	/* NICK_SENDPASS */
	gettext("Hi,\n"
	" \n"
	"You have requested to receive the password of nickname %s by e-mail.\n"
	"The password is %s. For security purposes, you should change it as soon as you receive this mail.\n"
	" \n"
	"If you don't know why this mail was sent to you, please ignore it silently.\n"
	" \n"
	"%s administrators."),
	/* NICK_SENDPASS_OK */
	gettext("Password of %s has been sent."),
	/* NICK_RESETPASS_SYNTAX */
	gettext("RESETPASS nickname"),
	/* NICK_RESETPASS_SUBJECT */
	gettext("Reset password request for %s"),
	/* NICK_RESETPASS_MESSAGE */
	gettext("Hi,\n"
	" \n"
	"You have requested to have the password for %s reset.\n"
	"To reset your password, type %R%s CONFIRM %s\n"
	" \n"
	"If you don't know why this mail was sent to you, please ignore it silently.\n"
	" \n"
	"%s administrators."),
	/* NICK_RESETPASS_COMPLETE */
	gettext("Password reset email for %s has been sent."),
	/* NICK_SUSPEND_SYNTAX */
	gettext("SUSPEND nickname reason"),
	/* NICK_SUSPEND_SUCCEEDED */
	gettext("Nick %s is now suspended."),
	/* NICK_SUSPEND_FAILED */
	gettext("Couldn't suspend nick %s!"),
	/* NICK_UNSUSPEND_SYNTAX */
	gettext("UNSUSPEND nickname"),
	/* NICK_UNSUSPEND_SUCCEEDED */
	gettext("Nick %s is now released."),
	/* NICK_UNSUSPEND_FAILED */
	gettext("Couldn't release nick %s!"),
	/* NICK_FORBID_SYNTAX */
	gettext("FORBID nickname [reason]"),
	/* NICK_FORBID_SYNTAX_REASON */
	gettext("FORBID nickname reason"),
	/* NICK_FORBID_SUCCEEDED */
	gettext("Nick %s is now forbidden."),
	/* NICK_FORBID_FAILED */
	gettext("Couldn't forbid nick %s!"),
	/* NICK_REQUESTED */
	gettext("This nick has already been requested, please check your e-mail address for the pass code"),
	/* NICK_REG_RESENT */
	gettext("Your passcode has been re-sent to %s."),
	/* NICK_REG_UNABLE */
	gettext("Nick NOT registered, please try again later."),
	/* NICK_IS_PREREG */
	gettext("This nick is awaiting an e-mail verification code before completing registration."),
	/* NICK_ENTER_REG_CODE */
	gettext("A passcode has been sent to %s, please type %R%s confirm <passcode> to complete registration"),
	/* NICK_GETPASS_PASSCODE_IS */
	gettext("Passcode for %s is %s."),
	/* NICK_FORCE_REG */
	gettext("Nickname %s confirmed"),
	/* NICK_REG_MAIL_SUBJECT */
	gettext("Nickname Registration (%s)"),
	/* NICK_REG_MAIL */
	gettext("Hi,\n"
	" \n"
	"You have requested to register the nickname %s on %s.\n"
	"Please type \" %R%s confirm %s \" to complete registration.\n"
	" \n"
	"If you don't know why this mail was sent to you, please ignore it silently.\n"
	" \n"
	"%s administrators."),
	/* NICK_CONFIRM_NOT_FOUND */
	gettext("Registration step 1 may have expired, please use \"%R%s register <password> <email>\" first."),
	/* NICK_CONFIRM_INVALID */
	gettext("Invalid passcode has been entered, please check the e-mail again, and retry"),
	/* NICK_CONFIRM_EXPIRED */
	gettext("Your password reset request has expired."),
	/* NICK_CONFIRM_SUCCESS */
	gettext("You are now identified for your nick. Change your password using \"%R%s SET PASSWORD newpassword\" now."),
	/* CHAN_LEVEL_AUTOOP */
	gettext("Automatic channel operator status"),
	/* CHAN_LEVEL_AUTOVOICE */
	gettext("Automatic mode +v"),
	/* CHAN_LEVEL_AUTOHALFOP */
	gettext("Automatic mode +h"),
	/* CHAN_LEVEL_AUTOPROTECT */
	gettext("Automatic mode +a"),
	/* CHAN_LEVEL_AUTODEOP */
	gettext("Channel operator status disallowed"),
	/* CHAN_LEVEL_NOJOIN */
	gettext("Not allowed to join channel"),
	/* CHAN_LEVEL_INVITE */
	gettext("Allowed to use INVITE command"),
	/* CHAN_LEVEL_AKICK */
	gettext("Allowed to use AKICK command"),
	/* CHAN_LEVEL_SET */
	gettext("Allowed to use SET command (not FOUNDER/PASSWORD)"),
	/* CHAN_LEVEL_CLEAR */
	gettext("Allowed to use CLEAR command"),
	/* CHAN_LEVEL_UNBAN */
	gettext("Allowed to use UNBAN command"),
	/* CHAN_LEVEL_OPDEOP */
	gettext("Allowed to use OP/DEOP commands"),
	/* CHAN_LEVEL_ACCESS_LIST */
	gettext("Allowed to view the access list"),
	/* CHAN_LEVEL_ACCESS_CHANGE */
	gettext("Allowed to modify the access list"),
	/* CHAN_LEVEL_MEMO */
	gettext("Allowed to list/read channel memos"),
	/* CHAN_LEVEL_ASSIGN */
	gettext("Allowed to assign/unassign a bot"),
	/* CHAN_LEVEL_BADWORDS */
	gettext("Allowed to use BADWORDS command"),
	/* CHAN_LEVEL_NOKICK */
	gettext("Never kicked by the bot's kickers"),
	/* CHAN_LEVEL_FANTASIA */
	gettext("Allowed to use fantaisist commands"),
	/* CHAN_LEVEL_SAY */
	gettext("Allowed to use SAY and ACT commands"),
	/* CHAN_LEVEL_GREET */
	gettext("Greet message displayed"),
	/* CHAN_LEVEL_VOICEME */
	gettext("Allowed to (de)voice him/herself"),
	/* CHAN_LEVEL_VOICE */
	gettext("Allowed to use VOICE/DEVOICE commands"),
	/* CHAN_LEVEL_GETKEY */
	gettext("Allowed to use GETKEY command"),
	/* CHAN_LEVEL_OPDEOPME */
	gettext("Allowed to (de)op him/herself"),
	/* CHAN_LEVEL_HALFOPME */
	gettext("Allowed to (de)halfop him/herself"),
	/* CHAN_LEVEL_HALFOP */
	gettext("Allowed to use HALFOP/DEHALFOP commands"),
	/* CHAN_LEVEL_PROTECTME */
	gettext("Allowed to (de)protect him/herself"),
	/* CHAN_LEVEL_PROTECT */
	gettext("Allowed to use PROTECT/DEPROTECT commands"),
	/* CHAN_LEVEL_KICKME */
	gettext("Allowed to kick him/herself"),
	/* CHAN_LEVEL_KICK */
	gettext("Allowed to use KICK command"),
	/* CHAN_LEVEL_SIGNKICK */
	gettext("No signed kick when SIGNKICK LEVEL is used"),
	/* CHAN_LEVEL_BANME */
	gettext("Allowed to ban him/herself"),
	/* CHAN_LEVEL_BAN */
	gettext("Allowed to use BAN command"),
	/* CHAN_LEVEL_TOPIC */
	gettext("Allowed to use TOPIC command"),
	/* CHAN_LEVEL_INFO */
	gettext("Allowed to use INFO command with ALL option"),
	/* CHAN_LEVEL_AUTOOWNER */
	gettext("Automatic mode +q"),
	/* CHAN_LEVEL_OWNER */
	gettext("Allowed to use OWNER command"),
	/* CHAN_LEVEL_OWNERME */
	gettext("Allowed to (de)owner him/herself"),
	/* CHAN_LEVEL_FOUNDER */
	gettext("Allowed to issue commands restricted to channel founders"),
	/* CHAN_IS_REGISTERED */
	gettext("This channel has been registered with %s."),
	/* CHAN_NOT_ALLOWED_OP */
	gettext("You are not allowed chanop status on channel %s."),
	/* CHAN_MAY_NOT_BE_USED */
	gettext("This channel may not be used."),
	/* CHAN_NOT_ALLOWED_TO_JOIN */
	gettext("You are not permitted to be on this channel."),
	/* CHAN_X_INVALID */
	gettext("Channel %s is not a valid channel."),
	/* CHAN_REGISTER_SYNTAX */
	gettext("REGISTER channel description"),
	/* CHAN_REGISTER_DISABLED */
	gettext("Sorry, channel registration is temporarily disabled."),
	/* CHAN_REGISTER_NOT_LOCAL */
	gettext("Local channels cannot be registered."),
	/* CHAN_MAY_NOT_BE_REGISTERED */
	gettext("Channel %s may not be registered."),
	/* CHAN_ALREADY_REGISTERED */
	gettext("Channel %s is already registered!"),
	/* CHAN_MUST_BE_CHANOP */
	gettext("You must be a channel operator to register the channel."),
	/* CHAN_REACHED_CHANNEL_LIMIT */
	gettext("Sorry, you have already reached your limit of %d channels."),
	/* CHAN_EXCEEDED_CHANNEL_LIMIT */
	gettext("Sorry, you have already exceeded your limit of %d channels."),
	/* CHAN_REGISTERED */
	gettext("Channel %s registered under your nickname: %s"),
	/* CHAN_REGISTER_NONE_CHANNEL */
	gettext("You have attempted to register a nonexistent channel %s"),
	/* CHAN_SYMBOL_REQUIRED */
	gettext("Please use the symbol of # when attempting to register"),
	/* CHAN_DROP_SYNTAX */
	gettext("DROP channel"),
	/* CHAN_DROP_DISABLED */
	gettext("Sorry, channel de-registration is temporarily disabled."),
	/* CHAN_DROPPED */
	gettext("Channel %s has been dropped."),
	/* CHAN_SASET_SYNTAX */
	gettext("SASET channel option parameters"),
	/* CHAN_SASET_KEEPTOPIC_SYNTAX */
	gettext("SASET channel KEEPTOPIC {ON | OFF}"),
	/* CHAN_SASET_OPNOTICE_SYNTAX */
	gettext("SASET channel OPNOTICE {ON | OFF}"),
	/* CHAN_SASET_PEACE_SYNTAX */
	gettext("SASET channel PEACE {ON | OFF}"),
	/* CHAN_SASET_PERSIST_SYNTAX */
	gettext("SASET channel PERSIST {ON | OFF}"),
	/* CHAN_SASET_PRIVATE_SYNTAX */
	gettext("SASET channel PRIVATE {ON | OFF}"),
	/* CHAN_SASET_RESTRICTED_SYNTAX */
	gettext("SASET channel RESTRICTED {ON | OFF}"),
	/* CHAN_SASET_SECURE_SYNTAX */
	gettext("SASET channel SECURE {ON | OFF}"),
	/* CHAN_SASET_SECUREFOUNDER_SYNTAX */
	gettext("SASET channel SECUREFOUNDER {ON | OFF}"),
	/* CHAN_SASET_SECUREOPS_SYNTAX */
	gettext("SASET channel SECUREOPS {ON | OFF}"),
	/* CHAN_SASET_SIGNKICK_SYNTAX */
	gettext("SASET channel SIGNKICK {ON | OFF}"),
	/* CHAN_SASET_TOPICLOCK_SYNTAX */
	gettext("SASET channel TOPICLOCK {ON | OFF}"),
	/* CHAN_SASET_XOP_SYNTAX */
	gettext("SASET channel XOP {ON | OFF}"),
	/* CHAN_SET_SYNTAX */
	gettext("SET channel option parameters"),
	/* CHAN_SET_DISABLED */
	gettext("Sorry, channel option setting is temporarily disabled."),
	/* CHAN_SETTING_CHANGED */
	gettext("%s for %s set to %s."),
	/* CHAN_SETTING_UNSET */
	gettext("%s for %s unset."),
	/* CHAN_SET_FOUNDER_TOO_MANY_CHANS */
	gettext("%s has too many channels registered."),
	/* CHAN_FOUNDER_CHANGED */
	gettext("Founder of %s changed to %s."),
	/* CHAN_SUCCESSOR_CHANGED */
	gettext("Successor for %s changed to %s."),
	/* CHAN_SUCCESSOR_UNSET */
	gettext("Successor for %s unset."),
	/* CHAN_SUCCESSOR_IS_FOUNDER */
	gettext("%s cannot be the successor on channel %s because he is its founder."),
	/* CHAN_DESC_CHANGED */
	gettext("Description of %s changed to %s."),
	/* CHAN_EMAIL_CHANGED */
	gettext("E-mail address for %s changed to %s."),
	/* CHAN_EMAIL_UNSET */
	gettext("E-mail address for %s unset."),
	/* CHAN_ENTRY_MSG_CHANGED */
	gettext("Entry message for %s changed."),
	/* CHAN_ENTRY_MSG_UNSET */
	gettext("Entry message for %s unset."),
	/* CHAN_SET_BANTYPE_INVALID */
	gettext("%s is not a valid ban type."),
	/* CHAN_SET_BANTYPE_CHANGED */
	gettext("Ban type for channel %s is now #%d."),
	/* CHAN_SET_MLOCK_UNKNOWN_CHAR */
	gettext("Unknown mode character %c ignored."),
	/* CHAN_SET_MLOCK_IMPOSSIBLE_CHAR */
	gettext("Mode %c ignored because you can't lock it."),
	/* CHAN_SET_MLOCK_L_REQUIRED */
	gettext("You must lock mode +l as well to lock mode +L."),
	/* CHAN_SET_MLOCK_K_REQUIRED */
	gettext("You must lock mode +i as well to lock mode +K."),
	/* CHAN_MLOCK_CHANGED */
	gettext("Mode lock on channel %s changed to %s."),
	/* CHAN_SET_KEEPTOPIC_SYNTAX */
	gettext("SET channel KEEPTOPIC {ON | OFF}"),
	/* CHAN_SET_KEEPTOPIC_ON */
	gettext("Topic retention option for %s is now ON."),
	/* CHAN_SET_KEEPTOPIC_OFF */
	gettext("Topic retention option for %s is now OFF."),
	/* CHAN_SET_TOPICLOCK_SYNTAX */
	gettext("SET channel TOPICLOCK {ON | OFF}"),
	/* CHAN_SET_TOPICLOCK_ON */
	gettext("Topic lock option for %s is now ON."),
	/* CHAN_SET_TOPICLOCK_OFF */
	gettext("Topic lock option for %s is now OFF."),
	/* CHAN_SET_PEACE_SYNTAX */
	gettext("SET channel PEACE {ON | OFF}"),
	/* CHAN_SET_PEACE_ON */
	gettext("Peace option for %s is now ON."),
	/* CHAN_SET_PEACE_OFF */
	gettext("Peace option for %s is now OFF."),
	/* CHAN_SET_PRIVATE_SYNTAX */
	gettext("SET channel PRIVATE {ON | OFF}"),
	/* CHAN_SET_PRIVATE_ON */
	gettext("Private option for %s is now ON."),
	/* CHAN_SET_PRIVATE_OFF */
	gettext("Private option for %s is now OFF."),
	/* CHAN_SET_SECUREOPS_SYNTAX */
	gettext("SET channel SECUREOPS {ON | OFF}"),
	/* CHAN_SET_SECUREOPS_ON */
	gettext("Secure ops option for %s is now ON."),
	/* CHAN_SET_SECUREOPS_OFF */
	gettext("Secure ops option for %s is now OFF."),
	/* CHAN_SET_SECUREFOUNDER_SYNTAX */
	gettext("SET channel SECUREFOUNDER {ON | OFF}"),
	/* CHAN_SET_SECUREFOUNDER_ON */
	gettext("Secure founder option for %s is now ON."),
	/* CHAN_SET_SECUREFOUNDER_OFF */
	gettext("Secure founder option for %s is now OFF."),
	/* CHAN_SET_RESTRICTED_SYNTAX */
	gettext("SET channel RESTRICTED {ON | OFF}"),
	/* CHAN_SET_RESTRICTED_ON */
	gettext("Restricted access option for %s is now ON."),
	/* CHAN_SET_RESTRICTED_OFF */
	gettext("Restricted access option for %s is now OFF."),
	/* CHAN_SET_SECURE_SYNTAX */
	gettext("SET channel SECURE {ON | OFF}"),
	/* CHAN_SET_SECURE_ON */
	gettext("Secure option for %s is now ON."),
	/* CHAN_SET_SECURE_OFF */
	gettext("Secure option for %s is now OFF."),
	/* CHAN_SET_SIGNKICK_SYNTAX */
	gettext("SET channel SIGNKICK {ON | LEVEL | OFF}"),
	/* CHAN_SET_SIGNKICK_ON */
	gettext("Signed kick option for %s is now ON."),
	/* CHAN_SET_SIGNKICK_LEVEL */
	gettext("Signed kick option for %s is now ON, but depends of the\n"
	"level of the user that is using the command."),
	/* CHAN_SET_SIGNKICK_OFF */
	gettext("Signed kick option for %s is now OFF."),
	/* CHAN_SET_OPNOTICE_SYNTAX */
	gettext("SET channel OPNOTICE {ON | OFF}"),
	/* CHAN_SET_OPNOTICE_ON */
	gettext("Op-notice option for %s is now ON."),
	/* CHAN_SET_OPNOTICE_OFF */
	gettext("Op-notice option for %s is now OFF."),
	/* CHAN_SET_XOP_SYNTAX */
	gettext("SET channel XOP {ON | OFF}"),
	/* CHAN_SET_XOP_ON */
	gettext("xOP lists system for %s is now ON."),
	/* CHAN_SET_XOP_OFF */
	gettext("xOP lists system for %s is now OFF."),
	/* CHAN_SET_PERSIST_SYNTAX */
	gettext("SET channel PERSIST {ON | OFF}"),
	/* CHAN_SET_PERSIST_ON */
	gettext("Channel %s is now persistant."),
	/* CHAN_SET_PERSIST_OFF */
	gettext("Channel %s is no longer persistant."),
	/* CHAN_SET_NOEXPIRE_SYNTAX */
	gettext("SET channel NOEXPIRE {ON | OFF}"),
	/* CHAN_SET_NOEXPIRE_ON */
	gettext("Channel %s will not expire."),
	/* CHAN_SET_NOEXPIRE_OFF */
	gettext("Channel %s will expire."),
	/* CHAN_XOP_REACHED_LIMIT */
	gettext("Sorry, you can only have %d AOP/SOP/VOP entries on a channel."),
	/* CHAN_XOP_LIST_FORMAT */
	gettext("  %3d  %s"),
	/* CHAN_XOP_ACCESS */
	gettext("You can't use this command. Use the ACCESS command instead.\n"
	"Type %R%s HELP ACCESS for more information."),
	/* CHAN_XOP_NOT_AVAILABLE */
	gettext("xOP system is not available."),
	/* CHAN_QOP_SYNTAX */
	gettext("QOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]"),
	/* CHAN_QOP_DISABLED */
	gettext("Sorry, channel QOP list modification is temporarily disabled."),
	/* CHAN_QOP_NICKS_ONLY */
	gettext("Channel QOP lists may only contain registered nicknames."),
	/* CHAN_QOP_ADDED */
	gettext("%s added to %s QOP list."),
	/* CHAN_QOP_MOVED */
	gettext("%s moved to %s QOP list."),
	/* CHAN_QOP_NO_SUCH_ENTRY */
	gettext("No such entry (#%d) on %s QOP list."),
	/* CHAN_QOP_NOT_FOUND */
	gettext("%s not found on %s QOP list."),
	/* CHAN_QOP_NO_MATCH */
	gettext("No matching entries on %s QOP list."),
	/* CHAN_QOP_DELETED */
	gettext("%s deleted from %s QOP list."),
	/* CHAN_QOP_DELETED_ONE */
	gettext("Deleted 1 entry from %s QOP list."),
	/* CHAN_QOP_DELETED_SEVERAL */
	gettext("Deleted %d entries from %s QOP list."),
	/* CHAN_QOP_LIST_EMPTY */
	gettext("%s QOP list is empty."),
	/* CHAN_QOP_LIST_HEADER */
	gettext("QOP list for %s:\n"
	"  Num  Nick"),
	/* CHAN_QOP_CLEAR */
	gettext("Channel %s QOP list has been cleared."),
	/* CHAN_AOP_SYNTAX */
	gettext("AOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]"),
	/* CHAN_AOP_DISABLED */
	gettext("Sorry, channel AOP list modification is temporarily disabled."),
	/* CHAN_AOP_NICKS_ONLY */
	gettext("Channel AOP lists may only contain registered nicknames."),
	/* CHAN_AOP_ADDED */
	gettext("%s added to %s AOP list."),
	/* CHAN_AOP_MOVED */
	gettext("%s moved to %s AOP list."),
	/* CHAN_AOP_NO_SUCH_ENTRY */
	gettext("No such entry (#%d) on %s AOP list."),
	/* CHAN_AOP_NOT_FOUND */
	gettext("%s not found on %s AOP list."),
	/* CHAN_AOP_NO_MATCH */
	gettext("No matching entries on %s AOP list."),
	/* CHAN_AOP_DELETED */
	gettext("%s deleted from %s AOP list."),
	/* CHAN_AOP_DELETED_ONE */
	gettext("Deleted 1 entry from %s AOP list."),
	/* CHAN_AOP_DELETED_SEVERAL */
	gettext("Deleted %d entries from %s AOP list."),
	/* CHAN_AOP_LIST_EMPTY */
	gettext("%s AOP list is empty."),
	/* CHAN_AOP_LIST_HEADER */
	gettext("AOP list for %s:\n"
	"  Num  Nick"),
	/* CHAN_AOP_CLEAR */
	gettext("Channel %s AOP list has been cleared."),
	/* CHAN_HOP_SYNTAX */
	gettext("HOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]"),
	/* CHAN_HOP_DISABLED */
	gettext("Sorry, channel HOP list modification is temporarily disabled."),
	/* CHAN_HOP_NICKS_ONLY */
	gettext("Channel HOP lists may only contain registered nicknames."),
	/* CHAN_HOP_ADDED */
	gettext("%s added to %s HOP list."),
	/* CHAN_HOP_MOVED */
	gettext("%s moved to %s HOP list."),
	/* CHAN_HOP_NO_SUCH_ENTRY */
	gettext("No such entry (#%d) on %s HOP list."),
	/* CHAN_HOP_NOT_FOUND */
	gettext("%s not found on %s HOP list."),
	/* CHAN_HOP_NO_MATCH */
	gettext("No matching entries on %s HOP list."),
	/* CHAN_HOP_DELETED */
	gettext("%s deleted from %s HOP list."),
	/* CHAN_HOP_DELETED_ONE */
	gettext("Deleted 1 entry from %s HOP list."),
	/* CHAN_HOP_DELETED_SEVERAL */
	gettext("Deleted %d entries from %s HOP list."),
	/* CHAN_HOP_LIST_EMPTY */
	gettext("%s HOP list is empty."),
	/* CHAN_HOP_LIST_HEADER */
	gettext("HOP list for %s:\n"
	"  Num  Nick"),
	/* CHAN_HOP_CLEAR */
	gettext("Channel %s HOP list has been cleared."),
	/* CHAN_SOP_SYNTAX */
	gettext("SOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]"),
	/* CHAN_SOP_DISABLED */
	gettext("Sorry, channel SOP list modification is temporarily disabled."),
	/* CHAN_SOP_NICKS_ONLY */
	gettext("Channel SOP lists may only contain registered nicknames."),
	/* CHAN_SOP_ADDED */
	gettext("%s added to %s SOP list."),
	/* CHAN_SOP_MOVED */
	gettext("%s moved to %s SOP list."),
	/* CHAN_SOP_NO_SUCH_ENTRY */
	gettext("No such entry (#%d) on %s SOP list."),
	/* CHAN_SOP_NOT_FOUND */
	gettext("%s not found on %s SOP list."),
	/* CHAN_SOP_NO_MATCH */
	gettext("No matching entries on %s SOP list."),
	/* CHAN_SOP_DELETED */
	gettext("%s deleted from %s SOP list."),
	/* CHAN_SOP_DELETED_ONE */
	gettext("Deleted 1 entry from %s SOP list."),
	/* CHAN_SOP_DELETED_SEVERAL */
	gettext("Deleted %d entries from %s SOP list."),
	/* CHAN_SOP_LIST_EMPTY */
	gettext("%s SOP list is empty."),
	/* CHAN_SOP_LIST_HEADER */
	gettext("SOP list for %s:\n"
	"  Num  Nick"),
	/* CHAN_SOP_CLEAR */
	gettext("Channel %s SOP list has been cleared."),
	/* CHAN_VOP_SYNTAX */
	gettext("VOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]"),
	/* CHAN_VOP_DISABLED */
	gettext("Sorry, channel VOP list modification is temporarily disabled."),
	/* CHAN_VOP_NICKS_ONLY */
	gettext("Channel VOP lists may only contain registered nicknames."),
	/* CHAN_VOP_ADDED */
	gettext("%s added to %s VOP list."),
	/* CHAN_VOP_MOVED */
	gettext("%s moved to %s VOP list."),
	/* CHAN_VOP_NO_SUCH_ENTRY */
	gettext("No such entry (#%d) on %s VOP list."),
	/* CHAN_VOP_NOT_FOUND */
	gettext("%s not found on %s VOP list."),
	/* CHAN_VOP_NO_MATCH */
	gettext("No matching entries on %s VOP list."),
	/* CHAN_VOP_DELETED */
	gettext("%s deleted from %s VOP list."),
	/* CHAN_VOP_DELETED_ONE */
	gettext("Deleted 1 entry from %s VOP list."),
	/* CHAN_VOP_DELETED_SEVERAL */
	gettext("Deleted %d entries from %s VOP list."),
	/* CHAN_VOP_LIST_EMPTY */
	gettext("%s VOP list is empty."),
	/* CHAN_VOP_LIST_HEADER */
	gettext("VOP list for %s:\n"
	"  Num  Nick"),
	/* CHAN_VOP_CLEAR */
	gettext("Channel %s VOP list has been cleared."),
	/* CHAN_ACCESS_SYNTAX */
	gettext("ACCESS channel {ADD|DEL|LIST|VIEW|CLEAR} [nick [level] | entry-list]"),
	/* CHAN_ACCESS_XOP */
	gettext("You can't use this command. \n"
	"Use the AOP, SOP and VOP commands instead.\n"
	"Type %R%s HELP command for more information."),
	/* CHAN_ACCESS_XOP_HOP */
	gettext("You can't use this command. \n"
	"Use the AOP, SOP, HOP and VOP commands instead.\n"
	"Type %R%s HELP command for more information."),
	/* CHAN_ACCESS_DISABLED */
	gettext("Sorry, channel access list modification is temporarily disabled."),
	/* CHAN_ACCESS_LEVEL_NONZERO */
	gettext("Access level must be non-zero."),
	/* CHAN_ACCESS_LEVEL_RANGE */
	gettext("Access level must be between %d and %d inclusive."),
	/* CHAN_ACCESS_NICKS_ONLY */
	gettext("Channel access lists may only contain registered nicknames."),
	/* CHAN_ACCESS_REACHED_LIMIT */
	gettext("Sorry, you can only have %d access entries on a channel."),
	/* CHAN_ACCESS_LEVEL_UNCHANGED */
	gettext("Access level for %s on %s unchanged from %d."),
	/* CHAN_ACCESS_LEVEL_CHANGED */
	gettext("Access level for %s on %s changed to %d."),
	/* CHAN_ACCESS_ADDED */
	gettext("%s added to %s access list at level %d."),
	/* CHAN_ACCESS_NO_SUCH_ENTRY */
	gettext("No such entry (#%d) on %s access list."),
	/* CHAN_ACCESS_NOT_FOUND */
	gettext("%s not found on %s access list."),
	/* CHAN_ACCESS_NO_MATCH */
	gettext("No matching entries on %s access list."),
	/* CHAN_ACCESS_DELETED */
	gettext("%s deleted from %s access list."),
	/* CHAN_ACCESS_DELETED_ONE */
	gettext("Deleted 1 entry from %s access list."),
	/* CHAN_ACCESS_DELETED_SEVERAL */
	gettext("Deleted %d entries from %s access list."),
	/* CHAN_ACCESS_LIST_EMPTY */
	gettext("%s access list is empty."),
	/* CHAN_ACCESS_LIST_HEADER */
	gettext("Access list for %s:\n"
	"  Num   Lev  Nick"),
	/* CHAN_ACCESS_LIST_FOOTER */
	gettext("End of access list."),
	/* CHAN_ACCESS_LIST_XOP_FORMAT */
	gettext("  %3d   %s  %s"),
	/* CHAN_ACCESS_LIST_AXS_FORMAT */
	gettext("  %3d  %4d  %s"),
	/* CHAN_ACCESS_CLEAR */
	gettext("Channel %s access list has been cleared."),
	/* CHAN_ACCESS_VIEW_XOP_FORMAT */
	gettext("  %3d   %s   %s\n"
	"    by %s, last seen %s"),
	/* CHAN_ACCESS_VIEW_AXS_FORMAT */
	gettext("  %3d  %4d  %s\n"
	"    by %s, last seen %s"),
	/* CHAN_AKICK_SYNTAX */
	gettext("AKICK channel {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [nick-or-usermask] [reason]"),
	/* CHAN_AKICK_DISABLED */
	gettext("Sorry, channel autokick list modification is temporarily disabled."),
	/* CHAN_AKICK_ALREADY_EXISTS */
	gettext("%s already exists on %s autokick list."),
	/* CHAN_AKICK_REACHED_LIMIT */
	gettext("Sorry, you can only have %d autokick masks on a channel."),
	/* CHAN_AKICK_ADDED */
	gettext("%s added to %s autokick list."),
	/* CHAN_AKICK_NO_SUCH_ENTRY */
	gettext("No such entry (#%d) on %s autokick list."),
	/* CHAN_AKICK_NOT_FOUND */
	gettext("%s not found on %s autokick list."),
	/* CHAN_AKICK_NO_MATCH */
	gettext("No matching entries on %s autokick list."),
	/* CHAN_AKICK_STUCK */
	gettext("%s is now always active on channel %s."),
	/* CHAN_AKICK_UNSTUCK */
	gettext("%s is not always active anymore on channel %s."),
	/* CHAN_AKICK_DELETED */
	gettext("%s deleted from %s autokick list."),
	/* CHAN_AKICK_DELETED_ONE */
	gettext("Deleted 1 entry from %s autokick list."),
	/* CHAN_AKICK_DELETED_SEVERAL */
	gettext("Deleted %d entries from %s autokick list."),
	/* CHAN_AKICK_LIST_EMPTY */
	gettext("%s autokick list is empty."),
	/* CHAN_AKICK_LIST_HEADER */
	gettext("Autokick list for %s:"),
	/* CHAN_AKICK_LIST_FORMAT */
	gettext("  %3d %s (%s)"),
	/* CHAN_AKICK_VIEW_FORMAT */
	gettext("%3d %s (by %s on %s)\n"
	"    %s"),
	/* CHAN_AKICK_VIEW_FORMAT_STUCK */
	gettext("%3d %s (stuck) (by %s on %s)\n"
	"    %s"),
	/* CHAN_AKICK_LAST_USED */
	gettext("    Last used %s"),
	/* CHAN_AKICK_ENFORCE_DONE */
	gettext("AKICK ENFORCE for %s complete; %d users were affected."),
	/* CHAN_AKICK_CLEAR */
	gettext("Channel %s akick list has been cleared."),
	/* CHAN_LEVELS_SYNTAX */
	gettext("LEVELS channel {SET | DIS[ABLE] | LIST | RESET} [item [level]]"),
	/* CHAN_LEVELS_XOP */
	gettext("Levels are not available as xOP is enabled on this channel."),
	/* CHAN_LEVELS_RANGE */
	gettext("Level must be between %d and %d inclusive."),
	/* CHAN_LEVELS_CHANGED */
	gettext("Level for %s on channel %s changed to %d."),
	/* CHAN_LEVELS_CHANGED_FOUNDER */
	gettext("Level for %s on channel %s changed to founder only."),
	/* CHAN_LEVELS_UNKNOWN */
	gettext("Setting %s not known.  Type %R%s HELP LEVELS DESC for a list of valid settings."),
	/* CHAN_LEVELS_DISABLED */
	gettext("%s disabled on channel %s."),
	/* CHAN_LEVELS_LIST_HEADER */
	gettext("Access level settings for channel %s:"),
	/* CHAN_LEVELS_LIST_DISABLED */
	gettext("    %-*s  (disabled)"),
	/* CHAN_LEVELS_LIST_FOUNDER */
	gettext("    %-*s  (founder only)"),
	/* CHAN_LEVELS_LIST_NORMAL */
	gettext("    %-*s  %d"),
	/* CHAN_LEVELS_RESET */
	gettext("Access levels for %s reset to defaults."),
	/* CHAN_STATUS_SYNTAX */
	gettext("STATUS channel item"),
	/* CHAN_STATUS_NOT_REGGED */
	gettext("STATUS ERROR Channel %s not registered"),
	/* CHAN_STATUS_FORBIDDEN */
	gettext("STATUS ERROR Channel %s forbidden"),
	/* CHAN_STATUS_NOTONLINE */
	gettext("STATUS ERROR Nick %s not online"),
	/* CHAN_STATUS_INFO */
	gettext("STATUS %s %s %d"),
	/* CHAN_INFO_SYNTAX */
	gettext("INFO channel"),
	/* CHAN_INFO_HEADER */
	gettext("Information for channel %s:"),
	/* CHAN_INFO_FOUNDER */
	gettext("        Founder: %s (%s)"),
	/* CHAN_INFO_NO_FOUNDER */
	gettext("        Founder: %s"),
	/* CHAN_INFO_SUCCESSOR */
	gettext("      Successor: %s (%s)"),
	/* CHAN_INFO_NO_SUCCESSOR */
	gettext("      Successor: %s"),
	/* CHAN_INFO_DESCRIPTION */
	gettext("    Description: %s"),
	/* CHAN_INFO_ENTRYMSG */
	gettext("  Entry message: %s"),
	/* CHAN_INFO_TIME_REGGED */
	gettext("     Registered: %s"),
	/* CHAN_INFO_LAST_USED */
	gettext("      Last used: %s"),
	/* CHAN_INFO_LAST_TOPIC */
	gettext("     Last topic: %s"),
	/* CHAN_INFO_TOPIC_SET_BY */
	gettext("   Topic set by: %s"),
	/* CHAN_INFO_URL */
	gettext("            URL: %s"),
	/* CHAN_INFO_EMAIL */
	gettext(" E-mail address: %s"),
	/* CHAN_INFO_BANTYPE */
	gettext("       Ban type: %d"),
	/* CHAN_INFO_OPTIONS */
	gettext("        Options: %s"),
	/* CHAN_INFO_OPT_KEEPTOPIC */
	gettext("Topic Retention"),
	/* CHAN_INFO_OPT_OPNOTICE */
	gettext("OP Notice"),
	/* CHAN_INFO_OPT_PEACE */
	gettext("Peace"),
	/* CHAN_INFO_OPT_RESTRICTED */
	gettext("Restricted Access"),
	/* CHAN_INFO_OPT_SECURE */
	gettext("Secure"),
	/* CHAN_INFO_OPT_SECUREOPS */
	gettext("Secure Ops"),
	/* CHAN_INFO_OPT_SECUREFOUNDER */
	gettext("Secure Founder"),
	/* CHAN_INFO_OPT_SIGNKICK */
	gettext("Signed kicks"),
	/* CHAN_INFO_OPT_TOPICLOCK */
	gettext("Topic Lock"),
	/* CHAN_INFO_OPT_XOP */
	gettext("xOP lists system"),
	/* CHAN_INFO_OPT_PERSIST */
	gettext("Persistant"),
	/* CHAN_INFO_MODE_LOCK */
	gettext("      Mode lock: %s"),
	/* CHAN_INFO_EXPIRE */
	gettext("      Expires on: %s"),
	/* CHAN_INFO_NO_EXPIRE */
	gettext("This channel will not expire."),
	/* CHAN_LIST_SERVADMIN_SYNTAX */
	gettext("LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE]"),
	/* CHAN_LIST_FORMAT */
	gettext("    %-20s  %s"),
	/* CHAN_LIST_END */
	gettext("End of list - %d/%d matches shown."),
	/* CHAN_INVITE_SYNTAX */
	gettext("INVITE channel"),
	/* CHAN_INVITE_ALREADY_IN */
	gettext("You are already in %s! "),
	/* CHAN_INVITE_SUCCESS */
	gettext("You have been invited to %s."),
	/* CHAN_INVITE_OTHER_SUCCESS */
	gettext("%s has been invited to %s."),
	/* CHAN_UNBAN_SYNTAX */
	gettext("UNBAN channel [nick]"),
	/* CHAN_UNBANNED */
	gettext("You have been unbanned from %s."),
	/* CHAN_UNBANNED_OTHER */
	gettext("%s has been unbanned from %s."),
	/* CHAN_TOPIC_SYNTAX */
	gettext("TOPIC channel [topic]"),
	/* CHAN_CLEAR_SYNTAX */
	gettext("CLEAR channel what"),
	/* CHAN_CLEARED_BANS */
	gettext("All bans on channel %s have been removed."),
	/* CHAN_CLEARED_EXCEPTS */
	gettext("All excepts on channel %s have been removed."),
	/* CHAN_CLEARED_MODES */
	gettext("All modes on channel %s have been reset."),
	/* CHAN_CLEARED_OPS */
	gettext("Mode +o has been cleared from channel %s."),
	/* CHAN_CLEARED_HOPS */
	gettext("Mode +h has been cleared from channel %s."),
	/* CHAN_CLEARED_VOICES */
	gettext("Mode +v has been cleared from channel %s."),
	/* CHAN_CLEARED_USERS */
	gettext("All users have been kicked from channel %s."),
	/* CHAN_CLEARED_INVITES */
	gettext("All invites on channel %s have been removed."),
	/* CHAN_GETKEY_SYNTAX */
	gettext("GETKEY channel"),
	/* CHAN_GETKEY_NOKEY */
	gettext("The channel %s has no key."),
	/* CHAN_GETKEY_KEY */
	gettext("Key for channel %s is %s."),
	/* CHAN_FORBID_SYNTAX */
	gettext("FORBID channel [reason]"),
	/* CHAN_FORBID_SYNTAX_REASON */
	gettext("FORBID channel reason"),
	/* CHAN_FORBID_SUCCEEDED */
	gettext("Channel %s is now forbidden."),
	/* CHAN_FORBID_FAILED */
	gettext("Couldn't forbid channel %s!"),
	/* CHAN_FORBID_REASON */
	gettext("This channel has been forbidden."),
	/* CHAN_SUSPEND_SYNTAX */
	gettext("SUSPEND channel [reason]"),
	/* CHAN_SUSPEND_SYNTAX_REASON */
	gettext("SUSPEND channel reason"),
	/* CHAN_SUSPEND_SUCCEEDED */
	gettext("Channel %s is now suspended."),
	/* CHAN_SUSPEND_FAILED */
	gettext("Couldn't suspended channel %s!"),
	/* CHAN_SUSPEND_REASON */
	gettext("This channel has been suspended."),
	/* CHAN_UNSUSPEND_SYNTAX */
	gettext("UNSUSPEND channel"),
	/* CHAN_UNSUSPEND_ERROR */
	gettext("No # found in front of channel name."),
	/* CHAN_UNSUSPEND_SUCCEEDED */
	gettext("Channel %s is now released."),
	/* CHAN_UNSUSPEND_FAILED */
	gettext("Couldn't release channel %s!"),
	/* CHAN_EXCEPTED */
	gettext("%s matches an except on %s and cannot be banned until the except have been removed."),
	/* CHAN_OP_SYNTAX */
	gettext("OP #channel [nick]"),
	/* CHAN_HALFOP_SYNTAX */
	gettext("HALFOP #channel [nick]"),
	/* CHAN_VOICE_SYNTAX */
	gettext("VOICE #channel [nick]"),
	/* CHAN_PROTECT_SYNTAX */
	gettext("PROTECT #channel [nick]"),
	/* CHAN_OWNER_SYNTAX */
	gettext("OWNER #channel"),
	/* CHAN_DEOP_SYNTAX */
	gettext("DEOP #channel [nick]"),
	/* CHAN_DEHALFOP_SYNTAX */
	gettext("DEHALFOP #channel [nick]"),
	/* CHAN_DEVOICE_SYNTAX */
	gettext("DEVOICE #channel [nick]"),
	/* CHAN_DEPROTECT_SYNTAX */
	gettext("DEROTECT #channel [nick]"),
	/* CHAN_DEOWNER_SYNTAX */
	gettext("DEOWNER #channel"),
	/* CHAN_KICK_SYNTAX */
	gettext("KICK #channel nick [reason]"),
	/* CHAN_BAN_SYNTAX */
	gettext("BAN #channel nick [reason]"),
	/* MEMO_HAVE_NEW_MEMO */
	gettext("You have 1 new memo."),
	/* MEMO_HAVE_NEW_MEMOS */
	gettext("You have %d new memos."),
	/* MEMO_TYPE_READ_LAST */
	gettext("Type %R%s READ LAST to read it."),
	/* MEMO_TYPE_READ_NUM */
	gettext("Type %R%s READ %d to read it."),
	/* MEMO_TYPE_LIST_NEW */
	gettext("Type %R%s LIST NEW to list them."),
	/* MEMO_AT_LIMIT */
	gettext("Warning: You have reached your maximum number of memos (%d).  You will be unable to receive any new memos until you delete some of your current ones."),
	/* MEMO_OVER_LIMIT */
	gettext("Warning: You are over your maximum number of memos (%d).  You will be unable to receive any new memos until you delete some of your current ones."),
	/* MEMO_X_MANY_NOTICE */
	gettext("There are %d memos on channel %s."),
	/* MEMO_X_ONE_NOTICE */
	gettext("There is %d memo on channel %s."),
	/* MEMO_NEW_X_MEMO_ARRIVED */
	gettext("There is a new memo on channel %s.\n"
	"Type %R%s READ %s %d to read it."),
	/* MEMO_NEW_MEMO_ARRIVED */
	gettext("You have a new memo from %s.\n"
	"Type %R%s READ %d to read it."),
	/* MEMO_HAVE_NO_MEMOS */
	gettext("You have no memos."),
	/* MEMO_X_HAS_NO_MEMOS */
	gettext("%s has no memos."),
	/* MEMO_DOES_NOT_EXIST */
	gettext("Memo %d does not exist!"),
	/* MEMO_LIST_NOT_FOUND */
	gettext("No matching memos found."),
	/* MEMO_SEND_SYNTAX */
	gettext("SEND {nick | channel} memo-text"),
	/* MEMO_SEND_DISABLED */
	gettext("Sorry, memo sending is temporarily disabled."),
	/* MEMO_SEND_PLEASE_WAIT */
	gettext("Please wait %d seconds before using the SEND command again."),
	/* MEMO_X_GETS_NO_MEMOS */
	gettext("%s cannot receive memos."),
	/* MEMO_X_HAS_TOO_MANY_MEMOS */
	gettext("%s currently has too many memos and cannot receive more."),
	/* MEMO_SENT */
	gettext("Memo sent to %s."),
	/* MEMO_MASS_SENT */
	gettext("A massmemo has been sent to all registered users."),
	/* MEMO_STAFF_SYNTAX */
	gettext("STAFF memo-text"),
	/* MEMO_CANCEL_SYNTAX */
	gettext("CANCEL {nick | channel}"),
	/* MEMO_CANCEL_DISABLED */
	gettext("Sorry, memo canceling is temporarily disabled."),
	/* MEMO_CANCEL_NONE */
	gettext("No memo was cancelable."),
	/* MEMO_CANCELLED */
	gettext("Last memo to %s has been cancelled."),
	/* MEMO_LIST_SYNTAX */
	gettext("LIST [channel] [list | NEW]"),
	/* MEMO_HAVE_NO_NEW_MEMOS */
	gettext("You have no new memos."),
	/* MEMO_X_HAS_NO_NEW_MEMOS */
	gettext("%s has no new memos."),
	/* MEMO_LIST_MEMOS */
	gettext("Memos for %s.  To read, type: %R%s READ num"),
	/* MEMO_LIST_NEW_MEMOS */
	gettext("New memos for %s.  To read, type: %R%s READ num"),
	/* MEMO_LIST_CHAN_MEMOS */
	gettext("Memos for %s.  To read, type: %R%s READ %s num"),
	/* MEMO_LIST_CHAN_NEW_MEMOS */
	gettext("New memos for %s.  To read, type: %R%s READ %s num"),
	/* MEMO_LIST_HEADER */
	gettext(" Num  Sender            Date/Time"),
	/* MEMO_LIST_FORMAT */
	gettext("%c%3d  %-16s  %s"),
	/* MEMO_READ_SYNTAX */
	gettext("READ [channel] {list | LAST | NEW}"),
	/* MEMO_HEADER */
	gettext("Memo %d from %s (%s).  To delete, type: %R%s DEL %d"),
	/* MEMO_CHAN_HEADER */
	gettext("Memo %d from %s (%s).  To delete, type: %R%s DEL %s %d"),
	/* MEMO_TEXT */
	gettext("%s"),
	/* MEMO_DEL_SYNTAX */
	gettext("DEL [channel] {num | list | ALL}"),
	/* MEMO_DELETED_NONE */
	gettext("No memos were deleted."),
	/* MEMO_DELETED_ONE */
	gettext("Memo %d has been deleted."),
	/* MEMO_DELETED_SEVERAL */
	gettext("Memos %s have been deleted."),
	/* MEMO_DELETED_ALL */
	gettext("All of your memos have been deleted."),
	/* MEMO_CHAN_DELETED_ALL */
	gettext("All memos for channel %s have been deleted."),
	/* MEMO_SET_DISABLED */
	gettext("Sorry, memo option setting is temporarily disabled."),
	/* MEMO_SET_NOTIFY_SYNTAX */
	gettext("SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF }"),
	/* MEMO_SET_NOTIFY_ON */
	gettext("%s will now notify you of memos when you log on and when they are sent to you."),
	/* MEMO_SET_NOTIFY_LOGON */
	gettext("%s will now notify you of memos when you log on or unset /AWAY."),
	/* MEMO_SET_NOTIFY_NEW */
	gettext("%s will now notify you of memos when they are sent to you."),
	/* MEMO_SET_NOTIFY_OFF */
	gettext("%s will not send you any notification of memos."),
	/* MEMO_SET_NOTIFY_MAIL */
	gettext("You will now be informed about new memos via email."),
	/* MEMO_SET_NOTIFY_NOMAIL */
	gettext("You will no longer be informed via email."),
	/* MEMO_SET_NOTIFY_INVALIDMAIL */
	gettext("There's no email address set for your nick."),
	/* MEMO_SET_LIMIT_SYNTAX */
	gettext("SET LIMIT [channel] limit"),
	/* MEMO_SET_LIMIT_SERVADMIN_SYNTAX */
	gettext("SET LIMIT [user | channel] {limit | NONE} [HARD]"),
	/* MEMO_SET_YOUR_LIMIT_FORBIDDEN */
	gettext("You are not permitted to change your memo limit."),
	/* MEMO_SET_LIMIT_FORBIDDEN */
	gettext("The memo limit for %s may not be changed."),
	/* MEMO_SET_YOUR_LIMIT_TOO_HIGH */
	gettext("You cannot set your memo limit higher than %d."),
	/* MEMO_SET_LIMIT_TOO_HIGH */
	gettext("You cannot set the memo limit for %s higher than %d."),
	/* MEMO_SET_LIMIT_OVERFLOW */
	gettext("Memo limit too large; limiting to %d instead."),
	/* MEMO_SET_YOUR_LIMIT */
	gettext("Your memo limit has been set to %d."),
	/* MEMO_SET_YOUR_LIMIT_ZERO */
	gettext("You will no longer be able to receive memos."),
	/* MEMO_UNSET_YOUR_LIMIT */
	gettext("Your memo limit has been disabled."),
	/* MEMO_SET_LIMIT */
	gettext("Memo limit for %s set to %d."),
	/* MEMO_SET_LIMIT_ZERO */
	gettext("Memo limit for %s set to 0."),
	/* MEMO_UNSET_LIMIT */
	gettext("Memo limit disabled for %s."),
	/* MEMO_INFO_SYNTAX */
	gettext("INFO [channel]"),
	/* MEMO_INFO_SERVADMIN_SYNTAX */
	gettext("INFO [nick | channel]"),
	/* MEMO_INFO_NO_MEMOS */
	gettext("You currently have no memos."),
	/* MEMO_INFO_MEMO */
	gettext("You currently have 1 memo."),
	/* MEMO_INFO_MEMO_UNREAD */
	gettext("You currently have 1 memo, and it has not yet been read."),
	/* MEMO_INFO_MEMOS */
	gettext("You currently have %d memos."),
	/* MEMO_INFO_MEMOS_ONE_UNREAD */
	gettext("You currently have %d memos, of which 1 is unread."),
	/* MEMO_INFO_MEMOS_SOME_UNREAD */
	gettext("You currently have %d memos, of which %d are unread."),
	/* MEMO_INFO_MEMOS_ALL_UNREAD */
	gettext("You currently have %d memos; all of them are unread."),
	/* MEMO_INFO_LIMIT */
	gettext("Your memo limit is %d."),
	/* MEMO_INFO_HARD_LIMIT */
	gettext("Your memo limit is %d, and may not be changed."),
	/* MEMO_INFO_LIMIT_ZERO */
	gettext("Your memo limit is 0; you will not receive any new memos."),
	/* MEMO_INFO_HARD_LIMIT_ZERO */
	gettext("Your memo limit is 0; you will not receive any new memos.  You cannot change this limit."),
	/* MEMO_INFO_NO_LIMIT */
	gettext("You have no limit on the number of memos you may keep."),
	/* MEMO_INFO_NOTIFY_OFF */
	gettext("You will not be notified of new memos."),
	/* MEMO_INFO_NOTIFY_ON */
	gettext("You will be notified of new memos at logon and when they arrive."),
	/* MEMO_INFO_NOTIFY_RECEIVE */
	gettext("You will be notified when new memos arrive."),
	/* MEMO_INFO_NOTIFY_SIGNON */
	gettext("You will be notified of new memos at logon."),
	/* MEMO_INFO_X_NO_MEMOS */
	gettext("%s currently has no memos."),
	/* MEMO_INFO_X_MEMO */
	gettext("%s currently has 1 memo."),
	/* MEMO_INFO_X_MEMO_UNREAD */
	gettext("%s currently has 1 memo, and it has not yet been read."),
	/* MEMO_INFO_X_MEMOS */
	gettext("%s currently has %d memos."),
	/* MEMO_INFO_X_MEMOS_ONE_UNREAD */
	gettext("%s currently has %d memos, of which 1 is unread."),
	/* MEMO_INFO_X_MEMOS_SOME_UNREAD */
	gettext("%s currently has %d memos, of which %d are unread."),
	/* MEMO_INFO_X_MEMOS_ALL_UNREAD */
	gettext("%s currently has %d memos; all of them are unread."),
	/* MEMO_INFO_X_LIMIT */
	gettext("%s's memo limit is %d."),
	/* MEMO_INFO_X_HARD_LIMIT */
	gettext("%s's memo limit is %d, and may not be changed."),
	/* MEMO_INFO_X_NO_LIMIT */
	gettext("%s has no memo limit."),
	/* MEMO_INFO_X_NOTIFY_OFF */
	gettext("%s is not notified of new memos."),
	/* MEMO_INFO_X_NOTIFY_ON */
	gettext("%s is notified of new memos at logon and when they arrive."),
	/* MEMO_INFO_X_NOTIFY_RECEIVE */
	gettext("%s is notified when new memos arrive."),
	/* MEMO_INFO_X_NOTIFY_SIGNON */
	gettext("%s is notified of news memos at logon."),
	/* MEMO_MAIL_SUBJECT */
	gettext("New memo"),
	/* NICK_MAIL_TEXT */
	gettext("Hi %s\n"
	" \n"
	"You've just received a new memo from %s. This is memo number %d.\n"
	" \n"
	"Memo text:\n"
	" \n"
	"%s"),
	/* MEMO_RSEND_PLEASE_WAIT */
	gettext("Please wait %d seconds before using the RSEND command again."),
	/* MEMO_RSEND_DISABLED */
	gettext("Sorry, RSEND has been disabled on this network."),
	/* MEMO_RSEND_SYNTAX */
	gettext("RSEND {nick | channel} memo-text"),
	/* MEMO_RSEND_NICK_MEMO_TEXT */
	gettext("[auto-memo] The memo you sent has been viewed."),
	/* MEMO_RSEND_CHAN_MEMO_TEXT */
	gettext("[auto-memo] The memo you sent to %s has been viewed."),
	/* MEMO_RSEND_USER_NOTIFICATION */
	gettext("A notification memo has been sent to %s informing him/her you have\n"
	"read his/her memo."),
	/* MEMO_CHECK_SYNTAX */
	gettext("CHECK nickname"),
	/* MEMO_CHECK_NOT_READ */
	gettext("The last memo you sent to %s (sent on %s) has not yet been read."),
	/* MEMO_CHECK_READ */
	gettext("The last memo you sent to %s (sent on %s) has been read."),
	/* MEMO_CHECK_NO_MEMO */
	gettext("Nick %s doesn't have a memo from you."),
	/* MEMO_NO_RSEND_SELF */
	gettext("You can not request a receipt when sending a memo to yourself."),
	/* BOT_DOES_NOT_EXIST */
	gettext("Bot %s does not exist."),
	/* BOT_NOT_ASSIGNED */
	gettext("You must assign a bot to the channel before using this command.\n"
	"Type %R%S HELP ASSIGN for more information."),
	/* BOT_NOT_ON_CHANNEL */
	gettext("Bot is not on channel %s."),
	/* BOT_REASON_BADWORD */
	gettext("Don't use the word \"%s\" on this channel!"),
	/* BOT_REASON_BADWORD_GENTLE */
	gettext("Watch your language!"),
	/* BOT_REASON_BOLD */
	gettext("Don't use bolds on this channel!"),
	/* BOT_REASON_CAPS */
	gettext("Turn caps lock OFF!"),
	/* BOT_REASON_COLOR */
	gettext("Don't use colors on this channel!"),
	/* BOT_REASON_FLOOD */
	gettext("Stop flooding!"),
	/* BOT_REASON_REPEAT */
	gettext("Stop repeating yourself!"),
	/* BOT_REASON_REVERSE */
	gettext("Don't use reverses on this channel!"),
	/* BOT_REASON_UNDERLINE */
	gettext("Don't use underlines on this channel!"),
	/* BOT_REASON_ITALIC */
	gettext("Don't use italics on this channel!"),
	/* BOT_SEEN_BOT */
	gettext("You found me, %s!"),
	/* BOT_SEEN_YOU */
	gettext("Looking for yourself, eh %s?"),
	/* BOT_SEEN_ON_CHANNEL */
	gettext("%s is on the channel right now!"),
	/* BOT_SEEN_ON_CHANNEL_AS */
	gettext("%s is on the channel right now (as %s) ! "),
	/* BOT_SEEN_ON */
	gettext("%s was last seen here %s ago."),
	/* BOT_SEEN_NEVER */
	gettext("I've never seen %s on this channel."),
	/* BOT_SEEN_UNKNOWN */
	gettext("I don't know who %s is."),
	/* BOT_BOT_SYNTAX */
	gettext("BOT ADD nick user host real\n"
	"BOT CHANGE oldnick newnick [user [host [real]]]\n"
	"BOT DEL nick"),
	/* BOT_BOT_ALREADY_EXISTS */
	gettext("Bot %s already exists."),
	/* BOT_BOT_CREATION_FAILED */
	gettext("Sorry, bot creation failed."),
	/* BOT_BOT_READONLY */
	gettext("Sorry, bot modification is temporarily disabled."),
	/* BOT_BOT_ADDED */
	gettext("%s!%s@%s (%s) added to the bot list."),
	/* BOT_BOT_ANY_CHANGES */
	gettext("Old info is equal to the new one."),
	/* BOT_BOT_CHANGED */
	gettext("Bot %s has been changed to %s!%s@%s (%s)"),
	/* BOT_BOT_DELETED */
	gettext("Bot %s has been deleted."),
	/* BOT_BOTLIST_HEADER */
	gettext("Bot list:"),
	/* BOT_BOTLIST_PRIVATE_HEADER */
	gettext("Bots reserved to IRC operators:"),
	/* BOT_BOTLIST_FOOTER */
	gettext("%d bots available."),
	/* BOT_BOTLIST_EMPTY */
	gettext("There are no bots available at this time.\n"
	"Ask a Services Operator to create one!"),
	/* BOT_ASSIGN_SYNTAX */
	gettext("ASSIGN chan nick"),
	/* BOT_ASSIGN_READONLY */
	gettext("Sorry, bot assignment is temporarily disabled."),
	/* BOT_ASSIGN_ALREADY */
	gettext("Bot %s is already assigned to channel %s."),
	/* BOT_ASSIGN_ASSIGNED */
	gettext("Bot %s has been assigned to %s."),
	/* BOT_UNASSIGN_SYNTAX */
	gettext("UNASSIGN chan"),
	/* BOT_UNASSIGN_UNASSIGNED */
	gettext("There is no bot assigned to %s anymore."),
	/* BOT_UNASSIGN_PERSISTANT_CHAN */
	gettext("You can not unassign bots while persist is set on the channel."),
	/* BOT_INFO_SYNTAX */
	gettext("INFO {chan | nick}"),
	/* BOT_INFO_NOT_FOUND */
	gettext("%s is not a valid bot or registered channel."),
	/* BOT_INFO_BOT_HEADER */
	gettext("Information for bot %s:"),
	/* BOT_INFO_BOT_MASK */
	gettext("       Mask : %s@%s"),
	/* BOT_INFO_BOT_REALNAME */
	gettext("  Real name : %s"),
	/* BOT_INFO_BOT_CREATED */
	gettext("    Created : %s"),
	/* BOT_INFO_BOT_USAGE */
	gettext("    Used on : %d channel(s)"),
	/* BOT_INFO_BOT_OPTIONS */
	gettext("    Options : %s"),
	/* BOT_INFO_CHAN_BOT */
	gettext("           Bot nick : %s"),
	/* BOT_INFO_CHAN_BOT_NONE */
	gettext("           Bot nick : not assigned yet."),
	/* BOT_INFO_CHAN_KICK_BADWORDS */
	gettext("   Bad words kicker : %s"),
	/* BOT_INFO_CHAN_KICK_BADWORDS_BAN */
	gettext("   Bad words kicker : %s (%d kick(s) to ban)"),
	/* BOT_INFO_CHAN_KICK_BOLDS */
	gettext("       Bolds kicker : %s"),
	/* BOT_INFO_CHAN_KICK_BOLDS_BAN */
	gettext("       Bolds kicker : %s (%d kick(s) to ban)"),
	/* BOT_INFO_CHAN_KICK_CAPS_ON */
	gettext("        Caps kicker : %s (minimum %d/%d%%)"),
	/* BOT_INFO_CHAN_KICK_CAPS_BAN */
	gettext("        Caps kicker : %s (%d kick(s) to ban; minimum %d/%d%%)"),
	/* BOT_INFO_CHAN_KICK_CAPS_OFF */
	gettext("        Caps kicker : %s"),
	/* BOT_INFO_CHAN_KICK_COLORS */
	gettext("      Colors kicker : %s"),
	/* BOT_INFO_CHAN_KICK_COLORS_BAN */
	gettext("      Colors kicker : %s (%d kick(s) to ban)"),
	/* BOT_INFO_CHAN_KICK_FLOOD_ON */
	gettext("       Flood kicker : %s (%d lines in %ds)"),
	/* BOT_INFO_CHAN_KICK_FLOOD_BAN */
	gettext("       Flood kicker : %s (%d kick(s) to ban; %d lines in %ds)"),
	/* BOT_INFO_CHAN_KICK_FLOOD_OFF */
	gettext("       Flood kicker : %s"),
	/* BOT_INFO_CHAN_KICK_REPEAT_ON */
	gettext("      Repeat kicker : %s (%d times)"),
	/* BOT_INFO_CHAN_KICK_REPEAT_BAN */
	gettext("      Repeat kicker : %s (%d kick(s) to ban; %d times)"),
	/* BOT_INFO_CHAN_KICK_REPEAT_OFF */
	gettext("      Repeat kicker : %s"),
	/* BOT_INFO_CHAN_KICK_REVERSES */
	gettext("    Reverses kicker : %s"),
	/* BOT_INFO_CHAN_KICK_REVERSES_BAN */
	gettext("    Reverses kicker : %s (%d kick(s) to ban)"),
	/* BOT_INFO_CHAN_KICK_UNDERLINES */
	gettext("  Underlines kicker : %s"),
	/* BOT_INFO_CHAN_KICK_UNDERLINES_BAN */
	gettext("  Underlines kicker : %s (%d kick(s) to ban)"),
	/* BOT_INFO_CHAN_KICK_ITALICS */
	gettext("     Italics kicker : %s"),
	/* BOT_INFO_CHAN_KICK_ITALICS_BAN */
	gettext("     Italics kicker : %s (%d kick(s) to ban)"),
	/* BOT_INFO_ACTIVE */
	gettext("enabled"),
	/* BOT_INFO_INACTIVE */
	gettext("disabled"),
	/* BOT_INFO_CHAN_OPTIONS */
	gettext("            Options : %s"),
	/* BOT_INFO_OPT_DONTKICKOPS */
	gettext("Ops protection"),
	/* BOT_INFO_OPT_DONTKICKVOICES */
	gettext("Voices protection"),
	/* BOT_INFO_OPT_FANTASY */
	gettext("Fantasy"),
	/* BOT_INFO_OPT_GREET */
	gettext("Greet"),
	/* BOT_INFO_OPT_NOBOT */
	gettext("No bot"),
	/* BOT_INFO_OPT_SYMBIOSIS */
	gettext("Symbiosis"),
	/* BOT_INFO_OPT_NONE */
	gettext("None"),
	/* BOT_SET_SYNTAX */
	gettext("SET (channel | bot) option settings"),
	/* BOT_SET_DISABLED */
	gettext("Sorry, bot option setting is temporarily disabled."),
	/* BOT_SET_UNKNOWN */
	gettext("Unknown option %s.\n"
	"Type %R%S HELP SET for more information."),
	/* BOT_SET_DONTKICKOPS_SYNTAX */
	gettext("SET channel DONTKICKOPS {ON|OFF}"),
	/* BOT_SET_DONTKICKOPS_ON */
	gettext("Bot won't kick ops on channel %s."),
	/* BOT_SET_DONTKICKOPS_OFF */
	gettext("Bot will kick ops on channel %s."),
	/* BOT_SET_DONTKICKVOICES_SYNTAX */
	gettext("SET channel DONTKICKVOICES {ON|OFF}"),
	/* BOT_SET_DONTKICKVOICES_ON */
	gettext("Bot won't kick voices on channel %s."),
	/* BOT_SET_DONTKICKVOICES_OFF */
	gettext("Bot will kick voices on channel %s."),
	/* BOT_SET_FANTASY_SYNTAX */
	gettext("SET channel FANTASY {ON|OFF}"),
	/* BOT_SET_FANTASY_ON */
	gettext("Fantasy mode is now ON on channel %s."),
	/* BOT_SET_FANTASY_OFF */
	gettext("Fantasy mode is now OFF on channel %s."),
	/* BOT_SET_GREET_SYNTAX */
	gettext("SET channel GREET {ON|OFF}"),
	/* BOT_SET_GREET_ON */
	gettext("Greet mode is now ON on channel %s."),
	/* BOT_SET_GREET_OFF */
	gettext("Greet mode is now OFF on channel %s."),
	/* BOT_SET_NOBOT_SYNTAX */
	gettext("SET botname NOBOT {ON|OFF}"),
	/* BOT_SET_NOBOT_ON */
	gettext("No Bot mode is now ON on channel %s."),
	/* BOT_SET_NOBOT_OFF */
	gettext("No Bot mode is now OFF on channel %s."),
	/* BOT_SET_PRIVATE_SYNTAX */
	gettext("SET botname PRIVATE {ON|OFF}"),
	/* BOT_SET_PRIVATE_ON */
	gettext("Private mode of bot %s is now ON."),
	/* BOT_SET_PRIVATE_OFF */
	gettext("Private mode of bot %s is now OFF."),
	/* BOT_SET_SYMBIOSIS_SYNTAX */
	gettext("SET channel SYMBIOSIS {ON|OFF}"),
	/* BOT_SET_SYMBIOSIS_ON */
	gettext("Symbiosis mode is now ON on channel %s."),
	/* BOT_SET_SYMBIOSIS_OFF */
	gettext("Symbiosis mode is now OFF on channel %s."),
	/* BOT_KICK_SYNTAX */
	gettext("KICK channel option {ON|OFF} [settings]"),
	/* BOT_KICK_DISABLED */
	gettext("Sorry, kicker configuration is temporarily disabled."),
	/* BOT_KICK_UNKNOWN */
	gettext("Unknown option %s.\n"
	"Type %R%S HELP KICK for more information."),
	/* BOT_KICK_BAD_TTB */
	gettext("%s cannot be taken as times to ban."),
	/* BOT_KICK_BADWORDS_ON */
	gettext("Bot will now kick bad words. Use the BADWORDS command\n"
	"to add or remove a bad word."),
	/* BOT_KICK_BADWORDS_ON_BAN */
	gettext("Bot will now kick bad words, and will place a ban after \n"
	"%d kicks for the same user. Use the BADWORDS command\n"
	"to add or remove a bad word."),
	/* BOT_KICK_BADWORDS_OFF */
	gettext("Bot won't kick bad words anymore."),
	/* BOT_KICK_BOLDS_ON */
	gettext("Bot will now kick bolds."),
	/* BOT_KICK_BOLDS_ON_BAN */
	gettext("Bot will now kick bolds, and will place a ban after \n"
	"%d kicks for the same user."),
	/* BOT_KICK_BOLDS_OFF */
	gettext("Bot won't kick bolds anymore."),
	/* BOT_KICK_CAPS_ON */
	gettext("Bot will now kick caps (they must constitute at least\n"
	"%d characters and %d%% of the entire message)."),
	/* BOT_KICK_CAPS_ON_BAN */
	gettext("Bot will now kick caps (they must constitute at least\n"
	"%d characters and %d%% of the entire message), and will \n"
	"place a ban after %d kicks for the same user."),
	/* BOT_KICK_CAPS_OFF */
	gettext("Bot won't kick caps anymore."),
	/* BOT_KICK_COLORS_ON */
	gettext("Bot will now kick colors."),
	/* BOT_KICK_COLORS_ON_BAN */
	gettext("Bot will now kick colors, and will place a ban after %d \n"
	"kicks for the same user."),
	/* BOT_KICK_COLORS_OFF */
	gettext("Bot won't kick colors anymore."),
	/* BOT_KICK_FLOOD_ON */
	gettext("Bot will now kick flood (%d lines in %d seconds)."),
	/* BOT_KICK_FLOOD_ON_BAN */
	gettext("Bot will now kick flood (%d lines in %d seconds), and \n"
	"will place a ban after %d kicks for the same user."),
	/* BOT_KICK_FLOOD_OFF */
	gettext("Bot won't kick flood anymore."),
	/* BOT_KICK_REPEAT_ON */
	gettext("Bot will now kick repeats (users that say %d times\n"
	"the same thing)."),
	/* BOT_KICK_REPEAT_ON_BAN */
	gettext("Bot will now kick repeats (users that say %d times\n"
	"the same thing), and will place a ban after %d \n"
	"kicks for the same user."),
	/* BOT_KICK_REPEAT_OFF */
	gettext("Bot won't kick repeats anymore."),
	/* BOT_KICK_REVERSES_ON */
	gettext("Bot will now kick reverses."),
	/* BOT_KICK_REVERSES_ON_BAN */
	gettext("Bot will now kick reverses, and will place a ban after %d \n"
	"kicks for the same user."),
	/* BOT_KICK_REVERSES_OFF */
	gettext("Bot won't kick reverses anymore."),
	/* BOT_KICK_UNDERLINES_ON */
	gettext("Bot will now kick underlines."),
	/* BOT_KICK_UNDERLINES_ON_BAN */
	gettext("Bot will now kick underlines, and will place a ban after %d \n"
	"kicks for the same user."),
	/* BOT_KICK_UNDERLINES_OFF */
	gettext("Bot won't kick underlines anymore."),
	/* BOT_KICK_ITALICS_ON */
	gettext("Bot will now kick italics."),
	/* BOT_KICK_ITALICS_ON_BAN */
	gettext("Bot will now kick italics, and will place a ban after \n"
	"%d kicks for the same user."),
	/* BOT_KICK_ITALICS_OFF */
	gettext("Bot won't kick italics anymore."),
	/* BOT_BADWORDS_SYNTAX */
	gettext("BADWORDS channel {ADD|DEL|LIST|CLEAR} [word | entry-list] [SINGLE|START|END]"),
	/* BOT_BADWORDS_DISABLED */
	gettext("Sorry, channel bad words list modification is temporarily disabled."),
	/* BOT_BADWORDS_REACHED_LIMIT */
	gettext("Sorry, you can only have %d bad words entries on a channel."),
	/* BOT_BADWORDS_ALREADY_EXISTS */
	gettext("%s already exists in %s bad words list."),
	/* BOT_BADWORDS_ADDED */
	gettext("%s added to %s bad words list."),
	/* BOT_BADWORDS_NO_SUCH_ENTRY */
	gettext("No such entry (#%d) on %s bad words list."),
	/* BOT_BADWORDS_NOT_FOUND */
	gettext("%s not found on %s bad words list."),
	/* BOT_BADWORDS_NO_MATCH */
	gettext("No matching entries on %s bad words list."),
	/* BOT_BADWORDS_DELETED */
	gettext("%s deleted from %s bad words list."),
	/* BOT_BADWORDS_DELETED_ONE */
	gettext("Deleted 1 entry from %s bad words list."),
	/* BOT_BADWORDS_DELETED_SEVERAL */
	gettext("Deleted %d entries from %s bad words list."),
	/* BOT_BADWORDS_LIST_EMPTY */
	gettext("%s bad words list is empty."),
	/* BOT_BADWORDS_LIST_HEADER */
	gettext("Bad words list for %s:\n"
	"  Num   Word                           Type"),
	/* BOT_BADWORDS_LIST_FORMAT */
	gettext("  %3d   %-30s %s"),
	/* BOT_BADWORDS_CLEAR */
	gettext("Bad words list is now empty."),
	/* BOT_SAY_SYNTAX */
	gettext("SAY channel text"),
	/* BOT_ACT_SYNTAX */
	gettext("ACT channel text"),
	/* BOT_EXCEPT */
	gettext("User matches channel except."),
	/* BOT_BAD_NICK */
	gettext("Bot Nicks may only contain valid nick characters."),
	/* BOT_BAD_HOST */
	gettext("Bot Hosts may only contain valid host characters."),
	/* BOT_BAD_IDENT */
	gettext("Bot Idents may only contain valid characters."),
	/* BOT_LONG_IDENT */
	gettext("Bot Idents may only contain %d characters."),
	/* BOT_LONG_HOST */
	gettext("Bot Hosts may only contain %d characters."),
	/* OPER_BOUNCY_MODES */
	gettext("Services is unable to change modes.  Are your servers configured correctly?"),
	/* OPER_BOUNCY_MODES_U_LINE */
	gettext("Services is unable to change modes.  Are your servers' U:lines configured correctly?"),
	/* OPER_GLOBAL_SYNTAX */
	gettext("GLOBAL message"),
	/* OPER_STATS_UNKNOWN_OPTION */
	gettext("Unknown STATS option %s."),
	/* OPER_STATS_CURRENT_USERS */
	gettext("Current users: %d (%d ops)"),
	/* OPER_STATS_MAX_USERS */
	gettext("Maximum users: %d (%s)"),
	/* OPER_STATS_UPTIME_DHMS */
	gettext("Services up %d days, %02d:%02d"),
	/* OPER_STATS_UPTIME_1DHMS */
	gettext("Services up %d day, %02d:%02d"),
	/* OPER_STATS_UPTIME_HMS */
	gettext("Services up %d hours, %d minutes"),
	/* OPER_STATS_UPTIME_H1MS */
	gettext("Services up %d hours, %d minute"),
	/* OPER_STATS_UPTIME_1HMS */
	gettext("Services up %d hour, %d minutes"),
	/* OPER_STATS_UPTIME_1H1MS */
	gettext("Services up %d hour, %d minute"),
	/* OPER_STATS_UPTIME_MS */
	gettext("Services up %d minutes, %d seconds"),
	/* OPER_STATS_UPTIME_M1S */
	gettext("Services up %d minutes, %d second"),
	/* OPER_STATS_UPTIME_1MS */
	gettext("Services up %d minute, %d seconds"),
	/* OPER_STATS_UPTIME_1M1S */
	gettext("Services up %d minute, %d second"),
	/* OPER_STATS_BYTES_READ */
	gettext("Bytes read    : %5d kB"),
	/* OPER_STATS_BYTES_WRITTEN */
	gettext("Bytes written : %5d kB"),
	/* OPER_STATS_USER_MEM */
	gettext("User          : %6d records, %5d kB"),
	/* OPER_STATS_CHANNEL_MEM */
	gettext("Channel       : %6d records, %5d kB"),
	/* OPER_STATS_GROUPS_MEM */
	gettext("NS Groups     : %6d records, %5d kB"),
	/* OPER_STATS_ALIASES_MEM */
	gettext("NS Aliases    : %6d records, %5d kB"),
	/* OPER_STATS_CHANSERV_MEM */
	gettext("ChanServ      : %6d records, %5d kB"),
	/* OPER_STATS_BOTSERV_MEM */
	gettext("BotServ       : %6d records, %5d kB"),
	/* OPER_STATS_HOSTSERV_MEM */
	gettext("HostServ      : %6d records, %5d kB"),
	/* OPER_STATS_OPERSERV_MEM */
	gettext("OperServ      : %6d records, %5d kB"),
	/* OPER_STATS_SESSIONS_MEM */
	gettext("Sessions      : %6d records, %5d kB"),
	/* OPER_STATS_AKILL_COUNT */
	gettext("Current number of AKILLs: %d"),
	/* OPER_STATS_AKILL_EXPIRE_DAYS */
	gettext("Default AKILL expiry time: %d days"),
	/* OPER_STATS_AKILL_EXPIRE_DAY */
	gettext("Default AKILL expiry time: 1 day"),
	/* OPER_STATS_AKILL_EXPIRE_HOURS */
	gettext("Default AKILL expiry time: %d hours"),
	/* OPER_STATS_AKILL_EXPIRE_HOUR */
	gettext("Default AKILL expiry time: 1 hour"),
	/* OPER_STATS_AKILL_EXPIRE_MINS */
	gettext("Default AKILL expiry time: %d minutes"),
	/* OPER_STATS_AKILL_EXPIRE_MIN */
	gettext("Default AKILL expiry time: 1 minute"),
	/* OPER_STATS_AKILL_EXPIRE_NONE */
	gettext("Default AKILL expiry time: No expiration"),
	/* OPER_STATS_SNLINE_COUNT */
	gettext("Current number of SNLINEs: %d"),
	/* OPER_STATS_SNLINE_EXPIRE_DAYS */
	gettext("Default SNLINE expiry time: %d days"),
	/* OPER_STATS_SNLINE_EXPIRE_DAY */
	gettext("Default SNLINE expiry time: 1 day"),
	/* OPER_STATS_SNLINE_EXPIRE_HOURS */
	gettext("Default SNLINE expiry time: %d hours"),
	/* OPER_STATS_SNLINE_EXPIRE_HOUR */
	gettext("Default SNLINE expiry time: 1 hour"),
	/* OPER_STATS_SNLINE_EXPIRE_MINS */
	gettext("Default SNLINE expiry time: %d minutes"),
	/* OPER_STATS_SNLINE_EXPIRE_MIN */
	gettext("Default SNLINE expiry time: 1 minute"),
	/* OPER_STATS_SNLINE_EXPIRE_NONE */
	gettext("Default SNLINE expiry time: No expiration"),
	/* OPER_STATS_SQLINE_COUNT */
	gettext("Current number of SQLINEs: %d"),
	/* OPER_STATS_SQLINE_EXPIRE_DAYS */
	gettext("Default SQLINE expiry time: %d days"),
	/* OPER_STATS_SQLINE_EXPIRE_DAY */
	gettext("Default SQLINE expiry time: 1 day"),
	/* OPER_STATS_SQLINE_EXPIRE_HOURS */
	gettext("Default SQLINE expiry time: %d hours"),
	/* OPER_STATS_SQLINE_EXPIRE_HOUR */
	gettext("Default SQLINE expiry time: 1 hour"),
	/* OPER_STATS_SQLINE_EXPIRE_MINS */
	gettext("Default SQLINE expiry time: %d minutes"),
	/* OPER_STATS_SQLINE_EXPIRE_MIN */
	gettext("Default SQLINE expiry time: 1 minute"),
	/* OPER_STATS_SQLINE_EXPIRE_NONE */
	gettext("Default SQLINE expiry time: No expiration"),
	/* OPER_STATS_SZLINE_COUNT */
	gettext("Current number of SZLINEs: %d"),
	/* OPER_STATS_SZLINE_EXPIRE_DAYS */
	gettext("Default SZLINE expiry time: %d days"),
	/* OPER_STATS_SZLINE_EXPIRE_DAY */
	gettext("Default SZLINE expiry time: 1 day"),
	/* OPER_STATS_SZLINE_EXPIRE_HOURS */
	gettext("Default SZLINE expiry time: %d hours"),
	/* OPER_STATS_SZLINE_EXPIRE_HOUR */
	gettext("Default SZLINE expiry time: 1 hour"),
	/* OPER_STATS_SZLINE_EXPIRE_MINS */
	gettext("Default SZLINE expiry time: %d minutes"),
	/* OPER_STATS_SZLINE_EXPIRE_MIN */
	gettext("Default SZLINE expiry time: 1 minute"),
	/* OPER_STATS_SZLINE_EXPIRE_NONE */
	gettext("Default SZLINE expiry time: No expiration"),
	/* OPER_STATS_RESET */
	gettext("Statistics reset."),
	/* OPER_STATS_UPLINK_SERVER */
	gettext("Uplink server: %s"),
	/* OPER_STATS_UPLINK_CAPAB */
	gettext("Uplink capab: %s"),
	/* OPER_STATS_UPLINK_SERVER_COUNT */
	gettext("Servers found: %d"),
	/* OPER_MODE_SYNTAX */
	gettext("MODE channel modes"),
	/* OPER_UMODE_SYNTAX */
	gettext("UMODE nick modes"),
	/* OPER_UMODE_SUCCESS */
	gettext("Changed usermodes of %s."),
	/* OPER_UMODE_CHANGED */
	gettext("%s changed your usermodes."),
	/* OPER_OLINE_SYNTAX */
	gettext("OLINE nick flags"),
	/* OPER_OLINE_SUCCESS */
	gettext("Operflags %s have been added for %s."),
	/* OPER_OLINE_IRCOP */
	gettext("You are now an IRC Operator."),
	/* OPER_CLEARMODES_SYNTAX */
	gettext("CLEARMODES channel [ALL]"),
	/* OPER_CLEARMODES_DONE */
	gettext("Binary modes and bans cleared from channel %s."),
	/* OPER_CLEARMODES_ALL_DONE */
	gettext("All modes cleared from channel %s."),
	/* OPER_KICK_SYNTAX */
	gettext("KICK channel user reason"),
	/* OPER_SVSNICK_SYNTAX */
	gettext("SVSNICK nick newnick "),
	/* OPER_SVSNICK_NEWNICK */
	gettext("The nick %s is now being changed to %s."),
	/* OPER_AKILL_SYNTAX */
	gettext("AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]"),
	/* OPER_AKILL_EXISTS */
	gettext("%s already exists on the AKILL list."),
	/* OPER_AKILL_ALREADY_COVERED */
	gettext("%s is already covered by %s."),
	/* OPER_AKILL_REACHED_LIMIT */
	gettext("Sorry, you can only have %d AKILLs."),
	/* OPER_AKILL_NO_NICK */
	gettext("Reminder: AKILL masks cannot contain nicknames; make sure you have not included a nick portion in your mask."),
	/* OPER_AKILL_ADDED */
	gettext("%s added to the AKILL list."),
	/* OPER_EXPIRY_CHANGED */
	gettext("Expiry time of %s changed."),
	/* OPER_AKILL_NOT_FOUND */
	gettext("%s not found on the AKILL list."),
	/* OPER_AKILL_NO_MATCH */
	gettext("No matching entries on the AKILL list."),
	/* OPER_AKILL_DELETED */
	gettext("%s deleted from the AKILL list."),
	/* OPER_AKILL_DELETED_ONE */
	gettext("Deleted 1 entry from the AKILL list."),
	/* OPER_AKILL_DELETED_SEVERAL */
	gettext("Deleted %d entries from the AKILL list."),
	/* OPER_AKILL_LIST_EMPTY */
	gettext("AKILL list is empty."),
	/* OPER_AKILL_LIST_HEADER */
	gettext("Current AKILL list:\n"
	"  Num   Mask                              Reason"),
	/* OPER_AKILL_LIST_FORMAT */
	gettext("  %3d   %-32s  %s"),
	/* OPER_AKILL_VIEW_HEADER */
	gettext("Current AKILL list:"),
	/* OPER_VIEW_FORMAT */
	gettext("%3d  %s (by %s on %s; %s)\n"
	"      %s"),
	/* OPER_AKILL_CLEAR */
	gettext("The AKILL list has been cleared."),
	/* OPER_CHANKILL_SYNTAX */
	gettext("CHANKILL [+expiry] {#channel} [reason]"),
	/* OPER_SNLINE_SYNTAX */
	gettext("SNLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list}[:reason]]"),
	/* OPER_SNLINE_UNSUPPORTED */
	gettext("Sorry, SNLINE is not available on this network."),
	/* OPER_SNLINE_EXISTS */
	gettext("%s already exists on the SNLINE list."),
	/* OPER_SNLINE_REACHED_LIMIT */
	gettext("Sorry, you can only have %d SNLINEs."),
	/* OPER_SNLINE_ADDED */
	gettext("%s added to the SNLINE list."),
	/* OPER_SNLINE_NOT_FOUND */
	gettext("%s not found on the SNLINE list."),
	/* OPER_SNLINE_NO_MATCH */
	gettext("No matching entries on the SNLINE list."),
	/* OPER_SNLINE_DELETED */
	gettext("%s deleted from the SNLINE list."),
	/* OPER_SNLINE_DELETED_ONE */
	gettext("Deleted 1 entry from the SNLINE list."),
	/* OPER_SNLINE_DELETED_SEVERAL */
	gettext("Deleted %d entries from the SNLINE list."),
	/* OPER_SNLINE_LIST_EMPTY */
	gettext("SNLINE list is empty."),
	/* OPER_SNLINE_LIST_HEADER */
	gettext("Current SNLINE list:\n"
	"  Num   Mask                              Reason"),
	/* OPER_SNLINE_VIEW_HEADER */
	gettext("Current SNLINE list:"),
	/* OPER_SNLINE_CLEAR */
	gettext("The SNLINE list has been cleared."),
	/* OPER_SQLINE_SYNTAX */
	gettext("SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]"),
	/* OPER_SQLINE_CHANNELS_UNSUPPORTED */
	gettext("Channel SQLINEs are not supported by your IRCd, so you can't use them."),
	/* OPER_SQLINE_EXISTS */
	gettext("%s already exists on the SQLINE list."),
	/* OPER_SQLINE_REACHED_LIMIT */
	gettext("Sorry, you can only have %d SQLINEs."),
	/* OPER_SQLINE_ADDED */
	gettext("%s added to the SQLINE list."),
	/* OPER_SQLINE_NOT_FOUND */
	gettext("%s not found on the SQLINE list."),
	/* OPER_SQLINE_NO_MATCH */
	gettext("No matching entries on the SQLINE list."),
	/* OPER_SQLINE_DELETED */
	gettext("%s deleted from the SQLINE list."),
	/* OPER_SQLINE_DELETED_ONE */
	gettext("Deleted 1 entry from the SQLINE list."),
	/* OPER_SQLINE_DELETED_SEVERAL */
	gettext("Deleted %d entries from the SQLINE list."),
	/* OPER_SQLINE_LIST_EMPTY */
	gettext("SQLINE list is empty."),
	/* OPER_SQLINE_LIST_HEADER */
	gettext("Current SQLINE list:\n"
	"  Num   Mask                              Reason"),
	/* OPER_SQLINE_VIEW_HEADER */
	gettext("Current SQLINE list:"),
	/* OPER_SQLINE_CLEAR */
	gettext("The SQLINE list has been cleared."),
	/* OPER_SZLINE_SYNTAX */
	gettext("SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]"),
	/* OPER_SZLINE_UNSUPPORTED */
	gettext("Sorry, SZLINE is not available on this network."),
	/* OPER_SZLINE_EXISTS */
	gettext("%s already exists on the SZLINE list."),
	/* OPER_SZLINE_REACHED_LIMIT */
	gettext("Sorry, you can only have %d SZLINEs."),
	/* OPER_SZLINE_ONLY_IPS */
	gettext("Reminder: you can only add IP masks to the SZLINE list."),
	/* OPER_SZLINE_ADDED */
	gettext("%s added to the SZLINE list."),
	/* OPER_SZLINE_NOT_FOUND */
	gettext("%s not found on the SZLINE list."),
	/* OPER_SZLINE_NO_MATCH */
	gettext("No matching entries on the SZLINE list."),
	/* OPER_SZLINE_DELETED */
	gettext("%s deleted from the SZLINE list."),
	/* OPER_SZLINE_DELETED_ONE */
	gettext("Deleted 1 entry from the SZLINE list."),
	/* OPER_SZLINE_DELETED_SEVERAL */
	gettext("Deleted %d entries from the SZLINE list."),
	/* OPER_SZLINE_LIST_EMPTY */
	gettext("SZLINE list is empty."),
	/* OPER_SZLINE_LIST_HEADER */
	gettext("Current SZLINE list:\n"
	"  Num   Mask                              Reason"),
	/* OPER_SZLINE_VIEW_HEADER */
	gettext("Current SZLINE list:"),
	/* OPER_SZLINE_CLEAR */
	gettext("The SZLINE list has been cleared."),
	/* OPER_SET_SYNTAX */
	gettext("SET option setting"),
	/* OPER_SET_IGNORE_ON */
	gettext("Ignore code will be used."),
	/* OPER_SET_IGNORE_OFF */
	gettext("Ignore code will not be used."),
	/* OPER_SET_IGNORE_ERROR */
	gettext("Setting for IGNORE must be ON or OFF."),
	/* OPER_SET_READONLY_ON */
	gettext("Services are now in read-only mode."),
	/* OPER_SET_READONLY_OFF */
	gettext("Services are now in read-write mode."),
	/* OPER_SET_READONLY_ERROR */
	gettext("Setting for READONLY must be ON or OFF."),
	/* OPER_SET_LOGCHAN_ON */
	gettext("Services are now reporting log messages to %s."),
	/* OPER_SET_LOGCHAN_OFF */
	gettext("Services are no longer reporting log messages to a channel."),
	/* OPER_SET_LOGCHAN_ERROR */
	gettext("Setting for LOGCHAN must be ON or OFF and LogChannel must be defined."),
	/* OPER_SET_DEBUG_ON */
	gettext("Services are now in debug mode."),
	/* OPER_SET_DEBUG_OFF */
	gettext("Services are now in non-debug mode."),
	/* OPER_SET_DEBUG_LEVEL */
	gettext("Services are now in debug mode (level %d)."),
	/* OPER_SET_DEBUG_ERROR */
	gettext("Setting for DEBUG must be ON, OFF, or a positive number."),
	/* OPER_SET_NOEXPIRE_ON */
	gettext("Services are now in no expire mode."),
	/* OPER_SET_NOEXPIRE_OFF */
	gettext("Services are now in expire mode."),
	/* OPER_SET_NOEXPIRE_ERROR */
	gettext("Setting for NOEXPIRE must be ON or OFF."),
	/* OPER_SET_UNKNOWN_OPTION */
	gettext("Unknown option %s."),
	/* OPER_SET_LIST_OPTION_ON */
	gettext("%s is enabled"),
	/* OPER_SET_LIST_OPTION_OFF */
	gettext("%s is disabled"),
	/* OPER_NOOP_SYNTAX */
	gettext("NOOP {SET|REVOKE} server"),
	/* OPER_NOOP_SET */
	gettext("All O:lines of %s have been removed."),
	/* OPER_NOOP_REVOKE */
	gettext("All O:lines of %s have been reset."),
	/* OPER_JUPE_SYNTAX */
	gettext("JUPE servername [reason]"),
	/* OPER_JUPE_HOST_ERROR */
	gettext("Please use a valid server name when juping"),
	/* OPER_JUPE_INVALID_SERVER */
	gettext("You can not jupe your services server or your uplink server."),
	/* OPER_UPDATING */
	gettext("Updating databases."),
	/* OPER_RELOAD */
	gettext("Services' configuration file has been reloaded."),
	/* OPER_CANNOT_RESTART */
	gettext("SERVICES_BIN not defined; cannot restart.  Rerun the \2configure\2 script and recompile Services to enable the RESTART command."),
	/* OPER_IGNORE_SYNTAX */
	gettext("IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick | mask]"),
	/* OPER_IGNORE_VALID_TIME */
	gettext("You have to enter a valid number as time."),
	/* OPER_IGNORE_TIME_DONE */
	gettext("%s will now be ignored for %s."),
	/* OPER_IGNORE_PERM_DONE */
	gettext("%s will now permanently be ignored."),
	/* OPER_IGNORE_DEL_DONE */
	gettext("%s will no longer be ignored."),
	/* OPER_IGNORE_LIST */
	gettext("Services ignore list:"),
	/* OPER_IGNORE_LIST_NOMATCH */
	gettext("Nick %s not found on ignore list."),
	/* OPER_IGNORE_LIST_EMPTY */
	gettext("Ignore list is empty."),
	/* OPER_IGNORE_LIST_CLEARED */
	gettext("Ignore list has been cleared."),
	/* OPER_KILLCLONES_SYNTAX */
	gettext("KILLCLONES nick"),
	/* OPER_KILLCLONES_UNKNOWN_NICK */
	gettext("Could not find user %s."),
	/* OPER_CHANLIST_HEADER */
	gettext("Channel list:\n"
	"Name                 Users Modes   Topic"),
	/* OPER_CHANLIST_HEADER_USER */
	gettext("%s channel list:\n"
	"Name                 Users Modes   Topic"),
	/* OPER_CHANLIST_RECORD */
	gettext("%-20s  %4d +%-6s %s"),
	/* OPER_CHANLIST_END */
	gettext("End of channel list."),
	/* OPER_USERLIST_HEADER */
	gettext("Users list:\n"
	"Nick                 Mask"),
	/* OPER_USERLIST_HEADER_CHAN */
	gettext("%s users list:\n"
	"Nick                 Mask"),
	/* OPER_USERLIST_RECORD */
	gettext("%-20s %s@%s"),
	/* OPER_USERLIST_END */
	gettext("End of users list."),
	/* OPER_SUPER_ADMIN_ON */
	gettext("You are now a SuperAdmin"),
	/* OPER_SUPER_ADMIN_OFF */
	gettext("You are no longer a SuperAdmin"),
	/* OPER_SUPER_ADMIN_SYNTAX */
	gettext("Setting for SuperAdmin must be ON or OFF (must be enabled in services.conf)"),
	/* OPER_SUPER_ADMIN_WALL_ON */
	gettext("%s is now a Super-Admin"),
	/* OPER_SUPER_ADMIN_WALL_OFF */
	gettext("%s is no longer a Super-Admin"),
	/* OPER_SUPER_ADMIN_ONLY */
	gettext("Only Super-Admins can use this command."),
	/* OPER_STAFF_LIST_HEADER */
	gettext("On Level Nick"),
	/* OPER_STAFF_FORMAT */
	gettext(" %c %s  %s"),
	/* OPER_STAFF_AFORMAT */
	gettext(" %c %s  %s [%s]"),
	/* OPER_DEFCON_SYNTAX */
	gettext("DEFCON [1|2|3|4|5]"),
	/* OPER_DEFCON_DENIED */
	gettext("Services are in Defcon mode, Please try again later."),
	/* OPER_DEFCON_CHANGED */
	gettext("Services are now at DEFCON %d"),
	/* OPER_DEFCON_WALL */
	gettext("%s Changed the DEFCON level to %d"),
	/* DEFCON_GLOBAL */
	gettext("The Defcon Level is now at Level: %d"),
	/* OPER_MODULE_LOADED */
	gettext("Module %s loaded"),
	/* OPER_MODULE_UNLOADED */
	gettext("Module %s unloaded"),
	/* OPER_MODULE_LOAD_FAIL */
	gettext("Unable to load module %s"),
	/* OPER_MODULE_REMOVE_FAIL */
	gettext("Unable to remove module %s"),
	/* OPER_MODULE_NO_UNLOAD */
	gettext("This module can not be unloaded."),
	/* OPER_MODULE_ALREADY_LOADED */
	gettext("Module %s is already loaded."),
	/* OPER_MODULE_ISNT_LOADED */
	gettext("Module %s isn't loaded."),
	/* OPER_MODULE_LOAD_SYNTAX */
	gettext("MODLOAD FileName"),
	/* OPER_MODULE_UNLOAD_SYNTAX */
	gettext("MODUNLOAD FileName"),
	/* OPER_MODULE_LIST_HEADER */
	gettext("Current Module list:"),
	/* OPER_MODULE_LIST */
	gettext("Module: %s [%s] [%s]"),
	/* OPER_MODULE_LIST_FOOTER */
	gettext("%d Modules loaded."),
	/* OPER_MODULE_INFO_LIST */
	gettext("Module: %s Version: %s Author: %s loaded: %s"),
	/* OPER_MODULE_CMD_LIST */
	gettext("Providing command: %R%s %s"),
	/* OPER_MODULE_MSG_LIST */
	gettext("Providing IRCD handler for: %s"),
	/* OPER_MODULE_NO_LIST */
	gettext("No modules currently loaded"),
	/* OPER_MODULE_NO_INFO */
	gettext("No information about module %s is available"),
	/* OPER_MODULE_INFO_SYNTAX */
	gettext("MODINFO FileName"),
	/* MODULE_HELP_HEADER */
	gettext("The following commands have been loaded by a module:"),
	/* OPER_EXCEPTION_SYNTAX */
	gettext("EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [params]"),
	/* OPER_EXCEPTION_ADD_SYNTAX */
	gettext("EXCEPTION ADD [+expiry] mask limit reason"),
	/* OPER_EXCEPTION_DEL_SYNTAX */
	gettext("EXCEPTION DEL {mask | list}"),
	/* OPER_EXCEPTION_MOVE_SYNTAX */
	gettext("EXCEPTION MOVE num position"),
	/* OPER_EXCEPTION_DISABLED */
	gettext("Session limiting is disabled."),
	/* OPER_EXCEPTION_ALREADY_PRESENT */
	gettext("Mask %s already present on exception list."),
	/* OPER_EXCEPTION_TOO_MANY */
	gettext("Session-limit exception list is full!"),
	/* OPER_EXCEPTION_ADDED */
	gettext("Session limit for %s set to %d."),
	/* OPER_EXCEPTION_MOVED */
	gettext("Exception for %s (#%d) moved to position %d."),
	/* OPER_EXCEPTION_NO_SUCH_ENTRY */
	gettext("No such entry (#%d) session-limit exception list."),
	/* OPER_EXCEPTION_NOT_FOUND */
	gettext("%s not found on session-limit exception list."),
	/* OPER_EXCEPTION_NO_MATCH */
	gettext("No matching entries on session-limit exception list."),
	/* OPER_EXCEPTION_DELETED */
	gettext("%s deleted from session-limit exception list."),
	/* OPER_EXCEPTION_DELETED_ONE */
	gettext("Deleted 1 entry from session-limit exception list."),
	/* OPER_EXCEPTION_DELETED_SEVERAL */
	gettext("Deleted %d entries from session-limit exception list."),
	/* OPER_EXCEPTION_LIST_HEADER */
	gettext("Current Session Limit Exception list:"),
	/* OPER_EXCEPTION_LIST_FORMAT */
	gettext("%3d  %4d   %s"),
	/* OPER_EXCEPTION_LIST_COLHEAD */
	gettext("Num  Limit  Host"),
	/* OPER_EXCEPTION_VIEW_FORMAT */
	gettext("%3d.  %s  (by %s on %s; %s)\n"
	"    Limit: %-4d  - %s"),
	/* OPER_EXCEPTION_INVALID_LIMIT */
	gettext("Invalid session limit. It must be a valid integer greater than or equal to zero and less than %d."),
	/* OPER_EXCEPTION_INVALID_HOSTMASK */
	gettext("Invalid hostmask. Only real hostmasks are valid as exceptions are not matched against nicks or usernames."),
	/* OPER_EXCEPTION_EXISTS */
	gettext("%s already exists on the EXCEPTION list."),
	/* OPER_EXCEPTION_CHANGED */
	gettext("Exception for %s has been updated to %d."),
	/* OPER_SESSION_SYNTAX */
	gettext("SESSION {LIST limit | VIEW host}"),
	/* OPER_SESSION_LIST_SYNTAX */
	gettext("SESSION LIST limit"),
	/* OPER_SESSION_VIEW_SYNTAX */
	gettext("SESSION VIEW host"),
	/* OPER_SESSION_INVALID_THRESHOLD */
	gettext("Invalid threshold value. It must be a valid integer greater than 1."),
	/* OPER_SESSION_NOT_FOUND */
	gettext("%s not found on session list."),
	/* OPER_SESSION_LIST_HEADER */
	gettext("Hosts with at least %d sessions:"),
	/* OPER_SESSION_LIST_COLHEAD */
	gettext("Sessions  Host"),
	/* OPER_SESSION_LIST_FORMAT */
	gettext("%6d    %s"),
	/* OPER_SESSION_VIEW_FORMAT */
	gettext("The host %s currently has %d sessions with a limit of %d."),
	/* OPER_HELP_EXCEPTION */
	gettext("Syntax: EXCEPTION ADD [+expiry] mask limit reason\n"
	"        EXCEPTION DEL {mask | list}\n"
	"        EXCEPTION MOVE num position\n"
	"        EXCEPTION LIST [mask | list]\n"
	"        EXCEPTION VIEW [mask | list]\n"
	" \n"
	"Allows Services Operators to manipulate the list of hosts that\n"
	"have specific session limits - allowing certain machines, \n"
	"such as shell servers, to carry more than the default number\n"
	"of clients at a time. Once a host reaches it's session limit,\n"
	"all clients attempting to connect from that host will be\n"
	"killed. Before the user is killed, they are notified, via a\n"
	"/NOTICE from %S, of a source of help regarding session\n"
	"limiting. The content of this notice is a config setting.\n"
	" \n"
	"EXCEPTION ADD adds the given host mask to the exception list.\n"
	"Note that nick!user@host and user@host masks are invalid!\n"
	"Only real host masks, such as box.host.dom and *.host.dom,\n"
	"are allowed because sessions limiting does not take nick or\n"
	"user names into account. limit must be a number greater than\n"
	"or equal to zero. This determines how many sessions this host\n"
	"may carry at a time. A value of zero means the host has an\n"
	"unlimited session limit. See the AKILL help for details about\n"
	"the format of the optional expiry parameter.\n"
	"EXCEPTION DEL removes the given mask from the exception list.\n"
	"EXCEPTION MOVE moves exception num to position. The\n"
	"exceptions inbetween will be shifted up or down to fill the gap.\n"
	"EXCEPTION LIST and EXCEPTION VIEW show all current\n"
	"exceptions; if the optional mask is given, the list is limited\n"
	"to those exceptions matching the mask. The difference is that\n"
	"EXCEPTION VIEW is more verbose, displaying the name of the\n"
	"person who added the exception, it's session limit, reason, \n"
	"host mask and the expiry date and time.\n"
	" \n"
	"Note that a connecting client will \"use\" the first exception\n"
	"their host matches. Large exception lists and widely matching\n"
	"exception masks are likely to degrade services' performance."),
	/* OPER_HELP_SESSION */
	gettext("Syntax: SESSION LIST threshold\n"
	"        SESSION VIEW host\n"
	" \n"
	"Allows Services Operators to view the session list.\n"
	"SESSION LIST lists hosts with at least threshold sessions.\n"
	"The threshold must be a number greater than 1. This is to \n"
	"prevent accidental listing of the large number of single \n"
	"session hosts.\n"
	"SESSION VIEW displays detailed information about a specific\n"
	"host - including the current session count and session limit.\n"
	"The host value may not include wildcards.\n"
	"See the EXCEPTION help for more information about session\n"
	"limiting and how to set session limits specific to certain\n"
	"hosts and groups thereof."),
	/* OPER_HELP_STAFF */
	gettext("Syntax: STAFF\n"
	"Displays all Services Staff nicks along with level\n"
	"and on-line status."),
	/* OPER_HELP_DEFCON */
	gettext("Syntax: DEFCON [1|2|3|4|5]\n"
	"The defcon system can be used to implement a pre-defined\n"
	"set of restrictions to services useful during an attempted\n"
	"attack on the network."),
	/* OPER_HELP_DEFCON_NO_NEW_CHANNELS */
	gettext("* No new channel registrations"),
	/* OPER_HELP_DEFCON_NO_NEW_NICKS */
	gettext("* No new nick registrations"),
	/* OPER_HELP_DEFCON_NO_MLOCK_CHANGE */
	gettext("* No MLOCK changes"),
	/* OPER_HELP_DEFCON_FORCE_CHAN_MODES */
	gettext("* Force Chan Modes (%s) to be set on all channels"),
	/* OPER_HELP_DEFCON_REDUCE_SESSION */
	gettext("* Use the reduced session limit of %d"),
	/* OPER_HELP_DEFCON_NO_NEW_CLIENTS */
	gettext("* Kill any NEW clients connecting"),
	/* OPER_HELP_DEFCON_OPER_ONLY */
	gettext("* Ignore any non-opers with message"),
	/* OPER_HELP_DEFCON_SILENT_OPER_ONLY */
	gettext("* Silently ignore non-opers"),
	/* OPER_HELP_DEFCON_AKILL_NEW_CLIENTS */
	gettext("* AKILL any new clients connecting"),
	/* OPER_HELP_DEFCON_NO_NEW_MEMOS */
	gettext("* No new memos sent"),
	/* OPER_HELP_CHANKILL */
	gettext("Syntax: CHANKILL [+expiry] channel reason\n"
	"Puts an AKILL for every nick on the specified channel. It\n"
	"uses the entire and complete real ident@host for every nick,\n"
	"then enforces the AKILL."),
	/* NEWS_LOGON_TEXT */
	gettext("[Logon News - %s] %s"),
	/* NEWS_OPER_TEXT */
	gettext("[Oper News - %s] %s"),
	/* NEWS_RANDOM_TEXT */
	gettext("[Random News - %s] %s"),
	/* NEWS_LOGON_SYNTAX */
	gettext("LOGONNEWS {ADD|DEL|LIST} [text|num]"),
	/* NEWS_LOGON_LIST_HEADER */
	gettext("Logon news items:"),
	/* NEWS_LIST_ENTRY */
	gettext("%5d (%s by %s)\n"
	"    %s"),
	/* NEWS_LOGON_LIST_NONE */
	gettext("There is no logon news."),
	/* NEWS_LOGON_ADD_SYNTAX */
	gettext("Syntax: LOGONNEWS ADD text"),
	/* NEWS_ADD_FULL */
	gettext("News list is full!"),
	/* NEWS_LOGON_ADDED */
	gettext("Added new logon news item (#%d)."),
	/* NEWS_LOGON_DEL_SYNTAX */
	gettext("Syntax: LOGONNEWS DEL {num | ALL}"),
	/* NEWS_LOGON_DEL_NOT_FOUND */
	gettext("Logon news item #%d not found!"),
	/* NEWS_LOGON_DELETED */
	gettext("Logon news item #%d deleted."),
	/* NEWS_LOGON_DEL_NONE */
	gettext("No logon news items to delete!"),
	/* NEWS_LOGON_DELETED_ALL */
	gettext("All logon news items deleted."),
	/* NEWS_OPER_SYNTAX */
	gettext("OPERNEWS {ADD|DEL|LIST} [text|num]"),
	/* NEWS_OPER_LIST_HEADER */
	gettext("Oper news items:"),
	/* NEWS_OPER_LIST_NONE */
	gettext("There is no oper news."),
	/* NEWS_OPER_ADD_SYNTAX */
	gettext("Syntax: OPERNEWS ADD text"),
	/* NEWS_OPER_ADDED */
	gettext("Added new oper news item (#%d)."),
	/* NEWS_OPER_DEL_SYNTAX */
	gettext("Syntax: OPERNEWS DEL {num | ALL}"),
	/* NEWS_OPER_DEL_NOT_FOUND */
	gettext("Oper news item #%d not found!"),
	/* NEWS_OPER_DELETED */
	gettext("Oper news item #%d deleted."),
	/* NEWS_OPER_DEL_NONE */
	gettext("No oper news items to delete!"),
	/* NEWS_OPER_DELETED_ALL */
	gettext("All oper news items deleted."),
	/* NEWS_RANDOM_SYNTAX */
	gettext("RANDOMNEWS {ADD|DEL|LIST} [text|num]"),
	/* NEWS_RANDOM_LIST_HEADER */
	gettext("Random news items:"),
	/* NEWS_RANDOM_LIST_NONE */
	gettext("There is no random news."),
	/* NEWS_RANDOM_ADD_SYNTAX */
	gettext("Syntax: RANDOMNEWS ADD text"),
	/* NEWS_RANDOM_ADDED */
	gettext("Added new random news item (#%d)."),
	/* NEWS_RANDOM_DEL_SYNTAX */
	gettext("Syntax: RANDOMNEWS DEL {num | ALL}"),
	/* NEWS_RANDOM_DEL_NOT_FOUND */
	gettext("Random news item #%d not found!"),
	/* NEWS_RANDOM_DELETED */
	gettext("Random news item #%d deleted."),
	/* NEWS_RANDOM_DEL_NONE */
	gettext("No random news items to delete!"),
	/* NEWS_RANDOM_DELETED_ALL */
	gettext("All random news items deleted."),
	/* NEWS_HELP_LOGON */
	gettext("Syntax: LOGONNEWS ADD text\n"
	"        LOGONNEWS DEL {num | ALL}\n"
	"        LOGONNEWS LIST\n"
	" \n"
	"Edits or displays the list of logon news messages.  When a\n"
	"user connects to the network, these messages will be sent\n"
	"to them.  (However, no more than %d messages will be\n"
	"sent in order to avoid flooding the user.  If there are\n"
	"more news messages, only the most recent will be sent.)\n"
	"NewsCount can be configured in services.conf.\n"
	" \n"
	"LOGONNEWS may only be used by Services Operators."),
	/* NEWS_HELP_OPER */
	gettext("Syntax: OPERNEWS ADD text\n"
	"        OPERNEWS DEL {num | ALL}\n"
	"        OPERNEWS LIST\n"
	" \n"
	"Edits or displays the list of oper news messages.  When a\n"
	"user opers up (with the /OPER command), these messages will\n"
	"be sent to them.  (However, no more than %d messages will\n"
	"be sent in order to avoid flooding the user.  If there are\n"
	"more news messages, only the most recent will be sent.)\n"
	"NewsCount can be configured in services.conf.\n"
	" \n"
	"OPERNEWS may only be used by Services Operators."),
	/* NEWS_HELP_RANDOM */
	gettext("Syntax: RANDOMNEWS ADD text\n"
	"        RANDOMNEWS DEL {num | ALL}\n"
	"        RANDOMNEWS LIST\n"
	" \n"
	"Edits or displays the list of random news messages.  When a\n"
	"user connects to the network, one (and only one) of the\n"
	"random news will be randomly chosen and sent to them.\n"
	" \n"
	"RANDOMNEWS may only be used by Services Operators."),
	/* NICK_HELP_CMD_CONFIRM */
	gettext("    CONFIRM    Confirm a nickserv auth code"),
	/* NICK_HELP_CMD_RESEND */
	gettext("    RESEND     Resend a nickserv auth code"),
	/* NICK_HELP_CMD_REGISTER */
	gettext("    REGISTER   Register a nickname"),
	/* NICK_HELP_CMD_GROUP */
	gettext("    GROUP      Join a group"),
	/* NICK_HELP_CMD_UNGROUP */
	gettext("    UNGROUP    Remove a nick from a group"),
	/* NICK_HELP_CMD_IDENTIFY */
	gettext("    IDENTIFY   Identify yourself with your password"),
	/* NICK_HELP_CMD_ACCESS */
	gettext("    ACCESS     Modify the list of authorized addresses"),
	/* NICK_HELP_CMD_SET */
	gettext("    SET        Set options, including kill protection"),
	/* NICK_HELP_CMD_SASET */
	gettext("    SASET      Set SET-options on another nickname"),
	/* NICK_HELP_CMD_DROP */
	gettext("    DROP       Cancel the registration of a nickname"),
	/* NICK_HELP_CMD_RECOVER */
	gettext("    RECOVER    Kill another user who has taken your nick"),
	/* NICK_HELP_CMD_RELEASE */
	gettext("    RELEASE    Regain custody of your nick after RECOVER"),
	/* NICK_HELP_CMD_SENDPASS */
	gettext("    SENDPASS   Forgot your password? Try this"),
	/* NICK_HELP_CMD_RESETPASS */
	gettext("    RESETPASS  Helps you reset lost passwords"),
	/* NICK_HELP_CMD_GHOST */
	gettext("    GHOST      Disconnects a \"ghost\" IRC session using your nick"),
	/* NICK_HELP_CMD_ALIST */
	gettext("    ALIST      List channels you have access on"),
	/* NICK_HELP_CMD_GLIST */
	gettext("    GLIST      Lists all nicknames in your group"),
	/* NICK_HELP_CMD_INFO */
	gettext("    INFO       Displays information about a given nickname"),
	/* NICK_HELP_CMD_LIST */
	gettext("    LIST       List all registered nicknames that match a given pattern"),
	/* NICK_HELP_CMD_LOGOUT */
	gettext("    LOGOUT     Reverses the effect of the IDENTIFY command"),
	/* NICK_HELP_CMD_STATUS */
	gettext("    STATUS     Returns the owner status of the given nickname"),
	/* NICK_HELP_CMD_UPDATE */
	gettext("    UPDATE     Updates your current status, i.e. it checks for new memos"),
	/* NICK_HELP_CMD_GETPASS */
	gettext("    GETPASS    Retrieve the password for a nickname"),
	/* NICK_HELP_CMD_GETEMAIL */
	gettext("    GETEMAIL   Matches and returns all users that registered using given email"),
	/* NICK_HELP_CMD_FORBID */
	gettext("    FORBID     Prevents a nickname from being registered"),
	/* NICK_HELP_CMD_SUSPEND */
	gettext("    SUSPEND    Suspend a given nick"),
	/* NICK_HELP_CMD_UNSUSPEND */
	gettext("    UNSUSPEND  Unsuspend a given nick"),
	/* NICK_HELP */
	gettext("%S allows you to \"register\" a nickname and\n"
	"prevent others from using it. The following\n"
	"commands allow for registration and maintenance of\n"
	"nicknames; to use them, type %R%S command.\n"
	"For more information on a specific command, type\n"
	"%R%S HELP command."),
	/* NICK_HELP_FOOTER */
	gettext(" \n"
	"NOTICE: This service is intended to provide a way for\n"
	"IRC users to ensure their identity is not compromised.\n"
	"It is NOT intended to facilitate \"stealing\" of\n"
	"nicknames or other malicious actions.  Abuse of %S\n"
	"will result in, at minimum, loss of the abused\n"
	"nickname(s)."),
	/* NICK_HELP_EXPIRES */
	gettext("Nicknames that are not used anymore are subject to \n"
	"the automatic expiration, i.e. they will be deleted\n"
	"after %d days if not used."),
	/* NICK_HELP_REGISTER */
	gettext("Syntax: REGISTER password [email]\n"
	" \n"
	"Registers your nickname in the %S database.  Once\n"
	"your nick is registered, you can use the SET and ACCESS\n"
	"commands to configure your nick's settings as you like\n"
	"them.  Make sure you remember the password you use when\n"
	"registering - you'll need it to make changes to your nick\n"
	"later.  (Note that case matters!  ANOPE, Anope, and \n"
	"anope are all different passwords!)\n"
	" \n"
	"Guidelines on choosing passwords:\n"
	" \n"
	"Passwords should not be easily guessable.  For example,\n"
	"using your real name as a password is a bad idea.  Using\n"
	"your nickname as a password is a much worse idea ;) and,\n"
	"in fact, %S will not allow it.  Also, short\n"
	"passwords are vulnerable to trial-and-error searches, so\n"
	"you should choose a password at least 5 characters long.\n"
	"Finally, the space character cannot be used in passwords.\n"
	" \n"
	"The parameter email is optional and will set the email\n"
	"for your nick immediately. However, it may be required\n"
	"on certain networks.\n"
	"Your privacy is respected; this e-mail won't be given to\n"
	"any third-party person.\n"
	" \n"
	"This command also creates a new group for your nickname,\n"
	"that will allow you to register other nicks later sharing\n"
	"the same configuration, the same set of memos and the\n"
	"same channel privileges. For more information on this\n"
	"feature, type %R%S HELP GROUP."),
	/* NICK_HELP_GROUP */
	gettext("Syntax: GROUP target password\n"
	" \n"
	"This command makes your nickname join the target nickname's \n"
	"group. password is the password of the target nickname.\n"
	" \n"
	"Joining a group will allow you to share your configuration,\n"
	"memos, and channel privileges with all the nicknames in the\n"
	"group, and much more!\n"
	" \n"
	"A group exists as long as it is useful. This means that even\n"
	"if a nick of the group is dropped, you won't lose the\n"
	"shared things described above, as long as there is at\n"
	"least one nick remaining in the group.\n"
	" \n"
	"You can use this command even if you have not registered\n"
	"your nick yet. If your nick is already registered, you'll\n"
	"need to identify yourself before using this command. Type\n"
	"%R%S HELP IDENTIFY for more information. This\n"
	"last may be not possible on your IRC network.\n"
	" \n"
	"It is recommended to use this command with a non-registered\n"
	"nick because it will be registered automatically when \n"
	"using this command. You may use it with a registered nick (to \n"
	"change your group) only if your network administrators allowed \n"
	"it.\n"
	" \n"
	"You can only be in one group at a time. Group merging is\n"
	"not possible.\n"
	" \n"
	"Note: all the nicknames of a group have the same password."),
	/* NICK_HELP_UNGROUP */
	gettext("Syntax: UNGROUP [nick]\n"
	" \n"
	"This command ungroups your nick, or if given, the specificed nick,\n"
	"from the group it is in. The ungrouped nick keeps its registration\n"
	"time, password, email, greet, language, url, and icq. Everything\n"
	"else is reset. You may not ungroup yourself if there is only one\n"
	"nick in your group."),
	/* NICK_HELP_IDENTIFY */
	gettext("Syntax: IDENTIFY [account] password\n"
	" \n"
	"Tells %S that you are really the owner of this\n"
	"nick.  Many commands require you to authenticate yourself\n"
	"with this command before you use them.  The password\n"
	"should be the same one you sent with the REGISTER\n"
	"command."),
	/* NICK_HELP_UPDATE */
	gettext("Syntax: UPDATE\n"
	"Updates your current status, i.e. it checks for new memos,\n"
	"sets needed chanmodes (ModeonID) and updates your vhost and\n"
	"your userflags (lastseentime, etc)."),
	/* NICK_HELP_LOGOUT */
	gettext("Syntax: LOGOUT\n"
	" \n"
	"This reverses the effect of the IDENTIFY command, i.e.\n"
	"make you not recognized as the real owner of the nick\n"
	"anymore. Note, however, that you won't be asked to reidentify\n"
	"yourself."),
	/* NICK_HELP_DROP */
	gettext("Syntax: DROP [nickname]\n"
	" \n"
	"Drops your nickname from the %S database.  A nick\n"
	"that has been dropped is free for anyone to re-register.\n"
	" \n"
	"You may drop a nick within your group by passing it\n"
	"as the nick parameter.\n"
	" \n"
	"In order to use this command, you must first identify\n"
	"with your password (%R%S HELP IDENTIFY for more\n"
	"information)."),
	/* NICK_HELP_ACCESS */
	gettext("Syntax: ACCESS ADD mask\n"
	"        ACCESS DEL mask\n"
	"        ACCESS LIST\n"
	" \n"
	"Modifies or displays the access list for your nick.  This\n"
	"is the list of addresses which will be automatically\n"
	"recognized by %S as allowed to use the nick.  If\n"
	"you want to use the nick from a different address, you\n"
	"need to send an IDENTIFY command to make %S\n"
	"recognize you.\n"
	" \n"
	"Examples:\n"
	" \n"
	"    ACCESS ADD anyone@*.bepeg.com\n"
	"        Allows access to user anyone from any machine in\n"
	"        the bepeg.com domain.\n"
	" \n"
	"    ACCESS DEL anyone@*.bepeg.com\n"
	"        Reverses the previous command.\n"
	" \n"
	"    ACCESS LIST\n"
	"        Displays the current access list."),
	/* NICK_HELP_SET_HEAD */
	gettext("Syntax: SET option parameters\n"
	" \n"
	"Sets various nickname options.  option can be one of:"),
	/* NICK_HELP_CMD_SET_DISPLAY */
	gettext("    DISPLAY    Set the display of your group in Services"),
	/* NICK_HELP_CMD_SET_PASSWORD */
	gettext("    PASSWORD   Set your nickname password"),
	/* NICK_HELP_CMD_SET_LANGUAGE */
	gettext("    LANGUAGE   Set the language Services will use when\n"
	"                   sending messages to you"),
	/* NICK_HELP_CMD_SET_EMAIL */
	gettext("    EMAIL      Associate an E-mail address with your nickname"),
	/* NICK_HELP_CMD_SET_GREET */
	gettext("    GREET      Associate a greet message with your nickname"),
	/* NICK_HELP_CMD_SET_KILL */
	gettext("    KILL       Turn protection on or off"),
	/* NICK_HELP_CMD_SET_SECURE */
	gettext("    SECURE     Turn nickname security on or off"),
	/* NICK_HELP_CMD_SET_PRIVATE */
	gettext("    PRIVATE    Prevent your nickname from appearing in a\n"
	"                   %R%S LIST"),
	/* NICK_HELP_CMD_SET_HIDE */
	gettext("    HIDE       Hide certain pieces of nickname information"),
	/* NICK_HELP_CMD_SET_MSG */
	gettext("    MSG        Change the communication method of Services"),
	/* NICK_HELP_CMD_SET_AUTOOP */
	gettext("    AUTOOP     Should services op you automatically.    "),
	/* NICK_HELP_SET_TAIL */
	gettext("In order to use this command, you must first identify\n"
	"with your password (%R%S HELP IDENTIFY for more\n"
	"information).\n"
	" \n"
	"Type %R%S HELP SET option for more information\n"
	"on a specific option."),
	/* NICK_HELP_SET_DISPLAY */
	gettext("Syntax: SET DISPLAY new-display\n"
	" \n"
	"Changes the display used to refer to your nickname group in \n"
	"Services. The new display MUST be a nick of your group."),
	/* NICK_HELP_SET_PASSWORD */
	gettext("Syntax: SET PASSWORD new-password\n"
	" \n"
	"Changes the password used to identify you as the nick's\n"
	"owner."),
	/* NICK_HELP_SET_LANGUAGE */
	gettext("Syntax: SET LANGUAGE language\n"
	" \n"
	"Changes the language Services uses when sending messages to\n"
	"you (for example, when responding to a command you send).\n"
	"language should be chosen from the following list of\n"
	"supported languages:"),
	/* NICK_HELP_SET_EMAIL */
	gettext("Syntax: SET EMAIL address\n"
	" \n"
	"Associates the given E-mail address with your nickname.\n"
	"This address will be displayed whenever someone requests\n"
	"information on the nickname with the INFO command."),
	/* NICK_HELP_SET_GREET */
	gettext("Syntax: SET GREET message\n"
	" \n"
	"Makes the given message the greet of your nickname, that\n"
	"will be displayed when joining a channel that has GREET\n"
	"option enabled, provided that you have the necessary \n"
	"access on it."),
	/* NICK_HELP_SET_KILL */
	gettext("Syntax: SET KILL {ON | QUICK | IMMED | OFF}\n"
	" \n"
	"Turns the automatic protection option for your nick\n"
	"on or off.  With protection on, if another user\n"
	"tries to take your nick, they will be given one minute to\n"
	"change to another nick, after which %S will forcibly change\n"
	"their nick.\n"
	" \n"
	"If you select QUICK, the user will be given only 20 seconds\n"
	"to change nicks instead of the usual 60.  If you select\n"
	"IMMED, user's nick will be changed immediately without being\n"
	"warned first or given a chance to change their nick; please\n"
	"do not use this option unless necessary.  Also, your\n"
	"network's administrators may have disabled this option."),
	/* NICK_HELP_SET_SECURE */
	gettext("Syntax: SET SECURE {ON | OFF}\n"
	" \n"
	"Turns %S's security features on or off for your\n"
	"nick.  With SECURE set, you must enter your password\n"
	"before you will be recognized as the owner of the nick,\n"
	"regardless of whether your address is on the access\n"
	"list.  However, if you are on the access list, %S\n"
	"will not auto-kill you regardless of the setting of the\n"
	"KILL option."),
	/* NICK_HELP_SET_PRIVATE */
	gettext("Syntax: SET PRIVATE {ON | OFF}\n"
	" \n"
	"Turns %S's privacy option on or off for your nick.\n"
	"With PRIVATE set, your nickname will not appear in\n"
	"nickname lists generated with %S's LIST command.\n"
	"(However, anyone who knows your nickname can still get\n"
	"information on it using the INFO command.)"),
	/* NICK_HELP_SET_HIDE */
	gettext("Syntax: SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}\n"
	" \n"
	"Allows you to prevent certain pieces of information from\n"
	"being displayed when someone does a %S INFO on your\n"
	"nick.  You can hide your E-mail address (EMAIL), last seen\n"
	"user@host mask (USERMASK), your services access status\n"
	"(STATUS) and  last quit message (QUIT).\n"
	"The second parameter specifies whether the information should\n"
	"be displayed (OFF) or hidden (ON)."),
	/* NICK_HELP_SET_MSG */
	gettext("Syntax: SET MSG {ON | OFF}\n"
	" \n"
	"Allows you to choose the way Services are communicating with \n"
	"you. With MSG set, Services will use messages, else they'll \n"
	"use notices."),
	/* NICK_HELP_SET_AUTOOP */
	gettext("Syntax: SET AUTOOP {ON | OFF}\n"
	" \n"
	"Sets whether you will be opped automatically. Set to ON to \n"
	"allow ChanServ to op you automatically when entering channels."),
	/* NICK_HELP_SASET_HEAD */
	gettext("Syntax: SASET nickname option parameters.\n"
	" \n"
	"Sets various nickname options.  option can be one of:"),
	/* NICK_HELP_CMD_SASET_DISPLAY */
	gettext("    DISPLAY    Set the display of the group in Services"),
	/* NICK_HELP_CMD_SASET_PASSWORD */
	gettext("    PASSWORD   Set the nickname password"),
	/* NICK_HELP_CMD_SASET_EMAIL */
	gettext("    EMAIL      Associate an E-mail address with the nickname"),
	/* NICK_HELP_CMD_SASET_GREET */
	gettext("    GREET      Associate a greet message with the nickname"),
	/* NICK_HELP_CMD_SASET_PRIVATE */
	gettext("    PRIVATE    Prevent the nickname from appearing in a\n"
	"                   %R%S LIST"),
	/* NICK_HELP_CMD_SASET_NOEXPIRE */
	gettext("    NOEXPIRE   Prevent the nickname from expiring"),
	/* NICK_HELP_CMD_SASET_AUTOOP */
	gettext("    AUTOOP     Turn autoop on or off"),
	/* NICK_HELP_CMD_SASET_LANGUAGE */
	gettext("    LANGUAGE   Set the language Services will use when\n"
	"               sending messages to nickname"),
	/* NICK_HELP_SASET_TAIL */
	gettext("Type %R%S HELP SASET option for more information\n"
	"on a specific option. The options will be set on the given\n"
	"nickname."),
	/* NICK_HELP_SASET_DISPLAY */
	gettext("Syntax: SASET nickname DISPLAY new-display\n"
	" \n"
	"Changes the display used to refer to the nickname group in \n"
	"Services. The new display MUST be a nick of the group."),
	/* NICK_HELP_SASET_PASSWORD */
	gettext("Syntax: SASET nickname PASSWORD new-password\n"
	" \n"
	"Changes the password used to identify as the nick's	owner."),
	/* NICK_HELP_SASET_EMAIL */
	gettext("Syntax: SASET nickname EMAIL address\n"
	" \n"
	"Associates the given E-mail address with the nickname."),
	/* NICK_HELP_SASET_GREET */
	gettext("Syntax: SASET nickname GREET message\n"
	" \n"
	"Makes the given message the greet of the nickname, that\n"
	"will be displayed when joining a channel that has GREET\n"
	"option enabled, provided that the user has the necessary \n"
	"access on it."),
	/* NICK_HELP_SASET_KILL */
	gettext("Syntax: SASET nickname KILL {ON | QUICK | IMMED | OFF}\n"
	" \n"
	"Turns the automatic protection option for the nick\n"
	"on or off.  With protection on, if another user\n"
	"tries to take the nick, they will be given one minute to\n"
	"change to another nick, after which %S will forcibly change\n"
	"their nick.\n"
	" \n"
	"If you select QUICK, the user will be given only 20 seconds\n"
	"to change nicks instead of the usual 60.  If you select\n"
	"IMMED, user's nick will be changed immediately without being\n"
	"warned first or given a chance to change their nick; please\n"
	"do not use this option unless necessary.  Also, your\n"
	"network's administrators may have disabled this option."),
	/* NICK_HELP_SASET_SECURE */
	gettext("Syntax: SASET nickname SECURE {ON | OFF}\n"
	" \n"
	"Turns %S's security features on or off for your\n"
	"nick.  With SECURE set, you must enter your password\n"
	"before you will be recognized as the owner of the nick,\n"
	"regardless of whether your address is on the access\n"
	"list.  However, if you are on the access list, %S\n"
	"will not auto-kill you regardless of the setting of the\n"
	"KILL option."),
	/* NICK_HELP_SASET_PRIVATE */
	gettext("Syntax: SASET nickname PRIVATE {ON | OFF}\n"
	" \n"
	"Turns %S's privacy option on or off for the nick.\n"
	"With PRIVATE set, the nickname will not appear in\n"
	"nickname lists generated with %S's LIST command.\n"
	"(However, anyone who knows the nickname can still get\n"
	"information on it using the INFO command.)"),
	/* NICK_HELP_SASET_HIDE */
	gettext("Syntax: SASET nickname HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}\n"
	" \n"
	"Allows you to prevent certain pieces of information from\n"
	"being displayed when someone does a %S INFO on the\n"
	"nick.  You can hide the E-mail address (EMAIL), last seen\n"
	"user@host mask (USERMASK), the services access status\n"
	"(STATUS) and  last quit message (QUIT).\n"
	"The second parameter specifies whether the information should\n"
	"be displayed (OFF) or hidden (ON)."),
	/* NICK_HELP_SASET_MSG */
	gettext("Syntax: SASET nickname MSG {ON | OFF}\n"
	" \n"
	"Allows you to choose the way Services are communicating with \n"
	"the given user. With MSG set, Services will use messages,\n"
	"else they'll use notices."),
	/* NICK_HELP_SASET_NOEXPIRE */
	gettext("Syntax: SASET nickname NOEXPIRE {ON | OFF}\n"
	" \n"
	"Sets whether the given nickname will expire.  Setting this\n"
	"to ON prevents the nickname from expiring."),
	/* NICK_HELP_SASET_AUTOOP */
	gettext("Syntax: SASET nickname AUTOOP {ON | OFF}\n"
	" \n"
	"Sets whether the given nickname will be opped automatically.\n"
	"Set to ON to allow ChanServ to op the given nickname \n"
	"automatically when joining channels."),
	/* NICK_HELP_SASET_LANGUAGE */
	gettext("Syntax: SASET nickname LANGUAGE language\n"
	" \n"
	"Changes the language Services uses when sending messages to\n"
	"nickname (for example, when responding to a command he sends).\n"
	"language should be chosen from a list of supported languages\n"
	"that you can get by typing %R%S HELP SET LANGUAGE."),
	/* NICK_HELP_RECOVER */
	gettext("Syntax: RECOVER nickname [password]\n"
	" \n"
	"Allows you to recover your nickname if someone else has\n"
	"taken it; this does the same thing that %S does\n"
	"automatically if someone tries to use a kill-protected\n"
	"nick.\n"
	" \n"
	"When you give this command, %S will bring a fake\n"
	"user online with the same nickname as the user you're\n"
	"trying to recover your nick from.  This causes the IRC\n"
	"servers to disconnect the other user.  This fake user will\n"
	"remain online for %s to ensure that the other\n"
	"user does not immediately reconnect; after that time, you\n"
	"can reclaim your nick.  Alternatively, use the RELEASE\n"
	"command (%R%S HELP RELEASE) to get the nick\n"
	"back sooner.\n"
	" \n"
	"In order to use the RECOVER command for a nick, your\n"
	"current address as shown in /WHOIS must be on that nick's\n"
	"access list, you must be identified and in the group of\n"
	"that nick, or you must supply the correct password for\n"
	"the nickname."),
	/* NICK_HELP_RELEASE */
	gettext("Syntax: RELEASE nickname [password]\n"
	" \n"
	"Instructs %S to remove any hold on your nickname\n"
	"caused by automatic kill protection or use of the RECOVER\n"
	"command.  This holds lasts for %s;\n"
	"this command gets rid of them sooner.\n"
	" \n"
	"In order to use the RELEASE command for a nick, your\n"
	"current address as shown in /WHOIS must be on that nick's\n"
	"access list, you must be identified and in the group of\n"
	"that nick, or you must supply the correct password for\n"
	"the nickname."),
	/* NICK_HELP_GHOST */
	gettext("Syntax: GHOST nickname [password]\n"
	" \n"
	"Terminates a \"ghost\" IRC session using your nick.  A\n"
	"\"ghost\" session is one which is not actually connected,\n"
	"but which the IRC server believes is still online for one\n"
	"reason or another.  Typically, this happens if your\n"
	"computer crashes or your Internet or modem connection\n"
	"goes down while you're on IRC.\n"
	" \n"
	"In order to use the GHOST command for a nick, your\n"
	"current address as shown in /WHOIS must be on that nick's\n"
	"access list, you must be identified and in the group of\n"
	"that nick, or you must supply the correct password for\n"
	"the nickname."),
	/* NICK_HELP_INFO */
	gettext("Syntax: INFO nickname\n"
	" \n"
	"Displays information about the given nickname, such as\n"
	"the nick's owner, last seen address and time, and nick\n"
	"options."),
	/* NICK_HELP_LIST */
	gettext("Syntax: LIST pattern\n"
	" \n"
	"Lists all registered nicknames which match the given\n"
	"pattern, in nick!user@host format.  Nicks with the\n"
	"PRIVATE option set will not be displayed.\n"
	" \n"
	"Examples:\n"
	" \n"
	"    LIST *!joeuser@foo.com\n"
	"        Lists all nicks owned by joeuser@foo.com.\n"
	" \n"
	"    LIST *Bot*!*@*\n"
	"        Lists all registered nicks with Bot in their\n"
	"        names (case insensitive).\n"
	" \n"
	"    LIST *!*@*.bar.org\n"
	"        Lists all nicks owned by users in the bar.org\n"
	"        domain."),
	/* NICK_HELP_ALIST */
	gettext("Syntax: ALIST [level]\n"
	" \n"
	"Lists all channels you have access on. Optionally, you can specify\n"
	"a level in XOP or ACCESS format. The resulting list will only\n"
	"include channels where you have the given level of access.\n"
	"Examples:\n"
	"    ALIST Founder\n"
	"        Lists all channels where you have Founder\n"
	"        access.\n"
	"    ALIST AOP\n"
	"        Lists all channels where you have AOP\n"
	"        access or greater.\n"
	"    ALIST 10\n"
	"        Lists all channels where you have level 10\n"
	"        access or greater.\n"
	"Channels that have the NOEXPIRE option set will be\n"
	"prefixed by an exclamation mark."),
	/* NICK_HELP_GLIST */
	gettext("Syntax: GLIST\n"
	" \n"
	"Lists all nicks in your group."),
	/* NICK_HELP_STATUS */
	gettext("Syntax: STATUS nickname...\n"
	" \n"
	"Returns whether the user using the given nickname is\n"
	"recognized as the owner of the nickname.  The response has\n"
	"this format:\n"
	" \n"
	"    nickname status-code account\n"
	" \n"
	"where nickname is the nickname sent with the command,\n"
	"status-code is one of the following, and account\n"
	"is the account they are logged in as.\n"
	" \n"
	"    0 - no such user online or nickname not registered\n"
	"    1 - user not recognized as nickname's owner\n"
	"    2 - user recognized as owner via access list only\n"
	"    3 - user recognized as owner via password identification\n"
	" \n"
	"Up to sixteen nicknames may be sent with each command; the\n"
	"rest will be ignored. If no nickname is given, your status\n"
	"will be returned."),
	/* NICK_HELP_SENDPASS */
	gettext("Syntax: SENDPASS nickname\n"
	" \n"
	"Send the password of the given nickname to the e-mail address\n"
	"set in the nickname record. This command is really useful\n"
	"to deal with lost passwords.\n"
	" \n"
	"May be limited to IRC operators on certain networks."),
	/* NICK_HELP_RESETPASS */
	gettext("Syntax: RESETPASS nickname\n"
	"Sends a code key to the nickname with instructions on how to\n"
	"reset their password."),
	/* NICK_HELP_CONFIRM */
	gettext("Syntax: CONFIRM passcode\n"
	" \n"
	"This is the second step of nickname registration process.\n"
	"You must perform this command in order to get your nickname\n"
	"registered with %S. The passcode (or called auth code also)\n"
	"is sent to your e-mail address in the first step of the\n"
	"registration process. For more information about the first\n"
	"stage of the registration process, type: %R%S HELP REGISTER\n"
	" \n"
	"This is also used after the RESETPASS command has been used to\n"
	"force identify you to your nick so you may change your password."),
	/* NICK_HELP_CONFIRM_OPER */
	gettext("Additionally, Services Operators with the nickserv/confirm permission can\n"
	"replace passcode with a users nick to force validate them."),
	/* NICK_HELP_RESEND */
	gettext("Syntax: RESEND\n"
	" \n"
	"This command will re-send the auth code (also called passcode)\n"
	"to the e-mail address of the user whom is performing it."),
	/* NICK_SERVADMIN_HELP */
	gettext(" \n"
	"Services Operators can also drop any nickname without needing\n"
	"to identify for the nick, and may view the access list for\n"
	"any nickname (%R%S ACCESS LIST nick)."),
	/* NICK_SERVADMIN_HELP_LOGOUT */
	gettext("Syntax: LOGOUT [nickname [REVALIDATE]]\n"
	" \n"
	"Without a parameter, reverses the effect of the IDENTIFY \n"
	"command, i.e. make you not recognized as the real owner of the nick\n"
	"anymore. Note, however, that you won't be asked to reidentify\n"
	"yourself.\n"
	" \n"
	"With a parameter, does the same for the given nick. If you \n"
	"specify REVALIDATE as well, Services will ask the given nick\n"
	"to re-identify. This use limited to Services Operators."),
	/* NICK_SERVADMIN_HELP_DROP */
	gettext("Syntax: DROP [nickname]\n"
	" \n"
	"Without a parameter, drops your nickname from the\n"
	"%S database.\n"
	" \n"
	"With a parameter, drops the named nick from the database.\n"
	"You may drop any nick within your group without any \n"
	"special privileges. Dropping any nick is limited to \n"
	"Services Operators."),
	/* NICK_SERVADMIN_HELP_LIST */
	gettext("Syntax: LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]\n"
	" \n"
	"Lists all registered nicknames which match the given\n"
	"pattern, in nick!user@host format.  Nicks with the PRIVATE\n"
	"option set will only be displayed to Services Operators.  Nicks\n"
	"with the NOEXPIRE option set will have a ! appended to\n"
	"the nickname for Services Operators.\n"
	" \n"
	"If the FORBIDDEN, SUSPENDED, NOEXPIRE or UNCONFIRMED options are given, only\n"
	"nicks which, respectively, are FORBIDDEN, SUSPENDED, UNCONFIRMED or have the\n"
	"NOEXPIRE flag set will be displayed. If multiple options are\n"
	"given, all nicks matching at least one option will be displayed.\n"
	"These options are limited to Services Operators.  \n"
	"Examples:\n"
	" \n"
	"    LIST *!joeuser@foo.com\n"
	"        Lists all registered nicks owned by joeuser@foo.com.\n"
	" \n"
	"    LIST *Bot*!*@*\n"
	"        Lists all registered nicks with Bot in their\n"
	"        names (case insensitive).\n"
	" \n"
	"    LIST * NOEXPIRE\n"
	"        Lists all registered nicks which have been set to\n"
	"        not expire."),
	/* NICK_SERVADMIN_HELP_ALIST */
	gettext("Syntax: ALIST [nickname] [level]\n"
	" \n"
	"With no parameters, lists channels you have access on. With\n"
	"one parameter, lists channels that nickname has access \n"
	"on. With two parameters lists channels that nickname has \n"
	"level access or greater on.\n"
	"This use limited to Services Operators."),
	/* NICK_SERVADMIN_HELP_GLIST */
	gettext("Syntax: GLIST [nickname]\n"
	" \n"
	"Without a parameter, lists all nicknames that are in\n"
	"your group.\n"
	" \n"
	"With a parameter, lists all nicknames that are in the\n"
	"group of the given nick.\n"
	"This use limited to Services Operators."),
	/* NICK_SERVADMIN_HELP_GETPASS */
	gettext("Syntax: GETPASS nickname\n"
	" \n"
	"Returns the password for the given nickname.  Note that\n"
	"whenever this command is used, a message including the\n"
	"person who issued the command and the nickname it was used\n"
	"on will be logged and sent out as a WALLOPS/GLOBOPS.\n"
	" \n"
	"This command is unavailable when encryption is enabled."),
	/* NICK_SERVADMIN_HELP_GETEMAIL */
	gettext("Syntax: GETEMAIL user@emailhost\n"
	"Returns the matching nicks that used given email. Note that\n"
	"you can not use wildcards for either user or emailhost. Whenever\n"
	"this command is used, a message including the person who issued\n"
	"the command and the email it was used on will be logged."),
	/* NICK_SERVADMIN_HELP_FORBID */
	gettext("Syntax: FORBID nickname [reason]\n"
	" \n"
	"Disallows a nickname from being registered or used by\n"
	"anyone.  May be cancelled by dropping the nick.\n"
	" \n"
	"On certain networks, reason is required."),
	/* NICK_SERVADMIN_HELP_SUSPEND */
	gettext("Syntax: SUSPEND nickname reason\n"
	"SUSPENDs a nickname from being used."),
	/* NICK_SERVADMIN_HELP_UNSUSPEND */
	gettext("Syntax: UNSUSPEND nickname\n"
	"UNSUSPENDS a nickname from being used."),
	/* CHAN_HELP_CMD_GETPASS */
	gettext("    GETPASS    Retrieve the founder password for a channel"),
	/* CHAN_HELP_CMD_FORBID */
	gettext("    FORBID     Prevent a channel from being used"),
	/* CHAN_HELP_CMD_SUSPEND */
	gettext("    SUSPEND    Prevent a channel from being used preserving\n"
	"               channel data and settings"),
	/* CHAN_HELP_CMD_UNSUSPEND */
	gettext("    UNSUSPEND  Releases a suspended channel"),
	/* CHAN_HELP_CMD_STATUS */
	gettext("    STATUS     Returns the current access level of a user\n"
	"               on a channel"),
	/* CHAN_HELP_CMD_REGISTER */
	gettext("    REGISTER   Register a channel"),
	/* CHAN_HELP_CMD_SET */
	gettext("    SET        Set channel options and information"),
	/* CHAN_HELP_CMD_SASET */
	gettext("    SASET      Forcefully set channel options and information"),
	/* CHAN_HELP_CMD_QOP */
	gettext("    QOP        Modify the list of QOP users"),
	/* CHAN_HELP_CMD_AOP */
	gettext("    AOP        Modify the list of AOP users"),
	/* CHAN_HELP_CMD_SOP */
	gettext("    SOP        Modify the list of SOP users"),
	/* CHAN_HELP_CMD_ACCESS */
	gettext("    ACCESS     Modify the list of privileged users"),
	/* CHAN_HELP_CMD_LEVELS */
	gettext("    LEVELS     Redefine the meanings of access levels"),
	/* CHAN_HELP_CMD_AKICK */
	gettext("    AKICK      Maintain the AutoKick list"),
	/* CHAN_HELP_CMD_DROP */
	gettext("    DROP       Cancel the registration of a channel"),
	/* CHAN_HELP_CMD_BAN */
	gettext("    BAN        Bans a selected nick on a channel"),
	/* CHAN_HELP_CMD_CLEAR */
	gettext("    CLEAR      Tells ChanServ to clear certain settings on a channel"),
	/* CHAN_HELP_CMD_DEVOICE */
	gettext("    DEVOICE    Devoices a selected nick on a channel"),
	/* CHAN_HELP_CMD_GETKEY */
	gettext("    GETKEY     Returns the key of the given channel"),
	/* CHAN_HELP_CMD_INFO */
	gettext("    INFO       Lists information about the named registered channel"),
	/* CHAN_HELP_CMD_INVITE */
	gettext("    INVITE     Tells ChanServ to invite you into a channel"),
	/* CHAN_HELP_CMD_KICK */
	gettext("    KICK       Kicks a selected nick from a channel"),
	/* CHAN_HELP_CMD_LIST */
	gettext("    LIST       Lists all registered channels matching the given pattern"),
	/* CHAN_HELP_CMD_OP */
	gettext("    OP         Gives Op status to a selected nick on a channel"),
	/* CHAN_HELP_CMD_TOPIC */
	gettext("    TOPIC      Manipulate the topic of the specified channel"),
	/* CHAN_HELP_CMD_UNBAN */
	gettext("    UNBAN      Remove all bans preventing a user from entering a channel"),
	/* CHAN_HELP_CMD_VOICE */
	gettext("    VOICE      Voices a selected nick on a channel"),
	/* CHAN_HELP_CMD_VOP */
	gettext("    VOP        Maintains the VOP (VOicePeople) list for a channel"),
	/* CHAN_HELP_CMD_DEHALFOP */
	gettext("    DEHALFOP   Dehalfops a selected nick on a channel"),
	/* CHAN_HELP_CMD_DEOWNER */
	gettext("    DEOWNER    Removes your owner status on a channel"),
	/* CHAN_HELP_CMD_DEPROTECT */
	gettext("    DEPROTECT  Deprotects a selected nick on a channel"),
	/* CHAN_HELP_CMD_HALFOP */
	gettext("    HALFOP     Halfops a selected nick on a channel"),
	/* CHAN_HELP_CMD_HOP */
	gettext("    HOP        Maintains the HOP (HalfOP) list for a channel"),
	/* CHAN_HELP_CMD_OWNER */
	gettext("    OWNER      Gives you owner status on channel"),
	/* CHAN_HELP_CMD_PROTECT */
	gettext("    PROTECT    Protects a selected nick on a channel"),
	/* CHAN_HELP_CMD_DEOP */
	gettext("    DEOP       Deops a selected nick on a channel"),
	/* CHAN_HELP */
	gettext("%S allows you to register and control various\n"
	"aspects of channels.  %S can often prevent\n"
	"malicious users from \"taking over\" channels by limiting\n"
	"who is allowed channel operator privileges.  Available\n"
	"commands are listed below; to use them, type\n"
	"%R%S command.  For more information on a\n"
	"specific command, type %R%S HELP command."),
	/* CHAN_HELP_EXPIRES */
	gettext("Note that any channel which is not used for %d days\n"
	"(i.e. which no user on the channel's access list enters\n"
	"for that period of time) will be automatically dropped."),
	/* CHAN_HELP_REGISTER */
	gettext("Syntax: REGISTER channel description\n"
	" \n"
	"Registers a channel in the %S database.  In order\n"
	"to use this command, you must first be a channel operator\n"
	"on the channel you're trying to register.\n"
	"The description, which must be included, is a\n"
	"general description of the channel's purpose.\n"
	" \n"
	"When you register a channel, you are recorded as the\n"
	"\"founder\" of the channel.  The channel founder is allowed\n"
	"to change all of the channel settings for the channel;\n"
	"%S will also automatically give the founder\n"
	"channel-operator privileges when s/he enters the channel.\n"
	"See the ACCESS command (%R%S HELP ACCESS) for\n"
	"information on giving a subset of these privileges to\n"
	"other channel users.\n"
	" \n"
	"NOTICE: In order to register a channel, you must have\n"
	"first registered your nickname.  If you haven't,\n"
	"%R%s HELP for information on how to do so."),
	/* CHAN_HELP_DROP */
	gettext("Syntax: DROP channel\n"
	" \n"
	"Unregisters the named channel.  Can only be used by\n"
	"channel founder."),
	/* CHAN_HELP_SASET_HEAD */
	gettext("Syntax: SASET channel option parameters\n"
	" \n"
	"Allows Services Operators to forcefully change settings\n"
	"on channels.\n"
	" \n"
	"Available options:"),
	/* CHAN_HELP_SET_HEAD */
	gettext("Syntax: SET channel option parameters\n"
	" \n"
	"Allows the channel founder to set various channel options\n"
	"and other information.\n"
	" \n"
	"Available options:"),
	/* CHAN_HELP_CMD_SET_FOUNDER */
	gettext("    FOUNDER       Set the founder of a channel"),
	/* CHAN_HELP_CMD_SET_SUCCESSOR */
	gettext("    SUCCESSOR     Set the successor for a channel"),
	/* CHAN_HELP_CMD_SET_DESC */
	gettext("    DESC          Set the channel description"),
	/* CHAN_HELP_CMD_SET_ENTRYMSG */
	gettext("    ENTRYMSG      Set a message to be sent to users when they\n"
	"                     enter the channel"),
	/* CHAN_HELP_CMD_SET_BANTYPE */
	gettext("    BANTYPE       Set how Services make bans on the channel"),
	/* CHAN_HELP_CMD_SET_MLOCK */
	gettext("    MLOCK         Lock channel modes on or off"),
	/* CHAN_HELP_CMD_SET_KEEPTOPIC */
	gettext("    KEEPTOPIC     Retain topic when channel is not in use"),
	/* CHAN_HELP_CMD_SET_OPNOTICE */
	gettext("    OPNOTICE      Send a notice when OP/DEOP commands are used"),
	/* CHAN_HELP_CMD_SET_PEACE */
	gettext("    PEACE         Regulate the use of critical commands"),
	/* CHAN_HELP_CMD_SET_PRIVATE */
	gettext("    PRIVATE       Hide channel from LIST command"),
	/* CHAN_HELP_CMD_SET_RESTRICTED */
	gettext("    RESTRICTED    Restrict access to the channel"),
	/* CHAN_HELP_CMD_SET_SECURE */
	gettext("    SECURE        Activate %S security features"),
	/* CHAN_HELP_CMD_SET_SECUREOPS */
	gettext("    SECUREOPS     Stricter control of chanop status"),
	/* CHAN_HELP_CMD_SET_SECUREFOUNDER */
	gettext("    SECUREFOUNDER Stricter control of channel founder status"),
	/* CHAN_HELP_CMD_SET_SIGNKICK */
	gettext("    SIGNKICK      Sign kicks that are done with KICK command"),
	/* CHAN_HELP_CMD_SET_TOPICLOCK */
	gettext("    TOPICLOCK     Topic can only be changed with TOPIC"),
	/* CHAN_HELP_CMD_SET_XOP */
	gettext("    XOP           Toggle the user privilege system"),
	/* CHAN_HELP_CMD_SET_PERSIST */
	gettext("    PERSIST       Set the channel as permanent"),
	/* CHAN_HELP_CMD_SET_NOEXPIRE */
	gettext("    NOEXPIRE      Prevent the channel from expiring"),
	/* CHAN_HELP_SET_TAIL */
	gettext("Type %R%S HELP SET option for more information on a\n"
	"particular option."),
	/* CHAN_HELP_SASET_TAIL */
	gettext("Type %R%S HELP SASET option for more information on a\n"
	"particular option."),
	/* CHAN_HELP_SET_FOUNDER */
	gettext("Syntax: %s channel FOUNDER nick\n"
	" \n"
	"Changes the founder of a channel.  The new nickname must\n"
	"be a registered one."),
	/* CHAN_HELP_SET_SUCCESSOR */
	gettext("Syntax: %s channel SUCCESSOR nick\n"
	" \n"
	"Changes the successor of a channel.  If the founder's\n"
	"nickname expires or is dropped while the channel is still\n"
	"registered, the successor will become the new founder of the\n"
	"channel.  However, if the successor already has too many\n"
	"channels registered (%d), the channel will be dropped\n"
	"instead, just as if no successor had been set.  The new\n"
	"nickname must be a registered one."),
	/* CHAN_HELP_SET_DESC */
	gettext("Syntax: %s channel DESC description\n"
	" \n"
	"Sets the description for the channel, which shows up with\n"
	"the LIST and INFO commands."),
	/* CHAN_HELP_SET_ENTRYMSG */
	gettext("Syntax: %s channel ENTRYMSG [message]\n"
	" \n"
	"Sets the message which will be sent via /notice to users\n"
	"when they enter the channel.  If no parameter is given,\n"
	"causes no message to be sent upon entering the channel."),
	/* CHAN_HELP_SET_BANTYPE */
	gettext("Syntax: %s channel BANTYPE bantype\n"
	" \n"
	"Sets the ban type that will be used by services whenever\n"
	"they need to ban someone from your channel.\n"
	" \n"
	"bantype is a number between 0 and 3 that means:\n"
	" \n"
	"0: ban in the form *!user@host\n"
	"1: ban in the form *!*user@host\n"
	"2: ban in the form *!*@host\n"
	"3: ban in the form *!*user@*.domain"),
	/* CHAN_HELP_SET_KEEPTOPIC */
	gettext("Syntax: %s channel KEEPTOPIC {ON | OFF}\n"
	" \n"
	"Enables or disables the topic retention option for a	\n"
	"channel. When topic retention is set, the topic for the\n"
	"channel will be remembered by %S even after the\n"
	"last user leaves the channel, and will be restored the\n"
	"next time the channel is created."),
	/* CHAN_HELP_SET_TOPICLOCK */
	gettext("Syntax: %s channel TOPICLOCK {ON | OFF}\n"
	" \n"
	"Enables or disables the topic lock option for a channel.\n"
	"When topic lock is set, %S will not allow the\n"
	"channel topic to be changed except via the TOPIC\n"
	"command."),
	/* CHAN_HELP_SET_MLOCK */
	gettext("Syntax: %s channel MLOCK modes\n"
	" \n"
	"Sets the mode-lock parameter for the channel. %S\n"
	"allows you to define certain channel modes to be always\n"
	"on, off or free to be either on or off.\n"
	" \n"
	"The modes parameter is constructed exactly the same way \n"
	"as a /MODE command; that is, modes followed by a + are \n"
	"locked on, and modes followed by a - are locked off. Note,\n"
	"however, that unlike the /MODE command, each use of\n"
	"SET MLOCK will remove all modes previously locked before\n"
	"setting the new!\n"
	" \n"
	"Warning:  If you set a mode-locked key, as in the second\n"
	"example below, you should also set the RESTRICTED option for\n"
	"the channel (see HELP SET RESTRICTED), or anyone entering\n"
	"the channel when it is empty will be able to see the key!\n"
	" \n"
	"Examples:\n"
	" \n"
	"    SET #channel MLOCK +nt-iklps\n"
	"        Forces modes n and t on, and modes i, k, l, p, and\n"
	"        s off.  Mode m is left free to be either on or off.\n"
	" \n"
	"    SET #channel MLOCK +knst-ilmp my-key\n"
	"        Forces modes k, n, s, and t on, and modes i, l, m,\n"
	"        and p off.  Also forces the channel key to be\n"
	"        \"my-key\".\n"
	" \n"
	"    SET #channel MLOCK +\n"
	"        Removes the mode lock; all channel modes are free\n"
	"        to be either on or off."),
	/* CHAN_HELP_SET_PEACE */
	gettext("Syntax: %s channel PEACE {ON | OFF}\n"
	" \n"
	"Enables or disables the peace option for a channel.\n"
	"When peace is set, a user won't be able to kick,\n"
	"ban or remove a channel status of a user that has\n"
	"a level superior or equal to his via %S commands."),
	/* CHAN_HELP_SET_PRIVATE */
	gettext("Syntax: %s channel PRIVATE {ON | OFF}\n"
	" \n"
	"Enables or disables the private option for a channel.\n"
	"When private is set, a %R%S LIST will not\n"
	"include the channel in any lists."),
	/* CHAN_HELP_SET_RESTRICTED */
	gettext("Syntax: %s channel RESTRICTED {ON | OFF}\n"
	" \n"
	"Enables or disables the restricted access option for a\n"
	"channel.  When restricted access is set, users not on the access list will\n"
	"instead be kicked and banned from the channel."),
	/* CHAN_HELP_SET_SECURE */
	gettext("Syntax: %s channel SECURE {ON | OFF}\n"
	" \n"
	"Enables or disables %S's security features for a\n"
	"channel. When SECURE is set, only users who have\n"
	"registered their nicknames with %s and IDENTIFY'd\n"
	"with their password will be given access to the channel\n"
	"as controlled by the access list."),
	/* CHAN_HELP_SET_SECUREOPS */
	gettext("Syntax: %s channel SECUREOPS {ON | OFF}\n"
	" \n"
	"Enables or disables the secure ops option for a channel.\n"
	"When secure ops is set, users who are not on the userlist\n"
	"will not be allowed chanop status."),
	/* CHAN_HELP_SET_SECUREFOUNDER */
	gettext("Syntax: %s channel SECUREFOUNDER {ON | OFF}\n"
	" \n"
	"Enables or disables the secure founder option for a channel.\n"
	"When secure founder is set, only the real founder will be\n"
	"able to drop the channel, change its password, its founder and its\n"
	"successor, and not those who have founder level access through\n"
	"the access/qop command."),
	/* CHAN_HELP_SET_SIGNKICK */
	gettext("Syntax: %s channel SIGNKICK {ON | LEVEL | OFF}\n"
	" \n"
	"Enables or disables signed kicks for a\n"
	"channel.  When SIGNKICK is set, kicks issued with\n"
	"%S KICK command will have the nick that used the\n"
	"command in their reason.\n"
	" \n"
	"If you use LEVEL, those who have a level that is superior \n"
	"or equal to the SIGNKICK level on the channel won't have their \n"
	"kicks signed. See %R%S HELP LEVELS for more information."),
	/* CHAN_HELP_SET_XOP */
	gettext("Syntax: %s channel XOP {ON | OFF}\n"
	" \n"
	"Enables or disables the xOP lists system for a channel.\n"
	"When XOP is set, you have to use the AOP/SOP/VOP\n"
	"commands in order to give channel privileges to\n"
	"users, else you have to use the ACCESS command.\n"
	" \n"
	"Technical Note: when you switch from access list to xOP \n"
	"lists system, your level definitions and user levels will be\n"
	"changed, so you won't find the same values if you\n"
	"switch back to access system! \n"
	" \n"
	"You should also check that your users are in the good xOP \n"
	"list after the switch from access to xOP lists, because the \n"
	"guess is not always perfect... in fact, it is not recommended \n"
	"to use the xOP lists if you changed level definitions with \n"
	"the LEVELS command.\n"
	" \n"
	"Switching from xOP lists system to access list system\n"
	"causes no problem though."),
	/* CHAN_HELP_SET_PERSIST */
	gettext("Syntax: %s channel PERSIST {ON | OFF}\n"
	"Enables or disables the persistant channel setting.\n"
	"When persistant is set, the service bot will remain\n"
	"in the channel when it has emptied of users.\n"
	" \n"
	"If your IRCd does not a permanent (persistant) channel\n"
	"mode you must have a service bot in your channel to\n"
	"set persist on, and it can not be unassigned while persist\n"
	"is on.\n"
	" \n"
	"If this network does not have BotServ enabled and does\n"
	"not have a permanent channel mode, ChanServ will\n"
	"join your channel when you set persist on (and leave when\n"
	"it has been set off).\n"
	" \n"
	"If your IRCd has a permanent (persistant) channel mode\n"
	"and is is set or unset (for any reason, including MLOCK),\n"
	"persist is automatically set and unset for the channel aswell.\n"
	"Additionally, services will set or unset this mode when you\n"
	"set persist on or off."),
	/* CHAN_HELP_SET_OPNOTICE */
	gettext("Syntax: %s channel OPNOTICE {ON | OFF}\n"
	" \n"
	"Enables or disables the op-notice option for a channel.\n"
	"When op-notice is set, %S will send a notice to the\n"
	"channel whenever the OP or DEOP commands are used for a user\n"
	"in the channel."),
	/* CHAN_HELP_QOP */
	gettext("Syntax: QOP channel ADD nick\n"
	"        QOP channel DEL {nick | entry-num | list}\n"
	"        QOP channel LIST [mask | list]\n"
	"        QOP channel CLEAR\n"
	" \n"
	"Maintains the QOP (AutoOwner) list for a channel. The QOP \n"
	"list gives users the right to be auto-owner on your channel,\n"
	"which gives them almost (or potentially, total) access.\n"
	" \n"
	"The QOP ADD command adds the given nickname to the\n"
	"QOP list.\n"
	" \n"
	"The QOP DEL command removes the given nick from the\n"
	"QOP list.  If a list of entry numbers is given, those\n"
	"entries are deleted.  (See the example for LIST below.)\n"
	" \n"
	"The QOP LIST command displays the QOP list.  If\n"
	"a wildcard mask is given, only those entries matching the\n"
	"mask are displayed.  If a list of entry numbers is given,\n"
	"only those entries are shown; for example:\n"
	"   QOP #channel LIST 2-5,7-9\n"
	"      Lists QOP entries numbered 2 through 5 and\n"
	"      7 through 9.\n"
	"      \n"
	"The QOP CLEAR command clears all entries of the\n"
	"QOP list.\n"
	" \n"
	"The QOP commands are limited to\n"
	"founders (unless SECUREOPS is off). However, any user on the\n"
	"QOP list may use the QOP LIST command.\n"
	" \n"
	"This command may have been disabled for your channel, and\n"
	"in that case you need to use the access list. See \n"
	"%R%S HELP ACCESS for information about the access list,\n"
	"and %R%S HELP SET XOP to know how to toggle between \n"
	"the access list and xOP list systems."),
	/* CHAN_HELP_AOP */
	gettext("Syntax: AOP channel ADD nick\n"
	"        AOP channel DEL {nick | entry-num | list}\n"
	"        AOP channel LIST [mask | list]\n"
	"        AOP channel CLEAR\n"
	" \n"
	"Maintains the AOP (AutoOP) list for a channel. The AOP \n"
	"list gives users the right to be auto-opped on your channel,\n"
	"to unban or invite themselves if needed, to have their\n"
	"greet message showed on join, and so on.\n"
	" \n"
	"The AOP ADD command adds the given nickname to the\n"
	"AOP list.\n"
	" \n"
	"The AOP DEL command removes the given nick from the\n"
	"AOP list.  If a list of entry numbers is given, those\n"
	"entries are deleted.  (See the example for LIST below.)\n"
	" \n"
	"The AOP LIST command displays the AOP list.  If\n"
	"a wildcard mask is given, only those entries matching the\n"
	"mask are displayed.  If a list of entry numbers is given,\n"
	"only those entries are shown; for example:\n"
	"   AOP #channel LIST 2-5,7-9\n"
	"      Lists AOP entries numbered 2 through 5 and\n"
	"      7 through 9.\n"
	"      \n"
	"The AOP CLEAR command clears all entries of the\n"
	"AOP list.\n"
	" \n"
	"The AOP ADD and AOP DEL commands are limited to\n"
	"SOPs or above, while the AOP CLEAR command can only\n"
	"be used by the channel founder. However, any user on the\n"
	"AOP list may use the AOP LIST command.\n"
	" \n"
	"This command may have been disabled for your channel, and\n"
	"in that case you need to use the access list. See \n"
	"%R%S HELP ACCESS for information about the access list,\n"
	"and %R%S HELP SET XOP to know how to toggle between \n"
	"the access list and xOP list systems."),
	/* CHAN_HELP_HOP */
	gettext("Syntax: HOP channel ADD nick\n"
	"        HOP channel DEL {nick | entry-num | list}\n"
	"        HOP channel LIST [mask | list]\n"
	"        HOP channel CLEAR\n"
	" \n"
	"Maintains the HOP (HalfOP) list for a channel. The HOP \n"
	"list gives users the right to be auto-halfopped on your \n"
	"channel.\n"
	" \n"
	"The HOP ADD command adds the given nickname to the\n"
	"HOP list.\n"
	" \n"
	"The HOP DEL command removes the given nick from the\n"
	"HOP list.  If a list of entry numbers is given, those\n"
	"entries are deleted.  (See the example for LIST below.)\n"
	" \n"
	"The HOP LIST command displays the HOP list.  If\n"
	"a wildcard mask is given, only those entries matching the\n"
	"mask are displayed.  If a list of entry numbers is given,\n"
	"only those entries are shown; for example:\n"
	"   HOP #channel LIST 2-5,7-9\n"
	"      Lists HOP entries numbered 2 through 5 and\n"
	"      7 through 9.\n"
	"      \n"
	"The HOP CLEAR command clears all entries of the\n"
	"HOP list.\n"
	" \n"
	"The HOP ADD, HOP DEL and HOP LIST commands are \n"
	"limited to AOPs or above, while the HOP CLEAR command \n"
	"can only be used by the channel founder.\n"
	" \n"
	"This command may have been disabled for your channel, and\n"
	"in that case you need to use the access list. See \n"
	"%R%S HELP ACCESS for information about the access list,\n"
	"and %R%S HELP SET XOP to know how to toggle between \n"
	"the access list and xOP list systems."),
	/* CHAN_HELP_SOP */
	gettext("Syntax: SOP channel ADD nick\n"
	"        SOP channel DEL {nick | entry-num | list}\n"
	"        SOP channel LIST [mask | list]\n"
	"        SOP channel CLEAR\n"
	" \n"
	"Maintains the SOP (SuperOP) list for a channel. The SOP \n"
	"list gives users all rights given by the AOP list, and adds\n"
	"those needed to use the AutoKick and the BadWords lists, \n"
	"to send and read channel memos, and so on.\n"
	" \n"
	"The SOP ADD command adds the given nickname to the\n"
	"SOP list.\n"
	" \n"
	"The SOP DEL command removes the given nick from the\n"
	"SOP list.  If a list of entry numbers is given, those\n"
	"entries are deleted.  (See the example for LIST below.)\n"
	" \n"
	"The SOP LIST command displays the SOP list.  If\n"
	"a wildcard mask is given, only those entries matching the\n"
	"mask are displayed.  If a list of entry numbers is given,\n"
	"only those entries are shown; for example:\n"
	"   SOP #channel LIST 2-5,7-9\n"
	"      Lists AOP entries numbered 2 through 5 and\n"
	"      7 through 9.\n"
	"      \n"
	"The SOP CLEAR command clears all entries of the\n"
	"SOP list.\n"
	" \n"
	"The SOP ADD, SOP DEL and SOP CLEAR commands are \n"
	"limited to the channel founder. However, any user on the\n"
	"AOP list may use the SOP LIST command.\n"
	" \n"
	"This command may have been disabled for your channel, and\n"
	"in that case you need to use the access list. See \n"
	"%R%S HELP ACCESS for information about the access list,\n"
	"and %R%S HELP SET XOP to know how to toggle between \n"
	"the access list and xOP list systems."),
	/* CHAN_HELP_VOP */
	gettext("Syntax: VOP channel ADD nick\n"
	"        VOP channel DEL {nick | entry-num | list}\n"
	"        VOP channel LIST [mask | list]\n"
	"        VOP channel CLEAR\n"
	" \n"
	"Maintains the VOP (VOicePeople) list for a channel.  \n"
	"The VOP list allows users to be auto-voiced and to voice \n"
	"themselves if they aren't.\n"
	" \n"
	"The VOP ADD command adds the given nickname to the\n"
	"VOP list.\n"
	" \n"
	"The VOP DEL command removes the given nick from the\n"
	"VOP list.  If a list of entry numbers is given, those\n"
	"entries are deleted.  (See the example for LIST below.)\n"
	" \n"
	"The VOP LIST command displays the VOP list.  If\n"
	"a wildcard mask is given, only those entries matching the\n"
	"mask are displayed.  If a list of entry numbers is given,\n"
	"only those entries are shown; for example:\n"
	"   VOP #channel LIST 2-5,7-9\n"
	"      Lists VOP entries numbered 2 through 5 and\n"
	"      7 through 9.\n"
	"      \n"
	"The VOP CLEAR command clears all entries of the\n"
	"VOP list.\n"
	" \n"
	"The VOP ADD, VOP DEL and VOP LIST commands are \n"
	"limited to AOPs or above, while the VOP CLEAR command \n"
	"can only be used by the channel founder.\n"
	" \n"
	"This command may have been disabled for your channel, and\n"
	"in that case you need to use the access list. See \n"
	"%R%S HELP ACCESS for information about the access list,\n"
	"and %R%S HELP SET XOP to know how to toggle between \n"
	"the access list and xOP list systems."),
	/* CHAN_HELP_ACCESS */
	gettext("Syntax: ACCESS channel ADD nick level\n"
	"        ACCESS channel DEL {nick | entry-num | list}\n"
	"        ACCESS channel LIST [mask | list]\n"
	"        ACCESS channel VIEW [mask | list]\n"
	"        ACCESS channel CLEAR\n"
	" \n"
	"Maintains the access list for a channel.  The access\n"
	"list specifies which users are allowed chanop status or\n"
	"access to %S commands on the channel.  Different\n"
	"user levels allow for access to different subsets of\n"
	"privileges; %R%S HELP ACCESS LEVELS for more\n"
	"specific information.  Any nick not on the access list has\n"
	"a user level of 0.\n"
	" \n"
	"The ACCESS ADD command adds the given nickname to the\n"
	"access list with the given user level; if the nick is\n"
	"already present on the list, its access level is changed to\n"
	"the level specified in the command.  The level specified\n"
	"must be less than that of the user giving the command, and\n"
	"if the nick is already on the access list, the current\n"
	"access level of that nick must be less than the access level\n"
	"of the user giving the command.\n"
	" \n"
	"The ACCESS DEL command removes the given nick from the\n"
	"access list.  If a list of entry numbers is given, those\n"
	"entries are deleted.  (See the example for LIST below.)\n"
	" \n"
	"The ACCESS LIST command displays the access list.  If\n"
	"a wildcard mask is given, only those entries matching the\n"
	"mask are displayed.  If a list of entry numbers is given,\n"
	"only those entries are shown; for example:\n"
	"   ACCESS #channel LIST 2-5,7-9\n"
	"      Lists access entries numbered 2 through 5 and\n"
	"      7 through 9.\n"
	" \n"
	"The ACCESS VIEW command displays the access list similar\n"
	"to ACCESS LIST but shows the creator and last used time.\n"
	" \n"
	"The ACCESS CLEAR command clears all entries of the\n"
	"access list."),
	/* CHAN_HELP_ACCESS_LEVELS */
	gettext("User access levels\n"
	" \n"
	"By default, the following access levels are defined:\n"
	" \n"
	"   Founder   Full access to %S functions; automatic\n"
	"                     opping upon entering channel.  Note\n"
	"                     that only one person may have founder\n"
	"                     status (it cannot be given using the\n"
	"                     ACCESS command).\n"
	"        10   Access to AKICK command; automatic opping.\n"
	"         5   Automatic opping.\n"
	"         3   Automatic voicing.\n"
	"         0   No special privileges; can be opped by other\n"
	"                     ops (unless secure-ops is set).\n"
	"        <0   May not be opped.\n"
	" \n"
	"These levels may be changed, or new ones added, using the\n"
	"LEVELS command; type %R%S HELP LEVELS for\n"
	"information."),
	/* CHAN_HELP_AKICK */
	gettext("Syntax: AKICK channel ADD {nick | mask} [reason]\n"
	"        AKICK channel STICK mask\n"
	"        AKICK channel UNSTICK mask\n"
	"        AKICK channel DEL {nick | mask | entry-num | list}\n"
	"        AKICK channel LIST [mask | entry-num | list]\n"
	"        AKICK channel VIEW [mask | entry-num | list]\n"
	"        AKICK channel ENFORCE\n"
	"        AKICK channel CLEAR\n"
	" \n"
	"Maintains the AutoKick list for a channel.  If a user\n"
	"on the AutoKick list attempts to join the channel,\n"
	"%S will ban that user from the channel, then kick\n"
	"the user.\n"
	" \n"
	"The AKICK ADD command adds the given nick or usermask\n"
	"to the AutoKick list.  If a reason is given with\n"
	"the command, that reason will be used when the user is\n"
	"kicked; if not, the default reason is \"You have been\n"
	"banned from the channel\".\n"
	"When akicking a registered nick the nickserv account\n"
	"will be added to the akick list instead of the mask.\n"
	"All users within that nickgroup will then be akicked.\n"
	" \n"
	"The AKICK STICK command permanently bans the given mask \n"
	"on the channel. If someone tries to remove the ban, %S\n"
	"will automatically set it again. You can't use it for\n"
	"registered nicks.\n"
	" \n"
	"The AKICK UNSTICK command cancels the effect of the\n"
	"AKICK STICK command, so you'll be able to unset the\n"
	"ban again on the channel.\n"
	" \n"
	"The AKICK DEL command removes the given nick or mask\n"
	"from the AutoKick list.  It does not, however, remove any\n"
	"bans placed by an AutoKick; those must be removed\n"
	"manually.\n"
	" \n"
	"The AKICK LIST command displays the AutoKick list, or\n"
	"optionally only those AutoKick entries which match the\n"
	"given mask.\n"
	" \n"
	"The AKICK VIEW command is a more verbose version of\n"
	"AKICK LIST command.\n"
	" \n"
	"The AKICK ENFORCE command causes %S to enforce the\n"
	"current AKICK list by removing those users who match an\n"
	"AKICK mask.\n"
	" \n"
	"The AKICK CLEAR command clears all entries of the\n"
	"akick list."),
	/* CHAN_HELP_LEVELS */
	gettext("Syntax: LEVELS channel SET type level\n"
	"        LEVELS channel {DIS | DISABLE} type\n"
	"        LEVELS channel LIST\n"
	"        LEVELS channel RESET\n"
	" \n"
	"The LEVELS command allows fine control over the meaning of\n"
	"the numeric access levels used for channels.  With this\n"
	"command, you can define the access level required for most\n"
	"of %S's functions.  (The SET FOUNDER and this command\n"
	"are always restricted to the channel founder.)\n"
	" \n"
	"LEVELS SET allows the access level for a function or group of\n"
	"functions to be changed.  LEVELS DISABLE (or DIS for short)\n"
	"disables an automatic feature or disallows access to a\n"
	"function by anyone, INCLUDING the founder (although, the founder\n"
	"can always reenable it).\n"
	" \n"
	"LEVELS LIST shows the current levels for each function or\n"
	"group of functions.  LEVELS RESET resets the levels to the\n"
	"default levels of a newly-created channel (see\n"
	"HELP ACCESS LEVELS).\n"
	" \n"
	"For a list of the features and functions whose levels can be\n"
	"set, see HELP LEVELS DESC."),
	/* CHAN_HELP_LEVELS_DESC */
	gettext("The following feature/function names are understood.  Note\n"
	"that the levels for AUTODEOP and NOJOIN are maximum levels,\n"
	"while all others are minimum levels."),
	/* CHAN_HELP_LEVELS_DESC_FORMAT */
	gettext("    %-*s  %s"),
	/* CHAN_HELP_INFO */
	gettext("Syntax: INFO channel\n"
	" \n"
	"Lists information about the named registered channel,\n"
	"including its founder, time of registration, last time\n"
	"used, description, and mode lock, if any. If ALL is \n"
	"specified, the entry message and successor will also \n"
	"be displayed."),
	/* CHAN_HELP_LIST */
	gettext("Syntax: LIST pattern\n"
	" \n"
	"Lists all registered channels matching the given pattern.\n"
	"(Channels with the PRIVATE option set are not listed.)\n"
	"Note that a preceding '#' specifies a range, channel names\n"
	"are to be written without '#'."),
	/* CHAN_HELP_OP */
	gettext("Syntax: OP #channel [nick]\n"
	" \n"
	"Ops a selected nick on a channel. If nick is not given,\n"
	"it will op you.\n"
	" \n"
	"By default, limited to AOPs or those with level 5 access \n"
	"and above on the channel."),
	/* CHAN_HELP_DEOP */
	gettext("Syntax: DEOP #channel [nick]\n"
	" \n"
	"Deops a selected nick on a channel. If nick is not given,\n"
	"it will deop you.\n"
	" \n"
	"By default, limited to AOPs or those with level 5 access \n"
	"and above on the channel."),
	/* CHAN_HELP_VOICE */
	gettext("Syntax: VOICE #channel [nick]\n"
	" \n"
	"Voices a selected nick on a channel. If nick is not given,\n"
	"it will voice you.\n"
	" \n"
	"By default, limited to AOPs or those with level 5 access \n"
	"and above on the channel, or to VOPs or those with level 3 \n"
	"and above for self voicing."),
	/* CHAN_HELP_DEVOICE */
	gettext("Syntax: DEVOICE #channel [nick]\n"
	" \n"
	"Devoices a selected nick on a channel. If nick is not given,\n"
	"it will devoice you.\n"
	" \n"
	"By default, limited to AOPs or those with level 5 access \n"
	"and above on the channel, or to VOPs or those with level 3 \n"
	"and above for self devoicing."),
	/* CHAN_HELP_HALFOP */
	gettext("Syntax: HALFOP #channel [nick]\n"
	" \n"
	"Halfops a selected nick on a channel. If nick is not given,\n"
	"it will halfop you.\n"
	" \n"
	"By default, limited to AOPs and those with level 5 access \n"
	"and above on the channel, or to HOPs or those with level 4 \n"
	"and above for self halfopping."),
	/* CHAN_HELP_DEHALFOP */
	gettext("Syntax: DEHALFOP #channel [nick]\n"
	" \n"
	"Dehalfops a selected nick on a channel. If nick is not given,\n"
	"it will dehalfop you.\n"
	" \n"
	"By default, limited to AOPs and those with level 5 access \n"
	"and above on the channel, or to HOPs or those with level 4 \n"
	"and above for self dehalfopping."),
	/* CHAN_HELP_PROTECT */
	gettext("Syntax: PROTECT #channel [nick]\n"
	" \n"
	"Protects a selected nick on a channel. If nick is not given,\n"
	"it will protect you.\n"
	" \n"
	"By default, limited to the founder, or to SOPs or those with \n"
	"level 10 and above on the channel for self protecting."),
	/* CHAN_HELP_DEPROTECT */
	gettext("Syntax: DEPROTECT #channel [nick]\n"
	" \n"
	"Deprotects a selected nick on a channel. If nick is not given,\n"
	"it will deprotect you.\n"
	" \n"
	"By default, limited to the founder, or to SOPs or those with \n"
	"level 10 and above on the channel for self deprotecting."),
	/* CHAN_HELP_OWNER */
	gettext("Syntax: OWNER #channel\n"
	" \n"
	"Gives you owner status on channel.\n"
	" \n"
	"Limited to those with founder access on the channel."),
	/* CHAN_HELP_DEOWNER */
	gettext("Syntax: DEOWNER #channel\n"
	" \n"
	"Removes your owner status on channel.\n"
	" \n"
	"Limited to those with founder access on the channel."),
	/* CHAN_HELP_INVITE */
	gettext("Syntax: INVITE channel\n"
	" \n"
	"Tells %S to invite you into the given channel.  \n"
	" \n"
	"By default, limited to AOPs or those with level 5 and above\n"
	"on the channel."),
	/* CHAN_HELP_UNBAN */
	gettext("Syntax: UNBAN channel [nick]\n"
	" \n"
	"Tells %S to remove all bans preventing you or the given\n"
	"user from entering the given channel.  \n"
	" \n"
	"By default, limited to AOPs or those with level 5 and above\n"
	"on the channel."),
	/* CHAN_HELP_KICK */
	gettext("Syntax: KICK #channel nick [reason]\n"
	" \n"
	"Kicks a selected nick on a channel.\n"
	" \n"
	"By default, limited to AOPs or those with level 5 access \n"
	"and above on the channel."),
	/* CHAN_HELP_BAN */
	gettext("Syntax: BAN #channel nick [reason]\n"
	" \n"
	"Bans a selected nick on a channel.\n"
	" \n"
	"By default, limited to AOPs or those with level 5 access \n"
	"and above on the channel."),
	/* CHAN_HELP_TOPIC */
	gettext("Syntax: TOPIC channel [topic]\n"
	" \n"
	"Causes %S to set the channel topic to the one\n"
	"specified. If topic is not given, then an empty topic\n"
	"is set. This command is most useful in conjunction\n"
	"with SET TOPICLOCK. See %R%S HELP SET TOPICLOCK\n"
	"for more information.\n"
	" \n"
	"By default, limited to those with founder access on the\n"
	"channel."),
	/* CHAN_HELP_CLEAR */
	gettext("Syntax: CLEAR channel what\n"
	" \n"
	"Tells %S to clear certain settings on a channel.  what\n"
	"can be any of the following:\n"
	" \n"
	"     MODES    Resets all modes on the channel (i.e. clears\n"
	"                  modes i,k,l,m,n,p,s,t).\n"
	"     BANS     Clears all bans on the channel.\n"
	"     EXCEPTS  Clears all excepts on the channel.\n"
	"     INVITES  Clears all invites on the channel.\n"
	"     OPS      Removes channel-operator status (mode +o) from\n"
	"                  all channel operators. If supported, removes\n"
	"                  channel-admin (mode +a) and channel-owner (mode +q)\n"
	"                  as well.\n"
	"     HOPS     Removes channel-halfoperator status (mode +h) from\n"
	"                  all channel halfoperators, if supported.\n"
	"     VOICES   Removes \"voice\" status (mode +v) from anyone\n"
	"                  with that mode set.\n"
	"     USERS    Removes (kicks) all users from the channel.\n"
	" \n"
	"By default, limited to those with founder access on the\n"
	"channel."),
	/* CHAN_HELP_GETKEY */
	gettext("Syntax: GETKEY channel\n"
	" \n"
	"Returns the key of the given channel."),
	/* CHAN_SERVADMIN_HELP */
	gettext(" \n"
	"Services Operators can also drop any channel without needing\n"
	"to identify via password, and may view the access, AKICK,\n"
	"and level setting lists for any channel."),
	/* CHAN_SERVADMIN_HELP_DROP */
	gettext("Syntax: DROP channel\n"
	" \n"
	"Unregisters the named channel.  Only Services Operators\n"
	"can drop a channel for which they have not identified."),
	/* CHAN_SERVADMIN_HELP_SET_NOEXPIRE */
	gettext("Syntax: SET channel NOEXPIRE {ON | OFF}\n"
	" \n"
	"Sets whether the given channel will expire.  Setting this\n"
	"to ON prevents the channel from expiring."),
	/* CHAN_SERVADMIN_HELP_LIST */
	gettext("Syntax: LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE]\n"
	" \n"
	"Lists all registered channels matching the given pattern.\n"
	"Channels with the PRIVATE option set will only be displayed\n"
	"to Services Operators. Channels with the NOEXPIRE option set \n"
	"will have a ! appended to the channel name for Services Operators.\n"
	" \n"
	"If the FORBIDDEN, SUSPENDED or NOEXPIRE options are given, only \n"
	"channels which, respectively, are FORBIDden, SUSPENDed or have \n"
	"the NOEXPIRE flag set will be displayed.  If multiple options are \n"
	"given, more types of channels will be displayed. These options are \n"
	"limited to Services Operators."),
	/* CHAN_SERVADMIN_HELP_GETPASS */
	gettext("Syntax: GETPASS channel\n"
	" \n"
	"Returns the password for the given channel.  Note that\n"
	"whenever this command is used, a message including the\n"
	"person who issued the command and the channel it was used\n"
	"on will be logged and sent out as a WALLOPS/GLOBOPS."),
	/* CHAN_SERVADMIN_HELP_FORBID */
	gettext("Syntax: FORBID channel [reason]\n"
	" \n"
	"Disallows anyone from registering or using the given\n"
	"channel.  May be cancelled by dropping the channel.\n"
	" \n"
	"Reason may be required on certain networks."),
	/* CHAN_SERVADMIN_HELP_SUSPEND */
	gettext("Syntax: SUSPEND channel [reason]\n"
	" \n"
	"Disallows anyone from registering or using the given\n"
	"channel.  May be cancelled by using the UNSUSPEND\n"
	"command to preserve all previous channel data/settings.\n"
	" \n"
	"Reason may be required on certain networks."),
	/* CHAN_SERVADMIN_HELP_UNSUSPEND */
	gettext("Syntax: UNSUSPEND channel\n"
	" \n"
	"Releases a suspended channel. All data and settings\n"
	"are preserved from before the suspension."),
	/* CHAN_SERVADMIN_HELP_STATUS */
	gettext("Syntax: STATUS channel nickname\n"
	" \n"
	"Returns the current access level of the given nick on the\n"
	"given channel.  The reply is of the form:\n"
	" \n"
	"    STATUS channel nickname access-level\n"
	" \n"
	"If an error occurs, the reply will be in the form:\n"
	" \n"
	"    STATUS ERROR error-message"),
	/* MEMO_HELP_CMD_SEND */
	gettext("    SEND   Send a memo to a nick or channel"),
	/* MEMO_HELP_CMD_CANCEL */
	gettext("    CANCEL Cancel last memo you sent"),
	/* MEMO_HELP_CMD_LIST */
	gettext("    LIST   List your memos"),
	/* MEMO_HELP_CMD_READ */
	gettext("    READ   Read a memo or memos"),
	/* MEMO_HELP_CMD_DEL */
	gettext("    DEL    Delete a memo or memos"),
	/* MEMO_HELP_CMD_SET */
	gettext("    SET    Set options related to memos"),
	/* MEMO_HELP_CMD_INFO */
	gettext("    INFO   Displays information about your memos"),
	/* MEMO_HELP_CMD_RSEND */
	gettext("    RSEND  Sends a memo and requests a read receipt"),
	/* MEMO_HELP_CMD_CHECK */
	gettext("    CHECK  Checks if last memo to a nick was read"),
	/* MEMO_HELP_CMD_SENDALL */
	gettext("    SENDALL  Send a memo to all registered users"),
	/* MEMO_HELP_CMD_STAFF */
	gettext("    STAFF  Send a memo to all opers/admins"),
	/* MEMO_HELP_HEADER */
	gettext("%S is a utility allowing IRC users to send short\n"
	"messages to other IRC users, whether they are online at\n"
	"the time or not, or to channels(*).  Both the sender's\n"
	"nickname and the target nickname or channel must be\n"
	"registered in order to send a memo.\n"
	"%S's commands include:"),
	/* MEMO_HELP_ADMIN */
	gettext("not used."),
	/* MEMO_HELP_FOOTER */
	gettext("Type %R%S HELP command for help on any of the\n"
	"above commands.\n"
	"(*) By default, any user with at least level 10 access on a\n"
	"    channel can read that channel's memos.  This can be\n"
	"    changed with the %s LEVELS command."),
	/* MEMO_HELP_SEND */
	gettext("Syntax: SEND {nick | channel} memo-text\n"
	" \n"
	"Sends the named nick or channel a memo containing\n"
	"memo-text.  When sending to a nickname, the recipient will\n"
	"receive a notice that he/she has a new memo.  The target\n"
	"nickname/channel must be registered."),
	/* MEMO_HELP_CANCEL */
	gettext("Syntax: CANCEL {nick | channel}\n"
	" \n"
	"Cancels the last memo you sent to the given nick or channel,\n"
	"provided it has not been read at the time you use the command."),
	/* MEMO_HELP_LIST */
	gettext("Syntax: LIST [channel] [list | NEW]\n"
	" \n"
	"Lists any memos you currently have.  With NEW, lists only\n"
	"new (unread) memos.  Unread memos are marked with a \"*\"\n"
	"to the left of the memo number.  You can also specify a list\n"
	"of numbers, as in the example below:\n"
	"   LIST 2-5,7-9\n"
	"      Lists memos numbered 2 through 5 and 7 through 9."),
	/* MEMO_HELP_READ */
	gettext("Syntax: READ [channel] {num | list | LAST | NEW}\n"
	" \n"
	"Sends you the text of the memos specified.  If LAST is\n"
	"given, sends you the memo you most recently received.  If\n"
	"NEW is given, sends you all of your new memos.  Otherwise,\n"
	"sends you memo number num.  You can also give a list of\n"
	"numbers, as in this example:\n"
	" \n"
	"   READ 2-5,7-9\n"
	"      Displays memos numbered 2 through 5 and 7 through 9."),
	/* MEMO_HELP_DEL */
	gettext("Syntax: DEL [channel] {num | list | LAST | ALL}\n"
	" \n"
	"Deletes the specified memo or memos.  You can supply\n"
	"multiple memo numbers or ranges of numbers instead of a\n"
	"single number, as in the second example below.\n"
	" \n"
	"If LAST is given, the last memo will be deleted.\n"
	"If ALL is given, deletes all of your memos.\n"
	" \n"
	"Examples:\n"
	" \n"
	"   DEL 1\n"
	"      Deletes your first memo.\n"
	" \n"
	"   DEL 2-5,7-9\n"
	"      Deletes memos numbered 2 through 5 and 7 through 9."),
	/* MEMO_HELP_SET */
	gettext("Syntax: SET option parameters\n"
	"Sets various memo options.  option can be one of:\n"
	" \n"
	"    NOTIFY      Changes when you will be notified about\n"
	"                    new memos (only for nicknames)\n"
	"    LIMIT       Sets the maximum number of memos you can\n"
	"                    receive\n"
	" \n"
	"Type %R%S HELP SET option for more information\n"
	"on a specific option."),
	/* MEMO_HELP_SET_NOTIFY */
	gettext("Syntax: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}\n"
	"Changes when you will be notified about new memos:\n"
	" \n"
	"    ON      You will be notified of memos when you log on,\n"
	"               when you unset /AWAY, and when they are sent\n"
	"               to you.\n"
	"    LOGON   You will only be notified of memos when you log\n"
	"               on or when you unset /AWAY.\n"
	"    NEW     You will only be notified of memos when they\n"
	"               are sent to you.\n"
	"    MAIL    You will be notified of memos by email aswell as\n"
	"               any other settings you have.\n"
	"    NOMAIL  You will not be notified of memos by email.\n"
	"    OFF     You will not receive any notification of memos.\n"
	" \n"
	"ON is essentially LOGON and NEW combined."),
	/* MEMO_HELP_SET_LIMIT */
	gettext("Syntax: SET LIMIT [channel] limit\n"
	" \n"
	"Sets the maximum number of memos you (or the given channel)\n"
	"are allowed to have. If you set this to 0, no one will be\n"
	"able to send any memos to you.  However, you cannot set\n"
	"this any higher than %d."),
	/* MEMO_HELP_INFO */
	gettext("Syntax: INFO [channel]\n"
	" \n"
	"Displays information on the number of memos you have, how\n"
	"many of them are unread, and how many total memos you can\n"
	"receive.  With a parameter, displays the same information\n"
	"for the given channel."),
	/* MEMO_SERVADMIN_HELP_SET_LIMIT */
	gettext("Syntax: SET LIMIT [user | channel] {limit | NONE} [HARD]\n"
	" \n"
	"Sets the maximum number of memos a user or channel is\n"
	"allowed to have.  Setting the limit to 0 prevents the user\n"
	"from receiving any memos; setting it to NONE allows the\n"
	"user to receive and keep as many memos as they want.  If\n"
	"you do not give a nickname or channel, your own limit is\n"
	"set.\n"
	" \n"
	"Adding HARD prevents the user from changing the limit.  Not\n"
	"adding HARD has the opposite effect, allowing the user to\n"
	"change the limit (even if a previous limit was set with\n"
	"HARD).\n"
	"This use of the SET LIMIT command is limited to Services\n"
	"admins.  Other users may only enter a limit for themselves\n"
	"or a channel on which they have such privileges, may not\n"
	"remove their limit, may not set a limit above %d, and may\n"
	"not set a hard limit."),
	/* MEMO_SERVADMIN_HELP_INFO */
	gettext("Syntax: INFO [nick | channel]\n"
	"Without a parameter, displays information on the number of\n"
	"memos you have, how many of them are unread, and how many\n"
	"total memos you can receive.\n"
	" \n"
	"With a channel parameter, displays the same information for\n"
	"the given channel.\n"
	" \n"
	"With a nickname parameter, displays the same information\n"
	"for the given nickname.  This use limited to Services\n"
	"admins."),
	/* MEMO_HELP_STAFF */
	gettext("Syntax: STAFF memo-text\n"
	"Sends all services staff a memo containing memo-text."),
	/* MEMO_HELP_SENDALL */
	gettext("Syntax: SENDALL memo-text\n"
	"Sends all registered users a memo containing memo-text."),
	/* MEMO_HELP_RSEND */
	gettext("Syntax: RSEND {nick | channel} memo-text\n"
	" \n"
	"Sends the named nick or channel a memo containing\n"
	"memo-text.  When sending to a nickname, the recipient will\n"
	"receive a notice that he/she has a new memo.  The target\n"
	"nickname/channel must be registered.\n"
	"Once the memo is read by its recipient, an automatic notification\n"
	"memo will be sent to the sender informing him/her that the memo\n"
	"has been read."),
	/* MEMO_HELP_CHECK */
	gettext("Syntax: CHECK nick\n"
	" \n"
	"Checks whether the _last_ memo you sent to nick has been read\n"
	"or not. Note that this does only work with nicks, not with chans."),
	/* OPER_HELP_CMD_GLOBAL */
	gettext("    GLOBAL      Send a message to all users"),
	/* OPER_HELP_CMD_STATS */
	gettext("    STATS       Show status of Services and network"),
	/* OPER_HELP_CMD_STAFF */
	gettext("    STAFF       Display Services staff and online status"),
	/* OPER_HELP_CMD_MODE */
	gettext("    MODE        Change a channel's modes"),
	/* OPER_HELP_CMD_KICK */
	gettext("    KICK        Kick a user from a channel"),
	/* OPER_HELP_CMD_CLEARMODES */
	gettext("    CLEARMODES  Clear modes of a channel"),
	/* OPER_HELP_CMD_KILLCLONES */
	gettext("    KILLCLONES  Kill all users that have a certain host"),
	/* OPER_HELP_CMD_AKILL */
	gettext("    AKILL       Manipulate the AKILL list"),
	/* OPER_HELP_CMD_SNLINE */
	gettext("    SNLINE      Manipulate the SNLINE list"),
	/* OPER_HELP_CMD_SQLINE */
	gettext("    SQLINE      Manipulate the SQLINE list"),
	/* OPER_HELP_CMD_SZLINE */
	gettext("    SZLINE      Manipulate the SZLINE list"),
	/* OPER_HELP_CMD_CHANLIST */
	gettext("    CHANLIST    Lists all channel records"),
	/* OPER_HELP_CMD_USERLIST */
	gettext("    USERLIST    Lists all user records"),
	/* OPER_HELP_CMD_LOGONNEWS */
	gettext("    LOGONNEWS   Define messages to be shown to users at logon"),
	/* OPER_HELP_CMD_RANDOMNEWS */
	gettext("    RANDOMNEWS  Define messages to be randomly shown to users \n"
	"                at logon"),
	/* OPER_HELP_CMD_OPERNEWS */
	gettext("    OPERNEWS    Define messages to be shown to users who oper"),
	/* OPER_HELP_CMD_SESSION */
	gettext("    SESSION     View the list of host sessions"),
	/* OPER_HELP_CMD_EXCEPTION */
	gettext("    EXCEPTION   Modify the session-limit exception list"),
	/* OPER_HELP_CMD_NOOP */
	gettext("    NOOP        Temporarily remove all O:lines of a server \n"
	"                remotely"),
	/* OPER_HELP_CMD_JUPE */
	gettext("    JUPE        \"Jupiter\" a server"),
	/* OPER_HELP_CMD_IGNORE */
	gettext("    IGNORE      Modify the Services ignore list"),
	/* OPER_HELP_CMD_SET */
	gettext("    SET         Set various global Services options"),
	/* OPER_HELP_CMD_RELOAD */
	gettext("    RELOAD      Reload services' configuration file"),
	/* OPER_HELP_CMD_UPDATE */
	gettext("    UPDATE      Force the Services databases to be\n"
	"                updated on disk immediately"),
	/* OPER_HELP_CMD_RESTART */
	gettext("    RESTART     Save databases and restart Services"),
	/* OPER_HELP_CMD_QUIT */
	gettext("    QUIT        Terminate the Services program with no save"),
	/* OPER_HELP_CMD_SHUTDOWN */
	gettext("    SHUTDOWN    Terminate the Services program with save"),
	/* OPER_HELP_CMD_DEFCON */
	gettext("    DEFCON      Manipulate the DefCon system"),
	/* OPER_HELP_CMD_CHANKILL */
	gettext("    CHANKILL    AKILL all users on a specific channel"),
	/* OPER_HELP_CMD_OLINE */
	gettext("    OLINE       Give Operflags to a certain user"),
	/* OPER_HELP_CMD_UMODE */
	gettext("    UMODE       Change a user's modes"),
	/* OPER_HELP_CMD_SVSNICK */
	gettext("    SVSNICK     Forcefully change a user's nickname"),
	/* OPER_HELP_CMD_MODLOAD */
	gettext("    MODLOAD     Load a module"),
	/* OPER_HELP_CMD_MODUNLOAD */
	gettext("    MODUNLOAD   Un-Load a module"),
	/* OPER_HELP_CMD_MODINFO */
	gettext("    MODINFO     Info about a loaded module"),
	/* OPER_HELP_CMD_MODLIST */
	gettext("    MODLIST     List loaded modules"),
	/* OPER_HELP */
	gettext("%S commands:"),
	/* OPER_HELP_LOGGED */
	gettext("Notice: All commands sent to %S are logged!"),
	/* OPER_HELP_GLOBAL */
	gettext("Syntax: GLOBAL message\n"
	" \n"
	"Allows Administrators to send messages to all users on the \n"
	"network. The message will be sent from the nick %s."),
	/* OPER_HELP_STATS */
	gettext("Syntax: STATS [AKILL | ALL | RESET | MEMORY | UPLINK]\n"
	" \n"
	"Without any option, shows the current number of users and\n"
	"IRCops online (excluding Services), the highest number of\n"
	"users online since Services was started, and the length of\n"
	"time Services has been running.\n"
	" \n"
	"With the AKILL option, displays the current size of the\n"
	"AKILL list and the current default expiry time.\n"
	" \n"
	"The RESET option currently resets the maximum user count\n"
	"to the number of users currently present on the network.\n"
	" \n"
	"The MEMORY option displays information on the memory\n"
	"usage of Services. Using this option can freeze Services for\n"
	"a short period of time on large networks; don't overuse it!\n"
	" \n"
	"The UPLINK option displays information about the current\n"
	"server Anope uses as an uplink to the network.\n"
	" \n"
	"The ALL displays the user and uptime statistics, and\n"
	"everything you'd see with MEMORY and UPLINK options."),
	/* OPER_HELP_IGNORE */
	gettext("Syntax: IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick | mask]\n"
	" \n"
	"Allows Services Operators to make Services ignore a nick or mask\n"
	"for a certain time or until the next restart. The default\n"
	"time format is seconds. You can specify it by using units.\n"
	"Valid units are: s for seconds, m for minutes, \n"
	"h for hours and d for days. \n"
	"Combinations of these units are not permitted.\n"
	"To make Services permanently ignore the	user, type 0 as time.\n"
	"When adding a mask, it should be in the format user@host\n"
	"or nick!user@host, everything else will be considered a nick.\n"
	"Wildcards are permitted.\n"
	" \n"
	"Ignores will not be enforced on IRC Operators."),
	/* OPER_HELP_MODE */
	gettext("Syntax: MODE channel modes\n"
	" \n"
	"Allows Services operators to set channel modes for any\n"
	"channel.  Parameters are the same as for the standard /MODE\n"
	"command."),
	/* OPER_HELP_UMODE */
	gettext("Syntax: UMODE user modes\n"
	" \n"
	"Allows Services Opers to set user modes for any user.\n"
	"Parameters are the same as for the standard /MODE\n"
	"command."),
	/* OPER_HELP_OLINE */
	gettext("Syntax: OLINE user flags\n"
	" \n"
	"Allows Services Opers to give Operflags to any user.\n"
	"Flags have to be prefixed with a \"+\" or a \"-\". To\n"
	"remove all flags simply type a \"-\" instead of any flags."),
	/* OPER_HELP_CLEARMODES */
	gettext("Syntax: CLEARMODES channel [ALL]\n"
	" \n"
	"Clears all binary modes (i,k,l,m,n,p,s,t) and bans from a\n"
	"channel.  If ALL is given, also clears all ops and\n"
	"voices (+o and +v modes) from the channel."),
	/* OPER_HELP_KICK */
	gettext("Syntax: KICK channel user reason\n"
	" \n"
	"Allows staff to kick a user from any channel.\n"
	"Parameters are the same as for the standard /KICK\n"
	"command.  The kick message will have the nickname of the\n"
	"IRCop sending the KICK command prepended; for example:\n"
	" \n"
	"*** SpamMan has been kicked off channel #my_channel by %S (Alcan (Flood))"),
	/* OPER_HELP_SVSNICK */
	gettext("Syntax: SVSNICK nick newnick\n"
	" \n"
	"Forcefully changes a user's nickname from nick to newnick."),
	/* OPER_HELP_AKILL */
	gettext("Syntax: AKILL ADD [+expiry] mask reason\n"
	"        AKILL DEL {mask | entry-num | list}\n"
	"        AKILL LIST [mask | list]\n"
	"        AKILL VIEW [mask | list]\n"
	"        AKILL CLEAR\n"
	" \n"
	"Allows Services operators to manipulate the AKILL list.  If\n"
	"a user matching an AKILL mask attempts to connect, Services\n"
	"will issue a KILL for that user and, on supported server\n"
	"types, will instruct all servers to add a ban (K-line) for\n"
	"the mask which the user matched.\n"
	" \n"
	"AKILL ADD adds the given user@host/ip mask to the AKILL\n"
	"list for the given reason (which must be given).\n"
	"expiry is specified as an integer followed by one of d \n"
	"(days), h (hours), or m (minutes).  Combinations (such as \n"
	"1h30m) are not permitted.  If a unit specifier is not \n"
	"included, the default is days (so +30 by itself means 30 \n"
	"days).  To add an AKILL which does not expire, use +0.  If the\n"
	"usermask to be added starts with a +, an expiry time must\n"
	"be given, even if it is the same as the default.  The\n"
	"current AKILL default expiry time can be found with the\n"
	"STATS AKILL command.\n"
	" \n"
	"The AKILL DEL command removes the given mask from the\n"
	"AKILL list if it is present.  If a list of entry numbers is \n"
	"given, those entries are deleted.  (See the example for LIST \n"
	"below.)\n"
	" \n"
	"The AKILL LIST command displays the AKILL list.  \n"
	"If a wildcard mask is given, only those entries matching the\n"
	"mask are displayed.  If a list of entry numbers is given,\n"
	"only those entries are shown; for example:\n"
	"   AKILL LIST 2-5,7-9\n"
	"      Lists AKILL entries numbered 2 through 5 and 7 \n"
	"      through 9.\n"
	"      \n"
	"AKILL VIEW is a more verbose version of AKILL LIST, and \n"
	"will show who added an AKILL, the date it was added, and when \n"
	"it expires, as well as the user@host/ip mask and reason.\n"
	" \n"
	"AKILL CLEAR clears all entries of the AKILL list."),
	/* OPER_HELP_SNLINE */
	gettext("Syntax: SNLINE ADD [+expiry] mask:reason\n"
	"    SNLINE DEL {mask | entry-num | list}\n"
	"        SNLINE LIST [mask | list]\n"
	"        SNLINE VIEW [mask | list]\n"
	"        SNLINE CLEAR\n"
	" \n"
	"Allows Services operators to manipulate the SNLINE list.  If\n"
	"a user with a realname matching an SNLINE mask attempts to \n"
	"connect, Services will not allow it to pursue his IRC\n"
	"session.\n"
	" \n"
	"SNLINE ADD adds the given realname mask to the SNLINE\n"
	"list for the given reason (which must be given).\n"
	"expiry is specified as an integer followed by one of d \n"
	"(days), h (hours), or m (minutes).  Combinations (such as \n"
	"1h30m) are not permitted.  If a unit specifier is not \n"
	"included, the default is days (so +30 by itself means 30 \n"
	"days).  To add an SNLINE which does not expire, use +0.  If the\n"
	"realname mask to be added starts with a +, an expiry time must\n"
	"be given, even if it is the same as the default.  The\n"
	"current SNLINE default expiry time can be found with the\n"
	"STATS AKILL command.\n"
	"Note: because the realname mask may contain spaces, the\n"
	"separator between it and the reason is a colon.\n"
	" \n"
	"The SNLINE DEL command removes the given mask from the\n"
	"SNLINE list if it is present.  If a list of entry numbers is \n"
	"given, those entries are deleted.  (See the example for LIST \n"
	"below.)\n"
	" \n"
	"The SNLINE LIST command displays the SNLINE list.  \n"
	"If a wildcard mask is given, only those entries matching the\n"
	"mask are displayed.  If a list of entry numbers is given,\n"
	"only those entries are shown; for example:\n"
	"   SNLINE LIST 2-5,7-9\n"
	"      Lists SNLINE entries numbered 2 through 5 and 7 \n"
	"      through 9.\n"
	"      \n"
	"SNLINE VIEW is a more verbose version of SNLINE LIST, and \n"
	"will show who added an SNLINE, the date it was added, and when \n"
	"it expires, as well as the realname mask and reason.\n"
	" \n"
	"SNLINE CLEAR clears all entries of the SNLINE list."),
	/* OPER_HELP_SQLINE */
	gettext("Syntax: SQLINE ADD [+expiry] mask reason\n"
	"        SQLINE DEL {mask | entry-num | list}\n"
	"        SQLINE LIST [mask | list]\n"
	"        SQLINE VIEW [mask | list]\n"
	"        SQLINE CLEAR\n"
	" \n"
	"Allows Services operators to manipulate the SQLINE list.  If\n"
	"a user with a nick matching an SQLINE mask attempts to \n"
	"connect, Services will not allow it to pursue his IRC\n"
	"session.\n"
	"If the first character of the mask is #, services will \n"
	"prevent the use of matching channels (on IRCds that \n"
	"support it).\n"
	" \n"
	"SQLINE ADD adds the given mask to the SQLINE\n"
	"list for the given reason (which must be given).\n"
	"expiry is specified as an integer followed by one of d \n"
	"(days), h (hours), or m (minutes).  Combinations (such as \n"
	"1h30m) are not permitted.  If a unit specifier is not \n"
	"included, the default is days (so +30 by itself means 30 \n"
	"days).  To add an SQLINE which does not expire, use +0.  \n"
	"If the mask to be added starts with a +, an expiry time \n"
	"must be given, even if it is the same as the default. The\n"
	"current SQLINE default expiry time can be found with the\n"
	"STATS AKILL command.\n"
	" \n"
	"The SQLINE DEL command removes the given mask from the\n"
	"SQLINE list if it is present.  If a list of entry numbers is \n"
	"given, those entries are deleted.  (See the example for LIST \n"
	"below.)\n"
	" \n"
	"The SQLINE LIST command displays the SQLINE list.  \n"
	"If a wildcard mask is given, only those entries matching the\n"
	"mask are displayed.  If a list of entry numbers is given,\n"
	"only those entries are shown; for example:\n"
	"   SQLINE LIST 2-5,7-9\n"
	"      Lists SQLINE entries numbered 2 through 5 and 7 \n"
	"      through 9.\n"
	"      \n"
	"SQLINE VIEW is a more verbose version of SQLINE LIST, and \n"
	"will show who added an SQLINE, the date it was added, and when \n"
	"it expires, as well as the mask and reason.\n"
	" \n"
	"SQLINE CLEAR clears all entries of the SQLINE list."),
	/* OPER_HELP_SZLINE */
	gettext("Syntax: SZLINE ADD [+expiry] mask reason\n"
	"        SZLINE DEL {mask | entry-num | list}\n"
	"        SZLINE LIST [mask | list]\n"
	"        SZLINE VIEW [mask | list]\n"
	"        SZLINE CLEAR\n"
	" \n"
	"Allows Services operators to manipulate the SZLINE list.  If\n"
	"a user with an IP matching an SZLINE mask attempts to \n"
	"connect, Services will not allow it to pursue his IRC\n"
	"session (and this, whether the IP has a PTR RR or not).\n"
	" \n"
	"SZLINE ADD adds the given IP mask to the SZLINE\n"
	"list for the given reason (which must be given).\n"
	"expiry is specified as an integer followed by one of d \n"
	"(days), h (hours), or m (minutes).  Combinations (such as \n"
	"1h30m) are not permitted.  If a unit specifier is not \n"
	"included, the default is days (so +30 by itself means 30 \n"
	"days).  To add an SZLINE which does not expire, use +0.  If the\n"
	"realname mask to be added starts with a +, an expiry time must\n"
	"be given, even if it is the same as the default.  The\n"
	"current SZLINE default expiry time can be found with the\n"
	"STATS AKILL command.\n"
	" \n"
	"The SZLINE DEL command removes the given mask from the\n"
	"SZLINE list if it is present.  If a list of entry numbers is \n"
	"given, those entries are deleted.  (See the example for LIST \n"
	"below.)\n"
	" \n"
	"The SZLINE LIST command displays the SZLINE list.  \n"
	"If a wildcard mask is given, only those entries matching the\n"
	"mask are displayed.  If a list of entry numbers is given,\n"
	"only those entries are shown; for example:\n"
	"   SZLINE LIST 2-5,7-9\n"
	"      Lists SZLINE entries numbered 2 through 5 and 7 \n"
	"      through 9.\n"
	"      \n"
	"SZLINE VIEW is a more verbose version of SZLINE LIST, and \n"
	"will show who added an SZLINE, the date it was added, and when\n"
	"it expires, as well as the IP mask and reason.\n"
	" \n"
	"SZLINE CLEAR clears all entries of the SZLINE list."),
	/* OPER_HELP_SET */
	gettext("Syntax: SET option setting\n"
	"Sets various global Services options.  Option names\n"
	"currently defined are:\n"
	"    READONLY   Set read-only or read-write mode\n"
	"    LOGCHAN    Report log messages to a channel\n"
	"    DEBUG      Activate or deactivate debug mode\n"
	"    NOEXPIRE   Activate or deactivate no expire mode\n"
	"    SUPERADMIN Activate or deactivate super-admin mode\n"
	"    IGNORE     Activate or deactivate ignore mode\n"
	"    LIST       List the options"),
	/* OPER_HELP_SET_READONLY */
	gettext("Syntax: SET READONLY {ON | OFF}\n"
	" \n"
	"Sets read-only mode on or off.  In read-only mode, normal\n"
	"users will not be allowed to modify any Services data,\n"
	"including channel and nickname access lists, etc.  IRCops\n"
	"with sufficient Services privileges will be able to modify\n"
	"Services' AKILL list and drop or forbid nicknames and\n"
	"channels, but any such changes will not be saved unless\n"
	"read-only mode is deactivated before Services is terminated\n"
	"or restarted.\n"
	" \n"
	"This option is equivalent to the command-line option\n"
	"-readonly."),
	/* OPER_HELP_SET_LOGCHAN */
	gettext("Syntax: SET LOGCHAN {ON | OFF}\n"
	"With this setting on, Services will send its logs to a specified\n"
	"channel as well as the log file. LogChannel must also be defined\n"
	"in the Services configuration file for this setting to be of any\n"
	"use.\n"
	" \n"
	"This option is equivalent to the command-line option -logchan.\n"
	" \n"
	"Note: This can have strong security implications if your log\n"
	"channel is not properly secured."),
	/* OPER_HELP_SET_DEBUG */
	gettext("Syntax: SET DEBUG {ON | OFF | num}\n"
	" \n"
	"Sets debug mode on or off.  In debug mode, all data sent to\n"
	"and from Services as well as a number of other debugging\n"
	"messages are written to the log file.  If num is\n"
	"given, debug mode is activated, with the debugging level set\n"
	"to num.\n"
	"This option is equivalent to the command-line option\n"
	"-debug."),
	/* OPER_HELP_SET_NOEXPIRE */
	gettext("Syntax: SET NOEXPIRE {ON | OFF}\n"
	"Sets no expire mode on or off.  In no expire mode, nicks,\n"
	"channels, akills and exceptions won't expire until the\n"
	"option is unset.\n"
	"This option is equivalent to the command-line option\n"
	"-noexpire."),
	/* OPER_HELP_SET_SUPERADMIN */
	gettext("Syntax: SET SUPERADMIN {ON | OFF}\n"
	"Setting this will grant you extra privileges such as the\n"
	"ability to be \"founder\" on all channel's etc...\n"
	"This option is not persistent, and should only be used when\n"
	"needed, and set back to OFF when no longer needed."),
	/* OPER_HELP_SET_IGNORE */
	gettext("Syntax: SET IGNORE {ON | OFF}\n"
	"Setting this will toggle Anope's usage of the IGNORE system \n"
	"on or off."),
	/* OPER_HELP_SET_LIST */
	gettext("Syntax: SET LIST\n"
	"Display the various %S settings"),
	/* OPER_HELP_NOOP */
	gettext("Syntax: NOOP SET server\n"
	"        NOOP REVOKE server\n"
	"NOOP SET remove all O:lines of the given\n"
	"server and kill all IRCops currently on it to\n"
	"prevent them from rehashing the server (because this\n"
	"would just cancel the effect).\n"
	"NOOP REVOKE makes all removed O:lines available again\n"
	"on the given server.\n"
	"Note: The server is not checked at all by the\n"
	"Services."),
	/* OPER_HELP_JUPE */
	gettext("Syntax: JUPE server [reason]\n"
	" \n"
	"Tells Services to jupiter a server -- that is, to create\n"
	"a fake \"server\" connected to Services which prevents\n"
	"the real server of that name from connecting.  The jupe\n"
	"may be removed using a standard SQUIT.  If a reason is\n"
	"given, it is placed in the server information field;\n"
	"otherwise, the server information field will contain the\n"
	"text \"Juped by <nick>\", showing the nickname of the\n"
	"person who jupitered the server."),
	/* OPER_HELP_UPDATE */
	gettext("Syntax: UPDATE\n"
	" \n"
	"Causes Services to update all database files as soon as you\n"
	"send the command."),
	/* OPER_HELP_RELOAD */
	gettext("Syntax: RELOAD\n"
	"Causes Services to reload the configuration file. Note that\n"
	"some directives still need the restart of the Services to\n"
	"take effect (such as Services' nicknames, activation of the \n"
	"session limitation, etc.)"),
	/* OPER_HELP_QUIT */
	gettext("Syntax: QUIT\n"
	" \n"
	"Causes Services to do an immediate shutdown; databases are\n"
	"not saved.  This command should not be used unless\n"
	"damage to the in-memory copies of the databases is feared\n"
	"and they should not be saved.  For normal shutdowns, use the\n"
	"SHUTDOWN command."),
	/* OPER_HELP_SHUTDOWN */
	gettext("Syntax: SHUTDOWN\n"
	" \n"
	"Causes Services to save all databases and then shut down."),
	/* OPER_HELP_RESTART */
	gettext("Syntax: RESTART\n"
	" \n"
	"Causes Services to save all databases and then restart\n"
	"(i.e. exit and immediately re-run the executable)."),
	/* OPER_HELP_CHANLIST */
	gettext("Syntax: CHANLIST [{pattern | nick} [SECRET]]\n"
	" \n"
	"Lists all channels currently in use on the IRC network, whether they\n"
	"are registered or not.\n"
	"If pattern is given, lists only channels that match it. If a nickname\n"
	"is given, lists only the channels the user using it is on. If SECRET is\n"
	"specified, lists only channels matching pattern that have the +s or\n"
	"+p mode."),
	/* OPER_HELP_USERLIST */
	gettext("Syntax: USERLIST [{pattern | channel} [INVISIBLE]]\n"
	" \n"
	"Lists all users currently online on the IRC network, whether their\n"
	"nick is registered or not.\n"
	" \n"
	"If pattern is given, lists only users that match it (it must be in\n"
	"the format nick!user@host). If channel is given, lists only users\n"
	"that are on the given channel. If INVISIBLE is specified, only users\n"
	"with the +i flag will be listed."),
	/* OPER_HELP_MODLOAD */
	gettext("Syntax: MODLOAD FileName\n"
	" \n"
	"This command loads the module named FileName from the modules\n"
	"directory."),
	/* OPER_HELP_MODUNLOAD */
	gettext("Syntax: MODUNLOAD FileName\n"
	" \n"
	"This command unloads the module named FileName from the modules\n"
	"directory."),
	/* OPER_HELP_MODINFO */
	gettext("Syntax: MODINFO FileName\n"
	" \n"
	"This command lists information about the specified loaded module"),
	/* OPER_HELP_MODLIST */
	gettext("Syntax: MODLIST [Core|3rd|protocol|encryption|supported|qatested]\n"
	" \n"
	"Lists all currently loaded modules."),
	/* BOT_HELP_CMD_BOTLIST */
	gettext("    BOTLIST        Lists available bots"),
	/* BOT_HELP_CMD_ASSIGN */
	gettext("    ASSIGN         Assigns a bot to a channel"),
	/* BOT_HELP_CMD_SET */
	gettext("    SET            Configures bot options"),
	/* BOT_HELP_CMD_KICK */
	gettext("    KICK           Configures kickers"),
	/* BOT_HELP_CMD_BADWORDS */
	gettext("    BADWORDS       Maintains bad words list"),
	/* BOT_HELP_CMD_ACT */
	gettext("    ACT            Makes the bot do the equivalent of a \"/me\" command"),
	/* BOT_HELP_CMD_INFO */
	gettext("    INFO           Allows you to see BotServ information about a channel or a bot"),
	/* BOT_HELP_CMD_SAY */
	gettext("    SAY            Makes the bot say the given text on the given channel"),
	/* BOT_HELP_CMD_UNASSIGN */
	gettext("    UNASSIGN       Unassigns a bot from a channel"),
	/* BOT_HELP_CMD_BOT */
	gettext("    BOT            Maintains network bot list"),
	/* BOT_HELP */
	gettext("%S allows you to have a bot on your own channel.\n"
	"It has been created for users that can't host or\n"
	"configure a bot, or for use on networks that don't\n"
	"allow user bots. Available commands are listed \n"
	"below; to use them, type %R%S command.  For \n"
	"more information on a specific command, type %R\n"
	"%S HELP command."),
	/* BOT_HELP_FOOTER */
	gettext("Bot will join a channel whenever there is at least\n"
	"%d user(s) on it."),
	/* BOT_HELP_BOTLIST */
	gettext("Syntax: BOTLIST\n"
	" \n"
	"Lists all available bots on this network."),
	/* BOT_HELP_ASSIGN */
	gettext("Syntax: ASSIGN chan nick\n"
	" \n"
	"Assigns a bot pointed out by nick to the channel chan. You\n"
	"can then configure the bot for the channel so it fits\n"
	"your needs."),
	/* BOT_HELP_UNASSIGN */
	gettext("Syntax: UNASSIGN chan\n"
	" \n"
	"Unassigns a bot from a channel. When you use this command,\n"
	"the bot won't join the channel anymore. However, bot\n"
	"configuration for the channel is kept, so you will always\n"
	"be able to reassign a bot later without have to reconfigure\n"
	"it entirely."),
	/* BOT_HELP_INFO */
	gettext("Syntax: INFO {chan | nick}\n"
	" \n"
	"Allows you to see %S information about a channel or a bot.\n"
	"If the parameter is a channel, then you'll get information\n"
	"such as enabled kickers. If the parameter is a nick,\n"
	"you'll get information about a bot, such as creation\n"
	"time or number of channels it is on."),
	/* BOT_HELP_SET */
	gettext("Syntax: SET (channel | bot) option parameters\n"
	" \n"
	"Configures bot options.  option can be one of:\n"
	" \n"
	"    DONTKICKOPS      To protect ops against bot kicks\n"
	"    DONTKICKVOICES   To protect voices against bot kicks\n"
	"    GREET            Enable greet messages\n"
	"    FANTASY          Enable fantaisist commands\n"
	"    SYMBIOSIS        Allow the bot to act as a real bot\n"
	" \n"
	"Type %R%S HELP SET option for more information\n"
	"on a specific option.\n"
	"Note: access to this command is controlled by the\n"
	"level SET."),
	/* BOT_HELP_SET_DONTKICKOPS */
	gettext("Syntax: SET channel DONTKICKOPS {ON|OFF}\n"
	" \n"
	"Enables or disables ops protection mode on a channel.\n"
	"When it is enabled, ops won't be kicked by the bot\n"
	"even if they don't match the NOKICK level."),
	/* BOT_HELP_SET_DONTKICKVOICES */
	gettext("Syntax: SET channel DONTKICKVOICES {ON|OFF}\n"
	" \n"
	"Enables or disables voices protection mode on a channel.\n"
	"When it is enabled, voices won't be kicked by the bot\n"
	"even if they don't match the NOKICK level."),
	/* BOT_HELP_SET_FANTASY */
	gettext("Syntax: SET channel FANTASY {ON|OFF}\n"
	"Enables or disables fantasy mode on a channel.\n"
	"When it is enabled, users will be able to use\n"
	"commands !op, !deop, !voice, !devoice,\n"
	"!kick, !kb, !unban, !seen on a channel (find how \n"
	"to use them; try with or without nick for each, \n"
	"and with a reason for some?).\n"
	" \n"
	"Note that users wanting to use fantaisist\n"
	"commands MUST have enough level for both\n"
	"the FANTASIA and another level depending\n"
	"of the command if required (for example, to use \n"
	"!op, user must have enough access for the OPDEOP\n"
	"level)."),
	/* BOT_HELP_SET_GREET */
	gettext("Syntax: SET channel GREET {ON|OFF}\n"
	" \n"
	"Enables or disables greet mode on a channel.\n"
	"When it is enabled, the bot will display greet\n"
	"messages of users joining the channel, provided\n"
	"they have enough access to the channel."),
	/* BOT_HELP_SET_SYMBIOSIS */
	gettext("Syntax: SET channel SYMBIOSIS {ON|OFF}\n"
	" \n"
	"Enables or disables symbiosis mode on a channel.\n"
	"When it is enabled, the bot will do everything\n"
	"normally done by %s on channels, such as MODEs,\n"
	"KICKs, and even the entry message."),
	/* BOT_HELP_KICK */
	gettext("Syntax: KICK channel option parameters\n"
	" \n"
	"Configures bot kickers.  option can be one of:\n"
	" \n"
	"    BOLDS         Sets if the bot kicks bolds\n"
	"    BADWORDS      Sets if the bot kicks bad words\n"
	"    CAPS          Sets if the bot kicks caps\n"
	"    COLORS        Sets if the bot kicks colors\n"
	"    FLOOD         Sets if the bot kicks flooding users\n"
	"    REPEAT        Sets if the bot kicks users who repeat\n"
	"                       themselves\n"
	"    REVERSES      Sets if the bot kicks reverses\n"
	"    UNDERLINES    Sets if the bot kicks underlines\n"
	"    ITALICS       Sets if the bot kicks italics\n"
	" \n"
	"Type %R%S HELP KICK option for more information\n"
	"on a specific option.\n"
	" \n"
	"Note: access to this command is controlled by the\n"
	"level SET."),
	/* BOT_HELP_KICK_BOLDS */
	gettext("Syntax: KICK channel BOLDS {ON|OFF} [ttb]\n"
	"Sets the bolds kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who use bolds.\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_COLORS */
	gettext("Syntax: KICK channel COLORS {ON|OFF} [ttb]\n"
	"Sets the colors kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who use colors.\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_REVERSES */
	gettext("Syntax: KICK channel REVERSES {ON|OFF} [ttb]\n"
	"Sets the reverses kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who use reverses.\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_UNDERLINES */
	gettext("Syntax: KICK channel UNDERLINES {ON|OFF} [ttb]\n"
	"Sets the underlines kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who use underlines.\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_CAPS */
	gettext("Syntax: KICK channel CAPS {ON|OFF} [ttb [min [percent]]]\n"
	"Sets the caps kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who are talking in\n"
	"CAPS.\n"
	"The bot kicks only if there are at least min caps\n"
	"and they constitute at least percent%% of the total \n"
	"text line (if not given, it defaults to 10 characters\n"
	"and 25%%).\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_FLOOD */
	gettext("Syntax: KICK channel FLOOD {ON|OFF} [ttb [ln [secs]]]\n"
	"Sets the flood kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who are flooding\n"
	"the channel using at least ln lines in secs seconds\n"
	"(if not given, it defaults to 6 lines in 10 seconds).\n"
	" \n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_REPEAT */
	gettext("Syntax: KICK #channel REPEAT {ON|OFF} [ttb [num]]\n"
	"Sets the repeat kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who are repeating\n"
	"themselves num times (if num is not given, it\n"
	"defaults to 3).\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_BADWORDS */
	gettext("Syntax: KICK #channel BADWORDS {ON|OFF} [ttb]\n"
	"Sets the bad words kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who say certain words\n"
	"on the channels.\n"
	"You can define bad words for your channel using the\n"
	"BADWORDS command. Type %R%S HELP BADWORDS for\n"
	"more information.\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_ITALICS */
	gettext("Syntax: KICK channel ITALICS {ON|OFF} [ttb]\n"
	"Sets the italics kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who use italics.\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_BADWORDS */
	gettext("Syntax: BADWORDS channel ADD word [SINGLE | START | END]\n"
	"        BADWORDS channel DEL {word | entry-num | list}\n"
	"        BADWORDS channel LIST [mask | list]\n"
	"        BADWORDS channel CLEAR\n"
	" \n"
	"Maintains the bad words list for a channel. The bad\n"
	"words list determines which words are to be kicked\n"
	"when the bad words kicker is enabled. For more information,\n"
	"type %R%S HELP KICK BADWORDS.\n"
	" \n"
	"The BADWORDS ADD command adds the given word to the\n"
	"badword list. If SINGLE is specified, a kick will be\n"
	"done only if a user says the entire word. If START is \n"
	"specified, a kick will be done if a user says a word\n"
	"that starts with word. If END is specified, a kick\n"
	"will be done if a user says a word that ends with\n"
	"word. If you don't specify anything, a kick will\n"
	"be issued every time word is said by a user.\n"
	" \n"
	"The BADWORDS DEL command removes the given word from the\n"
	"bad words list.  If a list of entry numbers is given, those\n"
	"entries are deleted.  (See the example for LIST below.)\n"
	" \n"
	"The BADWORDS LIST command displays the bad words list.  If\n"
	"a wildcard mask is given, only those entries matching the\n"
	"mask are displayed.  If a list of entry numbers is given,\n"
	"only those entries are shown; for example:\n"
	"   BADWORDS #channel LIST 2-5,7-9\n"
	"      Lists bad words entries numbered 2 through 5 and\n"
	"      7 through 9.\n"
	"      \n"
	"The BADWORDS CLEAR command clears all entries of the\n"
	"bad words list."),
	/* BOT_HELP_SAY */
	gettext("Syntax: SAY channel text\n"
	" \n"
	"Makes the bot say the given text on the given channel."),
	/* BOT_HELP_ACT */
	gettext("Syntax: ACT channel text\n"
	" \n"
	"Makes the bot do the equivalent of a \"/me\" command\n"
	"on the given channel using the given text."),
	/* BOT_SERVADMIN_HELP_BOT */
	gettext("Syntax: BOT ADD nick user host real\n"
	"        BOT CHANGE oldnick newnick [user [host [real]]]\n"
	"        BOT DEL nick\n"
	" \n"
	"Allows Services Operators to create, modify, and delete\n"
	"bots that users will be able to use on their own\n"
	"channels.\n"
	" \n"
	"BOT ADD adds a bot with the given nickname, username,\n"
	"hostname and realname. Since no integrity checks are done \n"
	"for these settings, be really careful.\n"
	"BOT CHANGE allows to change nickname, username, hostname\n"
	"or realname of a bot without actually delete it (and all\n"
	"the data associated with it).\n"
	"BOT DEL removes the given bot from the bot list.  \n"
	" \n"
	"Note: you cannot create a bot that has a nick that is\n"
	"currently registered. If an unregistered user is currently\n"
	"using the nick, they will be killed."),
	/* BOT_SERVADMIN_HELP_SET */
	gettext("These options are reserved to Services Operators:\n"
	" \n"
	"    NOBOT            Prevent a bot from being assigned to \n"
	"                        a channel\n"
	"    PRIVATE          Prevent a bot from being assigned by\n"
	"                        non IRC operators"),
	/* BOT_SERVADMIN_HELP_SET_NOBOT */
	gettext("Syntax: SET channel NOBOT {ON|OFF}\n"
	" \n"
	"This option makes a channel be unassignable. If a bot \n"
	"is already assigned to the channel, it is unassigned\n"
	"automatically when you enable the option."),
	/* BOT_SERVADMIN_HELP_SET_PRIVATE */
	gettext("Syntax: SET bot-nick PRIVATE {ON|OFF}\n"
	"This option prevents a bot from being assigned to a\n"
	"channel by users that aren't IRC operators."),
	/* HOST_EMPTY */
	gettext("The vhost list is empty."),
	/* HOST_ENTRY */
	gettext("#%d Nick:%s, vhost:%s (%s - %s)"),
	/* HOST_IDENT_ENTRY */
	gettext("#%d Nick:%s, vhost:%s@%s (%s - %s)"),
	/* HOST_SET */
	gettext("vhost for %s set to %s."),
	/* HOST_IDENT_SET */
	gettext("vhost for %s set to %s@%s."),
	/* HOST_SETALL */
	gettext("vhost for group %s set to %s."),
	/* HOST_DELALL */
	gettext("vhosts for group %s have been removed."),
	/* HOST_DELALL_SYNTAX */
	gettext("DELALL <nick>."),
	/* HOST_IDENT_SETALL */
	gettext("vhost for group %s set to %s@%s."),
	/* HOST_SET_ERROR */
	gettext("A vhost must be in the format of a valid hostmask."),
	/* HOST_SET_IDENT_ERROR */
	gettext("A vhost ident must be in the format of a valid ident"),
	/* HOST_SET_TOOLONG */
	gettext("Error! The vhost is too long, please use a host shorter than %d characters."),
	/* HOST_SET_IDENTTOOLONG */
	gettext("Error! The Ident is too long, please use an ident shorter than %d characters."),
	/* HOST_NOREG */
	gettext("User %s not found in the nickserv db."),
	/* HOST_SET_SYNTAX */
	gettext("SET <nick> <hostmask>."),
	/* HOST_SETALL_SYNTAX */
	gettext("SETALL <nick> <hostmask>."),
	/* HOST_DENIED */
	gettext("Access Denied."),
	/* HOST_NOT_ASSIGNED */
	gettext("Please contact an Operator to get a vhost assigned to this nick."),
	/* HOST_ACTIVATED */
	gettext("Your vhost of %s is now activated."),
	/* HOST_IDENT_ACTIVATED */
	gettext("Your vhost of %s@%s is now activated."),
	/* HOST_DEL */
	gettext("vhost for %s removed."),
	/* HOST_DEL_SYNTAX */
	gettext("DEL <nick>."),
	/* HOST_OFF */
	gettext("Your vhost was removed and the normal cloaking restored."),
	/* HOST_NO_VIDENT */
	gettext("Your IRCD does not support vIdent's, if this is incorrect, please report this as a possible bug"),
	/* HOST_GROUP */
	gettext("All vhost's in the group %s have been set to %s"),
	/* HOST_IDENT_GROUP */
	gettext("All vhost's in the group %s have been set to %s@%s"),
	/* HOST_LIST_FOOTER */
	gettext("Displayed all records (Count: %d)"),
	/* HOST_LIST_RANGE_FOOTER */
	gettext("Displayed records from %d to %d"),
	/* HOST_LIST_KEY_FOOTER */
	gettext("Displayed records matching key %s (Count: %d)"),
	/* HOST_HELP_CMD_ON */
	gettext("    ON          Activates your assigned vhost"),
	/* HOST_HELP_CMD_OFF */
	gettext("    OFF         Deactivates your assigned vhost"),
	/* HOST_HELP_CMD_GROUP */
	gettext("    GROUP       Syncs the vhost for all nicks in a group"),
	/* HOST_HELP_CMD_SET */
	gettext("    SET         Set the vhost of another user"),
	/* HOST_HELP_CMD_SETALL */
	gettext("    SETALL      Set the vhost for all nicks in a group"),
	/* HOST_HELP_CMD_DEL */
	gettext("    DEL         Delete the vhost of another user"),
	/* HOST_HELP_CMD_DELALL */
	gettext("    DELALL      Delete the vhost for all nicks in a group"),
	/* HOST_HELP_CMD_LIST */
	gettext("    LIST        Displays one or more vhost entries."),
	/* HOST_HELP_ON */
	gettext("Syntax: ON\n"
	"Activates the vhost currently assigned to the nick in use.\n"
	"When you use this command any user who performs a /whois\n"
	"on you will see the vhost instead of your real IP address."),
	/* HOST_HELP_SET */
	gettext("Syntax: SET <nick> <hostmask>.\n"
	"Sets the vhost for the given nick to that of the given\n"
	"hostmask.  If your IRCD supports vIdents, then using\n"
	"SET <nick> <ident>@<hostmask> set idents for users as \n"
	"well as vhosts."),
	/* HOST_HELP_DELALL */
	gettext("Syntax: DELALL <nick>.\n"
	"Deletes the vhost for all nick's in the same group as\n"
	"that of the given nick."),
	/* HOST_HELP_SETALL */
	gettext("Syntax: SETALL <nick> <hostmask>.\n"
	"Sets the vhost for all nicks in the same group as that\n"
	"of the given nick.  If your IRCD supports vIdents, then\n"
	"using SETALL <nick> <ident>@<hostmask> will set idents\n"
	"for users as well as vhosts.\n"
	"* NOTE, this will not update the vhost for any nick's\n"
	"added to the group after this command was used."),
	/* HOST_HELP_OFF */
	gettext("Syntax: OFF\n"
	"Deactivates the vhost currently assigned to the nick in use.\n"
	"When you use this command any user who performs a /whois\n"
	"on you will see your real IP address."),
	/* HOST_HELP_DEL */
	gettext("Syntax: DEL <nick>\n"
	"Deletes the vhost assigned to the given nick from the\n"
	"database."),
	/* HOST_HELP_LIST */
	gettext("Syntax: LIST [<key>|<#X-Y>]\n"
	"This command lists registered vhosts to the operator\n"
	"if a Key is specified, only entries whos nick or vhost match\n"
	"the pattern given in <key> are displayed e.g. Rob* for all\n"
	"entries beginning with \"Rob\"\n"
	"If a #X-Y style is used, only entries between the range of X\n"
	"and Y will be displayed, e.g. #1-3 will display the first 3\n"
	"nick/vhost entries.\n"
	"The list uses the value of NSListMax as a hard limit for the\n"
	"number of items to display to a operator at any 1 time."),
	/* HOST_HELP_GROUP */
	gettext("Syntax: GROUP\n"
	" \n"
	"This command allows users to set the vhost of their\n"
	"CURRENT nick to be the vhost for all nicks in the same\n"
	"group."),
	/* OPER_SVSNICK_UNSUPPORTED */
	gettext("Sorry, SVSNICK is not available on this network."),
	/* OPER_SQLINE_UNSUPPORTED */
	gettext("Sorry, SQLINE is not available on this network."),
	/* OPER_SVSO_UNSUPPORTED */
	gettext("Sorry, OLINE is not available on this network."),
	/* OPER_UMODE_UNSUPPORTED */
	gettext("Sorry, UMODE is not available on this network."),
	/* OPER_SUPER_ADMIN_NOT_ENABLED */
	gettext("SuperAdmin setting not enabled in services.conf"),
	/* OPER_HELP_SYNC */
	gettext("Syntax: SQLSYNC\n"
	" \n"
	"This command syncs your databases with SQL. You should\n"
	"only have to execute this command once, when you initially\n"
	"import your databases into SQL."),
	/* OPER_HELP_CMD_SQLSYNC */
	gettext("    SQLSYNC     Import your databases to SQL"),
	/* OPER_SYNC_UPDATED */
	gettext("Updating MySQL.")
};
