# Turkish language file.
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
# Son güncelleme - ysfm - ysfm@irc.org.tr - irc.sohbet.net.tr 		  #
# Imla ve Ceviri hatalarýný ysfm@irc.org.tr adresine bildiriniz		  #
# Turkish - Türkçe                                                        #
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
	"/msg %s" þeklinde uzun mesaj desteklenmemektedir. "/msg %s@%s" veya "/%s" þeklinde deneyiniz.

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
	Raw ayarlarý kapalý bulunuyor. DisableRaw Özelliði servis ayarlarý dosyasýnda açýk görünüyor.

MORE_OBSCURE_PASSWORD
	Lütfen daha zor bir þifre deneyin.  Þifreler en az 5 karakter uzunlukta, kolayca tahmin edilemeyecek birþey olmalýdýr(ör: gerçek isminiz, nickiniz v.b. olmamalýdýr), ve boþlukla tab kullanýlmamalýdýr.
PASSWORD_TOO_LONG
	Þifreniz çok uzun. Lütfen daha kýsa bir þifre deneyiniz.

NICK_NOT_REGISTERED
	Nickiniz kayýtlý deðil.
NICK_NOT_REGISTERED_HELP
	Nickiniz kayýtlý deðil.  Nick kayýt hakkýnda bilgi almak için %R%s HELP yazýn.
NICK_X_IS_SERVICES
	%s Rumuzu servislerin bir parçasý olarak görünüyor(Servisler Tarafýndan kullanýlýyor).
NICK_X_NOT_REGISTERED
	%s kayýtlý bir nick deðil.
NICK_X_IN_USE
	%s rumuzu þu anda kullanýlmaktadýr.
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
	%s rumuzu yasaklanmýþ bir rumuzdur. Kullanýma kapatýlmýþtýr.
NICK_X_TRUNCATED
	%s rumuzu uzun %d karakter nickten kýsaltýlacak.
NICK_X_SUSPENDED
	%s rumuzu askýya alýnmýþtýr(Geçici olarak kullanýma yasaklanmýþtýr).
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
	%s listesi sonu.


###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
LIST_INCORRECT_RANGE
	Geçersiz giriþ deniyorsunuz. Geçerli arama þekli #Aranan deðil Aranan'dýr.
CS_LIST_INCORRECT_RANGE
	' # ' Karakteri ile baslayan bir giriþ yaptýnýz, kanal aramalarýnda
	kanal ismi baþýna #-karakteri koymanýza gerek yoktur. (anope giriþi #anope þekline karþýlýk gelmektedir).

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
HELP_LIMIT_SERV_OPER
	Bu komut Services Operators kullanýmý icindir.
HELP_LIMIT_SERV_ADMIN
	Bu komut Services Administrators kullanýmý icindir.
HELP_LIMIT_SERV_ROOT
	Bu komut Services Roots kullanýmý icindir.
HELP_LIMIT_IRC_OPER
	Bu komut IRC Operators kullanýmý icindir.
HELP_LIMIT_HOST_SETTER
	Bu komut Host Setters kullanýmý icindir.
HELP_LIMIT_HOST_REMOVER
	Bu komut Host Removers kullanýmý icindir.


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
	Eðer bir dakika içinde nickinizi deðiþtirmezseniz, Sistem size otomatik olarak yeni bir rumuz atayacaktýr.
FORCENICKCHANGE_IN_20_SECONDS
	Eðer 20 saniye içinde nickinizi deðiþtirmezseniz, Sistem size otomatik olarak yeni bir rumuz atayacaktýr.
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
	%s Rumuzu kayýtlý bulunuyor.
NICK_PASSWORD_IS
	Nick Þifreniz %s - Ýlerde kullanacaðýnýz için unutmayýnýz ve kimseye söylemeyiniz.

# GROUP responses
NICK_REG_DELAY
	Rumuzunuzu kayýt edebilmeniz icin baglantý surenizden %d saniye sonra kayýt iþlemini denemelisiniz.

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
	Rumuzunuzu zaten sisteme tanýtmýþ durumdasýnýz.

# UPDATE responses
NICK_UPDATE_SUCCESS
	Durum güncellendi (Cevrimdisi mesajlar, ozel adres(vhost), ayarlar, yetkiler).
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	Nickinizin çýkýþý yapýldý.
NICK_LOGOUT_X_SUCCEEDED
	%s nickinin çýkýþý yapýldý.

# DROP responses
NICK_LOGOUT_SERVICESADMIN
	Cýkýþ yapýlamýyor %s Servis görevlisi durumunda.

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
	%s Fonksiyonu bu sunucuda devrede degildir.

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
	Artýk %s INFO da servisler üzerindeki yetkiniz gorunecek.
NICK_SET_HIDE_STATUS_OFF
	Servisler uzerindeki yetkiniz %s bilgilerinde gözükmeyecek.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_MSG_ON
	Servisler bundan sonra size mesaj(Genel Mesaj) ile cevap verecek.
NICK_SET_MSG_OFF
	Servisler bundan sonra size notice(Uyarý mesajý) ile cevap verecek.

# SET AUTOOP responses
NICK_SET_AUTOOP_SYNTAX
	SET AUTOOP {ON | OFF}
NICK_SET_AUTOOP_ON
	Services bundan sonra otomatik yetkilendirme yapacak.
NICK_SET_AUTOOP_OFF
	Services bundan sonra otomatik yetkilendirme yapmayacak.


# SET NOEXPIRE responses
NICK_SASET_SYNTAX
	SASET rumuz ayar parametereler
NICK_SASET_DISABLED
	Üzgünüz, bu düzenleme komutu kapalý durumdadýr.
NICK_SASET_UNKNOWN_OPTION
	%s gecersiz bir düzenmele girdisidir.
NICK_SASET_BAD_NICK
	%s rumuzu kayýtlý bulunmuyor.
NICK_SASET_OPTION_DISABLED
	%s ayarlarý bu sunucu uzerinde kullanýlabilir durumda degildir.

# SASET DISPLAY responses
NICK_SASET_DISPLAY_INVALID
	%s grubu gecerli bir nick grubu degil!
NICK_SASET_DISPLAY_CHANGED
	%s Yeni nick grubu olarak ayarlandý.

# SASET PASSWORD responses
NICK_SASET_PASSWORD_FAILED
	Üzgünüz, %s rumuzunun sifresi degiþtirilemedi.
NICK_SASET_PASSWORD_CHANGED
	%s rumuzunun sifresi degistirildi degistirildi.
NICK_SASET_PASSWORD_CHANGED_TO
	%s rumuzunun sifresi %s olarak degiþtirildi.

# SASET LANGUAGE responses
NICK_SASET_URL_CHANGED
	%s rumuzunun silgisi %s olarak degistirildi.
NICK_SASET_URL_UNSET
	%s Rumuzunun bilgisi silindi(Bilgi belirtilmemiþ konuma getirildi).

# SASET EMAIL responses
NICK_SASET_EMAIL_CHANGED
	%s rumuzunun silgisi %s olarak degistirildi.
NICK_SASET_EMAIL_UNSET
	%s Rumuzunun bilgisi silindi(Bilgi belirtilmemiþ konuma getirildi)..
NICK_SASET_EMAIL_UNSET_IMPOSSIBLE
	Mail adresi bu sunucu için geçersiz konuma alýnabilecek bir bilgi degildir.
	
# SASET ICQ responses
NICK_SASET_ICQ_CHANGED
	%s rumuzunun silgisi %s olarak degistirildi.
NICK_SASET_ICQ_UNSET
	%s Rumuzunun bilgisi silindi(Bilgi belirtilmemiþ konuma getirildi).
NICK_SASET_ICQ_INVALID
	%s gecerli bir numara deðil.
	
# SASET GREET responses
NICK_SASET_GREET_CHANGED
	%s rumuzunun silgisi %s olarak degistirildi.
NICK_SASET_GREET_UNSET
	%s Rumuzunun bilgisi silindi(Bilgi belirtilmemiþ konuma getirildi).

# SASET PROTECT responses
NICK_SASET_KILL_SYNTAX
	SASET nickname KILL {ON | QUICK | OFF}
NICK_SASET_KILL_IMMED_SYNTAX
	SASET nickname KILL {ON | QUICK | IMMED | OFF}
NICK_SASET_KILL_ON
	Koruma durumu ACIK deðiþiklik yapýlan nick %s.
NICK_SASET_KILL_QUICK
	Koruma durumu ACIK deðiþiklik yapýlan nick %s, kýsa sureli koruma aktif.
NICK_SASET_KILL_IMMED
	Koruma durumu ACIK deðiþiklik yapýlan nick %s, bekleme suresi olmayan koruma aktif.
NICK_SASET_KILL_IMMED_DISABLED
	IMMED bu sunucu icin gecerli bir rumuz koruma durumu degildir.
NICK_SASET_KILL_OFF
	Koruma durumu KAPALI deðiþiklik yapýlan nick %s.

# SASET SECURE responses
NICK_SASET_SECURE_SYNTAX
	SASET nickname SECURE {ON | OFF}
NICK_SASET_SECURE_ON
	%s rumuzu icin koruma ozellidi AKTIF.
NICK_SASET_SECURE_OFF
	%s rumuzu icin koruma ozellidi PASIF.

# SASET PRIVATE responses
NICK_SASET_PRIVATE_SYNTAX
	SASET nickname PRIVATE {ON | OFF}
NICK_SASET_PRIVATE_ON
	%s Rumuzu icin Ozel mesaj cevaplama Aktif.
NICK_SASET_PRIVATE_OFF
	%s Rumuzu icin Ozel mesaj cevaplama Pasif.

# SASET HIDE responses
NICK_SASET_HIDE_SYNTAX
	SET nickname HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SASET_HIDE_EMAIL_ON
	%s Rumuzuna ait bilgi %s INFO'da gosterime kapatýldý.
NICK_SASET_HIDE_EMAIL_OFF
	%s Rumuzuna ait bilgi %s INFO'da gosterime acýldý.
NICK_SASET_HIDE_MASK_ON
	%s Rumuzuna ait bilgi %s INFO'da gosterime kapatýldý.
NICK_SASET_HIDE_MASK_OFF
	%s Rumuzuna ait bilgi %s INFO'da gosterime acýldý.
NICK_SASET_HIDE_QUIT_ON
	%s Rumuzuna ait bilgi %s INFO'da gosterime kapatýldý.
NICK_SASET_HIDE_QUIT_OFF
	%s Rumuzuna ait bilgi %s INFO'da gosterime acýldý.
NICK_SASET_HIDE_STATUS_ON
	%s Rumuzuna ait bilgi %s INFO'da gosterime kapatýldý.
NICK_SASET_HIDE_STATUS_OFF
	%s Rumuzuna ait bilgi %s INFO'da gosterime acýldý..
	
# SASET MSG responses
NICK_SASET_MSG_SYNTAX
	SASET nickname MSG {ON | OFF}
NICK_SASET_MSG_ON
	Services bundan sonra %s rumuzuna mesajlar ile cevap verecek.
NICK_SASET_MSG_OFF
	Services bundan sonra %s rumuzuna notices(uyarýlar) ile cevap verecek.

# SET NOEXPIRE responses
NICK_SASET_NOEXPIRE_SYNTAX
	SASET nickname NOEXPIRE {ON | OFF}
NICK_SASET_NOEXPIRE_ON
	%s rumuzu zaman aþýmýna kapatýldý.
NICK_SASET_NOEXPIRE_OFF
	%s rumuzu zaman aþýmýna açýldý.

# ACCESS responses
NICK_SASET_AUTOOP_SYNTAX
	SASET nickname AUTOOP {ON | OFF}
NICK_SASET_AUTOOP_ON
	Servisler %s rumuzuna bundan sonra otomatik olarak yetkilerini verecek.
NICK_SASET_AUTOOP_OFF
	Servisler %s rumuzuna bundan sonra otomatik olarak yetkilerini veremeyecek.

# SASET LANGUAGE responses
NICK_SASET_LANGUAGE_SYNTAX
	SASET nickname LANGUAGE numara
NICK_SASET_LANGUAGE_UNKNOWN
	Bilinmeyen dil numarasý %d.  Lütfen %R%s HELP SET LANGUAGE komutu ile diller ve kodlarý hakkýnda bilgi alýnýz.
NICK_SASET_LANGUAGE_CHANGED
	Language for %s changed to %s.


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
	Size ait yetki bulunamadý.
NICK_ACCESS_LIST_X_EMPTY
	%s rumuzuna ait yetki bulunamadý.

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
	Zaman Aþýmý: %s
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
	Bu rumuz ' %s ' sebebinden dolayý askýya alýnmýstýr kullanýlamaz
NICK_INFO_SUSPENDED_NO_REASON
	Bu rumuz  alýnmýstýr kullanýlamaz

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
	   %c%s (zaman aþýmý %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nick [þifre]
NICK_NO_RECOVER_SELF
	Kendizi recover edemezsiniz!
NICK_RECOVERED
	Nickiniz serbest býrakýldý.
	%R%s RELEASE %s komutunu kullanarak %s saniye icinde nickinizi kullanabilirsiniz.

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
	GETEMAIL kullanýcýadi@emailsunucusu 
NICK_GETEMAIL_EMAILS_ARE
	%s Rumuzu %s adresini kullanýyor.
NICK_GETEMAIL_NOT_USED
	%s icin bir bilgi bulunamadý.
	
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
	%s þu anda askýya alýndý.
NICK_SUSPEND_FAILED
	%s rumuzu icin iþlem yapýlamadý!

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND nickname
NICK_UNSUSPEND_SUCCEEDED
	%s kullanýma acýldý.
NICK_UNSUSPEND_FAILED
	%s rumuzu icin iþlem yapýlamadý!

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
	Bu rumuz icin zaten bir kayýt istegi bulunuyor, lütfen mail adresinizi kontrol ederek aktivasyon bilgilerini öðreniniz.
NICK_REG_RESENT
	Aktivasyon kodunuz %s adresine tekrar gönderildi.
NICK_REG_UNABLE
	Rumuz kayit edilemiyor, lütfen daha sonra tekrar deneyiniz.
NICK_IS_PREREG
	Bu rumuz e-mail adresinde bulunan aktivasyon iþleminin sonucunu beklemektedir.
NICK_ENTER_REG_CODE
	Aktivasyon kodu %s adresine yollandý, %R%s confirm <passcode> þeklinde iþlemi tamamlayabilirsiniz.
NICK_CONFIRM_NOT_FOUND
	Kayýt iþlemine baþlamak icin önce "%R%s register <password> <email>" þeklinde bir kayýt talebinde bulunmalýsýnýz.
NICK_CONFIRM_INVALID
	Yanlýþ aktivasyon kodu, mail adresinizi kontrol ederek tekrar deneyiniz.
NICK_REG_MAIL_SUBJECT
	Rumuz Kayýt islemi (%s)
NICK_REG_MAIL_HEAD
	Merhabalar,
NICK_REG_MAIL_LINE_1
	%s Rumuzunu kayýt etmek icin bir istekte bulundunuz.
NICK_REG_MAIL_LINE_2
	Irc Sunucusu üzerinde " %R%s confirm %s " komutunu kullanarak kayýt iþleminizi tamamlayabilirsiniz.
NICK_REG_MAIL_LINE_3
	Eðer bu postanýn neden gönderildiði hakkýnda bir bilginiz yok ise lütfen postayý silerek adresi engelleyiniz.
NICK_REG_MAIL_LINE_4
	LÜTFEN BU POSTAYA CEVAP ATMAYINIZ!
NICK_REG_MAIL_LINE_5
	%s Sunucusu Yöneticisi.
NICK_GETPASS_PASSCODE_IS
	%s rumuzu icin gerekli aktivasyon kodu %s.
NICK_FORCE_REG
	%s rumuzu onaylandý.

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
	%s geçerli bir kanal deðil.

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
	Geçersiz bir kanalý kaydetmek istiyorsunuz %s
CHAN_SYMBOL_REQUIRED
	Kanalý kayýt etmek için # sembolunu kanal isminin baþýna eklemeniz gereklidir.


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
	Tüm tanýmlý kullanýcýlar artýk %s kanalý tarafýndan tanýnmýyor (kanal sahibi haric).
CHAN_LOGOUT_FOUNDER_FAILED
	%s Kanalýndan yetki cýkýsý yapamazsýnýz (kanal sahibi sizsiniz).

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
	Kanal baþlýðý saklama %s kanalý için aktif duruma gecti.
CHAN_SET_KEEPTOPIC_OFF
	Kanal baþlýðý saklama %s kanalý için pasif duruma gecti.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET kanaladý TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Kanal baþlýðý kilidi %s kanalý için aktif duruma gecti.
CHAN_SET_TOPICLOCK_OFF
	Kanal baþlýðý kilidi %s kanalý için pasif duruma gecti.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET kanaladi PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Bariþ secenegi %s kanalý icin aktif.
CHAN_SET_PEACE_OFF
	Bariþ secenegi %s kanalý icin pasif.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET kanaladý PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Özel kanal konumu %s kanalý icin  aktif.
CHAN_SET_PRIVATE_OFF
	Özel kanal konumu %s kanalý icin  pasif.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET kanaladý SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Operator korumasý %s kanalý icin  aktif.
CHAN_SET_SECUREOPS_OFF
	Operator korumasý %s kanalý icin  pasif.

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET kanaladý SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Kanal Sahibi korumasý %s kanalý icin  aktif.
CHAN_SET_SECUREFOUNDER_OFF
	Kanal Sahibi korumasý %s kanalý icin  pasif.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET kanaladý RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Ayrýcalýklý giriþ %s kanalý icin  aktif.
CHAN_SET_RESTRICTED_OFF
	Ayrýcalýklý giriþ %s kanalý icin  pasif.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET kanal SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Yetki seviyesi korumasi %s kanalý icin  aktif.
CHAN_SET_SECURE_OFF
	Yetki seviyesi korumasi %s kanalý icin  pasif.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET kanaladý SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	%s Kanalýnda kullanýlan atma komutlarý icin imza gosterimi aktif.
CHAN_SET_SIGNKICK_LEVEL
	%s kanalý icin yetki kullanan kisi gosterimi aktif, fakat
	kanal icinde ayma komutu uygulanan kiþinin yetkisi göz önünde tutulur.
CHAN_SET_SIGNKICK_OFF
	%s Kanalýnda kullanýlan atma komutlarý icin imza gosterimi pasif.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET kanaladý OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice  %s kanalý icin ON.
CHAN_SET_OPNOTICE_OFF
	Op-notice  %s kanalý icin OFF.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET kanaladi XOP {ON | OFF}
CHAN_SET_XOP_ON
	xOP listesi %s kanalý icin ON.
CHAN_SET_XOP_OFF
	xOP listesi  %s kanalý icin OFF.

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
	xOP sistemi geçerli degil.

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
	%s nicki %s kanalý AOP listesine taþýndý.
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
	%s nicki %s kanalý HOP listesine taþýndý.
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
	%s nicki %s kanalý SOP listesine taþýndý.
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
	%s nicki %s kanalý VOP listesine taþýndý.
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
	Bu komutu kullanamazsýnýz. 
	AOP, SOP, HOP ve VOP komutlarýný deneyiniz.
	%R%s HELP command þeklinde daha ayrýntýlý bilgi elde edebilirsiniz.
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
	Durum HATASI
CHAN_STATUS_NOT_REGGED
	Durum HATASI %s kanal kayýtlý degil
CHAN_STATUS_FORBIDDEN
	Durum HATASI %s kanalý yasaklý
CHAN_STATUS_NOTONLINE
	Durum HATASI Nic %s sunucuda degil
CHAN_STATUS_INFO
	Durum %s %s %d

# INFO responses
CHAN_INFO_SYNTAX
	INFO kanaladý [ALL]
CHAN_INFO_HEADER
	%s kanalý için bilgi:
CHAN_INFO_FOUNDER
	        Kanal Sahibi: %s (%s)
CHAN_INFO_NO_FOUNDER
	        Kanal Sahibi: %s
CHAN_INFO_SUCCESSOR
	      Kanal vârisi: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	      Kanal vârisi: %s
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
	      Zaman Aþýmý: %s
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
	%s kanalýndaki tüm ayrýcalýklar kaldýrýldý.
CHAN_CLEARED_MODES
	%s kanalýndaki tüm modlar resetlendi.
CHAN_CLEARED_OPS
	Mod +o %s kanalýnda temizlendi.
CHAN_CLEARED_HOPS
	Mod +h  %s kanalýnda temizlendi.
CHAN_CLEARED_VOICES
	Mod +v %s kanalýnda temizlendi.
CHAN_CLEARED_USERS
	%s kanalýndaki tüm kullanýcýlar atýldý.
CHAN_CLEARED_INVITES
	%s kanalýdaki tüm davetler kaldýrýldý.

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
	%s kanalý sifreli degil.
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
	Bu kanal yasaklanmýþ durumda.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND channel [reason]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND channel reason
CHAN_SUSPEND_SUCCEEDED
	%s bu kanal kullanýma kapatýldý
CHAN_SUSPEND_FAILED
	%s kanalý için islem yapýlamadi!
CHAN_SUSPEND_REASON
	Kanal kullanýma kapatýlmýþ.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND channel
CHAN_UNSUSPEND_ERROR
	' # ' iþareti kanal isminde bulunmuyor.
CHAN_UNSUSPEND_SUCCEEDED
	%s kanalý kullanýma acildi.
CHAN_UNSUSPEND_FAILED
	%s kanalý kullanýma acilamadi!

# Misc responses
CHAN_EXCEPTED
	%s %s kanalýnda ayrýcalýklý konumda bulunuyor ve bu konum kaldýrýlmadan kiþiyi kanaldan atamazsýnýz.

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
	%d adet %s kanalýna ait mesaj bulunuyor.
MEMO_X_ONE_NOTICE
	%d adet %s kanalýna ait mesaj bulunuyor.
MEMO_NEW_X_MEMO_ARRIVED
	%s kanalýnda okunmamýþ bir mesaj bulunuyor.
	%R%s READ %s %d þeklinde mesajý okuyabilirsiniz.
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
	Mesaj tüm kayýtlý kullanýcýlara iletildi.
	
# STAFF responses
MEMO_STAFF_SYNTAX
	STAFF memo-text
	
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
	%s kanalýna ait tüm mesajlar silindi.

# SET responses
MEMO_SET_SYNTAX
	SET özellik parametre
MEMO_SET_DISABLED
	MemoServ'ün SET komutu geçici olarak devre dýþýdýr.
MEMO_SET_UNKNOWN_OPTION
	Varolmayan SET özelliði: %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
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
	Yeni mesajlar hakkýnda mail adresiniz aracýlýgý ile bilgilendirileceksiniz.
MEMO_SET_NOTIFY_NOMAIL
	Mail yolu ile bilgilendirme kapatýldý.
MEMO_SET_NOTIFY_INVALIDMAIL
	Rumuzunuza ait gecerli bir mail adresi bulunmuyor.

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
	Yeni CevrimDýþý Mesaj Bildirimi
MEMO_MAIL_TEXT1
	Nerhabalar sayýn %s ,
MEMO_MAIL_TEXT2
	Bu mail yeni gönderilen Cevrimdýsý mesajlarý bilgilendirmek amaci ile gonderilmis bir e-postadýr. Mesajý yollayan %s. Mesaj numaranýz %d.
MEMO_MAIL_TEXT3
	Mesaj icerigi:

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_RSEND_PLEASE_WAIT
	RSEND komutunu kullanmadan önce %d saniye bekleyiniz sonra tekrar deneyiniz.
MEMO_RSEND_DISABLED
	Uzgunuz RSEND bu sunucuda aktif degil.
MEMO_RSEND_SYNTAX
	RSEND {nick | channel} memo-text
MEMO_RSEND_NICK_MEMO_TEXT
	[auto-memo] Gondermis oldugunuz mesaj okundu.
MEMO_RSEND_CHAN_MEMO_TEXT
	[auto-memo] %s na gondermis oldugunuz mesaj okundu.
MEMO_RSEND_USER_NOTIFICATION
	%s rumuzuna göndermis oldugu mesajýn okudugunu bildiren bir bildirim
	yollanacaktýr.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_CHECK_SYNTAX
	CHECK nickname
MEMO_CHECK_NOT_READ
	%s rumuzuna son yollanan mesaj (yollayan %s) okunmamýþ.
MEMO_CHECK_READ
	%s rumuzuna son yollanan mesaj (yollayan %s) okunmus durumda.
MEMO_CHECK_NO_MEMO
	%s rumuzunun sizin tarafýnýzdan yollanmýs bir mesajý bulunmuyor.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_NO_RSEND_SELF
	Kendinize yolladýgýnýz mesajlarýn durumunu kontrol edemezsiniz.

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
	%s botu zaten %s kanalýnda bulunuyor.
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
	Kullanýcý kanal ayrýcalýklý kullanýcýlarý listesinde bulunuyor.

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
BOT_BAD_NICK
	Bot rumuzu geçerli karakterlerden oluþmalýdýr.
BOT_BAD_HOST
	Bot'un adresi geçerli karakterlerden oluþmalýdýr.
BOT_BAD_IDENT
	Bot ident'i geçerli karakterlerden oluþmalýdýr..
BOT_LONG_IDENT
	Bot Ident'i en fazla %d karakter olmalýdýr.
BOT_LONG_HOST
	Bot adresi en fazla %d karakter olmalýdýr.

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
OPER_STATS_HOSTSERV_MEM
	HostServ      : %6d records, %5d kB
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
	Baglý bulunulan sunucu: %s
OPER_STATS_UPLINK_CAPAB
	Baglý Aðýn ozellikleri: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Bulunan sunucular: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE kanaladý modlar

# CLEARMODES responses
OPER_UMODE_SYNTAX
	UMODE nick modes

# OLINE responses
OPER_UMODE_SUCCESS
	%s rumuzunun modlarý deðiþtirildi.
OPER_UMODE_CHANGED
	%s nickli kullanýcý sizin modlarýnýzý deðiþtirdi.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE nick flags
OPER_OLINE_SUCCESS
	%s yetkileri %s rumuzu icin eklendi.

# CLEARMODES responses
OPER_OLINE_IRCOP
	Su anda irc operatoru konumuna gectiniz.

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
	%s rumuzu %s olarak degistirildi.
	
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
	%s rumuzu servis adminleri listesine alýndý.

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
	%s rumuzu servis operatorleri listesine alýndý.

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
	SQL code kullanýlabilir.
OPER_SET_SQL_OFF
	SQL code kullanýlamaz.
OPER_SET_SQL_ERROR
	Sql özelliði sadece ON veya OFF olarak ayarlanabilir.
OPER_SET_SQL_ERROR_INIT
	Error during init of SQL, check your logs to correct.
OPER_SET_SQL_ERROR_DISABLED
	SQL ayarlarý bu sunucu icin duzenlenmiþ durumda degil. lütfen ayar dosyasýný
	(services.conf) duzenleyiniz.
OPER_SET_LIST_OPTION_ON
	%s acýk
OPER_SET_LIST_OPTION_OFF
	%s kapalý

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
	Gecerli bir sunucu adý giriniz
OPER_JUPE_INVALID_SERVER
	Baðlý oldugunuz sunucuda bu uygulamayý yapamazsýnýz.
	
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
	IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick | mask]
OPER_IGNORE_VALID_TIME
	Gecerli bir zaman giriniz.
OPER_IGNORE_TIME_DONE
	%s rumuzu %s saniyeligine servisler tarafýndan engellendi.
OPER_IGNORE_PERM_DONE
	%s rumuzunu engelledim. Artýk beni kullanamaz.
OPER_IGNORE_DEL_DONE
	%s rumuzu servis engellerinden kaldýrýldý.
OPER_IGNORE_LIST
	Servisler ignore listesi:
OPER_IGNORE_LIST_NOMATCH
	%s rumuzu sunucunun engelledigi kiþiler arasýnda bulunmuyor.
OPER_IGNORE_LIST_EMPTY
	Ignore listesi boþ.

# KILLCLONES responses
OPER_IGNORE_LIST_CLEARED
	Ignore listesi temizlendi.

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
	Su anda SuperAdminsiniz
OPER_SUPER_ADMIN_OFF
	SuperAdmin Kapatýldý
OPER_SUPER_ADMIN_SYNTAX
	Bu özellik ON ve OFF olarak kullanýlabilir.
OPER_SUPER_ADMIN_WALL_ON
	%s Super-Admin
OPER_SUPER_ADMIN_WALL_OFF
	%s Super-Admin durumunda degil

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_SUPER_ADMIN_ONLY
	Sadece SuperAdmin kullanýmýna acýktýr.

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_STAFF_LIST_HEADER
	Durum Seviye Rumuz

# DefCon Messages

OPER_STAFF_FORMAT
	 %c %s  %s
OPER_STAFF_AFORMAT
	 %c %s  %s [%s]

# DefCon Messages

OPER_DEFCON_SYNTAX
	DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	Bu servis su anda kullanýlamaz, daha sonra tekrar deneyiniz
OPER_DEFCON_NO_CONF
	DEFCON sistemi services.conf dosyasýndan acýlabilir
OPER_DEFCON_CHANGED
	Servislerin DEFCON girdigi artýk %d
OPER_DEFCON_WALL
	%s DEFCON seviyesi %d olarak degiþtirildi
DEFCON_GLOBAL
	Defcon Seviyesi Su anda: %d

# Modules strings

OPER_MODULE_LOADED
	Eklenti %s yuklendi
OPER_MODULE_UNLOADED
	Eklenti %s kaldýrýldý
OPER_MODULE_LOAD_FAIL
	Bilinmeyen eklenti %s
OPER_MODULE_REMOVE_FAIL
	Geçersiz eklenti %s
OPER_MODULE_NO_UNLOAD
	Bu eklenti kaldýrýlamýyor.
OPER_MODULE_LOAD_SYNTAX
	MODLOAD FileName
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD FileName
OPER_MODULE_LIST_HEADER
	Mevcut eklenti listesi:
OPER_MODULE_LIST
	Module: %s [%s] [%s]
OPER_MODULE_LIST_FOOTER
	%d eklenti yüklü bulunuyor.
OPER_MODULE_INFO_LIST
	Eklenti: %s Versiyon: %s Yapýmcý: %s Yuklenme tarihi: %s
OPER_MODULE_CMD_LIST
	Belirtilmis komut: %R%s %s
OPER_MODULE_MSG_LIST
	Belirtilmis IRCD baðlantýsý: %s
OPER_MODULE_NO_LIST
	Eklenti bulunmuyor
OPER_MODULE_NO_INFO
	%s eklentisi icin bir bilgi bulunmuyor
OPER_MODULE_INFO_SYNTAX
	MODINFO EklentiIsmi

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
MODULE_HELP_HEADER
	Modulu yuklemek icin icerigi takip ediniz:

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
	%s Zaten listede bulunuyor.
OPER_EXCEPTION_CHANGED
	%s - %d olarak güncellendi.

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
	Kullanýmý: STAFF

	Sunucu üzerinde yetkili bulunan nicklerin yetki seviyelerini
	ve su anda baðlý olup olmadýklarýný gösterir.

OPER_HELP_DEFCON
	Kullanýmý: DEFCON [1|2|3|4|5]

	Defcon sistemý sunucu icin onceden tanýmlanmýs ayarlardýr
	servislerin daha iyi calýsmalarý ve herhangi bir saldýrý anýnda
	servislerin mevcut durumunu korumak icin düzenlenmistir.

OPER_HELP_DEFCON_NO_NEW_CHANNELS
	* Yeni kanal kayýt edilemez
OPER_HELP_DEFCON_NO_NEW_NICKS
	* Yeni rumuz kayýt edilemez
OPER_HELP_DEFCON_NO_MLOCK_CHANGE
	* Mlock degiþtirilemez
OPER_HELP_DEFCON_FORCE_CHAN_MODES
	* (%s) tüm kanallar icin zorunlu mod olarak atandý
OPER_HELP_DEFCON_REDUCE_SESSION
	* Baðlantý limiti azaltýldý %d
OPER_HELP_DEFCON_NO_NEW_CLIENTS
	* Yeni baglantýlar kopartýr
OPER_HELP_DEFCON_OPER_ONLY
	* Operler haric servisleri diger kullanýcýlarý engeller
OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* Sessizleri engelle(operler haric)
OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* Yeni baðlantýlarý akill listesine ekle


###########################################################################
#
# News system messages
#
###########################################################################

OPER_HELP_DEFCON_NO_NEW_MEMOS
	* Yeni cevrimdisi mesaj gonderilemez
OPER_HELP_CHANKILL
	Kullanýmý: CHANKILL [+expiry] channel reason

	Belirtilmis kanaldaki herkezi akill listesine ekler.
	Kullanýmý sonucu ident@host seklinde tüm rumuzlarý akill listesine alýr,
	bu listeden yasak kaldýrmak icin normal akill listesi kontrol edilmeli ve ayný yöntem kullanýlmalýdýr. 

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
	LOGONNEWS {ADD|DEL|LIST} [yazý|num]
NEWS_LOGON_LIST_HEADER
	Baðlantý haberleri:
NEWS_LOGON_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_LOGON_LIST_NONE
	Hiç baðlantý haberi yok.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD yazý
NEWS_LOGON_ADD_FULL
	Haber listesi dolu!
NEWS_LOGON_ADDED
	Yeni baðlantý haberleri maddesi eklendi (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {num | ALL}
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
	RANDOMNEWS {ADD|DEL|LIST} [yazi|no]
NEWS_RANDOM_LIST_HEADER
	Karisik haberler:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_RANDOM_LIST_NONE
	Hiç karisik haber yok.
NEWS_RANDOM_ADD_SYNTAX
	RANDOMNEWS ADD yazi
NEWS_RANDOM_ADD_FULL
	Haber listesi dolu!
NEWS_RANDOM_ADDED
	Yeni karisik haber eklendi (#%d).
NEWS_RANDOM_DEL_SYNTAX
	RANDOMNEWS DEL {no | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	#%d numarali karisik haber bulunamadi!
NEWS_RANDOM_DELETED
	#%d numarali karisik haber silindi.
NEWS_RANDOM_DEL_NONE
	Silinecek karisik haber yok!
NEWS_RANDOM_DELETED_ALL
	Tüm karisik haberler silindi.

NEWS_HELP_LOGON
	Kullanýmý: LOGONNEWS ADD yazý
	        LOGONNEWS DEL {num | ALL}
	        LOGONNEWS LIST
	
	Giriþ haberleri ve mesajlarýný düzenlemek icin kullanýlýr. 
	Kullanýcý sunucuya baðlantýðý anda bu belirtilen mesajlar giris yapan kullanýcýya 
	iletilir.  (Bununla beraber, %s adetten fazla mesaj kullanýcýya
	gonderilmez.  Eðer cok fazla mesaj eklenmiþ bulunuyor ise
	bunlardan en yenileri kullanýcýya ulaþtýrýlýr.)
	NewsCount(Giriþ haberleri) Özellikleri services.conf'ta belirtilmiþtir ve bu dosyadan ayarlanabilir.
	
	LOGONNEWS may only be used by Services admins. 

NEWS_HELP_OPER
	Kullanýmý: OPERNEWS ADD text
	        OPERNEWS DEL {num | ALL}
	        OPERNEWS LIST
	
	Giriþ haberleri ve mesajlarýný düzenlemek icin kullanýlýr(Yönetici giriþleri). 
	Kullanýcý sunucu içerisinde yönetici konumuna gectiði anda bu belirtilen mesajlar giris yapan operatore 
	iletilir.  (Bununla beraber, %s adetten fazla mesaj yöneticiye
	gonderilmez.  Eðer cok fazla mesaj eklenmiþ bulunuyor ise
	bunlardan en yenileri kullanýcýya ulaþtýrýlýr.)
	NewsCount(Giriþ haberleri) Özellikleri services.conf'ta belirtilmiþtir ve bu dosyadan ayarlanabilir.
	
	OPERNEWS may only be used by Services admins. 

NEWS_HELP_RANDOM
	Kullanimi: RANDOMNEWS ADD yazi
	           RANDOMNEWS DEL {no | ALL}
	           RANDOMNEWS LIST
	
	Karisik haberleri düzenlemenizi saglar. Bir kullanici
	network e baglandigi zaman, karisik haberlerden bir tanesi
	seçilerek kullaniciya gönderilir.
	
	RANDOMNEWS may only be used by Services admins. 

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
	    CONFIRM    Nick kayýt aktivasyonu için kullanýlýr. Aktivasyon kodunu tanýtma komutudur.
NICK_HELP_CMD_RESEND
	    RESEND     Nick Servisleri aktivasyon kodunu tekrar gönderir

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
	    SASET      Baþka bir rumuzun ayarlarýný düzenlemede kullanýlýr
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
	    GHOST      "askýda kalmýþ" bir rumuzu sunucudan atar 
NICK_HELP_CMD_ALIST
	    ALIST      Yetkili olduðunuz kanallari listeler
NICK_HELP_CMD_GLIST
	    GLIST      Nick grubunuzdaki nickleri listeler
NICK_HELP_CMD_INFO
	    INFO       Belirlenen rumuza ait olan bilgileri gösterir
NICK_HELP_CMD_LIST
	    LIST       Kayýtlý olan nickleri seçilen parametrelere göre listeler
NICK_HELP_CMD_LOGOUT
	    LOGOUT     Sisteme tanýttýðýnýz(identify) rumuzun sistemden cýkýþýný saðlar.(Böylece yetkisiz konuma gecersiniz.)
NICK_HELP_CMD_STATUS
	    STATUS     Seçilen rumuzun durum ve seviyesini gösterir
NICK_HELP_CMD_UPDATE
	    UPDATE     Durumuzunu günceller, Örnekle. Cevrimdisi mesajlarýnýz, Vhost vs.
NICK_HELP_CMD_GETPASS
	    GETPASS    Bir nickin þifresini öðrenmek içindir
	               (Sadece þifreleme(encryption) devre dýþýysa)
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Girilen mail adresi ile o adresi kayýtta kullanan kisileri eþleþtirir 
NICK_HELP_CMD_FORBID
	    FORBID     Bir nickin kullanýlmasýný yasaklar
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Seçilen rumuzu askýya alýr.
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Geçici olarak kullanýma kapatýlmýþ rumuzu kullanýma açar.

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
	Kullanýmý: UPDATE

	Durumunuzu günceller, Örneðin; Yeni cevrimdisi mesajlarýnýzý,
	sahib oldugunuz kanal modlarýný (ModeonID aktif olmalýdýr), vhost'unuzu  ve
	kiþisel bilgilerinizi günceller (son görünme zamaný, vs.).
	
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
	    AUTOOP     Servisler sizi otomatik op yapar.    
	
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
	Kullanýmý: SET MSG {ON | OFF}
	
	Eðer baðlý bulunduðunuz sunucu bunu destekliyor ise servisler sizinle 
	MSG(Düz mesajlar) ile konuþurlar, bu ayar aktif durumda olmazsa sizinle 
	iletiþimde duyuru(notice) kullanýrýz.

NICK_HELP_SET_AUTOOP
	Kullanýmý: SET AUTOOP {ON | OFF}
	
	Servislerin otomatik op vermesini saðlar. Eðer aktif duruma getirilir ise 
	servisler kanala girdiginizde sizi otomatik operator konumuna getirirler.

NICK_HELP_SASET
	Kullanýmý: SASET nickname option bilgi.
	
	Secilen rumuzun ayarlarýný düzenler.  kullanýlabilecek option(ayarlar) :
	
	    DISPLAY    Nick Grubunu düzenler
	    PASSWORD   Secilen rumuzun þifresini degiþtirir
	    URL        Secilen rumuzun kiþisel sitesini degiþtirir
	    EMAIL      Secilen rumuzun e-posta adresini degiþtirir
	    ICQ        Secilen rumuzun icq numarasýný degiþtirir
	    GREET      Secilen rumuzun karþýlama mesajýný degiþtirir
	    KILL       Rumuzun güvenligini acar veya kapatýr
	    SECURE     Rumuzun korumasýný acar veya kapatýr
	    PRIVATE    Nick'i nick listesinden ayrýcalýklý konuma getirir
	                   %R%S LIST
	    HIDE       Rumuzun belirlenen bilgisini INFO'da gösterimden kaldýrýr
	    MSG        Servislerin geri cevap secenegini belirler
	    NOEXPIRE   Nick zaman aþým süresini kaldýrýr
	    LANGUAGE   Rumuzun servisleri kullanýrken alacagý cevap
	               ve yardimlarda kullanýlacak olan dil'i belirler
	
	Daha fazla bilgiye %R%S HELP SASET option 
	þeklinde parametrelerin yardýmlarý ile ulasabilirsiniz. Secilen
	nickname bilgilerini deðiþtir. 
	
	Services admins kullanýmý ile sýnýrlýdýr.

NICK_HELP_SASET_DISPLAY
	Kullanýmý: SASET nickname DISPLAY new-display
	
	Secilen rumuzun servisler icerisinde kullandýgý nick grubunu deðiþtirir 
	Yeni grup kullanýlan grup ile ayný olamaz.
	
	Services admins Kullanýmý ile sýnýrlýnýr.

NICK_HELP_SASET_PASSWORD
	Kullanýmý: SASET nickname PASSWORD new-password
	
	Nick sahibinin sisteme nickini tanýtýrken kullandýgý sifreyi degiþtirir.
	
	Services admins Kullanýmý ile sýnýrlýnýr.

NICK_HELP_SASET_URL
	Kullanýmý: SASET nickname URL url
	
	Kiþi bilgilerinde görülen Url'i degiþtirir.  Bu URL
	kiþinin tercihi ile belirledigi bir bilgidir
	ve INFO bilgilerinde gösterilebilir.
	
	Services admins Kullanýmý ile sýnýrlýnýr.

NICK_HELP_SASET_EMAIL
	Kullanýmý: SASET nickname EMAIL address
	
	Rumuzun e-posta olarak belirlediði adresi degiþtirir.
	
	Services admins Kullanýmý ile sýnýrlýnýr.
	
NICK_HELP_SASET_ICQ
	Kullanýmý: SASET nickname ICQ number
	
	Kiþi bilgilerinde görülen ICQ numarasýný degiþtirir.  Bu 
	ICQ numarasý kiþinin tercihi ile belirledigi bir bilgidir
	ve INFO bilgilerinde gösterilebilir.
	
	Services admins Kullanýmý ile sýnýrlýnýr.
	
NICK_HELP_SASET_GREET
	Kullanýmý: SASET nickname GREET message
	
	Secilen rumuza karsýlama mesajý belirler, 
	bu mesaj kiþi kanala girdiginde servisler tarafýndan 
	karþýlama mesajý olarak gönderilir.  
	GREET(karþýlama) kiþinin yetkili oldugu kanallarda gönderilir.
	
	Services admins Kullanýmý ile sýnýrlýnýr.

NICK_HELP_SASET_KILL
	Kullanýmý: SASET nickname KILL {ON | QUICK | IMMED | OFF}
	
	Nickin otomatik koruma özelliðini açar ve kapatýr.
	Koruma açýkken eðer bir baþkasý o rumuzu kullanmaya 
	kalkarsa, o kiþiye nicki deðiþtirmesi için bir dakika süre 
	verilir, eðer deðiþtirmezse %S tarafýndan otomatik olarak
	deðiþtirilir.
	
	Eðer QUICK parametresini seçerseniz, kullanýcýya nickini
	deðiþtirmesi için 60 saniye yerine sadece 20 saniye verilir.
	Eðer IMMED parametresini seçerseniz kullanýcýnýn nicki
	uyarýlmadan hemen deðiþtirilir; lütfen çok gerekmedikçe
	bu özelliði kullanmayýn. Hatta bu
	özellik devre dýþý býrakýlmýþ olabilirler.
	
	Services admins Kullanýmý ile sýnýrlýnýr.

NICK_HELP_SASET_SECURE
	Kullanýmý: SASET nickname SECURE {ON | OFF}
	
	%S'ün güvenlik özelliklerini açar ve kapatýr. SECURE 
	ayarý ile, nickin sahibi olarak tanýnmasi için önce þifreyle 
	kiþinin kendini tanýtmasi gerekir(adresinin nickin access
	listesinde olup olmamasýna baðlý olarak). Fakat, eðer 
	adresi nickin access listesindeyse KILL özelliði
	kullanýlmýþ olsa bile %S sizi otomatik olarak kill
	lemeyecektir..
	
	Services admins Kullanýmý ile sýnýrlýnýr.

NICK_HELP_SASET_PRIVATE
	Kullanýmý: SASET nickname PRIVATE {ON | OFF}
	
	%S'ün özel nick özelliðini açar kapatýr. PRIVATE ayarý
	ile, %S'ün LIST komutu kullanýldýðýnda nickin listede
	gözükmemesi saðlanýr.(Buna raðmen, nicki bilen biri INFO 
	komutunu kullanarak 
	nick hakkýnda bilgi alabilir.)
	
	Services admins Kullanýmý ile sýnýrlýnýr.

NICK_HELP_SASET_HIDE
	Kullanýmý: SASET nickname HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	Birisi rumuz hakkýnda bilgi almak için %S INFO komutunu 
	kullandýðýnda bazý bilgilerin burada gözükmemesini saðlar.
	E-mail adresini (EMAIL), son göründüðünü user@host 
	maskýný (USERMASK), ve son çýkýþ mesajýný (QUIT)
	görünmez þekle alabilirsiniz. Ýkinici parametre bilginin görünüp (OFF)
	görünmeyeceðini 
	(ON) belirtmenizi saðlar.
	
	Services admins Kullanýmý ile sýnýrlýnýr.
	
NICK_HELP_SASET_MSG
	Kullanýmý: SASET nickname MSG {ON | OFF}
	
	Servislerin kiþiye cevapta kullanacaðý yolu belirtir 
	Eðer MSG özelliði aktif ise, servisler mesaj kullanýr,
	deðilse uyarý kullanýr.
	
	Services admins Kullanýmý ile sýnýrlýnýr.

NICK_HELP_SASET_NOEXPIRE
	Kullanýmý: SASET nickname NOEXPIRE {ON | OFF}
	
	Secilen nicki zaman aþýmýna kapatýr.  Bu düzenleme 
	ON þekline getirilir ise rumuz zaman aþýmý yüzünden silinmez.
	
	Services admins Kullanýmý ile sýnýrlýnýr.

NICK_HELP_SASET_AUTOOP
	Kullanýmý: SASET nickname AUTOOP {ON | OFF}
	
	rumuzun otomatik yetki almasýný saðlar.
	Eðer ON þekline getirlir ise nick kanala giriþinde 
	otomatik olarak operator konumuna geçer.

NICK_HELP_SASET_LANGUAGE
	Kullanýmý: SASET nickname LANGUAGE number
	
	Servislerin rumuza yolladýgý mesajlardaki dil secenegini ayarlamanýza yarar
	(Örneðin kiþi türkçe seçmiþ ise, servisler türkçe metinleri kullanýr).
	Dilnumaralari hakkýnda bilgilere
	%R%S HELP SET LANGUAGE komutu ile ulasabilirsiniz.

NICK_HELP_RECOVER
	Kullanýmý: RECOVER nickname [password]
	
	Rumuzunu baþkasýnýn kullandýgýnda kullanabileceginiz bir komuttur
	%S otomatik olarak nickinizi kullanýmdan kaldýrýr
	Böylece rumuzunuza otomatik gecebilirsiniz.
	gecebilirsiniz.
	
	Bu komutu kullandýgýnýzda, %S sahte kullanýcýdan
	nickinizi alýr ve kiþiyi sunucuda tutar
	ve rumuzunuzu geri alma sansi verir.  
	Eðer girer kullanýcý tekrar baðlanýr ve o rumuzu almaya kalkarsa
	%s kullanýmýnda olan rumuzu alamaz
	bundan sonra rumuzunuzu isterseniz, kullanmak icin RELEASE komutunu uygulamanýz 
	gerekmektedir.
	Bu komut hakkýnda (%R%S HELP RELEASE) 
	.
	
	RECOVER komutunu kullandýktan sonra
	rumuzu serbest býrakmalý ve nickinize geçiþ yapmalýsýnýz.
	yetkilerinizi ancak bu sekilde kullanabilirsiniz
	ancak bu sekilde
	kullanabilirsiniz. RECOVER sonrasý rumuzu kimin kullandýgýný WHOIS komutu ile ogrenebilirsiniz

NICK_HELP_RELEASE
	Kullanýmý: RELEASE nickname [password]
	
	%S belirli sebebler tarafýndan kullanýlan rumuzlarý serbest býrakmak
	icin kullanýlýr. Zaman korumalý rumuzlar veya RECOVER
	komutu ile %s kullanýmýna gecen rumuzlar icin kullanýlýr;
	boylece rumuzu kullanýlabilir duruma getirirsiniz.
	
	RELEASE komutu ancak nickserv tarafýndan kullanýlan nickler icin iþe yarar
	/WHOIS komutu ile rumuzu kullanan adrese ulasabilirsiniz.
	RELEASE kullanýmý sonrasý nick kullanýma acýlýr
	bundan sonra u rumuza geçis yapýlmalý ve 
	ve nick sifresi girilmelidir.

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
	Kullanýmý: ALIST [level]
	
	Yetkili oldugunuz tüm kanallarý ve seviyelerini listeler.
	Listelemede xOP veya access seneceklerine göre tarama yapabilirsiniz. 
	Bir secenek belirtilmez ise tüm yetkili bulundugunuz kanallari listeler.

	Örnekler:
	    ALIST Founder
	        Sahibi bulundugunuz kanallari
	    listeler.

	    ALIST AOP
	        AOP yetkisine sahip oldugunuz kanallarý listeler 
	    ve daha buyuk yetkileride gösterir.

	    ALIST 10
	        10 seviyesinde yetkiye sahip oldugunuz kanallarý listeler 
	    ve daha buyuk seviyelerinizide kapsar.
	
	Kanallarda zaman aþýmý kapalý ise yaný NOEXPIRE özelliði acýk ise 
	bunlarý iþaretle belirtir.
	
NICK_HELP_GLIST
	Kullanýmý: GLIST
	
	Grubunuzda bulunan tüm rumuzlarý listeler.

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
	Kullanýmý: CONFIRM aktivasyon-kodu
	
	Bu nick kayýt sýrasýndaki 2. adýmdýr.
	Rumuzunuzu kayýt edebilmek icin daha önceden
	%S (veya aktivasyon servisi) tarafýndan 
	e-posta adresinize yollanan aktivasyon kodunuzu confirm komutu ile sisteme göndermelisiniz.
	Eðer kodu bilmiyorsanýz %R%S HELP RESEND ile tekrar edinme bilgilerine ulasabilirsiniz. Daha önceden
	bir kayýt isteginiz yok ise: %R%S HELP REGISTER þeklinde kayýt iþlemi hakkýnda yardým alabilirsiniz.

NICK_HELP_RESEND
	Kullanýmý: RESEND
	
	Bu komut ulaþmamýþ veya doðru olmayan aktivasyon kodunu
	mail adresinize tekrar göndermek icin kullanýlýr.

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
	Kullanýmý: ALIST [nickname] [level]
	
	Secilen rumuzun yetkiye sahip oldugu kanllarý listeler.
	Belirli parametreler ile daha dar bir alanda listeme yapýlabilir 
	Bu taramalarda rumuz için  
	belirlenen seviye ve bunun üstündeki yetkiler gösterilir.

	Sadece Servis adminleri tarafýndan kullanýlýr.

NICK_SERVADMIN_HELP_GLIST
	Kullanýmý: GLIST [nickname]
	
	Bir parametre eklenmeden de kullanýlýr. Secilen rumuzun nick grubunu ve o gruptaký
	rumuzlarý gösterir .
	
	Bir parametre ile kullanýlýr ise, grupta o parametre ile
	eþleþen rumuzlar gösterilir.
	Sadece Servis adminleri tarafýndan kullanýlýr.

NICK_SERVADMIN_HELP_GETPASS
	Kullanýmý: GETPASS nick
	
	Belirtilen nickin þifresini söyler. Unutmayýnki bu komutun
	her kullanýlýþýnda , kullanýlan nick ve komutu kullanan
	kiþi kaydedilerek WALLOPS/GLOBOPS olarak gönderilir.
	
	Sadece Servis adminleri tarafýndan kullanýlýr.
	
	(Bu komut þifreleme(encryption) aktif olduðu zaman devre dýþýdýr.)

NICK_SERVADMIN_HELP_GETEMAIL
	Kullanýmý: GETEMAIL user@emailhost

	Secilen mail ile eþleþen rumuzu gösterir. Not
	Baþka rumuzlarin mail adresleri ile kullanmayýnýz.
	Eþleþmeleri daha sonra paylaþmayýnýz.
	Kiþi bilgilerini kullanmayýnýz.

	Services admins Kullanýmý ile sýnýrlýnýr.

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
	Kullanýmý: SUSPEND nickname sebeb / UNSUSPEND nickname

	SUSPENDs / UNSUSPENDs rumuzu kullanýmýný gecici olarak kapatýr.
	
	Sadece Servis adminleri tarafýndan kullanýlýr.

###########################################################################
#
# ChanServ help messages
#
###########################################################################

NICK_SERVADMIN_HELP_UNSUSPEND
	Kullanýmý: UNSUSPEND nickname

	UNSUSPEND Rumuzu eski bilgileri ile kullanýma acar.

	Sadece Servis adminleri tarafýndan kullanýlýr.

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
	    SUSPEND    Kanalý bilgilerini silmeden gecici olarak
	               kullanýma kapatýr
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Askýya alýnmýþ kanalý kullanýma acçr
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
	    BAN        Belirtilen nicki kanalda yasaklar
CHAN_HELP_CMD_CLEAR
	    CLEAR      Kanal ayarlarýný sýfýrlar
CHAN_HELP_CMD_DEVOICE
	    DEVOICE    Secilen rumuzdan Voice yetkisini geri alýr
CHAN_HELP_CMD_GETKEY
	    GETKEY     Secilen kanalýn anahtar þifresini alýr
CHAN_HELP_CMD_INFO
	    INFO       Kanal bilgilerini gösterir
CHAN_HELP_CMD_INVITE
	    INVITE     Chanserv sizi kanala davet eder
CHAN_HELP_CMD_KICK
	    KICK       Rumuzu kanaldan atar
CHAN_HELP_CMD_LIST
	    LIST       Girilen parametreye göre listeme yapar
CHAN_HELP_CMD_LOGOUT
	    LOGOUT     Kanaldan yetki cýkýsý yapar
CHAN_HELP_CMD_OP
	    OP         Secilen rumuzu operator yapar
CHAN_HELP_CMD_TOPIC
	    TOPIC      Kanal baslýðýný ayarlar
CHAN_HELP_CMD_UNBAN
	    UNBAN      Kanaldaki yasaklamalarý kaldýrýr
CHAN_HELP_CMD_VOICE
	    VOICE      Secilen rumuza voice verir
CHAN_HELP_CMD_VOP
	    VOP        Vop (Konuskan insanlar) listesini düzenler
CHAN_HELP_CMD_DEHALFOP
	    DEHALFOP   Secilen rumuzdan Yardýmcý operator yetkisini alýr 
CHAN_HELP_CMD_DEOWNER
	    DEOWNER    Kanal sahibi konumundan ayýrmayý saðlar
CHAN_HELP_CMD_DEPROTECT
	    DEPROTECT  Kanal yönetici konumundan ayýrmayý saðlar
CHAN_HELP_CMD_HALFOP
	    HALFOP     Secilen rumuzu yardýmcý operator yapar
CHAN_HELP_CMD_HOP
	    HOP        HOP listesini duzenler
CHAN_HELP_CMD_OWNER
	    OWNER      Kanal sahibi konumuna alýr
CHAN_HELP_CMD_PROTECT
	    PROTECT    Kanal Yöneticisi konumuna alýr
CHAN_HELP_CMD_ADMIN
	    ADMIN      Kanal Admini konumuna alir
CHAN_HELP_CMD_DEADMIN
	    DEADMIN    Kanal Admini konumundan geri alir
CHAN_HELP_CMD_DEOP
	    DEOP       Kanal yöneticisi konumundan geri alýr

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
	
	Enables or disables the restricted access option for a
	channel.  When restricted access is set, users not on
	the access list will instead be kicked and banned from the channel.

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
	     INVITES  Clears all invites on the channel.
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
	Kullanýmý: SUSPEND channel [reason]
	
	Kanalýn kullanýmýný yasaklar.
	Kanalý tekrar açmak icin UNSUSPEND kullanýldýgýnda
	kanal bilgi ve ayarlarýný tekrar kullanýma acar.
	
	Bir cok sunucuda ihtiyac duyulan bir uygulamadýr.
	
	Services admins Kullanýmý ile sýnýrlýnýr.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Kullanýmý: UNSUSPEND channel
	
	Kanalý kullanýma açar. Tüm bilgi ve ayarlarý
	yasaklanmadan önceki hali ile kullanýma sunar.
	
	Services admins Kullanýmý ile sýnýrlýnýr.

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
	    INFO   Mesajlar Hakkýnda bilgi almanýzý saglar
MEMO_HELP_CMD_RSEND
	    RSEND  Taahhutlu mesaj yollamanýzý saðlar:)
MEMO_HELP_CMD_CHECK
	    CHECK  Kiþinin son mesajýný okuyup okumadýgýný kontrol eder
MEMO_HELP_CMD_SENDALL
	    SENDALL  Tüm kayýtlý kullanýcýlara mesaj yollamanizi saglar
MEMO_HELP_CMD_STAFF
	    STAFF      Tüm görevlilere mesaj yollamanizi saglar

MEMO_HELP_HEADER
	%S kullanýcýlarýn birbirlerine veya kanallara(*) online 
	olmasalar bile mesaj göndermelerini saðlar. Mesaj gönderebilmek
	için gönderenin nicki, gönderilenin nicki veya kanalýn kayýtlý
	olmasý gereklidir.
	
	%S'ün mevcut komutlarý:
	

MEMO_HELP_ADMIN
	Kullanýlmýyor.
MEMO_HELP_OPER
	Kullanýlmýyor.
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
	Kullanýmý: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
	
	Yeni mesajlarda ne zaman uyarýlacaðýnýzý belirler:
	
	    ON      Yeni mesajlar için baðlandýðýnýzda, away den 
	               dönüþte ve size gönderildikleri anda 
	               uyarýlýrsýnýz.
	    LOGON   Yeni mesajlar için baðlandýðýnýzda ve away den
	               dönüþte uyarýlýrsýnýz.
	    NEW     Yeni mesajlar için sadece size gönderildikleri 
	               anda uyarýlýrsýnýz.
	    MAIL    You will be notified of memos by email aswell as
	                any other settings you have.
	    NOMAIL  You will not be notified of memos by email.
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
	Kullanýmý: STAFF memo-text

	Tüm servis görevlilerine  memaj yollamanýzý saglar.
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
	Kullanýmý: SENDALL mesaj

	Tüm kayýtlý kullanýcýlara mesaj yollamanýzý saglar.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_RSEND
	Kullanýmý: RSEND {nick | channel} memo-text
	
	Bir nick veya kanal mesaj yollayabilirsiniz
	memo-text.  Bir rumuza yolladýgýnýz zaman 
	Kiþi yeni mesaj aldýgýna dair uyarýlýr. Yollanan hedef
	kayýtlý bir rumuz veya kanal olmalýdýr.
	Mesaj okundugu zaman yollayan kiþiye, okuyan kiþilerin okuduguna dair
	bir uyari bilgisi iletilir.
	Bir baska deyiþ ile iadeli taahhutlu mektup gibidir.:)

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_CHECK
	Kullanýmý: CHECK nick
	
	Yollanan rumuzun son mesajý okudugunu veya okumadýgýný gosterir
	Not: Sadece rumuzlarda kullanýlýr, kanal mesajlarýnda geçerli degildir.

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
	    STAFF       Sunucu görevlilerini durumlarýna göre listeler
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
	    IGNORE      Servis engel listesini düzenler
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
	    DEFCON      DEFCON sistemini elle düzenlemenizi saðlar
OPER_HELP_CMD_CHANKILL
	    CHANKILL    Secilen kanaldaki tüm kullanýcýlarý sunucudan atar
OPER_HELP_CMD_OLINE
	    OLINE       Eklenen rumuza oper yetkisi verir
OPER_HELP_CMD_UMODE
	    UMODE       Kullanýcýnýn modlarýný degiþtirir
OPER_HELP_CMD_SVSNICK
	    SVSNICK     Kiþi nickini zorla deðiþtirir
OPER_HELP_CMD_MODLOAD
	    MODLOAD     Eklenti yükler
OPER_HELP_CMD_MODUNLOAD
	    MODUNLOAD   Eklenti kaldýrýr
OPER_HELP_CMD_MODINFO
	    MODINFO     Eklentinin bilgilerini verir
OPER_HELP_CMD_MODLIST
	    MODLIST     Eklentileri listeler

OPER_HELP
	%S komutlar:

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
	
	Servis Root Servis operatörleri listesine belirtilen 
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
	Kullanýmý: IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick | mask]
	
	Servis adminler tarafýndan kullanýlýr. 
	Belirli bir süre veya servisler yeniden baþlatýlana kadar kiþinin servisler tarafýndan
	engellenmesini saðlar. Varsayýlan zaman degeri saniyedir.
	Eðer farklý zaman bicimleri istenir ise: saniye icin s,dakika icin m, 
	saat icin h ve gün icin d kullanýlabilir. 
	Birleþik kombinasyonlar desteklenmez.
	Kiþiyi servislerden tamamen engellemek icin, 0 girdisi belirtilmelidir.
	Eger mask kullanýlacak ise, user@host
	veya nick!user@host,.
	kullanýlmalýdýr.
	
	Engelleme IRC Operatorleri tarafýndan kullanýlamaz.

OPER_HELP_MODE
	Kullanýmý: MODE kanaladý modlar
	
	Servis operatörlerinin herhangi bir kanalýn modlarýný
	ayarlamasýný saðlar. Parametreler standart /MODE komutununkiyle
	aynýdýr.
	
	Servis operatörlerinin Kullanýmýyla sýnýrlýdýr.

OPER_HELP_UMODE
	Kullanýmý: UMODE user modes
	
	Super Admins kullanýmýna acýk olmakla beraber.
	standart /MODE komutu gerine kullanýlan komuttur.
	Secilen kiþinin modlarýný degiþtirmek icin kullanýlýr.

OPER_HELP_OLINE
	Kullanýmý: OLINE user flags
	
	Super Admins kullanýmýna acýktýr kiþiye Oper yetkisi verir.
	Seçilen parametreler "+" veya "-" þeklinde belirtilmelidir. 
	tüm modlarý snomasklarý kaldýrmak icin  "-" kullanýlabilir.

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
	Kullanýmý: SVSNICK nick newnick
	
	Kiþinin rumuzunu degiþtirmek icin kullanýlýr.
	Super Admins kullanýmý icindir.

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
	Kullanýmý: SET LOGCHAN {ON | OFF}

	Bu ayar servis kayýtlarnýnýn bir kanala aktarýlmasýnýn baslatýlmasý
	icin kullanýlýr. Kayýtlar kayýt dosyasýna iþlenmesi istenilmis iþlemlerdir. LogChannel 
	Servis ayar dosyasi servies.conf'ta tanýmlanýr. Bir tanýmlama 
	Bulunmuyor ise bu özellik aktif edilemez.
	
	Bu ozellik -logchan seklýndede kullanýlabilir.

	Not: Bu Özellik kanal iyi korunmaz ise cok büyük bir güvenlik açýgý ortaya cýkartýr, 
	kanalý yeterince iyi koruyamýyorsaniz aktif etmeyiniz. Unutmayýn neredeyse tüm iþlemler bu kanala yansitilacaktir.

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
	Kullanýmý: SET SUPERADMIN {ON | OFF}

	Bu özellik ekstra yetkilendirme anlamýna gelir
	Süper admin olan kisi herþeyin "founder" (sahibi) konumuna gecer...

	Bu önerilen bir özellik degildir, Eðer yinede ihtiyaç duyarsaniz
	isteginizi gerceklestirdikten sonra uzun süre baðlý kalmadan  cýkýþ yapmanizi öneririz.

OPER_HELP_SET_SQL
	Kullanýmý: SET SQL {ON | OFF}

	Bu ayar Anope ile Sql sunucunuzun birbirine baglanmasýný saglar,
	Bu özellik Sql sunucunun servisler açýkken düþmesi sonucu
	tekrar baðlantý kurulmasý icin kullanýlýr.

OPER_HELP_SET_IGNORE
	Kullanýmý: SET IGNORE {ON | OFF}

	Anope ignore sisteminin aktif veya pasif konuma alýnmasýný 
	saðlar.

OPER_HELP_SET_LIST
	Kullanýmý: SET LIST

	Secilen %S parametresi ile ilgili yardim icin kullanýlýr

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
	Kullanýmý: MODLOAD FileName

	Eklentileri yüklenemek icin eklenti ismi ile beraber
	kullanýlýr.

	Services Roots kullanýmý icindir.

OPER_HELP_MODUNLOAD
	Kullanýmý: MODUNLOAD FileName

	Yüklü olan bir eklentiyi kaldýrmak icin eklenti adý belirtilerek
	kullanýlýr.

	Services Roots kullanýmý icindir.

OPER_HELP_MODINFO
	Kullanýmý: MODINFO FileName

	Eklenti hakkýnda bilgi almak icin kullanýlýr

	Services Roots kullanýmý icindir.

OPER_HELP_MODLIST
	Kullanýmý: MODLIST [Core|3rd|protocol|encryption|supported|qatested]

	Eklentileri listelemek icin kullanýlabilir.

	Services Roots kullanýmý icindir.


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
	ACT                Bota "/me" komutu þeklinde mesaj göndermek icin kullanýlýr
BOT_HELP_CMD_INFO
	INFO               Bir kanal veya bot hakkýnda bilgi almak icin kullanýlýr
BOT_HELP_CMD_SAY
	SAY                Botun yazýlan yazýyý kanala yollamasýný saglar
BOT_HELP_CMD_UNASSIGN
	UNASSIGN           botu kanaldan cýkartýr
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
	vhost listesi boþ.
HOST_ENTRY
	#%d Nick:%s, vhost:%s (%s - %s)
HOST_IDENT_ENTRY
	#%d Nick:%s, vhost:%s@%s (%s - %s)
HOST_SET
	%s icin vhost %s olarak ayarlandý.
HOST_IDENT_SET
	%s icin ident %s@%s olarak ayarlandý.
HOST_SETALL
	%s icin vhost %s olarak ayarlandý.
HOST_DELALL
	%s icin vhost kaldýrýldý.
HOST_DELALL_SYNTAX
	DELALL <nick>.
HOST_IDENT_SETALL
	%s icin ident %s@%s olarak ayarlandý.
HOST_SET_ERROR
	Hata! bir vhost sadece A-Z, a-z, 0-9, '.' ve '-' karakterlerini icermelidir.
HOST_SET_IDENT_ERROR
	vhost ident gecerli karakterlerden oluþmalýdýr
HOST_SET_TOOLONG
	HATA! Vhost cok uzun, %d karakterden kýsa bir host olusturunuz.
HOST_SET_IDENTTOOLONG
	HATA! kimlik cok uzun, %d karakterden kýsa bir kimlik olusturunuz..
HOST_NOREG
	%s nickserv veribankasinda bulunmuyor.
HOST_SET_SYNTAX
	SET <nick> <hostmask>.
HOST_SETALL_SYNTAX
	SETALL <nick> <hostmask>.
HOST_DENIED
	Yetkiniz yeterli degil.
HOST_NOT_ASSIGNED
	Vhost talebi icin irc operatorleriniz ile iletisime geciniz.
HOST_ACTIVATED
	%s vhostunuz aktif.
HOST_IDENT_ACTIVATED
	%s@%s vhost ve identiniz aktif.
HOST_ID
	Önce servislerimize kendinizi tanýtmalýsýnýz.
HOST_NOT_REGED
	Vhost istegi icin öncelikle kayýtlý bir rumuza sahip olmalýsýnýz
HOST_DEL
	%s vhostu silindi.
HOST_DEL_SYNTAX
	DEL <nick>.

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_OFF
	Vhost silindi ve sifrelemiþ hostunuz kullanýlýyor.
HOST_OFF_UNREAL
	Vhost silindi fakat þifreleme yapýlamadi lütfen tekrar baðlantý kurunuz veya /mode %s +%s komutunu deneyiniz
HOST_NO_VIDENT
	IRCD sisteminiz vIdent'i desteklemiyor. Eðer bu doðru deðilse sorunu bize iletiniz

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_GROUP
	%s grubu icin vhost %s
HOST_IDENT_GROUP
	%s grubu icin vident %s@%s

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_LIST_FOOTER
	Tüm kayýtlar gösteriliyor (Veri: %d)
HOST_LIST_RANGE_FOOTER
	gösterilen kayýtlar %d - %d
HOST_LIST_KEY_FOOTER
	Eþleþen veriler verilen %s (Veri: %d)

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_HELP_CMD_ON
	    ON          Vhostunuzu aktif eder
HOST_HELP_CMD_OFF
	    OFF         Vhostunuzu deaktif eder
HOST_HELP_CMD_GROUP
	    GROUP       Grubunuza vhost atar
HOST_HELP_CMD_SET
	    SET         Baska bir kullanýnýn vhostunu düzenlemenize yarar
HOST_HELP_CMD_SETALL
	    SETALL      Bir gruptaki tüm nicklere o vhost'u atar
HOST_HELP_CMD_DEL
	    DEL         Kullanýcýnýn vhostunu siler
HOST_HELP_CMD_DELALL
	    DELALL      Gruptaki tüm vhostlarý siler
HOST_HELP_CMD_LIST
	    LIST        Vhostlar hakkýnda bilgi verir.
HOST_OPER_HELP
	n/a
HOST_ADMIN_HELP
	n/a

HOST_HELP
	%S Komutlar:

HOST_HELP_ON
	Kullanýmý: ON

	Belirlenmis vhostu aktif duruma getirir.
	Bu komut kullanýldýgýnda /WHOIS komutunda bu adresiniz gorunuz
	gerçek IP adresinizi sadece siz gorursunuz.

HOST_HELP_SET
	Kullanýmý: SET <nick> <hostmask>.

	Secilen rumuza vhost atamanýzý saðlar
	Eðer IRCD sisteminiz Idents'i destekliyor ise,
	SET <nick> <ident>@<hostmask> komutu ile 
	vhost atayabilirsiniz.

	Services operatorleri kullanýmýna acýktýr.

HOST_HELP_DELALL
	Kullanýmý: DELALL <nick>.

	Secilen rumuzun grubundaki tüm nicklerin vhostlarýno
	silmek icin kullanýlýr.

	Host Düzenleyici Kullanýmýna acýktýr.


HOST_HELP_SETALL
	Kullanýmý: SETALL <nick> <hostmask>.

	Secilen rumuza ait grubtaki nicklere vhost atamanýzý saðlar
	Eðer IRCD sisteminiz Idents'i destekliyor ise,
	SETALL <nick> <ident>@<hostmask> komutu ile 
	vhost atayabilirsiniz.

	* NOT, Bu komut daha sonradan gruba katýlan nicklere
	vhost atamaz.

	Services operatorleri kullanýmýna acýktýr.

HOST_HELP_OFF
	Kullanýmý: OFF

	Vhost'u pasif konuma getirir.
	Bu komutu kullandýktan sonra herhangi birisi size /whois
	cektiginde sizin gercek IP adresinizi görür.

HOST_HELP_DEL
	Kullanýmý: DEL <nick>

	Veritabanýndan rumuza ait olan vhostu kaldýrmak icin
	kullanýlýr.

	Services operatorleri kullanýmýna acýktýr.

HOST_HELP_LIST
	Kullanýmý: LIST [<key>|<#X-Y>]

	Bu komut vhostlarý irc operatorlarine listelemed kullanýlýr
	Eðer bir anahtar secilir ise ona eþlecen ve hostlar gosterilir
	Örneklemek gerekir ise - Rob* - þeklinde bir girdide
	"Rob" ile baslayan bilgiler gosterilir

	Eðer #X-Y þeklinde bir girdi verilir ise, sadece X
	ve Y aralýgý gösterilir, örnek. #1-3 kullanýlýr ise sadece ilk 3
	nick/vhost gosterilir.

	En fazla gosterilecek bilgi sayýsý NSListMax belirtilmiþtir.
	Cok fazla eþleþme olmasý durumunda liste 1 defada gosterilemeyebilir.

	Services operatorleri kullanýmýna acýktýr.

HOST_HELP_GROUP
	Kullanýmý: GROUP
	
	Bu komut belirtilmis grup icin vhost atar
	Bazý grublardaki herkezin ayný vhostu almasýný
	saðlar.

OPER_SVSNICK_UNSUPPORTED
	ÜZGÜNÜZ, SVSNICK Bu sunucuda desteklenmiyor.

OPER_SQLINE_UNSUPPORTED
	ÜZGÜNÜZ, SQLINE Bu sunucuda desteklenmiyor.

OPER_SVSO_UNSUPPORTED
	ÜZGÜNÜZ, OLINE Bu sunucuda desteklenmiyor.

OPER_UMODE_UNSUPPORTED
	ÜZGÜNÜZ, UMODE Bu sunucuda desteklenmiyor.

OPER_SUPER_ADMIN_NOT_ENABLED
	SuperAdmin services.conf'ta aktif durumda degil

