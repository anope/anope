# Dutch Language File
#
# (C) 2003-2011 Anope Team
# Contact us at team@anope.org
#
# Please read COPYING and README for further details.
#
# Based on the original code of Epona by Lara.
# Based on the original code of Services by Andy Church.
#
# Translated into Dutch by GeniusDex <geniusdex@twistification.net>
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
# For example, "Español (Spanish)" or "Français (French)".

LANG_NAME
	Nederlands (Dutch)

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
	%d %b %Y %H:%M:%S %Z
# This is used as the format string for strftime() for a date alone in long
# format (English: including weekday).
STRFTIME_LONG_DATE_FORMAT
	%a %d %b %Y
# This is used as the format string for strftime() for a date alone in
# short format (English: without weekday).
STRFTIME_SHORT_DATE_FORMAT
	%d %b %Y

# These tell our strftime() what the names of months and days are.  If you
# don't use %a, %A, %b, or %B in your strftime() strings above, you can
# leave these empty.  However, if you enter names, they MUST stay in order,
# one per line, and the list MUST be complete!

# %a
STRFTIME_DAYS_SHORT
	Zo
	Ma
	Di
	Wo
	Do
	Vr
	Za
# %A
STRFTIME_DAYS_LONG
	Zondag
	Maandag
	Dinsdag
	Woensdag
	Donderdag
	Vrijdag
	Zaterdag
# %b
STRFTIME_MONTHS_SHORT
	Jan
	Feb
	Mar
	Apr
	Mei
	Jun
	Jul
	Aug
	Sep
	Okt
	Nov
	Dec
# %B
STRFTIME_MONTHS_LONG
	Januari
	Februari
	Maart
	April
	Mei
	Juni
	Juli
	Augustus
	September
	Oktober
	November
	December


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	, 


# Various error messages.
USER_RECORD_NOT_FOUND
	Interne fout - onmogelijk om aanvraag af te handelen.
UNKNOWN_COMMAND
	Onbekend commando %s.
UNKNOWN_COMMAND_HELP
	Onbekend commando %s.  "%R%s HELP" voor help.
SYNTAX_ERROR
	Gebruik: %s
MORE_INFO
	%R%s HELP %s voor meer informatie.
NO_HELP_AVAILABLE
	Geen help beschikbaar voor %s.
OBSOLETE_COMMAND
	Dit commando is verouderd; gebruik %s.

BAD_USERHOST_MASK
	Hostmask moet gegeven worden in de vorm user@host.
BAD_EXPIRY_TIME
	Ongeldige verloop tijd.
USERHOST_MASK_TOO_WIDE
	%s dekking is te groot; Gebruik aub een meer specifiek hostmask.

SERVICE_OFFLINE
	%s is momenteel offline.
READ_ONLY_MODE
	Let op: Services zijn in alleen-lezen mode; veranderingen worden niet opgeslagen!
PASSWORD_INCORRECT
	Wachtwoord incorrect.
INVALID_TARGET
	"/msg %s" is no longer supported.  Use "/msg %s@%s" or "/%s" instead.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Toegang geweigerd.
PERMISSION_DENIED
	Toestemming geweigerd.
RAW_DISABLED
	De RAW optie is uitgeschakeld. Als je deze moet gebruiken, stel dan de DisableRaw optie in de Services configuratie in.


MORE_OBSCURE_PASSWORD
	Probeer aub opnieuw met een onduidelijker wachtwoord.  Wachtwoorden moeten tenminste 5 tekens lang zijn, niet iets makkelijk raadbaars (bijv. je echte naam of je nick), en kan geen tab of spatie bevatten.
PASSWORD_TOO_LONG
	Je wachtwoord is te lang. Probeer het opnieuw met een korter wachtwoord.

NICK_NOT_REGISTERED
	Je nick is niet geregistreerd.
NICK_NOT_REGISTERED_HELP
	Je nick is niet geregistreerd.  Type %R%s HELP voor informatie over het registreren van je nick.
NICK_X_IS_SERVICES
	Nick %s is deel van de Services van dit netwerk.
NICK_X_NOT_REGISTERED
	Nick %s is niet geregistreerd.
NICK_X_IN_USE
	Nick %s is momenteel in gebruik.
NICK_X_NOT_IN_USE
	Nick %s is momenteel niet in gebruik.
NICK_X_NOT_ON_CHAN
	%s is momenteel niet op kanaal %s.
NICK_X_FORBIDDEN
	Nick %s mag niet geregistreerd of gebruikt worden.
NICK_X_FORBIDDEN_OPER
	Nick %s is verboden door %s:
	%s
NICK_X_ILLEGAL
	Nick %s is een illegale nickname en mag niet worden gebruikt.
NICK_X_TRUNCATED
	Nick %s is verkort naar %d tekens.
NICK_X_SUSPENDED
	Nick %s is tijdelijk buiten werking gesteld.
CHAN_X_NOT_REGISTERED
	Channel %s is niet geregistreerd.
CHAN_X_NOT_IN_USE
	Channel %s bestaat niet.
CHAN_X_FORBIDDEN
	Channel %s mag niet geregistreerd of gebruikt worden.
CHAN_X_FORBIDDEN_OPER
	Channel %s is verboden door %s:
	%s
CHAN_X_SUSPENDED
	      Tijdelijk buiten werking gesteld: [%s] %s
NICK_IDENTIFY_REQUIRED
	Wachtwoord verificatie benodigd voor dat commando.
	Probeer opnieuw na het typen van %R%s IDENTIFY wachtwoord.
CHAN_IDENTIFY_REQUIRED
	Wachtwoord verificatie is benodigd voor dat commando.
	Probeer opnieuw na het typen van %R%s IDENTIFY %s wachtwoord.
	
MAIL_DISABLED
	Services zijn geconfigureerd geen e-mail te versturen.
MAIL_INVALID
	E-mail voor %s is ongeldig.
MAIL_X_INVALID
	%s is geen geldig e-mail adres.
MAIL_LATER
	Onmogelijk e-mail te sturen; probeer na een tijdje opnieuw.
MAIL_DELAYED
	Wacht %d seconden en probeer opnieuw.
	
NO_REASON
	Geen reden
UNKNOWN
	<onbekend>

# Duration system
DURATION_DAY
	1 dag
DURATION_DAYS
	%d dagen
DURATION_HOUR
	1 uur
DURATION_HOURS
	%d uren
DURATION_MINUTE
	1 minuut
DURATION_MINUTES
	%d minuten
DURATION_SECOND
	1 seconde
DURATION_SECONDS
	%d secondes

# Human readable expiration
NO_EXPIRE
	verloopt niet
EXPIRES_SOON
	verloopt bij de volgende database update
EXPIRES_M
	verloopt over %d minuten
EXPIRES_1M
	verloopt over %d minuut
EXPIRES_HM
	verloopt over %d uur, %d minuten
EXPIRES_H1M
	verloopt over %d uur, %d minuut
EXPIRES_1HM
	verloopt over %d uur, %d minuten
EXPIRES_1H1M
	verloopt over %d uur, %d minuut
EXPIRES_D
	verloopt over %d dagen
EXPIRES_1D
	verloopt over %d dag

# Generic Footer message
END_OF_ANY_LIST
	Einde van de %s lijst.

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
LIST_INCORRECT_RANGE
	Ongeldig bereik opgegeven. Het correct gebruik is #van-tot.
CS_LIST_INCORRECT_RANGE
	Om te zoeken naar kanalen die met een # beginnen moet gezocht
	worden naar de kanaalnaam zonder het #-teken ervoor (anope
	in plaats van #anope).

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
HELP_LIMIT_SERV_OPER
	
	Gelimiteerd tot Services Operators.
HELP_LIMIT_SERV_ADMIN
	
	Gelimiteerd tot Services Administrators.
HELP_LIMIT_SERV_ROOT
	
	Gelimiteerd tot Services Roots.
HELP_LIMIT_IRC_OPER
	
	Gelimiteerd tot IRC Operators.
HELP_LIMIT_HOST_SETTER
	
	Gelimiteerd tot Host Setters.
HELP_LIMIT_HOST_REMOVER
	
	Gelimiteerd tot Host Removers.


###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	Deze nick is eigendom van iemand anders.  Kies een andere.
	(Als dit jouw nick is, type %R%s IDENTIFY wachtwoord.)
NICK_IS_SECURE
	Deze nickname is geregistreerd en beschermd.  Als het jouw
	nick is, type %R%s IDENTIFY wachtwoord.  Zo niet,
	kies een andere nick.
NICK_MAY_NOT_BE_USED
	Deze nickname mag niet gebruikt worden.  Kies een andere.
FORCENICKCHANGE_IN_1_MINUTE
	Als je niet binnen 1 minuut je nick verandert, verander ik je nick.
FORCENICKCHANGE_IN_20_SECONDS
	Als je niet binnen 20 seconden je nick verandert, verander ik je nick.
FORCENICKCHANGE_NOW
	Deze nickname is geregistreerd; je mag hem niet gebruiken.
FORCENICKCHANGE_CHANGING
	Je nickname wordt nu veranderd in %s

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER wachtwoord [email]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER wachtwoord email
NICK_REGISTRATION_DISABLED
	Sorry, nickname registratie is tijdelijk uitgeschakeld.
NICK_REGISTRATION_FAILED
	Sorry, registratie gefaald.
NICK_REG_PLEASE_WAIT
	Wacht %d seconden voor het opnieuw gebruiken van het REGISTER commando.
NICK_CANNOT_BE_REGISTERED
	Nickname %s mag niet geregistreerd worden.
NICK_ALREADY_REGISTERED
	Nickname %s is al geregistreerd!
NICK_REGISTERED
	Nickname %s is geregistreerd onder jouw account: %s
NICK_REGISTERED_NO_MASK
	Nickname %s is geregistreerd.
NICK_PASSWORD_IS
	Je wachtwoord is %s - onthoudt dit voor later gebruik.

# GROUP responses
NICK_REG_DELAY
	You have to be connected longer than %d seconds to register your nick.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP doel wachtwoord
NICK_GROUP_DISABLED
	Sorry, nickname groeperingen zijn tijdelijk uitgeschakeld.
NICK_GROUP_FAILED
	Sorry, groepering gefaald.
NICK_GROUP_PLEASE_WAIT
	Wacht %d seconden voor het opnieuw gebruiken van het GROUP commando.
NICK_GROUP_CHANGE_DISABLED
	Je nick is al geregistreerd; type eerst %R%s DROP.
NICK_GROUP_SAME
	Je bent al lid van de group van %s.
NICK_GROUP_TOO_MANY
	Er zijn teveel nicks in %s's groep; geef ze weer en drop er een paar.
	Type %R%s HELP GLIST en%R%s HELP DROP 
	voor meer informatie.
NICK_GROUP_JOINED
	Je zit nu in de group van %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY wachtwoord
NICK_IDENTIFY_FAILED
	Sorry, identificatie gefaald.
NICK_IDENTIFY_SUCCEEDED
	Wachtwoord geaccepteerd - je wordt nu herkend.
NICK_IDENTIFY_EMAIL_REQUIRED
	Je moet een e-mail adres opgeven voor je nick.
	Dit stelt je in staat je wachtwoord terug te halen in
	het geval dat je het vergeet.
NICK_IDENTIFY_EMAIL_HOWTO
	Type %R%S SET EMAIL e-mail om je e-mail in te stellen.
	Je privacy wordt gerespecteerd; deze e-mail wordt niet aan
	een derde partij doorgegeven.
	
# LOGOUT responses
NICK_ALREADY_IDENTIFIED
	Je bent al geidentificeerd.

# UPDATE responses
NICK_UPDATE_SUCCESS
	Status geupdate (memos, vhost, chmodes, flags).
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	Je nick is uitgelogd.
NICK_LOGOUT_X_SUCCEEDED
	Nick %s is uitgelogd.

# DROP responses
NICK_LOGOUT_SERVICESADMIN
	Kan %s niet uitloggen omdat hij een service administrator is.

# DROP responses
NICK_DROP_DISABLED
	Sorry, nickname de-registratie is tijdelijk uitgeschakeld.
NICK_DROPPED
	Je nickname is gedropped.
NICK_X_DROPPED
	Nickname %s is gedropped.

# SET responses
NICK_SET_SYNTAX
	SET optie parameters
NICK_SET_SERVADMIN_SYNTAX
	SET [nick] optie parameters
NICK_SET_DISABLED
	Sorry, nickname opties aanpassen is tijdelijk uitgeschakeld.
NICK_SET_UNKNOWN_OPTION
	Onbekende SET optie %s.
NICK_SET_OPTION_DISABLED
	Optie %s kan niet gezet worden op dit netwerk.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	De nieuwe weergave MOET een nickname van je nickname groep zijn!
NICK_SET_DISPLAY_CHANGED
	De nieuwe weergave is nu %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Sorry, kan het wachtwoord niet veranderen.
NICK_SET_PASSWORD_CHANGED
	Password veranderd.
NICK_SET_PASSWORD_CHANGED_TO
	Password veranderd in %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE nummer
NICK_SET_LANGUAGE_UNKNOWN
	Onbekend taal nummer %d.  Type %R%s HELP SET LANGUAGE voor een lijst van talen.
NICK_SET_LANGUAGE_CHANGED
	Taal veranderd in Nederlands.

# SET URL responses
NICK_SET_URL_CHANGED
	URL veranderd in %s.
NICK_SET_URL_UNSET
	URL verwijderd.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	E-mail adres veranderd in %s.
NICK_SET_EMAIL_UNSET
	E-mail adres verwijderd.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	Je kan je e-mail adres niet verwijderen op dit netwerk.
	
# SET ICQ responses
NICK_SET_ICQ_CHANGED
	ICQ nummer veranderd in %s.
NICK_SET_ICQ_UNSET
	ICQ nummer verwijderd.
NICK_SET_ICQ_INVALID
	%s is geen geldig nummer.
	
# SET GREET responses
NICK_SET_GREET_CHANGED
	Groet veranderd in %s.
NICK_SET_GREET_UNSET
	Groet verwijderd.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Protectie staat nu AAN.
NICK_SET_KILL_QUICK
	Protectie staat nu AAN, met een gereduceerde vertraging.
NICK_SET_KILL_IMMED
	Protectie staat nu AAN, zonder vertraging.
NICK_SET_KILL_IMMED_DISABLED
	De IMMED optie is niet beschikbaar op dit netwerk.
NICK_SET_KILL_OFF
	Protectie staat nu UIT.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Veiligheidsoptie staat nu AAN.
NICK_SET_SECURE_OFF
	Veiligheidsoptie staat nu UIT.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Prive optie staat nu AAN.
NICK_SET_PRIVATE_OFF
	Prive optie staat nu UIT.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Je e-mail adres zal vanaf nu verborgen worden op %s INFO schermen.
NICK_SET_HIDE_EMAIL_OFF
	Je e-mail adres zal vanaf nu getoond worden op %s INFO schermen.
NICK_SET_HIDE_MASK_ON
	Je laatst bekende hostmask zal vanaf nu verborgen worden op %s INFO schermen.
NICK_SET_HIDE_MASK_OFF
	Je laatst bekende hostmask zal vanaf nu getoond worden op %s INFO schermen.
NICK_SET_HIDE_QUIT_ON
	Je laatste quit bericht zal vanaf nu verborgen worden op %s INFO schermen.
NICK_SET_HIDE_QUIT_OFF
	Je laatste quit bericht zal vanaf nu getoond worden op %s INFO schermen.
	
# SET MSG responses
NICK_SET_HIDE_STATUS_ON
	Your services access status will now be hidden from %s INFO displays.
NICK_SET_HIDE_STATUS_OFF
	Your services access status will now be shown in %s INFO displays.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_MSG_ON
	Services zullen je nu beantwoorden met berichten.
NICK_SET_MSG_OFF
	Services zullen je nu beantwoorden met notices.

# SET AUTOOP responses
NICK_SET_AUTOOP_SYNTAX
	SET AUTOOP {ON | OFF}
NICK_SET_AUTOOP_ON
	Services will now autoop you in channels.
NICK_SET_AUTOOP_OFF
	Services will no longer autoop you in channels.


# SET NOEXPIRE responses
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

# SASET LANGUAGE responses
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
	SET nickname HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
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
	SSAET nickname MSG {ON | OFF}
NICK_SASET_MSG_ON
	Services will now reply to %s with messages.
NICK_SASET_MSG_OFF
	Services will now reply to %s with notices.

# SET NOEXPIRE responses
NICK_SASET_NOEXPIRE_SYNTAX
	SASET nickname NOEXPIRE {ON | OFF}
NICK_SASET_NOEXPIRE_ON
	Nick %s will not expire.
NICK_SASET_NOEXPIRE_OFF
	Nick %s will expire.

# ACCESS responses
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
	Hostmask %s bestaat al op je toegangslijst.
NICK_ACCESS_REACHED_LIMIT
	Sorry, er zijn maar %d plaatsen op de toegangslijst van een nickname.
NICK_ACCESS_ADDED
	%s toegevoegd aan je toegangslijst.
NICK_ACCESS_NOT_FOUND
	%s niet gevonden op je toegangslijst.
NICK_ACCESS_DELETED
	%s verwijderd van je toegangslijst.
NICK_ACCESS_LIST
	Toegangslijst:
NICK_ACCESS_LIST_X
	Toegangslijst voor %s:
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
	%s is een services beheerder.
NICK_INFO_SERVICES_ADMIN
	%s is een services eigenaar.
NICK_INFO_SERVICES_ROOT
	%s is a services root administrator.
NICK_INFO_ADDRESS
	Laatst bekende adres: %s
NICK_INFO_ADDRESS_ONLINE
	       Is online van: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s is momenteel online.
NICK_INFO_TIME_REGGED
	  Tijd geregistreerd: %s
NICK_INFO_LAST_SEEN
	  Tijd laatst gezien: %s
NICK_INFO_LAST_QUIT
	Laatste quit-bericht: %s
NICK_INFO_URL
	                 URL: %s
NICK_INFO_EMAIL
	        E-mail adres: %s
NICK_INFO_VHOST
	               vhost: %s
NICK_INFO_VHOST2
	            	vhost: %s@%s
NICK_INFO_ICQ
	               ICQ #: %d
NICK_INFO_GREET
	       Groet-bericht: %s
NICK_INFO_OPTIONS
	              Opties: %s
NICK_INFO_EXPIRE
		Expires in: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Bescherming
NICK_INFO_OPT_SECURE
	Veiligheid
NICK_INFO_OPT_PRIVATE
	Prive
NICK_INFO_OPT_MSG
	Bericht mode
NICK_INFO_OPT_AUTOOP
	Auto-op
NICK_INFO_OPT_NONE
	Geen
NICK_INFO_NO_EXPIRE
	Deze nick zal niet verlopen.
NICK_INFO_FOR_MORE
	Voor meer informatie, type %R%s INFO %s ALL.

# LIST responses
NICK_INFO_SUSPENDED
	Deze nick is tijdelijk buiten werking gesteld, reden: %s
NICK_INFO_SUSPENDED_NO_REASON
	Deze nick is tijdelijk buiten werking gesteld

# LIST responses
NICK_LIST_SYNTAX
	LIST trefbeeld
NICK_LIST_SERVADMIN_SYNTAX
	LIST trefbeeld [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Lijst van nicks overeenkomend met %s:
NICK_LIST_RESULTS
	Einde van lijst - %d/%d resultaten weergegeven.

# ALIST responses
NICK_ALIST_SYNTAX
	ALIST nick
NICK_ALIST_HEADER
	Kanalen waarop je toegang hebt:
	  Nr   Kanaal               Niveau   Beschrijving
NICK_ALIST_HEADER_X
	Kanalen waarop %s toegang heeft:
	  Nr   Kanaal               Niveau   Beschrijving
NICK_ALIST_XOP_FORMAT
	  %3d %c%-20s %-8s %s
NICK_ALIST_ACCESS_FORMAT
	  %3d %c%-20s %-8d %s
NICK_ALIST_FOOTER
	Einde van de lijst - %d/%d kanalen weergegeven.

# GLIST responses
NICK_GLIST_HEADER
	Lijst van nicks in je groep:
NICK_GLIST_HEADER_X
	Lijst van nicks in de groep van %s:
NICK_GLIST_FOOTER
	%d nicks in de groep.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (expires in %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nickname [wachtwoord]
NICK_NO_RECOVER_SELF
	Je kan jezelf niet herstellen!
NICK_RECOVERED
	Gebruiker die je nick claimde is gekilld.
	%R%s RELEASE %s om deze voor de %s timeout terug te krijgen.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nickname [wachtwoord]
NICK_RELEASE_NOT_HELD
	Nick %s wordt niet vastgehouden.
NICK_RELEASED
	Nick is vrijgegeven.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nickname [wachtwoord]
NICK_NO_GHOST_SELF
	Je kan jezelf niet ghosten!
NICK_GHOST_KILLED
	Ghost met jouw nick is gekilld.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nickname
NICK_GETPASS_UNAVAILABLE
	GETPASS commando is onbeschikbaar omdat encryptie aanstaat.
NICK_GETPASS_PASSWORD_IS
	Wachtwoord voor %s is %s.

# SENDPASS responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host geen Wildcards!!
NICK_GETEMAIL_EMAILS_ARE
	Emails %s komen overeen met %s.
NICK_GETEMAIL_NOT_USED
	Geen emails voor %s.
	
# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS nickname
NICK_SENDPASS_UNAVAILABLE
	SENDPASS commando is onbeschikbaar omdat encryptie aanstaat.
NICK_SENDPASS_SUBJECT
	Nick wachtwoord (%s)
NICK_SENDPASS_HEAD
	Hoi,
NICK_SENDPASS_LINE_1
	Je hebt het wachtwoord van nick %s via e-mail aangevraagd.
NICK_SENDPASS_LINE_2
	Het wachtwoord is %s Wegens veiligheidsredenen kun je dit het beste zo snel mogelijk veranderen.
NICK_SENDPASS_LINE_3
	Als je niet weet waarom deze email gestuurd is, negeer deze dan.
NICK_SENDPASS_LINE_4
	ANTWOORD NIET OP DEZE E-MAIL!
NICK_SENDPASS_LINE_5
	%s beheerders.
NICK_SENDPASS_OK
	Wachtwoord %s is verstuurd.

# FORBID responses
NICK_SUSPEND_SYNTAX
	SUSPEND nick reden
NICK_SUSPEND_SUCCEEDED
	Nick %s is tijdelijk buiten werking gesteld.
NICK_SUSPEND_FAILED
	Kan nick %s niet tijdelijk buiten werking stellen!

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND nick
NICK_UNSUSPEND_SUCCEEDED
	Nick %s is nu vrijgegeven.
NICK_UNSUSPEND_FAILED
	Kan nick %s niet vrijgeven!

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID nick [reden]
NICK_FORBID_SYNTAX_REASON
	FORBID nick reden
NICK_FORBID_SUCCEEDED
	Nick %s is nu verboden.
NICK_FORBID_FAILED
	Verbieden van nick %s is niet gelukt!

# Nick Registraion responses
NICK_REQUESTED
	Deze nick is al aangevraagd. Controleer je e-mail adres voor een verificatiecode.
NICK_REG_RESENT
	Je toegangscode is opnieuw gestuurd naar %s.
NICK_REG_UNABLE
	Nick is NIET geregistreerd, probeer later opnieuw.
NICK_IS_PREREG
	Deze nick wacht nog op een e-mail verificatie code coor de registratie is voltooid.
NICK_ENTER_REG_CODE
	Een verificatiecode is verstuurd naar %s. Type %R%s confirm <verificatiecode> om de registratie te voltooien.
NICK_CONFIRM_NOT_FOUND
	Registratie stap 1 kan verlopen zijn. Gebruik eerst "%R%s register <wachtwoord> <email>".
NICK_CONFIRM_INVALID
	Ongeldige verificatiecode ingevuld. Controleer je e-mail nog eens en probeer opnieuw.
NICK_REG_MAIL_SUBJECT
	Nickname Registratie (%s)
NICK_REG_MAIL_HEAD
	Hoi,
NICK_REG_MAIL_LINE_1
	Je hebt een aanvraag ingediend om de nickname %s te registreren.
NICK_REG_MAIL_LINE_2
	Type " %R%s confirm %s " om de registratie te voltooien.
NICK_REG_MAIL_LINE_3
	Als je niet weet waarom deze e-mail naar je gestuurd is, negeer deze dan.
NICK_REG_MAIL_LINE_4
	REAGEER NIET OP DEZE MAIL!
NICK_REG_MAIL_LINE_5
	%s administrators.
NICK_GETPASS_PASSCODE_IS
	Verificatiecode voor %s is %s.
NICK_FORCE_REG
	Nickname %s geverificeerd.

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Automatisch mode +o
CHAN_LEVEL_AUTOVOICE
	Automatisch mode +v
CHAN_LEVEL_AUTOHALFOP
	Automatisch mode +h
CHAN_LEVEL_AUTOPROTECT
	Automatisch mode +a
CHAN_LEVEL_AUTODEOP
	Kanaal operator status niet toegestaan
CHAN_LEVEL_NOJOIN
	Niet toegestaan kanaal te joinen indien RESTRICTED
CHAN_LEVEL_INVITE
	Toegestaan om INVITE te gebruiken
CHAN_LEVEL_AKICK
	Toegestaan om AKICK te gebruiken
CHAN_LEVEL_SET
	Toegestaan om SET te gebruiken (niet FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Toegestaan om CLEAR te gebruiken
CHAN_LEVEL_UNBAN
	Toegestaan om UNBAN te gebruiken
CHAN_LEVEL_OPDEOP
	Toegestaan om OP/DEOP te gebruiken
CHAN_LEVEL_ACCESS_LIST
	Toegestaan om de toegangslijst te bekijken
CHAN_LEVEL_ACCESS_CHANGE
	Toegestaan om de toegangslijst te wijzigen
CHAN_LEVEL_MEMO
	Toegestaan om kanaal memo's te lezen
CHAN_LEVEL_ASSIGN
	Toegestaan om een bot aan te stellen/weg te halen
CHAN_LEVEL_BADWORDS
	Toegestaan om BADWORDS te gebruiken
CHAN_LEVEL_NOKICK
	Wordt nooit gekicked door bot kickers
CHAN_LEVEL_FANTASIA
	Toegestaan om FANTASIA commando's te gebruiken
CHAN_LEVEL_SAY
	Toegestaan om SAY en ACT te gebruiken
CHAN_LEVEL_GREET
	Groetingsbericht weergegeven
CHAN_LEVEL_VOICEME
	Toegestaan om zichzelf te (de)voicen
CHAN_LEVEL_VOICE
	Toegestaan om VOICE/DEVOICE te gebruiken
CHAN_LEVEL_GETKEY
	Toegestaan om GETKEY te gebruiken
CHAN_LEVEL_OPDEOPME
	Toegestaan om zichzelf te (de)oppen
CHAN_LEVEL_HALFOPME
	Toegestaan om zichzelf te (de)halfoppen
CHAN_LEVEL_HALFOP
	Toegestaan om HALFOP/DEHALFOP te gebruiken
CHAN_LEVEL_PROTECTME
	Toegestaan om zichzelf (niet) te beschermen
CHAN_LEVEL_PROTECT
	Toegestaan om PROTECT/DEPROTECT te gebruiken
CHAN_LEVEL_KICKME
	Toegestaan om zichzelf te kicken
CHAN_LEVEL_KICK
	Toegestaan om KICK te gebruiken
CHAN_LEVEL_SIGNKICK
	Geen ondertekende kick wanneer SIGNKICK LEVEL is gebruikt
CHAN_LEVEL_BANME
	Toegestaan om zichzelf te bannen
CHAN_LEVEL_BAN
	Toegestaan om BAN te gebruiken
CHAN_LEVEL_TOPIC
	Toegestaan om TOPIC te gebruiken
CHAN_LEVEL_INFO
	Toegestaan om INFO met de ALL optie te gebruiken

# Automatic responses
CHAN_IS_REGISTERED
	Dit kanaal is geregistreerd met %s.
CHAN_NOT_ALLOWED_OP
	Je mag geen kanaal operator zijn op kanaal %s.
CHAN_MAY_NOT_BE_USED
	Dit kanaal mag niet gebruikt worden.
CHAN_NOT_ALLOWED_TO_JOIN
	Je hebt geen toestemming om op dit kanaal te zijn.

# REGISTER responses
CHAN_X_INVALID
	Channel %s is not a valid channel.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER kanaal wachtwoord beschrijving
CHAN_REGISTER_DISABLED
	Sorry, kanaal registratie is tijdelijk uitgeschakeld.
CHAN_REGISTER_NOT_LOCAL
	Lokale kanalen kunnen niet geregistreerd worden.
CHAN_MUST_REGISTER_NICK
	Je moet je nickname eerst registreren.  Type %R%s HELP voor meer informatie over het registreren van nicknames.
CHAN_MUST_IDENTIFY_NICK
	Identificeer eerst met %s, door middel van het commando:
	%R%s IDENTIFY wachtwoord
CHAN_MAY_NOT_BE_REGISTERED
	Kanaal %s mag niet geregistreerd worden.
CHAN_ALREADY_REGISTERED
	Kanaal %s is al geregistreerd!
CHAN_MUST_BE_CHANOP
	Je moet een operator zijn op het kanaal om deze te registreren.
CHAN_REACHED_CHANNEL_LIMIT
	Sorry, je hebt je limiet van %d kanaalregistraties bereikt.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Sorry, je hebt je limiet van %d kanaalregistraties overschreden.
CHAN_REGISTRATION_FAILED
	Sorry, registratie gefaald.
CHAN_REGISTERED
	Kanaal %s is geregistreerd onder jouw nickname: %s
CHAN_PASSWORD_IS
	Het wachtwoord van je kanaal is %s - onthoudt dit voor later gebruik.
CHAN_REGISTER_NONE_CHANNEL
	Je hebt geprobeerd om niet-bestaand kanaal %s te registeren.
CHAN_SYMBOL_REQUIRED
	Gebruik het # symbool wanner je probeert een kanaal te registeren.


# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY kanaal wachtwoord
CHAN_IDENTIFY_FAILED
	Sorry, identificatie gefaald.
CHAN_IDENTIFY_SUCCEEDED
	Wachtwoord goedgekeurd - je hebt nu stichter-niveau toegang to %s.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT kanaal nickname
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT kanaal [nickname]
CHAN_LOGOUT_SUCCEEDED
	Gebruiker %s is uitgelogd van kanaal %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	Alle geidentificeerde gebruikers zijn uitgelogd van kanaal %s (except the channel founder).
CHAN_LOGOUT_FOUNDER_FAILED
	You may not log out of %s (you are founder).

# DROP responses
CHAN_DROP_SYNTAX
	DROP kanaal
CHAN_DROP_DISABLED
	Sorry, kanaal de-registratie is tijdelijk uitgeschakeld.
CHAN_DROPPED
	Kanaal %s is gede-registreerd.

# SET responses
CHAN_SET_SYNTAX
	SET kanaal optie parameters
CHAN_SET_DISABLED
	Sorry, kanaal opties instellen is tijdelijk uitgeschakeld.
CHAN_SET_UNKNOWN_OPTION
	Onbekende SET optie %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s heeft te veel geregistreerde kanalen.
CHAN_FOUNDER_CHANGED
	Stichter van kanaal %s veranderd in %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Opvolger voor %s veranderd in %s.
CHAN_SUCCESSOR_UNSET
	Opvolger voor %s verwijderd.
CHAN_SUCCESSOR_IS_FOUNDER
	%s kan de opvolger niet zijn op kanaal %s omdat hij/zij founder is.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Sorry, kan wachtwoord niet zetten.
CHAN_PASSWORD_CHANGED
	%s wachtwoord veranderd.
CHAN_PASSWORD_CHANGED_TO
	%s wachtwoord veranderd in %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Beschrijving van %s veranderd in %s.

# SET URL responses
CHAN_URL_CHANGED
	URL voor %s veranderd in %s.
CHAN_URL_UNSET
	URL voor %s verwijderd.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	E-mail adres voor %s veranderd in %s.
CHAN_EMAIL_UNSET
	E-mail adres voor %s verwijderd.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Welkomstbericht voor %s veranderd.
CHAN_ENTRY_MSG_UNSET
	Welkomstbericht voor %s verwijderd.
	
# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s is geen geldig ban type.
CHAN_SET_BANTYPE_CHANGED
	Ban type voor kanaal %s is nu #%d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Onbekend mode karakter %c genegeerd.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Mode %c genegeerd omdat je deze niet kan forceren.
CHAN_SET_MLOCK_L_REQUIRED
	Je moet ook mode +l forceren als je mode +L forceert.
CHAN_SET_MLOCK_K_REQUIRED
	Je moet ook mode +i forceren als je mode +K forceert.
CHAN_MLOCK_CHANGED
	Mode forcering van kanaal %s veranderd in %s.


# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET kanaal KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Topic retention option for %s is now ON.
CHAN_SET_KEEPTOPIC_OFF
	Topic retention option for %s is now OFF.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET kanaal TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Topic lock option for %s is now ON.
CHAN_SET_TOPICLOCK_OFF
	Topic lock option for %s is now OFF.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET kanaal PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Peace option for %s is now ON.
CHAN_SET_PEACE_OFF
	Peace option for %s is now OFF.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET kanaal PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Private option for %s is now ON.
CHAN_SET_PRIVATE_OFF
	Private option for %s is now OFF.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET kanaal SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Secure ops option for %s is now ON.
CHAN_SET_SECUREOPS_OFF
	Secure ops option for %s is now OFF.

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET kanaal SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Secure founder option for %s is now ON.
CHAN_SET_SECUREFOUNDER_OFF
	Secure founder option for %s is now OFF.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET kanaal RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Restricted access option for %s is now ON.
CHAN_SET_RESTRICTED_OFF
	Restricted access option for %s is now OFF.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET kanaal SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Secure option for %s is now ON.
CHAN_SET_SECURE_OFF
	Secure option for %s is now OFF.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET kanaal SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Signed kick option for %s is now ON.
CHAN_SET_SIGNKICK_LEVEL
	Signed kick option for %s is now ON, but depends of the
	level of the user that is using the command.
CHAN_SET_SIGNKICK_OFF
	Signed kick option for %s is now OFF.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET kanaal OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice option for %s is now ON.
CHAN_SET_OPNOTICE_OFF
	Op-notice option for %s is now OFF.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET kanaal XOP {ON | OFF}
CHAN_SET_XOP_ON
	xOP lists system for %s is now ON.
CHAN_SET_XOP_OFF
	xOP lists system for %s is now OFF.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET kanaal NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Kanaal %s zal niet verlopen.
CHAN_SET_NOEXPIRE_OFF
	Kanaal %s zal verlopen.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Sorry, je kan maar %d AOPs/SOPs/VOPs in dit kanaal hebben.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	Je kan dit commando niet gebruiken. Gebruik het ACCESS commando.
	Type %R%s HELP ACCESS voor meer informatie.
CHAN_XOP_NOT_AVAILABLE
	xOP system is not available.

# AOP messages
CHAN_AOP_SYNTAX
	AOP kanaal {ADD|DEL|LIST|CLEAR} [nick | lijst]
CHAN_AOP_DISABLED
	Sorry, kanaal AOP lijst wijzigen is tijdelijk uitgeschakeld.
CHAN_AOP_NICKS_ONLY
	Kanaal AOP lijsten mogen alleen geregistreerde nicks bevatten.
CHAN_AOP_ADDED
	%s toegevoegd aan %s AOP lijst.
CHAN_AOP_MOVED
	%s verplaatst naar %s AOP lijst.
CHAN_AOP_NO_SUCH_ENTRY
	#%d bestaat niet op %s AOP lijst.
CHAN_AOP_NOT_FOUND
	%s niet gevonden op %s AOP lijst.
CHAN_AOP_NO_MATCH
	Geen overeenkomsten op %s AOP lijst.
CHAN_AOP_DELETED
	%s verwijderd van %s AOP lijst.
CHAN_AOP_DELETED_ONE
	1 AOP verwijderd van %s AOP lijst.
CHAN_AOP_DELETED_SEVERAL
	%d AOPs verwijderd van %s AOP lijst.
CHAN_AOP_LIST_EMPTY
	%s AOP lijst is leeg.
CHAN_AOP_LIST_HEADER
	AOP lijst voor %s:
	  Num  Nick
CHAN_AOP_CLEAR
	Kanaal %s AOP lijst is leeggemaakt.

# HOP messages
CHAN_HOP_SYNTAX
	HOP kanaal {ADD|DEL|LIST|CLEAR} [nick | lijst]
CHAN_HOP_DISABLED
	Sorry, kanaal HOP lijst wijzigen is tijdelijk uitgeschakeld.
CHAN_HOP_NICKS_ONLY
	Kanaal HOP lijsten mogen alleen geregistreerde nicks bevatten.
CHAN_HOP_ADDED
	%s toegevoegd aan %s HOP lijst.
CHAN_HOP_MOVED
	%s verplaatst naar %s HOP lijst.
CHAN_HOP_NO_SUCH_ENTRY
	#%d bestaat niet op %s HOP lijst.
CHAN_HOP_NOT_FOUND
	%s niet gevonden op %s HOP lijst.
CHAN_HOP_NO_MATCH
	Geen overeenkomsten op %s HOP lijst.
CHAN_HOP_DELETED
	%s verwijderd van %s HOP lijst.
CHAN_HOP_DELETED_ONE
	1 HOP verwijderd van de %s HOP lijst.
CHAN_HOP_DELETED_SEVERAL
	%d HOPs verwijderd van de %s HOP lijst.
CHAN_HOP_LIST_EMPTY
	%s HOP lijst is leeg.
CHAN_HOP_LIST_HEADER
	HOP lijst voor %s:
	  Num  Nick
CHAN_HOP_CLEAR
	Kanaal %s HOP lijst is leeggemaakt.

# SOP messages
CHAN_SOP_SYNTAX
	SOP kanaal {ADD|DEL|LIST|CLEAR} [nick | lijst]
CHAN_SOP_DISABLED
	Sorry, kanaal SOP lijst wijzigen is tijdelijk uitgeschakeld.
CHAN_SOP_NICKS_ONLY
	Kanaal SOP list mag alleen geregistreerde nicks bevatten.
CHAN_SOP_ADDED
	%s toegevoegd aan %s SOP lijst.
CHAN_SOP_MOVED
	%s verplaatst naar %s SOP lijst.
CHAN_SOP_NO_SUCH_ENTRY
	#%d bestaat niet op %s SOP lijst.
CHAN_SOP_NOT_FOUND
	%s niet gevonden op %s SOP lijst.
CHAN_SOP_NO_MATCH
	Geen overeenkomsten op %s SOP lijst.
CHAN_SOP_DELETED
	%s verwijderd van %s SOP lijst.
CHAN_SOP_DELETED_ONE
	1 SOP verwijderd van %s SOP lijst.
CHAN_SOP_DELETED_SEVERAL
	%d SOPs verwijderd van %s SOP lijst.
CHAN_SOP_LIST_EMPTY
	%s SOP lijst is leeg.
CHAN_SOP_LIST_HEADER
	SOP lijst voor %s:
	  Num  Nick
CHAN_SOP_CLEAR
	Kanaal %s SOP lijst is leeggemaakt.

# VOP messages
CHAN_VOP_SYNTAX
	VOP kanaal {ADD|DEL|LIST|CLEAR} [nick | lijst]
CHAN_VOP_DISABLED
	Sorry, kanaal VOP lijst wijzigen is tijdelijk uitgeschakeld.
CHAN_VOP_NICKS_ONLY
	Kanaal VOP lijst mag alleen geregistreerde nicks bevatten.
CHAN_VOP_ADDED
	%s toegevoegd aan %s VOP lijst.
CHAN_VOP_MOVED
	%s verplaatst %s VOP lijst.
CHAN_VOP_NO_SUCH_ENTRY
	#%d bestaat niet op %s VOP lijst.
CHAN_VOP_NOT_FOUND
	%s niet gevonden op %s VOP lijst.
CHAN_VOP_NO_MATCH
	Geen overeenkomsten op %s VOP lijst.
CHAN_VOP_DELETED
	%s verwijderd van %s VOP lijst.
CHAN_VOP_DELETED_ONE
	1 VOP verwijderd van %s VOP lijst.
CHAN_VOP_DELETED_SEVERAL
	%d VOPs verwijderd van %s VOP lijst.
CHAN_VOP_LIST_EMPTY
	%s VOP lijst is empty.
CHAN_VOP_LIST_HEADER
	VOP lijst for %s:
	  Num  Nick
CHAN_VOP_CLEAR
	Kanaal %s VOP lijst is leeggemaakt.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS kanaal {ADD|DEL|LIST|CLEAR} [nick [niveau] | lijst]
CHAN_ACCESS_XOP
	Je kan dit commando niet gebruiken. 
	Gebruik de AOP, SOP en VOP commando's in plaats van deze.
	Type %R%s HELP command voor meer informatie.
CHAN_ACCESS_XOP_HOP
	You can't use this command. 
	Use the AOP, SOP, HOP and VOP commands instead.
	Type %R%s HELP command for more information.
CHAN_ACCESS_DISABLED
	Sorry, kanaal toegangslijst modificatie is tijdelijk uitgeschakeld.
CHAN_ACCESS_LEVEL_NONZERO
	Toegangsniveau mag geen nul zijn.
CHAN_ACCESS_LEVEL_RANGE
	Toegansniveau moet tussen de %d en %d inclusief liggen.
CHAN_ACCESS_NICKS_ONLY
	Kanaal toegangslijst mag alleen geregistreerde nicks bevatten.
CHAN_ACCESS_REACHED_LIMIT
	Sorry, je kan maar %d nicks op de toegangslijst van een kanaal hebben.
CHAN_ACCESS_LEVEL_UNCHANGED
	Toegangsniveau voor %s op %s onveranderd van %d.
CHAN_ACCESS_LEVEL_CHANGED
	Toegangsniveau voor %s op %s veranderd in %d.
CHAN_ACCESS_ADDED
	%s toegevoegd aan %s toeganslijst met niveau %d.
CHAN_ACCESS_NO_SUCH_ENTRY
	#%d bestaat niet op %s toegangslijst.
CHAN_ACCESS_NOT_FOUND
	%s niet gevonden op %s toegangslijst.
CHAN_ACCESS_NO_MATCH
	Geen overeenkomsten op %s toegangslijst.
CHAN_ACCESS_DELETED
	%s verwijderd van %s toegangslijst.
CHAN_ACCESS_DELETED_ONE
	1 nick verwijderd van %s toegangslijst.
CHAN_ACCESS_DELETED_SEVERAL
	%d nicks verwijderd van %s toegangslijst.
CHAN_ACCESS_LIST_EMPTY
	%s toegangslijst is leeg.
CHAN_ACCESS_LIST_HEADER
	Toegangslijst voor %s:
	  Num   Niv  Nick
CHAN_ACCESS_LIST_FOOTER
	Einde van de toegangslijst.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	Kanaal %s toegangslijst is leeggemaakt.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK kanaal {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [nick-of-usermask] [reden]
CHAN_AKICK_DISABLED
	Sorry, kanaal autokick lijst wijzigen is tijdelijk uitgeschakeld.
CHAN_AKICK_ALREADY_EXISTS
	%s bestaat al op %s autokick lijst.
CHAN_AKICK_REACHED_LIMIT
	Sorry, je kan maar %d autokick masks op een kanaal hebben.
CHAN_AKICK_ADDED
	%s toegevoegd aan %s autokick lijst.
CHAN_AKICK_NO_SUCH_ENTRY
	#%d bestaat niet op %s autokick lijst.
CHAN_AKICK_NOT_FOUND
	%s niet gevonden op %s autokick lijst.
CHAN_AKICK_NO_MATCH
	Geen overeenkomsten op %s autokick lijst.
CHAN_AKICK_STUCK
	%s is nu altijd actief op kanaal %s.
CHAN_AKICK_UNSTUCK
	%s is niet meer altijd actief op kanaal %s.
CHAN_AKICK_DELETED
	%s verwijderd van %s autokick lijst.
CHAN_AKICK_DELETED_ONE
	1 autokick verwijderd van %s autokick lijst.
CHAN_AKICK_DELETED_SEVERAL
	%d autokicks verwijderd van %s autokick lijst.
CHAN_AKICK_LIST_EMPTY
	%s autokick lijst is leeg.
CHAN_AKICK_LIST_HEADER
	Autokick lijst voor %s:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (door %s op %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (eeuwig) (door %s op %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE voor %s compleet; %d gebruikers zijn getroffen.
CHAN_AKICK_CLEAR
	Kanaal %s autokick lijst is leeggemaakt.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS kanaal {SET | DIS[ABLE] | LIST | RESET} [item [niveau]]
CHAN_LEVELS_XOP
	Dit commando is nutteloos in deze mode.
CHAN_LEVELS_RANGE
	Niveau moet tussen de %d en %d inclusief zijn.
CHAN_LEVELS_CHANGED
	Niveau voor %s op kanaal %s veranderd in %d.
CHAN_LEVELS_UNKNOWN
	Instelling %s onbekend.  Type %R%s HELP LEVELS DESC voor een lijst van geldige instellingen.
CHAN_LEVELS_DISABLED
	%s uitgeschakeld op kanaal %s.
CHAN_LEVELS_LIST_HEADER
	Toegangsniveau instellingen voor kanaal %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (uitgeschakeld)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (alleen stichter)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	Toegangsniveaus voor %s teruggezet naar standaardwaarden.

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
	INFO kanaal [ALL]
CHAN_INFO_HEADER
	Informatie voor kanaal %s:
CHAN_INFO_FOUNDER
	        Stichter: %s (%s)
CHAN_INFO_NO_FOUNDER
	        Stichter: %s
CHAN_INFO_SUCCESSOR
	        Opvolger: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	        Opvolger: %s
CHAN_INFO_DESCRIPTION
	    Beschrijving: %s
CHAN_INFO_ENTRYMSG
	 Toegangsbericht: %s
CHAN_INFO_TIME_REGGED
	   Geregistreerd: %s
CHAN_INFO_LAST_USED
	 Laatst gebruikt: %s
CHAN_INFO_LAST_TOPIC
	   Laatste topic: %s
CHAN_INFO_TOPIC_SET_BY
	Topic gezet door: %s
CHAN_INFO_URL
	             URL: %s
CHAN_INFO_EMAIL
	    E-mail adres: %s
CHAN_INFO_BANTYPE
	        Ban type: %d
CHAN_INFO_OPTIONS
	          Opties: %s
CHAN_INFO_OPT_KEEPTOPIC
	Topicbehoud
CHAN_INFO_OPT_OPNOTICE
	OP Notice
CHAN_INFO_OPT_PEACE
	Vrede
CHAN_INFO_OPT_PRIVATE
	Prive
CHAN_INFO_OPT_RESTRICTED
	Beperkte Toegang
CHAN_INFO_OPT_SECURE
	Veilig
CHAN_INFO_OPT_SECUREOPS
	Veilige Ops
CHAN_INFO_OPT_SECUREFOUNDER
	Veilige Stichter
CHAN_INFO_OPT_SIGNKICK
	Ondertekende Kicks
CHAN_INFO_OPT_TOPICLOCK
	Topicslot
CHAN_INFO_OPT_XOP
	xOP lijsten systeem
CHAN_INFO_OPT_NONE
	Geen
CHAN_INFO_MODE_LOCK
	        Modeslot: %s
CHAN_INFO_EXPIRE
	      Expires in: %s
CHAN_INFO_NO_EXPIRE
	Dit kanaal zal niet verlopen.

# LIST responses
CHAN_LIST_SYNTAX
	LIST trefbeeld
CHAN_LIST_SERVADMIN_SYNTAX
	LIST trefbeeld [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
CHAN_LIST_HEADER
	Lijst van kanalen overeenkomend met %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Einde van lijst - %d/%d overeenkomsten weergegeven.
	
# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE kanaal

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN kanaal
CHAN_UNBANNED
	Bans met betrekking tot jou op %s zijn opgeheven.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC kanaal [topic]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR kanaal wat
CHAN_CLEARED_BANS
	Alle bans op kanaal %s zijn weggehaald.
CHAN_CLEARED_EXCEPTS
	Alle uitzonderingen op kanaal %s zijn verwijderd.
CHAN_CLEARED_MODES
	Alle modes op kanaal %s zijn teruggezet op hun oorspronkelijke waarde.
CHAN_CLEARED_OPS
	Mode +o is verwijderd van kanaal %s.
CHAN_CLEARED_HOPS
	Mode +h is verwijderd van kanaal %s.
CHAN_CLEARED_VOICES
	Mode +v is verwijderd van kanaal %s.
CHAN_CLEARED_USERS
	Alle users zijn gekicked van kanaal %s.
CHAN_CLEARED_INVITES
	All invites on channel %s have been removed.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS kanaal
CHAN_GETPASS_UNAVAILABLE
	GETPASS commando onbeschikbaar wegens encryptie van de gegevens.
CHAN_GETPASS_PASSWORD_IS
	Wachtwoord voor kanaal %s is %s.
	
# SENDPASS responses
CHAN_GETKEY_SYNTAX
	GETKEY channel
CHAN_GETKEY_NOKEY
	The channel %s has no key.
CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS kanaal
CHAN_SENDPASS_UNAVAILABLE
	SENDPASS commando onbeschikbaar wegens encryptie van de gegevens.
CHAN_SENDPASS_SUBJECT
	Kanaal wachtwoord (%s)
CHAN_SENDPASS_HEAD
	Hallo,
CHAN_SENDPASS_LINE_1
	Je hebt het wachtwoord aangevraag van kanaal %s via e-mail.
CHAN_SENDPASS_LINE_2
	Het wachtwoord is %s. Wegens veiligheidsredenen zou je deze zo snel mogelijk moeten veranderen.
CHAN_SENDPASS_LINE_3
	Als je niet weet waarom deze e-mail verstuurd is, negeer deze dan.
CHAN_SENDPASS_LINE_4
	ANTWOORD NIET OP DEZE MAIL!
CHAN_SENDPASS_LINE_5
	%s beheerders.
CHAN_SENDPASS_OK
	Wachtwoord %s is verstuurd.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID kanaal [reden]
CHAN_FORBID_SYNTAX_REASON
	FORBID kanaal reden
CHAN_FORBID_SUCCEEDED
	Kanaal %s is nu verboden.
CHAN_FORBID_FAILED
	Kan kanaal %s niet verbieden!
CHAN_FORBID_REASON
	Dit kanaal is verboden.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND kanaal [reden]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND kanaal reden
CHAN_SUSPEND_SUCCEEDED
	Kanaal %s is tijdelijk buiten werking gesteld.
CHAN_SUSPEND_FAILED
	Kan kanaal %s niet tijdelijk buiten werking stellen!
CHAN_SUSPEND_REASON
	Dit kanaal is tijdelijk buiten werking gesteld.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND kanaal
CHAN_UNSUSPEND_ERROR
	Geen # gevonden aan het begin van de kanaalnaam.
CHAN_UNSUSPEND_SUCCEEDED
	Kanaal %s is nu weer in werking gesteld.
CHAN_UNSUSPEND_FAILED
	Kan kanaal %s niet terug in werking stellen!

# Misc responses
CHAN_EXCEPTED
	%s komt overeen met een uitzondering op %s en kan niet worden gebanned totdat de uitzondering is verwijderd.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	Je hebt 1 nieuwe memo.
MEMO_HAVE_NEW_MEMOS
	Je hebt %d nieuwe memo's.
MEMO_TYPE_READ_LAST
	Type %R%s READ LAST om deze te lezen.
MEMO_TYPE_READ_NUM
	Type %R%s READ %d om deze te lezen.
MEMO_TYPE_LIST_NEW
	Type %R%s LIST NEW om ze weer te geven.
MEMO_AT_LIMIT
	Waarschuwing: Je hebt je maximum aantal memo's bereikt (%d).  Je kan geen nieuwe memo's ontvangen totdat je enkele van je huidige memo's verwijdert.
MEMO_OVER_LIMIT
	Waarschuwing: Je hebt je maximum aantal memo's overschreden (%d).  Je kan geen nieuwe memo's ontvangen totdat je enkele van je huidige memo's verwijdert.
MEMO_X_MANY_NOTICE
	Er zijn %d memo's op kanaal %s.
MEMO_X_ONE_NOTICE
	Er is %d memo op kanaal %s.
MEMO_NEW_X_MEMO_ARRIVED
	Er is een nieuwe memo op kanaal %s.
	Type %R%s READ %s %d om deze te lezen.
MEMO_NEW_MEMO_ARRIVED
	Je hebt een nieuwe memo van %s.
	Type %R%s READ %d om deze te lezen.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Je hebt geen memo's.
MEMO_X_HAS_NO_MEMOS
	%s heeft geen memo's.
MEMO_DOES_NOT_EXIST
	Memo %d bestaat niet!
MEMO_LIST_NOT_FOUND
	Geen overeenkomende memo's gevonden.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {nick | kanaal} memo-text
MEMO_SEND_DISABLED
	Sorry, memo's sturen is tijdelijk uitgeschakeld.
MEMO_SEND_PLEASE_WAIT
	Wacht %d seconden voor het opnieuw gebruiken van het SEND commando.
MEMO_X_GETS_NO_MEMOS
	%s kan geen memo's ontvangen.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s heeft momenteel te veel memo's en kan er geen meer ontvangen.
MEMO_SENT
	Memo verstuurd naar %s.
MEMO_MASS_SENT
	Een memo werd gestuurd naar alle geregistreerde gebruikers.
	
# STAFF responses
MEMO_STAFF_SYNTAX
	STAFF memo-text
	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {nick | kanaal}
MEMO_CANCEL_DISABLED
	Sorry, memo's annuleren is tijdelijk uitgeschakeld.
MEMO_CANCEL_NONE
	Geen annuleerbare memo's gevonden.
MEMO_CANCELLED
	Laatste memo naar %s is ge-annuleerd.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [kanaal] [lijst | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	Je hebt geen nieuwe memo's.
MEMO_X_HAS_NO_NEW_MEMOS
	%s heeft geen nieuwe memo's.
MEMO_LIST_MEMOS
	Memo's voor %s.  Om te lezen, type: %R%s READ nr
MEMO_LIST_NEW_MEMOS
	Nieuwe memo's voor %s.  Om te lezen, type: %R%s READ nr
MEMO_LIST_CHAN_MEMOS
	Memo's voor %s.  Om te lezen, type: %R%s READ %s nr
MEMO_LIST_CHAN_NEW_MEMOS
	Nieuwe memo's voor %s.  Om te lezen, type: %R%s READ %s nr
MEMO_LIST_HEADER
	 Nr   Verstuurder       Datum/Tijd
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [kanaal] {lijst | LAST | NEW}
MEMO_HEADER
	Memo %d van %s (%s).  Om te verwijderen, type: %R%s DEL %d
MEMO_CHAN_HEADER
	Memo %d van %s (%s).  Om te verwijderen, type: %R%s DEL %s %d
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [kanaal] {nr | lijst | ALL}
MEMO_DELETED_NONE
	Geen memo's verwijderd.
MEMO_DELETED_ONE
	Memo %d is verwijderd.
MEMO_DELETED_SEVERAL
	Memo's %s zijn verwijderd.
MEMO_DELETED_ALL
	Al je memo's zijn verwijderd.
MEMO_CHAN_DELETED_ALL
	Alle memo's voor kanaal %s zijn verwijderd.

# SET responses
MEMO_SET_SYNTAX
	SET optie parameters
MEMO_SET_DISABLED
	Sorry, memo opties instellen is tijdelijk uitgeschakeld.
MEMO_SET_UNKNOWN_OPTION
	Onbekende SET optie %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
MEMO_SET_NOTIFY_ON
	%s zal je nu informeren over memo's zodra je inlogt en wanneer deze naar je toe gezonden worden.
MEMO_SET_NOTIFY_LOGON
	%s zal je nu informeren over memo's wanneer je inlogt of /AWAY uitschakelt.
MEMO_SET_NOTIFY_NEW
	%s zal je nu informeren over memo's wanneer deze naar jou worden verzonden.
MEMO_SET_NOTIFY_OFF
	%s zal je nu niet over memo's informeren.

# SET LIMIT responses
MEMO_SET_NOTIFY_MAIL
	You will now be informed about new memos via email.
MEMO_SET_NOTIFY_NOMAIL
	You will no longer be informed via email.
MEMO_SET_NOTIFY_INVALIDMAIL
	There's no email address set for your nick.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [kanaal] limiet
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [gebruiker | kanaal] {limiet | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Je het geen toestemming je memo limiet te veranderen.
MEMO_SET_LIMIT_FORBIDDEN
	De memo limiet voor %s mag niet worden veranderd.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Je kan je memo limiet niet hoger dan %d zetten.
MEMO_SET_LIMIT_TOO_HIGH
	Je kan de memo limiet voor %s niet hoger dan %d zetten.
MEMO_SET_LIMIT_OVERFLOW
	Memo limiet te hoog; limitatie ingesteld op %d.
MEMO_SET_YOUR_LIMIT
	Je memo limiet is veranderd in %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Je kan niet langer memo's ontvangen.
MEMO_UNSET_YOUR_LIMIT
	Je memo limiet is uitgeschakeld.
MEMO_SET_LIMIT
	Memo limiet voor %s veranderd in %d.
MEMO_SET_LIMIT_ZERO
	Memo limiet voor %s veranderd in 0.
MEMO_UNSET_LIMIT
	Memo limit voor %s uitgeschakeld.

# INFO responses
MEMO_INFO_SYNTAX
	INFO [kanaal]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [nick | kanaal]
MEMO_INFO_NO_MEMOS
	Je hebt momenteel geen memo's.
MEMO_INFO_MEMO
	Je hebt momenteel 1 memo.
MEMO_INFO_MEMO_UNREAD
	Je hebt momenteel 1 memo, en deze is nog niet gelezen.
MEMO_INFO_MEMOS
	Je hebt momenteel %d memo's.
MEMO_INFO_MEMOS_ONE_UNREAD
	Je hebt momenteel %d memo's, waarven er 1 ongelezen is.
MEMO_INFO_MEMOS_SOME_UNREAD
	Je hebt momenteel %d memo's, waarvan er %d ongelezen zijn.
MEMO_INFO_MEMOS_ALL_UNREAD
	Je hebt momenteel %d memo's; allemaal zijn ze ongelezen.
MEMO_INFO_LIMIT
	Je memo limiet is %d.
MEMO_INFO_HARD_LIMIT
	Je memo limiet is %d, en mag niet worden veranderd.
MEMO_INFO_LIMIT_ZERO
	Je memo limiet is 0; Je zal geen memo's ontvangen.
MEMO_INFO_HARD_LIMIT_ZERO
	Je memo limiet is 0; Je zal geen memo's ontvangen.  Je kan deze limiet niet veranderen.
MEMO_INFO_NO_LIMIT
	Er staat geen limiet op het aantal memo's dat je mag hebben.
MEMO_INFO_NOTIFY_OFF
	Je wordt niet geinformeerd over nieuwe memo's.
MEMO_INFO_NOTIFY_ON
	Je wordt geinformeerd over nieuwe memo's wanneer je inlogt en wanneer deze arriveren.
MEMO_INFO_NOTIFY_RECEIVE
	Je wordt geinformeerd over nieuwe memo's wanneer deze arriveren.
MEMO_INFO_NOTIFY_SIGNON
	Je wordt geinformeerd over nieuwe memo's wanneer je inlogt.
MEMO_INFO_X_NO_MEMOS
	%s heeft momenteel geen memo's.
MEMO_INFO_X_MEMO
	%s heeft momenteel 1 memo.
MEMO_INFO_X_MEMO_UNREAD
	%s heeft momenteel 1 memo, en deze is nog ongelezen.
MEMO_INFO_X_MEMOS
	%s heeft momenteel %d memo's.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s heeft momenteel %d memo's, waarvan er 1 ongelezen is.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s heeft momenteel %d memo's, waarvan er %d ongelezen zijn.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s heeft momenteel %d memo's; allemaal zijn ze ongelezen.
MEMO_INFO_X_LIMIT
	%s's memo limiet is %d.
MEMO_INFO_X_HARD_LIMIT
	%s's memo limiet is %d, en mag niet worden veranderd.
MEMO_INFO_X_NO_LIMIT
	%s heeft geen memo limiet.
MEMO_INFO_X_NOTIFY_OFF
	%s wordt niet geinformeerd over nieuwe memo's.
MEMO_INFO_X_NOTIFY_ON
	%s wordt geinformeerd over nieuwe memo's wanneer je inlogt en wanneer deze arriveren.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s wordt geinformeerd over nieuwe memo's wanneer deze arriveren.
MEMO_INFO_X_NOTIFY_SIGNON
	%s wordt geinformeerd over nieuwe memo's wanneer je inlogt.
	
###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_MAIL_SUBJECT
	Nieuwe memo
MEMO_MAIL_TEXT1
	Hoi %s
MEMO_MAIL_TEXT2
	Je hebt zojuist een memo ontvangen van %s. Het gaat om memo nummer %d.
MEMO_MAIL_TEXT3
	Inhoud van de memo:

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_RSEND_PLEASE_WAIT
	Wacht %d seconden voor je het RSEND commando weer gebruikt.
MEMO_RSEND_DISABLED
	Sorry, RSEND is uitgeschakeld op dit netwerk.
MEMO_RSEND_SYNTAX
	RSEND {nick | kanaal} memo-tekst
MEMO_RSEND_NICK_MEMO_TEXT
	[auto-memo] De memo die je gestuurd hebt is bekeken.
MEMO_RSEND_CHAN_MEMO_TEXT
	[auto-memo] De memo die je naar %s gestuurd hebt is bekeken.
MEMO_RSEND_USER_NOTIFICATION
	Een notificatie-memo is naar %s verstuurd om hem/haar te
	informeren dat je zijn/haar memo gelezen hebt.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_CHECK_SYNTAX
	CHECK nick
MEMO_CHECK_NOT_READ
	De laatste memo die je naar %s gestuurd hebt (op %s) is nog niet gelezen.
MEMO_CHECK_READ
	De laatste memo die je naar %s gestuurd hebt (op %s) is gelezen.
MEMO_CHECK_NO_MEMO
	Nick %s heeft geen memo van jou.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_NO_RSEND_SELF
	Je kan geen ontvangstbevestiging aanvragen voor een
	memo die je naar jezelf stuurt.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
BOT_DOES_NOT_EXIST
	Bot %s bestaat niet.
BOT_NOT_ASSIGNED
	Je moet een bot aan een kanaal toewijzen voordat je dit commando gebruikt.
	Type %R%S HELP ASSIGN voor meer informatie.
BOT_NOT_ON_CHANNEL
	Bot is niet in kanaal %s.
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	Gebruik het woord %s niet in dit kanaal!
BOT_REASON_BADWORD_GENTLE
	Let op je taal!
BOT_REASON_BOLD
	Gebruik geen vet in dit kanaal!
BOT_REASON_CAPS
	Zet caps lock UIT!
BOT_REASON_COLOR
	Gebruik geen kleuren in dit kanaal!
BOT_REASON_FLOOD
	Stop met flooden!
BOT_REASON_REPEAT
	Stop met jezelf herhalen!
BOT_REASON_REVERSE
	Gebruik geen inverteringen in dit kanaal!
BOT_REASON_UNDERLINE
	Gebruik geen onderstrepingen in dit kanaal!
	
# !seen replies
BOT_SEEN_BOT
	Je hebt me gevonden, %s!
BOT_SEEN_YOU
	Naar jezelf aan het zoeken he, %s?
BOT_SEEN_ON_CHANNEL
	%s is nu op het kanaal!
BOT_SEEN_ON_CHANNEL_AS
	%s is nu op het kanaal (als %s) ! 
BOT_SEEN_ON
	%s is voor het laatst %s geleden gezien.
BOT_SEEN_NEVER
	Ik heb %s nooit op dit kanaal gezien.
BOT_SEEN_UNKNOWN
	Ik weet niet wie %s is.

# BOT responses
BOT_BOT_SYNTAX
	BOT ADD nick gebruiker host echte naam
	BOT CHANGE oude-nick nieuwe-nick [gebruiker [host [echte naam]]]
	BOT DEL nick
BOT_BOT_ALREADY_EXISTS
	Bot %s bestaat al.
BOT_BOT_CREATION_FAILED
	Sorry, bot creatie gefaald.
BOT_BOT_READONLY
	Sorry, bot wijzigen is tijdelijk uitgeschakeld.
BOT_BOT_ADDED
	%s!%s@%s (%s) toegevoegd aan de bot lijst.
BOT_BOT_ANY_CHANGES
	Oude informatie is gelijk aan de nieuwe.
BOT_BOT_CHANGED
	Bot %s is veranderd in %s!%s@%s (%s)
BOT_BOT_DELETED
	Bot %s is verwijderd.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Bot lijst:
BOT_BOTLIST_PRIVATE_HEADER
	Bots gereserveerd voor IRC operators:
BOT_BOTLIST_FOOTER
	%d bots beschikbaar.
BOT_BOTLIST_EMPTY
	Er zijn momenteel geen bots beschikbaar.
	Vraag een Services beheerder er een te maken!
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN kanaal nick
BOT_ASSIGN_READONLY
	Sorry, bot toewijzing is tijdelijk uitgeschakeld.
BOT_ASSIGN_ALREADY
	Channel %s heeft al een bot toegewezen namelijk: %s.
BOT_ASSIGN_ASSIGNED
	Bot %s toegewezen aan %s.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN kanaal
BOT_UNASSIGN_UNASSIGNED
	Er is geen bot meer toegewezen aan %s.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {kanaal | nick}
BOT_INFO_NOT_FOUND
	%s is geen geldige bot of geregistreerd kanaal.
BOT_INFO_BOT_HEADER
	Informatie voor bot %s:
BOT_INFO_BOT_MASK
	       Mask: %s@%s
BOT_INFO_BOT_REALNAME
	 Echte naam: %s
BOT_INFO_BOT_CREATED
	    Gemaakt: %s
BOT_INFO_BOT_USAGE
	Gebruikt op: %d kana(a)l(en)
BOT_INFO_BOT_OPTIONS
	     Opties: %s
BOT_INFO_OPT_PRIVATE
	Prive
BOT_INFO_CHAN_HEADER
	Informatie voor kanaal %s:
BOT_INFO_CHAN_BOT
	             Bot nick: %s
BOT_INFO_CHAN_BOT_NONE
	             Bot nick: nog geen toegewezen.
BOT_INFO_CHAN_KICK_BADWORDS
	 Foute woorden kicker: %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	 Foute woorden kicker: %s (%d kick(s) om te bannen)
BOT_INFO_CHAN_KICK_BOLDS
	           Vet kicker: %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	           Vet kicker: %s (%d kick(s) om te bannen)
BOT_INFO_CHAN_KICK_CAPS_ON
	   Hoofdletter kicker: %s (minimaal %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	   Hoofdletter kicker: %s (%d kick(s) om te bannen; minimaal %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	   Hoofdletter kicker: %s
BOT_INFO_CHAN_KICK_COLORS
	       Kleuren kicker: %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	       Kleuren kicker: %s (%d kick(s) om te bannen)
BOT_INFO_CHAN_KICK_FLOOD_ON
	         Flood kicker: %s (%d regels in %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	         Flood kicker: %s (%d kick(s) om te bannen; %d regels in %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	         Flood kicker: %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	    Herhalings kicker: %s (%d maal)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	    Herhalings kicker: %s (%d kick(s) om te bannen; %d maal)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	    Herhalings kicker: %s
BOT_INFO_CHAN_KICK_REVERSES
	  Inverterings kicker: %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	  Inverterings kicker: %s (%d kick(s) om te bannen)
BOT_INFO_CHAN_KICK_UNDERLINES
	Onderstrepings kicker: %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	Onderstrepings kicker: %s (%d kick(s) om bannen)
BOT_INFO_ACTIVE
	ingeschakeld
BOT_INFO_INACTIVE
	uitgeschakeld
BOT_INFO_CHAN_OPTIONS
	               Opties: %s
BOT_INFO_OPT_DONTKICKOPS
	Ops bescherming
BOT_INFO_OPT_DONTKICKVOICES
	Voices bescherming
BOT_INFO_OPT_FANTASY
	Fantasie
BOT_INFO_OPT_GREET
	Begroeting
BOT_INFO_OPT_NOBOT
	Geen bot
BOT_INFO_OPT_SYMBIOSIS
	Symbiose
BOT_INFO_OPT_NONE
	Geen
	
# SET responses
BOT_SET_SYNTAX
	SET kanaal optie instellingen
BOT_SET_DISABLED
	Sorry, bot opties instellen is tijdelijk uitgeschakeld.
BOT_SET_UNKNOWN
	Onbekende optie %s.
	Type %R%S HELP SET voor meer informatie.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET kanaal DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Bot zal geen ops kicken op kanaal %s.
BOT_SET_DONTKICKOPS_OFF
	Bot zal ops kicken op kanaal %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET kanaal DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Bot zal geen voices kicken op kanaal %s.
BOT_SET_DONTKICKVOICES_OFF
	Bot zal voices kicken op kanaal %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET kanaal FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Fantasie mode staat nu AAN op kanaal %s.
BOT_SET_FANTASY_OFF
	Fantasie mode staat nu UIT op kanaal %s.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET kanaal GREET {ON|OFF}
BOT_SET_GREET_ON
	Begroetingsmode staat nu AAN op kanaal %s.
BOT_SET_GREET_OFF
	Begroetingsmode staat nu UIT op kanaal %s.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET kanaal NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Geen Bot mode staat nu AAN op kanaal %s.
BOT_SET_NOBOT_OFF
	Geen Bot mode staat nu UIT op kanaal %s.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET kanaal PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Prive mode van bot %s staat nu AAN.
BOT_SET_PRIVATE_OFF
	Prive mode van bot %s staat nu UIT.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET kanaal SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Symbiose mode staat nu AAN op kanaal %s.
BOT_SET_SYMBIOSIS_OFF
	Symbiose mode staat nu UIT op kanaal %s.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK kanaal optie {ON|OFF} [instellingen]
BOT_KICK_DISABLED
	Sorry, kicker configuratie is tijdelijk uitgeschakeld.
BOT_KICK_UNKNOWN
	Onbekende optie %s.
	Type %R%S HELP KICK voor meer informatie.
BOT_KICK_BAD_TTB
	%s kan niet worden ingesteld als aantal keer om te bannen.

BOT_KICK_BADWORDS_ON
	Bot zal nu kicken op slechte woorden. Gebruik het BADWORDS commando
	om slechte woorden toe te voegen of te verwijderen.
BOT_KICK_BADWORDS_ON_BAN
	Bot zal nu kicken op slechte woorden, en zal een ban zetten na 
	%d kicks voor dezelde gebruiker. Gebruik BADWORDS commando
	om slechte woorden toe te voegen of te verwijderen.
BOT_KICK_BADWORDS_OFF
	Bot zal niet meer kicken op slechte woorden.
BOT_KICK_BOLDS_ON
	Bot zal nu kicken op vet.
BOT_KICK_BOLDS_ON_BAN
	Bot zal nu kicken op vet, en zal een ban zetten na 
	%d kicks voor dezelfde gebruiker.
BOT_KICK_BOLDS_OFF
	Bot zal niet meer kicken op vet.
BOT_KICK_CAPS_ON
	Bot zal nu kicken op hoofdletters (bij minstens
	%d karakters en %d%% van het gehele bericht).
BOT_KICK_CAPS_ON_BAN
	Bot zal nu kicken op hoofdletters (bij minstens
	%d karakters en %d%% van het gehele bericht), en zal 
	een ban plaatsen na %d kicks voor dezelfde gebruiker.
BOT_KICK_CAPS_OFF
	Bot zal niet meer kicken op hoofdletters.
BOT_KICK_COLORS_ON
	Bot zal nu kicken op kleuren.
BOT_KICK_COLORS_ON_BAN
	Bot zal nu kicken op kleuren, en zal een ban zetten na 
	%d kicks voor dezelfde gebruiker.
BOT_KICK_COLORS_OFF
	Bot zal niet meer kicken op kleuren.
BOT_KICK_FLOOD_ON
	Bot zal nu kicken op flood (%d regels in %d seconden).
BOT_KICK_FLOOD_ON_BAN
	Bot zal nu kicken op flood (%d regels in %d seconden), 
	en zal een ban zitten na %d kicks voor dezelfde gebruiker.
BOT_KICK_FLOOD_OFF
	Bot zal niet meer kicken op flood.
BOT_KICK_REPEAT_ON
	Bot zal nu kicken op herhalingen (users die %d maal
	hetzelfde zeggen).
BOT_KICK_REPEAT_ON_BAN
	Bot zal nu kicken op herhalingen (users die %d maal
	hetzelfde zeggen), en zal een ban zetten na %d
	kicks voor dezelfde gebruiker.
BOT_KICK_REPEAT_OFF
	Bot zal niet meer kicken op herhalingen.
BOT_KICK_REVERSES_ON
	Bot zal nu kicken op inverteringen.
BOT_KICK_REVERSES_ON_BAN
	Bot zal nu kicken op inverteringen, en zal een ban zetten na
	%d kicks voor dezelfde gebruiker.
BOT_KICK_REVERSES_OFF
	Bot zal niet meer kicken op inverteringen.
BOT_KICK_UNDERLINES_ON
	Bot zal nu kicken op onderstrepingen.
BOT_KICK_UNDERLINES_ON_BAN
	Bot zal nu kicken op onderstrepingen, en zal een ban zetten na 
	%d kicks voor dezelfde gebruiker.
BOT_KICK_UNDERLINES_OFF
	Bot zal niet meer kicken op onderstrepingen.
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS kanaal {ADD|DEL|LIST|CLEAR} [woord | entry-lijst] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	Sorry, kanaal slechte woorden lijst wijzigen is tijdelijk uitgeschakeld.
BOT_BADWORDS_REACHED_LIMIT
	Sorry, je kan maar %d slechte woorden per kanaal instellen.
BOT_BADWORDS_ALREADY_EXISTS
	%s bestaat al op %s slechte woorden lijst.
BOT_BADWORDS_ADDED
	%s toegevoegd aan %s slechte woorden lijst.
BOT_BADWORDS_NO_SUCH_ENTRY
	#%d niet gevonden op %s slechte woorden lijst.
BOT_BADWORDS_NOT_FOUND
	%s niet gevonden op %s slechte woorden lijst.
BOT_BADWORDS_NO_MATCH
	Geen overeenkomsten op %s slechte woorden lijst.
BOT_BADWORDS_DELETED
	%s verwijderd van %s slechte woorden lijst.
BOT_BADWORDS_DELETED_ONE
	1 slecht woord verwijderd van %s slechte woorden lijst.
BOT_BADWORDS_DELETED_SEVERAL
	%d slechte woorden verwijderd van %s slechte woorden lijst.
BOT_BADWORDS_LIST_EMPTY
	%s slechte woorden lijst is leeg.
BOT_BADWORDS_LIST_HEADER
	Slechte woorden lijst voor %s:
	  Nr    Woord                          Type
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	Slechte woorden lijst is nu leeg.

# SAY responses
BOT_SAY_SYNTAX
	SAY kanaal text

# ACT responses
BOT_ACT_SYNTAX
	ACT kanaal text

# Errors
BOT_EXCEPT
	Gebruiker komt overeen met kanaal-uitzondering.
BOT_BAD_NICK
	Bot Nicks mogen alleen geldige nick-tekens bevatten.
BOT_BAD_HOST
	Bot Hosts mogen alleen geldige host-tekens bevatten.
BOT_BAD_IDENT
	Bot Idents mogen alleen geldige tekens bevatten.
BOT_LONG_IDENT
	Bot Idents mogen maximaal %d tekens bevatten.
BOT_LONG_HOST
	Bot Hosts mogen maximaal %d tekens bevatten.

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Services kan geen modes veranderen.  Zijn je server goed geconfigureerd?
OPER_BOUNCY_MODES_U_LINE
	Services kan geen modes veranderen.  Zijn de  U:lines van je servers goed geconfigureerd?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL bericht

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Onbekende STATS optie %s.
OPER_STATS_CURRENT_USERS
	Huidige gebruikers: %d (%d ops)
OPER_STATS_MAX_USERS
	Maximum gebruikers: %d (%s)
# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Services draaien %d dagen, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Services draaien %d dagen, %02d:%02d
OPER_STATS_UPTIME_HMS
	Services draaien %d uur, %d minuten
OPER_STATS_UPTIME_HM1S
	Services draaien %d uur, %d minuten
OPER_STATS_UPTIME_H1MS
	Services draaien %d uur, %d minuut
OPER_STATS_UPTIME_H1M1S
	Services draaien %d uur, %d minuut
OPER_STATS_UPTIME_1HMS
	Services draaien %d uur, %d minuten
OPER_STATS_UPTIME_1HM1S
	Services draaien %d uur, %d minuten
OPER_STATS_UPTIME_1H1MS
	Services draaien %d uur, %d minuut
OPER_STATS_UPTIME_1H1M1S
	Services draaien %d uur, %d minuut
OPER_STATS_UPTIME_MS
	Services draaien %d minuten, %d seconden
OPER_STATS_UPTIME_M1S
	Services draaien %d minuten, %d seconde
OPER_STATS_UPTIME_1MS
	Services draaien %d minuut, %d seconden
OPER_STATS_UPTIME_1M1S
	Services draaien %d minuut, %d seconde
OPER_STATS_BYTES_READ
	   Bytes gelezen: %5d kB
OPER_STATS_BYTES_WRITTEN
	Bytes geschreven: %5d kB
OPER_STATS_USER_MEM
	Gebruiker     : %6d records, %5d kB
OPER_STATS_CHANNEL_MEM
	Kanaal        : %6d records, %5d kB
OPER_STATS_GROUPS_MEM
	NS Groepen    : %6d records, %5d kB
OPER_STATS_ALIASES_MEM
	NS Aliassen   : %6d records, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ      : %6d records, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ       : %6d records, %5d kB
OPER_STATS_HOSTSERV_MEM
	HostServ      : %6d records, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ      : %6d records, %5d kB
OPER_STATS_SESSIONS_MEM
	Sessies       : %6d records, %5d kB
OPER_STATS_AKILL_COUNT
	Huidig aantal AKILLs: %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Standaard AKILL verloop tijd: %d dagen
OPER_STATS_AKILL_EXPIRE_DAY
	Standaard AKILL verloop rijd: 1 dag
OPER_STATS_AKILL_EXPIRE_HOURS
	Standaard AKILL verloop tijd: %d uur
OPER_STATS_AKILL_EXPIRE_HOUR
	Standaard AKILL verloop tijd: 1 uur
OPER_STATS_AKILL_EXPIRE_MINS
	Standaard AKILL verloop tijd: %d minuten
OPER_STATS_AKILL_EXPIRE_MIN
	Standaard AKILL verloop tijd: 1 minuut
OPER_STATS_AKILL_EXPIRE_NONE
	Standaard AKILL verloop tijd: Verloopt niet
OPER_STATS_SGLINE_COUNT
	Huidig aantal SGLINEs: %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Standaard SGLINE verloop tijd: %d dagen
OPER_STATS_SGLINE_EXPIRE_DAY
	Standaard SGLINE verloop tijd: 1 dag
OPER_STATS_SGLINE_EXPIRE_HOURS
	Standaard SGLINE verloop tijd: %d uur
OPER_STATS_SGLINE_EXPIRE_HOUR
	Standaard SGLINE verloop tijd: 1 uur
OPER_STATS_SGLINE_EXPIRE_MINS
	Standaard SGLINE verloop tijd: %d minuten
OPER_STATS_SGLINE_EXPIRE_MIN
	Standaard SGLINE verloop tijd: 1 minuut
OPER_STATS_SGLINE_EXPIRE_NONE
	Standaard SGLINE verloop tijd: Verloopt niet
OPER_STATS_SQLINE_COUNT
	Huidig aantal SQLINEs: %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Standaard SQLINE verloop tijd: %d dagen
OPER_STATS_SQLINE_EXPIRE_DAY
	Standaard SQLINE verloop tijd: 1 dag
OPER_STATS_SQLINE_EXPIRE_HOURS
	Standaard SQLINE verloop tijd: %d uur
OPER_STATS_SQLINE_EXPIRE_HOUR
	Standaard SQLINE verloop tijd: 1 uur
OPER_STATS_SQLINE_EXPIRE_MINS
	Standaard SQLINE verloop tijd: %d minuten
OPER_STATS_SQLINE_EXPIRE_MIN
	Standaard SQLINE verloop tijd: 1 minuut
OPER_STATS_SQLINE_EXPIRE_NONE
	Standaard SQLINE verloop tijd: Verloopt niet
OPER_STATS_SZLINE_COUNT
	Huidig aantal SZLINEs: %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Standaard SZLINE verloop tijd: %d dagen
OPER_STATS_SZLINE_EXPIRE_DAY
	Standaard SZLINE verloop tijd: 1 dag
OPER_STATS_SZLINE_EXPIRE_HOURS
	Standaard SZLINE verloop tijd: %d uur
OPER_STATS_SZLINE_EXPIRE_HOUR
	Standaard SZLINE verloop tijd: 1 uur
OPER_STATS_SZLINE_EXPIRE_MINS
	Standaard SZLINE verloop tijd: %d minuten
OPER_STATS_SZLINE_EXPIRE_MIN
	Standaard SZLINE verloop tijd: 1 minuut
OPER_STATS_SZLINE_EXPIRE_NONE
	Standaard SZLINE verloop tijd: Verloopt niet
OPER_STATS_RESET
	Statistieken opnieuw gezet.

# MODE responses
OPER_STATS_UPLINK_SERVER
	Uplink server: %s
OPER_STATS_UPLINK_CAPAB
	Uplink capab: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Servers found: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE kanaal modes

# UMODE respondes
OPER_UMODE_SYNTAX
	UMODE nick modes
OPER_UMODE_SUCCESS
	Gebruikersmodes van %s veranderd.
OPER_UMODE_CHANGED
	%s heeft je gebruikersmodes veranderd.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE nick flags
OPER_OLINE_SUCCESS
	Operflags %s zijn toegevoegd voor %s.
OPER_OLINE_IRCOP
	Je bent nu een IRC Operator.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES kanaal [ALL]
OPER_CLEARMODES_DONE
	Binaire modes en bans verwijderd van kanaal %s.
OPER_CLEARMODES_ALL_DONE
	Alle modes verwijderd van kanaal %s.


# KICK responses
OPER_KICK_SYNTAX
	KICK kanaal gebruiker reden
	
# ADMIN responses
OPER_SVSNICK_SYNTAX
	SVSNICK nick nieuwe nick 
OPER_SVSNICK_NEWNICK
	De nick %s wordt nu veranderd in %s.
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [nick | toeganslijst]
OPER_ADMIN_SKELETON
	Services is in skelet mode; het ADMIN commando is niet beschikbaar.
OPER_ADMIN_EXISTS
	%s bestaat al op de Services admin lijst.
OPER_ADMIN_REACHED_LIMIT
	Sorry, je kan maar %d Services admins hebben.
OPER_ADMIN_ADDED
	%s toegevoegd aan de Services admin lijst.
OPER_ADMIN_NOT_FOUND
	%s niet gevonden op de Services admin lijst.
OPER_ADMIN_NO_MATCH
	Geen overkeenkomsten op de Services admin lijst.
OPER_ADMIN_DELETED
	%s verwijderd van de Services admin lijst.
OPER_ADMIN_DELETED_ONE
	1 admin verwijderd van de Services admin lijst.
OPER_ADMIN_DELETED_SEVERAL
	%d admins verwijderd van de Services admin lijst.
OPER_ADMIN_LIST_EMPTY
	Services admin lijst is leeg.
OPER_ADMIN_LIST_HEADER
	Services admin lijst:
	  Nr    Nick
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	Services admin lijst is leeggemaakt.
OPER_ADMIN_MOVED
	%s is verplaatst naar de Services admin lijst.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | toegangslijst]
OPER_OPER_SKELETON
	Services is in skelet mode; het OPER commando is onbeschikbaar.
OPER_OPER_EXISTS
	%s bestaat al op de Services operator lijst.
OPER_OPER_REACHED_LIMIT
	Sorry, je kan maar %d Services operators hebben.
OPER_OPER_ADDED
	%s toegevoegd aan Services operator lijst.
OPER_OPER_NOT_FOUND
	%s niet gevonden op Services operator lijst.
OPER_OPER_NO_MATCH
	Geen overeenkomsten op Services operator lijst.
OPER_OPER_DELETED
	%s verwijderd van Services operator lijst.
OPER_OPER_DELETED_ONE
	1 operator verwijderd van Services operator lijst.
OPER_OPER_DELETED_SEVERAL
	%d operators verwijderd van Services operator lijst.
OPER_OPER_LIST_EMPTY
	Services operator lijst is leeg.
OPER_OPER_LIST_HEADER
	Services operator lijst:
	  Nr    Nick
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	Services operator lijst is leeggemaakt.
OPER_OPER_MOVED
	%s is verplaatst naar de Services operator lijst.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+verloop] {mask | toegangslijst} [reden]]
OPER_AKILL_EXISTS
	%s bestaat al op de AKILL lijst.
OPER_AKILL_ALREADY_COVERED
	%s is al gedekt door %s.
OPER_AKILL_REACHED_LIMIT
	Sorry, je kan maar %d AKILLs hebben.
OPER_AKILL_NO_NICK
	Reminder: AKILL masks kunnen geen nicknames bevatten; ben er zeker van dat je geen nick-deel in je mask hebt.
OPER_AKILL_ADDED
	%s toegevoegd aan de AKILL lijst.
OPER_AKILL_CHANGED
	Verlooptijd van %s veranders.
OPER_AKILL_NOT_FOUND
	%s niet gevonden op de AKILL lijst.
OPER_AKILL_NO_MATCH
	Geen overeenkomsten op de AKILL lijst.
OPER_AKILL_DELETED
	%s verwijderd van de AKILL lijst.
OPER_AKILL_DELETED_ONE
	1 AKILL verwijderd van de AKILL lijst.
OPER_AKILL_DELETED_SEVERAL
	%d AKILLs verwijderd van de AKILL lijst.
OPER_AKILL_LIST_EMPTY
	AKILL lijst is leeg.
OPER_AKILL_LIST_HEADER
	Huidige AKILL lijst:
	  Nr    Mask                              Reden
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Huidige AKILL lijst:
# number, mask, set-by, set-time, expires, reason
OPER_AKILL_VIEW_FORMAT
	%3d  %s (door %s op %s; %s)
	      %s
OPER_AKILL_CLEAR
	De AKILL lijst is leeggemaakt.
	
# SGLINE responses
OPER_CHANKILL_SYNTAX
	CHANKILL [+expiry] {#channel} [reason]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+verloop] {mask | toegangslijst}[:reden]]
OPER_SGLINE_UNSUPPORTED
	Sorry, SGLINE is niet beschikbaar op dit netwerk.
OPER_SGLINE_EXISTS
	%s bestaat al op de SGLINE lijst.
OPER_SGLINE_ALREADY_COVERED
	%s is al gedekt door %s.
OPER_SGLINE_REACHED_LIMIT
	Sorry, je kan maar %d SGLINEs hebben.
OPER_SGLINE_ADDED
	%s toegevoegd aan de SGLINE lijst.
OPER_SGLINE_CHANGED
	Verlooptijd van %s veranderd.
OPER_SGLINE_NOT_FOUND
	%s niet gevonden op de SGLINE lijst.
OPER_SGLINE_NO_MATCH
	Geen overeenkomsten op de SGLINE lijst.
OPER_SGLINE_DELETED
	%s verwijderd van de SGLINE lijst.
OPER_SGLINE_DELETED_ONE
	1 SGLINE verwijderd van de SGLINE lijst.
OPER_SGLINE_DELETED_SEVERAL
	%d SGLINEs verwijderd van de SGLINE lijst.
OPER_SGLINE_LIST_EMPTY
	SGLINE lijst is leeg.
OPER_SGLINE_LIST_HEADER
	Huidige SGLINE lijst:
	  Nr    Mask                              Reden
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Huidige SGLINE lijst:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (door %s op %s; %s)
	      %s
OPER_SGLINE_CLEAR
	De SGLINE lijst is leeggemaakt.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+verloop] {mask | toegangslijst} [reden]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	Kanaal SQLINEs worden niet ondersteund door je IRCd, dus kun je ze niet gebruiken.
OPER_SQLINE_EXISTS
	%s bestaat al op de SQLINE lijst.
OPER_SQLINE_ALREADY_COVERED
	%s is al gedekt door %s.
OPER_SQLINE_REACHED_LIMIT
	Sorry, je kan maar %d SQLINEs hebben.
OPER_SQLINE_ADDED
	%s toegevoegd aan de SQLINE lijst.
OPER_SQLINE_CHANGED
	Verlooptijd van %s veranderd.
OPER_SQLINE_NOT_FOUND
	%s niet gevonden op de SQLINE lijst.
OPER_SQLINE_NO_MATCH
	Geen overeenkomsten op de SQLINE lijst.
OPER_SQLINE_DELETED
	%s verwijderd van de SQLINE lijst.
OPER_SQLINE_DELETED_ONE
	1 SQLINE verwijderd van de SQLINE lijst.
OPER_SQLINE_DELETED_SEVERAL
	%d SQLINEs verwijderd van de SQLINE lijst.
OPER_SQLINE_LIST_EMPTY
	SQLINE lijst is leeg.
OPER_SQLINE_LIST_HEADER
	Huidige SQLINE lijst:
	  Nr    Mask                              Reden
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Huidige SQLINE lijst:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (door %s op %s; %s)
	      %s
OPER_SQLINE_CLEAR
	The SQLINE lijst is leeggemaakt.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+verloop] {mask | toegangslijst} [reden]]
OPER_SZLINE_UNSUPPORTED
	Sorry, SZLINE is niet beschikbaar op dit netwerk.
OPER_SZLINE_EXISTS
	%s bestaat al op de SZLINE lijst.
OPER_SZLINE_ALREADY_COVERED
	%s is al gedekt door %s.
OPER_SZLINE_REACHED_LIMIT
	Sorry, je kan maar %d SZLINEs hebben.
OPER_SZLINE_ONLY_IPS
	Herrinering: Je kan alleen IP masks toevoegen aan de SZLINE lijst.
OPER_SZLINE_ADDED
	%s toegevoegd aan de SZLINE lijst.
OPER_SZLINE_CHANGED
	Verlooptijd van %s veranderd.
OPER_SZLINE_NOT_FOUND
	%s niet gevonden op de SZLINE lijst.
OPER_SZLINE_NO_MATCH
	Geen overeenkomsten op de SZLINE lijst.
OPER_SZLINE_DELETED
	%s verwijderd van de SZLINE lijst.
OPER_SZLINE_DELETED_ONE
	1 SZLINE verwijderd van de SZLINE lijst.
OPER_SZLINE_DELETED_SEVERAL
	%d SZLINEs verwijderd van de SZLINE lijst.
OPER_SZLINE_LIST_EMPTY
	SZLINE lijst is leeg.
OPER_SZLINE_LIST_HEADER
	Current SZLINE list:
	  Nr    Mask                              Reden
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Huidige SZLINE lijst:
# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (door %s op %s; %s)
	      %s
OPER_SZLINE_CLEAR
	The SZLINE lijst is leeggemaakt.

# SET responses
OPER_SET_SYNTAX
	SET optie instelling
OPER_SET_IGNORE_ON
	Negeer code zal worden gebruikt.
OPER_SET_IGNORE_OFF
	Negeer code zal niet worden gebruikt.
OPER_SET_IGNORE_ERROR
	Instelling voor negeer code moet ON of OFF zijn.
OPER_SET_READONLY_ON
	Services is nu in alleen-lezen mode.
OPER_SET_READONLY_OFF
	Services is nu in lezen-schrijven mode.
OPER_SET_READONLY_ERROR
	Instelling voor alleen-lezen moet ON of OFF zijn.
OPER_SET_LOGCHAN_ON
	Services rapporteert nu log-berichten naar %s.
OPER_SET_LOGCHAN_OFF
	Services rapporteert geen log-berichten meer naar een kanaal.
OPER_SET_LOGCHAN_ERROR
	Instelling voor LOGCHAN moet ON of OFF zijn en LogChannel moet gedefineerd zijn.
OPER_SET_DEBUG_ON
	Services is nu in debug mode.
OPER_SET_DEBUG_OFF
	Services is nu in non-debug mode.
OPER_SET_DEBUG_LEVEL
	Services is nu in debug mode (niveau %d).
OPER_SET_DEBUG_ERROR
	Instelling voor DEBUG moet ON, OFF, of een positief nummer zijn.
OPER_SET_NOEXPIRE_ON
	Services is nu in geen-verloop mode.
OPER_SET_NOEXPIRE_OFF
	Services is nu in verloop mode.
OPER_SET_NOEXPIRE_ERROR
	Instelling voor NOEXPIRE moet ON of OFF zijn.
OPER_SET_UNKNOWN_OPTION
	Onbekende optie %s.
OPER_SET_SQL_ON
	SQL code wordt gebruikt.
OPER_SET_SQL_OFF
	SQL code wordt niet gebruikt.
OPER_SET_SQL_ERROR
	Instelling voor SQL moet ON of OFF zijn.
OPER_SET_SQL_ERROR_INIT
	Fout tijdens het initialiseren van SQL. Controleer de logs om dit op te lossen.
OPER_SET_SQL_ERROR_DISABLED
	SQL is not configured for this network. Please edit the
	configuration file (services.conf).
OPER_SET_LIST_OPTION_ON
	%s is ingeschakeld
OPER_SET_LIST_OPTION_OFF
	%s is uitgeschakeld

# NOOP responses
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} server
OPER_NOOP_SET
	Alle O:lines van %s zijn verwijderd.
OPER_NOOP_REVOKE
	Alle O:lines van %s zijn hersteld.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE servername [reden]
OPER_JUPE_HOST_ERROR
	Gebruik een geldige servernaam bij het jupen
OPER_JUPE_INVALID_SERVER
	You can not jupe your services server or your uplink server.

# RAW responses
OPER_RAW_SYNTAX
	RAW text

# UPDATE responses
OPER_UPDATING
	Bijwerken databases.
	
# RELOAD responses
OPER_RELOAD
	Services' configuratie bestand is opnieuw geladen.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN niet gedefineerd; kan niet herstarten.  Voer het \2configure\2 script opnieuw uit en hercompileer de Services om het RESTART commando in te schakelen.

# IGNORE respondes
OPER_IGNORE_SYNTAX
	IGNORE {ADD|DEL|LIST|CLEAR} [tijd] [nick | mask]
OPER_IGNORE_VALID_TIME
	Je moet een geldig nummer als tijd invoeren.
OPER_IGNORE_TIME_DONE
	%s zal nu genegeerd worden voor %s.
OPER_IGNORE_PERM_DONE
	%s zal nu permanent genegeerd worden.
OPER_IGNORE_DEL_DONE
	%s zal niet meer genegeerd worden.
OPER_IGNORE_LIST
	Services negeer-lijst:
OPER_IGNORE_LIST_NOMATCH
	Nick %s niet gevonden op de negeer-lijst.
OPER_IGNORE_LIST_EMPTY
	Negeer-lijst is leeg.
OPER_IGNORE_LIST_CLEARED
	Negeer-lijst is leeggemaakt.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	Kan gebruiker %s niet vinden.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Kanalen lijst:
	Naam                 Gebruikers Modes   Topic
OPER_CHANLIST_HEADER_USER
	%s kanalen lijst:
	Naam                 Gebruikers Modes   Topic
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	Einde van kanalen lijst.
	
# USERLIST responses
OPER_USERLIST_HEADER
	Gebruikers lijst:
	Nick                 Mask
OPER_USERLIST_HEADER_CHAN
	%s gebruikers lijst:
	Nick                 Mask
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Einde van gebruikers lijst.

# CACHE responses
OPER_SUPER_ADMIN_ON
	Je bent nu een SuperAdministrator
OPER_SUPER_ADMIN_OFF
	Je bent geen SuperAdministrator meer
OPER_SUPER_ADMIN_SYNTAX
	Instelling voor SuperAdministrator moet ON of OFF zijn (moet ook ingeschakeld zijn in services.conf)
OPER_SUPER_ADMIN_WALL_ON
	%s is nu een SuperAdministrator
OPER_SUPER_ADMIN_WALL_OFF
	%s is geen SuperAdminstrator meer
OPER_SUPER_ADMIN_ONLY
	Alleen SuperAdminstrators kunnen dit commando gebruiken.

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_STAFF_LIST_HEADER
	On Level Nick

# DefCon Messages

OPER_STAFF_FORMAT
	 %c %s  %s
OPER_STAFF_AFORMAT
	 %c %s  %s [%s]

# DefCon Messages

OPER_DEFCON_SYNTAX
	DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	Services zijn in Defcon modus. Probeer later opnieuw.
OPER_DEFCON_NO_CONF
	Het Defcon systeem moet worden ingeschakeld via services.conf
OPER_DEFCON_CHANGED
	Services zijn nu in DEFCON %d
OPER_DEFCON_WALL
	%s heeft het DEFCON niveau veranderd naar %d
DEFCON_GLOBAL
	Het Defcon niveau staat nu op niveau: %d

# Module strings

OPER_MODULE_LOADED
	Module %s geladen
OPER_MODULE_UNLOADED
	Module %s is uitgeladen
OPER_MODULE_LOAD_FAIL
	Laden van de module %s is gefaald
OPER_MODULE_REMOVE_FAIL
	Het verwijderen van module %s is gefaald
OPER_MODULE_NO_UNLOAD
	This module can not be unloaded.
OPER_MODULE_LOAD_SYNTAX
	MODLOAD bestandsnaam
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD bestandsnaam
OPER_MODULE_LIST_HEADER
	Huidige Module lijst:
OPER_MODULE_LIST
	Module: %s [%s] [%s]
OPER_MODULE_LIST_FOOTER
	%d Modules geladen.
OPER_MODULE_INFO_LIST
	Module: %s Versie: %s Auteur: %s Geladen: %s
OPER_MODULE_CMD_LIST
	Stelt het commando: %R%s %s ter beschikking
OPER_MODULE_MSG_LIST
	Stelt de IRCD handler voor: %s ter beschikking
OPER_MODULE_NO_LIST
	Op dit moment zijn er geen modules geladen
OPER_MODULE_NO_INFO
	Er is geen informatie aanwezig voor module %s
OPER_MODULE_INFO_SYNTAX
	MODINFO bestandsnaame

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
MODULE_HELP_HEADER
	De volgende commando's zijn geladen door een module:

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [paramenters]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+verloop] mask limiet reden
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {mask | lijst}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE nr positie
OPER_EXCEPTION_DISABLED
	Sessie limitering is tijdelijk uitgeschakeld.
OPER_EXCEPTION_ALREADY_PRESENT
	Mask %s bestaat l op de uitzonderingenlijst.
OPER_EXCEPTION_TOO_MANY
	Sessie limiet uitzonderingenlijst is vol!
OPER_EXCEPTION_ADDED
	Sessie limiet voor %s veranderd in %d.
OPER_EXCEPTION_MOVED
	Uitzondering voor %s (#%d) verplaatst naar positie %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	#%d bestaat niet op de sessie limiet uitzonderingenlijst.
OPER_EXCEPTION_NOT_FOUND
	%s niet gevonden op de sessie limiet uitzonderingenlijst.
OPER_EXCEPTION_NO_MATCH
	Geen overeenkomsten op de sessie limiet uitzonderingenlijst.
OPER_EXCEPTION_DELETED
	%s verwijderd van de sessie limiet uitzonderingenlijst.
OPER_EXCEPTION_DELETED_ONE
	1 uitzondering verwijderd van de sessie limiet uitzonderingenlijst.
OPER_EXCEPTION_DELETED_SEVERAL
	%d uitzonderingen verwijderd van de sessie limiet uitzonderingenlijst.
OPER_EXCEPTION_LIST_HEADER
	Huidige Sessie Limiet Uitzonderingenlijst:
# mask, limit
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Nr   Limiet Host
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (door %s op %s; %s)
	    Limiet: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Ongeldige sessie limiet. Het moet een geldige integer groter dan of gelijk aan nul en kleiner dan %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Ongeldige hostmask. Alleen echte hostmasks zijn geldig als uitzondering omdat deze niet worden vergeleken met nicks of usernames.
OPER_EXCEPTION_EXISTS
	%s bestaat al op de uitzonderingenlijst.
OPER_EXCEPTION_CHANGED
	Uitzondering voor %s is veranderd in %d.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST limiet | VIEW host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST limiet
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW host
OPER_SESSION_DISABLED
	Session limitering is uitgeschakeld.
OPER_SESSION_INVALID_THRESHOLD
	Ongeldige drempel waarrde. Het moet een geldige integer groter dan 1 zijn.
OPER_SESSION_NOT_FOUND
	%s niet gevonden op de sessie lijst.
OPER_SESSION_LIST_HEADER
	Hosts met tenminste %d sessies:
OPER_SESSION_LIST_COLHEAD
	Sessies  Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d   %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	De host %s heeft momenteel %d sessies met een limiet van %d.

# EXCEPTION help

OPER_HELP_EXCEPTION
	Gebruik: EXCEPTION ADD [+verloop] mask limiet reden
	         EXCEPTION DEL {mask | lijst}
	         EXCEPTION MOVE nr positie
	         EXCEPTION LIST [mask | lijst]
	         EXCEPTION VIEW [mask | lijst]
	
	Staat Services admin toe de lijst van hosts met een specifieke
	sessielimit aan te passen, zodat zeker machines, bijvoorbeeld
	shell servers, meer dan het standaard aantal clients kunnen
	hebben. Zodra een host zijn seesie limiet bereikt, zullen alle
	client die proberen vanaf die host te verbinen worden gekilled
	Voor de gebruiker wordt gekilled wordt deze geinformeerd via
	een /NOTICE van %S, van een bron van hulp aangaande sessie
	limitering. The inhoud van deze notice is een configuratie
	optie.
	
	EXCEPTION ADD voegt de gegeven hostmask toe aan de
	uitzonderingenlijst. Let op: nick!user@host en user@host
	zijn ongeldig! Alleen echte host masks, zoals box.host.dom
	en *.hot.dom, zijn toegestaan omdat sessie limitering geen
	rekening houdt met nicks of usernames. limiet moet een nummer
	groter dan of gelijk aan 0 zijn. Dit bepaalt hoe veel sessies
	deze host tegelijkertijd mag hebben. Een waarde van 0 betekent
	dat de host een ongelimiteerde sessielimiet heeft. Bekijk de
	AKILL help voor details of het formaat van de optionele
	verloop paramenter.
	EXCEPTION DEL verwijdert het gegeven mask van de
	uitzonderingenlijst.
	EXCEPTION MOVE verplaatst uitzondering nr naar positie.
	De tussenliggende uitzondering worden omhoog of omlaag
	geschoven om het gat te vullen.
	EXCEPTION LIST en EXCEPTION VIEW geven alle huidige
	uitzonderingen weer; als het optionele mask is gegeven wordt de
	lijst gelimiteerd tot uitzondering overeenkomend het het
	gegeven mask. Het verschil is dat EXCEPTION VIEW meer
	informatie geeft: de naam van degene die de uitzondering
	aangemaakt heeft, de bijbehorende sessielimiet, reden, hostmask
	en de verloop datum en tijd.
	
	Let op: een verbindinde client zal de eerste uitzondering die
	met zijn host overeenkomt "gebruiken". Grote uitzonderingen-
	lijsten en breed overeenkomende uitzondering-masks zullen
	zeer waarschijnlijk de prestaties van de services verminderen.

OPER_HELP_SESSION
	Gebruik: SESSION LIST drempel
	         SESSION VIEW host
	
	Stelt Services admins instaat de sessie lijst te bekijken.
	
	SESSION LIST geeft hosts weer met tenminste drempel
	sessies weer. Deze drempel moet een nummer groter dan 1 zijn.
	Dit is om te verkomen dat je perongeluk het grote aantal
	hosts met maar 1 sessie weergeeft.
	SESSION VIEW geeft gedetailleerde informatie weer over een
	specifieke host, inclusief het huidige aantal sessies en de
	sessie limiet. De host waarde mag geen wildcards bevatten.
	
	Bekijk de EXCEPTION help voor meer informatie over sessie
	limiteringen en hoe sessie limieten in te stellen voor
	specifieken hosts en groepen daarvan.

###########################################################################
#
# News system messages
#
###########################################################################

OPER_HELP_STAFF
	Syntax: STAFF

	Geeft de nicks van alle Services Staff samen met
	hun level en on-line status weer.

OPER_HELP_DEFCON
	Syntax: DEFCON [1|2|3|4|5]

	Het defcon systeem kan gebruikt worden om een vooraf bepaalde
	set van restricties op de services toe te passen tijdens een
	aanval op het netwerk.

OPER_HELP_DEFCON_NO_NEW_CHANNELS
	* Geen nieuwe channel registratie's
OPER_HELP_DEFCON_NO_NEW_NICKS
	* Geen nieuwe nick registratie's
OPER_HELP_DEFCON_NO_MLOCK_CHANGE
	* Geen MLOCK veranderingen
OPER_HELP_DEFCON_FORCE_CHAN_MODES
	* Forceer Kanaal Modes (%s) op alle kanalen
OPER_HELP_DEFCON_REDUCE_SESSION
	* Gebruik de beperkte sessie limiet van %d
OPER_HELP_DEFCON_NO_NEW_CLIENTS
	* Kill elke nieuwe client die connecteerd
OPER_HELP_DEFCON_OPER_ONLY
	* Negeer elke non-oper met een boodschap
OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* Negeer non-opers
OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* AKILL elke nieuwe client die connecteerd
OPER_HELP_DEFCON_NO_NEW_MEMOS
	* Geen nieuwe memo's verzenden
OPER_HELP_CHANKILL
	Syntax: CHANKILL [+expiry] channel reason

	Plaatst een AKILL voor elke nick op een gegeven kanaal. Het
	gebruikt de complete en echte ident@host voor elke nick,
	en forceerd dan een AKILL.

###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Login Nieuws - %s] %s
NEWS_OPER_TEXT
	[Oper Nieuws - %s] %s
NEWS_RANDOM_TEXT
	[Willekeurig Nieuws - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD|DEL|LIST} [text|nr]
NEWS_LOGON_LIST_HEADER
	Login nieuwsberichten:
NEWS_LOGON_LIST_ENTRY
	%5d (%s door %s)
	    %s
NEWS_LOGON_LIST_NONE
	Er is geen login nieuws.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD text
NEWS_LOGON_ADD_FULL
	Nieuws lijst is vol!
NEWS_LOGON_ADDED
	Nieuw login nieuws toegevoegd (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {nr | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Login nieuwsbericht #%d niet gevonden!
NEWS_LOGON_DELETED
	Login nieuwsbericht #%d verwijderd.
NEWS_LOGON_DEL_NONE
	Geen login nieuwsberichten om te verwijderen!
NEWS_LOGON_DELETED_ALL
	Alle login nieuwsberichten zijn verwijderd.

NEWS_OPER_SYNTAX
	Gebruik: OPERNEWS {ADD|DEL|LIST} [text|nr]
NEWS_OPER_LIST_HEADER
	Oper nieuwsberichten:
NEWS_OPER_LIST_ENTRY
	%5d (%s door %s)
	    %s
NEWS_OPER_LIST_NONE
	Er is geen opernieuws.
NEWS_OPER_ADD_SYNTAX
	Gebruik: OPERNEWS ADD text
NEWS_OPER_ADD_FULL
	Nieuwslijst is vol!
NEWS_OPER_ADDED
	Nieuw oper nieuwsbericht toegevoegd (#%d).
NEWS_OPER_DEL_SYNTAX
	Gebruik: OPERNEWS DEL {nr | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Oper nieuwsbericht #%d niet gevonden!
NEWS_OPER_DELETED
	Oper nieuwsbericht #%d verwijderd.
NEWS_OPER_DEL_NONE
	Geen oper nieuwsberichten om te verwijderen!
NEWS_OPER_DELETED_ALL
	Alle oper nieuwsberichten zijn verwijderd.

NEWS_RANDOM_SYNTAX
	RANDOMNEWS {ADD|DEL|LIST} [text|nr]
NEWS_RANDOM_LIST_HEADER
	Willekeurige nieuwsberichten:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s door %s)
	    %s
NEWS_RANDOM_LIST_NONE
	Er is geen willekeurig nieuws.
NEWS_RANDOM_ADD_SYNTAX
	RANDOMNEWS ADD text
NEWS_RANDOM_ADD_FULL
	Nieuwslijst is vol!
NEWS_RANDOM_ADDED
	Nieuw willekeurig nieuwsbericht toegevoegd (#%d).
NEWS_RANDOM_DEL_SYNTAX
	RANDOMNEWS DEL {nr | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Willekeurig nieuwsbericht #%d niet gevonden!
NEWS_RANDOM_DELETED
	Willekeurig nieuwsbericht #%d verwijderd.
NEWS_RANDOM_DEL_NONE
	Geen willekeurige nieuwsberichten om te verwijderen!
NEWS_RANDOM_DELETED_ALL
	Alle willekeurige nieuwsberichten zijn verwijderd.

NEWS_HELP_LOGON
	Gebruik: LOGONNEWS ADD text
	         LOGONNEWS DEL {nr | ALL}
	         LOGONNEWS LIST
	
	Wijzigt de lijst met login nieuwsberichten of geeft deze weer.
	Wanneer een gebruiker met het netwerk verbindt, worden deze
	berichten naar hem/haar verzonden.  (Echter, er zullen nooit
	meer dan %s berichten worden verstuurd om het flooden van de
	gebruiker te voorkomen. Als er meer nieuwsberichten
	zijn worden alleen de meest recente vestuurd.)
	NewsCount kan geconfigureerd worden in services.conf.
	
	LOGONNEWS may only be used by Services admins. 

NEWS_HELP_OPER
	Gebruik: OPERNEWS ADD text
	         OPERNEWS DEL {nr | ALL}
	         OPERNEWS LIST
	
	Wijzigt de lijst met oper nieuwsberichten of geeft deze weer.
	Wanneer een gebruiker opert (dmv het /OPER commando), worden
	deze berichtennaar hem/haar verzonden.  (Echter, er zullen
	nooit meer dan %s berichten worden verstuurd om het flooden
	van de gebruiker te voorkomen. Als er meer nieuwsberichten 
	zijn worden alleen de meest recente vestuurd.)
	NewsCount kan geconfigureerd worden in services.conf.
	
	OPERNEWS may only be used by Services admins. 

NEWS_HELP_RANDOM
	Syntax: RANDOMNEWS ADD text
	        RANDOMNEWS DEL {num | ALL}
	        RANDOMNEWS LIST
	
	Wijzigt de lijst mer willekeurige nieuwsberichten of geeft
	deze weer. Wanneer een gebruiker met het netwerk verbindt,
	wordt er een (en niet meer dan een) van deze willekeurige
	nieuwsberichten willekeurig uitgekozen en naar hem/haar
	verstuurd.
	
	RANDOMNEWS may only be used by Services admins. 

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S is een service die informatie over Services geeft.
	Help onderwerpen zijn toegankelijk via de HELP commando's
	of de andere Service clients:
	
	%R%s HELP
	     voor informatie over nickname registratie

	%R%s HELP
	     voor informatie over kanaal instellingen en registratie
	     
	%R%s HELP
	     voor informatie over berichten naar offline gebruikers
	     sturen

HELP_HELP_BOT

	%R%s HELP
	     voor informatie over een bot in je kanaal instellen

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
	    REGISTER   Registreer een nickname
NICK_HELP_CMD_GROUP
	    GROUP      Bij een groep aanmelden
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY   Jezelf identificeren met je wachtwoord
NICK_HELP_CMD_ACCESS
	    ACCESS     De lijst met toegestane adressen wijzigen
NICK_HELP_CMD_SET
	    SET        Opties instellen, inclusief kill bescherming
NICK_HELP_CMD_SASET
	    SASET      Set SET-options on another nickname
NICK_HELP_CMD_DROP
	    DROP       De registratie van je nickname annuleren
NICK_HELP_CMD_RECOVER
	    RECOVER    Een andere gebruiker killen die je nick gebruikt
NICK_HELP_CMD_RELEASE
	    RELEASE    Je nick terugnemen nadat je RECOVER gebruikt hebt
NICK_HELP_CMD_SENDPASS
	    SENDPASS   Je wachtwoord vergeten? Probeer dit
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
	    GETPASS    Bekijk het wachtwoord voor een nick
	               (alleen als encryptie is uitgeschakeld)
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Matches and returns all users that registered using given email
NICK_HELP_CMD_FORBID
	    FORBID     Voorkom dat een nick gebruikt wordt
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Suspend a given nick
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Unsuspend a given nick

NICK_HELP
	%S stelt je in staat een nickname te "registreren", en
	te voorkomen dat anderen hem gebruiken. De volgende commando's
	kunnen gebruikt worden voor registratie en onderhoud van
	nicknames; om ze te gebruiken, type %R%S commando.
	Voor meer informatie over een specifiek commando, type
	%R%S HELP commando.
	
NICK_HELP_FOOTER
	OPMERKING: Deze service is bedoeld om IRC gebruikers een weg
	te bieden om te verzekeren dat hun identiteit niet in gevaar komt.
	Het is NIET bedoeld om het "stelen" van nicknames of andere
	kwaadwillige acties te helpen. Misbruik van %S zal resulteren in,
	minimaal, verlies van de misbruikte nick(s).
	
NICK_HELP_EXPIRES
	
	Nicknames die niet meer gebruikt worden zullen na een tijd
	automatisch verlopen, oftewel: ze zullen verwijderd worden na
	%d dagen inactiviteit.

NICK_HELP_REGISTER
	Gebruik: REGISTER wachtwoord [email]
	
	Registreert je nickname in de %S database. Wanneer je nick
	geregistreerd is kan je de SET en ACCESS commando's
	gebruiken om je nick instellingen te veranderen.
	Onthoud het wachtwoord dat je gebruikt bij het registreren,
	je hebt het nodig als je je nick instellingen later wilt
	veranderen. (Let erop dat hoofdletters uitmaken!  ANOPE,
	Anope, en anope zijn allemaal andere wachtwoorden!)
	
	Wachtwoordrichtlijnen:
	
	Wachtwoorden moeten niet makkelijk te raden zijn. Bijvoorbeeld,
	je echte naam als wachtwoord gebruiken is een slecht idee. Je
	nick als een wachtwoord gebruiken is een veel slechter idee ;)
	en in feite, %S staat het niet toe. Trouwens, korte wachtwoorden
	zijn kwetsbaar voor zoekacties,	dus je moet een wachtwoord
	kiezen dat minstens 5 tekens lang is. Als laatste, de spatie
	kan niet gebruikt worden in wachtwoorden.
	
	De email parameter is optioneel en stelt onmiddelijk de email
	voor je nick in. Het kan echter vereist zijn voor bepaalde
	netwerken.
	Je privacy wordt gerespecteerd; deze email zal nooit aan derden
	gegeven worden.
	
	Dit commando maakt ook een nieuwe groep voor je nickname, die
	toestaat andere nicks onder dezelfde configuratie, dezelfde
	memos en dezelfde kanaalrechten te delen. Voor meer informatie
	over deze functie, type %R%S HELP GROUP.
	
NICK_HELP_GROUP
	Gebruik: GROUP doel wachtwoord
	
	Dit commando zorgt ervoor dat je huidige nickname zich bij de
	groep van doel's nick aanmeldt. wachtwoord is het
	wachtwoord van het doel.
	
	Bij een groep aanmelden stelt je in staat om je configuratie,
	memos en kanaalrechten met alle nicknames in je groep te delen,
	en veel meer!
	
	Een groep bestaat zo lang als het nuttig is. Dit betekent dat
	zelfs als een nick in de groep geannuleerd je niet de gedeelde
	dingen die hierboven beschreven zijn verliest, zo lang als er
	minstens een nick overblijft in de groep.
	
	Je kan dit commando zelfs gebruiken als je nog niet
	geregistreerd hebt. Als je nick al geregistreerd is moet je je
	eerst identificeren voordat je dit commando gebruikt. Type
	%R%S HELP IDENTIFY voor meer informatie. Dit laatste is
	misschien niet mogelijk op je IRC netwerk.
	
	Het is aanbevolen om dit commando te gebruiken met een niet-
	geregistreerde nick omdat het automatisch geregistreerd wordt
	als je dit commando gebruikt. Je mag het alleen met een geregistreerde
	nick gebruiken (om je groep te veranderen) als de netwerkbeheerders
	het toestaan.
	
	Je kan maar in een groep tegelijk zijn. Groepen samenvoegen is
	niet mogelijk.
	
	Opmerking: alle nicks in een groep hebben hetzelfde wachtwoord.

NICK_HELP_IDENTIFY
	Gebruik: IDENTIFY wachtwoord
	
	Vertelt %S dat je echt de eigenaar van deze nick bent. Veel
	commandos vereisen dat je jezelf identificeert met dit commando
	voordat je ze gebruikt. Het wachtwoord moet hetzelfde zijn als
	die je gestuurd hebt met het REGISTER commando.
	
NICK_HELP_UPDATE
	Gebruik: UPDATE
	
	Werkt je huidige status bij, het controleert bijv. op nieuwe
	memo's, stelt de benodigde kanaalmodes in (ModeonID), en
	werkt je vhost en gebruikersflags bij (laatstgezien, etc).
	
NICK_HELP_LOGOUT
	Gebruik: LOGOUT
	
	Dit draait het effect van het IDENTIFY commando om: het
	zorgt er voor dat je niet meer herkend word als de echte
	eigenaar van de nick. Merk op dat je niet gevraagd wordt om
	je opnieuw te identificeren.

NICK_HELP_DROP
	Gebruik: DROP [nick]
	
	Verwijdert je nickname uit de %S database. Een nick die
	verwijderd is kan door iedereen weer geregistreerd worden.
	
	Je kan een nick binnen een groep verwijderen door de nick
	parameter toe te voegen.
	
	Om dit commando te kunnen gebruiken moet je je eerst identificeren
	met je wachtwoord. (%R%S HELP IDENTIFY voor meer informatie).

NICK_HELP_ACCESS
	Gebruik: ACCESS ADD mask
	         ACCESS DEL mask
	         ACCESS LIST
	
	Verandert of geeft de toegangslijst weer van je nick. Dit is
	de lijst met adressen waar vanaf je automatisch toegestaan
	wordt door %S om je nick te gebruiken. Als je je nick vanaf
	een ander adres wilt gebruiken moet je een IDENTIFY commando
	sturen om %S je te laten herkennen.
	
	Voorbeelden:
	
	    ACCESS ADD iemand@*.bepeg.com
	        Staat toegang toe voor gebruiker iemand vanuit elke machine in
	        het bepeg.com domein..
	
	    ACCESS DEL anyone@*.bepeg.com
	        Keert het vorige commando om.
	
	    ACCESS LIST
	        Geeft de huidige toegangslijst weer.

NICK_HELP_SET
	Gebruik: SET optie parameters
	
	Stelt verscheidene nick opties in.  option kan zijn:
	
	    DISPLAY    Stel de weergave van je groep in Services in
	    PASSWORD   Verander het wachtwoord voor je nick
	    LANGUAGE   Selecteer de taal die Services zal gebruiken
	                   voor het versturen van berichten naar jou
	    URL        Associeer een URL met je nick
	    EMAIL      Associeer een E-mail adres met je nick
	    ICQ        Associeer een ICQ nummer met je nick
	    GREET      Associeer een begroeting met je nick
	    KILL       Zet bescherming aan of uit
	    SECURE     Zet nickname beveiliging aan of uit
	    PRIVATE    Zorgt ervoor dat je nick niet verschijnt in een
	                   %R%S LIST
	    HIDE       Verberg verschillende soorten nick informatie
	    MSG        Verander de communicatiemanier van Services
	    AUTOOP     Should services op you automatically.    
	
	Om dit commando te gebruiken moet je je eerst identificeren
	met je wachtwoord (%R%S HELP IDENTIFY voor meer
	informatie).
	
	Type %R%S HELP SET optie voor meer informatie over een
	specifieke optie.

NICK_HELP_SET_DISPLAY
	Gebruik: SET DISPLAY nieuwe-weergave
	
	Verandert de weergegeven nick van je nick-groep in Services.
	De nieuwe weergave MOET een nick in je groep zijn.

NICK_HELP_SET_PASSWORD
	Gebruik: SET PASSWORD nieuw-wachtwoord
	
	Verandert het wachtwoord dat je gebruik om je te identificeren
	als de eigenaar van je nick.

NICK_HELP_SET_LANGUAGE
	Gebruik: SET LANGUAGE nummer
	
	Verandert de taal die Services gebruikt wanneer deze een
	bericht naar jou stuurt (bijvoorbeeld wanneer deze antwoord
	op een commando dat je hebt verzonden). nummer moet gekozen
	worden uit de volgende lijst van ondersteunde talen:

NICK_HELP_SET_URL
	Gebruik: SET URL url
	
	Associeert de gegeven URL met je nick. Deze URL zal worden
	weergegeven wanneer iemand informatie over jouw nick
	opvraagt met het INFO commando.

NICK_HELP_SET_EMAIL
	Gebruik: SET EMAIL adres
	
	Associeert het gegeven e-mail adres met jouw nick. Dit adres
	zal worden weergegeven wanneer iemand informatie over je nick
	opvraagt met het INFO commando.
	
NICK_HELP_SET_ICQ
	Gebruik: SET ICQ nummer
	
	Associeert het gegeven ICQ nummer met je nick. Dit nummer
	zal worden weergegeven wanneer iemand informatie opvraagt
	over je nick met het INFO commando.
	
NICK_HELP_SET_GREET
	Gebruik: SET GREET bericht
	
	Het opgegeven bericht zal als groet worden weergegeven in
	elk kanaal dat je joint waarbij de GREET optie aanstaat,
	indien je toegangsniveau hoog genoeg is hiervoor.

NICK_HELP_SET_KILL
	Gebruik: SET KILL {ON | QUICK | IMMED | OFF}
	
	Zet de automatiche beschermingsoptie voor je nick aan of uit.
	Met bescherming aan, krijgt elke gebruik met jouw nick een
	minuut om zich te identificeren of om zijn/haar nick te
	veranderen. Is geen van beiden gebeurd in die minuut, dan zal
	%S een nick-verandering forceren.
	
	Als je QUICK kiest, zal de gebruiker maar 20 seconden hebben
	om zijn/haar nick te veranderen in plaats van de gebruikelijke
	60. Als je IMMED kiest, zal de nick meteen worden aangepaast
	zonder enige waarschuwing vooraf of kans om zijn/haar nick
	te wijzigen; gebruik deze optie alleen wanneer noodzakelijk.
	Deze optie kan ook uitgeschakeld zijn door de netwerk
	administrators.

NICK_HELP_SET_SECURE
	Gebruik: SET SECURE {ON | OFF}
	
	Zet %S's veiligheidsopties aan of uit voor jouw nick.
	Met SECURE aan moet je altijd een wachtwoord invullen
	voordat je wordt herkend als de eigenaar van de nick,
	ook als je adres op de toegangslijst voor deze nick
	staat. Echter, als je op de toegangslijst staat zal
	%S je niet automatisch killen, hoe de KILL optie ook
	ingesteld is.

NICK_HELP_SET_PRIVATE
	Gebruik: SET PRIVATE {ON | OFF}
	
	Zet de prive optie van %S aan of uit voor jouw nick.
	Met PRIVATE aan zal je nick niet verschijnen in nicklijsten
	gegenereerd met %S's LIST commando. (Echter,
	iedereen die je nick weet kan nog steeds informatie krijgen
	mbv het INFO commando.

NICK_HELP_SET_HIDE
	Gebruik: SET HIDE {EMAIL | MASK | QUIT} {ON | OFF}
	
	Deze optie stelt je in staat verschillende informatie te
	verbergen wanneer iemand een %S INFO op jouw nick
	doet. Je kan je e-mail adres (EMAIL), laatst bekende
	gebruikter@host mask (MASK) en laatste quit bericht (QUIT)
	verbergen. De tweede paramenter geeft aan of de informatie
	moet worden weergegeven (OFF) of verborgen (ON).

NICK_HELP_SET_MSG
	Gebruik: SET MSG {ON | OFF}
	
	Deze optie stelt je in staat de manier te kiezen waaop Services
	met je communiceren. Met MSG aan zullen de Services gewone
	berichten gebruiken, anders zullen ze notices gebruiken.

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
	Gebruik: RECOVER nick [wachtwoord]
	
	Deze optie stelt je instaat je nick terug te nemen wanneer
	iemand anders deze heeft. Deze optie doet hetzelfde als
	%S automatisch doet als iemand een nick probeert te
	gebruiken die beschermd is door de KILL optie.
	
	Wanneer je dit commando geeft zal %S een nep-gebruiker
	online brengen met dezelfde nickname als de gebruiker waarvan
	je de nick terugneemt. Dit zorgt ervoor dat de IRC server de
	andere gebruiker zijn/haar verbinding sluiten. Deze nep-
	gebruiker zal voor %s online blijven om ervoor te zorgen
	dat de gebruiker niet meteen opnieuw verbindt met dezelfde
	nick. Na deze tijd kan je je nick opnieuw claimen, of je kan
	tussentijds je nick terugkrijgen mbv het RELEASE commando.
	(Bekijk %R%S HELP RELEASE voor meer informatie hierover)	
	
	Om het RECOVER commando op een nick te mogen gebruiken moet
	je huidige adres zoals deze in /WHOIS staat op de toegangslijst
	van deze nick zijn, je geindentificeerd zijn in de group van
	die nick, of het correcte wachtwoord opgegeven zijn.

NICK_HELP_RELEASE
	Gebruik: RELEASE nickname [wachtwoord]
	
	Geeft %S de opdracht elke nep-gebruiker met jouw nick te
	verwijderen. Deze nep-gebruiker wordt aangemaakt na een
	automatische kill-bescherming of na gebruik van het RECOVER
	commando. Standaard blijft zo'n nep-gebruiker %s staan;
	dit commando zorgt ervoor dat deze eerder wegghaald wordt.
	
	Om het RELEASE commando op een nick te mogen gebruiken moet
	je huidige adres zoals deze in /WHOIS staat op de toegangslijst
	van deze nick zijn, je geindentificeerd zijn in de group van
	die nick, of het correcte wachtwoord opgegeven zijn.


NICK_HELP_GHOST
	Gebruik: GHOST nick [wachtwoord]
	
	Be-eindigd een "ghost" IRC sessie met jouw nick. Een "ghost"
	sessie is een sessie die niet feitelijk verbonden is, maar
	waarvan de IRC server denkt dat deze nog steeds online is
	voor om het even welke reden. Dit gebeurt vaak als je
	computer crasht of als je internet of modem verbinding
	ermee ophoudt wanneer je op IRC zit.
	
	Om het GHOST commando op een nick te mogen gebruiken moet
	je huidige adres zoals deze in /WHOIS staat op de toegangslijst
	van deze nick zijn, je geindentificeerd zijn in de group van
	die nick, of het correcte wachtwoord opgegeven zijn.

NICK_HELP_INFO
	Gebruik: INFO nick [ALL]
	
	Geeft informatie weer over de gegeven nick, zoals de laatste
	eigenaar van de nick, het laatst beekende adres, wanneer deze
	voor het laatst gezien is, en de ingestelde optie. Als je
	voor de gegeven nick geidentificeerd bent en het ALL
	commando is gegeven zal je alle informatie te zien krijgen,
	om het verborgen is of niet.

NICK_HELP_LIST
	Gebruik: LIST trefbeeld
	
	Geeft alle geregistreerde nicks weer die overeenkomen met
	het gegeven trefbeeld, in nick!gebruiker@host formaat.
	Nicks met de PRIVATE optie aan worden niet weergegeven.
	
	Voorbeelden:
	
	    LIST *!joeuser@foo.com
	        Geeft alle nicks weer van joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Geeft alle geregistreerde nicks weer met Bot in
	        hun naam (hoofdlettergevoelig).
	
	    LIST *!*@*.bar.org
	        Geeft alle nicks weer van het bar.org domein

NICK_HELP_ALIST
	Gebruik: ALIST [niveau]
	
	Geeft alle kanalen weer waarop je toegang hebt. Optioneel
	kun je een niveau opgeven in XOP of ACCESS formaat. De
	resulterende lijst zal alleen kanalen bevatten waar je het
	gegeven toegangsniveau hebt.
	
	Voorbeelden:
	    ALIST Founder
	           Geeft alle kanalen weer waar je stichter-
	           toegang hebt.

	    ALIST AOP
	           Geeft alle kanalen weer waar je AOP-toegang of
	           meer hebt.
	
	    ALIST 10
	           Geeft alle kanalen weer waar je toegangsniveau
	           10 of hoger hebt.
	
	Kanalen met de NOEXPIRE optie aan zullen voorafgegaan
	worden door een uitroepteken.

NICK_HELP_GLIST
	Gebruik: GLIST
	
	Geeft alle nicks in je groep weer.

NICK_HELP_STATUS
	Gebruik: STATUS nick...
	
	Geef terug of de gebruiker van de gegeven nick wordt herkend
	als de eigenaar van die nick. Het antwoord is in dit formaat:
	
	    nick status-code
	
	waar nick de nick is die verstuurd is met he commando, en
	status-code een van de volgende mogelijkheden is:
	
	    0 - gebruiker niet online of nick niet geregistreerd
	    1 - gebruiker wordt niet herkend als de eigenaar van de nick
	    2 - gebruiker wordt alleen via de toegangslijst herkend als eigenaar
	    3 - gebruiker wordt door wachtwoordidentificatie herkend als eigenaar
	
	Er kunnen maximaal zestien nicks per keer worden verstuurd;
	de rest wordt genegeerd. If no nickname is given, your status
	will be returned.

NICK_HELP_SENDPASS
	Gebruik: SENDPASS nick
	
	Verstuur het wachtwoord van de gegeven nick naar het e-mail
	adres wat bij de nick hoort. Dit commando is erg handig om
	verloren wachtwoorden terug te krijgen.
	
	Kan gelimiteerd zijn tot IRC operators op sommige netwerken.
	
	Dit commando is alleen beschikbaar als encryptie is uitgeschakeld.

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
	Services admins kunnen ook de registratie van elke nick
	opheffen zonder zich voor die nick te identificeren, en
	mogen de toegangslijst voor elke nick bekijken.
	(%R%S ACCESS LIST nick)

NICK_SERVADMIN_HELP_LOGOUT
	Gebruik: LOGOUT [nick [REVALIDATE]]
	
	Zonder een paramenter keert dit command het effect van het
	IDENTIFY commando om; je wordt dus niet meer herkend als
	de echte eigenaar van je huidige nick. Merk echter op dat
	je niet wordt gevraagd jezelf opnieuw te identificeren.

	Met een paramenter doet dit commando hetzelfde voor de
	gegeven nick. Als je ook REVALIDATE meegeeft zullen de
	Services de gegeven nick vragen opnieuw te identificeren.
	Het gebruik hiervan is gelimiteerd tot Services admins.

NICK_SERVADMIN_HELP_DROP
	Gebruik: DROP [nick]
	
	Zonder een paramenter wordt je eigen nick uit de %S
	database verwijderd.
	
	Met een paramenter wordt de gegeven nick uit de database
	verwijderd. Je kan de registratie van elke nick in je groep
	ongedaan maken zonder speciale privileges. Het verwijderen
	van elke nick is gelimiteerd tot Services admins.

NICK_SERVADMIN_HELP_INFO
	
	Services admins mogen het ALL parameter met elke nick gebruiken.

NICK_SERVADMIN_HELP_LIST
	Gebruik: trefbeeld [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	Geeft alle geregistreerde nicknames weer die overeen komen
	met het gegeven trefbeeld, in nick!gebruiker@host formaat.
	Nicks met de PRIVATE optie in worden alleen weergegeven
	aan Services admins. Nicks met de NOEXPIRE optie aan zullen
	een ! voor de naam hebben als een Services admin de lijst
	bekijkt.
	
	If the FORBIDDEN, SUSPENDED, NOEXPIRE or UNCONFIRMED options are given, only
	nicks which, respectively, are FORBIDDEN, SUSPENDED, UNCONFIRMED or have the
	NOEXPIRE flag set will be displayed. If multiple options are
	given, all nicks matching at least one option will be displayed.
	These options are limited to Services admins.  
	
	Voorbeelden:
	
	    LIST *!joeuser@foo.com
	        Geeft alle nicks weer van joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Geeft alle geregistreerde nicks weer met Bot in
	        hun naam (hoofdlettergevoelig).
	
	    LIST * NOEXPIRE
	        Geeft alle geregistreerde nicks weer die niet
	        zullen verlopen.

NICK_SERVADMIN_HELP_ALIST
	Gebruik: ALIST [nick] [niveau]
	
	Zonder paramenters geeft dit elk kanaal weer waar je toegang
	hebt. Met een paramenter geeft het de kanalen weer waar
	nick toegang heeft. Met twee paramenters geeft het de
	kanalen weer waar nick een toegangsniveau van tenminste
	niveau heeft.
	
	Dit gebruik is gelimiteerd tot Services adminstrators.

NICK_SERVADMIN_HELP_GLIST
	Gebruik: GLIST [nick]
	
	Zonder een paramenter worden alle nicks weergegeven die
	zich in jouw groep bevinden.
	
	Indied een paramenter gegeven is worden alle nicks
	weergegeven die zich in de groep van de gegeven nick
	bevinden.
	Dit gebruik is gelimiteerd tot Services admins.

NICK_SERVADMIN_HELP_GETPASS
	Gebruik: GETPASS nick
	
	Geeft het wachtwoord van de opgegeven nick weer. Merk op
	dat wanneer je dit commando gebruikt er een bericht met de
	persoon die het commando heeft uitgevoerd en de nick waarop
	het is gebruikt zal worden gelogd en verstuurd zal worden
	als WALLOPS/GLOBOPS.
	
	Dit commando is onbeschikbaar wanneer encryptie is ingeschakeld.

NICK_SERVADMIN_HELP_GETEMAIL
	Syntax: GETEMAIL user@emailhost
	
	Geeft de bijpassende nicks weer dat gegeven email gebruikt.
	Merk op dat er geen wildcards mogen worden gebruikt in
	de user en de emailhost. Elke keer als dit commando wordt
	gebruikt wordt er een bericht gelogged met de persoon
	die dit commando uitvoerde samen met de emailhost waarop
	het werd gebruikt. 

NICK_SERVADMIN_HELP_FORBID
	Gebruik: FORBID nick [reden]
	
	Zorgt ervoor dat de gegeven nick niet kan worden geregistreerd
	of gebruikt door iedereen. Dit kan ongedaan gemaakt worden
	door de registratie van de nick ongedaan te maken (DROP).
	
	Op sommige netwerken is een reden verplicht.


###########################################################################
#
# ChanServ help messages
#
###########################################################################

NICK_SERVADMIN_HELP_SUSPEND
	Gebruik: SUSPEND nick reden / UNSUSPEND nick

	Stelt een nick tijdelijk buiten werking. Een nick die buiten werking
	is gesteld kan niet worden gebruikt: identificatie wordt uitgeschakeld.

###########################################################################
#
# ChanServ help messages
#
###########################################################################

NICK_SERVADMIN_HELP_UNSUSPEND
	Syntax: UNSUSPEND nickname

	UNSUSPENDS a nickname from being used.

###########################################################################
#
# ChanServ help messages
#
###########################################################################
CHAN_HELP_CMD_GETPASS
	    GETPASS    Haal het stichter-wachtwoord voor een kanaal op
CHAN_HELP_CMD_FORBID
	    FORBID     Verbied het gebruik van een kanaal
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    Stel een kanaal tijdelijk buiten werking terwijl de
	               kanaal-data en -instellingen bewaard blijven.
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Stelt een tijdelijk buiten werking gesteld kanaal weer
	               in werking
CHAN_HELP_CMD_STATUS
	    STATUS     Geeft het huidige toegangsniveau van een gebruiker
	               op een kanaal weer
CHAN_HELP_CMD_REGISTER
	    REGISTER   Registreer een kanaal
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   Identificeer jezelf met je wachtwoord
CHAN_HELP_CMD_SET
	    SET        Stel kanaal opties en informatie in
CHAN_HELP_CMD_AOP
	    AOP        Wijzig de lijst van AOP's
CHAN_HELP_CMD_SOP
	    SOP        Wijzig de lijst van SOP's
CHAN_HELP_CMD_ACCESS
	    ACCESS     Wijzig de lijst van toegestane gebruikers
CHAN_HELP_CMD_LEVELS
	    LEVELS     Verander de betekenissen van toegangsniveaus
CHAN_HELP_CMD_AKICK
	    AKICK      Beheer de AutoKick lijst
CHAN_HELP_CMD_DROP
	    DROP       Annuleer een kanaalregistratie
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Wachtwoord vergeten?
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
	%S stelt je in staat om kanalen te registreren en er
	verschillende opties van in te stellen.  %S kan vaak
	voorkomen dat boosaardig gebruikers een kanaal "overnemen"
	door te limiteren wie er op het kanaal operator mag zijn.
	Beschikbare commando's staan hier beneden; om ze te gebruiken
	type je %R%S commando. Voor meer informatie over een
	specifiek commando type je %R%S HELP commando.
	 
	
CHAN_HELP_EXPIRES
	
	Onthoud dat elk kanaal wat %d dagen ongebruikt is (dwz dat
	er geen gebruiker die op de toegangslijst staat in die periode
	het kanaal binnen is gekomen) automatisch zal worden verwijderd.

CHAN_HELP_REGISTER
	Gebruik: REGISTER kanaal wachtwoord beschrijving
	
	Registreert een kanaal in de %S database. Om dit commando te
	gebruiken moet je eerst een operator op het kanaal zijn dat
	je wil registreren. Het wachtwoord wordt gebruikt met het
	IDENTIFY commando dat anderen in staat stelt om later
	wijzigingen aan de kanaalinstellingen door te voeren. Het
	laatste paramenter, die moet worden opgegeven, is een
	algemene beschrijving van het doel van het kanaal.
	
	Wanneer je een kanaal registreert wordt je de "stichter"
	van het kanaal. De kanaalstichter mag alle instellingen
	van het kanaal veranderen. %S zal ook de stichter automatisch
	stichterrechten geven wanneer deze het kanaal binnenkomt.
	Zie het ACCESS commando (%R%S HELP ACCESS) voor meer
	informatie over het geven van rechten aan andere kanaalgebruikers.
	
	OPMERKING: Om een kanaal te registreren moet je je nick
	eerst geregistreerd hebben. Als je dit niet gedaan hebt
	bekijk dan %R%s HELP voor informatie over hoe je
	dit kunt doen.

CHAN_HELP_IDENTIFY
	Gebruik: IDENTIFY kanaal wachtwoord
	
	Identificeert je bij %S als de stichter van het gegeven
	kanaal. Veel commando's verplichten je dit commando te
	gebruiken voordat je ze gebruikt. Het wachtwoord moet
	identiek zijn aan degene verstuurd met het REGISTER
	commando.

CHAN_HELP_LOGOUT
	Gebruik: LOGOUT kanaal nick
	
	Dit commando zorgt ervoor dat de opgegeven nick niet meer
	geidentificeerd is voor het opgegeven kanaal
	
	Als je de stichter van het kanaal bent kan je iedereen
	uitloggen, anders alleen jezelf.

CHAN_HELP_DROP
	Gebruik: DROP kanaal
	
	Heft de registratie van het gegeven kanaal op. Kan alleen
	gebruik wordt door de kanaalstichter, die eerst het
	IDENTIFY commando moet gebruiken.

CHAN_HELP_SET
	Gebruik: SET kanaal optie parameters
	
	Stelt de kanaalstichter in staat om verschillende kanaal-
	opties en andere informatie in te stellen.
	
	Beschikbare opties:
	
	    FOUNDER       Stelt de stichter van het kanaal in
	    SUCCESSOR     Stelt de opvolger van het kanaal in
	    PASSWORD      Stelt het stichterwachtwoord in
	    DESC          Stelt de kanaalbeschrijving in
	    URL           Associeer een URL met het kanaal
	    EMAIL         Associate een E-mail adres met het kanaal
	    ENTRYMSG      Stel een bericht in dat naar de gebruikers
	                    wordt gestuurd zodra ze het kanaal binnekomen
	    BANTYPE       Stelt in hoe Services bans op het kanaal maken
	    MLOCK         Zet kanaal modes vast aan of uit
	    KEEPTOPIC     Onthoud het topic zoland het kanaal niet in
	                    gebruik is
	    OPNOTICE      Stuur een notice wanneer de OP/DEOP commando's
	                    worden gebruikt.
	    PEACE         Reguleer het gebruik van kritieke commando's
	    PRIVATE       Verberg een kanaal van het LIST commando
	    RESTRICTED    Limiteer toegang tot het kanaal
	    SECURE        Activeer %S veiligheidsopties
	    SECUREOPS     Sterkere controle van de kanaalop status
	    SECUREFOUNDER Sterkere controle van de stichter status
	    SIGNKICK      Onderteken kicks die worden uitgevoerd met het
	                    KICK commando
	    TOPICLOCK     Topic kan alleen veranderd worden met TOPIC
	    XOP           Schakel het gebruikersprivilegesysteem om
	
	Type %R%S HELP option voor meer informatie over een
	specifieke optie.

CHAN_HELP_SET_FOUNDER
	Gebruik: SET kanaal FOUNDER nick
	
	Veranderd de stichter van een kanaal. De nieuwe nick moet
	een geregistreerde nick zijn.

CHAN_HELP_SET_SUCCESSOR
	Gebruik: SET kanaal SUCCESSOR nick
	
	Veranderd de opvolger van een kanaal. Als de nick van de
	stichter verloopt of de registratie ervan wordt geannuleerd
	zolang het kanaal nog geregistreerd is wordt de opvolger
	de nieuwe stichter van het kanaal, behalve als de opvolger
	als te veel kanalen (%d) geregistreerd heeft: dan wordt de
	registratie van het kanaal geannuleerd, wat ook gebeurt als
	er geen opvolger is ingesteld. De nieuwe nick moet een
	geregistreerde nick zijn.

CHAN_HELP_SET_PASSWORD
	Gebruik: SET kanaal PASSWORD wachtwoord
	
	Stelt het wachtwoord in dat gebruikt wordt om de stichter
	van het kanaal te identificeren.

CHAN_HELP_SET_DESC
	Gebruik: SET kanaal DESC beschrijving
	
	Stelt de beschrijving van een kanaal in, die weergegeven
	wordt door de LIST en INFO commando's.

CHAN_HELP_SET_URL
	Gebruik: SET kanaal URL [url]
	
	Associeer de gegeven URL met het kanaal. Deze URL zal worden
	weergegeven wanneer iemand informatie over het kanaal op-
	vraagt met het INFO commando. Als er geen paramenter wordt
	wordt de URL voor het kanaal verwijderd.

CHAN_HELP_SET_EMAIL
	Gebruik: SET kanaal EMAIL [adres]
	
	Associeer het gegeven e-mail adres met het kanaal. Dit adres
	zal worden weergegeven wanneer iemand informatie over het
	kanaal opvraagt met het INFO commando. Als er geen para-
	menter wordt gegeven wordt het e-mail adres voor het kanaal
	verwijderd.

CHAN_HELP_SET_ENTRYMSG
	Gebruik: SET kanaal ENTRYMSG [bericht]
	
	Stelt het bericht in dat via /notice zal worden verstuurd
	aan gebruikers die het kanaal binnenkomen. Als er geen
	paramenter wordt gegeven wordt er geen bericht verstuurd
	als iemand het kanaal binnenkomt.

CHAN_HELP_SET_BANTYPE
	Gebruik: SET kanaal BANTYPE bantype
	
	Stelt het bantype in dat gebruikt zal worden door services
	wanneer ze een ban moet zetten op je kanaal.
	
	bantype is een nummer tussen 0 en 3 dat betekent:
	
	0: ban in het formaat *!gebruiker@host
	1: ban in het formaat *!*gebruiker@host
	2: ban in het formaat *!*@host
	3: ban in het formaat *!*gebruiker@*.domein

CHAN_HELP_SET_KEEPTOPIC
	Gebruik: SET kanaal KEEPTOPIC {ON | OFF}
	
	Zet de topicbehoud optie aan of uit voor een kanaal.
	Wanneer topicbehoud aan staat zal het topic van het
	kanaal onthouden worden door %S, zelfs nadat de laatste
	gebruiker het kanaal heeft verlaten. Het topic zal wanneer
	de volgende gebruiker het kanaal binnenkomt worden
	teruggezet.

CHAN_HELP_SET_TOPICLOCK
	Gebruik: SET kanaal TOPICLOCK {ON | OFF}
	
	Zet de topicslot optie aan of uit voor een kanaal.
	Wanneer topicslot aan staat zal %S het niet toestaan om
	het topic van het kanaal te veranderen. Het topic kan dan
	alleen veranderd worden met het TOPIC commando.

CHAN_HELP_SET_MLOCK
	Gebruik: SET kanaal MLOCK modes
	
	Stelt de mode-slot paramenter in voor het kanaal. %S
	staat je toe om verschillen kanaalmodes altijd aan of uit
	te laten staan, of om het mogelijk te maken deze aan of uit
	te zetten met het /MODE commando.
	
	De modes paramenter wordt precies hetzelfde opgebouwd als
	in een /MODE commando: modes achter een + zijn altijd aan,
	en modes achter een - zijn altijd uit. Merk wel op dat, in
	tegenstelling tot het /MODE commando, alleen opgegeven modes
	geforceerd worden door SET MLOCK; het mode-slot wordt eerst
	leeggemaakt voor de nieuwe modes erin worden gezet!
	
	Waarschuwing: Als je het mode-slot gebruikt om een wachtwoord
	op het kanaal te zette moet je, zoals in het 2e voorbeeld, ook
	de optie RESTRICTED aanzette (zie HELP SET RESTRICTED), of
	iedereen die het kanaal binnenkomt als het leeg is kan het
	wachtwoord zien!
	
	Voorbeelden:
	
	    SET #channel MLOCK +nt-iklps
	        Foreert modes n en t aan, en modes i, k, l, p, en s
	        uit. Mode m kan aan of uit gezet worden met /MODE.
	
	    SET #channel MLOCK +knst-ilmp wachtwoord
	        Forceert modes k, n, s, en t aan, en modes i, l, m,
	        en p uit. Forceert ook dat het kanaalwachtwoord
	        "wachtwoord" is.
	
	    SET #channel MLOCK +
	        Verwijderd het mode-slot; alle kanaalmodes kunnen
	        vrij ingesteld worden met /MODE.

CHAN_HELP_SET_PEACE
	Gebruik: SET kanaal PEACE {ON | OFF}
	
	Zet de vrede optie aan of uit voor een kanaal. Wanneer
	vrede aanstaat kan een gebruiker niemand kicken, bannen,
	of iemand's kanaal status wijzigen/verwijderen van een
	gebruiker met een gelijkwaardig of hoger level dan die
	van zichzelf via de %S commando's.

CHAN_HELP_SET_PRIVATE
	Gebruik: SET kanaal PRIVATE {ON | OFF}
	
	Zet de prive optie aan of uit voor een kanaal. Wanneer
	de prive optie aan staat zal %R%S LIST het kanaal
	in geen enkele lijst weergeven.

CHAN_HELP_SET_RESTRICTED
	Gebruik: SET kanaal RESTRICTED {ON | OFF}
	
	Enables or disables the restricted access option for a
	channel.  When restricted access is set, users not on
	the access list will instead be kicked and banned from the channel.

CHAN_HELP_SET_SECURE
	Gebruik: SET kanaal SECURE {ON | OFF}
	
	Zet %S's veiligheidsfuncties aan of uit voor een kanaal.
	Wanneer SECURE aan staat zullen alleen gebruikers die
	hun nicks met %s geregistreerd hebben en geidentificeerd
	zijn met hun wachtwoord toegang krijgen tot het kanaal,
	zoals geregeld door de toegangslijst.

CHAN_HELP_SET_SECUREOPS
	Gebruik: SET kanaal SECUREOPS {ON | OFF}
	
	Zet de veilige ops optie aan of uit voor een kanaal.
	Wanneer veilige ops aan staat mogen gebruikers die niet
	op de toegangslijst staan geen kanaalop status krijgen.

CHAN_HELP_SET_SECUREFOUNDER
	Gebruik: SET kanaal SECUREFOUNDER {ON | OFF}
	
	Zet de veilige stichter optie aan of uit voor een kanaal.
	Wanneer veilige stichter aan staat zal alleen de echte
	stichter een kanaal kunnen annuleren en het wachtwoord, de
	stichter en de opvolger kunnen veranderen, en niet degenen
	die alleen geidentificeerd zijn met %S.

CHAN_HELP_SET_SIGNKICK
	Gebruik: SET kanaal SIGNKICK {ON | LEVEL | OFF}
	
	Zet ondertekende kicks aan of uit voor een kanaal.
	Wanneer SIGNKICK aan staat zullen kicks die uitgevoerd
	worden dmv het %S KICK commando de nick van degene die
	het commando uitgevoerd heeft de nick in de reden hebben.
	
	Als je LEVEL gebruikt zullen degenen die een niveau hebben
	gelijk aan of hoger dan het SIGNKICK niveau op het kanaal
	hun nick niet toegevoegd hebben aan de kicks.
	Zie %R%S HELP LEVELS voor meer informatie.

CHAN_HELP_SET_XOP
	Gebruik: SET kanaal XOP {ON | OFF}
	
	Zet het xOP lijsten systeem aan of uit voor een kanaal.
	Wanneer XOP aan staat moet je de AOP/SOP/VOP
	commando's gebruiken om kanaalprivileges aan gebruikers
	te geven, anders moet je het ACCESS command gebruiken.
	
	Technische Opmerking: Als je van toegangslijsten naar xOP
	lijsten overschakeld zullen je level beschrijvingen en
	gebruikersniveaus worden aangepast. Je zult ze dus niet met
	dezelfde waarden terugvinden als je terugschakelt naar het
	toegangslijsten systeem!
	
	Je moet ook goed opletten of de gebruikers in de goede xOP
	lijst staan als je overschakelt, omdat de gok niet altijd
	perfect is... het is zelfs niet aangeraden om de xOP lijsten
	te gebruiken als je niveaubeschrijvingen hebt veranderd
	met het LEVELS commando.
	
	Het overschakelen van xOP lijsten naar het toegangslijsten
	systeem brengt echter geen problemen met zich mee.

CHAN_HELP_SET_OPNOTICE
	Gebruik: SET kanaal OPNOTICE {ON | OFF}
	
	Zet de op-notificatie optie aan of uit voor een kanaal.
	Wanneer op-notificatie aan staat  zal %S een notice naar
	het kanaal sturen wanneer de OP of DEOP commando's worden
	gebruikt voor een gebruiker in het kanaal.

CHAN_HELP_AOP
	Gebruik: AOP kanaal ADD nick
	         AOP kanaal DEL {nick | entry-nr | lijst}
	         AOP kanaal LIST [mask | lijst]
	         AOP kanaal CLEAR
	
	Onderhoud de AOP (AutoOP) lijst voor een kanaal. De AOP
	lijst geeft gebruikers het recht om automatisch operator te
	worden op het kanaal, zichzelf te unbannen of uit te nodigen
	als dat nodig is, hun groetbericht weer te geven als ze het
	kanaal binnenkomen, enzovoorts.
	
	Het AOP ADD commando voegt de gegeven nick toe aan de
	AOP lijst.
	
	Het AOP DEL commando verwijderd de gegeven nick van de
	AOP lijst. Als een lijst van entry-nummers is opgegeven
	worden die verwijderd. (Zie het LIST-voorbeeld hieronder)
	
	Het AOP LIST commando geeft de AOP lijst weer. Als er
	een mask is gegeven worden alleen de de AOP's die over-
	eenkomen met het opgegeven mask weergegeven. Als een lijst
	van entry-nummers is opgegeven, worden alleen die weer-
	gegeven. Bijvoorbeeld:
	
	   AOP #kanaal LIST 2-5,7-9
	      Geeft AOP's 2 t/m 5 en 7 t/m 9 weer.
	
	Het AOP CLEAR commando verwijderd alle AOP's van de
	AOP lijst.
	
	De AOP ADD en AOP DEL commando's zijn gelimiteerd tot
	SOP's of hoger. AOP CLEAR can alleen gebruikt worden door
	de kanaalstichter. Elke gebruiker op de AOP lijst mag
	echter AOP LIST gebruiken.
	
	Dit commando kan uitgeschakeld zijn voor je kanaal, en in
	dat geval heb je de toegangslijst nodig. Zie
	%R%S HELP ACCESS voor meer informatie over toegangs-
	lijsten, en %R%S HELP SET XOP om te weten te komen
	hoe je tussen toegangslijsten en xOP-lijsten schakelt.

CHAN_HELP_HOP
	Gebruik: HOP kanaal ADD nick
	         HOP kanaal DEL {nick | entry-nr | lijst}
	         HOP kanaal LIST [mask | lijst]
	         HOP kanaal CLEAR
	
	Onderhound de HOP (HalfOP) lijst voor een kanaal. De
	HOP lijst geeft gebruikers de rechten om automatisch
	half-operator te worden op het kanaal.
	
	Het HOP ADD commando voegt de gegeven nick toe aan de
	HOP lijst.
	
	Het HOP DEL commando verwijderd de gegeven nick van de
	HOP lijst. Als een lijst van entry-nummers is opgegeven
	worden die verwijderd. (Zie het LIST-voorbeeld hieronder)
	
	Het HOP LIST commando geeft de HOP lijst weer. Als er
	een mask is gegeven worden alleen de de HOP's die over-
	eenkomen met het opgegeven mask weergegeven. Als een lijst
	van entry-nummers is opgegeven, worden alleen die weer-
	gegeven. Bijvoorbeeld:
	
	   HOP #kanaal LIST 2-5,7-9
	      Geeft AOP's 2 t/m 5 en 7 t/m 9 weer.
	
	Het HOP CLEAR commando verwijderd alle HOP's van de
	HOP lijst.
	
	De HOP ADD, HOP DEL en HOP LIST commando's zijn 
	gelimiteerd tot AOP's en hoger. Het HOP CLEAR commando
	mag alleen door de kanaalstichter gebruikt worden.
	
	Dit commando kan uitgeschakeld zijn voor je kanaal, en in
	dat geval heb je de toegangslijst nodig. Zie
	%R%S HELP ACCESS voor meer informatie over toegangs-
	lijsten, en %R%S HELP SET XOP om te weten te komen
	hoe je tussen toegangslijsten en xOP-lijsten schakelt.

CHAN_HELP_SOP
	Gebruik: SOP kanaal ADD nick
	         SOP kanaal DEL {nick | entry-nr | lijst}
	         SOP kanaal LIST [mask | lijst]
	         SOP kanaal CLEAR
	
	Onderhoud de SOP (SuperOP) lijst voor een kanaal. De
	SOP lijst geeft gebruikers alle rechten van die van de AOP
	lijst, en voegt de vereiste rechten toe om de AutoKick en
	SlechteWoorden lijst te onderhouden, kanaalmemo's te ver-
	struen en te lezen, enzovoorts.
	
	Het SOP ADD commando voegt de gegeven nick toe aan de
	SOP lijst.
	
	Het SOP DEL commando verwijderd de gegeven nick van de
	SOP lijst. Als een lijst van entry-nummers is opgegeven
	worden die verwijderd. (Zie het LIST-voorbeeld hieronder)
	
	Het SOP LIST commando geeft de SOP lijst weer. Als er
	een mask is gegeven worden alleen de de SOP's die over-
	eenkomen met het opgegeven mask weergegeven. Als een lijst
	van entry-nummers is opgegeven, worden alleen die weer-
	gegeven. Bijvoorbeeld:
	
	   SOP #kanaal LIST 2-5,7-9
	      Geeft SOP's 2 t/m 5 en 7 t/m 9 weer.
	
	Het SOP CLEAR commando verwijderd alle SOP's van de
	SOP lijst.
	
	De SOP ADD, SOP DEL en SOP CLEAR commando's zijn
	gelimiteerd tot de kanaalstichter. SOP LIST mag door
	elke gebruiker op de AOP lijst gebruikt worden.
	
	Dit commando kan uitgeschakeld zijn voor je kanaal, en in
	dat geval heb je de toegangslijst nodig. Zie
	%R%S HELP ACCESS voor meer informatie over toegangs-
	lijsten, en %R%S HELP SET XOP om te weten te komen
	hoe je tussen toegangslijsten en xOP-lijsten schakelt.

CHAN_HELP_VOP
	Gebruik: VOP kanaal ADD nick
	         VOP kanaal DEL {nick | entry-nr | lijst}
	         VOP kanaal LIST [mask | lijst]
	         VOP kanaal CLEAR
	
	Onderhoud de VOP (VOicePeople) lijst voor een kanaal.
	De VOP lijst geeft gebruikers het recht om automatisch
	voice te worden op het kanaal en zichzelf voice te geven
	als ze die niet hebben.
	
	Het VOP ADD commando voegt de gegeven nick toe aan de
	VOP lijst.
	
	Het VOP DEL commando verwijderd de gegeven nick van de
	VOP lijst. Als een lijst van entry-nummers is opgegeven
	worden die verwijderd. (Zie het LIST-voorbeeld hieronder)
	
	Het VOP LIST commando geeft de VOP lijst weer. Als er
	een mask is gegeven worden alleen de de VOP's die over-
	eenkomen met het opgegeven mask weergegeven. Als een lijst
	van entry-nummers is opgegeven, worden alleen die weer-
	gegeven. Bijvoorbeeld:
	
	   VOP #kanaal LIST 2-5,7-9
	      Geeft VOP's 2 t/m 5 en 7 t/m 9 weer.
	
	Het VOP CLEAR commando verwijderd alle AOP's van de
	VOP lijst.
	
	De VOP ADD, VOP DEL en VOP LIST commando's zijn
	gelimiteerd tot AOPs en hoger. VOP CLEAR kan alleen
	door de kanaalstichter gebruikt worden.
	
	Dit commando kan uitgeschakeld zijn voor je kanaal, en in
	dat geval heb je de toegangslijst nodig. Zie
	%R%S HELP ACCESS voor meer informatie over toegangs-
	lijsten, en %R%S HELP SET XOP om te weten te komen
	hoe je tussen toegangslijsten en xOP-lijsten schakelt.

CHAN_HELP_ACCESS
	Gebruik: ACCESS kanaal ADD nick niveau
	         ACCESS kanaal DEL {nick | entry-nr | lijst}
	         ACCESS kanaal LIST [mask | lijst]
	         ACCESS kanaal CLEAR
	
	Onderhoud de toegangslijst voor een kanaal. De toegangs-
	lijst specificeert welke gebruikers toestemming hebben om
	kanaal operator te worden, of om %S commando's te gebruiken
	op het kanaal. Verschillende gebruikersniveaus maken het
	mogelijk om verschillende sets met privileges te maken.
	%R%S HELP ACCESS LEVELS voor meer informatie hierover.
	Elke gebruiker die niet op de toegangslijst staat heeft een
	gebruikersniveau van 0.
	
	Het ACCESS ADD commando voegt de gegeven nick toe aan de
	toegangslijst met het gegeven gebruikersniveau. Als de nick
	al op de lijst staat wordt het toegangsniveau veranderd in
	het niveau gespecificeerd in het commando. Het niveau dat
	gespecificeerd is moet minder zijn dan de gebruiker die het
	commando invoert, en als nick al op de toegangslijst
	bestaat moet het huidige toegangsniveau van die nick lager
	zijn dan die van degene die het commando uitvoert.
	
	Het ACCESS DEL commando verwijderd de opgegeven nick van
	de toegangslijst. Als een lijst met entry-nummers is
	gegeven worden die verwijderd. (Zie het voorbeeld voor
	LIST hier onder)
	
	Het ACCESS LIST commando geeft de toegangslijst weer. Als
	een wildcard mask is opgegeven worden alleen de overeen-
	komsten weergegeven. Als een lijst van entry-nummers ie
	gegeven wordt alleen die weergegeven. Bijvoorbeeld:
	
	   ACCESS #kanaal LIST 2-5,7-9
	      Geeft gebruikers 2 t/m 5 en 7 t/m 9 van de
	      toegangslijst weer.
	      
	Het ACCESS CLEAR commando maakt de toegangslijst leeg.

CHAN_HELP_ACCESS_LEVELS
	Gebruikerstoegangslevels
	
	Standaard zijn de volgende toegangslevels gedefineerd:
	
	   Founder   Volledige rechten tot de %S functies;
	                   automatisch op worden bij het binnenkomen
	                   van een kanaal. Merk op dat maar een
	                   persoon stichterstatus kan hebben (het
	                   kan niet worden gegeven met het ACCESS
	                   commando).
	        10   Toegang tot het AKICK commando; automatisch op.
	         5   Automatisch op.
	         3   Automatisch voice.
	         0   Geen speciale privileges; kan op status krijgen
	                   door andere ops (tenzij secure-ops aan is).
	        <0   Mag geen op worden.
	
	Deze niveaus kunnen worden veranderd, en er kunnen nieuwe
	worden toegevoegd dmv het LEVELS commando; type
	%R%S HELP LEVELS voor meer informatie.

CHAN_HELP_AKICK
	Gebruik: AKICK channel ADD {nick | mask} [reden]
	         AKICK channel STICK mask
	         AKICK channel UNSTICK mask
	         AKICK channel DEL {nick | mask | entry-num | lijst}
	         AKICK channel LIST [mask | entry-num | lijst]
	         AKICK channel VIEW [mask | entry-num | lijst]
	         AKICK kanaal ENFORCE
	         AKICK kanaal CLEAR
	
	Onderhoud de AutoKick lijst voor een kanaal. Als een
	gebruiker op de AutoKick lijst het kanaal binnen probeert
	te komen zal %S die gebruiker automatisch worden bannen
	en kicken van het kanaal.
	
	Het AKICK ADD commando voegt de gegeven nick of gebruikers-
	mask toe aan de AutoKick lijst. Als er een reden is gegeven
	met het commando zal die reden gebruikt worden om de gebruiker
	te kicken; zo niet is de reden "You have been banned from the
	channel".
	Als een geregistreerde nick wordt toegevoegd, wordt de nickserv
	account toegevoegd in plaats van de mask. Alle gebruikers binnen
	deze groep zullen automatisch worden gekicked.
	
	Het AKICK STICK commando bant het gegeven mask permanent
	op het kanaal. Als iemand de ban probeert te verwijderen
	zal %S de ban automatisch opnieu toevoegen.
	Deze functie kan niet op geregistreerde nicks gebruikt worden.
	
	Het AKICK UNSTICK commando annuleert het effect van het
	AKICK STICK commando, zodat je de ban weer weg kan halen
	van het kanaal.
	
	Het AKICK DEL commando verwijderd de gegeven nick of mask
	van de AutoKick lijst. Het verwijderd echter geen ban die
	is geplaatst door een AutoKick; die moeten handmatig
	verwijderd worden.
	
	Het AKICK LIST commando geeft de AutoKick lijst weer, of
	optioneel alleen de AutoKick's die overeenkomen met het
	opgegeven mask.
	
	Het AKICK VIEW commando is een woordenrijkere versie
	van het AKICK LIST commando.
	
	Het AKICK ENFORCE commando zorgt ervoor dat %S de
	huidige AKICK lijst forceert door het verwijderen van
	gebruikers die overeenkomen met een AKICK mask.
	
	The AKICK CLEAR commando maakt de AutoKick lijst leeg.

CHAN_HELP_LEVELS
	Gebruik: LEVELS kanaal SET type niveau
	         LEVELS kanaal {DIS | DISABLE} type
	         LEVELS kanaal LIST
	         LEVELS kanaal RESET
	
	Het LEVELS commando staat precieze controle toe over de
	betekenis van de numerieke toegangsniveaus die worden
	gebruikt voor kanalen. Met dit commando kan je de toegangs-
	niveaus defineren die benodigd zijn voor de meeste functies
	van %S. (De SET FOUNDER en SET PASSWORD commando's zijn,
	evenals dit commando, altijd gelimiteerd tot de stichter.)
	
	LEVELS SET staat toe om de toegangsniveaus voor functies
	of een groep functies te veranderen. LEVELS DISABLE (of
	kortweg DIS) schakelt een automatische functie uit, of
	staat gebruik van de functie niet toe door iemand anders
	dan de stichter.
	
	LEVELS LIST geeft de huidige levels voor elke functie of
	groep van functies weer. LEVELS RESET stelt de niveaus in
	op de standaardwaarden van een nieuw kanaal (zie
	HELP ACCESS LEVELS).
	
	Voor een list van de opties en functies waarvan de niveaus
	kunnen worden aangepast, zie HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	De volgende optie-/functienamen worden begrepen. Let op dat
	de niveaus voor AUTODEOP en NOJOIN maximale niveaus zijn,
	terwijl alle andere minimale niveaus zijn.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Gebruik: INFO kanaal [ALL]
	
	Geeft informatie weer over het gegeven geregistreerde
	kanaal, zoals de stichter, registratietijd, tijd van
	laatste gebruik, beschrijving, en mode-slot indien aan-
	wezig. Als ALL is gespecificeerd zullen het groetbericht
	en de opvolger ook worden weergegeven.
	
	Standaard is de ALL optie gelimiteerd tot degenen met
	stichter-toegang tot het kanaal.

CHAN_HELP_LIST
	Gebruik: LIST trefbeeld
	
	Geeft alle geregistreerde kanalen weer die overeen komen
	met het trefbeeld. (Kanalen met de PRIVATE optie aan
	worden niet weergegeven.)

CHAN_HELP_OP
	Gebruik: OP [#kanaal [nick]]
	
	Geeft de gegeven nick op-status op een kanaal. Als er geen
	nick is gegeven zal je zelf op-status krijgen. Als er geen
	kanaal en nick zijn gegeven zal je op-status krijgen op
	alle kanalen waar je die rechten hebt.
	
	Standaard is dit gelimiteerd tot AOPs of degenen met niveau
	5 of hoger op het kanaal.

CHAN_HELP_DEOP
	Gebruik: DEOP [#kanaal [nick]]
	
	Verwijderd de op status van de gegeven nick op een kanaal.
	Als er geen nick is gegeven zal je zelf je opstatus kwijt
	raken. Als er geen kanaal en nick zijn gegeveen zal je
	je op status kwijtraken op alle kanalen waar je momenteel
	bent waar je die rechten hebt.
	
	Standaard is dit gelimiteerd tot AOPs of degenen met niveau
	5 of hoger op het kanaal.

CHAN_HELP_VOICE
	Gebruik: VOICE [#kanaal [nick]]
	
	Geeft de geselecteerde nick voice status op een kanaal. Als
	er geen nick gegeven is zal je zelf voice status krijgen.
	Als er geen kanaal en geen nick gegeven zijn zal je op alle
	kanalen waar je momenteel bent en de rechten daartoe hebt
	voice status krijgen.
	
	Standaard is dit gelimiteerd tot AOPs of degenen met niveau
	5 of hoger op het kanaal, of tot VOPs of degenen met niveau
	3 of hoger voor het voice status geven van zichzelf.

CHAN_HELP_DEVOICE
	Gebruik: DEVOICE [#kanaal [nick]]
	
	Verwijdert de voice status van de gegeven nick op een kanaal.
	Als er geen nick is gegeven zal je zelf je voice status
	kwijtraken. Als er geen kanaal en nick gegeven zijn zal je
	zelf je voice status kwijtraken op alle kanalen waar je
	momenteel bent en de rechten ervoor hebt.
	
	Standaard is dit gelimiteerd tot AOPs of degenen met niveau
	5 of hoger op het kanaal, of tot VOPs of degenen met niveau
	3 of hoger voor het voice status geven van zichzelf.

CHAN_HELP_HALFOP
	Gebruik: HALFOP [#kanaal [nick]]
	
	Geeft de geselecteerde nick halfop status op een kanaal. Als
	er geen nick is gegeven zul je zelf halfop status krijgen.
	Als er geen kanaal en geen nick gegeven zijn zul je op alle
	kanalen waar je momenteel bent halfop krijgen, mits je daar
	rechten toe hebt.
	
	Standaard is dit gelimiteerd tot AOPs of degenen met niveau
	5 of hoger op het kanaal, of tot HOPs of degenen met niveau
	4 of hoger voor het halfop status geven van zichzelf.

CHAN_HELP_DEHALFOP
	Gebruik: DEHALFOP [#kanaal [nick]]
	
	Verwijdert de halfop status van de gegeven nick op een kanaal.
	Als er geen nick is gegeven zal je zelf je halfop status
	kwijtraken. Als er geen nick en geen kanaal zijn gegeven zal
	je zelf je halfop status kwijt raken op alle kanalen waar je
	momenteel bent, mits je de rechten daartoe hebt.
	
	Standaard is dit gelimiteerd tot AOPs of degenen met niveau
	5 of hoger op het kanaal, of tot HOPs of degenen met niveau
	4 of hoger voor het halfop status geven van zichzelf.

CHAN_HELP_PROTECT
	Gebruik: PROTECT [#kanaal [nick]]
	
	Beschermt de geselecteerde nick op een kanaal. Als nick niet
	is gegeven, zal je zelf beschermd worden. Als kanaal en nick
	beiden niet zijn gegeven, zal je zelf op alle kanalen waar
	je momenteel bent beschermd worden, mits je de rechten
	daartoe hebt.
	
	Standaard is dit gelimiteerd tot de oprichter, SOPs, of
	degenen met een niveau van 10 of hoger op het kanaal voor
	zelf-protectie.

CHAN_HELP_DEPROTECT
	Gebruik: DEPROTECT [#kanaal [nick]]
	
	Heft de bescherming van de gegeven nick op een kanaal op.
	Als nick niet is gegeven, zal de beschermen van jezelf
	opgeheven worden. Als kanaal en nick niet zijn gegeven,
	zal de bescherming van jezelf op alle kanalen waar je
	momenteel bent worden opgeheven, mits je de rechten
	daartoe hebt.
	
	Standaard is dit gelimiteerd tot de stichter, SOPs, of
	degenen met een niveau van 10 of hoger op het kanaal voor
	zelf-protectie.

CHAN_HELP_OWNER
	Gebruik: OWNER [#kanaal]
	
	Geeft je eigenaar status op kanaal. Als kanaal niet is
	gegeven krijg je eigenaar status op alle kanalen waar je
	momenteel bent, mits je de rechten daartoe hebt.
	
	Gelimiteerd tot degenen met stichter-rechten op het kanaal.

CHAN_HELP_DEOWNER
	Gebruik: DEOWNER [#kanaal]
	
	Verwijdert je eigenaar status op channel. Als kanaal niet
	is gegeven wordt je eigenaar status op alle kanalen waar je
	momenteel bent verwijderd, mits je de rechten daartoe hebt.
	
	Gelimiteerd tot degenen met stichter-rechten op het kanaal.

CHAN_HELP_INVITE
	Gebruik: INVITE kanaal
	
	Laat %S je uitnodigen in een gegeven kanaal.
	
	Standaard is dit gelimiteerd tot AOPs of degenen met
	niveau 5 of hoger op het kanaal.

CHAN_HELP_UNBAN
	Gebruik: UNBAN kanaal
	
	Zorgt ervoor dat %S alle bans verwijdert die je ervan
	belemmeren het gegeven kanaal te joinen.
	
	Standaard is dit gelimiteerd tot AOPs of degenen met
	niveau 5 of hoger op het kanaal.

CHAN_HELP_KICK
	Gebruik: KICK [#kanaal [nick [reden]]]
	
	Kickt de gegeven nick van het gegeven kanaal. Als nick niet
	is gegeven wordt je zelf gekicked. Als kanaal en nick niet
	zijn gegeven wordt je zelf gekicked op alle kanalen waar je
	bent, mits je de rechten daartoe hebt.
	
	Standaard is dit gelimiteerd tot AOPs of degenen met
	niveau 5 of hoger op het kanaal.

CHAN_HELP_BAN
	Gebruik: BAN [#kanaal [nick [reden]]]
	
	Bant de gegeven nick op het gegeven kanaal. Als nick niet
	is gegeven wordt je zelf gebanned. Als kanaal en nick niet
	zijn gegeven wordt je zelf gebanned op alle kanalen waar je
	momenteel bent, mits je de rechten daartoe hebt.
	
	Standaard is dit gelimiteerd tot AOPs of degenen met
	niveau 5 of hoger op het kanaal.

CHAN_HELP_TOPIC
	Gebruik: TOPIC kanaal [topic]
	
	Zorgt ervoor dat %S het kanaal topic verandert in het
	gegeven topic. Als topic niet is gegeven wordt het topic
	leeg gemaakt. Dit commando is het meest bruikbaar in
	samenwerking met SET TOPICLOCK. Zie ook %R%S HELP SET
	TOPICLOCK voor meer informatie.
	
	Standaard is dit gelimiteerd tot degenen met stichter-toegang
	op het kanaal.

CHAN_HELP_CLEAR
	Gebruik: CLEAR kanaal wat
	
	Zorgt ervoor dat %S bepaalde instellingen van het kanaal
	verwijdert. wat kan zijn:
	
	     MODES    Verwijder alle modes op het kanaal (verwijdert
	                  modes i,k,l,m,n,p,s,t).
	     BANS     Verwijder alle bans op het kanaal.
	     EXCEPTS  Clears all excepts on the channel.
	     INVITES  Clears all invites on the channel.
	     OPS      Verwijder kanaal-operator status (mode +o) van
	                  alle kanaal-operators.
	     HOPS     Verwijder kanaal-halfoperator status (mode +h)
	                  van alle kanaal-halfoperators, indien deze
			  mode ondersteund wordt.
	     VOICES   Verwijder "voice" status (mode +v) van iedereen
	                  met "voice" status op het kanaal.
	     USERS    Verwijder (kick) alle gebruikers van het kanaal.
	
	Standaard is dit gelimiteerd tot degenen met stichter-toegang
	op het kanaal.

CHAN_HELP_GETKEY
	Gebruik: GETKEY kanaal
	
	Geef de key van het gegeven kanaal terug. Dit is een
	commando wat vooral bedoeld is voor gebruik door bots of
	scripts, dus is de output in het volgende formaat:
	
	KEY <kanaal> <key>
	
	key is "NO KEY" als er geen key is ingesteld.
	
CHAN_HELP_SENDPASS
	Gebruik: SENDPASS kanaal
	
	Stuur het wachtwoord van het gegeven kanaal naar het email
	adres gespecificeerd in NickServ voor de stichter van het
	kanaal. Dit commando is erg bruikbaar om verloren wacht-
	woorden terug te halen.
	
	Kan gelimiteerd zijn tot IRC operators op sommige netwerken.
	
	Dit commando is onbeschikbaar wanneer encryptie is ingeschakeld.

CHAN_SERVADMIN_HELP
	
	Services administrators kunnen ook elk kanaal dropped zonder dat
	ze hoeven te indentificeren via een wachtwoord, en kunnen de
	toegangs-, AKICK-, en niveau-lijsten bekijken voor elk kanaal.

CHAN_SERVADMIN_HELP_LOGOUT
	Gebruik: LOGOUT kanaal [nick]
	
	Dit kanaal zorgt ervoor dat de gegeven nick niet meer
	geidentificeerd is voor het gegeven kanaal.
	
	Als je de stichter van het kanaal bent kun je iedereen
	uitloggen, anders kun je alleen jezelf uitloggen.
	
	Als je een Services adminstrator bent kun je iedereen
	uitloggen van elke kanaal zonder daar stichter-rechten te
	hebben. Je kan ook het nick-paramenter weglaten: dit
	zorgt ervoor dat iedereen uitgelogd zal worden.

CHAN_SERVADMIN_HELP_DROP
	Gebruik: DROP kanaal
	
	Zegt de registratie van het gegeven kanaal op. Alleen
	Services admins kunnen elk kanaal droppen, ook al zijn
	ze niet voor het betreffende kanaal geindtificeerd.

CHAN_SERVADMIN_HELP_SET
	
	Services adminstrators kunnen ook de NOEXPIRE optie
	instellen, waarmee kanalen niet kunnen verlopen. Ook
	kunnen ze elke optie voor elk kanaal zet zonder zich te
	identificeren met het wachtwoord voor het kanaal.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Gebruik: SET kanaal NOEXPIRE {ON | OFF}
	
	Stelt in of het gegeven kanaal zal verlopen. Door deze optie
	AAN te zetten kun je verhinderen dan het kanaal verloopt.

CHAN_SERVADMIN_HELP_INFO
	
	Services adminstrators kunnen het ALL parameter gebruiken
	bij elk kanaal.

CHAN_SERVADMIN_HELP_LIST
	Gebruik: LIST trefbeeld [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
	
	Geeft alle geregistreerde kanalen weer die overeen komen met
	het gegeven tgrefbeeld. Kanalen met de PRIVATE optie aan
	worden alleen weergegeven aan Services administrators.
	Kanalen met de NOEXPIRE aan zullen een ! voor de kanaal-
	naam hebben voor Services administrators.
	
	Als de FORBIDDEN, SUSPENDED of NOEXPIRE opties zijn gegeven
	worden alleen kanalen die, respectievelijk, verboden,
	tijdelijk buiten werken gesteld zijn, of als niet-verlopen
	gemarkeerd zijn weergegeven. Als er meerdere opties worden
	gegeven worden alle gevraagde types weergegeven. Deze opties
	zijn gelimiteerd tot Services adminstrators.

CHAN_SERVADMIN_HELP_GETPASS
	Gebruik: GETPASS kanaal
	
	Geeft het wachtwoord voor het gegeven kanaal terug.
	Let op: wanneer je dit commando gebruikt zal er een bericht
	waarin staat wie het commando op welk kanaal heeft gebruikt
	worden gelogged en verstuurd als WALLOPS/GLOBOPS.

CHAN_SERVADMIN_HELP_FORBID
	Gebruik: FORBID kanaal [reden]
	
	Verbied iedereen van het registreren of gebruiken van het
	gegeven kanaal. Kan ongedaan gemaakt worden door het
	droppen van het kanaal.
	
	Reden kan verplicht zijn op sommige netwerken.

CHAN_SERVADMIN_HELP_SUSPEND
	Gebruik: SUSPEND kanaal [reden]
	
	Verbied iedereen van het registreren of gebruiken van het
	gegeven kanaal. Kan ongedaan gemaakt worden door middel van
	het UNSUSPEND commando om alle vorige kanaal data en
	instellingen te behouden.
	
	Op sommige netwerken is een reden verplicht.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Syntax: UNSUSPEND channel
	
	Stelt een buiten werking gesteld kanaal weer in werking.
	Alle data en instellingen worden teruggeven van voor de
	buitenwerkingstelling.
	Releases a suspended channel. All data and settings
	are preserved from before the suspention.

CHAN_SERVADMIN_HELP_STATUS
	Gebruik: STATUS kanaal nick
	
	Geeft het huidige toegangsniveau van de gegeven nick op het
	gegeven kanaal weer. Het antwoord is in het formaat:
	
	    STATUS kanaal nick toegangsniveau
	
	Als er een fout optreed zal het antwoord zijn in het formaat:
	
	    STATUS ERROR fout-bericht

###########################################################################
#
# MemoServ help messages
#
###########################################################################
MEMO_HELP_CMD_SEND
	    SEND   Vertstuur een memo naar een nick of kanaal
MEMO_HELP_CMD_CANCEL
	    CANCEL Annuleer je laatst verzonden memo
MEMO_HELP_CMD_LIST
	    LIST   Geef je memo's weer
MEMO_HELP_CMD_READ
	    READ   Lees een of meerdere memo's
MEMO_HELP_CMD_DEL
	    DEL    Verwijder een of meerdere memo's
MEMO_HELP_CMD_SET
	    SET    Stel opties gerelateerd aan memo's in
MEMO_HELP_CMD_INFO
	    INFO   Toont info over jouw memo's
MEMO_HELP_CMD_RSEND
	    RSEND  Verstuur een memo en verzoek een ontvangstbevestiging
MEMO_HELP_CMD_CHECK
	    CHECK  Kijk of je laatste memo naar iemand is gelezen
MEMO_HELP_CMD_SENDALL
	    SENDALL  Stuur een memo naar alle geregistreerde gebruikers 
MEMO_HELP_CMD_STAFF
	    STAFF  Stuur een memo naar alle opers/admin

MEMO_HELP_HEADER
	%S is een services die IRC gebruikers in staat stelt om
	korte berichten te sturen naar andere IRC gebruikers, online
	of offline, of naar kanalen(*). De verstuurder's nick en de
	nick van de vanger, of het kanaal waar de memo naartoe gaat,
	moeten geregistreerd zijn om de memo te versturn.
	
	%S's commando's bevatten:
	

MEMO_HELP_ADMIN
	not used.
MEMO_HELP_OPER
	not used.
MEMO_HELP_FOOTER
	
	Type %R%S HELP commando voor hulp bij een van
	de bovenstaande commando's
	
	(*) Standaard kan elke gebruiker met tenminste toegangsniveau
	    10 de kanaalmemo's lezen. Dit kan aangepast worden met
	    het %s LEVELS commando.

MEMO_HELP_SEND
	Gebruik: SEND {nick | kanaal} memo-text
	
	Stuurt de genoemde nick of kanaal een memo bevattende
	memo-text. Wanneer je een memo naar een nick stuurt zal
	de ontvanger een notice krijgen dat hij/zij een nieuwe memo
	heeft. De doel nick/kanaal moet geregistreerd zijn.
	
MEMO_HELP_CANCEL
	Gebruik: CANCEL {nick | kanaal}
	
	Annuleert de laatste memo die je gestuurd hebt aan de gegeven
	nick of kanaal, indien deze nog niet gelezen is op het moment
	dat je dit commando gebruikt.

MEMO_HELP_LIST
	Gebruik: LIST [kanaal] [lijst | NEW]
	
	Geeft alle memo's die je hebt weer. Als je NEW opgeeft worden
	alleen nieuwe (ongelezen) memo's weergegeven. Ongelezen memo's
	worden gemarkeerd met een "*" links van het memo nummer. Je kan
	ook een lijst van nummers specificeren, zoals in het voorbeeld:
	
	   LIST 2-5,7-9
	      Geeft de memo's genummerd 2 t/m 5 en 7 t/m 9 weer.

MEMO_HELP_READ
	Gebruik: READ [kanaal] {nr | lijst | LAST | NEW}
	
	Verstuurt de text van de gespecificeerde memo's. Als LAST is
	gegeven wordt de laatst ontvangen memo verstuurd. Als NEW is
	gegeven worden alle nieuwe memo's verstuurd. Anders wordt
	memo nummer nr gegeven. Je kan ook een lijst van nummers
	opgeven, zoals in onderstaand voorbeeld:
	
	   READ 2-5,7-9
	      Geeft de memo's genummerd 2 t/m 5 en 7 t/m 9 weer.

MEMO_HELP_DEL
	Gebruik: DEL [kanaal] {nr | lijst | LAST | ALL}
	
	Verwijdert de gespecificeerde memo('s). Je kan meerdere
	memo-nummers specificeren of bereiken van nummers ipv een
	enkel nummer, zoals in het 2e voorbeeld hier onder.
	
	Als LAST is gegeven, wordt je laatste nieuwe memo verwijderd.
	Als ALL is gegeven, worden al je memo's verwijderd.
	
	Voorbeelden:
	
	   DEL 1
	      Verwijdert je eerste memo.
	
	   DEL 2-5,7-9
	      Verwijdert de memo's genummerd 2 t/m 5 en 7 t/m 9.

MEMO_HELP_SET
	Gebruik: SET optie parameters
	
	Stelt verschillende memo opties in.  optie kan zijn:
	
	    NOTIFY      Veranderd wanneer je geinformeerd wordt over
	                    nieuwe memo's (alleen voor nicks)
	    LIMIT       Stelt het maximum aantal memo's dat je kan
	                    ontvangen in
	
	Type %R%S HELP SET optie voor more informatie over
	een specifieke optie.

MEMO_HELP_SET_NOTIFY
	Gebruik: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
	
	Verandert wanneer je geinformeerd zal worden over nieuwe memo's:
	
	    ON      Je wordt geinformeerd over memo's wanneer je inlogt,
	               terugkomt van /AWAY, en wanneer ze naar je
	               gestuurd worden.
	    LOGON   Je wordt geinformeerd over memo's wanneer je inlogt,
	               of wanneer je terugkomt van /AWAY.
	    NEW     Je wordt alleen geinformeerd over memo's wanner ze
	               naar je worden verstuurd.
	    MAIL    You will be notified of memos by email aswell as
	               any other settings you have.
	    NOMAIL  You will not be notified of memos by email.
	    OFF     Je zal geen informaties over memo's ontvangen.
	
	ON is wezenlijk LOGON en NEW gecombineerd.

MEMO_HELP_SET_LIMIT
	Gebruik: SET LIMIT [kanaal] limiet
	
	Stelt het maximum aantal memo's in dat jij (of het gegeven
	kanaal) kan hebben. Als je dit op 0 zet zal niemand memo's
	naar je kunnen sturen. Je kan dit echter niet hoger dan %d
	zetten.

MEMO_HELP_INFO
	Gebruik: INFO [kanaal]
	
	Geeft informatie weer over het aantal memo's dat je hebt,
	hoeveel daarvan er ongelezen zijn, en hoeveel memo's je in
	totaal kan ontvangen. Met een paramenter wordt dezelfde
	informatie voor het gegeven kanaal weergegeven.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Gebruik: SET LIMIT [gebruiker | kanaall] {limiet | NONE} [HARD]
	
	Stelt het maximum aantal memo's in dat een gebruiker of
	kanaal mag hebben. Als je deze limiet op 0 zet kan de
	gebruiker geen memo's ontvangen; zet dit op NONE om toe
	te staan dat de gebruiker of kanaal zoveel memo's mag hebben
	als hij/zij wil. Als je geen nick of kanaal opgeeft wordt
	je eigen limiet ingesteld.
	
	Als je HARD toevoegt kan de gebruiker de limiet niet
	veranderen. HARD weglaten heeft het tegenovergestelde
	effect: de gebruiker in staat stellen de limiet te veranderen
	(zelfs als een vorige limiet is ingesteld met HARD).
	
	Het gebruiken van het SET LIMIT commando is gelimiteerd tot
	Services admins. Andere gebruikers mogen alleen een limiet
	voor zichzelf of voor een kanaal waar ze genoeg rechten hebben
	instellen, mogen geen limiet instellen boven %d, en mogen geen
	harde limiet instellen.

MEMO_SERVADMIN_HELP_INFO
	Gebruik: INFO [nick | kanaal]
	
	Geeft informatie weer over het aantal memo's dat je hebt,
	hoeveel daarvan er ongelezen zijn, en hoeveel memo's je in
	totaal kan ontvangen.
	
	Met een kanaal paramenter wordt dezelfde informatie voor
	het gegeven kanaal weergegeven.
	
	Met een nick paramenter wordt dezelfde informatie voor de
	gegeven nick weergegeven. Dit gebruik is gelimiteerd tot
	Services admins.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_STAFF
	Gebruik: STAFF memo-text
	
	Stuurt alle services-staf een memo die memo-text bevat.
	Let op: Als je opers op de operator- en op de administrator-
	lijst hebt zullen ze de memo twee keer ontvangen. Hetzelfde
	geldt voor oper's die tegelijk op de root-lijst als op een
	andere lijst staan.

MEMO_HELP_SENDALL
	Gebruik: SENDALL memo-text

	Stuurt alle geregistreerde gebruikers een memo die
	memo-text bevat.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_RSEND
	Gebruik: RSEND {nick | kanaal} memo-tekst
	
	Stuurt de gegeven nick of kanaal een memo met memo-tekst.
	Wanneer je een memo naar een nick stuurt zal de ontvanger een
	notificatie krijgen dat hij/zij een nieuwe memo heeft. Ook moet
	de gegeven nick/kanaal geregistreerd zijn.
	Zodra de memo is gelezen door de ontvanger zal er automatisch
	een memo teruggestuurd worden naar degene die de memo verstuurd
	heeft om hem/haar te informeren dat de memo gelezen is.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_CHECK
	Gebruik: CHECK nick
	
	Controleert of de _laatste_ memo die je naar nick gestuurd
	hebt isgelezen of niet. Let wel op: dit werkt alleen met nicks,
	niet met kanalen.

###########################################################################
#
# OperServ help messages
#
###########################################################################

OPER_HELP_CMD_GLOBAL
	    GLOBAL      Stuurt een bericht naar alle gebruikers
OPER_HELP_CMD_STATS
	    STATS       Geeft de status van Services en netwerk weer
OPER_HELP_CMD_OPER
	    OPER        Wijzig de Services operator lijst
OPER_HELP_CMD_ADMIN
	    ADMIN       Wijzig de Services Administrator lijst
OPER_HELP_CMD_STAFF
	    STAFF       Display Services staff and online status
OPER_HELP_CMD_MODE
	    MODE        Verander de modes op een kanaal
OPER_HELP_CMD_KICK
	    KICK        Kick een gebruiker uit een kanaal
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  Verwijder alle modes van een kanaal
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  Kill alle gebruikers met een bepaalde hostmask
OPER_HELP_CMD_AKILL
	    AKILL       Beheer de AKILL lijst
OPER_HELP_CMD_SGLINE
	    SGLINE      Beheer de SGLINE lijst
OPER_HELP_CMD_SQLINE
	    SQLINE      Beheer de SQLINE lijst
OPER_HELP_CMD_SZLINE
	    SZLINE      Beheer de SZLINE lijst
OPER_HELP_CMD_CHANLIST
	    CHANLIST    Geef alle kanalen weer
OPER_HELP_CMD_USERLIST
	    USERLIST    Geef alle gebruikers weer
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   Stel de berichten in die gebruikers krijgen als
	                ze inloggen
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  Stel de berichten in waarvan gebruikers er willekeurig
	                een te zien krijgen als ze inloggen
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    Stel de berichten in die gebruikers te zien krijgen
	                die OPER worden
OPER_HELP_CMD_SESSION
	    SESSION     Bekijk de lijst met sessies per host
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   Beheer de sessie-limiet uitzonderingen lijst
OPER_HELP_CMD_NOOP
	    NOOP        Verwijder tijdelijk alle O:regels van een server
	                op afstand

OPER_HELP_CMD_JUPE
	    JUPE        "Jupiter" een server
OPER_HELP_CMD_IGNORE
	    IGNORE      Wijzig de Services negeer-lijst
OPER_HELP_CMD_SET
	    SET         Stel verscheidene globale Services opties in
OPER_HELP_CMD_RELOAD
	    RELOAD      Herlaad Services' configuratie bestand
OPER_HELP_CMD_UPDATE
	    UPDATE      Forceer dat de Services database onmiddelijk
	                op de harde schijf wordt weggeschreven
OPER_HELP_CMD_RESTART
	    RESTART     Sla databases op en herstart Services
OPER_HELP_CMD_QUIT
	    QUIT        Sluit Services af zonder databases op te slaan
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    Sla databases op en sluit Services af
OPER_HELP_CMD_DEFCON
	    DEFCON      Manipulate the DefCon system
OPER_HELP_CMD_CHANKILL
	    CHANKILL    AKILL all users on a specific channel
OPER_HELP_CMD_OLINE
	    OLINE       Geef Operflags aan een bepaalde gebruiker
OPER_HELP_CMD_UMODE
	    UMODE       Wijzigt de modus van een gebruiker
OPER_HELP_CMD_SVSNICK
	    SVSNICK     Forcefully change a user's nickname
OPER_HELP_CMD_MODLOAD
	    MODLOAD     Laad een module
OPER_HELP_CMD_MODUNLOAD
	    MODUNLOAD   Ontlaad een module
OPER_HELP_CMD_MODINFO
	    MODINFO     Info over een geladen module
OPER_HELP_CMD_MODLIST
	    MODLIST     Geef geladen modules weer

OPER_HELP
	%S commando's:

OPER_HELP_LOGGED
	Waarschuwing: Alle commando's die naar %S gestuurd worden worden gelogd!

OPER_HELP_GLOBAL
	Gebruik: GLOBAL bericht
	
	Stelt Administrators in staat om berichten te versturen
	naar alle gebruikers op het netwerk. Berichten zullen 
	verstuurd worden vanaf de nick %s.

OPER_HELP_STATS
	Gebruik: STATS [AKILL | ALL | RESET]
	
	Zonder een optie wordt het huidig aantal gebruikers en IRCops
	online (zonder Services), het hoogste aantal gebruikers
	gelijktijdig online sinds Services zijn gestuurd, en hoe lang
	de Services al online zijn weegegeven.
	
	Met de AKILL optie worden de huidige grootte van de AKILL
	lijst en de huidige standaard verloop-tijd weergegeven.
	
	De ALL optie is alleen beschikbaar voor Services administrators
	en geeft informatie over het geheugen gebruik van Services.
	Het gebruiken van dit commando van er voor zorgen dat de Services
	gedurende een korte tijd niet meer reageren indien ze gebruikt
	worden op een groot netwerk, dus gebruik het niet te vaak!
	
	De ALL optie stelt momenteel het maximum aantal gebruikers in
	op het aantal gebruikers dat momenteel aanwezig is op het netwerk.
	
	UPTIME kan als een synoniem voor STATS gebruikt worden.

OPER_HELP_OPER
	Gebruik: OPER ADD nick
	         OPER DEL {nick | entry-nr | lijst}
	         OPER LIST [mask | lijst]
	         OPER CLEAR
	
	Stelt Services Root in staat om nicknames toe te
	voegen of te verwijderen uit de Services operator lijst. Een
	gebruiker van wie de nickname op de Services operator lijst
	staat en die geidentificeerd is bij %s zal in staat
	zijn om de Services operator commando's te gebruiken.
	
	Het OPER ADD commando voegt de opgegeven nickname toe aan
	de Services operator lijst.

	Het OPER DEL commando verwijdert de opgegeven nickname van
	de Services operator lijst. Als een lijst van entry-nummers is
	opgegeven worden die verwijderd.
	(Zie ook het voorbeeld van LIST hier onder.)
	
	Het OPER LIST commando geeft de Services operator lijst weer.
	Als een mask met een * is gegeven, worden alleen de gebruikers
	die overeenkomen met het opgegeven mask weergegven. Als een lijst
	van entry-nummers is opgegeven worden alleen die weergegeven.
	Bijvoorbeeld:

	   OPER LIST 2-5,7-9
	      Geeft Services operator nummer 2 t/m 5 en 7 t/m 9 weer.
	      
	Het OPER CLEAR commando verwijdert alle nicks van de Services
	operator lijst.
	
	Elke IRCop mag OPER LIST gebruiken.

OPER_HELP_ADMIN
	Gebruik: ADMIN ADD nick
	         ADMIN DEL {nick | entry-nr | lijst}
	         ADMIN LIST [mask | list]
	         ADMIN CLEAR
	
	Stelt de Services root in staat om nicknames toe te voegen of
	te verwijderen uit de Services administrator lijst. Een
	gebruiker van wie de nickname op de Services administrator
	lijst staat en die geidentificeerd is bij %s zal in staat
	zijn om de Services administrator commando's te gebruiken.
	
	Het ADMIN ADD commando voegt de opgegeven nickname toe aan
	de Services administrator lijst.

	Het ADMIN DEL commando verwijdert de opgegeven nickname van
	de Services administrator lijst. Als een lijst van entry-nummers is
	opgegeven worden die verwijderd.
	(Zie ook het voorbeeld van LIST hier onder.)
	
	Het ADMIN LIST commando geeft de Services administrator lijst weer.
	Als een mask met een * is gegeven, worden alleen de gebruikers
	die overeenkomen met het opgegeven mask weergegven. Als een lijst
	van entry-nummers is opgegeven worden alleen die weergegeven.
	Bijvoorbeeld:

	   ADMIN LIST 2-5,7-9
	      Geeft Services administrator nummer 2 t/m 5 en 7 t/m 9 weer.
	      
	Het ADMIN CLEAR commando verwijdert alle nicks van de Services
	adminisrator lijst.
	
	Elke IRCop mag ADMIN LIST gebruiken. De rest is gelimiteerd tot
	Services root.

OPER_HELP_IGNORE
	Gebruik: IGNORE {ADD|DEL|LIST|CLEAR} [tijd] [nick | mask]
	
	Stelt Services administrators in staat de Services een nick
	of mask voor een bepaalde tijd of tot de volgende herstart te
	laten negeren. Standaard wordt de tijd gegeven in seconden.
	Je kan de tijd opgegeven met behulup van units. Geldige units 
	zijn: s voor seconden, m voor minuten, h voor uren
	en d voor dagen. Combinaties hiervan zijn niet toegestaan. 
	Om Services iemand permanent te laten negeren kun je 0 opgeven als tijd.
	Als je een mask toevoegd moet deze in volgende format zijn:
	user@host of nick!user@host, iets anders wordt een nick beschouwd.
	Wildcards zijn toegestaan.
	
	IRC Operators zullen niet genegeerd worden, zelfs al komen
	ze overeen met een mask uit de lijst.

OPER_HELP_MODE
	Gebruik: MODE kanaal modes
	
	Stelt Services operators in staat om de modes van elk willekeurig
	kanaal in te stellen. De paramenters zijn het zelfde als bij een
	standaard /MODE commando.

OPER_HELP_UMODE
	Gebruik: UMODE gebruiker modes
	
	Stelt Super Admins in staat gebruikersmodes van
	elke willekeurige gebruiker in te stellen. De paramenters
	zijn hetzelfde als van een standaard /MODE commando.

OPER_HELP_OLINE
	Gebruik: OLINE gebruiker flags
	
	Stelt Super Admins in staat om Operflags te geven
	aan elke willekeurig gebruiker. Flags moeten worden vooraf
	gegeaan door een "+" of een "-". Om alle flags te verwijderen
	kun je simpelweg een "-" typen in plaats van de flags.

OPER_HELP_CLEARMODES
	Gebruik: CLEARMODES kanaal [ALL]
	
	Verwijdert alle binaire modes (i,k,l,m,n,p,s,t) en bans van
	een kanaal. Als ALL is opgegeven, worden ook alle ops en
	voices (+o en +v modes) van het kanaal verwijderd.

OPER_HELP_KICK
	Gebruik: KICK kanaal gebruiker reden
	
	Stelt de staf in staat om een gebruik van elk willekeurig
	kanaal te kicken. De paramenters zijn hetzelfde als die van
	een standaard /KICK commando. Het kick-bericht zal de nickname
	van de IRCop die het KICK commando uitvoert bevatten.
	Bijvoorbeeld:
	
	*** Gebruiker has been kicked off channel #mijn_kanaal by %S (IRCOp (Misbruik))

OPER_HELP_SVSNICK
	Gebruik: SVSNICK nick nieuwe nick

	Veranderd een gebruiker's nick van nick naar nieuwe nick.
	Gelimiteerd tot Super Admins.

OPER_HELP_AKILL
	Gebruik: AKILL ADD [+verlooptijd] mask reden
	         AKILL DEL {mask | entry-nr | lijst}
	         AKILL LIST [mask | lijst]
	         AKILL VIEW [mask | lijst]
	         AKILL CLEAR
	
	Stelt Services operators in staat de AKILL lijst te beheren.
	Als een gebruiker die overeenkomt met een AKILL mask probeert
	te verbinden zullen Services een KILL voor die gebruiker door-
	voeren en, als het netwerk dit ondersteund, zullen alle servers
	geinstrueerd worden een ban (K-line) voor het betreffende mask
	toe te voegen.
	
	AKILL ADD voeg het gegeven user@host mask toe aan de AKILL
	lijst met de gegeven reden (die moet worden gegeven).
	verlooptijd wordt gespecificeerd als een getal waarachter d 
	(dagen), h (uur), of m (minuten) geplaatst wordt. Combinaties
	(zoals 1h30m) zijn niet toegestaan. Als er geen d, h of m wordt 
	meegegeven, is het standaard dagen (dus +30 zelfstandig betekent
	30 dagen). Om een AKILL die niet verloopt toe te voegen, gebruik je
	+0. Als de usermask die toegevoegd moet worden met een + begint,
	is een verlooptijd verplicht, zelfs als die overeenkomt met de
	standaard verlooptijd. De huidige standaard verlooptijd voor een
	AKILL kan gevonden worden met het STATS AKILL commando.
	
	Het AKILL DEL commando verwijdert het gegeven mask van de AKILL
	lijst als deze erop staat. Als er een lijst van entry-nummers
	is opgegeven zullen die verwijderd worden. (Zie het voorbeeld voor
	LIST hier onder.)
	
	Her AKILL LIST commando geeft de AKILL lijst weer.
	Als een mask met een * is gegeven zullen alleen de AKILLs die
	overeenkomen met het mask worden weergegeven. Als een lijst van
	entry-nummers is gegeven worden alleen die weergegeven.
	Bijvoorbeeld:

	   AKILL LIST 2-5,7-9
	      Geeft AKILLs 2 t/m 5 en 7 t/m 9 weer.
	      
	AKILL VIEW is een versie van AKILL LIST die meer informatie
	geeft: de naam van degene die de AKILL toe heeft gevoegd, de datum
	waarop dit gebeurde, waneer deze verloopt, en de user@host mask
	en de reden.
	
	AKILL CLEAR maakt de AKILL lijst leeg.
	
OPER_HELP_SGLINE
	Gebruik: SGLINE ADD [+verlooptijd] mask reden
	         SGLINE DEL {mask | entry-nr | lijst}
	         SGLINE LIST [mask | lijst]
	         SGLINE VIEW [mask | lijst]
	         SGLINE CLEAR
	
	Stelt Services operators in staat de SGLINE lijst te beheren.
	Als een gebruiker die overeenkomt met een SGLINE mask probeert
	te verbinden zullen Services deze gebruiker niet toestaan zijn
	of haar IRC sessie voort te zetten.
	
	SGLINE ADD voeg het gegeven user@host mask toe aan de SGLINE
	lijst met de gegeven reden (die moet worden gegeven).
	verlooptijd wordt gespecificeerd als een getal waarachter d 
	(dagen), h (uur), of m (minuten) geplaatst wordt. Combinaties
	(zoals 1h30m) zijn niet toegestaan. Als er geen d, h of m wordt 
	meegegeven, is het standaard dagen (dus +30 zelfstandig betekent
	30 dagen). Om een SGLINE die niet verloopt toe te voegen, gebruik je
	+0. Als de usermask die toegevoegd moet worden met een + begint,
	is een verlooptijd verplicht, zelfs als die overeenkomt met de
	standaard verlooptijd. De huidige standaard verlooptijd voor een
	SGLINE kan gevonden worden met het STATS AKILL commando.
	
	Het SGLINE DEL commando verwijdert het gegeven mask van de SGLINE
	lijst als deze erop staat. Als er een lijst van entry-nummers
	is opgegeven zullen die verwijderd worden. (Zie het voorbeeld voor
	LIST hier onder.)
	
	Her SGLINE LIST commando geeft de SGLINE lijst weer.
	Als een mask met een * is gegeven zullen alleen de SGLINEs die
	overeenkomen met het mask worden weergegeven. Als een lijst van
	entry-nummers is gegeven worden alleen die weergegeven.
	Bijvoorbeeld:

	   SGLINE LIST 2-5,7-9
	      Geeft SGLINEs 2 t/m 5 en 7 t/m 9 weer.
	      
	SGLINE VIEW is een versie van SGLINE LIST die meer informatie
	geeft: de naam van degene die de SGLINE toe heeft gevoegd, de datum
	waarop dit gebeurde, waneer deze verloopt, en de user@host mask
	en de reden.
	
	SGLINE CLEAR maakt de SGLINE lijst leeg.

OPER_HELP_SQLINE
	Gebruik: SQLINE ADD [+verlooptijd] mask reden
	         SQLINE DEL {mask | entry-nr | lijst}
	         SQLINE LIST [mask | lijst]
	         SQLINE VIEW [mask | lijst]
	         SQLINE CLEAR
	
	Stelt Services operators in staat de SQLINE lijst te beheren.
	Als een gebruiker met een nick die overeenkomt met een SQLINE
	mask probeert zullen Services deze gebruiker niet toestaan zijn
	of haar IRC sessie voort te zetten.	
	
	Als het eerste teken van een mask een # is, zullen Services
	voorkomen dat overeenkomende kanalen gebruik worden (mits de
	IRC servers het ondersteunen).

	SQLINE ADD voeg het gegeven mask toe aan de SQLINE
	lijst met de gegeven reden (die moet worden gegeven).
	verlooptijd wordt gespecificeerd als een getal waarachter d 
	(dagen), h (uur), of m (minuten) geplaatst wordt. Combinaties
	(zoals 1h30m) zijn niet toegestaan. Als er geen d, h of m wordt 
	meegegeven, is het standaard dagen (dus +30 zelfstandig betekent
	30 dagen). Om een SQLINE die niet verloopt toe te voegen, gebruik je
	+0. Als de usermask die toegevoegd moet worden met een + begint,
	is een verlooptijd verplicht, zelfs als die overeenkomt met de
	standaard verlooptijd. De huidige standaard verlooptijd voor een
	SQLINE kan gevonden worden met het STATS AKILL commando.
	
	Het SQLINE DEL commando verwijdert het gegeven mask van de SQLINE
	lijst als deze erop staat. Als er een lijst van entry-nummers
	is opgegeven zullen die verwijderd worden. (Zie het voorbeeld voor
	LIST hier onder.)
	
	Her SQLINE LIST commando geeft de SQLINE lijst weer.
	Als een mask met een * is gegeven zullen alleen de SQLINEs die
	overeenkomen met het mask worden weergegeven. Als een lijst van
	entry-nummers is gegeven worden alleen die weergegeven.
	Bijvoorbeeld:

	   SQLINE LIST 2-5,7-9
	      Geeft SQLINEs 2 t/m 5 en 7 t/m 9 weer.
	      
	SQLINE VIEW is een versie van SQLINE LIST die meer informatie
	geeft: de naam van degene die de SQLINE toe heeft gevoegd, de datum
	waarop dit gebeurde, waneer deze verloopt, en de mask en de reden.
	
	SQLINE CLEAR maakt de SQLINE lijst leeg.

OPER_HELP_SZLINE
	Gebruik: SZLINE ADD [+verlooptijd] mask reden
	         SZLINE DEL {mask | entry-nr | lijst}
	         SZLINE LIST [mask | lijst]
	         SZLINE VIEW [mask | lijst]
	         SZLINE CLEAR
	
	Stelt Services operators in staat de SZLINE lijst te beheren.
	Als een gebruiker met een IP wat overeenkomt met een SZLINE
	mask probeert te verbinden zullen de Services deze verbieden
	zijn/haar IRC sessie voort te zetten (dit werkt ook als er
	een werkende reverse hostname bij het IP hoort).

	SZLINE ADD voeg het gegeven IP aan de SZLINE lijst toe
	met de gegeven reden (die moet worden gegeven).
	verlooptijd wordt gespecificeerd als een getal waarachter d 
	(dagen), h (uur), of m (minuten) geplaatst wordt. Combinaties
	(zoals 1h30m) zijn niet toegestaan. Als er geen d, h of m wordt 
	meegegeven, is het standaard dagen (dus +30 zelfstandig betekent
	30 dagen). Om een SZLINE die niet verloopt toe te voegen, gebruik je
	+0. Als de usermask die toegevoegd moet worden met een + begint,
	is een verlooptijd verplicht, zelfs als die overeenkomt met de
	standaard verlooptijd. De huidige standaard verlooptijd voor een
	SZLINE kan gevonden worden met het STATS AKILL commando.

	Het SZLINE DEL commando verwijdert het gegeven mask van de
	SZLINE lijst als deze erop staat. Als een lijst van entry-nummers
	is gegeven, zullen die verwijderd worden. (Zie het voorbijbeeld
	bij LIST hier onder.)
	
	Het SZLINE LIST commando geeft de SZLINE lijst weer. Als er
	een wildcard mask is gegeven zullen alleen de overeenkomende
	masks worden weergegeven. Als een lijst van entry-nummers is
	gegeven zullen alleen die worden weergegeven. Bijvoorbeeld:

	   SZLINE LIST 2-5,7-9
	      Geeft SZLINE 2 t/m 5 en 7 t/m 9 weer.
	      
	SZLINE VIEW is een meer verbale versie van SZLINE LIST,
	en geeft weer wie een SZLINE heeft toegevoegd, op welke datum
	hij/zij dat heeft gedaan, wanneer deze verloopt, en het IP
	adres en de reden.
	
	SZLINE CLEAR maakt de SZLINE lijst leeg

OPER_HELP_SET
	Gebruik: SET optie instelling
	
	Stelt verschillende globale Services opties in. Momenteel
	zijn de volgende opties beschikbaar:
	    READONLY   Stel alleen-lezen of lezen-schrijven mode in
	    LOGCHAN    Rapporteer log-berichten naar een kanaal
	    DEBUG      (De)activeer debug mode
	    NOEXPIRE   (De)activeer de geen-verloop mode
	    SUPERADMIN (De)activeer SuperAdministrator mode
	    SQL   	 Activate or deactivate sql mode
	    IGNORE     Activate or deactivate ignore mode
	    LIST       List the options

OPER_HELP_SET_READONLY
	Gebruik: SET READONLY {ON | OFF}
	
	Zet de alleen-lezen mode aan of uit. In alleen-lezen mode
	zullen normale gebruikers niet in staat zijn data van de
	Services te wijzigen, inclusief kanaal toegangslijsten,
	nickname alias lijst, etc. IRCops met voldoende Services
	rechten kunnen de Services' AKILL lijst wijzigen, en nicknames
	en kanalen droppen of verbieden, maar deze veranderingen
	zullen niet worden opgeslagen behalve als de alleen-leze mode
	wordt uitgeschakeld voordat de Services uitgezet of opnieuw
	gestart worden.
	
	Deze optie komt overeen met de commandline-optie -readonly.

OPER_HELP_SET_LOGCHAN
	Gebruik: SET LOGCHAN {ON | OFF}
	
	Wanneer deze optie aan staat zullen Services de log-berichten
	naast de log-bestanden ook naar een gespecificeerd kanaal
	sturen. LogChannel moet ik ook gedefineerd zijn in het Services
	configuratie bestand om deze instelling bruikbaar te laten zijn.
	
	Deze optie komt overeen met de commandline-optie -logchan.
	
	Let op: Dit kan grote beveiligingsimplicaties met zich mee
	bregen als het log-kanaal niet goed beveiligd is.

OPER_HELP_SET_DEBUG
	GEBRUIK: SET DEBUG {ON | OFF | nummer}
	
	Zet debug mode aan of uit. In debug mode zal alle data die
	naar en van Services verzonden worden, alsmede een aantal
	debug-berichten, naar het log-bestand geschreven worden.
	Als nummer is gegeven zal debug mode worden geactiveerd met
	het debug-niveau op nummer

	Deze optie komt overeen met de commandline-optie -debug.
	
OPER_HELP_SET_NOEXPIRE
	Gebruik: SET NOEXPIRE {ON | OFF}
	
	Zet de geen-verloop mode aan of uit. In de geen-verloop mode
	zullen nicks, kanalen, AKILLs en uitzonderingen niet verlopen
	totdat de geen-verloop mode weer wordt uitgezet.
	
	Deze optie komt overeen met de commandline-optie -noexpire.

OPER_HELP_SET_SUPERADMIN
	Gebruik: SET SUPERADMIN {ON | OFF}
	
	Als je dit aan zet krijg je extra privileges, zo ben je dan
	bijvoorbeeld stichter op alle kanalen, etc...
	
	Deze optie is niet persistent, en moet alleen gebruikt worden
	wanneer het echt nodig is, en terug op OFF gezet worden als het
	niet meer nodig is.

OPER_HELP_SET_SQL
	Gebruik: SET SQL {ON | OFF}
	
	Deze instelling schakelt Anope's SQL gebruik. Zo kan al het SQL
	verkeer worden uitgezet als de SQL server onbereikbaar wordt
	terwijl services nog draaien.

OPER_HELP_SET_IGNORE
	Syntax: SET IGNORE {ON | OFF}

	Setting this will toggle Anope's usage of the IGNORE system 
	on or off.

OPER_HELP_SET_LIST
	Gebruik: SET LIST
	
	Geef de verschillende %S instellingen weer.

OPER_HELP_NOOP
	Gebruik: NOOP SET server
	         NOOP REVOKE server
	
	NOOP SET verwijder alle O:lines van de gegeven server
	en /KILLt alle IRCops die momenteel verbonden zijn met de
	server om te voorkomen dat ze de server rehashen, omdat
	dit het effect weer ongedaan zou maken.
	
	NOOP REVOKE maakt alle verwijderde O:lines op de gegeven
	server weer beschikbaar.
	
	Pas op: De server wordt niet gecontroleerd door Services.

OPER_HELP_JUPE
	GEBRUIK: JUPE server [reden]
	
	Zorgt ervoor dat Services de opgegeven server "jupiteren".
	Dit betekent dat er een nep "server" die verbonden is aan
	Services gemaakt wordt, die voorkomt dat de echte server met
	die naam verbindt. De JUPE kan worden verwijderd met een
	standaard SQUIT commando. Als een reden is gegeven, wordt
	deze in het server informatie veld geplaatsd, anders zal het
	informatie veld de text "Juped by <nick>" bevatten, waar
	<nick> vervangen wordt door de <nick> die de server JUPEt.

OPER_HELP_RAW
	Gebruik: RAW text
	
	Stuurt text direct naar de server waarmee Services verbonden
	zijn. Dit commando heeft een beperkt aantal mogelijkheden, en
	kan voor chaos zorgen op een netwerk als het onjuist gebruikt
	wordt. GEBRUIK DIT COMMANDO NIET tenzij je absoluut zeker
	bent van wat je doet!

OPER_HELP_UPDATE
	Gebruik: UPDATE
	
	Zorgt ervoor dat Services de database bestanden op de harde
	schijf bijwerken zodra je dit commando vertsuurt.
	
OPER_HELP_RELOAD
	Gebruik: RELOAD
	
	Zorgt ervoor dat Services het configuratie-bestand opnieuw
	inladen. Let er wel op dat sommige instellingen nog steeds
	vereisen dat de Services opnieuw gestart worden (bijvoorbeeld
	Services' nicknames, activatie van de sessie limiet, etc.)

OPER_HELP_QUIT
	Gebruik: QUIT
	
	Zorgt ervoor dat Services meteen afsluiten: databases worden
	niet opgeslagen. Dit commando moet niet gebruikt worden
	tenzij schade aan de kopieen van de databases in het geheugen
	wordt gevreesd en ze niet moeten worden opgeslagen. Voor
	normaal afsluiten moet je het SHUTDOWN commando gebruiken.

OPER_HELP_SHUTDOWN
	Gebruik: SHUTDOWN
	
	Zorgt ervoor dat Services alle databases opslaat en dan
	afsluit.

OPER_HELP_RESTART
	Gebruik: RESTART
	
	Zorgt ervoor dat Services alle databases opslaat en dan
	opnieuw opstart.

OPER_HELP_CHANLIST
	Gebruik: CHANLIST [{trefbeeld | nick} [SECRET]]
	
	Geeft alle kanalen weer die momenteel in gebruik zijn op het
	IRC netwerk, ook de kanalen die niet geregistreerd zijn.
	
	Als trefbeeld is gegeven, worden alleen overeenkomende
	kanalen weergegeven. Als een nick is gegeven worden
	alleen de kanalen waar de betreffende gebruiker aanwezig is
	weergegeven. Als SECRET is gegeven worden alleen kanalen
	die met trefbeeld overeenkomen en +s of +p mode hebben.

OPER_HELP_USERLIST
	Gebruik: USERLIST [{trefbeeld | kanaal} [INVISIBLE]]
	
	Geeft alle gebruikers weer die momenteel online zijn op het
	IRC netwerk, ook degenen die niet geregistreerd zijn.
	
	Als trefbeeld is gegeven worden alleen overeenkomende
	gebruikers weergegeven (het moet in het nick!user@host
	formaat gegeven worden. Als kanaal is gegeven worden
	alleen gebruikers die op het betreffende kanaal zijn
	weergegeven. Als INVISBLE is gespecifificeerd worden alleen
	gebruikers met gebruikersmode +i weergegeven.

OPER_HELP_MODLOAD
	Gebruik: MODLOAD bestandsnaam
	
	Dit commando laadt de module genaamd bestandsnaam uit de
	modules directory.

OPER_HELP_MODUNLOAD
	Gebruik: MODUNLOAD bestandsnaam

	Dit commando ontlaadt de module genaamd bestandsnaam uit
	de modules directory.

OPER_HELP_MODINFO
	Gebruik: MODINFO bestandsnaam

	Dit command geeft informatie weer over de gespecificeerde
	geladen module.

OPER_HELP_MODLIST
	Gebruik: MODLIST [Core|3rd|protocol|encryption|supported|qatested]

	Geeft alle momenteel geladen modules weer.


###########################################################################
#
# BotServ help messages
#
###########################################################################


BOT_HELP_CMD_BOTLIST
	BOTLIST            Geef beschikbare bots weer
BOT_HELP_CMD_ASSIGN
	ASSIGN             Wijs een bot toe aan een kanaal
BOT_HELP_CMD_SET
	SET                Configureer bot opties
BOT_HELP_CMD_KICK
	KICK               Configureer kickers
BOT_HELP_CMD_BADWORDS
	BADWORDS           Beheer slechte-woorden lijst
BOT_HELP_CMD_ACT
	ACT                Makes the bot do the equivalent of a "/me" command
BOT_HELP_CMD_INFO
	INFO               Allows you to see BotServ information about a channel or a bot
BOT_HELP_CMD_SAY
	SAY                Makes the bot say the given text on the given channel
BOT_HELP_CMD_UNASSIGN
	UNASSIGN           Unassigns a bot from a channel
BOT_HELP_CMD_BOT
	BOT                Beheer de lijst met bots op het netwerk

BOT_HELP
	%S stelt je in staat een bot op je eigen kanaal te hebben.
	Het is gemaakt voor gebruikers die geen eigen bot kunnen
	hosten of configureren, of voor gebruik op netwerken die
	geen bots van gebruikers toestaan. Beschikbare commando's
	worden hieronder weergegeven. Om ze te gebruiken moet je
	%R%S commando typen. Voor meer informatie over een
	specifiek commando moet je %R%S HELP command typen.
	
BOT_HELP_FOOTER
	Bot zal een kanaal joinen wanneer daar tenminste %d
	gebruiker(s) is/zijn.
	
BOT_HELP_BOTLIST
	Gebruik: BOTLIST
	
	Geeft alle beschikbare bots op dit netwerk weer.
	
BOT_HELP_ASSIGN
	Gebruik: ASSIGN kanaal nick
	
	Wijst een bot met de nick nick toe aan kanaal kanaal.
	Hierna kun je de bot voor het kanaal instellen zoals je
	wil.

BOT_HELP_UNASSIGN
	Gebruik: UNASSIGN kanaal
	
	Verwijdert een bot van een kanaal. Wanneer je dit commando
	gebruikt zal de bot niet meer in het kanaal komen. De
	configuratie van de bot voor het kanaal wordt wel behouden,
	zodat je later de bot altijd opnieuw aan het kanaal kun
	toewijzen zonder dat je deze opnieuw hoeft te configureren.

BOT_HELP_INFO
	Gebruik: INFO {kanaal | nick}
	
	Stelt je in staat %S informatie te zien over een kanaal
	of een bot. Als het paramenter een kanaal is krijg je bijvoorbeeld
	informatie over kickers. Als het paramenter een nick is krijg
	je informatie over een bot, bijvoorbeeld de tijd waarop deze
	gemaakt is of op hoeveel kanalen deze is.

BOT_HELP_SET
	Gebruik: SET kanaal optie parameters
	
	Stel bot opties in. optie kan zijn:
	
	    DONTKICKOPS      Om ops te beschermen tegen bot kicks
	    DONTKICKVOICES   Om voices te beschermen tegen bot kicks
	    GREET            Zet groet-berichten aan
	    FANTASY          Zet fantasie-commando's aan
	    SYMBIOSIS        Stel de bot in staat te handelen als een
	                         echte bot.
	
	Type %R%S HELP SET optie voor meer informatie over een
	specifieke optie.
	
	Let op: toegang tot dit commando wordt geregeld via het
	niveau SET commando.
	
BOT_HELP_SET_DONTKICKOPS
	Gebruik: SET kanaal DONTKICKOPS {ON|OFF}
	
	(De)activeer ops protectie mode op een kanaal.
	Wanneer deze geactiveerd is zullen ops niet door de bot
	gekicked worden, zelfs wanneer deze niet overeen komen
	met het NOKICK niveau.

BOT_HELP_SET_DONTKICKVOICES
	Gebruik: SET kanaal DONTKICKVOICES {ON|OFF}
	
	(De)activeer voices protectie mode op een kanaal.
	Wanneer deze geactiveerd is zullen voices niet door de bot
	gekicked worden, zelfs wanneer deze niet overeen komen
	met het NOKICK niveau.

BOT_HELP_SET_FANTASY
	Gebruik: SET kanaal FANTASY {ON|OFF}
	
	(De)activeert fantasie mode op een kanaal.
	Wanner het geactiveerd is zullen users de !op, !deop,
	!voice, !devoice, !kick, !kb, !unban, en !seen commando's
	op een kanaal kunnen gebruiken (ontdek hoe je ze moet
	gebruiken; probeer ze allemaal met een nick, en misschien
	sommigen met een reden?).
	
	Let op dat de gebruikers die de fantasie-commando's willen
	gebruiken een hoog genoeg niveau MOETEN hebben voor zowel
	de FANTASIA en het niveau voor het commando indien vereisd.
	Bijvoorbeeld: om !op te mogen gebruiken moet een gebruiken
	genoeg rechten voor het OPDEOP niveau hebben.

BOT_HELP_SET_GREET
	Gebruik: SET kanaal GREET {ON|OFF}
	
	(De)activeert greet mode op een kanaal.
	Wanneer deze geactiveerd is zal de bot de groetberichten
	van de gebruikers die het kanaal joinen weergegeven, mits
	ze hiertoe rechten hebben op het kanaal.

BOT_HELP_SET_SYMBIOSIS
	Gebruik: SET kanaal SYMBIOSIS {ON|OFF}
	
	(De)activeert symbiosis mode op een kanaal.
	Wanneer dit is geactiveerd zal de bot alles doen wat normaal
	foor %s wordt gedaan bij kanalen, zoals MODEs, KICKs, en
	zelfs het welkomstbericht.

BOT_HELP_KICK
	Gebruik: KICK kanaal optie parameters
	
	Configureer bot kickers. optie kan zijn:
	
	    BOLDS         Stelt in of de bot kickt op vet
	    BADWORDS      Stelt in of de bot kickt op slechte woorden
	    CAPS          Stelt in of de bot kickt op hoofdletters
	    COLORS        Stelt in of de bot kicks op kleuren
	    FLOOD         Stelt in of de bot floodende gebruikers kickt
	    REPEAT        Stelt in of de bot gebruikers kickt die
	                       zichzelf herhalen
	    REVERSES      Stelt in of de bot kickt op inverteringen
	    UNDERLINES    Stelt in of de bot kickt op onderstrepingen
	
	Type %R%S HELP KICK optie voor meer informatie over
	een specifieke optie.
	
	Let op: toegang tot dit commando wordt gecontroleerd door
	het niveau SET commando.

BOT_HELP_KICK_BOLDS
	Gebruik: KICK kanaal BOLDS {ON|OFF} [ttb]

	Zet de vet-kicker aan of uit. Wanneer deze aan staat zal
	de bot gebruikers kicken die vet gebruiken.

	ttb is het aantal keren dat een gebruiker gekicked kan
	worden voordat deze gebanned wordt. Geef geen ttb op om het
	ban system uit te schakelen als deze kicker geactiveerd is.

BOT_HELP_KICK_COLORS
	Gebruik: KICK kanaal COLORS {ON|OFF} [ttb]

	Zet de kleuren-kicker aan of uit. Wanneer deze aan staat
	zal de bot gebruikers kicken die kleuren gebruiken.
	
	ttb is het aantal keren dat een gebruiker gekicked kan
	worden voordat deze gebanned wordt. Geef geen ttb op om het
	ban system uit te schakelen als deze kicker geactiveerd is.

BOT_HELP_KICK_REVERSES
	Gebruik: KICK kanaal REVERSES {ON|OFF} [ttb]
	
	Zet de inverteringen-kicker aan of uit. Wanneer deze aan
	staat zal de bot gebruikers kicken die inverteringen
	gebruiken.
	
	ttb is het aantal keren dat een gebruiker gekicked kan
	worden voordat deze gebanned wordt. Geef geen ttb op om het
	ban system uit te schakelen als deze kicker geactiveerd is.

BOT_HELP_KICK_UNDERLINES
	Gebruik: KICK kanaal UNDERLINES {ON|OFF} [ttb]
	
	Zet de onderstrepingen-kicker aan of uit. Wanneer deze aan
	staat zal de bot gebruikers kicken die onderstrepingen
	gebruiken.
	
	ttb is het aantal keren dat een gebruiker gekicked kan
	worden voordat deze gebanned wordt. Geef geen ttb op om het
	ban system uit te schakelen als deze kicker geactiveerd is.

BOT_HELP_KICK_CAPS
	Gebruik: KICK kanaal CAPS {ON|OFF} [ttb [min [procent]]]
	
	Zet de hoofdletter-kicker aan of uit. Wanneer deze aan staat
	zal de bot gebruikers kicken die HOOFDLETTERS gebruiken.
	
	De bot kickt alleen als er minstens min hoofdletters in
	de zin staan en deze minstens procent%% van de totale
	regel bevatten (indien niet gegeven, staat dit standaard op
	10 hoofdletters en 25%%).

	ttb is het aantal keren dat een gebruiker gekicked kan
	worden voordat deze gebanned wordt. Geef geen ttb op om het
	ban system uit te schakelen als deze kicker geactiveerd is.

BOT_HELP_KICK_FLOOD
	Gebruik: KICK kanaal FLOOD {ON|OFF} [ttb [regels [seconden]]]
	
	Zet de flood-kicker aan of uit. Wanneer deze aan staat zal
	de bot gebruikers kicken die het kanaal flooden met minstens
	regels regels in seconden seconden.	(indien niet gegeven,
	staat dit standaard op 6 regels in 10 seconden).
	
	ttb is het aantal keren dat een gebruiker gekicked kan
	worden voordat deze gebanned wordt. Geef geen ttb op om het
	ban system uit te schakelen als deze kicker geactiveerd is.

BOT_HELP_KICK_REPEAT
	Gebruik: KICK kanaal REPEAT {ON|OFF} [ttb [nr]]
	
	Zet de herhalings-kicker aan of uit. Wanneer deze aan staat
	zal de bot gebruikers kicken die zichzelf nr keer herhalen
	(als nr niet is gegeven staat die standaard op 3).

	ttb is het aantal keren dat een gebruiker gekicked kan
	worden voordat deze gebanned wordt. Geef geen ttb op om het
	ban system uit te schakelen als deze kicker geactiveerd is.

BOT_HELP_KICK_BADWORDS
	Gebruik: KICK kanaal BADWORDS {ON|OFF} [ttb]
	
	Zet de slechte-woorden-kicker aan of uit. Wanneer deze aan
	staat zal de bot gebruikers kicken die bepaald slechte
	woorden op het kanaal zeggen.
	
	Je kan de slechte woorden voor je kanaal instellen met het
	BADWORDS commando. Type %R%S HELP BADWORDS voor
	meer informatie.

	ttb is het aantal keren dat een gebruiker gekicked kan
	worden voordat deze gebanned wordt. Geef geen ttb op om het
	ban system uit te schakelen als deze kicker geactiveerd is.

BOT_HELP_BADWORDS
	Gebruik: BADWORDS kanaal ADD woord [SINGLE | START | END]
	         BADWORDS kanaal DEL {woord | entry-nr | lijst}
	         BADWORDS kanaal LIST [mask | lijst]
	         BADWORDS kanaal CLEAR
	
	Beheert de slechte woorden lijst voor een kanaal. De
	slechte woorden lijst bepaalt voor welke woorden wordt
	gekicked als de slechte-woorden-kicker aan staat. Voor
	meer informatie, type %R%S HELP KICK BADWORDS.
	
	Het BADWORDS ADD commando voegt het gegeven woord toe aan
	de slechte woorden lijst. Als SINGLE is gespecificeerd zal
	alleen worden gekicked als de gebruiker het gehele woord zegt.
	Als START is gespecificeerd zal allen gekicked worden als de
	gebruiker een woord zegt dat begint met woord. Als END is
	gespecificeerd zal alleen gekicked worden als de gebruiker
	een woord zegt dat eindigt op woord. Als er niks
	gespecificeerd is zal er elke keer dat woord gezegd wordt
	door een gebruiker gekicked worden.
	
	Het BADWORDS DEL commando verwijdert het gegeven woord van
	de slechte woorden lijst. Als een lijst van entry-nummers is
	gegeven worden die verwijderd. (Zie het voorbeeld bij LIST
	hier onder.)
	
	Het BADWORDS LIST commando geeft de slechte woorden lijst
	weer. Als een mask met een * is gegeven worden alleen de
	slechte woorden die overeenkomen met het mask weergegeven.
	Als een lijst van entry-nummers is gegeven worden alleen
	die weergegeven. Bijvoorbeeld:
	
	   BADWORDS #kanaa; LIST 2-5,7-9
	      Geeft slechte woorden 2 t/m 5 en 7 t/m 9 weer.
	
	Het BADWORDS CLEAR commando maakt de slechte woorden
	lijst leeg.

BOT_HELP_SAY
	Gebruik: SAY kanaal tekst
	
	Laat de bot de gegeven tekst op het gegeven kanaal zeggen.

BOT_HELP_ACT
	Gebruik: ACT kanaal tekst
	
	Laat de bot een "/me" commando doen op het gegeven kanaal
	met de gegeven tekst.

BOT_SERVADMIN_HELP_BOT
	Gebruik: BOT ADD nick gebruiker host echte naam
	         BOT CHANGE oldnick newnick [gebruiker [host [echte naam]]]
	         BOT DEL nick
	
	Stelt Services administrators in staat om bots te maken,
	wijzigen en verwijderen die gebruikers op hun eigen kanaal
	kunnen gebruiken.
	
	BOT ADD voegt een bot toe met de gegeven nickname,
	gebruikersnaam, host en echte naam. De paramenters worden
	niet nagekeken of ze correct zijn, dus pas op.
	BOT CHANGE kan de nick, gebruikersnaam, host of echte
	naam van een bot veranderen zonder deze (en de date die
	met de bot geassocieerd is) te verwijderen.
	BOT DEL verwijdert de gegeven bot van de bot lijst.
	
	Let op: Als je een bot maakt die een nick heeft die momenteel
	in gebruiker is zal de betreffende gebruiker worden gekilled.
	Als de nick geregistreerd is, wordt deze gedropped.

BOT_SERVADMIN_HELP_SET
	
	Deze opties zijn gereserveerd voor Services administrators:
	
	    NOBOT            Zorg dat een bot niet aan een kanaal kan
	                        worden toegevoegd
	    PRIVATE          Zorg dat een bot alleen door IRC operators
	                        kan worden toegevoegd

BOT_SERVADMIN_HELP_SET_NOBOT
	Gebruik: SET kanaal NOBOT {ON|OFF}
	
	Deze optie zorgt ervoor dat een kanaal geen bot kan hebben.
	Als er al een bot aan het kanaal is toegewezen wordt deze
	automatisch van het kanaal gehaald wanneer je deze optie
	aan zet.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Gebruik: SET bot-nick PRIVATE {ON|OFF}
	
	Deze optie szorgt ervoor dat een bot niet kan worden
	toegewezen aan een kanaal door gebruikers die geen
	IRC operator zijn.

###########################################################################
#
# HostServ messages
#
###########################################################################
HOST_EMPTY
	De vhost lijst is leeg.
HOST_ENTRY
	#%d Nick:%s, vhost:%s (%s - %s)
HOST_IDENT_ENTRY
	#%d Nick:%s, vhost:%s@%s (%s - %s)
HOST_SET
	vhost voor %s gezet op %s.
HOST_IDENT_SET
	vhost voor %s gezet op %s@%s.
HOST_SETALL
	vhost voor groep %s gezet op %s.
HOST_DELALL
	vhosts voor group %s zijn verwijderd.
HOST_DELALL_SYNTAX
	DELALL <nick>.
HOST_IDENT_SETALL
	vhost voor groep %s gezet op %s@%s.
HOST_SET_ERROR
	Een vhost moet in een geldig hostmask formaat zijn.
HOST_SET_IDENT_ERROR
	Een vhost ident moet in een geldig ident formaat zijn.
HOST_SET_TOOLONG
	Fout! De vhost is te lang. Gebruik een host die korter is dan %d tekens.
HOST_SET_IDENTTOOLONG
	Fout! De ident is te lang. Gebruik een ident die korter is dan %d tekens.
HOST_NOREG
	Gebruiker %s niet gevonden in de nickserv database.
HOST_SET_SYNTAX
	SET <nick> <hostmask>.
HOST_SETALL_SYNTAX
	SETALL <nick> <hostmask>.
HOST_DENIED
	Toegang geweigerd.
HOST_NOT_ASSIGNED
	Neem contact op met een Operator om een vhost aan deze nick te binden.
HOST_ACTIVATED
	Je vhost van %s is nu geactiveerd.
HOST_IDENT_ACTIVATED
	Je vhost van %s@%s is nu geactiveerd.
HOST_ID
	Identificeer eerst bij Services.
HOST_NOT_REGED
	Je moet je eerst registreren voordat een vhost aan je nick kan worden gebonden.
HOST_DEL
	vhost voor %s verwijderd.
HOST_DEL_SYNTAX
	DEL <nick>.
HOST_OFF
	Your vhost was removed and the normal cloaking restored.
HOST_OFF_UNREAL
	Je vhost is verwijderd. Om standaard host verberging weer aan te schakelen, type je /mode %s +%s
HOST_NO_VIDENT
	Je IRCD ondersteunt geen vIdent's. Als dit niet klopt, rapporteer dit dan als een mogelijke bug
HOST_GROUP
	Alle vhosts in de groep %s zijn ingesteld op %s
HOST_IDENT_GROUP
	Alle vhosts in de groep %s zijn ingesteld op %s@%s

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_LIST_FOOTER
	Alle records zijn weergegeven (Aantal: %d)
HOST_LIST_RANGE_FOOTER
	Alle records getoond van %d tot %d
HOST_LIST_KEY_FOOTER
	Alle records getoond met overeenkomende key %s (Aantal: %d)

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_HELP_CMD_ON
	    ON          Activeert je toegewezen vhost

HOST_HELP_CMD_OFF
	    OFF         Deactiveert je toegewezen vhost

HOST_HELP_CMD_GROUP
	    GROUP       Stelt de vhost voor all nicks in een groep gelijk

HOST_HELP_CMD_SET
	    SET         Stel de vhost van een andere gebruiker in

HOST_HELP_CMD_SETALL
	    SETALL      Stel de vhost voor alle nicks in een groep in

HOST_HELP_CMD_DEL
	    DEL         Verwijder de vhost van een andere gebruiker

HOST_HELP_CMD_DELALL
	    DELALL      Verwijder de vhost voor alle nicks in een groep

HOST_HELP_CMD_LIST
	    LIST        Geeft een of meer vhosts weer.

HOST_OPER_HELP
	not used.

HOST_ADMIN_HELP
	not used.

HOST_HELP
	%S commando'ss:

HOST_HELP_ON
	Gebruik: ON
	
	Activeert de vhost die momenteel toegewezen is aan de nick
	die je gebruikt. Als je dit commando uitvoert zal elke
	gebruiker die een /whois op je doet de vhost zien in plaats
	van je echte hostname.

HOST_HELP_SET
	Gebruik: SET <nick> <hostmask>.
	
	Stelt de vhost voor de gegeven nick in op de vhost die je
	meegegeven heeft. Als de IRCD vIdent's ondersteund kun je
	ook SET <nick> <ident>@<hostmask> gebruiken om de ident
	voor een gebruiker aan te passen gelijk met de hostname.

HOST_HELP_DELALL
	Syntax: DELALL <nick>.

	Verwijderd de vhost voor alle nick's in dezelfde groep
	als de gegeven nick.


HOST_HELP_SETALL
	Gebruik: SETALL <nick> <hostmask>.
	
	Stelt de vhost voor alle nicks in dezelfde groep als de
	gegeven nick in. Als de IRCD vIdent's ondersteund kun je
	ook SET <nick> <ident>@<hostmask> gebruiken om de ident
	voor gebruikers aan te passen gelijk met de hostname.
	
	* LET OP, dit wijzigt niet de vhost voor elke nick lid
	wordt van de groep nadat dit commando is gebruikt.

HOST_HELP_OFF
	Gebruik: OFF
	
	Deactiveert de vhost die moementeel toegewezen is aan de nick
	die je gebruikt. Als je dit commando uitvoert zal elke
	gebruiker die een /whois op je doet je echte hostname zien
	in plaats van je vhost.

HOST_HELP_DEL
	Gebruik: DEL <nick>
	
	Verwijdert de vhost die toegewezen is aan de gegeven nick
	uit de database.

HOST_HELP_LIST
	Gebruik: LIST [<nick>|<vhost>]
	
	Dit commando geeft alle geregistreerde vhosts weer. Als je
	een nick of vhost meegeeft zulleen alleen die nick/vhost
	worden weergegeven. LET OP: er is momenteel geen flood-
	bescherming voor grote databases.

HOST_HELP_GROUP
	Gebruik: GROUP
	
	Dit commando stelt gebruikers in staat de vhost van hun
	HUIDIGE nick de vhost van alle nicks in de zelfde groep te
	laten zijn.

OPER_SVSNICK_UNSUPPORTED
	Sorry, SVSNICK is niet beschikbaar op dit netwerk.

OPER_SQLINE_UNSUPPORTED
	Sorry, SQLINE is niet beschikbaar op dit netwerk.

OPER_SVSO_UNSUPPORTED
	Sorry, OLINE is niet beschikbaar op dit netwerk.

OPER_UMODE_UNSUPPORTED
	Sorry, UMODE is niet beschikbaar op dit netwerk.

OPER_SUPER_ADMIN_NOT_ENABLED
	SuperAdmin instelling is niet ingeschakeld in services.conf

