# Deutsches Language file.
#
# (C) 2003-2011 Anope Team
# Contact us at team@anope.org
#   
# Please read COPYING and README for further details.
#     
# Based on the original code of Epona by Lara.
# Based on the original code of Services by Andy Church. 
#
# Translated by chemical <chem@gmx.net>,shine <dh@shinewelt.de> bis 28.02.2003
# - Solltest du irgendwelche Rechtschreibfehler oder sonstige
#   Verbesserungsvorschläge haben, immer her damit!
#
# INFO ::: Letzte Änderung:	29.01.2011	-- Han` -- Han@mefalcon.org --
#
##########################################################################
#
# This program is free but copyrighted software; see the file COPYING for
# details.
#
# Based on the original code of Services by Andy Church.
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
##########################################################################
#
# Name of this language
#
###########################################################################

# For languages other than English, this string should have the following
# format:
#     language-name-in-language (language-name-in-English)
# For example, "Español (Spanish)" or "Français (French)".

LANG_NAME
	Deutsch (German)

###########################################################################
#
# General messages  ## 20.10.2004 ##
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
	%d.%b.%Y, %H:%M:%S %Z

# This is used as the format string for strftime() for a date alone in long
# format (English: including weekday).
STRFTIME_LONG_DATE_FORMAT
	%a, %d.%B.%Y
# This is used as the format string for strftime() for a date alone in
# short format (English: without weekday).
STRFTIME_SHORT_DATE_FORMAT
	%d.%b.%Y

# These tell our strftime() what the names of months and days are.  If you
# don't use %a, %A, %b, or %B in your strftime() strings above, you can
# leave these empty.  However, if you enter names, they MUST stay in order,
# one per line, and the list MUST be complete!

# %a
STRFTIME_DAYS_SHORT
	So
	Mo
	Di
	Mi
	Do
	Fr
	Sa
# %A
STRFTIME_DAYS_LONG
	Sonntag
	Montag
	Dienstag
	Mittwoch
	Donnerstag
	Freitag
	Samstag
# %b
STRFTIME_MONTHS_SHORT
	Jan
	Feb
	Mär
	Apr
	Mai
	Jun
	Jul
	Aug
	Sep
	Okt
	Nov
	Dez
# %B
STRFTIME_MONTHS_LONG
	Januar
	Februar
	März
	April
	Mai
	Juni
	Juli
	August
	September
	Oktober
	November
	Dezember


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	, 


# Various error messages.
USER_RECORD_NOT_FOUND
	Interner Fehler - kann die Anfrage nicht verarbeiten.
UNKNOWN_COMMAND
	Unbekannter Befehl %s.
UNKNOWN_COMMAND_HELP
	Unbekannter Befehl %s.  "%R%s HELP" für Hilfe.
SYNTAX_ERROR
	Syntax: %s
MORE_INFO
	%R%s HELP %s für weitere Informationen.
NO_HELP_AVAILABLE
	Keine Hilfe verfügbar für %s.
OBSOLETE_COMMAND
	Dieser Befehl ist überholt, benutze stattdessen %s.

BAD_USERHOST_MASK
	Die Hostmaske sollte in dieser Form sein: user@host.
BAD_EXPIRY_TIME
	Ungültige Auslaufzeit.
USERHOST_MASK_TOO_WIDE
	%s ist zu allgemeingültig, bitte benutze eine genauere Hostmaske.

SERVICE_OFFLINE
	%s sind derzeit nicht verfügbar, bitte versuche es später noch einmal.
READ_ONLY_MODE
	Achtung: Services sind im Read-Only-Modus, Änderungen werden nicht übernommen!

PASSWORD_INCORRECT
	Falsches Passwort.
INVALID_TARGET
	"/msg %s" wird nicht mehr unterstützt.  Benutze "/msg %s@%s" oder "/%s" stattdessen.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Zugriff verweigert.
PERMISSION_DENIED
	Berechtigung nicht ausreichend.
RAW_DISABLED
	RAW wurde deaktiviert. Um RAW nutzen zu können, muss DisableRaw in der Services Config Datei deaktiviert werden.

MORE_OBSCURE_PASSWORD
	Bitte versuche es nochmal mit einem besseren Passwort. 
	Passworte sollten mindestens 5 Zeichen lang sein und 
	nichts mit deinem Nicknamen zu tun haben. Weiterhin 
	kann keine Leer oder Tabulatortaste enthalten sein.

PASSWORD_TOO_LONG
	Dein Passwort ist zu lang. Versuche es erneut mit einen kürzeren Passwort.

NICK_NOT_REGISTERED
	Dein Nickname ist nicht registriert.
NICK_NOT_REGISTERED_HELP
	Dein Nickname ist nicht registriert. Benutze %R%s HELP REGISTER um zu erfahren wie du einen Nicknamen registrieren kannst.
NICK_X_IS_SERVICES
	Der Nickname %s ist ein teil der  Network Services.
NICK_X_NOT_REGISTERED
	Der Nickname %s ist nicht registriert.
NICK_X_IN_USE
	Der Nickname %s wird derzeit benutzt.
NICK_X_NOT_IN_USE
	Der Nickname %s wird derzeit nicht benutzt.
NICK_X_NOT_ON_CHAN
	%s ist derzeit nicht im channel %s.
NICK_X_FORBIDDEN
	Der Nickname %s ist verboten und darf nicht verwendet werden.
NICK_X_FORBIDDEN_OPER
	Der Nickname %s wurde verboten von %s:
	%s
NICK_X_ILLEGAL
	Der Nickname %s ist ein illegaler Nickname und kann nicht benutzt werden.
NICK_X_TRUNCATED
	Dein Nickname %s wurde auf %d Zeichen gekürzt.
NICK_X_SUSPENDED
	Der Nick %s ist vorübergehend gesperrt.
CHAN_X_NOT_REGISTERED
	Der Channel %s ist nicht registriert.
CHAN_X_NOT_IN_USE
	Dieser Channel %s existiert nicht.
CHAN_X_FORBIDDEN
	Der Channel %s ist verboten und darf nicht verwendet werden.
CHAN_X_FORBIDDEN_OPER
	Der Channel %s wurde verboten von %s:
	%s
CHAN_X_SUSPENDED
	      Vorübergehend gesperrt: [%s] %s
NICK_IDENTIFY_REQUIRED
	Um diesen Befehl nutzen zu können, musst du dich identifizieren.
	Versuch es erneut nach %R%s IDENTIFY Passwort.
CHAN_IDENTIFY_REQUIRED
	Um diesen Befehl nutzen zu können, musst du dich identifizieren.
	Versuch es erneut nach %R%s IDENTIFY %s Passwort.
	
MAIL_DISABLED
	eMail-Handling der Services wurde deaktiviert.
MAIL_INVALID
	eMail-Adresse für %s ist ungültig.
MAIL_X_INVALID
	%s ist keine gültige eMail-Adresse.
MAIL_LATER
	Die Services können gerade keine Mail verschicken, bitte versuche es später noch einmal.
MAIL_DELAYED
	Bitte warte noch %d Sekunden und versuche es dann erneut.
	
NO_REASON
	Kein Grund
UNKNOWN
	<unbekannt>

# Duration system
DURATION_DAY
	1 Tag
DURATION_DAYS
	%d Tage
DURATION_HOUR
	1 Stunde
DURATION_HOURS
	%d Stunden
DURATION_MINUTE
	1 Minute
DURATION_MINUTES
	%d Minuten
DURATION_SECOND
	1 Sekunde
DURATION_SECONDS
	%d Sekunden

# Human readable expiration
NO_EXPIRE
	läuft nicht aus
EXPIRES_SOON
	verfällt beim nächstem Datenbank-Update
EXPIRES_M
	verfällt in %d Minuten
EXPIRES_1M
	verfällt in %d Minute
EXPIRES_HM
	verfällt in %d Stunden, %d Minuten
EXPIRES_H1M
	verfällt in %d Stunden, %d Minute
EXPIRES_1HM
	verfällt in %d Stunde, %d Minuten
EXPIRES_1H1M
	verfällt in %d Stunde, %d Minute
EXPIRES_D
	verfällt in %d Tagen
EXPIRES_1D
	verfällt in %d Tag

# Generic Footer message
END_OF_ANY_LIST
	Ende der %s Liste.

###########################################################################
#
# NickServ messages 
#
###########################################################################

# Automatic messages
LIST_INCORRECT_RANGE
	Du hast einen fehlerhaften Bereich angegeben. 
	Die richtige Schreibweise ist #von-bis.
	Beispiel: #1-50 oder #1-100
CS_LIST_INCORRECT_RANGE
	Um nach Channeln zu suchen die mit # beginnen, suche nach Channelnamen
	ohne das #-Symbol voranszutellen (anope anstatt #anope).
	Wildcards können verwendet werden.

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
HELP_LIMIT_SERV_OPER
	Diese Funktion  ist beschränkt auf Services Operatoren.
HELP_LIMIT_SERV_ADMIN
	Diese Funktion  ist beschränkt auf Services Administratoren.
HELP_LIMIT_SERV_ROOT
	Diese Funktion  ist beschränkt auf Services Root-Administratoren.
HELP_LIMIT_IRC_OPER
	Diese Funktion  ist beschränkt auf IRC Operatoren.
HELP_LIMIT_HOST_SETTER
	Diese Funktion  ist beschränkt auf Host Setters.
HELP_LIMIT_HOST_REMOVER
	Diese Funktion  ist beschränkt auf Host Removers.


###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	Dieser Nickname gehört schon jemand anderem. Bitte wähle einen anderen.
	(Sollte dieser Nickname dir gehören, tippe %R%s IDENTIFY Passwort.)
NICK_IS_SECURE
	Dieser Nickname ist registriert und geschützt.  Wenn es dein Nickname
	ist, tippe %R%s IDENTIFY Passwort. Ansonsten wähle bitte
	einen anderen Nickname.
NICK_MAY_NOT_BE_USED
	Dieser Nickname kann nicht benutzt werden. Bitte wähle einen anderen.
FORCENICKCHANGE_IN_1_MINUTE
	Wenn du ihn nicht innerhalb einer Minute identifizierst, wird er automatisch geändert.
FORCENICKCHANGE_IN_20_SECONDS
	Wenn du ihn nicht innerhalb von 20 Sekunden identifizierst, wird er automatisch geändert.
FORCENICKCHANGE_NOW
	Dieser Nickname ist registriert, du kannst ihn nicht benutzen.
FORCENICKCHANGE_CHANGING
	Dein Nickname wurde geändert in %s

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER Passwort [eMail]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER Passwort eMail
NICK_REGISTRATION_DISABLED
	Das Registrieren von Nicknamen ist derzeit deaktiviert.
NICK_REGISTRATION_FAILED
	Registrierung fehlgeschlagen.
NICK_REG_PLEASE_WAIT
	Bitte warte %d Sekunden, bevor der REGISTER Befehl wieder funktioniert.
NICK_CANNOT_BE_REGISTERED
	Dieser Nickname %s kann nicht registriert werden.
NICK_ALREADY_REGISTERED
	Dieser Nickname %s ist bereits registriert!
NICK_REGISTERED
	Dein Nickname %s ist unter deinem Host registriert worden: %s
NICK_REGISTERED_NO_MASK
	Der Nickname %s wurde erfolgreich registriert.
NICK_PASSWORD_IS
	Dein Passwort ist %s - behalte es für spätere Benutzung.
NICK_REG_DELAY
	Du musst länger als %d Sekunden verbunden sein, um einen Nicknamen zu registrieren.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP Ziel Passwort
NICK_GROUP_DISABLED
	Das Gruppieren von Nicknamen ist derzeit deaktiviert.
NICK_GROUP_FAILED
	Gruppieren fehlgeschlagen.
NICK_GROUP_PLEASE_WAIT
	Bitte warte %d Sekunden, bevor der GROUP Befehl wieder funktioniert.
NICK_GROUP_CHANGE_DISABLED
	Dein Nickname ist bereits registriert, tippe %R%s DROP um ihn zu löschen.
NICK_GROUP_SAME
	Du bist bereits Mitglied der Gruppe %s.
NICK_GROUP_TOO_MANY
	Es sind zu viele Nicknamen in %s's Gruppe. Liste sie auf und lösche einige davon.
	Tippe %R%s HELP GLIST und %R%s HELP DROP für weitere Infos.
NICK_GROUP_JOINED
	Du bist jetzt in der Gruppe %s vertreten.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY Passwort
NICK_IDENTIFY_FAILED
	Die Identifizierung ist fehlgeschlagen.
NICK_IDENTIFY_SUCCEEDED
	Passwort akzeptiert - du bist jetzt angemeldet.
NICK_IDENTIFY_EMAIL_REQUIRED
	Du musst eine eMail-Adresse für deinen Nicknamen angeben.
	NickServ kann dir dann dein Passwort per Mail senden, falls du es
	vergessen solltest.
NICK_IDENTIFY_EMAIL_HOWTO
	Tippe %R%S SET EMAIL eMail wenn du deine eMail-Adresse einstellen willst.
	Deine Privatsphäre wird respektiert und deine eMail-Adresse ist Dritten nicht 
	zugänglich.
	
# LOGOUT responses
NICK_ALREADY_IDENTIFIED
	Du bist bereits angemeldet.

# UPDATE responses
NICK_UPDATE_SUCCESS
	Status aktualisiert (memos, vhost, chmodes, flags).
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	Du wurdest ausgeloggt.
NICK_LOGOUT_X_SUCCEEDED
	Der Nickname %s wurde erfolgreich ausgeloggt.

# DROP responses
NICK_LOGOUT_SERVICESADMIN
	Ausloggen von %s nicht möglich, da es sich um einen Services Administrator handelt.

# DROP responses
NICK_DROP_DISABLED
	Das Entfernen von Nicknamen ist derzeit deaktiviert.
NICK_DROPPED
	Die Registrierung deines Nicknamens wurde gelöscht.
NICK_X_DROPPED
	Der Nickname %s wurde aus der Datenbank entfernt.

# SET responses
NICK_SET_SYNTAX
	SET Option Parameter
NICK_SET_SERVADMIN_SYNTAX
	SET [Nickname] Option Parameter
NICK_SET_DISABLED
	Das Ändern der Optionen wurde deaktiviert.
NICK_SET_UNKNOWN_OPTION
	Unbekannte SET Option %s.
NICK_SET_OPTION_DISABLED
	Option %s kann in diesem Netzwerk nicht benutzt werden.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	Der neue "Anführer" MUSS ein Nickname in deiner Gruppe sein!
NICK_SET_DISPLAY_CHANGED
	Der neue "Anführer" deiner Gruppe ist jetzt %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Konnte dein Passwort nicht ändern.
NICK_SET_PASSWORD_CHANGED
	Passwort geändert.
NICK_SET_PASSWORD_CHANGED_TO
	Passwort geändert in %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE Nummer
NICK_SET_LANGUAGE_UNKNOWN
	Unbekannte Sprachen-Nummer %d.  Tippe %R%s HELP SET LANGUAGE um eine Liste aller verfügbaren Sprachen zu erhalten.
NICK_SET_LANGUAGE_CHANGED
	Sprache wurde geändert zu Deutsch.

# SET URL responses
NICK_SET_URL_CHANGED
	URL zu %s geändert.
NICK_SET_URL_UNSET
	URL entfernt.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	eMail-Adresse zu %s geändert.
NICK_SET_EMAIL_UNSET
	eMail-Adresse wurde entfernt.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	Du kannst in diesem Netzwerk deine eMail-Adresse nicht entfernen.
	
# SET ICQ responses
NICK_SET_ICQ_CHANGED
	ICQ-Nummer auf %s gesetzt.
NICK_SET_ICQ_UNSET
	ICQ-Nummer wurde entfernt.
NICK_SET_ICQ_INVALID
	%s ist keine gültige ICQ-Nummer.
	
# SET GREET responses
NICK_SET_GREET_CHANGED
	Begrüssungsnachricht wurde zu %s geändert.
NICK_SET_GREET_UNSET
	Begrüssungsnachricht wurde entfernt.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Kill-Schutz ist jetzt ON.
NICK_SET_KILL_QUICK
	Kill-Schutz ist jetzt ON, mit einer geringeren Wartezeit.
NICK_SET_KILL_IMMED
	Kill-Schutz ist jetzt ON, ohne Wartezeit.
NICK_SET_KILL_IMMED_DISABLED
	Die IMMED Option ist in diesem Netzwerk nicht verfügbar.
NICK_SET_KILL_OFF
	Kill-Schutz ist jetzt OFF.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Nickname-Sicherheit ist jetzt ON.
NICK_SET_SECURE_OFF
	Nickname-Sicherheit ist jetzt OFF.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Privat-Option ist jetzt ON.
NICK_SET_PRIVATE_OFF
	Privat-Option ist jetzt OFF.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Deine eMail-Adresse wird jetzt in der %s INFO Anzeige nicht mehr erscheinen.
NICK_SET_HIDE_EMAIL_OFF
	Deine eMail-Adresse wird jetzt in der %s INFO Anzeige erscheinen.
NICK_SET_HIDE_MASK_ON
	Deine letzte Hostmaske user@host wird jetzt in der %s INFO Anzeige nicht mehr erscheinen.
NICK_SET_HIDE_MASK_OFF
	Deine letzte Hostmaske user@host wird jetzt in der %s INFO erscheinen.
NICK_SET_HIDE_QUIT_ON
	Deine letzte Quit-Message wird jetzt in der %s INFO Anzeige nicht mehr erscheinen.
NICK_SET_HIDE_QUIT_OFF
	Deine letzte Quit-Message wird jetzt in der %s INFO Anzeige erscheinen.
NICK_SET_HIDE_STATUS_ON
	Dein Services-Zugriffsstatus wird jetzt in %s INFO Anzeigen versteckt.
NICK_SET_HIDE_STATUS_OFF
	Dein Services-Zugriffsstatus wird nicht länger in %s INFO Anzeigen versteckt.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_MSG_ON
	Die Services werden dir jetzt durch messages antworten.
NICK_SET_MSG_OFF
	Die Services werden dir jetzt durch notices antworten.

# SET AUTOOP responses
NICK_SET_AUTOOP_SYNTAX
	SET AUTOOP {ON | OFF}
NICK_SET_AUTOOP_ON
	Die Services werden Dir jetzt automatisch Op-Status geben in den Channels.
NICK_SET_AUTOOP_OFF
	Die Services werden Dir jetzt nicht mehr automatisch Op-Status geben in den Channels.

# SET NOEXPIRE responses
NICK_SASET_SYNTAX
	SASET nickname option parameters
NICK_SASET_DISABLED
	Das Setzen von Nick-Optionen ist zeitweise deaktiviert.
NICK_SASET_UNKNOWN_OPTION
	Ubekannte SASET option %s.
NICK_SASET_BAD_NICK
	Nickname %s ist nicht registriert.
NICK_SASET_OPTION_DISABLED
	Die Einstellung %s ist auf diesem Netzwerk nicht verfügbar.

# SASET DISPLAY responses
NICK_SASET_DISPLAY_INVALID
	Der für %s angezeigte Ursprung muss ein Nickname aus der zugehörigen Gruppe sein!
NICK_SASET_DISPLAY_CHANGED
	Der angezeigte Gruppenursprung ist jetzt %s.

# SASET PASSWORD responses
NICK_SASET_PASSWORD_FAILED
	Das Passwort für %s konnte nicht geändert werden.
NICK_SASET_PASSWORD_CHANGED
	Passwort für %s wurde geändert.
NICK_SASET_PASSWORD_CHANGED_TO
	Passwort für %s wurde auf %s gesetzt.

# SASET LANGUAGE responses
NICK_SASET_URL_CHANGED
	URL für %s wurde auf %s gesetzt.
NICK_SASET_URL_UNSET
	URL %s wurde entfernt.

# SASET EMAIL responses
NICK_SASET_EMAIL_CHANGED
	E-mail Adresse für %s wurde auf %s geändert.
NICK_SASET_EMAIL_UNSET
	E-mail Adresse  %s wurde entfernt.
NICK_SASET_EMAIL_UNSET_IMPOSSIBLE
	Auf diesem Netzwerk ist es nicht möglich die E-Mail Adresse zu entfernen.
	
# SASET ICQ responses
NICK_SASET_ICQ_CHANGED
	ICQ Nummer für %s wurde auf %s gesetzt.
NICK_SASET_ICQ_UNSET
	ICQ Nummer %s wurde entfernt.
NICK_SASET_ICQ_INVALID
	%s ist keine gültige ICQ Nummer.
	
# SASET GREET responses
NICK_SASET_GREET_CHANGED
	Begrüssungsnachricht %s wurde auf %s geändert.
NICK_SASET_GREET_UNSET
	Begrüssungsnachricht %s wurde entfernt.

# SASET PROTECT responses
NICK_SASET_KILL_SYNTAX
	SASET nickname KILL {ON | QUICK | OFF}
NICK_SASET_KILL_IMMED_SYNTAX
	SASET nickname KILL {ON | QUICK | IMMED | OFF}
NICK_SASET_KILL_ON
	Schutz des Nicknamens %s ist jetzt aktiviert (ON).
NICK_SASET_KILL_QUICK
	Schutz des Nicknamens %s mit verkürzter Verzögerung aktiviert (ON).
NICK_SASET_KILL_IMMED
	Schutz des Nicknamens %s ohne Verzögerung aktiviert (ON).
NICK_SASET_KILL_IMMED_DISABLED
	Die IMMED Einstellung (Schutz ohne Verzögerung) ist auf diesem Netzwerk nicht verfügbar.
NICK_SASET_KILL_OFF
	Schutz des Nicknamens %s ist jetzt deaktiviert (OFF).

# SASET SECURE responses
NICK_SASET_SECURE_SYNTAX
	SASET nickname SECURE {ON | OFF}
NICK_SASET_SECURE_ON
	Sicherheitsfunktion (SECURE) ist für %s jetzt aktiviert (ON).
NICK_SASET_SECURE_OFF
	Sicherheitsfunktion (SECURE) ist für %s jetzt deaktiviert (OFF).

# SASET PRIVATE responses
NICK_SASET_PRIVATE_SYNTAX
	SASET nickname PRIVATE {ON | OFF}
NICK_SASET_PRIVATE_ON
	Der Privat-Modus (PRIVATE) ist für %s jetzt aktiviert (ON).
NICK_SASET_PRIVATE_OFF
	Der Privat-Modus (PRIVATE) ist für %s jetzt deaktiviert (OFF).

# SASET HIDE responses
NICK_SASET_HIDE_SYNTAX
	SASET nickname HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SASET_HIDE_EMAIL_ON
	Die Emailadresse %s wird jetzt bei %s INFO Abrufen versteckt.
NICK_SASET_HIDE_EMAIL_OFF
	Die Emailadresse %s wird jetzt bei %s INFO Abrufen angezeigt.
NICK_SASET_HIDE_MASK_ON
	Die letzte bekannte user@host Maske von %s wird jetzt bei %s INFO Abrufen versteckt.
NICK_SASET_HIDE_MASK_OFF
	Die letzte bekannte user@host Maske von %s wird jetzt bei %s INFO Abrufen angezeigt.
NICK_SASET_HIDE_QUIT_ON
	Die letzte Quit-Nachricht von %s wird jetzt bei %s INFO Abrufen versteckt.
NICK_SASET_HIDE_QUIT_OFF
	Die letzte Quit-Nachricht von %s wird jetzt bei %s INFO Abrufen angezeigt.
NICK_SASET_HIDE_STATUS_ON
	Die Services-Zugriffsberechtigungen von %s werden jetzt bei %s INFO Abrufen versteckt.
NICK_SASET_HIDE_STATUS_OFF
	Die Services-Zugriffsberechtigungen von %s werden jetzt bei %s INFO Abrufen angezeigt.
	
# SASET MSG responses
NICK_SASET_MSG_SYNTAX
	SASET nickname MSG {ON | OFF}
NICK_SASET_MSG_ON
	Die Services werden jetzt %s mit messages antworten.
NICK_SASET_MSG_OFF
	Die Services werden jetzt %s mit notices antworten.

# SET NOEXPIRE responses
NICK_SASET_NOEXPIRE_SYNTAX
	SASET nickname NOEXPIRE {ON | OFF}
NICK_SASET_NOEXPIRE_ON
	Nickname %s wird nicht mehr verfallen.
NICK_SASET_NOEXPIRE_OFF
	Nickname %s wird wieder verfallen.

# ACCESS responses
NICK_SASET_AUTOOP_SYNTAX
	SASET nickname AUTOOP {ON | OFF}
NICK_SASET_AUTOOP_ON
	Die Services werden %s jetzt automatisch Op-Status geben in den Channels.
NICK_SASET_AUTOOP_OFF
	Die Services werden %s jetzt nicht mehr automatisch Op-Status geben in den Channels.

# SASET LANGUAGE responses
NICK_SASET_LANGUAGE_SYNTAX
	SASET nickname LANGUAGE number
NICK_SASET_LANGUAGE_UNKNOWN
	Unbekannte Sprachen-Nummer %d. 
	Tippe %R%s HELP SET LANGUAGE um eine Liste aller verfügbaren Sprachen zu erhalten.
NICK_SASET_LANGUAGE_CHANGED
	Language for %s changed to %s.


# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [Maske]
NICK_ACCESS_ALREADY_PRESENT
	Diese Hostmaske %s ist bereits auf der Access-Liste.
NICK_ACCESS_REACHED_LIMIT
	Du kannst nur %d Einträge auf der Access-Liste speichern.
NICK_ACCESS_ADDED
	%s wurde zu deiner Access-Liste hinzugefügt.
NICK_ACCESS_NOT_FOUND
	%s wurde nicht auf deiner Access-Liste gefunden.
NICK_ACCESS_DELETED
	%s wurde von deiner Access-Liste entfernt.
NICK_ACCESS_LIST
	Access-Liste:
NICK_ACCESS_LIST_X
	Access-Liste von %s:
NICK_ACCESS_LIST_EMPTY
	Deine Access-Liste ist leer.
NICK_ACCESS_LIST_X_EMPTY
	Die Access-Liste für %s leer.

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
	INFO Nickname [ALL]
NICK_INFO_REALNAME
	%s ist %s
NICK_INFO_SERVICES_OPER
	%s ist ein Services Operator.
NICK_INFO_SERVICES_ADMIN
	%s ist ein Services Administrator.
NICK_INFO_SERVICES_ROOT
	%s ist ein Services Root Administrator.
NICK_INFO_ADDRESS
	Letzte gesehene Hostmaske: %s
NICK_INFO_ADDRESS_ONLINE
	          ist online von: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s ist derzeit online.
NICK_INFO_TIME_REGGED
	        Registriert seit: %s
NICK_INFO_LAST_SEEN
	     Letztes Mal gesehen: %s
NICK_INFO_LAST_QUIT
	   Letzte Quit-Nachricht: %s
NICK_INFO_URL
	                     URL: %s
NICK_INFO_EMAIL
	           eMail-Adresse: %s
NICK_INFO_VHOST
	                   vHost: %s
NICK_INFO_VHOST2
	                 vHost: %s@%s
NICK_INFO_ICQ
	                   ICQ #: %d
NICK_INFO_GREET
	          Grussnachricht: %s
NICK_INFO_OPTIONS
	                Optionen: %s
NICK_INFO_EXPIRE
	Wird am %s auslaufen
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Kill-Schutz
NICK_INFO_OPT_SECURE
	Secure-Modus
NICK_INFO_OPT_PRIVATE
	Privat
NICK_INFO_OPT_MSG
	Message-Mode
NICK_INFO_OPT_AUTOOP
	Auto-op
NICK_INFO_OPT_NONE
	Keine
NICK_INFO_NO_EXPIRE
	Dieser Nickname wird nicht auslaufen.
NICK_INFO_FOR_MORE
	Für weitere Informationen, tippe %R%s INFO %s ALL.

# LIST responses
NICK_INFO_SUSPENDED
	Dieser Nickname ist zur Zeit gesperrt, Grund: %s
NICK_INFO_SUSPENDED_NO_REASON
	Dieser Nickname ist zur Zeit gesperrt


# LIST responses
NICK_LIST_SYNTAX
	LIST Muster
NICK_LIST_SERVADMIN_SYNTAX
	LIST Muster [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Liste der gefundenen Einträge %s:
NICK_LIST_RESULTS
	Ende der Liste - %d/%d Treffer angezeigt.

# GLIST responses
NICK_ALIST_SYNTAX
	ALIST Nickname
NICK_ALIST_HEADER
	Die Channels auf die du Zugriff hast:
	  Nummer  Channel              Level     Beschreibung
NICK_ALIST_HEADER_X
	Die Channels auf die %s Zugriff hat:
	  Nummer  Channel              Level     Beschreibung
NICK_ALIST_XOP_FORMAT
	  %3d %c%-20s %-8s %s
NICK_ALIST_ACCESS_FORMAT
	  %3d %c%-20s %-8d %s
NICK_ALIST_FOOTER
	Ende der Liste - %d/%d Treffer angezeigt.

# GLIST responses
NICK_GLIST_HEADER
	Liste der Nicknamen in deiner Gruppe:
NICK_GLIST_HEADER_X
	Liste der Nicknamen in der Gruppe von %s:
NICK_GLIST_FOOTER
	%d Nicknamen in der Gruppe.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (wird am %s auslaufen)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER Nickname [Passwort]
NICK_NO_RECOVER_SELF
	Du kannst dich nicht selber recovern!
NICK_RECOVERED
	Der User, der mit deinem Nicknamen online gewesen ist, wurde entfernt.Tippe:
	%R%s RELEASE %s ein, um ihn vor dem %s-Timeout zurückzubekommen.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE Nickname [Password]
NICK_RELEASE_NOT_HELD
	Der Nickname %s ist nicht gesperrt.
NICK_RELEASED
	Die Services haben den Nicknamen wieder verfügbar gemacht.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST Nickname [Passwort]
NICK_NO_GHOST_SELF
	Du kannst dich doch nicht selber ghosten!
NICK_GHOST_KILLED
	Ghost mit deinem Nicknamen wurde entfernt.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS Nickname
NICK_GETPASS_UNAVAILABLE
	GETPASS nicht verfügbar, da die Verschlüsselung aktiviert ist.
NICK_GETPASS_PASSWORD_IS
	Passwort von %s ist %s.

# SENDPASS responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host Keine WildCards!!
NICK_GETEMAIL_EMAILS_ARE
	Passende Emails %s bis %s.
NICK_GETEMAIL_NOT_USED
	Keine eingetragenen Emails für %s.
	
# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS Nickname
NICK_SENDPASS_UNAVAILABLE
	SENDPASS Befehl nicht verfügbar, da die Verschlüsselung aktiviert ist.
NICK_SENDPASS_SUBJECT
	Nick-Passwort (%s)
NICK_SENDPASS_HEAD
	Hi,
NICK_SENDPASS_LINE_1
	Du wolltest dir dein Passwort von dem Nicknamen %s via eMail schicken lassen.
NICK_SENDPASS_LINE_2
	Das Passwort ist %s Aus Sicherheitsgründen solltest du das Passwort nach Erhalt dieser eMail ändern.
NICK_SENDPASS_LINE_3
	Wenn du nicht weisst, warum du diese Mail erhalten hast, ignoriere sie bitte.
NICK_SENDPASS_LINE_4
	BITTE NICHT AUF DIESE NACHRICHT ANTWORTEN!
NICK_SENDPASS_LINE_5
	%s Administratoren.
NICK_SENDPASS_OK
	Das Passwort von %s wurde per eMail verschickt.

# FORBID responses
NICK_SUSPEND_SYNTAX
	SUSPEND Nickname Grund
NICK_SUSPEND_SUCCEEDED
	Der Nickname %s ist jetzt suspendiert.
NICK_SUSPEND_FAILED
	Konnte den Nicknamen %s nicht suspendieren!

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND Nickname
NICK_UNSUSPEND_SUCCEEDED
	Der Nickname %s ist jetzt freigegeben.
NICK_UNSUSPEND_FAILED
	Konnte den Nicknamen %s nicht freigeben!

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID Nickname [Grund]
NICK_FORBID_SYNTAX_REASON
	FORBID Nickname Grund
NICK_FORBID_SUCCEEDED
	Der Nickname %s ist jetzt verboten.
NICK_FORBID_FAILED
	Konnte den Nicknamen %s nicht verbieten!

# Nick Registration responses
NICK_REQUESTED
	Dieser Nickname wurde bereits angefordert, bitte prüfe deinen Email Account für das Passwort.
NICK_REG_RESENT
	Dein Passcode wurde noch einmal an %s geschickt.
NICK_REG_UNABLE
	Nick konnte NICHT registriert werden, versuche es später noch einmal.
NICK_IS_PREREG
	Dieser Nickname benötigt das Email Bestätigungs Passwort vor Abschluß der Registrierung.
NICK_ENTER_REG_CODE
	Ein Password wurde zu %s gesendet, bitte tippe %R%s confirm <passcode> um die Registrierung abzuschließen.
NICK_CONFIRM_NOT_FOUND
	Registrieungs Stufe 1 ist eventuell abgelaufen, bitte nutze zuerst "%R%s register <password> <email>".
NICK_CONFIRM_INVALID
	Ein ungültiger Passcode wurde eingegeben, prüfe bitte deine Email und gib den Code erneut ein.
NICK_REG_MAIL_SUBJECT
	Nickname Registrierung (%s)
NICK_REG_MAIL_HEAD
	Hi,
NICK_REG_MAIL_LINE_1
	Du hast die Registrierung des Nicknamen %s angefordert.
NICK_REG_MAIL_LINE_2
	Bitte tippe " %R%s confirm %s " zum Abschluß der Registrierung ein.
NICK_REG_MAIL_LINE_3
	Wenn du nicht weisst, warum du diese Mail erhalten hast, ignoriere sie bitte.
NICK_REG_MAIL_LINE_4
	BITTE NICHT AUF DIESE NACHRICHT ANTWORTEN!
NICK_REG_MAIL_LINE_5
	Die %s Administratoren.
NICK_GETPASS_PASSCODE_IS
	Passcode für %s ist %s.
NICK_FORCE_REG
	Nickname %s bestätigt

###########################################################################
#
# ChanServ messages 
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Automatischer Channel-Operator Status
CHAN_LEVEL_AUTOVOICE
	Automatischer Voice-Status
CHAN_LEVEL_AUTOHALFOP
	Automatischer Half-Op Status
CHAN_LEVEL_AUTOPROTECT
	Automatischer Founder-Status
CHAN_LEVEL_AUTODEOP
	Channel-Operator-Status nicht erlaubt
CHAN_LEVEL_NOJOIN
	Der Channel kann nicht betreten werden, da er RESTRICTED ist.
CHAN_LEVEL_INVITE
	INVITE-Befehl erlaubt
CHAN_LEVEL_AKICK
	AKICK-Befehl erlaubt
CHAN_LEVEL_SET
	SET-Befehl erlaubt (nicht FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	CLEAR-Befehl erlaubt
CHAN_LEVEL_UNBAN
	UNBAN-Befehl erlaubt
CHAN_LEVEL_OPDEOP
	OP/DEOP-Befehl erlaubt
CHAN_LEVEL_ACCESS_LIST
	Ansicht der Channel-Access-Liste erlaubt
CHAN_LEVEL_ACCESS_CHANGE
	Änderungen an der Channel-Access-Liste erlaubt
CHAN_LEVEL_MEMO
	Erlaubnis Channel-Memos zu lesen
CHAN_LEVEL_ASSIGN
	Erlaubnis Bots hinzuzufügen / zu entfernen
CHAN_LEVEL_BADWORDS
	BADWORDS-Befehl erlaubt
CHAN_LEVEL_NOKICK
	Wird nicht durch den Bot-Kicker gekicked
CHAN_LEVEL_FANTASIA
	Darf die Befehle des Fantasia-Mode benutzen
CHAN_LEVEL_SAY
	SAY/ACT-Befehl erlaubt
CHAN_LEVEL_GREET
	Grussmeldung wird angezeigt
CHAN_LEVEL_VOICEME
	Erlaubnis sich selbst zu (de)voicen
CHAN_LEVEL_VOICE
	VOICE/DEVOICE-Befehl erlaubt
CHAN_LEVEL_GETKEY
	GETKEY-Befehl erlaubt
CHAN_LEVEL_OPDEOPME
	Erlaubnis sich selbst zu (de)op'en
CHAN_LEVEL_HALFOPME
	Erlaubnis sich selbst zu (de)halfop'en
CHAN_LEVEL_HALFOP
	HALFOP/DEHALFOP-Befehl erlaubt
CHAN_LEVEL_PROTECTME
	Erlaubnis, selber den Schutz ein/auszuschalten
CHAN_LEVEL_PROTECT
	PROTECT-DEPROTECT-Befehl erlaubt
CHAN_LEVEL_KICKME
	Erlaubnis, sich selbst zu kicken
CHAN_LEVEL_KICK
	KICK-Befehl erlaubt
CHAN_LEVEL_SIGNKICK
	Kein signierter Kick wenn SIGNKICK LEVEL genutzt wird

# Automatic responses
CHAN_LEVEL_BANME
	Erlaubnis, sich selbst zu bannen
CHAN_LEVEL_BAN
	Erlaubnis, BAN zu nutzen
CHAN_LEVEL_TOPIC
	Erlaubnis, TOPIC zu nutzen
CHAN_LEVEL_INFO
	Erlaubnis, INFO mit der ALL Option zu nutzen

# Automatic responses
CHAN_IS_REGISTERED
	Dieser Channel wurde registriert von %s.
CHAN_NOT_ALLOWED_OP
	Du hast keine Erlaubnis, den Operator-Status in %s zu benutzen.
CHAN_MAY_NOT_BE_USED
	Dieser Channel kann nicht benutzt werden.
CHAN_NOT_ALLOWED_TO_JOIN
	Dir ist es nicht erlaubt, diesen Channel zu betreten.

# REGISTER responses
CHAN_X_INVALID
	Channel %s ist kein gültiger Channel.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER Channel Passwort Beschreibung
CHAN_REGISTER_DISABLED
	Die Registrierung von Channels ist derzeit deaktiviert.
CHAN_REGISTER_NOT_LOCAL
	Lokale Channels können nicht registriert werden.
CHAN_MUST_REGISTER_NICK
	Du musst erst deinen Nicknamen registrieren.  Tippe %R%s HELP REGISTER um Hilfe zu erhalten.
CHAN_MUST_IDENTIFY_NICK
	Bitte identifiziere dich zuerst durch %s, in dem du diesen Befehl benutzt:
	%R%s IDENTIFY Passwort
CHAN_MAY_NOT_BE_REGISTERED
	Der Channel %s kann nicht registriert werden.
CHAN_ALREADY_REGISTERED
	Der Channel %s ist bereits registriert!
CHAN_MUST_BE_CHANOP
	Du musst Channel-Operator sein um diesen Channel zu registrieren.
CHAN_REACHED_CHANNEL_LIMIT
	Du hast bereits %d Channels auf deinen Nicknamen registriert.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Du hast bereits das Limit von %d registrierten Channels erreicht.
CHAN_REGISTRATION_FAILED
	Registrierung fehlgeschlagen.
CHAN_REGISTERED
	Der Channel %s ist registriert unter deinem Nicknamen: %s
CHAN_PASSWORD_IS
	Dein Channel-Passwort ist %s - behalte es für spätere Benutzung.
CHAN_REGISTER_NONE_CHANNEL
	Du hast versucht ein nicht existierender Channel %s zu registrieren.
CHAN_SYMBOL_REQUIRED
	Verwende bitte nur das Symbol # wenn du versuchst ein
	Channel zu registrieren.

# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY Channel Passwort
CHAN_IDENTIFY_FAILED
	Identifizierung ist fehlgeschlagen.
CHAN_IDENTIFY_SUCCEEDED
	Passwort akzeptiert - du hast jetzt Founderlevel-Zugriff auf %s.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT Channel Nickname
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT Channel [Nickname]
CHAN_LOGOUT_SUCCEEDED
	Der User %s wurde vom Channel %s ausgeloggt.
CHAN_LOGOUT_ALL_SUCCEEDED
	Alle identifizierten User wurden vom Channel ausgeloggt %s (außer dem Channel Founder).
CHAN_LOGOUT_FOUNDER_FAILED
	You may not log out of %s (you are founder).

# DROP responses
CHAN_DROP_SYNTAX
	DROP Channel
CHAN_DROP_DISABLED
	Das Entfernen von Channel-Registrierungen ist derzeit deaktiviert.
CHAN_DROPPED
	Channel %s wurde aus der Datenbank entfernt.

# SET responses
CHAN_SET_SYNTAX
	SET Channel Option Parameter
CHAN_SET_DISABLED
	Channel-Optionen sind derzeit deaktiviert.
CHAN_SET_UNKNOWN_OPTION
	Unbekannte SET-Option %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s hat zu viele Channels auf seinen Nicknamen registriert.
CHAN_FOUNDER_CHANGED
	Founder von %s wurde geändert zu %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Vertreter von %s wurde geändert zu %s.
CHAN_SUCCESSOR_UNSET
	Der Eintrag des Vertreters für %s wurde entfernt.
CHAN_SUCCESSOR_IS_FOUNDER
	%s kann nicht gleichzeitig Founder und Vertreter im Channel %s sein.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Konnte das Passwort nicht ändern.
CHAN_PASSWORD_CHANGED
	%s Passwort wurde geändert.
CHAN_PASSWORD_CHANGED_TO
	%s Passwort wurde geändert zu %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Beschreibung von %s wurde geändert zu %s.

# SET URL responses
CHAN_URL_CHANGED
	URL von %s wurde geändert zu %s.
CHAN_URL_UNSET
	URL von %s wurde entfernt.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	eMail-Adresse von %s wurde geändert in %s.
CHAN_EMAIL_UNSET
	eMail-Adresse von %s wurde entfernt.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Begrüssungsnachricht von %s wurde geändert.
CHAN_ENTRY_MSG_UNSET
	Begrüssungsnachricht von %s wurde entfernt.
	
# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s ist kein gültiger Ban-Typ.
CHAN_SET_BANTYPE_CHANGED
	Ban-Typ für den Channel %s ist jetzt #%d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Unbekanntes Mode-Zeichen: %c wurde ignoriert.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Mode %c wurde ignoriert, da du diesen Mode nicht festsetzen kannst.
CHAN_SET_MLOCK_L_REQUIRED
	Lock Mode +L setzt das Festsetzen des Modes +l vorraus.
CHAN_SET_MLOCK_K_REQUIRED
	Du musst auch Mode +i festsetzen um Mode +K festsetzen zu können.
CHAN_MLOCK_CHANGED
	Mode lock für Channel %s geändert zu %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET Channel KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Die Topic-Beibehaltung für %s ist jetzt aktiviert (ON).
CHAN_SET_KEEPTOPIC_OFF
	Die Topic-Beibehaltung für %s ist jetzt deaktiviert (OFF).

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET Channel TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Topic-Sperre Option für %s ist jetzt ON.
CHAN_SET_TOPICLOCK_OFF
	Topic-Sperre Option für %s ist jetzt OFF.

# SET PRIVATE responses
CHAN_SET_PEACE_SYNTAX
	SET Channel PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Peace Option für %s ist jetzt ON.
CHAN_SET_PEACE_OFF
	Peace Option für %s ist jetzt OFF.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET Channel PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Private Option für %s ist jetzt ON.
CHAN_SET_PRIVATE_OFF
	Private Option für %s ist jetzt OFF.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET Channel SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Secure-Ops Option für %s ist jetzt ON.
CHAN_SET_SECUREOPS_OFF
	Secure-Ops Option für %s ist jetzt OFF.

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET Channel SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Secure Founder Option für %s ist jetzt ON.
CHAN_SET_SECUREFOUNDER_OFF
	Secure Founder Option für %s ist jetzt OFF.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET Channel RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Eingeschränkter Access Option für %s ist jetzt ON.
CHAN_SET_RESTRICTED_OFF
	Eingeschränkter Access Option für %s ist jetzt OFF.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET Channel SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Secure Option für %s ist jetzt ON.
CHAN_SET_SECURE_OFF
	Secure Option für %s ist jetzt OFF.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET Channel SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Signierte kicks Option für %s ist jetzt ON.
CHAN_SET_SIGNKICK_LEVEL
	Signierte kicks Option für %s ist jetzt ON, hängt aber von den
	Level des Benutzers der den Befehl verwendet ab.
CHAN_SET_SIGNKICK_OFF
	Signierte kicks Option für %s ist jetzt OFF.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET Channel OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice Option für %s ist jetzt ON.
CHAN_SET_OPNOTICE_OFF
	Op-notice Option für %s ist jetzt OFF.

# SET NOEXPIRE responses
CHAN_SET_XOP_SYNTAX
	SET Channel XOP {ON | OFF}
CHAN_SET_XOP_ON
	XOP System für %s ist jetzt ON.
CHAN_SET_XOP_OFF
	XOP System für %s ist jetzt OFF.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET channel NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Der Channel %s wird nicht mehr auslaufen.
CHAN_SET_NOEXPIRE_OFF
	Der Channel %s wird nach einer bestimmten Zeit auslaufen.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Du kannst maximal %d AOP/SOP/VOP Einträge in einem Channel haben.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	Du kannst diesen Befehl nicht benutzen. Benutze Stattdessen ACCESS.
	Tippe %R%s HELP ACCESS für weitere Informationen.
CHAN_XOP_NOT_AVAILABLE
	XOP System ist nicht verfügbar.

# AOP messages
CHAN_AOP_SYNTAX
	AOP Channel {ADD|DEL|LIST|CLEAR} [Nickname | entry-liste]
CHAN_AOP_DISABLED
	Die Channel AOP-Liste kann derzeit nicht geändert werden.
CHAN_AOP_NICKS_ONLY
	Die Channel AOP-Liste darf nur registrierte Nicknamen enthalten.
CHAN_AOP_ADDED
	%s wurde der %s AOP-Liste hinzugefügt.
CHAN_AOP_MOVED
	%s wurde auf die %s AOP-Liste verschoben.
CHAN_AOP_NO_SUCH_ENTRY
	Kein solcher Eintrag (#%d) in der %s AOP-Liste.
CHAN_AOP_NOT_FOUND
	%s wurde nicht in der %s AOP-Liste gefunden.
CHAN_AOP_NO_MATCH
	Keine passenden Einträge in der %s AOP-Liste.
CHAN_AOP_DELETED
	%s gelöscht von der %s AOP-Liste.
CHAN_AOP_DELETED_ONE
	1 Eintrag wurde von der %s AOP-Liste gelöscht.
CHAN_AOP_DELETED_SEVERAL
	%d Einträge wurden von der %s AOP-Liste gelöscht.
CHAN_AOP_LIST_EMPTY
	Die AOP-Liste von %s ist leer.
CHAN_AOP_LIST_HEADER
	AOP Liste für %s:
	  Nummer  Nickname
CHAN_AOP_CLEAR
	Die AOP-Liste von %s wurde geleert.

# HOP messages
CHAN_HOP_SYNTAX
	HOP Channel {ADD|DEL|LIST|CLEAR} [Nickname | entry-liste]
CHAN_HOP_DISABLED
	Die Channel HOP-Liste kann derzeit nicht geändert werden.
CHAN_HOP_NICKS_ONLY
	Die Channel HOP-Liste darf nur registrierte Nicknamen enthalten.
CHAN_HOP_ADDED
	%s wurde der %s HOP-Liste hinzugefügt.
CHAN_HOP_MOVED
	%s wurde auf die %s HOP-Liste verschoben.
CHAN_HOP_NO_SUCH_ENTRY
	Kein solcher Eintrag (#%d) in der %s HOP-Liste.
CHAN_HOP_NOT_FOUND
	%s wurde nicht gefunden in der %s HOP-Liste.
CHAN_HOP_NO_MATCH
	Keine passenden Einträge in der %s HOP-Liste.
CHAN_HOP_DELETED
	%s wurde gelöscht von der %s HOP-Liste.
CHAN_HOP_DELETED_ONE
	1 Eintrag wurde von der %s HOP-Liste gelöscht.
CHAN_HOP_DELETED_SEVERAL
	%d Einträge wurden von der %s HOP-Liste gelöscht.
CHAN_HOP_LIST_EMPTY
	Die HOP-Liste von %s ist leer.
CHAN_HOP_LIST_HEADER
	HOP Liste für %s:
	  Nummer  Nickname
CHAN_HOP_CLEAR
	Die %s HOP-Liste wurde geleert.

# SOP messages
CHAN_SOP_SYNTAX
	SOP Channel {ADD|DEL|LIST|CLEAR} [Nickname | entry-liste]
CHAN_SOP_DISABLED
	Die Channel SOP-Liste kann derzeit nicht geändert werden.
CHAN_SOP_NICKS_ONLY
	Die Channel SOP-Liste darf nur registrierte Nicknamen enthalten.
CHAN_SOP_ADDED
	%s wurde der %s SOP-Liste hinzugefügt.
CHAN_SOP_MOVED
	%s wurde auf %s SOP-Liste verschoben.
CHAN_SOP_NO_SUCH_ENTRY
	Kein solcher Eintrag (#%d) in der %s SOP-Liste.
CHAN_SOP_NOT_FOUND
	%s wurde nicht gefunden in der %s SOP-Liste.
CHAN_SOP_NO_MATCH
	Keine passenden Einträge in der %s SOP-Liste.
CHAN_SOP_DELETED
	%s wurde von der %s SOP-Liste gelöscht.
CHAN_SOP_DELETED_ONE
	1 Eintrag wurde von der %s SOP-Liste gelöscht.
CHAN_SOP_DELETED_SEVERAL
	%d Einträge wurden von der %s SOP-Liste gelöscht.
CHAN_SOP_LIST_EMPTY
	Die SOP-Liste von %s ist leer.
CHAN_SOP_LIST_HEADER
	SOP Liste für %s:
	  Nummer  Nickname
CHAN_SOP_CLEAR
	Die SOP-Liste von %s wurde geleert.

# VOP messages
CHAN_VOP_SYNTAX
	VOP Channel {ADD|DEL|LIST|CLEAR} [Nickname | entry-liste]
CHAN_VOP_DISABLED
	Die Channel VOP-Liste kann derzeit nicht geändert werden.
CHAN_VOP_NICKS_ONLY
	Die Channel VOP-Liste darf nur registrierte Nicknamen enthalten.
CHAN_VOP_ADDED
	%s wurde der %s VOP-Liste hinzugefügt.
CHAN_VOP_MOVED
	%s wurde auf die %s VOP-Liste verschoben.
CHAN_VOP_NO_SUCH_ENTRY
	Kein solcher Eintrag (#%d) in der %s VOP-Liste.
CHAN_VOP_NOT_FOUND
	%s wurde nicht gefunden in der %s VOP-Liste.
CHAN_VOP_NO_MATCH
	Keine passenden Einträge in der %s VOP-Liste.
CHAN_VOP_DELETED
	%s wurde gelöscht von der %s VOP-Liste.
CHAN_VOP_DELETED_ONE
	1 Eintrag wurde von der %s VOP-Liste gelöscht.
CHAN_VOP_DELETED_SEVERAL
	%d Einträge wurden von der %s VOP-Liste gelöscht.
CHAN_VOP_LIST_EMPTY
	Die VOP-Liste von %s ist leer.
CHAN_VOP_LIST_HEADER
	VOP Liste für %s:
	  Nummer  Nickname
CHAN_VOP_CLEAR
	Die VOP-Liste von %s wurde geleert.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS Channel {ADD|DEL|LIST|CLEAR} [Nickname [Level] | entry-liste]
CHAN_ACCESS_XOP
	Du kannst dieses Befehl nicht nutzen. 
	Nutze stattdessen die AOP, SOP und VOP Befehle.
	Tippe %R%s HELP Befehl für weitere Informationen.
CHAN_ACCESS_XOP_HOP
	Du kannst dieses Befehl nicht nutzen.
	Nutze stattdessen die AOP, SOP, HOP und VOP Befehle.
	Tippe %R%s HELP Befehl für weitere Informationen.
CHAN_ACCESS_DISABLED
	Das Ändern der Channel-Access-Liste wurde zwischenzeitig deaktiviert.
CHAN_ACCESS_LEVEL_NONZERO
	Access-Level muss grösser als 0 sein.
CHAN_ACCESS_LEVEL_RANGE
	Access-Level muss zwischen %d und einschliesslich %d liegen.
CHAN_ACCESS_NICKS_ONLY
	Die Channel-Access-Liste darf nur registrierte Nicknamen enthalten.
CHAN_ACCESS_REACHED_LIMIT
	Die Access-Liste ist auf %d Einträge beschränkt.
CHAN_ACCESS_LEVEL_UNCHANGED
	Access-Level für %s in %s ist unverändert auf %d.
CHAN_ACCESS_LEVEL_CHANGED
	Access-Level für %s in %s wurde geändert auf %d.
CHAN_ACCESS_ADDED
	%s wurde zur Access-Liste von %s hinzugefügt mit Level %d.
CHAN_ACCESS_NO_SUCH_ENTRY
	Kein solcher Eintrag (#%d) auf der Access-Liste von %s.
CHAN_ACCESS_NOT_FOUND
	%s wurde auf der Access-Liste von %s nicht gefunden.
CHAN_ACCESS_NO_MATCH
	Es existieren keine entsprechenden Einträge in der Access-Liste von %s.
CHAN_ACCESS_DELETED
	%s wurde von der Access-Liste von %s entfernt.
CHAN_ACCESS_DELETED_ONE
	1 Eintrag wurde aus der Access-Liste von %s entfernt.
CHAN_ACCESS_DELETED_SEVERAL
	%d Einträge wurden aus der Access-Liste von %s entfernt.
CHAN_ACCESS_LIST_EMPTY
	Die Access-Liste von %s ist leer.
CHAN_ACCESS_LIST_HEADER
	Access-Liste für %s:
	  Nummer   Level  Nickname
CHAN_ACCESS_LIST_FOOTER
	Ende der Access-liste.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	Die Access-Liste von %s wurde geleert.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK Channel {ADD | DEL | LIST | VIEW | ENFORCE | CLEAR} [Nickname/Usermaske]
CHAN_AKICK_DISABLED
	Das Bearbeiten der AutoKick-Liste wurde zwischenzeitlich deaktiviert.
CHAN_AKICK_ALREADY_EXISTS
	%s existiert bereits auf der AutoKick-Liste von %s.
CHAN_AKICK_REACHED_LIMIT
	Du kannst nur max. %d Einträge auf der AutoKick-Liste haben.
CHAN_AKICK_ADDED
	%s wurde der AutoKick-Liste von %s hinzugefügt.
CHAN_AKICK_NO_SUCH_ENTRY
	Kein solcher Eintrag (#%d) auf der Autokick-Liste von %s gefunden.
CHAN_AKICK_NOT_FOUND
	%s wurde nicht auf der AutoKick-Liste von %s gefunden.
CHAN_AKICK_NO_MATCH
	Keine entsprechenden Einträge auf der AutoKick-Liste von %s gefunden.
CHAN_AKICK_STUCK
	%s ist jetzt stetig aktiviert im %s.
CHAN_AKICK_UNSTUCK
	%s ist jetzt nicht mehr stetig aktiviert im %s.
CHAN_AKICK_DELETED
	%s wurde von der Autokick-Liste von %s entfernt.
CHAN_AKICK_DELETED_ONE
	1 Eintrag der AutoKick-Liste von %s entfernt.
CHAN_AKICK_DELETED_SEVERAL
	%d Einträge wurden aus der AutoKick-Liste von %s entfernt.
CHAN_AKICK_LIST_EMPTY
	Die Autokick-Liste von %s ist leer.
CHAN_AKICK_LIST_HEADER
	Autokick-Liste von %s:
CHAN_AKICK_LIST_FORMAT
	  %3d %s%s
CHAN_AKICK_VIEW_FORMAT
	%3d %s (von %s am %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (stuck) (von %s am %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE für %s erledigt. %d User betroffen.
CHAN_AKICK_CLEAR
	Die Autokick-Liste von %s wurde geleert.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS Channel {SET | DIS[ABLE] | LIST | RESET} [item [Level]]
CHAN_LEVELS_XOP
	Der Befehl ist in diesem Modus nutzlos - Deaktiviere zuerst XOP.
CHAN_LEVELS_RANGE
	Der Level kann nur zwischen %d und einschliesslich %d liegen.
CHAN_LEVELS_CHANGED
	Der Level für %s für den Channel %s wurde geändert auf %d.
CHAN_LEVELS_UNKNOWN
	Einstellung %s ist unbekannt.  Tippe %R%s HELP LEVELS DESC für eine ausführliche Einstellungsliste.
CHAN_LEVELS_DISABLED
	%s wurde deaktiviert im Channel %s.
CHAN_LEVELS_LIST_HEADER
	Zugriffslevel für Channel %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (deaktiviert)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (nur Founder)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	Zugriffslevel für %s wurden auf Standardwerte zurückgesetzt.

# Status Messages
CHAN_STATUS_SYNTAX
	STATUS ERROR Syntax error
CHAN_STATUS_NOT_REGGED
	STATUS ERROR Channel %s ist nicht registriert
CHAN_STATUS_FORBIDDEN
	STATUS ERROR Channel %s ist verboten
CHAN_STATUS_NOTONLINE
	STATUS ERROR Nickname %s ist nicht online
CHAN_STATUS_INFO
	STATUS %s %s %d

# INFO responses
CHAN_INFO_SYNTAX
	INFO Channel [ALL]
CHAN_INFO_HEADER
	Informationen über den Channel %s:
CHAN_INFO_FOUNDER
	           Founder: %s (%s)
CHAN_INFO_NO_FOUNDER
	           Founder: %s
CHAN_INFO_SUCCESSOR
	         Vertreter: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	         Vertreter: %s
CHAN_INFO_DESCRIPTION
	      Beschreibung: %s
CHAN_INFO_ENTRYMSG
	Eintrittsnachricht: %s
CHAN_INFO_TIME_REGGED
	       Registriert: %s
CHAN_INFO_LAST_USED
	   Zuletzt benutzt: %s
CHAN_INFO_LAST_TOPIC
	     Letztes Topic: %s
CHAN_INFO_TOPIC_SET_BY
	 Topic gesetzt von: %s
CHAN_INFO_URL
	               URL: %s
CHAN_INFO_EMAIL
	     eMail-Adresse: %s
CHAN_INFO_BANTYPE
	           Ban-Typ: %d
CHAN_INFO_OPTIONS
	          Optionen: %s
CHAN_INFO_OPT_KEEPTOPIC
	Topic-Wiederherstellung
CHAN_INFO_OPT_OPNOTICE
	OP Notice
CHAN_INFO_OPT_PEACE
	Peace
CHAN_INFO_OPT_PRIVATE
	Privat
CHAN_INFO_OPT_RESTRICTED
	Eingeschränkter Access
CHAN_INFO_OPT_SECURE
	Secure
CHAN_INFO_OPT_SECUREOPS
	Secure Ops
CHAN_INFO_OPT_SECUREFOUNDER
	Secure Founder
CHAN_INFO_OPT_SIGNKICK
	Signierte kicks
CHAN_INFO_OPT_TOPICLOCK
	Topic-Sperre
CHAN_INFO_OPT_XOP
	xOP Listen-System
CHAN_INFO_OPT_NONE
	Keine
CHAN_INFO_MODE_LOCK
	        Mode lock: %s
CHAN_INFO_EXPIRE
	      Wird am %s auslaufen.
CHAN_INFO_NO_EXPIRE
	Dieser Channel wird nicht auslaufen.

# LIST responses
CHAN_LIST_SYNTAX
	LIST Muster
CHAN_LIST_SERVADMIN_SYNTAX
	LIST Muster [FORBIDDEN] [NOEXPIRE]
CHAN_LIST_HEADER
	Liste passender Einträge zu %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Ende der Liste - %d/%d Treffer gezeigt.
	
# CLIST responses
CHAN_INVITE_SYNTAX
	INVITE Channel

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN Channel
CHAN_UNBANNED
	Dein Ban in %s wurde entfernt.

# CLEAR responses
CHAN_TOPIC_SYNTAX
	TOPIC Channel [Topic]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR Channel was
CHAN_CLEARED_BANS
	Alle Bans in %s wurden entfernt.
CHAN_CLEARED_EXCEPTS
	Alle Ausnahmen in %s wurden entfernt.
CHAN_CLEARED_MODES
	Alle Channel-Modes von %s wurden zurückgesetzt.
CHAN_CLEARED_OPS
	Alle Op-Flags wurden im Channel %s entfernt.
CHAN_CLEARED_HOPS
	Alle HalfOp-Flags wurden im Channel %s entfernt.
CHAN_CLEARED_VOICES
	Alle Voice-Flags wurden im Channel %s entfernt.
CHAN_CLEARED_USERS
	Alle User wurden aus Channel %s gekicked.
CHAN_CLEARED_INVITES
	Alle Invites wurden im Channel %s entfernt.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS Channel
CHAN_GETPASS_UNAVAILABLE
	GETPASS Befehl nicht verfügbar, da die Datenbank verschlüsselt ist.
CHAN_GETPASS_PASSWORD_IS
	Passwort für Channel %s ist %s.
	
# SENDPASS responses
CHAN_GETKEY_SYNTAX
	GETKEY Channel
CHAN_GETKEY_NOKEY
	Der Channel %s hat kein Passwort.
CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS Channel
CHAN_SENDPASS_UNAVAILABLE
	SENDPASS Befehl nicht verfügbar, da die Datenbank verschlüsselt ist.
CHAN_SENDPASS_SUBJECT
	Channel-Passwort (%s)
CHAN_SENDPASS_HEAD
	Hi,
CHAN_SENDPASS_LINE_1
	Du hast das Passwortes für deinen Channel %s angefordert.
CHAN_SENDPASS_LINE_2
	Das Passwort ist %s. Aus Sicherheitsgründen solltest du das Passwort bei Erhalt dieser Nachricht ändern.
CHAN_SENDPASS_LINE_3
	Wenn du nicht weisst, warum du diese Mail empfangen hast, ignoriere Sie bitte.
CHAN_SENDPASS_LINE_4
	AUF DIESE MAIL BITTE NICHT ANTWORTEN!
CHAN_SENDPASS_LINE_5
	%s, die Administration.
CHAN_SENDPASS_OK
	Passwort von %s wurde per eMail versandt.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID Channel [Grund]
CHAN_FORBID_SYNTAX_REASON
	FORBID Channel Grund
CHAN_FORBID_SUCCEEDED
	Der Channel %s hat jetzt den Status "verboten".
CHAN_FORBID_FAILED
	Konnte den Status "verboten" bei %s nicht setzen!

# Misc responses
CHAN_FORBID_REASON
	Dieser Channel wurde verboten.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND Channel [Grund]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND Channel Grund
CHAN_SUSPEND_SUCCEEDED
	Der Channel %s ist jetzt auf SUSPENDED gesetzt.
CHAN_SUSPEND_FAILED
	SUSPEND konnte in Channel %s nicht aktiviert werden!
CHAN_SUSPEND_REASON
	Dieser Channel wurde auf SUSPENDED gesetzt.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND Channel
CHAN_UNSUSPEND_ERROR
	Kein # vor dem Channelnamen gefunden.
CHAN_UNSUSPEND_SUCCEEDED
	Der Channel %s ist jetzt nicht mehr auf SUSPENDED gesetzt.
CHAN_UNSUSPEND_FAILED
	Der Channel %s konnte nicht auf UNSUSPENED gesetzt werden!

# Misc responses
CHAN_EXCEPTED
	%s passt zur Ausnahme von %s und kann nicht gebannt werden, solange die Ausnahme nicht gelöscht wird.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	Du hast eine neue Memo.
MEMO_HAVE_NEW_MEMOS
	Du hast %d neue Memos.
MEMO_TYPE_READ_LAST
	Tippe %R%s READ LAST um sie zu lesen.
MEMO_TYPE_READ_NUM
	Tippe %R%s READ %d um sie zu lesen.
MEMO_TYPE_LIST_NEW
	Tippe %R%s LIST NEW um eine Auflistung deiner neuen Memos zu erhalten.
MEMO_AT_LIMIT
	Warnung: Du hast die maximale Anzahl gespeicherter Memos erreicht (%d). Du kannst keine weiteren Nachrichten empfangen, bis du einige deiner jetzigen Memos gelöscht hast.
MEMO_OVER_LIMIT
	Warnung: Du hast die maximale Anzahl gespeicherter Memos überschritten (%d). Du kannst keine weiteren Memos empfangen, bis du einige deiner jetzigen Memos gelöscht hast.
MEMO_X_MANY_NOTICE
	Es gibt %d benachrichtigungen für den Channel %s.
MEMO_X_ONE_NOTICE
	Es gibt %d Benachrichtigung für Channel %s.
MEMO_NEW_X_MEMO_ARRIVED
	Es gibt eine neue Nachricht für Channel %s.
	Tippe %R%s READ %s %d um sie zu lesen.
MEMO_NEW_MEMO_ARRIVED
	Du hast eine neue Nachricht von %s empfangen.
	Tippe %R%s READ %d um sie zu lesen.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Du hast keine Memos.
MEMO_X_HAS_NO_MEMOS
	%s hat keine Memos.
MEMO_DOES_NOT_EXIST
	Memo %d existiert nicht!
MEMO_LIST_NOT_FOUND
	Keine entsprechenden Memos gefunden.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {Nickname | Channel} Memo-text
MEMO_SEND_DISABLED
	Der Memo-Versand wurde kurzzeitig deaktiviert.
MEMO_SEND_PLEASE_WAIT
	Bitte warte %d Sekunden bis du den SEND Befehl wieder benutzen kannst.
MEMO_X_GETS_NO_MEMOS
	%s kann keine Memos empfangen.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s hat zu viele Memos gespeichert und kann keine weiteren empfangen.
MEMO_SENT
	Memo wurde an %s versandt.
	
# CANCEL responses
MEMO_MASS_SENT
	Ein Massen-Memo wurde an alle regsitrierte User gesendet.
	
# STAFF responses
MEMO_STAFF_SYNTAX
	STAFF memo-text
	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {Nickname | Channel}
MEMO_CANCEL_DISABLED
	Das Widerrufen von Memos wurde deaktiviert.
MEMO_CANCEL_NONE
	Kein Memo konnte widerrufen werden.
MEMO_CANCELLED
	Die letzte Memo an %s wurde widerrufen.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [Channel] [Liste | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	Du hast keine neuen Memos.
MEMO_X_HAS_NO_NEW_MEMOS
	%s hat keine Memos.
MEMO_LIST_MEMOS
	Memos für %s.  Um sie zu lesen, tippe %R%s READ Nummer
MEMO_LIST_NEW_MEMOS
	Neue Memos für %s.  Um sie zu lesen, tippe %R%s READ Nummer
MEMO_LIST_CHAN_MEMOS
	Memos für %s.  Um sie zu lesen, tippe %R%s READ %s Nummer
MEMO_LIST_CHAN_NEW_MEMOS
	Neue Memos für %s.  Um sie zu lesen, tippe %R%s READ %s Nummer
MEMO_LIST_HEADER
	 Nummer  Sender            Datum/Zeit
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [Channel] {Liste | LAST | NEW}
MEMO_HEADER
	Memo %d von %s (%s).  Um sie zu löschen, tippe %R%s DEL %d
MEMO_CHAN_HEADER
	Memo %d von %s (%s).  Um sie zu löschen, tippe %R%s DEL %s %d
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [Channel] {Nummer | Liste | ALL}
MEMO_DELETED_NONE
	Es wurden keine Memos gelöscht.
MEMO_DELETED_ONE
	Memo %d wurde gelöscht.
MEMO_DELETED_SEVERAL
	Memos %s wurden gelöscht.
MEMO_DELETED_ALL
	Alle deine Memos wurden gelöscht.
MEMO_CHAN_DELETED_ALL
	Alle Memos für den Channel %s wurden gelöscht.

# SET responses
MEMO_SET_SYNTAX
	SET Option Parameter
MEMO_SET_DISABLED
	MEMO SET Befehl wurde kurzzeitig deaktiviert.
MEMO_SET_UNKNOWN_OPTION
	Unbekannte SET Option %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
MEMO_SET_NOTIFY_ON
	%s wird dich beim Einloggen über eingegangene Memos informieren. 
MEMO_SET_NOTIFY_LOGON
	%s wird dich beim Einloggen oder bei der Rückkehr aus /AWAY über eingegangene Memos informieren.
MEMO_SET_NOTIFY_NEW
	%s wird dich benachrichtigen, sobald neue Memos für dich eintreffen.
MEMO_SET_NOTIFY_OFF
	%s wird dich nicht länger beim Eintreffen neuer Memos benachrichtigen.
MEMO_SET_NOTIFY_MAIL
	Du wirst jetzt per Email über neue Memos informiert.
MEMO_SET_NOTIFY_NOMAIL
	Du wirst nicht länger per Email über neue Memos benachrichtigt.
MEMO_SET_NOTIFY_INVALIDMAIL
	Du hast keine zulässige Email Adresse gesetzt.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [Channel] Limit
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [User | Channel] {Limit | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Du hast keine Berechtigung dein Memo-Limit zu ändern.
MEMO_SET_LIMIT_FORBIDDEN
	Du hast keine Berechtigung das Memo-Limit für %s zu ändern.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Du kannst dein Memo-Limit nicht höher als %d setzen.
MEMO_SET_LIMIT_TOO_HIGH
	Du kannst das Memo-Limit für %s nicht höher als %d setzen.
MEMO_SET_LIMIT_OVERFLOW
	Das Memo-Limit ist zu gross, es wird reduziert auf %d.
MEMO_SET_YOUR_LIMIT
	Dein Memo-Limit wurde auf %d gesetzt.
MEMO_SET_YOUR_LIMIT_ZERO
	Du wirst ab sofort keine Memos mehr empfangen können.
MEMO_UNSET_YOUR_LIMIT
	Dein Memo-Limit wurde aufgehoben.
MEMO_SET_LIMIT
	Memo-Limit von %s wurde auf %d gesetzt.
MEMO_SET_LIMIT_ZERO
	Memo-Limit von %s wurde auf 0 gesetzt, kein Memo-Empfang für ihn mehr möglich.
MEMO_UNSET_LIMIT
	Memo-Limit von %s wurde aufgehoben.

# INFO responses
MEMO_INFO_SYNTAX
	INFO [Channel]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [Nickname | Channel]
MEMO_INFO_NO_MEMOS
	Du hast keine Memos.
MEMO_INFO_MEMO
	Du hast eine neue Memo.
MEMO_INFO_MEMO_UNREAD
	Du hast derzeit eine ungelesene Memo.
MEMO_INFO_MEMOS
	Du hast %d Memos.
MEMO_INFO_MEMOS_ONE_UNREAD
	Du hast insgesamt %d Memos, davon ist eine ungelesen.
MEMO_INFO_MEMOS_SOME_UNREAD
	Du hast insgesamt %d Memos, davon sind %d ungelesen.
MEMO_INFO_MEMOS_ALL_UNREAD
	Du hast insgesamt %d Memos, alle davon sind ungelesen.
MEMO_INFO_LIMIT
	Dein Memo-Limit liegt bei %d.
MEMO_INFO_HARD_LIMIT
	Dein Memo-Limit liegt bei %d, und kann nicht geändert werden.
MEMO_INFO_LIMIT_ZERO
	Dein Memo-Limit liegt bei 0, du kannst keine Memos empfangen.
MEMO_INFO_HARD_LIMIT_ZERO
	Dein Memo-Limit liegt bei 0, du kannst keine Memos empfangen. Das Limit kann von dir nicht verändert werden.
MEMO_INFO_NO_LIMIT
	Du hast kein Limit in der Anzahl deiner Memos.
MEMO_INFO_NOTIFY_OFF
	Du wirst nicht benachrichtigt bei neuen Memos.
MEMO_INFO_NOTIFY_ON
	Du wirst bei neuen Memos benachrichtigt, wenn du dich identifizierst bzw wenn sie eintreffen.
MEMO_INFO_NOTIFY_RECEIVE
	Du wirst bei neuen Memos benachrichtigt, wenn sie eintreffen.
MEMO_INFO_NOTIFY_SIGNON
	Du wirst bei neuen Memos benachrichtigt, wenn du dich identifizierst.
MEMO_INFO_X_NO_MEMOS
	%s hat derzeit keine Memos.
MEMO_INFO_X_MEMO
	%s hat derzeit eine Memo.
MEMO_INFO_X_MEMO_UNREAD
	%s hat derzeit eine ungelesene Memo.
MEMO_INFO_X_MEMOS
	%s hat derzeit %d Memos.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s hat derzeit %d Memos, von denen eine ungelesen ist.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s hat derzeit %d Memos, von denen %d ungelesen sind.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s hat derzeit %d Memos, von denen alle ungelesen sind.
MEMO_INFO_X_LIMIT
	%s's Memo-Limit liegt bei %d.
MEMO_INFO_X_HARD_LIMIT
	%s's Memo-Limit liegt bei %d, und kann nicht geändert werden.
MEMO_INFO_X_NO_LIMIT
	%s hat kein Memo-Limit.
MEMO_INFO_X_NOTIFY_OFF
	%s wird bei neuen Memos nicht benachrichtigt.
MEMO_INFO_X_NOTIFY_ON
	%s wird beim Logon und bei Ankunft neuer Memos benachrichtigt.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s wird bei Ankunft neuer Memos benachrichtigt.
MEMO_INFO_X_NOTIFY_SIGNON
	%s wird beim Logon über neuer Memos benachrichtigt.

# Memo2Mail
MEMO_MAIL_SUBJECT
	Neue Memo
MEMO_MAIL_TEXT1
	Hi %s
MEMO_MAIL_TEXT2
	Du hast gerade eine neue Memo von %s erhalten. Dies ist Memo Nummer %d.
MEMO_MAIL_TEXT3
	Memo Text:
	
# Standard responses
MEMO_RSEND_PLEASE_WAIT
	Bitte warte %d Sekunden bevor der RSEND Befehl wieder funktioniert.
MEMO_RSEND_DISABLED
	RSEND ist auf diesem Netzwerk ausgeschaltet.
MEMO_RSEND_SYNTAX
	RSEND {Nickname | Channel} Memo-text
MEMO_RSEND_NICK_MEMO_TEXT
	[auto-memo] Die Memo die du gesendet hast wurde angesehen.
MEMO_RSEND_CHAN_MEMO_TEXT
	[auto-memo] Die Memo die du zum %s gesendet hast wurde angesehen.
MEMO_RSEND_USER_NOTIFICATION
	Eine Benachrichtigungs-Memo wurde zum %s gesendet um ihn/sie zu
	informieren das du seine/ihre Memo gelesen hast.
	
# Standard responses
MEMO_CHECK_SYNTAX
	CHECK Nickname
MEMO_CHECK_NOT_READ
	Die letzte Memo die du zu %s gesendet hast (gesendet am %s) wurde nicht gelesen.
MEMO_CHECK_READ
	Die letzte Memo die du zu %s gesendet hast (gesendet am %s) wurde gelesen.
MEMO_CHECK_NO_MEMO
	Nickname %s hat keine Memo von dir.

# Standard responses
MEMO_NO_RSEND_SELF
	Das Erhalten von Empfangsbestätigungen ist nur beim Senden
	einer Memo an andere User möglich.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
BOT_DOES_NOT_EXIST
	Bot %s gibt es nicht.
BOT_NOT_ASSIGNED
	Ein Bot muss dem Channel zugewiesen werden, bevor dieser Befehl funktioniert.
	Tippe %R%S HELP ASSIGN für weitere Informationen.
BOT_NOT_ON_CHANNEL
	Der Bot ist nicht im Channel %s.
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	Benutze nicht das Wort %s in diesem Channel!
BOT_REASON_BADWORD_GENTLE
	Pass bitte auf was du sagst!
BOT_REASON_BOLD
	Im Channel ist keine fettgedruckte-Schrift erlaubt!
BOT_REASON_CAPS
	Schalte Caps-Lock aus!
BOT_REASON_COLOR
	Keine Farben im Channel benutzen!
BOT_REASON_FLOOD
	Hör auf zu flooden!
BOT_REASON_REPEAT
	Hör auf dich dauernd zu wiederholen!
BOT_REASON_REVERSE
	Nicht invertiert schreiben!
BOT_REASON_UNDERLINE
	Keine Unterstreichungen in diesem Channel benutzen!
	
# !seen replies
BOT_SEEN_BOT
	Hey, du hast mich gefunden %s!
BOT_SEEN_YOU
	Wieder auf der Suche nach dir selbst, %s?
BOT_SEEN_ON_CHANNEL
	%s ist doch im Channel!
BOT_SEEN_ON_CHANNEL_AS
	%s ist derzeit im Channel (als %s)! 
BOT_SEEN_ON
	%s wurde zuletzt vor %s gesehen.
BOT_SEEN_NEVER
	%s hab ich noch nie gesehen.
BOT_SEEN_UNKNOWN
	Ich kenne %s nicht.

# BOT responses
BOT_BOT_SYNTAX
	BOT ADD Botnick User Hostmaske Realname
	BOT CHANGE Botnick Neuer-Botnick [User [Hostmaske [Realname]]]
	BOT DEL Nickname
BOT_BOT_ALREADY_EXISTS
	Bot %s existiert bereits.
BOT_BOT_CREATION_FAILED
	Erstellung des Bots ist fehlgeschlagen.
BOT_BOT_READONLY
	Das Verändern von Bot-Einstellungen ist derzeit deaktiviert.
BOT_BOT_ADDED
	%s!%s@%s (%s) wurde zur der Bot-Liste hinzugefügt.
BOT_BOT_ANY_CHANGES
	Das alten Informationen stimmen mit den neuen überein.
BOT_BOT_CHANGED
	Bot %s wurde geändert auf %s!%s@%s (%s)
BOT_BOT_DELETED
	Bot %s wurde gelöscht.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Bot-Liste:
BOT_BOTLIST_PRIVATE_HEADER
	Bots die für IRC Operatoren reserviert sind:
BOT_BOTLIST_FOOTER
	%d Bots verfügbar.
BOT_BOTLIST_EMPTY
	Es sind derzeit keine Bots verfügbar. Frag einen 
	Services Administrator, ob er einen erstellt.
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN Channel Nickname
BOT_ASSIGN_READONLY
	Das Zuweisen von Bots ist derzeit deaktiviert.
BOT_ASSIGN_ALREADY
	Bot %s ist bereits dem Channel %s zugewiesen.
BOT_ASSIGN_ASSIGNED
	Bot %s wurde dem Channel %s zugewiesen.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN channel
BOT_UNASSIGN_UNASSIGNED
	Der Bot von %s wurde entfernt.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {Channel | Nickname}
BOT_INFO_NOT_FOUND
	%s ist kein gültiger Bot bzw. registrierter Channel.
BOT_INFO_BOT_HEADER
	Information vom Bot %s:
BOT_INFO_BOT_MASK
	     Hostmaske: %s@%s
BOT_INFO_BOT_REALNAME
	     Echter Name: %s
BOT_INFO_BOT_CREATED
	     Erstellt: %s
BOT_INFO_BOT_USAGE
	     benutzt in: %d Channel(s)
BOT_INFO_BOT_OPTIONS
	     Options : %s
BOT_INFO_OPT_PRIVATE
	     Private
BOT_INFO_CHAN_HEADER
	Information von Channel %s:
BOT_INFO_CHAN_BOT
	           Botnick : %s
BOT_INFO_CHAN_BOT_NONE
	           Botnick : noch nicht zugewiesen.
BOT_INFO_CHAN_KICK_BADWORDS
	   Bad words kicker : %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	   Bad words kicker : %s (%d Kick(s) bis zum Ban)
BOT_INFO_CHAN_KICK_BOLDS
	       Bolds kicker : %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	       Bolds kicker : %s (%d Kick(s) bis zum Ban)
BOT_INFO_CHAN_KICK_CAPS_ON
	        Caps kicker : %s (Minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	        Caps kicker : %s (%d Kick(s) bis zum Ban, Minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	        Caps kicker : %s
BOT_INFO_CHAN_KICK_COLORS
	      Colors kicker : %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	      Colors kicker : %s (%d Kick(s) bis zum Ban)
BOT_INFO_CHAN_KICK_FLOOD_ON
	       Flood kicker : %s (%d Zeilen in %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	       Flood kicker : %s (%d Kick(s) bis zum Ban, %d Zeilen in %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	       Flood kicker : %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	      Repeat kicker : %s (%d Mal)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	      Repeat kicker : %s (%d Kick(s) bis zum Ban, %d Mal)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	      Repeat kicker : %s
BOT_INFO_CHAN_KICK_REVERSES
	    Reverses kicker : %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	    Reverses kicker : %s (%d Kick(s) bis zum Ban)
BOT_INFO_CHAN_KICK_UNDERLINES
	  Underlines kicker : %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	  Underlines kicker : %s (%d Kick(s) bis zum Ban)
BOT_INFO_ACTIVE
	aktiv
BOT_INFO_INACTIVE
	nicht aktiv
BOT_INFO_CHAN_OPTIONS
	           Optionen : %s
BOT_INFO_OPT_DONTKICKOPS
	Op-Schutz
BOT_INFO_OPT_DONTKICKVOICES
	Voice-Schutz
BOT_INFO_OPT_FANTASY
	Fantasy-Mode
BOT_INFO_OPT_GREET
	Begrüssungsnachricht
BOT_INFO_OPT_NOBOT
	Kein Bot
BOT_INFO_OPT_SYMBIOSIS
	Symbiosis
BOT_INFO_OPT_NONE
	Keine
	
# SET responses
BOT_SET_SYNTAX
	SET (Channel | Bot) Option Einstellungen
BOT_SET_DISABLED
	Die SET-Optionen sind derzeit deaktiviert.
BOT_SET_UNKNOWN
	Unbekannte Option %s.
	Tippe %R%S HELP SET für weitere Informationen.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET Channel DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Bot wird keine Ops kicken im %s.
BOT_SET_DONTKICKOPS_OFF
	Bot wird Ops kicken im %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET Channel DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Bot wird keine Voices kicken im %s.
BOT_SET_DONTKICKVOICES_OFF
	Bot wird Voices kicken im %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET Channel FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Fantasy mode ist jetzt ON im %s.
BOT_SET_FANTASY_OFF
	Fantasy mode ist jetzt OFF im %s.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET Channel GREET {ON|OFF}
BOT_SET_GREET_ON
	Begrüssungsnachricht ist jetzt ON im %s.
BOT_SET_GREET_OFF
	Begrüssungsnachricht ist jetzt OFF im %s.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET Channel NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	No-Bot-Mode ist jetzt ON im Channel %s.
BOT_SET_NOBOT_OFF
	No-Bot-Mode ist jetzt OFF im Channel %s.
	
# SET SYMBIOSIS
BOT_SET_PRIVATE_SYNTAX
	SET Botname PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Private-Mode des Bots im %s ist jetzt ON.
BOT_SET_PRIVATE_OFF
	Private-Mode des Bots im %s ist jetzt OFF.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET Channel SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Symbiosis-Mode ist jetzt ON im %s.
BOT_SET_SYMBIOSIS_OFF
	Symbiosis-Mode ist jetzt OFF im %s.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK Channel Option {ON|OFF} [Einstellungen]
BOT_KICK_DISABLED
	Die Konfiguration des Kickers ist derzeit deaktiviert.
BOT_KICK_UNKNOWN
	Unbekannte Option %s.
	Tippe %R%S HELP KICK für weitere Informationen.
BOT_KICK_BAD_TTB
	%s ist eine ungültige Ban-Zeit.

BOT_KICK_BADWORDS_ON
	Bot wird jetzt auf bad words achten und kicken. Benutze den BADWORDS
	Befehl um Wörter zur Liste hinzuzufügen oder zu entfernen.

BOT_KICK_BADWORDS_ON_BAN
	Bot wird jetzt auf bad words achten und einen Ban setzen
	nach %d kicks. Benutze den BADWORDS Befehl um Wörter zur Liste
	hinzuzufügen oder zu entfernen.

BOT_KICK_BADWORDS_OFF
	bad word Kick ist jetzt deaktiviert.
BOT_KICK_BOLDS_ON
	Bot kickt jetzt Leute, die fett schreiben.
BOT_KICK_BOLDS_ON_BAN
	Bot kickt jetzt Leute, die fett schreiben und setzt einen Ban nach
	%d Kicks des Users.
BOT_KICK_BOLDS_OFF
	bold Kick ist jetzt deaktiviert.
BOT_KICK_CAPS_ON
	Bot wird jetzt User mit GROSSSCHRIFT kicken (es müssen allerdings
	%d Zeichen und %d%% der gesamten Nachricht sein).
BOT_KICK_CAPS_ON_BAN
	Bot wird jetzt User mit GROSSSCHRIFT kicken (es müssen allerdings
	%d Zeichen und %d%% der gesamten Nachricht sein), und wird nach %d Kicks
	einen Ban setzen.
BOT_KICK_CAPS_OFF
	Der Bot wird GROSSSCHRIFT ignorieren und nicht kicken.
BOT_KICK_COLORS_ON
	Bot kickt jetzt User, die farbig schreiben.
BOT_KICK_COLORS_ON_BAN
	Bot kickt jetzt User, die farbig schreiben, und wird einen Ban nach
	%d Kicks setzen.
BOT_KICK_COLORS_OFF
	Bot wird jetzt farbige Schrift ignorieren und nicht kicken.
BOT_KICK_FLOOD_ON
	Bot wird jetzt Flooder kicken (%d Zeilen in %d Sekunden).
BOT_KICK_FLOOD_ON_BAN
	Bot wird jetzt Flooder kicken (%d Zeilen in %d Sekunden), und wird einen
	Ban setzen nach %d Kicks.
BOT_KICK_FLOOD_OFF
	Bot wird jetzt Flooder ignorieren und nicht kicken.
BOT_KICK_REPEAT_ON
	Bot kickt jetzt User die sich wiederholen ( %d Mal das Gleiche)
BOT_KICK_REPEAT_ON_BAN
	Bot kickt jetzt User die sich wiederholen ( %d Mal das Gleiche), und setzt
	einen Ban nach %d Kicks.
BOT_KICK_REPEAT_OFF
	Bot ignoriert jetzt jetzt User die sich wiederholen und wird nicht kicken.
BOT_KICK_REVERSES_ON
	Bot kickt Leute, die invertiert schreiben.
BOT_KICK_REVERSES_ON_BAN
	Bot kickt User, die invertiert schreiben, und setzt einen Ban nach %d
	Kicks.
BOT_KICK_REVERSES_OFF
	Bot wird jetzt invertierten Schriftstil ignorieren und nicht kicken.
BOT_KICK_UNDERLINES_ON
	Bot kickt jetzt User, die unterstrichen schreiben.
BOT_KICK_UNDERLINES_ON_BAN
	Bot kickt jetzt User, die unterstrichen schreiben, und setzt einen Ban 
	nach %d Kicks.
BOT_KICK_UNDERLINES_OFF
	Bot wird jetzt unterstrichenen Text ignorieren und nicht kicken.
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS Channel {ADD|DEL|LIST|CLEAR} [Wort | entry-liste] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	Das Ändern der Bad Words-Liste ist derzeit deaktiviert.
BOT_BADWORDS_REACHED_LIMIT
	Du kannst nur %d Einträge in der Bad Words-Liste eines Channels haben.
BOT_BADWORDS_ALREADY_EXISTS
	%s existiert bereits in der Bad Words-Liste von %s.
BOT_BADWORDS_ADDED
	%s zur Bad Words-Liste von %s hinzugefügt.
BOT_BADWORDS_NO_SUCH_ENTRY
	Kein solcher Eintrag (#%d) wurde in der Bad Words-Liste von %s gefunden.
BOT_BADWORDS_NOT_FOUND
	%s nicht in der Bad Words-Liste von %s gefunden.
BOT_BADWORDS_NO_MATCH
	Keine entsprechenden Einträge zu %s in der Bad Words-Liste gefunden.
BOT_BADWORDS_DELETED
	%s wurde aus der Bad Words-Liste von %s entfernt.
BOT_BADWORDS_DELETED_ONE
	1 Eintrag wurde aus der Bad Words-Liste von %s entfernt.
BOT_BADWORDS_DELETED_SEVERAL
	%d Einträge wurden aus der Bad Words-Liste von %s entfernt.
BOT_BADWORDS_LIST_EMPTY
	Die Bad Words-Liste von %s ist leer.
BOT_BADWORDS_LIST_HEADER
	Bad Words-Liste von %s:
	  Nummer   Wort                           Typ
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	Die Bad Words-Liste ist jetzt leer.

# SAY responses
BOT_SAY_SYNTAX
	SAY Channel Text

# ACT responses
BOT_ACT_SYNTAX
	ACT Channel Text

# Errors
BOT_EXCEPT
	User stimmt mit Channel Exception überein.


# General messages
BOT_BAD_NICK
	Bot Nicknamen dürfen nur gültige Zeichen enthalten.
BOT_BAD_HOST
	Bot Hosts dürfen nur gültige Zeichen enthalten.
BOT_BAD_IDENT
	Bot Idents dürfen nur gültige Zeichen enthalten.
BOT_LONG_IDENT
	Bot Idents dürfen nur %d Zeichen enthalten.
BOT_LONG_HOST
	Bot Hosts may only contain %d characters.
###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Services können die Modi nicht ändern. Ist der Server richtig konfiguriert?
OPER_BOUNCY_MODES_U_LINE
	Services können die Modi nicht ändern. Sind die U:lines des
	Servers richtig konfiguriert?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL Nachricht

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Unbekannte STATS Option %s.
OPER_STATS_CURRENT_USERS
	Derzeit eingeloggte User: %d (%d Oper)
OPER_STATS_MAX_USERS
	                 Maximum: %d (%s)

# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Services laufen seit %d Tagen, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Services laufen seit %d Tag, %02d:%02d
OPER_STATS_UPTIME_HMS
	Services laufen seit %d Stunden, %d Minuten
OPER_STATS_UPTIME_HM1S
	Services laufen seit %d Stunden, %d Minuten
OPER_STATS_UPTIME_H1MS
	Services laufen seit %d Stunden, %d Minute
OPER_STATS_UPTIME_H1M1S
	Services laufen seit %d Stunden, %d Minute
OPER_STATS_UPTIME_1HMS
	Services laufen seit %d Stunde, %d Minuten
OPER_STATS_UPTIME_1HM1S
	Services laufen seit %d Stunde, %d Minuten
OPER_STATS_UPTIME_1H1MS
	Services laufen seit %d Stunde, %d Minute
OPER_STATS_UPTIME_1H1M1S
	Services laufen seit %d Stunde, %d Minute
OPER_STATS_UPTIME_MS
	Services laufen seit %d Minuten, %d Sekunden
OPER_STATS_UPTIME_M1S
	Services laufen seit %d Minuten, %d Sekunde
OPER_STATS_UPTIME_1MS
	Services laufen seit %d Minute, %d Sekunden
OPER_STATS_UPTIME_1M1S
	Services laufen seit %d Minute, %d Sekunde
OPER_STATS_BYTES_READ
	Bytes gelesen     : %5d kB
OPER_STATS_BYTES_WRITTEN
	Bytes geschrieben : %5d kB
OPER_STATS_USER_MEM
	User              : %6d Einträge, %5d kB
OPER_STATS_CHANNEL_MEM
	Channel           : %6d Einträge, %5d kB
OPER_STATS_GROUPS_MEM
	NickServ-Gruppen  : %6d Einträge, %5d kB
OPER_STATS_ALIASES_MEM
	NickServ-Aliase   : %6d Einträge, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ          : %6d Einträge, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ           : %6d Einträge, %5d kB
OPER_STATS_HOSTSERV_MEM
	HostServ          : %6d Einträge, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ          : %6d Einträge, %5d kB
OPER_STATS_SESSIONS_MEM
	Verbindungen      : %6d Einträge, %5d kB
OPER_STATS_AKILL_COUNT
	Aktuelle Anzahl von AKILLs: %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Standard Auslaufzeit von AKILLs: %d Tage
OPER_STATS_AKILL_EXPIRE_DAY
	Standard Auslaufzeit von AKILLs: 1 Tag
OPER_STATS_AKILL_EXPIRE_HOURS
	Standard Auslaufzeit von AKILLs: %d Stunden
OPER_STATS_AKILL_EXPIRE_HOUR
	Standard Auslaufzeit von AKILLs: 1 Stunde
OPER_STATS_AKILL_EXPIRE_MINS
	Standard Auslaufzeit von AKILLs: %d Minuten
OPER_STATS_AKILL_EXPIRE_MIN
	Standard Auslaufzeit von AKILLs: 1 Minute
OPER_STATS_AKILL_EXPIRE_NONE
	Standard Auslaufzeit von AKILLs: Kein Auslauf, permanent
OPER_STATS_SGLINE_COUNT
	Aktuelle Anzahl von SGLINEs: %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Standard Auslaufzeit von SGLINEs: %d Tage
OPER_STATS_SGLINE_EXPIRE_DAY
	Standard Auslaufzeit von SGLINEs: 1 Tag
OPER_STATS_SGLINE_EXPIRE_HOURS
	Standard Auslaufzeit von SGLINEs: %d Stunden
OPER_STATS_SGLINE_EXPIRE_HOUR
	Standard Auslaufzeit von SGLINEs: 1 Stunde
OPER_STATS_SGLINE_EXPIRE_MINS
	Standard Auslaufzeit von SGLINEs: %d Minuten
OPER_STATS_SGLINE_EXPIRE_MIN
	Standard Auslaufzeit von SGLINEs: 1 Minute
OPER_STATS_SGLINE_EXPIRE_NONE
	Standard Auslaufzeit von SGLINEs: Kein Auslauf, permanent
OPER_STATS_SQLINE_COUNT
	Aktuelle Anzahl von SQLINEs: %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Standard Auslaufzeit von SQLINEs: %d Tage
OPER_STATS_SQLINE_EXPIRE_DAY
	Standard Auslaufzeit von SQLINEs: 1 Tag
OPER_STATS_SQLINE_EXPIRE_HOURS
	Standard Auslaufzeit von SQLINEs: %d Stunden
OPER_STATS_SQLINE_EXPIRE_HOUR
	Standard Auslaufzeit von SQLINEs: 1 Stunde
OPER_STATS_SQLINE_EXPIRE_MINS
	Standard Auslaufzeit von SQLINEs: %d Minuten
OPER_STATS_SQLINE_EXPIRE_MIN
	Standard Auslaufzeit von SQLINEs: 1 Minuten
OPER_STATS_SQLINE_EXPIRE_NONE
	Standard Auslaufzeit von SQLINEs: Kein Auslauf, permanent
OPER_STATS_SZLINE_COUNT
	Aktuelle Anzahl von SZLINEs: %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Standard Auslaufzeit von SZLINEs: %d Tage
OPER_STATS_SZLINE_EXPIRE_DAY
	Standard Auslaufzeit von SZLINEs: 1 Tag
OPER_STATS_SZLINE_EXPIRE_HOURS
	Standard Auslaufzeit von SZLINEs: %d Stunden
OPER_STATS_SZLINE_EXPIRE_HOUR
	Standard Auslaufzeit von SZLINEs: 1 Stunde
OPER_STATS_SZLINE_EXPIRE_MINS
	Standard Auslaufzeit von SZLINEs: %d Minuten
OPER_STATS_SZLINE_EXPIRE_MIN
	Standard Auslaufzeit von SZLINEs: 1 Minute
OPER_STATS_SZLINE_EXPIRE_NONE
	Standard Auslaufzeit von SZLINEs: Kein Auslauf, permanent
OPER_STATS_RESET
	Statistiken zurückgesetzt.

# MODE responses
OPER_STATS_UPLINK_SERVER
	Uplink Server: %s
OPER_STATS_UPLINK_CAPAB
	Uplink-Fähigkeiten: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Server gefunden: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE Channel Modi

# UMODE respondes
OPER_UMODE_SYNTAX
	UMODE Nickname Modi

# UMODE responses
OPER_UMODE_SUCCESS
	Usermodi von %s geändert.
OPER_UMODE_CHANGED
	%s hat deine Usermodi geändert.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE Nickname Flags
OPER_OLINE_SUCCESS
	Operflags %s wurden für %s hinzugefügt. 
OPER_OLINE_IRCOP
	Du bist jetzt ein IRC Operator.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES Channel [ALL]
OPER_CLEARMODES_DONE
	Alle binären Modi und Bans wurden aus dem Channel %s entfernt.
OPER_CLEARMODES_ALL_DONE
	Alle Modi wurden vom Channel %s zurückgesetzt.


# KICK responses
OPER_KICK_SYNTAX
	KICK Channel User Grund
	
# ADMIN responses
OPER_SVSNICK_SYNTAX
	SVSNICK Nickname Neuer-Nickname 
OPER_SVSNICK_NEWNICK
	Der Nickname %s wird jetzt zu %s geändert.
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [Nickname | entry-liste]
OPER_ADMIN_SKELETON
	Services sind im Basis-Modus. Der ADMIN-Befehl ist nicht verfügbar.
OPER_ADMIN_EXISTS
	%s existiert bereits auf der Services Administratoren-Liste.
OPER_ADMIN_REACHED_LIMIT
	Du kannst nur %d Services Administratoren haben.
OPER_ADMIN_ADDED
	%s wurde zur Liste der Services Administratoren hinzugefügt.
OPER_ADMIN_NOT_FOUND
	%s wurde nicht auf der Liste der Services Administratoren gefunden.
OPER_ADMIN_NO_MATCH
	Keine entsprechenden Einträge in der Services Administratoren-Liste gefunden.
OPER_ADMIN_DELETED
	%s wurde aus der Services Administratoren-Liste entfernt.
OPER_ADMIN_DELETED_ONE
	Es wurde 1 Eintrag aus der Services Administratoren-Liste entfernt.
OPER_ADMIN_DELETED_SEVERAL
	Es wurden %d Einträge aus der Services Administratoren-Liste entfernt.
OPER_ADMIN_LIST_EMPTY
	Die Services Administratoren-Liste ist leer.
OPER_ADMIN_LIST_HEADER
	Services Administratoren-Liste:
	  Nummer   Nickname
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	Die Liste der Services Administratoren wurde geleert.
OPER_ADMIN_MOVED
	%s wurde zu der Services Administratoren-Liste verschoben.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [Nickname | entry-liste]
OPER_OPER_SKELETON
	Services sind im Basis-Modus. Das OPER-Befehl ist nicht verfügbar.
OPER_OPER_EXISTS
	%s existiert bereits auf der Services Operatoren-Liste.
OPER_OPER_REACHED_LIMIT
	Du kannst nur %d Services Operatoren haben.
OPER_OPER_ADDED
	%s wurde zur der Services Operatoren-Liste hinzugefügt.
OPER_OPER_NOT_FOUND
	%s wurde nicht auf der Services Operatoren-Liste gefunden.
OPER_OPER_NO_MATCH
	Keine entsprechenden Einträge in der Services Operatoren-Liste gefunden.
OPER_OPER_DELETED
	%s wurde aus der Services Operatoren-Liste entfernt.
OPER_OPER_DELETED_ONE
	Es wurde 1 Eintrag aus der Services Operatoren-Liste entfernt.
OPER_OPER_DELETED_SEVERAL
	Es wurden %d Einträge aus der Services Operatoren-Liste entfernt.
OPER_OPER_LIST_EMPTY
	Die Services Operatoren-Liste ist leer.
OPER_OPER_LIST_HEADER
	Services Operatoren-Liste:
	  Nummer   Nickname
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	Die Liste der Services Operatoren wurde geleert.
OPER_OPER_MOVED
	%s wurde zu der Services Operatoren-Liste verschoben.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+Zeit] {Hostmaske | entry-liste} [Grund]]
OPER_AKILL_EXISTS
	%s existiert bereits auf der AKILL-Liste.
OPER_AKILL_ALREADY_COVERED
	%s wird bereits von %s getroffen.
OPER_AKILL_REACHED_LIMIT
	Du kannst nur %d AKILLs haben.
OPER_AKILL_NO_NICK
	Achtung: AKILL-Masken dürfen keine Nicknamen enthalten, stelle sicher, dass du keinen in der Hostmaske eingetragen hast.
OPER_AKILL_ADDED
	%s wurde zur AKILL-Liste hinzugefügt.
OPER_AKILL_CHANGED
	Auslaufzeit von %s geändert.
OPER_AKILL_NOT_FOUND
	%s wurde nicht auf der AKILL-Liste gefunden.
OPER_AKILL_NO_MATCH
	Keine entsprechenden Einträge auf der AKILL-Liste.
OPER_AKILL_DELETED
	%s wurde von der AKILL-Liste entfernt.
OPER_AKILL_DELETED_ONE
	Es wurde 1 Eintrag von der AKILL-Liste entfernt.
OPER_AKILL_DELETED_SEVERAL
	Es wurden %d Einträge von der AKILL-Liste entfernt.
OPER_AKILL_LIST_EMPTY
	Die AKILL-Liste ist leer.
OPER_AKILL_LIST_HEADER
	Aktuelle AKILL-Liste:
	  Nummer   Maske                             Grund
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Aktuelle AKILL-Liste:
# number, mask, set-by, set-time, expires, reason
OPER_AKILL_VIEW_FORMAT
	%3d  %s (von %s am %s, %s)
	      %s
OPER_AKILL_CLEAR
	Die AKILL-Liste wurde geleert.
	
# SGLINE responses
OPER_CHANKILL_SYNTAX
	CHANKILL [+Zeit] {Channel} [Grund]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+Zeit] {Hostmask | entry-liste}[:Grund]]
OPER_SGLINE_UNSUPPORTED
	SGLINE ist in diesem Netz nicht verfügbar.
OPER_SGLINE_EXISTS
	%s existiert bereits auf der SGLINE-Liste.
OPER_SGLINE_ALREADY_COVERED
	%s wird bereits durch %s getroffen.
OPER_SGLINE_REACHED_LIMIT
	Du kannst nur %d SGLINE's haben.
OPER_SGLINE_ADDED
	%s wurde zur SGLINE-Liste hinzugefügt.
OPER_SGLINE_CHANGED
	Auslaufzeit von %s geändert.
OPER_SGLINE_NOT_FOUND
	%s wurde nicht auf der SGLINE-Liste gefunden.
OPER_SGLINE_NO_MATCH
	Keine entsprechenden Einträge auf der SGLINE-Liste.
OPER_SGLINE_DELETED
	%s wurde von der SGLINE-Liste entfernt.
OPER_SGLINE_DELETED_ONE
	Es wurde 1 Eintrag aus der SGLINE-Liste entfernt.
OPER_SGLINE_DELETED_SEVERAL
	Es wurden %d Einträge aus der SGLINE-Liste entfernt.
OPER_SGLINE_LIST_EMPTY
	Die SGLINE-Liste ist leer.
OPER_SGLINE_LIST_HEADER
	Aktuelle SGLINE-Liste:
	  Nummer   Maske                             Grund
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Aktuelle SGLINE-Liste:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (von %s am %s, %s)
	      %s
OPER_SGLINE_CLEAR
	Die SGLINE-Liste wurde geleert.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+Zeit] {Hostmaske | entry-liste} [Grund]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	Channel SQLINEs werden nicht von deinem IRCd unterstützt, folglich kannst du sie nicht nutzen.
OPER_SQLINE_EXISTS
	%s existiert bereits in der SQLINE Liste.
OPER_SQLINE_ALREADY_COVERED
	%s wird bereits durch %s getroffen.
OPER_SQLINE_REACHED_LIMIT
	Du kannst nur %d SQLINE's haben.
OPER_SQLINE_ADDED
	%s wurde zur SQLINE-Liste hinzugefügt.
OPER_SQLINE_CHANGED
	Auslaufzeit von %s geändert.
OPER_SQLINE_NOT_FOUND
	%s wurde nicht auf der SQLINE-Liste gefunden.
OPER_SQLINE_NO_MATCH
	Keine entsprechenden Einträge auf der SQLINE-Liste.
OPER_SQLINE_DELETED
	%s wurde von der SQLINE-Liste entfernt.
OPER_SQLINE_DELETED_ONE
	Es wurde 1 Eintrag aus der SQLINE-Liste entfernt.
OPER_SQLINE_DELETED_SEVERAL
	Es wurden %d Einträge aus der SQLINE-Liste entfernt.
OPER_SQLINE_LIST_EMPTY
	SQLINE Liste ist leer.
OPER_SQLINE_LIST_HEADER
	Aktuelle SQLINE Liste:
	  Nummer   Maske                              Grund
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Aktuelle SQLINE Liste:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (von %s am %s, %s)
	      %s
OPER_SQLINE_CLEAR
	Die SQLINE-Liste wurde geleert.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+Zeit] {Hostmaske | entry-liste} [Grund]]
OPER_SZLINE_UNSUPPORTED
	SZLINE ist auf diesem Netzwerk nicht verfügbar.
OPER_SZLINE_EXISTS
	%s existiert bereits auf der SZLINE-Liste.
OPER_SZLINE_ALREADY_COVERED
	%s wird bereits durch %s getroffen.
OPER_SZLINE_REACHED_LIMIT
	Du kannst nur %d SZLINE's haben.
OPER_SZLINE_ONLY_IPS
	Achtung: Man kann nur IP-Masken zur SZLINE-Liste hinzufügen.
OPER_SZLINE_ADDED
	%s wurde zur SZLINE-Liste hinzugefügt.
OPER_SZLINE_CHANGED
	Auslaufzeit von %s geändert.
OPER_SZLINE_NOT_FOUND
	%s wurde nicht auf der SZLINE-Liste gefunden.
OPER_SZLINE_NO_MATCH
	Kein entsprechender Eintrag in der SZLINE-Liste gefunden.
OPER_SZLINE_DELETED
	%s wurde von der SZLINE-Liste entfernt.
OPER_SZLINE_DELETED_ONE
	Es wurde 1 Eintrag aus der SZLINE-Liste entfernt.
OPER_SZLINE_DELETED_SEVERAL
	Es wurden %d Einträge aus der SZLINE-Liste entfernt.
OPER_SZLINE_LIST_EMPTY
	Die SZLINE-Liste ist leer.
OPER_SZLINE_LIST_HEADER
	Aktuelle SZLINE-Liste:
	  Nummer   Maske                             Grund
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Aktuelle SZLINE-Liste:
# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (von %s am %s, %s)
	      %s
OPER_SZLINE_CLEAR
	Die SZLINE-Liste wurde geleert.

# SET responses
OPER_SET_SYNTAX
	SET Option Einstellungen
OPER_SET_IGNORE_ON
	Ignore code wird benutzt.
OPER_SET_IGNORE_OFF
	Ignore code wird nicht benutzt.
OPER_SET_IGNORE_ERROR
	Die Einstellung von IGNORE kann nur ON oder OFF sein.
OPER_SET_READONLY_ON
	Services sind jetzt im read-only Modus.
OPER_SET_READONLY_OFF
	Services sind jetzt im read-write Modus.
OPER_SET_READONLY_ERROR
	Der Wert für READONLY muss entweder ON oder OFF sein.
OPER_SET_LOGCHAN_ON
	Die Services werden jetzt Logmessages an %s senden.
OPER_SET_LOGCHAN_OFF
	Die Services werden jetzt nicht mehr länger Logmessages an einen Channel senden.
OPER_SET_LOGCHAN_ERROR
	Die Logchannel Option muss auf ON oder OFF gesetzt werden und LogChannel muss definiert sein.
OPER_SET_DEBUG_ON
	Die Services sind jetzt im Debug-Modus.
OPER_SET_DEBUG_OFF
	Die Services sind nicht mehr im Debug-Modus.
OPER_SET_DEBUG_LEVEL
	Die Services sind jetzt im Debug-Modus (Level %d).
OPER_SET_DEBUG_ERROR
	Die Einstellung von DEBUG kann nur ON, OFF, oder eine positive Zahl sein.
OPER_SET_NOEXPIRE_ON
	Services sind jetzt im No Expire Modus.
OPER_SET_NOEXPIRE_OFF
	Services sind jetzt im Expire Modus.
OPER_SET_NOEXPIRE_ERROR
	Die Einstellung von NOEXPIRE kann nur ON oder OFF sein.
OPER_SET_UNKNOWN_OPTION
	Unbekannte Option %s.
OPER_SET_SQL_ON
	SQL code wird jetzt benutzt.
OPER_SET_SQL_OFF
	SQL code wird jetzt NICHT mehr benutzt.
OPER_SET_SQL_ERROR
	Einstellungen für SQL können nur ON oder OFF sein.
OPER_SET_SQL_ERROR_INIT
	SQL FEHLER, schaue bitte in deine Logs nach um zu sehen wie du es berichtigen kannst.
OPER_SET_SQL_ERROR_DISABLED
	SQL ist für den Netzwerk nicht konfiguriert. Um SQL code zu
	benutzen mußt du es in deine services.conf richtig Konfigurieren.
OPER_SET_LIST_OPTION_ON
	%s ist eingeschaltet
OPER_SET_LIST_OPTION_OFF
	%s ist ausgeschaltet

# NOOP responses
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} Server
OPER_NOOP_SET
	Alle O:Lines von %s wurden temporär entfernt.
OPER_NOOP_REVOKE
	Alle O:lines von %s wurden wiederhergestellt.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE Servername [Grund]

# RAW responses
OPER_JUPE_HOST_ERROR
	Benutze bitte einen gültigen Hostnamen, wenn du den JUPE Befehl benutzt.
OPER_JUPE_INVALID_SERVER
	Du kannst den JUPE Befehl nicht gegen Deinen Services Server oder Hub Server richten.
	
# RAW responses
OPER_RAW_SYNTAX
	RAW Text

# UPDATE responses
OPER_UPDATING
	Datenbanken wurden aktualisiert.
	
# RELOAD responses
OPER_RELOAD
	Die Konfigurationsdatei wurde neu eingelesen.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN wurde nicht definiert. Neustart nicht möglich. Benutze erneut das configure-Skript und kompiliere die Services erneut mit aktiviertem RESTART-Befehl.

# IGNORE respondes
OPER_IGNORE_SYNTAX
	IGNORE {ADD|DEL|LIST|CLEAR} [Zeit] [Nickname | Hostmaske]
OPER_IGNORE_VALID_TIME
	Du musst eine zulässige Zahl als Zeit eingeben.
OPER_IGNORE_TIME_DONE
	%s wird jetzt für %s ignoriert.
OPER_IGNORE_PERM_DONE
	%s will wird jetzt permanent ignoriert.
OPER_IGNORE_DEL_DONE
	%s wird nicht länger ignoriert.
OPER_IGNORE_LIST
	Services Ignore-Liste:
OPER_IGNORE_LIST_NOMATCH
	Der Nickname %s wurde nicht in der Ignore-Liste gefunden.
OPER_IGNORE_LIST_EMPTY
	Ignore-Liste ist leer.
OPER_IGNORE_LIST_CLEARED
	Die Ignore-Liste wurde geleert.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES Nickname
OPER_KILLCLONES_UNKNOWN_NICK
	Kann User %s nicht finden.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Channel-Liste:
	Name                  User Topic
OPER_CHANLIST_HEADER_USER
	%s Channel-Liste:
	Name                  User Topic
OPER_CHANLIST_RECORD
	%-20s  %4d %s
OPER_CHANLIST_END
	Ende der Channel-Liste.
	
# USERLIST responses
OPER_USERLIST_HEADER
	User-Liste:
	Nickname                 Maske
OPER_USERLIST_HEADER_CHAN
	%s User-Liste:
	Nickname                 Maske
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Ende der User-Liste.

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_SUPER_ADMIN_ON
	Du bist jetzt ein Super-Admin.
OPER_SUPER_ADMIN_OFF
	Du bist jetzt kein Super-Admin mehr.
OPER_SUPER_ADMIN_SYNTAX
	Super-Admin muss ON oder OFF sein.
OPER_SUPER_ADMIN_WALL_ON
	%s ist jetzt ein Super-Admin.
OPER_SUPER_ADMIN_WALL_OFF
	%s verlässt den Super-Admin Mode.
OPER_SUPER_ADMIN_ONLY
	NUR Super-Admins können dieses Befehl benutzen.

OPER_STAFF_LIST_HEADER
	On Level Nickname

# DefCon Messages

OPER_STAFF_FORMAT
	 %c %s  %s
OPER_STAFF_AFORMAT
	 %c %s  %s [%s]
OPER_DEFCON_SYNTAX
	DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	Dieser Service ist derzeit deaktiviert, bitte versuche es später nochmal
OPER_DEFCON_NO_CONF
	Das Defcon-System muss in der services.conf aktiviert worden sein
OPER_DEFCON_CHANGED
	Services sind jetzt beim DEFCON %d
OPER_DEFCON_WALL
	%s hat das DEFCON-Level geändert zu %d
DEFCON_GLOBAL
	Das Defcon-Level ist jetzt auf Level: %d

# Module strings

OPER_MODULE_LOADED
	Modul %s geladen
OPER_MODULE_UNLOADED
	Modul %s entfernt
OPER_MODULE_LOAD_FAIL
	Kann Modul %s nicht laden
OPER_MODULE_REMOVE_FAIL
	Kann Modul %s nicht entfernen
OPER_MODULE_NO_UNLOAD
	Modul KANN NICHT entfernt werden.
OPER_MODULE_LOAD_SYNTAX
	MODLOAD Modul-Name
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD Modul-Name
OPER_MODULE_LIST_HEADER
	Aktuelle Module - Liste:
OPER_MODULE_LIST
	Module: %s [%s] [%s]
OPER_MODULE_LIST_FOOTER
	%d Module geladen.
OPER_MODULE_INFO_LIST
	Module: %s Version: %s Autor: %s geladen: %s
OPER_MODULE_CMD_LIST
	Unterstützte Befehle: %R%s %s
OPER_MODULE_MSG_LIST
	Unterstützte IRCD's für: %s
OPER_MODULE_NO_LIST
	Derzeit keine Module geladen
OPER_MODULE_NO_INFO
	Es sind keine Informationen für das Modul %s verfügbar
OPER_MODULE_INFO_SYNTAX
	MODINFO Modul-Name

MODULE_HELP_HEADER
	Die folgenden Befehle wurden durch ein Modul geladen:

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [Parameter]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+Zeit] Hostmaske Limit Grund
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {Hostmaske | Liste}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE Nummer Position
OPER_EXCEPTION_DISABLED
	Eingeschränkte Verbindungen sind deaktiviert.
OPER_EXCEPTION_ALREADY_PRESENT
	Die Hostmaske %s ist bereits auf der Liste für eingeschränkte Verbindungen.
OPER_EXCEPTION_TOO_MANY
	Die Liste für eingeschränkte Verbindungen ist voll! 
OPER_EXCEPTION_ADDED
	Verbindungslimit für %s geändert auf %d.
OPER_EXCEPTION_MOVED
	DIe Exception für %s (#%d) wurde auf die  %d Position verschoben.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Kein solcher Eintrag (#%d) in der Liste der Verbindungs-
	einschränkungen.
OPER_EXCEPTION_NOT_FOUND
	%s nicht gefunden in der Liste der Verbindungs-
	einschränkungen.
OPER_EXCEPTION_NO_MATCH
	Keine entsprechenden Einträge in der Liste der 
	Verbindungseinschränkungen.
OPER_EXCEPTION_DELETED
	%s wurde von der Liste der Verbindungseinschränkungen 
	entfernt.
OPER_EXCEPTION_DELETED_ONE
	Es wurde 1 Eintrag von der Liste der Verbindungs-
	einschränkungen entfernt.
OPER_EXCEPTION_DELETED_SEVERAL
	Es wurden %d Einträge von der Liste der Verbindungs-
	einschränkungen entfernt.
OPER_EXCEPTION_LIST_HEADER
	Aktuelle Liste für eingeschränkte Verbindungen:
# mask, limit
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Nummer  Limit  Host
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (von %s am %s, %s)
	    Limit: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Ungültige Verbindungseinschränkung. Es muss eine gültige Integer-Zahl sein, grösser oder gleich
	Null aber kleiner als %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Ungültige Hostmask. Nur echte Hostmasken sind gültig als Einträge (ohne Nicknamen und Idents).
OPER_EXCEPTION_EXISTS
	%s ist bereits in der  EXCEPTION-Liste.
OPER_EXCEPTION_CHANGED
	Exception für %s ist zu %d aktualisiert worden.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST Limit | VIEW Host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST Limit
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW Host
OPER_SESSION_DISABLED
	SESSION Befehl ist deaktiviert.
OPER_SESSION_INVALID_THRESHOLD
	Ungültiger Wert. Es muss eine gültige grade Zahl sein, die grösser als 1 ist.
OPER_SESSION_NOT_FOUND
	Es wurde keine Verbindung von %s gefunden.
OPER_SESSION_LIST_HEADER
	Hosts mit mindestens %d Verbindungen:
OPER_SESSION_LIST_COLHEAD
	Verbindungen  Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	Der Host %s hat momentan %d Verbindungen und ein Limit von %d.

# EXCEPTION help

OPER_HELP_EXCEPTION
	Syntax: EXCEPTION ADD [+Zeit] Hostmaske Limit Grund
	        EXCEPTION DEL {Hostmaske | Liste}
	        EXCEPTION MOVE Nummer Position
	        EXCEPTION LIST [Hostmaske | Liste]
	        EXCEPTION VIEW [Hostmaske | Liste]
	
	Erlaubt Services Administratoren, die Liste der Verbindungs-
	einschränkungen zu bearbeiten.Dadurch ist es möglich
	bestimmte Hosts, z.b. Shell-Server, auf eine bestimmte 
	Anzahl von gleichzeitigen Verbindungen einschränken.Hat 
	ein Host dieses Limit erreicht, wird jede neue Verbindung
	(vom gleichen Host) nicht mehr akzeptiert. Der betroffene User 
	wird entfernt (gekilled) und via Notice vom %S informiert
	warum er vom Netzwerk getrennt wurden.
	
	EXCEPTION ADD fügt die angegebene Hostmaske zu der
	Liste der Verbindungseinschränkungen hinzu. 
	Beachte, dass nick!user@host und user@host keine richtigen
	Hostmasken sind!
	Nur echte Hosts, wie z.B. box.host.dom und *.host.dom sind 
	zulässig, weil für eingeschränkte Verbindungen die Nicknamen
	oder Idents nicht von Interesse sind.
	Limit muss eine Nummer grösser oder gleich 0 sein, und legt
	fest, wieviele Verbindungen von diesem Host gemacht werden dürfen.
	Ein Limit von Null bedeutet, dass dieser Host keine ein-
	geschränkte Verbindung hat. Siehe die AKILL Hilfe fuer
	weitere Informationen über das Format des optionalen Zeit 
	Parameter.
	
	EXCEPTION DEL entfernt den Host von der Liste.
	
	EXCEPTION MOVE verschiebt den Eintrag mit der Nummer 
	auf die Position. Die Einträge dazwischen werden
	hoch bzw runtergeschoben um die Lücken zu füllen.
	
	EXCEPTION LIST und EXCEPTION VIEW zeigt alle aktuellen 
	Verbindungseinschränkungen. Wird die optionale Hostmaske 
	angegeben, werden nur die entsprechenden Einträge aus
	der Liste angezeigt. Der Unterschied zwischen den beiden 
	Befehlen liegt darin, dass EXCEPTION VIEW ausführlicher ist.
	Es zeigt unter anderem den Namen der Person, die den Eintrag
	erstellt hat, dessen Grund, das Limit, die Hostmaske und
	die Auslaufzeit.
	
	Beachte das ein verbundender Client die erste Beschränkung
	die auf seinen Host zutrifft, "benutzen" wird. Grosse und
	weit treffende Einträge in der Liste der Verbindungs-
	einschränkungen senken die Performance der Services.

OPER_HELP_SESSION
	Syntax: SESSION LIST threshold
	        SESSION VIEW Host
	
	Zeigt Services Administratoren eine Liste mit Hosts, die 
	eine bestimmte Anzahl Verbindungen zum Chat-Netz haben.
	
	SESSION LIST zeigt die Hosts mit mindestens threshold 
	gleichzeitigen Verbindungen.
	Der threshold muss eine Nummer grösser als 1 sein. Dies
	soll verhindern, dass aus Versehen eine riesige Liste
	von Einzelsitzungen ausgegeben wird.
	
	SESSION VIEW ist etwas ausführlicher und zeigt mehr
	Informationen über den bestimmten Host - mit
	Anzahl Verbindungen und dessen Limit. Der Wert bei
	host kann keine Platzhalter (Wildcards) enthalten.
	
	Siehe die Hilfe zu EXCEPTION um weitere Informationen
	über das Einschränken von Verbindungen zu erhalten.

###########################################################################
#
# News system messages 
#
###########################################################################

OPER_HELP_STAFF
	Syntax: STAFF

	Zeigt alle Nicknamen, die Zugriff auf die Services haben samt Level 
	und Online-Status.

OPER_HELP_DEFCON
	Syntax: DEFCON [1|2|3|4|5]

	Das Defcon-System kann benutzt werden, um vordefinierte Einstellung
	von einschränkungen der Services einzubinden - nützlich während 
	versuchter Angriffe auf das Netzwerk.

OPER_HELP_DEFCON_NO_NEW_CHANNELS
	* Keine neuen Channel-Registrierungen
OPER_HELP_DEFCON_NO_NEW_NICKS
	* Keine neuen Nicknamen-Registrierungen
OPER_HELP_DEFCON_NO_MLOCK_CHANGE
	* keine MLOCK änderungen
OPER_HELP_DEFCON_FORCE_CHAN_MODES
	* Zwingt alle Channels die Modes (%s) zu setzen
OPER_HELP_DEFCON_REDUCE_SESSION
	* Benutzt das reduzierte Session-Limit von %d
OPER_HELP_DEFCON_NO_NEW_CLIENTS
	* Killt jeden NEU verbindenden Clienten
OPER_HELP_DEFCON_OPER_ONLY
	* Ignoriert jeden non-opers mit Nachrichten
OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* Stilles ignorieren von non-opers
OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* Setzt einen AKILL auf jeden NEU verbindenden Clienten
OPER_HELP_DEFCON_NO_NEW_MEMOS
	* Es werden keine neuen Memos gesendet
OPER_HELP_CHANKILL
	Syntax: CHANKILL [+Zeit] Channel Grund

	Setzt einen AKILL für jeden Nicknamen des angegebenen Channels. 
	Es werden die Einträge und komplette reale ident@host für jeden 
	Nickname benutzt, dann wird der AKILL durchgeführt. 

###########################################################################
#
# News system messages  ## 19.10.2004 ##
#
###########################################################################

NEWS_LOGON_TEXT
	[Logon News - %s] %s
NEWS_OPER_TEXT
	[Oper News - %s] %s

NEWS_RANDOM_TEXT
	[Random News - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD|DEL|LIST} [Text|Nummer]
NEWS_LOGON_LIST_HEADER
	Logon News Einträge:
NEWS_LOGON_LIST_ENTRY
	%5d (%s von %s)
	    %s
NEWS_LOGON_LIST_NONE
	Es gibt keine Logon News.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD Text
NEWS_LOGON_ADD_FULL
	Die News-Liste ist voll!
NEWS_LOGON_ADDED
	Neuer Logon News-Eintrag hinzugefügt (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {Nummer | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Logon News-Eintrag #%d konnte nicht gefunden werden!
NEWS_LOGON_DELETED
	Logon News-Eintrag #%d wurde gelöscht.
NEWS_LOGON_DEL_NONE
	Keine zu löschenden Einträge in der Logon News-Liste gefunden.
NEWS_LOGON_DELETED_ALL
	Alle Logon-News wurden entfernt.

NEWS_OPER_SYNTAX
	Syntax: OPERNEWS {ADD|DEL|LIST} [Text|Nummer]
NEWS_OPER_LIST_HEADER
	Oper News Einträge:
NEWS_OPER_LIST_ENTRY
	%5d (%s von %s)
	    %s
NEWS_OPER_LIST_NONE
	Es gibt keine Oper-News.
NEWS_OPER_ADD_SYNTAX
	Syntax: OPERNEWS ADD Text
NEWS_OPER_ADD_FULL
	Die News-Liste ist voll!
NEWS_OPER_ADDED
	Neuer Oper-News-Eintrag hinzugefügt (#%d).
NEWS_OPER_DEL_SYNTAX
	Syntax: OPERNEWS DEL {Nummer | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Oper-News-Eintrag #%d nicht gefunden!
NEWS_OPER_DELETED
	Oper-News-Eintrag #%d wurde entfernt.
NEWS_OPER_DEL_NONE
	Keine Oper-News-Einträge zu löschen!
NEWS_OPER_DELETED_ALL
	Alle Oper-News-Einträge wurden entfernt.

NEWS_RANDOM_SYNTAX
	RANDOMNEWS {ADD|DEL|LIST} [Text|Nummer]
NEWS_RANDOM_LIST_HEADER
	Random news Einträge:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s von %s)
	    %s
NEWS_RANDOM_LIST_NONE
	Keine Randomnews vorhanden.
NEWS_RANDOM_ADD_SYNTAX
	RANDOMNEWS ADD Text
NEWS_RANDOM_ADD_FULL
	Die News Liste ist voll!
NEWS_RANDOM_ADDED
	Neuer Randomnews Eintrag hinzugefügt (#%d).
NEWS_RANDOM_DEL_SYNTAX
	RANDOMNEWS DEL {Nummer | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Randomnews Eintrag #%d nicht gefunden!
NEWS_RANDOM_DELETED
	Randomnews Eintrag #%d gelöscht.
NEWS_RANDOM_DEL_NONE
	Keine Randomnews Einträge zu löschen!
NEWS_RANDOM_DELETED_ALL
	Ale Randomnews Einträge wurden gelöscht.

NEWS_HELP_LOGON
	Syntax: LOGONNEWS ADD Text
	        LOGONNEWS DEL {Nummer | ALL}
	        LOGONNEWS LIST
	
	Zeigt oder bearbeitet die Liste der LogonNews.
	Nach der Verbindung des Users werden diese Nachrichten
	angezeigt. (Es werden jedoch nur max. %s angezeigt, um ein
	Flooding des Users zu verhindern. Wenn mehr News
	existieren, werden nur die neuesten angezeigt.)
	Die Anzahl der News können in der services.conf festgelegt werden.
	
	Diese Funktion ist beschränkt auf die Services Administartoren.

NEWS_HELP_OPER
	Syntax: OPERNEWS ADD Text
	        OPERNEWS DEL {Nummer | ALL}
	        OPERNEWS LIST
	
	Zeigt oder bearbeitet die Liste der Oper-News.Nachdem 
	sich ein User die Oper-Flags gesetzt hat (mit dem
	/OPER Befehl) werden ihm diese Nachrichten angezeigt.
	(Es werden jedoch nur max. %s Nachrichten angezeigt, um ein
	Flooding des Users zu verhindern. Wenn mehr News
	existieren, werden nur die neüsten angezeigt.)
	Die Anzahl der News können in der services.conf festgelegt werden.
	
	Diese Funktion ist beschränkt auf die Services Administartoren.

NEWS_HELP_RANDOM
	Syntax: RANDOMNEWS ADD Text
	        RANDOMNEWS DEL {Nummer | ALL}
	        RANDOMNEWS LIST
	
	Bearbeitet die Liste der RandomNews Nachrichten oder zeigt diese an.
	Wenn ein User zum Network verbindet, wird eine (und nur eine!) dieser
	RandomNews Nachrichten zufällig gewählt und an den User gesendet.
	
	Diese Funktion ist beschränkt auf die Services Administartoren.

###########################################################################
#
# HelpServ help message  ## 20.10.2004 ##
#
###########################################################################

HELP_HELP
	%S dient dazu, Hilfe und weitere Informationen zu den Services
	auszugeben. Eine Themenübersicht bekommt man mit Hilfe des 
	HELP Befehls der anderen Services:
	
	%R%s HELP
	     für Informationen zum Registrieren der Nicknames
	
	%R%s HELP
	     für Informationen zum Registrieren und Kontrollieren der
	     Channels
	
	%R%s HELP
	     für Informationen zum Senden von Memos an User, die im
	     Moment nicht online sind

HELP_HELP_BOT

	%R%s HELP
	     für Informationen wie man einen Bot in einem Channel
	     einrichtet

HELP_HELP_HOST

	%R%s HELP
	     für Informationen über vHosts Einstellungen

###########################################################################
#
# NickServ help messages
#
###########################################################################
NICK_HELP_CMD_CONFIRM
	    CONFIRM    Bestätigt einen "nickserv auth code".
NICK_HELP_CMD_RESEND
	    RESEND     Sendet ernuet ein "nickserv auth code".

NICK_HELP_CMD_REGISTER
	    REGISTER    Registriert einen Nicknamen
NICK_HELP_CMD_GROUP
	    GROUP       Verwaltung von Nickname-Gruppierungen
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY    Identifizierung des Nicknames
NICK_HELP_CMD_ACCESS
	    ACCESS      Liste der autorisierten Adressen
NICK_HELP_CMD_SET
	    SET         Optionen einstellen, auch Kill-Schutz
NICK_HELP_CMD_SASET
	    SASET      Setzt SET Einstellungen auf einen anderen Nicknamen
NICK_HELP_CMD_DROP
	    DROP        Die Registrierung einen Nicknamen löschen
NICK_HELP_CMD_RECOVER
	    RECOVER     User killen, der deinen Nicknamen verwendet
NICK_HELP_CMD_RELEASE
	    RELEASE     Hebt die Nickname-Sperre auf nach der 
	                verwendung des RECOVER Befehls
NICK_HELP_CMD_SENDPASS
	    SENDPASS    Passwort vergessen? Versuch das hier.
NICK_HELP_CMD_GHOST
	    GHOST      Trennt eine "ghost" IRC-Verbindung mit deinen Nicknamen
NICK_HELP_CMD_ALIST
	    ALIST      Listet Channels in dem du Zugang hast
NICK_HELP_CMD_GLIST
	    GLIST      Listet alle Nicknamen in deine Gruppe
NICK_HELP_CMD_INFO
	    INFO       Zeigt Informationen über den angegebenen Nicknamen
NICK_HELP_CMD_LIST
	    LIST       Listet alle registrieten Nicknamen des angegebene Muster
NICK_HELP_CMD_LOGOUT
	    LOGOUT     Macht das IDENTIFY Befehl rückgängig
NICK_HELP_CMD_STATUS
	    STATUS     Zeigt den Status des angegebenen Nicknamens an
NICK_HELP_CMD_UPDATE
	    UPDATE     Erneuert dein gegenwärtigen Status ( Memos , Flags , vHost )
NICK_HELP_CMD_GETPASS
	    GETPASS    Das Passwort eines Nicknamens aus der
	               Datenbank auslesen (nur wenn keine
	               Verschlüsselung aktiv ist)
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Gibt die passenden Nicknamen wieder, 
	               welche die angegebene eMail benutzen.
NICK_HELP_CMD_FORBID
	    FORBID     Einen bestimmten Nicknamen verbieten
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Suspendet den angegebenen Nicknamen
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Unsuspend den angegebenen Nicknamen

NICK_HELP
	Mit %S kannst du einen Nicknamen "registrieren" und somit
	verhindern, dass andere ihn nutzen. Die folgenden Befehle
	erlauben die Registrierung und das Ändern der Einstellungen
	von den Nicknamen. Um sie zu benutzen, tippe 
	%R%S Befehl. Für weitergehende Informationen zu
	einem bestimmten Befehl, tippe %R%S HELP Befehl.
	
NICK_HELP_FOOTER
	WARNUNG: Dieser Service soll den Usern einen Weg ermöglichen,
	den Missbrauch ihres Nicknames zu verhindern.Es ist NICHT dazu 
	gedacht, Nicknamen zu stehlen oder in Massen zu reservieren. 
	Missbrauch von %S wird geahndet und resultiert mindestens im 
	Verlust deiner registrierten Nicknamen.

NICK_HELP_EXPIRES
	
	Nicknamen die seit einer längeren Zeit nicht mehr
	genutzt wurden, werden durch den automatischen Auslauf
	nach %d Tagen aus der Datenbank entfernt.

NICK_HELP_REGISTER
	Syntax: REGISTER Passwort [eMail]
	
	Registriert deinen Nicknamen in der Datenbank von 
	%S. Nachdem dein Nickname registriert ist, kannst du mit 
	den SET und ACCESS Befehlen die Einstellungen deines
	Nicknames anpassen wie du es möchtest. Stell bitte
	sicher, dass du dir das Passwort bei der Registrierung
	merkst - du wirst es brauchen, wenn du später
	Änderungen an den Einstellungen vornehmen willst!
	
	Beachte auch, dass Gross/Kleinschreibung unterschieden
	werden! ANOPE, Anope, und anope sind 
	verschiedene Passworte!
	
	Richtlinien bei der Auswahl eines Passwortes:
	
	Passworte sollten nicht einfach zu erraten sein.
	Zum Beispiel deinen echten Namen als Passwort zu 
	nehmen ist eine schlechte Idee. Deinen Nicknamen selbst
	als Passwort zu nehmen, ist eine noch viel schlechtere
	Idee :) und ausserdem würde %S es auch nicht erlauben.
	Weiterhin sind kurze Passworte anfällig bei Trial-
	and-Error-Versuchen, also sollte es zumindest 5 Zeichen
	lang sein.
	Zu guter letzt solltest du auch keine Leerzeichen im 
	Passwort verwenden. 
	
	Der Parameter eMail ist optional und ermöglicht
	Nickserv dir dein Passwort per eMail zu senden, falls
	du es vergessen haben solltest. 
	Deine Privatsphäre wird respektiert und deine 
	eMail-Adresse wird keinem Dritten verfügbar gemacht.
	
	Dieser Befehl (REGISTER) erstellt auch ausserdem
	eine neue Gruppe für deinen Nicknamen, die dir erlaubt,
	nachher weitere Nicknamen in diese Gruppe aufzunehmen,
	die dann automatisch die gleiche Konfiguration und
	Channel-Privilegien bekommen. Für weitere Informationen
	über diese Funktion tippe %R%S HELP GROUP.
	
NICK_HELP_GROUP
	Syntax: GROUP Ziel Passwort
	
	Dieser Befehl verbindet deinen Nicknamen mit der
	Gruppe von Ziel. Passwort ist das Passwort
	des Ziel-Nicknamens.
	
	Das Teilnehmen in der Gruppe teilt deine Konfiguration,
	Memos und Channel-Privilegien mit allen Nicknamen der 
	Gruppe, und vieles mehr!
	
	Eine Gruppe existiert so lange es sinnvoll ist. Dies
	bedeutet, dass selbst wenn ein Nicknamen der Gruppe 
	GEDROPED (gelöscht) wird, die geteilten Konfigurationen,
	Rechte erhalten bleiben, so lange mindestens ein Nickname 
	in der Gruppe verbleibt.
	 
	Dieser Befehl lässt sich sogar verwenden, wenn dein Nickname
	(noch) nicht registriert ist. Wenn er jedoch registriert ist, 
	musst du dich vorher identifizieren. 
	Tippe %R%S HELP IDENTIFY für weitere Informationen.
	
	Es wird allerdings empfohlen diesen Befehl mit einem
	nicht-registrierten Nicknamen zu verwenden, da der 
	Nickname automatisch registriert wird, wenn er der Gruppe 
	beitritt. Das Beitreten einer Gruppe mit einem bereits 
	registrierten Nicknamen kann durch die Netzwerk-Administratoren 
	gesperrt sein.
	    
	Du kannst nur in einer Gruppe zur gleichen Zeit sein.
	Das Zusammenführen verschiedener Gruppen ist nicht
	möglich.
	
	Achtung: Alle Mitglieder einer Gruppe haben dasselbe
	Passwort.

NICK_HELP_IDENTIFY
	Syntax: IDENTIFY Passwort
	
	Beweist %S, dass du wirklich der Besitzer dieses 
	Nicknamens bist. Viele Befehle erzwingen, dass 
	du dich vorher hiermit autorisierst, bevor du sie 
	benutzen kannst. Das Passwort ist das von deiner 
	Registrierung, die du mit dem REGISTER Befehl 
	gesetzt hast.
	
NICK_HELP_UPDATE
	Syntax: UPDATE

	Updatet deinen derzeiten Status, z.B. prüft es auf neue Nachrichten,
	setzt benötigte Channel-Modes (ModeonID), aktualisiert deinen vHost und
	deine Userflags (lastseentime, etc).
	
NICK_HELP_LOGOUT
	Syntax: LOGOUT
	
	Dieser Befehl macht den Effekt des IDENTIFY Befehls
	rückgängig. NickServ erkennt dich also nicht mehr als
	den Besitzer des Nicknamens. Beachte jedoch, dass du 
	nicht mehr gewarnt wirst, dich erneut zu identifizieren.

NICK_HELP_DROP
	Syntax: DROP [Nickname]
	
	Entfernt die Registrierung deines Nicknames aus
	der Datenbank von %S. Ein gelöschter Nickname 
	kann von jedem jederzeit wieder registriert 
	werden.
	
	Du kannst auch einen Nicknamen innerhalb deiner
	Gruppe entfernen lassen, wenn du ihn als Parameter
	bei Nickname angibst.
	
	Wenn du dieses Befehl benutzen willst, musst du
	dich vorher mit deinem Passwort identifizieren. 
	Tippe : %R%S HELP IDENTIFY für weitere Informationen

NICK_HELP_ACCESS
	Syntax: ACCESS ADD Maske
	        ACCESS DEL Maske
	        ACCESS LIST
	
	Zeigt oder bearbeitet die Zugriffsliste für deinen 
	Nicknamen. Durch diese Liste erkennt %S dich 
	automatisch als berechtigt, diesen Nicknamen zu 
	nutzen. Wenn du den Nicknamen von anderen Adressen
	verwenden willst, musst du dich durch das 
	IDENTIFY Befehls einloggen, damit %S
	dich erkennt.
	
	Beispiele:
	
	    ACCESS ADD anyone@*.bepeg.com
	    Erlaubt zugriff vom User anyone von
	    jedem Host aus der bepeg.com Domain.
	
	    ACCESS DEL anyone@*.bepeg.com
	    Hebt den oben genannten Befehl wieder auf.
	
	    ACCESS LIST
	    Zeigt die aktuelle Zugriffsliste an.

NICK_HELP_SET
	Syntax: SET Option Parameter
	
	Stellt verschiedene Optionen zu deinem Nicknamen ein.
	Option kann eines der folgenden Werte sein:
	
	    DISPLAY      Ändert den "Ursprung" deiner Gruppe
	    PASSWORD     Setzt das Passwort deines Nicknamens neu
	    LANGUAGE     Stellt die Sprache ein, in der dir die 
	                 Services antworten sollen.
	    URL          Verbindet eine URL mit deinem Nicknamen
	    ICQ          Trägt deine ICQ-Nummer unter den Infos
	                 zu deinem Nicknamen ein.
	    MSG          Ändert die Methode, wie die Services mit
	                 dir kommunizieren
	    EMAIL        Verbindet eine öffentliche eMail-Adresse
	                 mit deinem Nicknamen         
	    GREET        Stellt eine Begrüssungsmeldung für deinen
	                 Nicknamen ein.
	    KILL         Stellt Kill-Schutz an oder aus.
	    SECURE       Aktiviert/Deaktiviert den Sicherheitsmodus
	                 für deinen Nicknamen
	    PRIVATE      Verhindert das Erscheinen deines Nicknamens 
	                 auf der Liste von %R%S LIST
	    HIDE         Versteckt bestimmte Informationen deines
	                 Nicknamens
	    AUTOOP       Sollten Dir die Services automatisch
	                 Op-Status geben.    
	
	Wenn du eines dieser Optionen nutzen willst, musst du dich
	vorher identifizeren. 
	Tippe: %R%S HELP IDENTIFY für weitere Informationen.
	
	Tippe %R%S HELP SET Option für Erläuterungen
	zu einer bestimmten oben genannten Option.

NICK_HELP_SET_DISPLAY
	Syntax: SET DISPLAY Neues-Display
	
	Ändert den virtuellen "Anführer" deiner Gruppe in den
	Services. Der wert von Neues-Display MUSS ein Nickname
	in deiner Gruppe sein.
	
	Hinweis: Dieser Befehl ändert keine Zugriffsrechte
	des einzelnen Nicks, er wird nur als "Ursprung" der 
	Gruppe in der Liste aufgeführt.
	
NICK_HELP_SET_PASSWORD
	Syntax: SET PASSWORD Neues-Passwort
	
	Ändert das Passwort, mit dem du dich als Besitzer des
	Nicknamen identifizierst.

NICK_HELP_SET_LANGUAGE
	Syntax: SET LANGUAGE Nummer
	
	Stellt die Sprache ein, mit der die Services mit dir
	kommunizieren. (Zum Beispiel in einer Antwort auf einen
	Befehl den du gesendet hast.)
	
	Nummer muss von der folgenden Liste der unter-
	stützten Sprachen ausgewählt werden: 

NICK_HELP_SET_URL
	Syntax: SET URL Url
	
	Verbindet die angegebene URL mit deinem Nicknamen.
	Diese URL wird angezeigt, wenn jemand Informationen
	über deinen Nicknamen mit dem INFO Befehl anfordert.

NICK_HELP_SET_EMAIL
	Syntax: SET EMAIL Adresse
	
	Verbindet die angegebene eMail-Adresse mit deinem
	Nicknamen. Diese Adresse wird angezeigt, wenn jemand 
	Informationen über deinen Nicknamen mit dem INFO 
	Befehl anfordert.
	
NICK_HELP_SET_ICQ
	Syntax: SET ICQ Nummer
	
	Verbindet die angegebene ICQ Nummer mit deinem Nicknamen.
	Diese Nummer wird angezeigt, wenn jemand Informationen
	über deinen Nicknamen mit dem INFO Befehl anfordert.
	
NICK_HELP_SET_GREET
	Syntax: SET GREET Nachricht
	
	Die angegebene Nachricht wird als Begrüssungsnachricht
	gesendet, wenn du in einen Channel eintrittst, der
	die GREET Option eingeschaltet hat. Vorausgesetzt
	du hast den nötigen Zugriffslevel in der Access-Liste
	des Channels.

NICK_HELP_SET_KILL
	Syntax: SET KILL {ON | QUICK | IMMED | OFF}
	
	Stellt den automatischen Schutz des Nicknames durch
	einen Kill an oder aus. Nimmt ein User deinen Nickname, 
	hat er eine Minute Zeit sich zu identifizieren,
	danach wird er, bei eingeschaltetem Schutz, durch
	%S umbenannt in einen Zufallsnamen.
	
	Bei der Einstellung QUICK wird Wartezeit zum
	Identifizieren auf 20 Sekunden herabgesenkt. Bei
	IMMED wird der User sofort ohne Warnung
	umbenannt, wenn er nicht auf der Zugriffsliste des
	Nicknamens steht. Bitte benutze diese Option nicht
	unnötigerweise.
	Ausserdem ist es möglich, dass die Netz-Admins
	diese Option deaktiviert haben.

NICK_HELP_SET_SECURE
	Syntax: SET SECURE {ON | OFF}
	
	Stellt die Sicherheitsfunktionen von %S für deinen
	Nicknamen an oder aus. Mit eingeschaltetem SECURE
	musst du dich immer identifizieren, egal ob deine
	Hostmaske auf der Zugriffsliste für den Nicknamen
	steht oder nicht. Wenn du allerdings auf 
	der Zugriffsliste stehst, wird %S bei 
	fehlender Identifizierung den Nicknamen nicht killen
	(ist von der KILL Option unabhängig).

NICK_HELP_SET_PRIVATE
	Syntax: SET PRIVATE {ON | OFF}
	
	Ändert %S's Privat-Modus für deinen Nicknamen.
	Mit eingeschaltetem PRIVATE wird dein Nickname
	nicht auf Listen erscheinen, die durch %S's LIST 
	Befehls erstellt wurden.
	
	(Jeder der deinen Nicknamen kennt, kann gezielt mit 
	dem INFO Befehl Informationen über deinen Nicknamen 
	abrufen, unabhängig vom Privat-Modus.)

NICK_HELP_SET_HIDE
	Syntax: SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	Erlaubt es dir, bestimmte Informationen in der Ausgabe
	von %S INFO zu verstecken.
	Du kannst deine eMail-Adresse verstecken (EMAIL), die 
	zuletzt gesehene user@host-Maske (USERMASK), dein
	Services-Zugriffsstatus (STATUS) und die letzte Quit-Nachricht
	(QUIT).
	Der zweite Parameter bestimmt, ob die Information angezeigt
	werden soll (OFF) oder nicht (ON).
	
NICK_HELP_SET_MSG
	Syntax: SET MSG {ON | OFF}
	
	Bestimmt den Modus, mit dem sich die Services mit dir
	unterhalten. 
	Ist MSG eingeschaltet, benutzen die Services Messages
	(Queries), ansonsten werden dir die Antworten als Notices
	geschickt (Standard).

NICK_HELP_SET_AUTOOP
	Syntax: SET AUTOOP {ON | OFF}
	
	Bestimmt ob Du automatisch Op-Status erhälst. Auf ON gestellt
	erlaubt es ChanServ Dir automatisch Op-Status zu geben,
	wenn Du die Channels betrittst.

NICK_HELP_SASET
	Syntax: SASET nickname option parameters.
	
	Setzt verschiedene Einstellungen für den Nicknamen.  Gültig für option ist:
	
	    DISPLAY    Setzt einen Nicknamen als Ursprung der Nickgruppe
	    PASSWORD   Setzt das Passwort eines Nicknamens
	    URL        Fügt eine URL dem Nicknamen hinzu
	    EMAIL      Fügt eine Emailadresse dem Nicknamen hinzu
	    ICQ        Fügt eine ICQ Nummer dem Nicknamen hinzu
	    GREET      Fügt eine Begrüssungsnachricht dem Nicknamen hinzu
	    KILL       Schützt den Nicknamen durch Identifikationszwang
	    SECURE     Schaltet die Identifikationssicherheitsfunktion 
	               an oder aus
	    PRIVATE    Verhindert, dass der Nickname im Befehl
	               %R%S LIST auftaucht
	    HIDE       Versteckt bestimmte, dem Nicknamen zugeordnete, 
	               Informationen
	    MSG        Bestimmt die Art wie die Services Nachrichten senden
	    NOEXPIRE   Verhindert, dass der Nickname nach einer bestimmten
	               Zeit verfällt
	    LANGUAGE   Stellt die Sprache ein, in der die Services
	               dem Nicknamen antworten sollen.
	    AUTOOP     Sollten die Services dem Nick automatisch seinen
	               Op-Status geben.
	
	Mit %R%S HELP SASET option können mehr Informationen zu 
	den Einstellungen angefordert werden. Die Einstellungen betreffen den
	angegebenen Nicknamen. 
	
	Die Befehle sind nur für Services Admins verfügbar.

NICK_HELP_SASET_DISPLAY
	Syntax: SASET nickname DISPLAY new-display
	
	Ändert den Nicknamen der als Ursprung einer Nicknamengruppe
	angezeigt wird. Der neue Ursprung muss ein Nickname der 
	entsprechenden Gruppe sein.

NICK_HELP_SASET_PASSWORD
	Syntax: SASET nickname PASSWORD new-password
	
	Ändert das Passwort um sich als Besitzer eines Nicknamens
	zu identifizieren.
	
NICK_HELP_SASET_URL
	Syntax: SASET nickname URL url
	
	Verknüpft eine URL mit dem Nicknamen. Diese URL wird
	angezeigt wenn jemand mit dem INFO Befehl nähere 
	Informationen zu dem Nicknamen abruft.

NICK_HELP_SASET_EMAIL
	Syntax: SASET nickname EMAIL address
	
	Verknüpft die angegeben Emailadresse mit dem
	Nicknamen.
	
NICK_HELP_SASET_ICQ
	Syntax: SASET nickname ICQ number
	
	Verknüpft die angegebene ICQ Nummer mit dem Nicknamen.
	Diese ICQ Nummer wird angezeigt wenn jemand mit dem 
	INFO Befehl nähere Informationen zu dem Nicknamen abruft.
	
NICK_HELP_SASET_GREET
	Syntax: SASET nickname GREET message
	
	Setzt die angegebene Nachricht als Begrüssungsnachricht
	für den Nicknamen. Diese wird angezeigt, wenn der Nickname
	den Channel betritt und die GREET Einstellung aktiviert
	ist. Vorraussetzung ist, dass der Benutzer die entsprechenden
	Rechte besitzt.

NICK_HELP_SASET_KILL
	Syntax: SASET nickname KILL {ON | QUICK | IMMED | OFF}
	
	Stellt den automatischen Schutz des Nicknames durch
	einen Kill an oder aus. Nimmt ein User diesen Nickname, 
	hat er eine Minute Zeit sich zu identifizieren,
	danach wird er, bei eingeschaltetem Schutz, durch
	%S umbenannt in einen Zufallsnamen.
	
	Bei der Einstellung QUICK wird Wartezeit zum
	Identifizieren auf 20 Sekunden herabgesenkt. Bei
	IMMED wird der User sofort ohne Warnung
	umbenannt, wenn er nicht auf der Zugriffsliste des
	Nicknamens steht. Bitte benutze diese Option nicht
	unnötigerweise.
	Ausserdem ist es möglich, dass die Netz-Admins
	diese Option deaktiviert haben.

NICK_HELP_SASET_SECURE
	Syntax: SASET nickname SECURE {ON | OFF}
	
	Schaltet die Sicherheitsfunktionen von %S für den
	Nicknamen an oder aus. Mit eingeschaltetem SECURE
	muss sich der Nickname immer identifizieren, egal 
	ob seine Hostmaske auf der Zugriffsliste für den 
	Nicknamen steht oder nicht. Wenn er allerdings auf 
	der Zugriffsliste stehst, wird %S bei 
	fehlender Identifizierung den Nicknamen nicht killen
	(ist von der KILL Option unabhängig).

NICK_HELP_SASET_PRIVATE
	Syntax: SASET nickname PRIVATE {ON | OFF}
	
	Ändert %S's Privat-Modus für den Nicknamen.
	Mit eingeschaltetem PRIVATE wird der Nickname
	nicht auf Listen erscheinen, die durch %S's LIST 
	Befehl erstellt wurden.
	
	(Jeder der den Nicknamen kennt, kann gezielt mit 
	dem INFO Befehl Informationen über den Nicknamen 
	abrufen, unabhängig vom Privat-Modus.)

NICK_HELP_SASET_HIDE
	Syntax: SASET nickname HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}

	Erlaubt es beim angegebenen Nicknamen , bestimmte Informationen 
	in der Ausgabe von %S INFO zu verstecken.
	Es kann die eMail-Adresse versteckt (EMAIL) werden, die 
	zuletzt gesehene user@host-Maske (USERMASK), der
	Services-Zugriffsstatus (STATUS) und die letzte Quit-Nachricht
	(QUIT).
	Der zweite Parameter bestimmt, ob die Information angezeigt
	werden soll (OFF) oder nicht (ON).
	
NICK_HELP_SASET_MSG
	Syntax: SASET nickname MSG {ON | OFF}
	
	Bestimmt die Art, mit dem sich die Services mit dem 
	angebenen Nicknamen unterhalten. 
	Ist MSG eingeschaltet, benutzen die Services Messages
	(Queries), ansonsten werden die Antworten als Notices
	geschickt (Standard).

NICK_HELP_SASET_NOEXPIRE
	Syntax: SASET nickname NOEXPIRE {ON | OFF}
	
	Bestimmt ob ein Nickname nach einer gewissen Zeit 
	verfällt. Wird NOEXPIRE auf ON gesetzt verfällt
	der Nickname nie.

NICK_HELP_SASET_AUTOOP
	Syntax: SASET nickname AUTOOP {ON | OFF}
	
	Bestimmt ob der angegebene Nickname automatisch Op-Status erhält.
	Wird AUTOOP auf ON gestellt erlaubt es ChanServ den
	angegebenen Nicknamen automatisch Op-Status zu geben,
	wenn der Nick die Channels betritt.

NICK_HELP_SASET_LANGUAGE
	Syntax: SASET nickname LANGUAGE number
	
	Wechselt die Sprache der Services, wenn Meldungen an den
	Nicknamen gesendet werden.
	(Zum Beispiel in einer Antwort auf einen Befehl den der User gesendet hat).
	Die Nummer sollte aus der Liste der unterstützten Sprachen gewählt 
	werden, die man mit tippen des Befehls %R%S HELP SET LANGUAGE bekommt.

NICK_HELP_RECOVER
	Syntax: RECOVER Nickname [Passwort]
	
	Ermöglicht dir, deinen Nicknamen wieder freizugeben,
	wenn jemand anderes ihn angenommen hat. Diese Funktion
	wird auch durch %S genutzt, wenn jemand einen Nickname
	mit Kill-Schutz anzunehmen.
	
	Bei Aufruf dieses Befehls wird %S einen virtuellen
	User mit dem gleichen Nick online schalten den du
	recovern willst. Die Folge davon ist, dass der User,
	der deinen Nickname angenommen hatte, vom Server getrennt
	wird. Der virtuelle User von den Services wird noch
	%s online bleiben, um sicherzustellen, dass der
	andere User nicht sofort neu connected; Nach dieser
	zeit wird der Nickname wieder freigegeben und du kannst
	ihn annehmen.
	
	Als Alternative gibt es das RELEASE Befehl,
	(%R%S HELP RELEASE) um deinen Nickname 
	schneller zurückzubekommen.
	
	Als Vorraussetzung für das RECOVER Befehl muss 
	deine aktuelle Adresse (siehe Ausgabe von /WHOIS)
	auf der Zugriffsliste des zu recovernden Nicknamens 
	oder in dessen Gruppe sein. Als dritte Möglichkeit
	kannst du das Passwort für den Nicknamen angeben.

NICK_HELP_RELEASE
	Syntax: RELEASE Nickname [Passwort]
	
	Durch %S wird die Nickname-Sperre, ausgelöst
	durch z.B. den automatischen Kill-Schutz oder durch
	Verwendung des RECOVER Befehls, aufgehoben.
	Standardmässig hält diese Sperre %s, dieser
	Befehl setzt den Nickname früher wieder frei.
	
	Als Vorraussetzung für die Verwendung des RELEASE 
	Befehl für einen Nickname, muss deine aktuelle Hostmaske
	(siehe /WHOIS) in der Zugriffsliste des entsprechenden
	Nicknamens oder in der Gruppe des Nicknamen sein. Weiterhin 
	musst du das richtige Passwort für den Nicknamen haben.

NICK_HELP_GHOST
	Syntax: GHOST Nickname [Passwort]
	
	Trennt eine "geisternde" IRC Verbindung, die deinen
	Nickname besitzt. Eine solche Verbindung ist eine,
	die nicht wirklich verbunden ist, bzw. im Begriff ist
	getrennt zu werden. Normalerweise passiert dies,
	wenn dein Computer abstürtzt oder deine Internet-
	Verbindung zusammenbricht, wenn du im IRC bist.
	
	Wenn du das GHOST Befehl benutzen willst,muss 
	deine aktuelle Hostmaske (siehe /WHOIS) in der
	Zugriffsliste des entsprechenden Nicknamens oder
	in der Gruppe des Nicknamens sein.Weiterhin musst 
	du das richtige Passwort für den Nickname haben.

NICK_HELP_INFO
	Syntax: INFO Nickname [ALL]
	
	Zeigt Informationen über den angegebenen Nickname, zum
	Beispiel den Besitzer des Nicknamens, die letzte gesehene
	Adresse und dessen Uhrzeit und die Optionen des Nicknamens.
	Wenn du dich für den Nickname über den du Infos anzeigen
	willst identifiziert hast, und den Parameter ALL 
	benutzt, werden alle Informationen angezeigt.
	(Egal ob sie versteckt sind oder nicht.)

NICK_HELP_LIST
	Syntax: LIST Muster
	
	Listet alle registrierten Nicknamen auf, die auf das
	Suchmuster passen, im Format nickname!user@host.
	Nicknamen mit aktivierter PRIVATE Option werden 
	allerdings nicht angezeigt.
	
	Beispiele:
	
	    LIST *!joeuser@foo.com
	        Zeigt alle Nicknamen an, deren Besitzer joeuser@foo.com
	        ist.
	
	    LIST *Bot*!*@*
	        Zeigt alle registrierten Nicknamen mit Bot in ihrem
	        Ident (Gross/Kleinschreibung wird nicht unterschieden).
	
	    LIST *!*@*.bar.org
	    Listet alle Nicknamen von Usern in der bar.org Domain.

NICK_HELP_ALIST
	Syntax: ALIST [Level]
	
	Listet alle Channels auf, auf denen du Zugriff hast. Optional kannst du
	das Level-Format (XOP oder ACCESS) bestimmen. Die daraus resultierende 
	liste wird nur Channels enthalten, wo du den entsprechenden 
	Zugriffslevel hast.
	
	Beispiele:
	
	    ALIST Founder
	        Listet alle Channels auf, in denen du Founder-Access hast.
	
	    ALIST AOP
	        Listet alle Channels auf, in denen du AOP-Access oder höher
	        hast.
	
	    ALIST 10
	        Listet alle Channels auf, in denen du das Zugriffs-Level 10 
	        oder höher hast..
	
	Channels mit der NOEXPIRE Option sind mit einem Ausrufezeichen markiert.

NICK_HELP_GLIST
	Syntax: GLIST
	
	Zeigt alle Nicknamen die in deiner Gruppe enthalten sind.
	
NICK_HELP_STATUS
	Syntax: STATUS Nickname
	
	Liefert einen Wert zurück, der besagt, ob der augenblickliche
	Nutzer des Nicknamens der identifizierte Eigentümer ist, oder 
	nicht.Die Antwort hat das Format:
	
	    Nickname Status-Code
	
	wobei Nickname der Nickname ist, der mit dem Befehl angefragt
	wurde und Status-Code eine der folgenden Werte:
	
	    0 - User ist nicht online oder Nickname ist nicht registriert
	    1 - User wird nicht als Eigentümer erkannt
	    2 - User wird als Eigentümer erkannt durch die
	        Zugriffsliste
	    3 - User wird als Eigentümer erkannt durch die
	        Identifizierung mit Passwort
	
	Bis zu 16 Nicknamen können mit dem Befehl übergeben werden,
	der Rest wird ignoriert. Wenn kein Nickname angegeben wird,
	wird dein STATUS angezeigt.

NICK_HELP_SENDPASS
	Syntax: SENDPASS Nickname
	
	Sendet das Passwort des angegebenen Nicks zu der damit
	verbundenen eMail-Adresse aus der NickServ-Datenbank.
	Dieses Kommando ist wirklich nützlich, wenn du dein
	Passwort vergessen haben solltest.
	
	Kann in manchen Netzen auf IRC Operatoren begrenzt
	sein.
	
	Dieses Kommando ist nicht verfügbar, wenn die Datenbanken
	verschlüsselt angelegt sind.

NICK_HELP_CONFIRM
	Syntax: CONFIRM passcode
	
	Dies ist der zweite Schritt beim Registrieren des Nicknamens.
	Du mußt diesen Befehl ausführen, um ihn mit %S zu registrieren.
	Der Passcode (oder auch Authentifizierungscode genannt) wird
	an Deine email-adresse gesendet beim ersten Schritt des
	Registrationsprozesses. Für mehr Informationen über die erste
	Stufe des Registrationsprozesses tippe: %R%S HELP REGISTER

NICK_HELP_RESEND
	Syntax: RESEND
	
	Dieser Befehl löst das Versenden des Authentifizierungscodes
	(auch Passcode genannt) erneut an die gespeicherte Emailadresse
	des Users, sobald der User ihn ausführt.

NICK_SERVADMIN_HELP
	Services Administratoren können auch registrierte Nicknamen
	löschen,ohne sich für den jeweiligen Nicknamen identifizieren
	zu müssen. Zusätzlich können sie die Zugriffsliste eines
	jeden Nicknamen ansehen und bearbeiten. Siehe :
	%R%S ACCESS LIST Nickname

NICK_SERVADMIN_HELP_LOGOUT
	Syntax: LOGOUT [Nickname [REVALIDATE]]
	
	Ohne angegebenen Parameter, macht dieser Befehl die
	Wirkung von IDENTIFY rückgängig, zum Beispiel
	wirst du nicht mehr als Eigentümer des Nicknamens erkannt.
	Beachte jedoch, dass du nicht erneut zur Identifizierung
	aufgefordert wirst.
	
	Mit angegebenem Parameter passiert das gleiche für den
	Nicknamen. Wenn du zusätzlich REVALIDATE angibst, wird
	Services dich auffordern, dich erneut für den Nickname
	zu identifizieren. 
	
	Diese Funktion ist beschränkt auf die Services Administartoren.
	
NICK_SERVADMIN_HELP_DROP
	Syntax: DROP [Nickname]
	
	Ohne angegebenen Parameter entfernt es deinen Nicknamen
	aus der Datenbank von %S.
	
	Mit dem Nicknamen als Parameter, wird dieser Nickname
	von der Datenbank entfernt. Du kannst jeden Nickname 
	in deiner Gruppe ohne bestimmte Privilegien entfernen.
	
	Das Entfernen eines beliebigen Nicknamen ist beschränkt
	auf Services Administratoren.

NICK_SERVADMIN_HELP_INFO
	
	Services Administratoren können den ALL Parameter für
	jeden beliebigen Nickname benutzen.

NICK_SERVADMIN_HELP_LIST
	Syntax: LIST Muster [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	Zeigt alle registrierten Nicknamen an, die einem bestimmten
	Muster, nickname!user@host, entsprechen. Nicknamen mit 
	aktivierter PRIVATE und NOEXPIRE Option werden nur 
	Services Administratoren angezeigt. Nicknamen bei denen das 
	NOEXPIRE eingeschaltet ist, werden ein ! vor ihrem
	Eintrag in der Liste haben.
	
	Wenn die Option FORBIDDEN, SUSPENDED, NOEXPIRE oder UNCONFIRMED
	angegeben wird, werden nur Nicknamen angezeigt die das entsprechene
	Muster haben (FORBIDDEN, SUSPENDED, UNCONFIRMED oder NOEXPIRE). 
	Wenn mehr Optionen angegeben werden, werden alle Nicknamen angezeigt
	die den Muster entsprechen. 
	
	Diese optionen ist beschränkt auf Services Administratoren.
	
	Beispiele:
	
	    LIST *!joeuser@foo.com
	        Zeigt alle Nicknamen die von joeuser@foo.com
	        besessen werden.
	
	    LIST *Bot*!*@*
	        Listet alle registrierten Nicknamen die Bot in
	        ihrem Ident haben (unabhängig von Gross/Klein-
	        schreibung).
	
	    LIST * NOEXPIRE
	        Zeigt alle registrierten Nicknamen an, die das NOEXPIRE
	        Flag gesetzt haben.

NICK_SERVADMIN_HELP_ALIST
	Syntax: ALIST [Nickname] [Level]
	
	Mit keinen Parametern werden alle Channels aufgelistet, auf
	die du Zugriff hast. Mit einem Parameter werden alle Channels
	aufgelistet, auf die Nickname Zugriff hat. Mit zwei 
	Parametern werden alle Channel aufgelistet, in denen Nickname 
	Zugriffslevel Level oder höher hat.

NICK_SERVADMIN_HELP_GLIST
	Syntax: GLIST [Nickname]
	
	Wird kein Parameter angegeben, werden alle Nicknamen
	in deiner Gruppe angezeigt.
	
	Ansonsten werden alle Nicknamen, die in der Gruppe des
	angegebenen Nicknamens sind, aufgelistet.

NICK_SERVADMIN_HELP_GETPASS
	Syntax: GETPASS Nickname
	
	Liest das Passwort von Nickname aus der Datenbank aus.
	Beachte dass wenn jemand dieses Befehl benutzt,
	diese Aktion im Logbuch der Services vermerkt wird.
	Zusätzlich wird eine Nachricht an alle IRC Operatoren
	per WALLOPS/GLOBOPS ausgesandt.
	
	Dieser Befehl ist nicht verfügbar bei aktivierter
	Verschlüsselung.

NICK_SERVADMIN_HELP_GETEMAIL
	Syntax: GETEMAIL user@emailhost

	Gibt die passenden Nicknamen an, welche die angegebene eMail benutzen. 
	Beachte, dass du keine wildcards für den User oder emailhost
	verwenden kannst. Immer, wenn dieser Befehl benutzt wird, wird
	eine Nachricht mit dem Nickname der Person mitgeloggt.

NICK_SERVADMIN_HELP_FORBID
	Syntax: FORBID Nickname [Grund]
	
	Verhindert, dass der angegebene Nickname registriert
	oder genutzt wird. Diese Einschränkung kann durch
	DROPEN (löschen) des Nicknamen aufgehoben werden.
	
	In bestimmten Netzen wird ein Grund für das
	Verbot benötigt.

NICK_SERVADMIN_HELP_SUSPEND
	Syntax: SUSPEND Nickname Grund

	SUSPEND verhindert das der angegebene Nickname sich identifizieren kann.

NICK_SERVADMIN_HELP_UNSUSPEND
	Syntax: UNSUSPEND Nickname

	Gibt einen Nicknamen der SUSPENDED ist wieder frei.

###########################################################################
#
# ChanServ help messages
#
###########################################################################
CHAN_HELP_CMD_GETPASS
	    GETPASS    Liest das Passwort des Channel-Founders
	               aus der Datenbank aus (wenn die
	               Verschlüsselung deaktiviert ist)
CHAN_HELP_CMD_FORBID
	    FORBID     Verhindert die Nutzung eines Channels
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    Verbietet jegliche Nutzung des Channels.
	               Die Channel Einstellungen bleiben jedoch
	               erhalten
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Hebt ein SUSPEND wieder auf
CHAN_HELP_CMD_STATUS
	    STATUS     Liefert das Zugriffslevel eines Users
	               in dem Channel
CHAN_HELP_CMD_REGISTER
	    REGISTER   Einen Channel registrieren
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   Sich mit Passwort als Founder (z.B.)
	               des Channels identifizieren
CHAN_HELP_CMD_SET
	    SET        Bestimmte Channel-Optionen und
	               Informationen einstellen
CHAN_HELP_CMD_AOP
	    AOP        Verwaltet die AOP-Liste von einen Channel
CHAN_HELP_CMD_SOP
	    SOP        Verwaltet die SOP-Liste von einen Channel
CHAN_HELP_CMD_ACCESS
	    ACCESS     Die Liste der priviligierten User
	               bearbeiten
CHAN_HELP_CMD_LEVELS
	    LEVELS     Die Einstellungen verschiedener
	               Zugriffslevel neu definieren
CHAN_HELP_CMD_AKICK
	    AKICK      Die Autokick-Liste bearbeiten
CHAN_HELP_CMD_DROP
	    DROP       Registrierung eines Channels aufheben
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Hilft bei vergessenen Passwörtern
CHAN_HELP_CMD_BAN
	    BAN        Bant den angegebenen Nicknamen aus einen Channel
CHAN_HELP_CMD_CLEAR
	    CLEAR      Entfernt bestimmte Einstellungen des angegebenen Channel
CHAN_HELP_CMD_DEVOICE
	    DEVOICE    Enfernt den Voice-Status des angegebenen Nicknamens
	               in einem Channel
CHAN_HELP_CMD_GETKEY
	    GETKEY     Gibt das Channel-Passwort wieder
CHAN_HELP_CMD_INFO
	    INFO       Zeigt Informationen über den angegebenen Channel
CHAN_HELP_CMD_INVITE
	    INVITE     Befiehlt Chanserv Dich in einen Channel einzuladen
CHAN_HELP_CMD_KICK
	    KICK       Kickt den angegebenen Nicknamen aus einen Channel
CHAN_HELP_CMD_LIST
	    LIST       Listet alle registrierten Channels des angegebene Muster
CHAN_HELP_CMD_LOGOUT
	    LOGOUT     Meldet den angegebenen Nicknamen ab
CHAN_HELP_CMD_OP
	    OP         Gibt dem angegebenen Nicknamen Op-Status in einem Channel
CHAN_HELP_CMD_TOPIC
	    TOPIC      Ändert den Topic des angegebenen Channels
CHAN_HELP_CMD_UNBAN
	    UNBAN      Enfernt den oder die Bann(s) die Dich betreffen aus einem Channel
CHAN_HELP_CMD_VOICE
	    VOICE      Gibt dem angegebenen User Voice-Status in einem Channel
CHAN_HELP_CMD_VOP
	    VOP        Verwaltet die VOP-Liste eines Channels
CHAN_HELP_CMD_DEHALFOP
	    DEHALFOP   Enfernt den HalfOP-Status des angegebenen Nicknamens
	               in einem Channel
CHAN_HELP_CMD_DEOWNER
	    DEOWNER    Entfernt Deinen Founder (Gründer) Status in einem Channel
CHAN_HELP_CMD_DEPROTECT
	    DEPROTECT  Entfernt den Schutz des angegebenen Nicknamens 
	               in einem Channel
CHAN_HELP_CMD_HALFOP
	    HALFOP     Gibt dem angegebenen User HalfOP-Status in einem Channel
CHAN_HELP_CMD_HOP
	    HOP        Verwaltet die HOP-Liste von einen Channel
CHAN_HELP_CMD_OWNER
	    OWNER      Gibt Dir Founder (Gründer) Status in einem Channel
CHAN_HELP_CMD_PROTECT
	    PROTECT    Schützt den angegebenen Nicknamen in einem Channel
CHAN_HELP_CMD_ADMIN
	    ADMIN      Schützt den angegebenen Nicknamen in einem Channel
CHAN_HELP_CMD_DEADMIN
	    DEADMIN    Entfernt den Schutz des angegebenen Nicknamens 
	               in einem Channel
CHAN_HELP_CMD_DEOP
	    DEOP       Enfernt den OP-Status des angegebenen Nicknamens
	               in einem Channel

CHAN_HELP
	Mit Hilfe von %S kann man Channel registrieren und
	eine Vielzahl von Kontrolloptionen einstellen. %S kann 
	zum Beispiel das "Takeover" von Channels verhindern,in 
	dem man die Channel-Op Privilegien auf bestimmte registrierte 
	Nicknamen begrenzt. Verfügbare Befehle sind unten aufgelistet. 
	Um sie zu benutzen tippe %R%S Befehl. Für weitere 
	Informationen zu einem bestimmten Befehl, tippe :
	%R%S HELP Befehl.
	 
	
CHAN_HELP_EXPIRES
	
	Beachte das jeder Channel der für %d Tage nicht
	genutzt wird,automatisch auslaufen wird.

CHAN_HELP_REGISTER
	Syntax: REGISTER Channel Password Beschreibung
	
	Registriert einen Channel in der Datenbank von %S.
	Wenn du dieses Befehl nutzen möchtest, musst du
	Operator-Status in dem Channel besitzen. Das
	Passwort, dass du mit dem IDENTIFY Befehl
	einstellst, erlaubt es später anderen Änderungen 
	am Channel durchzuführen.Der letzte Parameter, der 
	angegeben werden MUSS,sollte eine allgemeine 
	Beschreibung des Channels bzw dessen Zweck beinhalten.
	
	Wenn du einen Channel registrierst, wird dein Nickname
	als Founder (Gründer) eingetragen. Der Channel-Founder 
	kann alle Einstellungen des Channels bearbeiten, und 
	wird automatisch beim Joinen des Channels durch %S mit 
	Founder/Operator Privilegien ausgestattet.  Mit Hilfe 
	des ACCESS Befehls (%R%S HELP ACCESS) kann man 
	einen Teil dieser Privilegien anderen registrierten 
	Nicknamen ebenfalls erteilen.
	
	Hinweis: Für das Registrieren eines Channels musst du 
	einen registrierten und identifizierten Nickname benutzen. 
	Hast du dies noch nicht getan, tippe %R%s HELP REGISTER
	um weitere Informationen darüber zu erhalten.

CHAN_HELP_IDENTIFY
	Syntax: IDENTIFY Channel Passwort
	
	Identifiziert ein Nickname gegenüber %S als 
	Founder (Gründer) des angegebenen Channels. 
	Das Passwort muss dasselbe wie bei der 
	registrierung des angegebenen Channels sein
	und ist in der Regel von den Real-Founder des
	Channels zu erhalten.
	
CHAN_HELP_LOGOUT
	Syntax: LOGOUT Channel Nickname
	
	Hebt die Identifizierung als Channel-Founder für
	den angegebenen Channel wieder auf.
	
	Wenn du der Channel-Founder bist, kannst du jeden
	ausloggen, jeder andere nur sich selbst.

CHAN_HELP_DROP
	Syntax: DROP Channel
	
	Entfernt die Registrierung des angegebenen Channels.
	Kann nur vom Channel-Founder benutzt werden.

CHAN_HELP_SET
	Syntax: SET Channel Option Parameter
	
	Erlaubt es dem Channel-Founder bestimmte Optionen
	und Informationen des Channels zu ändern.
	
	Verfügbare Optionen:
	
	    FOUNDER       Stellt den Founder (Gründer) des Channels ein
	    SUCCESSOR     Stellt den Vertreter des Founders ein
	    PASSWORD      Ändert das Passwort des Channel-Founders
	    DESC          Ändert die Channel-Beschreibung
	    URL           Verbindet eine URL mit dem Channel
	    EMAIL         Verbindet eine eMail-Adresse mit dem
	                  Channel
	    ENTRYMSG      Stellt eine Begrüssungsmeldung des
	                  Channels ein
	    TOPIC         Ändert das Topic im Channel
	    BANTYPE       Ändert den Ban-Typ, den die Services
	                  in dem Channel anwenden
	    KEEPTOPIC     Topic behalten, wenn Channel nicht
	                  benutzt wird
	    TOPICLOCK     Topic kann nur durch TOPIC geändert werden
	    MLOCK         Channel-Modes fest einstellen
	    PRIVATE       Den Channel vom /LIST-Befehl verstecken
	    RESTRICTED    Eingeschränkter Zugriff auf den Channel
	    SECURE        Aktiviert %S's Sicherheits-funktionen
	    SECUREOPS     Strengere Kontrolle des Op-Status im Channel
	    SECUREFOUNDER Strengere Kontrolle des Founder-Status
	    SIGNKICK      Kicks durch ChanServ in dem Channel
	                  werden durch den Kicker signiert
	    OPNOTICE      Sendet eine Notice wenn Op/Deop-Befehl
	                  benutzt werden
	    XOP           Schaltet das Listen System ein oder aus
	
	Tippe %R%S HELP Option für weitere Informationen
	zu einem bestimmten Befehl.

CHAN_HELP_SET_FOUNDER
	Syntax: SET Channel FOUNDER Nickname
	
	Ändert den Gründer (Founder) eines Channels. Der neue
	Nickname muss registriert sein.

CHAN_HELP_SET_SUCCESSOR
	Syntax: SET Channel SUCCESSOR Nickname
	
	Ändert den Vertreter des Founders (Gründer) in dem Channel.
	Wenn der Nickname des Founders ausläuft oder gedropped wird,
	wenn der Channel registriert ist, wird der Vertreter
	der neue Founder des Channels. Wenn jedoch der 
	Vertreter bereits zu viele Channels registriert hat
	(max %d), wird der Channel gedropped.
	
	Der Nickname des Vertreters muss registriert sein.

CHAN_HELP_SET_PASSWORD
	Syntax: SET Channel PASSWORD Passwort
	
	Ändert das Passwort, dass dich als Founder (Gründer)
	des Channels identifiziert.

CHAN_HELP_SET_DESC
	Syntax: SET Channel DESC Beschreibung
	
	Ändert die Beschreibung für einen Channel, die bei
	dem LIST und INFO Befehl angezeigt wird.

CHAN_HELP_SET_URL
	Syntax: SET Channel URL [Url]
	
	Verbindet die angegebene URL mit dem Channel. Diese
	URL wird angezeigt, wenn jemand Informationen
	über den Channel mit dem INFO Befehl abfragt.
	Wird kein Parameter angegeben, wird die aktuelle
	URL gelöscht.

CHAN_HELP_SET_EMAIL
	Syntax: SET Channel EMAIL [eMail-Adresse]
	
	Verbindet die angegebene eMail-Adresse mit dem
	Channel. Diese eMail-Adresse wird angezeigt,
	wenn jemand Informationen über den Channel mit
	dem INFO Befehl abfragt.Wird kein Parameter 
	angegeben, wird die aktuelle eMail-Adresse gelöscht.

CHAN_HELP_SET_ENTRYMSG
	Syntax: SET Channel ENTRYMSG [Nachricht]
	
	Ändert die Nachricht, die User beim Joinen des
	Channels per /notice bekommen. Wird kein Parameter
	angegeben, wird keine Nachricht beim Joinen 
	angezeigt.
	
CHAN_HELP_SET_BANTYPE
	Syntax: SET Channel BANTYPE Ban-Typ
	
	Stellt den Ban-Typ für den Channel ein, den die
	Services anwenden, wenn sie einen Ban eintragen
	(z.B. bei einem Auto-Kick).
	
	bantype muss einer der folgenden Werte sein:
	
	0: Ban in der Form *!user@host
	1: Ban in der Form *!*user@host
	2: Ban in der Form *!*@host
	3: Ban in der Form *!*user@*.domain

CHAN_HELP_SET_KEEPTOPIC
	Syntax: SET Channel KEEPTOPIC {ON | OFF}
	
	Aktiviert oder deaktiviert die Topic Wiederherstellung für
	den Channel. Wird diese Option eingeschaltet, wird das 
	Topic des Channels gespeichert durch %S. Selbst wenn 
	der letzte User den Channel verlässt, wird es 
	wiederhergestellt sobald jemand wieder in den Channel 
	kommt.

CHAN_HELP_SET_TOPICLOCK
	Syntax: SET Channel TOPICLOCK {ON | OFF}
	
	Aktiviert oder deaktiviert die topic lock-Option
	für den angegebenen Channel.
	Wenn topic lock eingeschaltet ist, wird %S jede
	Änderung des Topics verhindern/rückgängig machen,
	ausser es wurde durch TOPIC eingestellt.

CHAN_HELP_SET_MLOCK
	Syntax: SET Channel MLOCK Modi
	
	Stellt den Mode-Lock-Parameter für den angegebenen
	Channel ein. %S ermöglicht es dir, einzustellen,
	dass bestimmte Modi immer aus bzw an sein müssen.
	
	Die Modes die gelockt werden können sind von dein IRCD abhängig.
	Jede einzelne dieser Modes kann ein/aus oder gar nicht gelockt werden. 
	Der Modi Parameter ist genauso aufgebaut wie das jeweilige /MODE Befehl.
	Dies bedeutet, Modi die durch ein + gekennzeichnet sind, sind aktiviert,
	durch ein - sind sie deaktiviert.
	
	Warnung:  Wenn du den Channel mit Hilfe des SET MLOCK
	Befehls mit einem Key (Passwort) schützt, solltest du
	zusätzlich die RESTRICTED Option für den Channel ein-
	schalten (siehe HELP SET RESTRICTED), oder jeder der in
	diesen Channel kommt wenn er leer ist kann den Key sehen!
	
	Beispiele:
	
	    SET #channel MLOCK +nt-iklps
	        Erzwingt die Channel-Modi n und t auf an, die i,
	        k,l,p und s aus. Mode m wird nicht explizit gelockt
	        und kann wahlweise an oder ausgeschaltet werden.
	
	    SET #channel MLOCK +knst-ilmp mein-key
	        Erzwingt die Modi k, n, s und t an, die Modi i, l, m,
	        und p aus.  Weiterhin wird der Channel-Key fest auf
	        "mein-key" eingestellt.
	
	    SET #channel MLOCK +
	        Entfernt alle gelockten Channel-Modi, jeder Modus
	        kann jetzt wahlweise an oder ausgeschaltet werden.

CHAN_HELP_SET_PEACE
	Syntax: SET Channel PEACE {ON | OFF}

	Aktiviert oder deaktiviert die peace Option. Wenn
	peace aktiviert ist, kann ein User einen anderen User
	mit gleichem oder höherem Zugriffslevel nicht mehr per 
	%S  kicken, bannen oder dessen Channelstatus löschen.

CHAN_HELP_SET_PRIVATE
	Syntax: SET Channel PRIVATE {ON | OFF}
	
	Schaltet den Privat-Modus für einen Channel an
	oder aus. Wird er aktiviert, wird der Channel in der
	Ausgabe von%R%S LIST nicht mehr auftauchen.

CHAN_HELP_SET_RESTRICTED
	Syntax: SET Channel RESTRICTED {ON | OFF}
	
	Aktiviert oder deaktiviert den restricted access
	(beschränkter Zugriff)-Modi für einen Channel. Wenn 
	restricted access eingeschaltet ist, werden
	User die sonst keinen Channel Operator-Status besitzen
	würden (Nicknamen mit negativem Zugriffslevel, oder wenn
	Secure Ops aktiv ist, Nicknamen die nicht auf der
	Zugriffsliste sind) vom Channel gekicked und gebanned.

CHAN_HELP_SET_SECURE
	Syntax: SET Channel SECURE {ON | OFF}
	
	Aktiviert oder deaktiviert die Sicherheitsfunktionen
	von %S für den angegebenen Channel. 
	Wird SECURE eingeschaltet, werden nur registrierte
	Nicknamen (durch %s) die per Passwort 
	identifiziert sind in den Channel gelassen, so wie es 
	in der Zugriffsliste des Channels steht.

CHAN_HELP_SET_SECUREOPS
	Syntax: SET Channel SECUREOPS {ON | OFF}
	
	Ändert die Secure-Ops Option für einen Channel.
	Wird diese Option eingeschaltet, können Nicknamen, 
	die nicht auf der Zugriffsliste des Channels stehen, 
	keinen Op-Status erhalten.
	
CHAN_HELP_SET_SECUREFOUNDER
	Syntax: SET Channel SECUREFOUNDER {ON | OFF}
	
	Aktiviert oder deaktiviert die Secure Founder-Option
	für einen Channel.
	Wird diese Option eingeschaltet, kann nur der "echte"
	Gründer des Channels diesen DROPEN(löschen), das Passwort 
	und den Successor verändern und nicht derjenige, der sich 
	per IDENTIFY gegenüber %S als Gründer (Founder) identifiziert
	hat.
	
CHAN_HELP_SET_SIGNKICK
	Syntax: SET Channel SIGNKICK {ON | LEVEL | OFF}
	
	Aktiviert oder deaktiviert signierte Kicks für einen
	Channel. Ist SIGNKICK aktiviert, wird der Grund
	eines Kicks der durch %S KICK ausgeführt wurde, durch
	den Nicknamen des Kickers signiert.
	
	Wird LEVEL benutzt, werden Kicks durch Nicknamen
	deren Level grösser oder gleich dem SIGNKICK Level ist,
	nicht durch den Nicknamen signiert.
	Siehe %R%S HELP LEVELS für weitere Informationen.

CHAN_HELP_SET_XOP
	Syntax: SET Channel XOP {ON | OFF}
	
	Aktiviert oder deaktiviert das xOP Listen System für einen Channel.
	Wenn XOP aktiviert ist, musst du AOP/SOP/VOP
	Befehl benutzen, um Zugriffslevel festlegen zu können. Andernfalls
	musst du das ACCESS Befehl benutzen.
	
	Technische Bemerkung: Wenn du vom Access zum xOP System 
	wechselst, werden die Zugriffslevel geändert, folglich wirst du
	nicht die selben Werte finden, wenn du zu Access zurückwechselst.
	
	Du solltest nach einem Wechsel zum XOP System darauf achten, 
	dass die User das richtige Level besitzen, da das Umwechseln
	auf Vermutung basiert und nicht immer perfekt funktioniert. 
	Es ist nicht empfohlen nach einer Änderung der Levels das xOP
	System zu nutzen.
	
	Der Wechsel vom xOP System zum Access System fuktioniert
	jedoch fehlerfrei.

CHAN_HELP_SET_OPNOTICE
	Syntax: SET Channel OPNOTICE {ON | OFF}
	
	Aktiviert oder deaktiviert die Op-Notice Option für
	einen Channel.
	Wird Op-Notice eingeschaltet, sendet %S eine
	Notice an den Channel wenn OP oder DEOP Befehle
	benutzt werden.

CHAN_HELP_AOP
	Syntax: AOP Channel ADD Nickname
	        AOP Channel DEL {Nickname | entry-num | Liste}
	        AOP Channel LIST [Maske | Liste]
	        AOP Channel CLEAR
	
	Verwaltet die AOP (AutoOP) Liste eines Channels. Die 
	AOP Liste gibt Usern automatisch Operator Status in deinem
	und gibt ihnen das Recht die Befehle UNBAN und INVITE.
	zu nutzen, beim Betreten des Channels werden die Greet
	Messages dieser User angezeigt, usw.

	Der AOP ADD Befehl fügt den jeweiligen Nicknamen zur AOP-Liste
	hinzu.
	
	Der AOP DEL Befehl löscht den jeweiligen Nickname aus der
	AOP-Liste. Wenn mehrere Eintragsnummern angegeben werden, 
	werden diese Einträge gelöscht.  (Siehe Beispiel bei LIST.)

	
	Der AOP LIST Befehl zeigt die aktuelle AOP-Liste. Wenn
	Wildcard Masks angegeben werden, werden nur die betroffenen 
	Einträge angezeigt. Werden mehrere Eintragsnummern angegeben,
	werden nur diese Einträge angezeigt, zB:

	   AOP #channel LIST 2-5,7-9
	      Listet die AOP Einträge 2 bis 5 und 7 bis 9.
	      
	Der AOP CLEAR Befehl löscht alle Einträge der AOP-Liste.
	
	AOP ADD und AOP DEL  sind auf SOPs beschränkt, während
	AOP CLEAR nur vom Channel Founder genutzt werden kann.
	Jeder User auf der AOP Liste kann den AOP LIST Befehl benutzen.
	
	Dieser Befehl kann für deinen Channel ausgeschaltet sein, in
	diesem Fall nutze bitte die Access Liste. Für weitere Infos zur
	Access Liste, tippe bitte %R%S HELP ACCESS und
	%R%S HELP SET XOP für Infos über den Wechsel vom XOP und 
	vom Access System.

CHAN_HELP_HOP
	Syntax: HOP Channel ADD Nickname
	        HOP Channel DEL {Nickname | entry-num | Liste}
	        HOP Channel LIST [Maske | Liste]
	        HOP Channel CLEAR
	
	Verwaltet die HOP (HalfOP) Liste eines Channels. User auf
	dieser Liste werden automatisch gehalfopt, wenn sie joinen.
	
	Der HOP ADD Befehl fügt den jeweiligen Nicknamen zur HOP-Liste
	hinzu.

	Der HOP DEL Befehl löscht den jeweiligen Nicknamen aus der
	HOP-Liste. Wenn mehrere Eintragsnummern angegeben werden,
	werden diese Einträge gelöscht.  (Siehe Beispiel bei LIST.)
	        
	
	Der HOP LIST Befehl zeigt die aktuelle HOP-Liste an. Wenn
	Wildcard Masks angegeben werden, werden nur die betroffenen
	Einträge angezeigt. Werden mehrere Eintragsnummern angegeben,
	werden nur diese Einträge angezeigt, zB:
	
	   HOP #channel LIST 2-5,7-9
	     Listet die HOP Einträge 2 bis 5 und 7 bis 9.

	Der HOP CLEAR Befehl löscht alle Einträge der HOP-Liste.

	HOP ADD, HOP DEL und HOP LIST sind auf AOPs oder
	höher beschränkt, während HOP CLEAR nur vom Founder genutzt
	werden kann.
	
	Dieser Befehl kann für deinen Channel ausgeschaltet sein, in  
	diesem Fall nutze bitte die Access Liste. Für weitere Infos zur
	Access Liste, tippe bitte %R%S HELP ACCESS und
	%R%S HELP SET XOP für Infos über den Wechsel vom xOP und
	vom Access System.
	
CHAN_HELP_SOP
	Syntax: SOP Channel ADD Nickname
	        SOP Channel DEL {Nickname | entry-num | Liste}
	        SOP Channel LIST [Maske | Liste]
	        SOP Channel CLEAR
	
	Verwaltet die SOP (SuperOP) Liste eines Channels. Ein User
	in der SOP-Liste besitzt die selben Rechte wie ein AOP und kann
	zusätzlich die AutoKick und BadWord Listen bearbeiten, Channel-
	memos lesen und senden, usw. 
	
	Der SOP ADD Befehl fügt den jeweiligen Nicknamen zur SOP-Liste
	hinzu.

	Der SOP DEL Befehl löscht den jeweiligen Nicknamen aus der
	SOP-Liste. Wenn mehrere Eintragsnummern angegeben werden,
	werden diese Einträge gelöscht.  (Siehe Beispiel bei LIST.)
	
	            
	Der SOP LIST Befehl zeigt die aktuelle SOP-Liste. Wenn
	Wildcard Masks angegeben werden, werden nur die betroffenen
	Einträge angezeigt. Werden mehrere Eintragsnummern angegeben,
	werden nur diese Einträge angezeigt, zB:

	   SOP #channel LIST 2-5,7-9
	      Listet die SOP Einträge 2 bis 5 und 7 bis 9.
	
	Der SOP CLEAR Befehl löscht alle Einträge der SOP-Liste.

	SOP ADD, SOP DEL und SOP CLEAR sind auf den Channel-
	founder beschränkt. SOP LIST kann von jedem AOP genutzt
	werden.
	
	Dieser Befehl kann für deinen Channel ausgeschaltet sein, in  
	diesem Fall nutze bitte die Access Liste. Für weitere Infos zur
	Access Liste, tippe bitte %R%S HELP ACCESS und
	%R%S HELP SET XOP für Infos über den Wechsel vom xOP und
	vom Access System.

CHAN_HELP_VOP
	Syntax: VOP Channel ADD Nickname
	        VOP Channel DEL {Nickname | entry-num | Liste}
	        VOP Channel LIST [Maske | Liste]
	        VOP Channel CLEAR

	Verwaltet die VOP (Voice) Liste eines Channels.VOPs
	bekommen automatisch Voice, wenn sie den Channel betreten.

	Der VOP ADD Befehl fügt den jeweiligen Nicknamen zur VOP-Liste
	hinzu.

	Der VOP DEL Befehl löscht den jeweiligen Nicknamen aus der
	VOP Liste. Wenn mehrere Eintragsnummern angegeben werden,
	werden diese Einträge gelöscht.  (Siehe Beispiel bei LIST.)


	Der VOP LIST Befehl zeigt die aktuelle VOP-Liste. Wenn
	Wildcard Masks angegeben werden, werden nur die betroffenen
	Einträge angezeigt. Werden mehrere Eintragsnummern angegeben,
	werden nur diese Einträge angezeigt, zB:

	   VOP #channel LIST 2-5,7-9
	      Listet die VOP Einträge 2 bis 5 und 7 bis 9.

	Der VOP CLEAR Befehl löscht alle Einträge der VOP-Liste.

	VOP ADD, VOP DEL und VOP LIST sind auf AOPs oder
	höher beschränkt, wärend VOP CLEAR nur vom Channelfounder
	genutzt werden kann.
	
	Dieser Befehl kann für deinen Channel ausgeschaltet sein, in
	diesem Fall nutze bitte die Access Liste. Für weitere Infos zur
	Access Liste, tippe bitte %R%S HELP ACCESS und
	%R%S HELP SET XOP für Infos über den Wechsel vom xOP und
	vom Access System.

CHAN_HELP_ACCESS
	Syntax: ACCESS Channel ADD Nickname Level
	        ACCESS Channel DEL {Nickname | entry-num | Liste}
	        ACCESS Channel LIST [Maske | Liste]
	        ACCESS Channel CLEAR
	
	Verwaltet die Zugriffsliste für einen Channel. Diese
	Liste sagt aus, welchem User Channel-Op-Status ermöglicht
	oder wem Zugriff auf %S Befehle des Channels
	freigeschaltet werden soll.
	
	Verschiedene Zugriffslevel bestimmen den Zugriff auf
	Privilegien, die dieser User dann im Channel besitzt.
	Tippe %R%S HELP ACCESS LEVELS für weitere Infos.
	Jeder Nickname, der nicht auf der Zugriffsliste steht, 
	hat automatisch einen Level von 0.
	
	Der ACCESS ADD Befehl fügt einen Nickname mit dem
	angegeben Level auf die Zugriffsliste ein. Wenn der 
	Nickname schon auf der Liste steht, wird der Level nur 
	geändert. Der angegebene Level muss kleiner als der 
	des Users sein, der diesen Befehl aufruft und wenn der 
	Nickname schon auf der Zugriffsliste steht, muss sein 
	aktueller Level ebenfalls kleiner sein, als von dem User, 
	der den Befehl aufruft.
	
	Mit ACCESS DEL wird ein Nickname von der Zugriffs-
	liste entfernt. Wenn eine Liste von Nummern angegeben wird,
	werden diese Einträge entfernt (siehe die Beispiele
	für LIST weiter unten).
	
	Der ACCESS LIST Befehl zeigt die Zugriffsliste eines
	Channels an. Wird eine Bereichsliste angegeben, werden die
	entsprechenden Einträge angezeigt, zum Beispiel:
	
	   ACCESS #channel LIST 2-5,7-9
	      Zeigt Einträge mit den Nummern 2 bis 5 und 
	      7 bis 9 an.
	
	Durch ACCESS CLEAR werden alle Einträge der
	Zugriffsliste entfernt.

CHAN_HELP_ACCESS_LEVELS
	User Zugriffslevel
	
	Standardmässig sind die Zugriffslevel wie folgt
	definiert:
	
	   Founder   (Gründer) vollständiger Zugriff auf
	                   den Channel und alle dazugehörigen
	                   Funktionen durch %S wird 
	                   automatisch mit Op-Status versehen beim 
	                   Joinen. Beachte dass nur eine Person den 
	                   Founder-Status besitzen kann (kann auch 
	                   nicht durch den ACCESS Befehls 
	                   erteilt werden).
	        10   Zugriff auf AKICK, hat Auto-Op.
	         5   Automatischer Op-Status.
	         3   Automatischer Voice-Status.
	         0   Keine speziellen Privilegien, kann durch
	                   andere Ops Channel-Operator werden
	                   (ausser Secure-Ops ist aktiv).
	        <0   Kann keinen Operator-Status erlangen.
	
	Neue Levels können Levels hinzugefügt oder verändert
	werden mit Hilfe des LEVELS Befehls. Tippe %R
	%S HELP LEVELS für weitere Informationen.

CHAN_HELP_AKICK
	Syntax: AKICK channel ADD {nick | mask} [reason]
	        AKICK channel STICK mask
	        AKICK channel UNSTICK mask
	        AKICK channel DEL {nick | mask | entry-num | list}
	        AKICK channel LIST [mask | entry-num | list]
	        AKICK channel VIEW [mask | entry-num | list]
	        AKICK Channel ENFORCE
	        AKICK Channel CLEAR
	
	Verwaltet die AutoKick-Liste für einen Channel.
	Wenn ein User joined, der auf der AutoKick-Liste steht,
	wird %S einen Ban auf den User setzen und ihn 
	dann kicken.
	
	Mit Hilfe von AKICK ADD kann man Nicknamen oder Hostmasken
	der AutoKick-Liste hinzufügen. Wird ein Grund beim
	Befehl angegeben, wird dieser als Kick-Grund für den
	User genutzt, ohne Reason wird standardmässig 
	"You have been banned from the channel" verwendet.
	Wird ein registrierter Nick gekickt, so wird der
	registrierte Nickname zur Kickliste hinzugefügt,
	anstatt der Hostmaske. Alle Nicknamen die dieser Gruppe
	angehören werden ebenfalls gekickt.
	
	Durch den AKICK DEL Befehls wird ein Eintrag von der
	AutoKick-Liste entfernt. Es entfernt jedoch nicht die Bans,
	die noch im Channel von ChanServ gesetzt sind, diese
	müssen manuell entfernt werden.

	Der AKICK STICK Befehl bannt die gegebenne Maske 
	dauerhft vom Channel. Falls jemand versucht, den Ban zu
	entfernen, wird %S ihn automatisch neu setzen. Kann
	nicht für registrierte Nicknamen genutzt werden.
	
	Der AKICK UNSTICK Befehl macht den AKICK STICK
	Befehl wieder rückgängig, sodass der Ban wieder
	entfernt werden kann.
	
	Der AKICK LIST Befehl zeigt den Inhalt einer 
	AutoKick-Liste an, oder optional die Einträge, die auf
	das angegebene Muster passen.
	
	Das Ergebnis von AKICK VIEW ist im Grunde das Gleiche
	wie das von AKICK LIST, jedoch werden mehr Informationen
	angezeigt.
	
	Durch AKICK ENFORCE wird %S gezwungen, die 
	aktuelle AutoKick-Liste auf den Channel anzuwenden 
	und User, die ein Muster der Liste treffen, zu entfernen.
	
	Der AKICK CLEAR Befehl entfernt alle Einträge aus
	Der AutoKick-Liste.

CHAN_HELP_LEVELS
	Syntax: LEVELS channel SET Typ Level
	        LEVELS channel {DIS | DISABLE} Typ
	        LEVELS channel LIST
	        LEVELS channel RESET
	
	
	Der LEVELS Befehl erlaubt eine feinere Regelung
	der Zugriffslevel eines Channels. Mit diesem Befehl
	kann man Zugriffslevel definieren, die die meisten
	Funktionen von %S abdecken. (SET FOUNDER und SET PASSWORD,
	genau wie dieser Befehl, sind beschränkt auf den Channel-
	Founder.)
	
	LEVELS SET erlaubt einem Zugriffslevel eine Änderung seiner
	Funktion bzw Gruppe von Funktionen. LEVELS DISABLE (oder 
	DIS als Abkürzung) deaktiviert eine automatische Funktion
	oder verweigert den Zugriff auf jeden anderen ausser den
	Channel-Founder.
	
	Durch LEVELS LIST werden die aktuellen Levels und ihre
	Funktionen aufgelistet. LEVELS RESET setzt alle Level auf
	die Standardwerte eines neu erzeugten Channels zurück
	(siehe HELP ACCESS LEVELS).
	
	Für eine Auflistung der Funktionen, die diese Parameter 
	haben, siehe HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Die folgenden Befehle/Funktionen werden unterstützt. 
	Beachte bitte, dass die hier angegebenen Level für
	AUTODEOP und NOJOIN Maximum-Levels sind, alle anderen
	hingegegen sind Minimum-Level.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Syntax: INFO Channel [ALL]
	
	Zeigt Informationen über einen registrierten Channel,
	seinen Gründer, das Datum der Registrierung, die
	letzte Nutzung, seine Beschreibung und Mode-Locks,
	wenn welche eingestellt sind. Wenn du als Founder
	(Gründer) identifiziert bist und den Parameter 
	ALL angibst, wird die Begrüssungsnachricht und 
	die Successors (Vertreter) ebenfalls angezeigt.

CHAN_HELP_LIST
	Syntax: LIST Muster
	
	Listet alle registrierten Channels, die dem Muster
	entsprechen, auf. (Alle Channels mit aktiviertem
	PRIVATE-Flag werden nicht angezeigt.)
	Anmerkung: Beachte, dass ein führendes '#' einen
	numerischen Bereich angibt, Channelnamen müssen
	ohne '#' angegeben werden.

CHAN_HELP_OP
	Syntax: OP [Channel [Nickname]]
	
	Setzt den Operator-Status des angegebenen Nicknamen 
	in dem Channel. Wird kein Nickname angegeben, wird
	ChanServ Dir Op Status geben, vorausgesetzt,
	du hast dieentsprechenden Rechte in der Zugriffsliste
	um diesen Befehl auszuführen.
	
	Standardmässig ist dafür ein Level von 5 oder
	grösser erforderlich.

CHAN_HELP_DEOP
	Syntax: DEOP [Channel [Nickname]]
	
	Entfernt den Operator-Status eines bestimmten Nickname
	in dem Channel. Wird kein Nickname angegeben, wird 
	ChanServ Dir den Op Status nehmen. Wird weder Channel
	noch Nick als Parameter übergeben, wird ChanServ Dir 
	in allen Channels den Op Status nehmen, in denen Du die
	entsprechenden Rechte hast.
	
	Standardmässig ist dafür ein Level von 5 oder
	grösser erforderlich.
	
CHAN_HELP_VOICE
	Syntax: VOICE [Channel [Nickname]]
	
	Erteilt dem angegebenen Nickname Voice-Status in dem
	Channel. Wird kein Nick angegeben, wird ChanServ Dir
	Voice geben. Wird weder Channel noch Nickane angegeben,
	wird ChanServ Dir in allen Channels Voice geben,
	in denen Du die entsprechenden Rechte hast.
	
	Standardmässig brauchst du dafür ein Level von 5
	um anderen Voice zu erteilen, oder Level 3 um Dir
	selbst Voice zu geben.

CHAN_HELP_DEVOICE
	Syntax: DEVOICE [Channel [Nickname]]
	
	Entfernt den Voice-Status des Nicknamen in dem Channel.
	Wird kein Nickname angegeben, wird ChanServ Dir den
	Voice Status nehmen.Wird weder Channel noch Nick angegeben,
	wird ChanServ Dir in allen Channels den Voice Status nehmen,
	in denen Du die entsprechenden Rechte hast.
	
	Standardmässig brauchst du dafür ein Level von 5
	um anderen den Voice Status zu nehmen, oder Level 3 um
	dich selbst vom Voice-Flag zu befreien.
	
CHAN_HELP_HALFOP
	Syntax: HALFOP [Channel [Nickname]]
	
	Setzt den Half-Op-Status des angegebenen Nicknamen in dem
	Channel. Wird kein Nickname angegeben, wird ChanServ Dir
	den Halfop geben. Wird weder Channel noch Nickname angegeben,
	wird ChanServ Dir in allen Channels den Halfop geben,
	in denen du die entsprechenden Rechte hast.
	
	Standardmässig brauchst du dafür ein Level von 5
	um andere den Halfop Status zu geben, oder Level 4 
	um Dir selbst den Halfop Status zu geben.

CHAN_HELP_DEHALFOP
	Syntax: DEHALFOP [Channel [Nickname]]
	
	Entfernt den Half-Op-Status des angegebenen Nicknamen in dem
	Channel. Wird kein Nickname angegeben, wird ChanServ Dir
	den Halfop nehmen. Wird weder Channel noch Nickname angegeben,
	wird ChanServ Dir in allen Channels den Halfop nehmen, in denen
	du die entsprechenden Rechte hast.
	
	Standardmässig brauchst du dafür ein Level von 5
	um andere den Halfop Status zu nehmen, oder Level 4 
	um Dir selbst den Halfop Status zu nehmen.
	
CHAN_HELP_PROTECT
	Syntax: PROTECT [Channel [Nickname]]
	
	Beschützt den angegebenen Nicknamen in dem Channel. Wird kein
	Nickname angegeben, wird ChanServ dich beschützen. Wird weder
	Nickname noch Channel angegeben, wird ChanServ dich in allen
	Channels beschützen in denen du die entsprechenden
	Rechte hast.
	
	Standardmässig lässt sich diese Funktion vom Founder
	aufrufen oder von denjenigen mit Level 10 und grösser, 
	die den Schutz auf sich selbst anwenden.

CHAN_HELP_DEPROTECT
	Syntax: DEPROTECT [Channel [Nickname]]
	
	Entfernt den Schutz auf den angegebenen Nickname in dem Channel.
	Wird kein Nickname angegeben, wird ChanServ dich nicht mehr
	beschützen. Wird weder Nickname noch Channel angegeben, wird
	ChanServ dich in allen Channels nicht länger schützen,
	in denen du die entsprechenden Rechte hast.
	
	Standardmässig lässt sich diese Funktion vom Founder
	aufrufen oder von denjenigen mit Level 10 und grösser,
	die den Schutz von sich selbst aufheben wollen.
	
CHAN_HELP_OWNER
	Syntax: OWNER [Channel]
	
	Gibt dir Owner Status im Channel. Für den Fall, dass
	kein Channel angegeben wird, wird dir in jedem Channel,
	in dem du ausreichend Rechte besitzt, Owner Status
	gegeben.
	
	Benötigt Founder Zugriffslevel im jeweiligen Channel.

CHAN_HELP_DEOWNER
	Syntax: DEOWNER [Channel]
	
	Nimmt dir Owner Status im Channel. Für den Fall, dass    
	kein Channel angegeben wird, wird dir in jedem Channel,        
	in dem du ausreichend Rechte besitzt, Owner Status 
	genommen.
	
	Benötigt Founder Zugriffslevel im jeweiligen Channel.

CHAN_HELP_INVITE
	Syntax: INVITE Channel
	
	Diese Funktion erlaubt es dir, dich durch %S in den
	angegebenen Channel einladen zu lassen. Standardmässig
	wird dies nur ausgeführt, wenn den Nickname auf der 
	Zugriffsliste des Channels einen Level grösser 5 
	(oder INVITE) hat.

CHAN_HELP_UNBAN
	Syntax: UNBAN Channel
	
	Durch %S werden alle Bans entfernt, die dich am joinen
	des Channels hindern. Eine Vorraussetzung dafür ist
	allerdings, dass dein Level in dem Channel grösser als
	5 (oder UNBAN)ist.
	
CHAN_HELP_KICK
	Syntax: KICK [Channel [Nickname [Grund]]]
	
	Kickt den ausgewählten Nicknamen aus dem Channel. Wird
	kein Nickname angegeben, wird ChanServ dich (!) kicken.
	Wenn weder Nickname noch Channel angegeben werden, wird
	ChanServ dich aus allen Channels kicken, in denen
	du die entsprechenden Rechte hast.
	
	Standardmässig ist für diese Funktion ein Level
	von mindestens 5 erforderlich.

CHAN_HELP_BAN
	Syntax: BAN [Channel [Nickname [Grund]]]
	
	Bannt einen Nicknamen im Channel. Falls kein Nickname angegeben
	wird, wirst du gebannt. Falls kein Nickname und kein Channel
	angegeben werden, wirst du in allen Channels gebannt,
	vorausgesetzt du hast die nötige Berechtigung.
	
	Standardmässig auf AOPs oder User mit Access Level 5 oder
	höher beschränkt.

CHAN_HELP_TOPIC
	Syntax: TOPIC Channel [Topic]
	
	Veranlasst %S ein Topic im Channel zu setzen. Falls Topic
	nicht angegeben wird, wird ein leeres Topic gesetzt. Dieser
	Befehl ist vor allem in Verbindung mit SET TOPICLOCK
	nützlich. Tippe %R%S HELP SET TOPICLOCK für weitere
	Informationen.
	
	Standardmässig auf Founder (Gründer) beschränkt. 

CHAN_HELP_CLEAR
	Syntax: CLEAR Channel was
	
	Sagt %S, dass es bestimmte Einstellungen 
	in einem Channel zurücksetzen soll. Die Option 
	kann eine der folgenden sein:
	
	     MODES    Setzt alle Modi in dem Channel zurück.
	              (z.B. Modi i,k,l,m,n,p,s und t)
	     BANS     Entfernt alle Bans in dem Channel
	     EXCEPTS  Entfernt alle Excepts in dem Channel
	     INVITES  Löscht alle Einladungen im Channel.
	     OPS      Entfernt jeden Operator-Status (mode +o)
	              in dem Channel.
	     HOPS     Entfernt jeden Halfoperator-Status (mode +h) 
	              in den Channel.(Falls dein IRCD es unterstützt).
	     VOICES   Entfernt alle Voice-Flags (mode +v) in dem Channel.
	     USERS    Entfernt (kickt) alle User aus dem Channel.
	
	Standardmässig muss dein Zugriffslevel in diesem Channel
	mindestens 10 betragen.
	
CHAN_HELP_GETKEY
	Syntax: GETKEY Channel
	
	Liefert den Key des angegebenen Channels zurück. Dieser
	Befehl wird am häufigsten von Bots und/oder Skripten
	genutzt, daher wird das Ergebnis in dieser Form
	zurückgeliefert:
	
	KEY <channel> <key>
	
	key ist "NO KEY", wenn kein Key eingestellt ist.
	
CHAN_HELP_SENDPASS
	Syntax: SENDPASS Channel
	
	Sendet das Passwort des angegebenen Channels an die
	eMail-Adresse, die dem Founder in der Datenbank
	assoziiert ist. Dieser Befehl ist sehr nützlich,
	wenn man mit dem Problem des vergessenen Passwortes
	konfrontiert wird.
	
	Kann in einigen Netzen auf IRC Operatoren beschränkt
	sein.
	
	Dieser Befehl ist nicht verfügbar, wenn die
	Datenbank verschlüsselt gespeichert wird.

CHAN_SERVADMIN_HELP
	
	Services Administratoren können weiterhin beliebige Channels
	DROPEN (löschen), Zugriffslisten oder Channel-Einstellungen
	ansehen ohne sich vorher für den Channel identifizieren zu müssen.
	
CHAN_SERVADMIN_HELP_LOGOUT
	Syntax: LOGOUT Channel [Nickname]
	
	Durch dieser Befehl wird dich ChanServ nicht mehr als
	identifiziert für den angegebenen Channel betrachten.
	
	Wenn du der Founder (Gründer) dieses Channels bist, kannst
	du jeden ausloggen, ansonsten nur dich selbst.
	
	Bist du ein Services Administrator, kannst du jeden des
	angegebenen Channels ausloggen, ohne Founder-Rechte
	des Channels zu besitzen. Weiterhin musst du den
	Parameter Nickname nicht unbedingt angeben, dies hat zur
	Folge, dass jeder Nickname aus dem Channel ausgeloggt wird.

CHAN_SERVADMIN_HELP_DROP
	Syntax: DROP Channel
	
	Entfernt die Registrierung des angegebenen Channels aus
	der Datenbank. Zusätzlich werden Zugriffslisten, 
	Einstellungen usw mit aus der Datenbank entfernt.
	
	Nur Services Administratoren können Channel DROPEN, 
	(löschen) ohne sich vorher gegenüber dem Channel als 
	Founder zu identifizieren.

CHAN_SERVADMIN_HELP_SET
	
	Services Administratoren können zusätzlich die Option NOEXPIRE
	an einem Channel setzen, dies verhindert den automatischen
	Auslauf des Channels.
	
	Weiterhin können Services Administratoren Einstellungen 
	eines Channels verändern, ohne sich vorher für ihn
	identifizieren zu müssen.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Syntax: SET Channel NOEXPIRE {ON | OFF}
	
	Ändert das NOEXPIRE-Flag an einem Channel. Wird es
	auf ON gestellt, wird der Channel bei Nichtnutzung
	nicht auslaufen.
	
	Diese Funktion ist beschränkt auf Services Administratoren.

CHAN_SERVADMIN_HELP_INFO
	
	Services Administratoren können den ALL Parameter bei jedem
	Channel benutzen.

CHAN_SERVADMIN_HELP_LIST
	Syntax: LIST Muster [FORBIDDEN] [NOEXPIRE]
	
	Zeigt alle registrierten Channels die auf das angegebene
	Muster treffen.Channels mit aktivierter PRIVATE Option 
	werden nur Services Administratoren angezeigt. Channels 
	mit aktivem NOEXPIRE werden in der Liste ein ! vor 
	dem Eintrag haben.
	
	Wird FORBIDDEN oder NOEXPIRE angegeben, werden nur Channels
	mit dem entsprechenden Flag angezeigt. Werden mehr Optionen
	angegeben, werden nur Channels angezeigt, die mindestens eins
	der entsprechenden Flags gesetzt haben. Diese Funktionen sind
	beschränkt auf Services Administratoren.

CHAN_SERVADMIN_HELP_GETPASS
	Syntax: GETPASS Channel
	
	Liest das Founder-Passwort des angegebenen Channels 
	aus der Datenbank von ChanServ aus.
	Beachte dass wenn jemand dieser Befehl benutzt,
	diese Aktion im Logbuch der Services vermerkt wird.
	Zusätzlich wird eine Nachricht an alle IRC Operatoren 
	per WALLOPS/GLOBOPS ausgesandt.

CHAN_SERVADMIN_HELP_FORBID
	Syntax: FORBID Channel [Grund]
	
	Verbietet die Nutzung bzw die Registrierung des
	angegebenen Channels. 
	Die Wirkung wird wieder aufgehoben durch DROPEN
	(löschen) des Channels.
	
	Bei einigen Netzen muss evtl ein Grund angegeben
	werden.

CHAN_SERVADMIN_HELP_SUSPEND
	Syntax: SUSPEND Channel [Grund]
	
	Verbietet jegliche Nutzung des Channels. Kann durch
	den UNSUSPEND Befehls wieder aufgehoben werden,
	wobei alle vorherigen Einstellungen wieder übernommen
	werden.
	
	Ein Grund kann auf manchen Netzwerke notwendig sein.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Syntax: UNSUSPEND Channel
	
	Gibt einen Channel, der auf SUSPEND gesetzt ist,
	wieder frei. Alle vorherigen Einstellungen werden
	übernommen.
	
CHAN_SERVADMIN_HELP_STATUS
	Syntax: STATUS Channel Nickname
	
	Liefert den Zugriffslevel des angegebenen Nicknamen aus dem
	Channel. Die Antwort erfolgt in dieser Form:
	
	    STATUS Channel Nickname Access-Level
	
	Tritt ein Fehler auf, wird die Antwort in dieser Form
	zurückgeliefert:
	
	    STATUS ERROR error-message

###########################################################################
#
# MemoServ help messages
#
###########################################################################
MEMO_HELP_CMD_SEND
	    SEND   Sendet eine Memo an einen Nickname oder Channel
MEMO_HELP_CMD_CANCEL
	    CANCEL Annulliert deine letzte gesendete Memo
MEMO_HELP_CMD_LIST
	    LIST   Erzeugt eine Liste deiner Memos
MEMO_HELP_CMD_READ
	    READ   Lesen einer oder mehrerer Memos
MEMO_HELP_CMD_DEL
	    DEL    Löschen einer oder mehrerer Memos
MEMO_HELP_CMD_SET
	    SET    Stellt bestimmte Optionen ein in Hinsicht
	           auf Memos
MEMO_HELP_CMD_INFO
	    INFO   Zeigt Informationen bezüglich deiner Memos an
MEMO_HELP_CMD_RSEND
	    RSEND  Sendet eine Memo an einen Nickname oder Channel
	           und  sendet dir eine Empfangsbestätigung zurück
MEMO_HELP_CMD_CHECK
	    CHECK  Überprüft ob deine letzte gesendete Memo an
	           einen Nicknamen gelesen wurde

MEMO_HELP_CMD_SENDALL
	    SENDALL  Sendet eine Nachricht an alle registrierten User.
MEMO_HELP_CMD_STAFF
	    STAFF  Sendet allen Opers/Admins eine Nachricht.

MEMO_HELP_HEADER
	%S ist eine Einrichtung, die es erlaubt, 
	dass sich IRC User untereinander kurze Textnachrichten 
	senden, egal ob die Empfänger zu dem Zeitpunkt online 
	sind oder nicht.
	Als Empfänger können auch Channels angegeben werden (*).
	
	Sowohl der Nickname des Senders als auch Empfängers müssen
	registriert sein, um eine Memo versenden zu können.
	
	%S's Befehle beinhalten:
	
MEMO_HELP_ADMIN
	NICHT benutzt.
MEMO_HELP_OPER
	NICHT benutzt.
MEMO_HELP_FOOTER
	
	Tippe: %R%S HELP Befehl für hilfe bei jedem der
	oben aufgeführten Befehle.

	(*) Bei der Standarteinstellung kann jeder User mit einem 
	    Access-Level auf den Channel von 10 oder höher die 
	    Channel-Nachrichten lesen. Dies kann mit dem
	    %s LEVELS Befehl geändert werden.

MEMO_HELP_SEND
	Syntax: SEND {Nickname | Channel} Memo-Text
	
	Sendet an einen Nicknamen oder Channel die Memo 
	mit dem Inhalt Memo-Text.  Wird an einen Nicknamen 
	gesendet, erhält der Empfänger eine Notice, dass
	er/sie eine neue Memo hat.
	Der Empfänger (Nickname/Channel) muss registriert sein.
	
MEMO_HELP_CANCEL
	Syntax: CANCEL {Nickname | Channel}
	
	Widerruft die letzte Memo die du an den angegebenen Nicknamen
	oder Channel gesendet hast, vorrausgesetzt sie ist noch nicht
	gelesen worden.

MEMO_HELP_LIST
	Syntax: LIST [Channel] [List | NEW]
	
	Führt alle Memos auf, die du im Moment gespeichert hast.
	Mit dem Parameter NEW werden nur neue, ungelesene, Memos
	aufgelistet. Ungelesene Memos sind durch ein "*" markiert,
	links neben ihrer Nummer. Du kannst auch einen Bereich
	von Nummern definieren. Siehe das Beispiel:
	
	   LIST 2-5,7-9
	      Zeigt Memos mit den Nummern 2 bis 5 und 7 bis 9.

MEMO_HELP_READ
	Syntax: READ [Channel] {Nummer | List | LAST | NEW}
	
	Schickt dir den Inhalt der Memo, die du lesen möchtest.
	Wird LAST angegeben, bekommst du die Memo die du als 
	letztes erhalten hast. Mit NEW werden dir alle neuen,
	ungelesenen, Memos geschickt. 
	
	Andernfalls kannst du auch mit Hilfe der Memo-Nummer
	Nummer oder einer Bereichsliste dir Memos anzeigen
	lassen, zum Beispiel so:
	
	   READ 2-5,7-9
	      Zeigt den Inhalt der Memos 2 bis 5 und 7 bis 9.

MEMO_HELP_DEL
	Syntax: DEL [Channel] {Nummer | List | LAST | ALL}
	
	Entfernt Memos aus deinem Postfach. Du kannst mehrere
	Nummern oder Bereiche angeben anstatt einer einzelnen
	Nummer, wie das zweite Beispiel zeigt.
	
	Wird LAST angegeben, wird die letzte Memo gelöscht.
	Wird ALL angegeben, werden alle deine Memos
	entfernt.
	
	Beispiele:
	
	   DEL 1
	      Löscht dein Memo mit der Nummer 1.
	
	   DEL 2-5,7-9
	      Löscht Memos mit den Nummern 2 bis 5 und 7 bis 9.

MEMO_HELP_SET
	Syntax: SET Option Parameter
	
	Stellt verschiedene Optionen bezüglich der Memos ein.
	Option kann einer der Parameter sein:
	
	    NOTIFY      Stellt ein, wann du benachrichtigt wirst,
	                wenn du eine neue Memo bekommst.
	
	    LIMIT       Ändert die maximale Anzahl Memos, die
	                du empfangen kannst.
	
	Tippe: %R%S HELP SET Option für weitere Informationen
	über eine bestimmte Option.

MEMO_HELP_SET_NOTIFY
	Syntax: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
	
	Ändert den Zeitpunkt, wann MemoServ dich über neue
	eingegangene Memos informiert:
	
	    ON      Du wirst über neue Nachrichten informiert,
	            wenn du dich identifizierst, /AWAY aufhebst,
	            und wenn sie an dich gesandt werden.
	    LOGON   Du wirst nur benachrichtigt, wenn du dich
	            identifizierst oder das /AWAY rückgängig
	            machst.
	    NEW     Du wirst nur informiert, wenn sie an dich
	            gesendet werden.
	    MAIL    Du wirst über neue memos per email informiert.
	    NOMAIL  Du wirst nicht über neue memos informiert per email.
	    OFF     Es wird keine Benachrichtigung über neue
	            Memos stattfinden.
	
	ON ist eine Kombination aus LOGON und NEW.
	MAIL und NOMAIL sind optionale Einstellungen und
	beeinflussen ON | LOGON | NEW nicht. Nach der Einstellung
	OFF muß die Option Mail erneut gesetzt werden, damit
	memos per email zugestellt werden sollen.

MEMO_HELP_SET_LIMIT
	Syntax: SET LIMIT [Channel] Limit
	
	Ändert das Limit der Anzahl der Memos, die du
	(oder der angegebene Channel) haben darf. Wird dies auf
	0 gesetzt, kann dir niemand mehr Memos senden.
	Du kannst es jedoch auch nicht höher als %d setzen.

MEMO_HELP_INFO
	Syntax: INFO [Channel]
	
	Zeigt Informationen über die Anzahl der Memos die du
	hast, wie viele davon ungelesen sind und wie viele
	du max. empfangen kannst. Mit angegebenem Parameter
	wird die gleiche Information über den Channel angezeigt.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Syntax: SET LIMIT [User | Channel] {Limit | NONE} [HARD]
	
	Ändert die max. Anzahl von Memos die ein User oder
	Channel haben kann. Das setzen dieser Grenze auf 0
	verhindert das Empfangen jeglicher Memos. Wird 
	hingegen NONE eingestellt, kann der User so
	viele Memos empfangen und behalten wie er will.
	Wird kein User oder Channel angegeben, veränderst
	du dein eigenes Limit.
	
	Durch den Zusatzparameter HARD wird eine Änderung
	des Limits durch den entsprechenden User verhindert.
	Wird dieser Parameter nicht angegeben, hat das den
	gegenteiligen Effekt - der User kann sein Limit 
	ändern (sogar wenn das vorherige Limit auf 
	HARD eingestellt war).
	
	Die Nutzung des SET LIMIT Befehls ist beschränkt auf 
	Services Administratoren. Jeder andere User kann nur 
	das Limit von sich selbst oder dem Channel ändern, in 
	dem er einen entsprechenden Zugriffslevel besitzt, er 
	kann auch nicht die Grenze von %d Memos überschreiten.
	Der HARD Befehl kann er auch nicht nutzen.

MEMO_SERVADMIN_HELP_INFO
	Syntax: INFO [Nickname | Channel]
	
	Ohne Parameter werden Informationen über dein Postfach
	angezeigt, die Anzahl Memos die du hast, die davon neu
	bzw ungelesen sind, und wie viele Memos du max.
	empfangen kannst.
	
	Mit einem Channel als Parameter werden die entsprechenden
	Informationen über den Channel angezeigt.
	
	Wird eine Nickname übergeben, werden die entsprechenden
	Informationen über den Nicknamen angezeigt. Diese
	Funktion ist beschränkt auf Services Administratoren.

MEMO_HELP_STAFF
	Syntax: STAFF Memo-Text

	Sendet allen Usern mit Zugriff auf die Services eine Nachricht mit
	dem Memo-Text.
	Note: Wenn du Operatoren an beide der Services-Zugriffs-Listen 
	(Oper und Admin) hast, so werden die alle diese Nachricht erhalten. 
	Das gleichte trifft für die Operatoren auf der Root-Liste zu.

MEMO_HELP_SENDALL
	Syntax: SENDALL Memo-Text

	Sendet allen registrieten Usern eine Nachricht mit dem 
	Memo-Text.

MEMO_HELP_RSEND
	Syntax: RSEND {Nickname | Channel} Memo-Text
	
	Sendet an einen Nicknamen oder Channel die Memo 
	mit dem Inhalt Memo-Text.  Wird an einen Nicknamen 
	gesendet, erhält der Empfänger eine Notice, dass er/sie 
	eine neue Memo hat. Der Empfänger (Nickname/Channel) muss 
	registriert sein. Wenn deine Memo gelesen worden ist, 
	bekommst du eine Auto-Memo die dich Informirmiert
	das der / die jenige dein Memo-Text gelesen hat.

MEMO_HELP_CHECK
	Syntax: CHECK Nickname
	
	Überprüft ob deine letzte gesendete Memo an einen Nicknamen gelesen
	worden ist oder nicht. Der CHECK Befehl funktioniert nicht mit Channels.

###########################################################################
#
# OperServ help messages
#
###########################################################################

OPER_HELP_CMD_GLOBAL
	    GLOBAL       Eine Notice an alle User schicken
OPER_HELP_CMD_STATS
	    STATS        Status des Netzes und der Services anzeigen
OPER_HELP_CMD_OPER
	    OPER         Die Services Operatoren Liste bearbeiten
OPER_HELP_CMD_ADMIN
	    ADMIN        Liste der Services Administratoren bearbeiten
OPER_HELP_CMD_STAFF
	    STAFF        Zeigt alle Nicknamen, die Zugriff auf die 
	                 Services haben, Onlinestatus und Level
OPER_HELP_CMD_MODE
	    MODE         Channel-Modes ändern
OPER_HELP_CMD_KICK
	    KICK         Einen User aus einem beliebigen
	                 Channel kicken
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES   Die Modi eines Channels zurücksetzen
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES   Alle User mit einem bestimmten Host killen
OPER_HELP_CMD_AKILL
	    AKILL        Die AutoKill-Liste bearbeiten
OPER_HELP_CMD_SGLINE
	    SGLINE       Die SGLINE-Liste bearbeiten
OPER_HELP_CMD_SQLINE
	    SQLINE       Die SQLINE-Liste bearbeiten
OPER_HELP_CMD_SZLINE
	    SZLINE       Die SZLINE-Liste bearbeiten
OPER_HELP_CMD_CHANLIST
	    CHANLIST      Alle registrierten Channels anzeigen
OPER_HELP_CMD_USERLIST
	    USERLIST      Alle registrierten Nicknamen anzeigen
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS     Logon-News bearbeiten
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS    Definiert Nachrichten, die den Benutzern zufällig
	                  beim Verbinden angezeigt werden
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS      Oper News bearbeiten
OPER_HELP_CMD_SESSION
	    SESSION       Hosts mit einer bestimmten Anzahl
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION     Bearbeiten der Liste der eingeschränkten Verbindungen
OPER_HELP_CMD_NOOP
	    NOOP          Alle O:Lines temporär deaktivieren
OPER_HELP_CMD_JUPE
	    JUPE          Einen Server "jupitern"
OPER_HELP_CMD_IGNORE
	    IGNORE        Die "Services Ignore Liste" bearbeiten
OPER_HELP_CMD_SET
	    SET           Verschiedene Services Optionen einstellen
OPER_HELP_CMD_RELOAD
	    RELOAD        Die Konfigurationsdatei neu einlesen
OPER_HELP_CMD_UPDATE
	    UPDATE        Die Services Datenbänke sofort speichern
OPER_HELP_CMD_RESTART
	    RESTART       Datenbanken speichern und Services neustarten
OPER_HELP_CMD_QUIT
	    QUIT          Services _ohne_ Speichern der Datenbanken
	                  beenden
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN      Datenbanken speichern und Services beenden
OPER_HELP_CMD_DEFCON
	    DEFCON        Ändert das  DefCon System
OPER_HELP_CMD_CHANKILL
	    CHANKILL      Setzt einen AKILL auf alle User des angegebenen Channels.
OPER_HELP_CMD_OLINE
	    OLINE         Einem User "Operflags" setzen
OPER_HELP_CMD_UMODE
	    UMODE         User Modi ändern
OPER_HELP_CMD_SVSNICK
	    SVSNICK       Erzwingt einen Nicknamen wechsel
OPER_HELP_CMD_MODLOAD
	    MODLOAD       Lädt ein Modul
OPER_HELP_CMD_MODUNLOAD
	    MODUNLOAD     Entfernt ein Modul
OPER_HELP_CMD_MODINFO
	    MODINFO       Info über ein geladenes Modul
OPER_HELP_CMD_MODLIST
	    MODLIST       Listet die geladenen Module auf

OPER_HELP
	Befehle von %S:

OPER_HELP_LOGGED
	Achtung: Alle Befehle die an %S gesendet werden,
	werden geloggt!

OPER_HELP_GLOBAL
	Syntax: GLOBAL Nachricht
	
	Ermöglicht Administratoren eine Nachricht an alle User auf
	dem Netzwerk zu senden. Die Nachricht wird als
	Absender %s tragen.

OPER_HELP_STATS
	Syntax: STATS [AKILL | ALL | RESET]
	
	Wird keine Option angegeben, wird eine kurze Statistik
	ausgegeben. Sie enthält die aktuelle Anzahl verbundener
	User, IRCops (inklusive den Services). Die max. Anzahl
	gleichzeitig verbundener User seitdem die Services
	gestartet wurden und die Uptime.
	
	Mit der AKILL Option, wird die aktuelle Grösse der
	AutoKill-Liste ausgegeben und dessen standardmässige
	Auslaufzeit.
	
	Die ALL Option ist nur für Services Administratoren verfügbar.
	Sie zeigt unter anderem den aktuellen Speicherverbrauch der 
	Services. Dieser Befehl kann auf einem grossen Netzwerk dazu 
	führen, dass die Services für kurze Zeit nicht mehr ansprechbar 
	sind, also übertreib es nicht.
	
	Durch RESET werden einige Werte der Statistik zurück-
	gesetzt, unter anderem die max. Anzahl gleichzeitig
	verbundenen User.
	
	UPTIME kann als Synonym für STATS verwendet werden.

OPER_HELP_OPER
	Syntax: OPER ADD Nickname
	        OPER DEL {Nickname | entry-num | Liste}
	        OPER LIST [Maske | Liste]
	        OPER CLEAR
	
	Erlaubt Services Root bestimmte Nicknamen in 
	die Liste der Services Operatoren aufnehmen oder zu entfernen. 
	Ein User der sich für diesen Nicknamen gegenüber %s 
	identifiziert hat und auf der Liste der Services Operatoren 
	ist, hat Zugriff auf die Befehle von OperServ.
	
	Mit OPER ADD wird der angegebene Nickname auf die Liste der
	Services Operatoren hinzugefügt.
	
	Durch OPER DEL wird der angegebene Nickname von der Liste
	entfernt. Wird ein Bereich von Zahlen eingegeben, werden
	die entsprechenden Einträge gelöscht. (Siehe das Beispiel
	für LIST weiter unten.)
	
	Der OPER LIST Befehl zeigt den Inhalt der Liste der
	Services Operatoren. Wird eine Maske mit Platzhaltern
	(Wildcards) angegeben, werden die entsprechenden Einträge 
	angezeigt. Wird eine Zahlenbereich angegeben, werden
	ebenfalls nur die entsprechenden Einträge angezeigt,
	zum Beispiel:
	
	   OPER LIST 2-5,7-9
	      Zeigt die Einträge 2 bis 5 und 7 bis 9 aus
	      der Services Operatoren-Liste.
	
	Durch OPER CLEAR werden alle Einträge aus dieser Liste
	entfernt.
	
	Jeder IRC Operator kann den OPER LIST Befehl verwenden.

OPER_HELP_ADMIN
	Syntax: ADMIN ADD Nickname
	        ADMIN DEL {Nickname | entry-num | Liste}
	        ADMIN LIST [Maske | Liste]
	        ADMIN CLEAR
	
	Erlaubt Services Roots bestimmte Nicknamen in die Liste
	der Services Administratoren aufnehmen oder zu entfernen. 
	Ein User der sich für diesen Nicknamen gegenüber %s
	identifiziert hat und auf die Services Adminstratoren-Liste
	ist, hat Zugriff auf die Admin Befehlen von OperServ.
	
	Mit ADMIN ADD wir der angegebene Nickname auf die
	Services Administratoren-Liste hinzugefügt.
	
	Durch ADMIN DEL wird der angegebene Nickname von der Liste
	entfernt. Wird ein Bereich von Zahlen eingegeben, werden
	die entsprechenden Einträge gelöscht. (Siehe das Beispiel
	für LIST weiter unten.)
	
	Der ADMIN LIST Befehl zeigt den Inhalt der Services 
	Administratoren-Liste an. Wird eine Maske mit Platzhaltern
	(Wildcards) angegeben, werden die entsprechenden Einträge
	angezeigt. Wird ein Zahlenbereich angegeben, werden
	ebenfalls nur die entsprechenden Einträge angezeigt,
	zum Beispiel:
	
	   ADMIN LIST 2-5,7-9
	      Zeigt die Einträge 2 bis 5 und 7 bis 9 aus der
	      Services Administratoren-Liste an.
	
	Mit Hilfe von ADMIN CLEAR wird die Services
	Administartoren-Liste  komplett geleert.
	
	Jeder IRC Operator kann den Befehl ADMIN LIST nutzen, 
	jegliche anderen Befehle können nur von Services Roots
	aufgerufen werden.

OPER_HELP_IGNORE
	Syntax: IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick | mask]
	
	Erlaubt Services Admins den Services einen Nick oder eine
	Hostmaske für eine bestimmte Zeit oder bis zum nächsten
	Neustart zu ignorieren. Mit Hilfe von Zeit lässt sich eine
	bestimmte Auslaufzeit dieses Eintrags definieren.
	Zeit wird durch eine Ganzzahl angegeben, gefolgt von einem
	Buchstaben: s (Sekunden), m (Minuten), h (Stunden)
	oder d (Tage).
	Kombinationen (wie zum Beispiel 1h30m) sind nicht zulässig.
	Wird kein Buchstabe angegeben, wird von Sekunden ausgegangen
	(also 30 bedeutet dann z.B. 30 Sekunden). Um einen 
	IGNORE-Eintrag zu erstellen, der nicht ausläuft, benutze 0.
	Wenn eine Hostmaske hinzugefügt wird sollte es in dem Format
	user@host oder nick!user@host erfolgen, ansonsten wird es als
	Nick angesehen. Wildcards sind erlaubt.
	
	Ignores werden nicht bei IRC Operatoren erzwungen.

OPER_HELP_MODE
	Syntax: MODE Channel Modi
	
	Erlaubt es Services Operatoren die Channel Modes eines
	beliebigen Channels zu verändern. Der Paramater hat das
	gleiche Format wie der normale /MODE Befehl.

OPER_HELP_UMODE
	Syntax: UMODE User Modi
	
	Erlaubt es Super Adminstratoren die Usermodi eines 
	beliebigen Users zu ändern. Der Paramater hat das gleiche 
	Format wie der normale /MODE Befehl.

OPER_HELP_OLINE
	Syntax: OLINE User Flags
	
	Erlaubt es Super Adminstratoren einem User Operflags 
	zu setzen.Den Flags muss ein "+" oder ein "-" vorangestellt 
	werden. Um alle Operflags zu entfernen, nutze einfach ein 
	"-" ohne Operflags.

OPER_HELP_CLEARMODES
	Syntax: CLEARMODES Channel [ALL]
	
	Entfernt alle Modi und Bans eines Channels. 
	Wird ALL angegeben, werden ebenfalls alle
	Op-Flags und Voices (+o und +v Modi) entfernt.

OPER_HELP_KICK
	Syntax: KICK Channel User Grund
	
	Erlaubt staff einen User aus einem beliebigen
	Channel zu kicken. Die Parameter sind dieselben
	wie für das normale /KICK Befehl.
	Die Kick-Nachricht wird den Nicknamen des IRCops
	vor dem Grund haben. Zum Beispiel:
	
	*** SpamMan has been kicked off channel #my_channel by %S (chemical (Flood))

OPER_HELP_SVSNICK
	Syntax: SVSNICK Nickname Neuer-Nickname
	
	Erzwingt die Änderung des Nicknamens eines User zu dem neuen Nicknamen.
	Limited to Super Admins.

OPER_HELP_AKILL
	Syntax: AKILL ADD [+Zeit] Maske Grund
	        AKILL DEL {Maske | entry-num | Liste}
	        AKILL LIST [Maske | Liste]
	        AKILL VIEW [Maske | Liste]
	        AKILL CLEAR
	
	Erlaubt Services Operatoren die AutoKill-Liste zu
	bearbeiten. Versucht ein User der einen entsprechenden 
	Eintrag auf der AKILL-Liste hat zu verbinden, werden
	die Services ihn killen und falls der Server dies
	unterstützt wird ein Ban (G-line) für den User gesetzt.
	
	AKILL ADD fügt die angegebene user@Hostmaske mit
	dem Grund (dieser muss angegeben werden) zur AKILL-
	Liste hinzu. Mit Hilfe von Zeit lässt sich eine bestimmte
	Auslaufzeit dieses Eintrags definieren.
	Zeit wird durch eine Ganzzahl angegeben, gefolgt von
	einem Buchstaben:
	d (Tage), h (Stunden), oder m (Minuten). Kombinationen
	(wie zum Beispiel 1h30m) sind nicht zulässig. Wird kein
	Buchstabe angegeben, wird von Tagen ausgegangen (also +30 
	bedeutet dann z.B. 30 Tage). Um einen AKILL-Eintrag zu
	erstellen, der nicht ausläuft, benutze +0.  Fängt die Usermaske
	mit einem + an, muss eine Auslaufzeit (Zeit) definiert
	werden, auch wenn es die Standard-Zeit ist. Die Standard-
	Auslaufzeit für einen AKILL kann man unter STATS AKILL finden.
	
	Das AKILL DEL Befehl entfernt die angegebene Maske von
	der AKILL-Liste, sofern ein solcher Eintag existiert. Wird
	eine Liste von Nummern angegeben, werden diese Einträge
	entfernt. (Siehe das Beispiel für LIST weiter unten.)
	
	Mit Hilfe von AKILL LIST wird die AKILL Liste angezeigt.
	Werden Platzhalter (Wildcards) in der Maske angegeben, werden
	nur die entsprechenden Einträge angezeigt. Die Ausgabe
	lässt sich ebenfalls durch einen Zahlenbereich begrenzen, 
	wie das Beispiel zeigt:
	
	   AKILL LIST 2-5,7-9
	      Zeigt nur die Einträge 2 bis 5 und 7 bis 9.
	
	AKILL VIEW ist eine ausführlichere Version des AKILL LIST
	Befehls und zeigt zusätzlich, wer einen Eintrag wann 
	erstellt hat, wann er ausläuft und natürlich die user@host
	Maske mit dem Grund.
	
	AKILL CLEAR entfernt alle Einträge aus der AKILL-Liste.
	
OPER_HELP_SGLINE
	Syntax: SGLINE ADD [+Zeit] Maske:Grund
	    SGLINE DEL {Maske | entry-num | Liste}
	        SGLINE LIST [Maske | Liste]
	        SGLINE VIEW [Maske | Liste]
	        SGLINE CLEAR
	
	Erlaubt Services Operatoren die SGLINE-Liste zu
	bearbeiten. Versucht ein User dessen Realname auf 
	einen Eintrag in der SGLINE-Liste trifft zu verbinden,
	werden die Services ihn die verbindung auf das IRC Netz
	nicht ausführen lassen.
	
	SGLINE ADD fügt die angegebene Realnamen-Maske mit 
	dem Grund (muss angegeben werden) zur SGLINE-Liste 
	hinzu. Mit Hilfe von Zeit lässt sich eine bestimmte
	Auslaufzeit dieses Eintrags definieren.
	Zeit wird durch eine Ganzzahl angegeben, gefolgt von
	einem Buchstaben:
	d  (Tage), h (Stunden), oder m (Minuten). Kombinationen
	(wie zum Beispiel 1h30m) sind nicht zulässig.  Wird kein
	Buchstabe angegeben, wird von Tagen ausgegangen (also +30 
	bedeutet dann z.B. 30 Tage). Um einen SGLINE-Eintrag zu
	erstellen, der nicht ausläuft, benutze +0. Beginnt
	die Realnamen-Maske mit einem +, an, muss eine 
	Auslaufzeit (expiry) definiert werden, auch wenn es die 
	Standard-Zeit ist. Die Standard-Auslaufzeit für eine
	SGLINE kann man unter STATS AKILL finden.
	
	Beachte: Da die Realnamen-Maske Leerzeichen enthalten kann,
	ist das Trennzeichen zwischen der Maske und dem Grund
	ein Doppelpunkt.

	Das SGLINE DEL Befehl entfernt die angegebene Maske
	von der SGLINE-Liste, sofern ein solcher Eintag existiert.
	Wird eine Liste von Nummern angegeben, werden diese
	Einträge entfernt. (Siehe das Beispiel für LIST weiter
	unten.)
	
	Mit Hilfe von SGLINE LIST wird die SGLINE-Liste 
	angezeigt. Werden Platzhalter (Wildcards) in der Maske
	angegeben, werden nur die entsprechenden Einträge
	angezeigt. Die Ausgabe lässt sich ebenfalls durch einen
	Zahlenbereich begrenzen, wie das Beispiel zeigt:
	
	   SGLINE LIST 2-5,7-9
	      Zeigt nur die Einträge 2 bis 5 und 7 bis 9.
	
	SGLINE VIEW ist eine ausführlichere Version des 
	SGLINE LIST Befehls und zeigt zusätzlich, wer einen 
	Eintrag wann erstellt hat, wann er ausläuft und 
	natürlich die user@host Maske mit dem Grund.
	
	SGLINE CLEAR entfernt alle Einträge aus der 
	SGLINE-Liste.

OPER_HELP_SQLINE
	Syntax: SQLINE ADD [+Zeit] Maske Grund
	        SQLINE DEL {Maske | entry-num | Liste}
	        SQLINE LIST [Maske | Liste]
	        SQLINE VIEW [Maske | Liste]
	        SQLINE CLEAR
	
	Erlaubt Services Operatoren die SQLINE Liste zu bearbeiten.
	Falls ein User mit passendem Nicknamen sich zum Netzwerk
	verbinden möchte, werden die Services diese Verbindung nicht 
	ausführen lassen.
	
	Wenn der erste Buchstabe ein # ist, werden die Services die
	Nutzung des betreffenden Channels nicht zulassen (bei IRCds,
	die dieses Feature unterstützen).
	
	SQLINE ADD fügt den angegeben Nicknamen mit dem Grund (muss
	angegeben werden) zur SGLINE-Liste hinzu. ^_Zeit^_ wird
	durch eine Ganzzahl angegeben, gefolgt von einem Buchstaben: 
	d (Tage), h (Stunden), oder m (Minuten).
	Kombinationen sind nicht erlaubt. Wird kein Buchstabe angegeben,
	wird von Tagen ausgegangen (also +30 bedeutet 30 Tage).
	Um einen SQLINE-Eintrag zu erstellen, der nicht ausläuft,
	benutze+0.  
	Beginnt die Maske mit einem +, muss eine Auslaufzeit
	(expiry) definiert werden, auch wenn es die Standard-Zeit ist. 
	Die Standard-Auslaufzeit für eine SQLINE kann man unter
	STATS AKILL finden.
	
	Das SQLINE DEL Command entfernt die betreffende Maske von
	der SQLINE Liste.  Wird eine Liste von Nummern angegeben,
	werden diese Einträge entfernt. (Siehe das Beispiel für LIST 
	weiter unten.)
	
	Mit Hilfe von SQLINE LIST wird die SQLINE-Liste angezeigt.
	Werden Wildcards in der Maske angegeben, werden nur die
	entsprechenden Einträge angezeigt. Die Ausgabe lässt sich
	ebenfalls durch einen Zahlenbereich begrenzen:

	   SQLINE LIST 2-5,7-9
	      Zeigt die SQLINE Einträge 2 bis 5 und 7 bis 9 an.

	SQLINE VIEW ist eine ausführlichere Version von SQLINE LIST
	und zeigt an, wer eine SQLINE gesetzt hat, das Datum und wann
	diese ausläuft, ausserdem die Maske und den Grund.
	
	SQLINE CLEAR löscht alle Einträge der SQLINE Liste.

OPER_HELP_SZLINE
	Syntax: SZLINE ADD [+Zeit] Maske Grund
	        SZLINE DEL {Maske | entry-num | Liste}
	        SZLINE LIST [Maske | Liste]
	        SZLINE VIEW [Maske | Liste]
	        SZLINE CLEAR
	
	Erlaubt es Services Operatoren die SZLINE-Liste zu
	bearbeiten. Versucht ein User zu connecten dessen
	IP auf einen entsprechenden Eintrag auf der SZLINE-
	Liste passt, wird der Server seine IRC Verbindung
	verhindern (unabhängig davon, ob die IP einen
	PTR-Record hat oder nicht.)
	
	SZLINE ADD fügt die angegebene IP-Maske mit dem
	Reason (muss angegeben werden) zur SZLINE-Liste 
	hinzu. Mit Hilfe von expiry lässt sich eine bestimmte
	Auslaufzeit dieses Eintrags definieren.
	expiry wird durch eine Ganzzahl angegeben, gefolgt von
	einem Buchstaben:
	 d (Tage), h (Stunden), oder m (Minuten). Kombinationen
	(wie zum Beispiel 1h30m) sind nicht zulässig. Wird kein
	Buchstabe angegeben, wird von Tagen ausgegangen (also +30 
	bedeutet dann z.B. 30 Tage). Um einen SZLINE-Eintrag zu
	erstellen, der nicht ausläuft, benutze +0. Fängt die
	IP-Maske mit einem  + an, muss eine Auslaufzeit
	(expiry) definiert werden, auch wenn es die Standard-Zeit
	ist. Die Standard-Auslaufzeit für eine SZLINE kann man 
	unter STATS AKILL finden.
	
	Der SZLINE DEL Befehl entfernt die angegebene Maske
	von der SZLINE-Liste, sofern ein solcher Eintag existiert. 
	Wird eine Liste von Nummern angegeben, werden diese Einträge
	entfernt. (Siehe das Beispiel für LIST weiter unten.)
	
	Mit Hilfe von SZLINE LIST wird die SZLINE-Liste angezeigt.
	Werden Platzhalter (Wildcards) in der Maske angegeben, werden
	nur die entsprechenden Einträge angezeigt. Die Ausgabe
	lässt sich ebenfalls durch einen Zahlenbereich begrenzen,
	wie das Beispiel zeigt:
	
	   SZLINE LIST 2-5,7-9
	      Zeigt nur die Einträge 2 bis 5 und 7 bis 9.
	
	SZLINE VIEW ist eine ausführlichere Version des
	SZLINE LIST Befehls und zeigt zusätzlich, wer einen
	Eintrag wann erstellt hat, wann er ausläuft und natürlich
	die IP Maske mit dem Grund.
	
	SZLINE CLEAR entfernt alle Einträge aus der SZLINE-Liste.

OPER_HELP_SET
	Syntax: SET Option Einstellungen
	
	Ändert globale Einstellungen der Services Optionen.
	Diese Optionen lassen sich ändern:
	
	    READONLY   Nur-Lese Modus der Datenbank an oder aus
	    LOGCHAN    Zeigt Logmessages in einem Chan
	    DEBUG      Debug-Modus der Services an oder aus
	    NOEXPIRE   Auslauf-Modus der Services an oder aus
	    SUPERADMIN Den Super-Admin Modus an oder aus
	    SQL        SQL-Modus an oder aus
	    IGNORE     Ignore-Modus an oder aus
	    LIST       Optionen-Liste

OPER_HELP_SET_READONLY
	Syntax: SET READONLY {ON | OFF}
	
	Schaltet den Read-Only-Mode an oder aus. Im
	Read-Only-Modus können normale User nichts mehr
	an den Datenbanken der Services ändern, dies beinhaltet
	Channel und Nicknamen-Zugriffslisten, etc. IRCops mit
	genügend Rechten werden weiterhin Änderungen an der
	AKILL-Liste, Nicknamen sperren oder DROPEN (löschen) dürfen,
	aber diese Änderungen werden nicht gespeichert, bis
	der Read-Only-Mode deaktiviert wurde bevor die
	Services beendet oder neu gestartet werden.
	
	Diese Option ist äquivalent mit der Befehlszeilen-
	Option -readonly.

OPER_HELP_SET_LOGCHAN
	Syntax: SET LOGCHAN {ON | OFF}

	Mit dieser Option senden die Services alle Logeinträge zum
	angegebenen Channel. LogChannel muss in der Services Config
	Datei aktiviert sein.
	
	Diese Option ist äquivalent mit der Befehlszeilen-
	Option -logchan.
	
	Anmerkung: Diese Option kann ein großes Sicherheitsproblem
	               darstellen, wenn der Logchannel nicht ausreichend
	               gesichert ist.

OPER_HELP_SET_DEBUG
	Syntax: SET DEBUG {ON | OFF | Nummer}
	
	Schaltet den Debug-Modus an oder aus. Wird der aktiviert,
	werden alle Daten von/an die Services gesendet werden
	(und ein paar anderen Debug-Meldungen) in ein Logfile
	geschrieben. Wird Nummer angegeben, wird der Debugging
	Level auf Nummer gesetzt.
	
	Diese Option ist äquivalent mit der Befehlszeilen-
	Option -debug.
	
OPER_HELP_SET_NOEXPIRE
	Syntax: SET NOEXPIRE {ON | OFF}
	
	Setzt den No-Expire-Mode an oder aus. Wird er
	aktiviert, werden Nicknamen, Channels und AKILLs nicht
	auslaufen bis dieser Modus wieder ausgeschaltet
	wird.

	Diese Option ist äquivalent mit der Befehlszeilen-
	Option -noexpire.

OPER_HELP_SET_SUPERADMIN
	Syntax: SET SUPERADMIN {ON | OFF}

	Der Super-Admin Modus verleiht dir zusätzliche Rechte, wie
	zB. Founder in jedem Channel, etc.

	Dieser Modus ist nicht dauerhaft und sollte nur genutzt
	werden, wenn er wirklich gebraucht wird. Danach sollte er
	sofort wieder ausgeschaltet werden.

OPER_HELP_SET_SQL
	Syntax: SET SQL {ON | OFF}

	Diese Einstellung wird Anope veranlassen SQL zu benutzen oder nicht.
	Kann nützlich sein wenn zB der MySQL-Server aus ist.

OPER_HELP_SET_IGNORE
	Syntax: SET IGNORE {ON | OFF}

	Mit diese Einstellung kannst du  Anope's IGNORE-System 
	an oder aus machen.

OPER_HELP_SET_LIST
	Syntax: SET LIST

	Zeigt dir eine Liste der verfügbaren Einstellungen von %S

OPER_HELP_NOOP
	Syntax: NOOP SET Server
	        NOOP REVOKE Server
	
	NOOP SET entfernt temporär alle O:Lines des 
	angegebenen Servers und killt alle IRCops die 
	gegenwärtig auf diesem verbunden sind, und verhindert 
	so das rehashen des Servers (dieses würde die Funktion
	aufheben).
	
	NOOP REVOKE macht alle O:Lines wieder verfügbar auf
	dem angegebenen Server.
	
	Achtung: Der Server wird in keinster Weise
	von den Services geprüft.

OPER_HELP_JUPE
	Syntax: JUPE Server [Grund]

	Durch die Services wird ein Server "gejupitert". Dies
	bedeutet, dass ein Server vorgetäuscht und so der
	echte Server daran gehindert wird, zu connecten. Der
	"Server" kann durch das normale SQUIT Befehls
	entfernt werden. Wird ein Grund angegeben,
	wird dieser im Server Informationsfeld angezeigt,
	ansonsten nur "Jupitered by <nick>".

OPER_HELP_RAW
	Syntax: RAW Text
	
	Sendet die Zeichenkette von text direkt an den Server,
	zu denen die Services verbunden sind. Dieses Kommando
	hat einen sehr beschränkten Nutzenfaktor, und kann
	leicht den Ablauf des Netzes stören, wenn es falsch
	benutz wird. BENUTZE DIESES KOMMANDO NICHT so lange
	du nicht genau weisst, was du tust!

OPER_HELP_UPDATE
	Syntax: UPDATE
	
	Erzwingt die Services, sofort alle Datenbanken auf der
	Festplatte zu aktualisieren.
	
OPER_HELP_RELOAD
	Syntax: RELOAD
	
	Die Services werden bei diesem Befehl die Konfigurations-
	datei neu einlesen. Beachte das für einige Direktiven
	trotzdem ein Neustart der Services erforderlich ist, um
	sie wirksam zu machen (so wie die Services Nicknamen,
	Aktivierung der Verbindungseinschränkungen, etc.)

OPER_HELP_QUIT
	Syntax: QUIT
	
	Zwingt die Services, sich sofort zu beenden. Die
	Datenbanken werden dabei nicht gespeichert.
	Dieser Befehl sollte nicht benutzt werden, ausser
	die Datenbanken sollen wirklich nicht aktualisiert
	werden. Für das normale Beenden der Services sollte
	der SHUTDOWN Befehl benutzt werden.

OPER_HELP_SHUTDOWN
	Syntax: SHUTDOWN
	
	Die Services werden durch dieses Befehl normal
	beendet und die Datenbanken werden gespeichert.

OPER_HELP_RESTART
	Syntax: RESTART
	
	Die Services werden sich bei diesem Befehl beenden,
	die Datenbanken speichern, und danach neustarten
	(die ausführbare Datei erneut aufrufen).

OPER_HELP_CHANLIST
	Syntax: CHANLIST [{Muster | Nickname} [SECRET]]
	
	Zeigt alle Channel an, die augenblicklich benutzt werden, 
	egal ob sie registriert sind oder nicht.
	
	Wird ein Muster angegeben, werden nur die dem Muster 
	entsprechenden Channel angezeigt. Wird ein Nickname 
	angegeben, werden nur die Channels aufgelistet, in denen 
	sich der User mit dem Nick befindet. Wird zusätzlich der 
	Parameter SECRET angegeben, werden nur Channel aufgelistet, 
	die Mode +s oder Mode +p gesetzt haben.
	
OPER_HELP_USERLIST
	Syntax: USERLIST [{Muster | Channel} [INVISIBLE]]
	
	Listet alle User auf, die gerade im IRC Netzwerk sind, egal ob
	ihr Nicknamen registriert ist oder nicht.
	
	Wird ein Muster angegeben, werden nur die User
	angezeigt, auf die das Muster passt (es muss im Format
	nick!user@host sein).  Wird ein Channel angegeben, werden
	nur die User angezeigt, die in diesem Channel sind. Wird
	zusätzlich INVISIBLE angegeben, werden nur User mit dem Flag
	+i angezeigt.
	
OPER_HELP_MODLOAD
	Syntax: MODLOAD Modul-Name

	Dieser Befehl lädt das benannte Module aus dem 
	modules - Verzeichnis.
	
OPER_HELP_MODUNLOAD
	Syntax: MODUNLOAD Modul-Name

	Dieser Befehl entfernt das benannte Module aus dem 
	modules - Verzeichnis.

OPER_HELP_MODINFO
	Syntax: MODINFO Modul-Name

	Dieser Befehl listet Informationen über das benannte, geladene 
	Module auf.

OPER_HELP_MODLIST
	Syntax: MODLIST [Core|3rd|protocol|encryption|supported|qatested]

	Listet alle derzeit geladenen Module auf.

###########################################################################
#
# BotServ help messages
#
###########################################################################


BOT_HELP_CMD_BOTLIST
	    BOTLIST       Zeigt alle vorhandenen Bots an
BOT_HELP_CMD_ASSIGN
	    ASSIGN        Weist einem Channel einen Bot zu
BOT_HELP_CMD_SET
	    SET           Konfiguriert die Bot-Optionen
BOT_HELP_CMD_KICK
	    KICK          Konfiguriert den Kicker (s.u.)
BOT_HELP_CMD_BADWORDS
	    BADWORDS      Verwaltet die Bad-Word-Liste
BOT_HELP_CMD_ACT
	    ACT           Veranlaßt den Bot ein "/me" Befehl ausführen
BOT_HELP_CMD_INFO
	    INFO          Erlaubt Dir, Informationen über einen ServicesBot anzuzeigen
BOT_HELP_CMD_SAY
	    SAY           Veranlaßt den Bot etwas in einem Channel zu sagen
BOT_HELP_CMD_UNASSIGN
	    UNASSIGN      Entfernt ein ServicesBot von einem Channel
BOT_HELP_CMD_BOT
	    BOT           Verwaltung von Bots

BOT_HELP
	%S ist dafür geschaffen, Bots in Channels
	einzubringen. Geschaffen wurde er für User, die
	selbst keinen Bot ausfetzen können, oder auch Netzwerke,
	die Bots verbieten. Mögliche Befehle siehe unten,
	die Syntax ist %R%S Befehl.
	
	Für weitere Hilfe zu einem bestimmten Befehl
	tippe %R%S HELP Befehl

BOT_HELP_FOOTER
	Ein Bot betritt den Channel erst, wenn sich %d User 
	in diesem Channel befinden.
	
BOT_HELP_BOTLIST
	Syntax: BOTLIST
	
	Zeigt alle verfügbaren Bots des Netzwerkes.
	
BOT_HELP_ASSIGN
	Syntax: ASSIGN Channel Botnick
	
	Weist einem Channel einen Bot zu. Der Botnick muss 
	jedoch existieren. Nach dieser Zuweisung können die 
	Channel-spezifischen Einstellungen des Bots angepasst 
	werden.
	
BOT_HELP_UNASSIGN
	Syntax: UNASSIGN Channel
	
	Löscht die Zuweisung eines Bots zu einem Channel.
	Dieser Befehl bewirkt, dass der Bot den Channel
	nicht mehr betritt. Die Konfiguration des Bots für den 
	Channel bleibt jedoch erhalten, sodass der Bot dem
	Channel wieder zugewiesen werden kann, ohne das 
	alle Einstellungen verloren gehen.

BOT_HELP_INFO
	Syntax: INFO {Channel | Botnick}
	
	Zeigt %S Informationen über einen Bot oder einen Channel
	an. Wenn der Parameter ein Channel ist, so werden 
	Informationen wie aktive Kicks etc. angezeigt, wenn der 
	Parameter ein Botnick ist, werden Informationen wie 
	Erstellungsdatum oder Anzahl der Channels angezeigt.

BOT_HELP_SET
	Syntax: SET (Channel | Bot) Option Parameter
	
	Konfiguriert Optionen für einen Bot.
	Option kann folgende Werte annehmen:
	
	    DONTKICKOPS       Chan-Ops werden (nicht) vom Bot 
	                      gekickt.
	    DONTKICKVOICES    Gevoicte User werden (nicht) 
	                      vom Bot gekickt.
	    GREET             Begrüssungsnachricht
	    FANTASY           Aktiviert Befehle wie !op !deop usw. 
	    SYMBIOSIS         Bot übernimmt die Aufgaben vom ChanServ
	
	%R%S HELP SET Option um weitere Informationen
	über einen spezifischen Befehl zu erhalten.
	
	Achtung: Die Berechtigungsstufen für diesen Befehl
	können variieren!

BOT_HELP_SET_DONTKICKOPS
	Syntax: SET Channel DONTKICKOPS {ON|OFF}
	
	Aktiviert / deaktiviert die Op Protection in einem
	Channel. Wenn diese aktiviert ist, werden Ops nicht vom
	Bot gekickt, auch wenn sie nicht das NOKICK-Level
	haben.

BOT_HELP_SET_DONTKICKVOICES
	Syntax: SET Channel DONTKICKVOICES {ON|OFF}
	
	Aktiviert oder deaktiviert die Voice Protection 
	in einem Channel. Wenn diese aktiviert ist, werden 
	User mit voice (+v) vom Bot nicht gekickt, auch 
	wenn sie nicht das NOKICK-Level haben.

BOT_HELP_SET_FANTASY
	Syntax: SET Channel FANTASY {ON|OFF}
	
	Aktiviert oder deaktiviert den Fantasy in einem
	Channel. Wenn dieser aktiviert ist, ist es Usern
	möglich, Befehle wie !op, !deop, !voice, !devoice,
	!kick, !kb, !unban, !seen und !halfop im Channel zu 
	benutzen. Die Befehle sind selbsterklärend, 
	probiert's halt mal aus, mit oder ohne Nickname.
	
	Hinweis: Auch hier benötigt der ausführende User
	die Rechte, um die Befehle zu benutzen, und zwar
	sowohl das Level um Fantasy zu nutzen, als auch
	ein Level bzw Mode, mit dem er das gegebene 
	Kommando ausführen könnte. (Beispiel: Jemand, 
	der kein Op ist, kann auch niemanden mit !op oppen.)
	
BOT_HELP_SET_GREET
	Syntax: SET Channel GREET {ON|OFF}
	
	Aktiviert oder deaktiviert den Greet Mode
	in einem Channel. Wenn diese Mode aktiviert ist, zeigt der Bot
	eine Begrüssungsnachricht bei joinenden Usern, die eine
	Msg definiert haben und weiterhin auch die 
	Berechtigung haben, dass diese angezeigt wird.
	
BOT_HELP_SET_SYMBIOSIS
	Syntax: SET Channel SYMBIOSIS {ON|OFF}
	
	Aktiviert oder deaktiviert den Symbiosis Mode.
	Wenn diese Mode aktiviert ist, führt der Bot alle Aktionen aus,
	die sonst %s übernimmt. 
	(Modechanges, Kicks, Begrüssungsnachrichten usw.)
	
BOT_HELP_KICK
	Syntax: KICK Channel Option Parameter
	
	Konfiguriert die Kicks des Bots.
	Option kann folgende Werte annehmen:
	
	    BOLDS        Bot kickt bei fettem Schriftstil (Bold)
	    BADWORDS     Bot kickt bei Bad words
	    CAPS         Bot kickt bei CAPS (Grossschrift)
	    COLORS       Bot kickt bei Farben
	    FLOOD        Bot kickt Flooder
	    REPEAT       Bot kickt User, die sich wiederholen.
	    REVERSES     Bot kickt bei invertiertem Schriftstil
	    UNDERLINES   Bot kickt bei Unterstreichungen
	
	%R%S HELP KICK Option um weitere 
	Informationen über einen bestimmten Befehl
	zu erhalten.
	
	Achtung: Die Berechtigungsstufen für diesen Befehl
	können variieren!

BOT_HELP_KICK_BOLDS
	Syntax: KICK Channel BOLDS {ON|OFF} [ttb]
	
	Kicken bei fettem Schriftstil an oder aus. Wenn 
	der Parameter ON gegeben ist, werden User, die fetten 
	Schriftstil verwenden, durch den Bot gekickt.
	
	ttb beschreibt wie oft der User gekickt wird, 
	bevor er einen Ban bekommt. Wenn keine ttb 
	angegeben wird, wird er nie gebannt.

BOT_HELP_KICK_COLORS
	Syntax: KICK Channel COLORS {ON|OFF} [ttb]
	
	Kicken bei Farben an oder aus. Wenn der Parameter
	ON gegeben ist, werden User, die Farben verwenden,
	durch den Bot gekickt.
	
	ttb beschreibt wie oft der User gekickt wird, bevor
	er einen Ban bekommt. Wenn keine ttb angegeben 
	wird, wird er nie gebannt.

BOT_HELP_KICK_REVERSES
	Syntax: KICK Channel REVERSES {ON|OFF} [ttb]
	
	Kicken bei invertiertem Schriftstil an oder aus. 
	Wenn der Parameter ON gegeben ist, werden User, die 
	invertierten Schriftstil verwenden, durch den Bot 
	gekickt.
	
	ttb beschreibt wie oft der User gekickt wird, bevor
	er einen Ban bekommt. Wenn keine ttb angegeben 
	wird, wird er nie gebannt.

BOT_HELP_KICK_UNDERLINES
	Syntax: KICK Channel UNDERLINES {ON|OFF} [ttb]
	
	Kicken bei Unterstreichungen an oder aus. Wenn der 
	Parameter ON gegeben ist, werden User, die 
	Unterstreichungen verwenden, durch den Bot gekickt.
	
	ttb beschreibt wie oft der User gekickt wird, bevor
	er einen Ban bekommt. Wenn keine ttb angegeben 
	wird, wird er nie gebannt.

BOT_HELP_KICK_CAPS
	Syntax: KICK Channel CAPS {ON|OFF} [ttb [min [prozente]]]
	
	Kicken bei CAPS an oder aus. Wenn der Parameter
	ON gegeben ist, werden User, die CAPS verwenden,
	durch den Bot gekickt.
	
	Der Bot kickt nur, wenn mindestens min Grossbuchstaben
	verwendet wurden und der Text mindestens prozente%% 
	Grossbuchstaben enthält. (Standard ist hier zehn Zeichen und
	25 %% - dieser wird angewendet, wenn die Paramter nicht
	angegeben werden.
	
	ttb beschreibt wie oft der User gekickt wird, bevor
	er einen Ban bekommt. Wenn keine ttb angegeben 
	wird, wird er nie gebannt.

BOT_HELP_KICK_FLOOD
	Syntax: KICK Channel FLOOD {ON|OFF} [ttb [ln [secs]]]
	
	Schaltet den Flood-Kicker an oder aus. Wenn er 
	aktiviert ist, kickt er User, die den Channel 
	mit mindestens ln Zeilen in secs flooden.
	Sollte weder ln noch secs angegeben sein,
	so werden defaultmässig 6 Zeilen / 10 Sekunden 
	gesetzt.
	
	ttb beschreibt wie oft der User gekickt wird, bevor
	er einen Ban bekommt. Wenn keine ttb angegeben 
	wird, wird er nie gebannt.

BOT_HELP_KICK_REPEAT
	Syntax: KICK Channel REPEAT {ON|OFF} [ttb [nummer]]
	
	Schaltet den Wiederholungs-Kicker an oder aus.Wenn 
	dieser aktiviert ist, wird ein User, der sich nummer 
	mal wiederholt, gekickt. Sollte kein nummer angegeben 
	sein, so wird 3 als Standardwert gesetzt.
	
	ttb beschreibt wie oft der User gekickt wird, bevor
	er einen Ban bekommt. Wenn keine ttb angegeben 
	wird, wird er nie gebannt.

BOT_HELP_KICK_BADWORDS
	Syntax: KICK Channel BADWORDS {ON|OFF} [ttb]
	
	Schaltet den Bad-Word-Kicker an oder aus. Wenn er 
	aktiviert ist, kickt der er den Bösewicht, der ein Wort
	aus der Bad-Word-Liste sagt.
	
	Die Bad-Words werden mit dem BADWORDS Befehl festgelegt.
	%R%S HELP BADWORDS um weitere Informationen zu erhalten.
	
	ttb beschreibt wie oft der User gekickt wird, bevor
	er einen Ban bekommt. Wenn keine ttb angegeben 
	wird, wird er nie gebannt.
	
BOT_HELP_BADWORDS
	Syntax: BADWORDS Channel ADD Wort [SINGLE | START | END]
	        BADWORDS Channel DEL {Wort | entry-num | Liste}
	        BADWORDS Channel LIST [Maske | Liste]
	        BADWORDS Channel CLEAR
	
	Verwaltet die Bad-Word-List eines Channels.
	
	In dieser Liste sind Wörter gespeichert, bei denen der
	entsprechende User aus dem Channel gekickt wird, wenn er 
	eines dieser Wörter erwähnt.
	
	Für mehr Informationen: %R%S HELP KICK BADWORDS.
	
	Der BADWORDS ADD Befehl fügt Wörter zur der
	Bad-Word-Liste hinzu. Wenn das Wort mit dem Paramter
	SINGLE angeben wird, so wird ein User nur dann gekickt,
	wenn er das ganze Wort sagt. Mit dem Parametern
	END / START wird der User gekickt, wenn er ein Wort sagt,
	das mit dem angegebenen Wort anfängt / endet.
	
	Wenn keine Parameter angegeben werden, wird der User,
	sobald er das angegebene Wort in irgendeiner Kombination
	sagt, gekickt.
	
	Der BADWORDS DEL Befehl löscht Einträge von der Bad- 
	Word-Liste. Auch hier gibt es die Möglichkeit, mehrere
	Einträge auf einmal zu löschen (siehe BADWORDS LIST).
	
	Der BADWORDS LIST Befehl zeigt Einträge der Bad-Word-Liste.
	Wildcards wie auch Bereiche werden interpretiert, z.B:
	
	BADWORDS #Channel LIST 2-5,7-9
	Zeigt die Einträge 2-5 und 7-9 der Bad-Word-Liste an.
	
	Der BADWORDS CLEAR Befehl löscht alle Einträge aus der
	Bad-Word-Liste.

BOT_HELP_SAY
	Syntax: SAY Channel Text
	
	Der Bot sagt den eingegebenen Text auf dem angegebenen Channel.

BOT_HELP_ACT
	Syntax: ACT Channel Text
	
	Bringt den Bot dazu, /me text auf dem angegebenen Channel auszugeben.

BOT_SERVADMIN_HELP_BOT
	Syntax: BOT ADD Botnick User Host Realname
	        BOT CHANGE Botnick Neuer-Botnick [User [Host [Realname]]]
	        BOT DEL Botnick
	
	Erlaubt Services Administratoren Bots zu erstellen/verändern/
	löschen, die die User in Ihren Channeln benutzen können.
	
	Der BOT ADD Befehl fügt einen Bot mit dem angegebenen 
	Botnick, User, Host und Realnamen hinzu. 
	Der BOT CHANGE Befehl ändert Attribute eines Bots. 
	Der BOT DEL Befehl löscht den angegebenen Bot aus der Botliste.  
	
	Achtung: Wenn ein Bot erstellt wird, dessen Nickname 
	registriert ist, wird dieser Nickname gelöscht. Falls 
	der User mit diesem Nicknamen online ist, wird dieser 
	entfernt (gekillt).

BOT_SERVADMIN_HELP_SET
	
	Diese Optionen sind nur durch Services Administratoren nutzbar:
	
	    NOBOT          Mit dieser Option kann man einem Channel 
	                   keinen Bot zuweisen und alle aktuellen 
	                   Zuweisungen aufheben.

	    PRIVATE        Verhindert das Bots von Nicht-IRC Operatoren
	                   assigned werden können.
	
BOT_SERVADMIN_HELP_SET_NOBOT
	Syntax: SET Channel NOBOT {ON|OFF}
	
	Durch diese Option kann einem Channel explizit kein Bot 
	zugewiesen werden. Sollte dem Channel, für den diese 
	Option gewählt wird, bereits ein Bot zugewiesen 
	sein, so verfällt die Zuweisung.
	
BOT_SERVADMIN_HELP_SET_PRIVATE
	Syntax: SET Botnick PRIVATE {ON|OFF}
	
	Verhindert, das der Bot durch Nicht-IRC Operatoren
	assigned werden kann.

###########################################################################
#
# HostServ messages
#
###########################################################################
HOST_EMPTY
	Die vHost Liste ist leer.
HOST_ENTRY
	#%d Nickname:%s, vHost:%s (%s - %s)
HOST_IDENT_ENTRY
	#%d Nickname:%s, vHost:%s@%s (%s - %s)
HOST_SET
	vHost für %s wurde gesetzt: %s.
HOST_IDENT_SET
	vHost für %s wurde gesetzt: %s@%s.
HOST_SETALL
	vHost für die Gruppe %s wurde gesetzt: %s.
HOST_DELALL
	vHost für die Gruppe %s wurden gelöscht.
HOST_DELALL_SYNTAX
	DELALL Nickname.
HOST_IDENT_SETALL
	vHost für die Gruppe %s wurde gesetzt: %s@%s.
HOST_SET_ERROR
	Fehler! Ein vHost darf nur die Zeichen A-Z, a-z, 0-9, '.' und '-' enthalten.
HOST_SET_IDENT_ERROR
	Fehler! Eine vHost-vIdent muss das  Format einer gültiger Ident haben.
HOST_SET_TOOLONG
	Fehler! Der vHost ist zu lang, bitte nutze einen vHost mit weniger als
	 %d Buchstaben.
HOST_SET_IDENTTOOLONG
	Fehler! Die Ident ist zu lang, bitte nutze eine Ident mit weniger als
	%d Buchstaben.
HOST_NOREG
	User %s wurde nicht in der NickServ Datenbank gefunden.
HOST_SET_SYNTAX
	SET <Nickname> <Hostmaske>.
HOST_SETALL_SYNTAX
	SETALL <Nickname> <Hostmaske>.
HOST_DENIED
	Zugriff verweigert.
HOST_NOT_ASSIGNED
	Bitte kontaktiere einen IRC Operator, um diesem Nicknamen einen vHost zuzuweisen.
HOST_ACTIVATED
	Dein vHost (%s) ist jetzt aktiviert.
HOST_IDENT_ACTIVATED
	Dein vHost (%s@%s) ist jetzt aktiviert.
HOST_ID
	Bitte identifiziere dich zuerst.
HOST_NOT_REGED
	Du musst erst deinen Nickname registrieren, um einen vHost zu erhalten.
HOST_DEL
	vHost für %s wurde gelöscht.
HOST_DEL_SYNTAX
	DEL Nickname.
HOST_OFF
	Dein vHost wurde deaktiviert und die normale Host-Verschlüsselung wiederhergestellt.
HOST_OFF_UNREAL
	Dein vHost wurde deaktiviert. Um die normale Host-Verschlüsselung zu reaktivieren, tippe /mode %s +%s
HOST_NO_VIDENT
	Dein IRCd unterstützt keine vIdents. Falls das inkorrekt sein sollte, melde das bitte als einen Bug.
HOST_GROUP
	Alle vHosts in der Gruppe %s wurden auf %s gesetzt.
HOST_IDENT_GROUP
	Alle vHosts in der Gruppe %s wurden auf %s@%s gesetzt.
HOST_LIST_FOOTER
	Liste alle Einträge. (Count: %d)
HOST_LIST_RANGE_FOOTER
	Einträge von %d bis %d
HOST_LIST_KEY_FOOTER
	Auf das Muster %s passenden Einträge. (Count: %d)

HOST_HELP_CMD_ON
	    ON          Aktiviert deinen vHost
HOST_HELP_CMD_OFF
	    OFF         Deaktiviert deinen vHost
HOST_HELP_CMD_GROUP
	    GROUP       Gleicht den vHost aller Nicknamen 
	                in einer Gruppe an

HOST_HELP_CMD_SET
	    SET         Setzt einem User einen vHost
HOST_HELP_CMD_SETALL
	    SETALL      Setzt einer Gruppe einen vHost
HOST_HELP_CMD_DEL
	    DEL         Löscht den vHost eines Users
HOST_HELP_CMD_DELALL
	    DELALL      Löscht den vHost einer Gruppe
HOST_HELP_CMD_LIST
	    LIST        Zeigt einen oder mehrere vHost Einträge an

HOST_OPER_HELP
	Nicht benutzt.

HOST_ADMIN_HELP
	Nicht benutzt.

HOST_HELP
	%S Befehle:

HOST_HELP_ON
	Syntax: ON

	Aktiviert den vHost der deinem Nicknamen zugewiesen wurde. 
	Wenn du diesen Befehl benutzt, wird jeder User, der ein /whois
	auf dich anwendet, diesen vHost anstatt deiner Host sehen.

HOST_HELP_SET
	Syntax: SET Nickname Hostmaske.

	Setzt einem User einen vHost anstelle des normalen
	Hosts.  Falls dein IRCd vIdents unterstützt, kannst
	du mit SET <nickname> <ident>@<hostmaske> auch eine Ident
	wie einen vHost setzen.

HOST_HELP_DELALL
	Syntax: DELALL Nickname.

	Löscht den vHost aller Nicknamen aus der gleichen Gruppe 
	des angegeben Nicknamens.

HOST_HELP_SETALL
	Syntax: SETALL Nickname Hostmaske.

	Setzt allen Usern einer Gruppe einen vHost. Falls dein
	IRCd vIdents unterstützt, kannst du mit 
	SETALL <nickname> <ident>@<hostmaske> eine Ident sowie 
	einen vHosts setzen.

	* Anmerkung: Dieser Befehl betrifft keine Nicknamen,
	die nachträglich zu der Gruppe hinzugefügt wurden.

HOST_HELP_OFF
	Syntax: OFF

	Deaktiviert den gerade genutzten vHost eines Nicknamens.
	Nach der Nutzung dieses Befehls wird jeder User, der
	ein /whois auf dich anwendet, deine echte IP Adresse
	sehen.

HOST_HELP_DEL
	Syntax: DEL Nickname

	Löscht den vHost des angegebenen Nicknamens.

HOST_HELP_LIST
	Syntax: LIST [Muster|#X-Y]

	Dieser Befehl listet dem Operator die registrierten vHost's.
	Wenn ein Muster angegeben ist, werden nur die Einträge des 
	Nicknamens oder die vHost's, die dem gegeben Muster entsprechen, 
	z.B. Rob* für alle Einträge, die mit "Rob" beginnen.

	Wenn ein #X-Y Style benutzt wird, werden nur die Einträge 
	zwischen den angegeben Faktoren angezeigt, z.B. #1-3 wird 
	die ersten drei Nicknamen/vHost-Einträge anzeigen.

	Die Liste benutzt jedes mal den Wert von NSListMax als ein 
	festes Limit für die Anzahl der dem Operator anzuzeigenden 
	Einträge.

HOST_HELP_GROUP
	Syntax:GROUP
	
	Dieser Befehl erlaubt es Usern, den vHost ihres aktuellen
	Nicknamens, zum vHost der gesamten Gruppe zu setzen.

OPER_SVSNICK_UNSUPPORTED
	SVSNICK ist auf diesem Netzwerk nicht verfügbar.

OPER_SQLINE_UNSUPPORTED
	SQLINE ist auf diesem Netzwerk nicht verfügbar.

OPER_SVSO_UNSUPPORTED
	OLINE ist auf diesem Netzwerk nicht verfügbar.

OPER_UMODE_UNSUPPORTED
	UMODE ist auf diesem Netzwerk nicht verfügbar.

OPER_SUPER_ADMIN_NOT_ENABLED
	SuperAdmin muss in der services.conf eingeschaltet werden.
