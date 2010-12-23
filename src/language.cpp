#include "services.h"

#if GETTEXT_FOUND
# include LIBINTL
# define _(x) gettext(x)
#else
# define _(x) x
#endif

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

const Anope::string GetString(const Anope::string &language, LanguageString string)
{
	return GetString("anope", language, language_strings[string]);
}

const Anope::string GetString(LanguageString string)
{
	return GetString("anope", "", language_strings[string]);
}

const Anope::string GetString(NickCore *nc, LanguageString string)
{
	return GetString(nc ? nc->language : "", string);
}

const Anope::string GetString(User *u, LanguageString string)
{
	if (!u)
		return GetString(string);
	NickCore *nc = u->Account();
	NickAlias *na = NULL;
	if (!nc)
		na = findnick(u->nick);
	return GetString(nc ? nc : (na ? na->nc : NULL), string);
}

#if GETTEXT_FOUND
/* Used by gettext to make it always dynamically load language strings (so we can drop them in while Anope is running) */
extern "C" int _nl_msg_cat_cntr;
const Anope::string GetString(const char *domain, Anope::string language, const Anope::string &string)
{
	if (language == "en")
		language.clear();

	/* Apply def language */
	if (language.empty() && !Config->NSDefLanguage.empty())
		language = Config->NSDefLanguage;

	if (language.empty())
		return string;

	++_nl_msg_cat_cntr;
#ifdef _WIN32
	SetThreadLocale(MAKELCID(MAKELANGID(WindowsGetLanguage(language.c_str()), SUBLANG_DEFAULT), SORT_DEFAULT));
#else
	/* First, set LANGUAGE env variable.
	 * Some systems (Debian) don't care about this, so we must setlocale LC_ALL aswell.
	 * BUT if this call fails because the LANGUAGE env variable is set, setlocale resets
	 * the locale to "C", which short circuits gettext and causes it to fail on systems that
	 * use the LANGUAGE env variable. We must reset the locale to en_US (or, anything not
	 * C or POSIX) then.
	 */
	setenv("LANGUAGE", language.c_str(), 1);
	if (setlocale(LC_ALL, language.c_str()) == NULL)
		setlocale(LC_ALL, "en_US");
#endif
	const char *ret = dgettext(domain, string.c_str());
#ifdef _WIN32
	SetThreadLocale(MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), SORT_DEFAULT));
#else
	unsetenv("LANGUAGE");
	setlocale(LC_ALL, "");
#endif

	Anope::string translated = ret ? ret : "";

	if (Config->UseStrictPrivMsg)
		translated = translated.replace_all_cs("%R", "/");
	else
		translated = translated.replace_all_cs("%R", "/msg ");
	
	return translated;
}
#else
const Anope::string GetString(const char *domain, Anope::string language, const Anope::string &string)
{
	Anope::string translated = string;
	if (Config->UseStrictPrivMsg)
		translated = translated.replace_all_cs("%R", "/");
	else
		translated = translated.replace_all_cs("%R", "/msg ");
	return translated;
}
#endif

void SyntaxError(CommandSource &source, const Anope::string &command, LanguageString message)
{
	if (command.empty())
		return;

	Anope::string str = GetString(source.u, message);
	source.Reply(SYNTAX_ERROR, str.c_str());
	source.Reply(MORE_INFO, source.owner->nick.c_str(), command.c_str());
}

const char *const language_strings[LANG_STRING_COUNT] = {
	/* LANGUAGE_NAME */
	_("English"),
	/* COMMAND_REQUIRES_PERM */
	_("Access to this command requires the permission %s to be present in your opertype."),
	/* COMMAND_IDENTIFY_REQUIRED */
	_("You need to be identified to use this command."),
	/* COMMAND_CANNOT_USE */
	_("You cannot use this command."),
	/* COMMAND_CAN_USE */
	_("You can use this command."),
	/* COMMAND_DEPRECATED */
	_("This command is deprecated, use \"%R%s MODE\" instead."),
	/* USER_RECORD_NOT_FOUND */
	_("Internal error - unable to process request."),
	/* UNKNOWN_COMMAND */
	_("Unknown command %s."),
	/* UNKNOWN_COMMAND_HELP */
	_("Unknown command %s.  \"%R%s HELP\" for help."),
	/* SYNTAX_ERROR */
	_("Syntax: %s"),
	/* MORE_INFO */
	_("%R%s HELP %s for more information."),
	/* NO_HELP_AVAILABLE */
	_("No help available for %s."),
	/* BAD_USERHOST_MASK */
	_("Mask must be in the form user@host."),
	/* BAD_EXPIRY_TIME */
	_("Invalid expiry time."),
	/* USERHOST_MASK_TOO_WIDE */
	_("%s coverage is too wide; Please use a more specific mask."),
	/* SERVICE_OFFLINE */
	_("%s is currently offline."),
	/* READ_ONLY_MODE */
	_("Notice: Services is in read-only mode; changes will not be saved!"),
	/* PASSWORD_INCORRECT */
	_("Password incorrect."),
	/* INVALID_TARGET */
	_("\"/msg %s\" is no longer supported.  Use \"/msg %s@%s\" or \"/%s\" instead."),
	/* ACCESS_DENIED */
	_("Access denied."),
	/* MORE_OBSCURE_PASSWORD */
	_("Please try again with a more obscure password.  Passwords should be at least five characters long, should not be something easily guessed (e.g. your real name or your nick), and cannot contain the space or tab characters."),
	/* PASSWORD_TOO_LONG */
	_("Your password is too long. Please try again with a shorter password."),
	/* NICK_NOT_REGISTERED */
	_("Your nick isn't registered."),
	/* NICK_X_IS_SERVICES */
	_("Nick %s is part of this Network's Services."),
	/* NICK_X_NOT_REGISTERED */
	_("Nick %s isn't registered."),
	/* NICK_X_IN_USE */
	_("Nick %s is currently in use."),
	/* NICK_X_NOT_IN_USE */
	_("Nick %s isn't currently in use."),
	/* NICK_X_NOT_ON_CHAN */
	_("%s is not currently on channel %s."),
	/* NICK_X_FORBIDDEN */
	_("Nick %s may not be registered or used."),
	/* NICK_X_FORBIDDEN_OPER */
	_("Nick %s has been forbidden by %s:\n"
	"%s"),
	/* NICK_X_ILLEGAL */
	_("Nick %s is an illegal nickname and cannot be used."),
	/* NICK_X_TRUNCATED */
	_("Nick %s was truncated to %d characters."),
	/* NICK_X_SUSPENDED */
	_("Nick %s is currently suspended."),
	/* CHAN_X_NOT_REGISTERED */
	_("Channel %s isn't registered."),
	/* CHAN_X_NOT_IN_USE */
	_("Channel %s doesn't exist."),
	/* CHAN_X_FORBIDDEN */
	_("Channel %s may not be registered or used."),
	/* CHAN_X_FORBIDDEN_OPER */
	_("Channel %s has been forbidden by %s:\n"
	"%s"),
	/* CHAN_X_SUSPENDED */
	_("      Suspended: [%s] %s"),
	/* NICK_IDENTIFY_REQUIRED */
	_("Password authentication required for that command.\n"
	"Retry after typing %R%s IDENTIFY password."),
	/* MAIL_DISABLED */
	_("Services have been configured to not send mail."),
	/* MAIL_INVALID */
	_("E-mail for %s is invalid."),
	/* MAIL_X_INVALID */
	_("%s is not a valid e-mail address."),
	/* MAIL_LATER */
	_("Cannot send mail now; please retry a little later."),
	/* MAIL_DELAYED */
	_("Please wait %d seconds and retry."),
	/* NO_REASON */
	_("No reason"),
	/* UNKNOWN */
	_("<unknown>"),
	/* DURATION_DAY */
	_("1 day"),
	/* DURATION_DAYS */
	_("%d days"),
	/* DURATION_HOUR */
	_("1 hour"),
	/* DURATION_HOURS */
	_("%d hours"),
	/* DURATION_MINUTE */
	_("1 minute"),
	/* DURATION_MINUTES */
	_("%d minutes"),
	/* DURATION_SECOND */
	_("1 second"),
	/* DURATION_SECONDS */
	_("%d seconds"),
	/* NO_EXPIRE */
	_("does not expire"),
	/* EXPIRES_SOON */
	_("expires at next database update"),
	/* EXPIRES_M */
	_("expires in %d minutes"),
	/* EXPIRES_1M */
	_("expires in %d minute"),
	/* EXPIRES_HM */
	_("expires in %d hours, %d minutes"),
	/* EXPIRES_H1M */
	_("expires in %d hours, %d minute"),
	/* EXPIRES_1HM */
	_("expires in %d hour, %d minutes"),
	/* EXPIRES_1H1M */
	_("expires in %d hour, %d minute"),
	/* EXPIRES_D */
	_("expires in %d days"),
	/* EXPIRES_1D */
	_("expires in %d day"),
	/* END_OF_ANY_LIST */
	_("End of %s list."),
	/* LIST_INCORRECT_RANGE */
	_("Incorrect range specified. The correct syntax is #from-to."),
	/* CS_LIST_INCORRECT_RANGE */
	_("To search for channels starting with #, search for the channel\n"
	"name without the #-sign prepended (anope instead of #anope)."),
	/* NICK_IS_REGISTERED */
	_("This nick is owned by someone else.  Please choose another.\n"
	"(If this is your nick, type %R%s IDENTIFY password.)"),
	/* NICK_IS_SECURE */
	_("This nickname is registered and protected.  If it is your\n"
	"nick, type %R%s IDENTIFY password.  Otherwise,\n"
	"please choose a different nick."),
	/* NICK_MAY_NOT_BE_USED */
	_("This nickname may not be used.  Please choose another one."),
	/* FORCENICKCHANGE_IN_1_MINUTE */
	_("If you do not change within one minute, I will change your nick."),
	/* FORCENICKCHANGE_IN_20_SECONDS */
	_("If you do not change within 20 seconds, I will change your nick."),
	/* FORCENICKCHANGE_NOW */
	_("This nickname has been registered; you may not use it."),
	/* FORCENICKCHANGE_CHANGING */
	_("Your nickname is now being changed to %s"),
	/* NICK_REGISTER_SYNTAX */
	_("REGISTER password [email]"),
	/* NICK_REGISTER_SYNTAX_EMAIL */
	_("REGISTER password email"),
	/* NICK_REGISTRATION_DISABLED */
	_("Sorry, nickname registration is temporarily disabled."),
	/* NICK_REGISTRATION_FAILED */
	_("Sorry, registration failed."),
	/* NICK_REG_PLEASE_WAIT */
	_("Please wait %d seconds before using the REGISTER command again."),
	/* NICK_CANNOT_BE_REGISTERED */
	_("Nickname %s may not be registered."),
	/* NICK_ALREADY_REGISTERED */
	_("Nickname %s is already registered!"),
	/* NICK_REGISTERED */
	_("Nickname %s registered under your account: %s"),
	/* NICK_REGISTERED_NO_MASK */
	_("Nickname %s registered."),
	/* NICK_PASSWORD_IS */
	_("Your password is %s - remember this for later use."),
	/* NICK_REG_DELAY */
	_("You must have been using this nick for at least %d seconds to register."),
	/* NICK_GROUP_SYNTAX */
	_("GROUP target password"),
	/* NICK_GROUP_DISABLED */
	_("Sorry, nickname grouping is temporarily disabled."),
	/* NICK_GROUP_PLEASE_WAIT */
	_("Please wait %d seconds before using the GROUP command again."),
	/* NICK_GROUP_CHANGE_DISABLED */
	_("Your nick is already registered; type \002%R%s DROP\002 first."),
	/* NICK_GROUP_SAME */
	_("You are already a member of the group of %s."),
	/* NICK_GROUP_TOO_MANY */
	_("There are too many nicks in %s's group; list them and drop some.\n"
	"Type %R%s HELP GLIST and %R%s HELP DROP\n"
	"for more information."),
	/* NICK_GROUP_JOINED */
	_("You are now in the group of %s."),
	/* NICK_UNGROUP_ONE_NICK */
	_("Your nick is not grouped to anything, you can't ungroup it."),
	/* NICK_UNGROUP_NOT_IN_GROUP */
	_("The nick %s is not in your group."),
	/* NICK_UNGROUP_SUCCESSFUL */
	_("Nick %s has been ungrouped from %s."),
	/* NICK_IDENTIFY_SYNTAX */
	_("IDENTIFY [account] password"),
	/* NICK_IDENTIFY_FAILED */
	_("Sorry, identification failed."),
	/* NICK_IDENTIFY_SUCCEEDED */
	_("Password accepted - you are now recognized."),
	/* NICK_IDENTIFY_EMAIL_REQUIRED */
	_("You must now supply an e-mail for your nick.\n"
	"This e-mail will allow you to retrieve your password in\n"
	"case you forget it."),
	/* NICK_IDENTIFY_EMAIL_HOWTO */
	_("Type %R%S SET EMAIL e-mail in order to set your e-mail.\n"
	"Your privacy is respected; this e-mail won't be given to\n"
	"any third-party person."),
	/* NICK_ALREADY_IDENTIFIED */
	_("You are already identified."),
	/* NICK_UPDATE_SUCCESS */
	_("Status updated (memos, vhost, chmodes, flags)."),
	/* NICK_LOGOUT_SYNTAX */
	_("LOGOUT"),
	/* NICK_LOGOUT_SUCCEEDED */
	_("Your nick has been logged out."),
	/* NICK_LOGOUT_X_SUCCEEDED */
	_("Nick %s has been logged out."),
	/* NICK_LOGOUT_SERVICESADMIN */
	_("Can't logout %s because he's a Services Operator."),
	/* NICK_DROP_DISABLED */
	_("Sorry, nickname de-registration is temporarily disabled."),
	/* NICK_DROPPED */
	_("Your nickname has been dropped."),
	/* NICK_X_DROPPED */
	_("Nickname %s has been dropped."),
	/* NICK_SET_SYNTAX */
	_("SET option parameters"),
	/* NICK_SET_DISABLED */
	_("Sorry, nickname option setting is temporarily disabled."),
	/* NICK_SET_UNKNOWN_OPTION */
	_("Unknown SET option %s."),
	/* NICK_SET_OPTION_DISABLED */
	_("Option %s cannot be set on this network."),
	/* NICK_SET_DISPLAY_CHANGED */
	_("The new display is now %s."),
	/* NICK_SET_LANGUAGE_SYNTAX */
	_("SET LANGUAGE language"),
	/* NICK_SET_LANGUAGE_CHANGED */
	_("Language changed to English."),
	/* NICK_SET_EMAIL_UNSET */
	_("E-mail address unset."),
	/* NICK_SET_EMAIL_UNSET_IMPOSSIBLE */
	_("You cannot unset the e-mail on this network."),
	/* NICK_SET_KILL_SYNTAX */
	_("SET KILL {ON | QUICK | OFF}"),
	/* NICK_SET_KILL_IMMED_SYNTAX */
	_("SET KILL {ON | QUICK | IMMED | OFF}"),
	/* NICK_SET_KILL_IMMED */
	_("Protection is now ON, with no delay."),
	/* NICK_SET_KILL_IMMED_DISABLED */
	_("The IMMED option is not available on this network."),
	/* NICK_SET_SECURE_SYNTAX */
	_("SET SECURE {ON | OFF}"),
	/* NICK_SET_PRIVATE_SYNTAX */
	_("SET PRIVATE {ON | OFF}"),
	/* NICK_SET_HIDE_SYNTAX */
	_("SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}"),
	/* NICK_SET_MSG_SYNTAX */
	_("SET MSG {ON | OFF}"),
	/* NICK_SET_AUTOOP_SYNTAX */
	_("SET AUTOOP {ON | OFF}"),
	/* NICK_SASET_SYNTAX */
	_("SASET nickname option parameters"),
	/* NICK_SASET_UNKNOWN_OPTION */
	_("Unknown SASET option %s."),
	/* NICK_SASET_BAD_NICK */
	_("Nickname %s not registered."),
	/* NICK_SASET_DISPLAY_INVALID */
	_("The new display for %s MUST be a nickname of the nickname group!"),
	/* NICK_SASET_PASSWORD_FAILED */
	_("Sorry, couldn't change password for %s."),
	/* NICK_SASET_PASSWORD_CHANGED */
	_("Password for %s changed."),
	/* NICK_SASET_PASSWORD_CHANGED_TO */
	_("Password for %s changed to %s."),
	/* NICK_SASET_EMAIL_CHANGED */
	_("E-mail address for %s changed to %s."),
	/* NICK_SASET_EMAIL_UNSET */
	_("E-mail address for %s unset."),
	/* NICK_SASET_GREET_CHANGED */
	_("Greet message for %s changed to %s."),
	/* NICK_SASET_GREET_UNSET */
	_("Greet message for %s unset."),
	/* NICK_SASET_KILL_SYNTAX */
	_("SASET nickname KILL {ON | QUICK | OFF}"),
	/* NICK_SASET_KILL_IMMED_SYNTAX */
	_("SASET nickname KILL {ON | QUICK | IMMED | OFF}"),
	/* NICK_SASET_KILL_ON */
	_("Protection is now ON for %s."),
	/* NICK_SASET_KILL_QUICK */
	_("Protection is now ON for %s, with a reduced delay."),
	/* NICK_SASET_KILL_IMMED */
	_("Protection is now ON for %s, with no delay."),
	/* NICK_SASET_KILL_OFF */
	_("Protection is now OFF for %s."),
	/* NICK_SASET_SECURE_SYNTAX */
	_("SASET nickname SECURE {ON | OFF}"),
	/* NICK_SASET_SECURE_ON */
	_("Secure option is now ON for %s."),
	/* NICK_SASET_SECURE_OFF */
	_("Secure option is now OFF for %s."),
	/* NICK_SASET_PRIVATE_SYNTAX */
	_("SASET nickname PRIVATE {ON | OFF}"),
	/* NICK_SASET_PRIVATE_ON */
	_("Private option is now ON for %s."),
	/* NICK_SASET_PRIVATE_OFF */
	_("Private option is now OFF for %s."),
	/* NICK_SASET_HIDE_SYNTAX */
	_("SASET nickname HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}"),
	/* NICK_SASET_HIDE_EMAIL_ON */
	_("The E-mail address of %s will now be hidden from %s INFO displays."),
	/* NICK_SASET_HIDE_EMAIL_OFF */
	_("The E-mail address of %s will now be shown in %s INFO displays."),
	/* NICK_SASET_HIDE_MASK_ON */
	_("The last seen user@host mask of %s will now be hidden from %s INFO displays."),
	/* NICK_SASET_HIDE_MASK_OFF */
	_("The last seen user@host mask of %s will now be shown in %s INFO displays."),
	/* NICK_SASET_HIDE_QUIT_ON */
	_("The last quit message of %s will now be hidden from %s INFO displays."),
	/* NICK_SASET_HIDE_QUIT_OFF */
	_("The last quit message of %s will now be shown in %s INFO displays."),
	/* NICK_SASET_HIDE_STATUS_ON */
	_("The services access status of %s will now be hidden from %s INFO displays."),
	/* NICK_SASET_HIDE_STATUS_OFF */
	_("The services access status of %s will now be shown in %s INFO displays."),
	/* NICK_SASET_MSG_SYNTAX */
	_("SASAET nickname PRIVATE {ON | OFF}"),
	/* NICK_SASET_MSG_ON */
	_("Services will now reply to %s with messages."),
	/* NICK_SASET_MSG_OFF */
	_("Services will now reply to %s with notices."),
	/* NICK_SASET_NOEXPIRE_SYNTAX */
	_("SASET nickname NOEXPIRE {ON | OFF}"),
	/* NICK_SASET_NOEXPIRE_ON */
	_("Nick %s will not expire."),
	/* NICK_SASET_NOEXPIRE_OFF */
	_("Nick %s will expire."),
	/* NICK_SASET_AUTOOP_SYNTAX */
	_("SASET nickname AUTOOP {ON | OFF}"),
	/* NICK_SASET_AUTOOP_ON */
	_("Services will now autoop %s in channels."),
	/* NICK_SASET_AUTOOP_OFF */
	_("Services will no longer autoop %s in channels."),
	/* NICK_SASET_LANGUAGE_SYNTAX */
	_("SASET nickname LANGUAGE number"),
	/* NICK_ACCESS_SYNTAX */
	_("ACCESS {ADD | DEL | LIST} [mask]"),
	/* NICK_ACCESS_ALREADY_PRESENT */
	_("Mask %s already present on your access list."),
	/* NICK_ACCESS_REACHED_LIMIT */
	_("Sorry, you can only have %d access entries for a nickname."),
	/* NICK_ACCESS_ADDED */
	_("%s added to your access list."),
	/* NICK_ACCESS_NOT_FOUND */
	_("%s not found on your access list."),
	/* NICK_ACCESS_DELETED */
	_("%s deleted from your access list."),
	/* NICK_ACCESS_LIST */
	_("Access list:"),
	/* NICK_ACCESS_LIST_X */
	_("Access list for %s:"),
	/* NICK_ACCESS_LIST_EMPTY */
	_("Your access list is empty."),
	/* NICK_ACCESS_LIST_X_EMPTY */
	_("Access list for %s is empty."),
	/* NICK_STATUS_REPLY */
	_("STATUS %s %d %s"),
	/* NICK_INFO_SYNTAX */
	_("INFO nick"),
	/* NICK_INFO_REALNAME */
	_("%s is %s"),
	/* NICK_INFO_SERVICES_OPERTYPE */
	_("%s is a services operator of type %s."),
	/* NICK_INFO_ADDRESS */
	_("Last seen address: %s"),
	/* NICK_INFO_ADDRESS_ONLINE */
	_("   Is online from: %s"),
	/* NICK_INFO_ADDRESS_ONLINE_NOHOST */
	_("%s is currently online."),
	/* NICK_INFO_TIME_REGGED */
	_("  Time registered: %s"),
	/* NICK_INFO_LAST_SEEN */
	_("   Last seen time: %s"),
	/* NICK_INFO_LAST_QUIT */
	_("Last quit message: %s"),
	/* NICK_INFO_EMAIL */
	_("   E-mail address: %s"),
	/* NICK_INFO_VHOST */
	_("            vhost: %s"),
	/* NICK_INFO_VHOST2 */
	_("            vhost: %s@%s"),
	/* NICK_INFO_GREET */
	_("    Greet message: %s"),
	/* NICK_INFO_OPTIONS */
	_("          Options: %s"),
	/* NICK_INFO_EXPIRE */
	_("Expires on: %s"),
	/* NICK_INFO_OPT_KILL */
	_("Protection"),
	/* NICK_INFO_OPT_SECURE */
	_("Security"),
	/* NICK_INFO_OPT_PRIVATE */
	_("Private"),
	/* NICK_INFO_OPT_MSG */
	_("Message mode"),
	/* NICK_INFO_OPT_AUTOOP */
	_("Auto-op"),
	/* NICK_INFO_OPT_NONE */
	_("None"),
	/* NICK_INFO_NO_EXPIRE */
	_("This nickname will not expire."),
	/* NICK_INFO_SUSPENDED */
	_("This nickname is currently suspended, reason: %s"),
	/* NICK_INFO_SUSPENDED_NO_REASON */
	_("This nickname is currently suspended"),
	/* NICK_LIST_SYNTAX */
	_("LIST pattern"),
	/* NICK_LIST_SERVADMIN_SYNTAX */
	_("LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]"),
	/* NICK_LIST_HEADER */
	_("List of entries matching %s:"),
	/* NICK_LIST_RESULTS */
	_("End of list - %d/%d matches shown."),
	/* NICK_ALIST_HEADER */
	_("Channels that you have access on:\n"
	"  Num  Channel              Level    Description"),
	/* NICK_ALIST_HEADER_X */
	_("Channels that %s has access on:\n"
	"  Num  Channel              Level    Description"),
	/* NICK_ALIST_XOP_FORMAT */
	_("  %3d %c%-20s %-8s %s"),
	/* NICK_ALIST_ACCESS_FORMAT */
	_("  %3d %c%-20s %-8d %s"),
	/* NICK_ALIST_FOOTER */
	_("End of list - %d/%d channels shown."),
	/* NICK_GLIST_HEADER */
	_("List of nicknames in your group:"),
	/* NICK_GLIST_HEADER_X */
	_("List of nicknames in the group of %s:"),
	/* NICK_GLIST_FOOTER */
	_("%d nicknames in the group."),
	/* NICK_GLIST_REPLY */
	_("   %s (expires in %s)"),
	/* NICK_GLIST_REPLY_NOEXPIRE */
	_("   %s (does not expire)"),
	/* NICK_RECOVER_SYNTAX */
	_("RECOVER nickname [password]"),
	/* NICK_NO_RECOVER_SELF */
	_("You can't recover yourself!"),
	/* NICK_RECOVERED */
	_("User claiming your nick has been killed.\n"
	"%R%s RELEASE %s to get it back before %s timeout."),
	/* NICK_RELEASE_SYNTAX */
	_("RELEASE nickname [password]"),
	/* NICK_RELEASE_NOT_HELD */
	_("Nick %s isn't being held."),
	/* NICK_RELEASED */
	_("Services' hold on your nick has been released."),
	/* NICK_GHOST_SYNTAX */
	_("GHOST nickname [password]"),
	/* NICK_NO_GHOST_SELF */
	_("You can't ghost yourself!"),
	/* NICK_GHOST_KILLED */
	_("Ghost with your nick has been killed."),
	/* NICK_GHOST_UNIDENTIFIED */
	_("You may not ghost an unidentified user, use RECOVER instead."),
	/* NICK_GETPASS_SYNTAX */
	_("GETPASS nickname"),
	/* NICK_GETPASS_UNAVAILABLE */
	_("GETPASS command unavailable because encryption is in use."),
	/* NICK_GETPASS_PASSWORD_IS */
	_("Password for %s is %s."),
	/* NICK_GETEMAIL_SYNTAX */
	_("GETEMAIL user@email-host No WildCards!!"),
	/* NICK_GETEMAIL_EMAILS_ARE */
	_("Emails Match %s to %s."),
	/* NICK_GETEMAIL_NOT_USED */
	_("No Emails listed for %s."),
	/* NICK_SENDPASS_SYNTAX */
	_("SENDPASS nickname"),
	/* NICK_SENDPASS_UNAVAILABLE */
	_("SENDPASS command unavailable because encryption is in use."),
	/* NICK_SENDPASS_SUBJECT */
	_("Nickname password (%s)"),
	/* NICK_SENDPASS */
	_("Hi,\n"
	" \n"
	"You have requested to receive the password of nickname %s by e-mail.\n"
	"The password is %s. For security purposes, you should change it as soon as you receive this mail.\n"
	" \n"
	"If you don't know why this mail was sent to you, please ignore it silently.\n"
	" \n"
	"%s administrators."),
	/* NICK_SENDPASS_OK */
	_("Password of %s has been sent."),
	/* NICK_RESETPASS_SYNTAX */
	_("RESETPASS nickname"),
	/* NICK_RESETPASS_SUBJECT */
	_("Reset password request for %s"),
	/* NICK_RESETPASS_MESSAGE */
	_("Hi,\n"
	" \n"
	"You have requested to have the password for %s reset.\n"
	"To reset your password, type %R%s CONFIRM %s\n"
	" \n"
	"If you don't know why this mail was sent to you, please ignore it silently.\n"
	" \n"
	"%s administrators."),
	/* NICK_RESETPASS_COMPLETE */
	_("Password reset email for %s has been sent."),
	/* NICK_SUSPEND_SYNTAX */
	_("SUSPEND nickname reason"),
	/* NICK_SUSPEND_SUCCEEDED */
	_("Nick %s is now suspended."),
	/* NICK_UNSUSPEND_SYNTAX */
	_("UNSUSPEND nickname"),
	/* NICK_UNSUSPEND_SUCCEEDED */
	_("Nick %s is now released."),
	/* NICK_FORBID_SYNTAX */
	_("FORBID nickname [reason]"),
	/* NICK_FORBID_SYNTAX_REASON */
	_("FORBID nickname reason"),
	/* NICK_FORBID_SUCCEEDED */
	_("Nick %s is now forbidden."),
	/* NICK_REQUESTED */
	_("This nick has already been requested, please check your e-mail address for the pass code"),
	/* NICK_REG_RESENT */
	_("Your passcode has been re-sent to %s."),
	/* NICK_REG_UNABLE */
	_("Nick NOT registered, please try again later."),
	/* NICK_IS_PREREG */
	_("This nick is awaiting an e-mail verification code before completing registration."),
	/* NICK_ENTER_REG_CODE */
	_("A passcode has been sent to %s, please type %R%s confirm <passcode> to complete registration.\n"
	  "If you need to cancel your registration, use \"%R%s drop <password>\"."),
	/* NICK_GETPASS_PASSCODE_IS */
	_("Passcode for %s is %s."),
	/* NICK_FORCE_REG */
	_("Nickname %s confirmed"),
	/* NICK_REG_MAIL_SUBJECT */
	_("Nickname Registration (%s)"),
	/* NICK_REG_MAIL */
	_("Hi,\n"
	" \n"
	"You have requested to register the nickname %s on %s.\n"
	"Please type \" %R%s confirm %s \" to complete registration.\n"
	" \n"
	"If you don't know why this mail was sent to you, please ignore it silently.\n"
	" \n"
	"%s administrators."),
	/* NICK_CONFIRM_NOT_FOUND */
	_("Registration step 1 may have expired, please use \"%R%s register <password> <email>\" first."),
	/* NICK_CONFIRM_INVALID */
	_("Invalid passcode has been entered, please check the e-mail again, and retry"),
	/* NICK_CONFIRM_EXPIRED */
	_("Your password reset request has expired."),
	/* NICK_CONFIRM_SUCCESS */
	_("You are now identified for your nick. Change your password using \"%R%s SET PASSWORD newpassword\" now."),
	/* CHAN_LEVEL_AUTOOP */
	_("Automatic channel operator status"),
	/* CHAN_LEVEL_AUTOVOICE */
	_("Automatic mode +v"),
	/* CHAN_LEVEL_AUTOHALFOP */
	_("Automatic mode +h"),
	/* CHAN_LEVEL_AUTOPROTECT */
	_("Automatic mode +a"),
	/* CHAN_LEVEL_NOJOIN */
	_("Not allowed to join channel"),
	/* CHAN_LEVEL_INVITE */
	_("Allowed to use INVITE command"),
	/* CHAN_LEVEL_AKICK */
	_("Allowed to use AKICK command"),
	/* CHAN_LEVEL_SET */
	_("Allowed to use SET command (not FOUNDER/PASSWORD)"),
	/* CHAN_LEVEL_CLEARUSERS */
	_("Allowed to use CLEARUSERS command"),
	/* CHAN_LEVEL_UNBAN */
	_("Allowed to use UNBAN command"),
	/* CHAN_LEVEL_OPDEOP */
	_("Allowed to use OP/DEOP commands"),
	/* CHAN_LEVEL_ACCESS_LIST */
	_("Allowed to view the access list"),
	/* CHAN_LEVEL_ACCESS_CHANGE */
	_("Allowed to modify the access list"),
	/* CHAN_LEVEL_MEMO */
	_("Allowed to list/read channel memos"),
	/* CHAN_LEVEL_ASSIGN */
	_("Allowed to assign/unassign a bot"),
	/* CHAN_LEVEL_BADWORDS */
	_("Allowed to use BADWORDS command"),
	/* CHAN_LEVEL_NOKICK */
	_("Never kicked by the bot's kickers"),
	/* CHAN_LEVEL_FANTASIA */
	_("Allowed to use fantaisist commands"),
	/* CHAN_LEVEL_SAY */
	_("Allowed to use SAY and ACT commands"),
	/* CHAN_LEVEL_GREET */
	_("Greet message displayed"),
	/* CHAN_LEVEL_VOICEME */
	_("Allowed to (de)voice him/herself"),
	/* CHAN_LEVEL_VOICE */
	_("Allowed to use VOICE/DEVOICE commands"),
	/* CHAN_LEVEL_GETKEY */
	_("Allowed to use GETKEY command"),
	/* CHAN_LEVEL_OPDEOPME */
	_("Allowed to (de)op him/herself"),
	/* CHAN_LEVEL_HALFOPME */
	_("Allowed to (de)halfop him/herself"),
	/* CHAN_LEVEL_HALFOP */
	_("Allowed to use HALFOP/DEHALFOP commands"),
	/* CHAN_LEVEL_PROTECTME */
	_("Allowed to (de)protect him/herself"),
	/* CHAN_LEVEL_PROTECT */
	_("Allowed to use PROTECT/DEPROTECT commands"),
	/* CHAN_LEVEL_KICKME */
	_("Allowed to kick him/herself"),
	/* CHAN_LEVEL_KICK */
	_("Allowed to use KICK command"),
	/* CHAN_LEVEL_SIGNKICK */
	_("No signed kick when SIGNKICK LEVEL is used"),
	/* CHAN_LEVEL_BANME */
	_("Allowed to ban him/herself"),
	/* CHAN_LEVEL_BAN */
	_("Allowed to use BAN command"),
	/* CHAN_LEVEL_TOPIC */
	_("Allowed to use TOPIC command"),
	/* CHAN_LEVEL_MODE */
	_("Allowed to use MODE command"),
	/* CHAN_LEVEL_INFO */
	_("Allowed to use INFO command with ALL option"),
	/* CHAN_LEVEL_AUTOOWNER */
	_("Automatic mode +q"),
	/* CHAN_LEVEL_OWNER */
	_("Allowed to use OWNER command"),
	/* CHAN_LEVEL_OWNERME */
	_("Allowed to (de)owner him/herself"),
	/* CHAN_LEVEL_FOUNDER */
	_("Allowed to issue commands restricted to channel founders"),
	/* CHAN_IS_REGISTERED */
	_("This channel has been registered with %s."),
	/* CHAN_MAY_NOT_BE_USED */
	_("This channel may not be used."),
	/* CHAN_NOT_ALLOWED_TO_JOIN */
	_("You are not permitted to be on this channel."),
	/* CHAN_X_INVALID */
	_("Channel %s is not a valid channel."),
	/* CHAN_REGISTER_SYNTAX */
	_("REGISTER channel description"),
	/* CHAN_REGISTER_DISABLED */
	_("Sorry, channel registration is temporarily disabled."),
	/* CHAN_REGISTER_NOT_LOCAL */
	_("Local channels cannot be registered."),
	/* CHAN_MAY_NOT_BE_REGISTERED */
	_("Channel %s may not be registered."),
	/* CHAN_ALREADY_REGISTERED */
	_("Channel %s is already registered!"),
	/* CHAN_MUST_BE_CHANOP */
	_("You must be a channel operator to register the channel."),
	/* CHAN_REACHED_CHANNEL_LIMIT */
	_("Sorry, you have already reached your limit of %d channels."),
	/* CHAN_EXCEEDED_CHANNEL_LIMIT */
	_("Sorry, you have already exceeded your limit of %d channels."),
	/* CHAN_REGISTERED */
	_("Channel %s registered under your nickname: %s"),
	/* CHAN_SYMBOL_REQUIRED */
	_("Please use the symbol of # when attempting to register"),
	/* CHAN_DROP_SYNTAX */
	_("DROP channel"),
	/* CHAN_DROP_DISABLED */
	_("Sorry, channel de-registration is temporarily disabled."),
	/* CHAN_DROPPED */
	_("Channel %s has been dropped."),
	/* CHAN_SASET_SYNTAX */
	_("SASET channel option parameters"),
	/* CHAN_SASET_KEEPTOPIC_SYNTAX */
	_("SASET channel KEEPTOPIC {ON | OFF}"),
	/* CHAN_SASET_OPNOTICE_SYNTAX */
	_("SASET channel OPNOTICE {ON | OFF}"),
	/* CHAN_SASET_PEACE_SYNTAX */
	_("SASET channel PEACE {ON | OFF}"),
	/* CHAN_SASET_PERSIST_SYNTAX */
	_("SASET channel PERSIST {ON | OFF}"),
	/* CHAN_SASET_PRIVATE_SYNTAX */
	_("SASET channel PRIVATE {ON | OFF}"),
	/* CHAN_SASET_RESTRICTED_SYNTAX */
	_("SASET channel RESTRICTED {ON | OFF}"),
	/* CHAN_SASET_SECURE_SYNTAX */
	_("SASET channel SECURE {ON | OFF}"),
	/* CHAN_SASET_SECUREFOUNDER_SYNTAX */
	_("SASET channel SECUREFOUNDER {ON | OFF}"),
	/* CHAN_SASET_SECUREOPS_SYNTAX */
	_("SASET channel SECUREOPS {ON | OFF}"),
	/* CHAN_SASET_SIGNKICK_SYNTAX */
	_("SASET channel SIGNKICK {ON | OFF}"),
	/* CHAN_SASET_TOPICLOCK_SYNTAX */
	_("SASET channel TOPICLOCK {ON | OFF}"),
	/* CHAN_SASET_XOP_SYNTAX */
	_("SASET channel XOP {ON | OFF}"),
	/* CHAN_SET_SYNTAX */
	_("SET channel option parameters"),
	/* CHAN_SET_DISABLED */
	_("Sorry, channel option setting is temporarily disabled."),
	/* CHAN_SETTING_CHANGED */
	_("%s for %s set to %s."),
	/* CHAN_SETTING_UNSET */
	_("%s for %s unset."),
	/* CHAN_SET_FOUNDER_TOO_MANY_CHANS */
	_("%s has too many channels registered."),
	/* CHAN_FOUNDER_CHANGED */
	_("Founder of %s changed to %s."),
	/* CHAN_SUCCESSOR_CHANGED */
	_("Successor for %s changed to %s."),
	/* CHAN_SUCCESSOR_UNSET */
	_("Successor for %s unset."),
	/* CHAN_SUCCESSOR_IS_FOUNDER */
	_("%s cannot be the successor on channel %s because he is its founder."),
	/* CHAN_DESC_CHANGED */
	_("Description of %s changed to %s."),
	/* CHAN_SET_BANTYPE_INVALID */
	_("%s is not a valid ban type."),
	/* CHAN_SET_BANTYPE_CHANGED */
	_("Ban type for channel %s is now #%d."),
	/* CHAN_SET_MLOCK_DEPRECATED */
	_("MLOCK is deprecated. Use \002%R%s HELP MODE\002 instead."),
	/* CHAN_SET_KEEPTOPIC_SYNTAX */
	_("SET channel KEEPTOPIC {ON | OFF}"),
	/* CHAN_SET_KEEPTOPIC_ON */
	_("Topic retention option for %s is now ON."),
	/* CHAN_SET_KEEPTOPIC_OFF */
	_("Topic retention option for %s is now OFF."),
	/* CHAN_SET_TOPICLOCK_SYNTAX */
	_("SET channel TOPICLOCK {ON | OFF}"),
	/* CHAN_SET_TOPICLOCK_ON */
	_("Topic lock option for %s is now ON."),
	/* CHAN_SET_TOPICLOCK_OFF */
	_("Topic lock option for %s is now OFF."),
	/* CHAN_SET_PEACE_SYNTAX */
	_("SET channel PEACE {ON | OFF}"),
	/* CHAN_SET_PEACE_ON */
	_("Peace option for %s is now ON."),
	/* CHAN_SET_PEACE_OFF */
	_("Peace option for %s is now OFF."),
	/* CHAN_SET_PRIVATE_SYNTAX */
	_("SET channel PRIVATE {ON | OFF}"),
	/* CHAN_SET_PRIVATE_ON */
	_("Private option for %s is now ON."),
	/* CHAN_SET_PRIVATE_OFF */
	_("Private option for %s is now OFF."),
	/* CHAN_SET_SECUREOPS_SYNTAX */
	_("SET channel SECUREOPS {ON | OFF}"),
	/* CHAN_SET_SECUREOPS_ON */
	_("Secure ops option for %s is now ON."),
	/* CHAN_SET_SECUREOPS_OFF */
	_("Secure ops option for %s is now OFF."),
	/* CHAN_SET_SECUREFOUNDER_SYNTAX */
	_("SET channel SECUREFOUNDER {ON | OFF}"),
	/* CHAN_SET_SECUREFOUNDER_ON */
	_("Secure founder option for %s is now ON."),
	/* CHAN_SET_SECUREFOUNDER_OFF */
	_("Secure founder option for %s is now OFF."),
	/* CHAN_SET_RESTRICTED_SYNTAX */
	_("SET channel RESTRICTED {ON | OFF}"),
	/* CHAN_SET_RESTRICTED_ON */
	_("Restricted access option for %s is now ON."),
	/* CHAN_SET_RESTRICTED_OFF */
	_("Restricted access option for %s is now OFF."),
	/* CHAN_SET_SECURE_SYNTAX */
	_("SET channel SECURE {ON | OFF}"),
	/* CHAN_SET_SECURE_ON */
	_("Secure option for %s is now ON."),
	/* CHAN_SET_SECURE_OFF */
	_("Secure option for %s is now OFF."),
	/* CHAN_SET_SIGNKICK_SYNTAX */
	_("SET channel SIGNKICK {ON | LEVEL | OFF}"),
	/* CHAN_SET_SIGNKICK_ON */
	_("Signed kick option for %s is now ON."),
	/* CHAN_SET_SIGNKICK_LEVEL */
	_("Signed kick option for %s is now ON, but depends of the\n"
	"level of the user that is using the command."),
	/* CHAN_SET_SIGNKICK_OFF */
	_("Signed kick option for %s is now OFF."),
	/* CHAN_SET_OPNOTICE_SYNTAX */
	_("SET channel OPNOTICE {ON | OFF}"),
	/* CHAN_SET_OPNOTICE_ON */
	_("Op-notice option for %s is now ON."),
	/* CHAN_SET_OPNOTICE_OFF */
	_("Op-notice option for %s is now OFF."),
	/* CHAN_SET_XOP_SYNTAX */
	_("SET channel XOP {ON | OFF}"),
	/* CHAN_SET_XOP_ON */
	_("xOP lists system for %s is now ON."),
	/* CHAN_SET_XOP_OFF */
	_("xOP lists system for %s is now OFF."),
	/* CHAN_SET_PERSIST_SYNTAX */
	_("SET channel PERSIST {ON | OFF}"),
	/* CHAN_SET_PERSIST_ON */
	_("Channel %s is now persistant."),
	/* CHAN_SET_PERSIST_OFF */
	_("Channel %s is no longer persistant."),
	/* CHAN_SET_NOEXPIRE_SYNTAX */
	_("SET channel NOEXPIRE {ON | OFF}"),
	/* CHAN_SET_NOEXPIRE_ON */
	_("Channel %s will not expire."),
	/* CHAN_SET_NOEXPIRE_OFF */
	_("Channel %s will expire."),
	/* CHAN_XOP_REACHED_LIMIT */
	_("Sorry, you can only have %d AOP/SOP/VOP entries on a channel."),
	/* CHAN_XOP_LIST_FORMAT */
	_("  %3d  %s"),
	/* CHAN_XOP_ACCESS */
	_("You can't use this command. Use the ACCESS command instead.\n"
	"Type %R%s HELP ACCESS for more information."),
	/* CHAN_XOP_NOT_AVAILABLE */
	_("xOP system is not available."),
	/* CHAN_QOP_SYNTAX */
	_("QOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]"),
	/* CHAN_QOP_DISABLED */
	_("Sorry, channel QOP list modification is temporarily disabled."),
	/* CHAN_QOP_ADDED */
	_("%s added to %s QOP list."),
	/* CHAN_QOP_MOVED */
	_("%s moved to %s QOP list."),
	/* CHAN_QOP_NO_SUCH_ENTRY */
	_("No such entry (#%d) on %s QOP list."),
	/* CHAN_QOP_NOT_FOUND */
	_("%s not found on %s QOP list."),
	/* CHAN_QOP_NO_MATCH */
	_("No matching entries on %s QOP list."),
	/* CHAN_QOP_DELETED */
	_("%s deleted from %s QOP list."),
	/* CHAN_QOP_DELETED_ONE */
	_("Deleted 1 entry from %s QOP list."),
	/* CHAN_QOP_DELETED_SEVERAL */
	_("Deleted %d entries from %s QOP list."),
	/* CHAN_QOP_LIST_EMPTY */
	_("%s QOP list is empty."),
	/* CHAN_QOP_LIST_HEADER */
	_("QOP list for %s:\n"
	"  Num  Nick"),
	/* CHAN_QOP_CLEAR */
	_("Channel %s QOP list has been cleared."),
	/* CHAN_AOP_SYNTAX */
	_("AOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]"),
	/* CHAN_AOP_DISABLED */
	_("Sorry, channel AOP list modification is temporarily disabled."),
	/* CHAN_AOP_ADDED */
	_("%s added to %s AOP list."),
	/* CHAN_AOP_MOVED */
	_("%s moved to %s AOP list."),
	/* CHAN_AOP_NO_SUCH_ENTRY */
	_("No such entry (#%d) on %s AOP list."),
	/* CHAN_AOP_NOT_FOUND */
	_("%s not found on %s AOP list."),
	/* CHAN_AOP_NO_MATCH */
	_("No matching entries on %s AOP list."),
	/* CHAN_AOP_DELETED */
	_("%s deleted from %s AOP list."),
	/* CHAN_AOP_DELETED_ONE */
	_("Deleted 1 entry from %s AOP list."),
	/* CHAN_AOP_DELETED_SEVERAL */
	_("Deleted %d entries from %s AOP list."),
	/* CHAN_AOP_LIST_EMPTY */
	_("%s AOP list is empty."),
	/* CHAN_AOP_LIST_HEADER */
	_("AOP list for %s:\n"
	"  Num  Nick"),
	/* CHAN_AOP_CLEAR */
	_("Channel %s AOP list has been cleared."),
	/* CHAN_HOP_SYNTAX */
	_("HOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]"),
	/* CHAN_HOP_DISABLED */
	_("Sorry, channel HOP list modification is temporarily disabled."),
	/* CHAN_HOP_ADDED */
	_("%s added to %s HOP list."),
	/* CHAN_HOP_MOVED */
	_("%s moved to %s HOP list."),
	/* CHAN_HOP_NO_SUCH_ENTRY */
	_("No such entry (#%d) on %s HOP list."),
	/* CHAN_HOP_NOT_FOUND */
	_("%s not found on %s HOP list."),
	/* CHAN_HOP_NO_MATCH */
	_("No matching entries on %s HOP list."),
	/* CHAN_HOP_DELETED */
	_("%s deleted from %s HOP list."),
	/* CHAN_HOP_DELETED_ONE */
	_("Deleted 1 entry from %s HOP list."),
	/* CHAN_HOP_DELETED_SEVERAL */
	_("Deleted %d entries from %s HOP list."),
	/* CHAN_HOP_LIST_EMPTY */
	_("%s HOP list is empty."),
	/* CHAN_HOP_LIST_HEADER */
	_("HOP list for %s:\n"
	"  Num  Nick"),
	/* CHAN_HOP_CLEAR */
	_("Channel %s HOP list has been cleared."),
	/* CHAN_SOP_SYNTAX */
	_("SOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]"),
	/* CHAN_SOP_DISABLED */
	_("Sorry, channel SOP list modification is temporarily disabled."),
	/* CHAN_SOP_ADDED */
	_("%s added to %s SOP list."),
	/* CHAN_SOP_MOVED */
	_("%s moved to %s SOP list."),
	/* CHAN_SOP_NO_SUCH_ENTRY */
	_("No such entry (#%d) on %s SOP list."),
	/* CHAN_SOP_NOT_FOUND */
	_("%s not found on %s SOP list."),
	/* CHAN_SOP_NO_MATCH */
	_("No matching entries on %s SOP list."),
	/* CHAN_SOP_DELETED */
	_("%s deleted from %s SOP list."),
	/* CHAN_SOP_DELETED_ONE */
	_("Deleted 1 entry from %s SOP list."),
	/* CHAN_SOP_DELETED_SEVERAL */
	_("Deleted %d entries from %s SOP list."),
	/* CHAN_SOP_LIST_EMPTY */
	_("%s SOP list is empty."),
	/* CHAN_SOP_LIST_HEADER */
	_("SOP list for %s:\n"
	"  Num  Nick"),
	/* CHAN_SOP_CLEAR */
	_("Channel %s SOP list has been cleared."),
	/* CHAN_VOP_SYNTAX */
	_("VOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]"),
	/* CHAN_VOP_DISABLED */
	_("Sorry, channel VOP list modification is temporarily disabled."),
	/* CHAN_VOP_ADDED */
	_("%s added to %s VOP list."),
	/* CHAN_VOP_MOVED */
	_("%s moved to %s VOP list."),
	/* CHAN_VOP_NO_SUCH_ENTRY */
	_("No such entry (#%d) on %s VOP list."),
	/* CHAN_VOP_NOT_FOUND */
	_("%s not found on %s VOP list."),
	/* CHAN_VOP_NO_MATCH */
	_("No matching entries on %s VOP list."),
	/* CHAN_VOP_DELETED */
	_("%s deleted from %s VOP list."),
	/* CHAN_VOP_DELETED_ONE */
	_("Deleted 1 entry from %s VOP list."),
	/* CHAN_VOP_DELETED_SEVERAL */
	_("Deleted %d entries from %s VOP list."),
	/* CHAN_VOP_LIST_EMPTY */
	_("%s VOP list is empty."),
	/* CHAN_VOP_LIST_HEADER */
	_("VOP list for %s:\n"
	"  Num  Nick"),
	/* CHAN_VOP_CLEAR */
	_("Channel %s VOP list has been cleared."),
	/* CHAN_ACCESS_SYNTAX */
	_("ACCESS channel {ADD|DEL|LIST|VIEW|CLEAR} [mask [level] | entry-list]"),
	/* CHAN_ACCESS_XOP */
	_("You can't use this command. \n"
	"Use the AOP, SOP and VOP commands instead.\n"
	"Type %R%s HELP command for more information."),
	/* CHAN_ACCESS_XOP_HOP */
	_("You can't use this command. \n"
	"Use the AOP, SOP, HOP and VOP commands instead.\n"
	"Type %R%s HELP command for more information."),
	/* CHAN_ACCESS_DISABLED */
	_("Sorry, channel access list modification is temporarily disabled."),
	/* CHAN_ACCESS_LEVEL_NONZERO */
	_("Access level must be non-zero."),
	/* CHAN_ACCESS_LEVEL_RANGE */
	_("Access level must be between %d and %d inclusive."),
	/* CHAN_ACCESS_REACHED_LIMIT */
	_("Sorry, you can only have %d access entries on a channel."),
	/* CHAN_ACCESS_LEVEL_UNCHANGED */
	_("Access level for %s on %s unchanged from %d."),
	/* CHAN_ACCESS_LEVEL_CHANGED */
	_("Access level for %s on %s changed to %d."),
	/* CHAN_ACCESS_ADDED */
	_("%s added to %s access list at level %d."),
	/* CHAN_ACCESS_NOT_FOUND */
	_("%s not found on %s access list."),
	/* CHAN_ACCESS_NO_MATCH */
	_("No matching entries on %s access list."),
	/* CHAN_ACCESS_DELETED */
	_("%s deleted from %s access list."),
	/* CHAN_ACCESS_DELETED_ONE */
	_("Deleted 1 entry from %s access list."),
	/* CHAN_ACCESS_DELETED_SEVERAL */
	_("Deleted %d entries from %s access list."),
	/* CHAN_ACCESS_LIST_EMPTY */
	_("%s access list is empty."),
	/* CHAN_ACCESS_LIST_HEADER */
	_("Access list for %s:\n"
	"  Num   Lev  Mask"),
	/* CHAN_ACCESS_LIST_FOOTER */
	_("End of access list."),
	/* CHAN_ACCESS_LIST_XOP_FORMAT */
	_("  %3d   %s  %s"),
	/* CHAN_ACCESS_LIST_AXS_FORMAT */
	_("  %3d  %4d  %s"),
	/* CHAN_ACCESS_CLEAR */
	_("Channel %s access list has been cleared."),
	/* CHAN_ACCESS_VIEW_XOP_FORMAT */
	_("  %3d   %s   %s\n"
	"    by %s, last seen %s"),
	/* CHAN_ACCESS_VIEW_AXS_FORMAT */
	_("  %3d  %4d  %s\n"
	"    by %s, last seen %s"),
	/* CHAN_AKICK_SYNTAX */
	_("AKICK channel {ADD | DEL | LIST | VIEW | ENFORCE | CLEAR} [nick-or-usermask] [reason]"),
	/* CHAN_AKICK_DISABLED */
	_("Sorry, channel autokick list modification is temporarily disabled."),
	/* CHAN_AKICK_ALREADY_EXISTS */
	_("%s already exists on %s autokick list."),
	/* CHAN_AKICK_REACHED_LIMIT */
	_("Sorry, you can only have %d autokick masks on a channel."),
	/* CHAN_AKICK_ADDED */
	_("%s added to %s autokick list."),
	/* CHAN_AKICK_NOT_FOUND */
	_("%s not found on %s autokick list."),
	/* CHAN_AKICK_NO_MATCH */
	_("No matching entries on %s autokick list."),
	/* CHAN_AKICK_DELETED */
	_("%s deleted from %s autokick list."),
	/* CHAN_AKICK_DELETED_ONE */
	_("Deleted 1 entry from %s autokick list."),
	/* CHAN_AKICK_DELETED_SEVERAL */
	_("Deleted %d entries from %s autokick list."),
	/* CHAN_AKICK_LIST_EMPTY */
	_("%s autokick list is empty."),
	/* CHAN_AKICK_LIST_HEADER */
	_("Autokick list for %s:"),
	/* CHAN_AKICK_LIST_FORMAT */
	_("  %3d %s (%s)"),
	/* CHAN_AKICK_VIEW_FORMAT */
	_("%3d %s (by %s on %s)\n"
	"    %s"),
	/* CHAN_AKICK_LAST_USED */
	_("    Last used %s"),
	/* CHAN_AKICK_ENFORCE_DONE */
	_("AKICK ENFORCE for %s complete; %d users were affected."),
	/* CHAN_AKICK_CLEAR */
	_("Channel %s akick list has been cleared."),
	/* CHAN_LEVELS_SYNTAX */
	_("LEVELS channel {SET | DIS[ABLE] | LIST | RESET} [item [level]]"),
	/* CHAN_LEVELS_XOP */
	_("Levels are not available as xOP is enabled on this channel."),
	/* CHAN_LEVELS_RANGE */
	_("Level must be between %d and %d inclusive."),
	/* CHAN_LEVELS_CHANGED */
	_("Level for %s on channel %s changed to %d."),
	/* CHAN_LEVELS_CHANGED_FOUNDER */
	_("Level for %s on channel %s changed to founder only."),
	/* CHAN_LEVELS_UNKNOWN */
	_("Setting %s not known.  Type %R%s HELP LEVELS DESC for a list of valid settings."),
	/* CHAN_LEVELS_DISABLED */
	_("%s disabled on channel %s."),
	/* CHAN_LEVELS_LIST_HEADER */
	_("Access level settings for channel %s:"),
	/* CHAN_LEVELS_LIST_DISABLED */
	_("    %-*s  (disabled)"),
	/* CHAN_LEVELS_LIST_FOUNDER */
	_("    %-*s  (founder only)"),
	/* CHAN_LEVELS_LIST_NORMAL */
	_("    %-*s  %d"),
	/* CHAN_LEVELS_RESET */
	_("Access levels for %s reset to defaults."),
	/* CHAN_STATUS_SYNTAX */
	_("STATUS channel item"),
	/* CHAN_STATUS_NOT_REGGED */
	_("STATUS ERROR Channel %s not registered"),
	/* CHAN_STATUS_NOTONLINE */
	_("STATUS ERROR Nick %s not online"),
	/* CHAN_STATUS_INFO */
	_("STATUS %s %s %d"),
	/* CHAN_INFO_SYNTAX */
	_("INFO channel"),
	/* CHAN_INFO_HEADER */
	_("Information for channel %s:"),
	/* CHAN_INFO_NO_FOUNDER */
	_("        Founder: %s"),
	/* CHAN_INFO_NO_SUCCESSOR */
	_("      Successor: %s"),
	/* CHAN_INFO_DESCRIPTION */
	_("    Description: %s"),
	/* CHAN_INFO_TIME_REGGED */
	_("     Registered: %s"),
	/* CHAN_INFO_LAST_USED */
	_("      Last used: %s"),
	/* CHAN_INFO_LAST_TOPIC */
	_("     Last topic: %s"),
	/* CHAN_INFO_TOPIC_SET_BY */
	_("   Topic set by: %s"),
	/* CHAN_INFO_BANTYPE */
	_("       Ban type: %d"),
	/* CHAN_INFO_OPT_KEEPTOPIC */
	_("Topic Retention"),
	/* CHAN_INFO_OPT_OPNOTICE */
	_("OP Notice"),
	/* CHAN_INFO_OPT_PEACE */
	_("Peace"),
	/* CHAN_INFO_OPT_RESTRICTED */
	_("Restricted Access"),
	/* CHAN_INFO_OPT_SECURE */
	_("Secure"),
	/* CHAN_INFO_OPT_SECUREOPS */
	_("Secure Ops"),
	/* CHAN_INFO_OPT_SECUREFOUNDER */
	_("Secure Founder"),
	/* CHAN_INFO_OPT_SIGNKICK */
	_("Signed kicks"),
	/* CHAN_INFO_OPT_TOPICLOCK */
	_("Topic Lock"),
	/* CHAN_INFO_OPT_XOP */
	_("xOP lists system"),
	/* CHAN_INFO_OPT_PERSIST */
	_("Persistant"),
	/* CHAN_INFO_MODE_LOCK */
	_("      Mode lock: %s"),
	/* CHAN_INFO_EXPIRE */
	_("      Expires on: %s"),
	/* CHAN_INFO_NO_EXPIRE */
	_("This channel will not expire."),
	/* CHAN_LIST_END */
	_("End of list - %d/%d matches shown."),
	/* CHAN_INVITE_SYNTAX */
	_("INVITE channel"),
	/* CHAN_INVITE_ALREADY_IN */
	_("You are already in %s! "),
	/* CHAN_INVITE_SUCCESS */
	_("You have been invited to %s."),
	/* CHAN_INVITE_OTHER_SUCCESS */
	_("%s has been invited to %s."),
	/* CHAN_UNBAN_SYNTAX */
	_("UNBAN channel [nick]"),
	/* CHAN_UNBANNED */
	_("You have been unbanned from %s."),
	/* CHAN_UNBANNED_OTHER */
	_("%s has been unbanned from %s."),
	/* CHAN_TOPIC_SYNTAX */
	_("TOPIC channel [topic]"),
	/* CHAN_CLEARUSERS_SYNTAX */
	_("CLEARUSERS \037channel\037"),
	/* CHAN_CLEARED_USERS */
	_("All users have been kicked from \2%s\2."),
	/* CHAN_CLONED */
	_("All settings from \002%s\002 have been transferred to \002%s\002"),
	/* CHAN_CLONED_ACCESS */
	_("All access entries from \002%s\002 have been transferred to \002%s\002"),
	/* CHAN_CLONED_AKICK */
	_("All akick entries from \002%s\002 have been transferred to \002%s\002"),
	/* CHAN_CLONED_BADWORDS */
	_("All badword entries from \002%s\002 have been transferred to \002%s\002"),
	/* CHAN_CLONE_SYNTAX */
	_("CLONE \037channel\037 \037target\037"),
	/* CHAN_GETKEY_SYNTAX */
	_("GETKEY channel"),
	/* CHAN_GETKEY_NOKEY */
	_("The channel %s has no key."),
	/* CHAN_GETKEY_KEY */
	_("Key for channel %s is %s."),
	/* CHAN_FORBID_SYNTAX */
	_("FORBID channel [reason]"),
	/* CHAN_FORBID_SYNTAX_REASON */
	_("FORBID channel reason"),
	/* CHAN_FORBID_SUCCEEDED */
	_("Channel %s is now forbidden."),
	/* CHAN_FORBID_REASON */
	_("This channel has been forbidden."),
	/* CHAN_SUSPEND_SYNTAX */
	_("SUSPEND channel [reason]"),
	/* CHAN_SUSPEND_SYNTAX_REASON */
	_("SUSPEND channel reason"),
	/* CHAN_SUSPEND_SUCCEEDED */
	_("Channel %s is now suspended."),
	/* CHAN_SUSPEND_REASON */
	_("This channel has been suspended."),
	/* CHAN_UNSUSPEND_SYNTAX */
	_("UNSUSPEND channel"),
	/* CHAN_UNSUSPEND_ERROR */
	_("No # found in front of channel name."),
	/* CHAN_UNSUSPEND_SUCCEEDED */
	_("Channel %s is now released."),
	/* CHAN_UNSUSPEND_FAILED */
	_("Couldn't release channel %s!"),
	/* CHAN_EXCEPTED */
	_("%s matches an except on %s and cannot be banned until the except have been removed."),
	/* CHAN_OP_SYNTAX */
	_("OP [#channel] [nick]"),
	/* CHAN_HALFOP_SYNTAX */
	_("HALFOP [#channel] [nick]"),
	/* CHAN_VOICE_SYNTAX */
	_("VOICE [#channel] [nick]"),
	/* CHAN_PROTECT_SYNTAX */
	_("PROTECT [#channel] [nick]"),
	/* CHAN_OWNER_SYNTAX */
	_("OWNER [#channel] [\037nick\037]\002"),
	/* CHAN_DEOP_SYNTAX */
	_("DEOP [#channel] [nick]"),
	/* CHAN_DEHALFOP_SYNTAX */
	_("DEHALFOP [#channel] [nick]"),
	/* CHAN_DEVOICE_SYNTAX */
	_("DEVOICE [#channel] [nick]"),
	/* CHAN_DEPROTECT_SYNTAX */
	_("DEROTECT [#channel] [nick]"),
	/* CHAN_DEOWNER_SYNTAX */
	_("DEOWNER [#channel] [\037nick\037]"),
	/* CHAN_KICK_SYNTAX */
	_("KICK #channel nick [reason]"),
	/* CHAN_BAN_SYNTAX */
	_("BAN #channel nick [reason]"),
	/* CHAN_MODE_SYNTAX */
	_("MODE \037channel\037 {LOCK|SET} [\037modes\037 | {ADD|DEL|LIST} [\037what\037]]"),
	/* CHAN_MODE_LOCK_UNKNOWN */
	_("Unknown mode character %c ignored."),
	/* CHAN_MODE_LOCK_MISSING_PARAM */
	_("Missing parameter for mode %c."),
	/* CHAN_MODE_LOCK_NONE */
	_("Channel %s has no mode locks."),
	/* CHAN_MODE_LOCK_HEADER */
	_("Mode locks for %s:"),
	/* CHAN_MODE_LOCKED */
	_("%c%c%s locked on %s"),
	/* CHAN_MODE_NOT_LOCKED */
	_("%c is not locked on %s."),
	/* CHAN_MODE_UNLOCKED */
	_("%c%c%s has been unlocked from %s."),
	/* CHAN_MODE_LIST_FMT */
	_("%c%c%s, by %s on %s"),
	/* CHAN_ENTRYMSG_LIST_HEADER */
	_("Entry message list for \2%s\2:"),
	/* CHAN_ENTRYMSG_LIST_ENTRY */
	_("%3d    %s\n"
	"  Added by %s on %s"),
	/* CHAN_ENTRYMSG_LIST_END */
	_("End of entry message list."),
	/* CHAN_ENTRYMSG_LIST_EMPTY */
	_("Entry message list for \2%s\2 is empty."),
	/* CHAN_ENTRYMSG_LIST_FULL */
	_("The entry message list for \2%s\2 is full."),
	/* CHAN_ENTRYMSG_ADDED */
	_("Entry message added to \2%s\2"),
	/* CHAN_ENTRYMSG_DELETED */
	_("Entry message \2%i\2 for \2%s\2 deleted."),
	/* CHAN_ENTRYMSG_NOT_FOUND */
	_("Entry message \2%s\2 not found on channel \2%s\2."),
	/* CHAN_ENTRYMSG_CLEARED */
	_("Entry messages for \2%s\2 have been cleared."),
	/* CHAN_ENTRYMSG_SYNTAX */
	_("ENTRYMSG \037channel\037 {ADD|DEL|LIST|CLEAR} [\037message\037|\037num\037]"),
	/* MEMO_HAVE_NEW_MEMO */
	_("You have 1 new memo."),
	/* MEMO_HAVE_NEW_MEMOS */
	_("You have %d new memos."),
	/* MEMO_TYPE_READ_LAST */
	_("Type %R%s READ LAST to read it."),
	/* MEMO_TYPE_READ_NUM */
	_("Type %R%s READ %d to read it."),
	/* MEMO_TYPE_LIST_NEW */
	_("Type %R%s LIST NEW to list them."),
	/* MEMO_AT_LIMIT */
	_("Warning: You have reached your maximum number of memos (%d).  You will be unable to receive any new memos until you delete some of your current ones."),
	/* MEMO_OVER_LIMIT */
	_("Warning: You are over your maximum number of memos (%d).  You will be unable to receive any new memos until you delete some of your current ones."),
	/* MEMO_X_MANY_NOTICE */
	_("There are %d memos on channel %s."),
	/* MEMO_X_ONE_NOTICE */
	_("There is %d memo on channel %s."),
	/* MEMO_NEW_X_MEMO_ARRIVED */
	_("There is a new memo on channel %s.\n"
	"Type %R%s READ %s %d to read it."),
	/* MEMO_NEW_MEMO_ARRIVED */
	_("You have a new memo from %s.\n"
	"Type %R%s READ %d to read it."),
	/* MEMO_HAVE_NO_MEMOS */
	_("You have no memos."),
	/* MEMO_X_HAS_NO_MEMOS */
	_("%s has no memos."),
	/* MEMO_SEND_SYNTAX */
	_("SEND {nick | channel} memo-text"),
	/* MEMO_SENDALL_SYNTAX */
	_("SENDALL memo-text"),
	/* MEMO_SEND_DISABLED */
	_("Sorry, memo sending is temporarily disabled."),
	/* MEMO_SEND_PLEASE_WAIT */
	_("Please wait %d seconds before using the SEND command again."),
	/* MEMO_X_GETS_NO_MEMOS */
	_("%s cannot receive memos."),
	/* MEMO_X_HAS_TOO_MANY_MEMOS */
	_("%s currently has too many memos and cannot receive more."),
	/* MEMO_SENT */
	_("Memo sent to %s."),
	/* MEMO_MASS_SENT */
	_("A massmemo has been sent to all registered users."),
	/* MEMO_STAFF_SYNTAX */
	_("STAFF memo-text"),
	/* MEMO_CANCEL_SYNTAX */
	_("CANCEL {nick | channel}"),
	/* MEMO_CANCEL_NONE */
	_("No memo was cancelable."),
	/* MEMO_CANCELLED */
	_("Last memo to %s has been cancelled."),
	/* MEMO_LIST_SYNTAX */
	_("LIST [channel] [list | NEW]"),
	/* MEMO_HAVE_NO_NEW_MEMOS */
	_("You have no new memos."),
	/* MEMO_X_HAS_NO_NEW_MEMOS */
	_("%s has no new memos."),
	/* MEMO_LIST_MEMOS */
	_("Memos for %s.  To read, type: %R%s READ num"),
	/* MEMO_LIST_NEW_MEMOS */
	_("New memos for %s.  To read, type: %R%s READ num"),
	/* MEMO_LIST_CHAN_MEMOS */
	_("Memos for %s.  To read, type: %R%s READ %s num"),
	/* MEMO_LIST_CHAN_NEW_MEMOS */
	_("New memos for %s.  To read, type: %R%s READ %s num"),
	/* MEMO_LIST_HEADER */
	_(" Num  Sender            Date/Time"),
	/* MEMO_LIST_FORMAT */
	_("%c%3d  %-16s  %s"),
	/* MEMO_READ_SYNTAX */
	_("READ [channel] {list | LAST | NEW}"),
	/* MEMO_HEADER */
	_("Memo %d from %s (%s).  To delete, type: %R%s DEL %d"),
	/* MEMO_CHAN_HEADER */
	_("Memo %d from %s (%s).  To delete, type: %R%s DEL %s %d"),
	/* MEMO_TEXT */
	_("%s"),
	/* MEMO_DEL_SYNTAX */
	_("DEL [channel] {num | list | ALL}"),
	/* MEMO_DELETED_ONE */
	_("Memo %d has been deleted."),
	/* MEMO_DELETED_ALL */
	_("All of your memos have been deleted."),
	/* MEMO_CHAN_DELETED_ALL */
	_("All memos for channel %s have been deleted."),
	/* MEMO_SET_DISABLED */
	_("Sorry, memo option setting is temporarily disabled."),
	/* MEMO_SET_NOTIFY_SYNTAX */
	_("SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF }"),
	/* MEMO_SET_NOTIFY_ON */
	_("%s will now notify you of memos when you log on and when they are sent to you."),
	/* MEMO_SET_NOTIFY_LOGON */
	_("%s will now notify you of memos when you log on or unset /AWAY."),
	/* MEMO_SET_NOTIFY_NEW */
	_("%s will now notify you of memos when they are sent to you."),
	/* MEMO_SET_NOTIFY_OFF */
	_("%s will not send you any notification of memos."),
	/* MEMO_SET_NOTIFY_MAIL */
	_("You will now be informed about new memos via email."),
	/* MEMO_SET_NOTIFY_NOMAIL */
	_("You will no longer be informed via email."),
	/* MEMO_SET_NOTIFY_INVALIDMAIL */
	_("There's no email address set for your nick."),
	/* MEMO_SET_LIMIT_SYNTAX */
	_("SET LIMIT [channel] limit"),
	/* MEMO_SET_LIMIT_SERVADMIN_SYNTAX */
	_("SET LIMIT [user | channel] {limit | NONE} [HARD]"),
	/* MEMO_SET_YOUR_LIMIT_FORBIDDEN */
	_("You are not permitted to change your memo limit."),
	/* MEMO_SET_LIMIT_FORBIDDEN */
	_("The memo limit for %s may not be changed."),
	/* MEMO_SET_YOUR_LIMIT_TOO_HIGH */
	_("You cannot set your memo limit higher than %d."),
	/* MEMO_SET_LIMIT_TOO_HIGH */
	_("You cannot set the memo limit for %s higher than %d."),
	/* MEMO_SET_LIMIT_OVERFLOW */
	_("Memo limit too large; limiting to %d instead."),
	/* MEMO_SET_YOUR_LIMIT */
	_("Your memo limit has been set to %d."),
	/* MEMO_SET_YOUR_LIMIT_ZERO */
	_("You will no longer be able to receive memos."),
	/* MEMO_UNSET_YOUR_LIMIT */
	_("Your memo limit has been disabled."),
	/* MEMO_SET_LIMIT */
	_("Memo limit for %s set to %d."),
	/* MEMO_SET_LIMIT_ZERO */
	_("Memo limit for %s set to 0."),
	/* MEMO_UNSET_LIMIT */
	_("Memo limit disabled for %s."),
	/* MEMO_INFO_NO_MEMOS */
	_("You currently have no memos."),
	/* MEMO_INFO_MEMO */
	_("You currently have 1 memo."),
	/* MEMO_INFO_MEMO_UNREAD */
	_("You currently have 1 memo, and it has not yet been read."),
	/* MEMO_INFO_MEMOS */
	_("You currently have %d memos."),
	/* MEMO_INFO_MEMOS_ONE_UNREAD */
	_("You currently have %d memos, of which 1 is unread."),
	/* MEMO_INFO_MEMOS_SOME_UNREAD */
	_("You currently have %d memos, of which %d are unread."),
	/* MEMO_INFO_MEMOS_ALL_UNREAD */
	_("You currently have %d memos; all of them are unread."),
	/* MEMO_INFO_LIMIT */
	_("Your memo limit is %d."),
	/* MEMO_INFO_HARD_LIMIT */
	_("Your memo limit is %d, and may not be changed."),
	/* MEMO_INFO_LIMIT_ZERO */
	_("Your memo limit is 0; you will not receive any new memos."),
	/* MEMO_INFO_HARD_LIMIT_ZERO */
	_("Your memo limit is 0; you will not receive any new memos.  You cannot change this limit."),
	/* MEMO_INFO_NO_LIMIT */
	_("You have no limit on the number of memos you may keep."),
	/* MEMO_INFO_NOTIFY_OFF */
	_("You will not be notified of new memos."),
	/* MEMO_INFO_NOTIFY_ON */
	_("You will be notified of new memos at logon and when they arrive."),
	/* MEMO_INFO_NOTIFY_RECEIVE */
	_("You will be notified when new memos arrive."),
	/* MEMO_INFO_NOTIFY_SIGNON */
	_("You will be notified of new memos at logon."),
	/* MEMO_INFO_X_NO_MEMOS */
	_("%s currently has no memos."),
	/* MEMO_INFO_X_MEMO */
	_("%s currently has 1 memo."),
	/* MEMO_INFO_X_MEMO_UNREAD */
	_("%s currently has 1 memo, and it has not yet been read."),
	/* MEMO_INFO_X_MEMOS */
	_("%s currently has %d memos."),
	/* MEMO_INFO_X_MEMOS_ONE_UNREAD */
	_("%s currently has %d memos, of which 1 is unread."),
	/* MEMO_INFO_X_MEMOS_SOME_UNREAD */
	_("%s currently has %d memos, of which %d are unread."),
	/* MEMO_INFO_X_MEMOS_ALL_UNREAD */
	_("%s currently has %d memos; all of them are unread."),
	/* MEMO_INFO_X_LIMIT */
	_("%s's memo limit is %d."),
	/* MEMO_INFO_X_HARD_LIMIT */
	_("%s's memo limit is %d, and may not be changed."),
	/* MEMO_INFO_X_NO_LIMIT */
	_("%s has no memo limit."),
	/* MEMO_INFO_X_NOTIFY_OFF */
	_("%s is not notified of new memos."),
	/* MEMO_INFO_X_NOTIFY_ON */
	_("%s is notified of new memos at logon and when they arrive."),
	/* MEMO_INFO_X_NOTIFY_RECEIVE */
	_("%s is notified when new memos arrive."),
	/* MEMO_INFO_X_NOTIFY_SIGNON */
	_("%s is notified of news memos at logon."),
	/* MEMO_MAIL_SUBJECT */
	_("New memo"),
	/* NICK_MAIL_TEXT */
	_("Hi %s\n"
	" \n"
	"You've just received a new memo from %s. This is memo number %d.\n"
	" \n"
	"Memo text:\n"
	" \n"
	"%s"),
	/* MEMO_RSEND_PLEASE_WAIT */
	_("Please wait %d seconds before using the RSEND command again."),
	/* MEMO_RSEND_DISABLED */
	_("Sorry, RSEND has been disabled on this network."),
	/* MEMO_RSEND_SYNTAX */
	_("RSEND {nick | channel} memo-text"),
	/* MEMO_RSEND_NICK_MEMO_TEXT */
	_("[auto-memo] The memo you sent has been viewed."),
	/* MEMO_RSEND_CHAN_MEMO_TEXT */
	_("[auto-memo] The memo you sent to %s has been viewed."),
	/* MEMO_RSEND_USER_NOTIFICATION */
	_("A notification memo has been sent to %s informing him/her you have\n"
	"read his/her memo."),
	/* MEMO_CHECK_SYNTAX */
	_("CHECK nickname"),
	/* MEMO_CHECK_NOT_READ */
	_("The last memo you sent to %s (sent on %s) has not yet been read."),
	/* MEMO_CHECK_READ */
	_("The last memo you sent to %s (sent on %s) has been read."),
	/* MEMO_CHECK_NO_MEMO */
	_("Nick %s doesn't have a memo from you."),
	/* MEMO_NO_RSEND_SELF */
	_("You can not request a receipt when sending a memo to yourself."),
	/* MEMO_IGNORE_SYNTAX */
	_("IGNORE [\037channel\037] {\002ADD|DEL|LIST\002} [\037entry\037]"),
	/* MEMO_IGNORE_ADD */
	_("\002%s\002 added to your ignore list."),
	/* MEMO_IGNORE_ALREADY_IGNORED */
	_("\002%s\002 is already on your ignore list."),
	/* MEMO_IGNORE_DEL */
	_("\002%s\002 removed from your ignore list."),
	/* MEMO_IGNORE_NOT_IGNORED */
	_("\002%s\002 is not on your ignore list."),
	/* MEMO_IGNORE_LIST_EMPTY */
	_("Your memo ignore list is empty."),
	/* MEMO_IGNORE_LIST_HEADER */
	_("Ignore list:"),
	/* BOT_DOES_NOT_EXIST */
	_("Bot %s does not exist."),
	/* BOT_NOT_ASSIGNED */
	_("You must assign a bot to the channel before using this command.\n"
	"Type %R%S HELP ASSIGN for more information."),
	/* BOT_NOT_ON_CHANNEL */
	_("Bot is not on channel %s."),
	/* BOT_REASON_BADWORD */
	_("Don't use the word \"%s\" on this channel!"),
	/* BOT_REASON_BADWORD_GENTLE */
	_("Watch your language!"),
	/* BOT_REASON_BOLD */
	_("Don't use bolds on this channel!"),
	/* BOT_REASON_CAPS */
	_("Turn caps lock OFF!"),
	/* BOT_REASON_COLOR */
	_("Don't use colors on this channel!"),
	/* BOT_REASON_FLOOD */
	_("Stop flooding!"),
	/* BOT_REASON_REPEAT */
	_("Stop repeating yourself!"),
	/* BOT_REASON_REVERSE */
	_("Don't use reverses on this channel!"),
	/* BOT_REASON_UNDERLINE */
	_("Don't use underlines on this channel!"),
	/* BOT_REASON_ITALIC */
	_("Don't use italics on this channel!"),
	/* BOT_BOT_SYNTAX */
	_("BOT ADD nick user host real\n"
	"BOT CHANGE oldnick newnick [user [host [real]]]\n"
	"BOT DEL nick"),
	/* BOT_BOT_ALREADY_EXISTS */
	_("Bot %s already exists."),
	/* BOT_BOT_CREATION_FAILED */
	_("Sorry, bot creation failed."),
	/* BOT_BOT_READONLY */
	_("Sorry, bot modification is temporarily disabled."),
	/* BOT_BOT_ADDED */
	_("%s!%s@%s (%s) added to the bot list."),
	/* BOT_BOT_ANY_CHANGES */
	_("Old info is equal to the new one."),
	/* BOT_BOT_CHANGED */
	_("Bot %s has been changed to %s!%s@%s (%s)"),
	/* BOT_BOT_DELETED */
	_("Bot %s has been deleted."),
	/* BOT_BOTLIST_HEADER */
	_("Bot list:"),
	/* BOT_BOTLIST_PRIVATE_HEADER */
	_("Bots reserved to IRC operators:"),
	/* BOT_BOTLIST_FOOTER */
	_("%d bots available."),
	/* BOT_BOTLIST_EMPTY */
	_("There are no bots available at this time.\n"
	"Ask a Services Operator to create one!"),
	/* BOT_ASSIGN_SYNTAX */
	_("ASSIGN chan nick"),
	/* BOT_ASSIGN_READONLY */
	_("Sorry, bot assignment is temporarily disabled."),
	/* BOT_ASSIGN_ALREADY */
	_("Bot %s is already assigned to channel %s."),
	/* BOT_ASSIGN_ASSIGNED */
	_("Bot %s has been assigned to %s."),
	/* BOT_UNASSIGN_SYNTAX */
	_("UNASSIGN chan"),
	/* BOT_UNASSIGN_UNASSIGNED */
	_("There is no bot assigned to %s anymore."),
	/* BOT_UNASSIGN_PERSISTANT_CHAN */
	_("You can not unassign bots while persist is set on the channel."),
	/* BOT_INFO_SYNTAX */
	_("INFO {chan | nick}"),
	/* BOT_INFO_NOT_FOUND */
	_("%s is not a valid bot or registered channel."),
	/* BOT_INFO_BOT_HEADER */
	_("Information for bot %s:"),
	/* BOT_INFO_BOT_MASK */
	_("       Mask : %s@%s"),
	/* BOT_INFO_BOT_REALNAME */
	_("  Real name : %s"),
	/* BOT_INFO_BOT_CREATED */
	_("    Created : %s"),
	/* BOT_INFO_BOT_USAGE */
	_("    Used on : %d channel(s)"),
	/* BOT_INFO_BOT_OPTIONS */
	_("    Options : %s"),
	/* BOT_INFO_CHAN_BOT */
	_("           Bot nick : %s"),
	/* BOT_INFO_CHAN_BOT_NONE */
	_("           Bot nick : not assigned yet."),
	/* BOT_INFO_CHAN_KICK_BADWORDS */
	_("   Bad words kicker : %s"),
	/* BOT_INFO_CHAN_KICK_BADWORDS_BAN */
	_("   Bad words kicker : %s (%d kick(s) to ban)"),
	/* BOT_INFO_CHAN_KICK_BOLDS */
	_("       Bolds kicker : %s"),
	/* BOT_INFO_CHAN_KICK_BOLDS_BAN */
	_("       Bolds kicker : %s (%d kick(s) to ban)"),
	/* BOT_INFO_CHAN_KICK_CAPS_ON */
	_("        Caps kicker : %s (minimum %d/%d%%)"),
	/* BOT_INFO_CHAN_KICK_CAPS_BAN */
	_("        Caps kicker : %s (%d kick(s) to ban; minimum %d/%d%%)"),
	/* BOT_INFO_CHAN_KICK_CAPS_OFF */
	_("        Caps kicker : %s"),
	/* BOT_INFO_CHAN_KICK_COLORS */
	_("      Colors kicker : %s"),
	/* BOT_INFO_CHAN_KICK_COLORS_BAN */
	_("      Colors kicker : %s (%d kick(s) to ban)"),
	/* BOT_INFO_CHAN_KICK_FLOOD_ON */
	_("       Flood kicker : %s (%d lines in %ds)"),
	/* BOT_INFO_CHAN_KICK_FLOOD_BAN */
	_("       Flood kicker : %s (%d kick(s) to ban; %d lines in %ds)"),
	/* BOT_INFO_CHAN_KICK_FLOOD_OFF */
	_("       Flood kicker : %s"),
	/* BOT_INFO_CHAN_KICK_REPEAT_ON */
	_("      Repeat kicker : %s (%d times)"),
	/* BOT_INFO_CHAN_KICK_REPEAT_BAN */
	_("      Repeat kicker : %s (%d kick(s) to ban; %d times)"),
	/* BOT_INFO_CHAN_KICK_REPEAT_OFF */
	_("      Repeat kicker : %s"),
	/* BOT_INFO_CHAN_KICK_REVERSES */
	_("    Reverses kicker : %s"),
	/* BOT_INFO_CHAN_KICK_REVERSES_BAN */
	_("    Reverses kicker : %s (%d kick(s) to ban)"),
	/* BOT_INFO_CHAN_KICK_UNDERLINES */
	_("  Underlines kicker : %s"),
	/* BOT_INFO_CHAN_KICK_UNDERLINES_BAN */
	_("  Underlines kicker : %s (%d kick(s) to ban)"),
	/* BOT_INFO_CHAN_KICK_ITALICS */
	_("     Italics kicker : %s"),
	/* BOT_INFO_CHAN_KICK_ITALICS_BAN */
	_("     Italics kicker : %s (%d kick(s) to ban)"),
	/* BOT_INFO_CHAN_MSG */
	_("      Fantasy reply : %s"),
	/* BOT_INFO_ACTIVE */
	_("enabled"),
	/* BOT_INFO_INACTIVE */
	_("disabled"),
	/* BOT_INFO_CHAN_OPTIONS */
	_("            Options : %s"),
	/* BOT_INFO_OPT_DONTKICKOPS */
	_("Ops protection"),
	/* BOT_INFO_OPT_DONTKICKVOICES */
	_("Voices protection"),
	/* BOT_INFO_OPT_FANTASY */
	_("Fantasy"),
	/* BOT_INFO_OPT_GREET */
	_("Greet"),
	/* BOT_INFO_OPT_NOBOT */
	_("No bot"),
	/* BOT_INFO_OPT_SYMBIOSIS */
	_("Symbiosis"),
	/* BOT_INFO_OPT_NONE */
	_("None"),
	/* BOT_SET_SYNTAX */
	_("SET (channel | bot) option settings"),
	/* BOT_SET_DISABLED */
	_("Sorry, bot option setting is temporarily disabled."),
	/* BOT_SET_UNKNOWN */
	_("Unknown option %s.\n"
	"Type %R%S HELP SET for more information."),
	/* BOT_SET_DONTKICKOPS_SYNTAX */
	_("SET channel DONTKICKOPS {ON|OFF}"),
	/* BOT_SET_DONTKICKOPS_ON */
	_("Bot won't kick ops on channel %s."),
	/* BOT_SET_DONTKICKOPS_OFF */
	_("Bot will kick ops on channel %s."),
	/* BOT_SET_DONTKICKVOICES_SYNTAX */
	_("SET channel DONTKICKVOICES {ON|OFF}"),
	/* BOT_SET_DONTKICKVOICES_ON */
	_("Bot won't kick voices on channel %s."),
	/* BOT_SET_DONTKICKVOICES_OFF */
	_("Bot will kick voices on channel %s."),
	/* BOT_SET_FANTASY_SYNTAX */
	_("SET channel FANTASY {ON|OFF}"),
	/* BOT_SET_FANTASY_ON */
	_("Fantasy mode is now ON on channel %s."),
	/* BOT_SET_FANTASY_OFF */
	_("Fantasy mode is now OFF on channel %s."),
	/* BOT_SET_GREET_SYNTAX */
	_("SET channel GREET {ON|OFF}"),
	/* BOT_SET_GREET_ON */
	_("Greet mode is now ON on channel %s."),
	/* BOT_SET_GREET_OFF */
	_("Greet mode is now OFF on channel %s."),
	/* BOT_SET_NOBOT_SYNTAX */
	_("SET botname NOBOT {ON|OFF}"),
	/* BOT_SET_NOBOT_ON */
	_("No Bot mode is now ON on channel %s."),
	/* BOT_SET_NOBOT_OFF */
	_("No Bot mode is now OFF on channel %s."),
	/* BOT_SET_PRIVATE_SYNTAX */
	_("SET botname PRIVATE {ON|OFF}"),
	/* BOT_SET_PRIVATE_ON */
	_("Private mode of bot %s is now ON."),
	/* BOT_SET_PRIVATE_OFF */
	_("Private mode of bot %s is now OFF."),
	/* BOT_SET_SYMBIOSIS_SYNTAX */
	_("SET channel SYMBIOSIS {ON|OFF}"),
	/* BOT_SET_SYMBIOSIS_ON */
	_("Symbiosis mode is now ON on channel %s."),
	/* BOT_SET_SYMBIOSIS_OFF */
	_("Symbiosis mode is now OFF on channel %s."),
	/* BOT_SET_MSG_SYNTAX */
	_("SET \037channel\037 MSG {\037OFF|PRIVMSG|NOTICE|NOTICEOPS\037}"),
	/* BOT_SET_MSG_OFF */
	_("Fantasy replies will no longer be sent to %s."),
	/* BOT_SET_MSG_PRIVMSG */
	_("Fantasy replies will be sent via PRIVMSG to %s."),
	/* BOT_SET_MSG_NOTICE */
	_("Fantasy replies will be sent via NOTICE to %s."),
	/* BOT_SET_MSG_NOTICEOPS */
	_("Fantasy replies will be sent via NOTICE to channel ops on %s."),
	/* BOT_KICK_SYNTAX */
	_("KICK channel option {ON|OFF} [settings]"),
	/* BOT_KICK_DISABLED */
	_("Sorry, kicker configuration is temporarily disabled."),
	/* BOT_KICK_UNKNOWN */
	_("Unknown option %s.\n"
	"Type %R%S HELP KICK for more information."),
	/* BOT_KICK_BAD_TTB */
	_("%s cannot be taken as times to ban."),
	/* BOT_KICK_BADWORDS_ON */
	_("Bot will now kick bad words. Use the BADWORDS command\n"
	"to add or remove a bad word."),
	/* BOT_KICK_BADWORDS_ON_BAN */
	_("Bot will now kick bad words, and will place a ban after \n"
	"%d kicks for the same user. Use the BADWORDS command\n"
	"to add or remove a bad word."),
	/* BOT_KICK_BADWORDS_OFF */
	_("Bot won't kick bad words anymore."),
	/* BOT_KICK_BOLDS_ON */
	_("Bot will now kick bolds."),
	/* BOT_KICK_BOLDS_ON_BAN */
	_("Bot will now kick bolds, and will place a ban after \n"
	"%d kicks for the same user."),
	/* BOT_KICK_BOLDS_OFF */
	_("Bot won't kick bolds anymore."),
	/* BOT_KICK_CAPS_ON */
	_("Bot will now kick caps (they must constitute at least\n"
	"%d characters and %d%% of the entire message)."),
	/* BOT_KICK_CAPS_ON_BAN */
	_("Bot will now kick caps (they must constitute at least\n"
	"%d characters and %d%% of the entire message), and will \n"
	"place a ban after %d kicks for the same user."),
	/* BOT_KICK_CAPS_OFF */
	_("Bot won't kick caps anymore."),
	/* BOT_KICK_COLORS_ON */
	_("Bot will now kick colors."),
	/* BOT_KICK_COLORS_ON_BAN */
	_("Bot will now kick colors, and will place a ban after %d \n"
	"kicks for the same user."),
	/* BOT_KICK_COLORS_OFF */
	_("Bot won't kick colors anymore."),
	/* BOT_KICK_FLOOD_ON */
	_("Bot will now kick flood (%d lines in %d seconds)."),
	/* BOT_KICK_FLOOD_ON_BAN */
	_("Bot will now kick flood (%d lines in %d seconds), and \n"
	"will place a ban after %d kicks for the same user."),
	/* BOT_KICK_FLOOD_OFF */
	_("Bot won't kick flood anymore."),
	/* BOT_KICK_REPEAT_ON */
	_("Bot will now kick repeats (users that say %d times\n"
	"the same thing)."),
	/* BOT_KICK_REPEAT_ON_BAN */
	_("Bot will now kick repeats (users that say %d times\n"
	"the same thing), and will place a ban after %d \n"
	"kicks for the same user."),
	/* BOT_KICK_REPEAT_OFF */
	_("Bot won't kick repeats anymore."),
	/* BOT_KICK_REVERSES_ON */
	_("Bot will now kick reverses."),
	/* BOT_KICK_REVERSES_ON_BAN */
	_("Bot will now kick reverses, and will place a ban after %d \n"
	"kicks for the same user."),
	/* BOT_KICK_REVERSES_OFF */
	_("Bot won't kick reverses anymore."),
	/* BOT_KICK_UNDERLINES_ON */
	_("Bot will now kick underlines."),
	/* BOT_KICK_UNDERLINES_ON_BAN */
	_("Bot will now kick underlines, and will place a ban after %d \n"
	"kicks for the same user."),
	/* BOT_KICK_UNDERLINES_OFF */
	_("Bot won't kick underlines anymore."),
	/* BOT_KICK_ITALICS_ON */
	_("Bot will now kick italics."),
	/* BOT_KICK_ITALICS_ON_BAN */
	_("Bot will now kick italics, and will place a ban after \n"
	"%d kicks for the same user."),
	/* BOT_KICK_ITALICS_OFF */
	_("Bot won't kick italics anymore."),
	/* BOT_BADWORDS_SYNTAX */
	_("BADWORDS channel {ADD|DEL|LIST|CLEAR} [word | entry-list] [SINGLE|START|END]"),
	/* BOT_BADWORDS_DISABLED */
	_("Sorry, channel bad words list modification is temporarily disabled."),
	/* BOT_BADWORDS_REACHED_LIMIT */
	_("Sorry, you can only have %d bad words entries on a channel."),
	/* BOT_BADWORDS_ALREADY_EXISTS */
	_("%s already exists in %s bad words list."),
	/* BOT_BADWORDS_ADDED */
	_("%s added to %s bad words list."),
	/* BOT_BADWORDS_NOT_FOUND */
	_("%s not found on %s bad words list."),
	/* BOT_BADWORDS_NO_MATCH */
	_("No matching entries on %s bad words list."),
	/* BOT_BADWORDS_DELETED */
	_("%s deleted from %s bad words list."),
	/* BOT_BADWORDS_DELETED_ONE */
	_("Deleted 1 entry from %s bad words list."),
	/* BOT_BADWORDS_DELETED_SEVERAL */
	_("Deleted %d entries from %s bad words list."),
	/* BOT_BADWORDS_LIST_EMPTY */
	_("%s bad words list is empty."),
	/* BOT_BADWORDS_LIST_HEADER */
	_("Bad words list for %s:\n"
	"  Num   Word                           Type"),
	/* BOT_BADWORDS_LIST_FORMAT */
	_("  %3d   %-30s %s"),
	/* BOT_BADWORDS_CLEAR */
	_("Bad words list is now empty."),
	/* BOT_SAY_SYNTAX */
	_("SAY channel text"),
	/* BOT_ACT_SYNTAX */
	_("ACT channel text"),
	/* BOT_EXCEPT */
	_("User matches channel except."),
	/* BOT_BAD_NICK */
	_("Bot Nicks may only contain valid nick characters."),
	/* BOT_BAD_HOST */
	_("Bot Hosts may only contain valid host characters."),
	/* BOT_BAD_IDENT */
	_("Bot Idents may only contain valid characters."),
	/* BOT_LONG_IDENT */
	_("Bot Idents may only contain %d characters."),
	/* BOT_LONG_HOST */
	_("Bot Hosts may only contain %d characters."),
	/* OPER_BOUNCY_MODES */
	_("Services is unable to change modes.  Are your servers configured correctly?"),
	/* OPER_BOUNCY_MODES_U_LINE */
	_("Services is unable to change modes.  Are your servers' U:lines configured correctly?"),
	/* OPER_GLOBAL_SYNTAX */
	_("GLOBAL message"),
	/* OPER_STATS_UNKNOWN_OPTION */
	_("Unknown STATS option %s."),
	/* OPER_STATS_CURRENT_USERS */
	_("Current users: %d (%d ops)"),
	/* OPER_STATS_MAX_USERS */
	_("Maximum users: %d (%s)"),
	/* OPER_STATS_UPTIME_DHMS */
	_("Services up %d days, %02d:%02d"),
	/* OPER_STATS_UPTIME_1DHMS */
	_("Services up %d day, %02d:%02d"),
	/* OPER_STATS_UPTIME_HMS */
	_("Services up %d hours, %d minutes"),
	/* OPER_STATS_UPTIME_H1MS */
	_("Services up %d hours, %d minute"),
	/* OPER_STATS_UPTIME_1HMS */
	_("Services up %d hour, %d minutes"),
	/* OPER_STATS_UPTIME_1H1MS */
	_("Services up %d hour, %d minute"),
	/* OPER_STATS_UPTIME_MS */
	_("Services up %d minutes, %d seconds"),
	/* OPER_STATS_UPTIME_M1S */
	_("Services up %d minutes, %d second"),
	/* OPER_STATS_UPTIME_1MS */
	_("Services up %d minute, %d seconds"),
	/* OPER_STATS_UPTIME_1M1S */
	_("Services up %d minute, %d second"),
	/* OPER_STATS_BYTES_READ */
	_("Bytes read    : %5d kB"),
	/* OPER_STATS_BYTES_WRITTEN */
	_("Bytes written : %5d kB"),
	/* OPER_STATS_USER_MEM */
	_("User          : %6d records, %5d kB"),
	/* OPER_STATS_CHANNEL_MEM */
	_("Channel       : %6d records, %5d kB"),
	/* OPER_STATS_GROUPS_MEM */
	_("NS Groups     : %6d records, %5d kB"),
	/* OPER_STATS_ALIASES_MEM */
	_("NS Aliases    : %6d records, %5d kB"),
	/* OPER_STATS_CHANSERV_MEM */
	_("ChanServ      : %6d records, %5d kB"),
	/* OPER_STATS_BOTSERV_MEM */
	_("BotServ       : %6d records, %5d kB"),
	/* OPER_STATS_HOSTSERV_MEM */
	_("HostServ      : %6d records, %5d kB"),
	/* OPER_STATS_OPERSERV_MEM */
	_("OperServ      : %6d records, %5d kB"),
	/* OPER_STATS_SESSIONS_MEM */
	_("Sessions      : %6d records, %5d kB"),
	/* OPER_STATS_AKILL_COUNT */
	_("Current number of AKILLs: %d"),
	/* OPER_STATS_AKILL_EXPIRE_DAYS */
	_("Default AKILL expiry time: %d days"),
	/* OPER_STATS_AKILL_EXPIRE_DAY */
	_("Default AKILL expiry time: 1 day"),
	/* OPER_STATS_AKILL_EXPIRE_HOURS */
	_("Default AKILL expiry time: %d hours"),
	/* OPER_STATS_AKILL_EXPIRE_HOUR */
	_("Default AKILL expiry time: 1 hour"),
	/* OPER_STATS_AKILL_EXPIRE_MINS */
	_("Default AKILL expiry time: %d minutes"),
	/* OPER_STATS_AKILL_EXPIRE_MIN */
	_("Default AKILL expiry time: 1 minute"),
	/* OPER_STATS_AKILL_EXPIRE_NONE */
	_("Default AKILL expiry time: No expiration"),
	/* OPER_STATS_SNLINE_COUNT */
	_("Current number of SNLINEs: %d"),
	/* OPER_STATS_SNLINE_EXPIRE_DAYS */
	_("Default SNLINE expiry time: %d days"),
	/* OPER_STATS_SNLINE_EXPIRE_DAY */
	_("Default SNLINE expiry time: 1 day"),
	/* OPER_STATS_SNLINE_EXPIRE_HOURS */
	_("Default SNLINE expiry time: %d hours"),
	/* OPER_STATS_SNLINE_EXPIRE_HOUR */
	_("Default SNLINE expiry time: 1 hour"),
	/* OPER_STATS_SNLINE_EXPIRE_MINS */
	_("Default SNLINE expiry time: %d minutes"),
	/* OPER_STATS_SNLINE_EXPIRE_MIN */
	_("Default SNLINE expiry time: 1 minute"),
	/* OPER_STATS_SNLINE_EXPIRE_NONE */
	_("Default SNLINE expiry time: No expiration"),
	/* OPER_STATS_SQLINE_COUNT */
	_("Current number of SQLINEs: %d"),
	/* OPER_STATS_SQLINE_EXPIRE_DAYS */
	_("Default SQLINE expiry time: %d days"),
	/* OPER_STATS_SQLINE_EXPIRE_DAY */
	_("Default SQLINE expiry time: 1 day"),
	/* OPER_STATS_SQLINE_EXPIRE_HOURS */
	_("Default SQLINE expiry time: %d hours"),
	/* OPER_STATS_SQLINE_EXPIRE_HOUR */
	_("Default SQLINE expiry time: 1 hour"),
	/* OPER_STATS_SQLINE_EXPIRE_MINS */
	_("Default SQLINE expiry time: %d minutes"),
	/* OPER_STATS_SQLINE_EXPIRE_MIN */
	_("Default SQLINE expiry time: 1 minute"),
	/* OPER_STATS_SQLINE_EXPIRE_NONE */
	_("Default SQLINE expiry time: No expiration"),
	/* OPER_STATS_SZLINE_COUNT */
	_("Current number of SZLINEs: %d"),
	/* OPER_STATS_SZLINE_EXPIRE_DAYS */
	_("Default SZLINE expiry time: %d days"),
	/* OPER_STATS_SZLINE_EXPIRE_DAY */
	_("Default SZLINE expiry time: 1 day"),
	/* OPER_STATS_SZLINE_EXPIRE_HOURS */
	_("Default SZLINE expiry time: %d hours"),
	/* OPER_STATS_SZLINE_EXPIRE_HOUR */
	_("Default SZLINE expiry time: 1 hour"),
	/* OPER_STATS_SZLINE_EXPIRE_MINS */
	_("Default SZLINE expiry time: %d minutes"),
	/* OPER_STATS_SZLINE_EXPIRE_MIN */
	_("Default SZLINE expiry time: 1 minute"),
	/* OPER_STATS_SZLINE_EXPIRE_NONE */
	_("Default SZLINE expiry time: No expiration"),
	/* OPER_STATS_RESET */
	_("Statistics reset."),
	/* OPER_STATS_UPLINK_SERVER */
	_("Uplink server: %s"),
	/* OPER_STATS_UPLINK_CAPAB */
	_("Uplink capab: %s"),
	/* OPER_STATS_UPLINK_SERVER_COUNT */
	_("Servers found: %d"),
	/* OPER_MODE_SYNTAX */
	_("MODE channel modes"),
	/* OPER_UMODE_SYNTAX */
	_("UMODE nick modes"),
	/* OPER_UMODE_SUCCESS */
	_("Changed usermodes of %s."),
	/* OPER_UMODE_CHANGED */
	_("%s changed your usermodes."),
	/* OPER_OLINE_SYNTAX */
	_("OLINE nick flags"),
	/* OPER_OLINE_SUCCESS */
	_("Operflags %s have been added for %s."),
	/* OPER_OLINE_IRCOP */
	_("You are now an IRC Operator."),
	/* OPER_KICK_SYNTAX */
	_("KICK channel user reason"),
	/* OPER_SVSNICK_SYNTAX */
	_("SVSNICK nick newnick "),
	/* OPER_SVSNICK_NEWNICK */
	_("The nick %s is now being changed to %s."),
	/* OPER_AKILL_SYNTAX */
	_("AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {\037nick\037 | mask | entry-list} [reason]]"),
	/* OPER_AKILL_EXISTS */
	_("%s already exists on the AKILL list."),
	/* OPER_ALREADY_COVERED */
	_("%s is already covered by %s."),
	/* OPER_AKILL_NO_NICK */
	_("Reminder: AKILL masks cannot contain nicknames; make sure you have not included a nick portion in your mask."),
	/* OPER_AKILL_ADDED */
	_("%s added to the AKILL list."),
	/* OPER_EXPIRY_CHANGED */
	_("Expiry time of %s changed."),
	/* OPER_AKILL_NOT_FOUND */
	_("%s not found on the AKILL list."),
	/* OPER_AKILL_NO_MATCH */
	_("No matching entries on the AKILL list."),
	/* OPER_AKILL_DELETED */
	_("%s deleted from the AKILL list."),
	/* OPER_AKILL_DELETED_ONE */
	_("Deleted 1 entry from the AKILL list."),
	/* OPER_AKILL_DELETED_SEVERAL */
	_("Deleted %d entries from the AKILL list."),
	/* OPER_LIST_EMPTY */
	_("AKILL list is empty."),
	/* OPER_AKILL_LIST_HEADER */
	_("Current AKILL list:\n"
	"  Num   Mask                              Reason"),
	/* OPER_LIST_FORMAT */
	_("  %3d   %-32s  %s"),
	/* OPER_AKILL_VIEW_HEADER */
	_("Current AKILL list:"),
	/* OPER_VIEW_FORMAT */
	_("%3d  %s (by %s on %s; %s)\n"
	"      %s"),
	/* OPER_AKILL_CLEAR */
	_("The AKILL list has been cleared."),
	/* OPER_CHANKILL_SYNTAX */
	_("CHANKILL [+expiry] {#channel} [reason]"),
	/* OPER_SNLINE_SYNTAX */
	_("SNLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list}[:reason]]"),
	/* OPER_SNLINE_EXISTS */
	_("%s already exists on the SNLINE list."),
	/* OPER_SNLINE_ADDED */
	_("%s added to the SNLINE list."),
	/* OPER_SNLINE_NOT_FOUND */
	_("%s not found on the SNLINE list."),
	/* OPER_SNLINE_NO_MATCH */
	_("No matching entries on the SNLINE list."),
	/* OPER_SNLINE_DELETED */
	_("%s deleted from the SNLINE list."),
	/* OPER_SNLINE_DELETED_ONE */
	_("Deleted 1 entry from the SNLINE list."),
	/* OPER_SNLINE_DELETED_SEVERAL */
	_("Deleted %d entries from the SNLINE list."),
	/* OPER_SNLINE_LIST_EMPTY */
	_("SNLINE list is empty."),
	/* OPER_SNLINE_LIST_HEADER */
	_("Current SNLINE list:\n"
	"  Num   Mask                              Reason"),
	/* OPER_SNLINE_VIEW_HEADER */
	_("Current SNLINE list:"),
	/* OPER_SNLINE_CLEAR */
	_("The SNLINE list has been cleared."),
	/* OPER_SQLINE_SYNTAX */
	_("SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {\037nick\037 | mask | entry-list} [reason]]"),
	/* OPER_SQLINE_CHANNELS_UNSUPPORTED */
	_("Channel SQLINEs are not supported by your IRCd, so you can't use them."),
	/* OPER_SQLINE_EXISTS */
	_("%s already exists on the SQLINE list."),
	/* OPER_SQLINE_ADDED */
	_("%s added to the SQLINE list."),
	/* OPER_SQLINE_NOT_FOUND */
	_("%s not found on the SQLINE list."),
	/* OPER_SQLINE_NO_MATCH */
	_("No matching entries on the SQLINE list."),
	/* OPER_SQLINE_DELETED */
	_("%s deleted from the SQLINE list."),
	/* OPER_SQLINE_DELETED_ONE */
	_("Deleted 1 entry from the SQLINE list."),
	/* OPER_SQLINE_DELETED_SEVERAL */
	_("Deleted %d entries from the SQLINE list."),
	/* OPER_SQLINE_LIST_EMPTY */
	_("SQLINE list is empty."),
	/* OPER_SQLINE_LIST_HEADER */
	_("Current SQLINE list:\n"
	"  Num   Mask                              Reason"),
	/* OPER_SQLINE_VIEW_HEADER */
	_("Current SQLINE list:"),
	/* OPER_SQLINE_CLEAR */
	_("The SQLINE list has been cleared."),
	/* OPER_SZLINE_SYNTAX */
	_("SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {\037nick\037 | mask | entry-list} [reason]]"),
	/* OPER_SZLINE_EXISTS */
	_("%s already exists on the SZLINE list."),
	/* OPER_SZLINE_ONLY_IPS */
	_("Reminder: you can only add IP masks to the SZLINE list."),
	/* OPER_SZLINE_ADDED */
	_("%s added to the SZLINE list."),
	/* OPER_SZLINE_NOT_FOUND */
	_("%s not found on the SZLINE list."),
	/* OPER_SZLINE_NO_MATCH */
	_("No matching entries on the SZLINE list."),
	/* OPER_SZLINE_DELETED */
	_("%s deleted from the SZLINE list."),
	/* OPER_SZLINE_DELETED_ONE */
	_("Deleted 1 entry from the SZLINE list."),
	/* OPER_SZLINE_DELETED_SEVERAL */
	_("Deleted %d entries from the SZLINE list."),
	/* OPER_SZLINE_LIST_EMPTY */
	_("SZLINE list is empty."),
	/* OPER_SZLINE_LIST_HEADER */
	_("Current SZLINE list:\n"
	"  Num   Mask                              Reason"),
	/* OPER_SZLINE_VIEW_HEADER */
	_("Current SZLINE list:"),
	/* OPER_SZLINE_CLEAR */
	_("The SZLINE list has been cleared."),
	/* OPER_SET_SYNTAX */
	_("SET option setting"),
	/* OPER_SET_READONLY_ON */
	_("Services are now in read-only mode."),
	/* OPER_SET_READONLY_OFF */
	_("Services are now in read-write mode."),
	/* OPER_SET_READONLY_ERROR */
	_("Setting for READONLY must be ON or OFF."),
	/* OPER_SET_DEBUG_ON */
	_("Services are now in debug mode."),
	/* OPER_SET_DEBUG_OFF */
	_("Services are now in non-debug mode."),
	/* OPER_SET_DEBUG_LEVEL */
	_("Services are now in debug mode (level %d)."),
	/* OPER_SET_DEBUG_ERROR */
	_("Setting for DEBUG must be ON, OFF, or a positive number."),
	/* OPER_SET_NOEXPIRE_ON */
	_("Services are now in no expire mode."),
	/* OPER_SET_NOEXPIRE_OFF */
	_("Services are now in expire mode."),
	/* OPER_SET_NOEXPIRE_ERROR */
	_("Setting for NOEXPIRE must be ON or OFF."),
	/* OPER_SET_UNKNOWN_OPTION */
	_("Unknown option %s."),
	/* OPER_SET_LIST_OPTION_ON */
	_("%s is enabled"),
	/* OPER_SET_LIST_OPTION_OFF */
	_("%s is disabled"),
	/* OPER_NOOP_SYNTAX */
	_("NOOP {SET|REVOKE} server"),
	/* OPER_NOOP_SET */
	_("All O:lines of %s have been removed."),
	/* OPER_NOOP_REVOKE */
	_("All O:lines of %s have been reset."),
	/* OPER_JUPE_SYNTAX */
	_("JUPE servername [reason]"),
	/* OPER_JUPE_HOST_ERROR */
	_("Please use a valid server name when juping"),
	/* OPER_JUPE_INVALID_SERVER */
	_("You can not jupe your services server or your uplink server."),
	/* OPER_UPDATING */
	_("Updating databases."),
	/* OPER_RELOAD */
	_("Services' configuration file has been reloaded."),
	/* OPER_IGNORE_SYNTAX */
	_("IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick] [reason]"),
	/* OPER_IGNORE_VALID_TIME */
	_("You have to enter a valid number as time."),
	/* OPER_IGNORE_TIME_DONE */
	_("%s will now be ignored for %s."),
	/* OPER_IGNORE_PERM_DONE */
	_("%s will now permanently be ignored."),
	/* OPER_IGNORE_DEL_DONE */
	_("%s will no longer be ignored."),
	/* OPER_IGNORE_LIST */
	_("Services ignore list:\n"
	"  Mask        Creator      Reason      Expires"),
	/* OPER_IGNORE_LIST_NOMATCH */
	_("Nick %s not found on ignore list."),
	/* OPER_IGNORE_LIST_EMPTY */
	_("Ignore list is empty."),
	/* OPER_IGNORE_LIST_CLEARED */
	_("Ignore list has been cleared."),
	/* OPER_CHANLIST_HEADER */
	_("Channel list:\n"
	"Name                 Users Modes   Topic"),
	/* OPER_CHANLIST_HEADER_USER */
	_("%s channel list:\n"
	"Name                 Users Modes   Topic"),
	/* OPER_CHANLIST_RECORD */
	_("%-20s  %4d +%-6s %s"),
	/* OPER_CHANLIST_END */
	_("End of channel list."),
	/* OPER_USERLIST_HEADER */
	_("Users list:\n"
	"Nick                 Mask"),
	/* OPER_USERLIST_HEADER_CHAN */
	_("%s users list:\n"
	"Nick                 Mask"),
	/* OPER_USERLIST_RECORD */
	_("%-20s %s@%s"),
	/* OPER_USERLIST_END */
	_("End of users list."),
	/* OPER_SUPER_ADMIN_ON */
	_("You are now a SuperAdmin"),
	/* OPER_SUPER_ADMIN_OFF */
	_("You are no longer a SuperAdmin"),
	/* OPER_SUPER_ADMIN_SYNTAX */
	_("Setting for SuperAdmin must be ON or OFF (must be enabled in services.conf)"),
	/* OPER_SUPER_ADMIN_WALL_ON */
	_("%s is now a Super-Admin"),
	/* OPER_SUPER_ADMIN_WALL_OFF */
	_("%s is no longer a Super-Admin"),
	/* OPER_STAFF_LIST_HEADER */
	_("On Level Nick"),
	/* OPER_STAFF_FORMAT */
	_(" %c %s  %s"),
	/* OPER_STAFF_AFORMAT */
	_(" %c %s  %s [%s]"),
	/* OPER_DEFCON_SYNTAX */
	_("DEFCON [1|2|3|4|5]"),
	/* OPER_DEFCON_DENIED */
	_("Services are in Defcon mode, Please try again later."),
	/* OPER_DEFCON_CHANGED */
	_("Services are now at DEFCON %d"),
	/* OPER_DEFCON_WALL */
	_("%s Changed the DEFCON level to %d"),
	/* DEFCON_GLOBAL */
	_("The Defcon Level is now at Level: %d"),
	/* OPER_MODULE_LOADED */
	_("Module %s loaded"),
	/* OPER_MODULE_UNLOADED */
	_("Module %s unloaded"),
	/* OPER_MODULE_RELOADED */
	_("Module \002%s\002 reloaded"),
	/* OPER_MODULE_LOAD_FAIL */
	_("Unable to load module %s"),
	/* OPER_MODULE_REMOVE_FAIL */
	_("Unable to remove module %s"),
	/* OPER_MODULE_NO_UNLOAD */
	_("This module can not be unloaded."),
	/* OPER_MODULE_ALREADY_LOADED */
	_("Module %s is already loaded."),
	/* OPER_MODULE_ISNT_LOADED */
	_("Module %s isn't loaded."),
	/* OPER_MODULE_LOAD_SYNTAX */
	_("MODLOAD FileName"),
	/* OPER_MODULE_UNLOAD_SYNTAX */
	_("MODUNLOAD FileName"),
	/* OPER_MODULE_RELOAD_SYNTAX */
	_("MODRELOAD \037FileName\037"),
	/* OPER_MODULE_LIST_HEADER */
	_("Current Module list:"),
	/* OPER_MODULE_LIST */
	_("Module: %s [%s] [%s]"),
	/* OPER_MODULE_LIST_FOOTER */
	_("%d Modules loaded."),
	/* OPER_MODULE_INFO_LIST */
	_("Module: %s Version: %s Author: %s loaded: %s"),
	/* OPER_MODULE_CMD_LIST */
	_("Providing command: %R%s %s"),
	/* OPER_MODULE_NO_LIST */
	_("No modules currently loaded"),
	/* OPER_MODULE_NO_INFO */
	_("No information about module %s is available"),
	/* OPER_MODULE_INFO_SYNTAX */
	_("MODINFO FileName"),
	/* OPER_EXCEPTION_SYNTAX */
	_("EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [params]"),
	/* OPER_EXCEPTION_DISABLED */
	_("Session limiting is disabled."),
	/* OPER_EXCEPTION_ADDED */
	_("Session limit for %s set to %d."),
	/* OPER_EXCEPTION_MOVED */
	_("Exception for %s (#%d) moved to position %d."),
	/* OPER_EXCEPTION_NOT_FOUND */
	_("%s not found on session-limit exception list."),
	/* OPER_EXCEPTION_NO_MATCH */
	_("No matching entries on session-limit exception list."),
	/* OPER_EXCEPTION_DELETED */
	_("%s deleted from session-limit exception list."),
	/* OPER_EXCEPTION_DELETED_ONE */
	_("Deleted 1 entry from session-limit exception list."),
	/* OPER_EXCEPTION_DELETED_SEVERAL */
	_("Deleted %d entries from session-limit exception list."),
	/* OPER_EXCEPTION_LIST_HEADER */
	_("Current Session Limit Exception list:"),
	/* OPER_EXCEPTION_LIST_FORMAT */
	_("%3d  %4d   %s"),
	/* OPER_EXCEPTION_LIST_COLHEAD */
	_("Num  Limit  Host"),
	/* OPER_EXCEPTION_VIEW_FORMAT */
	_("%3d.  %s  (by %s on %s; %s)\n"
	"    Limit: %-4d  - %s"),
	/* OPER_EXCEPTION_INVALID_LIMIT */
	_("Invalid session limit. It must be a valid integer greater than or equal to zero and less than %d."),
	/* OPER_EXCEPTION_INVALID_HOSTMASK */
	_("Invalid hostmask. Only real hostmasks are valid as exceptions are not matched against nicks or usernames."),
	/* OPER_EXCEPTION_EXISTS */
	_("%s already exists on the EXCEPTION list."),
	/* OPER_EXCEPTION_CHANGED */
	_("Exception for %s has been updated to %d."),
	/* OPER_SESSION_LIST_SYNTAX */
	_("SESSION LIST limit"),
	/* OPER_SESSION_INVALID_THRESHOLD */
	_("Invalid threshold value. It must be a valid integer greater than 1."),
	/* OPER_SESSION_NOT_FOUND */
	_("%s not found on session list."),
	/* OPER_SESSION_LIST_HEADER */
	_("Hosts with at least %d sessions:"),
	/* OPER_SESSION_LIST_COLHEAD */
	_("Sessions  Host"),
	/* OPER_SESSION_LIST_FORMAT */
	_("%6d    %s"),
	/* OPER_SESSION_VIEW_FORMAT */
	_("The host %s currently has %d sessions with a limit of %d."),
	/* OPER_HELP_EXCEPTION */
	_("Syntax: EXCEPTION ADD [+expiry] mask limit reason\n"
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
	_("Syntax: SESSION LIST threshold\n"
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
	_("Syntax: STAFF\n"
	"Displays all Services Staff nicks along with level\n"
	"and on-line status."),
	/* OPER_HELP_DEFCON */
	_("Syntax: DEFCON [1|2|3|4|5]\n"
	"The defcon system can be used to implement a pre-defined\n"
	"set of restrictions to services useful during an attempted\n"
	"attack on the network."),
	/* OPER_HELP_DEFCON_NO_NEW_CHANNELS */
	_("* No new channel registrations"),
	/* OPER_HELP_DEFCON_NO_NEW_NICKS */
	_("* No new nick registrations"),
	/* OPER_HELP_DEFCON_NO_MLOCK_CHANGE */
	_("* No MLOCK changes"),
	/* OPER_HELP_DEFCON_FORCE_CHAN_MODES */
	_("* Force Chan Modes (%s) to be set on all channels"),
	/* OPER_HELP_DEFCON_REDUCE_SESSION */
	_("* Use the reduced session limit of %d"),
	/* OPER_HELP_DEFCON_NO_NEW_CLIENTS */
	_("* Kill any NEW clients connecting"),
	/* OPER_HELP_DEFCON_OPER_ONLY */
	_("* Ignore any non-opers with message"),
	/* OPER_HELP_DEFCON_SILENT_OPER_ONLY */
	_("* Silently ignore non-opers"),
	/* OPER_HELP_DEFCON_AKILL_NEW_CLIENTS */
	_("* AKILL any new clients connecting"),
	/* OPER_HELP_DEFCON_NO_NEW_MEMOS */
	_("* No new memos sent"),
	/* OPER_HELP_CHANKILL */
	_("Syntax: CHANKILL [+expiry] channel reason\n"
	"Puts an AKILL for every nick on the specified channel. It\n"
	"uses the entire and complete real ident@host for every nick,\n"
	"then enforces the AKILL."),
	/* NEWS_LOGON_TEXT */
	_("[Logon News - %s] %s"),
	/* NEWS_OPER_TEXT */
	_("[Oper News - %s] %s"),
	/* NEWS_RANDOM_TEXT */
	_("[Random News - %s] %s"),
	/* NEWS_LOGON_SYNTAX */
	_("LOGONNEWS {ADD|DEL|LIST} [text|num]"),
	/* NEWS_LOGON_LIST_HEADER */
	_("Logon news items:"),
	/* NEWS_LIST_ENTRY */
	_("%5d (%s by %s)\n"
	"    %s"),
	/* NEWS_LOGON_LIST_NONE */
	_("There is no logon news."),
	/* NEWS_LOGON_ADD_SYNTAX */
	_("Syntax: LOGONNEWS ADD text"),
	/* NEWS_ADD_FULL */
	_("News list is full!"),
	/* NEWS_LOGON_ADDED */
	_("Added new logon news item (#%d)."),
	/* NEWS_LOGON_DEL_SYNTAX */
	_("Syntax: LOGONNEWS DEL {num | ALL}"),
	/* NEWS_LOGON_DEL_NOT_FOUND */
	_("Logon news item #%d not found!"),
	/* NEWS_LOGON_DELETED */
	_("Logon news item #%d deleted."),
	/* NEWS_LOGON_DEL_NONE */
	_("No logon news items to delete!"),
	/* NEWS_LOGON_DELETED_ALL */
	_("All logon news items deleted."),
	/* NEWS_OPER_SYNTAX */
	_("OPERNEWS {ADD|DEL|LIST} [text|num]"),
	/* NEWS_OPER_LIST_HEADER */
	_("Oper news items:"),
	/* NEWS_OPER_LIST_NONE */
	_("There is no oper news."),
	/* NEWS_OPER_ADD_SYNTAX */
	_("Syntax: OPERNEWS ADD text"),
	/* NEWS_OPER_ADDED */
	_("Added new oper news item (#%d)."),
	/* NEWS_OPER_DEL_SYNTAX */
	_("Syntax: OPERNEWS DEL {num | ALL}"),
	/* NEWS_OPER_DEL_NOT_FOUND */
	_("Oper news item #%d not found!"),
	/* NEWS_OPER_DELETED */
	_("Oper news item #%d deleted."),
	/* NEWS_OPER_DEL_NONE */
	_("No oper news items to delete!"),
	/* NEWS_OPER_DELETED_ALL */
	_("All oper news items deleted."),
	/* NEWS_RANDOM_SYNTAX */
	_("RANDOMNEWS {ADD|DEL|LIST} [text|num]"),
	/* NEWS_RANDOM_LIST_HEADER */
	_("Random news items:"),
	/* NEWS_RANDOM_LIST_NONE */
	_("There is no random news."),
	/* NEWS_RANDOM_ADD_SYNTAX */
	_("Syntax: RANDOMNEWS ADD text"),
	/* NEWS_RANDOM_ADDED */
	_("Added new random news item (#%d)."),
	/* NEWS_RANDOM_DEL_SYNTAX */
	_("Syntax: RANDOMNEWS DEL {num | ALL}"),
	/* NEWS_RANDOM_DEL_NOT_FOUND */
	_("Random news item #%d not found!"),
	/* NEWS_RANDOM_DELETED */
	_("Random news item #%d deleted."),
	/* NEWS_RANDOM_DEL_NONE */
	_("No random news items to delete!"),
	/* NEWS_RANDOM_DELETED_ALL */
	_("All random news items deleted."),
	/* NEWS_HELP_LOGON */
	_("Syntax: LOGONNEWS ADD text\n"
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
	_("Syntax: OPERNEWS ADD text\n"
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
	_("Syntax: RANDOMNEWS ADD text\n"
	"        RANDOMNEWS DEL {num | ALL}\n"
	"        RANDOMNEWS LIST\n"
	" \n"
	"Edits or displays the list of random news messages.  When a\n"
	"user connects to the network, one (and only one) of the\n"
	"random news will be randomly chosen and sent to them.\n"
	" \n"
	"RANDOMNEWS may only be used by Services Operators."),
	/* NICK_HELP_CMD_CONFIRM */
	_("    CONFIRM    Confirm a nickserv auth code"),
	/* NICK_HELP_CMD_RESEND */
	_("    RESEND     Resend a nickserv auth code"),
	/* NICK_HELP_CMD_REGISTER */
	_("    REGISTER   Register a nickname"),
	/* NICK_HELP_CMD_GROUP */
	_("    GROUP      Join a group"),
	/* NICK_HELP_CMD_UNGROUP */
	_("    UNGROUP    Remove a nick from a group"),
	/* NICK_HELP_CMD_IDENTIFY */
	_("    IDENTIFY   Identify yourself with your password"),
	/* NICK_HELP_CMD_ACCESS */
	_("    ACCESS     Modify the list of authorized addresses"),
	/* NICK_HELP_CMD_SET */
	_("    SET        Set options, including kill protection"),
	/* NICK_HELP_CMD_SASET */
	_("    SASET      Set SET-options on another nickname"),
	/* NICK_HELP_CMD_DROP */
	_("    DROP       Cancel the registration of a nickname"),
	/* NICK_HELP_CMD_RECOVER */
	_("    RECOVER    Kill another user who has taken your nick"),
	/* NICK_HELP_CMD_RELEASE */
	_("    RELEASE    Regain custody of your nick after RECOVER"),
	/* NICK_HELP_CMD_SENDPASS */
	_("    SENDPASS   Forgot your password? Try this"),
	/* NICK_HELP_CMD_RESETPASS */
	_("    RESETPASS  Helps you reset lost passwords"),
	/* NICK_HELP_CMD_GHOST */
	_("    GHOST      Disconnects a \"ghost\" IRC session using your nick"),
	/* NICK_HELP_CMD_ALIST */
	_("    ALIST      List channels you have access on"),
	/* NICK_HELP_CMD_GLIST */
	_("    GLIST      Lists all nicknames in your group"),
	/* NICK_HELP_CMD_INFO */
	_("    INFO       Displays information about a given nickname"),
	/* NICK_HELP_CMD_LIST */
	_("    LIST       List all registered nicknames that match a given pattern"),
	/* NICK_HELP_CMD_LOGOUT */
	_("    LOGOUT     Reverses the effect of the IDENTIFY command"),
	/* NICK_HELP_CMD_STATUS */
	_("    STATUS     Returns the owner status of the given nickname"),
	/* NICK_HELP_CMD_UPDATE */
	_("    UPDATE     Updates your current status, i.e. it checks for new memos"),
	/* NICK_HELP_CMD_GETPASS */
	_("    GETPASS    Retrieve the password for a nickname"),
	/* NICK_HELP_CMD_GETEMAIL */
	_("    GETEMAIL   Matches and returns all users that registered using given email"),
	/* NICK_HELP_CMD_FORBID */
	_("    FORBID     Prevents a nickname from being registered"),
	/* NICK_HELP_CMD_SUSPEND */
	_("    SUSPEND    Suspend a given nick"),
	/* NICK_HELP_CMD_UNSUSPEND */
	_("    UNSUSPEND  Unsuspend a given nick"),
	/* NICK_HELP */
	_("%S allows you to \"register\" a nickname and\n"
	"prevent others from using it. The following\n"
	"commands allow for registration and maintenance of\n"
	"nicknames; to use them, type %R%S command.\n"
	"For more information on a specific command, type\n"
	"%R%S HELP command."),
	/* NICK_HELP_FOOTER */
	_(" \n"
	"NOTICE: This service is intended to provide a way for\n"
	"IRC users to ensure their identity is not compromised.\n"
	"It is NOT intended to facilitate \"stealing\" of\n"
	"nicknames or other malicious actions.  Abuse of %S\n"
	"will result in, at minimum, loss of the abused\n"
	"nickname(s)."),
	/* NICK_HELP_EXPIRES */
	_("Nicknames that are not used anymore are subject to \n"
	"the automatic expiration, i.e. they will be deleted\n"
	"after %d days if not used."),
	/* NICK_HELP_REGISTER */
	_("Syntax: REGISTER password [email]\n"
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
	_("Syntax: GROUP target password\n"
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
	_("Syntax: UNGROUP [nick]\n"
	" \n"
	"This command ungroups your nick, or if given, the specificed nick,\n"
	"from the group it is in. The ungrouped nick keeps its registration\n"
	"time, password, email, greet, language, url, and icq. Everything\n"
	"else is reset. You may not ungroup yourself if there is only one\n"
	"nick in your group."),
	/* NICK_HELP_IDENTIFY */
	_("Syntax: IDENTIFY [account] password\n"
	" \n"
	"Tells %S that you are really the owner of this\n"
	"nick.  Many commands require you to authenticate yourself\n"
	"with this command before you use them.  The password\n"
	"should be the same one you sent with the REGISTER\n"
	"command."),
	/* NICK_HELP_UPDATE */
	_("Syntax: UPDATE\n"
	"Updates your current status, i.e. it checks for new memos,\n"
	"sets needed chanmodes (ModeonID) and updates your vhost and\n"
	"your userflags (lastseentime, etc)."),
	/* NICK_HELP_LOGOUT */
	_("Syntax: LOGOUT\n"
	" \n"
	"This reverses the effect of the IDENTIFY command, i.e.\n"
	"make you not recognized as the real owner of the nick\n"
	"anymore. Note, however, that you won't be asked to reidentify\n"
	"yourself."),
	/* NICK_HELP_DROP */
	_("Syntax: DROP [nickname | \037password\037]\n"
	" \n"
	"Drops your nickname from the %S database.  A nick\n"
	"that has been dropped is free for anyone to re-register.\n"
	" \n"
	"You may drop a nick within your group by passing it\n"
	"as the nick parameter.\n"
	" \n"
	"If you have a nickname registration pending but can not confirm\n"
	"it for any reason, you can cancel your registration by passing\n"
	"your password as the \002password\002 parameter.\n"
	" \n"
	"In order to use this command, you must first identify\n"
	"with your password (%R%S HELP IDENTIFY for more\n"
	"information)."),
	/* NICK_HELP_ACCESS */
	_("Syntax: ACCESS ADD mask\n"
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
	_("Syntax: SET option parameters\n"
	" \n"
	"Sets various nickname options.  option can be one of:"),
	/* NICK_HELP_CMD_SET_DISPLAY */
	_("    DISPLAY    Set the display of your group in Services"),
	/* NICK_HELP_CMD_SET_PASSWORD */
	_("    PASSWORD   Set your nickname password"),
	/* NICK_HELP_CMD_SET_LANGUAGE */
	_("    LANGUAGE   Set the language Services will use when\n"
	"                   sending messages to you"),
	/* NICK_HELP_CMD_SET_EMAIL */
	_("    EMAIL      Associate an E-mail address with your nickname"),
	/* NICK_HELP_CMD_SET_GREET */
	_("    GREET      Associate a greet message with your nickname"),
	/* NICK_HELP_CMD_SET_KILL */
	_("    KILL       Turn protection on or off"),
	/* NICK_HELP_CMD_SET_SECURE */
	_("    SECURE     Turn nickname security on or off"),
	/* NICK_HELP_CMD_SET_PRIVATE */
	_("    PRIVATE    Prevent your nickname from appearing in a\n"
	"                   %R%S LIST"),
	/* NICK_HELP_CMD_SET_HIDE */
	_("    HIDE       Hide certain pieces of nickname information"),
	/* NICK_HELP_CMD_SET_MSG */
	_("    MSG        Change the communication method of Services"),
	/* NICK_HELP_CMD_SET_AUTOOP */
	_("    AUTOOP     Should services op you automatically.    "),
	/* NICK_HELP_SET_TAIL */
	_("In order to use this command, you must first identify\n"
	"with your password (%R%S HELP IDENTIFY for more\n"
	"information).\n"
	" \n"
	"Type %R%S HELP SET option for more information\n"
	"on a specific option."),
	/* NICK_HELP_SET_DISPLAY */
	_("Syntax: SET DISPLAY new-display\n"
	" \n"
	"Changes the display used to refer to your nickname group in \n"
	"Services. The new display MUST be a nick of your group."),
	/* NICK_HELP_SET_PASSWORD */
	_("Syntax: SET PASSWORD new-password\n"
	" \n"
	"Changes the password used to identify you as the nick's\n"
	"owner."),
	/* NICK_HELP_SET_LANGUAGE */
	_("Syntax: SET LANGUAGE language\n"
	" \n"
	"Changes the language Services uses when sending messages to\n"
	"you (for example, when responding to a command you send).\n"
	"language should be chosen from the following list of\n"
	"supported languages:"),
	/* NICK_HELP_SET_EMAIL */
	_("Syntax: SET EMAIL address\n"
	" \n"
	"Associates the given E-mail address with your nickname.\n"
	"This address will be displayed whenever someone requests\n"
	"information on the nickname with the INFO command."),
	/* NICK_HELP_SET_GREET */
	_("Syntax: SET GREET message\n"
	" \n"
	"Makes the given message the greet of your nickname, that\n"
	"will be displayed when joining a channel that has GREET\n"
	"option enabled, provided that you have the necessary \n"
	"access on it."),
	/* NICK_HELP_SET_KILL */
	_("Syntax: SET KILL {ON | QUICK | IMMED | OFF}\n"
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
	_("Syntax: SET SECURE {ON | OFF}\n"
	" \n"
	"Turns %S's security features on or off for your\n"
	"nick.  With SECURE set, you must enter your password\n"
	"before you will be recognized as the owner of the nick,\n"
	"regardless of whether your address is on the access\n"
	"list.  However, if you are on the access list, %S\n"
	"will not auto-kill you regardless of the setting of the\n"
	"KILL option."),
	/* NICK_HELP_SET_PRIVATE */
	_("Syntax: SET PRIVATE {ON | OFF}\n"
	" \n"
	"Turns %S's privacy option on or off for your nick.\n"
	"With PRIVATE set, your nickname will not appear in\n"
	"nickname lists generated with %S's LIST command.\n"
	"(However, anyone who knows your nickname can still get\n"
	"information on it using the INFO command.)"),
	/* NICK_HELP_SET_HIDE */
	_("Syntax: SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}\n"
	" \n"
	"Allows you to prevent certain pieces of information from\n"
	"being displayed when someone does a %S INFO on your\n"
	"nick.  You can hide your E-mail address (EMAIL), last seen\n"
	"user@host mask (USERMASK), your services access status\n"
	"(STATUS) and  last quit message (QUIT).\n"
	"The second parameter specifies whether the information should\n"
	"be displayed (OFF) or hidden (ON)."),
	/* NICK_HELP_SET_MSG */
	_("Syntax: SET MSG {ON | OFF}\n"
	" \n"
	"Allows you to choose the way Services are communicating with \n"
	"you. With MSG set, Services will use messages, else they'll \n"
	"use notices."),
	/* NICK_HELP_SET_AUTOOP */
	_("Syntax: SET AUTOOP {ON | OFF}\n"
	" \n"
	"Sets whether you will be opped automatically. Set to ON to \n"
	"allow ChanServ to op you automatically when entering channels."),
	/* NICK_HELP_SASET_HEAD */
	_("Syntax: SASET nickname option parameters.\n"
	" \n"
	"Sets various nickname options.  option can be one of:"),
	/* NICK_HELP_CMD_SASET_DISPLAY */
	_("    DISPLAY    Set the display of the group in Services"),
	/* NICK_HELP_CMD_SASET_PASSWORD */
	_("    PASSWORD   Set the nickname password"),
	/* NICK_HELP_CMD_SASET_EMAIL */
	_("    EMAIL      Associate an E-mail address with the nickname"),
	/* NICK_HELP_CMD_SASET_GREET */
	_("    GREET      Associate a greet message with the nickname"),
	/* NICK_HELP_CMD_SASET_PRIVATE */
	_("    PRIVATE    Prevent the nickname from appearing in a\n"
	"                   %R%S LIST"),
	/* NICK_HELP_CMD_SASET_NOEXPIRE */
	_("    NOEXPIRE   Prevent the nickname from expiring"),
	/* NICK_HELP_CMD_SASET_AUTOOP */
	_("    AUTOOP     Turn autoop on or off"),
	/* NICK_HELP_CMD_SASET_LANGUAGE */
	_("    LANGUAGE   Set the language Services will use when\n"
	"               sending messages to nickname"),
	/* NICK_HELP_SASET_TAIL */
	_("Type %R%S HELP SASET option for more information\n"
	"on a specific option. The options will be set on the given\n"
	"nickname."),
	/* NICK_HELP_SASET_DISPLAY */
	_("Syntax: SASET nickname DISPLAY new-display\n"
	" \n"
	"Changes the display used to refer to the nickname group in \n"
	"Services. The new display MUST be a nick of the group."),
	/* NICK_HELP_SASET_PASSWORD */
	_("Syntax: SASET nickname PASSWORD new-password\n"
	" \n"
	"Changes the password used to identify as the nick's	owner."),
	/* NICK_HELP_SASET_EMAIL */
	_("Syntax: SASET nickname EMAIL address\n"
	" \n"
	"Associates the given E-mail address with the nickname."),
	/* NICK_HELP_SASET_GREET */
	_("Syntax: SASET nickname GREET message\n"
	" \n"
	"Makes the given message the greet of the nickname, that\n"
	"will be displayed when joining a channel that has GREET\n"
	"option enabled, provided that the user has the necessary \n"
	"access on it."),
	/* NICK_HELP_SASET_KILL */
	_("Syntax: SASET nickname KILL {ON | QUICK | IMMED | OFF}\n"
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
	_("Syntax: SASET nickname SECURE {ON | OFF}\n"
	" \n"
	"Turns %S's security features on or off for your\n"
	"nick.  With SECURE set, you must enter your password\n"
	"before you will be recognized as the owner of the nick,\n"
	"regardless of whether your address is on the access\n"
	"list.  However, if you are on the access list, %S\n"
	"will not auto-kill you regardless of the setting of the\n"
	"KILL option."),
	/* NICK_HELP_SASET_PRIVATE */
	_("Syntax: SASET nickname PRIVATE {ON | OFF}\n"
	" \n"
	"Turns %S's privacy option on or off for the nick.\n"
	"With PRIVATE set, the nickname will not appear in\n"
	"nickname lists generated with %S's LIST command.\n"
	"(However, anyone who knows the nickname can still get\n"
	"information on it using the INFO command.)"),
	/* NICK_HELP_SASET_HIDE */
	_("Syntax: SASET nickname HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}\n"
	" \n"
	"Allows you to prevent certain pieces of information from\n"
	"being displayed when someone does a %S INFO on the\n"
	"nick.  You can hide the E-mail address (EMAIL), last seen\n"
	"user@host mask (USERMASK), the services access status\n"
	"(STATUS) and  last quit message (QUIT).\n"
	"The second parameter specifies whether the information should\n"
	"be displayed (OFF) or hidden (ON)."),
	/* NICK_HELP_SASET_MSG */
	_("Syntax: SASET nickname MSG {ON | OFF}\n"
	" \n"
	"Allows you to choose the way Services are communicating with \n"
	"the given user. With MSG set, Services will use messages,\n"
	"else they'll use notices."),
	/* NICK_HELP_SASET_NOEXPIRE */
	_("Syntax: SASET nickname NOEXPIRE {ON | OFF}\n"
	" \n"
	"Sets whether the given nickname will expire.  Setting this\n"
	"to ON prevents the nickname from expiring."),
	/* NICK_HELP_SASET_AUTOOP */
	_("Syntax: SASET nickname AUTOOP {ON | OFF}\n"
	" \n"
	"Sets whether the given nickname will be opped automatically.\n"
	"Set to ON to allow ChanServ to op the given nickname \n"
	"automatically when joining channels."),
	/* NICK_HELP_SASET_LANGUAGE */
	_("Syntax: SASET nickname LANGUAGE language\n"
	" \n"
	"Changes the language Services uses when sending messages to\n"
	"nickname (for example, when responding to a command he sends).\n"
	"language should be chosen from a list of supported languages\n"
	"that you can get by typing %R%S HELP SET LANGUAGE."),
	/* NICK_HELP_RECOVER */
	_("Syntax: RECOVER nickname [password]\n"
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
	_("Syntax: RELEASE nickname [password]\n"
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
	_("Syntax: GHOST nickname [password]\n"
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
	_("Syntax: INFO nickname\n"
	" \n"
	"Displays information about the given nickname, such as\n"
	"the nick's owner, last seen address and time, and nick\n"
	"options."),
	/* NICK_HELP_LIST */
	_("Syntax: LIST pattern\n"
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
	_("Syntax: ALIST [level]\n"
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
	_("Syntax: GLIST\n"
	" \n"
	"Lists all nicks in your group."),
	/* NICK_HELP_STATUS */
	_("Syntax: STATUS nickname...\n"
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
	_("Syntax: SENDPASS nickname\n"
	" \n"
	"Send the password of the given nickname to the e-mail address\n"
	"set in the nickname record. This command is really useful\n"
	"to deal with lost passwords.\n"
	" \n"
	"May be limited to IRC operators on certain networks."),
	/* NICK_HELP_RESETPASS */
	_("Syntax: RESETPASS nickname\n"
	"Sends a code key to the nickname with instructions on how to\n"
	"reset their password."),
	/* NICK_HELP_CONFIRM */
	_("Syntax: CONFIRM passcode\n"
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
	_("Additionally, Services Operators with the nickserv/confirm permission can\n"
	"replace passcode with a users nick to force validate them."),
	/* NICK_HELP_RESEND */
	_("Syntax: RESEND\n"
	" \n"
	"This command will re-send the auth code (also called passcode)\n"
	"to the e-mail address of the user whom is performing it."),
	/* NICK_SERVADMIN_HELP */
	_(" \n"
	"Services Operators can also drop any nickname without needing\n"
	"to identify for the nick, and may view the access list for\n"
	"any nickname (%R%S ACCESS LIST nick)."),
	/* NICK_SERVADMIN_HELP_LOGOUT */
	_("Syntax: LOGOUT [nickname [REVALIDATE]]\n"
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
	_("Syntax: DROP [nickname]\n"
	" \n"
	"Without a parameter, drops your nickname from the\n"
	"%S database.\n"
	" \n"
	"With a parameter, drops the named nick from the database.\n"
	"You may drop any nick within your group without any \n"
	"special privileges. Dropping any nick is limited to \n"
	"Services Operators."),
	/* NICK_SERVADMIN_HELP_LIST */
	_("Syntax: LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]\n"
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
	_("Syntax: ALIST [nickname] [level]\n"
	" \n"
	"With no parameters, lists channels you have access on. With\n"
	"one parameter, lists channels that nickname has access \n"
	"on. With two parameters lists channels that nickname has \n"
	"level access or greater on.\n"
	"This use limited to Services Operators."),
	/* NICK_SERVADMIN_HELP_GLIST */
	_("Syntax: GLIST [nickname]\n"
	" \n"
	"Without a parameter, lists all nicknames that are in\n"
	"your group.\n"
	" \n"
	"With a parameter, lists all nicknames that are in the\n"
	"group of the given nick.\n"
	"This use limited to Services Operators."),
	/* NICK_SERVADMIN_HELP_GETPASS */
	_("Syntax: GETPASS nickname\n"
	" \n"
	"Returns the password for the given nickname.  Note that\n"
	"whenever this command is used, a message including the\n"
	"person who issued the command and the nickname it was used\n"
	"on will be logged and sent out as a WALLOPS/GLOBOPS.\n"
	" \n"
	"This command is unavailable when encryption is enabled."),
	/* NICK_SERVADMIN_HELP_GETEMAIL */
	_("Syntax: GETEMAIL user@emailhost\n"
	"Returns the matching nicks that used given email. Note that\n"
	"you can not use wildcards for either user or emailhost. Whenever\n"
	"this command is used, a message including the person who issued\n"
	"the command and the email it was used on will be logged."),
	/* NICK_SERVADMIN_HELP_FORBID */
	_("Syntax: FORBID nickname [reason]\n"
	" \n"
	"Disallows a nickname from being registered or used by\n"
	"anyone.  May be cancelled by dropping the nick.\n"
	" \n"
	"On certain networks, reason is required."),
	/* NICK_SERVADMIN_HELP_SUSPEND */
	_("Syntax: SUSPEND nickname reason\n"
	"SUSPENDs a nickname from being used."),
	/* NICK_SERVADMIN_HELP_UNSUSPEND */
	_("Syntax: UNSUSPEND nickname\n"
	"UNSUSPENDS a nickname from being used."),
	/* CHAN_HELP_CMD_FORBID */
	_("    FORBID     Prevent a channel from being used"),
	/* CHAN_HELP_CMD_SUSPEND */
	_("    SUSPEND    Prevent a channel from being used preserving\n"
	"               channel data and settings"),
	/* CHAN_HELP_CMD_UNSUSPEND */
	_("    UNSUSPEND  Releases a suspended channel"),
	/* CHAN_HELP_CMD_STATUS */
	_("    STATUS     Returns the current access level of a user\n"
	"               on a channel"),
	/* CHAN_HELP_CMD_SET */
	_("    SET        Set channel options and information"),
	/* CHAN_HELP_CMD_SASET */
	_("    SASET      Forcefully set channel options and information"),
	/* CHAN_HELP_CMD_QOP */
	_("    QOP        Modify the list of QOP users"),
	/* CHAN_HELP_CMD_AOP */
	_("    AOP        Modify the list of AOP users"),
	/* CHAN_HELP_CMD_SOP */
	_("    SOP        Modify the list of SOP users"),
	/* CHAN_HELP_CMD_ACCESS */
	_("    ACCESS     Modify the list of privileged users"),
	/* CHAN_HELP_CMD_LEVELS */
	_("    LEVELS     Redefine the meanings of access levels"),
	/* CHAN_HELP_CMD_AKICK */
	_("    AKICK      Maintain the AutoKick list"),
	/* CHAN_HELP_CMD_DROP */
	_("    DROP       Cancel the registration of a channel"),
	/* CHAN_HELP_CMD_BAN */
	_("    BAN        Bans a selected nick on a channel"),
	/* CHAN_HELP_CMD_CLEARUSERS */
	_("    CLEARUSERS Tells ChanServ to clear (kick) all users on a channel"),
	/* CHAN_HELP_CMD_DEVOICE */
	_("    DEVOICE    Devoices a selected nick on a channel"),
	/* CHAN_HELP_CMD_GETKEY */
	_("    GETKEY     Returns the key of the given channel"),
	/* CHAN_HELP_CMD_INFO */
	_("    INFO       Lists information about the named registered channel"),
	/* CHAN_HELP_CMD_INVITE */
	_("    INVITE     Tells ChanServ to invite you into a channel"),
	/* CHAN_HELP_CMD_KICK */
	_("    KICK       Kicks a selected nick from a channel"),
	/* CHAN_HELP_CMD_LIST */
	_("    LIST       Lists all registered channels matching the given pattern"),
	/* CHAN_HELP_CMD_OP */
	_("    OP         Gives Op status to a selected nick on a channel"),
	/* CHAN_HELP_CMD_TOPIC */
	_("    TOPIC      Manipulate the topic of the specified channel"),
	/* CHAN_HELP_CMD_UNBAN */
	_("    UNBAN      Remove all bans preventing a user from entering a channel"),
	/* CHAN_HELP_CMD_VOICE */
	_("    VOICE      Voices a selected nick on a channel"),
	/* CHAN_HELP_CMD_VOP */
	_("    VOP        Maintains the VOP (VOicePeople) list for a channel"),
	/* CHAN_HELP_CMD_DEHALFOP */
	_("    DEHALFOP   Dehalfops a selected nick on a channel"),
	/* CHAN_HELP_CMD_DEOWNER */
	_("    DEOWNER    Removes your owner status on a channel"),
	/* CHAN_HELP_CMD_DEPROTECT */
	_("    DEPROTECT  Deprotects a selected nick on a channel"),
	/* CHAN_HELP_CMD_HALFOP */
	_("    HALFOP     Halfops a selected nick on a channel"),
	/* CHAN_HELP_CMD_HOP */
	_("    HOP        Maintains the HOP (HalfOP) list for a channel"),
	/* CHAN_HELP_CMD_OWNER */
	_("    OWNER      Gives you owner status on channel"),
	/* CHAN_HELP_CMD_PROTECT */
	_("    PROTECT    Protects a selected nick on a channel"),
	/* CHAN_HELP_CMD_DEOP */
	_("    DEOP       Deops a selected nick on a channel"),
	/* CHAN_HELP_CMD_CLONE */
	_("    CLONE      Copy all settings from one channel to another"),
	/* CHAN_HELP_CMD_MODE */
	_("    MODE       Control modes and mode locks on a channel"),
	/* CHAN_HELP_CMD_ENTRYMSG */
	_("    ENTRYMSG   Manage the channel's entrymsgs"),
	/* CHAN_HELP */
	_("%S allows you to register and control various\n"
	"aspects of channels.  %S can often prevent\n"
	"malicious users from \"taking over\" channels by limiting\n"
	"who is allowed channel operator privileges.  Available\n"
	"commands are listed below; to use them, type\n"
	"%R%S command.  For more information on a\n"
	"specific command, type %R%S HELP command."),
	/* CHAN_HELP_EXPIRES */
	_("Note that any channel which is not used for %d days\n"
	"(i.e. which no user on the channel's access list enters\n"
	"for that period of time) will be automatically dropped."),
	/* CHAN_HELP_REGISTER */
	_("Syntax: REGISTER channel description\n"
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
	_("Syntax: DROP channel\n"
	" \n"
	"Unregisters the named channel.  Can only be used by\n"
	"channel founder."),
	/* CHAN_HELP_SASET_HEAD */
	_("Syntax: SASET channel option parameters\n"
	" \n"
	"Allows Services Operators to forcefully change settings\n"
	"on channels.\n"
	" \n"
	"Available options:"),
	/* CHAN_HELP_SET_HEAD */
	_("Syntax: SET channel option parameters\n"
	" \n"
	"Allows the channel founder to set various channel options\n"
	"and other information.\n"
	" \n"
	"Available options:"),
	/* CHAN_HELP_CMD_SET_FOUNDER */
	_("    FOUNDER       Set the founder of a channel"),
	/* CHAN_HELP_CMD_SET_SUCCESSOR */
	_("    SUCCESSOR     Set the successor for a channel"),
	/* CHAN_HELP_CMD_SET_DESC */
	_("    DESC          Set the channel description"),
	/* CHAN_HELP_CMD_SET_BANTYPE */
	_("    BANTYPE       Set how Services make bans on the channel"),
	/* CHAN_HELP_CMD_SET_KEEPTOPIC */
	_("    KEEPTOPIC     Retain topic when channel is not in use"),
	/* CHAN_HELP_CMD_SET_OPNOTICE */
	_("    OPNOTICE      Send a notice when OP/DEOP commands are used"),
	/* CHAN_HELP_CMD_SET_PEACE */
	_("    PEACE         Regulate the use of critical commands"),
	/* CHAN_HELP_CMD_SET_PRIVATE */
	_("    PRIVATE       Hide channel from LIST command"),
	/* CHAN_HELP_CMD_SET_RESTRICTED */
	_("    RESTRICTED    Restrict access to the channel"),
	/* CHAN_HELP_CMD_SET_SECURE */
	_("    SECURE        Activate %S security features"),
	/* CHAN_HELP_CMD_SET_SECUREOPS */
	_("    SECUREOPS     Stricter control of chanop status"),
	/* CHAN_HELP_CMD_SET_SECUREFOUNDER */
	_("    SECUREFOUNDER Stricter control of channel founder status"),
	/* CHAN_HELP_CMD_SET_SIGNKICK */
	_("    SIGNKICK      Sign kicks that are done with KICK command"),
	/* CHAN_HELP_CMD_SET_TOPICLOCK */
	_("    TOPICLOCK     Topic can only be changed with TOPIC"),
	/* CHAN_HELP_CMD_SET_XOP */
	_("    XOP           Toggle the user privilege system"),
	/* CHAN_HELP_CMD_SET_PERSIST */
	_("    PERSIST       Set the channel as permanent"),
	/* CHAN_HELP_CMD_SET_NOEXPIRE */
	_("    NOEXPIRE      Prevent the channel from expiring"),
	/* CHAN_HELP_SET_TAIL */
	_("Type %R%S HELP SET option for more information on a\n"
	"particular option."),
	/* CHAN_HELP_SASET_TAIL */
	_("Type %R%S HELP SASET option for more information on a\n"
	"particular option."),
	/* CHAN_HELP_SET_FOUNDER */
	_("Syntax: %s channel FOUNDER nick\n"
	" \n"
	"Changes the founder of a channel.  The new nickname must\n"
	"be a registered one."),
	/* CHAN_HELP_SET_SUCCESSOR */
	_("Syntax: %s channel SUCCESSOR nick\n"
	" \n"
	"Changes the successor of a channel.  If the founder's\n"
	"nickname expires or is dropped while the channel is still\n"
	"registered, the successor will become the new founder of the\n"
	"channel.  However, if the successor already has too many\n"
	"channels registered (%d), the channel will be dropped\n"
	"instead, just as if no successor had been set.  The new\n"
	"nickname must be a registered one."),
	/* CHAN_HELP_SET_DESC */
	_("Syntax: %s channel DESC description\n"
	" \n"
	"Sets the description for the channel, which shows up with\n"
	"the LIST and INFO commands."),
	/* CHAN_HELP_SET_BANTYPE */
	_("Syntax: %s channel BANTYPE bantype\n"
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
	_("Syntax: %s channel KEEPTOPIC {ON | OFF}\n"
	" \n"
	"Enables or disables the topic retention option for a	\n"
	"channel. When topic retention is set, the topic for the\n"
	"channel will be remembered by %S even after the\n"
	"last user leaves the channel, and will be restored the\n"
	"next time the channel is created."),
	/* CHAN_HELP_SET_TOPICLOCK */
	_("Syntax: %s channel TOPICLOCK {ON | OFF}\n"
	" \n"
	"Enables or disables the topic lock option for a channel.\n"
	"When topic lock is set, %S will not allow the\n"
	"channel topic to be changed except via the TOPIC\n"
	"command."),
	/* CHAN_HELP_SET_PEACE */
	_("Syntax: %s channel PEACE {ON | OFF}\n"
	" \n"
	"Enables or disables the peace option for a channel.\n"
	"When peace is set, a user won't be able to kick,\n"
	"ban or remove a channel status of a user that has\n"
	"a level superior or equal to his via %S commands."),
	/* CHAN_HELP_SET_PRIVATE */
	_("Syntax: %s channel PRIVATE {ON | OFF}\n"
	" \n"
	"Enables or disables the private option for a channel.\n"
	"When private is set, a %R%S LIST will not\n"
	"include the channel in any lists."),
	/* CHAN_HELP_SET_RESTRICTED */
	_("Syntax: %s channel RESTRICTED {ON | OFF}\n"
	" \n"
	"Enables or disables the restricted access option for a\n"
	"channel.  When restricted access is set, users not on the access list will\n"
	"instead be kicked and banned from the channel."),
	/* CHAN_HELP_SET_SECURE */
	_("Syntax: %s channel SECURE {ON | OFF}\n"
	" \n"
	"Enables or disables %S's security features for a\n"
	"channel. When SECURE is set, only users who have\n"
	"registered their nicknames with %s and IDENTIFY'd\n"
	"with their password will be given access to the channel\n"
	"as controlled by the access list."),
	/* CHAN_HELP_SET_SECUREOPS */
	_("Syntax: %s channel SECUREOPS {ON | OFF}\n"
	" \n"
	"Enables or disables the secure ops option for a channel.\n"
	"When secure ops is set, users who are not on the userlist\n"
	"will not be allowed chanop status."),
	/* CHAN_HELP_SET_SECUREFOUNDER */
	_("Syntax: %s channel SECUREFOUNDER {ON | OFF}\n"
	" \n"
	"Enables or disables the secure founder option for a channel.\n"
	"When secure founder is set, only the real founder will be\n"
	"able to drop the channel, change its password, its founder and its\n"
	"successor, and not those who have founder level access through\n"
	"the access/qop command."),
	/* CHAN_HELP_SET_SIGNKICK */
	_("Syntax: %s channel SIGNKICK {ON | LEVEL | OFF}\n"
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
	_("Syntax: %s channel XOP {ON | OFF}\n"
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
	_("Syntax: %s channel PERSIST {ON | OFF}\n"
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
	_("Syntax: %s channel OPNOTICE {ON | OFF}\n"
	" \n"
	"Enables or disables the op-notice option for a channel.\n"
	"When op-notice is set, %S will send a notice to the\n"
	"channel whenever the OP or DEOP commands are used for a user\n"
	"in the channel."),
	/* CHAN_HELP_QOP */
	_("Syntax: QOP channel ADD mask\n"
	"        QOP channel DEL {mask | entry-num | list}\n"
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
	_("Syntax: AOP channel ADD mask\n"
	"        AOP channel DEL {mask | entry-num | list}\n"
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
	_("Syntax: HOP channel ADD mask\n"
	"        HOP channel DEL {mask | entry-num | list}\n"
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
	_("Syntax: SOP channel ADD mask\n"
	"        SOP channel DEL {mask | entry-num | list}\n"
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
	_("Syntax: VOP channel ADD mask\n"
	"        VOP channel DEL {mask | entry-num | list}\n"
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
	_("Syntax: ACCESS channel ADD mask level\n"
	"        ACCESS channel DEL {mask | entry-num | list}\n"
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
	"The ACCESS ADD command adds the given mask to the\n"
	"access list with the given user level; if the mask is\n"
	"already present on the list, its access level is changed to\n"
	"the level specified in the command.  The level specified\n"
	"must be less than that of the user giving the command, and\n"
	"if the mask is already on the access list, the current\n"
	"access level of that nick must be less than the access level\n"
	"of the user giving the command. When a user joins the channel\n"
	"the access they receive is from the highest level entry in the\n"
	"access list.\n"
	" \n"
	"The ACCESS DEL command removes the given nick from the\n"
	"access list.  If a list of entry numbers is given, those\n"
	"entries are deleted.  (See the example for LIST below.)\n"
	"You may remove yourself from an access list, even if you\n"
	"do not have access to modify that list otherwise.\n"
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
	_("User access levels\n"
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
	_("Syntax: AKICK channel ADD {nick | mask} [reason]\n"
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
	_("Syntax: LEVELS channel SET type level\n"
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
	_("The following feature/function names are understood.  Note\n"
	"that the leves for NOJOIN is the maximum level,\n"
	"while all others are minimum levels."),
	/* CHAN_HELP_LEVELS_DESC_FORMAT */
	_("    %-*s  %s"),
	/* CHAN_HELP_INFO */
	_("Syntax: INFO channel\n"
	" \n"
	"Lists information about the named registered channel,\n"
	"including its founder, time of registration, last time\n"
	"used, description, and mode lock, if any. If ALL is \n"
	"specified, the entry message and successor will also \n"
	"be displayed."),
	/* CHAN_HELP_LIST */
	_("Syntax: LIST pattern\n"
	" \n"
	"Lists all registered channels matching the given pattern.\n"
	"(Channels with the PRIVATE option set are not listed.)\n"
	"Note that a preceding '#' specifies a range, channel names\n"
	"are to be written without '#'."),
	/* CHAN_HELP_OP */
	_("Syntax: OP [#channel] [nick]\n"
	" \n"
	"Ops a selected nick on a channel. If nick is not given,\n"
	"it will op you. If channel is not given, it will op you\n"
	"on every channel.\n"
	" \n"
	"By default, limited to AOPs or those with level 5 access \n"
	"and above on the channel."),
	/* CHAN_HELP_DEOP */
	_("Syntax: DEOP [#channel] [nick]\n"
	" \n"
	"Deops a selected nick on a channel. If nick is not given,\n"
	"it will deop you. If channel is not given, it will deop\n"
	"you on every channel.\n"
	" \n"
	"By default, limited to AOPs or those with level 5 access \n"
	"and above on the channel."),
	/* CHAN_HELP_VOICE */
	_("Syntax: VOICE [#channel] [nick]\n"
	" \n"
	"Voices a selected nick on a channel. If nick is not given,\n"
	"it will voice you. If channel is not given, it will voice you\n"
	"on every channel.\n"
	" \n"
	"By default, limited to AOPs or those with level 5 access \n"
	"and above on the channel, or to VOPs or those with level 3 \n"
	"and above for self voicing."),
	/* CHAN_HELP_DEVOICE */
	_("Syntax: DEVOICE [#channel] [nick]\n"
	" \n"
	"Devoices a selected nick on a channel. If nick is not given,\n"
	"it will devoice you. If channel is not given, it will devoice\n"
	"you on every channel.\n"
	" \n"
	"By default, limited to AOPs or those with level 5 access \n"
	"and above on the channel, or to VOPs or those with level 3 \n"
	"and above for self devoicing."),
	/* CHAN_HELP_HALFOP */
	_("Syntax: HALFOP [#channel] [nick]\n"
	" \n"
	"Halfops a selected nick on a channel. If nick is not given,\n"
	"it will halfop you. If channel is not given, it will halfop\n"
	"you on every channel.\n"
	" \n"
	"By default, limited to AOPs and those with level 5 access \n"
	"and above on the channel, or to HOPs or those with level 4 \n"
	"and above for self halfopping."),
	/* CHAN_HELP_DEHALFOP */
	_("Syntax: DEHALFOP [#channel] [nick]\n"
	" \n"
	"Dehalfops a selected nick on a channel. If nick is not given,\n"
	"it will dehalfop you. If channel is not given, it will dehalfop\n"
	"you on every channel.\n"
	" \n"
	"By default, limited to AOPs and those with level 5 access \n"
	"and above on the channel, or to HOPs or those with level 4 \n"
	"and above for self dehalfopping."),
	/* CHAN_HELP_PROTECT */
	_("Syntax: PROTECT [#channel] [nick]\n"
	" \n"
	"Protects a selected nick on a channel. If nick is not given,\n"
	"it will protect you. If channel is not given, it will protect\n"
	"you on every channel.\n"
	" \n"
	"By default, limited to the founder, or to SOPs or those with \n"
	"level 10 and above on the channel for self protecting."),
	/* CHAN_HELP_DEPROTECT */
	_("Syntax: DEPROTECT [#channel] [nick]\n"
	" \n"
	"Deprotects a selected nick on a channel. If nick is not given,\n"
	"it will deprotect you. If channel is not given, it will deprotect\n"
	"you on every channel.\n"
	" \n"
	"By default, limited to the founder, or to SOPs or those with \n"
	"level 10 and above on the channel for self deprotecting."),
	/* CHAN_HELP_OWNER */
	_("Syntax: OWNER [#channel] [\037nick\037]\n"
	" \n"
	"Gives the selected nick owner status on channel. If nick is not\n"
	"given, it will give you owner. If channel is not given, it will\n"
	"give you owner on every channel.\n"
	" \n"
	"Limited to those with founder access on the channel."),
	/* CHAN_HELP_DEOWNER */
	_("Syntax: DEOWNER [#channel] [\037nick\037]\n"
	" \n"
	"Removes owner status from the selected nick on channel. If nick\n"
	"is not given, it will deowner you. If channel is not given, it will\n"
	"deowner you on every channel.\n"
	" \n"
	"Limited to those with founder access on the channel."),
	/* CHAN_HELP_INVITE */
	_("Syntax: INVITE channel\n"
	" \n"
	"Tells %S to invite you into the given channel.  \n"
	" \n"
	"By default, limited to AOPs or those with level 5 and above\n"
	"on the channel."),
	/* CHAN_HELP_UNBAN */
	_("Syntax: UNBAN channel [nick]\n"
	" \n"
	"Tells %S to remove all bans preventing you or the given\n"
	"user from entering the given channel.  \n"
	" \n"
	"By default, limited to AOPs or those with level 5 and above\n"
	"on the channel."),
	/* CHAN_HELP_KICK */
	_("Syntax: KICK #channel nick [reason]\n"
	" \n"
	"Kicks a selected nick on a channel.\n"
	" \n"
	"By default, limited to AOPs or those with level 5 access \n"
	"and above on the channel."),
	/* CHAN_HELP_BAN */
	_("Syntax: BAN #channel nick [reason]\n"
	" \n"
	"Bans a selected nick on a channel.\n"
	" \n"
	"By default, limited to AOPs or those with level 5 access \n"
	"and above on the channel."),
	/* CHAN_HELP_TOPIC */
	_("Syntax: TOPIC channel [topic]\n"
	" \n"
	"Causes %S to set the channel topic to the one\n"
	"specified. If topic is not given, then an empty topic\n"
	"is set. This command is most useful in conjunction\n"
	"with SET TOPICLOCK. See %R%S HELP SET TOPICLOCK\n"
	"for more information.\n"
	" \n"
	"By default, limited to those with founder access on the\n"
	"channel."),
	/* CHAN_HELP_CLEARUSERS */
	_("Syntax: CLEARUSERS channel\n"
	" \n"
	"Tells %S to clear (kick) all users certain settings on a channel."
	" \n"
	"By default, limited to those with founder access on the\n"
	"channel."),
	/* CHAN_HELP_GETKEY */
	_("Syntax: GETKEY channel\n"
	" \n"
	"Returns the key of the given channel."),
	/* CHAN_HELP_CLONE */
	_("Syntax: \002CLONE \037channel\037 \037target\037 [all | access | akick | badwords]\002\n"
	" \n"
	"Copies all settings, access, akicks, etc from channel to the\n"
	"target channel. If access, akick, or badwords is specified then only\n"
	"the respective settings are transferred. You must have founder level\n"
	"access to \037channel\037 and \037target\037."),
	/* CHAN_HELP_MODE */
	_("Syntax: \002MODE \037channel\037 LOCK {ADD|DEL|LIST} [\037what\037]\002\n"
	"        \002MODE \037channel\037 SET \037modes\037\002\n"
	" \n"
	"Mainly controls mode locks and mode access (which is different from channel access)\n"
	"on a channel.\n"
	" \n"
	"The \002MODE LOCK\002 command allows you to add, delete, and view mode locks on a channel.\n"
	"If a mode is locked on or off, services will not allow that mode to be changed.\n"
	"Example:\n"
	"     \002MODE #channel LOCK ADD +bmnt *!*@*aol*\002\n"
	" \n"
	"The \002MODE SET\002 command allows you to set modes through services. Wildcards * and ? may\n"
	"be given as parameters for list and status modes.\n"
	"Example:\n"
	"     \002MODE #channel SET +v *\002\n"
	"       Sets voice status to all users in the channel.\n"
	" \n"
	"     \002MODE #channel SET -b ~c:*\n"
	"       Clears all extended bans that start with ~c:"),
	/* CHAN_HELP_ENTRYMSG */
	_("Syntax: \002ENTRYMSG \037channel\037 {ADD|DEL|LIST|CLEAR} [\037message\037|\037num\037]\002\n"
	" \n"
	"Controls what messages will be sent to users when they join the channel."),
	/* CHAN_SERVADMIN_HELP */
	_(" \n"
	"Services Operators can also drop any channel without needing\n"
	"to identify via password, and may view the access, AKICK,\n"
	"and level setting lists for any channel."),
	/* CHAN_SERVADMIN_HELP_DROP */
	_("Syntax: DROP channel\n"
	" \n"
	"Unregisters the named channel.  Only Services Operators\n"
	"can drop a channel for which they have not identified."),
	/* CHAN_SERVADMIN_HELP_SET_NOEXPIRE */
	_("Syntax: SET channel NOEXPIRE {ON | OFF}\n"
	" \n"
	"Sets whether the given channel will expire.  Setting this\n"
	"to ON prevents the channel from expiring."),
	/* CHAN_SERVADMIN_HELP_FORBID */
	_("Syntax: FORBID channel [reason]\n"
	" \n"
	"Disallows anyone from registering or using the given\n"
	"channel.  May be cancelled by dropping the channel.\n"
	" \n"
	"Reason may be required on certain networks."),
	/* CHAN_SERVADMIN_HELP_SUSPEND */
	_("Syntax: SUSPEND channel [reason]\n"
	" \n"
	"Disallows anyone from registering or using the given\n"
	"channel.  May be cancelled by using the UNSUSPEND\n"
	"command to preserve all previous channel data/settings.\n"
	" \n"
	"Reason may be required on certain networks."),
	/* CHAN_SERVADMIN_HELP_UNSUSPEND */
	_("Syntax: UNSUSPEND channel\n"
	" \n"
	"Releases a suspended channel. All data and settings\n"
	"are preserved from before the suspension."),
	/* CHAN_SERVADMIN_HELP_STATUS */
	_("Syntax: STATUS channel nickname\n"
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
	_("    SEND   Send a memo to a nick or channel"),
	/* MEMO_HELP_CMD_CANCEL */
	_("    CANCEL Cancel last memo you sent"),
	/* MEMO_HELP_CMD_LIST */
	_("    LIST   List your memos"),
	/* MEMO_HELP_CMD_READ */
	_("    READ   Read a memo or memos"),
	/* MEMO_HELP_CMD_DEL */
	_("    DEL    Delete a memo or memos"),
	/* MEMO_HELP_CMD_SET */
	_("    SET    Set options related to memos"),
	/* MEMO_HELP_CMD_INFO */
	_("    INFO   Displays information about your memos"),
	/* MEMO_HELP_CMD_RSEND */
	_("    RSEND  Sends a memo and requests a read receipt"),
	/* MEMO_HELP_CMD_CHECK */
	_("    CHECK  Checks if last memo to a nick was read"),
	/* MEMO_HELP_CMD_SENDALL */
	_("    SENDALL  Send a memo to all registered users"),
	/* MEMO_HELP_CMD_STAFF */
	_("    STAFF  Send a memo to all opers/admins"),
	/* MEMO_HELP_CMD_IGNORE */
	_("    IGNORE Manage your memo ignore list"),
	/* MEMO_HELP_HEADER */
	_("%S is a utility allowing IRC users to send short\n"
	"messages to other IRC users, whether they are online at\n"
	"the time or not, or to channels(*).  Both the sender's\n"
	"nickname and the target nickname or channel must be\n"
	"registered in order to send a memo.\n"
	"%S's commands include:"),
	/* MEMO_HELP_FOOTER */
	_("Type %R%S HELP command for help on any of the\n"
	"above commands.\n"
	"(*) By default, any user with at least level 10 access on a\n"
	"    channel can read that channel's memos.  This can be\n"
	"    changed with the %s LEVELS command."),
	/* MEMO_HELP_SEND */
	_("Syntax: SEND {nick | channel} memo-text\n"
	" \n"
	"Sends the named nick or channel a memo containing\n"
	"memo-text.  When sending to a nickname, the recipient will\n"
	"receive a notice that he/she has a new memo.  The target\n"
	"nickname/channel must be registered."),
	/* MEMO_HELP_CANCEL */
	_("Syntax: CANCEL {nick | channel}\n"
	" \n"
	"Cancels the last memo you sent to the given nick or channel,\n"
	"provided it has not been read at the time you use the command."),
	/* MEMO_HELP_LIST */
	_("Syntax: LIST [channel] [list | NEW]\n"
	" \n"
	"Lists any memos you currently have.  With NEW, lists only\n"
	"new (unread) memos.  Unread memos are marked with a \"*\"\n"
	"to the left of the memo number.  You can also specify a list\n"
	"of numbers, as in the example below:\n"
	"   LIST 2-5,7-9\n"
	"      Lists memos numbered 2 through 5 and 7 through 9."),
	/* MEMO_HELP_READ */
	_("Syntax: READ [channel] {num | list | LAST | NEW}\n"
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
	_("Syntax: DEL [channel] {num | list | LAST | ALL}\n"
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
	_("Syntax: SET option parameters\n"
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
	_("Syntax: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}\n"
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
	_("Syntax: SET LIMIT [channel] limit\n"
	" \n"
	"Sets the maximum number of memos you (or the given channel)\n"
	"are allowed to have. If you set this to 0, no one will be\n"
	"able to send any memos to you.  However, you cannot set\n"
	"this any higher than %d."),
	/* MEMO_HELP_INFO */
	_("Syntax: INFO [channel]\n"
	" \n"
	"Displays information on the number of memos you have, how\n"
	"many of them are unread, and how many total memos you can\n"
	"receive.  With a parameter, displays the same information\n"
	"for the given channel."),
	/* MEMO_SERVADMIN_HELP_SET_LIMIT */
	_("Syntax: SET LIMIT [user | channel] {limit | NONE} [HARD]\n"
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
	_("Syntax: INFO [nick | channel]\n"
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
	_("Syntax: STAFF memo-text\n"
	"Sends all services staff a memo containing memo-text."),
	/* MEMO_HELP_SENDALL */
	_("Syntax: SENDALL memo-text\n"
	"Sends all registered users a memo containing memo-text."),
	/* MEMO_HELP_RSEND */
	_("Syntax: RSEND {nick | channel} memo-text\n"
	" \n"
	"Sends the named nick or channel a memo containing\n"
	"memo-text.  When sending to a nickname, the recipient will\n"
	"receive a notice that he/she has a new memo.  The target\n"
	"nickname/channel must be registered.\n"
	"Once the memo is read by its recipient, an automatic notification\n"
	"memo will be sent to the sender informing him/her that the memo\n"
	"has been read."),
	/* MEMO_HELP_CHECK */
	_("Syntax: CHECK nick\n"
	" \n"
	"Checks whether the _last_ memo you sent to nick has been read\n"
	"or not. Note that this does only work with nicks, not with chans."),
	/* MEMO_HELP_IGNORE */
	_("Syntax: \002IGNORE [\037channek\037] {\002ADD|DEL|LIST\002} [\037entry\037]\n"
	" \n"
	"Allows you to ignore users by nick or host from memoing you. If someone on your\n"
	"memo ignore list tries to memo you, they will not be told that you have them ignored."),
	/* OPER_HELP_CMD_GLOBAL */
	_("    GLOBAL      Send a message to all users"),
	/* OPER_HELP_CMD_STATS */
	_("    STATS       Show status of Services and network"),
	/* OPER_HELP_CMD_STAFF */
	_("    STAFF       Display Services staff and online status"),
	/* OPER_HELP_CMD_MODE */
	_("    MODE        Change a channel's modes"),
	/* OPER_HELP_CMD_KICK */
	_("    KICK        Kick a user from a channel"),
	/* OPER_HELP_CMD_AKILL */
	_("    AKILL       Manipulate the AKILL list"),
	/* OPER_HELP_CMD_SNLINE */
	_("    SNLINE      Manipulate the SNLINE list"),
	/* OPER_HELP_CMD_SQLINE */
	_("    SQLINE      Manipulate the SQLINE list"),
	/* OPER_HELP_CMD_SZLINE */
	_("    SZLINE      Manipulate the SZLINE list"),
	/* OPER_HELP_CMD_CHANLIST */
	_("    CHANLIST    Lists all channel records"),
	/* OPER_HELP_CMD_USERLIST */
	_("    USERLIST    Lists all user records"),
	/* OPER_HELP_CMD_LOGONNEWS */
	_("    LOGONNEWS   Define messages to be shown to users at logon"),
	/* OPER_HELP_CMD_RANDOMNEWS */
	_("    RANDOMNEWS  Define messages to be randomly shown to users \n"
	"                at logon"),
	/* OPER_HELP_CMD_OPERNEWS */
	_("    OPERNEWS    Define messages to be shown to users who oper"),
	/* OPER_HELP_CMD_SESSION */
	_("    SESSION     View the list of host sessions"),
	/* OPER_HELP_CMD_EXCEPTION */
	_("    EXCEPTION   Modify the session-limit exception list"),
	/* OPER_HELP_CMD_NOOP */
	_("    NOOP        Temporarily remove all O:lines of a server \n"
	"                remotely"),
	/* OPER_HELP_CMD_JUPE */
	_("    JUPE        \"Jupiter\" a server"),
	/* OPER_HELP_CMD_IGNORE */
	_("    IGNORE      Modify the Services ignore list"),
	/* OPER_HELP_CMD_SET */
	_("    SET         Set various global Services options"),
	/* OPER_HELP_CMD_RELOAD */
	_("    RELOAD      Reload services' configuration file"),
	/* OPER_HELP_CMD_UPDATE */
	_("    UPDATE      Force the Services databases to be\n"
	"                updated on disk immediately"),
	/* OPER_HELP_CMD_RESTART */
	_("    RESTART     Save databases and restart Services"),
	/* OPER_HELP_CMD_QUIT */
	_("    QUIT        Terminate the Services program with no save"),
	/* OPER_HELP_CMD_SHUTDOWN */
	_("    SHUTDOWN    Terminate the Services program with save"),
	/* OPER_HELP_CMD_DEFCON */
	_("    DEFCON      Manipulate the DefCon system"),
	/* OPER_HELP_CMD_CHANKILL */
	_("    CHANKILL    AKILL all users on a specific channel"),
	/* OPER_HELP_CMD_OLINE */
	_("    OLINE       Give Operflags to a certain user"),
	/* OPER_HELP_CMD_UMODE */
	_("    UMODE       Change a user's modes"),
	/* OPER_HELP_CMD_SVSNICK */
	_("    SVSNICK     Forcefully change a user's nickname"),
	/* OPER_HELP_CMD_MODLOAD */
	_("    MODLOAD     Load a module"),
	/* OPER_HELP_CMD_MODUNLOAD */
	_("    MODUNLOAD   Un-Load a module"),
	/* OPER_HELP_CMD_MODRELOAD */
	_("    MODRELOAD   Reload a module"),
	/* OPER_HELP_CMD_MODINFO */
	_("    MODINFO     Info about a loaded module"),
	/* OPER_HELP_CMD_MODLIST */
	_("    MODLIST     List loaded modules"),
	/* OPER_HELP */
	_("%S commands:"),
	/* OPER_HELP_LOGGED */
	_("Notice: All commands sent to %S are logged!"),
	/* OPER_HELP_GLOBAL */
	_("Syntax: GLOBAL message\n"
	" \n"
	"Allows Administrators to send messages to all users on the \n"
	"network. The message will be sent from the nick %s."),
	/* OPER_HELP_STATS */
	_("Syntax: STATS [AKILL | ALL | RESET | MEMORY | UPLINK]\n"
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
	_("Syntax: IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick] [reason]\n"
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
	_("Syntax: MODE channel modes\n"
	" \n"
	"Allows Services operators to set channel modes for any\n"
	"channel.  Parameters are the same as for the standard /MODE\n"
	"command."),
	/* OPER_HELP_UMODE */
	_("Syntax: UMODE user modes\n"
	" \n"
	"Allows Services Opers to set user modes for any user.\n"
	"Parameters are the same as for the standard /MODE\n"
	"command."),
	/* OPER_HELP_OLINE */
	_("Syntax: OLINE user flags\n"
	" \n"
	"Allows Services Opers to give Operflags to any user.\n"
	"Flags have to be prefixed with a \"+\" or a \"-\". To\n"
	"remove all flags simply type a \"-\" instead of any flags."),
	/* OPER_HELP_KICK */
	_("Syntax: KICK channel user reason\n"
	" \n"
	"Allows staff to kick a user from any channel.\n"
	"Parameters are the same as for the standard /KICK\n"
	"command.  The kick message will have the nickname of the\n"
	"IRCop sending the KICK command prepended; for example:\n"
	" \n"
	"*** SpamMan has been kicked off channel #my_channel by %S (Alcan (Flood))"),
	/* OPER_HELP_SVSNICK */
	_("Syntax: SVSNICK nick newnick\n"
	" \n"
	"Forcefully changes a user's nickname from nick to newnick."),
	/* OPER_HELP_AKILL */
	_("Syntax: AKILL ADD [+expiry] mask reason\n"
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
	"AKILL ADD adds the given nick or user@host/ip mask to the AKILL\n"
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
	_("Syntax: SNLINE ADD [+expiry] mask:reason\n"
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
	_("Syntax: SQLINE ADD [+expiry] mask reason\n"
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
	"SQLINE ADD adds the given (nick's) mask to the SQLINE\n"
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
	_("Syntax: SZLINE ADD [+expiry] mask reason\n"
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
	"SZLINE ADD adds the given (nick's) IP mask to the SZLINE\n"
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
	_("Syntax: SET option setting\n"
	"Sets various global Services options.  Option names\n"
	"currently defined are:\n"
	"    READONLY   Set read-only or read-write mode\n"
	"    DEBUG      Activate or deactivate debug mode\n"
	"    NOEXPIRE   Activate or deactivate no expire mode\n"
	"    SUPERADMIN Activate or deactivate super-admin mode\n"
	"    LIST       List the options"),
	/* OPER_HELP_SET_READONLY */
	_("Syntax: SET READONLY {ON | OFF}\n"
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
	/* OPER_HELP_SET_DEBUG */
	_("Syntax: SET DEBUG {ON | OFF | num}\n"
	" \n"
	"Sets debug mode on or off.  In debug mode, all data sent to\n"
	"and from Services as well as a number of other debugging\n"
	"messages are written to the log file.  If num is\n"
	"given, debug mode is activated, with the debugging level set\n"
	"to num.\n"
	"This option is equivalent to the command-line option\n"
	"-debug."),
	/* OPER_HELP_SET_NOEXPIRE */
	_("Syntax: SET NOEXPIRE {ON | OFF}\n"
	"Sets no expire mode on or off.  In no expire mode, nicks,\n"
	"channels, akills and exceptions won't expire until the\n"
	"option is unset.\n"
	"This option is equivalent to the command-line option\n"
	"-noexpire."),
	/* OPER_HELP_SET_SUPERADMIN */
	_("Syntax: SET SUPERADMIN {ON | OFF}\n"
	"Setting this will grant you extra privileges such as the\n"
	"ability to be \"founder\" on all channel's etc...\n"
	"This option is not persistent, and should only be used when\n"
	"needed, and set back to OFF when no longer needed."),
	/* OPER_HELP_SET_LIST */
	_("Syntax: SET LIST\n"
	"Display the various %S settings"),
	/* OPER_HELP_NOOP */
	_("Syntax: NOOP SET server\n"
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
	_("Syntax: JUPE server [reason]\n"
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
	_("Syntax: UPDATE\n"
	" \n"
	"Causes Services to update all database files as soon as you\n"
	"send the command."),
	/* OPER_HELP_RELOAD */
	_("Syntax: RELOAD\n"
	"Causes Services to reload the configuration file. Note that\n"
	"some directives still need the restart of the Services to\n"
	"take effect (such as Services' nicknames, activation of the \n"
	"session limitation, etc.)"),
	/* OPER_HELP_QUIT */
	_("Syntax: QUIT\n"
	" \n"
	"Causes Services to do an immediate shutdown; databases are\n"
	"not saved.  This command should not be used unless\n"
	"damage to the in-memory copies of the databases is feared\n"
	"and they should not be saved.  For normal shutdowns, use the\n"
	"SHUTDOWN command."),
	/* OPER_HELP_SHUTDOWN */
	_("Syntax: SHUTDOWN\n"
	" \n"
	"Causes Services to save all databases and then shut down."),
	/* OPER_HELP_RESTART */
	_("Syntax: RESTART\n"
	" \n"
	"Causes Services to save all databases and then restart\n"
	"(i.e. exit and immediately re-run the executable)."),
	/* OPER_HELP_CHANLIST */
	_("Syntax: CHANLIST [{pattern | nick} [SECRET]]\n"
	" \n"
	"Lists all channels currently in use on the IRC network, whether they\n"
	"are registered or not.\n"
	"If pattern is given, lists only channels that match it. If a nickname\n"
	"is given, lists only the channels the user using it is on. If SECRET is\n"
	"specified, lists only channels matching pattern that have the +s or\n"
	"+p mode."),
	/* OPER_HELP_USERLIST */
	_("Syntax: USERLIST [{pattern | channel} [INVISIBLE]]\n"
	" \n"
	"Lists all users currently online on the IRC network, whether their\n"
	"nick is registered or not.\n"
	" \n"
	"If pattern is given, lists only users that match it (it must be in\n"
	"the format nick!user@host). If channel is given, lists only users\n"
	"that are on the given channel. If INVISIBLE is specified, only users\n"
	"with the +i flag will be listed."),
	/* OPER_HELP_MODLOAD */
	_("Syntax: MODLOAD FileName\n"
	" \n"
	"This command loads the module named FileName from the modules\n"
	"directory."),
	/* OPER_HELP_MODUNLOAD */
	_("Syntax: MODUNLOAD FileName\n"
	" \n"
	"This command unloads the module named FileName from the modules\n"
	"directory."),
	/* OPER_HELP_MODRELOAD */
	_("Syntax: \002MODRELOAD\002 \002FileName\002\n"
	" \n"
	"This command reloads the module named FileName."),
	/* OPER_HELP_MODINFO */
	_("Syntax: MODINFO FileName\n"
	" \n"
	"This command lists information about the specified loaded module"),
	/* OPER_HELP_MODLIST */
	_("Syntax: MODLIST [Core|3rd|protocol|encryption|supported|qatested]\n"
	" \n"
	"Lists all currently loaded modules."),
	/* BOT_HELP_CMD_BOTLIST */
	_("    BOTLIST        Lists available bots"),
	/* BOT_HELP_CMD_ASSIGN */
	_("    ASSIGN         Assigns a bot to a channel"),
	/* BOT_HELP_CMD_SET */
	_("    SET            Configures bot options"),
	/* BOT_HELP_CMD_KICK */
	_("    KICK           Configures kickers"),
	/* BOT_HELP_CMD_BADWORDS */
	_("    BADWORDS       Maintains bad words list"),
	/* BOT_HELP_CMD_ACT */
	_("    ACT            Makes the bot do the equivalent of a \"/me\" command"),
	/* BOT_HELP_CMD_INFO */
	_("    INFO           Allows you to see BotServ information about a channel or a bot"),
	/* BOT_HELP_CMD_SAY */
	_("    SAY            Makes the bot say the given text on the given channel"),
	/* BOT_HELP_CMD_UNASSIGN */
	_("    UNASSIGN       Unassigns a bot from a channel"),
	/* BOT_HELP_CMD_BOT */
	_("    BOT            Maintains network bot list"),
	/* BOT_HELP */
	_("%S allows you to have a bot on your own channel.\n"
	"It has been created for users that can't host or\n"
	"configure a bot, or for use on networks that don't\n"
	"allow user bots. Available commands are listed \n"
	"below; to use them, type %R%S command.  For \n"
	"more information on a specific command, type %R\n"
	"%S HELP command."),
	/* BOT_HELP_FOOTER */
	_("Bot will join a channel whenever there is at least\n"
	"%d user(s) on it. Additionally, all %s commands\n"
	"can be used if fantasy is enabled by prefixing the command\n"
	"name with a %c."),
	/* BOT_HELP_BOTLIST */
	_("Syntax: BOTLIST\n"
	" \n"
	"Lists all available bots on this network."),
	/* BOT_HELP_ASSIGN */
	_("Syntax: ASSIGN chan nick\n"
	" \n"
	"Assigns a bot pointed out by nick to the channel chan. You\n"
	"can then configure the bot for the channel so it fits\n"
	"your needs."),
	/* BOT_HELP_UNASSIGN */
	_("Syntax: UNASSIGN chan\n"
	" \n"
	"Unassigns a bot from a channel. When you use this command,\n"
	"the bot won't join the channel anymore. However, bot\n"
	"configuration for the channel is kept, so you will always\n"
	"be able to reassign a bot later without have to reconfigure\n"
	"it entirely."),
	/* BOT_HELP_INFO */
	_("Syntax: INFO {chan | nick}\n"
	" \n"
	"Allows you to see %S information about a channel or a bot.\n"
	"If the parameter is a channel, then you'll get information\n"
	"such as enabled kickers. If the parameter is a nick,\n"
	"you'll get information about a bot, such as creation\n"
	"time or number of channels it is on."),
	/* BOT_HELP_SET */
	_("Syntax: SET (channel | bot) option parameters\n"
	" \n"
	"Configures bot options.  option can be one of:\n"
	" \n"
	"    DONTKICKOPS      To protect ops against bot kicks\n"
	"    DONTKICKVOICES   To protect voices against bot kicks\n"
	"    GREET            Enable greet messages\n"
	"    FANTASY          Enable fantaisist commands\n"
	"    SYMBIOSIS        Allow the bot to act as a real bot\n"
	"    MSG              Configure how fantasy commands should be replied to\n"
	" \n"
	"Type %R%S HELP SET option for more information\n"
	"on a specific option.\n"
	"Note: access to this command is controlled by the\n"
	"level SET."),
	/* BOT_HELP_SET_DONTKICKOPS */
	_("Syntax: SET channel DONTKICKOPS {ON|OFF}\n"
	" \n"
	"Enables or disables ops protection mode on a channel.\n"
	"When it is enabled, ops won't be kicked by the bot\n"
	"even if they don't match the NOKICK level."),
	/* BOT_HELP_SET_DONTKICKVOICES */
	_("Syntax: SET channel DONTKICKVOICES {ON|OFF}\n"
	" \n"
	"Enables or disables voices protection mode on a channel.\n"
	"When it is enabled, voices won't be kicked by the bot\n"
	"even if they don't match the NOKICK level."),
	/* BOT_HELP_SET_FANTASY */
	_("Syntax: SET channel FANTASY {ON|OFF}\n"
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
	_("Syntax: SET channel GREET {ON|OFF}\n"
	" \n"
	"Enables or disables greet mode on a channel.\n"
	"When it is enabled, the bot will display greet\n"
	"messages of users joining the channel, provided\n"
	"they have enough access to the channel."),
	/* BOT_HELP_SET_SYMBIOSIS */
	_("Syntax: SET channel SYMBIOSIS {ON|OFF}\n"
	" \n"
	"Enables or disables symbiosis mode on a channel.\n"
	"When it is enabled, the bot will do everything\n"
	"normally done by %s on channels, such as MODEs,\n"
	"KICKs, and even the entry message."),
	/* BOT_HELP_SET_MSG */
	_("Syntax: \002SET \037channel\037 MSG {\037OFF|PRIVMSG|NOTICE|NOTICEOPS\037}\002\n"
	" \n"
	"Configures how fantasy commands should be returned to the channel. Off disables\n"
	"fantasy from replying to the channel. Privmsg, notice, and noticeops message the\n"
	"channel, notice the channel, and notice the channel ops respectively.\n"
	" \n"
	"Note that replies over one line will not use this setting to prevent spam, and will\n"
	"go directly to the user who executed it."),
	/* BOT_HELP_KICK */
	_("Syntax: KICK channel option parameters\n"
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
	_("Syntax: KICK channel BOLDS {ON|OFF} [ttb]\n"
	"Sets the bolds kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who use bolds.\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_COLORS */
	_("Syntax: KICK channel COLORS {ON|OFF} [ttb]\n"
	"Sets the colors kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who use colors.\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_REVERSES */
	_("Syntax: KICK channel REVERSES {ON|OFF} [ttb]\n"
	"Sets the reverses kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who use reverses.\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_UNDERLINES */
	_("Syntax: KICK channel UNDERLINES {ON|OFF} [ttb]\n"
	"Sets the underlines kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who use underlines.\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_CAPS */
	_("Syntax: KICK channel CAPS {ON|OFF} [ttb [min [percent]]]\n"
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
	_("Syntax: KICK channel FLOOD {ON|OFF} [ttb [ln [secs]]]\n"
	"Sets the flood kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who are flooding\n"
	"the channel using at least ln lines in secs seconds\n"
	"(if not given, it defaults to 6 lines in 10 seconds).\n"
	" \n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_REPEAT */
	_("Syntax: KICK #channel REPEAT {ON|OFF} [ttb [num]]\n"
	"Sets the repeat kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who are repeating\n"
	"themselves num times (if num is not given, it\n"
	"defaults to 3).\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_KICK_BADWORDS */
	_("Syntax: KICK #channel BADWORDS {ON|OFF} [ttb]\n"
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
	_("Syntax: KICK channel ITALICS {ON|OFF} [ttb]\n"
	"Sets the italics kicker on or off. When enabled, this\n"
	"option tells the bot to kick users who use italics.\n"
	"ttb is the number of times a user can be kicked\n"
	"before it get banned. Don't give ttb to disable\n"
	"the ban system once activated."),
	/* BOT_HELP_BADWORDS */
	_("Syntax: BADWORDS channel ADD word [SINGLE | START | END]\n"
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
	_("Syntax: SAY channel text\n"
	" \n"
	"Makes the bot say the given text on the given channel."),
	/* BOT_HELP_ACT */
	_("Syntax: ACT channel text\n"
	" \n"
	"Makes the bot do the equivalent of a \"/me\" command\n"
	"on the given channel using the given text."),
	/* BOT_SERVADMIN_HELP_BOT */
	_("Syntax: BOT ADD nick user host real\n"
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
	_("These options are reserved to Services Operators:\n"
	" \n"
	"    NOBOT            Prevent a bot from being assigned to \n"
	"                        a channel\n"
	"    PRIVATE          Prevent a bot from being assigned by\n"
	"                        non IRC operators"),
	/* BOT_SERVADMIN_HELP_SET_NOBOT */
	_("Syntax: SET channel NOBOT {ON|OFF}\n"
	" \n"
	"This option makes a channel be unassignable. If a bot \n"
	"is already assigned to the channel, it is unassigned\n"
	"automatically when you enable the option."),
	/* BOT_SERVADMIN_HELP_SET_PRIVATE */
	_("Syntax: SET bot-nick PRIVATE {ON|OFF}\n"
	"This option prevents a bot from being assigned to a\n"
	"channel by users that aren't IRC operators."),
	/* HOST_ENTRY */
	_("#%d Nick:%s, vhost:%s (%s - %s)"),
	/* HOST_IDENT_ENTRY */
	_("#%d Nick:%s, vhost:%s@%s (%s - %s)"),
	/* HOST_SET */
	_("vhost for %s set to %s."),
	/* HOST_IDENT_SET */
	_("vhost for %s set to %s@%s."),
	/* HOST_SETALL */
	_("vhost for group %s set to %s."),
	/* HOST_DELALL */
	_("vhosts for group %s have been removed."),
	/* HOST_DELALL_SYNTAX */
	_("DELALL <nick>."),
	/* HOST_IDENT_SETALL */
	_("vhost for group %s set to %s@%s."),
	/* HOST_SET_ERROR */
	_("A vhost must be in the format of a valid hostmask."),
	/* HOST_SET_IDENT_ERROR */
	_("A vhost ident must be in the format of a valid ident"),
	/* HOST_SET_TOOLONG */
	_("Error! The vhost is too long, please use a host shorter than %d characters."),
	/* HOST_SET_IDENTTOOLONG */
	_("Error! The Ident is too long, please use an ident shorter than %d characters."),
	/* HOST_NOREG */
	_("User %s not found in the nickserv db."),
	/* HOST_SET_SYNTAX */
	_("SET <nick> <hostmask>."),
	/* HOST_SETALL_SYNTAX */
	_("SETALL <nick> <hostmask>."),
	/* HOST_NOT_ASSIGNED */
	_("Please contact an Operator to get a vhost assigned to this nick."),
	/* HOST_ACTIVATED */
	_("Your vhost of %s is now activated."),
	/* HOST_IDENT_ACTIVATED */
	_("Your vhost of %s@%s is now activated."),
	/* HOST_DEL */
	_("vhost for %s removed."),
	/* HOST_DEL_SYNTAX */
	_("DEL <nick>."),
	/* HOST_OFF */
	_("Your vhost was removed and the normal cloaking restored."),
	/* HOST_NO_VIDENT */
	_("Your IRCD does not support vIdent's, if this is incorrect, please report this as a possible bug"),
	/* HOST_GROUP */
	_("All vhost's in the group %s have been set to %s"),
	/* HOST_IDENT_GROUP */
	_("All vhost's in the group %s have been set to %s@%s"),
	/* HOST_LIST_FOOTER */
	_("Displayed all records (Count: %d)"),
	/* HOST_LIST_RANGE_FOOTER */
	_("Displayed records from %d to %d"),
	/* HOST_LIST_KEY_FOOTER */
	_("Displayed records matching key %s (Count: %d)"),
	/* HOST_HELP_CMD_ON */
	_("    ON          Activates your assigned vhost"),
	/* HOST_HELP_CMD_OFF */
	_("    OFF         Deactivates your assigned vhost"),
	/* HOST_HELP_CMD_GROUP */
	_("    GROUP       Syncs the vhost for all nicks in a group"),
	/* HOST_HELP_CMD_SET */
	_("    SET         Set the vhost of another user"),
	/* HOST_HELP_CMD_SETALL */
	_("    SETALL      Set the vhost for all nicks in a group"),
	/* HOST_HELP_CMD_DEL */
	_("    DEL         Delete the vhost of another user"),
	/* HOST_HELP_CMD_DELALL */
	_("    DELALL      Delete the vhost for all nicks in a group"),
	/* HOST_HELP_CMD_LIST */
	_("    LIST        Displays one or more vhost entries."),
	/* HOST_HELP_ON */
	_("Syntax: ON\n"
	"Activates the vhost currently assigned to the nick in use.\n"
	"When you use this command any user who performs a /whois\n"
	"on you will see the vhost instead of your real IP address."),
	/* HOST_HELP_SET */
	_("Syntax: SET <nick> <hostmask>.\n"
	"Sets the vhost for the given nick to that of the given\n"
	"hostmask.  If your IRCD supports vIdents, then using\n"
	"SET <nick> <ident>@<hostmask> set idents for users as \n"
	"well as vhosts."),
	/* HOST_HELP_DELALL */
	_("Syntax: DELALL <nick>.\n"
	"Deletes the vhost for all nick's in the same group as\n"
	"that of the given nick."),
	/* HOST_HELP_SETALL */
	_("Syntax: SETALL <nick> <hostmask>.\n"
	"Sets the vhost for all nicks in the same group as that\n"
	"of the given nick.  If your IRCD supports vIdents, then\n"
	"using SETALL <nick> <ident>@<hostmask> will set idents\n"
	"for users as well as vhosts.\n"
	"* NOTE, this will not update the vhost for any nick's\n"
	"added to the group after this command was used."),
	/* HOST_HELP_OFF */
	_("Syntax: OFF\n"
	"Deactivates the vhost currently assigned to the nick in use.\n"
	"When you use this command any user who performs a /whois\n"
	"on you will see your real IP address."),
	/* HOST_HELP_DEL */
	_("Syntax: DEL <nick>\n"
	"Deletes the vhost assigned to the given nick from the\n"
	"database."),
	/* HOST_HELP_LIST */
	_("Syntax: LIST [<key>|<#X-Y>]\n"
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
	_("Syntax: GROUP\n"
	" \n"
	"This command allows users to set the vhost of their\n"
	"CURRENT nick to be the vhost for all nicks in the same\n"
	"group."),
	/* OPER_SUPER_ADMIN_NOT_ENABLED */
	_("SuperAdmin setting not enabled in services.conf"),
	/* OPER_HELP_SYNC */
	_("Syntax: SQLSYNC\n"
	" \n"
	"This command syncs your databases with SQL. You should\n"
	"only have to execute this command once, when you initially\n"
	"import your databases into SQL."),
	/* OPER_HELP_CMD_SQLSYNC */
	_("    SQLSYNC     Import your databases to SQL"),
	/* OPER_SYNC_UPDATED */
	_("Updating MySQL.")
};
