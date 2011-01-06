# Russian language file for Anope
#
# (C) 2003-2011 Anope Team
# Contact us at team@anope.org
#
# Please read COPYING and README for further details.
#
# Based on the original code of Epona by Lara.
# Based on the original code of Services by Andy Church.
#
# Translated by Kein <kein-of@yandex.ru>
#
# Translated: 17.10.2008
# Revision: 18
# Charaster set: cp1251
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
# services.conf).  The %S's do not count as sprintf() parameters, so they
# can be rearranged, removed, or added as necessary.
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
#	language-name-in-language (language-name-in-English)
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
	Неизвестная команда %s. Список команд доступен по %R%s HELP
SYNTAX_ERROR
	Синтаксис: %s
MORE_INFO
	%R%s HELP %s для более подробной информации.
NO_HELP_AVAILABLE
	Справочная информация по %s отсутствует.
OBSOLETE_COMMAND
	Данная команда устарела, вместо нее используйте %s

BAD_USERHOST_MASK
	Маска должна быть строго в формате идент@хост.
BAD_EXPIRY_TIME
	Указано неверное значение срока истечения.
USERHOST_MASK_TOO_WIDE
	Границы маски %s слишком широки, пожалуйста, уточните маску.

SERVICE_OFFLINE
	%s на данный момент неактивен (skeleton-режим).
READ_ONLY_MODE
	Примечание: сейчас сервисы работают в режиме read-only (только-чтение), никакие изменения сохранены НЕ будут!
PASSWORD_INCORRECT
	Вы указали неверный пароль.
INVALID_TARGET
	Адресация вида "/msg %s" больше не поддерживается. Используйте "/msg %s@%s" или "/%s".

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
	Команда RAW отключена. Если вы желаете использовать ее, закомментируйте директиву DisableRaw в конфигурационном файле сервисов.

MORE_OBSCURE_PASSWORD
	Пожалуйста, попробуйте снова с более сложным паролем. Он должен иметь длину не менее 5 символов, состоять только из латинских букв или цифр, не быть легко отгадываемым (т.е. Вашим именем или Вашим ником), и не должен содержать пробелов и табуляций.
PASSWORD_TOO_LONG
	Вы указали слишком длинный пароль, попробуйте еще раз с более коротким.

NICK_NOT_REGISTERED
	Ваш ник незарегистрирован.
NICK_NOT_REGISTERED_HELP
	Ваш ник незарегистрирован. Для более подробной инфрмации о регистрации ника см. %R%s HELP
NICK_X_IS_SERVICES
	Ник %s является ником одного из сервисных псевдоклиентов данной сети.
NICK_X_NOT_REGISTERED
	Ник %s незарегистрирован.
NICK_X_IN_USE
	Ник %s на данный момент используется кем-то другим.
NICK_X_NOT_IN_USE
	Ник %s в сети не обнаружен.
NICK_X_NOT_ON_CHAN
	Пользователь %s не присутствует в данный момент на канале %s
NICK_X_FORBIDDEN
	Ник %s не может быть зарегистрирован или использован.
NICK_X_FORBIDDEN_OPER
	Ник %s был запрещен к использованию %s:
	%s
NICK_X_ILLEGAL
	Указанный ник %s содержит недопустимые символы.
NICK_X_TRUNCATED
	Ник %s был усечен до %d символов.
NICK_X_SUSPENDED
	Ник %s находится в режиме саспенда.
CHAN_X_NOT_REGISTERED
	Канал %s не зарегистрирован.
CHAN_X_NOT_IN_USE
	Канал %s не существует.
CHAN_X_FORBIDDEN
	Регистрация или использование канала %s ЗАПРЕЩЕНЫ.
CHAN_X_FORBIDDEN_OPER
	Канал %s ЗАПРЕЩЕН к использованию %s:
	%s
CHAN_X_SUSPENDED
	Канал 'заморожен': [%s] %s
NICK_IDENTIFY_REQUIRED
	Для выполнения данной команды вы должны идентифицироваться.
	Попробуйте снова после того, как используете команду %R%s IDENTIFY пароль.
CHAN_IDENTIFY_REQUIRED
	Для выполнения данной команды вы должны идентифицироваться.
	Попробуйте снова после того, как используете команду %R%s IDENTIFY %s пароль.
	
MAIL_DISABLED
	Настройки mail-функций сервисов некорректны или отключены, возможность отправки email недоступна.
MAIL_INVALID
	Email-адрес для %s некорректен.
MAIL_X_INVALID
	Email-адрес вида %s не является правильным электронной почты.
MAIL_LATER
	На данный момент отправка email невозможна, попробуйте позже.
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
	истекает при ближайшем обновлении БД
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
	Задан неверный диапазон. Синтаксис: #откуда-докуда.
CS_LIST_INCORRECT_RANGE
	Что бы найти канал, начинающийся с символа #, просто укажите его
	название без #. Например anope вместо #anope.

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
	Ник, который вы сейчас используете, зарегистрирован на сервисах сети.
	Если это ваш ник, идентифицируйтесь командой %R%s IDENTIFY пароль
	В противном случае - выберите другой ник.
NICK_IS_SECURE
	Ник, который вы сейчас используете, зарегистрирован и защищен.
	Если это ваш ник, идентифицируйтесь командой %R%s IDENTIFY пароль.
	Иначе, выберите, пожалуйста, другой ник.
NICK_MAY_NOT_BE_USED
	Выбранный вами ник запрещен к использованию. Возьмите какой-нибудь другой.
FORCENICKCHANGE_IN_1_MINUTE
	Если вы не выполните указанные рекомендации, ваш ник будет принудительно изменен в течении минуты.
FORCENICKCHANGE_IN_20_SECONDS
	Если вы не выполните указанные рекомендации, ваш ник будет принудительно изменен в течении 20 секунд.
FORCENICKCHANGE_NOW
	Этот ник зарегистрирован кем-то другим, вы не можете его использовать.
FORCENICKCHANGE_CHANGING
	Ваш ник изменен на %s

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER пароль [email]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER пароль email
NICK_REGISTRATION_DISABLED
	Извините, но регистрация ников временно приостановлена.
NICK_REGISTRATION_FAILED
	Извините, но зарегистрировать ник не удалось.
NICK_REG_PLEASE_WAIT
	Подождите %d секунд перед повторным использованием команды REGISTER.
NICK_CANNOT_BE_REGISTERED
	Ник %s не может быть зарегистрирован.
NICK_ALREADY_REGISTERED
	Ник %s уже зарегистрирован!
NICK_REGISTERED
	Ник %s успешно зарегистрирован, в ACCESS-список добавлена хостмаска: %s
NICK_REGISTERED_NO_MASK
	Ник %s успешно зарегистрирован.
NICK_PASSWORD_IS
	Пароль для вашего ника - %s - запомните его, он пригодится вам в дальнейшем!
NICK_REG_DELAY
	Чтобы начать регистрацию вашего текущего ника, вы должны использовать его не менее %d секунд

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP главный_ник пароль
NICK_GROUP_DISABLED
	Извините, но возможность группировки ников временно недоступна.
NICK_GROUP_FAILED
	Извините, но сгруппировать ники не удалось.
NICK_GROUP_PLEASE_WAIT
	Подождите %d секунд перед повторным использованием команды GROUP.
NICK_GROUP_CHANGE_DISABLED
	Перед внесением ника в группу, вы должны удалить его командой %R%s DROP.
NICK_GROUP_SAME
	Вы уже состоите в группе %s.
NICK_GROUP_TOO_MANY
	Слишком много ников в группе %s. Проверьте список и удалите ненужные.
	Для более подробной информации см. %R%s HELP GLIST и %R%s HELP DROP
NICK_GROUP_JOINED
	Вы успешно внесены в группу %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY пароль
NICK_IDENTIFY_FAILED
	Извините, но идентифицировать ник не удалось.
NICK_IDENTIFY_SUCCEEDED
	Пароль принят - вы признаны как владелец ника.
NICK_IDENTIFY_EMAIL_REQUIRED
	Сначала вы должны установить email-адрес для вашего ника. Реальный email-адрес
	позволит вам использовать функцию восстановления пароля, если вы его забудете.
NICK_IDENTIFY_EMAIL_HOWTO
	Используйте команду %R%S SET EMAIL email-адрес чтобы установить email-адрес.
	Не волнуйтесь, данный email-адрес знать будете лишь вы и Администрация сети.
NICK_ALREADY_IDENTIFIED
	Вы уже идентифицированы.

# UPDATE responses
NICK_UPDATE_SUCCESS
	Статус на сервисах обновлен (мемо-сообщения, вирутальный хост, и т.д.).
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	Деидентификация выполнена успешно.
NICK_LOGOUT_X_SUCCEEDED
	Ник %s успешно деидентифицирован.
NICK_LOGOUT_SERVICESADMIN
	Невозможно деидентифицировать %s так как он Администратор сервисов.

# DROP responses
NICK_DROP_DISABLED
	Извините, но возможность удаления ников временно недоступна.
NICK_DROPPED
	Ваш ник успешно удален из базы данных сервисов.
NICK_X_DROPPED
	Ник %s успешно удален из базы данных сервисов.

# SET responses
NICK_SET_SYNTAX
	SET опция параметры
NICK_SET_SERVADMIN_SYNTAX
	SET [ник] опция параметры
NICK_SET_DISABLED
	Извините, но возможность настройки опций ника временно недоступна.
NICK_SET_UNKNOWN_OPTION
	Неизвестная SET-опция %s.
NICK_SET_OPTION_DISABLED
	Возможность настройки опции %s в вашей IRC-сети недоступна.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	Новый основной ник должен быть зарегистрированным ником и состоять в вашей группе!
NICK_SET_DISPLAY_CHANGED
	Основной ник группы изменен на %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Извините, но изменить пароль ника не удалось.
NICK_SET_PASSWORD_CHANGED
	Пароль для вашего ника успешно изменен.
NICK_SET_PASSWORD_CHANGED_TO
	Пароль для вашего ника успешно изменен на: %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE номер
NICK_SET_LANGUAGE_UNKNOWN
	Языка под номером %d не существует. Список доступных языков: %R%s HELP SET LANGUAGE.
NICK_SET_LANGUAGE_CHANGED
	Язык сервисов изменен на Русский.

# SET URL responses
NICK_SET_URL_CHANGED
	URL-адрес для вашего ника изменен на: %s.
NICK_SET_URL_UNSET
	URL-адрес для вашего ника удален.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	Email-адрес для вашего ника изменен на: %s.
NICK_SET_EMAIL_UNSET
	Email-адрес для вашего ника удален.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	Возможность удаления email-адреса в вашей IRC-сети недоступна.

# SET ICQ responses
NICK_SET_ICQ_CHANGED
	ICQ-номер для вашего ника изменен на: %s
NICK_SET_ICQ_UNSET
	ICQ-номер для вашего ника удален.
NICK_SET_ICQ_INVALID
	Значение вида %s не является правильным ICQ-номером.

# SET GREET responses
NICK_SET_GREET_CHANGED
	Приветственное сообщение для вашего ника изменено на: %s.
NICK_SET_GREET_UNSET
	Приветственное сообщение для вашего ника удалено.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Защита ника активирована, лимит времени на ввод пароля: 60 секунд.
NICK_SET_KILL_QUICK
	Защита ника активирована, лимит времени на ввод пароля: 20 секунд.
NICK_SET_KILL_IMMED
	Защита ника активирована, лимит времени на ввод пароля: 0 секунд.
NICK_SET_KILL_IMMED_DISABLED
	Возможность установки режима IMMED в вашей IRC-сети недоступна.
NICK_SET_KILL_OFF
	Защита ника отключена.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Режим безопасности ника активирован.
NICK_SET_SECURE_OFF
	Режим безопасности ника отключен.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Режим приватности ника активирован.
NICK_SET_PRIVATE_OFF
	Режим приватности ника отключен.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	С этого момента, ваш email-адрес будет скрыт из информации по %s INFO
NICK_SET_HIDE_EMAIL_OFF
	С этого момента, ваш email-адрес будет показываться в информации по %s INFO
NICK_SET_HIDE_MASK_ON
	Теперь ваша маска вида идент@хост будет скрыта из информации по %s INFO
NICK_SET_HIDE_MASK_OFF
	Теперь ваша маска вида идент@хост будет показываться в информации по %s INFO
NICK_SET_HIDE_QUIT_ON
	С этого момента, ваше последнее quit-сообщение будет скрыто из информации по %s INFO
NICK_SET_HIDE_QUIT_OFF
	Теперь ваше последнее quit-сообщение будет показываться в информации по %s INFO
NICK_SET_HIDE_STATUS_ON
	С этого момента, ваш уровень доступа к сервисам будет скрыт из информации по %s INFO
NICK_SET_HIDE_STATUS_OFF
	Теперь ваш уровень доступа к сервисам показываться в информации по %s INFO

# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET MSG {ON | OFF}
NICK_SET_MSG_ON
	Режим приватных сообщений активирован, теперь сервисы будут отвечать вам в приват.
NICK_SET_MSG_OFF
	Режим приватных сообщений отключен, сервисы будут отвечать вам посредством нотисов.

# SET AUTOOP responses
NICK_SET_AUTOOP_SYNTAX
	SET AUTOOP {ON | OFF}
NICK_SET_AUTOOP_ON
	Режим автостатуса активирован.
NICK_SET_AUTOOP_OFF
	Режим автостатуса отключен.

# SET NOEXPIRE responses
NICK_SASET_SYNTAX
	SASET ник опция параметры
NICK_SASET_DISABLED
	Извините, но возможность установки параметров ника временно недоступна.
NICK_SASET_UNKNOWN_OPTION
	Неизвестная SASET-опция %s.
NICK_SASET_BAD_NICK
	Ник %s не зарегистрирован.
NICK_SASET_OPTION_DISABLED
	Возможность настройки опции %s в вашей IRC-сети недоступна.

# SASET DISPLAY responses
NICK_SASET_DISPLAY_INVALID
	Новый главный ник для %s должен быть зарегистрированным ником и состоять в его группе!
NICK_SASET_DISPLAY_CHANGED
	Новый главный ник теперь: %s.

# SASET PASSWORD responses
NICK_SASET_PASSWORD_FAILED
	Извините, но изменить пароль для %s неудалось.
NICK_SASET_PASSWORD_CHANGED
	Пароль для %s успешно изменен.
NICK_SASET_PASSWORD_CHANGED_TO
	Пароль для %s изменен на %s.

# SASET URL responses
NICK_SASET_URL_CHANGED
	URL-адрес для %s изменен на %s.
NICK_SASET_URL_UNSET
	URL-адрес для %s удален.

# SASET EMAIL responses
NICK_SASET_EMAIL_CHANGED
	Email-адрес для %s изменен на %s.
NICK_SASET_EMAIL_UNSET
	Email-адрес для %s удален.
NICK_SASET_EMAIL_UNSET_IMPOSSIBLE
	Возможность удаления email-адреса в вашей IRC-сети недоступна.

# SASET ICQ responses
NICK_SASET_ICQ_CHANGED
	ICQ-номер для %s установлен на %s.
NICK_SASET_ICQ_UNSET
	ICQ-номер для %s удален.
NICK_SASET_ICQ_INVALID
	Значение %s не является правильным ICQ-номером.

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
	Защита ника %s активирована, лимит времени на ввод пароля: 60 секунд.
NICK_SASET_KILL_QUICK
	Защита ника %s активирована,лимит времени на ввод пароля: 20 секунд.
NICK_SASET_KILL_IMMED
	Защита ника %s активирована, лимит времени на ввод пароля: 0 секунд.
NICK_SASET_KILL_IMMED_DISABLED
	Возможность установки режима IMMED в вашей IRC-сети недоступна.
NICK_SASET_KILL_OFF
	Защита ника %s отключена.

# SASET SECURE responses
NICK_SASET_SECURE_SYNTAX
	SASET ник SECURE {ON | OFF}
NICK_SASET_SECURE_ON
	Режим безопасности для %s активирован.
NICK_SASET_SECURE_OFF
	Режим безопасности для %s отключен.

# SASET PRIVATE responses
NICK_SASET_PRIVATE_SYNTAX
	SASET ник PRIVATE {ON | OFF}
NICK_SASET_PRIVATE_ON
	Режим приватности для %s активирован.
NICK_SASET_PRIVATE_OFF
	Режим приватности для %s отключен.

# SASET HIDE responses
NICK_SASET_HIDE_SYNTAX
	SET ник HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SASET_HIDE_EMAIL_ON
	С этого момента, email-адрес ника %s будет скрыт из информации по %s INFO.
NICK_SASET_HIDE_EMAIL_OFF
	Теперь, email-адрес ника %s будет показываться в информации по %s INFO.
NICK_SASET_HIDE_MASK_ON
	Последняя маска (идент@хост) для %s теперь будет скрыта из информации по %s INFO.
NICK_SASET_HIDE_MASK_OFF
	Последняя маска (идент@хост) для %s теперь будет показываться в информации по %s INFO.
NICK_SASET_HIDE_QUIT_ON
	Последнее quit-сообщение для %s теперь будет скрыто из информации по %s INFO.
NICK_SASET_HIDE_QUIT_OFF
	Последнее quit-сообщение для %s теперь будет показываться в информации по %s INFO.
NICK_SASET_HIDE_STATUS_ON
	Уровень доступа к сервисам для %s теперь будет скрыт из информации по %s INFO.
NICK_SASET_HIDE_STATUS_OFF
	Уровень доступа к сервисам для %s теперь будет показываться в информации по %s INFO.

# SASET MSG responses
NICK_SASET_MSG_SYNTAX
	SASET ник MSG {ON | OFF}
NICK_SASET_MSG_ON
	Режим приватных сообщений для %s активирован, теперь сервисы будут отвечать ему в приват.
NICK_SASET_MSG_OFF
	Режим приватных сообщений для %s отключен, сервисы будут отвечать ему посредством нотисов.

# SET NOEXPIRE responses
NICK_SASET_NOEXPIRE_SYNTAX
	SASET ник NOEXPIRE {ON | OFF}
NICK_SASET_NOEXPIRE_ON
	С этого момента регистрация ника %s никогда не истечет.
NICK_SASET_NOEXPIRE_OFF
	C этого момента ник %s снова в режиме истечения.

# SASET AUTOOP responses
NICK_SASET_AUTOOP_SYNTAX
	SASET ник AUTOOP {ON | OFF}
NICK_SASET_AUTOOP_ON
	Режим автостатуса для %s активирован
NICK_SASET_AUTOOP_OFF
	Режим автостатуса для %s отключен

# SASET LANGUAGE responses
NICK_SASET_LANGUAGE_SYNTAX
	SASET ник LANGUAGE номер
NICK_SASET_LANGUAGE_UNKNOWN
	Языка под номером %d не существует. Список доступных языков: %R%s HELP SET LANGUAGE.
NICK_SASET_LANGUAGE_CHANGED
	Language for %s changed to %s.

# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [маска]
NICK_ACCESS_ALREADY_PRESENT
	Маска вида %s уже присутствует в вашем списке доступа.
NICK_ACCESS_REACHED_LIMIT
	Извините, но количество записей в списке доступа не может превышать %d
NICK_ACCESS_ADDED
	Маска вида %s успешно добавлена в ваш список доступа.
NICK_ACCESS_NOT_FOUND
	Маска вида %s не найдена в вашем списке доступа.
NICK_ACCESS_DELETED
	Маска вида %s успешно удалена из вашего списка доступа.
NICK_ACCESS_LIST
	Ваш текущий список доступа:
NICK_ACCESS_LIST_X
	Список доступа для %s:
NICK_ACCESS_LIST_EMPTY
	Ваш список доступа пуст.
NICK_ACCESS_LIST_X_EMPTY
	Список доступа для %s пуст.

# Status messages
NICK_STATUS_0
	STATUS %s 0 (ник незарегистрирован или его нет в сети)
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
	%s имеет привилегии Оператора сервисов.
NICK_INFO_SERVICES_ADMIN
	%s имеет привилегии Администратора сервисов.
NICK_INFO_SERVICES_ROOT
	%s имеет привилегии Рута сервисов.
NICK_INFO_ADDRESS
	Последняя маска.........: %s
NICK_INFO_ADDRESS_ONLINE
	Сейчас онлайн, c маской.: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s сейчас в сети.
NICK_INFO_TIME_REGGED
	Зарегистрирован.........: %s
NICK_INFO_LAST_SEEN
	Последний раз замечен...: %s
NICK_INFO_LAST_QUIT
	Последнее quit-сообщение: %s
NICK_INFO_URL
	URL-адрес...............: %s
NICK_INFO_EMAIL
	Email-адрес.............: %s
NICK_INFO_VHOST
	Виртуальный хост........: %s
NICK_INFO_VHOST2
	Виртуальный хост........: %s@%s
NICK_INFO_ICQ
	ICQ-номер...............: %d
NICK_INFO_GREET
	Текст приветствия.......: %s
NICK_INFO_OPTIONS
	Активированные опции....: %s
NICK_INFO_EXPIRE
	Регистрация истекает....: %s
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
	Автостатус
NICK_INFO_OPT_NONE
	отсутствуют
NICK_INFO_NO_EXPIRE
	Дополнительно: регистрация этого ника никогда не истечет.
NICK_INFO_FOR_MORE
	Для более подробной информации см. %R%s INFO %s ALL.
NICK_INFO_SUSPENDED
	Дополнительно: этот ник находится в режиме саспенда по причине: %s
NICK_INFO_SUSPENDED_NO_REASON
	Дополнительно: этот ник находится в режиме саспенда (заморожен).

# LIST responses
NICK_LIST_SYNTAX
	LIST маска
NICK_LIST_SERVADMIN_SYNTAX
	LIST маска [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Список ников по маске %s:
NICK_LIST_RESULTS
	Конец списка - %d/%d записей показано.

# ALIST responses
NICK_ALIST_SYNTAX
	ALIST ник
NICK_ALIST_HEADER
	Каналы, на которых у вас есть уровни доступа:
	Номер  Канал                Уровень  Описание
NICK_ALIST_HEADER_X
	Каналы, на которых у %s есть уровни доступа:
	Номер  Канал                Уровень  Описание
NICK_ALIST_XOP_FORMAT
	  %3d %c%-20s %-8s %s
NICK_ALIST_ACCESS_FORMAT
	  %3d %c%-20s %-8d %s
NICK_ALIST_FOOTER
	Конец списка - %d/%d каналов показано.

# GLIST responses
NICK_GLIST_HEADER
	Список ников в вашей группе:
NICK_GLIST_HEADER_X
	Список ников в группе %s:
NICK_GLIST_FOOTER
	Количество ников в группе: %d
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (истекает %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER ник [пароль]
NICK_NO_RECOVER_SELF
	Эээ... вы не можете использовать команду RECOVER относительно самого себя.
NICK_RECOVERED
	Ваш ник успешно освобожден от использования его другим пользователем сети.
	Чтобы освободить ваш ник от удержания его сервисами, используйте команду
	%R%s RELEASE %s или просто немного подождите (время ожидания: %s).

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE ник [пароль]
NICK_RELEASE_NOT_HELD
	На данный момент, ник %s сервисами не удерживается.
NICK_RELEASED
	Ваш ник освобожден от удержания сервисами.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST ник [пароль]
NICK_NO_GHOST_SELF
	Вы не можете закрыть свою сессию!
NICK_GHOST_KILLED
	Сессия с вашим зависшим ником успешно закрыта.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS ник
NICK_GETPASS_UNAVAILABLE
	Команда GETPASS недоступна, так как включено шифрование паролей.
NICK_GETPASS_PASSWORD_IS
	Пароль для ника %s - %s

# GETEMAIL responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL логин@email-хост.tld (символьные маски запрещены!!)
NICK_GETEMAIL_EMAILS_ARE
	Найдено соответствие для ника %s: %s.
NICK_GETEMAIL_NOT_USED
	Ники с email-адресом %s не обнаружены.

# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS ник
NICK_SENDPASS_UNAVAILABLE
	Команда SENDPASS недоступна, так как включено шифрование паролей.
NICK_SENDPASS_SUBJECT
	Пароль для ника %s
NICK_SENDPASS_HEAD
	Доброго времени суток, многоуважаемый/ая!
NICK_SENDPASS_LINE_1
	Был получен запрос на восстановление пароля от вашего ника %s
NICK_SENDPASS_LINE_2
	Ваш пароль %s * Для большей безопасности, рекомендуется изменить его как можно скорее.
NICK_SENDPASS_LINE_3
	Если вы не запрашивали это письмо, просто проигнорируйте его.
NICK_SENDPASS_LINE_4
	ПОЖАЛУЙСТА, НЕ ОТВЕЧАЙТЕ НА ЭТО ПИСЬМО!
NICK_SENDPASS_LINE_5
	Администрация сети %s.
NICK_SENDPASS_OK
	Пароль для ника %s был выслан на его email-адрес.

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND ник причина
NICK_SUSPEND_SUCCEEDED
	С этого момента ник %s находится режиме саспенда.
NICK_SUSPEND_FAILED
	Извините, но установить режим саспенда на ник %s неудалось.

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND ник
NICK_UNSUSPEND_SUCCEEDED
	Режим саспенда с ника %s успешно снят.
NICK_UNSUSPEND_FAILED
	Извините, но снять режим саспенда с ника %s неудалось.

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID ник [причина]
NICK_FORBID_SYNTAX_REASON
	FORBID ник причина
NICK_FORBID_SUCCEEDED
	С этого момента ник %s запрещен к использованию.
NICK_FORBID_FAILED
	Извините, но установить режим запрета на ник %s неудалось.

# Nick Registraion responses
NICK_REQUESTED
	С используемого вами ника уже был получен запрос на регистрацию. Если регистрацию запрашивали именно вы, проверьте ваш e-mail на предмет получения auth-кода.
NICK_REG_RESENT
	Ваш код аутенфикации был заново выслан на %s.
NICK_REG_UNABLE
	Ник НЕ зарегистрирован, попробуйте, пожалуйста, позже.
NICK_IS_PREREG
	Этот ник ожидает подтверждения через e-mail перед завершением регистрации.
NICK_ENTER_REG_CODE
	Auth-код был выслан на %s, для завершения регистрации используйте команду %R%s confirm auth-код
NICK_CONFIRM_NOT_FOUND
	Время первой части регистрации истекло, воспользуйтесь командой "%R%s register <пароль> <email>"
NICK_CONFIRM_INVALID
	Был введен неверный auth-код, пожалуйста, проверьте ваш e-mail снова и попробуйте еще раз.
NICK_REG_MAIL_SUBJECT
	Регистрация ника %s
NICK_REG_MAIL_HEAD
	Доброго времени суток, многоуважаемый/ая!
NICK_REG_MAIL_LINE_1
	Вы запросили регистрацию ника %s
NICK_REG_MAIL_LINE_2
	Для завершения процесса регистрации ника, воспользуйтесь командой %R%s CONFIRM %s
NICK_REG_MAIL_LINE_3
	Если вы не запрашивали это письмо - просто проигнорируйте его.
NICK_REG_MAIL_LINE_4
	ПОЖАЛУЙСТА, НЕ ОТВЕЧАЙТЕ НА ЭТО ПИСЬМО!
NICK_REG_MAIL_LINE_5
	Администрация сети %s.
NICK_GETPASS_PASSCODE_IS
	Пароль для ника %s - %s.
NICK_FORCE_REG
	Регистрация ника %s подтверждена.

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Уровень доступа к автостатусу оператора (+o)
CHAN_LEVEL_AUTOVOICE
	Уровень доступа к автостатусу войса (+v)
CHAN_LEVEL_AUTOHALFOP
	Уровень доступа к автостатусу полуоператора (+h)
CHAN_LEVEL_AUTOPROTECT
	Уровень доступа к автостатусу защиты/админа (+a)
CHAN_LEVEL_AUTODEOP
	Уровень доступа, запрещающий получение статуса опа
CHAN_LEVEL_NOJOIN
	Уровень доступа, запрещающий заходить на канал
CHAN_LEVEL_INVITE
	Уровень доступа к команде INVITE
CHAN_LEVEL_AKICK
	Уровень доступа к команде AKICK
CHAN_LEVEL_SET
	Уровень доступа к команде SET (исключая опции FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Уровень доступа к команде CLEAR
CHAN_LEVEL_UNBAN
	Уровень доступа к команде UNBAN
CHAN_LEVEL_OPDEOP
	Уровень доступа к командам OP/DEOP
CHAN_LEVEL_ACCESS_LIST
	Уровень доступа к команде ACCESS LIST
CHAN_LEVEL_ACCESS_CHANGE
	Уровень доступа к командам модификации ACCESS-списка канала
CHAN_LEVEL_MEMO
	Уровень доступа к чтению мемо-сообщений канала
CHAN_LEVEL_ASSIGN
	Доступ к командам ASSIGN/UNASSIGN
CHAN_LEVEL_BADWORDS
	Уровень доступа к команде BADWORDS
CHAN_LEVEL_NOKICK
	Уровень доступа, исключающий обладателя из условий киков бота
CHAN_LEVEL_FANTASIA
	Уровень доступа к командам FANTASY-режима
CHAN_LEVEL_SAY
	Уровень доступа к командам SAY и ACT
CHAN_LEVEL_GREET
	Уровень доступа к использованию приветственного сообщения
CHAN_LEVEL_VOICEME
	Уровень доступа к командам VOICE/DEVOICE относительно себя
CHAN_LEVEL_VOICE
	Уровень доступа к командам VOICE/DEVOICE
CHAN_LEVEL_GETKEY
	Уровень доступа к команде GETKEY
CHAN_LEVEL_OPDEOPME
	Уровень доступа к командам OP/DEOP относительно себя
CHAN_LEVEL_HALFOPME
	Уровень доступа к командам HALFOP/DEHALFOP относительно себя
CHAN_LEVEL_HALFOP
	Уровень доступа к командам HALFOP/DEHALFOP
CHAN_LEVEL_PROTECTME
	Уровень доступа к командам PROTECT/DEPROTECT относительно себя
CHAN_LEVEL_PROTECT
	Уровень доступа к командам PROTECT/DEPROTECT
CHAN_LEVEL_KICKME
	Уровень доступа к команде KICK относительно себя
CHAN_LEVEL_KICK
	Уровень доступа к команде KICK
CHAN_LEVEL_SIGNKICK
	Уровень доступа, исключающий попадание под условия SIGNKICK
CHAN_LEVEL_BANME
	Уровень доступа к команде BAN относительно себя
CHAN_LEVEL_BAN
	Уровень доступа к команде BAN
CHAN_LEVEL_TOPIC
	Уровень доступа к команде TOPIC
CHAN_LEVEL_INFO
	Уровень доступа к команде INFO с параметром ALL

# Automatic responses
CHAN_IS_REGISTERED
	Этот канал был зарегистрирован с помощью %s
CHAN_NOT_ALLOWED_OP
	Статус оператора на канале %s для вас запрещен.
CHAN_MAY_NOT_BE_USED
	Использование данного канала запрещено.
CHAN_NOT_ALLOWED_TO_JOIN
	Вам не разрешено находиться на этом канале.
CHAN_X_INVALID
	Указано недопустимое имя канала: %s

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER #канал пароль описание
CHAN_REGISTER_DISABLED
	Извините, регистрация каналов временно приостановлена.
CHAN_REGISTER_NOT_LOCAL
	Локальные каналы не могут быть зарегистрированы.
CHAN_MUST_REGISTER_NICK
	Сначала вы должны зарегистрировать свой ник. Напишите %R%s HELP для получения информации о регистрации ника.
CHAN_MUST_IDENTIFY_NICK
	Сначала вы должны идентифицировать свой ник на %s, используя команду:
	%R%s IDENTIFY пароль
CHAN_MAY_NOT_BE_REGISTERED
	Канал %s не может быть зарегистрирован.
CHAN_ALREADY_REGISTERED
	Канал %s уже зарегистрирован!
CHAN_MUST_BE_CHANOP
	Чтобы зарегистрировать канал, вы должны быть его оператором.
CHAN_REACHED_CHANNEL_LIMIT
	Количество зарегистрированных вами каналов достигло %d, а это - максимум.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Извините, но вы уже превысили лимит (%d) зарегистрированных каналов.
CHAN_REGISTRATION_FAILED
	Извините, но зарегистрировать канал неудалось.
CHAN_REGISTERED
	Канал %s успешно зарегистрирован на ник %s
CHAN_PASSWORD_IS
	Пароль от вашего канала -  %s - запомните его для дальнейшего использования.
CHAN_REGISTER_NONE_CHANNEL
	Вы пытаетесь зарегистрировать несуществующий канал %s
CHAN_SYMBOL_REQUIRED
	Вы должны указать символ # перед названием канала.


# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY #канал пароль
CHAN_IDENTIFY_FAILED
	Извините, но произвести идентификацию неудалось.
CHAN_IDENTIFY_SUCCEEDED
	Пароль принят, теперь ваш уровень доступа на канале %s - владелец.
CHAN_LOGOUT_FOUNDER_FAILED
	Вы не можете деидентифицировать себя отностительно канала %s т.к. вы его реальный владелец.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT #канал ник
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT #канал [ник]
CHAN_LOGOUT_SUCCEEDED
	Пользователь %s успешно деидентифицирован от канала %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	Все пользователи, идентифицированные к каналу %s, успешно деидентифицированы.

# DROP responses
CHAN_DROP_SYNTAX
	DROP #канал
CHAN_DROP_DISABLED
	Извините, возможность удаление каналов временно недоступна.
CHAN_DROPPED
	Канал %s удален из базы данных.

# SET responses
CHAN_SET_SYNTAX
	SET #канал опция параметр
CHAN_SET_DISABLED
	Извините, возможность установки опций каналов временно недоступна.
CHAN_SET_UNKNOWN_OPTION
	Неизвестная SET-опция %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s имеет слишком много зарегистрированных каналов.
CHAN_FOUNDER_CHANGED
	Все права на владение каналом %s успешно переданы пользователю %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Наследником канала %s назначен пользователь %s.
CHAN_SUCCESSOR_UNSET
	Наследник канал %s удален.
CHAN_SUCCESSOR_IS_FOUNDER
	%s не может быть наследником канала %s так как он его владелец.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Извините, установить пароль неудалось.
CHAN_PASSWORD_CHANGED
	Пароль для канала %s успешно изменен.
CHAN_PASSWORD_CHANGED_TO
	Пароль для канала %s изменен на %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Описание канала %s изменено на %s.

# SET URL responses
CHAN_URL_CHANGED
	URL-адрес канала %s изменен на %s.
CHAN_URL_UNSET
	URL-адрес канала %s удален.

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
	Число %s не является валидным номером шаблона банмаски.
CHAN_SET_BANTYPE_CHANGED
	Шаблон банмаски на канале %s изменен на #%d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Неизвестный режим %c - проигнорирован.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Блокировка режима %c невозможна - режим проигнорирован.
CHAN_SET_MLOCK_L_REQUIRED
	Перед блокировкой режима +L, вы сперва должны заблокировать режим +l.
CHAN_SET_MLOCK_K_REQUIRED
	Перед блокировкой режима +K, вы сперва должны заблокировать режим +i.
CHAN_MLOCK_CHANGED
	Блокируемые режимы на канале %s изменены на %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET #канал KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Режим хранения топика для канала %s активирован.
CHAN_SET_KEEPTOPIC_OFF
	Режим хранения топика для канала %s отключен.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET #канал TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Режим блокировки топика для канала %s активирован.
CHAN_SET_TOPICLOCK_OFF
	Режим блокировки топика для канала %s отключен.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET #канал PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Режим спокойствия для канала %s активирован.
CHAN_SET_PEACE_OFF
	Режим спокойствия для канала %s отключен.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET #канал PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Режим приватности канала %s активирован.
CHAN_SET_PRIVATE_OFF
	Режим приватности канала %s отключен.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET #канал SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Режим контроля за статусом оператора для канала %s активирован.
CHAN_SET_SECUREOPS_OFF
	Режим контроля за статусом оператора для канала %s отключен.

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET #канал SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Режим безопасности владельца для канала %s активирован.
CHAN_SET_SECUREFOUNDER_OFF
	Режим безопасности владельца для канала %s отключен.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET #канал RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Режим ограниченного доступа на канал %s активирован.
CHAN_SET_RESTRICTED_OFF
	Режим ограниченного доступа на канал %s отключен.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET #канал SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Режим безопасности для канала %s активирован.
CHAN_SET_SECURE_OFF
	Режим безопасности для канала %s отключен.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET #канал SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Режим подписанных киков для канала %s активирован.
CHAN_SET_SIGNKICK_LEVEL
	Режим подписанных киков для канала %s активирован,
	но будет работать в зависимости от настроек уровней доступа.
CHAN_SET_SIGNKICK_OFF
	Режим подписанных киков для канала %s отключен.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET #канал OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Режим OP-уведомлений для канала %s активирован.
CHAN_SET_OPNOTICE_OFF
	Режим OP-уведомлений для канала %s отключен.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET #канал XOP {ON | OFF}
CHAN_SET_XOP_ON
	Система привелегий xOP для канала %s активирована.
CHAN_SET_XOP_OFF
	Система привелегий xOP для канала %s отключена.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET #канал NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	С этого момента, регистрация канала %s никогда не истечет.
CHAN_SET_NOEXPIRE_OFF
	Режим истечения по времени для канала %s активирована.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Извините, но количество VOP/HOP/AOP/SOP-записей не может первышать %d 
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	Вы не можете использовать эту команду, вместо нее используйте ACCESS.
	Для более подробной информации см. %R%s HELP ACCESS
CHAN_XOP_NOT_AVAILABLE
	Система привелегий xOP недоступна.

# AOP messages
CHAN_AOP_SYNTAX
	AOP #канал {ADD|DEL|LIST|CLEAR} [ник | номер запис]
CHAN_AOP_DISABLED
	Извините, но возможность модификации списка AOP'ов временно недоступна.
CHAN_AOP_NICKS_ONLY
	Список AOP'ов может включать в себя только зарегистрированные ники.
CHAN_AOP_ADDED
	%s успешно добавлен в AOP'ы канала %s.
CHAN_AOP_MOVED
	%s перемещен в AOP'ы канала %s.
CHAN_AOP_NO_SUCH_ENTRY
	Запись под номером #%d в списке AOP'ов канала %s не обнаружена.
CHAN_AOP_NOT_FOUND
	Ник %s в списке AOP'ов канала %s не обнаружен.
CHAN_AOP_NO_MATCH
	Совпадающих записей в списке AOP'ов канала %s не обнаружено.
CHAN_AOP_DELETED
	%s успешно удален из AOP'ов канала %s.
CHAN_AOP_DELETED_ONE
	Удалена 1 запись из списка AOP'ов канала %s.
CHAN_AOP_DELETED_SEVERAL
	Удалено %d записей из списка AOP'ов канала %s.
CHAN_AOP_LIST_EMPTY
	Список AOP'ов канала %s пуст.
CHAN_AOP_LIST_HEADER
	Список AOP'ов канала %s:
	 Номер  Ник
CHAN_AOP_CLEAR
	Список AOP'ов канала %s был полностью очищен.

# HOP messages
CHAN_HOP_SYNTAX
	HOP #канал {ADD|DEL|LIST|CLEAR} [ник | номер запис]
CHAN_HOP_DISABLED
	Извините, но возможность модификации списка HOP'ов временно недоступна.
CHAN_HOP_NICKS_ONLY
	Список HOP'ов может включать в себя только зарегистрированные ники.
CHAN_HOP_ADDED
	%s успешно добавлен в HOP'ы канала %s.
CHAN_HOP_MOVED
	%s перемещен в HOP'ы канала %s.
CHAN_HOP_NO_SUCH_ENTRY
	Запись под номером #%d в списке HOP'ов канала %s не обнаружена.
CHAN_HOP_NOT_FOUND
	Ник %s в списке HOP'ов канала %s не обнаружен.
CHAN_HOP_NO_MATCH
	Совпадающих записей в списке HOP'ов канала %s не обнаружено.
CHAN_HOP_DELETED
	%s успешно удален из списка HOP'ов канала %s.
CHAN_HOP_DELETED_ONE
	Удалена 1 запись из списка HOP'ов канала %s.
CHAN_HOP_DELETED_SEVERAL
	Удалено %d записей из списка HOP'ов канала %s.
CHAN_HOP_LIST_EMPTY
	Список HOP'ов канала %s пуст.
CHAN_HOP_LIST_HEADER
	Список HOP'ов канала %s:
	  Номер  Ник
CHAN_HOP_CLEAR
	Список HOP'ов канала %s был полностью очищен.

# SOP messages
CHAN_SOP_SYNTAX
	SOP #канал {ADD|DEL|LIST|CLEAR} [ник | номер запис]
CHAN_SOP_DISABLED
	Извините, но возможность модификации списка SOP'ов временно недоступна.
CHAN_SOP_NICKS_ONLY
	Список SOP'ов может включать в себя только зарегистрированные ники.
CHAN_SOP_ADDED
	%s успешно добавлен в SOP'ы канала %s.
CHAN_SOP_MOVED
	%s перемещен в SOP'ы канала %s.
CHAN_SOP_NO_SUCH_ENTRY
	Запись под номером #%d в списке SOP'ов канала %s не обнаружена.
CHAN_SOP_NOT_FOUND
	Ник %s в списке SOP'ов канала %s не обнаружен.
CHAN_SOP_NO_MATCH
	Совпадающих записей в списке SOP'ов канала %s не обнаружено.
CHAN_SOP_DELETED
	%s успешно удален из SOP'ов канала %s.
CHAN_SOP_DELETED_ONE
	Удалена 1 запись из списка SOP'ов канала %s.
CHAN_SOP_DELETED_SEVERAL
	Удалено %d записей из списка SOP'ов канала %s.
CHAN_SOP_LIST_EMPTY
	Список SOP'ов канала %s пуст.
CHAN_SOP_LIST_HEADER
	Список SOP'ов канала %s:
	 Номер  Ник
CHAN_SOP_CLEAR
	Список SOP'ов канала %s был полностью очищен.

# VOP messages
CHAN_VOP_SYNTAX
	VOP #канал {ADD|DEL|LIST|CLEAR} [ник | номер запис]
CHAN_VOP_DISABLED
	Извините, но возможность модификации списка VOP'ов временно недоступна.
CHAN_VOP_NICKS_ONLY
	Список VOP'ов может включать в себя только зарегистрированные ники.
CHAN_VOP_ADDED
	%s успешно добавлен в VOP'ы канала %s.
CHAN_VOP_MOVED
	%s перемещен в VOP'ы канала %s.
CHAN_VOP_NO_SUCH_ENTRY
	Запись под номером #%d в списке VOP'ов канала %s не обнаружена.
CHAN_VOP_NOT_FOUND
	Ник %s в списке VOP'ов канала %s не обнаружен.
CHAN_VOP_NO_MATCH
	Совпадающих записей в списке VOP'ов канала %s не обнаружено.
CHAN_VOP_DELETED
	%s успешно удален из VOP'ов канала %s.
CHAN_VOP_DELETED_ONE
	Удалена 1 запись из списка VOP'ов канала %s.
CHAN_VOP_DELETED_SEVERAL
	Удалено %d записей из списка VOP'ов канала %s.
CHAN_VOP_LIST_EMPTY
	Список VOP'ов канала %s пуст.
CHAN_VOP_LIST_HEADER
	Список VOP'ов канала %s:
	 Номер  Ник
CHAN_VOP_CLEAR
	Список VOP'ов канала %s был полностью очищен.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS #канал {ADD|DEL|LIST|CLEAR} [ник [уровень] | номер_записи]
CHAN_ACCESS_XOP
	Вы не можете использовать эту команду так как включена система привилегий xOP.
	Для модификации списка доступа используйте команды VOP, AOP и SOP.
	Более подробная информация доступна по %R%s HELP команда
CHAN_ACCESS_XOP_HOP
	Вы не можете использовать эту команду так как включена система привилегий xOP.
	Для модификации списка доступа используйте команды VOP, HOP, AOP и SOP.
	Более подробная информация доступна по %R%s HELP команда
CHAN_ACCESS_DISABLED
	Извините, но возможность модификации списка доступа канала временно недоступна.
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
	Запись под номером #%d в списке доступа канала %s не обнаружена.
CHAN_ACCESS_NOT_FOUND
	Ник %s в списке доступа канала %s не обнаружен.
CHAN_ACCESS_NO_MATCH
	Совпадающих записей в списке доступа канала %s не обнаружено.
CHAN_ACCESS_DELETED
	Ник %s успешно удален из списка доступа канала %s.
CHAN_ACCESS_DELETED_ONE
	Удалена 1 запись из списка доступа канала %s.
CHAN_ACCESS_DELETED_SEVERAL
	Удалено %d записей из списка доступа канала %s.
CHAN_ACCESS_LIST_EMPTY
	Список доступа канала %s пуст.
CHAN_ACCESS_LIST_HEADER
	Список доступа канала %s:
	  Ном.  Ур.  Ник
CHAN_ACCESS_LIST_FOOTER
	Конец списка доступа.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	Список доступа канала %s был полностью очищен.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK #канал {ADD|STICK|UNSTICK|DEL|LIST|VIEW|ENFORCE|CLEAR} [ник|маска] [причина]
CHAN_AKICK_DISABLED
	Извините, но возможность модификации списка автокиков канала временно недоступна.
CHAN_AKICK_ALREADY_EXISTS
	Запись вида %s уже присутствует в списке автокиков канала %s.
CHAN_AKICK_REACHED_LIMIT
	Список автокиков канала может содержать не более %d записей.
CHAN_AKICK_ADDED
	Запись вида %s успешно добавлена в список автокиков канала %s.
CHAN_AKICK_NO_SUCH_ENTRY
	Запись под номером #%d в списке автокиков канала %s не обнаружена.
CHAN_AKICK_NOT_FOUND
	Запись вида %s в списке автокиков канала %s не обнаружена.
CHAN_AKICK_NO_MATCH
	Совпадающих значений в списке автокиков канала %s не обнаружено.
CHAN_AKICK_STUCK
	Блокировка банмаски %s на канале %s активирована.
CHAN_AKICK_UNSTUCK
	Блокировка банмаски %s на канале %s снята.
CHAN_AKICK_DELETED
	Запись вида %s удалена из списка автокиков канала %s.
CHAN_AKICK_DELETED_ONE
	Удалена 1 запись из списка автокиков канала %s.
CHAN_AKICK_DELETED_SEVERAL
	Удалено %d записей из списка актокиков канала %s.
CHAN_AKICK_LIST_EMPTY
	Список автокиков канала %s пуст.
CHAN_AKICK_LIST_HEADER
	Список автокиков канала %s:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (от %s в %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (заблокирован) (от %s в %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE на канале %s завершен, пользователей забанено: %d 
CHAN_AKICK_CLEAR
	Список AKICK'ов на канале %s был полностью очищен.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS #канал {SET | DIS[ABLE] | LIST | RESET} [пункт [уровень]]
CHAN_LEVELS_XOP
	Система уровней не доступна, так как используется система привилегий xOP.
CHAN_LEVELS_RANGE
	Уровень должен быть числом между %d и %d включительно.
CHAN_LEVELS_CHANGED
	Уровень доступа к %s на канале %s изменен на %d.
CHAN_LEVELS_UNKNOWN
	Неизвестный пункт %s. Для получения списка всех доступных значений см. %R%s HELP LEVELS DESC
CHAN_LEVELS_DISABLED
	Уровень доступа к %s на канале %s отключен.
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
	STATUS ERROR Ошибка синтаксиса
CHAN_STATUS_NOT_REGGED
	STATUS ERROR Канал %s незарегистрирован
CHAN_STATUS_FORBIDDEN
	STATUS ERROR Канал %s запрещен к использованию
CHAN_STATUS_NOTONLINE
	STATUS ERROR Ник %s в сети отсутствует.
CHAN_STATUS_INFO
	STATUS %s %s %d

# INFO responses
CHAN_INFO_SYNTAX
	INFO #канал [ALL]
CHAN_INFO_HEADER
	Информация о канале %s:
CHAN_INFO_FOUNDER
	Владелец..................: %s (%s)
CHAN_INFO_NO_FOUNDER
	Владелец..................: %s
CHAN_INFO_SUCCESSOR
	Наследник.................: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	Наследник.................: %s
CHAN_INFO_DESCRIPTION
	Описание..................: %s
CHAN_INFO_ENTRYMSG
	Сообщение при входе.......: %s
CHAN_INFO_TIME_REGGED
	Зарегистрирован...........: %s
CHAN_INFO_LAST_USED
	Последний раз использован.: %s
CHAN_INFO_LAST_TOPIC
	Последний топик...........: %s
CHAN_INFO_TOPIC_SET_BY
	Топик установил...........: %s
CHAN_INFO_URL
	URL-адрес.................: %s
CHAN_INFO_EMAIL
	Email-адрес...............: %s
CHAN_INFO_BANTYPE
	Тип бана..................: %d
CHAN_INFO_OPTIONS
	Активированные опции......: %s
CHAN_INFO_OPT_KEEPTOPIC
	Хранение топика
CHAN_INFO_OPT_OPNOTICE
	OP-уведомление
CHAN_INFO_OPT_PEACE
	Спокойствие
CHAN_INFO_OPT_PRIVATE
	Приватный
CHAN_INFO_OPT_RESTRICTED
	Ограниченный доступ
CHAN_INFO_OPT_SECURE
	Безопасность
CHAN_INFO_OPT_SECUREOPS
	Контроль статуса оператора
CHAN_INFO_OPT_SECUREFOUNDER
	Безопасность владельца
CHAN_INFO_OPT_SIGNKICK
	Подписанные кики
CHAN_INFO_OPT_TOPICLOCK
	Блокировка топика
CHAN_INFO_OPT_XOP
	Система привелегий xOP
CHAN_INFO_OPT_NONE
	отсутствуют
CHAN_INFO_MODE_LOCK
	Блокировка режимов........: %s
CHAN_INFO_EXPIRE
	Регистрация истекает......: %s
CHAN_INFO_NO_EXPIRE
	Дополнительно: регистрация этого канала никогда не истечет.

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
	Все баны канала %s удалены.
CHAN_CLEARED_EXCEPTS
	Все исключения из банов канала %s удалены.
CHAN_CLEARED_MODES
	Все установленные режимы канала %s сброшены.
CHAN_CLEARED_OPS
	Все опы (+о) канала %s лишены своего статуса.
CHAN_CLEARED_HOPS
	Все хопы (+h) канала %s лишены своего статуса.
CHAN_CLEARED_VOICES
	Все войсы (+v) канала %s лишены своего статуса.
CHAN_CLEARED_USERS
	Все пользователи канала %s кикнуты.
CHAN_CLEARED_INVITES
	Все исключения на приглашение канала %s удалены.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS #канал
CHAN_GETPASS_UNAVAILABLE
	Команда GETPASS недоступна, так как используется шифрование паролей.
CHAN_GETPASS_PASSWORD_IS
	Пароль канала %s - %s.

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
	Команда SENDPASS недоступна, так как используется шифрование паролей.
CHAN_SENDPASS_SUBJECT
	Пароль от канала %s
CHAN_SENDPASS_HEAD
	Доброго времени суток, многоуважаемый/ая!
CHAN_SENDPASS_LINE_1
	Был получен запрос на восстановление пароля от вашего канала %s
CHAN_SENDPASS_LINE_2
	Текущий пароль: %s * Для большей безопасности, рекомендуется изменить его как можно скорее.
CHAN_SENDPASS_LINE_3
	Если вы не запрашивали это письмо, просто проигнорируйте его.
CHAN_SENDPASS_LINE_4
	ПОЖАЛУЙСТА, НЕ ОТВЕЧАЙТЕ НА ЭТО ПИСЬМО!
CHAN_SENDPASS_LINE_5
	Администрация сети %s.
CHAN_SENDPASS_OK
	Пароль от канала %s был выслан на email-адрес его владельца.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID #канал [причина]
CHAN_FORBID_SYNTAX_REASON
	FORBID #канал причина
CHAN_FORBID_SUCCEEDED
	С этого момента канал %s запрещен для использования.
CHAN_FORBID_FAILED
	Извините, установить запрет на канал %s неудалось.
CHAN_FORBID_REASON
	Использовать этот канал запрещено администрацией сети.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND #канал [причина]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND #канал причина
CHAN_SUSPEND_SUCCEEDED
	Активность канала %s приостановлена.
CHAN_SUSPEND_FAILED
	Извините, установить режим саспенда на канал %s неудалось.
CHAN_SUSPEND_REASON
	Активность данного канала была приостановлена администрацией сети.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND #канал
CHAN_UNSUSPEND_ERROR
	Вы не указали символ '#' перед названием канала.
CHAN_UNSUSPEND_SUCCEEDED
	Канал %s успешно восстановлен из режима саспенда.
CHAN_UNSUSPEND_FAILED
	Извините, восстановить активность канала %s неудалось.

# Misc responses
CHAN_EXCEPTED
	Одна из масок в списке исключений канала совпадает с хостмаской пользователя %s. Что бы забанить его, вы должны удалить исключения.

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
	Напишите %R%s LIST NEW чтобы посмотреть список сообщений.
MEMO_AT_LIMIT
	Внимание: вы достигли максимально допустимого числа хранимых сообщений (%d). Вы не сможете принимать новые сообщения, пока не удалите лишние.
MEMO_OVER_LIMIT
	Внимание: вы превысили лимит максимально допустимого числа хранимых сообщений (%d). Вы не сможете принимать новые сообщения, пока вы не удалите лишние.
MEMO_X_MANY_NOTICE
	%d сообщений на канале %s.
MEMO_X_ONE_NOTICE
	%d сообщение на канале %s.
MEMO_NEW_X_MEMO_ARRIVED
	Для канала %s пришло новое сообщение.
	Напишите %R%s READ %s %d чтобы прочитать его.
MEMO_NEW_MEMO_ARRIVED
	Вам пришло новое сообщение от %s.
	Напишите %R%s READ %d чтобы прочитать его.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Ваш список мемо-сообщений пуст.
MEMO_X_HAS_NO_MEMOS
	Список сообщений канала %s пуст.
MEMO_DOES_NOT_EXIST
	Сообщения под номером %d не существует.
MEMO_LIST_NOT_FOUND
	Совпадающих сообщений не обнаружено.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {ник | #канал} текст_сообщения
MEMO_SEND_DISABLED
	Извините, но возможность отправки сообщений временно недоступна.
MEMO_SEND_PLEASE_WAIT
	Пожалуйста, подождите %d секунд перед повторным использованием команды SEND.
MEMO_X_GETS_NO_MEMOS
	Адресат %s не может принимать сообщения.
MEMO_X_HAS_TOO_MANY_MEMOS
	Список сообщений для %s переполнен и принимать на данный момент новые он не может.
MEMO_SENT
	Мемо-сообщение для %s успешно отправлено.
MEMO_MASS_SENT
	Массовое сообщение успешно отправлено всем зарегистрированным пользователям.
	
# STAFF responses
MEMO_STAFF_SYNTAX
	STAFF memo-text

# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {ник | #канал}
MEMO_CANCEL_DISABLED
	Извините, отмена сообщений временно недоступна.
MEMO_CANCEL_NONE
	Нет сообщений доступных для отмены.
MEMO_CANCELLED
	Последнее ваше сообщение адресату %s успешно отменено.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [#канал] [список_записей | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	У вас нет новых сообщений.
MEMO_X_HAS_NO_NEW_MEMOS
	Новых сообщений для %s не обнаружено.
MEMO_LIST_MEMOS
	Список сообщений для %s. Чтобы прочесть их, напишите: %R%s READ номер
MEMO_LIST_NEW_MEMOS
	Новые сообщения для %s. Чтобы прочесть их, напишите: %R%s READ номер
MEMO_LIST_CHAN_MEMOS
	Сообщения канала %s. Чтобы прочесть их, напишите: %R%s READ %s номер
MEMO_LIST_CHAN_NEW_MEMOS
	Новые сообщения на канале %s. Чтобы прочесть их, напишите: %R%s READ %s номер
MEMO_LIST_HEADER
	 No.  Послал            Дата/Время
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

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
	DEL [#канал] {номер_сообщения | список_записей | ALL}
MEMO_DELETED_NONE
	Нет сообщений для удаления.
MEMO_DELETED_ONE
	Сообщение под номером %d было удалено.
MEMO_DELETED_SEVERAL
	Сообщения под номерами %s были удалены.
MEMO_DELETED_ALL
	Все ваши вообщения были удалены.
MEMO_CHAN_DELETED_ALL
	Все сообщения канала %s были удалены.

# SET responses
MEMO_SET_SYNTAX
	SET опция параметр
MEMO_SET_DISABLED
	Извините, но настройка опций службы сообщений временно невозможна.
MEMO_SET_UNKNOWN_OPTION
	Неизвестная SET-опция %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
MEMO_SET_NOTIFY_ON
	С этого момента, %s будет уведомлять вас о новых сообщениях сразу после идентификации к нику или при поступлении новых сообщений.
MEMO_SET_NOTIFY_LOGON
	С этого момента, %s будет уведомлять вас о новых сообщениях сразу после идентификации к нику или по возвращению из режима /AWAY.
MEMO_SET_NOTIFY_NEW
	С этого момента, %s будет уведомлять вас о новых сообщениях сразу по поступлении оных.
MEMO_SET_NOTIFY_OFF
	С этого момента, %s не будет уведомлять вас о новых сообщениях.
MEMO_SET_NOTIFY_MAIL
	С этого момента, вы будете получать уведомления о новых сообщениях по email.
MEMO_SET_NOTIFY_NOMAIL
	Оповещение о новых сообщениях посредством email отключено
MEMO_SET_NOTIFY_INVALIDMAIL
	Для вашего ника не установлен email-адрес.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [#канал] предел
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [ник | #канал] {предел | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Вам запрещено изменять свой лимит сообщений.
MEMO_SET_LIMIT_FORBIDDEN
	Лимит сообщений для %s не может быть изменен.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Вы не можете установить лимит сообщений больше чем %d.
MEMO_SET_LIMIT_TOO_HIGH
	Вы не можете установить лимит сообщений для %s больше чем %d.
MEMO_SET_LIMIT_OVERFLOW
	Указанное значение лимита чересчур велико и будет принудительно изменено до %d.
MEMO_SET_YOUR_LIMIT
	Лимит сообщений был установлен на %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Лимит сообщений установлен на 0, возможность их получения отключена.
MEMO_UNSET_YOUR_LIMIT
	Лимититирование вашего списка сообщений отключено.
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
	На данный момент у вас нет сообщений.
MEMO_INFO_MEMO
	На данный момент у вас имеется 1 сообщение.
MEMO_INFO_MEMO_UNREAD
	На данный момент у вас имеется 1 новое, непрочитанное сообщение.
MEMO_INFO_MEMOS
	На данный момент у вас имеется %d сообщений.
MEMO_INFO_MEMOS_ONE_UNREAD
	На данный момент у вас имеется %d сообщений, 1 из них не прочитано.
MEMO_INFO_MEMOS_SOME_UNREAD
	На данный момент у вас имеется %d сообщений, %d из них не прочитано.
MEMO_INFO_MEMOS_ALL_UNREAD
	На данный момент у вас имеется %d сообщений, ни одно из них не прочитано.
MEMO_INFO_LIMIT
	Ваш лимит сообщений равен %d.
MEMO_INFO_HARD_LIMIT
	Ваш лимит сообщений равен %d и не может быть изменен.
MEMO_INFO_LIMIT_ZERO
	Ваш лимит сообщений равен 0 и принимать их вы не можете.
MEMO_INFO_HARD_LIMIT_ZERO
	Ваш лимит сообщений равен 0, у вас нет возможности принимать сообщения и/или изменять лимит.
MEMO_INFO_NO_LIMIT
	У вас нет ограничения на количество хранимых сообщений.
MEMO_INFO_NOTIFY_OFF
	Уведомление о новых сообщениях отключено.
MEMO_INFO_NOTIFY_ON
	Вы будете уведомлены о новых сообщениях сразу после идентификации к нику или при поступлении новых сообщений.
MEMO_INFO_NOTIFY_RECEIVE
	Вы будете уведомлены о новых сообщениях сразу при поступлении оных.
MEMO_INFO_NOTIFY_SIGNON
	Вы будете уведомлены о новых сообщениях сразу после идентификации к нику
MEMO_INFO_X_NO_MEMOS
	Список сообщений для %s на данный момент пуст.
MEMO_INFO_X_MEMO
	%s сейчас имеет 1 сообщение.
MEMO_INFO_X_MEMO_UNREAD
	%s сейчас имеет 1 новое, непрочитанное сообщение.
MEMO_INFO_X_MEMOS
	%s сейчас имеет %d сообщений.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s сейчас имеет %d сообщений, 1 из них все еще не прочитано.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s сейчас имеет %d сообщений, %d из них все еще не прочитаны.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s сейчас имеет %d сообщений, ни одно из них не прочитано.
MEMO_INFO_X_LIMIT
	Лимит сообщений для %s равен %d.
MEMO_INFO_X_HARD_LIMIT
	Лимит собщений для %s равен %d и не может быть изменен.
MEMO_INFO_X_NO_LIMIT
	Лимит сообщений для %s отключен.
MEMO_INFO_X_NOTIFY_OFF
	%s не уведомляется о новых собщениях.
MEMO_INFO_X_NOTIFY_ON
	%s уведомляется о новых сообщениях сразу после идентификации или по мере прихода новых собщений.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s уведомляется о новых сообщениях сразу при поступлении оных.
MEMO_INFO_X_NOTIFY_SIGNON
	%s уведомляется о новых сообщениях сразу после идентификации к нику.

# Memo2Mail responses
MEMO_MAIL_SUBJECT
	Новое мемо-сообщение
MEMO_MAIL_TEXT1
	Здраствуйте %s.
MEMO_MAIL_TEXT2
	Вы получили новое мемо-сообщение от %s. Номер сообщения: %d.
MEMO_MAIL_TEXT3
	Текст сообщения:
	
# RSEND responses
MEMO_RSEND_PLEASE_WAIT
	Пожалуйста, подождите %d секунд перед повторным использованием команды RSEND.
MEMO_RSEND_DISABLED
	Извините, но возможность использования команды RSEND в вашей IRC-сети недоступна.
MEMO_RSEND_SYNTAX
	RSEND {ник | #канал} текст
MEMO_RSEND_NICK_MEMO_TEXT
	[авто-сообщение] Мемо-сообщение, которые вы посылали, было прочитано.
MEMO_RSEND_CHAN_MEMO_TEXT
	[авто-сообщение] Мемо-сообщение, которые вы посылали %s, было прочитано.
MEMO_RSEND_USER_NOTIFICATION
	Мемо-извещение было автоматически послано для %s, информирующее отправителя о том, что вы прочитали его/ее сообщение.

# CHECK responses
MEMO_CHECK_SYNTAX
	CHECK ник
MEMO_CHECK_NOT_READ
	Последнее сообщение, которое вы посылали для %s (а было это %s) еще не прочитано.
MEMO_CHECK_READ
	Последнее сообщение, которое вы посылали для %s (а было это %s) уже прочитано.
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
	Бота %s не существует.
BOT_NOT_ASSIGNED
	Перед тем, как использовать эту команду, вы должны назначить бота на
	канал. Для более подробной информации см.  %R%S HELP ASSIGN.
BOT_NOT_ON_CHANNEL
	На данный момент, бот не присутствует на канале %s.

# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	Слово %s запрещено на этом канале!
BOT_REASON_BADWORD_GENTLE
	Следи за своим языком!
BOT_REASON_BOLD
	Использование жирного шрифта у нас тут не приветствуется!
BOT_REASON_CAPS
	Выключи CapsLOCK!
BOT_REASON_COLOR
	Использование цветов у нас тут не приветствуется!
BOT_REASON_FLOOD
	Хватит флудить!
BOT_REASON_REPEAT
	Хватит повторов!
BOT_REASON_REVERSE
	Использование инверсии у нас тут не приветствуется!
BOT_REASON_UNDERLINE
	Использование подчеркиваний у нас тут не приветствуется!

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
	BOT CHANGE старый_ник новый_ник [идент [хост [реальное_имя]]]
	BOT DEL ник
BOT_BOT_ALREADY_EXISTS
	Бот с ником %s уже существует.
BOT_BOT_CREATION_FAILED
	Извините, но создать бота неудалось.
BOT_BOT_READONLY
	Извините, изменение настроек ботов временно недоступно.
BOT_BOT_ADDED
	%s!%s@%s (%s) добавлен в список ботов.
BOT_BOT_ANY_CHANGES
	Старая информация о боте не отличается от новой.
BOT_BOT_CHANGED
	Данные о боте %s были изменены на %s!%s@%s (%s)
BOT_BOT_DELETED
	Бот под ником %s успешно удален.

# BOTLIST responses
BOT_BOTLIST_HEADER
	Список ботов:
BOT_BOTLIST_PRIVATE_HEADER
	Боты, зарезервированные для IRC-операторов:
BOT_BOTLIST_FOOTER
	Количество доступных ботов: %d
BOT_BOTLIST_EMPTY
	Список доступных ботов пуст.
	По вопросам создания ботов обращайтесь к Администраторам сервисов.

# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN #канал ник
BOT_ASSIGN_READONLY
	Извините, назначение ботов на канал временно недоступно.
BOT_ASSIGN_ALREADY
	Бот %s уже назначен на канал %s.
BOT_ASSIGN_ASSIGNED
	Бот %s успешно назначен на канал %s.

# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN #канал
BOT_UNASSIGN_UNASSIGNED
	Бот с канала %s успешно удален.

# INFO responses
BOT_INFO_SYNTAX
	INFO {#канал | ник}
BOT_INFO_NOT_FOUND
	Указанный параметр %s не является ником бота или зарегистрированным каналом.
BOT_INFO_BOT_HEADER
	Информация о боте %s:
BOT_INFO_BOT_MASK
	Маска...........: %s@%s
BOT_INFO_BOT_REALNAME
	Реальное имя....: %s
BOT_INFO_BOT_CREATED
	Создан..........: %s
BOT_INFO_BOT_USAGE
	Кол-во каналов..: %d
BOT_INFO_BOT_OPTIONS
	Опции...........: %s
BOT_INFO_OPT_PRIVATE
	Приватный
BOT_INFO_CHAN_HEADER
	Информация о канале %s:
BOT_INFO_CHAN_BOT
	Ник бота.............: %s
BOT_INFO_CHAN_BOT_NONE
	Ник бота.............: бот не назначен.
BOT_INFO_CHAN_KICK_BADWORDS
	Цензор плохих слов...: %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	Цензор плохих слов...: %s (%d киков до бана)
BOT_INFO_CHAN_KICK_BOLDS
	Цензор жирных букв...: %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	Цензор жирных букв...: %s (%d киков до бана)
BOT_INFO_CHAN_KICK_CAPS_ON
	Цензор CapsLOCK......: %s (минимум %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	Цензор CapsLOCK......: %s (%d киков до бана; минимум %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	Цензор CapsLOCK......: %s
BOT_INFO_CHAN_KICK_COLORS
	Цензор цветов........: %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	Цензор цветов........: %s (%d киков до бана)
BOT_INFO_CHAN_KICK_FLOOD_ON
	Цензор флуда.........: %s (%d линий за %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	Цензор флуда.........: %s (%d киков до бана; %d линий из %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	Цензор флуда.........: %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	Цензор повторов......: %s (%d раз)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	Цензор повторов......: %s (%d киков до бана; %d раз)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	Цензор повторов......: %s
BOT_INFO_CHAN_KICK_REVERSES
	Цензор инверсий......: %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	Цензор инверсий......: %s (%d киков до бана)
BOT_INFO_CHAN_KICK_UNDERLINES
	Цензор подчеркиваний.: %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	Цензор подчеркиваний.: %s (%d киков до бана)
BOT_INFO_ACTIVE
	включен
BOT_INFO_INACTIVE
	отключен
BOT_INFO_CHAN_OPTIONS
	Активированные опции.: %s
BOT_INFO_OPT_DONTKICKOPS
	Не кикать операторов
BOT_INFO_OPT_DONTKICKVOICES
	Не кикать войсов
BOT_INFO_OPT_FANTASY
	FANTASY-режим
BOT_INFO_OPT_GREET
	Приветствия
BOT_INFO_OPT_NOBOT
	Без ботов
BOT_INFO_OPT_SYMBIOSIS
	Симбиоз
BOT_INFO_OPT_NONE
	отсутствуют

# SET responses
BOT_SET_SYNTAX
	SET (#канал | бот) опция параметр
BOT_SET_DISABLED
	Извините, но возможность настройки опций бота временно недоступна.
BOT_SET_UNKNOWN
	Неизвестная опция %s.
	Напишите %R%S HELP SET для более подробной информации.

# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET #канал DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Теперь, операторы канала %s будут исключены из условий кика за нарушения.
BOT_SET_DONTKICKOPS_OFF
	С этого момента, бот будет кикать операторов канала %s за нарушения.

# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET #канал DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Теперь, войсы канала %s будут исключены из условий кика за нарушения.
BOT_SET_DONTKICKVOICES_OFF
	С этого момента бот будет кикать войсов канала %s за нарушения.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET #канал FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	FANTASY-режим для канала %s активирован.
BOT_SET_FANTASY_OFF
	FANTASY-режим для канала %s отключен.

# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET #канал GREET {ON|OFF}
BOT_SET_GREET_ON
	Режим приветствий для канала %s активирован.
BOT_SET_GREET_OFF
	Режим приветствий для канала %s отключен.

# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET #канал NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Режим без-бота для канала %s активирован.
BOT_SET_NOBOT_OFF
	Режим без-бота для канала %s отключен.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET ник_бота PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Бот под ником %s успешно помечен как приватный.
BOT_SET_PRIVATE_OFF
	Режим приватный успешно снят с бота под ником %s

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET #канал SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Режим симбиоза для канале %s активирован.
BOT_SET_SYMBIOSIS_OFF
	Режим симбиоза для канале %s отключен.

# KICK responses
BOT_KICK_SYNTAX
	KICK #канал опция {ON|OFF} [параметры]
BOT_KICK_DISABLED
	Извините, но возможность конфигурации условий киков временно недоступна.
BOT_KICK_UNKNOWN
	Неизвестная опция %s.
	Используйте %R%S HELP KICK для получения более полной информации.
BOT_KICK_BAD_TTB
	Значение вида %s не может быть использовано как кол-во киков до бана.

BOT_KICK_BADWORDS_ON
	Режим кика за плохие слова активирован.
	Для модификации списка "плохих" слов воспользуйтесь командой BADWORDS.
BOT_KICK_BADWORDS_ON_BAN
	C этого момента, бот будет кикать за "плохие" слова и ставить бан
	нарушителю после %d кика(ов). Для модификации списка "плохих" слов
	воспользуйтесь командой BADWORDS.
BOT_KICK_BADWORDS_OFF
	Режим кика за плохие слова отключен.
BOT_KICK_BOLDS_ON
	Режим кика за использование жирного шрифта активирован.
BOT_KICK_BOLDS_ON_BAN
	Режим кика за использование жирного шрифта активирован.
	Дополнительно, после %d предупреждающих киков, бот забанит нарушителя.
BOT_KICK_BOLDS_OFF
	Режим кика за использование жирного шрифта отключен.
BOT_KICK_CAPS_ON
	Режим кика за чрезмерное использование CapsLOCK активирован.  Бот
	будет кикать посетителя, если его фраза содержит не менее %d больших
	символов и они составляют не менее %d%% от общего кол-ва символов.
BOT_KICK_CAPS_ON_BAN
	Режим кика за чрезмерное использование CapsLOCK активирован.  Бот
	будет кикать посетителя, если его фраза содержит не менее %d больших
	символов и они составляют не менее %d%% от общего кол-ва символов.
	Дополнительно, после %d предупреждающих киков, бот забанит нарушителя.
BOT_KICK_CAPS_OFF
	Режим кика за чрезмерное использование CapsLOCK отключен.
BOT_KICK_COLORS_ON
	Режим кика за использование цветов активирован.
BOT_KICK_COLORS_ON_BAN
	Режим кика за использование цветов активирован.
	Дополнительно, после %d предупреждающих киков, бот забанит нарушителя.
BOT_KICK_COLORS_OFF
	Режим кика за использование цветов отключен.
BOT_KICK_FLOOD_ON
	Режим кика за флуд активирован (условие: %d линий за %d секунд).
BOT_KICK_FLOOD_ON_BAN
	Режим кика за флуд активирован (условие: %d линий за %d секунд).
	Дополнительно, после %d предупреждающих киков, бот забанит нарушителя.
BOT_KICK_FLOOD_OFF
	Режим кика за флуд отключен.
BOT_KICK_REPEAT_ON
	Режим кика за чрезмерные повторы активирован.  Бот будет кикать
	посетителя если он повторит одну и ту же фразу %d раз подряд.
BOT_KICK_REPEAT_ON_BAN
	Режим кика за чрезмерные повторы активирован.
	Бот будет кикать пользователя после %d повторов подряд и ставить
	бан после %d предупреждающих киков.
BOT_KICK_REPEAT_OFF
	Режим кика за чрезмерные повторы отключен.
BOT_KICK_REVERSES_ON
	Режим кика за использование инверсного текста активирован.
BOT_KICK_REVERSES_ON_BAN
	Режим кика за использование инверсного текста активирован.
	Дополнительно, после %d предупреждающих киков, бот забанит нарушителя.
BOT_KICK_REVERSES_OFF
	Режим кика за использование инверсного текста отключен.
BOT_KICK_UNDERLINES_ON
	Режим кика за использование подчеркнутого текста активирован.
BOT_KICK_UNDERLINES_ON_BAN
	Режим кика за использование подчеркнутого текста активирован.
	Дополнительно, после %d предупреждающих киков, бот забанит нарушителя.
BOT_KICK_UNDERLINES_OFF
	Режим кика за использование подчеркнутого текста отключен.

# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS #канал {ADD|DEL|LIST|CLEAR} [слово|номер_записи] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	Извините, изменение базы данных плохих слов временно невозможно.
BOT_BADWORDS_REACHED_LIMIT
	Размер базы данных плохих слов не может превышать %d записей.
BOT_BADWORDS_ALREADY_EXISTS
	Запись вида %s уже содержится в списке плохих слов канала %s.
BOT_BADWORDS_ADDED
	Запись вида %s успешно добавлена в список плохих слов канала %s.
BOT_BADWORDS_NO_SUCH_ENTRY
	Записи под номером #%d в списке плохих слов канала %s не существует.
BOT_BADWORDS_NOT_FOUND
	Запись вида %s в списке плохих слов канала %s не обнаружена.
BOT_BADWORDS_NO_MATCH
	В списке плохих слов канала %s совпадающих записей не обнаружено.
BOT_BADWORDS_DELETED
	Запись вида %s успешно удалена из списка плохих слов канала %s.
BOT_BADWORDS_DELETED_ONE
	Удалена 1 запись из списка плохих слов канала %s.
BOT_BADWORDS_DELETED_SEVERAL
	Удалено %d записей из списка плохих слов канала %s.
BOT_BADWORDS_LIST_EMPTY
	Список плохих слов канала %s пуст.
BOT_BADWORDS_LIST_HEADER
	Список плохих слов для %s:
	  No.   Слово                          Тип
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	Список "плохих" слов полностью очищен.

# SAY responses
BOT_SAY_SYNTAX
	SAY #канал текст

# ACT responses
BOT_ACT_SYNTAX
	ACT #канал текст

# Errors
BOT_EXCEPT
	Одна из масок в списке исключений канала совпадает с хостмаской указанного пользователя. Чтобы забанить его, вы должны удалить исключения.
BOT_BAD_NICK
	Указанный для бота ник содержит недопустимые символы.
BOT_BAD_HOST
	Указанный для бота хост содержит недопустимые символы.
BOT_BAD_IDENT
	Указанный для бота идент содержит недопустимые символы.
BOT_LONG_IDENT
	Идент бота не должен превышать %d символов.
BOT_LONG_HOST
	Хост бота не должен превышать %d символов.

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Возможность изменять режимы сервисам недоступна. Вы вообще уверены, что настройки ваших серверов корректны?
OPER_BOUNCY_MODES_U_LINE
	Возможность изменять режимы сервисам недоступна. Вы вообще уверены, что U:line-записи ваших серверов корректны?
# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL сообщение

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Неизвестный STATS-параметр %s.
OPER_STATS_CURRENT_USERS
	Текущее кол-во пользователей: %d, IRC-операторов: %d 
OPER_STATS_MAX_USERS
	Зафиксированный максимум пользователей: %d (на %s)
# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Сервисы работают: %d дней, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Сервисы работают: %d день, %02d:%02d
OPER_STATS_UPTIME_HMS
	Сервисы работают: %d часов, %d минут
OPER_STATS_UPTIME_HM1S
	Сервисы работают: %d часов, %d минут
OPER_STATS_UPTIME_H1MS
	Сервисы работают: %d час, %d минут
OPER_STATS_UPTIME_H1M1S
	Сервисы работают: %d часов, %d минуту
OPER_STATS_UPTIME_1HMS
	Сервисы работают: %d час, %d минут
OPER_STATS_UPTIME_1HM1S
	Сервисы работают: %d час, %d минут
OPER_STATS_UPTIME_1H1MS
	Сервисы работают: %d час, %d минуту
OPER_STATS_UPTIME_1H1M1S
	Сервисы работают: %d час, %d минуту
OPER_STATS_UPTIME_MS
	Сервисы работают: %d минут, %d секунд
OPER_STATS_UPTIME_M1S
	Сервисы работают: %d минут, %d секунду
OPER_STATS_UPTIME_1MS
	Сервисы работают: %d минуту, %d секунд
OPER_STATS_UPTIME_1M1S
	Сервисы работают: %d минуту, %d секунду
OPER_STATS_BYTES_READ
	Байт считано    : %5d kB
OPER_STATS_BYTES_WRITTEN
	Байт записано   : %5d kB
OPER_STATS_USER_MEM
	Пользователей   : %6d записей, %5d kB
OPER_STATS_CHANNEL_MEM
	Каналов         : %6d записей, %5d kB
OPER_STATS_GROUPS_MEM
	NS-групп        : %6d записей, %5d kB
OPER_STATS_ALIASES_MEM
	NS-алиасов      : %6d записей, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ        : %6d записей, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ         : %6d записей, %5d kB
OPER_STATS_HOSTSERV_MEM
	HostServ        : %6d записей, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ        : %6d записей, %5d kB
OPER_STATS_SESSIONS_MEM
	Сессий          : %6d записей, %5d kB
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
	Время истечения AKILL'а по-умолчанию  : никогда
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
	Время истечения SGLINE'а по-умолчанию : никогда
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
	Время истечения SQGLINE'а по-умолчанию: никогда
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
	Время истечения SZLINE'а по-умолчанию : никогда
OPER_STATS_RESET
	Статистические данные сброшены.
OPER_STATS_UPLINK_SERVER
	Uplink-сервер: %s
OPER_STATS_UPLINK_CAPAB
	Uplink capab: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Текущее кол-во серверов: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE #канал режимы

# UMODE respondes
OPER_UMODE_SYNTAX
	UMODE ник режимы
OPER_UMODE_SUCCESS
	Режимы пользователя %s изменены.
OPER_UMODE_CHANGED
	%s принудительно изменил ваши пользовательские режимы.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE ник +флаги
OPER_OLINE_SUCCESS
	OLINE-флаги %s успешно установлены для сессий пользователя %s
OPER_OLINE_IRCOP
	Теперь вы IRC-оператор.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES #канал [ALL]
OPER_CLEARMODES_DONE
	Все режимы и баны канала %s были сброшены.
OPER_CLEARMODES_ALL_DONE
	Все режимы канала %s были сброшены.

# KICK responses
OPER_KICK_SYNTAX
	KICK #канал ник причина

# SVSNICK responses
OPER_SVSNICK_SYNTAX
	SVSNICK ник новый_ник 
OPER_SVSNICK_NEWNICK
	Ник пользователя %s был изменен на %s.

# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [ник | номер_записи]
OPER_ADMIN_SKELETON
	Сервисы работают в skeleton-режиме, команда ADMIN недоступна.
OPER_ADMIN_EXISTS
	Ник %s уже присутствует в списке Администраторов сервисов.
OPER_ADMIN_REACHED_LIMIT
	Количество записей в списке Администраторов сервисов не может превышать %d.
OPER_ADMIN_ADDED
	Пользователь %s успешно добавлен в список Администраторов сервисов.
OPER_ADMIN_NOT_FOUND
	Пользователь %s в списке Администраторов сервисов не обнаружен.
OPER_ADMIN_NO_MATCH
	Совпадающих записей не обнаружено.
OPER_ADMIN_DELETED
	Пользователь %s успешно удален из списка Администраторов сервисов.
OPER_ADMIN_DELETED_ONE
	Удалена 1 запись из списка Администраторов сервисов.
OPER_ADMIN_DELETED_SEVERAL
	Удалено %d записей из списка Администраторов сервисов.
OPER_ADMIN_LIST_EMPTY
	Список Администраторов сервисов пуст.
OPER_ADMIN_LIST_HEADER
	Список Администраторов сервисов:
	  No.   Ник
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	Список Администраторов сервисов полностью очищен.
OPER_ADMIN_MOVED
	Пользователь %s успешно перемещен в список Администраторов сервисов.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [ник | номер_записи]
OPER_OPER_SKELETON
	Сервисы работают в skeleton-режиме, команда OPER недоступна.
OPER_OPER_EXISTS
	Ник %s уже присутствует в списке Операторов сервисов.
OPER_OPER_REACHED_LIMIT
	Количество записей в списке Операторов сервисов не может превышать %d.
OPER_OPER_ADDED
	Пользователь %s успешно добавлен в список Операторов сервисов.
OPER_OPER_NOT_FOUND
	Пользователь %s в списке Операторов сервисов не обнаружен.
OPER_OPER_NO_MATCH
	Совпадающих записей не обнаружено.
OPER_OPER_DELETED
	Пользователь %s успешно удален из списка Операторов сервисов.
OPER_OPER_DELETED_ONE
	Удалена 1 запись из списка Операторов сервисов.
OPER_OPER_DELETED_SEVERAL
	Удалено %d записей из списка Операторов сервисов.
OPER_OPER_LIST_EMPTY
	Список Операторов сервисов пуст.
OPER_OPER_LIST_HEADER
	Список Операторов сервисов:
	  No.   Ник
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	Список Операторов сервисов полностью очищен.
OPER_OPER_MOVED
	%s был перемещен в список Операторов Сервисов.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD|DEL|LIST|VIEW|CLEAR} [[+время_истечения] {маска | номер_записи} [причина]]
OPER_AKILL_EXISTS
	Запись вида %s уже присутствует в списке AKILL'ов.
OPER_AKILL_ALREADY_COVERED
	Указанная маска вида %s совпадает с одной из масок в списке AKILL'ов: %s.
OPER_AKILL_REACHED_LIMIT
	Количество записей в AKILL-списке не может превышать %d.
OPER_AKILL_NO_NICK
	Помните: AKILL-запись базируется на маске вида идент@хост, использование ника в качестве маски недопустимо!
OPER_AKILL_ADDED
	Запись вида %s успешно добавлена в список AKILL'ов.
OPER_AKILL_CHANGED
	Срок истечения записи %s успешно изменен.
OPER_AKILL_NOT_FOUND
	Запись вида %s в списке AKILL'ов не обнаружена.
OPER_AKILL_NO_MATCH
	Совпадающих записей не обнаружено.
OPER_AKILL_DELETED
	Запись вида %s успешно удалена из списка AKILL'ов.
OPER_AKILL_DELETED_ONE
	Удалена 1 запись из спика AKILL'ов.
OPER_AKILL_DELETED_SEVERAL
	Удалено %d записей из списка AKILL'ов.
OPER_AKILL_LIST_EMPTY
	Список AKILL'ов пуст.
OPER_AKILL_LIST_HEADER
	Текущий список AKILL'ов:
	  No.   Маска                             Причина
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Текущий список AKILL'ов:
# номер, маска, создатель, когда, истекает, причина
OPER_AKILL_VIEW_FORMAT
	%3d  %s (от %s в %s; %s)
	      %s
OPER_AKILL_CLEAR
	Список AKILL'ов полностью очищен.
OPER_CHANKILL_SYNTAX
	CHANKILL [+срок_истечения] {#канал} [причина]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD|DEL|LIST|VIEW|CLEAR} [[+срок_истечения] {маска | номер_записи}[:причина]]
OPER_SGLINE_UNSUPPORTED
	SGLINE-возможности сервисов в вашей сети недоступны.
OPER_SGLINE_EXISTS
	Запись вида %s уже содержится в списке SGLINE'ов.
OPER_SGLINE_ALREADY_COVERED
	Указанная маска вида %s совпадает с одной из масок в списке SGLINE'ов: %s.
OPER_SGLINE_REACHED_LIMIT
	Количество записей в SGLINE-списке не может превышать %d.
OPER_SGLINE_ADDED
	Запись вида %s успешно добавлена в список SGLINE'ов.
OPER_SGLINE_CHANGED
	Срок истечения записи %s успешно изменен.
OPER_SGLINE_NOT_FOUND
	Запись вида %s в списке SGLINE'ов не обнаружена.
OPER_SGLINE_NO_MATCH
	Совпадающих записей не обнаружено.
OPER_SGLINE_DELETED
	Запись вида %s успешно удалена из списка SGLINE'ов.
OPER_SGLINE_DELETED_ONE
	Удалена 1 запись из спика SGLINE'ов.
OPER_SGLINE_DELETED_SEVERAL
	Удалено %d записей из списка SGLINE'ов.
OPER_SGLINE_LIST_EMPTY
	Список SGLINE'ов пуст.
OPER_SGLINE_LIST_HEADER
	Текущий список SGLINE'ов:
	  No.   Маска                             Причина
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Текущий список SGLINE'ов:
# номер, маска, создатель, когда, истекает, причина
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (от %s в %s; %s)
	      %s
OPER_SGLINE_CLEAR
	Список SGLINE'ов полностью очищен.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD|DEL|LIST|VIEW|CLEAR} [[+срок_истечения] {маска | номер_записи} [причина]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	SQLINE-возможности сервисов в вашей сети недоступны.
OPER_SQLINE_EXISTS
	Запись вида %s уже содержится в списке SQLINE'ов.
OPER_SQLINE_ALREADY_COVERED
	Указанная маска вида %s совпадает с одной из масок в списке SQLINE'ов: %s.
OPER_SQLINE_REACHED_LIMIT
	Количество записей в SQLINE-списке не может превышать %d.
OPER_SQLINE_ADDED
	Запись вида %s успешно добавлена в список SQLINE'ов.
OPER_SQLINE_CHANGED
	Срок истечения записи %s успешно изменен.
OPER_SQLINE_NOT_FOUND
	Запись вида %s в списке SQLINE'ов не обнаружена.
OPER_SQLINE_NO_MATCH
	Совпадающих записей не обнаружено.
OPER_SQLINE_DELETED
	Запись вида %s успешно удалена из списка SQLINE'ов.
OPER_SQLINE_DELETED_ONE
	Удалена 1 запись из спика SQLINE'ов.
OPER_SQLINE_DELETED_SEVERAL
	Удалено %d записей из списка SQLINE'ов.
OPER_SQLINE_LIST_EMPTY
	Список SQLINE'ов пуст.
OPER_SQLINE_LIST_HEADER
	Текущий список SQLINE'ов:
	  No.   Маска                             Причина
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Текущий список SQLINE'ов:
# номер, маска, создатель, когда, истекает, причина
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (от %s в %s; %s)
	      %s
OPER_SQLINE_CLEAR
	Список SQLINE'ов полностью очищен.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD|DEL|LIST|VIEW|CLEAR} [[+срок_истечения] {маска | номер_записи} [причина]]
OPER_SZLINE_UNSUPPORTED
	SZLINE-возможности сервисов в вашей сети недоступны.
OPER_SZLINE_EXISTS
	Запись вида %s уже содержится в списке SZLINE'ов.
OPER_SZLINE_ALREADY_COVERED
	Указанная маска вида %s совпадает с одной из масок в списке SZLINE'ов: %s.
OPER_SZLINE_REACHED_LIMIT
	Количество записей в SZLINE-списке не может превышать %d.
OPER_SZLINE_ONLY_IPS
	Помните: список SZLINE'ов может содержать только IP-маски!
OPER_SZLINE_ADDED
	Запись вида %s успешно добавлена в список SZLINE'ов.
OPER_SZLINE_CHANGED
	Срок истечения записи %s успешно изменен.
OPER_SZLINE_NOT_FOUND
	Запись вида %s в списке SZLINE'ов не обнаружена.
OPER_SZLINE_NO_MATCH
	Совпадающих записей не обнаружено.
OPER_SZLINE_DELETED
	Запись вида %s успешно удалена из списка SZLINE'ов.
OPER_SZLINE_DELETED_ONE
	Удалена 1 запись из спика SZLINE'ов.
OPER_SZLINE_DELETED_SEVERAL
	Удалено %d записей из списка SZLINE'ов.
OPER_SZLINE_LIST_EMPTY
	Список SZLINE'ов пуст.
OPER_SZLINE_LIST_HEADER
	Текущий список SZLINE'ов:
	  No.   Маска                             Причина
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Текущий список SZLINE'ов:
# номер, маска, создатель, когда, истекает, причина
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (от %s в %s; %s)
	      %s
OPER_SZLINE_CLEAR
	Список SZLINE'ов полностью очищен.

# SET responses
OPER_SET_SYNTAX
	SET опция параметры
OPER_SET_IGNORE_ON
	Режим использования игнор-списка активирован.
OPER_SET_IGNORE_OFF
	Режим использования игнор-списка отключен.
OPER_SET_IGNORE_ERROR
	В качестве параметра для опции IGNORE вы должны указать либо ON, либо OFF.
OPER_SET_READONLY_ON
	Сервисы перешли в режим read-only (только чтение).
OPER_SET_READONLY_OFF
	Сервисы перешли в режим read-write (чтение-запись).
OPER_SET_READONLY_ERROR
	В качестве параметра для опции READONLY вы должны указать либо ON, либо OFF.
OPER_SET_LOGCHAN_ON
	Режим логирования сообщений на сервисный канал %s активирован.
OPER_SET_LOGCHAN_OFF
	Режим логирования сообщений на сервисный канал отключен.
OPER_SET_LOGCHAN_ERROR
	В качестве параметра для опции LOGCHAN вы должны указать либо ON, либо OFF.
OPER_SET_DEBUG_ON
	Режим работы сервисов изменен: активирован DEBUG-режим (отладка).
OPER_SET_DEBUG_OFF
	Режим работы сервисов изменен: DEBUG-режим деактивирован.
OPER_SET_DEBUG_LEVEL
	Режим работы сервисов изменен: активирован DEBUG-режим (уровень %d).
OPER_SET_DEBUG_ERROR
	В качестве параметра для опции DEBUG вы должны указать ON, OFF, либо положительное число.
OPER_SET_NOEXPIRE_ON
	Режим работы сервисов изменен: активирован NOEXPIRE-режим (режим 'не-истечения').
OPER_SET_NOEXPIRE_OFF
	Режим работы сервисов изменен: NOEXPIRE-режим деактивирован.
OPER_SET_NOEXPIRE_ERROR
	В качестве параметра для опции NOEXPIRE вы должны указать либо ON, либо OFF.
OPER_SET_UNKNOWN_OPTION
	Неизвестная опция %s.
OPER_SET_SQL_ON
	Режим работы сервисов изменен: активирован SQL-режим (использование SQL DB).
OPER_SET_SQL_OFF
	Режим работы сервисов изменен: SQL-режим деактивирован.
OPER_SET_SQL_ERROR
	В качестве параметра для опции SQL вы должны указать либо ON, либо OFF.
OPER_SET_SQL_ERROR_INIT
	Ошибка инициализации SQL, обратитесь к логам для более подробной информации.
OPER_SET_SQL_ERROR_DISABLED
	SQL-функции не настроены или настроены некорректно. Внесите необходимые
	изменения в конфигурационный файл сервисов (services.conf).
OPER_SET_LIST_OPTION_ON
	Режим %s: включен.
OPER_SET_LIST_OPTION_OFF
	Режим %s: отключен.

# NOOP responses
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} сервер
OPER_NOOP_SET
	Все OLINE-записи на сервере %s были удалены.
OPER_NOOP_REVOKE
	Все OLINE-записи на сервере %s восстановлены.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE имя_сервера [причина]
OPER_JUPE_HOST_ERROR
	В качестве основного параметра вы должны указать реальное имя серевера 
OPER_JUPE_INVALID_SERVER
	You can not jupe your services server or your uplink server.

# RAW responses
OPER_RAW_SYNTAX
	RAW строка

# UPDATE responses
OPER_UPDATING
	Обновление баз данных...

# RELOAD responses
OPER_RELOAD
	Конфигурационный файл сервисов был считан заново.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN не определен, перезапуск невозможен. Запустите скрипт \2configure\2 и пересоберите сервисы, чтобы активировать команду RESTART.

# IGNORE respondes
OPER_IGNORE_SYNTAX
	IGNORE {ADD|DEL|LIST|CLEAR} [время] [ник]
OPER_IGNORE_VALID_TIME
	Значение параметра "время" указано не верно.
OPER_IGNORE_TIME_DONE
	Ник %s добавлен в список игнорирования, продолжительность игнора: %s.
OPER_IGNORE_PERM_DONE
	Ник %s добавлен в список игнорирования, продолжительность игнора: постоянно.
OPER_IGNORE_DEL_DONE
	Ник %s успешно удален из списка игнорирования.
OPER_IGNORE_LIST
	Список игнорируемых ников:
OPER_IGNORE_LIST_NOMATCH
	Ник %s в списке игнорируемых не обнаружен.
OPER_IGNORE_LIST_EMPTY
	Список игнорируемых ников пуст.
OPER_IGNORE_LIST_CLEARED
	Список игнорируемых ников полностью очищен.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES ник
OPER_KILLCLONES_UNKNOWN_NICK
	Пользователь %s в сети не обнаружен.

# CHANLIST responses
OPER_CHANLIST_HEADER
	Список каналов:
	Имя            Посетителей Режимы  Топик
OPER_CHANLIST_HEADER_USER
	Список каналов пользователя %s:
	Имя            Посетителей Режимы  Топик
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	Конец списка каналов.

# USERLIST responses
OPER_USERLIST_HEADER
	Список пользователей:
	Ник                  Маска
OPER_USERLIST_HEADER_CHAN
	Список пользователей канала %s:
	Ник                  Маска
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Конец списка пользователей.

OPER_SUPER_ADMIN_ON
	Режим Супер-Администратора активирован.
OPER_SUPER_ADMIN_OFF
	Режим Супер-Администратора деактивирован.
OPER_SUPER_ADMIN_SYNTAX
	В качестве параметра для опции SuperAdmin вы должны указать либо ON, либо OFF.
OPER_SUPER_ADMIN_WALL_ON
	Встречаем нового Супер-Администратора - %s
OPER_SUPER_ADMIN_WALL_OFF
	%s снял с себя режим Супер-Администратора.
OPER_SUPER_ADMIN_ONLY
	Только Супер-Администраторы могут использовать эту команду.
OPER_STAFF_LIST_HEADER
	Статус Уровень Ник
OPER_STAFF_FORMAT
	  %c    %s    %s
OPER_STAFF_AFORMAT
	  %c    %s    %s [%s]

# DefCon Messages

OPER_DEFCON_SYNTAX
	DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	На данный момент сервисы работают в DEFCON-режиме. Повторите свой запрос позже.
OPER_DEFCON_NO_CONF
	Активирование DEFCON-режима невозможно, так как он не включен в конфиге сервисов.
OPER_DEFCON_CHANGED
	DEFCON-режим активирован, уровень защиты: %d
OPER_DEFCON_WALL
	%s изменил уровень DEFCON на %d
DEFCON_GLOBAL
	Уровень DEFCON теперь равен %d

# Module strings

OPER_MODULE_LOADED
	Модуль %s успешно загружен.
OPER_MODULE_UNLOADED
	Модуль %s успешно выгружен.
OPER_MODULE_LOAD_FAIL
	Невозможно загрузить модуль %s
OPER_MODULE_REMOVE_FAIL
	Невозможно выгрузить модуль %s
OPER_MODULE_NO_UNLOAD
	Этот модуль не может быть выгружен.
OPER_MODULE_LOAD_SYNTAX
	MODLOAD имя_файла
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD имя_файла
OPER_MODULE_LIST_HEADER
	Текущий список модулей:
OPER_MODULE_LIST
	Модуль: %s [%s] [%s]
OPER_MODULE_LIST_FOOTER
	Количество загруженных модулей: %d
OPER_MODULE_INFO_LIST
	Модуль: %s Версия: %s Автор: %s Загружен: %s
OPER_MODULE_CMD_LIST
	Предоставляемая команда: %R%s %s
OPER_MODULE_MSG_LIST
	Предоставляет IRCD-обработчик для: %s
OPER_MODULE_NO_LIST
	Список модулей пуст.
OPER_MODULE_NO_INFO
	Информация о модуле %s недоступна. 
OPER_MODULE_INFO_SYNTAX
	MODINFO имя_файла
MODULE_HELP_HEADER
	Данный модуль автоматически добавил следующие команды:

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD|DEL|MOVE|LIST|VIEW} [параметры]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+срок_истечения] маска лимит причина
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {маска | список_записей}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE номер позиция
OPER_EXCEPTION_DISABLED
	Ограничение сессий отключено, список исключений не требуется.
OPER_EXCEPTION_ALREADY_PRESENT
	Маска %s уже есть в списке исключений.
OPER_EXCEPTION_TOO_MANY
	Список исключений из лимита сессий переполнен!
OPER_EXCEPTION_ADDED
	Лимит сессий для %s установлен на %d.
OPER_EXCEPTION_MOVED
	Исключение для %s (#%d) перемещено в позицию %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Запись под номером #%d в списке исключений из лимита сессий не обнаружена.
OPER_EXCEPTION_NOT_FOUND
	Запись вида %s в списке исключений из лимита сессий не обнаружена.
OPER_EXCEPTION_NO_MATCH
	Совпадающих значений в списке исключений из лимита сессий не обнаружено.
OPER_EXCEPTION_DELETED
	Запись вида %s успешно удалена из списка исключений лимита сессий.
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
	Некорректное значение лимита. Оно должно быть целым числом, больше или равным нулю, и меньше %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Указана некорректная хостмаска. Только реальный хост, не содержащий ников и идентов пользователей, может быть добавлен в список исключения.
OPER_EXCEPTION_EXISTS
	Запись вида %s уже присутствует в списке исключений из лимита сессий.
OPER_EXCEPTION_CHANGED
	Лимит сессий для записи вида %s был изменен на %d.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST лимит | VIEW хост}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST лимит
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW хост
OPER_SESSION_DISABLED
	Ограничение по количеству сессий отключено.
OPER_SESSION_INVALID_THRESHOLD
	Некорректное значение для порога сессий. В качестве параметра должно быть целое число >1.
OPER_SESSION_NOT_FOUND
	Хост вида %s в списке сессий не числится.
OPER_SESSION_LIST_HEADER
	Список хостов с количеством сессий %d и более:
OPER_SESSION_LIST_COLHEAD
	Сессии    Хост
# лимит, хост
OPER_SESSION_LIST_FORMAT
	%6d    %s
# хост, сессии, лимит
OPER_SESSION_VIEW_FORMAT
	Количество сессий с хоста %s: %d, допустимый максимум для данного хоста: %d

# EXCEPTION help

OPER_HELP_EXCEPTION
	Синтаксис: EXCEPTION ADD [+срок_истечения] маска лимит причина
	           EXCEPTION DEL {маска | список_записей}
	           EXCEPTION MOVE номер позиция
	           EXCEPTION LIST [маска | список_записей]
	           EXCEPTION VIEW [маска | список_записей]
	
	Позволяет Администраторам сервисов управлять списком хостов, которые
	будут иметь отдельно указанные ограничения на количество сессий.
	Возможности списка исключений позволяют разрешить определенным хостам
	(шеллов или BNC) иметь  большее  количество  клиентов  одновременно,
	нежели разрешено по-умолчанию. Как только количество сессий с хоста 
	достигнет установленного лимита, все последующие клиенты, пытающиеся
	подключиться с данного хоста, будут автоматически отключаться.
	Перед отключением, пользователь получит уведомление от %S
	о том, что с его хоста превышен лимит подключений. Тект сообщения
	можно изменить в конфигурационном файле сервисов.
	
	Команда EXCEPTION ADD позволяет добавить указанную маску в список
	исключений. Учтите, что общие хостмастки вида ник!идент@хост или
	идент@хост недопустимы! Вы должны указывать только реальные хосты
	вида box.host.dom и *.host.dom, так как ограничение сессий не
	использует при проверке ники и/или иденты. Значение лимита должно
	быть целым числом, больше или равным нулю. Данный параметр определяет,
	как много сессий данный хост может использовать одновременно.
	Установка этого значения равным 0 позволит хосту иметь неограниченное
	число сессий. Для более подробной информации о сроке истечения см.
	справку по команде AKILL.
	
	Команда  EXCEPTION DEL  позволяет  удалить  указанную  маску (или
	диапазон записей) из списка исключений.
	
	Команда EXCEPTION MOVE позволяет переместить указанное исключение
	под указанным номером на указанную позицию. Соответственно, n-ое
	количество записей списка исключений  будет  сдвинуто выше или ниже,
	чтобы заполнить образовавшийся промежуток.
	
	Команды EXCEPTION LIST и EXCEPTION VIEW позволяют получить текущий
	список исключений. Если в качестве параметра указана маска, будут
	отображены только совпадающие с ней записи.
	
	Команда EXCEPTION VIEW позволяет получить более подробную информацию
	об исключении: ник добавившего запись, установленный лимит, указанную
	при установке причину,  сам хост,  срок  истечения  записи и дату ее
	создания.
	
	Стоит заметить, что сервисы будут использовать значения первой же
	совпадающей записи из списка исключений для попадающего под условия
	хоста. Слишком большой список исключений с широкими масками в
	достаточной мере влияет на производительность сервисов.

OPER_HELP_SESSION
	Синтаксис: SESSION LIST порог
	           SESSION VIEW хост
	
	Позволяет Администраторам сервисов просматривать список сессий.
	
	Команда SESSION LIST позволяет получить список хостов с количеством
	сессий  большим  или равным порогу.  Значение порога должно быть
	целым числом и больше 1. Это позволит избежать слишком большого списка
	хостов с единичным количеством сессий.
	
	Команда  SESSION VIEW  покажет  детальную информацию об указанном
	хосте - текущее количестов сессий и ограничение для указанного хоста.
	Помните, что указанный хост не должен включать в себя подстановочные
	символы вида '*' и '?'.
	
	См. EXCEPTION HELP для более подробной информации о лимитировании
	сессий и как установить отдельный лимит сессий для определенного
	хоста или группы хостов.

OPER_HELP_STAFF
	Синтаксис: STAFF
	
	Выводит список привилегированных на сервисах пользователей (таких
	как Администраторы или Операторы сервисов) и их текущий статус.

OPER_HELP_DEFCON
	Синтаксис: DEFCON [1|2|3|4|5]
	
	Система DEFCON позволяет вам устанавливать определенные глобальные
	ограничения в сети со стороны сервисов. Данные ограничения могут
	касаться не только работы самих сервисов, но и функционирования сети
	в целом, что, порою, весьма полезно при разного рода атаках на сеть.

OPER_HELP_DEFCON_NO_NEW_CHANNELS
	* Запрет на регистрацию каналов.
OPER_HELP_DEFCON_NO_NEW_NICKS
	* Запрет на регистрацию ников.
OPER_HELP_DEFCON_NO_MLOCK_CHANGE
	* Запрет на изменение параметров MLOCK.
OPER_HELP_DEFCON_FORCE_CHAN_MODES
	* Принудительная установка режимов %s на все каналы сети.
OPER_HELP_DEFCON_REDUCE_SESSION
	* Уменьшение лимита сессий с одного хоста до %d.
OPER_HELP_DEFCON_NO_NEW_CLIENTS
	* Автоматическое отключение от сети всех новых клиентов.
OPER_HELP_DEFCON_OPER_ONLY
	* Игнорирование простых пользователей с оповещением их об этом.
OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* Игнорирование простых пользователей без уведомления их об этом.
OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* Автоматическая установка AKILL'ов на всех новых клиентов.
OPER_HELP_DEFCON_NO_NEW_MEMOS
	* Запрет на отправку мемо-сообщений.

OPER_HELP_CHANKILL
	Синтаксис: CHANKILL [+срок_истечения] #канал причина

	Добавляет в AKILL-список всех пользователей указанного канала с
	указанной причиной на указанный промежуток времени. В качестве маски
	AKILL'ов будут использованы маски посетителей канала вида идент@хост.

###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Новости сети - %s] %s
NEWS_OPER_TEXT
	[Опер-новости - %s] %s
NEWS_RANDOM_TEXT
	[Случайная новость - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD|DEL|LIST} [текст|номер]
NEWS_LOGON_LIST_HEADER
	Текущие список новостей:
NEWS_LOGON_LIST_ENTRY
	%5d (%s от %s)
	    %s
NEWS_LOGON_LIST_NONE
	Список новостей пуст.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD текст
NEWS_LOGON_ADD_FULL
	Список новостей переполнен.
NEWS_LOGON_ADDED
	Новость успешно добавлена, ее номер: #%d
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {номер | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Новость под номером #%d не обнаружена.
NEWS_LOGON_DELETED
	Новость под номером #%d удалена.
NEWS_LOGON_DEL_NONE
	Список новостей пуст, удалять нечего.
NEWS_LOGON_DELETED_ALL
	Список новостей полностью очищен.

NEWS_OPER_SYNTAX
	Синтаксис: OPERNEWS {ADD|DEL|LIST} [текст|номер]
NEWS_OPER_LIST_HEADER
	Список новостей для IRC-операторов:
NEWS_OPER_LIST_ENTRY
	%5d (%s от %s)
	    %s
NEWS_OPER_LIST_NONE
	Список опер-новостей пуст.
NEWS_OPER_ADD_SYNTAX
	Синтаксис: OPERNEWS ADD текст
NEWS_OPER_ADD_FULL
	Список опер-новостей переполнен.
NEWS_OPER_ADDED
	Новость успешно добавлена, ее номер: #%d
NEWS_OPER_DEL_SYNTAX
	Синтаксис: OPERNEWS DEL {номер | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Новость под номером #%d не обнаружена.
NEWS_OPER_DELETED
	Новость под номером #%d удалена.
NEWS_OPER_DEL_NONE
	Список опер-новостей пуст, удалять нечего.
NEWS_OPER_DELETED_ALL
	Список новостей для операторов полностью очищен.

NEWS_RANDOM_SYNTAX
	RANDOMNEWS {ADD|DEL|LIST} [текст|номер]
NEWS_RANDOM_LIST_HEADER
	Список случайных новостей:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s от %s)
	    %s
NEWS_RANDOM_LIST_NONE
	Список случайных новостей пуст.
NEWS_RANDOM_ADD_SYNTAX
	RANDOMNEWS ADD текст
NEWS_RANDOM_ADD_FULL
	Список случайных новостей переполнен.
NEWS_RANDOM_ADDED
	Новость успешно добавлена, ее номер: #%d
NEWS_RANDOM_DEL_SYNTAX
	RANDOMNEWS DEL {номер | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Новость под номером #%d не обнаружена.
NEWS_RANDOM_DELETED
	Новость под номером #%d удалена.
NEWS_RANDOM_DEL_NONE
	Список случайных новостей пуст, удалять нечего.
NEWS_RANDOM_DELETED_ALL
	Список случайных новостей полностью очищен.

NEWS_HELP_LOGON
	Синтаксис: LOGONNEWS ADD текст
	           LOGONNEWS DEL {номер | ALL}
	           LOGONNEWS LIST
	
	Позволяет  управлять списком новостей,  которые будут автоматически
	выдаваться пользователю  сразу же, как только он подключится к сети.
	Стоит заметить, что во избежание  флуда пользователь получит только
	%s новость(и), так что если у  вас в списке их  больше - показаны
	будут только  последние из них. Значение  NewsCount,  отвечающее за
	одновременно посылаемое кол-во новостей, указывается в services.conf
	
	Команда LOGONNEWS ADD позволяет добавить новость в список.
	
	Команда  LOGONNEWS DEL  позволяет  удалить  новость из списка. В
	качестве параметра вы можете указать номер новости - что бы удалить
	одну новость, или ALL - что бы очистить список новостей полностью.
	
	LOGONNEWS may only be used by Services admins. 

NEWS_HELP_OPER
	Синтаксис: OPERNEWS ADD текст
	           OPERNEWS DEL {номер | ALL}
	           OPERNEWS LIST
	
	Позволяет управлять списком новостей для IRC-Операторов.  Как только
	пользователь получит статус IRC-оператора (использовав команду /OPER),
	данные сообщения будут ему отправлены автоматически.
	Стоит заметить, что во избежание  флуда пользователь получит только
	%s новость(и), так что если у  вас в списке их  больше - показаны
	будут только  последние из них. Значение  NewsCount,  отвечающее за
	одновременно посылаемое кол-во новостей, указывается в services.conf
	
	Команда OPERNEWS ADD позволяет добавить новость в список.
	
	Команда OPERNEWS DEL позволяет удалить новость из списка. В
	качестве параметра вы можете указать номер новости - что бы удалить
	одну новость, или ALL - что бы очистить список новостей полностью.
	
	OPERNEWS may only be used by Services admins. 

NEWS_HELP_RANDOM
	Синтаксис: RANDOMNEWS ADD текст
	           RANDOMNEWS DEL {текст | ALL}
	           RANDOMNEWS LIST
	
	Позволяет управлять списком случайных новостей.
	Как  только  пользователь  успешно  подключится к IRC-сети, одна (и
	только одна) из случайных новостей будет выбрана сервисами наугад и
	отправлена ему.
	
	Команда RANDOMNEWS ADD позволяет добавить новость в список.
	
	Команда  RANDOMNEWS DEL  позволяет  удалить новость из списка. В
	качестве параметра вы можете указать номер новости - что бы удалить
	одну новость, или ALL - что бы очистить список новостей полностью.
	
	RANDOMNEWS may only be used by Services admins.

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S - это справочный сервис, призванный сообщить вам информацию обо
	всех основных сервисах сети. Вся справочная информация доступна через
	команду HELP:
	Основные сервисы сети:
	
	%R%s HELP
	     справочная информация о том, как регистрировать ники и
	     настраивать уже зарегистрированные.
	
	%R%s HELP
	     справочная информация о том, как регистрировать каналы и
	     управлять уже зарегистрированными.
	
	%R%s HELP
	     справочная информация о том, как отправлять и читать
	     мемо-сообщения.

HELP_HELP_BOT
	Дополнительные сервисы сети:
	
	%R%s HELP
	     справочная информация о том, как пригласить сервисного бота на
	     свой канал и правильно его настроить.

HELP_HELP_HOST

	%R%s HELP
	     справочная информация об установке/удалении vHost'ов
	     (виртуальных хостов).

###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP_CMD_CONFIRM
	    CONFIRM    Подтверждение регистрации кодом аутенфикации
NICK_HELP_CMD_RESEND
	    RESEND     Повторный запрос auth-кода на email
NICK_HELP_CMD_REGISTER
	    REGISTER   Регистрация ника
NICK_HELP_CMD_GROUP
	    GROUP      Объединение ников в группы
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY   Идентификация к нику в качестве владельца
NICK_HELP_CMD_ACCESS
	    ACCESS     Управление ACCESS-списком ника (списком хостмасок)
NICK_HELP_CMD_SET
	    SET        Настройка опций ника, включая защиту KILL'ом
NICK_HELP_CMD_SASET
	    SASET      Установка различных опций какого-либо ника
NICK_HELP_CMD_DROP
	    DROP       Отмена регистрации ника (удаление)
NICK_HELP_CMD_RECOVER
	    RECOVER    Освобождение вашего ника от использования его кем-то другим
NICK_HELP_CMD_RELEASE
	    RELEASE    Снятие защиты сервисов с вашего ника после команды RECOVER
NICK_HELP_CMD_SENDPASS
	    SENDPASS   Запрос пароля от ника на email-адрес
NICK_HELP_CMD_GHOST
	    GHOST      Отключение "мертвой" IRC-сессий с вашим ником
NICK_HELP_CMD_ALIST
	    ALIST      Вывод списка каналов, на которых у вас есть доступ
NICK_HELP_CMD_GLIST
	    GLIST      Список всех ников в вашей группе
NICK_HELP_CMD_INFO
	    INFO       Информация о каком-либо нике
NICK_HELP_CMD_LIST
	    LIST       Список всех зарегистрированных ников по маске
NICK_HELP_CMD_LOGOUT
	    LOGOUT     Деидентификация от используемого на данный момент ника
NICK_HELP_CMD_STATUS
	    STATUS     Запрос статуса идентификации указанного ника на сервисах
NICK_HELP_CMD_UPDATE
	    UPDATE     Обновление вашего текущего статуса на сервисах
NICK_HELP_CMD_GETPASS
	    GETPASS    Запрос пароля указанного ника
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Запрос списка всех ников с указанным email
NICK_HELP_CMD_FORBID
	    FORBID     Установка запрета на использование/регистрацию ника
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Установка режима саспенда ("заморозки") на ник
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Снятие режима саспенда с ника

NICK_HELP
	%S - это сервис, предназначенный для "регистрации" вашего
	ника и защиты его от использования еще кем-либо кроме вас. Ниже
	представлен список команд, позволяющих осуществлять регистрацию и
	настройку ников. Чтобы использовать команду, пошлите запрос вида
	%R%S команда.
	Для более подробной информации по какой-либо команде используйте:
	%R%S HELP команда.
	
	
NICK_HELP_FOOTER
	 
	Примечание: главное и основное предназначение %S - гарантия
	подлинности того или иного пользователя IRC-сети и защита его ника
	от использования посторонним лицом. Любые попытки злоупотребления
	%S, такие как: воровство ников, захват большого количества
	ников и другие злонамеренные действия - приведут, как минимум, к
	уничтожению этих самых ников.
	
NICK_HELP_EXPIRES
	
	Помните, что  неиспользуемые  в  течение  длительного  периода ники
	автоматически удаляются из базы данных. Срок существования неактивных
	ников: %d дней.

NICK_HELP_REGISTER
	Синтаксис: REGISTER пароль [email]
	
	Позволяет зарегистрировать ваш текущий ник в базе данных %S.
	После успешной его регистрации, вы можете использовать команды
	SET и ACCESS что бы сконфигурировать параметры ника так, как
	вам будет угодно. Постарайтесь не забыть указанный при регистрации
	пароль - он понадобится вам для дальнейшего использования ника!
	
	Принципы выбора пароля:
	Пароли не должны быть легко угадываемы. Например, использование
	вашего реального имени в качестве пароля - плохая идея.
	Использование вашего ника в качестве пароля - совсем плохая идея,
	%S просто-напросто не допустит этого. Также, короткие пароли
	очень легко подбираются простым перебором, поэтому длина пароля
	должна быть больше 5 символов. Пробелы и знаки табуляции в пароле
	недопустимы, вы можете использовать только латинские буквы и цифры.
	Примечание: все пароли чувствительны к регистру! MYPASSWORD,
	MyPassword, и mypassword - это разные пароли!
	
	Указанный  в качестве  дополнительного  параметра  email  будет
	автоматически ассоциирован с регистрируемым ником. Данный параметр
	может быть как опциональным, так и обязательным - это  зависит от
	требований и настроек IRC-сети.
	Вы можете не беспокоиться о приватности вашего email-адреса - никто
	из посторонних лиц его не получит.
	
	Так же, регистрация нового ника автоматически создает для него новую
	группу. Возможности групп ников позволяют вам иметь несколько ников
	с общей для них информацией, настройками, списком доступа. Для более
	подробной информации см. %R%S HELP GROUP
	
NICK_HELP_GROUP
	Синтаксис: GROUP главный_ник пароль
	
	Данная команда позволяет вам присоединить ваш текущий ник к группе
	главного_ника. В качестве пароля вам необходимо указать пароль
	главного ника.
	
	Использование групп дает вам возможность объединить несколько ников
	в одну группу с едиными настройками и привилегиями на каналах,
	едиными ассоциированными данными и паролем, единым списком сообщений.
	Группа существует и активна пока используется. Даже если вы удалите
	один из ников группы, все вышеописанные данные (ну и сама регистрация)
	будут существовать до тех пор, пока в группе есть хотя бы 1 ник.
	
	Вы можете использовать эту команду даже если ваш текущий ник все еще
	незарегистрирован. В противном случае, для использования описываемой
	команды вам необходимо будет идентифицироваться к группируемому нику.
	См. %R%S HELP IDENTIFY для более подробной информации.
	Примечание: возможность группировки уже зарегистрированных ников
	может быть недоступна в вашей IRC-сети.
	
	Оптимальным вариантом будет объединение еще не зарегистрированных
	ников с уже зарегистрированным (они указываются как главный ник).
	В противном случае, все данные группируемого ника будут уничтожены
	в процессе объединения двух ников. Именно поэтому, слияние групп -
	невозможно. Один зарегистрированный ник, не состоящий ни в одной
	группе - уже образует группу. И находится в двух группах этот ник
	никак неможет.
	Примечание 2: у всех ников в группе - единый пароль.

NICK_HELP_IDENTIFY
	Синтаксис: IDENTIFY пароль
	
	Позволяет вам идентифицироваться на %S к зарегистрированному
	ранее нику. Большинство сервисных команд, такие как SET, OP, BAN
	и т. д., требуют идентификации перед использованием. В качестве
	пароля вы должны указать тот самый пароль, который указывали при
	регистрации вашего ника.

NICK_HELP_UPDATE
	Синтаксис: UPDATE

	Обновляет ваш текущий статус на сервисах, в частности, проверяет на
	наличие новых мемо-сообщений, устанавливает необходимые режимы на
	всех каналах, где у вас есть статус (ModeonID), обновляет ваши
	виртуальные хосты и настройки (дата последнего посещения IRC, и т.д.).

NICK_HELP_LOGOUT
	Синтаксис: LOGOUT
	
	Данная команда имеет эффект, обратный эффекту команды IDENTIFY,
	то есть: снимает с вас статус владельца ника (деидентифицирует).
	Примечание: тем не  менее,  после  использования  данной  команды,
	сервисы не пошлют вам запрос о необходимости идентификации к нику.

NICK_HELP_DROP
	Синтаксис: DROP [ник]
	
	Удаляет ваш ник из базы данных %S. Удаленный таким образом ник
	снова становится свободным для регистрации.
	
	Использование ника в качестве дополнительного параметра
	позволяет вам удалять ник в пределах вашей группы.
	
	Чтобы использовать данную команду, вы должны быть идентифицированы
	к удаляемосу нику (или группе ников). Для более подробной информации
	см. %R%S HELP IDENTIFY

NICK_HELP_ACCESS
	Синтаксис: ACCESS ADD маска
	           ACCESS DEL маска
	           ACCESS LIST
	
	Позволяет просматривать/изменять список масок доступа на ваш ник.
	Имеется ввиду список хостмасок вида идент@хост, с которых %S
	позволит  использовать  ваш  ник  даже  без и дентификации к нему.
	В противном случае, чтобы использовать ник с хостмаски которая не
	присутствует в списоке доступа, вам необходимо будет использовать
	команду IDENTIFY для идентификации на %S.
	
	Примеры:
	
	    ACCESS ADD anyone@*.set.ru
	        позволяет использовать ник всем, кто имеет идент anyone
	        и чей хост попадает под маску *.set.ru.
	    ACCESS DEL anyone@*.set.ru
	        антоним предыдущей команды - удаляет маску из списка.
	    ACCESS LIST
	        выводит текущий список "доверенных" хостмасок.

NICK_HELP_SET
	Синтаксис: SET опция параметры
	
	Команда SET позволяет вам настроить различные опции ника.
	Список опций:
	
	    DISPLAY    установка главного ника группы
	    PASSWORD   изменение текущего пароля на ник
	    LANGUAGE   выбор языка, посредством которого
	               сервисы будут с вами общаться
	    URL        установка URL-адреса на ник
	    EMAIL      установка email-адреса на ник
	    ICQ        установка ICQ-номера на ник
	    GREET      установка приветственного сообщения
	    KILL       активирование/деактивирование режима защиты
	    SECURE     активирование/деактивирование режима безопасности
	    PRIVATE    скрытие вашего ника в списке ников по %R%S LIST
	    HIDE       скрытие различной информации о вашем нике
	    MSG        выбор метода общения сервисов с вами
	    AUTOOP     активирование/деактивирование автостатуса
	
	Чтобы получить справочную информацию по отдельно взятой опции
	воспользуйтесь командой %R%S HELP SET опция
	
	Примечание: перед  установкой  какой-либо опции, вам необходимо
	идентифицироваться  к  нику  в  качестве его владельца. Для более
	подробной информации см. %R%S HELP IDENTIFY

NICK_HELP_SET_DISPLAY
	Синтаксис: SET DISPLAY новый_ник
	
	Позволяет переназначить главный ник группы. Именно этот ник будет
	отображаться в списке доступа каналов, где вы прописаны.
	Примечание: новый главный ник должен состоять в вашей группе ников.

NICK_HELP_SET_PASSWORD
	Синтаксис: SET PASSWORD новый_пароль
	
	Позволяет изменить пароль ника, используемый для идентификации к
	нему в качестве полноправного владельца.

NICK_HELP_SET_LANGUAGE
	Синтаксис: SET LANGUAGE номер
	
	Позволяет  выбрать  язык,  на  котором  сервисы будут посылать вам
	различные сообщения. Например, текст справочной системы, который вы
	сейчас и читаете.
	В качестве номера, вы должны указать конкретный номер языка из
	списка поддерживаемых языков:
	

NICK_HELP_SET_URL
	Синтаксис: SET URL url-адрес
	
	Ассоциирует URL-адрес с вашим ником. Данный URL будет показан в
	информации о вашем нике, предоставляемой по команде INFO.
	
NICK_HELP_SET_EMAIL
	Синтаксис: SET EMAIL адрес@email
	
	Ассоциирует email-адрес с вашим ником. Данный email будет показан
	в информации о вашем нике, предоставляемой по команде INFO.
	
NICK_HELP_SET_ICQ
	Синтаксис: SET ICQ ICQ-номер
	
	Ассоциирует ICQ-номер с вашим ником. Данный ICQ-номер будет показан
	в информации о вашем нике, предоставляемой по команде INFO.
	
NICK_HELP_SET_GREET
	Синтаксис: SET GREET сообщение
	
	Позволяет  назначить  приветственное сообщение, которое сервисный
	бот будет говорить каждый раз, когда вы заходите на канал. Учтите,
	что для отображения приветствия, на канале должна быть активирована
	опция GREET и вы должны иметь соответствующий уровень доступа.

NICK_HELP_SET_KILL
	Синтаксис: SET KILL {ON | QUICK | IMMED | OFF}
	
	Активирует/деактивирует  автоматическую  защиту  для  вашего  ника.
	Активирование защиты ника позволяет вам лимитировать период времени
	на ввод пароля для идентификации. ON выставляет период в 60 секунд,
	по истечении которого сервисы принудительно сменят ник лже-владельца
	(или ваш, если вы не успели ввести пароль вовремя).
	
	Параметр QUICK позволяет назначить 20-секундный период.
	
	Параметр IMMED  позволяет отключить этот период вообще. Помните,
	что активирование этой опции  может  привести к потере вашего ника!
	Не используйте эту опцию без крайней на то необходимости, а перед ее
	включением, убедитесь, что ACCESS-список вашего ника содержит хотя
	бы одну из хостмасок, которая совпадает с вашей текущей.
	Примечание: возможность установки метода защиты IMMED может быть
	отключена в вашей IRC-сети.

NICK_HELP_SET_SECURE
	Синтаксис: SET SECURE {ON | OFF}
	
	Активирует/деактивирует  режим  безопасности  для вашего ника. При
	включенной опции SECURE вы должны идентифицироваться на %S 
	с помощью пароля, чтобы подтвердить что именно вы владелец текущего
	используемого вами ника. При этом на список хостмасок ника сервисы
	внимания не обращают.
	Отключение этой опции позволит вам идентифицироваться на %S
	через список масок ника, однако, максимально допустимый статус в
	данном случае будет равняться 2.
	
	Стоит также заметить, что сервисы не будут принудительно менять ваш
	ник, если в ACCESS-списке зарегистрированного ника есть хотя бы одна
	хостмаска, совпадающая с вашей текущей. Режим опции KILL при этом
	значения не имеет.

NICK_HELP_SET_PRIVATE
	Синтаксис: SET PRIVATE {ON | OFF}
	
	Активирует\деактивирует опцию приватности вашего ника на %S.
	Включенная таким образом PRIVATE скрывает ваш ник из информации
	по %S LIST.
	Примечание: тем не менее, любой, кто знает ваш ник, имеет возможность
	получить информацию о вас используя команду INFO.

NICK_HELP_SET_HIDE
	Синтаксис: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Данная опция позволяет вам скрыть часть информации о вашем нике,
	которая может запрошена у %S с помощью команды INFO
	
	В качестве второго параметра вы должны указать что именно будет
	скрыто:
	USERMASK  -  для скрытия вашей последней хостмаски вида идент@хост
	  STATUS  -  для скрытия вашего уровня доступа к сервисам
	   EMAIL  -  для скрытия ассоциированного email-адреса.
	    QUIT  -  для скрытия вашего последнего quit-сообщения
	
	В качестве третьего параметра вы должны указать либо ON либо OFF.
	Первый активирует скрытие, второй, соответственно, деактивирует.
	
NICK_HELP_SET_MSG
	Синтаксис: SET MSG {ON | OFF}
	
	Позволяет вам выбрать способ общения сервисов с вами. Укажите ON,
	что бы активировать режим приватных сообщений - в этом случае все
	сообщения от сервисов будут приходить вам в приват. Параметр OFF
	заставит сервисы использовать режим уведомлений (notice, нотисы).
	Примечание: настройка данной опции может быть заблокирована.

NICK_HELP_SET_AUTOOP
	Синтаксис: SET AUTOOP {ON | OFF}
	
	Активирует/деактивирует режим втоматического получения статуса.
	Укажите ON, если хотите что бы сервисы автоматически давали вам
	статус при входе на канал. Соответственно OFF - для отключения.

NICK_HELP_SASET
	Синтаксис: SASET ник опция параметры.
	
	Позволяет Администраторам сервисов настраивать какие-либо опции для
	указанного ника без идентификации к нему. Список опции:
	
	    DISPLAY    установка главного ника группы
	    PASSWORD   изменение текущего пароля на ник
	    URL        установка URL-адреса на ник
	    EMAIL      установка email-адреса на ник
	    ICQ        установка ICQ-номера на ник
	    GREET      установка приветственного сообщения
	    KILL       активирование/деактивирование режима защиты
	    SECURE     активирование/деактивирование режима безопасности
	    PRIVATE    скрытие ника в списке ников по %R%S LIST
	    HIDE       скрытие различной информации о нике
	    MSG        выбор метода общения сервисов с ником
	    NOEXPIRE   установка на ник т.н. режима 'не-истечения'
	    AUTOOP     активирование/деактивирование режима автостатуса
	    LANGUAGE   выбор языка, посредством которого сервисы будут с
	               указанным ником.
	
	Чтобы получить справочную информацию по отдельно взятой опции
	воспользуйтесь командой %R%S HELP SASET опция
	Помните, вы должны указать ник, опции которого вы хотите изменить.

NICK_HELP_SASET_DISPLAY
	Синтаксис: SASET ник DISPLAY новый_ник 
	
	Позволяет переназначить главный ник группы указанного ника. Именно
	этот ник будет отображаться в списке доступа каналов, где ник
	прописан.
	Примечание: новый главный ник должен состоять в группе ника.

NICK_HELP_SASET_PASSWORD
	Синтаксис: SASET ник PASSWORD новый_пароль 
	
	Позволяет изменить пароль ника, используемый для идентификации
	в качестве владельца ника.

NICK_HELP_SASET_URL
	Синтаксис: SASET ник URL url-адрес
	
	Ассоциирует URL-адрес с указанным ником. Данный URL будет показан
	в информации об указанном нике, предоставляемой по команде INFO.

NICK_HELP_SASET_EMAIL
	Синтаксис: SASET ник EMAIL адрес@email
	
	Ассоциирует email-адрес с указанным ником. Данный email будет показан
	в информации об указанном нике, предоставляемой по команде INFO.
	
NICK_HELP_SASET_ICQ
	Синтаксис: SASET ник ICQ ICQ-номер
	
	Ассоциирует ICQ-номер с указанным ником. Данный номер будет показан
	в информации об указанном нике, предоставляемой по команде INFO.

NICK_HELP_SASET_GREET
	Синтаксис: SASET ник GREET текст
	
	Позволяет назначить приветственное сообщение, которое сервисный
	бот будет говорить каждый раз, когда ник зайдет на канал. Учтите,
	что для отображения приветсвия, на канале должна быть активирована
	опция GREET и ник должен иметь соответствующий уровень доступа.

NICK_HELP_SASET_KILL
	Синтаксис: SASET ник KILL {ON | QUICK | IMMED | OFF}
	
	Активирует/деактивирует автоматическую защиту для указанного ника.
	Активирование защиты ника позволяет вам лимитировать период времени
	на ввод пароля идентификации. ON выставляет период в 60 секунд,
	по истечении которого %S принудительно сменит ник лже-владельца,
	попытавшегося взять указанный ник.
	
	Параметр QUICK позволяет назначить 20-секундный период.
	Параметр IMMED позволяет отключить этот период вообще. Помните,
	что активирование данной опции может привести к тому, что указанный
	ник не сможет взять никто! Не используйте эту опцию без крайней
	необходимости, а перед ее включением убедитесь, что ACCESS-список
	указанного ника содержит хотя бы одну из хостмасок, с которой его
	владелец заходит в IRC.
	Примечание: возможность установки метода защиты IMMED может быть
	отключена в вашей IRC-сети.

NICK_HELP_SASET_SECURE
	Синтаксис: SASET ник SECURE {ON | OFF}
	
	Активирует/деактивирует режим безопасности для указанного ника. При
	включенной опции SECURE владелец ника должен идентифицироваться
	на %S с помощью пароля, что бы подтвердить что именно он владелец
	текущего используемого им ника. При этом на список хостмасок ника
	сервисы внимания обращать не будут.
	Отключение этой опции позволит владельцу идентифицироваться на %S
	через список масок ника, однако максимально допустимый статус в
	данном случае будет равняться 2.
	
	Стоит так же заметить, что сервисы не будут принудительно менять ник
	владельца если в ACCESS-списке зарегистрированного ника есть хотя
	бы одна хостмаска, совпадающая с текущей хостмаской пользователя,
	взявшего ник. Режим опции KILL при этом значения не имеет.

NICK_HELP_SASET_PRIVATE
	Синтаксис: SASET ник PRIVATE {ON | OFF}
	
	Активирует\деактивирует опцию приватности указанного ника.
	Включенная таким образом PRIVATE скрывает отображение ника в
	списках по %S LIST.
	Примечание: тем не менее любой, кто знает точное написание ника,
	имеет возможность получить информацию о нем используя команду INFO.

NICK_HELP_SASET_HIDE
	Синтаксис: SASET ник HIDE {EMAIL|STATUS|USERMASK|QUIT} {ON|OFF}
	
	Данная опция позволяет вам скрыть часть информации об указанном
	нике, которая может запрошена у %S с помощью команды INFO
	
	В качестве второго параметра вы должны указать что именно будет
	скрыто:
	QUIT -  для скрытия последнего quit-сообщения владельца
	EMAIL - для скрытия ассоциированного email-адреса владельца
	STATUS - для скрытия уровня доступа к сервисам
	USERMASK - для скрытия последней хостмаски владельца (идент@хост)
	
	В качестве третьего параметра вы должны указать либо ON либо OFF.
	Первый активирует скрытие, второй, соответственно, деактивирует.
	
NICK_HELP_SASET_MSG
	Синтаксис: SASET ник MSG {ON | OFF}
	
	Позволяет вам выбрать способ общения сервисов с указанным ником.
	Укажите ON, что бы активировать режим приватных сообщений - в
	этом случае все сообщения от сервисов будут приходить владельцу в
	приват. Параметр OFF заставит сервисы использовать режим
	уведомлений (notice, нотисы).
	Примечание: настройка данной опции может быть заблокирована.

NICK_HELP_SASET_NOEXPIRE
	Синтаксис: SASET ник NOEXPIRE {ON | OFF}
	
	Позволяет исключить указанный ник из условий истечения регистрации
	по времени. Укажите ON, что бы активировать так называемый режим
	'не-истечения'. Параметр OFF снова возвращает ник в режим истечения.

NICK_HELP_SASET_AUTOOP
	Синтаксис: SASET ник AUTOOP {ON | OFF}
	
	Активирует/деактивирует режим автоматического получения статуса.
	Укажите ON, если хотите что бы сервисы автоматически давали
	владельцу ника статус при входе на канал. Соответственно OFF  -
	для отключения.

NICK_HELP_SASET_LANGUAGE
	Синтаксис: SASET ник LANGUAGE номер
	
	Изменяет язык, на котором сервисы будут посылать сообщения указанному
	нику (например,  текст  справочной  системы,  который  вы сейчас и
	читаете).
	В  качестве  номера  вы  должны указать конкретный номер языка из
	списка поддерживаемых языков: %R%S HELP SET LANGUAGE.

NICK_HELP_RECOVER
	Синтаксис: RECOVER ник [пароль]
	
	Команда RECOVER позволяет  вам вернуть ваш зарегистрированный ник
	в том случае если его взял  и  использует кто-то другой. Вы можете
	избежать подобных ситуаций - для этого достаточно настроить защиту
	вашего ника. Подробнее о режиме защиты вы можете узнать из справочной
	информации по SET KILL.
	
	Как только вы пошлете RECOVER-запрос, сервисы сети тут же создадут
	псевдопользователя с вашим зарегистрированным ником, что заставит
	IRC-сервер отключить захватчика от сети. Эффект команды разнится в
	зависимости от того, какой IRCd используется в вашей сети: если ваш
	сервер не поддерживает SVSNICK, то захватчик будет отключен, как при
	GHOST, а если поддерживает, то  ник  захватчика  будет  изменен на
	случайный, например на Guest31526. Опять же,  префикс случайного
	ника настраивается в конфиге сервисов и может меняться от сети к сети.
	Но и это еще не все. В зависимости от типа IRCd вашей сети, сервисы
	будут либо создавать псевдопользователя, либо устанавливать временный
	запрет в виде Q:LINE на ваш ник. Этот запрет (будь то псевдоклиент
	или Q:LINE) будет запрещать кому-либо взять восстановленный  ник  в
	течение  определенного  промежутка  времени,  указанного  в конфиге
	сервисов (по-умолчанию, это %s).
	
	Как только этот промежуток времени истечет - вы сможете использовать
	свой ник снова. Если вы не хотите ждать - воспользуйтесь командой
	RELEASE чтобы снять защиту сервисов с вашего ника немедленно. Для
	более подробной информации см. %R%S HELP RELEASE.
	
	Чтобы использовать данную команду,  вы  должны  указать пароль для
	изменяемого ника, либо же, вы должны быть идентифицированы к группе
	изменяемого ника. Допустимо использование данной команды даже при
	идентификации к нику всего лишь через список хостмасок.

NICK_HELP_RELEASE
	Синтаксис: RELEASE ник [пароль]
	
	Позволяет вручную  освободить ваш зарегистрированный  ник от удержания
	его сервисами.  Обычно это происходит в том случае, если у вашего ника
	активирован  режим  защиты  и  пользователь, который  пытался  ваш ник
	использовать, не успел ввести верный  пароль или не вводил  его вовсе.
	Эффект режима защиты аналогичен эффекту команды RECOVER (см. справку
	по %R%S HELP RECOVER для более подробной информации). Удержание ника
	продолжается в течении определенного промежутка времени, указанного  в
	конфиге сервисов (по-умолчанию, это %s).
	
	Чтобы  использовать  данную команду,  вы  должны  указать пароль для
	удерживаемого ника, либо же, вы должны быть идентифицированы к группе
	удерживаемого ника. Допустимо использование  данной команды  даже при
	идентификации к нику всего лишь через список хостмасок.

NICK_HELP_GHOST
	Синтаксис: GHOST ник [пароль]
	
	Уничтожает "мертвую" IRC-сессию, использующую Ваш ник. "Мертвая"
	сессия - это когда реально ее не существует, но IRC-сервер думает
	что клиент все еще в сети. Чаще всего это происходит при внезапном
	обрыве связи с сервером, когда IRC-клиент не успевает послать оному
	уведомление об отключении (например, когда ваш компьютер внезапно
	перезагрузился или у вас не менее внезапно отключили электричество).
	
	Чтобы  использовать  данную  команду,  вы  должны указать пароль для
	отключаемого ника, либо же, вы должны быть идентифицированы к группе
	отключаемого ника. Допустимо использование данной команды даже при
	идентификации к нику всего лишь через список хостмасок.

NICK_HELP_INFO
	Синтаксис: INFO ник [ALL]
	
	Запрашивает у сервисов информацию об указанном нике. В частности
	будут показаны: реальное имя владельца (realname), его последняя
	хостмаска, когда он последний раз был в IRC под этим ником. Если вы
	запрашиваете информацию относительно своего ника, вы можете указать
	параметр ALL, чтобы получить всю доступную информацию, в том числе
	и ту, которая скрыта.

NICK_HELP_LIST
	Синтаксис: LIST маска
	
	Выводит список всех зарегистрированных ников, которые совпадают с
	указанной маской вида ник!идент@хост.
	Ники с активированной опцией PRIVATE показаны не будут.
	
	Примеры:
	
	    LIST *!joeuser@foo.com
	        покажет все ники, маска которых совпадает с joeuser@foo.com
	    LIST *Bot*!*@*
	        покажет все зарегистрированные ники, которые содержат
	        подстроку вида Bot (регистр не имеет значения).
	    LIST *!*@*.bar.org
	        Покажет все ники, принадлежащие пользователям
	        в домене bar.org.

NICK_HELP_ALIST
	Синтаксис: ALIST [уровень]
	
	Покажет  все  каналы,  в  списках  доступа  которых  вы числитесь.
	Опционально, вы можете выбрать определенный уровень в формате xOP
	или LEVELS. Результирующий список будет содержать каналы, на которых
	у вас есть соответствующий уровень доступа.
	
	Примеры:
	     ALIST Founder
	        выведет список каналов, владельцем которых вы являетесь.
	     ALIST AOP
	        выведет список каналов, на которых у вас есть статус AOP
	        или выше.
	     ALIST 10
	        выведет список каналов, на которых ваш уровень равен 10
	        или выше.
	
	Примечание: если вы видите метку "!" перед именем канала, это
	значит что он находится в режиме NOEXPIRE.

NICK_HELP_GLIST
	Синтаксис: GLIST
	
	Выводит список ников в вашей группе.

NICK_HELP_STATUS
	Синтаксис: STATUS ник1 ник2 ник3...
	
	Запрашивает информацию о статусе ника на сервисах - идентифицировался
	ли использующий его как владелец. Ответ выводится в формате:
	
	    ник код-статуса
	
	где ник - это ник, статус которого вы запросили, а код-статуса
	может быть одним из:
	
	0 - такого пользователя нет в онлайне или этот ник не зарегистрирован
	1 - пользователь не идентифицирован как владелец ника
	2 - пользователь идентифицирован как владелец ника через список хостмасок
	3 - пользователь идентифицирован как владелец ника через идентификацию паролем
	
	Вы  можете запросить статус сразу у 16 ников (разделяя их пробелом),
	все лишние буду проигнорированы. Если параметр не указан - команда
	вернет ваш текущий статус на сервисах.

NICK_HELP_SENDPASS
	Синтаксис: SENDPASS ник
	
	Отправляет пароль ника на email-адрес, указанный при регистрации
	этого самого ника. Данная команда весьма полезна в случае "утери"
	пароля.
	
	Примечание: SENDPASS может быть ограничен для использования только
	IRC-операторами данной сети.
	Примечание 2: при включенном шифровании паролей эта команда недоступна.

NICK_HELP_CONFIRM
	Синтаксис: CONFIRM auth-код
	
	Данная команда необходима, что бы завершить второй шаг регистрации
	ника - подтверждение. После выполнения данного тербования, ваш ник
	будет зарегистрирован на %S.
	Кодовая фраза, которая указывается в качестве параметра, высылается
	на ваш e-mail в первой части процесса регистрации ника. Для более
	подробной информации см. %R%S HELP REGISTER

NICK_HELP_RESEND
	Синтаксис: RESEND
	
	Команда RESEND позволяет заново выслать новый auth-код (кодовую
	фразу) на email-адрес пользователя, использующего эту команду.

NICK_SERVADMIN_HELP
	 
	Администраторы сервисов могут удалить любой ник без идентификации
	к нему, а так же, могут смотреть список доступа любого ника
	(более подробно см. %R%S ACCESS LIST ник).

NICK_SERVADMIN_HELP_LOGOUT
	Синтаксис: LOGOUT [ник [REVALIDATE]]
	
	Использованная без параметров, команда LOGOUT производит эффект,
	обратный эффекту команды IDENTIFY, то есть - деидентифицирует вас
	от используемого на данный момент ника. Тем не менее, сервисы не
	пошлют повторный запрос на идентификацию.
	
	Если в качестве параметра вы укажете ник - деидентифицирует указанный
	ник. Дополнительно указанный параметр REVALIDATE заставит сервисы
	послать дендинтифицированному нику запрос на переавторизацию.
	Использовать параметры ник и REVALIDATE могут лишь Администраторы
	сервисов.

NICK_SERVADMIN_HELP_DROP
	Синтаксис: DROP [ник]
	
	Без параметров, удаляет Ваш ник из базы данных %S.
	
	С параметром, удаляет указанный ник из базы данных. Вы можете удалить
	любой ник из вашей группы, не имея каких-либо особых привилегий.
	Удалить абсолютно любой ник могут только Администраторы сервисов.

NICK_SERVADMIN_HELP_INFO
	
	Администраторы сервисов могу использовать параметр ALL для любого
	ника.

NICK_SERVADMIN_HELP_LIST
	Синтаксис: LIST маска [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	Показывает список всех зарегистрированных ников, которые совпадают
	с указанной в качестве основного  параметра маской.  Формат маски:
	ник!идент@хост допустимо использование  подстановочных символов.
	Ники,  у которых включена  опция PRIVATE, будут  показаны только
	Администраторам  сервисов. Ники с  включенным параметром NOEXPIRE
	будут показаны с префиксом !.
	
	В  качестве  дополнительных  опций  команды  LIST вы можете указать
	FORBIDDEN,  SUSPENDED,  NOEXPIRE или  UNCONFIRMED. Это позволит вам
	получить список ников, отвечающих определенным критериям, таким как:
	FORBIDDEN  -  ники, запрещенные к использованию, SUSPENDED  -  ники,
	"замороженные" до поры до времени, UNCONFIRMED  -  ники, регистрация
	которых все еще  не подтверждена, и NOEXPIRE  -  ники, находящиеся в
	режиме "не-истечения". Если указаны все четыре опции,  то все 4 типа
	ников будут перечислены в списке.
	
	Примеры:
	
	    LIST *!joeuser@foo.com
	       покажет все ники, маска которых совпадает с joeuser@foo.com

	    LIST *Bot*!*@*
	       покажет все зарегистрированные ники, которые содержат слово
	       Bot (вне зависимости от регистра).

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
	
	Без параметров, покажет список ников в вашей группе. С параметром,
	покажет все ники находящиеся в группе указанного ника.

	Для использования этой команды необходимы права Администратора сервисов.

NICK_SERVADMIN_HELP_GETPASS
	Синтаксис: GETPASS ник
	
	Позволяет получить пароль указанного ника.
	Примечание: сразу по использовании данной команды, сервисы пошлют
	по WALLOPS/GLOBOPS сообщение, содержащие ник использовавшего команду,
	и ник, относительно которого была использована данная команда.
	Одновременно с этим, событие будет записано в лог-файл.

	Примечание 2: команда недоступна, если включено шифрование паролей.

NICK_SERVADMIN_HELP_GETEMAIL
	Синтаксис: GETEMAIL user@emailhost

	Выводит список ников, в настройках у которых стоит указанный email.
	Примечание: вы не можете использовать символьные маски ни для user,
	ни для emailhost. Каждый раз, при использовании данной команды,
	сообщение, включающее ник вызвавшего команду и указанный email,
	будет записано в лог-файл сервисов.

NICK_SERVADMIN_HELP_FORBID
	Синтаксис: FORBID ник [причина]
	
	Запрещает регистрировать и/или использовать указанный ник. Эффект
	запрета модет быть снят путем удаления ника (DROP).
	
	В некоторых сетях, причина является обязательным параметром.

NICK_SERVADMIN_HELP_SUSPEND
	Синтаксис: SUSPEND ник причина

	Устанавливает режим саспенда ("заморозки") на ник, предотвращая тем
	самым возможность идентификации к нему или использования.
	Примечание: в режиме саспенда ник исключен из условий истечения по
	времени.

NICK_SERVADMIN_HELP_UNSUSPEND
	Syntax: UNSUSPEND ник

	Освобождает ник из режима саспенда (заморозки).

###########################################################################
#
# ChanServ help messages
#
###########################################################################

CHAN_HELP_CMD_GETPASS
	    GETPASS    Запрос пароля от канала
CHAN_HELP_CMD_FORBID
	    FORBID     Установка запрета на использование/регистрацию канала
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    Приостановка работы канала, с сохранением всех данных
	               и настроек канала
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Снятие режима саспенда с канала (восстановление)
CHAN_HELP_CMD_STATUS
	    STATUS     Возвращает текущий уровень доступа для указанного
	               пользователя на указанном канале
CHAN_HELP_CMD_REGISTER
	    REGISTER   Регистрация канала
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   Идентифицикация к каналу в качестве владельца
CHAN_HELP_CMD_SET
	    SET        Настройка различных опций канала
CHAN_HELP_CMD_AOP
	    AOP        Управление списком AOP'ов канала
CHAN_HELP_CMD_SOP
	    SOP        Управление списком SOP'ов канала
CHAN_HELP_CMD_ACCESS
	    ACCESS     Управление ACCESS-списком привилегий канала
CHAN_HELP_CMD_LEVELS
	    LEVELS     Переустановка значения уровней доступа канала
CHAN_HELP_CMD_AKICK
	    AKICK      Управление списком автокиков канала
CHAN_HELP_CMD_DROP
	    DROP       Удаление канала из базы данных сервисов
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Восстановление 'забытого' пароля от канала
CHAN_HELP_CMD_BAN
	    BAN        Установка бана на канале
CHAN_HELP_CMD_CLEAR
	    CLEAR      Очистка определенных режимов канала
CHAN_HELP_CMD_DEVOICE
	    DEVOICE    Снятие режима войса (-v) с указанного ника на канале
CHAN_HELP_CMD_GETKEY
	    GETKEY     Запрос установленного на канале ключа (+k)
CHAN_HELP_CMD_INFO
	    INFO       Запрос информации об указанном канале
CHAN_HELP_CMD_INVITE
	    INVITE     Tells ChanServ to invite you into a channel
CHAN_HELP_CMD_KICK
	    KICK       Выкидывает указанного пользователя с канала
CHAN_HELP_CMD_LIST
	    LIST       Запрос списка всех зарегистрированных каналов
CHAN_HELP_CMD_LOGOUT
	    LOGOUT     Деидентификация указанного пользователя от канала
CHAN_HELP_CMD_OP
	    OP         Установка статуса оператора (+o) указанному нику на канале
CHAN_HELP_CMD_TOPIC
	    TOPIC      Установка топика на канале посредством сервисов
CHAN_HELP_CMD_UNBAN
	    UNBAN      Удаление на канале всех банов, которые не позволяют вам войти
CHAN_HELP_CMD_VOICE
	    VOICE      Установка статуса войса (+v) указанному нику на канале
CHAN_HELP_CMD_VOP
	    VOP        Управление списком VOP'ов канала
CHAN_HELP_CMD_DEHALFOP
	    DEHALFOP   Снятие режима полуоператора (-h) с указанного ника на канале
CHAN_HELP_CMD_DEOWNER
	    DEOWNER    Снимает с вас статус владельца канала
CHAN_HELP_CMD_DEPROTECT
	    DEPROTECT  Снятие режима защиты (+a) с указанного ника на канале
CHAN_HELP_CMD_HALFOP
	    HALFOP     Установка статуса полуоператора (+h) указанному нику на канале
CHAN_HELP_CMD_HOP
	    HOP        Управление списком HOP'ов канале
CHAN_HELP_CMD_OWNER
	    OWNER      Запрос статуса владельца на канале
CHAN_HELP_CMD_PROTECT
	    PROTECT    Установка режима защиты (+a) указанному нику на канале
CHAN_HELP_CMD_ADMIN
	    ADMIN      Установка статуса администратора (+a) указанному нику на канале
CHAN_HELP_CMD_DEADMIN
	    DEADMIN    Снятие режима администратора (-a) с указанного ника на канале
CHAN_HELP_CMD_DEOP
	    DEOP       Снятие режима оператора (-o) с указанного ника на канале

CHAN_HELP
	%S позволяет вам регистрировать канал и управлять его различными
	настройками. Также, %S поможет вам обезопасить ваш канал от
	злонамеренных пользователей, желающих его "захватить", используя
	ограничение возможности получения статуса опа на канале и другие
	настройки безопасности. Список доступных команд представлен ниже,
	чтобы использовать их, пошлите запрос вида
	%R%S команда
	Для более подробной информации о конкретной команде, воспользуйтесь:
	%R%S HELP команда
	 
	
CHAN_HELP_EXPIRES
	
	Помните, что любой канал, не использующийся в течение %d дней
	(т.е. ни один пользователь, прописанный в списке доступа канала,
	не посетит канал в течении данного времени), будет автоматически
	удален из базы данных сервисов.

CHAN_HELP_REGISTER
	Синтаксис: REGISTER #канал пароль описание
	
	Регистрирует канал в базе данных %S. Чтобы использовать эту
	команду, вы должны быть оператором того канала, который вы пытаетесь
	зарегистрировать. Указанный при регистрации пароль можно использовать
	(командой IDENTIFY) для получения доступа к каналу и его различным
	настройкам.
	Последний параметр описание, который обязательно должен быть
	указан, является описанием канала и/или, возможно, его тематикой.
	
	Сразу после регистрации канала, вы будете автоматически внесены в
	список доступа с уровенем founder (основатель, владелец). Владелец
	канала имеет возможность изменять любые его настройки, а так же,
	автоматически получать статус опператора от %S при заходе
	на канал.
	Помимо всего этого, как владелец, вы можете воспользоваться командой
	ACCESS, что бы назначить дополнительных операторов канала, т. н.
	администрацию. Более подробную информацию о системе привелегий вы
	можете получить по команде %R%S HELP ACCESS
	
	Примечание: учтите, что перед тем как регистрировать свой канал, вы
	должны сперва зарегистрировать свой ник. Более подробная информация
	о том, как регистрировать ник, доступна по %R%s HELP

CHAN_HELP_IDENTIFY
	Синтаксис: IDENTIFY #канал пароль
	
	Позволяет идентифицироваться на %S в качестве владельца
	указанного канала. Многие команды по управлению каналом недоступны
	до тех пор, пока вы не идентифицируетесь к каналу. В качестве пароля
	необходимо указывать тот самый пароль, который вы указывали при
	регистрации вашего канала.
	Примечание: вам необязательно идентифицироваться к каналу, если вы
	его реальный владелец, достаточно просто идентифицироваться к нику.
	
CHAN_HELP_LOGOUT
	Синтаксис: LOGOUT #канал ник
	
	Данная команда позволяет деидентифицировать любой ник, который
	с помощью пароля идентифицировался к каналу в качестве владелеца.
	
	Если вы являетесь реальным владельцем канала, у вас есть возможность
	деидентифицировать кого угодно, в ином случае - только себя.

CHAN_HELP_DROP
	Синтаксис: DROP #канал
	
	Удаляет указанный канал из базы данных сервисов. Использовать DROP
	может лишь владелец канала, предварительно проидентифицировавшись
	к нему с помощью команды IDENTIFY.

CHAN_HELP_SET
	Синтаксис: SET #канал опция параметр
	
	Позволяет владельцу канала менять различные настройки канала.
	Доступные опции:
	
	    FOUNDER       смена владельца канала
	    SUCCESSOR     установка наследника канала
	    PASSWORD      смена пароля канала
	    DESC          изменение описания канала
	    URL           установка URL-адреса канала
	    EMAIL         установка Email-адреса канала
	    ENTRYMSG      установка автосообщения при входе на канал
	    BANTYPE       установка типа бана сервисами на канале
	    MLOCK         блокировка определенных режимов канала
	    KEEPTOPIC     активирование автосохранения топика канала
	    OPNOTICE      активирование уведомлений об использовании
	                  команд OP/DEOP
	    PEACE         активирование режима "спокойствия" на канале
	    PRIVATE       скрытие канала в списке каналов выводимых по LIST
	    RESTRICTED    ограничение уровня доступа на канал до прописанных
	    SECURE        дополнительные возможности %S'а по безопасности
	    SECUREOPS     контроль за получением статуса оператора канала
	    SECUREFOUNDER ограничение на получение статуса владельца канала
	    SIGNKICK      настройка режима "подписанных киков"
	    TOPICLOCK     блокировка топика канала от изменений его кем-либо
	    XOP           настройка системы привилегий канала
	
	Для более подробной информации о какой-либо конкретной опции, см.
	справку по %R%S HELP опция

CHAN_HELP_SET_FOUNDER
	Синтаксис: SET #канал FOUNDER ник
	
	Позволяет передать права на владение каналом другому пользователю.
	Ник нового владельца канала должен быть зарегистрированным.

CHAN_HELP_SET_SUCCESSOR
	Синтаксис: SET #канал SUCCESSOR ник
	
	Позволяет назначить наследника канала. Если ник текущего владельца
	канала будет удален ввиду истечения срока регистрации или по каким
	иным причинам, назначенный наследник станет новым владельцем канала.
	Так же, стоит заметить, что если наследник уже зарегистрировал
	чересчур много каналов (более %d), он не получит прав на владение
	каналом и тот будет удален. Ко всему прочему, ник наследника канала
	должен быть зарегистрированным ником.

CHAN_HELP_SET_PASSWORD
	Синтаксис: SET #канал PASSWORD пароль
	
	Позволяет изменить текущий пароль канала, используемый для
	идентификации к нему (каналу) в качестве владельца.

CHAN_HELP_SET_DESC
	Синтаксис: SET #канал DESC описание
	
	Позволяет установить описание канала, которое будет показано при
	использовании команд LIST и INFO.

CHAN_HELP_SET_URL
	Синтаксис: SET #канал URL [адрес]
	
	Позволяет установить URL-адрес канала. Данный URL будет показан
	тогда, когда кто-либо запросит информацию о канале с помощью
	команды INFO. Чтобы удалить установленный URL-адрес, используйте
	данную команду без параметра.

CHAN_HELP_SET_EMAIL
	Синтаксис: SET #канал EMAIL [адрес@почты]
	
	Позволяет установить адрес электронной почты канала. Этот адрес
	будет показан тогда, когда кто-либо запросит информацию о канале
	с помощью команды INFO. Чтобы удалить установленный email-адрес,
	используйте данную команду без параметра.

CHAN_HELP_SET_ENTRYMSG
	Синтаксис: SET #канал ENTRYMSG [текст]
	
	Позволяет  установить  "сообщение  при  входе",  которое  будет
	отправлено нотисом (notice) каждому вошедшему на канал пользователю.
	Если  использовать  команду без параметров  -  текущее установленное
	сообщение будет удалено.

CHAN_HELP_SET_BANTYPE
	Синтаксис: SET #канал BANTYPE тип_бана
	
	Позволяет установить шаблон банмаски, по которой будет забанен
	пользователь канала, относительно которого применяется сервисная
	команда BAN.
	
	Значение типа бана должно быть кокретным числом, от 0 до 3
	включительно. Допустимые значения представлены ниже:
	
	0 - бан вида *!идент@хост
	1 - бан вида *!*идент@хост
	2 - бан вида *!*@хост
	3 - бан вида *!*идент@*.домен

CHAN_HELP_SET_KEEPTOPIC
	Синтаксис: SET #канал KEEPTOPIC {ON | OFF}
	
	Позволяет активировать/деактивировать опцию хранения топика на
	канале. При включенном режиме хранения топика, текст топика канала
	будет сохраняться в базе данных %S после того, как последний
	пользователь покинет канал, и автоматически восстанавливаться сразу
	же, как только канал снова станет активным (то есть, на него опять
	кто-либо зайдет).

CHAN_HELP_SET_TOPICLOCK
	Синтаксис: SET #канал TOPICLOCK {ON | OFF}
	
	Включает или выключает опцию блокировки топика на канале. При
	включенном режиме блокировки топика, %S разрешит менять
	топик канала только через сервисную команду TOPIC.

CHAN_HELP_SET_MLOCK
	Синтаксис: SET #канал MLOCK режимы
	
	MLOCK позволяет вам установить и заблокировать определенные режимы
	канала. Вы  можете  заблокировать от снятия/установки любые режимы,
	какие только захотите - %S никому не позволит изменить их.
	
	В качестве параметра команды, вы должны указать режимы в том же
	формате, в  котором  указываете  при  ручной установке, с помощью
	команды /MODE - режимы с  префиксом + считаются включенными, и,
	соответственно, блокируются от снятия, а  режимы  с  префиксом -
	считаются выключенными и блокируются от установки.
	Примечание: однако, вы должны помнить, что повторное использование
	SET MLOCK удалит все предыдущие заблокированные режимы и заменит
	их новыми!
	
	Внимание: если вы установили и заблокировали ключ канала, как во
	втором примере ниже, вы должны также установить опцию RESTRICTED
	на канал (см. HELP SET RESTRICTED), или любой, кто зайдет на канал
	когда тот будет пуст, сможет увидеть установленный ключ!
	
	Примеры:
	
	    SET #канал MLOCK +nt-iklps
	        блокирует включенными режимы n и t, и выключенными i, k, l,
	        p, и s. Режим m остается свободным к установке/снятию.

	    SET #канал MLOCK +knst-ilmp мой-ключ-пароль
	        блокирует включенным режимы k, n, s, и t, и выключенными
	        i, l, m, и p. Такжеб устанавливает ключ для входа на канал
	        ("мой-ключ-пароль").

	    SET #канал MLOCK +
	        удаляет блокировку режимов. Все режимы канала могут быть
	        свободно изменены любым оператором канала.

CHAN_HELP_SET_PEACE
	Синтаксис: SET #канал PEACE {ON | OFF}
	
	Активирует/деактивирует опцию спокойствия на канале. Данный режим
	позволяет запретить использование таких команд %S'а как BAN,
	OP, DEOP, KICK и т. д. относительно тех, кто выше или равен по
	статусу относительно использующего.
	Примечание: тем не менее любой, имеющий доступ к этим командам,
	сможет использовать их относительно себя.

CHAN_HELP_SET_PRIVATE
	Синтаксис: SET #канал PRIVATE {ON | OFF}
	
	Активирует/деактивирует опцию приватности канала. Данный режим
	позволяет скрыть ваш канал в списке каналов по %R%S LIST.

CHAN_HELP_SET_RESTRICTED
	Синтаксис: SET #канал RESTRICTED {ON | OFF}
	
	Enables or disables the restricted access option for a
	channel.  When restricted access is set, users not on
	the access list will instead be kicked and banned from the channel.

CHAN_HELP_SET_SECURE
	Синтаксис: SET #канал SECURE {ON | OFF}
	
	Активирует/деактивирует режим безопасности для канала.
	Когда SECURE включено, только пользователи с зарегистрированными
	никами, проидентифицировавшиеся к ним через пароль и включенные в
	список доступа канала, смогут запросить статус на канале с помощью
	%S.
	Если безопасность отключена, пользователи, прописанные на канале,
	смогут запросить статус даже если имеют авторизацию к своему нику
	всего лишь через список масок на %s.

CHAN_HELP_SET_SECUREOPS
	Синтаксис: SET #канал SECUREOPS {ON | OFF}
	
	Активирует/деактивирует режим ограничения на получение статуса
	оператора канала. Данный  режим  позволяет  запретить возможность
	получения статуса опа тем пользователям, которые не присутствуют
	в  списке  доступа  канала  или  права  которых не соответствуют
	получаемому статусу.

CHAN_HELP_SET_SECUREFOUNDER
	Синтаксис: SET #канал SECUREFOUNDER {ON | OFF}
	
	Активирует/деактивирует режим безопасности владельца на канале.
	Активирование SECUREFOUNDER позволяет вам ограничить возможность
	удаления канала, изменения его пароля, владельца или наследника до
	реального владельца, прописанного в графе Founder (Владелец канала).
	(и не важно, кто там идентифицировался к каналу через %S ID).

CHAN_HELP_SET_SIGNKICK
	Синтаксис: SET #канал SIGNKICK {ON | LEVEL | OFF}
	
	Включает или выключает опцию "подписанных киков" на канале.
	Активирование режима SIGNKICK заставит %S автоматически
	добавлять к причине кика ник того, кто использовал команду KICK.
	
	В качестве дополнительного параметра для опции SIGNKICK вы можете
	указать значение LEVEL. Это  позволит  более  тонко  лимитировать
	действие данного режима, при  условии  конечно,  что вы используете
	систему LEVELS на канале (см. %R%S HELP LEVELS для
	более подробной информации).

CHAN_HELP_SET_XOP
	Синтаксис: SET #канал XOP {ON | OFF}
	
	Позволяет активировать/деактивировать систему привелегий xOP на
	вашем канале. Особенности данной системы состоят в том, что все
	манипуляции со списком  доступа канала производятся посредством
	использования готовых шаблонов:  SOP,  AOP, HOP и VOP. Чтобы
	внести  какие-либо  изменения в список доступа, вам достаточно
	использовать одноименные команды, вместо того, что бы разбираться
	в многоуровневой системе LEVELS.
	
	Примечание: когда вы переключитесь с системы ACCESS на систему
	привилегий xOP, все ваши предыдущие установки уровней для канала
	и уровни доступа пользователей будут изменены на стандартные и вы
	никак не сможете вернуть эти значения, переключившись обратно на
	систему привилегий ACCESS!
	
	После перехода с системы ACCESS на систему xOP, удостоверьтесь, что
	у всех ваших пользователей, прописанных на канале, выставлен нужный
	уровень доступа, ибо переход из  ACCESS в  xOP не всегда происходит
	так, как нужно. Более  того,  если  вы  уже  настроили определенные
	значения LEVELS  для  вашего канала, изменять систему привилегий на
	xOP-совместимую не рекомендуется.
	
	Переключение из системы  xOP в систему  ACCESS  обычно проходит без
	каких-либо проблем.

CHAN_HELP_SET_OPNOTICE
	Синтаксис: SET #канал OPNOTICE {ON | OFF}
	
	Включает или выключает опцию OP-уведомления на вашем канале.
	Когда оп-уведомление включено, %S будет посылать нотис
	на канал каждый раз, когда кто-либо использует команды OP/DEOP.

CHAN_HELP_AOP
	Синтаксис: AOP #канал ADD ник
	           AOP #канал DEL {ник | номер_записи | список_записей}
	           AOP #канал LIST [маска | список_записей]
	           AOP #канал CLEAR
	
	Позволяет управлять списком AOP'ов (автоператоров) вашего канала.
	Привилегии AOP'а дают пользователю возможность получить статус опа
	сразу при входе на канал, возможность устанавливать/снимать баны
	посредством сервисов, приглашать себя на канал, запрашивать ключ
	канала и многое другое.
	
	Команда AOP ADD добавляет указанный ник в список AOP'ов.
	
	Команда AOP DEL удаляет указанный ник из списка AOP'ов. В качестве
	параметра вы можете указать не только ник, но и конкретный номер
	записи или список записей (см. примеры использования LIST ниже).
	
	Команда AOP LIST выводит текущий список AOP'ов канала. В качестве
	дополнительного параметра вы можете указать символьную маску, что
	позволит вам получить список с конкретными никами, попадающими под
	эту маску, или же, вы можете указать список записей.
	
	Например:
	
	   AOP #канал LIST 2-5,7-9
	      отобразит все записи со 2-ой по 5-ую, и с 7-ой по 9-ю.

	   AOP #канал LIST *vas*
	      отобразит все записи, которые содержат подстроку "vas"
	
	Команда AOP CLEAR позволяет полностью очистить список AOP'ов.
	
	Команды AOP ADD и AOP DEL могут использовать только SOP'ы канала
	или владелец, команду AOP CLEAR может использовать только владелец.
	Команда AOP LIST доступна любому пользователю из списка AOP'ов.
	
	Помните, что данная команда работает  только в случае использования
	системы привилегий xOP. Если вы используете систему LEVELS, все
	изменения списка  доступа  канала  должны  производится посредством
	команды ACCESS. Для  более  подробной информации по использованию
	команды ACCESS и системы привилегий xOP см. справочную информацию
	по %R%S HELP ACCESS и %R%S HELP SET XOP.

CHAN_HELP_HOP
	Синтаксис: HOP #канал ADD ник
	           HOP #канал DEL {ник | номер_записи | список_записей}
	           HOP #канал LIST [маска | список_записей]
	           HOP #канал CLEAR
	
	Позволяет управлять списком HOP'ов (полуоператоров) вашего канала.
	Привилегии HOP'а дают пользователю возможность получить статус хопа
	сразу при входе на канал, или запросить его вручную, через сервисы.
	
	Команда HOP ADD добавляет указанный ник в список HOP'ов.
	
	Команда HOP DEL удаляет указанный ник из списка HOP'ов. В качестве
	параметра вы можете указать не только ник, но и конкретный номер
	записи или список записей (см. примеры использования LIST ниже).
	
	Команда HOP LIST выводит текущий список HOP'ов канала. В качестве
	дополнительного параметра, вы можете указать символьную маску, что
	позволит вам получить список с конкретными никами, попадающими под
	эту маску, или же, вы можете указать список записей.
	
	Например:
	
	   HOP #канал LIST 2-5,7-9
	      отобразит все записи со 2-ой по 5-ую, и с 7-ой по 9-ю.

	   HOP #канал LIST *vas*
	      отобразит все записи, которые содержат подстроку "vas"
	
	Команда HOP CLEAR позволяет полностью очистить список HOP'ов.
	
	Команды HOP ADD, HOP DEL и HOP LIST могут использовать AOP'ы
	канала или выше, команду HOP CLEAR может использовать только
	владелец канала.
	
	Помните, что данная команда работает  только в случае использования
	системы привилегий xOP. Если вы используете систему LEVELS, все
	изменения списка  доступа  канала  должны  производится посредством
	команды ACCESS. Для  более  подробной информации по использованию
	команды ACCESS и системы привилегий xOP см. справочную информацию
	по %R%S HELP ACCESS и %R%S HELP SET XOP.

CHAN_HELP_SOP
	Синтаксис: SOP #канал ADD ник
	           SOP #канал DEL {ник | номер записи | список_записей}
	           SOP #канал LIST [маска | список_записей]
	           SOP #канал CLEAR
	
	Позволяет управлять списком SOP'ов (супероператоров) вашего канала.
	Привилегии SOP'а включают в себя не только все привилегии AOP'а,  но
	и возможность  управлять списком доступа канала,  списком  автокиков,
	списком "плохих слов" и списком мемо-сообщений.
	
	Команда SOP ADD добавляет указанный ник в список SOP'ов.
	
	Команда SOP DEL удаляет указанный ник из списка SOP'ов. В качестве
	параметра вы можете указать не только ник, но и конкретный номер
	записи или список записей (см. примеры использования LIST ниже).
	
	Команда SOP LIST выводит текущий список SOP'ов канала. В качестве
	дополнительного параметра вы можете указать символьную маску, что
	позволит вам получить список с конкретными никами, попадающими под
	эту маску, или же, вы можете указать список записей.
	
	Например:
	
	   SOP #канал LIST 2-5,7-9
	      отобразит все записи со 2-ой по 5-ую, и с 7-ой по 9-ю.

	   SOP #канал LIST *vas*
	      отобразит все записи, которые содержат подстроку 'vas'
	
	Команда SOP CLEAR позволяет полностью очистить список SOP'ов.
	
	Команды SOP ADD, SOP DEL и SOP CLEAR может использовать только
	владелец канала, команда SOP LIST доступна всем пользователям с
	привилегиями AOP'а и выше.
	
	Помните, что данная команда работает  только в случае использования
	системы привилегий xOP. Если вы используете систему LEVELS, все
	изменения списка  доступа  канала  должны  производится посредством
	команды ACCESS. Для  более  подробной информации по использованию
	команды ACCESS и системы привилегий xOP см. справочную информацию
	по %R%S HELP ACCESS и %R%S HELP SET XOP.

CHAN_HELP_VOP
	Синтаксис: VOP #канал ADD ник
	           VOP #канал DEL {ник | номер_записи | список_записей}
	           VOP #канал LIST [маска | список_записей]
	           VOP #канал CLEAR
	
	Позволяет  управлять  списком  VOP'ов  (автовойсов)  вашего канала.
	Привилегии VOP'а дают пользователю возможность получить статус войса
	сразу  при  входе на канал, или запросить его вручную, через сервисы.
	
	Команда VOP ADD добавляет указанный ник в список HOP'ов.
	
	Команда VOP DEL удаляет указанный ник из списка VOP'ов. В качестве
	параметра  вы  можете указать не только ник, но и конкретный номер
	записи  или  список записей (см. примеры использования LIST ниже).
	
	Команда VOP LIST выводит текущий список VOP'ов канала. В качестве
	дополнительного параметра, вы можете указать символьную маску, что
	позволит вам получить список с конкретными никами, попадающими под
	эту маску, или же, вы можете указать список записей.
	
	Например:
	
	   VOP #канал LIST 2-5,7-9
	      отобразит все записи со 2-ой по 5-ую, и с 7-ой по 9-ю.

	   VOP #канал LIST *vas*
	      отобразит все записи, которые содержат подстроку 'vas'
	
	Команда VOP CLEAR позволяет полностью очистить список VOP'ов.
	
	Команды VOP ADD, VOP DEL и VOP LIST могут использовать AOP'ы
	канала или выше, команду VOP CLEAR может использовать только
	владелец канала.
	
	Помните, что данная команда работает  только в случае использования
	системы привилегий xOP. Если вы используете систему LEVELS, все
	изменения списка  доступа  канала  должны  производится посредством
	команды ACCESS. Для  более  подробной информации по использованию
	команды ACCESS и системы привилегий xOP см. справочную информацию
	по %R%S HELP ACCESS и %R%S HELP SET XOP.

CHAN_HELP_ACCESS
	Синтаксис: ACCESS #канал ADD ник уровень
	           ACCESS #канал DEL {ник | номер_записи | список_записей}
	           ACCESS #канал LIST [маска | список_записей]
	           ACCESS #канал CLEAR
	
	Позволяет  управлять  списком доступа  (ACCESS-списком)  канала.
	Данный список включает в себя всех пользователей,  которые имеют те
	или иные привилегии на канале, например: доступ к статусу оператора,
	доступ к управлению списком AKICK'ов, доступ  к  таким командам как
	KICK, BAN, VOICE, INVITE и т. д.
	Различные уровни доступа определяют различные привилегии на канале.
	Для более подробной информации о стандартных уровнях доступа канала
	см. %R%S HELP ACCESS LEVELS
	Примечание: любой,  не  включенный в список доступа пользователь,
	считается пользователем с уровнем доступа 0.
	
	Команда  ACCESS ADD  добавляет указанный ник в список доступа
	канала с указанным уровнем. Если этот ник уже числится в списке,
	его  уровень будет изменен на тот,  который был указан.  Значение
	уровня должно быть меньше, чем уровень пользователя, использующего
	команду ADD, и, если ник уже присутствует  в  списке  доступа,
	текущий уровень доступа указанного ника должен быть меньше, чем
	уровень доступа пользователя, использующего данную команду.
	
	Команда ACCESS DEL удаляет указанный ник из списка доступа.  Если
	в качестве параметра указан номер записи, или список записей - все
	записи под данными номерами будут удалены (см. пример использования
	команды LIST ниже).
	
	Команда ACCESS LIST выводит текущий список список  доступа канала.
	В качестве дополнительного параметра можно указать символьную маску,
	что позволит получить список с конкретными никами, попадающими под
	эту маску, или же, можно указать список записей.
	
	Например:
	
	   ACCESS #канал LIST 2-5,7-9
	      отобразит все записи со 2-ой по 5-ую, и с 7-ой по 9-ю.
	
	Команда ACCESS CLEAR позволяет полностью очистить список доступа.

CHAN_HELP_ACCESS_LEVELS
	Уровни доступа пользователей
	
	По-умолчанию, определены нижеследующие значения уровней доступа:
	
	   Founder - полный доступ к всем функциям %S на своем канале,
	             автоматическое получение  статуса оператора  при входе
	             на канал. Помните, что только один человек может иметь
	             статус владельца (и это никак нельзя изменить командой
	             ACCESS).
	        10 - Доступ к команде AKICK, автооп.
	         5 - Автооп.
	         3 - Автовойс.
	         0 - Никаких особых привилегий, может получить статус опа
	             от другого оператора канала
	             (только если режим ограниченного статуса отключен).
	        <0 - Автодеоп. Статус опа будет сразу же сниматься сервисами.
	
	С  помощью команды  LEVELS  вы можете переназначить уровни доступа
	по своему вкусу. Для более подробной информации о настройке уровней
	доступа см. %R%S HELP LEVELS.

CHAN_HELP_AKICK
	Синтаксис: AKICK #канал ADD {ник | маска} [причина]
	           AKICK #канал STICK маска
	           AKICK #канал UNSTICK маска
	           AKICK #канал DEL {ник | маска | номер_записи | список_записей}
	           AKICK #канал LIST {маска | номер_записи | список записей}
	           AKICK #канал VIEW {маска | номер_записи | список записей}
	           AKICK #канал ENFORCE
	           AKICK #канал CLEAR
	
	Позволяет управлять AKICK-списком канала. Любой пользователь, чья
	хостмаска  совпадет  с  одной  из  записей  в AKICK-списке, будет
	немедленно забанен и выкинут с канала  при  попытке на него зайти.
	Все списки автокиков хранятся на %S,  для  каждого канала -
	свой список.
	
	Команда AKICK ADD добавляет ник или маску в список автокиков.
	Дополнительно, вы можете указать причину  -  этот текст будет
	использован при кике пользователя  с канала.  Если  причина не
	указана - будет использован стандартный текст из конфигурационного
	файла сервисов (User has been banned from the channel).
	
	Команда AKICK STICK позволяет создать постоянный бан для указанной
	маски из списка автокиков. Данный бан всегда будет присутсвовать на
	канале, %S никому не позволит снять его.
	Примечание: установка  stick-записи  на  зарегистрированный ник -
	невозможна.
	
	Команда AKICK UNSTICK позволяет удалить  stick-запись, однако бан
	канала вы должны удалить вручную.
	
	Команда  AKICK DEL  удаляет  указанный  ник  или  маску из списка
	автокиков. В качестве  дополнительного параметра, вы можете указать
	номер записи или список записей. 
	Примечание: удаление  записи  из  AKICK-списка  не удаляет бан на
	канале, вы должны будете снять его вручную.
	
	Команда  AKICK LIST  показывает  текущий  список автокиков канала.
	В качестве дополнительного параметра вы можете указать маску, чтобы
	получить только совпадающие записи, или номер/список записей, чтобы
	просмотреть только эти записи.
	
	Команда  AKICK VIEW  является  более  подробным аналогом команды
	AKICK LIST (дополнительно показывается время установки AKICK'а и
	его причина).
	
	Команда AKICK ENFORCE посылает %S запрос на немедленную перепроверку
	AKICK-списка канала.
	
	Команда AKICK CLEAR полностью очищает список автокиков канала.

CHAN_HELP_LEVELS
	Синтаксис: LEVELS #канал SET тип уровень
	           LEVELS #канал {DIS | DISABLE} тип
	           LEVELS #канал LIST
	           LEVELS #канал RESET
	
	Команда  LEVELS  позволяет удобно управлять уровнями доступа,
	используемыми для списков доступа.  С помощью этой эту команды,
	вы можете определить уровень доступа, требуемый для большинства
	функций %S'а (команды SET FOUNDER и  SET PASSWORD,
	а также эту команду, может использовать только владелец канала).
	
	Команда  LEVELS SET  позволяет  установить  уровень  доступа для
	функции или группы функций.
	Команда  LEVELS DISABLE  (DIS - сокращенный аналог)  отключает
	функцию или запрещает доступ к функции всем, кроме владельца канала.
	
	Команда  LEVELS LIST  покажет  текущие  уровни доступа для каждой
	функции или группы функций.
	
	Команда  LEVELS RESET  сбросит  все  настройки уровней доступа на
	значения по-умолчанию (прописаные в HELP ACCESS LEVELS).
	
	Чтобы посмотреть список функций, на которые можно определять уровни
	доступа, см. справочную информацию по HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Ниже представлен список всех доступных команд/функций и их описание.
	Обратите внимание на функции NOJOIN и AUTODEOP - в отличие от всех
	других функций, их уровень является максимальным. Уровни всех других
	команд/функций расцениваются как минимальные (минимально необходимый
	уровень для использования).
	

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Синтаксис: INFO #канал [ALL]
	
	Позволяет получить подробную информацию о зарегистрированом канале,
	в частности такую как: ник владельца, время регистрации канала, дату
	его последнего использования и описание. С параметром ALL будут
	так же показаны заблокированные режимы, опции канала и его наследник.
	Примечание: по-умолчанию, использовать команду с параметром ALL
	может только владелец канала.

CHAN_HELP_LIST
	Синтаксис: LIST маска
	
	Выводит  список всех зарегистрированных каналов,  название  которых
	совпадает с указанной маской. Помимо маски вы можете указать список
	записей,  например запрос #1-5 покажет все каналы  с 1-го  по 5-ый
	(каналы с включенной опцией PRIVATE показаны не будут).
	Примечание: при поиске каналов начинающихся с "#",  сам символ
	"#" указывать не надо - он зарезервирован для списка (см. выше).

CHAN_HELP_OP
	Синтаксис: OP [#канал [ник]]
	
	Позволяет установить статус оператора канала указанному нику на
	указанном канале.  Если  ник  не указан, статус оператора будет
	выдан использующему команду.  Если  не  указаны ни канал ни ник -
	установит статус оператора использующему команду на всех каналах,
	где он имеет соответствующий уровень доступа.
	
	По-умолчанию, данная команда может быть использована AOP'ами или
	лицами с уровнем доступа 5 и выше.

CHAN_HELP_DEOP
	Синтаксис: DEOP [#канал [ник]]
	
	Снимает  статус  оператора  канала  с  указанного  ника на указанном
	канале. Если ник не указан, снимает статус оператора с использующего
	команду.  Если ни канал ни ник не указаны - снимает статус оператора
	с  использующего  команду  на  всех  каналах, на которых у него есть
	соответствующий уровень доступа.
	
	По-умолчанию,  данная  команда  может  быть  использована AOP'ами или
	лицами с уровнем доступа 5 и выше.

CHAN_HELP_VOICE
	Синтаксис: VOICE [#канал [ник]]
	
	Позволяет  установить  статус  войса указанному нику на указанном
	канале. Если ник не указан, статус войса будет выдан использующему
	команду. Если не указаны ни канал ни ник  -  установит статус войса
	использующему команду на всех каналах, где он имеет соответствующий
	уровень доступа.
	
	По-умолчанию,  данная  команда может быть использована AOP'ами или
	лицами с уровнем доступа 5 и выше. Может быть использована VOP'ами
	или лицами с уровнем доступа 3 и выше (в последнем случае - только
	относительно себя).

CHAN_HELP_DEVOICE
	Синтаксис: DEVOICE [#канал [ник]]
	
	Снимает  статус войса с указанного ника на указанном канале.  Если ник
	не указан, снимает статус войса с использующего команду. Если ни канал
	ни ник не указаны  -  снимает  статус войса с использующего команду на
	всех каналах,  на  которых у него есть соответствующий уровень доступа.
	
	По-умолчанию,  данная  команда  может  быть  использована  AOP'ами или
	лицами  с  уровнем  доступа  5 и выше. Может быть использована VOP'ами
	или лицами  с  уровнем  доступа  3 и выше (в последнем случае - только
	относительно себя).

CHAN_HELP_HALFOP
	Синтаксис: HALFOP [#канал [ник]]
	
	Позволяет установить статус  полуоператора канала указанному нику
	на указанном канале.  Если  ник  не указан,  статус полуоператора
	будет выдан использующему команду. Если не указаны ни канал ни ник -
	установит статус полуоператора использующему команду на всех каналах,
	где он имеет соответствующий уровень доступа.
	
	По-умолчанию,  данная  команда  может быть использована AOP'ами или
	лицами с уровнем доступа  5 и выше. Может быть использована HOP'ами
	или лицами с уровнем доступа 4 и выше  (в последнем случае - только
	относительно себя).

CHAN_HELP_DEHALFOP
	Синтаксис: DEHALFOP [#канал [ник]]
	
	Снимает статус полуоператора с указанного ника на указанном канале.
	Если ник не  указан,  снимает статус полуоператора с использующего
	команду. Если ни канал ни ник не указаны -  снимает статус полуопа
	с использующего команду на всех каналах,  на  которых  у него есть
	соответствующий уровень доступа.
	
	По-умолчанию,  данная  команда может быть использована AOP'ами или
	лицами с уровнем доступа 5 и выше. Может быть использована HOP'ами
	или лицами с уровнем доступа 4 и выше (в последнем случае - только
	относительно себя).

CHAN_HELP_PROTECT
	Синтаксис: PROTECT [#канал [ник]]
	
	Позволяет установить статус защиты (администратора) канала указанному
	нику на указанном канале. Если ник не указан, статус защиты будет
	выдан использующему команду.  Если  не  указаны  ни  канал  ни  ник -
	установит статус защиты использующему команду на всех каналах, где он
	имеет соответствующий уровень доступа.
	
	По-умолчанию,  данная  команда  может быть использована владельцем или
	лицами с уровнем доступа 10 (SOP) и выше  (в последнем случае - только
	относительно себя).

CHAN_HELP_DEPROTECT
	Синтаксис: DEPROTECT [#канал [ник]]
	
	Снимает статус защиты (администратора) с указанного ника на указанном
	канале.  Если  ник  не  указан, снимает статус защиты с использующего
	команду. Если  ни  канал  ни  ник  не указаны - снимает статус защиты
	с  использующего  команду  на всех  каналах,  на  которых у него есть
	соответствующий уровень доступа.
	
	По-умолчанию,  данная  команда может быть использована владельцем или
	лицами с уровнем доступа 10 (SOP) и выше (в последнем случае - только
	относительно себя).

CHAN_HELP_OWNER
	Синтаксис: OWNER [#канал]
	
	Устанавливает статус владельца канала использующему команду. Если
	канал  не указан - устанавливает статус владельца на всех каналах,
	на которых  у  использующего есть соответствующий уровень доступа.
	
	По-умолчанию, данная команда может быть использована только
	владельцем канала.

CHAN_HELP_DEOWNER
	Синтаксис: DEOWNER [#канал]
	
	Снимает  с  использующего  статус владельца канала. Если канал не
	указан,  снимает с использующего команду статус владельца на всех
	каналах, на  которых у него есть соответствующий уровень доступа.
	
	По-умолчанию, данная команда может быть использована только
	владельцем канала.

CHAN_HELP_INVITE
	Синтаксис: INVITE #канал
	
	Заставляет %S пригласить вас на указанный канал.
	
	По-умолчанию, данная команда может быть использована AOP'ами или
	лицами с уровнем доступа 5 и выше.

CHAN_HELP_UNBAN
	Синтаксис: UNBAN #канал
	
	Посылает %S запрос на удаление с указанного канала всех банов,
	которые не позволяют вам на него (канал) зайти.
	
	По-умолчанию,  данная команда может быть использована AOP'ами
	или лицами с уровнем доступа 5 и выше.

CHAN_HELP_KICK
	Синтаксис: KICK [#канал [ник [причина]]]
	
	Выкидывает указанного пользователя с указанного канала. Если ник не
	указан - выкидывает вас.  Если канал и ник не указаны - выкинет вас
	со всех каналов,  на  которых  у  вас  есть соответствующий уровень
	доступа.
	
	По-умолчанию,  данная  команда  может быть использована AOP'ами или
	лицами с уровнем доступа 5 и выше.

CHAN_HELP_BAN
	Синтаксис: BAN [#канал [ник [причина]]]
	
	Банит указанного пользователя на указанном канале.  Шаблон банмаски
	будет  взят  из установки  BANTYPE.  Если ник не указан - сервисы
	забанят вас.  Если  канал и ник не указаны - сервисы забанят вас на
	всех каналах, где у вас есть соответствующий уровень доступа.
	
	По-умолчанию,  данная  команда  может  быть использована AOP'ами или
	лицами с уровнем доступа 5 и выше.

CHAN_HELP_TOPIC
	Синтаксис: TOPIC #канал [текст]
	
	Позволяет  изменить топик канала посредством %S. Если текст
	топика не указан - топик канала будет полностью удален. Чаще всего,
	применять  данную  команду  имеет  смысл при активированном режиме
	блокировки топика,  когда изменить топик канала обычным способом
	не представляется возможным.  Для  более  подробной  информации см.
	справку по %R%S HELP SET TOPICLOCK.
	
	По-умолчанию, данная команда может быть использована только
	владельцем канала.

CHAN_HELP_CLEAR
	Синтаксис: CLEAR #канал что_именно
	
	Посылает %S запрос на очистку определеных режимов канала.
	Валидные значения:
	
	    MODES     сброс всех режимов канала (i,k,l,m,n,p,s,t и т. д.)
	    BANS      удаление всех банов канала
	    EXCEPTS   удаление всех исключений из бана
	    INVITES   удаление всех исключений на приглашение
	    OPS       снятие статуса оператора со всех операторов канала
	    HOPS      снятие статуса полуоператора со всех хопов канала
	    VOICES    снятие статуса войса со всех войсов канала
	    USERS     массовый кик всех посетителей канала
	
	По-умолчанию, данная команда может быть использована только
	владельцем канала.
	
CHAN_HELP_GETKEY
	Синтаксис: GETKEY #канал
	
	Позволяет запросить у сервисов ключ-пароль канала.  Данная команда
	обычно используется ботами и/или скриптами и возвращает строку вида:
	KEY #канал ключ
	
	Если секретный ключ показан как "NO KEY" - пароль на вход на канале
	не установлен.
	
	По-умолчанию,  данная  команда может быть использована AOP'ами или
	лицами с уровнем доступа 5 и выше.

CHAN_HELP_SENDPASS
	Синтаксис: SENDPASS #канал
	
	Отправляет пароль от канала на email-адрес его  реального владельца,
	прописанного в графе Founder (владелец).
	
	Примечание: в зависимости от настроек сети, доступ к данной команде
	может быть ограничен до IRC-операторов.
	Примечание 2: команда недоступна при включенном шифровании паролей.

CHAN_SERVADMIN_HELP
	
	Администраторы сервисов могут удалить любой канал без идентификации
	к  нему  в  качестве  владельца, могут просматривать списки доступа
	каналов, а так же, списки акиков и установки уровней доступа каналов.
	
CHAN_SERVADMIN_HELP_LOGOUT
	Синтаксис: LOGOUT #канал [ник]
	
	Деидентифицирует указанный ник от канала, снимая тем самым с него
	статус владельца.  Если  вы  реальный  владелец канала, вы можете
	деидентифицировать  любого,  в  противном  случае - только  себя.
	Примечание: деидентифицируются только те пользователи,  которые
	идентифицировались к каналу посредством команды IDENTIFY.
	
	Как Администратор сервисов, вы можете  деидентифицировать любого
	пользователя  от  любого  канала,  без  какой-либо  дополнительной
	идентификации к каналу. Если вы не укажете ник, то деидентифицрованы
	будут все пользователи канала.

CHAN_SERVADMIN_HELP_DROP
	Синтаксис: DROP #канал
	
	Удаляет указанный канал из базы данных сервисов. Вы м ожете удалить
	только те каналы, которыми вы владеете.
	Как Администратор сервисов, вы можете удалить любой зарегистрированный
	канал сети без какой-либо идентификации к нему.

CHAN_SERVADMIN_HELP_SET
	
	Администраторам сервисов доступна возможность изменять настройки
	любого  зарегистрированного  канала  сети. Также,  им  доступна
	установка скрытой опции NOEXPIRE  (исключает  канал  из условий
	истечения регистрации по времени).

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Синтаксис: SET #канал NOEXPIRE {ON | OFF}
	
	Позволяет установить режим не-истечения  для  указанного канала.
	Активирование этой опции автоматически  исключит канал из условий
	истечения его регистрации по времени. Соответственно деактивирование
	режима вернет все на круги своя.

CHAN_SERVADMIN_HELP_INFO
	
	Администраторы сервисов могут использовать параметр ALL
	относительно любого канала сети.

CHAN_SERVADMIN_HELP_LIST
	Синтаксис: LIST маска [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
	
	Выводит список всех  зарегистрированных  каналов  по указанной маске.
	Каналы с параметром PRIVATE будут показаны только Администраторам
	сервисов. Каналы с параметром NOEXPIRE будут показаны с префиксом
	! перед именем канала.
	
	В  качестве  дополнительных  опций  команды  LIST вы можете  указать
	FORBIDDEN,  SUSPENDED или NOEXPIRE. Это позволит вам получить список
	каналов, отвечающих  определенным  критериям, в частности таким как:
	FORBIDDEN - каналы, запрещенные к использованию; SUSPENDED - каналы,
	"замороженные" до поры до времени;  NOEXPIRE - каналы, находящиеся в
	режиме "не-истечения". Если указаны все четыре опции,  то все 3 типа
	каналов будут перечислены в списке.

CHAN_SERVADMIN_HELP_GETPASS
	Синтаксис: GETPASS #канал
	
	Позволяет получить пароль от указанного канала.
	
	Примечание: сразу по использовании данной команды, сервисы пошлют
	по WALLOPS/GLOBOPS сообщение, содержащие ник использовавшего команду,
	и ник, относительно которого была использована команда. Одновременно
	с этим, событие будет записано в лог-файл.
	Примечание 2: команда недоступна, если включено шифрование паролей.

CHAN_SERVADMIN_HELP_FORBID
	Синтаксис: FORBID #канал [причина]
	
	Запрещает регистрировать или использовать указанный канал. Эффект
	запрета отменяется путем удаления канала.
	
	Примечание: в зависимости от настроек сервисов, причина может
	быть параметром как опциональным, так и необходимым.

CHAN_SERVADMIN_HELP_SUSPEND
	Синтаксис: SUSPEND #канал [причина]
	
	Приостанавливает  любую  активность  указанного  канала (замораживает).
	В режиме саспенда, на канал не действуют условия истечения регистрации
	по времени, канал  невозможно  использовать или удалить. Единственное,
	что владелец все еще может  -  это изменять  настройки  своего канала.
	Эффект "заморозки" отменяется  путем использования команды  UNSUSPEND,
	которая  полностью восстанавливает активность  канала  со  всеми  его
	настройками.
	
	Примечание: в  зависимости  от  настроек  сервисов,  причина может
	быть параметром как опциональным, так и необходимым.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Синтаксис: UNSUSPEND #канал
	
	Полностью восстанавливает активность приостановленного канала.

CHAN_SERVADMIN_HELP_STATUS
	Синтаксис: STATUS #канал ник
	
	Возвращает текущий уровень доступа указанного ника на указанном
	канале. Данные возвращаются в следующем формате:
	
	    STATUS #канал ник уровень_доступа
	
	Если произойдет ошибка, то ответ будет таким:
	
	    STATUS ERROR сообщение_об_ошибке

###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP_CMD_SEND
	    SEND   Отправка мемо-сообщения указанному нику или каналу
MEMO_HELP_CMD_CANCEL
	    CANCEL Отмена последнего отправленного вами мемо-сообщения
MEMO_HELP_CMD_LIST
	    LIST   Список ваших мемо-сообщений
MEMO_HELP_CMD_READ
	    READ   Чтение какого-либо сообщения, группы сообщений
MEMO_HELP_CMD_DEL
	    DEL    Удаление мемо-сообщений
MEMO_HELP_CMD_SET
	    SET    Установка опций для мемо-сообщений
MEMO_HELP_CMD_INFO
	    INFO   Информация о кол-ве сообщений и настройках оповещения
MEMO_HELP_CMD_RSEND
	    RSEND  Отправка мемо-сообщения с условием подтверждения о прочтении
MEMO_HELP_CMD_CHECK
	    CHECK  Проверка на то, было ли прочитано последнее посланное вами сообщение
MEMO_HELP_CMD_SENDALL
	    SENDALL  Отправка массового мемо-сообщения всем пользователям сети
MEMO_HELP_CMD_STAFF
	    STAFF  Отправка мемо-сообщения всем операторам/администраторам сети

MEMO_HELP_HEADER

	Основное предназначение %S - это дать зарегистрированным
	пользователям возможность посылать друг-другу короткие "записки",
	называемые так же мемо-сообщениями. Данный сервис особенно удобен,
	когда вам необходимо что-то сообщить интересующему вас человеку, а
	он на данный момент вне сети (offline).
	В качестве отправителя вы можете указать как ник, так и имя канала*.
	Помните, что отправка сообщения возможна лишь зарегистрированному
	на сервисах адресату!
	
	Список команд %S:


MEMO_HELP_ADMIN
	не используется.
MEMO_HELP_OPER
	не используется.

MEMO_HELP_FOOTER
	
	Для более подробной информации см. %R%S HELP команда

	*  По-умолчанию, любой пользователь, с уровнем доступа на канале 
	   не менее 10, может читать мемо-сообщения отправленные каналу.
	   Изменить эти условия вы можете с помощью тонкой настройки через
	   %s LEVELS.

MEMO_HELP_SEND
	Синтаксис: SEND {ник | #канал} текст_сообщения
	
	Отправляет сообщение для ника или для #канала, содержащее
	текст_сообщения. Если в качестве получателя указан ник, то он
	будет уведомлен о том, что вы отправили ему мемо-собщение.
	Примечание: получатель, будь то ник или канал, должен быть
	зарегистрирован, для возможности отправки/получения мемо-сообщений.
	
MEMO_HELP_CANCEL
	Синтаксис: CANCEL {ник | #канал}
	
	Отменяет последнее сообщение, отправленное вами указанному нику
	или каналу, тем самым предохраняя его от прочтения адресатом.

MEMO_HELP_LIST
	Синтаксис: LIST [#канал] [список_записей | NEW]
	
	Выводит текущий список сообщений. С параметром NEW, покажет только
	новые (непрочитанные) сообщения, они будут помечены значком "*"
	слева от номера сообщения.
	Помимо этого, вы можете указать список записей или номер записи,
	например:
	
	   LIST 2-5,7-9
	      покажет сообщения с 2-го по 5-ое и с 7-го по 9-ое.

MEMO_HELP_READ
	Синтаксис: READ [#канал] {номер | список_записей | LAST | NEW}
	
	Позволяет прочесть указанные сообщения. С параметром LAST - выведет
	текст последнего полученного вами сообщения, с параметром NEW -
	отобразит все непрочитанные вами сообщения.
	Помимо этого, вы можете указать список записей или номер записи,
	например:
	
	   READ 2-5,7-9
	      выведет текст всех сообщений с 2-го по 5-ое и с 7-го по 9-ое.

MEMO_HELP_DEL
	Синтаксис: DEL [#канал] {номер | список_записей | LAST | ALL}
	
	Позволяет удалить указанное сообщение. В зависимости от указанного
	параметра, вы можете удалить сразу несколько сообщений или целый
	диапазон сообщений (см. пример ниже).
	
	Параметр LAST позволяет удалить последнее полученное сообщение.
	Параметр ALL позволяет полностью очистить список сообщений.
	
	Примеры:
	
	   DEL 1
	      удалит сообщение под номером 1.
	
	   DEL 2-5,7-9
	      удалит все сообщения с 2-го по 5-ое и с 7-го по 9-ое.

MEMO_HELP_SET
	Синтаксис: SET опция параметры
	
	Позволяет настроить ваши личные опции службы сообщений.
	Валидные опции:
	
	    NOTIFY      выбор способа уведомления о новых собщениях
	    LIMIT       установка вашего максимального лимита сообщений
	
	Для более подробной информации о конкретной опции воспользуйтесь
	командой %R%S HELP SET опция.

MEMO_HELP_SET_NOTIFY
	Синтаксис: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
	
	Данная опция позволяет настроить ваш личный метод уведомления о
	новых сообщениях, посланных вам кем-либо.
	Доступные режимы:

	    ON      уведомление будет поступать сразу, как только вы
	            идентифицируетесь к нику, снимете с себя статус
	            /AWAY или кто-то отправит вам новое сообщение.
	    LOGON   уведомление будет поступать сразу, как только вы
	            идентифицируетесь к нику или снимете с себя статус
	            /AWAY (отошел).
	    NEW     уведомление будет поступать сразу, как только кто-то
	            отправит вам новое сообщение.
	    MAIL    You will be notified of memos by email aswell as
	               any other settings you have.
	    NOMAIL  You will not be notified of memos by email.
	    OFF     уведомлений о получении новых сообщений поступать
	            не будет
	
	Параметр ON является комбинацией значений LOGON и NEW.

MEMO_HELP_SET_LIMIT
	Синтаксис: SET LIMIT [#канал] предел
	
	Позволяет ограничить максимальное количество сообщений, которое вы
	(или канал) можете принять. Установка значения в 0 полностью отключит
	возможность принятия сообщений.
	Максимально допустимое значение: %d.

MEMO_HELP_INFO
	Синтаксис: INFO [#канал]
	
	Показывает  информацию о  количестве и  статусе ваших сообщений, в
	частности сколько из них непрочитано, и сколько всего сообщений
	вы можете принять. Если  вы  укажете  в качестве параметра канал - 
	выведет аналогичную информацию об указанном канале.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Синтаксис: SET LIMIT [пользователь | #канал] {предел | NONE} [HARD]
	
	Устанавливает лимит на максимальное количество сообщений, которое
	пользователь (или канал) смогут принимать. Установка лимита равным
	0 отключит возможность принятия сообщения вообще, установка в NONE
	разрешает пользователю принимать и сохранять столько сообщений,
	сколько он пожелает. Если вы не укажете пользователя или канал,
	настройки буду применены относительно вас.
	
	Дополнительный параметр HARD запретит указанному пользователю
	изменять свой лимит сообщений. Соответственно установка лимита без
	параметра HARD отменяет эффект запрета и разрешает пользователю
	настраивать свой лимит сообщений.
	
	Расширенное использование команды SET LIMIT доступно только для
	Администраторов сервисов. Обычные пользователи могуть менять лимит
	только для себя или для канала, на котором у них есть соответствующие
	привилегии. Они не могут удалить свой лимит, не могут установить
	лимит выше %d, и не могут использовать опцию HARD.

MEMO_SERVADMIN_HELP_INFO
	Синтаксис: INFO [ник | #канал]
	
	Без параметров, покажет общее количество сообщений, которое вы
	имеете, сколько из них непрочитано, и сколько всего сообщений вы
	можете принимать.
	
	Если указать канал, покажет ту же информацию об указанном канале.
	
	Если указать в качестве параметра ник, покажет ту же информацию об
	указанном нике.
	Данный параметр могут использовать только Администраторы сервисов.

MEMO_HELP_STAFF
	Синтаксис: STAFF текст_сообщения
	
	Позволяет отправить мемо-сообщение всем операторам/администраторам
	сервисов сети.
	Примечание: пользователи, значащиеся как в списке операторов, так
	и в списке администраторов сервисов - получат данное сообщение дважды.
	Аналогично и по отношению к рутам сервисов.

MEMO_HELP_SENDALL
	Синтаксис: SENDALL текст_сообщения
	
	Отправляет массовое мемо-сообщение всем зарегистрированным
	пользователям сети.

MEMO_HELP_RSEND
	Синтаксис: RSEND {ник | #канал} текст_сообщения
	
	Данная команда аналогична команде SEND и позволяет послать
	сообщение для указанного ника или канала. Отличительной ее
	особенностью является то, что сразу после прочтения адресатом
	сообщения, отправитель автоматически получит извещение о прочтении.
	Само собой разумеется, что ник/канал отправителя/получателя должны
	быть зарегистрированы.

MEMO_HELP_CHECK
	Синтаксис: CHECK ник
	
	Проверяет, было ли прочитано последнее посланное вами сообщения
	указанным ником.
	Примечание: команда работает только для ников, не для каналов.

###########################################################################
#
# OperServ help messages
#
###########################################################################
OPER_HELP_CMD_GLOBAL
	    GLOBAL      Отправка глобального сообщения всем пользователям сети
OPER_HELP_CMD_STATS
	    STATS       Запрос информаци о статусе сервисов и сети
OPER_HELP_CMD_OPER
	    OPER        Управление списком Операторов сервисов
OPER_HELP_CMD_ADMIN
	    ADMIN       Управление списком Администраторов сервисов
OPER_HELP_CMD_STAFF
	    STAFF       Список всех прописанных на сервисах и их онлайн-статус
OPER_HELP_CMD_MODE
	    MODE        Изменение какого-либо режима на указанном канала
OPER_HELP_CMD_KICK
	    KICK        Кикнуть пользователя с канала посредством сервисов
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  Очистка всех режимов указанного канала
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  Отключить всех пользователей с определенного хоста
OPER_HELP_CMD_AKILL
	    AKILL       Управление списком AKILL'ов
OPER_HELP_CMD_SGLINE
	    SGLINE      Управление списком SGLINE'ов
OPER_HELP_CMD_SQLINE
	    SQLINE      Управление списком SQLINE'ов
OPER_HELP_CMD_SZLINE
	    SZLINE      Управление списком SZLINE'ов
OPER_HELP_CMD_CHANLIST
	    CHANLIST    Список текущих активных каналов сети
OPER_HELP_CMD_USERLIST
	    USERLIST    Список текущих пользователей сети
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   Управление списком новостей при подключении
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  Управление списком случайных новостей
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    Управление списком опер-новостей
OPER_HELP_CMD_SESSION
	    SESSION     Просмотр количества сессий с указанного хоста
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   Управление списком исключений из лимита сессий
OPER_HELP_CMD_NOOP
	    NOOP        Временное удаление всех O:line'ов с указанного
	                сервера сети
OPER_HELP_CMD_JUPE
	    JUPE        "Джуп" указанного сервера сети
OPER_HELP_CMD_IGNORE
	    IGNORE      Управление игнор-списком сервисов
OPER_HELP_CMD_SET
	    SET         Настройка различных опций сервисов
OPER_HELP_CMD_RELOAD
	    RELOAD      Перезагрузка конфигурационного файла сервисов
OPER_HELP_CMD_UPDATE
	    UPDATE      Обновление баз данных сервисов (сохранение на диск)
OPER_HELP_CMD_RESTART
	    RESTART     Сохранить базы данных и перезапустить сервисы
OPER_HELP_CMD_QUIT
	    QUIT        Завершить работу сервисов без сохранения базы данных
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    Завершить работу сервисов с сохранением базы данных
OPER_HELP_CMD_DEFCON
	    DEFCON      Управление системой DefCon (защитных контрмер)
OPER_HELP_CMD_CHANKILL
	    CHANKILL    "Прибить" всех пользователей на указанном канале
OPER_HELP_CMD_OLINE
	    OLINE       Установка оперфлагов указанному пользователю сети
OPER_HELP_CMD_UMODE
	    UMODE       Изменение режимов какого-либо пользователя сети
OPER_HELP_CMD_SVSNICK
	    SVSNICK     Принудительное изменение ника пользователя сети
OPER_HELP_CMD_MODLOAD
	    MODLOAD     Загрузить модуль
OPER_HELP_CMD_MODUNLOAD
	    MODUNLOAD   Выгрузить модуль
OPER_HELP_CMD_MODINFO
	    MODINFO     Информация о загруженном модуле
OPER_HELP_CMD_MODLIST
	    MODLIST     Список загруженных модулей

OPER_HELP
	Список команд %S:

OPER_HELP_LOGGED
	Примечание: все команды %S'у записываются в лог!

OPER_HELP_GLOBAL
	Синтаксис: GLOBAL сообщение
	
	Позволяет Администраторам сервисов отправлять глобальные сообщения
	всем пользователям сети. В качестве ника отправителя будет указан
	%s.

OPER_HELP_STATS
	Синтаксис: STATS [AKILL | ALL | RESET | MEMORY | UPLINK]
	
	Использованная без параметров, команда покажет текущее количество
	пользователей сети, текущее кол-во IRC-операторов (за исключением
	самих сервисов), максимально зафиксированное число пользователей
	и время работы сервисов в сети.
	
	С параметром AKILL, покажет текущий размер списка AKILL'ов и время
	истечения AKILL'ов по-умолчанию (AKILL, SZLINE, ZGLINE, ZQLINE).
	
	Параметр RESET сбрасывает счетчик зафиксированного максимума 
	пользователей сети.
	
	Параметр MEMORY выведет информацию об использовании сервисами
	оперативной памяти. Запросы подобной статистики могу затормозить
	работу сервисов на определенный промежуток времени, поэтому, не
	рекомендуется использовать параметр MEMORY слишком часто.
	
	Параметр UPLINK покажет информацию о сервере, к которому
	прилинкованы сервисы сети.
	
	Параметр ALL выведет общую статистику в комбинации с данными из
	MEMORY и UPLINK.
	
	Команда UPTIME является алиасом команды STATS.

OPER_HELP_OPER
	Синтаксис: OPER ADD ник
	           OPER DEL {ник | номер_записи | список_записей}
	           OPER LIST [маска | список_записей]
	           OPER CLEAR
	
	Позволяет  Рутам  сервисов  управлять  списком операторов сервисов.
	Операторы  сервисов  не имеют  возможности  администрировать ники и
	каналы сети, однако, они имеют доступ к некоторым командам/функциям
	операторского сервиса сети.  Для использования своих привилегий они
	должны иметь статус IRC-оператора и быть проидентифицированы к нику
	на %s.
	
	Команда  OPER ADD  позволяет  добавить  указанный  ник  в список
	Операторов сервисов.
	
	Команда OPER DEL удаляет  указанный  ник  из  списка  Операторов
	сервисов. В качестве параметра вы можете указать не только ник, но
	и  конкретный  номер записи  или  список  записей  (см. примеры
	использования команды LIST ниже).
	
	Команда OPER LIST выводит список Операторов сервисов. В качестве
	дополнительного параметра, вы можете указать символьную маску, что
	позволит вам получить список с конкретными никами, попадающими под
	эту маску, или же, вы можете указать список записей. Например:
	
	   OPER LIST 2-5,7-9
	      отобразит все записи с 2-ой по 5-ю и с 7-ой по 9-ю.
	
	Команда OPER CLEAR позволяет полностью очистить список Операторов
	сервисов.
	
	Использовать команду OPER LIST может любой пользователь с правами
	IRC-оператора, остальные команды ограничены до Рута сервисов.

OPER_HELP_ADMIN
	Синтаксис: ADMIN ADD ник
	           ADMIN DEL {ник | номер_записи | список_записей}
	           ADMIN LIST [маска | список_записей]
	           ADMIN CLEAR
	
	Позволяет Рутам сервисов управлять списком Администраторов сервисов.
	Помимо всех привилегий Операторво сервисов, Администраторы сервисов
	обладают дополнительными возможностями по админстрированию ников и
	каналов сети. Им  доступны  практически  все функции операторского
	сервиса сети.
	Для  использования  своих  привилегий,  они  должны  иметь  статус
	IRC-оператора сети и быть проидентифицированы к нику на %s.
	
	Команда  ADMIN ADD  позволяет  добавить  указанный  ник в список
	Администраторов сервисов.
	
	Команда ADMIN DEL удаляет  указанный ник из списка Администраторов
	сервисов. В качестве параметра вы можете указать не только ник, но
	и  конкретный  номер записи  или  список  записей  (см. примеры
	использования команды LIST ниже).
	
	Команда  ADMIN LIST  выводит  список Администраторов сервисов. В
	качестве  дополнительного  параметра, вы можете указать символьную
	маску,  что позволит  вам  получить  список  с конкретными никами,
	попадающими под эту маску, или же, вы можете указать список записей.
	Например:
	
	   ADMIN LIST 2-5,7-9
	      отобразит все записи с 2-ой по 5-ю и с 7-ой по 9-ю.
	
	Команда  ADMIN CLEAR  позволяет  полностью  очистить  список
	Администраторов сервисов.
	
	Использовать команду ADMIN LIST может любой пользователь с правами
	IRC-оператора, остальные команды ограничены до Рута сервисов.

OPER_HELP_IGNORE
	Синтаксис: IGNORE {ADD|DEL|LIST|CLEAR} [время] [ник|маска]
	
	Позволяет  Администраторам сервисов  управлять списком игнорируемых
	сервисами пользователей.
	По-умолчанию, параметр [время] является секундами. Однако, вы можете
	указать и иные единицы измерения, используя: s для секунд, m для
	минут, h для часов и d для дней. Комбинации  этих единиц измерения
	недопустимы. Установка  времени  игнора в 0  сделает его постоянным.
	В  качестве последнего значения  вы  должны  указать  валидный ник или
	маску вида ник!идент@хост или идент@хост. Любые другие значения будут
	расцениваться в качестве ника.

	Пользователи со статусом  IRC-оператора  игнорироваться  сервисами не
	будут, даже если они присутствуют в списке игнора.

OPER_HELP_MODE
	Синтаксис: MODE #канал режимы
	
	Позволяет Операторам сервисов изменять режимы на каком-либо канале.
	В качестве второго параметра команды, вы должны указать режимы в
	том же формате, в котором указываете при ручной установке, с помощью
	команды /MODE.

OPER_HELP_UMODE
	Синтаксис: UMODE ник режимы
	
	Позволяет Администраторам сервисов изменять пользовательские режимы
	(usermodes) любого посетителя сети. В  качестве  второго параметра
	команды, вы должны указать режимы  в том же  формате,  в котором
	указываете при ручной установке, с помощью команды /MODE.
	
	Для использования этой команды необходимы права Супер-Администратора.

OPER_HELP_OLINE
	Синтаксис: OLINE ник оперфлаги
	
	Позволяет Администраторам сервисов установить флаги IRC-Оператора
	любому посетителю сети. Перед флагами должен присутствовать префикс
	"+"  -  для установки,  или "-" -  соответственно  для снятия.
	Чтобы удалить все флаги, используйте "-" вместо какого-либо флага.
	
	Для использования этой команды необходимы права Супер-Администратора.

OPER_HELP_CLEARMODES
	Синтаксис: CLEARMODES #канал [ALL]
	
	Очищает все двоичные режимы (i,k,l,m,n,p,s,t) и баны на указанном
	канале. С дополнительным параметром ALL - снимет со всех статусы
	операторов/войсов (режимы +o и +v).

OPER_HELP_KICK
	Синтаксис: KICK #канал ник причина
	
	Позволяет  Операторам сервисов  выкинуть какого-либо пользователя с
	указанного канала сети.
	Параметры данной команды аналогичны параметрам стандартной команды
	/KICK. Причина кика будет включать в себя ник Оператора сервисов,
	который использовал данную команду, например:
	
	*** SpamMan was kicked by %S (Alcan (Flood))

OPER_HELP_SVSNICK
	Синтаксис: SVSNICK ник новый_ник
	
	Позволяет принудительно изменить указанный ник на новый_ник.

OPER_HELP_AKILL
	Синтаксис: AKILL ADD [+срок_истечения] маска причина
	           AKILL DEL {маска | номер_записи | список_записей}
	           AKILL LIST [маска | список_записей]
	           AKILL VIEW [маска | список_записей]
	           AKILL CLEAR
	
	Позволяет Операторам сервисов управлять списком сервисных AKILL'ов.
	Любой пользователь, подпадающий под маску AKILL'а, будет немедленно
	отключен от сети посредством сервисного KILL'а с указанной причиной,
	и не важно, на каком сервере он сидит или к какому подключается.
	Помимо  этого,  если  IRCd  вашей сети  это поддерживает, сервисы 
	установят KLINE/GLINE на всех серверах вашей сети, в качестве маски
	бана будет использована соответствующая запись AKILL'а.
	
	Команда AKILL ADD добавляет маску вида идент@хост/ip с указанной
	причиной в список AKILL'ов (наличие причины - обязательно).
	Значение срока истечения - это целое число, которое может быть
	одним из: d (дней), h (часов), или m (минут). Такие сочетания
	как 1h30m - недопустимы. Если единица измерения не указана, то
	по-умолчанию, она будет принята за "d" - "дни" (таким образом,
	+30 будет означать 30 дней). Чтобы добавить постоянный AKILL,
	используйте время истечения равным +0. Помните, что если в качестве
	первого  символа  второго параметра указан знак "+" - вы должны
	обозначить  сроки  истечения  записи, даже  если это будет значение
	по-умочанию.
	Текущее время истечения AKILL'а по-умолчанию, можно узнать с помощью
	команды STATS AKILL.
	
	Команда AKILL DEL удаляет указанную маску из списка AKILL'ов.
	В качестве параметра вы можете указать не только маску AKILL'а, но
	и  конкретный  номер записи  или  список  записей  (см. примеры
	использования команды LIST ниже).
	
	Команда AKILL LIST показывает текущий список AKILL'ов.
	В качестве дополнительного параметра, вы можете указать символьную
	маску, что  позволит  вам  получить  список с конкретными записями,
	попадающими под эту маску, или же, вы можете указать список записей.
	Например:
	
	   AKILL LIST 2-5,7-9
	      отобразит все записи с 2-ой по 5-ю и с 7-ой по 9-ю.
	
	AKILL VIEW более подробная версия AKILL LIST, она покажет вам
	кто добавил AKILL, время установки AKILL'а, когда он истекает, ну
	и, конечно же, маску вида идент@хост/ip и причину AKILL'а.
	
	AKILL CLEAR позволяет  полностью  очистить  список AKILL'ов.
	
OPER_HELP_SGLINE
	Синтаксис: SGLINE ADD [+срок_истечения] маска:причина
	           SGLINE DEL {маска | номер_записи | список_записей}
	           SGLINE LIST [маска | список_записей]
	           SGLINE VIEW [маска | список_записей]
	           SGLINE CLEAR
	
	Позволяет Операторам сервисов управлять списком сервисных SGLINE'ов.
	SGLINE-запись позволяет вам поставить глобальный бан на определенный
	RealName  aka  GECOS.  Любой  пользователь,  реальное имя которого
	попадет под маску какой-либо записи в SGLINE-списке, будет сразу же
	отключен от сети и в дальнейшем, сервисы не позволят ему подключится.
	
	Команда SGLINE ADD добавляет маску реального имени с указанной
	причиной в список SGLINE'ов (наличие причины - обязательно).
	Значение срока истечения - это целое число, которое может быть
	одним из: d (дней), h (часов), или m (минут). Такие сочетания
	как 1h30m - недопустимы. Если единица измерения не указана, то
	по-умолчанию, она будет принята за "d" - "дни" (таким образом,
	+30 будет означать 30 дней). Чтобы добавить постоянный SGLINE,
	используйте время истечения равным +0. Помните, что если в качестве
	первого  символа  второго параметра указан знак "+" - вы должны
	обозначить  сроки  истечения  записи, даже  если это будет значение
	по-умочанию.
	Текущее время истечения SGLINE'а по-умолчанию, можно узнать с помощью
	команды STATS AKILL.
	Примечание: так  как  строки реальных имен  могут содержать пробелы,
	разделителем между маской реального имени и причиной будет двоеточие.
	
	Команда  SGLINE DEL  удаляет указанную маску из списка SGLINE'ов.
	В качестве параметра вы можете указать не только маску SGLINE'а, но
	и  конкретный  номер записи  или  список  записей  (см. примеры
	использования команды LIST ниже).
	
	Команда SGLINE LIST показывает текущий список SGLINE'ов.
	В качестве дополнительного параметра, вы можете указать символьную
	маску, что  позволит  вам  получить  список с конкретными записями,
	попадающими под эту маску, или же, вы можете указать список записей.
	Например:
	
	   SGLINE LIST 2-5,7-9
	      отобразит все записи с 2-ой по 5-ю и с 7-ой по 9-ю.
	
	SGLINE VIEW более подробная версия  SGLINE LIST, она покажет кто
	добавил SGLINE, время установки SGLINE'а, когда он истекает, ну и,
	конечно же, маску реального имени и причину SGLINE'а.
	
	SGLINE CLEAR позволяет  полностью  очистить  список SGLINE'ов.

OPER_HELP_SQLINE
	Синтаксис: SQLINE ADD [+срок_истечения] маска причина
	           SQLINE DEL {маска | номер_записи | список_записей}
	           SQLINE LIST [маска | список_записей]
	           SQLINE VIEW [маска | список_записей]
	           SQLINE CLEAR
	
	Позволяет Операторам сервисов управлять списком сервисных SQLINE'ов.
	SQLINE-запись позволяет вам поставить глобальный бан на определенный
	ник или маску ника. Любой  пользователь,  ник которого попадет под
	маску какой-либо записи в SQLINE-списке, будет сразу же отключен от
	сети и в дальнейшем, сервисы не позволят ему подключится с этим ником.
	
	Если IRCd вашей сети поддерживает установку QLINE не только на ники,
	но и на каналы - вы можете указать в качестве первого символа маски
	'#' - чтобы запретить к использованию указанный канал.
	
	Команда SQLINE ADD добавляет указанный ник или канал с указанной
	причиной в список SQLINE'ов (наличие причины  -  обязательно).
	Значение срока истечения - это целое число,  которое может быть
	одним из: d (дней), h (часов), или m (минут). Такие сочетания
	как 1h30m - недопустимы. Если  единица  измерения не указана, то
	по-умолчанию, она  будет принята за "d" - "дни" (таким  образом,
	+30 будет означать 30 дней). Чтобы  добавить постоянный SQLINE,
	используйте время истечения равным +0. Помните, что если в качестве
	первого  символа  второго параметра указан знак "+" - вы должны
	обозначить  сроки  истечения  записи, даже  если это будет значение
	по-умочанию.
	Текущее время истечения SQLINE'а по-умолчанию, можно узнать с помощью
	команды STATS AKILL.
	
	Команда SQLINE DEL удаляет указанную маску из списка SQLINE'ов.
	В качестве параметра вы можете указать не только маску SQLINE'а, но
	и  конкретный  номер записи  или  список  записей  (см. примеры
	использования команды LIST ниже).
	
	Команда SQLINE LIST показывает текущий список SQLINE'ов.
	В качестве дополнительного параметра, вы можете указать символьную
	маску, что  позволит  вам  получить  список с конкретными записями,
	попадающими под эту маску, или же, вы можете указать список записей.
	Например:
	
	   SQLINE LIST 2-5,7-9
	      отобразит все записи с 2-ой по 5-ю и с 7-ой по 9-ю.
	
	SQLINE VIEW более подробная версия SQLINE LIST, она покажет вам
	кто добавил SQLINE, время установки SQLINE'а, когда он истекает, ну
	и, конечно же, маску ника или канала и причину устанвоки SQLINE'а.
	
	SQLINE CLEAR позволяет  полностью  очистить  список SQLINE'ов.

OPER_HELP_SZLINE
	Синтаксис: SZLINE ADD [+срок_истечения] ip-маска причина
	           SZLINE DEL {маска | номер_записи | список_записей}
	           SZLINE LIST [маска | список_записей]
	           SZLINE VIEW [маска | список_записей]
	           SZLINE CLEAR
	
	Позволяет Операторам сервисов управлять списком сервисных SZLINE'ов.
	SZLINE-запись позволяет вам поставить глобальный бан на определенный
	IP-адрес. Любой  пользователь,  ip-адрес которого  попадет  под
	маску какой-либо записи в SZLINE-списке, будет сразу же отключен от
	сети и в дальнейшем, сервисы не позволят ему подключится с этим IP.
	Наличие или отсутствие PTR RR записи для данного IP - роли не играет.
	
	Команда SZLINE ADD добавляет указанную маску IP-адреса указанной
	причиной в список SZLINE'ов (наличие причины  -  обязательно).
	Значение срока истечения - это целое число,  которое может быть
	одним из: d (дней), h (часов), или m (минут). Такие сочетания
	как 1h30m - недопустимы. Если  единица  измерения не указана, то
	по-умолчанию, она  будет принята за "d" - "дни" (таким  образом,
	+30 будет означать 30 дней). Чтобы  добавить постоянный SZLINE,
	используйте время истечения равным +0. Помните, что если в качестве
	первого  символа  второго параметра указан знак "+" - вы должны
	обозначить  сроки  истечения  записи, даже  если это будет значение
	по-умочанию.
	Текущее время истечения SZLINE'а по-умолчанию, можно узнать с помощью
	команды STATS AKILL.
	
	Команда SZLINE DEL удаляет указанную маску из списка SZLINE'ов.
	В качестве параметра вы можете указать не только маску SZLINE'а, но
	и  конкретный  номер записи  или  список  записей  (см. примеры
	использования команды LIST ниже).
	
	Команда SZLINE LIST показывает текущий список SZLINE'ов.
	В качестве дополнительного параметра, вы можете указать символьную
	маску, что  позволит  вам  получить  список с конкретными записями,
	попадающими под эту маску, или же, вы можете указать список записей.
	Например:
	
	   SZLINE LIST 2-5,7-9
	      отобразит все записи с 2-ой по 5-ю и с 7-ой по 9-ю.
	
	SZLINE VIEW более подробная версия SZLINE LIST, она покажет вам
	кто добавил SZLINE, время установки SZLINE'а, когда он истекает, ну
	и, конечно же, маску IP-адреса и причину установки SZLINE'а.
	
	SZLINE CLEAR позволяет  полностью  очистить  список SZLINE'ов.

OPER_HELP_SET
	Синтаксис: SET опция параметр
	
	Позволяет настроить различные глобальные опции сервисов.
	Доступные опции:
	
	    READONLY   активация/деактивация режима read-only (только-чтение)
	    LOGCHAN    активация/деактивация логирования событий на сервисный канал
	    DEBUG      активация/деактивация режим отладки (debug mode)
	    NOEXPIRE   активация/деактивация режима не-истечения
	    SQL        активация/деактивация SQL-режима
	    IGNORE     активация/деактивация обработки списка игнора
	    LIST       просмотр статуса всех вышеперечисленных опций
	    SUPERADMIN активация/деактивация режима Супер-Администратора
	                 (относительно использующего команду)

OPER_HELP_SET_READONLY
	Синтаксис: SET READONLY {ON | OFF}
	
	Включает или выключает глобальный режим READONLY - только-чтение.
	Работа  сервисов  в  данном режиме запрещает обычным пользователям
	изменять какие-либо сервисные данные, например - списки доступа для
	ников или каналов, их настройки и т. д. Тем не менее, IRC-операторы
	с достаточными привилегиями на сервисах смогут модифицировать список
	AKILL'ов и удалять или запрещать ники и каналы, но все эти изменения
	не будут сохранены до тех пор, пока режим READONLY активен. Что бы
	сохранить все изменения, сделанные во время работы сервисов в этом
	режиме, вы должны деактивировать режим только-чтения прежде, чем
	работа сервисов будет завершена или они будут перезапущены.
	
	Активирование данной опции эквивалентно запуску сервисов с ключом
	-readonly.

OPER_HELP_SET_LOGCHAN
	Синтаксис: SET LOGCHAN {ON | OFF}
	
	Позволяет активировать логирование сервисных событий на специальный
	сервисный канал, указаный в конфигурационном файле сервисов. Если
	данная директива в конфиге не указана, возможность настройки опции
	будет заблокирована.
	Примечание: из соображений безопасности, рекомендуется обратить
	особое внимание на ограничение доступа на этот канал.
	
	Активирование данной опции эквивалентно запуску сервисов с ключом
	-logchan.

OPER_HELP_SET_DEBUG
	Синтаксис: SET DEBUG {ON | OFF | уровень}
	
	Включает или выключает отладочный режим работы сервисов. Активация
	данного режима позволит более детально логировать работу сервисов,
	в частности - записывать в лог все посланные и принятые сервисами
	данные, и операции с этими самыми данными. В качестве параметра вы
	можеет указать уровень детальность отладочного режима, чем выше
	уровень - тем более детальными (и обьемными, хех) будут логи.
	
	Активирование данной опции эквивалентно запуску сервисов с ключом
	-debug.

OPER_HELP_SET_NOEXPIRE
	Синтаксис: SET NOEXPIRE {ON | OFF}
	
	Включает или выключает режим глобального "не-истечения". В этом
	режиме, все ники, каналы, AKILL'ы и исключения из лимита сессий -
	никогда не истекут, по крайней мере до тех пор, пока эта опция не
	будет отключена.
	
	Активирование данной опции эквивалентно запуску сервисов с ключом
	-noexpire.

OPER_HELP_SET_SUPERADMIN
	Синтаксис: SET SUPERADMIN {ON | OFF}
	
	Режим Супер-Администратора  не  влияет на саму работу сервисов как
	таковую, однако его активирование влияет на условия проверки уровня
	доступа пользователя к тем или иным функциям. В частности, данный
	режим предоставляет использовавшему его Руту сервисов неограниченные
	привилегии на сервисах, например такие, как статус владельца на всех
	каналах сети, и т. п.
	
	Эта опция не постоянная и должна использоваться только при крайней
	необходимости. Отключите ее сразу, как только перестанете нуждаться
	в ней.

OPER_HELP_SET_SQL
	Синтаксис: SET SQL {ON | OFF}
	
	Данная опция позволяет вам "на лету" включать или выключать SQL-режим
	сервисов. Весьма  полезно,  например, когда  MySQL сервер временно
	недоступен.

OPER_HELP_SET_IGNORE
	Синтаксис: SET IGNORE {ON | OFF}
	
	Позволяет активировать/деактивировать обработку сервисного списка
	игнорируемых  ников.  Включите данный режим, если хотите что бы
	сервисы игнорировали запросы от ников числящихся в списке игнора.

OPER_HELP_SET_LIST
	Синтаксис: SET LIST
	
	Отображает статус различных сервисных настроек (вкл/выкл).

OPER_HELP_NOOP
	Синтаксис: NOOP SET сервер
	           NOOP REVOKE сервер
	
	Команда NOOP SET позволяет удалить все активные O:lines на указанном
	сервере и одновременно с этим прибить всех его IRC-операторов, дабы
	исключить возможность /REHASH'а сервера, который восстановит O:line
	записи.
	
	NOOP REVOKE отменяет эффект команды NOOP SET и возвращает все
	O:lines указанного сервера.
	
	Примечание: наличие сервера в сети сервисами не проверяется.

OPER_HELP_JUPE
	Синтаксис: JUPE сервер [причина]
	
	Позволяет "джупнуть" указанный сервер с указанной причиной.
	В данном случае эффект джупа состоит в следующем: сервисы посылают
	SQUIT указанному серверу, тем самым отключая его от сети, а затем
	тут же создают виртуальный сервер с тем же именем, и причиной в
	качестве описания сервера (server description). Данный фальшивый
	сервер не позволит присоединиться к сети реальному, джупнутому
	серверу. Отменить эффект вы можете путем SQUIT'а виртуальной копии
	джупнутого сервера.
	Примечание: параметр причина является не обязательным, если вы его
	не укажете, будет использована стандартная, вида: "Juped by <ник>".

OPER_HELP_RAW
	Синтаксис: RAW текст
	
	Позволяет отправить указанную текстовую строку напрямую на IRC-сервер,
	к которому  прилинкованы  сервисы.  Учтите, что данные отправляются
	"as is", в сыром виде, и никаких проверок не  осуществляется! А это
	значит, что неверный формат данных  может  вызвать у IRCd-сервера
	ошибку интерпретации и привести к его зависанию и/или отключению.
	
	НЕ ИСПОЛЬЗУЙТЕ ЭТУ КОМАНДУ БЕЗ АБСОЛЮТНОЙ УВЕРЕННОСТИ В ТОМ, ЧТО ВЫ ДЕЛАЕТЕ!

OPER_HELP_UPDATE
	Синтаксис: UPDATE
	
	Заставляет сервисы немедленно сохранить все базы данных на диск.
	
OPER_HELP_RELOAD
	Синтаксис: RELOAD
	
	Заставляет сервисы перечитать конфигурационный файл services.conf.
	Примечание: активация некоторых новых/измененных директив конфига
	требует  полного  перезапуска  сервисов  (например: изменение ников
	сервисов, активация лимитирования сессий, и т.д.)

OPER_HELP_QUIT
	Синтаксис: QUIT
	
	Заставляет  сервисы  немедленно  завершить  свою работу. Все данные,
	накопленные с момента последнего обновления баз данных, сохранены
	не будут! Эта команда должна использоваться только в тех случаях,
	когда текущая загруженная в память база данных содержит какие-либо
	ошибки, сохранения которых нужно избежать.
	Для нормального завершения работы сервисов используйте команду SHUTDOWN.

OPER_HELP_SHUTDOWN
	Синтаксис: SHUTDOWN
	
	Заставляет сервисы сохранить все базы данных и завершить свою работу.

OPER_HELP_RESTART
	Синтаксис: RESTART
	
	Заставляет сервисы сохранить все базы данных и тут же перезапуститься.

OPER_HELP_CHANLIST
	Синтаксис: CHANLIST [{маска | ник} [SECRET]]
	
	Выводит список всех активных (на данный момент) каналов сети, вне
	зависимости от того, зарегистрированы они или нет.
	
	В качестве параметра вы можете указать маску имени канала, что бы
	получить только конкретные, совпадающие записи. Если же вы укажите
	ник,  будет  выведен список всех каналов, на которых находится этот
	пользователь.
	Дополнительный параметр SECRET позволяет получить список каналов
	с режимами +s/+p, совпадающих с указанной символьной маской.

OPER_HELP_USERLIST
	Синтаксис: USERLIST [{хостмаска | #канал} [INVISIBLE]]
	
	Выводит список всех пользователей, находящихся в данный момент в сети.
	Зарегистрирован их ник или нет - роли не играет.
	
	В качестве параметра, вы можете указать пользовательскую хостмаску
	вида  ник!идент@хост  -  это позволит получить только конкретные,
	совпадающие  с  маской  записи. Если  же вы укажите #канал, будет
	выведен список всех посетителей указанного канала.
	Дополнительный параметр INVISIBLE позволяет получить список ников
	с режимом +i, данные которых совпадают с указанной в качестве второго
	параметра хостмаской.

OPER_HELP_MODLOAD
	Синтаксис: MODLOAD имя_файла
	
	Позволяет загрузить какой-либо дополнительный модуль из директории
	с модулями (modules/). 

OPER_HELP_MODUNLOAD
	Синтаксис: MODUNLOAD имя_файла
	
	Выгружает указанный модуль (если модуль загружен и если условия
	позволяют его выгрузить).

OPER_HELP_MODINFO
	Синтаксис: MODINFO имя_файла
	
	Выводит подробную информацию о конкретном загруженном модуле.

OPER_HELP_MODLIST
	Синтаксис: MODLIST [Core|3rd|protocol|encryption|supported|qatested]
	
	Выводит  список  всех  загруженных модулей. В качестве дополнительного
	параметра вы можете указать тип модуля - это позволит получить список,
	отвечающий указанным требованиям.

###########################################################################
#
# BotServ help messages
#
###########################################################################
BOT_HELP_CMD_BOTLIST
	BOTLIST            Список доступных для установки ботов
BOT_HELP_CMD_ASSIGN
	ASSIGN             Установка бота на указанный канал
BOT_HELP_CMD_SET
	SET                Настройка различных опций бота
BOT_HELP_CMD_KICK
	KICK               Настройка условий KICK'а
BOT_HELP_CMD_BADWORDS
	BADWORDS           Управление списком "плохих" слов
BOT_HELP_CMD_ACT
	ACT                Выполнить действие от лица бота на указанном канале
BOT_HELP_CMD_INFO
	INFO               Запрос информации о боте и его настройках на канале
BOT_HELP_CMD_SAY
	SAY                Послать на указанный канал текст от лица бота
BOT_HELP_CMD_UNASSIGN
	UNASSIGN           Снять бота с указанного канала
BOT_HELP_CMD_BOT
	BOT                Управление сервисными ботами

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
	Бот автоматически присоединится к каналу сразу же после того, как
	на нем (на канале, а не на боте) окажется не менее %d посетителей.

BOT_HELP_BOTLIST
	Синтаксис: BOTLIST
	
	Выведет список всех доступных для установки ботов.

BOT_HELP_ASSIGN
	Синтаксис: ASSIGN #канал ник
	
	Позволяет установить указанного бота (ник) на указанный канал. Сразу
	же после установки, вы сможете сконфигурировать опции бота по своему
	усмотрению.

BOT_HELP_UNASSIGN
	Синтаксис: UNASSIGN #канал
	
	Позволяет снять бота с указанного канала. Канал будет удален из базы
	данных бота и сам бот больше на него заходить не будет, однако, все
	данные и настройки канала буду сохранены. В дальнейшем, это позволит
	вам переназначить бота на канал с восстановлением всех старых настроек.

BOT_HELP_INFO
	Синтаксис: INFO {#канал | ник}
	
	Позволяет просмотреть различную информацию о самом боте (ник, идент,
	хост, дату создания, количество обслуживаемых каналов), или о его
	настройках на конкретном #канале (условия киков и установленные
	опции).
	Примечание: данные будут посланы от лица %S

BOT_HELP_SET
	Синтаксис: SET (#канал| бот) опция параметры
	
	Конфигурация различных опций бота.
	Список доступных опции:
	
	    DONTKICKOPS      Не кикать операторов канала за нарушения
	    DONTKICKVOICES   Не кикать войсов канала за нарушения
	    GREET            Активировать/деактивировать режим приветствий
	    FANTASY          Активировать/деактивировать FANTASY-режим
	    SYMBIOSIS        Активировать/деактивировать режим Симбиоза с
	                     сервисом сети по управлению каналами.
	
	Для более подробной информации о конкретной опции воспользуйтесь
	командой %R%S HELP SET опция.
	
	Примечание: доступ к этой команде определяется уровнем доступа к
	команде SET (система LEVELS).

BOT_HELP_SET_DONTKICKOPS
	Синтаксис: SET #канал DONTKICKOPS {ON|OFF}
	
	Позволяет включить или выключить режим исключения операторов канала
	из условий киков  за нарушения.  Данное действие распространится на
	всех операторов канала, не важно, соответствует их уровень доступа
	значению установки NOKICK или нет.

BOT_HELP_SET_DONTKICKVOICES
	Синтаксис: SET #канал DONTKICKVOICES {ON|OFF}
	
	Позволяет  включить или  выключить  режим исключения войсов канала
	из условий киков  за нарушения.  Данное действие распространится на
	всех войсов канала, не важно, соответствует их уровень доступа
	значению установки NOKICK или нет.

BOT_HELP_SET_FANTASY
	Синтаксис: SET #канал FANTASY {ON|OFF}
	
	Активирует/деактивирует FANTASY-режим бота для указанного канала.
	Он позволяет посетителям канала использовать удобные аналоги аналоги
	стандартных команд прямо на канале, например:
	
	!protect, !deprotect  -  установка  статуса защиты  (администратора)
	                         на себя (без парамеров) или указанному нику.
	  !halfop, !dehalfop  -  запрос  статуса  полуоператора  канала, в
	                         качестве параметра вы можете указать ник
	                         посетителя канала.
	    !owner, !deowner  -  запрос  статуса  владельца канала (команда
	                         доступна лишь владельцу канала).
	    !voice, !devoice  -  запрос  статуса  войса канала, в качестве
	                         параметра вы можете указать ник посетителя
	                         канала.
	          !op, !deop  -  запрос статуса оператора канала, в качестве
	                         параметра  вы можете указать ник посетителя
	                         канала.
	          !kick, !kb  -  кикнуть или кикнуть и забанить указанного
	                         посетителя канала с указанной причиной
	                         (например: !kick Vasya пшел вон отсюда!).
	               !seen  -  запрос информации о том, когда указанный
	                         ник последний раз был в сети.
	
	Примечание: для использования команд FANTASY-режима, вы должны
	иметь соответствующий уровень доступа на канале (он определяется
	значением настройки FANTASIA, по-умолчанию это 3 или VOP).
	Помимо этого, ваш уровень доступа должен соответсвовать требованиям
	команды, например, вы не сможете использовать команду !op, если
	ваш уровень доступа не соответствует значению для OP/DEOP (или вы
	не AOP/SOP канала).
	
	Примечание 2: такие команды, как: !halfop, !dehalfop, !protect,
	!deprotect, !owner и !deowner  -  могут быть недоступны в вашей
	сети.

BOT_HELP_SET_GREET
	Синтаксис: SET #канал GREET {ON|OFF}
	
	Включает или выключает режим показа приветствий на канале. Если
	опция включена, бот будет показывать приветственные сообщения для
	всех тех пользователей, которые имеют достаточный уровень доступа
	на канале и установили себе текст приветствия (см. настройки ника).

BOT_HELP_SET_SYMBIOSIS
	Синтаксис: SET #канал SYMBIOSIS {ON|OFF}
	
	Включает  или  выключает  режим симбиоза  с основным сервисом по
	обслуживанию зарегистрированных каналов сети: %s.
	Выключите  данный режим, если  хотите чтобы все действия на канале
	бот совершал от себя, например такие, как: смена режимов, установка
	банов, кики пользователей и т. д.

BOT_HELP_KICK
	Синтаксис: KICK #канал опция параметры
	
	Позволяет тонко настроить условия канала, за нарушения которых бот
	будет автоматически кикать нарушивших их
	Список доступных опции:
	
	    BOLDS         настройка условий кика за чрезмерное использование
	                  жирного текста
	    BADWORDS      настройка условий кика за чрезмерное использование
	                  "плохих" слов
	    CAPS          настройка условий кика за чрезмерное использование
	                  букв в ВЕРХНЕМ регистре (CapsLock).
	    COLORS        настройка условий кика за чрезмерное использование
	                  04ц06в09е11т12н05о08г07о текста
	    FLOOD         настройка условий кика за флуд
	    REPEAT        настройка условий кика за повторяющиеся сообщения
	    REVERSES      настройка условия кика за чрезмерное использование
	                  инверсного текста
	    UNDERLINES    настройка условия кика за чрезмерное использование
	                  подчеркиваний
	
	Для более подробной информации о конкретной опции воспользуйтесь
	командой %R%S HELP KICK опция.
	
	Примечание: доступ к этой команде определяется уровнем доступа к
	команде SET (система LEVELS).

BOT_HELP_KICK_BOLDS
	Синтаксис: KICK #канал BOLDS {ON|OFF} [ttb]
	
	Активируйте  данную  опцию  с  нужными  вам значениями, если хотите
	чтобы  бот  кикал  посетителей  за чрезмерное использование жирного
	текста.
	
	Параметр ttb - это количество киков, по превышению которого бот
	поставит бан. Укажите вместо положительного числа значение "0" или
	"-" если не хотите чтобы бот банил нарушителя.

BOT_HELP_KICK_COLORS
	Синтаксис: KICK #канал COLORS {ON|OFF} [ttb]
	
	Активируйте  данную  опцию  с  нужными  вам значениями, если хотите
	чтобы  бот кикал  посетителей  за чрезмерное использование цветного
	текста.
	
	Параметр ttb - это количество киков, по превышению которого бот
	поставит бан. Укажите вместо положительного числа значение "0" или
	"-" если не хотите чтобы бот банил нарушителя.

BOT_HELP_KICK_REVERSES
	Синтаксис: KICK #канал REVERSES {ON|OFF} [ttb]
	
	Активируйте  данную  опцию  с  нужными  вам значениями, если хотите
	чтобы бот кикал  посетителей за чрезмерное использование инверсного
	текста.
	
	Параметр ttb - это количество киков, по превышению которого бот
	поставит бан. Укажите вместо положительного числа значение "0" или
	"-" если не хотите чтобы бот банил нарушителя.

BOT_HELP_KICK_UNDERLINES
	Синтаксис: KICK #канал UNDERLINES {ON|OFF} [ttb]
	
	Активируйте  данную  опцию  с  нужными  вам значениями, если хотите
	чтобы бот кикал посетителей за чрезмерное использование подчеркиваний.
	
	Параметр ttb - это количество киков, по превышению которого бот
	поставит бан. Укажите вместо положительного числа значение "0" или
	"-" если не хотите чтобы бот банил нарушителя.

BOT_HELP_KICK_CAPS
	Синтаксис: KICK #канал CAPS {ON|OFF} [ttb [минимум [проценты]]]
	
	Активируйте  данную  опцию  с  нужными  вам значениями, если хотите
	чтобы бот кикал посетителей за чрезмерное использование CapsLOCK.
	
	Условия наказания рассчитываются следующим образом: строка, которую
	послал посетитель, должна содержать X больших букв, определенных
	минимумом, и количество этих заглавных букв должно составлять как
	минимум X процентов%% от числа букв всей строки.
	Например: предположим, что настройки у нас стоят как 10 и 25,
	тогда строка вида:
	
	BLABLABLAB BLABLABLAB BLABLABlab blablablab blablablab blablablab
	blablablab blablablab blablablab blablablab
	
	будет попадать под следующее условие: количество заглавных букв
	больше 10 и и процентаж его относительно общего кол-ва букв > 25%.
	Как видите, выставленное нами условие в данном случае сработает и
	пользователь будет кикнут с соответствующей причиной.
	
	Если вы  не  укажете параметры режима, будут использованы значения
	по-умолчанию: 10 для минимума букв и 25% для процентажа.
	
	Параметр ttb - это количество киков, по превышению которого бот
	поставит бан. Укажите вместо положительного числа значение "0" или
	"-" если не хотите чтобы бот банил нарушителя.

BOT_HELP_KICK_FLOOD
	Синтаксис: KICK #канал FLOOD {ON|OFF} [ttb [линий [секунды]]]
	
	Активируйте  данную  опцию  с  нужными  вам значениями, если хотите
	чтобы бот кикал посетителей за флуд.
	
	Условия  наказания  рассчитываются  следующим образом: пользователь
	должен послать на канал количество сообщений, превыщающее параметр
	линий, и, если это произошло в указанный промежуток секунды -
	бот кикнет его с соответствующей причиной.
	
	Если вы  не  укажете параметры режима, будут использованы значения
	по-умолчанию: 6 строк за 10 секунд.
	
	Параметр ttb - это количество киков, по превышению которого бот
	поставит бан. Укажите вместо положительного числа значение "0" или
	"-" если не хотите чтобы бот банил нарушителя.

BOT_HELP_KICK_REPEAT
	Синтаксис: KICK #канал REPEAT {ON|OFF} [ttb [число_повторов]]
	
	Активируйте  данную  опцию  с  нужными  вам значениями, если хотите
	чтобы бот кикал посетителей за повторы.
	
	Условия  наказания  рассчитываются  следующим образом: пользователь
	должен X раз подряд послать на канал одинаковое сообщение, при этом
	сообщения других пользователей, посланные  в  это же время, никоим
	образом несбрасывают счетчик числа_повторов этого пользователя. 
	
	Если вы  не  укажете параметры режима, будут использованы значения
	по-умолчанию: 3 повтора подряд.
	
	Параметр ttb - это количество киков, по превышению которого бот
	поставит бан. Укажите вместо положительного числа значение "0" или
	"-" если не хотите чтобы бот банил нарушителя.

BOT_HELP_KICK_BADWORDS
	Синтаксис: KICK #канал BADWORDS {ON|OFF} [ttb]
	
	Активируйте  данную  опцию  с  нужными  вам значениями, если хотите
	чтобы бот кикал посетителей за использование на канале "плохих" слов.
	
	База "плохих" слов канала модифицируется посредством дополнительной
	команды BADWORDS. Для более подробной информации смотрите справку
	по %R%S HELP BADWORDS.
	
	Параметр ttb - это количество киков, по превышению которого бот
	поставит бан. Укажите вместо положительного числа значение "0" или
	"-" если не хотите чтобы бот банил нарушителя.
	
BOT_HELP_BADWORDS
	Синтаксис: BADWORDS #канал ADD слово [SINGLE | START | END]
	           BADWORDS #канал DEL {слово | номер_записи | список_записей}
	           BADWORDS #канал LIST [маска | список_записей]
	           BADWORDS #канал CLEAR
	
	Данная команда позволяет модифицировать список "плохих" слов канала.
	Этот список  определяет, за использование каких слов пользователь
	будет  кикнут  ботом, при  условии, конечно, что данная возможность
	активирована.
	Для более подробной информации по активации данного режима смотрите
	%R%S HELP KICK BADWORDS.
	
	Команда BADWORDS ADD добавляет новое слово в список "плохих" слов.
	Вы можете указать дополнительные условия обработки для добавляемого
	слова, например: опциональный параметр SINGLE заставит бота
	учитывать только факт полного употребления данного слова, параметр
	START заставит бота учитывать факт совпадения по маске "слово*"
	(то есть, совпадение какой-либо START-записи из списка с началом
	сказанного слова), ну и последний параметр END будет срабатывать
	при употреблении "*слова". Если вы не укажете ни один из этих
	параметров для добавляемого слова, будет использоваться совпадение
	по "*слову*".
	Примеры:
	
	BADWORDS #chuvaki ADD пес
	-> добавит слово "пес" в список плохих слов и заставит бота кикать
	   не только за употребление самого этого слова, но и любых его
	   словоформ, содержащих в себе оригин (например: пес, песок и т.д.)
	
	BADWORDS #chuvaki ADD пес SINGLE
	-> добавит слово "пес" в список плохих слов и заставит бота кикать
	   только за употребление самого этого слова, и никак иначе.
	
	Команда BADWORDS DEL удаляет указанное слово из списка плохих слов.
	В качестве параметра вы можете указать не только конкретную запись,
	но и номер записи или список записей (см. примеры использования
	команды LIST ниже).
	
	Команда BADWORDS LIST покажет текущий список плохих слов.
	В качестве дополнительного параметра, вы можете указать символьную
	маску, что  позволит  вам  получить  список с конкретными записями,
	попадающими под эту маску, или же, вы можете указать список записей.
	Например:
	
	   BADWORDS #канал LIST 2-5,7-9
	      выведет список записей со 2-ой по 5-ю и с 7-ой по 9-ю.
	
	Команда BADWORDS CLEAR позволяет полностью очистить список слов.

BOT_HELP_SAY
	Синтаксис: SAY #канал текст
	
	Заставляет бота послать указанный текст на указанный #канал.

BOT_HELP_ACT
	Синтаксис: ACT #канал текст
	
	Заставляет бота выполнить команду, эквивалентную команде "/me"
	в IRC-клиенте mIRC. Вы должны указать оба параметра: текст действия
	и #канал.

BOT_SERVADMIN_HELP_BOT
	Синтаксис: BOT ADD ник идент хост реальное_имя
	           BOT CHANGE старый ник новый_ник [идент [хост [реальное_имя]]]
	           BOT DEL ник
	
	Позволяет Администраторам сервисов создавать, изменять или удалять
	ботов, которые в дальнейшем могут быть использованы пользователями
	сети на своих каналах.
	
	Команда BOT ADD добавляет бота с указанным ником, идентом, хостом
	и реальным именем. Так как корректность этих значений не проверяется,
	будьте крайне внимательны при добавлении/изменении хаарктеристик бота.
	
	Команда BOT CHANGE позволяет изменить ник, идент, хост или реальное
	имя бота без необходимости его удаления.
	
	Команда BOT DEL удаляет указанного бота из списка ботов.
	
	Примечание: вы не можете создать бота с ником, который уже кем-то
	зарегистрирован. Если же вы создаете бота с незарегистрированным ником,
	любой, кто использует этот ник на момент создания бота  -  будет тут
	же отключен от сети.

BOT_SERVADMIN_HELP_SET
	
	Список опций, которые могут настраивать только Администраторы сервисов:
	
	    NOBOT         запретить использовать ботов на каком-либо канале.
	    PRIVATE       ограничить указанного бота до использования его
	                  только администрацией сети.

BOT_SERVADMIN_HELP_SET_NOBOT
	Синтаксис: SET #канал NOBOT {ON|OFF}
	
	Активирует/деактивирует режим NOBOT для указанного канала. Включение
	данного режима запретит администарции канала использовать бота на
	своем канале.
	Если бот уже установлен на канале, он будет автоматически удален с
	него сразу же после включения данной опции.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Синтаксис: SET ник_бота PRIVATE {ON|OFF}
	
	Установите данную опцию на указанного бота, если хотите запретить
	обычным пользователям использовать его. Доступ к нему будет только
	у администрации сети.

###########################################################################
#
# HostServ messages
#
###########################################################################
HOST_EMPTY
	Список виртуальных хостов пуст.
HOST_ENTRY
	#%d * Ник: %s * виртуальный хост: %s (%s - %s)
HOST_IDENT_ENTRY
	#%d * Ник: %s * виртуальный хост: %s@%s (%s - %s)
HOST_SET
	Виртуальный хост для %s установлен на %s.
HOST_IDENT_SET
	Виртуальный хост для %s установлен на %s@%s.
HOST_SETALL
	Виртуальный хост для группы %s установлен на %s.
HOST_DELALL
	Все виртуальные хосты для группы %s были удалены.
HOST_DELALL_SYNTAX
	DELALL <ник>.
HOST_IDENT_SETALL
	Виртуальный хост для группы %s установлен в %s@%s.
HOST_SET_ERROR
	Указанный вирутальный хост имеет неверный формат.
HOST_SET_IDENT_ERROR
	Указанный вирутальный идент имеет неверный формат.
HOST_SET_TOOLONG
	Ошибка: длина указанного виртуального хоста превышает порог в %d символов.
HOST_SET_IDENTTOOLONG
	Ошибка: длина указанного виртуального идента превышает порог в %d символов.
HOST_NOREG
	Пользователь %s не найден в сервисной базе данных ников.
HOST_SET_SYNTAX
	SET <ник> <хостмаска>.
HOST_SETALL_SYNTAX
	SETALL <ник> <хостмаска>.
HOST_DENIED
	Доступ запрещен.
HOST_NOT_ASSIGNED
	Виртуальный хост для вашего ника отсутствует.
HOST_ACTIVATED
	Ваш виртуальный хост %s активирован.
HOST_IDENT_ACTIVATED
	Ваш виртуальный хост %s@%s активирован.
HOST_ID
	Идентифицируйтесь к своему нику и повторите запрос на активацию.
HOST_NOT_REGED
	Какой виртуальный хост, ваш ник даже не зарегистрирован...
HOST_DEL
	Виртуальный хост для %s удален.
HOST_DEL_SYNTAX
	DEL <ник>.
HOST_OFF
	Your vhost was removed and the normal cloaking restored.
HOST_OFF_UNREAL
	Ваш виртуальный хост деактивирован. Для ре-активации используйте: /MODE %s +%s
HOST_NO_VIDENT
	IRCd вашей сети не поддерживает виртуальные иденты. Если это не так, сообщите нам (разработчкам) о данном факте.
HOST_GROUP
	Все виртуальные хосты для ников группы %s установлены на %s
HOST_IDENT_GROUP
	Все виртуальные хосты для ников группы %s установлены на %s@%s
HOST_LIST_FOOTER
	Конец списка хостов, записей насчитано: %d
HOST_LIST_RANGE_FOOTER
	Конец списка записей с %d по %d
HOST_LIST_KEY_FOOTER
	Конец списка хостов по маске %s, записей насчитано: %d

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_HELP_CMD_ON
	    ON          Активирует назначенный вам виртуальный хост

HOST_HELP_CMD_OFF
	    OFF         Деактивирует ваш текущий виртуальный хост

HOST_HELP_CMD_GROUP
	    GROUP       Синхронизирует виртуальный хост для всех ников группы

HOST_HELP_CMD_SET
	    SET         Установка виртуального хоста указанному пользователю

HOST_HELP_CMD_SETALL
	    SETALL      Установка вирт. хоста для всех ников указанной группы

HOST_HELP_CMD_DEL
	    DEL         Удаление виртуального хоста

HOST_HELP_CMD_DELALL
	    DELALL      Удаление вирт. хоста у всех ников указанной группы

HOST_HELP_CMD_LIST
	    LIST        Вывод списка всех виртуальных хостов

HOST_OPER_HELP
	не используется.

HOST_ADMIN_HELP
	не используется.

HOST_HELP
	Список команд %S:

HOST_HELP_ON
	Синтаксис: ON
	
	Активирует ваш виртуальный хост, при условии, конечно, что он у вас
	установлен. Сразу после активации, ваш реальный хост/IP-адрес будет
	заменен на виртуальный, и, соответственно, он и будет показан в
	/WHOIS-информации вашего ника.

HOST_HELP_SET
	Синтаксис: SET <ник> <хостмаска>.
	
	Устанавливает указанный виртуальный хост для указанного ника. Если
	IRCd вашей сети подерживает виртуальные иденты, можно использовать
	формат SET <ник> <идент>@<хостмаска> для установки виртуальных
	идента и хоста одновременно.

HOST_HELP_DELALL
	Синтаксис: DELALL <ник>.
	
	Удаляет виртуальные хосты у всех ников указанной группы.

HOST_HELP_SETALL
	Синтаксис: SETALL <ник> <хостмаска>.
	
	Устанавливает виртуальный хост для всех ников указанной группы.Если
	IRCd вашей сети подерживает виртуальные иденты, можно использовать
	формат SETALL <ник> <идент>@<хостмаска> для установки виртуальных
	идента и хоста одновременно.
	
	Примечание: виртуальный  хост  устанавливается  для всех текущих
	ников указанной группы и не будет автоматически обновлен для нового
	ника, внесенного в группу уже после установки виртуального хоста.

HOST_HELP_OFF
	Синтаксис: OFF
	
	Деактивирует ваш текущий виртуальный хост, при условии, конечно,
	что таковой установлен на момент использования команды.

HOST_HELP_DEL
	Синтаксис: DEL <ник>
	
	Удаляет виртуальный хост назначенный указанному нику.

HOST_HELP_LIST
	Синтаксис: LIST [<маска>|<#X-Y>]
	
	Выводит список всех виртуальных хостов и ников, на которые эти хосты
	установлены.
	
	В качестве дополнительного параметра, вы можете указать маску поиска,
	что позволит вам получить только совпадающие по символьной маске
	записи. Помимо этого, вы, так же, можете указать спискок записей в
	формате #X-Y, где X - номер стартовой запись, а Y - конечной.
	
	Например:
	
	   LIST *invis*
	      выведет список записей, ник или вирт. хост которых содержит
	      подстроку *invis* (invisible, covinis и т. д.)
	
	   LIST #2-5
	      выведет список записей со 2-ой по 5-ю.
	
	Лимит одновременно показываемых записей ограничен числом, указанным
	в конфигурационном файле сервисов директивой NSListMax.

HOST_HELP_GROUP
	Синтаксис: GROUP
	
	Эта команда позволяет пользователям установить виртуальный хост
	с их ТЕКУЩЕГО ника на все ники группы.

OPER_SVSNICK_UNSUPPORTED
	IRCd вашей сети не поддерживает возможности команды SVSNICK.

OPER_SQLINE_UNSUPPORTED
	IRCd вашей сети не поддерживает возможности команды SQLINE.

OPER_SVSO_UNSUPPORTED
	IRCd вашей сети не поддерживает возможности команды OLINE.

OPER_UMODE_UNSUPPORTED
	IRCd вашей сети не поддерживает возможности команды UMODE.

OPER_SUPER_ADMIN_NOT_ENABLED
	Директива SuperAdmin отключена в конфигурационном файле сервисов.
