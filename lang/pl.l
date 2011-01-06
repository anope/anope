# Polski plik jezykowy/Polish language file.
# Tlumaczenie/Translation:
#               Armada Jaroslaw    <yoplait@yoplait.org>
#               Eskela             <eskela@gmail.com>
#               Mierzwiak Bartosz  <bartosz@mierzwiak.com>
#               Swierkosz Szymon   <szymek@adres.pl>
#               Szczyrzyca Mateusz <annihilator@ircnet.pl>
#               PolNet Team: http://www.ircnet.pl
#
# Aktualizacja/Updated: 13/10/2008
# Wersja serwisow/Anope version: svn-1477
#
# This file is encoded using iso-8859-2.
#
###########################################################################
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
# For example, "Espaï¿½l (Spanish)" or "Franï¿½is (French)".
LANG_NAME
	Polski (Polish)

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
	B³±d wewnêtrzny - nie mogê obs³u¿yæ ¿±dania.

UNKNOWN_COMMAND
	Nieznana komenda %s.

UNKNOWN_COMMAND_HELP
	Nieznana komenda %s. Napisz "%R%s HELP" aby uzyskaæ pomoc.

SYNTAX_ERROR
	Sk³adnia: %s

MORE_INFO
	Napisz %R%s HELP %s aby uzyskaæ wiêcej informacji.

NO_HELP_AVAILABLE
	Brak dostêpnej pomocy dla %s.

OBSOLETE_COMMAND
	Ta komenda jest ju¿ przestarza³a. U¿yj %s.

BAD_USERHOST_MASK
	Maska musi mieæ format ident@host.

BAD_EXPIRY_TIME
	Nieprawid³owy czas wygasania.

USERHOST_MASK_TOO_WIDE
	%s jest za ogólna; u¿yj bardziej szczegó³owej maski.

SERVICE_OFFLINE
	%s jest obecnie offline.

READ_ONLY_MODE
	Uwaga: Serwisy s± w trybie tylko do odczytu - zmiany nie bêd± zapisywane!

PASSWORD_INCORRECT
	Nieprawid³owe has³o.

INVALID_TARGET
	"/msg %s" nie jest ju¿ obs³ugiwane. U¿yj "/msg %s@%s" lub "/%s".

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SASET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Odmowa dostêpu.

PERMISSION_DENIED
	Brak uprawnieñ.

RAW_DISABLED
	Polecenie RAW zosta³o wy³±czone. Je¶li chcesz go u¿ywaæ,
	ustaw odpowiednio DisableRaw w konfiguracji serwisów.

MORE_OBSCURE_PASSWORD
	Proszê spróbowaæ ponownie z trudniejszym has³em.
	Has³o powinno mieæ minimum 5 znaków oraz powinno
	zawieraæ cyfry i litery.

PASSWORD_TOO_LONG
	Twoje has³o jest za d³ugie. Spróbuj ponownie z krótszym has³em.

NICK_NOT_REGISTERED
	Twój nick nie jest zarejestrowany.

NICK_NOT_REGISTERED_HELP
	Twój nick jest niezarejestrowany.
	Napisz %R%s HELP aby uzyskaæ pomoc na temat rejestracji.

NICK_X_IS_SERVICES
	Nick %s jest serwisem!

NICK_X_NOT_REGISTERED
	Nick %s nie jest zarejestrowany.

NICK_X_IN_USE
	Nick %s jest aktualnie w u¿yciu.

NICK_X_NOT_IN_USE
	Nick %s nie jest aktualnie u¿ywany.

NICK_X_NOT_ON_CHAN
	%s nie jest aktualnie na kanale %s.

NICK_X_FORBIDDEN
	Nick %s nie mo¿e byæ rejestrowany ani u¿ywany.

NICK_X_FORBIDDEN_OPER
	Nick %s zosta³ zablokowany przez %s:
	%s

NICK_X_ILLEGAL
	Nick %s jest nieprawid³owy i nie mo¿e byæ u¿ywany.

NICK_X_TRUNCATED
	Nick %s zosta³ skrócony do %d znaków.

NICK_X_SUSPENDED
	Nick %s jest obecnie zawieszony.

CHAN_X_NOT_REGISTERED
	Kana³ %s nie jest zarejestrowany.

CHAN_X_NOT_IN_USE
	Kana³ %s nie istnieje.

CHAN_X_FORBIDDEN
	Kana³u %s nie mo¿na rejestrowaæ ani u¿ywaæ.

CHAN_X_FORBIDDEN_OPER
	Kana³ %s zosta³ zablokowany przez %s:
	%s

CHAN_X_SUSPENDED
	     Zawieszony: [%s] %s

NICK_IDENTIFY_REQUIRED
	Aby u¿yæ tej komendy musisz siê zidentyfikowaæ.
	Napisz %R%s IDENTIFY has³o, a pó¼niej ponów próbê.

CHAN_IDENTIFY_REQUIRED
	Do u¿ycia tej komendy wymagana jest identyfikacja.
	Napisz %R%s IDENTIFY %s has³o, a pó¼niej ponów próbê.
	

MAIL_DISABLED
	Serwisy zosta³y skonfigurowane tak, aby nie wysy³aæ poczty.

MAIL_INVALID
	Adres e-mail dla %s nie jest prawid³owy.

MAIL_X_INVALID
	%s nie jest prawid³owym adresem e-mail.

MAIL_LATER
	Nie mogê wys³aæ teraz maila. Spróbuj za chwilê.

MAIL_DELAYED
	Proszê odczekaæ %d sekund(y) i ponowiæ próbê.
	

NO_REASON
	Bez powodu

UNKNOWN
	<nieznane>

# Duration system
DURATION_DAY
	1 dzieñ

DURATION_DAYS
	%d dni

DURATION_HOUR
	1 godzina

DURATION_HOURS
	%d godzin(y)

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
	wygasa za %d minutê

EXPIRES_HM
	wygasa za %d godzin(y) i %d minut(y)

EXPIRES_H1M
	wygasa za %d godzinê i %d minut(y)

EXPIRES_1HM
	wygasa za %d godzinê i %d minut(y)

EXPIRES_1H1M
	wygasa za %d godzinê i %d minutê

EXPIRES_D
	wygasa za %d dni

EXPIRES_1D
	wygasa za %d dzieñ

# Generic Footer message
END_OF_ANY_LIST
	Koniec listy %s.

# Generic List error messages
LIST_INCORRECT_RANGE
	Podano nieprawid³owy zakres. Poprawny zapis to #od-do.

CS_LIST_INCORRECT_RANGE
	Aby wyszukaæ kana³y o nazwach zaczynaj±cych siê znakiem #
	nale¿y ten znak pomin±æ (anope zamiast #anope).

# Generic help limited to messages
HELP_LIMIT_SERV_OPER
	
	Polecenie dostêpne tylko dla operatorów serwisów.

HELP_LIMIT_SERV_ADMIN
	
	Polecenie dostêpne tylko dla administratorów serwisów.

HELP_LIMIT_SERV_ROOT
	
	Polecenie dostêpne tylko dla Services Roots.

HELP_LIMIT_IRC_OPER
	
	Polecenie dostêpne tylko dla IRC operatorów.

HELP_LIMIT_HOST_SETTER
	
	Polecenie dostêpne tylko dla Host Setters.

HELP_LIMIT_HOST_REMOVER
	
	Polecenie dostêpne tylko dla Host Removers.

###########################################################################
#
# NickServ messages
#
###########################################################################
# Automatic messages
NICK_IS_REGISTERED
	Ten nick jest zarejestrowany. Je¶li to Twój nick,
	wpisz: %R%s IDENTIFY has³o

NICK_IS_SECURE
	Ten nick jest zarejestrowany i chroniony.
	Zidentyfikuj siê pisz±c: %R%s IDENTIFY has³o
	je¶li nick nale¿y do Ciebie, w przeciwnym razie zmieñ go.

NICK_MAY_NOT_BE_USED
	Ten nick nie mo¿e byæ u¿ywany. Wybierz inny nick.

FORCENICKCHANGE_IN_1_MINUTE
	Masz jedn± minutê na zmianê nicka, potem zostanie zmieniony si³±.

FORCENICKCHANGE_IN_20_SECONDS
	Masz 20 sekund na zmianê nicka, potem zostanie zmieniony si³±.

FORCENICKCHANGE_NOW
	Ten nick zosta³ zarejestrowany i nie mo¿esz go u¿ywaæ.

FORCENICKCHANGE_CHANGING
	Twój nick zosta³ zmieniony na %s.

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER has³o [email]

NICK_REGISTER_SYNTAX_EMAIL
	REGISTER has³o email

NICK_REGISTRATION_DISABLED
	Rejestracja nicków jest tymczasowo wy³±czona.

NICK_REGISTRATION_FAILED
	Rejestracja nicka nie powiod³a siê.

NICK_REG_PLEASE_WAIT
	Odczekaj %d sekund przed ponownym u¿yciem komendy REGISTER.

NICK_CANNOT_BE_REGISTERED
	Nick %s nie mo¿e byæ rejestrowany.

NICK_ALREADY_REGISTERED
	Nick %s jest ju¿ zarejestrowany!

NICK_REGISTERED
	Nick %s zosta³ zarejestrowany z mask±: %s.

NICK_REGISTERED_NO_MASK
	Nick %s zosta³ zarejestrowany.

NICK_PASSWORD_IS
	Twoje has³o do nicka to %s

NICK_REG_DELAY
	Musisz byæ po³±czony(a) d³u¿ej ni¿ %d sekund(y), aby móc zarejestrowaæ nick.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP nazwa has³o

NICK_GROUP_DISABLED
	Grupowanie nicków jest tymczasowo wy³±czone.

NICK_GROUP_FAILED
	Grupowanie nie powiod³o siê.

NICK_GROUP_PLEASE_WAIT
	Odczekaj %d sekund przed ponownym u¿yciem komendy GROUP.

NICK_GROUP_CHANGE_DISABLED
	Twój nick jest ju¿ zarejestrowany. Napisz najpierw: %R%s DROP

NICK_GROUP_SAME
	Nale¿ysz ju¿ do grupy %s.

NICK_GROUP_TOO_MANY
	W grupie %s jest za du¿o nicków, usuñ kilka.
	Napisz %R%s HELP GLIST i %R%s HELP DROP,
	aby uzyskaæ wiêcej informacji.

NICK_GROUP_JOINED
	Nale¿ysz teraz do grupy %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY has³o

NICK_IDENTIFY_FAILED
	Identyfikacja nie powiod³a siê.

NICK_IDENTIFY_SUCCEEDED
	Has³o przyjête - jeste¶ zidentyfikowany(a).

NICK_IDENTIFY_EMAIL_REQUIRED
	Teraz ustaw adres e-mail dla Twojego nicka.
	Gdy zapomnisz has³a, zostanie ono wys³ane na e-mail.

NICK_IDENTIFY_EMAIL_HOWTO
	Napisz %R%S SET EMAIL e-mail, aby ustawiæ adres e-mail.
	Twoja prywatno¶æ bêdzie uszanowana - nie bêdzie podawany osobom trzecim,

NICK_ALREADY_IDENTIFIED
	Jeste¶ ju¿ zidentyfikowany(a).

# UPDATE responses
NICK_UPDATE_SUCCESS
	Status zaktualizowany (wiadomo¶ci, vhosty, flagi).
	

# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT

NICK_LOGOUT_SUCCEEDED
	Twój nick zosta³ pomy¶lnie wylogowany.

NICK_LOGOUT_X_SUCCEEDED
	Nick %s zosta³ wylogowany.

NICK_LOGOUT_SERVICESADMIN
	Nie mogê wylogowaæ %s, bo to administrator serwisów.

# DROP responses
NICK_DROP_DISABLED
	Usuwanie nicków jest tymczasowo wy³±czone.

NICK_DROPPED
	Twój nick zosta³ odrejestrowany.

NICK_X_DROPPED
	Nick %s zosta³ odrejestrowany.

# SET responses
NICK_SET_SYNTAX
	SET opcja parametry

NICK_SET_SERVADMIN_SYNTAX
	SET [nick] opcja parametry

NICK_SET_DISABLED
	Modyfikacja ustawieñ nicka jest tymczasowo wy³±czona.

NICK_SET_UNKNOWN_OPTION
	Nieznana opcja komendy SET %s.

NICK_SET_OPTION_DISABLED
	Opcja %s nie jest dostêpna w tej sieci.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	Now± nazw± MUSI byæ jednym z nicków w grupie!

NICK_SET_DISPLAY_CHANGED
	Now± nazw± jest %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Zmiana has³a nie powiod³a siê.

NICK_SET_PASSWORD_CHANGED
	Has³o zosta³o zmienione.

NICK_SET_PASSWORD_CHANGED_TO
	Has³o zosta³o zmienione na %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE numer

NICK_SET_LANGUAGE_UNKNOWN
	Nieznany numer jêzyka %d.
	Napisz %R%s HELP SET LANGUAGE, aby uzyskaæ listê dostêpnych jêzyków.

NICK_SET_LANGUAGE_CHANGED
	Jêzyk zosta³ zmieniony na Polski.

# SET URL responses
NICK_SET_URL_CHANGED
	Adres www zosta³ zmieniony na %s.

NICK_SET_URL_UNSET
	Adres www zosta³ usuniêty.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	Adres e-mail zosta³ zmieniony na %s.

NICK_SET_EMAIL_UNSET
	Adres e-mail zosta³ usuniêty.

NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	Nie mo¿esz usuwaæ adresu e-mail w tej sieci.
	

# SET ICQ responses
NICK_SET_ICQ_CHANGED
	Numer ICQ zosta³ zmieniony na %s.

NICK_SET_ICQ_UNSET
	Numer ICQ zosta³ usuniêty.

NICK_SET_ICQ_INVALID
	%s nie jest prawid³owym numerem.
	

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
	Ochrona jest teraz w³±czona.

NICK_SET_KILL_QUICK
	Ochrona ze zredukowanym opó¼nieniem jest teraz w³±czona.

NICK_SET_KILL_IMMED
	Natychmiastowa ochrona jest teraz w³±czona.

NICK_SET_KILL_IMMED_DISABLED
	Opcja IMMED nie jest jest dostêpna w tej sieci.

NICK_SET_KILL_OFF
	Ochrona jest teraz wy³±czona.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}

NICK_SET_SECURE_ON
	Opcja bezpieczeñstwa zosta³a w³±czona.

NICK_SET_SECURE_OFF
	Opcja bezpieczeñstwa zosta³a wy³±czona.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}

NICK_SET_PRIVATE_ON
	Opcja prywatno¶ci zosta³a w³±czona.

NICK_SET_PRIVATE_OFF
	Opcja prywatno¶ci zosta³a wy³±czona.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}

NICK_SET_HIDE_EMAIL_ON
	Twój adres e-mail bêdzie ukrywany w informacjach %s.

NICK_SET_HIDE_EMAIL_OFF
	Twój adres e-mail bêdzie pokazywany w informacjach %s.

NICK_SET_HIDE_MASK_ON
	Twoja ostatnia maska ident@host bêdzie ukrywana w informacjach %s.

NICK_SET_HIDE_MASK_OFF
	Twoja ostatnia maska ident@host bêdzie pokazywana w informacjach %s.

NICK_SET_HIDE_QUIT_ON
	Twoja ostatnia wiadomo¶æ QUIT bêdzie ukrywana w informacjach %s.

NICK_SET_HIDE_QUIT_OFF
	Twoja ostatnia wiadomo¶æ QUIT bêdzie pokazywana w informacjach %s.

NICK_SET_HIDE_STATUS_ON
	Twój status dostêpu do serwisów bêdzie ukrywany w informacjach %s.

NICK_SET_HIDE_STATUS_OFF
	Twój status dostêpu do serwisów bêdzie pokazywany w informacjach %s.
	

# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET MSG {ON | OFF}

NICK_SET_MSG_ON
	Serwisy bêd± teraz wysy³aæ wiadomo¶ci.

NICK_SET_MSG_OFF
	Serwisy bêd± teraz wysy³aæ powiadomienia.

# SET AUTOOP responses
NICK_SET_AUTOOP_SYNTAX
	SET AUTOOP {ON | OFF}

NICK_SET_AUTOOP_ON
	Serwisy bêd± automatycznie nadawa³y Tobie uprawnienia.

NICK_SET_AUTOOP_OFF
	Serwisy nie bêd± ju¿ automatycznie nadawa³y Tobie uprawnieñ.

# SASET responses
NICK_SASET_SYNTAX
	SASET nick opcja parametry

NICK_SASET_DISABLED
	Modyfikacja ustawieñ nicka jest tymczasowo wy³±czona.

NICK_SASET_UNKNOWN_OPTION
	Nieznana opcja komendy SASET %s.

NICK_SASET_BAD_NICK
	Nick %s nie jest zarejestrowany.

NICK_SASET_OPTION_DISABLED
	Opcja %s nie jest dostêpna w tej sieci.

# SASET DISPLAY responses
NICK_SASET_DISPLAY_INVALID
	Nowa nazwa dla %s MUSI byæ jednym z nicków w grupie!

NICK_SASET_DISPLAY_CHANGED
	Now± nazw± jest %s.

# SASET PASSWORD responses
NICK_SASET_PASSWORD_FAILED
	Zmiana has³a dla %s nie powiod³a siê.

NICK_SASET_PASSWORD_CHANGED
	Has³o dla %s zosta³o zmienione.

NICK_SASET_PASSWORD_CHANGED_TO
	Has³o dla %s zosta³o zmienione na %s.

# SASET URL responses
NICK_SASET_URL_CHANGED
	Adres www osoby %s zosta³ zmieniony na %s.

NICK_SASET_URL_UNSET
	Adres www osoby %s zosta³ usuniêty.

# SASET EMAIL responses
NICK_SASET_EMAIL_CHANGED
	Adres e-mail osoby %s zosta³ zmieniony na %s.

NICK_SASET_EMAIL_UNSET
	Adres e-mail osoby %s zosta³ usuniêty.

NICK_SASET_EMAIL_UNSET_IMPOSSIBLE
	Nie mo¿esz usuwaæ adresu e-mail w tej sieci.
	

# SASET ICQ responses
NICK_SASET_ICQ_CHANGED
	Numer ICQ osoby %s zosta³ zmieniony na %s.

NICK_SASET_ICQ_UNSET
	Numer ICQ osoby %s zosta³ usuniêty.

NICK_SASET_ICQ_INVALID
	%s nie jest prawid³owym numerem.
	

# SASET GREET responses
NICK_SASET_GREET_CHANGED
	Wiadomo¶æ powitalna osoby %s zosta³a zmieniona na %s.

NICK_SASET_GREET_UNSET
	Wiadomo¶æ powitalna osoby %s zosta³a usuniêta.

# SASET PROTECT responses
NICK_SASET_KILL_SYNTAX
	SASET nick KILL {ON | QUICK | OFF}

NICK_SASET_KILL_IMMED_SYNTAX
	SASET nick KILL {ON | QUICK | IMMED | OFF}

NICK_SASET_KILL_ON
	Ochrona dla %s jest teraz w³±czona.

NICK_SASET_KILL_QUICK
	Ochrona dla %s ze zredukowanym opó¼nieniem jest teraz w³±czona.

NICK_SASET_KILL_IMMED
	Natychmiastowa ochrona dla %s jest teraz w³±czona

NICK_SASET_KILL_IMMED_DISABLED
	Opcja IMMED nie jest dostêpna w tej sieci.

NICK_SASET_KILL_OFF
	Ochrona dla %s jest teraz wy³±czona.

# SASET SECURE responses
NICK_SASET_SECURE_SYNTAX
	SASET nick SECURE {ON | OFF}

NICK_SASET_SECURE_ON
	Opcja bezpieczeñstwa dla %s zosta³a w³±czona.

NICK_SASET_SECURE_OFF
	Opcja bezpieczeñstwa dla %s zosta³a wy³±czona.

# SASET PRIVATE responses
NICK_SASET_PRIVATE_SYNTAX
	SASET nick PRIVATE {ON | OFF}

NICK_SASET_PRIVATE_ON
	Opcja prywatno¶ci dla %s zosta³a w³±czona.

NICK_SASET_PRIVATE_OFF
	Opcja prywatno¶ci dla %s zosta³a wy³±czona.

# SASET HIDE responses
NICK_SASET_HIDE_SYNTAX
	SASET nick HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}

NICK_SASET_HIDE_EMAIL_ON
	Adres e-mail osoby %s bêdzie ukrywany w informacjach %s.

NICK_SASET_HIDE_EMAIL_OFF
	Adres e-mail osoby %s bêdzie pokazywany w informacjach %s.

NICK_SASET_HIDE_MASK_ON
	Ostatnia maska ident@host osoby %s bêdzie ukrywana w informacjach %s.

NICK_SASET_HIDE_MASK_OFF
	Ostatnia maska ident@host osoby %s bêdzie pokazywana w informacjach %s.

NICK_SASET_HIDE_QUIT_ON
	Ostatnia wiadomo¶æ QUIT osoby %s bêdzie ukrywana w informacjach %s.

NICK_SASET_HIDE_QUIT_OFF
	Ostatnia wiadomo¶æ QUIT osoby %s bêdzie pokazywana w informacjach %s.

NICK_SASET_HIDE_STATUS_ON
	Status dostêpu do serwisów osoby %s bêdzie ukrywany w informacjach %s.

NICK_SASET_HIDE_STATUS_OFF
	Status dostêpu do serwisów osoby %s bêdzie pokazywany w informacjach %s.
	

# SASET MSG responses
NICK_SASET_MSG_SYNTAX
	SASAET nick MSG {ON | OFF}

NICK_SASET_MSG_ON
	Serwisy bêd± teraz wysy³aæ do %s wiadomo¶ci.

NICK_SASET_MSG_OFF
	Serwisy bêd± teraz wysy³aæ do %s powiadomienia.

# SASET NOEXPIRE responses
NICK_SASET_NOEXPIRE_SYNTAX
	SASET nick NOEXPIRE {ON | OFF}

NICK_SASET_NOEXPIRE_ON
	Nick %s nie wyga¶nie.

NICK_SASET_NOEXPIRE_OFF
	Nick %s od teraz podlega zasadom wygasania.

# SASET AUTOOP responses
NICK_SASET_AUTOOP_SYNTAX
	SASET nick AUTOOP {ON | OFF}

NICK_SASET_AUTOOP_ON
	Serwisy bêd± automatycznie nadawa³y uprawnienia dla u¿ytkownika %s.

NICK_SASET_AUTOOP_OFF
	Serwisy nie bêd± ju¿ automatycznie nadawa³y uprawnieñ dla u¿ytkownika %s.

# SASET LANGUAGE responses
NICK_SASET_LANGUAGE_SYNTAX
	SASET nick LANGUAGE numer

NICK_SASET_LANGUAGE_UNKNOWN
	Nieznany numer jêzyka %d.
	Napisz %R%s HELP SET LANGUAGE, aby uzyskaæ listê dostêpnych jêzyków.

NICK_SASET_LANGUAGE_CHANGED
	Language for %s changed to %s.

# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [maska]

NICK_ACCESS_ALREADY_PRESENT
	Maska %s jest ju¿ w Twojej li¶cie dostêpu.

NICK_ACCESS_REACHED_LIMIT
	Mo¿esz mieæ tylko %d wpisy(ów) na li¶cie dostêpu.

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
	Twoja lista dostêpu jest pusta.

NICK_ACCESS_LIST_X_EMPTY
	Lista dostêpu dla %s jest pusta.

# Status messages
NICK_STATUS_0
	STATUS %s 0 (nie ma lub nie zarejestrowany)

NICK_STATUS_1
	STATUS %s 1 (nie jest zidentyfikowany)

NICK_STATUS_2
	STATUS %s 2 (identyfikacja list± dostêpu)

NICK_STATUS_3
	STATUS %s 3 (identyfikacja has³em)

# INFO responses
NICK_INFO_SYNTAX
	INFO nick [ALL]

NICK_INFO_REALNAME
	%s to %s

NICK_INFO_SERVICES_OPER
	%s jest operatorem serwisów.

NICK_INFO_SERVICES_ADMIN
	%s jest administratorem serwisów.

NICK_INFO_SERVICES_ROOT
	%s jest g³ównym administratorem serwisów.

NICK_INFO_ADDRESS
	  Ostatni host: %s

NICK_INFO_ADDRESS_ONLINE
	 Jest online z: %s

NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s jest teraz online.

NICK_INFO_TIME_REGGED
	Zarejestrowano: %s

NICK_INFO_LAST_SEEN
	      Widziano: %s

NICK_INFO_LAST_QUIT
	  Ostatni QUIT: %s

NICK_INFO_URL
	     Adres www: %s

NICK_INFO_EMAIL
	  Adres e-mail: %s

NICK_INFO_VHOST
	         vhost: %s

NICK_INFO_VHOST2
	         vhost: %s@%s

NICK_INFO_ICQ
	     Numer ICQ: %d

NICK_INFO_GREET
	     Powitanie: %s

NICK_INFO_OPTIONS
	         Opcje: %s

NICK_INFO_EXPIRE
	        Wygasa: %s

# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	KILL

NICK_INFO_OPT_SECURE
	SECURE

NICK_INFO_OPT_PRIVATE
	PRIVATE

NICK_INFO_OPT_MSG
	MSG

NICK_INFO_OPT_AUTOOP
	AUTOOP

NICK_INFO_OPT_NONE
	Brak

NICK_INFO_NO_EXPIRE
	Ten nick nie wyga¶nie.

NICK_INFO_FOR_MORE
	Aby uzyskaæ wiêcej informacji napisz %R%s INFO %s ALL.

NICK_INFO_SUSPENDED
	Ten nick jest obecnie zawieszony, powód: %s

NICK_INFO_SUSPENDED_NO_REASON
	Ten nick jest obecnie zawieszony

# LIST responses
NICK_LIST_SYNTAX
	LIST wzorzec

NICK_LIST_SERVADMIN_SYNTAX
	LIST wzorzec [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]

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
	Lista nicków w grupie:

NICK_GLIST_HEADER_X
	Lista nicków grupy %s:

NICK_GLIST_FOOTER
	%d nicków w grupie.

NICK_GLIST_REPLY
	   %c%s

NICK_GLIST_REPLY_ADMIN
	   %c%s (wygasa za %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nick [has³o]

NICK_NO_RECOVER_SELF
	Nie mo¿esz odzyskaæ nicka, którego w³a¶nie u¿ywasz!

NICK_RECOVERED
	Twój nick zosta³ w³a¶nie przejêty przez serwisy.
	Napisz %R%s RELEASE %s, je¶li nie chcesz czekaæ %s sekund na jego automatyczne zwolnienie.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nick [has³o]

NICK_RELEASE_NOT_HELD
	Nick %s nie jest zajêty przez serwisy.

NICK_RELEASED
	Serwisy w³a¶nie zwolni³y twojego nicka.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nick [has³o]

NICK_NO_GHOST_SELF
	Nie mo¿esz usun±æ samego siebie!

NICK_GHOST_KILLED
	Nick zosta³ usuniêty z sieci.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nick

NICK_GETPASS_UNAVAILABLE
	Komenda GETPASS nie dzia³a, bo has³a s± szyfrowane.

NICK_GETPASS_PASSWORD_IS
	Has³o dla %s to %s.

# GETEMAIL responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host Bez symboli wieloznacznych!

NICK_GETEMAIL_EMAILS_ARE
	Adresy e-mail %s pasuj±ce do %s.

NICK_GETEMAIL_NOT_USED
	Nie ma u¿ytkowników z adresem e-mail %s.
	

# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS nick

NICK_SENDPASS_UNAVAILABLE
	Komenda SENDPASS nie dzia³a, bo has³a s± szyfrowane.

NICK_SENDPASS_SUBJECT
	Has³o do nicka %s

NICK_SENDPASS_HEAD
	Witaj!

NICK_SENDPASS_LINE_1
	Za¿±da³e¶(a¶) wys³ania has³a do nicka %s na e-mail.

NICK_SENDPASS_LINE_2
	Twoje has³o to: %s
	Dla w³asnego bezpieczeñstwa nale¿y to has³o natychmiast zmieniæ na inne.

NICK_SENDPASS_LINE_3
	Je¶li nie wiesz dlaczego ta wiadomo¶æ dotar³a do Ciebie zignoruj j±. Je¶li ta sytuacja bêdzie siê powtarzaæ skontaktuj siê z administratorem sieci.

NICK_SENDPASS_LINE_4
	PROSZÊ NIE ODPOWIADAÆ NA T¡ WIADOMO¦Æ!

NICK_SENDPASS_LINE_5
	Administratorzy sieci %s.

NICK_SENDPASS_OK
	Has³o do nicka %s zosta³o wys³ane.

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND nick powód

NICK_SUSPEND_SUCCEEDED
	Nick %s zosta³ zawieszony.

NICK_SUSPEND_FAILED
	Zawieszenie nicka %s nie powiod³o siê!

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND nick

NICK_UNSUSPEND_SUCCEEDED
	Nick %s zosta³ uwolniony.

NICK_UNSUSPEND_FAILED
	Nie mogê uwolniæ nicka %s!

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID nick [powód]

NICK_FORBID_SYNTAX_REASON
	FORBID nick powód

NICK_FORBID_SUCCEEDED
	Nick %s zosta³ zablokowany.

NICK_FORBID_FAILED
	Nie mogê zablokowaæ nicka %s!

# Nick Registraion responses
NICK_REQUESTED
	Ten nick zosta³ ju¿ zarejestrowany. Sprawd¼ swoj± pocztê.

NICK_REG_RESENT
	Twój kod rejestracyjny zosta³ ponownie wys³any na %s.

NICK_REG_UNABLE
	Nick nie zosta³ zarejestrowany. Proszê spróbowaæ pó¼niej.

NICK_IS_PREREG
	Ten nick czeka na e-mail z kodem weryfikacyjnym przed zakoñczeniem rejestracji.

NICK_ENTER_REG_CODE
	Kod zosta³ wys³any na adres %s.
	Napisz %R%s CONFIRM <kod>, aby dokoñczyæ procedurê rejestracji.

NICK_CONFIRM_NOT_FOUND
	Termin wa¿no¶ci kodu rejestracyjnego up³yn±³.
	Napisz: %R%s REGISTER <has³o> <email>,
	aby znowu zacz±æ rejestracje.

NICK_CONFIRM_INVALID
	Podano nieprawid³owy kod, sprawd¼ wiadomo¶æ e-mail i spróbowaæ ponownie.

NICK_REG_MAIL_SUBJECT
	Rejestracja nicka %s

NICK_REG_MAIL_HEAD
	Witaj!

NICK_REG_MAIL_LINE_1
	Chcesz zarejestrowaæ nastêpuj±cy nick: %s.

NICK_REG_MAIL_LINE_2
	Proszê napisaæ %R%s CONFIRM %s aby dokoñczyæ rejestracjê.

NICK_REG_MAIL_LINE_3
	Je¶li nie wiesz dlaczego ta wiadomo¶æ dotar³a do Ciebie zignoruj j±. Je¶li sytuacja bêdzie siê powtarzaæ skontaktuj siê z administratorem sieci.
NICK_REG_MAIL_LINE_4
	PROSZÊ NIE ODPOWIADAÆ NA T¡ WIADOMO¦Æ!

NICK_REG_MAIL_LINE_5
	Administratorzy sieci %s.

NICK_GETPASS_PASSCODE_IS
	Kod rejestracyjny dla %s to %s.

NICK_FORCE_REG
	Nick %s potwierdzony.

###########################################################################
#
# ChanServ messages
#
###########################################################################
# Access level descriptions
CHAN_LEVEL_AUTOOP
	Automatyczne nadawanie statusu operatora (+o)

CHAN_LEVEL_AUTOVOICE
	Automatyczne nadawanie prawa g³osu (+v)

CHAN_LEVEL_AUTOHALFOP
	Automatyczne nadawanie statusu pó³-operatora (+h)

CHAN_LEVEL_AUTOPROTECT
	Automatyczne nadawanie ochrony (+a)

CHAN_LEVEL_AUTODEOP
	Automatyczne odbieranie statusu operatora (-o)

CHAN_LEVEL_NOJOIN
	Nie pozwala wej¶æ na kana³ w trybie RESTRICTED

CHAN_LEVEL_INVITE
	Pozwala u¿ywaæ komendy INVITE

CHAN_LEVEL_AKICK
	Pozwala u¿ywaæ komendy AKICK

CHAN_LEVEL_SET
	Pozwala modyfikowaæ ustawienia kana³u

CHAN_LEVEL_CLEAR
	Pozwala u¿ywaæ komendy CLEAR

CHAN_LEVEL_UNBAN
	Pozwala u¿ywaæ komendy UNBAN

CHAN_LEVEL_OPDEOP
	Pozwala u¿ywaæ komend OP i DEOP na innych osobach

CHAN_LEVEL_ACCESS_LIST
	Pozwala przegl±daæ listê dostêpu

CHAN_LEVEL_ACCESS_CHANGE
	Pozwala modyfikowaæ listê dostêpu

CHAN_LEVEL_MEMO
	Pozwala listowaæ i czytaæ kana³owe wiadomo¶ci

CHAN_LEVEL_ASSIGN
	Pozwala przydzielaæ i usuwaæ bota

CHAN_LEVEL_BADWORDS
	Pozwala u¿ywaæ komendy BADWORDS

CHAN_LEVEL_NOKICK
	Ochrona przed kopaniem za pomoc± komendy KICK

CHAN_LEVEL_FANTASIA
	Pozwala u¿ywaæ publicznych komend FANTASY

CHAN_LEVEL_SAY
	Pozwala u¿ywaæ komend SAY i ACT

CHAN_LEVEL_GREET
	Wy¶wietla wiadomo¶æ powitaln± (greet)

CHAN_LEVEL_VOICEME
	Pozwala u¿ywaæ komend VOICE i DEVOICE na sobie

CHAN_LEVEL_VOICE
	Pozwala u¿ywaæ komend VOICE i DEVOICE na innych osobach

CHAN_LEVEL_GETKEY
	Pozwala u¿ywaæ komendy GETKEY

CHAN_LEVEL_OPDEOPME
	Pozwala u¿ywaæ komend OP i DEOP na sobie

CHAN_LEVEL_HALFOPME
	Pozwala u¿ywaæ komend HALFOP i DEHALFOP na sobie

CHAN_LEVEL_HALFOP
	Pozwala u¿ywaæ komend HALFOP i DEHALFOP na innych osobach

CHAN_LEVEL_PROTECTME
	Pozwala u¿ywaæ komend PROTECT i DEPROTECT na sobie

CHAN_LEVEL_PROTECT
	Pozwala u¿ywaæ komend PROTECT i DEPROTECT na innych osobach

CHAN_LEVEL_KICKME
	Pozwala u¿ywaæ komendy KICK na sobie

CHAN_LEVEL_KICK
	Pozwala u¿ywaæ komendy KICK na innych osobach

CHAN_LEVEL_SIGNKICK
	Nie podpisze kicka, gdy SIGNKICK LEVEL jest u¿ywany

CHAN_LEVEL_BANME
	Pozwala u¿ywaæ komendy BAN na sobie

CHAN_LEVEL_BAN
	Pozwala u¿ywaæ komendy BAN na innych osobach

CHAN_LEVEL_TOPIC
	Pozwala u¿ywaæ komendy TOPIC

CHAN_LEVEL_INFO
	Pozwala u¿ywaæ opcji ALL w komendzie INFO

# Automatic responses
CHAN_IS_REGISTERED
	Ten kana³ zosta³ zarejestrowany z %s.

CHAN_NOT_ALLOWED_OP
	Nie masz uprawnieñ do statusu operatora na kanale %s.

CHAN_MAY_NOT_BE_USED
	Ten kana³ nie mo¿e byæ u¿ywany.

CHAN_NOT_ALLOWED_TO_JOIN
	Nie masz pozwolenia, aby przebywaæ na tym kanale.

CHAN_X_INVALID
	%s nie jest prawid³owym kana³em.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER kana³ has³o opis

CHAN_REGISTER_DISABLED
	Rejestracja kana³ów jest tymczasowo wy³±czona.

CHAN_REGISTER_NOT_LOCAL
	Lokalne kana³y nie mog± byæ rejestrowane.

CHAN_MUST_REGISTER_NICK
	Musisz mieæ zarejestrowany nick. Napisz %R%s HELP, aby uzyskaæ	wiêcej informacji.

CHAN_MUST_IDENTIFY_NICK
	Zidentyfikuj siê najpierw u %s komend±:
	%R%s IDENTIFY has³o

CHAN_MAY_NOT_BE_REGISTERED
	Kana³u %s nie mo¿na rejestrowaæ.

CHAN_ALREADY_REGISTERED
	Kana³ %s jest ju¿ zarejestrowany!

CHAN_MUST_BE_CHANOP
	Aby zarejestrowaæ kana³ musisz byæ jego operatorem.

CHAN_REACHED_CHANNEL_LIMIT
	B³±d, ju¿ masz osi±gniêty limit %d kana³ów.

CHAN_EXCEEDED_CHANNEL_LIMIT
	B³±d, ju¿ masz przekroczony limit %d kana³ów.

CHAN_REGISTRATION_FAILED
	Rejestracja nie powiod³a siê.

CHAN_REGISTERED
	Kana³ %s zosta³ teraz zarejestrowany.
	Jako %s jeste¶ od teraz jego w³a¶cicielem.

CHAN_PASSWORD_IS
	Has³o do Twojego kana³u to %s

CHAN_REGISTER_NONE_CHANNEL
	Próbujesz rejestrowaæ nieistniej±cy kana³ %s

CHAN_SYMBOL_REQUIRED
	U¿yj symboli # lub & podczas rejestracji

# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY kana³ has³o

CHAN_IDENTIFY_FAILED
	Identyfikacja nie powiod³a siê.

CHAN_IDENTIFY_SUCCEEDED
	Has³o przyjête - masz uprawnienia w³a¶ciciela kana³u %s.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT kana³ nick

CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT kana³ [nick]

CHAN_LOGOUT_SUCCEEDED
	U¿ytkownik %s zosta³ wylogowany z kana³u %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	Wylogowano zidentyfikowanych u¿ytkowników z kana³u %s (bez w³a¶ciciela).
CHAN_LOGOUT_FOUNDER_FAILED
	Nie mo¿esz siê wylogowaæ z %s (jeste¶ w³a¶cicielem).

# DROP responses
CHAN_DROP_SYNTAX
	DROP kana³

CHAN_DROP_DISABLED
	Usuwanie kana³ów jest tymczasowo wy³±czone.

CHAN_DROPPED
	Kana³ %s zosta³ odrejestrowany.

# SET responses
CHAN_SET_SYNTAX
	SET kana³ opcja parametry

CHAN_SET_DISABLED
	Modyfikacja ustawieñ kana³u jest tymczasowo wy³±czona.

CHAN_SET_UNKNOWN_OPTION
	Nieznana opcja komendy SET: %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s posiada za du¿o zarejestrowanych kana³ów.

CHAN_FOUNDER_CHANGED
	W³a¶cicielem kana³u %s jest teraz %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Zastêpcê dla kana³u %s zmieniono na %s.

CHAN_SUCCESSOR_UNSET
	Zastêpcê dla kana³u %s usuniêto.

CHAN_SUCCESSOR_IS_FOUNDER
	%s nie mo¿e byæ zastêpc± na kanale %s, poniewa¿ jest jego w³a¶cicielem.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Zmiana has³a nie powiod³a siê.

CHAN_PASSWORD_CHANGED
	Has³o do kana³u %s zosta³o zmienione.

CHAN_PASSWORD_CHANGED_TO
	Has³o do kana³u %s zosta³o zmienione na %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Opis kana³u %s zosta³ zmieniony na %s.

# SET URL responses
CHAN_URL_CHANGED
	Adres www dla kana³u %s zosta³ zmieniony na %s.

CHAN_URL_UNSET
	Adres www dla kana³u %s zosta³ usuniêty.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	Adres e-mail kana³u %s zosta³ zmieniony na %s.

CHAN_EMAIL_UNSET
	Adres e-mail kana³u %s zosta³ usuniêty.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Wiadomo¶æ powitalna kana³u %s zosta³a zmieniona.

CHAN_ENTRY_MSG_UNSET
	Wiadomo¶æ powitalna kana³u %s zosta³a usuniêta.
	

# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s nie jest prawid³owym rodzajem bana.

CHAN_SET_BANTYPE_CHANGED
	Rodzajem bana dla kana³u %s jest teraz #%d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Nieznany flaga %c zostaje zignorowana.

CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Flaga %c zosta³a zignorowana, bo nie mo¿na jej blokowaæ.

CHAN_SET_MLOCK_L_REQUIRED
	Musisz za³o¿yæ +l zanim za³o¿ysz +L.

CHAN_SET_MLOCK_K_REQUIRED
	Musisz za³o¿yæ +i równie¿, aby móc za³o¿yæ +K.

CHAN_MLOCK_CHANGED
	Blokada trybów kana³u %s zosta³a zmieniona na: %s

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET kana³ KEEPTOPIC {ON | OFF}

CHAN_SET_KEEPTOPIC_ON
	Temat kana³u %s bêdzie teraz utrzymywany.

CHAN_SET_KEEPTOPIC_OFF
	Temat kana³u %s nie bêdzie ju¿ utrzymywany.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET kana³ TOPICLOCK {ON | OFF}

CHAN_SET_TOPICLOCK_ON
	Blokada tematu na kanale %s zosta³a w³±czona.

CHAN_SET_TOPICLOCK_OFF
	Blokada tematu na kanale %s zosta³a wy³±czona.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET kana³ PEACE {ON | OFF}

CHAN_SET_PEACE_ON
	Opcja PEACE dla kana³u %s zosta³a w³±czona.

CHAN_SET_PEACE_OFF
	Opcja PEACE dla kana³u %s zosta³a wy³±czona.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET kana³ PRIVATE {ON | OFF}

CHAN_SET_PRIVATE_ON
	Opcja PRIVATE dla kana³u %s zosta³a w³±czona.

CHAN_SET_PRIVATE_OFF
	Opcja PRIVATE dla kana³u %s zosta³a wy³±czona.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET kana³ SECUREOPS {ON | OFF}

CHAN_SET_SECUREOPS_ON
	Status operatora kana³u %s bêdzie teraz chroniony.

CHAN_SET_SECUREOPS_OFF
	Status operatora kana³u %s nie bêdzie ju¿ chroniony.

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET kana³ SECUREFOUNDER {ON | OFF}

CHAN_SET_SECUREFOUNDER_ON
	Status w³a¶ciciela kana³u %s bêdzie teraz chroniony.

CHAN_SET_SECUREFOUNDER_OFF
	Status w³a¶ciciela kana³u %s nie bêdzie ju¿ chroniony.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET kana³ RESTRICTED {ON | OFF}

CHAN_SET_RESTRICTED_ON
	Dostêp do kana³u %s jest teraz ograniczony.

CHAN_SET_RESTRICTED_OFF
	Dostêp do kana³u %s nie jest ju¿ ograniczony.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET kana³ SECURE {ON | OFF}

CHAN_SET_SECURE_ON
	Opcja bezpieczeñstwa dla kana³u %s zosta³a w³±czona.

CHAN_SET_SECURE_OFF
	Opcja bezpieczeñstwa dla kana³u %s zosta³a wy³±czona.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET kana³ SIGNKICK {ON | LEVEL | OFF}

CHAN_SET_SIGNKICK_ON
	Podpisywanie komendy KICK na kanale %s zosta³o w³±czone.

CHAN_SET_SIGNKICK_LEVEL
	Podpisywanie komendy KICK na kanale %s zosta³o w³±czone, ale
	bêdzie zale¿a³o od poziomu dostêpu jaki posiada u¿ytkownik.

CHAN_SET_SIGNKICK_OFF
	Podpisywanie komendy KICK na kanale %s zosta³o wy³±czone.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET kana³ OPNOTICE {ON | OFF}

CHAN_SET_OPNOTICE_ON
	Opcja OPNOTICE dla kana³u %s zosta³a w³±czona.

CHAN_SET_OPNOTICE_OFF
	Opcja OPNOTICE dla kana³u %s zosta³a wy³±czona.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET kana³ XOP {ON | OFF}

CHAN_SET_XOP_ON
	System list xOP na kanale %s zosta³ w³±czony.

CHAN_SET_XOP_OFF
	System list xOP na kanale %s zosta³ wy³±czony.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET kana³ NOEXPIRE {ON | OFF}

CHAN_SET_NOEXPIRE_ON
	Kana³ %s nie wyga¶nie.

CHAN_SET_NOEXPIRE_OFF
	Kana³ %s od teraz podlega prawu wygasania.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Mo¿esz mieæ tylko %d wpisów na listach xOP.

CHAN_XOP_LIST_FORMAT
	  %3d  %s

CHAN_XOP_ACCESS
	Nie mo¿esz u¿yæ tej komendy. U¿yj komendy ACCESS.
	Napisz %R%s HELP ACCESS, aby uzyskaæ wiêcej informacji.

CHAN_XOP_NOT_AVAILABLE
	System xOP nie jest dostêpny.

# AOP messages
CHAN_AOP_SYNTAX
	AOP kana³ {ADD|DEL|LIST|CLEAR} [nick | pozycja]

CHAN_AOP_DISABLED
	Modyfikacja listy AOP jest tymczasowo wy³±czona.

CHAN_AOP_NICKS_ONLY
	Kana³owa lista AOP mo¿e zawieraæ jedynie zarejestrowane nicki.

CHAN_AOP_ADDED
	%s dodano do listy AOP kana³u %s.

CHAN_AOP_MOVED
	%s przeniesiono do listy AOP kana³u %s.

CHAN_AOP_NO_SUCH_ENTRY
	Brak wpisu (#%d) na li¶cie AOP kana³u %s.

CHAN_AOP_NOT_FOUND
	%s nie znaleziono na li¶cie AOP kana³u %s.

CHAN_AOP_NO_MATCH
	Nie znaleziono wpisów w li¶cie AOP kana³u %s.

CHAN_AOP_DELETED
	%s usuniêto z listy AOP kana³u %s.

CHAN_AOP_DELETED_ONE
	Usuniêto 1 wpis z listy AOP kana³u %s.

CHAN_AOP_DELETED_SEVERAL
	Usuniêto %d wpisów(y) z listy AOP kana³u %s.

CHAN_AOP_LIST_EMPTY
	Lista AOP kana³u %s jest pusta.

CHAN_AOP_LIST_HEADER
	Lista AOP kana³u %s:
	  Nr   Nick

CHAN_AOP_CLEAR
	Lista AOP kana³u %s zosta³a wyczyszczona.

# HOP messages
CHAN_HOP_SYNTAX
	HOP kana³ {ADD|DEL|LIST|CLEAR} [nick | pozycja]

CHAN_HOP_DISABLED
	Modyfikacja listy HOP jest tymczasowo wy³±czona.

CHAN_HOP_NICKS_ONLY
	Kana³owa lista HOP mo¿e zawieraæ jedynie zarejestrowane nicki.

CHAN_HOP_ADDED
	%s dodano do listy HOP kana³u %s.

CHAN_HOP_MOVED
	%s przeniesiono do listy HOP kana³u %s.

CHAN_HOP_NO_SUCH_ENTRY
	Brak wpisu (#%d) na li¶cie HOP kana³u %s.

CHAN_HOP_NOT_FOUND
	%s nie znaleziono na li¶cie HOP kana³u %s.

CHAN_HOP_NO_MATCH
	Nie znaleziono wpisów w li¶cie HOP kana³u %s.

CHAN_HOP_DELETED
	%s usuniêto z listy HOP kana³u %s.

CHAN_HOP_DELETED_ONE
	Usuniêto 1 wpis z listy HOP kana³u %s.

CHAN_HOP_DELETED_SEVERAL
	Usuniêto %d wpisów(y) z listy HOP kana³u %s.

CHAN_HOP_LIST_EMPTY
	Lista HOP kana³u %s jest pusta.

CHAN_HOP_LIST_HEADER
	Lista HOP kana³u %s:
	  Nr   Nick

CHAN_HOP_CLEAR
	Lista HOP kana³u %s zosta³a wyczyszczona.

# SOP messages
CHAN_SOP_SYNTAX
	SOP kana³ {ADD|DEL|LIST|CLEAR} [nick | pozycja]

CHAN_SOP_DISABLED
	Modyfikacja listy SOP jest tymczasowo wy³±czona.

CHAN_SOP_NICKS_ONLY
	Kana³owa lista SOP mo¿e zawieraæ jedynie zarejestrowane nicki.

CHAN_SOP_ADDED
	%s dodano do listy SOP kana³u %s.

CHAN_SOP_MOVED
	%s przeniesiono do listy SOP kana³u %s.

CHAN_SOP_NO_SUCH_ENTRY
	Brak wpisu (#%d) na li¶cie SOP kana³u %s.

CHAN_SOP_NOT_FOUND
	%s nie znaleziono na li¶cie SOP kana³u %s.

CHAN_SOP_NO_MATCH
	Nie znaleziono wpisów w li¶cie SOP kana³u %s.

CHAN_SOP_DELETED
	%s usuniêto z listy SOP kana³u %s.

CHAN_SOP_DELETED_ONE
	Usuniêto 1 wpis z listy SOP kana³u %s.

CHAN_SOP_DELETED_SEVERAL
	Usuniêto %d wpisów(y) z listy SOP kana³u %s.

CHAN_SOP_LIST_EMPTY
	Lista SOP kana³u %s jest pusta.

CHAN_SOP_LIST_HEADER
	Lista SOP kana³u %s:
	  Nr   Nick

CHAN_SOP_CLEAR
	Lista SOP kana³u %s zosta³a wyczyszczona.

# VOP messages
CHAN_VOP_SYNTAX
	VOP kana³ {ADD|DEL|LIST|CLEAR} [nick | pozycja]

CHAN_VOP_DISABLED
	Modyfikacja listy VOP jest tymczasowo wy³±czona.

CHAN_VOP_NICKS_ONLY
	Kana³owa lista VOP mo¿e zawieraæ jedynie zarejestrowane nicki.

CHAN_VOP_ADDED
	%s dodano do listy VOP kana³u %s.

CHAN_VOP_MOVED
	%s przeniesiono do listy VOP kana³u %s.

CHAN_VOP_NO_SUCH_ENTRY
	Brak wpisu (#%d) na li¶cie VOP kana³u %s.

CHAN_VOP_NOT_FOUND
	%s nie znaleziono na li¶cie VOP kana³u %s.

CHAN_VOP_NO_MATCH
	Nie znaleziono wpisów w li¶cie VOP kana³u %s.

CHAN_VOP_DELETED
	%s usuniêto z listy VOP kana³u %s.

CHAN_VOP_DELETED_ONE
	Usuniêto 1 wpis z listy VOP kana³u %s.

CHAN_VOP_DELETED_SEVERAL
	Usuniêto %d wpisów(y) z listy VOP kana³u %s.

CHAN_VOP_LIST_EMPTY
	Lista VOP kana³u %s jest pusta.

CHAN_VOP_LIST_HEADER
	Lista VOP kana³u %s:
	  Nr   Nick

CHAN_VOP_CLEAR
	Lista VOP kana³u %s zosta³a wyczyszczona.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS kana³ {ADD|DEL|LIST|CLEAR} [nick [poziom] | pozycja]

CHAN_ACCESS_XOP
	Nie mo¿esz u¿yæ tej komendy.
	Musisz u¿yæ poleceñ VOP, AOP lub SOP.
	Napisz %R%s HELP komenda, aby uzyskaæ wiêcej informacji.

CHAN_ACCESS_XOP_HOP
	Nie mo¿esz u¿yæ tej komendy.
	Musisz u¿yæ poleceñ VOP, HOP, AOP lub SOP.
	Napisz %R%s HELP komenda, aby uzyskaæ wiêcej informacji.

CHAN_ACCESS_DISABLED
	Modyfikacja listy dostêpu jest tymczasowo wy³±czona.

CHAN_ACCESS_LEVEL_NONZERO
	Poziom dostêpu nie mo¿e byæ zerem.

CHAN_ACCESS_LEVEL_RANGE
	Poziom dostêpu musi byæ pomiêdzy %d a %d w³±cznie.

CHAN_ACCESS_NICKS_ONLY
	Kana³owa lista dostêpu mo¿e zawieraæ tylko zarejestrowane nicki.

CHAN_ACCESS_REACHED_LIMIT
	Mo¿esz mieæ tylko %d wpisów na kana³owej li¶cie dostêpu.

CHAN_ACCESS_LEVEL_UNCHANGED
	Poziom dostêpu dla %s na kanale %s jest
	nadany od %d.

CHAN_ACCESS_LEVEL_CHANGED
	Poziom dostêpu dla %s na kanale %s
	zmieniono na %d.

CHAN_ACCESS_ADDED
	%s dodano do listy dostêpu kana³u %s z
	poziomem %d.

CHAN_ACCESS_NO_SUCH_ENTRY
	Brak wpisu (#%d) na li¶cie dostêpu kana³u %s.

CHAN_ACCESS_NOT_FOUND
	%s nie znaleziono na li¶cie dostêpu kana³u %s.

CHAN_ACCESS_NO_MATCH
	Nie znaleziono wpisów na li¶cie dostêpu kana³u %s.

CHAN_ACCESS_DELETED
	%s usuniêto z listy dostêpu kana³u %s.

CHAN_ACCESS_DELETED_ONE
	Usuniêto 1 wpis z listy dostêpu kana³u %s.

CHAN_ACCESS_DELETED_SEVERAL
	Usuniêto %d wpisów(y) z listy dostêpu kana³u %s.

CHAN_ACCESS_LIST_EMPTY
	Lista dostêpu kana³u %s jest pusta.

CHAN_ACCESS_LIST_HEADER
	Lista dostêpu kana³u %s:
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
	Modyfikacja listy AKICK jest tymczasowo wy³±czona.

CHAN_AKICK_ALREADY_EXISTS
	%s ju¿ istnieje w li¶cie AKICK kana³u %s.

CHAN_AKICK_REACHED_LIMIT
	Mo¿esz mieæ jedynie %d wpisów w li¶cie AKICK.

CHAN_AKICK_ADDED
	%s dodano do listy AKICK kana³u %s.

CHAN_AKICK_NO_SUCH_ENTRY
	Brak wpisu (#%d) na li¶cie AKICK kana³u %s.

CHAN_AKICK_NOT_FOUND
	%s nie znaleziono na li¶cie AKICK kana³u %s.

CHAN_AKICK_NO_MATCH
	Nie znaleziono wpisów na li¶cie AKICK kana³u %s.

CHAN_AKICK_STUCK
	%s jest od teraz ca³y czas aktywny na %s.

CHAN_AKICK_UNSTUCK
	%s nie jest ju¿ zawsze aktywny na %s.

CHAN_AKICK_DELETED
	%s usuniêto z listy AKICK %s.

CHAN_AKICK_DELETED_ONE
	Usuniêto 1 wpis z listy AKICK kana³u %s.

CHAN_AKICK_DELETED_SEVERAL
	Usuniêto %d wpisów z listy AKICK kana³u %s.

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
	Egzekucja listy AKICK kana³u %s zakoñczona.
	Dotyczy³a %d u¿ytkowników.

CHAN_AKICK_CLEAR
	Lista AKICK kana³u %s zosta³a wyczyszczona.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS kana³ {SET | DIS[ABLE] | LIST | RESET} [element [poziom]]

CHAN_LEVELS_XOP
	Modyfikowanie poziomów nie jest dostêpne w trybie XOP.

CHAN_LEVELS_RANGE
	Poziom musi byæ pomiêdzy %d a %d w³±cznie.

CHAN_LEVELS_CHANGED
	Wymagany poziom dla %s
	na kanale %s zmieniono teraz na %d.

CHAN_LEVELS_UNKNOWN
	Nieznane ustawienie %s.
	Napisz %R%s HELP LEVELS DESC, aby uzyskaæ listê dostêpnych mo¿liwo¶ci.

CHAN_LEVELS_DISABLED
	%s wy³±czono na kanale %s.

CHAN_LEVELS_LIST_HEADER
	Ustawienia poziomów dostêpu kana³u %s:

CHAN_LEVELS_LIST_DISABLED
	    %-*s  (wy³±czony)

CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (tylko w³a¶ciciel)

CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d

CHAN_LEVELS_RESET
	Poziomy dostêpu kana³u %s zosta³y zresetowane do domy¶lnych.

# Status messages
CHAN_STATUS_SYNTAX
	STATUS ERROR B³±d sk³adni

CHAN_STATUS_NOT_REGGED
	STATUS ERROR Kana³ %s nie jest zarejestrowany

CHAN_STATUS_FORBIDDEN
	STATUS ERROR Kana³ %s jest zakazany

CHAN_STATUS_NOTONLINE
	STATUS ERROR Nick %s nie jest online

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
	      Powitanie: %s

CHAN_INFO_TIME_REGGED
	 Zarejestrowano: %s

CHAN_INFO_LAST_USED
	        U¿ywano: %s

CHAN_INFO_LAST_TOPIC
	  Ostatni temat: %s

CHAN_INFO_TOPIC_SET_BY
	Ustawiony przez: %s

CHAN_INFO_URL
	      Adres www: %s

CHAN_INFO_EMAIL
	   Adres e-mail: %s

CHAN_INFO_BANTYPE
	       Typ bana: %d

CHAN_INFO_OPTIONS
	          Opcje: %s

CHAN_INFO_OPT_KEEPTOPIC
	KEEPTOPIC

CHAN_INFO_OPT_OPNOTICE
	OPNOTICE

CHAN_INFO_OPT_PEACE
	PEACE

CHAN_INFO_OPT_PRIVATE
	PRIVATE

CHAN_INFO_OPT_RESTRICTED
	RESTRICTED

CHAN_INFO_OPT_SECURE
	SECURE

CHAN_INFO_OPT_SECUREOPS
	SECUREOPS

CHAN_INFO_OPT_SECUREFOUNDER
	SECUREFOUNDER

CHAN_INFO_OPT_SIGNKICK
	SIGNKICK

CHAN_INFO_OPT_TOPICLOCK
	TOPICLOCK

CHAN_INFO_OPT_XOP
	XOP

CHAN_INFO_OPT_NONE
	Brak

CHAN_INFO_MODE_LOCK
	Blokowane tryby: %s

CHAN_INFO_EXPIRE
	         Wygasa: %s

CHAN_INFO_NO_EXPIRE
	Ten kana³ nie wyga¶nie.

# LIST responses
CHAN_LIST_SYNTAX
	LIST wzorzec

CHAN_LIST_SERVADMIN_SYNTAX
	LIST wzorzec [FORBIDDEN] [SUSPENDED] [NOEXPIRE]

CHAN_LIST_HEADER
	Lista pasuj±cych wpisów %s:

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
	Ban na kanale %s zosta³ zdjêty.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC kana³ [temat]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR kana³ co

CHAN_CLEARED_BANS
	Wszystkie bany z kana³u %s zosta³y usuniête.

CHAN_CLEARED_EXCEPTS
	Wszystkie wyj±tki z kana³u %s zosta³y usuniête.

CHAN_CLEARED_MODES
	Wszystkie flagi kana³u %s zosta³y zresetowane.

CHAN_CLEARED_OPS
	Flagi +o zosta³y wyczyszczone na kanale %s.

CHAN_CLEARED_HOPS
	Flagi +h zosta³y wyczyszczone na kanale %s.

CHAN_CLEARED_VOICES
	Flagi +v zosta³y wyczyszczone na kanale %s.

CHAN_CLEARED_USERS
	Wszyscy u¿ytkownicy zostali usuniêci z kana³u %s.

CHAN_CLEARED_INVITES
	Wszystkie zaproszenia z kana³u %s zosta³y usuniête.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS kana³

CHAN_GETPASS_UNAVAILABLE
	Komenda GETPASS nie dzia³a, bo has³a s± szyfrowane.

CHAN_GETPASS_PASSWORD_IS
	Has³o kana³u %s to %s.

# GETKEY responses
CHAN_GETKEY_SYNTAX
	GETKEY kana³

CHAN_GETKEY_NOKEY
	Kana³ %s nie posiada klucza.

CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS kana³

CHAN_SENDPASS_UNAVAILABLE
	Komenda SENDPASS nie dzia³a, bo has³a s± szyfrowane.

CHAN_SENDPASS_SUBJECT
	Has³o do kana³u %s

CHAN_SENDPASS_HEAD
	Witaj!

CHAN_SENDPASS_LINE_1
	Za¿±da³e¶(a¶) wys³ania has³a do kana³u %s na e-mail.

CHAN_SENDPASS_LINE_2
	Twoje has³o to: %s.
	Dla w³asnego bezpieczeñstwa nale¿y to has³o natychmiast zmieniæ na inne.

CHAN_SENDPASS_LINE_3
	Je¶li nie wiesz dlaczego ta wiadomo¶æ dotar³a do Ciebie zignoruj j±. Je¶li ta sytuacja bêdzie siê powtarzaæ skontaktuj siê z administratorem sieci.

CHAN_SENDPASS_LINE_4
	PROSZÊ NIE ODPOWIADAÆ NA T¡ WIADOMO¦Æ!

CHAN_SENDPASS_LINE_5
	Administratorzy sieci %s.

CHAN_SENDPASS_OK
	Has³o do kana³u %s zosta³o wys³ane.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID kana³ [powód]

CHAN_FORBID_SYNTAX_REASON
	FORBID kana³ powód

CHAN_FORBID_SUCCEEDED
	Kana³ %s zosta³ zablokowany.

CHAN_FORBID_FAILED
	Nie mogê zablokowaæ kana³u %s!

CHAN_FORBID_REASON
	Ten kana³ zosta³ zablokowany.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND kana³ [powód]

CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND kana³ powód

CHAN_SUSPEND_SUCCEEDED
	Kana³ %s zosta³ zawieszony.

CHAN_SUSPEND_FAILED
	Zawieszenie kana³u %s nie powiod³o siê!

CHAN_SUSPEND_REASON
	Ten kana³ zosta³ zawieszony.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND kana³

CHAN_UNSUSPEND_ERROR
	Nie znaleziono znaku # na pocz±tku nazwy kana³u.

CHAN_UNSUSPEND_SUCCEEDED
	Kana³ %s zosta³ uwolniony.

CHAN_UNSUSPEND_FAILED
	Nie mogê uwolniæ kana³u %s!

# Misc responses
CHAN_EXCEPTED
	%s posiada wyj±tek na %s i nie mo¿e byæ zbanowany póki nie zostanie usuniêty.

###########################################################################
#
# MemoServ messages
#
###########################################################################
# Automatic messages
MEMO_HAVE_NEW_MEMO
	Masz jedn± now± wiadomo¶æ.

MEMO_HAVE_NEW_MEMOS
	Masz %d nowe(ych) wiadomo¶ci.

MEMO_TYPE_READ_LAST
	Napisz %R%s READ LAST aby przeczytaæ.

MEMO_TYPE_READ_NUM
	Napisz %R%s READ %d aby przeczytaæ.

MEMO_TYPE_LIST_NEW
	Napisz %R%s LIST NEW,
	aby zobaczyæ listê nowych wiadomo¶ci.

MEMO_AT_LIMIT
	Uwaga: Osi±gniêto ju¿ limit wiadomo¶ci (%d).
	Nie bêdziesz otrzymywaæ nowych wiadomo¶ci dopóki
	nie usuniesz kilku starych.

MEMO_OVER_LIMIT
	Uwaga: Maksymalny limit wiadomo¶ci (%d) zosta³ osi±gniêty.
	Nie bêdziesz otrzymywaæ nowych wiadomo¶ci je¶li kilku nie usuniesz.

MEMO_X_MANY_NOTICE
	Jest %d wiadomo¶ci dla kana³u %s.

MEMO_X_ONE_NOTICE
	Jest %d wiadomo¶æ dla kana³u %s.

MEMO_NEW_X_MEMO_ARRIVED
	Jest nowa wiadomo¶æ dla kana³u %s.
	Napisz %R%s READ %s %d aby j± przeczytaæ.

MEMO_NEW_MEMO_ARRIVED
	Nasz now± wiadomo¶æ od %s.
	Napisz %R%s READ %d aby j± przeczytaæ.

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
	Wysy³anie wiadomo¶ci jest tymczasowo wy³±czone.

MEMO_SEND_PLEASE_WAIT
	Odczekaj %d sekund(y) przed ponownym u¿yciem komendy SEND.

MEMO_X_GETS_NO_MEMOS
	%s nie mo¿e odbieraæ wiadomo¶ci.

MEMO_X_HAS_TOO_MANY_MEMOS
	%s ma zbyt du¿o wiadomo¶ci i nie mo¿e otrzymywaæ nowych.

MEMO_SENT
	Wiadomo¶æ zosta³a wys³ana do %s.

MEMO_MASS_SENT
	Wiadomo¶æ zosta³a wys³ana do wszystkich zarejestrowanych u¿ytkowników.
	
# STAFF responses
MEMO_STAFF_SYNTAX
	STAFF tre¶æ wiadomo¶ci

# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {nick | kana³}

MEMO_CANCEL_DISABLED
	Odwo³ywanie wiadomo¶ci jest tymczasowo wy³±czone.

MEMO_CANCEL_NONE
	Brak wiadomo¶ci do odwo³ania.

MEMO_CANCELLED
	Ostatnia wiadomo¶æ do %s zosta³a odwo³ana.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [kana³] [lista | NEW]

MEMO_HAVE_NO_NEW_MEMOS
	Nie masz nowych wiadomo¶ci.

MEMO_X_HAS_NO_NEW_MEMOS
	%s nie ma nowych wiadomo¶ci.

MEMO_LIST_MEMOS
	Wiadomo¶ci dla %s. Aby przeczytaæ, napisz: %R%s READ numer

MEMO_LIST_NEW_MEMOS
	Nowe wiadomo¶ci dla %s. Aby przeczytaæ,
	napisz: %R%s READ numer

MEMO_LIST_CHAN_MEMOS
	Wiadomo¶ci dla %s. Aby przeczytaæ, napisz: %R%s READ %s numer

MEMO_LIST_CHAN_NEW_MEMOS
	Nowe wiadomo¶ci dla %s. Aby przeczytaæ,
	napisz: %R%s READ %s numer

MEMO_LIST_HEADER
	 Nr   Nadawca            Data/Czas

MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [kana³] {lista | LAST | NEW}

MEMO_HEADER
	Wiadomo¶æ %d od %s (%s). Aby usun±æ, napisz: %R%s DEL %d

MEMO_CHAN_HEADER
	Wiadomo¶æ %d od %s (%s). Aby usun±æ, napisz: %R%s DEL %s %d

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
	Wszystkie wiadomo¶ci kana³u %s zosta³y usuniête.

# SET responses
MEMO_SET_SYNTAX
	SET opcja parametry

MEMO_SET_DISABLED
	Ustawianie opcji wiadomo¶ci jest tymczasowo wy³±czone.

MEMO_SET_UNKNOWN_OPTION
	Nieznana opcja komendy SET %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | MAIL | NOMAIL | OFF }

MEMO_SET_NOTIFY_ON
	%s powiadomi Ciê o wiadomo¶ciach podczas logowania i w momencie ich nadej¶cia.

MEMO_SET_NOTIFY_LOGON
	%s powiadomi Ciê o wiadomo¶ciach podczas logowania oraz po wy³±czeniu /away.

MEMO_SET_NOTIFY_NEW
	%s bêdzie powiadamia³ Ciê o nowych wiadomo¶ciach w momencie ich nadej¶cia.

MEMO_SET_NOTIFY_OFF
	%s nie bêdzie powiadamia³ Ciê o nowych wiadomo¶ciach.

MEMO_SET_NOTIFY_MAIL
	Powiadomienie o nowych wiadomo¶ciach bêd± wysy³ane na adres e-mail.

MEMO_SET_NOTIFY_NOMAIL
	Powiadomienie o nowych wiadomo¶ciach nie bêd± wysy³ane na adres e-mail.

MEMO_SET_NOTIFY_INVALIDMAIL
	Twój nick nie ma ustawionego adresu e-mail.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [kana³] limit

MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [nick | kana³] {limit | NONE} [HARD]

MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Nie mo¿esz zmieniaæ swojego limitu wiadomo¶ci.

MEMO_SET_LIMIT_FORBIDDEN
	Limit wiadomo¶ci dla %s nie mo¿e byæ zmieniany.

MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Nie mo¿esz ustawiæ swojego limitu wiadomo¶ci wiêkszego ni¿ %d.

MEMO_SET_LIMIT_TOO_HIGH
	Nie mo¿esz ustawiæ limitu wiadomo¶ci dla %s wiêkszego ni¿ %d.

MEMO_SET_LIMIT_OVERFLOW
	Limit wiadomo¶ci jest za du¿y. Limituj do %d.

MEMO_SET_YOUR_LIMIT
	Twój limit wiadomo¶ci zosta³ zmieniony na %d.

MEMO_SET_YOUR_LIMIT_ZERO
	Nie bêdziesz ju¿ d³u¿ej odbieraæ wiadomo¶ci.

MEMO_UNSET_YOUR_LIMIT
	Twój limit wiadomo¶ci zosta³ wy³±czony.

MEMO_SET_LIMIT
	Limit wiadomo¶ci dla %s zmieniono na %d.

MEMO_SET_LIMIT_ZERO
	Limit wiadomo¶ci dla %s ustawiono na 0.

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
	Aktualnie masz jedn± wiadomo¶æ.

MEMO_INFO_MEMO_UNREAD
	Aktualnie masz jedn± wiadomo¶æ i jest ona nieprzeczytana.

MEMO_INFO_MEMOS
	Aktualnie masz %d wiadomo¶ci.

MEMO_INFO_MEMOS_ONE_UNREAD
	Aktualnie masz %d wiadomo¶ci, w tym jedn± nieprzeczytan±.

MEMO_INFO_MEMOS_SOME_UNREAD
	Aktualnie masz %d wiadomo¶ci, z czego %d jest nieprzeczytanych.

MEMO_INFO_MEMOS_ALL_UNREAD
	Aktualnie masz %d nieprzeczytanych wiadomo¶ci.

MEMO_INFO_LIMIT
	Twój limit wiadomo¶ci wynosi %d.

MEMO_INFO_HARD_LIMIT
	Twój limit wiadomo¶ci wynosi %d i nie mo¿e byæ zmieniony.

MEMO_INFO_LIMIT_ZERO
	Twój limit wiadomo¶ci wynosi 0, nie bêdziesz otrzymywaæ nowych.

MEMO_INFO_HARD_LIMIT_ZERO
	Twój limit wiadomo¶ci wynosi 0, nie bêdziesz odbieraæ nowych i nie mo¿esz zmieniæ tego limitu.
MEMO_INFO_NO_LIMIT
	Nie masz limitu wiadomo¶ci, które mo¿esz trzymaæ.

MEMO_INFO_NOTIFY_OFF
	Nie bêdziesz otrzymywaæ informacji o nowych wiadomo¶ciach.

MEMO_INFO_NOTIFY_ON
	Bêdziesz powiadomienie informacje o nowych wiadomo¶ciach
	podczas logowania i w momencie ich nadej¶cia.

MEMO_INFO_NOTIFY_RECEIVE
	Bêdziesz otrzymywaæ powiadomienie o nowych wiadomo¶ciach.

MEMO_INFO_NOTIFY_SIGNON
	Bêdziesz otrzymywaæ powiadomienie o nowych wiadomo¶ciach podczas logowania.

MEMO_INFO_X_NO_MEMOS
	%s aktualnie nie ma wiadomo¶ci.

MEMO_INFO_X_MEMO
	%s aktualnie ma jedn± wiadomo¶æ.

MEMO_INFO_X_MEMO_UNREAD
	%s ma aktualnie jedn± wiadomo¶æ, która jest nieprzeczytana.

MEMO_INFO_X_MEMOS
	%s ma aktualnie %d wiadomo¶ci.

MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s ma aktualnie %d wiadomo¶ci, w tym jedn± nieprzeczytan±.

MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s ma aktualnie %d wiadomo¶ci, z czego %d jest nieprzeczytanych.

MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s ma aktualnie %d nieprzeczytanych wiadomo¶ci.

MEMO_INFO_X_LIMIT
	%s ma limit wiadomo¶ci %d.

MEMO_INFO_X_HARD_LIMIT
	%s ma limit wiadomo¶ci %d i nie mo¿e byæ zmieniony.

MEMO_INFO_X_NO_LIMIT
	%s nie ma limitu wiadomo¶ci.

MEMO_INFO_X_NOTIFY_OFF
	%s nie otrzymuje powiadomieñ o nowych wiadomo¶ciach.

MEMO_INFO_X_NOTIFY_ON
	%s otrzymuje powiadomienia o nowych wiadomo¶ciach
	podczas logowania i w momencie ich nadej¶cia.

MEMO_INFO_X_NOTIFY_RECEIVE
	%s otrzymuje powiadomienia o nowych wiadomo¶ciach w momencie ich nadej¶cia.

MEMO_INFO_X_NOTIFY_SIGNON
	%s otrzymuje powiadomienia o nowych wiadomo¶ciach podczas logowania.

# Memo2Mail responses
MEMO_MAIL_SUBJECT
	Nowa wiadomo¶æ

MEMO_MAIL_TEXT1
	Witaj %s!

MEMO_MAIL_TEXT2
	W³a¶nie nadesz³a now± wiadomo¶æ od %s, ma numer %d.

MEMO_MAIL_TEXT3
	Tre¶æ wiadomo¶ci:

# RSEND responses
MEMO_RSEND_PLEASE_WAIT
	Odczekaj %d sekund przed ponownym u¿yciem komendy RSEND.

MEMO_RSEND_DISABLED
	Komenda RSEND nie jest dostêpna w tej sieci.

MEMO_RSEND_SYNTAX
	RSEND {nick | kana³} tre¶æ

MEMO_RSEND_NICK_MEMO_TEXT
	[auto-memo] Wys³ana przez Ciebie wiadomo¶æ zosta³a przeczytana.

MEMO_RSEND_CHAN_MEMO_TEXT
	[auto-memo] Wys³ana przez Ciebie wiadomo¶æ do %s zosta³a przeczytana.

MEMO_RSEND_USER_NOTIFICATION
	Powiadomienie o przeczytaniu wiadomo¶ci zosta³o wys³ane do %s.

# CHECK responses
MEMO_CHECK_SYNTAX
	CHECK nick

MEMO_CHECK_NOT_READ
	Ostatnia wys³ana przez Ciebie wiadomo¶æ
	do %s (w %s) jest wci±¿ nieprzeczytana.

MEMO_CHECK_READ
	Ostatnia wiadomo¶æ wys³ana do %s (w %s) zosta³a przeczytana.

MEMO_CHECK_NO_MEMO
	%s nie posiada wiadomo¶ci od Ciebie.

MEMO_NO_RSEND_SELF
	Nie mo¿esz wysy³aæ wiadomo¶ci z potwierdzeniem odbioru do siebie!

###########################################################################
#
# BotServ messages
#
###########################################################################
# Standard responses
BOT_DOES_NOT_EXIST
	Bot %s nie istnieje.

BOT_NOT_ASSIGNED
	Musisz najpierw przydzieliæ bota do kana³u.
	Napisz %R%S HELP ASSIGN, aby uzyskaæ wiêcej informacji.

BOT_NOT_ON_CHANNEL
	Nie ma bota na kanale %s.

# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	Nie u¿ywaj s³owa \"%s\" na tym kanale!

BOT_REASON_BADWORD_GENTLE
	Uwa¿aj co piszesz!

BOT_REASON_BOLD
	Nie pogrubiaj tekstu na tym kanale!

BOT_REASON_CAPS
	Wy³±cz CAPS LOCKa!

BOT_REASON_COLOR
	Nie u¿ywaj kolorów na tym kanale!

BOT_REASON_FLOOD
	Przestañ floodowaæ!

BOT_REASON_REPEAT
	Nie powtarzaj siê!

BOT_REASON_REVERSE
	Nie u¿ywaj odwracania kolorów!

BOT_REASON_UNDERLINE
	Nie u¿ywaj podkre¶leñ na tym kanale!

# !seen replies
BOT_SEEN_BOT
	Tak! Jestem tutaj %s!

BOT_SEEN_YOU
	%s? Szukasz siebie?

BOT_SEEN_ON_CHANNEL
	%s jest teraz na kanale!

BOT_SEEN_ON_CHANNEL_AS
	%s jest teraz na kanale (jako %s)!

BOT_SEEN_ON
	Widzia³em %s tutaj %s temu.

BOT_SEEN_NEVER
	Nigdy nie widzia³em %s na tym kanale.

BOT_SEEN_UNKNOWN
	Nie wiem kim jest %s.

# BOT responses
BOT_BOT_SYNTAX
	BOT ADD nick user host real
	BOT CHANGE stary-nick nowy-nick [user [host [real]]]
	BOT DEL nick

BOT_BOT_ALREADY_EXISTS
	Bot %s ju¿ istnieje.

BOT_BOT_CREATION_FAILED
	B³±d, nie uda³o siê utworzyæ bota.

BOT_BOT_READONLY
	Modyfikacja botów jest tymczasowo wy³±czona.

BOT_BOT_ADDED
	%s!%s@%s (%s) dodano do listy botów.

BOT_BOT_ANY_CHANGES
	Nic siê nie zmieni³o.
BOT_BOT_CHANGED
	Bot %s zosta³ zmieniony na %s!%s@%s (%s)

BOT_BOT_DELETED
	Bot %s zosta³ usuniêty.

# BOTLIST responses
BOT_BOTLIST_HEADER
	Lista botów:

BOT_BOTLIST_PRIVATE_HEADER
	Boty zarezerwowane dla IRC operatorów:

BOT_BOTLIST_FOOTER
	%d boty(ów) dostêpnych.

BOT_BOTLIST_EMPTY
	Obecnie nie ma ¿adnych botów dostêpnych.
	Popro¶ administratora serwisów o dodanie.

# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN kana³ nick

BOT_ASSIGN_READONLY
	Przydzielanie botów jest tymczasowo wy³±czone.

BOT_ASSIGN_ALREADY
	Bot %s jest ju¿ postawiony na %s.

BOT_ASSIGN_ASSIGNED
	Bot %s zosta³ przypisany do %s.

# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN kana³

BOT_UNASSIGN_UNASSIGNED
	Nie ma ¿adnych botów przydzielonych do %s.

# INFO responses
BOT_INFO_SYNTAX
	INFO {kana³ | nick}

BOT_INFO_NOT_FOUND
	%s nie jest prawid³ow± nazw± bota lub kana³u.

BOT_INFO_BOT_HEADER
	Informacje o bocie %s:

BOT_INFO_BOT_MASK
		Maska: %s@%s

BOT_INFO_BOT_REALNAME
		Nazwa: %s

BOT_INFO_BOT_CREATED
	    Utworzony: %s

BOT_INFO_BOT_USAGE
	    Obecny na: %d kana³ach
BOT_INFO_BOT_OPTIONS
		Opcje: %s

BOT_INFO_OPT_PRIVATE
	PRIVATE

BOT_INFO_CHAN_HEADER
	Informacje o kanale %s:

BOT_INFO_CHAN_BOT
	       Przydzielony bot: %s

BOT_INFO_CHAN_BOT_NONE
	       Przydzielony bot: (brak)

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
	             Inne opcje: %s

BOT_INFO_OPT_DONTKICKOPS
	DONTKICKOPS

BOT_INFO_OPT_DONTKICKVOICES
	DONTKICKVOICES

BOT_INFO_OPT_FANTASY
	FANTASY

BOT_INFO_OPT_GREET
	GREET

BOT_INFO_OPT_NOBOT
	NOBOT

BOT_INFO_OPT_SYMBIOSIS
	SYMBIOSIS

BOT_INFO_OPT_NONE
	Brak

# SET responses
BOT_SET_SYNTAX
	SET (kana³ | bot) opcja ustawienie

BOT_SET_DISABLED
	Konfigurowanie bota jest tymczasowo wy³±czone.

BOT_SET_UNKNOWN
	Nieznana opcja %s.
	Napisz %R%S HELP SET, aby uzyskaæ wiêcej informacji.

# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET kana³ DONTKICKOPS {ON|OFF}

BOT_SET_DONTKICKOPS_ON
	Bot nie bêdzie kopa³ operatorów na kanale %s.

BOT_SET_DONTKICKOPS_OFF
	Bot bêdzie teraz kopa³ operatorów na kanale %s.

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
	Komendy fantasy na kanale %s zosta³y w³±czone.

BOT_SET_FANTASY_OFF
	Komendy fantasy na kanale %s zosta³y wy³±czone.

# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET kana³ GREET {ON|OFF}

BOT_SET_GREET_ON
	Witanie u¿ytkowników na kanale %s zosta³o w³±czone.

BOT_SET_GREET_OFF
	Witanie u¿ytkowników na kanale %s zosta³o wy³±czone.

# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET nick-bota NOBOT {ON|OFF}

BOT_SET_NOBOT_ON
	Opcja NOBOT zosta³a w³±czona na kanale %s.

BOT_SET_NOBOT_OFF
	Opcja NOBOT zosta³a wy³±czona na kanale %s.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET nick-bota PRIVATE {ON|OFF}

BOT_SET_PRIVATE_ON
	Opcja PRIVATE bota %s zosta³a w³±czona.

BOT_SET_PRIVATE_OFF
	Opcja PRIVATE bota %s zosta³a wy³±czona.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET kana³ SYMBIOSIS {ON|OFF}

BOT_SET_SYMBIOSIS_ON
	Opcja SYMBIOSIS zosta³a w³±czona na kanale %s.

BOT_SET_SYMBIOSIS_OFF
	Opcja SYMBIOSIS zosta³a wy³±czona na kanale %s.

# KICK responses
BOT_KICK_SYNTAX
	KICK kana³ opcja {ON|OFF} [ustawienia]

BOT_KICK_DISABLED
	Konfigurowanie kopów jest tymczasowo wy³±czone.

BOT_KICK_UNKNOWN
	Nieznana opcja %s.
	Napisz %R%S HELP KICK, aby uzyskaæ wiêcej informacji.

BOT_KICK_BAD_TTB
	%s nie mo¿e byæ wziêty jako ilo¶æ wykroczeñ do za³o¿enia bana.

BOT_KICK_BADWORDS_ON
	Bot bêdzie kopa³ za zakazane s³owa.
	U¿yj komendy BADWORDS, aby modyfikowaæ listê s³ów.

BOT_KICK_BADWORDS_ON_BAN
	Bot bêdzie kopa³ za zakazane s³owa,
	po %d kopach dla tego samego u¿ytkownika za³o¿y bana.
	U¿yj komendy BADWORDS, aby modyfikowaæ listê.

BOT_KICK_BADWORDS_OFF
	Bot nie bêdzie kopa³ za zakazane s³owa.

BOT_KICK_BOLDS_ON
	Bot bêdzie kopa³ za pogrubienia.

BOT_KICK_BOLDS_ON_BAN
	Bot bêdzie kopa³ za pogrubienia,
	po %d kopach dla tego samego u¿ytkownika za³o¿y bana.

BOT_KICK_BOLDS_OFF
	Bot nie bêdzie kopa³ za pogrubienia.

BOT_KICK_CAPS_ON
	Bot bêdzie kopa³ za CAPSa (du¿e litery
	musz± stanowiæ %d znaków %d%% ca³ej wiadomo¶ci).

BOT_KICK_CAPS_ON_BAN
	Bot bêdzie kopa³ za CAPSa (du¿e litery
	musz± stanowiæ %d znaków %d%% ca³ej wiadomo¶ci).
	po %d kopach dla tego samego u¿ytkownika za³o¿y bana.

BOT_KICK_CAPS_OFF
	Bot nie bêdzie kopa³ za CAPSa.

BOT_KICK_COLORS_ON
	Bot bêdzie kopa³ za kolory.

BOT_KICK_COLORS_ON_BAN
	Bot bêdzie kopa³ za kolory,
	po %d kopach dla tego samego u¿ytkownika za³o¿y bana.

BOT_KICK_COLORS_OFF
	Bot nie bêdzie kopa³ za kolory.

BOT_KICK_FLOOD_ON
	Bot bêdzie kopa³ za flood
	(%d linii w %d sekund).

BOT_KICK_FLOOD_ON_BAN
	Bot bêdzie kopa³ za flood (%d linii w %d sekund),
	po %d kopach dla tego samego u¿ytkownika za³o¿y bana.

BOT_KICK_FLOOD_OFF
	Bot nie bêdzie kopa³ za flood.

BOT_KICK_REPEAT_ON
	Bot bêdzie kopa³ za powtórzenia
	(u¿ytkowników, którzy napisz± %d razy to samo).

BOT_KICK_REPEAT_ON_BAN
	Bot bêdzie kopa³ za powtórzenia
	(u¿ytkowników, którzy napisz± %d razy to samo),
	po %d kopach dla tego samego u¿ytkownika za³o¿y bana.

BOT_KICK_REPEAT_OFF
	Bot nie bêdzie kopa³ za powtórzenia.

BOT_KICK_REVERSES_ON
	Bot bêdzie kopa³ za odwracanie kolorów.

BOT_KICK_REVERSES_ON_BAN
	Bot bêdzie kopa³ za odwracanie kolorów,
	po %d kopach dla tego samego u¿ytkownika za³o¿y bana.

BOT_KICK_REVERSES_OFF
	Bot nie bêdzie kopa³ za odwracanie kolorów.

BOT_KICK_UNDERLINES_ON
	Bot bêdzie kopa³ za podkre¶lenia.

BOT_KICK_UNDERLINES_ON_BAN
	Bot bêdzie kopa³ za podkre¶lenia,
	po %d kopach dla tego samego u¿ytkownika za³o¿y bana.

BOT_KICK_UNDERLINES_OFF
	Bot nie bêdzie kopa³ za podkre¶lenia.

# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS kana³ {ADD|DEL|LIST|CLEAR} [s³owo | pozycja] [SINGLE|START|END]

BOT_BADWORDS_DISABLED
	Modyfikacja listy zakazanych s³ów jest tymczasowo wy³±czona.

BOT_BADWORDS_REACHED_LIMIT
	B³±d, mo¿esz mieæ tylko %d wpisów na li¶cie zakazanych s³ów.

BOT_BADWORDS_ALREADY_EXISTS
	%s ju¿ istnieje na li¶cie zakazanych s³ów kana³u %s.

BOT_BADWORDS_ADDED
	%s dodano do listy zakazanych s³ów kana³u %s.

BOT_BADWORDS_NO_SUCH_ENTRY
	Brak wpisów (#%d) na li¶cie zakazanych s³ów kana³u %s.

BOT_BADWORDS_NOT_FOUND
	%s nie znaleziono na li¶cie zakazanych s³ów kana³u %s.

BOT_BADWORDS_NO_MATCH
	Nie znaleziono pasuj±cych wpisów na li¶cie zakazanych s³ów kana³u %s.

BOT_BADWORDS_DELETED
	%s usuniêto z listy zakazanych s³ów kana³u %s.

BOT_BADWORDS_DELETED_ONE
	Usuniêto 1 wpis z listy zakazanych s³ów kana³u %s.

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
	Lista zakazanych s³ów zosta³a wyczyszczona.

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
	Ident bota mo¿e siê sk³adaæ maksymalnie z %d znaków.

BOT_LONG_HOST
	Host bota mo¿e siê sk³adaæ maksymalnie z %d znaków.

###########################################################################
#
# OperServ messages
#
###########################################################################
# General messages
OPER_BOUNCY_MODES
	Serwisy nie mog± zmieniaæ trybów. Czy serwery s± dobrze skonfigurowane?

OPER_BOUNCY_MODES_U_LINE
	Serwisy nie mog± zmieniaæ trybów. Czy U-linie serwerów s± dobrze skonfigurowane?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL tre¶æ wiadomo¶ci

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Nieznana opcja STATS %s.

OPER_STATS_CURRENT_USERS
	U¿ytkowników  : %d (%d operatorów)

OPER_STATS_MAX_USERS
	Rekord u¿ytk. : %d (%s)

# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Serwisy dzia³aj± %d dni, %02d:%02d

OPER_STATS_UPTIME_1DHMS
	Serwisy dzia³aj± %d dzieñ, %02d:%02d

OPER_STATS_UPTIME_HMS
	Serwisy dzia³aj± %d godzin i %d minut(y).

OPER_STATS_UPTIME_HM1S
	Serwisy dzia³aj± %d godzin(y) i %d minut(y).

OPER_STATS_UPTIME_H1MS
	Serwisy dzia³aj± %d godzin(y) i %d minutê.

OPER_STATS_UPTIME_H1M1S
	Serwisy dzia³aj± %d godzin(y) i %d minutê.

OPER_STATS_UPTIME_1HMS
	Serwisy dzia³aj± %d godzinê i %d minut(y).

OPER_STATS_UPTIME_1HM1S
	Serwisy dzia³aj± %d godzinê i %d minut(y).

OPER_STATS_UPTIME_1H1MS
	Serwisy dzia³aj± %d godzinê i %d minutê.

OPER_STATS_UPTIME_1H1M1S
	Serwisy dzia³aj± %d godzinê i %d minutê.

OPER_STATS_UPTIME_MS
	Serwisy dzia³aj± %d minut(y) i %d sekund(y).

OPER_STATS_UPTIME_M1S
	Serwisy dzia³aj± %d minut(y) i %d sekundê.

OPER_STATS_UPTIME_1MS
	Serwisy dzia³aj± %d minutê i %d sekund(y).

OPER_STATS_UPTIME_1M1S
	Serwisy dzia³aj± %d minutê i %d sekundê.

OPER_STATS_BYTES_READ
	Odebrano      : %5d kB

OPER_STATS_BYTES_WRITTEN
	Wys³ano       : %5d kB

OPER_STATS_USER_MEM
	U¿ytkownicy   : %6d rekordów, %5d kB

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

OPER_STATS_HOSTSERV_MEM
	HostServ      : %6d rekordów, %5d kB

OPER_STATS_OPERSERV_MEM
	OperServ      : %6d rekordów, %5d kB

OPER_STATS_SESSIONS_MEM
	Sesji         : %6d rekordów, %5d kB

OPER_STATS_AKILL_COUNT
	Aktualna ilo¶æ wpisów AKILL: %d

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
	Domy¶lny czas wygasania AKILL: nie wygasa

OPER_STATS_SGLINE_COUNT
	Aktualna ilo¶æ wpisów SGLINE: %d

OPER_STATS_SGLINE_EXPIRE_DAYS
	Domy¶lny czas wygasania SGLINE: %d dni

OPER_STATS_SGLINE_EXPIRE_DAY
	Domy¶lny czas wygasania SGLINE: 1 dzieñ

OPER_STATS_SGLINE_EXPIRE_HOURS
	Domy¶lny czas wygasania SGLINE: %d godzin(y)

OPER_STATS_SGLINE_EXPIRE_HOUR
	Domy¶lny czas wygasania SGLINE: 1 godzina

OPER_STATS_SGLINE_EXPIRE_MINS
	Domy¶lny czas wygasania SGLINE: %d minut(y)

OPER_STATS_SGLINE_EXPIRE_MIN
	Domy¶lny czas wygasania SGLINE: 1 minuta

OPER_STATS_SGLINE_EXPIRE_NONE
	Domy¶lny czas wygasania SGLINE: nie wygasa

OPER_STATS_SQLINE_COUNT
	Aktualna ilo¶æ wpisów SQLINE: %d

OPER_STATS_SQLINE_EXPIRE_DAYS
	Domy¶lny czas wygasania SQLINE: %d dni

OPER_STATS_SQLINE_EXPIRE_DAY
	Domy¶lny czas wygasania SQLINE: 1 dzieñ

OPER_STATS_SQLINE_EXPIRE_HOURS
	Domy¶lny czas wygasania SQLINE: %d godzin(y)

OPER_STATS_SQLINE_EXPIRE_HOUR
	Domy¶lny czas wygasania SQLINE: 1 godzina

OPER_STATS_SQLINE_EXPIRE_MINS
	Domy¶lny czas wygasania SQLINE: %d minut(y)

OPER_STATS_SQLINE_EXPIRE_MIN
	Domy¶lny czas wygasania SQLINE: 1 minuta

OPER_STATS_SQLINE_EXPIRE_NONE
	Domy¶lny czas wygasania SQLINE: nie wygasa

OPER_STATS_SZLINE_COUNT
	Aktualna ilo¶æ wpisów SZLINE: %d

OPER_STATS_SZLINE_EXPIRE_DAYS
	Domy¶lny czas wygasania SZLINE: %d dni

OPER_STATS_SZLINE_EXPIRE_DAY
	Domy¶lny czas wygasania SZLINE: 1 dzieñ

OPER_STATS_SZLINE_EXPIRE_HOURS
	Domy¶lny czas wygasania SZLINE: %d godzin(y)

OPER_STATS_SZLINE_EXPIRE_HOUR
	Domy¶lny czas wygasania SZLINE: 1 godzina

OPER_STATS_SZLINE_EXPIRE_MINS
	Domy¶lny czas wygasania SZLINE: %d minut(y)

OPER_STATS_SZLINE_EXPIRE_MIN
	Domy¶lny czas wygasania SZLINE: 1 minuta

OPER_STATS_SZLINE_EXPIRE_NONE
	Domy¶lny czas wygasania SZLINE: nie wygasa

OPER_STATS_RESET
	Statystyki zosta³y zresetowane.

OPER_STATS_UPLINK_SERVER
	Pod³±czony do: %s

OPER_STATS_UPLINK_CAPAB
	Uplink capab  : %s

OPER_STATS_UPLINK_SERVER_COUNT
	Serwerów      : %d

# MODE responses
OPER_MODE_SYNTAX
	MODE kana³ flagi

# UMODE respondes
OPER_UMODE_SYNTAX
	UMODE nick flagi

OPER_UMODE_SUCCESS
	Zmieniono flagi u¿ytkownika dla %s.

OPER_UMODE_CHANGED
	%s zmienia Twoje flagi u¿ytkownika.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE nick flagi

OPER_OLINE_SUCCESS
	Flagi operatora %s zosta³y przyznane dla %s.

OPER_OLINE_IRCOP
	Jeste¶ teraz IRC operatorem.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES kana³ [ALL]

OPER_CLEARMODES_DONE
	Tryby i bany zosta³y wyczyszczone na kanale %s.

OPER_CLEARMODES_ALL_DONE
	Wszystkie tryby zosta³y wyczyszczone na kanale %s.

# KICK responses
OPER_KICK_SYNTAX
	KICK kana³ nick powód

# SVSNICK responses
OPER_SVSNICK_SYNTAX
	SVSNICK nick nowy-nick

OPER_SVSNICK_NEWNICK
	Nick %s zostaje zmieniony na %s.

# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [nick | pozycja]

OPER_ADMIN_SKELETON
	Serwisy s± w trybie szkieletowym, komenda ADMIN nie jest dostêpna.

OPER_ADMIN_EXISTS
	%s ju¿ istnieje na li¶cie administratorów serwisów.

OPER_ADMIN_REACHED_LIMIT
	Mo¿esz mieæ tylko %d administratorów serwisów.

OPER_ADMIN_ADDED
	%s dodano do listy administratorów serwisów.

OPER_ADMIN_NOT_FOUND
	%s nie znaleziono na li¶cie administratorów serwisów.

OPER_ADMIN_NO_MATCH
	Nie znaleziono pasuj±cych wpisów na li¶cie administratorów serwisów.

OPER_ADMIN_DELETED
	%s usuniêto z listy administratorów serwisów.

OPER_ADMIN_DELETED_ONE
	Usuniêto 1 wpis z listy administratorów serwisów.

OPER_ADMIN_DELETED_SEVERAL
	Usuniêto %d wpisy(ów) z listy administratorów serwisów.

OPER_ADMIN_LIST_EMPTY
	Lista administratorów serwisów jest pusta.

OPER_ADMIN_LIST_HEADER
	Lista administratorów serwisów:
	  Nr    Nick

OPER_ADMIN_LIST_FORMAT
	  %3d   %s

OPER_ADMIN_CLEAR
	Lista administratorów serwisów zosta³a wyczyszczona.

OPER_ADMIN_MOVED
	%s przeniesiono na listê administratorów serwisów.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | pozycja]

OPER_OPER_SKELETON
	Serwisy s± w trybie szkieletowym, komenda OPER nie jest dostêpna.

OPER_OPER_EXISTS
	%s ju¿ istnieje na li¶cie operatorów serwisów.

OPER_OPER_REACHED_LIMIT
	Mo¿esz mieæ tylko %d operatorów serwisów.

OPER_OPER_ADDED
	%s dodano do listy operatorów serwisów.

OPER_OPER_NOT_FOUND
	%s nie znaleziono na li¶cie operatorów serwisów.

OPER_OPER_NO_MATCH
	Nie znaleziono pasuj±cych wpisów na li¶cie operatorów serwisów.

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
	%s przeniesiono na listê operatorów serwisów.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+czas-trwania] {maska | pozycja} [powód]]

OPER_AKILL_EXISTS
	%s ju¿ istnieje na li¶cie AKILL.

OPER_AKILL_ALREADY_COVERED
	%s ju¿ jest obejmowane przez %s.

OPER_AKILL_REACHED_LIMIT
	Mo¿esz mieæ tylko %d wpisów na li¶cie AKILL.

OPER_AKILL_NO_NICK
	Uwaga: maska AKILL nie mo¿e zawieraæ nicków.
	Upewnij siê, ¿e nie dodajesz czê¶ci nicka.

OPER_AKILL_ADDED
	%s dodano do listy AKILL.

OPER_AKILL_CHANGED
	Zmieniono czas wygasania %s.

OPER_AKILL_NOT_FOUND
	%s nie znaleziono na li¶cie AKILL.

OPER_AKILL_NO_MATCH
	Nie znaleziono pasuj±cych wpisów na li¶cie AKILL.

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
	CHANKILL [+czas-trwania] {#kana³} [powód]

# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+czas-trwania] {maska | pozycja} [powód]]

OPER_SGLINE_UNSUPPORTED
	Komenda SGLINE nie jest dostêpna w tej sieci.

OPER_SGLINE_EXISTS
	%s ju¿ istnieje na li¶cie SGLINE.

OPER_SGLINE_ALREADY_COVERED
	%s ju¿ jest obejmowane przez %s.

OPER_SGLINE_REACHED_LIMIT
	Mo¿esz mieæ tylko %d wpisów na li¶cie SGLINE.

OPER_SGLINE_ADDED
	%s dodano do listy SGLINE.

OPER_SGLINE_CHANGED
	Zmieniono czas wygasania %s.

OPER_SGLINE_NOT_FOUND
	%s nie znaleziono na li¶cie SGLINE.

OPER_SGLINE_NO_MATCH
	Nie znaleziono pasuj±cych wpisów na li¶cie SGLINE.

OPER_SGLINE_DELETED
	%s usuniêto z listy SGLINE.

OPER_SGLINE_DELETED_ONE
	Usuniêto 1 wpis z listy SGLINE.

OPER_SGLINE_DELETED_SEVERAL
	Usuniêto %d wpisy(ów) z listy SGLINE.

OPER_SGLINE_LIST_EMPTY
	Lista SGLINE jest pusta.

OPER_SGLINE_LIST_HEADER
	Aktualna lista SGLINE:
	  Nr    Maska                             Powód

OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s

OPER_SGLINE_VIEW_HEADER
	Aktualna lista SGLINE:

# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (przez %s na %s; %s)
	      %s

OPER_SGLINE_CLEAR
	Lista SGLINE zosta³a wyczyszczona.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+czas-trwania] {maska | pozycja} [powód]]

OPER_SQLINE_CHANNELS_UNSUPPORTED
	Kana³owe SQLINE nie jest obs³ugiwane przez Twoje ircd.

OPER_SQLINE_EXISTS
	%s ju¿ istnieje na li¶cie SQLINE.

OPER_SQLINE_ALREADY_COVERED
	%s ju¿ jest obejmowane przez %s.

OPER_SQLINE_REACHED_LIMIT
	Mo¿esz mieæ tylko %d wpisów na li¶cie SQLINE.

OPER_SQLINE_ADDED
	%s dodano do listy SQLINE.

OPER_SQLINE_CHANGED
	Zmieniono czas wygasania %s.

OPER_SQLINE_NOT_FOUND
	%s nie znaleziono na li¶cie SQLINE.

OPER_SQLINE_NO_MATCH
	Nie znaleziono pasuj±cych wpisów na li¶cie SQLINE.

OPER_SQLINE_DELETED
	%s usuniêto z listy SQLINE.

OPER_SQLINE_DELETED_ONE
	Usuniêto 1 wpis z listy SQLINE.

OPER_SQLINE_DELETED_SEVERAL
	Usuniêto %d wpisy(ów) z listy SQLINE.

OPER_SQLINE_LIST_EMPTY
	Lista SQLINE jest pusta.

OPER_SQLINE_LIST_HEADER
	Aktualna lista SQLINE:
	  Nr    Maska                             Powód

OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s

OPER_SQLINE_VIEW_HEADER
	Aktualna lista SQLINE:

# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (przez %s na %s; %s)
	      %s

OPER_SQLINE_CLEAR
	Lista SQLINE zosta³a wyczyszczona.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+czas-trwania] {maska | pozycja} [powód]]

OPER_SZLINE_UNSUPPORTED
	Komenda SZLINE nie jest dostêpna w tej sieci.

OPER_SZLINE_EXISTS
	%s ju¿ istnieje na li¶cie SZLINE.

OPER_SZLINE_ALREADY_COVERED
	%s ju¿ jest obejmowane przez %s.

OPER_SZLINE_REACHED_LIMIT
	Mo¿esz mieæ tylko %d wpisów na li¶cie SZLINE.

OPER_SZLINE_ONLY_IPS
	Uwaga: mo¿esz dodawaæ jedynie maski IP do listy SZLINE.

OPER_SZLINE_ADDED
	%s dodano do listy SZLINE.

OPER_SZLINE_CHANGED
	Zmieniono czas wygasania %s.

OPER_SZLINE_NOT_FOUND
	%s nie znaleziono na li¶cie SZLINE.

OPER_SZLINE_NO_MATCH
	Nie znaleziono pasuj±cych wpisów na li¶cie SZLINE.

OPER_SZLINE_DELETED
	%s usuniêto z listy SZLINE.

OPER_SZLINE_DELETED_ONE
	Usuniêto 1 wpis z listy SZLINE.

OPER_SZLINE_DELETED_SEVERAL
	Usuniêto %d wpisy(ów) z listy SZLINE.

OPER_SZLINE_LIST_EMPTY
	Lista SZLINE jest pusta.

OPER_SZLINE_LIST_HEADER
	Aktualna lista SZLINE:
	  Nr    Maska                             Powód

OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s

OPER_SZLINE_VIEW_HEADER
	Aktualna lista SZLINE:

# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (przez %s na %s; %s)
	      %s

OPER_SZLINE_CLEAR
	Lista SZLINE zosta³a wyczyszczona.

# SET responses
OPER_SET_SYNTAX
	SET opcja ustawienie

OPER_SET_IGNORE_ON
	Opcja IGNORE zosta³a w³±czona.

OPER_SET_IGNORE_OFF
	Opcja IGNORE zosta³a wy³±czona.

OPER_SET_IGNORE_ERROR
	Parametrem opcji IGNORE musi byæ ON albo OFF.

OPER_SET_READONLY_ON
	Serwisy s± teraz w trybie tylko-do-odczytu.

OPER_SET_READONLY_OFF
	Serwisy s± teraz w trybie odczyt-zapis.

OPER_SET_READONLY_ERROR
	Parametrem opcji READONLY musi byæ ON albo OFF.

OPER_SET_LOGCHAN_ON
	Serwisy bêd± wysy³aæ wiadomo¶ci na kana³ %s.

OPER_SET_LOGCHAN_OFF
	Serwisy nie bêd± ju¿ d³u¿ej wysy³aæ wiadomo¶ci na kana³.

OPER_SET_LOGCHAN_ERROR
	Parametrem dla LOGCHAN musi byæ ON albo OFF.
	Ponadto kana³ logowania musi byæ zdefiniowany w pliku konfiguracyjnym.

OPER_SET_DEBUG_ON
	Serwisy s± teraz w trybie debugowania.

OPER_SET_DEBUG_OFF
	Serwisy nie s± ju¿ w trybie debugowania.

OPER_SET_DEBUG_LEVEL
	Serwisy s± teraz w trybie debugowania (poziom %d).

OPER_SET_DEBUG_ERROR
	Parametrem opcji DEBUG musi byæ ON, OFF albo liczba dodatnia.

OPER_SET_NOEXPIRE_ON
	Serwisy nie s± ju¿ w trybie wygasania.

OPER_SET_NOEXPIRE_OFF
	Serwisy s± teraz w trybie wygasania.

OPER_SET_NOEXPIRE_ERROR
	Parametrem opcji NOEXPIRE musi byæ ON albo OFF.

OPER_SET_UNKNOWN_OPTION
	Nieznana opcja %s.

OPER_SET_SQL_ON
	Obs³uga SQL zosta³a w³±czona.

OPER_SET_SQL_OFF
	Obs³uga SQL zosta³a wy³±czona.

OPER_SET_SQL_ERROR
	Parametrem opcji SQL musi byæ ON albo OFF.

OPER_SET_SQL_ERROR_INIT
	B³±d podczas inicjacji SQL, sprawd¼ logi.

OPER_SET_SQL_ERROR_DISABLED
	SQL nie jest skonfigurowany dla tej sieci.
	Wprowad¼ zmiany w pliku konfiguracyjnym.

OPER_SET_LIST_OPTION_ON
	%s jest w³±czone

OPER_SET_LIST_OPTION_OFF
	%s jest wy³±czone

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

OPER_JUPE_INVALID_SERVER
	Nie mo¿na wykluczyæ samych serwisów lub uplinka serwisów.

# RAW responses
OPER_RAW_SYNTAX
	RAW tekst

# UPDATE responses
OPER_UPDATING
	Aktualizacja baz danych.

# RELOAD responses
OPER_RELOAD
	Plik konfiguracyjny serwisów zosta³ prze³adowany.

# RESTART responses
OPER_CANNOT_RESTART
	Nie zdefiniowano SERVICES_BIN; nie mogê restartowaæ.
	Uruchom ponownie skrypt configure i przekompiluj serwisy, aby w³±czyæ komendê RESTART.

# IGNORE respondes
OPER_IGNORE_SYNTAX
	IGNORE {ADD|DEL|LIST|CLEAR} [czas] [nick | maska]

OPER_IGNORE_VALID_TIME
	Musisz podaæ prawid³ow± liczbê dla czasu.

OPER_IGNORE_TIME_DONE
	Nick %s bêdzie ignorowany przez %s.

OPER_IGNORE_PERM_DONE
	Nick %s bêdzie stale ignorowany.

OPER_IGNORE_DEL_DONE
	Nick %s nie bêdzie ignorowany.

OPER_IGNORE_LIST
	Lista ignorowanych przez serwisy:

OPER_IGNORE_LIST_NOMATCH
	%s nie znaleziono na li¶cie ignorowanych.

OPER_IGNORE_LIST_EMPTY
	Lista ignorowanych jest pusta.

OPER_IGNORE_LIST_CLEARED
	Lista ignorowanych zosta³a wyczyszczona.

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

OPER_SUPER_ADMIN_ON
	Jeste¶ teraz super-administratorem.

OPER_SUPER_ADMIN_OFF
	Nie jeste¶ ju¿ d³u¿ej super-administratorem.

OPER_SUPER_ADMIN_SYNTAX
	Parametrem dla opcji SuperAdmin musi byæ ON lub OFF (musi byæ w³±czona w pliku konfiguracyjnym)

OPER_SUPER_ADMIN_WALL_ON
	%s jest teraz super-administratorem

OPER_SUPER_ADMIN_WALL_OFF
	%s nie jest ju¿ d³u¿ej super-administratorem

OPER_SUPER_ADMIN_ONLY
	Tylko super-administratorzy mog± u¿ywaæ tej komendy.

OPER_STAFF_LIST_HEADER
	 Na Poziom Nick

OPER_STAFF_FORMAT
	 %c %s  %s

OPER_STAFF_AFORMAT
	 %c %s  %s [%s]

# DefCon Messages
OPER_DEFCON_SYNTAX
	DEFCON [1|2|3|4|5]

OPER_DEFCON_DENIED
	Serwisy s± w trybie defcon, spróbuj ponownie pó¼niej.

OPER_DEFCON_NO_CONF
	System defcon musi byæ w³±czony w pliku konfiguracyjnym.

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
	Nie mo¿na za³adowaæ modu³u %s

OPER_MODULE_REMOVE_FAIL
	Nie mo¿na wy³adowaæ modu³u %s

OPER_MODULE_NO_UNLOAD
	Ten modu³ nie mo¿e zostaæ wy³adowany.

OPER_MODULE_LOAD_SYNTAX
	MODLOAD nazwa-pliku

OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD nazwa-pliku

OPER_MODULE_LIST_HEADER
	Aktualna lista modu³ów:

OPER_MODULE_LIST
	Modu³: %s [%s] [%s]

OPER_MODULE_LIST_FOOTER
	Za³adowanych modu³ów: %d.

OPER_MODULE_INFO_LIST
	Modu³: %s Wersja: %s Autor: %s Za³adowano: %s

OPER_MODULE_CMD_LIST
	Dodane komendy: %R%s %s

OPER_MODULE_MSG_LIST
	Zapewnia IRCD handler dla : %s

OPER_MODULE_NO_LIST
	Aktualnie nie ma za³adowanych modu³ów.

OPER_MODULE_NO_INFO
	Informacje o module %s nie s± dostêpne.

OPER_MODULE_INFO_SYNTAX
	MODINFO nazwa-pliku

MODULE_HELP_HEADER
	Nastêpuj±ce komendy zosta³y dodane przez modu³:

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################
# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [parametry]

OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+czas-trwania] maska limit powód

OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {maska | lista}

OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE numer pozycja

OPER_EXCEPTION_DISABLED
	Limitowanie sesji jest wy³±czone.

OPER_EXCEPTION_ALREADY_PRESENT
	Maska %s ju¿ istnieje na li¶cie wyj±tków.

OPER_EXCEPTION_TOO_MANY
	Lista wyj±tków limitów sesji jest pe³na!

OPER_EXCEPTION_ADDED
	Limit sesji dla %s zmieniono na %d.

OPER_EXCEPTION_MOVED
	Wyj±tek dla %s (#%d) przeniesiono na pozycjê %d.

OPER_EXCEPTION_NO_SUCH_ENTRY
	Nie znaleziono wpisu (#%d) na li¶cie wyj±tków limitów sesji.

OPER_EXCEPTION_NOT_FOUND
	%s nie znaleziono na li¶cie wyj±tków limitów sesji.

OPER_EXCEPTION_NO_MATCH
	Nie znaleziono wpisów na li¶cie wyj±tków limitów sesji.

OPER_EXCEPTION_DELETED
	%s usuniêto z listy wyj±tków limitów sesji.

OPER_EXCEPTION_DELETED_ONE
	Usuniêto 1 wpis z listy wyj±tków limitów sesji.

OPER_EXCEPTION_DELETED_SEVERAL
	Usuniêto %d wpisy(ów) z listy wyj±tków limitów sesji.

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
	Nieprawid³owy limit sesji. Musi byæ liczb± nieujemn± i mniejsz± ni¿ %d.

OPER_EXCEPTION_INVALID_HOSTMASK
	Nieprawid³owa maska. Tylko rzeczywiste maski s± prawid³owe jako wyj±tki.

OPER_EXCEPTION_EXISTS
	%s ju¿ istnieje na li¶cie wyj±tków.

OPER_EXCEPTION_CHANGED
	Wyj±tek dla %s zosta³ zaktualizowany do %d.

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
	Nieprawid³owa warto¶æ progu. Musi byæ liczb± ca³kowit± wiêksz± ni¿ 1.

OPER_SESSION_NOT_FOUND
	%s nie znaleziono na li¶cie sesji.

OPER_SESSION_LIST_HEADER
	Hosty z przynajmniej %d sesjami:

OPER_SESSION_LIST_COLHEAD
	Sesje   Host

# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s

# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	Host %s aktualnie posiada %d sesji z limitem %d.

# EXCEPTION help
OPER_HELP_EXCEPTION
	Sk³adnia: EXCEPTION ADD [+czas-trwania] maska limit powód
	          EXCEPTION DEL {maska | lista}
	          EXCEPTION MOVE numer pozycja
	          EXCEPTION LIST [maska | lista]
	          EXCEPTION VIEW [maska | lista]
	
	Pozwala administratorom serwisów na zarz±dzanie list± limitów
	sesji dla poszczególnych hostów - np. serwery shell mog±
	mieæ wiêksz± liczbê po³±czeñ. Kiedy limit po³±czeñ zostanie
	osi±gniêty ka¿dy nowy u¿ytkownik z takiego hosta bêdzie
	roz³±czany z sieci±. Przed roz³±czeniem zostanie wys³ane
	powiadomienie (jego tre¶æ ustawia siê w pliku konfiguracyjnym).
	
	EXCEPTION ADD dodaje wskazan± maskê na listê wyj±tków.
	Maski nick!user@host oraz user@host s± nieprawid³owe,
	poniewa¿ zawieraj± nick lub ident u¿ytkownika. Dopuszczalne
	maski to np. box.host.dom lub *.host.dom. Limit musi
	byæ liczb± nieujemn±. Podanie zera oznacza wy³±czenie limitu.
	Format czasu jest opisany w pomocy polecenia AKILL.
	EXCEPTION DEL usuwa wskazane wpisy z listy wyj±tków.
	EXCEPTION MOVE przenosi wyj±tek o wskazanym numerze na
	now± pozycjê. Wyj±tki znajduj±ce siê pomiêdzy zostan±
	odpowiednio przeniesione, aby wype³niæ powsta³± lukê.
	EXCEPTION LIST oraz EXCEPTION VIEW listuj± wyj±tki.
	Podanie maski powoduje ograniczenie wy¶wietlanych wpisów.
	EXCEPTION VIEW zwraca dodatkowo informacje o zak³adaj±cym,
	limicie, powodzie, masce oraz czasie.
	
	Nale¿y pamiêtaæ, ¿e pod³±czaj±cy siê klient bêdzie u¿ywa³
	pierwszego wyj±tku o pasuj±cej masce. Du¿a lista wyj±tków i
	zbyt ogólne maski powoduj± pogorszenie wydajno¶ci serwisów.

OPER_HELP_SESSION
	Sk³adnia: SESSION LIST minimum
	          SESSION VIEW host
	
	Pozwala administratorom serwisów na listowanie sesji.
	
	SESSION LIST listuje hosty z przynajmniej minimum sesjami.
	Minimum musi byæ wiêksze od 1, aby zapobiec przypadkowemu
	listowaniu du¿ej liczby pojedynczych hostów.
	SESSION VIEW wy¶wietla szczegó³owe informacje na temat 
	podanego hosta - podaj±c aktualny limit oraz ilo¶æ sesji.
	Parametr host mo¿e zawieraæ symboli wieloznacznych.
	
	Wiêcej informacji na temat ograniczania liczby sesji
	znajduje siê w pomocy polecenia EXCEPTION.

OPER_HELP_STAFF
	Sk³adnia: STAFF
	
	Wy¶wietla nicki obs³ugi serwisów, poziom i status online.

OPER_HELP_DEFCON
	Sk³adnia: DEFCON [1|2|3|4|5]
	
	System defcon umo¿liwia wprowadzanie wcze¶niej
	zdefiniowanych ograniczeñ dla serwisów podczas
	próby ataku na sieæ.
OPER_HELP_DEFCON_NO_NEW_CHANNELS
	* Wy³±cza rejestrowanie kana³ów

OPER_HELP_DEFCON_NO_NEW_NICKS
	* Wy³±cza rejestrowanie nicków

OPER_HELP_DEFCON_NO_MLOCK_CHANGE
	* Flagi kana³ów nie bêd± wymuszane (MLOCK)

OPER_HELP_DEFCON_FORCE_CHAN_MODES
	* Wymusza ustawienie flag (%s) na wszystkich kana³ach

OPER_HELP_DEFCON_REDUCE_SESSION
	* U¿ywa zredukowanego limitu sesji: %d

OPER_HELP_DEFCON_NO_NEW_CLIENTS
	* Killuje wszystkich nowych klientów

OPER_HELP_DEFCON_OPER_ONLY
	* Ignoruje zwyk³ych u¿ytkowników (informuje ich o tym)

OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* Cicho ignoruje zwyk³ych u¿ytkowników

OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* AKILLuje wszystkich nowych klientów

OPER_HELP_DEFCON_NO_NEW_MEMOS
	* Nowe wiadomo¶ci nie bêd± wysy³ane

OPER_HELP_CHANKILL
	Sk³adnia: CHANKILL [+czas] kana³ powód
	
	Zak³ada AKILLa na ka¿dy nick na wskazanym kanale.
	Na listê zostaj± dodane prawdziwe adresy (ident@host),
	po czym AKILL zostaje wykonany.
###########################################################################
#
# News system messages
#
###########################################################################
NEWS_LOGON_TEXT
	[Wiadomo¶æ powitalna - %s] %s

NEWS_OPER_TEXT
	[Wiadomo¶æ dla operatorów - %s] %s

NEWS_RANDOM_TEXT
	[Losowa wiadomo¶æ - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD|DEL|LIST} [tekst|numer]

NEWS_LOGON_LIST_HEADER
	Wiadomo¶ci powitalne:

NEWS_LOGON_LIST_ENTRY
	%5d (%s przez %s)
	    %s

NEWS_LOGON_LIST_NONE
	Nie ma wiadomo¶ci powitalnych.

NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD tekst

NEWS_LOGON_ADD_FULL
	Lista wiadomo¶ci powitalnych jest pe³na!

NEWS_LOGON_ADDED
	Dodano now± wiadomo¶æ powitaln± (#%d).

NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {numer | ALL}

NEWS_LOGON_DEL_NOT_FOUND
	Wiadomo¶æ powitalna #%d nie zosta³a znaleziona!

NEWS_LOGON_DELETED
	Wiadomo¶æ powitalna #%d zosta³a skasowana.

NEWS_LOGON_DEL_NONE
	Nie ma wiadomo¶ci powitalnych do skasowania!

NEWS_LOGON_DELETED_ALL
	Wszystkie wiadomo¶ci powitalne zosta³ skasowane.

NEWS_OPER_SYNTAX
	OPERNEWS {ADD|DEL|LIST} [tekst|numer]

NEWS_OPER_LIST_HEADER
	Wiadomo¶ci dla operatorów:

NEWS_OPER_LIST_ENTRY
	%5d (%s przez %s)
	    %s

NEWS_OPER_LIST_NONE
	Nie ma wiadomo¶ci dla operatorów.

NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD tekst

NEWS_OPER_ADD_FULL
	Lista wiadomo¶ci jest pe³na!

NEWS_OPER_ADDED
	Dodano now± wiadomo¶æ dla operatorów (#%d).

NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {numer | ALL}

NEWS_OPER_DEL_NOT_FOUND
	Wiadomo¶æ dla operatorów #%d nie zosta³a znaleziona!

NEWS_OPER_DELETED
	Wiadomo¶æ dla operatorów #%d zosta³a skasowana.

NEWS_OPER_DEL_NONE
	Nie ma wiadomo¶ci dla operatorów do skasowania!

NEWS_OPER_DELETED_ALL
	Wszystkie wiadomo¶ci dla operatorów zosta³y skasowane.

NEWS_RANDOM_SYNTAX
	RANDOMNEWS {ADD|DEL|LIST} [tekst|numer]

NEWS_RANDOM_LIST_HEADER
	Losowe wiadomo¶ci:

NEWS_RANDOM_LIST_ENTRY
	%5d (%s przez %s)
	    %s

NEWS_RANDOM_LIST_NONE
	Nie ma losowych wiadomo¶ci.

NEWS_RANDOM_ADD_SYNTAX
	RANDOMNEWS ADD tekst

NEWS_RANDOM_ADD_FULL
	Lista wiadomo¶ci jest pe³na!

NEWS_RANDOM_ADDED
	Dodano now± losow± wiadomo¶æ (#%d).

NEWS_RANDOM_DEL_SYNTAX
	RANDOMNEWS DEL {numer | ALL}

NEWS_RANDOM_DEL_NOT_FOUND
	Losowa wiadomo¶æ #%d nie zosta³a znaleziona!

NEWS_RANDOM_DELETED
	Losowa wiadomo¶æ #%d zosta³a skasowana.

NEWS_RANDOM_DEL_NONE
	Nie ma losowych wiadomo¶ci do skasowania!

NEWS_RANDOM_DELETED_ALL
	Wszystkie losowe wiadomo¶ci zosta³y skasowane.

NEWS_HELP_LOGON
	Sk³adnia: LOGONNEWS ADD tekst
	          LOGONNEWS DEL {numer | ALL}
	          LOGONNEWS LIST
	
	Edytuje lub wy¶wietla listê wiadomo¶ci wysy³anych po
	przy³±czeniu u¿ytkownika do sieci. Je¶li zdefniowanych
	wiadomo¶ci jest wiêcej tylko najnowsze %s zostanie
	wys³anych (aby zapobiec floodowaniu). Warto¶æ NewsCount
	mo¿e zostaæ zmieniona w pliku konfiguracyjym serwisów.
	
	LOGONNEWS may only be used by Services admins.

NEWS_HELP_OPER
	Sk³adnia: OPERNEWS ADD tekst
	          OPERNEWS DEL {numer | ALL}
	          OPERNEWS LIST
	
	Edytuje lub wy¶wietla listê wiadomo¶ci wysy³anych po
	wykonaniu polecenia /OPER. Je¶li zdefiniowanych
	wiadomo¶ci jest wiêcej tylko najnowsze %s zostanie
	wys³anych (aby zapobiec floodowaniu). Warto¶æ NewsCount
	mo¿e zostaæ zmieniona w pliku konfiguracyjym serwisów.
	
	OPERNEWS may only be used by Services admins. 

NEWS_HELP_RANDOM
	Sk³adnia: RANDOMNEWS ADD tekst
	          RANDOMNEWS DEL {numer | ALL}
	          RANDOMNEWS LIST
	
	Edytuje lub wy¶wietla listê losowych newsów.
	Po po³±czeniu z sieci± u¿ytkownik otrzyma dok³adnie
	jedn± wylosowan± z tej listy wiadomo¶æ.
	
	RANDOMNEWS may only be used by Services admins. 

###########################################################################
#
# HelpServ help message
#
###########################################################################
HELP_HELP
	%S jest serwisem udzielaj±cym informacji
	na temat innych serwisów. Dostêpne tematy pomocy:
	
	
	%R%s HELP
	     rejestracja i zarz±dzanie nickami
	
	%R%s HELP
	     rejestracja i zarz±dzanie kana³ami
	
	%R%s HELP
	     wysy³anie wiadomo¶ci do u¿ytkowników off-line

HELP_HELP_BOT
	%R%s HELP
	     stawianie serwisowego bota na kanale

HELP_HELP_HOST
	%R%s HELP
	     ustawianie vhostów

###########################################################################
#
# NickServ help messages
#
###########################################################################
NICK_HELP_CMD_CONFIRM
	    CONFIRM     Potwierdza rejestracjê nicka

NICK_HELP_CMD_RESEND
	    RESEND      Wysy³a ponownie kod autoryzuj±cy

NICK_HELP_CMD_REGISTER
	    REGISTER    Rejestruje nicka

NICK_HELP_CMD_GROUP
	    GROUP       Do³±cza nick do wskazanej grupy

NICK_HELP_CMD_IDENTIFY
	    IDENTIFY    Identyfikuje w³a¶ciciela nicka

NICK_HELP_CMD_ACCESS
	    ACCESS      Zarz±dza list± dostêpu

NICK_HELP_CMD_SET
	    SET         Modyfikuje ustawienia nicka

NICK_HELP_CMD_SASET
	    SASET       Ustawia opcje wskazanemu nickowi

NICK_HELP_CMD_DROP
	    DROP        Usuwa nicka

NICK_HELP_CMD_RECOVER
	    RECOVER     Od³±cza u¿ytkownika, który u¿ywa Twojego nicka

NICK_HELP_CMD_RELEASE
	    RELEASE     Zwalnia nick po u¿yciu komendy RECOVER

NICK_HELP_CMD_SENDPASS
	    SENDPASS    Pomaga odzyskaæ utracone has³o

NICK_HELP_CMD_GHOST
	    GHOST       Od³±cza wskazany nick od serwera

NICK_HELP_CMD_ALIST
	    ALIST       Listuje kana³y na których masz uprawnienia

NICK_HELP_CMD_GLIST
	    GLIST       Wy¶wietla wszystkie nicki w Twojej grupie

NICK_HELP_CMD_INFO
	    INFO        Pokazuje informacje o zarejestrowanym nicku

NICK_HELP_CMD_LIST
	    LIST        Listuje zarejestrowane nicki pasuj±ce do wzorca

NICK_HELP_CMD_LOGOUT
	    LOGOUT      Odwraca dzia³anie komendy IDENTIFY

NICK_HELP_CMD_STATUS
	    STATUS      Podaje status w³a¶ciciela podanego nicka

NICK_HELP_CMD_UPDATE
	    UPDATE      Aktualizuje twój obecny status np.
	                sprawdza nowe wiadomo¶ci

NICK_HELP_CMD_GETPASS
	    GETPASS     Podaje has³o do wskazanego nicka

NICK_HELP_CMD_GETEMAIL
	    GETEMAIL    Wy¶wietla u¿ytkowników zarejestrowanych z
	                podanym adresem e-mail

NICK_HELP_CMD_FORBID
	    FORBID      Blokuje podany nick

NICK_HELP_CMD_SUSPEND
	    SUSPEND     Blokuje podany nick z zachowaniem
	                wszelkich jego danych i ustawieñ

NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND   Uwalnia zawieszonego nicka

NICK_HELP
	%S umo¿liwia rejestracjê nicka i zapewnia
	wy³±czno¶æ na jego u¿ywanie. Poni¿sze komendy
	umo¿liwiaj± zarz±dzanie nickiem, aby ich u¿yæ wpisz:
	%R%S komenda
	Aby uzyskaæ szczegó³owy opis polecenia wpisz:
	%R%S HELP komenda
	
	

NICK_HELP_FOOTER
	
	UWAGA: Ten serwis ma na celu umo¿liwienie identyfikacji
	u¿ytkowników. NIE s³u¿y do kradzie¿y nicków czy innych
	z³o¶liwych akcji. Nadu¿ycia bêd± skutkowa³y co najmniej
	utrat± zarejestrowanego nicka.
	

NICK_HELP_EXPIRES
	
	Nicki nieu¿ywane przez d³u¿szy czas s± automatycznie
	usuwane po %d dniach od ostatniej identyfikacji.
	Wszelkie ustawienia oraz uprawnienia zostaj± usuniête.

NICK_HELP_REGISTER
	Sk³adnia: REGISTER has³o [email]
	
	Rejestruje aktualnie u¿ywany nick w bazie %S.
	Po rejestracji mo¿esz u¿yæ komend SET i ACCESS
	do konfiguracji niektórych opcji nicka wedle upodobañ.
	Upewnij siê, ¿e pamiêtasz has³o rejestracyjne, bo 
	bêdzie ono potrzebne w przysz³o¶ci do zmiany
	ustawieñ i identyfikacji. Wielko¶æ liter w ha¶le 
	ma znaczenie!!!.
	
	Jak wybraæ has³o?
	
	Has³o musi byæ trudne do odgadniêcia. U¿ywanie imion
	jako hase³ jest niewskazane. Bardzo krótkie has³o
	mo¿e zostaæ odgadniête metod± prób i b³êdów.
	Twoje has³o musi mieæ d³ugo¶æ co najmniej 5 znaków.
	Znak spacji nie mo¿e byæ u¿yty w ha¶le.
	
	Parametr email jest opcjonalny i od razu przy 
	rejestracji ustawia adres e-mail dla Twojego nicka. 
	Mo¿e siê zdarzyæ, ¿e adres e-mail mo¿e byæ wymagany 
	na niektórych sieciach. Twoja prywatno¶æ jest
	chroniona - adres nie bêdzie udostêpniany osobom
	trzecim.
	
	Wraz z rejestracj± zostaje utworzona grupa.
	Nicki z tej samej grupy maj± wspólne ustawienia,
	prawa na kanale oraz ustawienia wiadomo¶ci.
	Aby uzyskaæ wiêcej informacji napisz: 
	%R%S HELP GROUP.
	

NICK_HELP_GROUP
	Sk³adnia: GROUP nazwa has³o
	
	Ta komenda powoduje dodanie Twojego nicka do grupy
	reprezentowanej przez nick nazwa o ha¶le has³o.
	
	Grupowanie nicków pozwala na dzielenie siê ustawieniami,
	wiadomo¶ciami oraz uprawnieniami z innymi nickami z grupy.
	
	Je¶li jaki¶ nick z grupy zostanie usuniêty nie tracisz
	¿adnych ustawieñ czy uprawnieñ, jednak grupa istnieje
	dopóki jest w niej przynajmniej jeden nick.
	
	Komenda mo¿e byæ u¿yta nawet wtedy kiedy bie¿±cy nick
	nie jest zarejestrowany, w przeciwnym wypadku musisz
	siê zidentyfikowaæ. Aby uzyskaæ wiêcej informacji wpisz:
	%R%S HELP IDENTIFY
	
	Mo¿liwe i zalecane jest u¿ywanie tej komendy na nickach,
	które nie s± zarejestrowane, zostan± one wtedy automatycznie
	dodane do bazy %S.
	
	UWAGA: Komendê mo¿na u¿yæ tak¿e na zarejestrowanym nicku,
	nale¿y jednak wzi±æ pod uwagê to, ¿e je¶li bie¿±cy nick jest
	ostatni ze swojej grupy, to wszystkie jego ustawienia oraz
	uprawnienia zostan± usuniête! Z tego powodu najlepiej grupowaæ
	nowy nick ze starym, a nie na odwrót. Zmiana grupy mo¿e byæ
	wy³±czona przez administratora w niektórych sieciach.
	
	Mo¿na byæ tylko w jednej grupie w danej chwili,
	³±czenie grup nie jest mo¿liwe.
	
	Uwaga: wszystkie nicki w grupie maj± to samo has³o.

NICK_HELP_IDENTIFY
	Sk³adnia: IDENTIFY has³o
	
	Informuje %S, ¿e nick którego w³a¶nie u¿ywasz 
	nale¿y do Ciebie. Wiele komend wymaga identyfikacji 
	przed ich u¿yciem. Has³em jest to samo has³o, 
	którego u¿yto przy komendzie REGISTER.

NICK_HELP_UPDATE
	Sk³adnia: UPDATE
	
	Aktualizuje twój obecny status, np. sprawdza czy s± nowe
	wiadomo¶ci, ustawia odpowiednie uprawnienia na kana³ach,
	aktualizuje vhosta oraz flagi u¿ytkownika.

NICK_HELP_LOGOUT
	Sk³adnia: LOGOUT
	
	Odwraca dzia³anie komendy IDENTIFY. Po wydaniu
	tego polecenia nie bêdziesz rozpoznawany jako
	w³a¶ciciel nicka.
NICK_HELP_DROP
	Sk³adnia: DROP [nick]
	
	Usuwa nick z bazy danych %S. Usuniêty nick
	mo¿e zostaæ ponownie zarejestrowany przez ka¿dego.
	
	Mo¿esz usun±æ nick nale¿±cy do Twojej grupy podaj±c
	go jako dodatkowy parametr nick.
	
	U¿ycie tej komendy wymaga identyfikacji z u¿yciem
	has³a. Aby uzyskaæ wiêcej informacji wpisz:
	%R%S HELP IDENTIFY

NICK_HELP_ACCESS
	Sk³adnia: ACCESS ADD maska
	          ACCESS DEL maska
	          ACCESS LIST
	
	Modyfikuje lub wy¶wietla listê dostêpu dla nicka.
	Lista dostêpu okre¶la hosty u¿ytkowników mog±cych
	u¿ywaæ twojego nicka. Je¶li chcesz u¿ywaæ nicka z
	innego hosta musisz siê najpierw zidentyfikowaæ
	w serwisie %S komend± IDENTIFY.
	
	Przyk³ady:
	
	    ACCESS ADD userek@ircnet.pl
	        Zezwala na dostêp do nicka ka¿demu 
	        ³±cz±cemu siê z domeny ircnet.pl.
	
	    ACCESS DEL userek@ircnet.pl
	        Odwraca efekt poprzedniej komendy.
	
	    ACCESS LIST
	        Wy¶wietla aktualn± listê dostêpu.

NICK_HELP_SET
	Sk³adnia: SET opcja parametry
	
	Ustawia ró¿ne opcje nicka. Dostêpne opcje to:
	
	    DISPLAY    Ustawia nick reprezentuj±cy grupê nicków
	    PASSWORD   Ustawia has³o do nicka
	    LANGUAGE   Ustawia jêzyk w którym serwisy bêd±
	               wysy³a³y komunikaty
	    URL        Przypisuje adres www do nicka
	    EMAIL      Przypisuje adres e-mail do nicka
	    ICQ        Przypisuje numer ICQ do nicka
	    GREET      Przypisuje komunikat powitalny do nicka
	    KILL       W³±cza lub wy³±cza ochronê nicka
	    SECURE     W³±cza lub wy³±cza bezpieczeñstwo nicka
	    PRIVATE    Zapobiega wy¶wietlaniu nicka po wydaniu
	               polecenia: %R%S LIST
	    HIDE       Ukrywa poszczególne informacje o nicku
	    MSG        Zmienia sposób komunikacji serwisów
	    AUTOOP     W³±cza lub wy³±cza automatyczne opowanie
	
	Aby u¿yæ tej komendy musisz siê zidentyfikowaæ z u¿yciem
	Twojego has³a. Aby uzyskaæ wiêcej informacji wpisz:
	%R%S HELP IDENTIFY
	
	Aby uzyskaæ wiêcej informacji o danej opcji wpisz:
	%R%S HELP SET opcja

NICK_HELP_SET_DISPLAY
	Sk³adnia: SET DISPLAY nowa-nazwa
	
	Zmienia nazwê reprezentuj±c± Twoj± grupê nicków w
	serwisach na now±. Nazwa MUSI byæ jednym z nicków
	nale¿±cych do Twojej grupy.

NICK_HELP_SET_PASSWORD
	Sk³adnia: SET PASSWORD nowe-has³o
	
	Zmienia Twoje has³o do nicka.

NICK_HELP_SET_LANGUAGE
	Sk³adnia: SET LANGUAGE numer
	
	Zmienia jêzyk jakiego u¿ywaj± serwisy odpowiadaj±c 
	Tobie (czyli przyk³adowo kiedy odpowiadaj± na 
	wys³an± przez Ciebie komendê). Numer mo¿e byæ 
	wybrany z poni¿szej listy obs³ugiwanych jêzyków:

NICK_HELP_SET_URL
	Sk³adnia: SET URL adres
	
	Przypisuje podany adres strony www nickowi.
	Ten adres bêdzie siê pokazywa³ w opisie Twojego
	nicka dostêpnym po u¿yciu polecenia INFO.

NICK_HELP_SET_EMAIL
	Sk³adnia: SET EMAIL adres
	
	Przypisuje adres e-mail Twojemu nickowi. Adres
	bêdzie wy¶wietlany przy opisie Twojego nicka
	dostêpnym przy u¿yciu komendy INFO.

NICK_HELP_SET_ICQ
	Sk³adnia: SET ICQ numer
	
	Przypisuje numer ICQ Twojemu nickowi, który
	bêdzie wy¶wietlany przy opisie Twojego nicka
	dostêpnym przy u¿yciu komendy INFO.
	

NICK_HELP_SET_GREET
	Sk³adnia: SET GREET wiadomo¶æ
	
	Ustawia wiadomo¶æ powitaln± dla Twojego nicka.
	Je¶li kana³ ma przydzielonego bota, w³±czone witanie
	u¿ytkowników oraz masz odpowiedni poziom dostêpu to
	wtedy ta wiadomo¶æ bêdzie wysy³ana na kana³ przez bota.

NICK_HELP_SET_KILL
	Sk³adnia: SET KILL {ON | QUICK | IMMED | OFF}
	
	W³±cza lub wy³±cza automatyczn± ochronê dla nicka.
	U¿ytkownicy wchodz±cy na chroniony nick otrzymuj±
	informacje od %S, ¿e nick jest zarejestrowany i
	je¶li siê nie zidentyfikuj± lub go nie zmieni±
	w okre¶lonym czasie to ich nick zostanie zmieniony
	si³± lub zostan± od³±czeni od sieci.
	
	Opcja QUICK skraca czas oczekiwania na zmianê
	z 60 sekund do 20. W przypadku opcji IMMED
	nick zostanie zmieniony natychmiast bez mo¿liwo¶ci
	identyfikacji (wstêpna identyfikacja nastêpuje
	na podstawie listy dostêpu, dlatego t± opcjê
	nale¿y stosowaæ z rozwag±). Administrator sieci 
	mo¿e wy³±czyæ korzystanie z IMMED.
NICK_HELP_SET_SECURE
	Sk³adnia: SET SECURE {ON | OFF}
	
	W³±cza lub wy³±cza opcjê bezpieczeñstwa dla nicka.
	W³±czenie SECURE wymusza na u¿ytkowniku podanie
	has³a zanim ten zostanie rozpoznany jako w³a¶ciciel
	nicka. %S nie bierze pod uwagê w tym wypadku
	wystêpowania adresu li¶cie dostêpu. Je¶li adres 
	znajduje siê na li¶cie %S nie skilluje
	u¿ytkownika niezale¿nie od ustawienia opcji KILL.

NICK_HELP_SET_PRIVATE
	Sk³adnia: SET PRIVATE {ON | OFF}

	W³±cza lub wy³±cza opcjê PRIVATE dla nicka.
	Kiedy ta opcja jest w³±czona nick nie bêdzie
	umieszczany na listach wy¶wietlanych komend±:
	%R%S LIST
	Jednak ka¿dy, kto zna nick mo¿e uzyskaæ o nim
	informacje u¿ywaj±c polecenia INFO.

NICK_HELP_SET_HIDE
	Sk³adnia: SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	To polecenie pozwala na ukrycie czê¶ci informacji, które
	s± wy¶wietlane przez polecenie INFO serwisu %s.
	Pierwszy argument okre¶la ukrywan± informacjê:
	EMAIL - adres e-mail, USERMASK - ostatnio u¿ywany host,
	QUIT - ostatnia wiadomo¶æ po¿egnalna, STATUS - status
	dostêpu do serwisów. Drugi argument okre¶la, czy dana 
	informacja bêdzie pokazywana (OFF), czy ukrywana (ON).
NICK_HELP_SET_MSG
	Sk³adnia: SET MSG {ON | OFF}
	
	Pozwala wybraæ sposób w jaki serwisy bêd± pisaæ do
	Ciebie. Kiedy ta opcja jest w³±czona serwisy bêd±
	wysy³aæ wiadomo¶ci prywatne (query), w przeciwnym
	wypadku bêd± wysy³aæ powiadomienia (notice).

NICK_HELP_SET_AUTOOP
	Sk³adnia: SET AUTOOP {ON | OFF}
	
	W³±cza lub wy³±cza automatyczne nadawanie uprawnieñ
	przy wchodzeniu na kana³.

NICK_HELP_SASET
	Sk³adnia: SASET nick opcja parametry.
	
	Ustawia ró¿ne opcje nicka wskazanemu u¿ytkownikowi.
	Dostêpne opcje to:
	
	    DISPLAY    Ustawia nick reprezentuj±cy grupê nicków
	    PASSWORD   Ustawia has³o do nicka
	    LANGUAGE   Ustawia jêzyk w którym serwisy bêd±
	               wysy³a³y komunikaty
	    URL        Przypisuje adres www do nicka
	    EMAIL      Przypisuje adres e-mail do nicka
	    ICQ        Przypisuje numer ICQ do nicka
	    GREET      Przypisuje komunikat powitalny do nicka
	    KILL       W³±cza lub wy³±cza ochronê nicka
	    SECURE     W³±cza lub wy³±cza bezpieczeñstwo nicka
	    PRIVATE    Zapobiega wy¶wietlaniu nicka po wydaniu
	               polecenia: %R%S LIST
	    HIDE       Ukrywa poszczególne informacje o nicku
	    MSG        Zmienia sposób komunikacji serwisów
	    AUTOOP     W³±cza lub wy³±cza automatyczne opowanie
	    NOEXPIRE   Zapobiega wyga¶niêciu nicka
	
	Aby uzyskaæ wiêcej informacji o danej opcji wpisz:
	%R%S HELP SASET opcja

NICK_HELP_SASET_DISPLAY
	Sk³adnia: SASET nick DISPLAY nowa-nazwa
	
	Zmienia nazwê reprezentuj±c± grupê nicków wskazanego
	u¿ytkownika na now±. Nazwa MUSI byæ jednym z nicków
	nale¿±cych do tej grupy.

NICK_HELP_SASET_PASSWORD
	Sk³adnia: SASET nick PASSWORD nowe-has³o
	
	Zmienia has³o wskazanemu u¿ytkownikowi.

NICK_HELP_SASET_URL
	Sk³adnia: SASET nick URL url
	
	Przypisuje podany adres www wskazanemu u¿ytkownikowi.
	Ten adres bêdzie siê pokazywa³ w opisie jego nicka
	dostêpnym po u¿yciu polecenia INFO.

NICK_HELP_SASET_EMAIL
	Sk³adnia: SASET nick EMAIL adres
	
	Przypisuje wskazanemu u¿ytkownikowi adres e-mail.

NICK_HELP_SASET_ICQ
	Sk³adnia: SASET nick ICQ numer
	
	Przypisuje numer ICQ wskazanemu nickowi, który
	bêdzie wy¶wietlany przy opisie nicka dostêpnym
	przy u¿yciu komendy INFO.
NICK_HELP_SASET_GREET
	Sk³adnia: SASET nick GREET wiadomo¶æ
	
	Ustawia wiadomo¶æ powitaln± dla wskazanego u¿ytkownika.
	Je¶li kana³ ma przydzielonego bota, w³±czone witanie
	oraz u¿ytkownik ma odpowiedni poziom dostêpu to
	wtedy ta wiadomo¶æ bêdzie wysy³ana na kana³ przez bota.

NICK_HELP_SASET_KILL
	Sk³adnia: SASET nick KILL {ON | QUICK | IMMED | OFF}
	
	W³±cza lub wy³±cza automatyczn± ochronê dla nicka.
	U¿ytkownicy wchodz±cy na chroniony nick otrzymuj±
	informacje od %S, ¿e nick jest zarejestrowany i
	je¶li siê nie zidentyfikuj± lub go nie zmieni±
	w okre¶lonym czasie to ich nick zostanie zmieniony
	si³± lub zostan± od³±czeni od sieci.
	
	Opcja QUICK skraca czas oczekiwania na zmianê
	z 60 sekund do 20. W przypadku opcji IMMED
	nick zostanie zmieniony natychmiast bez mo¿liwo¶ci
	identyfikacji (wstêpna identyfikacja nastêpuje
	na podstawie listy dostêpu, dlatego t± opcjê
	nale¿y stosowaæ z rozwag±). Administrator sieci 
	mo¿e wy³±czyæ korzystanie z IMMED.
NICK_HELP_SASET_SECURE
	Sk³adnia: SASET nick SECURE {ON | OFF}
	
	W³±cza lub wy³±cza opcjê bezpieczeñstwa dla nicka.
	W³±czenie SECURE wymusza na u¿ytkowniku podanie
	has³a zanim ten zostanie rozpoznany jako w³a¶ciciel
	nicka. %S nie bierze pod uwagê w tym wypadku
	wystêpowania adresu li¶cie dostêpu. Je¶li adres 
	znajduje siê na li¶cie %S nie skilluje
	u¿ytkownika niezale¿nie od ustawienia opcji KILL.

NICK_HELP_SASET_PRIVATE
	Sk³adnia: SASET nick PRIVATE {ON | OFF}

	W³±cza lub wy³±cza opcjê PRIVATE dla nicka.
	Kiedy ta opcja jest w³±czona nick nie bêdzie
	umieszczany na listach wy¶wietlanych komend±:
	%R%S LIST
	Jednak ka¿dy, kto zna nick mo¿e uzyskaæ o nim
	informacje u¿ywaj±c polecenia INFO.

NICK_HELP_SASET_HIDE
	Sk³adnia: SASET nick HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	To polecenie pozwala na ukrycie czê¶ci informacji, które
	s± wy¶wietlane przez polecenie INFO serwisu %s.
	Pierwszy argument okre¶la ukrywan± informacjê:
	EMAIL - adres e-mail, USERMASK - ostatnio u¿ywany host,
	QUIT - ostatnia wiadomo¶æ po¿egnalna, STATUS - status
	dostêpu do serwisów. Drugi argument okre¶la, czy dana 
	informacja bêdzie pokazywana (OFF), czy ukrywana (ON).
NICK_HELP_SASET_MSG
	Sk³adnia: SASET nick MSG {ON | OFF}
	
	Pozwala wybraæ sposób w jaki serwisy bêd± pisaæ do
	wskazanego u¿ytkownika. Kiedy ta opcja jest
	w³±czona serwisy bêd± wysy³aæ wiadomo¶ci prywatne
	w przeciwnym wypadku bêd± wysy³aæ powiadomienia.

NICK_HELP_SASET_NOEXPIRE
	Sk³adnia: SASET nick NOEXPIRE {ON | OFF}
	
	W³±cza lub wy³±cza automatyczne usuniêcie nicka,
	je¶li nie bêdzie on u¿ywany.

NICK_HELP_SASET_AUTOOP
	Sk³adnia: SASET nick AUTOOP {ON | OFF}
	
	W³±cza lub wy³±cza automatyczne nadawanie uprawnieñ
	wskazanemu u¿ytkownikowi przy wchodzeniu na kana³.

NICK_HELP_SASET_LANGUAGE
	Sk³adnia: SASET nick LANGUAGE numer
	
	Zmienia jêzyk jakiego u¿ywaj± serwisy komunikuj±c 
	siê ze wskazan± osob±. Numer musi byæ wybrany z
	listy obs³ugiwanych jêzyków, która jest dostêpna
	po wydaniu polecenia %R%S HELP SET LANGUAGE.

NICK_HELP_RECOVER
	Sk³adnia: RECOVER nick [has³o]
	
	Pozwala na odzyskanie nicka je¶li jest on u¿ywany przez
	inn± osobê. Wykonuje te same czynno¶ci co %S w
	przypadku automatycznej ochrony nicka.
	
	U¿ycie tej komendy powoduje wprowadzenie sztucznego
	u¿ytkownika przez %S o nicku, który chcesz odzyskaæ.
	Spowoduje to od³±czenie u¿ytkownika, który go aktualnie
	u¿ywa. Serwis pozostanie przez minutê online, aby
	uniemo¿liwiæ natychmiastowe po³±czenie z serwerem
	na tym samym nicku. Po tym czasie mo¿esz zaj±æ nick.
	Je¶li nie chcesz czekaæ minuty mo¿esz u¿yæ polecenia
	RELEASE (%R%S HELP RELEASE).

	Aby u¿yæ polecenia RECOVER musi byæ spe³niony jeden
	z warunków:
	1. twój adres znajduje siê na li¶cie dostêpu,
	2. jeste¶ zidentyfikowany w grupie,
	   do której nale¿y wskazany nick,
	3. podasz prawid³owe has³o do nicka.

NICK_HELP_RELEASE
	Sk³adnia: RELEASE nick [has³o]
	
	Instruuje %S aby zdj±³ wszelkie blokady na³o¿one
	przez automatyczn± ochronê nicka lub w wyniku u¿ycia
	polecenia RECOVER. Blokada trwa %s; to polecenie
	pozwala j± skróciæ.
	
	Aby u¿yæ polecenia RELEASE musi byæ spe³niony jeden
	z warunków:
	1. twój adres znajduje siê na li¶cie dostêpu,
	2. jeste¶ zidentyfikowany w grupie,
	   do której nale¿y wskazany nick,
	3. podasz prawid³owe has³o do nicka.

NICK_HELP_GHOST
	Sk³adnia: GHOST nick [has³o]
	
	Opcja likwiduje sesjê-ducha o wskazanym nicku.
	Taka sytuacja najczê¶ciej ma miejsce wtedy, gdy
	po³±czenie z serwem IRC nie zosta³o zakoñczone w
	sposób prawid³owy (komputer siê zawiesi³ lub
	wyst±pi³a awaria ³±cza internetowego). Serwer IRC
	mo¿e wtedy nie rozpoznaæ zerwania po³±czenia i
	bêdzie czeka³ okre¶lon± ilo¶æ czasu na odpowied¼ 
	od klienta.
	
	Aby u¿yæ polecenia GHOST musi byæ spe³niony jeden
	z warunków:
	1. twój adres znajduje siê na li¶cie dostêpu,
	2. jeste¶ zidentyfikowany w grupie,
	   do której nale¿y wskazany nick,
	3. podasz prawid³owe has³o do nicka.

NICK_HELP_INFO
	Sk³adnia: INFO nick [ALL]
	
	Wy¶wietla informacje na temat wybranego nicka:
	w³a¶ciciel, kiedy ostatnio widziano, z jakiego 
	hosta oraz opcje nicka. Je¶li zidentyfikowany 
	u¿ytkownik u¿yje tej komendy na sobie wraz z
	parametrem ALL otrzyma wszystkie mo¿liwe
	informacje - nawet je¶li zosta³y one ukryte.

NICK_HELP_LIST
	Sk³adnia: LIST wzorzec
	
	Listuje wszystkie zarejestrowane nicki pasuj±ce do
	podanego wzorca w postaci nick!ident@host. Nicki z
	w³±czon± opcj± PRIVATE zostan± pominiête.
	
	Przyk³ady:
	
	    LIST *!adirm@mafia.ru
	        Lista wszystkich nicków pasuj±cych do
	        *!adirm@mafia.ru.
	
	    LIST *Bot*!*@*
	        Lista wszystkich nicków z Bot w nazwie
	        (wielko¶æ liter nie ma znaczenia).
	
	    LIST *!*@*.ircnet.pl
	        Lista wszystkich nicków z mask± domeny
	        ircnet.pl

NICK_HELP_ALIST
	Sk³adnia: ALIST [poziom]
	
	Listuje wszystkie kana³y, gdzie masz wpis na li¶cie
	dostêpu. Aby ograniczyæ listê pokazywanych kana³ów
	mo¿esz podaæ poziom XOP lub numer

	Przyk³ady:
	    ALIST Founder
	        Listuje kana³y, których jeste¶ w³a¶cicielem

	    ALIST AOP
	        Listuje kana³y, na których masz poziom
	        AOP lub wy¿szy.

	    ALIST 10
	        Listuje kana³y, na których masz poziom
	        10. lub wy¿szy.

	Kana³y z opcj± NOEXPIRE bêd± oznaczone wykrzyknikiem.
NICK_HELP_GLIST
	Sk³adnia: GLIST
	
	Listuje wszystkie nicki z Twojej grupy.

NICK_HELP_STATUS
	Sk³adnia: STATUS nick...
	
	Zwraca status wskazanych u¿ytkowników (czy s±
	rozpoznawani jako w³a¶ciciele nicka).
	Odpowied¼ ma nastêpuj±cy format:
	
	    nick kod
	
	Gdzie nick jest tym nickiem wys³anym w 
	komendzie, a kod mo¿e oznaczaæ:
	
	0 - u¿ytkownik jest offline lub nick niezarejestrowany
	1 - u¿ytkownik nie zidentyfikowa³ siê jako w³a¶ciciel nicka
	2 - u¿ytkownik rozpoznany na podstawie listy dostêpu
	3 - u¿ytkownik rozpoznany na podstawie has³a
	
	Maksymalnie mo¿na podaæ 16 nicków w jednym poleceniu.
	Reszta bêdzie ignorowana. Je¶li nie zostanie podany
	¿aden nick zostanie zwrócony twój status.

NICK_HELP_SENDPASS
	Sk³adnia: SENDPASS nick
	
	Wysy³a has³o do podanego nicka na adres e-mail
	przypisany do niego. Ta komenda pomaga odzyskaæ
	zapomniane has³o.
	
	Ta komenda w niektórych sieciach mo¿e byæ dostêpna
	jedynie dla osób z uprawnieniami IRC operatora.
	
	Ta komenda jest niedostêpna kiedy has³a s± szyfrowane.

NICK_HELP_CONFIRM
	Sk³adnia: CONFIRM kod
	
	To polecenie umo¿liwia zakoñczenie procesu rejestracji.
	Aby nick zosta³ zarejestrowany w serwisie %S nale¿y
	wywo³aæ to polecenie z kodem, który zosta³ wys³any na
	adres e-mail podany podczas pierwszego kroku rejestracji.
	Aby siê dowiedzieæ wiêcej wpisz %R%S HELP REGISTER

NICK_HELP_RESEND
	Sk³adnia: RESEND
	
	To polecenie powoduje ponowne wys³anie kodu autoryzuj±cego
	na adres e-mail u¿ytkownika, który wydaje polecenie.

NICK_SERVADMIN_HELP
	
	Administratorzy serwisów mog± usun±æ ka¿dy nick
	bez potrzeby identyfikacji jako jego w³a¶ciciele
	oraz przegl±daæ listê dostêpu do ka¿dego nicka
	(%R%S ACCESS LIST nick).

NICK_SERVADMIN_HELP_LOGOUT
	Sk³adnia: LOGOUT [nick [REVALIDATE]]

	Polecenie wydane bez parametrów odwraca dzia³anie
	komendy IDENTIFY - nie bêdziesz rozpoznawany jako
	w³a¶ciciel nicka.
	
	Administrator serwisów mo¿e podaæ jako parametr nick
	u¿ytkownika, który ma zostaæ wylogowany. Parametr
	REVALIDATE powoduje informowanie przez serwisy
	u¿ytkownika o potrzebie ponownej identyfikacji.

NICK_SERVADMIN_HELP_DROP
	Sk³adnia: DROP [nick]
	
	Komenda wywo³ana bez parametru spowoduje usuniêcie
	Twojego nicka z bazy serwisu %S.
	
	Komenda u¿yta z parametrem usuwa wskazany nick.
	Ka¿dy u¿ytkownik mo¿e usuwaæ nicki znajduj±ce siê
	w jego grupie bez specjalnych uprawnieñ. Usuwanie
	dowolnego nicka jest ograniczone do administratorów.

NICK_SERVADMIN_HELP_INFO
	
	Administratorzy serwisów mog± u¿ywaæ parametru
	ALL dla ka¿dego nicka.

NICK_SERVADMIN_HELP_LIST
	Sk³adnia: LIST wzorzec [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	Listuje wszystkie zarejestrowane nicki pasuj±ce
	do podanego wzorca w postaci nick!ident@host.
	Administratorzy serwisów mog± zobaczyæ nicki z
	ustawion± opcj± PRIVATE, a przy nickach z opcj±
	NOEXPIRE zobacz± dodany do nich wykrzyknik.
	
	Administratorzy serwisów mog± tak¿e podaæ parametry:
	FORBIDDEN, SUSPENDED, NOEXPIRE i UNCONFIRMED, które
	ogranicz± listê wy¶wietlanych nicków do tych, które
	spe³niaj± podane warunki. Je¶li zostanie podanych
	wiêcej warunków, wszystkie nicki spe³niaj±ce choæ
	jeden z nich zostan± pokazane.
	
	Przyk³ady:
	
	    LIST *!adirm@mafia.ru
	        Lista wszystkich nicków pasuj±cych do
	        *!adirm@mafia.ru.
	
	    LIST *Bot*!*@*
	        Lista wszystkich nicków z Bot w nazwie
	        (wielko¶æ liter nie ma znaczenia).
	
	    LIST *!*@*.ircnet.pl
	        Lista wszystkich nicków z mask± domeny
	        ircnet.pl

NICK_SERVADMIN_HELP_ALIST
	Sk³adnia: ALIST [nick] [poziom]
	
	Listuje wszystkie kana³y na których jeste¶ na li¶cie
	dostêpu. Opcjonalnie mo¿esz podaæ nick wraz z poziomem,
	co ograniczy ilo¶æ pokazywanych wpisów.
	
	Parametr nick jest przeznaczony tylko dla
	administratorów serwisów.

NICK_SERVADMIN_HELP_GLIST
	Sk³adnia: GLIST [nick]
	
	Polecenie wywo³ane bez parametrów listuje wszystkie
	nicki nale¿±ce do grupy wydaj±cego polecenie.
	
	Administratorzy serwisów mog± jako parametr podaæ
	nick osoby, której nicki chc± sprawdziæ.

NICK_SERVADMIN_HELP_GETPASS
	Sk³adnia: GETPASS nick
	
	Zwraca has³o w³a¶ciciela dla wskazanego nicka.
	UWAGA: u¿ycie tej komendy powoduje wys³anie
	informacji o jej u¿yciu przy pomocy WALLOP/GLOBOP.
	
	Ta komenda jest niedostêpna kiedy has³a s± szyfrowane.

NICK_SERVADMIN_HELP_GETEMAIL
	Sk³adnia: GETEMAIL uzytkownik@serwer.com
	
	Listuje nicki, które maj± ustawiony podany adres e-mail.
	Uwaga: nie mo¿na u¿ywaæ symboli wieloznacznych (* lub ?).
	Ka¿dy przypadek u¿ycia tej komendy jest logowany.

NICK_SERVADMIN_HELP_FORBID
	Sk³adnia: FORBID nick [powód]
	
	Uniemo¿liwia rejestracjê i korzystanie ze wskazanego
	nicka. Blokadê mo¿na zdj±æ u¿ywaj±c komendy DROP.
	
	W niektórych sieciach wymagane jest podanie powodu.

NICK_SERVADMIN_HELP_SUSPEND
	Sk³adnia: SUSPEND nick powód
	
	Zawiesza nick uniemo¿liwiaj±c identyfikacjê.

NICK_SERVADMIN_HELP_UNSUSPEND
	Sk³adnia: UNSUSPEND nick
	
	Uwalnia zawieszony nick.

###########################################################################
#
# ChanServ help messages
#
###########################################################################
CHAN_HELP_CMD_GETPASS
	    GETPASS     Podaje has³o w³a¶ciciela wskazanego kana³u

CHAN_HELP_CMD_FORBID
	    FORBID      Blokuje podany kana³

CHAN_HELP_CMD_SUSPEND
	    SUSPEND     Blokuje podany kana³ z zachowaniem
	                wszelkich jego danych i ustawieñ

CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND   Uwalnia zawieszony kana³

CHAN_HELP_CMD_STATUS
	    STATUS      Zwraca aktualny poziom dostêpu do
	                kana³u wskazanego u¿ytkownika

CHAN_HELP_CMD_REGISTER
	    REGISTER    Rejestruje kana³

CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY    Identyfikuje w³a¶ciciela kana³u

CHAN_HELP_CMD_SET
	    SET         Modyfikuje ustawienia kana³u

CHAN_HELP_CMD_AOP
	    AOP         Zarz±dza list± AOP (operatorów)

CHAN_HELP_CMD_SOP
	    SOP         Zarz±dza list± SOP (super-operatorów)

CHAN_HELP_CMD_ACCESS
	    ACCESS      Zarz±dza list± dostêpu

CHAN_HELP_CMD_LEVELS
	    LEVELS      Zarz±dza poziomami dostêpu

CHAN_HELP_CMD_AKICK
	    AKICK       Zarz±dza list± AKICK

CHAN_HELP_CMD_DROP
	    DROP        Usuwa kana³

CHAN_HELP_CMD_SENDPASS
	    SENDPASS    Pomaga odzyskaæ utracone has³o

CHAN_HELP_CMD_BAN
	    BAN         Banuje wskazan± osobê na kanale

CHAN_HELP_CMD_CLEAR
	    CLEAR       Usuwa wskazane obiekty z kana³u

CHAN_HELP_CMD_DEVOICE
	    DEVOICE     Odbiera wskazanej osobie prawo g³osu

CHAN_HELP_CMD_GETKEY
	    GETKEY      Podaje klucz do wskazanego kana³u

CHAN_HELP_CMD_INFO
	    INFO        Pokazuje informacje o zarejestrowanym kanale

CHAN_HELP_CMD_INVITE
	    INVITE      Zaprasza osobê wydaj±c± polecenie na kana³

CHAN_HELP_CMD_KICK
	    KICK        Wyrzuca wskazany nick z kana³u

CHAN_HELP_CMD_LIST
	    LIST        Listuje zarejestrowane kana³y pasuj±ce do wzorca

CHAN_HELP_CMD_LOGOUT
	    LOGOUT      Wyloguje wskazan± osobê

CHAN_HELP_CMD_OP
	    OP          Nadaje wskazanej osobie uprawnienia operatora

CHAN_HELP_CMD_TOPIC
	    TOPIC       Zmienia temat podanego kana³u

CHAN_HELP_CMD_UNBAN
	    UNBAN       Usuwa wszystkie bany blokuj±ce wej¶cie na kana³

CHAN_HELP_CMD_VOICE
	    VOICE       Nadaje wskazanej osobie prawo g³osu

CHAN_HELP_CMD_VOP
	    VOP         Zarz±dza list± VOP (osób z prawem g³osu)

CHAN_HELP_CMD_DEHALFOP
	    DEHALFOP    Odbiera wskazanej osobie uprawnienia pó³-operatora

CHAN_HELP_CMD_DEOWNER
	    DEOWNER     Usuwa twój status w³a¶ciciela kana³u

CHAN_HELP_CMD_DEPROTECT
	    DEPROTECT   Wy³±cza ochronê wskazanej osoby

CHAN_HELP_CMD_HALFOP
	    HALFOP      Nadaje wskazanej osobie uprawnienia pó³-operatora

CHAN_HELP_CMD_HOP
	    HOP         Zarz±dza list± HOP (pó³-operatorów)

CHAN_HELP_CMD_OWNER
	    OWNER       Nadaje Tobie status w³a¶ciciela kana³u

CHAN_HELP_CMD_PROTECT
	    PROTECT     Chroni wskazan± osobê

CHAN_HELP_CMD_ADMIN
	    ADMIN       Chroni wskazan± osobê

CHAN_HELP_CMD_DEADMIN
	    DEADMIN     Wy³±cza ochronê wskazanej osoby

CHAN_HELP_CMD_DEOP
	    DEOP        Odbiera wskazanej osobie uprawnienia operatora

CHAN_HELP
	%S umo¿liwia rejestracjê kana³u, zmianê
	jego ustawieñ oraz zapobiega przed odebraniem
	kontroli nad kana³em. Poni¿sze komendy pozwalaj±
	na zarz±dzanie kana³em, aby ich u¿yæ wpisz:
	%R%S komenda
	Aby uzyskaæ szczegó³owy opis polecenia wpisz:
	%R%S HELP komenda
	

CHAN_HELP_EXPIRES
	
	Uwaga! Kana³ który nie by³ u¿ywany przez %d dni
	(np. nie odwiedzi³ go ¿aden u¿ytkownik umieszczony
	na li¶cie dostêpu kana³u w tym czasie) zostanie
	automatycznie odrejestrowany.

CHAN_HELP_REGISTER
	Sk³adnia: REGISTER kana³ has³o opis
	
	Rejestruje kana³ w bazie danych %S. Aby u¿yæ
	tej komendy musisz byæ operatorem na kanale, który
	chcesz zarejestrowaæ. Has³o umo¿liwia pó¼niejsz±
	identyfikacjê jako w³a¶ciciel kana³u (IDENTIFY).
	Ostatni parametr, który musi zostaæ podany, okre¶la
	cel istnienia kana³u.
	
	Po rejestracji kana³u stajesz siê jego w³a¶cicielem,
	który mo¿e zmieniaæ wszystkie jego ustawienia.
	%S automatycznie nadaje uprawnienia operatora
	w³a¶cicielowi, który wchodzi na kana³. Informacje 
	na temat nadawania uprawnieñ innym u¿ytkownikom
	znajduj± siê w pomocy polecenia ACCESS
	(%R%S HELP ACCESS).
	
	UWAGA: Aby zarejestrowaæ kana³ nale¿y posiadaæ
	zarejestrowany nick, aby uzyskaæ wiêcej informacji
	na temat zarz±dzania nickiem wpisz: %R%s HELP

CHAN_HELP_IDENTIFY
	Sk³adnia: IDENTIFY kana³ has³o
	
	Autoryzuje u¿ytkownika w serwisie %S jako w³a¶ciciela
	wskazanego kana³u. Wiele wa¿nych komend wymaga dokonania
	identyfikacji przed ich u¿yciem. Has³em jest to samo
	has³o, którego u¿yto przy komendzie REGISTER.

CHAN_HELP_LOGOUT
	Sk³adnia: LOGOUT kana³ nick
	
	Pozwala na wylogowanie wskazanego u¿ytkownika z kana³u.
	W celu odzyskania uprawnieñ u¿ytkownik bêdzie musia³
	ponownie siê zidentyfikowaæ.
	
	Ka¿dy mo¿e wylogowaæ siebie, natomiast w³a¶ciciel kana³u
	mo¿e wylogowaæ ka¿dego.

CHAN_HELP_DROP
	Sk³adnia: DROP kana³
	
	Usuwa podany kana³. Komenda mo¿e zostaæ u¿yta
	jedynie przez w³a¶ciciela kana³u, po wcze¶niejszej
	IDENTIFYKACJI przy pomocy has³a kana³u.

CHAN_HELP_SET
	Sk³adnia: SET kana³ opcja parametry
	
	Pozwala w³a¶cicielowi kana³u na zmianê ro¿nych  
	opcji oraz informacji dotycz±cych kana³u.
	
	Dostêpne opcje:
	
	    FOUNDER       Ustawia nick w³a¶ciciela kana³u
	    SUCCESSOR     Ustawia nick spadkobiercy kana³u
	    PASSWORD      Ustawia has³o w³a¶ciciela
	    DESC          Ustawia opis kana³u
	    URL           Przypisuje adres www do kana³u
	    EMAIL         Przypisuje adres e-mail do kana³u
	    ENTRYMSG      Ustawia wiadomo¶æ, która bêdzie wysy³ana
	                  do u¿ytkowników wchodz±cych na kana³
	    BANTYPE       Ustawia sposób banowania przez serwisy
	    MLOCK         Blokuje zdejmowanie lub zak³adanie flag
	    KEEPTOPIC     Zapamiêtuje temat kiedy kana³ jest pusty
	    OPNOTICE      Wysy³a powiadomienie kiedy zostaj± u¿yte
	                  komendy OP/DEOP
	    PEACE         Reguluje u¿ycie krytycznych komend
	    PRIVATE       Zapobiega wy¶wietlaniu kana³u po wydaniu
	                  polecenia LIST
	    RESTRICTED    Ogranicza dostêp do kana³u
	    SECURE        W³±cza lub wy³±cza bezpieczeñstwo kana³u
	    SECUREOPS     ¦ci¶le kontroluje status operatorów
	    SECUREFOUNDER ¦ci¶le kontroluje status w³a¶ciciela
	    SIGNKICK      Podpisuje kicki kiedy u¿yto komendy KICK
	    TOPICLOCK     Ogranicza mo¿liwo¶æ zmiany tematu
	    XOP           W³±cza system uprawnieñ xOP
	                  (u¿ycie komend SOP, AOP, HOP, VOP)
	
	Aby uzyskaæ wiêcej informacji o danej opcji wpisz:
	%R%S HELP SET opcja

CHAN_HELP_SET_FOUNDER
	Sk³adnia: SET kana³ FOUNDER nick
	
	Zmienia w³a¶ciciela kana³u.
	Nowy nick musi byæ zarejestrowany.

CHAN_HELP_SET_SUCCESSOR
	Sk³adnia: SET kana³ SUCCESSOR nick
	
	Ustawia spadkobiercê kana³u. Je¶li nick w³a¶ciciela
	wyga¶nie lub zostanie usuniêty kana³ zostaje przekazany
	spadkobiercy, który staje siê nowym w³a¶cicielem.
	Je¿eli spadkobierca jest w³a¶cicielem zbyt du¿ej
	liczby kana³ów (%d) to kana³ zostanie odrejestrowany.
CHAN_HELP_SET_PASSWORD
	Sk³adnia: SET kana³ PASSWORD has³o
	
	Zmienia has³o w³a¶ciciela kana³u.

CHAN_HELP_SET_DESC
	Sk³adnia: SET kana³ DESC opis
	
	Ustawia opis kana³u widoczny po wydaniu polecenia LIST oraz INFO.

CHAN_HELP_SET_URL
	Sk³adnia: SET kana³ URL [url]
	
	Przypisuje adres strony www do kana³u. Adres bêdzie
	widoczny po u¿yciu polecenia INFO. Komenda wywo³ana
	bez parametru usuwa wcze¶niej ustawiony adres.

CHAN_HELP_SET_EMAIL
	Sk³adnia: SET kana³ EMAIL [adres]
	
	Przypisuje adres e-mail do kana³u. Adres bêdzie
	widoczny po u¿yciu polecenia INFO. Komenda wywo³ana
	bez parametru usuwa wcze¶niej ustawiony adres.

CHAN_HELP_SET_ENTRYMSG
	Sk³adnia: SET kana³ ENTRYMSG [tre¶æ]
	
	Ustawia tekst powiadomienia wysy³anego poprzez /notice
	do u¿ytkowników wchodz±cych na kana³. Komenda wywo³ana
	bez parametru usuwa wcze¶niej ustawion± wiadomo¶æ.

CHAN_HELP_SET_BANTYPE
	Sk³adnia: SET kana³ BANTYPE typ
	
	Ustala typ banów zak³adanych przez serwisy.
	
	Typ banów oznaczany jest numerami od 0 do 3,
	poszczególne numery oznaczaj±:
	
	0: ban w postaci *!ident@host
	1: ban w postaci *!*ident@host
	2: ban w postaci *!*@host
	3: ban w postaci *!*ident@*.domena

CHAN_HELP_SET_KEEPTOPIC
	Sk³adnia: SET kana³ KEEPTOPIC {ON | OFF}
	
	W³±cza lub wy³±cza opcjê zapamiêtywania tematu.
	Kiedy ta opcja jest w³±czona %S ustawi ostatni
	temat za ka¿dym razem, gdy na kana³ bêdzie wchodziæ
	pierwsza osoba.

CHAN_HELP_SET_TOPICLOCK
	Sk³adnia: SET kana³ TOPICLOCK {ON | OFF}
	
	W³±cza lub wy³±cza opcjê blokowania tematu kana³u.
	
	Je¶li ta opcja jest w³±czona, %S pozwoli na
	zmianê tematu tylko poprzez wydanie komendy TOPIC.

CHAN_HELP_SET_MLOCK
	Sk³adnia: SET kana³ MLOCK flagi
	
	Ustawia blokadê flag kana³owych. %S pozwala na 
	definiowanie, które flag kana³owe maj± byæ zawsze 
	w³±czone, wy³±czone lub ustawiane dowolnie przez 
	operatorów kana³u.
	
	Sk³adnia polecenia jest identyczna jak w komendzie 
	/MODE, tzn. flagi poprzedzone znakiem+ 
	zostan± za³o¿one na kana³, natomiast poprzedzone 
	znakiem - bêd± zdejmowane z kana³u. W odró¿nieniu 
	od komendy /MODE, ka¿de u¿ycie SET MLOCK, 
	usunie wszystkie wcze¶niej zdefiniowane mody!
	
	UWAGA: W przypadku ustawiania klucza dostêpu do 
	kana³u (drugi przyk³ad poni¿ej), powinna równie¿ 
	zostaæ aktywowana opcja RESTRICTED (zobacz 
	HELP SET RESTRICTED), w przeciwnym wypadku, 
	u¿ytkownik wchodz±cy na pusty kana³, bêdzie móg³ 
	odczytaæ za³o¿ony klucz!
	
	Przyk³ady:
	
	    SET #kana³ MLOCK +nt-iklps
	        Wymusza w³±czenie flag n oraz t, wy³±czenie flag
	        i, k, l, p, s; flaga m nie jest zmieniana.
	
	    SET #kana³ MLOCK +knst-ilmp my-key
	        Wymusza w³±czenie flag k, n, s, oraz t, wy³±czenie
	        i, l, m, oraz p. Wymusza tak¿e ustawienie klucza 
	        na warto¶æ "my-key".
	
	    SET #kana³ MLOCK +
	        Wy³±cza blokowanie flag - mog± byæ zmieniane dowolnie.

CHAN_HELP_SET_PEACE
	Sk³adnia: SET kana³ PEACE {ON | OFF}
	
	W³±cza lub wy³±cza opcjê PEACE dla kana³u.
	W³±czenie tej opcji uniemo¿liwia wykopanie,
	banowanie lub odebranie uprawnieñ u¿ytkownikowi,
	który ma poziom uprawnieñ wy¿szy lub równy
	korzystaj±c z poleceñ serwisu %S.

CHAN_HELP_SET_PRIVATE
	Sk³adnia: SET kana³ PRIVATE {ON | OFF}
	
	W³±cza lub wy³±cza opcjê PRIVATE dla kana³u.
	Kiedy ta opcja jest w³±czona kana³ nie bêdzie
	umieszczany na listach wy¶wietlanych komend±:
	%R%S LIST

CHAN_HELP_SET_RESTRICTED
	Sk³adnia: SET kana³ RESTRICTED {ON | OFF}
	
	W³±cza lub wy³±cza opcjê ograniczonego dostêpu do kana³u.
	Po w³±czeniu tej opcji tylko osoby znajduj±ce siê na
	li¶cie dostêpu bêd± mog³y przebywaæ na kanale. Pozostali
	u¿ytkownicy bêd± banowani, a nastêpnie usuwani z kana³u.

CHAN_HELP_SET_SECURE
	Sk³adnia: SET kana³ SECURE {ON | OFF}
	
	W³±cza lub wy³±cza opcje bezpieczeñstwa kana³u.
	Po w³±czeniu tej opcji tylko zarejestrowani
	u¿ytkownicy, którzy siê zidentyfikowali bêd±
	otrzymywaæ od serwisu %S uprawnienia
	wynikaj±ce z wpisów na li¶cie dostêpu.

CHAN_HELP_SET_SECUREOPS
	Sk³adnia: SET kana³ SECUREOPS {ON | OFF}
	
	W³±cza lub wy³±cza opcjê SECUREOPS dla kana³u.
	Kiedy ta opcja jest w³±czona u¿ytkownicy bez wpisu
	na li¶cie dostêpu kana³u (ACCESS LIST) nie bêd±
	mogli posiadaæ statusu operatora.

CHAN_HELP_SET_SECUREFOUNDER
	Sk³adnia: SET kana³ SECUREFOUNDER {ON | OFF}
	
	W³±cza lub wy³±cza opcjê bezpiecznego w³a¶ciciela.
	Ustawienie tej opcji powoduje ograniczenie uprawnieñ
	w³a¶ciciela do osoby, która jest wpisana jako w³a¶ciciel
	(a nie osób znaj±cych has³o w³a¶ciciela kana³u).
	Operacje takie jak usuniêcie kana³u, zmiana has³a,
	w³a¶ciciela oraz zastêpcy bêd± dostêpne tylko
	dla jednego u¿ytkownika.

CHAN_HELP_SET_SIGNKICK
	Sk³adnia: SET kana³ SIGNKICK {ON | LEVEL | OFF}
	
	W³±cza lub wy³±cza podpisywanie komend kick.
	W³±czenie tej opcji powoduje dodawanie nicka osoby 
	wydaj±cej polecenie do uzasadnienia kicka.
	
	Je¶li poziomy dostêpu by³y konfigurowane z u¿yciem LEVEL
	osoby maj±ce poziom wiêkszy lub równy SIGNKICK nie bêd±
	podpisywane. Aby uzyskaæ wiêcej informacji wpisz:
	%R%S HELP LEVELS

CHAN_HELP_SET_XOP
	Sk³adnia: SET kana³ XOP {ON | OFF}
	
	W³±cza lub wy³±cza system list xOP dla kana³u.
	Kiedy XOP zostanie w³±czony przyznawanie uprawnieñ
	odbywa siê przy pomocy poleceñ AOP/ SOP/ VOP.
	Wy³±czenie funkcji xOP wymusza korzystanie z
	polecenia ACCESS.
	
	Uwaga: w momencie w³±czania systemu list XOP
	wszelkie definicje poziomów dostêpu zostan±
	trwale zresetowane. Ponowne przej¶cie z XOP na
	system listy dostêpu nie spowoduje odzyskania
	poprzedniej konfiguracji!
	
	Kiedy XOP zostanie w³±czony u¿ytkownikom
	automatycznie zostaje przypisany odpowiedni poziom
	dostêpu. Automatyczne przyporz±dkowanie nie zawsze 
	musi byæ poprawne. Nie zaleca siê przej¶cia na
	system XOP, je¶li definicje poziomów dostêpu dla 
	kana³u zosta³y wcze¶niej zmodyfikowane przy pomocy 
	polecenia LEVELS.
	

CHAN_HELP_SET_OPNOTICE
	Sk³adnia: SET kana³ OPNOTICE {ON | OFF}
	
	W³±cza lub wy³±cza opcjê OPNOTICE dla kana³u.
	Kiedy ta opcja jest w³±czona, %S bêdzie
	informowa³ u¿ytkowników przebywaj±cych na kanale,
	kiedy zostanie u¿yta komenda OP lub DEOP.

CHAN_HELP_AOP
	Sk³adnia: AOP kana³ ADD nick
	          AOP kana³ DEL {nick | numer | lista}
	          AOP kana³ LIST [maska | lista]
	          AOP kana³ CLEAR
	
	Zarz±dza list± AOP (AutoOP) dla kana³u.
	U¿ytkownicy wpisani na t± listê bêd± otrzymywaæ
	automatycznie status operatora, maj± prawo siê
	odbanowaæ i zaprosiæ je¶li jest taka potrzeba itd.
	
	Komenda AOP ADD dodaje podany nick do listy AOP.
	
	Komenda AOP DEL usuwa wskazane wpisy z listy.
	Jako argument przyjmuje nick, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).
	
	Komenda AOP LIST wy¶wietla zawarto¶æ listy AOP.
	Parametr maska pozwala filtrowaæ wy¶wietlane wpisy
	(symbol * zastêpuje dowolny ci±g znaków w nicku).
	Jako parametr mo¿na te¿ podaæ numery wpisów na li¶cie:
	
	   AOP kana³ LIST 2-5,7-9
	      Wy¶wietli wpisy na li¶cie od numeru 2 do 5 oraz
	      od 7 do 9.
	
	Komenda AOP CLEAR usuwa wszystkie wpisy z listy.
	
	Komendy AOP ADD oraz AOP DEL s± dostêpne dla
	u¿ytkowników z uprawnieniami SOP lub wy¿szymi.
	Komenda AOP CLEAR jest dostêpna dla w³a¶ciciela
	kana³u. Wszyscy u¿ytkownicy wpisani na listê AOP
	mog± u¿ywaæ komendy AOP LIST.
	
	Komenda AOP mo¿e byæ wy³±czona dla Twojego kana³u,
	w takim przypadku nale¿y u¿ywaæ komendy ACCESS.
	Aby uzyskaæ informacje o komendzie ACCESS wpisz:
	%R%S HELP ACCESS
	Aby dowiedzieæ siê jak w³±czyæ tryb xOP wpisz:
	%R%S HELP SET XOP

CHAN_HELP_HOP
	Sk³adnia: HOP kana³ ADD nick
	          HOP kana³ DEL {nick | numer | lista}
	          HOP kana³ LIST [maska | lista]
	          HOP kana³ CLEAR
	
	Zarz±dza list± HOP (HalfOP) dla kana³u.
	U¿ytkownicy wpisani na t± listê bêd± otrzymywaæ
	automatycznie status pó³-operatora.
	
	Komenda HOP ADD dodaje podany nick do listy HOP.
	
	Komenda HOP DEL usuwa wskazane wpisy z listy.
	Jako argument przyjmuje nick, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).
	
	Komenda HOP LIST wy¶wietla zawarto¶æ listy HOP.
	Parametr maska pozwala filtrowaæ wy¶wietlane wpisy
	(symbol * zastêpuje dowolny ci±g znaków w nicku).
	Jako parametr mo¿na te¿ podaæ numery wpisów na li¶cie:
	
	   HOP kana³ LIST 2-5,7-9
	      Wy¶wietli wpisy na li¶cie od numeru 2 do 5 oraz
	      od 7 do 9.
	
	Komenda HOP CLEAR usuwa wszystkie wpisy z listy.
	
	Komendy HOP ADD, HOP DEL oraz HOP LIST s± dostêpne
	dla u¿ytkowników z uprawnieniami AOP lub wy¿szymi.
	Komenda HOP CLEAR jest dostêpna tylko dla
	w³a¶ciciela kana³u.
	
	Komenda HOP mo¿e byæ wy³±czona dla Twojego kana³u,
	w takim przypadku nale¿y u¿ywaæ komendy ACCESS.
	Aby uzyskaæ informacje o komendzie ACCESS wpisz:
	%R%S HELP ACCESS
	Aby dowiedzieæ siê jak w³±czyæ tryb xOP wpisz:
	%R%S HELP SET XOP

CHAN_HELP_SOP
	Sk³adnia: SOP kana³ ADD nick
	          SOP kana³ DEL {nick | numer | lista}
	          SOP kana³ LIST [maska | lista]
	          SOP kana³ CLEAR
	
	Zarz±dza list± SOP (SuperOP) dla kana³u.
	U¿ytkownicy wpisani na t± listê bêd± otrzymywaæ
	automatycznie status operatora.
	
	Komenda SOP ADD dodaje podany nick do listy SOP.
	
	Komenda SOP DEL usuwa wskazane wpisy z listy.
	Jako argument przyjmuje nick, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).
	
	Komenda SOP LIST wy¶wietla zawarto¶æ listy SOP.
	Parametr maska pozwala filtrowaæ wy¶wietlane wpisy
	(symbol * zastêpuje dowolny ci±g znaków w nicku).
	Jako parametr mo¿na te¿ podaæ numery wpisów na li¶cie:
	
	   SOP kana³ LIST 2-5,7-9
	      Wy¶wietli wpisy na li¶cie od numeru 2 do 5 oraz
	      od 7 do 9.
	
	Komenda SOP CLEAR usuwa wszystkie wpisy z listy.
	
	Komendy SOP ADD, SOP DEL oraz SOP CLEAR s±
	dostêpne tylko dla w³a¶ciciela kana³u.
	Komenda SOP LIST jest dostêpna dla
	u¿ytkowników wpisanych na listê AOP.
	
	Komenda SOP mo¿e byæ wy³±czona dla Twojego kana³u,
	w takim przypadku nale¿y u¿ywaæ komendy ACCESS.
	Aby uzyskaæ informacje o komendzie ACCESS wpisz:
	%R%S HELP ACCESS
	Aby dowiedzieæ siê jak w³±czyæ tryb xOP wpisz:
	%R%S HELP SET XOP

CHAN_HELP_VOP
	Sk³adnia: VOP kana³ ADD nick
	          VOP kana³ DEL {nick | numer | lista}
	          VOP kana³ LIST [maska | lista]
	          VOP kana³ CLEAR
	
	Zarz±dza list± VOP (VOicePeople) dla kana³u.
	U¿ytkownicy wpisani na t± listê bêd± otrzymywaæ
	automatycznie prawo g³osu i bêd± mogli je sobie
	nadaæ je¶li bêdzie taka potrzeba.
	
	Komenda VOP ADD dodaje podany nick do listy VOP.
	
	Komenda VOP DEL usuwa wskazane wpisy z listy.
	Jako argument przyjmuje nick, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).
		
	Komenda VOP LIST wy¶wietla zawarto¶æ listy VOP.
	Parametr maska pozwala filtrowaæ wy¶wietlane wpisy
	(symbol * zastêpuje dowolny ci±g znaków w nicku).
	Jako parametr mo¿na te¿ podaæ numery wpisów na li¶cie:
	
	   VOP kana³ LIST 2-5,7-9
	      Wy¶wietli wpisy na li¶cie od numeru 2 do 5 oraz
	      od 7 do 9.
	
	Komenda VOP CLEAR usuwa wszystkie wpisy z listy.
	
	Komendy VOP ADD, VOP DEL oraz VOP LIST s± dostêpne
	dla u¿ytkowników z uprawnieniami AOP lub wy¿szymi.
	Komenda VOP CLEAR jest dostêpna tylko dla
	w³a¶ciciela kana³u.
	
	Komenda VOP mo¿e byæ wy³±czona dla Twojego kana³u,
	w takim przypadku nale¿y u¿ywaæ komendy ACCESS.
	Aby uzyskaæ informacje o komendzie ACCESS wpisz:
	%R%S HELP ACCESS
	Aby dowiedzieæ siê jak w³±czyæ tryb xOP wpisz:
	%R%S HELP SET XOP

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
	Napisz msg %R%S HELP ACCESS LEVELS,
	aby uzyskaæ wiêcej informacji. Ka¿dy u¿ytkownik,
	który nie zosta³ wpisany na listê dostêpu, posiada
	poziom uprawnieñ równy 0.
	
	Polecenie ACCESS ADD dodaje podany nick do listy z
	okre¶lonym poziomem dostêpu. Je¶li nick jest ju¿
	wpisany na li¶cie, jego poziom dostêpu zostanie
	zmieniony zgodnie z podanym parametrem. Przyznawany
	poziom dostêpu nie mo¿e byæ wy¿szy ni¿ poziom osoby
	wydaj±cej komendê ADD.
	
	Polecenie ACCESS DEL usuwa wskazane wpisy z listy.
	Jako argument przyjmuje nick, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).
	
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
	
	Polecenie ACCESS CLEAR usuwa wszystkie wpisy z listy.

CHAN_HELP_ACCESS_LEVELS
	Poziomy dostêpu u¿ytkowników:
	
	Standardowe poziomy dostêpu zosta³y podane poni¿ej:
	   Founder  Pe³ny dostêp do poleceñ %S.
	            Automatyczne nadawanie statusu operatora.
	            UWAGA: tylko jedna osoba mo¿e byæ
	            w³a¶cicielem (Founder) kana³u.
	            Nie mo¿na nadaæ tego uprawnienia
	            za pomoc± polecenia ACCESS.
	        10  Dostêp do polecenia AKICK,
	            Automatyczne nadawanie statusu operatora.
	         5  Automatyczne nadawanie statusu operatora.
	         3  Automatyczne nadawanie prawa g³osu.
	         0  Brak specjalnych przywilejów.
	            Inne osoby mog± nadaæ status
	            operatora temu u¿ytkownikowi
	            (je¶li wy³±czono opcjê secure-ops).
	        <0  Brak mo¿liwo¶ci pe³nienia funkcji
	            operatora.
	
	Standardowe poziomy mog± zostaæ zmienione, mo¿liwe
	jest tak¿e definiowanie w³asnych poziomów dostêpu.
	Aby uzyskaæ wiêcej informacji wpisz:
	%R%S HELP LEVELS

CHAN_HELP_AKICK
	Sk³adnia: AKICK kana³ ADD {nick | maska} [powód]
	          AKICK kana³ STICK maska
	          AKICK kana³ UNSTICK maska
	          AKICK kana³ DEL {nick | maska | numer-wpisu | lista}
	          AKICK kana³ LIST [maska | numer-wpisu | lista]
	          AKICK kana³ VIEW [maska | numer-wpisu | lista]
	          AKICK kana³ ENFORCE
	          AKICK kana³ CLEAR
	
	Polecenie zarz±dza list± AutoKick dla kana³u. Je¶li
	u¿ytkownik znajduj±cy siê na li¶cie AKICK, wejdzie
	na kana³, %S za³o¿y bana, a nastêpnie wykopie
	u¿ytkownika z kana³u.
	
	Polecenie AKICK ADD dodaje podany nick lub maskê
	hosta do listy AutoKick. Je¶li zostanie podany
	powód, bêdzie on u¿ywany przy wykopywaniu
	u¿ytkownika dodanego do listy z kana³u, je¶li nie
	zostanie podany, u¿ywany bêdzie domy¶lny:
	You have been banned from channel.
	Dodawanie zarejestrowanego nicka na listê powoduje
	wykopywanie wszystkich nicków nale¿±cych do grupy.
	
	Polecenie AKICK STICK umo¿liwia permanentne
	dodanie podanej maski do banów na kanale. Za ka¿dym
	razem gdy kto¶ zdejmie wskazanego bana z kana³u,
	%S automatycznie za³o¿y go ponownie.
	Parametrem polecenia nie mo¿e byæ nick a jedynie
	host.
	
	Polecenie AKICK UNSTICK niweluje dzia³anie
	polecenia AKICK STICK - umo¿liwia zdjêcie bana z
	kana³u.
	
	AKICK DEL usuwa wskazany nick lub maskê z listy AKICK.
	Jako argument przyjmuje nick, maskê, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).
	
	AKICK LIST wy¶wietla zawarto¶æ listy AKICK. Je¶li
	zostanie podana maska tylko pasuj±ce wpisy zostan±
	pokazane.

	AKICK VIEW zwraca wiêcej szczegó³ów ni¿ AKICK LIST.
	
	Polecenie AKICK ENFORCE powoduje ¿e %S
	usunie z kana³u wszystkich u¿ytkowników, których
	hosty pasuj± do wskazanych masek w li¶cie AKICK.
	
	Polecenie AKICK CLEAR usuwa wszystkie wpisy z listy.

CHAN_HELP_LEVELS
	Sk³adnia: LEVELS kana³ SET typ poziom
	          LEVELS kana³ {DIS | DISABLE} typ
	          LEVELS kana³ LIST
	          LEVELS kana³ RESET
	
	Polecenie LEVELS umo¿liwia przypisywanie 
	uprawnieñ poszczególnym poziomom dostepu.
	Przy u¿yciu tej komendy mo¿esz definiowaæ 
	poziomy dostêpu do wszystkich funkcji %S 
	(SET FOUNDER, SET PASSWORD oraz LEVELS
	s± dostêpne tylko dla w³a¶ciciela kana³u).
	
	LEVELS SET pozwala zmieniæ przypisany poziom dla 
	okre¶lonej operacji lub ich grup. LEVELS DISABLE 
	(lub DIS w skrócie) wy³±cza okre¶lon± funkcjê lub
	zastrzega dostêp do niej dla w³a¶ciciela kana³u.
	LEVELS LIST wy¶wietla aktualnie ustalone poziomy 
	dla ka¿dej z operacji. LEVELS RESET przywraca 
	standardowe ustawienia poziomów dostêpu - identyczne 
	z tymi jakie ustawiane s± dla kana³u po jego 
	utworzeniu (zobacz HELP ACCESS LEVELS).
	
	Aby obejrzeæ spis wszystkich funkcji które mog± 
	zostaæ przypisane poziomom dostêpu, zobacz
	HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Poni¿ej znajduje siê lista wszystkich dostêpnych uprawnieñ.
	Poziomy dla AUTODEOP oraz NOJOIN to poziomy maksymalne,
	reszta uprawnieñ ma poziomy minimalne.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Sk³adnia: INFO kana³ [ALL]
	
	Wy¶wietla informacje o zarejestrowanym kanale:
	w³a¶ciciel, data za³o¿enia, opis, blokada flag,
	czas ostatniego u¿ycia. Kiedy zostanie podany parametr
	ALL zostan± wy¶wietlone tak¿e informacje o spadkobiercy
	kana³u i wiadomo¶ci powitalnej.
	
	Domy¶lnie parametr ALL jest dostêpny dla osób z
	uprawnieniami w³a¶ciciela kana³u.

CHAN_HELP_LIST
	Sk³adnia: LIST wzorzec
	
	Listuje kana³y o nazwie pasuj±cej do podanego wzorca
	(kana³y z opcj± PRIVATE nie bêd± pokazywane).
	Poprzedzenie znakiem '#' powoduje podawanie zakresu,
	nazwy kana³ów powinny byæ pisane bez znaku '#'.

CHAN_HELP_OP
	Sk³adnia: OP [kana³ [nick]]
	
	Nadaje status operatora wskazanemu u¿ytkownikowi.
	Je¶li nie zostanie podany nick operacja bêdzie
	przeprowadzona na osobie wydaj±cej t± komendê.
	Je¶li tak¿e nie zostanie podany kana³ operacja
	bêdzie przeprowadzona na wszystkich kana³ach na
	których przebywa osoba wydaj±ca to polecenie z
	uwzglêdnieniem posiadanych uprawnieñ.
	
	Standardowo ta komenda jest dostêpna dla u¿ytkowników
	wpisanych na listê AOP lub posiadaj±cych poziom
	dostêpu 5 lub wy¿szy.

CHAN_HELP_DEOP
	Sk³adnia: DEOP [kana³ [nick]]
	
	Odbiera status operatora wskazanemu u¿ytkownikowi.
	Je¶li nie zostanie podany nick operacja bêdzie
	przeprowadzona na osobie wydaj±cej t± komendê.
	Je¶li tak¿e nie zostanie podany kana³ operacja
	bêdzie przeprowadzona na wszystkich kana³ach na
	których przebywa osoba wydaj±ca to polecenie z
	uwzglêdnieniem posiadanych uprawnieñ.
	
	Standardowo ta komenda jest dostêpna dla u¿ytkowników
	wpisanych na listê AOP lub posiadaj±cych poziom
	dostêpu 5 lub wy¿szy.

CHAN_HELP_VOICE
	Sk³adnia: VOICE [kana³ [nick]]
	
	Nadaje prawo g³osu wskazanemu u¿ytkownikowi.
	Je¶li nie zostanie podany nick operacja bêdzie
	przeprowadzona na osobie wydaj±cej t± komendê.
	Je¶li tak¿e nie zostanie podany kana³ operacja
	bêdzie przeprowadzona na wszystkich kana³ach na
	których przebywa osoba wydaj±ca to polecenie z
	uwzglêdnieniem posiadanych uprawnieñ.
	
	Standardowo ta komenda jest dostêpna dla u¿ytkowników
	wpisanych na listê AOP lub posiadaj±cych przynajmniej
	5. poziom dostêpu. Osoby wpisane na listê VOP lub
	posiadaj±ce 3. poziom dostêpu mog± j± wykonywaæ
	jedynie na sobie.

CHAN_HELP_DEVOICE
	Sk³adnia: DEVOICE [kana³ [nick]]
	
	Odbiera prawo g³osu wskazanemu u¿ytkownikowi.
	Je¶li nie zostanie podany nick operacja bêdzie
	przeprowadzona na osobie wydaj±cej t± komendê.
	Je¶li tak¿e nie zostanie podany kana³ operacja
	bêdzie przeprowadzona na wszystkich kana³ach na
	których przebywa osoba wydaj±ca to polecenie z
	uwzglêdnieniem posiadanych uprawnieñ.
	
	Standardowo ta komenda jest dostêpna dla u¿ytkowników
	wpisanych na listê AOP lub posiadaj±cych przynajmniej
	5. poziom dostêpu. Osoby wpisane na listê VOP lub
	posiadaj±ce 3. poziom dostêpu mog± j± wykonywaæ
	jedynie na sobie.

CHAN_HELP_HALFOP
	Sk³adnia: HALFOP [kana³ [nick]]
	
	Nadaje status pó³-operatora wskazanemu u¿ytkownikowi.
	Je¶li nie zostanie podany nick operacja bêdzie
	przeprowadzona na osobie wydaj±cej t± komendê.
	Je¶li tak¿e nie zostanie podany kana³ operacja
	bêdzie przeprowadzona na wszystkich kana³ach na
	których przebywa osoba wydaj±ca to polecenie z
	uwzglêdnieniem posiadanych uprawnieñ.
	
	Standardowo ta komenda jest dostêpna dla u¿ytkowników
	wpisanych na listê AOP lub posiadaj±cych przynajmniej
	5. poziom dostêpu. Osoby wpisane na listê HOP lub
	posiadaj±ce 4. poziom dostêpu mog± j± wykonywaæ
	jedynie na sobie.

CHAN_HELP_DEHALFOP
	Sk³adnia: DEHALFOP [kana³ [nick]]
	
	Odbiera status pó³-operatora wskazanemu u¿ytkownikowi.
	Je¶li nie zostanie podany nick operacja bêdzie
	przeprowadzona na osobie wydaj±cej t± komendê.
	Je¶li tak¿e nie zostanie podany kana³ operacja
	bêdzie przeprowadzona na wszystkich kana³ach na
	których przebywa osoba wydaj±ca to polecenie z
	uwzglêdnieniem posiadanych uprawnieñ.
	
	Standardowo ta komenda jest dostêpna dla u¿ytkowników
	wpisanych na listê AOP lub posiadaj±cych przynajmniej
	5. poziom dostêpu. Osoby wpisane na listê HOP lub
	posiadaj±ce 4. poziom dostêpu mog± j± wykonywaæ
	jedynie na sobie.

CHAN_HELP_PROTECT
	Sk³adnia: PROTECT [kana³ [nick]]
	
	Nadaje ochronê wskazanemu u¿ytkownikowi.
	Je¶li nie zostanie podany nick operacja bêdzie
	przeprowadzona na osobie wydaj±cej t± komendê.
	Je¶li tak¿e nie zostanie podany kana³ operacja
	bêdzie przeprowadzona na wszystkich kana³ach na
	których przebywa osoba wydaj±ca to polecenie z
	uwzglêdnieniem posiadanych uprawnieñ.
	
	Standardowo ta komenda jest dostêpna dla u¿ytkowników
	wpisanych na listê SOP lub posiadaj±cych przynajmniej
	10. poziom dostêpu.

CHAN_HELP_DEPROTECT
	Sk³adnia: DEPROTECT [kana³ [nick]]
	
	Usuwa ochronê wskazanemu u¿ytkownikowi.
	Je¶li nie zostanie podany nick operacja bêdzie
	przeprowadzona na osobie wydaj±cej t± komendê.
	Je¶li tak¿e nie zostanie podany kana³ operacja
	bêdzie przeprowadzona na wszystkich kana³ach na
	których przebywa osoba wydaj±ca to polecenie z
	uwzglêdnieniem posiadanych uprawnieñ.
	
	Standardowo ta komenda jest dostêpna dla u¿ytkowników
	wpisanych na listê SOP lub posiadaj±cych przynajmniej
	10. poziom dostêpu.

CHAN_HELP_OWNER
	Sk³adnia: OWNER [kana³]
	
	Nadaje uprawnienia w³a¶ciciela kana³u.
	Je¶li nie zostanie podany kana³ operacja bêdzie
	przeprowadzona na wszystkich kana³ach na których
	przebywa osoba wydaj±ca to polecenie z
	uwzglêdnieniem posiadanych uprawnieñ.
	
	Standardowo ta komenda jest dostêpna tylko dla w³a¶ciciela kana³u.

CHAN_HELP_DEOWNER
	Sk³adnia: DEOWNER [kana³]
	
	Odbiera uprawnienia w³a¶ciciela kana³u.
	Je¶li nie zostanie podany kana³ operacja bêdzie
	przeprowadzona na wszystkich kana³ach na których
	przebywa osoba wydaj±ca to polecenie z
	uwzglêdnieniem posiadanych uprawnieñ.
	
	Standardowo ta komenda jest dostêpna tylko dla w³a¶ciciela kana³u.

CHAN_HELP_INVITE
	Sk³adnia: INVITE kana³
	
	%S wysy³a u¿ytkownikowi zaproszenie do kana³u.
	
	Standardowo ta komenda jest dostêpna dla u¿ytkowników
	wpisanych na listê AOP lub posiadaj±cych poziom
	dostêpu 5 lub wy¿szy.

CHAN_HELP_UNBAN
	Sk³adnia: UNBAN kana³
	
	Sprawia, ¿e %S zdejmie wszystkie bany, które
	uniemo¿liwiaj± wydaj±cemu komendê wej¶cie na
	wskazany kana³.
	
	Standardowo ta komenda jest dostêpna dla u¿ytkowników
	wpisanych na listê AOP lub posiadaj±cych poziom
	dostêpu 5 lub wy¿szy.

CHAN_HELP_KICK
	Sk³adnia: KICK [kana³ [nick [powód]]]
	
	Wykopuje wskazanego u¿ytkownika z kana³u.
	Pominiêcie nicka powoduje wyrzucenie osoby
	wydaj±cej polecenie. Dodatkowo pominiêcie
	nazwy kana³u powoduje wyrzucenie wydaj±cego
	polecenie ze wszystkich kana³ów, na których
	przebywa z uwzglêdnieniem uprawnieñ na nich
	przys³uguj±cych.
	
	Standardowo ta komenda jest dostêpna dla u¿ytkowników
	wpisanych na listê AOP lub posiadaj±cych poziom
	dostêpu 5 lub wy¿szy.

CHAN_HELP_BAN
	Sk³adnia: BAN [kana³ [nick [powód]]]
	
	Banuje wskazanego u¿ytkownika na kanale.
	Pominiêcie nicka powoduje zbanowanie osoby
	wydaj±cej polecenie. Dodatkowo pominiêcie
	nazwy kana³u powoduje zbanowanie wydaj±cego
	polecenie na wszystkich kana³ach, na których
	przebywa z uwzglêdnieniem uprawnieñ na nich
	przys³uguj±cych.

	Standardowo ta komenda jest dostêpna dla u¿ytkowników
	wpisanych na listê AOP lub posiadaj±cych poziom
	dostêpu 5 lub wy¿szy.

CHAN_HELP_TOPIC
	Sk³adnia: TOPIC kana³ [temat]
	
	Wymusza na serwisie %S zmianê tematu na kanale.
	Pominiêcie tematu powoduje usuniêcie istniej±cego.
	Ta komenda jest u¿yteczna w po³±czeniu z ustawieniem
	TOPICLOCK. Aby uzyskaæ wiêcej informacji wpisz
	%R%S HELP SET TOPICLOCK
	
	Standardowo ta komenda jest dostêpna dla w³a¶ciciela kana³u.

CHAN_HELP_CLEAR
	Sk³adnia: CLEAR kana³ opcje
	
	Umo¿liwia kompleksowe usuniêcie niektórych ustawieñ na
	kanale poprzez %S. Dostêpne opcje:
	
	   MODES     Usuwa tryby z kana³u
	   BANS      Usuwa bany z kana³u
	   EXCEPTS   Usuwa wyj±tki od banów z kana³u
	   INVITES   Usuwa zaproszenia z kana³u
	   OPS       Odbiera wszystkim status operatora
	   HOPS      Odbiera wszystkim status pó³-operatora
	   VOICES    Odbiera wszystkim prawo g³osu
	   USERS     Usuwa u¿ytkowników z kana³u
	
	Standardowo ta komenda jest dostêpna dla w³a¶ciciela kana³u.
	
CHAN_HELP_GETKEY
	Sk³adnia: GETKEY kana³
	
	Zwraca has³o wstêpu do wskazanego kana³u.
	Komenda jest g³ównie przeznaczona dla botów,
	dlatego wynik ma format:
	
	KEY <kana³> <klucz>
	
	Je¶li kana³ nie ma klucza zostanie zwrócona
	warto¶æ "NO KEY".

CHAN_HELP_SENDPASS
	Sk³adnia: SENDPASS kana³
	
	Wysy³a has³o do podanego kana³u na adres e-mail
	przypisany do nicka w³a¶ciciela kana³u. Ta komenda
	pomaga odzyskaæ zapomniane has³o.
	
	Ta komenda w niektórych sieciach mo¿e byæ dostêpna
	jedynie dla osób z uprawnieniami IRC operatora.
	
	Ta komenda jest niedostêpna kiedy has³a s± szyfrowane.

CHAN_SERVADMIN_HELP
	
	Administratorzy serwisów mog± usun±æ ka¿dy kana³
	bez potrzeby identyfikacji jako jego w³a¶ciciele.
	Mog± przegl±daæ listê dostêpu ka¿dego kana³u,
	listê AKICK, ustawienia poziomów dostêpu itd.

CHAN_SERVADMIN_HELP_LOGOUT
	Sk³adnia: LOGOUT kana³ nick
	
	Pozwala na wylogowanie wskazanego u¿ytkownika z kana³u.
	W celu odzyskania uprawnieñ u¿ytkownik bêdzie musia³
	ponownie siê zidentyfikowaæ.
	
	Ka¿dy mo¿e wylogowaæ siebie, natomiast w³a¶ciciel kana³u
	mo¿e wylogowaæ ka¿dego.
	
	Administrator serwisów mo¿e wylogowaæ dowoln± osobê
	z dowolnego kana³u. Pominiêcie nicka powoduje wylogowanie
	wszystkich zidentyfikowanych osób.

CHAN_SERVADMIN_HELP_DROP
	Sk³adnia: DROP kana³
	
	Usuwa wskazany kana³. Tylko administratorzy serwisów
	mog± usun±æ kana³, którego nie s± w³a¶cicielami.

CHAN_SERVADMIN_HELP_SET
	
	Administratorzy serwisów mog± ustawiæ kana³owi 
	opcjê NOEXPIRE, która zapobiega wyga¶niêciu
	kana³u je¶li nie jest on u¿ywany.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Sk³adnia: SET kana³ NOEXPIRE {ON | OFF}
	
	W³±cza lub wy³±cza automatyczne usuniêcie kana³u,
	je¶li nie bêdzie on u¿ywany.

CHAN_SERVADMIN_HELP_INFO
	
	Administratorzy serwisów zawsze mog± u¿ywaæ parametru ALL.

CHAN_SERVADMIN_HELP_LIST
	Sk³adnia: LIST wzorzec [FORBIDDEN] [SUSPENDED] [NOEXPIRE]

	Listuje kana³y o nazwie pasuj±cej do podanego wzorca
	(kana³y z opcj± PRIVATE bêd± pokazywane jedynie
	administratorom serwisów). Kana³y z ustawion± opcj±
	NOEXPIRE bêd± mia³y przy nazwie dodany znak !.

	Administratorzy serwisów mog± tak¿e podaæ parametry:
	FORBIDDEN, SUSPENDED i NOEXPIRE, które ogranicz± listê
	wy¶wietlanych kana³ów do tych, które spe³niaj± podane 
	warunki. Je¶li zostanie podane wiêcej warunków
	wszystkie kana³y spe³niaj±ce choæ jeden z nich 
	zostan± pokazane.

CHAN_SERVADMIN_HELP_GETPASS
	Sk³adnia: GETPASS kana³
	
	Zwraca has³o w³a¶ciciela dla wskazanego kana³u. 
	UWAGA: u¿ycie tej komendy powoduje wys³anie
	informacji o jej u¿yciu przy pomocy WALLOP/GLOBOP.

CHAN_SERVADMIN_HELP_FORBID
	Sk³adnia: FORBID kana³ [powód]
	
	Uniemo¿liwia rejestracjê i korzystanie ze wskazanego
	kana³u. Blokadê mo¿na zdj±æ u¿ywaj±c komendy DROP.
	
	W niektórych sieciach wymagane jest podanie powodu.

CHAN_SERVADMIN_HELP_SUSPEND
	Sk³adnia: SUSPEND kana³ [powód]
	
	Zawiesza dzia³anie wskazanego kana³u.
	Blokada mo¿e zostaæ zdjêta z zachowaniem
	wszelkich ustawieñ komend± UNSUSPEND.
	
	W niektórych sieciach wymagane jest podanie powodu.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Sk³adnia: UNSUSPEND kana³
	
	Uwalnia zawieszony kana³. Wszystkie dane i ustawienia
	s± odtworzone sprzed zawieszenia.

CHAN_SERVADMIN_HELP_STATUS
	Sk³adnia: STATUS kana³ nick
	
	Wy¶wietla aktualny poziom dostêpu (access level) 
	wskazanego nicka na kanale. Format odpowiedzi jest 
	nastêpuj±cy:
	
	    STATUS kana³ nick poziom-dostêpu
	
	W przypadku wyst±pienia b³êdu, format odpowiedzi 
	bêdzie nastêpuj±cy:
	
	    STATUS ERROR informacja-o-b³êdzie

###########################################################################
#
# MemoServ help messages
#
###########################################################################
MEMO_HELP_CMD_SEND
	    SEND        Wysy³a wiadomo¶æ do u¿ytkownika/kana³u

MEMO_HELP_CMD_CANCEL
	    CANCEL      Wycofuje ostatni± wys³an± wiadomo¶æ

MEMO_HELP_CMD_LIST
	    LIST        Wy¶wietla wszystkie wiadomo¶ci

MEMO_HELP_CMD_READ
	    READ        Wy¶wietla jedn± lub wiêcej wiadomo¶ci

MEMO_HELP_CMD_DEL
	    DEL         Kasuje jedn± lub wiêcej wiadomo¶ci

MEMO_HELP_CMD_SET
	    SET         Ustawia opcje zwi±zane z wiadomo¶ciami

MEMO_HELP_CMD_INFO
	    INFO        Wy¶wietla informacje o wiadomo¶ciach

MEMO_HELP_CMD_RSEND
	    RSEND       Wysy³a wiadomo¶æ z ¿±daniem potwierdzenia odbioru

MEMO_HELP_CMD_CHECK
	    CHECK       Sprawdza czy ostatnia wiadomo¶æ zosta³a przeczytana

MEMO_HELP_CMD_SENDALL
	    SENDALL     Wysy³a wiadomo¶æ do wszystkich
	                zarejestrowanych u¿ytkowników

MEMO_HELP_CMD_STAFF
	    STAFF       Wysy³a wiadomo¶æ do wszystkich
	                operatorów/administratorów

MEMO_HELP_HEADER
	%S umo¿liwia wysy³anie krótkich wiadomo¶ci
	do innych u¿ytkowników (niezale¿nie od tego czy
	s± oni w danej chwili na IRCu) lub do kana³ów(*).
	Aby móc u¿ywaæ tego serwisu nadawca i odbiorca
	(kana³ lub nick) musz± byæ zarejestrowani.
	Poni¿sze komendy pozwalaj± na wysy³anie i
	odbieranie wiadomo¶ci, aby ich u¿yæ wpisz:
	%R%S komenda
	Aby uzyskaæ szczegó³owy opis polecenia wpisz:
	%R%S HELP komenda

MEMO_HELP_ADMIN
	nie jest u¿ywane.

MEMO_HELP_OPER
	nie jest u¿ywane.

MEMO_HELP_FOOTER
	
	Napisz %R%S HELP polecenie aby uzyskaæ pomoc
	na temat powy¿szych poleceñ.

	(*) Domy¶lnie ka¿dy u¿ytkownik maj±cy przynajmniej 10.
	    poziom dostêpu na kanale mo¿e czytaæ wiadomo¶ci,
	    ale mo¿na to zmieniæ komend± LEVELS serwisu %s.

MEMO_HELP_SEND
	Sk³adnia: SEND {nick | kana³} tre¶æ-wiadomo¶ci
	
	Wysy³a wiadomo¶æ wskazanej osobie lub na podany kana³.
	Po wys³aniu odbiorca otrzyma powiadomienie o nowej
	wiadomo¶ci (o ile ma w³±czone powiadomienia).
	Adresatem musi byæ zarejestrowany nick lub kana³.

MEMO_HELP_CANCEL
	Sk³adnia: CANCEL {nick | kana³}
	
	Wycofuje ostatni± wys³an± wiadomo¶æ,
	o ile nie zosta³a przeczytana.

MEMO_HELP_LIST
	Sk³adnia: LIST [kana³] [lista | NEW]
	
	Wy¶wietla wszystkie odebrane wiadomo¶ci. Parametr 
	NEW ogranicza listê do nowych wiadomo¶ci, które
	oznaczane s± symbolem "*" po lewej stronie numeru.
	Mo¿na tak¿e sprecyzowaæ listê wy¶wietlanych 
	wiadomo¶ci podaj±c ich numery, np:

	   LIST 2-5,7-9
	      Wy¶wietli wiadomo¶ci od numeru 2 do 5 oraz od 7 do 9.

MEMO_HELP_READ
	Sk³adnia: READ [kana³] {numer | lista | LAST | NEW}
	
	Wy¶wietla tekst wskazanej wiadomo¶ci. Parametr LAST
	oznacza ostatni± z wiadomo¶ci, NEW wy¶wietla wszystkie
	nieprzeczytane wiadomo¶ci. Je¶li zostanie podany numer
	zostanie wy¶wietlona wiadomo¶æ o podanym numerze. Mo¿na
	tak¿e podawaæ listê wiadomo¶ci np.:
	
	   READ 2-5,7-9
	      Wy¶wietli wiadomo¶ci od numeru 2 do 5 oraz od 7 do 9.

MEMO_HELP_DEL
	Sk³adnia: DEL [kana³] {numer | lista | LAST | ALL}
	
	Kasuje jedn± lub wiêcej wiadomo¶ci. Dopuszczalne 
	jest podanie numeru wiadomo¶ci lub listy wiadomo¶ci, 
	przyk³ad poni¿ej.
	
	Parametr LAST oznacza skasowanie ostatniej wiadomo¶ci.
	Parametr ALL oznacza skasowanie wszystkich wiadomo¶ci.
	
	Przyk³ady:
	
	   DEL 1
	      Usuwa pierwsz± wiadomo¶æ.
	
	   DEL 2-5,7-9
	      Kasuje wiadomo¶ci od numeru 2 do 5 oraz od 7 do 9.

MEMO_HELP_SET
	Sk³adnia: SET opcja parametry
	
	Ustawia ró¿ne opcje wiadomo¶ci. Dostêpne opcje:
	
	    NOTIFY      Ustawia kiedy otrzymasz informacjê
	                o nowej wiadomo¶ci
	    LIMIT       Ustawia maksymaln± liczbê wiadomo¶ci
	                jakie mo¿esz odebraæ
	
	Aby uzyskaæ wiêcej informacji o danej opcji wpisz:
	%R%S HELP SET opcja

MEMO_HELP_SET_NOTIFY
	Sk³adnia: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
	
	Ustala sposób wys³ania powiadomieñ o nowych wiadomo¶ciach:
	
	   ON      Powiadomienie zostanie wys³ane po 
	           zalogowaniu siê, kiedy nie bêdzie 
	           ustawiony tryb /AWAY oraz w momencie
	           otrzymania wiadomo¶ci.
	   LOGON   Powiadomienie zostanie wys³ane jedynie 
	           po zalogowaniu siê lub po wy³±czeniu 
	           trybu /AWAY.
	   NEW     Powiadomienie bêdzie wysy³ane jedynie w 
	           momencie otrzymania wiadomo¶ci.
	   MAIL    Powiadomienia bêd± wysy³ane e-mailem oraz
	           wedle pozosta³ych ustawieñ.
	   NOMAIL  Powiadomienia nie bêd± wysy³ane e-mailem.
	   OFF     Powiadomienia nie bêd± wysy³ane.
	
	Opcja ON jest kombinacj± opcji LOGON oraz NEW.

MEMO_HELP_SET_LIMIT
	Sk³adnia: SET LIMIT [kana³] limit
	
	Ustala Twoj± (lub kana³u) maksymaln± liczbê 
	wiadomo¶ci. Je¶li ustalona zostanie ona na 0, 
	przyjmowanie wiadomo¶ci zostanie wy³±czone. Limit 
	nie mo¿e byæ wiêkszy ni¿ %d.

MEMO_HELP_INFO
	Sk³adnia: INFO [kana³]
	
	Wy¶wietla ilo¶æ wiadomo¶ci, ile z nich jest 
	nieprzeczytanych oraz limit przyjmowania 
	wiadomo¶ci. Je¶li podany zostanie kana³ wy¶wietlone 
	informacje bêd± odnosi³y siê do wskazanego kana³u.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Sk³adnia: SET LIMIT [nick | kana³] {limit | NONE} [HARD]
	
	Ustala maksymaln± ilo¶æ wiadomo¶ci jak± mo¿e 
	posiadaæ u¿ytkownik lub kana³. Ustalenie limitu 
	na 0, wy³±czy przyjmowanie wiadomo¶ci. Parametr 
	NONE pozwoli na nielimitowan± ilo¶æ wiadomo¶ci. 
	Je¶li nie podany zostanie nick lub kana³ zmieniony 
	zostanie limit wydaj±cego polecenie.
	
	Dodanie parametru HARD uniemo¿liwia zmianê 
	limitu przez u¿ytkownika.

	To u¿ycie polecenia SET LIMIT jest dostêpne dla 
	administratorów serwisów. Pozostali u¿ytkownicy 
	mog± jedynie modyfikowaæ w³asny limit lub kana³u na
	którym maj± do tego uprawnienia. Nie mog± wy³±czyæ 
	limitu, a tak¿e ustaliæ jego warto¶ci powy¿ej %d 
	oraz u¿yæ parametru HARD.



MEMO_SERVADMIN_HELP_INFO
	Sk³adnia: INFO [nick | kana³]
	
	Polecenie wydane bez parametru wy¶wietla informacje o
	liczbie odebranych wiadomo¶ci, liczbie nowych wiadomo¶ci
	oraz limicie wiadomo¶ci.
	
	Je¶li zostanie wskazany kana³, pokazane zostan± te 
	same informacje w odniesieniu do tego kana³u.
	
	Administratorzy serwisów mog± obejrzeæ informacje 
	dot. dowolnych u¿ytkowników podaj±c parametr nick.
	

MEMO_HELP_STAFF
	Sk³adnia: STAFF tre¶æ-wiadomo¶ci
	
	Wysy³a wiadomo¶æ o podanej tre¶ci do wszystkich
	operatorów i administratorów serwisów. U¿ytkownicy
	wpisani jednocze¶nie na kilka list otrzymaj±
	wiadomo¶ci kilka razy.

MEMO_HELP_SENDALL
	Sk³adnia: SENDALL tre¶æ-wiadomo¶ci
	
	Wysy³a wiadomo¶æ do wszystkich zarejestrowanych u¿ytkowników.

MEMO_HELP_RSEND
	Sk³adnia: RSEND {nick | kana³} tre¶æ-wiadomo¶ci
	
	Wysy³a wiadomo¶æ wskazanej osobie lub na podany kana³.
	Po wys³aniu odbiorca otrzyma powiadomienie o nowej
	wiadomo¶ci (o ile ma w³±czone powiadomienia).
	Adresatem musi byæ zarejestrowany nick lub kana³.
	Kiedy wiadomo¶æ zostanie przeczytana przez odbiorcê
	automatyczne potwierdzenie odbioru zostanie wys³ane
	do nadawcy.
MEMO_HELP_CHECK
	Sk³adnia: CHECK nick
	
	Sprawdza, czy ostatnia wiadomo¶æ wys³ana do nick
	zosta³a przeczytana. Ta komenda nie dzia³a dla kana³ów.

###########################################################################
#
# OperServ help messages
#
###########################################################################
OPER_HELP_CMD_GLOBAL
	    GLOBAL      Wysy³a wiadomo¶æ do wszystkich u¿ytkowników

OPER_HELP_CMD_STATS
	    STATS       Pokazuje informacje o serwisach i sieci

OPER_HELP_CMD_OPER
	    OPER        Zarz±dza list± operatorów serwisów

OPER_HELP_CMD_ADMIN
	    ADMIN       Zarz±dza list± administratorów serwisów

OPER_HELP_CMD_STAFF
	    STAFF       Wy¶wietla ekipê serwisów oraz jej status

OPER_HELP_CMD_MODE
	    MODE        Zmienia flagi kana³u

OPER_HELP_CMD_KICK
	    KICK        Wyrzuca u¿ytkownika z kana³u

OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  Usuwa wszystkie flagi z kana³u

OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  Od³±cza wszystkich u¿ytkowników o danym ho¶cie

OPER_HELP_CMD_AKILL
	    AKILL       Zarz±dza list± AKILL

OPER_HELP_CMD_SGLINE
	    SGLINE      Zarz±dza list± SGLINE

OPER_HELP_CMD_SQLINE
	    SQLINE      Zarz±dza list± SQLINE

OPER_HELP_CMD_SZLINE
	    SZLINE      Zarz±dza list± SZLINE

OPER_HELP_CMD_CHANLIST
	    CHANLIST    Listuje wszystkie kana³y

OPER_HELP_CMD_USERLIST
	    USERLIST    Listuje wszystkich u¿ytkowników

OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   Zarz±dza wiadomo¶ciami powitalnymi

OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  Zarz±dza losowymi wiadomo¶ciami

OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    Zarz±dza wiadomo¶ciami dla operatorów

OPER_HELP_CMD_SESSION
	    SESSION     Wy¶wietla listê sesji

OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   Zarz±dza list± wyj±tków dla limitów sesji

OPER_HELP_CMD_NOOP
	    NOOP        Tymczasowo usuwa wszystkie O:linie na serwerze

OPER_HELP_CMD_JUPE
	    JUPE        Blokuje pod³±czanie podanego serwera

OPER_HELP_CMD_IGNORE
	    IGNORE      Zarz±dza list± ignorowanych przez serwisy

OPER_HELP_CMD_SET
	    SET         Ustawia ró¿ne globalne opcje serwisów

OPER_HELP_CMD_RELOAD
	    RELOAD      £aduje na nowo plik konfiguracyjny

OPER_HELP_CMD_UPDATE
	    UPDATE      Wymusza natychmiastowy zapis baz na dysku

OPER_HELP_CMD_RESTART
	    RESTART     Zapisuje bazy i restartuje serwisy

OPER_HELP_CMD_QUIT
	    QUIT        Wy³±cza serwisy bez zapisywania baz

OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    Wy³±cza serwisy z zapisaniem baz

OPER_HELP_CMD_DEFCON
	    DEFCON      Modyfikuje poziom systemu DefCon

OPER_HELP_CMD_CHANKILL
	    CHANKILL    AKILL wszystkich u¿ytkowników na kanale

OPER_HELP_CMD_OLINE
	    OLINE       Daje flagi operatora wybranemu u¿ytkownikowi

OPER_HELP_CMD_UMODE
	    UMODE       Modyfikuje flagi wskazanego u¿ytkownika

OPER_HELP_CMD_SVSNICK
	    SVSNICK     Wymusza zmianê nicka danego u¿ytkownika

OPER_HELP_CMD_MODLOAD
	    MODLOAD     £aduje modu³

OPER_HELP_CMD_MODUNLOAD
	    MODUNLOAD   Wy³adowuje modu³

OPER_HELP_CMD_MODINFO
	    MODINFO     Wy¶wietla informacja o podanym module

OPER_HELP_CMD_MODLIST
	    MODLIST     Listuje za³adowane modu³y

OPER_HELP
	Komendy %S:

OPER_HELP_LOGGED
	Uwaga: Wszystkie komendy odbierane przez %S s± logowane!

OPER_HELP_GLOBAL
	Sk³adnia: GLOBAL wiadomo¶æ
	
	Pozwala administratorom wysy³aæ wiadomo¶ci do wszystkich
	u¿ytkowników. Wiadomo¶æ zostanie wys³ana z nicka %s.

OPER_HELP_STATS
	Sk³adnia: STATS [AKILL | ALL | RESET | MEMORY | UPLINK]
	
	Polecenie bez parametrów wy¶wietla bie¿±c± liczbê 
	u¿ytkowników oraz operatorów pod³±czonych do sieci (nie
	licz±c serwisów), najwiêksz± liczbê u¿ytkowników licz±c od 
	startu serwisów oraz czas pracy serwisów.
	
	Parametr AKILL wy¶wietla informacje o rozmiarze listy
	AKILL oraz domy¶lnym czasie wygasania wpisów.
	
	Parametr RESET resetuje maksymaln± liczbê u¿ytkowników
	pod³±czonych do sieci.
	
	Parametr MEMORY wy¶wietla informacje o zu¿yciu pamiêci
	przez serwisów. U¿ycie tego polecenia mo¿e chwilowo
	zawiesiæ dzia³anie serwisów na du¿ych sieciach.
	
	Parametr UPLINK wy¶wietla informacje o serwerze, do 
	którego serwisy s± bezpo¶rednio pod³±czone.
	
	Parametr ALL wy¶wietla wszystkie dostêpne informacje.
	
	UPTIME mo¿e byæ u¿ywane jako synonim STATS.

OPER_HELP_OPER
	Sk³adnia: OPER ADD nick
	          OPER DEL {nick | numer-wpisu | lista}
	          OPER LIST [maska | lista]
	          OPER CLEAR
	
	Pozwala g³ównym administratorom serwisów na zarz±dzanie
	list± operatorów serwisów. Osoby na takiej li¶cie po 
	identyfikacji u %s bêd± mog³y korzystaæ z poleceñ
	zarezerwowanych dla operatorów serwisów.
	
	OPER ADD dodaje wskazany nick na listê operatorów.
	
	OPER DEL usuwa wskazane wpisy z listy operatorów.
	Jako argument przyjmuje nick, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).
	
	OPER LIST wy¶wietla listê operatorów serwisów. Je¶li
	zostanie podana maska tylko pasuj±ce wpisy zostan±
	pokazane. Mo¿na tak¿e u¿yæ listy, przyk³ad:

	   OPER LIST 2-5,7-9
	      Listuje operatorów od 2 do 5 oraz od 7 do 9.
	
	OPER CLEAR usuwa wszystkie wpisy z listy operatorów.
	
	Ka¿dy IRC operator mo¿e u¿yæ polecenia OPER LIST.

OPER_HELP_ADMIN
	Sk³adnia: ADMIN ADD nick
	          ADMIN DEL {nick | numer-wpisu | lista}
	          ADMIN LIST [maska | lista]
	          ADMIN CLEAR
	
	Pozwala g³ównym administratorom serwisów na zarz±dzanie
	list± administratorów serwisów. Osoby na takiej li¶cie po 
	identyfikacji u %s bêd± mog³y korzystaæ z poleceñ
	zarezerwowanych dla administratorów serwisów.
	
	ADMIN ADD dodaje wskazany nick na listê administratorów.
	
	ADMIN DEL usuwa wskazane wpisy z listy administratorów.
	Jako argument przyjmuje nick, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).

	ADMIN LIST wy¶wietla listê administratorów serwisów. Je¶li
	zostanie podana maska tylko pasuj±ce wpisy zostan±
	pokazane. Mo¿na tak¿e u¿yæ listy, przyk³ad:

	   ADMIN LIST 2-5,7-9
	      Listuje administratorów od 2 do 5 oraz od 7 do 9.

	ADMIN CLEAR usuwa wszystkie wpisy z listy administratorów.
	
	Ka¿dy IRC operator mo¿e u¿yæ polecenia ADMIN LIST.

OPER_HELP_IGNORE
	Sk³adnia: IGNORE {ADD|DEL|LIST|CLEAR} [czas] [nick | maska]
	
	Pozwala administratorom serwisów na w³±czenie ignorowania
	osób na podstawie nicka lub maski przez okre¶lony czas lub
	do najbli¿szego restartu. Podaj±c czas mo¿na stosowaæ
	jednostki: s dla sekund, m dla minut, h dla godzin
	oraz d dla dni. Kombinacja tych jednostek nie jest mo¿liwa.
	Sta³e ignorowanie uzyskuje siêpodaj±c warto¶æ 0.
	Je¶li maska nie bêdzie w formacie user@host oraz 
	nick!user@host to zostanie potraktowana jako nick.
	Podaj±c maskê mo¿na korzystaæ z symboli wieloznacznych.

	IRC operatorzy nie bêd± ignorowani.
	
OPER_HELP_MODE
	Sk³adnia: MODE kana³ flagi
	
	Pozwala operatorom serwisów na zmianê flag dla wskazanego
	kana³u. Parametry s± identyczne jak dla komendy /MODE.

OPER_HELP_UMODE
	Sk³adnia: UMODE u¿ytkownik flagi
	
	Pozwala super-administratorom zmieniaæ flagi
	innym u¿ytkownikom. Parametry s± identyczne jak
	dla komendy /MODE.

OPER_HELP_OLINE
	Sk³adnia: OLINE nick flagi
	
	Pozwala super-administratorom na nadanie wskazanemu
	u¿ytkownikowi uprawnieñ irc operatora z okre¶lonymi
	flagami. Flagi powinny byæ poprzedzone znakami "+" lub "-". 
	Aby usun±æ wszystkie flagi wystarczy podaæ "-".

OPER_HELP_CLEARMODES
	Sk³adnia: CLEARMODES kana³ [ALL]
	
	Usuwa wszystkie binarne flagi (i,k,l,m,n,p,s,t) oraz bany
	ustawione na wskazanym kanale. Dodanie parametru ALL
	odbiera tak¿e uprawnienia (+o, +v).

OPER_HELP_KICK
	Sk³adnia: KICK kana³ nick powód
	
	Pozwala administratorom wyrzuciæ u¿ytkownika z
	dowolnego kana³u. Parametry s± identyczne jak
	dla komendy /KICK. Do powodu zostaje do³±czony
	nick operatora, który wydaje polecenie.
	
	*** SpamMan has been kicked off channel #my_channel by %S (Alcan (Flood))

OPER_HELP_SVSNICK
	Sk³adnia: SVSNICK nick nowy-nick
	
	Zmienia nick wskazanemu u¿ytkownikowi.

OPER_HELP_AKILL
	Sk³adnia: AKILL ADD [+czas-trwania] maska powód
	          AKILL DEL {maska | numer-wpisu | lista}
	          AKILL LIST [maska | lista]
	          AKILL VIEW [maska | lista]
	          AKILL CLEAR
	
	Pozwala operatorom serwisów na zarz±dzanie list± AKILL.
	Je¶li u¿ytkownik pasuj±cy do maski na li¶cie pod³±czy siê
	do sieci serwisy natychmiast go roz³±cz± oraz za³o¿± K-line
	na jego host.
	
	AKILL ADD dodaje maskê (w formacie user@host lub ip)
	oraz powód (który musi zostaæ podany) na listê AKILL.
	Podaj±c czas mo¿na stosowaæ jednostki: s dla sekund,
	m dla minut, h dla godzin oraz d dla dni.
	Kombinacja tych jednostek nie jest mo¿liwa. Domy¶ln±
	jednostk± s± dni (+30 oznacza 30 dni). Podanie +0
	oznacza, ¿e wpis nie wyga¶nie. Je¶li maska zaczyna siê
	znakiem + to podanie czasu jest obowi±zkowe (nawet je¶li
	jest identyczny z domy¶lnym). Domy¶lny czas wygasania
	mo¿na sprawdziæ przy pomocy polecenia STATS AKILL.
	
	AKILL DEL usuwa wskazane wpisy z listy AKILL.
	Jako argument przyjmuje nick, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).

	AKILL LIST wy¶wietla zawarto¶æ listy AKILL. Je¶li
	zostanie podana maska tylko pasuj±ce wpisy zostan±
	pokazane. Mo¿na tak¿e u¿yæ listy, przyk³ad:

	   AKILL LIST 2-5,7-9
	      Listuje wpisy od 2 do 5 oraz od 7 do 9.
	
	AKILL VIEW zwraca wiêcej szczegó³ów ni¿ AKILL LIST.
	Wy¶wietla informacje o dodaj±cym AKILL, datê dodania,
	datê wyga¶niêcia, maskê oraz powód.
	
	AKILL CLEAR usuwa wszystkie wpisy z listy AKILL.
	

OPER_HELP_SGLINE
	Sk³adnia: SGLINE ADD [+czas-trwania] maska powód
	          SGLINE DEL {maska | numer-wpisu | lista}
	          SGLINE LIST [maska | lista]
	          SGLINE VIEW [maska | lista]
	          SGLINE CLEAR
	
	Pozwala operatorom serwisów na zarz±dzanie list± SGLINE.
	Je¶li u¿ytkownik z realname pasuj±cym do maski na li¶cie
	pod³±czy siê do sieci serwisy natychmiast go roz³±cz±.
	
	SGLINE ADD dodaje wskazan± maskê realname oraz 
	powód (który musi zostaæ podany) na listê SGLINE.
	Podaj±c czas mo¿na stosowaæ jednostki: s dla sekund,
	m dla minut, h dla godzin oraz d dla dni.
	Kombinacja tych jednostek nie jest mo¿liwa. Domy¶ln±
	jednostk± s± dni (+30 oznacza 30 dni). Podanie +0
	oznacza, ¿e wpis nie wyga¶nie. Je¶li maska zaczyna siê
	znakiem + to podanie czasu jest obowi±zkowe (nawet je¶li
	jest identyczny z domy¶lnym). Domy¶lny czas wygasania
	mo¿na sprawdziæ przy pomocy polecenia STATS AKILL.
	Uwaga: realname mo¿e zawieraæ spacje, dlatego
	separatorem pomiêdzy mask± i powodem jest dwukropek.
	
	SGLINE DEL usuwa wskazane wpisy z listy SGLINE.
	Jako argument przyjmuje maskê, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).
	
	SGLINE LIST wy¶wietla zawarto¶æ listy SGLINE. Je¶li
	zostanie podana maska tylko pasuj±ce wpisy zostan±
	pokazane. Mo¿na tak¿e u¿yæ listy, przyk³ad:

	   SGLINE LIST 2-5,7-9
	      Listuje wpisy od 2 do 5 oraz od 7 do 9.
	
	SGLINE VIEW zwraca wiêcej szczegó³ów ni¿ SGLINE LIST.
	Wy¶wietla informacje o dodaj±cym SGLINE, datê dodania,
	datê wyga¶niêcia, maskê oraz powód.
	
	SGLINE CLEAR usuwa wszystkie wpisy z listy.

OPER_HELP_SQLINE
	Sk³adnia: SQLINE ADD [+czas-trwania] maska powód
	          SQLINE DEL {maska | numer-wpisu | lista}
	          SQLINE LIST [maska | lista]
	          SQLINE VIEW [maska | lista]
	          SQLINE CLEAR
	
	Pozwala operatorom serwisów na zarz±dzanie list± SQLINE.
	Je¶li u¿ytkownik z nickiem pasuj±cym do maski na li¶cie
	pod³±czy siê do sieci serwisy natychmiast go roz³±cz±.
	
	Je¶li pierwszym znakiem maski jest # serwisy zablokuj±
	mo¿liwo¶æ korzystania z kana³ów o nazwie pasuj±cej do
	niej (o ile IRCd obs³uguje taki rodzaj blokady).
	
	SQLINE ADD dodaje wskazan± maskê oraz powód
	(który musi zostaæ podany) na listê SQLINE.
	Podaj±c czas mo¿na stosowaæ jednostki: s dla sekund,
	m dla minut, h dla godzin oraz d dla dni.
	Kombinacja tych jednostek nie jest mo¿liwa. Domy¶ln±
	jednostk± s± dni (+30 oznacza 30 dni). Podanie +0
	oznacza, ¿e wpis nie wyga¶nie. Je¶li maska zaczyna siê
	znakiem + to podanie czasu jest obowi±zkowe (nawet je¶li
	jest identyczny z domy¶lnym). Domy¶lny czas wygasania
	mo¿na sprawdziæ przy pomocy polecenia STATS AKILL.
	
	SQLINE DEL usuwa wskazane wpisy z listy SQLINE.
	Jako argument przyjmuje maskê, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).
	
	SQLINE LIST wy¶wietla zawarto¶æ listy SQLINE. Je¶li
	zostanie podana maska tylko pasuj±ce wpisy zostan±
	pokazane. Mo¿na tak¿e u¿yæ listy, przyk³ad:

	   SQLINE LIST 2-5,7-9
	      Listuje wpisy od 2 do 5 oraz od 7 do 9.
	
	SQLINE VIEW zwraca wiêcej szczegó³ów ni¿ SQLINE LIST.
	Wy¶wietla informacje o dodaj±cym SQLINE, datê dodania,
	datê wyga¶niêcia, maskê oraz powód.

	SQLINE CLEAR usuwa wszystkie wpisy z listy.

OPER_HELP_SZLINE
	Sk³adnia: SZLINE ADD [+czas-trwania] maska powód
	          SZLINE DEL {maska | numer-wpisu | lista}
	          SZLINE LIST [maska | lista]
	          SZLINE VIEW [maska | lista]
	          SZLINE CLEAR
	
	Pozwala operatorom serwisów na zarz±dzanie list± SZLINE.
	Je¶li u¿ytkownik z adresem IP pasuj±cym do maski na li¶cie
	pod³±czy siê do sieci serwisy natychmiast go roz³±cz±.
	
	SZLINE ADD dodaje wskazan± maskê oraz powód
	(który musi zostaæ podany) na listê SZLINE.
	Podaj±c czas mo¿na stosowaæ jednostki: s dla sekund,
	m dla minut, h dla godzin oraz d dla dni.
	Kombinacja tych jednostek nie jest mo¿liwa. Domy¶ln±
	jednostk± s± dni (+30 oznacza 30 dni). Podanie +0
	oznacza, ¿e wpis nie wyga¶nie. Je¶li maska zaczyna siê
	znakiem + to podanie czasu jest obowi±zkowe (nawet je¶li
	jest identyczny z domy¶lnym). Domy¶lny czas wygasania
	mo¿na sprawdziæ przy pomocy polecenia STATS AKILL.
	
	SZLINE DEL usuwa wskazane wpisy z listy SZLINE.
	Jako argument przyjmuje maskê, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).
	
	SZLINE LIST wy¶wietla zawarto¶æ listy SZLINE. Je¶li
	zostanie podana maska tylko pasuj±ce wpisy zostan±
	pokazane. Mo¿na tak¿e u¿yæ listy, przyk³ad:

	   SZLINE LIST 2-5,7-9
	      Listuje wpisy od 2 do 5 oraz od 7 do 9.
	
	SZLINE VIEW zwraca wiêcej szczegó³ów ni¿ SZLINE LIST.
	Wy¶wietla informacje o dodaj±cym SZLINE, datê dodania,
	datê wyga¶niêcia, maskê oraz powód.

	SZLINE CLEAR usuwa wszystkie wpisy z listy.

OPER_HELP_SET
	Sk³adnia: SET opcja warto¶æ
	
	Ustawia ró¿ne globalne ustawienia serwisów.
	Obecnie zdefiniowane s± nastêpuj±ce ustawienia:
	    READONLY   W³±cza tryb tylko-do-odczytu lub odczyt-zapis
	    LOGCHAN    W³±cza raportowanie na kana³
	    DEBUG      W³±cza lub wy³±cza tryb debugowania
	    NOEXPIRE   W³±cza lub wy³±cza tryb noexpire
	    SUPERADMIN W³±cza lub wy³±cza tryb super administratora
	    SQL        W³±cza lub wy³±cza obs³ugê SQL
	    IGNORE     W³±cza lub wy³±cza tryb ignorowania
	    LIST       Listuje aktualne ustawienia

OPER_HELP_SET_READONLY
	Sk³adnia: SET READONLY {ON | OFF}
	
	W³±cza lub wy³±cza tryb tylko-do-odczytu. W³±czenie tej
	opcji powoduje zablokowanie wykonywania zmian w ustawieniach
	serwisów, dotyczy to list dostêpu kana³u oraz nicka itd.
	Operatorzy serwisów z odpowiednimi uprawnieniami bêd±
	mogli modyfikowaæ listê AKILL, usuwaæ lub blokowaæ nicki
	oraz kana³y, ale ¿adne z tych zmian nie zostan± zapisane
	dopóki tryb tylko-do-odczytu nie zostanie wy³±czony przed
	restartem lub wy³±czeniem serwisów.
	
	Ta opcja ma taki sam efekt jak argument linii poleceñ -readonly.

OPER_HELP_SET_LOGCHAN
	Sk³adnia: SET LOGCHAN {ON | OFF}
	
	W³±cza lub wy³±cza wysy³anie wiadomo¶ci na podany kana³, które
	trafiaj± do logów. Aby mo¿na by³o skorzystaæ z tej opcji nale¿y
	zdefiniowaæ w pliku konfiguracyjnym nazwê kana³u (LogChannel).

	Ta opcja ma taki sam efekt jak argument linii poleceñ -logchan.

	Uwaga: Ustawiony kana³ musi byæ odpowiednio zabezpieczony,
	aby chroniæ prywatno¶æ u¿ytkowników.
OPER_HELP_SET_DEBUG
	Sk³adnia: SET DEBUG {ON | OFF | poziom}
	
	W³±cza lub wy³±cza tryb debugowania. W tym trybie wszystkie
	dane wysy³ane do serwisów i przez nie oraz wiele innych
	komunikatów zapisywane s± do loga. Je¶li zostanie podany 
	numer poziomu poziom tryb debugowania jest aktywowany
	ze wskazanym poziomem.
	
	Ta opcja ma taki sam efekt jak argument linii poleceñ -debug.

OPER_HELP_SET_NOEXPIRE
	Sk³adnia: SET NOEXPIRE {ON | OFF}
	
	W³±cza lub wy³±cza tryb wygasania. Je¶li ta opcja
	zosta³a w³±czona nicki, kana³y, wpisy na listach
	AKILL oraz wyj±tków nie bêd± automatycznie wygasa³y.
	
	Ta opcja ma taki sam efekt jak argument linii poleceñ -noexpire.

OPER_HELP_SET_SUPERADMIN
	Sk³adnia: SET SUPERADMIN {ON | OFF}
	
	Ustawienie tej opcji nadaje wydaj±cemu polecenie
	dodatkowe uprawnienia (np. jest w³a¶cicielem 
	wszystkich kana³ów w sieci).
	
	Ta opcja powinna byæ u¿ywana tylko wtedy, kiedy
	jest taka potrzeba. Nale¿y j± pó¼niej wy³±czyæ.

OPER_HELP_SET_SQL
	Sk³adnia: SET SQL {ON | OFF}
	
	W³±cza lub wy³±cza korzystanie z bazy danych SQL.
	Komenda ta pozwala na wy³±czenie serwera SQL
	podczas nieprzerwanej pracy serwisów.

OPER_HELP_SET_IGNORE
	Sk³adnia: SET IGNORE {ON | OFF}
	
	To ustawienie pozwala w³±czaæ/wy³±czaæ system ignorowania.

OPER_HELP_SET_LIST
	Sk³adnia: SET LIST
	
	Listuje aktualne ustawienia serwisu %S.

OPER_HELP_NOOP
	Sk³adnia: NOOP SET serwer
	          NOOP REVOKE serwer
	
	NOOP SET usuwa wszystkie O:linie ze wskazanego
	serwera oraz killuje wszystkich operatorów, aby
	zapobiec zrehashowaniu serwera (prze³adowanie pliku
	konfiguracyjnego usuwa efekt tego polecenia).
	
	NOOP REVOKE powoduje przywrócenie wszystkich 
	uprawnieñ na wskazanym serwerze.
	
	Uwaga: Serwisy nie sprawdzaj± warto¶ci serwer.

OPER_HELP_JUPE
	Sk³adnia: JUPE serwer [powód]
	
	To polecenie powoduje wprowadzenie przez serwisy do sieci
	sztucznego serwera, co zapobiega pod³±czaniu prawdziwego
	serwera o tej samej nazwie. Blokadê mo¿na zdj±æ u¿ywaj±c
	standardowego polecenia SQUIT. Je¶li zostanie podany
	powód bêdzie on wy¶wietlany w polu informacyjnym serwera,
	w przeciwnym wypadku wy¶wietlane bêdzie "Juped by <nick>".

OPER_HELP_RAW
	Sk³adnia: RAW tekst
	
	Wysy³a tekst bezpo¶rednio do serwera, do którego s±
	pod³±czone serwisy. To polecenie ma bardzo ograniczone
	zastosowanie i mo¿e powodowaæ chaos, kiedy jest u¿ywane
	w sposób nieprawid³owy. NIE U¯YWAJ TEGO POLECENIA
	je¶li nie masz pewno¶ci co robisz.

OPER_HELP_UPDATE
	Sk³adnia: UPDATE
	
	Wymusza natychmiastowy zapis baz danych na dysku.

OPER_HELP_RELOAD
	Sk³adnia RELOAD
	
	Wymusza ponowne za³adowanie pliku konfiguracyjnego.
	Niektóre opcje wci±¿ wymagaj± restartu serwisów
	(zmiana nicków serwisów, aktywacja limitów sesji itd.).
OPER_HELP_QUIT
	Sk³adnia: QUIT
	
	Wymusza na serwisach natychmiastowe zakoñczenie pracy bez
	zapisywania baz. To polecenie powinno byæ u¿yte jedynie,
	gdy zachodzi podejrzenie uszkodzenia baz, które serwisy
	przechowuj± w pamiêci. Do normalnego zakoñczenia pracy
	serwisów nale¿y u¿yæ polecenia SHUTDOWN.

OPER_HELP_SHUTDOWN
	Sk³adnia: SHUTDOWN
	
	Wymusza zapisanie baz danych, a nastêpnie wy³±cza serwisy.

OPER_HELP_RESTART
	Sk³adnia: RESTART
	
	Wymusza zapisanie baz, a nastêpnie restartuje serwisy.

OPER_HELP_CHANLIST
	Sk³adnia: CHANLIST [{wzorzec | nick} [SECRET]]
	
	Listuje wszystkie istniej±ce kana³y w sieci, niezale¿nie od
	tego czy s± zarejestrowane.
	
	Podanie wzorca ogranicza listê pokazywanych kana³ów.
	Podanie nicka wy¶wietla kana³y, na których przebywa
	wskazany u¿ytkownik. Dodanie parametru SECRET ogranicza
	listê do kana³ów z flag± +s lub +p.

OPER_HELP_USERLIST
	Sk³adnia: USERLIST [{wzorzec | kana³} [INVISIBLE]]
	
	Listuje wszystkich u¿ytkowników w sieci, niezale¿nie od
	tego czy maj± zarejestrowanego nicka.
	
	Podanie wzorca ogranicza listê pokazywanych u¿ytkowników.
	(musi byæ w formacie nick!ident@host). Podanie kana³u 
	listuje u¿ytkowników przebywaj±cych na wskazanym kanale.
	Dodanie parametru INVISIBLE listuje tylko u¿ytkowników z flag± +i.

OPER_HELP_MODLOAD
	Sk³adnia: MODLOAD nazwa-pliku
	
	To polecenie ³aduje wskazany modu³ z katalogu modu³ów.

OPER_HELP_MODUNLOAD
	Sk³adnia: MODUNLOAD nazwa-pliku
	
	To polecenie wy³aduje wskazany modu³.

OPER_HELP_MODINFO
	Sk³adnia: MODINFO nazwa-pliku
	
	To polecenie wy¶wietla informacje na temat za³adowanego modu³u

OPER_HELP_MODLIST
	Sk³adnia: MODLIST [Core|3rd|protocol|encryption|supported|qatested]
	
	Listuje za³adowane modu³y.

###########################################################################
#
# BotServ help messages
#
###########################################################################
BOT_HELP_CMD_BOTLIST
	    BOTLIST        Listuje dostêpne boty

BOT_HELP_CMD_ASSIGN
	    ASSIGN         Przydziela bota na kana³

BOT_HELP_CMD_SET
	    SET            Konfiguruje bota

BOT_HELP_CMD_KICK
	    KICK           Konfiguruje opcje kopania

BOT_HELP_CMD_BADWORDS
	    BADWORDS       Zarz±dza list± zakazanych s³ów

BOT_HELP_CMD_ACT
	    ACT            Wyra¿a przez bota akcjê (komenda /me)

BOT_HELP_CMD_INFO
	    INFO           Pokazuje informacje i ustawienia BotServa
	                   dotycz±ce bota lub kana³u

BOT_HELP_CMD_SAY
	    SAY            Wypowiada przez bota tekst na kanale

BOT_HELP_CMD_UNASSIGN
	    UNASSIGN       Usuwa bota z kana³u

BOT_HELP_CMD_BOT
	    BOT            Zarz±dza sieciow± list± botów

BOT_HELP
	%S pozwala umie¶ciæ bota na w³asnym kanale.
	Zosta³ stworzony dla u¿ytkowników, którzy nie mog±
	hostowaæ w³asnego bota oraz dla tych sieci które
	nie pozwalaj± na ich u¿ywanie. Poni¿sze komendy
	umo¿liwiaj± zarz±dzanie botem, aby ich u¿yæ wpisz:
	%R%S komenda
	Aby uzyskaæ szczegó³owy opis polecenia wpisz:
	%R%S HELP komenda

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
	
	U¿ycie tej komendy usuwa bota z kana³u.
	Bêdzie mo¿na go ponownie przydzieliæ w ka¿dej
	chwili, bez potrzeby ponownego konfigurowania.

BOT_HELP_INFO
	Sk³adnia: INFO {kana³ | nick}
	
	Wy¶wietla informacje serwisu %S o wskazanym kanale
	lub bocie. Je¶li parametrem jest nazwa kana³u to
	zostan± pokazane informacje dotycz±ce kana³u.
	Je¶li parametrem bêdzie nick bota to zostan±
	pokazane informacje takie jak: data utworzenia,
	ilo¶æ obs³ugiwanych kana³ów itp.

BOT_HELP_SET
	Sk³adnia: SET (kana³ | bot) opcja parametry
	
	Umo¿liwia konfiguracjê nastêpuj±cych opcji bota:
	
	   DONTKICKOPS      Bot nie kopie operatorów
	   DONTKICKVOICES   Bot nie kopie osób z prawem g³osu
	   GREET            W³±cza wiadomo¶æ powitaln±
	   FANTASY          W³±cza publiczne komendy
	   SYMBIOSIS        Przejmuje zadania ChanServa
	
	Aby uzyskaæ wiêcej informacji wpisz:
	%R%S HELP SET opcja
	
	Informacja: dostêp do tej komendy jest kontrolowany
	przez poziom SET.
BOT_HELP_SET_DONTKICKOPS
	Sk³adnia: SET kana³ DONTKICKOPS {ON|OFF}
	
	W³±cza lub wy³±cza opcjê kopania u¿ytkowników
	posiadaj±cych status operatora nawet je¶li nie
	przys³uguje im poziom dostêpu NOKICK.

BOT_HELP_SET_DONTKICKVOICES
	Sk³adnia: SET kana³ DONTKICKVOICES {ON|OFF}
	
	W³±cza lub wy³±cza opcjê kopania u¿ytkowników
	posiadaj±cych prawo g³osu nawet je¶li nie
	przys³uguje im poziom dostêpu NOKICK.

BOT_HELP_SET_FANTASY
	Sk³adnia: SET kana³ FANTASY {ON|OFF}
	
	W³±cza lub wy³±cza dostêp do publicznych poleceñ
	na kanale takich jak: !op, !deop, !voice, !devoice,
	!kick, !kb, !unban, !seen (cze¶æ komend przyjmuje
	jako parametry nicki i powody).
	
	U¿ytkownicy dopuszczeni do u¿ywania komend
	publicznych musz± mieæ odpowiedni poziom dostêpu
	do wydania danego polecenia (np. aby u¿ywac !deop
	trzeba posiadaæ poziom dostêpu OPDEOP).

BOT_HELP_SET_GREET
	Sk³adnia: SET kana³ GREET {ON|OFF}
	
	W³±cza lub wy³±cza witanie podczas wej¶cia u¿ytkowników
	ich w³asn± wiadomo¶ci± powitaln±. U¿ytkownik zostanie
	powitany, je¶li bêdzie mia³ odpowiedni poziom dostêpu.

BOT_HELP_SET_SYMBIOSIS
	Sk³adnia: SET kana³ SYMBIOSIS {ON|OFF}
	
	W³±cza lub wy³±cza opcjê symbiozy na kanale.
	Gdy ten tryb jest w³±czony wszystkie operacje,
	które wykonywa³ %s (np. opowanie) bêd±
	wykonywane przez bota.

BOT_HELP_KICK
	Sk³adnia: KICK kana³ opcja parametry
	
	Konfiguruje opcje kopania na kanale:
	
	   BOLDS        Kopanie za u¿ywanie pogrubieñ
	   BADWORDS     Kopanie za u¿ywanie zakazanych s³ów
	   CAPS         Kopanie za pisanie WIELKIMI LITERAMI
	   COLORS       Kopanie za stosowanie kolorów
	   FLOOD        Kopanie za flood
	   REPEAT       Kopanie za wielokrotne powtarzanie
	   REVERSES     Kopanie za odwracanie kolorów
	   UNDERLINES   Kopanie za u¿ywanie podkre¶leñ
	
	Aby uzyskaæ wiêcej informacji wpisz:
	%R%S HELP KICK opcja

BOT_HELP_KICK_BOLDS
	Sk³adnia: KICK kana³ BOLDS {ON|OFF} [ttb]
	
	Ustawia kopanie za pogrubianie tekstu.
	
	ttb to liczba okre¶laj±ca ile razy u¿ytkownik
	zostanie wykopany zanim bot za³o¿y bana.
	Pominiêcie parametru ttb wy³±cza banowanie.

BOT_HELP_KICK_COLORS
	Sk³adnia: KICK kana³ COLORS {ON|OFF} [ttb]
	
	Ustawia kopanie za u¿ywanie kolorowego tekstu.
	
	ttb to liczba okre¶laj±ca ile razy u¿ytkownik
	zostanie wykopany zanim bot za³o¿y bana.
	Pominiêcie parametru ttb wy³±cza banowanie.

BOT_HELP_KICK_REVERSES
	Sk³adnia: KICK kana³ REVERSES {ON|OFF} [ttb]
	
	Ustawia kopanie za odwracanie kolorów tekstu (ctrl+r).
	
	ttb to liczba okre¶laj±ca ile razy u¿ytkownik
	zostanie wykopany zanim bot za³o¿y bana.
	Pominiêcie parametru ttb wy³±cza banowanie.

BOT_HELP_KICK_UNDERLINES
	Sk³adnia: KICK kana³ UNDERLINES {ON|OFF} [ttb]
	
	Ustawia kopanie za podkre¶lanie tekstu.
	
	ttb to liczba okre¶laj±ca ile razy u¿ytkownik
	zostanie wykopany zanim bot za³o¿y bana.
	Pominiêcie parametru ttb wy³±cza banowanie.

BOT_HELP_KICK_CAPS
	Sk³adnia: KICK kana³ CAPS {ON|OFF} [ttb [min [procent]]]
	
	Ustawia kopanie za u¿ywanie WIELKICH LITER.
	
	Bot kopie je¶li tekst zawiera przynajmniej min
	wielkich liter i stanowi przynajmniej procent
	ca³ego tekstu w linii (w przypadku nie podania tych
	warto¶ci przyjmowane s± domy¶lne: 10 znaków i 25%).
	
	ttb to liczba okre¶laj±ca ile razy u¿ytkownik
	zostanie wykopany zanim bot za³o¿y bana.
	Pominiêcie parametru ttb wy³±cza banowanie.

BOT_HELP_KICK_FLOOD
	Sk³adnia: KICK kana³ FLOOD {ON|OFF} [ttb [ln [sek]]]
	
	Ustawia kopanie za flood. W³±czenie tej opcji
	powoduje wykopywanie u¿ytkowników wysy³aj±cych na
	kana³ przynajmniej ln linii tekstu w ci±gu sek
	sekund (w przypadku nie podania tych warto¶ci przyjmowane
	s± domy¶lne: 6 linii w ci±gu 10 sekund).
	
	ttb to liczba okre¶laj±ca ile razy u¿ytkownik
	zostanie wykopany zanim bot za³o¿y bana.
	Pominiêcie parametru ttb wy³±cza banowanie.

BOT_HELP_KICK_REPEAT
	Sk³adnia: KICK kana³ REPEAT {ON|OFF} [ttb [liczba]]
	
	Ustawia kopanie za powtarzanie tekstu. W³±czenie tej
	opcji powoduje wykopywanie u¿ytkowników wysy³aj±cych
	na kana³ przynajmniej liczba razy ten sam tekst
	(w przypadku nie podania tego parametru jego warto¶æ
	ustalana jest domy¶lnie na 3).
	
	ttb to liczba okre¶laj±ca ile razy u¿ytkownik
	zostanie wykopany zanim bot za³o¿y bana.
	Pominiêcie parametru ttb wy³±cza banowanie.

BOT_HELP_KICK_BADWORDS
	Sk³adnia: KICK kana³ BADWORDS {ON|OFF} [ttb]
	
	Ustawia kopanie za u¿ywanie zakazanych s³ów.
	
	Aby ustaliæ listê zakazanych u¿yj polecenia BADWORDS.
	Aby uzyskaæ wiêcej informacji wpisz:
	%R%S HELP BADWORDS
	
	ttb to liczba okre¶laj±ca ile razy u¿ytkownik
	zostanie wykopany zanim bot za³o¿y bana.
	Pominiêcie parametru ttb wy³±cza banowanie.

BOT_HELP_BADWORDS
	Sk³adnia: BADWORDS kana³ ADD s³owo [SINGLE | START | END]
	          BADWORDS kana³ DEL {s³owo | numer-wpisu | lista}
	          BADWORDS kana³ LIST [maska | lista]
	          BADWORDS kana³ CLEAR
	
	Polecenie umo¿liwia zarz±dzanie list± niedozwolonych
	s³ów. Lista definiuje za jakie s³owa u¿ytkownicy mog±
	zostaæ wyrzuceni z kana³u. Aby uzyskaæ wiêcej informacji
	napisz: %R%S HELP KICK BADWORDS
	
	BADWORDS ADD dodaje wskazane s³owo do listy.
	Parametr SINGLE powoduje, ¿e u¿ytkownicy bêd± kopani za
	u¿ywanie wskazanego s³owa w ca³o¶ci.
	Parametr START powoduje, ¿e u¿ytkownicy bêd± kopani za
	u¿ywanie wskazanego s³owa na pocz±tku innych wyrazów.
	Parametr END powoduje, ¿e u¿ytkownicy bêd± kopani za
	u¿ywanie wskazanego s³owa na koñcu innych wyrazów.
	Pominiêcie parametru powoduje, ¿e u¿ytkownicy bêd±
	kopani w ka¿dym z przypadków, kiedy zostanie u¿yte 
	wskazane s³owo.
	
	BADWORDS DEL usuwa wskazane wpisy z listy s³ów.
	Jako argument przyjmuje s³owo, numer wpisu lub
	listê (przyk³ad poni¿ej dla polecenia LIST).

	BADWORDS LIST wy¶wietla zawarto¶æ listy s³ów. Je¶li
	zostanie podana maska tylko pasuj±ce wpisy zostan±
	pokazane. Mo¿na tak¿e u¿yæ listy, przyk³ad:

	   BADWORDS kana³ LIST 2-5,7-9
	      Listuje wpisy od numeru 2 do 5 oraz od 7 do 9.
	
	BADWORDS CLEAR usuwa wszystkie wpisy z listy.

BOT_HELP_SAY
	Sk³adnia: SAY kana³ tekst
	
	Wypowiada przez bota podany tekst na kanale.

BOT_HELP_ACT
	Sk³adnia: ACT kana³ tekst
	
	Sprawia, ¿e bot u¿yje komendy akcji (/me) przy
	wypowiadaniu podanego tekstu na kanale.

BOT_SERVADMIN_HELP_BOT
	Sk³adnia: BOT ADD nick ident host nazwa
	          BOT CHANGE starynick nowynick [ident [host [nazwa]]]
	          BOT DEL nick
	
	Polecenie pozwala administratorom serwisów na
	tworzenie, modyfikowanie oraz kasowanie botów
	u¿ywanych przez u¿ytkowników sieci na kana³ach.
	
	BOT ADD dodaje bota o podanym nicku, idencie,
	ho¶cie oraz realname.
	BOT CHANGE pozwala na zmianê nicka, identa,
	hosta lub realname bota bez jego kasowania.
	BOT DEL usuwa wskazanego bota.
	
	UWAGA: Je¶li stworzony zostanie bot o nicku,
	który jest ju¿ zarejestrowany, nick zostanie
	odrejestrowany. Podobnie je¶li nick stworzonego
	bota bêdzie u¿ywany przez innego u¿ytkownika,
	zostanie on wtedy skillowany.

BOT_SERVADMIN_HELP_SET
	
	Opcje dostêpne dla administratorów serwisów:
	
	   NOBOT     Uniemo¿liwia przypisanie bota do kana³u
	   PRIVATE   Zastrzega przypisywanie bota tylko dla IRC
	             Operatorów

BOT_SERVADMIN_HELP_SET_NOBOT
	Sk³adnia: SET kana³ NOBOT {ON|OFF}
	
	Polecenie to pozwala okre¶liæ kana³, dla którego
	nie wolno przypisywaæ botów. Je¶li w danej chwili
	bot jest przypisany do kana³u, zostanie usuniêty.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Sk³adnia: SET nick-bota PRIVATE {ON|OFF}
	
	Polecenie pozwala na ograniczenie przypisywania
	wskazanego bota dla u¿ytkowników nie bêd±cych
	IRC operatorami.

###########################################################################
#
# HostServ messages
#
###########################################################################
HOST_EMPTY
	Lista vhostów jest pusta.

HOST_ENTRY
	#%d Nick:%s, vhost:%s (%s - %s)

HOST_IDENT_ENTRY
	#%d Nick:%s, vhost:%s@%s (%s - %s)

HOST_SET
	Vhost dla %s zosta³ zmieniony na %s.

HOST_IDENT_SET
	Vhost dla %s zosta³ zmieniony na %s@%s.

HOST_SETALL
	Vhosty dla grupy %s zosta³y zmienione na %s.

HOST_DELALL
	Vhosty dla grupy %s zosta³y usuniête.

HOST_DELALL_SYNTAX
	DELALL <nick>.

HOST_IDENT_SETALL
	Vhosty dla grupy %s zosta³y zmienione na %s@%s.

HOST_SET_ERROR
	Vhost musi mieæ format prawid³owej maski hosta.

HOST_SET_IDENT_ERROR
	Ident vhosta musi mieæ format prawid³owego identa.

HOST_SET_TOOLONG
	B³±d! Vhost jest za d³ugi, u¿yj vhosta krótszego ni¿ %d znaków.

HOST_SET_IDENTTOOLONG
	B³±d! Ident jest za d³ugi, u¿yj identa krótszego ni¿ %d znaków.

HOST_NOREG
	Nie znaleziono %s w bazie serwisu NickServ.

HOST_SET_SYNTAX
	SET <nick> <maska-hosta>.

HOST_SETALL_SYNTAX
	SETALL <nick> <maska-hosta>.

HOST_DENIED
	Brak dostêpu.

HOST_NOT_ASSIGNED
	Skontaktuj siê z operatorem, aby uzyskaæ vhosta na ten nick.

HOST_ACTIVATED
	Twój vhost %s jest teraz aktywny.

HOST_IDENT_ACTIVATED
	Twój vhost %s@%s jest teraz aktywny.

HOST_ID
	Najpierw zidentyfikuj siê u serwisów.

HOST_NOT_REGED
	Aby uzyskaæ vhosta musisz byæ najpierw zarejestrowany.

HOST_DEL
	Vhost dla %s zosta³ usuniêty.

HOST_DEL_SYNTAX
	DEL <nick>.
HOST_OFF
	Twój vhost zosta³ usuniêty i zosta³o w³±czone normalne ukrywanie hosta.
HOST_OFF_UNREAL
	Twój vhost zosta³ usuniêty. Aby w³±czyæ standardowe ukrywanie hosta wpisz /mode %s +%s

HOST_NO_VIDENT
	Twoje IRCD nie obs³uguje vIdentów, je¶li nie jest to prawda zg³o¶ b³±d.

HOST_GROUP
	Wszystkie vhosty w grupie %s zosta³y zmienione na %s

HOST_IDENT_GROUP
	Wszystkie vhosty w grupie %s zosta³y zmienione na %s@%s

HOST_LIST_FOOTER
	Wy¶wietlono wszystkie wpisy (Liczba: %d)

HOST_LIST_RANGE_FOOTER
	Wy¶wietlono wpisy od %d do %d

HOST_LIST_KEY_FOOTER
	Wy¶wietlono pasuj±ce wpisy %s (Liczba: %d)

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_HELP_CMD_ON
	    ON          Aktywuje przypisanego vhosta

HOST_HELP_CMD_OFF
	    OFF         Deaktywuje przypisanego vhosta

HOST_HELP_CMD_GROUP
	    GROUP       Synchronizuje vhosty wszystkich nicków w grupie

HOST_HELP_CMD_SET
	    SET         Zmienia vhost wskazanemu u¿ytkownikowi

HOST_HELP_CMD_SETALL
	    SETALL      Zmienia vhosty wszystkich nicków w grupie

HOST_HELP_CMD_DEL
	    DEL         Usuwa vhost wskazanego u¿ytkownika

HOST_HELP_CMD_DELALL
	    DELALL      Usuwa vhosty wszystkich nicków w grupie

HOST_HELP_CMD_LIST
	    LIST        Listuje vhosty

HOST_OPER_HELP
	nie jest u¿ywane.

HOST_ADMIN_HELP
	nie jest u¿ywane.

HOST_HELP
	Komendy %S:

HOST_HELP_ON
	Sk³adnia: ON
	
	Aktywuje vhosta aktualnie przypisanego do u¿ywanego nicka.
	Po u¿yciu tego polecenia Twój prawdziwy adres zostanie 
	zast±piony vhostem.

HOST_HELP_SET
	Sk³adnia: SET <nick> <vhost>.
	
	Ustawia wskazanemu u¿ytkownikowi vhosta.
	Je¶li Twoje IRCD obs³uguje videnty to u¿ywaj±c
	polecenia SET <nick> <ident>@<vhost>
	mo¿na zmieniaæ tak¿e ident.

HOST_HELP_DELALL
	Sk³adnia: DELALL <nick>.
	
	Usuwa wszystkie vhosty grupy, do której nale¿y wskazany nick.

HOST_HELP_SETALL
	Sk³adnia: SETALL <nick> <vhost>.
	
	Ustawia vhosta dla wszystkich nicków w obrêbie grupy.
	Je¶li IRCD obs³uguje vIdenty wydanie polecenia
	SETALL <nick> <ident>@<vhost> spowoduje ustawienie
	tak¿e identów.
	
	UWAGA: to polecenie nie spowoduje zmiany vhosta
	dla nicka, który pó¼niej zostanie dodany do grupy.

HOST_HELP_OFF
	Sk³adnia: OFF
	
	Deaktywuje vhosta aktualnie przypisanego do u¿ywanego nicka.
	Po u¿yciu tego polecenia ka¿dy bêdzie móg³ sprawdziæ Twój
	prawdziwy adres u¿ywaj±c polecenia /WHOIS.

HOST_HELP_DEL
	Sk³adnia: DEL <nick>
	
	Usuwa vhosta przypisanego do wskazanego nicka.

HOST_HELP_LIST
	Sk³adnia: LIST [<wzorzec>|<#X-Y>]
	
	To polecenie listuje zarejestrowane vhosty operatorowi.
	Je¶li podano wzorzec tylko wpisy o pasuj±cych nickach
	lub vhostach zostan± pokazane. Podanie np. Rob*
	spowoduje wy¶wietlenie wszystkich wpisów zaczynaj±cych
	siê od Rob.
	
	Je¶li zostanie u¿yty format #X-Y to tylko wpisy nale¿±ce
	do przedzia³u od X do Y bêd± pokazane. Podanie #1-3
	spowoduje wy¶wietlenie pierwszych 3 wpisów wpisów.
	
	Maksymalna d³ugo¶æ listy wy¶wietlanej operatorowi podczas
	jednego zapytania jest okre¶lona przez ustawienie NSListMax.

HOST_HELP_GROUP
	Sk³adnia: GROUP
	
	To polecenie pozwala ustawiæ vhost bie¿±cego nicka
	dla wszystkich nicków w grupie.

OPER_SVSNICK_UNSUPPORTED
	Komenda SVSNICK nie jest dostêpna w tej sieci.

OPER_SQLINE_UNSUPPORTED
	Komenda SQLINE nie jest dostêpna w tej sieci.

OPER_SVSO_UNSUPPORTED
	Komenda OLINE nie jest dostêpna w tej sieci.

OPER_UMODE_UNSUPPORTED
	Komenda UMODE nie jest dostêpna w tej sieci.

OPER_SUPER_ADMIN_NOT_ENABLED
	Opcja SuperAdmin nie jest w³±czona w pliku konfiguracyjnym.

