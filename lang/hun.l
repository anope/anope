# Hungarian (magyar) language file.
#
# (C) 2003-2011 Anope Team
# Contact us at team@anope.org
#    
# Translated by:                                      
# ==============                                      
# Janos Kapitany 'Sarkanyka' (chates@freemail.hu)
# CJB CHAT Magyarország, http://cjbchat.hu     
# Ez a fordítás a CJB Chat megrendelésére készült. 
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

###########################################################
#
# A nyelvezet neve
#
###########################################################


# Angoltól (English) eltéro nyelvek esetén a stringnek a következo
# formátumban kell lennie:
#     nyelvezet-neve-a-nyelvben (nyelvezet-neve-angolul)
# Például, "Spanyol (Spanish)" vagy "Francia (French)".

LANG_NAME
	Magyar (Hungarian)

###########################################################
#
# Általános üzenetek
#
###########################################################
	
# strftime() beállitási sorai. 3 strfidõ információ formátumban adható
# meg a beállitásokban.  Rövid, hosszú és teljes verzió:
#	%a = Nap neve (rövid)		%H = óra
#	%b = Hónap neve (rövid)	%M = perc
#	%d = nap a hónapban		%S = másodperc
#	%Y = év			%Z = idõ zóne

# ezt a formát használd az strftime() mikor a dátum és idõ
# együtt szükséges.
STRFTIME_DATE_TIME_FORMAT
	%Z %Y %B %d %H:%M:%S
# ezt a formát használd az strftime() mikor a dátumnak hosszú
# formátum kell (Angol: a nap neve is).
STRFTIME_LONG_DATE_FORMAT
	%Y %B %d %A
# ezt a formát használd az strftime() mikor a dátumnak elég
# a rövig forma (Angol:: nap neve is).
STRFTIME_SHORT_DATE_FORMAT
	%b %d %Y

# Ez megadja az strftime() által használt hónap és nap neveket.  Ha nem
# használsz %a, %A, %b, vagy %B változót az strftime() stringben feljebb, akkor
# nyugodtan szabadon hagyhatod. Ha beírsz neveket, akkor azoknak sorban kell lenniük,
# és csak egy lehet soronként, a listának teljesnek KELL lennie!
	
# %a
STRFTIME_DAYS_SHORT
	Vas
	Hét
	Ke
	Sze
	Csü
	Pén
	Szo
# %A
STRFTIME_DAYS_LONG
	Vasárnap
	Hétfõ
	Kedd
	Szerda
	Csütörtök
	Péntek
	Szombat
# %b
STRFTIME_MONTHS_SHORT
	Jan
	Feb
	Márc
	Ápr
	Máj
	Jún
	Júl
	Aug
	Szept
	Okt
	Nov
	Dec
# %B
STRFTIME_MONTHS_LONG
	Január
	Február
	Március
	Április
	Május
	Június
	Július
	Augusztus
	Szeptember
	Október
	November
	December



# Ezt a ChanServ/NickServ INFO megjelenítéseknél használja.
COMMA_SPACE
	,


# Különbözõ hiba üzenetek.
USER_RECORD_NOT_FOUND
	Belsõ hiba - nem teljesíthetõ kérés.
UNKNOWN_COMMAND
	Ismeretlen Parancs %s.
UNKNOWN_COMMAND_HELP
	Ismeretlen parancs %s. Írd be: "%R%s HELP".
SYNTAX_ERROR
	Szintaktika: %s
MORE_INFO
	%R%s HELP %s bõvebb információért.
NO_HELP_AVAILABLE
	Nincs ezzel kapcsolatban segítség: %s.
OBSOLETE_COMMAND
	Nem használható parancs; használd a %s parancsot helyette.

BAD_USERHOST_MASK
	A maszknak ilyen formátumúnak kell lennie: user@host.
BAD_EXPIRY_TIME
	Érvénytelen elévülési idõ.
USERHOST_MASK_TOO_WIDE
	%s túl tág tartományra vonatkozik.

SERVICE_OFFLINE
	%s jelenleg offline.
READ_ONLY_MODE
	Megjegyzés: A Szervíz csak olvasás módban fut!
PASSWORD_INCORRECT
	Hibás jelszó.
INVALID_TARGET
	"/msg %s" is no longer supported.  Use "/msg %s@%s" or "/%s" instead.

# Mi a különbség a "Hozzáférés megtagadva" és az "Engedély letiltva" között?
# Nagyon kicsi; ha ugyanazt az üzenetet írod mindkettõre, nem igazán veszít
# a jelentésbõl. Ha különbséget akarsz tenni, akkor a "Hozzáférés megtagadva"
# általánosabb értelemben használatos, például, ha egy nem oper próbál 
# hozzáférni az OperServhez, akkor azt kapja, hogy "Hozzáférés megtagadva",
# amikor egy nem Szervíz admin próbálja használni a NickServ SET NOEXPIRE
# opciót, akkor õ azt kapja, hogy "Endegély letiltva".
ACCESS_DENIED
	Hozzáférés megtagadva.
PERMISSION_DENIED
	Engedély megtagadva.
RAW_DISABLED
	A RAW opcó nem használható. Ha  használnod kell, állitsd
	akkor be a DisableRaw beállitást  a Szerviz beállításnál.
MORE_OBSCURE_PASSWORD
	Próbálj nehezebb jelszót használni .Jelszavad  min. öt
	karakter legyen,	és nem kitalálható legyen, (ne a
	nickneved/valósneved),  ne legyen benne írásjel.
PASSWORD_TOO_LONG
	Your password is too long. Please try again with a shorter password.
NICK_NOT_REGISTERED
	A neved még nem regisztrált.
NICK_NOT_REGISTERED_HELP
	A neved még nem regisztrált.  Segítség: %R%s HELP.
NICK_X_IS_SERVICES
	A %s nick a Hálózat Szervízének egysége.
NICK_X_NOT_REGISTERED
	Ez a nick: %s nem regisztrált.
NICK_X_IN_USE
	Ez a nick: %s jelenleg használatban van.
NICK_X_NOT_IN_USE
	Ez a nick: %s nincs jelenleg használatban.
NICK_X_NOT_ON_CHAN
	%s jelenleg nincs a %s szobában .
NICK_X_FORBIDDEN
	 %snicket  nem lehet regisztrálni vagy használni.
NICK_X_FORBIDDEN_OPER
	Ezt a nicket: %s %s forbidolta:
	%s
NICK_X_ILLEGAL
	 %snick illegális nicknév vagy nem használható.
NICK_X_TRUNCATED
	A nicked %s meg lett csonkítva %d karakter hosszúságúra.
NICK_X_SUSPENDED
	Nick %s is currently suspended.
CHAN_X_NOT_REGISTERED
	 %s szoba nem regisztrált.
CHAN_X_NOT_IN_USE
	%scsatorna nem létezik.
CHAN_X_FORBIDDEN
	%scsatornát nem lehet regisztrálni vagy használni.
CHAN_X_FORBIDDEN_OPER
	%scsatornát %s forbidolta:
	%s
CHAN_X_SUSPENDED
	 Felfüggesztve: [%s] %s
NICK_IDENTIFY_REQUIRED
	Jelszavas azonositás szükséges ehhez a parancshoz.
	Próbáld újra miután beírtad: %R%s IDENTIFY jelszó.
CHAN_IDENTIFY_REQUIRED
	Jelszavas azonositás szükséges ehhez a parancshoz.
	Próbáld újra miután beírtad: %R%s IDENTIFY %s jelszó.
	
MAIL_DISABLED
	A Szervíz úgy van beállítva, hogy nem küld mailt.
MAIL_INVALID
	%sEmail címe nem érvényes.
MAIL_X_INVALID
	%s nem érvényes e-mail cím.
MAIL_LATER
	Nem lehet most levelet küldeni;  próbáld újra késõbb.
MAIL_DELAYED
	Kérlek várj %d másodpercet és próbáld újra.
	
NO_REASON
	Nincs indok
UNKNOWN
	<ismeretlen>

# Duration system
DURATION_DAY
	1 nap
DURATION_DAYS
	%d nap
DURATION_HOUR
	1 óra
DURATION_HOURS
	%d óra
DURATION_MINUTE
	1 perc
DURATION_MINUTES
	%d perc
DURATION_SECOND
	1 másodperc
DURATION_SECONDS
	%d másodperc

# Emberi olvasható elévülések
NO_EXPIRE
	nem fog elévülni
EXPIRES_SOON
	elévül a következõ adatbázis mentésnél
EXPIRES_M
	elévül %d perc múlva
EXPIRES_1M
	elévül %d perc múlva
EXPIRES_HM
	elévül %d óra, %d perc múlva
EXPIRES_H1M
	elévül %d óra, %d perc múlva
EXPIRES_1HM
	elévül %d óra, %d perc múlva
EXPIRES_1H1M
	elévül %d óra, %d perc múlva
EXPIRES_D
	elévül %d nap múlva
EXPIRES_1D
	elévül %d nap múlva
	
# Általános lábjegyzet üzenet
END_OF_ANY_LIST
	%s lista vége.

##########################################################
#
# NickServ üzenetek
#
##########################################################
	
# Automatikus üzenetek
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
	Ez a nick már regisztrált.  Kérlek válassz másikat.
	Ha ez a te nicked, akkor %R%s IDENTIFY jelszó! 
NICK_IS_SECURE
	Ez a nick regisztrált és védett.  Ha ez a te nicked,
	akkor %R%s IDENTIFY jelszó!
	Különben válassz másik nicknevet!
NICK_MAY_NOT_BE_USED
	Ez a nicknév nem használható. Kérlek válassz másikat!
FORCENICKCHANGE_IN_1_MINUTE
	Ha nem váltasz nevet 1 percen belül, én megváltoztatom.
FORCENICKCHANGE_IN_20_SECONDS
	Ha nem váltasz nevet 20 sec.-en belül, én megváltoztatom.
FORCENICKCHANGE_NOW
	Ez a nicknév regisztrált; te nem használhatod.
FORCENICKCHANGE_CHANGING
	A nickneved most meg lesz változtatva: %s.

# REGISTER válaszok
NICK_REGISTER_SYNTAX
	REGISTER jelszó [email]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER jelszó email
NICK_REGISTRATION_DISABLED
	Sajnálom, a nickregisztráció jelenleg nem mûködik.
NICK_REGISTRATION_FAILED
	Sajnálom, a regisztráció sikertelen.
NICK_REG_PLEASE_WAIT
	 Várj %d másodpercet mielõtt  használnád ezt a parancsot!
NICK_CANNOT_BE_REGISTERED
	Ez a nicknév: %s nem regisztrálható.
NICK_ALREADY_REGISTERED
	Ez a nicknév: %s már regisztrált!
NICK_REGISTERED
	Ez a nicknév: %s számodra lett regisztrálva: %s
NICK_REGISTERED_NO_MASK
	Nickname %s registered.
NICK_PASSWORD_IS
	Jelszavad: %s - jegyezd meg a késõbbi használathoz!
NICK_REG_DELAY
	A nicked regisztrálásához %d másodpercnél régebben kell csatlakozva lenned.
	
# GROUP válaszok
NICK_GROUP_SYNTAX
	GROUP célnick jelszó
NICK_GROUP_DISABLED
	Sajnálom, a csoportok használata ideiglenesen le van tiltva.
NICK_GROUP_FAILED
	Sajnálom, a group parancs használata sikertelen.
NICK_GROUP_PLEASE_WAIT
	Kérlek várj %d másodpercet, mielõtt ismét használod a GROUP parancsot.
NICK_GROUP_CHANGE_DISABLED
	A nicked már regisztrált; írd be %R%s DROP elõször.
NICK_GROUP_SAME
	Már tagja vagy ennek a csoportnak: %s.
NICK_GROUP_TOO_MANY
	Túl sok nick van a %s csoportban; listázd  és dropolj párat.
	Írd: %R%s HELP GLIST és %R%s HELP DROP
	bõvebb információkért.
NICK_GROUP_JOINED
	Mostantól tagja vagy a %scsoportnak.

# IDENTIFY válaszok
NICK_IDENTIFY_SYNTAX
	IDENTIFY jelszó
NICK_IDENTIFY_FAILED
	Sajnálom, azonosítás sikertelen.
NICK_IDENTIFY_SUCCEEDED
	Jelszavad elfogadva - azonosítás sikeres.
NICK_IDENTIFY_EMAIL_REQUIRED
	Be kell állítanod egy E-mail címet a nicknevedhez.
NICK_IDENTIFY_EMAIL_HOWTO
	Írd be: %R%S SET EMAIL e-mail 
	Az email címed nem lesz kiadva harmadik személynek.
NICK_ALREADY_IDENTIFIED
	Már be vagy azonosítva. :-)

# UPDATE válaszok
NICK_UPDATE_SUCCESS
	Státusz frissítve (memok, vhost, chmodok, flagek).
	
# LOGOUT válaszok
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	A nicked sikeresen kijelentkezett.
NICK_LOGOUT_X_SUCCEEDED
	 %s nick sikeresen kijelentkezett.
NICK_LOGOUT_SERVICESADMIN
	%s nem tud kijelentkezni, mert õ egy services admin.
	
# DROP válaszok
NICK_DROP_DISABLED
	Sajnálom,  a nicknevek regisztrációjának törlése  szünetel.
NICK_DROPPED
	A nickneved dropolva lett,
NICK_X_DROPPED
	A %s nicknevet dropoltad.
	
# SET válaszok
NICK_SET_SYNTAX
	SET opció paraméterek
NICK_SET_SERVADMIN_SYNTAX
	SET [nick] opció paraméterek
NICK_SET_DISABLED
	Sajnálom,  a nicknév opció beállítás jelenleg szünetel.
NICK_SET_UNKNOWN_OPTION
	Ismeretlen SET opció: %s.
NICK_SET_OPTION_DISABLED
	Ez az opció: %snem állítható be ezen a hálózaton.

# SET DISPLAY válaszok
NICK_SET_DISPLAY_INVALID
	Az új megjelenítéshez KELL az új becenév és a csoport!
NICK_SET_DISPLAY_CHANGED
	Az új megjelenítés: %s.
	
# SET PASSWORD válaszok
NICK_SET_PASSWORD_FAILED
	Sajnálom, a jelszavad nem sikerült megváltoztatni.
NICK_SET_PASSWORD_CHANGED
	Jelszavad megváltoztatva.
NICK_SET_PASSWORD_CHANGED_TO
	Jelszavad megváltoztatva a következõre: %s.
	
# SET LANGUAGE válaszok
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE szám
NICK_SET_LANGUAGE_UNKNOWN
	Ismeretlen nyelvi sorszám %d %R%s HELP SET LANGUAGE
NICK_SET_LANGUAGE_CHANGED
	Új nyelvezet: Magyar.

# SET URL válaszok
NICK_SET_URL_CHANGED
	URL megváltoztatva: %s.
NICK_SET_URL_UNSET
	URL törölve.

# SET EMAIL válaszok
NICK_SET_EMAIL_CHANGED
	E-mail cím megváltoztatva: %s.
NICK_SET_EMAIL_UNSET
	E-mail cím törölve.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	Nem törölheted ezen a hálózaton az e-mail címed.
	
# SET ICQ válaszok
NICK_SET_ICQ_CHANGED
	ICQ számod megváltoztatva: %s.
NICK_SET_ICQ_UNSET
	ICQ szám törölve.
NICK_SET_ICQ_INVALID
	%s nem érvényes szám.
	
# SET GREET válaszok
NICK_SET_GREET_CHANGED
	Köszöntõ üzenet megváltoztatva: %s.
NICK_SET_GREET_UNSET
	Köszöntõ üzeneted törölve.

# SET PROTECT válaszok
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	A Kill védelem opció most már ON.
NICK_SET_KILL_QUICK
	A Kill védelem opció most már ON, csökkentet idõvel.
NICK_SET_KILL_IMMED
	A Kill védelem opció most már ON, késleltetés nélkül.
NICK_SET_KILL_IMMED_DISABLED
	Az  IMMED opció nem elérhetõ ezen a hálózaton.
NICK_SET_KILL_OFF
	A Kill védelem opció most már OFF.

# SET SECURE válaszok
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	A Secure opció most már ON.
NICK_SET_SECURE_OFF
	A Secure opció most már OFF.

# SET PRIVATE válaszok
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	A Private opció most már ON.
NICK_SET_PRIVATE_OFF
	A Private opció most már OFF.

# SET HIDE válaszok
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	 E-mail címed most rejtve lesz a %s INFO kiírásnál.
NICK_SET_HIDE_EMAIL_OFF
	E-mail címed most látható lesz a %s INFO kiírásnál.
NICK_SET_HIDE_MASK_ON
	Utolsó user@host maszkod rejtve lesz a %s INFO kiírásnál.
NICK_SET_HIDE_MASK_OFF
	Utolsó user@host maszkod látható lesz a %s INFO kiírásnál.
NICK_SET_HIDE_QUIT_ON
	Utolsó kilépõ üzeneted rejtve lesz a %s INFO kiírásnál
NICK_SET_HIDE_QUIT_OFF
	Utolsó kilépõ üzeneted látható lesz a %s INFO kiírásnál.
NICK_SET_HIDE_STATUS_ON
	A szervíz hozzáférési státuszod rejtve lesz a %s INFO kiírásnál.
NICK_SET_HIDE_STATUS_OFF
	A szervíz hozzáférési státuszod látható lesz a %s INFO kiírásnál.

# SET MSG válaszok
NICK_SET_MSG_SYNTAX
	SET MSG {ON | OFF}
NICK_SET_MSG_ON
	A szervíz mostantól  privát üzenetekbenválaszol.
NICK_SET_MSG_OFF
	A szervíz újra notice-t küld a válaszoknál.

# SET AUTOOP responses
NICK_SET_AUTOOP_SYNTAX
	SET AUTOOP {ON | OFF}
NICK_SET_AUTOOP_ON
	Services will now autoop you in channels.
NICK_SET_AUTOOP_OFF
	Services will no longer autoop you in channels.

# SET NOEXPIRE válaszok
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
	ACCESS {ADD | DEL | LIST} [maszk]
NICK_ACCESS_ALREADY_PRESENT
	Ez a maszk: %s már szerepel a hozzáférési listádon.
NICK_ACCESS_REACHED_LIMIT
	Neked csak %d bejegyzésed lehet a hozzáférési listán.
NICK_ACCESS_ADDED
	%s hozzáadva a hozzáférési listádhoz.
NICK_ACCESS_NOT_FOUND
	%s nem található a hozzáférési listádon.
NICK_ACCESS_DELETED
	%s törölve a hozzáférési listádról.
NICK_ACCESS_LIST
	Hozzáférési lista:
NICK_ACCESS_LIST_X
	%s hozzáférési listája:
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

# INFO Válaszok
NICK_INFO_SYNTAX
	INFO nick [ALL]
NICK_INFO_REALNAME
	%s az %s
NICK_INFO_SERVICES_OPER
	%s egy szervíz operátor.
NICK_INFO_SERVICES_ADMIN
	%s egy szervíz adminisztrátor.
NICK_INFO_SERVICES_ROOT
	%s egy szervíz root adminisztrátor.
NICK_INFO_ADDRESS
	Utoljára látott cím: %s
NICK_INFO_ADDRESS_ONLINE
	Jelenleg online errõl a címrõl: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s jelenleg online.
NICK_INFO_TIME_REGGED
	Regisztrálás ideje: %s
NICK_INFO_LAST_SEEN
	Utoljára ekkor volt: %s
NICK_INFO_LAST_QUIT
	Utolsó kilépõ üzenet: %s
NICK_INFO_URL
	URL: %s
NICK_INFO_EMAIL
	E-mail cím: %s
NICK_INFO_VHOST
	vhost: %s
NICK_INFO_VHOST2
	vhost: %s@%s
NICK_INFO_ICQ
	ICQ #: %d
NICK_INFO_GREET
	Köszöntõ üzenet: %s
NICK_INFO_OPTIONS
	Opciók: %s
NICK_INFO_EXPIRE
	Expires in: %s
# Ezek a stringek NEM LEHETNEK üresek
NICK_INFO_OPT_KILL
	Kill védelem
NICK_INFO_OPT_SECURE
	Biztonság
NICK_INFO_OPT_PRIVATE
	Privát
NICK_INFO_OPT_MSG
	Üzenet mód
NICK_INFO_OPT_AUTOOP
	Auto-op
NICK_INFO_OPT_NONE
	Nincs
NICK_INFO_NO_EXPIRE
	Ez a nicknév nem fog elévülni.
NICK_INFO_FOR_MORE
	Bõvebb információkért, írd be: %R%s INFO %s ALL.

# LIST válaszok
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
	Azon bejegyzések listája, melyek egyeznek ezzel, %s:
NICK_LIST_RESULTS
	Lista vége - %d / %d találat megjelenítve.
	
# ALIST válaszok
NICK_ALIST_SYNTAX
	ALIST nicknév
NICK_ALIST_HEADER
	Azon csatornák, ahol hozzáférésed van:
	  Sorszám  Csatorna           Szint    Meghatározás 
NICK_ALIST_HEADER_X
	Azon csatornák, ahol %s nicknek hozzáférése van:
	  Sorszám  Csatorna              Szint    Meghatározás
NICK_ALIST_XOP_FORMAT
	  %3d %c %-20s %-8s  %s
NICK_ALIST_ACCESS_FORMAT
	  %3d %c %-20s %-8d %s
NICK_ALIST_FOOTER
	Lista vége - %d / %d csatorna megjelenítve.

# GLIST válaszok
NICK_GLIST_HEADER
	A csoportodba tartozó nickek listája:
NICK_GLIST_HEADER_X
	%s csoportjába tartozó nickek listája:
NICK_GLIST_FOOTER
	%d nicknév van a csoportban.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (expires in %s)

# RECOVER válaszok
NICK_RECOVER_SYNTAX
	RECOVER nicknév [jelszó]
NICK_NO_RECOVER_SELF
	Nem használhatod az aktiv nickedre!
NICK_RECOVERED
	User claiming your nick has been killed.
	%R%s RELEASE %s to get it back before %s timeout.

# RELEASE válaszok
NICK_RELEASE_SYNTAX
	RELEASE nicknév [jelszó]
NICK_RELEASE_NOT_HELD
	Ezt a nicket %s nem fogja a services.
NICK_RELEASED
	A Services rendelkezésedre bocsájtotta a nicket.

# GHOST válaszok
NICK_GHOST_SYNTAX
	GHOST nicknév [jelszó]
NICK_NO_GHOST_SELF
	Nem ghostolhatod ki magad!
NICK_GHOST_KILLED
	A befagyott kliens killelve.

# GETPASS válaszok
NICK_GETPASS_SYNTAX
	GETPASS nicknév
NICK_GETPASS_UNAVAILABLE
	GETPASS  nem elérhetõ, amíg az encryption használatban van
NICK_GETPASS_PASSWORD_IS
	%s jelszava: %s.

# GETEMAIL válaszok
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host Nincs * karakter!!
NICK_GETEMAIL_EMAILS_ARE
	Egyezõ emailek %s ehhez: %s.
NICK_GETEMAIL_NOT_USED
	Nincs Email listázva ehhez: %s.

# SENDPASS válaszok
NICK_SENDPASS_SYNTAX
	SENDPASS nicknév
NICK_SENDPASS_UNAVAILABLE
	SENDPASS nem elérhetõ, amíg az encryption használatban van.
NICK_SENDPASS_SUBJECT
	Nicknév jelszó (%s)
NICK_SENDPASS_HEAD
	Hi,
NICK_SENDPASS_LINE_1
	Azt kérted, hogy a %s nicknév jelszavát küldjük el e-mailben.
NICK_SENDPASS_LINE_2
	A jelszavad %s Ezt a biztosnág miatt meg kell váltóztatni,	miután megkaptad ezt a levelet.
NICK_SENDPASS_LINE_3
	Ha nem tudod miért kaptad ezt a levelet, akkor kérlek hagyd figyelmen kivül!
NICK_SENDPASS_LINE_4
	ERRE A LEVÉLRE NE VÁLASZOLJ!!!
NICK_SENDPASS_LINE_5
	%s adminisztrátorok.
NICK_SENDPASS_OK
	%s nick jelszava elküldve.

# FORBID válaszok
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
	FORBID nicknév [indok]
NICK_FORBID_SYNTAX_REASON
	FORBID nicknév indok
NICK_FORBID_SUCCEEDED
	%s nick most már forbidolt.
NICK_FORBID_FAILED
	Nem forbidolható: %s!

# Nick Registraion válaszok
NICK_REQUESTED
	Ezt a nicket már kérelmezték, ellenõrizd az e-mail cimed, egy levélben van a kódod.
NICK_REG_RESENT
	A kód újra el lett küldve erre a címre: %s.
NICK_REG_UNABLE
	Nick NEM regisztrált, próbáld késõbb.
NICK_IS_PREREG
	A nick az emailben elküldött kóddal való aktíválásra vár a regisztráció befejezéséhez.
NICK_ENTER_REG_CODE
	A kód a %s címre lett küldve, írd be %R%s confirm <kód> a regisztráció befejezéséhez.
NICK_CONFIRM_NOT_FOUND
	Regisztráció 1 lépése lehet, hogy elévült, írd be: "%R%s register <jelszó> <email>".
NICK_CONFIRM_INVALID
	Érvénytelen kód, ellenõrizze az e-mailben, majd próbálja újra.
NICK_REG_MAIL_SUBJECT
	Nicknév Regisztráció (%s)
NICK_REG_MAIL_HEAD
	Hi,
NICK_REG_MAIL_LINE_1
	Te kérelmeztad a %s nick regisztrálását.
NICK_REG_MAIL_LINE_2
	Írd be: " %R%s confirm %s ", hogy befejezd a regisztrációt.
NICK_REG_MAIL_LINE_3
	Ha nem tudod miért kaptad a levelet, kérlek hagyd figyelmen kívül!
NICK_REG_MAIL_LINE_4
	NE VÁLASZOLJ A LEVÉLRE!
NICK_REG_MAIL_LINE_5
	%s adminisztrátorok.
NICK_GETPASS_PASSCODE_IS
	Kód a %s nickhez:  %s.
NICK_FORCE_REG
	A %s nicknév érvényesítve. 

###########################################################
#
# ChanServ üzenetek
#
###########################################################
	
# Access level leírások
CHAN_LEVEL_AUTOOP
	Automatikus operátor (+o) státusz
CHAN_LEVEL_AUTOVOICE
	Automatikus voice (+v) státusz (hangadás)
CHAN_LEVEL_AUTOHALFOP
	Automatikus halfop (+h) státusz (félop)
CHAN_LEVEL_AUTOPROTECT
	Automatikus protect (+a) mód. 
CHAN_LEVEL_AUTODEOP
	Csatorna operátori státusz tiltása.
CHAN_LEVEL_NOJOIN
	Nem léphet a csatornára, ha a RESTRICTED használatban.
CHAN_LEVEL_INVITE
	Használhatja az INVITE parancsot.
CHAN_LEVEL_AKICK
	Használhatja az AKICK parancsot.
CHAN_LEVEL_SET
	Használhatja a SET parancsot (kivéve FOUNDER/PASSWORD).
CHAN_LEVEL_CLEAR
	Használhatja a CLEAR parancsot.
CHAN_LEVEL_UNBAN
	Használhatja az UNBAN parancsot.
CHAN_LEVEL_OPDEOP
	Használhatja az OP/DEOP parancsot.
CHAN_LEVEL_ACCESS_LIST
	Megtekintheti a hozzáférési listát.
CHAN_LEVEL_ACCESS_CHANGE
	Módosíthatja a hozzáférési listát.
CHAN_LEVEL_MEMO
	Írhat/olvashat csatorna memo üzeneteket
CHAN_LEVEL_ASSIGN
	Társíthat/eltávolíthat services botot
CHAN_LEVEL_BADWORDS
	Használhatja a BADWORDS parancsot
CHAN_LEVEL_NOKICK
	Nem lesz kirúgva a bot automatikus funkcióival.
CHAN_LEVEL_FANTASIA
	Használhatja a fantasia parancsokat.
CHAN_LEVEL_SAY
	Használhatja az ACT és SAY parancsokat.
CHAN_LEVEL_GREET
	NickServ-es köszöntõ üzenet megjelenik.
CHAN_LEVEL_VOICEME
	Használhatja a (de)voiceme parancsokat
CHAN_LEVEL_VOICE
	Használhatja a VOICE/DEVOICE parancsokat.
CHAN_LEVEL_GETKEY
	Használhatja a GETKEY parancsot.
CHAN_LEVEL_OPDEOPME
	Használhatja a (de)opme parancsot.
CHAN_LEVEL_HALFOPME
	Használhatja a (de)halfopme parancsot.
CHAN_LEVEL_HALFOP
	Használhatja a HALFOP/DEHALFOP parancsokat
CHAN_LEVEL_PROTECTME
	Használhatja a (de)protectme parancsot
CHAN_LEVEL_PROTECT
	Használhatja a PROTECT/DEPROTECT parancsokat
CHAN_LEVEL_KICKME
	Használhatja a kickme parancsot
CHAN_LEVEL_KICK
	Használhatja a KICK parancsot
CHAN_LEVEL_SIGNKICK
	Nincs signkick, ha a SIGNKICK LEVEL használatban van.
CHAN_LEVEL_BANME
	Használhatja a banme parancsot
CHAN_LEVEL_BAN
	Használhatja a BAN parancsot
CHAN_LEVEL_TOPIC
	Használhatja a TOPIC parancsot
CHAN_LEVEL_INFO
	Használhatja az INFO parancsot az ALL opcióval

# Automatikus válaszok
CHAN_IS_REGISTERED
	Ez a szoba a %s adatbázisában regisztrált.
CHAN_NOT_ALLOWED_OP
	Nem lehetsz szobaop ezen a csatornán: %s.
CHAN_MAY_NOT_BE_USED
	Ez a csatorna nem használható.
CHAN_NOT_ALLOWED_TO_JOIN
	Nem tartózkodhatsz ezen a csatornán.

# REGISTER válaszok
CHAN_X_INVALID
	Channel %s is not a valid channel.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER #szoba jelszó leírás
CHAN_REGISTER_DISABLED
	Sajnálom,  a csatorna regisztráció jelenleg szünetel.
CHAN_REGISTER_NOT_LOCAL
	Helyi csatornák nem regisztrálhatók.
CHAN_MUST_REGISTER_NICK
	Elõször regisztrálnod kell a nevedet. %R%s help
CHAN_MUST_IDENTIFY_NICK
	Kérlek azonosíts a %s-be, használd ezt a parancsot:
	%R%s IDENTIFY jelszó
CHAN_MAY_NOT_BE_REGISTERED
	Ez a csatorna %s nem regisztrálható. 
CHAN_ALREADY_REGISTERED
	A megadott csatorna %s már regisztrált!
CHAN_MUST_BE_CHANOP
	A csatornán operátornak kell lenned, hogy regisztrálhasd.
CHAN_REACHED_CHANNEL_LIMIT
	Már elérted a felsõ korlátot (%d) csatornák számában.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Már túllépted a felsõ korlátot (%d) a csatornán.
CHAN_REGISTRATION_FAILED
	Sajnálom,  regisztráció sikertelen.
CHAN_REGISTERED
	Regisztráltam a %s csatornát a (%s) nickre.
CHAN_PASSWORD_IS
	Csatorna jelszava: %s - Jegyezd fel a késõbbi használathoz.
CHAN_REGISTER_NONE_CHANNEL
	You have attempted to register a nonexistent channel %s
CHAN_SYMBOL_REQUIRED
	Please use the symbol of # when attempting to register


# IDENTIFY válaszok
CHAN_IDENTIFY_SYNTAX
	IDENTIFY   #szoba   jelszó
CHAN_IDENTIFY_FAILED
	Sajnálom,  azonosítás sikertelen.
CHAN_IDENTIFY_SUCCEEDED
	Jelszavad elfogadva - founder szintû a hozzáférésed a %s csatornán.

# LOGOUT válaszok
CHAN_LOGOUT_SYNTAX
	LOGOUT #szoba nicknév
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT #szoba  [nicknév]
CHAN_LOGOUT_SUCCEEDED
	%s felhasználó kijelentkezett a %s csatornáról.
CHAN_LOGOUT_ALL_SUCCEEDED
	Minden felhasználó kijelentkezett a %s csatornáról (except the channel founder).
CHAN_LOGOUT_FOUNDER_FAILED
	You may not log out of %s (you are founder).

# DROP válaszok
CHAN_DROP_SYNTAX
	DROP #szoba
CHAN_DROP_DISABLED
	A csatornák regisztrációjának törlése jelenleg szünetel.
CHAN_DROPPED
	%s csatorna regisztrációjának törlése sikerült.

# SET válaszok
CHAN_SET_SYNTAX
	SET #szoba opció paraméterek
CHAN_SET_DISABLED
	A csatornák opcióinak beállitása jelenleg szünetel.
CHAN_SET_UNKNOWN_OPTION
	Ismeretlen SET opció %s.

# SET FOUNDER válaszok
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s részére túl sok csatorna van regisztrálva.
CHAN_FOUNDER_CHANGED
	A megadott csatorna (%s) új foundere: %s.

# SET SUCCESSOR válaszok
CHAN_SUCCESSOR_CHANGED
	A megadott csatorna (%s) új successora: %s.
CHAN_SUCCESSOR_UNSET
	A megadott csatorna %s successora törölve.
CHAN_SUCCESSOR_IS_FOUNDER
	%s nem lehet a %s csatorna successora, mert õ a founder is.

# SET PASSWORD válaszok
CHAN_SET_PASSWORD_FAILED
	A jelszó beállítása sikertelen.
CHAN_PASSWORD_CHANGED
	%s csatorna jelszava megváltozott.
CHAN_PASSWORD_CHANGED_TO
	%s csatorna új jelszava: %s.

# SET DESC válaszok
CHAN_DESC_CHANGED
	%s csatorna leírás megváltoztatva: %s.

# SET URL válaszok
CHAN_URL_CHANGED
	A megadott csatorna (%s) új URL címe: %s.
CHAN_URL_UNSET
	A megadott csatorna %s URL címe törölve.

# SET EMAIL válaszok
CHAN_EMAIL_CHANGED
	A megadott csatorna (%s) új E-mail címe: %s.
CHAN_EMAIL_UNSET
	A megadott csatorna (%s) E-mail címe törölve.

# SET ENTRYMSG válaszok
CHAN_ENTRY_MSG_CHANGED
	%s csatorna belépõ üzenete megváltozott.
CHAN_ENTRY_MSG_UNSET
	%s csatorna belépõ üzenete törölve.

# SET BANTYPE válaszok
CHAN_SET_BANTYPE_INVALID
	%s nem érvényes ban típus.
CHAN_SET_BANTYPE_CHANGED
	A megadott csatorna (%s) új ban típusa: #%d.

# SET MLOCK válaszok
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Ismeretlen mód karakter: %c figyelmen kívül hagyva.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	%c mód figyelmen kívül hagyva, mert nem rögzítheted.
CHAN_SET_MLOCK_L_REQUIRED
	Rögzítened kell a +l módot is, ha a +L módot rögzited.
CHAN_SET_MLOCK_K_REQUIRED
	Rögzítened kell a +i módot is, ha a +K módot rögzíted.
CHAN_MLOCK_CHANGED
	A megadott csatornán (%s) az új Mode lock: %s.

# SET KEEPTOPIC válaszok
CHAN_SET_KEEPTOPIC_SYNTAX
	SET #szoba KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Topic retention option for %s is now ON.
CHAN_SET_KEEPTOPIC_OFF
	Topic retention option for %s is now OFF.

# SET TOPICLOCK válaszok
CHAN_SET_TOPICLOCK_SYNTAX
	SET #szoba TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Topic lock option for %s is now ON.
CHAN_SET_TOPICLOCK_OFF
	Topic lock option for %s is now OFF.

# SET PEACE válaszok
CHAN_SET_PEACE_SYNTAX
	SET #szoba PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Peace option for %s is now ON.
CHAN_SET_PEACE_OFF
	Peace option for %s is now OFF.

# SET PRIVATE válaszok
CHAN_SET_PRIVATE_SYNTAX
	SET #szoba PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Private option for %s is now ON.
CHAN_SET_PRIVATE_OFF
	Private option for %s is now OFF.

# SET SECUREOPS válaszok
CHAN_SET_SECUREOPS_SYNTAX
	SET #szoba SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Secure ops option for %s is now ON.
CHAN_SET_SECUREOPS_OFF
	Secure ops option for %s is now OFF.

# SET SECUREFOUNDER válaszok
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET #szoba SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Secure founder option for %s is now ON.
CHAN_SET_SECUREFOUNDER_OFF
	Secure founder option for %s is now OFF.

# SET RESTRICTED válaszok
CHAN_SET_RESTRICTED_SYNTAX
	SET #szoba RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Restricted access option for %s is now ON.
CHAN_SET_RESTRICTED_OFF
	Restricted access option for %s is now OFF.

# SET SECURE válaszok
CHAN_SET_SECURE_SYNTAX
	SET #szoba SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Secure option for %s is now ON.
CHAN_SET_SECURE_OFF
	Secure option for %s is now OFF.

# SET SIGNKICK válaszok
CHAN_SET_SIGNKICK_SYNTAX
	SET #szoba SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Signed kick option for %s is now ON.
CHAN_SET_SIGNKICK_LEVEL
	Signed kick option for %s is now ON, but depends of the
	level of the user that is using the command.
CHAN_SET_SIGNKICK_OFF
	Signed kick option for %s is now OFF.

# SET OPNOTICE válaszok
CHAN_SET_OPNOTICE_SYNTAX
	SET #szoba OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice option for %s is now ON.
CHAN_SET_OPNOTICE_OFF
	Op-notice option for %s is now OFF.

# SET XOP válaszok
CHAN_SET_XOP_SYNTAX
	SET #szoba XOP {ON | OFF}
CHAN_SET_XOP_ON
	xOP lists system for %s is now ON.
CHAN_SET_XOP_OFF
	xOP lists system for %s is now OFF.

# SET NOEXPIRE válaszok
CHAN_SET_NOEXPIRE_SYNTAX
	SET #szoba NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	A megadott csatorna (%s) nem fog elévülni.
CHAN_SET_NOEXPIRE_OFF
	A megadott csatorna (%s) el fog évülni.

# xOP üzenetek
CHAN_XOP_REACHED_LIMIT
	Csak %d számú helyed van a csatorna VOP/AOP/SOP listán.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	Nem használhatod a parancsot. Használd az ACCESS parancsot.
	Bõvebb információ: %R%s HELP ACCESS
CHAN_XOP_NOT_AVAILABLE
	xOP system is not available.

# AOP üzenetek
CHAN_AOP_SYNTAX
	AOP #szoba { ADD | DEL | LIST | CLEAR } [ nick | sorszám ]
CHAN_AOP_DISABLED
	Csatorna AOP lista módosítás szünetel.
CHAN_AOP_NICKS_ONLY
	Csatorna AOP listája csak regisztrált nevet tartalmazhat.
CHAN_AOP_ADDED
	%s hozzáadva %s AOP listához.
CHAN_AOP_MOVED
	%s  áthelyezve a %s AOP listára.
CHAN_AOP_NO_SUCH_ENTRY
	Nem található (#%d) a %s AOP listán.
CHAN_AOP_NOT_FOUND
	%s nem találatható a %s AOP listán.
CHAN_AOP_NO_MATCH
	Nincs ilyen bejegyzés a %s AOP listán.
CHAN_AOP_DELETED
	%s törölve lett a %s AOP listáról.
CHAN_AOP_DELETED_ONE
	1 bejegyzés törölve a %s AOP listáról.
CHAN_AOP_DELETED_SEVERAL
	%d bejegyzés törölve a %s AOP listáról.
CHAN_AOP_LIST_EMPTY
	%s AOP listája üres.
CHAN_AOP_LIST_HEADER
	%s AOP listája:
	  Sorszám  Nicknév

# HOP üzenetek
CHAN_AOP_CLEAR
	Channel %s AOP list has been cleared.

# HOP messages
CHAN_HOP_SYNTAX
	HOP #szoba { ADD | DEL | LIST | CLEAR } [ nick | sorszám ]
CHAN_HOP_DISABLED
	Csatorna HOP lista módosítás szünetel.
CHAN_HOP_NICKS_ONLY
	Csatorna HOP listája csak regisztrált nevet tartalmazhat.
CHAN_HOP_ADDED
	%s hozzáadva %s HOP listához.
CHAN_HOP_MOVED
	%s  áthelyezve a %s HOP listára.
CHAN_HOP_NO_SUCH_ENTRY
	Nem található (#%d) a %s HOP listán.
CHAN_HOP_NOT_FOUND
	%s nem találatható a %s HOP listán.
CHAN_HOP_NO_MATCH
	Nincs ilyen bejegyzés a %s HOP listán.
CHAN_HOP_DELETED
	%s törölve lett a %s HOP listáról.
CHAN_HOP_DELETED_ONE
	1 bejegyzés törölve a %s HOP listáról.
CHAN_HOP_DELETED_SEVERAL
	%d bejegyzés törölve a %s HOP listáról.
CHAN_HOP_LIST_EMPTY
	%s HOP listája üres.
CHAN_HOP_LIST_HEADER
	%s HOP listája:
	  Sorszám  Nicknév

# SOP üzenetek
CHAN_HOP_CLEAR
	Channel %s HOP list has been cleared.

# SOP messages
CHAN_SOP_SYNTAX
	SOP #szoba { ADD | DEL | LIST | CLEAR }[ nick | sorszám]
CHAN_SOP_DISABLED
	Csatorna SOP lista módosítás szünetel.
CHAN_SOP_NICKS_ONLY
	Csatorna SOP listája csak regisztrált nevet tartalmazhat.
CHAN_SOP_ADDED
	%s hozzáadva %s SOP listához.
CHAN_SOP_MOVED
	%s  áthelyezve a %s SOP listára.
CHAN_SOP_NO_SUCH_ENTRY
	Nem található (#%d) a %s SOP listán.
CHAN_SOP_NOT_FOUND
	%s nem találatható a %s SOP listán.
CHAN_SOP_NO_MATCH
	Nincs ilyen bejegyzés a %s SOP listán.
CHAN_SOP_DELETED
	%s törölve lett a %s SOP listáról.
CHAN_SOP_DELETED_ONE
	1 bejegyzés törölve a %s SOP listáról.
CHAN_SOP_DELETED_SEVERAL
	%d bejegyzés törölve a %s SOP listáról.
CHAN_SOP_LIST_EMPTY
	%s SOP listája üres.
CHAN_SOP_LIST_HEADER
	%s SOP listája:
	  Sorszám  Nicknév

# VOP messages
CHAN_SOP_CLEAR
	Channel %s SOP list has been cleared.

# VOP messages
CHAN_VOP_SYNTAX
	VOP #csatorna { ADD | DEL | LIST | CLEAR } [ nick | sorszám ]
CHAN_VOP_DISABLED
	Csatorna VOP lista módosítás szünetel.
CHAN_VOP_NICKS_ONLY
	Csatorna VOP listája csak regisztrált nevet tartalmazhat.
CHAN_VOP_ADDED
	%s hozzáadva %s VOP listához.
CHAN_VOP_MOVED
	%s  áthelyezve a %s VOP listára.
CHAN_VOP_NO_SUCH_ENTRY
	Nem található (#%d) a %s VOP listán.
CHAN_VOP_NOT_FOUND
	%s nem találatható a %s VOP listán.
CHAN_VOP_NO_MATCH
	Nincs ilyen bejegyzés a %s VOP listán.
CHAN_VOP_DELETED
	%s törölve lett a %s VOP listáról.
CHAN_VOP_DELETED_ONE
	1 bejegyzés törölve a %s VOP listáról.
CHAN_VOP_DELETED_SEVERAL
	%d bejegyzés törölve a %s VOP listáról.
CHAN_VOP_LIST_EMPTY
	%s VOP listája üres.
CHAN_VOP_LIST_HEADER
	%s VOP listája:
	  Sorszám  Nicknév

# ACCESS üzenetek
CHAN_VOP_CLEAR
	Channel %s VOP list has been cleared.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS#szoba{ ADD | DEL | LIST | CLEAR } [ nick [ szint ] | sorszám ]
CHAN_ACCESS_XOP
	Nem használhatod ezt a parancsot. 
	Használd az AOP, SOP és VOP parancsokat helyette.
	Írd be: %R%s HELP parancs bõvebb információért.
CHAN_ACCESS_XOP_HOP
	You can't use this command. 
	Use the AOP, SOP, HOP and VOP commands instead.
	Type %R%s HELP command for more information.
CHAN_ACCESS_DISABLED
	A csatorna hozzáférési listájának módosítása szünetel.
CHAN_ACCESS_LEVEL_NONZERO
	A hozzáférési szint nem lehet nulla.
CHAN_ACCESS_LEVEL_RANGE
	A hozzáférési szintnek %d és %d között kell lennie.
CHAN_ACCESS_NICKS_ONLY
	Csatorna listája csak regisztrált nickeket tartalmazhat.
CHAN_ACCESS_REACHED_LIMIT
	Neked csak %d  helyed van a csatorna listáján.
CHAN_ACCESS_LEVEL_UNCHANGED
	%s hozzáférési szintje a (%s) csatornán : %d
CHAN_ACCESS_LEVEL_CHANGED
	%s hozzáférése a (%s) csatornán változott: %d.
CHAN_ACCESS_ADDED
	%s hozzáadva a %s hozzáférési listájához.
	A hozzáférési szintje a ChanServ parancsokhoz:  %d.
CHAN_ACCESS_NO_SUCH_ENTRY
	A bejegyzés (#%d) nem található %s hozzáférési listáján.
CHAN_ACCESS_NOT_FOUND
	%s nem található a (%s) csatorna hozzáférési listáján.
CHAN_ACCESS_NO_MATCH
	Nincs ilyen bejegyzés a (%s) csatorna hozzáférési listáján.
CHAN_ACCESS_DELETED
	%s törölve lett a (%s) csatorna hozzáférési listájáról.
CHAN_ACCESS_DELETED_ONE
	1 bejegyzés törölve a (%s) csatorna hozzáférési listájáról
CHAN_ACCESS_DELETED_SEVERAL
	%d bejegyzés törölve (%s) csatorna hozzáférési listájáról.
CHAN_ACCESS_LIST_EMPTY
	%s hozzáférési listája üres.
CHAN_ACCESS_LIST_HEADER
	%s hozzáférési listája:
	  Sorszám   Szint  Nick
CHAN_ACCESS_LIST_FOOTER
	Hozzáférési lista vége.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
	
# AKICK válaszok
CHAN_ACCESS_CLEAR
	Channel %s access list has been cleared.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK #szoba {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR}[ nick-maszk ] [ indok ]
CHAN_AKICK_DISABLED
	Csatorna autokick listájának módosítása szünetel.
CHAN_AKICK_ALREADY_EXISTS
	%s már szerepel a (%s) csatorna autokick listáján.
CHAN_AKICK_REACHED_LIMIT
	%d bejegyzésnél nem lehet több a listán.
CHAN_AKICK_ADDED
	%s hozzáadva a (%s) csatorna autokick listájához
CHAN_AKICK_NO_SUCH_ENTRY
	A bejegyzés (#%d) nincs a (%s) csatorna autokick listáján.
CHAN_AKICK_NOT_FOUND
	%s nem szerepel a (%s) csatorna autokick listáján.
CHAN_AKICK_NO_MATCH
	Nincs ilyen bejegyzés a (%s) csatorna autokick listáján.
CHAN_AKICK_STUCK
	%s most már mindig aktív a (%s) csatornán.
CHAN_AKICK_UNSTUCK
	%s többé már nem aktív a (%s) csatornán.
CHAN_AKICK_DELETED
	%s törölve a (%s) csatorna autokick listájáról.
CHAN_AKICK_DELETED_ONE
	1 bejegyzés törölve a (%s) csatorna autokick listájáról.
CHAN_AKICK_DELETED_SEVERAL
	%d bejegyzés törölve a (%s) csatorna autokick listájáról.
CHAN_AKICK_LIST_EMPTY
	%s autokick listája üres.
CHAN_AKICK_LIST_HEADER
	%s autokick listája:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (%s által, ekkor: %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (stuck) (%s által, ekkor: %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	Az AKICK ENFORCE a(z) %s csatornán teljesítve;
	%d user eltávolítva.

# LEVELS válaszok
CHAN_AKICK_CLEAR
	Channel %s akick list has been cleared.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS #szoba{SET | DIS[ABLE] | LIST | RESET }[ opció [szint]]
CHAN_LEVELS_XOP
	Ez a parancs használhatatlan ebben a módban. (xOP)
CHAN_LEVELS_RANGE
	A szintnek kizárólag %d és %d között kell lennie.
CHAN_LEVELS_CHANGED
	%s szintjének új értéke a (%s) csatornán: %d.
CHAN_LEVELS_UNKNOWN
	%s beállítás nem ismert. Írd be %R%s HELP LEVELS DESC
	az érvényes beállítások listájának megtekintéséhez.  
CHAN_LEVELS_DISABLED
	%s letiltva a (%s) csatornán.
CHAN_LEVELS_LIST_HEADER
	%s hozzáférési szintjeinek beállításai:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (letiltva)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (csak foundernek)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	%s szintjei visszaállítva az alapértékekre.

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

# INFO válaszok
CHAN_INFO_SYNTAX
	INFO #szoba [ALL]
CHAN_INFO_HEADER
	%s információi:
CHAN_INFO_FOUNDER
	    Tulajdonos: %s (%s)
CHAN_INFO_NO_FOUNDER
	    Tulajdonos: %s
CHAN_INFO_SUCCESSOR
	    Jogutód: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	    Jogutód: %s
CHAN_INFO_DESCRIPTION
	    Leírás: %s
CHAN_INFO_ENTRYMSG
	    Köszöntõ üzenet: %s
CHAN_INFO_TIME_REGGED
	    Regisztrálás ideje: %s
CHAN_INFO_LAST_USED
	    Utoljára használva: %s
CHAN_INFO_LAST_TOPIC
	    Utolsó topic: %s
CHAN_INFO_TOPIC_SET_BY
	    A témát beállította: %s
CHAN_INFO_URL
	    URL: %s
CHAN_INFO_EMAIL
	    E-mail cím: %s
CHAN_INFO_BANTYPE
	    Ban típus: %d
CHAN_INFO_OPTIONS
	    Opciók: %s
CHAN_INFO_OPT_KEEPTOPIC
	Témamegõrzés
CHAN_INFO_OPT_OPNOTICE
	Opok értesítése
CHAN_INFO_OPT_PEACE
	Békesség
CHAN_INFO_OPT_PRIVATE
	Privát
CHAN_INFO_OPT_RESTRICTED
	Korlátozott hozzáférés
CHAN_INFO_OPT_SECURE
	Secure
CHAN_INFO_OPT_SECUREOPS
	Opjog védelme
CHAN_INFO_OPT_SECUREFOUNDER
	Founderjog védelme
CHAN_INFO_OPT_SIGNKICK
	Signed kicks
CHAN_INFO_OPT_TOPICLOCK
	Témaváltás lezárása
CHAN_INFO_OPT_XOP
	xOP lista rendszer
CHAN_INFO_OPT_NONE
	Nincs
CHAN_INFO_MODE_LOCK
		Rögzített módok: %s
CHAN_INFO_EXPIRE
	      Expires in: %s
CHAN_INFO_NO_EXPIRE
	Ez a csatorna nem fog elévülni.

# LIST válaszok
CHAN_LIST_SYNTAX
	LIST pattern
CHAN_LIST_SERVADMIN_SYNTAX
	LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
CHAN_LIST_HEADER
	%s listájának egyezõ bejegyzései
CHAN_LIST_FORMAT
		%-20s  %s
CHAN_LIST_END
	Lista vége - %d/%d találat megjelenítve.

# INVITE válaszok
CHAN_INVITE_SYNTAX
	INVITE #szoba

# UNBAN válaszok
CHAN_UNBAN_SYNTAX
	UNBAN #szoba
CHAN_UNBANNED
	%s csatornán a tiltást levetetted magadról.

# TOPIC válaszok
CHAN_TOPIC_SYNTAX
	TOPIC #szoba [topic]

# CLEAR válaszok
CHAN_CLEAR_SYNTAX
	CLEAR #szoba mit?
CHAN_CLEARED_BANS
	A (%s) csatornáról minden kitiltás levéve.
CHAN_CLEARED_EXCEPTS
	A (%s) csatornáról minden kivétel levéve.
CHAN_CLEARED_MODES
	A (%s) csatornáról minden mód el levéve.
CHAN_CLEARED_OPS
	A (%s) csatornán minden +o mód levéve.
CHAN_CLEARED_HOPS
	A (%s) csatornán minden +h mód levéve.
CHAN_CLEARED_VOICES
	A (%s) csatornán minden +v mód levéve.
CHAN_CLEARED_USERS
	A (%s) csatornáról minden felhasználó ki lett rúgva.
CHAN_CLEARED_INVITES
	All invites on channel %s have been removed.

# GETPASS válaszok
CHAN_GETPASS_SYNTAX
	GETPASS #szoba
CHAN_GETPASS_UNAVAILABLE
	GETPASS elérhetetlen, mig az encryption használatban van.
CHAN_GETPASS_PASSWORD_IS
	A (%s) csatorna  jelszava: %s.

# SENDPASS válaszok
CHAN_GETKEY_SYNTAX
	GETKEY channel
CHAN_GETKEY_NOKEY
	The channel %s has no key.
CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS #szoba
CHAN_SENDPASS_UNAVAILABLE
	SENDPASS elérhetetlen, amíg az encryption használatban van
CHAN_SENDPASS_SUBJECT
	Csatorna jelszava: (%s)
CHAN_SENDPASS_HEAD
	Hi,
CHAN_SENDPASS_LINE_1
	Kérted a szoba jelszavának megküldését %s címre.
CHAN_SENDPASS_LINE_2
	%s a szobajeszó.Biztonság kedvéért váltóztasd meg olvasás után
CHAN_SENDPASS_LINE_3
	Ha nem te kérted a levelet akkor töréld és feljts el
CHAN_SENDPASS_LINE_4
	KÉRLEN NE VÁLASZOLJ A LEVÉLRE!
CHAN_SENDPASS_LINE_5
	%s administrators.
CHAN_SENDPASS_OK
	Jelszó %s elküldve.
	
# FORBID válaszok
CHAN_FORBID_SYNTAX
	FORBID #szoba [indok]
CHAN_FORBID_SYNTAX_REASON
	FORBID #szoba indok
CHAN_FORBID_SUCCEEDED
	A (%s) szoba letiltva.
CHAN_FORBID_FAILED
	%s szobát nem sikerült letiltani!
CHAN_FORBID_REASON
	Ez a szoba forbidolva lett.
	
# SUSPEND válaszok
CHAN_SUSPEND_SYNTAX
	SUSPEND #szoba [indok]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND #szoba indok
CHAN_SUSPEND_SUCCEEDED
	(%s) szoba használata felfüggesztve.
CHAN_SUSPEND_FAILED
	%s szobát nem sikerült felfüggeszteni!
CHAN_SUSPEND_REASON
	A szoba használata felfüggesztve.

# UNSUSPEND válaszok
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND #szoba
CHAN_UNSUSPEND_ERROR
	Nincs # a szoba neve elõtt.
CHAN_UNSUSPEND_SUCCEEDED
	(%s) szoba újra használható.
CHAN_UNSUSPEND_FAILED
	%s szobát nem sikerült használhatóvá tenni!

# Misc válaszok
CHAN_EXCEPTED
	%s megegyezik egy kivétellel (%s) nem tiltható, amíg a kivétel él.

###########################################################
#
# MemoServ üzenetek
#
###########################################################
	
# Automatikus üzenetek
MEMO_HAVE_NEW_MEMO
	Van egy új üzeneted.
MEMO_HAVE_NEW_MEMOS
	%d darab új üzeneted van.
MEMO_TYPE_READ_LAST
	Használd a %R%s READ LAST parancsot.
MEMO_TYPE_READ_NUM
	Használd a %R%s READ %d parancsot.
MEMO_TYPE_LIST_NEW
	Használd a: %R%s LIST NEW parancsot
MEMO_AT_LIMIT
	Figyelem: Fogadható üzenetek maximális száma (%d).
	Nem fogadhatsz több üzenetet, amíg nem törölsz néhányat.
MEMO_OVER_LIMIT
	Figyelem: Fogadható üzenetek maximális számát (%d).
	Nem fogadhatsz több üzenetet, amíg törölsz néhányat.
MEMO_X_MANY_NOTICE
	Összesen %d üzenet van a %s csatornán.
MEMO_X_ONE_NOTICE
	%d üzenete van a %s szobának.
MEMO_NEW_X_MEMO_ARRIVED
	Új üzenete érkezett a %s szobának.
	Ha szeretnéd elolvasni, írd be: %R%s READ %s %d.
MEMO_NEW_MEMO_ARRIVED
	%s küldött egy új üzenetet.
	Ha szeretnéd elolvasni, írd be: %R%s READ %d.

# Multi-use válaszok
MEMO_HAVE_NO_MEMOS
	Nincs üzeneted.
MEMO_X_HAS_NO_MEMOS
	%s üzenetlistája üres.
MEMO_DOES_NOT_EXIST
	A (%d.) számú üzenet nem szerepel az üzenetlistán!
MEMO_LIST_NOT_FOUND
	A megadott üzenetek nem szerepelnek az üzenetlistán.

# SEND válaszok
MEMO_SEND_SYNTAX
	SEND {nick | #szoba} memo-szöveg
MEMO_SEND_DISABLED
	Sajnálom, az üzenetküldés szünetel.
MEMO_SEND_PLEASE_WAIT
	Kérlek, várj %d másodpercet mielõtt SEND parancsot újra használod!
MEMO_X_GETS_NO_MEMOS
	%s nem fogadhat üzeneteket.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s túl sok üzenettel rendelkezik, és nem fogadhat többet.
MEMO_SENT
	Üzenet elküldve %s részére.
MEMO_MASS_SENT
	Egy massmemo elküldve minden regisztrált felhasználónak.

# CANCEL válaszok
MEMO_CANCEL_SYNTAX
	CANCEL {nick | #szoba}
MEMO_CANCEL_DISABLED
	Sajnálom, a memo visszavonás jelenleg szünetel.
MEMO_CANCEL_NONE
	Nincs visszavont memo üzenet.
MEMO_CANCELLED
	%s részére küldött utolsó memo üzenet visszavonva.
	
# STAFF responses
MEMO_STAFF_SYNTAX
	STAFF memo-text

# LIST válaszok
MEMO_LIST_SYNTAX
	LIST [#szoba] [list | NEW ]
MEMO_HAVE_NO_NEW_MEMOS
	Nem kaptál új üzenetet.
MEMO_X_HAS_NO_NEW_MEMOS
	%s nem kapott új üzenetet.
MEMO_LIST_MEMOS
	%s üzenetei. Olvasáshoz írd be: %R%s READ szám
MEMO_LIST_NEW_MEMOS
	Új üzenet %s részére. Olvasáshoz: %R%s READ szám
MEMO_LIST_CHAN_MEMOS
	%s üzenetei.  Olvasáshoz : %R%s READ %s szám
MEMO_LIST_CHAN_NEW_MEMOS
	%s új üzenetei.  Olvasáshoz,: %R%s READ %s szám
MEMO_LIST_HEADER
	 Sorszám  Küldõ            Dátum/Idõ
MEMO_LIST_FORMAT
	%c %3d  %-16s  %s

# READ válaszok
MEMO_READ_SYNTAX
	READ [#szoba] {list | LAST | NEW }
MEMO_HEADER
	Üzenet szám: %d; feladó: %s (%s). Törléshez: %R%s DEL %d
MEMO_CHAN_HEADER
	Üzenet szám: %d; feladó.: %s (%s). Törléshez: %R%s DEL %s %d
MEMO_TEXT
	%s

# DEL válaszok
MEMO_DEL_SYNTAX
	DEL [#szoba] { szám | list | ALL}
MEMO_DELETED_NONE
	Nem töröltem egy üzenetet sem.
MEMO_DELETED_ONE
	A megadott üzenetet (%d) töröltem.
MEMO_DELETED_SEVERAL
	A megadott üzeneteket %s töröltem.
MEMO_DELETED_ALL
	Minden üzenetet töröltem.
MEMO_CHAN_DELETED_ALL
	Minden üzenetet töröltem a %s csatornán.

# SET válaszok
MEMO_SET_SYNTAX
	SET opció paraméterek
MEMO_SET_DISABLED
	Üzenet opcióinak beállítása szünetel.
MEMO_SET_UNKNOWN_OPTION
	Ismeretlen SET opció: %s

# SET NOTIFY válaszok
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
MEMO_SET_NOTIFY_ON
	%s értesíteni fog az üzenetekrõl csatlakozáskor.
MEMO_SET_NOTIFY_LOGON
	%s értesít az üzenetekrõl csatlakozáskor/AWAY OFF-kor.
MEMO_SET_NOTIFY_NEW
	%s mostmár értesíteni fog, ha új üzeneted érkezik.
MEMO_SET_NOTIFY_OFF
	%s mostmár nem fog értestíteni, ha új üzeneted érkezik.
MEMO_SET_NOTIFY_MAIL
	Az új memo üzeneteidrõl emailben kapsz értesítést.
MEMO_SET_NOTIFY_NOMAIL
	Már nem kapsz értesítést emailben.
MEMO_SET_NOTIFY_INVALIDMAIL
	Nincs beállítva email cím a nevedhez.


# SET LIMIT válaszok
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [#szoba] limit
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [user | #szoba] {limit | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Nem változtathatod meg a max. fogadható üzenetek számát.
MEMO_SET_LIMIT_FORBIDDEN
	Nem változtathatod meg %s üzeneteinek számát.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	A max. bejövõ üzenetek száma nem lehet több, mint %d.
MEMO_SET_LIMIT_TOO_HIGH
	%s max. bejövõ üzenetek száma nem lehet több, mint %d.
MEMO_SET_LIMIT_OVERFLOW
	Túl nagy érték a max.bejövÕ üzenetnek,%d az új értéke.
MEMO_SET_YOUR_LIMIT
	A max. bejövõ üzenetek számanak új értéke: %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Ezentúl nem tudsz üzeneteket fogadni.
MEMO_UNSET_YOUR_LIMIT
	Ezentúl korlátlan számú üzenetet fogadhatsz.
MEMO_SET_LIMIT
	%s maximálisan fogadható üzeneteinek száma: %d.
MEMO_SET_LIMIT_ZERO
	%s ezentúl nem fogadhat üzeneteket.
MEMO_UNSET_LIMIT
	%s ezentúl korlátlan számú üzenetet fogadhat.


# INFO válaszok
MEMO_INFO_SYNTAX
	INFO [#szoba]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [nick | #szoba]
MEMO_INFO_NO_MEMOS
	Jelenleg nincs üzeneted.
MEMO_INFO_MEMO
	Van 1 üzeneted.
MEMO_INFO_MEMO_UNREAD
	Van 1 olvasatlan üzeneted.
MEMO_INFO_MEMOS
	Van %d üzeneted.
MEMO_INFO_MEMOS_ONE_UNREAD
	Van %d üzeneted, ebbõl 1 olvasatlan.
MEMO_INFO_MEMOS_SOME_UNREAD
	Van %d üzeneted, ebbõl %d olvasatlan.
MEMO_INFO_MEMOS_ALL_UNREAD
	Van %d üzeneted; és mind olvasatlan.
MEMO_INFO_LIMIT
	A maximálisan fogadható üzentek száma:  %d.
MEMO_INFO_HARD_LIMIT
	A max. bejövõ üzentek száma: %d,ez fix érték.
MEMO_INFO_LIMIT_ZERO
	A max. bejövõ üzentek száma: 0; és több nem jöhet.
MEMO_INFO_HARD_LIMIT_ZERO
	A max. bejövõ üzentek száma: 0; és nem köhet több. 
	Ez nem változtatható meg.
MEMO_INFO_NO_LIMIT
	Korlátlan számú üzenetet fogadhatsz.
MEMO_INFO_NOTIFY_OFF
	Nem kapsz értesítést az új üzeneteidrõl.
MEMO_INFO_NOTIFY_ON
	Értesítést kapsz új üzenetek érkezésérõl csatlakozáskor.
MEMO_INFO_NOTIFY_RECEIVE
	Értesítést kapsz új üzenetek érkezésekor.
MEMO_INFO_NOTIFY_SIGNON
	Értesítést kapsz az új üzeneteidrõl,ha fellépsz az IRC-re.
MEMO_INFO_X_NO_MEMOS
	%s üzenetlistája üres.
MEMO_INFO_X_MEMO
	%s listáján van 1 üzenet.
MEMO_INFO_X_MEMO_UNREAD
	%s listáján van 1 olvasatlan üzenet.
MEMO_INFO_X_MEMOS
	%s számára %d üzenet érkezett.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s számára %d üzenet érkezett, ebbõl 1 olvasatlan.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s számára %d üzenet érkezett, ebbõl %d olvasatlan.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s számára %d üzenet érkezett, és mind olvasatlan.
MEMO_INFO_X_LIMIT
	%s maximálisan fogadható üzeneteinek száma: %d.
MEMO_INFO_X_HARD_LIMIT
	%s max. fogadható üzenteinek száma: %d, és ez fix érték.
MEMO_INFO_X_NO_LIMIT
	%s korlátlan számú üzenetet fogadhat.
MEMO_INFO_X_NOTIFY_OFF
	%s nem kért értesítést az üzeneteirõl.
MEMO_INFO_X_NOTIFY_ON
	%s értesítést kért csatlakozáskor,és ha új üzenete van,
MEMO_INFO_X_NOTIFY_RECEIVE
	%s értesítést kért, ha új üzenete érkezik.
MEMO_INFO_X_NOTIFY_SIGNON
	%s kérte, csatlakozáskor értesítést kapjon,üzeneteirõl.

# Memo2Mail válaszok
MEMO_MAIL_SUBJECT
	Új memo
MEMO_MAIL_TEXT1
	Hi %s
MEMO_MAIL_TEXT2
	Kaptál egy új memo üzenetet %s nicktõl. Ez a memo a %d sorszámú.
MEMO_MAIL_TEXT3
	Memo Szöveg:

# RSEND válaszok
MEMO_RSEND_PLEASE_WAIT
	Kérlek várj %d másodpercet mielõtt újra használod az RSEND parancsot.
MEMO_RSEND_DISABLED
	Sajnálom, az RSEND használata le van tiltva ezen a hálózaton.
MEMO_RSEND_SYNTAX
	RSEND {nick | #szoba} memo-szöveg
MEMO_RSEND_NICK_MEMO_TEXT
	[auto-memo] A memo, amit küldtél meg lett tekintve.
MEMO_RSEND_CHAN_MEMO_TEXT
	[auto-memo] A memo, amit %s számára küldtél meg lett tekintve.
MEMO_RSEND_USER_NOTIFICATION
	Az értesítõ üzenet %s részére el lett küldve, informálva
	õt arról, hogy elolvastad a memoját.

# CHECK válaszok
MEMO_CHECK_SYNTAX
	CHECK nicknév 
MEMO_CHECK_NOT_READ
	Az utolsó memo, amit %s részére (%s idõpontban) küldtél még olvasatlan.
MEMO_CHECK_READ
	Az utolsó memo, amit %s részére (%s idõpontban) küldtél már olvasott.
MEMO_CHECK_NO_MEMO
	%s nickre nem érkezett memo üzenet tõled.

	
###########################################################
#
# BotServ üzenetek
#
###########################################################
	
# Standard válaszok
MEMO_NO_RSEND_SELF
	You can not request a receipt when sending a memo to yourself.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
BOT_DOES_NOT_EXIST
	%s nevû bot nem létezik.
BOT_NOT_ASSIGNED
	Társítanod kellene egy botot a csatornára mielõtt használod
	ezt a parancsot, írd be %R%S HELP ASSIGN bõvebb infókért.
BOT_NOT_ON_CHANNEL
	A Bot nincs a %s csatornán.

# Kick válaszok (kell egy sor)
BOT_REASON_BADWORD
	Nem használható ez a szó: %s!
BOT_REASON_BADWORD_GENTLE
	Figyelj a stílusra kiskomám!
BOT_REASON_BOLD
	Nem használható kiemelt betû ezen a csatornán!
BOT_REASON_CAPS
	Kapcsold ki a caps lockod!
BOT_REASON_COLOR
	Nem szinezhetsz ezen a csatornán!
BOT_REASON_FLOOD
	Ne floodolj!
BOT_REASON_REPEAT
	Ne ismételd magad!
BOT_REASON_REVERSE
	Nem írj visszafelé ezen a csatornán!
BOT_REASON_UNDERLINE
	Nem használj aláhúzásokat ezen a csatornán!

# !seen válaszok
BOT_SEEN_BOT
	Megtaláltál, %s!
BOT_SEEN_YOU
	Magadat kérdezed le %s, eh?
BOT_SEEN_ON_CHANNEL
	%s a szobában van!
BOT_SEEN_ON_CHANNEL_AS
	%s a szobában van (%s néven) ! 
BOT_SEEN_ON
	%s utoljára %s ideje járt erre.
BOT_SEEN_NEVER
	Sosem láttam %s nicket ebben a szobában.
BOT_SEEN_UNKNOWN
	Én nem tudom, hogy %s kicsoda.:-(((

# BOT válaszok
BOT_BOT_SYNTAX
	BOT ADD nick user hoszt valósnév
	BOT CHANGE réginickújnick [user [hoszt [valósnév]]]
	BOT DEL nick
BOT_BOT_ALREADY_EXISTS
	%s nevû bot már létezik.
BOT_BOT_CREATION_FAILED
	A bot létrehozása sikertelen.
BOT_BOT_READONLY
	A bot módosítás ideiglenesen szünetel.
BOT_BOT_ADDED
	%s!%s@%s (%s) hozzáadva a bot listához.
BOT_BOT_ANY_CHANGES
	A régi info megegyezik az újjal.
BOT_BOT_CHANGED
	A %s nevû bot megváltoztatva %s!%s@%s (%s)
BOT_BOT_DELETED
	A %s nevû bot törölve.

# BOTLIST válaszok
BOT_BOTLIST_HEADER
	Botok listája:
BOT_BOTLIST_PRIVATE_HEADER
	Botok melyeket csak IRC operátorok társíthatnak:
BOT_BOTLIST_FOOTER
	%d botok rendelkezésre állnak.
BOT_BOTLIST_EMPTY
	Nem áll rendelkezésre bot.
	Kérj meg egy szervíz admint, hogy csináljon!

# ASSIGN válaszok
BOT_ASSIGN_SYNTAX
	ASSIGN #szoba nick
BOT_ASSIGN_READONLY
	Bot társítás ideiglenesen nem mûködik.
BOT_ASSIGN_ALREADY
	A %s nevû bot már társítva van a %s szobához.
BOT_ASSIGN_ASSIGNED
	A %s nevû bot társítva a %s szobához.
	
# UNASSIGN válaszok
BOT_UNASSIGN_SYNTAX
	UNASSIGN #szoba
BOT_UNASSIGN_UNASSIGNED
	Nincsen bot kijelölve %s szobában többet.

# INFO válaszok
BOT_INFO_SYNTAX
	INFO {#szoba | nick}
BOT_INFO_NOT_FOUND
	%s nem érvényes bot vagy regisztrált szoba.
BOT_INFO_BOT_HEADER
	Információk a %s nevû botról:
BOT_INFO_BOT_MASK
	Maszk : %s@%s
BOT_INFO_BOT_REALNAME
	Valódi neve: %s
BOT_INFO_BOT_CREATED
	Született: %s
BOT_INFO_BOT_USAGE
	Használva: %d szobában
BOT_INFO_BOT_OPTIONS
	Beállításai: %s
BOT_INFO_OPT_PRIVATE
	Privát
BOT_INFO_CHAN_HEADER
	Információ a %s szobáról:
BOT_INFO_CHAN_BOT
	Bot név: %s
BOT_INFO_CHAN_BOT_NONE
	Bot név: nincs társítva még.
BOT_INFO_CHAN_KICK_BADWORDS
		Csúnya szavakért kirúgás: %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
		Csúnya szavakért kirúgás: %s (%d kirúgás után kitiltás)
BOT_INFO_CHAN_KICK_BOLDS
		Félkövér betûkért kirúgás: %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
		Félkövér betûkért kirúgás: %s (%d kirúgás után kitiltás)
BOT_INFO_CHAN_KICK_CAPS_ON
		Caps Lockért kirúgás: %s (minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
		Caps Lockért kirúgás: %s (%d kirúgás után kitiltás; min. %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
		Caps Lockért kirúgás : %s
BOT_INFO_CHAN_KICK_COLORS
		Szinezésért kirúgás: %s
BOT_INFO_CHAN_KICK_COLORS_BAN
		Szinezésért kirúgás: %s (%d kirúgás után kitiltás)
BOT_INFO_CHAN_KICK_FLOOD_ON
		Floodért kirúgás: %s (%d sor %ds mp alatt)
BOT_INFO_CHAN_KICK_FLOOD_BAN
		Floodért kirúgás: %s (%d kirúgás után kitiltás; %d sor %ds mp alatt)
BOT_INFO_CHAN_KICK_FLOOD_OFF
		Floodért kirúgás: %s
BOT_INFO_CHAN_KICK_REPEAT_ON
		Ismétlésért kirúgás : %s (%d alkalommal)
BOT_INFO_CHAN_KICK_REPEAT_BAN
		Ismétlésért kirúgás: %s (%d kirúgás után kitiltás; %d idõre)
BOT_INFO_CHAN_KICK_REPEAT_OFF
		Ismétlésért kirúgás: %s
BOT_INFO_CHAN_KICK_REVERSES
		Visszafelé írásért kirúgás: %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
		Visszafelé írásért kirúgás: %s (%d kirúgás után kitiltás)
BOT_INFO_CHAN_KICK_UNDERLINES
		Aláhúzásért kirúgás: %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
		Aláhúzásért kirúgás: %s (%d kirúgás után kitiltás)
BOT_INFO_ACTIVE
	engedélyezve
BOT_INFO_INACTIVE
	letiltva
BOT_INFO_CHAN_OPTIONS
		Beállítások: %s
BOT_INFO_OPT_DONTKICKOPS
	Opok védelme
BOT_INFO_OPT_DONTKICKVOICES
	Voices védelme
BOT_INFO_OPT_FANTASY
	Fantasy
BOT_INFO_OPT_GREET
	Üdvözlés
BOT_INFO_OPT_NOBOT
	Nincs bot
BOT_INFO_OPT_SYMBIOSIS
	Együttélés
BOT_INFO_OPT_NONE
	Nincsen

# SET válaszok
BOT_SET_SYNTAX
	SET #szoba opció beállítások
BOT_SET_DISABLED
	Bot beállítás idõlegesen szünetel.
BOT_SET_UNKNOWN
	Ismeretlen opció: %s.
	Használd %R%S HELP SET információért.

# SET DONTKICKOPS üzenetek
BOT_SET_DONTKICKOPS_SYNTAX
	SET #szoba DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	A Bot nem fog opot kirúgni a %s szobából.
BOT_SET_DONTKICKOPS_OFF
	A Bot most már opot is kirúg a %s szobából.

# SET DONTKICKVOICES üzenetek
BOT_SET_DONTKICKVOICES_SYNTAX
	SET #szoba DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	A Bot nem fogja a hangadókat kirúgni a %s szobában.
BOT_SET_DONTKICKVOICES_OFF
	A Bot most már a hangadókat is kirúgja a %s szobában.

# SET FANTASY üzenetek
BOT_SET_FANTASY_SYNTAX
	SET #szoba FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Fantasy mód most ON a %s csatornán.
BOT_SET_FANTASY_OFF
	Fantasy mód most OFF a %s csatornán.

# SET GREET üzenetek
BOT_SET_GREET_SYNTAX
	SET szoba GREET {ON|OFF}
BOT_SET_GREET_ON
	Üdvözlõ mód most ON a %s szobán.
BOT_SET_GREET_OFF
	Üdvözlõ mód most OFF a %s szobán.

# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET botname NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Nincs Bot mód most ONa %s szobán.
BOT_SET_NOBOT_OFF
	Nincs Bot mód most OFFa %s szobán.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET botnév PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Private mód a %s botban most ON.
BOT_SET_PRIVATE_OFF
	Private mód a %s botban most OFF.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET szoba SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Együttélés mód most ONa %s szobán.
BOT_SET_SYMBIOSIS_OFF
	Együttélés mód most OFFa %s szobán.

# KICK válaszok
BOT_KICK_SYNTAX
	KICK #szoba opció {ON|OFF} [beállítások]
BOT_KICK_DISABLED
	Kirúgás használata ideiglenesen letiltva.
BOT_KICK_UNKNOWN
	Ismeretlen opció: %s.
	További információért írd be: %R%S HELP KICK 
BOT_KICK_BAD_TTB
	%s nem fogadható el banolási idõnek.

BOT_KICK_BADWORDS_ON
	Bot kirúgja tiltott szó használatáért. Használd a BADWORDS
	parancsot hogy hozzáadj vagy törölj tiltott szavakat a listából.
BOT_KICK_BADWORDS_ON_BAN
	Bot kirúgja tiltott szó használatáért, és kitiltja 
	%d kirúgás után a usert. Használd a BADWORDS parancsot
	hogy hozzáadj vagy törölj tiltott szavakat a listából.
BOT_KICK_BADWORDS_OFF
	A Bot most már nem fog kirúgni tiltott szó miatt.
BOT_KICK_BOLDS_ON
	A Bot kirúgja félkövér betû használat miatt a usert.
BOT_KICK_BOLDS_ON_BAN
	A Bot kirúgja félkövér betû használat miatt, és 
	kitiltja %d kirúgás után a usert.
BOT_KICK_BOLDS_OFF
	A Bot nem fog kirúgni félkövér betû használat miatt.
BOT_KICK_CAPS_ON
	Bot most már kirúgja caps lock miatt a usert (legalább
	%d karakter és ennek %d%%-a nagybetû).
BOT_KICK_CAPS_ON_BAN
	Bot most már kirúgja caps lock miatt, (legalább
	%d karakter és ennek %d%%-a nagybetû), kitiltja 
	%d kirúgás után a usert.
BOT_KICK_CAPS_OFF
	Bot nem fog kirúgni caps lock miatt.
BOT_KICK_COLORS_ON
	A Bot kirúgja szines írás miatt a usert.
BOT_KICK_COLORS_ON_BAN
	A Bot kirúgja szines írás miatt, és kitiltja 
	%d kirúgás után a usert.
BOT_KICK_COLORS_OFF
	Bot nem fog kirúgni szines írás miatt.
BOT_KICK_FLOOD_ON
	Bot kirúgja flood miatt a usert. (%d sor %d mp alatt)
BOT_KICK_FLOOD_ON_BAN
	A Bot kirúgja flood miatt (%d sor %d mp alatt),
	és kitiltja %d kirúgás után a usert.
BOT_KICK_FLOOD_OFF
	A Bot nem fog kirúgni flood miatt.
BOT_KICK_REPEAT_ON
	A Bot kirúgja ismétlés miatt a usert (ha %d alkalommal
	ismétli ugyanazt a sort).
BOT_KICK_REPEAT_ON_BAN
	A Bot kirúgja ismétlés miatt (ha %d alkalommal ismétli
	ugyanazt a sort), és kitiltja %d kirúgás után a usert.
BOT_KICK_REPEAT_OFF
	A Bot nem fog kirúgni ismétlés miatt.
BOT_KICK_REVERSES_ON
	A Bot kirúg fordított írás miatt.
BOT_KICK_REVERSES_ON_BAN
	A Bot kirúgja fordított írás miatt, és kitiltja %d
	kirúgás után a usert.
BOT_KICK_REVERSES_OFF
	A Bot nem fog rúgni fordított írás miatt.
BOT_KICK_UNDERLINES_ON
	A Bot kirúgja aláhúzásos írás miatt a usert.
BOT_KICK_UNDERLINES_ON_BAN
	A Bot kirúgja aláhúzásos írás miatt, és kitiltja
	%d kirúgás után a usert.
BOT_KICK_UNDERLINES_OFF
	Bot nem fog kirúgni aláhúzásos írás miatt.

# BADWORDS üzenetek
BOT_BADWORDS_SYNTAX
	BADWORDS #szoba{ADD|DEL|LIST|CLEAR}[szó|sorszám]
	[SINGLE|START|END]
BOT_BADWORDS_DISABLED
	Csatornán tiltott szaval listájának változtatása letiltva.
BOT_BADWORDS_REACHED_LIMIT
	Csak %d tiltott szó lehet a csatornán.
BOT_BADWORDS_ALREADY_EXISTS
	A %s szót már tartalmazza a %s tiltott szavak listája
BOT_BADWORDS_ADDED
	A %s szó hozzáadva a %s tiltott szavak listájához.
BOT_BADWORDS_NO_SUCH_ENTRY
	Nincs (#%d) ilyen szó a %s tiltott szavak listáján.
BOT_BADWORDS_NOT_FOUND
	A %s szó nem található a %s tiltott szavak listáján.
BOT_BADWORDS_NO_MATCH
	Nincs ilyen szó %s a tiltott szavak listáján.
BOT_BADWORDS_DELETED
	A %s szó törölve %s tiltott szavak listájából.
BOT_BADWORDS_DELETED_ONE
	Törölve 1 bejegyzés %s tiltott szavak listájáról.
BOT_BADWORDS_DELETED_SEVERAL
	Törölve %d bejegyzés %s tiltott szavak listájáról.
BOT_BADWORDS_LIST_EMPTY
	%s tiltott szavak listája üres.
BOT_BADWORDS_LIST_HEADER
	%s tiltott szavak listája:
	  Szám   szó                           tipus
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	Tiltott szó lista üres.

# SAY válaszok
BOT_SAY_SYNTAX
	SAY #szoba szöveg

# ACT válaszok
BOT_ACT_SYNTAX
	ACT #szoba szöveg

# Hibák
BOT_EXCEPT
	Fehasználó társítása szobával kivétel.
BOT_BAD_NICK
	Bot Nick csak érvényes karaktereket tartalmazhat.
BOT_BAD_HOST
	Bot Hoszt csak érvényes karaktereket tartalmazhat.
BOT_BAD_IDENT
	Bot Ident csak érvényes karaktereket tartalmazhat.
BOT_LONG_IDENT
	Bot Idents may only contain %d characters.
BOT_LONG_HOST
	Bot Hosts may only contain %d characters.

###########################################################
#
# OperServ üzenetek
#
###########################################################
	
# Általános üzenetek
OPER_BOUNCY_MODES
	Szervíz nem tudja változtatni a módokat.
	A szervereid jól vannak beállítva?
OPER_BOUNCY_MODES_U_LINE
	Szervíz nem tudja változtatni a módokat.
	A szerverek U:lines beállitása helyes?

# GLOBAL válaszok
OPER_GLOBAL_SYNTAX
	GLOBAL üzenet

# STATS válaszok
OPER_STATS_UNKNOWN_OPTION
	Ismeretlen STATS opció: %s.
OPER_STATS_CURRENT_USERS
	Aktuális felhasználók: %d (%d op)
OPER_STATS_MAX_USERS
	Legtöbb felhasználó: %d (%s)
# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Szervíz fent van %d nap, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Szervíz fent van %d nap, %02d:%02d
OPER_STATS_UPTIME_HMS
	Szervíz fent van %d óra, %d perce
OPER_STATS_UPTIME_HM1S
	Szervíz fent van %d óra, %d perce
OPER_STATS_UPTIME_H1MS
	Szervíz fent van %d óra, %d perce
OPER_STATS_UPTIME_H1M1S
	Szervíz fent van %d óra, %d perce
OPER_STATS_UPTIME_1HMS
	Szervíz fent van %d óra, %d perce
OPER_STATS_UPTIME_1HM1S
	Szervíz fent van %d óra, %d perce
OPER_STATS_UPTIME_1H1MS
	Szervíz fent van %d óra, %d perce
OPER_STATS_UPTIME_1H1M1S
	Szervíz fent van %d óra, %d perce
OPER_STATS_UPTIME_MS
	Szervíz fent van %d perc, %d másodperce.
OPER_STATS_UPTIME_M1S
	Szervíz fent van %d perc, %d másodperce.
OPER_STATS_UPTIME_1MS
	Szervíz fent van %d perc, %d másodperce.
OPER_STATS_UPTIME_1M1S
	Szervíz fent van %d perc, %d másodperce.
OPER_STATS_BYTES_READ
	Olvasott Byteok : %5d kB
OPER_STATS_BYTES_WRITTEN
	Írott Byteok : %5d kB
OPER_STATS_USER_MEM
	Felhasználó	 : %6d bejegyzés, %5d kB
OPER_STATS_CHANNEL_MEM
	Szoba	       : %6d bejegyzés, %5d kB
OPER_STATS_GROUPS_MEM
	NS Csoport	 : %6d bejegyzés, %5d kB
OPER_STATS_ALIASES_MEM
	NS Aliases	 : %6d bejegyzés, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ	 : %6d bejegyzés, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ	 : %6d bejegyzés, %5d kB
OPER_STATS_HOSTSERV_MEM
	HostServ      : %6d records, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ	 : %6d bejegyzés, %5d kB
OPER_STATS_SESSIONS_MEM
	Session	 : %6d bejegyzés, %5d kB
OPER_STATS_AKILL_COUNT
	Aktuális AKILLek száma: %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Alap AKILL lejárati idõ: %d nap
OPER_STATS_AKILL_EXPIRE_DAY
	Alap AKILL lejárati idõ: 1 nap
OPER_STATS_AKILL_EXPIRE_HOURS
	Alap AKILL lejárati idõ: %d óra
OPER_STATS_AKILL_EXPIRE_HOUR
	Alap AKILL lejárati idõ: 1 óra
OPER_STATS_AKILL_EXPIRE_MINS
	Alap AKILL lejárati idõ: %d perc
OPER_STATS_AKILL_EXPIRE_MIN
	Alap AKILL lejárati idõ: 1 perc
OPER_STATS_AKILL_EXPIRE_NONE
	Alap AKILL lejárati idõ: Nem elévülõ
OPER_STATS_SGLINE_COUNT
	Aktuális SGLINEok száma: %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Alap SGLINE lejárati idõ: %d nap
OPER_STATS_SGLINE_EXPIRE_DAY
	Alap SGLINE lejárati idõ: 1 nap
OPER_STATS_SGLINE_EXPIRE_HOURS
	Alap SGLINE lejárati idõ: %d óra
OPER_STATS_SGLINE_EXPIRE_HOUR
	Alap SGLINE lejárati idõ: 1 óra
OPER_STATS_SGLINE_EXPIRE_MINS
	Alap SGLINE lejárati idõ: %d perc
OPER_STATS_SGLINE_EXPIRE_MIN
	Alap SGLINE lejárati idõ: 1 perc
OPER_STATS_SGLINE_EXPIRE_NONE
	Alap SGLINE lejárati ido: Nem elévülõ
OPER_STATS_SQLINE_COUNT
	Aktuális SQLINEok száma : %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Alap SQLINE lejárati idõ: %d nap
OPER_STATS_SQLINE_EXPIRE_DAY
	Alap SQLINE lejárati idõ: 1 nap
OPER_STATS_SQLINE_EXPIRE_HOURS
	Alap SQLINE lejárati idõ: %d óra
OPER_STATS_SQLINE_EXPIRE_HOUR
	Alap SQLINE lejárati idõ: 1 óra
OPER_STATS_SQLINE_EXPIRE_MINS
	Alap SQLINE lejárati idõ: %d perc
OPER_STATS_SQLINE_EXPIRE_MIN
	Alap SQLINE lejárati idõ: 1 perc
OPER_STATS_SQLINE_EXPIRE_NONE
	Alap SQLINE lejárati idõ Nem elévülõ
OPER_STATS_SZLINE_COUNT
	Aktuális SZLINEok száma: %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Alap SZLINE lejárati idõ: %d nap
OPER_STATS_SZLINE_EXPIRE_DAY
	Alap SZLINE lejárati idõ: 1 nap
OPER_STATS_SZLINE_EXPIRE_HOURS
	Alap SZLINE lejárati idõ: %d óra
OPER_STATS_SZLINE_EXPIRE_HOUR
	Alap SZLINE lejárati idõ: 1 óra
OPER_STATS_SZLINE_EXPIRE_MINS
	Alap SZLINE lejárati idõ: %d perc
OPER_STATS_SZLINE_EXPIRE_MIN
	Alap SZLINE lejárati idõ: 1 perc
OPER_STATS_SZLINE_EXPIRE_NONE
	Alap SZLINE lejárati idõ: Nem elévülõ
OPER_STATS_RESET
	Statisztika nullázva.

# MODE válaszok
OPER_STATS_UPLINK_SERVER
	Uplink server: %s
OPER_STATS_UPLINK_CAPAB
	Uplink capab: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Servers found: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE szoba mód

# UMODE reagálások
OPER_UMODE_SYNTAX
	UMODE nick mód
OPER_UMODE_SUCCESS
	%s usermódjai megváltoztatva.
OPER_UMODE_CHANGED
	%s váltóztatott a módodon.

# OLINE válaszok
OPER_OLINE_SYNTAX
	OLINE nick flagek
OPER_OLINE_SUCCESS
	%s operflagek megadva a %s nicknek.
OPER_OLINE_IRCOP
	Most már IRC Operátor vagy!

# CLEARMODES válaszok
OPER_CLEARMODES_SYNTAX
	CLEARMODES #szoba [ALL]
OPER_CLEARMODES_DONE
	Bináris módok és kitiltások törölve a %s csatornáról.
OPER_CLEARMODES_ALL_DONE
	Összes mód törölve a %s csatornáról.
	
# KICK válaszok
OPER_KICK_SYNTAX
	KICK #szoba user indok

# SVSNICK válaszok
OPER_SVSNICK_SYNTAX
	SVSNICK nick újnick 
OPER_SVSNICK_NEWNICK
	A nicked %s meg lett változtatva erre: %s.

# ADMIN válaszok
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [nick | bejegyzések]
OPER_ADMIN_SKELETON
	Szervíz most skeleton módban fut, az ADMIN parancs nem elérhetõ
OPER_ADMIN_EXISTS
	%s már szerepel a szervíz admin litában.
OPER_ADMIN_REACHED_LIMIT
	Csak %d szervíz admin lehet a listán.
OPER_ADMIN_ADDED
	%s hozzáadva szervíz admin listához.
OPER_ADMIN_NOT_FOUND
	%s nem található szervíz admin listán.
OPER_ADMIN_NO_MATCH
	Nincs ilyen a szervíz admin listán.
OPER_ADMIN_DELETED
	%s törölve a szervíz admin listáról.
OPER_ADMIN_DELETED_ONE
	Töröltve 1 bejegyzés a szervíz admin listáról.
OPER_ADMIN_DELETED_SEVERAL
	Töröltve %d bejegyzés a szervíz admin listáról.
OPER_ADMIN_LIST_EMPTY
	Szervíz admin lista üres.
OPER_ADMIN_LIST_HEADER
	Szerviz admin lista:
	  Szám   Nick
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	Szervíz admin lista törölve.
OPER_ADMIN_MOVED
	%s has been moved to Services Administrators list.

# OPER válaszok
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | bejegyzések]
OPER_OPER_SKELETON
	Szervíz most skeleton módban fut, az OPER parancs nem elérhetõ.
OPER_OPER_EXISTS
	%s már szerepel a szervíz operátor listán.
OPER_OPER_REACHED_LIMIT
	Csak %d szervíz operátor lehet a listán.
OPER_OPER_ADDED
	%s hozzáadva a szervíz operátor listához.
OPER_OPER_NOT_FOUND
	%s nem található a szervíz operátor listán.
OPER_OPER_NO_MATCH
	Nincs ilyen bejegyzés a szervíz operátor listán.
OPER_OPER_DELETED
	%s törölve szervíz operátor listáról.
OPER_OPER_DELETED_ONE
	Törölve 1 bejegyzés a szervíz operátor listáról.
OPER_OPER_DELETED_SEVERAL
	Törölve %d bejegyzés a szervíz operátor listától.
OPER_OPER_LIST_EMPTY
	Szervíz operátor lista üres.
OPER_OPER_LIST_HEADER
	Szervíz operátor lista:
	  Szám   Nick
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	Szervíz operátor lista törölve.
OPER_OPER_MOVED
	%s has been moved to Services Operators list.

# AKILL válaszok
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+lejárat] {maszk | sorszám} [indok]]
OPER_AKILL_EXISTS
	%s már bekapcsolva az AKILL lista.
OPER_AKILL_ALREADY_COVERED
	%s van már %s ilyen bejegyzés.
OPER_AKILL_REACHED_LIMIT
	Te csak %d AKILL-t jegyezhetsz.
OPER_AKILL_NO_NICK
	Emlékeztetõ: AKILL maszk nem tartalmazhat nicknevet;
	ellenõrizd, hogy nem tartalmazhat nick részt a maszk.
OPER_AKILL_ADDED
	%s hozzáadva az AKILL listához.
OPER_AKILL_CHANGED
	Lejárati idõ megváltozott: %s.
OPER_AKILL_NOT_FOUND
	%s nem található az AKILL listán.
OPER_AKILL_NO_MATCH
	Nincs ilyen bejegyzés az AKILL listán.
OPER_AKILL_DELETED
	%s törölve az AKILL listáról.
OPER_AKILL_DELETED_ONE
	Törölve 1 bejegyzés az AKILL listáról.
OPER_AKILL_DELETED_SEVERAL
	Törölve %d bejegyzés az AKILL listáról.
OPER_AKILL_LIST_EMPTY
	AKILL lista üres.
OPER_AKILL_LIST_HEADER
	Aktuális AKILL lista:
	  Szám   Maszk                              Indok
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Aktuális AKILL lista:
# szám, mask, set-by, set-idõ, lejárat, indok
OPER_AKILL_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_AKILL_CLEAR
	Az AKILL lista törölve.
OPER_CHANKILL_SYNTAX
	CHANKILL [+lejárat] {#szoba} [indok]

# SGLINE válaszok
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[ +lejárat] {maszk| sorszám }[:Indok]]
OPER_SGLINE_UNSUPPORTED
	SGLINE nem használható ezen a hálózaton.
OPER_SGLINE_EXISTS
	%s már szerepel az SGLINE listán.
OPER_SGLINE_ALREADY_COVERED
	%s társítva van %s által.
OPER_SGLINE_REACHED_LIMIT
	Te csak %d számú SGLINE-t jegyezhetsz.
OPER_SGLINE_ADDED
	%s hozzáadva a SGLINE listához.
OPER_SGLINE_CHANGED
	Lejárati idõ megváltozott: %s .
OPER_SGLINE_NOT_FOUND
	%s nincs a SGLINE listán.
OPER_SGLINE_NO_MATCH
	Nincs ilyen bejegyzés az SGLINE listán.
OPER_SGLINE_DELETED
	%s törölve az SGLINE listáról.
OPER_SGLINE_DELETED_ONE
	Törölve 1 bejegyzés a SGLINE listáról.
OPER_SGLINE_DELETED_SEVERAL
	Törölve %d bejegyzés a SGLINE listáról.
OPER_SGLINE_LIST_EMPTY
	SGLINE lista üres.
OPER_SGLINE_LIST_HEADER
	Aktuális SGLINE lista:
	  Szám   Maszk                              Indok
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Aktuális SGLINE lista:
# szám, mask, set-by, set-idõ, lejárat, indok
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (%s által, ekkor: %s; %s)
	      %s
OPER_SGLINE_CLEAR
	Az SGLINE lista törölve.

# SQLINE válaszok
OPER_SQLINE_SYNTAX
	SQLINE {ADD|DEL|LIST|VIEW|CLEAR}[ [ +lejárat ] { maszk | sorszám }[ indok ]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	Szoba SQLINE-ok nem támogatottak ezen az IRCd-n,ezért
	nem tudod használni.
OPER_SQLINE_EXISTS
	%s már szerpel az SQLINE listán.
OPER_SQLINE_ALREADY_COVERED
	%s társítva van %s által.
OPER_SQLINE_REACHED_LIMIT
	Te csak %d SQLINE-t jegyezhetsz.
OPER_SQLINE_ADDED
	%s hozzáadva az SQLINE listához.
OPER_SQLINE_CHANGED
	Lejárati idõ %s megválzott.
OPER_SQLINE_NOT_FOUND
	%s nnem található az SQLINE listán.
OPER_SQLINE_NO_MATCH
	Nincs ilyen bejegyzés az SQLINE listán.
OPER_SQLINE_DELETED
	%s törölve az SQLINE listáról.
OPER_SQLINE_DELETED_ONE
	Törölve 1 bejegyzés az SQLINE listáról.
OPER_SQLINE_DELETED_SEVERAL
	Törölve %d bejegyzés az SQLINE listáról.
OPER_SQLINE_LIST_EMPTY
	SQLINE lista üres.
OPER_SQLINE_LIST_HEADER
	Aktuális SQLINE lista:
	  Szám   Mask                              Indok
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Aktuális SQLINE lista:
# Szám, mask, set-by, set-Idõ, lejárat, indok
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (%s által, ekkor: %s; %s)
	      %s
OPER_SQLINE_CLEAR
	Az SQLINE lista törölve.

# SZLINE válaszok
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR }[[ +lejárat ] { maszk | sorszám } [ indok ]]
OPER_SZLINE_UNSUPPORTED
	SZLINE nem használható ezen a hálózaton.
OPER_SZLINE_EXISTS
	%s már szerepel az SZLINE listán.
OPER_SZLINE_ALREADY_COVERED
	%s társítva %s által.
OPER_SZLINE_REACHED_LIMIT
	Te csak %d SZLINE-t jegyezhetsz.
OPER_SZLINE_ONLY_IPS
	Emlékeztetõ:Te hozzáadhatsz IP maszkot SZLINE listához.
OPER_SZLINE_ADDED
	%s hozzáadva az SZLINE listához.
OPER_SZLINE_CHANGED
	Lejárati idõ megváltozott: %s.
OPER_SZLINE_NOT_FOUND
	%s nem található az SZLINE listán.
OPER_SZLINE_NO_MATCH
	Nincs ilyen bejegyzés az SZLINE listán.
OPER_SZLINE_DELETED
	%s törölve az SZLINE listáról.
OPER_SZLINE_DELETED_ONE
	Törölve 1 bejegyzés az SZLINE listáról.
OPER_SZLINE_DELETED_SEVERAL
	Törölve %d bejegyzés az SZLINE listáról.
OPER_SZLINE_LIST_EMPTY
	SZLINE lista üres.
OPER_SZLINE_LIST_HEADER
	Aktuális SZLINE lista:
	Szám	Mask					Indok
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Aktuális SZLINE lista:
# Sszám, maszk, set-by, set-idõ, lejárat,Indok
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (%s által, ekkor: %s; %s)
	      %s
OPER_SZLINE_CLEAR
	Az SZLINE lista törölve

# SET válaszok
OPER_SET_SYNTAX
	SET opció beállítás
OPER_SET_IGNORE_ON
	Mellõzés kód lessz használva.
OPER_SET_IGNORE_OFF
	Mellõzés kód nem lessz használva.
OPER_SET_IGNORE_ERROR
	Mellõzés beállitásnak kell lenni ON vagy OFF.
OPER_SET_READONLY_ON
	Szervíz most  csak olvas módban.
OPER_SET_READONLY_OFF
	Szervíz most olvas ír módban.
OPER_SET_READONLY_ERROR
	READONLY beállitáshoz kell lenni ON vagy OFF.
OPER_SET_LOGCHAN_ON
	Szervíz most kiegészíti log üzeneteket %s -hoz.
OPER_SET_LOGCHAN_OFF
	A Szervíz nem írja ki a napló üzeneteket a csatornára.
OPER_SET_LOGCHAN_ERROR
	LOGCHAN beállításához kell ONvagy OFFés 
	LogCsatornát kell beállítani.
OPER_SET_DEBUG_ON
	Szervíz most debug módban fut.
OPER_SET_DEBUG_OFF
	Szervíz most már nem fut debug módban.
OPER_SET_DEBUG_LEVEL
	Szervíz most debug módban (szint: %d).
OPER_SET_DEBUG_ERROR
	A DEBUG beállításhoz kell ON,/OFF,/ pozitív szám.
OPER_SET_NOEXPIRE_ON
	Szervíz nem elévülõ módban van.
OPER_SET_NOEXPIRE_OFF
	Szervíz elévülõ módban van.
OPER_SET_NOEXPIRE_ERROR
	Az NOEXPIRE beállításhoz kell ON vagy OFF.
OPER_SET_UNKNOWN_OPTION
	Ismeretlen beállítás %s.
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

# NOOP válaszok
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} szerver
OPER_NOOP_SET
	Összes O:lines %s módosítva.
OPER_NOOP_REVOKE
	Összes O:lines %s újra aktív.

# JUPE válaszok
OPER_JUPE_SYNTAX
	JUPE szervernév [leírás]
OPER_JUPE_HOST_ERROR
	Kérlek használj érvényes szerver nevet amikor jupolsz!
OPER_JUPE_INVALID_SERVER
	You can not jupe your services server or your uplink server.

# RAW válaszok
OPER_RAW_SYNTAX
	RAW text

# UPDATE válaszok
OPER_UPDATING
	Adatbázis mentve.

# RELOAD válaszok
OPER_RELOAD
	Szervíz konfigurációs fájl újratöltve.

# RESTART válaszok
OPER_CANNOT_RESTART
	SERVICES_BIN nincs meghatározva; nem tud újraindulni.
	Térj vissza a \2configure\2 scripthez és kez újra a forgatást. 
	Szervízhez engedélyezd a RESTART parancsot.

# IGNORE válaszok
OPER_IGNORE_SYNTAX
	IGNORE {ADD|DEL|LIST|CLEAR} [idõ] [nick | mask]
OPER_IGNORE_VALID_TIME
	Használj érvényes számot idõnek. 
	(Adj meg érvényes számot az idõhöz.)
OPER_IGNORE_TIME_DONE
	%s nick most mellõzte %s nicket.
OPER_IGNORE_PERM_DONE
	%s nick most véglegesen mellõzve van.
OPER_IGNORE_DEL_DONE
	%s nick most már nics mellõzve.
OPER_IGNORE_LIST
	Szervíz mellõzés lista:
OPER_IGNORE_LIST_NOMATCH
	%s nick nem található a mellõzöttek listáján.
OPER_IGNORE_LIST_EMPTY
	Mellõzöttek lista üres.
OPER_IGNORE_LIST_CLEARED
	Mellõzöttek lista törölve.

# KILLCLONES válaszok
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	Nem talált ilyen felhasználót %s.

# CHANLIST válaszok
OPER_CHANLIST_HEADER
	Szoba lista:
	Név			User Mód   Topic
OPER_CHANLIST_HEADER_USER
	%s szoba listája:
	Név 			User Mód   Topic
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	Vége a szoba listának.

# USERLIST válaszok
OPER_USERLIST_HEADER
	Felhasználó lista:
	Nick                 Maszk
OPER_USERLIST_HEADER_CHAN
	%s felhasználó lista:
	Nick                 Maszk
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Vége a felhasználó listának.

# CACHE válaszok
OPER_SUPER_ADMIN_ON
	Most már Szuper-Admin vagy
OPER_SUPER_ADMIN_OFF
	Te nem vagy már Szuper-Admin
OPER_SUPER_ADMIN_SYNTAX
	SET SuperAdmin  ON vagy OFF 
	(engedélyezni kell a services.conf-ban)
OPER_SUPER_ADMIN_WALL_ON
	%s most már Szuper-Admin
OPER_SUPER_ADMIN_WALL_OFF
	%s már nem Szuper-Admin
OPER_SUPER_ADMIN_ONLY
	Csak Szuper-Admin használhatja a parancsot.
OPER_STAFF_LIST_HEADER
	On Szint Nick
OPER_STAFF_FORMAT
	 %c %s  %s
OPER_STAFF_AFORMAT
	 %c %s  %s [%s]

# DefCon Üzenet

OPER_DEFCON_SYNTAX
	DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	Szervíz Defcon módban fut, próbáld késõbb.
OPER_DEFCON_NO_CONF
	A Defcon rendszert engedélyezni kell a services.conf-ban
OPER_DEFCON_CHANGED
	Szervíz új DEFCON szintje %d
OPER_DEFCON_WALL
	%s módosította DEFCON a szintet erre: %d
DEFCON_GLOBAL
	A Defcon szint most: %d

# Module szólánc
OPER_MODULE_LOADED
	%s modul betöltve
OPER_MODULE_UNLOADED
	%s modul kitöltve
OPER_MODULE_LOAD_FAIL
	Nem sikerült a betölteni a %s modult
OPER_MODULE_REMOVE_FAIL
	Nem sikerült eltávolítani a %s modult
OPER_MODULE_NO_UNLOAD
	This module can not be unloaded.
OPER_MODULE_LOAD_SYNTAX
	MODLOAD Fájlnév
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD Fájlnév
OPER_MODULE_LIST_HEADER
	Jelenlegi Modul lista:
OPER_MODULE_LIST
	Modul: %s [%s] [%s]
OPER_MODULE_LIST_FOOTER
	%d Modul betöltve.
OPER_MODULE_INFO_LIST
	Modul: %s Verzió: %s Szerzõ: %s Betöltve: %s
OPER_MODULE_CMD_LIST
	Modul parancsai: %R%s %s
OPER_MODULE_MSG_LIST
	IRCD kezelés: %s
OPER_MODULE_NO_LIST
	Jelenleg nincs modul betöltve.
OPER_MODULE_NO_INFO
	Nincs elérhetõ információ a  %s modulról
OPER_MODULE_INFO_SYNTAX
	MODINFO Fájlnév
MODULE_HELP_HEADER
	A következõ parancsokat a modulok tartalmazzák:

###########################################################
#
# Session korlátozás és kivétel üzenetek
#
###########################################################
	
# EXCEPTION válaszok
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [paraméterek]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+elévülés] maszk korlát indok
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {maszk | list}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE Szám helyzet
OPER_EXCEPTION_DISABLED
	Session korlátozás tiltva.
OPER_EXCEPTION_ALREADY_PRESENT
	Maszk %s már szerepel a kivétel listán.
OPER_EXCEPTION_TOO_MANY
	Session-korlát lista megtelt!
OPER_EXCEPTION_ADDED
	Session korlát %s részére %d-re állítva.
OPER_EXCEPTION_MOVED
	%s kivétel (#%d) áthelyezve a %d pozicíóba.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Nincs bejegyezve (#%d) session-korlát a kivétel listán.
OPER_EXCEPTION_NOT_FOUND
	%s nem található a session-korlát kivétel listán.
OPER_EXCEPTION_NO_MATCH
	Nincs ilyen bejegyzés a session-korlát kivétel listán.
OPER_EXCEPTION_DELETED
	%s törölve a session-korlát kivétel listáról.
OPER_EXCEPTION_DELETED_ONE
	Törölve 1 bejegyzés a session-korlát kivétel listáról.
OPER_EXCEPTION_DELETED_SEVERAL
	Törölve %d bejegyzés a session-korlát kivétel listáról.
OPER_EXCEPTION_LIST_HEADER
	Aktuális Session korlát kivétel lista:
# mask, korlát
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Szám  Korlát  Host
# maszk, set-by, set-idõ,lejárat, korlát, Indok
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (%s által, ekkor: %s; %s)
	    Korlát: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Érvénytelen session korlát. Érvényes számnak kell lennie
	és nagyobbnak vagy egyenlõnek mint zéró és kissebbnek 
	mint %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Érvénytelen hosztmaszk. Csak érvényes hosztmaszk lehet,
	a kivétel nem lehet nick vagy usernév.
OPER_EXCEPTION_EXISTS
	%s already exists on the EXCEPTION list.
OPER_EXCEPTION_CHANGED
	Exception for %s has been updated to %d.

# SESSION válaszok
OPER_SESSION_SYNTAX
	SESSION {LIST korlát | VIEW hoszt}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST korlát
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW hoszt
OPER_SESSION_DISABLED
	Session korlátozás tiltva.
OPER_SESSION_INVALID_THRESHOLD
	Érvénytelen küszöb érték. 1-nél nagyobb egész számnak
	kell lennie.
OPER_SESSION_NOT_FOUND
	%s nincs a session listán.
OPER_SESSION_LIST_HEADER
	Hosztok legalább %d sessionnel:
OPER_SESSION_LIST_COLHEAD
	Sessions  Host
# korlát, hoszt
OPER_SESSION_LIST_FORMAT
	%6d    %s
# hoszt, sessions, korlát
OPER_SESSION_VIEW_FORMAT
	A %s hoszt aktuálisan %d sessiont használ, a határértéke %d.

# EXCEPTION help
OPER_HELP_EXCEPTION
	Syntax: EXCEPTION ADD [+lejárat] maszkkorlátindok
	        EXCEPTION DEL {maszk| list}
	        EXCEPTION MOVE szám helyzet
	        EXCEPTION LIST [maszk | lista]
	        EXCEPTION VIEW [maszk | lista]
	
	Lehetõséget nyújt a szervíz adminnak, hogy karbantartsák
	azon hostok listáját, amelyeknek korlátozásuk van,
	engedélyezve bizonyos gépeket,úgymint a shell szervereket,
	hogy egyidõben felvigyenek klienseket alapértelmezettnél
	nagyobb számban. Amennyiben a gép eléri a korlátot, minden
	csatlakozni próbáló kliens killelve lesz.Mielõtt a kill
	megtörténne, figyelmeztetést kapnak %S /NOTICE üzeneten 
	keresztül. Az üzenet tartalma a konfigurációtól függ.
	
	EXCEPTION ADD hozzáadja a hostmaszkot a kivétellistához.
	Vedd figyelembe, hogy a nick!user@host és user@host
	maszkok érvénytelenek! Csak valós hostmaszkok, mint pl.
	box.host.dom és *.host.dom, megengedettek, a session
	korlátozásmaszkja nem vesz fel nicknevet/felhasználónevet.
	A korlátlegyen egy nullával egyenlõ vagy nagyobb szám.
	Ez határozza meg, hogy hány sessiont vihet magával a host
	egyidõben. A nulla azt jelenti, hogy a hostnak korlátlan
	számú sessionja lehet. Lásd az AKILL helpet a részletek
	felöl a formáról a szükséges lejárati parameterrõl.
	EXCEPTION DEL Törli a megadott maszkot a kivétel litáról	
	EXCEPTION MOVE módisítja a kivétel poziciószámát. A 
	kivételek fel-le mozgatja, az üres helyekre is.
	EXCEPTION LIST és EXCEPTION VIEW Mutatja az összes
	aktuális kivételt; ha megadsz egy tetszõleges maszkot,
	korlátozza azokat a kivételeket, melyekre ráillik a maszk.
	A különbség a két parancs között az, hogy az
	EXCEPTION VIEW sokkal részletesebb, megjeleníti a
	kivételt felrakó nevét a session korlátot, indokot, 
	hostmaszkot és a lejárati idõt,éa a kivétel legutolsó
	"használati" idejét.
	
	Vedd figyelembe, a csatlakozó kliens a ráillõ kivételek
	közül mindig az elsõt használja. Hatalmas kivétellisták
	és széleskörû kivételmaszkok csökkenthetik a Szervíz 
	teljesítményét.
	
	Korlátozva Szervíz adminnak.

OPER_HELP_SESSION
	Syntax: SESSION LIST threshold
	        SESSION VIEW host
	
	Megengedi a Szervíz adminoknak, hogy megtekintsék a 
	session listát.

	SESSION LIST  kilistázza azokat a hostokat, amelyeknek
	legalább határértéksessionjuk van. Érték legyen 1-nél
	nagyobb szám,hogy megelõzze az egy sessionúak kilistázását
	SESSION VIEWmegjeleníti a host részletes információit,
	tartalmazva a jelenlegi session számot, session korlátot.
	A host értéke nem tartalmazhat maszkkaraktereket.
	
	Lásd az EXCEPTION helpjét további információért a 
	session-korlátozással és azzal kapcsolatban,hogyan állíts
	be különleges korlátokat bizonyos hostokra és csoportokra.
	
	
	Korlátozva Szervíz adminoknak.

OPER_HELP_STAFF
	Syntax: STAFF

	Kijelzi az összes szervíz résztvevõ nickjét, szintjét és 
	on-line helyzetét.

OPER_HELP_DEFCON
	Syntax: DEFCON [1|2|3|4|5]

	A defcon rendszer képes használni elõre beállított
	korlátozást a szervíz használatban egy támadás alkalmával
	a hálózaton.

OPER_HELP_DEFCON_NO_NEW_CHANNELS
	* Nem lehet új csatornát regisztrálni.
OPER_HELP_DEFCON_NO_NEW_NICKS
	* Nem lehet új nicket regisztrálni.
OPER_HELP_DEFCON_NO_MLOCK_CHANGE
	* Nincs MLOCK módosítás
OPER_HELP_DEFCON_FORCE_CHAN_MODES
	* Beállítja a következõ módokat: (%s) az összes csatornán.
OPER_HELP_DEFCON_REDUCE_SESSION
	* Használva a csökkentett session korlát: %d.
OPER_HELP_DEFCON_NO_NEW_CLIENTS
	* Killelve lesz minden új felhasználó csatlakozásnál. 
OPER_HELP_DEFCON_OPER_ONLY
	* Mellõzve minden nem operátortól jövõ üzenet
OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* Csendes mellõzés, csak operátorok
OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* AKILL minden új kliensre csatlakozáskor.
OPER_HELP_DEFCON_NO_NEW_MEMOS
	* Nem lehet új memot küldeni
OPER_HELP_CHANKILL
	Syntax: CHANKILL [+lejárat] #szobaindok

	Akillre tesz minden nicket az adott csatornán. Ez a létezo
	és teljesen valós ident@host maszkot használja, minden 
	nick esetében végrehajtja az AKILLt.

###########################################################################
#
# Hír rendszer üzenetek
#
###########################################################################

NEWS_LOGON_TEXT
	[Fellépési Hír] - %s %s
NEWS_OPER_TEXT
	[Oper Hír] - %s %s
NEWS_RANDOM_TEXT
	[Véletlenszerû Hír] - %s %s

NEWS_LOGON_SYNTAX
	LOGONNEWS { ADD | DEL | LIST } [szöveg|szám]
NEWS_LOGON_LIST_HEADER
	Fellépési hírek:
NEWS_LOGON_LIST_ENTRY
	%5d (%s %s által)
	    %s
NEWS_LOGON_LIST_NONE
	Nincs fellépõ üzenet.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD szöveg
NEWS_LOGON_ADD_FULL
	A Hírlista tele!
NEWS_LOGON_ADDED
	Hozzáadva az új fellépési hír: (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {szám | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Logon news tétel #%d nem található!
NEWS_LOGON_DELETED
	Logon news tétel #%d törölve.
NEWS_LOGON_DEL_NONE
	Nincs törölhetõ fellépési hír!
NEWS_LOGON_DELETED_ALL
	Összes fellépési hír tétel törölve.

NEWS_OPER_SYNTAX
	Syntax: OPERNEWS { ADD | DEL |LIST } [ szöveg | szám ]
NEWS_OPER_LIST_HEADER
	Oper hírek listája:
NEWS_OPER_LIST_ENTRY
	%5d (%s %s által)
	    %s
NEWS_OPER_LIST_NONE
	Nincs oper news.
NEWS_OPER_ADD_SYNTAX
	Syntax: OPERNEWS ADD szöveg
NEWS_OPER_ADD_FULL
	Az Oper hírek listája tele van!
NEWS_OPER_ADDED
	Hozzáadva az új oper hír:(#%d).
NEWS_OPER_DEL_SYNTAX
	Syntax: OPERNEWS DEL {szám | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Oper news tétel #%d nem található!
NEWS_OPER_DELETED
	Oper news tétel #%d törölve.
NEWS_OPER_DEL_NONE
	Nincs törölhetõ opernews tétel!
NEWS_OPER_DELETED_ALL
	Összes oper hír törölve.

NEWS_RANDOM_SYNTAX
	RANDOMNEWS { ADD | DEL | LIST } [ szöveg | szám ]
NEWS_RANDOM_LIST_HEADER
	Véletlenszerû hírek listája:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s %s által)
	    %s
NEWS_RANDOM_LIST_NONE
	Nincs véletlenszerû hír.
NEWS_RANDOM_ADD_SYNTAX
	RANDOMNEWS ADD szöveg
NEWS_RANDOM_ADD_FULL
	News lista tele!
NEWS_RANDOM_ADDED
	Hozzáadva az új random news tétel (#%d).
NEWS_RANDOM_DEL_SYNTAX
	RANDOMNEWS DEL {szám | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Random news tétel #%d nem található!
NEWS_RANDOM_DELETED
	Random news tétel #%d törölve.
NEWS_RANDOM_DEL_NONE
	Nincs törölhetõ random news tétel!
NEWS_RANDOM_DELETED_ALL
	Összes random news tétel törölve.

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
	Syntax: RANDOMNEWS ADD szöveg
	        RANDOMNEWS DEL {sorszám | ALL}
	        RANDOMNEWS LIST
	
	Szerkeszti és listázza a random news üzeneteket. Amikor
	egy felhasználó csatlakozik a hálózathoz, egy (csak egy)
	random newst fog véletlenszerûen elküldeni részére.
	
	RANDOMNEWS may only be used by Services admins. 
	
###########################################################
#
# HelpSzerv segítség üzenetei
#
###########################################################

HELP_HELP
	%S egy szolgáltatás melynek a feladata, hogy eligazítást
	adjon a Szervízzel kapcsolatos információkban.
	A következõ témakörök érhetõk el más Services kliensekrõl:

	%R%s HELP
	     A nicknév regisztrációval kapcsolatos információkról.

	%R%s HELP
	     A szoba regisztrációval és karbantartással 
	     kapcsolatos információkról

	%R%s HELP
	     Üzenetek küldése offline (regisztrált) usereknek.

HELP_HELP_BOT

	%R%s HELP
	     Szervíz bot felállításához a csatornádra.

HELP_HELP_HOST

	%R%s HELP
	     Vhost igénylésrõl bõvebb információkért.
	
###########################################################
#
# NickServ segítség üzenetei
#
###########################################################
NICK_HELP_CMD_CONFIRM
	    CONFIRM    Confirm a nickserv auth code
NICK_HELP_CMD_RESEND
	    RESEND     Resend a nickserv auth code

NICK_HELP_CMD_REGISTER
	    REGISTER   Nicknév regisztráció
NICK_HELP_CMD_GROUP
	    GROUP      Csatlakozás egy csoporthoz
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY   Jelszavas azonosítás
NICK_HELP_CMD_ACCESS
	    ACCESS     A hozzáférési lista módosítása
NICK_HELP_CMD_SET
	    SET        Opciók beállítása, például kill védelem
NICK_HELP_CMD_SASET
	    SASET      Set SET-options on another nickname
NICK_HELP_CMD_DROP
	    DROP       Nicknév regisztrációjának törlése
NICK_HELP_CMD_RECOVER
	    RECOVER    Killeli a nickedet használó usert
NICK_HELP_CMD_RELEASE
	    RELEASE    Visszaadja a neved a RECOVER után
NICK_HELP_CMD_SENDPASS
	    SENDPASS   Elfelejtetted a jelszavad? Próbáld ezt
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
	    GETPASS    Kiirja a jeszavát a nicknévnek
	               (encryption esetén nem használható)
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Kiírja az ilyen e-mail címmel regisztrált
	               felhasználókat
NICK_HELP_CMD_FORBID
	    FORBID     Megelõzi, hogy a nicknév regisztrált legyen
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Suspend a given nick
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Unsuspend a given nick

NICK_HELP
	%S lehetõséget nyújt a "register" parancs használatával
	a nickneved regisztrálására, ezzel megelõzi, hogy más is
	használhassa a nicked. A NickServ parancsokkal
	írányítható. Az adott parancs szintaktikáját megkapod, ha
	beírod, hogy: %R%S parancs, lásd még a
	%R%S HELP parancs.
	
NICK_HELP_FOOTER
	Megjegyzés: Ez a szolgáltatás szeretné biztosítani az 
	IRC felhasználok számára hogy a kilétüket ne lehessen 
	kompromitálni. Egyáltalán NEM azt szolgálja, hogy 
	lehetõséget nyújtson nicknevek "lopására" vagy más
	erkölcstelen akciókra. A %S segítségével elkövetett
	visszaélés jutalma minimum az visszaélõ nickjének
	elvesztése.
	
NICK_HELP_EXPIRES
	Ha egy regisztrált nicknevet %d napig nem használnak, akkor
	a regisztrációja elévül, a nicknevet bárki regisztrálhatja,
	és használhatja.

NICK_HELP_REGISTER
	Syntax: REGISTER jelszó [email]
	
	Regisztrálja a nickneved a %S adatbázisába. Amint a nicked
	regisztráltad, használhatod a SET és ACCESS parancsokat,
	hogy konfiguráljad a nicked beállításait. Olyan jelszót 
	válassz, amire emlékezni fogsz, hogy használni tudd a neved
	és a beállításokat eszközölhesd. Késõbb szükséged lehet a
	nickeddel kapcsolatos beállítások végrehajtására.
	(Jegyzet: PROBLÉMÁT OKOZHAT FIDO, Fido, és fido
	mind különbözõ jelszavak!)

	Útikalaúz jelszó választáshoz:
	
	A jelszavad ne legyen könnyen kitalálható. Például az
	általad használt valósnevet megadni jelszónak rossz ötlet.
	A nickneved használni jelszónak még rosszabb ötlet ;) és
	a %S nem is engedi. A rövid jelszavak is rosszak a védelem
	szempontjából, ezért válassz legalább 5 karakter hosszút.
	Végezetül ne használhatsz szóközt a jelszóban.
	
	Aze-mail paraméter megadás lehetséges, megadásakor
	beállítja a nickedhez. Megadása a legtöbb hálózaton
	szükséges. Az e-mail címed nem kerül harmadik személy
	tudtára, bizalmasan kezeljük.
	
	Ez a parancs létrehoz egy új csoportot a nickeddel, ami
	lehetõvé teszi számodra, hogy másik nickeket regisztrálva
	megoszthasd a hozzáféréseidet és beállításaidat a nickjeid
	között. Bõvebb információkért írd be: %R%S HELP GROUP

NICK_HELP_GROUP
	Syntax: GROUP célnicknév jelszó
	
	A parancs belépteti a nickneved a célnicknév csoportjába.
	A jelszó a célnicknév jelszavának kell lennie.
	
	A csoportba való belépés lehetõvé teszi, hogy megoszd a
	konfigurációt, memo üzeneteket, szoba hozzáféréseket
	a nicknevekkel a csoportban, és sok más lehetõséget nyújt!
	 
	A csoport addig létezik, amig használatban van.
	Ez azt jelenti, ha egy nicknevet dropolnak a csoportból,
	nem fogod elveszteni a megosztott dolgokat, egészen addig,
	amig legalább egy nick van a csoportban.
	
	Akkor is használhatod a parancsot, ha még nem regisztrált
	a nicked. Ha a nicked regisztrált, akkor elõbb azonosítani
	kell a jelszavaddal, mielõtt ezt a parancsot használod.
	Írd be:%R%S HELP IDENTIFY bõvebb információért.
	
	Ajánlott, hogy inkább nem regisztrált névvel használd
	ezt a parancsot, mert ezzel automatikusan regisztrálódik.
	Ha regisztált nickrõl használod (hogy megváltoztasd a
	csoportod) csak akkor mûködik, ha a network adminok ezt
	engedélyezik.
	
	Egyszerre egy csoportban lehetsz. 
	
	Megjegyzés: minden nicknévnek a csoportban ugyanaz
			 a jelszava.

NICK_HELP_IDENTIFY
	Syntax: IDENTIFY jelszó
	
	Tudatja a %S-vel, hogy te vagy a tulajdonosa ennek a 
	nicknek. Jelszónak ugyanannak kell lennie amit a 
	regisztrációnál megadtál, vagy amire módosítottad.

NICK_HELP_UPDATE
	Syntax: UPDATE

	Frissíti a jelenlegi státuszod, vagyis ellenõrzi, hogy 
	jött-e új memo üzenet, beállítja a szükséges szoba 
	módokat, (ModeonID)és frissíti a vhostod, az userflagjeid
	(utolsó fellépés, stb.)
	
NICK_HELP_LOGOUT
	Syntax: LOGOUT
	
	Ezzel az IDENTIFY parancs hatásának fordítottját éred el.
	Nem fog a NickServ egy adott nick tulajdonosának tekinteni.
	
NICK_HELP_DROP
	Syntax: DROP [nicknév]
	
	Regisztráció törlése (dropolja) a nickneved a %S
	adatbázisából. A droppolás után a nicknév már szabadon
	hsználható bárki által, újra regisztrálható lesz.
	
	Úgy dropolhatsz nevet a csoportodból, ha megadod a 
	nicknév paramétert.

	Ezen parancs kiadása elõtt, azonositanod kell magad,
	mint a nick tulajdonosa, használva az IDENTIFY parancsot,
	írd be: %R%S HELP IDENTIFY bõvebb információért. 

NICK_HELP_ACCESS
	Syntax: ACCESS ADD maszk
	        ACCESS DEL maszk
	        ACCESS LIST
	
	Módosítja vagy kiirja a hozzáférési listát a nickedhez.
	Ez azon címeknek a listája, amelyek automatikusan úgy 
	tekintõdnek a %S-ben, hogy engedélyük van a nicket 
	használni. Ha a nicked más címekrõl akarod használni,
	akkor nem kell IDENTIFY parancsot küldened, hogy úgy
	legyél tekintve, a %S-ben, mint a nick tulajdonosa.
	
	Példák:
	
	    ACCESS ADD anyone@*.bepeg.com
	          Hozzáférést engedélyez az anyone identnek bármely
	          géprõl aminek a bepeg.com a domainje.
	
	    ACCESS DEL anyone@*.bepeg.com
	          Törli az elözõ parancsot.
	
	    ACCESS LIST
	          Megmutatja a hozzáférési listát.

NICK_HELP_SET
	Syntax: SET opció paraméterek
	
	Beállítható nicknév opciók. Az opció ezek egyike lehet:
	
	    DISPLAY    Set the display of your group in Services
	    PASSWORD   Set your nickname password
	    LANGUAGE   A szervíz nyelvezetének megválasztása
	    URL        Egy URL társítása a nicknevedhez
	    EMAIL      Egy E-mail cím társítása a nicknevedhez
	    ICQ        ICQ szám társítása a nicknevedhez
	    GREET      Köszöntõ üzenet társítása a nicknevedhez
	    KILL       A kill védelem be-,kikapcsolása
	    SECURE     A nickneved védelmének be-,kikapcsolása
	    PRIVATE    A nickneved %R%S LIST -ból való elrejtése
	    HIDE       A nicknévhez tartozó információk elrejtése
	    MSG        Megváltoztatja a szervízzel való kommunikációt
	    AUTOOP     Should services op you automatically.    
	    
	Ezen parancs kiadása elõtt, azonositanod kell magad, mint
	a nick tulajdonosa az IDENTIFY parancsot használva, írd
	be: %R%S HELP IDENTIFY bõvebb információért. 
	
	Írd be: %R%S HELP SET opció bõvebb információért.
	
	
NICK_HELP_SET_DISPLAY
	Syntax: SET DISPLAY új-megjelenítés
	
	Megváltoztatja a nicknév csoportod megjelenítését a 
	szervízben.	Az új megjelenítésnek a csoport egyik
	nickjének KELL lennie.

NICK_HELP_SET_PASSWORD
	Syntax: SET PASSWORD új-jelszó
	
	Megváltoztatja a nicknév tulajdonosának jelszavát.

NICK_HELP_SET_LANGUAGE
	Syntax: SET LANGUAGE sorszám

	Megváltoztatja a SERVICES nyelvezetét, amin az üzenteket
	írja.(Például: amikor válaszol egy parancsra, amit te 
	adtál ki). A sorszám alapján válaszd ki a nyelvezetet:
	
NICK_HELP_SET_URL
	Syntax: SET URL url
	
	Egy URL társítása a nicknevedhez. Ez az URL szerepelni fog
	az INFO-idban, amit bárki lekérhet az INFO paranccsal.

NICK_HELP_SET_EMAIL
	Syntax: SET EMAIL cím
	
	Egy E-mail cím társítása a nicknevedhez.
	Ez az E-mail cím szerepelni fog az INFO-id között,
	amit bárki lekérhet az INFO paranccsal.
	
NICK_HELP_SET_ICQ
	Syntax: SET ICQ szám
	
	Az ICQ  számod társítása a nicknevedhez.
	Ez a szám szerepelni fog az INFO-id között,
	amit bárki lekérhet az INFO paranccsal.
	
NICK_HELP_SET_GREET
	Syntax: SET GREET üzenet
	
	Ez az opció társít egy üzenetet a nicknevedhez,
	ami akkor fog megjelenni belépéskor, ha a csatornán
	a GREET opció engedélyezve van és te rendelkezel a
	megfelelõ szinttel, ami az üzenet megjelenitéséhez
	szükséges a csatornán.

NICK_HELP_SET_KILL
	Syntax: SET KILL {ON | QUICK | IMMED | OFF}
	
	Be/kikapcsolja az automatikus kill védelmet a nickedhez.
	A kill védelem bekapcsolásakor, ha más user megpróbálja
	használni a nicked, kap egy percet mialatt meg kell
	változtatnia a nickjét, ha letelt az idõ akkor a %S 
	megváltóztatja a nickjét pl: Vendég-000001-re.
	
	Ha a QUICK -et választod, a user csak 20 másodpercet kap
	a nickváltásra eltérõen az általános 60 másodperctõl.
	Ha az IMMED-et választod akkor azonnal megváltóztatja.
	Figyelmeztetés nélkül,nem kap esélyt a nickváltásra;
	kérlek ne használd ezt az opciót csak ha nagyon indokolt.
	A network adminisztrátorok letilthatják ezt.
	
NICK_HELP_SET_SECURE
	Syntax: SET SECURE {ON | OFF}
	
	Be/kikapcsolja a %S a biztonsági beállításokat a nickhez.
	A SECURE beállítással, be kell írnod a jelszavadat
	mielõtt a nicknév tulajdonsaként leszel tekintve,
	kivéve, ha a hosztod szerepel az access listán.
	Habár, ha az access listán szerepel a hosztod, akkor
	nem fog automatikusan nevet váltóztatni a %S, hacsak
	nincs bekapcsolva a KILL opció.

NICK_HELP_SET_PRIVATE
	Syntax: SET PRIVATE {ON | OFF}
	
	Be/kikapcsolja a %S a privát opciót a nicknevedhez.
	A PRIVATE beállítással, beállíthatod azt, hogy
	a nickneved ne szerepeljen a nicknév listában ami 
	lekérhetõ a %S LIST paranccsal.
	(Máskülönben, bárki kaphat információt a nicknevedrõl
	használva az INFO parancsot.)

NICK_HELP_SET_HIDE
	Syntax: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Lehetõséget nyújt arra, hogy megelõzd néhány adatnak a
	szerepelését a %S INFO parancs használatakor .
	Elrejtheted az E-mail címed (EMAIL), az utoljára használt
	user@host maszkod (USERMASK), és az utolsó kilépõ 
	üzeneted(QUIT).	A második paraméter meghatározza, hogy 
	az opció ki legyen kapcsolva(OFF)	vagy el legyen rejtve
	az információ(ON).

NICK_HELP_SET_MSG
	Syntax: SET MSG {ON | OFF}
	
	Lehetõséget nyújt arra, hogy megválaszd azt, hogy a szervíz
	hogyan kommunikáljon veled.
	Az MSG beállítással, a szervíz, privát üzeneteket fog
	használni, egyébként pedig noticeokat.

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
	Syntax: GHOST nickév [jelszó]
	
	Ezt a parancsot akkor használhatod, ha a nicked beragad, 
	befagy például lefagy a géped vagy a kapcsolatod.
	A beragadt nicked killelheted, ha ismered a jelszót .
	
NICK_HELP_INFO
	Syntax: INFO nicknév [ALL]
	
	Megmutatja a nicknévvel kapcsolatos információkat, mint a
	nick tulajdonosa, utoljára látott címe, regisztrálás ideje,
	a nick opciókat. Ha beazonosítasz egy nickre, akkor minden 
	informaciót megkaphatsz az ALL paraméter használatával 
	és  így látni fogod a rejtett információkat is.
	
NICK_HELP_LIST
	Syntax: LIST paraméter
	
	Listázza az összes regisztrált nicknevet, amely
	megegyezik a mintában kiadott nick!user@host formátummal.
	Azok a nevek, amelyeken fent van PRIVATE opció beállítás
	NEM fognak megjelenni a listában.
	
	Példák:
	
	    LIST joeuser@foo.com
	          Listáz minden regisztrált nicknevet, amelyet utoljára:
	           joeuser@foo.com címrõl használtak.
	
	    LIST *Bot*
	          Listáz minden nicket amelyben szerepelt a Bot
	
	    LIST *@*.bar.org
	          Listáz minden nicket, amit utoljára olyan user használt,
	          akinek a bar.org volt a domain-je.
	
NICK_HELP_ALIST
	Syntax: ALIST [szint]
	
	Listáz minden csatornát, ahol hozzáférésed van. 
	Opcionálisan megadhatod a szintet XOP vagy ACCESS
	formátumban is. Az eredmény csak azon szobákat fogja 
	megjeleniteni, ahol a megadott hozzáférési szinted van.
	
	Példák:
	    ALIST Founder
	          Listáz minden csatornát, ahol Founder hozzáférésed van.
	    ALIST AOP
	          Listáz minden csatornát, ahol AOP hozzáférésed
	          vagy magasabb szinted van.
	    ALIST 10
	          Listáz minden csatornát, ahol 10-es hozzáférésed
	          vagy magasabb szinted van.

	Azok a csatornák melyeken a NOEXPIREopció be van állítva,
	prefixel lesznek megjelölve a listában.

NICK_HELP_GLIST
	Syntax: GLIST
	
	Listáz minden nicket a csoportodból.

NICK_HELP_STATUS
	Syntax: STATUS nicknév...
	
	A következõket kapod, ha a nicknévre, amit a felhasználó 
	használ azonosította magát, mint tulajdonos. 
	Az eredmény formája a következõ:
	
		nicknév státusz-kód
	
	ahol a nicknévaz a nicknév amit küldtél a parancsban, és
	a státusz-kóda következõ lehet:
	
	    0 - Nincs ilyen user online vagy a nick nem regisztrált
	    1 - A felhasználó nem azonosított be, mint a nick tulajdonosa
	    2 - A felhasználó beazonosított, mint tulajdonos de csak a 
	        hozzáférési lista alapján
	    3 - A felhasználó beazonosított tulajdonosként a jelszavával
	        
	Tizenhat nicknév küldhetõ el parancsonként; a többit 
	figyelmen kívül hagyja. If no nickname is given, your status
	will be returned.

NICK_HELP_SENDPASS
	Syntax: SENDPASS nicknév
	
	Küld számodra egy E-mail üzentet, ami tartalmazza a 
	megadott nicknév jelszavát, arra az E-mail címre, amit
	a regisztrációnál vagy a set email paranccsal megadtál,
	vagy amire váltóztattad. Ez igen hasznos, ha elfelejted
	a jelszavadat.
	
	Korlátozni lehet IRC operátorokra egyes hálózatokon.
	
	Parancs nem elérhetõ, ha az encryption használatban van.

NICK_HELP_CONFIRM
	Syntax: CONFIRM passcodE
	
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
	Szervíz adminok is droppolhatnak nickeket, akkor ha
	beazonosíttotak a nicknevükre, és megnézhetik a nickek
	hozzáférési listáját. (%R%S ACCESS LIST nick)

NICK_SERVADMIN_HELP_LOGOUT
	Syntax: LOGOUT [nicknév [REVALIDATE]]
	
	Kilépési paraméter, megfordítja az IDENTIFY parancs
	hatását, azaz nem leszel beazonosítva a nickedre, mint a 
	valódi tulajdonosa a nickednek.
	
	A paraméter ugyanezt teszi a megadott nickkel.
	Ha megadod a REVALIDATE opciót, akkor a nicktõl
	a szerviz újraazonosítást fog kérni. 
	
	Használat korlátozva Szervíz adminoknak.

NICK_SERVADMIN_HELP_DROP
	Syntax: DROP [nicknév]
	
	Paraméter nélkül, droppolja a neved a %S adatbázisból.
	
	Paraméterrel dropolva a nicknév törlõdik az adatbázisból.
	Dropolhatsz egy nicket a csoportodban, kivéve, ha van 
	különleges kiváltsága.
	
	Használat korlátozva Szervíz adminoknak.
	
NICK_SERVADMIN_HELP_INFO
	
	Szervíz adminok használhatják az ALL paramétert.

NICK_SERVADMIN_HELP_LIST
	Syntax: LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
	
	Listáz minden regisztrált nicknevet, amely vagy nicknév
	vagy az utoljára látott cím (user@hostformában), amely
	azonos a megadott sablonnal. Nickek PRIVATE opcióval
	beállítva csak szervíz adminoknak jelennek meg. A nickek
	NOEXPIRE opcióval beállítva !prefixel jelennek meg
	a szervíz adminoknak.
	
	Ha a FORBIDDEN, SUSPENDED vagy NOEXPIRE opció meg van adva, csak azok
	a nickek amelyek vagy forbidoltak vagy NOEXPIRE flegjük van
	fognak  megjelenik. Ha mindkét opció meg van adva mindkét
	tipus megjelenik. Az opciók korlátozva Services adminoknak.
	
	Példák:
	
		LIST *!joeuser@foo.com
	            Kiírja a felhasználókat, akiknek a hosztja joeuser@foo.com
	
		LIST *Bot*!*@*
	            Kiírja az összes regisztrált nicket, melyek nevében 
	            szerepel a Bot (betû érzéketlen).
	
	    LIST * NOEXPIRE
	            Listázza az összes felhasználót, akinek a beállítása
	            nem elévülõ.

NICK_SERVADMIN_HELP_ALIST
	Syntax: ALIST [nicknév] [szint]
	
	Paraméter nélkül listázza azokat a csatornákat, ahol 
	hozzáférésed van.	Egy paraméterrel azokat a szobákat 
	listázza, ahol hozzáférése van a nicknnév-nek. Két 
	paraméterrel azokat a csatornákat listázza, ahol megadott
	szintû vagy magasabb a hozzáférése a nicknév-nek.  
	
	Ennek a használata korlátozva Szervíz adminra.
	
NICK_SERVADMIN_HELP_GLIST
	Syntax: GLIST [nicknév]
	
	Paraméter nélkül, listázza az összes nicknevet 
	a csoportodban.
	Paraméterrel listázza az összes nicknevet, abból a 
	csoportból ahol az adott nicknév van.	
	
	Ennek a használata korlátozva Szervíz adminra.

NICK_SERVADMIN_HELP_GETPASS
	Syntax: GETPASS nicknév
	
	Visszadja a megadott nicknév jelszavát. Jegyzet Ezt 
	a parancsot amaikor használod, üzenetet küld arról 
	hogy ki használta és kinek a jelszavát kérte le. 
	Naplózva lesz és WALLOPS/GLOBOPS üzenetben is megjelenik.
		
	Korlátozva Szervíz adminokra.
	
	A parancs nem elérhetõ, ha az encryption használatban van.
	
NICK_SERVADMIN_HELP_GETEMAIL
	Syntax: GETEMAIL user@emailhoszt

	Visszaadja a megadott emaillel regisztrált nickeket.
	Megjegyzés: használhatsz * karaktert mind a user,
	mind az emailhost esetében. A parancs kiadásakor a
	lekért email cím és az azt lekérõ személy naplózva lesz.

	Korlátozva Szervíz adminra.

NICK_SERVADMIN_HELP_FORBID
	Syntax: FORBID nicknév [indok]
	
	Nem engedélyezi, hogy bárki regisztrálja a nicknevet 
	Akkor törölhetõ, ha droppolod a nicket.
	
	Egyes hálózatok indokot igényelnek.
	
	Korlátozva Szervíz adminra.
	
###########################################################
#
# ChanServ help üzenetek
#
###########################################################
	
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
	    GETPASS    Megadja a founder jelszót a csatornán
CHAN_HELP_CMD_FORBID
	    FORBID     Megakadályozza a szoba használatát
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    Felfüggeszti a szoba használatát de
	               megõrzi az adatokat és beállításokat.
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Törli a suspendet a csatornáról.
CHAN_HELP_CMD_STATUS
	    STATUS     Kiírja a felhasználó aktuális hozzáférési
	               szintjét a megadott csatornán
CHAN_HELP_CMD_REGISTER
	    REGISTER   Szoba regisztrálása
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   Azonosíthatod magad a jelszavaddal
CHAN_HELP_CMD_SET
	    SET        Beállíthatod a csatornád opcióit
CHAN_HELP_CMD_AOP
	    AOP        AutoOperátor lista beállítása
CHAN_HELP_CMD_SOP
	    SOP        SzuperOperátor lista beállítása
CHAN_HELP_CMD_ACCESS
	    ACCESS     A szoba hozzáférési listájának beállítása
CHAN_HELP_CMD_LEVELS
	    LEVELS     Átállíthatod a szoba hozzáférési szintjeit
CHAN_HELP_CMD_AKICK
	    AKICK      AutoKirúgás listára teheted a "rossz" usereket
CHAN_HELP_CMD_DROP
	    DROP       Törölheted a csatornád regisztrációját
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Elveszett a jelszavad? Jelszó küldés.
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
	
	%S lehetõséget ad arra, hogy regisztráld a csatornádat.
	%S használatával elkerülhetõ a "take over" jelenség,
	amit az opjoggal visszaélõ userek okozhatnak. Még pedig
	úgy, hogy meghatározható az, hogy ki kapjon a csatornán
	operátori kiváltságokat. A parancsok használatához írd be:
	%R%S parancs, ha több információ kell a paranccsal
	kapcsolatban, akkor írd be: %R%S HELP parancs.
	 
	
CHAN_HELP_EXPIRES
	
	Megjegyzés: azok a csatornák, amelyeket nem használnak %d
	napig, automatikusan elvesztik regisztrációjukat.
	Automatikusan dropolásra kerülnek.
	
CHAN_HELP_REGISTER
	Syntax: REGISTER #szoba jelszó leírás
	
	A csatornádat regisztrálja a %S adatbázisába.
	Hogy kiadhasd ezt a paracsot operátornak kell lenned
	az adott csatornán.
	A jelszót használhatod az IDENTIFY parancs segitségével
	azonosításra, mielõtt beállításokat eszközölsz a 
	késöbbiekben az adott csatonán.
	
	Az utolsó paraméternek, tartalmaznia kell
	a szoba általános leírását.
	
	Amikor regisztrálsz egy csatornát, te leszel a "founder".
	szoba founderjének van lehetõsége a szoba beállításait,
	a hozzáférési listáját, az autókirúgás listát, az infókat
	megváltoztatni; a %S automatikusan megadja belépéskor a
	founder hozzáférési jogot és szoba operátori jogokat.
	Nézd meg az ACCESS parancsot (%R%S HELP ACCESS),
	hogy infomálódj a különbözõ szintû kiváltságok adásáról
	(a szoba többi felhasználója részére).
	
	Megjegyzés: Ha regisztrálsz egy csatornát, elõször 
	regisztrálnod kell a nickneved és azonosítanod kell
	magad. (NickServ identify jelszó)
	Ha még nem nézted meg, akkor lásd még: %R%s HELP.
	
CHAN_HELP_IDENTIFY
	Syntax: IDENTIFY #szoba jelszó
	
	%S a parancs beírása után founder szintet ad a csatornán.
	Több parancs kiadása elõtt szükséges, hogy használd ezt.
	A jelszónak egyeznie kell a megadott jelszóval.
	
CHAN_HELP_LOGOUT
	Syntax: LOGOUT #szoba nicknév
	
	Ez a parancs kijelentkezésre birja a nicket, vagyis újra
	kell azonosítania, hogy újra megkapja a hozzáférést.
	
	Ha te vagy a szoba founderje, akkor bárkit ki tudsz
	jelentkeztetni, vagy ki tudsz jelentkezni akár te is.

CHAN_HELP_DROP
	Syntax: DROP #szoba
	
	Törli a megnevezett szoba regisztrációját.
	Csak a szoba founder használhatja, akinek
	elõbb használnia kell az IDENTIFY parancsot.

CHAN_HELP_SET
	Syntax: SET #szoba opció paraméterek
	
	Lehetõséget ad a foundernek, hogy a beállításokat
	és információkat megváltoztassa.
	
	Elérhetõ opciók:
	
	    FOUNDER       A founder megváltoztatása
	    SUCCESSOR     A successor megadása
	    PASSWORD      A founder jelszó változtatása
	    DESC          A szobameghatározás állítás
	    URL           Egy URL társítása a szobához
	    EMAIL         Egy E-mail cím társítása a szobához
	    ENTRYMSG      A szoba belépo üzenetének 
	                  állítása
	    BANTYPE       A banolás tipusának beállítása
	    MLOCK         Szoba módok lezárása (+flag -flag)
	    KEEPTOPIC     Topic megtartása ha a szoba nincs használatban
	    OPNOTICE      Üzenetet küld az OP/VOICE parancsok használtáról
	    PEACE         Szabályozza a kritikus parancsok használatát
	    PRIVATE       A szoba elrejtése a listából
	    SECURE        Aktiválja a %S biztonsági lehetõségeit
	    SECUREOPS     A csatornán az op státusz szigorú kezelése
	    SECUREFOUNDER Csatornán a founder státusz szigorú kezelése
	    SIGNKICK      Sign kickek, melyek a KICK paranccsal történnek
	    TOPICLOCK     Témaváltás letiltása 
	    XOP           Xop (Sop, Aop, Hop, Vop) lista rendszer bekapcsolása
	
	Írd be: %R%S HELP SET opció bõvebb információért az
	adott opcióról.

CHAN_HELP_SET_FOUNDER
	Syntax: SET #szoba FOUNDER nick
	
	Megváltoztatja a szoba founderjét.
	Az új nicknév regisztrált kell hogy legyen!

CHAN_HELP_SET_SUCCESSOR
	Syntax: SET #szoba SUCCESSOR nick
	
	Megváltoztatja a successort a csatornán. Ha a founder neve
	elévül vagy törlõdik, de a szoba még regisztrált, akkor
	a successor lesz a szoba új founderje.
	Ha, a successornak már túl sok regisztrált csatornája van 
	(%d), a csatornának megszûnik a regisztrációja, akkor
	is ez történik, ha nem volt megadva successor.
	A nicknek regisztrálnak kell lennie.

CHAN_HELP_SET_PASSWORD
	Syntax: SET #szoba PASSWORD  jelszó
	
	Beállítja a founder jelszót a csatornára, amivel
	a founder azonosíthatja magát.

CHAN_HELP_SET_DESC
	Syntax: SET #szoba DESC leírás
	
	Beállítható a szoba általános meghatározása, ami látszódni
	fog a  LIST és INFO parancs használata esetén.

CHAN_HELP_SET_URL
	Syntax: SET #szoba URL [url]
	
	Egy URL társítása a szobához. Ha van honlapja a szobának,
	akkor bárki megtudhatja, ha lekérte a szobáról az INFO-t.
	Ha nincs megadva paraméter, akkor törli a meglévõ URL-t.

CHAN_HELP_SET_EMAIL
	Syntax: SET #szoba EMAIL [emailcím]
	
	Egy E-mail cím társítása a szobához. Ha van E-mail címe
	a szobának; akkor azt bárki megtudhatja, ha lekérte
	a szobáról az INFO-t. Ha nincs megadva paraméter akkor
	törli a meglévõ e-mail címet.

CHAN_HELP_SET_ENTRYMSG
	Syntax: SET #szoba ENTRYMSG [üzenet]
	
	Beállít egy üzenetet, amit /notice formában fognak
	a szobába belépõ userek megkapni. Ha nincs paraméter
	megadva akkor a szoba belépõ üzenete törlõdik.

CHAN_HELP_SET_BANTYPE
	Syntax: SET #szoba BANTYPE bantípus
	
	A banolás típusának beállítása, amit a Services akkor fog
	használni, ha valakit banolni kell a csatornáról.
	
	A bantípus egy szám 0 és 3 között a jelentése:
	
	0: *!user@host formátumú tiltás
	1: *!*user@host formátumú tiltás
	2: *!*@host formátumú tiltás
	3: *!*user@*.domain formátumú tiltás

CHAN_HELP_SET_KEEPTOPIC
	Syntax: SET #szoba KEEPTOPIC {ON | OFF}
	
	Engedélyezi vagy letiltja a topic megtartása opciót a
	szoba részére.  Ha a topic megtartása be van állítva,
	a topicot a %S megjegyzi miután az utolsó user elhagyja a
	szobát, és visszaállítja a következõ megnyitáskor.

CHAN_HELP_SET_TOPICLOCK
	Syntax: SET #szoba TOPICLOCK {ON | OFF}
	
	Engedélyezi vagy letiltja a topic lezárása opciót a 
	csatornán. Ha a topic lezárása be van kapcsolva,a %S nem
	engedélyezi a csatornán a topic változtatását a TOPIC
	paranccsal.
	
CHAN_HELP_SET_MLOCK
	Syntax: SET #szoba  MLOCK módok
	
	Beállíthatod a mode-lock opciót a csatornán. A  %S
	lehetõséget nyújt arra, hogy meghatározd azokat a módokat
	amelyek mindig be (vagy ki) lesznek kapcsolva. 
		
	A módok felrakhatóak a /MODE paranccsal is; 
	azok a módok amelyek a + jel után vannak lockolódnak,
	és azok a módok amelyek a - jel után vannak nem
	rakhatóak fel a szobára.
	
	Figyelmeztetés:  Ha beállítasz egy mode-lock alatti 
	kulcsot, mint azt a második példában látod, akkor be kell
	állítanod a RESTRICTED opciót a csatornán (lásd 
	HELP	SET RESTRICTED) vagy bárki beléphet a szobába,
	amikor üres és lehetõsége lesz meglátni a kulcsot!
	
	Példák:
	
	    SET #szoba MLOCK +nt-ikl
	         Az  "n" és a "t" állandóan fent van, és az "i", "k", és
	         "l" állandóan ki lesz kapcsolva. Minden más mód 
	         felrakása-levétele szabad.
	
	    SET #szoba MLOCK +knst-ilmp my-key
	         A "k", "n", "s", és "t" állandóan fent van, és az "i",
	         "l", "m", és a "p" ki lesz kapcsolva. A szobába való
	         belépéshez szküséges jelszó:	"my-key".
	
	    SET #szoba MLOCK +
	         Eltávolítja az összes lockolt módot.
	         Minden mód szabadon ki/bekapcsolható.
	
CHAN_HELP_SET_PEACE
	Syntax: SET #szoba PEACE {ON | OFF}
	
	Engedélyezi vagy letiltja a peace opciót.
	Amikor a peace be van állítva, akkor a userek nem,
	rúghatnak ki, tilthatnak ki olyan usert akinek az övékkel
	egyenlõ vagy magasabb szintje van a %S parancsokhoz.
	
CHAN_HELP_SET_PRIVATE
	Syntax: SET #szoba PRIVATE {ON | OFF}
	
	Engedélyezi vagy letiltja a private opciót a csatornán.
	Amikor a private be van kapcsolva, a %R%S LIST 
	parancs által kilistázott csatornák között nem szerepel 
	a szoba.

CHAN_HELP_SET_RESTRICTED
	Syntax: SET #szoba RESTRICTED {ON | OFF}
	
	Enables or disables the restricted access option for a
	channel.  When restricted access is set, users not on
	the access list will instead be kicked and banned from the channel.
	
CHAN_HELP_SET_SECURE
	Syntax: SET #szoba SECURE {ON | OFF}
	
	Engedélyezi vagy letitlja a %S biztonsági lehetõségeit
	a szobában. Ha SECURE mûködik, csak azok a felhasználók
	akik regisztráltak és beazonosítottak a %s-be a	
	jelszavukkal (használván az IDENTIFY parancsot) kaphatnak
	a szobában jogokat, ha szerepelnek a hozzáférési listán.
	
CHAN_HELP_SET_SECUREOPS
	Syntax: SET #szoba SECUREOPS {ON | OFF}
	
	Bekapcsolja vagy letiltja a secure ops opciót.
	Amikor a secure ops be van kapcsolva, akkor azok, akik 
	nem szerepelnek a hozzáférési listán nem kaphatnak szoba
	operátori státuszt.
	
CHAN_HELP_SET_SECUREFOUNDER
	Syntax: SET #szoba SECUREFOUNDER {ON | OFF}
	
	Engedélyezi vagy letiltja a secure founder opciót a 
	csatornán. Amikor a secure founder be van állítva, csak
	a valódi founder tudja dropolni a csatornát, átírni a 
	szoba jelszavát, founderét, jogutódját, de azok nem,
	akik csak az IDENTIFY parancsot használták a %S-ben.
	
CHAN_HELP_SET_SIGNKICK
	Syntax: SET #szoba SIGNKICK {ON | SZINT | OFF}
	
	Engedélyezi vagy letiltja a signed kick opciót a csatornán
	Amikor a SIGNKICK be van állítva, a %S KICK parancsával
	végrehajtott kirúgásokban az indokban szerepel az op neve.
	
	Ha használod a LEVEL-t, akkor azok, akiknek a szintje
	megegyezik vagy magasabb, mint a signkick szintje, azoknak
	a neve nem fog szerepelni a kick indokjában.
	Lásd még: %R%S HELP LEVELS bõvebb információkért. 
	
CHAN_HELP_SET_XOP
	Syntax: SET #szoba XOP {ON | OFF}
	
	Engedélyezi vagy letiltja az xOP lista rendszert.
	Ha az XOP be van kapcsolva, akkor AOP/SOP/VOP
	parancsokkal osztogathatod a csatornán a privilégiumokat
	a usereknek, máskülönben az ACCESS parancsot használod.
	
	Az xOP lista rendszerrõl az access lista rendszerre való 
	áttérés nem okoz gondot.
	
CHAN_HELP_SET_OPNOTICE
	Syntax: SET #szoba OPNOTICE {ON | OFF}
	
	Be/kikapcsolja az opnotice opciót a csatornán.
	Amikor az op-notice be van kapcsolva, a %S megjegyzést
	fog küldeni a csatornára, amikor a felhasználók használják
	az OP, VOICE, vagy a hozzájuk kapcsolódó parancsokat a
	szoban.
	
CHAN_HELP_AOP
	Syntax: AOP #szoba ADD nick
	        AOP #szoba  DEL {nick  |  sorszám  |  lista }
	        AOP #szoba LIST [ maszk  |  lista ]
	        AOP #szoba CLEAR
	
	Karbantartja az AOP(AutoOP) listát a csatornán. Az AOP 
	listán szereplõ userek automatikusan megkapják belépéskor 
	az operátor státuszt, unbanhatják magukat, meghívhatják 
	magukat a csatornára,megjelenithetik a köszöntõ üzenetüket
	stb.
	
	Az AOP ADD paranccsal hozzáadhatsz egy nickenevet a 
	csatornád AOP listájához.
	
	Az AOP DEL paranccsal eltávolíthatsz egy megadott 
	nicknevet a csatornád AOP listájáról. Ha sorszámokat 
	intervallumban adunk meg (lásd a listázásnál), akkor a 
	megadott bejegyzések kerülnek törlésre.
	
	Az  AOP LIST parancs megadja az AOP listát.
	Ha nem vagyunk kiváncsiak a teljes listára akkor a 
	listában szereplõ Aop sorszámát megadva is megtudhatjuk a
	nevét, megadhatunk intervallumot is.
	
	Példák:
	
		AOP #szoba LIST 2-5,7-9
			Listázza az AOP-okat 2 és 5 között és
			7-tol  9-ig.
	      
	Az AOP CLEAR parancs kiüriti a szoba AOP listáját.
	
	Az AOP ADD és AOP DEL parancsokat csak SOP vagy 
	magasabb szinttel rendelkezo userek használhatják, de az
	AOP CLEAR parancsot csak a szoba foundere adhatja ki.
	Habár, minden AOP listán szereplo user használhatja az
	AOP LIST parancsot.
	
	Ezt a parancsot letilthatod a csatornádon, és helyette 
	használhatod az access lista rendszert. Nézd meg: %R
	%S HELP ACCESSbõvebb információért az access listáról, és
	írd be: %R%S HELP SET XOP hogy megnézd, hogyan tudsz 
	váltani az xOP rendszerrol az access lista rendszerre.
	
CHAN_HELP_HOP
	Syntax: HOP  #szoba ADD nick
	        HOP #szoba DEL { nick  |  sorszám  |  lista}
	        HOP #szoba LIST [ maszk | lista ]
	        HOP #szoba CLEAR
	
	Karbantartja az HOP (HALFOP) listát a csatornán. A HOP
	listán szereplõ userek automatikusan megkapják belépéskor
	a féloperátor státuszt.
	
	HOP ADD paranccsal hozzáadhatsz egy nickenevet csatornád
	HOP listájához.
	
	HOP DEL paranccsal eltávolíthatsz egy megadott nicknevet
	a csatornád HOP listájáról. Ha sorszámokat intervallumban
	adunk meg (lásd a listázásnál),akkor a megadott bejegyzések
	kerülnek törlésre.
	
	Az  HOP LIST parancs megadja az HOP listát. Ha nem vagy 
	kiváncsi a teljes listára akkor a listában szereplõ HOP 
	sorszámát megadva is megtudhatjuk a nevét, megadhatunk 
	intervallumot is.
	
	Példák:
	
		HOP #szoba LIST 2-5,7-9
		Listázza az HOP-okat 2 és 5 között és
		7-tol  9-ig.
	      
	Az HOP CLEAR parancs kiüriti a szoba HOP listáját.
	
	Az HOP ADD és HOP DEL parancsokat csak AOP / magasabb
	szinttel rendelkezõ userek használhatják,de az HOP CLEAR
	parancsot csak a szoba foundere adhatja ki.
	Habár, minden HOP listán szereplo user használhatja az
	HOP LIST parancsot.
	
	Ezt a parancsot letilthatod a csatornádon, és helyette 
	használhatod az access lista rendszert. Nézd meg: %R
	%S HELP ACCESS bõvebb információért az access listáról,
	és írd be: %R%S HELP SET XOP hogy megnézd, hogyan 
	tudsz váltani az xOP rendszerrõl access lista rendszerre.
	
CHAN_HELP_SOP
	Syntax: SOP #szoba ADD nick
	        SOP #szoba DEL { nick  |  sorszám  |  lista }
	        SOP #szoba LIST [ maszk | lista ]
	        SOP #szoba CLEAR
	
	Karbantartja az SOP(SuperOP)listát a csatornán. Az SOP
	listán szereplo userek automatikusan megkapják ugyanazokat
	amiket az AOP listán szereplok,és emellett lehetõségük van
	AKICK listát módosítani, szoba memo üzeneteket olvasni 
	és küldeni, BadWords listát szerkeszteni, stb.
	
	Az SOP ADD paranccsal hozzáadhatsz egy nickenevet a 
	csatornád SOP listájához.
	
	Az SOP DEL paranccsal eltávolíthatsz egy megadott 
	nicknevet a csatornád SOP listájáról. Ha sorszámokat 
	intervallumban adunk meg (lásd a listázásnál), akkor a
	megadott bejegyzések kerülnek törlésre.
	
	Az  SOP LIST parancs megadja az SOP listát. Ha nem 
	vagy kiváncsi a teljes listára akkor a listában szereplõ
	SOP sorszámát megadva is megtudhatjuk a nevét, megadhatunk
	intervallumot is.
	
	Példák:
	
		SOP #szoba LIST 2-5,7-9
			Listázza az SOP-okat 2 és 5 között és
			7-tõl  9-ig.
	
	Az SOP CLEAR parancs kiüriti a szoba SOP listáját.
	
	Az SOP ADD, SOP DEL és SOP CLEAR parancsokat csak a
	founder szintu userek használhatják. Habár, minden SOP 
	listán szereplo user használhatja az SOP LIST parancsot.


	
	Ezt a parancsot letilthatod a csatornádon, és helyette 
	használhatod az access lista rendszert. Nézd meg: %R
	%S HELP ACCESS bõvebb információért az access listáról,
	és írd be: %R%S HELP SET XOP hogy megnézd, hogyan 
	tudsz váltani az xOP rendszerrõl access lista rendszerre.
	
CHAN_HELP_VOP
	Syntax: VOP #szoba ADD nick
	        VOP #szoba DEL {nick | sorszám| lista}
	        VOP #szoba LIST [maszk | lista]
	        VOP #szoba CLEAR
	
	Karbantartja az VOP (VoiceOP) listát a csatornán. VOP 
	listán szereplõ userek belépéskor automatikusan hangadói 
	jogot (+v) kapnak, használhatják a voiceme parancsot. 
	
	Az VOP ADD paranccsal hozzáadhatsz egy nickenevet a 
	csatornád VOP listájához.
	
	A VOP DEL paranccsal eltávolíthatsz egy nicknevet a 
	csatornád VOP listájáról. Ha sorszámokat intervallumban
	adunk meg (lásd a listázásnál), akkor a megadott 
	bejegyzések kerülnek törlésre.
	
	Az  VOP LIST parancs megadja az VOP listát. Ha nem 
	vagyunk kiváncsiak a teljes listára akkor a listában
	szereplõ VOP sorszámát megadva is megtudhatjuk a nevét,
	megadhatunk intervallumot is.
	
	Példák:
	
		VOP #szoba LIST 2-5,7-9
			Listázza az VOP-okat 2 és 5 között és
			7-tõl  9-ig.
	      
	Az VOP CLEAR parancs kiüriti a szoba VOP listáját.
	
	Az VOP ADD, VOP DEL és VOP LIST parancsokat csak az
	AOP vagy magasabb szintu userek használhatják.
	A VOP CLEAR parancsot csak a founder használhatja.
	
	Ezt a parancsot letilthatod a csatornádon, és helyette 
	használhatod az access listát. Nézd meg: %R%S HELP 
	ACCESS bõvebb információért az access listáról, és írd
	be: %R%S HELP SET XOP hogy megnézd, hogyan tudsz 
	váltani az xOP rendszerrol az access lista rendszerre.
	
CHAN_HELP_ACCESS
	Syntax:	ACCESS #szoba ADD nick szint
			ACCESS #szoba DEL {nick | sorszám | lista}
			ACCESS #szoba LIST [maszk | lista]
			ACCESS #szoba CLEAR
	
	Access lista a csatornán. A hozzáférési lista (access)
	meghatározza, hogy mely userek kaphatnak op státuszt vagy
	hozzáférést a %S parancsokhoz a csatornán. Különbözõ
	user szintek lehetõséget adnak arra, hogy eltérõ jogokat 
	kaphassanak; információért %R%S HELP ACCESS LEVELS.
	A nick, amely nem szerepel az access listán 0-ás szintû.
	
	Az ACCESS ADD paranccsal hozzáadható a nicknév az
	access listához a megadott szinttel; ha a nick szerepel
	a listán,akkor a szintje változik csak meg. A szintamit
	meghatározol, kisebb kell, hogy legyen a te szintednél és
	ha a nick már szerepel a listán, akkor a jelenlegi 
	szintjánek a tiédnél alacsonyabbnak kell lennie.
	
	Az ACCESS DEL törli a nicknevet az access listáról.
	Ha intervallumot adsz meg, akkor minden benne szereplõ
	nicknevet törölsz.
	
	Az ACCESS LIST paranccsal megtekintheto az access lista.
	Ha intervallumot is megadsz, akkor abban az intervallumban
	szereplõ nicknevek listázása fog megtörténni.
	
	ACCESS #szoba LIST 2-5,7-9
		Listázza azokat a usereket, akik 2 és 5 között illetve
		7-tõl 9-ig szerepelnek a listán.
	
	Az ACCESS CLEAR parancs töröl minden bejegyzés a szoba
	hozzáférési listájáról.
	
CHAN_HELP_ACCESS_LEVELS
	Felhasználói hozzáférési szintek
	
	Alapból, a következõ hozzáférési szintek érvényesek:
	   
	   Founder   Teljes hozzáférés a %S funkciókhoz; automatikus
	                     opjog a csatornára való belépéskor.
	                     Megjegyzés, csak egy embernek lehet founder státusza
	                     (nem adható az ACCESS paranccsal).
	        10   Hozzáférés az AKICK parancshoz; automatikus op.
	         5   Automatikus op státusz.
	         3   Automatikus voice státusz.
	         0   Nincs különleges privilégiuma; opolható más
	                     opok által(ha a secure-ops ki van kapcsolva).
	        <0   Nem opolható.
	
	Ezek a szintek változhatnak, vagy újakat adhatsz hozzá, a
	LEVELS paranccsal; írd be: %R%S HELP LEVELS bõvebb
	információért.
	
CHAN_HELP_AKICK
	Syntax: AKICK #szoba ADD {nick | maszk} [indok]
	        AKICK #szoba STICK maszk
	        AKICK #szoba UNSTICK maszk
	        AKICK #szoba DEL {nick | maszk | entry-num | list}
	        AKICK #szoba LIST [maszk | entry-num | list]
	        AKICK #szoba VIEW [maszk | entry-num | list]
	        AKICK #szoba ENFORCE
	        AKICK #szoba CLEAR
	
	Karbantarthatod a csatornához tartozó autokirúgás listát
	Ha egy olyan felhasználó, akinek a maszkja megegyezik egy 
	az akick listán szerepelõ maszkkal megprobál belépni a 
	csatornára,	akkor a %S kitiltja és aztán kirúgja.
		
	Az AKICK ADD parancs hozzáadja a megadott nicket
	vagy usermaszkot az AutoKick listához. Ha megadsz 
	indokot is a parancsban, akkor ezzel az indokkal
	fogja kirúgni a ChanServ az illetõt, ha nincs 
	megadva, akkor az alapbeállítás szerinti indok:
	"You have been banned from the channel".
	
	Az AKICK STICK paranccsal véglegesen kitiltod az
	adott maszkot a csatornáról. Ha valaki megpróbálja 
	levenni a bant, a %S automatikusan visszateszi.
	Ezt nem használhatod regisztrált nevekre.
	
	Az AKICK UNSTICK parancs törli a hatását az
	AKICK STICK parancsnak, tehát ismét lehetõséged
	lesz levenni a tiltást a csatornádról.
	
	Az AKICK DEL parancs eltávolítja a megadott maszkot az
	autokirúgás listáról.
	Ez nem müködik, hacsak nem távolítunk el minden az adott
	nickre, identre, hostra szoló tiltást, ami az autókirugás
	listán szerepel; mindegyiket egyenként kell törölni.
	
	Az AKICK LIST parancs megjeleníti az autókirúgás listát,
	vagy optionális esetben csak azokat jeleniti meg, amelyek
	egyeznek a megadott maszkkal.
	
	Az AKICK VIEW ugyanaz, mint a AKICK LIST, de több
	részletet jelenít meg.
	
	AKICK ENFORCE hatására a %S ellenörzi az akick listát
	és eltávolít minden usert, aki szerepel a listán.
	
	AKICK CLEAR paranccsal az összes akick lista bejegyzés 
	törölhetõ.
	
CHAN_HELP_LEVELS
	Syntax:	LEVELS #szoba SET típus szint
			LEVELS #szoba {DIS | DISABLE} típus
			LEVELS #szoba LIST
			LEVELS #szoba RESET
	
	A LEVELS segítségével a számokkal definiált hozzáférési
	szintek módosíthatók.
	Ezzel a paranccsal,  definiálhatsz hozzáférési szinteket, 
	melyek szükségesek a  %S funkcióihoz.
	Ezt a parancsot csak a founder használhatja.
	
	LEVELS SET biztosítja, hogy megváltoztasd a hozzáférési
	szintjét egy adott funkciónak vagy funkciók csoportjának.
	
	A LEVELS DISABLE (vagy DIS röviden) letiltja
	az automatikus lehetõségeket, vagy megtiltja a hozzáférést
	bizonyos funkciókhoz, ezeket csak a founder változtathatja
	
	LEVELS LIST kiírja a jelenlegi hozzáférési szintjeit
	a különbözõ funkcióknak.
	
	LEVELS RESET visszaállítja az alapértelmezett értékeket.
	Lásd még: HELP ACCESS LEVELS.
	
	Ha szeretnéd megnézni a funkciók és lehetõségek listáját.
	akkor lesd meg a HELP LEVELS DESC leírást is.
	
CHAN_HELP_LEVELS_DESC
	A következõ lehetõségek/funkciók elérthetõek.
	Megjegyzés: az AUTODEOP és NOJOIN esetén maximum szintrõl
	az összes többi esetén minimum szintrõl van szó.
	
CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s
	
CHAN_HELP_INFO
	Syntax: INFO #szoba [ALL]
	
	Listázza az összes információt a megnevezett regisztrált
	csatornáról, pl a foundert, a regisztrálás idõpontját,
	az utolsó használat idejét, a leírást, az mlock-ot.  
	Ha beazonosítottad magad a szoba founderjének, akkor
	teljes információt az ALL paraméterrel kaphatsz, az 
	entrymessage, a successor és a rejtett infók is látszódnak
	
	Alapból az ALL opció csak a foundernek engedélyezett.

CHAN_HELP_LIST
	Syntax: LIST pattern
	
	Listázza a regisztrált csatornákat.
	(Ahol a PRIVATE opció be van kapcsolva nem fog
	 megjelenni a listában.)

CHAN_HELP_OP
	Syntax: OP [#szoba [nick]]
	
	Opolja a megadott nicket a megadott csatornán.
	Ha nem adsz meg nicket, magadat opolod.
	Ha nem adsz meg csatornát és nicket se, akkor minden
	csatornán (ahol bent vagy) opolod magad,
	ahol meg van hozzá a jogod.
	
	Alap beállítás szerint csak a minimum AOP listán
	vagy 5-ös accessen szereplo userek használhatják.
	
CHAN_HELP_DEOP
	Syntax: DEOP [#szoba [nick]]
	
	Deopolja a megadott nicket a megadott csatornán.
	Ha nem adsz meg nicket, magadat deopolod.
	Ha nem adsz meg csatornát és nicket se, akkor minden
	csatornán (ahol bent vagy) deopolod magad,
	ahol meg van hozzá a jogod.
	
	Alap beállítás szerint csak a minimum AOP listán
	vagy 5-ös access szinten szereplo userek használhatják.
	
CHAN_HELP_VOICE
	Syntax: VOICE [#szoba [nick]]
	
	Voiceolja a megadott nicket a megadott csatornán.
	Ha nem adsz meg nicket, magadat voiceolod.
	Ha nem adsz meg csatornát és nicket se, akkor minden
	csatornán (ahol bent vagy) voiceolod magad, ahol megvan
	hozzá a jogod.
	
	Alap beállítás szerint csak a minimum AOP listán
	vagy 5-ös accessen szereplo userek használhatják vagy min.
	a VOP listán vagy 3-as access szinten szereplo userek
	használhatják saját maguk voiceolására.
	
CHAN_HELP_DEVOICE
	Syntax: DEVOICE [#szoba [nick]]
	
	Elveszi a voiceot a megadott nicktõl a megadott csatornán.
	Ha nem adsz meg nicket, magadatól veszed el.
	Ha nem adsz meg csatornát és nicket se, akkor minden
	csatornán (ahol bent vagy) elveszed magadtól, ahol
	meg van hozzá a jogod.
	
	Alap beállítás szerint csak a minimum AOP listán vagy
	5-ös accessen szereplõ userek használhatják vagy min.
	a VOP listán vagy 3-as access szinten szereplo userek
	használhatják saját maguktól elvenni voiceot.
	
CHAN_HELP_HALFOP
	Syntax: HALFOP [#szoba [nick]]
	
	Félopot ad a megadott nicknek a megadott csatornán.
	Ha nem adsz meg nicket, magadnak adsz.
	Ha nem adsz meg csatornát és nicket se, akkor minden
	csatornán (ahol bent vagy) magadnak adsz félopot, ahol
	meg van hozzá a jogod.
	
	Alap beállítás szerint csak a minimum AOP listán vagy
	5-ös accessen szereplo userek használhatják vagy min.
	a HOP listán vagy 4-es access szinten szereplõ userek
	használhatják saját maguknak félop adására.
	
CHAN_HELP_DEHALFOP
	Syntax: DEHALFOP [#szoba [nick]]
	
	Elveszi a félopot a megadott nicktõl a megadott csatornán.
	Ha nem adsz meg nicket, magadatól veszed el.
	Ha nem adsz meg csatornát és nicket se, akkor minden
	csatornán (ahol bent vagy) elveszed magadtól a félopot,
	ahol meg van hozzá a jogod.
	
	Alap beállítás szerint csak a minimum AOP listán vagy
	5-ös accessen szereplo userek használhatják vagy min.
	a HOP listán vagy 4-es access szinten szereplõ userek
	használhatják, hogy saját maguktól elvegyék a félopot
	
CHAN_HELP_PROTECT
	Syntax: PROTECT [#szoba [nick]]
	
	Protectet ad a megadott nicknek a megadott csatornán.
	Ha nem adsz meg nicket, magadat protecteled.
	Ha nem adsz meg csatornát és nicket se, akkor minden
	csatornán (ahol bent vagy) protectet adsz magadnak,
	ahol meg van hozzá a jogod.
	
	Alap beállítás szerint csak a founder tudja használni,
	vagy minimum a SOP listán vagy 10-es access szinten
	szereplõ userek használhatják saját maguk protectolására.

CHAN_HELP_DEPROTECT
	Syntax: DEPROTECT [#szoba [nick]]
	
	Elveszi a protectet a nicktõl a megadott csatornán.
	Ha nem adsz meg nicket, magadat deprotecteled.
	Ha nem adsz meg csatornát és nicket se, akkor minden
	csatornán (ahol bent vagy) elveszed a protectet magadtól,
	ahol meg van hozzá a jogod.
	
	Alap beállítás szerint csak a founder tudja használni,
	vagy min. az SOP listán vagy 10-es access szinten
	szereplõ userek használhatják maguk deprotectolására.
	
CHAN_HELP_OWNER
	Syntax: OWNER [#szoba]
	
	Tulajdonosi státuszt ad a csatornán. Ha a szoba nincs
	megadva, akkor minden csatornán, ahol bent vagy ownert 
	kapsz, ahol meg van ehhez van jogod is.
	
	Csak founder szintû user használhatja.
	
CHAN_HELP_DEOWNER
	Syntax: DEOWNER [#szoba]
	
	Elveszi a tulajdonosi státuszt a csatornán.Ha a szoba
	nincs megadva, akkor mindenhol, ahol bent vagy elveszed
	magadtól az ownert, ahol meg van ehhez van jogod is.
	
	Csak founder szintu user használhatja.
	
CHAN_HELP_INVITE
	Syntax: INVITE #szoba
		
	Utasítja a %S-et, hogy hívjon meg a megadott csatornára.
	
	Alapbeállítás szerint, min. AOP vagy 5-ös access szinttel
	kell rendelkezned a csatornán, hogy használhasd.
	
CHAN_HELP_UNBAN
	Syntax: UNBAN #szoba
	
	A %S leveszi rólad a kitiltást és szabadon
	beléphetsz a csatonára.
	
	Alapbeállítás szerint, min. AOP vagy 5-ös access szinttel
	kell rendelkezned, hogy használhasd.	
	
CHAN_HELP_KICK
	Syntax: KICK [#szoba [nick [indok]]]
	
	A ChanServ kirúgja a megadott nicknevet a csatornáról.
	Ha nem adsz meg nicket, akkor téged fog kirúgni.
	Ha nem adsz meg szobát és nicket sem, akkor téged
	fog minden csatornáról, ahol bent vagy kirúgni, ha
	meg van a parancs használatához a szükséges szinted.
	
	Alapbeállítás szerint, min. AOP vagy 5-ös access szinttel
	kell rendelkezned a csatornán, hogy használhasd.
	
CHAN_HELP_BAN
	Syntax: BAN [#szoba [nick [indok]]]
	
	Kitilt egy kiválaszott nicket a csatornáról.
	Ha nem adsz meg nevet akkor téged tilt ki, ha
	nem adsz meg csatit és nicket, akkor minden csatornáról
	ahol bent vagy és van jogod rá, onnan kitilt.
	
	Alapbeállítás szerint, minimum AOP vagy 5-ös access 
	szinttel kell rendelkezned a csatornán, hogy használhasd.
	
CHAN_HELP_TOPIC
	Syntax: TOPIC #szoba [téma]
	
	A %S megváltoztatja a szoba témáját arra amit
	megadtál. Ha a téma nincs megadva, akkor egy üres
	topicot állít be. Ez a parancs hasznos, akkor ha a 
	SET TOPICLOCK be van állítva. Használd a %R%S HELP
	SET TOPICLOCK parancsot bõvebb információért.
	
	Alapból a szoba founder szintû userei használhatják
	csak a csatornán. 
	
CHAN_HELP_CLEAR
	Syntax: CLEAR #szoba mit?
	
	A %S eltávolítja a beállításokat a csatornán. 
	A következõ opciók lehetségesek:
	
	    MODES    Törli az összes módot ami nem szerepel a szoba
	             mode lockjában.
	    BANS     Törli az összes tiltást a csatornán.
	    EXCEPTS  Törli az összes kivételt a csatornán.
	    INVITES  Clears all invites on the channel.
	    OPS      Elvesz minden opot a csatornán
	    HOPS     Eltávolít minden félop csatorna státuszt (+h mód)
	             az összes csatornán lévõ féloperátortól, ha támogatott.
	    VOICES   Elvesz minden "voice"-t (mode +v) a csatornán.
	    USERS    Eltávolít minden usert a csatornáról (kirúgás).
		
	Alapbeállításban, a founder szintû userek használhatják.
	
CHAN_HELP_GETKEY
	Syntax: GETKEY #szoba 
	
	Megadja a szoba kulcsot. Ez a parancs kimondottan hasznos
	botok és/vagy scriptek számára, tehát a kimeneti rész
	a következõképpen néz ki:
	
	KEY <szoba> <kulcs>
	
	A kulcs "NO KEY", ha nincs kulcs beállítva.
	
CHAN_HELP_SENDPASS
	Syntax: SENDPASS #szoba
	
	Elküldi E-mailben a csatornádra beállított jelszót. Neked
	a szoba founderjének kell lenned, hogy használhasd ezt
	a parancsot és elõször azonosítanod kell magadat.
	
	Lehet korlátozni IRC operatorok részére a hálózaton.
	
	A parancs nem használható ha az encryption engedélyezett.
	
CHAN_SERVADMIN_HELP
	
	Szervíz adminok tudják droppolni a csatornákat anélkül, 
	hogy a szoba jelszót használnának, és megnézhetika az
	access, AKICK, és beállitási szint listát a csatornán.
	
CHAN_SERVADMIN_HELP_LOGOUT
	Syntax: LOGOUT #szoba [nicknév]
	
	A parancs naplózza a kijelentekeztetést, neki ójra kell
	azonosítani magukat,hogy visszakapják a hozzáféréseiket.	
	
	Ha ta  vagy a csatörna alpító, képes vagy kijelentkeztetni
	mást, te képes vagy magad is.	
	
	Ha szervíz admin vagy akkor kijelentekeztethetsz valakit a 
	csatornáról anélkül, hogy te lenneél a founder.Elhagyhatod
	a nincnév paramétert, és akkor az összes bejelentkezett 
	felhasználót kijelentkezteted a csatornáról.	

CHAN_SERVADMIN_HELP_DROP
	Syntax: DROP #szoba
	
	Törli a regisztrációt a csatornáról.Csak Services admins
	droppolhat csatornát,úgy hogy nem használja a szoba
	jelszavát.
	
CHAN_SERVADMIN_HELP_SET
	
	Szervíz admin beállíthatja a csatornát NEMELÉVÜLÕ -re,
	ekkor a sorban megelõzi az elévülõket. A szervíz 
	adminisztrátor beállíthatja anélkül, hogy szoba
	jelszót használná.	
	
CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Syntax: SET #szoba NOEXPIRE {ON | OFF}
	
	Beállítja a szoba elévülését.  Beállitva az
	ON paranmétert megelõzi az elévülõ csatornákat.
	
	Korlátozva Szervíz adminnak.


CHAN_SERVADMIN_HELP_INFO
	
	Szervíz adminok használhatják az ALL paramétert.
	
CHAN_SERVADMIN_HELP_LIST
	Syntax: LIST pattern[FORBIDDEN] [SUSPENDED] [NOEXPIRE]
	
	Lists all registered channels matching the given pattern.
	Channels with the PRIVATE option set will only be displayed
	to Services admins. Channels with the NOEXPIRE option set 
	will have a ! prepended to the channel name for Services admins.
	
	If the FORBIDDEN, SUSPENDED or NOEXPIRE options are given, only 
	channels which, respectively, are FORBIDden, SUSPENDed or have 
	the NOEXPIRE flag set will be displayed.  If both options are 
	given, both types of channels will be displayed. These options are 
	korlátozva szervíz adminnak.
	
CHAN_SERVADMIN_HELP_GETPASS
	Syntax: GETPASS #szoba
	
	Kiirja a szoba jelszavát.  Jegyzet bármikor használják a
	parancsot, egy üzenetet kap aki kiadta a parancsot és a szoba
	is ahol volt használva küld egy WALLOPS/GLOBOPS üzenetet.
	
	Korlátozva Szervíz adminnak.
	
CHAN_SERVADMIN_HELP_FORBID
	Syntax: FORBID #szoba [indok]
	
	Letiltja a szoba használatát.
	Csak droppolás után lehet a csatornát használni.
	
	Indoklást szükséges egyes hálózatokon.
	
	Korlátozva Szervíz adminnak.
	
CHAN_SERVADMIN_HELP_SUSPEND
	Syntax: SUSPEND #szoba [indok]

	Elutasít minden szobahasználatot. Lehet törölni a
	UNSUSPEND paranccsal és akkor használható az öüsszes 
	megörzött szoba adat/beállítás.
	
	Indoklást kér némelyik hálózat.
	
	Korlátozva Szervíz adminnak.
	
CHAN_SERVADMIN_HELP_UNSUSPEND
	Syntax: UNSUSPEND #szoba
	
	Törli a suspendet a csatornáról. Összes adat és beállítás
	megõrizve a suspend használat elõttröl.
	
	Korlátozva Szervíz adminnak.
	
CHAN_SERVADMIN_HELP_STATUS
	Syntax: STATUS  #szoba nicknév
	
	Kiirja a nicknév aktuális hozzáférési szintjét a megadott
	csatornán. 	Válasz a következõ formában::
	
	    STATUS szoba nicknév access-szint
	
	Ha hiba történik akkor a válasz a következõ formátunú:
	
	    STATUS ERROR Hiba - üzenet
	
	Korlátozva Szervíz adminnak.
	
###########################################################
#
# MemoServ help üzenetek
#
###########################################################
MEMO_HELP_CMD_SEND
	    SEND   Üzenetet küld egy nicknek vagy csatornára
MEMO_HELP_CMD_CANCEL
	    CANCEL Visszavonja az utoljára küldött memot
MEMO_HELP_CMD_LIST
	    LIST   Megjeleníti az üzenetek listáját
MEMO_HELP_CMD_READ
	    READ   Elolvassa a megadott üzenete(ke)t
MEMO_HELP_CMD_DEL
	    DEL    Törli a megadott üzenete(ke)t
MEMO_HELP_CMD_SET
	    SET    Üzenetekkel kapcsolatos beállításokat hajt végre
MEMO_HELP_CMD_INFO
	    INFO   Kiírja az üzenetekkel kapcsolatos információkat
MEMO_HELP_CMD_RSEND
	    RSEND  Memo üzenetet küld és olvasási visszaigazolást kér
MEMO_HELP_CMD_CHECK
	    CHECK  Ellenõrzi, hogy a címzett elolvasta-e már az üzit
MEMO_HELP_CMD_SENDALL
	    SENDALL Memo küldése minden regisztrált felhasználónak.
MEMO_HELP_CMD_STAFF
	    STAFF Memo küldése minden opernek/adminnak.

MEMO_HELP_HEADER
	%S lehetõséget nyújt az IRC felhasználókank, hogy rövid
	üzeneteket küldjenek más IRC felhasználók részére,
	akkor is, ha a fogadó fél nem online, és csatornákra (*).
	Ahhoz, hogy üzenetet lehessen küldeni, mind a küldõ és a
	fogadó fél nicknevének regisztráltnak kell lennie.
		
	%S parancsok a következõket tartalmazzák:
	
	
MEMO_HELP_ADMIN
	not used.
MEMO_HELP_OPER
	not used.
MEMO_HELP_FOOTER
	
	A parancsokkal kapcsolatos további segítségért
	írd be: %R%S HELP parancs.
	
	(*) Alapból, bármelyik user, minimum 10-es szinttel
	olvashatja a szoba memo üzeneit. Ez megváltoztatható a
	%s LEVELS paranccsal.
	
MEMO_HELP_SEND
	Syntax: SEND { nick  |  #szoba } memo-szöveg
	
	Üzenetet küld a megnevezett nicknek vagy csatornára
	a üzenet-szövegét tartalmazva. Amikor nicknévnek küldesz
	a címzett kap egy üzenetet az új üzenet érkezésérõl
	A címzett csatornának vagy nicknévnek regisztráltnak kell
	lennie.
	
MEMO_HELP_CANCEL
	Syntax: CANCEL { nick  |  #szoba }
	
	Visszavonja az utolsó memo üzenetet, amit a nicknek
	vagy csatornának küldtél.
	
MEMO_HELP_LIST
	Syntax: LIST [#szoba] [list | NEW]
	
	Megjelenít egy listát a jelenlegi üzenteteidrol. A NEW
	opcióval csak az új (olvasatlan) üzenteket listázza.
	listázza. Az olvasatlan üzeneteket jelöli egy "*" jellel
	az üzenetszámtól balra. Megadhatsz számlistát is, mint
	ahogy az alábbi példa mutatja:
	
		LIST 2-5,7-9
			Listázza az üzeneteket 2 és 5, illetve 7 és 9 között.
	
MEMO_HELP_READ
	Syntax: READ [#szoba] {sorszám| list| LAST | NEW}
	
	Elküldi a megadott üzenetek tartalmát. A LAST opcióval
	megjeleníti a legutolsó üzenetet. A NEW opcióval
	minden új üzenetet. Egyébként a megadott számú üzenetet.
	Megadhatsz számlistát is, mint a LIST parancsnál.
	
	Példák:
	
		READ 2-5,7-9
			Megjeleníti 2 és 5, a 7 és 9 között a memo üzeneteket
	
MEMO_HELP_DEL
	Syntax: DEL [#szoba] {sorszám | list | LAST | ALL}
	
	Törli a megadott memo üzenete(ke)t. 
	Több számot is megadhatsz, intervallumot is használhatsz,
	az egyszerû sorszám helyett.
	
	Ha a LAST meg van adva, az utolsó memo üzenetet törli
	Ha az ALL van megadve, akkor minden memo üzenetet töröl.
	
	Példák:
	
		DEL 1
			Törli az elsõ memo üzenetet.
	
		DEL 2-5,7-9
			Törli a memo üzeneteket 2 és 5, illetve 7 és 9 között.
	
MEMO_HELP_SET
	Syntax: SET opció paraméterek
	
	Beállít különbözo memo opciókat. 
	Az opció lehet az alábbiak egyike:
	
	    NOTIFY     Itt állíthatod be, ha figyelmeztetést szeretnél
	               kapni az új üzenetekrol (csak nicknevehez)
	    LIMIT      Beállítja a maximális üzenetek számát
	
	További információért írd be: %R%S HELP SET opció
	
MEMO_HELP_SET_NOTIFY
	Syntax: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
	
	Itt tudod beállítani, ha értesítést szeretnél kapni
	az új üzenetekrõl:
	
	    ON      Értesítést kapsz az új üzenetrõl csatlakozáskor,
	            amikor visszajössz /AWAY-ból, vagy ha küldenek neked.
	    LOGON   Értesítést kapsz az új üzenetekrõl csatlakozáskor
	            vagy ha visszajössz /AWAY-ból.
	    NEW     Csak akkor kapsz értesítést, ha új üzenet érkezett.
	    MAIL    You will be notified of memos by email aswell as
	            any other settings you have.
	    NOMAIL  You will not be notified of memos by email.
	    OFF     Nem kapsz értesítést az üzeneteidrõl.
	
	Az ON ekvivalens a LOGON és a NEW kombinálásával.
	
MEMO_HELP_SET_LIMIT
	Syntax: SET LIMIT [#szoba] limit
	
	Beállítja, hogy max. hány üzenetet fogadhatsz a nickedre
	(vagy a csatornára). Ha 0-ra állítod, senki nem
	küldhet üzenetet a nickedre/csatornára. Mindamellett,
	nem állíthatod ezt a számot többre, mint %d.
	
MEMO_HELP_INFO
	Syntax: INFO [#szoba]
	
	Információt jelenít meg arról, hogy mennyi üzeneted van,
	ebbõl mennyi olvasatlan, mennyi üzenetet fogadhatsz,
	és azt is megmondja, hogy kapsz-e üzenetet az új üzenetek
	érkezésekor. Paraméter megadása esetén az információt
	csatornára vonatkozóan jeleníti meg.
	
MEMO_SERVADMIN_HELP_SET_LIMIT
	Syntax: SET LIMIT [user | #szoba]{korlát|NONE}[HARD]
	
	Beállítja egy felhasználó vagy szoba max.fogadható
	memo üzenetienek számát, ha a limitet 0-ra állitod 
	akkor a felhasználó nem fogadhat üzeneteket.Ha a 
	NONE-vel állítod be akkor annyit fogadhat és tarthat
	meg amennyit akar.Ha nem adsz meg nicknevet, vagy csatornát
	akkor a saját korlátaidat állitja.
	
	HARD-al hozzáadva nem állíthatja a felhasználó a korlát 
	értékét Ha nem adod hozzá a HARD-ot akkor a felhasználó 
	kedve szerint váltóztathatja a korlát értékét (akkor is ha
	az elõzõ értéket a HARD-al adtad meg)
	
	SET LIMIT parancs használata korlátozható Szervíz
	adminokra. Egyéb felhasználók csak saját maguknak és a 
	csatornáknak ahol megfelelõ hozzáférésük van állíthatnak
	korlátot de nem törölhetik, és nem állíthatnak %d érték 
	felett valamint HARD korlátot.
	
	
MEMO_SERVADMIN_HELP_INFO
	Syntax: INFO [nick| #szoba]
	
	Paraméter nélkül kiirja az információt, hogy hány
	üzeneted van, ebböl hány olvasatlan, és összesen
	hány üzenetet fogadhatsz.
	
	A szoba paraméterrel, a csatornáról kapod meg  
	ezeket az információkat	

	A nicknév paraméterrel a megadott nicknévröl jeléeníti
	meg ezeket az információkat.Enek használat korlátozva 
	Szervíz adminra.
	
	
MEMO_HELP_STAFF
	Syntax: STAFF memo-üzenet
	
	Memo üzenet küldése az opereknek/adminoknak.
	Megjegyzés: Ha valamelyik nick szerepel az oper
	és admin listán is, akkor a memot kétszer kapja meg.
	Hasonló a helyzet a root és más listákkal.
	
MEMO_HELP_SENDALL
	Syntax: SENDALL memo-üzenet
	
	Memo üzenet küldése minden regisztrált felhasználónak.

MEMO_HELP_RSEND
	Syntax: RSEND {nick | #szoba} memo-üzenet
	
	Memot küld a megadott névnek, szobának a memo-üzenet
	tartalmával. Ha nicknévnek küldöd, akkor értesítést fog
	kapni, hogy új üzenete érkezett. A cél nicknévnek vagy
	csatornának regisztráltnak kell lennie.
	Amint a címzett elolvasta a memot, egy automatikus értesítést
	fog kapni a feladó arról, hogy az üzenetét elolvasták.

MEMO_HELP_CHECK
	Syntax: CHECK nick
	
	Ellenõrzi, hogy az _utolsó_ üzenet, amit a nick számára
	küldtél már olvasott-e vagy még olvasatlan.
	Megjegyzés, ez csak nickekkel mûködik szobákkal nem.

###########################################################
#
# OperServ help üzenetek
#
###########################################################

OPER_HELP_CMD_GLOBAL
	    GLOBAL      Üzenetet küld összes felhasználónak
OPER_HELP_CMD_STATS
	    STATS       Mutatja a Szervít és hálózat állapotát
OPER_HELP_CMD_OPER
	    OPER        Módosítja a Szervíz operátorok listáját
OPER_HELP_CMD_ADMIN
	    ADMIN       Módosítja a Szervíz admin listát
OPER_HELP_CMD_STAFF
	    STAFF       Kijelzi a szervízt és aktív állapotát
OPER_HELP_CMD_MODE
	    MODE        Váltóztatja a szoba módot
OPER_HELP_CMD_KICK
	    KICK        Kirúgja a felhasználót csatornáról
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  Törli a szoba módot
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  Killeli az összes klónt a hosztról
OPER_HELP_CMD_AKILL
	    AKILL       Szerkeszti az AKILL listát
OPER_HELP_CMD_SGLINE
	    SGLINE      Szerkeszti az SGLINE listát
OPER_HELP_CMD_SQLINE
	    SQLINE      Szerkeszti az SQLINE listát
OPER_HELP_CMD_SZLINE
	    SZLINE      Szerkeszti az SZLINE listát
OPER_HELP_CMD_CHANLIST
	    CHANLIST    Listázza az összes csatornát 
OPER_HELP_CMD_USERLIST
	    USERLIST    Listázza az összes felhasználót  
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   Fellépõ üzenetek beállítása
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  Véletlenszerûen megjelenõ üzenetet állít be 
	                a felhasználók részére
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    Operek részére megjelenõ üzenetet állít be
OPER_HELP_CMD_SESSION
	    SESSION     Olvassa a sessionok hoszt listáját
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   Módosítja a session-korlát/kivétel listát
OPER_HELP_CMD_NOOP
	    NOOP        Ideiglenesen törli az összes O:line-t a szerver
	                távollétében
OPER_HELP_CMD_JUPE
	    JUPE        Létrehoz egy "Jupiter" álszervert.
OPER_HELP_CMD_IGNORE
	    IGNORE      Módosítja a Szervíz mellõzési listáját
OPER_HELP_CMD_SET
	    SET         Beállítja szervíz általános változóit
OPER_HELP_CMD_RELOAD
	    RELOAD      Újratölti a szervíz Konfigurációs fájlját
OPER_HELP_CMD_UPDATE
	    UPDATE      A Szervíz adatbázist frissíti, felülírja
	                lemezre közvetlenül
OPER_HELP_CMD_RESTART
	    RESTART     Menti az adatbázist, és újraindítja a szervízt
OPER_HELP_CMD_QUIT
	    QUIT        Leállítja a szervíz programot mentés nélkül
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    Leállítja a szervíz programot mentéssel
OPER_HELP_CMD_DEFCON
	    DEFCON      Manipulálja a DefCon rendszert
OPER_HELP_CMD_CHANKILL
	    CHANKILL    AKILL az összes usert a csatornáról
OPER_HELP_CMD_OLINE
	    OLINE       Operflaget ad a biztos felhasználónak
OPER_HELP_CMD_UMODE
	    UMODE       Felhasználó módok állítása
OPER_HELP_CMD_SVSNICK
	    SVSNICK     Megváltoztatja a felhasználó nicknevét
OPER_HELP_CMD_MODLOAD
	    MODLOAD     Betölti a modult
OPER_HELP_CMD_MODUNLOAD
	    MODUNLOAD   Kitölti a modult
OPER_HELP_CMD_MODINFO
	    MODINFO     Információ a betöltött modulokról
OPER_HELP_CMD_MODLIST
	    MODLIST     Listázza a betöltött modulokat
	
OPER_HELP
	%S parancsok:
	
OPER_HELP_LOGGED
	Üzenet:Minden %S-nek küldött parancs naplózva van.
	
OPER_HELP_GLOBAL
	Syntax: GLOBAL üzenet
	
	Megengedi, hogy az IRCopok üzenetet küldjenek az összes 
	felhasználónak a hálózaton. Üzenetet a %s nickrõl küldi.
	
OPER_HELP_STATS
	Syntax: STATS [AKILL | ALL | RESET]
	
	Ha választasz valamit, megmutatja az aktuális
	felhasználók számát, a bejelentkezett IRCopokat (szervizek
	nélkül), legmagasabb felhasználószámot a szervíz indulása
	óta, és az idõt amióta fut a szerver.	

	Az  AKILL beállítással, kijelzi az aktuális maxot az
	AKILL listán és az aktuális lejárati idõt.
	
	Az ALL beállítást csak szervíz adminok használhatják és
	kiírja a szervíz memória használatának információját. Ez a
	beállítás lefagyaszthatja a szervízt egy rövid idõre nagy
	hálózat esetén, így nem lehet használni azt.	
	
	A RESET beállítás nullázza az aktuális felhasználók max.
	számát átírja a jelenlegi felhasználó számra	
	
	UPTIME, a STATS helyett használható.
	
OPER_HELP_OPER
	Syntax:	OPER ADD nick
			OPER DEL {nick | sorszám | lista}
			OPER LIST [maszk | lista]
			OPER CLEAR

	Megengedi a szervíz root, hogy hozzáadjon/töröljön
	nicknevet a szervíz operátor listáról. A nick használója
	aki a szervíz operátor listáján van és be van jelentkezve
	%s -hoz használni tudja az operátor parancsokat	

	Az  OPER ADD parancs hozzáad nicknevet a szervíz
	operátor listához.

	A OPER DEL parancs törli a nicknevet aszervíz operátor
	listáról. Ha listaszámot ad meg akkor azt a számú 
	felhasználót törli.( lásd a példa listát alul).
	
	Az  OPER LIST kiirja a szervíz operátorok listáját. Ha
	egy helyettesítõ lej van a maszkban megadva akkor csak
	az annak megfelelõt írja ki. Ha listaszámot ad meg csak
	azokat fogja mutatni:példában.	
	
		OPER LIST 2-5,7-9
			Lista a szervíz operátorokról 2 -töl 5-ig és
			7 -tõl 9-ig lévõk.
	      
	Az OPER CLEAR parancs töürli az öszzes nicket a
	Szervíz operátor listáról.
	
	Valamennyi IRC operátor használhat OPER LIST parancsot.
	
OPER_HELP_ADMIN
	Syntax:	ADMIN  ADD nick
			ADMIN  DEL { nick  |  sorszám  |  lista }
			ADMIN  LIST [ maszk  |  lista ]
			ADMIN  CLEAR
	
	Lehetõséget nyújt a Services rootnak, hogy hozzáadjon,
	töröljön nickneveket a Services adminok listáján. Az
	a felhasználó, aki szerepel a Szervíz adminok listáján,
	beazonosított a %s-be hozzáférhet az Admin parancsokhoz.
	
	ADMIN ADD parancs hozzáad egy felhasználót szervíz
	admin listához.
	
	ADMIN DEL parancs törli a felhasználót a szervíz admin
	listáról. Ha egy sorozatot adsz meg, akkor azok a
	bejegyzések lesznek törölve. (lásd az alábbiakban)
	
	ADMIN LIST parancs kiíratja a szervíz adminok listáját.  
	Ha maszk van megadva, csak a megegyezõ maszkot használó
	bejegyzések lesznek kiírva. Ha a listából bejegyzés 
	sorszámok vannak megadva csak azokat írja ki. Példa:
	
		ADMIN LIST 2-5,7-9
			Listázza a szervíz adminokat 2-tõl 5-ig és 7-tõl 
			9-ig.
	      
	ADMIN CLEAR parancs törli az összes bejegyzész a szervíz
	adminok listájából.
	
	Az IRC operátorok használhatják az ADMIN LIST parancsot.
	Összes egyébb parancs korlátozva Szervíz root részére.
	
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
	Syntax: MODE #szoba módok
	
	Megengedi a szervíz operátornak, hogy szoba módokat 
	állítson. Parameterek ugyanazok mint a szabvány
	/MODE parancsnál
		
	Korlátozva Szervíz operatornak.
	
OPER_HELP_UMODE
	Syntax: UMODE user módok
	
	Megengedi a szervíz adminoknak, hogy egy user felhasználói
	módjait megváltoztassa. Parameterek ugyanazok mint a
	szabvány /MODE parancsnál.
		
	Korlátozva Szuper adminnak.
	
OPER_HELP_OLINE
	Syntax: OLINE user flagek
	
	Lehetõséget ad a szervíz adminnak, hogy egy usernek 
	operflaget adjon. Elõjelként "+" vagy "-" adható meg.
	Ha csak a "-" paraméter adod meg akkor mindent eltávolít.
	
	Korlátozva Szuper adminnak.
	
OPER_HELP_CLEARMODES
	Syntax: CLEARMODES szoba [ALL]
	
	Töröl minden módot (i,k,l,m,n,p,s,t)és kitiltást a 
	csatornáról. Ha az ALL-t megadod akkor eltávolítja
	az op és voice jogokat (+o és +v módot) a csatornán. 
		
	Korlátozva Szervíz operátornak.
	
OPER_HELP_KICK
	Syntax: KICK #szoba user indok
	
	Lehetoséget nyújt az IRCopoknak, hogy bármelyik szobából
	kirúgjanak egy felhasználót. A paraméterek ugyanazok,
	mint a szabványos /KICK parancsnál. A kirúgási üzenetben
	benne lesz a KIK parancsot küldõ IRCop nickneve;  például:
	
	***xy ki lett rúgva a #csatornáról %S által.(op(csak))
	
	Korlátozva Szervíz operatornak.
	
OPER_HELP_SVSNICK
	Syntax: SVSNICK nick újnick
	
	Erõszakkal meváltóztatja a user nickevét újnicknévre.
	Limited to Super Admins.
	
OPER_HELP_AKILL
	Syntax: AKILL  ADD [ +lejárat] maszk indok
	        AKILL  DEL {maszk | sor-szám | list}
	        AKILL  LIST [maszk | list]
	        AKILL  VIEW [maszk | list]
	        AKILL  CLEAR
	
	Lehetõséget ad a szervíz opoknak, hogy karbantartsák az
	AKILL listát.  Ha egy felhasználó az AKILL maszkkal egyezõ
	cimrõl próbál csatlakozni , akkor szervíz kiad egy KILL 
	parancsot a nickre, és a támogatott szervertípusokon arra
	utasít minden szervert, hogy adjon K-linet a maszknak.
	
	AKILL ADD hozzáadja a megadott user@host/ip maszkot az
	AKILL listára a megadott indokkal (amit meg kell adnod).
	Az elévülést egész számmot kovet egy idõparaméter d (nap),
	h (óra), m (perc).Ezek kombinálása nem lehetséges(1h30m).
	ha nem adsz meg idõegységet akkor önmagában napot jelent.
	(+30 az 30 napot jelent).Ha +0-át adsz meg akkor nem 
	elévülõ lesz.Ha a usermaszk + -al kezdõdik akkor elévülést
	meg kell adni akkor is ha megegyezik az alapértelmezéssel.
	Az AKILL jelenlegi alapértelmezett elévüólési ideje 
	megnézhetõ a STATS AKILL paranccsal.

	AKILL DEL törli a maszkot az AKILL listáról, ha rajta
	van. Ha bejegyzés sorszámot adsz meg akkor azok fognak
	törlõdni.(lásd a példát alább)
	AKILL LIST kiirja a jelenlegi AKILL listát. Ha egy 
	"*" karakteres maszk van megadva, akkor csak azokat az
	AKILL-eket listázza, amelyek ráillenek a megadott maszkra.
	Ha sorszámot adsz meg akkor azok fognak törlõdni. Példa:

		AKILL LIST 2-5,7-9
	      Listázza az AKILL-en lévoket 2-tõl 5-ig és 7-tõl 9-ig
	      az AKILL listából

	AKILL VIEW részletesebb verziója az AKILL LIST-nek,
	amely megmutatja, hogy ki vette fel az AKILL-t,	mikor lett
	felvéve és mikor jár le, éppúgy, felhasználó@gazdanév 
	maszkot és indokot.
		
	AKILL CLEAR töröli az összes AKILL bejegyzést.
	
	Korlátozva Szervíz operatornak.
	
OPER_HELP_SGLINE
	Syntax: SGLINE ADD [+lejárat] maszk:indok
	    SGLINE DEL {maszk | sor-szám | list}
	        SGLINE LIST [maszk | list]
	        SGLINE VIEW [maszk | list]
	        SGLINE CLEAR
	
	Lehetõséget ad a szervíz opoknak, hogy karbantartsák az
	SGLINE listát.  Ha egy felhasználó egy valós névvel akar
	csatlakozni , ami szerepel az SGLINE listán akkor szervíz
	nem fogja engedélyezni részére a csatlakozást. 
	
	SGLINE ADD hozzáadja a valósnévmaszkot az SGLINE listára
	a megadott indokkal (amit meg is kell adni).	
	Az elévülést egész számmot kovet egy idoparaméter d (nap),
	h (óra), m (perc).Ezek kombinálása nem lehetséges(1h30m).
	ha nem adsz meg idoegységet akkor önmagában napot jelent.
	(+30 az 30 napot jelent).Ha +0-át adsz meg akkor nem 
	elévülo lesz.Ha a usermaszk + -al kezdodik akkor elévülést
	meg kell adni akkor is ha megegyezik az alapértelmezéssel.
	Az SGLINE jelenlegi alapértelmezett elévülési ideje 
	megnézheto a STATS AKILL paranccsal.
	Jegyzet: a valósnév maszkja tartalmazhat szóközt ezért
	közte és az indok között az elválasztó karakter a vesszõ.
	SGLINE DEL eltávolítja a megadott maszkot az SGLINE listáról,
	ha jelen van.Ha bejegyzés sorszámot adsz meg akkor azok fognak
	törlodni.	(Lásd a példa alább)
	SGLINE LIST kiirja az összes SGLINE bejegyzést; ha
	egy tetszõleges maszk meg van adva, a lista korlátozva lesz
	azokra a bejegyzésekre, amelyekre ráillik a maszk. Példa:
		
	   SGLINE LIST 2-5,7-9
	      Listázza a SGLINE-okat 2-töl 5-ig és 7-tõl 9-ig az 
	      SGLINE listáról
	      
	SGLINE VIEW részletesebb verziója a SGLINE LIST-nek,
	és megmutatja, ki készítette a SGLINE bejegyzéseket,
	és mikor jár le, mint a maszkot és az indokot.
	SGLINE CLEAR törli az SGLINE bejegyzéseket.
	
	Korlátozva Szervíz operatornak.
	
OPER_HELP_SQLINE
	Syntax: SQLINE ADD [+lejárat] maszk indok
	        SQLINE DEL {maszk | sor-szám | list}
	        SQLINE LIST [maszk | list]
	        SQLINE VIEW [maszk | list]
	        SQLINE CLEAR
	
	Engedélyezi a operátoroknak, hogy karbantartsák az SQLINE
	listát. Ha egy felhasználó "nick-je" ráillik egy SQLINe
	maszkra, megpróbál csatlakozni, a szervíz nem fogja engedni
	az egész irc-n.
	
	Ha az elsõ karakter a maszkban #, szervíz meg fogja elõzni 
	a használatát Az ilyen csatornáknak (az IRCD-k ezt 
	támogatják).

	SQLINE ADD hozzáadja az adott maszkot az SQLINE listához
	a megadott indokkal (amit meg kell adni).Az elévülést is,
	egész számmot követ egy idöparaméter d (nap),h (óra), 
	m (perc).Ezek kombinálása nem lehetséges(1h30m).
	Ha nem adsz meg idöegységet akkor önmagában napot jelent.
	(+30 az 30 napot jelent).Ha +0-át adsz meg akkor nem 
	elévülo lesz.Ha a usermaszk + -al kezdodik akkor elévülést
	meg kell adni akkor is ha megegyezik az alapértelmezéssel.
	Az SQLINE jelenlegi alapértelmezett elévülési ideje 
	megnézheto a STATS AKILL paranccsal.
	SQLINE DEL eltávolítja a maszkot az SQLINE listáról,
	ha jelen van.Ha bejegyzés sorszámot adsz meg akkor azok
	fognak törlõdni.	(Lásd a példa alább)
	SQLINE LIST kiirja az összes SQLINE bejegyzést; ha
	egy tetszöleges maszk meg van adva, a lista korlátozva lesz
	azokra a bejegyzésekre, amelyekre ráillik a maszk. Példa:
		
	   SQINE LIST 2-5,7-9
	      Listázza a SQLINE-okat 2-töl 5-ig és 7-tol 9-ig az 
	      SQLINE listáról
	      
	SQLINE VIEW részletesebb verziója a SQLINE LIST-nek,
	és megmutatja, ki készítette a SGLINE bejegyzéseket,
	és mikor jár le, mint a maszkot és az indokot.
	SQLINE CLEAR törli az SQLINE bejegyzéseket.
	
	Korlátozva Szervíz operatornak.
	
OPER_HELP_SZLINE
	Syntax: SZLINE ADD [+lejárat] maszk indok
	        SZLINE DEL {maszk | sorszám | list}
	        SZLINE LIST [maszk | list]
	        SZLINE VIEW [maszk | list]
	        SZLINE CLEAR
	
	Lehetöséget nyújt az operátoroknak, hogy karbantartsák az
	SZLINE listát. Ha egy felhasználó, IP címe ráillik egy 
	SZLINE maszkra, csatlakozni akar, a szervíz nem fogaj azt
	megengedni( és ez lehet IP vagy PTR RR vagy not)
	SZLINE ADD hozzáadja az adott maszkot az SZLINE listához
	a megadott indokkal (amit meg kell adni).Az elévülést is,
	egész számmot követ egy idöparaméter d (nap),h (óra), 
	m (perc).Ezek kombinálása nem lehetséges(1h30m).
	Ha nem adsz meg idöegységet akkor önmagában napot jelent.
	(+30 az 30 napot jelent).Ha +0-át adsz meg akkor nem 
	elévülo lesz.Ha a usermaszk + -al kezdodik akkor elévülést
	meg kell adni akkor is ha megegyezik az alapértelmezéssel.
	Az SZLINE jelenlegi alapértelmezett elévülési ideje 
	megnézheto a STATS AKILL paranccsal.
	SZLINE DEL eltávolítja a maszkot az SZLINE listáról,
	ha jelen van.Ha bejegyzés sorszámot adsz meg akkor azok 
	fognak törlodni.	(Lásd a példa alább)
	SZLINE LIST kiirja az összes SQLINE bejegyzést; ha
	egy tetszöleges maszk meg van adva, a lista korlátozva lesz
	azokra a bejegyzésekre, amelyekre ráillik a maszk. Példa:
		
	   SZINE LIST 2-5,7-9
	      Listázza a SZLINE-okat 2-töl 5-ig és 7-tol 9-ig az 
	      SZLINE listáról

			
	SZLINE VIEW részletesebb verziója a SZLINE LIST-nek,
	és megmutatja, ki készítette a SZLINE bejegyzéseket,
	és mikor jár le, mint az IP-t és az indokot.
	
	SQLINE CLEAR törli az SQLINE bejegyzéseket.
	
	Korlátozva Szervíz operatornak.
	
OPER_HELP_SET
	Syntax: SET  opció beállítás
	
	Beállítja a Services általános opcióit.
	A lehetséges opciók:
	    READONLY   Csak-olvasás vagy olvasás-írás mód beállítása
	    LOGCHAN    Kiírja a naplózott üzeneteket a csatornára
	    DEBUG      Bekapcsolja/kikapcsolja a debug módot
	    NOEXPIRE   Bekapcsolja/kikapcsolja a nem elévülõ módot
	    SUPERADMIN Bekapcsolja/kikapcsolja a super-admin módot
	    SQL   	 Activate or deactivate sql mode
	    IGNORE     Activate or deactivate ignore mode
	    LIST       List the options
	
	Korlátozva Szervíz adminnak.
	
OPER_HELP_SET_READONLY
	Syntax: SET READONLY {ON | OFF}
	
	Csak-olvasás mód aktiválása/kikapcsolása. A csak-olvasás
	mód, a normal felhasználó részére nem engedi, hogy bármit
	módosítson a szervízben, beleértve a nicknév, a
	hozzáférés listát, stb. IRCoperátorok a megfelelõ flaggel
	módosítani tudja a szervíz akill listát és a droppolhat
	vagy forbidolhat nickneveket és szobákat, de ez nem fog
	kerül mentésre, amíg a csak olvasni módot kikapcsolja
	és restartolja a szervízt.
	
	Ez a parancs egyenértéku a parancs-sorban kiadott
	-readonly opcióval.	

	
OPER_HELP_SET_LOGCHAN
	Syntax: SET LOGCHAN {ON | OFF}
	
	Beállítja, hogy a szervíz a naplózó csatornára küldje el
	a részletes napló adatokat és persze a naplófájlba.
	A Naplózó Csatornát (LogChannel) meg kell határozni a
	szervíz config fáljában, hogy	használhasd ezt az opciót.
	
	This option is equivalent to the command-line option -logchan.
	
	Jegyzet: súlyos biztonsági gondokat okozhat, ha ez a
	naplózó szoba nincs kellõen védve.
	
OPER_HELP_SET_DEBUG
	Syntax: SET DEBUG {ON | OFF | szám}
	
	Be vagy kikapcsolja a debug módot. Debug módban, minden a
	Szervíznek és a szervíz által küldött üzenet és persze még
	sok más üzenet rögzítésere kerül a naplófájlban. Ha megadsz
	számot is, akkor a debug mód bekapcsol és a megadott szinten
	fog muködni.
	
	Ez az opció megegyezik a parancs-sorban kiadható opcióval:
	-debug.
	
OPER_HELP_SET_NOEXPIRE
	Syntax: SET NOEXPIRE {ON | OFF}
	
	Bekapcsolja a nem elévülést. A nem elévülõ módban nickek,
	csatornák, akillek és kivételek rakhatóak nem elévülõre is
	amíg ezt az opciót nem kapcsolod ki.
	
	Ez az opció megegyezik a parancs-sorban kiadható opcióval:
	-noexpire.
	
OPER_HELP_SET_SUPERADMIN
	Syntax: SET SUPERADMIN {ON | OFF}
	
	Ez a beállítás extra jogokat biztosít számodra, például
	az "összes szoba founderje" stb...
	
	Ez az opció nem ajánlott, csak akkor használd, ha 
	szükséges és kapcsold ki, ha már nem.

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
	Syntax: NOOP SET szerver
	        NOOP REVOKE szerver
	
	NOOP SET levesz minden O:line-t a megadott
	szerver-en és killel minden IRCopot, aki fent van
	megelõzvén a szerver rehasholását (mert ez
	törölné a hatást).
	
	NOOP REVOKE minden levett O:line-t újra elérhetõvé tesz
	a megadott szerver-en.
	
	Jegyzet: A szerver paramétert nem ellenõrzi
	a Szerviz.
	
	Korlátozva Szervíz adminoknak.
	
OPER_HELP_JUPE
	Syntax: JUPE server [leirás]
	
	Tells Services to jupiter a server -- that is, to create
	a fake "server" connected to Services which prevents
	the real server of that name from connecting.  The jupe
	may be removed using a standard SQUIT.  If a reason is
	given, it is placed in the server information field;
	otherwise, the server information field will contain the
	text "Juped by <nick>", showing the nickname of the
	person who jupitered the server.
	
	Korlátozva Szervíz adminoknak.
	
OPER_HELP_RAW
	Syntax: RAW szöveg
	
	Sends a string of text directly to the server to which
	Services is connected.  This command has a very limited
	range of uses, and can wreak havoc on a network if used
	improperly.  DO NOT USE THIS COMMAND unless you are
	absolutely certain you know what you are doing!
	
	Korlátozva Szervíz adminoknak.
	
OPER_HELP_UPDATE
	Syntax: UPDATE
	
	Menti a Szervíz adatbázisát, azonnal amint kiadtad
	a parancsot.
	
	Korlátozva Szervíz adminoknak.
	
OPER_HELP_RELOAD
	Syntax: RELOAD
	
	Újratölti a Szervíz konfigurációs fájlját. Jegyzet: néhány
	direktívának ettõl függetlenül restart kell, hogy hatása
	életbe lépjen. (Például a Szervíz egységek nicknév váltása 
	vagy session korlátozás aktíválása, stb.)
	
	Korlátozva Szervíz adminoknak.
	
OPER_HELP_QUIT
	Syntax: QUIT
	
	A Szervíz azonnal leáll; az adatbázist nem menti el.
	Ez a parancs annak elkerülésére hasznos, hogy a memóriában
	lévõ adatok másolata bekerüljön az adatbázisba.
	Normál leállításokhoz használd kérlek a
	SHUTDOWN parancsot!
	
	Korlátozva Services adminoknak.
	
OPER_HELP_SHUTDOWN
	Syntax: SHUTDOWN
	
	Menti a Szervíz adatbázisát, majd leállítja a programot.
	
	Korlátozva Szervíz adminoknak.
	
OPER_HELP_RESTART
	Syntax: RESTART
	
	Menti a Szervíz adatbázisát, majd újraindítja a programot.
	(Kilép, majd újraindul.)
	
	Korlátozva Szervíz adminoknak.	
	
OPER_HELP_CHANLIST
	Syntax: CHANLIST [{pattern | nick} [SECRET]]
	
	Listáz minden csatornát, ami most aktív az IRC hálózaton
	attól függetlenül, hogy regisztrált vagy sem.
	
	Ha a pattern meg van adva, akkor csak az egyezõket
	listázza. Ha a nicknév van megadva, akkor listázza azokat
	a csatornákat, ahol az adott user bent van. Ha a SECRET
	van megadva, akkor listáz minden pattern-nek megfelelõ
	csatornár, melyen +s vagy +p mód van fent.
	
	Korlátozva Szervíz adminoknak.
	
OPER_HELP_USERLIST
	Syntax: USERLIST [{pattern | #szoba} [INVISIBLE]]
	
	Listáz minden jelenleg online usert az IRC hálózaton,
	attól függetlenül, hogy regisztrált vagy sem.
	
	Ha a pattern meg van adva, akkor csak az egyezõket 
	listázza (formátum: nick!user@host). Ha a #szoba
	meg van adva, akkor csak az adott csatornán lévõket
	listázza. Ha az INVISIBLE meg van adva, akkor csak a
	+i flages usereket listázza.
	
	Korlátozva Szervíz adminoknak.
	
OPER_HELP_MODLOAD
	Syntax: MODLOAD Fájlnév
	
	Ez a parancs betölti azt modult, a modules könyvtárból,
	amelyiknek a fájlnevét megadtad.
	
	Korlátozva Szervíz Rootnak.
	
OPER_HELP_MODUNLOAD
	Syntax: MODUNLOAD FileName
	
	Ez a parancs kitölti a megnevezett fájlnevû msdult a 
	modulok listájából.
		
	Korlátozva Szervíz Rootnak.
	
OPER_HELP_MODINFO
	Syntax: MODINFO Fájlnév
	
	A parancs részletes információt ad a betöltött modulokról.
	
	Korlátozva Szervíz Rootnak.
	
OPER_HELP_MODLIST
	Syntax: MODLIST [Core|3rd|protocol|encryption|supported|qatested]
	
	Listázza az összes betöltött aktív modult.
	
	Korlátozva Szervíz Rootnak.
		
###########################################################
#
# BotServ help üzenetek
#
###########################################################

BOT_HELP_CMD_BOTLIST
	BOTLIST            Listázza az elérhetõ botokat
BOT_HELP_CMD_ASSIGN
	ASSIGN             Társít egy botot a csatornához
BOT_HELP_CMD_SET
	SET                Konfigurálja a bot opcióit
BOT_HELP_CMD_KICK
	KICK               Konfigurálja a kirúgásokat
BOT_HELP_CMD_BADWORDS
	BADWORDS           Szerkeszti a tiltott szavak listáját
BOT_HELP_CMD_ACT
	ACT                Makes the bot do the equivalent of a "/me" command
BOT_HELP_CMD_INFO
	INFO               Allows you to see BotServ information about a channel or a bot
BOT_HELP_CMD_SAY
	SAY                Makes the bot say the given text on the given channel
BOT_HELP_CMD_UNASSIGN
	UNASSIGN           Unassigns a bot from a channel
BOT_HELP_CMD_BOT
	BOT                Karbantartja a hálózat botjainak listáját

BOT_HELP
	
	%S lehetõvé teszi, hogy botod legyen a csatornádon.
	Ezt olyan felhasználók számára készítették, akik
	nem tudnak saját botot futtatni, vagy a hálózaton nem
	engedélyezik ezt. Az elérhetõ parancsok listája alább 
	található; használatukhoz írd be %R%S parancs.
	Bõvebb információt így kaphatsz az adott parancsról:
	%R%S HELP parancs.
	
BOT_HELP_FOOTER
	A Bot akkor fog belépni a csatornára, ha legalább

	%d felhasználó van bent.
	
BOT_HELP_BOTLIST
	Syntax: BOTLIST
	
	Listázza az összes érvényes botot a hálózatról.
	
BOT_HELP_ASSIGN
	Syntax: ASSIGN #szoba nick
	
	Társítja a megnevezett botot a szobához. Beállíthatod a
	botodat a csatornád igényeidnek megfelelõen.
	Kellemes botozást! :-)
	
BOT_HELP_UNASSIGN
	Syntax: UNASSIGN #szoba
	
	Eltávolítja a botot a csatornáról. Ha használod ezt a
	parancsot, akkor a botod nem fog többet belépni a
	szobába. A bot konfigurációi megmaradnak.
	Azaz nem kell újra bekonfigurálnod, ha késõbb vissza
	akarod hozni a szobádba.
	
BOT_HELP_INFO
	Syntax: INFO {#szoba  |  nick}
	
	Lehetõvé teszi, hogy megnézd a %S információkat egy
	csatornával vagy bottal kapcsolatban. Ha a paraméter
	szobanév, akkor megkapod a szobára vonatkozó bot
	beállításokat. Ha a paraméter nick, akkor a botról
	kapsz információkat, például létrehozás ideje, csatornák
	száma.
	
BOT_HELP_SET
	Syntax: SET #szoba opció paraméterek
	
	Beállítja a bot opcióit. Az opció a következõ lehet:
	
	    DONTKICKOPS      Védelem az opoknak bot kirúgás ellen
	    DONTKICKVOICES   Védelem a voicesokna bot kirúgás ellen
	    GREET            Engedélyezi a köszöntõ üzenetet
	    FANTASY          Engedélyezi a fantaisi parancsokat
	    SYMBIOSIS        Megengedi, hogy a bot olyan legyen mint 
	                     az igazi botok
	
	Írd be %R%S HELP SET opció bõvebb információkért
	az adott opcióról.
	
	Megjegyzés: ezekhez a parancsokhoz a hozzáférést a
	levels-ben lehet állítani.
	
BOT_HELP_SET_DONTKICKOPS
	Syntax: SET #szoba DONTKICKOPS {ON|OFF}
	
	Engedélyezi vagy tiltja opok védelmét a csatornán.
	Ha engedélyezve van, akkor az opokat nem fogja a bot
	kirúgni, akkor se, ha nem egyeznek meg a NOKICK szinttel
		
BOT_HELP_SET_DONTKICKVOICES
	Syntax: SET #szoba DONTKICKVOICES { ON|OFF }
	
	Engedélyezi vagy tiltja hangadók védelmét a csatornán.
	Ha engedélyezve van, akkor a hangadókat nem fogja a bot
	kirúgni, akkor se, ha nem egyeznek meg a NOKICK szinttel
	
BOT_HELP_SET_FANTASY
	Syntax: SET #szoba FANTASY { ON|OFF }
	
	Engedélyezi vagy tiltja fantasy módot a csatornán.
	Ha engedélyezve van, akkor a következõ parancsok
	használhatók a csatornán: !op, !deop, !voice, !devoice,
	!kick, !kb, !unban, !seen (találd ki hogyan kell
	használni õket; próbáld meg nick megadásával, a nélkül
	is)
	
	Azoknak akik ezeket a parancsokat akarják használni
	rendelkezniük KELL a szükség szinttel mind a FANTASIA
	mind pedig az egyéb szintekkel, ami a parancshoz
	szükséges. (például, az !op használatához, meg kell
	legyen a megfelelõ szintjük az OPDEOP parancsokhoz.).
	
BOT_HELP_SET_GREET
	Syntax: SET #szoba  GREET { ON|OFF }
	
	Engedélyezi vagy tiltja Köszöntõ módot a csatornán.
	Ha engedélyezve van, akkor a bot megjeleníti azoknak
	a köszöntõ üzenetét, akiknek a szintjük megfelelõ ehhez.
	
BOT_HELP_SET_SYMBIOSIS
	Syntax: SET #szoba SYMBIOSIS { ON|OFF }
	
	Engedélyezi vagy tiltja symbiosis módot a csatornán.
	Ha engedélyezve van, akkor a bot fog mindent csinálni,
	amit egyébként a %s csinálna a csatornán, mint MODE,
	KICK, és a belépõüzenet kiírása.
	
BOT_HELP_KICK
	Syntax: KICK  #szobaopcióparaméterek
	
	Bot kirúgások beállítása. Az opció a következõ lehet:
	
	    BOLDS         A bot kirúgja félkövér betûkkel írókat
	    BADWORDS      A bot kirúgja a csúnyaszavakat használókat
	    CAPS          A bot kirúgja CapsLock használókat
	    COLORS        A bot kirúgja szinesen írókat
	    FLOOD         A bot kirúgja floodolókat
	    REPEAT        A bot kirúgja az önmagukat 
	                  ismétlõket.
	    REVERSES      A bot kirúgja visszafelé írókat
	    UNDERLINES    A bot kirúgja aláhúzással írókat
	
	Írd be %R%S HELP KICK opció az adott opcióval
	kapcsolatos bõvebb információkért.
	
	Megjegyzés: A parancshoz való hozzáférést a level SET-ben
	lehet korlátozni.
	
BOT_HELP_KICK_BOLDS
	Syntax: KICK  #szoba BOLDS { ON|OFF } [ ttb ]
	
	Beállítja a félkövér írásért járó kirúgást.
	Ha engedélyezve van akkor a bot kirúg a félkövér betûkért.
	
	A ttb a banolás elotti kickek száma.
	Ha nem adod meg, akkor csak kirúgást eszközöl a bot.
	Ha megadod a ttb-t, akkor x kirúgás után banol a bot.
	
BOT_HELP_KICK_COLORS
	Syntax: KICK  #szoba COLORS { ON|OFF } [ ttb ]
	
	Beállítja a szines írásért járó kirúgást.
	Ha engedélyezve van akkor a bot kirúgja a színesen írókat.
	
	A ttb a banolás elõtti kickek száma.
	Ha nem adod meg, akkor csak kirúgást eszközöl a bot.
	Ha megadod a ttb-t, akkor x kirúgás után banol a bot.
	
BOT_HELP_KICK_REVERSES
	Syntax: KICK  #szobaREVERSES { ON|OFF } [ ttb ]
	
	Beállítja a visszafelé írásért járó kirúgást.
	Ha engedélyezve van akkor a bot kirúgja a reverse írókat.
	
	A ttb a banolás elõtti kickek száma.
	Ha nem adod meg, akkor csak kirúgást eszközöl a bot.
	Ha megadod a ttb-t, akkor x kirúgás után banol a bot.	

BOT_HELP_KICK_UNDERLINES
	Syntax: KICK #szobaUNDERLINES { ON|OFF } [ ttb ]
	
	Beállítja az aláhúzott írásért járó kirúgást.
	Ha engedélyezve van akkor a bot kirúgja az aláhúzva írókat
	
	A ttb a banolás elõtti kickek száma.
	Ha nem adod meg, akkor csak kirúgást eszközöl a bot.
	Ha megadod a ttb-t, akkor x kirúgás után banol a bot.
	
BOT_HELP_KICK_CAPS
	Syntax: KICK #szobaCAPS { ON|OFF } [ ttb  [ min  [ százalék ]]]
	
	Beállítja a túlzott caps miatt járó kirúgást.
	Ha engedélyezve van, akkor a bot kirúgja a "kiabálókat".
	
	A bot csak akkor rúgja ki a usert, ha legalább min számú
	betû van és ez legalább százaléka%% az összes szövegnek
	a sorban (ha nincs megadva, akkor 10 karakter a min és
	annak legalább a 25%-a az alapbeállítás)
	
	A ttb a banolás elõtti kickek száma.
	Ha nem adod meg, akkor csak kirúgást eszközöl a bot.
	Ha megadod a ttb-t, akkor x kirúgás után banol a bot.
	
BOT_HELP_KICK_FLOOD
	Syntax: KICK #szoba FLOOD { ON|OFF } [ ttb  [ sor  [ mp ]]]
	
	Beállítja a flood miatti kirúgást. Ha engedélyezve van
	akkor a bot kirúgja a floodoló usereket, ha legalább
	sor számú sort írtak mp másodpercnyi idõ alatt.
	(ha nincs megadva, akkor 6 sor és 10 mp alatt).
	
	A ttb a banolás elõtti kickek száma.
	Ha nem adod meg, akkor csak kirúgást eszközöl a bot.
	Ha megadod a ttb-t, akkor x kirúgás után banol a bot.
	
BOT_HELP_KICK_REPEAT
	Syntax: KICK  #szoba REPEAT { ON|OFF } [ ttb  [ szám ]]
	
	Beállítja az ismétlés miatti kirúgást. Ha engedélyezve
	van az opció, akkor a bot kirúgja az önmagukat ismétlõ
	usereket, ha szám alkalommal ismételtek. 
	(Ha nem adsz meg értéket, akkor az érték 3 lesz)
	
	A ttb a banolás elõtti kickek száma.
	Ha nem adod meg, akkor csak kirúgást eszközöl a bot.
	Ha megadod a ttb-t, akkor x kirúgás után banol a bot.
	
BOT_HELP_KICK_BADWORDS
	Syntax: KICK #szoba BADWORDS {ON|OFF} [ttb]
	
	Beállítja a csúnyaszavak miatti kirúgást. Ha engedélyezve
	van, akkor a bot kirúgja azt, aki kiejt egy olyan szót,
	ami szerepel a csúnyaszavak listáján.
	
	A csúnyaszavak listáját a csatornádhoz neked kell megadnod
	A BADWORDS paranccsal. Írd be %R%S HELP BADWORDS 
	bõvebb információkért.
	
	A ttb a banolás elotti kickek száma.
	Ha nem adod meg, akkor csak kirúgást eszközöl a bot.
	Ha megadod a ttb-t, akkor x kirúgás után banol a bot.
	
BOT_HELP_BADWORDS
	Syntax:BADWORDS #szoba ADD  szó  [SINGLE  |  START  |  END]
		 BADWORDS #szoba DEL { szó  |  sorszám  |  list}
		 BADWORDS #szoba LIST  [ maszk  |  list]
		 BADWORDS #szoba CLEAR

	Karbantartja a szoba csúnyaszó listáját. A tiltott
	szavak listája meghatározza, hogy mely szavak kiejtésére
	rúgja ki a bot a usert, ha a kirúgás engedélyezve van,
	errõl bõvebb információk: %R%S HELP KICK BADWORDS.
	
	A BADWORDS ADD parancs hozzáad egy szót a listához.
	Ha a SINGLE van megadva, akkor lesz kirúgja a user, ha
	a konkrét szót mondja ki.
	Ha a START van megadva, akkor a szó elejét figyeli a bot.
	Ha az END van megadva, akkor a szó végét figyeli a bot.
	Ha nem adsz meg semmit, akkor	a user minden alkalommal
	kirúgásra kerül, ha kiejti a szót.
	Ha nem beszél csúnyán, akkor semmi baja nem lesz! :-)

	A BADWORDS DEL parancs eltávolítja a megadott szót
	a tiltott szavak listájáról. Ha egy sorozatot adsz meg
	a bejegyzések számával, akkor azok a bejegyzések kerülnek
	törlésre. (Lásd lentebb a listázást.)
	
	A BADWORDS LIST parancs megjeleníti a tiltott szavakat.
	Ha használod a * karaktert, akkor csak az egyezõ
	bejegyzések kerülnek listázásra. Ha egy sorozatot adsz meg
	akkor azok a bejegyzések kerülnek megjelenítésre; példa:

	   BADWORDS  #szoba  LIST  2-5,7-9
	      Listázza a csúnyaszavakat 2-tõl 5-ig és a 7-tõl
	      9-ig.
	      
	BADWORDS CLEAR parancs törli az összes bejegyzést
	a tiltott szavak listájáról
	
BOT_HELP_SAY
	Syntax: SAY #szoba szöveg
	
	Elküldi a bot a megadott szöveget a csatornára.
	
BOT_HELP_ACT
	Syntax: ACT #szoba szöveg
	
	A bot "/me" akcióval küldi el a megadott szöveget
	a csatornára.
	
BOT_SERVADMIN_HELP_BOT
	Syntax:	BOT ADD  nick   user   hoszt   valós
			BOT CHANGE  réginick   újnick [user  [ hoszt  [ valós ]]]
			BOT DEL nick
	
	Lehetõvé teszi a Services adminoknak, hogy létrehozzanak
	töröljenek, módosítsanak botokat, amiket a userek
	használhatnak a csatornáikon.
	
	BOT ADD hozzáad egy botot a megadott nick, usernév,
	hostnév és valósnév használatával. Amióta nincs ellenõrzés
	kérlek óvatósan használd ezt a parancsot.
	BOT CHANGE megváltoztatja egy bot nickjét, usernevét,
	hostnevét, valósnevét, anélkül hogy törölni kellene
	(minden adat, beállítás megmarad benne.)
	BOT DEL törli a megadott botot a listából.  
	
	Megjegyzés: ha létrehozol egy botot, egy már regisztrált
	nickkel, akkor a nick dropolva lesz. Akkor is, ha a user
	éppen használja a nicket; killelve lesz.
	
BOT_SERVADMIN_HELP_SET
	
	Ezeket az opciókat csak Szervíz adminok használhatják:
		
	    NOBOT            Megelõzi, hogy egy botot társítani lehessen
	                       csatornához
	    PRIVATE          Megelõzi, hogy egy botot nem IRCop személy
	                       társíthasson
	
BOT_SERVADMIN_HELP_SET_NOBOT
	Syntax: SET #szoba NOBOT {ON|OFF}
	
	Ez az opció a botot szobához társíthatatlanná teszi.
	Ha egy bot már társítva van egy csatornához
	akkor törlödik onnan, ha ezt az opciót bekapcsolod.
	
	Korlátozva Szervíz adminoknak.
	
BOT_SERVADMIN_HELP_SET_PRIVATE
	Syntax: SET bot-nick PRIVATE {ON|OFF}
	
	Ez az opció megelõzi, hogy az adott botot a csatornához
	nem IRC operátor személyek társíthassák.
	
	Korlátozva Szervíz adminoknak.
	
###########################################################
#
# HostServ üzenetek
#
###########################################################
HOST_EMPTY
	A vhost lista üres.
HOST_ENTRY
	#%d Nick:%s, vhost:%s (%s - %s)
HOST_IDENT_ENTRY
	#%d Nick:%s, vhost:%s@%s (%s - %s)
HOST_SET
	%s vhostja erre változott: %s.
HOST_IDENT_SET
	%s vhostja erre változott: %s@%s.
HOST_SETALL
	A %s csoport vhostja %s lett.
HOST_DELALL
	A %s csoport vhostja törölve.
HOST_DELALL_SYNTAX
	DELALL <nick>.
HOST_IDENT_SETALL
	A %s csoport vhostja erre változott: %s@%s.
HOST_SET_ERROR
	A vhost hosztmaszknak érvényes formátumban kell lennie.
HOST_SET_IDENT_ERROR
	A vhost identnek érvényes formátumúnak kell lenni.
HOST_SET_TOOLONG
	Hiba! A vhost hoszzú, írj %d karakternél rövidebbet!
HOST_SET_IDENTTOOLONG
	Hiba! Az Ident hoszzú, írj %d karakternél rövidebbet!
HOST_NOREG
	%s felhasználó nincs a nickserv db fájlban.
HOST_SET_SYNTAX
	SET <nick> <hosztmaszk>.
HOST_SETALL_SYNTAX
	SETALL <nick> <hosztmaszk>.
HOST_DENIED
	Hozzáférés megtagadva.
HOST_NOT_ASSIGNED
	Keress meg egy Operátort és kérj tõle vhostot.
HOST_ACTIVATED
	A %s virtuális hosztod aktiválva.
HOST_IDENT_ACTIVATED
	A  %s@%s virtuális hosztod aktiválva.
HOST_ID
	Kérlek azonosíts elõbb a szervízben.
HOST_NOT_REGED
	Csak regisztrált nickhez tudsz vhostot társítani.
HOST_DEL
	%s vhosztja törölve lett.
HOST_DEL_SYNTAX
	DEL <nick>.
HOST_OFF
	Your vhost was removed and the normal cloaking restored.
HOST_OFF_UNREAL
	A vhostod kikapcsolva. Hogy újra használd az alap
	hosztrejtést írd be /mode %s +%s
HOST_NO_VIDENT
	Ön ircdje nem támogatja a videnteket, ha ez nem igaz,
	akkor kérem jelentse ezt, mint lehetséges hibát.
HOST_GROUP
	Az összes user vhosztja a(z) %s csoportba %s lett
HOST_IDENT_GROUP
	Az összes user vhosztja a(z) %s csoportba %s@%s lett
HOST_LIST_FOOTER
	Kiírja az összes bejegyzést (Számláló: %d)
HOST_LIST_RANGE_FOOTER
	Kiírja a bejegyzéseket %d-tól %d-ig
HOST_LIST_KEY_FOOTER
	Kiírja azokat a hosztokat melyben a %s szó szerepel.
	(számláló: %d)
	
###########################################################
#
# HostServ Help üzenetek
#
###########################################################
HOST_HELP_CMD_ON
	    ON          Aktiválja a társított vhostot
HOST_HELP_CMD_OFF
	    OFF         Kikapcsolja a társított vhostot
HOST_HELP_CMD_GROUP
	    GROUP       A vhostot a csoport minden tagjára állítja
HOST_HELP_CMD_SET
	    SET         Beállít egy vhosztot a felhasználónak
HOST_HELP_CMD_SETALL
	    SETALL      Beállítja a vhosztot összes nicknek a csoportban
HOST_HELP_CMD_DEL
	    DEL         Törli egy felhasználónak a vhostját
HOST_HELP_CMD_DELALL
	    DELALL      Törli a vhosztot a nickekröl a csoportban
HOST_HELP_CMD_LIST
	    LIST        Kiír egy vagy több vhost bejegyzést

HOST_OPER_HELP
	not used.

HOST_ADMIN_HELP
	not used.

HOST_HELP
	%S parancsok:

HOST_HELP_ON
	Syntax: ON

	Aktiválja a vhostodat, ami a nickhez lett társítva.
	Ha valaki csinál egy /whois parancsot a nickeden,
	akkor a vhostodat fogja látni a valódi IP címed vagy
	hosztneved helyett.

HOST_HELP_SET
	Syntax: SET <nick> <hostmask>.
	
	Beállítja vhostnak a nick részére a hosztmaszkot.
	Ha az IRCD támogatja a vIdentet használd a SET
	<nick> <ident>@<hostmask>, beállitja az identet
	 és a hosztot a nick vhosztjaként.
	
	Korlátozva Hoszt változtatókra.

HOST_HELP_DELALL
	Syntax: DELALL <nick>.

	Törlõdik a vhost az összes nickrõl a csoportban ahol az
	adott nick van és ez a vhosztot használta 
	
	Korlátozva Hoszt változtatókra.

HOST_HELP_SETALL
	Syntax: SETALL <nick> <hosztmaszk>.

	Beállítja a vhosztot összes nick részére aki a csoportban 
	van. Ha az IRCD támogatja a vIdent, akkor használja a 
	settall <nick> <ident>@<hosztnév> parancsot, ez beállítja
	az adott identet és hosztot az összes tag részére vhosztként.

	*Jegyzet: ez nem fogja frissíteni a vhostot azoknak, akik
	a nickeket a csoportban a parancs után jegyezték	

	Korlátozva Hoszt változtatókra.

HOST_HELP_OFF
	Syntax: OFF

	Kikapcsolja a vhostodat,ez most társitva van a nickedhez.
	Ha valaki használ /whois parancsot a nickeden, akkor a
	valós IP címed vagy hosztod fogja látni.

HOST_HELP_DEL
	Syntax: DEL <nick>

	A vhost törölve az adott nick adatbázisából.

	Korlátozva Hoszt változtatókra.

HOST_HELP_LIST
	Syntax: LIST [<kulcs>|<#X-Y>]
	
	Ez parancs listázza a regisztrált vhostokat az operátornak
	Ha egy kulcs meg van határozva, akkor csak a feltételnek
	megfelelõ vhostokat adja ki melyekben vagy a nickben vagy
	a hosztban szerepel a kulcs. Pl: Rob*

	Ha az #x-y stílust használod, csak a megadott sorozatba
	tartozó X és Y közötti vhostok fognak megjelenni. Például
	az #1-3 csak az elsõ 3 vhostot jeleníti meg.

	A lista használja NSListMax értéket, mint tág határértéket
	amikor megjeleníti a tartalmat az operátornak.

	Korlátozva Szervíz operátorokra.

HOST_HELP_GROUP
	Syntax: GROUP
	
	Ez a parancs lehetõvé teszi, hogy a JELENLEGI nick
	vhostját beállítsuk a csoport minden nickjére.

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

