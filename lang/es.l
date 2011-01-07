# Spanish Language File.
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
# For example, "Español (Spanish)" or "Français (French)".

LANG_NAME
	Español (Spanish)

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
	Lun
	Mar
	Mie
	Jue
	Vie
	Sab
# %A
STRFTIME_DAYS_LONG
	Domingo
	Lunes
	Martes
	Miercoles
	Jueves
	Viernes
	Sabado
# %b
STRFTIME_MONTHS_SHORT
	Ene
	Feb
	Mar
	Abr
	May
	Jun
	Jul
	Ago
	Sep
	Oct
	Nov
	Dic
# %B
STRFTIME_MONTHS_LONG
	Enero
	Febrero
	Marzo
	Abril
	Mayo
	Junio
	Julio
	Agosto
	Septiembre
	Octubre
	Noviembre
	Diciembre


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	, 


# Various error messages.
USER_RECORD_NOT_FOUND
	Error Interno - Imposible procesar peticion.
UNKNOWN_COMMAND
	Comando no conocido %s.
UNKNOWN_COMMAND_HELP
	Comando no conocido %s. "%R%s HELP" para ayuda.
SYNTAX_ERROR
	Sintaxis: %s
MORE_INFO
	%R%s HELP %s para mayor informacion.
NO_HELP_AVAILABLE
	No existe ayuda para %s.
OBSOLETE_COMMAND
	Este comando esta obsoleto; usa %s en su lugar.

BAD_USERHOST_MASK
	La mascara debe ser en la forma usuario@host.
BAD_EXPIRY_TIME
	Tiempo de expiracion invalido.
USERHOST_MASK_TOO_WIDE
	%s La covertura es muy amplia; Por favor usa una mascara mas especifica.

SERVICE_OFFLINE
	%s esta momentaneamente offline.
READ_ONLY_MODE
	Noticia: Los Servicios estan en modo read-only; Los cambios no seran salvados!
PASSWORD_INCORRECT
	Clave incorrecta.
INVALID_TARGET
	"/msg %s" is no longer supported.  Use "/msg %s@%s" or "/%s" instead.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Acceso denegado.
PERMISSION_DENIED
	Permiso denegado.
RAW_DISABLED
	La opcion RAW ha sido deshabilitada. Si necesitas usarla 
	debes habilitar la directiva DisableRaw en la configuracion
	de los Servicios.

MORE_OBSCURE_PASSWORD
	Por favor intente nuevamente con una clave mas oscura.
	Las claves deben tener al menos 5 caracteres, no deben
	ser algo facil de adivinar (ej. Nombre real o Nick),
	y no pueden contener el caracter de espacio o tabulacion.
PASSWORD_TOO_LONG
	Your password is too long. Please try again with a shorter password.

NICK_NOT_REGISTERED
	Tu nick no esta registrado.
NICK_NOT_REGISTERED_HELP
	Tu nick no esta registrado. Tipea %R%s HELP para mayor
	informacion sobre el registro de nicknames.
NICK_X_IS_SERVICES
	Nick %s is part of this Network's Services.
NICK_X_NOT_REGISTERED
	El nick %s no esta registrado.
NICK_X_IN_USE
	El nick %s esta actualmente en uso.
NICK_X_NOT_IN_USE
	El nick %s no esta en uso.
NICK_X_NOT_ON_CHAN
	El nick %s no esta en el canal %s actualmente.
NICK_X_FORBIDDEN
	El nick %s no puede ser registrado o usado.
NICK_X_FORBIDDEN_OPER
	El nick %s ha sido prohibido por %s:
	%s
NICK_X_ILLEGAL
	El nick %ses un nickname ilegal, y no puede ser usado. 
NICK_X_TRUNCATED
	El nick %s fue reducido a %d caracteres.
NICK_X_SUSPENDED
	Nick %s is currently suspended.
CHAN_X_NOT_REGISTERED
	El Canal %s no esta registrado.
CHAN_X_NOT_IN_USE
	El Canal %s no existe.
CHAN_X_FORBIDDEN
	El Canal %s no puede ser registrado o usado.
CHAN_X_FORBIDDEN_OPER
	El Canal %s ha sido prohibido por %s:
	%s
CHAN_X_SUSPENDED
	      Suspendido: [%s] %s
NICK_IDENTIFY_REQUIRED
	Autentificacion por clave requerida para ese comando.
	Intenta nuevamente despues de tipear %R%s IDENTIFY clave.
CHAN_IDENTIFY_REQUIRED
	Autentificacion por clave requerida para ese comando.
	Intenta nuevamente despues de tipear %R%s IDENTIFY %s clave.
	
MAIL_DISABLED
	Los servicios han sido configurados para no enviar e-mail
MAIL_INVALID
	El e-mail para %s es invalido.
MAIL_X_INVALID
	%s no es un e-mail valido.
MAIL_LATER
	Imposible enviar el e-mail ahora; por favor intenta mas tarde.
MAIL_DELAYED
	Por favor espera %d segundos e intente nuevamente.
	
NO_REASON
	Sin razon.
UNKNOWN
	<desconocido>

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
	no expira
EXPIRES_SOON
	expira con el proximo update de la base de datos
EXPIRES_M
	expira en %d minutos
EXPIRES_1M
	expira en %d minutos
EXPIRES_HM
	expira en %d horas, %d minutos
EXPIRES_H1M
	expira en %d horas, %d minuto
EXPIRES_1HM
	expira en %d hora, %d minutos
EXPIRES_1H1M
	expira en %d hora, %d minuto
EXPIRES_D
	expira en %d dias
EXPIRES_1D
	expira en %d dia

# Generic Footer message
END_OF_ANY_LIST
	Fin de la lista de %s.

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
	Este nick pertenece a otra persona. Por favor elige otro nick.
	(Si este es tu nick, tipea %R%s IDENTIFY clave.)
NICK_IS_SECURE
	Este nick esta registrado y protegido. Si es tu
	nick, tipea %R%s IDENTIFY clave. Si no,
	por favor elige un nick diferente.
NICK_MAY_NOT_BE_USED
	Este nickname no puede ser usado. Por favor elige otro.
FORCENICKCHANGE_IN_1_MINUTE
	Si no cambias tu nick en un minuto, Lo cambiare yo.
FORCENICKCHANGE_IN_20_SECONDS
	Si no cambias tu nick en 20 segundos, Lo cambiare yo.
FORCENICKCHANGE_NOW
	Este nick ha sido registrado; No puedes usarlo.
FORCENICKCHANGE_CHANGING
	Tu nickname esta siendo cambiado a %s

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER clave [email]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER clave email
NICK_REGISTRATION_DISABLED
	Lo siento, el registro de nicknames esta temporalmente deshabilitado.
NICK_REGISTRATION_FAILED
	Lo siento, fallo en el registro.
NICK_REG_PLEASE_WAIT
	Por favor espera %d segundos antes de usar el comando REGISTER nuevamente.
NICK_CANNOT_BE_REGISTERED
	El nickname %s no puede ser registrado.
NICK_ALREADY_REGISTERED
	El nickname %s ya esta registrado!
NICK_REGISTERED
	El nickname %s esta registrado bajo tu cuenta: %s
NICK_REGISTERED_NO_MASK
	Nickname %s registrado.
NICK_PASSWORD_IS
	Tu clave es %s - Recuerdala para uso futuro.

# GROUP responses
NICK_REG_DELAY
	Debes estar conectado mas de %d segundos para registrar tu nick.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP grupo clave
NICK_GROUP_DISABLED
	Lo siento, La agrupacion de nicks esta temporalmente deshabilitada.
NICK_GROUP_FAILED
	Lo siento, La agrupacion de nicks ha fallado.
NICK_GROUP_PLEASE_WAIT
	Por favor espera %d segundos antes de usar el comando GROUP nuevamente.
NICK_GROUP_CHANGE_DISABLED
	Tu nick ya esta registrado; tipea %R%s DROP primero.
NICK_GROUP_SAME
	Ya eres miembro del grupo de %s.
NICK_GROUP_TOO_MANY
	Hay muchos nicks en el grupo de %s; listalos y bota algunos.
	Tipea %R%s HELP GLIST y %R%s HELP DROP
	para mayor informacion.
NICK_GROUP_JOINED
	Ahora eres miembro en el grupo de %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY clave
NICK_IDENTIFY_FAILED
	Lo siento, la identificacion ha fallado.
NICK_IDENTIFY_SUCCEEDED
	Clave aceptada - Has sido reconocido.
NICK_IDENTIFY_EMAIL_REQUIRED
	Ahora debes proveer un e-mail para tu nick.
	Este e-mail te permitira recuperar tu clave en
	caso que la olvides.
NICK_IDENTIFY_EMAIL_HOWTO
	Tipea %R%S SET EMAIL e-mail para setear tu e-mail.
	Respetamos tu privacidad; este e-mail no sera entregado
	a ninguna persona.
	
# LOGOUT responses
NICK_ALREADY_IDENTIFIED
	Ya estas identificado.

# UPDATE responses
NICK_UPDATE_SUCCESS
	Estado actualizado (memos, vhost, chmodes, flags).
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	Tu nick ha sido desconectado.
NICK_LOGOUT_X_SUCCEEDED
	El nick %s ha sido desconectado.

# DROP responses
NICK_LOGOUT_SERVICESADMIN
	No puedo desconectar a  %s porque es un administrador de servicios.

# DROP responses
NICK_DROP_DISABLED
	Lo siento, la de-registracion de nicknames esta temporalmente inabilitada.
NICK_DROPPED
	Tu nickname ha sido botado.
NICK_X_DROPPED
	El nickname %s ha sido botado.

# SET responses
NICK_SET_SYNTAX
	SET opcion parametros
NICK_SET_SERVADMIN_SYNTAX
	SET [nick] opcion parametros
NICK_SET_DISABLED
	Lo siento, el seteo de opciones para nicknames esta temporalmente deshabilitado.
NICK_SET_UNKNOWN_OPTION
	Opcion SET desconocida %s.
NICK_SET_OPTION_DISABLED
	Opcion %s no puede ser seteada en esta red.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	El nuevo display DEBE ser un nickname de tu grupo de nicknames!
NICK_SET_DISPLAY_CHANGED
	El nuevo display es ahora %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Lo siento, el cambio de clave no pudo ser completado.
NICK_SET_PASSWORD_CHANGED
	Clave cambiada.
NICK_SET_PASSWORD_CHANGED_TO
	Clave cambiada a %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE numero
NICK_SET_LANGUAGE_UNKNOWN
	Numero de lenguage %d desconocido. Tipea %R%s HELP SET LANGUAGE para una lista de lenguages.
NICK_SET_LANGUAGE_CHANGED
	Lenguage cambiado a Español.

# SET URL responses
NICK_SET_URL_CHANGED
	URL cambiada a %s.
NICK_SET_URL_UNSET
	URL no seteada.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	Direccion E-mail cambiada a %s.
NICK_SET_EMAIL_UNSET
	Direccion E-mail no seteada.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	No puedes no setear tu direccion e-mail en esta red.
	
# SET ICQ responses
NICK_SET_ICQ_CHANGED
	Numero ICQ seteado a %s.
NICK_SET_ICQ_UNSET
	Numero ICQ no seteado.
NICK_SET_ICQ_INVALID
	%s no es un numero valido.
	
# SET GREET responses
NICK_SET_GREET_CHANGED
	Mensaje de bienvenida cambiado a %s.
NICK_SET_GREET_UNSET
	Mensaje de bienvenida no seteado.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Proteccion Activada.
NICK_SET_KILL_QUICK
	La protection esta Activada, con demora reducida.
NICK_SET_KILL_IMMED
	La protection esta Activada, sin demora.
NICK_SET_KILL_IMMED_DISABLED
	La opcion IMMED no esta disponible en esta red.
NICK_SET_KILL_OFF
	La protection esta Desactivada.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	La opcion de seguridad esta ahora Activada.
NICK_SET_SECURE_OFF
	la opcion de seguridad esta ahora Desactivada.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	La opcion de privacidad esta ahora Activada.
NICK_SET_PRIVATE_OFF
	La opcion de privacidad esta ahora Desactivada.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Tu direccion e-mail no sera mostrada en %s INFO displays.
NICK_SET_HIDE_EMAIL_OFF
	Tu direccion e-mail sera ahora mostrada en %s INFO displays.
NICK_SET_HIDE_MASK_ON
	Tu ultima mascara usuario@host no sera mostrada en %s INFO displays.
NICK_SET_HIDE_MASK_OFF
	Tu ultima mascara usuario@host sera ahora mostrada en %s INFO displays.
NICK_SET_HIDE_QUIT_ON
	Tu ultimo mensaje quit no sera mostrado en %s INFO displays.
NICK_SET_HIDE_QUIT_OFF
	Tu ultimo mensaje quit sera ahora mostrado en %s INFO displays.
	
# SET MSG responses
NICK_SET_HIDE_STATUS_ON
	Tu estado de acceso a los Servicios no sera mostrado en %s INFO.
NICK_SET_HIDE_STATUS_OFF
	Tu estado de acceso a los Servicios ahora sera mostrado en %s INFO.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_MSG_ON
	Los servicios te responderan ahora con mensajes.
NICK_SET_MSG_OFF
	Los servicios te responderan ahora con noticias.

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
	ACCESS {ADD | DEL | LIST} [mascara]
NICK_ACCESS_ALREADY_PRESENT
	La mascara %s ya esta presente en tu lista de acceso.
NICK_ACCESS_REACHED_LIMIT
	Lo siento, solo puedes tener %d instancias en tu lista de acceso por nickname.
NICK_ACCESS_ADDED
	%s añadido a tu lista de acceso.
NICK_ACCESS_NOT_FOUND
	%s no encontrado en tu lista de acceso.
NICK_ACCESS_DELETED
	%s borrado de tu lista de acceso.
NICK_ACCESS_LIST
	Lista de acceso:
NICK_ACCESS_LIST_X
	Lista de acceso para %s:
NICK_ACCESS_LIST_EMPTY
	Tu lista de acceso esta vacia.
NICK_ACCESS_LIST_X_EMPTY
	La lista de acceso para %s esta vacia.

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
	%s es %s
NICK_INFO_SERVICES_OPER
	%s es un operador de servicios.
NICK_INFO_SERVICES_ADMIN
	%s es un administrador de servicios.
NICK_INFO_SERVICES_ROOT
	%s is a services root administrator.
NICK_INFO_ADDRESS
	Ultima direccion vista: %s
NICK_INFO_ADDRESS_ONLINE
	   Esta en linea desde: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s Esta actualmente en linea. 
NICK_INFO_TIME_REGGED
	     Tiempo registrado: %s
NICK_INFO_LAST_SEEN
	      Ultima vez visto: %s
NICK_INFO_LAST_QUIT
	   Ultimo mensaje quit: %s
NICK_INFO_URL
	                   URL: %s
NICK_INFO_EMAIL
	      Direccion E-mail: %s
NICK_INFO_VHOST
	                 vhost: %s
NICK_INFO_VHOST2
	            	vhost: %s@%s
NICK_INFO_ICQ
	                 ICQ #: %d
NICK_INFO_GREET
	 Mensaje de bienvenida: %s
NICK_INFO_OPTIONS
	              Opciones: %s
NICK_INFO_EXPIRE
	Expira en: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Proteccion
NICK_INFO_OPT_SECURE
	Seguro
NICK_INFO_OPT_PRIVATE
	Privado
NICK_INFO_OPT_MSG
	Modo de mensaje
NICK_INFO_OPT_AUTOOP
	Auto-op
NICK_INFO_OPT_NONE
	Ninguno
NICK_INFO_NO_EXPIRE
	Este nickname no expirara.
NICK_INFO_FOR_MORE
	Para mayor informacion, tipea %R%s INFO %s ALL.

# LIST responses
NICK_INFO_SUSPENDED
	This nickname is currently suspended, reason: %s
NICK_INFO_SUSPENDED_NO_REASON
	This nickname is currently suspended


# LIST responses
NICK_LIST_SYNTAX
	LIST patron
NICK_LIST_SERVADMIN_SYNTAX
	LIST patron [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Lista de resultados concordando %s:
NICK_LIST_RESULTS
	Fin de la lista - %d/%d resultado(s) mostrado(s).

# GLIST responses
NICK_ALIST_SYNTAX
	ALIST nick
NICK_ALIST_HEADER
	Canales en los que tienes acceso:
	  Num  Canal                Nivel    Descripcion 
NICK_ALIST_HEADER_X
	Canales en los que %s tiene acceso:
	  Num  Canal                Nivel    Descripcion 
NICK_ALIST_XOP_FORMAT
	  %3d %c%-20s %-8s %s
NICK_ALIST_ACCESS_FORMAT
	  %3d %c%-20s %-8d %s
NICK_ALIST_FOOTER
	Fin de la lista - %d/%d resultado(s) mostrado(s).

# GLIST responses
NICK_GLIST_HEADER
	Lista de nicknames en tu grupo:
NICK_GLIST_HEADER_X
	Lista de nicknames en el grupo de %s:
NICK_GLIST_FOOTER
	%d nicknames en el grupo.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (expires in %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nickname [clave]
NICK_NO_RECOVER_SELF
	No puedes recuperarte a ti mismo!
NICK_RECOVERED
	El usuario que ocupaba tu nick ha sido killeado.
	%R%s RELEASE %s para recuperarlo antes de %s.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nickname [clave]
NICK_RELEASE_NOT_HELD
	El nick %s no esta siendo ocupado.
NICK_RELEASED
	Tu nick ha sido liberado por los Servicios.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nickname [clave]
NICK_NO_GHOST_SELF
	No puedes ghostearte a ti mismo!
NICK_GHOST_KILLED
	El ghost con tu nick ha sido killeado.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nickname
NICK_GETPASS_UNAVAILABLE
	El comando GETPASS no esta disponible por el uso de encriptacion.
NICK_GETPASS_PASSWORD_IS
	La clave para %s es %s.

# SENDPASS responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host Sin comodines (WildCards)!!
NICK_GETEMAIL_EMAILS_ARE
	Emails Coinciden %s a %s.
NICK_GETEMAIL_NOT_USED
	No hay e-mails listados para %s.
	
# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS nickname
NICK_SENDPASS_UNAVAILABLE
	El comando SENDPASS no esta disponible por el uso de encriptacion.
NICK_SENDPASS_SUBJECT
	Clave para el Nickname (%s)
NICK_SENDPASS_HEAD
	Hola,
NICK_SENDPASS_LINE_1
	Tu has pedido recibir la clave para el nickname %s por e-mail.
NICK_SENDPASS_LINE_2
	La clave es %s Por razones de seguridad deberas cambiarla en cuanto recibas este mail.
NICK_SENDPASS_LINE_3
	Si no sabes porque este mail ha sido enviado a ti, por favor ignoralo.
NICK_SENDPASS_LINE_4
	POR FAVOR NO RESPONDER A ESTE EMAIL!
NICK_SENDPASS_LINE_5
	Administradores de %s.
NICK_SENDPASS_OK
	La clave de %s ha sido enviada.

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
	FORBID nickname [razon]
NICK_FORBID_SYNTAX_REASON
	FORBID nickname razon
NICK_FORBID_SUCCEEDED
	El nick %s es ahora prohibido.
NICK_FORBID_FAILED
	La prohibicion de %s no fue posible!

# Access level descriptions
NICK_REQUESTED
	Este nick ya ha sido pedido, chequea tu e-mail para el password.
NICK_REG_RESENT
	Tu password ha sido enviado nuevamente a %s.
NICK_REG_UNABLE
	Nick NO registrado. Intentalo nuevamente.
NICK_IS_PREREG
	Este nick esta esperando una verificacion por e-mail antes de poder completar el registro.
NICK_ENTER_REG_CODE
	El password ha sido enviado a %s, tipea %R%s confirm <password> para completar el registro.
NICK_CONFIRM_NOT_FOUND
	El paso de registro 1 puede haber expirado. Usa "%R%s register <password> <email>" primero.
NICK_CONFIRM_INVALID
	Password invalido. Chequea el e-mail e intenta nuevamente.
NICK_REG_MAIL_SUBJECT
	Registro de Nick (%s)
NICK_REG_MAIL_HEAD
	Hola,
NICK_REG_MAIL_LINE_1
	Tu has pedido registrar el nickname %s.
NICK_REG_MAIL_LINE_2
	Tipea " %R%s confirm %s " para completar el registro.
NICK_REG_MAIL_LINE_3
	Si no sabes por que has recibido este email, por favor ignorarlo.
NICK_REG_MAIL_LINE_4
	NO RESPONDAS A ESTE MAIL!
NICK_REG_MAIL_LINE_5
	Administradores de %s.
NICK_GETPASS_PASSCODE_IS
	El password para %s es %s.
NICK_FORCE_REG
	Nick %s confirmado.

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Status automatico de operador de canal
CHAN_LEVEL_AUTOVOICE
	Modo automatico +v
CHAN_LEVEL_AUTOHALFOP
	Modo automatico +h
CHAN_LEVEL_AUTOPROTECT
	Modo automatico +a
CHAN_LEVEL_AUTODEOP
	Status de operador de canal deshabilitado.
CHAN_LEVEL_NOJOIN
	No es posible entrar a un canal si esta RESTRINGIDO
CHAN_LEVEL_INVITE
	Habilitado para usar el comando INVITE
CHAN_LEVEL_AKICK
	Habilitado para usar el comando AKICK
CHAN_LEVEL_SET
	Habilitado para usar el comando SET (no FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Habilitado para usar el comando CLEAR
CHAN_LEVEL_UNBAN
	Habilitado para usar el comando UNBAN
CHAN_LEVEL_OPDEOP
	Habilitado para usar el comando OP/DEOP
CHAN_LEVEL_ACCESS_LIST
	Habilitado para ver la lista de acceso
CHAN_LEVEL_ACCESS_CHANGE
	Habilitado para modificar la lista de acceso
CHAN_LEVEL_MEMO
	Habilitado para listar/leer memos del canal
CHAN_LEVEL_ASSIGN
	Habilitado para asignar/deasignar un bot
CHAN_LEVEL_BADWORDS
	Habilitado para usar el comando BADWORDS
CHAN_LEVEL_NOKICK
	Jamas expulsado por los kicks del bot
CHAN_LEVEL_FANTASIA
	Habilitado para utilizar comandos de fantasia
CHAN_LEVEL_SAY
	Habilitado para usar el comando SAY y ACT
CHAN_LEVEL_GREET
	Mensaje de bienvenida mostrado
CHAN_LEVEL_VOICEME
	Autorizado para voice/devoice a si mismo
CHAN_LEVEL_VOICE
	Habilitado para usar el comando VOICE/DEVOICE
CHAN_LEVEL_GETKEY
	Habilitado para usar el comando GETKEY
CHAN_LEVEL_OPDEOPME
	Autorizado para op/deop a si mismo
CHAN_LEVEL_HALFOPME
	Autorizado para halfop/dehalfop a si mismo
CHAN_LEVEL_HALFOP
	Habilitado para usar el comando HALFOP/DEHALFOP
CHAN_LEVEL_PROTECTME
	Autorizado para protect/deprotect a si mismo
CHAN_LEVEL_PROTECT
	Habilitado para usar el comando PROTECT/DEPROTECT
CHAN_LEVEL_KICKME
	Autorizado para kickearse a si mismo
CHAN_LEVEL_KICK
	Habilitado para usar el comando KICK
CHAN_LEVEL_SIGNKICK
	Sin firma de kick cuando SIGNKICK LEVEL es usado
CHAN_LEVEL_BANME
	Permitido banearse a si mismo
CHAN_LEVEL_BAN
	Permitido usar el comando BAN
CHAN_LEVEL_TOPIC
	Permitido usar el comando TOPIC
CHAN_LEVEL_INFO
	Permitido usar el comando INFO con
	la opcion ALL

# Automatic responses
CHAN_IS_REGISTERED
	Este canal ha sido registrado con %s.
CHAN_NOT_ALLOWED_OP
	No estas autorizado al status de operador de canal en %s.
CHAN_MAY_NOT_BE_USED
	Este canal esta prohibido.
CHAN_NOT_ALLOWED_TO_JOIN
	No estas autorizado a estar en este canal.

# REGISTER responses
CHAN_X_INVALID
	Channel %s is not a valid channel.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER canal clave descripcion
CHAN_REGISTER_DISABLED
	Lo siento, el registro de canales esta temporalmente deshabilitado.
CHAN_REGISTER_NOT_LOCAL
	Canales locales no pueden ser registrados.
CHAN_MUST_REGISTER_NICK
	Debes registrar tu nickname primero. Tipea %R%s HELP para informacion en como registrar nicknames.
CHAN_MUST_IDENTIFY_NICK
	Por favor primero identificate con %s, usando el comando:
	%R%s IDENTIFY clave
CHAN_MAY_NOT_BE_REGISTERED
	El canal %s no puede ser registrado.
CHAN_ALREADY_REGISTERED
	El canal %s ya esta registrado!
CHAN_MUST_BE_CHANOP
	Debes ser un operador de canal para registrar el canal.
CHAN_REACHED_CHANNEL_LIMIT
	Lo siento, has alcanzado tu limite de %d canales.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Lo siento, has sobrepasado tu limite de %d canales.
CHAN_REGISTRATION_FAILED
	Lo siento, el registro ha fallado.
CHAN_REGISTERED
	El canal %s esta registrado bajo tu nickname: %s
CHAN_PASSWORD_IS
	Tu clave para el canal es %s - Recuerdala para uso futuro.
CHAN_REGISTER_NONE_CHANNEL
	Has intentado registrar el canal %s, el cual no existe.
CHAN_SYMBOL_REQUIRED
	Por favor usar el simbolo # cuando intentes registrar un canal.


# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY canal clave
CHAN_IDENTIFY_FAILED
	Lo siento, la identificacion ha fallado.
CHAN_IDENTIFY_SUCCEEDED
	Clave aceptada - ahora tienes nivel de fundador en %s.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT canal nickname
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT canal [nickname]
CHAN_LOGOUT_SUCCEEDED
	El usuario %s ha sido desconectado del canal %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	Todos los usuarios identificados han sido desconectados del canal %s (except the channel founder).
CHAN_LOGOUT_FOUNDER_FAILED
	You may not log out of %s (you are founder).

# DROP responses
CHAN_DROP_SYNTAX
	DROP canal
CHAN_DROP_DISABLED
	Lo siento, la de-registracion de canales esta temporalmente desabilitada.
CHAN_DROPPED
	El canal %s ha sido botado.

# SET responses
CHAN_SET_SYNTAX
	SET canal opcion parametros
CHAN_SET_DISABLED
	Lo siento, el seteo de opciones de canales esta temporalmente deshabilitado.
CHAN_SET_UNKNOWN_OPTION
	Opcion SET %s desconocida.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s tiene demasiados canales registrados.
CHAN_FOUNDER_CHANGED
	El Fundador de %s cambiado a %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Sucesor de %s cambiado a %s.
CHAN_SUCCESSOR_UNSET
	Sucesor de %s eliminado.
CHAN_SUCCESSOR_IS_FOUNDER
	%s no puede ser el sucesor en el canal %s ya que el es el fundador.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Lo siento, el seteo de la clave no fue posible.
CHAN_PASSWORD_CHANGED
	Clave para %s cambiada.
CHAN_PASSWORD_CHANGED_TO
	La clave para %s cambiado a %s.

# SET DESC responses
CHAN_DESC_CHANGED
	La descripcion de %s cambiada a%s.

# SET URL responses
CHAN_URL_CHANGED
	URL para %s cambiada a %s.
CHAN_URL_UNSET
	URL para %s no seteada.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	Direccion e-mail para %s cambiada a %s.
CHAN_EMAIL_UNSET
	Direccion e-mail para %s no seteada.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Mensage de entrada para %s cambiado.
CHAN_ENTRY_MSG_UNSET
	Mensaje de entrada para %s no seteado.
	
# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s no es un tipo de ban valido.
CHAN_SET_BANTYPE_CHANGED
	Tipo de ban para el canal %s es ahora #%d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Caracter de modo %c desconocido e ignorado.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Modo %c ignorado porque no puedes lockearlo.
CHAN_SET_MLOCK_L_REQUIRED
	Debes usar modo de lock +l tambien para el modo
	de lock +L.
CHAN_SET_MLOCK_K_REQUIRED
	Debes usar modo de lock +i tambien para el modo
	de lock +K.
CHAN_MLOCK_CHANGED
	Modo de lock en el canal %s cambiado a %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET canal KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Retencion de topic para %s Activada.
CHAN_SET_KEEPTOPIC_OFF
	Retencion de topic para %s Desactivada.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET canal TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Lock de topic para %s Activado.
CHAN_SET_TOPICLOCK_OFF
	Lock de topic para %s Desactivado.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET canal PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Opcion de paz para %s Activada.
CHAN_SET_PEACE_OFF
	Opcion de paz para %s Desactivada.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET canal PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Opcion de privacidad para %s Activada.
CHAN_SET_PRIVATE_OFF
	Opcion de privacidad para %s Desactivada.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET canal SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Seguridad de OPs para %s Activada.
CHAN_SET_SECUREOPS_OFF
	Seguridad de OPs para %s Desactivada.
	
# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET canal SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Seguridad de fundador para %s Activada.
CHAN_SET_SECUREFOUNDER_OFF
	Seguridad de fundador para %s Desactivada.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET canal RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Acceso restringido para %s Activado.
CHAN_SET_RESTRICTED_OFF
	Acceso restringido para %s Desactivado.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET canal SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Opcion de seguridad para %s Activada.
CHAN_SET_SECURE_OFF
	Opcion de seguridad para %s Desactivada.
	
# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET canal SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Opcion de kick firmado para %s Activada.
CHAN_SET_SIGNKICK_LEVEL
	Opcion de kick firmado para %s Activada, pero depende
	del nivel del usuario que use el comando.
CHAN_SET_SIGNKICK_OFF
	Opcion de kick firmado para %s Desactivada.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET canal OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Opcion OP-notice para %s Activada.
CHAN_SET_OPNOTICE_OFF
	Opcion OP-notice para %s Desactivada.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET canal XOP {ON | OFF}
CHAN_SET_XOP_ON
	El sistema de listas xOP para %s esta ahora Encendido.
CHAN_SET_XOP_OFF
	El sistema de listas xOP  para %s esta ahora Apagado.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET canal NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	El canal %s no expirara.
CHAN_SET_NOEXPIRE_OFF
	El canal %s expirara.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Lo siento, solo puedes tener %d instancias de AOP/SOP/VOP
	en un canal.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	No puedes usar este comando. Usa el comando ACCESS en su lugar.
	Tipea %R%s HELP ACCESS para mayor informacion.
CHAN_XOP_NOT_AVAILABLE
	El sistema xOP no esta disponible.

# AOP messages
CHAN_AOP_SYNTAX
	AOP canal {ADD|DEL|LIST|CLEAR} [nick | instancia]
CHAN_AOP_DISABLED
	Lo siento, la modificacion de la lista AOP de canales esta
	temporalmente deshabilitada.
CHAN_AOP_NICKS_ONLY
	La lista de AOP de canales solo puede contener nicks
	registrados.
CHAN_AOP_ADDED
	%s añadido a la lista AOP de %s.
CHAN_AOP_MOVED
	%s movido a %s en la lista AOP.
CHAN_AOP_NO_SUCH_ENTRY
	No existe la instancia (#%d) en la lista AOP de %s.
CHAN_AOP_NOT_FOUND
	%s no encontrado en la lista AOP de %s.
CHAN_AOP_NO_MATCH
	No existen instancias concordantes en la lista AOP de %s.
CHAN_AOP_DELETED
	%s borrado de la lista AOP de %s.
CHAN_AOP_DELETED_ONE
	1 instancia borrada de la lista AOP de %s.
CHAN_AOP_DELETED_SEVERAL
	%d instancias borradas de la lista AOP de %s.
CHAN_AOP_LIST_EMPTY
	La lista AOP de %s esta vacia.
CHAN_AOP_LIST_HEADER
	Lista AOP para %s:
	  Num  Nick
CHAN_AOP_CLEAR
	La lista AOP de canales %s ha sido limpiada.

# HOP messages
CHAN_HOP_SYNTAX
	HOP canal {ADD|DEL|LIST|CLEAR} [nick | lista]
CHAN_HOP_DISABLED
	Lo siento, la modificacion de listas HOP de canales esta
	temporalmente deshabilitada.
CHAN_HOP_NICKS_ONLY
	Listas HOP de canales solo pueden tener nicks registrados.
CHAN_HOP_ADDED
	%s añadido a la lista HOP de %s.
CHAN_HOP_MOVED
	%s movido a %s en la lista HOP.
CHAN_HOP_NO_SUCH_ENTRY
	No existe la instancia (#%d) en la lista HOP de %s.
CHAN_HOP_NOT_FOUND
	%s no encontrado en la lista HOP de %s.
CHAN_HOP_NO_MATCH
	No existen instancias concordantes en la lista HOP de %s.
CHAN_HOP_DELETED
	%s borrado de la lista HOP de %s.
CHAN_HOP_DELETED_ONE
	1 instancia borrada de la lista HOP de %s.
CHAN_HOP_DELETED_SEVERAL
	%d instancias borradas de la lista HOP de %s.
CHAN_HOP_LIST_EMPTY
	La lista HOP de %s esta vacia.
CHAN_HOP_LIST_HEADER
	Lista HOP para %s:
	  Num  Nick
CHAN_HOP_CLEAR
	La lista HOP de canales %s ha sido limpiada.

# SOP messages
CHAN_SOP_SYNTAX
	SOP canal {ADD|DEL|LIST|CLEAR} [nick | lista]
CHAN_SOP_DISABLED
	Lo siento, la modificacion de la lista SOP de canales esta
	temporalmente deshabilitada.
CHAN_SOP_NICKS_ONLY
	Listas SOP de canales solo pueden tener nicks registrados.
CHAN_SOP_ADDED
	%s añadido a la lista SOP de %s.
CHAN_SOP_MOVED
	%s movido a %s en la lista SOP.
CHAN_SOP_NO_SUCH_ENTRY
	No existe la instancia (#%d) en la lista SOP de %s.
CHAN_SOP_NOT_FOUND
	%s no encontrado en la lista SOP de %s.
CHAN_SOP_NO_MATCH
	No existen instancias concordantes en la lista SOP de %s.
CHAN_SOP_DELETED
	%s borrado de la lista SOP de %s.
CHAN_SOP_DELETED_ONE
	1 instancia borrada de la lista SOP de %s.
CHAN_SOP_DELETED_SEVERAL
	%d instancias borradas de la lista SOP de %s.
CHAN_SOP_LIST_EMPTY
	La lista SOP de %s esta vacia.
CHAN_SOP_LIST_HEADER
	Lista SOP para %s:
	  Num  Nick
CHAN_SOP_CLEAR
	La lista HOP de canales %s ha sido limpiada.

# VOP messages
CHAN_VOP_SYNTAX
	VOP canal {ADD|DEL|LIST|CLEAR} [nick | lista]
CHAN_VOP_DISABLED
	Lo siento, la modificacion de la lista VOP de canales esta
	temporalmente deshabilitada.
CHAN_VOP_NICKS_ONLY
	Listas VOP de canales solo pueden tener nicks registrados.
CHAN_VOP_ADDED
	%s añadido a la lista VOP de %s.
CHAN_VOP_MOVED
	%s movido a %s en la lista VOP.
CHAN_VOP_NO_SUCH_ENTRY
	No existe la instancia (#%d) en la lista VOP de %s.
CHAN_VOP_NOT_FOUND
	%s no encontrado en la lista VOP de %s.
CHAN_VOP_NO_MATCH
	No existen instancias concordantes en la lista VOP de %s.
CHAN_VOP_DELETED
	%s borrado de la lista VOP de %s.
CHAN_VOP_DELETED_ONE
	1 instancia borrada de la lista VOP de %s.
CHAN_VOP_DELETED_SEVERAL
	%d instancias borradas de la lista VOP de %s.
CHAN_VOP_LIST_EMPTY
	La lista VOP de %s esta vacia.
CHAN_VOP_LIST_HEADER
	Lista VOP para %s:
	  Num  Nick
CHAN_VOP_CLEAR
	La lista HOP de canales %s ha sido limpiada.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS canal {ADD|DEL|LIST|CLEAR} [nick [nivel] | lista]
CHAN_ACCESS_XOP
	No puedes usar este comando.
	Usa los comandos AOP, SOP y VOP en vez.
	Tipea %R%s HELP comando para mayor informacion.
CHAN_ACCESS_XOP_HOP
	You can't use this command. 
	Use the AOP, SOP, HOP and VOP commands instead.
	Type %R%s HELP command for more information.
CHAN_ACCESS_DISABLED
	Lo siento, la modificacion de acceso a canales esta temporalmete deshabilitada.
CHAN_ACCESS_LEVEL_NONZERO
	El nivel de acceso no puede ser cero.
CHAN_ACCESS_LEVEL_RANGE
	El nivel de acceso debe ser entre %d y %d inclusive.
CHAN_ACCESS_NICKS_ONLY
	Las listas de acceso a canales solo pueden contener nicks registrados.
CHAN_ACCESS_REACHED_LIMIT
	Lo siento, solo puedes tener %d instancias de acceso en un determinado canal.
CHAN_ACCESS_LEVEL_UNCHANGED
	Nivel de acceso para %s en %s no cambiada de %d.
CHAN_ACCESS_LEVEL_CHANGED
	Nivel de acceso para %s en %s cambiada a %d.
CHAN_ACCESS_ADDED
	%s añadido a la lista de acceso de %s con nivel %d.
CHAN_ACCESS_NO_SUCH_ENTRY
	No existe (#%d) en la lista de acceso de %s.
CHAN_ACCESS_NOT_FOUND
	%s no encontrado en la lista de acceso de %s.
CHAN_ACCESS_NO_MATCH
	No hay instancias que concuerden en la lista de acceso de %s .
CHAN_ACCESS_DELETED
	%s borrado de la lista de acceso de %s.
CHAN_ACCESS_DELETED_ONE
	1 instancia borrada de la lista de acceso de %s.
CHAN_ACCESS_DELETED_SEVERAL
	%d instancias borradas de la lista de acceso de %s.
CHAN_ACCESS_LIST_EMPTY
	La lista de acceso de %s esta vacia.
CHAN_ACCESS_LIST_HEADER
	Lista de acceso para %s:
	  Num   Niv  Nick
CHAN_ACCESS_LIST_FOOTER
	Fin de la lista de acceso.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	La lista de acceso para el canal %s ha sido limpiada.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK canal {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [nick-o-mascara] [razon]
CHAN_AKICK_DISABLED
	Lo siento, la modificacion de kicks automaticos en canales esta temporalmente deshabilitada.
CHAN_AKICK_ALREADY_EXISTS
	%s ya esta presente en la lista de kick automatico en el canal %s.
CHAN_AKICK_REACHED_LIMIT
	Lo siento, solo puedes tener %d mascaras para kicks automaticos en un canal.
CHAN_AKICK_ADDED
	%s añadido a la lista de kicks automaticos en el canal %s.
CHAN_AKICK_NO_SUCH_ENTRY
	La instancia (#%d) no existe en la lista de kicks automaticos en el canal %s.
CHAN_AKICK_NOT_FOUND
	%s no encontrado en la lista de kicks automaticos en el canal %s.
CHAN_AKICK_NO_MATCH
	No existen instancias que concuerden en la lista de kicks automaticos en el canal %s.
CHAN_AKICK_STUCK
	%s esta ahora siempre activo en el canal %s.
CHAN_AKICK_UNSTUCK
	%s ya no esta siempre activo en el canal %s.
CHAN_AKICK_DELETED
	%s borrado de la lista de kicks automaticos del canal %s.
CHAN_AKICK_DELETED_ONE
	1 instancia borrada de la lista de kicks automaticos del canal %s.
CHAN_AKICK_DELETED_SEVERAL
	%d instancias borradas de la lista de kicks automaticos del canal %s.
CHAN_AKICK_LIST_EMPTY
	La lista de kicks automaticos del canal %s esta vacia.
CHAN_AKICK_LIST_HEADER
	Lista de kicks automaticos para el canal %s:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (por %s en %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (pegado) (por %s en %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE en %s completa; %d usuarios afectados.
CHAN_AKICK_CLEAR
	La lista de kicks automaticos del canal %s ha sido limpiada.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS canal {SET | DIS[ABLE] | LIST | RESET} [item [nivel]]
CHAN_LEVELS_XOP
	Este comando es inservible en este modo.
CHAN_LEVELS_RANGE
	El nivel debe ser entre %d a %d inclusive.
CHAN_LEVELS_CHANGED
	Nivel para %s en el canal %s cambiado a %d.
CHAN_LEVELS_UNKNOWN
	Opcion %s no conocida. Tipea %R%s HELP LEVELS DESC para una lista de opciones validas.
CHAN_LEVELS_DISABLED
	%s inhabilitado(s) en el canal %s.
CHAN_LEVELS_LIST_HEADER
	Opciones del nivel de acceso para el canal %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (inhabilitado)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (solo el fundador)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	Niveles de acceso para %s resetados a los valores originales.

# Status Messages
CHAN_STATUS_SYNTAX
	STATUS ERROR Error de sintaxis
CHAN_STATUS_NOT_REGGED
	STATUS ERROR El canal %s no esta registrado
CHAN_STATUS_FORBIDDEN
	STATUS ERROR El canal %s esta prohibido
CHAN_STATUS_NOTONLINE
	STATUS ERROR El nick %s no esta online
CHAN_STATUS_INFO
	STATUS %s %s %d

# INFO responses
CHAN_INFO_SYNTAX
	INFO canal [ALL]
CHAN_INFO_HEADER
	Informacion para el canal %s:
CHAN_INFO_FOUNDER
	          Fundador: %s (%s)
CHAN_INFO_NO_FOUNDER
	          Fundador: %s
CHAN_INFO_SUCCESSOR
	           Sucesor: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	           Sucesor: %s
CHAN_INFO_DESCRIPTION
	       Descripcion: %s
CHAN_INFO_ENTRYMSG
	Mensaje de entrada: %s
CHAN_INFO_TIME_REGGED
	        Registrado: %s
CHAN_INFO_LAST_USED
	  Ultima vez usado: %s
CHAN_INFO_LAST_TOPIC
	      Ultimo topic: %s
CHAN_INFO_TOPIC_SET_BY
	 Topic seteado por: %s
CHAN_INFO_URL
	               URL: %s
CHAN_INFO_EMAIL
	  Direccion e-mail: %s
CHAN_INFO_BANTYPE
	       Tipo de ban: %d
CHAN_INFO_OPTIONS
	          Opciones: %s
CHAN_INFO_OPT_KEEPTOPIC
	Retencion de Topic
CHAN_INFO_OPT_OPNOTICE
	Noticia de OP
CHAN_INFO_OPT_PEACE
	Paz
CHAN_INFO_OPT_PRIVATE
	Privado
CHAN_INFO_OPT_RESTRICTED
	Acceso Restringido
CHAN_INFO_OPT_SECURE
	Seguro
CHAN_INFO_OPT_SECUREOPS
	Ops Seguros
CHAN_INFO_OPT_SECUREFOUNDER
	Fundador Seguro
CHAN_INFO_OPT_SIGNKICK
	Kicks Firmados
CHAN_INFO_OPT_TOPICLOCK
	Lock de topic
CHAN_INFO_OPT_XOP
	Sistema de listas xOP
CHAN_INFO_OPT_NONE
	Ninguno
CHAN_INFO_MODE_LOCK
	      Modo de lock: %s
CHAN_INFO_EXPIRE
	      Expira en: %s
CHAN_INFO_NO_EXPIRE
	Este canal no expirara.

# LIST responses
CHAN_LIST_SYNTAX
	LIST patron
CHAN_LIST_SERVADMIN_SYNTAX
	LIST patron [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
CHAN_LIST_HEADER
	Lista de instancias concordando a %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Fin de la lista - %d/%d concordancia(s) mostrada(s).
	
# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE canal

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN canal
CHAN_UNBANNED
	Has sido desbaneado de %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC canal [topico]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR canal modos-a-limpiar
CHAN_CLEARED_BANS
	Todos los bans en el canal %s han sido removidos.
CHAN_CLEARED_EXCEPTS
	Todas las excepciones en el canal %s han sido removidas.
CHAN_CLEARED_MODES
	Todos los modos en el canal %s han sido reseteados.
CHAN_CLEARED_OPS
	Modo +o ha sido removido del canal %s.
CHAN_CLEARED_HOPS
	El modo +h ha sido removido del canal %s.
CHAN_CLEARED_VOICES
	Modo +v ha sido removido del canal %s.
CHAN_CLEARED_USERS
	Todos los usuarios han sido kickeados del canal %s.
CHAN_CLEARED_INVITES
	Todos los invites en el canal %s han sido removidos.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS canal
CHAN_GETPASS_UNAVAILABLE
	Comando GETPASS no permitido porque encriptacion esta en uso.
CHAN_GETPASS_PASSWORD_IS
	La clave para el canal %s es %s.
	
# GETKEY responses
CHAN_GETKEY_SYNTAX
	GETKEY canal
CHAN_GETKEY_NOKEY
	El canal %s no tiene clave.
CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS canal
CHAN_SENDPASS_UNAVAILABLE
	Comando SENDPASS no permitido porque encriptacion esta en uso.
CHAN_SENDPASS_SUBJECT
	Clave del canal (%s)
CHAN_SENDPASS_HEAD
	Hola,
CHAN_SENDPASS_LINE_1
	Has solicitado recibir la clave del canal %s por e-mail.
CHAN_SENDPASS_LINE_2
	La clave es %s. Por razones de seguridad deberas cambiarla en cuanto recibas este mail.
CHAN_SENDPASS_LINE_3
	Si no sabes porque recibiste este mail, por favor ignoralo.
CHAN_SENDPASS_LINE_4
	POR FAVOR NO RESPONDER A ESTE EMAIL!
CHAN_SENDPASS_LINE_5
	Administradores de %s.
CHAN_SENDPASS_OK
	La clave de %s ha sido enviada.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID canal [razon]
CHAN_FORBID_SYNTAX_REASON
	FORBID canal razon
CHAN_FORBID_SUCCEEDED
	Canal %s ahora prohibido.
CHAN_FORBID_FAILED
	Imposible prohibir el canal %s!

# Misc responses
CHAN_FORBID_REASON
	Este canal ha sido prohibido.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND canal [razon]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND canal razon
CHAN_SUSPEND_SUCCEEDED
	Canal %s ahora prohibido.
CHAN_SUSPEND_FAILED
	Imposible suspender el canal %s!
CHAN_SUSPEND_REASON
	Este canal ha sido suspendido.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND canal
CHAN_UNSUSPEND_ERROR
	No se ha encontrado # en frente del nombre del canal.
CHAN_UNSUSPEND_SUCCEEDED
	Canal %s ahora liberado.
CHAN_UNSUSPEND_FAILED
	Imposible liberar el canal %s!

# Misc responses
CHAN_EXCEPTED
	%s coincide con una excepcion en %s y no puede ser baneado hasta que la excepcion haya sido removida.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	Tienes 1 memo nuevo.
MEMO_HAVE_NEW_MEMOS
	Tienes %d memos nuevos.
MEMO_TYPE_READ_LAST
	Tipea %R%s READ LAST para leer tu ultimo memo.
MEMO_TYPE_READ_NUM
	Tipea %R%s READ %d para leerlo.
MEMO_TYPE_LIST_NEW
	Tipea %R%s LIST NEW para listar tus memos.
MEMO_AT_LIMIT
	Advertencia: haz alcanzado el maximo numero de memos (%d). No podras recibir mas memos hasta que borres algunos de los existentes.
MEMO_OVER_LIMIT
	Advertencia: haz sobrepasado el maximo numero de memos (%d). Te sera imposible recibir mas memos hasta que borres algunos de los existentes.
MEMO_X_MANY_NOTICE
	Hay %d memos en el canal %s.
MEMO_X_ONE_NOTICE
	Hay %d memo en el canal %s.
MEMO_NEW_X_MEMO_ARRIVED
	Hay un memo nuevo en el canal %s.
	Tipea %R%s READ %s %d para leerlo.
MEMO_NEW_MEMO_ARRIVED
	Tienes un nuevo memo de %s.
	Tipea %R%s READ %d para leerlo.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	No tienes memos.
MEMO_X_HAS_NO_MEMOS
	%s no tiene memos.
MEMO_DOES_NOT_EXIST
	El memo %d no existe!
MEMO_LIST_NOT_FOUND
	No existen memos que concuerden.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {nick | canal} texto
MEMO_SEND_DISABLED
	Lo siento, el envio de memos esta temporalmente inhabilitado.
MEMO_SEND_PLEASE_WAIT
	Por favor espera %d segundos antes de utilizar el comando SEND nuevamente.
MEMO_X_GETS_NO_MEMOS
	%s no puede recibir memos.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s actualmente tiene muchos memos y no puede recibir mas.
MEMO_SENT
	Memo enviado a %s.
	
# CANCEL responses
MEMO_MASS_SENT
	Un massmemo ha sido enviado a todos los usuarios registrados.
	
# STAFF responses
MEMO_STAFF_SYNTAX
	STAFF memo-text
	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {nick | canal}
MEMO_CANCEL_DISABLED
	Lo siento, la cancelacion de memos esta temporalmente inabilitada.
MEMO_CANCEL_NONE
	Ningun memo pudo ser cancelado.
MEMO_CANCELLED
	Ultimo memo enviado a %s fue cancelado.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [canal] [lista | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	No tienes memos nuevos.
MEMO_X_HAS_NO_NEW_MEMOS
	%s no tiene memos nuevos.
MEMO_LIST_MEMOS
	Memos para %s. Para leerlos tipea: %R%s READ numero
MEMO_LIST_NEW_MEMOS
	Nuevos memos para %s. Para leerlos tipea: %R%s READ numero
MEMO_LIST_CHAN_MEMOS
	Memos para %s. Para leerlos tipea: %R%s READ %s numero
MEMO_LIST_CHAN_NEW_MEMOS
	Nuevos memos para %s. Para leerlos tipea: %R%s READ %s numero
MEMO_LIST_HEADER
	 Num  De                Fecha/Hora
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [canal] {lista | LAST | NEW}
MEMO_HEADER
	Memo %d de %s (%s). Para borrarlo tipea: %R%s DEL %d
MEMO_CHAN_HEADER
	Memo %d de %s (%s). Para borrarlo tipea: %R%s DEL %s %d
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [canal] {numero | lista | ALL}
MEMO_DELETED_NONE
	Ningun memo fue borrado.
MEMO_DELETED_ONE
	Memo %d ha sido borrado.
MEMO_DELETED_SEVERAL
	Los memos %s han sido borrados.
MEMO_DELETED_ALL
	Todos tus memos han sido borrados.

# SET responses
MEMO_CHAN_DELETED_ALL
	Todos los memos para el canal %s han sido borrados.

# SET responses
MEMO_SET_SYNTAX
	SET opcion parametros
MEMO_SET_DISABLED
	Lo siento, el seteo de opciones para memos esta temporalmente inhabilitado.
MEMO_SET_UNKNOWN_OPTION
	Opcion SET %s desconocida.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
MEMO_SET_NOTIFY_ON
	%s ahora te notificara de tus memos cuando te conectes y cuando te sean enviados.
MEMO_SET_NOTIFY_LOGON
	%s ahora te notificara de tus memos cuando te conectes o saques tu /AWAY.
MEMO_SET_NOTIFY_NEW
	%s ahora te notificara de tus memos cuando te sean enviados.
MEMO_SET_NOTIFY_OFF
	%s no te enviara ninguna notificacion acerca de tus memos.

# SET LIMIT responses
MEMO_SET_NOTIFY_MAIL
	Seras informado de nuevos memos via email.
MEMO_SET_NOTIFY_NOMAIL
	No seras informado de nuevos memos via email.
MEMO_SET_NOTIFY_INVALIDMAIL
	Tu nick no tiene una direccion de e-mail.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [canal] limite
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [usuario | canal] {limite | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	No estas autorizado a cambiar tu limite de memos.
MEMO_SET_LIMIT_FORBIDDEN
	El limite de memos para %s no puede ser cambiado.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	No puedes setear tu limite de memos mayor a %d.
MEMO_SET_LIMIT_TOO_HIGH
	No puedes setear el limite de memos para %s mayor a %d.
MEMO_SET_LIMIT_OVERFLOW
	Limite de memos muy alto; limitandolo a %d en vez.
MEMO_SET_YOUR_LIMIT
	Tu limite de memos ha sido seteado a %d.
MEMO_SET_YOUR_LIMIT_ZERO
	No te sera posible recibir mas memos.
MEMO_UNSET_YOUR_LIMIT
	Tu limite de memos ha sido deshabilitado.
MEMO_SET_LIMIT
	Limite de memos para %s seteado a %d.
MEMO_SET_LIMIT_ZERO
	Limite de memos para %s seteado a 0.
MEMO_UNSET_LIMIT
	Limite de memos para %s deshabilitado.

# INFO responses
MEMO_INFO_SYNTAX
	INFO [canal]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [nick | canal]
MEMO_INFO_NO_MEMOS
	No tienes memos.
MEMO_INFO_MEMO
	Tienes 1 memo.
MEMO_INFO_MEMO_UNREAD
	Tienes 1 memo y aun no ha sido leido.
MEMO_INFO_MEMOS
	Tienes %d memos.
MEMO_INFO_MEMOS_ONE_UNREAD
	Tienes %d memos, de los cuales 1 aun no ha sido leido.
MEMO_INFO_MEMOS_SOME_UNREAD
	Tienes %d memos, de los cuales %d aun no han sido leidos.
MEMO_INFO_MEMOS_ALL_UNREAD
	Tienes %d memos, ninguno de ellos leido.
MEMO_INFO_LIMIT
	Tu limite de memos es %d.
MEMO_INFO_HARD_LIMIT
	Tu limite de memos es %d y no puede ser cambiado.
MEMO_INFO_LIMIT_ZERO
	Tu limite de memos es 0; No recibiras nuevos memos.
MEMO_INFO_HARD_LIMIT_ZERO
	Tu limite de memos es 0; No recibiras nuevos memos. No puedes cambiar este limite.
MEMO_INFO_NO_LIMIT
	No tienes limite en el numero de memos que puedes mantener.
MEMO_INFO_NOTIFY_OFF
	No seras notificado por memos nuevos.
MEMO_INFO_NOTIFY_ON
	Seras notificado por nuevos memos cuando te conectes y en cuanto te sean enviados.
MEMO_INFO_NOTIFY_RECEIVE
	Seras notificado cuando los memos nuevos te sean enviados.
MEMO_INFO_NOTIFY_SIGNON
	Seras notificado por memos nuevos en cuanto te conectes.
MEMO_INFO_X_NO_MEMOS
	%s no tiene memos.
MEMO_INFO_X_MEMO
	%s tiene 1 memo.
MEMO_INFO_X_MEMO_UNREAD
	%s actualmente tiene 1 memo y aun no ha sido leido.
MEMO_INFO_X_MEMOS
	%s actualmente tiene %d memos.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s actualmente tiene %d memos, de los cuales 1 aun no ha sido leido.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s actualmente tienes %d memos, de los cuales %d aun no han sido leidos.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s actualmente tienes %d memos, de los cuales ninguno ha sido leido .
MEMO_INFO_X_LIMIT
	El limite de memos para %s es %d.
MEMO_INFO_X_HARD_LIMIT
	El limite de memos para %s es %d, y no puede ser cambiado.
MEMO_INFO_X_NO_LIMIT
	%s no tiene limite para memos.
MEMO_INFO_X_NOTIFY_OFF
	%s no es notificado por memos nuevos.
MEMO_INFO_X_NOTIFY_ON
	%s es notificado por memos nuevos cuando se conecta y cuando le son enviados.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s es notificado cuando nuevos memos le son enviados.
MEMO_INFO_X_NOTIFY_SIGNON
	%s es notificado de nuevos memos cuando se conecta.

# Standard responses
MEMO_MAIL_SUBJECT
	Nuevo memo
MEMO_MAIL_TEXT1
	Hola %s
MEMO_MAIL_TEXT2
	Acabas de recibir un nuevo memo de %s. Es el memo numero %d.
MEMO_MAIL_TEXT3
	Texto del Memo:

# Standard responses
MEMO_RSEND_PLEASE_WAIT
	Por favor espera %d segundos antes de usar el comando RSEND nuevamente.
MEMO_RSEND_DISABLED
	Lo siento, RSEND ha sido deshabilitado en esta red.
MEMO_RSEND_SYNTAX
	RSEND {nick | canal} texto-memo
MEMO_RSEND_NICK_MEMO_TEXT
	[auto-memo] El memo que enviaste ha sido leido.
MEMO_RSEND_CHAN_MEMO_TEXT
	[auto-memo] El memo que enviaste a %s ha sido leido.
MEMO_RSEND_USER_NOTIFICATION
	Una notificacion ha sido enviada a %s informando que has leido su memo.

# Standard responses
MEMO_CHECK_SYNTAX
	CHECK nickname
MEMO_CHECK_NOT_READ
	El ultimo memo que enviaste a %s (enviado el %s) aun no ha sido leido.
MEMO_CHECK_READ
	El ultimo memo que enviaste a %s (enviado el %s) ha sido leido.
MEMO_CHECK_NO_MEMO
	El nick %s no ha recibido ningun memo tuyo.
MEMO_NO_RSEND_SELF
	No puedes pedir una notificacion cuando te envias el memo a ti mismo.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
BOT_DOES_NOT_EXIST
	El bot %s no existe.
BOT_NOT_ASSIGNED
	Debes asignar un bot al canal antes de poder usar este comando.
	Tipea %R%S HELP ASSIGN para mayor informacion.
BOT_NOT_ON_CHANNEL
	El bot no esta en el canal %s.
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	No uses la palabra %s en este canal!
BOT_REASON_BADWORD_GENTLE
	Cuida tu lenguaje!
BOT_REASON_BOLD
	No uses negritas en este canal!
BOT_REASON_CAPS
	Apaga las mayusculas!
BOT_REASON_COLOR
	No uses colores en este canal!
BOT_REASON_FLOOD
	Para de floodear!
BOT_REASON_REPEAT
	Para de repetir!
BOT_REASON_REVERSE
	No uses reversos en este canal!
BOT_REASON_UNDERLINE
	No uses subrayados en este canal!
	
# !seen replies
BOT_SEEN_BOT
	Me encontraste %s!
BOT_SEEN_YOU
	Buscandote a ti mismo, eh %s?
BOT_SEEN_ON_CHANNEL
	%s esta en el canal en estos instantes!
BOT_SEEN_ON_CHANNEL_AS
	%s esta en el canal en estos instantes (como %s) ! 
BOT_SEEN_ON
	%s fue visto aqui hace %s.
BOT_SEEN_NEVER
	Jamas he visto a %s en este canal.
BOT_SEEN_UNKNOWN
	Jamas he visto a %s.
	
# BOT responses
BOT_BOT_SYNTAX
	BOT ADD nick usuario host nombre-real
	BOT CHANGE nick-antiguo nick-nuevo [usuario [host [nombre-real]]]
	BOT DEL nick
BOT_BOT_ALREADY_EXISTS
	El bot %s ya existe.
BOT_BOT_CREATION_FAILED
	Lo siento, la creacion de bot ha fallado
BOT_BOT_READONLY
	Lo siento, la modificacion de bots esta temporalmente inabilitada.
BOT_BOT_ADDED
	%s!%s@%s (%s) añadido a la lista de bots.
BOT_BOT_ANY_CHANGES
	La informacion antigua es igual a la nueva.
BOT_BOT_CHANGED
	El bot %s ha sido cambiado a %s!%s@%s (%s)
BOT_BOT_DELETED
	El bot %s ha sido borrado.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Lista de bots:
BOT_BOTLIST_PRIVATE_HEADER
	Bots reservados a operadores de IRC:
BOT_BOTLIST_FOOTER
	%d bots disponibles.
BOT_BOTLIST_EMPTY
	No hay bots disponibles en este momento.
	Pidele a un administrador de servicios que cree uno!
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN canal nick
BOT_ASSIGN_READONLY
	Lo siento, el asignamiento de bots esta temporalmente inhabilitado.
BOT_ASSIGN_ALREADY
	Bot %s ya esta asignado al canal %s.
BOT_ASSIGN_ASSIGNED
	El bot %s ha sido asignado al canal %s.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN canal
BOT_UNASSIGN_UNASSIGNED
	Ya no hay un bot asignado a %s.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {canal | nick}
BOT_INFO_NOT_FOUND
	%s no es un bot valido o el canal no esta registrado.
BOT_INFO_BOT_HEADER
	Informacion para el bot %s:
BOT_INFO_BOT_MASK
	       Mascara : %s@%s
BOT_INFO_BOT_REALNAME
	  Nombre real : %s
BOT_INFO_BOT_CREATED
	    Creado : %s
BOT_INFO_BOT_USAGE
	    Usado en : %d canal(es)
BOT_INFO_BOT_OPTIONS
	    Opciones : %s
BOT_INFO_OPT_PRIVATE
	Privado
BOT_INFO_CHAN_HEADER
	Informacion para el canal %s:
BOT_INFO_CHAN_BOT
	           Nick del bot : %s
BOT_INFO_CHAN_BOT_NONE
	           Nick del bot : Aun no asignado.
BOT_INFO_CHAN_KICK_BADWORDS
	   Kicker por Bad words : %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	   Kicker por Bad words : %s (%d kick(s) para ban)
BOT_INFO_CHAN_KICK_BOLDS
	    Kicker por negritas : %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	    Kicker por negritas : %s (%d kick(s) para ban)
BOT_INFO_CHAN_KICK_CAPS_ON
	            Caps kicker : %s (minimo %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	  Kicker por mayusculas : %s (%d kick(s) para ban; minimo %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	  Kicker por mayusculas : %s
BOT_INFO_CHAN_KICK_COLORS
	     Kicker por colores : %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	     Kicker por colores : %s (%d kick(s) para ban)
BOT_INFO_CHAN_KICK_FLOOD_ON
	       Kicker por flood : %s (%d lineas en %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	       Kicker por flood : %s (%d kick(s) para ban; %d lineas en %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	       Kicker por flood : %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	  Kicker por repeticion : %s (%d veces)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	  Kicker por repeticion : %s (%d kick(s) para ban; %d veces)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	  Kicker por repeticion : %s
BOT_INFO_CHAN_KICK_REVERSES
	    Kicker por inversos : %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	    Kicker por inversos : %s (%d kick(s) para ban)
BOT_INFO_CHAN_KICK_UNDERLINES
	  Kicker por subrayados : %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	  Kicker por subrayados : %s (%d kick(s) para ban)
BOT_INFO_ACTIVE
	Activada
BOT_INFO_INACTIVE
	Desactivada
BOT_INFO_CHAN_OPTIONS
	               Opciones : %s
BOT_INFO_OPT_DONTKICKOPS
	Proteccion de Ops
BOT_INFO_OPT_DONTKICKVOICES
	Proteccion de Voices
BOT_INFO_OPT_FANTASY
	Fantasia
BOT_INFO_OPT_GREET
	Saludo
BOT_INFO_OPT_NOBOT
	Sin bot
BOT_INFO_OPT_SYMBIOSIS
	Simbiosis
BOT_INFO_OPT_NONE
	Ninguna
	
# SET responses
BOT_SET_SYNTAX
	SET canal opcion seteos
BOT_SET_DISABLED
	Lo siento, el seteo de opciones para bot esta temporalmente inhabilitado
BOT_SET_UNKNOWN
	Opcion %s desconocida.
	Tipea %R%S HELP SET para mayor informacion.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET canal DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Bot no kickeara ops en el canal %s.
BOT_SET_DONTKICKOPS_OFF
	Bot kickeara ops en el canal %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET canal DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Bot no kickeara voices en el canal %s.
BOT_SET_DONTKICKVOICES_OFF
	Bot kickeara voices en el canal %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET canal FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Modo fantasia ahora Activado en el canal %s.
BOT_SET_FANTASY_OFF
	Modo fantasia ahora Desactivado en el canal %s.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET canal GREET {ON|OFF}
BOT_SET_GREET_ON
	Modo saludo ahora Activado en el canal %s.
BOT_SET_GREET_OFF
	Modo saludo ahora Desactivado en el canal %s.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET canal NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Modo no bot ahora Activado en el canal %s.
BOT_SET_NOBOT_OFF
	Modo no bot ahora Desactivado en el canal %s.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET canal PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Modo privado del bot %s esta ahora Activado.
BOT_SET_PRIVATE_OFF
	Modo privado del bot %s esta ahora Desactivado.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET canal SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Modo Simbiosis ahora Activado en el canal %s.
BOT_SET_SYMBIOSIS_OFF
	Modo Simbiosis ahora Desactivado en el canal %s.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK canal opcion {ON|OFF} [seteos]
BOT_KICK_DISABLED
	Lo siento, la configuracion de kicker esta temporalmente inabilitada.
BOT_KICK_UNKNOWN
	Opcion %s desconocida.
	Tipea %R%S HELP KICK para mayor informacion
BOT_KICK_BAD_TTB
	%s no puede ser tomado como veces a banear

BOT_KICK_BADWORDS_ON
	El bot ahora kickeara bad words. Usa el comando BADWORDS
	para añadir o remover palabras.
BOT_KICK_BADWORDS_ON_BAN
	El bot ahora kickeara bad words, y baneara despues de
	%d kicks al mismo usuario. Usa el comando BADWORDS
	para añadir o remover palabras.
BOT_KICK_BADWORDS_OFF
	El bot ya no kickeara bad words. 
BOT_KICK_BOLDS_ON
	El bot ahora kickeara negritas.
BOT_KICK_BOLDS_ON_BAN
	El bot ahora kickeara negritas, y baneara despues de 
	%d kicks al mismo usuario.
BOT_KICK_BOLDS_OFF
	El bot ya no kickeara negritas.
BOT_KICK_CAPS_ON
	El bot ahora kickeara mayusculas (deben ser al menos
	%d caracteres y %d%% del mensaje entero).
BOT_KICK_CAPS_ON_BAN
	El bot ahora kickeara mayusculas (deben ser al menos
	%d caracteres y %d%% del mensaje entero), y baneara
	despues de %d kicks al mismo usuario.
BOT_KICK_CAPS_OFF
	El bot ya no kickeara mayusculas.
BOT_KICK_COLORS_ON
	El bot ahora kickeara colores.
BOT_KICK_COLORS_ON_BAN
	El bot ahora kickeara colores, y baneara despues de %d 
	kicks al mismo usuario.
BOT_KICK_COLORS_OFF
	El bot ya no kickeara colores.
BOT_KICK_FLOOD_ON
	El bot ahora kickeara flood (%d lineas en %d segundos).
BOT_KICK_FLOOD_ON_BAN
	El bot ahora kickeara flood (%d lineas in %d segundos), y 
	baneara despues de %d kicks al mismo usuario.
BOT_KICK_FLOOD_OFF
	El bot ya no kickeara flood.
BOT_KICK_REPEAT_ON
	El bot ahora kickeara repeats (usuarios que repitan %d veces
	lo mismo).
BOT_KICK_REPEAT_ON_BAN
	El bot ahora kickeara repeats (usuarios que repitan %d veces
	lo mismo), y baneara despues de %d 
	kicks al mismo usuario.
BOT_KICK_REPEAT_OFF
	El bot ya no kickeara repeats.
BOT_KICK_REVERSES_ON
	El bot ahora kickeara reversos.
BOT_KICK_REVERSES_ON_BAN
	El bot ahora kickeara reversos, y baneara despues de %d 
	kicks al mismo usuario.
BOT_KICK_REVERSES_OFF
	El bot ya no kickeara reversos.
BOT_KICK_UNDERLINES_ON
	El bot ahora kickeara subrayados.
BOT_KICK_UNDERLINES_ON_BAN
	El bot ahora kickeara subrayados, y baneara despues de %d 
	kicks al mismo usuario.
BOT_KICK_UNDERLINES_OFF
	El bot ya no kickeara subrayados.
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS canal {ADD|DEL|LIST|CLEAR} [palabra | lista] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	Lo siento, la lista de  bad words para canales esta temporalmente inabilitada.
BOT_BADWORDS_REACHED_LIMIT
	Lo siento, solo puedes tener %d bad words en un canal.
BOT_BADWORDS_ALREADY_EXISTS
	%s ya existe en la lista de bad words de %s.
BOT_BADWORDS_ADDED
	%s añadida a  la lista de bad words de  %s.
BOT_BADWORDS_NO_SUCH_ENTRY
	No existe la instancia (#%d) en la lista de bad words de %s.
BOT_BADWORDS_NOT_FOUND
	%s no encontrada en la lista de bad words de %s.
BOT_BADWORDS_NO_MATCH
	No existen palabras que concuerden en la lista de bad words de %s.
BOT_BADWORDS_DELETED
	%s borrada de la lista de bad words de %s.
BOT_BADWORDS_DELETED_ONE
	1 palabra borrada de la lista de bad words de %s.
BOT_BADWORDS_DELETED_SEVERAL
	%d palabras borradas de la lista de bad words de %s.
BOT_BADWORDS_LIST_EMPTY
	La lista de bad words de %s esta vacia.
BOT_BADWORDS_LIST_HEADER
	Lista de Bad words para %s:
	  Num   Palabra                        Tipo
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	La lista de bad words esta ahora vacia.

# SAY responses
BOT_SAY_SYNTAX
	SAY canal texto

# ACT responses
BOT_ACT_SYNTAX
	ACT canal texto

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
BOT_EXCEPT
	Usuario coincide con el except del canal.

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
BOT_BAD_NICK
	Los nicks de bots pueden solo contener caracteres de nicks validos.
BOT_BAD_HOST
	Los hosts de bots pueden contener solo caracteres de hosts validos.
BOT_BAD_IDENT
	Los idents de bots pueden contener solo caracteres validos.
BOT_LONG_IDENT
	Los Idents de bots pueden tener solo %d caracteres.
BOT_LONG_HOST
	Los Hosts de bots pueden tener solo %d caracteres.
###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	Los Servicios no han podido cambiar modos. Estan bien configurados tus servidores?
OPER_BOUNCY_MODES_U_LINE
	Los Servicios no han podido cambiar modos. Estan las lineas U: de tus servidores configuradas correctamente?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL mensaje

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Opcion STATS %s desconocida.
OPER_STATS_CURRENT_USERS
	Actuales usuarios: %d (%d ops)
OPER_STATS_MAX_USERS
	Maximo  numero de usuarios: %d (%s)
# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Servicios arriba %d dias, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Servicios arriba %d dias, %02d:%02d
OPER_STATS_UPTIME_HMS
	Servicios arriba %d horas, %d minutos
OPER_STATS_UPTIME_HM1S
	Servicios arriba %d horas, %d minutos
OPER_STATS_UPTIME_H1MS
	Servicios arriba %d horas, %d minuto
OPER_STATS_UPTIME_H1M1S
	Servicios arriba %d horas, %d minuto
OPER_STATS_UPTIME_1HMS
	Servicios arriba %d hora, %d minutos
OPER_STATS_UPTIME_1HM1S
	Servicios arriba %d hora, %d minutos
OPER_STATS_UPTIME_1H1MS
	Servicios arriba %d hora, %d minuto
OPER_STATS_UPTIME_1H1M1S
	Servicios arriba %d hora, %d minuto
OPER_STATS_UPTIME_MS
	Servicios arriba %d minutos, %d segundos
OPER_STATS_UPTIME_M1S
	Servicios arriba %d minutos, %d segundo
OPER_STATS_UPTIME_1MS
	Servicios arriba %d minuto, %d segundos
OPER_STATS_UPTIME_1M1S
	Servicios arriba %d minuto, %d segundo
OPER_STATS_BYTES_READ
	Bytes leidos   : %5d kB
OPER_STATS_BYTES_WRITTEN
	Bytes escritos : %5d kB
OPER_STATS_USER_MEM
	Usuario      : %6d records, %5d kB
OPER_STATS_CHANNEL_MEM
	Canal        : %6d records, %5d kB
OPER_STATS_GROUPS_MEM
	Grupos NS    : %6d records, %5d kB
OPER_STATS_ALIASES_MEM
	Aliases NS   : %6d records, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ     : %6d records, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ      : %6d records, %5d kB
OPER_STATS_HOSTSERV_MEM
	HostServ      : %6d records, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ     : %6d records, %5d kB
OPER_STATS_SESSIONS_MEM
	Sesiones     : %6d records, %5d kB
OPER_STATS_AKILL_COUNT
	Numero actual de AKILLs: %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Tiempo de expiracion por defecto de AKILL: %d dias
OPER_STATS_AKILL_EXPIRE_DAY
	Tiempo de expiracion por defecto de AKILL: 1 dia
OPER_STATS_AKILL_EXPIRE_HOURS
	Tiempo de expiracion por defecto de AKILL: %d horas
OPER_STATS_AKILL_EXPIRE_HOUR
	Tiempo de expiracion por defecto de AKILL: 1 hora
OPER_STATS_AKILL_EXPIRE_MINS
	Tiempo de expiracion por defecto de AKILL: %d minutos
OPER_STATS_AKILL_EXPIRE_MIN
	Tiempo de expiracion por defecto de AKILL: 1 minuto
OPER_STATS_AKILL_EXPIRE_NONE
	Tiempo de expiracion por defecto de AKILL: Sin expiracion
OPER_STATS_SGLINE_COUNT
	Numero actual de SGLINEs: %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Tiempo de expiracion por defecto de SGLINE: %d dias
OPER_STATS_SGLINE_EXPIRE_DAY
	Tiempo de expiracion por defecto de SGLINE: 1 dia
OPER_STATS_SGLINE_EXPIRE_HOURS
	Tiempo de expiracion por defecto de SGLINE: %d horas
OPER_STATS_SGLINE_EXPIRE_HOUR
	Tiempo de expiracion por defecto de SGLINE: 1 hora
OPER_STATS_SGLINE_EXPIRE_MINS
	Tiempo de expiracion por defecto de SGLINE: %d minutos
OPER_STATS_SGLINE_EXPIRE_MIN
	Tiempo de expiracion por defecto de SGLINE: 1 minuto
OPER_STATS_SGLINE_EXPIRE_NONE
	Tiempo de expiracion por defecto de SGLINE: Sin expiracion
OPER_STATS_SQLINE_COUNT
	Numero actual de SQLINEs: %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Tiempo de expiracion por defecto de SQLINE: %d dias
OPER_STATS_SQLINE_EXPIRE_DAY
	Tiempo de expiracion por defecto de SQLINE: 1 dia
OPER_STATS_SQLINE_EXPIRE_HOURS
	Tiempo de expiracion por defecto de SQLINE: %d horas
OPER_STATS_SQLINE_EXPIRE_HOUR
	Tiempo de expiracion por defecto de SQLINE: 1 hora
OPER_STATS_SQLINE_EXPIRE_MINS
	Tiempo de expiracion por defecto de SQLINE: %d minutos
OPER_STATS_SQLINE_EXPIRE_MIN
	Tiempo de expiracion por defecto de SQLINE: 1 minuto
OPER_STATS_SQLINE_EXPIRE_NONE
	Tiempo de expiracion por defecto de SQLINE: Sin expiracion
OPER_STATS_SZLINE_COUNT
	Numero actual de SZLINEs: %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Tiempo de expiracion por defecto de SZLINE: %d dias
OPER_STATS_SZLINE_EXPIRE_DAY
	Tiempo de expiracion por defecto de SZLINE: 1 dia
OPER_STATS_SZLINE_EXPIRE_HOURS
	Tiempo de expiracion por defecto de SZLINE: %d horas
OPER_STATS_SZLINE_EXPIRE_HOUR
	Tiempo de expiracion por defecto de SZLINE: 1 hora
OPER_STATS_SZLINE_EXPIRE_MINS
	Tiempo de expiracion por defecto de SZLINE: %d minutos
OPER_STATS_SZLINE_EXPIRE_MIN
	Tiempo de expiracion por defecto de SZLINE: 1 minuto
OPER_STATS_SZLINE_EXPIRE_NONE
	Tiempo de expiracion por defecto de SZLINE: Sin expiracion
OPER_STATS_RESET
	Estadisticas reseteadas.

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

# CLEARMODES responses
OPER_UMODE_SYNTAX
	UMODE nick modos

# OLINE responses
OPER_UMODE_SUCCESS
	Cambiados los modos de usuario de %s.
OPER_UMODE_CHANGED
	%s cambio tus modos de usuario.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE nick opciones
OPER_OLINE_SUCCESS
	Una O:Line con las opciones %s ha sido agregada para %s.

# CLEARMODES responses
OPER_OLINE_IRCOP
	Eres ahora un operador de IRC.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES canal [ALL]
OPER_CLEARMODES_DONE
	Modos binarios y bans limpiados del canal %s.
OPER_CLEARMODES_ALL_DONE
	Todos los modos limpiados del canal %s.


# KICK responses
OPER_KICK_SYNTAX
	KICK canal usuario razon
	
# ADMIN responses
OPER_SVSNICK_SYNTAX
	SVSNICK nick nuevonick 
OPER_SVSNICK_NEWNICK
	El nick %s esta siendo cambiado a %s.
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [nick | lista]
OPER_ADMIN_SKELETON
	Los Servicios estan en modo skeleton; El comando ADMIN no esta disponible.
OPER_ADMIN_EXISTS
	%s ya esta presente en la lista de administradores de Servicios.
OPER_ADMIN_REACHED_LIMIT
	Lo siento, solo puedes tener %d administradores de Servicios.
OPER_ADMIN_ADDED
	%s añadido a la lista de administradores de Servicios.
OPER_ADMIN_NOT_FOUND
	%s no encontrado en la lista de administradores de Servicios.
OPER_ADMIN_NO_MATCH
	No existen instancias similares en la lista de administradores de Servicios.
OPER_ADMIN_DELETED
	%s borrado de la lista de administradores de Servicios.
OPER_ADMIN_DELETED_ONE
	1 Instancia borrada de la lista de administradores de Servicios.
OPER_ADMIN_DELETED_SEVERAL
	%d instancias borradas de la lista de administradores de Servicios.
OPER_ADMIN_LIST_EMPTY
	La lista de administradores de Servicios esta vacia.
OPER_ADMIN_LIST_HEADER
	Lista de administradores de Servicios:
	  Num   Nick
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	La lista de administradores de Servicios ha sido limpiada.
OPER_ADMIN_MOVED
	%s ha sido movido a la lista de Administradores de Servicios.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | lista]
OPER_OPER_SKELETON
	Los Servicios estan en modo skeleton; El comando OPER no esta disponible.
OPER_OPER_EXISTS
	%s ya existe en la lista de operadores de Servicios.
OPER_OPER_REACHED_LIMIT
	Lo siento, solo puedes tener %d operadores de Servicios.
OPER_OPER_ADDED
	%s añadido a la lista de operadores de Servicios.
OPER_OPER_NOT_FOUND
	%s no encontrado en la lista de operadores de Servicios.
OPER_OPER_NO_MATCH
	No existen instancias similares en la lista de operadores de Servicios.
OPER_OPER_DELETED
	%s borrado de la lista de operadores de Servicios.
OPER_OPER_DELETED_ONE
	1 Instancia borrada de la lista de operadores de Servicios.
OPER_OPER_DELETED_SEVERAL
	%d instancias borradas de la lista de operadores de Servicios.
OPER_OPER_LIST_EMPTY
	La lista de administradores de Servicios esta vacia.
OPER_OPER_LIST_HEADER
	Lista de operadores de Servicios:
	  Num   Nick
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	La lista de operadores de Servicios ha sido limpiada.
OPER_OPER_MOVED
	%s ha sido movido a la lista de Operadores de Servicios.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+expiracion] {mascara | lista} [razon]]
OPER_AKILL_EXISTS
	%s ya esta presente en la lista de AKILLs.
OPER_AKILL_ALREADY_COVERED
	%s ya esta cubierto por %s.
OPER_AKILL_REACHED_LIMIT
	Lo siento, solo puedes tener %d AKILLs.
OPER_AKILL_NO_NICK
	Recordatorio: Las mascaras AKILL no pueden contener nicks; asegurate de no haber incluido el nick en tu mascara.
OPER_AKILL_ADDED
	%s añadido a la lista de AKILLs.
OPER_AKILL_CHANGED
	Tiempo de expiracion para %s cambiado.
OPER_AKILL_NOT_FOUND
	%s no encontrado en la lista de AKILLs.
OPER_AKILL_NO_MATCH
	No existen instancias similares en la lista de AKILLs.
OPER_AKILL_DELETED
	%s borrado de la lista de AKILLs.
OPER_AKILL_DELETED_ONE
	1 instancia borrada de la lista de AKILLs.
OPER_AKILL_DELETED_SEVERAL
	%d instancias borradas de la lista de AKILLs.
OPER_AKILL_LIST_EMPTY
	La lista de AKILLS esta vacia.
OPER_AKILL_LIST_HEADER
	Lista actual de AKILLs:
	  Num   Mascara                           Razon
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Lista actual de AKILLs:
# number, mask, set-by, set-time, expires, reason
OPER_AKILL_VIEW_FORMAT
	%3d  %s (por %s en %s; %s)
	      %s
OPER_AKILL_CLEAR
	La lista de AKILLs ha sido limpiada.
	
# SGLINE responses
OPER_CHANKILL_SYNTAX
	CHANKILL [+expiry] {#channel} [reason]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiracion] {mascara | lista}[:razon]]
OPER_SGLINE_UNSUPPORTED
	Lo siento, SGLINE no esta disponible en esta red.
OPER_SGLINE_EXISTS
	%s ya existe en la lista de SGLINEs.
OPER_SGLINE_ALREADY_COVERED
	%s ya esta cubierto por %s.
OPER_SGLINE_REACHED_LIMIT
	Lo siento, solo puedes tener %d SGLINEs.
OPER_SGLINE_ADDED
	%s añadido a la lista de SGLINEs.
OPER_SGLINE_CHANGED
	Tiempo de expiracion para %s cambiado.
OPER_SGLINE_NOT_FOUND
	%s no encontrado en la lista de SGLINEs.
OPER_SGLINE_NO_MATCH
	No existen instancias similares en la lista de SGLINEs.
OPER_SGLINE_DELETED
	%s borrado de la lista de SGLINEs.
OPER_SGLINE_DELETED_ONE
	1 instancia borrada de la lista de SGLINEs.
OPER_SGLINE_DELETED_SEVERAL
	%d instancias borradas de la lista de SGLINEs.
OPER_SGLINE_LIST_EMPTY
	La lista de SGLINEs esta vacia.
OPER_SGLINE_LIST_HEADER
	Lista actual de SGLINEs:
	  Num   Mascara                           Razon
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Lista actual de SGLINEs:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (por %s en %s; %s)
	      %s
OPER_SGLINE_CLEAR
	La lista de SGLINEs ha sido limpiada.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiracion] {mascara | lista} [razon]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	SQLINEs de canal no estan soportadas por tu IRCd, asi que no las puedes usar.
OPER_SQLINE_EXISTS
	%s ya existe en la lista de SQLINEs.
OPER_SQLINE_ALREADY_COVERED
	%s ya esta cubierto por %s.
OPER_SQLINE_REACHED_LIMIT
	Lo siento, solo puedes tener %d SQLINEs.
OPER_SQLINE_ADDED
	%s añadido a la lista de SQLINEs.
OPER_SQLINE_CHANGED
	Tiempo de expiracion para %s cambiado.
OPER_SQLINE_NOT_FOUND
	%s no encontrado en la lista de SQLINEs
OPER_SQLINE_NO_MATCH
	No existen instancias similares en la lista de SQLINEs.
OPER_SQLINE_DELETED
	%s borrado de la lista de SQLINEs.
OPER_SQLINE_DELETED_ONE
	1 instancia borrada de la lista de SQLINEs.
OPER_SQLINE_DELETED_SEVERAL
	%d instancias borradas de la lista de SQLINEs.
OPER_SQLINE_LIST_EMPTY
	La lista de SQLINEs esta vacia.
OPER_SQLINE_LIST_HEADER
	Lista actual de SQLINEs:
	  Num   Mascara                           Razon
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Lista actual de SQLINEs:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (por %s en %s; %s)
	      %s
OPER_SQLINE_CLEAR
	La lista de SQLINEs ha sido limpiada.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiracion] {mascara | lista} [razon]]
OPER_SZLINE_UNSUPPORTED
	Lo siento, el comando SZLINE no esta disponible en esta red.
OPER_SZLINE_EXISTS
	%s ya existe en la lista de SZLINEs.
OPER_SZLINE_ALREADY_COVERED
	%s ya esta cubierto por %s.
OPER_SZLINE_REACHED_LIMIT
	Lo siento, solo puedes tener %d SZLINEs.
OPER_SZLINE_ONLY_IPS
	Recordatorio: Solo puedes añadir mascaras IP a la lista de SZLINEs.
OPER_SZLINE_ADDED
	%s añadido a la lista de SZLINEs.
OPER_SZLINE_CHANGED
	Tiempo de expiracion de %s cambiado.
OPER_SZLINE_NOT_FOUND
	%s no encontrado en la lista de SZLINEs.
OPER_SZLINE_NO_MATCH
	No existen instancias similares en la lista de SZLINEs.
OPER_SZLINE_DELETED
	%s borrado de la lista de SZLINEs.
OPER_SZLINE_DELETED_ONE
	1 instancia borrada de la lista de SZLINEs.
OPER_SZLINE_DELETED_SEVERAL
	%d instancias borradas de la lista de SZLINEs.
OPER_SZLINE_LIST_EMPTY
	La lista de SZLINEs esta vacia.
OPER_SZLINE_LIST_HEADER
	Lista actual de SZLINEs:
	  Num   Mascara                            Razon
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Lista actual de SZLINEs:
# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (por %s en %s; %s)
	      %s
OPER_SZLINE_CLEAR
	La lista de SZLINEs ha sido limpiada.

# SET responses
OPER_SET_SYNTAX
	SET opcion seteo
OPER_SET_IGNORE_ON
	Codigo ignore sera usado.
OPER_SET_IGNORE_OFF
	Codigo ignore no sera usado.
OPER_SET_IGNORE_ERROR
	El seteo para IGNORE debe ser ON u OFF.
OPER_SET_READONLY_ON
	Servicios ahora en modo read-only.
OPER_SET_READONLY_OFF
	Servicios ahora en modo de read-write.
OPER_SET_READONLY_ERROR
	El seteo para READONLY debe ser ON u OFF.
OPER_SET_LOGCHAN_ON
	Servicios ahora reportando el log en %s.
OPER_SET_LOGCHAN_OFF
	Servicios ha dejado de reportal el log.
OPER_SET_LOGCHAN_ERROR
	Setting for LOGCHAN debe ser ON or OFF y LogChannel debe estar definido.
OPER_SET_DEBUG_ON
	Servicios ahora en modo debug.
OPER_SET_DEBUG_OFF
	Servicios ahora en modo no-debug.
OPER_SET_DEBUG_LEVEL
	Servicios ahora en modo debug (nivel %d).
OPER_SET_DEBUG_ERROR
	El seteo para DEBUG debe ser ON, OFF, o un numero positivo.
OPER_SET_NOEXPIRE_ON
	Servicios ahora en modo no expire.
OPER_SET_NOEXPIRE_OFF
	Servicios ahora en modo expire.
OPER_SET_NOEXPIRE_ERROR
	Seteo para NOEXPIRE debe ser ON u OFF.
OPER_SET_UNKNOWN_OPTION
	Opcion %s desconocida.
OPER_SET_SQL_ON
	El codigo SQL sera usado.
OPER_SET_SQL_OFF
	El codigo SQL no sera usado.
OPER_SET_SQL_ERROR
	Opciones para SQL son ON o OFF.
OPER_SET_SQL_ERROR_INIT
	Error durante el inicio de SQL, revisa tus logs para corregir.
OPER_SET_SQL_ERROR_DISABLED
	SQL no esta configurado para esta red. Por favor edita
	el archivo de configuracion (services.conf).
OPER_SET_LIST_OPTION_ON
	%s esta habilitado
OPER_SET_LIST_OPTION_OFF
	%s esta deshabilitado

# NOOP responses
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} servidor
OPER_NOOP_SET
	Todas las lineas O de %s han sido removidas.
OPER_NOOP_REVOKE
	Todas las lineas O de %s han sido reseteadas.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE nombre-del-servidor [razon]

# RAW responses
OPER_JUPE_HOST_ERROR
	Por favor usa un nombre de servidor valido cuando use JUPE
OPER_JUPE_INVALID_SERVER
	You can not jupe your services server or your uplink server.
	
# RAW responses
OPER_RAW_SYNTAX
	RAW texto

# UPDATE responses
OPER_UPDATING
	Updateando las bases de datos.
	
# RELOAD responses
OPER_RELOAD
	El archivo de configuracion de Servicios ha sido cargado nuevamente.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN no definido; imposible partir nuevamente. Corre nuevamente el
	script \2configure\2  y recompila Services para incluir el comando RESTART.

# IGNORE responses
OPER_IGNORE_SYNTAX
	IGNORE {ADD|DEL|LIST|CLEAR} [tiempo] [nick | mask]
OPER_IGNORE_VALID_TIME
	Debes ingresar un numero valido como tiempo.
OPER_IGNORE_TIME_DONE
	%s sera ignorado por %s.
OPER_IGNORE_PERM_DONE
	%s sera ignorado permanentemente.
OPER_IGNORE_DEL_DONE
	%s ya no sera ignorado.
OPER_IGNORE_LIST
	Lista ignore de Servicios:
OPER_IGNORE_LIST_NOMATCH
	%s no encontrado en la lista ignore.
OPER_IGNORE_LIST_EMPTY
	La lista ignore esta vacia.

# KILLCLONES responses
OPER_IGNORE_LIST_CLEARED
	La lista ignore ha sido vaciada.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	Imposible encontrar al usuario %s.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Lista de canal:
	Nombre               Modos de Usr. Topico
OPER_CHANLIST_HEADER_USER
	Lista del canal %s:
	Nombre               Modos de Usr. Topico
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	Fin de la lista de canales.
	
# USERLIST responses
OPER_USERLIST_HEADER
	Lista de usuarios:
	Nick                 Mascara
OPER_USERLIST_HEADER_CHAN
	Lista de usuarios de %s:
	Nick                 Mascara
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Fin de la lista de usuarios.

# CACHE responses
OPER_SUPER_ADMIN_ON
	Ahora eres un SuperAdmin.
OPER_SUPER_ADMIN_OFF
	Ya no eres un SuperAdmin.
OPER_SUPER_ADMIN_SYNTAX
	Opciones para SuperAdmin deben ser ON u OFF.
OPER_SUPER_ADMIN_WALL_ON
	%s es ahora un Super-Admin.
OPER_SUPER_ADMIN_WALL_OFF
	%s ya no es un Super-Admin.

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_SUPER_ADMIN_ONLY
	Solo Super-Admins pueden usar este comando.

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
	DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	Los servicios estan deshabilitados temporalmente, por favor intenta mas tarde 
OPER_DEFCON_NO_CONF
	El sistema Defcon debe ser habilitado en el archivo services.conf
OPER_DEFCON_CHANGED
	Servicios estan ahora en DEFCON %d
OPER_DEFCON_WALL
	%s Cambiado el nivel de DEFCON a %d
DEFCON_GLOBAL
	El Nivel de Defcon esta ahora a Nivel: %d

# Module strings

OPER_MODULE_LOADED
	Modulo %s cargado 
OPER_MODULE_UNLOADED
	Modulo %s descargado
OPER_MODULE_LOAD_FAIL
	Imposible cargar modulo %s
OPER_MODULE_REMOVE_FAIL
	Imposible quitar modulo %s
OPER_MODULE_NO_UNLOAD
	This module can not be unloaded.
OPER_MODULE_LOAD_SYNTAX
	MODLOAD NombreArchivo
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD NombreArchivo
OPER_MODULE_LIST_HEADER
	Lista actual de modulos:
OPER_MODULE_LIST
	Modulo: %s [%s] [%s]
OPER_MODULE_LIST_FOOTER
	%d Modulos cargados.
OPER_MODULE_INFO_LIST
	Modulo: %s Version: %s  Autor: %s cargado: %s
OPER_MODULE_CMD_LIST
	Comandos entregados: %R%s %s
OPER_MODULE_MSG_LIST
	Entrega control al IRCD para: %s
OPER_MODULE_NO_LIST
	Actualmente no hay modulos cargados
OPER_MODULE_NO_INFO
	No hay informacion disponible para el modulo %s
OPER_MODULE_INFO_SYNTAX
	MODINFO NombreArchivo

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
MODULE_HELP_HEADER
	Los siguientes comandos han sido cargados por un modulo:

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [parametros]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+expiracion] mascara limite razon
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {mascara | lista}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE numero posicion
OPER_EXCEPTION_DISABLED
	Limites de sesion no disponibles.
OPER_EXCEPTION_ALREADY_PRESENT
	La mascara %s ya esta presente en la lista de excepciones.
OPER_EXCEPTION_TOO_MANY
	La lista de excepciones al limite de sesiones esta llena!
OPER_EXCEPTION_ADDED
	Limite de sesion para %s seteado a %d.
OPER_EXCEPTION_MOVED
	Excepcion para %s (#%d) movido a la posicion %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	La instancia no existe (#%d)  en la lista de excepciones al limite de sesiones.
OPER_EXCEPTION_NOT_FOUND
	%s no encontrado en la lista de excepciones al limite de sesiones.
OPER_EXCEPTION_NO_MATCH
	No existen instancias similares en la lista de excepciones al limite de sesiones.
OPER_EXCEPTION_DELETED
	%s instancias borradas de la lista de excepciones al limite de sesiones.
OPER_EXCEPTION_DELETED_ONE
	1 instancia borrada de la lista de excepciones al limite de sesiones.
OPER_EXCEPTION_DELETED_SEVERAL
	%d instancias borradas de la lista de excepciones al limite de sesiones.
OPER_EXCEPTION_LIST_HEADER
	Lista de excepciones al limite de sesiones actual:
# mask, limit
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Num  Limite Host
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (by %s on %s; %s)
	   Limite: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Limite de sesion invalido. Debe ser un entero valido mayor o igual a cero y menor a %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Mascara invalida. Solo mascaras reales son validas ya que excepciones no son comparadas a nicks o usernames.
OPER_EXCEPTION_EXISTS
	%s already exists on the EXCEPTION list.
OPER_EXCEPTION_CHANGED
	Exception for %s has been updated to %d.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST limite | VIEW host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST limite
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW host
OPER_SESSION_DISABLED
	El limite de sesion esta inhabilitado.
OPER_SESSION_INVALID_THRESHOLD
	Valor de umbral invalido. debe ser un entero valido mayor a 1.
OPER_SESSION_NOT_FOUND
	%s no encontrado en la lista de sesion.
OPER_SESSION_LIST_HEADER
	Hosts con al menos %d sesiones:
OPER_SESSION_LIST_COLHEAD
	Sesiones  Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	El host %s actualmente tiene %d sesiones con un limite de %d.

# EXCEPTION help

OPER_HELP_EXCEPTION
	Sintaxis: EXCEPTION ADD [+expiracion] mascara limite razon
	          EXCEPTION DEL {mascara | lista}
	          EXCEPTION MOVE numero posicion
	          EXCEPTION LIST [mascara | lista]
	          EXCEPTION VIEW [mascara | lista]
	
	Permite a los administradores de Servicios manipular la lista
	de hosts que tienen un limite de sesion especifico - permitiendo 
	a ciertas maquinas, como servidores de shells, mantener mayor numero 
	de clientes al mismo tiempo. En cuanto un host alcance su limite
	de sesiones, todos los clientes intentando conectarse desde ese 
	host seran killeados. Antes de killear usuarios, estos son
	notificados a traves de un /NOTICE del %S con ayuda acerca
	de el limite de sesion. El contenido de este notice es un seteo
	de configuracion.
	
	EXCEPTION ADD añade la mascara de host dada a la lista de
	excepcion. Ten en cuenta que las mascaras nick!usuario@host
	y usuario@host son invalidas. Solo mascaras reales como
	box.host.dom and *.host.dom, son permitidas ya que el
	limite de sesiones no toma en cuenta nicks o nombres de usuario.
	limite debe ser un numero mayor o igual a cero. Esto determina
	cuantas sesiones el host puede tener a un tiempo dado. El valor
	cero significa que el host no tiene limites de sesion. Ver la ayuda
	acerca de AKILL para detalles acerca del parametro opcional
	expiracion.
	EXCEPTION DEL remueve la mascara dada de la lista de excepcion.
	EXCEPTION MOVE mueve la excepcion numero a posicion. Las
	excepciones entre medio seran movidas arriba o abajo para llenar el
	espacio.
	EXCEPTION LIST y EXCEPTION VIEW muestran todas las
	excepciones actuales; si la mascara opcional es dada, la lista es
	limitada a las excepciones similares a la mascara. La diferencia
	es que EXCEPTION VIEW es mas verbal, mostrando el nombre de la
	persona que agrego la excepcion, el limite de excepcion, razon,
	mascara de hosts y la fecha y hora de expiracion. 
	
	Toma en cuenta que un cliente conectandose "usara" la primera
	excepcion a la que el host se asemeje. Largas listas de excepcion
	y mascaras que se asemejen a muchas mascaras posiblemente degraden
	el rendimiento de los Servicios.
	
	Limitado a administradores de Servicios.

OPER_HELP_SESSION
	Sintaxis: SESSION LIST umbral
	          SESSION VIEW host
	
	Permite a los administradores de Servicios ver la lista de sesiones.
	
	SESSION LIST lista hosts con al menos umbral sesiones.
	El umbral debe ser un numero mayor a 1. Esto es para prevenir
	listados accidentales del gran numero de hosts de sesion.
	SESSION VIEW muestra informacion detallada acerca de un host 
	especifico - incluyendo la cuenta de sesion y el limite de sesion
	actual.
	El valor host no puede incluir comodines.
	
	Mira la ayuda sobre EXCEPTION para mayor informacion acerca de
	limites de sesion y como setearlos para hosts y grupos especificos
	de ahi en adelante. 
	
	Limitado a administradores de Servicios.

###########################################################################
#
# News system messages
#
###########################################################################

OPER_HELP_STAFF
	Sintaxis: STAFF

	Muestra todos los nicks de Services Staff, junto con su nivel
	y estado on-line.

OPER_HELP_DEFCON
	Sintaxis: DEFCON [1|2|3|4|5]

	El sistema defcon puede ser usado para implementar un conjunto
	pre-definido de restricciones que son muy utiles en un eventual
	ataque a la red.

OPER_HELP_DEFCON_NO_NEW_CHANNELS
	* No nuevos registros de canales
OPER_HELP_DEFCON_NO_NEW_NICKS
	* No nuevos registros de nicks
OPER_HELP_DEFCON_NO_MLOCK_CHANGE
	* No cambios en MLOCK
OPER_HELP_DEFCON_FORCE_CHAN_MODES
	* Fuerza Modos de Canal (%s) para ser seteado en todos los canales
OPER_HELP_DEFCON_REDUCE_SESSION
	* Usa el reducido limite de sesion de %d
OPER_HELP_DEFCON_NO_NEW_CLIENTS
	* Kill a cualquier NUEVO cliente que se conecte
OPER_HELP_DEFCON_OPER_ONLY
	* Ignora a cualquier no-operador notificando
OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* Ignora a los no-operadores sin notificar
OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* AKILL cualquier nuevo cliente que se conecte


###########################################################################
#
# News system messages
#
###########################################################################

OPER_HELP_DEFCON_NO_NEW_MEMOS
	* No hay nuevos memos enviados
OPER_HELP_CHANKILL
	Sintaxis: CHANKILL [+expira] canal razon

	Pone un AKILL para cada nick en el canal especificado. Este
	usa el entero, completo y real ident@host para cada nick,
	entonces ejecuta el AKILL. 

###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[Noticias de Logon - %s] %s
NEWS_OPER_TEXT
	[Noticias de Oper - %s] %s
NEWS_RANDOM_TEXT
	[Noticias al Azar - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD|DEL|LIST} [texto|numero]
NEWS_LOGON_LIST_HEADER
	Noticias de entrada:
NEWS_LOGON_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_LOGON_LIST_NONE
	No hay Noticias de entrada.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD texto
NEWS_LOGON_ADD_FULL
	La lista de noticias esta llena!
NEWS_LOGON_ADDED
	Nueva noticia de entrada anadida (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {numero | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Noticia de entrada #%d no encontrada!
NEWS_LOGON_DELETED
	Noticia de entrada #%d borrada.
NEWS_LOGON_DEL_NONE
	No hay noticias de entrada para borrar!
NEWS_LOGON_DELETED_ALL
	Todas las noticias de entrada borradas.

NEWS_OPER_SYNTAX
	Sintaxis: OPERNEWS {ADD|DEL|LIST} [texto|numero]
NEWS_OPER_LIST_HEADER
	Noticias de oper:
NEWS_OPER_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_OPER_LIST_NONE
	No hay noticias de oper.
NEWS_OPER_ADD_SYNTAX
	Sintaxis: OPERNEWS ADD texto
NEWS_OPER_ADD_FULL
	La lista de noticias esta llena!
NEWS_OPER_ADDED
	Nueva noticia de oper anadida (#%d).
NEWS_OPER_DEL_SYNTAX
	Sintaxis: OPERNEWS DEL {numero | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Noticia de oper #%d no encontrada!
NEWS_OPER_DELETED
	Noticia de oper #%d borrada.
NEWS_OPER_DEL_NONE
	No hay noticias de oper para borrar!
NEWS_OPER_DELETED_ALL
	Todas las noticias de oper borradas.

NEWS_RANDOM_SYNTAX
	RANDOMNEWS {ADD|DEL|LIST} [texto|numero]
NEWS_RANDOM_LIST_HEADER
	Noticias al Azar:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s por %s)
	    %s
NEWS_RANDOM_LIST_NONE
	No hay Noticias al azar.
NEWS_RANDOM_ADD_SYNTAX
	RANDOMNEWS ADD texto
NEWS_RANDOM_ADD_FULL
	La lista de noticias esta llena!
NEWS_RANDOM_ADDED
	Nueva noticia al azar anadida (#%d).
NEWS_RANDOM_DEL_SYNTAX
	RANDOMNEWS DEL {numero | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Noticia al azar #%d no encontrada!
NEWS_RANDOM_DELETED
	Noticia al azar #%d borrada.
NEWS_RANDOM_DEL_NONE
	No hay noticias al azar para borrar!
NEWS_RANDOM_DELETED_ALL
	Todas las noticias al azar borradas.

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
	Sintaxis: RANDOMNEWS ADD texto
	          RANDOMNEWS DEL {numero | ALL}
	          RANDOMNEWS LIST
	
	Edita o muestra la lista de mensajes de noticia al azar.
	Cuando un usuario se conecta a la red, uno (y solo uno) de los
	mensajes al azar sera elejido y enviado a ellos.
	
	RANDOMNEWS may only be used by Services admins.

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S es un servicio designado a dar informacion sobre
	Servicios. Topicos de ayuda estan disponibles via los
	comandos HELP de los otros clientes de Servicios:

	%R%s HELP
	     para informacion acerca registrar nicknames

	%R%s HELP
	     para informacion acerca registrar y controlar
	     canales

	%R%s HELP
	     para informacion acerca enviar mensajes a usuarios no
	     en linea.

HELP_HELP_BOT

	%R%s HELP
	     para informacion acerca setear un bot en tu canal

HELP_HELP_HOST

	%R%s HELP
	     Para informacion sobre como setear vHosts para nicknames.

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
	    REGISTER   Registra un nickname
NICK_HELP_CMD_GROUP
	    GROUP      Ingresa a un grupo
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY   Para identificarte con tu clave
NICK_HELP_CMD_ACCESS
	    ACCESS     Modifica la lista de direcciones autorizadas
NICK_HELP_CMD_SET
	    SET        Setea protecciones, incluyendo kills
NICK_HELP_CMD_SASET
	    SASET      Set SET-options on another nickname
NICK_HELP_CMD_DROP
	    DROP       Cancela la registracion de un nickname
NICK_HELP_CMD_RECOVER
	    RECOVER    Killea otro usuario que este usando tu nickname
NICK_HELP_CMD_RELEASE
	    RELEASE    Retoma custodia de tu nick luego de RECOVER
NICK_HELP_CMD_SENDPASS
	    SENDPASS   Olvidaste tu clave? Intenta esto
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
	    GETPASS    Recupera la clave para un nickname
	               (solo si encripcion esta desabilitada)
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Matches and returns all users that registered using given email
NICK_HELP_CMD_FORBID
	    FORBID     Previene un nickname a ser usado
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Suspend a given nick
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Unsuspend a given nick

NICK_HELP
	%S te permite "registrar" un nickname y
	prevenir a otros usarlo. Los siguientes comandos
	permiten el registro y mantenimiento de nicknames;
	para usarlos, tipea %R%S comando.
	Para mayor informacion acerca de un comando especifico
	tipea %R%S HELP comando.
	
NICK_HELP_FOOTER
	NOTESE: Este servicio intenta proveer una forma para
	usuarios de IRC de asegurarse que su identidad no esta
	comprometida, NO para facilitar el "robo" de nicknames
	u otra accion maliciosa. El Abuso de %S resultara en, a lo
	minimo, la perdida del nickname comprometido.

NICK_HELP_EXPIRES
	Nicknames en desuso estan sujetos a la expiracion
	automatica, en otras palabras, seran borrados despues
	de %d dias.

NICK_HELP_REGISTER
	Sintaxis: REGISTER clave [email]
	
	Registra tu nickname en la base de datos de %S. Una
	vez registrado, puedes usar los comandos SET y ACCESS
	para configurar los seteos de tu nick a gusto.
	Asegurate de recordar la clave que usaste para
	registrarte - lo necesitaras para hacer cambios a tu nick
	(Tomar en cuenta que mayusculas importan!  ANOPE,
	Anope, y anope son todas claves diferentes!)
	
	Guias para elegir claves:
	
	Las claves no deben ser faciles de adivinar. Por ejemplo,
	usar tu nombre real como clave es una mala idea. Usar
	tu nickname como clave es una idea mucho peor ;) y,
	de hecho, %S no lo permitira. Ademas, claves cortas
	son vulnerables a adivinanzas, asi que deberias elejir una
	clave de al menos 5 caracteres.
	Por ultimo, el caracter de espacio no puede ser usado.
	
	The parametro email es opcional y seteara el email para
	tu nick inmediatamente. Como sea, puede que sea requerido
	en ciertas redes.
	Tu privacidad es respetada; este e-mail no sera dado a
	terceras personas.
	
	Este comando ademas crea un nuevo grupo para tu nickname
	que te permitira registrar otros nicks despues compartiendo
	la misma configuracion, memos y privilegios. Para mayor
	informacion acerca de esta caracteristica, tipea %R%S
	HELP GROUP.
	
NICK_HELP_GROUP
	Sintaxis: GROUP grupo clave
	
	Este comando hace a tu nickname unirse al grupo de nicknames
	grupo. clave es la de tu nickname.
	
	La union a un grupo te permitira compartir configuraciones,
	memos, y privilegios en canales con todos los nicknames en un
	grupo, y mucho mas!
	 
	Un grupo existe siempre y cuando sea servible. Esto
	significa que si un nick del grupo es botado, no perderas
	los rasgos compartidos descritos anteriormente, siempre y
	cuando al menos un nick permanezca en el grupo.
	
	Puedes usar este comando inclusive si aun no registras tu
	nick. Si tu nick ya esta registrado deberas identificarte
	antes de usar este comando. Tipea %R%S HELP
	IDENTIFY para mayor informacion. Esto ultimo quizas no sea
	posible en tu red.
	
	Es recomendable usar este comando con un nick no registrado ya
	que sera registrado automaticamente cuando uses el comando.
	Puedes usarlo con un nick ya registrado (para cambiar tu grupo) 
	solo si los administradores de tu red lo permiten.
	
	Puedes estar en un solo grupo a la vez. La union de
	grupos no es posible.
	
	Ten en cuenta: todos los nicks en un grupo comparten la
	misma clave.

NICK_HELP_IDENTIFY
	Sintaxis: IDENTIFY clave
	
	Informa a %S que tu eres realmente el dueño de tu nick.
	Muchos comandos requieren que te autentifiques con este
	comando antes de usarlos. La clave deberia ser la misma
	que enviaste con el comando REGISTER.
	
NICK_HELP_UPDATE
	Sintaxis: UPDATE

	Actualiza tu estado actual, ej. revisa los memos nuevos,
	setea, si es necesario, los modos de canales (ModeonID)
	y actualiza tu vhost y tus userflags (lastseentime, etc).
	
NICK_HELP_LOGOUT
	Sintaxis: LOGOUT
	
	Este comando invierte el efecto de IDENTIFY, osea, 
	te hace ya no reconocible como el dueño de tu nick.
	Ten en cuenta que no te sera requerido identificarte
	nuevamente.

NICK_HELP_DROP
	Sintaxis: DROP [nickname]
	
	Bota tu nickname de la base de datos de %S. Un nick
	que ha sido botado esta libre para que cualquier persona
	lo registre.
	
	Puedes botar un nick en tu grupo pasandolo como el
	parametro nick.
	
	Antes de usar este comando debes identificarte con tu
	clave (%R%S HELP IDENTIFY para mayor informacion).

NICK_HELP_ACCESS
	Sintaxis: ACCESS ADD mascara
	          ACCESS DEL mascara
	          ACCESS LIST
	
	Modifica o muestra la lista de acceso para tu nick. Esta
	es la lista de direcciones que seran automaticamente
	reconocidas por %S como autorizadas a usar el nick. Si
	quieres usar el nick desde una direccion diferente,
	debes enviar un comando IDENTIFY para hacer que %S te
	reconozca.
	
	Ejemplos:
	
	    ACCESS ADD anyone@*.bepeg.com
	        Permite acceso al usuario anyone desde
	        cualquier maquina en el dominio bepeg.com.
	
	    ACCESS DEL anyone@*.bepeg.com
	        Revierte el comando anterior.
	
	    ACCESS LIST
	        Muestra la lista de acceso actual.

NICK_HELP_SET
	Sintaxis: SET opcion parametros
	
	Setea varias opciones para nicknames. opcion puede ser
	una de:
	
	    DISPLAY    Setea el display de tu grupo con Servicios
	    PASSWORD   Setea la clave de tu nickname
	    LANGUAGE   Setea el lenguaje con el que Servicios te
	                   enviara mensajes
	    URL        Asocia una URL con tu nickname
	    EMAIL      Asocia un E-mail con tu nickname
	    ICQ        Asocia un numero ICQ con tu nickname
	    GREET      Asocia un saludo con tu nickname
	    KILL       Enciende o apaga la proteccion
	    SECURE     Enciende o apaga la seguridad de nickname
	    PRIVATE    Previene tu nickname de aparecer en un
	                   %R%S LIST
	    HIDE       Esconde algunas partes de tu informacion
	    MSG        Cambia el metodo de comunicacion con
	                    Servicios
	    AUTOOP     Should services op you automatically.    
	
	Para usar este comando, debes primero identificarte con
	Servicios usando tu clave (%R%S HELP IDENTIFY
	para mayor informacion).
	
	Tipea %R%S HELP SET opcion para mayor
	informacion acerca de una opcion especifica.

NICK_HELP_SET_DISPLAY
	Sintaxis: SET DISPLAY nuevo-display
	
	Cambia el display usado para referirte a tu grupo de
	nicks en Servicios. El nuevo display DEBE ser un nick
	de tu grupo.

NICK_HELP_SET_PASSWORD
	Sintaxis: SET PASSWORD nueva-clave
	
	Cambia la clave usada para identificarte como el
	dueño del nick.

NICK_HELP_SET_LANGUAGE
	Sintaxis: SET LANGUAGE numero
	
	Cambia el lenguaje que Servicios usa para enviarte mensajes
	(por ejemplo, al responder a un comando que enviaste).
	numero debe ser elejido de la lista de lenguajes
	soportados:
	

NICK_HELP_SET_URL
	Sintaxis: SET URL url
	
	Asocia la URL dada con tu nickname. Esta URL
	sera mostrada cuando alguien requiera informacion
	sobre tu nick con el comando INFO.

NICK_HELP_SET_EMAIL
	Sintaxis: SET EMAIL direccion
	
	Asocia el E-mail dado con tu nickname. La direccion
	sera mostrada cuando alguien requiera informacion 
	sobre el canal con el comando INFO.
	
NICK_HELP_SET_ICQ
	Sintaxis: SET ICQ numero
	
	Asocia el numero ICQ dado con tu nickname. Este
	numero sera mostrado cuando alguien solicite
	informacion acerca de tu nick con el comando INFO.
	
NICK_HELP_SET_GREET
	Sintaxis: SET GREET mensaje
	
	Hace al mensaje ingresado el saludo para tu nickname, el
	que sera mostrado cuando ingreses a un canal con la
	opcion GREET abilitada, siempre y cuando tengas en el
	el acceso necesario.

NICK_HELP_SET_KILL
	Sintaxis: SET KILL {ON | QUICK | IMMED | OFF}
	
	Cambia la proteccion automatica de tu nick a on u off.
	Con proteccion on, si otro usuario intenta usar tu nick,
	tendra un minuto para cambiar de nick, luego del cual
	%S cambiara su nick automaticamente.
	
	Si seteas QUICK, el usuario sera dado solo de 20 segundos
	para cambiar el nick en vez de los normales 60. Si seleccionas
	IMMED, el nick del usuario sera cambiado automaticamente
	sin ser advertido o dado oportunidad de cambiarlo antes;
	Por favor no uses esta opcion a menos que lo consideres
	necesario. Ademas, los administradores de la red pueden haber
	deshabilitado esta opcion.

NICK_HELP_SET_SECURE
	Sintaxis: SET SECURE {ON | OFF}
	
	Cambia las caracteristicas de seguridad de %S para tu nick en on
	u off. Con SECURE seteado, debes ingresar tu clave para ser
	reconocido como propietario del nick, a pesar de que tu direccion
	este en la lista de accesos. Sin embargo, si estas en la lista de
	acceso, %S no te auto-killeara no importando el seteo de la opcion
	KILL.

NICK_HELP_SET_PRIVATE
	Sintaxis: SET PRIVATE {ON | OFF}
	
	Hace que %S active o desactive la opcion de privacidad de tu
	nick. Con PRIVATE seteado,tu nickname no aparecera en las
	listas de nicknames generadas con el comando LIST de %S.
	(Sin embargo, cualquier persona que sepa tu nick podra chequear
	tu informacion usando el comando INFO.)

NICK_HELP_SET_HIDE
	Sintaxis: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Permite prevenir que cierta informacion sobre tu nick cuando
	es solicitada mediante el comando %S INFO a tu nick.
	Puedes esconder tu direccion E-mail (EMAIL), la ultima
	mascara usuario@host (USERMASK), y tu ultimo mensaje
	quit (QUIT).
	El segundo parametro especifica si la informacion debe ser
	escondida (OFF) o mostrada (ON).
	
NICK_HELP_SET_MSG
	Sintaxis: SET MSG {ON | OFF}
	
	Te permite elegir la forna en la que los Servicios se
	comunicaran contigo. Con MSG seteado, los Servicios
	usaran msgs, de otra forma usaran notices. 

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
	Sintaxis: GHOST nickname [clave]
	
	Termina una sesion IRC "fantasma" usando tu nick. Una
	sesion fantasma es una conexion falsa, pero la cual los
	servidores IRC creen esta en linea por una razon u otra.
	Tipicamente, esto ocurre si tu computador se bloquea o tu
	conexion a internet o por modem se cortan mientras estas
	en IRC.
	
	Para usar el comando GHOST para un nick, tu
	direccion actual mostrada en /WHOIS debe estar en la lista
	de acceso de ese nick, debes estar identificado y en el
	grupo de ese nick, o debes proveer la clave correcta
	para ese nickname.

NICK_HELP_INFO
	Sintaxis: INFO nickname [ALL]
	
	Muestra informacion relativa al nick que especifiques,
	tal como el propietario del nick, la ultima direccion usada
	y tiempo, y opciones del nick. Si estas identificado con el
	nick por el cual pides informacion y usas el parametro
	ALL, podras ver toda la informacion; independiente de
	si esta oculta o no.

NICK_HELP_LIST
	Sintaxis: LIST patron
	
	Lista todos los nicknames que concuerden con el patron
	dado, en el formato nick!usuario@host. Nicks con la
	opcion PRIVATE seteada no seran mostrados.
	
	Ejemplos:
	
	    LIST *!joeuser@foo.com
	        Lista todos los nicks registrados por joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Lista todos los nicks registrados con Bot en sus
	        nomvres (sensible a mayusculas y minisculas).
	
	    LIST *!*@*.bar.org
	        Lista todos los nicks registrados por usuarios en el
	        dominio bar.org.

NICK_HELP_ALIST
	Sintaxis: ALIST [nivel]
	
	Lista todos los canales en los que tengas acceso. Opcionalmente, puedes especificar
	un nivel en formato XOP o ACCESS. Los resultados incluiran solamente
	canales en los que tengas el nivel de acceso dado.

	Ejemplos:
	    ALIST Founder
	        Lista los canales en los que tienes acceso de
	    fundador.

	    ALIST AOP
	        Lista los canales en los que tienes acceso de
	    AOP o mayor.

	    ALIST 10
	        Lista los canales en los que tengas nivel de acceso 10
	    o mayor.
	
	Canales que tengan la opcion NOEXPIRE seteada seran precedidos
	por un signo de exclamacion.
	
NICK_HELP_GLIST
	Sintaxis: GLIST
	
	Lista todos los nicks en tu grupo.

NICK_HELP_STATUS
	Sintaxis: STATUS nickname...
	
	Devuelve informacion acerca de si el usuario usando el nick dado
	ha sido reconocido como el propietario del nickname.
	La respuesta tiene el siguiente formato:
	
	    nickname codigo-del-status
	
	Donde nickname es el nickname enviado con el comando, y
	codigo-del-status es uno de los siguientes:
	
	    0 - Usuario no esta en linea o el nick no esta registrado.
	    1 - Usuario no reconocido como el propietario del nick.
	    2 - Usuario reconocido solo mediante la lista de acceso.
	    3 - Usuario reconocido mediante autentificacion por clave.
	
	hasta 16 nicknames pueden ser enviados con cada comando. El
	resto seran ingnorados. Si no se entrega un nickname, se mostrara
	tu estado.

NICK_HELP_SENDPASS
	Sintaxis: SENDPASS nickname
	
	Envia la clave del nickname dado a la direccion
	e-mail seteada en el record del nickname. Este comando
	es sumamente util para lidiar con claves perdidas.
	
	Puede ser limitado a operadores de IRC en ciertas
	redes.
	
	Este comando no esta disponible cuando encripcion esta habilitada.

NICK_HELP_CONFIRM
	Sintaxis: CONFIRM passcode
	
	Este es el segundo paso en el proceso de registro de nicknames.
	Debes ejecutar este comando para que tu nick sea registrado con %S.
	El passcode (tambien conocido como codigo de autorizacion) es
	enviado a tu direccion e-mail en el primer paso del proceso de 
	registo. Para mas informacion sobre la primera etpa del proceso 
	de registro, escribe: %R%S HELP REGISTER

NICK_HELP_RESEND
	Sintaxis: RESEND
	
	Este comando envia el codigo de autorizacion (conocido como passcode)
	a la direccion e-mail del usuario que lo esta ejecutando.

NICK_SERVADMIN_HELP
	Administradores de Servicios pueden tambien botar cualquier
	nickname sin tener que identificarse por el nick y pueden
	ver la lista de acceso para cualquier nickname
	(%R%S ACCESS LIST nick).

NICK_SERVADMIN_HELP_LOGOUT
	Sintaxis: LOGOUT [nickname [REVALIDATE]]
	
	Sin un parametro, revierte el efecto del comando IDENTIFY, 
	osea te hace ya no reconocido como el verdadero dueño del nick
	Ten en cuenta, que no seras requerido a reidentificarte.
	
	Con un parametro, hace los mismo para el nick dado. Si
	ademas especificas REVALIDATE, los Servicios requeriran al
	dado nick a reidentificarse. Este uso esta limitado a
	administradores de Servicios.

NICK_SERVADMIN_HELP_DROP
	Sintaxis: DROP [nickname]
	
	Sin un parametro, bota tu nickname de la base de datos de
	%S.
	
	Con un parametro, bota el nick nombrado de la base des
	 datos. Puedes botar cualquier nick de tu grupo sin
	privilegios especiales. Botar cualquier nick esta
	limitado a administradores de Servicios.

NICK_SERVADMIN_HELP_INFO
	
	Administradores de Servicios pueden usar el parametro
	ALL con cualquier nick.

NICK_SERVADMIN_HELP_LIST
	Sintaxis: LIST patron [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	Lista todos los nicknames registrados que concuerden con el
	patron dado, en el formato nick!usuario@host. Nicks con
	la opcion PRIVATE solo seran mostrados a administradores
	de Servicios. Nicks con la opcion NOEXPIRE seteada tendran
	un ! antepuestos al nickname para administradores de
	Servicios.
	
	Si las opciones FORBIDDEN, SUSPENDED, NOEXPIRE o UNCONFIRMED son dadas,
	solo nicks que, respectivamete, esten FORBIDDEN, SUSPENDED, UNCONFIRMED
	o esten seteados NOEXPIRE seran mostrados. Si las tres opciones
	son dadas, los tres tipos de nicks seran mostrados.
	Estas opciones estan limitadas a administradores de Servicios.
	
	Ejemplos:
	
	    LIST *!joeuser@foo.com
	        Lista todos los nicks registrados por joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Lista todos los nicks registrados con Bot en
	        sus nombres. (no sensible a mayusculas y minusculas).
	
	    LIST * NOEXPIRE
	        Lista todos los nicks registrados que hayan sido seteados
	        a no expiracion.

NICK_SERVADMIN_HELP_ALIST
	Sintaxis: ALIST [nick] [nivel]
	
	Sin parametros, lista canales en los que tengas acceso. Con
	un parametro, lista canales en los que nick tenga acceso. 
	Con dos parametros, lista canales en los que nick tenga 
	acceso igual o mayor a level.

	Su uso esta limitado a administradores de Servicios.

NICK_SERVADMIN_HELP_GLIST
	Sintaxis: GLIST [nickname]
	
	Sin un parametro, lista todos los nicknames en tu
	grupo.
	
	Con un parametro, lista todos los nicknames que hay
	en el grupo del nick dado.
	Su uso esta limitado a administradores de Servicios.

NICK_SERVADMIN_HELP_GETPASS
	Sintaxis: GETPASS nickname
	
	Devuelve la clave para el nickname dado. Ten en cuenta
	que cuando este comando es usado, un mensaje incluyendo
	la persona que ejecuto el comando y el nickname en el cual
	fue usado sera monitoreado y enviado como WALLOPS/GLOBOPS.
	
	Limitado a administradores de Servicios.
	
	Este comando no esta disponible si encripcion esta habilitada.

NICK_SERVADMIN_HELP_GETEMAIL
	Sintaxis: GETEMAIL user@emailhost

	Muestra los nicks que usen el email dado. Notar que no puedes
	usar comodines (wildcards) para ningun usuario o emailhost. Donde
	sea que el comando se use, un mensaje incluyendo la persona que uso
	el comando y el email sera logeado.

	Limitado a Services admins.

NICK_SERVADMIN_HELP_FORBID
	Sintaxis: FORBID nickname [razon]
	
	Impide a un nickname de ser registrado o usado por alguien.
	Puede ser cancelado botando el nick.
	
	En algunas redes, la razon es requerida.
	
	Limitado a administradores de Servicios.

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
	    GETPASS    Retorna la clave de fundador para un canal
CHAN_HELP_CMD_FORBID
	    FORBID     Previene un canal a ser usado
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    Previene un canal a ser usado preservando 
	               toda su informacion.
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Libera la suspencion del canal.
CHAN_HELP_CMD_STATUS
	    STATUS     Retorna el nivel de acceso actual de un
	               usuario en un canal
CHAN_HELP_CMD_REGISTER
	    REGISTER   Registra un canal
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   Te identifica con tu clave
CHAN_HELP_CMD_SET
	    SET        Setea opciones e informacion de un canal
CHAN_HELP_CMD_AOP
	    AOP        Modifica la lista de usuarios AOP
CHAN_HELP_CMD_SOP
	    SOP        Modifica la lista de usuarios SOP
CHAN_HELP_CMD_ACCESS
	    ACCESS     Modifica la lista de usuarios privilegiados
CHAN_HELP_CMD_LEVELS
	    LEVELS     Redefine el significado de niveles de acceso
CHAN_HELP_CMD_AKICK
	    AKICK      Mantiene la lista de AutoKick
CHAN_HELP_CMD_DROP
	    DROP       Cancela la registracion de un canal
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Ayuda a recuperar claves perdidas
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
	%S te permite registrar y controlar varios
	aspectos de los canales. A menudo, %S puede prevenir
	a usuarios maliciosos "tomarse" canales limitando
	quienes estan permitidos a tener privilegios de operador.
	Los comandos disponibles estan listados a continuacion;
	para usarlos, tipea %R%S comando.
	Para mayor informacion acerca de un comando especifico,
	tipea %R%S HELP comando.
	 
	
CHAN_HELP_EXPIRES
	
	Ten en cuenta que cualquier canal que no es usado por
	%d dias (al cual ningun usuario en la lista de acceso
	del canal entre en ese periodo de tiempo) sera
	automaticamente botado.

CHAN_HELP_REGISTER
	Sintaxis: REGISTER canal clave descripcion
	
	Registra un canal en la base de datos de %S. Para
	usar este comando, debes primero ser un operador en el
	canal que intentas registrar. La clave es usada con
	el comando IDENTIFY para permitir a otros hacer
	cambios al seteo del canal posteriormente.
	El ultimo parametro, el cual debe ser incluido, es
	una descripcion general del proposito del canal.
	
	Cuando registras un canal, eres reconocido como el
	"fundador" del canal. El fundador del canal puede
	cambiar todos los seteos para el canal;
	%S tambien dara al fundador prvilegios de operador
	automaticamente cuando el usuario entre al canal.
	Averigua sobre el comando ACCESS
	(%R%S HELP ACCESS) para informacion sobre
	como dar estos privilegios a otros usuarios del canal.
	
	NOTESE: Para registrar un canal debes haber registrado
	tu nickname primero. Si aun no lo haz hecho, tipea
	%R%s HELP para mayor informacion en como hacerlo.

CHAN_HELP_IDENTIFY
	Sintaxis: IDENTIFY canal clave
	
	Te identifica con %S como el fundador del canal dado.
	Muchos comandos requieren que uses este comando antes
	de usarlos. La clave deberia ser la misma que
	enviaste con el comando REGISTER.
	
CHAN_HELP_LOGOUT
	Sintaxis: LOGOUT canal nickname
	
	Este comando hace al nickname dado ya no identificado
	en el canal dado.
	
	Si eres el fundador del canal, puedes desconectar a
	quien sea, de otra forma solo te puedes desconectar
	a ti mismo.

CHAN_HELP_DROP
	Sintaxis: DROP canal
	
	Desregistra el canal dado. Solo puede ser usado por el
	fundador del canal, quien debe primero usar el comando
	 IDENTIFY.

CHAN_HELP_SET
	Sintaxis: SET canal opcion parametros
	
	Le permite al fundador del canal setear varias opciones
	de canal y otra informacion.
	
	Opciones disponibles:
	
	    FOUNDER       Setea el fundador de un canal
	    SUCCESSOR     Setea el sucesor de un canal
	    PASSWORD      Setea el pasword del fundador
	    DESC          Setea la descripcion del canal
	    URL           Asocia una URL con el canal
	    EMAIL         Asocia un E-mail con el canal
	    ENTRYMSG      Setea un mensaje a enviar a
	                  los usuarios cuando entren al canal
	    BANTYPE       Setea como los Servicios ponen bans
	                  en el canal
	    MLOCK         Fija los modos de canal on u off
	    KEEPTOPIC     Retiene el topic cuando el canal no
	                  esta en uso
	    OPNOTICE      Envia una noticia cuando los comandos
	                  OP/DEOP son usados
	    PEACE         Regula el uso de comandos criticos
	    PRIVATE       Esconde el canal del comando LIST
	    RESTRICTED    Restrinje el acceso al canal
	    SECURE        Activa las opciones de seguridad de %S
	    SECUREOPS     Estricto control del estatus de chanop
	    SECUREFOUNDER Estricto control del estatus de fundador
	                  de canal
	    SIGNKICK      Firma kicks hechos con el comando KICK
	    TOPICLOCK     El topic solo puede ser cambiado con
	                  TOPIC
	    XOP           Cambia el sistema de privilegios de
	                  usuario
	
	Tipea %R%S HELP opcion para mayor informacion
	acerca de una opcion en particular.

CHAN_HELP_SET_FOUNDER
	Sintaxis: SET canal FOUNDER nick
	
	Cambia el fundador de un canal. El nuevo nickname debe
	ser uno registrado.

CHAN_HELP_SET_SUCCESSOR
	Sintaxis: SET canal SUCCESSOR nick
	
	Cambia el sucesor de un canal. Si el nickname fundador
	expira o es botado mientras el canal esta aun registrado,
	el sucesor se convertira en el nuevo fundador del canales.
	Sin embargo, si el sucesor ya tiene muchos canales
	registrados (%d), el canal sera botado en vez, tal y como
	si no hubiese un sucesor. El nuevo nickname debe ser uno
	registrado.

CHAN_HELP_SET_PASSWORD
	Sintaxis: SET canal PASSWORD clave
	
	Setea la clave usado para identificarse como el
	fundador del canal.

CHAN_HELP_SET_DESC
	Sintaxis: SET canal DESC descripcion
	
	Setea la descripcion para el canal, la que es mostrada
	con los comandos LIST y INFO.

CHAN_HELP_SET_URL
	Sintaxis: SET canal URL [url]
	
	Asocia la URL dada con el canal. Esta URL sera mostrada
	cuando alguien solicite informacion sobre el canal con
	el comando INFO. Si ningun parametro es dado, 
	borra la URL actual para el canal.

CHAN_HELP_SET_EMAIL
	Sintaxis: SET canal EMAIL [direccion]
	
	Asocia la direccion E-mail dada con el canal.
	Esta direccion sera mostrada cuando alguien solicite
	informacion sobre el canal con el comando INFO.
	Si ningun parametro es dado, borra la direccion E-mail
	actual para el canal.

CHAN_HELP_SET_ENTRYMSG
	Sintaxis: SET canal ENTRYMSG [mensaje]
	
	Setea el mensaje que sera enviado via /notice a los
	usuarios cuando ingresen al canal. Si ningun parametro
	es dado, ningun mensaje sera enviado al entrar al canal.
	
CHAN_HELP_SET_BANTYPE
	Sintaxis: SET canal BANTYPE tipo_de_ban
	
	Setea el tipo de ban que sera usado por los
	Servicios cuando necesiten banear a alguien
	de tu canal.
	
	tipo de ban es un numero entre 0 y 3 que significa:
	
	0: ban en la forma *!usuario@host
	1: ban en la forma *!*usuario@host
	2: ban en la forma *!*@host
	3: ban en la forma *!*usuario@*.dominio

CHAN_HELP_SET_KEEPTOPIC
	Sintaxis: SET canal KEEPTOPIC {ON | OFF}
	
	Habilita o deshabilita la opcion de retencion de topic 
	para un canal. Cuando la retencion de topic esta
	seteada, el topic para el canal sera recordado por
	%S incluso despues de que el ultimo usuario
	salga del canal, y sera restorado la proxima vez que
	el canal sea creado.

CHAN_HELP_SET_TOPICLOCK
	Sintaxis: SET canal TOPICLOCK {ON | OFF}
	
	Abilita o desabilita la opcion de fijacion de topic
	para un canal. Cuando la fijacion de topic esta
	seteada, %S no permitira que el topico del canal
	sea cambiado excepto con el comando TOPIC.

CHAN_HELP_SET_MLOCK
	Sintaxis: SET canal MLOCK modos
	
	Setea el parametro de fijacion de modos para el canal.
	%S te permite definir ciertos modos de canal para estar
	siempre encendidos o apagados o libres de estar uno de los
	anteriores.
	
	El parametro modos es construido de la misma forma
	que el comando /MODE; esto significa, modos seguidos
	por un + estan fijados encendidos, y modos seguidos por
	un - estan fijados apagados. Tener en cuenta que, a
	diferencia del comando MODE, cada uso de SET MLOCK
	removera todos los modos fijados previamente antes de
	setear los nuevos!
	
	Advertencia: Si seteas una key con fijacion de modo, como
	en el segundo ejemplo mas abajo, deberas tambien setear la
	opcion RESTRICTED para el canal (ver HELP SET RESTRICTED),
	o quien quiera que entre al canal cuando este vacio podra ver la
	key!
	
	Ejemplos:
	
	    SET #canal MLOCK +nt-iklps
	        Fuerza los modos n y t encendidos, y los modos i, k,
	        l, p y s apagados. El modo m es dejado libre para
	        estar encendido o apagado.
	
	    SET #canal MLOCK +knst-ilmp mi-key
	        Fuerza los modos k, n, s, y t encendidos, y los modos
	        i, l, m, y p apagados. Tambien fuerza la key del canal
	        a ser "mi-key".
	
	    SET #canal MLOCK +
	        Remueve la fijacion de modos; todos los modos del canal
	        son libres de estar encendidos o apagados.

CHAN_HELP_SET_PEACE
	Sintaxis: SET canal PEACE {ON | OFF}
	
	Habilita o deshabilita la opcion de paz para un
	canal. Cuando paz esta seteada, un usuario no
	podra patear, banear o remover un estatus de canal
	de un usuario con nivel igual o superior a el via
	comandos de %S.

CHAN_HELP_SET_PRIVATE
	Sintaxis: SET canal PRIVATE {ON | OFF}
	
	Habilita o deshabilita la opcion de privacidad para un canal.
	Cuando privacidad esta seteada, un %R%S LIST no
	incluira el canal en ninguna lista.

CHAN_HELP_SET_RESTRICTED
	Sintaxis: SET canal RESTRICTED {ON | OFF}
	
	Enables or disables the restricted access option for a
	channel.  When restricted access is set, users not on
	the access list will instead be kicked and banned from the channel.

CHAN_HELP_SET_SECURE
	Sintaxis: SET canal SECURE {ON | OFF}
	
	Habilita o deshabilita las caracteristicas de seguridad
	de %S para un canal. Cuando SECURE esta
	seteado, solo usuarios que han registrado sus nicknames
	con %s y han usado IDENTIFY con su clave seran
	otorgados acceso al canal controlado por la lista de
	acceso.

CHAN_HELP_SET_SECUREOPS
	Sintaxis: SET canal SECUREOPS {ON | OFF}
	
	Habilita o deshabilita la opcion seguridad de ops para un
	canal.  Cuando seguridad de ops esta seteada, usuarios
	que no estab en la lista de usuarios no seran permitidos a
	obtener el status de operador de canal.
	
CHAN_HELP_SET_SECUREFOUNDER
	Sintaxis: SET canal SECUREFOUNDER {ON | OFF}
	
	Habilita o deshabilita la opcion seguridad de fundador para
	un canal. Cuando seguridad de fundador esta seteada, solo
	el fundador real podra botar el canal, cambiar su clave,
	su fundador y su sucesor, y no aquellos que esten identificados
	con %S.
	
CHAN_HELP_SET_SIGNKICK
	Sintaxis: SET canal SIGNKICK {ON | LEVEL | OFF}
	
	Habilita o deshabilita los kicks firmados para un
	canal. Cuando SIGNKICK esta seteado, kicks
	hechos con el comando %S KICK tendran el
	nick que uso el comando en la razon.
	
	Si usas LEVEL, aquellos que tienen nivel superior o
	igual al nivel SIGNKICK en el canal no tendran sus kicks 
	firmados. Ver %R%S HELP LEVELS para mayor informacion.

CHAN_HELP_SET_XOP
	Sintaxis: SET canal XOP {ON | OFF}
	
	Habilita o deshabilita el sistema de listas xOP para un canal.
	Cuando XOP esta seteado, debes usar los comandos AOP/SOP/VOP
	para dar privilegios de canal a usuarios. De otra manera
	debes usar el comando ACCESS.
	
	Nota Tecnica: cuando cambias de lista de acceso a sistema
	de listas xOP, tu definicion de niveles de usuario sera
	cambiada, asi que no tendras los mismos valores si vuelves a
	cambiar a lista de acceso!
	
	Tambien deberias chequear si tus usuarios estan en la lista xOP
	correspondiente despues del cambio, ya que la deduccion no es
	siempre perfecta... de hecho, no es recomendado el usar listas
	xOP si cambiaste la definicion de niveles con el comando
	LEVELS.
	
	Cambiar de listas xOP al sistema de listas de acceso no causa
	problema alguno.

CHAN_HELP_SET_OPNOTICE
	Sintaxis: SET canal OPNOTICE {ON | OFF}
	
	Habilita o deshabilita la opcion de op-notice para un canal.
	Cuando op-notice esta seteado, %S enviara una noticia
	al usuario del canal cuando los comandos OP o DEOP sean
	usados para un usuario en el canal.

CHAN_HELP_AOP
	Sintaxis: AOP canal ADD nick
	          AOP canal DEL {nick | instancia | lista}
	          AOP canal LIST [mascara | lista]
	          AOP canal CLEAR
	
	Mantiene la lista OP (AutoOP) para un canal. La lista
	AOP le da al usuario el derecho de ser automaticamente
	opeado en tu canal, desbaneado o invitarse a si mismos de
	ser necesario, mostrar el mensaje de saludo al entrar, etc.
	
	El comando AOP ADD agrega al nickname dado a la lista
	AOP.
	
	El comando AOP DEL remueve el nickname dado de la
	lista AOP. Si una lista de instancias es dada, esas
	instancias son borradas. (Ver el ejemplo para LIST mas
	abajo.)
	
	El comando AOP LIST muestra la lista AOP. Si una
	mascara con comodin es dada, solo las instancias
	concordando la mascara son mostradas. Si una lista de
	instancias es dada, solo esas instancias son mostradas;
	por ejemplo:
	
	   AOP #canal LIST 2-5,7-9
	      Muestra instancias de AOP enumeradas 2 a 5 y
	      7 a 9.
	
	El comando AOP CLEAR limpia todas las instancias de la
	lista AOP.
	
	Los comandos AOP ADD y AOP DEL estan limitados a
	SOPs o superior, mientras que el comando AOP CLEAR
	solo puede ser usado por el fundador del canal. Aun asi,
	cualquier usuario en la lista AOP puede usar el comando
	AOP LIST.
	
	Este comando puede haber sido deshabilitado para tu canal.
	Si este es el caso, debes usar la lista de acceso. Ver
	%R%S HELP ACCESS para incormacion acerca la lista
	de acceso y %R%S HELP SET XOP para saber como cambiar
	entre la lista de acceso y el sistema de listas xOP.

CHAN_HELP_HOP
	Sintaxis: HOP canal ADD nick
	          HOP canal DEL {nick | instancia | lista}
	          HOP canal LIST [mascara | lista]
	          HOP canal CLEAR
	
	Mantiene la lista HOP (HalfOP) para un canal. La lista
	HOP le da al usuario el derecho de ser automaticamente
	halfopeado en tu canal.
	
	El comando HOP ADD agrega al nickname dado a la lista
	HOP.
	
	El comando HOP DEL remueve el nickname dado de la
	lista HOP. Si una lista de instancias es dada, esas
	instancias son borradas. (Ver el ejemplo para LIST mas
	abajo.)
	
	El comando HOP LIST muestra la lista HOP. Si una
	mascara con comodin es dada, solo las instancias
	concordando la mascara son mostradas. Si una lista de
	instancias es dada, solo esas instancias son mostradas;
	por ejemplo:
	
	   HOP #canal LIST 2-5,7-9
	      Muestra instancias de HOP enumeradas 2 a 5 y
	      7 a 9.
	
	El comando HOP CLEAR limpia todas las instancias de la
	lista HOP.
	
	Los comandos HOP ADD y HOP DEL estan limitados a
	AOPs o superior, mientras que el comando HOP CLEAR
	solo puede ser usado por el fundador del canal.
	
	Este comando puede haber sido deshabilitado para tu canal.
	Si este es el caso, debes usar la lista de acceso. Ver
	%R%S HELP ACCESS para incormacion acerca la lista
	de acceso y %R%S HELP SET XOP para saber como cambiar
	entre la lista de acceso y el sistema de listas xOP.

CHAN_HELP_SOP
	Sintaxis: SOP canal ADD nick
	          SOP canal DEL {nick | instancia | lista}
	          SOP canal LIST [mascara | lista]
	          SOP canal CLEAR
	
	Mantiene la lista SOP (SuperOP) para un canal. La lista
	SOP le da al usuario los derechos dados por la lista AOP,
	y agrega aquellos necesarios para usar las listas de
	AutoKick y BadWords, mandar y leer memos de canal, etc.
	
	El comando SOP ADD agrega al nickname dado a la lista
	SOP.
	
	El comando SOP DEL remueve el nickname dado de la
	lista SOP. Si una lista de instancias es dada, esas
	instancias son borradas. (Ver el ejemplo para LIST mas
	abajo.)
	
	El comando SOP LIST muestra la lista SOP. Si una
	mascara con comodin es dada, solo las instancias
	concordando la mascara son mostradas. Si una lista de
	instancias es dada, solo esas instancias son mostradas;
	por ejemplo:
	
	   SOP #canal LIST 2-5,7-9
	      Muestra instancias de SOP enumeradas 2 a 5 y
	      7 a 9.
	
	El comando SOP CLEAR limpia todas las instancias de la
	lista SOP.
	
	Los comandos SOP ADD, SOP DEL y SOP CLEAR estan
	limitados al fundador del canal. Cualquier usuario en la
	lista AOP puede usar el comando SOP LIST.
	
	Este comando puede haber sido deshabilitado para tu canal.
	Si este es el caso, debes usar la lista de acceso. Ver
	%R%S HELP ACCESS para incormacion acerca la lista
	de acceso y %R%S HELP SET XOP para saber como cambiar
	entre la lista de acceso y el sistema de listas xOP.

CHAN_HELP_VOP
	Sintaxis: VOP canal ADD nick
	          VOP canal DEL {nick | instancia | lista}
	          VOP canal LIST [mascara | lista]
	          VOP canal CLEAR
	
	Mantiene la lista VOP (VOicePeople) para un canal.
	La lista VOP le da al usuario el derecho de ser
	automaticamente dado voice o darse voice a si mismos.
	
	El comando VOP ADD agrega al nickname dado a la lista
	VOP.
	
	El comando VOP DEL remueve el nickname dado de la
	lista VOP. Si una lista de instancias es dada, esas
	instancias son borradas. (Ver el ejemplo para LIST mas
	abajo.)
	
	El comando VOP LIST muestra la lista VOP. Si una
	mascara con comodin es dada, solo las instancias
	concordando la mascara son mostradas. Si una lista de
	instancias es dada, solo esas instancias son mostradas;
	por ejemplo:
	
	   VOP #canal LIST 2-5,7-9
	      Muestra instancias de VOP enumeradas 2 a 5 y
	      7 a 9.
	
	El comando VOP CLEAR limpia todas las instancias de la
	lista VOP.
	
	Los comandos VOP ADD, VOP DEL y VOP LIST estan
	limitados a AOPs o superior, mientras que el comando
	VOP CLEAR solo puede ser usado por el fundador del
	canal.
	
	Este comando puede haber sido deshabilitado para tu canal.
	Si este es el caso, debes usar la lista de acceso. Ver
	%R%S HELP ACCESS para incormacion acerca la lista
	de acceso y %R%S HELP SET XOP para saber como cambiar
	entre la lista de acceso y el sistema de listas xOP.

CHAN_HELP_ACCESS
	Sintaxis: ACCESS canal ADD nick nivel
	          ACCESS canal DEL {nick | indices | lista}
	          ACCESS canal LIST [mascara | lista]
	          ACCESS canal CLEAR
	
	Mantiene la lista de acceso para un canal. La lista
	de acceso especifica que usuarios estan permitidos atener
	el estatus de operador de canal o acceso a comandos de
	%S en el canal.  Diferentes niveles de usuarios permiten
	acceso a diferentes subniveles de privilegios;
	%R%S HELP ACCESS LEVELS para informacion mas
	especifica. Cualquier nick no presente en la lista de
	acceso tiene un nivel 0.
	
	El comando ACCESS ADD añade el nickname dado a la lista
	de acceso con el nivel de usuario dado; si el nick ya esta
	presente en la lista, el nivel de acceso es cambiado al
	especificado en el comando. El nivel especificado
	debe ser menos que el del usuario dando el comando, y si
	el nick ya esta en la lista de acceso, el acceso actual
	del nick debe ser menor que el acceso del usuario dando el
	comando.
	
	El comando ACCESS DEL remueve el nick dado de la lista
	de acceso. Si una lista de indices es dada, esas instancias
	son borradas.  (Ver el ejeplo para LIST mas abajo.)
	
	El comando ACCESS LIST muestra la lista de acceso. Si
	una mascara con comodin es dada, solo esas instancias
	concordando a la mascara son mostradas. Si una lista de
	indices es dada, solo esas instancias son mostrdas; por
	ejemplo:

	   ACCESS #canal LIST 2-5,7-9
	      Lista instancias de acceso numeradas 2 hasta 5 y
	      7 hasta 9.
	      
	El comando ACCESS CLEAR limpia todas las instancias
	de la lista de acceso.

CHAN_HELP_ACCESS_LEVELS
	Niveles de acceso para usuarios
	
	Por defecto, los siguientes niveles de acceso estan definidos:
	
	   Fundador   Acceso completo a las funciones de %S;
	                     Op automatico al entrar al canal. Notese
	                     que solo una persona puede tener estatus
	                     de fundador (no puede ser dado usando el
	                     comando ACCESS).
	        10   Acceso al comando AKICK; Op automatico.
	         5   Op automatico.
	         3   Voice automatico.
	         0   Ningun privilegio especial; Puede ser opeado
	                     por otros ops (a menos que seguridad de
	                     ops este seteada).
	        <0   No puede ser opeado.
	
	Esyos niveles pueden ser cambiado, o nuevos añadidos,
	usando el comando LEVELS; tipea %R%S HELP LEVELS
	para mayor informacion.

CHAN_HELP_AKICK
	Sintaxis: AKICK channel ADD {nick | mask} [reason]
	          AKICK channel STICK mask
	          AKICK channel UNSTICK mask
	          AKICK channel DEL {nick | mask | entry-num | list}
	          AKICK channel LIST [mask | entry-num | list]
	          AKICK channel VIEW [mask | entry-num | list]
	          AKICK canal ENFORCE
	          AKICK canal CLEAR
	
	Mantiene la lista de AutoKicks para un canal. Si
	un usuario en la lista de Autokicks intenta entrar
	al canal, %S baneara al usuario del canal, luego lo
	kickeara.
	
	El comando AKICK ADD añade el nick o mascara de
	usuario dado a la lista de Autokicks. Si una razon
	es dada con el comando, esta razon sera usada cuando el
	usuario es kickeado; si no, la razon por defecto es
	"Has sido baneado del canal".
	When akicking a registered nick the nickserv account
	will be added to the akick list instead of the mask.
	All users within that nickgroup will then be akicked.
	
	El comando AKICK STICK banea permanente la mascara
	dada en el canal. Si alguien intenta remover el ban,
	%S la seteara nuevamente automaticamente. No puede ser
	usado para nicks registrados.
	
	El comando AKICK UNSTICK cancela el efecto del
	comando AKICK STICK, asi que sera posible remover
	el ban del canal nuevamente.
	
	El comando AKICK DEL remueve el nick o mascara dados
	de la lista de AutoKicks. Aun asi, no remueve ningun ban
	puesto por un AutoKick; aquellos deben ser removidos
	manualmente.
	
	El comando AKICK LIST muestra la lista de AutoKicks,
	u opcionalmente solo las instancias de AutoKick AutoKick
	que concuerden con la mascara dada.
	
	El comando AKICK VIEW es una version mas detallada
	del comando AKICK LIST.
	
	El comando AKICK ENFORCE hace a %S a enforzar la
	lista AKICK actual removiendo a los usuarios que
	concuerden la mascara AKICK.
	
	El comando AKICK CLEAR limpia todas las instancias
	de ka lista de akicks.

CHAN_HELP_LEVELS
	Sintaxis: LEVELS canal SET tipo nivel
	        LEVELS canal {DIS | DISABLE} tipo
	        LEVELS canal LIST
	        LEVELS canal RESET
	
	El comando LEVELS permite un control fino sobre el
	significado de los niveles de acceso numericos usados para
	canales. Con este comando, puedes definir el nivel de acceso
	requerido para la mayoria de las funciones de %S.
	(Los comandos SET FOUNDER y SET PASSWORD, asi como
	este comando estan siempre restringidos al fundador del
	canal.)
	
	LEVELS SET permite cambiar el nivel de acceso para una funcion
	o un grupo de funciones. LEVELS DISABLE (o DIS para corto)
	desabilita una caracteristica automatica o prohibe acceso a una
	funcion para todos excepto el fundador del canal.
	LEVELS LIST muestra los niveles actuales para cada funcion o
	grupo de funciones. LEVELS RESET resetea los niveles a los 
	niveles por defecto de un canal recien creado (ver
	HELP ACCESS LEVELS).
	
	Para una lista de las caracteristicas y funciones a los que
	se les puede cambiar los niveles, ver HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Los siguientes nombres de decaracteristicas/funciones son
	entendidos. Tener en cuenta que los niveles para AUTODEOP
	y NOJOIN son niveles maximos, mientras todos los otros
	son niveles minimos.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Sintaxis: INFO canal [ALL]
	
	Lista informacion acerca del canal registrado dado,
	incluyendo su fundador, tiempo de registracion, ultima
	vez usado, descripcion y fijacion de modos, si existen.
	Si ALL es especificado, el mensaje de entrada y
	sucesor tambien seran mostrados.
	
	Por defecto, la opcion ALL esta limitada a aquellos
	con acceso de fundador en el canal.

CHAN_HELP_LIST
	Sintaxis: LIST patron
	
	Lista todos los canales registrados concordando el patron
	dado (Canales con la opcion PRIVATE seteada no son
	listados.)

CHAN_HELP_OP
	Sintaxis: OP [#canal [nick]]
	
	Opea al nick seleccionado en un canal. Si nick no es
	dado, te opeara a ti. SI canal y nick no son dados,
	te opeara en todos los canales en los que estes,
	siempre y cuando tengas los permisos para hacerlo.
	
	Por defecto, limitado a AOPs o aquellos con nivel de
	acceso 5 y mayor en el canal.

CHAN_HELP_DEOP
	Sintaxis: DEOP [#canal [nick]]
	
	Deopea al nick seleccionado en un canal. Si nick no es
	dado, te deopeara a ti. Si canal y nick no son dados,
	te deopeara a ti en todos los canales en los que estes,
	siempre y cuando tengas los permisos para hacerlo.
	
	Por defecto, limitado a AOPs o aquellos con nivel de
	acceso 5 y mayor en el canal.
	
CHAN_HELP_VOICE
	Sintaxis: VOICE [#canal [nick]]
	
	Da voice al nick seleccionado en un canal. Si nick no
	es dado, te dara voice a ti. Si canal y nick no son
	dados, te dara voice en todos los canales en los que
	estes, siempre y cuando tengas los permisos para
	hacerlo.
	
	Por defecto, limitado AOPs o aquellos con nivel de
	acceso 5 y mayor en el canal, o a VOPs o aquellos
	con nivel 3 y mayor para darse voice a si mismos.

CHAN_HELP_DEVOICE
	Sintaxis: DEVOICE [#canal [nick]]
	
	Quita voice al nick seleccionado en un canal. Si nick no
	es dado, te quitara voice a ti. Si canal y nick no son
	dados, te quitara voice en todos los canales en los que
	estes, siempre y cuando tengas los permisos para hacerlo.
	
	Por defecto, limitado a AOPs o aquellos con nivel de
	acceso 5 y mayor en el canal, o a VOPs o aquellos con
	nivel 3 y mayor para darse voice a si mismos.
	
CHAN_HELP_HALFOP
	Sintaxis: HALFOP [#canal [nick]]
	
	Da Halfop al nick seleccionado en un canal. Si nick no
	es dado, te dara halfop a ti. Si canal y nick no son
	dados, te dara halfop en todos los canales en los que
	estes, siempre y cuando tengas los permisos para hacerlo.
	
	Por defecto, limitado a AOPs y aquellos con nivel de
	acceso 5 y mayor en el canal, o a HOPs o aquellos con nivel
	4 y mayor para darse halfop a si mismos.

CHAN_HELP_DEHALFOP
	Sintaxis: DEHALFOP [#canal [nick]]
	
	Quita Halfop al nick seleccionado en un canal. Si nick no
	es dado, te quitara halfop a ti. Si canal y nick no son
	dados, te quitara halfop en todos los canales en los que
	estes, siempre y cuando tengas los permisos para hacerlo.
	
	Por defecto, limitado a AOPs y aquellos con nivel de
	acceso 5 y mayor en el canal, o a HOPs o aquellos con
	nivel 4 y mayor para quitarse halfop a si mismos.

CHAN_HELP_PROTECT
	Sintaxis: PROTECT [#canal [nick]]
	
	Proteje al nick seleccionado en un canal. Si nick no es
	dado, te protejera a ti. Si canal y nick no son dados,
	te protejera en todos los canales en los que estes,
	siempre y cuando tengas los permisos para hacerlo.
	
	Por defecto, limitado al fundador, o a SOPs o aquellos
	con nivel 10 y mayor en el canal para protejerse a si
	mismos.

CHAN_HELP_DEPROTECT
	Sintaxis: DEPROTECT [#canal [nick]]
	
	Desproteje al nick seleccionado en un canal. Si nick no
	es dado, te desprotejera a ti. Si canal y nick no son
	dados, te desprotejera en todos los canales en los que
	estes, siempre y cuando tengas los permisos para hacerlo.
	
	Por defecto, limitado al fundador, o a SOPs o aquellos
	con nivel 10 y mayor en el canal para desprotejerse a
	si mismos.

CHAN_HELP_OWNER
	Sintaxis: OWNER [#canal]
	
	Te da el estatus de dueño en canal. Si canal no
	es dado, te dara estatus de dueño en todos los
	canales en los que estes, siempre y cuando tengas
	permiso para hacerlo.
	
	Limitado a aquellos con nivel de fundador en el canal.

CHAN_HELP_DEOWNER
	Sintaxis: DEOWNER [#canal]
	
	Remueve tu estatus de dueño en canal. Si canal no
	es dado, remueve tu estatus de dueño en todos los
	canales en los que estes, siempre y cuando tengas
	permiso para hacerlo.
	
	Limitado a aquellos con nivel de fundador en el canal.

CHAN_HELP_INVITE
	Sintaxis: INVITE canal
	
	Le dice a %S que te invite al canal dado.
	
	Por defecto, limitado a AOPs o aquellos con nivel 5
	y mayor en el canal.

CHAN_HELP_UNBAN
	Sintaxis: UNBAN canal
	
	Le dice a %S que remueva todos los bans previeniendo
	que entres al canal dado.
	
	Por defecto, limitado a AOPs o aquellos con nivel 5
	y mayor en el canal.

CHAN_HELP_KICK
	Sintaxis: KICK [#canal [nick [razon]]]
	
	Kickea al nick seleccionado en el canal. Si nick no es
	dado, te kickeara a ti. Si canal y nick no son dados,
	te kickeara en todos los canales, siempre y cuando tengas
	permiso para hacerlo.
	
	Por defecto, limitado a AOPs o aquellos con nivel de
	acceso 5 y mayor en el canal.

CHAN_HELP_BAN
	Sintaxis: BAN [#canal [nick [razon]]]
	
	Banea un nick seleccionado en un canal. Si el nick no
	es dado, te baneara a ti. Si canal y nick no son dados,
	te baneara de todos los canales, siempre y cuando tengas
	permiso para hacerlo.
	
	Por defecto, limitado a AOPs o aquellos con nivel de
	acceso 5 y mayor en el canal.

CHAN_HELP_TOPIC
	Sintaxis: TOPIC canal [topico]
	
	Hace a %S setear el topic del canal al especificado.
	If topico no es dado, un topico vacio es seteado.
	Este comando es mas util usado en conjunto a SET
	TOPICLOCK. Ver %R%S HELP SET TOPICLOCK
	para mayor informacion.
	
	Por defecto, limitado a aquellos con nivel de fundador
	en el canal.

CHAN_HELP_CLEAR
	Sintaxis: CLEAR canal que
	
	Le dice a %S que limpie ciertos seteos en el canal.
	que puede ser uno de los siguientes:
	
	     MODES    Resetea todos los modos en el canal (osea,
	                  limpia los modos i,k,l,m,n,p,s,t).
	     BANS     Limpia todos los bans en el canal.
	     EXCEPTS  Limpia todos los excepts en el canal.
	     INVITES  Clears all invites on the channel.
	     OPS      Remueve estatus de operador de canal(modo +o)
	                  de todos los operadores de canal.
	     HOPS     Removes channel-halfoperator status (mode +h) from
	                  all channel halfoperators, if supported.
	     VOICES   Remueve el estatus "voice" (modo +v) de quien
	                  tenga ese modo seteado.
	     USERS    Remueve (kickea) todos los usuarios del canal.
	
	Por defecto, tu nivel de acceso en el canal debe ser de
	fundador.

CHAN_HELP_GETKEY
	Sintaxis: GETKEY canal
	
	Retorna la key del canal dado. Este es un comando
	principalmente para ser usado por bots y/o scripts,
	asi que el resultado es de la siguiente manera:
	
	KEY <canal> <key>
	
	key es "NO KEY" si no hay key seteada.
	
CHAN_HELP_SENDPASS
	Sintaxis: SENDPASS canal
	
	Envia la clave del canal dado a la direccion email seteada
	en el archivo del nickname del fundador. Este comando
	es sumamente util para lidiar con claves perdidas.
	
	Puede ser limitado a  Operadores de IRC en ciertas redes.
	
	Este comando no es disponible cuando encripcion esta abilitada.

CHAN_SERVADMIN_HELP
	
	Administradores de Servicios tambien pueden botar un canal
	sin tener que identificarse por medio de clave, y pueden
	ver el acceso, AKICK, y seteos de las listas de niveles para
	cualquier canal.

CHAN_SERVADMIN_HELP_LOGOUT
	Sintaxis: LOGOUT canal [nickname]
	
	Este comando hace al nick seleccionado ya no identificado
	para el canal dado.
	
	Si tu eres el fundador del canal, puedes desconectar a
	quien sea, de otra manera solo te puedes desconectar a
	ti mismo.
	
	Si eres un administrador de Servicios, puedes
	desconectar a quien sea de un canal sin ser el fundador
	del canal. Ademas, puedes omitir el parametro nickname;
	esto desconectara a todos los usuarios identificados
	del canal.

CHAN_SERVADMIN_HELP_DROP
	Sintaxis: DROP canal
	
	Desregistra el canal dado.  Solo
	administradores de Servicios pueden botar un
	canal por el cual no se hayan identificado.

CHAN_SERVADMIN_HELP_SET
	
	Administradores de Servicios pueden tambien setear la
	opcion NOEXPIRE, con la cual canales pueden ser
	prevenidos a expirar. Ademas, administradores de
	Servicios pueden setear opciones para cualquier canal
	sin tener que identificarse por clave en el canal.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Sintaxis: SET canal NOEXPIRE {ON | OFF}
	
	Setea si el canal expirara. Seteando esto
	a ON previene el canal a expirar.
	
	Limitado a administradores de Servicios.

CHAN_SERVADMIN_HELP_INFO
	
	Administradores de Servicios pueden usar el parametro
	ALL con cualquier canal.

CHAN_SERVADMIN_HELP_LIST
	Sintaxis: LIST patron [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
	
	Lista todos los canales registrados concordando el patron dado.
	Canales con la opcion PRIVATE seteada solo seran mostrados
	a administradores de Servicios. Canales con la opcion NOEXPIRE
	tendran un ! antepuesto al nombre del canal para
	administradores de Servicios.
	
	Si las opciones FORBIDDEN o NOEXPIRE son dadas, solo canales
	que, respectivamente, tienen FORBID o NOEXPIRE seteado seran
	mostrados. Si ambas opciones son dadas, ambos tipos de canales
	seran mostrados. Lo mismo aplica para canales SUSPENDED. Estas 
	opciones estan limitadas a administradores de Servicios.

CHAN_SERVADMIN_HELP_GETPASS
	Sintaxis: GETPASS canal
	
	Devuelve la clave para el canal dado. Tener en cuenta
	que cuando este comando es usado, un mensaje incluyendo la
	persona que ejecuto el comando y el canal en el que fue usado
	seran monitoreados y enviados como WALLOPS/GLOBOPS.
	
	Limitado a administradores de Servicios.

CHAN_SERVADMIN_HELP_FORBID
	Sintaxis: FORBID canal [razon]
	
	Prohibe a cualquiera de registrar o usar el canal
	dado. Puede ser cancelado botando el canal.
	
	Razon puede ser requerida en ciertas redes.
	
	Limitado a administradores de Servicios.

CHAN_SERVADMIN_HELP_SUSPEND
	Sintaxis: SUSPEND canal [razon]
	
	Impide a cualquier persona de registrar o usar el
	canal dado.  Puede ser cancelado usando el comando UNSUSPEND
	para preservar los datos/seteos previos.
	
	Razon puede ser mandatorio en ciertas redes.
	
	Limitado a administradores de Servicios.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Sintaxis: UNSUSPEND canal
	
	Libera un canal suspendido. Todos los datos y seteos
	antes de la suspencion son preservados.
	
	Limitado a administradores de Servicios.

CHAN_SERVADMIN_HELP_STATUS
	Sintaxis: STATUS canal nickname
	
	Devuelve el nivel de acceso actual del nick dado
	en el canal dado. La respuesta es de la forma:
	
	    STATUS canal nickname nivel-de-acceso
	
	Si un error ocurre, la respuesta sera de la forma:
	
	    STATUS ERROR mensaje-de-error
	
	Limitado a administradores de Servicios.

###########################################################################
#
# MemoServ help messages
#
###########################################################################
MEMO_HELP_CMD_SEND
	    SEND   Envia un memo a un nick o canal
MEMO_HELP_CMD_CANCEL
	    CANCEL Cancela el ultimo memo que enviaste
MEMO_HELP_CMD_LIST
	    LIST   Lista tus memos
MEMO_HELP_CMD_READ
	    READ   Lee uno o mas memos
MEMO_HELP_CMD_DEL
	    DEL    Borra uno o mas memos
MEMO_HELP_CMD_SET
	    SET    Setea opciones relacionadas a memos
MEMO_HELP_CMD_INFO
	    INFO   Displays information about your memos
MEMO_HELP_CMD_RSEND
	    RSEND  Sends a memo and requests a read receipt
MEMO_HELP_CMD_CHECK
	    CHECK  Checks if last memo to a nick was read
MEMO_HELP_CMD_SENDALL
	    SENDALL  Envia un memo a todos los usuarios registrados
MEMO_HELP_CMD_STAFF
	    STAFF    Envia un memo a todos los opers/admins

MEMO_HELP_HEADER
	%S es una utilidad que permite a usuarios de IRC
	enviar cortos mensajes a otros usuarios de IRC, esten
	en linea o no, o a canales(*). Ambos el nickname que
	envia y el nickname o canal destinatarios deben estar
	registrados para enviar un memo.
	
	Los comandos de %S incluyen:
	
	
MEMO_HELP_ADMIN
	not used.
MEMO_HELP_OPER
	not used.
MEMO_HELP_FOOTER
	
	Tipea %R%S HELP comando para ayuda de cualquiera
	de los comandos mencionados.

	(*) Por defecto, cualquier usuario con al menos nivel 10 de 
	    de acceso en el canal puede leer los memos del canal. Esto
	    puede ser cambiado con el comando %s LEVELS.

MEMO_HELP_SEND
	Sintaxis: SEND {nick | canal} texto-del-memo
	
	Envia un memo al nick o canal dado, conteniendo
	texto-del-memo. Al enviar a un nickname, el recipiente
	recibira noticia de que el/ella tiene un nuevo memo. El nick
	o canal recipientes deben estar registrados.

MEMO_HELP_CANCEL
	Sintaxis: CANCEL {nick | canal}
	
	Cancela el ultimo memo que enviaste al nick o canal dado,
	siempre y cuando no haya sido leido al momento en el que
	usaste el comando.

MEMO_HELP_LIST
	Sintaxis: LIST [canal] [lista | NEW]
	
	Lista los memos que tienes actualmente. Con NEW, lista
	solo los memos nuevos (no leidos). Memos nuevos estan
	marcados con un "*" a la izquierda del numero del memo.
	Tambien puedes especificar una lista de numeros, como en el
	ejemplo mas abajo:
	
	   LIST 2-5,7-9
	      Lista memos enumerados 2 a 5 y 7 a 9.

MEMO_HELP_READ
	Sintaxis: READ [canal] {numero | lista | LAST | NEW}
	
	Te envia el texto de los memos especificados. Si LAST es
	dado, te envia el ultimo memo recibido. si NEW es dado, te
	envia todos tus memos nuevos. De otra forma, te envia el
	memo numero numero.  Tambien puedes dar una lista de
	numeros, como en el ejemplo:
	
	   READ 2-5,7-9
	      Muestra los memos enumerados 2 a 5 y 7 a 9.

MEMO_HELP_DEL
	Sintaxis: DEL [canal] {numero | lista | LAST | ALL}
	
	Borra el o los memos especificados. Puedes proveer multiples
	numeros o rangos de numeros en vez de uno solo, como en el
	ejemplo mas abajo.
	
	Si LAST es dado, el ultimo memo sera eliminado.
	Si ALL es dado, borra todos tus memos.
	
	Ejemplos:
	
	   DEL 1
	      Borra tu primer memo.
	
	   DEL 2-5,7-9
	      Borra los memos enumerados 2 a 5 y 7 a 9.

MEMO_HELP_SET
	Sintaxis: SET opcion parametros
	
	Setea varias opciones de memos. opcion puede ser una de:
	
	    NOTIFY      Cambia cuando seras notificado acerca de
	                    nuevos memos (solo para nicknames)
	    LIMIT       Setea el maximo numero de memos que
	                    puedes recibir
	
	Tipea %R%S HELP SET opcion para mas informacion
	acerca de una opcion especifica.

MEMO_HELP_SET_NOTIFY
	Sintaxis: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
	
	Cambia cuando seras notificado acerca de nuevos memos:
	
	    ON      Seras notificado de memos cuando te conectes,
	               cuando saques tu /AWAY, y cuando te sean
	               enviados.
	    LOGON   Solo seras notificado de memos cuando te conectes
	               o cuando saques tu /AWAY.
	    NEW     Solo seras notificado de memos cuando te sean
	               enviados.
	    MAIL    You will be notified of memos by email aswell as
	               any other settings you have.
	    NOMAIL  You will not be notified of memos by email.
	    OFF     No recibiras ninguna notificacion por memos.
	
	ON es escencialmente LOGON y NEW combinados.

MEMO_HELP_SET_LIMIT
	Sintaxis: SET LIMIT [canal] limite
	
	Setea el maximo numero de memos que tu (o el canal dado)
	estas permitido a tener. Si seteas esto a 0, nadie podra
	enviarte memos. Ademas, no puedes setear el limite mas
	alto que %d.

MEMO_HELP_INFO
	Sintaxis: INFO [canal]
	
	Muestra informacion acerca del numero de memos que tienes,
	cuantos de ellos aun no han sido leidos, y cuantos memos
	puedes recibir. Con un parametro, muestra la misma
	informacion para el canal dado.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Sintaxis: SET LIMIT [usuario | canal] {limite | NONE} [HARD]
	
	Setea el maximo numero de memos un usuario o canal esta
	permitido a tener. Setear el limite a 0 previene al usuario
	de recibir memos; setearlo a NONE permite al usuario
	a recibir y guardar cuantos memos quiera. Si no das un
	nickname o canal, tu propio limite es seteado.
	
	Añadir HARD previene al usuario de cambiar el limite. No
	añadir HARD tiene el efecto contrario, permitiendo al
	usuario cambiar el limite (aun si un limite previo fue seteado
	con HARD).
	
	El uso del comando SET LIMIT esta limitado a
	administradores de Servicios. Otros usuarios pueden
	solo setear limite para ellos mismos o para un canal en
	el que tengan ciertos privilegios, no pueden remover su
	limite, no pueden setear el limite mas alla de %d, y no
	pueden setear un limite hard.

MEMO_SERVADMIN_HELP_INFO
	Sintaxis: INFO [nick | canal]
	
	Sin un parametro, muestra informacion acerca del numero
	de memos que tienes, cuantos de ellos aun no han sido
	leidos, y el numero total de memos que puedes recibir.
	
	Con un canal de parametro, muestra la misma informacion
	para el canal dado.
	
	Con un nickname de parametro, muestra la misma
	informacion para el nickname dado. Este uso esta
	limitado a administradores de Servicios.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_STAFF
	Sintaxis: STAFF texto-memo

	Envia a todo el staff de services un memo conteniendo texto-memo.
	Nota: Si tienes operadores en la lista de opers y en la lista de
	admins, reciviran el memo dos veces. Lo mismo de aplica para
	operadores en la lista de Root y en otras listas. 

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_SENDALL
	Sintaxis: SENDALL texto-memo

	Envia a todos los usuarios registrados un memo conteniendo texto-memo.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_RSEND
	Sintaxis: RSEND {nick | canal} texto-memo

	Envia a nick o canal un memo conteniendo memo-text. 
	Cuando se envia a un nickname, el receptor recibira una noticia 
	informandole que tiene un nuevo memo. El nickname o canal al cual se le 
	enviara el memo debe estar registrado.
	Una vez que el memo ha sido leido, quien envio el memo recibira una 
	notificacion automatica informando que el memo ha sido leido.	

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_CHECK
	Sintaxis: CHECK nick

	Verifica que el _ultimo_ memo que enviaste a nick haya sido leido.
	 Nota que esto solo funciona para nicknames, no para canales.

###########################################################################
#
# OperServ help messages
#
###########################################################################

OPER_HELP_CMD_GLOBAL
	    GLOBAL      Envia un mensaje a todos los usuarios
OPER_HELP_CMD_STATS
	    STATS       Muestra estatus de los Servicios y la red
OPER_HELP_CMD_OPER
	    OPER        Modifica la lista de operadores de Servicios
OPER_HELP_CMD_ADMIN
	    ADMIN       Modifica la lista de administradores de
	                Servicios
OPER_HELP_CMD_STAFF
	    STAFF	Muestra la lista del staff y su status
OPER_HELP_CMD_MODE
	    MODE        Cambia los modos de un canal
OPER_HELP_CMD_KICK
	    KICK        Kickea un usuario de un canal
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  Limpia los modos de un canal
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  Killea todos los usuarios en un host
OPER_HELP_CMD_AKILL
	    AKILL       Manipula la lista AKILL
OPER_HELP_CMD_SGLINE
	    SGLINE      Manipula la lista SGLINE
OPER_HELP_CMD_SQLINE
	    SQLINE      Manipula la lista SQLINE
OPER_HELP_CMD_SZLINE
	    SZLINE      Manipula la lista SZLINE
OPER_HELP_CMD_CHANLIST
	    CHANLIST    Lista todos los records de canal
OPER_HELP_CMD_USERLIST
	    USERLIST    Lista todos los records de usuarios
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   Define mensajes para ser mostrados a usuarios
	                al conectarse
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  Define mensajes para ser mostrados al azar a
	                usuarios al conectarse
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    Define mensajes a ser mostrados a usuarios al
	                transformarse en operadores
OPER_HELP_CMD_SESSION
	    SESSION     Muestra la lista de sesiones de host
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   Modifica la lista de limite de sesion
OPER_HELP_CMD_NOOP
	    NOOP        Temporalmente remueve todas las lineas O: de
	                un servidor remotamente
OPER_HELP_CMD_JUPE
	    JUPE        "Jupiter" un servidor
OPER_HELP_CMD_IGNORE
	    IGNORE	Ignora a un usuario
OPER_HELP_CMD_SET
	    SET         Setea varias opciones globales de los Servicios
OPER_HELP_CMD_RELOAD
	    RELOAD      Recarga la configuracion de los Servicios
OPER_HELP_CMD_UPDATE
	    UPDATE      Fuerza a la base de datos de los Servicios a
	                ser updateada a disco inmediatamente
OPER_HELP_CMD_RESTART
	    RESTART     Guarda la base de datos de los Servicios y
	                recarga los Servicios
OPER_HELP_CMD_QUIT
	    QUIT        Termina el programa de servicios sin guardar
	                los cambios
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    Termina el programa de Servicios guardando los
	                cambios
OPER_HELP_CMD_DEFCON
	    DEFCON      Manipulate the DefCon system
OPER_HELP_CMD_CHANKILL
	    CHANKILL    AKILL all users on a specific channel
OPER_HELP_CMD_OLINE
	    OLINE	Otorga privilegios IRCOP temporarios
OPER_HELP_CMD_UMODE
	    UMODE	Otorga cambios de modos de usuario
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
	Comandos para %S:

OPER_HELP_LOGGED
	Noticia: todos los comandos enviados a %S son monitoreados!

OPER_HELP_GLOBAL
	Sintaxis: GLOBAL mensaje
	
	Permite a los Administrators a enviar mensajes a los usuarios
	en la red. El mensaje sera enviado desde el nick %s.

OPER_HELP_STATS
	Sintaxis: STATS [AKILL | ALL | RESET]
	
	Sin una opcion, muestra el numero actual de usuarios e
	IRCops en linea (excluyendo a los Servicios), el maximo
	numero de usuarios desde que los Servicios partieron, y
	el tiempo que los Servicios han estado corriendo.
	
	Con la opcion AKILL, muestra el tamaño actual de la
	lista AKILL y el actual tiempo de expiracion por defecto.
	
	La opcion ALL esta solo disponible a administradores de
	Servicios, y muestra informacion en el uso de memoria de
	los Servicios. Usar esta opcion puede congelar a los
	Servicios por un corto periodo de tiempo en redes grandes,
	asi que no lo uses demasiado!
	
	La opcion RESET resetea la cuenta maxima de usuarios
	al numero de usuarios actualmente presentes en la red.
	
	UPTIME puede ser usado como sinonimo para STATS.

OPER_HELP_OPER
	Sintaxis: OPER ADD nick
	          OPER DEL {nick | numero | lista}
	          OPER LIST [mascara | lista]
	          OPER CLEAR
	
	Permite a los root de Servicios añadir o
	remover nicknames a la lista de operadores de
	Servicios. Un usuario con su nickname en la lista de
	operadores de Servicios y que ha sido identificado
	con %s podra acceder comandos de operador de
	Servicios.
	
	El comando OPER ADD añade el nickname dado a la
	lista de operadores de Servicios.
	
	El comando OPER DEL remueve el nickname dado de
	la lista de operadores de Servicios. Si una lista de
	instancias es dada, esas instancias son borradas.
	(Ver el ejemplo para LIST mas abajo.)
	
	El comando OPER LIST muestra la lista de
	operadores de Servicios. Si una mascara es dada, solo
	las instancias que concuerden con la mascara son
	mostradas. Si una lista de numeros es dada, solo esas
	instancias son mostradas; por ejemplo:
	
	   OPER LIST 2-5,7-9
	      Lista instancias de operadores de Servicios
	      enumeradas 2 a 5 y 7 a 9.
	
	El comando OPER CLEAR limpia todas las instancias
	de la lista de operadores de Servicios.
	
	Cualquier operador de IRC puede usar la forma
	OPER LIST del comando.

OPER_HELP_ADMIN
	Sintaxis: ADMIN ADD nick
	          ADMIN DEL {nick | numero | lista}
	          ADMIN LIST [mascara | lista]
	          ADMIN CLEAR
	
	Permite al root de Servicios a añadir o remover nicknames
	a la lista de administradores de Servicios. Un usuario
	cuyo nickname esta en la lista de administradores de
	Servicios y que ha sido identificado con %s podra
	acceder comandos de administradores de Servicios.
	
	El comando ADMIN ADD añade el nickname dado a la lista
	de administradores de Servicios.
	
	El comando ADMIN DEL remueve el nickname dado de la
	lista de administradores de Servicios. Si una lista de
	instancias es dada, esas instancias son borradas.
	(Ver el ejemplo para LIST mas abajo.)
	
	El comando ADMIN LIST muestra la lista de
	administradores de Servicios. Si una mascara es dada, solo
	las instancias que concuerden con la mascara son mostradas;
	por ejemplo:
	
	   ADMIN LIST 2-5,7-9
	      Lista instancias en la lista de administradores de
	      Servicios enumeradas 2 a 5 y 7 a 9.
	
	El comando ADMIN CLEAR limpia todas las instancias de
	la lista de administradores de Servicios.
	
	Cualquier operador de IRC puede usar la forma ADMIN LIST
	del comando. Todos los otros usos estan limitados al
	root de Servicios.

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
	Sintaxis: MODE canal modos
	
	Permite a los operadores de Servicios setear modos de canal
	para cualquier canal. Parametros son los mismos que para el
	comando estandart /MODE.
	
	Limitado a operadores de Servicios.

OPER_HELP_UMODE
	Sintaxis: UMODE usuario modos
	
	Permite a Super Admins a setear modos de
	usuarios para cualquier usuario. Los parametros son los
	mismos que para el comando /MODE.

OPER_HELP_OLINE
	Sintaxis: OLINE usuario opciones
	
	Permite a Super Admins a dar permisos de
	operador a cualquier usuario. Las opciones deben estar
	antepuestas con un "+" o un "-". Para remover todas las
	opciones simplemente tipear un "-" en vez de cualquier
	opcion.

OPER_HELP_CLEARMODES
	Sintaxis: CLEARMODES canal [ALL]
	
	Limpia todos los modos binarios (i,k,l,m,n,p,s,t) y
	bans del canal. Si ALL es dado, tambien limpia
	todos los ops y voices (modos +o y +v) del canal.
	
	Limitado a operadores de Servicios.

OPER_HELP_KICK
	Sintaxis: KICK canal usuario razon
	
	Permite a los IRCops a kickear un usuario de cualquier
	canal. Parametros son los mismos que para el comando
	/KICK. El mensaje de kick tendra el nickname del IRCop
	enviando el comando KICK antepuesto; por ejemplo:
	
	*** SpamMan ha sido kickeado del canal #mi_canal por %S (DrStein (Flood))
	
	Limitado a operadores de Servicios.

OPER_HELP_SVSNICK
	Syntax: SVSNICK nick nuevonick
	
	Cambia, a la fuerza, el nick del usuario from nick a nuevonick.
	Limitado a Super Admins.

OPER_HELP_AKILL
	Sintaxis: AKILL ADD [+expiracion] mascara razon
	          AKILL DEL {mascara | numero | lista}
	          AKILL LIST [mascara | lista]
	          AKILL VIEW [mascara | lista]
	          AKILL CLEAR
	
	Permite a los operadores de Servicios manipular la lista de
	AKILLS. Si un usuario concordando con una mascara en la
	lista de AKILLS se intenta conectar, los Servicios enviaran
	un KILL a ese usuario y, en tipos de servidores soportados,
	instruira a todos los servidores a añadir un ban (linea K)
	para la mascara que el usuario concordo con.
	
	AKILL ADD añade la mascara usuario@host dada a la lista
	de AKILLS por la razon dada (la que debe ser dada).
	expiracion es especificada como un entero seguido por uno de
	 d (dias), h (horas), o m (minutos). Combinaciones (como
	1h30m) no estan permitidas. Si un especificador de unidades no
	es incluido, por defecto seran dias (osea +30 por si solo
	significa 30 dias).  Para añadir un AKILL que no expire, usa +0.
	Si la la mascara a ser añadida comienza con un +, un tiempo
	de expiracion debe ser dado, aun si es el mismo que el por
	defecto. El tiempo de expiracion por defecto actual de AKILLS
	puede ser visto con el comando STATS AKILL.
	
	El comando AKILL DEL remueve la mascara dada de la lista
	de AKILLS si esta presente. Si una lista de numeros es dada,
	esas instancias son borradas.  (Ver el ejemplo para LIST 
	mas abajo.)
	
	El comando AKILL LIST muestra la lista de AKILLS.  
	Si una mascara es dada, solo las instancias concordando
	con la mascara son mostradas. Si una lista de numeros es
	dada, solo esas instancias son mostradas; por ejemplo:
	
	   AKILL LIST 2-5,7-9
	      Lista las instancias de AKILL enumeradas 2 a 5 y 7 
	      a 9.
	      
	AKILL VIEW es una version mas verbal de AKILL LIST, y 
	mostrara quien añadio un AKILL, la fecha en que fue añadido,
	y cuando expirara, asi como tambien la mascara usuario@host
	y la razon.
	
	AKILL CLEAR limpia todas las instancias de la lista de
	AKILLS.
	
	Limitado a Operadores de Servicios.
	
OPER_HELP_SGLINE
	Sintaxis: SGLINE ADD [+expiracion] mascara:razon
	          SGLINE DEL {mascara | numero | lista}
	          SGLINE LIST [mascara | lista]
	          SGLINE VIEW [mascara | lista]
	          SGLINE CLEAR
	
	Permite a los operadores de Servicios a manipular la lista
	de SGLINES. Si un usuario con un nombre real concordando
	una mascara SGLINE se intenta conectar, los Servicios le
	impediran conseguir una sesion de IRC.
	
	SGLINE ADD añade la mascara de nombre real dada a la
	lista de SGLINES por la razon dada (la que debe ser
	dada). expiracion es especificada como un entero
	seguido de uno de d (dias), h (horas), o m
	(minutos).  Combinaciones (como 1h30m) no estan
	permitidas. Si la especificacion de la unidad no es
	incluida, por defecto son dias (osea +30 por si solo
	significa 30 dias). Para añadir una SGLINE que no expire,
	usa +0. Si la mascara de nombre real dada empieza con
	+, el tiempo de expiracion debe ser dado, aun si es
	el mismo que el por defecto. El tiempo actual de expiracion
	por defecto para SGLINES puede ser visto con el comando
	STATS AKILL.
	Notese: Ya que la mascara de nombre real puede contener
	espacios, el separador entre esta y la razon es un signo de
	dos puntos.
	
	El comando SGLINE DEL remueve la mascara dada de la
	lista de SGLINES si esta presente. Si una lista de
	instancias es dada, esas instancias son borradas. (Ver el
	ejemplo para LIST mas abajo.)
	
	El comando SGLINE LIST muestra la lista de SGLINES. Si
	una mascara es dada, solo las instancias concordando con
	la mascara son mostradas. Si una lista de instancias es
	dada, solo esas instancias son mostradas; por ejemplo:
	
	   SGLINE LIST 2-5,7-9
	      Lista instancias de SGLINES enumeradas 2 a 5 y 7 
	      a 9.
	      
	SGLINE VIEW es una version mas verbal que SGLINE LIST,
	y mostrara quien añadio la SGLINE, la fecha en la que fue
	añadida, y cuando expira, ademas de la mascara de nombre real
	y la razon.
	
	SGLINE CLEAR limpia todas las instancias de la lista de
	SGLINES.
	
	Limitado a operadores de Servicios.

OPER_HELP_SQLINE
	Sintaxis: SQLINE ADD [+expiracion] mascara razon
	          SQLINE DEL {mascara | numero | lista}
	          SQLINE LIST [mascara | lista]
	          SQLINE VIEW [mascara | lista]
	          SQLINE CLEAR
	
	Permite a los operadores de Servicios a manipular la lista
	de SQLINES. Si un usuario con un nick concordando una
	mascara SQLINE se intenta conectar, los Servicios le
	impediran conseguir una sesion de IRC.
	
	Si el primer caracter de la mascara es #, los servicios
	prevendran el uso de los canales concordantes (en IRCds
	que lo soporten).
	
	SQLINE ADD añade la mascara de nombre real dada a la
	lista de SQLINES por la razon dada (la que debe ser
	dada). expiracion es especificada como un entero
	seguido de uno de d (dias), h (horas), o m
	(minutos).  Combinaciones (como 1h30m) no estan
	permitidas. Si la especificacion de la unidad no es
	incluida, por defecto son dias (osea +30 por si solo
	significa 30 dias). Para añadir una SQLINE que no expire,
	usa +0. Si la mascara de nombre real dada empieza con
	+, el tiempo de expiracion debe ser dado, aun si es
	el mismo que el por defecto. El tiempo actual de expiracion
	por defecto para SQLINES puede ser visto con el comando
	STATS AKILL.
	
	El comando SQLINE DEL remueve la mascara dada de la
	lista de SQLINES si esta presente. Si una lista de
	instancias es dada, esas instancias son borradas. (Ver el
	ejemplo para LIST mas abajo.)
	
	El comando SQLINE LIST muestra la lista de SQLINES. Si
	una mascara es dada, solo las instancias concordando con
	la mascara son mostradas. Si una lista de instancias es
	dada, solo esas instancias son mostradas; por ejemplo:
	
	   SQLINE LIST 2-5,7-9
	      Lista instancias de SQLINES enumeradas 2 a 5 y 7
	      a 9.
	
	SQLINE VIEW es una version mas verbal que SQLINE LIST,
	y mostrara quien añadio la SQLINE, la fecha en la que fue
	añadida, y cuando expira, ademas de la mascara de nombre real
	y la razon.
	
	SQLINE CLEAR limpia todas las instancias de la lista de
	SQLINES.
	
	Limitado a operadores de Servicios.

OPER_HELP_SZLINE
	Sintaxis: SZLINE ADD [+expiracion] mascara razon
	          SZLINE DEL {mascara | numero | lista}
	          SZLINE LIST [mascara | lista]
	          SZLINE VIEW [mascara | lista]
	          SZLINE CLEAR
	
	Permite a los operadores de Servicios manipular la
	lista de SZLINES. Si un usuario con una IP concordando
	una mascara en la lista de SZLINES intenta conectarse,
	los Servicios le impediran obtener una sesion de IRC
	(y esto es, si la IP tiene un PTR RR o no).
	
	SZLINE ADD añade la mascara IP dada a la lista de
	SZLINES por la razon dada (la que debe ser dada).
	expiracion es especificada como un entero seguido
	de uno de d (dias), h (horas), or m (minutos).
	Combinaciones (como 1h30m) no estan permitidas. Si
	una especificacion de unidad no es incluida, por
	defecto es dias (osea +30 por si solo significa
	30 dias). Para añadir una SZLINE que no expire, usa
	+0. Si la mascara a ser añadida empieza con un
	+, el tiempo de expiracion debe ser dado, incluso
	si es el mismo que el por defecto. El tiempo actual
	de expiracion por defecto para SZLINES puede ser
	visto con el comando STATS AKILL.
	
	El comando SZLINE DEL remueve la mascara dada de la
	lista de SZLINES si esta presente. Si una lista de
	instancias es dada, esas instancias son borradas. (Ver
	el ejemplo para LIST mas abajo.)
	
	El comando SZLINE LIST muestra la lista de SZLINES.  
	Si una mascara es dada, solo aquellas instancias
	concordando la mascara son mostradas. Si una lista de
	instancias es dada, solo esas instancias son mostradas;
	por ejemplo:
	
	   SZLINE LIST 2-5,7-9
	      Lista instancias SZLINE enumeradas 2 a 5 y 7 
	      a 9.
	      
	SZLINE VIEW es una version mas verbal de SZLINE LIST,
	y mostrara quien añadio una SZLINE, la fecha en la que
	fue añadida, y cuando expira, ademas de la mascara IP y
	la razon.
	
	SZLINE CLEAR limpia todas las instancias de la lista
	de SZLINES.
	
	Limitado a operadores de Servicios.

OPER_HELP_SET
	Sintaxis: SET opcion seteo
	
	Setea varias opciones globales de Servicios.
	Nombres de opciones actualmente definidas son:
	    READONLY   Setea modo read-only o read-write
	    LOGCHAN    Reporta el log a un canal
	    DEBUG      Activa o desactiva el modo debug
	    NOEXPIRE   Activa o desactiva el modo no expire
	    SUPERADMIN Activa o desactiva el modo SuperAdmin
	    SQL   	 Activate or deactivate sql mode
	    IGNORE     Activate or deactivate ignore mode
	    LIST       List the options
	
	Limitado a administradores de Servicios.

OPER_HELP_SET_READONLY
	Sintaxis: SET READONLY {ON | OFF}
	
	Setea el modo read-only a encencido o apagado. En modo
	read-only, usuarios normales no podran modificar datos
	de Servicios, incluyendo acceso a listas de canales y
	nicknames, etc. IRCops con suficientes privilegios con
	los Servicios podran modificar la lista AKILL de
	Servicios y botar o prohibir nicknames y canales, pero
	ninguno de estos cambios sera salvado a menos que el
	modo read-only es desactivado antes de que los Servicios
	sean terminados o recargados.
	
	Esta opcion es equivalente a la opcion -readonly en
	la linea de comando.

OPER_HELP_SET_LOGCHAN
	Sintaxis: SET LOGCHAN {ON | OFF}

	En modo on, los Servicios envian sus logs a un canal especificado
	y a sus debidos archivos. LogChannel debe estar definido en la
	configuracion de los Servicios para poder usar esta opcion.
	
	Esta opcion es equivalente a la opcion -logchan en
	la linea de comando.
	
	Nota: Es importante que el canal sea apropriadamente 
	seguro con ingreso a Administradores solamente.

OPER_HELP_SET_DEBUG
	Sintaxis: SET DEBUG {ON | OFF | numero}
	
	Setea el modo debug a encendido o apagado. En modo debug,
	toda la informacion enviada de y hacia los Servicios,
	ademas de un numero de mensajes son escritos al archivo
	de log.  Si numero es dado, el modo debug es
	activado, con el nivel de debug setead a numero.
	
	Esta opcion es equivalente a la opcion -debug en
	la linea de comando.

OPER_HELP_SET_NOEXPIRE
	Sintaxis: SET NOEXPIRE {ON | OFF}
	
	Setea el modo no expire a encendido o apagado. En
	modo no expire, nicks, canales, akills y excepciones
	no expiraran hasta que la opcion sea sacada.
	
	Esta opcion es equivalente a la opcion -noexpire
	en la linea de comando.

OPER_HELP_SET_SUPERADMIN
	Sintaxis: SET SUPERADMIN {ON | OFF}

	Este seteo de da privilegios extra como la abilidad de ser
	"fundador" en todos los canales, etc.

	Esta opcion no es persistente y debe ser usada solo de ser
	necesario.

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
	Sintaxis: NOOP SET servidor
	          NOOP REVOKE servidor
	
	NOOP SET remueve todas las lineas O: del
	servidor dado y killea a todos los IRCops actualmente
	en el para prevenirlos a hacer rehash en el servidor
	(ya que esto cancelaria el efecto).
	
	NOOP REVOKE hace las lineas 0: disponibles nuevamente
	en el servidor dado.
	
	Notese: El servidor no es checkeado por los
	Servicios.
	
	Limitado a administradores de Servicios.

OPER_HELP_JUPE
	Sintaxis: JUPE servidor [razon]
	
	Le dice a los Servicios que hagan jupiter a un servidor
	-- esto es, crear un "servidor" falso conectado a
	los Servicios el cual previene al servidor real de
	conectarse. El jupe puede ser removido usando el
	estandart SQUIT. Si una razon es dada, es puesta
	en el campo de informacion sobre el servidor; de otra
	forma, el campo de informacion sobre el servidor tendra
	el texto "Juped by <nick>", mostrando el nickname
	de la persona que jupiterio el servidor.
	
	Limitado a administradores de Servicios.

OPER_HELP_RAW
	Sintaxis: RAW texto
	
	Envia una linea de texto directamente al servidor al cual
	los Servicios estan conectados. Este comando tiene un
	uso muy limitado y puede trastornar a una red si es
	usado de manera impropia. NO USES ESTE COMANDO a
	menos que estes absolutamente seguro de lo que estas
	haciendo!
	
	Limitado a administradores de Servicios.

OPER_HELP_UPDATE
	Sintaxis: UPDATE
	
	Causa a los Servicios a updatear todos los archivos de
	la base de datos tan pronto como envies el comando.
	
	Limitado a administradores de Servicios.

OPER_HELP_RELOAD
	Sintaxis: RELOAD
	
	Causa a los Servicios a recargar el archivo de
	configuracion. Notese que algunas directivas aun asi
	necesitan que los Servicios partan denuevo para tomar
	efecto (tal como nicknames de los Servicios, activacion
	de la limitacion de sesiones, etc.)
	
	Limitado a administradores de Servicios.

OPER_HELP_QUIT
	Sintaxis: QUIT
	
	Causa a los Servicios a parar inmediatamente; las bases de
	datos no son salvadas. Este comando no debe ser usado a
	menos que las copias en la memoria se crean dañadas y no
	deban ser guardadas. Para parar normalmente, usa el comando
	SHUTDOWN.
	
	Limitado a administradores de Servicios.

OPER_HELP_SHUTDOWN
	Sintaxis: SHUTDOWN
	
	Causa a los Servicios a guardar todas las bases de
	datos y parar.
	
	Limitado a administradores de Servicios.

OPER_HELP_RESTART
	Sintaxis: RESTART
	
	Causa a los Servicios a guardar todas las bases de
	datos y luego parar y partir nuevamente.
	(esto es, terminar el programa e inmediatamente correr
	el ejecutable nuevamente).
	
	Limitado a administradores de Servicios.

OPER_HELP_CHANLIST
	Sintaxis: CHANLIST [{patron | nick} [SECRET]]
	
	Lista todos los canales actualmente en uso en la red IRC,
	esten registrados o no.
	
	Si patron es dado, lista solo canales que concuerden con el. Si
	un nickname es dado, lista solo los canales en los que el usuario
	este. Si SECRET es especificado, lista solo los canales que
	concuerden con patron y que tengan modos +s o +p.
	
	Limitado a administradores de Servicios.

OPER_HELP_USERLIST
	Sintaxis: USERLIST [{patron | canal} [INVISIBLE]]
	
	Lista todos los usuarios actualmente en linea en la red IRC,
	esten registrados o no.
	
	Si patron es dado, lista solo a los usuarios que concuerden
	con el (debe ser en el formato nick!usuario@host). si canal
	es dado, lista solo usuarios que esten en el canal dado. Si
	INVISIBLE especificado, solo usuarios con la bandera +i seran
	listados.
	
	Limitado a administradores de Servicios.

OPER_HELP_MODLOAD
	Sintaxis: MODLOAD NombreArchivo

	Este comando carga el modulo llamado NombreArchivo del directorio de 
	modulos.

	Limitado a Services Roots.

OPER_HELP_MODUNLOAD
	Sintaxis: MODUNLOAD NombreArchivo

	Este comando descarga el modulo llamado NombreArchivo del directorio 
	de modulos.

	Limitado a Services Roots.

OPER_HELP_MODINFO
	Sintaxis: MODINFO NombreArchivo

	Este comando lista informacion sobre el modulo cargado pedido 

	Limitado a Services Roots.

OPER_HELP_MODLIST
	Sintaxis: MODLIST [Core|3rd|protocol|encryption|supported|qatested]

	Lista todos los modulos actualmente cargados.

	Limitado a Services Roots.


###########################################################################
#
# BotServ help messages
#
###########################################################################

BOT_HELP_CMD_BOTLIST
	BOTLIST            Lista bots disponibles
BOT_HELP_CMD_ASSIGN
	ASSIGN             Asigna un bot a un canal
BOT_HELP_CMD_SET
	SET                Configura opciones de bots
BOT_HELP_CMD_KICK
	KICK               Configura kickers
BOT_HELP_CMD_BADWORDS
	BADWORDS           Mantiene lista de bad words
BOT_HELP_CMD_ACT
	ACT                Makes the bot do the equivalent of a "/me" command
BOT_HELP_CMD_INFO
	INFO               Allows you to see BotServ information about a channel or a bot
BOT_HELP_CMD_SAY
	SAY                Makes the bot say the given text on the given channel
BOT_HELP_CMD_UNASSIGN
	UNASSIGN           Unassigns a bot from a channel
BOT_HELP_CMD_BOT
	BOT                Mantiene la lista de bots de la red

BOT_HELP
	%S te permite crear un bot en tu propio canal.
	Esto ha sido creado para usuarios que no puedan
	hostear o configurar un bot, o para uso en redes
	que no permitan bots de usuarios. Comandos
	disponibles estan listados mas abajo; para
	usarlos, tipea %R%S comando. Para mayor
	informacion sobre un comando especifico, tipea  
	%R%S HELP comando.
	
BOT_HELP_FOOTER
	El bot entrara a un canal cuando quiera que hayan
	%d usuario(s) en el.
	
BOT_HELP_BOTLIST
	Sintaxis: BOTLIST
	
	Lista todos los bots disponibles en esta red.
	
BOT_HELP_ASSIGN
	Sintaxis: ASSIGN canal nick
	
	Assigna un bot denominado por nick al canal canal.
	Luego de esto puedes configurar al bot para acomodarlo
	a tus necesidades.

BOT_HELP_UNASSIGN
	Sintaxis: UNASSIGN canal
	
	Deasigna a un bot de un canal. Al usar este comando
	el bot ya no entrara al canal. Aun asi, la configuracion
	del bot para el canal es guardada, asi te sera posible
	reasignar un bot luego sin tener que configurarlo
	completamente.

BOT_HELP_INFO
	Sintaxis: INFO {canal | nick}
	
	Permite ver la informacion de %S acerca de un canal o
	bot. Si el parametro es un canal, recibiras informacion
	tal como kickers activados. Si el parametro es un nick,
	recibiras informacion acerca de un bot, tal como tiempo
	de creacion o numero de canales en los que este.

BOT_HELP_SET
	Sintaxis: SET canal opcion parametros
	
	Configura opciones de bot.  opcion puede ser uno de:
	
	    DONTKICKOPS      Para proteger ops contra kicks de bots
	    DONTKICKVOICES   Para proteger voices contra kicks de bots
	    GREET            Abilita mensajes de bienvenida
	    FANTASY          Abilita comandos de fantasia
	    SYMBIOSIS        Permite al bot a actuar como un bot real
	
	Tipea %R%S HELP SET opcion para mayor informacion
	sobre una opcion especifica.
	
	Notese: el acceso a este comando esta controlado por
	el nivel SET.
	
BOT_HELP_SET_DONTKICKOPS
	Sintaxis: SET canal DONTKICKOPS {ON|OFF}
	
	Habilita o deshabilita el modo de proteccion de ops
	en un canal. Al abilitarlo, ops no seran
	kickeados por el bot, incluso si no concuerdan con
	el nivel NOKICK.

BOT_HELP_SET_DONTKICKVOICES
	Sintaxis: SET canal DONTKICKVOICES {ON|OFF}
	
	Habilita o deshabilita el modo de proteccion de voices
	en un canal. Al abilitarlo, voices no seran
	kickeados por el bot, incluso si no concuerdan con
	el nivel NOKICK.

BOT_HELP_SET_FANTASY
	Sintaxis: SET canal FANTASY {ON|OFF}
	
	Habilita o deshabilita el modo de fantasia
	en un canal. Al abilitarlo, los usuarios podran
	usar los comandos !op, !deop, !voice, !devoice,
	!kick, !kb, !unban, !seen en un canal (averigua
	como usarlos; intenta con o sin nick para cada
	uno, y con razon para algunos?).
	
	Notese que usuarios que quieran usar comandos
	de fantasia DEBEN tener el nivel necesario
	para ambos el nivel FANTASIA y otro dependiendo
	del comando si es requerido (por ejemplo, para
	usar !op, el usuario debe tener suficiente
	acceso para el nivel OPDEOP).

BOT_HELP_SET_GREET
	Sintaxis: SET canal GREET {ON|OFF}
	
	Habilita o deshabilita el modo de asludo en
	un canal. Al abilitarlo, el bot mostrara los
	mensajes de los usuarios entrando al canal,
	siempre y cuando tengan el suficiente acceso
	al canal.

BOT_HELP_SET_SYMBIOSIS
	Sintaxis: SET canal SYMBIOSIS {ON|OFF}
	
	Habilita o deshabilita el modo simbiosis en
	un canal. Al abilitarlo, el bot hara cualquier
	cosa que normalmente habria hecho %s en los
	canales, tal como MODOS, KICKS e incluso los
	mensajes de entrada.

BOT_HELP_KICK
	Sintaxis: KICK canal opcion parametros
	
	Configura los kickers de bot. opcion puede ser una de:
	
	    BOLDS         Setea si el bot kickea negritas
	    BADWORDS      Setea si el bot kickea bad words
	    CAPS          Setea si el bot kickea mayusculas
	    COLORS        Setea si el bot kickea colores
	    FLOOD         Setea si el bot kickea usuarios
	                       que floodean
	    REPEAT        Setea si el bot kickea usuarios
	                       que repiten
	    REVERSES      Setea si el bot kickea reversos
	    UNDERLINES    Setea si el bot kickea subrayados
	
	Tipea %R%S HELP KICK opcion para mayor
	informacion sobre una opcion especifica.
	
	Notese: acceso a este comando es controlado por el
	nivel SET.

BOT_HELP_KICK_BOLDS
	Sintaxis: KICK canal BOLDS {ON|OFF} [ttb]

	Setea el kick por negritas a encendido o apagado.
	Al abilitarlo, esta opcion le dice al bot que kickee
	usuarios que usen negritas.
	
	ttb es el numero de veces que un usuario sera kickeado
	antes de ser baneado. No des ttb para desabilitar
	el sistema de ban una vez activado.

BOT_HELP_KICK_COLORS
	Sintaxis: KICK canal COLORS {ON|OFF} [ttb]
	
	Setea el kick por colores a encendido o apagado.
	Al habilitarlo, esta opcion le dice al bot que kickee
	usuarios que usen colores.
	
	ttb es el numero de veces que un usuario sera kickeado
	antes de ser baneado. No des ttb para desabilitar
	el sistema de ban una vez activado.

BOT_HELP_KICK_REVERSES
	Sintaxis: KICK canal REVERSES {ON|OFF} [ttb]
	
	Setea el kick por reversos a encendido o apagado.
	Al habilitarlo, esta opcion le dice al bot que kickee
	usuarios que usen reversos.
	
	ttb es el numero de veces que un usuario sera kickeado
	antes de ser baneado. No des ttb para desabilitar
	el sistema de ban una vez activado.

BOT_HELP_KICK_UNDERLINES
	Sintaxis: KICK canal UNDERLINES {ON|OFF} [ttb]
	
	Setea el kick por subrayados a encendido o apagado.
	Al habilitarlo, esta opcion le dice al bot que kickee
	usuarios que usen subrayados.
	
	ttb es el numero de veces que un usuario sera kickeado
	antes de ser baneado. No des ttb para desabilitar
	el sistema de ban una vez activado.

BOT_HELP_KICK_CAPS
	Sintaxis: KICK canal CAPS {ON|OFF} [ttb [min [porcentaje]]]

	Setea el kick por mayusculas a encendido o apagado.
	Al habilitarlo, esta opcion le dice al bot que kickee
	usuarios que esten hablando en mayusculas.
	
	El bot kickea solo si hay al menos min mayusculas
	y que constituyan al menos porcentaje%% del total
	del texto (si no es dado, por defecto es seteado a 10
	caracteres y 25%%).
	
	ttb es el numero de veces que un usuario sera kickeado
	antes de ser baneado. No des ttb para desabilitar
	el sistema de ban una vez activado.

BOT_HELP_KICK_FLOOD
	Sintaxis: KICK canal FLOOD {ON|OFF} [ttb [ln [segs]]]

	Setea el kick por flood a encendido o apagado.
	Al habilitarlo, esta opcion le dice al bot que kickee
	usuarios que esten floodeando el canal usando al menos
	ln lineas en segs segundos (si no es dado, por
	defecto es seteado a 6 lineas en 10 segundos).
	
	ttb es el numero de veces que un usuario sera kickeado
	antes de ser baneado. No des ttb para desabilitar
	el sistema de ban una vez activado.

BOT_HELP_KICK_REPEAT
	Sintaxis: KICK #canal REPEAT {ON|OFF} [ttb [num]]
	
	Setea el the kick por repeticion a encendido o apagado.
	Al habilitarlo, esta opcion le dice al bot que kickee
	usuarios que esten repitiendo num veces (si num no
	es dado, por defecto es seteado a 3).
	
	ttb es el numero de veces que un usuario sera kickeado
	antes de ser baneado. No des ttb para desabilitar
	el sistema de ban una vez activado.

BOT_HELP_KICK_BADWORDS
	Sintaxis: KICK #canal BADWORDS {ON|OFF} [ttb]
	
	Setea el kick por bad words a encendido o apagado.
	Al habilitarlo, esta opcion le dice al bot que kickee
	usuarios que digan ciertas palabras en el canal.
	
	Puedes definir bad words para tu canal usando el comando
	BADWORDS. Tipea %R%S HELP BADWORDS para mayor
	informacion.
	
	ttb es el numero de veces que un usuario sera kickeado
	antes de ser baneado. No des ttb para desabilitar
	el sistema de ban una vez activado.

BOT_HELP_BADWORDS
	Sintaxis: BADWORDS canal ADD palabra [SINGLE | START | END]
	          BADWORDS canal DEL {palabra | numero | lista}
	          BADWORDS canal LIST [mascara | lista]
	          BADWORDS canal CLEAR
	
	Mantiene la lista de bad words para un canal. La lista
	de bad words determina cuales palabtas deben ser kickeadas
	cuando el kick por bad words esta habilitado. Para mayor
	informacion, tipea %R%S HELP KICK BADWORDS.
	
	El comando BADWORDS ADD añade una palabra a la lista
	de bad words. Si SINGLE es especificado, el kick sera
	hecho solo si el usuario dice la palabra completa. Si
	START es especificado, el kick sera hecho solo si el
	usuario dice una palabra que comience con palabra.
	Si END es especificado, el kick sera hecho solo si el
	usuario dice una palabra que termine con palabra. Si
	no especificas nada, un kick sera hecho cada vez que
	palabra sea dicha por un usuario.
	
	El comando BADWORDS DEL remueve la palabra dada de la
	lista de bad words. Si una lista de numeros es dada, esas
	instancias son borradas. (ver el ejemplo para LIST mas
	abajo).
	
	El comando BADWORDS LIST muestra la lista de bad
	words. Si una mascara es dada, solo las instancias
	concordando con la mascara son mostradas. Si una lista
	de numeros es dada, solo esas instancias son mostradas;
	por ejemplo:
	
	   BADWORDS #canal LIST 2-5,7-9
	      Lista bad words enumeradas 2 a 5 y 7 a 9.
	      
	El comando BADWORDS CLEAR limpia todas las instancias
	de la lista de bad words.

BOT_HELP_SAY
	Sintaxis: SAY canal texto
	
	Hace al bot decir el texto dado en el canal dado.

BOT_HELP_ACT
	Sintaxis: ACT canal texto
	
	Hace al bot hacer el equivalente del comando "/me"
	en el canal dado usando el texto dado.

BOT_SERVADMIN_HELP_BOT
	Sintaxis: BOT ADD nick usuario host real
	          BOT CHANGE nick-ant nick-nue [usuario [host [real]]]
	          BOT DEL nick
	
	Permite a los administradores de Servicios crear, modificar
	y borrar bots que usuarios podran usar en sus propios
	canales.
	
	BOT ADD añade un bot con el nickname, usuario, host
	y nombre real dados. Ya que no se hacen chequeos de
	integridad para estos seteos, ten mucho cuidado.
	BOT CHANGE permite cambiar el nickname, usuario, host
	o nombre real de un bot sin tener que borrarlo (y la
	informacion asociada a el).
	BOT DEL remueve el bot dado de la lista de bots.
	
	Nota:: no puedes crear un bot que tenga un nick que ya
	este registrado. Si un usuario no registrado esta usando 
	el nick, sera killeado.	

BOT_SERVADMIN_HELP_SET
	
	Estas opciones estan reservadas a administradores de
	Servicios:
	
	    NOBOT            Previene a un bot de ser asignado
	                        a un canal
	    PRIVATE          Previene a un bot de ser asignado
	                        por alguien mas que un operador
	                        de IRC

BOT_SERVADMIN_HELP_SET_NOBOT
	Sintaxis: SET canal NOBOT {ON|OFF}
	
	Esta opcion hace al canal no asignable. Si un
	bot ya esta asignado al canal, es desasignado
	automaticamente cuando abilitas esta opcion.
	
	Limitado a administradores de Servicios.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Sintaxis: SET nick-del-bot PRIVATE {ON|OFF}
	
	Esta opcion previene un bot de ser asignado
	a un canal por usuarios que no son operadores
	de IRC.
	
	Limitado a administradores de Servicios.
HOST_EMPTY
	La lista vhost esta vacia.
HOST_ENTRY
	#%d Nick:%s, vhost:%s (%s - %s)
HOST_IDENT_ENTRY
	#%d Nick:%s, vhost:%s@%s (%s - %s)
HOST_SET
	vhost para %s seteado a %s.
HOST_IDENT_SET
	vhost para %s seteado a %s@%s.
HOST_SETALL
	vhost para el grupo %s seteado a %s.
HOST_DELALL
	El vhost para el group %s ha sido removido.
HOST_DELALL_SYNTAX
	DELALL <nick>.
HOST_IDENT_SETALL
	vhost para el grupo %s seteado a %s@%s.
HOST_SET_ERROR
	Error! Un vhost solo puede contener A-Z, a-z, 0-9, '.' y '-'.
HOST_SET_IDENT_ERROR
	Un vhost ident debe tener el formato de un ident valido
HOST_SET_TOOLONG
	Error! El vhost es muy largo. Usa un host con menos de %d caracteres.
HOST_SET_IDENTTOOLONG
	Error! El Ident es muy largo. Usa un ident con menos de %d caracteres.
HOST_NOREG
	Usuario %s no encontrado en la base de datos de nickserv.
HOST_SET_SYNTAX
	SET <nick> <hostmask>.
HOST_SETALL_SYNTAX
	SETALL <nick> <hostmask>.
HOST_DENIED
	Acceso Denegado.
HOST_NOT_ASSIGNED
	Por favor contactar a un Operador para que asigne un vhost a este nick.
HOST_ACTIVATED
	Tu vhost %s ha sido activado.
HOST_IDENT_ACTIVATED
	Tu vhost %s@%s ha sido activado.
HOST_ID
	Por favor identificate con los servicios primero.
HOST_NOT_REGED
	Necesitas registrarte antes de que un vhost sea asignado a ti.
HOST_DEL
	vhost para %s removido.
HOST_DEL_SYNTAX
	DEL <nick>.

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_OFF
	Your vhost was removed and the normal cloaking restored.
HOST_OFF_UNREAL
	Tu vhost ha sido removido. Para habilitar nuevamente el enmascaramiento de hosts tipea /mode %s +%s
HOST_NO_VIDENT
	Tu IRCD no soporta vIdent's. Si esto es incorrecto, por favor reportalo como un posible bug.

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_GROUP
	Todos los vhosts en el grupo %s han sido seteados a %s
HOST_IDENT_GROUP
	Todos los vhosts en el grupo %s han sido seteados a %s@%s

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_LIST_FOOTER
	Todos los registros mostrados (Cuenta: %d)
HOST_LIST_RANGE_FOOTER
	Todos los registros mostrados de %d a %d
HOST_LIST_KEY_FOOTER
	Todos los registro mostrados que coinciden con la clave %s (Cuenta: %d)

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_HELP_CMD_ON
	    ON          Activa tu vhost asignado.

HOST_HELP_CMD_OFF
	    OFF         Desactiva tu vhost asignado.

HOST_HELP_CMD_GROUP
	    GROUP	Sincroniza vhost en todos los nicks del grupo.

HOST_HELP_CMD_SET
	    SET         Setea el vhost de otro usuario.
HOST_HELP_CMD_SETALL
	    SETALL      Setea el vhost de todos los nicks en un grupo.
HOST_HELP_CMD_DEL
	    DEL         Borra el vhost de otro usuario.
HOST_HELP_CMD_DELALL
	    DELALL      Borra el vhost de todos los nicks en un grupo.
HOST_HELP_CMD_LIST
	    LIST        Muestra uno o mas vhosts.

HOST_OPER_HELP
	not used.

HOST_ADMIN_HELP
	not used.

HOST_HELP
	Comandos para %S:

HOST_HELP_ON
	Sintaxis: ON

	Activa el vhost actualmente asignado al nick en uso.
	Cuando uses este comando cualquier usuario que haga un /whois
	a ti vera el vhost asignado en vez de tu IP real.

HOST_HELP_SET
	Sintaxis: SET <nick> <host>.

	Setea el vhost para el nick dado al host dado. Si
	tu IRCD soporta vIdents, al usar SET <nick> <ident>@<host>
	setea idents para usuarios ademas de vhosts.

	Limitado a operadores de Servicios.

HOST_HELP_DELALL
	Sintaxis: DELALL <nick>.

	Elimina el vhost para todos los nicks en el mismo grupo que el
	nick dado.

	Limitado a Host Removers.


HOST_HELP_SETALL
	Sintaxis: SETALL <nick> <host>.

	Setea el vhost para todos los nicks en el mismo grupo que
	el nick dado.  Si tu IRCD soporta vIdents, al usar
	SETALL <nick> <ident>@<host> setea idents para usuarios
	ademas de vhosts.

	* NOTA: Esto no actualizara el vhost para ningun nick
	agregado al grupo despues que este comando haya sido usado.

	Limitado a Operadores de Servicios.

HOST_HELP_OFF
	Sintaxis: OFF

	Desactiva el vhost actualmente asignado al nick en uso.
	Cuando uses este comando cualquier usuario que haga un /whois
	a ti vera tu IP real.

HOST_HELP_DEL
	Sintaxis: DEL <nick>

	Borra el vhost asignado al nick dado de la base de datos.

	Limitado a Operadores de Servicios.

HOST_HELP_LIST
	Sintaxis: LIST [<clave>|<#X-Y>]

	Este comando lista los vhosts registrados a el operador
	si una Clave es dada, solo las entradas en las que el nick 
	o el vhost coincidan con el patron dado en <clave> seran mostradas
	ej. DrStein* para todas las entradas que comiencen con "DrStein"

	Si un #X-Y estilo es usado, solo las entradas en el rango entre X
	e Y seran mostradas, ej. #1-3 va a mostrar las primeras 3 entradas
	de nicks/vhosts.

	La lista usa el valor de NSListMax como un limite maximo de numeros
	de items a mostrar al operador 1 vez.

	Limitado a Operadores de Servicios.

HOST_HELP_GROUP
	Sintaxis: GROUP
	
	Este comando permite a usuarios a setear el vhost de su
	nick actual para ser el vhost para todos los nicks en el
	mismo grupo.

OPER_SVSNICK_UNSUPPORTED
	Lo siento, SVSNICK no esta disponible en esta red.

OPER_SQLINE_UNSUPPORTED
	Lo siento, SQLINE no esta disponible en esta red.

OPER_SVSO_UNSUPPORTED
	Lo siento, OLINE no esta disponible en esta red.

OPER_UMODE_UNSUPPORTED
	Lo siento, OLINE no esta disponible en esta red.

OPER_SUPER_ADMIN_NOT_ENABLED
	SuperAdmin setting not enabled in services.conf

