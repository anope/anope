# Portuguese language file.
#
# (C) 2003 Anope Team
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
# config.h).  The %S's do not count as sprintf() parameters, so they can be
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
	Sab
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
	Comando desconhecido %s.  "/msg %s HELP" para ajuda.
SYNTAX_ERROR
	Sintaxe: %s
MORE_INFO
	/msg %s HELP %s para maiores informações.
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
	Atenção: Os services estão no modo leitura apenas; as mudanças não serão salvas!
PASSWORD_INCORRECT
	Senha incorreta.

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
	A opção RAW ha sido desabilitada. Se realmente precisar deve habilitar a opção DisableRaw na configuração dos Servicos.

MORE_OBSCURE_PASSWORD
	Por favor tente novamente com uma senha mais difícil. Não use TAB ou espaços.
PASSWORD_TRUNCATED
	Atenção: Sua senha foi reduzida a %d caracteres.

NICK_NOT_REGISTERED
	Seu nick não está registrado.
NICK_NOT_REGISTERED_HELP
	Seu nick não está registrado.  Digite /msg %s HELP para maiores informações sobre como registrar o nick.
NICK_X_IS_SERVICES
	Nick %s is part of this Network's Services.
NICK_X_NOT_REGISTERED
	Nick %s não está registrado.
NICK_X_IN_USE
	Nick %s is currently in use.
NICK_X_NOT_IN_USE
	Nick %s não está sendo usado.
NICK_X_NOT_ON_CHAN
	%s is not currently on channel %s.
NICK_X_FORBIDDEN
	Nick %s não pode ser registrado ou usado.
NICK_X_FORBIDDEN_OPER
	Nick %s foi bloqueado por %s:
	%s
NICK_X_ILLEGAL
	Nick %s is an illegal nickname and cannot be used.
NICK_X_TRUNCATED
	Nick %s was truncated to %d characters.
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
	      Suspended: [%s] %s
NICK_IDENTIFY_REQUIRED
	Identificação de senha requerida para usar este comando.
	Tente novamente após digitar /msg %s IDENTIFY senha.
CHAN_IDENTIFY_REQUIRED
	Identificação de senha requerida para usar este comando.
	Tente novamente após digitar /msg %s IDENTIFY %s senha.
	
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
	End of %s list.

###########################################################################
#
# NickServ messages
#
###########################################################################

# Automatic messages
NICK_IS_REGISTERED
	Este nick foi registrado por outra pessoa.  Por favor escolha outro.
	(Se este for seu nick, digite /msg %s IDENTIFY senha.)
NICK_IS_SECURE
	Este nick está registrado e protegido.  Se este for seu
	nick, digite /msg %s IDENTIFY senha.  Caso contrário,
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
NICK_PASSWORD_IS
	Sua senha é %s - guarde ela para uso posterior.

# GROUP responses
NICK_REG_DELAY
	You have to be connected longer than %d seconds to register your nick.

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
	Seu nick já está registrado; digite /msg %s DROP primeiro.
NICK_GROUP_SAME
	Você já é um membro do grupo %s.
NICK_GROUP_TOO_MANY
	There are too many nicks in %s's group; list them and drop some.
	Type /msg %s HELP GLIST and /msg %s HELP DROP 
	for more information.
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
	Digite /msg %S SET EMAIL e-mail para setar seu e-mail.
	Sua privacidade é respeitada; seu e-mail não será dado
	para terceiros.
	
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
	Seu nick foi desconectado.
NICK_LOGOUT_X_SUCCEEDED
	O nick %s foi desconectado.

# DROP responses
NICK_LOGOUT_SERVICESADMIN
	Can't logout %s because he's a services administrator.

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
NICK_SET_UNKNOWN_OPTION_OR_BAD_NICK
	Opção SET desconhecida: %s, ou nick não registrado.
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
	Número de linguagem desconhecida %d.  Digite /msg %s HELP SET LANGUAGE para uma lista das linguagens disponíveis.
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
	Você não pode remover o e-mail nessa Rede.
	
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
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Seu e-mail está agora escondido do comango %s INFO.
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
	
# SET MSG responses
NICK_SET_HIDE_STATUS_ON
	Your services access status will now be hidden from %s INFO displays.
NICK_SET_HIDE_STATUS_OFF
	Your services access status will now be shown in %s INFO displays.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_MSG_ON
	Os services irão agora respondê-lo através de mensagens.
NICK_SET_MSG_OFF
	Os services irão agora respondê-lo através de notices.

# SET NOEXPIRE responses
NICK_SET_NOEXPIRE_SYNTAX
	SET [nic] NOEXPIRE {ON | OFF}
NICK_SET_NOEXPIRE_ON
	O nick %s não irá expirar.
NICK_SET_NOEXPIRE_OFF
	O nick %s irá expirar.

# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [mask]
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
	%s é um Admin dos Services.
NICK_INFO_SERVICES_ROOT
	%s is a services root administrator.
NICK_INFO_ADDRESS
	Último endereço visto: %s
NICK_INFO_ADDRESS_ONLINE
	está online em: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s está online neste momento.
NICK_INFO_TIME_REGGED
	Hora de registro: %s
NICK_INFO_LAST_SEEN
	Ultimo horário visto: %s
NICK_INFO_LAST_QUIT
	Última mensagem de saída: %s
NICK_INFO_URL
	URL: %s
NICK_INFO_EMAIL
	E-mail: %s
NICK_INFO_VHOST
	    vhost: %s
NICK_INFO_VHOST2
	    vhost: %s@%s
NICK_INFO_ICQ
	ICQ: %d
NICK_INFO_GREET
	Mensagem de entrada: %s
NICK_INFO_OPTIONS
	Opções: %s
NICK_INFO_EXPIRE
	Expires in: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Proteção
NICK_INFO_OPT_SECURE
	Segurança
NICK_INFO_OPT_PRIVATE
	Privado
NICK_INFO_OPT_MSG
	Modo mensagem
NICK_INFO_OPT_NONE
	Nenhuma
NICK_INFO_NO_EXPIRE
	Este nick não irá expirar.
NICK_INFO_FOR_MORE
	Para maiores informações sobre o usuário, digite /msg %s INFO %s ALL.

# LIST responses
NICK_LIST_SYNTAX
	LIST opções
NICK_LIST_SERVADMIN_SYNTAX
	LIST opções [FORBIDDEN] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Lista de entradas com %s:
NICK_LIST_RESULTS
	Fim da listagem - %d/%d resultados mostrados.

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
	   %c%s (expires in %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nick [senha]
NICK_NO_RECOVER_SELF
	Você não pode usar o comando RECOVER em si mesmo!
NICK_RECOVERED
	Usuário que usava seu nick foi derrubado.
	/msg %s RELEASE %s para liberar seu nick antes de um minuto.

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
	Se você não souber porque este e-mail lhe foi enviado, por favor ignore-o.
NICK_SENDPASS_LINE_4
	NÃO RESPONDA ESTE E-MAIL!
NICK_SENDPASS_LINE_5
	Administradores do %s.
NICK_SENDPASS_OK
	Senha para %s foi enviada.

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID nick [motivo]
NICK_FORBID_SYNTAX_REASON
	FORBID nick motivo
NICK_FORBID_SUCCEEDED
	Nick %s está agora bloqueado.
NICK_FORBID_FAILED
	Não foi possível bloquear o nick %s!

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
	A passcode has been sent to %s, please type /msg %s confirm <passcode> to complete registration
NICK_CONFIRM_NOT_FOUND
	Registration step 1 may have expired, please use "/msg %s register <password> <email>" first.
NICK_CONFIRM_INVALID
	Invalid passcode has been entered, please check the e-mail again, and retry
NICK_REG_MAIL_SUBJECT
	Nickname Registration (%s)
NICK_REG_MAIL_HEAD
	Hi,
NICK_REG_MAIL_LINE_1
	You have requested to register the following nickname %s.
NICK_REG_MAIL_LINE_2
	Please type " /msg %s confirm %s " to complete registration.
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
	Entrada no canal não permitida se a opção RESTRICTED estiver ativada
CHAN_LEVEL_INVITE
	Permitido o uso do comando INVITE
CHAN_LEVEL_AKICK
	Permitido o uso do comando AKICK
CHAN_LEVEL_SET
	Permitido o uso do comando SET (não inclui FOUNDER/PASSWORD)
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
	Permitido associar/desassociar um bot à um canal
CHAN_LEVEL_BADWORDS
	Permitido o uso do comando BADWORDS
CHAN_LEVEL_NOKICK
	Nunca ser kickado pelos kickers do bot
CHAN_LEVEL_FANTASIA
	Permitido o uso do comando FANTASIA
CHAN_LEVEL_SAY
	Permitido o uso dos comandos SAY e ACT
CHAN_LEVEL_GREET
	Mensagem de entrada
CHAN_LEVEL_VOICEME
	Permitido usar o comando (de)voice em si mesmo
CHAN_LEVEL_VOICE
	Permitido o uso dos comandos VOICE/DEVOICE
CHAN_LEVEL_GETKEY
	Permitido o uso do comando GETKEY
CHAN_LEVEL_OPDEOPME
	Permitido usar o comando (de)op em si mesmo
CHAN_LEVEL_HALFOPME
	Permitido usar o comando (de)halfop em si mesmo
CHAN_LEVEL_HALFOP
	Permitido o uso dos comandos HALFOP/DEHALFOP
CHAN_LEVEL_PROTECTME
	Permitido usar o comando (de)protect em si mesmo
CHAN_LEVEL_PROTECT
	Permitido o uso dos comandos PROTECT/DEPROTECT
CHAN_LEVEL_KICKME
	Permitido usar o comando KICK em si mesmo
CHAN_LEVEL_KICK
	Permitido o uso do comando KICK
CHAN_LEVEL_SIGNKICK
	Sem assinatura no kick quando a opção SIGNKICK LEVEL estiver em uso

# Automatic responses
CHAN_LEVEL_BANME
	Allowed to ban him/herself
CHAN_LEVEL_BAN
	Allowed to use BAN command
CHAN_LEVEL_TOPIC
	Allowed to use TOPIC command
CHAN_LEVEL_INFO
	Allowed to use INFO command with ALL option

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
CHAN_REGISTER_SYNTAX
	REGISTER canal senha descrição
CHAN_REGISTER_DISABLED
	Desculpe, o registro de canais está temporariamente desativado.
CHAN_REGISTER_NOT_LOCAL
	Canais locais não podem ser registrados.
CHAN_MUST_REGISTER_NICK
	Você deve registrar seu nick primeiro.  Digite /msg %s HELP para maiores informações sobre registro de nick.
CHAN_MUST_IDENTIFY_NICK
	Por favor se identifique com o %s primeiro, usando o comando:
	/msg %s IDENTIFY senha
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
	You have attempted to register a none existant channel %s
CHAN_SYMBOL_REQUIRED
	Please use the symbol of # or & when attempting to register


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
	Todos os usuários registrados foram desconectados do canal %s.

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
	Mode %c ignored because you can't lock it.
CHAN_SET_MLOCK_L_REQUIRED
	Você deve setar o modo +l bem como o modo +L.
CHAN_SET_MLOCK_K_REQUIRED
	You must lock mode +i as well to lock mode +K.
CHAN_MLOCK_CHANGED
	Modo MLOCK no canal %s alterado para %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET canal KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Retenção de tópico está agora ATIVADA.
CHAN_SET_KEEPTOPIC_OFF
	Retenção de tópico está agora DESATIVADA.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET canal TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Trava de tópico está agora ATIVADA.
CHAN_SET_TOPICLOCK_OFF
	Trava de tópico está agora DESATIVADA.

# SET PRIVATE responses
CHAN_SET_PEACE_SYNTAX
	SET channel PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Peace option is now ON.
CHAN_SET_PEACE_OFF
	Peace option is now OFF.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET canal PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Opção Private está agora ATIVADA.
CHAN_SET_PRIVATE_OFF
	Opção Private está agora DESATIVADA.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET canal SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Opção Secure ops está agora ATIVADA.
CHAN_SET_SECUREOPS_OFF
	Opção Secure ops está agora DESATIVADA.
	
# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET canal SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Opção Secure founder está agora ATIVADA.
CHAN_SET_SECUREFOUNDER_OFF
	Opção Secure founder está agora DESATIVADA.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET canal RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Opção Restricted está agora ATIVADA.
CHAN_SET_RESTRICTED_OFF
	Opção Restricted está agora DESATIVADA.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET canal SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Opção Secure está agora ATIVADA.
CHAN_SET_SECURE_OFF
	Opção Secure está agora DESATIVADA.
	
# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET canal SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Opção Signed kick está agora ATIVADA.
CHAN_SET_SIGNKICK_LEVEL
	Opção Signed kick está agora ATIVADA, mas depende do nível de acesso 
	do usuário que usar o comando.
CHAN_SET_SIGNKICK_OFF
	Opção Signed kick está agora DESATIVADA.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET canal OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Opção Op-notice está agora ATIVADA.
CHAN_SET_OPNOTICE_OFF
	Opção Op-notice está agora DESATIVADA.

# SET NOEXPIRE responses
CHAN_SET_XOP_SYNTAX
	SET channel XOP {ON | OFF}
CHAN_SET_XOP_ON
	xOP lists system is now ON.
CHAN_SET_XOP_OFF
	xOP lists system is now OFF.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET canal NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	O canal %s não irá expirar.
CHAN_SET_NOEXPIRE_OFF
	O canal %s irá expirar.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Sorry, you can only have %d AOP/SOP/VOP entries on a channel.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	You can't use this command. Use the ACCESS command instead.
	Type /msg %s HELP ACCESS for more information.

# AOP messages
CHAN_AOP_SYNTAX
	AOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]
CHAN_AOP_DISABLED
	Sorry, channel AOP list modification is temporarily disabled.
CHAN_AOP_NICKS_ONLY
	Channel AOP lists may only contain registered nicknames.
CHAN_AOP_ADDED
	%s added to %s AOP list.
CHAN_AOP_MOVED
	%s moved to %s AOP list.
CHAN_AOP_NO_SUCH_ENTRY
	No such entry (#%d) on %s AOP list.
CHAN_AOP_NOT_FOUND
	%s not found on %s AOP list.
CHAN_AOP_NO_MATCH
	No matching entries on %s AOP list.
CHAN_AOP_DELETED
	%s deleted from %s AOP list.
CHAN_AOP_DELETED_ONE
	Deleted 1 entry from %s AOP list.
CHAN_AOP_DELETED_SEVERAL
	Deleted %d entries from %s AOP list.
CHAN_AOP_LIST_EMPTY
	%s AOP list is empty.
CHAN_AOP_LIST_HEADER
	AOP list for %s:
	  Num  Nick
CHAN_AOP_CLEAR
	Channel AOP list has been cleared.

# HOP messages
CHAN_HOP_SYNTAX
	HOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]
CHAN_HOP_DISABLED
	Sorry, channel HOP list modification is temporarily disabled.
CHAN_HOP_NICKS_ONLY
	Channel HOP lists may only contain registered nicknames.
CHAN_HOP_ADDED
	%s added to %s HOP list.
CHAN_HOP_MOVED
	%s moved to %s HOP list.
CHAN_HOP_NO_SUCH_ENTRY
	No such entry (#%d) on %s HOP list.
CHAN_HOP_NOT_FOUND
	%s not found on %s HOP list.
CHAN_HOP_NO_MATCH
	No matching entries on %s HOP list.
CHAN_HOP_DELETED
	%s deleted from %s HOP list.
CHAN_HOP_DELETED_ONE
	Deleted 1 entry from %s HOP list.
CHAN_HOP_DELETED_SEVERAL
	Deleted %d entries from %s HOP list.
CHAN_HOP_LIST_EMPTY
	%s HOP list is empty.
CHAN_HOP_LIST_HEADER
	HOP list for %s:
	  Num  Nick
CHAN_HOP_CLEAR
	Channel HOP list has been cleared.

# SOP messages
CHAN_SOP_SYNTAX
	SOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]
CHAN_SOP_DISABLED
	Sorry, channel SOP list modification is temporarily disabled.
CHAN_SOP_NICKS_ONLY
	Channel SOP lists may only contain registered nicknames.
CHAN_SOP_ADDED
	%s added to %s SOP list.
CHAN_SOP_MOVED
	%s moved to %s SOP list.
CHAN_SOP_NO_SUCH_ENTRY
	No such entry (#%d) on %s SOP list.
CHAN_SOP_NOT_FOUND
	%s not found on %s SOP list.
CHAN_SOP_NO_MATCH
	No matching entries on %s SOP list.
CHAN_SOP_DELETED
	%s deleted from %s SOP list.
CHAN_SOP_DELETED_ONE
	Deleted 1 entry from %s SOP list.
CHAN_SOP_DELETED_SEVERAL
	Deleted %d entries from %s SOP list.
CHAN_SOP_LIST_EMPTY
	%s SOP list is empty.
CHAN_SOP_LIST_HEADER
	SOP list for %s:
	  Num  Nick
CHAN_SOP_CLEAR
	Channel SOP list has been cleared.

# VOP messages
CHAN_VOP_SYNTAX
	VOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]
CHAN_VOP_DISABLED
	Sorry, channel VOP list modification is temporarily disabled.
CHAN_VOP_NICKS_ONLY
	Channel VOP lists may only contain registered nicknames.
CHAN_VOP_ADDED
	%s added to %s VOP list.
CHAN_VOP_MOVED
	%s moved to %s VOP list.
CHAN_VOP_NO_SUCH_ENTRY
	No such entry (#%d) on %s VOP list.
CHAN_VOP_NOT_FOUND
	%s not found on %s VOP list.
CHAN_VOP_NO_MATCH
	No matching entries on %s VOP list.
CHAN_VOP_DELETED
	%s deleted from %s VOP list.
CHAN_VOP_DELETED_ONE
	Deleted 1 entry from %s VOP list.
CHAN_VOP_DELETED_SEVERAL
	Deleted %d entries from %s VOP list.
CHAN_VOP_LIST_EMPTY
	%s VOP list is empty.
CHAN_VOP_LIST_HEADER
	VOP list for %s:
	  Num  Nick
CHAN_VOP_CLEAR
	Channel VOP list has been cleared.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS canal {ADD|DEL|LIST|CLEAR} [nick [nível] | entry-list]
CHAN_ACCESS_XOP
	You can't use this command. 
	Use the AOP, SOP and VOP commands instead.
	Type /msg %s HELP command for more information.
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
	%s adicionado a lista de acesso do %s com o nível %d.
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
	Lista de acesso do %s vazia.
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
	Lista de acesso do canal apagada.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK canal {ADD | DEL | LIST | VIEW | ENFORCE | CLEAR} [nick-ou-máscara]
CHAN_AKICK_DISABLED
	Desculpe, o comando AKICK está temporariamente desativado.
CHAN_AKICK_ALREADY_EXISTS
	%s já existe na lista de akick do %s.
CHAN_AKICK_REACHED_LIMIT
	Desculpe, você pode ter apenas %d akicks na lista do canal.
CHAN_AKICK_ADDED
	%s adicionado a lista de akick do %s.
CHAN_AKICK_NO_SUCH_ENTRY
	Nenhuma entrada com (#%d) na lista de akick do %s.
CHAN_AKICK_NOT_FOUND
	%s não encontrado na lista de akick do %s.
CHAN_AKICK_NO_MATCH
	Nenhum resultado encontrado na lista de akick do %s.
CHAN_AKICK_STUCK
	%s is now always active on channel %s.
CHAN_AKICK_UNSTUCK
	%s is not always active anymore on channel %s.
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
	  %3d %s%s
CHAN_AKICK_VIEW_FORMAT
	%3d %s (por %s em %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (stuck) (by %s on %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE para %s terminado; %d usuários afetados.
CHAN_AKICK_CLEAR
	Lista de akick do canal foi limpa.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS canal {SET | DIS[ABLE] | LIST | RESET} [item [nível]]
CHAN_LEVELS_XOP
	This command is useless in this mode.
CHAN_LEVELS_RANGE
	O nível deve ser entre %d e %d inclusive.
CHAN_LEVELS_CHANGED
	Nível para %s no canal %s alterado para %d.
CHAN_LEVELS_UNKNOWN
	Level %s desconhecido.  Digite /msg %s HELP LEVELS DESC para uma lista de levels válidos.
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
	OP Notice
CHAN_INFO_OPT_PEACE
	Peace
CHAN_INFO_OPT_PRIVATE
	Privado
CHAN_INFO_OPT_RESTRICTED
	Acesso restrito
CHAN_INFO_OPT_SECURE
	Seguro
CHAN_INFO_OPT_SECUREOPS
	Secure Ops
CHAN_INFO_OPT_SECUREFOUNDER
	Secure Founder
CHAN_INFO_OPT_SIGNKICK
	Signed kicks
CHAN_INFO_OPT_TOPICLOCK
	Trava de tópico
CHAN_INFO_OPT_XOP
	xOP lists system
CHAN_INFO_OPT_NONE
	Nenhuma
CHAN_INFO_MODE_LOCK
	        Trava de modo: %s
CHAN_INFO_EXPIRE
	      Expires in: %s
CHAN_INFO_NO_EXPIRE
	Este canal não irá expirar.

# LIST responses
CHAN_LIST_SYNTAX
	LIST padrão
CHAN_LIST_SERVADMIN_SYNTAX
	LIST padrão [FORBIDDEN] [NOEXPIRE]
CHAN_LIST_HEADER
	Lista de entradas com %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Fim da lista - %d/%d resultados mostrados.
	
# CLIST responses
CHAN_INVITE_SYNTAX
	INVITE canal

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN canal
CHAN_UNBANNED
	Você não está mais banido do %s.

# CLEAR responses
CHAN_TOPIC_SYNTAX
	TOPIC channel [topic]

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
	Mode +h has been cleared from channel %s.
CHAN_CLEARED_VOICES
	Mode +v foi retirado do canal %s.
CHAN_CLEARED_USERS
	Todos os usuários foram kickados do canal %s.
CHAN_CLEARED_INVITES
	All invites on channel %s have been removed.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS canal
CHAN_GETPASS_UNAVAILABLE
	Comando GETPASS indisponível: modo de encriptação em uso.
CHAN_GETPASS_PASSWORD_IS
	Senha do canal %s é %s.
	
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
	Você tem 1 novo memo.
MEMO_HAVE_NEW_MEMOS
	Você tem %d novos memos.
MEMO_TYPE_READ_LAST
	Digite /msg %s READ LAST para ler.
MEMO_TYPE_READ_NUM
	Digite /msg %s READ %d para ler.
MEMO_TYPE_LIST_NEW
	Digite /msg %s LIST NEW para listar.
MEMO_AT_LIMIT
	Atenção: Você atingiu seu número máximo de memos (%d).  Não será possível receber novos memos enquanto você não apagar alguns.
MEMO_OVER_LIMIT
	Atenção: Você ultrapassou seu número máximo de memos (%d).  Não será possível receber novos memos enquanto você não apagar alguns.
MEMO_X_MANY_NOTICE
	There are %d memos on channel %s.
MEMO_X_ONE_NOTICE
	There is %d memo on channel %s.
MEMO_NEW_X_MEMO_ARRIVED
	There is a new memo on channel %s.
	Type /msg %s READ %s %d to read it.
MEMO_NEW_MEMO_ARRIVED
	Você possui um novo memo de %s.
	Digite /msg %s READ %d para ler.

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
	
# CANCEL responses
MEMO_MASS_SENT
	A massmemo has been sent to all registered users.
	
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
	Memos para %s.  Para ler, digite: /msg %s READ núm
MEMO_LIST_NEW_MEMOS
	Novos memos para %s.  Para ler, digite: /msg %s READ núm
MEMO_LIST_CHAN_MEMOS
	Memos para %s.  Para ler, digite: /msg %s READ %s núm
MEMO_LIST_CHAN_NEW_MEMOS
	Novos memos para %s.  Para ler, digite: /msg %s READ %s núm
MEMO_LIST_HEADER
	 Núm  Remetente            Data/Hora
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [canal] {list | LAST | NEW}
MEMO_HEADER
	Memo %d de %s (%s).  Para apagar, digite: /msg %s DEL %d
MEMO_CHAN_HEADER
	Memo %d de %s (%s).  Para apagar, digite: /msg %s DEL %s %d
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

# SET responses
MEMO_CHAN_DELETED_ALL
	All memos for channel %s have been deleted.

# SET responses
MEMO_SET_SYNTAX
	SET opção parâmetros
MEMO_SET_DISABLED
	Desculpe, a opção SET está temporariamente desativada.
MEMO_SET_UNKNOWN_OPTION
	Comando SET desconhecido: %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s irá agora notificá-lo de novos memos quando você conectar ou quando eles lhe forem enviados.
MEMO_SET_NOTIFY_LOGON
	%s irá agora notificá-lo de novos memos quando você conectar ou desativar o /AWAY.
MEMO_SET_NOTIFY_NEW
	%s irá agora notificá-lo de novos memos quando eles lhe forem enviados.
MEMO_SET_NOTIFY_OFF
	%s não irá mais notificá-lo de novos memos.

# SET LIMIT responses
MEMO_SET_NOTIFY_MAIL
	You will now be informed about new memos via email.
MEMO_SET_NOTIFY_NOMAIL
	You will no longer be informed via email.
MEMO_SET_NOTIFY_INVALIDMAIL
	There's no email address set for your nick.

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
	INFO [anal]
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
	Seu limite de memos é de 0; você não pode receber novos memos.  Você não pode alterar esse limite.
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
	Bot %s não existe.
BOT_NOT_ASSIGNED
	Você precisa primeiro associar um bot ao canal antes de usar este comando.
	Digite /msg %S HELP ASSIGN para maiores informações.
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
	Por favor não user reversos no canal!
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
	BOT LIST
BOT_BOT_ALREADY_EXISTS
	Bot %s já existe.
BOT_BOT_CREATION_FAILED
	Desculpe, criação do bot falhou.
BOT_BOT_READONLY
	Desculpe, comando CHANGE temporariamente desativado.
BOT_BOT_ADDED
	%s!%s@%s (%s) adicionado a lista de bots.
BOT_BOT_ANY_CHANGES
	Informação antiga é igual a nova.
BOT_BOT_CHANGED
	Bot %s foi alterado para %s!%s@%s (%s)
BOT_BOT_DELETED
	Bot %s foi removido.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Lista de bots:
BOT_BOTLIST_PRIVATE_HEADER
	Bots reserved to IRC operators:
BOT_BOTLIST_FOOTER
	%d bots available.
BOT_BOTLIST_EMPTY
	Não existem bots disponíveis no momento.
	Pergunte a um Administrador dos Services para criar mais bots!
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN canal nick
BOT_ASSIGN_READONLY
	Desculpe, mas o comando ASSIGN está temporariamente desativado.
BOT_ASSIGN_ALREADY
	Bot %s is already assigned to channel %s.
BOT_ASSIGN_ASSIGNED
	Bot %s foi associado ao canal %s.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN canal
BOT_UNASSIGN_UNASSIGNED
	Não possui mais nenhum bot associado ao canal %s.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {canal | nick}
BOT_INFO_NOT_FOUND
	%s não é um bot válido ou um canal registrado.
BOT_INFO_BOT_HEADER
	Informações do bot %s:
BOT_INFO_BOT_MASK
	       Máscara : %s@%s
BOT_INFO_BOT_REALNAME
	     Nome real : %s
BOT_INFO_BOT_CREATED
	        Criado : %s
BOT_INFO_BOT_USAGE
	      Usado em : %d canal(is)
BOT_INFO_BOT_OPTIONS
	    Options : %s
BOT_INFO_OPT_PRIVATE
	Private
BOT_INFO_CHAN_HEADER
	Informações do canal %s:
BOT_INFO_CHAN_BOT
	           Nick do bot : %s
BOT_INFO_CHAN_BOT_NONE
	           Nick do bot : nenhum associado no momento.
BOT_INFO_CHAN_KICK_BADWORDS
	     Kick por palavrão : %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	     Kick por palavrão : %s (%d kick(s) para banir)
BOT_INFO_CHAN_KICK_BOLDS
	      Kick por negrito : %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	      Kick por negrito : %s (%d kick(s) para banir)
BOT_INFO_CHAN_KICK_CAPS_ON
	         Kick por Caps : %s (mínimo %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	         Kick por Caps : %s (%d kick(s) para banir; mínimo %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	         Kick por Caps : %s
BOT_INFO_CHAN_KICK_COLORS
	        Kick por cores : %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	        Kick por cores : %s (%d kick(s) para banir)
BOT_INFO_CHAN_KICK_FLOOD_ON
	        Kick por flood : %s (%d linhas em %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	        Kick por flood : %s (%d kick(s) para banir; %d linhas em %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	        Kick por flood : %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	    Kick por repetição : %s (%d vezes)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	    Kick por repetição : %s (%d kick(s) para banir; %d vezes)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	    Kick por repetição : %s
BOT_INFO_CHAN_KICK_REVERSES
	    Kick por reversos : %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	    Kick por reversos : %s (%d kick(s) para banir)
BOT_INFO_CHAN_KICK_UNDERLINES
	   Kick por sublinhado : %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	   Kick por sublinhado : %s (%d kick(s) para banir)
BOT_INFO_ACTIVE
	ativado
BOT_INFO_INACTIVE
	desativado
BOT_INFO_CHAN_OPTIONS
	                Opções : %s
BOT_INFO_OPT_DONTKICKOPS
	Proteção de Ops
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
	SET canal opções configurações
BOT_SET_DISABLED
	Desculpe, o comando SET está temporariamente desativado.
BOT_SET_UNKNOWN
	Opção desconhecida: %s.
	Digite /msg %S HELP SET para maiores informações.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET canal DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Bot não irá kickar ops no canal %s.
BOT_SET_DONTKICKOPS_OFF
	Bot irá kickar ops no canal %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET canal DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Bot não irá kickar voices no canal %s.
BOT_SET_DONTKICKVOICES_OFF
	Bot irá kickar voices no canal %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET canal FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Modo Fantasia está agora ATIVADO no canal %s.
BOT_SET_FANTASY_OFF
	Modo Fantasia está agora DESATIVADO no canal %s.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET canal GREET {ON|OFF}
BOT_SET_GREET_ON
	Modo mensagem de entrada está agora ATIVADO no canal %s.
BOT_SET_GREET_OFF
	Modo mensagem de entrada está agora DESATIVADO no canal %s.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET canal NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Modo NOBOT está agora ATIVADO no canal %s.
BOT_SET_NOBOT_OFF
	Modo NOBOT está agora DESATIVADO no canal %s.
	
# SET SYMBIOSIS
BOT_SET_PRIVATE_SYNTAX
	SET channel PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Private mode of bot %s is now ON.
BOT_SET_PRIVATE_OFF
	Private mode of bot %s is now OFF.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET canal SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Modo SYMBIOSIS está agora ATIVADO no canal %s.
BOT_SET_SYMBIOSIS_OFF
	Modo SYMBIOSIS está agora DESATIVADO no canal %s.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK canal opções {ON|OFF} [configurações]
BOT_KICK_DISABLED
	Desculpe, as configurações do kicker estão temporariamente desativadas.
BOT_KICK_UNKNOWN
	Opção desconhecida: %s.
	Digite /msg %S HELP KICK para maiores informações.
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
	SAY canal texto

# ACT responses
BOT_ACT_SYNTAX
	ACT canal texto

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
###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Os services não conseguiram alterar os modos.  Seus servers estão configurados corretamente?
OPER_BOUNCY_MODES_U_LINE
	Os services não conseguiram alterar os modos.  As U:lines do seus servers estão configuradas corretamente?

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
	Services ativos %d horas, %d minut0
OPER_STATS_UPTIME_H1M1S
	Services ativos %d horas, %d minut0
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
	Channel       : %6d registros, %5d kB
OPER_STATS_GROUPS_MEM
	NS Grupos     : %6d registros, %5d kB
OPER_STATS_ALIASES_MEM
	NS Aliases    : %6d registros, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ      : %6d registros, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ       : %6d registros, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ      : %6d registros, %5d kB
OPER_STATS_SESSIONS_MEM
	Sessões       : %6d registros, %5d kB
OPER_STATS_PROXY_MEM
	Proxies       : %6d records, %5d kB
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
	Current number of SQLINEs: %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Default SQLINE expiry time: %d days
OPER_STATS_SQLINE_EXPIRE_DAY
	Default SQLINE expiry time: 1 day
OPER_STATS_SQLINE_EXPIRE_HOURS
	Default SQLINE expiry time: %d hours
OPER_STATS_SQLINE_EXPIRE_HOUR
	Default SQLINE expiry time: 1 hour
OPER_STATS_SQLINE_EXPIRE_MINS
	Default SQLINE expiry time: %d minutes
OPER_STATS_SQLINE_EXPIRE_MIN
	Default SQLINE expiry time: 1 minute
OPER_STATS_SQLINE_EXPIRE_NONE
	Default SQLINE expiry time: No expiration
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
OPER_MODE_SYNTAX
	MODE canal modos

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
	SVSNICK nick newnick 
OPER_SVSNICK_NEWNICK
	The nick %s is now being changed to %s.
	
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
	%s adicionado a lista de Administradores dos Services.
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

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | lista]
OPER_OPER_SKELETON
	Services estão no modo esqueleto; o comando OPER não está disponível..
OPER_OPER_EXISTS
	%s já está na lista de Operadores dos Services.
OPER_OPER_REACHED_LIMIT
	Desculpe, você pode ter apenas %d Operadores dos Services.
OPER_OPER_ADDED
	%s adicionado a lista de Operadores dos Services.
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
	%s adicionado a lista de AKILL.
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
	Current AKILL list:
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
	
# SGLINE responses
OPER_CHANKILL_SYNTAX
	CHANAKILL [+expiry] {#channel} [reason]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+tempo] {máscara | lista}[:motivo]]
OPER_SGLINE_UNSUPPORTED
	Desculpe, o comando SGLINE não está disponível nesta Rede.
OPER_SGLINE_EXISTS
	%s já se encontra na lista de SGLINE.
OPER_SGLINE_ALREADY_COVERED
	%s já está coberto por %s.
OPER_SGLINE_REACHED_LIMIT
	Desculpe, você pode ter apenas %d SGLINEs.
OPER_SGLINE_ADDED
	%s adicionado a lista de SGLINE.
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
	Current SGLINE list:
	  Núm   Máscara                           Motivo
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Current SGLINE list:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (por %s em %s; %s)
	      %s
OPER_SGLINE_CLEAR
	A lista  de SGLINE foi apagada.

# SZLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	Channel SQLINEs are not supported by your IRCd, so you can't use them.
OPER_SQLINE_EXISTS
	%s already exists on the SQLINE list.
OPER_SQLINE_ALREADY_COVERED
	%s is already covered by %s.
OPER_SQLINE_REACHED_LIMIT
	Sorry, you can only have %d SQLINEs.
OPER_SQLINE_ADDED
	%s added to the SQLINE list.
OPER_SQLINE_CHANGED
	Expiry time of %s changed.
OPER_SQLINE_NOT_FOUND
	%s not found on the SQLINE list.
OPER_SQLINE_NO_MATCH
	No matching entries on the SQLINE list.
OPER_SQLINE_DELETED
	%s deleted from the SQLINE list.
OPER_SQLINE_DELETED_ONE
	Deleted 1 entry from the SQLINE list.
OPER_SQLINE_DELETED_SEVERAL
	Deleted %d entries from the SQLINE list.
OPER_SQLINE_LIST_EMPTY
	SQLINE list is empty.
OPER_SQLINE_LIST_HEADER
	Current SQLINE list:
	  Num   Mask                              Reason
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Current SQLINE list:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SQLINE_CLEAR
	The SQLINE list has been cleared.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+tempo] {máscara | lista} [motivo]]
OPER_SZLINE_UNSUPPORTED
	Desculpe, comando SZLINE não disponível nesta Rede.
OPER_SZLINE_EXISTS
	%s já existe na lista de SZLINE.
OPER_SZLINE_ALREADY_COVERED
	%s já está coberto por %s.
OPER_SZLINE_REACHED_LIMIT
	Desculpe, você pode ter apenas %d SZLINEs.
OPER_SZLINE_ONLY_IPS
	Lembrete: você pode apenas adicionar endereços de IP para a lista de SZLINE.
OPER_SZLINE_ADDED
	%s adicionado a lista de SZLINE.
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
	Current SZLINE list:
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

# NOOP responses
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} servidor
OPER_NOOP_SET
	Todas as O:lines do servidor %s foram removidas.
OPER_NOOP_REVOKE
	Todas as O:lines do servidor %s foram resetadas.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE server [motivo]

# RAW responses
OPER_JUPE_HOST_ERROR
	Please use a valid server name when juping
	
# RAW responses
OPER_RAW_SYNTAX
	RAW texto

# UPDATE responses
OPER_UPDATING
	Atualizando banco de dados.
	
# RELOAD responses
OPER_RELOAD
	Arquivos de configuração dos services foram recarregados.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN não definido; não foi possível reiniciar.  Rode novamente o \2configure\2 e reconfigure os services para habilitar o comando RESTART.

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
	Lista de ignore dos Services:
OPER_IGNORE_LIST_NOMATCH
	%s not found on ignore list.
OPER_IGNORE_LIST_EMPTY
	Lista de ignore dos Services vazia.

# KILLCLONES responses
OPER_IGNORE_LIST_CLEARED
	Ignore list has been cleared.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	Não foi possível encontrar o nick %s.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Lista de canais:
	Nome                 Users Tópico
OPER_CHANLIST_HEADER_USER
	Lista de canais do %s:
	Nome                 Users Tópico
OPER_CHANLIST_RECORD
	%-20s  %4d %s
OPER_CHANLIST_END
	Fim da listagem.
	
# USERLIST responses
OPER_USERLIST_HEADER
	Lista de usuários:
	Nick                 Máscara
OPER_USERLIST_HEADER_CHAN
	Lista de users do %s:
	Nick                 Máscara
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Fim da listagem.

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_CACHE_SYNTAX
	CACHE {DEL | LIST} {hostname | pattern} [QUEUED | ALL]
OPER_CACHE_DISABLED
	Proxy detection is disabled.
OPER_CACHE_NOT_FOUND
	%s was not found in the cache.
OPER_CACHE_REMOVED
	%s has been removed from the cache.
OPER_CACHE_HEADER
	Hostname                                         Status
OPER_CACHE_LIST
	%-48s %s
OPER_CACHE_FOOTER
	End of list - %d/%d matches shown.
OPER_CACHE_QUEUED
	Queued
OPER_CACHE_PROGRESS
	In progress
OPER_CACHE_NORMAL
	Normal
OPER_CACHE_WINGATE
	Wingate
OPER_CACHE_SOCKS4
	SOCKS 4
OPER_CACHE_SOCKS5
	SOCKS 5
OPER_CACHE_HTTP
	HTTP proxy

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
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
OPER_MODULE_LOAD_SYNTAX
	MODLOAD FileName
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD FileName
OPER_MODULE_LIST_HEADER
	Current Module list:
OPER_MODULE_LIST
	Module: %s [%s]
OPER_MODULE_LIST_FOOTER
	%d Modules loaded.
OPER_MODULE_INFO_LIST
	Module: %s Version: %s Author: %s loaded: %s
OPER_MODULE_CMD_LIST
	Providing command: /msg %s %s
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
	Atual lista de Kimite de sessões:
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
	EXCEPTION MOVE move uma exceção de número num para uma
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
	O limite deve ser um número maior que 1. Isto deve previnir 
	a listagem de muitos hosts com um limite de conexões 
	limitado a 1.
	SESSION VIEW mostra informações detalhadas sobre um host
	específico, incluindo o número de conexões atual e o limite
	A máscara dada em host não deve incluir wildcards.
	
	Veja a ajuda sobre EXCEPTION para mais informações sobre
	o limite de conexões e como especificar limites de conexões
	para certos hosts e grupos de hosts.
	
	Limitado aos Administradores dos Services.

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
	[Logon News - %s] %s
NEWS_OPER_TEXT
	[Oper News - %s] %s

NEWS_RANDOM_TEXT
	[Random News - %s] %s

NEWS_LOGON_SYNTAX
	Sintaxe: LOGONNEWS {ADD|DEL|LIST} [texto|núm]
NEWS_LOGON_LIST_HEADER
	Logon news items:
NEWS_LOGON_LIST_ENTRY
	%5d (%s por %s)
	    %s
NEWS_LOGON_LIST_NONE
	Não existem logon news.
NEWS_LOGON_ADD_SYNTAX
	Sintaxe: LOGONNEWS ADD texto
NEWS_LOGON_ADD_FULL
	Lista de logon news está cheia!
NEWS_LOGON_ADDED
	Adicionado novo logon news (#%d).
NEWS_LOGON_DEL_SYNTAX
	Sintaxe: LOGONNEWS DEL {núm | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Logon news não encontrado #%d!
NEWS_LOGON_DELETED
	Logon news removido #%d.
NEWS_LOGON_DEL_NONE
	Nenhum logon news para remover!
NEWS_LOGON_DELETED_ALL
	Todos os logon news foram removidos.

NEWS_OPER_SYNTAX
	Sintaxe: OPERNEWS {ADD|DEL|LIST} [texto|núm]
NEWS_OPER_LIST_HEADER
	Oper news items:
NEWS_OPER_LIST_ENTRY
	%5d (%s por %s)
	    %s
NEWS_OPER_LIST_NONE
	Não existe nenhum oper news.
NEWS_OPER_ADD_SYNTAX
	Sintaxe: OPERNEWS ADD texto
NEWS_OPER_ADD_FULL
	Lista de oper news está cheia!
NEWS_OPER_ADDED
	Adicionado novo oper news (#%d).
NEWS_OPER_DEL_SYNTAX
	Sintaxe: OPERNEWS DEL {núm | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Oper news não encontrado #%d!
NEWS_OPER_DELETED
	Oper news removido #%d.
NEWS_OPER_DEL_NONE
	Nenhum oper news para remover!
NEWS_OPER_DELETED_ALL
	Todos os oper news foram removidos.

NEWS_RANDOM_SYNTAX
	Syntax: RANDOMNEWS {ADD|DEL|LIST} [text|num]
NEWS_RANDOM_LIST_HEADER
	Random news items:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_RANDOM_LIST_NONE
	There is no random news.
NEWS_RANDOM_ADD_SYNTAX
	Syntax: RANDOMNEWS ADD text
NEWS_RANDOM_ADD_FULL
	News list is full!
NEWS_RANDOM_ADDED
	Added new random news item (#%d).
NEWS_RANDOM_DEL_SYNTAX
	Syntax: RANDOMNEWS DEL {num | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Random news item #%d not found!
NEWS_RANDOM_DELETED
	Random news item #%d deleted.
NEWS_RANDOM_DEL_NONE
	No random news items to delete!
NEWS_RANDOM_DELETED_ALL
	All random news items deleted.

NEWS_HELP_LOGON
	Sintaxe: LOGONNEWS ADD texto
	         LOGONNEWS DEL {núm | ALL}
	         LOGONNEWS LIST
	
	Edita ou mostra a lista de logon news. Quando um usuário
	se conecta à rede, estas mensagens são enviadas para ele.
	(No entanto, não mais que três mensagens serão enviadas
	simultaneamente para evitar flood. Se houver mais que três
	mensagens novas, apenas essas mais novas serão mostradas.)
	
	LOGONNEWS LIST pode ser usado por qualquer Operador
	para listar as mensagens atuais. ADD e DEL só podem
	ser usados pelos Administradores dos Services.

NEWS_HELP_OPER
	Sintaxe: OPERNEWS ADD texto
	         OPERNEWS DEL {núm | ALL}
	         OPERNEWS LIST
	
	Edita ou mostra a lista de oper news. Quando um usuário
	se torna operador (pelo comando /OPER), estas mensagens
	lhe serão enviadas. (No entanto, não mais que três
	mensagens serão enviadas simultaneamente para evitar
	flood. Se houver mais que três mensagens novas, apenas
	as três mais novas serão mostradas).
	
	OPERNEWS LIST pode ser usado por qualquer Operador
	para listar as oper news. ADD e DEL só podem ser usados
	Administradores dos Services.

###########################################################################
#
# HelpServ help message
#
###########################################################################

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
	%S é um services criado para dar informações sobre
	os Services.  Os tópicos de ajuda são enviados pelo 
	comando HELP dos outros clientes dos Services:

	/msg %s HELP
	     para informações sobre como registrar nicks

	/msg %s HELP
	     para informações sobre como registrar e controlar
	     canais

	/msg %s HELP
	     para informações sobre como enviar mensagens para
	     usuários que não estejam conectados no momento.

HELP_HELP_BOT

	/msg %s HELP
	     para informações sobre como colocar um bot em seu canal

HELP_HELP_HOST

	/msg %s HELP
	     for information on setting up nick vHosts

###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP
	%S permite que você "registre" um nick e evita que outras
	pessoas o utilizem. Os seguintes comandos permitem o
	registro e a manutenção dos nicks; para utilizá-los, digite
	/msg %S comando. Para maiores informações sobre um
	comando digite /msg %S HELP comando.
	
	    REGISTER   Registra um nick
	    GROUP      Se junta a um grupo
	    IDENTIFY   Identifica seu nick com sua senha
	    ACCESS     Modifica a lista de endereços autorizados
	    SET        Ajusta opções, incluindo proteção de kill
	    DROP       Cancela o registro de um nick
	    RECOVER    Derruba usuário que esteja usando seu nick
	    RELEASE    Retoma a custódia do seu nick após um RECOVER
	    SENDPASS   Esqueceu sua senha? Tente isto
	
	Outros comandos: GHOST, GLIST, INFO, LIST, LOGOUT, STATUS
	
	NOTICE: Este serviço tem a intenção de prover um modo para
	que usuários do IRC não tenham sua identidade comprometida.
	Isto NÃO é voltado para facilitar "roubo" dos nicks ou
	outras atitudes má intencionadas. O abuso do %S
	resultará em, no mínimo, perda do(s) nick(s) abusivo(s). 
	
NICK_HELP_EXPIRES
	
	Nicks que não estejam mais sendo usados estão sujeitos 
	a expirarem automaticamente, ex. eles serão apagados
	após %d dias sem serem usados.

NICK_HELP_REGISTER
	Sintaxe: REGISTER senha [email]
	
	Registra seu nick no banco de dados do %S. Uma vez
	que seu nick está registrado, você pode usar os comandos
	SET e ACCESS para configurar as opções do seu nick como
	você preferir depois. Certifique-se de lembrar sua senha
	durante o registro, você precisará dela para fazer alterações
	no seu nick futuramente.
	(Lembre-se que maiúsculas diferem, como:  ANOPE, Anope e 
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
	Sua privacidade é respeitada; teste e-mail nunca será dado
	para terceiros.
	
	Este comando servirá também para criar um GRUPO para o seu
	nick, e irá permitir que você registre outros nicks e divida
	as mesmas configurações, os mesmos memos e os mesmos 
	privilégios em canais. É conhecido em outras redes como LINK.
	Para maiores informações, digite /msg %S HELP GROUP.
	
NICK_HELP_GROUP
	Syntax: GROUP alvo senha
	
	Liga seu nick a outro, efetivamente, fazendo uma ligação 
	do seu nick atual com um outro nick especificado. Quando
	você emite este comando, a lista de acesso do seu nick
	atualmente em uso é apagada e substituida pelo nick que
	você está ligando, todos os memos do seu nick atual são
	adicionados na lista de memos do nick alvo (isso pode
	causar um excesso no limite de memos do nick, em alguns
	casos você precisa apagar alguns memos antes de poder
	receber mais memos de uma outra pessoa).
	
	Uma vez que a ligação foi estabilizada, seu nick será
	transparentemente convertido para o nick alvo em todos
	os lugares nos Services, exceto para os comandos NickServ
	UNLINK e DROP.
	 
	Por exemplo, você pode usar um nick ou outro para ler
	o mesmo conjunto de memos (e mensagens enviadas para
	um nick ou outro irão para a mesma lista). Se voce se
	identificar para um nick, estará automaticamente
	identificado para o outro. Se um nick ou outro está
	na lista de acesso de um canal, ambos nicks terão os
	mesmos privilégios de acesso.
	
	Para usar este comando, você deve identificar o nick
	atual usando /msg %S IDENTIFY senha, e você
	deve forneceer a senha para o nick que você deseja 
	ligar.
	
	Atenção: todos os nicks do grupo terão a mesma senha.

NICK_HELP_IDENTIFY
	Syntax: IDENTIFY senha
	
	Identifica você ao %S como sendo dono do nick.
	Muitos comandos exigem que você se identifique com este
	comando antes de você usá-los. A senha deve ser a mesma que
	você enviou com o comando REGISTER.
	
NICK_HELP_UPDATE
	Syntax: UPDATE

	Updates your current status, i.e. it checks for new memos,
	sets needed chanmodes (ModeonID) and updates your vhost and
	your userflags (lastseentime, etc).
	
NICK_HELP_LOGOUT
	Sintaxe: LOGOUT
	
	Este comando reverte o efeito do comando IDENTIFY, ex.
	faz com que você não seja mais reconhecido como o verdadeiro
	dono do nick. Note, entretanto, que ele não pedira que
	você se identifique novamente.

NICK_HELP_DROP
	Syntax: DROP [nick]
	
	Sem parâmetros, cancela o registro de seu nick no banco de
	dados do %S.
	
	Com parâmetros, cancela o registro de um nick especificado
	do banco de dados (restrito aos Administradores dos
	Services).
	
	Para você usar este comando, será necessário que primeiro
	você se identifique usando sua senha (/msg %S HELP DROP 
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
	
	Ajusta várias opções de nick.  A opção pode ser:
	
	    DISPLAY    Mostra o seu grupo nos Services
	    PASSWORD   Ajusta a senha do seu nick
	    LANGUAGE   Ajusta a linguagem dos Services quando
	                   mensagens são enviadas à você
	    URL        Associa um endereço URL com seu nick
	    EMAIL      Associa um endereço de e-mail ao seu nick
	    ICQ        Associa um número de ICQ ao seu nick
	    GREET      Associa uma mensgem de entrada ao seu nick
	    KILL       Ativa/Desativa a proteção de kill para o seu
	               nick
	    SECURE     Ativa/Desativa o dispositivo de segurança para
	               o seu nick
	    PRIVATE    Previne seu nick de aparecer em um /msg %S LIST
	    HIDE       Esconde certas informações sobre seu nick
	    MSG        Altera o método de comunicação dos Services
	
	Para usar este comando, você deve primeiro se identificar
	com sua senha (/msg %S HELP IDENTIFY para maiores
	informações).
	
	Digite /msg %S HELP SET oção para maiores informações
	sobre um comando específico.

NICK_HELP_SET_DISPLAY
	Sintaxe SET DISPLAY novo-display
	
	Altera o display usado como referência ao nome do seu grupo 
	nos Services. O novo display DEVE ser um nick do seu grupo.

NICK_HELP_SET_PASSWORD
	Sintaxe: SET PASSWORD nova-senha
	
	Muda a senha de seu nick.

NICK_HELP_SET_LANGUAGE
	Sintaxe: SET LANGUAGE número
	
	Muda a linguagem que os Services usam ao enviar mensagens
	para você (por exemplo, quando responde um comando que você
	emite). Lembre-se que apenas a linguagem em Português está
	totalmente atualizada, outras linguagens podem enviar
	mensagens com informações incorretas.
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
	será dado 1 minuto para este mudar de nick, após isso, será
	trocado de nick ou removido do irc pelo %S.
	
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
	constar na lista de acesso.  Entretanto, se você está na
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
	Sintaxe: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Permite que você previna que certas informações sejam
	mostradas quando alguém usa %S INFO no seu nick. Você pode
	esconder seu endereço de e-mail (EMAIL), última máscara vista
	user@host (USERMASK), e sua última mensagem de saida (QUIT).
	O segundo parâmetro especifica quando a informação deve ser
	mostrada (OFF) ou escondida (ON).
	
NICK_HELP_SET_MSG
	Sintaxe: SET MSG {ON | OFF}
	
	Permite que você escolha a forma que os Services devem se 
	comunicar com você. Com MSG acionado, os Services irão usar
	mensagems, caso contrário, usarão notices. 

NICK_HELP_RECOVER
	Syntax: RECOVER nickname [password]
	
	Permite que você recupere seu nick se alguém o tiver
	tomado; isso faz o mesmo que o %S faz automaticamente
	se alguém "rouba" um nick protegido com kill.
	
	Quando você usa este comando, o %S trará um falso
	usuário online com o mesmo nick de quem o está usando.
	Isto causará uma desconexão pelo servidor de IRC do outro
	usuário. O falso usuário ficara online por 1 minuto para
	assegurar que o outro usuário não reconecte imediatamente;
	Durante este minuto, você pode reivindicar seu nick.
	Alternativamente, use o comando RELEASE
	(/msg %S HELP RELEASE) para retomar seu nick mais 
	rapidamente.
	
	Para usar o comando RECOVER com um nick, seu endereço atual
	como mostrado com /WHOIS deve estar na lista de acesso do
	nick, ou você deve fornecer a senha correta para o nick.

NICK_HELP_RELEASE
	Sintaxe: RELEASE nick [senha]
	
	Instrui o %S a remover qualquer espera no seu nick
	causada pelo kill automático de proteção ou pelo uso do
	comando RECOVER. Por padrão, essa espera dura 1 minuto;
	este comando libera o seu nick mais cedo.
	 
	Para usar o comando RELEASE para um nick, seu endereço
	atual	como mostrado com /WHOIS deve estar na lista de acesso 
	do nick, ou você deve fornecer a senha correta para o nick.

NICK_HELP_GHOST
	Sintaxe: GHOST nick [senha]
	 
	Elimina uma sessão "fantasma" de IRC que esteja usando seu
	nick. A sessão "fantasma" é aquela que não está realmente
	conectada, mas que o servidor de IRC acredita estar ainda
	ativa por qualquer razão. Isso acontece normalmente se seu
	computador travar ou se sua conexão com a internet ou modem
	cair por um período curto de tempo enquanto você esta no IRC.
	 
	Para usar o comando GHOST em um nick, seu endereço atual
	como mostrado no /WHOIS deve pertencer a lista de acesso do
	nick, ou você deve fornecer a senha correta para o nick.

NICK_HELP_INFO
	Sintaxe: INFO nick [ALL]
	 
	Mostra a informação sobre o nick dado, como o dono do nick,
	último endereço utilizado, tempo e opções. Se você está
	identificado para o nick que você esta obtendo informações
	e a opção ALL é especificada, será mostrado à você toda
	informação possível; havendo opções para esconder
	informações ou não.

NICK_HELP_LIST
	Sintaxe: LIST padrão
	
	Lista todos os nicks registrados que combinam com o padrão
	fornecido no formato nick!ident@host. Nicks com a opção
	PRIVATE ligada não serão mostrados.

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
	Sintaxe: GLIST
	
	Lista todos os nicks do grupo.

NICK_HELP_STATUS
	Sintaxe: STATUS nick
	 
	Retorna se o usuário que está usando um determinado nick
	é reconhecio como dono do nick. 
	A resposta tem este formato:
	 
	    nickname código-de-status
	 
	onde nick é o nick enviado com o comando, e
	código-de-status um dos seguintes:
	
	    0 - usuário não está conectado ou o nick não está registrado
	    1 - usuário não reconhecido como dono do nick
	    2 - usuário reconhecido como dono só pela lista de acesso
	    3 - usuário reconhecido como dono via identificação por senha
	
	Até 16 nicks podem ser enviados com cada comando; o resto será
	ignorado. Não é gerado mensagem de erro se nenhum nick for fornecido.
	
NICK_HELP_SENDPASS
	Sintaxe: SENDPASS nick
	
	Envia a senha do nick em questão para o endereço de email
	que ele tiver setado para seu nick. Esse comando é bem útil em
	se falando de senhas perdidas.
	
	Poder ser limitado a IRC operators em algumas redes.
	
	Este comando não está disponível quando estiver usando encriptação.

NICK_SERVADMIN_HELP
	
	Os comandos abaixo estão disponíveis para Admins dos Services:
	
	    GETPASS    Recupera a senha de um nick determinado
	                 (apenas se não estiver com encriptação)
	    FORBID     Previne um nick de ser usado ou registrado
	
	Os Administradores podem ainda usar o DROP em qualquer nick
	sem ser necessário se identificar como dono dele (/msg %S 
	ACCESS LIST nick).

NICK_SERVADMIN_HELP_LOGOUT
	Sintaxe: LOGOUT [nick [REVALIDATE]]
	
	Sem nenhum parâmetro ele irá apenas reverter o efeito do comando IDENTIFY 
	
	Com um parâmetro, fará o mesmo para o nick determinado. Se você usar 
	o comando REVALIDATE os Services irão pedir que o nick em questão
	se identifique novamente. 
	
	Comando limitado aos Administradores dos Services.

NICK_SERVADMIN_HELP_DROP
	Sintaxe: DROP [nick]
	
	Sem parâmetros, cancela o registro de seu nick no banco de
	dados do %S.
	
	Com parâmetros, cancela o registro de um nick especificado
	do banco de dados.
	Este comando é restrito aos Administradores dos Services.

NICK_SERVADMIN_HELP_SET
	
	Administradores dos Services podem ajustar a opção NOEXPIRE,
	com a qual os nicks podem ser protegidos de expirar. E ainda,
	Administradores dos Services podem ajustar opções para qualquer
	nick sem se identificar com a senha, usando:
	SET nick opção parametros.

NICK_SERVADMIN_HELP_SET_NOEXPIRE
	Sintaxe: SET nick NOEXPIRE {ON | OFF}
	
	Determina se um nick fornecido poderá ser expirado.
	Ajustando este para ON, evita que o nick seja expirado.
	
	Limitado aos Administradores dos Services.

NICK_SERVADMIN_HELP_INFO
	
	Administradores de Services podem usar o comando ALL com qualquer nick.

NICK_SERVADMIN_HELP_LIST
	Sintaxe: LIST padrão [FORBIDDEN] [NOEXPIRE] [UNCONFIRMED]
	
	Lista todos os nicks registrados que combinem com o padrão
	fornecido (formato nick!ident@host). Nicks com opção PRIVATE
	serão somente exibidos para os Administradores dos Services.
	Nicks com opção NOEXPIRE ajustada terão um ! preposto ao nick
	na lista quando for a pedido de Administrador dos Services.
	
	If the FORBIDDEN, NOEXPIRE or UNCONFIRMED options are given, only
	nicks which, respectively, are FORBIDden, UNCONFIRMED or have the
	NOEXPIRE flag set will be displayed. If all three options are
	given, all three types of nicks will be displayed. These options
	are limited to Services admins.  
	
	Exemplos:
	
	     LIST *!ident@servidor.com.br
	        Lista todos os nicks de *!ident@servidor.com.br.
	
	     LIST *Bot*!*@*
	        Lista todos os nicks registrados com Bot em seus nomes.
	
	     LIST * NOEXPIRE
	        Lista todos os nicks registrados cujos registros foram
	        ajustados para não expirar.

NICK_SERVADMIN_HELP_ALIST
	Syntax: ALIST [nickname] [level]
	
	With no parameters, lists channels you have access on. With
	one parameter, lists channels that nickname has access 
	on. With two parameters lists channels that nickname has 
	level access or greater on.

	This use limited to Services admins.

NICK_SERVADMIN_HELP_GLIST
	Sintaxe: GLIST [nick]
	
	Sem nenhum parâmetro lista todos os nicks no seu grupo.
	
	Com um parâmetro, lista todos os nicks no grupo determinado.
	
	Este comando é limitado aos Administradores dos Services.

NICK_SERVADMIN_HELP_GETPASS
	Sintaxe: GETPASS nick
	
	Mostra a senha de um nick especificado.
	Note que sempre que este comando for usado, uma mensagem
	incluindo o nick da pessoa que emitiu o comando o nick
	requisitado será gravado e enviado para WALLOPS/GLOBOPS.
	
	Limitado aos Administradores dos Services.
	
	Este comando não é disponível quando a encriptação está habilitada.

NICK_SERVADMIN_HELP_GETEMAIL
	Syntax: GETEMAIL user@emailhost

	Returns the matching nicks that used given email. Note that
	you can not use wildcards for either user or emailhost. Whenever
	this command is used, a message including the person who issued
	the command and the email it was used on will be logged.

	Limited to Services admins.

NICK_SERVADMIN_HELP_FORBID
	Sintaxe: FORBID nick
	
	Não permite que um nick seja registrado ou usado por
	ninguém. Pode ser cancelado desregistrando o nick.
	
	Limitado aos Administradores dos Services.

###########################################################################
#
# ChanServ help messages
#
###########################################################################

CHAN_HELP
	%S permite que você registre e controle vários
	aspectos do canal. %S pode prevenir o "take over"
	do canal de usuários maliciosos limitando quem possui
	o privilégio de OP do canal. Os comandos disponíveis
	estão listados abaixo.
	Para usá-los, digite /msg %S comando.
	Para mais informações de um comando, digite
	/msg %S HELP comando.
	
	    REGISTER   Registra um canal
	    IDENTIFY   Identifica seu nick
	    SET        Ajusta as opções e informações do canal
	    ACCESS     Modifica a lista de usuários privilegiados
	    LEVELS     Redefine os níveis de acesso
	    AKICK      Faz a manutenção da Lista de Autokick
	    DROP       Cancela o registro de um canal
	    SENDPASS   Ajuda para recuperar senhas
	
	Outros comandos: CLEAR, DEOP, DEVOICE, GETKEY, INFO, 
	                INVITE, KICK, LIST, LOGOUT, OP, UNBAN, 
	                VOICE
	
CHAN_HELP_UNREAL
	
	Estes comandos estão também disponíveis nesta Rede:
	    HALFOP, DEHALFOP, PROTECT, DEPROTECT

CHAN_HELP_ULTIMATE
	
	Estes comandos estão também disponíveis nesta Rede:
	    HALFOP, DEHALFOP, HOP

CHAN_HELP_ULTIMATE3
	
	Estes comandos estão também disponíveis nesta Rede:
	    HALFOP, DEHALFOP, HOP, ADMIN, DEADMIN

CHAN_HELP_EXPIRES
	
	Note que qualquer canal não utilizado por %d dias
	(entenda como se nenhum usuário da lista de acesso
	desse canal entrar neste periodo de tempo) terá
	automaticamente cancelado o seu registro.

CHAN_HELP_REGISTER
	Sintaxe: REGISTER canal senha descrição
	
	Registra um canal no banco de dados do %S. 
	Para utilizar este comando, você deve primeiro ser OP
	do canal que está tentando registrar. O canal é registrado
	para o seu nick, e você terá acesso de fundador do canal
	quando tiver identificado quando tiver identificado a senha 
	do canal com o %S. Para ajuda sobre como mudar os privilégios 
	de usuários ao seu canal, escreva /msg %S HELP ACCESS.
	
	IMPORTANTE: Para registrar um canal, você deve antes ter
	registrado seu nick. Se ainda não tiver seu nick registrado,
	digite /msg %s HELP para mais informações de como fazê-lo.

CHAN_HELP_IDENTIFY
	Sintaxe: IDENTIFY canal senha
	
	Identifica você no %S como fundador do canal fornecido.
	Muitos comandos exigem que você use este comando antes
	de usá-los. A senha deverá ser a mesma usada por você
	no comando REGISTER.
	
CHAN_HELP_LOGOUT
	Sintaxe: LOGOUT canal nick
	
	Este comando faz que o nick determinado não esteja mais
	reconhecido no canal especificado.
	
	Se você for o dono do canal você pode usar este comando
	em qualquer um, caso contrário, apenas em si mesmo.

CHAN_HELP_DROP
	Sintaxe: DROP #canal
	
	Cancela o registro do canal. So poderá ser usado pelo
	fundador do canal, que primeiramente deverá estar
	identificado com o nickserv.

CHAN_HELP_SET
	Sintaxe: SET canal opção parâmetros
	
	Permite ao fundador do canal ajustar as várias opções do
	canal e outras informações. O fundador deve se identificar
	com o nickserv antes de usar o SET.
	
	Comandos disponiveis:
	
	    FOUNDER       Ajusta o fundador do canal
	    SUCCESSOR     Ajusta o sucessor do canal
	    PASSWORD      Ajusta a senha do founder
	    DESC          Ajusta a descrição do canal
	    URL           Associa uma URL a um canal
	    EMAIL         Associa um e-mail a um canal
	    ENTRYMSG      Ajusta a mensagem que deve ser mandada
	                  aos usuários quando eles entram no canal
	    TOPIC         Altera o tópico do canal
	    BANTYPE       Ajusta com os services devem fazer o ban
	    KEEPTOPIC     Mantem o tópico quando o canal
	                  não está em uso
	    TOPICLOCK     O tópico pode ser mudado apenas
	                  com o TOPIC
	    MLOCK         Liga ou desliga a trava dos modos do canal
	    PRIVATE       Esconde o canal do comando LIST
	    RESTRICTED    Acesso restrito ao canal
	    SECURE        Ativa os recursos de segurança do %S
	    SECUREOPS     Rigoroso o controle de status de OP
	    SECUREFOUNDER Controle restrito do fundador do canal
	    SIGNKICK      Assinatura dos kicks usados pelo comando kick
	    OPNOTICE    Envia uma notice quando os comandos OP/DEOP 
				são usados
	    XOP           Toggle the user privilege system
	
	Digite /msg %S HELP SET opções para maiores informações
	sobre uma opção em particular.

CHAN_HELP_SET_FOUNDER
	Sintaxe: SET canal FOUNDER nick
	
	Muda o fundador do canal. O novo nick deve estar
	registrado.

CHAN_HELP_SET_SUCCESSOR
	Sintaxe: SET canal SUCCESSOR nick
	
	Muda o sucessor de um canal. Se o nick do founder expirar
	ou estiver dropado enquanto o canal ainda estiver registrado,
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
	Sintaxe: SET canal EMAIL [endereco]
	
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
	Sintaxe: SET canal BANTYPE tipo-do-ban
	
	Seleciona o tipo do ban sempre que os Services precisarem
	banir alguém do seu canal.
	
	O tipo do ban é um número entre 0 e 3 que significa:
	
	0: ban na forma *!user@host
	1: ban na forma *!*user@host
	2: ban na forma *!*@host
	3: ban na forma *!*user@*.domain

CHAN_HELP_SET_KEEPTOPIC
	Sintaxe: SET canal KEEPTOPIC {ON|OFF}
	
	Ativa ou desativa a opção de retenção de tópico para 	
	um canal. Quando a retenção de tópico está acionada,
	o tópico para o canal será relembrado pelo %S
	mesmo depois que os usuários deixarem o canal, e será
	restaurado na próxima vez que o canal for re-aberto.

CHAN_HELP_SET_TOPICLOCK
	Sintaxe: SET canal TOPICLOCK {ON|OFF}
	
	Ativa ou desativa a opção de trava do tópico de um canal.
	Quando a Trava do tópico está ativada, o %S não
	permitirá a mudança do tópico do canal exceto pelo comando 
	TOPIC.

CHAN_HELP_SET_MLOCK
	Sintaxe: SET canal MLOCK modos
	
	Ajusta o parâmetro da trava do modo para um canal. O
	%S permite que você defina certos modos no canal
	a serem sempre ativos (ou desativos). Os modos podem 
	ser i, k, l, m, n, p, s e t; cada um desses travado
	como ativo, desativo ou não travado. O parâmetro
	modos e usado exatamente do mesmo modo como o comando
	/MODE;  isto é, modos seguidos de + são travados como
	ativos, e modos seguidos de - são travados como desativos.
	
	Cuidado: Se você trava uma senha, como no segundo exemplo
	abaixo, você deveria também ajustar a opção RESTRICTED para
	o canal (veja HELP SET RESTRICTED), ou qualquer usuário
	que entra no canal que está vazio será capaz de ver a senha!
	
	Exemplos:
	
	    SET #canal MLOCK +nt-iklps
	        Trava os modos n e t ativados, e os modos i, k, l, p,
	        e s desligados. Modo m é livremente deixado para se
	        ligado(on)ou desligado(off).
	
	    SET #canal MLOCK +knst-ilmp minha-senha
	        Trava os modos k, n, s, t como ligados e os modos i,
	        l, m, p como desligados. Também trava a senha do canal
	        como "minha-senha".
	
	    SET #canal MLOCK +
	        Elimina a trava do modos; todos os modos do canal
	        ficam livres para ser ativados ou desativados.

CHAN_HELP_SET_PEACE
	Syntax: SET channel PEACE {ON | OFF}
	
	Enables or disables the peace option for a channel.
	When peace is set, an user won't be able to kick,
	ban or remove a channel status of an user that has
	a level superior or equal to his via %S commands.

CHAN_HELP_SET_PRIVATE
	Sintaxe: SET canal PRIVATE {ON|OFF}
	
	Ativa ou desativa a opção private para um canal. Quando
	private está ativada, um /msg %S LIST não irá incluir
	o canal em qualquer listagem.

CHAN_HELP_SET_RESTRICTED
	Sintaxe: SET canal RESTRICTED {ON|OFF}
	
	Ativa ou desativa a opção de acesso restrito para um canal.
	Quando o acesso restrito estiver ativado, usuários que
	normalmente não possuem privilegios de OP no canal
	(usuários com nível negativo de acesso e, se secure ops
	estiver ativado, usuários que não pertence a lista de
	acesso) serão imediatamente kickados e banidos do canal.

CHAN_HELP_SET_SECURE
	Sintaxe: SET canal SECURE {ON|OFF}
	
	Ativa ou desativa os recursos de segurança do %S
	para um canal. Quando o SECURE estiver ativo, somente
	usuários que possuirem seus nicks registrados no %s
	e identificados com sua senha poderão entrar no canal
	controlado pela lista de acesso.

CHAN_HELP_SET_SECUREOPS
	Sintaxe: SET canal SECUREOPS {ON|OFF}
	
	Ativa ou desativa a opção de segurança de ops para um canal.
	Quando secure-ops estiver ativada, os usuários que nao
	estiverem na lista de acesso não poderão ter o status de OP.
	
CHAN_HELP_SET_SECUREFOUNDER
	Syntax: SET canal SECUREFOUNDER {ON | OFF}
	
	Habilita ou desabilitada a opção secure founder para um canal.
	Quando o secure founder está ligado, apenas o verdadeiro founder
	poderá dar DROP no canal, mudar sua senha, seu founder e seu
	successor, e não aqueles que estiverem com IDENTIFY no %S.
	
CHAN_HELP_SET_SIGNKICK
	Sintaxe: SET canal SIGNKICK {ON | LEVEL | OFF}
	
	Habilita ou desabilita a assinatura nos kicks para um
	canal.  Quando o SIGNKICK está habilitado, kicks através
	do comando %S KICK terão o nick que usou o comando no motivo
	do kick.
	
	Se você usar LEVEL, aqueles que tiverem um level igual ou 
	superior ao level do SIGNKICK não terão o nick aparecendo no 
	motivo. Veja /msg %S HELP LEVELS para maiores informações. 

CHAN_HELP_SET_XOP
	Syntax: SET channel XOP {ON | OFF}
	
	Enables or disables the xOP lists system for a channel.
	When XOP is set, you have to use the AOP/SOP/VOP
	commands in order to give channel privileges to
	users, else you have to use the ACCESS command.
	
	Technical Note: when you switch from access list to xOP 
	lists system, your level definitions and user levels will be
	changed, so you won't find the same values if you
	switch back to access system! 
	
	You should also check that your users are in the good xOP 
	list after the switch from access to xOP lists, because the 
	guess is not always perfect... in fact, it is not recommended 
	to use the xOP lists if you changed level definitions with 
	the LEVELS command.
	
	Switching from xOP lists system to access list system
	causes no problem though.

CHAN_HELP_SET_OPNOTICE
	Sintaxe: SET canal OPNOTICE {ON | OFF}
	
	Ativa e desativa a opção Op-Notice para um canal. Quando
	Op-Notice estiver ativado, o %S enviará uma notice para
	o canal sempre que os comandos OP ou DEOP forem usados em um
	usuário no canal.

CHAN_HELP_AOP
	Syntax: AOP channel ADD nick
	        AOP channel DEL {nick | entry-num | list}
	        AOP channel LIST [mask | list]
	        AOP channel CLEAR
	
	Maintains the AOP (AutoOP) list for a channel. The AOP 
	list gives users the right to be auto-opped on your channel,
	to unban or invite themselves if needed, to have their
	greet message showed on join, and so on.
	
	The AOP ADD command adds the given nickname to the
	AOP list.
	
	The AOP DEL command removes the given nick from the
	AOP list.  If a list of entry numbers is given, those
	entries are deleted.  (See the example for LIST below.)
	
	The AOP LIST command displays the AOP list.  If
	a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   AOP #channel LIST 2-5,7-9
	      Lists AOP entries numbered 2 through 5 and
	      7 through 9.
	      
	The AOP CLEAR command clears all entries of the
	AOP list.
	
	The AOP ADD and AOP DEL commands are limited to
	SOPs or above, while the AOP CLEAR command can only
	be used by the channel founder. However, any user on the
	AOP list may use the AOP LIST command.
	
	This command may have been disabled for your channel, and
	in that case you need to use the access list. See 
	/msg %S HELP ACCESS for information about the access list,
	and /msg %S HELP SET XOP to know how to toggle between 
	the access list and xOP list systems.

CHAN_HELP_HOP
	Syntax: HOP channel ADD nick
	        HOP channel DEL {nick | entry-num | list}
	        HOP channel LIST [mask | list]
	        HOP channel CLEAR
	
	Maintains the HOP (HalfOP) list for a channel. The HOP 
	list gives users the right to be auto-halfopped on your 
	channel.
	
	The HOP ADD command adds the given nickname to the
	HOP list.
	
	The HOP DEL command removes the given nick from the
	HOP list.  If a list of entry numbers is given, those
	entries are deleted.  (See the example for LIST below.)
	
	The HOP LIST command displays the HOP list.  If
	a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   HOP #channel LIST 2-5,7-9
	      Lists HOP entries numbered 2 through 5 and
	      7 through 9.
	      
	The HOP CLEAR command clears all entries of the
	HOP list.
	
	The HOP ADD, HOP DEL and HOP LIST commands are 
	limited to AOPs or above, while the HOP CLEAR command 
	can only be used by the channel founder.
	
	This command may have been disabled for your channel, and
	in that case you need to use the access list. See 
	/msg %S HELP ACCESS for information about the access list,
	and /msg %S HELP SET XOP to know how to toggle between 
	the access list and xOP list systems.

CHAN_HELP_SOP
	Syntax: SOP channel ADD nick
	        SOP channel DEL {nick | entry-num | list}
	        SOP channel LIST [mask | list]
	        SOP channel CLEAR
	
	Maintains the SOP (SuperOP) list for a channel. The SOP 
	list gives users all rights given by the AOP list, and adds
	those needed to use the AutoKick and the BadWords lists, 
	to send and read channel memos, and so on.
	
	The SOP ADD command adds the given nickname to the
	SOP list.
	
	The SOP DEL command removes the given nick from the
	SOP list.  If a list of entry numbers is given, those
	entries are deleted.  (See the example for LIST below.)
	
	The SOP LIST command displays the SOP list.  If
	a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   SOP #channel LIST 2-5,7-9
	      Lists AOP entries numbered 2 through 5 and
	      7 through 9.
	      
	The SOP CLEAR command clears all entries of the
	SOP list.
	
	The SOP ADD, SOP DEL and SOP CLEAR commands are 
	limited to the channel founder. However, any user on the
	AOP list may use the SOP LIST command.
	
	This command may have been disabled for your channel, and
	in that case you need to use the access list. See 
	/msg %S HELP ACCESS for information about the access list,
	and /msg %S HELP SET XOP to know how to toggle between 
	the access list and xOP list systems.

CHAN_HELP_VOP
	Syntax: VOP channel ADD nick
	        VOP channel DEL {nick | entry-num | list}
	        VOP channel LIST [mask | list]
	        VOP channel CLEAR
	
	Maintains the VOP (VOicePeople) list for a channel.  
	The VOP list allows users to be auto-voiced and to voice 
	themselves if they aren't.
	
	The VOP ADD command adds the given nickname to the
	VOP list.
	
	The VOP DEL command removes the given nick from the
	VOP list.  If a list of entry numbers is given, those
	entries are deleted.  (See the example for LIST below.)
	
	The VOP LIST command displays the VOP list.  If
	a wildcard mask is given, only those entries matching the
	mask are displayed.  If a list of entry numbers is given,
	only those entries are shown; for example:

	   VOP #channel LIST 2-5,7-9
	      Lists VOP entries numbered 2 through 5 and
	      7 through 9.
	      
	The VOP CLEAR command clears all entries of the
	VOP list.
	
	The VOP ADD, VOP DEL and VOP LIST commands are 
	limited to AOPs or above, while the VOP CLEAR command 
	can only be used by the channel founder.
	
	This command may have been disabled for your channel, and
	in that case you need to use the access list. See 
	/msg %S HELP ACCESS for information about the access list,
	and /msg %S HELP SET XOP to know how to toggle between 
	the access list and xOP list systems.

CHAN_HELP_ACCESS
	Sintaxe: ACCESS canal ADD nick nível
	         ACCESS canal DEL {nick | entry-num | list}
	         ACCESS canal LIST [máscara | list]
	
	Faz manutenção da lista de acesso de um canal. A lista
	de acesso especifica que usuários tem permissão para
	0 status de OP ou tem acesso aos comandos do %S em
	um canal. Níveis diferentes de usuários permitem acesso
	a diferentes subníveis de privilégios; /msg %S HELP
	ACCESS LEVELS para mais informações específicas. Por
	padrão, qualquer nick não presente na lista de acesso
	possui nivel 0.
	
	O comando ACCESS ADD adiciona um nick ou máscara de um
	usuário na lista de acesso com o nível especificado; se o
	nick ou máscara já estiver presente na lista, seu nível de
	acesso será alterado para o nível especificado. O nivel
	especificado deve ser menor do que o usuário que emitiu o
	comando.
	
	O comando ACCESS DEL remove um nick especificado da lista
	de acesso. Se uma lista de números é fornecida, essas
	entradas são apagadas. (Veja o exemplo para LIST abaixo.)
	
	O comando ACCESS LIST mostra a lista de acesso do canal.
	Se uma máscara for fornecida, somente aquelas entradas que
	combinarem com o padrão especificado na máscara serão 
	mostradas. Se uma lista de números é fornecida, somente
	essas entradas serão mostradas, por exemplo:
	
	ACCESS #canal LIST 2-5,7-9
	   Lista as entradas de acesso numeradas de 2 a 5 e 7 a 9.

CHAN_HELP_ACCESS_LEVELS
	Níveis de Acesso do Usuário
	
	Por padrão, os seguintes níveis de acesso são definidos:
	
	   Founder	Acesso total as funções do %S; OP automático
	                  quando entrar no canal. Note que de somente uma
	                  pessoa pode ter o status fundador (isto não pode
	                  ser dado usando o comando ACCESS).
	        10      Acesso ao comando AKICK; OP automático.
	         5      OP automático.
	         3      Voice automático.
	         0      Sem privilégios especiais; pode se tornar OP
	                  através de outros ops (a menos que secure-ops
	                  esteja ativada).
	        <0      Não pode ser OP.
	
	Esses níveis podem ser mudados, ou adicionar novos, usando o
	comando LEVELS; digite /msg %S HELP LEVELS para informações.

CHAN_HELP_AKICK
	Sintaxe: AKICK canal ADD máscara [razão]
	         AKICK canal DEL máscara
	         AKICK canal LIST [máscara]	
	         AKICK canal ENFORCE
	   	
	Faz a manutenção da lista de Autokick de um canal.
	Se um usuário que estiver na lista de AutoKick tentar
	entrar no canal, o %S banirá este usuário do 
	canal, e depois kickará o usuário.
	
	O comando AKICK ADD adiciona o usuário especificado ou
	máscara na lista de Autokick. Se uma razão é dada com
	o comando, esta razão será usada quando o usuário for
	kickado; se não, a razão padrão será "Você foi banido
	deste canal" via o %S.
	
	O comando AKICK DEL remove um nick ou máscara da lista
	de AutoKick. Isso, entretanto, não remove qualquer ban
	colocado pelo AutoKick; estes devem ser removidos
	manualmente.
	
	O comando AKICK LIST mostra a lista de AutoKick, ou
	opcionalmente, só os registros de AutoKick que se
	encaixam com o padrão de uma máscara dada.
	
	O comando AKICK ENFORCE obriga o %S a verificar
	a lista de AKICKs para remover os usuários conectados que
	sejam compatíveis com a máscara dada.

CHAN_HELP_LEVELS
	Sintaxe: LEVELS canal SET tipo nível
	         LEVELS canal {DIS | DISABLE} tipo
	         LEVELS canal LIST
	         LEVELS canal RESET
	
	O comando LEVELS permite ajuste detalhado sobre o
	significado numérico dos níveis de acesso usados pelos
	canais. Com esse comando, você pode definir o nível de
	acesso requerido pela maioria das funções do %S.
	(Os comandos SET FOUNDER e SET PASSWORD, assim como
	este comando, são sempre restritos ao fundador do canal).
	
	LEVELS SET permite que o nível de acesso para uma função ou
	grupo de funções sejam alterados. LEVELS DISABLE (ou DIS
	para encurtar) desabilita automaticamente os dispositivos ou
	não permite o acesso a funções para qualquer outra pessoa que
	não seja o fundador do canal. LEVELS LIST mostra os níveis
	atuais para cada função ou para um grupo de funções. LEVELS
	RESET recoloca os níveis de acordo com o padrão de um canal
	recem-criado (veja HELP ACCESS LEVELS).
	
	Para lista de recursos e funções cujos níveis podem ser
	ajustados, veja HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	A seguinte característica/função são entendidas. Note que
	estes níveis para AUTODEOP e NOJOIN são os níveis máximos,
	enquanto que todos os outros são níveis mínimos.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Sintaxe: INFO canal [ALL]
	
	Lista os dados referentes ao canal registrado dado,
	incluindo seu fundador, data do registro, última vez que
	foi usado, descrição, e trava dos modos (se existirem).
	Se você está identificado como fundador do canal que está
	solicitando informações e a opção ALL é especificada,
	informações sobre a mensagem de entrada e sobre o sucessor
	do canal também será mostrada.

CHAN_HELP_LIST
	Sintaxe: LIST padrão
	
	Lista todos os canais registrados que combinam com o padrão
	dado. (Canais com opção PRIVATE ativada não serão listados).

CHAN_HELP_OP
	Sintaxe: OP #canal nick
	
	Atribui status de Operador (modo +o) para o nick dado em
	um determinado canal. Limitado aos usuários de nível 5, no
	mínimo, do canal especificado.

CHAN_HELP_DEOP
	Sintaxe: DEOP #canal nick
	
	Remove o status de Operador (modo +o) do nick dado para
	um determinado canal. Limitado aos usuários de nível 5,
	no mínimo, do canal especificado.
	
CHAN_HELP_VOICE
	Sintaxe: VOICE [#canal [nick]]
	
	Dá voice para um determinado nick no canal.

CHAN_HELP_DEVOICE
	Sintaxe: DEVOICE [#canal [nick]]
	
	Tira o voice de um determinado nick no canal.
	
CHAN_HELP_HALFOP
	Sintaxe: HALFOP [#canal [nick]]
	
	Halfop um nick em um determinado canal.

CHAN_HELP_DEHALFOP
	Sintaxe: DEHALFOP [#canal [nick]]
	
	Dehalfop um nick em um determinado canal.

CHAN_HELP_PROTECT
	Sintaxe: PROTECT [#canal [nick]]
	
	Protege um determinado nick no canal.

CHAN_HELP_DEPROTECT
	Sintaxe: DEPROTECT [#canal [nick]]
	
	Desprotege um nick em um determinado canal.

CHAN_HELP_OWNER
	Syntax: OWNER [#channel]
	
	Gives you owner status on channel. If channel is not 
	given, it will give you owner status on all channels you're 
	on, provided you have the rights to.
	
	Limited to those with founder access on the channel.

CHAN_HELP_DEOWNER
	Syntax: DEOWNER [#channel]
	
	Removes your owner status on channel. If channel is 
	not given, it will remove your owner status on all channels 
	you're on, provided you have the rights to.
	
	Limited to those with founder access on the channel.

CHAN_HELP_INVITE
	Sintaxe: INVITE #canal
	
	Manda o %S dar um invite em você para o canal dado.
	Somente funcionará se seu nível de acesso for maior
	que zero.

CHAN_HELP_UNBAN
	Sintaxe: UNBAN #canal
	
	Diz ao %S para remover todos os bans permitindo que
	você entre em um canal. Por padrão, somente funciona se
	seu nível de acesso no canal for maior que zero.
	
CHAN_HELP_KICK
	Sintaxe: KICK [#canal [nick [motivo]]]
	
	Kicka um determinado nick no canal.

CHAN_HELP_BAN
	Syntax: BAN [#channel [nick [reason]]]
	
	Bans a selected nick on a channel. If nick is not given,
	it will ban you. If channel and nick are not given,
	it will ban you on all channels you're on, provided you
	have the rights to.
	
	By default, limited to AOPs or those with level 5 access 
	and above on the channel.

CHAN_HELP_TOPIC
	Syntax: TOPIC channel [topic]
	
	Causes %S to set the channel topic to the one
	specified. If topic is not given, then an empty topic
	is set. This command is most useful in conjunction
	with SET TOPICLOCK. See /msg %S HELP SET TOPICLOCK
	for more information.
	
	By default, limited to those with founder access on the
	channel. 

CHAN_HELP_CLEAR
	Sintaxe: CLEAR canal opção
	
	Diz ao %S para remover certas colocações de um canal.
	A opção pode ser uma das seguintes:
	
	     MODES    Anula todos os modos do canal (isto é, remove
	              os modos i,k,l,m,n,p,s,t).
	     BANS     Remove todos os bans do canal.
	     EXCEPTS  Remove todos os excepts do canal.
	     OPS      Remove o status de Operador do canal (mode +o)
	              de todos os Operadores do canal.
	     HOPS     Removes channel-halfoperator status (mode +h) from
	              all channel halfoperators, if supported.
	     VOICES   Remove o status "voice" (mode +v) de qualquer
	              pessoa que tenha esse modo.
	     USERS    Remove (kicka) todos os usuários do canal.
	
	Por padrão, seu nível de acesso no canal dever ser no
	mínimo 10 para usar este comando.
	
CHAN_HELP_GETKEY
	Sintaxe: GETKEY canal
	
	Retorna a key de um determinado canal. Este comando é
	geralmente usado por bots e/ou scripts, da seguinte forma:
	
	KEY <canal> <key>
	
	key é "NENHUM KEY" se nenhuma key estiver gravada.
	
CHAN_HELP_SENDPASS
	Sintaxe: SENDPASS canal
	
	Envia a senha de um determinado canal para o email gravado
	como email do canal no ChanServ. Este comando é realmente 
	útil quando se trata de senhas perdidas.
	
	Pode estar limitado aos IRC operators em algumas Redes.
	
	Este comando só funciona se o modo de encriptação não estiver
	ligado.

CHAN_SERVADMIN_HELP
	
	Os seguintes comandos estao disponíveis para
	os Administradores dos Services:
	
	    GETPASS    Recupera a senha de um canal
	               (caso mode de encriptação não seja usado)
	    FORBID     Previne um canal de ser registrado/usado
	    STATUS     Fornece o atual nível de acesso de um
	               usuário em um canal
	    CLIST      Mostra todos canais que um determinado user
	               é founder.
	
	Os Administradores dos Services podem também usar o comando
	DROP mesmo sem se identificarem como donos do cnaal, e podem
	também ver as listas de acesso, AKICK, e levels de qualquer
	canal.
	
CHAN_SERVADMIN_HELP_LOGOUT
	Sintaxe: LOGOUT canal [nick]
	
	Faz com que um determinado nick não esteja mais identificado
	em um determinado canal.
	
	Se você for o founder do canal pode usar este comando em
	qualquer usuário e em si mesmo.

CHAN_SERVADMIN_HELP_DROP
	Sintaxe: DROP canal
	
	Cancela o registro de um canal. Somente Administradores dos
	Services podem cancelar canais que pertencem a outros usuários.

CHAN_SERVADMIN_HELP_SET
	
	Administradores dos Services também pode ajustar a opção 
	NOEXPIRE,	fazendo com que canais sejam impedidos de ter 
	seu registro expirado. AdministradoresRoot dos Services 
	ainda podem ajustar opções para qualquer canal de outros usuários.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Sintaxe: SET canal NOEXPIRE {ON | OFF}
	
	Determina se o canal poderá ter seu registro expirado.
	Ajustando para ON, evita que o canal seja desregistrado
	por passar do tempo de expiração.
	
	Limitado aos Administradores dos Services.

CHAN_SERVADMIN_HELP_INFO
	
	Administradores de Services podem usar o comando ALL com qualquer canal.

CHAN_SERVADMIN_HELP_LIST
	Sintaxe: LIST padrao
	
	Lista todos os canais registrados que combinam com o
	padrão fornecido. Canais com opção PRIVATE ativada
	só serão mostrados aos Administradores dos Services.

	Sintaxe: GETPASS canal
	
	Retorna a senha de um canal. Note que toda vez que esse
	este comando for usado, uma mensagem incluindo o nick da
	pessoa que emitiu o comando e o canal que foi dado serão
	gravados e enviados para WALLOPS/GLOBOPS.
	
	Limitado aos Administradores dos Services.

CHAN_SERVADMIN_HELP_GETPASS
	Syntax: GETPASS channel
	
	Returns the password for the given channel.  Note that
	whenever this command is used, a message including the
	person who issued the command and the channel it was used
	on will be logged and sent out as a WALLOPS/GLOBOPS.
	
	Limited to Services admins.

CHAN_SERVADMIN_HELP_FORBID
	Sintaxe: FORBID canal
	
	Proibe que qualquer pessoa registre e utilize o canal
	dado.  Pode ser cancelado pelo desregistro do canal.
	
	Limitado aos Administradores dos Services.

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
	Retorna o nível de acesso atual de um determinado nick em um
	determinado canal. A resposta será no formato:
	
	    STATUS canal nick nível-de-acesso
	
	Se um erro ocorrer, a resposta será no formato:
	
	    STATUS ERROR mensagem-de-erro
	
	Limitado aos Administradores dos Services

###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP
	%S eh um utilitario que permite aos usuarios de IRC
	enviar curtas mensagens a outros usuarios, mesmo
	que eles estejam conectados no momento ou nao; ou
	nos canais(*).  Ambos, destinatarios e remetentes,
	devem possuir seu nick (ou canal) registrados para
	que possam enviar uma mensagem (memo).
	Os comandos do %S sao:
	
	    SEND   Envia uma mensagem para um nick ou para um canal
	    CANCEL Cancela o último memo enviado por você
	    LIST   Lista suas mensagens
	    READ   Le a(s) mensagem(ns)
	    DEL    Apaga a(s) mensagem(ns)
	    SET    Ajusta opcoes relacionadas as mensagens
	    INFO   Displays information about your memos
	
MEMO_HELP_ADMIN
	    SENDALL  Send a memo to all registered users

MEMO_HELP_OPER
	    STAFF  Send a memo to all opers/admins

MEMO_HELP_FOOTER
	
	Type /msg %S HELP command for help on any of the
	above commands.

	(*) By default, any user with at least level 10 access on a
	    channel can read that channel's memos.  This can be
	    changed with the %s LEVELS command.

MEMO_HELP_SEND
	Sintaxe: SEND {nick | canal} mensagem
	
	Envia um memo ao nick ou ao canal determinado contendo
	a mensagem.  Quando enviada para o destinatario, este
	recebera um aviso que possui uma nova mensagem.  O nick/
	/canal de destino devera estar registrado.
	
MEMO_HELP_CANCEL
	Sintaxe: CANCEL {nick | canal}
	
	Cancela a última mensagem enviada para um determinado nick ou
	canal caso ele ainda não tenha lido no momento que este comando
	for usado.

MEMO_HELP_LIST
	Sintaxe: LIST [canal] [list | NEW]
	
	Lista quaisquer mensagens que voce tiver. Com NEW, lista apenas
	as novas mensagens(nao lidas). Mesangem nao lidas sao marcadas
	com "*" a esquerda do numero. Voce tambem pode especificar uma
	lista de numeros, como no exemplo abaixo:
	
	LIST 2-5,7-9
	   Lista mensagens numeradas de 2 a 5 e 7 a 9.

MEMO_HELP_READ
	Sintaxe: READ [canal] {num | list | LAST | NEW}
	
	Mostra as mensagens especificadas. Se LAST eh fornecido, envia
	a voce a mensagem recebida mais recentemente. Se NEW eh fornecido,
	envia a voce todas as suas novas mensagens. Caso contrario, envia
	a voce uma mensagem numerada. Voce tambem pode fornecer uma lista
	de numeros, como por exemplo:
	
	READ 2-5,7-9
	   Mostra as mensagens numeradas de 2 a 5 e 7 a 9.

MEMO_HELP_DEL
	Sintaxe: DEL [canal] {num | list | LAST | ALL}
	
	Apaga a(s) mensagem(ns) especificada(s). Voce pode fornecer
	varios numeros de mensagens ou sequencias de numeros ao inves
	de um unico numero, como no segundo exemplo abaixo.

	If LAST is given, the last memo will be deleted.
	Se ALL eh fornecido, apaga todas suas mensagens.
	
	Exemplos:
	
	   DEL 1
	      Apaga sua primeira mensagem.
	
	   DEL 2-5,7-9
	      Apaga mensagens numeradas de 2 a 5 e de 7 a 9.

MEMO_HELP_SET
	Sintaxe: SET opcao parametros
	
	Ajusta varias opcoes de mensagens.  opcao pode ser uma dessas:
	
	    NOTIFY      Muda quando voce for notificado sobre novas
	                    mensagens (so para nicks)
	    LIMIT       Ajusta o numero maximo de mensagens que voce
	                    pode receber.
	
	Digite /msg %S HELP SET <comando>, para mais informacoes sobre
	um comando especifico.

MEMO_HELP_SET_NOTIFY
	Sintaxe: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Mudancas que ocorrem quando voce for notificado sobre novas
	mensagens:
	
	    ON      Voce soh sera notificado sobre mensagens quando voce
	                conectar, quando voltar de um /AWAY e quando
	                sao enviadas a voce.
	    LOGON   Voce soh sera notificado sobre mensagens quando voce
	                conectar ou quando voltar de um /AWAY.
	    NEW     Voce soh sera notificado sobre mensagens quando
	                estiverem enviando para voce.
	    OFF     Voce nao recebera nenhuma notificacao sobre mensagens.
	
	ON eh essencialmente a combinacao LOGON e NEW.

MEMO_HELP_SET_LIMIT
	Sintaxe: SET LIMIT [canal] limite
	
	Ajusta o numero maximo de mensagens que voce pode ter
	(ou do canal fornecido).  Se voce ajustar para 0, ninguem
	sera capaz de enviar mensagens para voce.  Entretanto,
	voce nao pode ajustar este limite para mais que %d.

MEMO_HELP_INFO
	Sintaxe: INFO [canal]
	
	Informa o numero de mensagens que voce tem, quantas
	mensagens nao foram lidas, e o total de quantas
	mensagens voce pode receber.
	Com o parametro, mostra a mesma informacao para o canal
	fornecido.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Sintaxe: SET LIMIT [usuario | canal] {limite | NONE} [HARD]
	
	Ajusta o numero maximo de mensagens que um nick ou canal podem
	ter.  Ajustando o limite para 0, previne o usuario de receber
	qualquer mensagem; colocando o limite em NONE permite que o
	usuario receba e mantenha quantas mensagens quiser.  Se voce
	nao fornecer o nick ou canal, seu proprio limite sera ajustado.
	
	Adicionando HARD previne que o usuario mude o limite.
	Nao colocar HARD tem o efeito contrario, permitindo
	que o usuario mude o limite (mesmo se um limite anterior
	foi ajustado com HARD).
	
	O uso do comando SET LIMIT eh limitado aos Administradores
	dos Services.  Outros usuarios somente ajustarao seus
	proprios limites ou de um canal onde tenham privilegios
	para isso, nao podem remover seu proprio limite, nao
	podem ajustar o limite acima de %d, e nao podem ajustar
	um limite maximo (HARD).

MEMO_SERVADMIN_HELP_INFO
	Sintaxe: INFO [nick | canal]
	
	Sem um parametro, informa o numero de mensagens que voce
	tem, quantas mensagens ainda nao foram lidas, e o total
	de quantas mensagens voce pode receber.
	
	Com o parametro do canal, mostra a mesma informacao sobre
	um canal.
	
	Com o parametro de nick, mostra a mesma informacao sobre
	um nick.  Esta derivacao do comando eh limitada aos
	Administradores dos Services.

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

OPER_HELP
	Comandos do %S:
	    GLOBAL      Envia uma mensagem para todos usuários
	    STATS       Mostra estatísticas dos Services e da Rede
	    OPER LIST   Lista todos os Operadores dos Services
	    ADMIN LIST  Lista todos os Administradores dos Services
	    STAFF       Display Services staff and online status

OPER_HELP_OPER_CMD
	Comandos disponíveis para Operadores e Admins dos Services:
	    MODE        Muda o modo de um canal
	    KICK        Kicka um usuário de um canal
	    CLEARMODES  Limpa os modos de um canal
	    AKILL       Manipula a Lista de AKILL
	    SGLINE      Manipula a lista de SGLINE
	    SZLINE      Manipula a lista de SZLINE

OPER_HELP_ADMIN_CMD
	Comandos disponíveis para os Administradores dos Services:
	    SESSION     Mostra a lista de conexões
	    EXCEPTION   Modifica a lista de limite de sessões
	    KILLCLONES  Derruba todos os usuário de um mesmo host
	    CHANLIST    Lista todos os records de um canal
	    USERLIST    Lista todos os records dos usuários
	    OPER        Modifica a lista de Operadores dos Services
	    LOGONNEWS   Define mensagens a serem mostradas na conexão
	    RANDOMNEWS  Define mensagens a serem mostradas ao azar na conexão
	    OPERNEWS    Defini mensagens a serem mostradas aos ircops
	    NOOP        Remove temporariamente as O:lines de um 
	                determinado server remotamente
	    JUPE        Jupa um servidor
	    SET         Configura varias opções globais dos Services
	    UPDATE      Salva os dados atuais
	    RELOAD      Recarrega o arquivo de configuração
	    QUIT        Retira os services da rede sem salvar os dados
	    RESTART     Salva os dados e reinicia os Services
	    SHUTDOWN    Termina os Services salvando os dados
	
	Comandos disponíveis para Super usuários dos Services:
	    ADMIN       Modifica lista de Administradores dos Services

OPER_HELP_ROOT_CMD
	Commands available to Serivces Roots only:
	    MODLOAD     Load a module
	    MODUNLOAD   Un-Load a module
	    MODLIST     List loaded modules
	    MODINFO     Info about a loaded module

OPER_HELP_LOGGED
	Atenção: Todos os comandos enviados para o %S são logados!

OPER_HELP_GLOBAL
	Sintaxe: GLOBAL mensagens
	
	Permite que os IRCops enviem mensagens para todos os usuários
	da Rede. A menssagen sera enviada por %s.

OPER_HELP_STATS
	Sintaxe: STATS [AKILL | ALL]
	
	Sem nenhuma opcao, mostra o numero atual de usuarios e
	IRCops online (excluindo Services), o numero maximo de
	usuarios online conectados simultaneamente e o tempo
	de duracao desde que a secao dos Services comecou a funcionar.
	
	Com a opcao AKILL, mostra o tamanho atual da lista de
	AKILL e o tempo default para um AKILL expirar.
	
	A opcao ALL eh disponivel somente para os Administradores
	dos Services, e mostra informacoes sobre o consumo de memoria
	dos Services.  Usando este comando, pode-se congelar os
	Services por um curto periodo de tempo em redes grandes,
	entao nao abuse deste comando!
	
	UPTIME pode ser usado como um sinonimo para STATS.

OPER_HELP_OPER
	Sintaxe: OPER ADD nick
	         OPER DEL nick
	         OPER LIST
	
	Permite os Administradores dos Services adicionar ou remover
	nicks da lista de Operadores dos Services.  O usuario que
	tem o nick na lista de Operadores dos Services e que esta
	identificado no %s eh capaz de acessar os comandos de
	Operador dos Services.
	
	Qualquer IRCop pode usar o OPER LIST.
	Todos as outras derivacoes do comando OPER sao limitados
	aos Administradores dos Services.

OPER_HELP_ADMIN
	Sintaxe: ADMIN ADD nick
	         ADMIN DEL nick
	         ADMIN LIST
	
	Permite que os super-usuarios dos Services adicionem ou
	removam nicks da lista de Administradores dos Services.
	O usuario que tem o nick na lista de Administradores dos
	Services e que ja tenham se identificado no %s sera
	capaz de ter acesso aos comandos de Administradores dos
	Services.
	
	Qualquer IRCop pode usar o comando ADMIN LIST.
	Todas as outras derivacoes do ADMIN sao limitadas ao
	Super-usuario (root) dos Services

OPER_HELP_IGNORE
	Syntax: IGNORE {ADD|DEL|LIST} [time] [nick]

	Allows Services Admins to make Services ignore an user for
	a certain time or until the next restart. The default time
	format is seconds. You can specify it by using units. Valid
	units are: s for seconds, m for minutes, h for hours and
	d for days. Combinations of these units are not permitted. To 
	make Services permanently ignore the user type 0 as time.

OPER_HELP_MODE
	Sintaxe: MODE canal modos
	
	Permite que os Operadores dos Services ajustem os modos de
	qualquer canal.  Os parametros sao os mesmos que o do
	comando /MODE padrao.
	
	Limitado aos Operadores dos Services.

OPER_HELP_UMODE
	Syntax: UMODE user modes
	
	Allows Services admins to set user modes for any user.
	Parameters are the same as for the standard /MODE
	command.
	
	Limited to Services admins.

OPER_HELP_OLINE
	Syntax: OLINE user flags
	
	Allows Services admins to give Operflags to any user.
	Flags have to be prefixed with a "+" or a "-". To
	remove all flags simply type a "-" instead of any flags.

	Limited to Services admins.

OPER_HELP_CLEARMODES
	Sintaxe: CLEARMODES canal [ALL]
	
	Limpa todos os modos binarios (i,k,l,m,n,p,s,t) e bans de um
	canal.  Se ALL eh fornecido, limpa tambem todos os ops e
	voices (modos +o e +v) de um canal.
	
	Limitado aos Operadores dos Services.

OPER_HELP_KICK
	Sintaxe: KICK canal usuario razao
	
	Permite IRCops kickar qualquer usuario de um canal.
	Paramentros sao os mesmos que comando /KICK padrao.
	A mensagem do kick tem o nick do IRCop que envia o
	comando KICK, por exemplo:
	
	*** SpamMan has been kicked off from channel #bras por %S (Bras (Flood))
	
	Limitado aos Operadores dos Services.

OPER_HELP_SVSNICK
	Syntax: SVSNICK nick newnick
	
	Forcefully changes a user's nickname from nick to newnick.
	
	Limited to Services admins

OPER_HELP_AKILL
	Sintaxe: AKILL ADD [+tempo] mascara razao
	         AKILL DEL mascara
	         AKILL LIST [mascara]
	         AKILL VIEW [mascara]
	
	Permite que Operadores dos Services manipulem a lista de
	AKILL.  Se um usuario que tenta conectar possui uma mascara
	prevista na lista de AKILL, os Services emitem um KILL para
	aquele usuario, para servidores que suporta, e instrui todos
	os servidores da rede a adicionar um ban (K-line) para a
	mascara do usuario encontrado.
	
	AKILL ADD adiciona uma mascara ident@host na lista de AKILL
	para uma dada razao (deve ser posta obrigatoriamente).
	AKILL DEL remove uma dada mascara da lista de AKILL, se esta
	estiver presente.  AKILL LIST mostra todos os AKILLs atuais;
	se uma mascara opcional for fornecida, a listagem sera
	limitada aquela mascara.  AKILL VIEW eh uma versao mais
	detalhada do AKILL LIST e mostrara quem adicionou o AKILL, a
	data que foi adicionado, e quando sera expirado, como tambem
	a mascara (ident@host) e a razao.
	
	Opcionalmente, um tempo para o akill expirar pode ser
	fornecido com o comando AKILL ADD.  O tempo precede a
	mascara ident@host, e eh especificado como um numero inteiro
	seguido por um dos d (dias), h (horas) ou m (minutos).
	Combinacoes (como de 1h30m) nao sao permitidas.  Se uma
	unidade especifica nao eh incluida, o default e em dias
	(entao +30 significa 30 dias). Para adicionar um AKILL que
	nao expira, use +0.  Se a mascara a ser adicionada comecar
	com um +, o tempo do AKILL deve ser fornecido, ate mesmo se
	este tempo for igual ao default.  O atual tempo de expiracao
	de AKILLs pode ser encontrado com o comando STATS AKILL.
	
	A razao existe apenas para uso dos IRCops, e nao sera
	mostrada aos usuarios nas mensagens de KILL.
	
	Este comando eh limitado aos Operadores dos Services.
	
OPER_HELP_SGLINE
	Sintaxe: SGLINE ADD [+tempo] máscara:motivo
	       SGLINE DEL {máscara | núm-entrada | lista}
	         SGLINE LIST [máscara | lista]
	         SGLINE VIEW [máscara | lista]
	         SGLINE CLEAR
	
	Permite que os Operadores dos Services manipulem a lista de
	SGLINE. Se um user estiver com a mesma máscara do SGLINE tentar 
	conectar, os Services não permitirão tal feito.
	
	Limitado aos Operadores dos Services.

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
	Sintaxe: SZLINE ADD [+tempo] máscara motivo
	         SZLINE DEL {máscara | núm-entrada | lista}
	         SZLINE LIST [máscara | lista]
	         SZLINE VIEW [máscara | lista]
	         SZLINE CLEAR
	
	Permite que os Operadores dos Services manipulem a lista
	de SZLINE. Se um usuário com o mesmo IP do SZLINE tentar 
	conectar, os Services não permitirão tal feito.
	
	Limitado aos Operadores dos Services.

OPER_HELP_SET
	Sintaxe: SET opcao selecao
	
	Ajusta varias opcoes globais dos Services.  Nomes de opcao
	atualmente definidas sao:
	    READONLY   Determina o modo read-only ou read-write
	    LOGCHAN    Ativa ou desativa reported de log a um canal
	    DEBUG      Ativa ou desativa o modo de depuracao (debug)
	
	Limitado aos Administradores dos Services.

OPER_HELP_SET_READONLY
	Sintaxe: SET READONLY {ON | OFF}
	
	Ativa ou desativa o modo read-only.  No modo read-only,
	usuarios normais nao serao capazes de modificar nenhum dado
	nos Services, incluindo lista de acesso de canais e nick,
	etc.  IRCops com privilegios suficientes nos Services serao
	capazes de mudar a lista de AKILL e de proibir e cancelar o
	registro de canais ou nicks, embora quaisquer mudancas nao
	serao salvas no banco de dados a menos que o modo read-only
	seja desativado antes dos Services serem terminados ou
	reiniciados.
	
	Esta opcao eh equivalente a opcao da linha de comando
	"-readonly".

OPER_HELP_SET_LOGCHAN
	Sintaxe: SET LOGCHAN {ON | OFF}

	No modo on, Services enviam todo o log para o canal especificado.
	LogChannel tem q estar definido na configuracion dos Services para
	poder usar esta opcao.

	Nota: O canal deve estar apropriadamente seguro, e so permitir
	ingresso a Administradores.

OPER_HELP_SET_DEBUG
	Sintaxe: SET DEBUG {ON | OFF | numero}
	
	Ativa ou desativa o modo de depuracao (debug).  No modo de
	depuracao, todos os dados enviados para os Services, assim
	como todas as outras mensagens de depuracao sao enviadas a
	um arquivo de log.  Se num eh fornecido, o modo de depuracao
	eh ativado, com nivel de debug fornecido.
	
	Esta opcao eh equivalente a opcao da linha de comando
	"-debug".
	
OPER_HELP_SET_NOEXPIRE
	Sintaxe: SET NOEXPIRE {ON | OFF}
	
	Ativa ou desativa o modo NOEXPIRE.  Se estiver ativado, os nicks
	canais, akills e lista de sessões não irão expirar enquanto a 
	opção não for desativada.

OPER_HELP_SET_SUPERADMIN
	Syntax: SET SUPERADMIN {ON | OFF}

	Setting this will grant you extra privlages such as the
	ability to be "founder" on all channel's etc...

	This opion is _NOT_ persistant, and should only be used when
	needed, and set back to OFF when no longer needed.


OPER_HELP_NOOP
	Sintaxe: NOOP SET servidor
	         NOOP REVOKE servidor
	
	NOOP SET Adiciona NOOP em um servidor 
	
	NOOP REVOKE Remove NOOP de um servidor
	
	Lembrete: O servidor nunca é checado pelos Services.
	
	Limitado aos Administradores dos Services.

OPER_HELP_JUPE
	Sintaxe: JUPE servidor
	
	Diz aos Services para "jupar" um servidor -- isto eh, criar
	um falso servidor conectado aos Services que previne que o
	servidor real com aquele nome se conecte na rede.  O jupe
	pode ser removido usando um simples SQUIT.
	
	Limitado aos Administradores dos Services.

OPER_HELP_RAW
	Sintaxe: RAW texto
	
	Envia uma serie de texto diretamente ao servidor onde os
	Services estao conectados.  Este comando tem varias
	limitacoes de uso, e pode criar confusoes e problemas na
	rede se usado impropriamente. NAO USE ESTE COMANDO a menos
	que voce tenha a certeza do que esta fazendo!
	
	Limitado aos Administradores dos Services.

OPER_HELP_UPDATE
	Sintaxe: UPDATE
	
	Faz com que os Services atualizem o banco de dados logo que
	voce envia este comando.
	
	Limitado aos Administradores dos Services.
	
OPER_HELP_RELOAD
	Syntax: RELOAD
	
	Faz com que os Services recarreguem seu arquivo de configuração. 
	Repare que algumas mudanças serão necessárias que os Services 
	sejam reinicados para ter efeito.
	
	Limitado aos Administradores dos Services.

OPER_HELP_QUIT
	Sintaxe: QUIT
	
	Faz com que deligue os Services imediatamente; o banco de
	dados nao eh salvo.  Este comando nao deve ser usado a menos
	que haja um dano da copia do banco de dados na memoria e por
	isso nao eles nao devem ser salvos.  Para desligar
	normalmente, use o comando SHUTDOWN.
	
	Limitado aos Administradores dos Services.

OPER_HELP_SHUTDOWN
	Sintaxe: SHUTDOWN
	
	Salva o banco de dados dos Services e os desliga.
	
	Limitado aos Administradores dos Services.

OPER_HELP_RESTART
	Sintaxe: RESTART
	
	Salva o banco de dados dos Services e os reinicia.
	(sai e roda imediatamente o executavel).
	
	Limitado aos Administradores dos Services.

OPER_HELP_KILLCLONES
	Sintaxe: KILLCLONES nick
	
	Expulsa ("killa") todos os usuarios que tenham os mesmos
	hosts que o nick especificado, adicionando *@host.com como 
	AKILL temporario para prevenir que usuarios indesejados
	reconectem imediatamente. Uma mensagem em WallOps eh tambem
	enviada para indicar quem usou o comando, quais hosts foram
	afetados e quantos usuarios foram expulsos ("killados").
	Eh usado para remover varios clones da rede.
	
	Limitado aos Administradores dos Services.
	
OPER_HELP_CHANLIST
	Sintaxe: CHANLIST [{padrão | nick} [SECRET]]
	
	Lista todos os canais em uso na Rede, sejam eles registrados ou não.
	
	Limitado aos Administradores dos Services.
	
OPER_HELP_USERLIST
	Sintaxe: USERLIST [{padrão | canal} [INVISIBLE]]
	
	Lista todos os usuários online na Rede, sejam eles registrados
	ou não.
	
	Limitado aos Administradores dos Services.
	
###########################################################################
#
# BotServ help messages
#
###########################################################################
	
OPER_HELP_CACHE
	Syntax: CACHE DEL hostname
	        CACHE LIST pattern [QUEUED | ALL]
	
	The CACHE DEL command deletes the specified hostname from 
	the cache. This is meant for debug purpose only.

	The CACHE LIST command lists all found proxies that match the
	given pattern. If the QUEUED option is given, it will list
	the queued or in progress scans instead, and if the ALL option 
	is given, it will list all cached hostnames.
	
	Limited to Services admins.

###########################################################################
#
# BotServ help messages
#
###########################################################################
	
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
	
BOT_HELP
	%S permite que você tenha um bot no seu canal.
	Foi criado para usuários que não tem como hospedar
	ou configurar um bot, ou para Redes que não permitem
	os usuários terem um bot. Os comandos disponíveis 
	estão listados abaixo; para usá-los, digite
	/msg %S comando. Para informações mais específicas
	sobre um comando, digite /msg %S HELP comando.
	
	BOTLIST            Lista os bots disponíveis
	ASSIGN             Associa um bot ao canal
	SET                Configura as opções do bot
	KICK               Configura os kickers
	BADWORDS           Configura os palavrões
	
	Outros comandos: ACT INFO SAY UNASSIGN
	
	O bot irá entrar no canal sempre que tiver ao menos
	%d usuário(s) nele.
	
BOT_HELP_BOTLIST
	Sintaxe: BOTLIST
	
	Lista todos os bots disponíveis nesta Rede.
	
BOT_HELP_ASSIGN
	Sintaxe: ASSIGN canal nick-bot
	
	Associa o nick de um bot a um canal. Você poderá então
	configurar o bot para o canal de acordo com as suas
	necessidades.  
	
BOT_HELP_UNASSIGN
	Sintaxe: UNASSIGN canal
	
	Desassocia um bot de um canal. Quando você usa este comando,
	o bot não irá entrar mais no canal. Entretando, será mantido
	a configuração do bot para aquele canal, para que você possa
	sempre poder associá-lo aquele canal mais tarde sem ter que
	configurar tudo novamente.
	
BOT_HELP_INFO
	Sintaxe: INFO {canal | nick}
	
	Permite que você possas ver as informações do %S sobre um
	canal ou bot. Se a opção for um canal, então você terá
	informações tais como kickers ativos. Se tiver sido um nick,
	você terá informações sobre o bot, como o horário de criação
	ou número do canal.
	
BOT_HELP_SET
	Sintaxe: SET canal opção parâmetros
	
	Configura as opções do bot.  opção pode ser:
	
	    DONTKICKOPS      Protege os ops contra kicks
	    DONTKICKVOICES   Protege os voices contra kicks
	    GREET            Habilita a mensagem de entrada
	    FANTASY          Habilita os comandos Fantasia
	    SYMBIOSIS        Permite que o bot seja um bot real
	
	Digite /msg %S HELP SET opção para maiores informações
	sobre uma opção específica.
	
	Atenção: o acesso a este comando é controlado pela opção
	level SET.
	
BOT_HELP_SET_DONTKICKOPS
	Sintaxe: SET canal DONTKICKOPS {ON|OFF}
	
	Ativa ou desativa o modo de proteção de ops em um canal.
	Quando estiver ativado, os ops não serão kickados pelo bot.
	
BOT_HELP_SET_DONTKICKVOICES
	Sintaxe: SET canal DONTKICKVOICES {ON|OFF}
	
	Ativa ou desativa o modo de proteção de voices em um canal.
	Quando estiver ativado, os voices não serão kickados pelo bot.
	
BOT_HELP_SET_FANTASY
	Sintaxe: SET canal FANTASY {ON|OFF}
	
	Ativa ou desativa o modo fantasia em um canal.
	Quando estiver ativado, os usuários poderão usar
	comandos como !op, !deop, !voice, !devoice,
	!kick, !kb, !unban, !seen em um canal.
	
	Note que os usuários que quiserem usar esses 
	comandos DEVEM ter um nível suficiente tanto para
	o FANTASIA quando para outro nível dependendo
	do comando usado (por exemplo, para usar !op, o 
	usuário deve ter acesso aos comandos OP e DEOP).

BOT_HELP_SET_GREET
	Sintaxe: SET canal GREET {ON|OFF}
	
	Ativa ou desativa o modo greet em um canal.
	Quando ativado, o bot irá mostrar mensagens de
	entradas dos usuários quando eles entrarem no
	canal, caso eles tenham nível de acessosuficiente.

BOT_HELP_SET_SYMBIOSIS
	Sintaxe: SET canal SYMBIOSIS {ON|OFF}
	
	Ativa ou desativa o modo symbiosis em um canal.
	Quando ativado, o bot fará tudo aquilo que 
	normalmente é feito pelo %s nos canais, como as 
	mudanças de modos, kicks, e inclusive a mensagem
	de entrada (entrymsg).

BOT_HELP_KICK
	Sintaxe: KICK canal opção parâmetros
	
	Configura os kicks do bot.  opção pode ser:
	
	    BOLDS         Seleciona se o bot kicka por negrito
	    BADWORDS      Seleciona se o bot kicka por palavrões
	    CAPS          Seleciona se o bot kicka por caps
	    COLORS        Seleciona se o bot kicka por cores
	    FLOOD         Seleciona se o bot kicka por flood
	    REPEAT        Seleciona se o bot kicka por repetição
	    REVERSES      Seleciona se o bot kicka por reversos
	    UNDERLINES    Seleciona se o bot kicka por sublinhado
	
	Digite /msg %S HELP KICK opção para maiores informações
	sobre uma opção específica.
	
	Nota: o acesso a esse comando é controlado pelo level SET.

BOT_HELP_KICK_BOLDS
	Sintaxe: KICK canal BOLDS {ON|OFF} [ttb]

	Ativa ou desativa o kick por negrito. Quando ativado
	o bot irá kickar os usuários que usarem negrito.

	ttb é o número de vezes que o usuário será kickado
	antes de ser banido.

BOT_HELP_KICK_COLORS
	Sintaxe: KICK canal COLORS {ON|OFF} [ttb]

	Ativa ou desativa o kick por cores. Quando ativado
	o bot irá kickar os usuários que usarem cores.

	ttb é o número de vezes que o usuário será kickado
	antes de ser banido.

BOT_HELP_KICK_REVERSES
	Sintaxe: KICK canal REVERSES {ON|OFF} [ttb]

	Ativa ou desativa o kick por reverso. Quando ativado
	o bot irá kickar os usuários que usarem reverso.

	ttb é o número de vezes que o usuário será kickado
	antes de ser banido.

BOT_HELP_KICK_UNDERLINES
	Sintaxe: KICK canal UNDERLINES {ON|OFF} [ttb]

	Ativa ou desativa o kick por sublinhado. Quando ativado
	o bot irá kickar os usuários que usarem sublinhado.

	ttb é o número de vezes que o usuário será kickado
	antes de ser banido.

BOT_HELP_KICK_CAPS
	Sintaxe: KICK canal CAPS {ON|OFF} [ttb [min [porcent]]]

	Ativa ou desativa o kick por caps. Quando ativado
	o bot irá kickar os usuários que usarem caps.

	O bot irá kickar apenas se tiver pelo menos min caps
	e eles constituírem pelo menos porcent%% do total 
	do texto (se não for especificado, o padrão é 10 caracteres
	e 25%%).

	ttb é o número de vezes que o usuário será kickado
	antes de ser banido.

BOT_HELP_KICK_FLOOD
	Sintaxe: KICK canal FLOOD {ON|OFF} [ttb [linhas [segs]]]

	Ativa ou desativa o kick por flood. Quando ativado o
	o bot irá kickar por flood no canal desde que sejam
	pelo menos linhas linhas em segs segundos.
	(se não for especificado, o padrão é 6 linhas em
	10 segundos).
	
	ttb é o número de vezes que o usuário será kickado
	antes de ser banido.

BOT_HELP_KICK_REPEAT
	Sintaxe: KICK #canal REPEAT {ON|OFF} [ttb [núm]]

	Ativa ou desativa o kick por repetição. Quando ativado
	o bot irá kickar os usuários que repetirem (onde
	núm é o número de repetições para kick).

	ttb é o número de vezes que o usuário será kickado
	antes de ser banido.

BOT_HELP_KICK_BADWORDS
	Sintaxe: KICK #canal BADWORDS {ON|OFF} [ttb]

	Ativa ou desativa o kick por sublinhado. Quando ativado
	o bot irá kickar os usuários que usarem sublinhado.

	ttb é o número de vezes que o usuário será kickado
	antes de ser banido.

	Você pode definir os palavrões para um canal usando o
	comando BADWORDS. Digite /msg %S HELP BADWORDS para
	maiores informações.

BOT_HELP_BADWORDS
	Sintaxe: BADWORDS canal ADD palavra [SINGLE | START | END]
	         BADWORDS canal DEL {palavra | núm-entrada | lista}
	         BADWORDS canal LIST [máscara | lista]
	         BADWORDS canal CLEAR
	
	Faz a manutenção da lista de palavrões em um canal. Essa lista
	determina quais são as palvras que deverão ser motivo de kick
	em um canal quando a opção BADWORDS estiver ativada. Para maiores
	informações, digite /msg %S HELP KICK BADWORDS.
	
	O comando BADWORDS ADD adiciona a palavra na lista de
	palavrões. A opção SINGLE quando ativada só kicka o usuário
	se ele escrever a palavra inteira. A opção START quando 
	ativada irá kickar o usuário que escrever uma palavra que comece
	com a palavra. A opção END se tiver sido ativada só kicka
	o usuário que escrever uma palavra que termine com a
	palavra. Se você não especificar nada o bot irá kickar sempre
	que a palavra for escrita pelo usuário.
	
	O comando BADWORDS DEL remove uma palavra da lista de
	palavrões de um determinado canal.
	
	O comando BADWORDS LIST mostra a lista de palavrões.
	Se forem usados wildcards, apenas aquelas entradas com
	as determinadas máscaras serão mostradas.  Se uma lista
	de números de entrada for dada, apenas aquelas entradas
	serão mostradas; por exemplo:

	   BADWORDS #canal LIST 2-5,7-9
	      Lista as entradas dos palavrões numeradas de 2 a 5 e
	      7 a 9.
	      
	O comando BADWORDS CLEAR limpa todas entradas da lista.

BOT_HELP_SAY
	Sintaxe: SAY canal texto
	
	Faz com que o bot diga determinado texto no determinado canal.

BOT_HELP_ACT
	Sintaxe: ACT canal texto
	
	Faz com que o bot faça o mesmo que o comando "/me"
	em um determinado canal com o determinado texto.

BOT_SERVADMIN_HELP
	
	Os seguintes comandos podem ser usados por Admins dos Services:
	
	    BOT              Mantém a lista de bots da Rede

BOT_SERVADMIN_HELP_BOT
	Sintaxe: BOT ADD nick user host real
	         BOT CHANGE oldnick novonick [user [host [real]]]
	         BOT DEL nick
	         BOT LIST
	
	Permite que os Administradores dos Services criem,
	modifiquem, e apaguem bots que os usuários poderão usar
	em seus próprios canais.
	
	BOT ADD adiciona um bot com um determinado nick, username,
	hostname e realname. 
	BOT CHANGE permite alterar o nick, username, hostname
	ou realname de um bot sem que seja necessário apagá-lo (e
	todas as informações nele contidas).
	BOT DEL remove um bot da lista de bots.  
	BOT LIST é um an alias de BOTLIST e apenas lista todos
	os bots disponíveis na Rede.
	
	Nota: se você criar um bot que use um nick registrado 
	no NickServ, este será dropado. E se tiver um usuário
	usando o nick na hora, este será killado.

BOT_SERVADMIN_HELP_SET
	
	Essas opções são reservadas aos Administradores dos Services:
	
	    NOBOT            Previne um bot de ser associado a 
	                     um canal

BOT_SERVADMIN_HELP_SET_NOBOT
	Sintaxe: SET canal NOBOT {ON|OFF}
	
	Este comando faz com que um determinado canal não possa ser
	associado a um bot. Se já houver um bot associado a este
	canal, este será automaticamente desassociado quando você
	ativar essa opção.
	
	Limitado aos Administradores dos Services.
BOT_SERVADMIN_HELP_SET_PRIVATE
	Syntax: SET bot-nick PRIVATE {ON|OFF}
	
	This option prevents a bot from being assigned to a
	channel by users that aren't IRC operators.
	
	Limited to Services admins.

###########################################################################
#
# HostServ messages
#
###########################################################################
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

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_OFF_UNREAL
	Your vhost was removed. To re-enable the standard host cloaking, type /mode %s +x
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
HOST_HELP
	%S commands:
	    ON          Activates your assigned vhost
	    OFF         Deactivates your assigned vhost

HOST_OPER_HELP
	Commands available to services operators only:
	    SET         Set the vhost of another user
	    SETALL      Set the vhost for all nicks in a group
	    DEL         Delete the vhost of another user
	    LIST        Displays one or more vhost entries.

HOST_ADMIN_HELP
	Commands available to services admins only:

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

