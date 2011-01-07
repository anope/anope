# Portuguese language file.
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
###########################################################
## Translation fully reviewed/completed/updated
## by Ricardo at 01/16/2005.
###
## Tradução totalmente revista/completada/atualizada
## por Ricardo em 16/01/2005.
## Atualizações/Updates:
## * SVN 564 - 01/31/2005.
## * SVN 611 - 03/08/2005.
## * SVN 815 - 05/27/2005.
###########################################################
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
	Português (Portuguese)

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
	Dom
	Seg
	Ter
	Qua
	Qui
	Sex
	Sáb
# %A
STRFTIME_DAYS_LONG
	Domingo
	Segunda
	Terça
	Quarta
	Quinta
	Sexta
	Sábado
# %b
STRFTIME_MONTHS_SHORT
	Jan
	Fev
	Mar
	Abr
	Mai
	Jun
	Jul
	Ago
	Set
	Out
	Nov
	Dez
# %B
STRFTIME_MONTHS_LONG
	Janeiro
	Fevereiro
	Março
	Abril
	Maio
	Junho
	Julho
	Agosto
	Setembro
	Outubro
	Novembro
	Dezembro


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	, 


# Various error messages.
USER_RECORD_NOT_FOUND
	Erro interno - não foi possível processar seu pedido.
UNKNOWN_COMMAND
	Comando desconhecido: %s.
UNKNOWN_COMMAND_HELP
	Comando desconhecido %s. Digite %R%s HELP para ajuda.
SYNTAX_ERROR
	Sintaxe: %s
MORE_INFO
	Digite %R%s HELP %s para maiores informações.
NO_HELP_AVAILABLE
	Nenhuma ajuda disponível para %s.
OBSOLETE_COMMAND
	Este comando não é mais usado; use %s.

BAD_USERHOST_MASK
	A máscara deve ser no formato user@host.
BAD_EXPIRY_TIME
	Tempo de expiração inválido.
USERHOST_MASK_TOO_WIDE
	Máscara %s é muito grande; por favor seja mais específico.

SERVICE_OFFLINE
	%s não está conectado no momento.
READ_ONLY_MODE
	Atenção: Os services estão no modo somente leitura; as mudanças não serão salvas!
PASSWORD_INCORRECT
	Senha incorreta.
INVALID_TARGET
	"/msg %s" is no longer supported.  Use "/msg %s@%s" or "/%s" instead.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Acesso negado.
PERMISSION_DENIED
	Permissão negada.
RAW_DISABLED
	A opção RAW está desabilitada. Se realmente precisar, deve desabilitar a opção DisableRaw na configuração dos Services.

MORE_OBSCURE_PASSWORD
	Por favor tente novamente com uma senha mais difícil. Não use TAB ou espaços.
PASSWORD_TOO_LONG
	Your password is too long. Please try again with a shorter password.

NICK_NOT_REGISTERED
	Seu nick não está registrado.
NICK_NOT_REGISTERED_HELP
	Seu nick não está registrado. Digite %R%s HELP para maiores informações sobre como registrar o nick.
NICK_X_IS_SERVICES
	Nick %s é parte dos Services desta Rede.
NICK_X_NOT_REGISTERED
	Nick %s não está registrado.
NICK_X_IN_USE
	Nick %s está atualmente em uso.
NICK_X_NOT_IN_USE
	Nick %s não está sendo usado.
NICK_X_NOT_ON_CHAN
	%s não está atualmente no canal %s.
NICK_X_FORBIDDEN
	Nick %s não pode ser registrado ou usado.
NICK_X_FORBIDDEN_OPER
	Nick %s foi bloqueado por %s:
	%s
NICK_X_ILLEGAL
	Nick %s é um nick ilegal e não pode ser usado.
NICK_X_TRUNCATED
	Nick %s foi truncado para %d caracteres.
NICK_X_SUSPENDED
	Nick %s está atualmente suspenso.
CHAN_X_NOT_REGISTERED
	O canal %s não está registrado.
CHAN_X_NOT_IN_USE
	O canal %s não existe.
CHAN_X_FORBIDDEN
	O canal %s não pode ser registrado ou usado.
CHAN_X_FORBIDDEN_OPER
	Canal %s foi bloqueado por %s:
	%s
CHAN_X_SUSPENDED
	             Suspenso: [%s] %s
NICK_IDENTIFY_REQUIRED
	Identificação de senha requerida para usar este comando.
	Tente novamente após digitar %R%s IDENTIFY senha.
CHAN_IDENTIFY_REQUIRED
	Identificação de senha requerida para usar este comando.
	Tente novamente após digitar %R%s IDENTIFY %s senha.
	
MAIL_DISABLED
	Os services foram configurados para não enviar e-mail.
MAIL_INVALID
	E-mail para %s é inválido.
MAIL_X_INVALID
	%s não é um endereço de e-mail válido.
MAIL_LATER
	Não foi possível enviar e-mail agora; tente novamente mais tarde.
MAIL_DELAYED
	Por favor aguarde %d segundos e tente novamente.
	
NO_REASON
	Sem motivo
UNKNOWN
	<desconhecido>

# Duration system
DURATION_DAY
	1 dia
DURATION_DAYS
	%d dias
DURATION_HOUR
	1 hora
DURATION_HOURS
	%d horas
DURATION_MINUTE
	1 minuto
DURATION_MINUTES
	%d minutos
DURATION_SECOND
	1 segundo
DURATION_SECONDS
	%d segundos
	
# Human readable expiration
NO_EXPIRE
	não expira
EXPIRES_SOON
	irá expirar na próxima atualização do banco de dados
EXPIRES_M
	expira em %d minutos
EXPIRES_1M
	expira em %d minuto
EXPIRES_HM
	expira em %d horas, %d minutos
EXPIRES_H1M
	expira em %d horas, %d minuto
EXPIRES_1HM
	expira em %d hora, %d minutos
EXPIRES_1H1M
	expira em %d hora, %d minuto
EXPIRES_D
	expira em %d dias
EXPIRES_1D
	expira em %d dia

# Generic Footer message
END_OF_ANY_LIST
	Fim da lista de %s.

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
	Este nick foi registrado por outra pessoa.  Por favor escolha outro.
	(Se este for seu nick, digite %R%s IDENTIFY senha.)
NICK_IS_SECURE
	Este nick está registrado e protegido.  Se este for seu
	nick, digite %R%s IDENTIFY senha.  Caso contrário,
	por favor escolha outro nick.
NICK_MAY_NOT_BE_USED
	Este nick não pode ser usado.  Por favor escolha outro.
FORCENICKCHANGE_IN_1_MINUTE
	Se você não mudar seu nick em 1 minuto, eu irei mudá-lo.
FORCENICKCHANGE_IN_20_SECONDS
	Se você não mudar seu nick em 20 segundos, eu irei mudá-lo.
FORCENICKCHANGE_NOW
	Este nick é registrado; você não pode usá-lo.
FORCENICKCHANGE_CHANGING
	Seu nick está sendo mudado para %s.

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER senha [email]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER senha email
NICK_REGISTRATION_DISABLED
	Desculpe, registros de nick estão temporariamente desativados.
NICK_REGISTRATION_FAILED
	Desculpe, registro de nick falhou.
NICK_REG_PLEASE_WAIT
	Por favor espere %d segundos antes de usar o comando REGISTER novamente.
NICK_CANNOT_BE_REGISTERED
	O nick %s não pode ser registrado.
NICK_ALREADY_REGISTERED
	O nick %s já está registrado!
NICK_REGISTERED
	O nick %s foi registrado em sua conta: %s
NICK_REGISTERED_NO_MASK
	Nick %s registrado.
NICK_PASSWORD_IS
	Sua senha é %s - guarde ela para uso posterior.
NICK_REG_DELAY
	Você deve estar conectado há mais de %d segundos para registrar seu nick.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP alvo senha
NICK_GROUP_DISABLED
	Desculpe, o comando GROUP está temporariamente desativado.
NICK_GROUP_FAILED
	Desculpe, o comando GROUP falhou.
NICK_GROUP_PLEASE_WAIT
	Por favor espere %d segundos antes de usar o comando GROUP novamente.
NICK_GROUP_CHANGE_DISABLED
	Seu nick já está registrado; digite %R%s DROP primeiro.
NICK_GROUP_SAME
	Você já é um membro do grupo %s.
NICK_GROUP_TOO_MANY
	Existem muitos nicks no grupo de %s; liste-os e remova alguns.
	Digite %R%s HELP GLIST e %R%s HELP DROP para
	mais informações.
NICK_GROUP_JOINED
	Você é agora um membro do grupo %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY senha
NICK_IDENTIFY_FAILED
	Desculpe, a identificação falhou.
NICK_IDENTIFY_SUCCEEDED
	Senha aceita - você está agora reconhecido.
NICK_IDENTIFY_EMAIL_REQUIRED
	Você deve agora setar um e-mail para seu nick.
	Este e-mail permitirá que você receba sua senha por ele
	caso você se esqueça dela.
NICK_IDENTIFY_EMAIL_HOWTO
	Digite %R%S SET EMAIL e-mail para setar seu e-mail.
	Sua privacidade é respeitada; seu e-mail não será dado
	para terceiros.
	
# LOGOUT responses
NICK_ALREADY_IDENTIFIED
	Você já está identificado.

# UPDATE responses
NICK_UPDATE_SUCCESS
	Status atualizado (memos, vhost, chmodes, flags).
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	Seu nick foi desconectado.
NICK_LOGOUT_X_SUCCEEDED
	O nick %s foi desconectado.

# DROP responses
NICK_LOGOUT_SERVICESADMIN
	Impossível fazer logout em %s porque ele é um Administrador dos Services.

# DROP responses
NICK_DROP_DISABLED
	Desculpe, o comando DROP está temporariamente desativado.
NICK_DROPPED
	O registro do seu nick foi cancelado.
NICK_X_DROPPED
	O nick %s foi desregistrado.

# SET responses
NICK_SET_SYNTAX
	SET opções parâmetros
NICK_SET_SERVADMIN_SYNTAX
	SET [nick] opções parâmetros
NICK_SET_DISABLED
	Desculpe, a opção SET está temporariamente desativada.
NICK_SET_UNKNOWN_OPTION
	Opção SET desconhecida: %s.
NICK_SET_OPTION_DISABLED
	Opção %s não pode ser usada nesta Rede.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	O novo display deve ser um nick do seu grupo!
NICK_SET_DISPLAY_CHANGED
	O novo display é agora %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Desculpe, não foi possível alterar a senha.
NICK_SET_PASSWORD_CHANGED
	Senha alterada.
NICK_SET_PASSWORD_CHANGED_TO
	Senha alterada para %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE número
NICK_SET_LANGUAGE_UNKNOWN
	Número de linguagem desconhecida %d. Digite %R%s HELP SET LANGUAGE para uma lista das linguagens disponíveis.
NICK_SET_LANGUAGE_CHANGED
	Linguagem alterada para Português.

# SET URL responses
NICK_SET_URL_CHANGED
	URL alterada para %s.
NICK_SET_URL_UNSET
	URL removida.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	E-mail alterado para %s.
NICK_SET_EMAIL_UNSET
	E-mail removido.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	Você não pode remover o e-mail nesta Rede.
	
# SET ICQ responses
NICK_SET_ICQ_CHANGED
	Número de ICQ alterado para %s.
NICK_SET_ICQ_UNSET
	Número de ICQ removido.
NICK_SET_ICQ_INVALID
	%s não é um número válido.
	
# SET GREET responses
NICK_SET_GREET_CHANGED
	Mensagem de entrada alterada para %s.
NICK_SET_GREET_UNSET
	Mensagem de entrada removida.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Proteção de KILL está agora ATIVADA.
NICK_SET_KILL_QUICK
	Proteção de KILL está agora ATIVADA, com tempo de espera reduzido.
NICK_SET_KILL_IMMED
	Proteção de KILL está agora ATIVADA, sem tempo de espera.
NICK_SET_KILL_IMMED_DISABLED
	O comando IMMED não está disponível nesta Rede.
NICK_SET_KILL_OFF
	Proteção de KILL está agora DESATIVADA.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Opção SECURE está agora ATIVADA.
NICK_SET_SECURE_OFF
	Opção SECURE está agora DESATIVADA.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Opção PRIVATE está agora ATIVADA.
NICK_SET_PRIVATE_OFF
	Opção PRIVATE está agora DESATIVADA.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Seu e-mail está agora escondido do comando %s INFO.
NICK_SET_HIDE_EMAIL_OFF
	Seu e-mail irá agora aparecer no comando %s INFO.
NICK_SET_HIDE_MASK_ON
	Seu último endereço usado (user@host) está agora escondido do comando %s INFO.
NICK_SET_HIDE_MASK_OFF
	Seu último endereço usado (user@host) irá agora aparecer no comando %s INFO.
NICK_SET_HIDE_QUIT_ON
	Sua última mensagem de saída está agora escondida do comando %s INFO.
NICK_SET_HIDE_QUIT_OFF
	Sua última mensagem de saída irá agora aparecer no comando %s INFO.
NICK_SET_HIDE_STATUS_ON
	Seu status de acesso nos Services está agora escondido do comando %s INFO.
NICK_SET_HIDE_STATUS_OFF
	Seu status de acesso nos Services irá agora aparecer no comando %s INFO.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_MSG_ON
	Os services irão agora respondê-lo através de mensagens.
NICK_SET_MSG_OFF
	Os services irão agora respondê-lo através de notices.

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
	ACCESS {ADD | DEL | LIST} [máscara]
NICK_ACCESS_ALREADY_PRESENT
	A máscara %s já se encontra em sua lista de acesso.
NICK_ACCESS_REACHED_LIMIT
	Desculpe, você pode ter somente %d entradas em sua lista de acesso.
NICK_ACCESS_ADDED
	%s adicionado em sua lista de acesso.
NICK_ACCESS_NOT_FOUND
	%s não foi encontrado em sua lista de acesso.
NICK_ACCESS_DELETED
	%s removido da sua lista de acesso.
NICK_ACCESS_LIST
	Lista de acesso:
NICK_ACCESS_LIST_X
	Lista de acesso para %s:
NICK_ACCESS_LIST_EMPTY
	Sua lista de acesso está vazia.
NICK_ACCESS_LIST_X_EMPTY
	Lista de acesso para %s está vazia.

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
	%s é %s
NICK_INFO_SERVICES_OPER
	%s é um Operador dos Services.
NICK_INFO_SERVICES_ADMIN
	%s é um Administrador dos Services.
NICK_INFO_SERVICES_ROOT
	%s é um Services Root Administrator.
NICK_INFO_ADDRESS
	   Último endereço visto: %s
NICK_INFO_ADDRESS_ONLINE
	          Está online em: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s está online neste momento.
NICK_INFO_TIME_REGGED
	        Hora de registro: %s
NICK_INFO_LAST_SEEN
	    Último horário visto: %s
NICK_INFO_LAST_QUIT
	Última mensagem de saída: %s
NICK_INFO_URL
	                     URL: %s
NICK_INFO_EMAIL
	                  E-mail: %s
NICK_INFO_VHOST
	                   vHost: %s
NICK_INFO_VHOST2
	                   vHost: %s@%s
NICK_INFO_ICQ
	                     ICQ: %d
NICK_INFO_GREET
	     Mensagem de entrada: %s
NICK_INFO_OPTIONS
	                  Opções: %s
NICK_INFO_EXPIRE
	               Expira em: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Proteção
NICK_INFO_OPT_SECURE
	Segurança
NICK_INFO_OPT_PRIVATE
	Privado
NICK_INFO_OPT_MSG
	Modo mensagem
NICK_INFO_OPT_AUTOOP
	Auto-op
NICK_INFO_OPT_NONE
	Nenhuma
NICK_INFO_NO_EXPIRE
	Este nick não irá expirar.
NICK_INFO_FOR_MORE
	Para informações mais detalhadas, digite %R%s INFO %s ALL.

# LIST responses
NICK_INFO_SUSPENDED
	Este nick está atualmente suspenso, motivo: %s
NICK_INFO_SUSPENDED_NO_REASON
	Este nick está atualmente suspenso

# LIST responses
NICK_LIST_SYNTAX
	LIST opções
NICK_LIST_SERVADMIN_SYNTAX
	LIST opções [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Lista de entradas com %s:
NICK_LIST_RESULTS
	Fim da listagem - %d/%d resultados mostrados.

# ALIST responses
NICK_ALIST_SYNTAX
	ALIST nick
NICK_ALIST_HEADER
	Canais nos quais você tem acesso:
	  Núm  Canal                Nível    Descrição
NICK_ALIST_HEADER_X
	Canais nos quais %s tem acesso:
	  Núm  Canal                Nível    Descrição 
NICK_ALIST_XOP_FORMAT
	  %3d %c%-20s %-8s %s
NICK_ALIST_ACCESS_FORMAT
	  %3d %c%-20s %-8d %s
NICK_ALIST_FOOTER
	Fim da listagem - %d/%d resultados mostrados.

# GLIST responses
NICK_GLIST_HEADER
	Lista de nicks no grupo:
NICK_GLIST_HEADER_X
	Lista de nicks no grupo %s:
NICK_GLIST_FOOTER
	%d nicks no grupo.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (expira em %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nick [senha]
NICK_NO_RECOVER_SELF
	Você não pode usar o comando RECOVER em si mesmo!
NICK_RECOVERED
	Usuário que usava seu nick foi derrubado.
	%R%s RELEASE %s para liberar seu nick antes de %s.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nick [senha]
NICK_RELEASE_NOT_HELD
	Nick %s não está em uso no momento.
NICK_RELEASED
	Seu nick foi derrubado e pode ser usado.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nick [senha]
NICK_NO_GHOST_SELF
	Você não pode usar o comando GHOST em si mesmo!
NICK_GHOST_KILLED
	Seu nick foi derrubado e pode ser usado.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nick
NICK_GETPASS_UNAVAILABLE
	Comando GETPASS não disponível: modo de encriptação ativado.
NICK_GETPASS_PASSWORD_IS
	Senha para %s é %s.

# GETEMAIL responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@emailhost Não use coringas!
NICK_GETEMAIL_EMAILS_ARE
	O nick %s usa o E-mail %s.
NICK_GETEMAIL_NOT_USED
	Nenhum email listado para %s.
	
# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS nick
NICK_SENDPASS_UNAVAILABLE
	Comando SENDPASS não disponível: modo de encriptação ativado.
NICK_SENDPASS_SUBJECT
	Senha do nick (%s)
NICK_SENDPASS_HEAD
	Olá,
NICK_SENDPASS_LINE_1
	Você pediu para que a senha do nick %s fosse enviada por e-mail.
NICK_SENDPASS_LINE_2
	A senha é %s. Por motivos de segurança, recomendamos que você mude sua senha após ler este e-mail.
NICK_SENDPASS_LINE_3
	Se você não souba porque este e-mail lhe foi enviado, por favor ignore-o.
NICK_SENDPASS_LINE_4
	NÃO RESPONDA A ESTE E-MAIL!
NICK_SENDPASS_LINE_5
	Administradores %s.
NICK_SENDPASS_OK
	Senha para %s foi enviada.

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND nick motivo
NICK_SUSPEND_SUCCEEDED
	Nick %s foi suspendido.
NICK_SUSPEND_FAILED
	Impossível suspender o nick %s!

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND nick
NICK_UNSUSPEND_SUCCEEDED
	Nick %s foi liberado.
NICK_UNSUSPEND_FAILED
	Impossível liberar o nick %s!

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID nick [motivo]
NICK_FORBID_SYNTAX_REASON
	FORBID nick motivo
NICK_FORBID_SUCCEEDED
	Nick %s está agora bloqueado.
NICK_FORBID_FAILED
	Não foi possível bloquear o nick %s!

# Nick Registration responses
NICK_REQUESTED
	Esse nick já foi solicitado, por favor verifique seu e-mail pelo passcode
NICK_REG_RESENT
	Seu passcode foi re-enviado para %s.
NICK_REG_UNABLE
	Nick NÃO registrado, por favor tente novamente mais tarde.
NICK_IS_PREREG
	Esse nick está aguardando um código de verificação por e-mail antes de completar o registro.
NICK_ENTER_REG_CODE
	O passcode foi enviado para %s, por favor digite %R%s confirm <passcode> para completar o registro.
NICK_CONFIRM_NOT_FOUND
	O primeiro passo do registro deve ter expirado, por favor use %R%s register <senha> <email> primeiro.
NICK_CONFIRM_INVALID
	Um passcode inválido foi digitado, por favor verifique o e-mail e tente novamente
NICK_REG_MAIL_SUBJECT
	Registro de nick (%s)
NICK_REG_MAIL_HEAD
	Olá,
NICK_REG_MAIL_LINE_1
	Você solicitou o registro do seguinte nick: %s.
NICK_REG_MAIL_LINE_2
	Por favor digite %R%s confirm %s para completar o registro.
NICK_REG_MAIL_LINE_3
	Se você não sabe porque este e-mail lhe foi enviado, por favor ignore-o com sigilo.
NICK_REG_MAIL_LINE_4
	POR FAVOR NÃO RESPONDA A ESTE E-MAIL!
NICK_REG_MAIL_LINE_5
	Administradores %s.
NICK_GETPASS_PASSCODE_IS
	Passcode para %s é %s.
NICK_FORCE_REG
	Nick %s confirmado

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Modo +o automático
CHAN_LEVEL_AUTOVOICE
	Modo +v automático
CHAN_LEVEL_AUTOHALFOP
	Modo +h automático
CHAN_LEVEL_AUTOPROTECT
	Modo +a automático
CHAN_LEVEL_AUTODEOP
	Modo -o automático
CHAN_LEVEL_NOJOIN
	Entrada no canal não permitida
CHAN_LEVEL_INVITE
	Permitido o uso do comando INVITE
CHAN_LEVEL_AKICK
	Permitido o uso do comando AKICK
CHAN_LEVEL_SET
	Permitido o uso do comando SET (não inclui as opções FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Permitido o uso do comando CLEAR
CHAN_LEVEL_UNBAN
	Permitido o uso do comando UNBAN
CHAN_LEVEL_OPDEOP
	Permitido o uso do comando OP/DEOP
CHAN_LEVEL_ACCESS_LIST
	Permitido visualizar a lista de acesso
CHAN_LEVEL_ACCESS_CHANGE
	Permitido modificar a lista de acesso
CHAN_LEVEL_MEMO
	Permitido listar/ler memos do canal
CHAN_LEVEL_ASSIGN
	Permitido associar/desassociar um bot a/de um canal
CHAN_LEVEL_BADWORDS
	Permitido o uso do comando BADWORDS
CHAN_LEVEL_NOKICK
	Nunca ser kickado pelos motivos dos Kickers do bot
CHAN_LEVEL_FANTASIA
	Permitido o uso do comando FANTASY de bots
CHAN_LEVEL_SAY
	Permitido o uso dos comandos SAY e ACT
CHAN_LEVEL_GREET
	Mensagem de entrada
CHAN_LEVEL_VOICEME
	Permitido usar o comando (DE)VOICE em si mesmo
CHAN_LEVEL_VOICE
	Permitido o uso dos comandos VOICE/DEVOICE
CHAN_LEVEL_GETKEY
	Permitido o uso do comando GETKEY
CHAN_LEVEL_OPDEOPME
	Permitido usar o comando (DE)OP em si mesmo
CHAN_LEVEL_HALFOPME
	Permitido usar o comando (DE)HALFOP em si mesmo
CHAN_LEVEL_HALFOP
	Permitido o uso dos comandos HALFOP/DEHALFOP
CHAN_LEVEL_PROTECTME
	Permitido usar o comando (DE)PROTECT em si mesmo
CHAN_LEVEL_PROTECT
	Permitido o uso dos comandos PROTECT/DEPROTECT
CHAN_LEVEL_KICKME
	Permitido usar o comando KICK em si mesmo
CHAN_LEVEL_KICK
	Permitido o uso do comando KICK
CHAN_LEVEL_SIGNKICK
	Sem assinatura no kick quando a opção SIGNKICK LEVEL estiver em uso
CHAN_LEVEL_BANME
	Permitido o uso do comando BAN em si mesmo
CHAN_LEVEL_BAN
	Permitido o uso do comando BAN
CHAN_LEVEL_TOPIC
	Permitido o uso do comando TOPIC
CHAN_LEVEL_INFO
	Permitido o uso do comando INFO com a opção ALL

# Automatic responses
CHAN_IS_REGISTERED
	Este canal foi registrado com o %s.
CHAN_NOT_ALLOWED_OP
	Você não tem permissão para ser operador no canal %s.
CHAN_MAY_NOT_BE_USED
	Este canal não pode ser usado.
CHAN_NOT_ALLOWED_TO_JOIN
	Você não tem permissão para permanecer neste canal.

# REGISTER responses
CHAN_X_INVALID
	Channel %s is not a valid channel.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER canal senha descrição
CHAN_REGISTER_DISABLED
	Desculpe, o registro de canais está temporariamente desativado.
CHAN_REGISTER_NOT_LOCAL
	Canais locais não podem ser registrados.
CHAN_MUST_REGISTER_NICK
	Você deve registrar seu nick primeiro.  Digite %R%s HELP para maiores informações sobre registro de nick.
CHAN_MUST_IDENTIFY_NICK
	Por favor se identifique com o %s primeiro, usando o comando:
	%R%s IDENTIFY senha
CHAN_MAY_NOT_BE_REGISTERED
	O canal %s não pode ser registrado.
CHAN_ALREADY_REGISTERED
	O canal %s já está registrado!
CHAN_MUST_BE_CHANOP
	Você deve ser no mínimo um operador para registrar o canal.
CHAN_REACHED_CHANNEL_LIMIT
	Desculpe, você já alcançou o limite de %d canais registrados.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Desculpe, você já ultrapassou o limite de %d canais registrados.
CHAN_REGISTRATION_FAILED
	Desculpe, o registro falhou.
CHAN_REGISTERED
	O canal %s está registrado sob seu nick: %s
CHAN_PASSWORD_IS
	A senha do seu canal é %s - guarde ela para uso posterior.
CHAN_REGISTER_NONE_CHANNEL
	Você tentou registrar um canal não existente: %s
CHAN_SYMBOL_REQUIRED
	Por favor use o símbolo # quando tentar registrar

# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY canal senha
CHAN_IDENTIFY_FAILED
	Desculpe, a identificação falhou.
CHAN_IDENTIFY_SUCCEEDED
	Senha aceita - você está sendo reconhecido agora como dono do %s.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT canal nick
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT canal [nick]
CHAN_LOGOUT_SUCCEEDED
	Usuário %s foi desconectado do canal %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	Todos os usuários registrados foram desconectados do canal %s (except the channel founder).
CHAN_LOGOUT_FOUNDER_FAILED
	You may not log out of %s (you are founder).

# DROP responses
CHAN_DROP_SYNTAX
	DROP canal
CHAN_DROP_DISABLED
	Desculpe, o comando DROP está temporariamente desativado.
CHAN_DROPPED
	O canal %s foi desregistrado.

# SET responses
CHAN_SET_SYNTAX
	SET canal opções parâmetros
CHAN_SET_DISABLED
	Desculpe, o comando SET está temporariamente desativado.
CHAN_SET_UNKNOWN_OPTION
	Comando SET %s desconhecido.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s possui muitos canais registrados.
CHAN_FOUNDER_CHANGED
	Founder do canal %s alterado para %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Successor do canal %s alterado para %s.
CHAN_SUCCESSOR_UNSET
	Successor do canal %s removido.
CHAN_SUCCESSOR_IS_FOUNDER
	%s não pode ser o successor do canal %s porque ele é o founder.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Desculpe, a mudança de senha falhou.
CHAN_PASSWORD_CHANGED
	A senha do %s foi alterada.
CHAN_PASSWORD_CHANGED_TO
	A senha do %s foi alterada para %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Descrição do canal %s alterada para %s.

# SET URL responses
CHAN_URL_CHANGED
	URL do canal %s alterada para %s.
CHAN_URL_UNSET
	URL do canal %s removida.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	E-mail do canal %s alterado para %s.
CHAN_EMAIL_UNSET
	E-mail do canal %s removido.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Mensagem de entrada para o canal %s alterada.
CHAN_ENTRY_MSG_UNSET
	Mensagem de entrada para o canal %s removida.
	
# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s não é um ban válido.
CHAN_SET_BANTYPE_CHANGED
	Tipo de ban para o canal %s é agora #%d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Modo %c desconhecido e ignorado.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Modo %c ignorado porque você não pode travá-lo.
CHAN_SET_MLOCK_L_REQUIRED
	Você deve travar o modo +l para poder travar o modo +L.
CHAN_SET_MLOCK_K_REQUIRED
	Você deve travar o modo +i para poder travar o modo +K.
CHAN_MLOCK_CHANGED
	Modo MLOCK no canal %s alterado para %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET canal KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Retenção de tópico para %s está agora ATIVADA.
CHAN_SET_KEEPTOPIC_OFF
	Retenção de tópico para %s está agora DESATIVADA.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET canal TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Trava de tópico para %s está agora ATIVADA.
CHAN_SET_TOPICLOCK_OFF
	Trava de tópico para %s está agora DESATIVADA.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET canal PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Opção Peace para %s está agora ATIVADA.
CHAN_SET_PEACE_OFF
	Opção Peace para %s está agora DESATIVADA.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET canal PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Opção Private para %s está agora ATIVADA.
CHAN_SET_PRIVATE_OFF
	Opção Private para %s está agora DESATIVADA.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET canal SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Opção Secure OPs para %s está agora ATIVADA.
CHAN_SET_SECUREOPS_OFF
	Opção Secure OPs para %s está agora DESATIVADA.
	
# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET canal SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Opção Secure Founder para %s está agora ATIVADA.
CHAN_SET_SECUREFOUNDER_OFF
	Opção Secure Founder para %s está agora DESATIVADA.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET canal RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Opção de acesso restrito para %s está agora ATIVADA.
CHAN_SET_RESTRICTED_OFF
	Opção de acesso restrito para %s está agora DESATIVADA.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET canal SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Opção Secure para %s está agora ATIVADA.
CHAN_SET_SECURE_OFF
	Opção Secure para %s está agora DESATIVADA.
	
# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET canal SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Opção Signed Kicks para %s está agora ATIVADA.
CHAN_SET_SIGNKICK_LEVEL
	Opção Signed Kicks para %s está agora ATIVADA, mas depende
	do nível de acesso do usuário que está usando o comando.
CHAN_SET_SIGNKICK_OFF
	Opção Signed Kicks para %s está agora DESATIVADA.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET canal OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Opção Op-notice para %s está agora ATIVADA.
CHAN_SET_OPNOTICE_OFF
	Opção Op-notice para %s está agora DESATIVADA.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET canal XOP {ON | OFF}
CHAN_SET_XOP_ON
	Sistema de acesso xOP para %s está agora ATIVADO.
CHAN_SET_XOP_OFF
	Sistema de acesso xOP para %s está agora DESATIVADO.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET canal NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	O canal %s não irá expirar.
CHAN_SET_NOEXPIRE_OFF
	O canal %s irá expirar.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Desculpe, você pode ter apenas %d entradas de AOP/SOP/VOP em um canal.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	Você não pode usar este comando. Ao invés, use o comando ACCESS.
	Digite %R%s HELP ACCESS para mais informação.
CHAN_XOP_NOT_AVAILABLE
	xOP system is not available.

# AOP messages
CHAN_AOP_SYNTAX
	AOP canal {ADD|DEL|LIST|CLEAR} [nick | entrada]
CHAN_AOP_DISABLED
	Desculpe, a modificação da lista de AOP está temporariamente desabilitada.
CHAN_AOP_NICKS_ONLY
	Lista de AOP do canal só pode conter nicks registrados.
CHAN_AOP_ADDED
	%s adicionado à lista de AOP do %s.
CHAN_AOP_MOVED
	%s movido para a lista de AOP do %s.
CHAN_AOP_NO_SUCH_ENTRY
	Entrada (#%d) não encontrada na lista de AOP do %s.
CHAN_AOP_NOT_FOUND
	Nick %s não encontrado na lista de AOP do %s.
CHAN_AOP_NO_MATCH
	Resultado não encontrado na lista de AOP do %s.
CHAN_AOP_DELETED
	%s deletado da lista de AOP do %s.
CHAN_AOP_DELETED_ONE
	Deletada 1 entrada da lista de AOP do %s.
CHAN_AOP_DELETED_SEVERAL
	Deletadas %d entradas da lista de AOP do %s.
CHAN_AOP_LIST_EMPTY
	Lista de AOP do canal %s está vazia.
CHAN_AOP_LIST_HEADER
	Lista de AOP para %s:
	  Núm  Nick
CHAN_AOP_CLEAR
	Lista de AOP do canal %s foi limpa.

# HOP messages
CHAN_HOP_SYNTAX
	HOP canal {ADD|DEL|LIST|CLEAR} [nick | entrada]
CHAN_HOP_DISABLED
	Desculpe, a modificação da lista de HOP está temporariamente desabilitada.
CHAN_HOP_NICKS_ONLY
	Lista de HOP do canal só pode conter nicks registrados.
CHAN_HOP_ADDED
	%s adicionado à lista de HOP do %s.
CHAN_HOP_MOVED
	%s movido para a lista de HOP do %s.
CHAN_HOP_NO_SUCH_ENTRY
	Entrada (#%d) não encontrada na lista de HOP do %s.
CHAN_HOP_NOT_FOUND
	Nick %s não encontrado na lista de HOP do %s.
CHAN_HOP_NO_MATCH
	Resultado não encontrado na lista de HOP do %s.
CHAN_HOP_DELETED
	%s deletado da lista de HOP do %s.
CHAN_HOP_DELETED_ONE
	Deletada 1 entrada da lista de HOP do %s.
CHAN_HOP_DELETED_SEVERAL
	Deletadas %d entradas da lista de HOP do %s.
CHAN_HOP_LIST_EMPTY
	Lista de HOP do canal %s está vazia.
CHAN_HOP_LIST_HEADER
	Lista de HOP para %s:
	  Núm  Nick
CHAN_HOP_CLEAR
	Lista de HOP do canal %s foi limpa.

# SOP messages
CHAN_SOP_SYNTAX
	SOP canal {ADD|DEL|LIST|CLEAR} [nick | entrada]
CHAN_SOP_DISABLED
	Desculpe, a modificação da lista de SOP está temporariamente desabilitada.
CHAN_SOP_NICKS_ONLY
	Lista de SOP do canal só pode conter nicks registrados.
CHAN_SOP_ADDED
	%s adicionado à lista de SOP do %s.
CHAN_SOP_MOVED
	%s movido para a lista de SOP do %s.
CHAN_SOP_NO_SUCH_ENTRY
	Entrada (#%d) não encontrada na lista de SOP do %s.
CHAN_SOP_NOT_FOUND
	Nick %s não encontrado na lista de SOP do %s.
CHAN_SOP_NO_MATCH
	Resultado não encontrado na lista de SOP do %s.
CHAN_SOP_DELETED
	%s deletado da lista de SOP do %s.
CHAN_SOP_DELETED_ONE
	Deletada 1 entrada da lista de SOP do %s.
CHAN_SOP_DELETED_SEVERAL
	Deletadas %d entradas da lista de SOP do %s.
CHAN_SOP_LIST_EMPTY
	Lista de SOP do canal %s está vazia.
CHAN_SOP_LIST_HEADER
	Lista de SOP para %s:
	  Núm  Nick
CHAN_SOP_CLEAR
	Lista de SOP do canal %s foi limpa.

# VOP messages
CHAN_VOP_SYNTAX
	VOP canal {ADD|DEL|LIST|CLEAR} [nick | entrada]
CHAN_VOP_DISABLED
	Desculpe, a modificação da lista de VOP está temporariamente desabilitada.
CHAN_VOP_NICKS_ONLY
	Lista de VOP do canal só pode conter nicks registrados.
CHAN_VOP_ADDED
	%s adicionado à lista de VOP do %s.
CHAN_VOP_MOVED
	%s movido para a lista de VOP do %s.
CHAN_VOP_NO_SUCH_ENTRY
	Entrada (#%d) não encontrada na lista de VOP do %s.
CHAN_VOP_NOT_FOUND
	Nick %s não encontrado na lista de VOP do %s.
CHAN_VOP_NO_MATCH
	Resultado não encontrado na lista de VOP do %s.
CHAN_VOP_DELETED
	%s deletado da lista de VOP do %s.
CHAN_VOP_DELETED_ONE
	Deletada 1 entrada da lista de VOP do %s.
CHAN_VOP_DELETED_SEVERAL
	Deletadas %d entradas da lista de VOP do %s.
CHAN_VOP_LIST_EMPTY
	Lista de VOP do canal %s está vazia.
CHAN_VOP_LIST_HEADER
	Lista de VOP para %s:
	  Núm  Nick
CHAN_VOP_CLEAR
	Lista de VOP do canal %s foi limpa.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS canal {ADD|DEL|LIST|CLEAR} [nick [nível] | lista-entradas]
CHAN_ACCESS_XOP
	Você não pode usar este comando.
	Ao invés, use os comandos VOP, HOP, AOP e SOP.
	Digite %R%s HELP comando para mais informação.
CHAN_ACCESS_XOP_HOP
	You can't use this command. 
	Use the AOP, SOP, HOP and VOP commands instead.
	Type %R%s HELP command for more information.
CHAN_ACCESS_DISABLED
	Desculpe, o registro de canais está temporariamente desativado.
CHAN_ACCESS_LEVEL_NONZERO
	O nível de acesso deve ser diferente de zero.
CHAN_ACCESS_LEVEL_RANGE
	O nível de acesso deve ser entre %d e %d inclusive.
CHAN_ACCESS_NICKS_ONLY
	A lista de acesso do canal deve conter apenas nicks registrados.
CHAN_ACCESS_REACHED_LIMIT
	Desculpe, você pode ter apenas %d entradas na lista de acesso do canal.
CHAN_ACCESS_LEVEL_UNCHANGED
	Nível de acesso para %s no %s não alterado %d.
CHAN_ACCESS_LEVEL_CHANGED
	Nível de acesso para %s no %s alterado para %d.
CHAN_ACCESS_ADDED
	%s adicionado à lista de acesso do %s com o nível %d.
CHAN_ACCESS_NO_SUCH_ENTRY
	Nenhuma entrada com (#%d) encontrada na lista de acesso do %s.
CHAN_ACCESS_NOT_FOUND
	%s não foi encontrado na lista de acesso do %s.
CHAN_ACCESS_NO_MATCH
	Resultado não encontrado na lista de acesso do %s.
CHAN_ACCESS_DELETED
	%s removido da lista de acesso do %s.
CHAN_ACCESS_DELETED_ONE
	Removida 1 entrada da lista de acesso do %s.
CHAN_ACCESS_DELETED_SEVERAL
	Removidas %d entradas da lista de acesso do %s.
CHAN_ACCESS_LIST_EMPTY
	Lista de acesso do canal %s está vazia.
CHAN_ACCESS_LIST_HEADER
	Lista de acesso para %s:
	  Num   Nív  Nick
CHAN_ACCESS_LIST_FOOTER
	Fim da lista de acesso.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	Lista de acesso do canal %s apagada.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK canal {ADD|STICK|UNSTICK|DEL|LIST|VIEW|ENFORCE|CLEAR} [nick-ou-máscara] [motivo]
CHAN_AKICK_DISABLED
	Desculpe, o comando AKICK está temporariamente desativado.
CHAN_AKICK_ALREADY_EXISTS
	%s já existe na lista de akick do %s.
CHAN_AKICK_REACHED_LIMIT
	Desculpe, você pode ter apenas %d akicks na lista do canal.
CHAN_AKICK_ADDED
	%s adicionado à lista de akick do %s.
CHAN_AKICK_NO_SUCH_ENTRY
	Nenhuma entrada com (#%d) na lista de akick do %s.
CHAN_AKICK_NOT_FOUND
	%s não encontrado na lista de akick do %s.
CHAN_AKICK_NO_MATCH
	Nenhum resultado encontrado na lista de akick do %s.
CHAN_AKICK_STUCK
	%s está agora sempre ativo no canal %s.
CHAN_AKICK_UNSTUCK
	%s não está mais sempre ativo no canal %s.
CHAN_AKICK_DELETED
	%s removido da lista de akick do %s.
CHAN_AKICK_DELETED_ONE
	Removida 1 entrada da lista de akick do %s.
CHAN_AKICK_DELETED_SEVERAL
	Removidas %d entradas da lista de akick do %s.
CHAN_AKICK_LIST_EMPTY
	Lista de akick do %s vazia.
CHAN_AKICK_LIST_HEADER
	Lista de akick para %s:
CHAN_AKICK_LIST_FORMAT
	  %3d %s %s
CHAN_AKICK_VIEW_FORMAT
	%3d %s (por %s em %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (stuck) (por %s em %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE para %s terminado; %d usuários afetados.
CHAN_AKICK_CLEAR
	Lista de akick do canal %s foi limpa.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS canal {SET | DIS[ABLE] | LIST | RESET} [item [nível]]
CHAN_LEVELS_XOP
	Este comando não tem utilidade neste modo.
CHAN_LEVELS_RANGE
	O nível deve ser entre %d e %d inclusive.
CHAN_LEVELS_CHANGED
	Nível para %s no canal %s alterado para %d.
CHAN_LEVELS_UNKNOWN
	Level %s desconhecido. Digite %R%s HELP LEVELS DESC para uma lista de Levels válidos.
CHAN_LEVELS_DISABLED
	%s desabilitado no canal %s.
CHAN_LEVELS_LIST_HEADER
	Opções de nível de acesso para o canal %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (desabilitado)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (founder apenas)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	Níveis de acesso para o %s redefinidos para o padrão.

# Status Messages
CHAN_STATUS_SYNTAX
	STATUS ERROR Erro de sintaxe. Use a sintaxe STATUS canal nick.
CHAN_STATUS_NOT_REGGED
	STATUS ERROR Canal %s não está registrado
CHAN_STATUS_FORBIDDEN
	STATUS ERROR Canal %s forbidado
CHAN_STATUS_NOTONLINE
	STATUS ERROR Nick %s não está online
CHAN_STATUS_INFO
	STATUS %s %s %d

# INFO responses
CHAN_INFO_SYNTAX
	INFO canal [ALL]
CHAN_INFO_HEADER
	Informações do canal %s:
CHAN_INFO_FOUNDER
	             Fundador: %s (%s)
CHAN_INFO_NO_FOUNDER
	             Fundador: %s
CHAN_INFO_SUCCESSOR
	             Sucessor: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	             Sucessor: %s
CHAN_INFO_DESCRIPTION
	            Descrição: %s
CHAN_INFO_ENTRYMSG
	  Mensagem de entrada: %s
CHAN_INFO_TIME_REGGED
	           Registrado: %s
CHAN_INFO_LAST_USED
	           Último uso: %s
CHAN_INFO_LAST_TOPIC
	        Último tópico: %s
CHAN_INFO_TOPIC_SET_BY
	           Tópico por: %s
CHAN_INFO_URL
	                  URL: %s
CHAN_INFO_EMAIL
	               E-mail: %s
CHAN_INFO_BANTYPE
	          Tipo de ban: %d
CHAN_INFO_OPTIONS
	               Opções: %s
CHAN_INFO_OPT_KEEPTOPIC
	Retenção de tópico
CHAN_INFO_OPT_OPNOTICE
	Op-notice
CHAN_INFO_OPT_PEACE
	Paz
CHAN_INFO_OPT_PRIVATE
	Privado
CHAN_INFO_OPT_RESTRICTED
	Acesso restrito
CHAN_INFO_OPT_SECURE
	Seguro
CHAN_INFO_OPT_SECUREOPS
	OPs Seguros
CHAN_INFO_OPT_SECUREFOUNDER
	Founder Seguro
CHAN_INFO_OPT_SIGNKICK
	Kicks assinados
CHAN_INFO_OPT_TOPICLOCK
	Trava de tópico
CHAN_INFO_OPT_XOP
	Sistema de acesso xOP
CHAN_INFO_OPT_NONE
	Nenhuma
CHAN_INFO_MODE_LOCK
	        Trava de modo: %s
CHAN_INFO_EXPIRE
	            Expira em: %s
CHAN_INFO_NO_EXPIRE
	Este canal não irá expirar.

# LIST responses
CHAN_LIST_SYNTAX
	LIST padrão
CHAN_LIST_SERVADMIN_SYNTAX
	LIST padrão [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
CHAN_LIST_HEADER
	Lista de entradas com %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Fim da lista - %d/%d resultados mostrados.
	
# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE canal

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN canal
CHAN_UNBANNED
	Você não está mais banido do %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC canal [tópico]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR canal opção
CHAN_CLEARED_BANS
	Todos os bans do canal %s foram removidos.
CHAN_CLEARED_EXCEPTS
	Todos os excepts do canal %s foram removidos.
CHAN_CLEARED_MODES
	Todos os modos no canal %s foram removidos.
CHAN_CLEARED_OPS
	Modo +o foi retirado do canal %s.
CHAN_CLEARED_HOPS
	Modo +h foi retirado do canal %s.
CHAN_CLEARED_VOICES
	Modo +v foi retirado do canal %s.
CHAN_CLEARED_USERS
	Todos os usuários foram kickados do canal %s.
CHAN_CLEARED_INVITES
	Todos os Invites do canal %s foram removidos.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS canal
CHAN_GETPASS_UNAVAILABLE
	Comando GETPASS indisponível: modo de encriptação em uso.
CHAN_GETPASS_PASSWORD_IS
	Senha do canal %s é %s.
	
# GETKEY responses
CHAN_GETKEY_SYNTAX
	GETKEY canal
CHAN_GETKEY_NOKEY
	O canal %s não tem key.
CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS canal
CHAN_SENDPASS_UNAVAILABLE
	Comando SENDPASS indisponível: modo de encriptação em uso.
CHAN_SENDPASS_SUBJECT
	Senha do canal (%s)
CHAN_SENDPASS_HEAD
	Olá,
CHAN_SENDPASS_LINE_1
	Você pediu para receber a senha do canal %s por e-mail.
CHAN_SENDPASS_LINE_2
	A senha é %s. Por motivos de segurança, recomendamos que você mude a senha assim que receber este e-mail.
CHAN_SENDPASS_LINE_3
	Se você não sabe porque este e-mail lhe foi enviado, por favor ignore-o.
CHAN_SENDPASS_LINE_4
	POR FAVOR NÃO RESPONDA ESTE E-MAIL!
CHAN_SENDPASS_LINE_5
	Administradores do %s.
CHAN_SENDPASS_OK
	Senha do canal %s foi enviada.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID canal [motivo]
CHAN_FORBID_SYNTAX_REASON
	FORBID canal motivo
CHAN_FORBID_SUCCEEDED
	Canal %s está agora bloqueado.
CHAN_FORBID_FAILED
	Não foi possível bloquear o canal %s!

# Misc responses
CHAN_FORBID_REASON
	Este canal foi forbidado.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND canal [motivo]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND canal motivo
CHAN_SUSPEND_SUCCEEDED
	Canal %s está agora suspenso.
CHAN_SUSPEND_FAILED
	Impossível suspender o canal %s!
CHAN_SUSPEND_REASON
	Este canal foi suspenso.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND canal
CHAN_UNSUSPEND_ERROR
	Não foi encontrado "#" em frente ao nome do canal.
CHAN_UNSUSPEND_SUCCEEDED
	Canal %s está agora liberado.
CHAN_UNSUSPEND_FAILED
	Impossível liberar canal %s!

# Misc responses
CHAN_EXCEPTED
	%s combina com uma Except no %s e não pode ser banido até que a Except sejá removida.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	Você tem 1 novo memo.
MEMO_HAVE_NEW_MEMOS
	Você tem %d novos memos.
MEMO_TYPE_READ_LAST
	Digite %R%s READ LAST para ler.
MEMO_TYPE_READ_NUM
	Digite %R%s READ %d para ler.
MEMO_TYPE_LIST_NEW
	Digite %R%s LIST NEW para listar.
MEMO_AT_LIMIT
	Atenção: Você atingiu seu número máximo de memos (%d). Não será possível receber novos memos enquanto você não apagar alguns.
MEMO_OVER_LIMIT
	Atenção: Você ultrapassou seu número máximo de memos (%d). Não será possível receber novos memos enquanto você não apagar alguns.
MEMO_X_MANY_NOTICE
	Existem %d memos para o canal %s.
MEMO_X_ONE_NOTICE
	Existe %d memo para o canal %s.
MEMO_NEW_X_MEMO_ARRIVED
	Foi recebido um novo memo para o canal %s.
	Digite %R%s READ %s %d para ler.
MEMO_NEW_MEMO_ARRIVED
	Você recebeu um novo memo de %s.
	Digite %R%s READ %d para ler.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Você não possui memos.
MEMO_X_HAS_NO_MEMOS
	%s não possui memos.
MEMO_DOES_NOT_EXIST
	Memo %d não existe!
MEMO_LIST_NOT_FOUND
	Nenhum memo encontrado.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {nick | canal} texto
MEMO_SEND_DISABLED
	Desculpe, o envio de memos está temporariamente desabilitado.
MEMO_SEND_PLEASE_WAIT
	Por favor aguarde %d segundos antes de usar o comando SEND novamente.
MEMO_X_GETS_NO_MEMOS
	%s não pode receber memos.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s possui muitos memos atualmente e não pode receber mais nenhum.
MEMO_SENT
	Memo enviado para %s.
MEMO_MASS_SENT
	Uma mensagem global foi enviada a todos os usuários registrados.
	
# STAFF responses
MEMO_STAFF_SYNTAX
	STAFF memo-text
	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {nick | canal}
MEMO_CANCEL_DISABLED
	Desculpe, o comando CANCEL está temporariamente desativado.
MEMO_CANCEL_NONE
	Nenhum memo foi cancelado.
MEMO_CANCELLED
	Último memo enviado para %s foi cancelado.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [canal] [list | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	Você não possui novos memos.
MEMO_X_HAS_NO_NEW_MEMOS
	%s não possui novos memos.
MEMO_LIST_MEMOS
	Memos para %s. Para ler, digite: %R%s READ núm
MEMO_LIST_NEW_MEMOS
	Novos memos para %s. Para ler, digite: %R%s READ núm
MEMO_LIST_CHAN_MEMOS
	Memos para %s. Para ler, digite: %R%s READ %s núm
MEMO_LIST_CHAN_NEW_MEMOS
	Novos memos para %s. Para ler, digite: %R%s READ %s núm
MEMO_LIST_HEADER
	 Núm  Remetente            Data/Hora
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [canal] {list | LAST | NEW}
MEMO_HEADER
	Memo %d de %s (%s). Para apagar, digite: %R%s DEL %d
MEMO_CHAN_HEADER
	Memo %d de %s (%s). Para apagar, digite: %R%s DEL %s %d
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [canal] {núm | list | ALL}
MEMO_DELETED_NONE
	Nenhum memo apagado.
MEMO_DELETED_ONE
	Memo %d foi apagado.
MEMO_DELETED_SEVERAL
	Memos %s foram apagados.
MEMO_DELETED_ALL
	Todos os seus memos foram apagados.
MEMO_CHAN_DELETED_ALL
	Todos os memos para o canal %s foram apagados.

# SET responses
MEMO_SET_SYNTAX
	SET opção parâmetros
MEMO_SET_DISABLED
	Desculpe, a opção SET está temporariamente desativada.
MEMO_SET_UNKNOWN_OPTION
	Comando SET desconhecido: %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
MEMO_SET_NOTIFY_ON
	%s irá agora notificá-lo de novos memos quando você conectar, quando eles lhe forem enviados ou quando desativar o /AWAY.
MEMO_SET_NOTIFY_LOGON
	%s irá agora notificá-lo de novos memos quando você conectar ou quando desativar o /AWAY.
MEMO_SET_NOTIFY_NEW
	%s irá agora notificá-lo de novos memos quando eles lhe forem enviados.
MEMO_SET_NOTIFY_OFF
	%s não irá mais notificá-lo de novos memos.
MEMO_SET_NOTIFY_MAIL
	Você será notificado sobre novos memos por email.
MEMO_SET_NOTIFY_NOMAIL
	Você não será mais notificado por email.
MEMO_SET_NOTIFY_INVALIDMAIL
	Não há endereço de email ajustado para o seu nick.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [canal] limite
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [usuário | canal] {limite | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Você não tem permissão para mudar seu limite de memos.
MEMO_SET_LIMIT_FORBIDDEN
	O limite de memos para %s não pode ser mudado.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Você não pode colocar seu limite de memos maior que %d.
MEMO_SET_LIMIT_TOO_HIGH
	Você não pode colocar o limite de memos para %s maior que %d.
MEMO_SET_LIMIT_OVERFLOW
	Limite de memos muito grande; limitando em %d.
MEMO_SET_YOUR_LIMIT
	Seu limite de memo é agora de %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Você não poderá mais receber memos.
MEMO_UNSET_YOUR_LIMIT
	Seu limite de memos foi desativado.
MEMO_SET_LIMIT
	Limite de memos para %s é agora de %d.
MEMO_SET_LIMIT_ZERO
	Limite de memos para %s é agora de 0.
MEMO_UNSET_LIMIT
	Limite de memos desativado para %s.

# INFO responses
MEMO_INFO_SYNTAX
	INFO [canal]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [nick | canal]
MEMO_INFO_NO_MEMOS
	Você não possui nenhum memo atualmente.
MEMO_INFO_MEMO
	Você possui atualmente 1 memo.
MEMO_INFO_MEMO_UNREAD
	Você possui atualmente 1 memo, e este ainda não foi lido.
MEMO_INFO_MEMOS
	Você possui atualmente %d memos.
MEMO_INFO_MEMOS_ONE_UNREAD
	Você possui atualmente %d memos, dos quais 1 não foi lido.
MEMO_INFO_MEMOS_SOME_UNREAD
	Você possui atualmente %d memos, dos quais %d não foram lidos.
MEMO_INFO_MEMOS_ALL_UNREAD
	Você possui atualmente %d memos; dos quais nenhum foi lido.
MEMO_INFO_LIMIT
	Seu limite de memos é de %d.
MEMO_INFO_HARD_LIMIT
	Seu limite de memos é de %d, e não pode ser alterado.
MEMO_INFO_LIMIT_ZERO
	Seu limite de memos é de 0; você não pode receber novos memos.
MEMO_INFO_HARD_LIMIT_ZERO
	Seu limite de memos é de 0; você não pode receber novos memos. Você não pode alterar esse limite.
MEMO_INFO_NO_LIMIT
	Você não possui limite de memos que pode guardar.
MEMO_INFO_NOTIFY_OFF
	Você será notificado de novos memos.
MEMO_INFO_NOTIFY_ON
	Você será notificado de novos memos quando conectar e quando eles forem enviados.
MEMO_INFO_NOTIFY_RECEIVE
	Você será notificado de novos memos quando eles forem enviados.
MEMO_INFO_NOTIFY_SIGNON
	Você será notificado de novos memos quando conectar.
MEMO_INFO_X_NO_MEMOS
	%s não possui nenhum memo atualmente.
MEMO_INFO_X_MEMO
	%s possui atualmente 1 memo.
MEMO_INFO_X_MEMO_UNREAD
	%s possui atualmente 1 memo, e ainda não foi lido.
MEMO_INFO_X_MEMOS
	%s possui atualmente %d memos.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s possui atualmente %d memos, dos quais 1 não foi lido.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s possui atualmente %d memos, dos quais %d não foram lidos.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s possui atualmente %d memos; dos quais nenhum foi lido.
MEMO_INFO_X_LIMIT
	Limite de memos para %s é de %d.
MEMO_INFO_X_HARD_LIMIT
	Limite de memos para %s é de %d, e não pode ser alterado.
MEMO_INFO_X_NO_LIMIT
	%s não possui limite de memos.
MEMO_INFO_X_NOTIFY_OFF
	%s não é notificado de novos memos.
MEMO_INFO_X_NOTIFY_ON
	%s é notificado de novos memos quando conecta e quando eles são enviados.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s é notificado de novos memos quando eles são enviados.
MEMO_INFO_X_NOTIFY_SIGNON
	%s é notificado de novos memos quando conecta.
	
# Memo2Mail responses
MEMO_MAIL_SUBJECT
	Novo memo
MEMO_MAIL_TEXT1
	Olá %s
MEMO_MAIL_TEXT2
	Você recebeu um novo memo de %s. Este é seu memo número %d.
MEMO_MAIL_TEXT3
	Mensagem do memo:

# RSEND responses
MEMO_RSEND_PLEASE_WAIT
	Por favor aguarde %d segundos antes de usar o comando RSEND novamente.
MEMO_RSEND_DISABLED
	Desculpe, RSEND foi desabilitado nesta rede.
MEMO_RSEND_SYNTAX
	RSEND {nick | canal} mensagem
MEMO_RSEND_NICK_MEMO_TEXT
	[Auto-Memo] O memo que você enviou foi lido.
MEMO_RSEND_CHAN_MEMO_TEXT
	[Auto-Memo] O memo que você enviou para %s foi lido.
MEMO_RSEND_USER_NOTIFICATION
	Um memo de notificação foi enviado para %s informando que você leu seu memo.

# CHECK responses
MEMO_CHECK_SYNTAX
	CHECK nick
MEMO_CHECK_NOT_READ
	O último memo que você enviou para %s (enviado em %s) ainda não foi lido.
MEMO_CHECK_READ
	O último memo que você enviou para %s (enviado em %s) foi lido.
MEMO_CHECK_NO_MEMO
	O nick %s não possui nenhum memo seu.
MEMO_NO_RSEND_SELF
	Você não pode pedir uma notificação ao enviar uma mensagem a si próprio.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
BOT_DOES_NOT_EXIST
	Bot %s não existe.
BOT_NOT_ASSIGNED
	Você precisa primeiro associar um bot ao canal antes de usar este comando.
	Digite %R%S HELP ASSIGN para maiores informações.
BOT_NOT_ON_CHANNEL
	Bot não está no canal %s.
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	Não use a palavra %s neste canal!
BOT_REASON_BADWORD_GENTLE
	Cuidado com suas palavras!
BOT_REASON_BOLD
	Não use negritos neste canal!
BOT_REASON_CAPS
	Desligue o CAPS LOCK!
BOT_REASON_COLOR
	Não use cores neste canal!
BOT_REASON_FLOOD
	Pare de floodar!
BOT_REASON_REPEAT
	Pare de ficar repetindo!
BOT_REASON_REVERSE
	Por favor não use reversos no canal!
BOT_REASON_UNDERLINE
	Não use sublinhados neste canal!
	
# !seen replies
BOT_SEEN_BOT
	Você me encontrou, %s!
BOT_SEEN_YOU
	Procurando por si mesmo %s?
BOT_SEEN_ON_CHANNEL
	%s está no canal neste momento!
BOT_SEEN_ON_CHANNEL_AS
	%s está no canal neste momento (como %s) ! 
BOT_SEEN_ON
	%s foi visto aqui pela última vez %s atrás.
BOT_SEEN_NEVER
	Eu nunca vi %s neste canal.
BOT_SEEN_UNKNOWN
	Eu não sei quem é %s.

# BOT responses
BOT_BOT_SYNTAX
	
	BOT ADD nick user host real
	BOT CHANGE nickantigo nicknovo [user [host [real]]]
	BOT DEL nick
BOT_BOT_ALREADY_EXISTS
	Bot %s já existe.
BOT_BOT_CREATION_FAILED
	Desculpe, criação do bot falhou.
BOT_BOT_READONLY
	Desculpe, comando CHANGE temporariamente desativado.
BOT_BOT_ADDED
	%s!%s@%s (%s) adicionado à lista de bots.
BOT_BOT_ANY_CHANGES
	A informação antiga é igual a nova.
BOT_BOT_CHANGED
	Bot %s foi alterado para %s!%s@%s (%s)
BOT_BOT_DELETED
	Bot %s foi removido.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Lista de bots:
BOT_BOTLIST_PRIVATE_HEADER
	Bots reservados aos Operadores de IRC:
BOT_BOTLIST_FOOTER
	%d bots disponíveis.
BOT_BOTLIST_EMPTY
	Não existem bots disponíveis no momento.
	Peça a um Administrador dos Services para criar mais bots!
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN canal nick
BOT_ASSIGN_READONLY
	Desculpe, mas o comando ASSIGN está temporariamente desativado.
BOT_ASSIGN_ALREADY
	Bot %s já está associado ao canal %s.
BOT_ASSIGN_ASSIGNED
	Bot %s foi associado ao canal %s.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN canal
BOT_UNASSIGN_UNASSIGNED
	Não possui mais nenhum bot associado ao canal %s.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {canal | nick}
BOT_INFO_NOT_FOUND
	%s não é um bot válido ou um canal registrado.
BOT_INFO_BOT_HEADER
	Informações do bot %s:
BOT_INFO_BOT_MASK
	       Máscara: %s@%s
BOT_INFO_BOT_REALNAME
	      RealName: %s
BOT_INFO_BOT_CREATED
	        Criado: %s
BOT_INFO_BOT_USAGE
	      Usado em: %d canal(is)
BOT_INFO_BOT_OPTIONS
	        Opções: %s
BOT_INFO_OPT_PRIVATE
	Privado
BOT_INFO_CHAN_HEADER
	Informações do canal %s:
BOT_INFO_CHAN_BOT
	           Nick do bot: %s
BOT_INFO_CHAN_BOT_NONE
	           Nick do bot: nenhum associado no momento.
BOT_INFO_CHAN_KICK_BADWORDS
	     Kick por palavrão: %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	     Kick por palavrão: %s (%d kick(s) para banir)
BOT_INFO_CHAN_KICK_BOLDS
	      Kick por negrito: %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	      Kick por negrito: %s (%d kick(s) para banir)
BOT_INFO_CHAN_KICK_CAPS_ON
	         Kick por Caps: %s (mínimo %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	         Kick por Caps: %s (%d kick(s) para banir; mínimo %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	         Kick por Caps: %s
BOT_INFO_CHAN_KICK_COLORS
	        Kick por cores: %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	        Kick por cores: %s (%d kick(s) para banir)
BOT_INFO_CHAN_KICK_FLOOD_ON
	        Kick por flood: %s (%d linhas em %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	        Kick por flood: %s (%d kick(s) para banir; %d linhas em %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	        Kick por flood: %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	    Kick por repetição: %s (%d vezes)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	    Kick por repetição: %s (%d kick(s) para banir; %d vezes)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	    Kick por repetição: %s
BOT_INFO_CHAN_KICK_REVERSES
	     Kick por reversos: %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	     Kick por reversos: %s (%d kick(s) para banir)
BOT_INFO_CHAN_KICK_UNDERLINES
	   Kick por sublinhado: %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	   Kick por sublinhado: %s (%d kick(s) para banir)
BOT_INFO_ACTIVE
	ativado
BOT_INFO_INACTIVE
	desativado
BOT_INFO_CHAN_OPTIONS
	                Opções: %s
BOT_INFO_OPT_DONTKICKOPS
	Proteção de OPs
BOT_INFO_OPT_DONTKICKVOICES
	Proteção de Voices
BOT_INFO_OPT_FANTASY
	Fantasia
BOT_INFO_OPT_GREET
	Mensagem de entrada
BOT_INFO_OPT_NOBOT
	Sem bot
BOT_INFO_OPT_SYMBIOSIS
	Simbiose
BOT_INFO_OPT_NONE
	Nenhuma
	
# SET responses
BOT_SET_SYNTAX
	SET canal opções configurações
BOT_SET_DISABLED
	Desculpe, o comando SET está temporariamente desativado.
BOT_SET_UNKNOWN
	Opção desconhecida: %s.
	Digite %R%S HELP SET para maiores informações.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET canal DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Bot não irá kickar OPs no canal %s.
BOT_SET_DONTKICKOPS_OFF
	Bot irá kickar OPs no canal %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET canal DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Bot não irá kickar voices no canal %s.
BOT_SET_DONTKICKVOICES_OFF
	Bot irá kickar voices no canal %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET canal FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Modo Fantasia está agora ATIVADO no canal %s.
BOT_SET_FANTASY_OFF
	Modo Fantasia está agora DESATIVADO no canal %s.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET canal GREET {ON|OFF}
BOT_SET_GREET_ON
	Modo mensagem de entrada está agora ATIVADO no canal %s.
BOT_SET_GREET_OFF
	Modo mensagem de entrada está agora DESATIVADO no canal %s.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET canal NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Modo NOBOT está agora ATIVADO no canal %s.
BOT_SET_NOBOT_OFF
	Modo NOBOT está agora DESATIVADO no canal %s.
	
# SET SYMBIOSIS
BOT_SET_PRIVATE_SYNTAX
	SET canal PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Modo Privado do bot %s está agora ATIVADO.
BOT_SET_PRIVATE_OFF
	Modo Privado do bot %s está agora DESATIVADO.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET canal SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Modo SYMBIOSIS está agora ATIVADO no canal %s.
BOT_SET_SYMBIOSIS_OFF
	Modo SYMBIOSIS está agora DESATIVADO no canal %s.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK canal opções {ON|OFF} [configurações]
BOT_KICK_DISABLED
	Desculpe, as configurações do kicker estão temporariamente desativadas.
BOT_KICK_UNKNOWN
	Opção desconhecida: %s.
	Digite %R%S HELP KICK para maiores informações.
BOT_KICK_BAD_TTB
	%s não pode ser colocado como tempo para ban.

BOT_KICK_BADWORDS_ON
	O bot irá agora kickar por palavrões. Use o comando
	BADWORDS para adicionar ou remover um palavrão.
BOT_KICK_BADWORDS_ON_BAN
	O bot irá agora kickar por palavrões, e irá setar um ban 
	após %d kicks no mesmo usuário. Use o comando BADWORDS
	para adicionar ou remover um palavrão.
BOT_KICK_BADWORDS_OFF
	O bot não irá mais kickar por palavrões.
BOT_KICK_BOLDS_ON
	O bot irá agora kickar por negritos.
BOT_KICK_BOLDS_ON_BAN
	O bot irá agora kickar por negritos, e irá setar um ban 
	após %d kicks no mesmo usuário.
BOT_KICK_BOLDS_OFF
	O bot não irá mais kickar por negritos.
BOT_KICK_CAPS_ON
	O bot irá agora kickar por caps (deve ter no mínimo
	%d caracteres e %d%% na mensagem inteira).
BOT_KICK_CAPS_ON_BAN
	O bot irá agora kickar por caps (deve ter no mínimo
	%d caracteres e %d%% na mensagem inteira), e irá setar
	um ban após %d kicks no mesmo usuário.
BOT_KICK_CAPS_OFF
	O bot não irá mais kickar por caps.
BOT_KICK_COLORS_ON
	O bot irá agora kickar por cores.
BOT_KICK_COLORS_ON_BAN
	O bot irá agora kickar por cores, e irá setar um ban após %d 
	kicks no mesmo usuário.
BOT_KICK_COLORS_OFF
	O bot não irá mais kickar por cores.
BOT_KICK_FLOOD_ON
	O bot irá agora kickar por flood (%d linhas em %d segundos).
BOT_KICK_FLOOD_ON_BAN
	O bot irá agora kickar por flood (%d linhas em %d segundos), e 
	setar um ban após %d kicks no mesmo usuário.
BOT_KICK_FLOOD_OFF
	O bot não irá mais kickar por flood.
BOT_KICK_REPEAT_ON
	O bot irá agora kickar por repetição (usuários que escreverem
	%d vezes a mesma coisa).
BOT_KICK_REPEAT_ON_BAN
	O bot irá agora kickar por repetição (usuários que escreverem
	%d vezes a mesma coisa), e irá setar um ban após %d 
	kicks no mesmo usuário.
BOT_KICK_REPEAT_OFF
	O bot não irá mais kickar por repetição.
BOT_KICK_REVERSES_ON
	O bot irá agora kickar por reversos.
BOT_KICK_REVERSES_ON_BAN
	O bot irá agora kickar por reversos, e irá setar um ban após %d 
	kicks no mesmo usuário.
BOT_KICK_REVERSES_OFF
	O bot não irá mais kickar por reversos.
BOT_KICK_UNDERLINES_ON
	O bot irá agora kickar por sublinhados.
BOT_KICK_UNDERLINES_ON_BAN
	O bot irá agora kickar por sublinhados, e irá setar um ban após %d 
	kicks no mesmo usuário.
BOT_KICK_UNDERLINES_OFF
	O bot não irá mais kickar por sublinhados.
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS canal {ADD|DEL|LIST|CLEAR} [palavra | lista] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	Desculpe, o comando BADWORDS está temporariamente desatvado.
BOT_BADWORDS_REACHED_LIMIT
	Desculpe, você pode ter apenas %d palavrões em sua lista.
BOT_BADWORDS_ALREADY_EXISTS
	%s já existe na lista de palavrões do %s.
BOT_BADWORDS_ADDED
	%s adicionado na lista de palavrões do %s.
BOT_BADWORDS_NO_SUCH_ENTRY
	Entrada não encontrada (#%d) na lista de palavrões do %s.
BOT_BADWORDS_NOT_FOUND
	%s não foi encontrado na lista de palavrões do %s.
BOT_BADWORDS_NO_MATCH
	Nenhum resultado correspondente encontrado na lista do %s.
BOT_BADWORDS_DELETED
	%s removido da lista de palavrões do %s.
BOT_BADWORDS_DELETED_ONE
	Removida 1 entrada da lista de palavrões do %s.
BOT_BADWORDS_DELETED_SEVERAL
	Removidas %d entradas da lista de palavrões do %s.
BOT_BADWORDS_LIST_EMPTY
	Lista de palvrões do %s está vazia.
BOT_BADWORDS_LIST_HEADER
	Lista de palavrões do %s:
	  Núm   Palavra                        Tipo
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	Lista de palavrões está agora vazia.

# SAY responses
BOT_SAY_SYNTAX
	SAY canal texto

# ACT responses
BOT_ACT_SYNTAX
	ACT canal texto

BOT_EXCEPT
	Usuário combina com Except do canal.

BOT_BAD_NICK
	O Nick do bot deve conter apenas caracteres válidos a Nicks.
BOT_BAD_HOST
	O Host do bot deve conter apenas caracteres válidos a Hosts.
BOT_BAD_IDENT
	A Identd do bot deve conter apenas caracteres válidos.
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
	Os Services não conseguiram alterar os modos. Seus servidores estão configurados corretamente?
OPER_BOUNCY_MODES_U_LINE
	Os Services não conseguiram alterar os modos. As U:lines dos seus servidores estão configuradas corretamente?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL mensagem

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Opção STATS desconhecida: %s.
OPER_STATS_CURRENT_USERS
	Usuários atuais: %d (%d ops)
OPER_STATS_MAX_USERS
	Máximo atingido: %d (%s)

# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Services ativos %d dias, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Services ativos %d dia, %02d:%02d
OPER_STATS_UPTIME_HMS
	Services ativos %d horas, %d minutos
OPER_STATS_UPTIME_HM1S
	Services ativos %d horas, %d minutos
OPER_STATS_UPTIME_H1MS
	Services ativos %d horas, %d minuto
OPER_STATS_UPTIME_H1M1S
	Services ativos %d horas, %d minuto
OPER_STATS_UPTIME_1HMS
	Services ativos %d hora, %d minutos
OPER_STATS_UPTIME_1HM1S
	Services ativos %d hora, %d minutos
OPER_STATS_UPTIME_1H1MS
	Services ativos %d hora, %d minuto
OPER_STATS_UPTIME_1H1M1S
	Services ativos %d hora, %d minuto
OPER_STATS_UPTIME_MS
	Services ativos %d minutos, %d segundos
OPER_STATS_UPTIME_M1S
	Services ativos %d minutos, %d segundo
OPER_STATS_UPTIME_1MS
	Services ativos %d minuto, %d segundos
OPER_STATS_UPTIME_1M1S
	Services ativos %d minuto, %d segundo
OPER_STATS_BYTES_READ
	Bytes lidos   : %5d kB
OPER_STATS_BYTES_WRITTEN
	Bytes escritos: %5d kB
OPER_STATS_USER_MEM
	Usuários      : %6d registros, %5d kB
OPER_STATS_CHANNEL_MEM
	Canais        : %6d registros, %5d kB
OPER_STATS_GROUPS_MEM
	Grupos NS     : %6d registros, %5d kB
OPER_STATS_ALIASES_MEM
	Nicks NS      : %6d registros, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ      : %6d registros, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ       : %6d registros, %5d kB
OPER_STATS_HOSTSERV_MEM
	HostServ      : %6d registros, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ      : %6d registros, %5d kB
OPER_STATS_SESSIONS_MEM
	Sessões       : %6d registros, %5d kB
OPER_STATS_AKILL_COUNT
	Número atual de AKILLs: %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Tempo atual de expiração de akill: %d dias
OPER_STATS_AKILL_EXPIRE_DAY
	Tempo atual de expiração de akill: 1 dia
OPER_STATS_AKILL_EXPIRE_HOURS
	Tempo atual de expiração de akill: %d horas
OPER_STATS_AKILL_EXPIRE_HOUR
	Tempo atual de expiração de akill: 1 hora
OPER_STATS_AKILL_EXPIRE_MINS
	Tempo atual de expiração de akill: %d minutos
OPER_STATS_AKILL_EXPIRE_MIN
	Tempo atual de expiração de akill: 1 minuto
OPER_STATS_AKILL_EXPIRE_NONE
	Tempo atual de expiração de akill: Não expira
OPER_STATS_SGLINE_COUNT
	Número atual de SGLINEs: %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Tempo atual de expiração de SGLINE: %d dias
OPER_STATS_SGLINE_EXPIRE_DAY
	Tempo atual de expiração de SGLINE: 1 dia
OPER_STATS_SGLINE_EXPIRE_HOURS
	Tempo atual de expiração de SGLINE: %d horas
OPER_STATS_SGLINE_EXPIRE_HOUR
	Tempo atual de expiração de SGLINE: 1 hora
OPER_STATS_SGLINE_EXPIRE_MINS
	Tempo atual de expiração de SGLINE: %d minutos
OPER_STATS_SGLINE_EXPIRE_MIN
	Tempo atual de expiração de SGLINE: 1 minuto
OPER_STATS_SGLINE_EXPIRE_NONE
	Tempo atual de expiração de SGLINE: Não expira
OPER_STATS_SQLINE_COUNT
	Número atual de SQLINEs: %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Tempo atual de expiração de SQLINE: %d dias
OPER_STATS_SQLINE_EXPIRE_DAY
	Tempo atual de expiração de SQLINE: 1 dia
OPER_STATS_SQLINE_EXPIRE_HOURS
	Tempo atual de expiração de SQLINE: %d horas
OPER_STATS_SQLINE_EXPIRE_HOUR
	Tempo atual de expiração de SQLINE: 1 hora
OPER_STATS_SQLINE_EXPIRE_MINS
	Tempo atual de expiração de SQLINE: %d minutos
OPER_STATS_SQLINE_EXPIRE_MIN
	Tempo atual de expiração de SQLINE: 1 minuto
OPER_STATS_SQLINE_EXPIRE_NONE
	Tempo atual de expiração de SQLINE: Não expira
OPER_STATS_SZLINE_COUNT
	Número atual de SZLINEs: %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Tempo atual de expiração de SZLINE: %d dias
OPER_STATS_SZLINE_EXPIRE_DAY
	Tempo atual de expiração de SZLINE: 1 dia
OPER_STATS_SZLINE_EXPIRE_HOURS
	Tempo atual de expiração de SZLINE: %d horas
OPER_STATS_SZLINE_EXPIRE_HOUR
	Tempo atual de expiração de SZLINE: 1 hora
OPER_STATS_SZLINE_EXPIRE_MINS
	Tempo atual de expiração de SZLINE: %d minutos
OPER_STATS_SZLINE_EXPIRE_MIN
	Tempo atual de expiração de SZLINE: 1 minuto
OPER_STATS_SZLINE_EXPIRE_NONE
	Tempo atual de expiração de SZLINE: Não expira
OPER_STATS_RESET
	Resetar estatísticas.

# MODE responses
OPER_STATS_UPLINK_SERVER
	Uplink server: %s
OPER_STATS_UPLINK_CAPAB
	Uplink capab: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Servers found: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE canal modos

# UMODE respondes
OPER_UMODE_SYNTAX
	UMODE nick modos
OPER_UMODE_SUCCESS
	Modos do usuário %s alterados.
OPER_UMODE_CHANGED
	%s alterou seus modos de usuário.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE nick flags
OPER_OLINE_SUCCESS
	As Operflags %s foram adicionadas para %s.
OPER_OLINE_IRCOP
	Você agora é um Operador de IRC.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES canal [ALL]
OPER_CLEARMODES_DONE
	Modos binários e bans removidos do canal %s.
OPER_CLEARMODES_ALL_DONE
	Todos os modos foram removidos do canal %s.


# KICK responses
OPER_KICK_SYNTAX
	KICK canal usuário motivo
	
# ADMIN responses
OPER_SVSNICK_SYNTAX
	SVSNICK nick novonick 
OPER_SVSNICK_NEWNICK
	O nick %s está sendo alterado para %s.
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [nick | lista]
OPER_ADMIN_SKELETON
	Services estão no modo esqueleto; o comando ADMIN não está disponível.
OPER_ADMIN_EXISTS
	%s já está na lista de Administradores dos Services.
OPER_ADMIN_REACHED_LIMIT
	Desculpe, você pode ter apenas %d Administradores dos Services.
OPER_ADMIN_ADDED
	%s adicionado à lista de Administradores dos Services.
OPER_ADMIN_NOT_FOUND
	%s não encontrado na lista de Administradores dos Services.
OPER_ADMIN_NO_MATCH
	Nenhum resultado encontrado na lista de Administradores dos Services.
OPER_ADMIN_DELETED
	%s removido da lista de Administradores dos Services.
OPER_ADMIN_DELETED_ONE
	Removida 1 entrada da lista de Administradores dos Services.
OPER_ADMIN_DELETED_SEVERAL
	Removidas %d entradas da lista de Administradores dos Services.
OPER_ADMIN_LIST_EMPTY
	A lista de Administradores dos Services está vazia.
OPER_ADMIN_LIST_HEADER
	Lista de Administradores dos Services:
	  Núm   Nick
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	Lista de Administradores dos Services foi apagada.
OPER_ADMIN_MOVED
	%s foi movido para a lista de Administradores dos Services.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | lista]
OPER_OPER_SKELETON
	Services estão no modo esqueleto; o comando OPER não está disponível.
OPER_OPER_EXISTS
	%s já está na lista de Operadores dos Services.
OPER_OPER_REACHED_LIMIT
	Desculpe, você pode ter apenas %d Operadores dos Services.
OPER_OPER_ADDED
	%s adicionado à lista de Operadores dos Services.
OPER_OPER_NOT_FOUND
	%s não encontrado na lista de Operadores dos Services.
OPER_OPER_NO_MATCH
	Nenhum resultado equivalente na lista de Operadores dos Services.
OPER_OPER_DELETED
	%s removido da lista de Operadores dos Services.
OPER_OPER_DELETED_ONE
	Removida 1 entrada da lista de Operadores dos Services.
OPER_OPER_DELETED_SEVERAL
	Removidas %d entradas da lista de Operadores dos Services.
OPER_OPER_LIST_EMPTY
	A lista de Operadores dos Services está vazia.
OPER_OPER_LIST_HEADER
	Lista de Operadores dos Services:
	  Núm   Nick
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	A lista de Operadores dos Services foi apagada.
OPER_OPER_MOVED
	%s foi movido para a lista de Operadores dos Services.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+tempo] {máscara | lista} [motivo]]
OPER_AKILL_EXISTS
	%s já se encontra na lista de AKILL.
OPER_AKILL_ALREADY_COVERED
	%s já está coberto pelo AKILL %s.
OPER_AKILL_REACHED_LIMIT
	Desculpe, você pode ter apenas %d AKILLs.
OPER_AKILL_NO_NICK
	Lembrete: As máscaras de AKILL não devem conter o nick; tenha certeza de que você não incluiu o nick na máscara.
OPER_AKILL_ADDED
	%s adicionado à lista de AKILL.
OPER_AKILL_CHANGED
	Tempo de expiração para %s alterado.
OPER_AKILL_NOT_FOUND
	%s não encontrado na lista de AKILL.
OPER_AKILL_NO_MATCH
	Nenhum resultado equivalente na lista de AKILL.
OPER_AKILL_DELETED
	%s removido da lista de AKILL.
OPER_AKILL_DELETED_ONE
	Removida 1 entrada da lista de AKILL.
OPER_AKILL_DELETED_SEVERAL
	Removidas %d entradas da lista de AKILL.
OPER_AKILL_LIST_EMPTY
	A lista de AKILL está vazia.
OPER_AKILL_LIST_HEADER
	Lista de AKILL atual:
	  Núm   Máscara                           Motivo
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Lista de AKILL atual:
# number, mask, set-by, set-time, expires, reason
OPER_AKILL_VIEW_FORMAT
	%3d  %s (por %s em %s; %s)
	      %s
OPER_AKILL_CLEAR
	Lista de AKILL apagada.
OPER_CHANKILL_SYNTAX
	CHANKILL [+tempo] {#canal} [motivo]

# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+tempo] {máscara | lista}[:motivo]]
OPER_SGLINE_UNSUPPORTED
	Desculpe, SGLINE não está disponível nesta Rede.
OPER_SGLINE_EXISTS
	%s já existe na lista de SGLINE.
OPER_SGLINE_ALREADY_COVERED
	%s já está coberto por %s.
OPER_SGLINE_REACHED_LIMIT
	Desculpe, você pode ter apenas %d SGLINEs.
OPER_SGLINE_ADDED
	%s adicionado à lista de SGLINE.
OPER_SGLINE_CHANGED
	Tempo de expiração para %s aterado.
OPER_SGLINE_NOT_FOUND
	%s não encontrado na lista de SGLINE.
OPER_SGLINE_NO_MATCH
	Nenhum resultado equivalente na lista de SGLINE.
OPER_SGLINE_DELETED
	%s removido da lista de SGLINE.
OPER_SGLINE_DELETED_ONE
	Removida 1 entrada da lista de SGLINE.
OPER_SGLINE_DELETED_SEVERAL
	Removidas %d entradas da lista de SGLINE.
OPER_SGLINE_LIST_EMPTY
	A lista de SGLINE está vazia.
OPER_SGLINE_LIST_HEADER
	Lista de SGLINE atual:
	  Núm   Máscara                           Motivo
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Lista de SGLINE atual:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (por %s em %s; %s)
	      %s
OPER_SGLINE_CLEAR
	A lista de SGLINE foi apagada.

# SZLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+tempo] {máscara | lista} [motivo]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	SQLINEs de canal não são suportadas por este IRCd, então você não pode usá-las.
OPER_SQLINE_EXISTS
	%s já existe na lista de SQLINE.
OPER_SQLINE_ALREADY_COVERED
	%s já está coberto por %s.
OPER_SQLINE_REACHED_LIMIT
	Desculpe, você pode ter apenas %d SQLINEs.
OPER_SQLINE_ADDED
	%s adicionado à lista de SQLINE.
OPER_SQLINE_CHANGED
	Tempo de expiração para %s alterado.
OPER_SQLINE_NOT_FOUND
	%s não encontrado na lista de SQLINE.
OPER_SQLINE_NO_MATCH
	Nenhum resultado equivalente na lista de SQLINE.
OPER_SQLINE_DELETED
	%s removido da lista de SQLINE.
OPER_SQLINE_DELETED_ONE
	Removida 1 entrada da lista de SQLINE.
OPER_SQLINE_DELETED_SEVERAL
	Removidas %d entradas da lista de SQLINE.
OPER_SQLINE_LIST_EMPTY
	A lista de SQLINE está vazia.
OPER_SQLINE_LIST_HEADER
	Lista de SQLINE atual:
	  Núm   Máscara                           Motivo
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Lista de SQLINE atual:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (por %s em %s; %s)
	      %s
OPER_SQLINE_CLEAR
	A lista de SQLINE foi apagada.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+tempo] {máscara | lista} [motivo]]
OPER_SZLINE_UNSUPPORTED
	Desculpe, SZLINE não está disponível nesta Rede.
OPER_SZLINE_EXISTS
	%s já existe na lista de SZLINE.
OPER_SZLINE_ALREADY_COVERED
	%s já está coberto por %s.
OPER_SZLINE_REACHED_LIMIT
	Desculpe, você pode ter apenas %d SZLINEs.
OPER_SZLINE_ONLY_IPS
	Lembrete: você pode apenas adicionar endereços de IP para a lista de SZLINE.
OPER_SZLINE_ADDED
	%s adicionado à lista de SZLINE.
OPER_SZLINE_CHANGED
	Tempo de expiração para %s alterado.
OPER_SZLINE_NOT_FOUND
	%s não foi encontrado na lista de SZLINE.
OPER_SZLINE_NO_MATCH
	Nenhum resultado equivalente na lista de SZLINE.
OPER_SZLINE_DELETED
	%s removido da lista de SZLINE.
OPER_SZLINE_DELETED_ONE
	Removida 1 entrada da lista de SZLINE.
OPER_SZLINE_DELETED_SEVERAL
	Removidas %d entradas da lista de SZLINE.
OPER_SZLINE_LIST_EMPTY
	A lista de SZLINE está vazia.
OPER_SZLINE_LIST_HEADER
	Lista atual de SZLINE:
	  Núm   Máscara                           Motivo
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Lista atual de SZLINE:
# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (por %s em %s; %s)
	      %s
OPER_SZLINE_CLEAR
	A lista de SZLINE foi apagada.

# SET responses
OPER_SET_SYNTAX
	SET opção configuração
OPER_SET_IGNORE_ON
	Opção IGNORE será usada.
OPER_SET_IGNORE_OFF
	Opção IGNORE não será usada.
OPER_SET_IGNORE_ERROR
	Configuração do IGNORE deve ser ON ou OFF.
OPER_SET_READONLY_ON
	Services estão agora no modo somente leitura.
OPER_SET_READONLY_OFF
	Services estão agora no modo leitura e gravação.
OPER_SET_READONLY_ERROR
	Configuração do READONLY deve ser ON ou OFF.
OPER_SET_LOGCHAN_ON
	Services estão enviando o log no canal %s.
OPER_SET_LOGCHAN_OFF
	Services deixando de enviar o log.
OPER_SET_LOGCHAN_ERROR
	Configuração do LOGCHAN deve ser ON ou OFF e LogChan deve estar definido.
OPER_SET_DEBUG_ON
	Services estão agora no modo debug.
OPER_SET_DEBUG_OFF
	Services não estão mais no modo debug.
OPER_SET_DEBUG_LEVEL
	Services estão agora no modo debug (nível %d).
OPER_SET_DEBUG_ERROR
	Configuração do DEBUG deve ser ON, OFF, ou um número positivo.
OPER_SET_NOEXPIRE_ON
	Services estão agora no modo não expira.
OPER_SET_NOEXPIRE_OFF
	Services estão agora no modo expira.
OPER_SET_NOEXPIRE_ERROR
	Opção NOEXPIRE deve ser ON ou OFF.
OPER_SET_UNKNOWN_OPTION
	Opção desconhecida: %s.
OPER_SET_SQL_ON
	Código SQL será usado.
OPER_SET_SQL_OFF
	Código SQL não será usado.
OPER_SET_SQL_ERROR
	Ajuste para SQL deve ser ON ou OFF.
OPER_SET_SQL_ERROR_INIT
	Erro durante inicialização de SQL, verifique seus logs para corrigir.
OPER_SET_SQL_ERROR_DISABLED
	SQL não está configurada para esta rede. Por favor, edite o
	arquivo de configuração (services.conf).
OPER_SET_LIST_OPTION_ON
	%s está ativada
OPER_SET_LIST_OPTION_OFF
	%s está desativada

# NOOP responses
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} servidor
OPER_NOOP_SET
	Todas as O:lines do servidor %s foram removidas.
OPER_NOOP_REVOKE
	Todas as O:lines do servidor %s foram resetadas.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE servidor [motivo]
OPER_JUPE_HOST_ERROR
	Por favor, use um nome de servidor válido quando estiver "jupando".
OPER_JUPE_INVALID_SERVER
	You can not jupe your services server or your uplink server.
	
# RAW responses
OPER_RAW_SYNTAX
	RAW texto

# UPDATE responses
OPER_UPDATING
	Atualizando banco de dados.
	
# RELOAD responses
OPER_RELOAD
	Arquivos de configuração dos Services foram recarregados.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN não definido; não foi possível reiniciar. Rode novamente o \2configure\2 e recompile os services para habilitar o comando RESTART.

# LISTIGNORE responses
OPER_IGNORE_SYNTAX
	IGNORE {ADD|DEL|LIST|CLEAR} [tempo] [nick | mask]
OPER_IGNORE_VALID_TIME
	Você precisa digitar um número válido como tempo.
OPER_IGNORE_TIME_DONE
	%s agora será ignorado por %s.
OPER_IGNORE_PERM_DONE
	%s agora será permanentemente ignorado.
OPER_IGNORE_DEL_DONE
	%s não será mais ignorado.
OPER_IGNORE_LIST
	Lista de Ignore dos Services:
OPER_IGNORE_LIST_NOMATCH
	%s não encontrado na lista de Ignore.
OPER_IGNORE_LIST_EMPTY
	Lista de Ignore dos Services vazia.

# KILLCLONES responses
OPER_IGNORE_LIST_CLEARED
	Lista de Ignore foi apagada.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	Não foi possível encontrar o nick %s.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Lista de canais:
	Nome              Usuários Tópico
OPER_CHANLIST_HEADER_USER
	Lista de canais do %s:
	Nome              Usuários Tópico
OPER_CHANLIST_RECORD
	%-20s  %4d %s
OPER_CHANLIST_END
	Fim da listagem.
	
# USERLIST responses
OPER_USERLIST_HEADER
	Lista de usuários:
	Nick                 Máscara
OPER_USERLIST_HEADER_CHAN
	Lista de usuários do %s:
	Nick                 Máscara
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Fim da listagem.

OPER_SUPER_ADMIN_ON
	Você é agora um Super-Admin
OPER_SUPER_ADMIN_OFF
	Você não é mais um Super-Admin
OPER_SUPER_ADMIN_SYNTAX
	Configuração de SuperAdmin deve ser ON ou OFF (deve estar habilitada no services.conf).
OPER_SUPER_ADMIN_WALL_ON
	%s é agora um Super-Admin
OPER_SUPER_ADMIN_WALL_OFF
	%s não é mais um Super-Admin
OPER_SUPER_ADMIN_ONLY
	Apenas Super-Admins podem usar este comando.
OPER_STAFF_LIST_HEADER
	On Nível Nick
OPER_STAFF_FORMAT
	 %c %s  %s
OPER_STAFF_AFORMAT
	 %c %s  %s [%s]

# DefCon Messages

OPER_DEFCON_SYNTAX
	DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	Este serviço está temporariamente desabilitado, por favor tente novamente mais tarde
OPER_DEFCON_NO_CONF
	O sistema Defcon deve estar habilitado no arquivo services.conf
OPER_DEFCON_CHANGED
	Services estão agora em DEFCON %d
OPER_DEFCON_WALL
	%s alterou o Nível do DEFCON para %d
DEFCON_GLOBAL
	O Nível do Defcon está agora no Nível: %d

# Modules strings

OPER_MODULE_LOADED
	Módulo %s carregado
OPER_MODULE_UNLOADED
	Módulo %s descarregado
OPER_MODULE_LOAD_FAIL
	Impossível carregar o módulo %s
OPER_MODULE_REMOVE_FAIL
	Impossível remover o módulo %s
OPER_MODULE_NO_UNLOAD
	This module can not be unloaded.
OPER_MODULE_LOAD_SYNTAX
	MODLOAD NomeDoArquivo
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD NomeDoArquivo
OPER_MODULE_LIST_HEADER
	Lista de Módulos atual:
OPER_MODULE_LIST
	Módulo: %s [%s] [%s]
OPER_MODULE_LIST_FOOTER
	%d módulos carregados.
OPER_MODULE_INFO_LIST
	Módulo: %s Versão: %s Autor: %s Carregado: %s
OPER_MODULE_CMD_LIST
	Comando fornecido: /%R%s %s
OPER_MODULE_MSG_LIST
	Fornecendo IRCd handler para: %s
OPER_MODULE_NO_LIST
	Nenhum módulo carregado atualmente.
OPER_MODULE_NO_INFO
	Nenhuma informação sobre o módulo %s está disponível.
OPER_MODULE_INFO_SYNTAX
	MODINFO NomeDoArquivo

MODULE_HELP_HEADER
	Os seguintes comandos foram carregados por um módulo:

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [parâmetros]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+tempo] máscara limite motivo
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {máscara | lista}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE núm posição
OPER_EXCEPTION_DISABLED
	Opção de limite de sessões desabilitada.
OPER_EXCEPTION_ALREADY_PRESENT
	Máscara %s já presente na lista de limite de sessões.
OPER_EXCEPTION_TOO_MANY
	Lista de limite de sessões está cheia!
OPER_EXCEPTION_ADDED
	Limite de sessões para %s é agora de %d.
OPER_EXCEPTION_MOVED
	Sessão para %s (#%d) alterada para a posição %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Nenhuma entrada equivalente (#%d) na lista de limite de sessões.
OPER_EXCEPTION_NOT_FOUND
	%s não encontrado na lista de limite de sessões.
OPER_EXCEPTION_NO_MATCH
	Nenhum resultado encontrado na lista de limite de sessões.
OPER_EXCEPTION_DELETED
	%s removido da lista de limite de sessões.
OPER_EXCEPTION_DELETED_ONE
	Removida 1 entrada da lista de limite de sessões.
OPER_EXCEPTION_DELETED_SEVERAL
	Removidas %d entradas da lista de limite de sessões.
OPER_EXCEPTION_LIST_HEADER
	Atual lista de Limite de sessões:
# mask, limit
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Núm  Limite  Host
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (por %s em %s; %s)
	    Limite: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Limite de sessão inválida. O limite deve ser um número inteiro maior ou igual a zero e menor que %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Máscara inválida. Apenas máscaras (hosts) são válidas como exceções aos limites de sessões. Nick ou username não são válidos.
OPER_EXCEPTION_EXISTS
	%s já existe na lista de EXCEPTION.
OPER_EXCEPTION_CHANGED
	Exceção para %s foi atualizada para %d.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST limite | VIEW host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST limite
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW host
OPER_SESSION_DISABLED
	Limite de sessões está desativado.
OPER_SESSION_INVALID_THRESHOLD
	Número inválido. Deve ser um número inteiro maior que 1.
OPER_SESSION_NOT_FOUND
	%s não encontrado na lista de Limite de Sessões.
OPER_SESSION_LIST_HEADER
	Hosts com pelo menos %d sessões:
OPER_SESSION_LIST_COLHEAD
	Sessões  Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	O host %s possui atualmente %d sessões com um limite de %d.


# EXCEPTION help

OPER_HELP_EXCEPTION
	Sintaxe: EXCEPTION ADD [+tempo] máscara limite motivo
	         EXCEPTION DEL {máscara | lista}
	         EXCEPTION MOVE núm posição
	         EXCEPTION LIST [máscara | lista]
	         EXCEPTION VIEW [máscara | lista]
	
	Permite que os Administradores dos Services manipulem a lista
	de hosts que tenham limite de conexões especificados - deixando 
	certos hosts, como shell de servidores, conectar mais que o
	número padrão de conexões simultâneas. Uma vez um host atingindo
	um limite de conexão, todos os clientes que tentem conectar 
	daquele host serão expulsos ("killados").

	Antes do usuário ser expulso ("killado"), ele será notificado
	via /NOTICE do %S com um tópico de ajuda sobre limitação
	de conexões.
	
	EXCEPTION ADD adiciona a máscara fornecida à lista de exceções.
	Note que as máscaras nick!user@host e user@host são
	inválidas. Só nomes reais de hosts, como mail.dominio.com e 
	*.dominio.com, são permitidos porque a limitação de
	conexão não utiliza o nick ou ident para bloquear o acesso. 
	
	EXCEPTION DEL remove a máscara especificada da Lista de
	exceções.
	EXCEPTION MOVE move uma exceção de número núm para uma
	posição dada. As exceções que estavam na faixa onde houve
	alteração serão todas movidas para cima ou para baixo conforme
	for o deslocamento necessário.
	EXCEPTION LIST ou EXCEPTION VIEW mostram todas as 
	exceções atuais válidas; se uma máscara é dada como parâmetro
	opcional, a lista é limitada as exceções que são compatíveis
	com a máscara dada. A diferenca é que EXCEPTION VIEW 
	é mais detalhado, mostrando o nome da pessoa que adicionou 
	a exceção, o seu limite de conexões, o motivo, a máscara do
	host, a data e o tempo de validade.
	
	Note que um cliente que conecte vai usar a primeira exceção
	que for compatível com o seu host. Listas de exceções muito
	grandes e máscaras de hosts muito abrangentes certamente
	irão degradar a eficácia de funcionamento ("performance")
	dos services.
	
	Limitado aos Administradores dos Services.

OPER_HELP_SESSION
	Sintaxe: SESSION LIST número
	         SESSION VIEW host
	
	Permite que os Administradores dos Services vejam a lista de
	conexões.
	
	SESSION LIST lista máscaras de hosts com pelo menos 
	número conexões permitidas.
	O limite deve ser um número maior que 1. Isto deve prevenir 
	a listagem de muitos hosts com um limite de conexões 
	limitado a 1.
	SESSION VIEW mostra informações detalhadas sobre um host
	específico, incluindo o número de conexões atual e o limite
	A máscara dada em host não deve incluir wildcards.
	
	Veja a ajuda sobre EXCEPTION para mais informações sobre
	o limite de conexões e como especificar limites de conexões
	para certos hosts e grupos de hosts.
	
	Limitado aos Administradores dos Services.

OPER_HELP_STAFF
	Sintaxe: STAFF

	Exibe todos os nicks da Staff dos Services juntamente com seus
	níveis e status on-line.

OPER_HELP_DEFCON
	Sintaxe: DEFCON [1|2|3|4|5]

	O sistema Defcon pode ser usado para implementar um conjunto
	pré-definido de restrições aos Services, útil durante uma
	tentativa de ataque à Rede.

OPER_HELP_DEFCON_NO_NEW_CHANNELS
	* Impedido o registro de novos canais
OPER_HELP_DEFCON_NO_NEW_NICKS
	* Impedido o registro de novos nicks
OPER_HELP_DEFCON_NO_MLOCK_CHANGE
	* Impedida a mudança de MLOCK
OPER_HELP_DEFCON_FORCE_CHAN_MODES
	* Força os Modos de Canal (%s) a serem setados em todos os canais
OPER_HELP_DEFCON_REDUCE_SESSION
	* Usa o limite de sessões reduzido de %d
OPER_HELP_DEFCON_NO_NEW_CLIENTS
	* Killa quaisquer NOVOS clientes conectando
OPER_HELP_DEFCON_OPER_ONLY
	* Ignora quaisquer não-opers com mensagem
OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* Silenciosamente ignora não-opers
OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* AKILLA quaisquer novos clientes conectando
OPER_HELP_DEFCON_NO_NEW_MEMOS
	* Impede o envio de novos memos
OPER_HELP_CHANKILL
	Sintaxe: CHANKILL [+tempo] canal motivo

	Adiciona um AKILL para cada nick no canal especificado. Isso
	usa por completo todo o host real ident@host para cada nick;
	em seguida, força o AKILL.

###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Logon News - %s] %s
NEWS_OPER_TEXT
	[Oper News - %s] %s

NEWS_RANDOM_TEXT
	[Random News - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD|DEL|LIST} [texto|núm]
NEWS_LOGON_LIST_HEADER
	Logon News itens:
NEWS_LOGON_LIST_ENTRY
	%5d (%s por %s)
	    %s
NEWS_LOGON_LIST_NONE
	Não existem logon news.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD texto
NEWS_LOGON_ADD_FULL
	Lista de logon news está cheia!
NEWS_LOGON_ADDED
	Adicionado novo logon news (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {núm | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Logon news não encontrado #%d!
NEWS_LOGON_DELETED
	Logon news removido #%d.
NEWS_LOGON_DEL_NONE
	Nenhum logon news para remover!
NEWS_LOGON_DELETED_ALL
	Todos os logon news foram removidos.

NEWS_OPER_SYNTAX
	OPERNEWS {ADD|DEL|LIST} [texto|núm]
NEWS_OPER_LIST_HEADER
	Oper News itens:
NEWS_OPER_LIST_ENTRY
	%5d (%s por %s)
	    %s
NEWS_OPER_LIST_NONE
	Não existe nenhum oper news.
NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD texto
NEWS_OPER_ADD_FULL
	Lista de oper news está cheia!
NEWS_OPER_ADDED
	Adicionado novo OperNews (#%d).
NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {núm | ALL}
NEWS_OPER_DEL_NOT_FOUND
	OperNews não encontrado #%d!
NEWS_OPER_DELETED
	OperNews removido #%d.
NEWS_OPER_DEL_NONE
	Nenhum OperNews para remover!
NEWS_OPER_DELETED_ALL
	Todos os OperNews foram removidos.

NEWS_RANDOM_SYNTAX
	RANDOMNEWS {ADD|DEL|LIST} [texto|núm]
NEWS_RANDOM_LIST_HEADER
	Random News itens:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s por %s)
	    %s
NEWS_RANDOM_LIST_NONE
	Não há RandomNews.
NEWS_RANDOM_ADD_SYNTAX
	RANDOMNEWS ADD texto
NEWS_RANDOM_ADD_FULL
	A lista de News está cheia!
NEWS_RANDOM_ADDED
	Adicionada novo item RandomNews (#%d).
NEWS_RANDOM_DEL_SYNTAX
	RANDOMNEWS DEL {núm | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	RandomNews item #%d não encontrado!
NEWS_RANDOM_DELETED
	RandomNews item #%d deletado.
NEWS_RANDOM_DEL_NONE
	Nenhum item RandomNews para deletar!
NEWS_RANDOM_DELETED_ALL
	Todos os itens RandomNews deletados.

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
	Sintaxe: RANDOMNEWS ADD texto
	         RANDOMNEWS DEL {núm | ALL}
	         RANDOMNEWS LIST
	
	Edita ou mostra a lista de random news. Quando um usuário
	se conecta à rede, uma (apenas uma) das random news será
	aleatóriamente escolhida e enviada a ele.
	
	RANDOMNEWS may only be used by Services admins. 

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S é um services criado para dar informações sobre
	os Services. Os tópicos de ajuda são enviados pelo 
	comando HELP dos outros clientes dos Services:

	%R%s HELP
	     para informações sobre como registrar nicks

	%R%s HELP
	     para informações sobre como registrar e controlar canais

	%R%s HELP
	     para informações sobre como enviar mensagens para
	     usuários que não estejam conectados no momento

HELP_HELP_BOT

	%R%s HELP
	     para informações sobre como colocar um bot em seu canal

HELP_HELP_HOST

	%R%s HELP
	     para informação sobre como ajustar vHosts em nicks

###########################################################################
#
# NickServ help messages
#
###########################################################################
NICK_HELP_CMD_CONFIRM
	    CONFIRM    Confirma um código de autorização do NickServ
NICK_HELP_CMD_RESEND
	    RESEND     Reenvia um código de autorização do NickServ
NICK_HELP_CMD_REGISTER
	    REGISTER   Registra um nick
NICK_HELP_CMD_GROUP
	    GROUP      Se junta a um grupo
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY   Identifica seu nick com sua senha
NICK_HELP_CMD_ACCESS
	    ACCESS     Modifica a lista de endereços autorizados
NICK_HELP_CMD_SET
	    SET        Ajusta opções, incluindo proteção de kill
NICK_HELP_CMD_SASET
	    SASET      Set SET-options on another nickname
NICK_HELP_CMD_DROP
	    DROP       Cancela o registro de um nick
NICK_HELP_CMD_RECOVER
	    RECOVER    Derruba usuário que esteja usando seu nick
NICK_HELP_CMD_RELEASE
	    RELEASE    Retoma a custódia do seu nick após um RECOVER
NICK_HELP_CMD_SENDPASS
	    SENDPASS   Esqueceu sua senha? Tente isto
NICK_HELP_CMD_GHOST
	    GHOST      Termina uma sessão "fantasma" de IRC que esteja usando seu nick
NICK_HELP_CMD_ALIST
	    ALIST      Lista todos os canais nos quais você tem acesso
NICK_HELP_CMD_GLIST
	    GLIST      Lista todos os nicks do seu grupo
NICK_HELP_CMD_INFO
	    INFO       Mostra informações sobre algum nick registrado
NICK_HELP_CMD_LIST
	    LIST       Lista todos os nicks registrados que combinam com o padrão fornecido
NICK_HELP_CMD_LOGOUT
	    LOGOUT     Reverte o efeito do comando IDENTIFY
NICK_HELP_CMD_STATUS
	    STATUS     Retorna o status do owner de um determinado nick
NICK_HELP_CMD_UPDATE
	    UPDATE     Atualiza seu status atual, ou seja, verifica novos memos
NICK_HELP_CMD_GETPASS
	    GETPASS    Recupera a senha de um determinado nick
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Encontra e retorna todos os usuários que se
	                   registraram usando o email fornecido
NICK_HELP_CMD_FORBID
	    FORBID     Previne um nick de ser usado ou registrado
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Suspende o nick fornecido
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Libera o nick fornecido

NICK_HELP
	%S permite que você "registre" um nick e evita que outras
	pessoas o utilizem. Os seguintes comandos permitem o
	registro e a manutenção dos nicks; para utilizá-los, digite
	%R%S comando. Para mais informações sobre um
	comando específico, digite %R%S HELP comando.
	
NICK_HELP_FOOTER
	NOTA: Este serviço tem a intenção de prover um modo para
	que usuários do IRC não tenham sua identidade comprometida.
	Isto NÃO é voltado para facilitar "roubo" dos nicks ou
	outras atitudes mau intencionadas. O abuso do %S
	resultará em, no mínimo, perda do(s) nick(s) abusivo(s). 
	
NICK_HELP_EXPIRES
	
	Nicks que não estejam mais sendo usados estão sujeitos 
	a expirarem automaticamente, ou seja, eles serão apagados
	após %d dias sem serem usados.

NICK_HELP_REGISTER
	Sintaxe: REGISTER senha [email]
	
	Registra seu nick no banco de dados do %S. Uma vez
	que seu nick está registrado, você pode usar os comandos
	SET e ACCESS para configurar as opções do seu nick como
	você preferir depois. Certifique-se de lembrar sua senha
	durante o registro, você precisará dela para fazer alterações
	no seu nick futuramente.
	(Lembre-se que maiúsculas diferem, como:  ANOPE, Anope e 
	anope são senhas diferentes!)
	
	Sugestões ao escolher sua senha:
	
	As senhas não devem ser de fácil adivinhação. Por exemplo,
	usar seu nome como senha é uma péssima idéia. Utilizar seu
	nick como senha é uma idéia muito pior e, de fato, o %S
	não permitirá isso. Da mesma maneira, senhas curtas são
	vulneráveis a testes de erro-e-acerto, então você deve
	escolher uma senha com pelo menos 5 caracteres. Finalmente,
	o caractere "ESPAÇO" não pode ser usado em senhas.
	
	O parâmetro email é opcional e servirá para setar um
	email para seu nick. Entretanto, o email pode ser obrigatório
	em algumas Redes de IRC.
	Sua privacidade é respeitada; este e-mail nunca será dado
	para terceiros.
	
	Este comando servirá também para criar um GRUPO para o seu
	nick, e irá permitir que você registre outros nicks e divida
	as mesmas configurações, os mesmos memos e os mesmos 
	privilégios em canais. Para maiores informações neste recurso,
	digite %R%S HELP GROUP.
	
NICK_HELP_GROUP
	Sintaxe: GROUP alvo senha
	
	Este comando faz seu nick entrar no grupo de um nick alvo.
	A senha é a senha do nick alvo.
	
	Entrando em um grupo permitirá a você compartilhar suas
	configurações, memos e privilégios em canais com todos os
	nicks do grupo.
	
	Um grupo existe por quanto tempo for necessário. Isso
	significa que mesmo se um nick do grupo for desregistrado,
	você não perderá os recursos compartilhados citados acima,
	durante o tempo em que houver pelo menos um nick restando
	no grupo.
	
	Você pode usar este comando mesmo se não ainda não tiver
	registrado seu nick. Se seu nick já estiver registrado,
	você precisará identificá-lo antes de usar este comando.
	Digite %R%S HELP IDENTIFY para mais informação; talvez
	isso não seja possível em algumas redes.
	
	É recomendável usar este comando com nicks não-registrados,
	pois eles serão automaticamente registrados ao usar o comando.
	Você pode usar isto com um nick registrado (para alterar seu
	grupo) apenas se for permitido na rede.
	
	Você só pode estar em um grupo por vez. Mesclar grupos não é
	possível.
	
	Atenção: todos os nicks do grupo terão a mesma senha.

NICK_HELP_IDENTIFY
	Sintaxe: IDENTIFY senha
	
	Identifica você ao %S como sendo dono do nick.
	Muitos comandos exigem que você se identifique com este
	comando antes de você usá-los. A senha deve ser a mesma que
	você enviou com o comando REGISTER.
	
NICK_HELP_UPDATE
	Sintaxe: UPDATE

	Atualiza seu status atual, ou seja, checa por novos memos,
	seta modos de canal necessários (ModeonID) e atualiza seu
	vHost e suas flags de usuário (Último horário visto etc).
	
NICK_HELP_LOGOUT
	Sintaxe: LOGOUT
	
	Este comando reverte o efeito do comando IDENTIFY, ou seja,
	faz com que você não seja mais reconhecido como o verdadeiro
	dono do nick. Note, entretanto, que ele não pedirá que você se
	identifique novamente.

NICK_HELP_DROP
	Sintaxe: DROP [nick]
	
	Sem parâmetros, cancela o registro de seu nick no banco
	de dados do %S. Um nick que foi desregistrado é livre para
	qualquer um que queira registrá-lo.

	Você pode também desregistrar um nick de seu grupo
	especificando-o com o parâmetro nick.
	
	Para você usar este comando, será necessário que primeiro
	você se identifique usando sua senha (%R%S HELP IDENTIFY 
	para maiores informações).

NICK_HELP_ACCESS
	Sintaxe: ACCESS ADD máscara
	         ACCESS DEL máscara
	         ACCESS LIST
	
	Modifica ou mostra a lista de acesso do seu nick. Esta é
	a lista de endereços a qual será automaticamente reconhecida
	pelo %S como as permitidas a usarem seu nick. Se você
	quer usar um nick de um endereço diferente, você deve usar o
	comando IDENTIFY para identificar-se ao %S
	
	Exemplos:
	
	    ACCESS ADD nick@*.provedor.com.br
	        Permite o acesso do usuário nick de qualquer
	        máquina com o domínio provedor.com.br.
	
	    ACCESS DEL nick@*.provedor.com.br
	        Reverte o comando anterior
	
	    ACCESS LIST
	        Mostra a lista de acesso atual.

NICK_HELP_SET
	Sintaxe: SET opção parâmetros
	
	Ajusta várias opções de nick. A opção pode ser:
	
	    DISPLAY    Mostra o seu grupo nos Services
	    PASSWORD   Ajusta a senha do seu nick
	    LANGUAGE   Ajusta a linguagem dos Services quando
	                   mensagens são enviadas à você
	    URL        Associa um endereço URL com seu nick
	    EMAIL      Associa um endereço de e-mail ao seu nick
	    ICQ        Associa um número de ICQ ao seu nick
	    GREET      Associa uma mensgem de entrada ao seu nick
	    KILL       Ativa/Desativa a proteção de kill para o seu nick
	    SECURE     Ativa/Desativa os recursos de segurança para o seu nick
	    PRIVATE    Previne seu nick de aparecer em um %R%S LIST
	    HIDE       Esconde certas informações sobre seu nick
	    MSG        Altera o método de comunicação dos Services
	    AUTOOP     Should services op you automatically.    
	
	Para usar este comando, você deve primeiro se identificar
	com sua senha (%R%S HELP IDENTIFY para maiores
	informações).
	
	Digite %R%S HELP SET opção para maiores informações
	sobre um comando específico.

NICK_HELP_SET_DISPLAY
	Sintaxe: SET DISPLAY novo-display
	
	Altera o display usado como referência ao nome do seu grupo 
	nos Services. O novo display DEVE ser um nick do seu grupo.

NICK_HELP_SET_PASSWORD
	Sintaxe: SET PASSWORD nova-senha
	
	Muda a senha de seu nick.

NICK_HELP_SET_LANGUAGE
	Sintaxe: SET LANGUAGE número
	
	Muda a linguagem que os Services usam ao enviar mensagens
	para você (por exemplo, quando responde um comando que você
	emite). A linguagem Português, assim como outras, pode
	apresentar algumas diferenças da linguagem Inglês (English),
	já que Inglês é a linguagem oficial destes Services.
	O número deve ser escolhido da seguinte lista de línguas:
	

NICK_HELP_SET_URL
	Sintaxe: SET URL url
	
	Associa uma dada URL ao seu nick. Esta URL será mostrada
	sempre que alguem solicitar informação sobre seu nick
	através do comando INFO.

NICK_HELP_SET_EMAIL
	Sintaxe: SET EMAIL endereço
	
	Associa um endereço de email designado ao seu nick. Este
	endereço será sempre mostrado quando alguém solicitar
	informação sobre seu nick atraves do comando INFO.
	O email especificado também é usado para enviar sua senha
	caso seja perdida. Caso não tenha nenhum endereço para seu
	nick ou o mesmo seje inválido, não será possível recuperar
	a senha perdida.
	
NICK_HELP_SET_ICQ
	Sintaxe: SET ICQ número
	
	Associa um número de ICQ ao seu nick. Este número
	será sempre mostrado quando alguém solicitar 
	informação sobre seu nick através do comando INFO.
	
NICK_HELP_SET_GREET
	Sintaxe: SET GREET mensagem
	
	Faz com que a mensagen escolhida se torne a sua mensagem
	de entrada que aparecerá sempre quando você entrar em um canal
	que tenha a opção GREET habilitada, e também tenha acesso a isto.

NICK_HELP_SET_KILL
	Sintaxe: SET KILL {ON | QUICK | IMMED | OFF}
	
	Possibilita a proteção de KILL do seu nick. Com a proteção
	de KILL acionada, se outro usuário tentar tomar seu nick,
	será dado 1 minuto para este mudar de nick; após isso, será
	forçado pelo %S a trocar de nick, ou será desconectado da
	rede (esse último não acontece em todas elas).
	
	Se você selecionar QUICK, será dado ao usuário somente 20
	segundos para mudar de nick ao invés do usual (60 segundos).
	Se você selecionar a opção IMMED, o nick será mudado
	imediatamente sem mesmo ser avisado antes; é recomendado
	não usar essa opção, a não ser que seja necessário. Pode
	acontecer do Administrador da Rede desabilitar essa opção.

NICK_HELP_SET_SECURE
	Sintaxe: SET SECURE {ON | OFF}
	
	Torna os recursos de segurança do %S ativados (ON) ou
	desativados (OFF) sobre o seu nick. Com o SECURE acionado
	você deve entrar com sua senha antes de ser reconhecido como
	usuário do nick, independentemente do fato do seu endereço
	constar na lista de acesso. Entretanto, se você está na
	lista de acesso, o %S não irá dar auto-kill em você de
	acordo com os ajustes da opção KILL.

NICK_HELP_SET_PRIVATE
	Sintaxe: SET PRIVATE {ON | OFF}
	
	Torna a opção de privacidade do %S sobre o seu nick
	em ativada (ON) ou desativada (OFF). Com o PRIVATE ativado,
	seu nick não aparecerá nas listagens de nicks geradas com
	o comando %S's LIST. (Entretando, alguém que sabe do seu
	nick ainda pode obter informações usando o comando INFO).

NICK_HELP_SET_HIDE
	Sintaxe: SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	Permite que você previna que certas informações sejam
	mostradas quando alguém usa %S INFO no seu nick. Você pode
	esconder seu endereço de e-mail (EMAIL), última máscara vista
	user@host (USERMASK), seu status de acesso aos Services
	(STATUS) e sua última mensagem de saída (QUIT).
	O segundo parâmetro especifica quando a informação deve ser
	mostrada (OFF) ou escondida (ON).
	
NICK_HELP_SET_MSG
	Sintaxe: SET MSG {ON | OFF}
	
	Permite que você escolha a forma que os Services devem se 
	comunicar com você. Com MSG acionado, os Services irão usar
	mensagems, caso contrário, usarão notices. 

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

NICK_HELP_SASET_LANGUAGE
	Syntax: SASET nickname LANGUAGE number
	
	Changes the language Services uses when sending messages to
	nickname (for example, when responding to a command he sends).
	number should be chosen from a list of supported languages
	that you can get by typing %R%S HELP SET LANGUAGE.

NICK_HELP_RECOVER
	Sintaxe: RECOVER nick [senha]
	
	Permite que você recupere seu nick se alguém o tiver
	tomado; isso faz o mesmo que o %S faz automaticamente
	se alguém "rouba" um nick protegido com kill.
	
	Quando você usa este comando, o %S trará um falso
	usuário online com o mesmo nick de quem o está usando.
	Isto causará uma desconexão pelo servidor de IRC do outro
	usuário. O falso usuário ficará online por %s para
	assegurar que o outro usuário não reconecte imediatamente;
	Durante você pode reivindicar seu nick.
	Alternativamente, use o comando RELEASE
	(%R%S HELP RELEASE) para retomar seu nick mais 
	rapidamente.
	
	Para usar o comando RECOVER em um nick, seu endereço
	atual como mostrado no /WHOIS deve estar na lista de acesso
	do nick, você deve estar identificado e estar no grupo daquele
	nick, ou você deve fornecer a senha correta para o nick.

NICK_HELP_RELEASE
	Sintaxe: RELEASE nick [senha]
	
	Instrui o %S a remover qualquer espera no seu nick
	causada pelo kill automático de proteção ou pelo uso do
	comando RECOVER. Por padrão, essa espera dura %s;
	este comando libera o seu nick mais cedo.
	
	Para usar o comando RELEASE em um nick, seu endereço
	atual como mostrado no /WHOIS deve estar na lista de acesso 
	do nick, você deve estar identificado e estar no grupo daquele
	nick, ou você deve fornecer a senha correta para o nick.

NICK_HELP_GHOST
	Sintaxe: GHOST nick [senha]
	 
	Termina uma sessão "fantasma" de IRC que esteja usando seu
	nick. A sessão "fantasma" é aquela que não está realmente
	conectada, mas que o servidor de IRC acredita estar ainda
	ativa por qualquer razão. Isso acontece normalmente se seu
	computador travar ou se sua conexão com a internet ou modem
	cair por um período curto de tempo enquanto você está no IRC.
	
	Para usar o comando GHOST em um nick, seu endereço
	atual como mostrado no /WHOIS deve estar na lista de acesso 
	do nick, você deve estar identificado e estar no grupo daquele
	nick, ou você deve fornecer a senha correta para o nick.

NICK_HELP_INFO
	Sintaxe: INFO nick [ALL]
	
	Mostra a informação sobre o nick dado, como o dono do nick,
	último endereço utilizado e horário, e opções. Se você está
	identificado para o nick que você está obtendo informações
	e a opção ALL é especificada, será mostrado a você toda
	informação possível; independente de haver opções para esconder.

NICK_HELP_LIST
	Sintaxe: LIST padrão
	
	Lista todos os nicks registrados que combinam com o padrão
	fornecido no formato nick!ident@host. Nicks com a opção
	PRIVATE ligada não serão mostrados.
	
	Exemplos:
	
	    LIST *!ident@servidor.com.br
	        Lista todos os nicks de *!ident@servidor.com.br.
	
	    LIST *Bot*!*@*
	        Lista todos os nicks registrados com Bot em seus
	        idents (insensível a maiúsculas).
	
	    LIST *!*@*.bar.org
	        Lista todos os nicks possuídos por usuários do domínio
	        bar.org

NICK_HELP_ALIST
	Sintaxe: ALIST [nível]
	
	Lista todos os canais nos quais você tem acesso. Opcionalmente,
	você pode especificar um nível no formato XOP ou ACCESS. A lista
	de resultado inclui apenas os canais nos quais você tem o nível
	de acesso fornecido.
	
	Exemplos:
	ALIST Founder
	     Lista todos os canais nos quais você tem acesso de Founder.
	
	ALIST AOP
	     Lista todos os canais nos quais você tem acesso AOP ou maior.
	
	ALIST 10
	    Lista todos os canais nos quais você tem nível 10 de acesso ou maior.
	
	Canais com a opção NOEXPIRE serão prefixados por uma exclamação.
	
NICK_HELP_GLIST
	Sintaxe: GLIST
	
	Lista todos os nicks do grupo.

NICK_HELP_STATUS
	Sintaxe: STATUS nick...
	
	Retorna se o usuário que está usando um determinado nick
	é reconhecio como dono do nick. 
	A resposta tem este formato:
	
	    nick código-de-status
	
	onde nick é o nick enviado com o comando, e
	código-de-status é um dos seguintes:
	
	    0 - usuário não está conectado ou o nick não está registrado
	    1 - usuário não reconhecido como dono do nick
	    2 - usuário reconhecido como dono só pela lista de acesso
	    3 - usuário reconhecido como dono via identificação por senha
	
	Até 16 nicks podem ser enviados com cada comando; o resto será
	ignorado. Se nenhum nick for fornecido, seu status será exibido.
	
NICK_HELP_SENDPASS
	Sintaxe: SENDPASS nick
	
	Envia a senha do nick em questão para o endereço de email
	ajustado no nick. Esse comando é bem útil em se falando
	de senhas perdidas.
	
	Poder ser limitado a Operadores de IRC em algumas redes.
	
	Este comando não está disponível quando estiver usando encriptação.

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
	Os Administradores dos Services podem ainda usar o DROP em qualquer nick
	sem ser necessário se identificar como dono dele, e podem ver a lista de
	acesso de qualquer nick (%R%S ACCESS LIST nick).

NICK_SERVADMIN_HELP_LOGOUT
	Sintaxe: LOGOUT [nick [REVALIDATE]]
	
	Sem nenhum parâmetro, reverte o efeito do comando IDENTIFY,
	ou seja, faz com que você não seja mais reconhecido como o
	verdadeiro dono do nick. Note, entretanto, que ele não pedirá
	que você se identifique novamente.
	
	Com um parâmetro, fará o mesmo para o nick determinado. Se você
	especificar REVALIDATE também, os Services irão pedir que o nick
	em questão se identifique novamente. 
	
	Comando limitado aos Administradores dos Services.

NICK_SERVADMIN_HELP_DROP
	Sintaxe: DROP [nick]
	
	Sem parâmetros, cancela o registro de seu nick no banco de
	dados do %S.
	
	Com parâmetros, cancela o registro do nick especificado
	do banco de dados. Você pode desregistrar qualquer nick DE
	SEU GRUPO sem nenhum privilégio especial, mas desregistrar
	QUALQUER nick é restrito aos Administradores dos Services.

NICK_SERVADMIN_HELP_INFO
	
	Administradores de Services podem usar o parâmetro ALL com qualquer nick.

NICK_SERVADMIN_HELP_LIST
	Sintaxe: LIST padrão [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	Lista todos os nicks registrados que combinem com o padrão
	fornecido (formato nick!ident@host). Nicks com opção PRIVATE
	serão somente exibidos para os Administradores dos Services.
	Nicks com opção NOEXPIRE ajustada terão um ! preposto ao nick
	na lista quando for a pedido de Administrador dos Services.
	
	Se as opções FORBIDDEN, SUSPENDED, NOEXPIRE ou UNCONFIRMED são fornecidas, apenas
	nicks que são, respectivamente, FORBIDDEN, SUSPENDED, UNCONFIRMED ou têm a opção
	NOEXPIRE ativa, serão exibidos. Se todas as três opções forem fornecidas,
	todos os três tipos de nicks serão exibidos. Estas opções são limitas aos
	Administradores dos Services.
	
	Exemplos:
	
	     LIST *!ident@servidor.com.br
	        Lista todos os nicks de *!ident@servidor.com.br.
	
	     LIST *Bot*!*@*
	        Lista todos os nicks registrados com Bot em seus nomes.
	
	     LIST * NOEXPIRE
	        Lista todos os nicks registrados cujos registros foram
	        ajustados para não expirar.

NICK_SERVADMIN_HELP_ALIST
	Sintaxe: ALIST [nick] [nível]
	
	Sem parâmetros, lista os canais nos quais você tem acesso. Com
	um parâmetro, lista os canais nos quais nick tem acesso.
	Com dois parâmetros lista os canais nos quais nick tem
	no mínimo o nível de acesso especificado.
	
	Este uso é limitado aos Administradores dos Services.

NICK_SERVADMIN_HELP_GLIST
	Sintaxe: GLIST [nick]
	
	Sem nenhum parâmetro, lista todos os nicks no seu grupo.
	Com um parâmetro, lista todos os nicks no grupo determinado.
	
	Este comando é limitado aos Administradores dos Services.

NICK_SERVADMIN_HELP_GETPASS
	Sintaxe: GETPASS nick
	
	Mostra a senha de um nick especificado.
	Note que sempre que este comando for usado, uma mensagem
	incluindo o nick da pessoa que emitiu o comando e o nick
	no qual foi usado serão gravados e enviados para WALLOPS/GLOBOPS.
	
	Este comando não fica disponível quando a encriptação está habilitada.
	
	Limitado aos Administradores dos Services.

NICK_SERVADMIN_HELP_GETEMAIL
	Sintaxe: GETEMAIL usuário@emailhost
	
	Retorna os nicks que utilizam o email dado. Observe que
	você não pode usar coringas para usuário nem para emailhost.
	Sempre que este comando for usado, uma mensagem incluindo a pessoa
	que emitiu o comando e o email no qual foi usado serão gravados.
	
	Limitado aos Administradores dos Services.

NICK_SERVADMIN_HELP_FORBID
	Sintaxe: FORBID nick [motivo]
	
	Não permite que um nick seja registrado ou usado por
	ninguém. Pode ser cancelado desregistrando o nick.
	
	Em algumas redes, um motivo é necessário.
	
	Limitado aos Administradores dos Services.

NICK_SERVADMIN_HELP_SUSPEND
	Sintaxe: SUSPEND nick motivo

	SUSPENDE um nick, impedindo-o de ser usado.

	Limitado aos Administradores dos Services.

NICK_SERVADMIN_HELP_UNSUSPEND
	Sintaxe: UNSUSPEND nick

	LIBERA um nick suspendido, permitindo que seja usado.

	Limitado aos Administradores dos Services.

###########################################################################
#
# ChanServ help messages
#
###########################################################################
CHAN_HELP_CMD_GETPASS
	    GETPASS    Recupera a senha de um canal
CHAN_HELP_CMD_FORBID
	    FORBID     Previne um canal de ser registrado/usado
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    Previne um canal de ser registrado/usado,
	               preservando os dados do canal e suas configurações
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Libera um canal suspenso
CHAN_HELP_CMD_STATUS
	    STATUS     Fornece o atual nível de acesso de um usuário em um canal
CHAN_HELP_CMD_REGISTER
	    REGISTER   Registra um canal
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   Identifica seu nick
CHAN_HELP_CMD_SET
	    SET        Ajusta as opções e informações do canal
CHAN_HELP_CMD_AOP
	    AOP        Modifica a lista de AOP (AutoOP) de um canal
CHAN_HELP_CMD_SOP
	    SOP        Modifica a lista de SOP (SuperOP) de um canal
CHAN_HELP_CMD_ACCESS
	    ACCESS     Modifica a lista de usuários privilegiados
CHAN_HELP_CMD_LEVELS
	    LEVELS     Redefine os níveis de acesso
CHAN_HELP_CMD_AKICK
	    AKICK      Faz a manutenção da Lista de Autokick
CHAN_HELP_CMD_DROP
	    DROP       Cancela o registro de um canal
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Ajuda para recuperar senhas
CHAN_HELP_CMD_BAN
	    BAN        Bane o nick selecionado em um canal
CHAN_HELP_CMD_CLEAR
	    CLEAR      Diz ao ChanServ para remover certos ajustes de um canal
CHAN_HELP_CMD_DEVOICE
	    DEVOICE    Retira o status de Voice de um nick no canal
CHAN_HELP_CMD_GETKEY
	    GETKEY     Retorna a key (do modo +k) do canal fornecido
CHAN_HELP_CMD_INFO
	    INFO       Mostra informações sobre algum canal registrado
CHAN_HELP_CMD_INVITE
	    INVITE     Tells ChanServ to invite you into a channel
CHAN_HELP_CMD_KICK
	    KICK       Kicka (expulsa) um nick de um canal
CHAN_HELP_CMD_LIST
	    LIST       Lista todos os canais registrados que combinam com o padrão fornecido
CHAN_HELP_CMD_LOGOUT
	    LOGOUT     Desconecta um determinado nick
CHAN_HELP_CMD_OP
	    OP         Atribui status de OP a um nick no canal
CHAN_HELP_CMD_TOPIC
	    TOPIC      Modifica o tópico de um canal
CHAN_HELP_CMD_UNBAN
	    UNBAN      Remove todos os bans permitindo que você entre em um canal
CHAN_HELP_CMD_VOICE
	    VOICE      Atribui status de Voice a um nick no canal
CHAN_HELP_CMD_VOP
	    VOP        Modifica a lista de VOP (VOicePeople) de um canal
CHAN_HELP_CMD_DEHALFOP
	    DEHALFOP   Retira o status de HalfOP de um nick no canal
CHAN_HELP_CMD_DEOWNER
	    DEOWNER    Retira seu status de Owner no canal
CHAN_HELP_CMD_DEPROTECT
	    DEPROTECT  Desprotege um determinado nick no canal
CHAN_HELP_CMD_HALFOP
	    HALFOP     Atribui status de HalfOP a um nick no canal
CHAN_HELP_CMD_HOP
	    HOP        Modifica a lista de HOP (HalfOP) de um canal
CHAN_HELP_CMD_OWNER
	    OWNER      Concede-lhe status de Owner no canal
CHAN_HELP_CMD_PROTECT
	    PROTECT    Protege um determinado nick no canal
CHAN_HELP_CMD_ADMIN
	    ADMIN      Protege um determinado nick no canal
CHAN_HELP_CMD_DEADMIN
	    DEADMIN    Desprotege um determinado nick no canal
CHAN_HELP_CMD_DEOP
	    DEOP       Retira o status de OP de um nick no canal

CHAN_HELP
	%S permite que você registre e controle vários
	aspectos do canal. %S pode prevenir o "take over"
	do canal por usuários maliciosos, limitando quem possui
	o privilégio de OP do canal. Os comandos disponíveis
	estão listados abaixo; para usá-los, digite
	%R%S comando. Para mais informações sobre
	um comando específico, digite %R%S HELP comando.
	
	
CHAN_HELP_EXPIRES
	
	Note que qualquer canal não utilizado por %d dias
	(entenda como se nenhum usuário da lista de acesso
	desse canal entrar neste período de tempo) terá
	automaticamente cancelado o seu registro.

CHAN_HELP_REGISTER
	Sintaxe: REGISTER canal senha descrição
	
	Registra um canal no banco de dados do %S sob seu nick. 
	Para utilizar este comando, você deve primeiro ser OP
	do canal que está tentando registrar.
	
	A senha é usada com o comando IDENTIFY para permitir
	que outras pessoas façam alterações nas configurações do
	canal. O último parâmetro, que deve ser incluído, é uma
	descrição geral da finalidade do canal.
	
	Quando você registra um canal, você é gravado como "fundador"
	do mesmo. Ao fundador do canal é permitido alterar todas as
	configurações do mesmo; %S irá dar automaticamente ao fundador
	privilégios de Operador de Canal quando ele entrar no canal.
	Veja o comando ACCESS (%R%S HELP ACCESS) para
	informação sobre como liberar uma parte dos seus privilégios
	a outros usuários do canal.
	
	IMPORTANTE: Para registrar um canal, você deve antes ter
	registrado seu nick. Se ainda não tiver seu nick registrado,
	digite %R%s HELP para mais informações de como fazê-lo.

CHAN_HELP_IDENTIFY
	Sintaxe: IDENTIFY canal senha
	
	Identifica você no %S como fundador do canal fornecido.
	Muitos comandos exigem que você use este comando antes
	de usá-los. A senha deverá ser a mesma usada por você
	no comando REGISTER.
	
CHAN_HELP_LOGOUT
	Sintaxe: LOGOUT canal nick
	
	Este comando faz com que um determinado nick seja des-identificado
	do canal, fazendo com que seja necessário ele se identificar
	novamente para recuperar seu acesso.
	
	Se você for o Founder do canal, pode usar este comando em
	qualquer usuário e em si mesmo.

CHAN_HELP_DROP
	Sintaxe: DROP canal
	
	Cancela o registro do canal. So poderá ser usado pelo
	fundador do canal, que primeiramente deverá estar
	identificado com o NickServ.

CHAN_HELP_SET
	Sintaxe: SET canal opção parâmetros
	
	Permite ao fundador do canal ajustar as várias opções do
	canal e outras informações.
	
	Opções disponíveis:
	
	    FOUNDER       Ajusta o fundador do canal
	    SUCCESSOR     Ajusta o sucessor do canal
	    PASSWORD      Ajusta a senha do founder
	    DESC          Ajusta a descrição do canal
	    URL           Associa uma URL a um canal
	    EMAIL         Associa um E-mail a um canal
	    ENTRYMSG      Ajusta a mensagem que deve ser mandada
	                    aos usuários quando eles entram no canal
	    BANTYPE       Ajusta como os Services devem fazer o ban
	    MLOCK         Liga ou desliga a trava dos modos do canal
	    KEEPTOPIC     Mantem o tópico quando o canal não está em uso
	    TOPICLOCK     O tópico pode ser mudado apenas com o comando TOPIC
	    PEACE         Regula o uso de comandos críticos
	    PRIVATE       Esconde o canal do comando LIST
	    RESTRICTED    Acesso restrito ao canal
	    SECURE        Ativa os recursos de segurança do %S
	    SECUREOPS     Controle rigoroso do status de OP
	    SECUREFOUNDER Controle rigoroso do status de Fundador do Canal
	    SIGNKICK      Assinatura dos kicks feitos pelo comando KICK
	    OPNOTICE      Envia uma notice quando os comandos OP/DEOP são usados
	    XOP           Altera o sistema de privilégio de usuário
	
	Digite %R%S HELP SET opção para maiores informações
	sobre uma opção em particular.

CHAN_HELP_SET_FOUNDER
	Sintaxe: SET canal FOUNDER nick
	
	Muda o fundador do canal. O novo nick deve estar
	registrado.

CHAN_HELP_SET_SUCCESSOR
	Sintaxe: SET canal SUCCESSOR nick
	
	Altera o sucessor de um canal. Se o nick do founder expirar
	ou for dropado enquanto o canal ainda estiver registrado,
	o sucessor se tornará o novo founder do canal.
	No entanto, se o sucessor possuir muitos canais registrados
	(%d), o canal será dropado, como se nenhum sucessor tivesse
	sido ajustado. O novo nick deverá estar registrado.

CHAN_HELP_SET_PASSWORD
	Sintaxe: SET canal PASSWORD senha
	
	Ajusta a senha utilizada para identificação do
	fundador do canal.

CHAN_HELP_SET_DESC
	Sintaxe: SET canal DESC descrição
	
	Ajusta a descrição para o canal, que são mostradas com
	os comandos LIST e INFO.

CHAN_HELP_SET_URL
	Sintaxe: SET canal URL [url]
	
	Associa uma URL ao canal. Esta URL será mostrada
	sempre que alguem solicitar informação sobre o canal
	com o comando INFO. Se nenhum parâmetro for fornecido,
	apaga qualquer URL atualmente designada para o canal.

CHAN_HELP_SET_EMAIL
	Sintaxe: SET canal EMAIL [endereço]
	
	Associa um endereço de e-mail fornecido ao canal.
	Este endereço será mostrado sempre que alguem solicitar
	informação sobre o canal com o comando INFO. Se nenhum
	parâmetro for fornecido, apaga qualquer endereço de e-mail
	do canal.

CHAN_HELP_SET_ENTRYMSG
	Sintaxe: SET canal ENTRYMSG [mensagem]
	
	Ajusta a mensagem na qual será enviada via /notice para
	os usuários quando eles entrarem no canal. Se nenhum
	parâmetro é fornecido, nenhuma mensagem será enviada
	ao se entrar no canal.
	
CHAN_HELP_SET_BANTYPE
	Sintaxe: SET canal BANTYPE tipo-de-ban
	
	Seleciona o tipo de ban que será usado sempre que os Services
	precisarem banir alguém do seu canal.
	
	O tipo-de-ban é um número entre 0 e 3 que significa:
	
	0: ban na forma *!user@host
	1: ban na forma *!*user@host
	2: ban na forma *!*@host
	3: ban na forma *!*user@*.domain

CHAN_HELP_SET_KEEPTOPIC
	Sintaxe: SET canal KEEPTOPIC {ON|OFF}
	
	Ativa ou desativa a opção de retenção de tópico para
	um canal. Quando a retenção de tópico está acionada,
	o tópico para o canal será lembrado pelo %S
	mesmo depois que os usuários deixarem o canal, e será
	restaurado na próxima vez que o canal for reutilizado.

CHAN_HELP_SET_TOPICLOCK
	Sintaxe: SET canal TOPICLOCK {ON|OFF}
	
	Ativa ou desativa a opção de trava do tópico de um canal.
	Quando a trava do tópico está ativada, o %S não
	permitirá a mudança do tópico do canal exceto pelo comando 
	TOPIC.

CHAN_HELP_SET_MLOCK
	Sintaxe: SET canal MLOCK modos
	
	Ajusta o parâmetro da trava do modo para um canal. O
	%S permite que você defina certos modos no canal
	a serem sempre ativo, desativos ou não travados.
	Os modos podem ser i, k, l, m, n, p, s, S, t
	entre outros.
	
	O parâmetro modos é usado exatamente do mesmo modo
	que o comando /MODE, ou seja, modos seguidos de + são
	travados como ativos, e modos seguidos de - são travados
	como desativos. Note, no entanto, que ao contrário do
	comando /MODE, cada uso do SET MLOCK irá remover
	todos os modos anteriormente travados antes de travar
	os novos.
	
	Cuidado: Se você trava uma senha, como no segundo exemplo
	abaixo, você deveria também ajustar a opção RESTRICTED para
	o canal (veja HELP SET RESTRICTED), ou qualquer
	usuário que entrar no canal que está vazio será capaz de ver a senha!
	
	Exemplos:
	
	    SET #canal MLOCK +nt-iklps
	        Trava os modos n e t ativados, e os modos i, k, l, p,
	        e s desligados. Modo m é livremente deixado para ser
	        ativo ou desativo (não travado).
	
	    SET #canal MLOCK +knst-ilmp minha-senha
	        Trava os modos k, n, s, t como ligados e os modos i,
	        l, m, p como desligados. Também trava a senha do canal
	        como "minha-senha".
	
	    SET #canal MLOCK +
	        Elimina a trava do modos; todos os modos do canal
	        ficam livres para ser ativos ou desativos.

CHAN_HELP_SET_PEACE
	Sintaxe: SET canal PEACE {ON | OFF}
	
	Ativa ou destiva a opção peace para um canal.
	Quando peace está ativada, um usuário não poderá kickar,
	banir ou remover os modos no canal de um usuário que tenha
	nível superior ou igual ao seu via comandos do %S.

CHAN_HELP_SET_PRIVATE
	Sintaxe: SET canal PRIVATE {ON|OFF}
	
	Ativa ou desativa a opção private para um canal. Quando
	private está ativada, um %R%S LIST não irá incluir
	o canal em qualquer listagem.

CHAN_HELP_SET_RESTRICTED
	Sintaxe: SET canal RESTRICTED {ON|OFF}
	
	Enables or disables the restricted access option for a
	channel.  When restricted access is set, users not on
	the access list will instead be kicked and banned from the channel.

CHAN_HELP_SET_SECURE
	Sintaxe: SET canal SECURE {ON|OFF}
	
	Ativa ou desativa os recursos de segurança do %S
	para um canal. Quando o SECURE estiver ativo, somente
	usuários que possuirem seus nicks registrados no %s
	e identificados com sua senha poderão entrar no canal
	controlado pela lista de acesso.

CHAN_HELP_SET_SECUREOPS
	Sintaxe: SET canal SECUREOPS {ON|OFF}
	
	Ativa ou desativa a opção de segurança de ops para um canal.
	Quando secure ops estiver ativada, os usuários que não
	estiverem na lista de acesso não poderão ter o status de OP.
	
CHAN_HELP_SET_SECUREFOUNDER
	Sintaxe: SET canal SECUREFOUNDER {ON | OFF}
	
	Ativa ou desativa a opção secure founder para um canal.
	Quando o secure founder está ligado, apenas o verdadeiro founder
	poderá dar DROP no canal, mudar sua senha, seu founder e seu
	successor, e não aqueles que estiverem identificados no %S.
	
CHAN_HELP_SET_SIGNKICK
	Sintaxe: SET canal SIGNKICK {ON | LEVEL | OFF}
	
	Habilita ou desabilita a assinatura nos kicks para um
	canal. Quando o SIGNKICK está habilitado, kicks através
	do comando %S KICK terão o nick de quem usou o comando
	no motivo do kick.
	
	Se você usar LEVEL, aqueles que tiverem um nível igual ou 
	superior ao nível do SIGNKICK não terão o nick aparecendo no 
	motivo. Veja %R%S HELP LEVELS para maiores informações. 

CHAN_HELP_SET_XOP
	Sintaxe: SET canal XOP {ON | OFF}
	
	Ativa ou desativa o sistema de listas xOP para um canal.
	Quando XOP está ativado, você deve usar os comandos
	AOP/SOP/VOP para poder dar privilégios aos usuários
	do canal, a não ser que você deva usar o comando ACCESS.
	
	Nota técnica: quando você muda de lista de acesso para
	o sistema de listas xOP, sua definição de níveis e níveis
	de usuário serão alteradas, assim, você não encontrará os
	mesmos valores se você voltar ao sistema de acesso!
	
	Você também deve verificar se seus usuários ficaram em uma
	lista xOP correta depois que você mudou de sistema de acesso
	para listas xOP, porque a conversão nem sempre é perfeita...
	de fato, não é recomendável usar listas xOP caso tenha alterado
	suas definições de nível com o comando LEVELS.
	
	Entretanto, mudar do sistema de listas xOP para sistema de lista
	de acesso não causa problemas.

CHAN_HELP_SET_OPNOTICE
	Sintaxe: SET canal OPNOTICE {ON | OFF}
	
	Ativa e desativa a opção Op-notice para um canal. Quando
	Op-notice estiver ativado, o %S enviará uma notice para
	o canal sempre que os comandos OP ou DEOP forem usados em um
	usuário no canal.

CHAN_HELP_AOP
	Sintaxe: AOP canal ADD nick
	         AOP canal DEL {nick | entrada | lista-entradas}
	         AOP canal LIST [máscara | lista-entradas]
	         AOP canal CLEAR
	
	Mantém a lista de AOP (AutoOP) para um canal. A lista
	de AOP dá a um usuário os direitos de: receber OP
	automaticamente ao entrar no canal, de se desbanir ou convidar
	a si próprio se necessário, de ter sua mensagem de entrada
	exibida ao entrar no canal, e assim por diante.
	
	O comando AOP ADD adiciona o nick fornecido à lista de AOP.
	
	O comando AOP DEL remove o nick fornecido da lista de AOP.
	Se uma lista de números de entrada é fornecida, essas entradas
	são deletadas. (Veja o exemplo para LIST abaixo.)
	
	O comando AOP LIST exibe a lista de AOP. Se uma máscara
	é fornecida, apenas as entradas que combinarem com ela serão
	mostradas. Se uma lista de números de entrada é fornecida,
	apenas essas entradas são mostradas; por exemplo:
	
	   AOP #canal LIST 2-5,7-9
	      Lista as entradas de AOP numeradas de 2 a 5 e de
	      7 a 9.
	
	O comando AOP CLEAR remove todas as entradas da lista de AOP.
	
	Os comandos AOP ADD e AOP DEL são limitados aos SOPs
	ou acima, enquanto o comando AOP CLEAR pode ser usado apenas
	pelo Fundador do canal. No entanto, qualquer usuário na lista de
	AOP pode usar o comando AOP LIST.
	
	Este comando pode ter sido desabilitado para seu canal
	e, nesse caso, você precisa usar a lista de acesso. Veja
	%R%S HELP ACCESS para informações sobre a lista
	de acesso, e %R%S HELP SET XOP para saber como
	escolher entre a lista de acesso e o sistema de listas xOP.

CHAN_HELP_HOP
	Sintaxe: HOP canal ADD nick
	         HOP canal DEL {nick | entrada | lista-entradas}
	         HOP canal LIST [máscara | lista-entradas]
	         HOP canal CLEAR
	
	Mantém a lista de HOP (HalfOP) para um canal. A lista
	de HOP dá a um usuário o direito de receber HalfOP
	automaticamente ao entrar no canal.
	
	O comando HOP ADD adiciona o nick fornecido à lista de HOP.
	
	O comando HOP DEL remove o nick fornecido da lista de HOP.
	Se uma lista de números de entrada é fornecida, essas entradas
	são deletadas. (Veja o exemplo para LIST abaixo.)
	
	O comando HOP LIST exibe a lista de HOP. Se uma máscara
	é fornecida, apenas as entradas que combinarem com ela serão
	mostradas. Se uma lista de números de entrada é fornecida,
	apenas essas entradas são mostradas; por exemplo:
	
	   HOP #canal LIST 2-5,7-9
	      Lista as entradas de HOP numeradas de 2 a 5 e de
	      7 a 9.
	
	O comando HOP CLEAR remove todas as entradas da lista de HOP.
	
	Os comandos HOP ADD, HOP DEL e HOP LIST são limitados
	aos AOPs ou acima, enquanto o comando HOP CLEAR pode ser usado
	apenas pelo Fundador do canal.
	
	Este comando pode ter sido desabilitado para seu canal
	e, nesse caso, você precisa usar a lista de acesso. Veja
	%R%S HELP ACCESS para informações sobre a lista
	de acesso, e %R%S HELP SET XOP para saber como
	escolher entre a lista de acesso e o sistema de listas xOP.

CHAN_HELP_SOP
	Sintaxe: SOP canal ADD nick
	         SOP canal DEL {nick | entrada | lista-entradas}
	         SOP canal LIST [máscara | lista-entradas]
	         SOP canal CLEAR
	
	Mantém a lista de SOP (SuperOP) para um canal. A lista
	de SOP dá a um usuário todos os direitos dados pela lista de
	AOP, e adiciona a isso os direitos de: usar as listas de AutoKick
	e BadWords, enviar e ler memos do canal, e assim por diante.
	
	O comando SOP ADD adiciona o nick fornecido à lista de SOP.
	
	O comando SOP DEL remove o nick fornecido da lista de SOP.
	Se uma lista de números de entrada é fornecida, essas entradas
	são deletadas. (Veja o exemplo para LIST abaixo.)
	
	O comando SOP LIST exibe a lista de SOP. Se uma máscara
	é fornecida, apenas as entradas que combinarem com ela serão
	mostradas. Se uma lista de números de entrada é fornecida,
	apenas essas entradas são mostradas; por exemplo:
	
	   SOP #canal LIST 2-5,7-9
	      Lista as entradas de SOP numeradas de 2 a 5 e de
	      7 a 9.
	
	O comando SOP CLEAR remove todas as entradas da lista de SOP.
	
	Os comandos SOP ADD, SOP DEL e SOP CLEAR são limitados
	ao Fundador do canal. No entanto, qualquer usuário na lista de AOP
	pode usar o comando SOP LIST.
	
	Este comando pode ter sido desabilitado para seu canal
	e, nesse caso, você precisa usar a lista de acesso. Veja
	%R%S HELP ACCESS para informações sobre a lista
	de acesso, e %R%S HELP SET XOP para saber como
	escolher entre a lista de acesso e o sistema de listas xOP.

CHAN_HELP_VOP
	Sintaxe: VOP canal ADD nick
	         VOP canal DEL {nick | entrada | lista-entradas}
	         VOP canal LIST [máscara | lista-entradas]
	         VOP canal CLEAR
	
	Mantém a lista de VOP (VOicePeople) para um canal. A lista
	de VOP permite a um usuário receber Voice automaticamente
	ao entrar no canal e dar Voice a si mesmo se não possuir.
	
	O comando VOP ADD adiciona o nick fornecido à lista de VOP.
	
	O comando VOP DEL remove o nick fornecido da lista de VOP.
	Se uma lista de números de entrada é fornecida, essas entradas
	são deletadas. (Veja o exemplo para LIST abaixo.)
	
	O comando VOP LIST exibe a lista de VOP. Se uma máscara
	é fornecida, apenas as entradas que combinarem com ela serão
	mostradas. Se uma lista de números de entrada é fornecida,
	apenas essas entradas são mostradas; por exemplo:
	
	   VOP #canal LIST 2-5,7-9
	      Lista as entradas de VOP numeradas de 2 a 5 e de
	      7 a 9.
	
	O comando VOP CLEAR remove todas as entradas da lista de VOP.
	
	Os comandos VOP ADD, VOP DEL e VOP LIST são limitados
	aos AOPs ou acima, enquanto o comando VOP CLEAR pode ser usado
	apenas pelo Fundador do canal.
	
	Este comando pode ter sido desabilitado para seu canal
	e, nesse caso, você precisa usar a lista de acesso. Veja
	%R%S HELP ACCESS para informações sobre a lista
	de acesso, e %R%S HELP SET XOP para saber como
	escolher entre a lista de acesso e o sistema de listas xOP.

CHAN_HELP_ACCESS
	Sintaxe: ACCESS canal ADD nick nível
	         ACCESS canal DEL {nick | entrada | lista-entradas}
	         ACCESS canal LIST [máscara | lista-entradas]
	         ACCESS canal CLEAR
	
	Faz manutenção da lista de acesso de um canal. A lista
	de acesso especifica que usuários tem permissão para
	status de OP ou tem acesso aos comandos do %S em
	um canal. Níveis diferentes de usuários permitem acesso
	a diferentes subníveis de privilégios; %R%S HELP
	ACCESS LEVELS para mais informações específicas. Por
	padrão, qualquer nick não presente na lista de acesso
	possui nível 0.
	
	O comando ACCESS ADD adiciona um nick ou máscara de um
	usuário na lista de acesso com o nível especificado; se o
	nick ou máscara já estiver presente na lista, seu nível de
	acesso será alterado para o nível especificado. O nível
	especificado deve ser menor do que o do usuário que emitiu o
	comando, e se o nick já estiver na lista de acesso, o
	nível de acesso atual desse nick deve ser menor que o nível
	de acesso do usuário que emitiu o comando.
	
	O comando ACCESS DEL remove um nick especificado da lista
	de acesso. Se uma lista de números é fornecida, essas
	entradas são apagadas. (Veja o exemplo para LIST abaixo.)
	
	O comando ACCESS LIST mostra a lista de acesso do canal.
	Se uma máscara for fornecida, somente aquelas entradas que
	combinarem com o padrão especificado na máscara serão 
	mostradas. Se uma lista de números é fornecida, somente
	essas entradas serão mostradas, por exemplo:
	
	ACCESS #canal LIST 2-5,7-9
	   Lista as entradas de acesso numeradas de 2 a 5 e de 7 a 9.
	
	O comando ACCESS CLEAR limpa todas as entradas da lista
	de acesso.

CHAN_HELP_ACCESS_LEVELS
	Níveis de Acesso do Usuário
	
	Por padrão, os seguintes níveis de acesso são definidos:
	
	   Founder   Acesso total às funções do %S; OP automático
	                  quando entrar no canal. Note que somente uma
	                  pessoa pode ter o status fundador (isto não pode
	                  ser dado usando o comando ACCESS).
	        10   Acesso ao comando AKICK; OP automático.
	         5   OP automático.
	         3   Voice automático.
	         0   Sem privilégios especiais; pode se tornar OP
	                  através de outros OPs (a menos que secure-ops
	                  esteja ativada).
	        <0   Não pode ser OP.
	
	Esses níveis podem ser mudados, ou adicionar novos, usando o
	comando LEVELS; digite %R%S HELP LEVELS para informações.

CHAN_HELP_AKICK
	Sintaxe: AKICK canal ADD {nick | máscara} [razão]
	         AKICK canal STICK máscara
	         AKICK canal UNSTICK máscara
	         AKICK canal DEL {nick | máscara | entry-num | list}
	         AKICK canal LIST [máscara | entry-num | list]
	         AKICK canal VIEW [máscara | entry-num | list]
	         AKICK canal ENFORCE
	         AKICK canal CLEAR
	
	Faz a manutenção da lista de AutoKick de um canal.
	Se um usuário que estiver na lista de AutoKick tentar
	entrar no canal, o %S banirá este usuário do 
	canal, e depois kickará o usuário.
	
	O comando AKICK ADD adiciona o usuário especificado ou
	máscara na lista de AutoKick. Se uma razão é dada com
	o comando, esta razão será usada quando o usuário for
	kickado; se não, a razão padrão será "Você foi banido
	deste canal".
	When akicking a registered nick the nickserv account
	will be added to the akick list instead of the mask.
	All users within that nickgroup will then be akicked.
	
	O comando AKICK STICK bane permanentemente no canal
	a máscara fornecida. Se alguém tentar remover o ban,
	%S irá automaticamente colocá-lo de volta. Você
	não pode usar isto para nicks registrados.
	
	O comando AKICK UNSTICK cancela o efeito do comando
	AKICK STICK, assim você poderá remover o ban no canal.
	
	O comando AKICK DEL remove um nick ou máscara da lista
	de AutoKick. Isso, entretanto, não remove nenhum ban
	colocado pelo AutoKick; estes devem ser removidos
	manualmente.
	
	O comando AKICK LIST mostra a lista de AutoKick ou,
	opcionalmente, só os registros de AutoKick que se
	encaixam com o padrão de uma máscara dada.
	
	O comando AKICK VIEW é uma versão mais completa do
	comando AKICK LIST.
	
	O comando AKICK ENFORCE obriga o %S a verificar
	a lista de AutoKicks para banir os usuários que sejam
	compatíveis com alguma máscara da lista de AutoKick.
	
	O comando AKICK CLEAR limpa todas as entradas da lista
	de AutoKick.

CHAN_HELP_LEVELS
	Sintaxe: LEVELS canal SET tipo nível
	         LEVELS canal {DIS | DISABLE} tipo
	         LEVELS canal LIST
	         LEVELS canal RESET
	
	O comando LEVELS permite ajuste detalhado sobre o
	significado dos níveis de acesso numéricos usados pelos
	canais. Com esse comando, você pode definir o nível de
	acesso requerido pela maioria das funções do %S.
	(Os comandos SET FOUNDER e SET PASSWORD, assim como
	este comando, são sempre restritos ao Fundador do canal).
	
	LEVELS SET permite que o nível de acesso para uma função ou
	grupo de funções seja alterado. LEVELS DISABLE (ou DIS
	para encurtar) desabilita um recurso automático ou não permite
	o acesso a funções para qualquer um que não seja o Fundador
	do canal. LEVELS LIST mostra os níveis atuais para cada
	função ou para um grupo de funções. LEVELS RESET recoloca
	os níveis de acordo com o padrão de um canal recém-criado
	(veja HELP ACCESS LEVELS).
	
	Para lista de recursos e funções cujos níveis podem ser
	ajustados, veja HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Os seguintes nomes de recursos/funções são entendidos.
	Note que os níveis para AUTODEOP e NOJOIN são os níveis
	máximos, enquanto que todos os outros são níveis mínimos.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Sintaxe: INFO canal [ALL]
	
	Lista as informações referentes ao canal registrado dado,
	incluindo seu fundador, data do registro, última vez que
	foi usado, descrição, e trava dos modos (se existirem).
	
	Se você está identificado como fundador do canal que está
	solicitando informações e a opção ALL é especificada,
	informações sobre a mensagem de entrada e sobre o sucessor
	do canal também serão mostradas.

CHAN_HELP_LIST
	Sintaxe: LIST padrão
	
	Lista todos os canais registrados que combinam com o padrão
	fornecido. (Canais com opção PRIVATE ativada não serão listados).

CHAN_HELP_OP
	Sintaxe: OP #canal nick
	
	Atribui status de Operador ao nick dado em um determinado
	canal. Se não for fornecido um nick, o status será atribuído
	a você mesmo. Se nem canal nem nick forem fornecidos, o
	status será atribuído a você mesmo em todos os canais onde
	estiver, caso tenha nível suficiente para usar o comando em
	cada um.
	
	Por padrão, é limitado aos AOPs ou àqueles que possuem
	nível de acesso 5 ou superior no canal.

CHAN_HELP_DEOP
	Sintaxe: DEOP #canal nick
	
	Remove o status de Operador do nick dado em um determinado
	canal. Se não for fornecido um nick, o status será removido
	de você mesmo. Se nem canal nem nick forem fornecidos, o
	status será removido de você mesmo em todos os canais onde
	estiver, caso tenha nível suficiente para usar o comando em
	cada um.
	
	Por padrão, é limitado aos AOPs ou àqueles que possuem
	nível de acesso 5 ou superior no canal.

CHAN_HELP_VOICE
	Sintaxe: VOICE [#canal [nick]]
	
	Atribui status de Voice ao nick dado em um determinado
	canal. Se não for fornecido um nick, o status será atribuído
	a você mesmo. Se nem canal nem nick forem fornecidos, o
	status será atribuído a você mesmo em todos os canais onde
	estiver, caso tenha nível suficiente para usar o comando em
	cada um.
	
	Por padrão, é limitado aos AOPs ou àqueles que possuem
	nível de acesso 5 ou superior no canal; ou aos VOPs ou
	àqueles que possuem nível de acesso 3 ou superior, para
	atribuírem o status a eles mesmos.

CHAN_HELP_DEVOICE
	Sintaxe: DEVOICE [#canal [nick]]
	
	Remove o status de Voice do nick dado em um determinado
	canal. Se não for fornecido um nick, o status será removido
	de você mesmo. Se nem canal nem nick forem fornecidos, o
	status será removido de você mesmo em todos os canais onde
	estiver, caso tenha nível suficiente para usar o comando em
	cada um.
	
	Por padrão, é limitado aos AOPs ou àqueles que possuem
	nível de acesso 5 ou superior no canal; ou aos VOPs ou
	àqueles que possuem nível de acesso 3 ou superior, para
	atribuírem o status a eles mesmos.
	
CHAN_HELP_HALFOP
	Sintaxe: HALFOP [#canal [nick]]
	
	Atribui status de HalfOP ao nick dado em um determinado
	canal. Se não for fornecido um nick, o status será atribuído
	a você mesmo. Se nem canal nem nick forem fornecidos, o
	status será atribuído a você mesmo em todos os canais onde
	estiver, caso tenha nível suficiente para usar o comando em
	cada um.
	
	Por padrão, é limitado aos AOPs ou àqueles que possuem
	nível de acesso 5 ou superior no canal; ou aos HOPs ou
	àqueles que possuem nível de acesso 4 ou superior, para
	atribuírem o status a eles mesmos.

CHAN_HELP_DEHALFOP
	Sintaxe: DEHALFOP [#canal [nick]]
	
	Remove o status de HalfOP do nick dado em um determinado
	canal. Se não for fornecido um nick, o status será removido
	de você mesmo. Se nem canal nem nick forem fornecidos, o
	status será removido de você mesmo em todos os canais onde
	estiver, caso tenha nível suficiente para usar o comando em
	cada um.
	
	Por padrão, é limitado aos AOPs ou àqueles que possuem
	nível de acesso 5 ou superior no canal; ou aos HOPs ou
	àqueles que possuem nível de acesso 4 ou superior, para
	atribuírem o status a eles mesmos.

CHAN_HELP_PROTECT
	Sintaxe: PROTECT [#canal [nick]]
	
	Protege um determinado nick no canal. Se nick não for
	fornecido, isso irá lhe proteger. Sem nem canal nem nick
	forem fornecidos, irá lhe proteger em todos os canais em que
	você estiver, caso você tenha nível suficiente para usar o
	comando em cada um.
	
	Por padrão, limitado ao Founder, a SOPs ou àqueles com nível
	10 ou superior no canal para se auto-protegerem.

CHAN_HELP_DEPROTECT
	Sintaxe: DEPROTECT [#canal [nick]]
	
	Desprotege um determinado nick no canal. Se nick não for
	fornecido, isso irá lhe desproteger. Sem nem canal nem nick
	forem fornecidos, irá lhe desproteger em todos os canais em que
	você estiver, caso você tenha nível suficiente para usar o
	comando em cada um.
	
	Por padrão, limitado ao Founder, a SOPs ou àqueles com nível
	10 ou superior no canal para se auto-desprotegerem.

CHAN_HELP_OWNER
	Sintaxe: OWNER [#canal]
	
	Te concede status de owner no canal. Isso irá lhe
	proteger inclusive daqueles que possuem status "Protect".
	Se canal não for fornecido, irá lhe dar status de owner
	em todos os canais em que você estiver, caso você tenha
	nível suficiente para usar o comando em cada um.
	
	Limitado àqueles que possuem acesso de Founder no canal.

CHAN_HELP_DEOWNER
	Sintaxe: DEOWNER [#canal]
	
	Remove seu status de owner no canal. Se canal não
	for fornecido, irá remover seu status de owner em todos os
	canais em que você estiver, caso você tenha nível suficiente
	para usar o comando em cada um.
	
	Limitado àqueles que possuem acesso de Founder no canal.

CHAN_HELP_INVITE
	Sintaxe: INVITE #canal
	
	Manda o %S dar um invite em você para o canal dado.

	Por padrão, é limitado aos AOPs ou àqueles que possuem
	nível de acesso 5 ou superior no canal.

CHAN_HELP_UNBAN
	Sintaxe: UNBAN #canal
	
	Diz ao %S para remover todos os bans permitindo que
	você entre em um canal.
	
	Por padrão, é limitado aos AOPs ou àqueles que possuem
	nível de acesso 5 ou superior no canal.

CHAN_HELP_KICK
	Sintaxe: KICK [#canal [nick [motivo]]]
	
	Kicka (expulsa) um nick dado em um determinado canal.
	Se não for fornecido um nick, você mesmo será kickado.
	Se nem canal nem nick forem fornecidos, você mesmo será
	kickado em todos os canais onde estiver, caso tenha nível
	suficiente para usar o comando em cada um.

	Por padrão, é limitado aos AOPs ou àqueles que possuem
	nível de acesso 5 ou superior no canal.

CHAN_HELP_BAN
	Sintaxe: BAN [#canal [nick [motivo]]]
	
	Bane o nick selecionado em um canal. Se o nick não é fornecido,
	irá banir você. Se nem canal nem nick forem fornecidos,
	irá banir você em todos os canais onde você estiver, supondo que
	você tem acesso para isso em cada canal.
	
	Por padrão, limitado a AOPs ou àqueles com nível de acesso 5 ou
	superior no canal.

CHAN_HELP_TOPIC
	Sintaxe: TOPIC canal [tópico]
	
	Manda %S ajustar o tópico do canal para o que foi especificado.
	Se tópico não é fornecido, então um tópico vazio será
	ajustado. Este comando é mais útil se usado junto com a opção
	SET TOPICLOCK. Veja %R%S HELP SET TOPICLOCK
	para mais informação.
	
	Por padrão, limitado àqueles que possuem acesso de Founder no canal.

CHAN_HELP_CLEAR
	Sintaxe: CLEAR canal opção
	
	Diz ao %S para remover certos ajustes de um canal.
	A opção pode ser uma das seguintes:
	
	     MODES    Recoloca todos os modos do canal, isto é, remove
	                os modos i,k,l,m,n,p,s,t e recoloca os que estão
	                travados com MLOCK.
	     BANS     Remove todos os bans do canal.
	     EXCEPTS  Remove todos os excepts do canal.
	     INVITES  Clears all invites on the channel.
	     OPS      Remove o status de Operador de canal (modo +o)
	                de todos os Operadores do canal.
	     HOPS     Remove o status de Half-Operador de canal (modo +h)
	                de todos os Half-Operadores do canal, se suportado.
	     VOICES   Remove o status "voice" (modo +v) de qualquer
	                pessoa que tenha esse modo.
	     USERS    Remove (kicka) todos os usuários do canal.
	
	Por padrão, limitado àqueles que possuem acesso de Founder no canal.
	
CHAN_HELP_GETKEY
	Sintaxe: GETKEY canal
	
	Retorna a key (do modo +k) de um determinado canal. Este comando é
	geralmente usado por bots e/ou scripts, da seguinte forma:
	
	KEY <canal> <key>
	
	key é "NENHUMA KEY" se nenhuma key estiver gravada.
	
CHAN_HELP_SENDPASS
	Sintaxe: SENDPASS canal
	
	Envia a senha de um determinado canal para o fundador
	do mesmo, usando o e-mail gravado no nick dele. Este
	comando é realmente útil quando se trata de senhas
	perdidas.
	
	Pode estar limitado aos Operadores de IRC em
	algumas Redes.
	
	Este comando só funciona se o modo de encriptação
	não estiver ligado.

CHAN_SERVADMIN_HELP
	
	Os Administradores dos Services podem também usar o comando
	DROP mesmo sem se identificarem como donos do canal, e podem
	também ver as listas de acesso, AKICK, e níveis de qualquer
	canal.
	
CHAN_SERVADMIN_HELP_LOGOUT
	Sintaxe: LOGOUT canal [nick]
	
	Este comando faz com que um determinado nick seja des-identificado
	do canal, fazendo com que seja necessário ele se identificar
	novamente para recuperar seu acesso.
	
	Se você for o Founder do canal, pode usar este comando em
	qualquer usuário e em si mesmo.
	
	Se você for um Administrador dos Services, poderá
	desconectar qualquer um de qualquer canal sem precisar
	ser o Founder dos mesmos. Além disso, você pode omitir
	o parâmetro nick; com isso, irá des-identificar todos
	os usuários registrados do canal.

CHAN_SERVADMIN_HELP_DROP
	Sintaxe: DROP canal
	
	Cancela o registro de um canal. Somente Administradores dos
	Services podem cancelar canais que pertencem a outros usuários.

CHAN_SERVADMIN_HELP_SET
	
	Administradores dos Services também podem ajustar a opção 
	NOEXPIRE, fazendo com que canais sejam impedidos de ter 
	seu registro expirado. Services Root Administrators ainda
	podem ajustar opções para qualquer canal de outros usuários.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Sintaxe: SET canal NOEXPIRE {ON | OFF}
	
	Determina se o canal poderá ter seu registro expirado.
	Ajustando para ON, evita que o canal seja desregistrado
	por passar do tempo de expiração.
	
	Limitado aos Administradores dos Services.

CHAN_SERVADMIN_HELP_INFO
	
	Administradores de Services podem usar o comando ALL com qualquer canal.

CHAN_SERVADMIN_HELP_LIST
	Sintaxe: LIST padrão [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
	
	Lista todos os canais registrados que combinam com o padrão fornecido.
	Canais com opção PRIVATE ativada só serão mostrados aos
	Administradores dos Services. Canais com a opção NOEXPIRE ativada
	terão um ! preposto ao nome do canal na lista quando for a pedido
	de um Administrador dos Services.
	
	Se as opções FORBIDDEN, SUSPENDED ou NOEXPIRE são fornecidas, apenas
	canais que são, respectivamente, forbidados, suspendidos ou têm a opção
	NOEXPIRE ativa, serão exibidos. Se todas as três opções forem fornecidas,
	todos os três tipos de canais serão exibidos. Estas opções são limitas
	aos Administradores dos Services.

CHAN_SERVADMIN_HELP_GETPASS
	Sintaxe: GETPASS canal
	
	Retorna a senha do canal fornecido. Observe que sempre
	que este comando for usado, uma mensagem incluindo o nick da
	pessoa que emitiu o comando e o canal que foi dado serão
	gravados e enviados para WALLOPS/GLOBOPS.
	
	Limitado aos Administradores dos Services.

CHAN_SERVADMIN_HELP_FORBID
	Sintaxe: FORBID canal [motivo]
	
	Proibe que qualquer pessoa registre e utilize o canal
	dado. Pode ser cancelado com o desregistro do canal.
	
	Um motivo pode ser necessário em algumas redes.
	
	Limitado aos Administradores dos Services.

CHAN_SERVADMIN_HELP_SUSPEND
	Sintaxe: SUSPEND canal [motivo]
	
	Proibe qualquer um de registrar ou usar o canal dado.
	Pode ser cancelado usando o comando UNSUSPEND para
	preservar todos os dados/configurações anteriores do canal.
	
	Um motivo pode ser necessário em algumas redes.
	
	Limitado aos Administradores dos Services.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Sintaxe: UNSUSPEND canal
	
	Libera um canal suspenso. Todos os dados e configurações
	são preservados de antes da suspensão.
	
	Limitado aos Administradores dos Services.

CHAN_SERVADMIN_HELP_STATUS
	Sintaxe: STATUS canal nick
	
	Retorna o nível de acesso atual de um determinado nick em um
	determinado canal. A resposta será no formato:
	
	    STATUS canal nick nível-de-acesso
	
	Se um erro ocorrer, a resposta será no formato:
	
	    STATUS ERROR mensagem-de-erro
	
	Limitado aos Administradores dos Services.

###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP_CMD_SEND
	    SEND     Envia uma mensagem para um nick ou para um canal
MEMO_HELP_CMD_CANCEL
	    CANCEL   Cancela o último memo enviado por você
MEMO_HELP_CMD_LIST
	    LIST     Lista suas mensagens
MEMO_HELP_CMD_READ
	    READ     Lê a(s) mensagem(ns)
MEMO_HELP_CMD_DEL
	    DEL      Apaga a(s) mensagem(ns)
MEMO_HELP_CMD_SET
	    SET      Ajusta opções relacionadas às mensagens
MEMO_HELP_CMD_INFO
	    INFO     Exibe informações sobre suas mensagens
MEMO_HELP_CMD_RSEND
	    RSEND    Envia um memo e pede uma confirmação de leitura
MEMO_HELP_CMD_CHECK
	    CHECK    Verifica se o último memo enviado para um nick foi lido
MEMO_HELP_CMD_SENDALL
	    SENDALL  Envia um memo para todos os usuários registrados
MEMO_HELP_CMD_STAFF
	    STAFF    Envia um memo para todos os opers/admins

MEMO_HELP_HEADER
	%S é um serviço que permite aos usuários de IRC
	enviar curtas mensagens a outros usuários, mesmo
	que eles estejam conectados no momento ou não; ou
	a canais(*). Ambos, destinatários e remetentes,
	devem possuir seus nicks (ou canais) registrados para
	que possam enviar uma mensagem (memo).
	Os comandos do %S são:
	
	
MEMO_HELP_ADMIN
	Não usado.
MEMO_HELP_OPER
	Não usado.
MEMO_HELP_FOOTER
	
	Digite %R%S HELP comando para ajuda em qualquer um
	dos comandos acima.
	
	(*) Por padrão, qualquer usuário com pelo menos nível 10 de acesso
	    em um canal pode ler os memos do canal. Isso pode ser mudado
	    com o comando %s LEVELS.

MEMO_HELP_SEND
	Sintaxe: SEND {nick | canal} mensagem
	
	Envia um memo ao nick ou ao canal determinado contendo
	a mensagem. Quando enviada para o destinatário, este
	receberá um aviso que possui uma nova mensagem. O nick/canal
	de destino deverá estar registrado.
	
MEMO_HELP_CANCEL
	Sintaxe: CANCEL {nick | canal}
	
	Cancela a última mensagem enviada para um determinado nick ou
	canal, caso ela ainda não tenha sido lida no momento em que este
	comando for usado.

MEMO_HELP_LIST
	Sintaxe: LIST [canal] [list | NEW]
	
	Lista quaisquer mensagens que você tiver. Com NEW, lista apenas
	as novas mensagens (não lidas). Mensagens não lidas são marcadas
	com "*" à esquerda do número. Você também pode especificar uma
	lista de números, como no exemplo abaixo:
	
	LIST 2-5,7-9
	   Lista mensagens numeradas de 2 a 5 e de 7 a 9.

MEMO_HELP_READ
	Sintaxe: READ [canal] {núm | list | LAST | NEW}
	
	Mostra as mensagens especificadas. Se LAST é fornecido, envia
	a você a mensagem recebida mais recentemente. Se NEW é fornecido,
	envia a você todas as suas novas mensagens. Caso contrário, envia
	a você a mensagem de número núm. Você também pode fornecer uma
	lista de números, como neste exemplo:
	
	READ 2-5,7-9
	   Mostra as mensagens numeradas de 2 a 5 e de 7 a 9.

MEMO_HELP_DEL
	Sintaxe: DEL [canal] {num | list | LAST | ALL}
	
	Apaga a(s) mensagem(ns) especificada(s). Você pode fornecer
	vários números de mensagens ou seqüências de números ao invés
	de um único número, como no segundo exemplo abaixo.

	Se LAST é fornecido, a última mensagem será apagada.
	Se ALL é fornecido, todas as suas mensagens serão apagadas.
	
	Exemplos:
	
	   DEL 1
	      Apaga sua primeira mensagem.
	
	   DEL 2-5,7-9
	      Apaga mensagens numeradas de 2 a 5 e de 7 a 9.

MEMO_HELP_SET
	Sintaxe: SET opção parâmetros
	
	Ajusta várias opções de mensagens. A opção pode ser uma dessas:
	
	    NOTIFY      Altera quando você será notificado sobre novas
	                    mensagens (só para nicks)
	    LIMIT       Ajusta o número máximo de mensagens que você
	                    pode receber.
	
	Digite %R%S HELP SET opção, para mais informações sobre
	uma opção específica.

MEMO_HELP_SET_NOTIFY
	Sintaxe: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
	
	Altera quando você será notificado sobre novas mensagens:
	
	    ON      Você será notificado sobre mensagens quando você
	                conectar, quando voltar de um /AWAY e quando
	                são enviadas a você.
	    LOGON   Você só será notificado sobre mensagens quando você
	                conectar ou quando voltar de um /AWAY.
	    NEW     Você só será notificado sobre mensagens quando elas
	                forem enviadas a você.
	    MAIL    You will be notified of memos by email aswell as
	                any other settings you have.
	    NOMAIL  You will not be notified of memos by email.
	    OFF     Você não receberá nenhuma notificação sobre mensagens.
	
	ON é essencialmente a combinação de LOGON e NEW.

MEMO_HELP_SET_LIMIT
	Sintaxe: SET LIMIT [canal] limite
	
	Ajusta o número máximo de mensagens que você (ou o canal fornecido)
	podem ter. Se você ajustar para 0, ninguém será capaz de enviar
	mensagens para você. Entretanto, você não pode ajustar este
	limite para mais que %d.

MEMO_HELP_INFO
	Sintaxe: INFO [canal]
	
	Informa o número de mensagens que você tem, quantas
	mensagens não foram lidas, e quantas mensagens no total
	você pode receber. Com o parâmetro, mostra a mesma
	informação para o canal fornecido.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Sintaxe: SET LIMIT [usuário | canal] {limite | NONE} [HARD]
	
	Ajusta o número máximo de mensagens que um usuário ou canal podem
	ter. Ajustando o limite para 0, previne o usuário de receber
	qualquer mensagem; ajustando o limite em NONE permite que o
	usuário receba e mantenha quantas mensagens quiser. Se você
	não fornecer o nick ou canal, seu próprio limite será ajustado.
	
	Adicionando HARD previne que o usuário mude o limite.
	Não adicionando HARD tem o efeito contrário, permitindo
	que o usuário mude o limite (mesmo se um limite anterior
	foi ajustado com HARD).
	
	Este uso do comando SET LIMIT é limitado aos Administradores
	dos Services. Outros usuários somente ajustarão seus
	próprios limites ou de um canal onde tenham privilégios
	para isso, não podem remover seu próprio limite, não
	podem ajustar o limite acima de %d, e não podem ajustar
	um limite máximo (HARD).

MEMO_SERVADMIN_HELP_INFO
	Sintaxe: INFO [nick | canal]
	
	Sem um parâmetro, informa o número de mensagens que você
	tem, quantas mensagens ainda não foram lidas, e quantas
	mensagens no total você pode receber.
	
	Com o parâmetro do canal, mostra a mesma informação para
	o canal fornecido.
	
	Com o parâmetro de nick, mostra a mesma informação para
	o nick fornecido. Esta derivação do comando é limitada aos
	Administradores dos Services.

MEMO_HELP_STAFF
	Sintaxe: STAFF mensagem
	
	Envia um memo a todos os membros da Staff dos Services
	contendo uma mensagem.
	Nota: Se você tem acesso em ambas as listas de Oper e Admin
	irá receber o mesmo memo duas vezes. O mesmo se aplica a
	Operadores que estejam na lista de Root como em outra(s) também.

MEMO_HELP_SENDALL
	Sintaxe: SENDALL mensagem
	
	Envia um memo para todos os usuários registrados
	contendo uma mensagem.

MEMO_HELP_RSEND
	Sintaxe: RSEND {nick | canal} mensagem
	
	Envia para um nick ou canal um memo contendo uma
	mensagem. Quando enviar a um nick, o destinatário receberá
	um aviso de que ele tem um novo memo. O destino (nick/canal)
	deve ser registrado.
	Uma vez que o memo for lido pelo destinatário, uma notificação
	automática será enviada ao remetente informando-o que o memo
	foi lido.

MEMO_HELP_CHECK
	Sintaxe: CHECK nick
	
	Verifica se o último memo que você enviou para nick
	foi lido ou não. Observe que isso só funciona com nicks,
	nunca com canais.

###########################################################################
#
# OperServ help messages
#
###########################################################################

OPER_HELP_CMD_GLOBAL
	    GLOBAL      Envia uma mensagem para todos usuários
OPER_HELP_CMD_STATS
	    STATS       Mostra estatísticas dos Services e da Rede
OPER_HELP_CMD_OPER
	    OPER        Modifica a lista de Operadores dos Services
OPER_HELP_CMD_ADMIN
	    ADMIN       Modifica lista de Administradores dos Services
OPER_HELP_CMD_STAFF
	    STAFF       Exibe os membros da Staff dos Services e seu status online
OPER_HELP_CMD_MODE
	    MODE        Muda o modo de um canal
OPER_HELP_CMD_KICK
	    KICK        Kicka um usuário de um canal
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  Limpa os modos de um canal
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  Derruba todos os usuários de um mesmo host
OPER_HELP_CMD_AKILL
	    AKILL       Manipula a lista de AKILL
OPER_HELP_CMD_SGLINE
	    SGLINE      Manipula a lista de SGLINE
OPER_HELP_CMD_SQLINE
	    SQLINE      Manipula a lista de SQLINE
OPER_HELP_CMD_SZLINE
	    SZLINE      Manipula a lista de SZLINE
OPER_HELP_CMD_CHANLIST
	    CHANLIST    Lista todos os canais em uso na Rede
OPER_HELP_CMD_USERLIST
	    USERLIST    Lista todos os usuários online na Rede
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   Define mensagens a serem mostradas na conexão
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  Define mensagens a serem mostradas ao azar na conexão
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    Define mensagens a serem mostradas a quem pega Oper
OPER_HELP_CMD_SESSION
	    SESSION     Mostra a lista de sessões de host
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   Modifica a lista de limite de sessões
OPER_HELP_CMD_NOOP
	    NOOP        Remove temporariamente todas as O:lines de um 
	                  servidor remotamente
OPER_HELP_CMD_JUPE
	    JUPE        Jupa um servidor
OPER_HELP_CMD_IGNORE
	    IGNORE      Modifica a lista de ignorados dos Services
OPER_HELP_CMD_SET
	    SET         Configura várias opções globais dos Services
OPER_HELP_CMD_RELOAD
	    RELOAD      Recarrega o arquivo de configuração dos Services
OPER_HELP_CMD_UPDATE
	    UPDATE      Salva os dados atuais dos Services
OPER_HELP_CMD_RESTART
	    RESTART     Salva os dados e reinicia os Services
OPER_HELP_CMD_QUIT
	    QUIT        Termina os Services sem salvar os dados
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    Termina os Services salvando os dados
OPER_HELP_CMD_DEFCON
	    DEFCON      Manipula o sistema DefCon
OPER_HELP_CMD_CHANKILL
	    CHANKILL    AKILL todos os usuários de um canal específico
OPER_HELP_CMD_OLINE
	    OLINE       Dá flags de Operador a um determinado usuário
OPER_HELP_CMD_UMODE
	    UMODE       Altera os modos de um usuário
OPER_HELP_CMD_SVSNICK
	    SVSNICK     Força a mudança do nick de um usuário
OPER_HELP_CMD_MODLOAD
	    MODLOAD     Carrega um módulo
OPER_HELP_CMD_MODUNLOAD
	    MODUNLOAD   Descarrega um módulo
OPER_HELP_CMD_MODINFO
	    MODINFO     Informação sobre um módulo carregado
OPER_HELP_CMD_MODLIST
	    MODLIST     Lista os módulos carregados

OPER_HELP
	Comandos do %S:

OPER_HELP_LOGGED
	Aviso: Todos os comandos enviados para o %S são gravados!

OPER_HELP_GLOBAL
	Sintaxe: GLOBAL mensagens
	
	Permite que os IRCops enviem mensagens para todos os usuários
	da Rede. A mensagem será enviada pelo nick %s.

OPER_HELP_STATS
	Sintaxe: STATS [AKILL | ALL | RESET]
	
	Sem nenhuma opção, mostra o número atual de usuários e
	IRCops online (excluindo Services), o número máximo de
	usuários online conectados simultaneamente desde que os
	Services foram iniciados na rede, e o tempo pelo qual os
	Services estão rodando sem interrupções.
	
	Com a opção AKILL, será mostrado o tamanho atual da
	listas de AKILL e SQLINE e o tempo padrão para um AKILL
	ou SQLINE expirar.
	
	A opção ALL está disponível apenas para os Administradores
	dos Services, e mostra informações sobre o consumo de memória
	dos Services. O uso deste comando pode congelar os Services
	por um curto período de tempo em redes grandes, então não
	abuse deste comando!
	
	A opção RESET reinicia a contagem do máximo de usuários
	para o número de usuários atualmente conectados na rede.
	
	UPTIME pode ser usado como um sinônimo para STATS.

OPER_HELP_OPER
	Sintaxe: OPER ADD nick
	         OPER DEL {nick | entrada | lista-entradas}
	         OPER LIST [máscara | lista-entradas]
	         OPER CLEAR
	
	Permite aos Root dos Services adicionar ou remover
	nicks da lista de Operadores dos Services. O usuário que
	tem o nick na lista de Operadores dos Services e que está
	identificado no %s é capaz de acessar os comandos de
	Operador dos Services.
	
	O comando OPER ADD adiciona o nick fornecido na lista
	de Operadores dos Services.
	
	O comando OPER DEL remove o nick especificado da lista
	de Operadores dos Services. Se uma lista de números de entrada
	é fornecida, essas entradas são apagadas. (Veja o exemplo
	para LIST abaixo.)
	
	O comando OPER LIST mostra a lista de Operadores dos
	Services. Se uma máscara for fornecida, somente aquelas
	entradas que combinarem com o padrão especificado na
	máscara serão mostradas. Se uma lista de números de entrada
	é fornecida, somente essas entradas serão mostradas;
	por exemplo:
	
	  OPER LIST 2-5,7-9
	     Lista as entradas de Operadores dos Services numeradas
	     de 2 a 5 e de 7 a 9.
	
	O comando OPER CLEAR limpa todas as entradas da lista
	de Operadores dos Services.
	
	Qualquer Operador de IRC pode usar o OPER LIST.

OPER_HELP_ADMIN
	Sintaxe: ADMIN ADD nick
	         ADMIN DEL {nick | entrada | lista-entradas}
	         ADMIN LIST [máscara | lista-entradas]
	         ADMIN CLEAR
	
	Permite aos Service Root Administrators adicionar ou remover
	nicks da lista de Administradores dos Services. O usuário que
	tem o nick na lista de Administradores dos Services e que está
	identificado no %s é capaz de acessar os comandos de
	Administradores dos Services.
	
	O comando ADMIN ADD adiciona o nick fornecido na lista
	de Administradores dos Services.
	
	O comando ADMIN DEL remove o nick especificado da lista
	de Administradores dos Services. Se uma lista de números de
	entrada é fornecida, essas entradas são apagadas.
	(Veja o exemplo para LIST abaixo.)
	
	O comando ADMIN LIST mostra a lista de Administradores
	dos Services. Se uma máscara for fornecida, somente aquelas
	entradas que combinarem com o padrão especificado na máscara
	serão mostradas. Se uma lista de números de entrada é fornecida,
	somente essas entradas serão mostradas; por exemplo:
	
	  ADMIN LIST 2-5,7-9
	     Lista as entradas de Administradores dos Services numeradas
	     de 2 a 5 e de 7 a 9.
	
	O comando ADMIN CLEAR limpa todas as entradas da lista
	de Administradores dos Services.
	
	Qualquer Operador de IRC pode usar o ADMIN LIST.
	Todos os outros usos do comando são limitados aos
	Service Root Administrators.

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
	Sintaxe: MODE canal modos
	
	Permite que os Operadores dos Services ajustem os modos de
	qualquer canal. Os parâmetros são os mesmos do comando
	/MODE padrão.
	
	Limitado aos Operadores dos Services.

OPER_HELP_UMODE
	Sintaxe: UMODE usuário modos
	
	Permite aos Super Admins ajustar os
	modos de usuário para qualquer usuário. Os parâmetros
	são os mesmos do comando /MODE padrão.

OPER_HELP_OLINE
	Sintaxe: OLINE usuário flags
	
	Permite aos Super Admins dar flags de Oper
	a qualquer usuário. Flags devem ser prefixadas com "+" ou
	"-". Para remover todas as flags apenas digite um "-" em
	vez de usar alguma flag.

OPER_HELP_CLEARMODES
	Sintaxe: CLEARMODES canal [ALL]
	
	Limpa todos os modos binários (i,k,l,m,n,p,s,t) e bans de um
	canal. Se ALL é fornecido, limpa também todos os OPs e
	voices (modos +o e +v) de um canal.
	
	Limitado aos Operadores dos Services.

OPER_HELP_KICK
	Sintaxe: KICK canal usuário motivo
	
	Permite IRCops kickar um usuário de um canal.
	Os parâmetros são os mesmos do comando /KICK padrão.
	A mensagem do kick tem o nick do IRCop que enviou o
	comando KICK, por exemplo:
	
	*** Spam foi kickado do canal #canal pelo %S (Operador (Flood))
	
	Limitado aos Operadores dos Services.

OPER_HELP_SVSNICK
	Sintaxe: SVSNICK nick novo-nick
	
	Força a mudança do nick de um usuário, de nick a novo-nick.
	Limitado aos Super-Admins.

OPER_HELP_AKILL
	Sintaxe: AKILL ADD [+tempo] máscara motivo
	         AKILL DEL {máscara | entrada | lista-entradas}
	         AKILL LIST [máscara | lista-entradas]
	         AKILL VIEW [máscara | lista-entradas]
	         AKILL CLEAR
	
	Permite que Operadores dos Services manipulem a lista de
	AKILL. Se um usuário que tenta conectar possui uma máscara
	prevista na lista de AKILL, os Services emitem um KILL para
	aquele usuário, em servidores que suportam, e instrui todos
	os servidores da rede a adicionar um ban (K-line) para a
	máscara do usuário encontrado.
	
	AKILL ADD adiciona uma máscara ident@host na lista de AKILL
	para um dado motivo (deve ser fornecido).
	Opcionalmente, um tempo para o akill expirar pode ser
	fornecido. O tempo precede a máscara ident@host, e é
	especificado como um número inteiro seguido por uma das
	seguintes unidades: d (dias), h (horas) ou m (minutos).
	Combinações (como 1h30m) não são permitidas. Se uma unidade
	específica não é incluída, o padrão é em dias (assim, +30
	significa 30 dias). Para adicionar um AKILL que não expira,
	use +0. Se a máscara a ser adicionada começar com um +,
	o tempo do AKILL deve ser fornecido, mesmo se esse tempo for igual
	ao padrão. O tempo atual de expiração de AKILLs pode ser encontrado
	com o comando STATS AKILL.
	
	AKILL DEL remove uma dada máscara da lista de AKILL, se ela
	estiver presente. Se uma lista de números de entrada for fornecida,
	essas entradas serão apagadas. (Veja o exemplo para LIST abaixo.)
	
	AKILL LIST mostra a lista de AKILL atual.
	Se uma máscara for fornecida, apenas as entradas que combinarem
	com a máscara serão exibidas. Se uma lista de números de entrada
	for fornecida, apenas essas entradas serão exibidas; por exemplo:
	
	   AKILL LIST 2-5,7-9
	      Lista as entradas de AKILL numeradas de 2 a 5 e de 7 a 9.
	
	AKILL VIEW é uma versão mais detalhada do AKILL LIST,
	e mostrará quem adicionou o AKILL, a data em que foi adicionado,
	e quando irá expirar, como também a máscara (ident@host) e o motivo.
	
	AKILL CLEAR apaga todas as entradas da lista de AKILL.
	
	Limitado aos Operadores dos Services.
	
OPER_HELP_SGLINE
	Sintaxe: SGLINE ADD [+tempo] máscara:motivo
	         SGLINE DEL {máscara | entrada | lista-entradas}
	         SGLINE LIST [máscara | lista-entradas]
	         SGLINE VIEW [máscara | lista-entradas]
	         SGLINE CLEAR
	
	Permite que Operadores dos Services manipulem a lista de
	SGLINE. Se um usuário que tenta conectar possui um realname
	previsto na lista de SGLINE, os Services não permitirão que
	ele continue sua sessão de IRC.
	
	SGLINE ADD adiciona uma máscara realname na lista de SGLINE
	para um dado motivo (deve ser fornecido).
	Opcionalmente, um tempo pode ser fornecido. O tempo precede a
	máscara e é especificado como um número inteiro seguido por uma das
	seguintes unidades: d (dias), h (horas) ou m (minutos).
	Combinações (como 1h30m) não são permitidas. Se uma unidade
	específica não é incluída, o padrão é em dias (assim, +30
	significa 30 dias). Para adicionar um SGLINE que não expira,
	use +0. Se a máscara de realname a ser adicionada começar com
	um +, o tempo do SGLINE deve ser fornecido, mesmo se esse tempo
	for igual ao padrão. O tempo atual de expiração de SGLINE pode ser
	encontrado com o comando STATS AKILL.
	Nota: devido ao fato da máscara de realname poder conter espaços,
	o separador entre ela e o motivo é um sinal de dois-pontos.
	
	SGLINE DEL remove uma dada máscara da lista de SGLINE, se ela
	estiver presente. Se uma lista de números de entrada for fornecida,
	essas entradas serão apagadas. (Veja o exemplo para LIST abaixo.)
	
	SGLINE LIST mostra a lista de SGLINE atual.
	Se uma máscara for fornecida, apenas as entradas que combinarem
	com a máscara serão exibidas. Se uma lista de números de entrada
	for fornecida, apenas essas entradas serão exibidas; por exemplo:
	
	   SGLINE LIST 2-5,7-9
	      Lista as entradas de SGLINE numeradas de 2 a 5 e de 7 a 9.
	
	SGLINE VIEW é uma versão mais detalhada do SGLINE LIST,
	e mostrará quem adicionou o SGLINE, a data em que foi adicionado,
	e quando irá expirar, como também a máscara realname e o motivo.
	
	SGLINE CLEAR apaga todas as entradas da lista de SGLINE.
	
	Limitado aos Operadores dos Services.

OPER_HELP_SQLINE
	Sintaxe: SQLINE ADD [+tempo] máscara motivo
	         SQLINE DEL {máscara | entrada | lista-entradas}
	         SQLINE LIST [máscara | lista-entradas]
	         SQLINE VIEW [máscara | lista-entradas]
	         SQLINE CLEAR
	
	Permite que Operadores dos Services manipulem a lista de
	SQLINE. Se um usuário que tenta conectar possui um nick
	previsto na lista de SQLINE, os Services não permitirão que
	ele continue sua sessão de IRC.
	
	SQLINE ADD adiciona uma máscara na lista de SQLINE
	para um dado motivo (deve ser fornecido).
	Opcionalmente, um tempo pode ser fornecido. O tempo precede a
	máscara e é especificado como um número inteiro seguido por uma das
	seguintes unidades: d (dias), h (horas) ou m (minutos).
	Combinações (como 1h30m) não são permitidas. Se uma unidade
	específica não é incluída, o padrão é em dias (assim, +30
	significa 30 dias). Para adicionar um SQLINE que não expira,
	use +0. Se a máscara a ser adicionada começar com um +,
	o tempo do SQLINE deve ser fornecido, mesmo se esse tempo for
	igual ao padrão. O tempo atual de expiração de SQLINE pode ser
	encontrado com o comando STATS AKILL.
	
	SQLINE DEL remove uma dada máscara da lista de SQLINE, se ela
	estiver presente. Se uma lista de números de entrada for fornecida,
	essas entradas serão apagadas. (Veja o exemplo para LIST abaixo.)
	
	SQLINE LIST mostra a lista de SQLINE atual.
	Se uma máscara for fornecida, apenas as entradas que combinarem
	com a máscara serão exibidas. Se uma lista de números de entrada
	for fornecida, apenas essas entradas serão exibidas; por exemplo:
	
	   SQLINE LIST 2-5,7-9
	      Lista as entradas de SQLINE numeradas de 2 a 5 e de 7 a 9.
	
	SQLINE VIEW é uma versão mais detalhada do SQLINE LIST,
	e mostrará quem adicionou o SQLINE, a data em que foi adicionado,
	e quando irá expirar, como também a máscara e o motivo.
	
	SQLINE CLEAR apaga todas as entradas da lista de SQLINE.
	
	Limitado aos Operadores dos Services.

OPER_HELP_SZLINE
	Sintaxe: SZLINE ADD [+tempo] máscara motivo
	         SZLINE DEL {máscara | entrada | lista-entradas}
	         SZLINE LIST [máscara | lista-entradas]
	         SZLINE VIEW [máscara | lista-entradas]
	         SZLINE CLEAR
	
	Permite que Operadores dos Services manipulem a lista de
	SZLINE. Se um usuário que tenta conectar possui um IP
	previsto na lista de SZLINE, os Services não permitirão que
	ele continue sua sessão de IRC (e isso, mesmo se o IP possuir
	ou não um PTR RR).
	
	SZLINE ADD adiciona uma máscara de IP na lista de SZLINE
	para um dado motivo (deve ser fornecido).
	Opcionalmente, um tempo pode ser fornecido. O tempo precede a
	máscara e é especificado como um número inteiro seguido por uma das
	seguintes unidades: d (dias), h (horas) ou m (minutos).
	Combinações (como 1h30m) não são permitidas. Se uma unidade
	específica não é incluída, o padrão é em dias (assim, +30
	significa 30 dias). Para adicionar um SZLINE que não expira,
	use +0. Se a máscara a ser adicionada começar com um +,
	o tempo do SZLINE deve ser fornecido, mesmo se esse tempo for
	igual ao padrão. O tempo atual de expiração de SZLINE pode ser
	encontrado com o comando STATS AKILL.
	
	SZLINE DEL remove uma dada máscara da lista de SZLINE, se ela
	estiver presente. Se uma lista de números de entrada for fornecida,
	essas entradas serão apagadas. (Veja o exemplo para LIST abaixo.)
	
	SZLINE LIST mostra a lista de SZLINE atual.
	Se uma máscara for fornecida, apenas as entradas que combinarem
	com a máscara serão exibidas. Se uma lista de números de entrada
	for fornecida, apenas essas entradas serão exibidas; por exemplo:
	
	   SZLINE LIST 2-5,7-9
	      Lista as entradas de SZLINE numeradas de 2 a 5 e de 7 a 9.
	
	SZLINE VIEW é uma versão mais detalhada do SZLINE LIST,
	e mostrará quem adicionou o SZLINE, a data em que foi adicionado,
	e quando irá expirar, como também a máscara IP e o motivo.
	
	SZLINE CLEAR apaga todas as entradas da lista de SZLINE.
	
	Limitado aos Operadores dos Services.

OPER_HELP_SET
	Sintaxe: SET opção seleção
	
	Ajusta várias opções globais dos Services. Os nomes de opção
	atualmente definidas são:
	    READONLY    Ativa o modo read-only ou read-write
	    LOGCHAN     Ativa ou desativa envio de log a um canal
	    DEBUG       Ativa ou desativa o modo de depuração (debug)
	    NOEXPIRE    Ativa ou desativa o modo não-expira
	    SUPERADMIN  Ativa ou desativa o modo Super-Admin
	    SQL         Ativa ou desativa o modo SQL
	    IGNORE      Ativa ou desativa o modo Ignore
	    LIST        Lista as opções atuais
	
	Limitado aos Administradores dos Services.

OPER_HELP_SET_READONLY
	Sintaxe: SET READONLY {ON | OFF}
	
	Ativa ou desativa o modo read-only (somente leitura).
	No modo read-only, usuários normais não serão capazes de
	modificar nenhum dado nos Services, incluindo lista de acesso
	de canais e nicks etc. IRCops com privilégios suficientes
	nos Services serão capazes de modificar a lista de AKILL
	e de proibir ou cancelar o registro de canais ou nicks,
	embora quaisquer mudanças não serão salvas no banco de
	dados a menos que o modo read-only seja desativado antes
	dos Services serem terminados ou reiniciados.
	
	Esta opção é equivalente à opção da linha de comando
	-readonly.

OPER_HELP_SET_LOGCHAN
	Sintaxe: SET LOGCHAN {ON | OFF}
	
	Com esta opção ativada, os Services enviarão seus logs
	para um canal especificado. LogChannel deve estar definido
	na configuração dos Services para esta opção poder ser usada.
	
	This option is equivalent to the command-line option -logchan.
	
	Nota: Pode haver graves implicações de segurança se o canal
	de log não estive devidamente seguro.

OPER_HELP_SET_DEBUG
	Sintaxe: SET DEBUG {ON | OFF | número}
	
	Ativa ou desativa o modo de depuração (debug). No modo de
	depuração, todos os dados enviados para os Services e vindos
	deles, assim como um número de outras mensagens de depuração
	são enviadas a um arquivo de log. Se número é fornecido,
	o modo de depuração é ativado, com nível de depuração ajustado
	para número.
	
	Esta opção é equivalente à opção da linha de comando
	-debug.
	
OPER_HELP_SET_NOEXPIRE
	Sintaxe: SET NOEXPIRE {ON | OFF}
	
	Ativa ou desativa o modo NOEXPIRE. Se estiver ativado, os
	nicks, canais, akills e exceções não irão expirar enquanto a 
	opção não for desativada.
	
	Esta opção é equivalente à opção da linha de comando
	-noexpire.

OPER_HELP_SET_SUPERADMIN
	Sintaxe: SET SUPERADMIN {ON | OFF}
	
	Ativando esta opção, lhe serão concedidos privilégios extras como a
	habilidade de ser "founder" em todos os canais etc.

	Esta opção não é permanente, e deve ser usada somente quando
	necessária, e desativada quando não for mais necessária.

OPER_HELP_SET_SQL
	Sintaxe: SET SQL {ON | OFF}
	
	Ajustando esta opção, fará os Services usarem ou não SQL. Isso deve
	ser usado para ativar ou desativar SQL caso seu servidor SQL caia
	enquanto os Services estiverem rodando.

OPER_HELP_SET_IGNORE
	Sintaxe: SET IGNORE {ON | OFF}
	
	Ajustando esta opção, fará os Services usarem ou não o sistema
	de IGNORE.

OPER_HELP_SET_LIST
	Sintaxe: SET LIST
	
	Exibe as várias configurações do %S.

OPER_HELP_NOOP
	Sintaxe: NOOP SET servidor
	         NOOP REVOKE servidor
	
	NOOP SET remove todas as O:lines do servidor
	especificado e killa todos os IRCops atualmente nele
	para prevenir que eles dêem rehash no servidor (pois
	isto simplesmente cancela o efeito).

	NOOP REVOKE torna todas as O:lines removidas
	novamente disponíveis no servidor especificado.

	Nota: O servidor não é checado como um todo
	pelos Services.
	
	Limitado aos Administradores dos Services.

OPER_HELP_JUPE
	Sintaxe: JUPE servidor [motivo]
	
	Diz aos Services para "jupar" um servidor -- isto é, criar
	um falso servidor conectado aos Services que previne que o
	servidor real com aquele nome se conecte na rede. O jupe
	pode ser removido usando um SQUIT padrão. Se um motivo
	é dado, ele é colocado no campo de informação do servidor;
	caso contrário, o campo de informação do servidor irá conter
	o texto "Jupado por <nick>", mostrando o nick de quem mascarou
	o servidor.
	
	Limitado aos Administradores dos Services.

OPER_HELP_RAW
	Sintaxe: RAW texto
	
	Envia uma série de texto diretamente ao servidor onde os
	Services estão conectados. Este comando tem várias
	limitações de uso, e pode criar destruição e problemas na
	rede se for usado inadequadamente. NÃO USE ESTE COMANDO
	a menos que você tenha absoluta certeza do que está fazendo!
	
	Limitado aos Administradores dos Services.

OPER_HELP_UPDATE
	Sintaxe: UPDATE
	
	Faz com que os Services atualizem o banco de dados logo que
	você envia este comando.
	
	Limitado aos Administradores dos Services.
	
OPER_HELP_RELOAD
	Sintaxe: RELOAD
	
	Faz com que os Services recarreguem seu arquivo de configuração. 
	Repare que, para algumas mudanças terem efeito, será necessário
	que os Services sejam reiniciados.
	
	Limitado aos Administradores dos Services.

OPER_HELP_QUIT
	Sintaxe: QUIT
	
	Faz com que desligue os Services imediatamente; o banco de
	dados não é salvo. Este comando não deve ser usado a menos
	que haja um dano da cópia do banco de dados na memória e por
	isso eles não devam ser salvos. Para desligar normalmente,
	use o comando SHUTDOWN.
	
	Limitado aos Administradores dos Services.

OPER_HELP_SHUTDOWN
	Sintaxe: SHUTDOWN
	
	Salva o banco de dados dos Services e os desliga.
	
	Limitado aos Administradores dos Services.

OPER_HELP_RESTART
	Sintaxe: RESTART
	
	Salva o banco de dados dos Services e os reinicia.
	(sai e roda imediatamente o executável).
	
	Limitado aos Administradores dos Services.

OPER_HELP_CHANLIST
	Sintaxe: CHANLIST [{padrão | nick} [SECRET]]
	
	Lista todos os canais em uso na Rede, sejam eles registrados ou não.
	
	Se um padrão é fornecido, listará apenas canais que combinem com
	ele. Se um nick é fornecido, listará apenas canais que o usuário
	estiver usando. Se SECRET é especificado, listará apenas canais
	combinando com o padrão e que tenham modo +s ou +p.
	
	Limitado aos Administradores dos Services.
	
OPER_HELP_USERLIST
	Sintaxe: USERLIST [{padrão | canal} [INVISIBLE]]
	
	Lista todos os usuários online na Rede, tenham eles nicks
	registrados ou não.
	
	Se um padrão é fornecido, listará apenas usuários que combinem
	com ele (deve estar no formato nick!ident@host). Se um canal é
	fornecido, listará apenas usuários que estão no canal dado.
	estiver usando. Se INVISIBLE é especificado, listará apenas usuários
	que tenham o modo +i.
	
	Limitado aos Administradores dos Services.
	
OPER_HELP_MODLOAD
	Sintaxe: MODLOAD NomeDoArquivo
	
	Este comando carrega o módulo chamado NomeDoArquivo
	do diretório de módulos.
	
	Limitado aos Service Root Administrators.
	
OPER_HELP_MODUNLOAD
	Sintaxe: MODUNLOAD NomeDoArquivo

	Este comando descarrega o módulo chamado NomeDoArquivo
	do diretório de módulos.

	Limitado aos Service Root Administrators.
	
OPER_HELP_MODINFO
	Sintaxe: MODINFO NomeDoArquivo
	
	Este comando lista informações sobre um módulo carregado.
	
	Limitado aos Service Root Administrators.
	
OPER_HELP_MODLIST
	Sintaxe: MODLIST [Core|3rd|protocol|encryption|supported|qatested]
	
	Lista todos os módulos atualmente carregados.
	
	Limitado aos Service Root Administrators.
	

###########################################################################
#
# BotServ help messages
#
###########################################################################
BOT_HELP_CMD_BOTLIST
	    BOTLIST            Lista os bots disponíveis
BOT_HELP_CMD_ASSIGN
	    ASSIGN             Associa um bot ao canal
BOT_HELP_CMD_SET
	    SET                Configura as opções do bot
BOT_HELP_CMD_KICK
	    KICK               Configura os kickers
BOT_HELP_CMD_BADWORDS
	    BADWORDS           Configura os palavrões
BOT_HELP_CMD_ACT
	    ACT                Faz com que o bot faça o mesmo que o comando "/me"
BOT_HELP_CMD_INFO
	    INFO               Permite que você veja informações do BotServ sobre um canal ou bot
BOT_HELP_CMD_SAY
	    SAY                Faz com que o bot diga determinado texto no canal dado
BOT_HELP_CMD_UNASSIGN
	    UNASSIGN           Desassocia um bot de um canal
BOT_HELP_CMD_BOT
	    BOT                Mantém a lista de bots da Rede

BOT_HELP
	%S permite que você tenha um bot no seu canal.
	Foi criado para usuários que não tem como hospedar
	ou configurar um bot, ou para Redes que não permitem
	aos usuários terem um bot. Os comandos disponíveis 
	estão listados abaixo; para usá-los, digite
	%R%S comando. Para informações mais específicas
	sobre um comando, digite %R%S HELP comando.
	
BOT_HELP_FOOTER
	O bot irá entrar no canal sempre que tiver ao menos %d usuário(s) nele.
	
BOT_HELP_BOTLIST
	Sintaxe: BOTLIST
	
	Lista todos os bots disponíveis nesta Rede.
	
BOT_HELP_ASSIGN
	Sintaxe: ASSIGN canal nick
	
	Associa um bot escolhido pelo nick a um canal.
	Você poderá então configurar o bot para o canal de acordo
	com as suas necessidades.
	
BOT_HELP_UNASSIGN
	Sintaxe: UNASSIGN canal
	
	Desassocia um bot de um canal. Quando você usa este comando,
	o bot não irá entrar mais no canal. Entretando, será mantida
	a configuração do bot para aquele canal, para que você possa
	sempre poder associá-lo àquele canal mais tarde sem ter que
	configurar tudo novamente.
	
BOT_HELP_INFO
	Sintaxe: INFO {canal | nick}
	
	Permite que você veja informações do %S sobre um
	canal ou bot. Se a opção for um canal, então você terá
	informações tais como kickers ativos. Se tiver sido um nick,
	você terá informações sobre o bot, como o horário de criação
	ou número de canais onde ele está.
	
BOT_HELP_SET
	Sintaxe: SET canal opção parâmetros
	
	Configura as opções do bot. A opção pode ser:
	
	    DONTKICKOPS      Protege os OPs contra kicks
	    DONTKICKVOICES   Protege os voices contra kicks
	    GREET            Habilita a mensagem de entrada
	    FANTASY          Habilita os comandos Fantasia
	    SYMBIOSIS        Permite que o bot seja um bot real
	
	Digite %R%S HELP SET opção para maiores informações
	sobre uma opção específica.
	
	Aviso: o acesso a este comando é controlado pelo ajuste
	dos níveis de acesso do canal.
	
BOT_HELP_SET_DONTKICKOPS
	Sintaxe: SET canal DONTKICKOPS {ON|OFF}
	
	Ativa ou desativa o modo de proteção de OPs em um canal.
	Quando estiver ativado, os OPs não serão kickados pelo bot,
	mesmo se eles não se enquadram no nível de NOKICK.
	
BOT_HELP_SET_DONTKICKVOICES
	Sintaxe: SET canal DONTKICKVOICES {ON|OFF}
	
	Ativa ou desativa o modo de proteção de voices em um canal.
	Quando estiver ativado, os voices não serão kickados pelo bot,
	mesmo se eles não se enquadram no nível de NOKICK.
	
BOT_HELP_SET_FANTASY
	Sintaxe: SET canal FANTASY {ON|OFF}
	
	Ativa ou desativa o modo fantasia em um canal.
	Quando estiver ativado, os usuários poderão usar
	comandos como !op, !deop, !voice, !devoice,
	!kick, !kb, !unban, !seen em um canal. Em algumas
	redes, mais comandos podem estar disponíveis, tais
	como: !owner, !deowner, !protect, !deprotect,
	!halfop, !dehalfop.
	
	Note que os usuários que quiserem usar esses 
	comandos DEVEM ter um nível suficiente tanto para
	o nível FANTASIA quanto para outro, dependendo
	do comando usado (por exemplo, para usar !op, o 
	usuário deve ter acesso aos comandos OP e DEOP).

BOT_HELP_SET_GREET
	Sintaxe: SET canal GREET {ON|OFF}
	
	Ativa ou desativa o modo greet em um canal.
	Quando ativado, o bot irá mostrar mensagens de
	entrada dos usuários quando eles entrarem no
	canal, caso tenham nível de acesso suficiente.

BOT_HELP_SET_SYMBIOSIS
	Sintaxe: SET canal SYMBIOSIS {ON|OFF}
	
	Ativa ou desativa o modo symbiosis em um canal.
	Quando ativado, o bot fará tudo aquilo que 
	normalmente é feito pelo %s nos canais, como as 
	mudanças de modos, kicks e, inclusive, a mensagem
	de entrada (entrymsg).

BOT_HELP_KICK
	Sintaxe: KICK canal opção parâmetros
	
	Configura os kicks do bot. A opção pode ser uma dessas:
	
	    BOLDS         Selecione para o bot kickar por negrito
	    BADWORDS      Selecione para o bot kickar por palavrões
	    CAPS          Selecione para o bot kickar por caps
	    COLORS        Selecione para o bot kickar por cores
	    FLOOD         Selecione para o bot kickar por flood
	    REPEAT        Selecione para o bot kickar por repetição
	    REVERSES      Selecione para o bot kickar por reversos
	    UNDERLINES    Selecione para o bot kickar por sublinhado
	
	Digite %R%S HELP KICK opção para maiores informações
	sobre uma opção específica.
	
	Nota: o acesso a esse comando é controlado pelo ajuste
	dos níveis de acesso do canal.
	
BOT_HELP_KICK_BOLDS
	Sintaxe: KICK canal BOLDS {ON|OFF} [ttb]
	
	Ativa ou desativa o kick por negrito. Quando ativado
	o bot irá kickar os usuários que usarem negrito.
	
	ttb é o número de vezes que o usuário será kickado
	antes de ser banido. Não forneça ttb para desabilitar
	o sistema de ban que foi uma vez ativado.
	
BOT_HELP_KICK_COLORS
	Sintaxe: KICK canal COLORS {ON|OFF} [ttb]
	
	Ativa ou desativa o kick por cores. Quando ativado
	o bot irá kickar os usuários que usarem cores.
	
	ttb é o número de vezes que o usuário será kickado
	antes de ser banido. Não forneça ttb para desabilitar
	o sistema de ban que foi uma vez ativado.
	
BOT_HELP_KICK_REVERSES
	Sintaxe: KICK canal REVERSES {ON|OFF} [ttb]
	
	Ativa ou desativa o kick por reverso. Quando ativado
	o bot irá kickar os usuários que usarem reverso.
	
	ttb é o número de vezes que o usuário será kickado
	antes de ser banido. Não forneça ttb para desabilitar
	o sistema de ban que foi uma vez ativado.
	
BOT_HELP_KICK_UNDERLINES
	Sintaxe: KICK canal UNDERLINES {ON|OFF} [ttb]
	
	Ativa ou desativa o kick por sublinhado. Quando ativado
	o bot irá kickar os usuários que usarem sublinhado.
	
	ttb é o número de vezes que o usuário será kickado
	antes de ser banido. Não forneça ttb para desabilitar
	o sistema de ban que foi uma vez ativado.
	
BOT_HELP_KICK_CAPS
	Sintaxe: KICK canal CAPS {ON|OFF} [ttb [mínimo [porcentagem]]]
	
	Ativa ou desativa o kick por caps. Quando ativado
	o bot irá kickar os usuários que usarem CAPS (maiúsculas).
	
	O bot irá kickar apenas se usarem pelo menos um mínimo de
	caps e eles constituírem pelo menos porcentagem%% do total 
	do texto (se não for especificado, o padrão é 10 caracteres
	e 25%%).
	
	ttb é o número de vezes que o usuário será kickado
	antes de ser banido. Não forneça ttb para desabilitar
	o sistema de ban que foi uma vez ativado.
	
BOT_HELP_KICK_FLOOD
	Sintaxe: KICK canal FLOOD {ON|OFF} [ttb [linhas [segs]]]
	
	Ativa ou desativa o kick por flood. Quando ativado o
	o bot irá kickar por flood no canal desde que sejam
	pelo menos linhas linhas em segs segundos.
	(se não for especificado, o padrão é 6 linhas em
	10 segundos).
	
	ttb é o número de vezes que o usuário será kickado
	antes de ser banido. Não forneça ttb para desabilitar
	o sistema de ban que foi uma vez ativado.
	
BOT_HELP_KICK_REPEAT
	Sintaxe: KICK canal REPEAT {ON|OFF} [ttb [núm]]
	
	Ativa ou desativa o kick por repetição. Quando ativado
	o bot irá kickar os usuários que fizerem repetições (onde
	núm é o número de repetições para kick; se não for fornecido,
	o padrão é 3).
	
	ttb é o número de vezes que o usuário será kickado
	antes de ser banido. Não forneça ttb para desabilitar
	o sistema de ban que foi uma vez ativado.
	
BOT_HELP_KICK_BADWORDS
	Sintaxe: KICK canal BADWORDS {ON|OFF} [ttb]
	
	Ativa ou desativa o kick por palavrões. Quando ativado
	o bot irá kickar usuários que disserem determinadas
	palavras no canal.
	
	Você pode definir os palavrões para um canal usando o
	comando BADWORDS. Digite %R%S HELP BADWORDS para
	maiores informações.
	
	ttb é o número de vezes que o usuário será kickado
	antes de ser banido. Não forneça ttb para desabilitar
	o sistema de ban que foi uma vez ativado.
	
BOT_HELP_BADWORDS
	Sintaxe: BADWORDS canal ADD palavra [SINGLE | START | END]
	         BADWORDS canal DEL {palavra | núm-entrada | lista}
	         BADWORDS canal LIST [máscara | lista]
	         BADWORDS canal CLEAR
	
	Faz a manutenção da lista de palavrões em um canal.
	Essa lista determina quais são as palavras que deverão
	ser motivo de kick em um canal quando a opção BADWORDS
	estiver ativada. Para maiores informações, digite
	%R%S HELP KICK BADWORDS.
	
	O comando BADWORDS ADD adiciona a palavra na lista
	de palavrões. A opção SINGLE, quando especificada, só
	kicka o usuário se ele escrever a palavra inteira.
	A opção START, quando especificada, irá kickar o usuário
	que escrever uma palavra que comece com a palavra.
	A opção END se tiver sido especificada só kicka o usuário
	que escrever uma palavra que termine com a palavra.
	Se você não especificar nada, o bot irá kickar sempre
	que a palavra for escrita pelo usuário.
	
	O comando BADWORDS DEL remove uma palavra da lista de
	palavrões de um determinado canal. Se uma lista de números
	de entrada for fornecida, essas entradas serão apagadas
	(veja o exemplo para LIST abaixo).
	
	O comando BADWORDS LIST mostra a lista de palavrões.
	Se forem usados coringas, apenas aquelas entradas com
	as determinadas máscaras serão mostradas. Se uma lista
	de números de entrada for dada, apenas aquelas entradas
	serão mostradas; por exemplo:
	
	   BADWORDS #canal LIST 2-5,7-9
	      Lista as entradas dos palavrões numeradas de 2 a 5
	      e de 7 a 9.
	
	O comando BADWORDS CLEAR limpa todas entradas da lista.

BOT_HELP_SAY
	Sintaxe: SAY canal texto
	
	Faz com que o bot diga determinado texto no canal dado.

BOT_HELP_ACT
	Sintaxe: ACT canal texto
	
	Faz com que o bot faça o mesmo que o comando "/me"
	em um determinado canal com o determinado texto.

BOT_SERVADMIN_HELP_BOT
	Sintaxe: BOT ADD nick user host real
	         BOT CHANGE NickAntigo NickNovo [user [host [real]]]
	         BOT DEL nick
	
	Permite que os Administradores dos Services criem,
	modifiquem, e apaguem bots que os usuários poderão usar
	em seus próprios canais.
	
	BOT ADD adiciona um bot com um determinado nick, username,
	hostname e realname. 
	BOT CHANGE permite alterar o nick, username, hostname
	ou realname de um bot sem que seja necessário apagá-lo (e
	todas as informações nele contidas).
	BOT DEL remove um bot da lista de bots.  
	
	Nota: você não pode criar um bot que use um nick que já
	seja registrado. Se um usuário não registrado está atualmente
	usando o nick, ele será killado.
	
BOT_SERVADMIN_HELP_SET
	
	Essas opções são reservadas aos Administradores dos Services:
	
	    NOBOT            Previne um bot de ser associado a 
	                       um canal
	    PRIVATE          Previne um bot de ser associado por
	                       quem não é Operador de IRC.

BOT_SERVADMIN_HELP_SET_NOBOT
	Sintaxe: SET canal NOBOT {ON|OFF}
	
	Este comando faz com que um determinado bot não possa ser
	associado a um canal. Se já houver um bot associado a este
	canal, este será automaticamente desassociado quando você
	ativar essa opção.
	
	Limitado aos Administradores dos Services.
	
BOT_SERVADMIN_HELP_SET_PRIVATE
	Sintaxe: SET nick PRIVATE {ON|OFF}
	
	Esta opção previne um bot de ser associado a um canal
	por usuários que não sejam Operadores de IRC.
	
	Limitado aos Administradores dos Services.

###########################################################################
#
# HostServ messages
#
###########################################################################
HOST_EMPTY
	A lista de vHost está limpa.
HOST_ENTRY
	#%d Nick:%s, vHost:%s (%s - %s)
HOST_IDENT_ENTRY
	#%d Nick:%s, vHost:%s@%s (%s - %s)
HOST_SET
	vhost de %s ajustado para %s.
HOST_IDENT_SET
	vhost de %s ajustado para %s@%s.
HOST_SETALL
	vhost do grupo %s ajustado para %s.
HOST_DELALL
	vhosts do grupo %s foi removido.
HOST_DELALL_SYNTAX
	DELALL <nick>.
HOST_IDENT_SETALL
	vhost do grupo %s ajustado para %s@%s.
HOST_SET_ERROR
	Erro! Um vhost pode conter apenas A-Z, a-z, 0-9, '.' e '-'.
HOST_SET_IDENT_ERROR
	O Ident de um vhost deve estar no formato de um Ident válido.
HOST_SET_TOOLONG
	Erro! O host é muito comprido, por favor use um host de até %d caracteres.
HOST_SET_IDENTTOOLONG
	Erro! O Ident é muito comprido, por favor use um Ident de até %d caracteres.
HOST_NOREG
	Usuário %s não encontrado na DB do NickServ.
HOST_SET_SYNTAX
	SET <nick> <hostmask>.
HOST_SETALL_SYNTAX
	SETALL <nick> <hostmask>.
HOST_DENIED
	Acesso Negado.
HOST_NOT_ASSIGNED
	Por favor contacte algum Operator para receber um vhost associado a este nick.
HOST_ACTIVATED
	Seu vhost %s está agora ativado.
HOST_IDENT_ACTIVATED
	Seu vhost %s@%s está agora ativado.
HOST_ID
	Por favor, identifique-se aos Services primeiro.
HOST_NOT_REGED
	Primeiro você precisa registrar seu nick senão um vhost não pode ser associado a você.
HOST_DEL
	vhost para %s removido.
HOST_DEL_SYNTAX
	DEL <nick>.
HOST_OFF
	Your vhost was removed and the normal cloaking restored.
HOST_OFF_UNREAL
	Seu vhost foi removido. Para reabilitar a encriptação padrão de host, digite /mode %s +%s
HOST_NO_VIDENT
	Seu IRCD não suporta vIdent's, se isto não está certo, por favor reporte isso como um possível bug.
HOST_GROUP
	Todos os vhosts no grupo %s foram ajustados para %s
HOST_IDENT_GROUP
	Todos os vhosts no grupo %s foram ajustados para %s@%s
HOST_LIST_FOOTER
	Exibidos todos os registros (Contador: %d)
HOST_LIST_RANGE_FOOTER
	Exibidos registros de %d até %d
HOST_LIST_KEY_FOOTER
	Exibidos registros coincidindo com a chave %s (Contador: %d)

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_HELP_CMD_ON
	    ON          Ativa seu vHost associado
HOST_HELP_CMD_OFF
	    OFF         Desativa seu vHost associado
HOST_HELP_CMD_GROUP
	    GROUP       Sincroniza um mesmo vHost para todos os nicks de um grupo
HOST_HELP_CMD_SET
	    SET         Ajusta o vHost de outro usuário
HOST_HELP_CMD_SETALL
	    SETALL      Ajusta o vHost de todos os nicks de um grupo
HOST_HELP_CMD_DEL
	    DEL         Deleta o vHost de outro usuário
HOST_HELP_CMD_DELALL
	    DELALL      Deleta o vHost de todos os nicks de um grupo
HOST_HELP_CMD_LIST
	    LIST        Exibe uma ou mais entradas de vHost

HOST_OPER_HELP
	not used.

HOST_ADMIN_HELP
	not used.

HOST_HELP
	Comandos do %S:

HOST_HELP_ON
	Sintaxe: ON

	Ativa o vhost atualmente associado ao nick em uso.
	Quando você usa este comando, qualquer usuário que faça um /whois
	em você irá ver o vhost em vez do seu endereço IP real/encriptado.

HOST_HELP_SET
	Sintaxe: SET <nick> <hostmask>.

	Ajusta o vhost para o nick dado com a hostmask fornecida.
	Caso seu IRCd suporte vIdents, então usando
	SET <nick> <ident>@<hostmask> ajustará idents para
	usuários, assim como vhosts.

	Limitado a Host Setters/Operadores dos Services.

HOST_HELP_DELALL
	Sintaxe: DELALL <nick>.

	Deleta o vhost para todos os nicks no mesmo grupo do
	nick fornecido.

	Limitado a Host Removers/Operadores dos Servicess.

HOST_HELP_SETALL
	Sintaxe: SETALL <nick> <hostmask>.

	Ajusta o vhost para todos os nicks no mesmo grupo do
	nick fornecido. Caso seu IRCd suporte vIdents, então
	usando SETALL <nick> <ident>@<hostmask> ajustará idents
	para usuários, assim como vhosts.

	* NOTA: isto não irá atualizar o vhost de nenhum nick
	adicionado ao grupo depois que o comando foi usado.

	Limitado a Host Setters/Operadores dos Services.

HOST_HELP_OFF
	Sintaxe: OFF

	Desativa o vhost atualmente associado ao nick em uso.
	Quando você usa este comando, qualquer usuário que faça um /whois
	em você irá ver seu endereço IP real/encriptado.

HOST_HELP_DEL
	Sintaxe: DEL <nick>

	Deleta o vhost associado ao nick dado.

	Limitado a Host Removers/Operadores dos Services.

HOST_HELP_LIST
	Sintaxe: LIST [<key>|<#X-Y>]

	Este comando lista ao Operador os vhosts registrados.
	Se uma Key é especificada, apenas entradas cujo nick ou vhost
	combinem com o padrão fornecido em <key> serão exibidas.
	Ex.: Rob* mostra todas as entradas começando com "Rob"

	Se o estilo #X-Y for usado, apenas entradas entre o alvo X
	e Y serão exibidas. Ex.: #1-3 irá mostrar as 3 primeiras
	entradas de nick/vhost.

	A lista usa o valor de NSListMax como limite do número de
	itens para exibir a um Operator de uma vez só.

	Limitado aos Operadores dos Services.

HOST_HELP_GROUP
	Sintaxe: GROUP
	
	Este comando permite aos usuários ajustar o vhost do seu
	nick ATUAL para ser o vhost de todos os nicks no mesmo grupo.

OPER_SVSNICK_UNSUPPORTED
	Desculpe, SVSNICK não está disponível nesta rede.

OPER_SQLINE_UNSUPPORTED
	Desculpe, SQLINE não está disponível nesta rede.

OPER_SVSO_UNSUPPORTED
	Desculpe, OLINE não está disponível nesta rede.

OPER_UMODE_UNSUPPORTED
	Desculpe, UMODE não está disponível nesta rede.

OPER_SUPER_ADMIN_NOT_ENABLED
	Opção SuperAdmin não habilitada no arquivo services.conf

