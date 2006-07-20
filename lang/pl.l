# Polski plik jêzykowy/Polish language file.
# T³umaczenie/Translation: 	
#		Mateusz Szczyrzyca <annihilator@ircnet.pl>, 
# 		Bartosz Mierzwiak <bartosz@mierzwiak.com>
# 		Admins of PolNet: http://www.ircnet.pl
# 		and Eskela: <eskela@royy.net>
#	
# Aktualizacja/Updated: 16/7/2004
# Wersja serwisów/Anope version: 1.7.4
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# (C) 2003-2005 Anope Team
# Contact us at info@anope.org
#   
# Please read COPYING and README for furhter details.
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
# (dev@anope.org) which message is causing a problem and I'll see
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
# Nazwa tego jêzyka
#
###########################################################################

# For languages other than English, this string should have the following
# format:
#     language-name-in-language (language-name-in-English)
# For example, "Español (Spanish)" or "Français (French)".

LANG_NAME
	Polski (Polish)

###########################################################################
#
# Podstawowe komunikaty
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
	Nie
	Pon
	Wto
	¦ro
	Czw
	Pia
	Sob
# %A
STRFTIME_DAYS_LONG
	Niedziela
	Poniedzia³ek
	Wtorek
	¦roda
	Czwartek
	Pi±tek
	Sobota
# %b
STRFTIME_MONTHS_SHORT
	Sty
	Lut
	Mar
	Kwi
	Maj
	Cze
	Lip
	Sie
	Wrz
	Pa¼
	Lis
	Gru
# %B
STRFTIME_MONTHS_LONG
	Styczeñ
	Luty
	Marzec
	Kwiecieñ
	Maj
	Czerwiec
	Lipiec
	Sierpieñ
	Wrzesieñ
	Pa¼dziernik
	Listopad
	Grudzieñ


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	, 


# Various error messages.
USER_RECORD_NOT_FOUND
	B³±d wewnêtrzny - nie moge wys³aæ ¿±dania do procesu.
UNKNOWN_COMMAND
	Nieznana komenda %s.
UNKNOWN_COMMAND_HELP
	Nieznana komenda %s. Napisz "/msg %s HELP" aby 
	uzyskaæ pomoc.
SYNTAX_ERROR
	Sk³adnia: %s
MORE_INFO
	Napisz /msg %s HELP %s aby uzyskaæ wiêcej 
	informacji.
NO_HELP_AVAILABLE
	Brak dostêpnej pomocy dla %s.
OBSOLETE_COMMAND
	Ta komenda jest ju¿ przestarza³a. 
	U¿yj %s.

BAD_USERHOST_MASK
	Maska musi miec format ident@host.
BAD_EXPIRY_TIME
	Nieprawid³owy czas wygasania.
USERHOST_MASK_TOO_WIDE
	%s jest nazbyt szczegó³owa i d³uga. 
	Proszê u¿yæ innej maski.

SERVICE_OFFLINE
	%s jest w tej chwili offline.
READ_ONLY_MODE
	Informacja: Services s± w trybie tylko do 
	odczytu - zmiany nie bêd± zapisywane!
PASSWORD_INCORRECT
	Nieprawid³owe has³o.
INVALID_TARGET
	"/msg %s" is no longer supported.  Use "/msg %s@%s" or "/%s" instead.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Odmowa dostêpu.
PERMISSION_DENIED
	Brak uprawnieñ.
RAW_DISABLED
	Opcja RAW zosta³a wy³aczona. Jesli chcesz jej u¿yæ, 
	dodaj DisableRaw w konfiguracji ¼róde³.

MORE_OBSCURE_PASSWORD
	Proszê spróbowaæ ponownie z trudniejszym has³em. 
	Has³o powinno mieæ minimum 5 znaków oraz powinno 
	zawieraæ cyfry i litery.
PASSWORD_TRUNCATED
	Informacja: Twoje has³o zosta³o skrócone 
	do %d znaków.

NICK_NOT_REGISTERED
	Twój nick jest niezarejestrowany.
NICK_NOT_REGISTERED_HELP
	Twój nick jest niezarejestrowany. 
	Napisz /msg %s HELP aby uzyskaæ pomoc na
	temat rejestracji Twojego nicka.
NICK_X_IS_SERVICES
	Nick %s jest czê¶ci± serwisów sieci.
NICK_X_NOT_REGISTERED
	Nick %s jest niezarejestrowany.
NICK_X_IN_USE
	Nick %s jest aktualnie u¿ywany.
NICK_X_NOT_IN_USE
	Nick %s nie jest aktualnie u¿ywany.
NICK_X_NOT_ON_CHAN
	%s nie jest aktualnie na kanale %s.
NICK_X_FORBIDDEN
	Nick %s nie mo¿e byæ rejestrowany lub u¿ywany.
NICK_X_FORBIDDEN_OPER
	Nick %s zosta³ porzucony przez %s:
	%s
NICK_X_ILLEGAL
	Nick %s jest nieprawid³owy i nie 
	mo¿e byæ u¿yty.
NICK_X_TRUNCATED
	Nick %s zosta³ skrócony do %d znaków.
NICK_X_SUSPENDED
	Nick %s is currently suspended.
CHAN_X_NOT_REGISTERED
	Kana³ %s jest niezarejestrowany.
CHAN_X_NOT_IN_USE
	Kana³ %s nie istnieje.
CHAN_X_FORBIDDEN
	Kana³ %s nie mo¿e byæ rejestrowany 
	lub u¿ywany.
CHAN_X_FORBIDDEN_OPER
	Kana³ %s zosta³ porzucony przez %s:
	%s
CHAN_X_SUSPENDED
	      Zawieszony: [%s] %s
NICK_IDENTIFY_REQUIRED
	Musisz siê zidentyfikowaæ, aby u¿yæ tej komendy.
	Napisz /msg %s IDENTIFY has³o,
	a pó¼niej ponów próbê.
CHAN_IDENTIFY_REQUIRED
	Do u¿ycia tej komendy wymagana jest identyfikacja.
	Napisz /msg %s IDENTIFY %s has³o, 
	a pó¼niej ponów próbê.
	
MAIL_DISABLED
	Services s± skonfigurowane bez supportu dla 
	wysy³ania e-maili.
MAIL_INVALID
	E-mail dla %s jest nieprawid³owy.
MAIL_X_INVALID
	%s jest nieprawid³owym adresem e-mail.
MAIL_LATER
	Nie mogê wys³aæ teraz maila. Proszê spróbowaæ za 
	chwile.
MAIL_DELAYED
	Proszê odczekaæ %d sekund(y) i ponowiæ próbê.
	
NO_REASON
	Brak powodu.
UNKNOWN
	<nieznane>

# Duration system
DURATION_DAY
	1 dzieñ
DURATION_DAYS
	%d dni
DURATION_HOUR
	1 godzin(y)
DURATION_HOURS
	%d godzin
DURATION_MINUTE
	1 minuta
DURATION_MINUTES
	%d minut(y)
DURATION_SECOND
	1 sekunda
DURATION_SECONDS
	%d sekund(y)

# Human readable expiration
NO_EXPIRE
	nie wygasa
EXPIRES_SOON
	wygasa przy nastêpnej aktualizacji baz
EXPIRES_M
	wygasa za %d minut(y)
EXPIRES_1M
	wygasa za %d minut(y)
EXPIRES_HM
	wygasa za %d godzin(y) i %d minut(y)
EXPIRES_H1M
	wygasa za %d godzin(y) i %d minuty
EXPIRES_1HM
	wygasa za %d godzinê(y) i %d minut(y)
EXPIRES_1H1M
	wygasa za %d godzinê i %d minut(y)
EXPIRES_D
	wygasa za %d dni
EXPIRES_1D
	wygasa za %d dzieñ

# Generic Footer message
END_OF_ANY_LIST
	Koniec listy %s.

###########################################################################
#
# Komunikaty NickServ
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	Ten nick jest zarejestrowany. Je¶li to Twój nick, 
	wpisz: /msg %s IDENTIFY has³o
NICK_IS_SECURE
	Ten nick jest zarejestrowany i pod ochron±.
	Je¶li jest to Twój nick zidentyfikuj siê 
	pisz±c: /msg %s IDENTIFY has³o. 
	Je¶li ten nick nie nale¿y do Ciebie, zmieñ go.
NICK_MAY_NOT_BE_USED
	Ten nick nie mo¿e byæ u¿ywany. Wybierz inny nick. 
FORCENICKCHANGE_IN_1_MINUTE
	Ostrzegam, ¿e je¶li nie zmienisz nicka w ci±gu 1 
	minuty, zostanie Ci on zmieniony si³±. 
FORCENICKCHANGE_IN_20_SECONDS
	Ostrzegam, ¿e je¶li nie zmienisz nicka w ci±gu 20 
	sekund, zostanie Ci on zmieniony si³±.
FORCENICKCHANGE_NOW
	Ten nick zosta³ zarejestrowany i nie mo¿esz go u¿yæ.
FORCENICKCHANGE_CHANGING
	Twój nick zosta³ zmieniony na %s.

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER has³o [email]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER has³o email
NICK_REGISTRATION_DISABLED
	B³±d, rejestracja tymczasowo niedostêpna.
NICK_REGISTRATION_FAILED
	B³±d, rejestracja nieudana.
NICK_REG_PLEASE_WAIT
	Proszê odczekaæ %d sekund przed ponownym u¿yciem 
	komendy REGISTER.
NICK_CANNOT_BE_REGISTERED
	Nick %s nie mo¿e byæ rejestrowany.
NICK_ALREADY_REGISTERED
	Nick %s jest ju¿ zarejestrowany!
NICK_REGISTERED
	Nick %s zosta³ teraz zarejestrowany dla 
	Ciebie z mask±: %s.
NICK_REGISTERED_NO_MASK
	Nickname %s registered.
NICK_PASSWORD_IS
	Twoje has³o nicka to %s
	Zapamiêtaj je do przysz³ego u¿ytku.
NICK_REG_DELAY
	Musisz byæ po³±czony(a) d³u¿ej ni¿ %d sekund(y), 
	aby móc zarejestrowaæ nick.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP nazwa has³o 
NICK_GROUP_DISABLED
	B³±d, grupowanie nicków jest tymczasowo niedostêpne.
NICK_GROUP_FAILED
	B³±d, groupowanie nieudane.
NICK_GROUP_PLEASE_WAIT
	Proszê odczekaæ %d sekund przed ponownym u¿yciem 
	komendy GROUP.
NICK_GROUP_CHANGE_DISABLED
	Twój nick jest ju¿ zarejestrowany. 
	Napisz wpierw: /msg %s DROP 
NICK_GROUP_SAME
	Nale¿ysz ju¿ do grupy %s.
NICK_GROUP_TOO_MANY
	Jest zbyt du¿o nicków w Twojej grupie: %s. 
	Proszê sprawdziæ i usun±æ czê¶æ z nich.
	Napisz /msg %s HELP GLIST oraz 
	/msg %s HELP DROP,
	aby uzyskaæ wiêcej informacji.
NICK_GROUP_JOINED
	Nale¿ysz od teraz do grupy %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY has³o
NICK_IDENTIFY_FAILED
	B³±d, identyfikacja nieudana.
NICK_IDENTIFY_SUCCEEDED
	Has³o przyjête. 
	Od teraz jeste¶ zidentyfikowany(a).
NICK_IDENTIFY_EMAIL_REQUIRED
	Musisz teraz ustawiæ adres e-mail przy Twoim nicku.
	Gdy zapomnisz has³a, zostanie ono wys³ane na e-mail.
NICK_IDENTIFY_EMAIL_HOWTO
	Napisz /msg %S SET EMAIL e-mail, 
	aby ustawiæ adres e-mail. Twój e-mail jest pod 
	ochron± - nie bêdzie wyjawiany osobom trzecim,
	ani nie bêdzie u¿ywany do celów innych ni¿ na 
	potrzeby Twojego nicka.
NICK_ALREADY_IDENTIFIED
	Jeste¶ ju¿ zidentyfikowany(a).

# UPDATE responses
NICK_UPDATE_SUCCESS
	Zaktualizowano (memosy, vhosty, chanmody, flagi).
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	Twój nick zosta³ pomy¶lnie wylogowany.
NICK_LOGOUT_X_SUCCEEDED
	Nick %s zosta³ wylogowany.
NICK_LOGOUT_SERVICESADMIN
	Nie mogê wylogowaæ %s poniewa¿ jest 
	Administratorem Serwisów.

# DROP responses
NICK_DROP_DISABLED
	B³±d, odrejestrowywanie nicków jest tymczasowo 
	niedostêpne.
NICK_DROPPED
	Twój nick zosta³ odrejestrowany.
NICK_X_DROPPED
	Nick %s zosta³ odrejestrowany.

# SET responses
NICK_SET_SYNTAX
	SET opcja parametr
NICK_SET_SERVADMIN_SYNTAX
	SET [nick] opcja parametr
NICK_SET_DISABLED
	B³±d, modyfikowanie ustawieñ nicka jest tymczasowo 
	niedostêpne.
NICK_SET_UNKNOWN_OPTION
	Nieznana opcja komendy SET %s.
NICK_SET_UNKNOWN_OPTION_OR_BAD_NICK
	Nieznana opcja komendy SET %s, 
	lub ten nick jest niezarejestrowany.
NICK_SET_OPTION_DISABLED
	Opcja %s jest niedostêpna na tej sieci.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	Nowa nazwa MUSI byæ nickiem z Twojej grupy nicków!
NICK_SET_DISPLAY_CHANGED
	Nowa nazwa to teraz %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Nie mo¿na zmieniæ has³a.
NICK_SET_PASSWORD_CHANGED
	Has³o zosta³o zmienione.
NICK_SET_PASSWORD_CHANGED_TO
	Has³o zosta³o zmienione na %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE numer
NICK_SET_LANGUAGE_UNKNOWN
	Nieznany numer jêzyka %d. 
	Napisz /msg %s HELP SET LANGUAGE, 
	aby uzyskaæ liste dostêpnych jêzyków.
NICK_SET_LANGUAGE_CHANGED
	Jêzyk zosta³ zmieniony na English.

# SET URL responses
NICK_SET_URL_CHANGED
	Adres WWW zosta³ zmieniony na %s.
NICK_SET_URL_UNSET
	Adres WWW zosta³ usuniêty.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	Adres email zosta³ zmieniony na %s.
NICK_SET_EMAIL_UNSET
	Adres email usuniêto.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	Nie mo¿esz usuwaæ adresu email na tej sieci.
	
# SET ICQ responses
NICK_SET_ICQ_CHANGED
	Numer ICQ zosta³ ustawiony na %s.
NICK_SET_ICQ_UNSET
	Numer ICQ zosta³ usuniêty.
NICK_SET_ICQ_INVALID
	%s jest nieprawid³owym numerem.
	
# SET GREET responses
NICK_SET_GREET_CHANGED
	Wiadomo¶æ powitalna zosta³a zmieniona na: 
	%s.
NICK_SET_GREET_UNSET
	Wiadomo¶æ powitalna zosta³a usuniêta.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Ochrona jest od teraz aktywna.
NICK_SET_KILL_QUICK
	Ochrona jest od teraz aktywna.
	Dodatkowo posiada opó¼niony czas dzia³ania.
NICK_SET_KILL_IMMED
	Ochrona jest od teraz aktywna. 
	Dzia³a natychmiastowo po próbie u¿ycia tego nicka.
NICK_SET_KILL_IMMED_DISABLED
	Opcja IMMED jest niedostêpna dla tej sieci.
NICK_SET_KILL_OFF
	Ochrona jest teraz nieaktywna.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Opcja bezpieczeñstwa jest od teraz aktywna.
NICK_SET_SECURE_OFF
	Opcja bezpieczeñstwa jest od teraz nieaktywna.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Opcja prywatno¶ci jest od teraz aktywna.
NICK_SET_PRIVATE_OFF
	Opcja prywatno¶ci jest od teraz nieaktywna.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Twój adres e-mail bêdzie od teraz ukrywany dla listy 
	INFO w %s.
NICK_SET_HIDE_EMAIL_OFF
	Twój adres e-mail bêdzie od teraz pokazywany w 
	li¶cie INFO w %s.
NICK_SET_HIDE_MASK_ON
	Twój ostatnio widziany ident@host bêdzie od teraz 
	ukrywany dla listy INFO w %s.
NICK_SET_HIDE_MASK_OFF
	Twój ostatnio widziany ident@host bêdzie od teraz 
	pokazywany w li¶cie INFO w %s.
NICK_SET_HIDE_QUIT_ON
	Twoja ostatnia wiadomo¶æ QUIT bêdzie od teraz 
	ukrywana dla listy INFO w %s.
NICK_SET_HIDE_QUIT_OFF
	Twoja ostatnia wiadomo¶æ QUIT bêdzie od teraz 
	pokazywana w li¶cie INFO w %s.
NICK_SET_HIDE_STATUS_ON
	Twój status dostêpu do serwisów bêdzie od teraz 
	ukrywany dla listy INFO w %s.
NICK_SET_HIDE_STATUS_OFF
	Twój status dostêpu do serwisów bêdzie od teraz 
	pokazywany w li¶cie INFO w %s.

# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_MSG_ON
	Od teraz serwisy bêd± siê z Tob± komunikowaæ przez 
	wiadomo¶ci na priv (query).
NICK_SET_MSG_OFF
	Od teraz serwisy bêd± siê z Tob± komunikowaæ przez 
	wiadomo¶ci notice.

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
	SSAET nickname PRIVATE {ON | OFF}
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
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [maska]
NICK_ACCESS_ALREADY_PRESENT
	Maska %s jest ju¿ w Twojej li¶cie dostêpu.
NICK_ACCESS_REACHED_LIMIT
	B³±d, mo¿esz mieæ tylko %d wpisy(ów) listy dostêpu 
	dla nicka.
NICK_ACCESS_ADDED
	%s dodano do Twojej listy dostêpu.
NICK_ACCESS_NOT_FOUND
	%s nie znaleziono na Twojej li¶cie dostêpu.
NICK_ACCESS_DELETED
	%s usuniêto z Twojej listy dostêpu.
NICK_ACCESS_LIST
	Lista dostêpu:
NICK_ACCESS_LIST_X
	Lista dostêpu dla %s:
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
	%s to %s
NICK_INFO_SERVICES_OPER
	%s to operator serwisów.
NICK_INFO_SERVICES_ADMIN
	%s to administrator serwisów.
NICK_INFO_SERVICES_ROOT
	%s to g³ówny admin serwisów.
NICK_INFO_ADDRESS
	Ostatni host: %s
NICK_INFO_ADDRESS_ONLINE
	Jest online z: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s jest w tej chwili online.
NICK_INFO_TIME_REGGED
	Data rejestracji: %s
NICK_INFO_LAST_SEEN
	Ostatnio widziano: %s
NICK_INFO_LAST_QUIT
	Ostatni QUIT: %s
NICK_INFO_URL
	URL: %s
NICK_INFO_EMAIL
	Adres e-mail: %s
NICK_INFO_VHOST
	vhost: %s
NICK_INFO_VHOST2
	vhost: %s@%s
NICK_INFO_ICQ
	Numer ICQ: %d
NICK_INFO_GREET
	Wiadomo¶æ powitalna: %s
NICK_INFO_OPTIONS
	Opcje: %s
NICK_INFO_EXPIRE
	Expires in: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Chroniony
NICK_INFO_OPT_SECURE
	Zabezpieczony
NICK_INFO_OPT_PRIVATE
	Prywatny
NICK_INFO_OPT_MSG
	Komunikacja na priv
NICK_INFO_OPT_NONE
	Brak
NICK_INFO_NO_EXPIRE
	Ten nick nie podlega prawu wygasania.
NICK_INFO_FOR_MORE
	Aby uzyskaæ jeszcze wiêcej informacji, 
	napisz /msg %s INFO %s ALL.

# LIST responses
NICK_INFO_SUSPENDED
	This nickname is currently suspended, reason: %s
NICK_INFO_SUSPENDED_NO_REASON
	This nickname is currently suspended

# LIST responses
NICK_LIST_SYNTAX
	LIST wzór
NICK_LIST_SERVADMIN_SYNTAX
	LIST wzór [FORBIDDEN] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Lista pasuj±cych wpisów %s:
NICK_LIST_RESULTS
	Koniec listy - %d/%d wpisów wy¶wietlono.

# ALIST responses
NICK_ALIST_SYNTAX
	ALIST nick
NICK_ALIST_HEADER
	Kana³y, na których jeste¶ na li¶cie dostêpu:
	  Nr   Kana³              Poziom    Opis 
NICK_ALIST_HEADER_X
	Kana³y, na których %s jest na li¶cie dostêpu:
	  Nr   Kana³              Poziom    Opis
NICK_ALIST_XOP_FORMAT
	  %3d %c%-20s %-8s %s
NICK_ALIST_ACCESS_FORMAT
	  %3d %c%-20s %-8d %s
NICK_ALIST_FOOTER
	Koniec listy - %d/%d kana³ów pokazano.

# GLIST responses
NICK_GLIST_HEADER
	Lista cz³onków w Twojej grupie:
NICK_GLIST_HEADER_X
	Lista cz³onków grupy %s:
NICK_GLIST_FOOTER
	%d cz³onków w grupie.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (expires in %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nick [has³o]
NICK_NO_RECOVER_SELF
	Nie mo¿esz sobie samemu odzyskaæ nicka :P
NICK_RECOVERED
	Twój nick zosta³ w³a¶nie przejêty przez serwisy.
	Napisz /msg %s RELEASE %s 
	je¶li chcesz, by zosta³ on zwolniony teraz.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nick [has³o]
NICK_RELEASE_NOT_HELD
	Nie trzymam nicka %s
NICK_RELEASED
	Serwisy w³a¶nie zwolni³y nicka na Twoje ¿±danie.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nick [has³o]
NICK_NO_GHOST_SELF
	Nie mo¿esz takich rzeczy robiæ.
NICK_GHOST_KILLED
	Nick zosta³ usuniêty z sieci.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nick
NICK_GETPASS_UNAVAILABLE
	Komenda GETPASS jest niedostêpna poniewa¿ has³a s± 
	szyfrowane.
NICK_GETPASS_PASSWORD_IS
	Has³o dla %s to %s.

# GETEMAIL responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host 
NICK_GETEMAIL_EMAILS_ARE
	Adresy e-mail %s pasuj±ce do %s.
NICK_GETEMAIL_NOT_USED
	Brak listy e-mail dla %s.
	
# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS nick
NICK_SENDPASS_UNAVAILABLE
	Komenda SENDPASS jest niedostêpna poniewa¿ has³a s± 
	szyfrowane.
NICK_SENDPASS_SUBJECT
	Has³o nicka (%s)
NICK_SENDPASS_HEAD
	Witaj,
NICK_SENDPASS_LINE_1
	Zarz±da³e¶(a¶) przypomnenia has³a do Twojego nicka %s 
	na e-mail.
NICK_SENDPASS_LINE_2
	Twoje has³o to: %s 
	Ze wzglêdów bezpieczeñstwa zalecana jest natychmiastowa 
	zmiana  tego has³a na inne zaraz po identyfikacji.
NICK_SENDPASS_LINE_3
	Je¶li nie wiesz dlaczego ta wiadomo¶æ dotar³a do Ciebie
	to zignoruj j±. Je¶li ta sytuacja bêdzie siê powtarzaæ, 
	skontaktuj siê z ircoperatorem sieci.
NICK_SENDPASS_LINE_4
	Proszê nie odpowiadaæ na t± wiadomo¶æ.
NICK_SENDPASS_LINE_5
	Z wyrazami szacunku, 
	Administratorzy %s.
NICK_SENDPASS_OK
	Has³o do %s zosta³o wys³ane.

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
	FORBID nick [powód]
NICK_FORBID_SYNTAX_REASON
	FORBID nick powód
NICK_FORBID_SUCCEEDED
	Nick %s zosta³ porzucony.
NICK_FORBID_FAILED
	Nie mogê porzuciæ nicka %s!

# Nick Registraion responses
NICK_REQUESTED
	Ten nick zosta³ ju¿ zarejestrowany. Sprawd¼ swoj± 
	poczte.
NICK_REG_RESENT
	Twój kod rejestracyjny zosta³ ponownie wys³any 
	na %s.
NICK_REG_UNABLE
	Nick niezarejestrowany. Proszê spróbowaæ pó¼niej.
NICK_IS_PREREG
	Ten nick czeka na email z kodem weryfikacyjnym 
	przed zakoñczeniem rejestracji.
NICK_ENTER_REG_CODE
	Kod zosta³ wys³any na %s. 
	Napisz /msg %s confirm <kod>, 
	aby dokoñczyæ procedure rejestracji.
NICK_CONFIRM_NOT_FOUND
	Termin wa¿no¶ci kodu rejestrcyjnego up³yn±³. 
	Napisz: "/msg %s REGISTER <password> <email>",
	aby znowu zacz±æ rejestracje.
NICK_CONFIRM_INVALID
	Podano nieprawid³owy kod. Proszê sprawdziæ e-mail 
	i spróbowaæ ponownie.
NICK_REG_MAIL_SUBJECT
	Rejestracja nicka (%s)
NICK_REG_MAIL_HEAD
	Witaj,
NICK_REG_MAIL_LINE_1
	Chcesz zarejestrowaæ sobie nastêpuj±cy nick: %s.
NICK_REG_MAIL_LINE_2
	Proszê napisaæ na IRC " /msg %s confirm %s " aby dokoñczyæ 
	rejestracje.
NICK_REG_MAIL_LINE_3
	Je¶li nie wiesz dlaczego ta wiadomo¶æ dotar³a do Ciebie to 
	zignoruj j±. Je¶li ta sytuacja bêdzie siê powtarzaæ, skontaktuj 
	siê z ircoperatorem sieci.		
NICK_REG_MAIL_LINE_4
	Proszê nie odpowiadaæ na t± wiadomo¶æ.
NICK_REG_MAIL_LINE_5
	Z wyrazami szacunku,
	Administratorzy %s.
NICK_GETPASS_PASSCODE_IS
	Kod rejestracyjny dla %s to %s.
NICK_FORCE_REG
	Nick %s potwierdzony.



###########################################################################
#
# Komunikaty ChanServ.
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Automatyczny op (@).
CHAN_LEVEL_AUTOVOICE
	Automatyczny voice (+v).
CHAN_LEVEL_AUTOHALFOP
	Automatyczny halfop (+h).
CHAN_LEVEL_AUTOPROTECT
	Automatyczny protect (+a).
CHAN_LEVEL_AUTODEOP
	Automatyczne deopowanie.
CHAN_LEVEL_NOJOIN
	Nie pozwala wej¶æ na kana³ w trybie RESTRICTED.
CHAN_LEVEL_INVITE
	Pozwala u¿ywac komendy INVITE.
CHAN_LEVEL_AKICK
	Pozwala u¿ywaæ komendy AKICK.
CHAN_LEVEL_SET
	Pozwala modyfikowaæ ustawienia kana³u.
CHAN_LEVEL_CLEAR
	Pozwala u¿ywaæ komendy CLEAR.
CHAN_LEVEL_UNBAN
	Pozwala u¿ywaæ komendy UNBAN.
CHAN_LEVEL_OPDEOP
	Pozwala u¿ywaæ komend OP i DEOP. 
CHAN_LEVEL_ACCESS_LIST
	Pozwala zagl±daæ do kana³owej listy dostêpu.
CHAN_LEVEL_ACCESS_CHANGE
	Pozwala zmieniaæ kana³ow± listê dostêpu.
CHAN_LEVEL_MEMO
	Pozwala listowaæ i czytaæ kana³owe wiadomo¶ci.
CHAN_LEVEL_ASSIGN
	Pozwala stawiaæ i zdejmowaæ bota.
CHAN_LEVEL_BADWORDS
	Pozwala u¿ywaæ komendy BADWORDS.
CHAN_LEVEL_NOKICK
	Ochrona przed kopanem za pomoc± komendy KICK.
CHAN_LEVEL_FANTASIA
	Pozwala u¿ywaæ komend FANTASTIC.
CHAN_LEVEL_SAY
	Pozwala u¿ywaæ komendy SAY i ACT. 
CHAN_LEVEL_GREET
	Wy¶wietla wiadomo¶æ powitaln± (greeta).
CHAN_LEVEL_VOICEME
	Pozwala zabieraæ voice(+v) sobie samemu.
CHAN_LEVEL_VOICE
	Pozwala u¿ywaæ komendy VOICE i DEVOICE.
CHAN_LEVEL_GETKEY
	Pozwala u¿ywaæ komendy GETKEY.
CHAN_LEVEL_OPDEOPME
	Pozwala zabieraæ opa(@) sobie samemu/samej.
CHAN_LEVEL_HALFOPME
	Pozwala zabieraæ halfopa(%) sobie samemu/samej.
CHAN_LEVEL_HALFOP
	Pozwala u¿ywaæ komendy HALFOP i DEHALFOP.
CHAN_LEVEL_PROTECTME
	Pozwala w³±czaæ/wy³±czaæ ochrone samego siebie.
CHAN_LEVEL_PROTECT
	Pozwala u¿ywaæ komendy PROTECT i DEPROTECT.
CHAN_LEVEL_KICKME
	Pozwala kopaæ siebie samego.
CHAN_LEVEL_KICK
	Pozwala u¿ywaæ komendy KICK.
CHAN_LEVEL_SIGNKICK
	Nie podpisze kicka, gdy SIGNKICK LEVEL jest u¿ywany.
CHAN_LEVEL_BANME
	Pozwala banowaæ siebie samego.
CHAN_LEVEL_BAN
	Pozwala u¿ywaæ komendy BAN.
CHAN_LEVEL_TOPIC
	Pozwala u¿ywaæ komendy TOPIC.
CHAN_LEVEL_INFO
	Pozwala u¿ywaæ opcji ALL w komendzie INFO.

# Automatic responses
CHAN_IS_REGISTERED
	Tem kana³ zosta³ zarejestrowany z %s.
CHAN_NOT_ALLOWED_OP
	Nie masz uprawnieñ do posiadania statusu operatora 
	na %s.
CHAN_MAY_NOT_BE_USED
	Ten kana³ nie mo¿e byæ u¿ywany.
CHAN_NOT_ALLOWED_TO_JOIN
	Nie masz pozwolenia, aby przebywaæ na tym kanale.

# REGISTER responses
CHAN_X_INVALID
	Channel %s is not a valid channel.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER kana³ has³o opis
CHAN_REGISTER_DISABLED
	B³±d, rejestracja kana³u jest tymczasowo 
	niedostêpna.
CHAN_REGISTER_NOT_LOCAL
	Kana³y lokalne nie mog± byæ rejestrowane.
CHAN_MUST_REGISTER_NICK
	Musisz mieæ wpierw zarejestrowany nick. 
	Napisz /msg %s HELP, aby uzyskaæ 
	wiêcej informacji odno¶nie rejestracji nicka.
CHAN_MUST_IDENTIFY_NICK
	Proszê zidentyfikowaæ najpierw nicka w %s 
	komend±: /msg %s IDENTIFY has³o
CHAN_MAY_NOT_BE_REGISTERED
	Kana³ %s nie mo¿e byæ zarejestrowany.
CHAN_ALREADY_REGISTERED
	Kana³ %s jest ju¿ zarejestrowany!
CHAN_MUST_BE_CHANOP
	Aby zarejestrowaæ kana³, musisz byæ jego 
	operatorem (posiadaæ status @).
CHAN_REACHED_CHANNEL_LIMIT
	B³±d, ju¿ masz osi±gniêty limit %d kana³ów.
CHAN_EXCEEDED_CHANNEL_LIMIT
	B³±d, ju¿ masz przekroczony limit %d kana³ów.
CHAN_REGISTRATION_FAILED
	B³±d, rejestracja nieudana.
CHAN_REGISTERED
	Kana³ %s zosta³ teraz zarejestrowany. 
	Jako %s jeste¶ od teraz jego w³a¶cicielem.
CHAN_PASSWORD_IS
	Has³o Twojego kana³u to %s 
	Zapamiêtaj do przysz³ego u¿ytku.
CHAN_REGISTER_NONE_CHANNEL
	You have attempted to register a none existant channel %s
CHAN_SYMBOL_REQUIRED
	Please use the symbol of # or & when attempting to register


# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY kana³ has³o
CHAN_IDENTIFY_FAILED
	B³±d, identyfikacja nieudana.
CHAN_IDENTIFY_SUCCEEDED
	Has³o przyjête. 
	Jeste¶ teraz w³a¶cicielem(k±) kana³u %s.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT kana³ nick
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT kana³ [nick]
CHAN_LOGOUT_SUCCEEDED
	U¿ytkownik %s zosta³ wylogowany z kana³u %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	Wszyscy zidentyfikowani u¿ytkownicy zostali 
	wylogowani z kana³u %s.

# DROP responses
CHAN_DROP_SYNTAX
	DROP kana³
CHAN_DROP_DISABLED
	B³±d, odrejestrowywanie kana³ów jest tymczasowo 
	niedostêpne.
CHAN_DROPPED
	Kana³ %s zosta³ odrejestrowany.

# SET responses
CHAN_SET_SYNTAX
	SET kana³ opcja parametry
CHAN_SET_DISABLED
	B³±d, modyfikowanie ustawieñ kana³u jest tymczasowo
	niedostêpne.
CHAN_SET_UNKNOWN_OPTION
	Nieznana opcja komendy SET: %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s posiada za du¿o zarejestrowanych kana³ów.
CHAN_FOUNDER_CHANGED
	Za³o¿ycielem %s jest teraz %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Zastêpcê dla %s zmieniono na %s.
CHAN_SUCCESSOR_UNSET
	Zastêpcê dla %s usuniêto.
CHAN_SUCCESSOR_IS_FOUNDER
	%s nie mo¿e byæ zastêpc± na kanale %s 
	poniewa¿ jest ju¿ jego za³o¿ycielem.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	B³±d, nie mogê zmieniæ has³a.
CHAN_PASSWORD_CHANGED
	Has³o %s zosta³o zmienione.
CHAN_PASSWORD_CHANGED_TO
	Has³o %s zosta³o zmienione na %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Opis kana³u %s zosta³ zmieniony na %s.

# SET URL responses
CHAN_URL_CHANGED
	Adres WWW dla %s zosta³ zmieniony na %s.
CHAN_URL_UNSET
	Adres WWW dla %s zosta³ usuniêty.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	Adres e-mail dla %s zosta³ zmieniony na %s.
CHAN_EMAIL_UNSET
	Adres e-mail dla %s zosta³ usuniêty.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Wiadomo¶æ powitalna %s zosta³a zmieniona.
CHAN_ENTRY_MSG_UNSET
	Wiadomo¶æ powitalna %s zosta³a usuniêta.
	
# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s jest nieprawid³owym rodzajem bana.
CHAN_SET_BANTYPE_CHANGED
	Rodzaj bana dla kana³u %s to teraz #%d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Nieznany znak trybu %c.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Chanmod %c zosta³ zignorowany poniewa¿ nie 
	mo¿esz go zablokowaæ.
CHAN_SET_MLOCK_L_REQUIRED
	Musisz za³o¿yæ +l zanim za³o¿ysz +L.
CHAN_SET_MLOCK_K_REQUIRED
	Musisz za³o¿yæ +i równie¿, aby móc za³o¿yæ +K.
CHAN_MLOCK_CHANGED
	Blokada chanmodów kana³u %s zosta³a zmieniona 
	na tak±: %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET kana³ KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Topic retention option for %s is now ON.
CHAN_SET_KEEPTOPIC_OFF
	Topic retention option for %s is now OFF.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET kana³ TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Topic lock option for %s is now ON.
CHAN_SET_TOPICLOCK_OFF
	Topic lock option for %s is now OFF.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET kana³ PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Peace option for %s is now ON.
CHAN_SET_PEACE_OFF
	Peace option for %s is now OFF.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET kana³ PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Private option for %s is now ON.
CHAN_SET_PRIVATE_OFF
	Private option for %s is now OFF.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET kana³ SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Secure ops option for %s is now ON.
CHAN_SET_SECUREOPS_OFF
	Secure ops option for %s is now OFF.

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET kana³ SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Secure founder option for %s is now ON.
CHAN_SET_SECUREFOUNDER_OFF
	Secure founder option for %s is now OFF.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET kana³ RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Restricted access option for %s is now ON.
CHAN_SET_RESTRICTED_OFF
	Restricted access option for %s is now OFF.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET kana³ SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Secure option for %s is now ON.
CHAN_SET_SECURE_OFF
	Secure option for %s is now OFF.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET kana³ SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Signed kick option for %s is now ON.
CHAN_SET_SIGNKICK_LEVEL
	Signed kick option for %s is now ON, but depends of the
	level of the user that is using the command.
CHAN_SET_SIGNKICK_OFF
	Signed kick option for %s is now OFF.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET kana³ OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice option for %s is now ON.
CHAN_SET_OPNOTICE_OFF
	Op-notice option for %s is now OFF.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET kana³ XOP {ON | OFF}
CHAN_SET_XOP_ON
	xOP lists system for %s is now ON.
CHAN_SET_XOP_OFF
	xOP lists system for %s is now OFF.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET kana³ NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Kana³ %s od teraz nie podlega prawu wygasania.
CHAN_SET_NOEXPIRE_OFF
	Kana³ %s od teraz podlega prawu wygasania.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	B³±d, mo¿e byæ %d AOP/SOP/VOP wpisów na kanale.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	Nie mo¿esz u¿yæ tej komendy. U¿yj komendy ACCESS.
	Napisz /msg %s HELP ACCESS, 
	aby uzyskaæ wiêcej informacji.

# AOP messages
CHAN_AOP_SYNTAX
	AOP kana³ {ADD|DEL|LIST|CLEAR} [nick | pozycja]
CHAN_AOP_DISABLED
	B³±d, modyfikowanie kana³owej listy AOP jest 
	tymczasowo niedostêpne.
CHAN_AOP_NICKS_ONLY
	Kana³owa lista AOP mo¿e posiadaæ jedynie 
	zarejestrowane nicki.
CHAN_AOP_ADDED
	%s dodano do listy AOP %s.
CHAN_AOP_MOVED
	%s przeniesiono do listy AOP %s.
CHAN_AOP_NO_SUCH_ENTRY
	Brak wpisu (#%d) na AOP li¶cie %s.
CHAN_AOP_NOT_FOUND
	%s nie znaleziono na AOP li¶cie %s.
CHAN_AOP_NO_MATCH
	Nie znaleziono wpisów w li¶cie AOP %s.
CHAN_AOP_DELETED
	%s usuniêto z listy AOP %s.
CHAN_AOP_DELETED_ONE
	Usuniêto 1 wpis z listy AOP %s.
CHAN_AOP_DELETED_SEVERAL
	Usuniêto %d wpisów(y) z listy AOP %s.
CHAN_AOP_LIST_EMPTY
	Lista AOP %s jest pusta.
CHAN_AOP_LIST_HEADER
	Lista AOP dla %s:
	  Nr   Nick
CHAN_AOP_CLEAR
	Kana³owa %s lista AOP zosta³a wyczyszczona.

# HOP messages
CHAN_HOP_SYNTAX
	HOP kana³ {ADD|DEL|LIST|CLEAR} [nick | pozycja]
CHAN_HOP_DISABLED
	B³±d, modyfikowanie kana³owej listy HOP jest 
	tymczasowo niedostêpne.
CHAN_HOP_NICKS_ONLY
	Kana³owa lista HOP mo¿e posiadaæ jedynie 
	zarejestrowane nicki.
CHAN_HOP_ADDED
	%s dodano do listy HOP %s.
CHAN_HOP_MOVED
	%s przeniesiono do listy HOP %s.
CHAN_HOP_NO_SUCH_ENTRY
	Brak wpisu (#%d) na li¶cie HOP %s.
CHAN_HOP_NOT_FOUND
	%s nie znaleziono na li¶cie HOP %s.
CHAN_HOP_NO_MATCH
	Nie znaleziono wpisów na li¶cie HOP %s.
CHAN_HOP_DELETED
	%s usuniêto z listy HOP %s.
CHAN_HOP_DELETED_ONE
	Usuniêto 1 wpis z listy HOP %s.
CHAN_HOP_DELETED_SEVERAL
	Usuniêto %d wpisów(y) z listy HOP %s.
CHAN_HOP_LIST_EMPTY
	Lista HOP %s jest pusta.
CHAN_HOP_LIST_HEADER
	Lista HOP dla %s:
	  Nr   Nick
CHAN_HOP_CLEAR
	Kana³owa %s lista HOP zosta³a wyczyszczona.

# SOP messages
CHAN_SOP_SYNTAX
	SOP kana³ {ADD|DEL|LIST|CLEAR} [nick | pozycja]
CHAN_SOP_DISABLED
	B³±d, modyfikowanie kana³owej listy SOP jest 
	tymczasowo niedostêpne
CHAN_SOP_NICKS_ONLY
	Kana³owa lista SOP mo¿e posiadaæ jedynie 
	zarejestrowane nicki.
CHAN_SOP_ADDED
	%s dodano do listy SOP %s.
CHAN_SOP_MOVED
	%s przeniesiono do listy SOP %s.
CHAN_SOP_NO_SUCH_ENTRY
	Brak wpisu (#%d) na li¶cie SOP %s.
CHAN_SOP_NOT_FOUND
	%s nie znaleziono na li¶cie SOP %s.
CHAN_SOP_NO_MATCH
	Nie znaleziono wpisów na li¶cie SOP %s.
CHAN_SOP_DELETED
	%s usuniêto z listy SOP %s.
CHAN_SOP_DELETED_ONE
	Usuniêto 1 wpis z listy SOP %s.
CHAN_SOP_DELETED_SEVERAL
	Usuniêto %d wpisów(y) z listy SOP %s.
CHAN_SOP_LIST_EMPTY
	Lista SOP %s jest pusta.
CHAN_SOP_LIST_HEADER
	Lista SOP dla %s:
	  Nr   Nick
CHAN_SOP_CLEAR
	Kana³owa %s lista SOP zosta³a wyczyszczona.

# VOP messages
CHAN_VOP_SYNTAX
	VOP #kana³ {ADD|DEL|LIST|CLEAR} [nick | pozycja]
CHAN_VOP_DISABLED
	B³±d, modyfikowanie kana³owej listy VOP jest 
	tymczasowo niedostêpne.
CHAN_VOP_NICKS_ONLY
	Kana³owa lista VOP mo¿e posiadaæ jedynie 
	zarejestrowane nicki.
CHAN_VOP_ADDED
	%s dodano do listy VOP %s.
CHAN_VOP_MOVED
	%s przeniesiono do listy VOP %s.
CHAN_VOP_NO_SUCH_ENTRY
	Brak wpisu (#%d) na li¶cie VOP %s.
CHAN_VOP_NOT_FOUND
	%s nie znaleziono na li¶cie VOP %s.
CHAN_VOP_NO_MATCH
	Nie znaleziono wpisów na li¶cie VOP %s.
CHAN_VOP_DELETED
	%s usuniêto z listy VOP %s.
CHAN_VOP_DELETED_ONE
	Usuniêto 1 wpis z listy VOP %s.
CHAN_VOP_DELETED_SEVERAL
	Usuniêto %d wpisów(y) z listy VOP %s.
CHAN_VOP_LIST_EMPTY
	Lista VOP %s jest pusta.
CHAN_VOP_LIST_HEADER
	Lista VOP dla %s:
	  Nr   Nick
CHAN_VOP_CLEAR
	Kana³owa %s lista VOP zosta³a wyczyszczona.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS kana³ {ADD|DEL|LIST|CLEAR} [nick [poziom] | pozycja]
CHAN_ACCESS_XOP
	Nie mo¿esz u¿yæ tej komendy. 
	Musisz u¿yæ VOP, HOP, AOP lub SOP.
	Napisz /msg %s HELP komenda,
	aby uzyskaæ wiêcej informacji.
CHAN_ACCESS_DISABLED
	B³±d, modyfikacje kana³owej listy dostêpu s± 
	tymczasowo niedostêpne.
CHAN_ACCESS_LEVEL_NONZERO
	Poziom dostêpu nie mo¿e byæ zerem.
CHAN_ACCESS_LEVEL_RANGE
	Poziom dostêpu musi siê zawieraæ pomiêdzy %d a %d.
CHAN_ACCESS_NICKS_ONLY
	Kana³owa lista dostêpu mo¿e zawieraæ tylko 
	zarejestrowane nicki.
CHAN_ACCESS_REACHED_LIMIT
	B³±d, mo¿esz mieæ tylko %d wpisów na li¶cie dostêpu 
	kana³u.
CHAN_ACCESS_LEVEL_UNCHANGED
	Poziom dostêpu dla %s na %s jest 
	nadany od %d.
CHAN_ACCESS_LEVEL_CHANGED
	Poziom dostêpu dla %s na %s zmieniono 
	teraz na %d.
CHAN_ACCESS_ADDED
	%s dodano do listy dostêpu %s z 
	poziomem %d.
CHAN_ACCESS_NO_SUCH_ENTRY
	Brak wpisu (#%d) na li¶cie dostêpu %s.
CHAN_ACCESS_NOT_FOUND
	%s nie znaleziono na li¶cie dostêpu %s.
CHAN_ACCESS_NO_MATCH
	Nie znaleziono wpisów na li¶cie dostêpu %s.
CHAN_ACCESS_DELETED
	%s usuniêto z listy dostêpu %s.
CHAN_ACCESS_DELETED_ONE
	Usuniêto 1 wpis z listy dostêpu %s.
CHAN_ACCESS_DELETED_SEVERAL
	Usuniêto %d wpisów(y) z listy dostêpu %s.
CHAN_ACCESS_LIST_EMPTY
	Kana³owa lista dostêpu dla %s jest pusta.
CHAN_ACCESS_LIST_HEADER
	Kana³owa lista dostêpu dla %s:
	  Nr   Poz  Nick
CHAN_ACCESS_LIST_FOOTER
	Koniec listy dostêpu.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	Lista dostêpu kana³u %s zosta³a wyczyszczona.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK kana³ {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [nick-lub-maska] [powód]
CHAN_AKICK_DISABLED
	B³±d, modyfikacje kana³owej listy AKICK s± 
	tymczasowo niedostêpne.
CHAN_AKICK_ALREADY_EXISTS
	%s ju¿ istnije w li¶cie AKICK %s.
CHAN_AKICK_REACHED_LIMIT
	B³±d, mo¿esz mieæ jedynie %d wpisów w li¶cie AKICK 
	kana³u.
CHAN_AKICK_ADDED
	%s dodano do listy AKICK kana³u %s.
CHAN_AKICK_NO_SUCH_ENTRY
	Brak wpisu (#%d) na li¶cie AKICK %s.
CHAN_AKICK_NOT_FOUND
	%s nie znaleziono na li¶cie AKICK %s.
CHAN_AKICK_NO_MATCH
	Nie znaleziono wpisów na li¶cie AKICK %s.
CHAN_AKICK_STUCK
	%s jest od teraz ca³y czas aktywny na %s.
CHAN_AKICK_UNSTUCK
	%s jest od teraz nie zawsze aktywny na %s.
CHAN_AKICK_DELETED
	%s usuniêto z listy AKICK %s.
CHAN_AKICK_DELETED_ONE
	Usuniêto 1 wpis z listy AKICK %s.
CHAN_AKICK_DELETED_SEVERAL
	Usuniêto %d wpisów z listy AKICK %s.
CHAN_AKICK_LIST_EMPTY
	Lista AKICK kana³u %s jest pusta.
CHAN_AKICK_LIST_HEADER
	Lista AKICK dla kana³u %s:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (przez %s na %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (zablokowany) (przez %s na %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	Egzekucja listy AKICK %s zakoñczona. 
	Wyegzekwowano %d u¿ytkowników.
CHAN_AKICK_CLEAR
	Lista AKICK kana³ %s zosta³a wyczyszczona.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS kana³ {SET | DIS[ABLE] | LIST | RESET} [rzecz [poziom]]
CHAN_LEVELS_XOP
	Ta komenda jest nieu¿yteczna w tym trybie.
CHAN_LEVELS_RANGE
	Poziom musi siê zawieraæ pomiêdzy %d a %d.
CHAN_LEVELS_CHANGED
	Wymagany poziom dla %s 
	na kanale %s zmieniono teraz na %d.
CHAN_LEVELS_UNKNOWN
	Ustawienie %s jest nieznane. 
	Napisz /msg %s HELP LEVELS DESC, 
	aby uzyskaæ liste dostêpnych mo¿liwo¶ci.
CHAN_LEVELS_DISABLED
	%s wy³±czono na kanale %s.
CHAN_LEVELS_LIST_HEADER
	Ustawienia poziomów dostêpu na %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (wy³±czony)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (tylko w³a¶ciciel)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	Poziomy dostêpu na %s zosta³y zresetowane do 
	domy¶lnych.

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
	INFO kana³ [ALL]
CHAN_INFO_HEADER
	Informacje o kanale %s:
CHAN_INFO_FOUNDER
	W³a¶ciciel: %s (%s)
CHAN_INFO_NO_FOUNDER
	W³a¶ciciel: %s
CHAN_INFO_SUCCESSOR
	Zastêpca: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	Zastêpca: %s
CHAN_INFO_DESCRIPTION
	Opis: %s
CHAN_INFO_ENTRYMSG
	Wiadomo¶æ powitalna: %s
CHAN_INFO_TIME_REGGED
	Zarejestrowano: %s
CHAN_INFO_LAST_USED
	Ostatnio u¿ywany: %s
CHAN_INFO_LAST_TOPIC
	Ostatni topic: %s
CHAN_INFO_TOPIC_SET_BY
	Topic ustawiony przez: %s
CHAN_INFO_URL
	Adres WWW: %s
CHAN_INFO_EMAIL
	Adres e-mail: %s
CHAN_INFO_BANTYPE
	Typ bana: %d
CHAN_INFO_OPTIONS
	Opcje: %s
CHAN_INFO_OPT_KEEPTOPIC
	Podtrzymywanie topicu
CHAN_INFO_OPT_OPNOTICE
	OP-Komunikaty
CHAN_INFO_OPT_PEACE
	Opcja PEACE
CHAN_INFO_OPT_PRIVATE
	Prywatny
CHAN_INFO_OPT_RESTRICTED
	Dostêp restrykcyjny
CHAN_INFO_OPT_SECURE
	Opcja SECURE
CHAN_INFO_OPT_SECUREOPS
	Opcja SECUREOPS
CHAN_INFO_OPT_SECUREFOUNDER
	Opcja SECUREFOUNDER
CHAN_INFO_OPT_SIGNKICK
	Opcja SIGNKICK
CHAN_INFO_OPT_TOPICLOCK
	Opcja TOPICLOCK
CHAN_INFO_OPT_XOP
	System listy dostêpu xOP
CHAN_INFO_OPT_NONE
	Brak
CHAN_INFO_MODE_LOCK
	Zablokowane tryby: %s
CHAN_INFO_EXPIRE
	      Expires in: %s
CHAN_INFO_NO_EXPIRE
	Ten kana³ nie podlega prawu wygasania.

# LIST responses
CHAN_LIST_SYNTAX
	LIST wzór
CHAN_LIST_SERVADMIN_SYNTAX
	LIST wzór [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
CHAN_LIST_HEADER
	Lista pasuj±cyh wpisów %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Koniec listy - %d/%d wpisów znalezionych.
	
# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE kana³

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN kana³
CHAN_UNBANNED
	Odbanowa³em Cie na %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC kana³ [temat]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR kana³ co
CHAN_CLEARED_BANS
	Wszystkie bany na %s zosta³y wyczyszczone.
CHAN_CLEARED_EXCEPTS
	Wszystkie wyj±tki na %s zosta³y wyczyszczone.
CHAN_CLEARED_MODES
	Wszystkie tryby na %s zosta³y zresetowane.
CHAN_CLEARED_OPS
	Tryby +o zosta³y wyczyszczone na %s.
CHAN_CLEARED_HOPS
	Tryby +h zosta³y wyczyszczone na %s.
CHAN_CLEARED_VOICES
	Tryby +v zosta³y wyczyszczone na %s.
CHAN_CLEARED_USERS
	Wszyscy u¿ytkownicy zostali wykopani z %s.
CHAN_CLEARED_INVITES
	All invites on channel %s have been removed.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS kana³
CHAN_GETPASS_UNAVAILABLE
	Komenda GETPASS jest niedostêpna poniewa¿ has³a s± 
	szyfrowane.
CHAN_GETPASS_PASSWORD_IS
	Has³o kana³u %s to %s.
	
# SENDPASS responses
CHAN_GETKEY_SYNTAX
	GETKEY channel
CHAN_GETKEY_NOKEY
	The channel %s has no key.
CHAN_GETKEY_KEY
	Key for channel %s is %s.

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS kana³
CHAN_SENDPASS_UNAVAILABLE
	Komenda SENDPASS jest niedostêpna poniewa¿ has³a s± 
	szyfrowane.
CHAN_SENDPASS_SUBJECT
	Has³o kana³u (%s)
CHAN_SENDPASS_HEAD
	Witaj,
CHAN_SENDPASS_LINE_1
	Zarz±da³e¶(a¶) wys³ania has³a kana³u %s na e-mail.
CHAN_SENDPASS_LINE_2
	Twoje has³o to: %s. 
	Dla w³asnego bezpieczeñstwa nale¿y to has³o natychmiast 
	zmieniæ na inne.
CHAN_SENDPASS_LINE_3
	Je¶li nie wiesz dlaczego ta wiadomo¶æ dotar³a do Ciebie 
	to zignoruj j±. Je¶li ta sytuacja bêdzie siê powtarzaæ, 
	skontaktuj siê z ircoperatorem sieci.
CHAN_SENDPASS_LINE_4
	Proszê nie odpowiadaæ na t± wiadomo¶æ.
CHAN_SENDPASS_LINE_5
	Z wyrazami szacunku,
	Administratorzy %s.
CHAN_SENDPASS_OK
	Has³o do %s zosta³o wys³ane.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID kana³ [powód]
CHAN_FORBID_SYNTAX_REASON
	FORBID kana³ powód
CHAN_FORBID_SUCCEEDED
	Kana³ %s zosta³ teraz porzucony.
CHAN_FORBID_FAILED
	Nie mogê porzuciæ kana³u %s!
CHAN_FORBID_REASON
	Ten kana³ zosta³ porzucony.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND kana³ [powód]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND kana³ powód
CHAN_SUSPEND_SUCCEEDED
	Kana³ %s zosta³ teraz zawieszony.
CHAN_SUSPEND_FAILED
	Nie moge zawiesiæ kana³u %s!
CHAN_SUSPEND_REASON
	Ten kana³ zosta³ zawieszony.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND kana³
CHAN_UNSUSPEND_ERROR
	Nie znaleziono znaku # na pocz±tku nazwy kana³u.
CHAN_UNSUSPEND_SUCCEEDED
	Kana³ %s jest od teraz ju¿ wolny.
CHAN_UNSUSPEND_FAILED
	Nie moge zwolniæ kana³u %s!

# Misc responses
CHAN_EXCEPTED
	%s posiada wyj±tek na %s i nie mo¿e byæ 
	zbanowany póki nie zostanie usuniêty.

###########################################################################
#
# Komunikaty MemoServ.
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	Masz 1 now± wiadomo¶æ.
MEMO_HAVE_NEW_MEMOS
	Masz %d nowe(ych) wiadomo¶ci.
MEMO_TYPE_READ_LAST
	Napisz /msg %s READ LAST aby przeczytaæ.
MEMO_TYPE_READ_NUM
	Napisz /msg %s READ %d aby przeczytaæ.
MEMO_TYPE_LIST_NEW
	Napisz /msg %s LIST NEW,
	aby zobaczyæ liste nowych wiadomo¶ci.
MEMO_AT_LIMIT
	Uwaga: Osi±gniêto ju¿ limit wiadomo¶ci (%d). 
	Nie bêdziesz móg³(mog³a) odebraæ ¿adnej nowej dopóki 
	nie usuniesz czê¶ci starych.
MEMO_OVER_LIMIT
	Uwaga: Ju¿ masz skoñczony limit maksymalny 
	ilo¶ci wiadomo¶ci (%d). 
	Nie bêdziesz móg³(mog³a) odebraæ ¿adnej nowej dopóki 
	nie usuniesz czê¶ci starych.
MEMO_X_MANY_NOTICE
	Jest %d wiadomo¶ci dla kana³u %s.
MEMO_X_ONE_NOTICE
	Jest %d wiadomo¶æ dla kan³u %s.
MEMO_NEW_X_MEMO_ARRIVED
	Jest nowa wiadomo¶æ dla kana³u %s.
	Napisz /msg %s READ %s %d aby j± przeczytaæ.
MEMO_NEW_MEMO_ARRIVED
	Nasz now± wiadomo¶æ od %s.
	Napisz /msg %s READ %d aby j± przeczytaæ.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Nie masz wiadomo¶ci.
MEMO_X_HAS_NO_MEMOS
	%s nie ma wiadomo¶ci.
MEMO_DOES_NOT_EXIST
	Wiadomo¶æ %d nie istnieje!
MEMO_LIST_NOT_FOUND
	Nie znaleziono pasuj±cych wiadomo¶ci.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {nick | kana³} tre¶æ
MEMO_SEND_DISABLED
	B³±d, wysy³anie wiadomo¶ci jest tymczasowo niedostêpne.
MEMO_SEND_PLEASE_WAIT
	Prosze odczekaæ %d sekund(y) przed ponownym u¿yciem
	komendy SEND.
MEMO_X_GETS_NO_MEMOS
	%s nie mo¿e odbieraæ wiadomo¶ci.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s ma w tej chwili zbyt du¿o wiadomo¶ci i nie mo¿e
	odbieraæ nowych.
MEMO_SENT
	Wiadomo¶æ do %s zosta³a wys³ana.
MEMO_MASS_SENT
	Wiadomo¶æ globalna zosta³a wys³ana do wszystkich 
	zarejestrowanych u¿ytkowników.
	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {nick | kana³}
MEMO_CANCEL_DISABLED
	B³±d, odwo³ywanie wiadomo¶ci jest tymczasowo 
	niedostêpne.
MEMO_CANCEL_NONE
	Nie ma wiadomo¶ci odwo³anej.
MEMO_CANCELLED
	Ostatnia wiadomo¶æ do %s zosta³a odwo³ana.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [kana³] [list | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	Nie masz nowych wiadomo¶ci.
MEMO_X_HAS_NO_NEW_MEMOS
	%s nie ma nowych wiadomo¶ci.
MEMO_LIST_MEMOS
	Wiadomo¶ci dla %s. Aby przeczytaæ, 
	napisz: /msg %s READ numer
MEMO_LIST_NEW_MEMOS
	Nowe wiadomo¶ci dla %s. Aby przeczytaæ, 
	napisz: /msg %s READ numer
MEMO_LIST_CHAN_MEMOS
	Wiadomo¶ci dla %s. Aby przeczytaæ, 
	napisz: /msg %s READ %s numer
MEMO_LIST_CHAN_NEW_MEMOS
	Nowe wiadomo¶ci dla %s. Aby przeczytaæ, 
	napisz: /msg %s READ %s numer
MEMO_LIST_HEADER
	 Nr   Nadawca            Data/Czas
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [kana³] {list | LAST | NEW}
MEMO_HEADER
	Wiadomo¶æ %d od %s (%s). Aby usun±æ, 
	napisz: /msg %s DEL %d
MEMO_CHAN_HEADER
	Wiadomo¶æ %d od %s (%s). Aby usun±æ, 
	napisz: /msg %s DEL %s %d
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [kana³] {numer | list | ALL}
MEMO_DELETED_NONE
	Nie usuniêto ¿adnych wiadomo¶ci.
MEMO_DELETED_ONE
	Wiadomo¶æ %d zosta³a usuniêta.
MEMO_DELETED_SEVERAL
	Wiadomo¶ci %s zosta³y usuniête.
MEMO_DELETED_ALL
	Wszystkie Twoje wiadomo¶ci zosta³y usuniête.
MEMO_CHAN_DELETED_ALL
	Wszystkie wiadomo¶ci dla kana³u %s zosta³y usuniête.

# SET responses
MEMO_SET_SYNTAX
	SET opcja parametry
MEMO_SET_DISABLED
	B³±d, ustawianie opcji wiadomo¶ci jest tymczasowo 
	niedostêpne.
MEMO_SET_UNKNOWN_OPTION
	Nieznana opcja komendy SET %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | OFF }
MEMO_SET_NOTIFY_ON
	%s powiadomi Ciê o wiadomo¶ciach, gdy siê zalogujesz
	i gdy zostan± one wys³ane.
MEMO_SET_NOTIFY_LOGON
	%s powiadomi Cie o wiadomo¶ciach, gdy siê zalogujesz 
	lub bêdziesz away.
MEMO_SET_NOTIFY_NEW
	%s bêdzie powiadamia³ Ciê o nowych wiadomo¶ciach, 
	gdy zostan± wys³ane.
MEMO_SET_NOTIFY_OFF
	%s nie bedzie Ciê powiadamia³ o wiadomo¶ciach.
MEMO_SET_NOTIFY_MAIL
	Powiadomienie o nowych wiadomo¶ciach bêd± Ci 
	przesy³ane na adres e-mail.
MEMO_SET_NOTIFY_NOMAIL
	Nie bêdziesz d³u¿ej informowany(a) o nowych 
	wiadomo¶ciach na adres e-mail.
MEMO_SET_NOTIFY_INVALIDMAIL
	Nie ma adresu e-mail ustawionego dla Twojego nicka.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [kana³] limit
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [user | kana³] {limit | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Nie masz uprawnieñ, aby zmieniaæ swój limit 
	wiadomo¶ci.
MEMO_SET_LIMIT_FORBIDDEN
	Limit wiadomo¶ci dla %s nie mo¿e byæ zmieniany.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Nie mo¿esz ustawiæ swojego limitu wiadomo¶ci 
	wiêkszego ni¿ %d.
MEMO_SET_LIMIT_TOO_HIGH
	Nie mo¿esz ustawiæ limitu wiadomo¶ci dla %s 
	wiêkszego ni¿ %d.
MEMO_SET_LIMIT_OVERFLOW
	Limit wiadomo¶ci jest za du¿y. Limituj do %d.
MEMO_SET_YOUR_LIMIT
	Twój limit wiadomo¶ci zosta³ ustawiony na %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Nie mo¿esz ju¿ d³u¿ej odbieraæ wiadomo¶ci.
MEMO_UNSET_YOUR_LIMIT
	Twój limit wiadomo¶ci zosta³ wy³±czony.
MEMO_SET_LIMIT
	Limit wiadomo¶ci dla %s ustawiono na %d.
MEMO_SET_LIMIT_ZERO
	Limit wiadomo¶ci dla for %s ustawiono na 0.
MEMO_UNSET_LIMIT
	Limit wiadomo¶ci dla %s jest wy³±czony.

# INFO responses
MEMO_INFO_SYNTAX
	INFO [kana³]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [nick | kana³]
MEMO_INFO_NO_MEMOS
	Aktualnie nie masz wiadomo¶ci.
MEMO_INFO_MEMO
	Aktualnie masz 1 wiadomo¶æ.
MEMO_INFO_MEMO_UNREAD
	Aktualnie masz 1 wiadomo¶æ i nie jest ona 
	jeszcze przeczytana.
MEMO_INFO_MEMOS
	Aktualnie masz %d wiadomo¶ci.
MEMO_INFO_MEMOS_ONE_UNREAD
	Aktualnie masz %d wiadomo¶ci, 
	z czego 1 jest nieprzeczytana.
MEMO_INFO_MEMOS_SOME_UNREAD
	Aktualnie masz %d wiadomo¶ci, 
	z czego %d nieprzeczytanych.
MEMO_INFO_MEMOS_ALL_UNREAD
	Aktualnie masz %d wiadomo¶ci, 
	z czego wszystkie s± nieprzeczytane.
MEMO_INFO_LIMIT
	Twój limit wiadomo¶ci wynosi %d.
MEMO_INFO_HARD_LIMIT
	Twój limit wiadomo¶ci wynosi %d 
	i nie mo¿e byæ zmieniony.
MEMO_INFO_LIMIT_ZERO
	Twój limit wiadomo¶ci wynosi 0 
	i nie mo¿esz odbieraæ ¿adnych nowych.
MEMO_INFO_HARD_LIMIT_ZERO
	Twój limit wiadomo¶ci wynosi 0 i nie mo¿esz 
	odbieraæ ¿adnych nowych, ale mo¿esz zmieniaæ limit.
MEMO_INFO_NO_LIMIT
	Nie masz limitu wiadomo¶ci, które mo¿esz trzymaæ.
MEMO_INFO_NOTIFY_OFF
	Nie bêdziesz informowany(a) o nowych wiadomo¶ciach.
MEMO_INFO_NOTIFY_ON
	Bêdziesz informowany(a) o nowych wiadomo¶ciach po 
	zalogowaniu i gdy zostan± do Ciebie wys³ane.
MEMO_INFO_NOTIFY_RECEIVE
	Bêdziesz informowany(a) o nowych wiadomo¶ciach kiedy 
	zostan± do Ciebie wys³ane.
MEMO_INFO_NOTIFY_SIGNON
	Bêdziesz informowany(a) o nowych wiadomo¶ciach po 
	zalogowaniu.
MEMO_INFO_X_NO_MEMOS
	%s aktualnie nie ma wiadomo¶ci.
MEMO_INFO_X_MEMO
	%s aktualnie ma 1 wiadomo¶æ.
MEMO_INFO_X_MEMO_UNREAD
	%s aktualnie ma 1 wiadomo¶æ, która jest jeszcze 
	nieprzeczytana.
MEMO_INFO_X_MEMOS
	%s aktualnie ma %d wiadomo¶ci.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s aktualnie ma %d wiadomo¶ci, 
	z czego 1 nieprzeczytana.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s aktualnie ma %d wiadomo¶ci, 
	z czego %d nieprzeczytanych.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s aktualnie ma %d wiadomo¶ci, 
	z czego wszystkie s± nieprzeczytane.
MEMO_INFO_X_LIMIT
	%s ma limit wiadomo¶ci na %d.
MEMO_INFO_X_HARD_LIMIT
	%s ma limit wiadomo¶ci na %d i nie mo¿e byæ 
	zmieniony.
MEMO_INFO_X_NO_LIMIT
	%s nie ma limitu wiadomo¶ci.
MEMO_INFO_X_NOTIFY_OFF
	%s nie jest informowany(a) o nowych 
	wiadomo¶ciach.
MEMO_INFO_X_NOTIFY_ON
	%s jest informowany(a) o nowych wiadomo¶ciach 
	po zalogowaniu i po wys³aniu.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s jest informowany(a) o nowych wiadomo¶ciach 
	po wys³aniu.
MEMO_INFO_X_NOTIFY_SIGNON
	%s jest informowany(a) o nowych wiadomo¶ciach 
	po zalogowaniu.

# Memo2Mail responses
MEMO_MAIL_SUBJECT
	Nowa wiadomo¶æ
MEMO_MAIL_TEXT1
	Witaj %s
MEMO_MAIL_TEXT2
	W³a¶nie otrzyma³e¶(a¶ now± wiadomo¶æ od %s. 
	Jest to Twoja %d wiadomo¶æ.
MEMO_MAIL_TEXT3
	Tre¶æ wiadomo¶ci:

# RSEND responses
MEMO_RSEND_PLEASE_WAIT
	Proszê odczekaæ %d sekund przed ponownym u¿yciem 
	komendy.
MEMO_RSEND_DISABLED
	B³±d, komenda RSEND jest niedostêpna na tej sieci.
MEMO_RSEND_SYNTAX
	RSEND {nick | kana³} tre¶æ
MEMO_RSEND_NICK_MEMO_TEXT
	[auto-memo] Wys³ana przez Ciebie wiadomo¶æ 
	zosta³a zauwa¿ona.
MEMO_RSEND_CHAN_MEMO_TEXT
	[auto-memo] Wys³ana przez Ciebie wiadomo¶æ 
	do %s zosta³a zauwa¿ona.
MEMO_RSEND_USER_NOTIFICATION
	Informacja o tym, ¿e przeczyta³e¶(a¶) t± wiadomo¶æ 
	zosta³a wys³ana do %s.

# CHECK responses
MEMO_CHECK_SYNTAX
	CHECK nick
MEMO_CHECK_NOT_READ
	Ostatnia wys³ana przez Ciebie wiadomo¶æ 
	do %s (w %s) wci±¿ jest nieprzeczytana.
MEMO_CHECK_READ
	Ostatnia wys³ana przez Ciebie wiadomo¶æ 
	do %s (w %s) zosta³a przeczytana.
MEMO_CHECK_NO_MEMO
	%s nie posiada wiadomo¶ci od Ciebie.

###########################################################################
#
# Komunikaty BotServ 
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
	Bot %s nie istnieje.
BOT_NOT_ASSIGNED
	Musisz przydzieliæ najpierw bota na kana³.
	Napisz /msg %S HELP ASSIGN, 
	aby uzyskaæ wiêcej informacji.
BOT_NOT_ON_CHANNEL
	Nie ma bota na kanale %s.
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	Uprasza siê o nie u¿ywanie s³owa %s na tym kanale!
BOT_REASON_BADWORD_GENTLE
	Patrz co piszesz, tr±bo!
BOT_REASON_BOLD
	Nie pogrubiaj mi tekstu na tym kanale!
BOT_REASON_CAPS
	Wy³±cz tego CAPS LOCKa! 
BOT_REASON_COLOR
	Nie u¿ywaj kolorów na tym kanale! Kolory masz na 
	têczy.
BOT_REASON_FLOOD
	Przestañ floodziæ! Kumasz? Nie? To nie!
BOT_REASON_REPEAT
	Przestañ siê powtarzaæ, bo w koñcu siê sklonujesz!
BOT_REASON_REVERSE
	Nie u¿ywaj mi tu te³, bo Cie walne!
BOT_REASON_UNDERLINE
	Przestañ u¿ywaæ podkre¶leñ na tym kanale!
	
# !seen replies
BOT_SEEN_BOT
	Znalaz³e¶(a¶) mnie, %s!
BOT_SEEN_YOU
	%s? Szukasz siebie? Szukajcie, a znajdziecie! 
BOT_SEEN_ON_CHANNEL
	%s jest teraz na kanale!
BOT_SEEN_ON_CHANNEL_AS
	%s jest teraz na kanale (jako %s)! 
BOT_SEEN_ON
	Widzia³em %s tutaj %s temu.
BOT_SEEN_NEVER
	Nigdy nie widzia³em %s na tym kanale.
BOT_SEEN_UNKNOWN
	Nie wiem kim %s jest.

# BOT responses
BOT_BOT_SYNTAX
	BOT ADD nick user host real
	BOT CHANGE starynick nowynick [user [host [real]]]
	BOT DEL nick
BOT_BOT_ALREADY_EXISTS
	Bot %s ju¿ istnieje.
BOT_BOT_CREATION_FAILED
	B³±d, utworzenie nieudane.
BOT_BOT_READONLY
	B³±d, modyfikowanie botów jest tymczasowo 
	niedostêpne.
BOT_BOT_ADDED
	%s!%s@%s (%s) dodano do listy bocików.
BOT_BOT_ANY_CHANGES
	Nowe informacje s± takie same jak stare.
BOT_BOT_CHANGED
	Bot %s zosta³ zmieniony na %s!%s@%s (%s)
BOT_BOT_DELETED
	Bot %s zosta³ usuniêty.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Lista botów:
BOT_BOTLIST_PRIVATE_HEADER
	Boty zarezerwowane dla IRC Operatorów:
BOT_BOTLIST_FOOTER
	%d boty(ów) dostêpnych.
BOT_BOTLIST_EMPTY
	Obecnie nie ma ¿adnych botów dostêpnych.
	Popro¶ IRC Operatora o dodanie.
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN kana³ nick
BOT_ASSIGN_READONLY
	B³±d, przydzielanie botów jest tymczasowo 
	niedostêpne.
BOT_ASSIGN_ALREADY
	Bot %s jest ju¿ postawiony na %s.
BOT_ASSIGN_ASSIGNED
	Bot %s zosta³ postawiony na %s.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN kana³
BOT_UNASSIGN_UNASSIGNED
	Nie ma ¿adnych botów przydzielonych na %s.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {kana³ | nick}
BOT_INFO_NOT_FOUND
	%s to nieprawid³owy bot lub kana³.
BOT_INFO_BOT_HEADER
	Informacje o bocie %s:
BOT_INFO_BOT_MASK
		Maska: %s@%s
BOT_INFO_BOT_REALNAME
		Nazwa: %s
BOT_INFO_BOT_CREATED
	    Utworzony: %s
BOT_INFO_BOT_USAGE
	    Obecny na: %d kanale(kana³ach)
BOT_INFO_BOT_OPTIONS
		Opcje: %s
BOT_INFO_OPT_PRIVATE
	Prywatny.
BOT_INFO_CHAN_HEADER
	Innformacje o kanale %s:
BOT_INFO_CHAN_BOT
	       Przydzielony bot: %s
BOT_INFO_CHAN_BOT_NONE
	       Przydzielony bot: nie przydzielono jeszcze.
BOT_INFO_CHAN_KICK_BADWORDS
	       Kopanie za s³owa: %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	       Kopanie za s³owa: %s (%d kop(y) do bana)
BOT_INFO_CHAN_KICK_BOLDS
	 Kopanie za pogrubienia: %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	 Kopanie za pogrubienia: %s (%d kop(y) do bana)
BOT_INFO_CHAN_KICK_CAPS_ON
	       Kopanie za capsa: %s (minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	       Kopanie za capsa: %s (%d kop(y) do bana; minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	       Kopanie za capsa: %s
BOT_INFO_CHAN_KICK_COLORS
	      Kopanie za kolory: %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	      Kopanie za kolory: %s (%d kop(y) do bana)
BOT_INFO_CHAN_KICK_FLOOD_ON
	       Kopanie za flood: %s (%d linii w %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	       Kopanie za flood: %s (%d kop(y) do bana; %d linii w %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	       Kopanie za flood: %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	 Kopanie za powtarzanie: %s (%d czasu)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	 Kopanie za powtarzanie: %s (%d kop(y) do bana; %d czasu)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	 Kopanie za powtarzanie: %s
BOT_INFO_CHAN_KICK_REVERSES
	 Kopanie za odw kolorów: %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	 Kopanie za odw kolorów: %s (%d kop(y) do bana)
BOT_INFO_CHAN_KICK_UNDERLINES
	Kopanie za podkre¶lenia: %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	Kopanie za podkre¶lenia: %s (%d kop(y) do bana)
BOT_INFO_ACTIVE
	w³±czone
BOT_INFO_INACTIVE
	wy³±czone
BOT_INFO_CHAN_OPTIONS
	             Inne Opcje: %s
BOT_INFO_OPT_DONTKICKOPS
	Ochrona opów
BOT_INFO_OPT_DONTKICKVOICES
	Ochrona voiców
BOT_INFO_OPT_FANTASY
	Komendy publiczne (fantasy)
BOT_INFO_OPT_GREET
	Powitanie
BOT_INFO_OPT_NOBOT
	Opcja NoBot
BOT_INFO_OPT_SYMBIOSIS
	Opcja SYMBIOSIS
BOT_INFO_OPT_NONE
	Brak
	
# SET responses
BOT_SET_SYNTAX
	SET kana³ opcja ustawienie
BOT_SET_DISABLED
	B³±d, ustawianie opcji bota jest tymczasowo 
	niedostêpne.
BOT_SET_UNKNOWN
	Nieznana opcja %s.
	Napisz /msg %S HELP SET,
	aby uzyskaæ wiêcej informacji.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET kana³ DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Bot nie bêdzie od teraz kopa³ operatorów 
	na kanale %s.
BOT_SET_DONTKICKOPS_OFF
	Bot bêdzie od teraz kopa³ operatorów 
	na kanale %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET kana³ DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Bot nie bêdzie kopa³ voiców na kanale %s.
BOT_SET_DONTKICKVOICES_OFF
	Bot bêdzie kopa³ voiców na kanale %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET kana³ FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Komendy fantasy s± od teraz aktywne 
	na kanale %s.
BOT_SET_FANTASY_OFF
	Komendy fantasy s± od teraz nieaktywne 
	na kanale %s.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET kana³ GREET {ON|OFF}
BOT_SET_GREET_ON
	Opcja powitania jest od teraz aktywna 
	na kanale %s.
BOT_SET_GREET_OFF
	Opcja powitania jest od teraz nieaktywna 
	na kanale %s.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET nickbota NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Opcja NoBot jest od teraz aktywna 
	na kanale %s.
BOT_SET_NOBOT_OFF
	Opcja NoBot jest od teraz nieaktywna 
	na kanale %s.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET nickbota PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Opcja prywatno¶ci bota %s jest 
	od teraz aktywna.
BOT_SET_PRIVATE_OFF
	Opcja prywatno¶ci bota %s jest 
	od teraz nieaktywna.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET kana³ SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Opcja symbiozy jest od teraz aktywna 
	na kanale %s.
BOT_SET_SYMBIOSIS_OFF
	Opcja symbiozy jest od teraz nieaktywna 
	na kanale %s.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK kana³ opcja {ON|OFF} [settings]
BOT_KICK_DISABLED
	B³±d, konfiguracja kopów jest tymczasowo 
	niedostêpna.
BOT_KICK_UNKNOWN
	Nieznana opcja %s.
	Napisz /msg %S HELP KICK, 
	aby uzyskaæ wiêcej informacji.
BOT_KICK_BAD_TTB
	%s nie mo¿e byæ wziêty jako czas bana.

BOT_KICK_BADWORDS_ON
	Bot bêdzie od teraz kopa³ za zakazane s³owa. 
	U¿yj komendy BADWORDS, aby dodaæ b±d¼ usun±æ 
	poszczególne s³owa.
BOT_KICK_BADWORDS_ON_BAN
	Bot bêdzie od teraz kopa³ za zakazane s³owa, 
	oraz stawia³ bana po %d kopach dla tego samego 
	u¿ytkownika. U¿yj komendy BADWORDS, aby dodaæ b±d¼ 
	usunaæ poszczególne s³owa.
BOT_KICK_BADWORDS_OFF
	Bot nie bêdzie wiêcej kopa³ za zakazane s³owa.
BOT_KICK_BOLDS_ON
	Bot bêdzie od teraz kopa³ za pogrubienia.
BOT_KICK_BOLDS_ON_BAN
	Bot bêdzie od teraz kopa³ za pogrubienia, oraz 
	stawia³ bana po %d kopach dla tego samego 
	u¿ytkownika.
BOT_KICK_BOLDS_OFF
	Bot nie bêdzie wiêcej kopa³ za pogrubienia.
BOT_KICK_CAPS_ON
	Bot bêdzie teraz kopa³ za CAPSa (du¿e litery 
	musz± stanowiæ %d znaków %d%% ca³ej wiadomo¶ci).
BOT_KICK_CAPS_ON_BAN
	Bot bêdzie teraz kopa³ za CAPSa (du¿e litery 
	musz± stanowiæ %d znaków i %d%% ca³ej wiadomo¶ci), 
	oraz stawia³ bana po %d kopach dla tego samego 
	u¿ytkownika.
BOT_KICK_CAPS_OFF
	Bot nie bêdzie wiêcej kopa³ za CAPSa.
BOT_KICK_COLORS_ON
	Bot bêdzie od teraz kopa³ za kolory.
BOT_KICK_COLORS_ON_BAN
	Bot bêdzie od teraz kopa³ za kolory, oraz 
	stawia³ bana po %d kopach dla tego samego 
	u¿ytkownika.
BOT_KICK_COLORS_OFF
	Bot nie bêdzie wiêcej kopa³ za kolory.
BOT_KICK_FLOOD_ON
	Bot bêdzie od teraz kopa³ za flood 
	(%d linii w %d sekund).
BOT_KICK_FLOOD_ON_BAN
	Bot bêdzie od teraz kopa³ za flood 
	(%d linii w %d sekund), oraz stawia³ bana po %d 
	kopach dla tego samego u¿ytkownika.
BOT_KICK_FLOOD_OFF
	Bot nie bêdzie wiêcej kopa³ za flood.
BOT_KICK_REPEAT_ON
	Bot bêdzie od teraz kopa³ za powtórzenia 
	(u¿ytkowników, którzy napisz± %d razy taki sam 
	wyraz).
BOT_KICK_REPEAT_ON_BAN
	Bot bêdzie od teraz kopa³ za powtórzenia 
	(u¿ytkowników, którzy napisz± %d razy taki sam 
	wyraz), oraz stawia³ bana po %d kopach dla tego 
	samego u¿ytkownika.
BOT_KICK_REPEAT_OFF
	Bot nie bêdzie wiêcej kopa³ za powtórzenia.
BOT_KICK_REVERSES_ON
	Bot bêdzie od teraz kopa³ za 
	zamiane koloru t³a z tekstem.
BOT_KICK_REVERSES_ON_BAN
	Bot bêdzie od teraz kopa³ za 
	zamiane koloru t³a z tekstem, oraz stawia³ bana
	po %d kopach dla tego samego u¿ytkownika.
BOT_KICK_REVERSES_OFF
	Bot nie bêdzie wiêcej kopa³ za 
	zamiane koloru t³a z tekstem.
BOT_KICK_UNDERLINES_ON
	Bot bêdzie od teraz kopa³ za podkre¶lenia.
BOT_KICK_UNDERLINES_ON_BAN
	Bot bêdzie od teraz kopa³ za podkre¶lenia, 
	oraz stawia³ bana po %d kopach dla tego samego 
	u¿ytkownika.
BOT_KICK_UNDERLINES_OFF
	Bot nie bêdzie wiêcej kopa³ za podkre¶lenia.
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS kana³ {ADD|DEL|LIST|CLEAR} [s³owo | pozycja] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	B³±d, modyfikacje listy zakazanych s³ów kana³u s± 
	tymczasowo niedostêpne.
BOT_BADWORDS_REACHED_LIMIT
	B³±d, mo¿esz mieæ tylko %d wpisów na li¶cie 
	zakazanych s³ów.
BOT_BADWORDS_ALREADY_EXISTS
	%s ju¿ istnieje w li¶cie zakazanych s³ów 
	kana³u %s.
BOT_BADWORDS_ADDED
	%s dodano do listy zakazanych s³ów kana³u %s.
BOT_BADWORDS_NO_SUCH_ENTRY
	Brak wpisów (#%d) na li¶cie zakazanych s³ów 
	kana³u %s.
BOT_BADWORDS_NOT_FOUND
	%s nie znaleziono na li¶cie zakazanych s³ów 
	kana³u %s.
BOT_BADWORDS_NO_MATCH
	Nie znaleziono wpisów na li¶cie zakazanych s³ów 
	kana³u %s.
BOT_BADWORDS_DELETED
	%s usuniêto z listy zakazanych s³ów 
	kana³u %s.
BOT_BADWORDS_DELETED_ONE
	Usuniêto 1 wpis z listy zakazanych s³ów 
	kana³u %s.
BOT_BADWORDS_DELETED_SEVERAL
	Usuniêto %d wpisów z listy zakazanych s³ów 
	kana³u %s.
BOT_BADWORDS_LIST_EMPTY
	Lista zakazanych s³ów dla kana³u %s jest pusta.
BOT_BADWORDS_LIST_HEADER
	Lista zakazanych s³ów dla %s:
	  Nr    S³owo                          Typ
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	Lista zakazanych s³ów jest od teraz pusta.

# SAY responses
BOT_SAY_SYNTAX
	SAY kana³ tekst

# ACT responses
BOT_ACT_SYNTAX
	ACT kana³ tekst

# Errors
BOT_EXCEPT
	U¿ytkownik posiada wyj±tek na kanale.
BOT_BAD_NICK
	Nick bota mo¿e zawieraæ tylko prawid³owe znaki.
BOT_BAD_HOST
	Host bota mo¿e zawieraæ tylko prawid³owe znaki.
BOT_BAD_IDENT
	Ident bota mo¿e zawieraæ tylko prawid³owe znaki.
BOT_LONG_IDENT
	Bot Idents may only contain 9 characters.
###########################################################################
#
# Komunikaty OperServ 
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Serwisy nie mog± zmieniæ trybu. 
	Czy aby na pewno s± dobrze skonfigurowane?
OPER_BOUNCY_MODES_U_LINE
	Serwisy nie mog± zmieniæ trybu. 
	Czy na pewno U:linie s± skonfigurowane poprawnie?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL tre¶æ wiadomo¶ci

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Nieznana opcja STATS %s.
OPER_STATS_CURRENT_USERS
	Ilo¶æ u¿ytkowników: %d (IRC Opów: %d)
OPER_STATS_MAX_USERS
	Rekord: %d (%s)
# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Serwisy dzia³aj± %d dni, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Serwisy dzia³aj± %d dnia, %02d:%02d
OPER_STATS_UPTIME_HMS
	Serwisy dzia³aj± %d godzin i %d minut(y).
OPER_STATS_UPTIME_HM1S
	Serwisy dzia³aj± %d godzin(y) i %d minut(y).
OPER_STATS_UPTIME_H1MS
	Serwisy dzia³aj± %d godzin(y) i %d minut(y).
OPER_STATS_UPTIME_H1M1S
	Serwisy dzia³aj± %d godzin(y) i %d minut(y).
OPER_STATS_UPTIME_1HMS
	Serwisy dzia³aj± %d godzine i %d minut(y).
OPER_STATS_UPTIME_1HM1S
	Serwisy dzia³aj± %d godzine i %d minut(y).
OPER_STATS_UPTIME_1H1MS
	Serwisy dzia³aj± %d godzine i %d minut(y).
OPER_STATS_UPTIME_1H1M1S
	Serwisy dzia³aj± %d godzine i %d minut(y).
OPER_STATS_UPTIME_MS
	Serwisy dzia³aj± %d minut(y) i %d sekund(y).
OPER_STATS_UPTIME_M1S
	Serwisy dzia³aj± %d minut(y) i %d sekund(y).
OPER_STATS_UPTIME_1MS
	Serwisy dzia³aj± %d minut(y) i %d sekund(y).
OPER_STATS_UPTIME_1M1S
	Serwisy dzia³aj± %d minute i %d sekund(y).
OPER_STATS_BYTES_READ
	Bajtów czytanych: %5d kB
OPER_STATS_BYTES_WRITTEN
	Bajtów zapisanych: %5d kB
OPER_STATS_USER_MEM
	U¿ytkownik    : %6d rekordów, %5d kB
OPER_STATS_CHANNEL_MEM
	Kana³         : %6d rekordów, %5d kB
OPER_STATS_GROUPS_MEM
	Grupy NS      : %6d rekordów, %5d kB
OPER_STATS_ALIASES_MEM
	Aliasy NS     : %6d rekordów, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ      : %6d rekordów, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ       : %6d rekordów, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ      : %6d rekordów, %5d kB
OPER_STATS_SESSIONS_MEM
	Sesji         : %6d rekordów, %5d kB
OPER_STATS_AKILL_COUNT
	Aktualna ilo¶æ AKILLów: %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Domy¶lny czas wygasania AKILL: %d dni
OPER_STATS_AKILL_EXPIRE_DAY
	Domy¶lny czas wygasania AKILL: 1 dzieñ
OPER_STATS_AKILL_EXPIRE_HOURS
	Domy¶lny czas wygasania AKILL: %d godzin(y)
OPER_STATS_AKILL_EXPIRE_HOUR
	Domy¶lny czas wygasania AKILL: 1 godzina
OPER_STATS_AKILL_EXPIRE_MINS
	Domy¶lny czas wygasania AKILL: %d minut(y)
OPER_STATS_AKILL_EXPIRE_MIN
	Domy¶lny czas wygasania AKILL: 1 minuta
OPER_STATS_AKILL_EXPIRE_NONE
	Domy¶lny czas wygasania AKILL: Nie wygasaj±
OPER_STATS_SGLINE_COUNT
	Aktualna liczba SGLINI: %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Domy¶lny czas wygasania SGLINI: %d dni
OPER_STATS_SGLINE_EXPIRE_DAY
	Domy¶lny czas wygasania SGLINI: 1 dzieñ
OPER_STATS_SGLINE_EXPIRE_HOURS
	Domy¶lny czas wygasania SGLINI: %d godzin(y)
OPER_STATS_SGLINE_EXPIRE_HOUR
	Domy¶lny czas wygasania SGLINI: 1 godzina
OPER_STATS_SGLINE_EXPIRE_MINS
	Domy¶lny czas wygasania SGLINI: %d minut(y)
OPER_STATS_SGLINE_EXPIRE_MIN
	Domy¶lny czas wygasania SGLINI: 1 minuta
OPER_STATS_SGLINE_EXPIRE_NONE
	Domy¶lny czas wygasania SGLINI: Nie wygasaj±
OPER_STATS_SQLINE_COUNT
	Aktualna liczba SQLINI: %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Domy¶lny czas wygasania SQLINI: %d dni
OPER_STATS_SQLINE_EXPIRE_DAY
	Domy¶lny czas wygasania SQLINI: 1 dzieñ
OPER_STATS_SQLINE_EXPIRE_HOURS
	Domy¶lny czas wygasania SQLINI: %d godzin(y)
OPER_STATS_SQLINE_EXPIRE_HOUR
	Domy¶lny czas wygasania SQLINI: 1 godzina
OPER_STATS_SQLINE_EXPIRE_MINS
	Domy¶lny czas wygasania SQLINI: %d minut(y)
OPER_STATS_SQLINE_EXPIRE_MIN
	Domy¶lny czas wygasania SQLINI: 1 minute
OPER_STATS_SQLINE_EXPIRE_NONE
	Domy¶lny czas wygasania SQLINI: Nie wygasaj±
OPER_STATS_SZLINE_COUNT
	Aktualna liczba SZLINI: %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Domy¶lny czas wygasania SZLINI: %d dni
OPER_STATS_SZLINE_EXPIRE_DAY
	Domy¶lny czas wygasania SZLINI: 1 dzieñ
OPER_STATS_SZLINE_EXPIRE_HOURS
	Domy¶lny czas wygasania SZLINI: %d godzin(y)
OPER_STATS_SZLINE_EXPIRE_HOUR
	Domy¶lny czas wygasania SZLINI: 1 godzina
OPER_STATS_SZLINE_EXPIRE_MINS
	Domy¶lny czas wygasania SZLINI: %d minut(y)
OPER_STATS_SZLINE_EXPIRE_MIN
	Domy¶lny czas wygasania SZLINI: 1 minuta
OPER_STATS_SZLINE_EXPIRE_NONE
	Domy¶lny czas wygasania SZLINI: Nie wygasaj±
OPER_STATS_RESET
	Statystyki zresetowano.

# MODE responses
OPER_STATS_UPLINK_SERVER
	Uplink server: %s
OPER_STATS_UPLINK_CAPAB
	Uplink capab: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Servers found: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE kana³ mody

# UMODE respondes
OPER_UMODE_SYNTAX
	UMODE nick mody
OPER_UMODE_SUCCESS
	Zmieniono usermody dla %s.
OPER_UMODE_CHANGED
	%s zmieni³(a) Tobie usermody.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE nick flagi
OPER_OLINE_SUCCESS
	Flagi opera %s zosta³y przyznane dla %s.
OPER_OLINE_IRCOP
	Jeste¶ teraz IRC Operatorem.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES kana³ [ALL]
OPER_CLEARMODES_DONE
	Tryby i bany zosta³y wyczyszczone na 
	kanale %s.
OPER_CLEARMODES_ALL_DONE
	Wszystkie tryby zosta³y wyczyszczone na 
	kanale %s.


# KICK responses
OPER_KICK_SYNTAX
	KICK kana³ nick powód

# SVSNICK responses
OPER_SVSNICK_SYNTAX
	SVSNICK nick nowynick 
OPER_SVSNICK_NEWNICK
	U¿ytkownik %s ma teraz nicka %s.
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [nick | pozycja]
OPER_ADMIN_SKELETON
	Serwisy s± w trybie szkieletowym i komenda ADMIN 
	jest niedostêpna.
OPER_ADMIN_EXISTS
	%s ju¿ istnieje na li¶cie administratorów 
	serwisów.
OPER_ADMIN_REACHED_LIMIT
	B³±d, mo¿esz mieæ tylko %d administratorów 
	serwisów.
OPER_ADMIN_ADDED
	%s dodano do listy administratorów serwisów.
OPER_ADMIN_NOT_FOUND
	%s nie znaleziono na li¶cie administratorów 
	serwisów.
OPER_ADMIN_NO_MATCH
	Nie znaleziono wpisów na li¶cie administratorów 
	serwisów.
OPER_ADMIN_DELETED
	%s usuniêto z listy administratorów serwisów.
OPER_ADMIN_DELETED_ONE
	Usuniêto 1 wpis z listy administratorów serwisów.
OPER_ADMIN_DELETED_SEVERAL
	Usuniêto %d wpisy(ów) z listy administratorów 
	serwisów.
OPER_ADMIN_LIST_EMPTY
	Lista administratorów serwisów jest pusta.
OPER_ADMIN_LIST_HEADER
	Lista administratorów serwisów:
	  Nr    Nick
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	Lista administratorów serwisów zosta³a 
	wyczyszczona.
OPER_ADMIN_MOVED
	%s has been moved to Services Administrators list.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | pozycja]
OPER_OPER_SKELETON
	Serwisy s± w trybie szkieletowym i komenda OPER 
	jest niedostêpna.
OPER_OPER_EXISTS
	%s ju¿ istnieje na li¶cie operatorów serwisów.
OPER_OPER_REACHED_LIMIT
	B³±d, mo¿esz mieæ tylko %d operatorów serwisów.
OPER_OPER_ADDED
	%s dodano do listy operatorów serwisów.
OPER_OPER_NOT_FOUND
	%s nie znaleziono na li¶cie operatorów serwisów.
OPER_OPER_NO_MATCH
	Nie znaleziono wpisów na li¶cie operatorów serwisów.
OPER_OPER_DELETED
	%s usuniêto z listy operatorów serwisów.
OPER_OPER_DELETED_ONE
	Usuniêto 1 wpis z listy operatorów serwisów.
OPER_OPER_DELETED_SEVERAL
	Usuniêto %d wpisy(ów) z listy operatorów serwisów.
OPER_OPER_LIST_EMPTY
	Lista operatorów serwisów jest pusta.
OPER_OPER_LIST_HEADER
	Lista operatorów serwisów:
	  Nr    Nick
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	Lista operatorów serwisów zosta³a wyczyszczona.
OPER_OPER_MOVED
	%s has been moved to Services Operators list.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+czaswygasania] {maska | pozycja} [powód]]
OPER_AKILL_EXISTS
	%s ju¿ istnieje na li¶cie AKILL.
OPER_AKILL_ALREADY_COVERED
	%s ju¿ jest postawione przez %s.
OPER_AKILL_REACHED_LIMIT
	B³±d, mo¿esz mieæ tylko %d AKILLów.
OPER_AKILL_NO_NICK
	Uwaga: maska AKILL nie mo¿e zawieraæ nicków. 
	Upewnij siê, ¿e nie doda³e¶(a¶) czê¶ci nicka.
OPER_AKILL_ADDED
	%s dodano do listy AKILL.
OPER_AKILL_CHANGED
	Zmieniono czas wygasania %s.
OPER_AKILL_NOT_FOUND
	%s nie znaleziono na li¶cie AKILL.
OPER_AKILL_NO_MATCH
	Nie znaleziono wpisów na li¶cie AKILL.
OPER_AKILL_DELETED
	%s usuniêto z listy AKILL.
OPER_AKILL_DELETED_ONE
	Usuniêto 1 wpis z listy AKILL.
OPER_AKILL_DELETED_SEVERAL
	Usuniêto %d wpisy(ów) z listy AKILL.
OPER_AKILL_LIST_EMPTY
	Lista AKILL jest pusta.
OPER_AKILL_LIST_HEADER
	Aktualna lista AKILL:
	  Nr    Maska                             Powód
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Aktualna lista AKILL:
# number, mask, set-by, set-time, expires, reason
OPER_AKILL_VIEW_FORMAT
	%3d  %s (przez %s na %s; %s)
	      %s
OPER_AKILL_CLEAR
	Lista AKILL zosta³a wyczyszczona.
OPER_CHANKILL_SYNTAX
	CHANAKILL [+czastrwania] {#kana³} [powód]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+czaswygasania] {maska | pozycja}[:powód]]
OPER_SGLINE_UNSUPPORTED
	B³±d, SGLINIE nie s± dostêpne na tej sieci.
OPER_SGLINE_EXISTS
	%s ju¿ istnieje w li¶cie SGLINII.
OPER_SGLINE_ALREADY_COVERED
	%s ju¿ jest postawione przez %s.
OPER_SGLINE_REACHED_LIMIT
	B³±d, mo¿esz mieæ tylko %d SGLINII.
OPER_SGLINE_ADDED
	%s dodano do listy SGLINII.
OPER_SGLINE_CHANGED
	Zmieniono czas wygasania %s.
OPER_SGLINE_NOT_FOUND
	%s nie znaleziono na li¶cie SGLINII.
OPER_SGLINE_NO_MATCH
	Nie znaleziono wpisy(ów) na li¶cie SGLINII.
OPER_SGLINE_DELETED
	%s usuniêto z listy SGLINII.
OPER_SGLINE_DELETED_ONE
	Usuniêto 1 wpis z listy SGLINII.
OPER_SGLINE_DELETED_SEVERAL
	Usuniêto %d wpisy(ów) z listy SGLINII.
OPER_SGLINE_LIST_EMPTY
	Lista SGLINII jest pusta.
OPER_SGLINE_LIST_HEADER
	Aktualna lista SGLINII:
	  Nr    Maska                              Powód
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Aktualna lista SGLINII:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (przez %s na %s; %s)
	      %s
OPER_SGLINE_CLEAR
	Lista SGLINII zosta³a wyczyszczona.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+czastrwania] {maska | pozycja} [powód]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	SQLINIE kana³owe nie s± obs³ugiwane przez Twoje 
	ircd wiêc nie mo¿esz ich u¿yæ.
OPER_SQLINE_EXISTS
	%s ju¿ istnieje na li¶cie SQLINII.
OPER_SQLINE_ALREADY_COVERED
	%s jest ju¿ postawione przez %s.
OPER_SQLINE_REACHED_LIMIT
	B³±d, mo¿esz mieæ jedynie %d SQLINII.
OPER_SQLINE_ADDED
	%s dodano do listy SQLINII.
OPER_SQLINE_CHANGED
	Zmieniono czas wygasania %s.
OPER_SQLINE_NOT_FOUND
	%s nie znaleziono na li¶cie SQLINII.
OPER_SQLINE_NO_MATCH
	Nie znaleziono wpisy(ów) na li¶cie SQLINII.
OPER_SQLINE_DELETED
	%s usuniêto z listy SQLINII.
OPER_SQLINE_DELETED_ONE
	Usuniêto 1 wpis z listy SQLINII.
OPER_SQLINE_DELETED_SEVERAL
	Usuniêto %d wpisy(ów) z listy SQLINII.
OPER_SQLINE_LIST_EMPTY
	Lista SQLINII jest pusta.
OPER_SQLINE_LIST_HEADER
	Aktualna lista SQLINII:
	  Nr    Maska                             Powód
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Aktualna lista SQLINII:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (przez %s na %s; %s)
	      %s
OPER_SQLINE_CLEAR
	Lista SQLINII zosta³a wyczyszczona.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+czastrwania] {maska | pozycja} [powód]]
OPER_SZLINE_UNSUPPORTED
	B³±d, SZLINIE nie s± dostêpne na tej sieci.
OPER_SZLINE_EXISTS
	%s ju¿ istnieje na li¶cie SZLINII.
OPER_SZLINE_ALREADY_COVERED
	%s jest ju¿ postawione przez %s.
OPER_SZLINE_REACHED_LIMIT
	B³±d, mo¿esz mieæ tylko %d SZLINII.
OPER_SZLINE_ONLY_IPS
	Uwaga: mo¿esz dodaæ jedynie maski IP do listy 
	SZLINII.
OPER_SZLINE_ADDED
	%s dodano do listy SZLINII.
OPER_SZLINE_CHANGED
	Zmieniono czas wygasania %s.
OPER_SZLINE_NOT_FOUND
	%s nie znaleziono na li¶cie SZLINII.
OPER_SZLINE_NO_MATCH
	Nie znaleziono wpisów na li¶cie SZLINII.
OPER_SZLINE_DELETED
	%s usuniêto z listy SZLINII.
OPER_SZLINE_DELETED_ONE
	Usuniêto 1 wpis z listy SZLINII.
OPER_SZLINE_DELETED_SEVERAL
	Usuniêto %d wpisy(ów) z listy SZLINII.
OPER_SZLINE_LIST_EMPTY
	Lista SZLINII jest pusta.
OPER_SZLINE_LIST_HEADER
	Aktualna lista SZLINII:
	  Nr    Maska                             Powód
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Aktualna lista SZLINII:
# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (przez %s na %s; %s)
	      %s
OPER_SZLINE_CLEAR
	Lista SZLINII zosta³a wyczyszczona.

# SET responses
OPER_SET_SYNTAX
	SET opcja ustawienie
OPER_SET_IGNORE_ON
	Kod ignorowania bêdzie u¿ywany.
OPER_SET_IGNORE_OFF
	Kod ignorowania nie bêdzie u¿ywany.
OPER_SET_IGNORE_ERROR
	Ustawieniem dla opcji IGNORE musi byæ 
	ON lub OFF.
OPER_SET_READONLY_ON
	Serwisy s± teraz w trybie tylko do odczytu.
OPER_SET_READONLY_OFF
	Serwisy s± teraz w trybie odczytu i zapisu.
OPER_SET_READONLY_ERROR
	Ustawieniem dla opcji READONLY musi byæ 
	ON lub OFF.
OPER_SET_LOGCHAN_ON
	Serwisy teraz bêd± wypisywaæ komunikaty 
	na kana³ %s.
OPER_SET_LOGCHAN_OFF
	Serwisy nie bêd± ju¿ d³u¿ej wypisywaæ komunikatów 
	na kana³.
OPER_SET_LOGCHAN_ERROR
	Ustawieniem dla opcji LOGCHAN musi byæ 
	ON lub OFF i kana³ logowania musi byæ 
	zdefiniowany.
OPER_SET_DEBUG_ON
	Serwisy s± teraz w trybie debugowania.
OPER_SET_DEBUG_OFF
	Serwisy nie s± od teraz w trybue debugowania.
OPER_SET_DEBUG_LEVEL
	Serwisy s± teraz w trybie debugowania (poziom %d).
OPER_SET_DEBUG_ERROR
	Ustawieniem dla opcji DEBUG musi byæ 
	ON, OFF, lub liczba dodatnia.
OPER_SET_NOEXPIRE_ON
	Serwisy s± teraz w trybie nie wygasania.
OPER_SET_NOEXPIRE_OFF
	Serwisy s± teraz w trybie wygasania.
OPER_SET_NOEXPIRE_ERROR
	Ustawieniem dla opcji NOEXPIRE musi byæ 
	ON lub OFF.
OPER_SET_UNKNOWN_OPTION
	Nieznana opcja %s.
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
	NOOP {SET|REVOKE} serwer
OPER_NOOP_SET
	Wszystkie O:linie %s zosta³y usuniête.
OPER_NOOP_REVOKE
	Wszystkie O:linie %s zosta³y zresetowane.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE serwer [powód]
OPER_JUPE_HOST_ERROR
	Proszê u¿yæ prawid³owej nazwy serwera.
	
# RAW responses
OPER_RAW_SYNTAX
	RAW tekst

# UPDATE responses
OPER_UPDATING
	Zaktualizowano bazy.
	
# RELOAD responses
OPER_RELOAD
	Plik konfiguracyjny serwisów zosta³ prze³adowany.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN nie zdefiniowano, nie mogê restartowaæ. 
	Uruchom ponownie skrypt configure i przekompiluj 
	serwisy w³±czaj±c opcje RESTART.

# IGNORE respondes
OPER_IGNORE_SYNTAX
	Sk³adnia: IGNORE {ADD|DEL|LIST|CLEAR} [czas] [nick]
OPER_IGNORE_VALID_TIME
	Musisz podaæ prawid³ow± liczbê dla czasu.
OPER_IGNORE_TIME_DONE
	Nick %s bêdzie od teraz igorowany na %s.
OPER_IGNORE_PERM_DONE
	Nick %s bêdzie od teraz igorowany ca³y czas.
OPER_IGNORE_DEL_DONE
	Nick %s nie bêdzie ju¿ d³u¿ej ignorowany.
OPER_IGNORE_LIST
	Lista ignore serwisów:
OPER_IGNORE_LIST_NOMATCH
	%s nie znaleziono na li¶cie ignorów.
OPER_IGNORE_LIST_EMPTY
	Lista ignorów jest pusta.
OPER_IGNORE_LIST_CLEARED
	Lista ignorów zosta³a wyczyszczona.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	Nie mogê znale¼æ u¿ytkownika %s.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Lista kana³ów:
	Nazwa                Tryby u¿yt.   Temat
OPER_CHANLIST_HEADER_USER
	Lista kana³ów %s:
	Nazwa                Tryby u¿yt.   Temat
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	Koniec listy kana³ów.
	
# USERLIST responses
OPER_USERLIST_HEADER
	Lista u¿ytkowników:
	Nick                 Maska
OPER_USERLIST_HEADER_CHAN
	Lista u¿ytkowników %s:
	Nick                 Maska
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Koniec listy u¿ytkowników.

# CACHE responses
OPER_SUPER_ADMIN_ON
	Jeste¶ teraz SuperAdminem.
OPER_SUPER_ADMIN_OFF
	Nie jeste¶ d³u¿ej SuperAdminem.
OPER_SUPER_ADMIN_SYNTAX
	Ustawieniem dla opcji SuperAdmin musi byæ 
	ON lub OFF (musi byæ w³±czone w 
	services.conf)
OPER_SUPER_ADMIN_WALL_ON
	%s jest teraz SuperAdminem
OPER_SUPER_ADMIN_WALL_OFF
	%s nie jest ju¿ d³u¿ej SuperAdminem
OPER_SUPER_ADMIN_ONLY
	Tylko SuperAdmini mog± u¿ywaæ tej komendy.
OPER_STAFF_LIST_HEADER
	 Na Poziom Nick
OPER_STAFF_FORMAT
	 %c %s  %s
OPER_STAFF_AFORMAT
	 %c %s  %s [%s]

# DefCon Messages

OPER_DEFCON_SYNTAX
	Sk³adnia: DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	Serwisy s± w trybie Defcon, proszê spróbowaæ 
	ponownie.
OPER_DEFCON_NO_CONF
	System Defcon musi byæ w³±czony w pliku 
	services.conf
OPER_DEFCON_CHANGED
	Serwisy s± teraz w systemie DEFCON %d
OPER_DEFCON_WALL
	%s zmieniono poziom DEFCON na %d
DEFCON_GLOBAL
	Defcon jest teraz na poziomie: %d

# Module strings

OPER_MODULE_LOADED
	Modu³ %s zosta³ za³adowany.
OPER_MODULE_UNLOADED
	Modu³ %s zosta³ wy³adowany.
OPER_MODULE_LOAD_FAIL
	Nie moge za³adowaæ modu³u %s
OPER_MODULE_REMOVE_FAIL
	Nie moge wy³adowaæ modu³u %s
OPER_MODULE_NO_UNLOAD
	This module can not be unloaded.
OPER_MODULE_LOAD_SYNTAX
	MODLOAD NazwaPliku
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD NazwaPliku
OPER_MODULE_LIST_HEADER
	Aktualna lista modu³u:
OPER_MODULE_LIST
	Modu³: %s [%s] [%s]
OPER_MODULE_LIST_FOOTER
	Ilo¶æ modu³ów za³adowanych: %d.
OPER_MODULE_INFO_LIST
	Modu³: %s 
	Wersja: %s 
	Autor: %s 
	za³adowano: %s
OPER_MODULE_CMD_LIST
	Dodane komendy: /msg %s %s
OPER_MODULE_MSG_LIST
	Dostarczone opcja IRCD dla(?): %s
OPER_MODULE_NO_LIST
	Aktualnie nie ma za³adowanych modu³ów.
OPER_MODULE_NO_INFO
	Nie ma informacji na temat modu³u %s.
OPER_MODULE_INFO_SYNTAX
	MODINFO NazwaPliku
MODULE_HELP_HEADER
	Nastêpuj±ce komendy zosta³y dodane przez modu³:

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [params]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+czastrwania] maska limit powód
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {maska | lista}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE nr pozycja
OPER_EXCEPTION_DISABLED
	Limitowanie sesji jest wy³±czone.
OPER_EXCEPTION_ALREADY_PRESENT
	Maska %s ju¿ istnieje w li¶cie wyj±tków.
OPER_EXCEPTION_TOO_MANY
	Lista wyj±tków limitów sesji jest pe³na!
OPER_EXCEPTION_ADDED
	Limit sesji dla %s zmieniono na %d.
OPER_EXCEPTION_MOVED
	Wyj±tek dla %s (#%d) przeniesiono 
	na pozycje %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	No such entry (#%d) session-limit exception list.
OPER_EXCEPTION_NOT_FOUND
	%s nie znaleziono na li¶cie wyj±tków limitów 
	sesji.
OPER_EXCEPTION_NO_MATCH
	Nie znaleziono wpisów na li¶cie wyj±tków limitów 
	sesji.
OPER_EXCEPTION_DELETED
	%s usuniêto z listy wyj±tków limitów sesji.
OPER_EXCEPTION_DELETED_ONE
	Usuniêto 1 wpis z listy wyj±tków limitów sesji.
OPER_EXCEPTION_DELETED_SEVERAL
	Usuniêto %d wpisy(ów) z listy wyj±tków limitów 
	sesji.
OPER_EXCEPTION_LIST_HEADER
	Aktualna lista wyj±tków limitów sesji:
# mask, limit
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Nr   Limit  Host
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (przez %s na %s; %s)
	    Limit: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Nieprawid³owy limit sesji. Musi byæ liczb± dodatni±, 
	wiêksz± lub równ± zero i mniejsz± ni¿ %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Nieprawid³owa maska. Tylko rzeczywiste maski s± 
	prawid³owe jako wyj±tki.
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
	Limitowanie sesji jest wy³±czone.
OPER_SESSION_INVALID_THRESHOLD
	Nieprawid³owa warto¶æ progu. Musi byæ prawid³ow± 
	liczb± ca³kowit± wiêksz± ni¿ 1.
OPER_SESSION_NOT_FOUND
	%s nie znaleziono na li¶cie sesji.
OPER_SESSION_LIST_HEADER
	Hosty z najmniejsz± %d ilo¶ci± sesji:
OPER_SESSION_LIST_COLHEAD
	Sesje   Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	Host %s aktualnie posiada %d sesji 
	z limitem %d.

# EXCEPTION help

OPER_HELP_EXCEPTION
	Sk³adnia: EXCEPTION ADD [+czastrwania] maska limit powód
	          EXCEPTION DEL {maska | lista}
	          EXCEPTION MOVE nr pozycja
	          EXCEPTION LIST [maska | lista]
	          EXCEPTION VIEW [maska | lista]
	
	Allows Services admins to manipulate the list of 
	hosts that have specific session limits - allowing 
	certain machines, such as shell servers, to carry 
	more than the default number of clients at a time. 
	Once a host reaches it's session limit, all clients 
	attempting to connect from that host will be
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
	
	Dostêpne dla Adminów serwisów.

OPER_HELP_SESSION
	Syntax: SESSION LIST threshold
	        SESSION VIEW host
	
	Allows Services admins to view the session list.

	SESSION LIST lists hosts with atleast threshold sessions.
	The threshold must be a number greater than 1. This is to 
	prevent accidental listing of the large number of single 
	session hosts.
	SESSION VIEW displays detailed information about a specific
	host - including the current session count and session limit.
	The host value may not include wildcards.

	See the EXCEPTION help for more information about session
	limiting and how to set session limits specific to certain
	hosts and groups thereof.
	
	Dostêpne dla administratorów serwisów.

OPER_HELP_STAFF
	Sk³adnia: STAFF

	Displays all Services Staff nicks along with level
	and on-line status.

OPER_HELP_DEFCON
	Sk³adnia: DEFCON [1|2|3|4|5]

	The defcon system can be used to implement a pre-defined
	set of restrictions to services useful during an attemtped
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
	[Wiadomo¶æ powitalna - %s] %s
NEWS_OPER_TEXT
	[Wiadomo¶æi powitalna dla i@ - %s] %s
NEWS_RANDOM_TEXT
	[Losowa wiadomo¶æ powitalna - %s] %s

NEWS_LOGON_SYNTAX
	Sk³adnia: LOGONNEWS {ADD|DEL|LIST} [tekst|numer]
NEWS_LOGON_LIST_HEADER
	Logon news items:
NEWS_LOGON_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_LOGON_LIST_NONE
	Nie ma niusów powitalnych.
NEWS_LOGON_ADD_SYNTAX
	Sk³adnia: LOGONNEWS ADD tekst
NEWS_LOGON_ADD_FULL
	Lista niusów jest pe³na!
NEWS_LOGON_ADDED
	Dodano powitalnego niusa (#%d).
NEWS_LOGON_DEL_SYNTAX
	Sk³adnia: LOGONNEWS DEL {numer | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Wiadomo¶æ powitalna #%d nie znaleziona!
NEWS_LOGON_DELETED
	Wiadomo¶æ powitalna #%d zosta³a skasowana.
NEWS_LOGON_DEL_NONE
	Nie ma wiadomo¶ci powitalnych do skasowania!
NEWS_LOGON_DELETED_ALL
	Wszystkie wiadomo¶ci powitalne zosta³ skasowane :(.

NEWS_OPER_SYNTAX
	Sk³adnia: OPERNEWS {ADD|DEL|LIST} [text|num]
NEWS_OPER_LIST_HEADER
	Oper-nius lista:
NEWS_OPER_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_OPER_LIST_NONE
	Nie ma oper-niusów.
NEWS_OPER_ADD_SYNTAX
	Sk³adnia: OPERNEWS ADD text
NEWS_OPER_ADD_FULL
	Lista wiadomo¶ci jest pe³na!
NEWS_OPER_ADDED
	Dodano oper-niusa (#%d).
NEWS_OPER_DEL_SYNTAX
	Syntax: OPERNEWS DEL {num | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Oper-nius #%d nie zosta³ znaleziony!
NEWS_OPER_DELETED
	Oper-nius #%d zosta³ skasowany.
NEWS_OPER_DEL_NONE
	Nie ma oper-niusów do skasowania!
NEWS_OPER_DELETED_ALL
	Wszystkie oper-niusy zosta³y skasowane :(.

NEWS_RANDOM_SYNTAX
	Syntax: RANDOMNEWS {ADD|DEL|LIST} [text|num]
NEWS_RANDOM_LIST_HEADER
	Losowe niusy:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_RANDOM_LIST_NONE
	Nie ma ¿adnych losowych niusów.
NEWS_RANDOM_ADD_SYNTAX
	Syntax: RANDOMNEWS ADD text
NEWS_RANDOM_ADD_FULL
	Lista niusów jest pe³na!
NEWS_RANDOM_ADDED
	Nowy nius losowy zosta³ dodany (#%d).
NEWS_RANDOM_DEL_SYNTAX
	Syntax: RANDOMNEWS DEL {num | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Losowy nius #%d nie zosta³ znaleziony!
NEWS_RANDOM_DELETED
	Nius losowy #%d zosta³ skasowany.
NEWS_RANDOM_DEL_NONE
	Nie ma losowych niusów do skasowania!
NEWS_RANDOM_DELETED_ALL
	Wszystkie losowe niusy zosta³y skasowane :(.

NEWS_HELP_LOGON
	Sk³adnia: LOGONNEWS ADD text
	          LOGONNEWS DEL {num | ALL}
	        LOGONNEWS LIST
	
	Edits or displays the list of logon news messages.  When a
	user connects to the network, these messages will be sent
	to them.  (However, no more than three messages will be
	sent in order to avoid flooding the user.  If there are
	more than three news messages, only the three most recent
	will be sent.)
	
	LOGONNEWS LIST may be used by any IRC operator to list the
	current news messages.  ADD and DEL may only be used by
	Services admins.

NEWS_HELP_OPER
	Syntax: OPERNEWS ADD text
	        OPERNEWS DEL {num | ALL}
	        OPERNEWS LIST
	
	Edits or displays the list of oper news messages.  When a
	user opers up (with the /OPER command), these messages will
	be sent to them.  (However, no more than three messages will
	be sent in order to avoid flooding the user.  If there are
	more than three news messages, only the three most recent
	will be sent.)
	
	OPERNEWS LIST may be used by any IRC operator to list the
	current oper news messages.  ADD and DEL may only be used by
	Services admins.

NEWS_HELP_RANDOM
	Syntax: RANDOMNEWS ADD text
	        RANDOMNEWS DEL {num | ALL}
	        RANDOMNEWS LIST
	
	Edits or displays the list of random news messages.  When a
	user connects to the network, one (and only one) of the
	random news will be randomly chosen and sent to them.
	
	RANDOMNEWS LIST may be used by any IRC operator to list the
	current news messages.  ADD and DEL may only be used by
	Services admins.

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S jest serwisem udzielaj±cym pomocnych
	informacji na temat innych serwisów. 
	Dostêpne tematy pomocy: 

	/msg %s HELP
	     Informacje na temat rejestrowania i kontroli 
	     nicków.

	/msg %s HELP
	     Informacje na temat rejestrowania i kontroli 
	     kana³ów.

	/msg %s HELP
	     Informacje na temat zostawiania wiadomo¶ci dla 
	     u¿ytkowników offline.

HELP_HELP_BOT

	/msg %s HELP
	     Informacje na temat stawiania bota na swoim 
	     kanale.

HELP_HELP_HOST

	/msg %s HELP
	     informacja na temat ustawiania vhostów.

###########################################################################
#
# HELP NickServ 
#
###########################################################################
NICK_HELP_CMD_CONFIRM
	    CONFIRM    Confirm a nickserv auth code
NICK_HELP_CMD_RESEND
	    RESEND     Resend a nickserv auth code

NICK_HELP_CMD_REGISTER
	   REGISTER   Rejestracja nicka.
NICK_HELP_CMD_GROUP
	   GROUP      Stworzenie grupy w³asnych nicków.      
NICK_HELP_CMD_IDENTIFY
	   IDENTIFY   Identyfikacja nicka za pomoc± has³a.
NICK_HELP_CMD_ACCESS
	   ACCESS     Modyfikacje listy dostêpu.
NICK_HELP_CMD_SET
	   SET        Ustawienia ró¿nych opcji nicka.
NICK_HELP_CMD_SASET
	    SASET      Set SET-options on another nickname
NICK_HELP_CMD_DROP
	   DROP       Odrejestruj nicka.
NICK_HELP_CMD_RECOVER
	   RECOVER    Odzyskuje Twojego nicka.
NICK_HELP_CMD_RELEASE
	   RELEASE    Zwalnia nick po opcji RECOVER.
NICK_HELP_CMD_SENDPASS
	   SENDPASS   Zapomniane has³o? Spróbuj tego.
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
	    GETPASS    Odzyskanie has³a dla nicka (tylko przy wy³±czonym szyfrowaniu)
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Wy¶wietla pasuj±cych u¿ytkowników zarejestrowanych z podanym 
	               e-mailem
NICK_HELP_CMD_FORBID
	    FORBID     Zabrania rejestracji podanego nicka.
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Suspend a given nick
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Unsuspend a given nick

NICK_HELP
	%S umo¿liwia Tobie zarejestrowanie w³asnego 
	nicka. Dzieki rejestracji tylko Ty bedziesz go móg³ 
	u¿ywaæ oraz uzyskasz dostêp do dodatkowych opcji. 
	Aby ich u¿yæ wpisz /msg %S komenda.
	Wiêcej informacji na temat konkretnej komendy mo¿na 
	uzyskaæ pisz±c:
	/msg %S HELP komenda.
	
NICK_HELP_FOOTER
	INFO: Ten serwis ma na celu identyfikacjê 
	u¿ytkowników bez wzglêdu na host czy zmienne IP.
	To NIE jest po to, aby zajmowaæ komu¶ nicka, 
	ale obowi±zuje zasada: kto pierwszy ten lepszy.
	Je¶li chcia³e¶ mieæ nicka takiego jak imiê, jest 
	ju¿ zajêty - wymy¶l co¶ nowego. Kradzie¿e nicka czy 
	nadu¿ycia %S bêd± owocowa³y, w najl¿ejszym 
	przypadku, utrat± zarejestrowanego nicka.
	
	
NICK_HELP_EXPIRES
	
	Nick nieu¿ywany przed d³u¿szy czas podlega prawu 
	wygasania. Zostanie on automatycznie odrejestrowany 
	je¶li, nie bêdzie u¿ywany przez %d dni. Wraz 
	z wyga¶niêciem nicka utracisz wszystkie opcje, 
	poziomy dostêpu na kana³ach, a tak¿e kana³, który 
	jest zarejestrowany na ten nick.

NICK_HELP_REGISTER
	Sk³adnia: REGISTER has³o [email]
	
	Rejestruje Twój nick w bazie %S. Po rejstracji 
	mo¿esz u¿yæ komend SET i ACCESS aby 
	skonfigurowaæ niektóre opcje nicka wedle upodobañ.  
	Upewnij siê, ¿e pamiêtasz has³o rejestracyjne, bo 
	bêdzie Ci ono potrzebne w przysz³o¶ci dla zmian 
	ustawieñ i identyfikacji. Wielko¶æ liter w ha¶le 
	ma znaczenie!!!.
	
	Jak wybraæ has³o?
	
	Has³o musi byæ trudne do odgadniêcia. Przyk³adowo,
	u¿ycie Twojego imienia jako has³a jest niewskazane.
	U¿yæ Twojego nicka jako has³a równie¿ nie mo¿esz, 
	%S nie pozwoli na co¶ takiego. 
	Bardzo krótkie has³o jest do odgadniêcia metod± prób 
	i b³êdów. Twoje has³o musi mieæ d³ugo¶æ conajmniej 
	5 znaków. Znak spacji nie mo¿e byæ u¿yty w ha¶le.
	
	Parametr email jest opcjonalny i od razu przy 
	rejestracji ustawia adres e-mail dla Twojego nicka. 
	Mo¿e siê zdarzyæ, ¿e adres e-mail mo¿e byæ wymagany 
	na niektórych sieciach. Twoja prywatno¶æ jest pod 
	ochron± - e-mail nie bêdzie znany osobom trzecim. 
	
	Wraz z rejestracj± zostaje utworzona grupa dla 
	Twojego nicka. Nicki z tej samej grupy maj± tê sam± 
	konfiguracjê, te same prawa na kanale oraz 
	ustawienia memo. Je¶li chcesz uzyskaæ wiêcej 
	informacji na ten temat, napisz: 
	/msg %S HELP GROUP.
	
NICK_HELP_GROUP
	Sk³adnia: GROUP nazwa has³o
	
	Ta komenda dodaje Twój nick do grupy nicków o 
	nazwie nazwa.
	Has³o jest has³em Twojego nicka.
	
	Uczestnictwo w grupie pozwala na udostêpnienie 
	Twojej konfiguracji, wiadomo¶ci, praw kana³owych i 
	nie tylko tego dla wszystkich nicków nale¿±cych do 
	tej samej groupy. Je¶li u¿ywasz kilku nicków to 
	ta opcja powinna Ci siê przydaæ.
	 
	Grupa przechowuje wszystkie w³a¶ciwo¶ci nicka. Wiêc 
	je¶li jeden nick zostanie odrejestrowany, nic siê 
	nie stanie. Grupa przestaje istnieæ dopiero wtedy, 
	kiedy nie bêdzie w niej ¿adnego nicka.
	
	Mo¿esz u¿yæ tej komendy nawet je¶li jeszcze nie masz 
	zarejestrowanego nicka. Je¶li Twój nick jest ju¿ 
	zarejestrowany to przed u¿yciem tej komendy musisz 
	siê zidentyfikowaæ. 
	Napisz /msg %S HELP IDENTIFY aby uzyskaæ 
	wiêcej informacji o identyfikacji. 
	
	U¿ycie tej komendy dla niezarejestrowanego 
	nicka jest polecane, poniewa¿ zostanie on 
	automatycznie zarejestrowany po u¿yciu tej komendy. 
	Mo¿esz jej te¿ u¿yæ przy zarejestrowanym nicku (aby 
	zmieniæ swoj± grupê), ale tylko wtedy, je¶li 
	administratorzy sieci tak± mo¿liwo¶æ udostêpnili.
	
	Mo¿esz nale¿eæ jednocze¶nie tylko do jednej grupy. 
	£±czenie grup jest niemo¿liwe.
	
	Informacja: wszystkie nicki z tej samej grupy 
	maj± takie samo has³o. Pamiêtaj o tym.

NICK_HELP_IDENTIFY
	Sk³adnia: IDENTIFY has³o
	
	Informuje %S, ¿e nick który w³a¶nie u¿ywasz 
	nale¿y do Ciebie. Wiele komend wymaga identyfikacji 
	przed ich u¿yciem. Has³o to jest to samo has³o 
	jakiego u¿yto przy komendzie REGISTER.

NICK_HELP_UPDATE
	Sk³adnia: UPDATE

	Aktualizuje Twój aktualny status, czyli sprawdza 
	czy nie masz, nowych wiadomo¶ci, ustawia wymagane 
	mody na kanale, aktualizuje Twój host oraz Twoje 
	flagi u¿ytkownika.
	
NICK_HELP_LOGOUT
	Sk³adnia: LOGOUT
	
	Komenda ta ma dzia³anie odwrotne do komendy 
	IDENTIFY. Jednym s³owem zapomina identyfikacje 
	Twojego nicka i odbiera Ci prawa zidentyfikowanego 
	u¿ytkownika. Po tej komendzie nie zostanie Ci zadane 
	pytanie czy chcesz siê ponownie zidentyfikowaæ. 

NICK_HELP_DROP
	Sk³adnia: DROP [nick]
	
	Usuwa rejestracjê Twojego nicka z bazy %S. 
	Nick, który jest odrejestrowany staje siê wolnym 
	nickiem i mo¿e byæ potem zarejestrowany na nowo 
	przez kogo¶ innego. 
	UWAGA: po odrejestrowaniu oczywi¶cie stracisz
	wpisy na kana³owych listach dostêpu. 
	
	Przed u¿yciem tej komendy nale¿y siê wpierw
	zidentyfikowaæ za pomoc± has³a nicka 
	(/msg %S HELP IDENTIFY aby uzyskaæ
	wiêcej informacji).

NICK_HELP_ACCESS
	Sk³adnia: ACCESS ADD maska
		  ACCESS DEL maska
	          ACCESS LIST
	
	Modyfikuje lub wy¶wietla listê dostêpu dla Twojego 
	nicka. To jest lista hostów, które zostan± 
	automatycznie rozpoznane przez %S jako hosty, z 
	których Twój nick siê mo¿e ³±czyæ. Je¶li chcesz 
	u¿ywaæ nicka z innego hosta musisz wpierw u¿yæ 
	komendy IDENTIFY aby %S móg³ Ciebie rozpoznaæ.
	
	Przyk³ady:
	
	    ACCESS ADD userek@ircnet.pl
	        Zezwala na dostêp do nicka ka¿demu 
		³±cz±cemu siê z domeny ircnet.pl.
	
	    ACCESS DEL userek@ircnet.pl
	        Odwraca efekt poprzedniej komendy.
	
	    ACCESS LIST
	        Wy¶wietla aktualn± liste dostêpu.

NICK_HELP_SET
	Sk³adnia: SET opcja parametry
	
	Ustawienia zmiennych opcji nicka. 
	Mo¿liwe opcje to:
	
	    DISPLAY    Ustawienia wy¶wietlania Twojej grupy 
	               nicków.
	    PASSWORD   Ustawienia has³a Twojego nicka.
	    LANGUAGE   Ustawienia jêzyka w jakim serwisy 
	               bêd± do Ciebie wysy³aæ odpowiedzi.
	    URL        Przypisz adres WWW do Twojego nicka.
	    EMAIL      Przypisz adres e-mail do Twojego 
	               nicka.
	    ICQ        Przypisz numer ICQ do Twojego nicka.
	    GREET      Przypisz wiadomo¶æ powitaln± do 
	               Twojego nicka.
	    KILL       W³±cz/Wy³±cz ochrone nicka.
	    SECURE     W³±cz/Wy³±cz bezpieczeñstwo nicka.
	    PRIVATE    Ukryj dane nicka przy komendzie LIST
	             /msg %S LIST
	    HIDE       Ukryj pewne informacje o nicku.
	    MSG        Zmieñ sposób komunikacji z serwisami.
	    AUTOOP     Should services op you automatically.    
	
	Przed u¿yciem tej komendy trzeba siê wpierw 
	zidentyfikowaæ za pomoc± has³a nicka 
	(/msg %S HELP IDENTIFY aby uzyskaæ
	wiêcej informacji).
	
	Napisz /msg %S HELP SET opcja aby 
	uzyskaæ pomoc na temat wybranej opcji.

NICK_HELP_SET_DISPLAY
	Sk³adnia: SET DISPLAY nowa-nazwa
	
	Zmienia nazwe reprezentuj±c± Twoj± grupe nicków w 
	serwisach. Now± nazw± MUSI byæ nick z Twojej grupy 
	nicków.

NICK_HELP_SET_PASSWORD
	Sk³adnia: SET PASSWORD nowe-has³o
	
	Zmienia Twoje has³o do nicka.

NICK_HELP_SET_LANGUAGE
	Sk³adnia: SET LANGUAGE numer
	
	Zmienia jêzyk jakiego u¿ywaj± serwisy odpowiadaj±c 
	Tobie (czyli przyk³adowo kiedy odpowiadaj± na 
	wys³an± przez Ciebie komendê). Numer mo¿e byæ 
	wybrany z nastêpuj±cej listy obs³ugiwanych jêzyków:
	

NICK_HELP_SET_URL
	Sk³adnia: SET URL adres
	
	Przypisuje adres WWW Twojemu nickowi, który bêdzie 
	wy¶wietlany przy opisie Twojego nicka, dostêpnym
	przy u¿yciu komendy INFO.

NICK_HELP_SET_EMAIL
	Sk³adnia: SET EMAIL adres
	
	Przypisuje adres e-mail Twojemu nickowi, który 
	bêdzie wy¶wietlany przy opisie Twojego nicka, 
	dostêpnym przy u¿yciu komendy INFO.
			
NICK_HELP_SET_ICQ
	Sk³adnia: SET ICQ numer
	
	Przypisuje numer ICQ Twojemu nickowi, który to 
	bêdzie wy¶wietlany przy opisie Twojego nicka, 
	dostêpnym przy u¿yciu komendy INFO.			

NICK_HELP_SET_GREET
	Sk³adnia: SET GREET wiadomo¶æ
	
	Tworzy wiadomo¶æ powitaln± dla Twojego nicka, która 
	bêdzie wy¶wietlana podczas wchodzenia na kana³, w 
	którym opcja GREET jest w³±czona, ale tylko pod 
	warunkiem posiadania przez Ciebie wymaganego poziomu 
	dostêpu na tym kanale.

NICK_HELP_SET_KILL
	Sk³adnia: SET KILL {ON | QUICK | IMMED | OFF}
	
	W³±cza/Wy³±cza automatyczn± ochronê dla Twojego 
	nicka. Kiedy ochrona jest aktywna i inny u¿ytkownik 
	spróbuje u¿yæ Twojego nicka, wtedy bêdzie mia³ 
	minutê na jego zwolnienie pod gro¼b± auto-zmiany 
	przez %S jego nicka na inny lub usuniêcia
	tego¿ u¿ytkownika z sieci.
	
	Je¶li wybierzesz opcjê QUICK, to u¿ytkownik 
	bêdzie mia³ tylko 20 sekund na zmianê nicka zamiast 
	standardowych 60. Je¶li natomiast wybierzesz opcje 
	IMMED, to u¿ytkownik natychmiastowo bez 
	jakiegokolwiek ostrze¿enia poniesie konsekwencje 
	opisane akapit wy¿ej. Proszê nie u¿ywaæ tej opcji 
	bez wyra¼nej konieczno¶ci. Administratorzy sieci 
	mog± t± opcje wy³±czyæ.

NICK_HELP_SET_SECURE
	Sk³adnia: SET SECURE {ON | OFF}
	
	W³±cza/Wy³±cza %S opcje bezpieczeñstwa dla Twojego 
	nicka. Gdy opcja SECURE jest w³±czona, musisz 
	podawaæ swoje has³o (identyfikowaæ siê) zanim 
	zostaniesz rozpoznany(a) jako w³a¶ciciel(ka) nicka, 
	bez wzglêdu na to czy Twój host jest na li¶cie 
	dostêpu czy nie. Jednak¿e je¶li ju¿ bêdziesz na 
	li¶cie dostêpu, %S nie skilluje Ciê bez 
	wzglêdu na  ustawienie opcji KILL.

NICK_HELP_SET_PRIVATE
	Sk³adnia: SET PRIVATE {ON | OFF}
	
	W³±cza/Wy³±cza %S opcjê prywatno¶ci dla Twojego nicka.
	Gdy opcja PRIVATE jest w³±czona Twój nick nie 
	bêdzie widnia³ na li¶cie pokazywanej po u¿yciu 
	komendy LIST w %S. (Jednak¿e, 
	ktokolwiek bêdzie zna³ Twój nick wci±¿
	bêdzie móg³ uzyskaæ informacje przy pomocy komendy 
	INFO)

NICK_HELP_SET_HIDE
	Sk³adnia: SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	Pozwala Tobie ukryæ pewne informacje, które s± 
	wy¶wietlane po u¿yciu komendy INFO w %S na Twój 
	nick. Mo¿esz ukryæ adres e-mail (EMAIL), 
	ostatnio widzian± maske ident@host (USERMASK), 
	Twoj± rangê w serwisach (STATUS), a tak¿e 
	ostatni± wiadomo¶æ przy wychodzeniu (QUIT). 
	Drugi parametr okre¶la czy informacje maj± byæ 
	wy¶wietlane (OFF) czy ukrywane (ON).
	
NICK_HELP_SET_MSG
	Sk³adnia: SET MSG {ON | OFF}
	
	Pozwala Ci wybraæ sposób w jaki serwisy bêd± Ci 
	odpowiadaæ. Gdy opcja MSG jest w³±czona, 
	serwisy bêd± u¿ywaæ wiadomo¶ci na privie(query), 
	gdy jest wy³±czona, wtedy bêd± u¿ywaæ komunikatów
	notice.

NICK_HELP_SASET
	Syntax: SASET nickname option parameters.
	
	Sets various nickname options.  option can be one of:
	
	    DISPLAY    Set the display of the group in Services
	    PASSWORD   Set the nickname password
	    LANGUAGE   Set the language Services will use when
	                   sending messages
	    URL        Associate a URL with the nickname
	    EMAIL      Associate an E-mail address with the nickname
	    ICQ        Associate an ICQ number with the nickname
	    GREET      Associate a greet message with the nickname
	    KILL       Turn protection on or off
	    SECURE     Turn nickname security on or off
	    PRIVATE    Prevent the nickname from appearing in a
	                   /msg %S LIST
	    HIDE       Hide certain pieces of nickname information
	    MSG        Change the communication method of Services
	    NOEXPIRE   Prevent the nickname from expiring
	
	Type /msg %S HELP SASET option for more information
	on a specific option. The options will be set on the given
	nickname. 
	
	Limited to Services admins.

NICK_HELP_SASET_DISPLAY
	Syntax: SASET nickname DISPLAY new-display
	
	Changes the display used to refer to the nickname group in 
	Services. The new display MUST be a nick of the group.
	
	Limited to Services admins.

NICK_HELP_SASET_PASSWORD
	Syntax: SET nickname PASSWORD new-password
	
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

NICK_HELP_RECOVER
	Sk³adnia: RECOVER nick [has³o]
	
	Umo¿liwia odzyskanie Twojego nicka w wypadku gdy 
	kto¶ go aktualnie u¿ywa. Tej opcji u¿ywa %S 
	automatycznie, gdy kto¶ próbuje u¿yæ nicka 
	chronionego opcj± KILL.
	
	Gdy u¿yjesz tej komendy, %S wprowadzi na sieæ 
	serwis bota z identycznym nickiem jak nick tego 
	u¿ytkownika, który u¿ywa Twojego nicka. To 
	spowoduje, ¿e serwer IRC od³±czy od sieci 
	u¿ytkownika u¿ywaj±cego Twojego nicka. Ten serwis 
	bot bêdzie obecny minutê na sieci zabezpieczaj±c,
	aby u¿ytkownik po ponownym po³±czeniu nie zaj±³ 
	ponownie Twojego nicka. Po up³ywie minuty mo¿esz 
	wej¶æ na swój nick. Je¶li nie masz zamiaru tyle 
	czekaæ musis¿ u¿yæ komendy 
	RELEASE (/msg %S HELP RELEASE) aby 
	zarz±daæ natychmiastowego zwrotu nicka od serwisów.
	
	Aby móc u¿yæ komendy RECOVER, musisz spe³niaæ, 
	który¶ z warunków: 
	1. Twój obecny adres (ten z /whois) musi byæ na 
	li¶cie dostêpu, 
	2. musisz byæ zidentyfikowany i byæ w grupie tego 
	nicka albo 
	3. musisz podaæ has³o do nicka.
					
NICK_HELP_RELEASE
	Sk³adnia: RELEASE nick [has³o]
	
	Informuje %S aby przesta³ trzymaæ Twojego nicka po
	opcji RECOVER lub skutkach dzia³ania opcji 
	KILL. Domy¶lnie nic jest trzymany przez jedn± 
	minutê, ta komenda ma za zadanie zwolniæ nicka 
	wcze¶niej.
	
	Aby móc u¿yæ komendy RELEASE, musisz spe³niaæ, 
	który¶ z warunków: 
	1. Twój obecny adres (ten z /whois) musi byæ na 
	li¶cie dostêpu, 
	2. musisz byæ zidentyfikowany i byæ w grupie tego 
	nicka albo 
	3. musisz podaæ has³o do nicka.
				
			
NICK_HELP_GHOST
	Sk³adnia: GHOST nick [has³o]
	
	Opcja likwiduje sesjê-ducha Twojego nicka. Taka 
	sesja istnieje wtedy, gdy u¿ytkownik nie jest 
	po³±czony, ale serwer IRC wci±¿ go widzi (i jego 
	nicka) - taki nick pozostaje na IRC przez pewien 
	czas do momentu, a¿ serwer IRC go nie od³±czy. To 
	siê mo¿e zdarzyæ gdy Twój komputer siê zawiesi, b±d¼ 
	padnie Twoje po³±czenie internetowe wtedy gdy 
	bêdziesz siedzieæ na IRCu. Mimo i¿ opcja ta s³u¿y 
	g³ównie do usuwania martwych sesji, równie dobrze 
	mo¿na jej u¿yæ zamiast skomplikowanej RECOVER, 
	aby odzyskaæ swój nick.
	
	Aby móc u¿yæ komendy GHOST, musisz spe³niaæ, 
	który¶ z warunków: 
	1. Twój obecny adres (ten z /whois) musi byæ na 
	li¶cie dostêpu, 
	2. musisz byæ zidentyfikowany i byæ w grupie tego 
	nicka albo 
	3. musisz podaæ has³o do nicka.			
	
NICK_HELP_INFO
	Sk³adnia: INFO nick [ALL]
	
	Wy¶wietla informacje na temat wybranego nicka takie
	jak w³a¶ciciel, kiedy ostatnio widziano, z jakiego 
	hosta oraz opcje nicka. Je¶li jeste¶ 
	zidentyfikowany(a) mo¿esz uzyskaæ wiêcej informacji 
	dopisuj±c na koñcu komendy s³owo ALL. Wtedy
	zostan± Ci wy¶wietlone wszystkie mo¿liwe informacje 
	bez wzglêdu na to czy jest nick ma opcjê PRIVATE 
	czy nie.

NICK_HELP_LIST
	Sk³adnia: LIST wzór
	
	Wy¶wietla wszystkie zarejestrowane nicki zgodnie z 
	podanym wzorem w postaci nick!ident@host. Nicki, 
	które maj± w³±czon± opcje PRIVATE nie zostan± 
	wy¶wietlone na li¶cie.
	
	Przyk³ady:
	
	    LIST *!adirm@mafia.ru
	        Lista wszystkich nicków z mask± 
		*!adirm@mafia.ru.
	
	    LIST *Bot*!*@*
	        Lista wszystkich nicków z Bot w nazwie 
		(wielko¶æ liter nie ma znaczenia).
	
	    LIST *!*@*.ircnet.pl
	        Lista wszystkich nicków z mask± domeny 
		ircnet.pl

NICK_HELP_ALIST
	Sk³adnia: ALIST [poziom]
	
	Lista wszystkich kana³ów na których jeste¶ w li¶cie 
	dostêpu. Opcjonalnie mo¿esz podaæ poziom XOP lub 
	zwyk³y numer poziomu. Wyniki wy¶wietl± kana³y, gdzie 
	masz wybrany poziom dostêpu.

	Przyk³ady:
	    ALIST Founder
	        Lista wszystkich kana³ów, na których jeste¶ 
	        za³o¿ycielem(k±) - czyli posiadasz status 
	        foundera.

	    ALIST AOP
	        Lista wszystkich kana³ów, na których masz 
	        poziom AOP w li¶cie dostêpu lub wiêkszy.

	    ALIST 10
	        Lista wszystkich kana³ów, na których masz 
	        10-ty poziom w li¶cie dostêpu lub wy¿szy.

	Kana³y z opcj± NOEXPIRE bêd± oznaczone znaczkiem
	wykrzyknika (!).

NICK_HELP_GLIST
	Sk³adnia: GLIST
	
	Lista wszystkich nicków znajduj±cych siê w Twojej 
	grupie.

NICK_HELP_STATUS
	Sk³adnia: STATUS nick...
	
	Zwraca informacjê czy u¿ytkownik u¿ywaj±cy danego 
	nicka jest rozpoznany aktualnie jako jego 
	w³a¶ciciel. Odpowied¼ ma nastêpuj±c± sk³adniê:
	
	    nick kod
	
	Gdzie nick jest tym nickiem wys³anym w 
	komendzie, a kod mo¿e oznaczaæ:
	
	0 - nie ma u¿ytkownika online lub nick 
	    niezarejestrowany.
	1 - u¿ytkownik nie jest rozpoznany jako w³a¶ciciel 
	    nicka.
	2 - jest rozpoznany jako w³a¶ciciel na podstawie 
	    listy dostêpu.
	3 - jest rozpoznany jako w³a¶ciciel na podstawie 
	    identyfikacji.
	
	Maksymalnie mo¿na podaæ 16 nicków za jednym razem. 
	Reszta bêdzie ignorowana. If no nickname is given, your status
	will be returned.

NICK_HELP_SENDPASS
	Sk³adnia: SENDPASS nick
	
	Wysy³a has³o podanego nicka na adres e-mail 
	przyporz±dkowany danemu nickowi. Jest to bardzo 
	u¿yteczna komenda do odzyskiwania 
	straconych/zapomnianych hase³.
	
	Mo¿e byæ organiczona do IRC operatorów na 
	niektórych sieciach.
	
	Ta komenda jest bezu¿yteczna (niedostêpna) wtedy 
	gdy has³a s± szyfrowane.

NICK_SERVADMIN_HELP
	Administratorzy Serwisów mog± odrejestrowaæ ka¿dy 
	nick bez potrzeby identyfikacji dla niego i mog± 
	widzieæ listê dostêpu dla ka¿dego nicka 
	(/msg %S ACCESS LIST nick).

NICK_SERVADMIN_HELP_LOGOUT
	Sk³adnia: LOGOUT [nick [REVALIDATE]]
	
	Komenda ma odwrotne dzia³anie do komendy IDENTIFY 
	czyli zapomina identifikacje nicka oraz odbiera Ci 
	prawa zidentyfkowanego u¿ytkownika. Po tej komendzie 
	nie padnie pytanie o ponown± identyfikacje.
	
	Bez ¿adnego parametru komenda bêdzie siê odnosiæ do 
	Ciebie. Je¶li wpiszesz nick to dzia³anie bêdzie 
	dotyczyæ tego nicka, ale w tym przypadku serwisy ju¿
	poprosz± u¿ytkownika o ponown± identyfikacje tego¿ 
	nicka. Ta komenda jest dostêpna dla
	administratorów serwisów.

NICK_SERVADMIN_HELP_DROP
	Sk³adnia: DROP [nick]
	
	Bez ¿adnego parametru, spowoduje odrejestrowanie 
	Twojego nicka z bazy %S.
	
	Natomiast z parametrem, odrejestruje wybranego nicka 
	z bazy.Mo¿esz odrejestrowaæ ka¿dy nick w Twojej 
	grupie bez ¿adnych dodatkowych uprawnieñ. 
	Odrejestrowywanie ka¿dego nicka jest 
	dostêpne tylko dla administratorów serwisów.

NICK_SERVADMIN_HELP_INFO
	
	Administratorzy serwisów mog± u¿ywaæ parametru 
	ALL dla ka¿dego nicka.

NICK_SERVADMIN_HELP_LIST
	Sk³adnia: LIST wzór [FORBIDDEN] [NOEXPIRE] [UNCONFIRMED]
	
	Wy¶wietla wszystkie zarejestrowane nicki zgodnie z 
	podanym wzorem w postaci nick!ident@host. Nicki, 
	które maj± w³±czon± opcje PRIVATE zostan± 
	wy¶wietlone tylko administratorom serwisów. Nicki z 
	flag± NOEXPIRE zostan± oznaczone znakiem ! 
	na li¶cie dla administratorów serwisów.
	
	Je¶li podane bêd± flagi FORBIDDEN, NOEXPIRE lub 
	UNCONFIRMED to wtedy nicki, które je posiadaj± bêd± 
	wy¶wietlone. Je¶li wszystkie 3 opcje bêd± ustawione 
	to wszystkie 3 typy nicków bêd± wy¶wietlone. Ta 
	opcja jest dostêpna dla administratorów serwisów.  

	Przyk³ady:
	
	    LIST *!adirm@mafia.ru
	        Lista wszystkich nicków z mask± 
	        *!adirm@mafia.ru.
					   
	    LIST *Bot*!*@*
	        Lista wszystkich zarejestrowanych nicków z 
	      Bot w nazwie (wielko¶æ liter nie ma 
	        znaczenia).
					      
	    LIST * NOEXPIRE
	        Lista wszystkich nicków, które maj± 
	        ustawion± flage NOEXPIRE.

NICK_SERVADMIN_HELP_ALIST
	Sk³adnia: ALIST [nick] [poziom]
	
	Bez parametrów wy¶wietla liste kana³ów, na których 
	jeste¶ na li¶cie dostêpu. Z jednym parametrem 
	wy¶wietli liste kana³ów, na których nick jest w 
	li¶cie dostêpu. Z dwoma parametrami pokazuje listê 
	kana³ów, na których nick jest w li¶cie dostêpu z 
	poziomem dostêpu conajmniej poziom.

	Dostêpne tylko dla administratorów serwisów.

NICK_SERVADMIN_HELP_GLIST
	Sk³adnia: GLIST [nick]
	
	Bez parametrów wy¶wietla liste wszystkich nicków w 
	Twojej grupie.
	
	Je¶li jako parametr zostanie podany jaki¶ nick to 
	zostan± wy¶wietlone wszystkie nicki bêd±ce w grupie 
	razem z podanym nickiem.
	Dostêpne tylko dla administratorów serwisów.

NICK_SERVADMIN_HELP_GETPASS
	Sk³adnia GETPASS nick
	
	Wy¶wietla has³o dla podanego nicka.Notka: 
	dok³adna informacja o u¿yciu tej opcji (przez kogo, 
	dla jakiego nicka) zostanie wys³ana przy pomocy 
	WALLOP/GLOBOP.
	
	Dostêpne tylko dla administratorów serwisów.
	
	Opcja jest niedostêpna gdy szyfrowanie hase³ jest 
	w³±czone.

NICK_SERVADMIN_HELP_GETEMAIL
	Sk³adnia: GETEMAIL adres@email.com

	Wy¶wietla nicki, które posiadaj± podany adres 
	e-mail. 
	Notka nie mo¿na u¿ywaæ znaków zastêpczych dla 
	u¿ytkownika b±d¼ adresu e-mail. Kiedy ta komenda 
	jest u¿ywana, jest logowana informacja kto jej u¿y³ 
	oraz jaki poda³ e-mail.

	Dostêpna tylko dla administratorów serwisów.

NICK_SERVADMIN_HELP_FORBID
	Sk³adnia: FORBID nick [powód]
	
	Blokuje mo¿liwo¶æ rejestracji wybranego nicka. Mo¿na 
	j± anulowaæ tylko przez odrejestrowanie 
	zablokowanego nicka.
	
	Dla maksymalnej pewno¶ci dzia³ania, powód jest 
	wymagany.
	
	Opcja tylko dla adminów serwisów.

###########################################################################
#
# HELP ChanServ
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
	   GETPASS     Uzyskanie has³a w³a¶ciciela dla 
	               wskazanego kana³u 
CHAN_HELP_CMD_FORBID
	   FORBID      Zamkniêcie wskazanego kana³u.
CHAN_HELP_CMD_SUSPEND
	   SUSPEND     Blokada kana³u z zachowaniem 
	               wszelkich jego danych i ustawieñ.
CHAN_HELP_CMD_UNSUSPEND
	   UNSUSPEND   Odblokowanie kana³u.
CHAN_HELP_CMD_STATUS
	   STATUS      Zwraca aktualny poziom dostêpu do 
	               kana³u wskazanego u¿ytkownika.
CHAN_HELP_CMD_REGISTER
	    REGISTER   Rejestracja kana³u.
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   Identyfikacja w³a¶ciciela kana³u.
CHAN_HELP_CMD_SET
	    SET        Ustawienia ró¿nych opcji kana³u.
CHAN_HELP_CMD_AOP
	    AOP        Modyfikacje listy AOP kana³u.
CHAN_HELP_CMD_SOP
	    SOP        Modyfikacje listy SOP kana³u.
CHAN_HELP_CMD_ACCESS
	    ACCESS     Zarz±dzanie kana³ow± list± dostêpu
CHAN_HELP_CMD_LEVELS
	    LEVELS     Zarz±dzanie poziomami dostêpu.
CHAN_HELP_CMD_AKICK
	    AKICK      Zarz±dzanie kana³ow± list± AKICK.
CHAN_HELP_CMD_DROP
	    DROP       Odrejestrowanie kana³u.
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Odzyskanie utraconego has³a.
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
	    INVITE     Tells ChanServ to invite you or a given nick into a channel
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
	%S pozwala na rejestracjê oraz kontrolê 
	poszczególnych ustawieñ kana³ów. %S ma na celu 
	uniemo¿liwiæ innym u¿ytkownikom przejmowanie 
	kana³ów, poprzez ograniczenie dostêpu do statusu 
	operatora kana³u. Dostêpne komendy zosta³y podane 
	poni¿ej, aby ich u¿yæ napisz:
	/msg %S komenda. Aby uzyskaæ wiêcej 
	informacji o poszczególnych komendach, napisz: 
	/msg %S HELP komenda.
	 
	
CHAN_HELP_EXPIRES
	
	Uwaga! Kana³ który nie by³ u¿ywany od %d dni (np. 
	¿aden u¿ytkownik umieszczony na access li¶cie 
	kana³u, nie odwiedzi³ go w tym czasie) zostanie 
	automatycznie odrejestrowany.

CHAN_HELP_REGISTER
	Sk³adnia: REGISTER kanal has³o opis
	
	Rejestracja kana³u w bazie %S. Aby móc u¿yæ tej 
	komendy, musisz posiadaæ status operatora na kanale, 
	który chcesz zarejestrowaæ. Podane has³o bêdzie 
	wymagane w komendzie IDENTIFY, umo¿liwi dostêp 
	do konfiguracji ustawieñ kana³u. Ostatni parametr, 
	który musi zostaæ podany, jest ogólnym opisem 
	kana³u.
	
	W momencie zakoñczenia rejestracji, jeste¶ uznawany 
	za w³a¶ciciela kana³u. W³a¶ciciel(za³o¿yciel) mo¿e 
	modyfikowaæ wszelkie ustawienia kana³u, %S 
	automatycznie przyzna przys³uguj±ce uprawnienia 
	w³a¶ciciela po jego wej¶ciu na kana³.
	U¿ywaj komendy ACCESS 
	(/msg %S HELP ACCESS) aby 
	przyznawaæ wszelkie uprawnienia do operowania na 
	kanale innym u¿ytkownikom.
	
	UWAGA: Aby móc zarejestrowaæ w³asny kana³, konieczna
	jest wcze¶niejsza rejestracja nicka. Opis tej 
	procedury otrzymasz po wydaniu polecenia: 
	/msg %s HELP.

CHAN_HELP_IDENTIFY
	Sk³adnia: IDENTIFY kana³ has³o
	
	Procedura identyfikacji, umo¿liwiaj±ca rozpoznanie 
	przez %S jako za³o¿yciela kana³u. Wiele komend 
	wymaga, aby dokonaæ IDENTYFIKACJI przed ich 
	wydaniem. Has³o musi byæ takie same, jak podane przy 
	rejestracji kana³u.
	
CHAN_HELP_LOGOUT
	Sk³adnia: LOGOUT kana³ nick
	
	Komenda WYLOGOWANIA podanego nicka, sprawi ¿e bêdzie 
	on musia³ ponownie ZINDETYFIKOWAÆ siê aby móc 
	korzystaæ z nadanych uprawnieñ.
	
	Za³o¿yciel kana³u mo¿e wylogowaæ wszystkich 
	pozosta³ych u¿ytkowników; pozosta³e osoby, wylogowaæ 
	mog± jedynie siebie.

CHAN_HELP_DROP
	Sk³adnia: DROP kana³
	
	Odrejestrowuje podany kana³. Komenda mo¿e zostaæ 
	u¿yta jedynie przez w³a¶ciciela kana³u, po 
	uprzedniej IDENTIFYKACJI przy pomocy has³a
	kana³u.

CHAN_HELP_SET
	Sk³adnia: SET kana³ opcja parametry
	
	Pozwala w³a¶cicielowi kana³u na kontrolê nad jego 
	ustawieniami, oraz informacjami.
	
	Dostêpne opcje:
	
	   FOUNDER         Okre¶la w³a¶ciciela kana³u.
	   SUCCESSOR       Okre¶la spadkobiercê kana³u.
	   PASSWORD        Zmiana has³a w³a¶ciciela kana³u.
	   DESC            Zmiana opisu kana³u.
	   URL             Ustawia adres strony kana³u.
	   EMAIL           Przypisanie adresu e-mail do 
	                   kana³u.
	   ENTRYMSG        Konfiguracja tre¶ci wiadomo¶ci 
	                   powitalnej na kanale.
	   BANTYPE         Ustawienia typu banów zak³adanych 
	                   przez Serwisy.
	   MLOCK           Blokada modów kana³owych, 
	                (on lub off).
	   KEEPTOPIC       Zachowanie ustawionego wcze¶niej 
	                   topicu kana³u.
	   OPNOTICE        Informacje o egzekwowaniu komend 
		           OP/DEOP.
	   PEACE           Ograniczenia u¿ywania krytycznych 
	                   komend.
	   PRIVATE         Nie pokazuj kana³u po wydaniu 
	                   polecenia LIST.
	   RESTRICTED      Ograniczenia dostêpu do kana³u.
	   SECURE          W³±cz zabezpieczenia %S.
	   SECUREOPS       Ograniczenie nadania uprawnieñ 
	                   operatora kana³u.
	   SECUREFOUNDER   Ograniczenia nadania statusu 
	                   w³a¶ciciela kana³u.
	   SIGNKICK        Podpisywanie kicków wykonywanych 
	                   za pomoc± komendy KICK.
	   TOPICLOCK       Umo¿liwia zmiany topica kana³u 
	                   jedynie przy u¿yciu komendy 
			   TOPIC.
	   XOP             W³±cza/Wy³±cza system listy 
	                   uprawnieñ xOP 
			   (SOP, AOP, HOP, VOP).
	
	Napisz /msg %S HELP SET opcja, 
	aby uzyskaæ wiêcej informacji o poszczególnych 
	komendach.

CHAN_HELP_SET_FOUNDER
	Sk³adnia: SET kana³ FOUNDER nick
	
	Zmienia w³a¶ciciela kana³u. Nowy nick musi byæ 
	oczywi¶cie zarejestrowany.

CHAN_HELP_SET_SUCCESSOR
	Sk³adnia: SET kana³ SUCCESSOR nick
	
	Ustala spadkobiercê kana³u. Je¶li nick za³o¿yciela
	kana³u ulegnie przedawnieniu i zostanie 
	odrejestrowany, podczas gdy kana³ wci±¿ jest 
	aktywny, spadkobierca zostanie nowym ¿a³o¿ycielem 
	kana³u. Jednak¿e, je¶li spadkobierca  posiada zbyt 
	wiele (%d) zarejestrowanych kana³ów, kana³ zostanie 
	odrejestrowany. Nick spadkobiercy musi byæ 
	oczywi¶cie zarejestrowany.

CHAN_HELP_SET_PASSWORD
	Sk³adnia: SET kana³ PASSWORD has³o
	
	Zmienia has³o konieczne do zidentyfikowania siê 
	jako za³o¿yciel kana³u.

CHAN_HELP_SET_DESC
	Sk³adnia: SET kana³ DESC opis
	
	Ustawia opis kana³u, widoczny po wydaniu polecenia 
	LIST oraz INFO.

CHAN_HELP_SET_URL
	Sk³adnia: SET kanal URL [url]
	
	Pozwala ustawiæ adres strony www kana³u. Adres 
	bêdzie widoczny po u¿yciu polecenia INFO. Je¶li 
	komenda zostanie podana bez nowego adresu, wcze¶niej 
	podany URL ulegnie skasowaniu.

CHAN_HELP_SET_EMAIL
	Sk³adnia: SET kanal EMAIL [adres]
	
	Przypisuje adres e-mail do kana³u. Adres bêdzie 
	widoczny po u¿yciu polecenia INFO. Je¶li komenda 
	zostanie podana bez nowego adresu, wcze¶niej podany 
	e-mail ulegnie skasowaniu.

CHAN_HELP_SET_ENTRYMSG
	Sk³adnia: SET kana³ ENTRYMSG [tre¶æ]
	
	Ustala tre¶æ powiadomienia wysy³anego poprzez 
	/notice do u¿ytkowników wchodz±cych na kana³. Je¶li 
	komenda zostanie podana bez nowej tre¶æ, wcze¶niej
	ustalona wiadomo¶æ zostanie skasowana.

CHAN_HELP_SET_BANTYPE
	Sk³adnia: SET kana³ BANTYPE <typ banu>
	
	Ustala typ banów zak³adanych przez Serwisy.
	
	Typ banów oznaczany jest numerami od 0 do 3, 
	poszczególne numery oznaczaj±:
	
	0: ban w postaci *!ident@host
	1: ban w postaci *!*ident@host
	2: ban w postaci *!*@host
	3: ban w postaci *!*ident@*.domena

CHAN_HELP_SET_KEEPTOPIC
	Sk³adnia: SET kana³ KEEPTOPIC {ON | OFF}
	
	W³±cza lub wy³±cza opcjê zapamiêtywania topicu 
	kana³u. Kiedy zapamiêtywanie jest w³±czone, %S 
	zapamiêta ustawiony topic w przypadku gdy wszyscy 
	u¿ytkownicy opuszcz± kana³, i przywróci go po 
	ponownej aktywacji kana³u (czyli jak kto¶ wejdzie).

CHAN_HELP_SET_TOPICLOCK
	Sk³adnia: SET kana³ TOPICLOCK {ON | OFF}
	
	W³±cza lub wy³±cza opcjê blokowania topica kana³u.
	Je¶li opcja ta zostanie aktywowana, %S pozwoli 
	na zmiane topicu wy³±cznie poprzez wydanie komendy 
	TOPIC.

CHAN_HELP_SET_MLOCK
	Sk³adnia: SET kana³ MLOCK mody
	
	Ustawia blokadê modów kana³owych. %S pozwala na 
	definiowanie, które mody kana³owe maj± byæ zawsze 
	w³±czone, wy³±czone, lub ustawiane dowolnie przez 
	operatorów kana³u.
	
	Sk³adnia polecenia jest identyczna jak w komendzie 
	/MODE, tzn flagi poprzedzone znakiem+ 
	zostan± za³o¿one na kana³, natomiast poprzedzone 
	znakiem- bêd± zdejmowane z kana³u. W odró¿nieniu 
	od komendy /MODE, ka¿de u¿ycie SET MLOCK, 
	usunie wszystkie wcze¶niej zdefiniowane mody!
	
	UWAGA: W przypadku ustawiania klucza dostêpu do 
	kana³u (drugi przyk³ad poni¿ej), powinna rownie¿ 
	zostaæ aktywowana opcja RESTRICTED (zobacz 
	HELP SET RESTRICTED), w przeciwnym wypadku, 
	u¿ytkownik wchodz±cy na pusty kana³, bêdzie mog³ 
	odczytaæ za³o¿ony klucz!
	
	Przyk³ady:
	
	    SET kana³ MLOCK +nt-iklps
	        Wymusza za³o¿enie flag n oraz t, 
		oraz wy³±cza flagi i,
		k, l, p, s. 
		Nie wyszczególniona flaga m, mo¿e 
		byæ dowolnie w³±czana lub wy³±czana przez 
		operatorów kaba³u.
	
	    SET kana³ MLOCK +knst-ilmp <mój-klucz>
	        Wymusza za³o¿enie flag k, n, s 
		oraz t, mody i, l, m a tak¿e 
		p zostan± wy³±czone. Zostanie tak¿e 
		za³o¿ony klucz o warto¶ci: "mój-klucz". 
		Klucz to oczywi¶cie has³o.
	
	    SET kana³ MLOCK +
	        Usuwa wszelkie ograniczenia zak³adania flag. 
		Wszystkie mody mog± byæ dowolnie zak³adane i 
		zdejmowane z kana³u.

CHAN_HELP_SET_PEACE
	Sk³adnia: SET kana³ PEACE {ON | OFF}
	
	W³±cza lub wy³±cza opcjê PEACE dla kana³u.
	Kiedy opcja ta jest aktywna, operatorzy kana³u nie 
	bêd± mogli u¿ywaæ poleceñ kick lub ban, oraz 
	zmieniaæ statusu innego u¿ytkownika (przez %S) 
	którego poziom dostêpu jest równy lub wy¿szy ni¿ 
	ich samych.

CHAN_HELP_SET_PRIVATE
	Sk³adnia: SET kana³ PRIVATE {ON | OFF}
	
	W³±cza lub wy³±cza opcjê PRIVATE dla kana³u.
	Kiedy opcja ta jest w³±czona, kana³ nie bêdzie 
	widoczny po wydaniu polecenia: 
	/msg %S LIST.

CHAN_HELP_SET_RESTRICTED
	Sk³adnia: SET kana³ RESTRICTED {ON | OFF}
	
	W³±cza lub wy³±cza ograniczenie dostêpu do kana³u.
	Kiedy opcja ta zostanie w³±czona, u¿ytkownicy 
	normalnie nie uprawnieni do pe³nienia statusu 
	operatora kana³u (u¿ytkownicy z ujemnym poziomem 
	dostêpu, oraz je¶li zostanie w³±czona funkcja 
	secure ops, u¿ytkownicy bez zdefiniowanego 
	poziomu dostêpu) bêd± natychmiast wyrzucani i 
	banowani na kanale.

CHAN_HELP_SET_SECURE
	Sk³adnia: SET kana³ SECURE {ON | OFF}
	
	W³±cza lub wy³±cza %S opcje ochronn± kana³u. Je¶li
	funkcja ta zostanie aktywowana, jedynie 
	u¿ytkownikom, którzy zarejestrowali swoje nicki i 
	dokonali %s identyfikacji, bêdzie przyznawany dostêp 
	do operowania kana³em zgodnie z wpisami w li¶cie 
	dostêpu (ACCESS LIST).

CHAN_HELP_SET_SECUREOPS
	Sk³adnia: SET kana³ SECUREOPS {ON | OFF}
	
	Kiedy funkcja ta zostanie w³±czona, u¿ytkownicy 
	którzy nie zostali wpisani na liste dostêpu kana³u 
	(ACCESS LIST), nie bêd±	mogli korzystaæ ze statusu 
	operatora.
	
CHAN_HELP_SET_SECUREFOUNDER
	Sk³adnia: SET kana³ SECUREFOUNDER {ON | OFF}
	
	Kiedy funkcja ta %S zostanie aktywowana, jedynie 
	prawdziwy w³a¶ciciel kana³u (a nie ka¿da osoba, 
	która dokona identyfikacji has³em kana³u) bêdzie 
	móg³ odrejestrowaæ kana³, zmieniæ jego has³o 
	dostêpowe, a tak¿e zmieniæ za³o¿yciela oraz 
	spadkobiercê (successor) kana³u.
	
CHAN_HELP_SET_SIGNKICK
	Sk³adnia: SET #kana³ SIGNKICK {ON | LEVEL | OFF}
	
	W³±cza lub wy³±cza "podpisywanie" komend kick. 
	Kiedy opcja SIGNKICK zostanie aktywowana, 
	polecenia %S kick bêd± podpisywane nickiem osoby 
	wydaj±cej komendê.
	
	Parametr LEVEL powoduje ¿e nicki osób z równym 
	lub wy¿szym od okre¶lonego poziomem dostêpu, nie 
	bêd± pokazywane przy poleceniu kick. 
	Zobacz /msg %S HELP LEVELS, 
	aby uzyskaæ wiêcej informacji.

CHAN_HELP_SET_XOP
	Sk³adnia: SET #kana³ XOP {ON | OFF}
	
	W³±cza lub wy³±cza system dostêpu xOP dla kana³u.
	Kiedy xOP zostanie w³±czony, konieczne jest 
	u¿ywanie poleceñ AOP/ SOP/ VOP w celu 
	przyznawania uprawnieñ na kanale innym u¿ytkowniom.
	Wy³±czenie funkcji xOP wymaga u¿ywania polecenia 
	ACCESS.
	
	Uwaga techniczna: kiedy lista dostêpu (access 
	list) zostanie zmieniona na system list XOP, 
	wszelkie definicje poziomów dostepu (access level) 
	zostan± zmienione. Ponowne przej¶cie z xOP na system 
	listy dostêpu, nie spowoduje odzyskania poprzedniej 
	listy poziomów dostêpu!
	
	Po zmianie systemu definiowania dostêpu na xOP, 
	nale¿y sprawdziæ czy u¿ytkownikom zosta³ przypisany 
	odpowiedni poziom dostêpu. Automatyczne 
	przyporz±dkowanie nie zawsze musi byæ poprawne.
	Nie zaleca siê przej¶cia na system XOP, je¶li 
	definicje poziomów dostêpu dla kana³u, zosta³y 
	wcze¶niej zmodyfikowane, przy pomocy polecenia 
	LEVELS.
	
CHAN_HELP_SET_OPNOTICE
	Sk³adnia: SET kana³ OPNOTICE {ON | OFF}
	
	Kiedy opcja ta zostanie w³±czona, %S bêdzie 
	informowa³ u¿ytkowników przebywaj±cych na kanale, 
	za ka¿dym razem, kiedy zostanie u¿yta komenda OP 
	lub DEOP.

CHAN_HELP_AOP
	Sk³adnia: AOP kana³ ADD nick
	          AOP kana³ DEL {nick | numer | list}
	          AOP kana³ LIST [maska | list]
	          AOP kana³ CLEAR
	
	Pozwala na zarz±dzaniem dostêpem do automatycznie 
	nadawanego przez statusu operatora. 
	U¿ytkownicy, którym zostanie nadane to uprawnienie, 
	bêd± automatycznie opowani na kanale, bêd± mogli siê 
	odbanowaæ, oraz wys³ac zaproszenie (invite) na kana³. 
	
	Komenda AOP ADD dodaje podany nick do listy AOP.
	
	Komenda AOP DEL usuwa z listy podany nick. 
	Je¶li jako parametr zostanie podany zakres numerów 
	wpisów z listy AOP, dostêp dla u¿ytkowników 
	znajduj±cych siê na li¶cie pod wskazanymi pozycjami 
	zostanie usuniêty (zobacz poni¿szy przyk³ad dla 
	polecenia LIST).
	
	Polecenie AOP LIST wy¶wietla zawarto¶æ listy 
	AOP. Parametr maska pozwala zawêziæ wynik 
	polecenia do jedynie tych wpisów, których host 
	pasuje do wskazanej w komendzie maski (dozwolone
	jest u¿ywanie symbolu * - zastêpuje on dowolny 
	ci±g	znaków).
	Jako parametr mo¿na podaæ tak¿e numery wpisów na 
	li¶cie AOP, na przyk³ad:

	   AOP kana³ LIST 2-5,7-9
	      Wy¶wietli wpisy na li¶cie od numeru 2 do 5, 
	      oraz od 7 do 9.
	      
	Komenda AOP CLEAR usuwa wszystkie wpisy na 
	li¶cie AOP.
	
	Polecenia AOP ADD oraz AOP DEL s± 
	zastrze¿one jedynie dla u¿ytkowników z uprawnieniami 
	SOP lub wy¿szymi. AOP CLEAR mo¿e byæ u¿ywany 
	tylko przez w³a¶ciciela kana³u. Wszyscy u¿ytkownicy 
	wpisani na liste AOP s± uprawnieni do u¿ywania 
	komendy AOP LIST.
	
	Lista AOP mo¿e byæ wy³±czona dla twojego kana³u, w 
	takim przypadku nale¿y u¿ywac kana³owej listy 
	dostêpu (access list). Zobacz 
	/msg %S HELP ACCESS, aby uzyskaæ 
	wiêcej informacji, oraz
	/msg %S HELP SET XOP aby dowiedzieæ siê jak 
	zmieniæ sposób zarz±dzania kana³em na system xOP.

CHAN_HELP_HOP
	Sk³adnia: HOP kana³ ADD nick
	          HOP kana³ DEL {nick | numer | list}
	          HOP kana³ LIST [maska | list]
	          HOP kana³ CLEAR
	
	Pozwala na zarz±dzanie dostêpem do automatycznie 
	nadawanego przez statusu halfop. 
	U¿ytkownicy którym zostanie nadane to uprawnienie, 
	bêdzie automatycznie nadawany status halfop'a na 
	kanale.

	Komenda HOP ADD dodaje podany nick do listy HOP.
	
	Komenda HOP DEL usuwa z listy podany nick. Je¶li 
	jako parametr zostanie podany zakres numerów wpisów 
	z listy HOP, dostêp dla u¿ytkowników znajduj±cych 
	siê na li¶cie pod wskazanymi pozycjami zostanie 
	usuniêty (zobacz poni¿szy przyk³ad dla polecenia 
	LIST).
	
	Polecenie HOP LIST wy¶wietla zawarto¶æ listy 
	HOP. Parametr maska pozwala zawêziæ wynik 
	polecenia do jedynie tych wpisów, których host 
	pasuje do wskazanej w komendzie maski (dozwolone
	jest u¿ywanie symbolu * - zastêpuje on dowolny 
	ci±g znaków). Jako parametr mo¿na podaæ tak¿e numery 
	wpisów na li¶cie HOP, na przyk³ad:
	
		HOP kana³ LIST 2-5,7-9
		Wy¶wietli wpisy na li¶cie od numeru 2 do 5, 
		oraz od 7 do 9.

	Komenda HOP CLEAR usuwa wszystkie wpisy na 
	li¶cie HOP.
	
	Polecenia HOP ADD oraz HOP DEL s± 
	zastrze¿one jedynie dla u¿ytkowników z uprawnieniami 
	AOP lub wy¿szymi. HOP CLEAR mo¿e byæ u¿ywany 
	jedynie przez w³a¶ciciela kana³u.
	
	Lista HOP mo¿e byæ wy³±czona dla twojego kana³u, w 
	takim przypadku nale¿y u¿ywac kana³owej listy 
	dostêpu (access list). Zobacz 
	/msg %S HELP ACCESS, aby uzyskaæ 
	wiêcej informacji, oraz
	/msg %S HELP SET XOP, aby dowiedzieæ siê 
	jak zmieniæ sposób zarz±dzania kana³em na system xOP.

CHAN_HELP_SOP
	Sk³adnia: SOP kana³ ADD nick
	          SOP kana³ DEL {nick | numer | list}
	          SOP kana³ LIST [maska | list]
	          SOP kana³ CLEAR
	
	Pozwala na zarz±dzaniem dostêpu do automatycznie 
	nadawanego przez statusu SOP. U¿ytkownicy, którym 
	zostanie nadane to uprawnienie, mog± wykonywaæ takie 
	same operacje jak ci dopisani do listy AOP, a 
	dodatkowo tak¿e maj± mo¿liwo¶æ korzystania z funkcji 
	AutoKick oraz BadWords.

	Komenda SOP ADD dodaje podany nick do listy SOP.
	
	Komenda SOP DEL usuwa z listy podany nick. Je¶li 
	jako parametr zostanie podany zakres numerów wpisów 
	z listy SOP, dostêp dla u¿ytkowników znajduj±cych 
	siê na li¶cie pod wskazanymi pozycjami zostanie 
	usuniêty (zobacz poni¿szy przyk³ad dla polecenia 
	LIST).
	
	Polecenie SOP LIST wy¶wietla zawarto¶æ listy 
	SOP. Parametr maska pozwala zawêziæ wynik 
	polecenia do jedynie tych wpisów, których host 
	pasuje do wskazanej w komendzie maski (dozwolone
	jest u¿ywanie symbolu * - zastêpuje on dowolny 
	ci±g znaków). Jako parametr mo¿na podaæ tak¿e numery 
	wpisów na li¶cie SOP, na przyk³ad:
	
		SOP kana³ LIST 2-5,7-9
		Wy¶wietli wpisy na li¶cie od numeru 2 do 5, 
		oraz od 7 do 9.

	Komenda SOP CLEAR usuwa wszystkie wpisy na 
	li¶cie SOP.
	
	Polecenia SOP ADD, SOP DEL oraz SOP CLEAR 
	mog± byæ wydane jedynie przez w³a¶ciciela kana³u. 
	SOP LIST jest dostêpne tak¿e dla u¿ytkowników z
	listy AOP.
	
	Lista SOP mo¿e byæ wy³±czona dla twojego kana³u, w 
	takim przypadku nale¿y u¿ywac kana³owej listy dostêpu 
	(access list). Zobacz /msg %S HELP ACCESS, 
	aby uzyskaæ wiêcej informacji, oraz
	/msg %S HELP SET XOP aby dowiedzieæ siê 
	jak zmieniæ sposób zarz±dzania kana³em na system xOP.
	
CHAN_HELP_VOP
	Sk³adnia: VOP kana³ ADD nick
	          VOP kana³ DEL {nick | numer | list}
	          VOP kana³ LIST [maska | list]
	          VOP kana³ CLEAR
	
	Pozwala na zarz±dzaniem dostêpu do automatycznie lub na ¿±danie 
	nadawanego przez statusu Voice. 

	Komenda VOP ADD dodaje podany nick do listy VOP.

	Komenda VOP DEL usuwa z listy podany nick. Je¶li 
	jako parametr zostanie podany zakres numerów wpisów 
	z listy VOP, dostêp dla u¿ytkowników znajduj±cych 
	siê na li¶cie pod wskazanymi pozycjami zostanie 
	usuniêty (zobacz poni¿szy przyk³ad dla polecenia 
	LIST).

	Polecenie VOP LIST wy¶wietla zawarto¶æ listy 
	VOP. Parametr maska pozwala zawêziæ wynik 
	polecenia do jedynie tych wpisów, których host 
	pasuje do wskazanej w komendzie maski (dozwolone
	jest u¿ywanie symbolu * - zastêpuje on dowolny 
	ci±g znaków).
	Jako parametr mo¿na podaæ tak¿e numery wpisów na 
	li¶cie VOP, na przyk³ad:
	
		VOP kana³ LIST 2-5,7-9
		Wy¶wietli wpisy na li¶cie od numeru 2 do 5, 
		oraz od 7 do 9.

	Komenda VOP CLEAR usuwa wszystkie wpisy na 
	li¶cie VOP.
	
	Polecenia VOP ADD, VOP DEL oraz VOP LIST 
	mog± byæ wydawane przez osoby z dostêpem AOP lub 
	wy¿szym. Komenda VOP CLEAR mo¿e byæ u¿ywana 
	jedynie przez w³a¶ciciela kana³u.
	
	Lista VOP mo¿e byæ wy³±czona dla twojego kana³u, w 
	takim przypadku nale¿y u¿ywac kana³owej listy 
	dostêpu (access list). 
	Zobacz /msg %S HELP ACCESS, aby uzyskaæ 
	wiêcej informacji, oraz 
	/msg %S HELP SET XOP aby dowiedzieæ siê jak 
	zmieniæ sposób zarz±dzania kana³em na system xOP.

CHAN_HELP_ACCESS
	Sk³adnia: ACCESS kana³ ADD nick level
	          ACCESS kana³ DEL {nick | numer | list}
	          ACCESS kana³ LIST [maska | list]
	          ACCESS kana³ CLEAR
	
	Polecenie pozwala na zarz±dzanie wpisami w kana³owej
	li¶cie dostêpu (access list). Ustalony poziom 
	dostêpu okre¶la, którzy u¿ytkownicy mog± pe³niæ 
	funkcje operatorów kana³u, oraz posiadaæ dostêp do 
	komend %S. Ró¿ne poziomy okre¶laj± inne 
	przywileje. 
	Napisz msg %S HELP ACCESS LEVELS,
	aby uzyskaæ wiêcej informacji. Ka¿dy u¿ytkownik, 
	który nie zosta³ wpisany na listê dostepu, posiada 
	poziom uprawnieñ równy 0.
	
	Komenda ACCESS ADD dodaje podany nick do listy z 
	okre¶lonym poziomem dostêpu. Je¶li nick jest ju¿ 
	wpisany na li¶cie, jego poziom dostêpu zostanie 
	zmieniony zgodnie z podanym parametrem. Przyznawany 
	poziom dostêpu nie mo¿e byæ wy¿szy ni¿ poziom osoby
	wydaj±cej komende ADD.
	
	Polecenie ACCESS DEL usuwa podany nick z listy. 
	Je¶li jako parametr zostanie podany zakres numerów z 
	listy dostêpu dla u¿ytkowników znajduj±cych siê na 
	li¶cie pod wskazanymi pozycjami zostanie usuniêty 
	(zobacz poni¿szy przyk³ad dla polecenia LIST).
	
	Polecenie ACCESS LIST wy¶wietla zawarto¶æ listy. 
	Parametr maska pozwala zawêziæ wynik polecenia 
	do jedynie tych wpisów, których host pasuje do 
	wskazanej w komendzie maski (dozwolone jest u¿ywanie 
	znaku * - zastêpuje on dowolny ci±g znaków).
	Jako parametr mo¿na podaæ tak¿e numery wpisów na 
	li¶cie, na przyk³ad:

	   ACCESS kana³ LIST 2-5,7-9
	    Wy¶wietli wpisy na li¶cie od numeru 2 do 5, oraz 
	    od 7 do 9.

	Komenda ACCESS CLEAR usuwa wszystkie wpisy na 
	li¶cie.
	
CHAN_HELP_ACCESS_LEVELS
	Poziomy dostêpu u¿ytkowników:
	
	Standardowe poziomy dostêpu zosta³y podane poni¿ej:
	   Founder	 Pe³ny dostêp do poleceñ %S,
	                 automatyczne nadawanie statusu
	                 operatora po wej¶ciu na kana³. 
	                 UWAGA: tylko jedna osoba mo¿e byæ 
	                 w³a¶cicielem (Founder) kana³u. Nie 
	                 mo¿na nadaæ tego uprawnienia za 
	                 pomoc± polecenia ACCESS.
	        10   Dostêp do polecenia AKICK, 
	                 automatyczne opowanie.
	         5   Automatyczne opowanie.
	         3   Automatyczne nadawanie voica.
	         0   Brak specjalnych przywilejów. Inne 
	                 osoby mog± nadaæ status operatora 
	                 na kanale temu u¿ytkownikowi (chyba 
	                 ¿e w³±czona zosta³a opcja 
			 secure-ops).
	        <0   Brak mo¿liwo¶ci pe³nienia funkcji 
	                 operatora.
	   
	Standardowe poziomy mog± zostaæ zmienione, mo¿liwe 
	jest tak¿e definiowanie w³asnych poziomów dostêpu. 
	Zobacz: /msg %S HELP LEVELS, aby 
	uzyskaæ wiêcej informacji.

CHAN_HELP_AKICK
	Sk³adnia: AKICK kana³ ADD maska [powód]
	          AKICK kana³ STICK maska
	          AKICK kana³ UNSTICK maska
	          AKICK kana³ DEL maska
	          AKICK kana³ LIST [maska]
	          AKICK kana³ VIEW [maska]
	          AKICK kana³ ENFORCE
	          AKICK kana³ CLEAR
	
	Polecenie zarz±dza list± AutoKick dla kana³u. Je¶li
	u¿ytkownik znajduj±cy siê na li¶cie AKICK, wejdzie 
	na kana³, %S za³o¿y bana a nastepnie wykopie 
	u¿ytkownika z kana³u.

	Polecenie AKICK ADD dodaje podany nick lub maskê 
	hosta do listy AutoKick. Je¶li zostanie podany 
	powód, bêdzie on u¿ywany przy wykopywaniu 
	u¿ytkownika dodanego do listy z kana³u, je¶li nie 
	zostanie podany, u¿ywany bêdzie standardowy zwrot: 
	You have been banned from channel.
	
	Komenda AKICK STICK umo¿liwia permanentne 
	dodanie podanej maski do banów na kanale. Za ka¿dym 
	razem gdy kto¶ zdejmie wskazanego bana z kana³u, 
	%S automatycznie za³o¿y go ponownie.
	Parametrem polecenia nie mo¿e byæ nick a jedynie 
	host.
	
	Komenda AKICK UNSTICK niweluje dzia³anie 
	polecenia AKICK STICK - umo¿liwia zdjêcie bana z 
	kana³u.
	
	Polecenia AKICK DEL usuwa wskazany nick lub maskê
	z listy AutoKick. 
	
	AKICK LIST - polecenie wy¶wietla listê wpisów 
	AutoKick. Je¶li zostanie podany opcjonalny parametr 
	maski, pokazane zostan± jedynie wpisy pasuj±ce do 
	niej.
	
	AKICK VIEW jest bardziej szczegó³ow± odmian± 
	polecenia AKICK LIST.
	
	Polecenie AKICK ENFORCE powoduje ¿e %S 
	usunie z kana³u wszystkich u¿ytkowników, których 
	hosty pasuj± do wskazanych masek w li¶cie AKICK.
	
	Komenda AKICK CLEAR usuwa wszelkie wpisy z 
	listy AKICK.

CHAN_HELP_LEVELS
	Sk³adnia: LEVELS kana³ SET typ poziom
	          LEVELS kana³ {DIS | DISABLE} typ
	          LEVELS kana³ LIST
	          LEVELS kana³ RESET
	
	Polecenie LEVELS umo¿liwia przypisywanie 
	uprawnieñ poszczególnym poziomom dostepu (LEVEL 
	ACCESS). Przy u¿yciu tej komendy, mo¿esz definiowaæ 
	poziomy dostêpu do wszystkich funkcji %S 
	(SET FOUNDER, SET PASSWORD podobnie jak i
	LEVELS sa zastrze¿one wy³±cznie dla w³a¶ciciela 
	kana³u).
	
	LEVELS SET pozwala zmieniæ przypisany poziom dla 
	okre¶lonej operacji lub ich grup. LEVELS DISABLE 
	(lub DIS w skrócie) wy³±cza okre¶lon± funkcjê, 
	lub zastrzega dostêp do niej jedynie dla w³a¶ciciela 
	kana³u.
	LEVELS LIST wy¶wietla aktualnie ustalone poziomy 
	dla ka¿dej z operacji. LEVELS RESET przywraca 
	standardowe ustawienia poziomów dostêpu - identyczne 
	z tymi jakie ustawiane s± dla kana³u po jego 
	utworzeniu (zobacz HELP ACCESS LEVELS).
	
	Aby obejrzeæ spis wszystkich funkcji które mog± 
	zostaæ przypisane poziomom dostêpu, zobacz: 
	HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	
	Spis wszystkich funkcji dostepnych dla poziomów 
	dostepu:

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Sk³adnia: INFO #kana³ [ALL]
	
	Komenda INFO pozwala uzyskaæ informacje o 
	zarejestrowanym kanale: o za³o¿ycielu, dacie 
	rejestracji, dacie ostatniej aktywno¶ci oraz opisie 
	i ustawionych opcjach. Je¶li podany zostanie 
	parametr ALL pokazana bêdzie tak¿e zdefiniowana 
	wiadomo¶æ powitalna, oraz informacje o spadkobiercy 
	kana³u (successor)
	
	Standardowo u¿ycie parametru ALL jest dozwolone
	jedynie dla za³o¿ycieli kana³u.

CHAN_HELP_LIST
	Sk³adnia: LIST wzorzec
	
	Listuje wszystkie kana³y których nazwa pasuje do 
	zdefiniowanego wzorca. (Kana³y z opcj± PRIVATE 
	nie bêd± pokazywane).

CHAN_HELP_OP
	Sk³adnia: OP [kana³ [nick]]
	
	Nadaje status operatora na wskazanym kanale. Je¶li 
	nick nie zostanie podany, opowany bêdzie wydaj±cy 
	komendê. Je¶li nie podany zostanie ani #kana³ ani 
	nick, opowany bêdzie wydaj±cy komendê, na wszystkich 
	kana³ach, na których przebywa, uwzglêdniaj±c 
	przys³uguj±ce mu na nich poziomy dostêpu.
	
	Standardowo polecenia tego mog± u¿ywaæ u¿ytkownicy 
	wpisani na listê AOP lub posiadaj±cy poziom dostêpu 
	5-ty lub wy¿szy.

CHAN_HELP_DEOP
	Sk³adnia: DEOP [kana³ [nick]]
	
	Deopuje wskazanego u¿ytkownika na kanale. Je¶li nie 
	zostanie podany nick, deopowany zostanie wydaj±cy 
	komendê. Je¶li nie zostanie podany nick oraz kana³, 
	deopowany bêdzie wydaj±cy komedê na wszystkich 
	kana³ach na których przebywa, z uwzglêdnieniem 
	przys³uguj±cych mu na nich uprawnieñ.
	
	Standardowo polecenia tego mog± u¿ywaæ u¿ytkownicy 
	wpisani na listê AOP lub posiadaj±cy poziom dostêpu 
	5-ty lub wy¿szy.
	
CHAN_HELP_VOICE
	Sk³adnia: VOICE [kana³ [nick]]
	
	Nadaje voice wskazanemu u¿ytkownikowi. Je¶li nick 
	nie zostanie podany, voice nadany zostanie 
	wydaj±cemu polecenie. Je¶li nie podany zostanie nick 
	i kana³, voice przydzielony zostanie u¿ytkownikowi 
	wydaj±cemu polecenie, na wszystkich kana³ach na 
	których przebywa, z uwzglêdnieniem przys³uguj±cych 
	mu na nich uprawnieñ.
	
	Standardowo polecenia tego mog± u¿ywaæ u¿ytkownicy 
	wpisani na listê AOP lub posiadaj±cy poziom dostêpu 
	5-ty lub wy¿szy, oraz u¿ytkownicy wpisani na listê 
	VOP, lub posiadaj±cy poziom dostêpu 3 umo¿liwiaj±cy 
	samodzielne nadanie statusu voice.

CHAN_HELP_DEVOICE
	Sk³adnia: DEVOICE [kana³ [nick]]
	
	Zabiera status voice wskazanemu u¿ytkownikowi. Je¶li 
	nick nie zostanie podany, voice zostanie zdjêty 
	u¿ytkownikowi wydaj±cemy polecenie. Je¶li nie podany 
	zostanie nick oraz kana³, voice zostanie zdjêty 
	u¿ytkownikowi wydaj±cemy polecenie na wszystkich 
	kana³ach na których przebywa, z uwzglêdnieniem 
	przys³uguj±cych mu na nich uprawnieñ.
	
	Standardowo polecenia tego mog± u¿ywaæ u¿ytkownicy 
	wpisani na listê AOP lub posiadaj±cy poziom dostêpu 
	5 lub wy¿szy, oraz u¿ytkownicy wpisani na listê VOP 
	lub posiadaj±cy poziom dostêpu 3 umo¿liwiaj±cy 
	samodzielne zdjêcie statusu voice.
	

CHAN_HELP_HALFOP
	Sk³adnia: HALFOP [kana³ [nick]]
	
	Nadaje status halfop wskazanemu u¿ytkownikowi. Je¶li 
	nick nie zostanie podany, halfop zostanie nadany 
	u¿ytkownikowi wydaj±cemu polecenie. Je¶li nie podany 
	zostanie nick oraz kana³, halfop zostanie nadany 
	u¿ytkownikowi wydaj±cemu polecenie na wszystkich 
	kana³ach na których przebywa, z uwzglêdnieniem 
	przys³uguj±cych mu na nich uprawnieñ.

	Standardowo polecenia tego mog± u¿ywaæ u¿ytkownicy 
	wpisani na listê AOP. lub posiadaj±cy poziom dostêpu 
	5 lub wy¿szy, oraz u¿ytkownicy wpisani na listê HOP, 
	lub posiadaj±cy poziom dostêpu 4 umo¿liwiaj±cy 
	samodzielne nadanie statusu halfop.
	
CHAN_HELP_DEHALFOP
	Sk³adnia: DEHALFOP [kana³ [nick]]
	
	Zdejmuje status halfop wskazanemu u¿ytkownikowi. 
	Je¶li nick nie zostanie podany, halfop zostanie 
	zabrany u¿ytkownikowi wydaj±cemu polecenie. Je¶li 
	nie zostanie podany nick oraz kana³, halfop zostanie 
	zabrany u¿ytkownikowi wydaj±cemu polecenie na 
	wszystkich kana³ach na których przebywa, z 
	uwzglêdnieniem przys³uguj±cych mu na nich uprawnieñ.
	
	Standardowo polecenia tego mog± u¿ywaæ u¿ytkownicy 
	wpisani na listê AOP. lub posiadaj±cy poziom dostêpu 
	5-ty lub wy¿szy, oraz u¿ytkownicy wpisani na listê 
	HOP, lub posiadaj±cy poziom dostêpu 4 umo¿liwiaj±cy 
	samodzielne zdjêcie statusu halfop.

CHAN_HELP_PROTECT
	Sk³adnia: PROTECT [kana³ [nick]]
	
	Chroni wskazanego u¿ytkownika na kanale. Je¶li nick 
	nie zostanie podany opcja zostanie w³±czona dla 
	wydaj±cego polecenie. Je¶li nie zostanie podany nick 
	oraz kana³ polecenie bêdzie odnosi³o siê do 
	u¿ytkownika wydaj±cego polecenie na wszystkich 
	kana³ach na których przebywa, z uwzglêdnieniem 
	przys³uguj±cych mu na nich uprawnieñ.
	
	Standardowo komendy tej mog± u¿ywaæ w³a¶ciciele
	kana³ów, u¿ytkownicy wpisani na listê SOP lub 
	posiadaj±cy poziom dostêpu 10 lub wy¿szy 
	umo¿liwiaj±cy samodzielne w³±czenie opcji 
	ochronnej.

CHAN_HELP_DEPROTECT
	Sk³adnia: DEPROTECT [kana³ [nick]]
	
	Usuwa ochronê wskazanego u¿ytkownika na kanale. 
	Je¶li nick nie zostanbie podany, polecenie bêdzie 
	odnosiæ siê do wydaj±cego komendê. Je¶li nie podany 
	zostanie nick oraz kana³, usuniêta zostanie ochrona 
	u¿ytkownika wydaj±cego polecenie, na wszystkich 
	kana³ach na których przebywa, z uwzglêdnieniem 
	przys³uguj±cych mu na nich uprawnieñ.

	Standardowo komendy tej mog± u¿ywaæ w³a¶ciciele 
	kana³ów, u¿ytkownicy wpisani na listê SOP lub 
	posiadaj±cy poziom dostêpu 10 lub wy¿szy 
	umo¿liwiaj±cy samodzielne wy³±czenie opcji 
	ochronnej.

CHAN_HELP_OWNER
	Sk³adnia: OWNER [kana³]
	
	Nadaje uprawnienia w³a¶ciciela (Ownera) kana³u. 
	Je¶li kana³ nie zostanie podany, status ownera 
	zostanie nadany na wszystkich kana³ach na których 
	przebywasz z uwzglêdnieniem przys³uguj±cych na nich 
	uprawnieñ.
	
	Standardowo komendy tej mog± u¿ywaæ w³a¶ciciele 
	kana³ów.

CHAN_HELP_DEOWNER
	Sk³adnia: DEOWNER [kana³]
	
	Usuwa uprawnienia w³a¶ciciela (Ownera) kana³u. 
	Je¶li kana³ nie zostanie podany, status ownera 
	zostanie usuniêty na wszystkich kana³ach na których 
	przebywasz z uwzglêdnieniem przys³uguj±cych na nich 
	uprawnieñ.

	Standardowo komendy tej mog± u¿ywaæ za³o¿yciele 
	kana³ów.

CHAN_HELP_INVITE
	Sk³adnia: INVITE kana³
	
	Zaproszenie (INVITE) na kana³ przez %S.
	
	Standardowo komenda dostêpna dla u¿ytkowników 
	wpisanych na liste AOP oraz posiadaj±cych poziom 
	dostêpu 5 lub wy¿szy.

CHAN_HELP_UNBAN
	Sk³adnia: UNBAN kana³
	
	Sprawia ¿e %S zdejmie wszelkie bany 
	uniemo¿liwiaj±ce wydaj±cemu komendê wej¶cie na 
	wskazany kana³.
	
	Standardowo komenda dostêpna dla u¿ytkowników 
	wpisanych na liste AOP oraz posiadaj±cych poziom 
	dostêpu 5 lub wy¿szy.
	
CHAN_HELP_KICK
	Sk³adnia: KICK [kana³ [nick [powód]]]
	
	Wyrzuca (kick) wskazanego u¿ytkownika z kana³u. 
	Je¶li nick nie zostanie podany. wyrzucony zostanie 
	wydaj±cy komendê. Je¶li nie podany zostanie nick 
	oraz kana³, wyrzucony zostanie wydaj±cy komendê, ze 
	wszystkich kana³ów na których przebywa z 
	uwzglêdnieniem przys³uguj±cych mu na nich uprawnieñ.
	
	Standardowo komenda dostêpna dla u¿ytkowników 
	wpisanych na liste AOP oraz posiadaj±cych poziom 
	dostêpu 5 lub wy¿szy.

CHAN_HELP_BAN
	Sk³adnia: BAN [kana³ [nick [powód]]]
	
	Nak³ada bana na wskazanego u¿ytkownika. Je¶li nick 
	nie zostanie podany, ban zostanie za³o¿ony na 
	wydaj±cego polecenie. Je¶li nie zostanie podany nick 
	oraz kana³ ban zostanie za³o¿ony na wydaj±cego 
	polecenie na wszystkich kana³ach na których 
	przebywa, z uwzglêdnieniem przys³uguj±cych mu na 
	nich uprawnieñ.
	
	Standardowo komenda dostêpna dla u¿ytkowników 
	wpisanych na liste AOP oraz posiadaj±cych poziom 
	dostêpu 5 lub wy¿szy.

CHAN_HELP_TOPIC
	Sk³adnia: TOPIC kana³ [topic]
	
	Wymusza ustawienie podanego topicu na kanale przez 
	%S. Je¶li nie zostanie podana tre¶c nowego 
	topicu, dotychczasowy zostanie usuniêty. Komendy 
	mo¿na u¿ywaæ wraz z opcj± SET TOPICLOCK 
	(zobacz: /msg %S HELP SET TOPICLOCK, aby 
	uzyskaæ wiêcej informacji).
	
	Standardowo komenda ta jest dostêpna dla w³a¶ciciela 
	kana³u.

CHAN_HELP_CLEAR
	Sk³adnia: CLEAR kana³ opcje
	
	Umo¿liwia kompleksowe usuniêcie niektórych ustawieñ na
	kanale poprzez %S. Dostêpne opcje:
	     
	   MODES    Zdejmuje wszystkie flagi z kana³u (np 
	            i, k, l, m, n, p, s, t).
	   BANS     Zdejmuje wszystkie bany na kanale.
	   EXCEPTS  Zdejmuje wyj±tki od banów na kanale.
	   OPS      Zdejmuje status operatora wszystkim 
	            u¿ytkownikom na kanale.
	   HOPS     Zdejmuje status pó³opa wszystkim 
	            u¿ytkownikom na kanale.	
	   VOICES   Zdejmuje voica wszystkim u¿ytkownikom 
	            na kanale.
	   USERS    Wyrzuca (kick) wszystkich u¿ytkowników 
	            z kana³u.
	
	Standardowo komenda ta jest dostêpna dla w³a¶ciciela 
	kana³u.
	
CHAN_HELP_GETKEY
	Sk³adnia: GETKEY kana³
	
	Zwraca klucz dostepu ustanowiony na wskazanym 
	kanale. Komenda g³ównie przeznaczona do 
	wykorzystania dla botów lub innych automatów, wynik 
	jest podawany w formacie:

	KEY <kana³> <klucz>
	
	Je¶li klucz nie zosta³ ustanowiony, wynik <klucz> 
	bêdzie przyjmowa³ warto¶æ "NO KEY".
	
CHAN_HELP_SENDPASS

	Sk³adnia: SENDPASS kana³
	
	Wysy³a has³o do kana³u na adres email za³o¿yciela. 
	Funkcja bardzo pomocna przy odzyskiwaniu 
	zapomnianych hase³.

	W niektórych sieciach, dostêp do tego polecenia 
	ograniczony jest dla IRC Operatorów.

	Komenda jest niedostêpna je¶li zosta³o w³±czone 
	szyfrowanie hase³.

CHAN_SERVADMIN_HELP
	
	Administratorzy Serwisów mog± tak¿e usun±æ ka¿dy z 
	kana³ów bez potrzeby identyfikowania siê z u¿yciem 
	has³a w³a¶ciciela. Mog± przejrzeæ listy dostêpów, 
	listy AKICK, i ustawienia poziomów dostêpu dla 
	kana³ów.
	    
CHAN_SERVADMIN_HELP_LOGOUT
	Sk³adnia: LOGOUT kana³ [nick]
	
	Polecenie wyloguje u¿ytkownika o wskazanym nicku, 
	oznacza to, ¿e bêdzie siê on musia³ ponownie 
	zidentyfikowaæ, aby odzyskaæ wszelkie przys³uguj±ce 
	mu uprawnienia.
	
	Je¶li jeste¶ za³o¿ycielem kana³u mo¿esz wylogowaæ 
	wszystkich u¿ytkowników, w przeciwnym wypadku 
	jedynie siebie.
	
	Administratorzy Serwisów mog± wylogowaæ dowolnego 
	u¿ytkownika z ka¿dego kana³u. Pominiêcie podania 
	nicka, spowoduje wylogowanie wszystkich u¿ytkowników 
	na danym kanale.

CHAN_SERVADMIN_HELP_DROP
	Sk³adnia: DROP kana³
	
	Odrejestrowuje wskazany kana³.

CHAN_SERVADMIN_HELP_SET
	
	Administratorzy Serwisów mog± ustawiæ opcjê 
	NOEXPIRE dla wskazanego kana³u, która 
	uniemozliwi automatyczne odrejestrowanie 
	nieu¿ywanego kana³u. Oznacza to, ¿e kana³
	z t± opcj± nie podlega prawu wygasania.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Sk³adnia: SET kana³ NOEXPIRE {ON | OFF}
	
	Ustala czy wskazany kana³ mo¿e ulec automatycznemu
	odrejestrowaniu je¶li nie bêdzie ju¿ u¿ywany.
	
	Komenda dostêpna dla Administratorów Serwisów.

CHAN_SERVADMIN_HELP_INFO
	
	Administratorzy Servisów mog± u¿ywaæ parametru ALL 
	odnosz±cego siê do wszystkich kana³ów.

CHAN_SERVADMIN_HELP_LIST
	Sk³adnia: LIST wzorzec [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
	
	Listuje wszystkie zarejestrowane kana³y których 
	nazwa pasuje do podanego wzorca. Kana³y z ustawion± 
	opcj± PRIVATE zostan± pokazane jedynie 
	Admninistratorom Serwisów. Kana³y z ustawion± opcj± 
	NOEXPIRE bêd± oznaczane znakiem ! dla 
	Administratorów Serwisów.
	
	Je¶li zostanie podany parametr FORBIDDEN, SUSPENDED 
	lub NOEXPIRE, pokazane zostan± jedynie kana³y z 
	w³±czonymi tymi opcjami. Parametrów tych mo¿na 
	u¿ywaæ jednocze¶nie. Komenda dostêpna jedynie dla 
	Administratorów Serwisów.

CHAN_SERVADMIN_HELP_GETPASS
	Sk³adnia: GETPASS kana³
	
	Zwraca has³o dostêpu dla wskazanego kana³u. 
	UWAGA: za ka¿dym razem gdy ta komenda zostanie 
	u¿yta, wiadomo¶æ zawieraj±ca dane wydaj±cego 
	polecenie GETPASS oraz kana³ dla którego zosta³o ono 
	zastosowane, bêdzie logowana oraz rozsy³ana za 
	pomoc± WALLOP/GLOBOP.
	
	Polecenie dostepne dla Administratorów Serwisów.

CHAN_SERVADMIN_HELP_FORBID
	Sk³adnia: FORBID kana³ [powód]

	Blokuje wskazany kana³ uniemo¿liwiaj±c jego 
	rejestracjê oraz u¿ywanie. Blokada mo¿e zostaæ 
	wy³±czona poprzez odrejestrowanie kana³u.
	
	W niektórych sieciach podanie powodu jest wymagane.
	
	Polecenie dostêpne dla Administratorów Serwisów.

CHAN_SERVADMIN_HELP_SUSPEND
	Sk³adnia: SUSPEND kana³ [powód]
	
	Zawieszenie dzia³ania wskazanego kana³u, 
	uniemo¿liwiaj±ce jego rejestracjê oraz u¿ywanie. 
	Blokada mo¿e zostaæ wy³±czona poprzez u¿ycie 
	polecenia UNSUSPEND. Zawieszenie kana³u nie powoduje 
	utraty wszystkich jego ustawieñ i danych.
	
	W niektórych sieciach podanie powodu jest wymagane.
	
	Polecenie dostepne dla Administratorów Serwisów.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Sk³adnia: UNSUSPEND kana³
	
	Cofa zawieszenie kana³u na³o¿one poleceniem 
	SUSPEND.
	
	Polecenie dostepne dla Administratorów Serwisów.

CHAN_SERVADMIN_HELP_STATUS
	Sk³adnia: STATUS kana³ nick
	
	Wy¶wietla aktualny poziom dostêpu (access level) 
	wskazanego nicka na kanale. Format odpowiedzi jest 
	nastêpuj±cy:
	
	    STATUS kana³ nick poziom-dostêpu
	
	W przypadku wystapienia b³êdu, format odpowiedzi 
	bêdzie nastêpuj±cy:
	
	    STATUS ERROR informacja-o-b³êdzie
	
	Polecenie dostepne dla Administratorów Serwisów.

###########################################################################
#
# Wiadomo¶ci pomocy MemoServ 
#
###########################################################################
MEMO_HELP_CMD_SEND
	    SEND   Wyslij wiadomo¶æ do u¿ytkownika/kana³u.
MEMO_HELP_CMD_CANCEL
	    CANCEL Wycofaj ostatni± ustawion± wiadomo¶æ.
MEMO_HELP_CMD_LIST
	    LIST   Wy¶wietl wszystkie ustawione wiadomo¶ci.
MEMO_HELP_CMD_READ
	    READ   Przeczytaj jedn± lub wiêcej wiadomo¶ci.
MEMO_HELP_CMD_DEL
	    DEL    Skasuj jedn± lub wiêcej wiadomo¶ci.
MEMO_HELP_CMD_SET
	    SET    Ustaw opcje dla wiadomo¶ci.
MEMO_HELP_CMD_INFO
	    INFO   Wy¶wietl informacje o wiadomo¶ciach.
MEMO_HELP_CMD_RSEND
	    RSEND  Wysy³a wiadomo¶æ z ¿±daniem potwierdzenia 
	    	   odbioru.
MEMO_HELP_CMD_CHECK
	    CHECK  Checks if last memo to a nick was read
MEMO_HELP_CMD_SENDALL
	    SENDALL  Wysy³a wiadomo¶æ do wszystkich 
	    zarejestrowanych u¿ytkowników.

MEMO_HELP_CMD_STAFF
	    STAFF  Wysy³a wiadomo¶æ do wszystkich 
	    operatorów/administratorów Serwisów.

MEMO_HELP_HEADER
	%S jest mechanizmem umo¿liwiaj±cym przesy³anie
	krótkich wiadomo¶ci innym uzytkownikom sieci lub
	kana³om(*). Zarówno nick osoby wysy³aj±cej jak i
	adresata wiadomo¶ci lub adresowany kana³, musz±
	byæ zarejestrowane.

	%S lista dostêpnych komend:


MEMO_HELP_ADMIN
	not used.
MEMO_HELP_OPER
	not used.
MEMO_HELP_FOOTER
	
	Napisz /msg %S HELP komenda, aby 
	uzyskaæ pomoc odno¶nie powy¿szych komend.
	
	(*) Standardowo ka¿dy u¿ytkownik z poziomem dostepu 
	    10 do kana³u mo¿e czytaæ wiadomo¶ci przes³ane na 
	    kana³. Mo¿na's to zmieniæ za pomoc± komendy %s
	  LEVELS.

MEMO_HELP_SEND
	Sk³adnia: SEND {nick | kana³} tre¶æ-wiadomo¶ci
	
	Wysy³a wskazanej osobie lub na podany kana³ 
	wiadomo¶æ o okre¶lonej tre¶ci. W przypadku wysy³ania 
	do u¿ytkownika otrzyma on/ona komunikat o 
	oczekuj±cej wiadomo¶ci. Adresat nick/kana³ musi byæ 
	zarejestrowany.
	
MEMO_HELP_CANCEL
	Sk³adnia: CANCEL {nick | kana³}
	
	Wycofuje ostatni± wys³an± wiadomo¶æ, o ile nie 
	zosta³a ona jeszcze dostarczona adresatowi.

MEMO_HELP_LIST
	Sk³adnia: LIST [kana³] [list | NEW]
	
	Wy¶wietla wszystkie posiadane wiadomo¶ci. Parametr 
	NEW powoduje pokazanie jedynie nie przeczytanych 
	wiadomo¶ci. Nieprzeczytane wiadomo¶ci oznaczone s± 
	poprzez "*" po lewej stronie numeru. Mo¿na tak¿e 
	sprecyzowaæ liste wy¶wietlanych wiadomo¶ci podaj±c 
	ich numery, np:

		LIST 2-5,7-9
		  Wy¶wietli wiadomo¶ci od numeru 2 do 5 
		  oraz od 7 do 9.

MEMO_HELP_READ
	Sk³adnia: READ [kana³] {numer | list | LAST | NEW}
	
	Wy¶wietla wskazan± wiadomo¶æ. Je¶li parametr LAST 
	zostanie podany, pokazana zostanie ostatnia z 
	wiadomo¶ci. NEW oznacza wszystkie nowe wiadomo¶ci. 
	Podaj±c numer mo¿emy okre¶liæ która z wiadomo¶ci ma 
	zostaæ wy¶wietlona. Mo¿na tak¿e sprecyzowaæ listê 
	wy¶wietlanych wiadomo¶ci podaj±c ich numery, 
	np:
		READ 2-5,7-9
		  Wy¶wietli wiadomo¶ci od numeru 2 do 5 
		  oraz od 7 do 9.

MEMO_HELP_DEL
	Sk³adnia: DEL [kana³] {numer | list | LAST | ALL}
	
	Kasuje jedn± lub wiêcej wiadomo¶ci. Dopuszczalne 
	jest podanie numeru wiadomo¶ci lub grupy wiadomo¶ci, 
	przyk³ad poni¿ej.
	
	Parametr LAST oznacza ostatni± wiadomo¶æ.
	U¿ycie ALL spowoduje usuniêcie wszystkich 
	wiadomo¶ci.
	
	Przyk³ady:
	
	   DEL 1
	      Kasuje pierwsz± wiadomo¶æ.
	
	   DEL 2-5,7-9
	      Kasuje wiadomo¶ci od numeru 2 do 5 oraz od 7 
	      do 9.

MEMO_HELP_SET
	Sk³adnia: SET opcja parametry

	Komenda umo¿liwia zmianê nastêpuj±cych ustawieñ:
	
	   NOTIFY   Ustala czy zostaniesz powiadomiony/a
	            o nowych wiadomo¶ciach (tylko dla 
	            nicków)
	   LIMIT    Ustala maksymaln± ilo¶æ wiadomo¶ci jak± 
	            mo¿esz przyj±æ.
	
	Napisz /msg %S HELP SET opcja, 
	aby uzyskaæ wiêcej informacji o poszczególnych 
	opcjach.

MEMO_HELP_SET_NOTIFY
	Sk³adnia: SET NOTIFY {ON | LOGON | NEW | OFF}

	Ustala kiedy zostanie wys³ane powiadomienie o nowych
	wiadomo¶ciach:
	
	   ON      Powiadomienie zostanie wys³ane po 
	           zalogowaniu siê, kiedy nie bêdzie 
	           ustawiony tryb /AWAY, oraz w momencie
	           otrzymania wiadomo¶ci.
	   LOGON   Powiadomienie zostanie wys³ane jedynie 
	           po zalogowaniu siê lub po wy³±czeniu 
	           trybu /AWAY.
	   NEW     Powiadomienie bêdzie wysy³ane jedynie w 
	           momencie otrzymania wiadomo¶ci.
	   OFF     Powiadomienia nie bêd± wysy³ane.
	
	Opcja ON jest kombinacj± opcji LOGON oraz 
	NEW.

MEMO_HELP_SET_LIMIT
	Sk³adnia: SET LIMIT [kana³] limit
	
	Ustala Twoj± (lub kana³u) maksymaln± liczbê 
	wiadomo¶ci. Je¶li ustalona zostanie ona na 0, 
	przyjmowanie wiadomo¶ci zostanie wy³±czone. Limit 
	nie mo¿e byæ wiêkszy ni¿ %d.

MEMO_HELP_INFO
	Sk³adnia: INFO [kana³]
	
	Wy¶wietla ilo¶æ wiadomo¶ci, ilo¶æ wiadomo¶ci 
	nieprzeczytanych, oraz limit przyjmowania 
	wiadomo¶ci. Je¶li podany zostanie kana³ wy¶wietlone 
	informacje bêd± odnosi³y siê do wskazanego kaa³u.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Sk³adnia: SET LIMIT [u¿ytkownik | kana³] {limit | NONE} [HARD]
	
	Ustala maksymaln± ilo¶æ wiadomo¶ci jak± mo¿e 
	posiadaæ u¿ytkownik lub kana³. Ustalenie limitu 
	na 0, wy³±czy przyjmowanie wiadomo¶ci. Parametr 
	NONE pozwoli na nielimitowan± ilo¶æ wiadomo¶ci. 
	Je¶li nie podany zostanie nick lub kana³ zmieniony 
	zostanie limit wydaj±cego polecenie.
	
	Dodanie paramteru HARD uniemo¿liwia zmiane 
	limitu przez u¿ytkownika.

	Polecenie SET LIMIT jest dostêpne dla 
	Administratorów Serwisów. Pozostali u¿ytkownicy 
	mog± jedynie modyfikowaæ w³asny limit lub kana³u na, 
	którym maj± do tego uprawnienia. Nie mog± wy³±czyæ 
	limitu, a tak¿e ustalic jego warto¶ci powy¿ej %d 
	oraz u¿yæ parametru HARD.

MEMO_SERVADMIN_HELP_INFO
	Sk³adnia: INFO [nick | kana³]

	Polecenie wydane bez parametru wy¶wietla informacjê o
	ilo¶ci w³asnych wiadomo¶ci, liczbie wiadomo¶ci nie 
	odczytanych oraz limicie ilo¶ci wiadomo¶ci.
	
	Je¶li zostanie wskazany kana³, pokazane zostan± te 
	same informacje w odniesieniu do tego kana³u.
	
	Administratorzy Serwisów mog± obejrzeæ informacje 
	dla poszczególnych u¿ytkowników podaj±c parametr 
	nick.

MEMO_HELP_STAFF
	Sk³adnia: STAFF tre¶æ-wiadomo¶ci

	Wysy³a do wszystkich operatorów/administratorów 
	Serwisów wiadomo¶æ o podanej tre¶ci.
	U¿ytkownicy wpisani jednocze¶nie na listê 
	operatorów jak i administratorów otrzymaj± 
	wiadomo¶æ podwójnie.

MEMO_HELP_SENDALL
	Sk³adnia: SENDALL tre¶æ-wiadomo¶ci

	Wysy³a wiadomo¶æ o okre¶lonej tre¶ci do wszystkich 
	u¿ytkowników.
	
MEMO_HELP_RSEND
	Sk³adnia: RSEND {nick | kana³} tre¶c-wiadomo¶ci
	
	Wysy³a wskazanemu u¿ytkownikowi lub na okre¶lony 
	kana³ wiadomo¶æ o podanej tre¶ci. Adresat musi byæ 
	zarejestrowany. Kiedy wiadomo¶æ zostanie przeczytana 
	przez odbiorce, automatyczne powiadomienie o tym 
	zostanie przes³ane do nadawcy.

MEMO_HELP_CHECK
	Sk³adnia: CHECK nick
	
	Sprawdza czy _ostatnia_ wiadomo¶æ wys³ana do 
	wskazanej osoby zosta³a przeczytana. Polecenie to 
	dzia³a jedynie dla wiadomo¶ci wysy³anych innym 
	u¿ytkownikom, a nie kana³om.

###########################################################################
#
# OperServ help messages
#
###########################################################################

OPER_HELP_CMD_GLOBAL
	   GLOBAL      Wiadomo¶æ do wszystkich u¿ytkowników.
OPER_HELP_CMD_STATS
	   STATS       Informacje na temat serwisów i sieci.
OPER_HELP_CMD_OPER
	   OPER        Modyfikacja listy operatorów serwisów.
OPER_HELP_CMD_ADMIN
	   ADMIN       Modyfikacja listy adminów serwisów.
OPER_HELP_CMD_STAFF
	   STAFF       Wy¶wietla ekipe serwisów i jej stan.
OPER_HELP_CMD_MODE
	   MODE        Zmiana modów kana³owych.
OPER_HELP_CMD_KICK
	   KICK        Wykopanie u¿ytkownika z kana³u.
OPER_HELP_CMD_CLEARMODES
	   CLEARMODES  Czyszczenie modów na kanale.
OPER_HELP_CMD_KILLCLONES
	   KILLCLONES  Skilluj u¿ytkowników z danego hosta.
OPER_HELP_CMD_AKILL
	   AKILL       Modyfikacje listy AKILL. 
OPER_HELP_CMD_SGLINE
	    SGLINE      Modyfikacje listy SGLINII.
OPER_HELP_CMD_SQLINE
	    SQLINE      Modyfikacje listy SQLINII.
OPER_HELP_CMD_SZLINE
	    SZLINE      Modyfikacje listy SZLINII.
OPER_HELP_CMD_CHANLIST
	   CHANLIST    Lista wszystkich rekordów kana³ów.
OPER_HELP_CMD_USERLIST
	   USERLIST    Lista wszystkich rekordów u¿ytkowników.
OPER_HELP_CMD_LOGONNEWS
	   LOGONNEWS   Modyfikacja wiadomo¶ci wy¶wietlanych 
	               u¿ytkownikom po po³±czeniu.
OPER_HELP_CMD_RANDOMNEWS
	   RANDOMNEWS  Modyfikacja losowych wiadomo¶ci wy¶wietlanych 
	               u¿ytkownikom po po³±czeniu.
OPER_HELP_CMD_OPERNEWS
	   OPERNEWS    Modyfikacja wiadomo¶ci wy¶wietlanych ircopom.
OPER_HELP_CMD_SESSION
	   SESSION     Pokazuje liste sesji.
OPER_HELP_CMD_EXCEPTION
	   EXCEPTION   Modyfikacja listy wyj±tków dla limitów sesji. 
OPER_HELP_CMD_NOOP
	   NOOP        Tymczasowe usuniêcie wszystkich O:linii na 
	               serwerze.
OPER_HELP_CMD_JUPE
	   JUPE        Fake serwer do blokady po³±czeñ.
OPER_HELP_CMD_IGNORE
	   IGNORE      Modyfikacja listy ignore serwisów.
OPER_HELP_CMD_SET
	   SET         Ustawienia ró¿nych, globalnych opcji.
OPER_HELP_CMD_RELOAD
	   RELOAD      Prze³adowanie pliku konfiguracyjnego.
OPER_HELP_CMD_UPDATE
	   UPDATE      Natychmiastowa i wymuszona aktualizacja baz
	               serwisów na dysku.
OPER_HELP_CMD_RESTART
	   RESTART     Zapis baz i restart serwisów.
OPER_HELP_CMD_QUIT
	   QUIT        Wy³±czenie serwisów bez zapisywania baz.
OPER_HELP_CMD_SHUTDOWN
	   SHUTDOWN    Wy³±czenie serwisów z zapisaniem baz.
OPER_HELP_CMD_DEFCON
	   DEFCON      Modyfikacje systemu DefCon.
OPER_HELP_CMD_CHANKILL
	   CHANKILL    Skilluj wszystkich u¿ytkowników na kanale.
OPER_HELP_CMD_OLINE
	    OLINE       Daje flagi opera wybranemu u¿ytkownikowi 
OPER_HELP_CMD_UMODE
	    UMODE       Zmiana usermod dla u¿ytkownika.
OPER_HELP_CMD_SVSNICK
	    SVSNICK     Wymuszona zmiana nicka danego u¿ytkownika.
OPER_HELP_CMD_MODLOAD
	   MODLOAD     Za³aduj modu³.
OPER_HELP_CMD_MODUNLOAD
	   MODUNLOAD   Wy³aduj modu³.
OPER_HELP_CMD_MODINFO
	   MODINFO     Informacja na temat wybranego modu³u.
OPER_HELP_CMD_MODLIST
	   MODLIST     Lista za³adowanych modu³ów.

OPER_HELP
	komendy %S:

OPER_HELP_LOGGED
	Notka: Wszystkie komendy w %S s± logowane!

OPER_HELP_GLOBAL
	Syntax: GLOBAL message
	
	Allows IRCops to send messages to all users on the network.
	The message will be sent from the nick %s.

OPER_HELP_STATS
	Syntax: STATS [AKILL | ALL | RESET]
	
	Without any option, shows the current number of users and
	IRCops online (excluding Services), the highest number of
	users online since Services was started, and the length of
	time Services has been running.
	
	With the AKILL option, displays the current size of the
	AKILL list and the current default expiry time.
	
	The ALL option is available only to Services admins, and
	displays information on Services' memory usage.  Using this
	option can freeze Services for a short period of time on
	large networks, so don't overuse it!
	
	The RESET option currently resets the maximum user count
	to the number of users currently present on the network.
	
	UPTIME may be used as a synonym for STATS.

OPER_HELP_OPER
	Syntax: OPER ADD nick
	        OPER DEL {nick | entry-num | list}
	        OPER LIST [mask | list]
	        OPER CLEAR
	
	Allows the Services admins to add or remove nicknames
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
	All other use limited to Services admins.

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
	Syntax: IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick]

	Allows Services Admins to make Services ignore a nick for a
	certain time or until the next restart. The default time
	format is seconds. You can specify it by using units. Valid
	units are: s for seconds, m for minutes, h for hours and
	d for days. Combinations of these units are not permitted. To 
	make Services permanently ignore the user type 0 as time.

OPER_HELP_MODE
	Syntax: MODE channel modes
	
	Allows Services operators to set channel modes for any
	channel.  Parameters are the same as for the standard /MODE
	command.
	
	Limited to Services operators.

OPER_HELP_UMODE
	Syntax: UMODE user modes
	
	Allows Services admins to set user modes for any user.
	Parameters are the same as for the standard /MODE
	command.
	
	Limited to Super admins.

OPER_HELP_OLINE
	Syntax: OLINE user flags
	
	Allows Services admins to give Operflags to any user.
	Flags have to be prefixed with a "+" or a "-". To
	remove all flags simply type a "-" instead of any flags.

	Limited to Super admins.

OPER_HELP_CLEARMODES
	Syntax: CLEARMODES channel [ALL]
	
	Clears all binary modes (i,k,l,m,n,p,s,t) and bans from a
	channel.  If ALL is given, also clears all ops and
	voices (+o and +v modes) from the channel.
	
	Limited to Services operators.

OPER_HELP_KICK
	Syntax: KICK channel user reason
	
	Allows IRCops to kick a user from any channel.
	Parameters are the same as for the standard /KICK
	command.  The kick message will have the nickname of the
	IRCop sending the KICK command prepended; for example:
	
	*** SpamMan has been kicked off channel #my_channel by %S (Alcan (Flood))
	
	Limited to Services operators.

OPER_HELP_SVSNICK
	Syntax: SVSNICK nick newnick
	
	Forcefully changes a user's nickname from nick to newnick.
	
	Limited to Super admins

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
	
	Limited to Services operators.
	
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
	
	Limited to Services operators.

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
	
	Limited to Services operators.

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
	
	Limited to Services operators.

OPER_HELP_SET
	Syntax: SET option setting

	Sets various global Services options.  Option names
	currently defined are:
	    READONLY   Set read-only or read-write mode
	    LOGCHAN    Report log messages to a channel
	    DEBUG      Activate or deactivate debug mode
	    NOEXPIRE   Activate or deactivate no expire mode
	    SUPERADMIN Activate or deactivate super-admin mode
	    SQL   	 Activate or deactivate sql mode
	    IGNORE     Activate or deactivate ignore mode
	    LIST       List the options

	Limited to Services admins.

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

	Setting this will grant you extra privlages such as the
	ability to be "founder" on all channel's etc...

	This option is not persistant, and should only be used when
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

	Display the various OperServ settings

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
	
	Limited to Services admins.

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
	
	Limited to Services admins.

OPER_HELP_RAW
	Syntax: RAW text
	
	Sends a string of text directly to the server to which
	Services is connected.  This command has a very limited
	range of uses, and can wreak havoc on a network if used
	improperly.  DO NOT USE THIS COMMAND unless you are
	absolutely certain you know what you are doing!
	
	Limited to Services admins.

OPER_HELP_UPDATE
	Syntax: UPDATE
	
	Causes Services to update all database files as soon as you
	send the command.
	
	Limited to Services admins.
	
OPER_HELP_RELOAD
	Syntax: RELOAD

	Causes Services to reload the configuration file. Note that
	some directives still need the restart of the Services to
	take effect (such as Services' nicknames, activation of the 
	session limitation, etc.)
	
	Limited to Services admins.

OPER_HELP_QUIT
	Syntax: QUIT
	
	Causes Services to do an immediate shutdown; databases are
	not saved.  This command should not be used unless
	damage to the in-memory copies of the databases is feared
	and they should not be saved.  For normal shutdowns, use the
	SHUTDOWN command.
	
	Limited to Services admins.

OPER_HELP_SHUTDOWN
	Syntax: SHUTDOWN
	
	Causes Services to save all databases and then shut down.
	
	Limited to Services admins.

OPER_HELP_RESTART
	Syntax: RESTART
	
	Causes Services to save all databases and then restart
	(i.e. exit and immediately re-run the executable).
	
	Limited to Services admins.	

OPER_HELP_KILLCLONES
	Syntax: KILLCLONES nick
	
	Kills all users who have the same hostname as nick. A
	temporary AKILL, in the form *@host, is added to 
	prevent the offending clients from immediately 
	reconnecting. A wallops is also sent indicating who 
	used the command, which host was affected and how many
	users were killed. It's usefull for removing numerous 
	clones from the network.

	Limited to Services operators.
	
OPER_HELP_CHANLIST
	Syntax: CHANLIST [{pattern | nick} [SECRET]]
	
	Lists all channels currently in use on the IRC network, whether they
	are registered or not.

	If pattern is given, lists only channels that match it. If a nickname
	is given, lists only the channels the user using it is on. If SECRET is
	specified, lists only channels matching pattern that have the +s or
	+p mode.
	
	Limited to Services admins.

OPER_HELP_USERLIST
	Syntax: USERLIST [{pattern | channel} [INVISIBLE]]
	
	Lists all users currently online on the IRC network, whether their
	nick is registered or not.
	
	If pattern is given, lists only users that match it (it must be in
	the format nick!user@host). If channel is given, lists only users
	that are on the given channel. If INVISIBLE is specified, only users
	with the +i flag will be listed.
	
	Limited to Services admins.

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
	Syntax: MODLIST

	Lists all currently loaded modules.

	Limited to Services Roots.


###########################################################################
#
# Wiadomo¶ci pomocy BotServ
#
###########################################################################


BOT_HELP_CMD_BOTLIST
	   BOTLIST    Lista dostêpnych boty.
BOT_HELP_CMD_ASSIGN
	   ASSIGN     Przydziela bota do kana³u.
BOT_HELP_CMD_SET
	   SET        Konfiguruje bota.
BOT_HELP_CMD_KICK
	   KICK       Konfiguruje kick'i.
BOT_HELP_CMD_BADWORDS
	   BADWORDS   Zarz±dzanie lista zakaznych s³ów.
BOT_HELP_CMD_ACT
	   ACT        Makes the bot do the equivalent of a "/me" command
BOT_HELP_CMD_INFO
	   INFO       Allows you to see BotServ information about a channel or a bot
BOT_HELP_CMD_SAY
	   SAY        Makes the bot say the given text on the given channel
BOT_HELP_CMD_UNASSIGN
	   UNASSIGN   Unassigns a bot from a channel
BOT_HELP_CMD_BOT
	   BOT        Zarz±dza list± botów sieci.

BOT_HELP
	%S pozwala umie¶ciæ bota na w³asnym kanale.
	Zosta³ stworzony dla u¿ytkowników, którzy nie mog±
	hostowaæ w³asnego bota, oraz dla tych sieci które
	nie pozwalaj± na ich u¿ywanie. Dostêpne komendy
	s± poni¿ej. Aby ich u¿yæ nale¿y wpisaæ: 
	/msg %S komenda. Aby uzyskaæ wiêcej 
	informacji wpisz /msg %S HELP komenda.
	
BOT_HELP_FOOTER
	Bot wejdzie na kana³ je¶li przebywa na nim 
	przynajmniej %d u¿ytkownik(ów).
	
BOT_HELP_BOTLIST
	Sk³adnia: BOTLIST
	
	Listuje dostêpne w sieci boty.
	
BOT_HELP_ASSIGN
	Sk³adnia: ASSIGN kana³ nick
	
	Przydziela bota o wskazanym nicku na kana³.
	
BOT_HELP_UNASSIGN
	Sk³adnia: UNASSIGN kana³
	
	U¿ycie tej komendy powoduje ¿e bot zostanie usuniêty 
	z kana³u, jednak jego konfiguracja nie zostanie 
	skasowana. Bêdzie mo¿na go ponownie przydzieliæ do 
	kana³u w ka¿dej chwili, bez potrzeby ponownego 
	konfigurowania.

BOT_HELP_INFO
	Sk³adnia: INFO {kana³ | nick}
	
	Wy¶wietla %S informacje o wskazanym kanale lub bocie, w
	zale¿no¶ci od podanego parametru. Z parametrem kana³ 
	zostan± pokazane informacje i opcje dotycz±ce danego 
	kana³u. Je¶li parametrem bêdzie nick bota, podane 
	informacje bêd± zawiera³ dane jak: data stworzenia
	bota, ilo¶æ obs³ugiwanych kana³ów itp.
	
BOT_HELP_SET
	Sk³adnia: SET kana³ opcja parametry
	
	Umo¿liwia konfiguracjê nastêpuj±cych opcji bota:
	
	   DONTKICKOPS      Bot nie bêdzie kopa³ opów.
	   DONTKICKVOICES   Bot nie bêdzie kopa³ voiceów.
	   GREET            W³±cza wiadomo¶æ powitaln±.
	   FANTASY          W³±cza publiczne komendy 
	                    kana³owe (fantasy).
	   SYMBIOSIS        Rozszerzony tryb bota. 
	
	Napisz /msg %S HELP SET opcja, 
	aby uzyskaæ wiêcej informacji.
	
BOT_HELP_SET_DONTKICKOPS
	Sk³adnia: SET kana³ DONTKICKOPS {ON|OFF}
	
	W³±cza lub wy³±cza opcjê nie kopania u¿ytkowników
	posiadaj±cych status operatora nawet je¶li nie 
	przys³uguje im poziom dostêpu NOKICK.
	
BOT_HELP_SET_DONTKICKVOICES
	Sk³adnia: SET kana³ DONTKICKVOICES {ON|OFF}
	
	W³±cza lub wy³±cza opcjê nie kopania u¿ytkowników
	posiadj±cych status Voice nawet je¶li nie 
	przys³uguje im poziom dostêpu NOKICK.
	
BOT_HELP_SET_FANTASY
	Sk³adnia: SET kana³ FANTASY {ON|OFF}

	W³acza lub wy³±cza dostêp do kana³owych poleceñ
	publicznych takich jak:	!op, !deop, !voice, 
	!devoice, !kick, !kb, !unban, !seen na kanale 
	(cze¶æ komend przyjmuje jako paremetry nicki 
	u¿ytkowników i powody).
	
	U¿ytkownicy dopuszczeni do u¿ywania komend 
	publicznych musz± mieæ odpowiedni poziom dostêpu,
	to¿samy z komendami (np. aby u¿ywac komendy
	!deop, nadane mu uprawnienia musz± zawieraæ
	operacjê OPDEOP).

BOT_HELP_SET_GREET
	Sk³adnia: SET kana³ GREET {ON|OFF}
	
	W³±cza lub wy³±cza wiadomo¶æ powitaln± dla kana³u,
	wy¶wietlan± u¿ytkownikom po wej¶ciu na kana³.
	
BOT_HELP_SET_SYMBIOSIS
	Sk³adnia: SET kana³ SYMBIOSIS {ON|OFF}
	
	W³±cza lub wy³±cza rozszerzony tryb bota dla kana³u.
	Gdy ten tryb jest w³±czon, bot bêdzie wykonywa³
	wszelkie dzia³ania na kanale w³±cznie z ustawianiem
	flag, modów i kicków zamiast %s.

BOT_HELP_KICK
	Sk³adnia: KICK kana³ opcja parametry
	
	Konfiguruje opcje kopania na kanale:
	
	   BOLDS        Kopanie za u¿ywanie pogrubienia.
	   BADWORDS     Kopanie za u¿ywanie zakazanych s³ów.
	   CAPS         Kopanie za pisanie WIELKIMI LITERAMI.
	   COLORS       Kopanie za stosowanie kolorków.
	   FLOOD        Kopanie flooduj±cych u¿ytkowników.
	   REPEAT       Kopanie za wielokrotne powtarzanie.
	   REVERSES     Kopanie za odwrócenie kolorów.
	   UNDERLINES   Kopanie za u¿ywanie podkre¶leñ.
	
	Napisz /msg %S HELP KICK opcja, 
	aby uzyskaæ wiêcej informacji po poszczególnych 
	funkcjach.
	
BOT_HELP_KICK_BOLDS
	Sk³adnia: KICK kana³ BOLDS {ON|OFF} [ttb]

	Ustawia kopanie za u¿ywanie pogrubionego tekstu.

	ttb to numer który okre¶la ile razy u¿ytkownik 
	stosuj±cy pogrubienie mo¿e byæ kopniêty zanim 
	ustawiony zostanie ban.

BOT_HELP_KICK_COLORS
	Sk³adnia: KICK kana³ COLORS {ON|OFF} [ttb]

	Ustawia kopanie za u¿ywanie kolorowego tekstu.
	
	ttb to numer który okre¶la ile razy u¿ytkownik 
	stosuj±cy kolorowanie tekstu mo¿e byæ kopniêty zanim 
	ustawiony zostanie ban. Nie podanie parametru ttb 
	wy³±cza wcze¶niej ustawione banowanie.

BOT_HELP_KICK_REVERSES
	Sk³adnia: KICK kana³ REVERSES {ON|OFF} [ttb]

	Ustawia kopanie za u¿ywanie odwrócenia kolorów 
	tekstu, komend± np. ctrl + r.

	ttb (time to ban) to numer który okre¶la ile razy 
	u¿ytkownik stosuj±cy odwrócenie kolorów tekstu mo¿e 
	byæ kopniêty zanim ustawiony zostanie ban. Nie 
	podanie parametru ttb wy³±cza wcze¶niej ustawione 
	banowanie.

BOT_HELP_KICK_UNDERLINES
	Sk³adnia: KICK kana³ UNDERLINES {ON|OFF} [ttb]

	Ustawia kopanie za u¿ywanie podkre¶lenia tekstu.

	ttb to numer który okre¶la ile razy u¿ytkownik 
	stosuj±cy podkre¶lenie tekstu mo¿e byæ kopniêty 
	zanim ustawiony zostanie ban. Nie podanie parametru 
	ttb wy³±cza wcze¶niej ustawione banowanie.

BOT_HELP_KICK_CAPS
	Sk³adnia: KICK kana³ CAPS {ON|OFF} [ttb [min [procent]]]

	Ustawia kopanie za u¿ywanie WIELKICH LITER.

	Bot kopie je¶li tekst zawiera przynajmniej min
	wielkich liter co stanowi conajmniej procent
	ca³ego tekstu w linii (w przypadku nie podania tych
	warto¶ci przyjmowane s± standardowe: 10 znaków 
	i 25%).
	
	ttb to numer który okre¶la ile razy u¿ytkownik 
	stosuj±cy wielkie litery mo¿e byæ kopniêty zanim 
	ustawiony zostanie ban. Nie podanie parametru ttb 
	wy³±cza wcze¶niej ustawione banowanie.

BOT_HELP_KICK_FLOOD
	Sk³adnia: KICK kana³ FLOOD {ON|OFF} [ttb [ln [sek]]]

	Ustawia kopanie za flood. W³±czenie tej opcji 
	powoduje wykopywanie u¿ytkowników flooduj±cych, tj 
	wysy³aj±cych na kana³ conajmniej ln linii tekstu 
	w ci±gu seksekund (w przypadku nie podania tych warto¶ci przyjmowane 
	s± standardowe: 6 linii w ci±gu 10 sekund).
	
	ttb to numer który okre¶la ile razy u¿ytkownik 
	flooduj±cy mo¿e byæ kopniêty zanim ustawiony 
	zostanie ban. Nie podanie parametru ttb wy³±cza 
	wcze¶niej ustawione banowanie.

BOT_HELP_KICK_REPEAT
	Sk³adnia: KICK kana³ REPEAT {ON|OFF} [ttb [num]]

	Ustawia kopanie za powtarzanie tekstu. W³±czenie tej 
	opcji powoduje wykopywanie u¿ytkowników 
	powtarzaj±cych tj wysy³aj±cych na kana³ conajmniej 
	num razy ten sam tekst (w przypadku nie podania 
	tego parametru jego warto¶c ustalana jest na 3).

	ttb to numer który okre¶la ile razy u¿ytkownik 
	powtarzaj±cy tekst mo¿e byæ kopniêty zanim ustawiony 
	zostanie ban. Nie podanie parametru ttb wy³±cza 
	wcze¶niej ustawione banowanie.

BOT_HELP_KICK_BADWORDS
	Sk³adnia: KICK kana³ BADWORDS {ON|OFF} [ttb]

	Ustawia kopanie za u¿ywanie zakazanych s³ów.
	
	Mo¿esz ustaliæ listê zakazanych s³ów za pomoc±
	polecenia BADWORDS. Napisz 
	/msg %S HELP BADWORDS,aby 
	uzyskaæ wiêcej informacji.

	ttb to numer który okre¶la ile razy u¿ytkownik 
	u¿ywaj±cy zakazanych s³ów mo¿e byæ kopniêty zanim 
	ustawiony zostanie ban. Nie podanie parametru ttb 
	wy³±cza wcze¶niej ustawione banowanie.
	
BOT_HELP_BADWORDS
	Sk³adnia: BADWORDS kana³ ADD s³owo [SINGLE | START | END]
	          BADWORDS kana³ DEL {s³owo | pozycja | lista}
	          BADWORDS kana³ LIST [maska | lista]
	          BADWORDS kana³ CLEAR
	
	Polecenie umo¿liwia zarz±dzanie list± niedozwolonych
	s³ów. Lista definiuja za u¿ywanie których s³ów 
	u¿ytkownicy mog± zostaæ wyrzuceni z kana³u. Aby 
	uzyskaæ wiêcej informacji napisz: 
	/msg %S HELP KICK BADWORDS
	
	Komenda BADWORDS ADD umo¿liwia dodanie 
	wskazanego s³owa do listy. Parametr SIGLE powoduje, 
	¿e u¿ytkownicy bêd± kopani za u¿ywanie wskazanego 
	s³owa w ca³o¶ci. 
	Parametr START - kopanie za u¿ywanie wskazanego 
	s³owa na pocz±tku innych wyrazów. END powoduje 
	kopanie za u¿ywanie wskazanego s³owa na koñcu 
	wyrazów. Nie podanie parametru spowoduje kopniêcie 
	u¿ytkownika w ka¿dym z przypadków kiedy zostanie 
	u¿yte wskazane s³owo.
	
	Polecenie BADWORDS DEL usuwa wskazane s³owo lub
	s³owa z listy (dopuszczalne jest podanie numeru 
	s³owa, lub grupy s³ów, przyk³ad poni¿ej).
	
	Polecenie BADWORDS LIST wy¶wietla listê 
	zakazanych s³ów. Je¶li zostanie podana maska, 
	wy¶wietlone zostan± jedynie wpisy pasuj±ce do wzorca 
	(dopuszczalne jest podanie numeru s³owa, lub grupy 
	s³ów, przyk³ad poni¿ej).

	Przyk³ad:
	   BADWORDS kana³ LIST 2-5,7-9
	      Listuje s³owa niedozwolone od numeru 2 do 5
	      oraz od 7 do 9.
	      
	Polecenie BADWORDS CLEAR usuwa wszystkie wpisy z 
	listy zakazanych s³ów.

BOT_HELP_SAY
	Sk³adnia: SAY kana³ tekst
	
	Wymusza wypowiedzenie podanego tekstu przez bota na 
	podanym kanale.

BOT_HELP_ACT
	Sk³adnia: ACT kana³ tekst
	
	Sprawia ¿e Bot u¿yje komendy akcji (/me) przy 
	wypowiedzeniu podanego tekstu na kanale.

BOT_SERVADMIN_HELP_BOT
	Sk³adnia: BOT ADD nick ident host nazwa
	          BOT CHANGE starynick nowynick [ident [host [nazwa]]]
	          BOT DEL nick
	
	Polecenie pozwala Administratorom Serwisów na 
	tworzenie, modyfikowanie oraz kasowanie botów 
	u¿ywanych przez u¿ytkowników sieci na kana³ach.
	
	BOT ADD dodaje bota o podanym nicku, idencie, 
	ho¶cie oraz realname. 
	BOT CHANGE pozwala na zmiane nicka, identa, 
	hosta lub realname bota bez jego kasowania. 
	BOT DEL usuwa wskazanego bota. 
	
	UWAGA: Je¶li stworzony zostanie bot o nicku, 
	który jest ju¿ zarejestrowany, nick zostanie 
	odrejestrowany. Podobnie je¶li nick stworzonego 
	bota bêdzie u¿ywany przez innego u¿ytkownika, 
	zostanie on wtedy skillowany.

BOT_SERVADMIN_HELP_SET
	
	Poni¿sze opcje s± dostêpne dla Administratorów 
	Serwisów:
	
	   NOBOT     Uniemo¿liwia botowi przypisywanie go do 
	             kana³ów.
	   PRIVATE   Umo¿liwia przypisywanie bota jedynie przez 
	             IRC Operatorów.

BOT_SERVADMIN_HELP_SET_NOBOT
	Sk³adnia: SET kana³ NOBOT {ON|OFF}
	
	Polecenie to pozwala okre¶liæ kana³ do którego
	nie wolno przypisywaæ botów. Dotychczasowe 
	przypisania botów zostan± cofniête dla kana³u po 
	aktywacji tej opcji.
	
	Komenda dostêpna dla Administratorów Serwisów.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Sk³adnia: SET nick-bota PRIVATE {ON|OFF}

	Polecenie pozwala na uniemo¿liwienie przypisania
	wskazanego bota do kana³ów, przez u¿ytkowników nie
	bêd±cych IRC Operatorami.
	
	Komenda dostêpna dla Administratorów Serwisów.

###########################################################################
#
# Komunikaty HostServ 
#
###########################################################################
HOST_EMPTY
	Lista vhostów jest pusta.
HOST_ENTRY
	#%d Nick:%s, vhost:%s (%s - %s)
HOST_IDENT_ENTRY
	#%d Nick:%s, vhost:%s@%s (%s - %s)
HOST_SET
	vhost dla %s ustawiono na %s.
HOST_IDENT_SET
	vhost dla %s ustawiono na %s@%s.
HOST_SETALL
	vhost dla grupy %s ustawiono na %s.
HOST_DELALL
	vhosty dla grupy %s zosta³y usuniête.
HOST_DELALL_SYNTAX
	Sk³adnia: DELALL <nick>.
HOST_IDENT_SETALL
	vhost dla groupy %s ustawiono na 
	%s@%s.
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
	Syntax /msg %s set <nick> <hostmask>.
HOST_SETALL_SYNTAX
	Syntax /msg %s setall <nick> <hostmask>.
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
	Syntax: /msg %s del <nick>.
HOST_OFF_UNREAL
	Your vhost was removed. To re-enable the standard host cloaking, type /mode %s +%s
HOST_NO_VIDENT
	Your IRCD does not support vIdent's, if this is incorrect, please report this as a possible bug
HOST_GROUP
	All vhost's in the group %s have been set to %s
HOST_IDENT_GROUP
	All vhost's in the group %s have been set to %s@%s
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

	Limited to Host Setters.

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

	Limited to Host Setters.

HOST_HELP_OFF
	Syntax: OFF

	Deactivates the vhost currently assigned to the nick in use.
	When you use this command any user who performs a /whois
	on you will see your real IP address.

HOST_HELP_DEL
	Syntax: DEL <nick>

	Deletes the vhost assigned to the given nick from the
	database.

	Limited to HostRemovers.

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

