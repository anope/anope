# Russian language file for Anope 1.5.10.
# 
# (C) 2003 Anope Team
# Contact us at info@anope.org
#
# Please read COPYING and README for furhter details.
#
# Based on the original code of Epona by Lara.
# Based on the original code of Services by Andy Church.
#
# Translated by Oleg Nikolaev aka Isot <isot@complife.ru>
# - Special thanks to Mishgan and Leeho for testing
#
# Translated: 7.12.2003
# Charaster set is cp1251
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
# For example, "Espaсol (Spanish)" or "Franзais (French)".

LANG_NAME
	Русский (Russian)

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
	Вск
	Пн
	Вт
	Ср
	Чт
	Пт
	Сб
# %A
STRFTIME_DAYS_LONG
	Воскресенье
	Понедельник
	Вторник
	Среда
	Четверг
	Пятница
	Суббота
# %b
STRFTIME_MONTHS_SHORT
	Янв
	Фев
	Мар
	Апр
	Май
	Июн
	Июл
	Авг
	Сен
	Окт
	Ноя
	Дек
# %B
STRFTIME_MONTHS_LONG
	Январь
	Февраль
	Март
	Апрель
	Май
	Июнь
	Июль
	Август
	Сентябрь
	Октябрь
	Ноябрь
	Декабрь


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	, 


# Various error messages.
USER_RECORD_NOT_FOUND
	Внутренняя ошибка - невозможно выполнить запрос.
UNKNOWN_COMMAND
	Неизвестная команда %s.
UNKNOWN_COMMAND_HELP
	Неизвестная команда %s. "/msg %s HELP" чтобы получить помощь по командам.
SYNTAX_ERROR
	Синтаксис: %s
MORE_INFO
	/msg %s HELP %s для более подробной информации.
NO_HELP_AVAILABLE
	Нет помощи по %s.
OBSOLETE_COMMAND
	Данная команда устарела; используйте вместо нее %s.

BAD_USERHOST_MASK
	Маска должна быть в формате user@host.
BAD_EXPIRY_TIME
	Неверное время истекания.
USERHOST_MASK_TOO_WIDE
	Границы %s слишком широки; Пожалуйста уточните маску.

SERVICE_OFFLINE
	%s сейчас в оффлайне.
READ_ONLY_MODE
	Примечание: сейчас сервисы работают в режиме только-чтение; изменения НЕ будут сохранены!
PASSWORD_INCORRECT
	Неверный пароль.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Доступ запрещен.
PERMISSION_DENIED
	Недостаточно прав для доступа.
RAW_DISABLED
	Опция RAW отключена. Если Вы желаете использовать ее, сконфигурируйте директиву DisableRaw в файле конфигурации сервисов.

MORE_OBSCURE_PASSWORD
	Пожалуйста попробуйте снова с более сложным паролем.  Пароль должен иметь длину не менее 5 символов, должен состоять только из латинских букв, не должен быть легко отгадываемым (т.е. Вашим именем или Вашим ником), и не должен содержать пробелов и табуляций.
PASSWORD_TRUNCATED
	Сообщение:  Ваш пароль был усечен до %d символов.

NICK_NOT_REGISTERED
	Ваш ник не зарегистрирован.
NICK_NOT_REGISTERED_HELP
	Ваш ник не зарегистрирован.  Напишите /msg %s HELP для получения информации о том, как зарегистрировать ник.
NICK_X_IS_SERVICES
	Nick %s is part of this Network's Services.
NICK_X_NOT_REGISTERED
	Ник %s не зарегистрирован.
NICK_X_IN_USE
	Ник %s уже используется.
NICK_X_NOT_IN_USE
	Ник %s в данный момент не используется.
NICK_X_NOT_ON_CHAN
	%s не присутствует в данный момент на канале %s.
NICK_X_FORBIDDEN
	Ник %s не может быть зарегистрирован или использован.
NICK_X_FORBIDDEN_OPER
	Ник %s был запрещен к использованию %s:
	%s
NICK_X_ILLEGAL
	Ник %s является недопустимым и не может быть использован.
NICK_X_TRUNCATED
	Ник %s был усечен до %d символов.
CHAN_X_NOT_REGISTERED
	Канал %s не зарегистрирован.
CHAN_X_NOT_IN_USE
	Канал %s не существует.
CHAN_X_FORBIDDEN
	Канал %s не может быть зарегистрирован или использован.
CHAN_X_FORBIDDEN_OPER
	Канал %s запрещен к использованию %s:
	%s
CHAN_X_SUSPENDED
	      Приостановлено: [%s] %s
NICK_IDENTIFY_REQUIRED
	Для выполнения данной команды Вы должны идентифицироваться. Попробуйте снова после того, 
	как напишете команду /msg %s IDENTIFY password.
CHAN_IDENTIFY_REQUIRED
	Для выполнения данной команды Вы должны идентифицироваться. Попробуйте снова после того,
	как напишете команду /msg %s IDENTIFY %s password.
	
MAIL_DISABLED
	Сервисы не сконфигурированы для отправки почты.
MAIL_INVALID
	Адрес электронной почты %s неверен.
MAIL_X_INVALID
	%s не является правильным адресом email.
MAIL_LATER
	Не удается отправить email; попробуйте, пожалуйста, пожже.
MAIL_DELAYED
	Подождите пожалуйста %d секунд и повторите.
	
NO_REASON
	Нет причины
UNKNOWN
	<неизвестно>

# Duration system
DURATION_DAY
	1 день
DURATION_DAYS
	%d дней
DURATION_HOUR
	1 час
DURATION_HOURS
	%d часов
DURATION_MINUTE
	1 минута
DURATION_MINUTES
	%d минут
DURATION_SECOND
	1 секунда
DURATION_SECONDS
	%d секунд
	
# Human readable expiration
NO_EXPIRE
	никогда не истечет
EXPIRES_SOON
	истекает при ближайшем обновлении базы данных
EXPIRES_M
	истечет через %d минут
EXPIRES_1M
	истечет через %d минут
EXPIRES_HM
	истечет через %d часов, %d минут
EXPIRES_H1M
	истечет через %d часов, %d минут
EXPIRES_1HM
	истечет через %d часов, %d минут
EXPIRES_1H1M
	истечет через %d часов %d минут
EXPIRES_D
	истечет через %d дней
EXPIRES_1D
	истечет через %d дней

# Generic Footer message
END_OF_ANY_LIST
	Конец списка %s.

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	Этот ник зарегистрирован и используется. Выберите, пожалуйста, другой ник.
	(Если это Ваш ник, напишите команду /msg %s IDENTIFY password.)
NICK_IS_SECURE
	Этот ник зарегистрирован и защищен.  Если это ваш ник, напишите
	команду /msg %s IDENTIFY password.  В ином случае,
	выберите, пожалуйста, другой ник.
NICK_MAY_NOT_BE_USED
	Этот ник не может быть использован. Пожалуйста выберите любой другой.
FORCENICKCHANGE_IN_1_MINUTE
	Если вы не выполните указанные рекомендации, Ваш ник будет принудительно изменен в течении минуты.
FORCENICKCHANGE_IN_20_SECONDS
	Если вы не выполните указанные рекомендации, Ваш ник будет принудительно изменен в течении 20 секунд.
FORCENICKCHANGE_NOW
	Этот ник был зарегистрирован; Вы не можете его использовать.
FORCENICKCHANGE_CHANGING
	Ваш ник был изменен на %s

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER password [email]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER password email
NICK_REGISTRATION_DISABLED
	Извините, регистрация ников временно приостановлена.
NICK_REGISTRATION_FAILED
	Извините, ошибка регистрации.
NICK_REG_PLEASE_WAIT
	Пожалуйста подождите %d секунд перед повторным использованием команды REGISTER.
NICK_CANNOT_BE_REGISTERED
	Ник %s не может быть зарегистрирован.
NICK_ALREADY_REGISTERED
	Ник %s уже зарегистрирован!
NICK_REGISTERED
	Ник %s зарегистрирован на Вашу учетную запись: %s
NICK_REGISTERED_NO_MASK
	Nickname %s registered.
NICK_PASSWORD_IS
	Ваш пароль - %s - используйте его в дальнейшем и не забывайте.

# GROUP responses
NICK_REG_DELAY
	You have to be connected longer than %d seconds to register your nick.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP target password
NICK_GROUP_DISABLED
	Извините, группирование ников временно недоступно.
NICK_GROUP_FAILED
	Извините, ошибка группирования ников.
NICK_GROUP_PLEASE_WAIT
	Пожалуйста подождите %d секунд перед повторным использованием команды GROUP.
NICK_GROUP_CHANGE_DISABLED
	Ваш ник уже зарегистрирован; Сначала удалите ник командой /msg %s DROP.
NICK_GROUP_SAME
	Вы уже являетесь членом группы %s.
NICK_GROUP_TOO_MANY
	Слишком много ников в группе %s's ; просмотрите список и удалите часть.
	Напишите /msg %s HELP GLIST и /msg %s HELP DROP 
	для более подробной информации.
NICK_GROUP_JOINED
	Вы успешно внесены в группу %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY password
NICK_IDENTIFY_FAILED
	Извините, но попытка идентификации неудачна.
NICK_IDENTIFY_SUCCEEDED
	Пароль принят - вы признаны как владелец ника.
NICK_IDENTIFY_EMAIL_REQUIRED
	Сначала Вы должны указать Ваш e-mail. Правильный e-mail позволит Вам получить на него пароль,
	если Вы его забудете.
NICK_IDENTIFY_EMAIL_HOWTO
	Напишите команду /msg %S SET EMAIL e-mail чтобы установить e-mail. Ваша приватность защищена;
	этот e-mail не может быть узнан третьим лицом.
NICK_ALREADY_IDENTIFIED
	Вы уже идентифицированы.

# UPDATE responses
NICK_UPDATE_SUCCESS
	Статус обновлен (мемки, вирутальные хосты, режимы на каналах, флаги).
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	Деидентификация выполнена успешно.
NICK_LOGOUT_X_SUCCEEDED
	Ник %s был деидентифицирован.
NICK_LOGOUT_SERVICESADMIN
	Невозможно отлогинить %s, так как это администратор сервисов.

# DROP responses
NICK_DROP_DISABLED
	Извините, ник временно не может быть разрегистрирован.
NICK_DROPPED
	Ваш ник был удален из базы данных.
NICK_X_DROPPED
	Ник %s был удален из базы данных.

# SET responses
NICK_SET_SYNTAX
	SET option parameters
NICK_SET_SERVADMIN_SYNTAX
	SET [nick] option parameters
NICK_SET_DISABLED
	Извините, установка параметров ника временно недоступна.
NICK_SET_UNKNOWN_OPTION
	Неизвестный SET параметр %s.
NICK_SET_UNKNOWN_OPTION_OR_BAD_NICK
	Неизвестный SET параметр %s, или ник не зарегистрирован.
NICK_SET_OPTION_DISABLED
	Параметр %s не может быть установлен в этой IRC сети.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	Новый основной ник, ДОЛЖЕН быть зарегистрированным ником или ником в Вашей группе!
NICK_SET_DISPLAY_CHANGED
	Новый основной ник изменен на %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Извините, невозможно изменить пароль.
NICK_SET_PASSWORD_CHANGED
	Пароль изменен.
NICK_SET_PASSWORD_CHANGED_TO
	Пароль изменен на %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE номер
NICK_SET_LANGUAGE_UNKNOWN
	неизвестный номер языка %d.  Напишите /msg %s HELP SET LANGUAGE для получения списка доступных языков.
NICK_SET_LANGUAGE_CHANGED
	Язык изменен на Русский.

# SET URL responses
NICK_SET_URL_CHANGED
	URL изменен на %s.
NICK_SET_URL_UNSET
	URL удален.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	Адрес E-mail изменен на %s.
NICK_SET_EMAIL_UNSET
	Адрес E-mail удален.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	Вы не можете отключить адрес email в этой IRC сети.
	
# SET ICQ responses
NICK_SET_ICQ_CHANGED
	Номер ICQ установлен: %s.
NICK_SET_ICQ_UNSET
	Номер ICQ удален.
NICK_SET_ICQ_INVALID
	%s не является правильным номером.
	
# SET GREET responses
NICK_SET_GREET_CHANGED
	Приветственное сообщение изменено на %s.
NICK_SET_GREET_UNSET
	Приветственное сообщение удалено.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Защита от захвата ника: ВКЛЮЧЕНА.
NICK_SET_KILL_QUICK
	Защита от захвата ника: ВКЛЮЧЕНА, с небольшой задержкой.
NICK_SET_KILL_IMMED
	Защита от захвата ника: ВКЛЮЧЕНА, без задержки.
NICK_SET_KILL_IMMED_DISABLED
	Параметр IMMED недоступен в этой IRC сети.
NICK_SET_KILL_OFF
	Защита от захвата ника: ВЫКЛЮЧЕНА.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Безопасность: ВКЛЮЧЕНА.
NICK_SET_SECURE_OFF
	Безопасность: ВЫКЛЮЧЕНА.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Приватность: ВКЛЮЧЕНА.
NICK_SET_PRIVATE_OFF
	Приватность: ВЫКЛЮЧЕНА.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Теперь Ваш email будет скрыт из запроса %s INFO.
NICK_SET_HIDE_EMAIL_OFF
	Теперь Ваш email будет показываться по запросу %s INFO.
NICK_SET_HIDE_MASK_ON
	Теперь Ваша последняя маска user@host будет скрыта из запроса %s INFO.
NICK_SET_HIDE_MASK_OFF
	Теперь Ваша последняя маска user@host будет показана по запросу %s INFO.
NICK_SET_HIDE_QUIT_ON
	Ваше последнее сообщение при выходе будет скрыто из запроса %s INFO.
NICK_SET_HIDE_QUIT_OFF
	Ваше последнее сообщение при выходе будет показано по запросу %s INFO.
	
# SET MSG responses
NICK_SET_HIDE_STATUS_ON
	Your services access status will now be hidden from %s INFO displays.
NICK_SET_HIDE_STATUS_OFF
	Your services access status will now be shown in %s INFO displays.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_MSG_ON
	Теперь сервисы будут отвечать Вам посредством привата.
NICK_SET_MSG_OFF
	Теперь сервисы будут отвечать Вам посредством нотисов.

# SET NOEXPIRE responses
NICK_SET_NOEXPIRE_SYNTAX
	SET [nickname] NOEXPIRE {ON | OFF}
NICK_SET_NOEXPIRE_ON
	Ник %s никогда не истечет.
NICK_SET_NOEXPIRE_OFF
	Ник %s может истекать.

# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [mask]
NICK_ACCESS_ALREADY_PRESENT
	Маска %s уже есть в вашем списке доступа.
NICK_ACCESS_REACHED_LIMIT
	Извините, вы можете иметь только %d записей в списке доступа.
NICK_ACCESS_ADDED
	%s добавлено в ваш список доступа.
NICK_ACCESS_NOT_FOUND
	%s не найдено в вашем списке доступа.
NICK_ACCESS_DELETED
	%s удалено из списка доступа.
NICK_ACCESS_LIST
	Список доступа:
NICK_ACCESS_LIST_X
	Список доступа для %s:
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
	%s это %s
NICK_INFO_SERVICES_OPER
	%s оператор сервисов.
NICK_INFO_SERVICES_ADMIN
	%s администратор сервисов.
NICK_INFO_SERVICES_ROOT
	%s is a services root administrator.
NICK_INFO_ADDRESS
	Последняя маска:                 %s
NICK_INFO_ADDRESS_ONLINE
	Сейчас в онлайне c маской:       %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s сейчас в онлайне.
NICK_INFO_TIME_REGGED
	Зарегистрирован:                 %s
NICK_INFO_LAST_SEEN
	Последний раз замечен:           %s
NICK_INFO_LAST_QUIT
	Последнее сообщение при выходе:  %s
NICK_INFO_URL
	URL:                             %s
NICK_INFO_EMAIL
	адрес E-mail:                    %s
NICK_INFO_VHOST
	Виртуальный хост:                %s
NICK_INFO_VHOST2
	vhost: 				 %s@%s
NICK_INFO_ICQ
	ICQ #:                           %d
NICK_INFO_GREET
	Сообщение приветствия:           %s
NICK_INFO_OPTIONS
	Параметры:                       %s
NICK_INFO_EXPIRE
	Expires in: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Защита
NICK_INFO_OPT_SECURE
	Безопасность
NICK_INFO_OPT_PRIVATE
	Приватность
NICK_INFO_OPT_MSG
	Режим приватных сообщений
NICK_INFO_OPT_NONE
	нет
NICK_INFO_NO_EXPIRE
	Этот ник никогда не истечет.
NICK_INFO_FOR_MORE
	Для получения более подробной информации напишите /msg %s INFO %s ALL.

# LIST responses
NICK_LIST_SYNTAX
	LIST маска
NICK_LIST_SERVADMIN_SYNTAX
	LIST маска [FORBIDDEN] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Список, содержащий %s:
NICK_LIST_RESULTS
	Конец списка - %d/%d записей.

# ALIST responses
NICK_ALIST_SYNTAX
	ALIST ник
NICK_ALIST_HEADER
	Каналы, на которых у Вас есть уровни доступа:
	Номер Канал             Уровень  Описание
NICK_ALIST_HEADER_X
	Каналы, на которых у %s есть уровни доступа:
	Номер Канал             Уровень  Описание
NICK_ALIST_XOP_FORMAT
	  %3d %c%-20s %-8s %s
NICK_ALIST_ACCESS_FORMAT
	  %3d %c%-20s %-8d %s
NICK_ALIST_FOOTER
	Конец списка - %d/%d каналов найдено.

# GLIST responses
NICK_GLIST_HEADER
	Список ников в Вашей группе:
NICK_GLIST_HEADER_X
	Список ников в группе %s:
NICK_GLIST_FOOTER
	%d ников в группе.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (expires in %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER ник [пароль]
NICK_NO_RECOVER_SELF
	Вы не можете восстановить самого себя!
NICK_RECOVERED
	По требованию пользователя Ваш ник был убит. Напишите /msg %s RELEASE %s
	в течение минуты, чтобы Вы смогли его использовать.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE ник [пароль]
NICK_RELEASE_NOT_HELD
	Ник %s не был удержан.
NICK_RELEASED
	Сервисы удержали Ваш ник и освободили.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST ник [пароль]
NICK_NO_GHOST_SELF
	Вы не можете вышибить самого себя!
NICK_GHOST_KILLED
	Ваш зависший ник был убит.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nickname
NICK_GETPASS_UNAVAILABLE
	команда GETPASS недоступна так как включено шифрование.
NICK_GETPASS_PASSWORD_IS
	Пароль %s - %s.

# GETEMAIL responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host не используйте символьные маски !!
NICK_GETEMAIL_EMAILS_ARE
	Email'ы подходящие под %s: %s.
NICK_GETEMAIL_NOT_USED
	Нет Email'ов для %s.

# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS ник
NICK_SENDPASS_UNAVAILABLE
	команда SENDPASS недоступна так как включено шифрование.
NICK_SENDPASS_SUBJECT
	Пароль на ник (%s)
NICK_SENDPASS_HEAD
	Hi,
NICK_SENDPASS_LINE_1
	Вы просили %s выслать пароль от Вашего ника на e-mail.
NICK_SENDPASS_LINE_2
	Ваш пароль %s. Для большей безопасности, Вам рекомендуется изменить как можно скорее.
NICK_SENDPASS_LINE_3
	Если Вы не запрашивали это письмо, пожалуйста проигнорируйте его.
NICK_SENDPASS_LINE_4
	ПОЖАЛУЙСТА НЕ ОТВЕЧАЙТЕ НА ЭТО ПИСЬМО!
NICK_SENDPASS_LINE_5
	Администраторы %s.
NICK_SENDPASS_OK
	Пароль %s был отправлен.

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID ник [причина]
NICK_FORBID_SYNTAX_REASON
	FORBID ник причина
NICK_FORBID_SUCCEEDED
	Ник %s запрещен к использованию.
NICK_FORBID_FAILED
	Не удалось запретить ник %s!

# Nick Registraion responses
NICK_REQUESTED
	Этот ник уже запрашивал регистрацию, пожалуйста проверьте Ваш e-mail на предмет получения пароля
NICK_REG_RESENT
	Ваш пароль был снова отправлен на %s.
NICK_REG_UNABLE
	Ник НЕ ЗАРЕГИСТРИРОВАН, попробуйте, пожалуйста, позднее.
NICK_IS_PREREG
	Этот ник ожидает подтверждения по e-mail перед завершением регистрации.
NICK_ENTER_REG_CODE
	Пароль был выслан на %s, пожалуйста напишите команду /msg %s confirm <пароль> для завершения регистрации
NICK_CONFIRM_NOT_FOUND
	Время первой части регистрации истекло, пожалуйста используйте сначала "/msg %s register <пароль> <email>" .
NICK_CONFIRM_INVALID
	Был введен неверный пароль, пожалуйста проверьте Ваш e-mail снова, и попробуйте еще раз
NICK_REG_MAIL_SUBJECT
	Регистрация ника (%s)
NICK_REG_MAIL_HEAD
	Hi,
NICK_REG_MAIL_LINE_1
	Вы хотели зарегистрировать Ваш ник на %s.
NICK_REG_MAIL_LINE_2
	Пожалуйста напишите команду " /msg %s confirm %s " чтобы завершить регистрацию.
NICK_REG_MAIL_LINE_3
	Если Вы не запрашивали это письмо, пожалуйста проигнорируйте его.
NICK_REG_MAIL_LINE_4
	ПОЖАЛУЙСТА НЕ ОТВЕЧАЙТЕ НА ЭТО ПИСЬМО!
NICK_REG_MAIL_LINE_5
	Администраторы %s.
NICK_GETPASS_PASSCODE_IS
	Пароль %s - %s.
NICK_FORCE_REG
	Ник %s подтвердил регистрацию



###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Автоматически устанавливает статус оператора канала
CHAN_LEVEL_AUTOVOICE
	Автоматически ставит режим +v
CHAN_LEVEL_AUTOHALFOP
	Автоматически ставит режим +h
CHAN_LEVEL_AUTOPROTECT
	Автоматически ставит режим +a
CHAN_LEVEL_AUTODEOP
	Статус оператора канала запрещен
CHAN_LEVEL_NOJOIN
	Невозможно зайти в канал: доступ ограничен
CHAN_LEVEL_INVITE
	Можно использовать команду INVITE
CHAN_LEVEL_AKICK
	Можно использовать команду AKICK
CHAN_LEVEL_SET
	Можно использовать команду SET (кроме FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Можно использовать команду CLEAR
CHAN_LEVEL_UNBAN
	Можно использовать команду UNBAN
CHAN_LEVEL_OPDEOP
	Можно использовать команды OP/DEOP
CHAN_LEVEL_ACCESS_LIST
	Можно просматривать список доступа
CHAN_LEVEL_ACCESS_CHANGE
	Можно модифицировать список доступа
CHAN_LEVEL_MEMO
	Можно читать сообщения для канала
CHAN_LEVEL_ASSIGN
	Можно добавлять/убирать бота
CHAN_LEVEL_BADWORDS
	Можно использовать команду BADWORDS
CHAN_LEVEL_NOKICK
	Никогда не будет кикнут ботом
CHAN_LEVEL_FANTASIA
	Можно использовать fantaisist команды BotServ'a
CHAN_LEVEL_SAY
	Можно использовать команды SAY и ACT
CHAN_LEVEL_GREET
	Показывать приветственное сообщение
CHAN_LEVEL_VOICEME
	Можно использовать команду (de)voice на себя
CHAN_LEVEL_VOICE
	Можно использовать команды VOICE/DEVOICE
CHAN_LEVEL_GETKEY
	Можно использовать команду GETKEY
CHAN_LEVEL_OPDEOPME
	Можно использовать команды OP/DEOP на себя
CHAN_LEVEL_HALFOPME
	Можно использовать команды HALFOP/DEHALFOP на себя
CHAN_LEVEL_HALFOP
	Можно использовать команды HALFOP/DEHALFOP
CHAN_LEVEL_PROTECTME
	Можно использовать команду PROTECT/DEPROTECT на себя
CHAN_LEVEL_PROTECT
	Можно использовать команды PROTECT/DEPROTECT
CHAN_LEVEL_KICKME
	Можно использовать команду KICK на себя
CHAN_LEVEL_KICK
	Можно использовать KICK
CHAN_LEVEL_SIGNKICK
	Невозможно кикать, когда используется SIGNKICK LEVEL
CHAN_LEVEL_BANME
	Можно использовать команду BAN на себя
CHAN_LEVEL_BAN
	Можно использовать команду BAN
CHAN_LEVEL_TOPIC
	Можно использовать команду TOPIC
CHAN_LEVEL_INFO
	Можно использовать команду INFO с параметром ALL

# Automatic responses
CHAN_IS_REGISTERED
	Этот канал был зарегистрирован %s.
CHAN_NOT_ALLOWED_OP
	Вы должны быть оператором канала %s.
CHAN_MAY_NOT_BE_USED
	Этот канал нельзя использовать.
CHAN_NOT_ALLOWED_TO_JOIN
	Вам не разрешено находиться на этом канале.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER #канал пароль описание
CHAN_REGISTER_DISABLED
	Извините, регистрация каналов временно приостановлена.
CHAN_REGISTER_NOT_LOCAL
	Локальные каналы не могут быть зарегистрированы.
CHAN_MUST_REGISTER_NICK
	Сначала Вы должны зарегистрировать свой ник. Напишите /msg %s HELP для получения информации о том, как зарегистрировать ник.
CHAN_MUST_IDENTIFY_NICK
	Сперва идентифицируйтесь пожалуйста на %s, используя команду:
	/msg %s IDENTIFY пароль
CHAN_MAY_NOT_BE_REGISTERED
	Канал %s не может быть зарегистрирован.
CHAN_ALREADY_REGISTERED
	Канал %s уже зарегистрирован!
CHAN_MUST_BE_CHANOP
	Вы должны быть оператором на канале, чтобы зарегистрировать его.
CHAN_REACHED_CHANNEL_LIMIT
	Извините, Вы уже зарегистрировали %d каналов, больше, к сожалению, нельзя.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Извините, Вы уже превысили лимит (%d) зарегистрированных каналов.
CHAN_REGISTRATION_FAILED
	Извините, ошибка регистрации.
CHAN_REGISTERED
	Канал %s зарегистрирован на Вас: %s
CHAN_PASSWORD_IS
	Пароль Вашего канала -  %s - не забывайте его!
CHAN_REGISTER_NONE_CHANNEL
	You have attempted to register a none existant channel %s
CHAN_SYMBOL_REQUIRED
	Please use the symbol of # or & when attempting to register


# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY #канал пароль
CHAN_IDENTIFY_FAILED
	Неверный пароль.
CHAN_IDENTIFY_SUCCEEDED
	Пароль принят - Теперь Ваш уровень доступа на %s - владелец.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT #канал ник
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT #канал [ник]
CHAN_LOGOUT_SUCCEEDED
	Пользователь %s ьыл деидентифицирован на канал %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	Все идентифицированные пользователи были деидентифицированы на канал %s.

# DROP responses
CHAN_DROP_SYNTAX
	DROP #канал
CHAN_DROP_DISABLED
	Извините, удаление канала временно невозможно.
CHAN_DROPPED
	Канал %s был удален.

# SET responses
CHAN_SET_SYNTAX
	SET #канал опция параметр
CHAN_SET_DISABLED
	Извините, установка опций для каналов временно недоступна.
CHAN_SET_UNKNOWN_OPTION
	Неизвестная опция SET %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	У %s слишком много зарегистрированных каналов.
CHAN_FOUNDER_CHANGED
	Владелец канала %s изменен на %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Наследник канала %s изменен на %s.
CHAN_SUCCESSOR_UNSET
	Наследник %s удален.
CHAN_SUCCESSOR_IS_FOUNDER
	%s не может быть наследником канала %s, так как он его владелец.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Извините, невозможно установить пароль.
CHAN_PASSWORD_CHANGED
	Пароль %s изменен.
CHAN_PASSWORD_CHANGED_TO
	Пароль канала %s изменен на %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Описание канала %s изменено на %s.

# SET URL responses
CHAN_URL_CHANGED
	URL канала %s изменен на %s.
CHAN_URL_UNSET
	URL на канал %s удален.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	Адрес E-mail для канала %s изменен на %s.
CHAN_EMAIL_UNSET
	Адрес E-mail для канала %s удален.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Сообщение при входе на канал for %s изменено.
CHAN_ENTRY_MSG_UNSET
	Сообщение при входе на канал %s удалено.
	
# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s не является верным типом бана.
CHAN_SET_BANTYPE_CHANGED
	Тип бана на канале %s теперь #%d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Неизвестный режим %c - проигнорирован.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Режим %c не установлен, так как Вы не можете заблокировать его.
CHAN_SET_MLOCK_L_REQUIRED
	Чтобы заблокировать режим +l Вы должны также заблокировать и режим +L.
CHAN_SET_MLOCK_K_REQUIRED
	тобы заблокировать режим +i Вы должны также заблокировать и режим +K.
CHAN_MLOCK_CHANGED
	Заблокированные режимы на канале %s изменены на %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET #канал KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Удерживание топика ВКЛЮЧЕНО.
CHAN_SET_KEEPTOPIC_OFF
	Удерживание топика ВЫКЛЮЧЕНО.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET #канал TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Блокирование топика ВКЛЮЧЕНО.
CHAN_SET_TOPICLOCK_OFF
	Блокирование топика ВЫКЛЮЧЕНО.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET #канал PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Опция спокойствия ВКЛЮЧЕНА.
CHAN_SET_PEACE_OFF
	Опция спокойствия ВЫКЛЮЧЕНА.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET #канал PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Приватность канала ВКЛЮЧЕНА.
CHAN_SET_PRIVATE_OFF
	Приватность канала ВЫКЛЮЧЕНА.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET #канал SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Безопасность статуса оператора канала ВКЛЮЧЕНА.
CHAN_SET_SECUREOPS_OFF
	Безопасность статуса оператора канала ВЫКЛЮЧЕНА.
	
# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET #канал SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Безопасность владельца ВКЛЮЧЕНА.
CHAN_SET_SECUREFOUNDER_OFF
	Безрпасность владельца ВЫКЛЮЧЕНА.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET #канал RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Ограниченный доступ на канал ВКЛЮЧЕН.
CHAN_SET_RESTRICTED_OFF
	Ограниченный доступ на канал ВЫКЛЮЧЕН.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET #канал SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Безопасность ВКЛЮЧЕНА.
CHAN_SET_SECURE_OFF
	Безопасность ВЫКЛЮЧЕНА.
	
# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET #канал SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Подпись киков на канале ВКЛЮЧЕНА.
CHAN_SET_SIGNKICK_LEVEL
	Подпись киков на канале ВКЛЮЧЕНА, но зависит от уровня пользователя, 
	использовшего данную команду.
CHAN_SET_SIGNKICK_OFF
	Подпись киков на канале ВЫКЛЮЧЕНА.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET #канал OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Уведомление об использовании команды OP ВКЛЮЧЕНО.
CHAN_SET_OPNOTICE_OFF
	Уведомление об использовании команды OP ВЫКЛЮЧЕНО.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET #канал XOP {ON | OFF}
CHAN_SET_XOP_ON
	Тип списка доступа xOP ВКЛЮЧЕН.
CHAN_SET_XOP_OFF
	Тип списка доступа xOP ВЫКЛЮЧЕН.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET #канал NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Канал %s никогда не истечет.
CHAN_SET_NOEXPIRE_OFF
	Канал %s может истекать.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Извините, Вы можете использовать только %d AOP/SOP/VOP на канале.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	Вы не можете использоват эту команду. Вместо нее используйте команду ACCESS.
	Напишите /msg %s HELP ACCESS для получения более подробной информации.

# AOP messages
CHAN_AOP_SYNTAX
	AOP #канал {ADD|DEL|LIST|CLEAR} [ник | номера записей]
CHAN_AOP_DISABLED
	Извините, модификация списка AOP'ов временно недоступна.
CHAN_AOP_NICKS_ONLY
	Список AOP может включать только зарегистрированные ники.
CHAN_AOP_ADDED
	%s добавлен в AOP'ы канала %s.
CHAN_AOP_MOVED
	%s перемещен в AOP'ы %s.
CHAN_AOP_NO_SUCH_ENTRY
	Нет такой записи (#%d) в AOP'ах %s.
CHAN_AOP_NOT_FOUND
	%s не найден в AOP'ах %s.
CHAN_AOP_NO_MATCH
	Нет таких записей в AOP'ах канала %s.
CHAN_AOP_DELETED
	%s удален из AOP'ов канала %s.
CHAN_AOP_DELETED_ONE
	Удалена 1 запись из AOP'ов канала %s.
CHAN_AOP_DELETED_SEVERAL
	Удалено %d записей из AOP'ов канала %s.
CHAN_AOP_LIST_EMPTY
	Список AOP'ов канала %s пуст.
CHAN_AOP_LIST_HEADER
	Список AOP'ов канала %s:
	 Номер  Ник
CHAN_AOP_CLEAR
	Список AOP'ов канала был очищен.

# HOP messages
CHAN_HOP_SYNTAX
	HOP #канал {ADD|DEL|LIST|CLEAR} [ник | номера записей]
CHAN_HOP_DISABLED
	Извините, модификация списка HOP'ов временно недоступна.
CHAN_HOP_NICKS_ONLY
	Список HOP может включать только зарегистрированные ники.
CHAN_HOP_ADDED
	%s добавлен в HOP'ы канала %s.
CHAN_HOP_MOVED
	%s перемещен в HOP'ы %s.
CHAN_HOP_NO_SUCH_ENTRY
	Нет такой записи (#%d) в HOP'ах %s.
CHAN_HOP_NOT_FOUND
	%s не найден в HOP'ах %s.
CHAN_HOP_NO_MATCH
	Нет таких записей в HOP'ах канала %s.
CHAN_HOP_DELETED
	%s удален из HOP'ов канала %s.
CHAN_HOP_DELETED_ONE
	Удалена 1 запись из HOP'ов канала %s.
CHAN_HOP_DELETED_SEVERAL
	Удалено %d записей из HOP'ов канала %s.
CHAN_HOP_LIST_EMPTY
	Список HOP'ов канала %s пуст.
CHAN_HOP_LIST_HEADER
	Список HOP'ов канала %s:
	  Номер  Ник
CHAN_HOP_CLEAR
	Список HOP'ов канала был очищен.

# SOP messages
CHAN_SOP_SYNTAX
	SOP #канал {ADD|DEL|LIST|CLEAR} [ник | номера записей]
CHAN_SOP_DISABLED
	Извините, модификация списка SOP'ов временно недоступна.
CHAN_SOP_NICKS_ONLY
	Список SOP может включать только зарегистрированные ники.
CHAN_SOP_ADDED
	%s добавлен в SOP'ы канала %s.
CHAN_SOP_MOVED
	%s перемещен в SOP'ы %s.
CHAN_SOP_NO_SUCH_ENTRY
	Нет такой записи (#%d) в SOP'ах %s.
CHAN_SOP_NOT_FOUND
	%s не найден в SOP'ах %s.
CHAN_SOP_NO_MATCH
	Нет таких записей в SOP'ах канала %s.
CHAN_SOP_DELETED
	%s удален из SOP'ов канала %s.
CHAN_SOP_DELETED_ONE
	Удалена 1 запись из SOP'ов канала %s.
CHAN_SOP_DELETED_SEVERAL
	Удалено %d записей из SOP'ов канала %s.
CHAN_SOP_LIST_EMPTY
	Список SOP'ов канала %s пуст.
CHAN_SOP_LIST_HEADER
	Список SOP'ов канала %s:
	  Номер  Ник
CHAN_SOP_CLEAR
	Список SOP'ов канала был очищен.

# VOP messages
CHAN_VOP_SYNTAX
	VOP #канал {ADD|DEL|LIST|CLEAR} [ник | номера записей]
CHAN_VOP_DISABLED
	Извините, модификация списка VOP'ов временно недоступна.
CHAN_VOP_NICKS_ONLY
	Список VOP может включать только зарегистрированные ники.
CHAN_VOP_ADDED
	%s добавлен в VOP'ы канала %s.
CHAN_VOP_MOVED
	%s перемещен в VOP'ы %s.
CHAN_VOP_NO_SUCH_ENTRY
	Нет такой записи (#%d) в VOP'ах %s.
CHAN_VOP_NOT_FOUND
	%s не найден в VOP'ах %s.
CHAN_VOP_NO_MATCH
	Нет таких записей в VOP'ах канала %s.
CHAN_VOP_DELETED
	%s удален из VOP'ов канала %s.
CHAN_VOP_DELETED_ONE
	Удалена 1 запись из VOP'ов канала %s.
CHAN_VOP_DELETED_SEVERAL
	Удалено %d записей из VOP'ов канала %s.
CHAN_VOP_LIST_EMPTY
	Список VOP'ов канала %s пуст.
CHAN_VOP_LIST_HEADER
	Список VOP'ов канала %s:
	  Номер  Ник
CHAN_VOP_CLEAR
	Список VOP'ов канала был очищен.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS #канал {ADD|DEL|LIST|CLEAR} [ник [уровень] | список записей]
CHAN_ACCESS_XOP
	Вы не можете использовать эту команду. Используйте команды AOP, SOP и VOP вместо нее.
	Напишите /msg %s HELP команда для получения
	более подробной информации.
CHAN_ACCESS_DISABLED
	Извините, модификация списка доступа канала временно недоступна.
CHAN_ACCESS_LEVEL_NONZERO
	Уровень доступа должен быть отличен от нуля.
CHAN_ACCESS_LEVEL_RANGE
	Уровень доступа должен быть между %d и %d включительно.
CHAN_ACCESS_NICKS_ONLY
	Список доступа канала может включать только зарегистрированные ники.
CHAN_ACCESS_REACHED_LIMIT
	Извините, в списке доступа канала может быть не более %d записей.
CHAN_ACCESS_LEVEL_UNCHANGED
	Уровень доступа для %s на канал %s остался прежним - %d.
CHAN_ACCESS_LEVEL_CHANGED
	Уровень доступа для %s на канал %s изменен на %d.
CHAN_ACCESS_ADDED
	%s добавлен в список доступа канала %s с уровнем доступа %d.
CHAN_ACCESS_NO_SUCH_ENTRY
	Нет записи (#%d) в списке доступа канала %s.
CHAN_ACCESS_NOT_FOUND
	%s не найден в списке доступа канала %s.
CHAN_ACCESS_NO_MATCH
	Нет таких записей в списке доступа канала %s.
CHAN_ACCESS_DELETED
	%s удален из списка доступа канала %s.
CHAN_ACCESS_DELETED_ONE
	Удалена 1 запись из списка доступа канала %s.
CHAN_ACCESS_DELETED_SEVERAL
	Удалено %d записей из списка доступа канала %s.
CHAN_ACCESS_LIST_EMPTY
	Список доступа канала %s пуст.
CHAN_ACCESS_LIST_HEADER
	Список доступа канала %s:
	Номер Уровень Ник
CHAN_ACCESS_LIST_FOOTER
	Конец списка доступа.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	Список доступа канала очищен.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK #канал {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [ник-или-маска-пользователя] [причина]
CHAN_AKICK_DISABLED
	Извините, модификация списка автокиков канала временно недоступна.
CHAN_AKICK_ALREADY_EXISTS
	%s уже есть в списке автокиков канала %s.
CHAN_AKICK_REACHED_LIMIT
	Извините, Вы можете использовать для автокика только маски %d на канале.
CHAN_AKICK_ADDED
	%s добавлен в список автокиков канала %s.
CHAN_AKICK_NO_SUCH_ENTRY
	Нет такой записи (#%d) в списке автокиков канала %s.
CHAN_AKICK_NOT_FOUND
	%s не найден в списке автокиков канала %s.
CHAN_AKICK_NO_MATCH
	Нет таких значение в списке автокиков канала %s.
CHAN_AKICK_STUCK
	%s теперь всегда активен на канале %s.
CHAN_AKICK_UNSTUCK
	%s больше не активен на канале %s.
CHAN_AKICK_DELETED
	%s удален из списка автокиков канала %s.
CHAN_AKICK_DELETED_ONE
	Удалена 1 запись из списка автокиков канала %s.
CHAN_AKICK_DELETED_SEVERAL
	Удалено %d записей из списка актокиков канала %s.
CHAN_AKICK_LIST_EMPTY
	Список автокиков канала %s пуст.
CHAN_AKICK_LIST_HEADER
	Список автокиков на канал %s:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (by %s on %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (перманентный) (by %s on %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE на канале %s завершен; %d пользователей выкинуто.
CHAN_AKICK_CLEAR
	Список автокиков на канал очищен.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS #канал {SET | DIS[ABLE] | LIST | RESET} [пункт [уровень]]
CHAN_LEVELS_XOP
	Эта команда не может быть использована в данном режиме.
CHAN_LEVELS_RANGE
	Уровень должен быть цифрой между %d и %d включительно.
CHAN_LEVELS_CHANGED
	Уровень %s на канале %s изменен на %d.
CHAN_LEVELS_UNKNOWN
	Неизвестный параметр %s. Напишите /msg %s HELP LEVELS DESC для получения списка возможных значений.
CHAN_LEVELS_DISABLED
	%s отключен на канале %s.
CHAN_LEVELS_LIST_HEADER
	Установки списка доступа для канала %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (отключено)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (только для владельца)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	Уровни доступа для канала %s сброшены на значения по умолчанию.

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
	INFO #канал [ALL]
CHAN_INFO_HEADER
	Информация о канале %s:
CHAN_INFO_FOUNDER
	Владелец:                  %s (%s)
CHAN_INFO_NO_FOUNDER
	Владелец:                  %s
CHAN_INFO_SUCCESSOR
	Наследник:                 %s (%s)
CHAN_INFO_NO_SUCCESSOR
	Наследник:                 %s
CHAN_INFO_DESCRIPTION
	Описание:                  %s
CHAN_INFO_ENTRYMSG
	Сообщение при входе:       %s
CHAN_INFO_TIME_REGGED
	Зарегистрирован:           %s
CHAN_INFO_LAST_USED
	Последний раз использован: %s
CHAN_INFO_LAST_TOPIC
	Последний топик:           %s
CHAN_INFO_TOPIC_SET_BY
	Топик установил:           %s
CHAN_INFO_URL
	URL:                       %s
CHAN_INFO_EMAIL
	Адрес E-mail:              %s
CHAN_INFO_BANTYPE
	Тип бана:                  %d
CHAN_INFO_OPTIONS
	Опции:                     %s
CHAN_INFO_OPT_KEEPTOPIC
	Удерживание Топика
CHAN_INFO_OPT_OPNOTICE
	OP Уведомление
CHAN_INFO_OPT_PEACE
	Спокойствие
CHAN_INFO_OPT_PRIVATE
	Приватный
CHAN_INFO_OPT_RESTRICTED
	Ограниченный Доступ
CHAN_INFO_OPT_SECURE
	Безопасность
CHAN_INFO_OPT_SECUREOPS
	Безопасноть Статуса Опа
CHAN_INFO_OPT_SECUREFOUNDER
	Безопасность Владельца
CHAN_INFO_OPT_SIGNKICK
	Подписанные Кики
CHAN_INFO_OPT_TOPICLOCK
	Блокировка Топика
CHAN_INFO_OPT_XOP
	Система Списка Доступа xOP
CHAN_INFO_OPT_NONE
	Нет
CHAN_INFO_MODE_LOCK
	Блокировка режимов: %s
CHAN_INFO_EXPIRE
	      Expires in: %s
CHAN_INFO_NO_EXPIRE
	Этот канал никогда не истечет.

# LIST responses
CHAN_LIST_SYNTAX
	LIST шаблон
CHAN_LIST_SERVADMIN_SYNTAX
	LIST шаблон [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
CHAN_LIST_HEADER
	Список записей, содержащий %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Конец списка - найдено %d/%d записей.
	
# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE #канал

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN #канал
CHAN_UNBANNED
	Вы были разбанены на канале %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC #канал [топик]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR #канал опция
CHAN_CLEARED_BANS
	Все баны на канале %s были удалены.
CHAN_CLEARED_EXCEPTS
	Все исключения из банов на канале %s были удалены.
CHAN_CLEARED_MODES
	Все режимы на канале %s были сброшены.
CHAN_CLEARED_OPS
	Все опы были сняты на канале %s.
CHAN_CLEARED_HOPS
	Mode +h has been cleared from channel %s.
CHAN_CLEARED_VOICES
	Все войсы были сняты на канале %s.
CHAN_CLEARED_USERS
	Все пользователи были выкинуты с канала %s.
CHAN_CLEARED_INVITES
	All invites on channel %s have been removed.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS #канал
CHAN_GETPASS_UNAVAILABLE
	Команда GETPASS недоступна при включенном шифровании.
CHAN_GETPASS_PASSWORD_IS
	Пароль на канал %s - %s.
	
# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS #канал
CHAN_SENDPASS_UNAVAILABLE
	Команда SENDPASS недоступна при включенном шифровании.
CHAN_SENDPASS_SUBJECT
	Пароль канала (%s)
CHAN_SENDPASS_HEAD
	Hi,
CHAN_SENDPASS_LINE_1
	Вы просили %s выслать пароль от Вашего канала на e-mail.
CHAN_SENDPASS_LINE_2
	Пароль Вашего канала %s. Для большей безопасности, Вам рекомендуется изменить как можно скорее.
CHAN_SENDPASS_LINE_3
	Если Вы не запрашивали это письмо, пожалуйста проигнорируйте его.
CHAN_SENDPASS_LINE_4
	ПОЖАЛУЙСТА НЕ ОТВЕЧАЙТЕ НА ЭТО ПИСЬМО!
CHAN_SENDPASS_LINE_5
	Администраторы %s.
CHAN_SENDPASS_OK
	Пароль канала %s был отправлен.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID #канал [причина]
CHAN_FORBID_SYNTAX_REASON
	FORBID #канал причина
CHAN_FORBID_SUCCEEDED
	Канал %s теперь запрещен к использованию.
CHAN_FORBID_FAILED
	Невозможно запретить использовать канал %s!
CHAN_FORBID_REASON
	Этот канал запрещен к использованию.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND #канал [причина]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND #канал причина
CHAN_SUSPEND_SUCCEEDED
	Канал %s приостановлен.
CHAN_SUSPEND_FAILED
	Невозможно приостановить канал %s!
CHAN_SUSPEND_REASON
	Этот канал был приостановлен.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND #канал
CHAN_UNSUSPEND_ERROR
	Отсутствует # перед названием канала.
CHAN_UNSUSPEND_SUCCEEDED
	Канал %s восстановлен.
CHAN_UNSUSPEND_FAILED
	Невозможно восстановить канал %s!

# Misc responses
CHAN_EXCEPTED
	Значения %s исключены на %s и не могут быть забанены пока исключения не будут удалены.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	Вам пришло 1 новое сообщение.
MEMO_HAVE_NEW_MEMOS
	Вам пришло %d новых сообщений.
MEMO_TYPE_READ_LAST
	Напишите /msg %s READ LAST чтобы прочитать его.
MEMO_TYPE_READ_NUM
	Напишите /msg %s READ %d чтобы прочитать его.
MEMO_TYPE_LIST_NEW
	Напишите /msg %s LIST NEW чтобы посмотреть список соощений.
MEMO_AT_LIMIT
	Внимание: Ваш почтовый ящик для сообщений переполнен! У Вас уже (%d) сообщений. Вы не сможете принимать новые сообщения, пока Вы не удалите часть сообщений из ящика.
MEMO_OVER_LIMIT
	Внимание: Ваш почтовый ящик для сообщений переполнен! У Вас уже (%d) сообщений. Вы не сможете принимать новые сообщения, пока Вы не удалите часть сообщений из ящика.
MEMO_X_MANY_NOTICE
	%d сообщений на канале %s.
MEMO_X_ONE_NOTICE
	%d сообщение на канале %s.
MEMO_NEW_X_MEMO_ARRIVED
	На канале %s новое сообщение.
	Напишите /msg %s READ %s %d чтобы прочитать его.
MEMO_NEW_MEMO_ARRIVED
	Вам пришло новое сообщение от %s.
	Напишите /msg %s READ %d чтобы прочитать его.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	У вас нет сообщений.
MEMO_X_HAS_NO_MEMOS
	У %s нет сообщений.
MEMO_DOES_NOT_EXIST
	Сообщения %d не существует!
MEMO_LIST_NOT_FOUND
	Такие сообщения не найдены.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {ник | #канал} текст сообщения
MEMO_SEND_DISABLED
	Извините, отправка сообщений временно приостановлена.
MEMO_SEND_PLEASE_WAIT
	Пожалуйста подождите %d секунд перед тем, как использовать команду SEND снова.
MEMO_X_GETS_NO_MEMOS
	%s не может принимать сообщения.
MEMO_X_HAS_TOO_MANY_MEMOS
	У %s переполнен ящик и в данный момент он не может принимать сообщения.
MEMO_SENT
	Сообщение отправлено %s.
MEMO_MASS_SENT
	Массовое сообщение было отправлено всем заргеистрированным пользователям.

	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {ник | #канал}
MEMO_CANCEL_DISABLED
	Извините, отмена сообщений временно недоступна.
MEMO_CANCEL_NONE
	Нет сообщений, доступных дл отмены.
MEMO_CANCELLED
	Последнее сообщение для %s было отменено.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [#канал] [list | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	У вас нет новых сообщений.
MEMO_X_HAS_NO_NEW_MEMOS
	У %s нет новых сообщений.
MEMO_LIST_MEMOS
	Сообщения для %s. Чтобы прочесть их, напишите: /msg %s READ номер сообщения
MEMO_LIST_NEW_MEMOS
	Новые сообщения для %s. Чтобы прочесть их, напишите: /msg %s READ номер сообщения
MEMO_LIST_CHAN_MEMOS
	Сообщения для %s. Чтобы прочесть их, напишите: /msg %s READ %s номер сообщения
MEMO_LIST_CHAN_NEW_MEMOS
	Новые сообщения для %s.  Чтобы прочесть их, напишите: /msg %s READ %s номер сообщения
MEMO_LIST_HEADER
	Номер  Отправитель           Дата/Время
MEMO_LIST_FORMAT
	%c%3d    %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [#канал] {list | LAST | NEW}
MEMO_HEADER
	Сообщение %d от %s (%s). Чтобы удалить его, напишите команду: /msg %s DEL %d
MEMO_CHAN_HEADER
	Сообщение %d от %s (%s). Чтобы удалить его, напишите команду: /msg %s DEL %s %d
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [#канал] {номер сообщения | list | ALL}
MEMO_DELETED_NONE
	Нет сообщений для удаления.
MEMO_DELETED_ONE
	Сообщение %d было удалено.
MEMO_DELETED_SEVERAL
	Сообщения %s были удалены.
MEMO_DELETED_ALL
	Все Ваши вообщения были удалены.
MEMO_CHAN_DELETED_ALL
	Все сообщения для канала %s были удалены.

# SET responses
MEMO_SET_SYNTAX
	SET опция параметр
MEMO_SET_DISABLED
	Извините, установка опций для службы сообщений временно невозможна.
MEMO_SET_UNKNOWN_OPTION
	Неизвестная опуия SET %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s будет уведомлять Вас о новых сообщениях, когда Вы зайдете в сеть и когда сообщения будут отправлены Вам.
MEMO_SET_NOTIFY_LOGON
	%s будет уведомлять Вас о новых сообщениях, когда Вы зайдете в сеть или снимете с себя флаг /AWAY.
MEMO_SET_NOTIFY_NEW
	%s будет уведомлять Вас о новых сообщениях, когда они будет отправлены Вам.
MEMO_SET_NOTIFY_OFF
	%s не будет уведомлять Вас о новых сообщениях.

# SET LIMIT responses
MEMO_SET_NOTIFY_MAIL
	You will now be informed about new memos via email.
MEMO_SET_NOTIFY_NOMAIL
	You will no longer be informed via email.
MEMO_SET_NOTIFY_INVALIDMAIL
	There's no email address set for your nick.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [#канал] limit
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [пользователь | #канал] {limit | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Вы не можете изменить лимит сообщений для Вас.
MEMO_SET_LIMIT_FORBIDDEN
	Лимит сообщений для %s был изменен.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Вы не можете установить лимит сообщений больше, чем %d.
MEMO_SET_LIMIT_TOO_HIGH
	Вы не можете установить лимит сообщений для %s больше чем %d.
MEMO_SET_LIMIT_OVERFLOW
	Лимит сообщений чересчур велик; уменьшите его до %d.
MEMO_SET_YOUR_LIMIT
	Лимит сообщений был установлен в %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Вы больше не будете принимать сообщения.
MEMO_UNSET_YOUR_LIMIT
	Лимит сообщений для Вас отключен.
MEMO_SET_LIMIT
	Лимит собщений для %s установлен равным %d.
MEMO_SET_LIMIT_ZERO
	Лимит сообщений для %s установлен равным 0.
MEMO_UNSET_LIMIT
	Лимит сообщений для %s отключен.

# INFO responses
MEMO_INFO_SYNTAX
	INFO [#канал]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [ник | #канал]
MEMO_INFO_NO_MEMOS
	На данный момент у Вас нет сообщений.
MEMO_INFO_MEMO
	На данный момент у Вас есть 1 сообщение.
MEMO_INFO_MEMO_UNREAD
	На данный момент у Вас есть 1 сообщений, и они все еще не прочитаны Вами.
MEMO_INFO_MEMOS
	На данный момент у Вас есть %d сообщений.
MEMO_INFO_MEMOS_ONE_UNREAD
	На данный момент у Вас есть %d сообщений, из них 1 непрочитанное.
MEMO_INFO_MEMOS_SOME_UNREAD
	На данный момент у Вас есть %d сообщений, из них %d непрочитанных.
MEMO_INFO_MEMOS_ALL_UNREAD
	На данный момент у Вас есть %d сообщений; все они не прочитаны Вами.
MEMO_INFO_LIMIT
	Ваш лимит сообщений равен %d.
MEMO_INFO_HARD_LIMIT
	Ваш лимит сообщений равен %d, и не может быть изменен.
MEMO_INFO_LIMIT_ZERO
	Ваш лимит сообщений равен 0; Вы не будете принимать любые новые сообщения.
MEMO_INFO_HARD_LIMIT_ZERO
	Ваш лимит сообщений равен 0; Вы не будете принимать любые новые сообщения.  Вы не можете изменить этот лимит.
MEMO_INFO_NO_LIMIT
	У вас нет ограничеиня на количество хранимых сообщений.
MEMO_INFO_NOTIFY_OFF
	Вы не будете уведомляться о новых собщениях.
MEMO_INFO_NOTIFY_ON
	Вы будете уведомляться о новых сообщениях при входе в сеть и по мере прихода новых сообщений.
MEMO_INFO_NOTIFY_RECEIVE
	Вы будете уведовляться сразу по приходе новых сообщений.
MEMO_INFO_NOTIFY_SIGNON
	Вы будете уведомляться о приходе новых сообщений после входа в сеть.
MEMO_INFO_X_NO_MEMOS
	%s сейчас не имеет сообщений.
MEMO_INFO_X_MEMO
	%s сейчас имеет 1 сообщение.
MEMO_INFO_X_MEMO_UNREAD
	%s сейчас имеет 1 сообщений, и они не были прочитаны.
MEMO_INFO_X_MEMOS
	%s сейчас имеет %d сообщений.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s сейчас имеет %d сообщений, 1 из которых не прочитано.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s сейчас имеет %d сообщений, %d из которых не прочитаны.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s сейчас имеет %d сообщений; все они не прочитаны.
MEMO_INFO_X_LIMIT
	Лимит сообщений для %s равен %d.
MEMO_INFO_X_HARD_LIMIT
	Лимит собщений для %s равен %d, и не может быть изменен.
MEMO_INFO_X_NO_LIMIT
	У %s нет ограничения количества сообщений.
MEMO_INFO_X_NOTIFY_OFF
	%s не уведомляется о новых собщениях.
MEMO_INFO_X_NOTIFY_ON
	%s уведомляется о новых сообщениях после входа в сеть и по мере прихода собщений.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s уведомляется о новых сообщениях по мере их прихода.
MEMO_INFO_X_NOTIFY_SIGNON
	%s уведомляется о новых сообщениях после захода в сеть.
	
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
BOT_DOES_NOT_EXIST
	Бот %s не существует.
BOT_NOT_ASSIGNED
	Вы должны назначить бота на канал перед тем, как использовать эту команду.
	Напишите /msg %S HELP ASSIGN для получения более подробной информации.
BOT_NOT_ON_CHANNEL
	Бота нет на канале %s.
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	Запрещено использовать слово %s на этом канале!
BOT_REASON_BADWORD_GENTLE
	Следи за своим языком!
BOT_REASON_BOLD
	Не используйте жирный шрифт на этом канале!
BOT_REASON_CAPS
	Выключите CapsLOCK!
BOT_REASON_COLOR
	Не используйте выделение цветом на этом канале!
BOT_REASON_FLOOD
	Хватит флудить!
BOT_REASON_REPEAT
	Хватить повторов!
BOT_REASON_REVERSE
	Не используйте инверсию на этом канале!
BOT_REASON_UNDERLINE
	Не используйте цвета на этом канале!
	
# !seen replies
BOT_SEEN_BOT
	Ты нашел меня, %s!
BOT_SEEN_YOU
	Любуешься собой, %s?
BOT_SEEN_ON_CHANNEL
	%s на канале прямо сейчас!
BOT_SEEN_ON_CHANNEL_AS
	%s на канале прямо сейчас (под ником %s) ! 
BOT_SEEN_ON
	%s последний раз был %s назад.
BOT_SEEN_NEVER
	Я никогда не видел %s на канале.
BOT_SEEN_UNKNOWN
	Я не знаю кто такой %s.
	
# BOT responses
BOT_BOT_SYNTAX
	BOT ADD ник идент хост реальное имя
	BOT CHANGE старый ник новый ник [идент [хост [реальное имя]]]
	BOT DEL ник
	BOT LIST
BOT_BOT_ALREADY_EXISTS
	Бот %s уже существует.
BOT_BOT_CREATION_FAILED
	Извините, ошибка создания бота.
BOT_BOT_READONLY
	Извините, изменение натроек бота временно невозвожно.
BOT_BOT_ADDED
	%s!%s@%s (%s) добавлен в список ботов.
BOT_BOT_ANY_CHANGES
	Старая информация эквивалентна новой.
BOT_BOT_CHANGED
	Бот %s был изменен на %s!%s@%s (%s)
BOT_BOT_DELETED
	Бот %s был удален.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Список ботов:
BOT_BOTLIST_PRIVATE_HEADER
	Боты, зарезервированные для IRC Операторов:
BOT_BOTLIST_FOOTER
	%d ботов доступно.
BOT_BOTLIST_EMPTY
	Сейчас нет доступных ботов.
	Попросите Администратора сервисов создать хотя бы одного!
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN chan nick
BOT_ASSIGN_READONLY
	Извините, назначение ботов на канал временно недоступно.
BOT_ASSIGN_ALREADY
	Бот %s уже назначен на канал %s.
BOT_ASSIGN_ASSIGNED
	Бот %s был назначен на %s.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN #канал
BOT_UNASSIGN_UNASSIGNED
	Боты с канала %s удалены.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {#канал | ник}
BOT_INFO_NOT_FOUND
	%s не является ботом или зарегистрированным каналом.
BOT_INFO_BOT_HEADER
	Информация о боте %s:
BOT_INFO_BOT_MASK
	       Маска    : %s@%s
BOT_INFO_BOT_REALNAME
	Реальное имя    : %s
BOT_INFO_BOT_CREATED
	      Создан    : %s
BOT_INFO_BOT_USAGE
	Используется на : %d channel(s)
BOT_INFO_BOT_OPTIONS
	    Опции       : %s
BOT_INFO_OPT_PRIVATE
	Частный
BOT_INFO_CHAN_HEADER
	Информация о канале %s:
BOT_INFO_CHAN_BOT
	            Ник бота : %s
BOT_INFO_CHAN_BOT_NONE
	            Ник бота : бот не назначен.
BOT_INFO_CHAN_KICK_BADWORDS
	  Цензор плохих слов : %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	  Цензор плохих слов : %s (%d киков до бана)
BOT_INFO_CHAN_KICK_BOLDS
	  Цензор жирных букв : %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	  Цензор жирных букв : %s (%d киков до бана)
BOT_INFO_CHAN_KICK_CAPS_ON
	     Цензор CapsLOCK : %s (минимум %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	     Цензор CapsLOCK : %s (%d киков до бана; минимум %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	     Цензор CapsLOCK : %s
BOT_INFO_CHAN_KICK_COLORS
	       Цензор цветов : %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	       Цензор цветов : %s (%d киков до бана)
BOT_INFO_CHAN_KICK_FLOOD_ON
	        Цензор флуда : %s (%d линий за %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	        Цензор флуда : %s (%d киков до бана; %d линий из %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	        Цензор флуда : %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	     Цензор повторов : %s (%d раз)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	     Цензор повторов : %s (%d киков до бана; %d раз)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	     Цензор повторов : %s
BOT_INFO_CHAN_KICK_REVERSES
	     Цензор инверсий : %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	     Цензор инверсий : %s (%d киков до бана)
BOT_INFO_CHAN_KICK_UNDERLINES
	Цензор подчеркиваний : %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	Цензор подчеркиваний : %s (%d киков до бана)
BOT_INFO_ACTIVE
	включен
BOT_INFO_INACTIVE
	выключен
BOT_INFO_CHAN_OPTIONS
	               Опции : %s
BOT_INFO_OPT_DONTKICKOPS
	Защита операторов
BOT_INFO_OPT_DONTKICKVOICES
	Защита войсов
BOT_INFO_OPT_FANTASY
	Фантазия
BOT_INFO_OPT_GREET
	Приветствие
BOT_INFO_OPT_NOBOT
	Без-бота
BOT_INFO_OPT_SYMBIOSIS
	Симбиоз
BOT_INFO_OPT_NONE
	Нет
	
# SET responses
BOT_SET_SYNTAX
	SET #канал опция параметр
BOT_SET_DISABLED
	Извините, установка опций бота временно недоступна.
BOT_SET_UNKNOWN
	Неизвестная опция %s.
	Напишите /msg %S HELP SET для более полной информации.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET #канал DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Бот не будет кикать операторов канала %s.
BOT_SET_DONTKICKOPS_OFF
	Бот будет кикать операторов канала %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET #канал DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Бот не будет кикать войсов канала %s.
BOT_SET_DONTKICKVOICES_OFF
	Бот будет кикать войсов канала %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET #канал FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Режим Фантазии ВКЛЮЧЕНЫ на канале %s.
BOT_SET_FANTASY_OFF
	Режим Фантазии ВЫКЛЮЧЕН на канале %s.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET #канал GREET {ON|OFF}
BOT_SET_GREET_ON
	Режим приветствий on channel %s ВКЛЮЧЕН.
BOT_SET_GREET_OFF
	Режим приветствий on channel %s ВЫКЛЮЧЕН.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET #канал NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Режим Без-бота ВКЛЮЧЕН на канале %s.
BOT_SET_NOBOT_OFF
	Режим Без-бота ВЫКЛЮЧЕН на канале %s.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET #канал PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Частный режим на канале %s is now ВКЛЮЧЕН.
BOT_SET_PRIVATE_OFF
	Частный режим на канале %s is now ВЫКЛЮЧЕН.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET #канал SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Режим Симбиоза ВКЛЮЧЕН на канале %s.
BOT_SET_SYMBIOSIS_OFF
	Режим Симбиоза ВЫКЛЮЧЕН на канале %s.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK #канал опция {ON|OFF} [параметры]
BOT_KICK_DISABLED
	Извините, конфигурация киков временно невозможна.
BOT_KICK_UNKNOWN
	Неизвестная опция %s.
	Напишите /msg %S HELP KICK для получения более полной информации.
BOT_KICK_BAD_TTB
	%s не может быть забанен сейчас.

BOT_KICK_BADWORDS_ON
	Бот будет кикать за плохие слова. Используйте команду BADWORDS
	чтобы добавить или удалить плохое слово.
BOT_KICK_BADWORDS_ON_BAN
	Теперь бот будет кикать за плохие слова, и банить через
	%d киков данного пользователя. Используйте команду BADWORDS
	чтобы добавить или удалить плохое слово.
BOT_KICK_BADWORDS_OFF
	Бот не будет кикать за плохие слова.
BOT_KICK_BOLDS_ON
	Бот будет кикать за использование жирного шрифта.
BOT_KICK_BOLDS_ON_BAN
	Бот будет кикать за использование жирного шрифта, и будет банить через
	%d киков данного пользователя.
BOT_KICK_BOLDS_OFF
	Бот не будет кикать за использование жирного шрифта.
BOT_KICK_CAPS_ON
	Бот будет кикать за использование CapsLOCK (фраза должна содержать
	%d больших символов или они должны составлять не менее %d%% от всего сообщения).
BOT_KICK_CAPS_ON_BAN
	Бот будет кикать за использование CapsLOCK фраза должна содержать
	%d больших символов или они должны составлять не менее %d%% от всего сообщения), и банить
	после %d киков данного пользователя.
BOT_KICK_CAPS_OFF
	Бот не будет кикать за использование CapsLOCK.
BOT_KICK_COLORS_ON
	Бот будет кикать за использование выделения цветом
BOT_KICK_COLORS_ON_BAN
	Бот будет кикать за использование выделения цветом, и банить после %d 
	киков данного пользователя.
BOT_KICK_COLORS_OFF
	Бот не будет кикать за использование выделения цветом
BOT_KICK_FLOOD_ON
	Бот будет кикать за флуд (%d линий за %d секунд).
BOT_KICK_FLOOD_ON_BAN
	Бот будет кикать за флуд (%d линий за %d секунд), и банить 
	после %d киков данного пользователя.
BOT_KICK_FLOOD_OFF
	Бот не будет кикать за флуд.
BOT_KICK_REPEAT_ON
	Бот будет кикать за повторы (пользователь должен повторить фразу
	%d раз подряд).
BOT_KICK_REPEAT_ON_BAN
	Бот будет кикать за повторы (пользователь должен повторить фразу
	%d раз подряд), и банить после %d 
	киков данного пользователя.
BOT_KICK_REPEAT_OFF
	Бот не будет кикать за повторы.
BOT_KICK_REVERSES_ON
	Бот будет кикать за инверсии.
BOT_KICK_REVERSES_ON_BAN
	Бот будет кикать за инверсии, и банить через %d 
	киков данного пользователя.
BOT_KICK_REVERSES_OFF
	Бот не будет кикать за инверсии.
BOT_KICK_UNDERLINES_ON
	Бот будет кикать за использование подчеркивания.
BOT_KICK_UNDERLINES_ON_BAN
	Бот будет кикать за использование подчеркивания., и банить после %d 
	киков данного пользователя.
BOT_KICK_UNDERLINES_OFF
	Бот не будет кикать за использование подчеркивания.
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS #канал {ADD|DEL|LIST|CLEAR} [слово | номер записи] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	Извините, изменение базы данных по плохим словам временно невозможно.
BOT_BADWORDS_REACHED_LIMIT
	Извините, база данных плохих слов может содержать только %d записей на канал.
BOT_BADWORDS_ALREADY_EXISTS
	%s уже есть в списке плохих слов %s.
BOT_BADWORDS_ADDED
	%s добавлено в список плохих слов %s.
BOT_BADWORDS_NO_SUCH_ENTRY
	Нет такой записи (#%d) в списке плохих слов %s.
BOT_BADWORDS_NOT_FOUND
	%s не найдено в списке плохих слов %s.
BOT_BADWORDS_NO_MATCH
	Нет таких значений в списке плохих слов %s.
BOT_BADWORDS_DELETED
	%s удалено из списка плохих слов %s.
BOT_BADWORDS_DELETED_ONE
	Удалена 1 запись из списка плохих слов %s.
BOT_BADWORDS_DELETED_SEVERAL
	Удалено %d записей из списка плохих слов %s.
BOT_BADWORDS_LIST_EMPTY
	Список плохих слов %s пуст.
BOT_BADWORDS_LIST_HEADER
	Список плохих слов %s:
	 Номер   Слово                           Тип
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	Теперь список плохих слов пуст.

# SAY responses
BOT_SAY_SYNTAX
	SAY #канал текст

# ACT responses
BOT_ACT_SYNTAX
	ACT #канал текст

# Errors
BOT_EXCEPT
	Пользователь соответствует исключению канала.
BOT_BAD_NICK
	Ник бота должен содержать только допустимые символы.
BOT_BAD_HOST
	Хост бота должен содержать только допустимые символы для хоста.
BOT_BAD_IDENT
	Идент бота должен содержать только допустимые символы для идента.

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Сервисы не могут изменять режимы. Вы уверены, что Ваши серверы сконфигурированы правильно?
OPER_BOUNCY_MODES_U_LINE
	Сервисы не могут изменять режимы. Вы уверены, что U:lines сконфигурированы правильно?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL сообщение

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Неизвестная опция STATS %s.
OPER_STATS_CURRENT_USERS
	Пользователи: %d (%d Операторов)
OPER_STATS_MAX_USERS
	Максимум пользователей: %d (%s)
# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Сервисы работают %d дней, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Сервисы работают %d день, %02d:%02d
OPER_STATS_UPTIME_HMS
	Сервисы работают %d часов, %d минут
OPER_STATS_UPTIME_HM1S
	Сервисы работают %d часов, %d минут
OPER_STATS_UPTIME_H1MS
	Сервисы работают %d час, %d минут
OPER_STATS_UPTIME_H1M1S
	Сервисы работают %d часов, %d минуту
OPER_STATS_UPTIME_1HMS
	Сервисы работают %d час, %d минут
OPER_STATS_UPTIME_1HM1S
	Сервисы работают %d час, %d минут
OPER_STATS_UPTIME_1H1MS
	Сервисы работают %d час, %d минуту
OPER_STATS_UPTIME_1H1M1S
	Сервисы работают %d час, %d минуту
OPER_STATS_UPTIME_MS
	Сервисы работают %d минут, %d секунд
OPER_STATS_UPTIME_M1S
	Сервисы работают %d минут, %d секунду
OPER_STATS_UPTIME_1MS
	Сервисы работают %d минуту, %d секунд
OPER_STATS_UPTIME_1M1S
	Сервисы работают %d минуту, %d секунду
OPER_STATS_BYTES_READ
	Байт прочитано                        : %5d kB
OPER_STATS_BYTES_WRITTEN
	Байт записано                         : %5d kB
OPER_STATS_USER_MEM
	Пользователи                          : %6d записей, %5d kB
OPER_STATS_CHANNEL_MEM
	Каналы                                : %6d записей, %5d kB
OPER_STATS_GROUPS_MEM
	Групп ников                           : %6d записей, %5d kB
OPER_STATS_ALIASES_MEM
	Алиасов ников                         : %6d записей, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ                              : %6d записей, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ                               : %6d записей, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ                              : %6d записей, %5d kB
OPER_STATS_SESSIONS_MEM
	Сессии                                : %6d записей, %5d kB
OPER_STATS_PROXY_MEM
	Прокси                                : %6d записей, %5d kB
OPER_STATS_AKILL_COUNT
	Количество AKILL'ов                   : %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Время истечения AKILL'а по умолчанию  : %d дней
OPER_STATS_AKILL_EXPIRE_DAY
	Время истечения AKILL'а по умолчанию  : 1 день
OPER_STATS_AKILL_EXPIRE_HOURS
	Время истечения AKILL'а по умолчанию  : %d часов
OPER_STATS_AKILL_EXPIRE_HOUR
	Время истечения AKILL'а по умолчанию  : 1 час
OPER_STATS_AKILL_EXPIRE_MINS
	Время истечения AKILL'а по умолчанию  : %d минут
OPER_STATS_AKILL_EXPIRE_MIN
	Время истечения AKILL'а по умолчанию  : 1 минута
OPER_STATS_AKILL_EXPIRE_NONE
	Время истечения AKILL'а по умолчанию  : Не истекает
OPER_STATS_SGLINE_COUNT
	Количество SGLINEs                    : %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Время истечения SGLINE'а по умолчанию : %d дней
OPER_STATS_SGLINE_EXPIRE_DAY
	Время истечения SGLINE'а по умолчанию : 1 день
OPER_STATS_SGLINE_EXPIRE_HOURS
	Время истечения SGLINE'а по умолчанию : %d часов
OPER_STATS_SGLINE_EXPIRE_HOUR
	Время истечения SGLINE'а по умолчанию : 1 час
OPER_STATS_SGLINE_EXPIRE_MINS
	Время истечения SGLINE'а по умолчанию : %d минут
OPER_STATS_SGLINE_EXPIRE_MIN
	Время истечения SGLINE'а по умолчанию : 1 минута
OPER_STATS_SGLINE_EXPIRE_NONE
	Время истечения SGLINE'а по умолчанию : Не истекает
OPER_STATS_SQLINE_COUNT
	Количество SQLINE'ов                  : %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Время истечения SQGLINE'а по умолчанию: %d дней
OPER_STATS_SQLINE_EXPIRE_DAY
	Время истечения SQGLINE'а по умолчанию: 1 день
OPER_STATS_SQLINE_EXPIRE_HOURS
	Время истечения SQGLINE'а по умолчанию: %d часов
OPER_STATS_SQLINE_EXPIRE_HOUR
	Время истечения SQGLINE'а по умолчанию: 1 час
OPER_STATS_SQLINE_EXPIRE_MINS
	Время истечения SQGLINE'а по умолчанию: %d минут
OPER_STATS_SQLINE_EXPIRE_MIN
	Время истечения SQGLINE'а по умолчанию: 1 минута
OPER_STATS_SQLINE_EXPIRE_NONE
	Время истечения SQGLINE'а по умолчанию: Не истекает
OPER_STATS_SZLINE_COUNT
	Количество SZLINE'ов                  : %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Время истечения SZLINE'а по умолчанию : %d дней
OPER_STATS_SZLINE_EXPIRE_DAY
	Время истечения SZLINE'а по умолчанию : 1 день
OPER_STATS_SZLINE_EXPIRE_HOURS
	Время истечения SZLINE'а по умолчанию : %d часов
OPER_STATS_SZLINE_EXPIRE_HOUR
	Время истечения SZLINE'а по умолчанию : 1 час
OPER_STATS_SZLINE_EXPIRE_MINS
	Время истечения SZLINE'а по умолчанию : %d минут
OPER_STATS_SZLINE_EXPIRE_MIN
	Время истечения SZLINE'а по умолчанию : 1 минута
OPER_STATS_SZLINE_EXPIRE_NONE
	Время истечения SZLINE'а по умолчанию : Не истекает
OPER_STATS_RESET
	Статистика сброшена.

# MODE responses
OPER_MODE_SYNTAX
	MODE #канал режимы

# UMODE respondes
OPER_UMODE_SYNTAX
	UMODE ник режимы
OPER_UMODE_SUCCESS
	Режимы изменены %s.
OPER_UMODE_CHANGED
	%s изменил Ваши режимы пользователя.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE ник флаги
OPER_OLINE_SUCCESS
	Оперфлаги %s были добавлены для %s.
OPER_OLINE_IRCOP
	Теперь Вы IRC Оператор.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES #канал [ALL]
OPER_CLEARMODES_DONE
	Двоичные режимы и баны очищены на канале %s.
OPER_CLEARMODES_ALL_DONE
	Все режимы очищены на канале %s.


# KICK responses
OPER_KICK_SYNTAX
	KICK #канал пользователь причина

# SVSNICK responses
OPER_SVSNICK_SYNTAX
	SVSNICK ник новый ник 
OPER_SVSNICK_NEWNICK
	Ник %s был изменен на %s.
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [ник | номер записи]
OPER_ADMIN_SKELETON
	Сервисы работают в режиме skeleton; команда ADMIN недоступна.
OPER_ADMIN_EXISTS
	%s уже есть в списках Администраторов сервисов.
OPER_ADMIN_REACHED_LIMIT
	Извините, в Вас может быть только %d Администраторов сервисов.
OPER_ADMIN_ADDED
	%s добавлен в список Администраторов сервисов.
OPER_ADMIN_NOT_FOUND
	%s не найден в списке Администраторов сервисов.
OPER_ADMIN_NO_MATCH
	Нет таких значений в списке Администраторов сервисок.
OPER_ADMIN_DELETED
	%s удален из списка Администраторов сервисов.
OPER_ADMIN_DELETED_ONE
	Удалена 1 запись из списка Администраторов сервисов.
OPER_ADMIN_DELETED_SEVERAL
	Удалено %d записей из списка Администраторов сервисов.
OPER_ADMIN_LIST_EMPTY
	Список Администраторов сервисов пуст.
OPER_ADMIN_LIST_HEADER
	Список Администраторов сервисов:
	 Номер   Ник
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	Список Администраторов сервисов очищен.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [ник | номер записи]
OPER_OPER_SKELETON
	Сервисы работают в режиме skeleton; команда OPER недоступна.
OPER_OPER_EXISTS
	%s уже есть в списках Операторов сервисов.
OPER_OPER_REACHED_LIMIT
	Извините, в Вас может быть только %d Операторов сервисов.
OPER_OPER_ADDED
	%s добавлен в список Операторов сервисов.
OPER_OPER_NOT_FOUND
	%s не найден в списке Операторов сервисов.
OPER_OPER_NO_MATCH
	Нет таких значений в списке Операторов сервисок.
OPER_OPER_DELETED
	%s удален из списка Операторов сервисов.
OPER_OPER_DELETED_ONE
	Удалена 1 запись из списка Операторов сервисов.
OPER_OPER_DELETED_SEVERAL
	Удалено %d записей из списка Операторов сервисов.
OPER_OPER_LIST_EMPTY
	Список Операторов сервисов пуст.
OPER_OPER_LIST_HEADER
	Список Операторов сервисов:
	 Номер   Ник
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	Список Операторов сервисов очищен.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+время истечения] {маска | номер записи} [причина]]
OPER_AKILL_EXISTS
	%s уже есть в списке AKILL'ов.
OPER_AKILL_ALREADY_COVERED
	%s уже прикрыт %s.
OPER_AKILL_REACHED_LIMIT
	Извините, Вы можете иметь только %d AKILL'ов.
OPER_AKILL_NO_NICK
	Помните: маска AKILL'а не может содержать ники; удостоверьтесь, что ник не не включен в указанную Вами маску.
OPER_AKILL_ADDED
	%s добавлен в список AKILL'ов.
OPER_AKILL_CHANGED
	Время истечения %s изменено.
OPER_AKILL_NOT_FOUND
	%s не найден в списке AKILL'ов.
OPER_AKILL_NO_MATCH
	Нет таких записей в списке AKILL'ов.
OPER_AKILL_DELETED
	%s удален из списка AKILL'ов.
OPER_AKILL_DELETED_ONE
	Удалена 1 запись из спика AKILL'ов.
OPER_AKILL_DELETED_SEVERAL
	Удалено %d записей из списка AKILL'ов.
OPER_AKILL_LIST_EMPTY
	Список AKILL'ов пуст.
OPER_AKILL_LIST_HEADER
	Текущий список AKILL'ов:
	 Номер   Маска                             Причина
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Текущий список AKILL'ов:
# номер, маска, кем поставлен, когда поставлен, когда истекает, причина
OPER_AKILL_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_AKILL_CLEAR
	Список AKILL'ов очищен.
	
# SGLINE responses
OPER_CHANKILL_SYNTAX
	CHANAKILL [+expiry] {#channel} [reason]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+время истечения] {маска | номер записи}[:причина]]
OPER_SGLINE_UNSUPPORTED
	Извините, использование SGLINE недоступно в этой сети.
OPER_SGLINE_EXISTS
	%s уже есть в списке SGLINE'ов.
OPER_SGLINE_ALREADY_COVERED
	%s уже прикрыт %s.
OPER_SGLINE_REACHED_LIMIT
	Извините, Вы можете иметь только %d SGLINE'ов.
OPER_SGLINE_ADDED
	%s добавлен в список SGLINE'ов.
OPER_SGLINE_CHANGED
	Время истечения %s изменено.
OPER_SGLINE_NOT_FOUND
	%s не найден в списке SGLINE'ов.
OPER_SGLINE_NO_MATCH
	Нет таких записей в списке SGLINE'ов.
OPER_SGLINE_DELETED
	%s удален из списка SGLINE'ов.
OPER_SGLINE_DELETED_ONE
	Удалена 1 запись из спика SGLINE'ов.
OPER_SGLINE_DELETED_SEVERAL
	Удалено %d записей из списка SGLINE'ов.
OPER_SGLINE_LIST_EMPTY
	Список SGLINE'ов пуст.
OPER_SGLINE_LIST_HEADER
	Текущий список SGLINE'ов:
	 Номер   Маска                             Причина
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Текущий список SGLINE'ов:
# номер, маска, кем поставлен, когда поставлен, когда истекает, причина
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SGLINE_CLEAR
	Список SGLINE'ов очищен.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+время истечения] {маска | номер записи} [причина]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	Извините, использование SQLINE недоступно в этой сети.
OPER_SQLINE_EXISTS
	%s уже есть в списке SQLINE'ов.
OPER_SQLINE_ALREADY_COVERED
	%s уже прикрыт %s.
OPER_SQLINE_REACHED_LIMIT
	Извините, Вы можете иметь только %d SQLINE'ов.
OPER_SQLINE_ADDED
	%s добавлен в список SQLINE'ов.
OPER_SQLINE_CHANGED
	Время истечения %s изменено.
OPER_SQLINE_NOT_FOUND
	%s не найден в списке SQLINE'ов.
OPER_SQLINE_NO_MATCH
	Нет таких записей в списке SQLINE'ов.
OPER_SQLINE_DELETED
	%s удален из списка SQLINE'ов.
OPER_SQLINE_DELETED_ONE
	Удалена 1 запись из спика SQLINE'ов.
OPER_SQLINE_DELETED_SEVERAL
	Удалено %d записей из списка SQLINE'ов.
OPER_SQLINE_LIST_EMPTY
	Список SQLINE'ов пуст.
OPER_SQLINE_LIST_HEADER
	Текущий список SQLINE'ов:
	 Номер   Маска                             Причина
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Текущий список SQLINE'ов:
# номер, маска, кем поставлен, когда поставлен, когда истекает, причина
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SQLINE_CLEAR
	Список SQLINE'ов очищен.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+время истечения] {маска | номер записи} [причина]]
OPER_SZLINE_UNSUPPORTED
	Извините, использование SZLINE недоступно в этой сети.
OPER_SZLINE_EXISTS
	%s уже есть в списке SZLINE'ов.
OPER_SZLINE_ALREADY_COVERED
	%s уже прикрыт %s.
OPER_SZLINE_REACHED_LIMIT
	Извините, Вы можете иметь только %d SZLINE'ов.
OPER_SZLINE_ONLY_IPS
	Помните: Вы можете добавлять только IP маски в список SZLINE'ов.
OPER_SZLINE_ADDED
	%s добавлен в список SZLINE'ов.
OPER_SZLINE_CHANGED
	Время истечения %s изменено.
OPER_SZLINE_NOT_FOUND
	%s не найден в списке SZLINE'ов.
OPER_SZLINE_NO_MATCH
	Нет таких записей в списке SZLINE'ов.
OPER_SZLINE_DELETED
	%s удален из списка SZLINE'ов.
OPER_SZLINE_DELETED_ONE
	Удалена 1 запись из спика SZLINE'ов.
OPER_SZLINE_DELETED_SEVERAL
	Удалено %d записей из списка SZLINE'ов.
OPER_SZLINE_LIST_EMPTY
	Список SZLINE'ов пуст.
OPER_SZLINE_LIST_HEADER
	Текущий список SZLINE'ов:
	 Номер   Маска                             Причина
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Текущий список SZLINE'ов:
# номер, маска, кем поставлен, когда поставлен, когда истекает, причина
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SZLINE_CLEAR
	Список SZLINE'ов очищен.

# SET responses
OPER_SET_SYNTAX
	SET опция параметры
OPER_SET_IGNORE_ON
	Код игнорирования будет использован.
OPER_SET_IGNORE_OFF
	Код игнорирования не будет использован.
OPER_SET_IGNORE_ERROR
	Опция IGNORE должна иметь значения только ON или OFF.
OPER_SET_READONLY_ON
	Сервисы сейчас в режиме только чтение.
OPER_SET_READONLY_OFF
	Сервисы сейчас в режиме чтения-записи.
OPER_SET_READONLY_ERROR
	Опция READONLY должна иметь значения только ON или OFF.
OPER_SET_LOGCHAN_ON
	Теперь сервисы будут вести лог в канал %s.
OPER_SET_LOGCHAN_OFF
	Сервисы больше не будут вести лог в канал.
OPER_SET_LOGCHAN_ERROR
	Опция LOGCHAN должна иметь значения только ON или OFF и должен быть определен параметр LogChannel в конфигурации сервисов.
OPER_SET_DEBUG_ON
	Сервисы находятся в режиме отладки.
OPER_SET_DEBUG_OFF
	Сервисы больше не находятся в режиме отладки.
OPER_SET_DEBUG_LEVEL
	Сервисы находятся в режиме отладки (уровень %d).
OPER_SET_DEBUG_ERROR
	Опция DEBUG должна иметь значения только ON, OFF, или полозительное число.
OPER_SET_NOEXPIRE_ON
	Сервисы находятся в режиме не истечения.
OPER_SET_NOEXPIRE_OFF
	Сервисы теперь находятся в режиме истечения.
OPER_SET_NOEXPIRE_ERROR
	Опция NOEXPIRE должна иметь значения только ON или OFF.
OPER_SET_UNKNOWN_OPTION
	Неизвестная опция %s.

# NOOP responses
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} сервер
OPER_NOOP_SET
	Все O:lines на %s удалены.
OPER_NOOP_REVOKE
	Все O:lines на %s восстановлены.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE имя сервера [причина]
OPER_JUPE_HOST_ERROR
	Пожалуйста используйте действительное имя сервера при джупинге.

# RAW responses
OPER_RAW_SYNTAX
	RAW текст

# UPDATE responses
OPER_UPDATING
	Обновление баз данных.
	
# RELOAD responses
OPER_RELOAD
	Конфигурационный файл сервисов был считан заново.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN не определен; перезапуск невозможен. Запустите скрипт \2configure\2  откомпилируйте сервисы, чтобы включить команду RESTART.

# IGNORE respondes
OPER_IGNORE_SYNTAX
	Синтакстс: IGNORE {ADD|DEL|LIST|CLEAR} [время] [ник]
OPER_IGNORE_VALID_TIME
	Вы должны правильно ввести время.
OPER_IGNORE_TIME_DONE
	Ник %s теперь будет игнорироваться в течение %s.
OPER_IGNORE_PERM_DONE
	Ник %s будет постоянно игнорироваться.
OPER_IGNORE_DEL_DONE
	Ник %s больше не будет игнорироваться.
OPER_IGNORE_LIST
	Список игнорируемых ников:
OPER_IGNORE_LIST_NOMATCH
	Ник %s не найден в списке игнорируемых.
OPER_IGNORE_LIST_EMPTY
	Список игнорируемых ников пуст.
OPER_IGNORE_LIST_CLEARED
	Список игнорируемых ников очищен.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES ник
OPER_KILLCLONES_UNKNOWN_NICK
	Не найден пользователь %s.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Список каналов:
	Имя           Пользователи Режимы  Топик
OPER_CHANLIST_HEADER_USER
	%s список каналов:
	Имя           Пользователи Режимы  Топик
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	Конец списка каналов.
	
# USERLIST responses
OPER_USERLIST_HEADER
	Список пользователей:
	Ник                  Маска
OPER_USERLIST_HEADER_CHAN
	%s список пользователей:
	Ник                  Маска
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Конец списка пользователей.

# CACHE responses
OPER_CACHE_SYNTAX
	CACHE {DEL | LIST} {имя хоста | символьная маска} [QUEUED | ALL]
OPER_CACHE_DISABLED
	Детектор прокси отключен.
OPER_CACHE_NOT_FOUND
	%s не найден в кэше.
OPER_CACHE_REMOVED
	%s удален из кэша.
OPER_CACHE_HEADER
	Имя хоста                                         Статус
OPER_CACHE_LIST
	%-48s %s
OPER_CACHE_FOOTER
	Конец списка - %d/%d записей найдено.
OPER_CACHE_QUEUED
	Очередь
OPER_CACHE_PROGRESS
	Выполняется
OPER_CACHE_NORMAL
	Normal
OPER_CACHE_WINGATE
	Wingate
OPER_CACHE_SOCKS4
	SOCKS 4
OPER_CACHE_SOCKS5
	SOCKS 5
OPER_CACHE_HTTP
	HTTP прокси
OPER_SUPER_ADMIN_ON
	Теперь Вы Супер-Админ
OPER_SUPER_ADMIN_OFF
	Вы больше не Суперадмин
OPER_SUPER_ADMIN_SYNTAX
	Опция SuperAdmin может быть только ON или OFF (должно быть включено в services.conf)
OPER_SUPER_ADMIN_WALL_ON
	%s теперь Супер-Админ
OPER_SUPER_ADMIN_WALL_OFF
	%s больше не Супер-Админ
OPER_SUPER_ADMIN_ONLY
	Только Супер-Админы могут использовать эту команду.
OPER_STAFF_LIST_HEADER
	On Level Nick

# DefCon Messages

OPER_STAFF_FORMAT
	 %c %s  %s
OPER_STAFF_AFORMAT
	 %c %s  %s [%s]

# DefCon Messages

OPER_DEFCON_SYNTAX
	Синтаксис: DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	Сервисы теперь работают в режиме Defcon, Пожалуйста, попытайтесь позже.
OPER_DEFCON_NO_CONF
	Режим Defcon должен быть включен в файле services.conf
OPER_DEFCON_CHANGED
	Сейчас сервисы в режиме DEFCON %d
OPER_DEFCON_WALL
	%s Изменил уровень DEFCON на %d
DEFCON_GLOBAL
	Уровень Defcon теперь равен: %d

# Module strings

OPER_MODULE_LOADED
	Модуль %s загружен
OPER_MODULE_UNLOADED
	Модель %s выгружен
OPER_MODULE_LOAD_FAIL
	Невозможно загрузить модуль %s
OPER_MODULE_REMOVE_FAIL
	Невозможно выгрузить модуль %s
OPER_MODULE_LOAD_SYNTAX
	MODLOAD ИмяФайла
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD ИмяФайла
OPER_MODULE_LIST_HEADER
	Current Module list:
OPER_MODULE_LIST
	Модуль: %s [%s]
OPER_MODULE_LIST_FOOTER
	%d Modules loaded.
OPER_MODULE_INFO_LIST
	Модуль: %s Версия: %s Автор: %s загружен: %s
OPER_MODULE_CMD_LIST
	Допустимые команды: /msg %s %s
OPER_MODULE_MSG_LIST
	Допустимы IRCD заголовки для: %s
OPER_MODULE_NO_LIST
	Не загружено ни одного модуля
OPER_MODULE_NO_INFO
	Нет доступной информации о модуле %s
OPER_MODULE_INFO_SYNTAX
	MODINFO ИмяФайла
MODULE_HELP_HEADER
	Нижеследующие команды поддерживаются данным модулем:

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [параметры]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+время истечения] маска лимит причина
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {маска | list}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE номер позиция
OPER_EXCEPTION_DISABLED
	Ограничение сессий отключено.
OPER_EXCEPTION_ALREADY_PRESENT
	Маска %s уже есть в списке исключений.
OPER_EXCEPTION_TOO_MANY
	Список исключений из лимита сессий переполнен!
OPER_EXCEPTION_ADDED
	Лимит сессий для %s установлен в %d.
OPER_EXCEPTION_MOVED
	Исключение для %s (#%d) перемещено в позицию %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Нет такой записи (#%d) в списке исключений из лимитв сессий.
OPER_EXCEPTION_NOT_FOUND
	%s не найден в списке исключений из лимит сессий.
OPER_EXCEPTION_NO_MATCH
	Нет таких значений в списке исключений из лимита сессий.
OPER_EXCEPTION_DELETED
	%s удален из списка исключений из лимита сессий.
OPER_EXCEPTION_DELETED_ONE
	Удалена 1 запись из списка исключений из лимита сессий.
OPER_EXCEPTION_DELETED_SEVERAL
	Удалено %d записей из списка исключений из лимита сессий.
OPER_EXCEPTION_LIST_HEADER
	Текущий список исключений из лимита сессий:
# маска, лимит
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Номер  Лимит  Хост
# меска, кем установлено, когда установлено, когда истекает, лимит, причина
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (by %s on %s; %s)
	    Лимит: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Некорректный лимит сессий. Он должен быть целым числом больше или равным нуля и меньше %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Некорректная хостмаска. Только реальная хостмаска может быть добавлена в исключения, она не должна содержать ников или имен пользователей.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST лимит | VIEW хост}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST лимит
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW хост
OPER_SESSION_DISABLED
	Ограничение сессий отключено.
OPER_SESSION_INVALID_THRESHOLD
	Неверное пороговое значение. Оно должно быть целым числом больше 1.
OPER_SESSION_NOT_FOUND
	%s не найден в списке сессий.
OPER_SESSION_LIST_HEADER
	Хосты с меньшим чем %d количеством сессий:
OPER_SESSION_LIST_COLHEAD
	Сессия  Хост
# лимит, хост
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	Хост %s сейчас использует %d сессий из %d максимально допустимых.

# EXCEPTION help

OPER_HELP_EXCEPTION
	Синтаксис: EXCEPTION ADD [+время истечения] маска лимит причина
	           EXCEPTION DEL {маска | list}
	           EXCEPTION MOVE номер позиция
	           EXCEPTION LIST [маска | list]
	           EXCEPTION VIEW [маска | list]
	
	Позволяет Администраторам сервисов управлять списком хостов,
	имеющих отдельные ограничения на количество сессий - разрешит определеным машинам, 
	например шелл-серверам, иметь бОльшее количество клиентов одновременно, чем
	разрешено по умолчанию. Как только количество сессий с хоста достигнет лимита,
	все последующие клиенты, пытающиеся коннектиться с данного хоста будут
	прибиваться. Перед отсоединением пользователя, он будет уведомлен
	/NOTICE от %S, что с данного хоста превышен лимит
	сессий. Содержание этого уведомления настраивается в конфигурации.
	
	EXCEPTION ADD добавляет указанную маску в список исключений.
	Примечание: маски ник!идент@хост и идент@хост некорректны!
	Только реальные хостмаски, например box.host.dom и *.host.dom, 
	разрешено использовать, так как ограничение сессий не использует
	ни ники ни иденты. лимит должен быть целым числом больше или
	равным нуля. Это определяет как много сессий данный хост
	использовать одновременно. Установка этого значения равным 0 позволит хосту
	иметь неограниченное число сессий. См. AKILL help для более подробной информации
	о формате параметра время истечения.
	EXCEPTION DEL удалят указанную маску из списка исключений.
	EXCEPTION MOVE переместит исключение под номером номер в позицию позиция.
	Интервал исключений будет сдвинут вверх или вниз, заполняя пустой промежуток.
	EXCEPTION LIST и EXCEPTION VIEW покажет все текущие
	исключения; если указана маска, список исключений будет показан
	только для совпадающих с указанной масок. EXCEPTION VIEW покажет
	более подробную информацию, покажет имя пользователя и того,
	кто добавил данное исключение, ограничение сессий, причину, 
	хостмаску, а также дату и время истечения исключения.
	
	Примечание: коннектящийся клиент будет "использовать" первое в списке исключение,
	содержащее его хост. Большие списки исключений с малоспецифичными масками
	исключений могут снизить производительность сервисов.
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_SESSION
	Синтаксис: SESSION LIST порог
	           SESSION VIEW хост
	
	Позволяет Администраторам сервисов просмотреть список сессий.
	
	SESSION LIST покажет хосты с меньшим, чем порог количеством сессий.
	Порог должен быть целым больше 1. Это предохранит
	от случайной выдачи списка с большим числом единичных
	сессий.
	SESSION VIEW покажет детальную информацию об указанном
	хосте - включая текущий счетчик сессий и лимит сессий.
	host не может включать в себя символьные маски (*, ? и т.п.).
	
	См. EXCEPTION help для более подробной информации о лимитировании
	сессий, как установить отдельный лимит сессий для определенного
	хоста или группы хостов.
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_STAFF
	Синтаксис: STAFF

	Показывает всех, прописанных на сервисах (Операторов, Администраторов..) 
	включая их уровень и онлайн статус.

OPER_HELP_DEFCON
	Синтаксис: DEFCON [1|2|3|4|5]

	Система defcon может быть использована для осуществления различных
	сервисных действий, удобно использовать при попытках
	атаки на сеть.

OPER_HELP_DEFCON_NO_NEW_CHANNELS
	* Нельзя регистрировать каналы
OPER_HELP_DEFCON_NO_NEW_NICKS
	* Нельзя регистрировать ники
OPER_HELP_DEFCON_NO_MLOCK_CHANGE
	* Нельзя изменять MLOCK
OPER_HELP_DEFCON_FORCE_CHAN_MODES
	* Форсированно установить режимы (%s) на все каналы
OPER_HELP_DEFCON_REDUCE_SESSION
	* Уменьшить лимит сессий до %d
OPER_HELP_DEFCON_NO_NEW_CLIENTS
	* Прибивать всех коннектящихся клиентов
OPER_HELP_DEFCON_OPER_ONLY
	* Игнорировать любые сообщения от не-операторов
OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* Молча игнорировать не-операторов
OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* Добавлять AKILL всем коннектящимся клиентам
OPER_HELP_DEFCON_NO_NEW_MEMOS
	* Запрет отправлять мемки
OPER_HELP_CHANKILL
	Синтаксис: CHANKILL [+истечение] #канал причина

	Установить AKILL для каждого ника, зашедшего на определенный канал. Будет
	использован полностью реальный ident@host для каждого ника,
	который подпадет под данный AKILL. 

###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Новость - %s] %s
NEWS_OPER_TEXT
	[Новости для Операторов - %s] %s
NEWS_RANDOM_TEXT
	[Случайная новость - %s] %s

NEWS_LOGON_SYNTAX
	Синтаксис: LOGONNEWS {ADD|DEL|LIST} [текст|номер]
NEWS_LOGON_LIST_HEADER
	Текущие новости:
NEWS_LOGON_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_LOGON_LIST_NONE
	Новостей нет.
NEWS_LOGON_ADD_SYNTAX
	Синтаксис: LOGONNEWS ADD текст
NEWS_LOGON_ADD_FULL
	Список новостей переполнен!
NEWS_LOGON_ADDED
	Добавлена очередная новость (#%d).
NEWS_LOGON_DEL_SYNTAX
	Синтаксис: LOGONNEWS DEL {номер | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Новость под номером #%d не найдена!
NEWS_LOGON_DELETED
	Новость под номером #%d удалена.
NEWS_LOGON_DEL_NONE
	Нет новостей, подлежащих удалению!
NEWS_LOGON_DELETED_ALL
	Все новости удалены.

NEWS_OPER_SYNTAX
	Синтаксис: OPERNEWS {ADD|DEL|LIST} [текст|номер]
NEWS_OPER_LIST_HEADER
	Список новостей для Операторов:
NEWS_OPER_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_OPER_LIST_NONE
	Нет новостей для Операторов.
NEWS_OPER_ADD_SYNTAX
	Синтаксис: OPERNEWS ADD текст
NEWS_OPER_ADD_FULL
	Список новостей переполнен!
NEWS_OPER_ADDED
	Добавлена очередная новость для Операторов (#%d).
NEWS_OPER_DEL_SYNTAX
	Синтаксис: OPERNEWS DEL {номер | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Новость для Операторов под номером #%d не найдена!
NEWS_OPER_DELETED
	Новость для Операторов под номером #%d удалена.
NEWS_OPER_DEL_NONE
	Нет новостей для Операторов, подлежащих удалению!
NEWS_OPER_DELETED_ALL
	Все новости для Операторов удалены.

NEWS_RANDOM_SYNTAX
	Синтаксис: RANDOMNEWS {ADD|DEL|LIST} [текст|номер]
NEWS_RANDOM_LIST_HEADER
	Список случайных новостей:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_RANDOM_LIST_NONE
	Нет случайных новостей.
NEWS_RANDOM_ADD_SYNTAX
	Синтаксис: RANDOMNEWS ADD текст
NEWS_RANDOM_ADD_FULL
	Список новостей переполнен!
NEWS_RANDOM_ADDED
	Добавлена очередная случайная новость (#%d).
NEWS_RANDOM_DEL_SYNTAX
	Синтаксис: RANDOMNEWS DEL {номер | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Случайная новость под номером #%d не найдена!
NEWS_RANDOM_DELETED
	Случайная новость под номером #%d удалена.
NEWS_RANDOM_DEL_NONE
	Нет случайных новостей, подлежащих удалению!
NEWS_RANDOM_DELETED_ALL
	Все случайные новости удалены.

NEWS_HELP_LOGON
	Синтаксис: LOGONNEWS ADD текст
	           LOGONNEWS DEL {номер | ALL}
	           LOGONNEWS LIST
	
	Управление списком новостей. Когда пользователь
	заходит в IRC сеть, эти новости будут отправлены
	ему. (Тем не менее, не более трех сообщений будет
	отправлено, чтобы не допустить флуда. Если у Вас новостей
	больше трех, только три последние новости
	будут посланы.)
	
	Команда LOGONNEWS LIST может быть использована любым IRC Оператором для просмотра
	текущих новостей. Команды ADD и DEL могут использовать только
	Администраторы сервисов.

NEWS_HELP_OPER
	Синтаксис: OPERNEWS ADD текст
	           OPERNEWS DEL {номер | ALL}
	           OPERNEWS LIST
	
	Управление списком новостей для Операторов. Когда пользователь
	станет IRC Оператором (использовав команду /OPER), эти сообщения
	будут ему отправлены. (Тем не менее, не более трех сообщений будет
	отправлено, чтобы не допустить флуда. Если у Вас новостей
	больше трех, только три последние новости
	будут посланы.)
	
	Команда OPERNEWS LIST может быть использована любым IRC Оператором для просмотра
	текущих новостей. Команды ADD и DEL могут использовать только
	Администраторы сервисов.

NEWS_HELP_RANDOM
	Синтаксис: RANDOMNEWS ADD текст
	           RANDOMNEWS DEL {текст | ALL}
	           RANDOMNEWS LIST
	
	Управление списком случайных новостей. Когда пользователь
	заходит в IRC сеть, одна (и только одна) из
	случайных новостей будет выбрана наугад и отправлена ему.
	
	Команда RANDOMNEWS LIST может быть использована любым IRC Оператором для просмотра
	текущих новостей. Команды ADD и DEL могут использовать только
	Администраторы сервисов.

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S сообщит вам информацию о сервисах.
	Начните работу с сервисами с команд:

	/msg %s HELP
	информация о регистрации ников

	/msg %s HELP
	информация о регистрации каналов и управлении ими
	     
	/msg %s HELP
	информация о том, как оставлять сообщения пользователям,
	которые в данный момент в оффлайне.

HELP_HELP_BOT

	/msg %s HELP
	информация о том, как установить сервисвного бота на свой канал

HELP_HELP_HOST

	/msg %s HELP
	     for information on setting up nick vHosts

###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP
	%S это сервис, предназначенный для "регистрации" Вашего ника и его защиты
	от использования кем-либо, кроме Вас. Существует несколько команд для 
	регистрации ника и различных настроек; чтобы использовать 
	данные команды, напишите /msg %S команда.
	Для более подробной информации по какой-либо
	команде напишите: /msg %S HELP команда.
	
	    REGISTER   Регистрация ника
	    GROUP      Объединение ников в группы
	    IDENTIFY   Идентификация Вас как владельца ника
	    ACCESS     Управление списком доступа
	    SET        Установка параметров, включая защиту kill'ом
	    DROP       Отмена регистрации ника
	    RECOVER    Выбросить из чата пользователя, взявшего Ваш ник
	    RELEASE    Восстановление использования Вашего ника после команды RECOVER
	    SENDPASS   Забыли пароль? Попробуйте эту команду
	
	Другие команды: GHOST, ALIST, GLIST, INFO, LIST, LOGOUT, 
	                STATUS
	
	ВАЖНО: Данный сервис предназначен для того, чтобы пользователи IRC  
	могли воспользоваться многими удобными возможностями.
	NickServ НЕ предназначен для облегчения "воровства" ников,
	"киберсквоттинга" (захвата большого количества ников) или других зловредных действий.
	Злоупотребление %S приведет, как минимум, к уничтожению Вашего ника(ников).
	
NICK_HELP_EXPIRES
	
	Неиспользуемые в течение длительного времени ники автоматически
	удаляются из базы данных - ник будет удален, если его не использовать
	в течение %d дней.

NICK_HELP_REGISTER
	Синтаксис: REGISTER пароль [email]
	
	Регистрирует Ваш ник в базе данных %S.  После того, как Вы
	зарегистрировали ник, вы можете использовать команды SET и ACCESS,
	чтобы сконфигурировать параметры Вашего ника по Вашему вкусу.
	Не забывайте пароль, который Вы укажете при регистрации! Не используйте
	пароли, являющиеся Вашим ником или Вашим именем. Помните, что регистр символов
	в пароле - различается.
	(Примечание: пароли чувствительны к регистру!  MYPASSWORD, MyPassword, и mypassword - это РАЗНЫЕ пароли!)
	Принципы выбора пароля:
	
	Пароли не должны быть легко угадываемы.  Например,
	использование Вашего реального имени в качестве пароля - плохая идея.

	Пароль не должен легко угадываться - например являться Вашим ником или паролем,
	и %S не допустит их использовать.  Также, 
	короткие пароли очень легко подбираются простым 
	перебором, поэтому длина пароля должна быть
	больше 5 символов.Пробелы и знаки табуляции в пароле 
	недопустимы. Используйте в качестве пароля только 
	латинские буквы.
	
	Параметр email является необязательным. 
	Но, тем не менее, его требуется указывать
	в большинстве сетей.
	Ваша приватность будет соблюдена; этот e-mail не будет 
	передан третьим лицам.
	
	Эта команда также создает новую группу для Вашего ника,	 использование которой позволит регистрировать другие ники, все из них
	будут являться частью главного ника - в группу которого вы регистрируете другие ники;
	соответственно под любым и них можно будет получить короткое сообщение, будут сохранены все настройки главного ника группы, и у Вас будут все привилегии на каналах, 
	на которых Вы прописаны в списке доступа. Для получения более полной информации
	напишите /msg %S HELP GROUP.
	
NICK_HELP_GROUP
	Синтаксис: GROUP главный_ник пароль
	
	Эта команда позволяет Вам присоединить Ваш ник к группе главного_ника 
	пароль - это пароль главного ника.
	
	Использование групп позволит Вам объединить Ваши настройки,
	получать и отправлять короткие сообщения и привилегии на каналах со всеми никами группы и
	многое другое!
	 
	Группа существует до тех пор, пока используется.
	если Вы удалите (дропнете) один из ников в группе, Вы не потеряете
	регистрацию по крайней мере до тех пор, пока,
	в группе остается хотя бы 1 ник.
	
	Вы можете использовать эту команду, даже если Вы все еще не зарегистрировали
	Ваш ник. Если Ваш ник уже зарегистрирован, Вам необходимо
	проидентифицироваться перед использованием этой команды. Напишите 
	/msg %S HELP IDENTIFY для получения более полной информации. Впрочем, есть вероятность,
	что  в Вашей IRC сети это будет невозможно.
	
	Однако, рекомендуется использовать эту команду с незарегистрированного
	ника, потому что он автоматически зарегистрируется при успешном использовании 
	данной команды. Вы также можете использовать эту команду с зарегистрированного ника (чтобы 
	перейти в другую группу) только если администраторы Вашей IRC сети это
	разрешили.
	
	Вы можно быть только в одной группе одновреиенно. Слияние групп
	невозможно.
	
	Примечание: У всех ников в группе единый пароль.

NICK_HELP_IDENTIFY
	Синтаксис: IDENTIFY пароль
	
	Сообщает %S что Вы действительно являетесь владельцем
	ника.  Многие команды требуют идентификации, без этого
	Вы не сможете их использовать. Пароль должен быть
	полностью идентичен тому, который вы указывали когда 
	использовали команду REGISTER.
	
NICK_HELP_UPDATE
	Синтаксис: UPDATE

	Обновляет Ваш текущий статус, т.е. проверяет наличие новых мемок,
	устанавливает необходимые режимы на каналах (ModeonID) и обновляет ваши
	виртуальные хосты и флаги (а также последнее время посещения IRC, и т.д.).

NICK_HELP_LOGOUT
	Синтаксис: LOGOUT
	
	Эта команда имеет эффект, обратный эффекту команды IDENTIFY, т.е.
	снимает с Вас статус владельца ника. 
	Примечание: тем не менее, данная команда не будет требовать, чтобы вы
	идентифицаировали себя.

NICK_HELP_DROP
	Синтаксис: DROP [nickname]
	
	Удаляет ваш ник из базы данных %S.  Удаленный таким образом ник
	становится свободным для регистрации любым человеком.
	
	Вы можете удалять ник в пределах Вашей группы, при этом если вы удаляете не главный ник,
	то он остается вместе с группой, удаляется только Ваш текущий ник.
	
	Порядок использования данной команды: сначала Вы должны идентифицироваться
	на NickServ (/msg %S HELP IDENTIFY для более полной
	информации).

NICK_HELP_ACCESS
	Синтаксис: ACCESS ADD mask
	           ACCESS DEL mask
	           ACCESS LIST
	
	Изменяет или показывает список масок доступа на Ваш ник. Имеется в виду
	список адресов, которые будут автоматически идентифицированы
	%S как адреса, с которых допустимо использовать Ваш ник. Если
	Вы хотите использовать ник с различных адресов, Вам нобходимо
	использовать команду IDENTIFY чтобы %S
	идентифицировал Вас.
	
	Примеры:
	
	    ACCESS ADD anyone@*.bepeg.com
	    Разрешает доступ пользователю anyone с любого компьютера
	    в домене bepeg.com.
	
	    ACCESS DEL anyone@*.bepeg.com
	    Антоним предыдущей команды.
	
	    ACCESS LIST
	    Покажет текущий список адресов доступа.

NICK_HELP_SET
	Синтаксис: SET опция параметры
	
	Устанавливает различные опции ника.  опция может быть:
	
	    DISPLAY    Установить главный ник группы
	    PASSWORD   Установить или изменить пароль на ник
	    LANGUAGE   Установить язык, на котором сервисы будут
	               посылать Вам сообщения
	    URL        Ассоциировать URL с Вашим ником
	    EMAIL      Ассоциировать E-mail с Вашим ником
	    ICQ        Ассоциировать номер ICQ с Вашим ником
	    GREET      Ассоциировать приветствие с Вашим ником
	    KILL       Включен или выключить и выбрать тип защиты, 
	    SECURE     Включить или выключить режим безопасности ника 
	    PRIVATE    Предохранить ваш ник от появления в 
	               /msg %S LIST
	    HIDE       Скрыть часть информации о Вас
	    MSG        Выбрать метод общения сервисов с Вами - приват или нотисы
	
	Перед установкой любой опции, Вы должны сначала идентифицироваться
	с Вашим паролем (/msg %S HELP IDENTIFY для более подробной
	информации).
	
	Напишите /msg %S HELP SET опция для более подробной информации
	по конкретной опции.

NICK_HELP_SET_DISPLAY
	Синтаксис: SET DISPLAY новый_главный_ник
	
	Изменяет Ваш главный ник в группе.  
	Новый главный ник должен присутствовать в Ваше группе.

NICK_HELP_SET_PASSWORD
	Синтаксис: SET PASSWORD новый_пароль
	
	Изменяет пароль, который вы используете для идентификации
	Вашего ника.

NICK_HELP_SET_LANGUAGE
	Синтаксис: SET LANGUAGE номер
	
	Изменяет язык, на котором сервисы будут посылать Вам
	сообщения (например, при ответе на посланную Вами команду).
	номер должен быть выбран из нижеследующего списка
	поддерживаемых языков:
	

NICK_HELP_SET_URL
	Синтаксис: SET URL url
	
	Ассоциирует URL с Вашим ником.  Этот URL
	будет показан, когда кто-либо запросит информацию
	о Вашем нике с помощью команды INFO.

NICK_HELP_SET_EMAIL
	Синтаксис: SET EMAIL адреc@email
	
	Ассоциирует адрес E-mail с Вашим ником. Этот адрес
	будет показан, когда кто-либо запросит информацию
	о Вашем нике с помощью команды INFO.
	
NICK_HELP_SET_ICQ
	Синтаксис: SET ICQ номер ICQ
	
	Ассоциирует номер ICQ с Вашим ником. Этот номер
	будет показан, когда кто-либо запросит информацию
	о Вашем нике с помощью команды INFO.
	
NICK_HELP_SET_GREET
	Синтаксис: SET GREET сообщение
	
	Создает приветсвие для Вашего ника, которое
	будет показываться при заходе на канал на котором включена опция GREET;
	учтите, что Вам понадобится необходимый уровень  
	доступа на этот канал, чтобы использовать приветствие.

NICK_HELP_SET_KILL
	Синтаксис: SET KILL {ON | QUICK | IMMED | OFF}
	
	Переключает автоматическую защиту для Вашего ника:
	включает или выключает.  Когда защита в состоянии ON, если другой пользователь
	попытается взять ваш ник, у него будет 1 минута для того, чтобы
	выбрать себе другой ник, после этого %S изменит
	его ник.
	
	Если Вы выберете QUICK, у пользователя будет только 20 секунд
	ятобы сменить ник.  Если вы выберете
	IMMED, ник пользователя будет изменен немедленно без без
	предупреждения и шанса сменить ник; пожалуйста
	не используйте эту опцию без необходимости. Также, администратор
	вашей IRC сети может отключить эту опцию.

NICK_HELP_SET_SECURE
	Синтаксис: SET SECURE {ON | OFF}
	
	Включает или выключает опцию Безопасность на %S для Вашего
	ника. При включенном SECURE, Вы должны идентифицироваться на NickServ
	чтобы подтвердить, что именно Вы владелец данного ника,
	при этом на список доступа сервисы не обращают внимания.
	Тем не менее, если Вы есть в списке доступа, %S
	не будет автоматически отсоединять Вас не обращая внимания на установку
	опции KILL.

NICK_HELP_SET_PRIVATE
	Синтаксис: SET PRIVATE {ON | OFF}
	
	Переключает %S опци приватности в положение on или off для Вашего ника.
	Когда PRIVATE включена, Ваш ник не будет показываться
	по команде %S's LIST.
	(Тем не менее, любой, кто знает Ваш ник имеет возможность получить
	информацию о Вас, используя команду INFO.)

NICK_HELP_SET_HIDE
	Синтаксис: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Позволяет Вам скрыть часть информации о Вас, которая
	будет показываться по команде %S INFO на Ваш ник.
	Вы можете скрыть Ваш адрес email (EMAIL), время последнего захода в сеть
	и маску с которой Вы заходили (USERMASK) и последнее сообщение выхода (QUIT).
	Второй параметр определяет будет ли эта информация
	показываться (OFF) или скрываться (ON).
	
NICK_HELP_SET_MSG
	Синтаксис: SET MSG {ON | OFF}
	
	Позволяет Вам выбрать способ общения сервисов с Вами.
	Если установить режим MSG, сервисы будут использовать собщения в приват, в ином случае 
	будут использовать нотисы.

NICK_HELP_RECOVER
	Синтаксис: RECOVER ник [пароль]
	
	Позволяет Вам вернуть Ваш ник, если кто-то еще
	взял его; %S будет делать это автоматически
	если у вас установлена автоматическая защита.
	
	При использовании данной команды, %S создает псевдопользователя
	с указанным ником. Это заставит IRC сервер отключить
	другого пользователя с данным ником. Этот псевдопользователь
	будет оставаться в онлайне в течении минуты, чтобы гарантировать,
	что другой пользователь не сможет немедленно снова взять этот ник;
	через минуту вы можете вернуть себе Ваш ник. Также, вы можете использовать
	команду RELEASE (/msg %S HELP RELEASE) чтобы вернуть свой ник
	раньше.
	
	При использовании команды RECOVER, Ваш текущий адрес, который
	показывается в /WHOIS должен быть в списке доступа,
	вы должны быть идентифицированы, как владелец группы ника,
	который возвращаете, или вы должны указать верный пароль на
	данный ник.

NICK_HELP_RELEASE
	Синтаксис: RELEASE ник [пароль]
	
	Приказывает %S удалить любого, удерживающего ник пользователя,
	включая защиту ника или используя команду RECOVER.
	По умолчанию, удерживается в течении минуты;
	эта команда освобождает ник довольно быстро.
	
	Порядок использования команды RELEASE для ника: Ваш
	текущий адрес, который виден по команде /WHOIS должен быть одним из адресов
	в списке доступа, вы должны быть идентифицированы и находиться в группе
	этого ника, или Вы должны ввести верный пароль на
	этот ник.

NICK_HELP_GHOST
	Синтаксис: GHOST ник [пароль]
	
	Уничтожает "мертвое" IRC соединение, использующее Ваш ник.
	"мертвое" соединение - это когда реально его не существует,
	но когда IRC сервер думает, что оно все еще онлайн.
	Обычно это происходит, если Ваш компьютер перезагрузился,
	или Ваше соединение с локальной сетью или Интернет
	оборвалось в тот момент, когда Вы были в IRC.
	
	Порядок использования команды GHOST для ника: Ваш
	текущий адрес, который виден по команде /WHOIS должен быть одним из адресов
	в списке доступа, вы должны быть идентифицированы и находиться в группе
	этого ника, или Вы должны ввести верный пароль на
	этот ник.

NICK_HELP_INFO
	Синтаксис: INFO ник [ALL]
	
	Показывает информацию о нике, владельца
	ника, его последнюю маску и время, когда он заходил в IRC,
	параметры ника. Если Вы идентифицировали Ваш ник, Вы 
	получите больше информации и если Вы укажете параметр ALL, Вы
	увидите всю доступную информацию; не обращая внимания на то скрыта она 
	или нет.

NICK_HELP_LIST
	Синтаксис: LIST маска
	
	Показывает список всех зарегистрированных ников, которые содержат
	введенную маску, в формате nick!user@host. Ники, у которых включена опция
	PRIVATE не будут показаны.
	
	Примеры:
	
	    LIST *!joeuser@foo.com
	    Покажет все ники, владельцы которых имеют маску joeuser@foo.com.
	
	    LIST *Bot*!*@*
	    Покажет все зарегистрированные ники со словом Bot в их
	    нике (регистр неважен).
	
	    LIST *!*@*.bar.org
	    Покажет все ники, принадлежащие пользователям
	    в домене bar.org.

NICK_HELP_ALIST
	Синтаксис: ALIST [level]
	
	Покажет все каналы, в списках доступа которых Вы числитесь. Опционально, Вы можете выбрать
	определенный уровень в формате XOP или ACCESS. Результирующий список
	выведет каналы, на которых у Вас есть соответствующий уровень.

	Примеры:
	        ALIST Founder
	        Покажет все каналы, на которых Вы являетесь
	        фаундером (владельцем).

	        ALIST AOP
	        Покажет все каналы, на которых Вы являетесь аопом
	        или выше.

	        ALIST 10
	        Покажет все каналы, на которых Ваш уровень равен 10
	        или выше.
	
	Каналы, помеченные как NOEXPIRE будут показаны с восклицательным знаком
	в качестве префикса.
	
NICK_HELP_GLIST
	Syntax: GLIST
	
	Показывает все ники в Вашей группе.

NICK_HELP_STATUS
	Синтаксис: STATUS ник...
	
	Возвращает информацию о данном нике
	идентифицировался ли он как владелец ника. Ответ выводится
	в формате:
	
	    ник код-статуса
	
	где ник это ник, который Вы послали при вводе команды, и
	код-статуса может быть одним из:
	
	    0 - такого пользователя нет в онлайне или этот ник не зарегистрирован
	    1 - пользователь не идентифицирован как владелец ника
	    2 - пользователь идентифицирован как владелец ника только по списку доступа
	    3 - пользователь идентифицирован как владелец ника через идентификацию паролем
	
	До 16 ников сразу может быть запрошено этой командой;
	остальные будут проигнорированы. Сообщение об ошибке не будет генерироваться, если
	ник не отправлен в качестве параметра команды.
	
NICK_HELP_SENDPASS
	Синтаксис: SENDPASS nickname
	
	Отправляет пароль ника на адрес e-mail
	указанный при регистрации ника. Эту команду учень удобно использовать
	чтобы восстановить забытый пароль.
	
	Может быть ограничена к использованию только для IRC операторов в данной сети.
	
	При включенном шифровании паролей эта команда недоступна.

NICK_SERVADMIN_HELP
	
	Доступные команды для Администраторов сервисов:
	
	    GETPASS    Возвращает пароль ника
	                 (только если шифрование выключено)
	    FORBID     Предохраняет ник от использования
	
	Администраторы сервисов могут также удалить любой ник без идентификации
	для него, и могут смотреть список доступа для любого ника
	(/msg %S ACCESS LIST ник).

NICK_SERVADMIN_HELP_LOGOUT
	Синтаксис: LOGOUT [ник [REVALIDATE]]
	
	Без параметров, совершает обратный IDENTIFY эффект,
	т.е. делает Вас неидентифицированным как владельца ника.
	Примечание, однако, Вас не будут просить идентифицировать
	себя.
	
	с параметром, делает тоже самое с введенным ником. Если Вы
	укажете еще и REVALIDATE, сервисы попросят данный ник переидентифицировать
	себя. Это могут использовать только Администраторы сервисов.

NICK_SERVADMIN_HELP_DROP
	Синтаксис: DROP [ник]
	
	Без параметров, удаляет Ваш ник из
	базы данных %S.
	
	С параметром, удаляет выбранный нин из базы данных.
	Вы можете удалить любой ник из Вашей группы не имя каких-либо 
	особых привилегий. Удалить любой ник могут только
	Администраторы сервисов.

NICK_SERVADMIN_HELP_SET
	
	Администраторы сервисов могу также ставить параметр NOEXPIRE, с
	ники предохраняются от истечения.
	Кроме того, администраторы сервисов могут устанавливать опции для любого ника
	без ввода пароля ника, используя формат
	SET ник опция параметры.

NICK_SERVADMIN_HELP_SET_NOEXPIRE
	Синтаксис: SET [ник] NOEXPIRE {ON | OFF}
	
	Устанавливает особый режим для данного ника - никогда не истекать. Устанавливая это,
	в положение ON предохраняет ник от истечения. Если ник
	не указан, то не-истечение будет установлено на Ваш ник.
	
	Могут использовать только Администраторы сервисов.

NICK_SERVADMIN_HELP_INFO
	
	Администраторы сервисов могу использовать параметр ALL для любого ника.

NICK_SERVADMIN_HELP_LIST
	Синтаксис: LIST маска [FORBIDDEN] [NOEXPIRE] [UNCONFIRMED]
	
	Показывает список всех зарегистрированных ников which которые содержат
	введенную маску, в формате nick!user@host. Ники, у которых включена опция
	PRIVATE будут показаны только Администраторам сервисов. Ники
	с включенным параметром NOEXPIRE будут показаны с префиксом ! перед
	ником.
	
	If the FORBIDDEN, NOEXPIRE or UNCONFIRMED options are given, only
	nicks which, respectively, are FORBIDden, UNCONFIRMED or have the
	NOEXPIRE flag set will be displayed. If all three options are
	given, all three types of nicks will be displayed. These options
	are limited to Services admins.  
	
	Примеры:
	
	    LIST *!joeuser@foo.com
	    Покажет все зарегистрированные ники, владелец которых является joeuser@foo.com.
	
	    LIST *Bot*!*@*
	    Покажет все зарегистрированные ники, в которых есть слово Bot
	    (не зависит от регистра).
	
	    LIST * NOEXPIRE
	    Покажет все зарегистрированные ники у которых включена опция
	    NOEXPIRE.

NICK_SERVADMIN_HELP_ALIST
	Синтаксис: ALIST [ник] [уровень]
	
	Без параметров покажет список каналов, на которых Вы прописаны. С
	первым параметром, покажет каналы, на которых прописан nickname.
	С двумя параметрами покажет каналы, где прописан nickname с уровнем
	level или выше.

	Эту команду могут использовать только Администраторы сервисов.

NICK_SERVADMIN_HELP_GLIST
	Синтаксис: GLIST [ник]
	
	Без параметров, покажет список ников
	в Вашей группе.
	
	С параметром, покажет все ники, находящиеся в 
	группе указанного ника.
	Эту команду могут использовать только Администраторы сервисов.

NICK_SERVADMIN_HELP_GETPASS
	Синтаксис: GETPASS ник
	
	Возвращает пароль указанного ника.  ПРимечание при
	использовании данной команды, сообщение, включающее ник,
	вызвавший данную команду и ник, на который был запрошен пароль
	будет записан в лог и отправлен в WALLOPS/GLOBOPS.
	
	Эту команду могут использовать только Администраторы сервисов.
	
	Эта команда недоступна, если шифрование включено.

NICK_SERVADMIN_HELP_GETEMAIL
	Синтаксис: GETEMAIL user@emailhost

	Возвращает ники, у которых в настройках стоит указанный email. Внимание
	запрещено использовать символьные маски ни для user ни для emailhost. Каждый раз
	при использовании данной команды, сообщение, включающее человека, вызвавшего
	команду и указанный email будет записано в лог.

	Эту команду могут использовать только Администраторы сервисов.

NICK_SERVADMIN_HELP_FORBID
	Синтаксис: FORBID ник [причина]
	
	Запрещает регистрировать или исползовать ник
	Может быть отменено путем удаления ника (DROP).
	
	В некоторых сетях причина является необходимым параметром.
	
	Эту команду могут использовать только Администраторы сервисов.

###########################################################################
#
# ChanServ help messages
#
###########################################################################

CHAN_HELP
	%S позволяет Вам регистрировать канал и управлять
	каналом.  %S может предохранить
	от злобных пользователей, желающих "захватить" канал, используя
	ограничение возможности получения опа на канале. Доступные
	команды показаны ниже; чтобы использовать их пишите
	/msg %S команда. Для более подробной информации о конкретной
	команде, напишите /msg %S HELP команда.
	
	    REGISTER   Регистрация канала
	    IDENTIFY   Идентифицироваться как владелец канала
	    SET        Установить информацию о канале и опции канала
	    AOP        Модифицировать список AOP'ов
	    SOP        Модифицировать список SOP'ов
	    ACCESS     Модифицировать список привилегированных пользователей
	    LEVELS     Переустановить значения уровней доступа
	    AKICK      Управление списком автокиков
	    DROP       Удалить канал из базы данных ChanServ
	    SENDPASS   Поможет восстановить забытый пароль
	
	Другие команды: BAN, CLEAR, DEOP, DEVOICE, GETKEY, INFO, 
	                INVITE, KICK, LIST, LOGOUT, OP, TOPIC, 
	                UNBAN, VOICE, VOP

CHAN_HELP_UNREAL
	
	В данной сети также доступны команды:
	DEHALFOP, DEOWNER, DEPROTECT, HALFOP, HOP, OWNER,
	PROTECT

CHAN_HELP_ULTIMATE
	
	В этой сети также доступны команды:
	DEHALFOP, HALFOP, HOP

CHAN_HELP_ULTIMATE3
	
	Эти команды также доступны в Вашей сети:
	    DEHALFOP, HALFOP, HOP, ADMIN, DEADMIN

CHAN_HELP_EXPIRES
	
	Помните, что любой канал, не использующийся в течение %d дней
	(т.е. ни один пользователь, прописанный в списке доступа канала
	не посетит канал в течении данного времени) будет автоматически удален.

CHAN_HELP_REGISTER
	Синтаксис: REGISTER #канал пароль описание
	
	Регистрирует канал в базе данных %S. Чтобы использовать эту команду
	Вы должны быть оператором канала, который Вы
	пытаетесь зарегистрировать. Пароль должен быть
	использован с командой IDENTIFY, чтобы иметь возможность
	изменять различные параметры канала.
	Последний параметр (описание), который обязательно должен присутствовать,
	является описанием канала и, возможно, его тематикой.
	
	Когда Вы зарегистрируете канал, Вы будете записаны в список доступа канала
	как "владелец" канала.  Владелец канала может
	изменять любые параметры канала;
	%S также автоматически даст статус оператора канала
	владельцу при заходе его на канал.
	Используйте команду ACCESS (/msg %S HELP ACCESS) для
	для получения информации о том, как давать дополнительные привилегии
	на канале для других пользователей.
	
	ПРИМЕЧАНИЕ: Чтобы зарегистрировать канал, сперва
	Вы должны зарегистрировать Ваш ник. Если Вы этого еще не сделали,
	/msg %s HELP для получения информации о том, как зарегистрировать ник.

CHAN_HELP_IDENTIFY
	Синтаксис: IDENTIFY #канал пароль
	
	Идентифицирует Вас на %S как владельца данного
	канала. Многие команды для канала недоступны, пока Вы не
	идентифицируетесь. Пароль должен быть тем паролем, который
	Вы указывали, когда использовали команду REGISTER.
	
CHAN_HELP_LOGOUT
	Синтаксис: LOGOUT #канал ник
	
	Эта команда делает указанный ник неидентифицированным как
	владелец данного канала.
	
	Если Вы являетесь владельцем, Вы можете деидентифицировать кого угодно,
	в ином случае только себя.

CHAN_HELP_DROP
	Синтаксис: DROP #канал
	
	Удаляет указанный канал. Команда может быть использована только
	владельцем канала, который должен сначала дать команду IDENTIFY.

CHAN_HELP_SET
	Синтаксис: SET #канал опция параметры
	
	Позволяет владельцу канала менять различные настройки
	канала и другое.
	
	Доступные опции:
	
	    FOUNDER       Установить владельца канала
	    SUCCESSOR     Установить наследника канала
	    PASSWORD      Установить пароль владельца
	    DESC          Установить описание канала
	    URL           Установить URL канала
	    EMAIL         Установить адрес E-mail для канала
	    ENTRYMSG      Установить сообщение, которое будет отправлено
	                  пользователям, вошедшим в канал
	    BANTYPE       Установить тип банов сервисов на канале
	    MLOCK         Включить или выключить блокировку режимов канала
	    KEEPTOPIC     Хранить топик, если все покинули канал
	    OPNOTICE      Уведомлять об использовании команд OP/DEOP
	    PEACE         Регулировать использование критических команд
	    PRIVATE       Скрыть канал из показываемых по команде LIST
	    RESTRICTED    Ограничить доступ на канал
	    SECURE        Активировать дополнительные возможности %S'а по безопасности.
	    SECUREOPS     Ограничить получение статуса оператора канала
	    SECUREFOUNDER Ограничить получение статуса владельца канала
	    SIGNKICK      Подписывать или нет кик при использовании команды KICK
	    TOPICLOCK     Изменять топик только с помощью команды TOPIC
	    XOP           Выбрать систему привилегий
	
	Напишите /msg %S HELP опция для получения более подробной информации
	о конкретной опции.

CHAN_HELP_SET_FOUNDER
	Синтаксис: SET #канал FOUNDER ник
	
	Изменяет владельца канала. Ник нового владельца должен
	быть зарегистрированным.

CHAN_HELP_SET_SUCCESSOR
	Синтаксис: SET #канал SUCCESSOR ник
	
	Установить наследника канала. Если ник владельца истечет
	или будет удален, пока канал существует,
	наследник станет новым владельцем канала.
	Тем не менее, если наследник уже зарегистрировал
	чересчур много каналов (более %d), канал будет удален,
	так же, как если бы наследник не был указан. Ник наследника
	канала должен быть зарегистрированным.

CHAN_HELP_SET_PASSWORD
	Синтаксис: SET #канал PASSWORD пароль
	
	Установить пароль, используемый для идентификации
	на канале как его владелец.

CHAN_HELP_SET_DESC
	Синтаксис: SET #канал DESC описание
	
	Устанавливает описание канала, которое будет показано при использовании
	команд LIST и INFO.

CHAN_HELP_SET_URL
	Синтаксис: SET #канал URL [http://адрес.странички]
	
	Установить URL канала. Этот URL
	будет показан, когда кто-либо запросит информацию о канале
	с помощью коанды INFO. Если команду дать без параметров,
	удалит текущий URL канала.

CHAN_HELP_SET_EMAIL
	Синтаксис: SET #канал EMAIL [адрес@почты]
	
	Установить адрес электронной почты канала.
	Этот адрес будет показан, когда кто-либо запросит
	информацию о канале с помощью команды INFO. Если команду
	дать без параметров, удалит текущий email
	канала.

CHAN_HELP_SET_ENTRYMSG
	Синтаксис: SET #канал ENTRYMSG [сообщение]
	
	Установить сообщение, которое будет отправлено нотисом
	каждому вошедшему на канал пользователю. Если команду дать без параметров,
	удалит сообщение входа.

CHAN_HELP_SET_BANTYPE
	Синтаксис: SET #канал BANTYPE тип бана
	
	Установить тип бана, который будет использован, когда
	кто-то будет забанен сервисами на Вашем канале.
	
	тип бана должен быть номером между 0 и 3 включительно:
	
	0: бан вида *!user@host
	1: бан вида *!*user@host
	2: бан вида *!*@host
	3: бан вида *!*user@*.domain

CHAN_HELP_SET_KEEPTOPIC
	Синтаксис: SET #канал KEEPTOPIC {ON | OFF}
	
	Включает или выключает опцию хранения топика на
	канале. Когда хранение топика включено, топик канале
	будет хранится с помощью %S даже после того, как
	последний пользователь покинет канал, и будет восстановлен
	как только на канал кто-либо снова зайдет.

CHAN_HELP_SET_TOPICLOCK
	Синтаксис: SET #канал TOPICLOCK {ON | OFF}
	
	Включает или выключает опцию блокировать топик на канале.
	Когда блокирование топика включено, %S разрешит менять
	топик канала только используя команду 
	TOPIC.

CHAN_HELP_SET_MLOCK
	Синтаксис: SET #канал MLOCK режимы
	
	Устанавливает и блокирует определенные режимы канала. %S
	позволяет Вам установить режимы канала, которые Вы хотите
	включить, выключить, или сделать свободными для включения/выключения.
	
	Параметр режимы в прнципе напоминает команду
	/MODE; режимы, начинающиеся с + блокируются в
	состоянии включено, а режимы, начинающиеся с - в состоянии выключено. Примечание:
	тем не менее, это не то же самое, кто команда /MODE, использование
	SET MLOCK удалит все предыдущие режимы
	и заменит их новыми!
	
	Внимание:  Если вы установили и заблокировали ключ канала, как во втором
	примере ниже, вы должны также установить опцию RESTRICTED
	на канал (см. HELP SET RESTRICTED), или любой, кто зайдет на канал
	когда тот будет пуст сможет увидеть ключ!
	
	Примеры:
	
	    SET #канал MLOCK +nt-iklps
	    Блокирует включенными режимы n и t, и выключенными i, k, l, p, и s.
	    Режим m остается свободным к включению/выключению любым оператором.
	
	    SET #канал MLOCK +knst-ilmp мой-ключ-пароль
	    Блокирует включенным режимы k, n, s, и t, и выключенными i, l, m, и p.
	    Также устанавливает ключ для входа на канал
	    "мой-ключ-пароль".
	
	    SET #канал MLOCK +
	    Удаляет блокировку режимов; Все режимы каналов свободно могут быть
	    изменены любым оператором канала.

CHAN_HELP_SET_PEACE
	Синтаксис: SET #канал PEACE {ON | OFF}
	
	Включает или выключает опцию спокойствия на канале.
	Когда спокойствие включено, пользователь не может кикать,
	банить или удалять статус на канале с другого пользователя с уровнем
	выше или равным командами %S.

CHAN_HELP_SET_PRIVATE
	Синтаксис: SET #канал PRIVATE {ON | OFF}
	
	Включает или выключает опцию приватности на канале.
	Когда приватность включена, использование /msg %S LIST не покажет
	данный канал в списке.

CHAN_HELP_SET_RESTRICTED
	Синтаксис: SET #канал RESTRICTED {ON | OFF}
	
	Включает или выключает ограниченный доступ на
	канал. Когда ограниченный доступ включен, пользователи,
	не имеющие права на получение статуса оператора канала
	(пользователи с отрицательными уровнями доступа, и, если
	безопасность статуса опа включена и пользователя нет в списке доступа) будут
	немедленно забанены и выкинуты с канала.

CHAN_HELP_SET_SECURE
	Синтаксис: SET #канал SECURE {ON | OFF}
	
	Включает или выключает опции безопасности %S для
	канала. Когда SECURE включено, только пользователи
	с зарегистрированными никами, проидентифицировавшиеся
	на %s и включенные в список доступа канала
	смогут войти на канал.

CHAN_HELP_SET_SECUREOPS
	Синтаксис: SET #канал SECUREOPS {ON | OFF}
	
	Включает или выключает безопасность статуса оператора канала.
	Когда безопасность статуса оператора включена, пользователи, не прописанные операторами
	в списке доступа не могут получить статус опа на канале.
	
CHAN_HELP_SET_SECUREFOUNDER
	Синтаксис: SET #канал SECUREFOUNDER {ON | OFF}
	
	Включает или выключает безопасность владельца на канале.
	Когда безопасность владельца включена, только настоящий владелец
	может удалить канал, изменить его пароль, его владельца и его
	наследника, а не любой, кто идентифицируется на %S.
	
CHAN_HELP_SET_SIGNKICK
	Синтаксис: SET #канал SIGNKICK {ON | LEVEL | OFF}
	
	Включает или выключает подписанные кики
	на канале. Когда SIGNKICK включен, кики через команду
	%S KICK будут "подписаны" тем, кто испоьзовал эту команду
	с указанием причины.
	
	Если Вы используете LEVEL, тот, у кого уровень выше или равен 
	Вам (если Вы пытаетесь его кикнуть) то
	кик не произойдет. См. /msg %S HELP LEVELS для получения более подробной информации.

CHAN_HELP_SET_XOP
	Синтаксис: SET #канал XOP {ON | OFF}
	
	Включает или выключает систему списка доступа xOP на канале.
	Когда XOP включен, Вы можете использовать только команды AOP/SOP/VOP
	чтобы изменять привилегии пользователей на канале,
	в противоположном случае, вы можете использовать команду ACCESS.
	
	Техническое примечание: когда Вы переключите систему привилегий в xOP,
	ваши установки уровней дла канала и пользователей будут изменены,
	и Вы не сможете вернуть эти значение, переключившись
	обратно в систему привилегий ACCESS! 
	
	Также, Вы должны удостовериться в том, что Ваши полльзователи правильно прописаны в xOP 
	листе после переключения из системы ACCESS в xOP, потому что
	это может пройти вовсе не безупречно... вообще, крайне не рекомендуется
	использовать систему xOP если Вы изменили значения уровней, используя
	команду LEVELS.
	
	Переключение из системы xOP в систему ACCESS обычно
	проходит без проблем.

CHAN_HELP_SET_OPNOTICE
	Синтаксис: SET #канал OPNOTICE {ON | OFF}
	
	Включает или выключает оп-уведомление на канале.
	Когда оп-уведомление включено, %S будет посылать нотис
	на канал когда кто-либо использует команду OP или DEOP
	на канале.

CHAN_HELP_AOP
	Синтаксис: AOP #канал ADD ник
	           AOP #канал DEL {nick | номер-записи | list}
	           AOP #канал LIST [маска | list]
	           AOP #канал CLEAR
	
	Поддерживает список AOP (АвтоОператоров) канала. AOP 
	дает возможность пользователю получить опа при входе на канал,
	снимать с себя баны, приглашать по необходимости, иметь приветственное
	сообщение входа на канал, и многое другое.
	
	Команда AOP ADD добавляет указанный ник в список
	AOP'ов.
	
	Команда AOP DEL удаляет указанный ник из списка
	AOP'ов. Если параметр указывает на номер записи, данные
	записи будут удалены. (См. примеры команды LIST ниже.)
	
	Команда AOP LIST показывает список AOP'ов. Если
	использовать символьную маску, только совпадающие с ней записи
	будут показаны. Если указан список номеров записей,
	только эти записи будут показаны; например:

	   AOP #канал LIST 2-5,7-9
	   Покажет AOP'ов, записи со 2 до 5 и с
	   7 до 9.
	      
	Команда AOP CLEAR удаляет все записи из списка
	AOP'ов.
	
	Команды AOP ADD и AOP DEL могут использовать только
	SOP'ы или выше, а команду AOP CLEAR может использовать только
	владелец канала. Тем не менее, любой пользователь из списка
	AOP'ов может использовать команду AOP LIST.
	
	Эта команда может быть отключена на Вашем канале, и
	тогда вы должен использовать команды списка доступа (access list). См. 
	/msg %S HELP ACCESS для получения информации об использовании списка доступа,
	и /msg %S HELP SET XOP чтобы узнать как переключиться между системами
	список доступа и системой xOP.

CHAN_HELP_HOP
	Синтаксис: HOP #канал ADD ник
	           HOP #канал DEL {ник | номер записи | list}
	           HOP #канал LIST [маска | list]
	           HOP #канал CLEAR
	
	Поддерживает список HOP (ПолуОператоров) канала. Список HOP 
	включает пользователей, которые могу получить статус полуоператора
	на канале.
	
	Команда HOP ADD добавляет указанный ник в список
	HOP'ов.
	
	Команда HOP DEL удаляет указанный ник из списка
	HOP'ов. Если параметр указывает на номер записи, данные
	записи будут удалены. (См. примеры команды LIST ниже.)
	
	Команда HOP LIST показывает список HOP'ов. Если
	использовать символьную маску, только совпадающие с ней записи
	будут показаны. Если указан список номеров записей,
	только эти записи будут показаны; например:

	   HOP #канал LIST 2-5,7-9
	   Покажет записи HOP'ов с номерами с 2 до 5 и с
	   7 по 9.
	      
	Команда HOP CLEAR удаляет все записи из списка
	HOP'ов.
	
	Команды HOP ADD и HOP DEL могут использовать только
	AOP'ы или выше, а команду HOP CLEAR может использовать только
	владелец канала.
	
	Эта команда может быть отключена на Вашем канале, и
	тогда вы должен использовать команды списка доступа (access list). См. 
	/msg %S HELP ACCESS для получения информации об использовании списка доступа,
	и /msg %S HELP SET XOP чтобы узнать как переключиться между системами
	список доступа и системой xOP.

CHAN_HELP_SOP
	Синтаксис: SOP #канал ADD ник
	           SOP #канал DEL {ник | номер записи | list}
	           SOP #канал LIST [маска | list]
	           SOP #канал CLEAR
	
	Включает список SOP (СуперОператоров) канала. Список SOP'ов
	включает пользователей, у которых есть все права AOP'ов, и кроме того
	они могут использовать автокик и цензуру слов, 
	отправлять и читать сообщения канал, и многое другое.
	
	Команда SOP ADD добавляет указанный ник в список
	SOP'ов.
	
	Команда SOP DEL удаляет указанный ник из списка
	SOP'ов. Если параметр указывает на номер записи, данные
	записи будут удалены. (См. примеры команды LIST ниже.)
	
	Команда SOP LIST показывает список SOP'ов. Если
	использовать символьную маску, только совпадающие с ней записи
	будут показаны. Если указан список номеров записей,
	только эти записи будут показаны; например:

	   SOP #канал LIST 2-5,7-9
	   Покажет список AOP'ов с номерами записей от 2 до 5 и от
	   7 до 9.
	      
	Команда SOP CLEAR удалит все записи из списка
	SOP'ов.
	
	Команды SOP ADD, SOP DEL и SOP CLEAR могут быть использованы
	только владельцем канала. Тем не менее, любой пользователь из списка
	AOP'ов может использовать команду SOP LIST.
	
	Эта команда может быть отключена на Вашем канале, и
	тогда вы должен использовать команды списка доступа (access list). См. 
	/msg %S HELP ACCESS для получения информации об использовании списка доступа,
	и /msg %S HELP SET XOP чтобы узнать как переключиться между системами
	список доступа и системой xOP.

CHAN_HELP_VOP
	Синтаксис: VOP #канал ADD ник
	           VOP #канал DEL {ник | номер записи | list}
	           VOP #канал LIST [маска | list]
	           VOP #канал CLEAR
	
	Включает список VOP (Войсов) канала.  
	Список VOP включают пользователей, которые могут получить войс
	на канале по необходимости.
	
	Команда VOP ADD добавляет указанный ник в список
	VOP'ов.
	
	Команда VOP DEL удаляет указанный ник из списка
	VOP'ов. Если параметр указывает на номер записи, данные
	записи будут удалены. (См. примеры команды LIST ниже.)
	
	Команда VOP LIST показывает список VOP'ов. Если
	использовать символьную маску, только совпадающие с ней записи
	будут показаны. Если указан список номеров записей,
	только эти записи будут показаны; например:

	   VOP #канал LIST 2-5,7-9
	   Покажет список VOP'ов с номерами записей от 2 до 5 и от
	   7 до 9.
	      
	Команда VOP CLEAR удалит все записи из списка
	VOP'ов.
	
	Команды VOP ADD и VOP DEL могут использовать только
	AOP'ы или выше, а команду VOP CLEAR может использовать только
	владелец канала.
	
	Эта команда может быть отключена на Вашем канале, и
	тогда вы должен использовать команды списка доступа (access list). См. 
	/msg %S HELP ACCESS для получения информации об использовании списка доступа,
	и /msg %S HELP SET XOP чтобы узнать как переключиться между системами
	список доступа и системой xOP.

CHAN_HELP_ACCESS
	Синтаксис: ACCESS #канал ADD ник уровень
	           ACCESS #канал DEL {ник | номер записи | list}
	           ACCESS #канал LIST [маска | list]
	           ACCESS #канал CLEAR
	
	Управляет списком доступа канала. Список доступа
	определяет какие пользователи могут быть операторами канала,
	иметь доступ к командам %S на канал. Различные уровни доступа
	разрешают доступ к различным настройкам канала и определяют пользовательские
	привилегии; /msg %S HELP ACCESS LEVELS для получения более
	подробной информации. Любой, не включенный в список доступа пользователь
	считается пользователем в уровнем доступа 0.
	
	Команда ACCESS ADD добавляет указанный ник в
	список доступа с указанным же уровнем; если данный ник уже
	уже есть в списке, его уровень будет изменен на указанный
	в этой команде. Указанный уровень
	должен быть меньше чем уровень пользователя, вызвавшего это команду, и
	если ник уже есть в списке доступа, текущий
	уровень доступа данного ника должен быть меньше, чем уровнь доступа
	пользователя, вызвавшего эту команду.
	
	Команда ACCESS DEL удаляет указанный ник из
	списка доступа. если указаны номера записей, данные записи
	будут удалены. (см. пример использования команды LIST ниже.)
	
	Команда ACCESS LIST показывает список доступа. Если
	указать символьную маску, только совпадающие с ней значения
	будут показаны. Если указать номера записей, то только,
	эти записи будет показаны; например:

	   ACCESS #канал LIST 2-5,7-9
	   Покажет в списке доступа канала записи со 2 по 5 и с
	   7 по 9.
	      
	Команда ACCESS CLEAR удаляет все записи из
	списка доступа.

CHAN_HELP_ACCESS_LEVELS
	Уровни доступа пользователей
	
	По умолчанию, определены нижеследующие значения уровней доступа:
	
	   Founder   Полный доступ к функциям %S; автоматическое получение
	                     статуса оператора при входе на канал. Помните,
	                     что только один человек может иметь статус
	                     владельца (и это нельзя никак изменить командой
	                     ACCESS).
	        10   Доступ к команде AKICK; автооп.
	         5   Автооп.
	         3   Автовойс.
	         0   Никаких особых привилегий; может быть опнут другим
	                     опом (только если безопасность статуса опа выключена).
	        <0   Не может получиь опа на канале.
	
	Эти уровни могут быть изменены, возможно добавление новых уровней, используя
	команду LEVELS; напишите /msg %S HELP LEVELS для более полной
	информации.

CHAN_HELP_AKICK
	Синтаксис: AKICK #канал ADD маска [причина]
	           AKICK #канал STICK маска
	           AKICK #канал UNSTICK маска
	           AKICK #канал DEL маска
	           AKICK #канал LIST [маска]
	           AKICK #канал VIEW [маска]
	           AKICK #канал ENFORCE
	           AKICK #канал CLEAR
	
	Поддерживает список автокиков канала. Если пользователь,
	включенный в список автокиков попытается зайти на канал,
	%S забанит его на канале, а затем выкинет
	его.
	
	Команда AKICK ADD добавляет ник или маску
	в список автокиков. Если указана причина,
	то она будет использована, когда пользователь будет
	кикнут; если причина не указана, будет использована причина по умолчанию: "You have been
	banned from the channel".
	
	Команда AKICK STICK создает постоянный бан указанной маски
	на канале. Если кто-то попытается удалить этот бан, %S
	автоматически установит его снова. Вы можете использовать это
	для зарегистрированных ников.
	
	Команда AKICK UNSTICK отменяет эффект, вызываетмый командой
	AKICK STICK, но бан с канала Вы должны
	удалить вручную.
	
	Команда AKICK DEL удаляет указанный ник или маску
	из списка автокиков. Тем не менее, это не удалит баны
	с канала; они должны быть удалены
	вручную.
	
	Команда AKICK LIST показывает текущий список автокиков, или
	опционально показывает автокики из указанной
	маски.
	
	Команда AKICK VIEW это более подробная версия команды
	AKICK LIST.
	
	Команда AKICK ENFORCE заставляет %S немедлено
	проверить список акиков и удалить с канала пользователей, включенных
	в него.
	
	Команда AKICK CLEAR удаляет все записи
	из списка автокиков.

CHAN_HELP_LEVELS
	Синтаксис: LEVELS #канал SET тип уровень
	           LEVELS #канал {DIS | DISABLE} тип
	           LEVELS #канал LIST
	           LEVELS #канал RESET
	
	Команда LEVELS позволяет удобно управлять уровнями
	доступа, используемыми для списков доступа. Используй эту
	команду, Вы можете определить уровень доступа, требуемый для большинства
	функций %S'а. (Команды SET FOUNDER и SET PASSWORD,
	также как эту команда, может использовать только
	владелец канала.)
	
	LEVELS SET устанавливает уровень доступа для функции или группы функций.
	LEVELS DISABLE (или DIS - так короче ;)
	отключает функцию или запрещает доступ к функции
	всем, кроме владельца канала.
	LEVELS LIST покажет текущие уровни доступа для каждой функции или
	группы функций. LEVELS RESET сбросит все настройки уровней
	доступа на настройки по умолчанию (см.
	HELP ACCESS LEVELS).
	
	Чтобы посмотреть список функций, на которые можно определять уровни доступа,
	см. HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Следующая функция принята. Примечание:
	уровни для Автодеоп и запрета на вход являются минимальными,
	в то время как все остальные являются максимальными.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Синтаксис: INFO #канал [ALL]
	
	Показывает информацию об указанном канале,
	включая его владельца, время регистрации, когда был последний
	раз использован, описание, заблокированные режимы, если они имеют место быть. Если 
	указан параметр ALL, также будет показано сообщение входа и
	наследник.
	
	По умолчанию, опцию ALL может использовать только
	владелец канала.

CHAN_HELP_LIST
	Синтаксис: LIST маска
	
	Покажет все зарегистрированные каналы по указанной символьной маске.
	(Каналы с включенной опцией PRIVATE не будут показаны.)

CHAN_HELP_OP
	Синтаксис: OP [#канал [ник]]
	
	Дает статус оператора канала указанному нику на указанном канале. Если ник не указан,
	дает статус оператора канала Вам. Если канал и ник не указаны,
	дает статус оператора на всех каналах, на которых у Вас есть
	соответствующие права доступа.
	
	По умолчанию, может быть использована AOP'ами или лицами с уровнем доступа 5
	и выше.

CHAN_HELP_DEOP
	Синтаксис: DEOP [#канал [ник]]
	
	Снимает статус оператора канала с указанного ника на указанном канале. Если ник не указан,
	снимает статус оператора с Вас. Если ни канал, ни ник не указаны, снимает статус,
	оператора с Вас на всех каналх, на которых у Вас есть соответствующие
	права доступа.
	
	По умолчанию, может быть использована AOP'ами или лицами с уровнем доступа 5
	и выше.
	
CHAN_HELP_VOICE
	Синтаксис: VOICE [#канал [ник]]
	
	Дает войс указанному нику на указанном канале. Если ник не указан,
	дает войс Вам. Если канал и ник не указаны,
	дает войс Вам на всех каналах, на которых у Вас есть
	соответствующие права доступа.
	
	По умолчанию, может быть использована AOP'ами или лицами с уровнем доступа 5
	и выше. Может быть использована VOP'ами (либо лицами с уровнем доступа больше 3)
	чтобы дать войс самому себе.

CHAN_HELP_DEVOICE
	Синтаксис: DEVOICE [#канал [ник]]
	
	Снимает войс с указанного ника на указаном канале. Если ник не указан,
	снимет войса с Вас. Если канал и ник не указаны,
	снимет войс с Вас на всех каналах, на которых у Вас есть
	соответствующие права доступа.
	
	По умолчанию, может быть использована AOP'ами или лицами с уровнем доступа 5
	и выше. Может быть использована VOP'ами (либо лицами с уровнем доступа больше 3)
	чтобы снять войс с себя.
	
CHAN_HELP_HALFOP
	Синтаксис: HALFOP [#канал [ник]]
	
	Дает статус полуоператора канала указанному нику на указанном канале. Если ник не указан,
	дает статус полуоператора канала Вам. Если канал и ник не указаны,
	дает статус полуоператора на всех каналах, на которых у Вас есть
	соответствующие права доступа.
	
	По умолчанию, может быть использована AOP'ами или лицами с уровнем доступа 5
	и выше. Может быть использована HOP'ами (либо лицами с уровнем доступа больше 4)
	чтобы дать статус полуоператора себе.

CHAN_HELP_DEHALFOP
	Синтаксис: DEHALFOP [#канал [ник]]
	
	Снимает статус полуоператора канала с указанного ника на указанном канале. Если ник не указан,
	снимает статус полуоператора с Вас. Если ни канал, ни ник не указаны, снимает статус,
	полуоператора с Вас на всех каналх, на которых у Вас есть соответствующие
	права доступа.
	
	По умолчанию, может быть использована AOP'ами или лицами с уровнем доступа 5
	и выше. Может быть использована HOP'ами (либо лицами с уровнем доступа больше 4)
	чтобы снять статус полуоператора с себя.
	
CHAN_HELP_PROTECT
	Синтаксис: PROTECT [#канал [ник]]
	
	Ставит защиту на указанный ник на указанном канале. Если ник не указан,
	ставит защиту на Вас. Если канал и ник не указаны,
	ставит защиту на Вас на всех каналах, на которых у Вас есть
	соответствующие права доступа.
	
	По умолчанию может быть использована только владельцем. SOP'ы либо лица с уровнем
	доступа 10 и лучше могут использовать эту команду только на себя.

CHAN_HELP_DEPROTECT
	Синтаксис: DEPROTECT [#канал [ник]]
	
	Снимает защиту на указанный ник на указанном канале. Если ник не указан,
	снимает защиту с Вас. Если канал и ник не указаны,
	снимает защиту с Вас на всех каналах, на которых у Вас есть
	соответствующие права доступа.
	
	По умолчанию может быть использована только владельцем. SOP'ы либо лица с уровнем
	доступа 10 и лучше могут использовать эту команду только на себя.

CHAN_HELP_OWNER
	Синтаксис: OWNER [#канал]
	
	Дает Вам статус владельца канала. Если канал не указан,
	даст Вам статус владельца на всех каналах, на которых у Вас
	есть соответствующие права доступа.
	
	Может быть использована только ДЕЙСТВИТЕЛЬНЫМ владельцем канала.

CHAN_HELP_DEOWNER
	Синтаксис: DEOWNER [#канал]
	
	Снимает с Вас статус владельца канала. Если канал не указан,
	снимает с Вас статус владельца на всех каналах, на которых у Вас
	есть соответствующие права доступа.
	
	Может быть использована только ДЕЙСТВИТЕЛЬНЫМ владельцем канала.

CHAN_HELP_INVITE
	Синтаксис: INVITE канал
	
	Заставит %S пригласить Вас на указанный канал.  
	
	По умолчанию, может быть испоьзована только AOP'ами или лицами с уровнем доступа 5
	и выше.

CHAN_HELP_UNBAN
	Синтаксис: UNBAN канал
	
	Заставит %S удалить с указанного канала все баны, не позволяющие
	Вам зайти на него.  
	
	По умолчанию, может быть испоьзована только AOP'ами или лицами с уровнем доступа 5
	и выше.
	
CHAN_HELP_KICK
	Синтаксис: KICK [#канал [ник [причина]]]
	
	Выкидывает указанного пользователя с указанного канала. Если ник не указан,
	выкидывает Вас. Если канал и ник не указаны,
	выкинет Вас со всех каналов, на которых у Вас есть соответствующие права
	доступа.
	
	По умолчанию, может быть испоьзована только AOP'ами или лицами с уровнем доступа 5
	и выше.

CHAN_HELP_BAN
	Синтаксис: BAN [#канал [ник [причина]]]
	
	Банит указанный ник на указанном канале. Если ник не указан,
	банит Вас. Если канал и ник не указан,
	банит Вас на всех каналах, на которых у Вас есть соответствующие
	права доступа.
	
	По умолчанию, может быть испоьзована только AOP'ами или лицами с уровнем доступа 5
	и выше.

CHAN_HELP_TOPIC
	Синтаксис: TOPIC #канал [топик]
	
	Заставит %S изменить топик канала на указанный
	Вами. Если топик не указан, то он будет полностью
	удален. Эту команду обычно используют для установки топика при включенной
	опции SET TOPICLOCK. Напишите /msg %S HELP SET TOPICLOCK
	для более подробной информации.
	
	По умолчанию, может быть использована только
	владельцем канала. 

CHAN_HELP_CLEAR
	Синтаксис: CLEAR #канал что_именно
	
	Заставляет %S очистить определенные режимы и настройки канала.  что_именно
	может быть одним из:
	
	     MODES    Сбросить все режимы канала (т.е. удалить
	              режимы i,k,l,m,n,p,s,t).
	     BANS     Удалить все баны на канале.
	     EXCEPTS  Удалить все исключения из банов на канале.
	     OPS      Снять статус оператора (режим +o) со
	              всех операторов канала.
	     HOPS     Removes channel-halfoperator status (mode +h) from
	              all channel halfoperators, if supported.
	     VOICES   Снять "войсы" (режим +v) со всех, 
	              на ком установлен этот режим.
	     USERS    Удалить (выкинуть) всех пользователей с канала.
	
	По умолчанию, может быть использована только
	владельцем канала. 
	
CHAN_HELP_GETKEY
	Синтаксис: GETKEY #канал
	
	Возвращает секретный ключ (пароль для входа) канала. Эта команда
	обычно используется ботами и/или скриптами, команда возвращает
	такую строку:
	
	KEY <#канал> <секретный ключ>
	
	если секретный ключ показан как "NO KEY"  - ключа на канале нет.
	
CHAN_HELP_SENDPASS
	Синтаксис: SENDPASS #канал
	
	Отправляет пароль канала по адресу e-mail,
	указанному в настройках владельца канала. Эту команду удобно использовать
	чтобы вернуть забытый пароль.
	
	В некоторых сетях эту команду могут использовать только IRC операторы.
	
	Эта команда недоступна при включенном шифровании.

CHAN_SERVADMIN_HELP
	
	Нижеследующие команды доступны для администраторов сервисов:
	
	    GETPASS    Получить пароль владельца канала
	               (только если шифрование выключено)
	    FORBID     Запретить использовать канал
	    SUSPEND    Приостановить работу канала с сохранением
	               данных и настроек канала.
	    UNSUSPEND  Восстановить работу приостановленного канала.
	    STATUS     Возвращает текущий уровень доступа для указанного
	               пользователя на канале.
	
	Администраторы сервисов могут также удалить любой канал без
	идентификации как владелец канала, могут просматривать список доступа, список акиков,
	и установки уровней доступа на канале.
	
CHAN_SERVADMIN_HELP_LOGOUT
	Синтаксис: LOGOUT #канал [ник]
	
	Эта команда деидентифицирует указанный ник
	на указаном канале.
	
	Если Вы владелец канала, вы можете деидентифицировать любого,
	иначе же, только себя.
	
	Если Вы Администратор сервисов, вы можете деидентифицировать
	любого на любом канале без какой-либо дополнительной идентификации
	на канале. Также, если Вы не укажете ник;
	эта команда деидентифицирует всех пользователей на указанном канале.

CHAN_SERVADMIN_HELP_DROP
	Синтаксис: DROP #канал
	
	Удаляет указанный канал. Только Администраторы сервисов
	могут удалить канал, не идентифицируясь на ChanServ как владелец.

CHAN_SERVADMIN_HELP_SET
	
	Администраторы сервисов также могут установить опцию NOEXPIRE,
	если ее включить, данный канал никогда не истечет.
	Также, Администраторы сервисов могут устанавливать любую опцию
	на любом канале без какой либо дополнительной идентификации.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Синтаксис: SET #канал NOEXPIRE {ON | OFF}
	
	Делает указанный канал неистекаемым. Установка этой опции
	в положение ON предохраняет канал от истекания.
	
	Может быть испоьзована только Администраторами сервисов.

CHAN_SERVADMIN_HELP_INFO
	
	Администраторы сервисов могут использовать параметр ALL для любого канала.

CHAN_SERVADMIN_HELP_LIST
	Синтаксис: LIST маска [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
	
	Список всех зарегистрированных каналов по указанной маске.
	Каналы с параметром PRIVATE будут показаны только
	Администраторам сервисов. Каналы с параметром NOEXPIRE будут
	показаны с префиксом ! перед именем канала.
	
	Если указать опции FORBIDDEN, SUSPENDED или NOEXPIRE то
	будут показаны только каналы с параметрами FORBID, SUSPEND или
	NOEXPIRE соответственно. Если указать все эти опции,
	будут показаны все типы каналов. Эти параметры могут использовать
	только Администраторы сервисов.

CHAN_SERVADMIN_HELP_GETPASS
	Синтаксис: GETPASS #канал
	
	Возвращает пароль указанного канала. ПРимечание при
	использовании этой команды, сообщение, включающее того,
	использовал эту команду и канал, пароль на который был запрошен,
	будут записаны в лог и отправлены WALLOPS/GLOBOPS.
	
	Эту команду могут использовать только Администраторы сервисов.

CHAN_SERVADMIN_HELP_FORBID
	Синтаксис: FORBID #канал [причина]
	
	Запрещает регистрировать или использовать указанный канал.
	Отменяется путем удаления канала.
	
	В некоторых сетях причина является обязательным параметром.
	
	Эту команду могут использовать только Администраторы сервисов.

CHAN_SERVADMIN_HELP_SUSPEND
	Синтаксис: SUSPEND #канал [причина]
	
	Приостанавливает регистрацию или использование указанного канала.
	Отменяется путем использования команды UNSUSPEND, которая
	восстанавливает все настройки и данные канала.
	
	В некоторых сетях причина является обязательным параметром.
	
	Эту команду могут использовать только Администраторы сервисов.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Синтаксис: UNSUSPEND #канал
	
	Восстанавливает приостановленный канал. Все данные и настройки
	канала будут также восстановлены.
	
	Эту команду могут использовать только Администраторы сервисов.

CHAN_SERVADMIN_HELP_STATUS
	Синтаксис: STATUS #канал ник
	
	Возвращает текущий уровень доступа указанного ника на
	указанном канале. Ответ может быть таким:
	
	    STATUS #канал ник уровень доступа
	
	Если произойдет ошибка, ответ будет таким:
	
	    STATUS ERROR сообщение об ошибке
	
	Могут использовать только Администраторы сервисов.

###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP
	%S это удобный сервис, позволяющий пользователям IRC отправлять
	короткие сообщения другим пользователям, которых нет в данный момент в сети,
	или на каналы(*). Ник отправителя, как и
	ник получатели или имя канала должны быть
	зарегистрированы, для отправки мемки.

	%S's включает команды:

	    SEND   Отправить мемо на ник или канал
	    CANCEL Отменить последнюю отправленную Вами мемку
	    LIST   Список Ваших мемок
	    READ   Читать мемки
	    DEL    Удалять мемки
	    SET    Установка опций для мемок
	    INFO   показать информацию о Ваших мемках

MEMO_HELP_ADMIN
	    SENDALL  отправляет мемку всем зарегистрированным пользователям

MEMO_HELP_OPER
	    STAFF  Отправляет мемку всем операторам/администраторам сервисов

MEMO_HELP_FOOTER
	
	Напишите /msg %S HELP команда для получения более подробной
	информации.

	(*) По умолчанию, любой пользователь с уровнем доступа на канале 
	    не менее 10 может читать мемки, отправленные каналу. Это может быть
	    изменено использованием команды %s LEVELS.

MEMO_HELP_SEND
	Синтаксис: SEND {ник | #канал} текст сообщения
	
	Отправляет сообщения для ника или для #канала, содержащее
	текст сообщения. Если получатель ник, то он будет уведомлен
	что Вы отправили ему собщение. Получатель, будь то ник или канал,
	должен быть зарегистрирован на NickServ или ChanServ соответственно.
	
MEMO_HELP_CANCEL
	Синтаксис: CANCEL {ник | #канал}
	
	Отменяет последнее сообщение, отправленное вами на ник или канал,
	предохраняя его от прочтения адресатом.

MEMO_HELP_LIST
	Синтаксис: LIST [#канал] [list | NEW]
	
	Показывает текущий список сообщений. С параметром NEW, покажет только
	новые (непрочитанные) сообщения. Непрочитанные сообщения будут с пометкой в виде "*"
	слева от номера сообщения. Вы можете также указать диапазон
	номеров, например так:

	   LIST 2-5,7-9
	   Покажет сообщения со 2 до 5 и с 7 до 9.

MEMO_HELP_READ
	Синтаксис: READ [#канал] {номер | list | LAST | NEW}
	
	Выведет Вам текст сообщения. Если указан параметр LAST, то
	выведет последнее отправленное Вам сообщение. Если указан параметр
	NEW, выведет Вам все непрочитанные ранее сообщения. Иначе,
	выведет Вам сообщение под номером номер. Вы можете запросить сразу несколько
	сообщений, например так:
	
	   READ 2-5,7-9
	   Покажет сообщения со 2 по 5 и с 7 по 9.

MEMO_HELP_DEL
	Синтаксис: DEL [#канал] {номер | list | LAST | ALL}
	
	Удалит указанное сообщение. Вы можете указать
	несколько номеров сообщений или диапазон номеров сообщений, необязательно указывать
	только один номер, как во втором примере выше.
	
	Если указан параметр LAST, будет удалено последнее сообщение.
	Если указан параметр ALL, все Ваши сообщения будут удалены.
	
	Примеры:
	
	   DEL 1
	   Удалить сообщение под номером 1.
	
	   DEL 2-5,7-9
	   Удалить сообщения с 2 до 5 и с 7 до 9.

MEMO_HELP_SET
	Синтаксис: SET опция параметры
	
	Устанавливает различные параметры для службы сообщений. опция может быть одной из:
	
	    NOTIFY      Выбрать способ уведомления Вас
	                о новых собщениях (только для ников)
	    LIMIT       Устанавливает ограничение на количество сообщений, которое
	                Вы можете принимать
	
	Напишите /msg %S HELP SET опция для получения более подробной информации
	о конкретной опции.

MEMO_HELP_SET_NOTIFY
	Синтаксис: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Изменить способ уведомления Вас о новых сообщениях:
	
	    ON      Вы будете уведомлены о новом сообщении при входе в сеть,
	            когда Вы снимете с себя флаг /AWAY, и как только кто-то отправит Вам
	            сообщение.
	    LOGON   Вы будете уведомлены о новом сообщении только при входе в сеть,
	            или когда Вы снимете с себя флаг /AWAY.
	    NEW     Вы будете уведомлены о новом сообщении только когда они,
	            будут Вам отправлены.
	    OFF     Вы не будете уведомляться о получении новых сообщений.
	
	В принципе, ON является комбинацией LOGON и NEW.

MEMO_HELP_SET_LIMIT
	Синтаксис: SET LIMIT [#канал] limit
	
	Ограничивает максимальное количество сообщений которое Вы (или канал)
	можете принимать. Если Вы установите его равным 0, никто
	не сможет отправить Вам сообщение. Также, Вы не можете установить ограничение
	больше, чем %d.

MEMO_HELP_INFO
	Синтаксис: INFO [#канал]
	
	Показывает информацию о количестве сообщений для Вас, сколько
	из них непрочитано, и сколько всего сообщений было Вами
	принято. С параметром, покажет ту же информацию
	для данного канала.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Синтаксис: SET LIMIT [пользователь | #канал] {limit | NONE} [HARD]
	
	Ограничивает максимальное количество сообщений которое пользователь (или канал)
	может принимать. Установка лимита равным 0 не дает возможности
	принимать сообщения вообще; установка в NONE разрешает
	пользователю принимать и сохранять столько сообщений, сколько он пожелает. Если
	Вы не укажете пользователя или канал, то это будет установлено для
	Вас.
	
	Опция HARD не позволит пользователю изменить лимит. Если
	HARD не идобавить - будет противоположный эффект, т.е. пользователь сможет
	изменять лимит (даже если предыдущий лимит был установлен с параметром
	HARD).
	
	Данную команду SET LIMIT могут использовать только Администраторы
	сервисов. Другие пользователи могуть менять лимит только для себя
	или для канала, на котором у низ есть соответствующие привилегии, не могут
	удалить свой лимит, не могут установить лимит выше %d, и не могут
	установить опцию HARD.

MEMO_SERVADMIN_HELP_INFO
	Синтаксис: INFO [ник | #канал]
	
	Без параметров, покажет количество сообщений в Вашем ящике,
	сколько из них непрочитано, и сколько всего
	сообщений Вы можете принимать.
	
	Если указать канал, покажет ту же информацию
	о данном канале.
	
	Если указать ник, покажет ту же информацию
	о данном нике. Эту команду могут использовать только Администраторы
	сервисов.

MEMO_HELP_STAFF
	Синтаксис: STAFF текст сообщения

	Отправляет всем операторам/администраторам сервисов мемку, содержащую текст сообщения.
	Внимание: Если Вы прописаны дважды, например как оператор,и как 
	администратор, Вы получите это сообщение дважды. То же
	самое произойдет для прописанных как суперпользователи сервисов и 
	в то же время в других листах.

MEMO_HELP_SENDALL
	Синтаксис: SENDALL текст сообщения

	Отправляет всем зарегистрированным пользователям текст сообщения.

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

OPER_HELP
	%S commands:
	    GLOBAL      Послать сообщение всем пользователям
	    STATS       Показать статистику сервисов и сети
	    OPER LIST   Список Операторов сервисов
	    ADMIN LIST  Список Администраторов Сервисов
	    STAFF       Список всех прописанных на сервисах и их онлайн-статус

OPER_HELP_OPER_CMD
	Команды, доступные только Операторам и Администраторам сервисов и:
	    MODE        Изменяет режим на канале
	    KICK        Кикнуть пользователя с канала
	    CLEARMODES  Очистка режимов на канале
	    KILLCLONES  Прибить всех пользователей с определенного хоста
	    AKILL       Управление списком AKILL'ов
	    SGLINE      Управление списком SGLINE'ов
	    SQLINE      Управление списком SQLINE'ов
	    SZLINE      Управление списком SZLINE'ов

OPER_HELP_ADMIN_CMD
	Команды, доступные только Администраторам сервисов:
	    OPER        Управление списком Операторов сервисов
	    SVSNICK     Сменить ник пользователю
	    CHANLIST    Показать все текущие каналы
	    USERLIST    Показать всех текущих пользователей
	    LOGONNEWS   Установить сообщения при входе в сеть
	    RANDOMNEWS  Установить случайные сообщения
	                    при входе в сеть
	    OPERNEWS    Установить сообщения, показываемые при /oper
	    SESSION     Посмотреть список текущих соединений
	    EXCEPTION   Управление списком исключений из лимита сессий
	    CACHE       Список кэшированных хостов, используемых детектором прокси
	    NOOP        Временно удалить все O:лайны на указанном
	                    сервере
	    JUPE        "Джупнуть" сервер
	    IGNORE      Управление списком игноров сервисов
	    UMODE       Изменение режимов пользователя
	    OLINE       Установить флаги IRC Операторов определенному пользователю (только 
	                    UnrealIRCd)
	    SET         Установить различные опции сервисов
	    RELOAD      Перезагрузить конфигурационный файл сервисов
	    UPDATE      Заставить сервисы немедленно сохранить
	                    все базы данных на диск
	    RESTART     Сохранить базы данных и перезапустить сервисы
	    QUIT        Завершить работу сервисов без сохранения данных
	    SHUTDOWN    Завершить работу сервисов с сохранением данных
	    DEFCON      Управление системой DefCon
	    CHANKILL    Прибить всех пользователей на определенном канале

	Команды, доступные только суперпользователю сервисов:
	    ADMIN       Управление списком администраторов сервисов

OPER_HELP_ROOT_CMD
	Команды, доступны только суперпользователям сервисов:
	    MODLOAD     Загрузить модуль
	    MODUNLOAD   Выгрузить модуль
	    MODLIST     Список загруженных модулей
	    MODINFO     Информация о загруженном модуле

OPER_HELP_LOGGED
	Внимание: Все команды %S'у записываются в лог!

OPER_HELP_GLOBAL
	Синтаксис: GLOBAL сообщение
	
	Позволяет IRC Операторам отправить сообщение всем пользователям IRC сети.
	Это сообщение будет отправлено от ника %s.

OPER_HELP_STATS
	Синтаксис: STATS [AKILL | ALL | RESET]
	
	Без параметров, покажет текущее количество пользователей и
	IRC Операторов в онлайне (кроме Сервисных), максимальное число пользователей
	онлайн зарегистрированное сервисами с момента старта, и длительность
	бесперебойной работы сервисов.
	
	С опцией AKILL, покажет текущий размер списка AKILL'ов
	и время истекания AKILL'ов по умолчанию.
	
	Опция ALL доступна только для Администраторов сервисов, и
	покажет информацию об использовании памяти сервисами. Ипользование
	этой опции может затормозить сервисы на небольшой промежуток времени в
	больших сетях, не используйте эту опцию чересчур часто!
	
	Опция RESET сбрасывает счетчик маскимально зарегистрированного сервисами
	количества пользователей в онлайне.
	
	UPTIME это синоним команды STATS.

OPER_HELP_OPER
	Синтаксис: OPER ADD ник
	           OPER DEL {ник | номер записи | list}
	           OPER LIST [маска | list]
	           OPER CLEAR
	
	Позволяет Администраторам сервисов управлять списком
	Операторов сервисов. Пользователь, чей ник
	присутствует в списке Операторов сервисов, идентифицировавшись на
	%s получает доступ к командам Операторов сервисов.
	
	Команда OPER ADD добавляет указанный ник в список
	Операторов сервисов.
	
	Команда OPER DEL удаляет указанный ник из списка
	Операторов сервисов. Если указаны номера записей, 
	эти записи будут удалены. (См. пример ипользования команды LIST ниже.)
	
	Команда OPER LIST показывает список Операторов сервисов.
	Если указана символьная маска, только соответствующие ей
	записи будут показаны. Если указаны номера записей,
	только эти записи будут показаны; например:

	   OPER LIST 2-5,7-9
	   Покажет Операторов сервисов с номерами записей от 2 до
	   5 и от 7 до 9.
	      
	Команда OPER CLEAR удаляет все записи из списка
	Операторов сервисов.
	
	Любой IRC Оператор может использовать команду OPER LIST.
	Все остальные команды могут использовать только Администраторы сервисов.

OPER_HELP_ADMIN
	Синтаксис: ADMIN ADD ник
	           ADMIN DEL {ник | номер записи | list}
	           ADMIN LIST [маска | list]
	           ADMIN CLEAR
	
	Позволяет суперпользователю сервисов добавлять или удалять ники
	из списка Администраторов сервисов. Пользователь, чей ник
	находится с списке Администраторов сервисов и который идентифицировался
	%s получае доступ к командам Администраторов сервисов.
	
	Команда ADMIN ADD добавляет указанный ник в список
	администраторов сервисов.
	
	Команда ADMIN DEL удаляет указанный ник из списка
	администраторов сервисов. Если указать номер(а) записи(записей),
	все они будут удалены. (См. пример использования команды LIST ниже.)
	
	Команда ADMIN LIST показывает список администраторов сервисов.  
	Если указать символьную маску, только подпадающие под нее значения
	будут показаны. Если указать номера записей, то
	будут показаны только они; например:

	   ADMIN LIST 2-5,7-9
	      Покажет администраторов сервисов с номерами записей от 2 до
	      5 и от 7 до 9.
	      
	Команда ADMIN CLEAR удалит все записи из списка
	администраторов сервисов.
	
	Любой IRC оператор может использовать команду ADMIN LIST.
	Остальное может использоваеть только Суперпользователь сервисов.

OPER_HELP_IGNORE
	Синтаксис: IGNORE {ADD|DEL|LIST|CLEAR} [время] [ник]

	Позволяет Администраторам сервисов управлять списком игнорируемых сервисами ников
	на указанное время или до перезапуска сервисов. По умолчанию формат указания времени
	игнорирования - секунды. Вы можете также указать иные единицы измерения, используя:
	s секунд, m минут, h часов и
	d дней. Комбинации этих единиц измерения не допустимы. Чтобы сделать
	постоянный игнор пользователя, установите время игнора равным 0.

OPER_HELP_MODE
	Синтаксис: MODE #канал режимы
	
	Позволяет Операторам сервисов изменять режимы канала на любом
	канале. Параметры данной команды такие же, что и при использовании
	команды /MODE.
	
	Эту команду могут использовать только Операторы сервисов.

OPER_HELP_UMODE
	Синтаксис: UMODE пользователь режимы
	
	Позволяет Администраторам сервисов изменять пользовательские режимы для любого пользователя.
	Параметры данной команды такие же, что и при использовании
	команды /MODE.
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_OLINE
	Синтаксис: OLINE пользователь флаги
	
	Позволяет Администраторам сервисов установить флаги IRC Операторов любому пользователю.
	Перед флагами должен присутствовать префикс "+" или "-". Чтобы
	удалить все флаги, используйте "-" вместо любого флага.

	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_CLEARMODES
	Синтаксис: CLEARMODES #канал [ALL]
	
	Очищает все двоичные режимы (i,k,l,m,n,p,s,t) и баны на указанном
	канале. Если указан параметр ALL, также снимет со всех статус оператора канала,
	войсы (режимы +o и +v) на канале.
	
	Эту команду могут использовать только Операторы сервисов.

OPER_HELP_KICK
	Синтаксис: KICK #канал пользователь причина
	
	Позволяет IRC Операторам выкинуть пользователя с любого канала.
	Параметры данной команды такие же, что и при испоьзовании команды /KICK.
	Сообщение кика будет включать имя IRC Оператора, использовавшего команду
	и указанную им причину, например:
	
	*** SpamMan has been kicked off channel #my_channel by %S (Alcan (Flood))
	
	Эту команду могут использовать только Операторы сервисов.

OPER_HELP_SVSNICK
	Синтаксис: SVSNICK ник новый ник
	
	Изменить ник на новый ник.
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_AKILL
	Синтаксис: AKILL ADD [+время истечения] маска причина
	           AKILL DEL {маска | номер записи | list}
	           AKILL LIST [маска | list]
	           AKILL VIEW [маска | list]
	           AKILL CLEAR
	
	Позволяет Операторам сервисов управлять списком AKILL'ов. Если
	пользователь, подпадающий под маску AKILL'а попытается приконнектиться, сервисы
	используют на него KILL, и, Если Ваш IRC сервер это поддерживает,
	установит на всех серверах IRC сети K-line на
	маску данного пользователя.
	
	AKILL ADD добавляет маску вида user@host/ip в список AKILL'ов
	с указанной причиной (которая обязательно должна быть указана).
	время истечения - это целое число, которое может быть одним из: d 
	(дней), h (часов), или m (минут). Сочетания (то есть, к примеру
	1h30m) не допускаются. Если единица измерения не указана,
	по умолчанию она будет установлена в "дни" (таким образом +30 будет означать 30 
	дней). Чтобы добавить вечный AKILL, используйте время истечения равным +0. Если
	маска пользователя to была добавлена, начиная с +, время истечения
	будет задано по умолчанию. Текущее время AKILL'а
	по умолчанию можно узнать командой
	STATS AKILL.
	
	Команда AKILL DEL удаляет указанную маску из списка
	AKILL'ов. Если указаны номера записей,
	эти записи будут удалены. (См. пример испоьзования команды LIST 
	ниже.)
	
	Команда AKILL LIST показывает текущий список AKILL'ов.  
	Если указать символьную маску, будут показаны только совпадающие с ней
	значения. Если указать номера записей, только,
	эти записи будут показаны; например:

	   AKILL LIST 2-5,7-9
	   Список AKILL'ов с номерами записей со 2 до 5 и с 7 
	   по 9.
	      
	AKILL VIEW покажет более подробную информацию, чем AKILL LIST, и
	покажет также кто добавил AKILL, время установления AKILL'а, когда
	он истекает, ну и конечно же маску вида user@host/ip и причину AKILL'а.
	
	AKILL CLEAR удаляет все записи из списка AKILL'ов.
	
	Эту команду могут использовать только Операторы сервисов.
	
OPER_HELP_SGLINE
	Синтаксис: SGLINE ADD [+время истечения] маска:причина
	    	   SGLINE DEL {маска | номер записи | list}
	           SGLINE LIST [маска | list]
	           SGLINE VIEW [маска | list]
	           SGLINE CLEAR
	
	Позволяет Операторам сервисов управлять списком SGLINE'ов. Если
	пользователь с реальным именем, подпадающим под маску SGLINE'апопытается
	приконнектиться, сервисы не разрешат ему установить
	соединение.
	
	SGLINE ADD добавляет маску реального имени в список SGLINE'ов
	с указанной причиной (которая обязательно должна быть указана).
	время истечения - это целое число, которое может быть одним из: d 
	(дней), h (часов), или m (минут). Сочетания (то есть, к примеру
	1h30m) не допускаются. Если единица измерения не указана,
	по умолчанию она будет установлена в "дни" (таким образом +30 будет означать 30 
	дней). Чтобы добавить вечный SGLINE, используйте время истечения равным +0. Если
	маска пользователя to была добавлена, начиная с +, время истечения
	будет задано по умолчанию. Текущее время SGLINE'а
	по умолчанию можно узнать командой
	STATS AKILL.
	Примечание: так как маски реальных имен могут содержать пробелы,
	разделителем между реальным именем и причиной будет двоеточие.
	
	Команда SGLINE DEL удаляет указанную маску из списка
	SGLINE'ов. Если указаны номера записей,
	эти записи будут удалены. (См. пример испоьзования команды LIST 
	ниже.)
	
	Команда SGLINE LIST показывает текущий список SGLINE'ов.  
	Если указать символьную маску, будут показаны только совпадающие с ней
	значения. Если указать номера записей, только,
	эти записи будут показаны; например:

	   SGLINE LIST 2-5,7-9
	   Список SGLINE'ов с номерами записей со 2 до 5 и с 7 
	   по 9.
	      
	SGLINE VIEW покажет более подробную информацию, чем SGLINE LIST, и
	покажет также кто добавил SGLINE, время установления SGLINE'а, когда
	он истекает, ну и конечно же маску реального имени и причину SGLINE'а.
	
	SGLINE CLEAR удаляет все записи из списка SGLINE'ов.
	
	Эту команду могут использовать только Операторы сервисов.

OPER_HELP_SQLINE
	Синтаксис: SQLINE ADD [+время истечения] маска:причина
	    	   SQLINE DEL {маска | номер записи | list}
	           SQLINE LIST [маска | list]
	           SQLINE VIEW [маска | list]
	           SQLINE CLEAR
	
	Позволяет Операторам сервисов управлять списком SQLINE'ов. Если
	пользователь с ником, подпадающий под маску SQLINE'а попытается
	приконнектиться, сервисы не разрешат ему установить
	соединение.
	
	Если первый символ маски #, сервисы не разрешат
	использовать подпадающие под маску каналы (на IRC серверах, которые
	это поддерживают).
	
	SQLINE ADD добавляет указанную маску в список SQLINE'ов
	с указанной причиной (которая обязательно должна быть указана).
	время истечения - это целое число, которое может быть одним из: d 
	(дней), h (часов), или m (минут). Сочетания (то есть, к примеру
	1h30m) не допускаются. Если единица измерения не указана,
	по умолчанию она будет установлена в "дни" (таким образом +30 будет означать 30 
	дней). Чтобы добавить вечный SQLINE, используйте время истечения равным +0. Если
	маска пользователя to была добавлена, начиная с +, время истечения
	будет задано по умолчанию. Текущее время SQLINE'а
	по умолчанию можно узнать командой
	STATS AKILL.
	
	Команда SQLINE DEL удаляет указанную маску из списка
	SGLINE'ов. Если указаны номера записей,
	эти записи будут удалены. (См. пример испоьзования команды LIST 
	ниже.)
	
	Команда SQLINE LIST показывает текущий список SQLINE'ов.  
	Если указать символьную маску, будут показаны только совпадающие с ней
	значения. Если указать номера записей, только,
	эти записи будут показаны; например:

	   SQLINE LIST 2-5,7-9
	   Список SQLINE'ов с номерами записей со 2 до 5 и с 7 
	   по 9.
	      
	SQLINE VIEW покажет более подробную информацию, чем SQLINE LIST, и
	покажет также кто добавил SQLINE, время установления SQLINE'а, когда
	он истекает, ну и конечно же маску реального имени и причину SQLINE'а.
	
	SQLINE CLEAR удаляет все записи из списка SQLINE'ов.
	
	Эту команду могут использовать только Операторы сервисов.

OPER_HELP_SZLINE
	Синтаксис: SZLINE ADD [+время истечения] маска причина
	           SZLINE DEL {маска | номер записи | list}
	           SZLINE LIST [маска | list]
	           SZLINE VIEW [маска | list]
	           SZLINE CLEAR
	
	Позволяет Операторам сервисов управлять списком SZLINE'ов. Если
	пользователь с IP адресом, подпадающим под маску SZLINE'а попытается 
	приконнектиться, сервисы не разрешат ему установить
	соединение (неважно, есть ли для этого IP запись PTR RR или нет).
	
	SZLINE ADD добавляет IP маску в список SZLINE'ов
	с указанной причиной (которая обязательно должна быть указана).
	время истечения - это целое число, которое может быть одним из: d 
	(дней), h (часов), или m (минут). Сочетания (то есть, к примеру
	1h30m) не допускаются. Если единица измерения не указана,
	по умолчанию она будет установлена в "дни" (таким образом +30 будет означать 30 
	дней). Чтобы добавить вечный SGLINE, используйте время истечения равным +0. Если
	маска пользователя to была добавлена, начиная с +, время истечения
	будет задано по умолчанию. Текущее время SZLINE'а
	по умолчанию можно узнать командой
	STATS AKILL.
	
	Команда SZLINE DEL удаляет указанную маску из списка
	SZLINE'ов. Если указаны номера записей,
	эти записи будут удалены. (См. пример испоьзования команды LIST 
	ниже.)
	
	Команда SZLINE LIST показывает текущий список SZLINE'ов.  
	Если указать символьную маску, будут показаны только совпадающие с ней
	значения. Если указать номера записей, только,
	эти записи будут показаны; например:

	   SZLINE LIST 2-5,7-9
	   Список SZLINE'ов с номерами записей со 2 до 5 и с 7 
	   по 9.
	      
	SZLINE VIEW покажет более подробную информацию, чем SZLINE LIST, и
	покажет также кто добавил SZLINE, время установления SZLINE'а, когда
	он истекает, ну и конечно же маску реального имени и причину SZLINE'а.
	
	SZLINE CLEAR удаляет все записи из списка SZLINE'ов.
	
	Эту команду могут использовать только Операторы сервисов.

OPER_HELP_SET
	Синтаксис: SET опция параметр

	Устанавливает различные глобальные опции сервисов. Опции
	такие:
	    READONLY   Установить режим только-чтение или чтение-запись
	    LOGCHAN    Вести лог сервисов в канал
	    DEBUG      Активировать/деактивировать режим отладки
	    NOEXPIRE   Активировать/деактивировать режим не-истечения
	    SUPERADMIN Активировать/деактивировать режим Суперадмина

	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_SET_READONLY
	Синтаксис: SET READONLY {ON | OFF}
	
	Включает или выключает режим только-чтения. В режиме только-чтения
	обычные пользователи не могу изменять любые данные,
	включая списки доступа для ников и каналов, и многое другое. IRC Операторы
	с достаточными привилегиями на сервисах могут модифицировать список
	AKILL'ов и удалять или запрещать ники и
	каналы, но все эти изменения не будут сохранены пока режим
	только-чтения не будет деактивирован либо сервисы не завершат работу
	или не перезапустятся.
	
	Эта опция является эквивалентом параметра командной строки
	-readonly.

OPER_HELP_SET_LOGCHAN
	Синтаксис: SET LOGCHAN {ON | OFF}

	С включенным LOHCHAN, сервисы будут не только записывать все сообщения в лог, но и отправлять их
	на специальный канал. Этот канал должен быть определен в
	конфигурационном файле сервисов чтобы эта опция могла быть
	использована.

	Примечание: Из соображений безопасности, рекомендуется обратить особое
	внимание на безопасность доступа на этот канал.

OPER_HELP_SET_DEBUG
	Синтаксис: SET DEBUG {ON | OFF | номер}
	
	Включает/выключает режим отладки. В режиме отладки, данные, которыми сервисы
	обмениваются с IRC сервером, количество данных зависит от уровня (номера) отладки
	будут записаны в лог. Если указать номер,
	режим отладки активируется с уровнем отладки, установленным в
	номер.

	Эта опция является эквивалентом параметра командной строки
	-debug.

OPER_HELP_SET_NOEXPIRE
	Синтаксис: SET NOEXPIRE {ON | OFF}

	Включает/выключает режим не-истекания. В этом режиме ники,
	каналы, AKILL'ы и исключения никогда не истекут, пока опция
	не будет отключена.

	Эта опция является эквивалентом параметра командной строки
	-noexpire.

OPER_HELP_SET_SUPERADMIN
	Синтаксис: SET SUPERADMIN {ON | OFF}

	Эта опция позволяет Вам получить супер привилегии - например такие как
	"владелец" на всех каналах и другое...

	Эта опция не постоянная, и должна использоваться только при крайней
	необходимости, установите ее в OFF как только перестанете нуждаться в ней.


OPER_HELP_NOOP
	Синтаксис: NOOP SET сервер
	           NOOP REVOKE сервер

	NOOP SET удаляет все O:lines на указанном
	сервере и прибивает всех IRC Операторов, находящихся на нем,
	чтобы не дать им возможность рехашнуть сервер (так как это
	отменит эффект данной команды).

	NOOP REVOKE делает все ранее удаленные O:lines доступными
	для использования на сервере.

	Примечание: сервер не будет полностью ограничен
	сервисами.
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_JUPE
	Синтаксис: JUPE сервер [причина]
	
	Джупнуть сервисы -- это создаст
	фальшивый "сервер" приконнекченный к сервисам, который
	не даст реальному серверу с этим именем приконнектиться к сети. Джуп
	может быть удален, используя стандартный SQUIT. Если указана причина,
	она будет показана в информации, выдаваемой коннектящемуся серверу;
	в ином случае, эта информационная строка будет содержать текст
	"Juped by <ник>", то есть покажет человека, который
	джупнул данный сервер.
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_RAW
	Синтаксис: RAW текст
	
	Отправляет текстовую строку напрямую на IRC сервер, к которому
	прилинкованы сервисы. Эта команда имеет очень ограниченное
	использование, она может вызвать фатальную ошибку в сети при
	неверном испоьзовании. НЕ ИСПОЛЬЗУЙТЕ ЭТУ КОМАНДУ без
	абсолютной уверенности в том, что Вы делаете!
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_UPDATE
	Синтаксис: UPDATE
	
	Заставляет сервисы немедленно обновить файды баз данных
	на диске.
	
	Эту команду могут использовать только Администраторы сервисов.
	
OPER_HELP_RELOAD
	Синтаксис: RELOAD
	
	Заставляет сервисы перечитать конфигурационный файл. Примечание:
	некоторые вещи требуют перезапуска сервисов.
	(например изменение ников псевдоклиентов сервисов, активация
	лимитирования сессий, и т.д.)
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_QUIT
	Синтаксис: QUIT
	
	Заставляет сервисы немедленно завершить свою работу; базы данных
	не будут сохранены. Эта команда не должна использоваться кроме случаев
	повреждения базы данных в памяти, чтобы поврежденная база
	данных не была сохранена. Для нормального завершения работы используйте
	команду SHUTDOWN.
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_SHUTDOWN
	Синтаксис: SHUTDOWN
	
	Заставляет сервисы сохранить все базы данных и завершить работу.
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_RESTART
	Синтаксис: RESTART
	
	Заставляет сервисы сохранить все базы данных и перезапуститься
	(т.е. завершить работу и немедленно снова запустить сервисы на исполнение).
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_KILLCLONES
	Синтаксис: KILLCLONES ник
	
	Прибивает всех пользователей, хост которых такой же, как у указанного ника.
	Добавляется временный AKILL, в виде *@host, чтобы предотвратить
	немедленный реконнект прибитых клиентов.
	Также при использовании этой команды будет отправлен wallop,
	который сообщит о прибитом хосте и количестве прибитых клиентов.
	Эту команду очень удобно применять, чтобы убрать
	клонов из IRC сети.
	
	Эту команду могут использовать только Операторы сервисов.
	
OPER_HELP_CHANLIST
	Синтаксис: CHANLIST [{символная маска | ник} [SECRET]]
	
	Показывает все созданные на данный момент каналы в IRC сети, неважно
	зарегистрированы они или нет.
	
	Если указана символьная маска, покажет только те каналы, которые совпадают с ней. Если указан ник,
	покажет только те каналы, на которых находится этот ник. Если указан параметр SECRET,
	покажет только каналы с указанной символьной маской на которых
	стоит режим +s или +p.
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_USERLIST
	Синтаксис: USERLIST [{символьная маска | #канал} [INVISIBLE]]
	
	Показать всех пользователей, находящихся в данный момент в IRC сети, без разницы
	зарегистрирован у них ник или нет.
	
	Если указана символьная маска, покажет только пользователей совпадающих с ней (маска должна быть
	в формате nick!user@host). Если указан #канал, покажет только пользователей,
	находящихся на этом канале. Если указан пареметр INVISIBLE будут показаны
	только те пользователи, на которых стоит флаг +i.
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_CACHE
	Синтаксис: CACHE DEL хост
	           CACHE LIST символьная маска [QUEUED | ALL]
	
	Команда CACHE DEL удаляет указанный хост из
	кэша. Эта команда предназначена только для отладки.

	Команда CACHE LIST показывает списко всех найденных прокси, подпадающих под
	указанную символьную маску. Если указать параметр QUEUED, то будет показана
	очередь сканов, и если указать параметр ALL,
	то будут показаны все кэшированные хосты.
	
	Эту команду могут использовать только Администраторы сервисов.

OPER_HELP_MODLOAD
	Синтаксис: MODLOAD ИмяФайла

	Эта команда загрузит указанный модуль из директории
	с модулями.

	Эту команду могут использовать только Суперпользователи сервисов.

OPER_HELP_MODUNLOAD
	Синтаксис: MODUNLOAD ИмяФайла

	Эта команда загрузит указанный модуль из директории
	с модулями.

	Эту команду могут использовать только Суперпользователи сервисов.

OPER_HELP_MODINFO
	Синтаксис: MODINFO ИмяФайла

	Эта команда покажет информацию об указанном модуле

	Эту команду могут использовать только Суперпользователи сервисов.

OPER_HELP_MODLIST
	Синтаксис: MODLIST

	Список всех загруженных модулей

	Эту команду могут использовать только Суперпользователи сервисов.


###########################################################################
#
# BotServ help messages
#
###########################################################################
	
BOT_HELP
	%S позволяет Вам повесить бота на свой канал.
	Это сделано для тех, кто не может иначе установить или
	настроить бота, или если использование ботов запрещено
	в Вашей IRC сети. Доступные команды перечислены ниже;
	чтобы использовать их, напишите /msg %S команда. Для получения 
	более подробной информации по конкретной команде, напишите /msg 
	%S HELP команда.
	
	BOTLIST            Список доступных для установки ботов
	ASSIGN             Установить бота на канал
	SET                Конфигурировать настройки бота
	KICK               Конфигурировать за что бот будет кикать и банить
	BADWORDS           Управление списком плохих слов
	
	Другие команды: ACT INFO SAY UNASSIGN
	
	Бот присоединится к каналу после того, как на нем окажется
	не менее %d пользователей.
	
BOT_HELP_BOTLIST
	Синтаксис: BOTLIST
	
	Покажет всех доступных для установки ботов.
	
BOT_HELP_ASSIGN
	Синтаксис: ASSIGN #канал ник
	
	Установит бота под указанным ником на указанный канал. Теперь
	Вы можете настраивать опции бота так, как Вам
	необходимо.  
	
BOT_HELP_UNASSIGN
	Синтаксис: UNASSIGN #канал
	
	Убирает бота с указанного канала. При использовании этой команды
	бот больше не будет заходить на канал. Тем не менее, конфигурация
	бота для данного канала будет сохранена, поэтому если Вы позднее
	снова установите этого бота на канал, все его настройки будут
	восстановлены.
	
BOT_HELP_INFO
	Синтаксис: INFO {#канал | ник}
	
	ПОзволяет Вам увидеть информацию %S о канале или о боте.
	Если указать #канал, то Вы получите информацию о канале
	и настройках киков. Если указать ник,
	Вы получите информацию о боте, когда бот был создан
	и число каналов на котором он находится.
	
BOT_HELP_SET
	Синтаксис: SET #канал опция параметры
	
	Конфигурация опций бота. Настраиваемые опции:
	
	    DONTKICKOPS      Не кикать операторов
	    DONTKICKVOICES   Не кикать войсов
	    GREET            Включить сообщение приветствий
	    FANTASY          Включить режим Фантазии
	    SYMBIOSIS        Позволить боту кикать/банить/менять режимы, используя ChanServ
	
	Напишите /msg %S HELP SET опция для более подробной информации
	о конкретной опции.
	
	Примечание: доступ к этой команде может быть изменен через
	level SET.
	
BOT_HELP_SET_DONTKICKOPS
	Синтаксис: SET #канал DONTKICKOPS {ON|OFF}
	
	Включает или выключает защиту операторов канала на указанном канале.
	Если включено, операторы не будут кикнуты ботом
	даже если их уровень доступа не соответствует уровню NOKICK.
	
BOT_HELP_SET_DONTKICKVOICES
	Синтаксис: SET #канал DONTKICKVOICES {ON|OFF}
	
	Включает или выключает защиту войсов на указанном канале.
	Если включено, войсы не будут кикнуты ботом
	даже если их уровень доступа не соответствует уровню NOKICK.
	
BOT_HELP_SET_FANTASY
	Синтаксис: SET #канал FANTASY {ON|OFF}
	
	Включает или выключает режим Фантазии на канале.
	Если включено, пользователи могут использовать
	команды !op, !deop, !voice, !devoice,
	!kick, !kb, !unban, !seen на канале (можно использовать,
	указывая ник (необязательно), 
	или причину (для команд !kick, !kb)).
	
	Примечание: чтобы использовать команды Фантазии,
	пользователи ДОЛЖНЫ иметь соответствующий уровень доступа
	к командам Фантазии и другие привилегии,
	требуемые для этих команд (например, чтобы использовать команду
	!op, пользователь должен иметь права на использование команд 
	OP/DEOP).

BOT_HELP_SET_GREET
	Синтаксис: SET #канал GREET {ON|OFF}
	
	Включает или выключает режим приветствий на канале.
	Если включено, бот будет показывать приветственные сообщения
	для пользователей, заходящих на канал и имеющих
	достаточный уровень доступа.

BOT_HELP_SET_SYMBIOSIS
	Синтаксис: SET #канал SYMBIOSIS {ON|OFF}
	
	Включает или выключает режим симбиоза на канале.
	Если включено, бот будет выполнять многие действия
	на канале через %s, например изменение режимов канала,
	кики, и др.

BOT_HELP_KICK
	Синтаксис: KICK #канал опция параметры
	
	Конфигурировать за что бот будет кикать и банить. Доступные опции:
	
	    BOLDS         Будет ли бот кикать за выделение жирным шрифтом
	    BADWORDS      Будет ли бот кикать за плохие слова
	    CAPS          Будет ли бот кикать за использование CapsLOCK
	    COLORS        Будет ли бот кикать за выделение цветами
	    FLOOD         Будет ли бот кикать за флуд
	    REPEAT        Будет ли бот кикать за
	                       повторы
	    REVERSES      Будет ли бот кикать за реверсы
	    UNDERLINES    Будет ли бот кикать за выделение подчеркиванием
	
	Напишите /msg %S HELP KICK опция для получения более полной
	информации о конкретной опции.
	
	Примечание: доступ к этой команде может быть изменен через
	level SET.

BOT_HELP_KICK_BOLDS
	Синтаксис: KICK #канал BOLDS {ON|OFF} [ttb]

	Будет ли бот кикать за выделение жирным шрифтом.
	Если включено - будет.

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_COLORS
	Синтаксис: KICK #канал COLORS {ON|OFF} [ttb]

	Будет ли бот кикать за выделение цветами.
	Если включено - будет.

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_REVERSES
	Синтаксис: KICK #канал REVERSES {ON|OFF} [ttb]

	Будет ли бот кикать за реверсы.
	Если включено - будет.

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_UNDERLINES
	Синтаксис: KICK #канал UNDERLINES {ON|OFF} [ttb]

	Будет ли бот кикать за выделение подчеркиванием.
	Если включено - будет.

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_CAPS
	Синтаксис: KICK #канал CAPS {ON|OFF} [ttb [минимум [проценты]]]

	Будет ли бот кикать за использование CapsLOCK.
	Если включено - бот будет кикать тех, кто любит разговаривать
	большими буквами.

	Бот кикнет пользователя, если он использует минимум больших букв,
	количество которых составит проценты%% от общего количества
	букв (Если не указаны, то значения по умолчанию 10 символов
	и 25%%).

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_FLOOD
	Синтаксис: KICK #канал FLOOD {ON|OFF} [ttb [линий [секунды]]]

	Будет ли бот кикать за флуд. Если включено,
	бот будет кикать флудеров с канала,
	если они сказали более линий строк текста за секунды секунд
	(Если не указано, значения по умолчанию 6 строк за 10 секунд).
	
	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_REPEAT
	Синтаксис: KICK #канал REPEAT {ON|OFF} [ttb [число повторов]]

	Будет ли бот кикать за повторы. Если включено,
	бот будет кикать пользователей, которые повторяют один и тот же
	текст число повторов раз (Если не указано, значение по умолчанию
	равно 3).

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_BADWORDS
	Синтаксис: KICK #канал BADWORDS {ON|OFF} [ttb]

	Будет ли бот кикать за плохие слова. Если включено,
	бот будет кикать пользователей, использующих плохие слова
	на канале.

	Вы можете установить плохие слова на канале, используя
	команду BADWORDS. Напишите /msg %S HELP BADWORDS для
	получения более подробной информации.

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.
	
BOT_HELP_BADWORDS
	Синтаксис: BADWORDS #канал ADD слово [SINGLE | START | END]
	           BADWORDS #канал DEL {слово | номер записи | list}
	           BADWORDS #канал LIST [маска | list]
	           BADWORDS #канал CLEAR
	
	Управляет списком плохих слов для канала. Список плохих слов
	определяет за использование каких слов пользователь будет кикнут,
	при условии что данная возможность включена. Для получения более полной информации,
	напишите /msg %S HELP KICK BADWORDS.
	
	Команда BADWORDS ADD добавит указанное слово в список
	плохих слов. Если указан параметр SINGLE, пользователь будет
	кикнут, только если польностью произнесет это слово. Если указать параметр START,
	пользователь будет кикнут, когда произнесет слово, начинающееся
	на слово. Если указать параметр END, то кик будет произведен,
	если пользователь произнесет слово, заканчивающееся на
	слово. Если Вы не укажете ни один из этих параметров, любой пользователь
	будет кикнут всякий раз по произнесении слова.
	
	Команда BADWORDS DEL удаляет указанное слово из списка плохих слов.
	Если указать номера записей, указанные записи
	будут удалены. (См. пример использования команды LIST ниже.)
	
	Команда BADWORDS LIST покажет текущий список плохих слов. Если указать
	символьную маску, только совпадающие с ней слова будут
	показаны. Если указать номера записей, будут показаны только
	указанные записи; например:

	   BADWORDS #channel LIST 2-5,7-9
	   Покажет плохие слова с номерами записей со 2 по 5 и с
	   7 по 9.
	      
	Команда BADWORDS CLEAR удалит все плохие слова
	из списка.

BOT_HELP_SAY
	Синтаксис: SAY #канал текст
	
	Заставляет бота произнести указанный текст на указанный канал.

BOT_HELP_ACT
	Синтаксис: ACT #канал текст
	
	Заставляет бота выполнить команду, эквивалентную команду "/me"
	на указанном канале, использовав указанный текст.

BOT_SERVADMIN_HELP
	
	Нижеследующие команды доступны для Администраторов сервисов:
	
	    BOT              Управление ботами

BOT_SERVADMIN_HELP_BOT
	Синтаксис: BOT ADD ник идент хост реальное имя
	           BOT CHANGE старый ник новый ник [идент [хост [реальное имя]]]
	           BOT DEL ник
	           BOT LIST
	
	Позволяет Администраторам сервисов создавать, изменять и удалять
	ботов, которые могут быть в дальнейшем использованы пользователями
	на своих каналах.
	
	BOT ADD добавляет бота с указанным ником, идентом,
	хостом и реальным именем. Так как корректность этих значений
	не проверяется, будьте крайне осторожны!
	BOT CHANGE позволяет изменять ник, идент, хост
	или реальное имя бота без удаления его (и всех
	его настроек).
	BOT DEL удаляет указанного бота из списка ботов.  
	BOT LIST или BOTLIST покажет список
	всех доступных в сети ботов.
	
	Примечание: Если Вы создали бота с ником, ранее кем-то зарегистрированным,
	этот ник будет удален. А также, если кто-то использует данный ник
	в момент создания бота, он будет прибит.

BOT_SERVADMIN_HELP_SET
	
	Опции, зарезервированные для использования Администраторами сервисов:
	
	    NOBOT            Запретить использование ботов
	                     на каком-либо канале
	    PRIVATE          Запретить использование бота кем-либо, кроме
	                     IRC Операторов

BOT_SERVADMIN_HELP_SET_NOBOT
	Синтаксис: SET #канал NOBOT {ON|OFF}
	
	Эта опция запрещает использовать ботов на канале. Если бот
	уже установлен на канал, он будет автоматически удален с него
	сразу после включения данной опции.
	
	Эту команду могут использовать только Администраторы сервисов.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Синтаксис: SET ник бота PRIVATE {ON|OFF}
	
	Эта опция запрещает устанавливать бота на канал обычным пользователям.
	Такого бота могут установить только IRC Операторы.
	
	Эту команду могут использовать только Администраторы сервисов.

###########################################################################
#
# HostServ messages
#
###########################################################################
HOST_EMPTY
	Лист виртуальных хостов пуст.
HOST_ENTRY
	#%d Ник:%s, виртуальный хост:%s (%s - %s)
HOST_IDENT_ENTRY
	#%d Ник:%s, виртуальный хост:%s@%s (%s - %s)
HOST_SET
	Виртуальный хост для %s установлен в %s.
HOST_IDENT_SET
	Виртуальный хост %s установлен в %s@%s.
HOST_SETALL
	Виртуальных хост для группы %s установлен в %s.
HOST_DELALL
	Виртуальные хосты для группы %s были удалены.
HOST_DELALL_SYNTAX
	Синтаксис: DELALL <ник>.
HOST_IDENT_SETALL
	Виртуальный хост для группы %s установлен в %s@%s.
HOST_SET_ERROR
	Виртуальный хост должен быть в правильном формате или действительной хостмаской.
HOST_SET_IDENT_ERROR
	Идент виртуального хоста должен быть в правильном формате.
HOST_SET_TOOLONG
	Ошибка! Виртуальный хост слишком длинный, пожалуйста используйте хост длиной меньше %d символов.
HOST_SET_IDENTTOOLONG
	Ошибка! Идент слишком длинный, пожалуйста используйте идент длиной меньше %d символов.
HOST_NOREG
	Пользователь %s не найден в базе данных NickServ.
HOST_SET_SYNTAX
	Ситаксис: /msg %s set <ник> <хостмаска>.
HOST_SETALL_SYNTAX
	Синтаксис /msg %s setall <ник> <хостмаска>.
HOST_DENIED
	ДОступ запрещен.
HOST_NOT_ASSIGNED
	Пожалуйста обратитесь к IRC Оператору, чтобы получить виртуальный хост.
HOST_ACTIVATED
	Ваш виртуальный хост %s активирован.
HOST_IDENT_ACTIVATED
	Ваш виртуальный хост %s@%s активирован.
HOST_ID
	Сначала, пожалуйста, идентифицируйте свой ник.
HOST_NOT_REGED
	Ваш ник должен быть зарегистрирован, чтобы Вы смогли использовать виртуальный хост.
HOST_DEL
	Виртуальный хост для %s удален.
HOST_DEL_SYNTAX
	Синтаксис: /msg %s del <ник>.
HOST_OFF_UNREAL
	Ваш виртуальный хост удален. чтобы снова установить его на Вас, напишите /mode %s +x
HOST_NO_VIDENT
	Ваш IRC сервер не поддерживает виртуальные иденты, если это не так, пожалуйста обратитесь к нас, это может быть ошибка в программе.
HOST_GROUP
	Все виртуальные хосты в группе %s были установлены в %s
HOST_IDENT_GROUP
	Все виртуальные хосты в группе %s были установлены в %s@%s
HOST_LIST_FOOTER
	Показывает все записи (Count: %d)
HOST_LIST_RANGE_FOOTER
	Показывает записи с %d по %d
HOST_LIST_KEY_FOOTER
	Показывает записи, содержащие ключ %s (Счетчик: %d)

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_HELP
	%S commands:
	    ON          Активирует назначенный Вам виртуальный хост
	    OFF         Деактивирует виртуальный хост
	    GROUP       Устанавливает и синхронизирует виртуальный хост для всех ников в Вашей группе

HOST_OPER_HELP
	Команды, доступные только Операторам хостов (setter/removers):
	    SET         Установить вирутальный хост пользователю
	    SETALL      Установить виртуальный хост для всех нков в группе
	    DEL         Удалить виртуальный хост пользователя
	    DELALL      Удалить виртуальные хосты для всех ников в группе

HOST_ADMIN_HELP
	Команды, доступные только Администраторам сервисов:
	    LIST        Покажет список виртуальных хостов.

HOST_HELP_ON
	Синтаксис: ON

	Активирует виртуальный хост, назначенный Вашему нику.
	Когда Вы испоьзуете эту команду любой пользователь, сделавший /whois
	на Вас увидит виртуальный хост вместо Вашего реального IP адреса.

HOST_HELP_SET
	Синтаксис: SET <ник> <хостмаска>.

	Устанавливает виртуальный хост на указанный ник в указанную
	хостмаску. Если Ваш IRC сервер подерживает виртуальные иденты, можно использовать
	SET <ник> <идент>@<хостмаска> для установки идента и
	виртуального хоста одновременно.

	Эту команду могут использовать только Операторы хостов(setters).

HOST_HELP_DELALL
	Синтаксис: DELALL <ник>.

	Удаляет виртуальные хосты для всех ников в группе
	указанного ника.

	Эту команду могут использовать только Операторы хостов(removers).

HOST_HELP_SETALL
	Синтаксис: SETALL <ник> <хостмаска>.

	Устанавливает виртуальный хост для всех ников в группе
	указанного ника. Если Ваш IRC сервер поддерживает виртуальные иденты, Вы можете
	использовать SETALL <ник> <идент>@<хостмаска> чтобы установить идент и
	виртуальный хост одновременно.

	* Примечание, виртуальный хост НЕ будет обновляться автоматически для любого
	ника, добавленного в группу после использования этой команды.

	Эту команду могут использовать только Операторы хостов(setters).

HOST_HELP_OFF
	Синтаксис: OFF

	Деактивирует виртуальный хост, назначенный Вашему нику.
	Когда Вы испоьзуете эту команду любой пользователь, сделавший /whois
	на Вас увидит Ваш реальный IP адрес.

HOST_HELP_DEL
	Синтаксис: DEL <ник>

	Удаляет виртуальный хост, назначенный указанному нику из
	базы данных.

	Эту команду могут использовать только Операторы хостов.

HOST_HELP_LIST
	Синтаксис: LIST [<ключ>|<#X-Y>]

	Эта команда показывает зарегистрированные виртуальные хосты оператору.
	Если указан Ключ, то будут показаны только те записи, которые
	содержат указанный шаблон, т.е. Rob* для всех записей,
	начинающихся с "Rob"

	Если использован стиль #X-Y, будут показаны записи от X
	до Y, т.е. #1-3 покажет первые 3
	записи ник/виртуальный хост.

	Лимит одновременно показываемых значений ограничен числом NSListMax
	в конфигурационном файле сервисов.

	Эту команду могут использовать только Операторы сервисов.

HOST_HELP_GROUP
	Синтаксис: GROUP
	
	Эта команда позволяет пользователям установить виртуальный хост с их
	ТЕКУЩЕГО ника на все ники
	группы.

OPER_SVSNICK_UNSUPPORTED
	Sorry, SVSNICK is not available on this network.

OPER_SQLINE_UNSUPPORTED
	Sorry, SQLINE is not available on this network.

OPER_SVSO_UNSUPPORTED
	Sorry, OLINE is not available on this network.

OPER_UMODE_UNSUPPORTED
	Sorry, UMODE is not available on this network.

