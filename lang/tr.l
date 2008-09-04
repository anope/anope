# Turkish language file.
#
# (C) 2003-2008 Anope Team
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
# Name of this language
#
###########################################################################

# For languages other than English, this string should have the following
# format:
#     language-name-in-language (language-name-in-English)
# For example, "Español (Spanish)" or "Français (French)".

LANG_NAME
	Türkçe (Turkish)
#################################################################
# Translated by CafeiN (oytuny@yahoo.com) irc.ixir.net          #
# Modified by MeShGuL - guzelbey@cs.utk.edu - irc.arkadas.com   #
#################################################################

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
	Paz
	Pzt
	Sal
	Çar
	Per
	Cum
	Cmt
# %A
STRFTIME_DAYS_LONG
	Pazar
	Pazartesi
	Salý
	Çarþamba
	Perþembe
	Cuma
	Cumartesi
# %b
STRFTIME_MONTHS_SHORT
	Oca
	Þub
	Mar
	Nis
	May
	Haz
	Tem
	Aðu
	Eyl
	Eki
	Kas
	Ara
# %B
STRFTIME_MONTHS_LONG
	Ocak
	Þubat
	Mart
	Nisan
	Mayýs
	Haziran
	Temmuz
	Aðustos
	Eylül
	Ekim
	Kasým
	Aralýk


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	, 


# Various error messages.
USER_RECORD_NOT_FOUND
	Dahili hata - Kullanýcý kaydý bulunamadý.
UNKNOWN_COMMAND
	Varolmayan komut %s.
UNKNOWN_COMMAND_HELP
	Varolmayan komut %s. yardým için "%R%s HELP" yazýnýz.
SYNTAX_ERROR
	Kullanýmý: %s
MORE_INFO
	Daha fazla bilgi için %R%s HELP %s yazýnýz.
NO_HELP_AVAILABLE
	%s hakkýnda yardým bulunamadý.
OBSOLETE_COMMAND
	Bu komut artýk kullanýlmýyor; onun yerine %s komutunu deneyin.

BAD_USERHOST_MASK
	Mask user@host þeklinde tanýmlanmalýdýr.
BAD_EXPIRY_TIME
	Geçersiz zaman aþýmý süresi.
USERHOST_MASK_TOO_WIDE
	%s kapsamý aþýrý geniþ; Lütfen daha belirli bir mask kullanýn.

SERVICE_OFFLINE
	%s þu an devre dýþý.
READ_ONLY_MODE
	Not: Servisler read-only modunda; deðiþiklikler kaydedilmeyecektir!
PASSWORD_INCORRECT
	Þifre geçersiz.
INVALID_TARGET
	"/msg %s" is no longer supported.  Use "/msg %s@%s" or "/%s" instead.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Bu komuta eriþiminiz yok.
PERMISSION_DENIED
	Bu komutu kullanma izniniz yok.
RAW_DISABLED
	The RAW option has been disabled. If you must use it, enable the DisableRaw directive in Services configuration.

MORE_OBSCURE_PASSWORD
	Lütfen daha zor bir þifre deneyin.  Þifreler en az 5 karakter uzunlukta, kolayca tahmin edilemeyecek birþey olmalýdýr(ör: gerçek isminiz, nickiniz v.b. olmamalýdýr), ve boþlukla tab kullanýlmamalýdýr.
PASSWORD_TRUNCATED
	Uyarý:  Þifreniz %d karaktere kadar kýsaltýldý.

NICK_NOT_REGISTERED
	Nickiniz kayýtlý deðil.
NICK_NOT_REGISTERED_HELP
	Nickiniz kayýtlý deðil.  Nick kayýt hakkýnda bilgi almak için %R%s HELP yazýn.
NICK_X_IS_SERVICES
	Nick %s is part of this Network's Services.
NICK_X_NOT_REGISTERED
	%s kayýtlý bir nick deðil.
NICK_X_IN_USE
	Nick %s is currently in use.
NICK_X_NOT_IN_USE
	%s nicki þu an kullanýmda deðil.
NICK_X_NOT_ON_CHAN
	%s su anda %s kanalinda degil.
NICK_X_FORBIDDEN
	%s nicki kayýt edilemez ve kullanýlamaz.
NICK_X_FORBIDDEN_OPER
	%s nicki yasaklanmýþtýr. %s:
	%s
NICK_X_ILLEGAL
	Nick %s is an illegal nickname and cannot be used.
NICK_X_TRUNCATED
	Nick %s was truncated to %d characters.
NICK_X_SUSPENDED
	Nick %s is currently suspended.
CHAN_X_NOT_REGISTERED
	%s kanalý kayýtlý deðil.
CHAN_X_NOT_IN_USE
	%s kanalý þu an kullanýmda deðil.
CHAN_X_FORBIDDEN
	%s kanalý kayýt edilemez ve kullanýlamaz.
CHAN_X_FORBIDDEN_OPER
	%s kanalý yasaklanmýþtýr. %s:
	%s
CHAN_X_SUSPENDED
	      Suspended: [%s] %s
NICK_IDENTIFY_REQUIRED
	Bu komutu kullanabilmek için þifrenizi girmelisiniz.
	%R%s IDENTIFY þifreniz yazarak yeniden deneyiniz.
CHAN_IDENTIFY_REQUIRED
	Bu komutu kullanabilmek için þifrenizi girmelisiniz.
	%R%s IDENTIFY %s þifreniz yazarak yeniden deneyiniz.
	
MAIL_DISABLED
	Servislerin mail gönderme özelliði devre dýþýdýr.
MAIL_INVALID
	%s için mail adresi geçerli deðil.
MAIL_X_INVALID
	%s geçerli bir mail adresi deðil.
MAIL_LATER
	Þu an mail gönderilemiyor; lütfen az sonra tekrar deneyin.
MAIL_DELAYED
	Lütfen %d saniye bekleyin ve tekrar deneyin.
	
NO_REASON
	Sebep yok
UNKNOWN
	<varolmayan>

# Duration system
DURATION_DAY
	1 gün
DURATION_DAYS
	%d gün
DURATION_HOUR
	1 saat
DURATION_HOURS
	%d saat
DURATION_MINUTE
	1 dakika
DURATION_MINUTES
	%d dakika
DURATION_SECOND
	1 saniye
DURATION_SECONDS
	%d saniye

# Human readable expiration
NO_EXPIRE
	zaman aþýmý yok
EXPIRES_SOON
	bir sonraki veritabaný güncellemesinde zaman aþýmý süresi dolacak
EXPIRES_M
	%d dakika sonra zaman aþýmý süresi dolacak
EXPIRES_1M
	%d dakika sonra zaman aþýmý süresi dolacak
EXPIRES_HM
	%d saat, %d dakika sonra zaman aþýmý süresi dolacak
EXPIRES_H1M
	%d saat, %d dakika sonra zaman aþýmý süresi dolacak
EXPIRES_1HM
	%d saat, %d dakika sonra zaman aþýmý süresi dolacak
EXPIRES_1H1M
	%d saat, %d dakika sonra zaman aþýmý süresi dolacak
EXPIRES_D
	%d gün sonra zaman aþýmý süresi dolacak
EXPIRES_1D
	%d gün sonra zaman aþýmý süresi dolacak

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
	Bu nick baþkasýna ait. Lütfen baþka bir tane seçin.
	(Eðer bu sizin nickiniz ise, %R%s IDENTIFY þifreniz yazýnýz.)
NICK_IS_SECURE
	Bu nick kayýtlý ve korumalýdýr. Eðer bu sizin nickiniz ise, 
	%R%s IDENTIFY þifreniz yazýnýz. Yada, baþka bir nick seçin.
NICK_MAY_NOT_BE_USED
	Bu nick kullanýlamaz.  Lütfen baþka bir tane seçin.
FORCENICKCHANGE_IN_1_MINUTE
	Eðer bir dakika içinde nickinizi deðiþtirmezseniz, Ben deðiþtireceðim.
FORCENICKCHANGE_IN_20_SECONDS
	Eðer 20 saniye içinde nickinizi deðiþtirmezseniz, Ben deðiþtireceðim.
FORCENICKCHANGE_NOW
	Bu nick kayýtlýdýr; bunu kullanamazsýnýz.
FORCENICKCHANGE_CHANGING
	Nickiniz %s olarak deðiþtirildi.

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER þifreniz [emailiniz]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER þifreniz emailiniz
NICK_REGISTRATION_DISABLED
	Nick kaydý geçici olarak devre dýþýdýr.
NICK_REGISTRATION_FAILED
	Nick kaydý baþarýsýz.
NICK_REG_PLEASE_WAIT
	Lütfen %d saniye REGISTER komutunu tekrar kullanmak için bekleyin.
NICK_CANNOT_BE_REGISTERED
	%s nicki kayýt edilemez.
NICK_ALREADY_REGISTERED
	%s nicki zaten kayýtlý!
NICK_REGISTERED
	%s nicki sizin adýnýza kayýt edildi: %s
NICK_REGISTERED_NO_MASK
	Nickname %s registered.
NICK_PASSWORD_IS
	Nick Þifreniz %s - Ýlerde kullanacaðýnýz için unutmayýnýz ve kimseye söylemeyiniz.

# GROUP responses
NICK_REG_DELAY
	You have to be connected longer than %d seconds to register your nick.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP hedef þifre
NICK_GROUP_DISABLED
	Nick gruplama geçici olarak devre dýþý.
NICK_GROUP_FAILED
	Nick gruplama baþarýsýz.
NICK_GROUP_PLEASE_WAIT
	GROUP komutunu tekrar kullanmadan önce lütfen %d saniye bekleyin.
NICK_GROUP_CHANGE_DISABLED
	Nickiniz zaten kayýtlý; önce %R%s DROP yazýn.
NICK_GROUP_SAME
	%s grubunun zaten bir üyesisiniz.
NICK_GROUP_TOO_MANY
	%s nickinin grubunda çok fazla nick var; listeleyip birkaçini silin.
	Daha fazla bilgi için %R%s HELP GLIST ve %R%s HELP DROP 
	yazin.
NICK_GROUP_JOINED
	%s grubuna dahil edildiniz.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY þifreniz
NICK_IDENTIFY_FAILED
	Tanýmlama baþarýsýz.
NICK_IDENTIFY_SUCCEEDED
	Þifre kabul edildi.
NICK_IDENTIFY_EMAIL_REQUIRED
	Þimdi nickiniz için bir e-mail adresi belirtmelisiniz.
	Bu e-mail adresi þifrenizi unutmanýz halinde þifrenizi
	isterken kullanýlacaktýr.
NICK_IDENTIFY_EMAIL_HOWTO
	Mail adresinizi ayarlamak için %R%S SET EMAIL e-mailadresiniz 
	yazýn. Gizliliðiniz korunacaktýr; bu mail adresi herhangi üçüncü 
	þahýslara verilmeyecektir.

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
	Nickinizin çýkýþý yapýldý.
NICK_LOGOUT_X_SUCCEEDED
	%s nickinin çýkýþý yapýldý.

# DROP responses
NICK_LOGOUT_SERVICESADMIN
	Can't logout %s because he's a services administrator.

# DROP responses
NICK_DROP_DISABLED
	Nick kaydý silinmesi geçici olarak devre dýþýdýr.
NICK_DROPPED
	Nickinizin kaydý silindi (dropped).
NICK_X_DROPPED
	%s nickinin kaydý silindi (dropped).

# SET responses
NICK_SET_SYNTAX
	SET özellik parametre
NICK_SET_SERVADMIN_SYNTAX
	SET [nick] özellik parametre
NICK_SET_DISABLED
	NickServ'ün SET komutu geçici olarak devre dýþýdýr.
NICK_SET_UNKNOWN_OPTION
	Varolmayan SET özelliði: %s.

# SET DISPLAY responses
NICK_SET_OPTION_DISABLED
	Option %s cannot be set on this network.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	Yeni gösterim nick grubunuzdan bir nick olmalýdýr!
NICK_SET_DISPLAY_CHANGED
	Yeni gösterim %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Þifre deðiþtirilemedi.
NICK_SET_PASSWORD_CHANGED
	Þifreniz deðiþtirildi.
NICK_SET_PASSWORD_CHANGED_TO
	Nick þifreniz %s olarak deðiþtirildi.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE numara
NICK_SET_LANGUAGE_UNKNOWN
	Varolmayan dil numarasý %d.  Dil listesi için %R%s HELP SET LANGUAGE yazýn.
NICK_SET_LANGUAGE_CHANGED
	Dil Türkçe olarak deðiþtirildi.

# SET URL responses
NICK_SET_URL_CHANGED
	URL adresi %s olarak deðiþtirildi.
NICK_SET_URL_UNSET
	URL adresi silindi.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	E-mail adresi %s olarak deðiþtirildi.
NICK_SET_EMAIL_UNSET
	E-mail adresi silindi.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	E-mail adresini bu networkte silemezsiniz.

# SET ICQ responses
NICK_SET_ICQ_CHANGED
	ICQ numarasý %s olarak deðiþtirildi.
NICK_SET_ICQ_UNSET
	ICQ numarasý silindi.
NICK_SET_ICQ_INVALID
	%s geçerli bir numara deðil.
	
# SET GREET responses
NICK_SET_GREET_CHANGED
	Karþýlama mesajý %s olarak deðiþtirildi.
NICK_SET_GREET_UNSET
	Karþýlama mesajý silindi.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Kill korumasý: AKTÝF.
NICK_SET_KILL_QUICK
	Kill korumasý: zaman limitli olarak AKTÝF.
NICK_SET_KILL_IMMED
	Anýnda kill korumasý: AKTÝF.
NICK_SET_KILL_IMMED_DISABLED
	HEMEN seçeneði bu networkte aktif deðil.
NICK_SET_KILL_OFF
	Kill korumasý: KAPALI.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Güvenlik seçeneði: AKTÝF.
NICK_SET_SECURE_OFF
	Güvenlik seçeneði: KAPALI.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Özel seçeneði: AKTÝF.
NICK_SET_PRIVATE_OFF
	Özel seçeneði: KAPALI.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Artýk %s INFO da e-mail adresiniz gözükmeyecek.
NICK_SET_HIDE_EMAIL_OFF
	Artýk %s INFO da e-mail adresiniz gözükecek.
NICK_SET_HIDE_MASK_ON
	Artýk son göründüðünüz user@host maskýnýz %s INFO da gözükmeyecek.
NICK_SET_HIDE_MASK_OFF
	Artýk son göründüðünüz user@host maskýnýz %s INFO da gözükecek.
NICK_SET_HIDE_QUIT_ON
	Artýk son çýkýþ mesajýnýz(/quit) %s INFO da gözükmeyecek.
NICK_SET_HIDE_QUIT_OFF
	Artýk son çýkýþ mesajýnýz(/quit) %s INFO da gözükecek.

# SET NOEXPIRE responses
NICK_SET_HIDE_STATUS_ON
	Your services access status will now be hidden from %s INFO displays.
NICK_SET_HIDE_STATUS_OFF
	Your services access status will now be shown in %s INFO displays.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
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
	Language changed to Turkish.


# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [mask]
NICK_ACCESS_ALREADY_PRESENT
	%s access listenizde zaten var.
NICK_ACCESS_REACHED_LIMIT
	Bir nick için sadece %d tane access girilebilir.
NICK_ACCESS_ADDED
	%s access listenize eklendi.
NICK_ACCESS_NOT_FOUND
	%s access listenizde bulunamadý.
NICK_ACCESS_DELETED
	%s access listenizden silindi.
NICK_ACCESS_LIST
	Access listesi:
NICK_ACCESS_LIST_X
	%s nickinin access listesi:
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
	   %s                isim: %s
NICK_INFO_SERVICES_OPER
	      %s servis operatörüdür.
NICK_INFO_SERVICES_ADMIN
	      %s servis adminidir.
NICK_INFO_SERVICES_ROOT
	%s is a services root administrator.
NICK_INFO_ADDRESS
	      Son görüldüðü adres: %s
NICK_INFO_ADDRESS_ONLINE
	      Baðlandýðý adres: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	      %s þu an baðlý.
NICK_INFO_TIME_REGGED
	          Kayýt zamaný: %s
NICK_INFO_LAST_SEEN
	   Son görüldüðü tarih: %s
NICK_INFO_LAST_QUIT
	      Son çýkýþ mesajý: %s
NICK_INFO_URL
	                   URL: %s
NICK_INFO_EMAIL
	         E-mail adresi: %s
NICK_INFO_VHOST
	                 vhost: %s
NICK_INFO_VHOST2
	            	vhost: %s@%s
NICK_INFO_ICQ
	                 ICQ #: %d
NICK_INFO_GREET
	      Karþýlama mesajý: %s
NICK_INFO_OPTIONS
	            Özellikler: %s
NICK_INFO_EXPIRE
	Expires in: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Kill korumasý
NICK_INFO_OPT_SECURE
	Güvenlik
NICK_INFO_OPT_PRIVATE
	Özel
NICK_INFO_OPT_MSG
	Message mode
NICK_INFO_OPT_AUTOOP
	Auto-op
NICK_INFO_OPT_NONE
	YOK
NICK_INFO_NO_EXPIRE
	Bu nick zaman aþýmýna uðramayacak.
NICK_INFO_FOR_MORE
	Geniþ bilgi için, %R%s INFO %s ALL yazýn.

# LIST responses
NICK_INFO_SUSPENDED
	This nickname is currently suspended, reason: %s
NICK_INFO_SUSPENDED_NO_REASON
	This nickname is currently suspended

# LIST responses
NICK_LIST_SYNTAX
	LIST model
NICK_LIST_SERVADMIN_SYNTAX
	LIST model [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	%s içeren kayýtlarýn listesi:
NICK_LIST_RESULTS
	Liste sonu - %d/%d .

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
	Liste sonu - %d/%d .

# GLIST responses
NICK_GLIST_HEADER
	Grubunuzdaki nicklerin listesi:
NICK_GLIST_HEADER_X
	%s grubundaki nicklerin listesi:
NICK_GLIST_FOOTER
	Grupta %d nick var.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (expires in %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nick [þifre]
NICK_NO_RECOVER_SELF
	Kendizi recover edemezsiniz!
NICK_RECOVERED
	User claiming your nick has been killed.
	%R%s RELEASE %s to get it back before %s timeout.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nick [þifre]
NICK_RELEASE_NOT_HELD
	%s nicki zaten serbest.
NICK_RELEASED
	Nickiniz serbest býrakýldý.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nick [þifre]
NICK_NO_GHOST_SELF
	Kendinizi ghost edemezsiniz!
NICK_GHOST_KILLED
	Asýlý kalan nickiniz kill lendi.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nick
NICK_GETPASS_UNAVAILABLE
	GETPASS komutu devre dýþý çünkü þifreleme(encryption) devrede.
NICK_GETPASS_PASSWORD_IS
	%s nickinin þifresi %s.

# SENDPASS responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host No WildCards!!
NICK_GETEMAIL_EMAILS_ARE
	Emails Match %s to %s.
NICK_GETEMAIL_NOT_USED
	No Emails listed for %s.
	
# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS nick
NICK_SENDPASS_UNAVAILABLE
	SENDPASS komutu devre dýþý çünkü þifreleme(encryption) devrede.
NICK_SENDPASS_SUBJECT
	Nick þifresi (%s)
NICK_SENDPASS_HEAD
	Selam,
NICK_SENDPASS_LINE_1
	%s nickinizin þifresinin e-mail adresinize yollanmasýný istemiþsiniz.
NICK_SENDPASS_LINE_2
	Nickinizin Þifresi %s Güvenlik nedenlerinden dolayý, bu maili aldýktan sonra deðiþtirseniz iyi olur.
NICK_SENDPASS_LINE_3
	Eðer bu mailin neden size gönderildiðini bilmiyorsanýz, maili ciddiye almayýn.
NICK_SENDPASS_LINE_4
	LÜTFEN BU MAÝLE CEVAP VERMEYÝN!
NICK_SENDPASS_LINE_5
	%s yönetimi.
NICK_SENDPASS_OK
	%s nickinin þifresi gönderildi.

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
	FORBID nick [sebep]
NICK_FORBID_SYNTAX_REASON
	FORBID nick sebep
NICK_FORBID_SUCCEEDED
	%s nickinin Kullanýmý yasaklandý.
NICK_FORBID_FAILED
	%s nickinin Kullanýmý yasaklanamadý!

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
	Kanala giriþte ChanServ tarafýndan otomatik op (+o)
CHAN_LEVEL_AUTOVOICE
	Kanala giriþte ChanServ tarafýndan otomatik voice (+v)
CHAN_LEVEL_AUTOHALFOP
	Kanala giriþte ChanServ tarafýndan otomatik yarým op (+h)
CHAN_LEVEL_AUTOPROTECT
	Kanala giriþte ChanServ tarafýndan otomatik koruma (+a)
CHAN_LEVEL_AUTODEOP
	Kanal içinde op olmasýný yasaklar.
CHAN_LEVEL_NOJOIN
	RESTRICTED özelliðinin aktif olmasý halinde kanala giremez.
CHAN_LEVEL_INVITE
	INVITE komutunu kullanmaya izinlidir.
CHAN_LEVEL_AKICK
	AKICK komutunu kullanmaya izinlidir.
CHAN_LEVEL_SET
	SET komutunu (FOUNDER/PASSWORD hariç) kullanmaya izinlidir.
CHAN_LEVEL_CLEAR
	CLEAR komutunu kullanmaya izinlidir.
CHAN_LEVEL_UNBAN
	UNBAN komutunu kullanmaya izinlidir.
CHAN_LEVEL_OPDEOP
	OP/DEOP komutlarýný kullanmaya izinlidir.
CHAN_LEVEL_ACCESS_LIST
	Kanalýn Access listesini görüntülemeye izinlidir.
CHAN_LEVEL_ACCESS_CHANGE
	Kanalýn Access listesinde deðiþiklik yapmaya izinlidir.
CHAN_LEVEL_MEMO
	Kanala gelen mesajlari listelemeye/okumaya izinlidir.
CHAN_LEVEL_ASSIGN
	Botun assign/unassign komutlarýný kullanmaya izinlidir.
CHAN_LEVEL_BADWORDS
	BADWORDS komutunu kullanmaya izinlidir.
CHAN_LEVEL_NOKICK
	Bot tarafýndan kicklenmemeyi saðlar.
CHAN_LEVEL_FANTASIA
	Kanalda fantazi komutlarýný kullanmaya izinlidir. 
CHAN_LEVEL_SAY
	SAY ve ACT komutlarýný kullanmaya izinlidir.
CHAN_LEVEL_GREET
	Karþýlama mesajý alabilir.
CHAN_LEVEL_VOICEME
	Kendini (de)voice etmeye izinlidir.
CHAN_LEVEL_VOICE
	VOICE/DEVOICE komutlarýný kullanmaya izinlidir.
CHAN_LEVEL_GETKEY
	GETKEY komutunu kullanmaya izinlidir.
CHAN_LEVEL_OPDEOPME
	Kendini (de)op etmeye izinlidir.
CHAN_LEVEL_HALFOPME
	Kendini (de)halfop etmeye izinlidir.
CHAN_LEVEL_HALFOP
	HALFOP/DEHALFOP komutlarýný kullanmaya izinlidir.
CHAN_LEVEL_PROTECTME
	Kendini (de)protect etmeye izinlidir.
CHAN_LEVEL_PROTECT
	PROTECT/DEPROTECT komutlarýný kullanmaya izinlidir.
CHAN_LEVEL_KICKME
	Kendini atmaya izinlidir.
CHAN_LEVEL_KICK
	KICK komutunu kullanmaya izinlidir.
CHAN_LEVEL_SIGNKICK
	SIGNKICK LEVEL kullanýldýðýnda iþaretli atma yapýlmaz.
CHAN_LEVEL_BANME
	Kendini banlamaya yetkili
CHAN_LEVEL_BAN
	BAN komutunu kullanmaya yetkili
CHAN_LEVEL_TOPIC
	TOPIC komutunu kullanmaya yetkili
CHAN_LEVEL_INFO
	INFO komutunu ALL seçenegiyle kullanmaya yetkili

# Automatic responses
CHAN_IS_REGISTERED
	Bu kanal %s ile kayýtlýdýr.
CHAN_NOT_ALLOWED_OP
	%s kanalýnda op olmaya yetkiniz yok.
CHAN_MAY_NOT_BE_USED
	Bu kanal kullanýlamaz.
CHAN_NOT_ALLOWED_TO_JOIN
	Bu kanalda bulunmanýza müsaade yok.

# REGISTER responses
CHAN_X_INVALID
	Channel %s is not a valid channel.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER kanaladý þifre açýklama
CHAN_REGISTER_DISABLED
	Kanal kaydý geçici olarak devre dýþýdýr.
CHAN_REGISTER_NOT_LOCAL
	Local kanallar kayýt edilemezler.
CHAN_MUST_REGISTER_NICK
	Önce nickinizi kayýt etmeniz gerekiyor.  Nick kayýt hakkýnda bilgi için %R%s HELP yazýnýz.
CHAN_MUST_IDENTIFY_NICK
	Lütfen önce %s ile bu komutu kullanarak nickinizi tanýtýn:
	%R%s IDENTIFY þifreniz
CHAN_MAY_NOT_BE_REGISTERED
	%s kanalý kayýt edilemez.
CHAN_ALREADY_REGISTERED
	%s kanalý zaten kayýtlý!
CHAN_MUST_BE_CHANOP
	Kanal kayýt edebilmeniz için kayýt edilmemiþ bir kanala ilk girip op olmanýz gerekmektedir.
CHAN_REACHED_CHANNEL_LIMIT
	%d kanallýk limitinizi doldurdunuz.
CHAN_EXCEEDED_CHANNEL_LIMIT
	%d kanallýk limitinizi aþtýnýz.
CHAN_REGISTRATION_FAILED
	Kanal kaydý baþarýsýz.
CHAN_REGISTERED
	%s kanalý bu nicke kayýt edildi: %s
CHAN_PASSWORD_IS
	Kanal þifreniz %s - ilerde kullanacaðýnýz için unutmayýnýz ve kimseye söylemeyiniz.
CHAN_REGISTER_NONE_CHANNEL
	You have attempted to register a nonexistent channel %s
CHAN_SYMBOL_REQUIRED
	Please use the symbol of # when attempting to register


# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY kanaladý þifreniz
CHAN_IDENTIFY_FAILED
	Geçersiz þifre.
CHAN_IDENTIFY_SUCCEEDED
	Þifre kabul edildi - %s kanalýnda founder seviyesinde eriþim kazandýnýz.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT kanaladý nick
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT kanaladý [nick]
CHAN_LOGOUT_SUCCEEDED
	Kullanýcý %s artýk %s kanalý tarafýndan tanýnmýyor.
CHAN_LOGOUT_ALL_SUCCEEDED
	Tüm tanýmlý kullanýcýlar artýk %s kanalý tarafýndan tanýnmýyor.

# DROP responses
CHAN_DROP_SYNTAX
	DROP kanaladý
CHAN_DROP_DISABLED
	Kanal kaydýnýn silinmesi(DROP) geçici olarak devre dýþýdýr.
CHAN_DROPPED
	%s kanalýnýn kaydý silinmiþtir (dropped).

# SET responses
CHAN_SET_SYNTAX
	SET kanaladý özellik parametre
CHAN_SET_DISABLED
	Chanserv'ün SET komutu geçici olarak devre dýþýdýr.
CHAN_SET_UNKNOWN_OPTION
	Varolmayan SET özelliði: %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s çok fazla kanala sahip.
CHAN_FOUNDER_CHANGED
	%s kanalýnýn founderý %s olarak deðiþtirildi.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	%s için successor %s olarak deðiþtirildi.
CHAN_SUCCESSOR_UNSET
	%s için successor kaldýrýldý.
CHAN_SUCCESSOR_IS_FOUNDER
	%s %s kanalýnda successor olamaz çünkü zaten o kanalýn founderý.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Þifre deðiþimi baþarýsýz.
CHAN_PASSWORD_CHANGED
	%s kanalýnýn þifresi deðiþtirildi.
CHAN_PASSWORD_CHANGED_TO
	%s kanalýnýn þifresi %s olarak deðiþtirildi.

# SET DESC responses
CHAN_DESC_CHANGED
	%s kanalýnýn açýklamasý %s olarak deðiþtirildi.

# SET URL responses
CHAN_URL_CHANGED
	%s kanalýnýn URL adresi %s olarak deðiþtirildi.
CHAN_URL_UNSET
	%s kanalýnýn URL adresi kaldýrýldý.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	%s kanalýnýn e-mail adresi %s olarak deðiþtirildi.
CHAN_EMAIL_UNSET
	%s kanalýnýn e-mail adresi kaldýrýldý.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	%s kanalýnýn giriþ mesajý deðiþtirildi.
CHAN_ENTRY_MSG_UNSET
	%s kanalýnýn giriþ mesajý kaldýrýldý.
	
# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s geçerli bir ban tipi deðil.
CHAN_SET_BANTYPE_CHANGED
	%s kanalýnýn ban tipi #%d olarak deðiþtirildi.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Varolmayan mod karakteri %c yoksayýldý.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	%c modunu kitleyemezsiniz, yoksayildi.
CHAN_SET_MLOCK_L_REQUIRED
	+L modunu kullanabilmek için +l modunuda kullanmalisiniz.
CHAN_SET_MLOCK_K_REQUIRED
	+K modunu kullanabilmek için +i modunuda kullanmalisiniz.
CHAN_MLOCK_CHANGED
	%s kanalýnýn mod kilidi %s olarak deðiþtirildi.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET kanaladý KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Topic retention option for %s is now ON.
CHAN_SET_KEEPTOPIC_OFF
	Topic retention option for %s is now OFF.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET kanaladý TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Topic lock option for %s is now ON.
CHAN_SET_TOPICLOCK_OFF
	Topic lock option for %s is now OFF.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET kanaladi PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Peace option for %s is now ON.
CHAN_SET_PEACE_OFF
	Peace option for %s is now OFF.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET kanaladý PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Private option for %s is now ON.
CHAN_SET_PRIVATE_OFF
	Private option for %s is now OFF.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET kanaladý SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Secure ops option for %s is now ON.
CHAN_SET_SECUREOPS_OFF
	Secure ops option for %s is now OFF.

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET kanaladý SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Secure founder option for %s is now ON.
CHAN_SET_SECUREFOUNDER_OFF
	Secure founder option for %s is now OFF.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET kanaladý RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Restricted access option for %s is now ON.
CHAN_SET_RESTRICTED_OFF
	Restricted access option for %s is now OFF.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET kanal SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Secure option for %s is now ON.
CHAN_SET_SECURE_OFF
	Secure option for %s is now OFF.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET kanaladý SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Signed kick option for %s is now ON.
CHAN_SET_SIGNKICK_LEVEL
	Signed kick option for %s is now ON, but depends of the
	level of the user that is using the command.
CHAN_SET_SIGNKICK_OFF
	Signed kick option for %s is now OFF.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET kanaladý OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice option for %s is now ON.
CHAN_SET_OPNOTICE_OFF
	Op-notice option for %s is now OFF.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET kanaladi XOP {ON | OFF}
CHAN_SET_XOP_ON
	xOP lists system for %s is now ON.
CHAN_SET_XOP_OFF
	xOP lists system for %s is now OFF.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET kanaladý NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	%s kanalý zaman aþýmýndan çýkarýldý.
CHAN_SET_NOEXPIRE_OFF
	%s kanalý zaman aþýmýna eklendi.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Bir kanala toplam %d AOP/SOP/VOP kayit edilebilir.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	Bu komutu kullanamazsiniz. Yerine ACCESS komutunu kullanin.
	Daha fazla bilgi için %R%s HELP ACCESS yazin.
CHAN_XOP_NOT_AVAILABLE
	xOP system is not available.

# AOP messages
CHAN_AOP_SYNTAX
	AOP kanal {ADD|DEL|LIST|CLEAR} [nick | liste-no]
CHAN_AOP_DISABLED
	Kanal AOP listesi degisiklikleri geçici olarak devre disidir.
CHAN_AOP_NICKS_ONLY
	Kanal AOP listeleri sadece kayitli nicklerden olusabilir.
CHAN_AOP_ADDED
	%s %s kanalinin AOP listesine eklenmistir.
CHAN_AOP_MOVED
	%s moved to %s AOP list.
CHAN_AOP_NO_SUCH_ENTRY
	(#%d) %s kanalinin AOP listesinde böyle bir kayit bulunamadi.
CHAN_AOP_NOT_FOUND
	%s %s kanali AOP listesinde bulunamadi.
CHAN_AOP_NO_MATCH
	%s kanali AOP listesinde uyusan kayit bulunamadi.
CHAN_AOP_DELETED
	%s %s kanali AOP listesinden silindi.
CHAN_AOP_DELETED_ONE
	%s kanalinin AOP listesinden 1 kayit silindi.
CHAN_AOP_DELETED_SEVERAL
	Deleted %d entries from %s AOP list.
CHAN_AOP_LIST_EMPTY
	%s kanalinin AOP listesi bos.
CHAN_AOP_LIST_HEADER
	%s kanali AOP listesi:
	  Num  Nick
CHAN_AOP_CLEAR
	Kanalin %s AOP listesi temizlendi.

# HOP messages
CHAN_HOP_SYNTAX
	HOP kanaladi {ADD|DEL|LIST|CLEAR} [nick | liste-no]
CHAN_HOP_DISABLED
	Kanal HOP listesi degisiklikleri geçici olarak devre disidir.
CHAN_HOP_NICKS_ONLY
	Kanal HOP listeleri sadece kayitli nicklerden olusabilir.
CHAN_HOP_ADDED
	%s %s kanali HOP listesine eklendi.
CHAN_HOP_MOVED
	%s moved to %s HOP list.
CHAN_HOP_NO_SUCH_ENTRY
	(#%d) %s kanali HOP listesinde uyusan kayit bulunamadi.
CHAN_HOP_NOT_FOUND
	%s %s kanali HOP listesinde bulunamadi.
CHAN_HOP_NO_MATCH
	%s kanali HOP listesinde uyusan kayit bulunamadi.
CHAN_HOP_DELETED
	%s %s kanali HOP listesinden silindi.
CHAN_HOP_DELETED_ONE
	%s kanali HOP listesinden 1 kayit silindi.
CHAN_HOP_DELETED_SEVERAL
	Deleted %d entries from %s HOP list.
CHAN_HOP_LIST_EMPTY
	%s kanali HOP listesi bos.
CHAN_HOP_LIST_HEADER
	%s kanali HOP listesi:
	  Num  Nick
CHAN_HOP_CLEAR
	Kanal %s HOP listesi temizlendi.

# SOP messages
CHAN_SOP_SYNTAX
	SOP kanaladi {ADD|DEL|LIST|CLEAR} [nick | liste-no]
CHAN_SOP_DISABLED
	Kanal SOP listesi degisikligi geçici olarak devre disidir.
CHAN_SOP_NICKS_ONLY
	Kanal SOP listesi sadece kayitli nicklerden olusabilir.
CHAN_SOP_ADDED
	%s %s kanali SOP listesine eklendi.
CHAN_SOP_MOVED
	%s moved to %s SOP list.
CHAN_SOP_NO_SUCH_ENTRY
	(#%d) %s kanali SOP listesinde uyusan kayit bulunamadi.
CHAN_SOP_NOT_FOUND
	%s %s kanali SOP listesinde bulunamadi.
CHAN_SOP_NO_MATCH
	%s kanali SOP listesinde uyusan kayit bulunamadi.
CHAN_SOP_DELETED
	%s %s kanali SOP listesinden silindi.
CHAN_SOP_DELETED_ONE
	%s kanali SOP listesinden 1 kayit silindi.
CHAN_SOP_DELETED_SEVERAL
	Deleted %d entries from %s SOP list.
CHAN_SOP_LIST_EMPTY
	%s kanali SOP listesi bos.
CHAN_SOP_LIST_HEADER
	%s kanali SOP listesi:
	  Num  Nick
CHAN_SOP_CLEAR
	Kanal %s SOP listesi temizlendi.

# VOP messages
CHAN_VOP_SYNTAX
	VOP kanaladi {ADD|DEL|LIST|CLEAR} [nick | liste-no]
CHAN_VOP_DISABLED
	Kanal VOP listesi degisikligi geçici olarak devre disidir.
CHAN_VOP_NICKS_ONLY
	Kanal VOP listeleri sadece kayitli nicklerden olusabilir.
CHAN_VOP_ADDED
	%s %s kanali VOP listesine eklendi.
CHAN_VOP_MOVED
	%s moved to %s VOP list.
CHAN_VOP_NO_SUCH_ENTRY
	(#%d) %s kanali VOP listesinde uyusan kayit bulunamadi.
CHAN_VOP_NOT_FOUND
	%s %s kanali VOP listesinde bulunamadi.
CHAN_VOP_NO_MATCH
	%s kanali VOP listesinde uyusan kayit bulunamadi.
CHAN_VOP_DELETED
	%s %s kanali VOP listesinden silindi.
CHAN_VOP_DELETED_ONE
	%s kanali VOP listesinden 1 kayit silindi.
CHAN_VOP_DELETED_SEVERAL
	Silindi %d VOP listesinden kayit %s kanali.
CHAN_VOP_LIST_EMPTY
	%s kanali VOP listesi bos.
CHAN_VOP_LIST_HEADER
	%s kanali VOP listesi:
	  Num  Nick
CHAN_VOP_CLEAR
	Kanal %s VOP listesi temizlendi.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS kanaladý {ADD|DEL|LIST|CLEAR} [nick [seviye] | kayýt-listesi]
CHAN_ACCESS_XOP
	Bu komutu kullanamazsiniz. 
	Yerine AOP, SOP veya VOP komutlarini kullanin.
	Daha fazla bilgi için %R%s HELP komut yazin.
CHAN_ACCESS_XOP_HOP
	You can't use this command. 
	Use the AOP, SOP, HOP and VOP commands instead.
	Type %R%s HELP command for more information.
CHAN_ACCESS_DISABLED
	Kanal access listesinin deðiþtirilmesi geçici olarak devre dýþýdýr.
CHAN_ACCESS_LEVEL_NONZERO
	Access seviyesi sýfýrdan farklý bir sayý olmalýdýr.
CHAN_ACCESS_LEVEL_RANGE
	Access seviyesi %d ve %d arasýnda olmalýdýr.
CHAN_ACCESS_NICKS_ONLY
	Kanal access listesi sadece kayýtlý kullanýcýlarý içermelidir.
CHAN_ACCESS_REACHED_LIMIT
	Kanalda sadece %d tane access kaydý olabilir.
CHAN_ACCESS_LEVEL_UNCHANGED
	%s nickinin access seviyesi %s kanalýnda zaten %d.
CHAN_ACCESS_LEVEL_CHANGED
	%s nickinin access seviyesi %s kanalýnda %d olarak deðiþtirildi.
CHAN_ACCESS_ADDED
	%s nicki %s kanalinin access listesine %d seviyeden eklendi.
CHAN_ACCESS_NO_SUCH_ENTRY
	(#%d) þeklinde bir kayýt %s kanalýnýn access listesinde bulunamadý.
CHAN_ACCESS_NOT_FOUND
	%s %s kanalýnýn access listesinde bulunamadý.
CHAN_ACCESS_NO_MATCH
	Uyuþan kayýt %s kanalýnýn access listesinde yok.
CHAN_ACCESS_DELETED
	%s %s kanalýnýn access listesinden silindi.
CHAN_ACCESS_DELETED_ONE
	%s kanalýnýn access listesinden 1 kayýt silindi.
CHAN_ACCESS_DELETED_SEVERAL
	Deleted %d entries from %s access list.
CHAN_ACCESS_LIST_EMPTY
	%s kanalýnýn access listesi boþ.
CHAN_ACCESS_LIST_HEADER
	%s kanalý için access listesi:
	  Sýra   Seviye  Nick
CHAN_ACCESS_LIST_FOOTER
	access listenin sonu.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	Kanal %s access listesi temizlendi.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK kanaladý {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [nick-veya-usermask] [sebep]
CHAN_AKICK_DISABLED
	Kanal Akick listesinin deðiþtirilmesi geçici olarak devre dýþýdýr.
CHAN_AKICK_ALREADY_EXISTS
	%s nicki %s kanalýnýn akick listesinde zaten var.
CHAN_AKICK_REACHED_LIMIT
	Bir kanalýn akick listesinde en fazla %d kiþi olabilir.
CHAN_AKICK_ADDED
	%s nicki %s kanalýnýn akick listesine eklendi.
CHAN_AKICK_NO_SUCH_ENTRY
	(#%d) þeklinde bir kayýt %s kanalýnýn akick listesinde bulunamadý.
CHAN_AKICK_NOT_FOUND
	%s %s kanalýnýn akick listesinde bulunamadý.
CHAN_AKICK_NO_MATCH
	Uyuþan kayýt %s kanalýnýn akick listesinde bulunamadý.
CHAN_AKICK_STUCK
	%s artik %s kanalinda sürekli aktif.
CHAN_AKICK_UNSTUCK
	%s artik %s kanalinda sürekli aktif degil.
CHAN_AKICK_DELETED
	%s %s kanalýnýn akick listesinden silindi.
CHAN_AKICK_DELETED_ONE
	%s kanalýnýn akick listesinden 1 kayýt silindi.
CHAN_AKICK_DELETED_SEVERAL
	Deleted %d entries from %s autokick list.
CHAN_AKICK_LIST_EMPTY
	%s kanalýnýn akick listesi boþ.
CHAN_AKICK_LIST_HEADER
	%s kanalýnýn akick listesi:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (by %s on %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (kalici) (by %s on %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	 %s için AKICK uygulamasý tamam; %d kullanýcý için geçerli.
CHAN_AKICK_CLEAR
	Kanal %s akick listesi temizlendi.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS kanaladý {SET | DIS[ABLE] | LIST | RESET} [madde [seviye]]
CHAN_LEVELS_XOP
	Bu modda bu komut kullanilmaz.
CHAN_LEVELS_RANGE
	Level %d ile %d arasýnda olmalýdýr.
CHAN_LEVELS_CHANGED
	%s için level %s kanalýnda %d olarak deðiþtirildi.
CHAN_LEVELS_UNKNOWN
	Varolmayan ayar: %s. Geçerli ayar listesi için %R%s HELP LEVELS DESC yazýnýz.
CHAN_LEVELS_DISABLED
	%s %s kanalýnda iptal edildi.
CHAN_LEVELS_LIST_HEADER
	%s kanalý için access level ayarlarý:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (kapatýldý)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (sadece founder)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	%s için access levelleri varsayýlanlarla deðiþtirildi.

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
	INFO kanaladý [ALL]
CHAN_INFO_HEADER
	%s kanalý için bilgi:
CHAN_INFO_FOUNDER
	        Founder: %s (%s)
CHAN_INFO_NO_FOUNDER
	        Founder: %s
CHAN_INFO_SUCCESSOR
	      Successor: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	      Successor: %s
CHAN_INFO_DESCRIPTION
	       Açýklama: %s
CHAN_INFO_ENTRYMSG
	   Giriþ mesajý: %s
CHAN_INFO_TIME_REGGED
	   Kayýt tarihi: %s
CHAN_INFO_LAST_USED
	 Son kullanýlýþ: %s
CHAN_INFO_LAST_TOPIC
	      Son topic: %s
CHAN_INFO_TOPIC_SET_BY
	   Topici yazan: %s
CHAN_INFO_URL
	            URL: %s
CHAN_INFO_EMAIL
	  E-mail adresi: %s
CHAN_INFO_BANTYPE
	       Ban tipi: %d
CHAN_INFO_OPTIONS
	     Özellikler: %s
CHAN_INFO_OPT_KEEPTOPIC
	Topic hatýrlama
CHAN_INFO_OPT_OPNOTICE
	OP Notu
CHAN_INFO_OPT_PEACE
	Baris
CHAN_INFO_OPT_PRIVATE
	Özel
CHAN_INFO_OPT_RESTRICTED
	Kisitli Erisim.
CHAN_INFO_OPT_SECURE
	Güvenlik
CHAN_INFO_OPT_SECUREOPS
	Secure Ops
CHAN_INFO_OPT_SECUREFOUNDER
	Secure Founder
CHAN_INFO_OPT_SIGNKICK
	Ýþaretli atma
CHAN_INFO_OPT_TOPICLOCK
	Topic kilidi
CHAN_INFO_OPT_XOP
	xOP sistemi
CHAN_INFO_OPT_NONE
	YOK
CHAN_INFO_MODE_LOCK
	     Mod kilidi: %s
CHAN_INFO_EXPIRE
	      Expires in: %s
CHAN_INFO_NO_EXPIRE
	Bu kanal için zaman aþýmý söz konusu deðil.

# LIST responses
CHAN_LIST_SYNTAX
	LIST model
CHAN_LIST_SERVADMIN_SYNTAX
	LIST model [FORBIDDEN] [NOEXPIRE]
CHAN_LIST_HEADER
	%s ile uyuþan kayýt listesi:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Liste sonu - %d/%d uyan kayýt bulundu.

# CLIST responses
CHAN_INVITE_SYNTAX
	INVITE kanaladý

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN kanaladý
CHAN_UNBANNED
	%s kanalýndaki banýnýz kaldýrýldý.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC kanaladi [konu]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR kanaladý neyi
CHAN_CLEARED_BANS
	%s kanalýndaki tüm banlar kaldýrýldý.
CHAN_CLEARED_EXCEPTS
	All excepts on channel %s have been removed.
CHAN_CLEARED_MODES
	%s kanalýndaki tüm modlar resetlendi.
CHAN_CLEARED_OPS
	Mod +o %s kanalýnda temizlendi.
CHAN_CLEARED_HOPS
	Mode +h has been cleared from channel %s.
CHAN_CLEARED_VOICES
	Mod +v %s kanalýnda temizlendi.
CHAN_CLEARED_USERS
	%s kanalýndaki tüm kullanýcýlar atýldý.
CHAN_CLEARED_INVITES
	All invites on channel %s have been removed.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS kanaladý
CHAN_GETPASS_UNAVAILABLE
	GETPASS komutu devre dýþý çünkü þifreleme(encryption) devrede.
CHAN_GETPASS_PASSWORD_IS
	%s kanalýnýn þifresi %s.
	
# SENDPASS responses
CHAN_GETKEY_SYNTAX
	GETKEY channel
CHAN_GETKEY_NOKEY
	The channel %s has no key.
CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS kanaladý
CHAN_SENDPASS_UNAVAILABLE
	SENDPASS komutu devre dýþý çünkü þifreleme(encryption) devrede.
CHAN_SENDPASS_SUBJECT
	Kanal þifresi (%s)
CHAN_SENDPASS_HEAD
	Selam,
CHAN_SENDPASS_LINE_1
	%s kanalýnýzýn þifresinin e-mail adresinize gönderilmesini istemiþsiniz.
CHAN_SENDPASS_LINE_2
	Kanal Þifresi %s. Güvenlik nedenlerinden dolayý, þifrenizi bu maili aldýktan sonra deðiþtirirseniz iyi olur.
CHAN_SENDPASS_LINE_3
	Eðer bu mailin niye size gönderildiðini bilmiyorsanýz, lütfen ciddiye almayýn.
CHAN_SENDPASS_LINE_4
	LÜTFEN BU MAÝLE CEVAP VERMEYÝN!
CHAN_SENDPASS_LINE_5
	%s yönetimi.
CHAN_SENDPASS_OK
	%s kanalýnýn þifresi gönderildi.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID kanaladý [sebep]
CHAN_FORBID_SYNTAX_REASON
	FORBID kanaladý sebep
CHAN_FORBID_SUCCEEDED
	%s kanalýnýn Kullanýmý yasaklandý.
CHAN_FORBID_FAILED
	%s kanalýnýn Kullanýmý yasaklanamadý!

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
	1 yeni mesajýnýz var.
MEMO_HAVE_NEW_MEMOS
	%d yeni mesajýnýz var.
MEMO_TYPE_READ_LAST
	Okumak için %R%s READ LAST yazýn.
MEMO_TYPE_READ_NUM
	Okumak için %R%s READ %d yazýn.
MEMO_TYPE_LIST_NEW
	Listelemek için %R%s LIST NEW yazýn.
MEMO_AT_LIMIT
	Dikkat: Maksimum mesaj sayýnýza ulaþtýnýz (%d).  Þimdiki mesajlarýnýzdan bir kaçýný silmezseniz yeni mesaj alamayacaksýnýz.
MEMO_OVER_LIMIT
	Dikkat: Maksimum mesaj sayýnýzý aþtýnýz (%d).  Þimdiki mesajlarýnýzdan bir kaçýný silmezseniz yeni mesaj alamayacaksýnýz.
MEMO_X_MANY_NOTICE
	There are %d memos on channel %s.
MEMO_X_ONE_NOTICE
	There is %d memo on channel %s.
MEMO_NEW_X_MEMO_ARRIVED
	There is a new memo on channel %s.
	Type %R%s READ %s %d to read it.
MEMO_NEW_MEMO_ARRIVED
	%s nickinden yeni mesajýnýz var.
	Okumak için %R%s READ %d yazýn.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Hiç mesajýnýz yok.
MEMO_X_HAS_NO_MEMOS
	%s nickinin hiç mesajý yok.
MEMO_DOES_NOT_EXIST
	%d numaralý mesaj bulunamadý!
MEMO_LIST_NOT_FOUND
	Uyuþan mesaj bulunamadý.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {nick | kanaladý} mesaj
MEMO_SEND_DISABLED
	Mesaj yollama geçici olarak devre dýþýdýr.
MEMO_SEND_PLEASE_WAIT
	Lütfen %d saniye SEND komutunu tekrar kullanmak için bekleyin.
MEMO_X_GETS_NO_MEMOS
	%s nicki mesaj alamaz.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s nickinin limiti dolduðundan daha fazla mesaj alamaz.
MEMO_SENT
	%s nickine mesajýnýz gönderildi.
	
# CANCEL responses
MEMO_MASS_SENT
	A massmemo has been sent to all registered users.
	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {nick | kanaladý}
MEMO_CANCEL_DISABLED
	Mesaj iptal etme geçici olarak devre dýþýdýr.
MEMO_CANCEL_NONE
	Ýptal edilecek mesaj yoktu.
MEMO_CANCELLED
	%s nickine gönderilen son mesaj iptal edildi.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [kanaladý] [liste | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	Hiç yeni mesajýnýz yok.
MEMO_X_HAS_NO_NEW_MEMOS
	%s nickinin hiç yeni mesajý yok.
MEMO_LIST_MEMOS
	%s nicki için mesajlar.  Okumak için: %R%s READ num yazýn.
MEMO_LIST_NEW_MEMOS
	%s nicki için yeni mesajlar.  Okumak için: %R%s READ num yazýn.
MEMO_LIST_CHAN_MEMOS
	%s kanali için mesajlar.  Okumak için: %R%s READ %s num yazýn.
MEMO_LIST_CHAN_NEW_MEMOS
	%s kanali için yeni mesajlar.  Okumak için: %R%s READ %s num yazýn.
MEMO_LIST_HEADER
	 Num  Gönderen          Tarih/Zaman
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [kanaladý] {liste | LAST | NEW}
MEMO_HEADER
	%d nolu mesaj %s nickinden (%s).  Silmek için: %R%s DEL %d yazýn.
MEMO_CHAN_HEADER
	%d nolu mesaj %s nickinden (%s).  Silmek için: %R%s DEL %s %d yazýn.
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [kanaladý] {num | liste | ALL}
MEMO_DELETED_NONE
	Hiç mesaj silinmedi.
MEMO_DELETED_ONE
	%d nolu mesaj silindi.
MEMO_DELETED_SEVERAL
	%s mesajlarý silindi.
MEMO_DELETED_ALL
	Tüm mesajlarýnýz silindi.

# SET responses
MEMO_CHAN_DELETED_ALL
	All memos for channel %s have been deleted.

# SET responses
MEMO_SET_SYNTAX
	SET özellik parametre
MEMO_SET_DISABLED
	MemoServ'ün SET komutu geçici olarak devre dýþýdýr.
MEMO_SET_UNKNOWN_OPTION
	Varolmayan SET özelliði: %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s sizi yeni mesajlar için baðlandýðýnýzda veya mesajlar size gönderildiði anda uyaracak.
MEMO_SET_NOTIFY_LOGON
	%s sizi yeni mesajlar için baðlandýðýnýzda veya AWAY modundan dönüþte uyaracak.
MEMO_SET_NOTIFY_NEW
	%s sizi yeni mesajlar için size gönderildiði anda uyaracak.
MEMO_SET_NOTIFY_OFF
	%s sizi yeni mesajlar için uyarmayacak.

# SET LIMIT responses
MEMO_SET_NOTIFY_MAIL
	You will now be informed about new memos via email.
MEMO_SET_NOTIFY_NOMAIL
	You will no longer be informed via email.
MEMO_SET_NOTIFY_INVALIDMAIL
	There's no email address set for your nick.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [kanaladý] limit
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [kullanýcý | kanaladý] {limit | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Mesaj limitinizi belirlemeye izinli deðilsiniz.
MEMO_SET_LIMIT_FORBIDDEN
	%s nickinin mesaj limiti deðiþtirelemez.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Mesaj limitinizi %d sayýsýndan daha fazla yapamazsýnýz.
MEMO_SET_LIMIT_TOO_HIGH
	%s nickinin mesaj limitini %d sayýsýndan daha fazla yapamazsýnýz.
MEMO_SET_LIMIT_OVERFLOW
	Mesaj limiti çok fazla; limit %d olarak deðiþtiriliyor.
MEMO_SET_YOUR_LIMIT
	Mesaj limitiniz %d olarak ayarlandý.
MEMO_SET_YOUR_LIMIT_ZERO
	Limitiniz sýfýr olduðundan artýk mesaj alamayacaksýnýz.
MEMO_UNSET_YOUR_LIMIT
	Mesaj limitiniz kapatýldý.
MEMO_SET_LIMIT
	%s nickinin mesaj limiti %d olarak ayarlandý.
MEMO_SET_LIMIT_ZERO
	%s nickinin mesaj limiti 0 olarak ayarlandý.
MEMO_UNSET_LIMIT
	%s nickinin mesaj limiti kapatýldý.

# INFO responses
MEMO_INFO_SYNTAX
	INFO [kanaladý]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [nick | kanaladý]
MEMO_INFO_NO_MEMOS
	Þu an hiç mesajýnýz yok.
MEMO_INFO_MEMO
	Þu an 1 mesajýnýz var.
MEMO_INFO_MEMO_UNREAD
	Þu an 1 mesajýnýz var, ve henüz okunmadý.
MEMO_INFO_MEMOS
	Þu an %d mesajýnýz var.
MEMO_INFO_MEMOS_ONE_UNREAD
	Þu an %d mesajýnýz var, 1 tanesi okunmamýþ.
MEMO_INFO_MEMOS_SOME_UNREAD
	Þu an %d mesajýnýz var, %d tanesi okunmamýþ.
MEMO_INFO_MEMOS_ALL_UNREAD
	Þu an %d mesajýnýz var; hiçbiri okunmamýþ.
MEMO_INFO_LIMIT
	Mesaj limitiniz %d.
MEMO_INFO_HARD_LIMIT
	Mesaj limitiniz %d, ve deðiþtirilemez.
MEMO_INFO_LIMIT_ZERO
	Mesaj limitiniz 0; hiç mesaj alamayacaksýnýz.
MEMO_INFO_HARD_LIMIT_ZERO
	Mesaj limitiniz 0; hiç yeni mesaj alamayacaksýnýz.  Bu limiti deðiþtiremezsiniz.
MEMO_INFO_NO_LIMIT
	Saklayabileceðiniz mesaj sayýsýnda limit yok.
MEMO_INFO_NOTIFY_OFF
	Yeni mesajlar için uyarýlmayacaksýnýz.
MEMO_INFO_NOTIFY_ON
	Yeni mesajlar için baðlandýðýnýzda ve size ulaþtýðýnda uyarýlacaksýnýz.
MEMO_INFO_NOTIFY_RECEIVE
	Yeni mesajlar için ulaþtýklarý anda uyarýlacaksýnýz.
MEMO_INFO_NOTIFY_SIGNON
	Yeni mesajlar için baðlandýðýnýzda uyarýlacaksýnýz.
MEMO_INFO_X_NO_MEMOS
	%s nickinin þu an hiç mesajý yok.
MEMO_INFO_X_MEMO
	%s nickinin þu an 1 mesajý var.
MEMO_INFO_X_MEMO_UNREAD
	%s nickinin þu an 1 mesajý var, ve daha okunmamýþ.
MEMO_INFO_X_MEMOS
	%s nickinin þu an %d mesajý var.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s nickinin þu an %d mesajý var, 1 tanesi okunmamýþ.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s nickinin þu an %d mesajý var, %d tanesi okunmamýþ.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s nickinin þu an %d mesajý var; hiçbiri okunmamýþ.
MEMO_INFO_X_LIMIT
	%s nickinin mesaj limiti %d.
MEMO_INFO_X_HARD_LIMIT
	%s nickinin mesaj limiti %d, ve deðiþtirilemez.
MEMO_INFO_X_NO_LIMIT
	%s nickinin mesaj limiti yok.
MEMO_INFO_X_NOTIFY_OFF
	%s yeni mesajlar için uyarýlmayacak.
MEMO_INFO_X_NOTIFY_ON
	%s yeni mesajlar için baðlandýðýnda ve mesajlar ulaþtýðý anda uyarýlacak.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s yeni mesajlar ulaþtýðý anda uyarýlacak.
MEMO_INFO_X_NOTIFY_SIGNON
	%s yeni mesajlar için baðlandýðýnda uyarýlacak.
	
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
	Bot %s mevcut deðil.
BOT_NOT_ASSIGNED
	Bu komutu kullanmadan önce kanala bir bot sokmalýsýnýz.
	Daha fazla bilgi için %R%S HELP ASSIGN yazýn.
BOT_NOT_ON_CHANNEL
	Bot %s kanalýnda deðil.
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	%s kelimesinin bu kanalda kullanýlmasý yasaktýr!
BOT_REASON_BADWORD_GENTLE
	Konuþmalarýna dikkat et!
BOT_REASON_BOLD
	Bu kanalda kalýn yazý kullanýlmasý yasaktýr!
BOT_REASON_CAPS
	Lütfen büyük harf kullanmayýn!
BOT_REASON_COLOR
	Lütfen bu kanalda renkli yazý kullanmayýn!
BOT_REASON_FLOOD
	Flood yapma!
BOT_REASON_REPEAT
	Kendini tekrar etme!
BOT_REASON_REVERSE
	Lütfen bu kanalda Ctrl+R ile yazýlmýþ yazý kullanmayýn!
BOT_REASON_UNDERLINE
	Lütfen bu kanalda altý çizili yazý kullanmayýn!

# !seen replies
BOT_SEEN_BOT
	Beni buldun, %s!
BOT_SEEN_YOU
	Kendinimi arýyorsun, %s?
BOT_SEEN_ON_CHANNEL
	%s þu anda kanalda!
BOT_SEEN_ON_CHANNEL_AS
	%s þu anda kanalda (%s olarak) ! 
BOT_SEEN_ON
	%s %s önce buradaydý.
BOT_SEEN_NEVER
	%s diye birini bu kanalda hiç görmedim.
BOT_SEEN_UNKNOWN
	%s þahsýnýn kim olduðunu bilmiyorum.

# BOT responses
BOT_BOT_SYNTAX
	BOT ADD nick user host gerçekismi
	BOT CHANGE eskinick yeninick [user [host [gerçekismi]]]
	BOT DEL nick
BOT_BOT_ALREADY_EXISTS
	%s nickli bot zaten var.
BOT_BOT_CREATION_FAILED
	Bot yaratma baþarýsýz.
BOT_BOT_READONLY
	Botta deðiþiklik yapma geçici olarak devre dýþý.
BOT_BOT_ADDED
	%s!%s@%s (%s) bot listesine eklendi.
BOT_BOT_ANY_CHANGES
	Eski bilgiler þimdikilerle ayný.
BOT_BOT_CHANGED
	%s botu %s!%s@%s (%s) olarak deðiþtirildi.
BOT_BOT_DELETED
	%s botu silindi.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Bot listesi:
BOT_BOTLIST_PRIVATE_HEADER
	IRCoplara ayrilmis botlar:
BOT_BOTLIST_FOOTER
	%d bot mevcut.
BOT_BOTLIST_EMPTY
	Þu an mevcut bot yok.
	Server adminine bir tane yaratmasýný söyleyin!
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN kanaladý nick
BOT_ASSIGN_READONLY
	Kanala bot sokma geçici olarak devre dýþý.
BOT_ASSIGN_ALREADY
	Bot %s is already assigned to channel %s.
BOT_ASSIGN_ASSIGNED
	%s botu %s kanalýna sokuldu.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN kanaladý
BOT_UNASSIGN_UNASSIGNED
	%s kanalýndan bot çýkarýldý.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {kanaladý | nick}
BOT_INFO_NOT_FOUND
	%s geçerli bir bot yada kanal deðil.
BOT_INFO_BOT_HEADER
	%s botunun bilgileri:
BOT_INFO_BOT_MASK
	               Mask : %s@%s
BOT_INFO_BOT_REALNAME
	        Gerçek ismi : %s
BOT_INFO_BOT_CREATED
	          Yaratýlýþ : %s
BOT_INFO_BOT_USAGE
	Kullanýlan kanallar : %d kanalda
BOT_INFO_BOT_OPTIONS
	    Ayarlar : %s
BOT_INFO_OPT_PRIVATE
	Özel
BOT_INFO_CHAN_HEADER
	%s kanalýnýn bilgileri:
BOT_INFO_CHAN_BOT
	           Botun nicki : %s
BOT_INFO_CHAN_BOT_NONE
	           Botun nicki : þu an yok.
BOT_INFO_CHAN_KICK_BADWORDS
	          Küfürde atma : %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	          Küfürde atma : %s (%d tanede ban)
BOT_INFO_CHAN_KICK_BOLDS
	     Kalýn yazýda atma : %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	     Kalýn yazýda atma : %s (%d tanede ban)
BOT_INFO_CHAN_KICK_CAPS_ON
	     Büyük yazýda atma : %s (minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	     Büyük yazýda atma : %s (%d tanede ban; minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	     Büyük yazýda atma : %s
BOT_INFO_CHAN_KICK_COLORS
	    Renkli yazýda atma : %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	    Renkli yazýda atma : %s (%d tanede ban)
BOT_INFO_CHAN_KICK_FLOOD_ON
	          Floodda atma : %s (%d sýra %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	          Floodda atma : %s (%d tanede ban; %d sýra %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	          Floodda atma : %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	         Tekrarda atma : %s (%d kez)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	         Tekrarda atma : %s (%d tanede ban; %d kez)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	         Tekrarda atma : %s
BOT_INFO_CHAN_KICK_REVERSES
	        Ctrl+R de atma : %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	        Ctrl+R de atma : %s (%d tanede ban)
BOT_INFO_CHAN_KICK_UNDERLINES
	Altýçizili yazýda atma : %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	Altýçizili yazýda atma : %s (%d tanede ban)
BOT_INFO_ACTIVE
	Aktif
BOT_INFO_INACTIVE
	Kapalý
BOT_INFO_CHAN_OPTIONS
	         Özellikler : %s
BOT_INFO_OPT_DONTKICKOPS
	Oplarý koruma
BOT_INFO_OPT_DONTKICKVOICES
	Voicelarý koruma
BOT_INFO_OPT_FANTASY
	Fantazi
BOT_INFO_OPT_GREET
	Karþýlama
BOT_INFO_OPT_NOBOT
	Bot yok
BOT_INFO_OPT_SYMBIOSIS
	Gerçek bot
BOT_INFO_OPT_NONE
	YOK
	
# SET responses
BOT_SET_SYNTAX
	SET kanaladý özellik ayarlar
BOT_SET_DISABLED
	Bot özellik ayarý geçici olarak devre dýþý.
BOT_SET_UNKNOWN
	Varolmayan özellik %s.
	Daha fazla bilgi için %R%S HELP SET yazýn.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET kanaladý DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Bot %s kanalýnda oplarý atmayacak .
BOT_SET_DONTKICKOPS_OFF
	Bot %s kanalýnda oplarýda atacak.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET kanaladý DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Bot %s kanalýnda voicelarý atmayacak.
BOT_SET_DONTKICKVOICES_OFF
	Bot %s kanalýnda voicelarýda atacak.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET kanaladý FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Fantazi modu %s kanalýnda AKTÝF.
BOT_SET_FANTASY_OFF
	Fantazi modu %s kanalýnda KAPALI.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET kanaladý GREET {ON|OFF}
BOT_SET_GREET_ON
	Karþýlama modu %s kanalýnda AKTÝF.
BOT_SET_GREET_OFF
	Karþýlama modu %s kanalýnda KAPALI.

# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET kanaladý NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Bot yok modu %s kanalý için AKTÝF.
BOT_SET_NOBOT_OFF
	Bot yok modu %s kanalý için KAPALI.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET kanaladi PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	%s botunun özel seçenegi artik AKTIF.
BOT_SET_PRIVATE_OFF
	%s botunun özel seçenegi artik KAPALI.
	
# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET kanaladý SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Gerçek bot modu %s kanalýnda AKTÝF.
BOT_SET_SYMBIOSIS_OFF
	Gerçek bot modu %s kanalýnda KAPALI.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK kanaladý özellik {ON|OFF} [ayarlar]
BOT_KICK_DISABLED
	Atma ayarlarý geçici olarak devre dýþý.
BOT_KICK_UNKNOWN
	Varolmayan özellik %s.
	Daha fazla bilgi için %R%S HELP KICK yazýn.
BOT_KICK_BAD_TTB
	%s deðeri kullanýlamaz.
BOT_KICK_BADWORDS_ON
	Bot þimdi küfürlerde atacak. BADWORDS komutunu kullanarak
	listeye küfür ekleyip silebilirsiniz.
BOT_KICK_BADWORDS_ON_BAN
	Bot þimdi küfürlerde atacak, ve %d atma sonrasýnda 
	ayný kullanýcýyý banlayacak. BADWORDS komutunu
	kullanarak küfür ekleyip çýkarabilirsiniz.
BOT_KICK_BADWORDS_OFF
	Bot artýk küfürlerde atmayacak.
BOT_KICK_BOLDS_ON
	Bot þimdi kalýn yazýda atacak.
BOT_KICK_BOLDS_ON_BAN
	Bot þimdi kalýn yazýda atacak, ve 
	%d atma sonrasýnda ayný kullanýcýyý banlayacak.
BOT_KICK_BOLDS_OFF
	Bot artýk kalýn yazýda atmayacak.
BOT_KICK_CAPS_ON
	Bot þimdi büyük yazýda atacak(Tüm mesajýn en az
	%d karakterini ve %d%% kadarýný içermelidir).
BOT_KICK_CAPS_ON_BAN
	Bot þimdi büyük yazýda atacak(Tüm mesajýn en az
	%d karakterini ve %d%% kadarýný içermelidir), ve 
	%d atma sonrasýnda ayný kullanýcýyý banlayacak.
BOT_KICK_CAPS_OFF
	Bot artýk büyük yazýda atmayacak.
BOT_KICK_COLORS_ON
	Bot þimdi renkli yazýda atacak.
BOT_KICK_COLORS_ON_BAN
	Bot þimdi renkli yazýda atacak, ve 
	%d atma sonrasýnda ayný kullanýcýyý banlayacak.
BOT_KICK_COLORS_OFF
	Bot artýk renkli yazýda atmayacak.
BOT_KICK_FLOOD_ON
	Bot þimdi text floodda atacak(%d sýra %d saniyede).
BOT_KICK_FLOOD_ON_BAN
	Bot þimdi text floodda atacak(%d sira %d saniyede), ve 
	%d atma sonrasýnda ayný kullanýcýyý banlayacak.
BOT_KICK_FLOOD_OFF
	Bot artýk text floodda atmayacak.
BOT_KICK_REPEAT_ON
	Bot þimdi tekrarda atacak(kullanýcý %d kez
	ayný þeyi söylerse).
BOT_KICK_REPEAT_ON_BAN
	Bot þimdi tekrarda atacak(kullanýcý %d kez
	ayný þeyi söylerse), ve %d atma sonrasýnda
	ayný kullanýcýyý banlayacak.
BOT_KICK_REPEAT_OFF
	Bot artýk tekrarda atmayacak.
BOT_KICK_REVERSES_ON
	Bot þimdi Ctrl+R de atacak.
BOT_KICK_REVERSES_ON_BAN
	Bot þimdi Ctrl+R de atacak, ve %d atma sonrasýnda 
	ayný kullanýcýyý banlayacak.
BOT_KICK_REVERSES_OFF
	Bot artýk Ctrl+R de atmayacak.
BOT_KICK_UNDERLINES_ON
	Bot þimdi altýçizili yazýda atacak.
BOT_KICK_UNDERLINES_ON_BAN
	Bot þimdi altýçizili yazýda atacak, ve %d atma 
	sonrasýnda ayný kullanýcýyý banlayacak.
BOT_KICK_UNDERLINES_OFF
	Bot artýk altýçizili yazýda atmayacak.
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS kanaladý {ADD|DEL|LIST|CLEAR} [nick | kayýt-listesi]
BOT_BADWORDS_DISABLED
	Kanalýn küfür listesinde deðiþiklik yapma geçici olarak devre dýþýdýr.
BOT_BADWORDS_REACHED_LIMIT
	Bir kanalda sadece %d tane küfür kaydý olabilir.
BOT_BADWORDS_ALREADY_EXISTS
	%s zaten %s kanalýnýn küfür listesinde var.
BOT_BADWORDS_ADDED
	%s %s kanalýnýn küfür listesine eklendi.
BOT_BADWORDS_NO_SUCH_ENTRY
	(#%d) þeklinde bir kayýt %s kanalýnýn küfür listesinde bulunamadý.
BOT_BADWORDS_NOT_FOUND
	%s %s kanalýnýn küfür listesinde bulunamadý.
BOT_BADWORDS_NO_MATCH
	Uyuþan kayýt %s kanalýnýn küfür listesinde bulunamadý.
BOT_BADWORDS_DELETED
	%s %s kanalýnýn küfür listesinden çýkarýldý.
BOT_BADWORDS_DELETED_ONE
	%s kanalýnýn küfür listesinden 1 kayýt silindi.
BOT_BADWORDS_DELETED_SEVERAL
	Deleted %d entries from %s bad words list.
BOT_BADWORDS_LIST_EMPTY
	%s kanalýnýn küfür listesi boþ.
BOT_BADWORDS_LIST_HEADER
	%s kanalýnýn küfür listesi:
	  Sayý   Kelime                           Tipi
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	Küfür listesi þimdi boþ.

# SAY responses
BOT_SAY_SYNTAX
	SAY kanaladý yazý

# ACT responses
BOT_ACT_SYNTAX
	ACT kanaladý yazý

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
	Servisler mod deðiþtiremiyor.  Serverin doðru ayarlandýðýna emin olun.
OPER_BOUNCY_MODES_U_LINE
	Servisler mod deðiþtiremiyor.  Serverin U:line larýnýn doðru ayarlandýðýna emin olun.

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL mesaj

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Varolmayan STATS özelliði: %s.
OPER_STATS_CURRENT_USERS
	Þu anki kullanýcý sayýsý: %d (%d op)
OPER_STATS_MAX_USERS
	Maximum kullanýcý sayýsý: %d (%s)
# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Servisler %d gündür çalýþýyor, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Servisler %d gündür çalýþýyor, %02d:%02d
OPER_STATS_UPTIME_HMS
	Servisler %d saat, %d dakikadýr çalýþýyor.
OPER_STATS_UPTIME_HM1S
	Servisler %d saat, %d dakikadýr çalýþýyor.
OPER_STATS_UPTIME_H1MS
	Servisler %d saat, %d dakikadýr çalýþýyor.
OPER_STATS_UPTIME_H1M1S
	Servisler %d saat, %d dakikadýr çalýþýyor.
OPER_STATS_UPTIME_1HMS
	Servisler %d saat, %d dakikadýr çalýþýyor.
OPER_STATS_UPTIME_1HM1S
	Servisler %d saat, %d dakikadýr çalýþýyor.
OPER_STATS_UPTIME_1H1MS
	Servisler %d saat, %d dakikadýr çalýþýyor.
OPER_STATS_UPTIME_1H1M1S
	Servisler %d saat, %d dakikadýr çalýþýyor.
OPER_STATS_UPTIME_MS
	Servisler %d dakika, %d saniyedir çalýþýyor.
OPER_STATS_UPTIME_M1S
	Servisler %d dakika, %d saniyedir çalýþýyor.
OPER_STATS_UPTIME_1MS
	Servisler %d dakika, %d saniyedir çalýþýyor.
OPER_STATS_UPTIME_1M1S
	Servisler %d dakika, %d saniyedir çalýþýyor.
OPER_STATS_BYTES_READ
	Okunan veri  : %5d kB
OPER_STATS_BYTES_WRITTEN
	Yazýlan veri : %5d kB
OPER_STATS_USER_MEM
	Kullanýcý  : %6d kayýt, %5d kB
OPER_STATS_CHANNEL_MEM
	Kanal      : %6d kayýt, %5d kB
OPER_STATS_GROUPS_MEM
	NS Groups     : %6d records, %5d kB
OPER_STATS_ALIASES_MEM
	NS Aliases    : %6d records, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ   : %6d kayýt, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ    : %6d kayýt, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ   : %6d kayýt, %5d kB
OPER_STATS_SESSIONS_MEM
	Oturumlar  : %6d kayýt, %5d kB
OPER_STATS_AKILL_COUNT
	Þu anki AKILL sayýsý: %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Varsayýlan AKILL zaman aþýmý: %d gün
OPER_STATS_AKILL_EXPIRE_DAY
	Varsayýlan AKILL zaman aþýmý: 1 gün
OPER_STATS_AKILL_EXPIRE_HOURS
	Varsayýlan AKILL zaman aþýmý: %d saat
OPER_STATS_AKILL_EXPIRE_HOUR
	Varsayýlan AKILL zaman aþýmý: 1 saat
OPER_STATS_AKILL_EXPIRE_MINS
	Varsayýlan AKILL zaman aþýmý: %d dakika
OPER_STATS_AKILL_EXPIRE_MIN
	Varsayýlan AKILL zaman aþýmý: 1 dakika
OPER_STATS_AKILL_EXPIRE_NONE
	Varsayýlan AKILL zaman aþýmý: zaman aþýmý yok
OPER_STATS_SGLINE_COUNT
	Þu anki SGLINE sayýsý: %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Varsayýlan SGLINE zaman aþýmý süresi: %d gün
OPER_STATS_SGLINE_EXPIRE_DAY
	Varsayýlan SGLINE zaman aþýmý süresi: 1 gün
OPER_STATS_SGLINE_EXPIRE_HOURS
	Varsayýlan SGLINE zaman aþýmý süresi: %d saat
OPER_STATS_SGLINE_EXPIRE_HOUR
	Varsayýlan SGLINE zaman aþýmý süresi: 1 saat
OPER_STATS_SGLINE_EXPIRE_MINS
	Varsayýlan SGLINE zaman aþýmý süresi: %d dakika
OPER_STATS_SGLINE_EXPIRE_MIN
	Varsayýlan SGLINE zaman aþýmý süresi: 1 dakika
OPER_STATS_SGLINE_EXPIRE_NONE
	Varsayýlan SGLINE zaman aþýmý süresi: Zaman aþýmý yok
OPER_STATS_SQLINE_COUNT
	Su anki SQLINE sayisi: %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Varsayilan SQLINE zaman asimi süresi: %d gün
OPER_STATS_SQLINE_EXPIRE_DAY
	Varsayilan SQLINE zaman asimi süresi: 1 gün
OPER_STATS_SQLINE_EXPIRE_HOURS
	Varsayilan SQLINE zaman asimi süresi: %d saat
OPER_STATS_SQLINE_EXPIRE_HOUR
	Varsayilan SQLINE zaman asimi süresi: 1 saat
OPER_STATS_SQLINE_EXPIRE_MINS
	Varsayilan SQLINE zaman asimi süresi: %d dakika
OPER_STATS_SQLINE_EXPIRE_MIN
	Varsayilan SQLINE zaman asimi süresi: 1 dakika
OPER_STATS_SQLINE_EXPIRE_NONE
	Varsayilan SQLINE zaman asimi süresi: Zaman asimi yok
OPER_STATS_SZLINE_COUNT
	Þu anki SZLINE sayýsý: %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Varsayýlan SZLINE zaman aþýmý süresi: %d gün
OPER_STATS_SZLINE_EXPIRE_DAY
	Varsayýlan SZLINE zaman aþýmý süresi: 1 gün
OPER_STATS_SZLINE_EXPIRE_HOURS
	Varsayýlan SZLINE zaman aþýmý süresi: %d saat
OPER_STATS_SZLINE_EXPIRE_HOUR
	Varsayýlan SZLINE zaman aþýmý süresi: 1 saat
OPER_STATS_SZLINE_EXPIRE_MINS
	Varsayýlan SZLINE zaman aþýmý süresi: %d dakika
OPER_STATS_SZLINE_EXPIRE_MIN
	Varsayýlan SZLINE zaman aþýmý süresi: 1 dakika
OPER_STATS_SZLINE_EXPIRE_NONE
	Varsayýlan SZLINE zaman aþýmý süresi: Zaman aþýmý yok
OPER_STATS_RESET
	Ýstatistikler resetlendi.

# MODE responses
OPER_STATS_UPLINK_SERVER
	Uplink server: %s
OPER_STATS_UPLINK_CAPAB
	Uplink capab: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Servers found: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE kanaladý modlar

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
	CLEARMODES kanaladý [ALL]
OPER_CLEARMODES_DONE
	%s kanalýnda modlar ve banlar temizlendi.
OPER_CLEARMODES_ALL_DONE
	%s kanalýndaki tüm modlar temizlendi.


# KICK responses
OPER_KICK_SYNTAX
	KICK kanaladý kullanýcý sebep

# ADMIN responses
OPER_SVSNICK_SYNTAX
	SVSNICK nick newnick 
OPER_SVSNICK_NEWNICK
	The nick %s is now being changed to %s.
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [nick | kayýt-listesi]
OPER_ADMIN_SKELETON
	Servisler iskelet modunda(skeleton); ADMIN komutu devre dýþý.
OPER_ADMIN_EXISTS
	%s zaten Servis adminleri listesinde.
OPER_ADMIN_REACHED_LIMIT
	Sadece %d tane servis admini olabilir.
OPER_ADMIN_ADDED
	%s Servis adminleri listesine eklendi.
OPER_ADMIN_NOT_FOUND
	%s Servisler admin listesinde bulunamadý.
OPER_ADMIN_NO_MATCH
	Servis adminleri listesinde uyuþan kayýt yok.
OPER_ADMIN_DELETED
	%s servis adminleri listesinden silindi.
OPER_ADMIN_DELETED_ONE
	Servis adminleri listesinden 1 kayýt silindi.
OPER_ADMIN_DELETED_SEVERAL
	Servis adminleri listesinden %d kayýt silindi.
OPER_ADMIN_LIST_EMPTY
	Servis adminleri listesi boþ.
OPER_ADMIN_LIST_HEADER
	Servis adminleri listesi:
	  Num   Nick
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	Servis adminleri listesi temizlendi.
OPER_ADMIN_MOVED
	%s has been moved to Services Administrators list.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | kayýt-listesi]
OPER_OPER_SKELETON
	Servisler iskelet modunda(skeleton); OPER komutu devre dýþý.
OPER_OPER_EXISTS
	%s zaten servis operatörleri listesinde.
OPER_OPER_REACHED_LIMIT
	Sadece %d tane servis operatörü olabilir.
OPER_OPER_ADDED
	%s servis operatörleri listesine eklendi.
OPER_OPER_NOT_FOUND
	%s servis operatörleri listesinde bulunamadý.
OPER_OPER_NO_MATCH
	Servis operatörleri listesinde uyuþan kayýt bulunamadý.
OPER_OPER_DELETED
	%s servis operatörleri listesinden silindi.
OPER_OPER_DELETED_ONE
	Servis operatörleri listesinden 1 kayýt silindi.
OPER_OPER_DELETED_SEVERAL
	Servis operatörleri listesinden %d kayýt silindi.
OPER_OPER_LIST_EMPTY
	Servis operatörleri listesi boþ.
OPER_OPER_LIST_HEADER
	Servis operatörleri listesi:
	  Num   Nick
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	Servis operatörleri listesi temizlendi.
OPER_OPER_MOVED
	%s has been moved to Services Operators list.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+zamanaþýmý] {mask | kayýt-no} [sebep]]
OPER_AKILL_EXISTS
	%s zaten AKILL listesinde.
OPER_AKILL_ALREADY_COVERED
	%s zaten %s tarafýndan deðiþtirildi.
OPER_AKILL_REACHED_LIMIT
	Sadece %d kiþi AKILL listesinde olabilir.
OPER_AKILL_NO_NICK
	Hatýrlatma: AKILL maskýna nick eklenemez; AKILL maskýna nick eklemediðinizden emin olun.
OPER_AKILL_ADDED
	%s AKILL listesine eklendi.
OPER_AKILL_CHANGED
	%s nickinin zaman aþýmý süresi deðiþtirildi.
OPER_AKILL_NOT_FOUND
	%s AKILL listesinde bulunamadý.
OPER_AKILL_NO_MATCH
	Uyuþan kayýt AKILL listesinde bulunamadý.
OPER_AKILL_DELETED
	%s AKILL listesinden silindi.
OPER_AKILL_DELETED_ONE
	AKILL listesinden 1 kayýt silindi.
OPER_AKILL_DELETED_SEVERAL
	AKILL listesinden %d kayýt silindi.
OPER_AKILL_LIST_EMPTY
	AKILL listesi boþ.
OPER_AKILL_LIST_HEADER
	Þu anki AKILL listesi:
	  No    Mask                              Sebep
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Þu anki AKILL listesi:
# number, mask, set-by, set-time, expires, reason
OPER_AKILL_VIEW_FORMAT
	%3d  %s (%s tarafýndan %s tarihinde; %s)
	      %s
OPER_AKILL_CLEAR
	AKILL listesi temizlendi.

# SGLINE responses
OPER_CHANKILL_SYNTAX
	CHANKILL [+expiry] {#channel} [reason]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+zamanaþýmý] {mask | kayýt-sýrasý} [sebep]]
OPER_SGLINE_UNSUPPORTED
	SGLINE bu networkte mevcut deðil.
OPER_SGLINE_EXISTS
	%s zaten SGLINE listesinde.
OPER_SGLINE_ALREADY_COVERED
	%s zaten %s tarafýndan deðiþtirildi.
OPER_SGLINE_REACHED_LIMIT
	Sadece %d kiþi SGLINE listesinde olabilir.
OPER_SGLINE_ADDED
	%s SGLINE listesinde eklendi.
OPER_SGLINE_CHANGED
	%s nickinin zaman aþýmý süresi deðiþtirildi.
OPER_SGLINE_NOT_FOUND
	%s SGLINE listesinde bulunamadý.
OPER_SGLINE_NO_MATCH
	SGLINE listesinde uyuþan kayýt bulunamadý.
OPER_SGLINE_DELETED
	%s SGLINE listesinden silindi.
OPER_SGLINE_DELETED_ONE
	SGLINE listesinden 1 kayýt silindi.
OPER_SGLINE_DELETED_SEVERAL
	SGLINE listesinden %d kayýt silindi.
OPER_SGLINE_LIST_EMPTY
	SGLINE listesi boþ.
OPER_SGLINE_LIST_HEADER
	Þu anki SGLINE listesi:
	  No   Mask                              Sebep
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Þu anki SGLINE listesi:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SGLINE_CLEAR
	SGLINE listesi temizlendi.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+zaman asimi] {mask | kayit-sirasi} [sebep]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	SQLINE kullandiginiz IRCd tarafindan desteklenmiyor, bu yüzden kullanmazsiniz.
OPER_SQLINE_EXISTS
	%s zaten SQLINE listesinde mevcut.
OPER_SQLINE_ALREADY_COVERED
	%s zaten %s tarafindan degistirildi.
OPER_SQLINE_REACHED_LIMIT
	Toplam %d SQLINE olabilir.
OPER_SQLINE_ADDED
	%s SQLINE listesine eklendi.
OPER_SQLINE_CHANGED
	%s için zaman asimi süresi degistirildi.
OPER_SQLINE_NOT_FOUND
	%s SQLINE listesinde bulunamadi.
OPER_SQLINE_NO_MATCH
	SQLINE listesinde uyusan kayit bulunamadi.
OPER_SQLINE_DELETED
	%s SQLINE listesinden silindi.
OPER_SQLINE_DELETED_ONE
	SQLINE listesinden 1 kayit silindi.
OPER_SQLINE_DELETED_SEVERAL
	SQLINE listesinden %d kayit silindi.
OPER_SQLINE_LIST_EMPTY
	SQLINE listesi bos.
OPER_SQLINE_LIST_HEADER
	Mevcut SQLINE listesi:
	  Num   Mask                              Sebep
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Mevcut SQLINE listesi:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SQLINE_CLEAR
	SQLINE listesi temizlendi.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+zamanaþýmý] {mask | kayýt-sýrasý} [sebep]]
OPER_SZLINE_UNSUPPORTED
	SZLINE bu networkte mevcut deðil.
OPER_SZLINE_EXISTS
	%s zaten SZLINE listesinde.
OPER_SZLINE_ALREADY_COVERED
	%s zaten %s tarafýndan deðiþtirildi.
OPER_SZLINE_REACHED_LIMIT
	Sadece %d kiþi SZLINE listesinde olabilir.
OPER_SZLINE_ONLY_IPS
	Hatýrlatma: SZLINE listesine sadece IP masklarýný ekleyebilirsiniz.
OPER_SZLINE_ADDED
	%s SZLINE listesine eklendi.
OPER_SZLINE_CHANGED
	%s nickinin zaman aþýmý süresi deðiþtirildi.
OPER_SZLINE_NOT_FOUND
	%s SZLINE listesinde bulunamadý.
OPER_SZLINE_NO_MATCH
	SZLINE listesinde uyuþan kayýt bulunamadý.
OPER_SZLINE_DELETED
	%s SZLINE listesinden silindi.
OPER_SZLINE_DELETED_ONE
	SZLINE listesinden 1 kayýt silindi.
OPER_SZLINE_DELETED_SEVERAL
	SZLINE listesinden %d kayýt silindi.
OPER_SZLINE_LIST_EMPTY
	SZLINE listesi boþ.
OPER_SZLINE_LIST_HEADER
	Þu anki SZLINE listesi:
	  No   Mask                              Sebep
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Þu anki SZLINE listesi:
# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SZLINE_CLEAR
	SZLINE listesi temizlendi.

# SET responses
OPER_SET_SYNTAX
	SET özellik ayar
OPER_SET_IGNORE_ON
	Ignore kodu AKTIF.
OPER_SET_IGNORE_OFF
	Ignore kodu KAPALI.
OPER_SET_IGNORE_ERROR
	IGNORE ayarý ON veya OFF þeklinde olmalýdýr.
OPER_SET_READONLY_ON
	Servisler þimdi read-only modunda.
OPER_SET_READONLY_OFF
	Servisler þimdi read-write modunda.
OPER_SET_READONLY_ERROR
	READONLY ayarý ON veya OFF þeklinde olmalýdýr.
OPER_SET_LOGCHAN_ON
	Services is now reporting log messages to %s.
OPER_SET_LOGCHAN_OFF
	Services is no longer reporting log messages to a channel.
OPER_SET_LOGCHAN_ERROR
	Setting for LOGCHAN must be ON or OFF and LogChannel must be defined.
OPER_SET_DEBUG_ON
	Servisler þimdi debug modunda.
OPER_SET_DEBUG_OFF
	Servisler þimdi non-debug modunda.
OPER_SET_DEBUG_LEVEL
	Servisler þimdi debug modunda (seviye %d).
OPER_SET_DEBUG_ERROR
	DEBUG ayarý ON, OFF, veya pozitif bir sayý olmalýdýr.
OPER_SET_NOEXPIRE_ON
	Servisler þimdi no expire modunda.
OPER_SET_NOEXPIRE_OFF
	Servisler þimdi expire modunda.
OPER_SET_NOEXPIRE_ERROR
	NOEXPIRE ayarý ON yada OFF olmalýdýr.
OPER_SET_UNKNOWN_OPTION
	Varolmayan özellik %s.
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
	NOOP {SET|REVOKE} server
OPER_NOOP_SET
	%s serverýndaki tüm O:line lar silindi.
OPER_NOOP_REVOKE
	%s serverýndaki tüm O:line lar resetlendi.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE serverismi [sebep]

# RAW responses
OPER_JUPE_HOST_ERROR
	Please use a valid server name when juping
	
# RAW responses
OPER_RAW_SYNTAX
	RAW yazý

# UPDATE responses
OPER_UPDATING
	Veritabaný güncelleniyor.

# RELOAD responses
OPER_RELOAD
	Servislerin conf dosyasý yeniden yüklendi.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN tanýmlanmamýþ; restart edilemiyor.  Yeniden \2configure\2 edin ve RESTART komutunu aktifleþtirmek için Servisleri yeniden derleyin.

# LISTIGNORE responses
OPER_IGNORE_SYNTAX
	Syntax: IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick]
OPER_IGNORE_VALID_TIME
	You have to enter a valid number as time.
OPER_IGNORE_TIME_DONE
	%s will now be ignored for %s.
OPER_IGNORE_PERM_DONE
	%s will now permanently be ignored.
OPER_IGNORE_DEL_DONE
	%s will no longer be ignored.
OPER_IGNORE_LIST
	Servisler ignore listesi:
OPER_IGNORE_LIST_NOMATCH
	%s not found on ignore list.
OPER_IGNORE_LIST_EMPTY
	Ignore listesi boþ.

# KILLCLONES responses
OPER_IGNORE_LIST_CLEARED
	Ignore list has been cleared.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	Kullanýcý %s bulunamadý.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Kanal listesi:
	Ýsim              Kullanici Modlari  Konu
OPER_CHANLIST_HEADER_USER
	%s kanal listesi:
	Ýsim              Kullanici Modlari  Konu
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	Kanal listesi sonu.
	
# USERLIST responses
OPER_USERLIST_HEADER
	Kullanýcý listesi:
	Nick                 Mask
OPER_USERLIST_HEADER_CHAN
	%s kullanýcý listesi:
	Nick                 Mask
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Kullanýcý listesi sonu.

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
	Syntax: DEFCON [1|2|3|4|5]
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

# Modules strings

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
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [parametreler]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+zamanaþýmý] mask limit sebep
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {mask | liste}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE num pozisyon
OPER_EXCEPTION_DISABLED
	Session limit koyma devre dýþý.
OPER_EXCEPTION_ALREADY_PRESENT
	Mask %s zaten exception listesinde bulunuyor.
OPER_EXCEPTION_TOO_MANY
	Session-limit exception listesi dolu!
OPER_EXCEPTION_ADDED
	%s için session limit %d olarak deðiþtirildi.
OPER_EXCEPTION_MOVED
	%s (#%d) için exception %d pozisyonuyla deðiþtirildi.
OPER_EXCEPTION_NO_SUCH_ENTRY
	(#%d) böyle bir kayýt yok. session-limit exception listesi.
OPER_EXCEPTION_NOT_FOUND
	%s session-limit exception listesinde bulunamadý.
OPER_EXCEPTION_NO_MATCH
	session-limit exception listesinde uyuþan kayýt bulunamadý.
OPER_EXCEPTION_DELETED
	%s session-limit exception listesinden silindi.
OPER_EXCEPTION_DELETED_ONE
	Session-limit exception listesinden 1 kayýt silindi.
OPER_EXCEPTION_DELETED_SEVERAL
	Session-limit exception listesinden %d kayýt silindi.
OPER_EXCEPTION_LIST_HEADER
	Þu anki Session Limit Exception listesi:
# mask, limit
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Sayý  Limit  Host
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (by %s on %s; %s)
	    Limit: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Geçersiz session limiti. Sýfýrdan büyük yada eþit ve %d den az bir geçerli tamsayý olmalýdýr.
OPER_EXCEPTION_INVALID_HOSTMASK
	Geçersiz hostmask. Sadece gerçek hostmasklar exception olarak geçerlidir.
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
	Session limit koyma devre dýþý.
OPER_SESSION_INVALID_THRESHOLD
	Geçersiz threshold deðeri. 1 den büyük geçerli tamsayý olmalýdýr.
OPER_SESSION_NOT_FOUND
	%s session listesinde bulunamadý.
OPER_SESSION_LIST_HEADER
	En az %d sessionlý hostlar:
OPER_SESSION_LIST_COLHEAD
	Sessionlar  Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	Host %s þu anda %d session a sahip ve %d ile limitli.

# EXCEPTION help

OPER_HELP_EXCEPTION
	Kullanýmý: EXCEPTION ADD [+zamanaþýmý] mask limit sebep
	           EXCEPTION DEL {mask | liste}
	           EXCEPTION MOVE num pozisyon
	           EXCEPTION LIST [mask | liste]
	           EXCEPTION VIEW [mask | liste]
	
	Servis adminlerinin, belirli oturum limiti olan hostlarýn 
	listesini deðiþtirmesini saðlar. Shell server gibi bazý 
	makinalarýn varsayýlandan daha fazla kullanýcýyý ayný anda
	taþýmasýný saðlar. Eðer bir host session(oturum) limitine
	ulaþmýþsa o hosttan baðlanmaya çalýþan tüm kullanýcýlar 
	kill lenir. Kill lenmeden önce, %S'ün /NOTICE i ile
	uyarýlarak session limiti hakkýnda yardým verilir. Bu uyarýnýn
	içeriði ayar yapmayla ilgilidir.
	
	EXCEPTION ADD exception(istisna) listesine belirtilen hostmaský
	ekler. nick!user@host ve user@host masklarýnýn geçersiz 
	olduðunu unutmayýn! Sadece box.host.dom ve *.host.dom gibi 
	gerçek hostmasklar kullanýlabilinir, çünkü session limitleme 
	nickleri ve username leri hesaba almaz. limit sýfýrdan büyük 
	yada eþit bir rakam olmalýdýr. Bu o host un ayný anda kaç oturum
	(session) kaldýrabileceðini belirler. Sýfýr deðeri o host un
	sýnýrsýz oturum limiti olduðunu belirtir. Opsiyonel zamanaþýmý 
	parametresinin formatý ile ilgili yardým için AKILL in 
	yardýmýna bakýnýz.
	EXCEPTION DEL listesinden belirtilen mask ý çýkarýr.
	EXCEPTION MOVE exception numarasýný(num) pozisyona (position)
	çevirir. Aralarýndaki exceptionlarýn yeri aralýðý doldurmak için 
	yukarý yada aþþaðý deðiþtirilecektir.
	EXCEPTION LIST ve EXCEPTION VIEW tüm mevcut exceptionlarý 
	gösterir; eðer opsiyonel olarak mask belirtilmiþse, liste
	bu masklarla uyuþan exceptionlarla sýnýrlandýrýlýr. Aradaki fark
	EXCEPTION VIEW daha gelismis bir komuttur ve exception ý ekleyenin 
	nickini, o exceptionýn session limitini, sebebi, host maský ve zaman 
	aþýmý tarih ve zamanýný gösterir.
	
	Baðlanan kullanýcý, hostuyla uyuþan ilk exceptioný kullanýr. Büyük 
	exception listeleri ve geniþ olarak uyuþan exception masklarý 
	servislerin performansýný düþürür. 
	
	Servis adminleri tarafýndan kullanýlabilir.

OPER_HELP_SESSION
	Kullanýmý: SESSION LIST threshold
	           SESSION VIEW host
	
	Servis adminlerinin session listesini görüntülemesini saðlar.
	
	SESSION LIST en az threshold sessionlardaki hostlarý listeler.
	Threshold(eþik) 1 den büyük bir sayý olmalýdýr. Bu büyük 
	ölçüdeki tek session hostlarýn yanlýþlýkla listelenmelerini 
	önlemek içindir.
	SESSION VIEW belirli bir host hakkýnda ayrýntýlý bilgileri 
	görüntüler. Bu o anki session sayýsýný ve session limitinide
	gösterir. host deðeri wildcard(açýk olarak yazýlmalýdýr) 
	içermemelidir.
	
	Session limitleme ve belirli hostlara ve gruplara nasýl session 
	limit koyulacaðý hakkýnda yardým için EXCEPTION yardýmýna bakýn.
	
	Servis adminleri tarafýndan kullanýlabilir.

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
	[Baðlantý Haberleri - %s] %s
NEWS_OPER_TEXT
	[Operatör Haberleri - %s] %s
NEWS_RANDOM_TEXT
	[Karisik Haberler - %s] %s

NEWS_LOGON_SYNTAX
	Kullanýmý: LOGONNEWS {ADD|DEL|LIST} [yazý|num]
NEWS_LOGON_LIST_HEADER
	Baðlantý haberleri:
NEWS_LOGON_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_LOGON_LIST_NONE
	Hiç baðlantý haberi yok.
NEWS_LOGON_ADD_SYNTAX
	Kullanýmý: LOGONNEWS ADD yazý
NEWS_LOGON_ADD_FULL
	Haber listesi dolu!
NEWS_LOGON_ADDED
	Yeni baðlantý haberleri maddesi eklendi (#%d).
NEWS_LOGON_DEL_SYNTAX
	Kullanýmý: LOGONNEWS DEL {num | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Baðlantý haberleri maddesi #%d bulunamadý!
NEWS_LOGON_DELETED
	Baðlantý haberleri maddesi #%d silindi.
NEWS_LOGON_DEL_NONE
	Silebileceðiniz baðlantý haberleri maddesi yok!
NEWS_LOGON_DELETED_ALL
	Tüm baðlantý haberleri silindi.

NEWS_OPER_SYNTAX
	Kullanýmý: OPERNEWS {ADD|DEL|LIST} [yazý|num]
NEWS_OPER_LIST_HEADER
	Operatör haberleri:
NEWS_OPER_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_OPER_LIST_NONE
	Hiç operatör haberi yok.
NEWS_OPER_ADD_SYNTAX
	Kullanýmý: OPERNEWS ADD yazý
NEWS_OPER_ADD_FULL
	Haber listesi dolu!
NEWS_OPER_ADDED
	Yeni operatör haberleri maddesi eklendi (#%d).
NEWS_OPER_DEL_SYNTAX
	Kullanýmý: OPERNEWS DEL {num | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Operatör haberleri maddesi #%d bulunamadý!
NEWS_OPER_DELETED
	Operatör haberleri maddesi #%d silindi.
NEWS_OPER_DEL_NONE
	Silebileceðiniz operatör haberleri maddesi yok!
NEWS_OPER_DELETED_ALL
	Tüm operatör haberleri maddeleri silindi.

NEWS_RANDOM_SYNTAX
	Kullanýmý: RANDOMNEWS {ADD|DEL|LIST} [yazi|no]
NEWS_RANDOM_LIST_HEADER
	Karisik haberler:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_RANDOM_LIST_NONE
	Hiç karisik haber yok.
NEWS_RANDOM_ADD_SYNTAX
	Kullanýmý: RANDOMNEWS ADD yazi
NEWS_RANDOM_ADD_FULL
	Haber listesi dolu!
NEWS_RANDOM_ADDED
	Yeni karisik haber eklendi (#%d).
NEWS_RANDOM_DEL_SYNTAX
	Kullanýmý: RANDOMNEWS DEL {no | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	#%d numarali karisik haber bulunamadi!
NEWS_RANDOM_DELETED
	#%d numarali karisik haber silindi.
NEWS_RANDOM_DEL_NONE
	Silinecek karisik haber yok!
NEWS_RANDOM_DELETED_ALL
	Tüm karisik haberler silindi.

NEWS_HELP_LOGON
	Syntax: LOGONNEWS ADD text
	        LOGONNEWS DEL {num | ALL}
	        LOGONNEWS LIST
	
	Edits or displays the list of logon news messages.  When a
	user connects to the network, these messages will be sent
	to them.  (However, no more than %d messages will be
	sent in order to avoid flooding the user.  If there are
	more news messages, only the most recent will be sent.)
	NewsCount can be configured in services.conf.
	
	LOGONNEWS LIST may be used by any IRC operator to list the
	current news messages.  ADD and DEL may only be used by
	Services admins.

NEWS_HELP_OPER
	Syntax: OPERNEWS ADD text
	        OPERNEWS DEL {num | ALL}
	        OPERNEWS LIST
	
	Edits or displays the list of oper news messages.  When a
	user opers up (with the /OPER command), these messages will
	be sent to them.  (However, no more than %d messages will
	be sent in order to avoid flooding the user.  If there are
	more news messages, only the most recent will be sent.)
	NewsCount can be configured in services.conf.
	
	OPERNEWS LIST may be used by any IRC operator to list the
	current oper news messages.  ADD and DEL may only be used by
	Services admins.

NEWS_HELP_RANDOM
	Kullanimi: RANDOMNEWS ADD yazi
	           RANDOMNEWS DEL {no | ALL}
	           RANDOMNEWS LIST
	
	Karisik haberleri düzenlemenizi saglar. Bir kullanici
	network e baglandigi zaman, karisik haberlerden bir tanesi
	seçilerek kullaniciya gönderilir.
	
	RANDOMNEWS LIST herhangi bir IRC operatörü tarafindan
	haberleri listelemek için kullanilabilir. ADD ve DEL
	komutlarini sadece Servis adminlerinin kullanabilir.

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S servisler hakkýnda bilgi vermek için tasarlanmýþtýr.
	Yardým konularýna diðer servislerin HELP komutuyla ulaþýlýr.
	
	%R%s HELP
	     nick kayýt hakkýnda bilgiler içerir.
	
	%R%s HELP
	     kanal kayýdý ve kontrolü hakkýnda bilgi içerir.
	
	%R%s HELP
	     kullanýcýlara internette olmasalar bile mesaj göndermek
	     için gerekli bilgileri içerir.

HELP_HELP_BOT

	%R%s HELP
	     kanallara bot sokulmasý ve ayarlarý hakkýnda bilgi içerir.

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
	    REGISTER   Nickinizi kaydeder
NICK_HELP_CMD_GROUP
	    GROUP      Bir gruba dahil eder
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY   Þifrenizle nickinizi tanýtýr
NICK_HELP_CMD_ACCESS
	    ACCESS     Ýzinli adreslerin listesini düzenler
NICK_HELP_CMD_SET
	    SET        Ayarlar, kill korumasýný içerir
NICK_HELP_CMD_SASET
	    SASET      Set SET-options on another nickname
NICK_HELP_CMD_DROP
	    DROP       Nickin kaydýný siler
NICK_HELP_CMD_RECOVER
	    RECOVER    Nickinizi kullanan birini kill ler
NICK_HELP_CMD_RELEASE
	    RELEASE    RECOVER komutundan sonra nickinizi serbest 
	               býrakýr
NICK_HELP_CMD_SENDPASS
	    SENDPASS   Þifrenizimi unuttunuz? Bunu deneyin
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
	    GETPASS    Bir nickin þifresini öðrenmek içindir
	               (Sadece þifreleme(encryption) devre dýþýysa)
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Matches and returns all users that registered using given email
NICK_HELP_CMD_FORBID
	    FORBID     Bir nickin kullanýlmasýný yasaklar
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Suspend a given nick
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Unsuspend a given nick

NICK_HELP
	%S nickinizi "kayýt" etmenizi ve diðerlerinin nickinizi 
	kullanmalarýný engellemeyi saðlar. Aþaðýdaki komutlar nicki
	kayýt etmeye ve korumaya izin verir, kullanmak için, 
	%R%S komut yazýn. Belirli bir komut hakkýnda daha 
	fazla bilgi için, %R%S HELP komut yazýn.
	
NICK_HELP_FOOTER
	NOT: Bu servis kullanýcýlarýn kimliklerini tehlikeye 
	atmayacak þekilde tasarlanmýþtýr. Bu nickleri "çalmayý" 
	veya diðer kötü niyetli hareketleri kolaylaþtýrmayý saðlama
	amaçlý deðildir. %S 'ün kötüye Kullanýmý kötüye 
	kullanýlan kiþinin nickini kaybetmesiyle sonuçlanacaktýr.

NICK_HELP_EXPIRES
	
	Daha fazla kullanýlmayan nicklerin otomatik olarak
	kaydýnýn silinmesi söz konusudur, mesela kullanýlmayan
	nickler %d gün sonra silinirler.

NICK_HELP_REGISTER
	Kullanýmý: REGISTER þifre [email]
	
	%S 'ün veritabanýna nickinizi kaydeder. Nickinizi 
	bir kere kayýt ettikten sonra nickinizin özelliklerini 
	istediðiniz gibi belirlemek için SET ve ACCESS 
	komutlarýný kullanabilirsiniz. Kayýtta kullandýðýnýz þifrenin
	hatýrlanabilecek birþey olmasýna dikkat edin - ilerde
	nickinizin özelliklerinde deðiþiklik yaparken ve sunucuda
	kullanabilmek için ihtiyacýnýz olacak. (Þifrelerin büyük
	küçük harf hassasiyeti olduðunu unutmayýn! ÝXÝR, Ýxir, 
	ve ixir þifreleri birbirinden farklýdýr.)
	
	Þifreleri seçme konusunda rehber:
	
	Þifreler kolayca tahmin edilebilir olmamalýdýr.  Örneðin,
	gerçek isminizi þifre olarak kullanmak kötü bir fikirdir.
	Nickinizi þifre olarak kullanmak daha kötü bir fikirdir :)
	ve zaten %S buna izin vermez. Ayrýca kýsa þifreler 
	deneme yanýlma yöntemiyle bulunabilirler, bu yüzden þifrenizin
	en az 5 karakter olmasýna dikkat edin. Son olarak þifrede
	boþluk kullanmamanýz gerekir.
	
	email parametresi opsiyoneldir ve nickiniz için bir 
	email adresi belirler. Fakat bazý networklerde bu
	parametrenin kullanýlmasý zorunlu olabilir.(Unutmayýn
	email adresinizi doðru olarak vermeniz ilerde þifrenizi 
	unutmanýz halinde çok iþe yarayacaktýr)
	Gizliliðinize saygý duyulacak ve mail adresiniz herhangi 
	üçüncü þahýslara söylenmeyecektir.

	Bu komut ayrýca nickiniz için yeni bir grup oluþturur ki bu
	ilerde gruba kaydedeceðiniz nickler için ayný özellikleri, 
	memo ayarlarýný ve ayný kanal ayrýcalýklarýný paylaþmanýzý 
	saðlar. Bu özellik hakkýnda daha fazla bilgi için, 
	%R%S HELP GROUP yazýn.


NICK_HELP_GROUP
	Kullanýmý: GROUP hedef þifre
	
	Bu komut nickinizin hedef gösterilen nickin grubuna katýlmasýný
	saðlar. þifre hedef gösterilen nickin þifresidir.
	
	Bir gruba dahil olmak size o gruba dahil olan tüm nicklerinizin
	ayarlarýný, memolarýný, ve kanal ayrýcalýklarýný paylaþmanýzý ve 
	daha fazlasýný saðlar!
	 
	Bir grup kullanýþlý olduðu sürece kalýcýdýr. Bu demektirki eðer
	gruba dahil nickleriniz kaydýný silerseniz, grupta en az bir nick 
	kalmasý kaydiyle yukarda açýklanan paylaþýmlarý kaybetmezseniz.
	
	Bu komutu nickinizi kaydetmemiþ olsanýz bile kullanabilirsiniz.
	Eðer nickiniz zaten kayýtlý ise, bu komutu kullanmadan önce
	nickinizi tanýtmanýz gerekir, daha fazla bilgi için
	%R%S HELP IDENTIFY yazýn. Sizin IRC networkunuzda
	bu mümkün olmayabilir.
	
	Bu komutu kayýtlý olmayan bir nickle kullanmanýz önerilir çünkü
	bu komutu kullanýcý o nick otomatik olarak kaydedilir. Bunu
	kayýtlý bir nicklede eðer network adminleri buna izin veriyorsa
	kullanabilirsiniz.
	
	Bir seferde sadece bir grupta olabilirsiniz. Grup karýþtýrma
	mümkün deðildir.
	
	Not: bir gruptaki tüm nickler bir þifreye sahip olurlar.

NICK_HELP_IDENTIFY
	Kullanýmý: IDENTIFY þifreniz
	
	%S 'e kullandýðýnýz nickin gerçek sahibinin siz olduðunu
	belirtir. Çoðu komut kullanýlmadan önce bu komutla 
	tanýtýlmanýzý gerektirir. Þifreniz nickinizi kayýt ederken 
	REGISTER komutuyla beraber kullandýðýnýzla aynýdýr.

NICK_HELP_UPDATE
	Syntax: UPDATE

	Updates your current status, i.e. it checks for new memos,
	sets needed chanmodes (ModeonID) and updates your vhost and
	your userflags (lastseentime, etc).
	
NICK_HELP_LOGOUT
	Kullanýmý: LOGOUT
	
	IDENTIFY komutunun yaptýðý etkiyi tersine çevirir,
	yani artýk o nickin gerçek sahibi olarak tanýnmamanýzý
	saðlar. Fakat sizden nickinizi tekrar identify etmeniz 
	istenmez.

NICK_HELP_DROP
	Kullanýmý: DROP [nick]
	
	%S 'ün veritabanýndan nickinizi siler. Silinmiþ bir nick
	baþkasý tarafýndan kayýt edilebilir.

	nick parametresini kullanarak belirttiðiniz nicki
	grubunuzdan silebilirsiniz.
	
	Bu komutu kullanmadan önce, þifrenizle kendinizi tanýtmanýz
	gereklidir (Daha fazla bilgi için %R%S HELP IDENTIFY 
	yazýn).

NICK_HELP_ACCESS
	Kullanýmý:  ACCESS ADD mask
	        ACCESS DEL mask
	        ACCESS LIST
	
	Nickinizin access litesini görüntüler ve deðiþiklikler 
	yapmanýzý saðlar. Bu listede belirtilen adresler %S 
	tarafýndan nickinizi kullanabilmek için otomatik olarak 
	tanýnacak adreslerdir. (böylece identify komutunu 
	kullanmanýza gerek kalmaz.) Eðer belirtilenden farklý bir 
	adresle baðlanmýþsanýz %S 'ün sizi tanýmasý için 
	IDENTIFY komutunu kullanmanýz gerekir.
	
	Örnekler:
	
	    ACCESS ADD birisi@*.ixir.com
	        Kullanýcý birisi'ne ixir.com 
	        domaininden herhangi bir makineden 
	        eriþim saðlar.
	
	    ACCESS DEL birisi@*.ixir.com
	        Önceki komutun tersini yapar(siler).
	
	    ACCESS LIST
	        Mevcut access listesini görüntüler.

NICK_HELP_SET
	Kullanýmý: SET özellik parametre
	
	Çeþitli nick özelliklerini ayarlar.  özellik þunlardan biri 
	olabilir:
	
	    DISPLAY    Grubunuzun servislerde nasýl görüneceðini ayarlar
	    PASSWORD   Nickinize þifre belirtir
	    LANGUAGE   Servislerin size hitap edeceði 
	                  dili belirler
	    URL        Nickinizle bir URL yi iliþkilendirir
	    EMAIL      Nickinizle bir email adresini iliþkilendirir
	    ICQ        Nickinizle bir ICQ numarasýný iliþikilendirir
	    GREET      Nickiniz için bir karþýlama mesajý belirler
	    KILL       Korumayý açar kapatýr
	    SECURE     Nick güvenliðini açar kapatýr
	    PRIVATE    Nickinizin %R%S LIST yazýldýðýnda görünmesini
	                  engeller
	    HIDE       Nickinizle ilgili bazý bilgileri saklar
	    AUTOOP     Should services op you automatically.    
	
	Bu komutu kullanabilmek için önce, þifrenizle 
	kendinizi tanýtmanýz gerekir (Daha fazla bilgi için 
	%R%S HELP IDENTIFY yazýn).
	
	Belirli bir özellik(option) hakkýnda bilgi almak için
	%R%S HELP SET option yazýn.

NICK_HELP_SET_DISPLAY
	Kullanýmý: SET DISPLAY yeni-görünüm
	
	Servislerde grubunuzdaki nicklerden hangisinin gözükeceðini
	ayarlar. Yeni görünüm grubunuzda bulunan nicklerden biri 
	olmalýdýr.

NICK_HELP_SET_PASSWORD
	Kullanýmý: SET PASSWORD yeni þifre
	
	Nickinizi tanýtmak için kullandýðýnýz þifreyi
	deðiþtirir.

NICK_HELP_SET_LANGUAGE
	Kullanýmý: SET LANGUAGE numara
	
	Servislerin size hitap edeceði dili belirlemenizi saðlar.
	(örneðin, bir komut yazdýðýnýzda gelecek cevabýn dili gibi).
	numara aþaðýdaki listede desteklenen dillerden biri olarak 
	belirlenir:

NICK_HELP_SET_URL
	Kullanýmý: SET URL url
	
	Nickiniz için bir URL adresi belirtir. Bu URL adresi birisi
	sizin hakkýnýzda bilgi almak için INFO komutunu 
	kullandýðýnda görünür.

NICK_HELP_SET_EMAIL
	Kullanýmý: SET EMAIL email adresiniz
	
	Nickiniz için bir email adresi belirtir. Bu email adresi
	birisi sizin hakkýnýzda bilgi almak için INFO komutunu 
	kullandýðýnda görünür.

NICK_HELP_SET_ICQ
	Kullanýmý: SET ICQ icqnumaranýz
	
	Nickiniz için bir ICQ numarasý belirtir. Bu ICQ numarasý
	birisi sizin hakkýnýzda bilgi almak için INFO komutunu
	kullandýðýnda görünür.

NICK_HELP_SET_GREET
	Kullanýmý: SET GREET mesaj
	
	Yazdýðýnýz mesajý nickinizin karþýlama mesajý olarak
	belirler, bu mesaj GREET özelliði aktif bir kanala 
	girdiðinizde gönderilir, kanaldaki access seviyenize
	baðlýdýr.

NICK_HELP_SET_KILL
	Kullanýmý: SET KILL {ON | QUICK | IMMED | OFF}
	
	Nickiniz için otomatik koruma özelliðini açar ve kapatýr.
	Koruma açýkken eðer bir baþkasý sizin nickinizi kullanmaya 
	kalkarsa, o kiþiye nicki deðiþtirmesi için bir dakika süre 
	verilir, eðer deðiþtirmezse %S tarafýndan otomatik olarak
	deðiþtirilir.
	
	Eðer QUICK parametresini seçerseniz, kullanýcýya nickini
	deðiþtirmesi için 60 saniye yerine sadece 20 saniye verilir.
	Eðer IMMED parametresini seçerseniz kullanýcýnýn nicki
	uyarýlmadan hemen deðiþtirilir; lütfen çok gerekmedikçe
	bu özelliði kullanmayýn. Hatta network yöneticileri bu
	özelliði devre dýþý býrakmýþ olabilirler.

NICK_HELP_SET_SECURE
	Kullanýmý: SET SECURE {ON | OFF}
	
	%S'ün güvenlik özelliklerini açar ve kapatýr. SECURE 
	ayarý ile, nickin sahibi olarak tanýnmanýz için önce þifreyle 
	kendinizi tanýtmanýz gerekir(adresinizin nickin access
	listesinde olup olmamasýna baðlý olarak). Fakat, eðer 
	adresiniz nickin access listesindeyse KILL özelliði
	kullanýlmýþ olsa bile %S sizi otomatik olarak kill
	lemeyecektir.

NICK_HELP_SET_PRIVATE
	Kullanýmý: SET PRIVATE {ON | OFF}
	
	%S'ün özel nick özelliðini açar kapatýr. PRIVATE ayarý
	ile, %S'ün LIST komutu kullanýldýðýnda nickiniz listede
	gözükmeyecektir.(Buna raðmen, nickinizi bilen biri INFO 
	komutunu kullanarak hakkýnýzda bilgi alabilir.)

NICK_HELP_SET_HIDE
	Kullanýmý: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Birisi hakkýnýzda bilgi almak için %S INFO komutunu 
	kullandýðýnda bazý bilgilerin burada gözükmemesini saðlar.
	E-mail adresinizi (EMAIL), son göründüðünüz user@host 
	maskýný (USERMASK), ve son çýkýþ mesajýnýzý (QUIT)
	saklayabilirsiniz. Ýkinici parametre bilginin görünüp (OFF)
	görünmeyeceðini (ON) belirtmenizi saðlar.

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

NICK_HELP_SASET_AUTOOP
	Syntax: SASET nickname AUTOOP {ON | OFF}
	
	Sets whether the given nickname will be opped automatically.
	Set to ON to allow ChanServ to op you automatically when joining
	channels.
	
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
	Kullanýmý: GHOST nick [þifre]
	
	Asýlý kalan nickinizin baðlantýsýný koparýr. Asýlý kalan nick 
	aslýnda hayalet nicktir fakat IRC serverý bunun hala hatta
	olduðunu sanar. Genellikle bu durum bilgisayarýnýzýn çökmesi 
	yada modem baðlantýnýzýn serverdayken kopmasý durumunda 
	gerçekleþir.
	
	GHOST komutunu bir nick üzerinde kullanabilmek için o an
	/WHOIS inizdeki adresin o nickin access listesinde olmasý, 
	o nick için doðru þifrenin belirtilmiþ olmasý yada o nickin
	grubundaki baþka bir nickin tanýtýlmýþ olmasý gerekir.

NICK_HELP_INFO
	Kullanýmý: INFO nick [ALL]
	
	Belirtilen nick hakkýnda o nickin sahibi, son görüldüðü adres ve 
	zaman, ve nickin özellikleri gibi bilgiler verir. Eðer nick 
	hakkýnda bilgi alýrken o nickin sahibi sizseniz ve tanýtmýþsanýz 
	ALL parametresini kullandýðýnýzda bilgiler gizli olsa bile o 
	nick hakkýndaki tüm bilgiler görünür.

NICK_HELP_LIST
	Kullanýmý: LIST model
	
	nick!user@host formatýnda, belirtilen modeli içeren
	tüm kayýtlý nickleri listeler. PRIVATE özelliði aktif olan
	nickler listelenmez.
	
	Örnekler:
	
	    LIST *!aliuser@ixir.com
	        identi aliuser@ixir.com olan tüm kayýtlý nickleri 
	        listeler.
	
	    LIST *Bot*!*@*
	        Bot kelimesini içeren tüm kayýtlý nickleri listeler.
	       (büyük küçük harf hassasiyeti vardýr.)
	
	    LIST *!*@*.ixir.com
	        ixir.com domainine sahip tüm kayýtlý nickleri listeler.

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
	Syntax: GLIST
	
	Lists all nicks in your group.

NICK_HELP_STATUS
	Kullanýmý: STATUS nick...
	
	Nickin kullanýcý tarafýndan nickin sahibi olarak tanýtýlýp 
	tanýtýlmadýðýný gösterir. Cevap þu formattadýr:
	
	    nick durum-kodu
	
	nick komutta yazýlan nick, ve durum-kodu da
	aþaðýdakilerden biridir:
	
	    0 - Böyle bir kullanýcý baðlý deðil veya nick 
	           kayýtlý deðil
	    1 - Kullanýcý nickin sahibi olarak tanýtýlmamýþ
	    2 - Kullanýcý nickin sahibi olarak access listesinden 
	           tanýtýlmýþ
	    3 - Kullanýcý nickin sahibi olarak þifreyle tanýtýlmýþ
	
	Her komutta en fazla 16 nick için cevap verilir; gerisi 
	yoksayýlýr. If no nickname is given, your status
	will be returned.

NICK_HELP_SENDPASS
	Kullanýmý: SENDPASS nick
	
	Belirtilen nickin þifresini nickin kayýtlarýnda belirtilmiþ 
	email adresine gönderir.
	
	Bazý networklerde sadece IRC operatörleri kullanabilir.
	
	Eðer þifreleme(encryption) aktifse bu komut devre dýþý kalýr.

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
	Servis adminleri herhangi bir nickin tanýtmasýna gerek olmadan 
	kaydýný silebilir(drop) ve herhangi bir nickin access listesini 
	görüntüleyebilir. (%R%S ACCESS LIST nick).

NICK_SERVADMIN_HELP_LOGOUT
	Kullanýmý: LOGOUT [nick [REVALIDATE]]
	
	Parametre kullanýlmazsa, IDENTIFY komutunun yaptýðý 
	etkiyi tersine çevirir, yani artýk o nickin gerçek sahibi
	olarak tanýnmamanýzý saðlar. Fakat sizden nickinizi tekrar
	identify etmeniz istenmez.
	
	Parametre kullanýlýrsada aynýsýný yapar ama buna ek olarak
	REVALIDATE belirtilirse servisler o kiþiye nickini yeniden 
	identify etmesini söyler. Servis adminlerinin Kullanýmýyla
	sýnýrlýdýr.

NICK_SERVADMIN_HELP_DROP
	Kullanýmý: DROP [nick]
	
	Parametreye gerek olmadan, %S veritabanýndan nickinizi siler.
	
	Parametreyle, belirtilen bir nicki veritabanýndan siler.
	Grubunuzda bulunan herhangi bir nicki özel ayrýcalýklara
	gerek olmadan silebilirsiniz. Bu iþlemler Servis adminleri 
	tarafýndan yapýlabilir.

NICK_SERVADMIN_HELP_INFO
	
	Servis adminleri ALL parametresini herhangi bir nick için 
	kullanabilirler.

NICK_SERVADMIN_HELP_LIST
	Kullanýmý: LIST model [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	Belirtilen modele uyuþan tüm kayýtlý nickleri nick!user@host 
	formatýnda listeler. PRIVATE özellið aktif olan kullanýcýlar 
	sadece servis adminleri tarafýndan görüntülenir. NOEXPIRE 
	ayarý olan nicklerde servis adminleri için asýlý bir ! bulunur.
	
	If the FORBIDDEN, SUSPENDED, NOEXPIRE or UNCONFIRMED options are given, only
	nicks which, respectively, are FORBIDDEN, SUSPENDED, UNCONFIRMED or have the
	NOEXPIRE flag set will be displayed. If multiple options are
	given, all nicks matching at least one option will be displayed.
	These options are limited to Services admins.  
	
	Örnekler:
	
	    LIST *!aliuser@ixir.com
	        identi aliuser@ixir.com olan tüm kayýtlý nickler 
	        listelenir.
	
	    LIST *Bot*!*@*
	        Ýsimlerinde Bot yazan tüm kayýtlý nickler listelenir
	        (küçük büyük harf hassasiyeti vardýr).
	
	    LIST * NOEXPIRE
	        Zaman asimi olmayan tüm kayýtlý nickler listelenir.

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
	Kullanýmý: GETPASS nick
	
	Belirtilen nickin þifresini söyler. Unutmayýnki bu komutun
	her kullanýlýþýnda , kullanýlan nick ve komutu kullanan
	kiþi kaydedilerek WALLOPS/GLOBOPS olarak gönderilir.
	
	Sadece Servis adminleri tarafýndan kullanýlýr.
	
	(Bu komut þifreleme(encryption) aktif olduðu zaman devre dýþýdýr.)

NICK_SERVADMIN_HELP_GETEMAIL
	Syntax: GETEMAIL user@emailhost

	Returns the matching nicks that used given email. Note that
	you can not use wildcards for either user or emailhost. Whenever
	this command is used, a message including the person who issued
	the command and the email it was used on will be logged.

	Limited to Services admins.

NICK_SERVADMIN_HELP_FORBID
	Kullanýmý: FORBID nick [sebep]
	
	Belirtilen nickin kullanýlmasýný ve kayýt edilmesini engeller.
	Nick drop edilerek iptal edilebilir.
	
	Bazý networklerde, sebep yazýlmasý gereklidir.
	
	Sadece Servis adminleri tarafýndan kullanýlýr.

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
	    GETPASS    Bir kanalýn founder þifresini ister
CHAN_HELP_CMD_FORBID
	    FORBID     Kanalý kullanýma kapatýr
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    Prevent a channel from being used preserving
	               channel data and settings
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Releases a suspended channel
CHAN_HELP_CMD_STATUS
	    STATUS     Kullanýcýnýn kanaldaki access seviyesini söyler
CHAN_HELP_CMD_REGISTER
	    REGISTER   Kanalý kaydeder
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   Þifreyle sizi kanalýn sahibi olarak tanýmlar
CHAN_HELP_CMD_SET
	    SET        Kanal özelliklerini ve bilgisini ayarlar
CHAN_HELP_CMD_AOP
	    AOP        AOP listesini düzenler
CHAN_HELP_CMD_SOP
	    SOP        SOP listesini düzenler
CHAN_HELP_CMD_ACCESS
	    ACCESS     Ayrýcalýklý kullanýcýlarý belirler deðiþtirir
CHAN_HELP_CMD_LEVELS
	    LEVELS     Access seviyelerini isteðinize göre düzenler
CHAN_HELP_CMD_AKICK
	    AKICK      Autokick listesini düzenler
CHAN_HELP_CMD_DROP
	    DROP       Kanal kaydýný siler
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Kayýp þifrelerinizi söyler
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
	%S kanallarýnýzý kayýt etmenizi ve kontrol etmenizi
	saðlar. %S kötü niyetli kullanýcýlarýn kanallarý ele 
	geçirmelerini(takeover), kanalda kimlerin op olacaðýný sizin
	tarafýnýzdan beliryerek engeller. Mevcut komutlar aþaðýda 
	listelenmiþtir; onlarý kullanmak için %R%S komut
	yazýn. Belirli bir komut hakkýnda daha fazla bilgi için,
	%R%S HELP komut yazýn.
	 
	
CHAN_HELP_EXPIRES
	
	Unutmayýn %d gün kullanýlmayan kanallarýn
	(kanalýn access listesinde olan bir kimse o süre içinde 
	kanala girmezse ) kaydý silinir.

CHAN_HELP_REGISTER
	Kullanýmý: REGISTER kanaladý þifre açýklama
	
	%S veritabanýna belirtilen kanalý ekler yani kaydeder.
	Bu komutu kullanabilmek için kayýt etmek istediðiniz kanal
	önceden kayýtlý olmamalý ve kanalda op olmalýsýnýz. Þifre
	kanal özelliklerinde deðiþiklikler yapmanýz için ilerde
	lazým olacaktýr. Son parametre kanalýn genel bir tanýmýdýr
	ve mutlaka belirtilmelidir.
	
	Kanalý kayýt ettiðiniz zaman, kanalýn "founder" 'ý olursunuz.
	Kanal founderý kanalýn tüm özelliklerini deðiþtirme yetkisine 
	sahip olan kiþidir; %S kanala girdiðiniz zaman size
	otomatik kanal opu hakkýný verecektir. Kanal opu hakkýný
	baþkalarýnada vermek için gerekli bilgiyi ACCESS komutunun
	yardýmýna bakarak alabilirsiniz (%R%S HELP ACCESS).
	
	NOT: Bir kanalý kaydedebilmek için, önce nickinizi kaydetmelisiniz.
	Eðer kaydetmediyseniz %R%s HELP yazarak nasýl yapacaðýnýz
	hakkýnda bilgi alabilirsiniz.

CHAN_HELP_IDENTIFY
	Kullanýmý: IDENTIFY kanaladý þifre
	
	%S tarafýndan kanalýn founderý olarak tanýnmanýzý saðlar.
	Birçok komutu kullanabilmek için kendinizi kanalýn sahibi
	(founder) olarak bu komutu kullanarak tanýtmanýz gerekir.
	Þifre kanalý kaydederken  kullandýðýnýz þifreyle aynýdýr.

CHAN_HELP_LOGOUT
	Kullanýmý: LOGOUT kanaladý nick
	
	Bu komut belirtilen nickin artýk belirtilen kanal için
	tanýmlanmamasýný saðlar.
	
	Eðer kanalýn founderý iseniz istediðiniz kiþi üzerinde,
	deðilseniz kendi üzerinizde bu komutu kullanabilirsiniz.

CHAN_HELP_DROP
	Kullanýmý: DROP kanaladý
	
	Kanalýn kaydýný siler.  Sadece kanal founderý tarafýndan
	kullanýlabilir ve o kiþinin önce IDENTIFY komutunu kullanarak
	kanalýn sahibi olduðunu belirtmesi gerekir.

CHAN_HELP_SET
	Kullanýmý: SET kanaladý özellik parametre
	
	Kanal founderýnýn çeþitli kanal özelliklerini ve diðer bilgileri
	ayarlamasýný saðlar.
	
	Mevcut özellikler:
	
	    FOUNDER       Kanal founderýný belirler
	    SUCCESSOR     Kanal successorýný belirler
	    PASSWORD      Founder þifresini belirler
	    DESC          Kanal açýklamasýný belirler
	    URL           Kanala bir URL adresini iliþkilendirir
	    EMAIL         Kanala bir E-mail adresini iliþkilendirir
	    ENTRYMSG      Kanala giren kullanýcýlara gönderilecek mesajý
	                   belirler
	    BANTYPE       Servislerin kanalda ne tip ban koyacaðýný belirler
	    KEEPTOPIC     Kanalda kimse olmasa bile topiði hatýrlar
	    TOPICLOCK     Topic sadece SET TOPIC komutuyla deðiþtirilir
	    MLOCK         Kanal modlarýný kitler yada açar
	    PRIVATE       LIST komutuyla kanal listelenmez
	    RESTRICTED    Kanala izinli giriþ gerektirir
	    SECURE        %S güvenlik özelliklerini aktifleþtirir
	    SECUREOPS     Kanal opu durumunun sýký kontrolünü saðlar
	    SECUREFOUNDER Kanal founderý durumunun sýký kontrolünü saðlar
	    SIGNKICK      KICK komutuyla yapýlan atmalarý iþaretler
	    OPNOTICE      OP/DEOP komutlarý kullanýldýðýnda mesaj yollar
	    PEACE         kritik komutlarin kullanilmasini engeller
	    XOP           Ayricalik sistemleri arasinda geçisi saglar

	
	Belirli bir özellik hakkýnda daha fazla bilgi için
	%R%S HELP özellik yazýn.

CHAN_HELP_SET_FOUNDER
	Kullanýmý: SET kanaladý FOUNDER nick
	
	Kanal founderýný deðiþtirir.  Yeni nickin kayýtlý 
	olmasý gereklidir.

CHAN_HELP_SET_SUCCESSOR
	Kullanýmý: SET kanaladý SUCCESSOR nick
	
	Kanal successorýný belirler, deðiþtirir. Eðer bir kanalýn
	founderýnýn nicki zaman aþýmýna uðrayýp kaydý silinirse
	yada kanal kayýtlýyken nickin kaydý bilerek silinirse(drop)
	successor kanalýn yeni founderý durumuna gelir. Fakat eðer
	successorýnda çok fazla kayýtlý kanalý varsa(%d) veya kanal
	için bir successor belirtilmemiþse kanalýn kaydý silinir(drop).
	Komutta belirtilen nickin kayýtlý olmasý gereklidir.

CHAN_HELP_SET_PASSWORD
	Kullanýmý: SET kanaladý PASSWORD þifre
	
	Kanal founderýnýn þifresini deðiþtirir.

CHAN_HELP_SET_DESC
	Kullanýmý: SET kanaladý DESC açýklama
	
	LIST ve INFO komutlarý kullanýldýðýnda gözükecek kanal 
	açýklamasýný belirler.

CHAN_HELP_SET_URL
	Kullanýmý: SET kanaladý URL [urladresi]
	
	Belirtilen URL adresini kanal ile iliþkilendirir. Bu URL 
	adresi birisi kanal hakkýnda bilgi almak için INFO 
	komutunu kullandýðýnda gözükür. Eðer urladresi parametresi
	belirtilmezse kanal için önceden belirtilen URL adresi
	silinir.

CHAN_HELP_SET_EMAIL
	Kullanýmý: SET kanaladý EMAIL [emailadresi]
	
	Belirtilen E-mail adresini kanal ile iliþkilendirir.Bu E-mail 
	adresi birisi kanal hakkýnda bilgi almak için INFO komutunu 
	kullandýðýnda gözükür. Eðer emailadresi parametresi
	belirtilmezse kanal için önceden belirtilen E-mail adresi
	silinir.

CHAN_HELP_SET_ENTRYMSG
	Kullanýmý: SET kanaladý ENTRYMSG [mesaj]
	
	Kanala giren kullanýcýlara notice þeklinde gönderilecek
	mesajý belirler.  Eðer mesaj parametresi belirtilmezse
	giren kullanýcýya bir mesaj gönderilmez.

CHAN_HELP_SET_BANTYPE
	Kullanýmý: SET kanaladý BANTYPE bantipi
	
	Servislerin birini kanaldan banlarken kullanacaðý ban 
	tipini belirler.
	
	bantipi 0 ile 3 arasýnda bir sayýdýr ve anlamlarý þudur:
	
	0: *!user@host formatýnda ban için
	1: *!*user@host formatýnda ban için
	2: *!*@host formatýnda ban için
	3: *!*user@*.domain formatýnda ban için

CHAN_HELP_SET_KEEPTOPIC
	Kullanýmý: SET kanaladý KEEPTOPIC {ON | OFF}
	
	Bir kanal için topic hatýrlamayý aktifleþtirir yada kapatýr.
	topic hatýrlama aktifse kanal kapansa bile kanal topic i
	%S tarafýndan hatýrlanýr ve birisi kanala girdiðinde bu
	topic geçerli olur.

CHAN_HELP_SET_TOPICLOCK
	Kullanýmý: SET kanaladý TOPICLOCK {ON | OFF}
	
	Bir kanal için topic kilidini aktifleþtirir yada kapatýr.
	topic kilidi aktifse, %S SET TOPIC komutunun
	dýþýnda kanal topic inin deðiþtirilmesine izin vermez.

CHAN_HELP_SET_MLOCK
	Kullanýmý: SET kanaladý MLOCK modlar
	
	Bir kanal için mod-kilidini aktifleþtirir. %S belirlediðiniz
	modlarýn her zaman açýk yada kapalý olacaðýný ayarlar.

	+ ile belirtilen modlar açik olarak, - ile belirtilen modlar
	kapali olarak kitlenir.
	
	Dikkat:  Eðer ikinci örnekteki gibi bir mod kilidi koyarsanýz,
	kanalýn RESTRICTED özelliðinide aktifleþtirmeniz gerekir
	(HELP SET RESTRICTED e bakýn), yoksa kanal boþken ilk
	giren kiþi kanal key ini(anahtar) görebilir!
	
	Örnekler:
	
	    SET #kanaladý MLOCK +nt-iklps
	        n ve t modlarýný hep açýk, i, k, l, p, ve s modlarýný hep
	        açýk olarak ayarlar. m modu ise istenildiði gibi ayarlanabilir
	        açýk yada kapalý.
	
	    SET #kanaladý MLOCK +knst-ilmp anahtar
	        k, n, s, ve t modlarý hep açýk, ve i, l, m, ve p modlarý
	        hep kapalý. Ayrýca kanal anahtarýný belirler ve hep o 
	        anahtar olmasýný saðlar.
	
	    SET #kanaladý MLOCK +
	        Mod kilidini kaldýrýr; ve tüm kanal modlarýný açýlýp 
	        kapanacak þekilde serbest býrakýr.

CHAN_HELP_SET_PEACE
	Kullanimi: SET kanaladi PEACE {ON | OFF}
	
	Bir kanalin peace özelligini açar veya kapatir.
	peace özelligi ayarlandiginda, hic kimse seviyesi kendinden
	yüksek veya esit olanlari atamaz, banlayamaz veya %S
	seviyesini degistiremez.

CHAN_HELP_SET_PRIVATE
	Kullanýmý: SET kanaladý PRIVATE {ON | OFF}
	
	Bir kanal için private özelliðini açar kapatýr. private özelliði
	aktifse, %R%S LIST komutuyla kanal listelenemez.

CHAN_HELP_SET_RESTRICTED
	Kullanýmý: SET kanaladý RESTRICTED {ON | OFF}
	
	Bir kanal için izinli giriþ özelliðini açar yada kapatýr.
	izinli giriþ özelliði aktifse, kanal opu özelliði olmayan
	kullanýcýlar(negatif access seviyesi olanlar, secure ops
	özelliði aktif olanlar, ve access listesinde olmayan 
	kullanýcýlar) kanaldan atýlýp banlanýrlar.

CHAN_HELP_SET_SECURE
	Kullanýmý: SET kanaladý SECURE {ON | OFF}
	
	Bir kanal için %S'ün güvenlik özelliklerini açar veya 
	kapatýr. SECURE özelliði aktifse, sadece nicki kayýtlý
	ve %s ile nickini tanýtmýþ kullanýcýlar kanalda access 
	alabilirler ve access listesindeki statülerini
	alabilirler.(mesela op olabilirler)

CHAN_HELP_SET_SECUREOPS
	Kullanýmý: SET kanaladý SECUREOPS {ON | OFF}
	
	Bir kanal için güvenli op özelliðini açar veya kapatýr.
	güvenli op özelliði aktifse, access listesinde olmayan
	kullanýcýlar kanalda op olamazlar.

CHAN_HELP_SET_SECUREFOUNDER
	Kullanýmý: SET kanaladý SECUREFOUNDER {ON | OFF}
	
	Bir kanal için güvenli founder özelliðini açar yada kapatýr.
	güvenli founder özelliði aktifse, sadece gerçek founder 
	kanalýn kaydýný silebilir, kanal þifresini deðiþtirebilir yada
	kanal founderýný ve successor ýný deðiþtirebilir. Kanal 
	þifresiyle kendilerini %S'e kanal founderý olarak tanýtan 
	ama nicki founderýn nicki olmayanlar bu komutlarý kullanamazlar.

CHAN_HELP_SET_SIGNKICK
	Kullanýmý: SET kanaladý SIGNKICK {ON | LEVEL | OFF}
	
	Bir kanal için iþaretli atmayý açar yada kapatýr.
	SIGNKICK ayarlandýðý zaman, %S KICK
	komutuyla yapýlan atmalarda komutu kullanan kiþinin
	nicki atma sebebinde yer alýr.
	
	Eðer LEVEL parametresini kullanýrsanýz, seviyesi
	SIGNKICK seviyesine eþit yada fazla olanlarýn yaptýðý
	atmalarda atanýn nicki sebepte gözükmez. Daha fazla
	bilgi için %R%S HELP LEVELS yazýn.

CHAN_HELP_SET_XOP
	Kullanimi: SET kanaladi XOP {ON | OFF}
	
	Bir kanalin xOP sistemini açar veya kapatir. XOP
	ayarlandigi zaman, AOP/SOP/VOP komutlariyla
	yetkileri düzenlemelisiniz, aksi takdirde ACCESS
	komutunu kullanmalisiniz.
	
	Teknik Not: Access sisteminden xOP sisteminde
	geçtiginizde seviye ayarlariniz degistirilir, bu
	yüzden tekrar access sistemine geçtiginizde ayni
	ayarlari bulamazsiniz! 
	
	Access sisteminden xOP sistemine geçtiginizde listedeki
	ayricaliklari kontrol edip listenizdeki nicklerin dogru
	xOP ayarlarinda bulundugunu kontrol edin, çünkü sistemin
	tahmin etmesi her zaman dogru olmayabilir... Eger LEVELS
	komutuyla komut seviye ayarlarini degistirdiyseniz xOP
	sistemini kullanmaniz önerilmez.
	
	Bunlarin disinda xOP sisteminden access sistemine geçmek
	bir problem çikarmaz.

CHAN_HELP_SET_OPNOTICE
	Kullanýmý: SET kanaladý OPNOTICE {ON | OFF}
	
	Bir kanal için op-notu özelliðini açar yada kapatýr.
	op-notu özelliði aktifse, %S kanala bir kullanýcý 
	üzerinde OP veya DEOP komutlarýnýn kullanýldýðýný
	belirten bir mesaj gönderir.

CHAN_HELP_AOP
	Kullanýmý: AOP kanaladi ADD nick
	           AOP kanaladi DEL {nick | kayit-no | liste}
	           AOP kanaladi LIST [mask | liste]
	           AOP kanaladi CLEAR
	
	Bir kanalin AOP (otoop) listesini düzenler. AOP listesi
	o kanalda kullaniciya otomatik olarak op olma, gerekirse 
	kendi banlarini kaldirma veya davet edilme, giriste karsilama
	mesajlarini görüntüleme, vs yetkilerini verir.
	
	AOP ADD komutu belirtilen nicki AOP listesine ekler.
	
	AOP DEL komutu belirtilen nicki AOP listesinden siler.  
	Eger kayit numaralari belirtilirse, o numaralara ait nickler
	silinir. (Asagidaki örnegi inceleyin.)
	
	AOP LIST komutu AOP listesini gösterir. Eger bir hostmask
	belirtilmisse, sadece onla uyusan kayitlari gösterir. Eger 
	kayit numaralari belirtilmisse o numaralarla uyusan kayitlari
	gösterir; örnegin:

	   AOP #kanal LIST 2-5,7-9
	      2 den 5 e ve 7 den 9 a kadar olan kayitlari gösterir.
	      
	AOP CLEAR komutu AOP listesindeki tüm kayitlari siler.
	
	AOP ADD ve AOP DEL komutlarini kanal SOP lari veya üstü
	kullanabilir. AOP CLEAR komutunu ise sadece kanal founderi
	kullanabilir. Ayrica, AOP listesindeki herhangi biri AOP LIST 
	komutunu kullanabilir.
	
	Bu komutu kanaliniz için devre disi birakabilirsiniz. O zaman
	access komutlarini kullanmalisiniz. Access komutlari hakkinda
	bilgi almak için  %R%S HELP ACCESS yazin. 
	Access ile xOP arasinda geçis yapabilmek için gerekli bilgilere
	%R%S HELP SET XOP yazarak ulasabilirsiniz.

CHAN_HELP_HOP
	Kullanýmý: HOP kanaladi ADD nick
	           HOP kanaladi DEL {nick | kayit-no | liste}
	           HOP kanaladi LIST [mask | liste]
	           HOP kanaladi CLEAR
	
	Bir kanalin HOP (HalfOP) listesini düzenler. HOP
	listesinde bulunan bir kisi kanalda otomatik olarak
	halfop olur.
	
	HOP ADD komutu belirtilen nicki HOP listesine ekler.
	
	HOP DEL komutu belirtilen nicki HOP listesinden siler.
	Eger kayit numaralari listesi belirtilirse, bu kayitlar
	silinir. (Asagidaki örnegi inceleyin.)
	
	HOP LIST komutu HOP listesini gösterir. Eger bir hostmask
	belirtilmisse, o hostmask la uyusan kayitlar gösterilir.
	Eger kayit numaralari listesi belirtilmisse, o numaralardaki
	nickler gösterilir; örnegin:

	   HOP #kanaladi LIST 2-5,7-9
	       2 den 5 e ve 7 den 9 a kadar olan kayitlari gösterir.
	      
	HOP CLEAR komutu HOP listesindeki tüm kayitlari temziler.
	
	HOP ADD, HOP DEL ve HOP LIST komutlarini AOP lar ve üstü
	HOP CLEAR komutunu ise sadece founder kullanabilir.
	
	Bu komutu kanaliniz için devre disi birakabilirsiniz. O zaman
	access komutlarini kullanmalisiniz. Access komutlari hakkinda
	bilgi almak için  %R%S HELP ACCESS yazin. 
	Access ile xOP arasinda geçis yapabilmek için gerekli bilgilere
	%R%S HELP SET XOP yazarak ulasabilirsiniz.

CHAN_HELP_SOP
	Kullanýmý: SOP kanaladi ADD nick
	           SOP kanaladi DEL {nick | kayit-no | liste}
	           SOP kanaladi LIST [mask | liste]
	           SOP kanaladi CLEAR
	
	Bir kanalin SOP (SuperOP) listesini düzenler. SOP
	listesindekiler AOP larin sahip oldugu tüm yetkilere 
	sahiptir. Ayrica AutoKick ve BadWords listesini
	düzenleyebilirler, kanal memolarini okuyabilirler, vs.
	
	SOP ADD komutu belirtilen nicki SOP listesine ekler.
	
	SOP DEL komutu belirtilen nicki SOP listesinden siler.
	Eger kayit numaralari listesi belirtilmisse, bu kayitlar
	silinir. (Asagidaki örnegi inceleyin.)
	
	SOP LIST komutu SOP listesini görüntüler. Eger bir
	hostmask belirtilmisse o hostmaskla uyusan kayitlar
	gösterilir. Eger kayit numaralari listesi belirtilmisse
	o numaralardaki nickler gösterilir; örnegin:

	   SOP #kanaladi LIST 2-5,7-9
	      2 den 5 e ve 7 den 9 a kadar olan kayitlar gösterilir.
	      
	SOP CLEAR komutu SOP listesindeki tüm kayitlari temizler.
	
	SOP ADD, SOP DEL ve SOP CLEAR komutlarini sadece kanal
	founderi kullanabilir. Ama AOP listesindeki herhangi biri
	SOP LIST komutunu kullanabilir.
	
	Bu komutu kanaliniz için devre disi birakabilirsiniz. O zaman
	access komutlarini kullanmalisiniz. Access komutlari hakkinda
	bilgi almak için  %R%S HELP ACCESS yazin. 
	Access ile xOP arasinda geçis yapabilmek için gerekli bilgilere
	%R%S HELP SET XOP yazarak ulasabilirsiniz.

CHAN_HELP_VOP
	Kullanýmý: VOP kanaladi ADD nick
	           VOP kanaladi DEL {nick | kayit-no | liste}
	           VOP kanaladi LIST [mask | liste]
	           VOP kanaladi CLEAR
	
	Bir kanalin VOP (otovoice) listesini düzenler. VOP
	listesindekiler kanalda otomatik olarak voice alirlar.
	
	VOP ADD komutu belirtilen nicki VOP listesine ekler.
	
	VOP DEL komutu belirtilen nicki VOP listesindene siler.
	Eger kayit numaralari listesi belirtilmisse, o kayitlar
	silinir. (Asagidaki örnegi inceleyin.)
	
	VOP LIST komutu VOP listesini görüntüler. Eger bir
	hostmask belirtilmisse o hostmaskla uyusan kayitlar
	listelenir. Eger kayit numaralari listesi belirtilmisse
	o numaralardaki nickler gösterilir; örnegin:

	   VOP #kanaladi LIST 2-5,7-9
	      2 den 5 e ve 7 den 9 a kadar olan kayitlar gösterilir.
	      
	VOP CLEAR komutu VOP listesindeki tüm kayitlari temizler.
	
	VOP ADD, VOP DEL ve VOP LIST komutlarini AOP lar ve üstü,
	VOP CLEAR komutunu ise sadece founder kullanabilir.
	
	Bu komutu kanaliniz için devre disi birakabilirsiniz. O zaman
	access komutlarini kullanmalisiniz. Access komutlari hakkinda
	bilgi almak için  %R%S HELP ACCESS yazin. 
	Access ile xOP arasinda geçis yapabilmek için gerekli bilgilere
	%R%S HELP SET XOP yazarak ulasabilirsiniz.

CHAN_HELP_ACCESS
	Kullanýmý:  ACCESS kanaladý ADD nick seviye
	            ACCESS kanaladý DEL {nick | kayýt-no | liste}
	            ACCESS kanaladý LIST [mask | liste]
	            ACCESS kanaladý CLEAR
	
	Bir kanal için access listesi oluþturur. Access listesi
	kanalda kimlerin op olacaðýný ve %S komutlarýna
	kimlerin eriþimi olacaðýný belirtir. Deðiþik access
	seviyeleri deðiþik komutlara eriþim saðlar. Ayrýntýlý bilgi
	için %R%S HELP ACCESS LEVELS yazýn. Access
	listesinde olmayan kullanýcýlarýn seviyesi 0 dýr.
	
	ACCESS ADD komutu belirtilen nicki belirtilen seviyeden
	access listesine ekler; eðer o kiþi zaten access listesindeyse
	seviyesini belirtilen seviyeyle deðiþtirir. Belirtilen seviye 
	komutu kullanan kiþinin seviyesinden az olmalýdýr, eðer 
	belirtilen nick zaten access listesindeyse komutu kullanan 
	kiþinin access seviyesi onunkinden büyük olmalýdýr.
	
	ACCESS DEL komutu belirtilen nicki access listesinden siler.
	eðer kayýt numarasý belirtilmiþse o kayýt numarasý olan kiþinin 
	accessi silinir.(Aþaðýdaki LIST için verilen örneðe bakýn.)
	
	ACCESS LIST komutu access listesini görüntüler. Eðer * lý 
	bir mask belirtilirse o maskla uyuþan kiþiler listelenir. 
	Eðer kayýt numarasý listesi belirtilirse o kiþiler listelenir.
	Örneðin:
	
	   ACCESS #kanaladý LIST 2-5,7-9
	      2'den 5'e ve 7'den 9'a kadar olan access kayýtlarý
	      listelenir.
	      
	ACCESS CLEAR komutu tüm access listesini temizler.

CHAN_HELP_ACCESS_LEVELS
	Kullanýcý access seviyeleri
	
	Varsayýlan olarak, aþaðýdaki access seviyeleri tanýmlanmýþtýr:
	
	   Founder   %S fonksiyonlarýna tam eriþim hakký vardýr.
	                     kanal girer girmez op olur. Unutmayýnki
	                     sadece bir kiþi founder olma hakkýna 
	                     sahiptir. (ACCESS komutuyla verilemez.)
	        10   AKICK komutuna eriþimi vardýr; otomatik oplanýr.
	         5   Otomatik oplanýr.
	         3   Otomatik voice lanýr.
	         0   Bir özelliði yoktur; diðer oplar tarafýndan
	                     oplanabilir (güvenli-op kapalýysa).
	        <0   Kanalda oplanamaz.
	
	Bu seviyeler LEVELS komutu kullanýlarak deðiþtirilebilir, veya
	yenileri eklenebilir, bilgi için %R%S HELP LEVELS 
	yazýn.

CHAN_HELP_AKICK
	Kullanýmý:  AKICK channel ADD {nick | mask} [reason]
	            AKICK channel STICK mask
	            AKICK channel UNSTICK mask
	            AKICK channel DEL {nick | mask | entry-num | list}
	            AKICK channel LIST [mask | entry-num | list]
	            AKICK channel VIEW [mask | entry-num | list]
	            AKICK kanaladý ENFORCE
	            AKICK kanaladý CLEAR
	
	Bir kanal için AutoKick listesi oluþturur.  Eðer Akick 
	listesindeki bir kullanýcý kanala girmeye çalýþýrsa, Bu
	kiþi %S tarafýndan banlanýr ve kanaldan atýlýr.
	
	AKICK ADD komutu belirtilen nicki yada maský Akick
	listesine ekler. Eðer komutla beraber birde sebep
	belirtilmiþse bu sebep kullanýcý kanaldan atýlýrken
	kullanýlýr; eðer kullanýlmazsa geçerli sebep olan
	"You have been banned from the channel" kullanýlýr.

	AKICK STICK komutu belirtilen maski kalici olarak
	banlar. Eger biri bani kaldirmaya çalisirsa, %S
	otomatik olarak yeniden banlar. Bunu kayitli nickler
	için kullanamazsiniz.
	
	AKICK UNSTICK komutu AKICK STICK komutunun etkilerini
	iptal eder, böylece bani kaldirabilirsiniz.
	
	AKICK DEL komutu belirtilen nicki yada maský Akick
	listesinden siler. Fakat bu komutla önceden Akick 
	listesindeyken banlanmýþ birinin baný manuel olarak
	açýlana kadar kalkmaz.
	
	AKICK LIST komutu Akick listesinde kimlerin olduðunu
	listeler, yada opsiyonel olarak belirtilen maskla uyuþan 
	Akick listesindeki kullanýcýlarý listeler.
	
	AKICK VIEW komutu AKICK LIST komutunun daha açýklayýcý
	versiyonudur.
	
	AKICK ENFORCE komutu %S'ün mevcut akick listesinde bulunan 
	masklara sahip kullanýcýlarý kanaldan atmasýný saðlar.
	
	AKICK CLEAR komutu akick listesinde bulunan tüm kayýtlarý
	siler.

CHAN_HELP_LEVELS
	Kullanýmý:  LEVELS kanaladý SET tip seviye
	            LEVELS kanaladý {DIS | DISABLE} tip
	            LEVELS kanaladý LIST
	            LEVELS kanaladý RESET
	
	LEVELS komutu bir kanaldaki access seviyelerinin iyi
	kontrolünü saðlar. Bu komutla, %S'ün birçok
	fonksiyonu için gereken access seviyelerini
	belirleyebilirsiniz. (SET FOUNDER ve SET PASSWORD
	komutlarý ve bu komut sadece kanal founderý tarafýndan
	kullanýlabilir.)
	
	LEVELS SET komutu bir fonksiyon yada fonksiyon gruplarý için
	access seviyelerini deðiþtirmeye yarar. LEVELS DISABLE (veya
	kýsaca DIS) otomatik bir özelliði kapatýr yada founderdan 
	baþkasýnýn kullanmasýný engeller. LEVELS LIST komutu 
	fonksiyonlar yada fonksiyon gruplarý için mevcut seviyeleri 
	gösterir. LEVELS RESET komutu seviyeleri varsayýlan deðerlerine
	geri çevirir. (HELP ACCESS LEVELS e bakýn).
	
	Seviyeleri ayarlanabilecek fonksiyonlar için, HELP LEVELS DESC e 
	bakýn.

CHAN_HELP_LEVELS_DESC
	Aþaðýda seviyeleri deðiþtirilebilecek  fonksiyonlar listelidir.
	Dikkat edilirse AUTODEOP ve NOJOIN deðerleri maximum, diðerleri
	minimum seviyelerdir.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Kullanýmý: INFO kanaladý [ALL]
	
	Belirtilen kayýtlý kanalla ilgili bilgiler sunar. Bu 
	bilgilerin içeriðinde kanalýn founderý, kayýt zamaný,
	son kullanýldýðý zaman, açýklamasý, ve varsa mod kilidi
	bulunur. Eðer kanalýn founderý olarak kendinizi 
	tanýtmýþsanýz ve ALL parametresini belirtmiþseniz
	giriþ mesajý ve successor hakkýndada bilgi alabilirsiniz.

	Varsayilan olarak, ALL seçenegini founderlar kullanabilir.

CHAN_HELP_LIST
	Kullanýmý: LIST model
	
	Belirtilen modelle uyuþan tüm kayýtlý kanallarý listeler.
	(PRIVATE özelliði aktif olan kanallar listelenmez.)

CHAN_HELP_OP
	Kullanýmý: OP [#kanaladý [nick]]
	
	Belirtilen nicki kanalda oplar. Eðer nick belirtilmemiþse,
	kendinizi oplarsýnýz. Eðer kanal ve nick belirtilmemiþse,
	bulunduðunuz tüm kanalda sizi oplar, tabi eðer op accessiniz
	ve bu komutu kullanmaya yetkiniz varsa.
	
	Varsayýlan olarak, AOP lar ile access seviyesi 5 ve üzeri
	olanlar kullanabilir.

CHAN_HELP_DEOP
	Kullanýmý: DEOP [#kanaladý [nick]]
	
	Belirtilen nicki kanalda deoplar. Eðer nick belirtilmemiþse,
	kendinizi deoplarsýnýz. Eðer nick ve kanal belirtilmemiþse,
	bulunduðunuz tüm kanallarda sizi deoplar, tabi eðer bu komutu 
	kullanmaya yetkiniz varsa.
	
	Varsayýlan olarak, AOP lar veya access seviyesi 5 ve üzeri
	olanlar kullanabilir.

CHAN_HELP_VOICE
	Kullanýmý: VOICE [#kanaladý [nick]]
	
	Belirtilen nicke kanalda voice verir. Eðer nick belirtilmemiþse,
	size voice verir. Eðer kanal ve nick belirtilmemiþse, size
	bulunduðunuz tüm kanallarda voice verir, tabi eðer accessiniz ve
	bu komutu kullanmaya izniniz varsa.
	
	Varsayýlan olarak, AOP lar ile access seviyesi 5 ve üzeri olanlar
	kullanabilir, veya VOP lar ile access seviyesi 3 ve yukarý olanlar
	kendilerine voice vermek için kullanabilirler.

CHAN_HELP_DEVOICE
	Kullanýmý: DEVOICE [#kanaladý [nick]]
	
	Belirtilen nicki kanalda devoice eder. Eðer nick belirtilmemiþse,
	sizi devoice eder. Eðer kanal ve nick belirtilmemiþse sizi,
	bulunduðunuz tüm kanallarda devoice eder, tabi eðer accessiniz
	ve bu komutu kullanmaya izniniz varsa.
	
	Varsayýlan olarak, AOP lar, access seviyesi 5 ve üzeri olanlar
	kullanabilir, veya VOP lar ile access seviyesi 3 ve üzeri olanlar
	kendilerini devoice etmek için kullanabilirler.

CHAN_HELP_HALFOP
	Kullanýmý: HALFOP [#kanaladý [nick]]
	
	Belirtilen nicki kanalda halfop(yarýmop) yapar. Eðer nick
	belirtilmemiþse, sizi halfop yapar. Eðer nick ve kanal
	belirtilmemiþse, sizi bulunduðunuz tüm kanallarda halfop yapar,
	tabi eðer bu komutu kullanmaya izniniz varsa.
	
	Varsayýlan olarak, AOP lar ile access seviyesi 5 ve üzeri
	olanlar kullanabilir, veya HOP lar ile access seviyesi 4 ve
	üzeri olanlar kendilerini halfop yapmak için kullanabilir.

CHAN_HELP_DEHALFOP
	Kullanýmý: DEHALFOP [#kanaladý [nick]]
	
	Belirtilen nickin halfopunu(yarýmop) alýr. Eðer nick
	belirtilmemiþse, sizin halfopunuz alýr. Eðer kanal ve nick 
	belirtilmemiþse, bulunduðunuz tüm kanallardan halfopunuzu
	alýr, tabi eðer bu komutu kullanmaya izniniz varsa.
	
	Varsayýlan olarak, AOP lar veya 5 ve üzeri access seviyesi
	olanlar kullanabilir, ayrica HOP lar veya access seviyesi
	4 ve üzeri olanlar kendi halfoplarýný alabilirler.

CHAN_HELP_PROTECT
	Kullanýmý: PROTECT [#kanaladý [nick]]
	
	Belirtilen nicki kanalda korur. Eðer nick belirtilmezse,
	sizi korur. Eðer nick ve kanal belirtilmezse sizi bulunduðunuz
	tüm kanallarda koruyacaktýr, tabi eðer bu komutu kullanmaya
	izniniz varsa.
	
	Varsayýlan olarak, founder kullanabilir, veya SOP lar ile access
	seviyesi 10 ve üzeri olanlar kendilerinde kullanabilirler.

CHAN_HELP_DEPROTECT
	Kullanýmý: DEPROTECT [#kanaladý [nick]]
	
	Belirtilen nickin korumasýný kaldýrýr. Eðer nick belirtilmemiþse,
	sizin korumanýzý kaldýrýr. Eðer kanal ve nick belirtilmezse, 
	sizin bulunduðunuz tüm kanallardaki korumalarýnýz kaldýrýr, 
	tabi eðer bu komutu kullanmaya izniniz varsa.
	
	Varsayýlan olarak, founder kullanabilir, veya SOP lar ile
	access seviyesi 10 ve üzeri olanlar kendilerinde kullanabilir.

CHAN_HELP_OWNER
	Kullanýmý: OWNER [#kanaladi]
	
	Belirtilen kanalda owner statüsü verilir. Eger kanal
	belirtilmemisse bulundugunuz tüm kanallarda size owner
	statüsü verilir. Tabi bu yetkinizin olup olmamasina
	baglidir.
	
	Belirtilen kanal founderlarinin kullanma yetkisi vardir.

CHAN_HELP_DEOWNER
	Kullanýmý: DEOWNER [#kanaladi]
	
	Belirtilen kanaldaki owner statünüzü kaldirir. Eger
	kanal belirtilmezse bulundugunuz tüm kanallardaki owner
	statünüzü kaldirir. Tabi bu yetkinizin olup olmamasina
	baglidir.
	
	Belirtilen kanal founderlarinin kullanma yetkisi vardir.

CHAN_HELP_INVITE
	Kullanýmý: INVITE kanaladý
	
	%S'e sizi belirtilen kanala davet etmesini söyler.

	Varsayýlan olarak AOP lar ile access seviyesi 5 veya
	üzeri olanlar kullanabilir.

CHAN_HELP_UNBAN
	Kullanýmý: UNBAN kanaladý
	
	%S'e sizin kanala girmenizi engelleyen tüm banlarý kaldýrmasýný
	söyler.

	Varsayýlan olarak, AOP lar ile 5 ve üzeri access seviyesi olanlar
	kullanabilir.

CHAN_HELP_KICK
	Kullanýmý: KICK [#kanaladý [nick [sebep]]]
	
	Belirtilen nicki kanaldan atar. Eðer nick belirtilmemiþse
	sizi atar. Eðer kanal ve nick belirtilmemiþse, sizi 
	bulunduðunuz tüm kanallardan atar.
	
	Varsayýlan olarak, AOP lar ile access seviyesi 5 ve üzeri
	olanlar bu komutu kullanabilir.

CHAN_HELP_BAN
	Kullanýmý: BAN [#kanaladi [nick [sebep]]]
	
	Belirtilen nicki kanaldan banlar. Eger nick belirtilmezse
	sizi banlar. Eger nick ve kanal belirtilmezse sizi bulundugunuz
	tüm kanallardan banlar.
	
	Varsayilan olarak, AOP larin veya 5. seviyeden access i olanlarin
	ve üstünün kullanma yetkisi vardir.

CHAN_HELP_TOPIC
	Kullanýmý: TOPIC kanaladi [konu]
	
	%S belirtilen kanala belirtilen konuyu yazar.Eger
	topic belirtilmemisse, bos bir konu yazilir. Bu
	komut SET TOPICLOCK aktif oldugu zaman kullanislidir.
	Daha fazla bilgi için %R%S HELP SET TOPICLOCK
	yazin.
	
	Varsayilan olarak, founderin kullanma yetkisi vardir.

CHAN_HELP_CLEAR
	Kullanýmý: CLEAR kanaladý neyi
	
	%S'e belirtilen ayarý kaldýrmasýný söyler. neyi
	þunlardan biri olmalýdýr:
	
	     MODES    Kanal modlarýný temizler. (ör. i,k,l,m,n,p,s,t
	                   modlarýný temizler).
	     BANS     Kanaldaki tüm banlarý kaldýrýr.
	     EXCEPTS  Clears all excepts on the channel.
	     OPS      Kanal opu statüsünde olanlarýn (mode +o)
	                  oplarýný alýr.
	     HOPS     Removes channel-halfoperator status (mode +h) from
	                  all channel halfoperators, if supported.
	     VOICES   Voice statüsünde olanlarýn (mode +v) voice larýný
	                  alýr.
	     USERS    Tüm kullanýcýlarý kanaldan atar.
	
	Varsayýlan olarak, founder tarafindan kullanilabilir.

CHAN_HELP_GETKEY
	Kullanýmý: GETKEY kanaladý
	
	Belirtilen kanalýn anahtarýný söyler. Bu komut botlar
	veya scriptler tarafýndan kullanýlmak üzere tasarlanmýþtýr,
	ve çýktýsý þu þekildedir:
	
	KEY <kanaladý> <anahtar>
	
	Eðer bir anahtar yoksa anahtar kýsmýnda "NO KEY" olacaktýr.

CHAN_HELP_SENDPASS
	Kullanýmý: SENDPASS kanaladý
	
	Kanalýn þifresini founderýn nick kayýtlarýnda belirtilen
	e-mail adresine gönderir. Bu komut kanal þifresini 
	unutanlarýn çok iþine yarayacaktýr.
	
	Bazý networklerde IRC operatörlerinin Kullanýmýyla 
	sýnýrlandýrýlmýþ olabilir.
	
	Bu komut þifreleme(encryption) aktifken devre dýþýdýr.

CHAN_SERVADMIN_HELP
	
	Servis adminleri kanalýn kaydýný þifreyle tanýtmadan silebilir,
	ve access, Akick ve seviye ayarlarýný görüntüleyebilirler.

CHAN_SERVADMIN_HELP_LOGOUT
	Kullanýmý: LOGOUT kanaladý [nick]
	
	Bu komut belirtilen nickin artýk belirtilen kanalda
	tanýmlanmamasýný saðlar.
	
	Eðer kanal founderý iseniz, istediðiniz kiþide, deðilseniz
	sadece kendinizde bu komutu kullanabilirsiniz.
	
	Eðer Servis admini iseniz, kanal founderý olmanýza gerek
	olmadan istediðiniz kiþide bu komutu kullanabilirsiniz.
	Hatta nick parametresini boþ býrakarak o kanalda kendini
	kanala tanýtmýþ herkesi tanýmlanmamýþ yapabilirsiniz.

CHAN_SERVADMIN_HELP_DROP
	Kullanýmý: DROP kanaladý
	
	Belirtilen kanalýn kaydýný siler. Sadece Servis adminleri
	kanalý tanýtmadan kaydýný silebilirler.

CHAN_SERVADMIN_HELP_SET
	
	Servis adminleri NOEXPIRE özelliðini ayarlayarak istedikleri
	kanalýn zaman aþýmýna uðramasýný engelleyebilirler.
	Ek olarak, Servis adminleri þifreyle kanalý tanýtmaya gerek 
	olmadan bir kanalýn tüm özelliklerini ayarlayabilirler.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Kullanýmý: SET kanaladý NOEXPIRE {ON | OFF}
	
	Belirtilen kanalýn zaman asimina uðrayýp uðramayacaðýný 
	belirler. Bunu ON olarak ayarlamak kanalýn zaman aþýmýna 
	uðramasýný engelleyecektir.
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

CHAN_SERVADMIN_HELP_INFO
	
	Servis adminleri herhangi bir kanal için ALL parametresini
	kullanabilirler.

CHAN_SERVADMIN_HELP_LIST
	Kullanýmý: LIST model [FORBIDDEN] [NOEXPIRE]
	
	Belirtilen modelle uyuþan tüm kayýtlý kanallarý listeler.
	PRIVATE özelliði ayarlý kanallar sadece Servis adminleri
	tarafýndan görüntülenir. NOEXPIRE özelliði olan kanallarýn
	yanýnda Servis adminleri için ! iþareti bulunur.
	
	Eðer FORBIDDEN veya NOEXPIRE özellikleri belirtilmiþse,
	sadece yasaklý(FORBIDDEN) yada zaman aþýmýna(NOEXPIRE) 
	uðramayacak kanallar listelenir. Eðer her iki özellikte 
	belirtilmiþse, her iki tip kanalda listelenir. Bu komutlar
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

CHAN_SERVADMIN_HELP_GETPASS
	Kullanýmý: GETPASS kanaladý
	
	Belirtilen kanalýn þifresini söyler. Unutmayýnki bu
	komut ne zaman kullanýlsa, Komutu kullananýn ve komutta
	kullanýlan kanalýn adý kaydedilir ve WALLOPS/GLOBOPS olarak
	gönderilir.
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

CHAN_SERVADMIN_HELP_FORBID
	Kullanýmý: FORBID kanaladý [sebep]
	
	Belirtilen kanalýn birisi tarafýndan kayýt edilmesini veya 
	kullanýlmasýný önler. Kanal droplanarak iptal edilir.
	
	Bazý networklerde sebep gereklidir.
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

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
	Kullanýmý: STATUS kanaladý nick
	
	Belirtilen nickin belirtilen kanaldaki access seviyesini
	söyler. Cevabýn formatý þu þekildedir:
	
	    STATUS kanaladý nick access-seviyesi
	
	Eðer bir hata olursa, cevabun formatý þöyle olur:
	
	    STATUS ERROR hata-mesajý
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP_CMD_SEND
	    SEND   Nicke veya kanala mesaj gönderir
MEMO_HELP_CMD_CANCEL
	    CANCEL Son gönderilen mesajý iptal eder
MEMO_HELP_CMD_LIST
	    LIST   Mesajlarýnýzý listeler
MEMO_HELP_CMD_READ
	    READ   Mesajlarý okumanýzý saðlar
MEMO_HELP_CMD_DEL
	    DEL    Mesajlarý silmenizi saðlar
MEMO_HELP_CMD_SET
	    SET    Mesajlarla ilgili ayarlarý yapar
MEMO_HELP_CMD_INFO
	    INFO   Displays information about your memos
MEMO_HELP_CMD_RSEND
	    RSEND  Sends a memo and requests a read receipt
MEMO_HELP_CMD_CHECK
	    CHECK  Checks if last memo to a nick was read
MEMO_HELP_CMD_SENDALL
	    SENDALL  Send a memo to all registered users
MEMO_HELP_CMD_STAFF
	    STAFF      Send a memo to all opers/admins

MEMO_HELP_HEADER
	%S kullanýcýlarýn birbirlerine veya kanallara(*) online 
	olmasalar bile mesaj göndermelerini saðlar. Mesaj gönderebilmek
	için gönderenin nicki, gönderilenin nicki veya kanalýn kayýtlý
	olmasý gereklidir.
	
	%S'ün mevcut komutlarý:
	

MEMO_HELP_ADMIN
	not used.
MEMO_HELP_OPER
	not used.
MEMO_HELP_FOOTER
	
	Yukardaki herhangi bir komut hakkýnda yardým için 
	%R%S HELP komut yazýn.
	
	(*) Varsayýlan olarak, kanal mesajlarýný o kanalda access
	    seviyesi en az 10 olanlar okuyabilir. Bu %s LEVELS 
	    komutuyla deðiþtirebilir.

MEMO_HELP_SEND
	Kullanýmý: SEND {nick | kanaladý} mesaj
	
	Belirtilen nicke veya kanala yazýlan mesajý gönderir.
	Nicke gönderirken, gönderilen kiþi yeni mesajý olduðuna dair
	bir not alýr. Mesaj gönderilen nick/kanal kayýtlý olmalýdýr.

MEMO_HELP_CANCEL
	Kullanýmý: CANCEL {nick | kanaladý}
	
	Belirtilen kanala yada nicke gönderilen son mesajý iptal eder,
	tabi eðer o mesaj siz komutu kullandýðýnýz sýrada okunmamýþsa.

MEMO_HELP_LIST
	Kullanýmý: LIST [kanaladý] [liste | NEW]
	
	Mevcut mesajlarýnýzý listeler. NEW parametresi sadece yeni
	okunmamýþ mesajlarý listeler. Okunmamýþ mesajlar mesaj 
	numarasýnýn solunda "*" ile iþaretlidir. Aþaðýdaki örnekte
	olduðu gibi belirli numaralarýda listeleyebilirsiniz:
	
	   LIST 2-5,7-9
	      2'den 5'e ve 7'den 9'a kadar olan mesajlarý listeler.

MEMO_HELP_READ
	Kullanýmý: READ [kanaladý] {num | liste | LAST | NEW}
	
	Belirtilen mesajlarý okumanýzý saðlar. Eðer LAST belirtilmiþse
	en son gelen mesajýnýzý okursunuz. Eðer NEW belirtilmiþse tüm 
	yeni mesajlarýný okursunuz. Eðer bir numara belirtirseniz (num)
	o numaraya ait mesajý okursunuz. Aþaðýdaki örnekteki gibi belirli
	numaralarýda okuyabilirsiniz:
	
	   READ 2-5,7-9
	      2'den 5'e ve 7'den 9'a kadar olan mesajlarý gösterir.

MEMO_HELP_DEL
	Kullanýmý: DEL [kanaladý] {num | liste | LAST | ALL}
	
	Belirtilen mesajlarý siler. Aþaðýdaki örnekteki gibi birden
	fazla mesajýda silebilirsiniz.
	
	If LAST is given, the last memo will be deleted.
	Eðer ALL parametresi belirtilmiþse, tüm mesajlarýnýz silinir.
	
	Örnekler:
	
	   DEL 1
	      Ýlk gelen mesajýnýzý siler.
	
	   DEL 2-5,7-9
	      2'den 5'e ve 7'den 9'a kadar olan mesajlarý siler.

MEMO_HELP_SET
	Kullanýmý: SET özellik parametre
	
	Çeþitli mesaj ayarlarýný yapar. özellik aþaðýdakilerden 
	biri olabilir:
	
	    NOTIFY      Yeni mesajlarda ne zaman uyarýlacaðýnýzý
	                    belirler (sadece nickler içindir)
	    LIMIT       Alabileceðiniz maksimum mesaj sayýsýný
	                    belirler
	
	Belirli bir özellik hakkýnda daha fazla bilgi için, 
	%R%S HELP SET özellik yazýn.

MEMO_HELP_SET_NOTIFY
	Kullanýmý: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Yeni mesajlarda ne zaman uyarýlacaðýnýzý belirler:
	
	    ON      Yeni mesajlar için baðlandýðýnýzda, away den 
	               dönüþte ve size gönderildikleri anda 
	               uyarýlýrsýnýz.
	    LOGON   Yeni mesajlar için baðlandýðýnýzda ve away den
	               dönüþte uyarýlýrsýnýz.
	    NEW     Yeni mesajlar için sadece size gönderildikleri 
	               anda uyarýlýrsýnýz.
	    OFF     Yeni mesajlar için herhangi bir uyarý almazsýnýz.
	
	ON parametresi LOGON ve NEW 'in kombinasyonudur

MEMO_HELP_SET_LIMIT
	Kullanýmý: SET LIMIT [kanaladý] limit
	
	Sizin alabileceðiniz (yada belirtilen kanalýn) maksimum mesaj
	sayýsýný belirler. Eðer bunu 0 yaparsanýz kimse size mesaj 
	gönderemeyecektir. Ayrýca bu limiti %d den fazla yapamazsýnýz.

MEMO_HELP_INFO
	Kullanýmý: INFO [kanaladý]
	
	Sahip olduðunuz mesaj sayýsý, kaç tanesinin okunmadýðý, ve 
	alabileceðiniz maksimum mesaj sayýsý gibi bilgiler verir.
	Eðer kanaladý parametresi belirtilirse ayný bilgiyi o kanal
	için gösterir.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Kullanýmý: SET LIMIT [kullanýcý | kanaladý] {limit | NONE} [HARD]
	
	Bir kullanýcýcn yada kanalýn alabileceði maksimum mesaj
	sayýsýný belirler. Bunu 0 yapmak kullanýcýnýn mesaj almasýný 
	engeller; NONE yapmaksa kullanýcýnýn istediði kadar mesaj
	alýp saklamasýný saðlar. Eðer kanal veya nick belirtilmezse,
	kendi limitinizi ayarlarsýnýz.
	
	HARD parametresini eklemek kullanýcýnýn bu limiti deðiþtirmesini
	engeller. Bunu eklememekde, kullanýcýnýn limiti deðiþtirmesine izin
	verir. (Bir önceki limit ayarýnda HARD kullanýlmýþ olsa bile).
	
	SET LIMIT komutunun Kullanýmý Servis adminleriyle sýnýrlýdýr.
	Diðer kullanýcýlar limitlerini kaldýramaz, %d üzeri bir limit 
	belirleyemez, ve hard limitini ayarlayamaz. Sadece gerekli 
	ayrýcalýklarý varsa bir kanalýn mesaj limitini ve kendi mesaj
	limitlerini ayarlayabilirler.

MEMO_SERVADMIN_HELP_INFO
	Kullanýmý: INFO [nick | kanaladý]
	
	Parametre kullanýlmazsa, sahip olduðunuz mesajlarý, kaç 
	tanesinin okunmamýþ olduðunu, ve toplam kaç tane mesaj 
	alabileceðiniz hakkýnda bilgiler verir.
	
	Kanaladý parametresiyle, ayný bilgiyi belirtilen kanal
	için verir.
	
	Nick parametresiyle, ayný bilgiyi belirtilen nick için
	verir. Kullanýmý Servis adminleriyle sýnýrlýdýr.

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
	    GLOBAL      Tüm kullanýcýlara mesaj gönderir
OPER_HELP_CMD_STATS
	    STATS       Servislerin ve networkun durumunu gösterir
OPER_HELP_CMD_OPER
	    OPER        Servis operatör listesini düzenler
OPER_HELP_CMD_ADMIN
	    ADMIN       Servis adminleri listesini düzenler
OPER_HELP_CMD_STAFF
	    STAFF       Display Services staff and online status
OPER_HELP_CMD_MODE
	    MODE        Bir kanalýn modlarýný deðiþtirir
OPER_HELP_CMD_KICK
	    KICK        Bir kullanýcýyý kanaldan atar
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  Bir kanalýn modlarýný temizler
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  Ayni ip'deki tum kullanicilari kill'ler.
OPER_HELP_CMD_AKILL
	    AKILL       AKILL listesini düzenler
OPER_HELP_CMD_SGLINE
	    SGLINE      SGLINE listesini düzenler
OPER_HELP_CMD_SQLINE
	    SQLINE      SQLINE listesini düzenler
OPER_HELP_CMD_SZLINE
	    SZLINE      SZLINE listesini düzenler
OPER_HELP_CMD_CHANLIST
	    CHANLIST    Tüm kanal kayýtlarýný listeler
OPER_HELP_CMD_USERLIST
	    USERLIST    Tüm kullanýcý kayýtlarýný listeler
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   Baðlantý sýrasýnda kullanýcýnýn göreceði mesajý 
	                belirler
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  Baglanti sirasina kullaniciya gönderilen karisik
	                mesajlari belirler
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    Oper olan kullanýcýlarýn görüntüleyeceði mesajý 
	                belirler
OPER_HELP_CMD_SESSION
	    SESSION     Host session(oturum) larýn listesini gösterir
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   Session-limit exception listesini düzenler
OPER_HELP_CMD_NOOP
	    NOOP        Geçici olarak tüm O:line larý kaldýrýr
OPER_HELP_CMD_JUPE
	    JUPE        Bir server'ý "Kullanýlmaz" yapar
OPER_HELP_CMD_IGNORE
	    IGNORE      Modify the Services ignore list
OPER_HELP_CMD_SET
	    SET         Çeþitli global servis özelliklerini ayarlar
OPER_HELP_CMD_RELOAD
	    RELOAD      Servislerin conf dosyasýný yeniden yükler
OPER_HELP_CMD_UPDATE
	    UPDATE      Diskteki Servis veritabanýnýn hemen
	                güncellenmesini saðlar

OPER_HELP_CMD_RESTART
	    RESTART     Veritabanýný kaydeder ve servisleri yeniden 
	                baþlatýr

OPER_HELP_CMD_QUIT
	    QUIT        Servis programýný kapatýr
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    Servisler programýný kayýt ederek kapatýr
OPER_HELP_CMD_DEFCON
	    DEFCON      Manipulate the DefCon system
OPER_HELP_CMD_CHANKILL
	    CHANKILL    Kill all users on a specific channel
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
	Not: %S'e gönderilen tüm komutlarýn kaydý tutulur!

OPER_HELP_GLOBAL
	Kullanýmý: GLOBAL mesaj
	
	IRCoplarýn networkteki tüm kullanýcýlara mesaj yollamasýný
	saðlar. Mesajlar %s nickinden gönderilir.

OPER_HELP_STATS
	Kullanýmý: STATS [AKILL | ALL | RESET]
	
	Hiçbir parametre yokken, mevcut kullanýcý sayýsýný ve online
	olan IRCoplarý(Servisler hariç), Servisler baþlatýldýðýndan 
	beri baðlý olan en fazla kullanýcý sayýsýný, ve servislerin
	çalýþma süresini gösterir.
	
	AKILL parametresiyle, mevcut AKILL listesini ve o anki 
	varsayýlan zaman aþýmý süresini gösterir.
	
	ALL parametresi sadece Servis adminleri tarafýndan 
	kullanýlabilir, ve servislerin kullandýðý hafýza(memory) 
	hakkýnda bilgi verir. Büyük networklerde bu komutun kullanýlmasý
	servisleri kýsa bir süre dondurabilir, bu yüzden tekrar kullanmaya
	kalkmayýn!
	
	RESET parametresi önceki maksimum kullanýcý sayýsýný o anda
	networkte bulunan kullanýcý sayýsýyla deðiþtirir.
	
	UPTIME komutu STATS komutunun alternatifi olarak kullanýlabilir.

OPER_HELP_OPER
	Kullanýmý: OPER ADD nick
	           OPER DEL {nick | kayýt-num | liste}
	           OPER LIST [mask | liste]
	           OPER CLEAR
	
	Servis adminlerinin Servis operatörleri listesine belirtilen 
	nicki ekleyip çýkarmasýný saðlar. Nicki Servis operatörleri
	listesinde olan ve kendini %s ile tanýtmýþ olan kullanýcýlar
	Servis operatörleri komutlarýný kullanabilir.
	
	OPER ADD komutu belirtilen nicki servis operatörleri listesine
	ekler.
	
	OPER DEL komutu belirtilen nicki servis operatörleri listesinden
	siler. Eðer bir kayýt numarasý listesi verilmiþse, bu kayýtlar
	silinir. (Aþaðýdaki LIST ile ilgili örneði inceleyin.)
	
	OPER LIST komutu servis operatörleri listesini görüntüler.
	Eðer yýldýzlý(wildcard) bir mask belirtilirse, sadece o maskla
	uyuþan kayýtlar listelenir. Eðerbir kayýt numarasý listesi 
	belirtilirse, sadece o kayýtlar gösterilir; Örneðin:
	
	   OPER LIST 2-5,7-9
	      Servis operatörlerinden kaydý 2 den 5 e ve 7 den 9 a
	      kadar olanlar listelenir.
	
	OPER CLEAR komutu servis operatörleri listesindeki tüm kayýtlarý
	siler.
	
	Herhangi bir IRC operatörü OPER LIST komutunu kullanabilir.
	Diðer tüm OPER komutlarýný Servis adminleri kullanabilir.

OPER_HELP_ADMIN
	Kullanýmý: ADMIN ADD nick
	           ADMIN DEL {nick | kayýt-num | liste}
	           ADMIN LIST [mask | liste]
	           ADMIN CLEAR
	
	Servis root unun Servis adminleri listesine 
	belirtilen nicki ekleyip çýkarmasýný saðlar. Nicki Servis 
	adminleri listesinde olan ve kendini %s ile tanýtmýþ olan
	kullanýcý Servis adminlerini komutlarýný kullanabilir.
	
	ADMIN ADD komutu servis adminleri listesine belirtilen nicki
	ekler.
	
	ADMIN DEL komutu belirtilen nicki servis adminleri listesinden
	siler. Eðer kayýt numaralarý listesi verilmiþse, bu kayýtlar 
	silinir. (Aþaðýdaki LIST örneðini inceleyin.)
	
	ADMIN LIST komutu servis adminleri listesini görüntüler.
	Eðer yýldýzlý(wildcard) bir mask belirtilmiþse, sadece bu
	maskla uyuþan kayýtlar görüntülenir. Eðer bir kayýt numarasý 
	listesi belirtilmiþse, sadece o kayýtlar gösterilir; Örneðin:
	
	   ADMIN LIST 2-5,7-9
	      Servis adminleri kayýtlarýndan 2 den 5 e ve 7 den 9 a
	      kadar olanlar listelenir.
	      
	ADMIN CLEAR komutu tüm servis adminlerinin kayýtlarýný siler.

	Herhangi bir IRC operatörü ADMIN LIST komutunu kullanabilir.
	Diðer tüm ADMIN komutlarýný Servis rootlarý kullanabilir.

OPER_HELP_IGNORE
	Syntax: IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick | mask]
	
	Allows Services Admins to make Services ignore a nick or mask
	for a certain time or until the next restart. The default
	time format is seconds. You can specify it by using units.
	Valid units are: s for seconds, m for minutes, 
	h for hours and d for days. 
	Combinations of these units are not permitted. To make
	Services permanently (untill next restart) ignore the user,
	type 0 as time.
	When adding a mask, it should be in the format user@host
	or nick!user@host, everything else will be considered a nick.
	Wildcards are permitted.

OPER_HELP_MODE
	Kullanýmý: MODE kanaladý modlar
	
	Servis operatörlerinin herhangi bir kanalýn modlarýný
	ayarlamasýný saðlar. Parametreler standart /MODE komutununkiyle
	aynýdýr.
	
	Servis operatörlerinin Kullanýmýyla sýnýrlýdýr.

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
	Kullanýmý: CLEARMODES kanaladý [ALL]
	
	Bir kanaldaki tüm modlarý (i,k,l,m,n,p,s,t) ve banlarý
	temizler. Eðer ALL parametresi belirtilmiþse, tüm
	oplarý (+o) ve voicelarý (+v) temizler.
	
	Servis operatörlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_KICK
	Kullanýmý: KICK kanaladý kullanýcý sebep
	
	IRCoplarýn herhangi bir kullanýcýyý kanaldan atmalarýný
	saðlar. Parametreler standart /KICK komutununkiyle aynýdýr.
	Kick mesajýnda komutu kullanan IRCop un nickide belirtilir.
	Örneðin:
	
	*** SpamMan has been kicked off channel #my_channel by %S (MJJ (Flood))
	
	Servis operatörlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_SVSNICK
	Syntax: SVSNICK nick newnick
	
	Forcefully changes a user's nickname from nick to newnick.
	Limited to Super Admins.

OPER_HELP_AKILL
	Kullanýmý: AKILL ADD [+zamanaþýmý] mask sebep
	           AKILL DEL {mask | kayýt-no | liste}
	           AKILL LIST [mask | liste]
	           AKILL VIEW [mask | liste]
	           AKILL CLEAR
	
	Servis operatörlerinin AKILL listesini iþletmesini saðlar.
	Eðer AKILL listesineki mask la uyuþan bir kullanýcý
	baðlanmaya çalýþýrsa, Servisler o kullanýcýyý KILL ler ve,
	destekleyen server tiplerinde, tüm serverlarýn o maska K-line
	koymasýný saðlar.
	
	AKILL ADD belirtilen user@host maskýný AKILL listesine
	belirtilen sebeple(mutlaka belirtilmelidir) birlikte
	ekler. AKILL DEL belirtilen maský eðer listedeyse AKILL 
	listesinden çýkarýr. AKILL LIST mevcut tüm AKILL leri
	gösterir; eðer opsiyonel olarak mask belirtilmiþse, AKILL
	listesi belirtilen maskla uyuþanlarla sýnýrlandýrýlýr. 
	
	AKILL VIEW, AKILL LIST in daha açýklayýcý versiyonudur, 
	ve AKILL i ekleyeni, eklenme tarihini, ne zaman sona ereceðini,
	user@host maskýyla ve sebeple birlikte gösterir.
	
	Opsiyonel olarak, zaman aþýmý süresi AKILL ADD komutuyla
	birlikte belirtilebilir. Zaman aþýmý süresi user@host maskýndan
	önce gelir, ve bir tamsayý olarak þunlardan birinden önce gelir, 
	d (gün), h (saat), veya m (dakika). 1h30m gibi kombinasyonlar 
	kullanýlamaz. Eðer birim belirleyici belirtilmemiþse varsayýlan 
	olarak gün kullanýlýr (yani +30 un anlamý 30 gündür). Zaman 
	aþýmýna uðramayacak bir AKILL eklemek için +0 kullanýn. Eðer 
	eklenecek mask + ile baþlarsa zaman aþýmý süresi belirtilmelidir. 
	Mevcut varsayýlan AKILL zaman aþýmý süresi STATS AKILL komutuyla 
	bulunabilir.

	AKILL CLEAR AKILL listesindeki tüm kayýtlarý siler.
	
	sebep sadece IRC operatörleri tarafýndan kullanýlmak içindir,
	ve kullanýcýlarýn KILL mesajlarýnda gözükmez.
	
	Servis operatörlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_SGLINE
	Kullanýmý: SGLINE ADD [+zamanaþýmý] mask:sebep
	         SGLINE DEL {mask | kayýt-no | liste}
	           SGLINE LIST [mask | liste]
	           SGLINE VIEW [mask | liste]
	           SGLINE CLEAR
	
	Servis operatörlerinin SGLINE listesi oluþturmalarýný saðlar.
	Eðer SGLINE listesindeki bir kullanýcý gerçek ismi(realname)
	servera baðlanmaya çalýþan bir kullanýcýnýnkiyle uyuþuyorsa
	o kullanýcýnýn servera baðlanmasý engellenir.
	
	SGLINE ADD komutu belirtilen realname(gerçek isim) mask ýný
	SGLINE listesine bir sebeple(belirtilmesi zorunludur) 
	birlikte ekler.
	zamanaþýmý zaman belirleyici olarak tanýmlanýr ve þunlar birini
	takip etmelidir: d (gün), h (saat), veya m (dakika). 1h30m
	þeklindeki kombinasyonlar kullanýlamaz. Eðer zaman tanýmlayýcý öðe
	konulmazsa varsayýlan olarak gün kabul edilir (yani +30 un manasý
	30 gündür). Zaman aþýmýna uðramayacak bir SGLINE eklemek için +0
	kullanýn.  Eklenecek realname mask ý + ile baþlýyorsa, o zaman
	zaman aþýmý süresi varsayýlanla ayný olsa bile belirtilmelidir.
	Mevcut SGLINE varsayýlan zaman aþýmý süresi STATS AKILL komutuyla
	bulunabilir.
	Not: realname maský boþluk içerebileceðinden bunla sebep arasýnda iki
	nokta üstüste(:) kullanýlmalýdýr.
	
	SGLINE DEL komutu belirtilen maský(eðer listede mevcutsa) SGLINE 
	listesinden siler. eðer bir kayýt numaralarý listesi belirtilmiþse
	o kayýtlar silinir. (Aþaðýdaki LIST için verilmiþ örneði inceleyin.)
	
	SGLINE LIST komutu SGLINE listesini görüntüler. Eðer bir yýldýzlý
	mask verilmiþse, sadece o maskla uyuþan kayýtlarý içeren bir liste
	görüntülenir.  Eðer bir kayýt numaralarý listesi belirtilmiþse,
	sadece o kayýtlar görüntülenir; örneðin:
	
	   SGLINE LIST 2-5,7-9
	      2 den 5 e 7 den 9 a kadar olan SGLINE kayýtlarýný listeler.
	      
	SGLINE VIEW komutu SGLINE LIST komutunun daha açýklayýcý bilgi 
	veren þeklidir, ve SGLINE ý kimin eklediðini, eklendiði tarihi, ve
	ne zaman sona ereceðini realname ve sebeple birlikte gösterir.
	
	SGLINE CLEAR komutu SGLINE listesindeki tüm kayýtlarý siler.
	
	Servis operatörlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_SQLINE
	Kullanimi: SQLINE ADD [+zamanasimi] mask sebep
	           SQLINE DEL {mask | kayit-no | liste}
	           SQLINE LIST [mask | liste]
	           SQLINE VIEW [mask | liste]
	           SQLINE CLEAR
	
	Servis adminlerinin SQLINE listesini yönetmelerini saglar.
	Eger SQLINE listesindeki masklardan biriyle uyusan bir nick
	servera baglanmaya çalisirsa, servisler buna izin vermezler.
	
	eger maskin ilk karakteri # ise servisler onla uyusan 
	kanallarin kullanilmasina izin vermezler. (destekleyen IRCd
	yazilimlarinda geçerlidir).
	
	SQLINE ADD belirtilen maski belirtilen sebeple (mutlaka
	belirtilmelidir) birlikte SQLINE listesine ekler. zaman asimi
	ardinda d (gün), h (saat), veya m (dakika) olan bir sayi
	olmalidir..  1h30m seklindeki kombinasyonlar geçersizdir.
	eger son ekleme harfi belirtilmemisse, varsayilan olarak gün
	kullanilir. (yani +30 30 gün demektir). Zaman asimina ugramasini
	istemediginiz bir SQLINE eklemek için +0 kullanin.  
	Egegr eklenecek maskin basinda + varsa, zaman asimi süresi
	belirtilmelidir, aksi halde zaman asimi süresi varsayilanla ayni
	olacaktir. Mevcut SQLINE varsayilan zaman asimi süresi STATS AKILL
	komutuyla bulunabilir.
	
	SQLINE DEL komutu belirtilen maski SQLINE listesinden siler.
	Eger kayit numaralari listesi belirtilmisse, o kayitlari siler.
	(Asagidaki örnegi inceleyin.)
	
	SQLINE LIST komutu SQLINE listesini gösterir. Egegr bir mask
	belirtilmisse, maskla uyusan kayitlar gösterilir. Egegr kayit
	numaralari listesi belirtilmisse, o numaralara karsilik olan
	kayitlar gösterilir; örnegin:

	   SQLINE LIST 2-5,7-9
	      2 den 5 e ve 7 den 9 a kadar olan SQLINE kayitlari
	      gösterilir.
	      
	SQLINE VIEW komutu SQLINE LIST komutunun daha gelismisidir,
	ve SQLINE kaydini ekleyeni, eklendigi tarihi, süresinin ne zaman
	bitecegini, mask ve sebeple beraber gösterir.
	
	SQLINE CLEAR SQLINE listesindeki tüm kayitlari temizler.
	
	Servis operatörlerinin kullanimiyla sinirlidir.

OPER_HELP_SZLINE
	Kullanýmý: SZLINE ADD [+zamanaþýmý] mask sebep
	           SZLINE DEL {mask | kayýt-no | liste}
	           SZLINE LIST [mask | liste]
	           SZLINE VIEW [mask | liste]
	           SZLINE CLEAR
	
	Servis operatörlerinin SZLINE listesi oluþturmalarýný saðlar.
	Eðer SZLINE listesindeki bir IP numarasýyla uyuþan bir kiþi
	servera baðlanmaya çalýþýrsa buna izin verilmez. (ve bu IP
	numarasý PTR RR olsada olmasada gerçekleþtirilir).
	
	SZLINE ADD komutu belirtilen IP maskýný SZLINE listesine
	belirtilen bir sebeple(ki mutlaka belirtilmelidir) birlikte
	ekler. zamanaþýmý zaman belirleyici olarak tanýmlanýr ve 
	þunlardan birini takip etmelidir: d (gün), h (saat), veya
	m (dakika). 1h30m þeklindeki kombinasyonlar kullanýlamaz.
	Eðer zaman tanýmlayýcý öðe belirtilmemiþse, varsayýlan olarak
	gün kullanýlýr (yani +30 30 gün demektir).  Zaman aþýmýna 
	uðramayacak bir SZLINE eklemek için +0 kullanýn.  Eðer
	eklenecek realname maský + ile baþlýyorsa, zaman aþýmý süresi
	varsayýlanla ayný olsa bile belirtilmelidir. Mevcut SZLINE 
	varsayýlan zaman aþýmý süresi STATS AKILL komutuyla öðrenilebilir.
	
	SZLINE DEL komutu belirtilen maský eðer mevcutsa SZLINE 
	listesinden siler. Eðer kayýt numaralarý listesi belirtilmiþse,
	bu kayýtlar silinir. (LIST için verilmiþ aþaðýdaki örneði inceleyin.)
	
	SZLINE LIST komutu SZLINE listesini görüntüler. Eðer yýldýzlý bir
	mask belirtilmiþse, sadece o maskla uyuþan kayýtlar görüntülenir.
	Eðer kayýt numaralarý listesi belirtilmiþse, sadece o kayýtlar 
	gösterilir;  örneðin:
	
	   SZLINE LIST 2-5,7-9
	      2 den 5 e 7 den 9 a kadar olan kayýtlarý görüntüler.
	      
	SZLINE VIEW komutu SZLINE LIST komutunun daha geniþ bilgi veren 
	bir þeklidir, ve SZLINE ý kimin eklediðini, eklendiði tarihi, ve ne
	zaman sona ereceðini, IP maský ve sebeple birlikte görüntüler.
	
	SZLINE CLEAR komutu SZLINE listesindeki tüm kayýtlarý siler.
	
	Servis operatörlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_SET
	Kullanýmý: SET özellik ayar
	
	Çeþitli global servis özelliklerini ayarlar.  Özellikler
	aþaðýda tanýmlanmýþtýr:
	    READONLY   Read-only ve read-write modunu ayarlar
	    LOGCHAN    Report log messages to a channel
	    DEBUG      Debug modunu aktifleþtirir yada kapatýr
	    NOEXPIRE   No expire modunu aktifleþtirir yada kapatýr
	    SUPERADMIN Activate or deactivate super-admin mode
	    SQL   	 Activate or deactivate sql mode
	    IGNORE     Activate or deactivate ignore mode
	    LIST       List the options
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_SET_READONLY
	Kullanýmý: SET READONLY {ON | OFF}
	
	Read-only modunu açar yada kapatýr. Read-only modunda,
	normal kullanýcýlar herhangi bir servis verisini deðiþtiremezler,
	bunun içinde kanal ve nick access listeleri v.s. vardýr.
	Yeterli ayrýcalýklarý olan IRCoplar servislerin AKILL listesini
	deðiþtirebilir, nicklerin ve kanallarýn kaydýný silebilir yada 
	yasaklayabilir, fakat deðiþiklikler servisler kapatýlmadan yada
	tekrar baþlatýlmadan önce read-only modu kapatýlmazsa kaydedilmez.
	
	-readonly komut satýrý özelliðiyle aynýdýr.

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
	Kullanýmý: SET DEBUG {ON | OFF | num}
	
	Debug modunu açar yada kapatýr. Debug modunda, servislere 
	gönderilen ve alýnan tüm veriler diðer bir kaç debug 
	mesajlarýyla beraber bir kayýt dosyasýna yazýlýr. Eðer num 
	parametresi belirtilmiþse, debug modu num a baðlý debug 
	seviye ayarý ile aktifleþtirilir.
	
	-debug komut satýrý özelliðiyle aynýdýr.

OPER_HELP_SET_NOEXPIRE
	Kullanýmý: SET NOEXPIRE {ON | OFF}
	
	No expire modunu açar yada kapatýr. No expire modunda,
	nickler, kanallar, akiller, ve exceptionlar mod kapanana
	kadar zaman aþýmýna uðramazlar.
	
	-noexpire komut satýrý özelliðiyle aynýdýr.

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
	Kullanýmý: NOOP SET server
	           NOOP REVOKE server
	
	NOOP SET komutu belirtilen serverdaki tüm O:line larý
	siler ve tüm ircoplarý rehash komutunu kullanýp komutun
	etkisini yok etmemeleri için killer.
	
	NOOP REVOKE komutu belirtilen serverdaki tüm kaldýrýlmýþ
	O:line larý yeniden oluþturur.
	
	Not: server parametresinin doðruluðu servisler tarafýndan
	kontrol edilmez.
	
	Services adminlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_JUPE
	Kullanýmý: JUPE server [sebep]
	
	Servislere bir serverýn Jupiter yapýlmasýný söyler. -- Bu,
	servislere bir sahte server baðlayarak, gerçek serverýn o 
	isimden baðlanmasýný engeller. JUPE modu standart SQUIT
	kullanýlarak kaldýrýlabilir. Eðer sebep belirtilmiþse,
	bu server bilgi satýrýna yazýlýr; belirtilmezse server bilgi
	satýrýnda "Jupitered by <nick>" yazýsý yer alýr, ve serverý 
	jupiter edenin nicki yazýlýr.
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_RAW
	Kullanýmý: RAW yazý
	
	Servislerin baðlý olduðu server a direk bir yazý gönderir.
	Bu komutun kullaným alanlarý sýnýrlýdýr, ve yanlýþ kullanýlýrsa
	networkte çok büyük hasara yol açabilir. Tam olarak ne 
	yaptýðýnýzý bilmiyorsanýz BU KOMUTU KULLANMAYIN!
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_UPDATE
	Kullanýmý: UPDATE
	
	Komutu gönderir gönderirmez servislerin tüm veri tabaný 
	dosyalarýný güncellemelerini saðlar.
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_RELOAD
	Kullanýmý: RELOAD
	
	Servislerin conf dosyasýný yeniden yüklemesine neden olur.
	Unutmayýnki bazý direktifler hala servislerin yeniden 
	baþlatýlmasýný gerektirir. (Servislerin nickleri, session 
	limitinin aktifleþtirilmesi, etc.)
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_QUIT
	Kullanýmý: QUIT
	
	Servislerin birden kapanmasýna neden olur; veritabanlarý
	kaydedilmez. Bu komut veritabanlarýnýn hafýzadaki 
	kopyalarýna zarar gelmesinden korkuluyorsa ve 
	yedeklenmemiþlerse kullanýlmamalýdýr. Normal kapatmalar 
	için, SHUTDOWN komutunu kullanýn.
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_SHUTDOWN
	Kulanýmý: SHUTDOWN
	
	Servislerin tüm veritabanlarýný kaydederek kapanmasýný saðlar.
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_RESTART
	Kullanýmý: RESTART
	
	Servislerin tüm veritabanlarýný kaydederek yeniden 
	baþlamasýný saðlar.
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.	

OPER_HELP_CHANLIST
	Kullanýmý: CHANLIST [{model | nick} [SECRET]]
	
	Networkte o an kullanýlan kanallarý kayýtlý olsun olmasýn 
	listeler.
	
	Eðer model belirtilmiþse, onla uyuþan kanallarý listeler. 
	Eðer nickname belirtilmiþse, o kullanýcýnýn bulunduðu 
	kanallarý listeler. Eðer SECRET belirtilmiþse, sadece model
	le uyuþan ve +s veya +p modunda olan kanallar listelenir.
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_USERLIST
	Kullanýmý: USERLIST [{model | kanaladý} [INVISIBLE]]
	
	Networkte o an bulunan tüm kullanýcýlarý kayýtlý olsun olmasýn 
	listeler.
	
	Eðer model belirtilmiþse, onla uyuþan kullanýcýlarý listeler
	(nick!user@host formatýnda olmalýdýr). Eðer kanaladý 
	belirtilmiþse, sadece o kanalda bulunan kullanýcýlarý listeler.
	Eðer INVISIBLE belirtilmiþse, sadece +i modundaki kullanýcýlar 
	listelenir.
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

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
# BotServ help messages
#
###########################################################################


BOT_HELP_CMD_BOTLIST
	BOTLIST            Mevcut botlarý listeler
BOT_HELP_CMD_ASSIGN
	ASSIGN             Kanala bot sokar
BOT_HELP_CMD_SET
	SET                Bot özelliklerini ayarlar
BOT_HELP_CMD_KICK
	KICK               Atma özelliklerini ayarlar
BOT_HELP_CMD_BADWORDS
	BADWORDS           Küfür listesi oluþturur
BOT_HELP_CMD_ACT
	ACT                Makes the bot do the equivalent of a "/me" command
BOT_HELP_CMD_INFO
	INFO               Allows you to see BotServ information about a channel or a bot
BOT_HELP_CMD_SAY
	SAY                Makes the bot say the given text on the given channel
BOT_HELP_CMD_UNASSIGN
	UNASSIGN           Unassigns a bot from a channel
BOT_HELP_CMD_BOT
	BOT                Networkün bot listesini düzenler

BOT_HELP
	%S kanalýnýzda bir botunuzun olmasýný saðlar. Bu
	servis bot konfigürasyonundan anlamayanlar yada
	bota izin vermeyen networkler için geliþtirilmiþtir.
	Mevcut komutlar aþaðýda listelenmiþtir. Kullanmak için
	%R%S command yazýn. Belirli bir komut hakkýnda
	daha fazla bilgi için, %R%S HELP komut
	yazýn.
	
BOT_HELP_FOOTER
	Kanalda en az %d kullanýcý olduðu zaman bot kanala girer.

BOT_HELP_BOTLIST
	Kullanýmý: BOTLIST
	
	Networkte mevcut olan tüm botlarý listeler.

BOT_HELP_ASSIGN
	Kullanýmý: ASSIGN kanaladý botunnicki
	
	Belirtilen nickteki botu belirtilen kanala sokar. Bundan 
	sonra botun ayarlarýný ihtiyaçlarýnýza uygun þekilde 
	yapabilirsiniz.

BOT_HELP_UNASSIGN
	Kullanýmý: UNASSIGN kanaladý
	
	Belirtilen kanaldan botu çýkarýr. Bu komutu kullandýktan
	sonra bot bir daha kanala girmeyecektir. Fakat yapýlmýþ bot
	ayarlarý saklanýr, böylece istediðinz zaman tekrar ayar 
	yapmaya gerek olmadan kanala bot sokabilirsiniz.

BOT_HELP_INFO
	Kullanýmý: INFO {kanaladý | botunnicki}
	
	Bir kanal yada bot hakkýnda %S bilgisini gösterir. Eðer
	parametre kanalsa, aktif atma özellikleri hakkýnda bilgi
	alýrsýnýz. Eðer parametre nickse, bot hakkýnda yaratýlma 
	zamaný veya bulunduðu kanal sayýsý gibi bilgiler alýrsýnýz.

BOT_HELP_SET
	Kullanýmý: SET kanaladý özellik parametre
	
	Bot özelliklerini ayarlar. özellik þunlardan biri olabilir:
	
	    DONTKICKOPS      Botun atmasýna karþý oplarý korur
	    DONTKICKVOICES   Botun atmasýna karþý voice larý korur
	    GREET            Greet mesajýný aktifleþtirir
	    FANTASY          Fantazi komutlarýný aktifleþtirir
	    SYMBIOSIS        Gerçek bot gibi davranmasýný saðlar.
	
	Belirli bir komut hakkýnda daha fazla bilgi için 
	%R%S HELP SET özellik yazýn.
	
	Not: Bu komuta access eriþimi seviye(level) ayarýyla saðlanýr.

BOT_HELP_SET_DONTKICKOPS
	Kullanýmý: SET kanaladý DONTKICKOPS {ON|OFF}
	
	Kanalda op korumasýný açar yada kapatýr.
	Aktifleþtirildiði zaman, oplar bot tarafýnda NOKICK
	seviyeleri olmasalar bile atýlmazlar.

BOT_HELP_SET_DONTKICKVOICES
	Kullanýmý: SET kanaladý DONTKICKVOICES {ON|OFF}
	
	Kanalda voice korumasýný açar yada kapatýr.
	Aktifleþtirildiði zaman, voice lar bot tarafýndan
	NOKICK seviyeleri olmasalar bile atýlmazlar.

BOT_HELP_SET_FANTASY
	Kullanýmý: SET kanaladý FANTASY {ON|OFF}
	
	Kanalda fantazi modunu açar yada kapatýr.
	Aktifleþtirildiði zaman, kulanýcýlar kanalda
	!op, !deop, !voice, !devoice, !kick, !kb, 
	!unban, !seen gibi komutlarý kullanabilirler. 
	(nasýl kullanýldýklarýný öðrenmek için; onlarý 
	nickle ve nicksiz kullanýn vede bazýlarýndada sebep 
	belirtin).
	
	Unutmayýnki fantazi komutlarýný kullanmak
	isteyen kullanýcýlarýn FANTASIA ve komutun 
	gerektirdiði diðer seviyelere yeterli access
	seviyesi olmasý gereklidir. (örneðin, !op
	komutunu kullanmak için, kullanýcýnýn OPDEOP
	seviyesine yeterli access i olmalýdýr).

BOT_HELP_SET_GREET
	Kullanýmý: SET kanaladý GREET {ON|OFF}
	
	Kanalda karþýlama modunu açar yada kapatýr.
	Aktifleþtirildiði zaman, bot kanala giren 
	kullanýcýlara bir karþýlama mesajý gönderir,
	tabi eðer bu kiþilerin yeterli access leri varsa.

BOT_HELP_SET_SYMBIOSIS
	Kullanýmý: SET kanaladý SYMBIOSIS {ON|OFF}
	
	Kanalda simbiyoz modunu açar yada kapatýr.
	Aktifleþtirildiði zaman bot %s'ün yaptýðý herþeyi
	kendisi yapar. Modelar, kickler, ve giriþ mesajý gibi.

BOT_HELP_KICK
	Kullanýmý: KICK kanaladý özellik parametre
	
	Botun kick lerini ayarlar. özellik þunlardan biri olabilir:
	
	    BOLDS         Kalýn yazýda atar
	    BADWORDS      Küfürde atar
	    CAPS          Büyük yazýda atar
	    COLORS        Renkli yazýda atar
	    FLOOD         Flood da atar
	    REPEAT        Kendini tekrar edenleri atar
	    REVERSES      Ctrl+r nin kullanýlmasýnda atar
	    UNDERLINES    Altýçizgili yazýda atar
	
	Belirli bir özellik hakkýnda daha fazla bilgi için
	%R%S HELP KICK özellik yazýn.
	
	Not: Bu komuta eriþim seviye(level) ayarýyla yapýlýr.

BOT_HELP_KICK_BOLDS
	Kullanýmý: KICK kanaladý BOLDS {ON|OFF} [böas]
	
	Kalýn yazýda atmayý açar yada kapatýr. 
	Aktifleþtirildiðinde bu özellik bota kalýn yazý
	kullananlarý atmasýný söyler.
	
	böas banlamadan önce kiþinin kaç kez atýlacaðýný 
	belirler. Kaldýrmak için komutta böas ý belirtmeyin.

BOT_HELP_KICK_COLORS
	Kullanýmý: KICK kanaladý COLORS {ON|OFF} [böas]
	
	Renkli yazýda atmayý açar yada kapatýr. 
	Aktifleþtirildiðinde bu özellik bota renkli yazý 
	kullananlarý atmasýný söyler.
	
	böas banlamadan önce kiþinin kaç kez atýlacaðýný 
	belirler. Kaldýrmak için komutta böas ý belirtmeyin.

BOT_HELP_KICK_REVERSES
	Kullanýmý: KICK kanaladý REVERSES {ON|OFF} [böas]
	
	Ctrl+r nin kullanýlmasýnda atmayý açar yada kapatýr. 
	Aktifleþtirildiðinde bu özellik bota Ctrl+r yi 
	kullananlarý atmasýný söyler.
	
	böas banlamadan önce kiþinin kaç kez atýlacaðýný 
	belirler. Kaldýrmak için komutta böas ý belirtmeyin.

BOT_HELP_KICK_UNDERLINES
	Kullanýmý: KICK kanaladý UNDERLINES {ON|OFF} [böas]
	
	Altýçizgili yazýda atmayý açar yada kapatýr.
	Aktifleþtirildiðinde bu özellik bota altýçizgili
	yazanlarý atmasýný söyler.
	
	böas banlamadan önce kiþinin kaç kez atýlacaðýný 
	belirler. Kaldýrmak için komutta böas ý belirtmeyin.

BOT_HELP_KICK_CAPS
	Kullanýmý: KICK kanaladý CAPS {ON|OFF} [böas [min [yüzde]]]
	
	Büyük yazýda atmayý açar yada kapatýr.
	Aktifleþtirildiðinde bu özellik bota büyük yazý
	kullananlarý atmasýný söyler.
	
	Bot belirtilen en az(min) büyük harfin toplam yazýnýn
	yüzdesine%% baðlý olarak atar. (Belirtilmemiþse,
	varsayýlan olarak 10 karakter ve 25%% de atar).
	
	böas banlamadan önce kiþinin kaç kez atýlacaðýný 
	belirler. Kaldýrmak için komutta böas ý belirtmeyin.

BOT_HELP_KICK_FLOOD
	Kullanýmý: KICK kanaladý FLOOD {ON|OFF} [böas [sýra [sn]]]
	
	Flood da atmayý açar yada kapatýr. Aktifleþtirildiðinde
	bu özellik bota sn saniyede enaz sýra sýrada flood
	yapanlarý atmasýný söyler. (Belirtilmemiþse varsayýlan
	olarak 10 saniyede 6 sýra da atar).
	
	böas banlamadan önce kiþinin kaç kez atýlacaðýný 
	belirler. Kaldýrmak için komutta böas ý belirtmeyin.

BOT_HELP_KICK_REPEAT
	Kullanýmý: KICK #kanaladý REPEAT {ON|OFF} [böas [sayý]]
	
	Tekrar yazýlan ayný yazýda atmayý açar yada kapar.
	Aktifleþtirildiðinde bu özellik bota kendilerini
	sayý kez tekrar edenleri atmasýný söyler. (Eðer
	sayý belirtilmemiþse, varsayýlan olarak 3 tür).
	
	böas banlamadan önce kiþinin kaç kez atýlacaðýný 
	belirler. Kaldýrmak için komutta böas ý belirtmeyin.

BOT_HELP_KICK_BADWORDS
	Kullanýmý: KICK #channel BADWORDS {ON|OFF} [böas]
	
	Küfürde atmayý açar yada kapatýr. Aktifleþtirildiðinde
	bu özellik bota küfür edenleri atmasýný söyler.
	
	Bir kanal için küfür belirlemek için BADWORDS komutunu
	kullanabilirsiniz. Ayrýntýlar için  %R%S HELP BADWORDS
	yazýn.
	
	böas banlamadan önce kiþinin kaç kez atýlacaðýný 
	belirler. Kaldýrmak için komutta böas ý belirtmeyin.

BOT_HELP_BADWORDS
	Kullanýmý:  BADWORDS kanaladý ADD kelime [SINGLE | START | END]
	            BADWORDS kanaladý DEL {kelime | kayýt-no | liste}
	            BADWORDS kanaladý LIST [mask | liste]
	            BADWORDS kanaladý CLEAR
	
	Bir kanal için küfür listesi düzenler. Küfür listesi
	küfürde atma aktifleþtirildiði zaman hangi kelimelerde
	atýlacaðýný belirler. Daha fazla bilgi için 
	%R%S HELP KICK BADWORDS yazýn.
	
	BADWORDS ADD komutu belirtilen kelimeyi küfür listesine
	ekler. Eðer SINGLE parametresi belirtilmiþse, atma iþlemi
	kullanýcý o kelimenin tamamýný söylerse yapýlýr. Eðer START
	parametresi belirtilmiþse, kullanýcý o kelimeyle baþlayan 
	birþey söylediðinde atýlýr. Eðer END parametresi belirtilmiþse
	kullanýcý o kelimeyle biten birþey söylediðinde atýlýr.
	Eðer bir parametre kullanmazsanýz, atma iþlemi kullanýcý o 
	kelimeyi her kullandýðýnda yapýlýr.
	
	BADWORDS DEL komutu belirtilen kelimeyi küfür listesinden 
	siler. Eðer kayýt numarasý listesi verilirse, o kelimeler
	silinir. (LIST için aþaðýdaki örneði inceleyin) 
	
	BADWORDS LIST komutu küfür listesini görüntüler. Eðer
	bir küfürün bir kýsmý belirtilmiþse, o kýsma uyan küfürler
	listelenir. Eðer bir kayýt numarasý listesi belirtilmiþse,
	sadece o küfürler listelenir; örneðin:
	
	   BADWORDS #kanaladý LIST 2-5,7-9
	      2'den 5'e ve 7'den 9'a kadar olan küfürler 
	      listelenir.
	      
	BADWORDS CLEAR komutu tüm küfür listesini temizler.

BOT_HELP_SAY
	Kullanýmý: SAY kanaladý yazý
	
	Botun belirtilen yazýyý belirtilen kanalda söylemesini saðlar.

BOT_HELP_ACT
	Kullanýmý: ACT kanaladý yazý
	
	Botun belirtilen yazýyla belirtilen kanalda "/me" komutunun 
	yaptýðýný kendi üzerinde yapmasýný saðlar.

BOT_SERVADMIN_HELP_BOT
	Kullanýmý: BOT ADD nick user host gerçekismi
	        BOT CHANGE eskinick yeninick [user [host [gerçekismi]]]
	        BOT DEL nick
	
	Servis adminlerinin kullanýcýlarýn kanallarýnda kullanmasý 
	için botlarý yaratmasýný, deðiþtirmesini ve silmesini saðlar.
	
	BOT ADD belirtilen nickte, username, hostname ve gerçek 
	isimde bir bot ekler. Doðruluk sýnamasý yapýlmadýðýnda komutu
	kullanýrken çok dikkatli olun.
	BOT CHANGE botun nickini, username ini, hostname ini veya
	gerçek ismini silmeye gerek kalmadan deðiþtirmeyi saðlar.
	BOT DEL belirtilen botu bot listesinden siler.
	
	Not: Eðer botun nicki olarak, kayýtlý bir nick
	kullanýrsanýz o nickin kaydý silinir(drop). Ayrýca
	o an bir kuulanýcý bu nicki kullanýyorsa kill lenir.

BOT_SERVADMIN_HELP_SET
	
	Bu özellikleri servis adminlerini kullanabilir:
	
	    NOBOT            Bir kanala bot girmesini engeller.
	    PRIVATE          Botun IRC operatörleri haricinde kanallara
	                        sokulmasini engeller

	                        
BOT_SERVADMIN_HELP_SET_NOBOT
	Kullanýmý: SET kanaladý NOBOT {ON|OFF}
	
	Bu özellik bir kanalý bot sokulamaz hale getirir. Eðer
	kanala zaten bir bot sokulmuþsa otomatik olarak o bot
	kanaldan çýkarýlýr.
	
	Servis adminlerinin Kullanýmýyla sýnýrlýdýr.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Kullanýmý: SET botun-nicki PRIVATE {ON|OFF}
	
	Bu özellik IRC Operatörleri disindaki userlarin bu
	botlari kanallara sokmalarini engeller.
	
	Bu komutu Servis Adminleri kullanabilir.
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
	Syntax: DELALL <nick>.
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
	Syntax %R%s set <nick> <hostmask>.
HOST_SETALL_SYNTAX
	Syntax %R%s setall <nick> <hostmask>.
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
	Syntax: %R%s del <nick>.

###########################################################################
#
# HostServ Help messages
#
###########################################################################
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
	All vhost's in the group %s have been set to %s
HOST_IDENT_GROUP
	All vhost's in the group %s have been set to %s@%s

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
	n/a
HOST_ADMIN_HELP
	n/a

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

