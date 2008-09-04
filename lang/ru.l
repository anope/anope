# Russian language file for Anope
# 
# (C) 2003-2008 Anope Team
# Contact us at info@anope.org
#
# Please read COPYING and README for further details.
#
# Based on the original code of Epona by Lara.
# Based on the original code of Services by Andy Church.
#
# Unofficial translate by Kein <kein-of@yandex.ru> 
#
# Translated: 23.10.2007
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
#	%a = день недели (сокр.)	%H = часы
#	%b = месяц (сокр.)		%M = минуты
#	%d = число			%S = секунды
#	%Y = год			%Z = временная зона

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
	Неизвестная команда %s. "%R%s HELP" чтобы получить помощь по командам.
SYNTAX_ERROR
	Синтаксис: %s
MORE_INFO
	%R%s HELP %s для более подробной информации.
NO_HELP_AVAILABLE
	Нет помощи по %s.
OBSOLETE_COMMAND
	Данная команда устарела, вместо нее используйте %s.

BAD_USERHOST_MASK
	Маска должна быть в формате идент@хост.
BAD_EXPIRY_TIME
	Неверное время истечения.
USERHOST_MASK_TOO_WIDE
	Границы %s слишком широки, пожалуйста, уточните маску.

SERVICE_OFFLINE
	%s сейчас в оффлайне.
READ_ONLY_MODE
	Примечание: сейчас сервисы работают в режиме только-чтения, изменения НЕ будут сохранены!
PASSWORD_INCORRECT
	Неверный пароль.
INVALID_TARGET
	"/msg %s" больше не поддерживается. Используйте "/msg %s@%s" или "/%s".

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
	Опция RAW отключена. Если Вы желаете использовать ее, сконфигурируйте директиву
	DisableRaw в конфигурационном файле сервисов.

MORE_OBSCURE_PASSWORD
	Пожалуйста, попробуйте снова с более сложным паролем. Пароль должен иметь длину не менее 5 символов, состоять только из латинских букв или цифр, не быть легко отгадываемым (т.е. Вашим именем или Вашим ником), и не должен содержать пробелов и табуляций.
PASSWORD_TRUNCATED
	Примечание: Ваш пароль был усечен до %d символов.

NICK_NOT_REGISTERED
	Ваш ник не зарегистрирован.
NICK_NOT_REGISTERED_HELP
	Ваш ник не зарегистрирован. Напишите %R%s HELP для получения информации о том, как зарегистрировать ник.
NICK_X_IS_SERVICES
	Ник %s является ником одного из Сервисов данной сети.
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
NICK_X_SUSPENDED
	Ник %s находится в режиме саспенда.
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
	как напишете команду %R%s IDENTIFY пароль.
CHAN_IDENTIFY_REQUIRED
	Для выполнения данной команды Вы должны идентифицироваться. Попробуйте снова после того,
	как напишете команду %R%s IDENTIFY %s пароль.
	
MAIL_DISABLED
	Сервисы не сконфигурированы для отправки почты.
MAIL_INVALID
	Email-адрес %s некорректен.
MAIL_X_INVALID
	%s не является правильным адресом email.
MAIL_LATER
	Не удается отправить email, попробуйте, пожалуйста, позже.
MAIL_DELAYED
	Пожалуйста, подождите %d секунд и повторите запрос.
	
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

# Generic List error messages
LIST_INCORRECT_RANGE
	Задан не верный диапазон. Синтаксис #from-to.
CS_LIST_INCORRECT_RANGE
	Что бы найти канал, начинающийся с символа #, просто укажите его
	название, без #. Например anope вместо #anope.

# Generic help limited to messages
HELP_LIMIT_SERV_OPER
	
	Для использования этой команды необходимы права Оператора сервисов.
HELP_LIMIT_SERV_ADMIN
	
	Для использования этой команды необходимы права Администратора сервисов.
HELP_LIMIT_SERV_ROOT
	
	Для использования этой команды необходимы права Рута сервисов.
HELP_LIMIT_IRC_OPER
	
	Для использования этой команды необходимы права IRC-оператора.
HELP_LIMIT_HOST_SETTER
	
	Для использования этой команды необходимы права Оператора хостов.
HELP_LIMIT_HOST_REMOVER
	
	Для использования этой команды необходимы права Оператора хостов.

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	Этот ник уже зарегистрирован и используется. Выберите, пожалуйста, другой ник.
	(Если это Ваш ник, напишите команду %R%s IDENTIFY пароль.)
NICK_IS_SECURE
	Этот ник зарегистрирован и защищен. Если это ваш ник, напишите
	команду %R%s IDENTIFY пароль. Иначе, выберите, пожалуйста, другой ник.
NICK_MAY_NOT_BE_USED
	Этот ник не может быть использован. Пожалуйста, выберите любой другой.
FORCENICKCHANGE_IN_1_MINUTE
	Если вы не выполните указанные рекомендации, Ваш ник будет принудительно изменен в течении минуты.
FORCENICKCHANGE_IN_20_SECONDS
	Если вы не выполните указанные рекомендации, Ваш ник будет принудительно изменен в течении 20 секунд.
FORCENICKCHANGE_NOW
	Этот ник был зарегистрирован, Вы не можете его использовать.
FORCENICKCHANGE_CHANGING
	Ваш ник был изменен на %s

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER пароль [email]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER пароль email
NICK_REGISTRATION_DISABLED
	Извините, но регистрация ников временно приостановлена.
NICK_REGISTRATION_FAILED
	Извините, ошибка регистрации.
NICK_REG_PLEASE_WAIT
	Пожалуйста, подождите %d секунд перед повторным использованием команды REGISTER.
NICK_CANNOT_BE_REGISTERED
	Ник %s не может быть зарегистрирован.
NICK_ALREADY_REGISTERED
	Ник %s уже зарегистрирован!
NICK_REGISTERED
	Ник %s зарегистрирован на Вашу учетную запись: %s
NICK_REGISTERED_NO_MASK
	Ник %s зарегистрирован.
NICK_PASSWORD_IS
	Ваш пароль - %s - используйте его в дальнейшем и не забывайте.
NICK_REG_DELAY
	Вы должны использовать данный ник не менее %d секунд, что бы начать его регистрацию.


# GROUP responses
NICK_GROUP_SYNTAX
	GROUP главный_ник пароль
NICK_GROUP_DISABLED
	Извините, группирование ников временно недоступно.
NICK_GROUP_FAILED
	Извините, ошибка группирования ников.
NICK_GROUP_PLEASE_WAIT
	Пожалуйста, подождите %d секунд перед повторным использованием команды GROUP.
NICK_GROUP_CHANGE_DISABLED
	Ваш ник уже зарегистрирован, сначала удалите ник командой %R%s DROP.
NICK_GROUP_SAME
	Вы уже являетесь членом группы %s.
NICK_GROUP_TOO_MANY
	Слишком много ников в группе %s, просмотрите список и удалите ненужные.
	Напишите %R%s HELP GLIST и %R%s HELP DROP
	для более подробной информации.
NICK_GROUP_JOINED
	Вы успешно внесены в группу %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY пароль
NICK_IDENTIFY_FAILED
	Извините, но попытка идентификации неудачна.
NICK_IDENTIFY_SUCCEEDED
	Пароль принят - вы признаны как владелец ника.
NICK_IDENTIFY_EMAIL_REQUIRED
	Сначала Вы должны указать Ваш e-mail. Правильный e-mail позволит Вам получить на него пароль,
	если Вы его забудете.
NICK_IDENTIFY_EMAIL_HOWTO
	Напишите команду %R%S SET EMAIL e-mail чтобы установить e-mail. Ваша приватность
	гарантирована, этот e-mail не может быть узнан третьим лицом.
NICK_ALREADY_IDENTIFIED
	Вы уже идентифицированы.

# UPDATE responses
NICK_UPDATE_SUCCESS
	Статус обновлен (мемо-сообщения, вирутальные хосты, режимы на каналах, флаги).
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	Деидентификация выполнена успешно.
NICK_LOGOUT_X_SUCCEEDED
	Ник %s был деидентифицирован.
NICK_LOGOUT_SERVICESADMIN
	Невозможно деидентифицировать %s, так как он Администратор Сервисов.

# DROP responses
NICK_DROP_DISABLED
	Извините, удаление ников временно запрещено.
NICK_DROPPED
	Ваш ник был удален из базы данных.
NICK_X_DROPPED
	Ник %s был удален из базы данных.

# SET responses
NICK_SET_SYNTAX
	SET опция параметры
NICK_SET_SERVADMIN_SYNTAX
	SET [ник] опция параметры
NICK_SET_DISABLED
	Извините, установка параметров ника временно недоступна.
NICK_SET_UNKNOWN_OPTION
	Неизвестный SET-параметр %s.
NICK_SET_OPTION_DISABLED
	Параметр %s не может быть установлен в этой IRC сети.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	Новый основной ник должен быть ЗАРЕГИСТРИРОВАННЫМ ником и состоять в Вашей группе!
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
	Неизвестный номер языка %d. Напишите %R%s HELP SET LANGUAGE для получения списка доступных языков.
NICK_SET_LANGUAGE_CHANGED
	Язык сервисов изменен на Русский.

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
	Вы не можете удалить адрес email в этой IRC сети.
	
# SET ICQ responses
NICK_SET_ICQ_CHANGED
	ICQ-номер %s успешно установлен.
NICK_SET_ICQ_UNSET
	ICQ-номер удален.
NICK_SET_ICQ_INVALID
	%s не является правильным ICQ-номером.
	
# SET GREET responses
NICK_SET_GREET_CHANGED
	Приветственное сообщение изменено на: %s.
NICK_SET_GREET_UNSET
	Приветственное сообщение удалено.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Защита от захвата ника ВКЛЮЧЕНА, лимит времени на ввод пароля: 60 секунд.
NICK_SET_KILL_QUICK
	Защита от захвата ника ВКЛЮЧЕНА, лимит времени на ввод пароля: 20 секунд.
NICK_SET_KILL_IMMED
	Защита от захвата ника ВКЛЮЧЕНА, лимит времени на ввод пароля: 0 секунд.
NICK_SET_KILL_IMMED_DISABLED
	Параметр IMMED недоступен в этой IRC сети.
NICK_SET_KILL_OFF
	Защита от захвата ника ВЫКЛЮЧЕНА.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Безопасность ВКЛЮЧЕНА.
NICK_SET_SECURE_OFF
	Безопасность ВЫКЛЮЧЕНА.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Приватность ника ВКЛЮЧЕНА.
NICK_SET_PRIVATE_OFF
	Приватность ника ВЫКЛЮЧЕНА.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Теперь Ваш email будет скрыт из запроса %s INFO.
NICK_SET_HIDE_EMAIL_OFF
	Теперь Ваш email будет показываться по запросу %s INFO.
NICK_SET_HIDE_MASK_ON
	Теперь Ваша последняя маска идент@хост будет скрыта из запроса %s INFO.
NICK_SET_HIDE_MASK_OFF
	Теперь Ваша последняя маска идент@хост будет показана по запросу %s INFO.
NICK_SET_HIDE_QUIT_ON
	Ваше последнее сообщение при выходе будет скрыто из запроса %s INFO.
NICK_SET_HIDE_QUIT_OFF
	Ваше последнее сообщение при выходе будет показано по запросу %s INFO.
NICK_SET_HIDE_STATUS_ON
	Ваш уровень доступа к сервисам теперь скрыт из запроса %s INFO.
NICK_SET_HIDE_STATUS_OFF
	Ваш уровень доступа к сервисам теперь будет показан по запросу %s INFO.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET MSG {ON | OFF}
NICK_SET_MSG_ON
	Теперь сервисы будут отвечать Вам посредством приватных сообщений.
NICK_SET_MSG_OFF
	Теперь сервисы будут отвечать Вам посредством нотисов.

# SET AUTOOP responses
NICK_SET_AUTOOP_SYNTAX
	SET AUTOOP {ON | OFF}
NICK_SET_AUTOOP_ON
	Теперь сервисы будут автоматически давать вам статус при заходе на канал.
NICK_SET_AUTOOP_OFF
	Теперь сервисы не будут автоматически давать вам статус при заходе на канал.

# SET NOEXPIRE responses
NICK_SASET_SYNTAX
	SASET ник опция параметры
NICK_SASET_DISABLED
	Извините, установка параметров ника временно недоступна.
NICK_SASET_UNKNOWN_OPTION
	Неизвестный SASET параметр %s.
NICK_SASET_BAD_NICK
	Ник %s незарегистрирован.
NICK_SASET_OPTION_DISABLED
	Параметр %s не может быть установлен в этой IRC сети.

# SASET DISPLAY responses
NICK_SASET_DISPLAY_INVALID
	Новый главный ник для %s должен быть ЗАРЕГИСТРИРОВАННЫМ ником и состоять в его группе!
NICK_SASET_DISPLAY_CHANGED
	Новый главный ник теперь: %s.

# SASET PASSWORD responses
NICK_SASET_PASSWORD_FAILED
	Извините, невозможно изменить пароль для %s.
NICK_SASET_PASSWORD_CHANGED
	Пароль для %s изменен.
NICK_SASET_PASSWORD_CHANGED_TO
	Пароль для %s изменен на %s.

# SASET LANGUAGE responses
NICK_SASET_URL_CHANGED
	URL для %s изменен на %s.
NICK_SASET_URL_UNSET
	URL для %s удален.

# SASET EMAIL responses
NICK_SASET_EMAIL_CHANGED
	E-mail адрес для %s изменен на %s.
NICK_SASET_EMAIL_UNSET
	E-mail адрес для %s удален.
NICK_SASET_EMAIL_UNSET_IMPOSSIBLE
	Вы не можете удалить адрес email в этой IRC сети.
	
# SASET ICQ responses
NICK_SASET_ICQ_CHANGED
	ICQ-номер для %s теперь %s.
NICK_SASET_ICQ_UNSET
	ICQ-номер для %s удален.
NICK_SASET_ICQ_INVALID
	%s не является правильным ICQ-номером.
	
# SASET GREET responses
NICK_SASET_GREET_CHANGED
	Приветствие для %s изменено на %s.
NICK_SASET_GREET_UNSET
	Приветствие для %s удалено.

# SASET PROTECT responses
NICK_SASET_KILL_SYNTAX
	SASET ник KILL {ON | QUICK | OFF}
NICK_SASET_KILL_IMMED_SYNTAX
	SASET ник KILL {ON | QUICK | IMMED | OFF}
NICK_SASET_KILL_ON
	Защита для %s теперь ВКЛЮЧЕНА, лимит времени на ввод пароля: 60 секунд.
NICK_SASET_KILL_QUICK
	Защита для %s теперь ВКЛЮЧЕНА,лимит времени на ввод пароля: 20 секунд.
NICK_SASET_KILL_IMMED
	Защита для %s теперь ВКЛЮЧЕНА, лимит времени на ввод пароля: 0 секунд.
NICK_SASET_KILL_IMMED_DISABLED
	Опция IMMED недоступна в этой IRC-сети.
NICK_SASET_KILL_OFF
	Защита для %s теперь ВЫКЛЮЧЕНА.

# SASET SECURE responses
NICK_SASET_SECURE_SYNTAX
	SASET ник SECURE {ON | OFF}
NICK_SASET_SECURE_ON
	Безопасность для %s теперь ВКЛЮЧЕНА.
NICK_SASET_SECURE_OFF
	Безопасность для %s теперь ВЫКЛЮЧЕНА.

# SASET PRIVATE responses
NICK_SASET_PRIVATE_SYNTAX
	SASET ник PRIVATE {ON | OFF}
NICK_SASET_PRIVATE_ON
	Приватность для %s теперь ВКЛЮЧЕНА.
NICK_SASET_PRIVATE_OFF
	Приватность для %s теперь ВЫКЛЮЧЕНА.

# SASET HIDE responses
NICK_SASET_HIDE_SYNTAX
	SET ник HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SASET_HIDE_EMAIL_ON
	E-mail адрес для %s теперь будет скрыт из запроса %s INFO.
NICK_SASET_HIDE_EMAIL_OFF
	E-mail адрес для %s теперь будет показан по запросу %s INFO.
NICK_SASET_HIDE_MASK_ON
	Последняя маска (идент@хост) для %s теперь будет скрыта из запроса %s INFO.
NICK_SASET_HIDE_MASK_OFF
	Последняя маска (идент@хост) для %s теперь будет показана по запросу %s INFO.
NICK_SASET_HIDE_QUIT_ON
	Последнее quit-сообщение для %s теперь будет скрыто из запроса %s INFO.
NICK_SASET_HIDE_QUIT_OFF
	Последнее quit-сообщение для %s теперь будет показано по запросу %s INFO.
NICK_SASET_HIDE_STATUS_ON
	Уровень доступа к сервисам для %s теперь будет скрыт из запроса %s INFO.
NICK_SASET_HIDE_STATUS_OFF
	TУровень доступа к сервисам для %s теперь будет показан по запросу %s INFO.
	
# SASET MSG responses
NICK_SASET_MSG_SYNTAX
	SSAET ник MSG {ON | OFF}
NICK_SASET_MSG_ON
	Теперь, сервисы будут отвечать %s посредством приватных сообщений.
NICK_SASET_MSG_OFF
	Теперь, сервисы будут отвечать %s посредством нотисов.

# SET NOEXPIRE responses
NICK_SASET_NOEXPIRE_SYNTAX
	SASET ник NOEXPIRE {ON | OFF}
NICK_SASET_NOEXPIRE_ON
	Теперь, регистрация ника %s никогда не истечет.
NICK_SASET_NOEXPIRE_OFF
	Теперь, ник %s снова в режиме истечения.

# SASET AUTOOP responses
NICK_SASET_AUTOOP_SYNTAX
	SASET ник AUTOOP {ON | OFF}
NICK_SASET_AUTOOP_ON
	Теперь, сервисы будут автоматически давать статус для %s на каналах.
NICK_SASET_AUTOOP_OFF
	Теперь, сервисы больше не будут автоматически давать статус для %s на каналах.

# SASET LANGUAGE responses
NICK_SASET_LANGUAGE_SYNTAX
	SASET nickname LANGUAGE number
NICK_SASET_LANGUAGE_UNKNOWN
	Unknown language number %d.  Type %R%s HELP SET LANGUAGE for a list of languages.
NICK_SASET_LANGUAGE_CHANGED
	Language changed to Russian.


# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [маска]
NICK_ACCESS_ALREADY_PRESENT
	Маска %s уже присутствует в вашем списке доступа.
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
	Ва список доступа пуст.
NICK_ACCESS_LIST_X_EMPTY
	Список доступа для %s пуст.

# Status messages
NICK_STATUS_0
	STATUS %s 0 (ник не зарегистрирован или его нету в сети)
NICK_STATUS_1
	STATUS %s 1 (ник зарегистрирован и в сети, но пользователь не идентифицировался к нему)
NICK_STATUS_2
	STATUS %s 2 (ник зарегистрирован и в сети, пользователь идентифицирован к нему через список масок)
NICK_STATUS_3
	STATUS %s 3 (пользователь идентифицирован как владелец ника через идентификацию паролем)

# INFO responses
NICK_INFO_SYNTAX
	INFO ник [ALL]
NICK_INFO_REALNAME
	%s это %s
NICK_INFO_SERVICES_OPER
	%s это Оператор сервисов.
NICK_INFO_SERVICES_ADMIN
	%s это Администратор сервисов.
NICK_INFO_SERVICES_ROOT
	%s это Рут сервисов.
NICK_INFO_ADDRESS
	         Последняя маска: %s
NICK_INFO_ADDRESS_ONLINE
	 Сейчас онлайн, c маской: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	     %s сейчас в онлайне.
NICK_INFO_TIME_REGGED
	         Зарегистрирован: %s
NICK_INFO_LAST_SEEN
	   Последний раз замечен: %s
NICK_INFO_LAST_QUIT
	Последнее quit-сообщение: %s
NICK_INFO_URL
	                     URL: %s
NICK_INFO_EMAIL
	             Email-адрес: %s
NICK_INFO_VHOST
	        Виртуальный хост: %s
NICK_INFO_VHOST2
	        Виртуальный хост: %s@%s
NICK_INFO_ICQ
	                   ICQ #: %d
NICK_INFO_GREET
	   Сообщение приветствия: %s
NICK_INFO_OPTIONS
	               Параметры: %s
NICK_INFO_EXPIRE
	    Регистрация истекает: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Защита
NICK_INFO_OPT_SECURE
	Безопасность
NICK_INFO_OPT_PRIVATE
	Приватность
NICK_INFO_OPT_MSG
	Режим приватных сообщений
NICK_INFO_OPT_AUTOOP
	Авто-оп
NICK_INFO_OPT_NONE
	нет
NICK_INFO_NO_EXPIRE
	Этот ник никогда не истечет.
NICK_INFO_FOR_MORE
	Для более подробной информации напишите %R%s INFO %s ALL.
NICK_INFO_SUSPENDED
	Этот ник в режиме саспенда по причине: %s
NICK_INFO_SUSPENDED_NO_REASON
	Этот ник находится в режима саспенда.

# LIST responses
NICK_LIST_SYNTAX
	LIST маска
NICK_LIST_SERVADMIN_SYNTAX
	LIST маска [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Список, содержащий %s:
NICK_LIST_RESULTS
	Конец списка - %d/%d записей показано.

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
	   %c%s (истекает %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER ник [пароль]
NICK_NO_RECOVER_SELF
	Вы не можете применить RECOVER на самого себя!
NICK_RECOVERED
	User claiming your nick has been killed.
	%R%s RELEASE %s to get it back before %s timeout.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE ник [пароль]
NICK_RELEASE_NOT_HELD
	Ник %s не был удержан.
NICK_RELEASED
	Ваш ник освобожден от удержания Сервисами.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST ник [пароль]
NICK_NO_GHOST_SELF
	Вы не можете закрыть свою сессию!
NICK_GHOST_KILLED
	Ваш зависший ник был убит.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS ник
NICK_GETPASS_UNAVAILABLE
	Команда GETPASS недоступна, так как включено шифрование.
NICK_GETPASS_PASSWORD_IS
	Пароль для %s - %s.

# GETEMAIL responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host Никаких символьных масок!!
NICK_GETEMAIL_EMAILS_ARE
	Email-адреса подходящие под %s: %s.
NICK_GETEMAIL_NOT_USED
	Нет совпадений Email-адресов для %s.

# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS ник
NICK_SENDPASS_UNAVAILABLE
	Команда SENDPASS недоступна, так как включено шифрование.
NICK_SENDPASS_SUBJECT
	Пароль на ник (%s)
NICK_SENDPASS_HEAD
	Здраствуйте,
NICK_SENDPASS_LINE_1
	Вы просили %s выслать пароль от Вашего ника на e-mail.
NICK_SENDPASS_LINE_2
	Ваш пароль %s. Для большей безопасности, Вам рекомендуется изменить его как можно скорее.
NICK_SENDPASS_LINE_3
	Если Вы не запрашивали это письмо, просто проигнорируйте его.
NICK_SENDPASS_LINE_4
	ПОЖАЛУЙСТА, НЕ ОТВЕЧАЙТЕ НА ЭТО ПИСЬМО!
NICK_SENDPASS_LINE_5
	Администраторы %s.
NICK_SENDPASS_OK
	Пароль %s был отправлен.

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND ник причина
NICK_SUSPEND_SUCCEEDED
	Ник %s теперь в режиме саспенда.
NICK_SUSPEND_FAILED
	Невозможно установить режим саспенда на %s!

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND ник
NICK_UNSUSPEND_SUCCEEDED
	Режим саспенда с %s снят.
NICK_UNSUSPEND_FAILED
	Невозможно снять режим саспенда с %s!

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
	Этот ник уже запрашивал регистрацию, пожалуйста, проверьте Ваш e-mail на предмет получения auth-кода.
NICK_REG_RESENT
	Ваш код аутенфикации был заново выслан на %s.
NICK_REG_UNABLE
	Ник НЕ ЗАРЕГИСТРИРОВАН, попробуйте, пожалуйста, позднее.
NICK_IS_PREREG
	Этот ник ожидает подтверждения через e-mail перед завершением регистрации.
NICK_ENTER_REG_CODE
	Auth-код был выслан на %s, пожалуйста, напишите команду %R%s confirm auth-код для завершения регистрации
NICK_CONFIRM_NOT_FOUND
	Время первой части регистрации истекло, пожалуйста, воспользуйтесь сначала "%R%s register <пароль> <email>" .
NICK_CONFIRM_INVALID
	Был введен неверный auth-код, пожалуйста, проверьте Ваш e-mail снова, и попробуйте еще раз.
NICK_REG_MAIL_SUBJECT
	Регистрация ника (%s)
NICK_REG_MAIL_HEAD
	Здраствуйте,
NICK_REG_MAIL_LINE_1
	Вы запросили регистрацию ника %s.
NICK_REG_MAIL_LINE_2
	Пожалуйста, напишите команду " %R%s confirm %s " чтобы завершить регистрацию ника.
NICK_REG_MAIL_LINE_3
	Если Вы не запрашивали это письмо, просто проигнорируйте его.
NICK_REG_MAIL_LINE_4
	ПОЖАЛУЙСТА, НЕ ОТВЕЧАЙТЕ НА ЭТО ПИСЬМО!
NICK_REG_MAIL_LINE_5
	Администраторы %s.
NICK_GETPASS_PASSCODE_IS
	Пароль для %s - %s.
NICK_FORCE_REG
	Ник %s подтвердил свою регистрацию.

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Автоматически устанавливает статус оператора (+o)
CHAN_LEVEL_AUTOVOICE
	Автоматически ставит режим +v
CHAN_LEVEL_AUTOHALFOP
	Автоматически ставит режим +h
CHAN_LEVEL_AUTOPROTECT
	Автоматически ставит режим +a
CHAN_LEVEL_AUTODEOP
	Статус оператора канала запрещен
CHAN_LEVEL_NOJOIN
	Невозможно зайти на канал: доступ ограничен
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
	Можно использовать FANTASY-команды у бота
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
	Без подписи кика, если используется SIGNKICK LEVEL
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
	Статус оператора на канале %s для вас запрещен.
CHAN_MAY_NOT_BE_USED
	Этот канал нельзя использовать.
CHAN_NOT_ALLOWED_TO_JOIN
	Вам не разрешено находиться на этом канале.
CHAN_X_INVALID
	Недопустимый канал %s.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER #канал пароль описание
CHAN_REGISTER_DISABLED
	Извините, регистрация каналов временно приостановлена.
CHAN_REGISTER_NOT_LOCAL
	Локальные каналы не могут быть зарегистрированы.
CHAN_MUST_REGISTER_NICK
	Сначала Вы должны зарегистрировать свой ник. Напишите %R%s HELP для получения информации о регистрации.
CHAN_MUST_IDENTIFY_NICK
	Сперва идентифицируйте свой ник на %s, используя команду:
	%R%s IDENTIFY пароль
CHAN_MAY_NOT_BE_REGISTERED
	Канал %s не может быть зарегистрирован.
CHAN_ALREADY_REGISTERED
	Канал %s уже зарегистрирован!
CHAN_MUST_BE_CHANOP
	Вы должны быть оператором на канале, чтобы зарегистрировать его.
CHAN_REACHED_CHANNEL_LIMIT
	Извините, Вы уже зарегистрировали %d каналов, больше, к сожалению, нельзя.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Извините, но Вы уже превысили лимит (%d) зарегистрированных каналов.
CHAN_REGISTRATION_FAILED
	Извините, ошибка регистрации.
CHAN_REGISTERED
	Канал %s зарегистрирован на Ваш ник: %s
CHAN_PASSWORD_IS
	Пароль Вашего канала -  %s - не забывайте его! Ни в коем случае не давайте свой пароль посторонним людям!
CHAN_REGISTER_NONE_CHANNEL
	Вы пытаетесь зарегистрировать не существующий канал %s
CHAN_SYMBOL_REQUIRED
	Пожалуйста, используйте символы # перед названием канала, когда пытаетесь зарегистрировать его.


# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY #канал пароль
CHAN_IDENTIFY_FAILED
	Ошибка идентификации - неверный пароль.
CHAN_IDENTIFY_SUCCEEDED
	Пароль принят, теперь Ваш уровень доступа на %s - владелец.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT #канал ник
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT #канал [ник]
CHAN_LOGOUT_SUCCEEDED
	Пользователь %s был деидентифицирован от канала %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	Все идентифицированные пользователи были деидентифицированы от канала %s.

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
	Неизвестная SET-опция %s.

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
	URL канала %s удален.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	Email-адрес для канала %s изменен на %s.
CHAN_EMAIL_UNSET
	Email-адрес для канала %s удален.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Сообщение при входе на канал %s изменено.
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
	Режим %c проигнорирован, так как Вы не можете заблокировать его.
CHAN_SET_MLOCK_L_REQUIRED
	Чтобы заблокировать режим +L Вы должны сперва заблокировать режим +l.
CHAN_SET_MLOCK_K_REQUIRED
	Чтобы заблокировать режим +K Вы должны сперва заблокировать режим +i.
CHAN_MLOCK_CHANGED
	Заблокированные режимы на канале %s изменены на %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET #канал KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Хранение топика для канала %s теперь ВКЛЮЧЕНО.
CHAN_SET_KEEPTOPIC_OFF
	Хранение топика для канала %s теперь ВЫКЛЮЧЕНО.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET #канал TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Блокировка топика для канала %s теперь ВКЛЮЧЕНА.
CHAN_SET_TOPICLOCK_OFF
	Блокировка топика для канала %s теперь ВЫКЛЮЧЕНА.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET #канал PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Режим Спокойствия для канала %s теперь ВКЛЮЧЕН.
CHAN_SET_PEACE_OFF
	Режим Спокойствия для канала %s теперь ВЫКЛЮЧЕН.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET #канал PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Приватность для канала %s теперь ВКЛЮЧЕНА.
CHAN_SET_PRIVATE_OFF
	Приватность для канала %s теперь ВЫКЛЮЧЕНА.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET #канал SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Режим Безопасности Операторов для канала %s теперь ВКЛЮЧЕН.
CHAN_SET_SECUREOPS_OFF
	Режим Безопасности Операторов для канала %s теперь ВЫКЛЮЧЕН.
	
# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET #канал SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Режим Безопасности Владельца для канала %s теперь ВКЛЮЧЕН.
CHAN_SET_SECUREFOUNDER_OFF
	Режим Безопасности Владельца для канала %s теперь ВЫКЛЮЧЕН.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET #канал RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Режим Запретный для канала %s теперь ВКЛЮЧЕН.
CHAN_SET_RESTRICTED_OFF
	Режим Запретный для канала %s теперь ВЫКЛЮЧЕН.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET #канал SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Режим Безопасности для канала %s теперь ВКЛЮЧЕН.
CHAN_SET_SECURE_OFF
	Режим Безопасности для канала %s теперь ВЫКЛЮЧЕН.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET #канал SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Режим Подписанных Киков для канала %s теперь ВКЛЮЧЕН.
CHAN_SET_SIGNKICK_LEVEL
	Режим Подписанных Киков для канала %s теперь ВКЛЮЧЕН,
	но зависит от уровня пользователя, который использует эту команду.
CHAN_SET_SIGNKICK_OFF
	Режим Подписанных Киков для канала %s теперь ВЫКЛЮЧЕН.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET #канал OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Режим OP-уведомления (об ОПе\деОПе) для канала %s теперь ВКЛЮЧЕН.
CHAN_SET_OPNOTICE_OFF
	Режим OP-уведомления (об ОПе\деОПе) для канала %s теперь ВЫКЛЮЧЕН.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET #канал XOP {ON | OFF}
CHAN_SET_XOP_ON
	Система привелегий xOP для канала %s теперь ВКЛЮЧЕНА.
CHAN_SET_XOP_OFF
	Система привелегий xOP для канала %s теперь ВЫКЛЮЧЕНА.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET #канал NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Канал %s никогда не истечет.
CHAN_SET_NOEXPIRE_OFF
	Режим истечения по времени для канала %s теперь включен.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Извините, но доступно лишь %d VOP/HOP/AOP/SOP записей на канале.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	Вы не можете использоват эту команду. Вместо нее используйте команду ACCESS.
	Напишите %R%s HELP ACCESS для получения более подробной информации.
CHAN_XOP_NOT_AVAILABLE
	Система привелегий xOP недоступна.

# AOP messages
CHAN_AOP_SYNTAX
	AOP #канал {ADD|DEL|LIST|CLEAR} [ник | номер запис]
CHAN_AOP_DISABLED
	Извините, модификация списка AOP'ов временно недоступна.
CHAN_AOP_NICKS_ONLY
	Список AOP'ов может включать только зарегистрированные ники.
CHAN_AOP_ADDED
	%s добавлен в AOP'ы канала %s.
CHAN_AOP_MOVED
	%s перемещен в AOP'ы канала %s.
CHAN_AOP_NO_SUCH_ENTRY
	Нет такой записи (#%d) в AOP'ах канала %s.
CHAN_AOP_NOT_FOUND
	%s не найден в AOP'ах канала %s.
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
	Список AOP'ов на канале %s был полностью очищен.

# HOP messages
CHAN_HOP_SYNTAX
	HOP #канал {ADD|DEL|LIST|CLEAR} [ник | номер запис]
CHAN_HOP_DISABLED
	Извините, модификация списка HOP'ов временно недоступна.
CHAN_HOP_NICKS_ONLY
	Список HOP'ов может включать только зарегистрированные ники.
CHAN_HOP_ADDED
	%s добавлен в HOP'ы канала %s.
CHAN_HOP_MOVED
	%s перемещен в HOP'ы канала %s.
CHAN_HOP_NO_SUCH_ENTRY
	Нет такой записи (#%d) в HOP'ах канала %s.
CHAN_HOP_NOT_FOUND
	%s не найден в HOP'ах канала %s.
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
	Список HOP'ов на канале %s был полностью очищен.

# SOP messages
CHAN_SOP_SYNTAX
	SOP #канал {ADD|DEL|LIST|CLEAR} [ник | номер записи]
CHAN_SOP_DISABLED
	Извините, модификация списка SOP'ов временно недоступна.
CHAN_SOP_NICKS_ONLY
	Список SOP'ов может включать только зарегистрированные ники.
CHAN_SOP_ADDED
	%s добавлен в SOP'ы канала %s.
CHAN_SOP_MOVED
	%s перемещен в SOP'ы %s.
CHAN_SOP_NO_SUCH_ENTRY
	Нет такой записи (#%d) в SOP'ах канала %s.
CHAN_SOP_NOT_FOUND
	%s не найден в SOP'ах канала %s.
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
	Список SOP'ов на канале %s был полностью очищен.

# VOP messages
CHAN_VOP_SYNTAX
	VOP #канал {ADD|DEL|LIST|CLEAR} [ник | номер записи]
CHAN_VOP_DISABLED
	Извините, модификация списка VOP'ов временно недоступна.
CHAN_VOP_NICKS_ONLY
	Список VOP'ов может включать только зарегистрированные ники.
CHAN_VOP_ADDED
	%s добавлен в VOP'ы канала %s.
CHAN_VOP_MOVED
	%s перемещен в VOP'ы канала %s.
CHAN_VOP_NO_SUCH_ENTRY
	Нет такой записи (#%d) в VOP'ах канала %s.
CHAN_VOP_NOT_FOUND
	%s не найден в VOP'ах канала %s.
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
	Список VOP'ов на канале %s был полностью очищен.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS #канал {ADD|DEL|LIST|CLEAR} [ник [уровень] | номер записи]
CHAN_ACCESS_XOP
	Вы не можете использовать эту команду.
	Используйте команды VOP, AOP и SOP вместо нее.
	Напишите %R%s HELP команда для получения более подробной информации.
CHAN_ACCESS_XOP_HOP
	Вы не можете использовать эту команду. 
	Вместо нее, используйте команды AOP, SOP, HOP и VOP .
	Введите %R%s HELP команда для более подробной информации.
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
	Уровень доступа для %s на канале %s остался прежним - %d.
CHAN_ACCESS_LEVEL_CHANGED
	Уровень доступа для %s на канале %s изменен на %d.
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
	Список доступа для канала %s был полностью очищен.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK #канал {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [ник-или-маска] [причина]
CHAN_AKICK_DISABLED
	Извините, модификация списка автокиков канала временно недоступна.
CHAN_AKICK_ALREADY_EXISTS
	%s уже есть в списке автокиков канала %s.
CHAN_AKICK_REACHED_LIMIT
	Извините, но список автокика на канале может содержать лишь %d масок.
CHAN_AKICK_ADDED
	%s добавлен в список автокиков канала %s.
CHAN_AKICK_NO_SUCH_ENTRY
	Нет такой записи (#%d) в списке автокиков канала %s.
CHAN_AKICK_NOT_FOUND
	%s не найден в списке автокиков канала %s.
CHAN_AKICK_NO_MATCH
	Нет совпадающих значений в списке автокиков канала %s.
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
	%3d %s (от %s в %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (перманентный) (от %s в %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE на канале %s завершен, результаты: %d пользователей выкинуто.
CHAN_AKICK_CLEAR
	Список AKICK'ов на канале %s был полностью очищен.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS #канал {SET | DIS[ABLE] | LIST | RESET} [пункт [уровень]]
CHAN_LEVELS_XOP
	Система уровней не доступна, так как используется система привилегий xOP.
CHAN_LEVELS_RANGE
	Уровень должен быть цифрой между %d и %d включительно.
CHAN_LEVELS_CHANGED
	Уровень для %s на канале %s изменен на %d.
CHAN_LEVELS_UNKNOWN
	Неизвестный параметр %s. Напишите %R%s HELP LEVELS DESC для получения списка возможных значений.
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
	Уровни доступа для канала %s сброшены на значения по-умолчанию.

# Status Messages
CHAN_STATUS_SYNTAX
	STATUS ERROR Ошибка синтакса
CHAN_STATUS_NOT_REGGED
	STATUS ERROR Канал %s не зарегистрирован
CHAN_STATUS_FORBIDDEN
	STATUS ERROR Канал %s запрещен к использованию
CHAN_STATUS_NOTONLINE
	STATUS ERROR Ник %s не в сети.
CHAN_STATUS_INFO
	STATUS %s %s %d

# INFO responses
CHAN_INFO_SYNTAX
	INFO #канал [ALL]
CHAN_INFO_HEADER
	Информация о канале %s:
CHAN_INFO_FOUNDER
	                  Владелец: %s (%s)
CHAN_INFO_NO_FOUNDER
	                  Владелец: %s
CHAN_INFO_SUCCESSOR
	                 Наследник: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	                 Наследник: %s
CHAN_INFO_DESCRIPTION
	                  Описание: %s
CHAN_INFO_ENTRYMSG
	       Сообщение при входе: %s
CHAN_INFO_TIME_REGGED
	           Зарегистрирован: %s
CHAN_INFO_LAST_USED
	 Последний раз использован: %s
CHAN_INFO_LAST_TOPIC
	           Последний топик: %s
CHAN_INFO_TOPIC_SET_BY
	           Топик установил: %s
CHAN_INFO_URL
	                 URL-адрес: %s
CHAN_INFO_EMAIL
	               Email-адрес: %s
CHAN_INFO_BANTYPE
	                  Тип бана: %d
CHAN_INFO_OPTIONS
	                     Опции: %s
CHAN_INFO_OPT_KEEPTOPIC
	Хранение Топика
CHAN_INFO_OPT_OPNOTICE
	OP-уведомление
CHAN_INFO_OPT_PEACE
	Спокойствие
CHAN_INFO_OPT_PRIVATE
	Приватный
CHAN_INFO_OPT_RESTRICTED
	Запретный
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
	Система привелегий xOP
CHAN_INFO_OPT_NONE
	Нет
CHAN_INFO_MODE_LOCK
	        Блокировка режимов: %s
CHAN_INFO_EXPIRE
	      Регистрация истекает: %s
CHAN_INFO_NO_EXPIRE
	Этот канал никогда не истечет.

# LIST responses
CHAN_LIST_SYNTAX
	LIST маска
CHAN_LIST_SERVADMIN_SYNTAX
	LIST маска [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
CHAN_LIST_HEADER
	Список записей, совпадающий с %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Конец списка - %d/%d совпадений показано.
	
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
	TOPIC #канал [текст_топика]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR #канал что_именно
CHAN_CLEARED_BANS
	Все баны на канале %s были удалены.
CHAN_CLEARED_EXCEPTS
	Все исключения из банов на канале %s были удалены.
CHAN_CLEARED_MODES
	Все режимы на канале %s были сброшены.
CHAN_CLEARED_OPS
	Все опы (+о) были сняты на канале %s.
CHAN_CLEARED_HOPS
	Все хопы (+h) были сняты на канале %s.
CHAN_CLEARED_VOICES
	Все войсы (+v) были сняты на канале %s.
CHAN_CLEARED_USERS
	Все пользователи были выкинуты с канала %s.
CHAN_CLEARED_INVITES
	Все исключения на приглашение на канале %s были удалены.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS #канал
CHAN_GETPASS_UNAVAILABLE
	Команда GETPASS недоступна, так как используется шифрование.
CHAN_GETPASS_PASSWORD_IS
	Пароль на канал %s - %s.
	
# GETKEY responses
CHAN_GETKEY_SYNTAX
	GETKEY #канал
CHAN_GETKEY_NOKEY
	Канал %s не имеет ключа (no key).
CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS #канал
CHAN_SENDPASS_UNAVAILABLE
	Команда SENDPASS недоступна, так как используется шифрование.
CHAN_SENDPASS_SUBJECT
	Пароль канала (%s)
CHAN_SENDPASS_HEAD
	Здраствуйте,
CHAN_SENDPASS_LINE_1
	Вы просили выслать пароль от Вашего канала %s на Ваш e-mail.
CHAN_SENDPASS_LINE_2
	Пароль канала %s. Для большей безопасности, Вам рекомендуется изменить его как можно скорее.
CHAN_SENDPASS_LINE_3
	Если Вы не запрашивали это письмо, просто проигнорируйте его.
CHAN_SENDPASS_LINE_4
	ПОЖАЛУЙСТА, НЕ ОТВЕЧАЙТЕ НА ЭТО ПИСЬМО!
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
	Канал %s был восстановлен из режима саспенда.
CHAN_UNSUSPEND_FAILED
	Невозможно восстановить канал %s!

# Misc responses
CHAN_EXCEPTED
	Значения %s исключены из бана на %s и не могут быть забанены, пока исключения не будут удалены.

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
	Напишите %R%s READ LAST чтобы прочитать его.
MEMO_TYPE_READ_NUM
	Напишите %R%s READ %d чтобы прочитать его.
MEMO_TYPE_LIST_NEW
	Напишите %R%s LIST NEW чтобы посмотреть список соощений.
MEMO_AT_LIMIT
	Внимание: Вы достигли максимально допустимого числа храненимых сообщений (%d). Вы не сможете принимать новые сообщения, пока Вы не удалите какие-нибудь лишние сообщения.
MEMO_OVER_LIMIT
	Внимание: Вы превысили лимит максимального числа храненим сообщений (%d). Вы не сможете принимать новые сообщения, пока Вы не удалите какие-нибудь лишние сообщения.
MEMO_X_MANY_NOTICE
	%d сообщений на канале %s.
MEMO_X_ONE_NOTICE
	%d сообщение на канале %s.
MEMO_NEW_X_MEMO_ARRIVED
	На канале %s новое сообщение.
	Напишите %R%s READ %s %d чтобы прочитать его.
MEMO_NEW_MEMO_ARRIVED
	Вам пришло новое сообщение от %s.
	Напишите %R%s READ %d чтобы прочитать его.

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
	SEND {ник | #канал} текст_сообщения
MEMO_SEND_DISABLED
	Извините, отправка сообщений временно приостановлена.
MEMO_SEND_PLEASE_WAIT
	Пожалуйста, подождите %d секунд перед тем, как использовать команду SEND снова.
MEMO_X_GETS_NO_MEMOS
	%s не может принимать сообщения.
MEMO_X_HAS_TOO_MANY_MEMOS
	У %s переполнен ящик и в данный момент он не может больше принимать сообщения.
MEMO_SENT
	Сообщение для %s отправлено.
MEMO_MASS_SENT
	Массовое сообщение было отправлено всем зарегистрированным пользователям.

# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {ник | #канал}
MEMO_CANCEL_DISABLED
	Извините, отмена сообщений временно недоступна.
MEMO_CANCEL_NONE
	Нет сообщений, доступных для отмены.
MEMO_CANCELLED
	Последнее сообщение для %s было отменено.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [#канал] [список_записей | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	У вас нет новых сообщений.
MEMO_X_HAS_NO_NEW_MEMOS
	У %s нет новых сообщений.
MEMO_LIST_MEMOS
	Сообщения для %s. Чтобы прочесть их, напишите: %R%s READ номер_сообщения
MEMO_LIST_NEW_MEMOS
	Новые сообщения для %s. Чтобы прочесть их, напишите: %R%s READ номер_сообщения
MEMO_LIST_CHAN_MEMOS
	Сообщения для %s. Чтобы прочесть их, напишите: %R%s READ %s номер_сообщения
MEMO_LIST_CHAN_NEW_MEMOS
	Новые сообщения для %s. Чтобы прочесть их, напишите: %R%s READ %s номер сообщения
MEMO_LIST_HEADER
	Номер  Отправитель           Дата/Время
MEMO_LIST_FORMAT
	%c%3d    %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [#канал] {список_записей | LAST | NEW}
MEMO_HEADER
	Сообщение %d от %s (%s). Чтобы удалить его, напишите команду: %R%s DEL %d
MEMO_CHAN_HEADER
	Сообщение %d от %s (%s). Чтобы удалить его, напишите команду: %R%s DEL %s %d
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [#канал] {номер сообщения | список_записей | ALL}
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
	Неизвестная SET-опция %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s будет уведомлять Вас о новых сообщениях, когда Вы идентифицируетесь и когда сообщения будут отправлены Вам.
MEMO_SET_NOTIFY_LOGON
	%s будет уведомлять Вас о новых сообщениях, когда Вы идентифицируетесь или снимете с себя режим /AWAY.
MEMO_SET_NOTIFY_NEW
	%s будет уведомлять Вас о новых сообщениях, когда они будет отправлены Вам.
MEMO_SET_NOTIFY_OFF
	%s не будет уведомлять Вас о новых сообщениях.
MEMO_SET_NOTIFY_MAIL
	Теперь вы будете информированы о новых сообщениях с помощью email.
MEMO_SET_NOTIFY_NOMAIL
	Вы больше не будете информированы о новых сообщениях по email.
MEMO_SET_NOTIFY_INVALIDMAIL
	Для вашего ника не установлен email-адрес.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [#канал] предел
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [пользователь | #канал] {предел | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Вам запрещено изменять свой лимит сообщений.
MEMO_SET_LIMIT_FORBIDDEN
	Лимит сообщений для %s не может быть изменен.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Вы не можете установить лимит сообщений больше чем %d.
MEMO_SET_LIMIT_TOO_HIGH
	Вы не можете установить лимит сообщений для %s больше чем %d.
MEMO_SET_LIMIT_OVERFLOW
	Лимит сообщений чересчур велик, уменьшите его до %d.
MEMO_SET_YOUR_LIMIT
	Лимит сообщений был установлен на %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Вы больше не будете принимать сообщения.
MEMO_UNSET_YOUR_LIMIT
	Лимит сообщений для Вас отключен.
MEMO_SET_LIMIT
	Лимит собщений для %s установлен на %d.
MEMO_SET_LIMIT_ZERO
	Лимит сообщений для %s установлен на 0.
MEMO_UNSET_LIMIT
	Лимит сообщений для %s отключен.

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
	На данный момент у Вас есть %d сообщений, из них 1 непрочитанно.
MEMO_INFO_MEMOS_SOME_UNREAD
	На данный момент у Вас есть %d сообщений, из них %d непрочитанных.
MEMO_INFO_MEMOS_ALL_UNREAD
	На данный момент у Вас есть %d сообщений и все они не прочитаны Вами.
MEMO_INFO_LIMIT
	Ваш лимит сообщений равен %d.
MEMO_INFO_HARD_LIMIT
	Ваш лимит сообщений равен %d и не может быть изменен.
MEMO_INFO_LIMIT_ZERO
	Ваш лимит сообщений равен 0. Вы не будете принимать любые новые сообщения.
MEMO_INFO_HARD_LIMIT_ZERO
	Ваш лимит сообщений равен 0, Вы не будете принимать любые новые сообщения и не можете изменить этот лимит.
MEMO_INFO_NO_LIMIT
	У вас нет ограничения на количество хранимых сообщений.
MEMO_INFO_NOTIFY_OFF
	Вы не будете уведомляться о новых собщениях.
MEMO_INFO_NOTIFY_ON
	Вы будете уведомляться о новых сообщениях при идентификации и по мере прихода новых сообщений.
MEMO_INFO_NOTIFY_RECEIVE
	Вы будете уведовляться сразу по приходу новых сообщений.
MEMO_INFO_NOTIFY_SIGNON
	Вы будете уведомляться о приходе новых сообщений после идентификации.
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
	%s сейчас имеет %d сообщений, все они не прочитаны.
MEMO_INFO_X_LIMIT
	Лимит сообщений для %s равен %d.
MEMO_INFO_X_HARD_LIMIT
	Лимит собщений для %s равен %d и не может быть изменен.
MEMO_INFO_X_NO_LIMIT
	У %s нет ограничения количества сообщений.
MEMO_INFO_X_NOTIFY_OFF
	%s не уведомляется о новых собщениях.
MEMO_INFO_X_NOTIFY_ON
	%s уведомляется о новых сообщениях после идентификации и по мере прихода собщений.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s уведомляется о новых сообщениях по мере их прихода.
MEMO_INFO_X_NOTIFY_SIGNON
	%s уведомляется о новых сообщениях после идентификации.
	
# Memo2Mail responses
MEMO_MAIL_SUBJECT
	Новое мемо-сообщение
MEMO_MAIL_TEXT1
	Здраствуйте %s
MEMO_MAIL_TEXT2
	Вы получили новое мемо-сообщение от %s. Номер этого сообщения: %d.
MEMO_MAIL_TEXT3
	Текст сообщения:
	
# RSEND responses
MEMO_RSEND_PLEASE_WAIT
	Пожалуйста, подождите %d секунд прежде чем использовать команду RSEND снова.
MEMO_RSEND_DISABLED
	Извините, но команда RSEND запрещена в этой IRC-сети.
MEMO_RSEND_SYNTAX
	RSEND {ник | #канал} текст
MEMO_RSEND_NICK_MEMO_TEXT
	[авто-сообщение] Мемо-сообщение, которые вы посылали, было прочитано.
MEMO_RSEND_CHAN_MEMO_TEXT
	[авто-сообщение] Мемо-сообщение, которые вы посылали %s, было прочитано.
MEMO_RSEND_USER_NOTIFICATION
	Мемо-извещение было послано для %s, информирующее его/ее о том, что вы прочитали его/ее сообщение.

# CHECK responses
MEMO_CHECK_SYNTAX
	CHECK ник
MEMO_CHECK_NOT_READ
	Последнее сообщение, которое вы посылали для %s (дата: %s) еще не прочитано.
MEMO_CHECK_READ
	Последнее сообщение, которое вы посылали для %s (дата: %s) уже прочитано.
MEMO_CHECK_NO_MEMO
	Ник %s не получал от вас никаких сообщений.
MEMO_NO_RSEND_SELF
	Вы не можете запросить информацию о получении, когда посылаете сообщение себе.

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
	Напишите %R%S HELP ASSIGN для получения более подробной информации.
BOT_NOT_ON_CHANNEL
	Бота нет на канале %s.
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	Слово %s запрещено на этом канале!
BOT_REASON_BADWORD_GENTLE
	Следи за своим языком!
BOT_REASON_BOLD
	Не используй жирный шрифт на этом канале!
BOT_REASON_CAPS
	Выключи CapsLOCK!
BOT_REASON_COLOR
	В глазах уже рябит от твоих цветов в словах!
BOT_REASON_FLOOD
	Хватит флудить!
BOT_REASON_REPEAT
	Хватит повторов!
BOT_REASON_REVERSE
	Не используй инверсию на этом канале!
BOT_REASON_UNDERLINE
	Не используй подчеркивания на этом канале!
	
# !seen replies
BOT_SEEN_BOT
	Да-да, %s, умный ход спрашивать у бота где он...
BOT_SEEN_YOU
	Любуешься собой в зеркале, да, %s?
BOT_SEEN_ON_CHANNEL
	Если вы не видите %s на этом канале, то вам нужны новые глаза...
BOT_SEEN_ON_CHANNEL_AS
	Хитрюга %s шифруется! Он на канале, но под ником %s! 
BOT_SEEN_ON
	Дух %s последний раз был здесь %s назад.
BOT_SEEN_NEVER
	Что? Какой еще %s? Никогда не видел его на канале...
BOT_SEEN_UNKNOWN
	Кто такой %s я не знаю и знать не хочу.

# BOT responses
BOT_BOT_SYNTAX
	BOT ADD ник идент хост реальное_имя
	BOT CHANGE старый ник новый ник [идент [хост [реальное_имя]]]
	BOT DEL ник
BOT_BOT_ALREADY_EXISTS
	Бот %s уже существует.
BOT_BOT_CREATION_FAILED
	Извините, ошибка создания бота.
BOT_BOT_READONLY
	Извините, изменение натроек бота временно невозвожно.
BOT_BOT_ADDED
	%s!%s@%s (%s) добавлен в список ботов.
BOT_BOT_ANY_CHANGES
	Старая информация не отличается от новой.
BOT_BOT_CHANGED
	Бот %s был изменен на %s!%s@%s (%s)
BOT_BOT_DELETED
	Бот %s был удален.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Список ботов:
BOT_BOTLIST_PRIVATE_HEADER
	Боты, зарезервированные для IRC-Операторов:
BOT_BOTLIST_FOOTER
	%d ботов доступно.
BOT_BOTLIST_EMPTY
	Сейчас нет доступных ботов.
	Попросите Администратора сервисов создать хотя бы одного!
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN #канал ник
BOT_ASSIGN_READONLY
	Извините, назначение ботов на канал временно недоступно.
BOT_ASSIGN_ALREADY
	Бот %s уже назначен на канал %s.
BOT_ASSIGN_ASSIGNED
	Бот %s был назначен на %s.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN #канал
BOT_UNASSIGN_UNASSIGNED
	Боты с канала %s удалены.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {#канал | ник}
BOT_INFO_NOT_FOUND
	%s не является ником бота или зарегистрированным каналом.
BOT_INFO_BOT_HEADER
	Информация о боте %s:
BOT_INFO_BOT_MASK
	          Маска : %s@%s
BOT_INFO_BOT_REALNAME
	   Реальное имя : %s
BOT_INFO_BOT_CREATED
	         Создан : %s
BOT_INFO_BOT_USAGE
	Используется на : %d каналe(ах)
BOT_INFO_BOT_OPTIONS
	          Опции : %s
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
	Без бота
BOT_INFO_OPT_SYMBIOSIS
	Симбиоз
BOT_INFO_OPT_NONE
	Нет
	
# SET responses
BOT_SET_SYNTAX
	SET (#канал | бот) опция установка
BOT_SET_DISABLED
	Извините, установка опций бота временно недоступна.
BOT_SET_UNKNOWN
	Неизвестная опция %s.
	Напишите %R%S HELP SET для более полной информации.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET #канал DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Бот не будет кикать операторов канала %s.
BOT_SET_DONTKICKOPS_OFF
	Бот будет кикать операторов канала %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET #канал DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Бот не будет кикать войсов канала %s.
BOT_SET_DONTKICKVOICES_OFF
	Бот будет кикать войсов канала %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET #канал FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Режим Фантазии на канале %s ВКЛЮЧЕНЫ.
BOT_SET_FANTASY_OFF
	Режим Фантазии на канале %s ВЫКЛЮЧЕН.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET #канал GREET {ON|OFF}
BOT_SET_GREET_ON
	Режим приветствий на канале %s ВКЛЮЧЕН.
BOT_SET_GREET_OFF
	Режим приветствий на канале %s ВЫКЛЮЧЕН.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET #канал NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Режим Без-бота на канале %s ВКЛЮЧЕН.
BOT_SET_NOBOT_OFF
	Режим Без-бота на канале %s ВЫКЛЮЧЕН.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET ник_бота PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Частный режим на бота %s УСТАНОВЛЕН.
BOT_SET_PRIVATE_OFF
	Частный режим с бота %s был СНЯТ.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET #канал SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Режим Симбиоза на канале %s ВКЛЮЧЕН.
BOT_SET_SYMBIOSIS_OFF
	Режим Симбиоза на канале %s ВЫКЛЮЧЕН.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK #канал опция {ON|OFF} [параметры]
BOT_KICK_DISABLED
	Извините, конфигурация киков временно невозможна.
BOT_KICK_UNKNOWN
	Неизвестная опция %s.
	Напишите %R%S HELP KICK для получения более полной информации.
BOT_KICK_BAD_TTB
	%s не может быть установлено как кол-во киков до бана.

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
	Бот будет кикать за использование CapsLOCK (фраза должна содержать
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
	Бот будет кикать за инверсии, и банить после %d 
	киков данного пользователя.
BOT_KICK_REVERSES_OFF
	Бот не будет кикать за инверсии.
BOT_KICK_UNDERLINES_ON
	Бот будет кикать за использование подчеркивания.
BOT_KICK_UNDERLINES_ON_BAN
	Бот будет кикать за использование подчеркивания, и банить после %d 
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
	Список плохих слов для %s:
	 Номер   Слово                           Тип
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	Теперь список плохих слов пуст.

# SAY responses
BOT_SAY_SYNTAX
	SAY #канал текст

# ACT responses
BOT_ACT_SYNTAX
	ACT #канал текст

# Errors
BOT_EXCEPT
	Пользователь соответствует исключению канала.
BOT_BAD_NICK
	Ник бота должен содержать только допустимые символы.
BOT_BAD_HOST
	Хост бота должен содержать только допустимые символы для хоста.
BOT_BAD_IDENT
	Идент бота должен содержать только допустимые символы для идента.
BOT_LONG_IDENT
	Идент бота не должен превышать %d символов.
BOT_LONG_HOST
	Bot Hosts may only contain %d characters.

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
	Неизвестная STATS-опция %s.
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
	Байт считано    : %5d kB
OPER_STATS_BYTES_WRITTEN
	Байт записано   : %5d kB
OPER_STATS_USER_MEM
	Пользователи    : %6d записей, %5d kB
OPER_STATS_CHANNEL_MEM
	Каналы          : %6d записей, %5d kB
OPER_STATS_GROUPS_MEM
	NS-Групп        : %6d записей, %5d kB
OPER_STATS_ALIASES_MEM
	NS-Алиасов      : %6d записей, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ        : %6d записей, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ         : %6d записей, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ        : %6d записей, %5d kB
OPER_STATS_SESSIONS_MEM
	Сессии          : %6d записей, %5d kB
OPER_STATS_AKILL_COUNT
	Текущее количество AKILL'ов           : %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Время истечения AKILL'а по-умолчанию  : %d дней
OPER_STATS_AKILL_EXPIRE_DAY
	Время истечения AKILL'а по-умолчанию  : 1 день
OPER_STATS_AKILL_EXPIRE_HOURS
	Время истечения AKILL'а по-умолчанию  : %d часов
OPER_STATS_AKILL_EXPIRE_HOUR
	Время истечения AKILL'а по-умолчанию  : 1 час
OPER_STATS_AKILL_EXPIRE_MINS
	Время истечения AKILL'а по-умолчанию  : %d минут
OPER_STATS_AKILL_EXPIRE_MIN
	Время истечения AKILL'а по-умолчанию  : 1 минута
OPER_STATS_AKILL_EXPIRE_NONE
	Время истечения AKILL'а по-умолчанию  : Не истекает
OPER_STATS_SGLINE_COUNT
	Текущее количество SGLINE'ов          : %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Время истечения SGLINE'а по-умолчанию : %d дней
OPER_STATS_SGLINE_EXPIRE_DAY
	Время истечения SGLINE'а по-умолчанию : 1 день
OPER_STATS_SGLINE_EXPIRE_HOURS
	Время истечения SGLINE'а по-умолчанию : %d часов
OPER_STATS_SGLINE_EXPIRE_HOUR
	Время истечения SGLINE'а по-умолчанию : 1 час
OPER_STATS_SGLINE_EXPIRE_MINS
	Время истечения SGLINE'а по-умолчанию : %d минут
OPER_STATS_SGLINE_EXPIRE_MIN
	Время истечения SGLINE'а по-умолчанию : 1 минута
OPER_STATS_SGLINE_EXPIRE_NONE
	Время истечения SGLINE'а по-умолчанию : Не истекает
OPER_STATS_SQLINE_COUNT
	Текущее количество SQLINE'ов          : %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Время истечения SQGLINE'а по-умолчанию: %d дней
OPER_STATS_SQLINE_EXPIRE_DAY
	Время истечения SQGLINE'а по-умолчанию: 1 день
OPER_STATS_SQLINE_EXPIRE_HOURS
	Время истечения SQGLINE'а по-умолчанию: %d часов
OPER_STATS_SQLINE_EXPIRE_HOUR
	Время истечения SQGLINE'а по-умолчанию: 1 час
OPER_STATS_SQLINE_EXPIRE_MINS
	Время истечения SQGLINE'а по-умолчанию: %d минут
OPER_STATS_SQLINE_EXPIRE_MIN
	Время истечения SQGLINE'а по-умолчанию: 1 минута
OPER_STATS_SQLINE_EXPIRE_NONE
	Время истечения SQGLINE'а по-умолчанию: Не истекает
OPER_STATS_SZLINE_COUNT
	Текущее количество SZLINE'ов          : %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Время истечения SZLINE'а по-умолчанию : %d дней
OPER_STATS_SZLINE_EXPIRE_DAY
	Время истечения SZLINE'а по-умолчанию : 1 день
OPER_STATS_SZLINE_EXPIRE_HOURS
	Время истечения SZLINE'а по-умолчанию : %d часов
OPER_STATS_SZLINE_EXPIRE_HOUR
	Время истечения SZLINE'а по-умолчанию : 1 час
OPER_STATS_SZLINE_EXPIRE_MINS
	Время истечения SZLINE'а по-умолчанию : %d минут
OPER_STATS_SZLINE_EXPIRE_MIN
	Время истечения SZLINE'а по-умолчанию : 1 минута
OPER_STATS_SZLINE_EXPIRE_NONE
	Время истечения SZLINE'а по-умолчанию : Не истекает
OPER_STATS_RESET
	Статистика сброшена.
OPER_STATS_UPLINK_SERVER
	Uplink-сервер: %s
OPER_STATS_UPLINK_CAPAB
	Uplink capab: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Серверов найдено: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE #канал режимы

# UMODE respondes
OPER_UMODE_SYNTAX
	UMODE ник режимы
OPER_UMODE_SUCCESS
	Режимы %s изменены.
OPER_UMODE_CHANGED
	%s изменил Ваши режимы пользователя.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE ник флаги
OPER_OLINE_SUCCESS
	Оперфлаги для %s изменены на %s
OPER_OLINE_IRCOP
	Теперь Вы IRC-Оператор.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES #канал [ALL]
OPER_CLEARMODES_DONE
	Двоичные режимы и баны очищены на канале %s.
OPER_CLEARMODES_ALL_DONE
	Все режимы на канале %s были сброшены.


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
	Сервисы работают в skeleton-режиме, команда ADMIN недоступна.
OPER_ADMIN_EXISTS
	%s уже есть в списках Администраторов сервисов.
OPER_ADMIN_REACHED_LIMIT
	Извините, может быть только %d Администраторов сервисов.
OPER_ADMIN_ADDED
	%s добавлен в список Администраторов сервисов.
OPER_ADMIN_NOT_FOUND
	%s не найден в списке Администраторов сервисов.
OPER_ADMIN_NO_MATCH
	Нет совпадающих значений в списке Администраторов сервисов.
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
OPER_ADMIN_MOVED
	%s был перемещен в список Администраторов Сервисов.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [ник | номер записи]
OPER_OPER_SKELETON
	Сервисы работают в skeleton-режиме, команда OPER недоступна.
OPER_OPER_EXISTS
	%s уже есть в списках Операторов сервисов.
OPER_OPER_REACHED_LIMIT
	Извините, но может быть только %d Операторов сервисов.
OPER_OPER_ADDED
	%s добавлен в список Операторов сервисов.
OPER_OPER_NOT_FOUND
	%s не найден в списке Операторов сервисов.
OPER_OPER_NO_MATCH
	Нет совпадающих значений в списке Операторов сервисов.
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
OPER_OPER_MOVED
	%s был перемещен в список Операторов Сервисов.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+время_истечения] {маска | номер записи} [причина]]
OPER_AKILL_EXISTS
	%s уже есть в списке AKILL'ов.
OPER_AKILL_ALREADY_COVERED
	%s уже прикрыт %s.
OPER_AKILL_REACHED_LIMIT
	Извините, Вы можете иметь только %d AKILL'ов.
OPER_AKILL_NO_NICK
	Помните: маска AKILL'а не может содержать ники, удостоверьтесь, что ник не не включен в указанную Вами маску.
OPER_AKILL_ADDED
	%s добавлен в список AKILL'ов.
OPER_AKILL_CHANGED
	Время истечения %s изменено.
OPER_AKILL_NOT_FOUND
	%s не найден в списке AKILL'ов.
OPER_AKILL_NO_MATCH
	Нет совпадающих записей в списке AKILL'ов.
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
# номер, маска, поставил, когда, истекает, причина
OPER_AKILL_VIEW_FORMAT
	%3d  %s (от %s в %s; %s)
	      %s
OPER_AKILL_CLEAR
	Список AKILL'ов очищен.
OPER_CHANKILL_SYNTAX
	CHANKILL [+истечение] {#канал} [причина]
	# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+время_истечения] {маска | номер записи}[:причина]]
OPER_SGLINE_UNSUPPORTED
	Извините, использование SGLINE недоступно в этой сети.
OPER_SGLINE_EXISTS
	%s уже есть в списке SGLINE'ов.
OPER_SGLINE_ALREADY_COVERED
	%s уже прикрыт %s.
OPER_SGLINE_REACHED_LIMIT
	Извините, Вы можете иметь только %d записей SGLINE'ов.
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
# номер, маска, поставил, когда, истекает, причина
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (от %s в %s; %s)
	      %s
OPER_SGLINE_CLEAR
	Список SGLINE'ов очищен.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+время_истечения] {маска | номер записи} [причина]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	Извините, использование SQLINE для каналов недоступно в этой сети.
OPER_SQLINE_EXISTS
	%s уже есть в списке SQLINE'ов.
OPER_SQLINE_ALREADY_COVERED
	%s уже прикрыт %s.
OPER_SQLINE_REACHED_LIMIT
	Извините, но Вы можете иметь только %d записей SQLINE'ов.
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
# номер, маска, поставил, когда, истекает, причина
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (от %s в %s; %s)
	      %s
OPER_SQLINE_CLEAR
	Список SQLINE'ов очищен.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+время_истечения] {маска | номер записи} [причина]]
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
	Нет совпадающих записей в списке SZLINE'ов.
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
# номер, маска, поставил, когда, истекает, причина
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (от %s в %s; %s)
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
	Теперь сервисы в режиме только чтение.
OPER_SET_READONLY_OFF
	Теперь сервисы в режиме чтения-записи.
OPER_SET_READONLY_ERROR
	Опция READONLY должна иметь значения только ON или OFF.
OPER_SET_LOGCHAN_ON
	Теперь сервисы будут вести лог на канал %s.
OPER_SET_LOGCHAN_OFF
	Сервисы больше не будут вести лог на канал.
OPER_SET_LOGCHAN_ERROR
	Опция LOGCHAN должна иметь значения только ON или OFF и должен быть определен параметр LogChannel.
OPER_SET_DEBUG_ON
	Сервисы находятся в режиме отладки.
OPER_SET_DEBUG_OFF
	Режим отладки сервисов отключен.
OPER_SET_DEBUG_LEVEL
	Сервисы находятся в режиме отладки (уровень %d).
OPER_SET_DEBUG_ERROR
	Опция DEBUG должна иметь значения ON, OFF или положительное число.
OPER_SET_NOEXPIRE_ON
	Сервисы находятся в режиме не истечения.
OPER_SET_NOEXPIRE_OFF
	Режим не истечения для сервисов отключен.
OPER_SET_NOEXPIRE_ERROR
	Опция NOEXPIRE должна иметь значения только ON или OFF.
OPER_SET_UNKNOWN_OPTION
	Неизвестная опция %s.
OPER_SET_SQL_ON
	SQL-код теперь будет использоваться.
OPER_SET_SQL_OFF
	SQL-код теперь не будет использоваться.
OPER_SET_SQL_ERROR
	Установка для SQL должна быть либо ON, либо OFF.
OPER_SET_SQL_ERROR_INIT
	Ошибка при инициализации SQL, проверьте логи для исправления.
OPER_SET_SQL_ERROR_DISABLED
	SQL не настроено для данной сети. Пожалуйста, отредактируйте
	конфигурационный файл (services.conf).
OPER_SET_LIST_OPTION_ON
	%s включено.
OPER_SET_LIST_OPTION_OFF
	%s выключено.

# NOOP responses
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} сервер
OPER_NOOP_SET
	Все O:lines на %s были удалены.
OPER_NOOP_REVOKE
	Все O:lines на %s были восстановлены.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE имя_сервера [причина]
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
	SERVICES_BIN не определен, перезапуск невозможен. Запустите скрипт \2configure\2  перекомпилируйте сервисы, чтобы включить команду RESTART.

# IGNORE respondes
OPER_IGNORE_SYNTAX
	Синтакстс: IGNORE {ADD|DEL|LIST|CLEAR} [время] [ник]
OPER_IGNORE_VALID_TIME
	Вы должны ввести правильное значение времени.
OPER_IGNORE_TIME_DONE
	Ник %s теперь будет игнорироваться в течение %s.
OPER_IGNORE_PERM_DONE
	Ник %s теперь будет игнорироваться постоянно.
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
OPER_SUPER_ADMIN_ON
	Режим Супер-Администратора активирован.
OPER_SUPER_ADMIN_OFF
	Режим Супер-Администратора деактивирован.
OPER_SUPER_ADMIN_SYNTAX
	Опция SuperAdmin может быть только ON или OFF (должно быть прописано в services.conf)
OPER_SUPER_ADMIN_WALL_ON
	%s теперь Супер-Администратор.
OPER_SUPER_ADMIN_WALL_OFF
	%s снял с себя режим Супер-Администратора.
OPER_SUPER_ADMIN_ONLY
	Только Супер-Администраторы могут использовать эту команду.
OPER_STAFF_LIST_HEADER
	В сети Уровень Ник
OPER_STAFF_FORMAT
	 %c %s  %s
OPER_STAFF_AFORMAT
	 %c %s  %s [%s]

# DefCon Messages

OPER_DEFCON_SYNTAX
	Синтаксис: DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	Сервисы работают в режиме DEFCON, Пожалуйста, попытайтесь позже.
OPER_DEFCON_NO_CONF
	Режим DEFCON должен быть включен в файле services.conf
OPER_DEFCON_CHANGED
	Сейчас сервисы в режиме DEFCON %d
OPER_DEFCON_WALL
	%s изменил уровень DEFCON на %d
DEFCON_GLOBAL
	Уровень DEFCON теперь равен: %d

# Module strings

OPER_MODULE_LOADED
	Модуль %s загружен
OPER_MODULE_UNLOADED
	Модель %s выгружен
OPER_MODULE_LOAD_FAIL
	Невозможно загрузить модуль %s
OPER_MODULE_REMOVE_FAIL
	Невозможно выгрузить модуль %s
OPER_MODULE_NO_UNLOAD
	Этот модуль не может быть выгружен.
OPER_MODULE_LOAD_SYNTAX
	MODLOAD ИмяФайла
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD ИмяФайла
OPER_MODULE_LIST_HEADER
	Текущий список модулей:
OPER_MODULE_LIST
	Модуль: %s [%s] [%s]
OPER_MODULE_LIST_FOOTER
	%d модулей загружено.
OPER_MODULE_INFO_LIST
	Модуль: %s Версия: %s Автор: %s загружен: %s
OPER_MODULE_CMD_LIST
	Допустимые команды: %R%s %s
OPER_MODULE_MSG_LIST
	Допустимые IRCD заголовки для: %s
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
	EXCEPTION ADD [+время_истечения] маска лимит причина
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {маска | список_записей}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE номер позиция
OPER_EXCEPTION_DISABLED
	Ограничение сессий отключено.
OPER_EXCEPTION_ALREADY_PRESENT
	Маска %s уже есть в списке исключений.
OPER_EXCEPTION_TOO_MANY
	Список исключений из лимита сессий переполнен!
OPER_EXCEPTION_ADDED
	Лимит сессий для %s установлен на %d.
OPER_EXCEPTION_MOVED
	Исключение для %s (#%d) перемещено в позицию %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Нет такой записи (#%d) в списке исключений из лимита сессий.
OPER_EXCEPTION_NOT_FOUND
	%s не найден в списке исключений из лимита сессий.
OPER_EXCEPTION_NO_MATCH
	Нет таких значений в списке исключений из лимита сессий.
OPER_EXCEPTION_DELETED
	%s удален из списка исключений лимита сессий.
OPER_EXCEPTION_DELETED_ONE
	Удалена 1 запись из списка исключений лимита сессий.
OPER_EXCEPTION_DELETED_SEVERAL
	Удалено %d записей из списка исключений лимита сессий.
OPER_EXCEPTION_LIST_HEADER
	Текущий список исключений из лимита сессий:
# маска, лимит
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Номер  Лимит  Хост
# меска, поставил, когда, истекает, лимит, причина
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (by %s on %s; %s)
	    Лимит: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Некорректный лимит сессий. Он должен быть целым числом больше или равным нулю и меньше %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Некорректная хостмаска. Только реальная хостмаска может быть добавлена в исключения, она не должна содержать ников или идентов пользователей.
OPER_EXCEPTION_EXISTS
	%s уже есть в списке ИСКЛЮЧЕНИЙ.
OPER_EXCEPTION_CHANGED
	Исключение для %s было обновлено на %d.

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
	Хосты с количеством сесси %d и более:
OPER_SESSION_LIST_COLHEAD
	Сессия  Хост
# лимит, хост
OPER_SESSION_LIST_FORMAT
	%6d    %s
# хост, сессии, лимит
OPER_SESSION_VIEW_FORMAT
	Хост %s сейчас используют %d сессий из %d максимально допустимых.

# EXCEPTION help

OPER_HELP_EXCEPTION
	Синтаксис: EXCEPTION ADD [+время истечения] маска лимит причина
	           EXCEPTION DEL {маска | список_записей}
	           EXCEPTION MOVE номер позиция
	           EXCEPTION LIST [маска | список_записей]
	           EXCEPTION VIEW [маска | список_записей]
	
	Позволяет Администраторам сервисов управлять списком хостов,
	имеющих отдельные ограничения на количество сессий - разрешит определеным машинам, 
	например шелл-серверам, иметь бОльшее количество клиентов одновременно, чем
	разрешено по умолчанию. Как только количество сессий с хоста достигнет лимита,
	все последующие клиенты, пытающиеся коннектиться с данного хоста будут
	прибиваться. Перед отсоединением пользователя, он будет уведомлен
	/NOTICE'ом от %S, что с данного хоста превышен лимит
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

OPER_HELP_SESSION
	Синтаксис: SESSION LIST порог
	           SESSION VIEW хост
	
	Позволяет Администраторам сервисов просмотреть список сессий.
	
	SESSION LIST покажет хосты с меньшим, чем порог количеством сессий.
	Порог должен быть целым и больше 1. Это предохранит
	от случайной выдачи списка с большим числом единичных
	сессий.
	SESSION VIEW покажет детальную информацию об указанном
	хосте - включая текущий счетчик сессий и лимит сессий.
	Хосты не могут включать в себя символьные маски (*, ? и т.п.).
	
	См. EXCEPTION help для более подробной информации о лимитировании
	сессий и как установить отдельный лимит сессий для определенного
	хоста или группы хостов.

OPER_HELP_STAFF
	Синтаксис: STAFF

	Показывает всех, прописанных на сервисах (Операторов, Администраторов, Рутов) 
	включая их уровень и онлайн статус.

OPER_HELP_DEFCON
	Синтаксис: DEFCON [1|2|3|4|5]

	Система DEFCON может быть использована для установки определенных
	сервисных ограничений, удобно использовать при попытках
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
	* Игнорировать любые сообщения, за исключением сообщений ирк-операторов, и оповещать об игноре.
OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* Игнорировать всех, кроме ирк-операторов, БЕЗ оповещения об этом.
OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* Ставить AKILL всем новым соединениям (с этого момента).
OPER_HELP_DEFCON_NO_NEW_MEMOS
	* Запрет отправлять мемо-сообщения
OPER_HELP_CHANKILL
	Синтаксис: CHANKILL [+истечение] #канал причина

	Установить AKILL для каждого ника, зашедшего на определенный канал. Будет
	использован полностью реальный идент@хост для каждого ника,
	который подпадет под данный AKILL. 

###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Новость - %s] %s
NEWS_OPER_TEXT
	[Новости Операторов - %s] %s
NEWS_RANDOM_TEXT
	[Случайная новость - %s] %s

NEWS_LOGON_SYNTAX
	Синтаксис: LOGONNEWS {ADD|DEL|LIST} [текст|номер]
NEWS_LOGON_LIST_HEADER
	Текущие новости:
NEWS_LOGON_LIST_ENTRY
	%5d (%s от %s)
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
	Все новости были удалены.

NEWS_OPER_SYNTAX
	Синтаксис: OPERNEWS {ADD|DEL|LIST} [текст|номер]
NEWS_OPER_LIST_HEADER
	Список новостей для Операторов:
NEWS_OPER_LIST_ENTRY
	%5d (%s от %s)
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
	%5d (%s от %s)
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
	Синтаксис: RANDOMNEWS ADD текст
	           RANDOMNEWS DEL {текст | ALL}
	           RANDOMNEWS LIST
	
	Управление списком случайных новостей. Когда пользователь
	заходит в IRC сеть, одна (и только одна) из
	случайных новостей будет выбрана наугад и отправлена ему.
	
	Команда RANDOMNEWS LIST может быть использована любым
	IRC-Оператором для просмотра текущих новостей. Команды
	ADD и DEL могут использовать только Администраторы сервисов.

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S это сервис помощи, который сообщит вам информацию об
	основных сервисах сети.
	Начните работу с сервисами с команд:

	%R%s HELP
	     для информации о регистрации ников

	%R%s HELP
	     для информации о регистрации каналов
	     и управления ими
	     
	%R%s HELP
	     для информации о том, как оставлять сообщения
	     пользователям, которые на данный момент не в сети.

HELP_HELP_BOT

	%R%s HELP
	     для информации о том, как установить сервисного бота на свой канал

HELP_HELP_HOST

	%R%s HELP
	     для информации о том, как установить на ник vHost (виртуальный хост).

###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP_CMD_CONFIRM
	    CONFIRM    Подтвердить регистрацию кодом аутенфикации
NICK_HELP_CMD_RESEND
	    RESEND     Переслать код аутенфикации еще раз
NICK_HELP_CMD_REGISTER
	    REGISTER   Регистрация ника
NICK_HELP_CMD_GROUP
	    GROUP      Объединение ников в группы
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY   Идентификация Вас как владельца ника
NICK_HELP_CMD_ACCESS
	    ACCESS     Управление списком доступа
NICK_HELP_CMD_SET
	    SET        Установка параметров, включая защиту kill'ом
NICK_HELP_CMD_SASET
	    SASET      Устанавливает SET-параметры какого-либо ника
NICK_HELP_CMD_DROP
	    DROP       Отмена регистрации ника (удаление)
NICK_HELP_CMD_RECOVER
	    RECOVER    Сменить ник пользователя, взявшего Ваш ник
NICK_HELP_CMD_RELEASE
	    RELEASE    Снятие защиты сервисов с Вашего ника после команды RECOVER
NICK_HELP_CMD_SENDPASS
	    SENDPASS   Забыли пароль? Попробуйте эту команду
NICK_HELP_CMD_GHOST
	    GHOST      Закрыть "мертвую" IRC-сессию, использующую ваш ник
NICK_HELP_CMD_ALIST
	    ALIST      Показывает список каналов, на которых у вас есть доступ
NICK_HELP_CMD_GLIST
	    GLIST      Список всех ников в группе
NICK_HELP_CMD_INFO
	    INFO       Информация о каком-либо нике
NICK_HELP_CMD_LIST
	    LIST       Список всех зарегистрированных ников по маске
NICK_HELP_CMD_LOGOUT
	    LOGOUT     Деидентифицирует вас от используемого вами ника
NICK_HELP_CMD_STATUS
	    STATUS     Возвращает статус какого-либо ника на NickServ
NICK_HELP_CMD_UPDATE
	    UPDATE     Обновляет ваш текущий статус (проверка на новые мемо и т. д.)
NICK_HELP_CMD_GETPASS
	    GETPASS    Возвращает пароль ника
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Выдает список всех пользователей, которые зарегистрировались с данным e-mail
NICK_HELP_CMD_FORBID
	    FORBID     Предохраняет ник от использования и регистрации
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Устанавливает режим саспенда ("заморозки") на ник
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Снимает режим саспенда с ника

NICK_HELP
	%S это сервис, предназначенный для "регистрации"
	Вашего ника и его защиты от использования еще кем-либо,
	кроме Вас. Нижеупомянутые команды предназначены для регистрации 
	ника и управления его настройками, чтобы использовать данные
	команды, напишите %R%S команда.
	Для более подробной информации по какой-либо
	команде напишите: %R%S HELP команда.

	
NICK_HELP_FOOTER

	ПРИМЕЧАНИЕ: Данный сервис предназначен для того,
	чтобы гарантировать подлинность того или иного пользователя
	IRC и защитить его ник от использования посторонним лицом.
	NickServ НЕ предназначен для облегчения "воровства" ников,
	"киберсквоттинга" (захвата большого количества ников)
	или других зловредных действий. Злоупотребление %S приведет,
	как минимум, к уничтожению Вашего ника(ников).
	
NICK_HELP_EXPIRES
	
	Неиспользуемые в течение длительного времени ники
	автоматически удаляются из базы данных. Ник будет удален,
	если его не использовать в течение %d дней.

NICK_HELP_REGISTER
	Синтаксис: REGISTER пароль [email]
	
	Регистрирует Ваш ник в базе данных %S.  После того, как Вы
	зарегистрировали ник, вы можете использовать команды SET и ACCESS,
	чтобы сконфигурировать параметры Вашего ника по Вашему вкусу.
	Не забывайте пароль, который Вы укажете при регистрации -
	он будет нужен вам для дальнейшего использования ника!
	(Примечание: пароли чувствительны к регистру!
	MYPASSWORD, MyPassword, и mypassword - это РАЗНЫЕ пароли!)

	Принципы выбора пароля:
	
	Пароли не должны быть легко угадываемы.  Например,
	использование Вашего реального имени в качестве пароля - плохая идея.
	Использование вашего ника в качестве пароля совсем плохая идея ;),
	да и %S не позволит этого. Также, короткие пароли очень легко
	подбираются простым перебором, поэтому длина пароля должна быть
	больше 5 символов. Пробелы и знаки табуляции в пароле недопустимы.
	Используйте в качестве пароля только латинские буквы или цифры.
	
	Параметр email является опциональным и, если указан,
	сразу же ассоциирует с вашим ником указанный email.
	Однако, в большинстве сетей, параметр email
	может быть обязательным.
	Ваша конфидециальность будет соблюдена - этот e-mail
	не будет передан третьим лицам.
	
	Так же, команда регистрации автоматически создает новую группу
	для Вашего ника, использование которой позволит регистрировать
	другие ники,которые будут являться частью главного ника -
	общими будут настройки, мемо-сообщения и привелегии на каналах.
	Для получения более полной информации напишите %R%S HELP GROUP.
	
NICK_HELP_GROUP
	Синтаксис: GROUP главный_ник пароль
	
	Эта команда позволяет Вам присоединить Ваш ник
	к группе главного_ника. пароль - это пароль главного ника.
	
	Использование групп позволит Вам объединить Ваши настройки,
	получать и отправлять короткие сообщения, использовать привелегии
	на каналах всеми никами группы и многое другое!
	 
	Группа существует до тех пор, пока используется.
	Если Вы удалите (дропнете) один из ников в группе,
	Вы не потеряете регистрацию по крайней мере до тех пор, пока,
	в группе остается хотя бы 1 ник.
	
	Вы можете использовать эту команду, даже если Вы все еще
	не зарегистрировали Ваш ник. Если Ваш ник уже зарегистрирован,
	Вам необходимо идентифицироваться перед использованием этой команды.
	Напишите %R%S HELP IDENTIFY для получения более полной информации.
	Впрочем, есть вероятность, что в Вашей IRC-сети это будет невозможно.
	
	Рекомендуется использовать эту команду с незарегистрированного
	ника, потому что он автоматически зарегистрируется при успешном
	использовании данной команды. Вы также можете использовать эту
	команду с зарегистрированного ника (чтобы перейти в другую группу)
	только если администраторы Вашей IRC сети это разрешили.
	
	Вы можно быть только в одной группе одновреиенно. Слияние групп
	невозможно.
	
	Примечание: У всех ников в группе единый пароль.

NICK_HELP_IDENTIFY
	Синтаксис: IDENTIFY пароль
	
	Сообщает %S что Вы действительно являетесь владельцем
	ника. Многие команды требуют идентификации, без этого
	Вы не сможете их использовать. Пароль должен быть
	полностью идентичен тому, который вы указывали, когда 
	использовали команду REGISTER.
	
NICK_HELP_UPDATE
	Синтаксис: UPDATE

	Обновляет Ваш текущий статус, т.е. проверяет наличие новых мемок,
	устанавливает необходимые режимы на каналах (ModeonID) и обновляет ваши
	виртуальные хосты и флаги (последнее время посещения IRC, и т.д.).

NICK_HELP_LOGOUT
	Синтаксис: LOGOUT
	
	Эта команда имеет эффект, обратный эффекту команды IDENTIFY, т.е.
	снимает с Вас статус владельца ника (ДЕидентифицирует) 
	Примечание: тем не менее, после использования данной команды
	NickServ не попросит вас идентифицировать себя снова.

NICK_HELP_DROP
	Синтаксис: DROP [ник]
	
	Удаляет ваш ник из базы данных %S. Удаленный таким образом ник
	становится свободным для регистрации любым человеком.
	
	Вы можете удалять ник в пределах Вашей группы,
	используя дополнительный параметр ник.

	Что бы использовать данную команду, Вы должны
	идентифицироваться на NickServ как владелец ника.
	(%R%S HELP IDENTIFY для более полной информации).

NICK_HELP_ACCESS
	Синтаксис: ACCESS ADD маска
	           ACCESS DEL маска
	           ACCESS LIST
	
	Позволяет менять/просматривать список масок доступа на Ваш ник.
	Имеется в виду список адресов, с которых %S позволит использовать
	Ваш ник. Если Вы хотите использовать ник с различных адресов,
	Вам нобходимо использовать команду IDENTIFY чтобы %S
	идентифицировал Вас.
	
	Примеры:
	
	    ACCESS ADD anyone@*.set.ru
	        Разрешает доступ пользователю с идентом anyone
		с любого компьютера в домене set.ru.
	
	    ACCESS DEL anyone@*.set.ru
	        Антоним предыдущей команды - удаляет маску.
	
	    ACCESS LIST
	        Покажет текущий список адресов доступа.

NICK_HELP_SET
	Синтаксис: SET опция параметры
	
	Устанавливает различные опции ника. Опция может быть:
	
	    DISPLAY    Установить главный ник группы
	    PASSWORD   Изменить пароль на ник
	    LANGUAGE   Установить язык, на котором сервисы будут
	                   посылать Вам сообщения
	    URL        Ассоциировать URL-адрес с Вашим ником
	    EMAIL      Ассоциировать Email-адрес с Вашим ником
	    ICQ        Ассоциировать ICQ-номер с Вашим ником
	    GREET      Ассоциировать приветствие с Вашим ником
	    KILL       Включить или выключить режим защиты ника.
	    SECURE     Включить или выключить режим безопасности ника 
	    PRIVATE    Предохранить ваш ник от появления в 
	                   %R%S LIST
	    HIDE       Скрыть какую-либо информацию о Вас
	    MSG        Выбрать метод общения сервисов с Вами - приват или нотисы
	    AUTOOP     Будут ли сервисы ставить вам статус автоматически.    
	
	Перед установкой любой опции, Вы должны сначала
	идентифицироваться с Вашим паролем
	(%R%S HELP IDENTIFY для более подробной информации).
	
	Напишите %R%S HELP SET опция для более подробной
	информации по конкретной опции.

NICK_HELP_SET_DISPLAY
	Синтаксис: SET DISPLAY новый_главный_ник
	
	Изменяет Ваш главный ник в группе.  
	Новый главный ник должен присутствовать в Вашей группе.

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
	
	Ассоциирует URL-адрес с Вашим ником.  Этот URL
	будет показан, когда кто-либо запросит информацию
	о Вашем нике с помощью команды INFO.

NICK_HELP_SET_EMAIL
	Синтаксис: SET EMAIL адрес@email
	
	Ассоциирует Email-адрес с Вашим ником. Этот адрес
	будет показан, когда кто-либо запросит информацию
	о Вашем нике с помощью команды INFO.
	
NICK_HELP_SET_ICQ
	Синтаксис: SET ICQ номер ICQ
	
	Ассоциирует ICQ-номер с Вашим ником. Этот номер
	будет показан, когда кто-либо запросит информацию
	о Вашем нике с помощью команды INFO.
	
NICK_HELP_SET_GREET
	Синтаксис: SET GREET сообщение
	
	Создает приветсвие для Вашего ника, которое
	будет показываться при заходе на канал, на котором
	включена опция GREET и висит сервисный бот. Учтите,
	что Вам понадобится необходимый уровень доступа
	на этот канал, чтобы использовать приветствие.

NICK_HELP_SET_KILL
	Синтаксис: SET KILL {ON | QUICK | IMMED | OFF}
	
	Активирует\деактивирует автоматическую защиту для Вашего ника.
	Когда защита включена, любой другой пользователь, который
	попытается взять ваш ник, будет иметь 1 минуту для того, чтобы
	выбрать себе другой ник, иначе %S принудительно изменит его ник.
	
	Если Вы выберете QUICK, у пользователя будет только 20 секунд
	на то, чтобы сменить ник.  Если вы выберете IMMED,
	ник пользователя будет изменен немедленно, БЕЗ 
	предупреждения и возможности сменить ник самому.
	Пожалуйста, не используйте эту опцию без КРАЙНЕЙ
	необходимости. Также, администратор вашей IRC-сети
	может отключить эту опцию.

NICK_HELP_SET_SECURE
	Синтаксис: SET SECURE {ON | OFF}
	
	Активирует\деактивирует опцию Безопасности на %S для Вашего
	ника. При включенной SECURE, Вы должны идентифицироваться
	на NickServ через пароль, чтобы подтвердить, что именно Вы
	владелец данного ника, при этом на список доступа сервисы
	не обращают внимания. Тем не менее, если Вы есть в списке доступа,
	%S не будет автоматически менять Ваш ник, не обращая внимания
	на установку опции KILL.

NICK_HELP_SET_PRIVATE
	Синтаксис: SET PRIVATE {ON | OFF}
	
	Активирует\деактивирует опцию приватности вашего ника на %S.
	Когда PRIVATE включена, Ваш ник не будет показываться
	по команде %S's LIST.
	(Тем не менее, любой, кто знает Ваш ник, имеет возможность получить
	информацию о Вас, используя команду INFO.)

NICK_HELP_SET_HIDE
	Синтаксис: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Позволяет Вам скрыть часть информации о Вас, которая
	будет показываться по команде %S INFO на Ваш ник.
	Вы можете скрыть Ваш email-адрес (EMAIL), время
	последнего захода в сеть и маску, с которой Вы
	заходили (USERMASK), а так же, последнее сообщение
	при выходе (QUIT).
	Второй параметр определяет, будет ли эта информация
	показываться (OFF) или скрываться (ON).
	
NICK_HELP_SET_MSG
	Синтаксис: SET MSG {ON | OFF}
	
	Позволяет Вам выбрать способ сообщения сервисов с Вами.
	Если установить режим MSG, сервисы будут использовать
	в общении с вами приватные сообщения, в ином случае 
	будут использовать нотисы.

NICK_HELP_SET_AUTOOP
	Синтаксис: SET AUTOOP {ON | OFF}
	
	Устанавливает режим автоматического получения статуса.
	ВКЛЮЧИТЕ (ON), если хотите, что бы ChanServ автоматически
	давал вам статус при входе на канал.

NICK_HELP_SASET
	Синтаксис: SASET ник опции параметры.
	
	Устанавливает различные опции ника.  Опции могут быть:
	
	    DISPLAY    Устанавливает главный ник группы
	    PASSWORD   Изменяет пароль ника
	    URL        Ассоциирует URL-адрес с ником
	    EMAIL      Ассоциирует Email-адрес с ником
	    ICQ        Ассоциирует ICQ-номер с ником
	    GREET      Устанавливает приветствие на ник.
	    KILL       Включает или выключает Защиту для ника
	    SECURE     Включает или выключает Безопасность для ника
	    PRIVATE    Скрывает появление ника в списке ников по
	                   %R%S LIST
	    HIDE       Скрывает какую-либо информацию о нике
	    MSG        Изменяет режим сообщения с сервисами
	    NOEXPIRE   Устанавливает на ник режим не-истечения
	    AUTOOP     Включает или выключает режим автостатуса
	    LANGUAGE   Set the language Services will use when
	               sending messages to nickname
	
	Введите %R%S HELP SASET опция для большей информации
	о какой-либо опции. Опции будут установлены для указаного
	ника. 

NICK_HELP_SASET_DISPLAY
	Синтаксис: SASET ник DISPLAY новый_главный_ник 
	
	Изменяет главный ник в группе ников. Новый главный ник 
	ДОЛЖЕН находится в группе указанного ника.

NICK_HELP_SASET_PASSWORD
	Синтаксис: SASET ник PASSWORD новый_пароль 
	
	Изменяет пароль, который используется для идентификации как владелец ника.

NICK_HELP_SASET_URL
	Синтаксис: SASET ник URL url-адрес
	
	Ассоциирует указанный URL с указаным ником. Этот URL
	будет показан в информации, когда кто-то запросит ее,
	используя команду INFO.

NICK_HELP_SASET_EMAIL
	Синтаксис: SASET ник EMAIL адрес
	
	Ассоциирует указанный Email-адрес с указанным ником.
	
NICK_HELP_SASET_ICQ
	Синтаксис: SASET ник ICQ номер
	
	Ассоциирует указанный ICQ-номер с указаным ником. Этот номер
	будет показан в информации, когда кто-то запросит ее,
	используя команду INFO.
	
NICK_HELP_SASET_GREET
	Синтаксис: SASET ник GREET текст
	
	Устанавливает приветствие на указанный ник, которое будет
	сказано ботом, когда ник заходит на какой либо канал, где функция
	ПРИВЕТСТВИЯ включена, и ник имеет какой-либо доступ на этом канале.

NICK_HELP_SASET_KILL
	Синтаксис: SASET ник KILL {ON | QUICK | IMMED | OFF}
	
	Активирует\деактивирует автоматическую защиту для указанного ника.
	Когда защита ВКЛЮЧЕНА (ON), любой другой пользователь, который
	попытается взять указанный ник, будет иметь 1 минуту для того, чтобы
	выбрать себе другой ник, иначе %S принудительно сменит его ник.
	
	Если Вы выберете QUICK, у пользователя будет только 20 секунд
	чтобы сменить ник.  Если вы выберете IMMED, ник пользователя
	будет изменен немедленно, без предупреждения и шанса сменить ник.
	Пожалуйста, не используйте опцию IMMED без КРАЙНЕЙ необходимости.
	Также, администратор вашей IRC сети может отключить эту опцию. 

NICK_HELP_SASET_SECURE
	Синтаксис: SASET ник SECURE {ON | OFF}
	
	Активирует\деактивирует опции Безопасности на %S для указанного
	ника. При включенном SECURE, ник должны идентифицироваться
	на NickServ, чтобы подтвердить, что именно он владелец данного
	ника, при этом на список доступа сервисы не обращают внимания.
	Тем не менее, если ник есть в списке доступа, %S
	не будет автоматически переименовывать его, не обращая внимания
	на установку опции KILL. 

NICK_HELP_SASET_PRIVATE
	Синтаксис: SASET ник PRIVATE {ON | OFF}
	
	Активирует\деактивирует опций приватности на %S для указанного ника.
	Когда PRIVATE включена, указанный ник не будет показываться
	по команде %S's LIST.
	(Тем не менее, любой, кто знает данный ник, имеет возможность
	получить информацию о нем, используя команду INFO.) 

NICK_HELP_SASET_HIDE
	Синтаксис: SASET ник HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	Устанавливает опции скрытия информации для указаного ника,
	которая будет показываться по команде %S INFO на данный ник.
	Вы можете скрыть email-адрес (EMAIL) ника, время последнего
	захода в сеть и маску (идент@хост), с которой он заходил (USERMASK),
	доступ на сервисах (STATUS) и последнее сообщение выхода (QUIT).
	Второй параметр определяет, будет ли эта информация
	показываться (OFF) или скрываться (ON). 
	
NICK_HELP_SASET_MSG
	Синтаксис: SASET ник MSG {ON | OFF}
	
	Позволяет установить способ общения сервисов с указаным ником.
	Если установить режим MSG, сервисы будут использовать приватные
	сообщения, в ином случае будут использоваться нотисы. 

NICK_HELP_SASET_NOEXPIRE
	Синтаксис: SASET ник NOEXPIRE {ON | OFF}
	
	Устанавливает, будет ли распространяться правило истечения
	регистрации на указанный ник. Установите ON, если хотите
	активировать режим не-истечения для данного ника.

NICK_HELP_SASET_AUTOOP
	Синтаксис: SASET ник AUTOOP {ON | OFF}
	
	Активирует\деактивирует режим автоматического получения статуса
	на каналах. Установите в ON, если хотите, что бы ChanServ
	автоматически давал статус нику при заходе на канал.

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
	Синтаксис: GHOST ник [пароль]
	
	Уничтожает "мертвое" IRC соединение, использующее Ваш ник.
	"Мертвое" соединение - это когда реально его не существует,
	но когда IRC сервер думает, что оно все еще онлайн.
	Обычно это происходит, если Ваш компьютер перезагрузился,
	или Ваше соединение с локальной сетью или Интернет
	оборвалось в тот момент, когда Вы были в IRC.
	
	Порядок использования команды GHOST для ника: Ваш
	текущий адрес, который виден по команде /WHOIS должен
	быть одним из адресов в списке доступа, вы должны быть
	идентифицированы и находиться в группе этого ника, или,
	Вы должны ввести верный пароль на этот ник.

NICK_HELP_INFO
	Синтаксис: INFO ник [ALL]
	
	Показывает информацию о нике: имя владельца ника,
	его последнюю маску и время, когда он заходил в IRC,
	параметры ника. Если Вы идентифицировали Ваш ник, Вы 
	получите больше информации о Вашем нике, если укажете
	параметр ALL, вне зависимости от того, скрыта она или нет.

NICK_HELP_LIST
	Синтаксис: LIST маска
	
	Показывает список всех зарегистрированных ников,
	которые содержат введенную маску, в формате ник!идент@хост.
	Ники, у которых включена опция PRIVATE, не будут показаны.
	
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
	Синтаксис: ALIST [уровень]
	
	Покажет все каналы, в списках доступа которых Вы числитесь.
	Опционально, Вы можете выбрать определенный уровень в формате
	xOP или ACCESS. Результирующий список выведет каналы, на которых
	у Вас есть соответствующий уровень.

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
	
	Каналы, помеченные как NOEXPIRE, будут показаны с восклицательным знаком
	в качестве префикса.
	
NICK_HELP_GLIST
	Синтаксис: GLIST
	
	Показывает все ники в Вашей группе.

NICK_HELP_STATUS
	Синтаксис: STATUS ник1 ник2 ник3...
	
	Возвращает информацию о статсуе ника - идентифицировался ли
	использующий его как владелец ника. Ответ выводится
	в формате:
	
	    ник код-статуса
	
	где ник это ник, который Вы послали при вводе команды, и
	код-статуса может быть одним из:
	
	    0 - такого пользователя нет в онлайне или этот ник не зарегистрирован
	    1 - пользователь не идентифицирован как владелец ника
	    2 - пользователь идентифицирован как владелец ника только по списку доступа
	    3 - пользователь идентифицирован как владелец ника через идентификацию паролем
	
	До 16 ников сразу может быть запрошено этой командой,
	остальные будут проигнорированы. Если ник не указан,
	возвратит Ваш текущий статус

NICK_HELP_SENDPASS
	Синтаксис: SENDPASS ник
	
	Отправляет пароль ника на email-адрес указанный
	при регистрации ника. Эту команду учень удобно
	использовать, чтобы восстановить забытый пароль.
	
	Может быть ограничена для использования только
	IRC-операторами данной сети.
	
	При включенном шифровании паролей эта команда недоступна.

NICK_HELP_CONFIRM
	Синтаксис: CONFIRM auth-код
	
	Данная команда необходима, что бы завершить второй шаг
	регистрации ника - подтверждение. После выполнения данного
	тербования, ваш ник будет зарегистрирован у %S.
	Кодовая фраза, которая указывается в качестве параметра,
	высылается на ваш e-mail в первой части процесса регистрации
	ника. Для более подробной информации используйте:
	%R%S HELP REGISTER

NICK_HELP_RESEND
	Syntax: RESEND
	
	This command will re-send the auth code (also called passcode)
	to the e-mail address of the user whom is performing it.

NICK_SERVADMIN_HELP
	Администраторы сервисов могут удалить любой ник
	без идентификации к нему, а так же, могут смотреть
	список доступа для любого ника (%R%S ACCESS LIST ник).

NICK_SERVADMIN_HELP_LOGOUT
	Синтаксис: LOGOUT [ник [REVALIDATE]]
	
	Без параметров, совершает обратный IDENTIFY эффект,
	т.е. делает Вас неидентифицированным как владельца ника.
	Однако, сервисы не будут просить Вас идентифицировать
	себя заново.
	
	Если ввести ник как параметр, деидентифицирует указанный ник.
	Если же Вы укажете еще и REVALIDATE, сервисы попросят данный ник
	переидентифицировать себя. Данные параметры могут использовать
	только Администраторы сервисов.

NICK_SERVADMIN_HELP_DROP
	Синтаксис: DROP [ник]
	
	Без параметров, удаляет Ваш ник из базы данных %S.
	
	С параметром, удаляет указанный ник из базы данных.
	Вы можете удалить любой ник из Вашей группы не имея
	каких-либо особых привилегий. Удалить абсолютно ЛЮБОЙ
	ник могут только Администраторы сервисов.

NICK_SERVADMIN_HELP_INFO
	
	Администраторы сервисов могу использовать параметр
	ALL для любого ника.

NICK_SERVADMIN_HELP_LIST
	Синтаксис: LIST маска [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	Показывает список всех зарегистрированных ников которые попадают под
	введенную маску, в формате ник!идент@хост. Ники, у которых включена
	опция PRIVATE, будут показаны только Администраторам сервисов. Ники
	с включенным параметром NOEXPIRE будут показаны с префиксом ! перед
	ником.
	
	Если указаны опции FORBIDDEN, SUSPENDED, NOEXPIRE или UNCONFIRMED, будут
	показаны только те ники, которые в режиме FORBIDDEN (запрещен), SUSPENDED,
	UNCONFIRMED (не подтвержден) или имеют установку
	NOEXPIRE (режим не-истечения). Если указаны все три опци,
	все три типа ников будут показаны. 
	
	Примеры:
	
	    LIST *!joeuser@foo.com
	    Покажет все зарегистрированные ники, владелец которых является
	    joeuser@foo.com.
	
	    LIST *Bot*!*@*
	    Покажет все зарегистрированные ники, в которых есть слово Bot
	    (вне зависимости от регистра).
	
	    LIST * NOEXPIRE
	    Покажет все зарегистрированные ники у которых включена опция
	    NOEXPIRE.

NICK_SERVADMIN_HELP_ALIST
	Синтаксис: ALIST [ник] [уровень]
	
	Без параметров, покажет список каналов, на которых Вы прописаны.
	С первым параметром, покажет каналы, на которых прописан указанный
	ник. С обоими параметрами, покажет каналы, где прописан указанный
	ник с указанным уровнем или выше.

	Для использования этой команды необходимы права Администратора сервисов.

NICK_SERVADMIN_HELP_GLIST
	Синтаксис: GLIST [ник]
	
	Без параметров, покажет список ников в Вашей группе.
	
	С параметром, покажет все ники, находящиеся в 
	группе указанного ника.
	Для использования этой команды необходимы права Администратора сервисов.

NICK_SERVADMIN_HELP_GETPASS
	Синтаксис: GETPASS ник
	
	Возвращает пароль указанного ника.  Примечание: при
	использовании данной команды, по WALLOPS/GLOBOPS
	посылается сообщение, включающее ник,
	который использовал данную команду и ник, пароль
	которого был запрошен. Одновременно с этим, сообщение
	записывается в лог.
	
	Данная команда недоступна, если включено шифрование паролей.

NICK_SERVADMIN_HELP_GETEMAIL
	Синтаксис: GETEMAIL user@emailhost

	Возвращает ники, у которых в настройках стоит указанный email.
	Примечание: вы не можете использовать симольные маски ни для user,
	ни для emailhost. Каждый раз при использовании данной команды,
	сообщение, включающее ник вызвавшего команду и указанный email,
	будет записано в лог.

NICK_SERVADMIN_HELP_FORBID
	Синтаксис: FORBID ник [причина]
	
	Запрещает регистрировать или использовать ник указанный ник.
	Может быть отменено путем удаления ника (DROP).
	
	В некоторых сетях, причина является обязательным параметром.

NICK_SERVADMIN_HELP_SUSPEND
	Синтаксис: SUSPEND ник причина

	Устанавливает режим саспенда ("заморозки") на ник,
	предотвращая возможность идентификации к нему или использования.

NICK_SERVADMIN_HELP_UNSUSPEND
	Syntax: UNSUSPEND ник

	Симает режим саспенда с ника.

###########################################################################
#
# ChanServ help messages
#
###########################################################################

CHAN_HELP_CMD_GETPASS
	    GETPASS    Получить пароль владельца канала
CHAN_HELP_CMD_FORBID
	    FORBID     Запретить использовать канал
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    Приостановить работу канала, с сохранением
	               данных и настроек канала
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Восстановить работу приостановленного канала.
CHAN_HELP_CMD_STATUS
	    STATUS     Возвращает текущий уровень доступа для указанного
	               пользователя на канале.
CHAN_HELP_CMD_REGISTER
	    REGISTER   Регистрация канала
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   Идентифицироваться как владелец канала
CHAN_HELP_CMD_SET
	    SET        Установить информацию о канале и его опции
CHAN_HELP_CMD_AOP
	    AOP        Управляет списком AOP'ов на канале
CHAN_HELP_CMD_SOP
	    SOP        Управляет списком SOP'ов на канале
CHAN_HELP_CMD_ACCESS
	    ACCESS     Управляет ACCESS-списком привилегий пользователей
CHAN_HELP_CMD_LEVELS
	    LEVELS     Переустановить значения уровней доступа
CHAN_HELP_CMD_AKICK
	    AKICK      Управление списком автокиков
CHAN_HELP_CMD_DROP
	    DROP       Удалить канал из базы данных ChanServ
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Поможет восстановить забытый пароль
CHAN_HELP_CMD_BAN
	    BAN        Банит указанного пользователя на канале
CHAN_HELP_CMD_CLEAR
	    CLEAR      Очищает определенные режимы на канале
CHAN_HELP_CMD_DEVOICE
	    DEVOICE    Снимает режим войса (-v) с указанного ника
CHAN_HELP_CMD_GETKEY
	    GETKEY     Возвращает ключ канала
CHAN_HELP_CMD_INFO
	    INFO       Выдает информацию об указанном канале
CHAN_HELP_CMD_INVITE
	    INVITE     Заставляет ChanServ пригласить вас на указанный канал.
CHAN_HELP_CMD_KICK
	    KICK       Выкидывает указанного пользователя с канала
CHAN_HELP_CMD_LIST
	    LIST       Список всех зарегистрированных каналов по указанной маске
CHAN_HELP_CMD_LOGOUT
	    LOGOUT     Данная команда деидентифицирует указанный ник от канала
CHAN_HELP_CMD_OP
	    OP         Дает статус оператора (+o) указанному нику на канале
CHAN_HELP_CMD_TOPIC
	    TOPIC      Позволяет устанавливать топик на канале посредством ChanServ
CHAN_HELP_CMD_UNBAN
	    UNBAN      Удаляет все баны , которые не позволяют вам зайти на канал
CHAN_HELP_CMD_VOICE
	    VOICE      Дает статус голоса (+v) указаному нику на канале
CHAN_HELP_CMD_VOP
	    VOP        Управляет списокм VOP'ов на канале
CHAN_HELP_CMD_DEHALFOP
	    DEHALFOP   Снимает статус полуоператора (-h) с указанного ника на канале
CHAN_HELP_CMD_DEOWNER
	    DEOWNER    Снимает с вас статус владельца канала
CHAN_HELP_CMD_DEPROTECT
	    DEPROTECT  Снимает режим защиты с указанного ника на канале
CHAN_HELP_CMD_HALFOP
	    HALFOP     Дает статус полуоператора (+h) указанному нику на канале
CHAN_HELP_CMD_HOP
	    HOP        Управляет списоком HOP'ов на канале
CHAN_HELP_CMD_OWNER
	    OWNER      Дает вам статус владельца канала
CHAN_HELP_CMD_PROTECT
	    PROTECT    Устанавливает режим защиты указаному нику на канале
CHAN_HELP_CMD_ADMIN
	    ADMIN      Устанавливает режим защиты указаному нику на канале
CHAN_HELP_CMD_DEADMIN
	    DEADMIN    Снимает режим защиты с указанного ника на канале
CHAN_HELP_CMD_DEOP
	    DEOP       Снимает статус оператора (-o) с указанного ника на канале

CHAN_HELP
	%S позволяет Вам регистрировать канал и управлять его
	различными настройками. Также, %S поможет вам обезопасить
	ваш канал от злонамеренных пользователей, желающих его
	"захватить", используя ограничение возможности получения
	опа на канале и др. настройки безопасности. Доступные команды
	показаны ниже, чтобы использовать их, напишите
	%R%S команда. Для более подробной информации
	о конкретной команде, напишите %R%S HELP команда.
	 
	
CHAN_HELP_EXPIRES
	
	Помните, что любой канал, не использующийся в течение %d дней
	(т.е. ни один пользователь, прописанный в списке доступа канала,
	не посетит канал в течении данного времени), будет автоматически
	удален.

CHAN_HELP_REGISTER
	Синтаксис: REGISTER #канал пароль описание
	
	Регистрирует канал в базе данных %S. Чтобы использовать
	эту команду, Вы должны быть оператором канала, который Вы
	пытаетесь зарегистрировать. Указанный при регистрации пароль,
	может использоваться (командой IDENTIFY) для получения доступа
	к каналу, его различным настройкам.
	Последний параметр (описание), который обязательно должен
	присутствовать, является описанием канала и/или, возможно, его тематикой.
	
	Когда Вы зарегистрируете канал, Вы автоматически будете
	внесены в список доступа канала как "владелец" (founder).
	Владелец канала может изменять любые его (канала) параметры.
	Так же, %S автоматически будет давать владельцу статус
	оператора канала, когда он/она заходит на канал.
	Вы можете использовать команду ACCESS (%R%S HELP ACCESS),
	для для получения информации о том, как давать дополнительные
	привилегии на канале для других пользователей.
	
	ПРИМЕЧАНИЕ: Чтобы зарегистрировать канал, Вы ДОЛЖНЫ
	зарегистрировать Ваш ник. Если Вы этого еще не сделали,
	напишите %R%s HELP для получения информации о том,
	как зарегистрировать ник.

CHAN_HELP_IDENTIFY
	Синтаксис: IDENTIFY #канал пароль
	
	Идентифицирует Вас на %S как владельца указанного
	канала. Многие команды для канала будут недоступны,
	пока Вы не идентифицируетесь. Пароль должен быть тем
	паролем, который Вы указывали, когда использовали команду
	REGISTER при регистрации канала.
	
CHAN_HELP_LOGOUT
	Синтаксис: LOGOUT #канал ник
	
	Эта команда деидентифицирует указанный ник,
	который идентифицировался как владелец канала
	с помощью пароля.
	
	Если Вы являетесь владельцем, Вы можете деидентифицировать
	кого угодно, в ином случае - только себя.

CHAN_HELP_DROP
	Синтаксис: DROP #канал
	
	Удаляет указанный канал. Команда может быть использована
	ТОЛЬКО владельцем канала, который должен сперва
	использовать команду IDENTIFY.

CHAN_HELP_SET
	Синтаксис: SET #канал опция параметры
	
	Позволяет владельцу канала менять различные настройки
	канала и какую-либо информацию.
	
	Доступные опции:
	
	    FOUNDER       Установить владельца канала
	    SUCCESSOR     Установить наследника канала
	    PASSWORD      Установить пароль владельца
	    DESC          Установить описание канала
	    URL           Установить URL-адрес канала
	    EMAIL         Установить Email-адрес канала
	    ENTRYMSG      Установить сообщение, которое будет отправлено
	                  всем пользователям, вошедшим в канал
	    BANTYPE       Установить тип банов сервисами на канале
	    MLOCK         Включить или выключить блокировку режимов канала
	    KEEPTOPIC     Хранить топик, если все покинули канал
	    OPNOTICE      Уведомлять об использовании команд OP/DEOP
	    PEACE         Регулировать использование критических команд
	    PRIVATE       Скрыть канал из показываемых по команде LIST
	    RESTRICTED    Ограничить доступ на канал
	    SECURE        Активировать дополнительные возможности %S'а по безопасности.
	    SECUREOPS     Ограничить получение статуса оператора канала
	    SECUREFOUNDER Ограничить получение статуса ПОЛНОГО владельца канала
	    SIGNKICK      Подписывать или нет кик при использовании команды KICK
	    TOPICLOCK     Изменять топик только с помощью команды TOPIC
	    XOP           Выбрать систему привилегий
	
	Напишите %R%S HELP опция для получения более подробной
	информации о конкретной опции.

CHAN_HELP_SET_FOUNDER
	Синтаксис: SET #канал FOUNDER ник
	
	Изменяет владельца канала. Ник нового владельца должен
	быть зарегистрированным.

CHAN_HELP_SET_SUCCESSOR
	Синтаксис: SET #канал SUCCESSOR ник
	
	Установить наследника канала. Если ник владельца
	истечет или будет удален, пока канал существует,
	наследник станет новым владельцем канала.
	Тем не менее, если наследник уже зарегистрировал
	чересчур много каналов (более %d), канал будет удален,
	так же, как если бы наследник не был указан. Ник наследника
	канала должен быть зарегистрированным.

CHAN_HELP_SET_PASSWORD
	Синтаксис: SET #канал PASSWORD пароль
	
	Установить пароль, используемый для идентификации
	к каналу как его владелец.

CHAN_HELP_SET_DESC
	Синтаксис: SET #канал DESC описание
	
	Устанавливает описание канала, которое будет показано
	при использовании команд LIST и INFO.

CHAN_HELP_SET_URL
	Синтаксис: SET #канал URL [адрес]
	
	Установить URL-адрес канала. Этот URL будет показан,
	когда кто-либо запросит информацию о канале с помощью
	команды INFO. Если команду дать без параметров,
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
	(/notice) каждому вошедшему на канал пользователю.
	Если команду дать без параметров, удалит сообщение при входе.

CHAN_HELP_SET_BANTYPE
	Синтаксис: SET #канал BANTYPE тип_бана
	
	Установить тип бана, который будет использован сервисами,
	когда кто-либо запрашивает команду BAN на Вашем канале.
	
	тип_бана должен быть номером между 0 и 3 включительно:
	
	0: бан вида *!идент@хост
	1: бан вида *!*идент@хост
	2: бан вида *!*@хост
	3: бан вида *!*идент@*.домен

CHAN_HELP_SET_KEEPTOPIC
	Синтаксис: SET #канал KEEPTOPIC {ON | OFF}
	
	Включает или выключает опцию хранения топика на
	канале. Когда хранение топика включено, топик канале
	будет хранится с помощью %S даже после того, как
	последний пользователь покинет канал, и будет восстановлен
	как только на канал кто-либо снова зайдет.

CHAN_HELP_SET_TOPICLOCK
	Синтаксис: SET #канал TOPICLOCK {ON | OFF}
	
	Включает или выключает опцию блокировки топика на канале.
	Когда блокирование топика включено, %S разрешит менять
	топик канала только с использованием команды TOPIC.

CHAN_HELP_SET_MLOCK
	Синтаксис: SET #канал MLOCK режимы
	
	Устанавливает и блокирует определенные режимы канала. %S
	позволяет Вам установить режимы канала, которые Вы хотите
	заблокировать, разблокировать, или сделать свободными для
	установки/снятия.
	
	Параметр режимы своим использованием напоминает команду
	/MODE - режимы, начинающиеся с + блокируются в
	состоянии включено (заблокированы от снятия), а режимы,
	начинающиеся с - в состоянии выключено (заблокированы от установки).
	Примечание: однако, вы должны помнить, что повторное использование
	SET MLOCK удалит все предыдущие заблокированные режимы и заменит
	их новыми!
	
	Внимание: Если вы установили и заблокировали ключ канала, как во
	втором примере ниже, вы должны также установить опцию RESTRICTED
	на канал (см. HELP SET RESTRICTED), или любой, кто зайдет на канал
	когда тот будет пуст, сможет увидеть установленный ключ!
	
	Примеры:
	
	    SET #канал MLOCK +nt-iklps
	        Блокирует включенными режимы n и t, и выключенными i, k, l, p, и s.
	        Режим m остается свободным к установке/снятию любым оператором.
	
	    SET #канал MLOCK +knst-ilmp мой-ключ-пароль
	        Блокирует включенным режимы k, n, s, и t, и выключенными
		i, l, m, и p. Также? устанавливает ключ для входа на канал
	        ("мой-ключ-пароль").
	
	    SET #канал MLOCK +
	        Удаляет блокировку режимов. Все режимы каналов свободно могут быть
	        изменены любым оператором канала.

CHAN_HELP_SET_PEACE
	Синтаксис: SET #канал PEACE {ON | OFF}
	
	Включает или выключает опцию спокойствия на канале.
	Когда спокойствие включено, пользователь не может кикать,
	банить или снимать статус на канале с другого пользователя
	с уровнем выше или равным ему посредством команд %S.

CHAN_HELP_SET_PRIVATE
	Синтаксис: SET #канал PRIVATE {ON | OFF}
	
	Включает или выключает опцию приватности на канале.
	Когда приватность включена, канал не будет показан
	в списке при использовании команды %R%S LIST.

CHAN_HELP_SET_RESTRICTED
	Синтаксис: SET #канал RESTRICTED {ON | OFF}
	
	Включает или выключает ограниченный доступ на
	канал. Когда ограниченный доступ включен, пользователи,
	не имеющие права на получение статуса на канале
	(пользователи с отрицательными уровнями доступа, и, если
	безопасность статуса опа включена и пользователя нет
	в списке доступа) будут немедленно забанены и выкинуты с канала.

CHAN_HELP_SET_SECURE
	Синтаксис: SET #канал SECURE {ON | OFF}
	
	Включает или выключает опции безопасности %S для канала.
	Когда SECURE включено, только пользователи с зарегистрированными никами,
	проидентифицировавшиеся к своему нику через пароль и включенные
	в список доступа канала смогут запросить через сервисы статус на канале.
	Если Безопасность выключена, пользователи, прописанные на канале,
	могут запросить статус даже если имеют авторизацию к своему нику
	всего лишь через список масок на %s.

CHAN_HELP_SET_SECUREOPS
	Синтаксис: SET #канал SECUREOPS {ON | OFF}
	
	Включает или выключает безопасность статуса оператора канала.
	Когда безопасность статуса оператора включена, пользователи,
	не прописанные операторами в списке доступа не могут получить
	статус опа на канале.
	
CHAN_HELP_SET_SECUREFOUNDER
	Синтаксис: SET #канал SECUREFOUNDER {ON | OFF}
	
	Включает или выключает безопасность владельца на канале.
	Когда безопасность владельца включена, только настоящий владелец
	может удалить канал, изменить его пароль, сменить владельца канала и его
	наследника, а не любой, кто идентифицируется к каналу на %S через пароль.
	
CHAN_HELP_SET_SIGNKICK
	Синтаксис: SET #канал SIGNKICK {ON | LEVEL | OFF}
	
	Включает или выключает подписанные кики на канале.
	Когда SIGNKICK включен, кики через команду
	%S KICK будут "подписаны" тем, кто использовал эту
	команду с указанием причины кика (если была указана причина).
	
	Если Вы используете LEVEL, тот, у кого уровень выше или равен 
	указаному в установке сможет кикать и его ник не будет указан в причине кика.
	См. %R%S HELP LEVELS для получения более подробной информации.

CHAN_HELP_SET_XOP
	Синтаксис: SET #канал XOP {ON | OFF}
	
	Включает или выключает систему привелегий xOP на канале.
	Когда XOP включен, Вы можете использовать только команды
	SOP/AOP/HOP/VOP чтобы изменять привилегии пользователей на канале,
	в ином случае, вы можете использовать команду ACCESS.
	
	Техническое примечание: когда Вы переключитесь из системы ACCESS
	в систему привилегий в xOP, ваши установки уровней для канала и уровни
	пользователей будут изменены, и Вы не сможете вернуть эти значения,
	переключившись обратно в систему привилегий ACCESS! 
	
	Также, Вы должны удостовериться в том, что Ваши пользователи
	правильно прописаны в xOP-листе после переключения из системы
	ACCESS в xOP, потому что переход может пройти не всегда гладко...
	и вообще, крайне не рекомендуется использовать систему xOP,
	если Вы изменили значения уровней, используя команду LEVELS.
	
	Переключение из системы xOP в систему ACCESS обычно
	проходит без проблем.

CHAN_HELP_SET_OPNOTICE
	Синтаксис: SET #канал OPNOTICE {ON | OFF}
	
	Включает или выключает режим OP-уведомление на канале.
	Когда оп-уведомление включено, %S будет посылать нотис
	на канал, когда кто-либо использует команду OP или DEOP.

CHAN_HELP_AOP
	Синтаксис: AOP #канал ADD ник
	           AOP #канал DEL {nick | номер-записи | список_записей}
	           AOP #канал LIST [маска | список_записей]
	           AOP #канал CLEAR
	
	Позволяет управлять списоком AOP (АвтоОператоров) канала.
	AOP дает возможность пользователю получить опа при входе на канал,
	снимать с себя баны, при необходимости, приглашать себя, получать
	приветственное сообщение от бота, входя на канал, и многое другое.
	
	Команда AOP ADD добавляет указанный ник в список
	AOP'ов.
	
	Команда AOP DEL удаляет указанный ник из списка
	AOP'ов. Если в качестве параметра указан номер/а записи,
	данные записи будут удалены. (См. примеры команды LIST ниже.)
	
	Команда AOP LIST показывает список AOP'ов. Если
	использовать символьную маску, только совпадающие
	с ней записи будут показаны. Если указан список
	номеров записей, только эти записи будут показаны.
	Например:

	   AOP #канал LIST 2-5,7-9
	      Покажет AOP'ов, записи со 2 до 5 и с
	      7 до 9.
	      
	Команда AOP CLEAR удаляет ВСЕ записи из списка AOP'ов.
	
	Команды AOP ADD и AOP DEL могут использовать только
	SOP'ы или выше, а команду AOP CLEAR может использовать только
	владелец канала. Тем не менее, любой пользователь из списка
	AOP'ов может использовать команду AOP LIST.
	
	Эта команда может быть отключена на Вашем канале, и
	тогда вы должны использовать систему ACCESS. См. %R%S HELP ACCESS
	для получения информации об использовании системы ACCESS,
	и %R%S HELP SET XOP чтобы узнать как переключиться между
	системами привелегий.

CHAN_HELP_HOP
	Синтаксис: HOP #канал ADD ник
	           HOP #канал DEL {ник | номер записи | список_записей}
	           HOP #канал LIST [маска | список_записей]
	           HOP #канал CLEAR
	
	Позволяет управлять списком HOP'ов (ПолуОператоров) канала.
	Список HOP'ов включает пользователей, которые могу получить
	статус полуоператора на канале при входе (или по запросу).
	
	Команда HOP ADD добавляет указанный ник в список
	HOP'ов.
	
	Команда HOP DEL удаляет указанный ник из списка
	HOP'ов. Если в качестве параметра указан номер/а записи,
	данные записи будут удалены. (См. примеры команды LIST ниже.)
	
	Команда HOP LIST показывает список HOP'ов. Если
	использовать символьную маску, только совпадающие
	с ней записи будут показаны. Если указан список номеров
	записей, только эти записи будут показаны.
	Например:

	   HOP #канал LIST 2-5,7-9
	      Покажет записи HOP'ов с номерами с 2 по 5 и с
	      7 по 9.
	      
	Команда HOP CLEAR удаляет ВСЕ записи из списка
	HOP'ов.
	
	Команды HOP ADD и HOP DEL могут использовать только
	AOP'ы или выше, а команду HOP CLEAR может использовать
	только владелец канала.
	
	Эта команда может быть отключена на Вашем канале, и
	тогда вы должны использовать систему ACCESS.
	См. %R%S HELP ACCESS для получения информации
	об использовании системы ACCESS, и %R%S HELP SET XOP
	чтобы узнать как переключиться между системами привелегий.

CHAN_HELP_SOP
	Синтаксис: SOP #канал ADD ник
	           SOP #канал DEL {ник | номер записи | список_записей}
	           SOP #канал LIST [маска | список_записей]
	           SOP #канал CLEAR
	
	Позволяет управлять списком SOP (СуперОператоров) канала.
	Список SOP'ов включает пользователей, у которых есть все права
	AOP'ов, и кроме того, они могут использовать автокик и цензуру слов, 
	отправлять и читать сообщения каналп, и многое другое.
	
	Команда SOP ADD добавляет указанный ник в список
	SOP'ов.
	
	Команда SOP DEL удаляет указанный ник из списка
	SOP'ов. Если в качестве параметра указан номер/а записи,
	данные записи будут удалены. (См. примеры команды LIST ниже.)
	
	Команда SOP LIST показывает список SOP'ов. Если
	использовать символьную маску, только совпадающие
	с ней записи будут показаны. Если указан список
	номеров записей, только эти записи будут показаны.
	Например:

	   SOP #канал LIST 2-5,7-9
	      Покажет список SOP'ов с номерами записей c 2 по 5
	      и с 7 по 9.
	      
	Команда SOP CLEAR удалит ВСЕ записи из списка
	SOP'ов.
	
	Команды SOP ADD, SOP DEL и SOP CLEAR могут быть
	использованы только владельцем канала. Тем не менее,
	любой пользователь из списка SOP'ов может использовать
	команду SOP LIST.
	
	Эта команда может быть отключена на Вашем канале, и
	тогда вы должны использовать систему ACCESS. См.
	%R%S HELP ACCESS для получения информации об
	использовании системы ACCESS, и %R%S HELP SET XOP
	чтобы узнать как переключиться между системами привелегий.

CHAN_HELP_VOP
	Синтаксис: VOP #канал ADD ник
	           VOP #канал DEL {ник | номер записи | список_записей}
	           VOP #канал LIST [маска | список_записей]
	           VOP #канал CLEAR
	
	Позволяет управлять списком VOP'ов (АвтоВойсов) канала.  
	Список VOP включают пользователей, которые могут
	получить войс на канале при входе или по запросу.
	
	Команда VOP ADD добавляет указанный ник в список
	VOP'ов.
	
	Команда VOP DEL удаляет указанный ник из списка
	VOP'ов. Если в качестве параметра указан номер/а записи,
	данные записи будут удалены. (См. примеры команды LIST ниже.)
	
	Команда VOP LIST показывает список VOP'ов. Если
	использовать символьную маску, только совпадающие
	с ней записи будут показаны. Если указан список
	номеров записей, только эти записи будут показаны.
	Например:

	   VOP #канал LIST 2-5,7-9
	      Покажет список VOP'ов с номерами записей с 2 по 5
	      и с 7 по 9.
	      
	Команда VOP CLEAR удалит ВСЕ записи из списка
	VOP'ов.
	
	Команды VOP ADD и VOP DEL могут использовать только
	AOP'ы или выше, а команду VOP CLEAR может использовать только
	владелец канала.
	
	Эта команда может быть отключена на Вашем канале, и
	тогда вы должны использовать систему ACCESS. См.
	%R%S HELP ACCESS для получения информации об
	использовании системы ACCESS, и %R%S HELP SET XOP
	чтобы узнать как переключиться между системами привелегий.

CHAN_HELP_ACCESS
	Синтаксис: ACCESS #канал ADD ник уровень
	           ACCESS #канал DEL {ник | номер_записи | список_записей}
	           ACCESS #канал LIST [маска | список_записей]
	           ACCESS #канал CLEAR
	
	Позволяет управлять списком доступа канала. Список доступа
	определяет какие пользователи могут быть операторами канала или
	иметь доступ к командам %S на канале. Различные уровни доступа
	разрешают доступ к различным настройкам канала и определяют
	пользовательские привилегии. %R%S HELP ACCESS LEVELS для
	получения более подробной информации. Любой, не включенный
	в список доступа пользователь, считается пользователем с
	уровнем доступа 0.
	
	Команда ACCESS ADD добавляет указанный ник в
	список доступа с указанным уровнем. Если данный ник уже
	уже есть в списке, его уровень будет изменен на указанный
	в этой команде. Указанный уровень должен быть меньше,
	чем уровень пользователя, использующего это команду, и
	если ник уже есть в списке доступа, текущий
	уровень доступа УКАЗАННОГО для прописи ника должен быть
	МЕНЬШЕ, чем уровнь доступа пользователя, вызвавшего эту команду.
	
	Команда ACCESS DEL удаляет указанный ник из
	списка доступа. Если в качестве параметра указан номер/а записи,
	данные записи будут удалены. (См. примеры команды LIST ниже.)
	
	Команда ACCESS LIST показывает список доступа. Если
	указать символьную маску, только совпадающие с ней
	значения будут показаны. Если указать номера записей,
	то только, эти записи будет показаны.
	Например:

	   ACCESS #канал LIST 2-5,7-9
	      Покажет в списке доступа канала записи со 2 по 5 и с
	      7 по 9.
	      
	Команда ACCESS CLEAR удаляет ВСЕ записи из
	списка доступа.

CHAN_HELP_ACCESS_LEVELS
	Уровни доступа пользователей
	
	По умолчанию, определены нижеследующие значения уровней доступа:
	
	   Founder - Полный доступ к функциям %S, автоматическое получение
	               статуса оператора при входе на канал. Помните,
	               что только один человек может иметь статус
	               владельца (и это нельзя никак изменить командой
	               ACCESS).
	        10 - Доступ к команде AKICK, автооп.
	         5 - Автооп.
	         3 - Автовойс.
	         0 - Никаких особых привилегий, может дать получить статус опа
	               от другого опа
	               (только если безопасность статуса опа отключена).
	        <0 - Автодеоп. Статус опа будет сразу же сниматься сервисами.
	
	Эти уровни могут быть изменены, возможно добавление новых уровней,
	используя команду LEVELS. Напишите %R%S HELP LEVELS для более
	подробной информации.

CHAN_HELP_AKICK
	Синтаксис: AKICK channel ADD {nick | mask} [reason]
	           AKICK channel STICK mask
	           AKICK channel UNSTICK mask
	           AKICK channel DEL {nick | mask | entry-num | list}
	           AKICK channel LIST [mask | entry-num | list]
	           AKICK channel VIEW [mask | entry-num | list]
	           AKICK #канал ENFORCE
	           AKICK #канал CLEAR
	
	Позволяет управлять список автокиков канала. Если
	пользователь, включенный в список автокиков, попытается
	зайти на канал, %S забанит его на канале и сразу же выкинет.
	
	Команда AKICK ADD добавляет ник или маску
	в список автокиков. Если указана причина,
	то она будет использована, когда пользователь будет
	кикнут. Если причина не указана, будет использована
	причина по умолчанию: "User has been banned from the channel".
	
	Команда AKICK STICK создает постоянный бан указанной маски
	на канале. Если кто-то попытается удалить этот бан, %S
	автоматически установит его снова. Вы не можете использовать
	это для зарегистрированных ников.
	
	Команда AKICK UNSTICK отменяет эффект,
	вызываемый командой AKICK STICK, но бан с
	канала Вы должны удалить вручную.
	
	Команда AKICK DEL удаляет указанный ник или маску
	из списка автокиков. Тем не менее, это не удалит баны
	с канала, они должны быть удалены вручную.
	
	Команда AKICK LIST показывает текущий список автокиков,
	или, опционально, показывает автокики по указанной
	маске.
	
	Команда AKICK VIEW это более подробная версия команды
	AKICK LIST.
	
	Команда AKICK ENFORCE заставляет %S немедлено
	перепроверить список акиков и удалить с канала пользователей,
	включенных в него.
	
	Команда AKICK CLEAR удаляет ВСЕ записи из списка автокиков.

CHAN_HELP_LEVELS
	Синтаксис: LEVELS #канал SET тип уровень
	           LEVELS #канал {DIS | DISABLE} тип
	           LEVELS #канал LIST
	           LEVELS #канал RESET
	
	Команда LEVELS позволяет удобно управлять уровнями
	доступа, используемыми для списков доступа. Используя эту
	команду, Вы можете определить уровень доступа, требуемый
	для большинства функций %S'а. (Команды SET FOUNDER и 
	SET PASSWORD, а также, эту команду (LEVELS), может
	использовать ТОЛЬКО владелец канала.)
	
	LEVELS SET устанавливает уровень доступа для функции или группы функций.
	LEVELS DISABLE (или DIS - так короче) отключает функцию или запрещает
	доступ к функции всем, кроме владельца канала.
	LEVELS LIST покажет текущие уровни доступа для каждой функции или
	группы функций. LEVELS RESET сбросит все настройки уровней
	доступа на настройки по умолчанию (см. HELP ACCESS LEVELS).
	
	Чтобы посмотреть список функций, на которые можно определять уровни
	доступа, см. HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Нижеследующие функции доступны. Примечание:
	уровни для Автодеоп и запрета на вход (NOJOIN)
	являются минимальными, в то время как все остальные
	являются максимальными.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Синтаксис: INFO #канал [ALL]
	
	Показывает информацию об указанном канале, включая ник
	его владельца, время регистрации, когда был последний
	раз использован, описание, заблокированные режимы,
	если они имеют место быть. Если указан параметр ALL,
	также будет показано сообщение входа и наследник канала.
	
	По умолчанию, опцию ALL может использовать только
	владелец канала.

CHAN_HELP_LIST
	Синтаксис: LIST маска
	
	Покажет все зарегистрированные каналы по указанной символьной маске.
	(Каналы с включенной опцией PRIVATE не будут показаны.)
	Примечание: при поиске каналов, начинающихся с "#", сам символ
	"#" писать не надо.

CHAN_HELP_OP
	Синтаксис: OP [#канал [ник]]
	
	Дает статус оператора канала указанному нику на указанном канале.
	Если ник не указан, дает вам статус оператора на указанном канале.
	Если канал и ник не указаны, дает вам статус оператора на всех каналах,
	на которых у Вас есть соответствующие права доступа.
	
	По умолчанию, может быть использована AOP'ами или лицами с уровнем доступа
	5 и выше.

CHAN_HELP_DEOP
	Синтаксис: DEOP [#канал [ник]]
	
	Снимает статус оператора канала с указанного ника на указанном канале.
	Если ник не указан, снимает статус оператора с Вас. Если ни канал,
	ни ник не указаны, снимает статус, оператора с Вас на всех каналх,
	на которых у Вас есть соответствующие права доступа.
	
	По умолчанию, может быть использована AOP'ами или лицами с уровнем доступа
	5 и выше.
	
CHAN_HELP_VOICE
	Синтаксис: VOICE [#канал [ник]]
	
	Дает войс указанному нику на указанном канале. Если ник не указан,
	дает войс Вам. Если канал и ник не указаны, дает войс Вам на всех
	каналах, на которых у Вас есть соответствующие права доступа.
	
	По умолчанию, может быть использована AOP'ами или лицами с уровнем
	доступа 5 и выше. Может быть использована VOP'ами (либо лицами с
	уровнем доступа 3 и выше) чтобы дать войс самому себе.

CHAN_HELP_DEVOICE
	Синтаксис: DEVOICE [#канал [ник]]
	
	Снимает войс с указанного ника на указаном канале. Если ник не указан,
	снимет войса с Вас. Если канал и ник не указаны, снимет войс с Вас
	на всех каналах, на которых у Вас есть соответствующие права доступа.
	
	По умолчанию, может быть использована AOP'ами или лицами с уровнем
	доступа 5 и выше. Может быть использована VOP'ами (либо лицами с
	уровнем доступа 3 и выше) чтобы снять войс с самого себя.
	
CHAN_HELP_HALFOP
	Синтаксис: HALFOP [#канал [ник]]
	
	Дает статус полуоператора канала указанному нику на указанном канале.
	Если ник не указан, дает статус полуоператора канала Вам. Если канал
	и ник не указаны, дает статус полуоператора на всех каналах, на которых
	у Вас есть соответствующие права доступа.
	
	По умолчанию, может быть использована AOP'ами или лицами с уровнем
	доступа 5 и выше. Может быть использована HOP'ами (либо лицами с
	уровнем доступа 4 и выше) чтобы дать хопа себе.

CHAN_HELP_DEHALFOP
	Синтаксис: DEHALFOP [#канал [ник]]
	
	Снимает статус полуоператора канала с указанного ника на указанном канале.
	Если ник не указан, снимает статус полуоператора с Вас. Если ни канал,
	ни ник не указаны, снимает статус, полуоператора с Вас на всех каналах,
	на которых у Вас есть соответствующие права доступа.
	
	По умолчанию, может быть использована AOP'ами или лицами с уровнем
	доступа 5 и выше. Может быть использована HOP'ами (либо лицами с
	уровнем доступа 4 и выше) чтобы снять хопа с самого себя.
	
CHAN_HELP_PROTECT
	Синтаксис: PROTECT [#канал [ник]]
	
	Ставит защиту на указанный ник на указанном канале. Если ник не указан,
	ставит защиту на Вас. Если канал и ник не указаны, ставит защиту на Вас
	на всех каналах, на которых у Вас есть соответствующие права доступа.
	
	По умолчанию, может быть использована владельцем или лицами с уровнем
	доступа 10 (SOP) и выше для того, что бы дать статус защиты себе.

CHAN_HELP_DEPROTECT
	Синтаксис: DEPROTECT [#канал [ник]]
	
	Снимает защиту с указанного ника на указанном канале. Если ник не указан,
	снимает защиту с Вас. Если канал и ник не указаны, снимает защиту с Вас
	на всех каналах, на которых у Вас есть соответствующие права доступа.
	
	По умолчанию, может быть использована владельцем или лицами с уровнем
	доступа 10 (SOP) и выше для того, что бы снять статус защиты с себя.

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
	
	По умолчанию, может быть испоьзована только AOP'ами
	или лицами с уровнем доступа 5 и выше.

CHAN_HELP_UNBAN
	Синтаксис: UNBAN канал
	
	Заставит %S удалить с указанного канала все баны, не позволяющие
	Вам зайти на него.  
	
	По-умолчанию, может быть использована только AOP'ами
	или лицами с уровнем доступа 5 и выше.
	
CHAN_HELP_KICK
	Синтаксис: KICK [#канал [ник [причина]]]
	
	Выкидывает указанного пользователя с указанного канала.
	Если ник не указан, выкидывает Вас. Если канал и ник не указаны,
	выкинет Вас со всех каналов, на которых у Вас есть соответствующие
	права доступа.
	
	По-умолчанию, может быть использована только AOP'ами или лицами
	с уровнем доступа 5 и выше.

CHAN_HELP_BAN
	Синтаксис: BAN [#канал [ник [причина]]]
	
	Банит указанный ник на указанном канале по установленному
	типу_бана. Если ник не указан, банит Вас. Если канал и ник
	не указаны, банит Вас на всех каналах, на которых у Вас есть
	соответствующие права доступа.
	
	По-умолчанию, может быть использована только AOP'амии
	или лицами с уровнем доступа 5 и выше.

CHAN_HELP_TOPIC
	Синтаксис: TOPIC #канал [топик]
	
	Заставит %S изменить топик канала на указанный
	Вами. Если топик не указан, то он будет полностью
	удален. Эту команду обычно используют для установки
	топика при включенной опции SET TOPICLOCK. Напишите
	%R%S HELP SET TOPICLOCK для более подробной
	информации.
	
	По-умолчанию, может быть использована только
	владельцем канала. 

CHAN_HELP_CLEAR
	Синтаксис: CLEAR #канал что_именно
	
	Заставляет %S очистить определенные режимы и настройки канала.  что_именно
	может быть одним из:
	
	     MODES    Сбросить все режимы канала (т.е. удалить
	              режимы i,k,l,m,n,p,s,t).
	     BANS     Удалить все баны на канале.
	     EXCEPTS  Удалить все исключения из банов на канале.
	     INVITES  Удалить все исключения на приглашение на канале
	     OPS      Снять статус оператора (режим +o) со
	              всех операторов канала.
	     HOPS     Снять статус полуоператора (mode +h) со всех
	              полуоператоров канала.
	     VOICES   Снять "войсы" (режим +v) со всех, 
	              на ком установлен этот режим.
	     USERS    Удалить (выкинуть) всех пользователей с канала.
	
	По-умолчанию, может быть использована только
	владельцем канала. 
	
CHAN_HELP_GETKEY
	Синтаксис: GETKEY #канал
	
	Возвращает секретный ключ (пароль для входа) канала. Эта команда
	обычно используется ботами и/или скриптами, команда возвращает
	соедующую строку:
	
	KEY <#канал> <секретный ключ>
	
	если секретный ключ показан как "NO KEY"  - ключа на канале нет.
	
CHAN_HELP_SENDPASS
	Синтаксис: SENDPASS #канал
	
	Отправляет пароль канала по email-адресу, указанному
	в настройках владельца канала. Эту команду удобно
	использовать, чтобы вернуть забытый пароль.
	
	В некоторых сетях эту команду могут использовать только
	IRC-операторы.
	
	Эта команда недоступна при включенном шифровании.

CHAN_SERVADMIN_HELP
	
	Администраторы сервисов могут удалить любой канал без идентификации
	к нему в качестве владельца, могут просматривать список доступа,
	список акиков, и установки уровней доступа на канале.
	
CHAN_SERVADMIN_HELP_LOGOUT
	Синтаксис: LOGOUT #канал [ник]
	
	Эта команда деидентифицирует указанный ник
	на указаном канале, снимая с него статус владельца.
	
	Если Вы владелец канала, вы можете деидентифицировать
	любого, в противном случае - только себя.
	
	Если Вы Администратор сервисов, вы можете деидентифицировать
	любого пользователя от любого канале без какой-либо дополнительной
	идентификации к каналу. Также, если Вы не укажете ник,
	эта команда деидентифицирует всех пользователей на указанном канале.

CHAN_SERVADMIN_HELP_DROP
	Синтаксис: DROP #канал
	
	Удаляет указанный канал. Только Администраторы сервисов
	могут удалить канал, не идентифицируясь на ChanServ как владелец.

CHAN_SERVADMIN_HELP_SET
	
	Администраторы сервисов могут установить опцию NOEXPIRE,
	которая отключит истечение по времени для указанного канала.
	Также, Администраторы сервисов могут устанавливать любую опцию
	на любом канале без какой либо дополнительной идентификации.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Синтаксис: SET #канал NOEXPIRE {ON | OFF}
	
	Активирует/деактивирует режим не-истечения для указанного канала.
	Установка этой опции в положение ON предохраняет канал от истечения
	по времени.

CHAN_SERVADMIN_HELP_INFO
	
	Администраторы сервисов могут использовать параметр ALL для любого канала.

CHAN_SERVADMIN_HELP_LIST
	Синтаксис: LIST маска [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
	
	Выводит список всех зарегистрированных каналов по указанной
	маске. Каналы с параметром PRIVATE будут показаны только
	Администраторам сервисов. Каналы с параметром NOEXPIRE будут
	показаны с префиксом ! перед именем канала.
	
	Если указать опции FORBIDDEN, SUSPENDED или NOEXPIRE, то
	будут показаны только каналы с параметрами FORBIDDEN, SUSPENDED
	или NOEXPIRE соответственно. Если указать все эти опции, будут
	показаны все типы каналов. Эти параметры могут использовать
	только Администраторы сервисов.

CHAN_SERVADMIN_HELP_GETPASS
	Синтаксис: GETPASS #канал
	
	Возвращает пароль указанного канала. ПРимечание: при
	использовании этой команды, сообщение, включающее ник того,
	кто использовал эту команду и название канала, пароль на
	который был запрошен, будут записаны в лог и отправлены по
	WALLOPS/GLOBOPS.

CHAN_SERVADMIN_HELP_FORBID
	Синтаксис: FORBID #канал [причина]
	
	Запрещает регистрировать или использовать указанный канал.
	Отменяется путем удаления канала.
	
	В некоторых сетях, причина является обязательным параметром.

CHAN_SERVADMIN_HELP_SUSPEND
	Синтаксис: SUSPEND #канал [причина]
	
	Приостанавливает регистрацию или использование указанного канала.
	Отменяется путем использования команды UNSUSPEND, которая
	восстанавливает все настройки и данные канала.
	
	В некоторых сетях причина является обязательным параметром.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Синтаксис: UNSUSPEND #канал
	
	Восстанавливает приостановленный канал. Все данные и настройки
	канала будут также восстановлены.

CHAN_SERVADMIN_HELP_STATUS
	Синтаксис: STATUS #канал ник
	
	Возвращает текущий уровень доступа указанного ника на
	указанном канале. Ответ может быть таким:
	
	    STATUS #канал ник уровень_доступа
	
	Если произойдет ошибка, ответ будет таким:
	
	    STATUS ERROR сообщение_об_ошибке

###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP_CMD_SEND
	    SEND   Отправить мемо-сообщение нику или каналу
MEMO_HELP_CMD_CANCEL
	    CANCEL Отменить последнее посланное Вами мемо-сообщение
MEMO_HELP_CMD_LIST
	    LIST   Список Ваших мемо-сообщений
MEMO_HELP_CMD_READ
	    READ   Чтение мемо-сообщений
MEMO_HELP_CMD_DEL
	    DEL    Удаление мемо-сообщений
MEMO_HELP_CMD_SET
	    SET    Установка опций для мемо-сообщений
MEMO_HELP_CMD_INFO
	    INFO   Показать информацию о Ваших мемо-сообщениях
MEMO_HELP_CMD_RSEND
	    RSEND  Посылает мемо-сообщение и запрашивает подтверждение о прочтении
MEMO_HELP_CMD_CHECK
	    CHECK  Проверить, прочел ли последнее отправленное вами сообщение указанный ник
MEMO_HELP_CMD_SENDALL
	    SENDALL  Отправляет мемо-сообщение всем зарегистрированным пользователям
MEMO_HELP_CMD_STAFF
	    STAFF  Отправляет мемо-сообщение всем операторам/администраторам

MEMO_HELP_HEADER
	%S это удобный сервис, позволяющий IRC-пользователям
	отправлять короткие сообщения другим пользователям,
	которых нет в данный момент в сети, или на каналы(*).
	Ник отправителя, так же, как и ник получателя или имя канала,
	должны быть зарегистрированы, для возможности отправки/получения
	мемо-сообщения.

	%S включает следующие команды:


MEMO_HELP_ADMIN
	не используется.
MEMO_HELP_OPER
	не используется.

MEMO_HELP_FOOTER
	
	Напишите %R%S HELP команда для получения более подробной
	информации.

	(*) По-умолчанию, любой пользователь, с уровнем доступа на канале 
	    не менее 10, может читать мемо-сообщения, отправленные каналу.
	    Данная установка может быть изменена с помощью команды
	    %s LEVELS.

MEMO_HELP_SEND
	Синтаксис: SEND {ник | #канал} текст_сообщения
	
	Отправляет сообщение для ника или для #канала, содержащее
	текст_сообщения. Если получатель ник, то он будет уведомлен
	что Вы отправили ему собщение. Получатель, будь то ник или канал,
	должены быть зарегистрированы для возможности отправки/получения
	сообщения.
	
MEMO_HELP_CANCEL
	Синтаксис: CANCEL {ник | #канал}
	
	Отменяет последнее сообщение, отправленное вами на ник или канал,
	предохраняя его от прочтения адресатом.

MEMO_HELP_LIST
	Синтаксис: LIST [#канал] [список_записей | NEW]
	
	Показывает текущий список сообщений. С параметром NEW, покажет
	только новые (непрочитанные) сообщения. Непрочитанные сообщения
	будут помаркированы значком "*" слева от номера сообщения. Также,
	Вы можете указать диапазон номеров, например:

	   LIST 2-5,7-9
	      Покажет сообщения со 2 до 5 и с 7 по 9.

MEMO_HELP_READ
	Синтаксис: READ [#канал] {номер | список_записей | LAST | NEW}
	
	Выведет Вам текст указанных сообщений. Если указан параметр LAST, то
	выведет последнее полученное Вами сообщение. Если указан параметр
	NEW, выведет Вам все непрочитанные Вами сообщения. Если указан номер,
	выведет Вам сообщение под номером номер. Вы можете запросить сразу
	несколько сообщений, например так:
	
	   READ 2-5,7-9
	      Покажет сообщения со 2 по 5 и с 7 по 9.

MEMO_HELP_DEL
	Синтаксис: DEL [#канал] {номер | список_записей | LAST | ALL}
	
	Удалит указанное сообщение/ия. Вы можете указать несколько
	номеров сообщений или диапазон номеров сообщений, необязательно
	указывать только один номер (см. пример ниже).
	
	Если указан параметр LAST, будет удалено последнее сообщение.
	Если указан параметр ALL, все Ваши сообщения будут удалены.
	
	Примеры:
	
	   DEL 1
	      Удалит сообщение под номером 1.
	
	   DEL 2-5,7-9
	      Удалит сообщения с 2 по 5 и с 7 по 9 включительно.

MEMO_HELP_SET
	Синтаксис: SET опция параметры
	
	Устанавливает различные параметры для службы сообщений, опция может быть:
	
	    NOTIFY      Выбрать способ уведомления Вас о новых
	                      собщениях (только для ников)
	    LIMIT       Устанавливает ограничение на количество
	                 сообщений, которое Вы можете принимать
	
	Напишите %R%S HELP SET опция для получения более
	подробной информации о конкретной опции.

MEMO_HELP_SET_NOTIFY
	Синтаксис: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Выбрать, каким образом вы будете информироваться о новых сообщениях:
	
	    ON      Вы будете уведомлены о новом сообщении при идентификации,
	                когда снимете с себя режим /AWAY, или как только
			кто-то отправит Вам сообщение.
	    LOGON   Вы будете уведомлены о новом сообщении при идентификации,
	                или когда Вы снимете с себя режим /AWAY.
	    NEW     Вы будете уведомлены о новом сообщении только когда они
	                будут отправлены Вам.
	    OFF     Вы не будете уведомляться о получении новых сообщений.
	
	В принципе, опция ON является комбинацией LOGON и NEW.

MEMO_HELP_SET_LIMIT
	Синтаксис: SET LIMIT [#канал] предел
	
	Ограничивает максимальное количество сообщений, которое
	Вы (или канал) можете принять. Если Вы установите его
	равным 0, никто не сможет отправить Вам сообщение. Однако,
	Вы не можете установить ограничение больше, чем %d.

MEMO_HELP_INFO
	Синтаксис: INFO [#канал]
	
	Показывает информацию о количестве сообщений для Вас,
	сколько из них непрочитано, и сколько всего сообщений
	вы можете принять. С параметром, покажет ту же информацию
	для указанного канала.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Синтаксис: SET LIMIT [пользователь | #канал] {предел | NONE} [HARD]
	
	Устанавливает лимит на максимальное количество сообщений,
	которое пользователь (или канал) смогут принимать. Установка
	лимита равным 0 не дает возможности принимать сообщения вообще,
	установка в NONE разрешает пользователю принимать и сохранять
	столько сообщений, сколько он пожелает. Если Вы не укажете
	пользователя или канал, настройки буду применены для Вас.
	
	Опция HARD не позволит указанному пользователю менять свой лимит.
	Если HARD не добавить - будет противоположный эффект, т.е.,
	пользователь сможет изменять лимит (даже если предыдущий лимит
	был установлен с параметром HARD).
	
	Пасширенное использование команды SET LIMIT доступно только для
	Администраторы сервисов. Обычные пользователи могуть менять лимит
	только для себя или для канала, на котором у них есть соответствующие
	привилегии. Они не могут удалить свой лимит, не могут установить лимит
	выше %d, и не могут использовать опцию HARD.

MEMO_SERVADMIN_HELP_INFO
	Синтаксис: INFO [ник | #канал]
	
	Без параметров, покажет общее количество сообщений,
	которое вы имеете, сколько из них непрочитано, и сколько
	всего сообщений Вы можете принимать.
	
	Если указать канал, покажет ту же информацию
	об указанном канале.
	
	Если указать в качестве параметра ник, покажет ту же
	информацию об указанном нике. Данную опцию могут
	использовать только Администраторы сервисов.

MEMO_HELP_STAFF
	Синтаксис: STAFF текст_сообщения

	Отправляет всем операторам/администраторам сервисов мемо-сообщение,
	содержащее текст_сообщения. Примечание: Если Вы имеете операторов
	в списке операторов и администраторов сервисов, они получат это
	сообщение дважды. Данная особенность применительна и к рутам сервисов.

MEMO_HELP_SENDALL
	Синтаксис: SENDALL текст_сообщения

	Отправляет всем зарегистрированным пользователям текст_сообщения.

MEMO_HELP_RSEND
	Синтаксис: RSEND {ник | #канал} текст_сообщения
	
	Посылает указанному нику или каналу мемо-сообщение.
	Когда указан ник, получатель будет извещен нотисом о том, что он/она
	получил/а новое сообщение. Указанные ник/канал должны быть зарегистрированы.
	Как только мемо-сообщение будет прочитано получателем, отправитель будет
	автоматически извещен о том, что посланное им сообщение было прочитано.

MEMO_HELP_CHECK
	Синтаксис: CHECK ник
	
	Проверяет, было ли прочитано последнее посланное вами
	сообщения для указанного ника. Примечание: опция работает
	ТОЛЬКО для ников, НЕ для каналов.

###########################################################################
#
# OperServ help messages
#
###########################################################################
OPER_HELP_CMD_GLOBAL
	    GLOBAL      Послать сообщение всем пользователям
OPER_HELP_CMD_STATS
	    STATS       Показать статистику сервисов и сети
OPER_HELP_CMD_OPER
	    OPER        Управление списком Операторов сервисов
OPER_HELP_CMD_ADMIN
	    ADMIN       Управление списком Администраторов сервисов
OPER_HELP_CMD_STAFF
	    STAFF       Список всех прописанных на сервисах и их онлайн-статус
OPER_HELP_CMD_MODE
	    MODE        Изменить какой-либо режим на канале
OPER_HELP_CMD_KICK
	    KICK        Кикнуть пользователя с канала
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  Очистка всех режимов на канале
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  Прибить всех пользователей с определенного хоста
OPER_HELP_CMD_AKILL
	    AKILL       Управление списком AKILL'ов
OPER_HELP_CMD_SGLINE
	    SGLINE      Управление списком SGLINE'ов
OPER_HELP_CMD_SQLINE
	    SQLINE      Управление списком SQLINE'ов
OPER_HELP_CMD_SZLINE
	    SZLINE      Управление списком SZLINE'ов
OPER_HELP_CMD_CHANLIST
	    CHANLIST    Показать все текущие каналы
OPER_HELP_CMD_USERLIST
	    USERLIST    Показать всех текущих пользователей
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   Установить сообщения при входе в сеть
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  Установить случайные сообщения при входе в сеть
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    Установить сообщения для IRC-операторов
OPER_HELP_CMD_SESSION
	    SESSION     Посмотреть список текущих соединений
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   Управление списком исключений из лимита сессий
OPER_HELP_CMD_NOOP
	    NOOP        Временно удалить все O:лайны на указанном
	                сервере
OPER_HELP_CMD_JUPE
	    JUPE        "Джупнуть" сервер
OPER_HELP_CMD_IGNORE
	    IGNORE      Управление игнор-списком сервисов
OPER_HELP_CMD_SET
	    SET         Установить различные опции сервисов
OPER_HELP_CMD_RELOAD
	    RELOAD      Перечитать конфигурационный файл сервисов
OPER_HELP_CMD_UPDATE
	    UPDATE      Заставить сервисы немедленно сохранить
	                все базы данных на диск
OPER_HELP_CMD_RESTART
	    RESTART     Сохранить базы данных и перезапустить сервисы
OPER_HELP_CMD_QUIT
	    QUIT        Завершить работу сервисов без сохранения данных
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    Завершить работу сервисов с сохранением данных
OPER_HELP_CMD_DEFCON
	    DEFCON      Управление системой DefCon (защитных контрмер)
OPER_HELP_CMD_CHANKILL
	    CHANKILL    Прибить всех пользователей на определенном канале
OPER_HELP_CMD_OLINE
	    OLINE       Установить флаги IRC-Операторов определенному пользователю
OPER_HELP_CMD_UMODE
	    UMODE       Измененить режимы какого-либо пользователя
OPER_HELP_CMD_SVSNICK
	    SVSNICK     Изменить ник пользователя
OPER_HELP_CMD_MODLOAD
	    MODLOAD     Загрузить модуль
OPER_HELP_CMD_MODUNLOAD
	    MODUNLOAD   Выгрузить модуль
OPER_HELP_CMD_MODINFO
	    MODINFO     Информация о загруженном модуле
OPER_HELP_CMD_MODLIST
	    MODLIST     Список загруженных модулей

OPER_HELP
	Команды %S:

OPER_HELP_LOGGED
	Примечание: Все команды %S'у записываются в лог!

OPER_HELP_GLOBAL
	Синтаксис: GLOBAL сообщение
	
	Позволяет IRC-Операторам отправить сообщение всемпользователям
	IRC сети. Данное сообщение будет отправлено от ника %s.

OPER_HELP_STATS
	Синтаксис: STATS [AKILL | ALL | RESET | MEMORY | UPLINK]
	
	Без параметров, покажет текущее количество пользователей и
	IRC-Операторов в онлайне (за исключением Сервисов),
	максимальное число пользователей онлайн запомненное сервисами
	с момента старта, и длительность бесперебойной работы сервисов.
	
	С опцией AKILL, покажет текущий размер списка AKILL'ов
	и время истечения AKILL'ов по-умолчанию.

	Опция RESET сбрасывает счетчик маскимально запомненного
	сервисами количества пользователей в онлайне.

	Опция MEMORY покажет информацию об использовании сервисами
	памяти. Использование данной опции затормаживает работу сервисов
	на определенный промежуток времени, поэтому, НЕ злоупотребляйте ею!

	Опция UPLINK покажет информацию о сервере, который сервисы
	используют для связи с сетью.
	
	Опция ALL покажет статистику пользователей и аптайма, плюс всю
	информацию из опций MEMORY и UPLINK.
	
	UPTIME это синоним команды STATS.

OPER_HELP_OPER
	Синтаксис: OPER ADD ник
	           OPER DEL {ник | номер записи | список_записей}
	           OPER LIST [маска | список_записей]
	           OPER CLEAR
	
	Позволяет Администраторам сервисов управлять списком
	Операторов сервисов. Пользователь, чей ник присутствует
	в списке Операторов сервисов, идентифицировавшись на
	%s, получает доступ к командам Операторов сервисов.
	
	Команда OPER ADD добавляет указанный ник в список
	Операторов сервисов.
	
	Команда OPER DEL удаляет указанный ник из списка
	Операторов сервисов. Если в качестве параметра указаны
	номера записей, данные записи будут удалены.
	(См. пример ипользования команды LIST ниже.)
	
	Команда OPER LIST показывает список Операторов сервисов.
	Если указана символьная маска, только соответствующие ей
	записи будут показаны. Если указаны номера записей,
	только эти записи будут показаны; например:

	   OPER LIST 2-5,7-9
	      Покажет Операторов сервисов с номерами записей со 2 по
	      5 и с 7 по 9.
	      
	Команда OPER CLEAR удаляет ВСЕ записи из списка
	Операторов сервисов.
	
	Любой IRC-Оператор может использовать команду OPER LIST.
	Остальные команды могут использовать только Руты сервисов.

OPER_HELP_ADMIN
	Синтаксис: ADMIN ADD ник
	           ADMIN DEL {ник | номер записи | список_записей}
	           ADMIN LIST [маска | список_записей]
	           ADMIN CLEAR
	
	Позволяет Руту сервисов управлять списком Администраторов сервисов.
	Пользователь, чей ник находится с списке Администраторов сервисов,
	идентифицировавшись на %s, получает доступ к командам Администраторов
	сервисов.
	
	Команда ADMIN ADD добавляет указанный ник в список
	администраторов сервисов.
	
	Команда ADMIN DEL удаляет указанный ник из списка
	администраторов сервисов. Если в качестве параметра указаны
	номера записей, данные записи будут удалены.
	(См. пример ипользования команды LIST ниже.)
	
	Команда ADMIN LIST показывает список администраторов сервисов.  
	Если указать символьную маску, только подпадающие под нее значения
	будут показаны. Если указать номера записей, то будут показаны только
	они. Например:

	   ADMIN LIST 2-5,7-9
	      Покажет администраторов сервисов с номерами записей от 2 до
	      5 и от 7 до 9 включительно.
	      
	Команда ADMIN CLEAR удалит ВСЕ записи из списка
	администраторов сервисов.
	
	Любой IRC-Оператор может использовать команду ADMIN LIST.
	Остальные команды может использоваеть только Рут сервисов.

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
	Синтаксис: MODE #канал режимы
	
	Позволяет Операторам сервисов изменять режимы на любом
	канале. Параметры данной команды такие же, что и при
	использовании команды /MODE.

OPER_HELP_UMODE
	Синтаксис: UMODE пользователь режимы
	
	Позволяет Администраторам сервисов устанавливать пользовательские
	режимы для любого пользователя. Параметры данной команды такие же,
	что и при использовании команды /MODE.
	
	Для использования этой команды необходимы права Супер-Администратора.

OPER_HELP_OLINE
	Синтаксис: OLINE пользователь флаги
	
	Позволяет Администраторам сервисов установить флаги
	IRC-Операторов любому пользователю. Перед флагами должен
	присутствовать префикс "+" или "-". Чтобы удалить все флаги,
	используйте просто "-" вместо какого-либо флага.

	Для использования этой команды необходимы права Супер-Администратора.

OPER_HELP_CLEARMODES
	Синтаксис: CLEARMODES #канал [ALL]
	
	Очищает все двоичные режимы (i,k,l,m,n,p,s,t) и баны на указанном
	канале. Если указан параметр ALL, также снимет со всех статус
	оператора/войса (режимы +o и +v) на канале.

OPER_HELP_KICK
	Синтаксис: KICK #канал пользователь причина
	
	Позволяет IRC-Операторам выкинуть пользователя с любого канала.
	Параметры данной команды такие же, что и при использовании команды /KICK.
	Сообщение кика будет включать имя IRC-Оператора, использовавшего команду
	и указанную им причину, например:
	
	*** SpamMan was kicked by %S (Alcan (Flood))

OPER_HELP_SVSNICK
	Синтаксис: SVSNICK ник новый_ник
	
	Принудительно изменить указанный ник на новый_ник.

OPER_HELP_AKILL
	Синтаксис: AKILL ADD [+время_истечения] маска причина
	           AKILL DEL {маска | номер_записи | список_записей}
	           AKILL LIST [маска | список_записей]
	           AKILL VIEW [маска | список_записей]
	           AKILL CLEAR
	
	Позволяет Операторам сервисов управлять списком AKILL'ов. Если
	пользователь, подпадающий под маску AKILL'а попытается
	присоединиться к серверу сети, сервисы используют на него KILL, и,
	Если Ваш IRC-сервер это поддерживает, установит на всех серверах
	IRC-сети K-line на маску данного пользователя.
	
	AKILL ADD добавляет маску вида идент@хост/ip в список AKILL'ов
	с указанной причиной (которая обязательно должна быть указана).
	время истечения - это целое число, которое может быть одним из:
	d (дней), h (часов), или m (минут). Сочетания (например
	1h30m) не допускаются. Если единица измерения не указана,
	по-умолчанию она будет установлена в "дни" (таким образом, +30
	будет означать 30 дней). Чтобы добавить вечный AKILL, используйте
	время истечения равным +0. Если маска пользователя была добавлена
	начиная с +, время истечения будет задано по-умолчанию. Текущее
	время истечения AKILL'а по-умолчанию, можно узнать командой
	STATS AKILL.
	
	Команда AKILL DEL удаляет указанную маску из списка
	AKILL'ов. Если в качестве параметра указаны
	номера записей, данные записи будут удалены.
	(См. пример ипользования команды LIST ниже.)
	
	Команда AKILL LIST показывает текущий список AKILL'ов.  
	Если указать символьную маску, будут показаны только
	совпадающие с ней значения. Если указать номера записей,
	только, эти записи будут показаны.
	Например:

	   AKILL LIST 2-5,7-9
	      Список AKILL'ов с номерами записей от 2 до 5 и от 7 
	      до 9 включительно.
	      
	AKILL VIEW более подробная версия AKILL LIST, она
	покажет кто добавил AKILL, время установки AKILL'а, когда
	он истекает, ну и, конечно же, маску вида идент@хост/ip с
	причиной AKILL'а.
	
	AKILL CLEAR удаляет ВСЕ записи из списка AKILL'ов.
	
OPER_HELP_SGLINE
	Синтаксис: SGLINE ADD [+время_истечения] маска:причина
	           SGLINE DEL {маска | номер записи | список_записей}
	           SGLINE LIST [маска | список_записей]
	           SGLINE VIEW [маска | список_записей]
	           SGLINE CLEAR
	
	Позволяет Операторам сервисов управлять списком SGLINE'ов. Если
	пользователь с реальным именем, подпадающим под маску SGLINE'а
	попытается присоединиться к серверу сети, сервисы не разрешат
	ему установить соединение.
	
	SGLINE ADD добавляет маску реального имени в список SGLINE'ов
	с указанной причиной (которая обязательно должна быть указана).
	время_истечения - это целое число, которое может быть одним из:
	d (дней), h (часов), или m (минут). Комбинации значений
	(например 1h30m) не допускаются. Если единица измерения не указана,
	по-умолчанию, она будет установлена в "дни" (таким образом, +30
	будет означать 30 дней). Чтобы добавить вечный SGLINE, используйте
	время истечения равным +0. Если маска имени пользователя была
	добавлена, начиная с +, время истечения будет задано по-умолчанию.
	Текущее время истечения SGLINE'а по-умолчанию, можно узнать командой
	STATS AKILL.
	Примечание: так как маски реальных имен могут содержать пробелы,
	разделителем между реальным именем и причиной будет двоеточие.
	
	Команда SGLINE DEL удаляет указанную маску из списка
	SGLINE'ов. Если в качестве параметра указаны
	номера записей, данные записи будут удалены.
	(См. пример ипользования команды LIST ниже.)
	
	Команда SGLINE LIST показывает текущий список SGLINE'ов.  
	Если указать символьную маску, будут показаны только
	совпадающие с ней значения. Если указать номера записей,
	только, эти записи будут показаны. Например:

	   SGLINE LIST 2-5,7-9
	      Список SGLINE'ов с номерами записей от 2 до 5 и от 7 
	      до 9 включительно.
	      
	SGLINE VIEW более подробная версия  SGLINE LIST, она
	покажет кто добавил SGLINE, время установки SGLINE'а, когда
	он истекает, ну и, конечно же, маску реального имени и причину SGLINE'а.
	
	SGLINE CLEAR удаляет ВСЕ записи из списка SGLINE'ов.

OPER_HELP_SQLINE
	Синтаксис: SQLINE ADD [+время_истечения] маска причина
	           SQLINE DEL {маска | номер_записи | список_записей}
	           SQLINE LIST [маска | список_записей]
	           SQLINE VIEW [маска | список_записей]
	           SQLINE CLEAR
	
	Позволяет Операторам сервисов управлять списком SQLINE'ов. Если
	пользователь, ник которого подпадает под маску SQLINE'а попытается
	присоединиться к серверу сети, сервисы не разрешат ему установить
	соединение.
	
	Если первый символ маски указан как #, сервисы не разрешат
	использовать подпадающие под маску каналы (только если ваше
	IRCD это поддерживает).
	
	SQLINE ADD добавляет указанную маску в список SQLINE'ов
	с указанной причиной (которая обязательно должна быть указана).
	время_истечения - это целое число, которое может быть одним из:
	d (дней), h (часов), или m (минут). Комбинации значений
	(например 1h30m) не допускаются. Если единица измерения не указана,
	по-умолчанию, она будет установлена в "дни" (таким образом, +30
	будет означать 30 дней). Чтобы добавить вечный SQLINE, используйте
	время истечения равным +0. Если маска ника/канала была добавлена
	начиная с +, время истечения будет задано по-умолчанию. Текущее
	время истечения SQLINE'а по-умолчанию можно узнать командой
	STATS AKILL.
	
	Команда SQLINE DEL удаляет указанную маску из списка
	SQLINE'ов. Если в качестве параметра указаны
	номера записей, данные записи будут удалены.
	(См. пример ипользования команды LIST ниже.)
	
	Команда SQLINE LIST показывает текущий список SQLINE'ов.  
	Если указать символьную маску, будут показаны только
	совпадающие с ней значения. Если указать номера записей,
	только, эти записи будут показаны. Например:

	   SQLINE LIST 2-5,7-9
	      Список SQLINE'ов с номерами записей от 2 до 5 и от 7 
	      пд 9 включительно.
	      
	SQLINE VIEW более подробная версия SQLINE LIST, она
	покажет кто установил SQLINE, время установки SQLINE'а, когда
	он истекает, ну и, конечно же, маску ника/канала и причину SQLINE'а.
	
	SQLINE CLEAR удаляет ВСЕ записи из списка SQLINE'ов.

OPER_HELP_SZLINE
	Синтаксис: SZLINE ADD [+время_истечения] маска причина
	           SZLINE DEL {маска | номер записи | список_записей}
	           SZLINE LIST [маска | список_записей]
	           SZLINE VIEW [маска | список_записей]
	           SZLINE CLEAR
	
	Позволяет Операторам сервисов управлять списком SZLINE'ов.
	Если пользователь, с IP адресом, подпадающим под маску SZLINE'а
	попытается присоединиться к серверу, сервисы не разрешат ему
	установить соединение (и неважно, есть ли для этого IP запись
	PTR RR или нет).
	
	SZLINE ADD добавляет указанную IP-маску в список SZLINE'ов
	с указанной причиной (которая обязательно должна быть указана).
	время истечения - это целое число, которое может быть одним из:
	d (дней), h (часов), или m (минут). Комбинации значений
	(например 1h30m) не допускаются. Если единица измерения не указана,
	по-умолчанию, она будет установлена в "дни" (таким образом, +30
	будет означать 30 дней). Чтобы добавить вечный SZLINE, используйте
	время истечения равным +0. Если IP-маска пользователя была добавлена,
	начиная с +, время истечения будет задано по-умолчанию. Текущее время
	истечения SZLINE'а по-умолчанию, можно узнать командой STATS AKILL.
	
	Команда SZLINE DEL удаляет указанную IP-маску из списка
	SZLINE'ов. Если в качестве параметра указаны номера
	записей, данные записи будут удалены. (См. пример
	ипользования команды LIST ниже.)
	
	Команда SZLINE LIST показывает текущий список SZLINE'ов.  
	Если указать символьную маску, будут показаны только
	совпадающие с ней значения. Если указать номера записей,
	только, эти записи будут показаны. Например:

	   SZLINE LIST 2-5,7-9
	      Список SZLINE'ов с номерами записей от 2 до 5 и от 7 
	      до 9 включительно.
	      
	SZLINE VIEW более подробная версия SZLINE LIST, она
	покажет кто добавил SZLINE, время установки SZLINE'а, когда
	он истекает, ну и, конечно же, IP-маску и причину SZLINE'а.
	
	SZLINE CLEAR удаляет ВСЕ записи из списка SZLINE'ов.

OPER_HELP_SET
	Синтаксис: SET опция параметр

	Устанавливает различные глобальные опции сервисов. Опции
	могут быть следующими:
	    READONLY   Установить режим только-чтение или чтение-запись
	    LOGCHAN    Вести лог сервисов на сервисный канал
	    DEBUG      Активировать/деактивировать режим отладки
	    NOEXPIRE   Активировать/деактивировать режим не-истечения
	    SUPERADMIN Активировать/деактивировать режим Супер-Администратора
	    SQL        Активировать/деактивировать SQL-режим
	    IGNORE     Активировать/деактивировать режим игнора
	    LIST       Список опций

OPER_HELP_SET_READONLY
	Синтаксис: SET READONLY {ON | OFF}
	
	Включает или выключает режим только-чтения. В режиме только-чтения,
	обычные пользователи не могу изменять никакие данные, включая списки
	доступа для ников и каналов, их настройи и т. д. IRC-Операторы
	с достаточными привилегиями на сервисах могут модифицировать список
	AKILL'ов и удалять или запрещать ники и каналы, но все эти изменения
	не будут сохранены, пока режим только-чтения не будет деактивирован
	прежде, чем сервисы завершат работу или перезапустятся.
	
	Эта опция является эквивалентом параметра командной строки
	-readonly.

OPER_HELP_SET_LOGCHAN
	Синтаксис: SET LOGCHAN {ON | OFF}

	С включенным LOHCHAN, сервисы будут не только записывать
	все сообщения в лог, но и отправлять их на специальный канал.
	Этот канал должен быть определен в конфигурационном файле
	сервисов, чтобы данная опция могла быть использована.
	
	This option is equivalent to the command-line option -logchan.

	Примечание: Из соображений безопасности, рекомендуется
	обратить особое внимание на ограничение доступа на этот канал.

OPER_HELP_SET_DEBUG
	Синтаксис: SET DEBUG {ON | OFF | номер}
	
	Включает/выключает режим отладки. В режиме отладки, данные,
	посланные или принятые сервисами, количество которых зависит
	от уровня (номера) отладки, будут записаны в лог. Если указать
	номер, режим отладки активируется с уровнем отладки, указанным
	как номер.

	Эта опция является эквивалентом параметра командной строки
	-debug.

OPER_HELP_SET_NOEXPIRE
	Синтаксис: SET NOEXPIRE {ON | OFF}

	Включает/выключает режим глобального не-истечения. В этом режиме,
	ники, каналы, AKILL'ы и исключения никогда не истекут, пока опция
	не будет отключена.

	Эта опция является эквивалентом параметра командной строки
	-noexpire.

OPER_HELP_SET_SUPERADMIN
	Синтаксис: SET SUPERADMIN {ON | OFF}

	Эта опция позволяет Вам получить супер привилегии,
	например такие, как "владелец" на всех каналах и многое
	другое...

	Эта опция не постоянная, и должна использоваться только при
	крайней необходимости, установите ее обратно в OFF как только
	перестанете нуждаться в ней.

OPER_HELP_SET_SQL
	Синтаксис: SET SQL {ON | OFF}

	Данная установка позволяет включать или выключать
	использование сервисами Anope SQL-режима.

OPER_HELP_SET_IGNORE
	Синтаксис: SET IGNORE {ON | OFF}

	Устанавливает, будет использоваться режим ИГНОРА сервисами или нет.

OPER_HELP_SET_LIST
	Синтаксис: SET LIST

	Показывает различные настройки %S

OPER_HELP_NOOP
	Синтаксис: NOOP SET сервер
	           NOOP REVOKE сервер

	NOOP SET удаляет все O:lines на указанном сервере
	и прибивает всех IRC-Операторов, находящихся на нем,
	чтобы не дать им возможность rehash'нуть сервер (так
	как это отменит эффект данной команды).

	NOOP REVOKE делает все ранее удаленные O:lines доступными
	для использования на сервере.

	Примечание: наличие сервера в сети не проверяется сервисами.

OPER_HELP_JUPE
	Синтаксис: JUPE сервер [причина]
	
	Приказывает сервисам джупнуть серер -- это создаст
	фальшивый "сервер", присоединенный к сервисам, который
	не даст реальному серверу с этим именем присоединиться к сети.
	Джуп может быть удален, использованием стандартного SQUIT на
	фальшивый сервер. Если указана причина, она будет показана в
	информации сервера, в ином случае, эта информационная строка
	будет содержать текст "Juped by <ник>", то есть покажет ник
	человека, который джупнул данный сервер.

OPER_HELP_RAW
	Синтаксис: RAW текст
	
	Отправляет текстовую строку напрямую на IRC сервер, к которому
	прилинкованы сервисы. Эта команда имеет очень ограниченное
	использование, она может вызвать фатальную ошибку в сети при
	неверном испоьзовании. НЕ ИСПОЛЬЗУЙТЕ ЭТУ КОМАНДУ БЕЗ
	АБСОЛЮТНОЙ УВЕРЕННОСТИ В ТОМ, ЧТО ВЫ ДЕЛАЕТЕ!

OPER_HELP_UPDATE
	Синтаксис: UPDATE
	
	Заставляет сервисы немедленно обновить все файлы баз данных
	на диске.
	
OPER_HELP_RELOAD
	Синтаксис: RELOAD
	
	Заставляет сервисы перечитать конфигурационный файл. Примечание:
	некоторые директивы будут требовать перезапуска сервисов для
	вступления в силу (например: изменение ников сервисов, активация
	лимитирования сессий, и т.д.)

OPER_HELP_QUIT
	Синтаксис: QUIT
	
	Заставляет сервисы немедленно завершить свою работу. Базы данных
	не будут сохранены. Эта команда должна использоваться только в
	тех случаях, когда наличиствует повреждения базы данных в памяти,
	и надо избежать сохранения поврежденной базы данных. Для нормального
	завершения работы сервисов используйте команду SHUTDOWN.

OPER_HELP_SHUTDOWN
	Синтаксис: SHUTDOWN
	
	Заставляет сервисы сохранить все базы данных и завершить работу.

OPER_HELP_RESTART
	Синтаксис: RESTART
	
	Заставляет сервисы сохранить все базы данных и перезапуститься
	(т.е., завершить работу немедленно снова запустить сервисы на исполнение).

OPER_HELP_CHANLIST
	Синтаксис: CHANLIST [{символная маска | ник} [SECRET]]
	
	Показывает все созданные на данный момент каналы в IRC-сети,
	вне зависимости от того, зарегистрированы они или нет.
	
	Если указана символьная маска, покажет только те каналы,
	которые совпадают с ней. Если указан ник, покажет только те
	каналы, на которых находится этот ник. Если указан параметр SECRET,
	покажет только каналы с указанной символьной_маской на которых
	стоит режим +s или +p.

OPER_HELP_USERLIST
	Синтаксис: USERLIST [{символьная маска | #канал} [INVISIBLE]]
	
	Показывает всех пользователей, находящихся в данный момент в IRC-сети,
	вне зависимости от того, зарегистрирован у них ник или нет.
	
	Если указана символьная маска, покажет только совпадающих с ней пользователей
	(маска должна быть в формате ник!идент@хост). Если указан #канал, покажет
	только тех пользователей, которые находяться на этом канале. Если указан пареметр
	INVISIBLE, будут показаны только те пользователи, на которых стоит флаг +i.

OPER_HELP_MODLOAD
	Синтаксис: MODLOAD ИмяФайла

	Эта команда загрузит указанный модуль из директории
	с модулями..

OPER_HELP_MODUNLOAD
	Синтаксис: MODUNLOAD ИмяФайла

	Эта команда выгрузит указанный модуль из директории
	с модулями.

OPER_HELP_MODINFO
	Синтаксис: MODINFO ИмяФайла

	Эта команда покажет информацию об указанном загруженом модуле

OPER_HELP_MODLIST
	Синтаксис: MODLIST

	Список всех загруженных модулей

###########################################################################
#
# BotServ help messages
#
###########################################################################
BOT_HELP_CMD_BOTLIST
	BOTLIST            Список доступных для установки ботов
BOT_HELP_CMD_ASSIGN
	ASSIGN             Установить бота на канал
BOT_HELP_CMD_SET
	SET                Конфигурировать настройки бота
BOT_HELP_CMD_KICK
	KICK               Установить, за что бот будет кикать и банить
BOT_HELP_CMD_BADWORDS
	BADWORDS           Управление списком плохих слов
BOT_HELP_CMD_ACT
	ACT                Заставляет бота выполнить команду, эквивалентную команде "/me" в mIRC
BOT_HELP_CMD_INFO
	INFO               Позволяет вам посмотреть информацию о боте или канале
BOT_HELP_CMD_SAY
	SAY                Заставляет бота сказать определенный текст на канал
BOT_HELP_CMD_UNASSIGN
	UNASSIGN           Удаляет бота с канала
BOT_HELP_CMD_BOT
	BOT                Управление ботами

BOT_HELP
	%S позволяет Вам установить бота на свой канал.
	Даный сервис сделан для тех пользователей, которые
	не могут иначе установить или настроить бота, или же,
	использование ботов запрещено в IRC сети. Доступные
	команды перечислены ниже, чтобы использовать их, напишите
	%R%S команда. Для получения более подробной
	информации по конкретной команде, напишите
	%R %S HELP команда.
	
BOT_HELP_FOOTER
	Бот присоединится к каналу после того, как на нем окажется
	не менее %d пользователей.
	
BOT_HELP_BOTLIST
	Синтаксис: BOTLIST
	
	Покажет всех доступных для установки ботов.
	
BOT_HELP_ASSIGN
	Синтаксис: ASSIGN #канал ник
	
	Установит бота под указанным ником на указанный канал. Теперь
	Вы можете настраивать опции бота так, как Вам необходимо.  
	
BOT_HELP_UNASSIGN
	Синтаксис: UNASSIGN #канал
	
	Убирает бота с указанного канала. При использовании этой команды
	бот больше не будет заходить на канал. Тем не менее, конфигурация
	бота для данного канала будет сохранена, поэтому, если Вы позднее
	снова установите этого бота на канал, все его настройки будут
	восстановлены.
	
BOT_HELP_INFO
	Синтаксис: INFO {#канал | ник}
	
	ПОзволяет Вам увидеть информацию через %S о канале или о боте.
	Если указать #канал, то Вы получите информацию о канале
	и настройках киков. Если указать ник, Вы получите информацию о
	боте, когда бот был создан и число каналов, на котором он находится.
	
BOT_HELP_SET
	Синтаксис: SET (#канал| бот) опция параметры
	
	Конфигурация опций бота. Настраиваемые опции:
	
	    DONTKICKOPS      Не кикать операторов
	    DONTKICKVOICES   Не кикать войсов
	    GREET            Включить сообщения приветствий
	    FANTASY          Включить режим Фантазии
	    SYMBIOSIS        Позволить боту кикать/банить/менять режимы "от себя"
	
	Напишите %R%S HELP SET опция для более подробной информации
	о конкретной опции.
	
	Примечание: доступ к этой команде может быть изменен через
	level SET.
	
BOT_HELP_SET_DONTKICKOPS
	Синтаксис: SET #канал DONTKICKOPS {ON|OFF}
	
	Включает или выключает защиту операторов канала на указанном канале.
	Если включено, операторы не будут кикнуты ботом даже если их уровень
	доступа не соответствует уровню NOKICK.
	
BOT_HELP_SET_DONTKICKVOICES
	Синтаксис: SET #канал DONTKICKVOICES {ON|OFF}
	
	Включает или выключает защиту войсов на указанном канале.
	Если включено, войсы не будут кикнуты ботом даже если их
	уровень доступа не соответствует уровню NOKICK.
	
BOT_HELP_SET_FANTASY
	Синтаксис: SET #канал FANTASY {ON|OFF}
	
	Включает или выключает режим Фантазии на канале.
	Если включено, пользователи могут использовать
	команды !op, !deop, !voice, !devoice, !kick, !kb,
	!unban, !seen на канале (можно использовать,
	указывая ник (необязательно), или причину
	(для команд !kick, !kb)).
	
	Примечание: чтобы использовать команды Фантазии,
	пользователи ДОЛЖНЫ иметь соответствующий уровень доступа
	к командам режима Фантазии и другие привилегии, требуемые
	для определенных команд (например, чтобы использовать команду
	!op, пользователь должен иметь права на использование команд 
	OP/DEOP).

BOT_HELP_SET_GREET
	Синтаксис: SET #канал GREET {ON|OFF}
	
	Включает или выключает режим приветствий на канале.
	Если включено, бот будет показывать приветственные
	сообщения для пользователей, заходящих на канал,
	и имеющих достаточный уровень доступа.

BOT_HELP_SET_SYMBIOSIS
	Синтаксис: SET #канал SYMBIOSIS {ON|OFF}
	
	Включает или выключает режим симбиоза на канале.
	Если включено, бот будет выполнять большинство действий
	на канале посредством %s, например: изменение режимов
	канала, кики, и др.

BOT_HELP_KICK
	Синтаксис: KICK #канал опция параметры
	
	Установки, за что бот будет кикать и банить. Доступные опции:
	
	    BOLDS         Будет ли бот кикать за выделение жирным шрифтом
	    BADWORDS      Будет ли бот кикать за плохие слова
	    CAPS          Будет ли бот кикать за использование CapsLOCK
	    COLORS        Будет ли бот кикать за выделение цветами
	    FLOOD         Будет ли бот кикать за флуд
	    REPEAT        Будет ли бот кикать за повторы
	    REVERSES      Будет ли бот кикать за реверсы
	    UNDERLINES    Будет ли бот кикать за выделение подчеркиванием
	
	Напишите %R%S HELP KICK опция для получения более
	подробной информации о конкретной опции.
	
	Примечание: доступ к этой команде может быть изменен
	через level SET.

BOT_HELP_KICK_BOLDS
	Синтаксис: KICK #канал BOLDS {ON|OFF} [ttb]

	Если опция включена, бот будет кикать за использование
	жирного шрифта на канале.

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_COLORS
	Синтаксис: KICK #канал COLORS {ON|OFF} [ttb]

	Если опция включена, бот будет кикать за использование
	цветного текста на канале.

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_REVERSES
	Синтаксис: KICK #канал REVERSES {ON|OFF} [ttb]

	Если опция включена, бот будет кикать за использование
	реверсивного текста на канале.

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_UNDERLINES
	Синтаксис: KICK #канал UNDERLINES {ON|OFF} [ttb]

	Если опция включена, бот будет кикать за использование
	подчеркнутого текста на канале.

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_CAPS
	Синтаксис: KICK #канал CAPS {ON|OFF} [ttb [минимум [проценты]]]

	Будет ли бот кикать за использование CapsLOCK.
	Если включено - бот будет кикать тех, кто любит
	разговаривать большими буквами.

	Бот кикнет пользователя, если он использует минимум больших букв,
	количество которых составит проценты%% от общего количества букв
	тектовой линии (Если параметры не указаны, то будут установлены
	значения по-умолчанию: 10 символов и 25%).

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_FLOOD
	Синтаксис: KICK #канал FLOOD {ON|OFF} [ttb [линий [секунды]]]

	Будет ли бот кикать за флуд. Если включено,
	бот будет кикать флудеров с канала, если они сказали
	более линий строк текста за секунды секунд
	(Если параметры не указаны, то будут установлены
	значения по-умолчанию: 6 строк за 10 секунд).
	
	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_REPEAT
	Синтаксис: KICK #канал REPEAT {ON|OFF} [ttb [число_повторов]]

	Будет ли бот кикать за повторы. Если включено, бот будет кикат
	пользователей, которые повторяют один и тот же текст
	число повторов раз (Если параметры не указаны, то будут
	установлены значения по-умолчанию: 3).

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.

BOT_HELP_KICK_BADWORDS
	Синтаксис: KICK #канал BADWORDS {ON|OFF} [ttb]

	Будет ли бот кикать за плохие слова. Если включено,
	бот будет кикать пользователей, использующих различные
	плохие слова на канале.

	Вы можете установить плохие слова на канале, используя
	команду BADWORDS. Напишите %R%S HELP BADWORDS для
	получения более подробной информации.

	ttb это количество киков, после которых
	бот поставит бан. Если Вы не укажете ttb,
	пользователь никогда не будет баниться.
	
BOT_HELP_BADWORDS
	Синтаксис: BADWORDS #канал ADD слово [SINGLE | START | END]
	           BADWORDS #канал DEL {слово | номер записи | список_записей}
	           BADWORDS #канал LIST [маска | список_записей]
	           BADWORDS #канал CLEAR
	
	Управляет списком плохих слов для канала. Список плохих слов
	определяет, за использование каких слов пользователь будет
	кикнут ботом, при условии, что данная возможность включена.
	Для получения более полной информации, напишите
	%R%S HELP KICK BADWORDS.
	
	Команда BADWORDS ADD добавит указанное слово в список
	плохих слов. Если указан параметр SINGLE, пользователь будет
	кикнут, только если польностью произнесет это слово. Если указать
	параметр START, пользователь будет кикнут, когда произнесет слово,
	начинающееся на слово*. Если указать параметр END, то кик будет
	произведен, если пользователь произнесет слово, заканчивающееся на
	*слово. Если Вы не укажете ни один из этих параметров, любой
	пользователь будет кикнут всякий раз при упоминании *слова*.
	
	Команда BADWORDS DEL удаляет указанное слово из списка плохих слов.
	Если указать в качестве параметра номера записей, указанные записи
	будут удалены. (См. пример использования команды LIST ниже.)
	
	Команда BADWORDS LIST покажет текущий список плохих слов. Если указать
	символьную маску, только совпадающие с ней слова будут показаны.
	Если указать номера записей, будут показаны только указанные записи.
	Например:

	   BADWORDS #канал LIST 2-5,7-9
	      Покажет плохие слова с номерами записей от 2 до 5 и
	      с 7 по 9 включительно.
	      
	Команда BADWORDS CLEAR удалит ВСЕ плохие слова из списка.

BOT_HELP_SAY
	Синтаксис: SAY #канал текст
	
	Заставляет бота произнести указанный текст на указанный канал.

BOT_HELP_ACT
	Синтаксис: ACT #канал текст
	
	Заставляет бота выполнить команду, эквивалентную команду "/me"
	в mIRC на указанном канале, использовав указанный текст.

BOT_SERVADMIN_HELP_BOT
	Синтаксис: BOT ADD ник идент хост реальное_имя
	           BOT CHANGE старый ник новый ник [идент [хост [реальное_имя]]]
	           BOT DEL ник
	
	Позволяет Администраторам сервисов создавать, изменять
	и удалять ботов, которые могут быть в дальнейшем
	использованы пользователями на своих каналах.
	
	BOT ADD добавляет бота с указанным ником, идентом,
	хостом и реальным именем. Так как корректность этих значений
	не проверяется, будьте крайне осторожны!
	BOT CHANGE позволяет изменять ник, идент, хост
	или реальное имя бота без удаления его (и всех
	его настроек).
	BOT DEL удаляет указанного бота из списка ботов.  
	
	Примечание: Вы не можете создать бота с ником, который уже кем-то
	зарегистриован. Если вы создаете бота с незарегистрированным ником,
	любой, кто использует этот ник будет тут же "прибит", то есть - 
	отключен от сети.

BOT_SERVADMIN_HELP_SET
	
	Опции, зарезервированные для использования Администраторами сервисов:
	
	    NOBOT            Запретить использование ботов
	                     на каком-либо канале
	    PRIVATE          Запретить использование бота кем-либо, кроме
	                     IRC-Операторов

BOT_SERVADMIN_HELP_SET_NOBOT
	Синтаксис: SET #канал NOBOT {ON|OFF}
	
	Эта опция запрещает использовать ботов на указанном канале.
	Если бот уже установлен на канал, он будет автоматически удален
	с него сразу после включения данной опции.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Синтаксис: SET ник_бота PRIVATE {ON|OFF}
	
	Эта опция запрещает устанавливать бота на канал обычным
	пользователям. Такого бота могут установить только IRC-Операторы.

###########################################################################
#
# HostServ messages
#
###########################################################################
HOST_EMPTY
	Лист виртуальных хостов пуст.
HOST_ENTRY
	#%d Ник: %s, виртуальный хост: %s (%s - %s)
HOST_IDENT_ENTRY
	#%d Ник: %s, виртуальный хост: %s@%s (%s - %s)
HOST_SET
	Виртуальный хост для %s установлен на %s.
HOST_IDENT_SET
	Виртуальный хост для %s установлен на %s@%s.
HOST_SETALL
	Виртуальных хост для группы %s установлен на %s.
HOST_DELALL
	Виртуальные хосты для группы %s были удалены.
HOST_DELALL_SYNTAX
	Синтаксис: DELALL <ник>.
HOST_IDENT_SETALL
	Виртуальный хост для группы %s установлен в %s@%s.
HOST_SET_ERROR
	Маска виртуального хоста должна быть указана в правильном формате.
HOST_SET_IDENT_ERROR
	Идент виртуального хоста должен быть указан в правильном формате.
HOST_SET_TOOLONG
	Ошибка! Виртуальный хост слишком длинный, пожалуйста, используйте хост длиной меньше %d символов.
HOST_SET_IDENTTOOLONG
	Ошибка! Идент слишком длинный, пожалуйста, используйте идент длиной меньше %d символов.
HOST_NOREG
	Пользователь %s не найден в базе данных NickServ.
HOST_SET_SYNTAX
	Ситаксис: %R%s set <ник> <хостмаска>.
HOST_SETALL_SYNTAX
	Синтаксис %R%s setall <ник> <хостмаска>.
HOST_DENIED
	Доступ запрещен.
HOST_NOT_ASSIGNED
	Пожалуйста, обратитесь к IRC -ператору, чтобы получить виртуальный хост.
HOST_ACTIVATED
	Ваш виртуальный хост %s активирован.
HOST_IDENT_ACTIVATED
	Ваш виртуальный хост %s@%s активирован.
HOST_ID
	Сперва, вы должны идентифицировать свой ник.
HOST_NOT_REGED
	Ваш ник должен быть зарегистрирован, чтобы Вы смогли использовать виртуальный хост.
HOST_DEL
	Виртуальный хост для %s удален.
HOST_DEL_SYNTAX
	Синтаксис: %R%s del <ник>.
HOST_OFF_UNREAL
	Ваш виртуальный хост снят. Чтобы снова установить его , напишите /mode %s +%s
HOST_NO_VIDENT
	Ваш IRC-сервер не поддерживает виртуальные иденты. Если это не так, пожалуйста, сообщите нам о найденной ошибке.
HOST_GROUP
	Все виртуальные хосты в группе %s были установлены на %s
HOST_IDENT_GROUP
	Все виртуальные хосты в группе %s были установлены на %s@%s
HOST_LIST_FOOTER
	Показаны все записи (Кол-во: %d)
HOST_LIST_RANGE_FOOTER
	Показаны записи с %d по %d
HOST_LIST_KEY_FOOTER
	Показаны записи, содержащие ключ %s (Кол-во: %d)

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_HELP_CMD_ON
	    ON          Активирует назначенный Вам виртуальный хост

HOST_HELP_CMD_OFF
	    OFF         Деактивирует виртуальный хост

HOST_HELP_CMD_GROUP
	    GROUP       Синхронизирует виртуальный хост для всех ников в Вашей группе

HOST_HELP_CMD_SET
	    SET         Установить вирутальный хост пользователю

HOST_HELP_CMD_SETALL
	    SETALL      Установить виртуальный хост для всех ников в группе

HOST_HELP_CMD_DEL
	    DEL         Удалить виртуальный хост пользователя

HOST_HELP_CMD_DELALL
	    DELALL      Удалить виртуальные хосты для всех ников в группе

HOST_HELP_CMD_LIST
	    LIST        Показать список всех виртуальных хостов.

HOST_OPER_HELP
	не используется.

HOST_ADMIN_HELP
	не используется.

HOST_HELP
	Команды %S:

HOST_HELP_ON
	Синтаксис: ON

	Активирует виртуальный хост, назначенный Вашему нику.
	Когда Вы используете эту команду, любой пользователь,
	сделавший /whois на Вас увидит виртуальный хост вместо
	Вашего реального IP-адреса/хоста.

HOST_HELP_SET
	Синтаксис: SET <ник> <хостмаска>.

	Устанавливает виртуальный хост указанному нику на указанную
	хостмаску. Если Ваш IRC-сервер подерживает виртуальные иденты,
	можно использовать SET <ник> <идент>@<хостмаска> для установки
	идента и виртуального хоста одновременно.

HOST_HELP_DELALL
	Синтаксис: DELALL <ник>.

	Удаляет виртуальные хосты для всех ников в группе
	указанного ника.

HOST_HELP_SETALL
	Синтаксис: SETALL <ник> <хостмаска>.

	Устанавливает виртуальный хост для всех ников в группе
	указанного ника. Если Ваш IRC-сервер поддерживает виртуальные
	иденты, Вы можете использовать SETALL <ник> <идент>@<хостмаска>,
	чтобы установить идент и виртуальный хост одновременно.

	* Примечание, виртуальный хост НЕ будет обновляться автоматически для любого
	ника, добавленного в группу после использования этой команды.

HOST_HELP_OFF
	Синтаксис: OFF

	Деактивирует виртуальный хост, назначенный Вашему нику.
	Когда Вы используете эту команду любой пользователь,
	сделавший /whois на Вас, увидит Ваш реальный IP-адрес/хост.

HOST_HELP_DEL
	Синтаксис: DEL <ник>

	Удаляет виртуальный хост, назначенный указанному нику, из
	базы данных.

HOST_HELP_LIST
	Синтаксис: LIST [<ключ>|<#X-Y>]

	Эта команда показывает зарегистрированные виртуальные хосты оператору.
	Если указан Ключ, то будут показаны только те записи, которые содержат
	указанный шаблон, т.е. Rob* для всех записей, начинающихся с "Rob"

	Если использован параметр #X-Y, будут показаны записи от X
	до Y, т.е. #1-3 покажет первые 3 записи ник/виртуальный хост.

	Лимит одновременно показываемых значений ограничен числом
	NSListMax в конфигурационном файле сервисов.

HOST_HELP_GROUP
	Синтаксис: GROUP
	
	Эта команда позволяет пользователям установить виртуальный хост
	с их ТЕКУЩЕГО ника на все ники группы.

OPER_SVSNICK_UNSUPPORTED
	Извините, но команда SVSNICK недоступна в этой сети.

OPER_SQLINE_UNSUPPORTED
	Извините, но команда SQLINE недоступна в этой сети.

OPER_SVSO_UNSUPPORTED
	Извините, но команда OLINE недоступна в этой сети.

OPER_UMODE_UNSUPPORTED
	Извините, но команда UMODE недоступна в этой сети.

OPER_SUPER_ADMIN_NOT_ENABLED
	Установка SuperAdmin не включена в services.conf
