# Catalan Language File.
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
# order of per ameters for sprintf() is fixed in the source code, so your
# messages need to take the same per ameters in the same order as the
# English messages do.  (Obviously, this doesn't hold for the strftime()
# format lines immediately below.)  If you can't get a natural translation
# of a message without changing the order of the parameters, let us know
# (team@anope.org) which message is causing a problem and I'll see
# what I can do.
#
# In help messages, "%S" (capital S, not lowercase) refers to the name of
# the service sending the message; for example, in NickServ help messages,
# "%S" is replaced by "NickServ" (or whatever it is renamed to in
# services.conf).  The %S's do not count as sprintf() per ameters, so they can be
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
# Translated by AUJAC (Associació d'Usuaris de Java de Catalunya. http://www.aujac.org)
LANG_NAME
	Catala (Catalan)

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
	%d %b %H:%M:%S %Y %Z
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
	Diu
	Dll
	Dmt
	Dmc
	Dj
	Dv
	Dsb
# %A
STRFTIME_DAYS_LONG
	Diumenge
	Dilluns
	Dimarts
	Dimecres
	Dijous
	Divendres
	Dissabte
# %b
STRFTIME_MONTHS_SHORT
	Gen
	Feb
	Mar
	Abr
	Mai
	Jun
	Jul
	Ago
	Sep
	Oct
	Nov
	Des
# %B
STRFTIME_MONTHS_LONG
	Gener
	Febrer
	Març
	Abril
	Maig
	Juny
	Juliol
	Agost
	Setembre
	Octubre
	Novembre
	Desembre


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	, 


# Various error messages.
USER_RECORD_NOT_FOUND
	Error Intern - Impossible processar petició.
UNKNOWN_COMMAND
	Comanda no coneguda %s.
UNKNOWN_COMMAND_HELP
	Comanda no coneguda %s. "%R%s HELP" per ajuda.
SYNTAX_ERROR
	Sintàxis: %s
MORE_INFO
	%R%s HELP %s per més informació.
NO_HELP_AVAILABLE
	No existeix ajuda per %s.
OBSOLETE_COMMAND
	Aquesta comanda esta obsoleta; utilitzi %s en substitució.

BAD_USERHOST_MASK
	La mascara ha de ser de la forma usuari@host.
BAD_EXPIRY_TIME
	Temps d'expiració invàlid.
USERHOST_MASK_TOO_WIDE
	%s La cobertura es mes amplia; Per favor utilitzi una mascara mes especifica.

SERVICE_OFFLINE
	%s esta momentaneament offline.
READ_ONLY_MODE
	Noticia: Els serveis estan en mode read-only; Els canvis no seran guardats!
PASSWORD_INCORRECT
	Clau incorrecta.
INVALID_TARGET
	"/msg %s" is no longer supported.  Use "/msg %s@%s" or "/%s" instead.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	accés denegat.
PERMISSION_DENIED
	Permis denegat.
RAW_DISABLED
	The RAW option has been disabled. If you must use it, enable the DisableRaw directive in Services configuration.

MORE_OBSCURE_PASSWORD
	Per favor intenti novamente amb una clau mes oculta. Les claus han de ser de com a mínim 5 caràcters, no han de ser fàcils d'encertar (ex. Nom real o Nick), i no poden contenir el caràcter d'espai o tabulació.
PASSWORD_TOO_LONG
	Your password is too long. Please try again with a shorter password.

NICK_NOT_REGISTERED
	El teu nick no esta registrat.
NICK_NOT_REGISTERED_HELP
	El teu nick no esta registrat. Escriu %R%s HELP per mes informació al registrat el teu nickname.
NICK_X_IS_SERVICES
	Nick %s is part of this Network's Services.
NICK_X_NOT_REGISTERED
	El nick %s no esta registrat.
NICK_X_IN_USE
	Nick %s is currently in use.
NICK_X_NOT_IN_USE
	El nick %s no esta en us.
NICK_X_NOT_ON_CHAN
	El nick %s no esta en el canal %s actualment.
NICK_X_FORBIDDEN
	El nick %s no pot ser registrat o utilitzat.
NICK_X_FORBIDDEN_OPER
	El nick %s ha estat prohibit per %s:
	%s
NICK_X_ILLEGAL
	Nick %s is an illegal nickname and cannot be used.
NICK_X_TRUNCATED
	Nick %s was truncated to %d characters.
NICK_X_SUSPENDED
	Nick %s is currently suspended.
CHAN_X_NOT_REGISTERED
	El Canal %s no esta registrat.
CHAN_X_NOT_IN_USE
	El Canal %s No existeix.
CHAN_X_FORBIDDEN
	El Canal %s no pot ser registrat o usat.
CHAN_X_FORBIDDEN_OPER
	El Canal %s ha estat prohibit per %s:
	%s
CHAN_X_SUSPENDED
	      Suspended: [%s] %s
NICK_IDENTIFY_REQUIRED
	Autentificació per clau requerida per aquesta comanda.
	Intenta un altre cop després de escriure %R%s IDENTIFY clau.
CHAN_IDENTIFY_REQUIRED
	Autentificació per clau requerida per aquesta comanda.
	Intenta de nou després d'escriure %R%s IDENTIFY %s clau.
	
MAIL_DISABLED
	Els serveis han estat configurats per no enviar email
MAIL_INVALID
	E-mail per %s es invalid.
MAIL_X_INVALID
	%s no es un e-mail valid.
MAIL_LATER
	Imposible enviar email ara; per favor intenta en uns instants.
MAIL_DELAYED
	Per favor espera %d segons i intenta un altre cop.
	
NO_REASON
	Sense raó
UNKNOWN
	<desconegut>

# Duration system
DURATION_DAY
	1 dia
DURATION_DAYS
	%d dies
DURATION_HOUR
	1 hora
DURATION_HOURS
	%d hores
DURATION_MINUTE
	1 minut
DURATION_MINUTES
	%d minuts
DURATION_SECOND
	1 segon
DURATION_SECONDS
	%d segons

# Human readable expiration
NO_EXPIRE
	no expira
EXPIRES_SOON
	expira amb el pròxim update de la base de dades
EXPIRES_M
	expira en %d minuts
EXPIRES_1M
	expira en %d minuts
EXPIRES_HM
	expira en %d hores, %d minuts
EXPIRES_H1M
	expira en %d hores, %d minut
EXPIRES_1HM
	expira en %d hora, %d minuts
EXPIRES_1H1M
	expira en %d hora, %d minut
EXPIRES_D
	expira en %d dies
EXPIRES_1D
	expira en %d dia

# Generic Footer message
END_OF_ANY_LIST
	End of %s list.

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
LIST_INCORRECT_RANGE
	Incorrect range specified. The correct syntax is #from-to.
CS_LIST_INCORRECT_RANGE
	To search for channels starting with #, search for the channel
	name without the #-sign prepended (anope instead of #anope).

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
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
	Aquest nick pertany a un altra persona. Per favor escull un altre nick.
	(Si aquest es el teu nick, escriu %R%s IDENTIFY clau.)
NICK_IS_SECURE
	Aquest nick esta registrat i protegit. Si es el teu
	nick, escriu %R%s IDENTIFY clau. Si no,
	per favor escull un nick diferent.
NICK_MAY_NOT_BE_USED
	Aquest nickname no pot ser utilitzat. Per favor escull un altre.
FORCENICKCHANGE_IN_1_MINUTE
	Si no canvies el teu nick en un minut, el canviare jo.
FORCENICKCHANGE_IN_20_SECONDS
	Si no canvies el teu nick en 20 segons, el canviare jo.
FORCENICKCHANGE_NOW
	Aquest nick ha estat registrat; No pots utilitzar-lo.
FORCENICKCHANGE_CHANGING
	El teu nickname s'esta canviant a %s

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER clau [email]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER clau email
NICK_REGISTRATION_DISABLED
	Ho sento, el registre de nicknames esta temporalment deshabilitat.
NICK_REGISTRATION_FAILED
	Ho sento, error en el registre.
NICK_REG_PLEASE_WAIT
	Per favor espera %d segons abans d'utilitzar la comanda REGISTER novament.
NICK_CANNOT_BE_REGISTERED
	El nickname %s no pot ser registrat.
NICK_ALREADY_REGISTERED
	El nickname %s ja esta registrat!

NICK_REGISTERED
	El nickname %s ja esta registrat sota el teu compte: %s
NICK_REGISTERED_NO_MASK
	Nickname %s registered.
NICK_PASSWORD_IS
	La teva clau es %s - Recordala per un us futur.

# GROUP responses
NICK_REG_DELAY
	You have to be connected longer than %d seconds to register your nick.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP grup clau
NICK_GROUP_DISABLED
	Ho sento, La agrupació de nicks esta temporalment inabilitada.
NICK_GROUP_FAILED
	Ho sento, La agrupació de nicks ha fallat.
NICK_GROUP_PLEASE_WAIT
	Per favor espera %d segons abans d'utilitzar la comanda GROUP novament.
NICK_GROUP_CHANGE_DISABLED
	El teu nick ja esta registrat; escriu %R%s DROP primer.
NICK_GROUP_SAME
	Ja ets membre del grup de %s.
NICK_GROUP_TOO_MANY
	N'hi ha molts nicks en el grup de %s; lllistals i treu-ne alguns.
	Escriu %R%s HELP GLIST i %R%s HELP DROP
	per més informació.
NICK_GROUP_JOINED
	Ara ets membre en el grup de %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY clau
NICK_IDENTIFY_FAILED
	Ho sento, la identificació ha fallat.
NICK_IDENTIFY_SUCCEEDED
	Clau aceptada - Has estat reconegut.
NICK_IDENTIFY_EMAIL_REQUIRED
	Ara has de proveir un e-mail pel teu nick.
	Aquest e-mail et permetra recuperar la teva clau en
	cas de que la olvidis.
NICK_IDENTIFY_EMAIL_HOWTO
	Escriu %R%S SET EMAIL e-mail per provar el teu e-mail.
	Respectems la teva privacitat; Aquest e-mail no sera entregat
	a cap persona.
	
# LOGOUT responses
NICK_ALREADY_IDENTIFIED
	You are already identified.

# UPDATE responses
NICK_UPDATE_SUCCESS
	Status updated (memos, vhost, chmodes, flags).
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	El teu nick ha estat desconnectat.
NICK_LOGOUT_X_SUCCEEDED
	Nick %s ha estat desconnectat.

# DROP responses
NICK_LOGOUT_SERVICESADMIN
	Can't logout %s because he's a services administrator.

# DROP responses
NICK_DROP_DISABLED
	Ho sento, desenregistrament de nickname esta temporalment inhabilitat.
NICK_DROPPED
	El teu nickname ha estat expulsat.
NICK_X_DROPPED
	Nickname %s ha estat expulsat.

# SET responses
NICK_SET_SYNTAX
	SET opcio per ametres
NICK_SET_SERVADMIN_SYNTAX
	SET [nick] opcio per ametres
NICK_SET_DISABLED
	Ho sento, el seteig de Opciós per nicknames esta temporalment inhabilitat.
NICK_SET_UNKNOWN_OPTION
	Opcio SET desconeguda %s.
NICK_SET_OPTION_DISABLED
	Opció %s no pot ser establerta en aquesta xarxa.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	El nou display HA DE SER ser un nickname del teu grup de nicknames!
NICK_SET_DISPLAY_CHANGED
	El nou display es ara %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Ho sento, el canvi de clau no pot ser completat.
NICK_SET_PASSWORD_CHANGED
	Clau canviada.
NICK_SET_PASSWORD_CHANGED_TO
	Clau canviada a %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE numero
NICK_SET_LANGUAGE_UNKNOWN
	Numero de llenguatge %d desconegut. Escriu %R%s HELP SET LANGUAGE per una lllista de llenguatges.
NICK_SET_LANGUAGE_CHANGED
	Llenguatge canviat a Català.

# SET URL responses
NICK_SET_URL_CHANGED
	URL canviada a %s.
NICK_SET_URL_UNSET
	URL no establerta.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	Adreça E-mail canviada a %s.
NICK_SET_EMAIL_UNSET
	Adreça E-mail no establerta.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	No pots establir la teva adreça e-mail en aquesta xarxa.
	
# SET ICQ responses
NICK_SET_ICQ_CHANGED
	Numero ICQ establert a %s.
NICK_SET_ICQ_UNSET
	Numero ICQ no establert.
NICK_SET_ICQ_INVALID
	%s no es un numero vàlid.
	
# SET GREET responses
NICK_SET_GREET_CHANGED
	Missatge de benvinguda canviat a %s.
NICK_SET_GREET_UNSET
	Missatge de benvinguda no establert.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Protecció activada.
NICK_SET_KILL_QUICK
	La protecció esta activada, amb demora reduida.
NICK_SET_KILL_IMMED
	La protecció esta activada, sense demora.
NICK_SET_KILL_IMMED_DISABLED
	L'opció IMMED no esta disponible en aquesta xarxa.
NICK_SET_KILL_OFF
	La protecció esta desactivada.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	L'opció de seguretat esta activada.
NICK_SET_SECURE_OFF
	L'opció de seguretat esta desactivada.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	L'opció de privacitat esta activada.
NICK_SET_PRIVATE_OFF
	L'opció de privacitat esta desactivada.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	La teva adreça e-mail no sera mostrada a %s INFO displays.
NICK_SET_HIDE_EMAIL_OFF
	La teva adreça e-mail sera mostrada a %s INFO displays.
NICK_SET_HIDE_MASK_ON
	La teva ultima mascara usuari@host no sera mostrada a %s INFO displays.
NICK_SET_HIDE_MASK_OFF
	La teva ultima mascara usuari@host sera mostrada a %s INFO displays.
NICK_SET_HIDE_QUIT_ON
	El teu últim missatge quit no sera mostrat a %s INFO displays.
NICK_SET_HIDE_QUIT_OFF
	El teu últim missatge quit sera mostrat a %s INFO displays.
	
# SET MSG responses
NICK_SET_HIDE_STATUS_ON
	Your services access status will now be hidden from %s INFO displays.
NICK_SET_HIDE_STATUS_OFF
	Your services access status will now be shown in %s INFO displays.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_MSG_ON
	Els serveis et respondran ara amb missatge.
NICK_SET_MSG_OFF
	Els serveis et respondran ara amb noticies.

# SET AUTOOP responses
NICK_SET_AUTOOP_SYNTAX
	SET AUTOOP {ON | OFF}
NICK_SET_AUTOOP_ON
	Services will now autoop you in channels.
NICK_SET_AUTOOP_OFF
	Services will no longer autoop you in channels.

# SASET NOEXPIRE responses
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
	SSAET nickname  MSG {ON | OFF}
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
	ACCESS {ADD | DEL | LIST} [mascara]
NICK_ACCESS_ALREADY_PRESENT
	La mascara %s ja esta present a la teva lllista d'accés.
NICK_ACCESS_REACHED_LIMIT
	Ho sento, sols pots tenir %d instàncies a la teva lllista d'accés per nickname.
NICK_ACCESS_ADDED
	%s afegit a la teva lllista d'accés.
NICK_ACCESS_NOT_FOUND
	%s no trobat a la teva lllista d'accés.
NICK_ACCESS_DELETED
	%s esborrat de la teva lllista d'accés.
NICK_ACCESS_LIST
	Lllista d'accés:
NICK_ACCESS_LIST_X
	Lllista d'accés per %s:
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
	%s es %s
NICK_INFO_SERVICES_OPER
	%s es un operador de serveis.
NICK_INFO_SERVICES_ADMIN
	%s es un administrador de serveis.
NICK_INFO_SERVICES_ROOT
	%s is a services root administrator.
NICK_INFO_ADDRESS
	Ultima adreça vista: %s
NICK_INFO_ADDRESS_ONLINE
	   Esta en linea desde: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s Esta actualment online.
NICK_INFO_TIME_REGGED
	     Temps registrat: %s
NICK_INFO_LAST_SEEN
	      Ultima vegada vist: %s
NICK_INFO_LAST_QUIT
	   Ùltim missatge quit: %s
NICK_INFO_URL
	                   URL: %s
NICK_INFO_EMAIL
	      		Adreça E-mail: %s
NICK_INFO_VHOST
	            	vhost: %s
NICK_INFO_VHOST2
	            	vhost: %s@%s
NICK_INFO_ICQ
	                 ICQ #: %d
NICK_INFO_GREET
	 Missatge de benvinguda: %s
NICK_INFO_OPTIONS
	              Opciós: %s
NICK_INFO_EXPIRE
	Expires in: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Protecció
NICK_INFO_OPT_SECURE
	Segur
NICK_INFO_OPT_PRIVATE
	Privat
NICK_INFO_OPT_MSG
	Mode de missatge
NICK_INFO_OPT_AUTOOP
	Auto-op
NICK_INFO_OPT_NONE
	Cap
NICK_INFO_NO_EXPIRE
	Aquest nickname no caducara.
NICK_INFO_FOR_MORE
	Per més informació, escriu %R%s INFO %s ALL.

# LIST responses
NICK_INFO_SUSPENDED
	This nickname is currently suspended, reason: %s
NICK_INFO_SUSPENDED_NO_REASON
	This nickname is currently suspended

# LIST responses
NICK_LIST_SYNTAX
	LIST patro
NICK_LIST_SERVADMIN_SYNTAX
	LIST patro [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Lllista de resultats concordant %s:
NICK_LIST_RESULTS
	Fi de la lllista - %d/%d resultat(s) mostrat(s).

# GLIST responses
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
	Fi de la lllista - %d/%d resultat(s) mostrat(s).

# GLIST responses
NICK_GLIST_HEADER
	Lllista de nicknames en el teu grup:
NICK_GLIST_HEADER_X
	Lllista de nicknames en el grup de %s:
NICK_GLIST_FOOTER
	%d nicknames en el grup.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (expires in %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nickname [clau]
NICK_NO_RECOVER_SELF
	No pots recuperar-te a tu mateix!
NICK_RECOVERED
	L'usuari demanant el teu nick ha estat expulsat.
	%R%s RELEASE %s per recuperar-lo abans %s time-out.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nickname [clau]
NICK_RELEASE_NOT_HELD
	El nick %s no esta sent utilitzat.
NICK_RELEASED
	El teu nick ha estat alliberat pels serveis.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nickname [clau]
NICK_NO_GHOST_SELF
	No pots fer GHOST a tu mateix!
NICK_GHOST_KILLED
	El ghost amb el teu nick ha estat expulsat.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nickname
NICK_GETPASS_UNAVAILABLE
	La comanda GETPASS no esta disponible per l'us d'encriptació.
NICK_GETPASS_PASSWORD_IS
	La clau per %s es %s.

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
	La comanda SENDPASS no esta disponible per l'us d'encriptació.
NICK_SENDPASS_SUBJECT
	Clau pel Nickname (%s)
NICK_SENDPASS_HEAD
	Hola,
NICK_SENDPASS_LINE_1
	El teu has demanat rebre la clau pel nickname %s via e-mail.
NICK_SENDPASS_LINE_2
	La clau es %s Per raons de seguretat hauries de canviar-la quant rebis aquest mail.
NICK_SENDPASS_LINE_3
	Si no saps per que aquest mail t'ha estat enviat, per favor ignora'l.
NICK_SENDPASS_LINE_4
	Per favor NO RESPONGUIS a aquest EMAIL!
NICK_SENDPASS_LINE_5
	Administradors de %s.
NICK_SENDPASS_OK
	La clau de %s ha estat enviada.

# FORBID responses
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
	FORBID nickname [rao]
NICK_FORBID_SYNTAX_REASON
	FORBID nickname rao
NICK_FORBID_SUCCEEDED
	El nick %s esta prohibit.
NICK_FORBID_FAILED
	La prohibició de %s no es possible!

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
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
	You have requested to register the following nickname %s.
NICK_REG_MAIL_LINE_2
	Please type " %R%s confirm %s " to complete registration.
NICK_REG_MAIL_LINE_3
	If you don't know why this mail is sent to you, please ignore it silently.
NICK_REG_MAIL_LINE_4
	PLEASE DON'T ANSWER TO THIS MAIL!
NICK_REG_MAIL_LINE_5
	%s administrators.
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
	Status automàtic d'operador de canal
CHAN_LEVEL_AUTOVOICE
	Mode automàtico +v
CHAN_LEVEL_AUTOHALFOP
	Mode automàtic +h
CHAN_LEVEL_AUTOPROTECT
	Mode automàtic +a
CHAN_LEVEL_AUTODEOP
	Status d'operador de canal inhabilitat
CHAN_LEVEL_NOJOIN
	No es possible entra a un canal si esta RESTRINGIT
CHAN_LEVEL_INVITE
	Abilitat a utilitzar la comanda INVITE
CHAN_LEVEL_AKICK
	Abilitat a utilitzar la comanda AKICK
CHAN_LEVEL_SET
	Abilitat a utilitzar la comanda SET (no FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Abilitat a utilitzar la comanda CLEAR
CHAN_LEVEL_UNBAN
	Abilitat a utilitzar la comanda UNBAN
CHAN_LEVEL_OPDEOP
	Abilitat a utilitzar la comanda OP/DEOP
CHAN_LEVEL_ACCESS_LIST
	Abilitat a veure la lllista de accés
CHAN_LEVEL_ACCESS_CHANGE
	Abilitat a modificar la lllista de accés
CHAN_LEVEL_MEMO
	Abilitat a llistar/llegir memos del canal
CHAN_LEVEL_ASSIGN
	Abilitat a assignar/deassignar un bot
CHAN_LEVEL_BADWORDS
	Abilitat a utilitzar la comanda BADWORDS
CHAN_LEVEL_NOKICK
	Mai expulsat pels kicks del bot
CHAN_LEVEL_FANTASIA
	Abilitat a utilizar comandes de fantasia
CHAN_LEVEL_SAY
	Abilitat a utilitzar la comanda SAY i ACT
CHAN_LEVEL_GREET
	Missatge de benvinguda mostrat
CHAN_LEVEL_VOICEME
	Autorizat a voice/devoice a tu mateix
CHAN_LEVEL_VOICE
	Abilitat a utilitzar La comanda VOICE/DEVOICE
CHAN_LEVEL_GETKEY
	Abilitat a utilitzar La comanda GETKEY
CHAN_LEVEL_OPDEOPME
	Autorizat a op/deop a tu mateix
CHAN_LEVEL_HALFOPME
	Autorizat a halfop/dehalfop a tu mateix
CHAN_LEVEL_HALFOP
	Abilitat a utilitzar la comanda HALFOP/DEHALFOP
CHAN_LEVEL_PROTECTME
	Autorizat a protect/deprotect a tu mateix
CHAN_LEVEL_PROTECT
	Abilitat a utilitzar la comanda PROTECT/DEPROTECT
CHAN_LEVEL_KICKME
	Autoritzat a expulsar-te a tu mateix
CHAN_LEVEL_KICK
	Abilitat a utilitzar la comanda KICK
CHAN_LEVEL_SIGNKICK
	Sense firma de kick quant SIGNKICK LEVEL es utilitzat
CHAN_LEVEL_BANME
	Autoritzat a banegar-te
CHAN_LEVEL_BAN
	Autoritzat a utilitzar la comanda BAN
CHAN_LEVEL_TOPIC
	Autoritzat a utilitzar la comanda TOPIC
CHAN_LEVEL_INFO
	Autoritzar a utilitzar La comanda INFO amb
	l'opció ALL

# Automatic responses
CHAN_IS_REGISTERED
	Aquest canal ha estat registrat amb %s.
CHAN_NOT_ALLOWED_OP
	No estas autoritzat al status d'operador del canal a %s.
CHAN_MAY_NOT_BE_USED
	Aquest canal esta prohibit.
CHAN_NOT_ALLOWED_TO_JOIN
	No estas autoritzat a estar en aquest canal.

# REGISTER responses
CHAN_X_INVALID
	Channel %s is not a valid channel.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER canal clau descripció
CHAN_REGISTER_DISABLED
	Ho sento, el registre de canals esta temporalment inhabilitat.
CHAN_REGISTER_NOT_LOCAL
	Canals locals no poden ser enregistrats.
CHAN_MUST_REGISTER_NICK
	Has de registrar el teu nickname primer. Escriu %R%s HELP per més informació de com registrar nicknames.
CHAN_MUST_IDENTIFY_NICK
	Per favor primer identifica't amb %s, utilitzant la comanda:
	%R%s IDENTIFY clau
CHAN_MAY_NOT_BE_REGISTERED
	El canal %s no pot ser enregistrat.
CHAN_ALREADY_REGISTERED
	El canal %s ja esta enregistrat!
CHAN_MUST_BE_CHANOP
	Has de ser un operador de canal per registrar el canal.
CHAN_REACHED_CHANNEL_LIMIT
	Ho sento, has arribat al teu límit de %d canals.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Ho sento, has sobrepassat el teu límit de %d canals.
CHAN_REGISTRATION_FAILED
	Ho sento, la registració ha fallat.
CHAN_REGISTERED
	El canal %s esta registrat sota el teu nickname: %s
CHAN_PASSWORD_IS
	La teva clau pel canal es %s - Recorda-la per un us futur.
CHAN_REGISTER_NONE_CHANNEL
	You have attempted to register a nonexistent channel %s
CHAN_SYMBOL_REQUIRED
	Please use the symbol of # when attempting to register


# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY canal clau
CHAN_IDENTIFY_FAILED
	Ho sento, la identificació ha fallat.
CHAN_IDENTIFY_SUCCEEDED
	Clau aceptada - ara tens nivell de fundador a %s.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT canal nickname
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT canal [nickname]
CHAN_LOGOUT_SUCCEEDED
	L'usuari %s ha estat desconnectat del canal %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	Tots els usuaris identificats han estat desconnectats del canal %s (except the channel founder).
CHAN_LOGOUT_FOUNDER_FAILED
	You may not log out of %s (you are founder).

# DROP responses
CHAN_DROP_SYNTAX
	DROP canal
CHAN_DROP_DISABLED
	Ho sento, el desenregistrament de canals esta temporalment desabilitat.
CHAN_DROPPED
	El canal %s ha estat eliminat.

# SET responses
CHAN_SET_SYNTAX
	SET canal opció per ametres
CHAN_SET_DISABLED
	Ho sento, l'establiment d'Opciós de canal esta temporalment deshAbilitat.
CHAN_SET_UNKNOWN_OPTION
	Opció SET %s desconoceguda.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s te masses canals registrats.
CHAN_FOUNDER_CHANGED
	El Fundador de %s cambiat a %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Sucessor de %s cambiat a %s.
CHAN_SUCCESSOR_UNSET
	Sucessor de %s desabilitat.
CHAN_SUCCESSOR_IS_FOUNDER
	%s no pot ser el sucessor en el canal %s ja que n'es el fundador.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Ho sento, l'establiment de la clau no ha estat possible.
CHAN_PASSWORD_CHANGED
	Clau per a %s canviada.
CHAN_PASSWORD_CHANGED_TO
	La clau per a %s canviada a %s.

# SET DESC responses
CHAN_DESC_CHANGED
	La descripció de %s canviada a%s.

# SET URL responses
CHAN_URL_CHANGED
	URL per a %s canviada a %s.
CHAN_URL_UNSET
	URL per a %s no establerta.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	Adreça e-mail per a %s canviada a %s.
CHAN_EMAIL_UNSET
	Adreça e-mail per a %s no establerta.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Missatge d'entrada per a %s canviat.
CHAN_ENTRY_MSG_UNSET
	Missatge d'entrada per a %s no establert.
	
# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s no es un tipus de ban valid.
CHAN_SET_BANTYPE_CHANGED
	Tipus de ban pel canal %s es ara #%d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Caràcter de Mode %c desconegut i ignorat.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Mode %c ignorat per que no pots ¿¿¿lockerjar-lo???.
CHAN_SET_MLOCK_L_REQUIRED
	Has d'utilitzar Mode de lock +l també pel Mode
	de lock +L.
CHAN_SET_MLOCK_K_REQUIRED
	Has d'utilitzar Mode de lock +i tambe pel Mode
	de lock +K.
CHAN_MLOCK_CHANGED
	Mode de lock en el canal %s canviat a %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET canal KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Topic retention option for %s is now ON.
CHAN_SET_KEEPTOPIC_OFF
	Topic retention option for %s is now OFF.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET canal TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Topic lock option for %s is now ON.
CHAN_SET_TOPICLOCK_OFF
	Topic lock option for %s is now OFF.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET canal PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Peace option for %s is now ON.
CHAN_SET_PEACE_OFF
	Peace option for %s is now OFF.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET canal PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Private option for %s is now ON.
CHAN_SET_PRIVATE_OFF
	Private option for %s is now OFF.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET canal SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Secure ops option for %s is now ON.
CHAN_SET_SECUREOPS_OFF
	Secure ops option for %s is now OFF.

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET canal SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Secure founder option for %s is now ON.
CHAN_SET_SECUREFOUNDER_OFF
	Secure founder option for %s is now OFF.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET canal RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Restricted access option for %s is now ON.
CHAN_SET_RESTRICTED_OFF
	Restricted access option for %s is now OFF.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET canal SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Secure option for %s is now ON.
CHAN_SET_SECURE_OFF
	Secure option for %s is now OFF.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET canal SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Signed kick option for %s is now ON.
CHAN_SET_SIGNKICK_LEVEL
	Signed kick option for %s is now ON, but depends of the
	level of the user that is using the command.
CHAN_SET_SIGNKICK_OFF
	Signed kick option for %s is now OFF.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET canal OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice option for %s is now ON.
CHAN_SET_OPNOTICE_OFF
	Op-notice option for %s is now OFF.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET canal XOP {ON | OFF}
CHAN_SET_XOP_ON
	xOP lists system for %s is now ON.
CHAN_SET_XOP_OFF
	xOP lists system for %s is now OFF.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET canal NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	El canal %s no caducara.
CHAN_SET_NOEXPIRE_OFF
	El canal %s caducara.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Ho sento, sols pots tenir %d instàncies de AOP/SOP/VOP
	en un canal.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	No pots utilitzar aquesta comanda. Utilitza la comanda ACCESS.
	Escriu %R%s HELP ACCESS per més informació.
CHAN_XOP_NOT_AVAILABLE
	xOP system is not available.

# AOP messages
CHAN_AOP_SYNTAX
	AOP canal {ADD|DEL|LIST|CLEAR} [nick | instància]
CHAN_AOP_DISABLED
	Ho sento, la modificació de la llista AOP de canals esta
	temporalment deshabilitada.
CHAN_AOP_NICKS_ONLY
	La llista de AOP de canals nomès pot contenir nicks
	registrats.
CHAN_AOP_ADDED
	%s afegit a la llista AOP de %s.
CHAN_AOP_MOVED
	%s moved to %s AOP list.
CHAN_AOP_NO_SUCH_ENTRY
	No existeix la instància (#%d) a la llista AOP de %s.
CHAN_AOP_NOT_FOUND
	%s no trobat a la llista AOP de %s.
CHAN_AOP_NO_MATCH
	No existeixen instàncies coincidents a la llista AOP de %s.
CHAN_AOP_DELETED
	%s esborrat de la llista AOP de %s.
CHAN_AOP_DELETED_ONE
	1 instància esborrada de la llista AOP de %s.
CHAN_AOP_DELETED_SEVERAL
	%d instàncies esborrades de la llista AOP de %s.
CHAN_AOP_LIST_EMPTY
	La llista AOP de %s esta buida.
CHAN_AOP_LIST_HEADER
	Llista AOP per a %s:
	  Num  Nick
CHAN_AOP_CLEAR
	La llista AOP de canals %s ha estat netegada.

# HOP messages
CHAN_HOP_SYNTAX
	HOP canal {ADD|DEL|LIST|CLEAR} [nick | llista]
CHAN_HOP_DISABLED
	Ho sento, la modificació de llistes HOP de canals esta
	temporalment deshabilitada.
CHAN_HOP_NICKS_ONLY
	Llistes HOP de canals sols pot tenir nicks registrats.
CHAN_HOP_ADDED
	%s afegit a la llista HOP de %s.
CHAN_HOP_MOVED
	%s moved to %s HOP list.
CHAN_HOP_NO_SUCH_ENTRY
	No existeix la instància (#%d) a la llista HOP de %s.
CHAN_HOP_NOT_FOUND
	%s no trobat a la llista HOP de %s.
CHAN_HOP_NO_MATCH
	No existeixen instàncies coincidents a la llista HOP de %s.
CHAN_HOP_DELETED
	%s esborrat de la llista HOP de %s.
CHAN_HOP_DELETED_ONE
	1 instància esborrada de la llista HOP de %s.
CHAN_HOP_DELETED_SEVERAL
	%d instàncies esborrades de la llista HOP de %s.
CHAN_HOP_LIST_EMPTY
	La llista HOP de %s esta buida.
CHAN_HOP_LIST_HEADER
	Llista HOP per a %s:
	  Num  Nick
CHAN_HOP_CLEAR
	La llista HOP de canals %s ha estat netegada.

# SOP messages
CHAN_SOP_SYNTAX
	SOP canal {ADD|DEL|LIST|CLEAR} [nick | llista]
CHAN_SOP_DISABLED
	Ho sento, la modificació de la llista SOP de canals esta
	temporalment deshabilitada.
CHAN_SOP_NICKS_ONLY
	Llistes SOP de canals sols pot tenir nicks registrats.
CHAN_SOP_ADDED
	%s afegit a la llista SOP de %s.
CHAN_SOP_MOVED
	%s moved to %s SOP list.
CHAN_SOP_NO_SUCH_ENTRY
	No existeix la instància (#%d) a la llista SOP de %s.
CHAN_SOP_NOT_FOUND
	%s no trobat a la llista SOP de %s.
CHAN_SOP_NO_MATCH
	No existeixen instàncies coincidents a la llista SOP de %s.
CHAN_SOP_DELETED
	%s esborrat de la llista SOP de %s.
CHAN_SOP_DELETED_ONE
	1 instància esborrada de la llista SOP de %s.
CHAN_SOP_DELETED_SEVERAL
	%d instàncies esborrades de la llista SOP de %s.
CHAN_SOP_LIST_EMPTY
	La llista SOP de %s esta buida.
CHAN_SOP_LIST_HEADER
	Llista SOP per a %s:
	  Num  Nick
CHAN_SOP_CLEAR
	La llista SOP de canals %s ha estat netegada.

# VOP messages
CHAN_VOP_SYNTAX
	VOP canal {ADD|DEL|LIST|CLEAR} [nick | llista]
CHAN_VOP_DISABLED
	Ho sento, la modificació de la llista VOP de canals esta
	temporalment deshabilitada.
CHAN_VOP_NICKS_ONLY
	Llistes VOP de canals sols pot tenir nicks registrats.
CHAN_VOP_ADDED
	%s afegit a la llista VOP de %s.
CHAN_VOP_MOVED
	%s moved to %s VOP list.
CHAN_VOP_NO_SUCH_ENTRY
	No existeix la instància (#%d) a la llista VOP de %s.
CHAN_VOP_NOT_FOUND
	%s no trobat a la llista VOP de %s.
CHAN_VOP_NO_MATCH
	No existeixen instàncies coincidents a la llista VOP de %s.
CHAN_VOP_DELETED
	%s esborrat de la llista VOP de %s.
CHAN_VOP_DELETED_ONE
	1 instància esborrada de la llista VOP de %s.
CHAN_VOP_DELETED_SEVERAL
	%d instàncies esborrades de la llista VOP de %s.
CHAN_VOP_LIST_EMPTY
	La llista VOP de %s esta buida.
CHAN_VOP_LIST_HEADER
	Llista VOP per a %s:
	  Num  Nick
CHAN_VOP_CLEAR
	La llista VOP de canals %s ha estat netegada.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS canal {ADD|DEL|LIST|CLEAR} [nick [nivell] | llista]
CHAN_ACCESS_XOP
	No pots utilitzar aquest comanda.
	Utilitza les comandes AOP, SOP i VOP.
	Escriu %R%s HELP comanda per a més informació.
CHAN_ACCESS_XOP_HOP
	You can't use this command. 
	Use the AOP, SOP, HOP and VOP commands instead.
	Type %R%s HELP command for more information.
CHAN_ACCESS_DISABLED
	Ho sento, la modificació d'accés a canals esta temporalment inabilitada.
CHAN_ACCESS_LEVEL_NONZERO
	El nivell de accés no pot ser zero.
CHAN_ACCESS_LEVEL_RANGE
	El nivell d'accés ha de ser entre %d i %d inclosos.
CHAN_ACCESS_NICKS_ONLY
	Las llistes de accés a canals sols pot contenir nicks registrats.
CHAN_ACCESS_REACHED_LIMIT
	Ho sento, sols pots tenir %d instàncies d'accés en un determinat canal.
CHAN_ACCESS_LEVEL_UNCHANGED
	Nivell d'accés per a %s en %s no cambiada de %d.
CHAN_ACCESS_LEVEL_CHANGED
	Nivell d'accés per a %s en %s cambiada a %d.
CHAN_ACCESS_ADDED
	%s afegit a la llista d'accés de %s amb nivel %d.
CHAN_ACCESS_NO_SUCH_ENTRY
	No existeix (#%d) a la llista d'accés de %s.
CHAN_ACCESS_NOT_FOUND
	%s no trobat a la llista d'accés de %s.
CHAN_ACCESS_NO_MATCH
	No hi ha instàncies que concordin a la llista d'accés de %s .
CHAN_ACCESS_DELETED
	%s esborrat de la llista d'accés de %s.
CHAN_ACCESS_DELETED_ONE
	1 instància esborrada de la llista d'accés de %s.
CHAN_ACCESS_DELETED_SEVERAL
	%d instàncies esborrades de la llista d'accés de %s.
CHAN_ACCESS_LIST_EMPTY
	La llista d'accés de %s esta buida.
CHAN_ACCESS_LIST_HEADER
	Llista d'accés per a %s:
	  Num   Niv  Nick
CHAN_ACCESS_LIST_FOOTER
	fi de llista d'accés.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	La llista d'accés pel canal %s ha estat netegada.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK canal {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [nick-o-mascara] [rao]
CHAN_AKICK_DISABLED
	Ho sento, la modificació de kicks automàtics a canals esta temporalment inabilitada.
CHAN_AKICK_ALREADY_EXISTS
	%s ja esta present a la llista de kick automàtic en el canal %s.
CHAN_AKICK_REACHED_LIMIT
	Ho sento, sols pots tenir %d mascaras per a kicks automàtics a un canal.
CHAN_AKICK_ADDED
	%s afegit a la llista de kicks automàtics en el canal %s.
CHAN_AKICK_NO_SUCH_ENTRY
	La instància (#%d) no existeix a la llista de kicks automàtics en el canal %s.
CHAN_AKICK_NOT_FOUND
	%s no trobat a la llista de kicks automàtics en el canal %s.
CHAN_AKICK_NO_MATCH
	No existeixen instàncies que concordin a la llista de kicks automàtics en el canal %s.
CHAN_AKICK_STUCK
	%s esta ara sempre actiu en el canal %s.
CHAN_AKICK_UNSTUCK
	%s ja no esta sempre actiu en el canal %s.
CHAN_AKICK_DELETED
	%s esborrat de la llista de kicks automàtics del canal %s.
CHAN_AKICK_DELETED_ONE
	1 instància esborrada de la llista de kicks automàtics del canal %s.
CHAN_AKICK_DELETED_SEVERAL
	%d instàncies esborrades de la llista de kicks automàtics del canal %s.
CHAN_AKICK_LIST_EMPTY
	La llista de kicks automàtics del canal %s esta buida.
CHAN_AKICK_LIST_HEADER
	Llista de kicks automàtics pel canal %s:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (per %s en %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (enganxat) (per %s en %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE en %s completa; %d usuaris afectats.
CHAN_AKICK_CLEAR
	La llista de kicks automàtics en canal %s ha estat netegada.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS canal {SET | DIS[ABLE] | LIST | RESET} [item [nivell]]
CHAN_LEVELS_XOP
	Aquesta comanda es inservible en aquest mode.
CHAN_LEVELS_RANGE
	El nivell ha d'estar entre %d i %d inclosos.
CHAN_LEVELS_CHANGED
	Nivell per a %s en el canal %s cambiat a %d.
CHAN_LEVELS_UNKNOWN
	Opció %s no coneguda. Escriu %R%s HELP LEVELS DESC per a una llista de opcións valides.
CHAN_LEVELS_DISABLED
	%s inhabilitat(s) en el canal %s.
CHAN_LEVELS_LIST_HEADER
	Opcions del nivell d'accés pel canal %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (inhabilitat)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (nomès el fundador)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	Nivells d'accés per a %s canviats als valors originals.

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
	INFO canal [ALL]
CHAN_INFO_HEADER
	Informació pel canal %s:
CHAN_INFO_FOUNDER
	          Fundador: %s (%s)
CHAN_INFO_NO_FOUNDER
	          Fundador: %s
CHAN_INFO_SUCCESSOR
	           Successor: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	           Successor: %s
CHAN_INFO_DESCRIPTION
	       Descripció: %s
CHAN_INFO_ENTRYMSG
	Missatge d'entrada: %s
CHAN_INFO_TIME_REGGED
	        Registrat: %s
CHAN_INFO_LAST_USED
	  Últim cop utilitzat: %s
CHAN_INFO_LAST_TOPIC
	      Últim tòpic: %s
CHAN_INFO_TOPIC_SET_BY
	 Tòpic establert per: %s
CHAN_INFO_URL
	               URL: %s
CHAN_INFO_EMAIL
	  Adreça e-mail: %s
CHAN_INFO_BANTYPE
	       Tipus de ban: %d
CHAN_INFO_OPTIONS
	          Opcions: %s
CHAN_INFO_OPT_KEEPTOPIC
	Retenció de tòpic
CHAN_INFO_OPT_OPNOTICE
	Notícia de OP
CHAN_INFO_OPT_PEACE
	Pau
CHAN_INFO_OPT_PRIVATE
	Privat
CHAN_INFO_OPT_RESTRICTED
	accés restringit
CHAN_INFO_OPT_SECURE
	Segur
CHAN_INFO_OPT_SECUREOPS
	Ops Segurs
CHAN_INFO_OPT_SECUREFOUNDER
	Fundador Segur
CHAN_INFO_OPT_SIGNKICK
	Kicks Signats
CHAN_INFO_OPT_TOPICLOCK
	Lock de tòpic
CHAN_INFO_OPT_XOP
	Sistema de llistes xOP
CHAN_INFO_OPT_NONE
	Cap
CHAN_INFO_MODE_LOCK
	      Mode de lock: %s
CHAN_INFO_EXPIRE
	      Expires in: %s
CHAN_INFO_NO_EXPIRE
	Aquest canal no caducara.

# LIST responses
CHAN_LIST_SYNTAX
	LIST patró
CHAN_LIST_SERVADMIN_SYNTAX
	LIST patró [FORBIDDEN] [NOEXPIRE]
CHAN_LIST_HEADER
	Llista de instàncies concordant a %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Fi de la llista - %d/%d concordància(es) mostrada(es).
	
# CLIST responses
CHAN_INVITE_SYNTAX
	INVITE canal

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN canal
CHAN_UNBANNED
	Has estat desbanegat de %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC canal [tòpic]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR canal Modes-a-netegar
CHAN_CLEARED_BANS
	Tots els bans del canal %s han estat esborrats.
CHAN_CLEARED_EXCEPTS
	Tots els excepts del canal %s han estat esborrats.
CHAN_CLEARED_MODES
	Tots els modes en el canal %s han estat reestablerts.
CHAN_CLEARED_OPS
	Mode +o ha estat reestablert del canal %s.
CHAN_CLEARED_HOPS
	Mode +h has been cleared from channel %s.
CHAN_CLEARED_VOICES
	Mode +v ha estat eliminat del canal %s.
CHAN_CLEARED_USERS
	Tots els usuaris han estat kickegats del canal %s.
CHAN_CLEARED_INVITES
	All invites on channel %s have been removed.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS canal
CHAN_GETPASS_UNAVAILABLE
	Comanda GETPASS no permessa per que encriptació esta en ús.
CHAN_GETPASS_PASSWORD_IS
	La clau pel canal %s es %s.
	
# SENDPASS responses
CHAN_GETKEY_SYNTAX
	GETKEY channel
CHAN_GETKEY_NOKEY
	The channel %s has no key.
CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS canal
CHAN_SENDPASS_UNAVAILABLE
	Comanda SENDPASS no permessa per que encriptació esta en ús.
CHAN_SENDPASS_SUBJECT
	Clau del canal (%s)
CHAN_SENDPASS_HEAD
	Hola,
CHAN_SENDPASS_LINE_1
	Has sol.licitat rebre la clau del canal %s per e-mail.
CHAN_SENDPASS_LINE_2
	La clau es %s. Per raons de seguretat hauries de cambiar-la quant rebis aquest mail.
CHAN_SENDPASS_LINE_3
	Si no saps per que has rebut aquest mail, per favor ignora'l.
CHAN_SENDPASS_LINE_4
	Per favor NO RESPONGUIS a aquest email!
CHAN_SENDPASS_LINE_5
	Administradors de %s.
CHAN_SENDPASS_OK
	La clau de %s ha estat enviada.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID canal [raó]
CHAN_FORBID_SYNTAX_REASON
	FORBID canal raó
CHAN_FORBID_SUCCEEDED
	Canal %s ara prohibit.
CHAN_FORBID_FAILED
	Impossible prohibir el canal %s!

# Misc responses
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
	Tens 1 memo nou.
MEMO_HAVE_NEW_MEMOS
	Tens %d memos nous.
MEMO_TYPE_READ_LAST
	Escriu %R%s READ LAST per llegir el teu últim memo.
MEMO_TYPE_READ_NUM
	Escriu %R%s READ %d per a llegir-lo.
MEMO_TYPE_LIST_NEW
	Escriu %R%s LIST NEW per a llistar els teus memos.
MEMO_AT_LIMIT
	Advertència: Has assolit el màxim numero de memos (%d). No podras rebre mes memos fins que esborris alguns dels existents.
MEMO_OVER_LIMIT
	Advertència: Has sobrepassat el màximo numero de memos (%d). Et resultara impossible rebre més memos fins que esborris alguns dels existents.
MEMO_X_MANY_NOTICE
	There are %d memos on channel %s.
MEMO_X_ONE_NOTICE
	There is %d memo on channel %s.
MEMO_NEW_X_MEMO_ARRIVED
	There is a new memo on channel %s.
	Type %R%s READ %s %d to read it.
MEMO_NEW_MEMO_ARRIVED
	Tens un nou memo de %s.
	Escriu %R%s READ %d per a llegir-lo.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	No tens memos.
MEMO_X_HAS_NO_MEMOS
	%s no te memos.
MEMO_DOES_NOT_EXIST
	El memo %d no existeix!
MEMO_LIST_NOT_FOUND
	No existeixen memos que concordin.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {nick | canal} text
MEMO_SEND_DISABLED
	Ho sento, l'enviament de memos esta temporalment inhabilitat.
MEMO_SEND_PLEASE_WAIT
	Per favor espera %d segons abans d'utilizar la comanda SEND un altre cop.
MEMO_X_GETS_NO_MEMOS
	%s no pot rebre memos.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s actualment te molts memos i no pot rebre'n mes.
MEMO_SENT
	Memo enviat a %s.
	
# CANCEL responses
MEMO_MASS_SENT
	A massmemo has been sent to all registered users.
	
# STAFF responses
MEMO_STAFF_SYNTAX
	STAFF memo-text
	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {nick | canal}
MEMO_CANCEL_DISABLED
	Ho sento, la cancel.lació de memos esta temporalment inabilitada.
MEMO_CANCEL_NONE
	Cap memo pot ser cancelat.
MEMO_CANCELLED
	Ultim memo enviat a %s va ser cancel.lat.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [canal] [llista | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	No tens memos nous.
MEMO_X_HAS_NO_NEW_MEMOS
	%s no te memos nous.
MEMO_LIST_MEMOS
	Memos per a %s. per a llegir-los escriu: %R%s READ numero
MEMO_LIST_NEW_MEMOS
	Nous memos per a %s. per a llegir-los escriu: %R%s READ numero
MEMO_LIST_CHAN_MEMOS
	Memos per a %s. per a llegir-los escriu: %R%s READ %s numero
MEMO_LIST_CHAN_NEW_MEMOS
	Nous memos per a %s. Per a llegir-los escriu: %R%s READ %s numero
MEMO_LIST_HEADER
	 Num  De                Data/Hora
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [canal] {llista | LAST | NEW}
MEMO_HEADER
	Memo %d de %s (%s). per a esborrar-lo escriu: %R%s DEL %d
MEMO_CHAN_HEADER
	Memo %d de %s (%s). per a esborrar-lo escriu: %R%s DEL %s %d
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [canal] {numero | llista | ALL}
MEMO_DELETED_NONE
	Cap memo fou esborrat.
MEMO_DELETED_ONE
	Memo %d ha estat esborrat.
MEMO_DELETED_SEVERAL
	Els memos %s han estat esborrats.
MEMO_DELETED_ALL
	Tots els teus memos han estat esborrats.

# SET responses
MEMO_CHAN_DELETED_ALL
	All memos for channel %s have been deleted.

# SET responses
MEMO_SET_SYNTAX
	SET Opció parametres
MEMO_SET_DISABLED
	Ho sento, l'establiment d'opcions per a memos esta temporalment inhabilitat.
MEMO_SET_UNKNOWN_OPTION
	Opció SET %s desconeguda.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
MEMO_SET_NOTIFY_ON
	%s ara et notificara dels teus memos quant et conectis i quant et siguin enviats.
MEMO_SET_NOTIFY_LOGON
	%s ara et notificara dels teus memos quant et conectis o treguis el teu /AWAY.
MEMO_SET_NOTIFY_NEW
	%s ara te notificara dels teus memos quant se t'enviin.
MEMO_SET_NOTIFY_OFF
	%s no t'enviara cap notificació dels teus memos.

# SET LIMIT responses
MEMO_SET_NOTIFY_MAIL
	You will now be informed about new memos via email.
MEMO_SET_NOTIFY_NOMAIL
	You will no longer be informed via email.
MEMO_SET_NOTIFY_INVALIDMAIL
	There's no email address set for your nick.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [canal] límit
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [usuari | canal] {límit | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	No estas autoritzat a canviar el teu límit de memos.
MEMO_SET_LIMIT_FORBIDDEN
	El límit de memos per a %s no pot ser canviat.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	No pots establir el teu límit de memos a mes de %d.
MEMO_SET_LIMIT_TOO_HIGH
	No pots establir el límite de memos per %s mes de %d.
MEMO_SET_LIMIT_OVERFLOW
	Límit de memos molt alt; limitant-lo a %d.
MEMO_SET_YOUR_LIMIT
	El teu límite de memos ha estat establert a %d.
MEMO_SET_YOUR_LIMIT_ZERO
	No et sera possible rebre més memos.
MEMO_UNSET_YOUR_LIMIT
	El teu límit de memos ha estat deshabilitat.
MEMO_SET_LIMIT
	Límite de memos per a %s establert a %d.
MEMO_SET_LIMIT_ZERO
	Límit de memos per a %s establert a 0.
MEMO_UNSET_LIMIT
	Límit de memos per a %s deshabilitat.

# INFO responses
MEMO_INFO_SYNTAX
	INFO [canal]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [nick | canal]
MEMO_INFO_NO_MEMOS
	No tens memos.
MEMO_INFO_MEMO
	Tens 1 memo.
MEMO_INFO_MEMO_UNREAD
	Tens 1 memo i encara no ha estat llegit.
MEMO_INFO_MEMOS
	Tens %d memos.
MEMO_INFO_MEMOS_ONE_UNREAD
	Tens %d memos, dels quals 1 encara no ha estat llegit.
MEMO_INFO_MEMOS_SOME_UNREAD
	Tens %d memos, dels quals %d encara no han estat llegits.
MEMO_INFO_MEMOS_ALL_UNREAD
	Tens %d memos, cap d'ells llegit.
MEMO_INFO_LIMIT
	El teu límit de memos es %d.
MEMO_INFO_HARD_LIMIT
	El teu límit de memos es %d i no pot ser canviat.
MEMO_INFO_LIMIT_ZERO
	El teu límit de memos es 0; no rebràs nous memos.
MEMO_INFO_HARD_LIMIT_ZERO
	El teu límit de memos es 0; no rebràs nous memos. No pots cambiar aquest límit.
MEMO_INFO_NO_LIMIT
	No tens límit en el numero de memos que pots mantenir.
MEMO_INFO_NOTIFY_OFF
	No seràs notificat per memos nous.
MEMO_INFO_NOTIFY_ON
	Seràs notificat per nous memos quant et conectis i quant se t'enviin.
MEMO_INFO_NOTIFY_RECEIVE
	Seràs notificat quan els memos nous se t'enviin.
MEMO_INFO_NOTIFY_SIGNON
	Seràs notificat per memos nous quant et conectis.
MEMO_INFO_X_NO_MEMOS
	%s no te memos.
MEMO_INFO_X_MEMO
	%s te 1 memo.
MEMO_INFO_X_MEMO_UNREAD
	%s actualment te 1 memo i encara no ha estat llegit.
MEMO_INFO_X_MEMOS
	%s actualment te %d memos.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s actualment te %d memos, dels quals 1 encara no ha estat llegit.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s actualment te %d memos, dels quals %d encara no han estat llegits.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s actualment te %d memos, dels quals cap ha estat llegit.
MEMO_INFO_X_LIMIT
	El límit de memos per a %s es %d.
MEMO_INFO_X_HARD_LIMIT
	El límit de memos per a %s es %d, i no pot ser canviat.
MEMO_INFO_X_NO_LIMIT
	%s no te límit per a memos.
MEMO_INFO_X_NOTIFY_OFF
	%s no es notificat per memos nous.
MEMO_INFO_X_NOTIFY_ON
	%s es notificat per memos nous quant es connecta i quant li són enviats.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s es notificat quant nous memos li són enviats.
MEMO_INFO_X_NOTIFY_SIGNON
	%s es notificat de nous memos quant es connecta.
	
###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_MAIL_SUBJECT
	New memo
MEMO_MAIL_TEXT1
	Hi %s
MEMO_MAIL_TEXT2
	You've just received a new memo from %s. This is memo number %d.
MEMO_MAIL_TEXT3
	Memo Text:

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
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

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_CHECK_SYNTAX
	CHECK nickname
MEMO_CHECK_NOT_READ
	The last memo you sent to %s (sent on %s) has not yet been read.
MEMO_CHECK_READ
	The last memo you sent to %s (sent on %s) has been read.
MEMO_CHECK_NO_MEMO
	Nick %s doesn't have a memo from you.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_NO_RSEND_SELF
	You can not request a receipt when sending a memo to yourself.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
BOT_DOES_NOT_EXIST
	El bot %s no existeix.
BOT_NOT_ASSIGNED
	Has d'assignar un bot al canal abans de poder utilitzar aquesta comanda.
	Escriu %R%S HELP ASSIGN per a més informació.
BOT_NOT_ON_CHANNEL
	El bot no esta al canal %s.
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	No utilitzis la paraula %s en aquest canal!
BOT_REASON_BADWORD_GENTLE
	Cuida el teu llenguatge!
BOT_REASON_BOLD
	No utilitzis negreta en aquest canal!
BOT_REASON_CAPS
	Desactiva les majuscules!
BOT_REASON_COLOR
	No utilitzis colors en aquest canal!
BOT_REASON_FLOOD
	Para de floodejar!
BOT_REASON_REPEAT
	No repeteixis com l'allioli!
BOT_REASON_REVERSE
	No utilitzis reversos en aquest canal!
BOT_REASON_UNDERLINE
	No utilitzis subratllats en aquest canal!
	
# !seen replies
BOT_SEEN_BOT
	M'has trobat %s!
BOT_SEEN_YOU
	Cercant-te a tu mateix, eh %s?
BOT_SEEN_ON_CHANNEL
	%s esta en el canal en aquests instants!
BOT_SEEN_ON_CHANNEL_AS
	%s esta en el canal en aquests instantes (com %s) ! 
BOT_SEEN_ON
	%s va ser vist aqui fa %s.
BOT_SEEN_NEVER
	Mai he vist a %s en aquest canal.
BOT_SEEN_UNKNOWN
	Mai he visto a %s.

# BOT responses
BOT_BOT_SYNTAX
	BOT ADD nick usuari host nom-real
	BOT CHANGE nick-antic nick-nou [usuari [host [nom-real]]]
	BOT DEL nick
BOT_BOT_ALREADY_EXISTS
	El bot %s ja existeix.
BOT_BOT_CREATION_FAILED
	Ho sento, la creació de bot ha fallat
BOT_BOT_READONLY
	Ho sento, la modificació de bots esta temporalment inabilitada.
BOT_BOT_ADDED
	%s!%s@%s (%s) afegit a la llista de bots.
BOT_BOT_ANY_CHANGES
	La informació antiga es igual a la nova.
BOT_BOT_CHANGED
	El bot %s ha estat canviat a %s!%s@%s (%s)
BOT_BOT_DELETED
	El bot %s ha estat esborrat.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Llista de bots:
BOT_BOTLIST_PRIVATE_HEADER
	Bots reservats a operadors de IRC:
BOT_BOTLIST_FOOTER
	%d bots disponibles.
BOT_BOTLIST_EMPTY
	No hi ha bots disponibles en aquest moment.
	Demana-li a un administrador de serveis que en fagi un!
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN canal nick
BOT_ASSIGN_READONLY
	Ho sento, el assignament de bots esta temporalment inhabilitat.
BOT_ASSIGN_ALREADY
	Bot %s is already assigned to channel %s.
BOT_ASSIGN_ASSIGNED
	El bot %s ha estat assignat al canal %s.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN canal
BOT_UNASSIGN_UNASSIGNED
	Ja no hi ha un bot assignat a %s.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {canal | nick}
BOT_INFO_NOT_FOUND
	%s no és un bot valid o un canal registrat.
BOT_INFO_BOT_HEADER
	Informació pel bot %s:
BOT_INFO_BOT_MASK
	       Mascara : %s@%s
BOT_INFO_BOT_REALNAME
	  Nom real : %s
BOT_INFO_BOT_CREATED
	    Creat : %s
BOT_INFO_BOT_USAGE
	    Utilitzat a : %d canal(s)
BOT_INFO_BOT_OPTIONS
	    Opcions : %s
BOT_INFO_OPT_PRIVATE
	Privat
BOT_INFO_CHAN_HEADER
	Informació pel canal %s:
BOT_INFO_CHAN_BOT
	           Nick del bot : %s
BOT_INFO_CHAN_BOT_NONE
	           Nick del bot : Encara no assignat.
BOT_INFO_CHAN_KICK_BADWORDS
	   Kicker per paraules malsonants : %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	   Kicker per paraules malsonants : %s (%d kick(s) per a ban)
BOT_INFO_CHAN_KICK_BOLDS
	    Kicker per negretes : %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	    Kicker per negretes : %s (%d kick(s) per a ban)
BOT_INFO_CHAN_KICK_CAPS_ON
	            Caps kicker : %s (mínim %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	  Kicker per majuscules : %s (%d kick(s) per a ban; mínim %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	  Kicker per majuscules : %s
BOT_INFO_CHAN_KICK_COLORS
	     Kicker per colors : %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	     Kicker per colors : %s (%d kick(s) per a ban)
BOT_INFO_CHAN_KICK_FLOOD_ON
	       Kicker per flood : %s (%d linees en %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	       Kicker per flood : %s (%d kick(s) per a ban; %d linees en %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	       Kicker per flood : %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	  Kicker per repetició : %s (%d vegades)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	  Kicker per repetició : %s (%d kick(s) per a ban; %d vegades)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	  Kicker per repetició : %s
BOT_INFO_CHAN_KICK_REVERSES
	    Kicker per inversos : %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	    Kicker per inversos : %s (%d kick(s) per a ban)
BOT_INFO_CHAN_KICK_UNDERLINES
	  Kicker per subrallats : %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	  Kicker per subratllats : %s (%d kick(s) per a ban)
BOT_INFO_ACTIVE
	Activada
BOT_INFO_INACTIVE
	Desactivada
BOT_INFO_CHAN_OPTIONS
	               Opcions : %s
BOT_INFO_OPT_DONTKICKOPS
	Protecció de Ops
BOT_INFO_OPT_DONTKICKVOICES
	Protecció de Voices
BOT_INFO_OPT_FANTASY
	Fantasia
BOT_INFO_OPT_GREET
	Salutació
BOT_INFO_OPT_NOBOT
	Sense bot
BOT_INFO_OPT_SYMBIOSIS
	Simbiosis
BOT_INFO_OPT_NONE
	Cap
	
# SET responses
BOT_SET_SYNTAX
	SET canal Opció establiments
BOT_SET_DISABLED
	Ho sento, l'establiment d'opcions per a bot esta temporalment inhabilitat
BOT_SET_UNKNOWN
	Opció %s desconeguda.
	Escriu %R%S HELP SET per a més informació.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET canal DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Bot no kickejara ops en el canal %s.
BOT_SET_DONTKICKOPS_OFF
	Bot kickejara ops en el canal %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET canal DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Bot no kickejara voices en el canal %s.
BOT_SET_DONTKICKVOICES_OFF
	Bot kickejara voices en el canal %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET canal FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Mode fantasia ara activada en el canal %s.
BOT_SET_FANTASY_OFF
	Mode fantasia ara desactivat en el canal %s.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET canal GREET {ON|OFF}
BOT_SET_GREET_ON
	Mode salutació ara activat en el canal %s.
BOT_SET_GREET_OFF
	Mode salutació ara desactivat en el canal %s.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET canal NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Mode no bot ara activat en el canal %s.
BOT_SET_NOBOT_OFF
	Mode no bot ara desactivat en el canal %s.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET canal PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Mode privado del bot %s esta ara activat.
BOT_SET_PRIVATE_OFF
	Mode privado del bot %s esta ara desactivat.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET canal SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Mode Simbiosis ara activat en el canal %s.
BOT_SET_SYMBIOSIS_OFF
	Mode Simbiosis ara desactivat en el canal %s.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK canal opció {ON|OFF} [establiments]
BOT_KICK_DISABLED
	Ho sento, la configuració de kicker esta temporalment inabilitada.
BOT_KICK_UNKNOWN
	Opció %s desconeguda.
	Escriu %R%S HELP KICK per a més informació
BOT_KICK_BAD_TTB
	%s no pot ser pres com vegades a banegar

BOT_KICK_BADWORDS_ON
	El bot ara kickejara paraules malsonants. Utilitza la comanda BADWORDS
	per a afegir o treure paraules.
BOT_KICK_BADWORDS_ON_BAN
	El bot ara kickejara paraules malsonants, i banejara després de
	%d kicks al mateix usuari. Utilitza la comanda BADWORDS
	per a afegir o treure paraules.
BOT_KICK_BADWORDS_OFF
	El bot ja no kickejara paraules malsonants. 
BOT_KICK_BOLDS_ON
	El bot ara kickjeara negretes.
BOT_KICK_BOLDS_ON_BAN
	El bot ara kickjeara negretes, i banejara després de 
	%d kicks al mateix usuari.
BOT_KICK_BOLDS_OFF
	El bot ja no kickejara negretes.
BOT_KICK_CAPS_ON
	El bot ara kickejara majuscules (han de ser al menys
	%d caràcters i %d%% del missatge senser).
BOT_KICK_CAPS_ON_BAN
	El bot ara kickejara majusculas (han de ser al menys
	%d caràcters i %d%% del missatge senser), i banegara
	després de %d kicks al mateix usuari.
BOT_KICK_CAPS_OFF
	El bot ja no kickejara majuscules.
BOT_KICK_COLORS_ON
	El bot ara kickejara colors.
BOT_KICK_COLORS_ON_BAN
	El bot ara kickejara colors, i banejara després de %d 
	kicks al mateix usuari.
BOT_KICK_COLORS_OFF
	El bot ja no kickejara colors.
BOT_KICK_FLOOD_ON
	El bot ara kickejara flood (%d linees en %d segons).
BOT_KICK_FLOOD_ON_BAN
	El bot ara kickejara flood (%d linees in %d segons), i 
	banejara després de %d kicks al mateix usuari.
BOT_KICK_FLOOD_OFF
	El bot ja no kickejara flood.
BOT_KICK_REPEAT_ON
	El bot ara kickejara repeticions (usuaris que repeteixin %d vegades
	el mateix).
BOT_KICK_REPEAT_ON_BAN
	El bot ara kickejara repeticions (usuaris que repeteixin %d vegades
	el mateix), i banejara després de %d 
	kicks al mateix usuari.
BOT_KICK_REPEAT_OFF
	El bot ja no kickejara repeticions.
BOT_KICK_REVERSES_ON
	El bot ara kickejara reversos.
BOT_KICK_REVERSES_ON_BAN
	El bot ara kickejara reversos, i banejara després de %d 
	kicks al mateix usuari.
BOT_KICK_REVERSES_OFF
	El bot ja no kickejara reversos.
BOT_KICK_UNDERLINES_ON
	El bot ara kickejara subratllats.
BOT_KICK_UNDERLINES_ON_BAN
	El bot ara kickejara subratllats, i banejara després de %d 
	kicks al mateix usuari.
BOT_KICK_UNDERLINES_OFF
	El bot ja no kickejara subratllats.
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS canal {ADD|DEL|LIST|CLEAR} [paraula | llista] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	Ho sento, la llista de  paraules malsonants per a canals esta temporalment inabilitada.
BOT_BADWORDS_REACHED_LIMIT
	Ho sento, sols pots tenir %d paraules malsonants en un canal.
BOT_BADWORDS_ALREADY_EXISTS
	%s ja existeix a la llista de paraules malsonants de %s.
BOT_BADWORDS_ADDED
	%s afegida a  la llista de paraules malsonants de  %s.
BOT_BADWORDS_NO_SUCH_ENTRY
	No existeix la instància (#%d) a la llista de paraules malsonants de %s.
BOT_BADWORDS_NOT_FOUND
	%s no trobada a la llista de paraules malsonants de %s.
BOT_BADWORDS_NO_MATCH
	No existeixen paraules que concordin a la llista de paraules malsonants de %s.
BOT_BADWORDS_DELETED
	%s esborrada de la llista de paraules malsonants de %s.
BOT_BADWORDS_DELETED_ONE
	1 paraula esborrada de la llista de paraules malsonants de %s.
BOT_BADWORDS_DELETED_SEVERAL
	%d paraules esborrades de la llista de paraules malsonants de %s.
BOT_BADWORDS_LIST_EMPTY
	La llista de paraules malsonants de %s esta buida.
BOT_BADWORDS_LIST_HEADER
	llista de paraules malsonants per a %s:
	  Num   Paraula                        Tipo
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	La llista de paraules malsonants esta ara buida.

# SAY responses
BOT_SAY_SYNTAX
	SAY canal text

# ACT responses
BOT_ACT_SYNTAX
	ACT canal text

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
BOT_EXCEPT
	User matches channel except.

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
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
	Els serveis no han pogut canviar Modes. Estan ben configurats els teus servidors?
OPER_BOUNCY_MODES_U_LINE
	Els serveis no han pogut canviar Modes. Estan les linees U: dels teus servidors configurades correctament?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL missatge

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Opció STATS %s desconeguda.
OPER_STATS_CURRENT_USERS
	Actuals usuaris: %d (%d ops)
OPER_STATS_MAX_USERS
	Màxim  numero d'usuaris: %d (%s)
# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Serveis amunt %d dies, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Serveis amunt %d dies, %02d:%02d
OPER_STATS_UPTIME_HMS
	Serveis amunt %d hores, %d minuts
OPER_STATS_UPTIME_HM1S
	Serveis amunt %d hores, %d minuts
OPER_STATS_UPTIME_H1MS
	Serveis amunt %d hores, %d minut
OPER_STATS_UPTIME_H1M1S
	Serveis amunt %d hores, %d minut
OPER_STATS_UPTIME_1HMS
	Serveis amunt %d hora, %d minuts
OPER_STATS_UPTIME_1HM1S
	Serveis amunt %d hora, %d minuts
OPER_STATS_UPTIME_1H1MS
	Serveis amunt %d hora, %d minut
OPER_STATS_UPTIME_1H1M1S
	Serveis amunt %d hora, %d minut
OPER_STATS_UPTIME_MS
	Serveis amunt %d minuts, %d segons
OPER_STATS_UPTIME_M1S
	Serveis amunt %d minuts, %d segon
OPER_STATS_UPTIME_1MS
	Serveis amunt %d minut, %d segons
OPER_STATS_UPTIME_1M1S
	Serveis amunt %d minut, %d segon
OPER_STATS_BYTES_READ
	Bytes llegits   : %5d kB
OPER_STATS_BYTES_WRITTEN
	Bytes escrits : %5d kB
OPER_STATS_USER_MEM
	Usuari      : %6d registres, %5d kB
OPER_STATS_CHANNEL_MEM
	Canal        : %6d registres, %5d kB
OPER_STATS_GROUPS_MEM
	Grups NS    : %6d registres, %5d kB
OPER_STATS_ALIASES_MEM
	Aliases NS   : %6d registres, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ     : %6d registres, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ      : %6d registres, %5d kB
OPER_STATS_HOSTSERV_MEM
	HostServ      : %6d registres, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ     : %6d registres, %5d kB
OPER_STATS_SESSIONS_MEM
	Sessions     : %6d registres, %5d kB
OPER_STATS_AKILL_COUNT
	Numero actual de AKILLs: %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Temps d'expiració per defecte de AKILL: %d dies
OPER_STATS_AKILL_EXPIRE_DAY
	Temps d'expiració per defecte de AKILL: 1 dia
OPER_STATS_AKILL_EXPIRE_HOURS
	Temps d'expiració per defecte de AKILL: %d hores
OPER_STATS_AKILL_EXPIRE_HOUR
	Temps d'expiració per defecte de AKILL: 1 hora
OPER_STATS_AKILL_EXPIRE_MINS
	Temps d'expiració per defecte de AKILL: %d minuts
OPER_STATS_AKILL_EXPIRE_MIN
	Temps d'expiració per defecte de AKILL: 1 minut
OPER_STATS_AKILL_EXPIRE_NONE
	Temps d'expiració per defecte de AKILL: Sense expiració
OPER_STATS_SGLINE_COUNT
	Numero actual de SGLINEs: %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Temps d'expiració per defecte de SGLINE: %d dies
OPER_STATS_SGLINE_EXPIRE_DAY
	Temps d'expiració per defecte de SGLINE: 1 dia
OPER_STATS_SGLINE_EXPIRE_HOURS
	Temps d'expiració per defecte de SGLINE: %d hores
OPER_STATS_SGLINE_EXPIRE_HOUR
	Temps d'expiració per defecte de SGLINE: 1 hora
OPER_STATS_SGLINE_EXPIRE_MINS
	Temps d'expiració per defecte de SGLINE: %d minuts
OPER_STATS_SGLINE_EXPIRE_MIN
	Temps d'expiració per defecte de SGLINE: 1 minut
OPER_STATS_SGLINE_EXPIRE_NONE
	Temps d'expiració per defecte de SGLINE: Sense expiració
OPER_STATS_SQLINE_COUNT
	Numero actual de SQLINEs: %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Temps d'expiració per defecte de SQLINE: %d dies
OPER_STATS_SQLINE_EXPIRE_DAY
	Temps d'expiració per defecte de SQLINE: 1 dia
OPER_STATS_SQLINE_EXPIRE_HOURS
	Temps d'expiració per defecte de SQLINE: %d hores
OPER_STATS_SQLINE_EXPIRE_HOUR
	Temps d'expiració per defecte de SQLINE: 1 hora
OPER_STATS_SQLINE_EXPIRE_MINS
	Temps d'expiració per defecte de SQLINE: %d minuts
OPER_STATS_SQLINE_EXPIRE_MIN
	Temps d'expiració per defecte de SQLINE: 1 minut
OPER_STATS_SQLINE_EXPIRE_NONE
	Temps d'expiració per defecte de SQLINE: Sense expiració
OPER_STATS_SZLINE_COUNT
	Numero actual de SZLINEs: %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Temps d'expiració per defecte de SZLINE: %d dies
OPER_STATS_SZLINE_EXPIRE_DAY
	Temps d'expiració per defecte de SZLINE: 1 dia
OPER_STATS_SZLINE_EXPIRE_HOURS
	Temps d'expiració per defecte de SZLINE: %d hores
OPER_STATS_SZLINE_EXPIRE_HOUR
	Temps d'expiració per defecte de SZLINE: 1 hora
OPER_STATS_SZLINE_EXPIRE_MINS
	Temps d'expiració per defecte de SZLINE: %d minuts
OPER_STATS_SZLINE_EXPIRE_MIN
	Temps d'expiració per defecte de SZLINE: 1 minut
OPER_STATS_SZLINE_EXPIRE_NONE
	Temps d'expiració per defecte de SZLINE: Sense expiració
OPER_STATS_RESET
	Estadistiques restablertes.

# MODE responses
OPER_STATS_UPLINK_SERVER
	Uplink server: %s
OPER_STATS_UPLINK_CAPAB
	Uplink capab: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Servers found: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE canal Modes

# CLEARMODES responses
OPER_UMODE_SYNTAX
	UMODE nick modes

# OLINE responses
OPER_UMODE_SUCCESS
	Changed usermodes of %s.
OPER_UMODE_CHANGED
	%s changed your usermodes.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE nick flags
OPER_OLINE_SUCCESS
	An O:Line with the flags %s has been added for %s.

# CLEARMODES responses
OPER_OLINE_IRCOP
	You are now an IRC Operator.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES canal [ALL]
OPER_CLEARMODES_DONE
	Modes binaris i bans netegats del canal %s.
OPER_CLEARMODES_ALL_DONE
	Tots els Modes netegats del canal %s.


# KICK responses
OPER_KICK_SYNTAX
	KICK canal usuari raó
	
# ADMIN responses
OPER_SVSNICK_SYNTAX
	SVSNICK nick newnick 
OPER_SVSNICK_NEWNICK
	The nick %s is now being changed to %s.
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [nick | llista]
OPER_ADMIN_SKELETON
	Els serveis estan en Mode skeleton; la comanda ADMIN no esta disponible.
OPER_ADMIN_EXISTS
	%s ja esta present a la llista d'administradors de serveis.
OPER_ADMIN_REACHED_LIMIT
	Ho sento, sols pots tenir %d administradors de serveis.
OPER_ADMIN_ADDED
	%s afegit a la llista d'administradors de serveis.
OPER_ADMIN_NOT_FOUND
	%s no trobat a la llista d'administradors de serveis.
OPER_ADMIN_NO_MATCH
	No existeixen instàncies similars a la llista d'administradors de serveis.
OPER_ADMIN_DELETED
	%s esborrat de la llista d'administradors de serveis.
OPER_ADMIN_DELETED_ONE
	1 instància esborrada de la llista d'administradors de serveis.
OPER_ADMIN_DELETED_SEVERAL
	%d instàncies esborrades de la llista d'administradors de serveis.
OPER_ADMIN_LIST_EMPTY
	La llista d'administradors de serveis esta buida.
OPER_ADMIN_LIST_HEADER
	Llista d'administradors de serveis:
	  Num   Nick
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	La llista d'administradors de serveis ha estat netegada.
OPER_ADMIN_MOVED
	%s has been moved to Services Administrators list.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | llista]
OPER_OPER_SKELETON
	Els serveis estan en Mode skeleton; la comanda OPER no esta disponible.
OPER_OPER_EXISTS
	%s ja existeix a la llista d'operadors de serveis.
OPER_OPER_REACHED_LIMIT
	Ho sento, sols pots tenir %d operadors de serveis.
OPER_OPER_ADDED
	%s afegit a la llista d'operadors de serveis.
OPER_OPER_NOT_FOUND
	%s no trobat a la llista d'operadors de serveis.
OPER_OPER_NO_MATCH
	No existeixen instàncies similars a la llista d'operadors de serveis.
OPER_OPER_DELETED
	%s esborrat de la llista d'operadors de serveis.
OPER_OPER_DELETED_ONE
	1 instància esborrada de la llista d'operadors de serveis.
OPER_OPER_DELETED_SEVERAL
	%d instàncies esborrades de la llista d'operadors de serveis.
OPER_OPER_LIST_EMPTY
	La llista d'administradors de serveis esta buida.
OPER_OPER_LIST_HEADER
	Llista d'operadors de serveis:
	  Num   Nick
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	La llista d'operadors de serveis ha estat netegada.
OPER_OPER_MOVED
	%s has been moved to Services Operators list.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+expiració] {mascara | llista} [raó]]
OPER_AKILL_EXISTS
	%s ja esta present a la llista de AKILLs.
OPER_AKILL_ALREADY_COVERED
	%s ja esta cobert per %s.
OPER_AKILL_REACHED_LIMIT
	Ho sento, sols pots tenir %d AKILLs.
OPER_AKILL_NO_NICK
	Recordatori: Les mascares AKILL no poden contenir nicks; asegura't de no incloure el nick en la teva mascara.
OPER_AKILL_ADDED
	%s afegit a la llista de AKILLs.
OPER_AKILL_CHANGED
	Temps d'expiració per a %s canviat.
OPER_AKILL_NOT_FOUND
	%s no trobat a la llista de AKILLs.
OPER_AKILL_NO_MATCH
	No existeixen instàncies similars a la llista de AKILLs.
OPER_AKILL_DELETED
	%s esborrat de la llista de AKILLs.
OPER_AKILL_DELETED_ONE
	1 instància esborrada de la llista de AKILLs.
OPER_AKILL_DELETED_SEVERAL
	%d instàncies esborrades de la llista de AKILLs.
OPER_AKILL_LIST_EMPTY
	La llista de AKILLS esta buida.
OPER_AKILL_LIST_HEADER
	Llista actual de AKILLs:
	  Num   Mascara                           Raó
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Llista actual de AKILLs:
# number, mask, set-by, set-time, expires, reason
OPER_AKILL_VIEW_FORMAT
	%3d  %s (per %s en %s; %s)
	      %s
OPER_AKILL_CLEAR
	La llista de AKILLs ha estat netegada.
	
# SGLINE responses
OPER_CHANKILL_SYNTAX
	CHANKILL [+expiry] {#channel} [reason]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiració] {mascara | llista}[:raó]]
OPER_SGLINE_UNSUPPORTED
	Ho sento, SGLINE no esta disponible en aquesta xarxa.
OPER_SGLINE_EXISTS
	%s ja existeix a la llista de SGLINEs.
OPER_SGLINE_ALREADY_COVERED
	%s ja esta cobert per %s.
OPER_SGLINE_REACHED_LIMIT
	Ho sento, sols pots tenir %d SGLINEs.
OPER_SGLINE_ADDED
	%s afegit a la llista de SGLINEs.
OPER_SGLINE_CHANGED
	Temps d'expiració per a %s canviat.
OPER_SGLINE_NOT_FOUND
	%s no trobat a a la llista de SGLINEs.
OPER_SGLINE_NO_MATCH
	No existeixen instàncies similars a la llista de SGLINEs.
OPER_SGLINE_DELETED
	%s esborrat de la llista de SGLINEs.
OPER_SGLINE_DELETED_ONE
	1 instància esborrada de la llista de SGLINEs.
OPER_SGLINE_DELETED_SEVERAL
	%d instàncies esborrades de la llista de SGLINEs.
OPER_SGLINE_LIST_EMPTY
	La llista de SGLINEs esta buida.
OPER_SGLINE_LIST_HEADER
	Llista actual de SGLINEs:
	  Num   Mascara                           Raó
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Llista actual de SGLINEs:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (per %s en %s; %s)
	      %s
OPER_SGLINE_CLEAR
	La llista de SGLINEs ha estat netegada.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiració] {mascara | llista} [raó]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	SQLINEs de canal no estan soportades pel teu IRCd, així que no les pots utilitzar.
OPER_SQLINE_EXISTS
	%s ja existeix a la llista de SQLINEs.
OPER_SQLINE_ALREADY_COVERED
	%s ja esta cobert per %s.
OPER_SQLINE_REACHED_LIMIT
	Ho sento, sols pots tenir %d SQLINEs.
OPER_SQLINE_ADDED
	%s afegit a la llista de SQLINEs.
OPER_SQLINE_CHANGED
	Temps d'expiració per a %s canviat.
OPER_SQLINE_NOT_FOUND
	%s no trobat a la llista de SQLINEs
OPER_SQLINE_NO_MATCH
	No existeixen instàncies similars a la llista de SQLINEs.
OPER_SQLINE_DELETED
	%s esborrat de la llista de SQLINEs.
OPER_SQLINE_DELETED_ONE
	1 instància esborrada de la llista de SQLINEs.
OPER_SQLINE_DELETED_SEVERAL
	%d instàncies esborrades de la llista de SQLINEs.
OPER_SQLINE_LIST_EMPTY
	La llista de SQLINEs esta buida.
OPER_SQLINE_LIST_HEADER
	Llista actual de SQLINEs:
	  Num   Mascara                           Raó
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Llista actual de SQLINEs:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (per %s en %s; %s)
	      %s
OPER_SQLINE_CLEAR
	La llista de SQLINEs ha estat netegada.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiració] {mascara | llista} [raó]]
OPER_SZLINE_UNSUPPORTED
	Ho sento, la comanda SZLINE no esta disponible en aquesta xarxa.
OPER_SZLINE_EXISTS
	%s ja existeix a la llista de SZLINEs.
OPER_SZLINE_ALREADY_COVERED
	%s ja esta cobert per %s.
OPER_SZLINE_REACHED_LIMIT
	Ho sento, sols pots tenir %d SZLINEs.
OPER_SZLINE_ONLY_IPS
	Recordatori: Sols pots afegir mascares IP a la llista de SZLINEs.
OPER_SZLINE_ADDED
	%s afegit a la llista de SZLINEs.
OPER_SZLINE_CHANGED
	Temps d'expiració de %s canviat.
OPER_SZLINE_NOT_FOUND
	%s no trobat a la llista de SZLINEs.
OPER_SZLINE_NO_MATCH
	No existeixen instàncies similars a la llista de SZLINEs.
OPER_SZLINE_DELETED
	%s esborrat de la llista de SZLINEs.
OPER_SZLINE_DELETED_ONE
	1 instància esborrada de la llista de SZLINEs.
OPER_SZLINE_DELETED_SEVERAL
	%d instàncies esborrades de la llista de SZLINEs.
OPER_SZLINE_LIST_EMPTY
	La llista de SZLINEs esta buida.
OPER_SZLINE_LIST_HEADER
	Llista actual de SZLINEs:
	  Num   Mascara                           Raó
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Llista actual de SZLINEs:
# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (per %s a %s; %s)
	      %s
OPER_SZLINE_CLEAR
	La llista de SZLINEs ha estat netegada.

# SET responses
OPER_SET_SYNTAX
	SET Opció estabilment
OPER_SET_IGNORE_ON
	Codi ignore sera utilitzat.
OPER_SET_IGNORE_OFF
	Codi ignore no sera utilitzat.
OPER_SET_IGNORE_ERROR
	L'establiment per a IGNORE ha de ser ON o OFF.
OPER_SET_READONLY_ON
	Serveis ara en Mode nomès lectura.
OPER_SET_READONLY_OFF
	Serveis ara en Mode de lectura-escriptura.
OPER_SET_READONLY_ERROR
	L'establiment per a READONLY ha de ser ON o OFF.
OPER_SET_LOGCHAN_ON
	Services is now reporting log messages to %s.
OPER_SET_LOGCHAN_OFF
	Services is no longer reporting log messages to a channel.
OPER_SET_LOGCHAN_ERROR
	Setting for LOGCHAN must be ON or OFF and LogChannel must be defined.
OPER_SET_DEBUG_ON
	Serveis ara en Mode debug.
OPER_SET_DEBUG_OFF
	Serveis ara en Mode no-debug.
OPER_SET_DEBUG_LEVEL
	Serveis ara en Mode debug (nivell %d).
OPER_SET_DEBUG_ERROR
	L'establiment per a DEBUG ha de ser ON, OFF, o un numero positiu.
OPER_SET_NOEXPIRE_ON
	Serveis ara en Mode no expire.
OPER_SET_NOEXPIRE_OFF
	Serveis ara en Mode expire.
OPER_SET_NOEXPIRE_ERROR
	Establiment per a NOEXPIRE ha de ser ON o OFF.
OPER_SET_UNKNOWN_OPTION
	Opció %s desconeguda.
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
	%s is enable
OPER_SET_LIST_OPTION_OFF
	%s is disabled

# NOOP responses
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} servidor
OPER_NOOP_SET
	Totes les linees O de %s han estat esborrades.
OPER_NOOP_REVOKE
	Totes les linees O de %s han estat esborrades.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE nom-del-servidor [raó]

# RAW responses
OPER_JUPE_HOST_ERROR
	Please use a valid server name when juping
OPER_JUPE_INVALID_SERVER
	You can not jupe your services server or your uplink server.
	
# RAW responses
OPER_RAW_SYNTAX
	RAW text

# UPDATE responses
OPER_UPDATING
	Actualitzant les bases de dades.
	
# RELOAD responses
OPER_RELOAD
	L'arxiu de configuració de serveis ha estat carregat novament.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN no definit; impossible arrencar un altre cop. Arranca un altre cop el script \2configure\2  i recompila Services per a incloure la comanda RESTART.

# LISTIGNORE responses
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
	Llista ignore de serveis:
OPER_IGNORE_LIST_NOMATCH
	%s not found on ignore list.
OPER_IGNORE_LIST_EMPTY
	La llista ignore esta buida.

# KILLCLONES responses
OPER_IGNORE_LIST_CLEARED
	Ignore list has been cleared.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	Impossible trobar a l'usuari %s.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Llista de canal:
	Nom                 Modes de Usr. Tòpic
OPER_CHANLIST_HEADER_USER
	Llista del canal %s:
	Nom                 Modes de Usr. Tòpic
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	Fi de la llista de canals.
	
# USERLIST responses
OPER_USERLIST_HEADER
	Llista d'usuaris:
	Nick                 Mascara
OPER_USERLIST_HEADER_CHAN
	llista d'usuaris de %s:
	Nick                 Mascara
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Fi de la llista d'usuaris.

# CACHE responses
OPER_SUPER_ADMIN_ON
	You are now a SuperAdmin
OPER_SUPER_ADMIN_OFF
	You are no longer a SuperAdmin
OPER_SUPER_ADMIN_SYNTAX
	Setting for SuperAdmin must be ON or OFF.
OPER_SUPER_ADMIN_WALL_ON
	%s is now a Super-Admin
OPER_SUPER_ADMIN_WALL_OFF
	%s is no longer a Super-Admin

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_SUPER_ADMIN_ONLY
	Only Super-Admins can use this command.

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
	This service is temporarly disabled, please try again later
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

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
MODULE_HELP_HEADER
	The following commands have been loaded by a module:

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [parametres]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+expiració] mascara límit raó
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {mascara | llista}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE numero posició
OPER_EXCEPTION_DISABLED
	Límits de sessions no disponibles.
OPER_EXCEPTION_ALREADY_PRESENT
	La mascara %s ja esta present a la llista d'excepcions.
OPER_EXCEPTION_TOO_MANY
	La llista d'excepcions al límit de sessiones esta plena!
OPER_EXCEPTION_ADDED
	Límit de sessió per a %s establert a %d.
OPER_EXCEPTION_MOVED
	Excepció per a %s (#%d) moguda a la posició %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	La instància no existeix (#%d)  a la llista d'excepcions al límit de sessions.
OPER_EXCEPTION_NOT_FOUND
	%s no trobat a a la llista d'excepcions al límit de sessions.
OPER_EXCEPTION_NO_MATCH
	No existeixen instàncies similars a la llista d'excepcions al límit de sessions.
OPER_EXCEPTION_DELETED
	%s instàncies esborrades de la llista d'excepcions al límit de sessions.
OPER_EXCEPTION_DELETED_ONE
	1 instància esborrada de la llista d'excepcions al límit de sessions.
OPER_EXCEPTION_DELETED_SEVERAL
	%d instàncies esborrades de la llista d'excepcions al límit de sessions.
OPER_EXCEPTION_LIST_HEADER
	llista d'excepcions al límit de sessions actual:
# mask, limit
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Num  Límit Host
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (per %s a %s; %s)
	   Límit: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Límit de sessió invàlid. Ha de ser un sencer vàlido més gran o igual a zero i mes petit que %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Mascara invalida. Nomès mascares reals son valides ja que excepcions no son comparades a nicks o usernames.
OPER_EXCEPTION_EXISTS
	%s already exists on the EXCEPTION list.
OPER_EXCEPTION_CHANGED
	Exception for %s has been updated to %d.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST límit | VIEW host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST límit
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW host
OPER_SESSION_DISABLED
	El límit de sessió esta inhabilitat.
OPER_SESSION_INVALID_THRESHOLD
	Valor de umbral invàlid. Ha de ser un sencer vàlid més gran a 1.
OPER_SESSION_NOT_FOUND
	%s no trobat a la llista de sessió.
OPER_SESSION_LIST_HEADER
	Hosts amb un mínim de %d sessions:
OPER_SESSION_LIST_COLHEAD
	Sessions  Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	El host %s actualment te %d sessions amb un límit de %d.

# EXCEPTION help

OPER_HELP_EXCEPTION
	Sintaxis: EXCEPTION ADD [+expiració] mascara límit raó
	          EXCEPTION DEL {mascara | llista}
	          EXCEPTION MOVE numero posició
	          EXCEPTION LIST [mascara | llista]
	          EXCEPTION VIEW [mascara | llista]
	
	Permet als administradors de serveis manipular la llista
	de hosts que tenen un límit de sessió específic - permetent 
	a certes màquines, com servidors de shells, mantenir un nombre més alt 
	de clients al mateix temps. En quant un host arribi al seu límit
	de sessions, tots els clients intentant conectarse desde aquest 
	host seran expulsats. Abans de expulsar usuaris, aquests son
	notificats a traves de un /NOTICE del %S amb ajuda sobre
	el límit de sessió. El contingut d'aquest notice es un establiment
	de configuració.
	
	EXCEPTION ADD afegeix la mascara de host donada a la llista de
	excepcion. Teniu en compte que les mascares nick!usuari@host
	i usuari@host son invalides. Nomès mascares reals com
	irc.aujac.org i *.aujac.org, son permessas ja que el
	límit de sessions no compta nicks o noms d'usuari.
	Límit ha de ser un numero mes gran o igual a zero. Això determina
	quantes sessions el host pot tenir en un cert temps. El valor
	zero significa que el host no te límits de sessió. Veure la ajuda
	sobre AKILL per a més detalls sobre el parametre opcional
	expiració.
	EXCEPTION DEL remou la mascara donada de la llista d'excepcions.
	EXCEPTION MOVE mou l'excepció numero a posició. Les
	excepcions entremig seran mogudes amunt o avall per a omplir l'espai.
	
	EXCEPTION LIST i EXCEPTION VIEW mostra totes les
	excepcions actuals; si la mascara opcional es donada, la llista es
	limitada a les excepcions similars a la mascara. La diferencia
	es que EXCEPTION VIEW es mes verbal, mostrant el nom de la
	persona que va afegir l'excepció, el límit d'excepció, raó,
	mascara de hosts i la data i hora d'expiració. 
	
	Pren nota de que un client conectant-se "utilitzara" la primera
	excepció a la que el host s'assembli. Llargues llistes d'excepcions
	i mascares que s'assemblin a moltes mascares possiblement contribueixin
	al mal rendiment dels serveis.
	
	Limitat a administradors de serveis.

OPER_HELP_SESSION
	Sintaxis: SESSION LIST umbral
	          SESSION VIEW host
	
	Permet als administradors de serveis veure la llista de sessions.
	
	SESSION LIST llista hosts amb un mínim de umbral sessions.
	L'umbral ha de ser un numero superior a 1. Això es per a prevenir
	llistats accidentals del gran nombre de hosts de sessió.
	SESSION VIEW mostra informació detallada sobre un host 
	específic - incloent el compte de sessió i el límit de sessió
	actual.
	El valor host no pot incluir comodins.
	
	Mireu l'ajuda sobre EXCEPTION per a més informació sobre
	límits de sessió i como establir-los per a hosts i grups específics
	d'ara endavant. 
	
	Limitat a administradors de serveis.

###########################################################################
#
# News system messages
#
###########################################################################

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


###########################################################################
#
# News system messages
#
###########################################################################

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
	[Notícies de Logon - %s] %s
NEWS_OPER_TEXT
	[Notícies de Oper - %s] %s
NEWS_RANDOM_TEXT
	[Notícies al Atzar - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD|DEL|LIST} [text|numero]
NEWS_LOGON_LIST_HEADER
	Notícies d'entrada:
NEWS_LOGON_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_LOGON_LIST_NONE
	No hi ha notícies de entrada.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD texto
NEWS_LOGON_ADD_FULL
	La llista de notícies esta plena!
NEWS_LOGON_ADDED
	Noves notícies d'entrada afegida (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {numero | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Notícia d'entrada #%d no trobada!
NEWS_LOGON_DELETED
	Notícia d'entrada #%d esborrada.
NEWS_LOGON_DEL_NONE
	No hi ha notícies d'entrada per a esborrar!
NEWS_LOGON_DELETED_ALL
	Totes les notícies d'entrada esborrades.

NEWS_OPER_SYNTAX
	Sintaxis: OPERNEWS {ADD|DEL|LIST} [texto|numero]
NEWS_OPER_LIST_HEADER
	notícies de oper:
NEWS_OPER_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_OPER_LIST_NONE
	No hay notícies de oper.
NEWS_OPER_ADD_SYNTAX
	Sintaxis: OPERNEWS ADD texto
NEWS_OPER_ADD_FULL
	La llista de notícies esta plena!
NEWS_OPER_ADDED
	Nueva noticia de oper anadida (#%d).
NEWS_OPER_DEL_SYNTAX
	Sintaxis: OPERNEWS DEL {numero | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Noticia de oper #%d no trobada!
NEWS_OPER_DELETED
	Noticia de oper #%d esborrada.
NEWS_OPER_DEL_NONE
	No hi ha notícies de oper per esborrar!
NEWS_OPER_DELETED_ALL
	Totes les notícies de oper esborrades.

NEWS_RANDOM_SYNTAX
	RANDOMNEWS {ADD|DEL|LIST} [text|numero]
NEWS_RANDOM_LIST_HEADER
	Notícies al Azar:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s per %s)
	    %s
NEWS_RANDOM_LIST_NONE
	No hi ha notícies al azar.
NEWS_RANDOM_ADD_SYNTAX
	RANDOMNEWS ADD text
NEWS_RANDOM_ADD_FULL
	La llista de notícies esta plena!
NEWS_RANDOM_ADDED
	Nova noticia a l'atzar afegida (#%d).
NEWS_RANDOM_DEL_SYNTAX
	RANDOMNEWS DEL {numero | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Noticia a l'atzar #%d no trobada!
NEWS_RANDOM_DELETED
	Notícia a l'atzar #%d esborrada.
NEWS_RANDOM_DEL_NONE
	No hi ha notícies a l'atzar per esborrar!
NEWS_RANDOM_DELETED_ALL
	Totes les notícies al azar esborrades.

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
	Sintaxis: RANDOMNEWS ADD text
	          RANDOMNEWS DEL {numero | ALL}
	          RANDOMNEWS LIST
	
	Edita o mostra la llista de missatges de noticia a l'atzar.
	Quant un usuari es connecta a la xarxa, un (i nomès un) dels
	missatges a l'atzar sera escollit i enviat a ells.
	
	RANDOMNEWS may only be used by Services admins.

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S es un servei designat per donar informació sobre
	serveis. Tòpics d'ajuda estan disponibles via les
	comandes HELP dels altres clients de serveis:

	%R%s HELP
	     per mes informació sobre registrar nicknames

	%R%s HELP
	     per mes informació sobre registrar i controlar
	     canals

	%R%s HELP
	     per més informació sobre enviar missatges a usuaris no
	     en linea.

HELP_HELP_BOT

	%R%s HELP
	     per més informació sobre establir un bot en el teu canal

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
	    REGISTER   Registra un nickname
NICK_HELP_CMD_GROUP
	    GROUP      Ingresa en un grup
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY   Per identificar-te amb la teva clau
NICK_HELP_CMD_ACCESS
	    ACCESS     Modifica la llista d'adreçes autoritzades
NICK_HELP_CMD_SET
	    SET        Estableix proteccions, inclouent kills
NICK_HELP_CMD_SASET
	    SASET      Set SET-options on another nickname
NICK_HELP_CMD_DROP
	    DROP       Cancela el registre d'un nickname
NICK_HELP_CMD_RECOVER
	    RECOVER    Kickeja un usuari que estigui utilitzant el teu nickname
NICK_HELP_CMD_RELEASE
	    RELEASE    Repren possesió del teu nick després de RECOVER
NICK_HELP_CMD_SENDPASS
	    SENDPASS   Vas oblidar la teva clau? Intenta això
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
	    GETPASS    Recupera la clau per un nickname
	               (sols si encriptació esta deshabilitada)
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Matches and returns all users that registered using given email
NICK_HELP_CMD_FORBID
	    FORBID     Impossibilita que un nickname sigui utilitzat
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Suspend a given nick
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Unsuspend a given nick

NICK_HELP
	%S Et permet "registrar" un nickname i
	prevenir a d'altres utilitzar-lo. Les següents comandes
	permeten el registre i manteniment de nicknames;
	per a utilitzar-los, escriu %R%S comanda.
	per més informació sobre una comanda especifica
	escriu %R%S HELP comanda.
	
NICK_HELP_FOOTER
	NOTESE: Aquest servei intenta proveir una eina
	pels usuaris d'IRC d'assegurar-se que la seva identitat no esta
	compromessa, NO per facilitar el "robatori" de nicknames
	o un altre acció maliciosa. L'abus de %S resultara, com a
	mínim, en la perdua del nickname esmentat.

NICK_HELP_EXPIRES
	Nicknames en desus estan subjectes a l'expiració
	automatica, en altres paraules, seran esborrats després
	de %d dies.

NICK_HELP_REGISTER
	Sintaxis: REGISTER clau [email]
	
	Registra el teu nickname a la base de dades de %S. Un
	cop registrat, pots utilitzar les comandes SET i ACCESS
	per configurar els parametres del teu nick al teu gust.
	Assegura't de recordar la clau que vas fer servir per
	registrar-te - la necessitaras per fer canvis en el teu nick
	(Tingues en compte que les majuscules importen!  AUJAC,
	Aujac, i aujac són totes claus diferents!)
	
	Guies per escollir claus:
	
	Las claus no han de ser fàcils d'endevinar. Per exemple,
	utilitzar el teu nom real com clau es una mala idea. Utilitzar
	el teu nickname com a clau es una idea pitjor :( i,
	de fet, %S no ho permetria. A mes, claus curtes
	son vulnerables a ser encertades, aixi que hauries d'escollir una
	clau de com a mínim 5 caràcters.
	Per últim, el caràcter d'espai no pot ser utilitzat.
	
	El parametre email es opcional i establira l'email pel
	teu nick inmediatament. Com sigui, pot ser que sigui demanat
	en certes xarxes.
	La teva privacitat es respectada; aquest e-mail no sera donat a
	terceres persones.
	
	Aquesta comanda crea un nou grup pel teu nickname
	que et permetra registrar altres nicks després compartint
	la mateixa configuració, memos i privilegis. Per més
	informació sobre aquesta característica, escriu %R%S
	HELP GROUP.
	
NICK_HELP_GROUP
	Sintaxis: GROUP grupo clau
	
	Aquesta comanda fa que el teu nickname s'adhereixi al grup de nicknames
	grup. clau es la de teu nickname.
	
	L'unió a un grup et permetra compartir configuracions,
	memos, i privilegis en canals amb tots els nicknames en un
	grup, i molt més!
	 
	Un grup existeix sempre i quant sigui servible. Això
	significa que si un nick del grup es eliminat, no perdràs
	les caracteristiques compartides descrites anteriorment, sempre i
	quant al menys un nick quedi al grup.
	
	Pots utilitzar aquesta comanda encara que no estigui registrar el teu
	nick. Si el teu nick ja esta registrat hauras d'identificar-te
	abans d'utilitzar aquesta comanda. Escriu %R%S HELP
	IDENTIFY per més informació. Potser que no sigui possible
	a la teva xarxa.
	
	Es recomenable utilitzar aquesta comanda amb un nick no registrat ja
	que sera registrat automaticament quant l'utilitzis.
	Pots utilitzar-la amb un nick ja registrat (per canviar de grup) 
	sols si els administradors de la teva xarxa ho permeten.
	
	Pots estar en un sol grup a la vegada. L'unió de
	grups no es possible.
	
	Tingues en compte: tots els nicks en un grup comparteixen la
	mateixa clau.

NICK_HELP_IDENTIFY
	Sintaxis: IDENTIFY clau
	
	Informa a %S que tu ets realment el propietari del teu nick.
	Moltes comandes requereixen que t'identifiquis amb aquesta
	comanda abans d'utilitzar-les. La clau hauria de ser la mateixa
	que va ser enviada amb la comanda REGISTER.
	
NICK_HELP_UPDATE
	Syntax: UPDATE

	Updates your current status, i.e. it checks for new memos,
	sets needed chanmodes (ModeonID) and updates your vhost and
	your userflags (lastseentime, etc).
	
NICK_HELP_LOGOUT
	Sintaxis: LOGOUT
	
	Aquesta comanda inverteix l'efecte de IDENTIFY, o sigui, 
	et fa ja no reconeixible com a propietari del teu nick.
	Tingues en compte que no et sera obligatori identificar-te
	un altre cop.

NICK_HELP_DROP
	Sintaxis: DROP [nickname]
	
	Elimina el teu nickname de la base de dades de %S. Un nick
	que ha estat eliminat esta lliure per que qualsevol persona
	el registri.
	
	Pots eliminar un nick en el teu grupo indicant-lo com a
	parametre nick.
	
	Abans d'utilitzar aquesta comanda has d'identificar-te amb la teva
	clau (%R%S HELP IDENTIFY per més informació).

NICK_HELP_ACCESS
	Sintaxis: ACCESS ADD mascara
	          ACCESS DEL mascara
	          ACCESS LIST
	
	Modifica o mostra la llista d'accés pel teu nick. Aquesta
	és la llista d'adreces que seran automaticament
	reconegudes per %S com autoritzades a utilitzar el nick. Si
	vols utilitzar el nick des d'una adreça diferent,
	has d'enviar una comanda IDENTIFY per fer que %S et
	reconegui.
	
	Exemples:
	
	    ACCESS ADD anyone@*.aujac.org
	        Permet accés a l'usuari anyone des de
	        qualsevol maquina en el domini aujac.org.
	
	    ACCESS DEL anyone@*.aujac.org
	        Anul.la la comanda anterior.
	
	    ACCESS LIST
	        Mostra la llista d'accés actual.

NICK_HELP_SET
	Sintaxis: SET Opció parametres
	
	Estableix varies opcions per a nicknames. Opció pot ser
	una de:
	
	    DISPLAY    Estableix el display del teu grup amb serveis
	    PASSWORD   Estableix la clau del teu nickname
	    LANGUAGE   Estableix el llenguatge amb el que els serveis
	                   t'enviaran missatges
	    URL        Associa una URL amb el teu nickname
	    EMAIL      Associa un E-mail amb el teu nickname
	    ICQ        Associa un numero ICQ amb el teu nickname
	    GREET      Associa un salutació amb el teu nickname
	    KILL       Activa o desactiva la protecció
	    SECURE     Activa o desactiva la seguretat de nickname
	    PRIVATE    Preveu el teu nickname d'apareixer en un
	                   %R%S LIST
	    HIDE       Oculta algunes parts de la teva informació
	    MSG        canvia el metode de comunicació amb els
	                    serveis
	    AUTOOP     Should services op you automatically.    
	
	Per utilitzar aquest comanda, has d'identificar-te primer 
	utilitzant la teva clau (%R%S HELP IDENTIFY
	per mes informació).
	
	Escriu %R%S HELP SET opció per mes
	informació sobre una opció especifica.

NICK_HELP_SET_DISPLAY
	Sintaxis: SET DISPLAY nou-display
	
	canvia el display utilitzat per referir-te al teu grup de
	nicks en serveis. El nou display HA DE SER un nick
	del teu grup.

NICK_HELP_SET_PASSWORD
	Sintaxis: SET PASSWORD nueva-clau
	
	Canvia la clau utilitzada per identificar-te com el
	propietari del nick.

NICK_HELP_SET_LANGUAGE
	Sintaxis: SET LANGUAGE numero
	
	Canvia el llenguatge que serveis utilitza per enviar-te missatges
	(per exemple, al respondre a una comanda que enviis).
	numero ha de ser escollit de la llista de llenguatges
	soportats:
	

NICK_HELP_SET_URL
	Sintaxis: SET URL url
	
	Associa l'URL donada amb el teu nickname. Aquesta URL
	sera mostrada quant algu demani informació
	sobre el teu nick amb la comanda INFO.

NICK_HELP_SET_EMAIL
	Sintaxis: SET EMAIL direccion
	
	Associa el E-mail donat amb el teu nickname. L'e-mail
	sera mostrat quant algu demani informació
	sobre el teu nick amb la comanda INFO.
	
NICK_HELP_SET_ICQ
	Sintaxis: SET ICQ numero
	
	Associa el numero ICQ donat amb el teu nickname. Aquest
	numero sera mostrat quant algu soliciti
	informació sobre el teu nick amb la comanda INFO.
	
NICK_HELP_SET_GREET
	Sintaxis: SET GREET missatge
	
	Fa que el missatge introduit sigui la salutació pel teu nickname, 
	el que sera mostrat quant entris a un canal amb la
	opció GREET habilitada, sempre i quant tinguis 
	l'accés necessari.

NICK_HELP_SET_KILL
	Sintaxis: SET KILL {ON | QUICK | IMMED | OFF}
	
	Canvia la protecció automàtica del teu nick a on o off.
	Amb protecció on, si un altre usuari intenta utilitzar el teu nick,
	tindra un minut per canviar de nick, desprès del qual
	%S canviara el seu nick automaticament.
	
	Si estableixes QUICK, l'usuari tibdrà nomès 20 segons
	per canviar el nick en comptes dels 60 habituals. Si selecciones
	IMMED, el nick del usuari sera canviat automaticament
	sense ser avisat ni donar-li oportunitat de canviar-lo abans;
	per favor no utilitzis aquesta opció excepte que ho consideris
	necessari. A mes, els administradors de la xarxa poden haber
	deshabilitat aquesta opció.

NICK_HELP_SET_SECURE
	Sintaxis: SET SECURE {ON | OFF}
	
	Canvia les caracteristiques de seguretat de %S pel teu nick a on
	u off. Amb SECURE establert, has d'especificar la teva clau 
	per ser reconegut com propietari del nick, a pesar de que la teva adreça
	estigui a la llista d'accéss. D'altra banda, si estas a la llista 
	d'accés, %S no te auto-kickejara no important l'establiment de l'opció
	KILL.

NICK_HELP_SET_PRIVATE
	Sintaxis: SET PRIVATE {ON | OFF}
	
	Fa que %S activi o desactivi l'opció de privacitat del teu
	nick. Amb PRIVATE establert,el teu nickname no apareixera en les
	llistes de nicknames generades amb la comanda LIST de %S.
	(D'altra banda, qualsevol persona que sapigue el teu nick podra comprobar
	la teva informació utilitzant la comanda INFO.)

NICK_HELP_SET_HIDE
	Sintaxis: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Permet prevenir que certa informació sobre el teu nick sigui donada
	quant es sol.licitada mitjançant la comanda %S INFO.
	Pots ocultar la teva adreça e-mail (EMAIL), l'ultima
	mascara usuari@host (USERMASK), i el teu últim missatge
	quit (QUIT).
	El segon parametre especifica si la informació ha de ser
	ocultada (OFF) o mostrada (ON).
	
NICK_HELP_SET_MSG
	Sintaxis: SET MSG {ON | OFF}
	
	Et permet escollir la forma en la que els serveis es
	comunicaran amb tu. Amb MSG establert, els serveis
	utilitzaran msgs, de l'altra forma utilitzaran notícies. 

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
	
	Limited to Services admins.

NICK_HELP_SASET_DISPLAY
	Syntax: SASET nickname DISPLAY new-display
	
	Changes the display used to refer to the nickname group in 
	Services. The new display MUST be a nick of the group.
	
	Limited to Services admins.

NICK_HELP_SASET_PASSWORD
	Syntax: SASET nickname PASSWORD new-password
	
	Changes the password used to identify as the nick's	owner.
	
	Limited to Services admins.

NICK_HELP_SASET_URL
	Syntax: SASET nickname URL url
	
	Associates the given URL with the nickname.  This URL
	will be displayed whenever someone requests information
	on the nick with the INFO command.
	
	Limited to Services admins.

NICK_HELP_SASET_EMAIL
	Syntax: SASET nickname EMAIL address
	
	Associates the given E-mail address with the nickname.
	
	Limited to Services admins.
	
NICK_HELP_SASET_ICQ
	Syntax: SASET nickname ICQ number
	
	Associates the given ICQ number with the nickname.  This 
	number will be displayed whenever someone requests 
	information on the nick with the INFO command.
	
	Limited to Services admins.
	
NICK_HELP_SASET_GREET
	Syntax: SASET nickname GREET message
	
	Makes the given message the greet of the nickname, that
	will be displayed when joining a channel that has GREET
	option enabled, provided that the user has the necessary 
	access on it.
	
	Limited to Services admins.

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
	
	Limited to Services admins.

NICK_HELP_SASET_SECURE
	Syntax: SASET nickname SECURE {ON | OFF}
	
	Turns %S's security features on or off for your
	nick.  With SECURE set, you must enter your password
	before you will be recognized as the owner of the nick,
	regardless of whether your address is on the access
	list.  However, if you are on the access list, %S
	will not auto-kill you regardless of the setting of the
	KILL option.
	
	Limited to Services admins.

NICK_HELP_SASET_PRIVATE
	Syntax: SASET nickname PRIVATE {ON | OFF}
	
	Turns %S's privacy option on or off for the nick.
	With PRIVATE set, the nickname will not appear in
	nickname lists generated with %S's LIST command.
	(However, anyone who knows the nickname can still get
	information on it using the INFO command.)
	
	Limited to Services admins.

NICK_HELP_SASET_HIDE
	Syntax: SASET nickname HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	Allows you to prevent certain pieces of information from
	being displayed when someone does a %S INFO on the
	nick.  You can hide the E-mail address (EMAIL), last seen
	user@host mask (USERMASK), the services access status
	(STATUS) and  last quit message (QUIT).
	The second parameter specifies whether the information should
	be displayed (OFF) or hidden (ON).
	
	Limited to Services admins.
	
NICK_HELP_SASET_MSG
	Syntax: SASET nickname MSG {ON | OFF}
	
	Allows you to choose the way Services are communicating with 
	the given user. With MSG set, Services will use messages,
	else they'll use notices.
	
	Limited to Services admins.

NICK_HELP_SASET_NOEXPIRE
	Syntax: SASET nickname NOEXPIRE {ON | OFF}
	
	Sets whether the given nickname will expire.  Setting this
	to ON prevents the nickname from expiring.
	
	Limited to Services admins.

NICK_HELP_SASET_AUTOOP
	Syntax: SASET nickname AUTOOP {ON | OFF}
	
	Sets whether the given nickname will be opped automatically.
	Set to ON to allow ChanServ to op the given nickname 
	automatically when joining channels.
	
	Limited to Services admins.

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
	Sintaxis: GHOST nickname [clau]
	
	Termina una sessió IRC "fantasma" utilitzant el teu nick. Una
	sessió fantasma és una connexió falsa, de tal manera que els
	servidors IRC creuen esta en linea per una raó o altre.
	Tipicament, això succeix si el teu ordinador es bloqueja o la
	teva connexió a internet es talla mentres estas
	a IRC.
	
	Per utilitzar la comanda GHOST per un nick, la teva
	adreça actual mostrada a /WHOIS ha d'estar a la llista
	d'accés d'aquest nick, has d'estar identificat i en el
	grup d'aquest nick, o has de proveir la clau correcta
	per aquest nickname.

NICK_HELP_INFO
	Sintaxis: INFO nickname [ALL]
	
	Mostra informació relativa al nick que s'especifiqui,
	tal com el propietari del nick, l'ultima adreça utilitzada
	i temps, i opcions del nick. Si estas identificat amb el
	nick per el qual demanes informació i utilitzes el parametre
	ALL, podras veure tota la informació; independentment de
	si esta oculta o no.

NICK_HELP_LIST
	Sintaxis: LIST patró
	
	Llista tots els nicknames que concordin amb el patró
	donat, en el format nick!usuari@host. Nicks amb 
	l'opció PRIVATE establerta no seran mostrats.
	
	Exemples:
	
	    LIST *!cistus@aujac.org
	        Llista tots els nicks registrats per cistus@aujac.org.
	
	    LIST *Bot*!*@*
	        Llista tots els nicks registrats amb Bot en els seus
	        noms (sensible a majuscules i miniscules).
	
	    LIST *!*@*.aujac.org
	        Llista tots els nicks registrats per usuaris amb el
	        domini aujac.org.

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
	        Lists all channels where you have AOP access 
	    or greater.

	    ALIST 10
	        Lists all channels where you have level 10 
	    access or greater.
	
	Channels that have the NOEXPIRE option set will be prefixed by 
	an exclamation mark.
	
NICK_HELP_GLIST
	Sintaxis: GLIST
	
	Llista tots els nicks en el teu grup.

NICK_HELP_STATUS
	Sintaxis: STATUS nickname...
	
	Retorna informació sobre si l'usuari que utilitza el nick especificat
	ha estat reconegut com el propietari del nickname.
	La resposta te el següent format:
	
	    nickname codi-del-status
	
	A on nickname es el nickname enviat amb la comanda, i
	codi-del-status es un dels següents:
	
	    0 - Usuari no esta en linea o el nick no esta registrat.
	    1 - Usuari no reconegut com el propietari del nick.
	    2 - Usuari reconegut sols mitjançant la llista d'accés.
	    3 - Usuari reconegut mitjançant autentificació per clau.
	
	Fins 16 nicknames poden ser enviats amb cada comanda. El
	reste seran ignorats. If no nickname is given, your status
	will be returned.

NICK_HELP_SENDPASS
	Sintaxis: SENDPASS nickname
	
	Envia la clau del nickname donat a l'adreça
	e-mail establerta en el registre del nickname. Aquesta comanda
	es sumament útil per recuperar claus perdudes.
	
	Pot ser limitat a operadors d'IRC en certes
	xarxes.
	
	Aquesta comanda no esta disponible quant encriptació esta habilitada.

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
	Administradors de serveis poden també eliminar qualsevol
	nickname sense tenir que identificar-se pel nick i poden
	veure la llista d'accés per qualsevol nickname
	(%R%S ACCESS LIST nick).

NICK_SERVADMIN_HELP_LOGOUT
	Sintaxis: LOGOUT [nickname [REVALIDATE]]
	
	Sense un parametre, desfà l'efecte de la comanda IDENTIFY, 
	és a dir, fa que no siguis reconegut com l'autentic propietari del nick
	Tingues en compte, que no seras necessari tornar-te a identificar.
	
	Amb un parametre, fa lo mateix pel nick donat. Si
	a més a més especifiques REVALIDATE, els serveis requeriran
	a l'esmentat nick a reidentificar-se. Aquest ús esta limitat a
	administradors de serveis.

NICK_SERVADMIN_HELP_DROP
	Sintaxis: DROP [nickname]
	
	Sense un parametre, elimina el teu nickname de la base de dades de
	%S.
	
	Amb un parametre, elimina el nick esmentat de la base de
	dades. Pots eliminar qualsevol nick del teu grup sense
	privilegis especials. Eliminar qualsevol nick esta
	limitat a administradors de serveis.

NICK_SERVADMIN_HELP_INFO
	
	Administradors de serveis poden utilitzar el parametre
	ALL amb qualsevol nick.

NICK_SERVADMIN_HELP_LIST
	Sintaxis: LIST patró [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	Llista tots els nicknames registrats que concordin amb el
	patró donat, en el format nick!usuari@host. Nicks amb
	L'opció PRIVATE tan sols sera mostrada a administradors
	de serveis. Nicks amb l'opció NOEXPIRE establerta tindran
	un ! avantposat al nickname per a administradors de
	serveis.
	
	If the FORBIDDEN, SUSPENDED, NOEXPIRE or UNCONFIRMED options are given, only
	nicks which, respectively, are FORBIDDEN, SUSPENDED, UNCONFIRMED or have the
	NOEXPIRE flag set will be displayed. If multiple options are
	given, all nicks matching at least one option will be displayed.
	These options are limited to Services admins.  
	
	Exemples:
	
	    LIST *!cistus@aujac.org
	        Llista tots els nicks registrats per cistus@aujac.org.
	
	    LIST *Bot*!*@*
	        Llista tots els nicks registrats amb Bot en
	        els seus noms. (no sensible a majuscules i minuscules).
	
	    LIST * NOEXPIRE
	        Llista tots els nicks registrats que hagin estat establerts
	        a no caducitat.

NICK_SERVADMIN_HELP_ALIST
	Syntax: ALIST [nickname] [level]
	
	With no parameters, lists channels you have access on. With
	one parameter, lists channels that nickname has access 
	on. With two parameters lists channels that nickname has 
	level access or greater on.

	This use limited to Services admins.

NICK_SERVADMIN_HELP_GLIST
	Sintaxis: GLIST [nickname]
	
	Sense un parametre, llista tots els nicknames en el teu
	grup.
	
	Amb un parametre, llista tots els nicknames que hi ha
	en el grup del nick esmentat.
	El seu ús esta limitat a administradors de serveis.

NICK_SERVADMIN_HELP_GETPASS
	Sintaxis: GETPASS nickname
	
	Retorna la clau pel nickname donat. Tingues en compte
	que quant aquesta comanda es utilitzada, un missatge inclouent
	la persona que va executar la comanda i el nickname en el qual
	va ser utilitzat sera establert i enviat com WALLOPS/GLOBOPS.
	
	Limitat a administradors de serveis.
	
	Aquesta comanda no esta disponible si encriptació esta habilitada.

NICK_SERVADMIN_HELP_GETEMAIL
	Syntax: GETEMAIL user@emailhost

	Returns the matching nicks that used given email. Note that
	you can not use wildcards for either user or emailhost. Whenever
	this command is used, a message including the person who issued
	the command and the email it was used on will be logged.

	Limited to Services admins.

NICK_SERVADMIN_HELP_FORBID
	Sintaxis: FORBID nickname [raó]
	
	Impedeix que un nickname sigui registrat o utilitzat per algú.
	Pot ser cancel.lat eliminat el nick.
	
	En algunes xarxes, la raó es necessaria.
	
	Limitat a administradors de serveis.

###########################################################################
#
# ChanServ help messages
#
###########################################################################

NICK_SERVADMIN_HELP_SUSPEND
	Syntax: SUSPEND nickname reason / UNSUSPEND nickname

	SUSPENDs / UNSUSPENDs a nickname from being identified to
	
	Limited to Services admins.

###########################################################################
#
# ChanServ help messages
#
###########################################################################

NICK_SERVADMIN_HELP_UNSUSPEND
	Syntax: UNSUSPEND nickname

	UNSUSPENDS a nickname from being used.

	Limited to Services admins.

###########################################################################
#
# ChanServ help messages
#
###########################################################################
CHAN_HELP_CMD_GETPASS
	    GETPASS    Retorna la clau de fundador per a un canal
	               (sols si l'encriptació esta deshabilitada)
CHAN_HELP_CMD_FORBID
	    FORBID     Evita que un canal es faci servir
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    Prevent a channel from being used preserving
	               channel data and settings
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Releases a suspended channel
CHAN_HELP_CMD_STATUS
	    STATUS     Returns the current access level of a user
	               on a channel
CHAN_HELP_CMD_REGISTER
	    REGISTER   Registra un canal
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   T'identifica amb la teva clau
CHAN_HELP_CMD_SET
	    SET        Establir opcions i informació d'un canal
CHAN_HELP_CMD_AOP
	    AOP        Modifica la llista d'usuaris AOP
CHAN_HELP_CMD_SOP
	    SOP        Modifica la llista d'usuaris SOP
CHAN_HELP_CMD_ACCESS
	    ACCESS     Modifica la llista d'usuaris privilegiats
CHAN_HELP_CMD_LEVELS
	    LEVELS     Redefine el significat de nivells d'accés
CHAN_HELP_CMD_AKICK
	    AKICK      Manté la llista d'AutoKick
CHAN_HELP_CMD_DROP
	    DROP       Cancel.la l'enregistrament d'un canal
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Ajuda a recuperar claus perdudes
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
	%S et permet registrar i controlar varis
	aspectes dels canals. Sovint, %S pot prevenir
	a usuaris maliciosos "pendre" canals limitant
	qui esta permés a tenir privilegis d'operador.
	Les comandes disponibles estan llistades a continuació;
	Per a utilitzar-los, escriu %R%S comanda.
	Per més informació sobre una comanda especifica,
	escriu %R%S HELP comanda.
	 

CHAN_HELP_EXPIRES
	
	Tingues en compte que qualsevol canal que no sigui utilitzat durant
	%d dies (al qual cap usuari a la llista d'accés
	del canal entri en aquest periode de temps) sera
	automaticament eliminat.

CHAN_HELP_REGISTER
	Sintaxis: REGISTER canal clau descripció
	
	Registra un canal en la base de dades de %S. Per a
	utilitzar aquesta comanda, has de ser un operador en el
	canal que intentis registrar. La clau es utilitzada amb
	la comanda IDENTIFY per a permetre a altres fer
	canvis en les opcions del canal posteriorment.
	L'últim parametre, el qual ha de ser inclòs, és
	una descripció general del propòsit del canal.
	
	Quant registres un canal, ets reconegut com el
	"fundador" del canal. El fundador del canal pot
	canviar tots els parametres pel canal;
	%S també donarà al fundador privilegis d'operador
	automaticamente quant l'usuari entri al canal.
	Esbrina sobre la comanda ACCESS
	(%R%S HELP ACCESS) Per més informació sobre
	com donar aquests privilegis a altres usuaris del canal.
	
	NOTA: Per registrar un canal has d'haver registrat
	el teu nickname primer. Si encara no ho has fet, escriu
	%R%s HELP per més informació de com fer-ho.

CHAN_HELP_IDENTIFY
	Sintaxis: IDENTIFY canal clau
	
	T'identifica amb %S com el fundador del canal especificat.
	Moltes comandes requereixen que utilitzis aquesta comanda abans
	d'utilitzar-los. La clau hauria de ser la mateixa que
	vas enviar amb la comanda REGISTER.
	
CHAN_HELP_LOGOUT
	Sintaxis: LOGOUT canal nickname
	
	Aquest comanda fa que el nickname donat ja no siguis identificat
	en el canal especificat.
	
	Si ets el fundador del canal, pots desconectar a
	qui sigui, d'altra banda tan sols et pots desconnectar
	a tu mateix.

CHAN_HELP_DROP
	Sintaxis: DROP canal
	
	Desenregistra el canal especificat. Nomès pot ser utilitzat pel
	fundador del canal, qui ha d'utilitzar primer la comanda
	 IDENTIFY.

CHAN_HELP_SET
	Sintaxis: SET canal opció parametres
	
	Li permet al fundador del canal establir varies opcions
	del canal i altres informacions.
	
	Opcions disponibles:
	
	    FOUNDER       Estableix el fundador d'un canal
	    SUCCESSOR     Estableix el successor d'un canal
	    PASSWORD      Estableix el password del fundador
	    DESC          Estableix la descripció del canal
	    URL           Associa una URL amb el canal
	    EMAIL         Associa un E-mail amb el canal
	    ENTRYMSG      Estableix un missatge a enviar als
	                  usuaris quant entrin al canal
	    BANTYPE       Estableix com els serveis posen bans
	                  en el canal
	    MLOCK         Fixa els Modes de canal on o off
	    KEEPTOPIC     Rete el topic quant el canal no
	                  esta en uso
	    OPNOTICE      Envia una noticia quant les comandes
	                  OP/DEOP son utilitzats
	    PEACE         Regula l'ús de comandes critiques
	    PRIVATE       Oculta el canal de la comanda LIST
	    RESTRICTED    Restringeix l'accés al canal
	    SECURE        Activa les opcions de seguretat de %S
	    SECUREOPS     Estricte control de l'estatus de chanop
	    SECUREFOUNDER Estricto control de l'estatus de fundador
	                  del canal
	    SIGNKICK      Firma kicks fets amb la comanda KICK
	    TOPICLOCK     El topic sols pot ser canviat amb
	                  TOPIC
	    XOP           Canvia el sistema de privilegis d'usuari
	
	Escriu %R%S HELP opció per més informació
	sobre una opció en particular.

CHAN_HELP_SET_FOUNDER
	Sintaxis: SET canal FOUNDER nick
	
	Canvia el fundador d'un canal. El nou nickname ha de
	ser un d'enregistrat.

CHAN_HELP_SET_SUCCESSOR
	Sintaxis: SET canal SUCCESSOR nick
	
	Canvia el successor d'un canal. Si el nickname fundador
	caduca o es eliminat mentres el canal esta encara registrat,
	el successor es convertira en el nou fundador del canal.
	Però, si el successor ja te molts canals
	registrats (%d), el canal sera eliminat, tal i com
	si no hi haguès un successor. El nou nickname ha de ser un
	d'enregistrat.

CHAN_HELP_SET_PASSWORD
	Sintaxis: SET canal PASSWORD clau
	
	Estableix la clau utilitzada per identificar-se com el
	fundador del canal.

CHAN_HELP_SET_DESC
	Sintaxis: SET canal DESC descripció
	
	Estableix la descripció pel canal, la que es mostrada
	amb les comandes LIST i INFO.

CHAN_HELP_SET_URL
	Sintaxis: SET canal URL [url]
	
	Associa l'URL especificada amb el canal. Aquesta URL sera mostrada
	quant algu sol.liciti informació sobre el canal amb
	la comanda INFO. Si no s'especifica cap parametre, 
	esborra la URL actual pel canal.

CHAN_HELP_SET_EMAIL
	Sintaxis: SET canal EMAIL [direcció]
	
	Associa l'adreça e-mail especificada amb el canal.
	Aquesta adreça sera mostrada quant algu sol.liciti
	informació sobre el canal amb la comanda INFO.
	Si cap parametre es establert, borra l'adreça e-mail
	actual pel canal.

CHAN_HELP_SET_ENTRYMSG
	Sintaxis: SET canal ENTRYMSG [missatge]
	
	Especifica el missatge que sera enviat via /notice als
	usuaris quant ingressin al canal. Si cap parametre
	es especificat, cap missatge sera enviat al entrar al canal.
	
CHAN_HELP_SET_BANTYPE
	Sintaxis: SET canal BANTYPE tipus_de_ban
	
	Especifica el tipus de ban que sera utilitzat pels
	serveis quant necessitin banear algu
	del teu canal.
	
	Tipus de ban es un numero entre 0 i 3 que significa:
	
	0: ban de la forma *!usuari@host
	1: ban de la forma *!*usuari@host
	2: ban de la forma *!*@host
	3: ban de la forma *!*usuari@*.domini

CHAN_HELP_SET_KEEPTOPIC
	Sintaxis: SET canal KEEPTOPIC {ON | OFF}
	
	Habilita o deshabilita l'opció de retenció de topic 
	per un canal. Quant la retenció de topic esta
	establerta, el topic pel canal sera recordat 
	%S fins i tot després de que l'últim usuari
	sorti del canal, i sigui restaurat la pròxima vegada que
	el canal sigui creat.

CHAN_HELP_SET_TOPICLOCK
	Sintàxi: SET canal TOPICLOCK {ON | OFF}
	
	Habilita o deshabilita L'opció de fixació de topic
	per a un canal. Quan la fixació de topic està
	establerta, %S no permetrà que el topico del canal
	sigui canviat excepte amb la comanda TOPIC.

CHAN_HELP_SET_MLOCK
	Sintàxi: SET canal MLOCK Modes
	
	Estableix el paràmetre de fixació de Modes pel canal.
	%S et permet definir certs Modes de canal per a estar
	sempre encesos o apagats o lliures d' estar un dels
	anteriors.
	
	El paràmetre Modes és construit de la mateixa forma
	que La comanda /MODE; això significa, Modes seguits
	per un + estan fixats encesos, i Modes seguits per
	un - estan fixats apagats. tenir en compte que, a
	difèrencia de La comanda MODE, cada us de SET MLOCK
	esborrarà tots els Modes fixats prèviament abans de
	establir els nous!
	
	AdVeuretència: Si estableixes una clau amb fixació de Mode, com
	en el segon exemple de més endevant, hauràs també d'establir la
	Opció RESTRICTED pel canal (veure HELP SET RESTRICTED),
	o qui sigui que entri al canal quan aquest és buit podrà veure la
	clau!
	
	Exemples:
	
	    SET #canal MLOCK +nt-iklps
	        Força els Modes n i t encesos, i els Modes i, k,
	        l, p i s apagats. El Mode m és deixat lliure per a
	        estar encès o apagat.
	
	    SET #canal MLOCK +knst-ilmp mi-key
	        Força els Modes k, n, s, i t encesos, i els Modes
	        i, l, m, i p apagats. També força la clau del canal
	        a ser "mi-clau".
	
	    SET #canal MLOCK +
	        Esborra la fixació de Modes; tots els Modes del canal
	        són lliures d'estar encesos o apagats.

CHAN_HELP_SET_PEACE
	Sintàxi: SET canal PEACE {ON | OFF}
	
	Habilita o deshabilita l'opció de pau per a un
	canal. Quan pau està establerta, un usuari no
	podra expulsar, banejar o esborrar un estatus de canal
	d'un usuari amb nivelll igual o superior a ell via
	comandes de %S.

CHAN_HELP_SET_PRIVATE
	Sintàxi: SET canal PRIVATE {ON | OFF}
	
	Habilita o deshabilita l'opció de privacitat per a un canal.
	Quan privacitat està establerta, un %R%S LIST no
	inclourà el canal en cap llista.

CHAN_HELP_SET_RESTRICTED
	Sintàxi: SET canal RESTRICTED {ON | OFF}
	
	Enables or disables the restricted access option for a
	channel.  When restricted access is set, users not on
	the access list will instead be kicked and banned from the channel.

CHAN_HELP_SET_SECURE
	Sintàxi: SET canal SECURE {ON | OFF}
	
	Habilita o deshabilita les característiques de seguretat
	de %S per a un canal. Quan SECURE està
	establert, només usuaris que haguin registrat els seus nicknames
	amb %s i haguin utilitzat IDENTIFY amb la seva clau els serà
	atorgat accés al canal controlat per la llista de
	accés.

CHAN_HELP_SET_SECUREOPS
	Sintàxi: SET canal SECUREOPS {ON | OFF}
	
	Habilita o deshabilita l'opció seguretat de ops per a un
	canal.  Quan seguretat de ops està establerta, usuaris
	que no estaven a la llista d'usuaris no els serà permès 
	obtenir l' estatus d'operador de canal.
	
CHAN_HELP_SET_SECUREFOUNDER
	Sintàxi: SET canal SECUREFOUNDER {ON | OFF}
	
	Habilita o deshabilita l'opció seguretat de fundador per a
	un canal. Quan seguretat de fundador està establerta, només
	el fundador real podrà eliminar el canal, canviar la seva clau,
	el seu fundador i el seu sucesor, i no aquells que estiguin identificats
	amb %S.
	
CHAN_HELP_SET_SIGNKICK
	Sintàxi: SET canal SIGNKICK {ON | LEVEL | OFF}
	
	Habilita o deshabilita els kicks firmats per a un
	canal. Quan SIGNKICK està establert, kicks
	fets amb la comanda %S KICK tindran el
	nick que ha fet servir la comanda en la raó.
	
	Si uses LEVEL, aquells que tenen nivelll superior o
	igual al nivelll SIGNKICK en el canal no tindran els seus kicks 
	firmats. Veure %R%S HELP LEVELS per a més informació.

CHAN_HELP_SET_XOP
	Sintàxi: SET canal XOP {ON | OFF}
	
	Habilita o deshabilita el sistema de llistes xOP per a un canal.
	Quan XOP està establert, has de fer servir les comandes AOP/SOP/VOP
	per a donar privilegis de canal a usuaris. D'altra manera
	has de fer servir la comanda ACCESS.
	
	Nota Tècnica: Quan canvies de llista d'accés a sistema
	de llistes xOP, al teva definició de nivellls d'usuari serà
	canviada, així que no tindras els mateixos valors si tornes a
	canviar a llista de accés!
	
	També hauries de comprovar si els teus usuaris estan a la llista xOP
	corresponent després del canvi, ja que la deducció no és
	sempre perfecta... de fet, no es recomana el fer servir llistes
	xOP si vas canviar la definició de nivellls amb la comanda
	LEVELS.
	
	Canviar de llistes xOP al sistema de llistes d'accés no causa cap
	problema.

CHAN_HELP_SET_OPNOTICE
	Sintàxi: SET canal OPNOTICE {ON | OFF}
	
	Habilita o deshabilita l'opció de op-notice per a un canal.
	Quan op-notice està establert, %S enviarà una noticia
	a l'usuari del canal quan les comandes OP o DEOP siguin
	utilitzades per a un usuari en el canal.

CHAN_HELP_AOP
	Sintàxi: AOP canal ADD nick
	          AOP canal DEL {nick | instancia | llista}
	          AOP canal LIST [mascara | llista]
	          AOP canal CLEAR
	
	Manté la llista OP (AutoOP) per a un canal. La llista
	AOP li dóna a l'usuari el dret de ser automàticament
	opejat en el teu canal, desbanejat o convidar-se a ell mateix en cas de
	ser necesàri, mostrar el missatge de salutació al entrar, etc.
	
	La comanda AOP ADD agrega al nickname donat a la llista
	AOP.
	
	La comanda AOP DEL esborra el nickname donat de la
	llista AOP. Si una llista de instàncies és donada, aquestes
	instàncies són esborrades. (Veure l'exemple per a LIST més
	endevant.)
	
	La comanda AOP LIST mostra la llista AOP. Si una
	mascara amb comodí és donada, només les instàncies
	que concordin amb la mascara són mostrades. Si una llista de
	instàncies és donada, sols aquestes instàncies són mostrades;
	per exemple:
	
	   AOP #canal LIST 2-5,7-9
	      Mostra instàncies de AOP enumerades 2 a 5 i
	      7 a 9.
	
	La comanda AOP CLEAR neteja totes les instàncies de la
	llista AOP.
	
	Les comandes AOP ADD i AOP DEL està limitats a
	SOPs o superior, mentre que la comanda AOP CLEAR
	sols pot ser utilitzada per el fundador del canal. Tot i així,
	qualsevol usuari a la llista AOP pot usar la comanda
	AOP LIST.
	
	Aquesta comanda pot haVeure sigut deshabilitada per al teu canal.
	Si aquest és el cas, has de fer servir la llista d'accés. Veure
	%R%S HELP ACCESS per a informació sobre la llista
	de accés i %R%S HELP SET XOP per a saber com canviar
	entre la llista de accés i el sistema de llistes xOP.

CHAN_HELP_HOP
	Sintàxi: HOP canal ADD nick
	          HOP canal DEL {nick | instancia | llista}
	          HOP canal LIST [mascara | llista]
	          HOP canal CLEAR
	
	Manté la llista HOP (HalfOP) per a un canal. La llista
	HOP li dóna a l'usuari el dret de ser automàticament
	halfopejat en el teu canal.
	
	La comanda HOP ADD agrega al nickname donat a la llista
	HOP.
	
	La comanda HOP DEL esborra el nickname donat de la
	llista HOP. Si una llista d'instàncies és donada, aquestes
	instàncies són esborrades. (Veure l'exemple per a LIST més
	endevant.)
	
	La comanda HOP LIST mostra la llista HOP. Si una
	mascara amb comodí és donada, només les instàncies
	que concordin amb la mascara són mostrades. Si una llista de
	instàncies és donada, només aquestes instàncies són mostrades;
	per exemple:
	
	   HOP #canal LIST 2-5,7-9
	      Mostra instàncies de HOP enumerades 2 a 5 y
	      7 a 9.
	
	La comanda HOP CLEAR neteja totes les instàncies de la
	llista HOP.
	
	Les comandes HOP ADD i HOP DEL estan limitades a
	AOPs o superior, mentre que la comanda HOP CLEAR
	sols pot ser utilitzada per el fundador del canal.
	
	Aquesta comanda pot haber estat deshabilitada per al teu canal.
	Si aquest és el cas, has de fer servir la llista d'accés. Veure
	%R%S HELP ACCESS per a informació sobre la llista
	d'accés i %R%S HELP SET XOP per a saber com canviar
	entre la llista d'accés i el sistema de llistes xOP.

CHAN_HELP_SOP
	Sintàxi: SOP canal ADD nick
	          SOP canal DEL {nick | instancia | llista}
	          SOP canal LIST [mascara | llista]
	          SOP canal CLEAR
	
	Manté la llista SOP (SuperOP) per a un canal. La llista
	SOP li dóna a l'usuari els drets donats per la llista AOP,
	i agrega aquells necesaris per a utilitzar les llistes de
	AutoKick i BadWords, enviar i llegir memos de canal, etc.
	
	La comanda SOP ADD afegeix el nickname donat a la llista
	SOP.
	
	La comanda SOP DEL esborra el nickname donat de la
	llista SOP. Si una llista d'instàncies és dada, aquestes
	instàncies són esborrades. (Veure l'exemple per a LIST més
	endevant.)
	
	La comanda SOP LIST mostra la llista SOP. Si una
	mascara amb comodí és donada, només les instàncies
	coincidents amb la mascara són mostrades. Si una llista de
	instàncies és donada, sols aquestes instàncies són mostrades;
	per exemple:
	
	   SOP #canal LIST 2-5,7-9
	      Mostra instàncies de SOP enumerades 2 a 5 i
	      7 a 9.
	
	La comanda SOP CLEAR neteja totes les instàncies de la
	llista SOP.
	
	Les comandes SOP ADD, SOP DEL i SOP CLEAR estan
	limitades al fundador del canal. Qualsecol usuari de la
	llista AOP pot fer servir la comanda SOP LIST.
	
	Aquesta comanda pot haber sigut deshabilitada per al teu canal.
	Si aquest és el cas, has de fer servir la llista d'accés. Veure
	%R%S HELP ACCESS per a informació sobre la llista
	d'accés i %R%S HELP SET XOP per a saber com canviar
	entre la llista d'accés i el sistema de llistes xOP.

CHAN_HELP_VOP
	Sintàxi: VOP canal ADD nick
	          VOP canal DEL {nick | instancia | llista}
	          VOP canal LIST [mascara | llista]
	          VOP canal CLEAR
	
	Manté la llista VOP (VOicePeople) per a un canal.
	La llista VOP li dona a l'usuari el dret de ser
	automàticament donat veu o donar-se veu a si mateix.
	
	La comanda VOP ADD agrega el nickname donat a la llista
	VOP.
	
	La comanda VOP DEL esborra el nickname donat de la
	llista VOP. Si una llista de instàncies és donada, aquestes
	instàncies són esborrades. (Veure l'exemple per a LIST més
	endevant.)
	
	La comanda VOP LIST mostra la llista VOP. Si una
	mascara amb comodí és donada, sols les instàncies
	concordant amb la mascara són mostrades. Si una llista de
	instàncies és donada, sols aquetes instàncies són mostrades;
	per exemple:
	
	   VOP #canal LIST 2-5,7-9
	      mostra instàncies de VOP enumerades 2 a 5 i
	      7 a 9.
	
	La comanda VOP CLEAR neteja totes les instàncies de la
	llista VOP.
	
	Les comandes VOP ADD, VOP DEL i VOP LIST estan
	limitades a AOPs o superior, mentre que la comanda
	VOP CLEAR sols pot ser utilitzada per el fundador del
	canal.
	
	Aquesta comanda pot haber sigut deshabilitada per al teu canal.
	Si aquest és el cas, has de fer servir la llista de accés. Veure
	%R%S HELP ACCESS per a informació sobre la llista
	d'accés i %R%S HELP SET XOP per a saber como canviar
	entre la llista d'accés i el sistema de llistes xOP.

CHAN_HELP_ACCESS
	Sintàxi: ACCESS canal ADD nick nivell
	          ACCESS canal DEL {nick | indices | llista}
	          ACCESS canal LIST [mascara | llista]
	          ACCESS canal CLEAR
	
	Manté la llista d'accés per a un canal. La llista
	de accés específica quins usuaris estan permesos a tenir
	l'estatus d'operador de canal o accés a comandes de
	%S en el canal.  Diferents nivellls d'usuaris permeten
	accés a diferents subnivelles de privilegis;
	%R%S HELP ACCESS LEVELS per a informació més
	específica. Qualsevol nick no present a la llista de
	accés té un nivelll 0.
	
	La comanda ACCESS ADD afegeix el nickname donat a la llista
	d'accés amb el nivelll d'usuari donat; si el nick ja està
	present a la llista, el nivelll d'accés és canviat al
	especificat en la comanda. El nivelll especificat
	ha de ser menys que el de l'usuari que fa la comanda, i si
	el nick ja està a la llista d'accés, l'accés actual
	del nick ha de ser menor que l'accés de l'usuari que fa la
	comanda.
	
	La comanda ACCESS DEL esborra el nick donat de la llista
	d'accés. Si una llista d'instàncies és donada, aquestes instàncies
	són esborrades.  (Veure l'exemple per a LIST més endevant.)
	
	La comanda ACCESS LIST mostra la llista d'accés. Si
	una mascara amb comodí és donada, sols aquestes instàncies
	concordant amb la mascara són mostrades. Si una llista de
	instàncies és donada, sols aquestes instàncies són mostrades; per
	exemple:

	   ACCESS #canal LIST 2-5,7-9
	      llista instàncies d'accés numerades 2 fins 5 i
	      7 fins a 9.
	      
	La comanda ACCESS CLEAR neteja totes les instàncies
	de la llista d'accés.

CHAN_HELP_ACCESS_LEVELS
	nivellls d'accés per a usuaris
	
	Per defecte, els següents nivellls d'accés estan definits:
	
	   Fundador   accés complet a les funcions de %S;
	                     Op automàtico al entrar al canal. Notis
	                     que sols una persona pot tenir estatus
	                     de fundador (no pot ser donat fent servir la
	                     comanda ACCESS).
	        10   accés a la comanda AKICK; Op automàtico.
	         5   Op automàtico.
	         3   Voice automàtico.
	         0   Cap privilegi especial; pot ser opejat
	                     per altres ops (a menys que seguretat de
	                     ops estigui establerta).
	        <0   No pot ser opejat.
	
	Aquests nivellls poden ser canviats, o afegir-ne de nous,
	fent servir la comanda LEVELS; escriu %R%S HELP LEVELS
	per a més informació.

CHAN_HELP_AKICK
	Sintàxi: AKICK channel ADD {nick | mask} [reason]
	         AKICK channel STICK mask
	         AKICK channel UNSTICK mask
	         AKICK channel DEL {nick | mask | entry-num | list}
	         AKICK channel LIST [mask | entry-num | list]
	         AKICK channel VIEW [mask | entry-num | list]
	         AKICK canal ENFORCE
	         AKICK canal CLEAR
	
	Manté la llista d'AutoKicks per a un canal. Si
	un usuari de la llista de Autokicks intenta entrar
	al canal, %S banejarà l'usuari del canal, després el
	kickejarà.
	
	La comanda AKICK ADD afegeix el nick o mascara de
	usuari donat a la llista de Autokicks. Si una raó
	es donada amb la comanda, aquesta raó serà utilitzada quan 
	l'usuari sigui kickejat; si no, la raó per defecte és
	"Has sigut banejat del canal".
	When akicking a registered nick the nickserv account
	will be added to the akick list instead of the mask.
	All users within that nickgroup will then be akicked.
	
	La comanda AKICK STICK baneja permanent la mascara
	donada en el canal. Si algú intenta treure el ban,
	%S l'establirà automàticament. No pot ser
	usada per a nicks registrats.
	
	La comanda AKICK UNSTICK cancela l'efecto de la
	comanda AKICK STICK, així que serà possible esborrar
	el ban del canal.
	
	La comanda AKICK DEL esborra el nick o mascara donats
	de la llista de AutoKicks. Tot i així, no esborra cap ban
	posat per un AutoKick; aquells han de ser esborrats
	manualment.
	
	La comanda AKICK LIST mostra la llista de AutoKicks,
	o opcionalment sols les instàncies de AutoKick 
	que concordin amb la mascara especificada.
	
	La comanda AKICK VIEW és una versió més detallada
	de la comanda AKICK LIST.
	
	La comanda AKICK ENFORCE fa a %S a forçar la
	llista AKICK actual esborrant als usuaris que
	concordin amb la mascara AKICK.
	
	La comanda AKICK CLEAR neteja totes les instàncies
	de la llista de akicks.

CHAN_HELP_LEVELS
	Sintàxi: LEVELS canal SET tipo nivell
	        LEVELS canal {DIS | DISABLE} tipo
	        LEVELS canal LIST
	        LEVELS canal RESET
	
	La comanda LEVELS permet un control fin sobre el
	significat dels nivellls d'accés numèrics per a
	canals. Amb aquesta comanda, pots definir el nivelll d'accés
	requerit per a la majoria de les funcions de %S.
	(Les comandes SET FOUNDER i SET PASSWORD, així com
	aquesta comanda estan sempre restringidws al fundador del
	canal.)
	
	LEVELS SET permet canviar el nivelll d'accés per a una funció
	o un grup de funcions. LEVELS DISABLE (o DIS abreviat)
	deshabilita una característica automàtica o prohibeix accés a una
	funció per a totss excepte el fundador del canal.
	LEVELS LIST mostra els nivellls actuals per a cada funció o
	grup de funcions. LEVELS RESET restableix els nivellls als 
	nivellls per defecte d'un canal acabat de crear (Veure
	HELP ACCESS LEVELS).
	
	Per a una llista de les características i funcions a les que
	se'ls pot canviar els nivellls, Veure HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Els següents nombres de característiques/funcions són
	entesos. Cal tenir en compte que els nivellls per a AUTODEOP
	i NOJOIN són nivellls màxims, mentre tots els altres
	són nivellls mínims.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Sintàxi: INFO canal [ALL]
	
	llista informació sobre el canal registrat donat,
	incloent el seu fundador, temps de registració, últim
	cop utilitzat, descripció i fixació de Modes, si existeixen.
	Si ALL és especificat, el misatge d'entrada i
	sucsesor també seran mostrats.
	
	Per defecte, l'opció ALL està limitada a aquells
	amb accés de fundador en el canal.

CHAN_HELP_LIST
	Sintàxi: LIST patró
	
	llista tots els canals registrats concordant amb el patró 
	donat (canals amb l'opció PRIVATE establerta no són
	llistats.)

CHAN_HELP_OP
	Sintàxi: OP [#canal [nick]]
	
	Opeja al nick seleccionat en un canal. Si un nick no és
	donat, t'opejarà a tu. Si el canal i el nick no són donats,
	te opejarà en tots els canals en els que estiguis,
	sempre i quan tinguis els permisos per a fer-ho.
	
	Per defecte, limitat a AOPs o aquells amb nivelll d'
	accés 5 i major en el canal.

CHAN_HELP_DEOP
	Sintàxi: DEOP [#canal [nick]]
	
	Desopeja al nick seleccionado en un canal. Si el nick no és
	donat, et desopejarà a tu. Si el canal i el nick no són donats,
	et desopejarà a tu en tots els canals en els que estiguis,
	sempre i quan tinguis els permisos per a fer-ho.
	
	Per defecte, limitat a AOPs o aquells amb nivelll d'
	accés 5 i major en el canal.
	
CHAN_HELP_VOICE
	Sintàxi: VOICE [#canal [nick]]
	
	Dóna voice al nick seleccionat en un canal. Si el nick no
	és donat, et donarà voice a ti. Si el canal i el nick no són
	donats, et donarà voice en tots els canals en els que
	estiguis, sempre i quan tinguis els permisos per a
	fer-ho.
	
	Per defecte, limitat AOPs o aquells amb nivelll d'
	accés 5 i major en el canal, o a VOPs o aquells
	amb nivelll 3 i major per a donar-se voice a si mateixos.

CHAN_HELP_DEVOICE
	Sintàxi: DEVOICE [#canal [nick]]
	
	Treu voice al nick seleccionat en un canal. Si el nick no
	és donat, et treurà voice a tu. Si el canal i el nick no són
	donats, et treurà voice en tots els canals en els que
	estiguis, sempre i quan tinguis els permisos per a fer-ho.
	
	Per defecte, limitat a AOPs o aquells amb nivelll d'
	accés 5 i major en el canal, o a VOPs o aquells amb
	nivelll 3 i major per a donar-se voice a si mateixos.
	
CHAN_HELP_HALFOP
	Sintàxi: HALFOP [#canal [nick]]
	
	Dóna Halfop al nick seleccionat en un canal. Si el nick no
	és donat, et donarà halfop a tu. Si el canal i el nick no són
	donats, et donarà halfop en tots els canals en els que
	estiguis, sempre i quan tinguis els permisos per a fer-ho.
	
	Per defecte, limitat a AOPs i aquells amb nivelll d'
	accés 5 i major en el canal, o a HOPs o aquells amb nivelll
	4 i major per a donar-se halfop a si mateixos.

CHAN_HELP_DEHALFOP
	Sintàxi: DEHALFOP [#canal [nick]]
	
	Treu Halfop al nick seleccionat en un canal. Si el nick no
	és donat, et treurà halfop a tu. Si el canal i el nick no són
	donats, et treurà halfop en tots els canals en els que
	estiguis, sempre i quan tinguis els permisos per a fer-ho.
	
	Per defecte, limitat a AOPs i aquells amb nivelll d'
	accés 5 i major en el canal, o a HOPs o aquells amb
	nivelll 4 i major per a treure's halfop a si mateixos.

CHAN_HELP_PROTECT
	Sintàxi: PROTECT [#canal [nick]]
	
	Protejeix al nick seleccionat en un canal. Si el nick no és
	donat, et protegirà a tu. Si el canal i el nick no són donats,
	et protejirà en tots els canals en els que
	estiguis, sempre i quan tinguis els permisos per a fer-ho..
	
	Per defecte, limitat al fundador, o a SOPs o aquells
	amb nivell 10 i major en el canal per a protejerse a si
	mismos.

CHAN_HELP_DEPROTECT
	Sintàxi: DEPROTECT [#canal [nick]]
	
	Desprotejeix al nick seleccionat en un canal. Si el nick no
	és donat, et desprotegirà a tu. Si el canal i el nick no són
	donats, et desprotegirà en tots els canals en els que
	estiguis, sempre i quan tinguis els permisos per a fer-ho..
	
	Per defecte, limitat al fundador, o a SOPs o aquells
	amb nivelll 10 i major en el canal per a desprotegir-se a
	si mateixos.

CHAN_HELP_OWNER
	Sintàxi: OWNER [#canal]
	
	Et dóna l'estatus de propietari del canal. Si el canal no
	és donat, et donarà estatus de propietari en tots els canals en els que
	estiguis, sempre i quan tinguis els permisos per a fer-ho.
	
	Limitad a aquells amb nivelll de fundador en el canal.

CHAN_HELP_DEOWNER
	Sintàxi: DEOWNER [#canal]
	
	Esborra l'estatus de propietari del canal. Si el canal no
	és donat, esborra l'estatus de propietari en tots els canals en els que
	estiguis, sempre i quan tinguis els permisos per a fer-ho.
	
	Limitat a aquells amb nivelll de fundador en el canal.

CHAN_HELP_INVITE
	Sintàxi: INVITE canal
	
	Li diu a %S que et convidi al canal donat.
	
	Per defecte, limitat a AOPs o aquells amb nivell 5
	i major en el canal.

CHAN_HELP_UNBAN
	Sintàxi: UNBAN canal
	
	Li diu a %S que esborri tots els bans que no permeten que 
	entris al canal donat.
	
	Per defecte, limitat a AOPs o aquells amb nivell 5
	i major en el canal.

CHAN_HELP_KICK
	Sintàxi: KICK [#canal [nick [raó]]]
	
	Kickeja al nick seleccionat en el canal. Si el nick no és
	donat, et kickejara a tu. Si el canal i el nick no són donats,
	et kickejarà en tots els canals en els que
	estiguis, sempre i quan tinguis els permisos per a fer-ho.
	
	Per defecte, limitat a AOPs o aquells amb nivell d'
	accés 5 i major en el canal.

CHAN_HELP_BAN
	Sintàxi: BAN [#canal [nick [raó]]]
	
	Baneja un nick seleccionat en un canal. Si el nick no
	és donat, et banejarà a ti. Si el canal i el nick no són donats,
	et banejara en tots els canals en els que
	estiguis, sempre i quan tinguis els permisos per a fer-ho.
	
	Per defecte, limitat a AOPs o aquells amb nivell d'
	accés 5 i major en el canal.

CHAN_HELP_TOPIC
	Sintàxi: TOPIC canal [topico]
	
	Fa a %S establir el topic del canal al especificat.
	Si el topic no és donat, un topic buit és establert.
	Aquesta comanda és més util utilitzada en conjunt a SET
	TOPICLOCK. Veure %R%S HELP SET TOPICLOCK
	per a més informació.
	
	Per defecte, limitat a aquells amb nivell de fundador
	en el canal.

CHAN_HELP_CLEAR
	Sintàxi: CLEAR canal que
	
	Li diu a %S que netegi certs paràmetres en el canal.
	que pot ser un dels següents:
	
	     MODES    Reestableix totss los Modes en el canal (o sigui,
	                  neteja els Modes i,k,l,m,n,p,s,t).
	     BANS     neteja tots els bans en el canal.
	     EXCEPTS  neteja tots els excepts en el canal.
	     INVITES  Clears all invites on the channel.
	     OPS      Esborra estatus d'operador de canal(Mode +o)
	                  de tots els operadors de canal.
	     HOPS     Removes channel-halfoperator status (mode +h) from
	                  all channel halfoperators, if supported.
	     VOICES   Esborra l'estatus "voice" (Mode +v) de qui
	                  tingui aquest Mode establert.
	     USERS    Explusa (kickea) tots els usuaris del canal.
	
	Per defecte, el nivell d'accés en el canal ha de ser de
	fundador.

CHAN_HELP_GETKEY
	Sintàxi: GETKEY canal
	
	Retorna la clau del canal donat. Aquesta és una comanda
	principalment per a ser utilitzada per bots i/o scripts,
	així que el resultat és de la següent manera:
	
	KEY <canal> <key>
	
	key és "NO KEY" si no hi ha key establerta.
	
CHAN_HELP_SENDPASS
	Sintàxi: SENDPASS canal
	
	Envia la clau del canal donat a l'adresa email establerta
	en l'arxiu del nickname del fundador. Aquesta comanda
	és sumament útil per a recuperar claus perdudes.
	
	pot ser limitat a  Operadors de IRC en certas xarxes.
	
	Aquesta comanda no és disponible quan l'encriptació està habilitada.

CHAN_SERVADMIN_HELP
	
	Les següents comandes estan disponibles per a administradors
	de serveis:
	
	    STATUS     Retorna el nivell d'accés actual d'un
	                   usuari en un canal
	    CLIST      llista tots els canals en els que l'usuari
	               sigui fundador
	
	Administradors de serveis també poden eliminar un canal
	sense haver d'identificar-se per mitjà d'una clau, i poden
	Veure l'accés, AKICK, i paràmetres de les llistes de nivells per a
	qualsevol canal.

CHAN_SERVADMIN_HELP_LOGOUT
	Sintàxi: LOGOUT canal [nickname]
	
	Aquesta comanda ha fa que el nick seleccionat no sigui identificat
	pel canal donat.
	
	Si tu ets el fundador del canal, pots desconectar a
	qui sigui, sinó només et pots desconectar a
	tu mateix.
	
	Si ets un administrador de serveis, pots
	desconectar a qui sigui d'un canal sense ser el fundador
	del canal. A més, pots omitir el paràmetro nickname;
	això desconectara a tots els usuaris identificados
	del canal.

CHAN_SERVADMIN_HELP_DROP
	Sintàxi: DROP canal
	
	Desregistra el canal donat.  Només
	administradors de serveis poden eliminar un
	canal per el qual no s'haguin identificado.

CHAN_SERVADMIN_HELP_SET
	
	Administradors de serveis poden tambié establir la
	Opció NOEXPIRE, amb la qual s'evita que els canals
	expirin. A més, administradors de
	serveis poden establir opcions per a qualsevol canal
	sense haver d'identificarse per clau en el canal.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Sintàxi: SET canal NOEXPIRE {ON | OFF}
	
	Estableix si el canal expirarà. Establint-lo 
	a ON evita que el canal  expiri.
	
	limitat a administradors de serveis.

CHAN_SERVADMIN_HELP_INFO
	
	Administradors de serveis poden utilitzar el paràmetre
	ALL amb qualsevol canal

CHAN_SERVADMIN_HELP_LIST
	Sintàxi: LIST patró [FORBIDDEN] [NOEXPIRE]
	
	llista tots els canals registrats concordant amb el patró donat.
	canals amb l'opció PRIVATE establreta sols seran mostrats
	a administradors de serveis. canals amb l'opció NOEXPIRE
	tindran un ! davant del nom del canal per a
	administradors de serveis.
	
	Si les opcions FORBIDDEN o NOEXPIRE son donades, sols canals
	que, respectivament, tenen FORBID o NOEXPIRE establert seran
	mostrats. Si ambdues opcions són donades, ambdós tipus de canals
	seran mostrats. Aquestes opcions estan limitadas a
	administradors de serveis.

CHAN_SERVADMIN_HELP_GETPASS
	Sintàxi: GETPASS canal
	
	Retorna la clau pel canal donat. tenir en compte
	que quan aquesta comanda es fa servir, un missatge incloent la
	persona que va executar la comanda i el canal en el que es va fer servir
	seran seguits i enviats com a WALLOPS/GLOBOPS.
	
	limitat a administradors de serveis.

CHAN_SERVADMIN_HELP_FORBID
	Sintàxi: FORBID canal [raó]
	
	Prohibeix a qualsevol de registrar o usar el canal
	donat. pot ser cancelat eliminant el canal.
	
	Raó pot ser requerida en certas xarxes.
	
	limitat a administradors de serveis.

CHAN_SERVADMIN_HELP_SUSPEND
	Syntax: SUSPEND channel [reason]
	
	Disallows anyone from registering or using the given
	channel.  May be cancelled by using the UNSUSPEND
	command to preserve all previous channel data/settings.
	
	Reason may be required on certain networks.
	
	Limited to Services admins.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Syntax: UNSUSPEND channel
	
	Releases a suspended channel. All data and settings
	are preserved from before the suspension.
	
	Limited to Services admins.

CHAN_SERVADMIN_HELP_STATUS
	Sintàxi: STATUS canal nickname
	
	Retorna el nivell d'accés actual del nick donat
	en el canal donat. La resposta és de la forma:
	
	    STATUS canal nickname nivell-de-accés
	
	Si un error es dóna, la resposta serà de la forma:
	
	    STATUS ERROR missatge-de-error
	
	limitat a administradors de serveis.

###########################################################################
#
# MemoServ help messages
#
###########################################################################
MEMO_HELP_CMD_SEND
	    SEND   Envia un memo a un nick o canal
MEMO_HELP_CMD_CANCEL
	    CANCEL Cancela l'últim memo enviat
MEMO_HELP_CMD_LIST
	    LIST   llista els teus memos
MEMO_HELP_CMD_READ
	    READ   Llegeix un o més memos
MEMO_HELP_CMD_DEL
	    DEL    Esborra un o més memos
MEMO_HELP_CMD_SET
	    SET    Estableix opcions relacionades a memos
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
	%S és una utilitat que permet a usuaris de IRC
	enviar missatges curts a altres usuaris de IRC, estiguin
	en linea o no, o a canals(*). Ambdós el nickname que
	envia i el nickname o canal destinataris han d'estar
	registrats per a enviar un memo.
	
	Les comandes de %S incloen:
	
MEMO_HELP_ADMIN
	not used.
MEMO_HELP_OPER
	not used.
MEMO_HELP_FOOTER
	
	escriu %R%S HELP comando per a més ajuda en
	qualsevol de les comandes llistades anteriorment.
	
	(*) Per defecte, qualsevol usuari amb al menys nivell
	    d'accés 10 en un canal pot lleguir els memos del
	    canal. Això pot ser canviat amb la comanda
	    LEVELS de %s.

MEMO_HELP_SEND
	Sintàxi: SEND {nick | canal} text-del-memo
	
	Envia un memo al nick o canal donat, que conté
	text-del-memo. A l'enviar a un nickname, aquest
	rebrà noticia de que ell/ella té un nou memo. El nick
	o canal receptors han d'estar registrats.

MEMO_HELP_CANCEL
	Sintàxi: CANCEL {nick | canal}
	
	Cancela l'ultim memo enviat al nick o canal donat,
	sempre i quan no hagi sigut llegit al moment en el que
	vas fer servir la comanda.

MEMO_HELP_LIST
	Sintàxi: LIST [canal] [llista | NEW]
	
	llista els memos que tens actualment. Amb NEW, llista
	només els memos nous (no llegits). Memos nous estan
	marcats amb un "*" a l'esquerra del nobre del memo.
	També pots especificar una llista de nombres, com en el
	exemple més endevant:
	
	   LIST 2-5,7-9
	      llista memos enumerats 2 a 5 i 7 a 9.

MEMO_HELP_READ
	Sintàxi: READ [canal] {nombre | llista | LAST | NEW}
	
	T'envia el text dels memos especificats. Si LAST és
	donat, t'envia l'ultim memo rebut. si NEW és donat, t'
	envia tots els teus memos nous. D'altra forma, t'envia el
	memo nombre nombre.  També pots donar una llista de
	nombres, com en el exemple:
	
	   READ 2-5,7-9
	      mostra els memos enumerats 2 a 5 i 7 a 9.

MEMO_HELP_DEL
	Sintàxi: DEL [canal] {nombre | llista | LAST | ALL}
	
	Esorra el o els memos especificats. pots donar multiples
	nombres o rangs de nombres en lloc d'un solo, com en el
	exemple.
	
	If LAST is given, the last memo will be deleted.
	Si ALL és donat, esborra tots els teus memos.
	
	exemples:
	
	   DEL 1
	      Esorra el teu primer memo.
	
	   DEL 2-5,7-9
	      Esborra els memos enumerats 2 a 5 i 7 a 9.

MEMO_HELP_SET
	Sintàxi: SET Opció paràmetros
	
	Estableix varies opcions de memos. Opció pot ser una de:
	
	    NOTIFY      Canvia quan seràs notificat sobre de
	                    nous memos (sols per a nicknames)
	    LIMIT       Estableix el màxim nombre de memos que
	                    pots rebre
	
	escriu %R%S HELP SET Opció per a més informació
	sobre una opció específica.

MEMO_HELP_SET_NOTIFY
	Sintàxi: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
	
	Canvia quan se't notificarà de nous memos:
	
	    ON      Notificació de memos quan et conectis,
	               quan treguis  /AWAY, i quan 
	                s'enviin
	    LOGON   Sols seras notificat de memos quan et conectis
	               o quan treguis /AWAY.
	    NEW     Sols seras notificat de memos quan se't 
	               enviin.
	    MAIL    You will be notified of memos by email aswell as
	               any other settings you have.
	    NOMAIL  You will not be notified of memos by email.
	    OFF     No rebràs cap notificació per memos.
	
	ON és escencialment LOGON i NEW combinats.

MEMO_HELP_SET_LIMIT
	Sintàxi: SET LIMIT [canal] límit
	
	Estableix el màxim nombre de memos que tu (o el canal donat)
	pots  tenir. Si queda a 0, ningú podrà
	enviar-te memos. A més, no pots establir el límit més
	alt que %d.

MEMO_HELP_INFO
	Sintàxi: INFO [canal]
	
	mostra informació del nombre de memos que tens,
	quants d'ells encar no han sigut llegits, i quants memos
	pots rebre. Amb un paràmetre, mostra la mateixa
	informació pel canal donat.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Sintàxi: SET LIMIT [usuari | canal] {límit | NONE} [HARD]
	
	Estableix el màxim nombre de memos un usuari o canal està
	permès. Establir el límit a 0 evita a l'usuari
	de rebre memos; posar-lo a NONE permet a l'usuari
	de rebre i guardar els memos que vulgui. Si no dones un
	nickname o canal, el teu propi límit està establert.
	
	Afegir HARD evita al usuari de canviar el límit. No
	afegir HARD té l'efect contrari, permetent a l'
	usuari canviar el límit (encara que un límit previ hagui sigut establert
	amb HARD).
	
	L'us de la comanda SET LIMIT està limitat a
	administradors de serveis. altres usuaris poden
	només establir límits per a ells mateixos o per a un canal en
	el que tinguin certos privilegos, no podenn esborrar el seu
	límit, no poden establir el límit més enllà de %d, i no
	poden establir un límit hard.

MEMO_SERVADMIN_HELP_INFO
	Sintàxi: INFO [nick | canal]
	
	Sense un paràmetro, mostra informació del nombre
	de memos que tens, quants d' ells encara no  s'han
	llegit, i el nombre total de memos que pots rebre.
	
	amb un canal de paràmetre, mostra la mateixa informació
	pel canal donat.
	
	amb un nickname de paràmetr, mostra la misma
	informació pel nickname donat. Aquest us està
	limitat a administradors de serveis.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_STAFF
	Syntax: STAFF memo-text

	Sends all services staff a memo containing memo-text.
	Note: If you have opers on both the oper list and the
	admin list they will recive the memo twice.  The same
	applies for oper's on the Root list as well as other
	lists.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_SENDALL
	Syntax: SENDALL memo-text

	Sends all registered users a memo containing memo-text.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_RSEND
	Syntax: RSEND {nick | channel} memo-text
	
	Sends the named nick or channel a memo containing
	memo-text.  When sending to a nickname, the recipient will
	receive a notice that he/she has a new memo.  The target
	nickname/channel must be registered.
	Once the memo is read by its recepient, an automatic notification
	memo will be sent to the sender informing him/her that the memo
	has been read.

###########################################################################
#
# OperServ help messages
#
###########################################################################

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
	    GLOBAL      Envia un missatge a totss els usuaris
OPER_HELP_CMD_STATS
	    STATS       mostra estatus dels serveis i la xarxa
OPER_HELP_CMD_OPER
	    OPER        Modifica la llista d'operadors de serveis
OPER_HELP_CMD_ADMIN
	    ADMIN       Modifica la llista d'administradors de
	                serveis
OPER_HELP_CMD_STAFF
	    STAFF       Display Services staff and online status
OPER_HELP_CMD_MODE
	    MODE        Canvia els Modes d'un canal
OPER_HELP_CMD_KICK
	    KICK        Kickeja un usuari d'un canal
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  neteja els Modes d'un canal
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  Mata tots els usuaris en un host
OPER_HELP_CMD_AKILL
	    AKILL       Manipula la llista AKILL
OPER_HELP_CMD_SGLINE
	    SGLINE      Manipula la llista SGLINE
OPER_HELP_CMD_SQLINE
	    SQLINE      Manipula la llista SQLINE
OPER_HELP_CMD_SZLINE
	    SZLINE      Manipula la llista SZLINE
OPER_HELP_CMD_CHANLIST
	    CHANLIST    Llista tots els records de canal
OPER_HELP_CMD_USERLIST
	    USERLIST    Llista tots els records d'usuaris
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   Defineix missatges per a ser mostrats a usuaris
	                al conectarse
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  Defineix missatges per a ser mostrats a usuaris
	                al conectarse
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    Defineix missatges per a ser mostrats a usuaris
	                al transformar-se en operadors
OPER_HELP_CMD_SESSION
	    SESSION     Mostra la llista de sessions de host
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   Modifica la llista de límit de sessió
OPER_HELP_CMD_NOOP
	    NOOP        Temporalment elimina totes les linees O:
	                d'un servidor remotamente
OPER_HELP_CMD_JUPE
	    JUPE        "Jupiter" (Mart,Saturn....) un servidor
OPER_HELP_CMD_IGNORE
	    IGNORE      Modify the Services ignore list
OPER_HELP_CMD_SET
	    SET         Estableix varies opcions globals dels serveis
OPER_HELP_CMD_RELOAD
	    RELOAD      Recarrega la configuració dels serveis
OPER_HELP_CMD_UPDATE
	    UPDATE      Força a la base de dades dels serveis a
	                ser actualitzada a disc inmediatament
OPER_HELP_CMD_RESTART
	    RESTART     Guarda la base de dades dels serveis i
	                reinicia els serveis
OPER_HELP_CMD_QUIT
	    QUIT        Termina el programa de serveis sense guardar
	                els canvis
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    Termina el programa de serveis guardant els
	                canvios
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
	Noticia: Totes les comandes enviades a %S son monitorejades!

OPER_HELP_GLOBAL
	Sintàxi: GLOBAL missatge
	
	Permet als Administrators a enviar missatges als usuaris de
	la xarxa. El missatge sera enviat desde el nick %s.

OPER_HELP_STATS
	Sintàxi: STATS [AKILL | ALL | RESET]
	
	Sense una opció, mostra el nom actual d'usuaris i
	IRCops en linea (excluient els serveis), el màxim
	nombre d'usuaris desde que els serveis s'han iniciat, i
	el temps que els serveis han estat funcionant.
	
	Amb L'opció AKILL, mostra la mida actual de la
	llista AKILL i l'actual temps d'expiració per defecte.
	
	L'opció ALL esta sols disponible per administradors de
	serveis, i mostra informació en el us de memòria dels
	serveis. Utilitzar aquesta opció pot congelar els
	serveis per un curt periode de temps en xarxes grans,
	aixi que no es recomana utilitzar sovint.
	
	L'opció RESET estableix el nombre màxim d'usuaris
	al nombre d'usuaris actualment presents en la xarxa.
	
	UPTIME pot ser utilitzat com a sinònim per STATS.

OPER_HELP_OPER
	Sintàxi: OPER ADD nick
	          OPER DEL {nick | nom | llista}
	          OPER LIST [mascara | llista]
	          OPER CLEAR
	
	Permet als root de serveis afegir o
	eliminar nicknames a la llista d'operadors de
	serveis. Un usuari amb el seu nickname a la llista
	d'operadors de serveis i que ha estat identificat
	amb %s podra accedir a comandes d'operador de
	serveis.
	
	La comanda OPER ADD afegeix el nickname donat a la
	llista d'operadors de serveis.
	
	La comanda OPER DEL elimina el nickname donat de
	la llista d'operadors de serveis. Si una llista de
	instàncies es especificada, aquestes instàncies son esborrades.
	(Veure l'exemple per a LIST especificat més endavant.)
	
	La comanda OPER LIST mostra la llista
	d'operadors de serveis. Si s'especifica una mascara, nomès
	les instàncies que concordin amb la mascara són
	mostrades. Si s'especifica una llista de noms es , nomès aquestes
	instàncies son mostrades; per exemple:
	
	   OPER LIST 2-5,7-9
	      Llista instàncies d'operadors de serveis
	      enumerades de 2 a 5 i de 7 a 9.
	
	La comanda OPER CLEAR neteja totes les instàncies
	de la llista d'operadors de serveis.
	
	Qualsevol operador d'IRC pot utilitzar la forma
	OPER LIST de la comanda.

OPER_HELP_ADMIN
	Sintàxi: ADMIN ADD nick
	          ADMIN DEL {nick | nom | llista}
	          ADMIN LIST [mascara | llista]
	          ADMIN CLEAR
	
	Permet al root de serveis afegir o eliminar nicknames
	a la llista d'administradors de serveis. Un usuari
	el nickname del qual estigui a la llista d'administradors de
	serveis i que ha estat identificat amb %s podra
	accedir a comandes d'administradors de serveis.
	
	La comanda ADMIN ADD afegeix el nickname especificat a la llista
	d'administradors de serveis.
	
	La comanda ADMIN DEL elimina el nickname especificat de la
	llista d'administradors de serveis. Si una llista
	d'instàncies es especificada, aquestes instàncies son esborrades.
	(Veure l'exemple per LIST que s'especifica més endavant.)
	
	La comanda ADMIN LIST mostra la llista
	d'administradors de serveis. Si s'especifica una mascara, nomès
	les instàncies que concordin amb la mascara són mostrades;
	per exemple:
	
	   ADMIN LIST 2-5,7-9
	      Llista instàncies de la llista d'administradors de
	      serveis enumerades de 2 a 5 i de 7 a 9.
	
	La comanda ADMIN CLEAR neteja totes les instàncies de
	la llista d'administradors de serveis.
	
	Qualsevol operador d'IRC pot utilitzar la forma ADMIN LIST
	de la comanda. Tots les altres utilitats estan limitades al
	root de serveis.

OPER_HELP_IGNORE
	Syntax: IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick | mask]
	
	Allows Services Admins to make Services ignore a nick or mask
	for a certain time or until the next restart. The default
	time format is seconds. You can specify it by using units.
	Valid units are: s for seconds, m for minutes, 
	h for hours and d for days. 
	Combinations of these units are not permitted. To make
	Services permanently ignore the user, type 0 as time.
	When adding a mask, it should be in the format user@host
	or nick!user@host, everything else will be considered a nick.
	Wildcards are permitted.
	
	Ignores will not be enforced on IRC Operators.

OPER_HELP_MODE
	Sintàxi: MODE canal Modes
	
	Permet als operadors de serveis establir modes de canal
	per a qualsevol canal. Els parametres son els mateixos que per la
	comando estandart /MODE.
	
	Limitat a operadors de serveis.

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
	Sintàxi: CLEARMODES canal [ALL]
	
	Neteja tots els modes binarios (i,k,l,m,n,p,s,t) i
	bans del canal. Si s'especifica ALL, tambe neteja
	tots els ops i voices (modes +o i +v) del canal.
	
	Limitat a operadors de serveis.

OPER_HELP_KICK
	Sintàxi: KICK canal usuari raó
	
	Permet als IRCops kickejar un usuari de qualsevol
	canal. Els parametres son els mateixos que per la comanda
	/KICK. El missatge de kick tindra el nickname del IRCop
	que envia la comanda KICK; per exemple:
	
	*** SpamMan ha estat kickejat del canal #aujac per %S (cistus (Flood))
	
	Limitat a operadors de serveis.

OPER_HELP_SVSNICK
	Syntax: SVSNICK nick newnick
	
	Forcefully changes a user's nickname from nick to newnick.
	Limited to Super Admins.

OPER_HELP_AKILL
	Sintàxi: AKILL ADD [+expiració] mascara raó
	          AKILL DEL {mascara | nom | llista}
	          AKILL LIST [mascara | llista]
	          AKILL VIEW [mascara | llista]
	          AKILL CLEAR
	
	Permet als operadors de serveis manipular la llista de
	AKILLS. Si un usuari coincideix amb una mascara en la
	llista de AKILLS i s'esta intentant connectar, els serveis enviaran
	un KILL a aquest usuari i, en tipus de servidors soportats,
	notificara a tots els servidors que afegeixin un ban (linea K)
	per la mascara de l'usuari.
	
	AKILL ADD afegeix la mascara usuari@host especificada a la llista
	de AKILLS per la raó especificada (que HA DE SER especificada).
	expiració es especificada com un sencer seguit de
	 d (dies), h (hores), o m (minuts). Combinacions (com
	1h30m) no estan permesses. Si un especificador d'unitats no
	es inclòs, per defecte seran dies (o sigui +30 nomès
	significa 30 dies).  Per afegir un AKILL que no caduqui, utilitza +0.
	Si la mascara a afegir comença amb un +, s'ha d'especificar
	un temps d'expiració. El temps d'expiració per defecte actual de AKILLS
	pot ser consultat amb la comanda STATS AKILL.
	
	La comanda AKILL DEL esborra la mascara especificada de la llista
	d'AKILLS si esta present. Si s'especifica una llista de noms,
	aquestes instàncies son esborrades.  (Veure l'exemple per LIST 
	especificat més endavant.)
	
	La comanda AKILL LIST mostra la llista d'AKILLS.  
	Si s'especifica una mascara, nomès les instàncies coincidents
	amb la mascara són mostrades. Si s'especifica una llista de noms,
	nomès aquestes instàncies son mostrades; per exemple:
	
	   AKILL LIST 2-5,7-9
	      Llista les instàncies de AKILL enumerades de 2 a 5 i de 7 
	      a 9.
	      
	AKILL VIEW es una versió més verbal d'AKILL LIST, i 
	mostrara qui va afegir un AKILL, la data en que va ser afegit,
	i quan caducara, així com tambe la mascara usuari@host
	i la raó.
	
	AKILL CLEAR neteja totes les instàncies de la llista d'AKILLS.
	
	Limitat a operadors de serveis.
	
OPER_HELP_SGLINE
	Sintàxi: SGLINE ADD [+expiració] mascara:raó
	         SGLINE DEL {mascara | nom | llista}
	         SGLINE LIST [mascara | llista]
	         SGLINE VIEW [mascara | llista]
	         SGLINE CLEAR
	
	Permet als operadors de serveis manipular la llista
	de SGLINES. Si un usuari amb un nom real concordant
	amb una mascara SGLINE s'intenta connectar, els serveis
	l'impediran aconseguir una sessió d'IRC.
	
	SGLINE ADD afegeix la mascara de nom real especificada a la
	llista de SGLINES per la raó especificada (obligatori). 
	expiració es especificada com un sencer
	seguit de d (dies), h (hores), o m
	(minuts).  Combinacions (com 1h30m) no estan
	permesses. Si l'especificació de la unitat no es
	especificada, per defecte són dies (és a dir +30 nomès
	significa 30 dies). Per afegir una SGLINE que no caduqui,
	utilitza +0. Si la mascara de nom real especificada comença amb
	+, el temp d'expiració ha de ser especificat, encara que sigui
	el mateix que el per defecte. El temps actual d'expiració
	per defecte per SGLINES pot ser consultat amb la comanda
	STATS AKILL.
	Nota: Ja que la mascara de nom real pot contenir
	espais, el separador entre aquest i la raó es un signe de
	dos punts.
	
	La comanda SGLINE DEL esborra la mascara especificada de la
	llista de SGLINES si esta present. Si s'especifica una llista de
	instàncies, aquestes instàncies son esborrades. (Veure 
	l'exemple per LIST especificat més endavant.)
	
	La comanda SGLINE LIST mostra la llista de SGLINES. Si
	s'especifica una mascara, nomès les instàncies concordant amb
	la mascara són mostrades. Si s'especifica una llista de instàncies,
	nomès aquestes instàncies son mostrades; per exemple:
	
	   SGLINE LIST 2-5,7-9
	      Llista instàncies de SGLINES enumerades de 2 a 5 i de 7 
	      a 9.
	      
	SGLINE VIEW es una versió més verbal que SGLINE LIST,
	i mostrara qui va fegir la SGLINE, la data en la que va
	ser afegida, i quan caducara, a més de la mascara de nom real
	i la raó.
	
	SGLINE CLEAR neteja totes les instàncies de la llista de
	SGLINES.
	
	Limitat a operadors de serveis.

OPER_HELP_SQLINE
	Sintàxi: SQLINE ADD [+expiració] mascara raó
	         SQLINE DEL {mascara | nom | llista}
	         SQLINE LIST [mascara | llista]
	         SQLINE VIEW [mascara | llista]
	         SQLINE CLEAR
	
	Permet als operadors de serveis manipular la llista
	de SQLINES. Si un usuari amb un nick concordant amb una
	mascara SQLINE s'intenta connectar, els serveis li
	impediran aconseguir una sessió d'IRC.
	
	Si el primer caràcter de la mascara es #, els serveis
	evitaran l'ús dels canals coincidents (a IRCds
	que ho soportin).
	
	SQLINE ADD afegeix la mascara de nom real especificada a la
	llista de SQLINES per la raó especificada (que s'ha d'especficar). 
	expiració es especificada como un sencer
	seguit de d (dies), h (hores), o m
	(minuts).  Combinacions (com 1h30m) no estan
	permesses. Si l'especificació de la unitat no es
	especificada, per defecte són dies (és a dir +30 nomès
	significa 30 dies). Per afegir una SQLINE que no caduqui,
	utilitzar +0. Si la mascara de nom real especificada comença amb
	+, el temps d'expiració ha de ser especificat, encara que sigui
	el mateix que el per defecte. El temps actual d'expiració
	per defecte per SQLINES pot ser consultat amb la comanda
	STATS AKILL.
	
	La comanda SQLINE DEL esborra la mascara especificada de la
	llista de SQLINES si esta present. Si s'especifica una llista de
	instàncies, aquestes instàncies son esborrades. (Veure
	l'exemple per LIST especificat més endavant.)
	
	La comanda SQLINE LIST mostra la llista de SQLINES. Si
	s'especifica una mascara, nomès les instàncies coincidents amb
	la mascara son mostrades. Si s'especifica una llista de instàncies,
	sols esas instàncies son mostrades; per exemple:
	
	   SQLINE LIST 2-5,7-9
	      llista instàncies de SQLINES enumerades de 2 a 5 i de 7
	      a 9.
	
	SQLINE VIEW es una versió més verbal que SQLINE LIST,
	i mostrara qui va afegir la SQLINE, la data en la que va
	ser afegida, i quan caducara, a més de la mascara de nom real
	i la raó.
	
	SQLINE CLEAR neteja totes les instàncies de la llista de
	SQLINES.
	
	Limitat a operadors de serveis.

OPER_HELP_SZLINE
	Sintàxi: SZLINE ADD [+expiració] mascara raó
	          SZLINE DEL {mascara | nom | llista}
	          SZLINE LIST [mascara | llista]
	          SZLINE VIEW [mascara | llista]
	          SZLINE CLEAR
	
	Permet als operadors de serveis manipular la
	llista de SZLINES. Si un usuari amb una IP concordant
	amb una mascara de la llista de SZLINES intenta connectar-se,
	els serveis le impediran obtenir una sessió d'IRC
	(és a dir, si la IP te un PTR RR o no).
	
	SZLINE ADD afegeix la mascara IP especificada a la llista de
	SZLINES per la raó especificada (que s'ha d'especificar).
	expiració es especificada com un sencer seguit
	de d (dias), h (hores), o m (minuts).
	Combinacions (com 1h30m) no estan permesses. Si
	una especificació d'unitat no es especificada, per
	defecte es dies (és a dir, +30 significa
	30 dies). Per afegir una SZLINE que no caduqui, utilitzar
	+0. Si la mascara a afegir comença amb un
	+, el temps d'expiració ha de ser especificat, encara
	que sigui el mateix que el per defecte. El temps actual
	d'expiració per defecte per SZLINES pot ser
	consultat amb la comanda STATS AKILL.
	
	La comanda SZLINE DEL esborra la mascara especificada de la
	llista de SZLINES si esta present. Si s'especifica una llista de
	instàncies, aquestes instàncies son esborrades. (Veure
	l'exemple per LIST especificat més endavant.)
	
	La comanda SZLINE LIST mostra la llista de SZLINES.  
	Si s'especifica una mascara, nomès aquellas instàncies
	coincidents amb la mascara són mostrades. Si s'especifica una llista
	de instàncies, nomès aquestes instàncies són mostrades;
	per exemple:
	
	   SZLINE LIST 2-5,7-9
	      Llista instàncies SZLINE enumerades de 2 a 5 i de 7 
	      a 9.
	      
	SZLINE VIEW es una versió més verbal de SZLINE LIST,
	i mostrara qui va afegir una SZLINE, la data en la que
	va ser afegida, i quan caducara, a més de la mascara IP i
	la raó.
	
	SZLINE CLEAR neteja totes les instàncies de la llista
	de SZLINES.
	
	Limitat a operadors de serveis.

OPER_HELP_SET
	Sintàxi: SET opció parametres
	
	Estableix varies opcions globals de serveis.
	Noms de opcions actualment definides són:
	    READONLY   Estableix Mode nomès lectura o nomès escritura
	    LOGCHAN    Report log messages to a channel
	    DEBUG      Activa o desactiva el Mode debug
	    NOEXPIRE   Activa o desactiva el Mode no expire
	    SUPERADMIN Activate or deactivate super-admin mode
	    SQL   	 Activate or deactivate sql mode
	    IGNORE     Activate or deactivate ignore mode
	    LIST       List the options
	
	Limitat a administradors de serveis.

OPER_HELP_SET_READONLY
	Sintàxi: SET READONLY {ON | OFF}
	
	Estableix el mode nomès lectura a activat o desactivat. En mode
	nomès lectura, els usuaris normals no podran modificar dades
	de serveis, incloent accés a llistes de canals i
	nicknames, etc. IRCops amb suficients privilegis amb
	els serveis podren modificar la llista AKILL de
	serveis i eliminar o prohibir nicknames i canals, però
	cap d'aquests canvios sera guardat excepte que el
	mode nomès lectura es desactivi abans de que els serveis
	siguin desactivats o reiniciats.
	
	Aquesta opció es equivalent a l'opció -readonly en
	la linea de comandes.

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
	Sintàxi: SET DEBUG {ON | OFF | nom}
	
	Estableix el mode debug a activat o desactivat. En mode debug,
	tota la informació enviada desde i cap als serveis,
	a més de un numero de missatges són escrits a l'arxiu
	de log.  Si s'especifica nom, el mode debug es
	activat, amb el nivell de debug especificat a nom.
	
	Aquesta opció es equivalent a l'opció -debug en
	la linea de comandes.

OPER_HELP_SET_NOEXPIRE
	Sintàxi: SET NOEXPIRE {ON | OFF}
	
	Estableix el mode no expire a activat o desactivat. En
	mode no expire, nicks, canals, akills i excepcions
	no caducaran fins que l'opció sigui desactivada.
	
	Aquaquesta opció es equivalent a l'opció -noexpire
	en la linea de comandes.

OPER_HELP_SET_SUPERADMIN
	Syntax: SET SUPERADMIN {ON | OFF}

	Setting this will grant you extra privlages such as the
	ability to be "founder" on all channel's etc...

	This opion is _NOT_ persistant, and should only be used when
	needed, and set back to OFF when no longer needed.

OPER_HELP_SET_SQL
	Syntax: SET SQL {ON | OFF}

	Setting this will toggle Anope's from usage of SQL, this should
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
	Sintàxi: NOOP SET servidor
	          NOOP REVOKE servidor
	
	NOOP SET esborra totes les linees O: del
	servidor especificat i killeja a tots els IRCops actuals
	per a prevenir-los de fer rehash en el servidor
	(ja que això cancel.laria l'efecte).
	
	NOOP REVOKE fa que les linees 0: disponibles novament
	en el servidor especificat.
	
	Nota: El servidor no es testejat pels
	serveis.
	
	Limitat a administradors de serveis.

OPER_HELP_JUPE
	Sintàxi: JUPE servidor [raó]
	
	Li diu als serveis que fagin jupiter (saturn, mart,...) a un servidor
	-- és a dir, crear un "servidor" fals conectant als
	serveis el qual preveu al servidor real de
	connectar-se. El jupe pot ser eliminat utilitzant la comanda
	estandart SQUIT. Si una raó es especificada, es afegida
	en el camp d'informació sobre el servidor; d'altra
	banda, el camp de informació sobre el servidor tindra
	el text "Juped per <nick>", mostrant el nickname
	de la persona que va jupitejar el servidor.
	
	Limitat a administradors de serveis.

OPER_HELP_RAW
	Sintàxi: RAW text
	
	Envia una linea de text directament al servidor al qual
	els serveis estan connectats. Aquesta comanda te un
	us molt limitat i pot malmetre una xarxa si es
	utilitzat de manera impròpia. NO UTILITZIS aquesta COMANDA a
	no ser que estiguis absolutament seguro del que fas!
	    
	Limitat a administradors de serveis.

OPER_HELP_UPDATE
	Sintàxi: UPDATE
	
	Força als serveis a actualitzar tots els arxius de
	la base de dades tan aviat como enviis la comanda.
	
	Limitat a administradors de serveis.

OPER_HELP_RELOAD
	Sintàxi: RELOAD
	
	Força als serveis a recarregar l'arxiu de
	configuració. Tenir en compe que algunes directives
	necessiten que els serveis es reinicin de nou per tenir
	efecte (tal com nicknames dels serveis, activació
	de la limitació de sessions, etc.)
	
	Limitat a administradors de serveis.

OPER_HELP_QUIT
	Sintàxi: QUIT
	
	Força als serveis a parar inmediatament; les bases de
	dades no son guardades. Aquesta comanda no ha de ser utilitzada
	excepte que les còpies en la memòria semblin espatllades i no
	hagin de ser guardades. Per parar normalment, utilitza la comanda
	SHUTDOWN.
	
	Limitat a administradors de serveis.

OPER_HELP_SHUTDOWN
	Sintàxi: SHUTDOWN
	
	Força als serveis a guardar totes les bases de
	datos i parar.
	
	Limitat a administradors de serveis.

OPER_HELP_RESTART
	Sintàxi: RESTART
	
	Força als serveis a guardar totes les bases de
	dades i després parar i arrancar de nou.
	(es a dir, finalitzar el programa e inmediatament reiniciar
	el programa de nou).
	
	Limitat a administradors de serveis.

OPER_HELP_CHANLIST
	Sintàxi: CHANLIST [{patró | nick} [SECRET]]
	
	Llista tots els canals actualment en ús en la xarxa IRC,
	estiguin registrats o no.
	
	Si s'especifica patró, llista nomès els canals que concordin amb ell. 
	Si s'especifica un nickname, llista nomès els canals en els que l'usuari
	hi sigui. Si s'especifica SECRET, llista nomès els canals que
	concordin amb patró i que tinguin modes +s o +p.
	
	Limitat a administradors de serveis.

OPER_HELP_USERLIST
	Sintàxi: USERLIST [{patró | canal} [INVISIBLE]]
	
	Llista tots els usuaris actualment en linea en la xarxa IRC,
	estiguin registrats o no.
	
	Si s'especifica patró, llista nomès els usuaris que concordin
	amb ell (ha de ser amb el format nick!usuari@host). Si s'especifica canal,
	llista nomès els  usuaris que estiguin en el canal donat. Si s'especifica
	INVISIBLE, nomès els usuaris amb la bandera +i seran
	llistats.
	
	Limitat a administradors de serveis.

OPER_HELP_MODLOAD
	Syntax: MODLOAD FileName

	This command loads the module named FileName from the modules
	directory.

	Limited to Services Roots.

OPER_HELP_MODUNLOAD
	Syntax: MODUNLOAD FileName

	This command unloads the module named FileName from the modules
	directory.

	Limited to Services Roots.

OPER_HELP_MODINFO
	Syntax: MODINFO FileName

	This command lists information about the specified loaded module

	Limited to Services Roots.

OPER_HELP_MODLIST
	Syntax: MODLIST [Core|3rd|protocol|encryption|supported|qatested]

	Lists all currently loaded modules.

	Limited to Services Roots.


###########################################################################
#
# BotServ help messages
#
###########################################################################

BOT_HELP_CMD_BOTLIST
	BOTLIST            Llista bots disponibles
BOT_HELP_CMD_ASSIGN
	ASSIGN             Assigna un bot a un canal
BOT_HELP_CMD_SET
	SET                Configura opcions de bots
BOT_HELP_CMD_KICK
	KICK               Configura kickers
BOT_HELP_CMD_BADWORDS
	BADWORDS           Manté una llista de paraules malsonants
BOT_HELP_CMD_ACT
	ACT                Makes the bot do the equivalent of a "/me" command
BOT_HELP_CMD_INFO
	INFO               Allows you to see BotServ information about a channel or a bot
BOT_HELP_CMD_SAY
	SAY                Makes the bot say the given text on the given channel
BOT_HELP_CMD_UNASSIGN
	UNASSIGN           Unassigns a bot from a channel
BOT_HELP_CMD_BOT
	BOT                Manté la llista de bots de la xarxa
	
BOT_HELP
	%S et permet crear un bot en el teu propi canal.
	Això ha estat creat per a usuaris que no poden
	tenir un bot en un host o configurar un bot, o per a us en xarxes
	que no permeten bots d'usuaris. Comandes
	disponibles estan llistades més endavant; per a
	utilitzar-los, escriu %R%S comanda. per a més
	informació sobre un comanda especifica, escriu  
	%R%S HELP comanda.
	
	
BOT_HELP_FOOTER
	El bot entrara a un canal quan hi hagin
	%d usuari(s) en ell.
	
BOT_HELP_BOTLIST
	Sintàxi: BOTLIST
	
	Llista tots els bots disponibles en aquesta xarxa.
	
BOT_HELP_ASSIGN
	Sintàxi: ASSIGN canal nick
	
	Assigna un bot especificat per nick al canal.
	Desprès d'això pots configurar el bot segons
	les teves necessitats.

BOT_HELP_UNASSIGN
	Sintàxi: UNASSIGN canal
	
	Desassigna un bot d'un canal. A l'utilitzar aquesta comanda
	el bot ja no entrara al canal. De totes maneres, la configuració
	del bot pel canal es enmagatzemada, així et sera possible
	reassignar un bot sense haver de configurar-lo de nou.

BOT_HELP_INFO
	Sintàxi: INFO {canal | nick}
	
	Permet veure la informació de %S sobre un canal o
	bot. Si el parametre es un canal, rebràs informació
	així como kickers activats. Si el parametre es un nick,
	rebràs informació sobre el bot, així com temps
	de creació o nom de canals en els que aquest estigui.

BOT_HELP_SET
	Sintàxi: SET canal opció parametres
	
	Configura opcions de bot.  Opció pot ser uno de:
	
	    DONTKICKOPS      Per a protegir ops contra kicks de bots
	    DONTKICKVOICES   Per a protegir voices contra kicks de bots
	    GREET            Habilita missatges de benvinguda
	    FANTASY          Habilita comandoe de fantasia
	    SYMBIOSIS        Permet al bot a actuar como un bot real
	
	Escriu %R%S HELP SET opció per més informació
	sobre una opció especifica.
	
	Nota: L'accés a aquesta comanda esta controlat pel
	nivell SET.
	
BOT_HELP_SET_DONTKICKOPS
	Sintàxi: SET canal DONTKICKOPS {ON|OFF}
	
	Habilita o deshabilita el mode de protecció d'ops
	en un canal. En habilitar-lo, ops no seran
	kickejats pel bot, encara que no concideixen amb
	el nivell NOKICK.

BOT_HELP_SET_DONTKICKVOICES
	Sintàxi: SET canal DONTKICKVOICES {ON|OFF}
	
	Habilita o deshabilita el mode de protecció de voices
	en un canal. En hbilitarlo, voices no seran
	kickejatos pel bot, encara que no coincideixin amb
	el nivell NOKICK.

BOT_HELP_SET_FANTASY
	Sintàxi: SET canal FANTASY {ON|OFF}
	
	Habilita o deshabilita el mode de fantasia
	en un canal. En habilitar-lo, els usuaris podran
	utilitzar les comandes !op, !deop, !voice, !devoice,
	!kick, !kb, !unban, !seen en un canal (investiga
	com utilitzar-los; intenta amb o sense nick per cadascú
	i amb raó per a alguns?).
	
	Cal dir que els usuaris que desitgin utilitzar comandes
	de fantasia HAN DE DISPOSSAR del nivell necessari
	pel nivell FANTASIA i altres depenent
	de la comanda si es requereix (per exemple, per a
	utilitzar !op, l'usuari ha de tenir suficient
	accés pel nivell OPDEOP).

BOT_HELP_SET_GREET
	Sintàxi: SET canal GREET {ON|OFF}
	
	Habilita o deshabilita el mode de asludo en
	un canal. En habilitar-lo, el bot mostrara els
	missatges dels usuaris que entrin al canal,
	sempre i quan tinguin el suficient accés
	al canal.

BOT_HELP_SET_SYMBIOSIS
	Sintàxi: SET canal SYMBIOSIS {ON|OFF}
	
	Habilita o dehsabilita el mode simbiosis en
	un canal. En habilitar-lo, el bot fara qualsevol
	cosa que normalmente hauria fet %s en els
	canals, tal como modes, KICKS i fins tot els
	missatges d'entrada.

BOT_HELP_KICK
	Sintàxi: KICK canal opció parametres
	
	Configura els kickers de bot. opció pot ser una de:
	
	    BOLDS         Estableix si el bot kickejara negretes
	    BADWORDS      Estableix si el bot kickejara paraules malsonants
	    CAPS          Estableix si el bot kickejara majuscules
	    COLORS        Estableix si el bot kickea colors
	    FLOOD         Estableix si el bot kickejara usuaris
	                  que floodejan
	    REPEAT        Estableix si el bot kickejara usuaris
	                  que repeteixen com l'allioli
	    REVERSES      Estableix si el bot kickejara reversos
	    UNDERLINES    Estableix si el bot kickejara subratllats
	
	Escriu %R%S HELP KICK Opció per més
	informació sobre una opció especifica.
	
	Nota: L'accés a aquesta comanda es control.lat pel
	nivell SET.

BOT_HELP_KICK_BOLDS
	Sintàxi: KICK canal BOLDS {ON|OFF} [ttb]

	Estableix el kick per negretes a actvitat o desactivat.
	En habilitar-lo, aquesta opció li diu al bot que kickegi
	usuaris que utilitzin negretes.
	
	ttb es el numero de cops que un usuari sera kickejat
	abans de ser banejat. No cal especificar ttb per a deshabilitar
	el sistema de ban un cop activat.

BOT_HELP_KICK_COLORS
	Sintàxi: KICK canal COLORS {ON|OFF} [ttb]
	
	Estableix el kick per colors a activat o desactivat.
	En habilitar-lo, aquesta opció li diu al bot que kickegi
	usuaris que utilitzin colors.
	
	ttb es el numero de cops que un usuari sera kickejat
	abans de ser banejat. No cal especificar ttb per a deshabilitar
	el sistema de ban un cop activat.

BOT_HELP_KICK_REVERSES
	Sintàxi: KICK canal REVERSES {ON|OFF} [ttb]
	
	Estableix el kick per reversos a activat o desactivat.
	En habilitar-lo, aquesta opció li diu al bot que kickegi
	usuaris que utilitzin reversos.
	
	ttb es el numero de cops que un usuari sera kickejat
	abans de ser banejat. No cal especificar ttb per a deshabilitar
	el sistema de ban un cop activat.

BOT_HELP_KICK_UNDERLINES
	Sintàxi: KICK canal UNDERLINES {ON|OFF} [ttb]
	
	Estableix el kick per subratllats a activat o apagat.
	En habilitar-lo, aquesta opció li diu al bot que kickegi
	usuaris que utilitzin subratllats.
	
	ttb es el numero de cops que un usuari sera kickejat
	abans de ser banejat. No cal especificar ttb per a deshabilitar
	el sistema de ban un cop activat.

BOT_HELP_KICK_CAPS
	Sintàxi: KICK canal CAPS {ON|OFF} [ttb [min [porcentaje]]]

	Estableix el kick per mayusculas a activat o desactivat.
	En habilitar-lo, aquesta opció le diu al bot que kickegi
	usuaris que estiguin escrivint en majuscules.
	
	El bot kickeja nomès si hi ha com a mínim min majuscules
	i que constitueixin com a mínim porcentatge%% del total
	del text (si no s'especifica, per defecte s'estableix a 10
	caràcters i 25%%).
	
	ttb es el numero de cops que un usuari sera kickejat
	abans de ser banejat. No cal especificar ttb per a deshabilitar
	el sistema de ban un cop activat.

BOT_HELP_KICK_FLOOD
	Sintàxi: KICK canal FLOOD {ON|OFF} [ttb [ln [segs]]]

	Estableix el kick per flood a activat o desactivat.
	En habilitar-lo, aquesta opció li diu al bot que kickegi
	usuaris que estiguin floodejant el canal utilitzant com a mínim
	ln linees en segs segons (si no s'especifica, per
	defecte es establert a 6 linees en 10 segons).
	
	ttb es el numero de cops que un usuari sera kickejat
	abans de ser banejat. No cal especificar ttb per a deshabilitar
	el sistema de ban un cop activat.

BOT_HELP_KICK_REPEAT
	Sintàxi: KICK #canal REPEAT {ON|OFF} [ttb [num]]
	
	Estableix el kick per repetició a activat o desactivat.
	En habilitar-lo, aquesta opció li diu al bot que kickegi
	usuaris que estiguin repetint num cops (si num no s'especifica,
	per defecte es establert a 3).
	
	ttb es el numero de cops que un usuari sera kickejat
	abans de ser banejat. No cal especificar ttb per a deshabilitar
	el sistema de ban un cop activat.

BOT_HELP_KICK_BADWORDS
	Sintàxi: KICK #canal BADWORDS {ON|OFF} [ttb]
	
	Estableix el kick per paraules malsonants a activat o desactivat.
	En habilitar-lo, aquesta opció li diu al bot que kickegi
	usuaris que utilitzin certes paraules al canal.
	
	Pots definir paraules malsonants pel teu canal utilitzant la comanda
	BADWORDS. Escriu %R%S HELP BADWORDS per més informació.
	
	ttb es el numero de cops que un usuari sera kickejat
	abans de ser banejat. No cal especificar ttb per a deshabilitar
	el sistema de ban un cop activat.

BOT_HELP_BADWORDS
	Sintàxi: BADWORDS canal ADD paraula [SINGLE | START | END]
	          BADWORDS canal DEL {paraula | nom | llista}
	          BADWORDS canal LIST [mascara | llista]
	          BADWORDS canal CLEAR
	
	Mante la llista de paraules malsonants per un canal. La llista
	de paraules malsonants determina quines paraules han de donar peu a ser kickejats
	Quan el kick per paraules malsonants esta habilitat. Per més
	informació, escriu %R%S HELP KICK BADWORDS.
	
	La comanda BADWORDS ADD afegeix una paraula a la llista
	de paraules malsonants. Si s'especifica SINGLE, el kick es
	fara nomès si l'usuari du la paraula completa. Si s'especifica
	START, el kick es fara nomès si l'usuari
	escriu una paraula que comenci amb paraula.
	Si no s'especifica END, el kick es fara nomès si
	l'usuari escriu una paraula que finalitzi amb paraula. Si
	no s'especifica res, un kick es farà cada vegada que
	paraula sigui escrita per un usuari.
	
	La comanda BADWORDS DEL esborra la paraula esmantada de la
	llista de paraules malsonants. Si s'especifica una llista de noms, aquestes
	instàncies son esborrades. (Veure l'exemple per LIST especificat més
	endavant).
	
	La comanda BADWORDS LIST mostra la llista de bad
	words. Si s'especifica una mascara, nomès les instàncies
	concordant amb la mascara són mostrades. Si s'especifica una llista
	de noms, nomès aquestes instàncies son mostrades;
	per exemple:
	
	   BADWORDS #canal LIST 2-5,7-9
	      Llista paraules malsonants enumerades de 2 a 5 i de 7 a 9.
	      
	La comanda BADWORDS CLEAR neteja totes les instàncies
	de la llista de paraules malsonants.

BOT_HELP_SAY
	Sintàxi: SAY canal text
	
	Fa que el bot escrigui el text especificat en el canal especificat.

BOT_HELP_ACT
	Sintàxi: ACT canal text
	
	Fa que el bot fagi l'equivalent a la comanda "/me"
	en el canal especificat utilitzant el text especificat.


BOT_SERVADMIN_HELP_BOT
	Sintaxi: BOT ADD nick usuari host real
	          BOT CHANGE nick-ant nick-nue [usuari [host [real]]]
	          BOT DEL nick
	
	Permet als administradors de serveis crear, modificar
	i esborrar bots que usuaris podran usar en els seus propis
	canals.
	
	BOT ADD afegeix un bot con el nickname, usuari, host
	i nombre reial donats. Ja que no es fan comprovacions 
	d'integritat per aquests parametres, tingues molta cura.
	BOT CHANGE permet cambiar el nickname, usuari, host
	o nom real d'un bot sense tenir que esborrar-lo (i la
	informació associada a ell).
	BOT DEL esborra el bot donat de la llista de bots.
	
	Nota: Si fas un bot amb un nick ja enregistrat,
	sera eliminat. A més, si un usuari està actualment
	usant el nick, serà killejat.

BOT_SERVADMIN_HELP_SET
	
	Aquestes opcions estan reservades a administradors de
	Serveis:
	
	    NOBOT            Evita a un bot de ser assignat
	                        a un canal
	    PRIVATE          Evita a un bot de ser assignat
	                     per algú mes que un operador
	                     d'IRC

BOT_SERVADMIN_HELP_SET_NOBOT

	Sintaxi: SET canal NOBOT {ON|OFF}
	
	Aquesta opció fa al canal no sigui assignable. Si un
	bot ja està assignat al canal, es desassignara
	automàticament quan habiliteu aquesta opció.
	
	Limitat a administradors de serveis.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Sintaxi: SET nick-del-bot PRIVATE {ON|OFF}
	
	Aquesta opció evita un bot de ser assignat
	a un canal per usuaris que no son operadors
	d'IRC.
	
	Limitat a administradors de serveis.
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
	Error! A vhost can only contain A-Z, a-z, 0-9, '.' and '-'.
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

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_OFF
	Your vhost was removed and the normal cloaking restored.
HOST_OFF_UNREAL
	Your vhost was removed. To re-enable the standard host cloaking, type /mode %s +%s
HOST_NO_VIDENT
	Your IRCD does not support vIdent's, if this is incorrect, please report this as a possible bug

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_GROUP
	All vhosts in the group %s have been set to %s
HOST_IDENT_GROUP
	All vhosts in the group %s have been set to %s@%s

###########################################################################
#
# HostServ Help messages
#
###########################################################################
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

	Limited to Services operators.

HOST_HELP_DELALL
	Syntax: DELALL <nick>.

	Deletes the vhost for all nick's in the same group as
	that of the given nick.

	Limited to Host Removers.


HOST_HELP_SETALL
	Syntax: SETALL <nick> <hostmask>.

	Sets the vhost for all nicks in the same group as that
	of the given nick.  If your IRCD supports vIdents, then
	using SETALL <nick> <ident>@<hostmask> will set idents
	for users as well as vhosts.

	* NOTE, this will not update the vhost for any nick's
	added to the group after this command was used.

	Limited to Services operators.

HOST_HELP_OFF
	Syntax: OFF

	Deactivates the vhost currently assigned to the nick in use.
	When you use this command any user who performs a /whois
	on you will see your real IP address.

HOST_HELP_DEL
	Syntax: DEL <nick>

	Deletes the vhost assigned to the given nick from the
	database.

	Limited to Services operators.

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

	Limited to Services operators.

HOST_HELP_GROUP
	Syntax: GROUP
	
	This command allows users to set the vhost of thier
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
