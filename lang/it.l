# Italian language file.
#
# (C) 2003-2005 Anope Team
# Contact us at info@anope.org
#
# Please read COPYING and README for furhter details.
#
# Based on the original code of Epona by Lara.
# Based on the original code of Services by Andy Church.
#
# Translated by Daniele Nicolucci <jollino@discussioni.org>
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
	Italiano (Italiano)

###########################################################################
#
# General messages
#
###########################################################################

# strftime() format strings.  man 3 strftime for information on the
# meanings of the format specifiers.  Short version:
#	%a = weekday name (short)	%H = hour
#	%b = month name (short)	    %M = minute
#	%d = day of month	    %S = second
#	%Y = year	    	%Z = time zone

# This is used as the format string for strftime() for a date and time
# together.
STRFTIME_DATE_TIME_FORMAT
	%a %d %b %Y %Z - %H:%M:%S
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
	Mer
	Gio
	Ven
	Sab
# %A
STRFTIME_DAYS_LONG
	Domenica
	Lunedì
	Martedì
	Mercoledì
	Giovedì
	Venerdì
	Sabato
# %b
STRFTIME_MONTHS_SHORT
	Gen
	Feb
	Mar
	Apr
	Mag
	Giu
	Lug
	Ago
	Set
	Ott
	Nov
	Dic
# %B
STRFTIME_MONTHS_LONG
	Gennaio
	Febbraio
	Marzo
	Aprile
	Maggio
	Giugno
	Luglio
	Agosto
	Settembre
	Ottobre
	Novembre
	Dicembre


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	, 


# Various error messages.
USER_RECORD_NOT_FOUND
	Errore interno - Impossibile elaborare la richiesta.
UNKNOWN_COMMAND
	Comando sconosciuto %s.
UNKNOWN_COMMAND_HELP
	Comando sconosciuto %s.  "/msg %s HELP" per l'aiuto.
SYNTAX_ERROR
	Sintassi: %s
MORE_INFO
	/msg %s HELP %s per altre informazioni.
NO_HELP_AVAILABLE
	Nessun aiuto disponibile per %s.
OBSOLETE_COMMAND
	Questo comando è obsoleto; usa %s invece.

BAD_USERHOST_MASK
	La maschera dev'essere nel formato user@host.
BAD_EXPIRY_TIME
	Tempo di scadenza non valido.
USERHOST_MASK_TOO_WIDE
	%s La copertura è troppo ampia; per favore, usa una maschera più specifica.

SERVICE_OFFLINE
	%s al momento è offline.
READ_ONLY_MODE
	Nota: i Services sono in modalità read-only; le modifiche non saranno salvate!
PASSWORD_INCORRECT
	Password errata.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Accesso negato.
PERMISSION_DENIED
	Permesso negato.
RAW_DISABLED
	L'opzione RAW è stata disabilitata. Se devi usarla, attiva la direttiva DisableRaw nella configurazione dei Services.

MORE_OBSCURE_PASSWORD
	Per favore, prova di nuovo con una password più oscura. Le password devono essere lunghe almeno 5 caratteri, non devono essere indovinate facilmente (ad es. il proprio nome o nick), e non possono contenere i caratteri di spazio e di tabulazione.
PASSWORD_TRUNCATED
	Nota: La tua password è stata troncata a %d caratteri.

NICK_NOT_REGISTERED
	Il tuo nick non è registrato.
NICK_NOT_REGISTERED_HELP
	Il tuo nick non è registrato. Digita /msg %s HELP per informazioni sulla registrazione del nick.
NICK_X_IS_SERVICES
	Nick %s is part of this Network's Services.
NICK_X_NOT_REGISTERED
	Il nick %s non è registrato.
NICK_X_IN_USE
	Nick %s is currently in use.
NICK_X_NOT_IN_USE
	Il nick %s non è attualmente in uso.
NICK_X_NOT_ON_CHAN
	%s al momento non è nel canale %s.
NICK_X_FORBIDDEN
	Il nick %s non può essere registrato o usato.
NICK_X_FORBIDDEN_OPER
	Il nick %s è stato vietato da %s:
	%s
NICK_X_ILLEGAL
	Nick %s is an illegal nickname and cannot be used.
NICK_X_TRUNCATED
	Nick %s was truncated to %d characters.
CHAN_X_NOT_REGISTERED
	Il canale %s non è registrato.
CHAN_X_NOT_IN_USE
	Il canale %s non esiste.
CHAN_X_FORBIDDEN
	Il canale %s non può essere registrato o usato.
CHAN_X_FORBIDDEN_OPER
	Il canale %s è stato vietato da %s:
	%s
CHAN_X_SUSPENDED
	      Sospeso: [%s] %s
NICK_IDENTIFY_REQUIRED
	Autentificazione richiesta per questo comando.
	Riprova dopo aver digitato /msg %s IDENTIFY password.
CHAN_IDENTIFY_REQUIRED
	Autentificazione richiesta per questo comando.
	Riprova dopo aver digitato /msg %s IDENTIFY %s password.
MAIL_DISABLED
	I servizi sono stati configurati per non inviare e-mail.
MAIL_INVALID
	L'e-mail per %s non è valida.
MAIL_X_INVALID
	%s non è un indirizzo e-mail valido.
MAIL_LATER
	Impossibile inviare e-mail adesso; riprova più tardi.
MAIL_DELAYED
	Attendi %d secondi è riprova.
NO_REASON
	Nessun motivo
UNKNOWN
	<sconosciuto>

# Duration system
DURATION_DAY
	1 giorno
DURATION_DAYS
	%d giorni
DURATION_HOUR
	1 ora
DURATION_HOURS
	%d ore
DURATION_MINUTE
	1 minuto
DURATION_MINUTES
	%d minuti
DURATION_SECOND
	1 secondo
DURATION_SECONDS
	%d secondi

# Human readable expiration
NO_EXPIRE
	non scade
EXPIRES_SOON
	scade con il prossimo aggiornamento dei database
EXPIRES_M
	scade tra %d minuti
EXPIRES_1M
	scade tra %d minuto
EXPIRES_HM
	scade tra %d ore, %d minuti
EXPIRES_H1M
	scade tra %d ore, %d minuto
EXPIRES_1HM
	scade tra %d ora, %d minuti
EXPIRES_1H1M
	scade tra %d ora, %d minuto
EXPIRES_D
	scade tra %d giorni
EXPIRES_1D
	scade tra %d giorno

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
	Questo nick appartiene a qualcun altro. Per favore scegline un altro.
	(Se questo è il tuo nick, digita /msg %s IDENTIFY password.)
NICK_IS_SECURE
	Questo nick è registrato e protetto. Se questo è il tuo
	nick, digita /msg %s IDENTIFY password. Altrimenti,
	scegli un nick diverso.
NICK_MAY_NOT_BE_USED
	Questo nick non può essere usato. Per favore scegline un altro.
FORCENICKCHANGE_IN_1_MINUTE
	Se non cambi il tuo nick entro un minuto, verrà cambiato dal server.
FORCENICKCHANGE_IN_20_SECONDS
	Se non cambi il tuo nick entro 20 secondi, verrà cambiato dal server.
FORCENICKCHANGE_NOW
	Questo nick è stato registrato; non puoi usarlo.
FORCENICKCHANGE_CHANGING
	Il tuo nick sarà cambiato a %s

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER password [email]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER password email
NICK_REGISTRATION_DISABLED
	Spiacente, la registrazione dei nick è temporaneamente disabilitata.
NICK_REGISTRATION_FAILED
	Spiacente, non è andata a buon fine.
NICK_REG_PLEASE_WAIT
	Per favore attendi %d secondi prima di usare di nuovo il comando REGISTER.
NICK_CANNOT_BE_REGISTERED
	Il nick %s non può essere registrato.
NICK_ALREADY_REGISTERED
	Il nick %s è già registrato!
NICK_REGISTERED
	Il nick %s è stato registrato sotto il tuo account: %s
NICK_REGISTERED_NO_MASK
	Nickname %s registered.
NICK_PASSWORD_IS
	La tua password è %s - non dimenticarla!

# GROUP responses
NICK_REG_DELAY
	You have to be connected longer than %d seconds to register your nick.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP destinatario password
NICK_GROUP_DISABLED
	Spiacente, il raggruppamento dei nick è temporaneamente disabilitato.
NICK_GROUP_FAILED
	Spiacente, il raggruppamento non è andato a buon fine.
NICK_GROUP_PLEASE_WAIT
	Per favore attendi %d secondi prima di usare di nuovo il comando GROUP.
NICK_GROUP_CHANGE_DISABLED
	Il tuo nick è già registrato; digita prima /msg %s DROP.
NICK_GROUP_SAME
	Sei già un membro del gruppo di %s.
NICK_GROUP_TOO_MANY
	Ci sono troppi nick nel gruppo di %s; controlla la lista e cancellane
	alcuni. Digita /msg %s HELP GLIST e /msg %s HELP DROP 
	per avere altre informazioni.
NICK_GROUP_JOINED
	Adesso sei nel gruppo di %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY password
NICK_IDENTIFY_FAILED
	Spiacente, l'identificazione non è andata a buon fine.
NICK_IDENTIFY_SUCCEEDED
	Password accettata - adesso sei riconosciuto.
NICK_IDENTIFY_EMAIL_REQUIRED
	Adesso devi impostare un indirizzo e-mail per il tuo nick.
	Questa e-mail ti permetterà di recuperare la tua password
	nel caso tu la dimentichi.
NICK_IDENTIFY_EMAIL_HOWTO
	Digita /msg %S SET EMAIL e-mail per inserire la tua e-mail.
	La tua privacy è rispettata; questa e-mail non sarà data a
	terzi.

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
	Non sei più identificato per il tuo nick.
NICK_LOGOUT_X_SUCCEEDED
	Il nick %s è stato deidentificato.

# DROP responses
NICK_LOGOUT_SERVICESADMIN
	Can't logout %s because he's a services administrator.

# DROP responses
NICK_DROP_DISABLED
	Spiacente, la deregistrazione dei nick è temporaneamente disabilitata.
NICK_DROPPED
	Il tuo nick è stato deregistrato.
NICK_X_DROPPED
	Il nick %s è stato deregistrato.

# SET responses
NICK_SET_SYNTAX
	SET opzione parametri
NICK_SET_SERVADMIN_SYNTAX
	SET [nick] opzione parametri
NICK_SET_DISABLED
	Spiacente, l'impostazione delle opzioni dei nick è temporaneamente disabilitata.
NICK_SET_UNKNOWN_OPTION
	Opzione SET %s sconosciuta.
NICK_SET_UNKNOWN_OPTION_OR_BAD_NICK
	Opzione SET %s sconosciuta, o nick non registrato.
NICK_SET_OPTION_DISABLED
	L'opzione %s non può essere impostata su questa rete.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	Il nuovo display DEVE essere un nick nel tuo gruppo di nick!
NICK_SET_DISPLAY_CHANGED
	Il nuovo display adesso è %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Spiacente, il cambio di password non è andato a buon fine.
NICK_SET_PASSWORD_CHANGED
	La password è stata cambiata.
NICK_SET_PASSWORD_CHANGED_TO
	La password è stata impostata a %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE numero
NICK_SET_LANGUAGE_UNKNOWN
	Numero sconosciuto di lingua %d.
	Digita /msg %s HELP SET LANGUAGE per la lista delle lingue.
NICK_SET_LANGUAGE_CHANGED
	Lingua cambiata a Italiano.

# SET URL responses
NICK_SET_URL_CHANGED
	URL cambiato a %s.
NICK_SET_URL_UNSET
	Impostazione dell'URL annullata.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	Indirizzo e-mail cambiato a %s.
NICK_SET_EMAIL_UNSET
	Impostazione dell'indirizzo e-mail annullata.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	Non puoi annullare l'impostazione dell'e-mail.

# SET ICQ responses
NICK_SET_ICQ_CHANGED
	Numero ICQ impostato a %s.
NICK_SET_ICQ_UNSET
	Impostazione del numero ICQ annullata.
NICK_SET_ICQ_INVALID
	%s non è un numero valido.

# SET GREET responses
NICK_SET_GREET_CHANGED
	Messaggio di benvenuto impostato a %s.
NICK_SET_GREET_UNSET
	Messaggio di benvenuto annullato.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	La protezione ora è ATTIVA.
NICK_SET_KILL_QUICK
	La protezione ora è ATTIVA, con un ritardo ridotto.
NICK_SET_KILL_IMMED
	La protezione ora è ATTIVA, senza ritardo.
NICK_SET_KILL_IMMED_DISABLED
	L'opzione IMMED non è disponibile su questa rete.
NICK_SET_KILL_OFF
	La protezione ora è DISATTIVA.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	L'opzione Secure ora è ATTIVA.
NICK_SET_SECURE_OFF
	L'opzione Secure ora è DISATTIVA.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	L'opzione Private ora è ATTIVA.
NICK_SET_PRIVATE_OFF
	L'opzione Private ora è DISATTIVA.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Il tuo indirizzo e-mail sarà nascosto dai messaggi INFO di %s.
NICK_SET_HIDE_EMAIL_OFF
	Il tuo indirizzo e-mail sarà visualizzato nei messaggi INFO di %s.
NICK_SET_HIDE_MASK_ON
	La tua ultima hostmask sarà nascosta dai messaggi INFO di %s.
NICK_SET_HIDE_MASK_OFF
	La tua ultima hostmask sarà visualizzata nei messaggi INFO di %s.
NICK_SET_HIDE_QUIT_ON
	Il tuo ultimo messaggio di quit sarà nascosto dai messaggi INFO di %s.
NICK_SET_HIDE_QUIT_OFF
	Il tuo ultimo messaggio di quit sarà visualizzato nei messaggi INFO di %s.

# SET MSG responses
NICK_SET_HIDE_STATUS_ON
	Your services access status will now be hidden from %s INFO displays.
NICK_SET_HIDE_STATUS_OFF
	Your services access status will now be shown in %s INFO displays.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_MSG_ON
	I servizi ora ti risponderanno per mezzo di messaggi.
NICK_SET_MSG_OFF
	I servizi ora ti risponderanno per mezzo di notice.

# SET NOEXPIRE responses
NICK_SET_NOEXPIRE_SYNTAX
	SET [nick] NOEXPIRE {ON | OFF}
NICK_SET_NOEXPIRE_ON
	Il nick %s non scadrà.
NICK_SET_NOEXPIRE_OFF
	Il nick %s scadrà.

# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [mask]
NICK_ACCESS_ALREADY_PRESENT
	La maschera %s è già presente nella tua lista di accesso.
NICK_ACCESS_REACHED_LIMIT
	Spiacente, puoi avere soltanto %d maschere nella lista di accesso di un nick.
NICK_ACCESS_ADDED
	La maschera %s è stata aggiunta alla tua lista di accesso.
NICK_ACCESS_NOT_FOUND
	La maschera %s non è stata trovata nella tua lista di accesso.
NICK_ACCESS_DELETED
	La maschera %s è stata eliminata dalla tua lista di accesso.
NICK_ACCESS_LIST
	Lista di accesso:
NICK_ACCESS_LIST_X
	Lista di accesso di %s:
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
	%s è %s
NICK_INFO_SERVICES_OPER
	%s è un operatore dei servizi (services oper).
NICK_INFO_SERVICES_ADMIN
	%s è un amministratore dei servizi (services admin).
NICK_INFO_SERVICES_ROOT
	%s is a services root administrator.
NICK_INFO_ADDRESS
	     Ultimo indirizzo usato: %s
NICK_INFO_ADDRESS_ONLINE
	                Connesso da: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s è online in questo momento.
NICK_INFO_TIME_REGGED
	       Ora di registrazione: %s
NICK_INFO_LAST_SEEN
	             Ultimo accesso: %s
NICK_INFO_LAST_QUIT
	   Ultimo messaggio di quit: %s
NICK_INFO_URL
	                        URL: %s
NICK_INFO_EMAIL
	           Indirizzo e-mail: %s
NICK_INFO_VHOST
	                      vhost: %s
NICK_INFO_VHOST2
	            	      vhost: %s@%s
NICK_INFO_ICQ
	                 Numero ICQ: %d
NICK_INFO_GREET
	Messaggio di saluto (greet): %s
NICK_INFO_OPTIONS
	                    Opzioni: %s
NICK_INFO_EXPIRE
	Expires in: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Protezione
NICK_INFO_OPT_SECURE
	Sicurezza
NICK_INFO_OPT_PRIVATE
	Privato
NICK_INFO_OPT_MSG
	Modalità messaggio
NICK_INFO_OPT_NONE
	Nessuna
NICK_INFO_NO_EXPIRE
	Questo nick non scadrà.
NICK_INFO_FOR_MORE
	Per ottenere altre informazioni, digita /msg %s INFO %s ALL.

# LIST responses
NICK_LIST_SYNTAX
	LIST pattern
NICK_LIST_SERVADMIN_SYNTAX
	LIST pattern [FORBIDDEN] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Lista dei nick che corrispondono a %s:
NICK_LIST_RESULTS
	Fine della lista - %d/%d risultati mostrati.

# ALIST responses
NICK_ALIST_SYNTAX
	ALIST nick
NICK_ALIST_HEADER
	Canali su cui hai accesso:
	  Num  Canale               Livello  Descrizione
NICK_ALIST_HEADER_X
	Canali su cui %s ha accesso:
	  Num  Canale               Livello  Descrizione
NICK_ALIST_XOP_FORMAT
	  %3d %c%-20s %-8s %s
NICK_ALIST_ACCESS_FORMAT
	  %3d %c%-20s %-8d %s
NICK_ALIST_FOOTER
	Fine della lista - %d/%d canali mostrati.

# GLIST responses
NICK_GLIST_HEADER
	Lista dei nick nel tuo gruppo:
NICK_GLIST_HEADER_X
	Lista dei nick nel gruppo di %s:
NICK_GLIST_FOOTER
	%d nick nel gruppo.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (expires in %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nick [password]
NICK_NO_RECOVER_SELF
	Non puoi recuperare te stesso!
NICK_RECOVERED
	L'utente che usava il tuo nick è stato disconnesso.
	Digita /msg %s RELEASE %s per usarlo prima che termini il blocco di un minuto.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nick [password]
NICK_RELEASE_NOT_HELD
	Il nick %s non è bloccato.
NICK_RELEASED
	Il blocco dei servizi sul tuo nick è stato rilasciato.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nick [password]
NICK_NO_GHOST_SELF
	Non puoi disconnettere te stesso!
NICK_GHOST_KILLED
	La connessione fantasma con il tuo nick è stata disconnessa.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nick
NICK_GETPASS_UNAVAILABLE
	Il comando GETPASS non è disponibile perché è in uso la criptazione dei dati.
NICK_GETPASS_PASSWORD_IS
	La password di %s è %s.

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
	Il comando SENDPASS non è disponibile perché è in uso la criptazione dei dati.
NICK_SENDPASS_SUBJECT
	Password del nick (%s)
NICK_SENDPASS_HEAD
	Salve,
NICK_SENDPASS_LINE_1
	Hai richiesto di ricevere via e-mail la password del nick %s.
NICK_SENDPASS_LINE_2
	La password è %s Per ragioni di sicurezza, ti invitiamo a cambiarla il più presto possibile con il comando "/ns set password".
NICK_SENDPASS_LINE_3
	Se non sai perché hai ricevuto questa mail, ignorala.
NICK_SENDPASS_LINE_4
	IMPORTANTE! Non rispondere a questa mail!
NICK_SENDPASS_LINE_5
	Gli amministratori di %s.
NICK_SENDPASS_OK
	La password di %s è stata inviata.

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID nick [motivo]
NICK_FORBID_SYNTAX_REASON
	FORBID nick motivo
NICK_FORBID_SUCCEEDED
	Il nick %s ora è vietato.
NICK_FORBID_FAILED
	Impossibile vietare il nick %s!

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
NICK_REQUESTED
	Questo nick è già stato richiesto. Per favore, controlla la tua casella di posta; dovresti aver ricevuto il codice di attivazione.
NICK_REG_RESENT
	Il codice di attivazione è stato reinviato a %s.
NICK_REG_UNABLE
	Nick NON registrato, prova più tardi.
NICK_IS_PREREG
	Questo nick è in attesa di un codice di attivazione via e-mail per completare la registrazione.
NICK_ENTER_REG_CODE
	Un codice di attivazione è stato inviato a %s. Digita /msg %s CONFIRM <codice> per completare la registrazione.
NICK_CONFIRM_NOT_FOUND
	Il primo passo della registrazione potrebbe essere scaduto. Usa /msg %s REGISTER <password> <e-mail>.
NICK_CONFIRM_INVALID
	È stato inserito un codice di attivazione non valido. Controlla di nuovo la tua casella di posta e riprova.
NICK_REG_MAIL_SUBJECT
	Registrazione del nick (%s)
NICK_REG_MAIL_HEAD
	Salve,
NICK_REG_MAIL_LINE_1
	Hai richiesto la registrazione del nick %s.
NICK_REG_MAIL_LINE_2
	Digita " /msg %s CONFIRM %s " (senza virgolette) per completare la registrazione.
NICK_REG_MAIL_LINE_3
	Se non sai perché hai ricevuto questa e-mail, ignorala.
NICK_REG_MAIL_LINE_4
	NON RISPONDERE A QUESTA MAIL!
NICK_REG_MAIL_LINE_5
	Gli amministratori di %s.
NICK_GETPASS_PASSCODE_IS
	Il codice di attivazione per %s is %s.
NICK_FORCE_REG
	Il nick %s è stato confermato.

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Modo di canale +o automatico (op)
CHAN_LEVEL_AUTOVOICE
	Modo di canale +v automatico (voice)
CHAN_LEVEL_AUTOHALFOP
	Modo di canale +h automatico (halfop)
CHAN_LEVEL_AUTOPROTECT
	Modo di canale +a automatico (protect)
CHAN_LEVEL_AUTODEOP
	Modo di canale +o non permesso (op)
CHAN_LEVEL_NOJOIN
	Non permette l'accesso se il canale è RESTRICTED
CHAN_LEVEL_INVITE
	Permette l'uso del comando INVITE
CHAN_LEVEL_AKICK
	Permette l'uso del comando AKICK
CHAN_LEVEL_SET
	Permette l'uso del comando SET (escluso FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Permette l'uso del comando CLEAR
CHAN_LEVEL_UNBAN
	Permette l'uso del comando UNBAN
CHAN_LEVEL_OPDEOP
	Permette l'uso dei comandi OP e DEOP
CHAN_LEVEL_ACCESS_LIST
	Permette la visione della lista di accesso
CHAN_LEVEL_ACCESS_CHANGE
	Permette le modifiche alla lista di accesso
CHAN_LEVEL_MEMO
	Permette la lista e la lettura dei memo di canale
CHAN_LEVEL_ASSIGN
	Permette l'assegnazione e la revoca di un bot
CHAN_LEVEL_BADWORDS
	Permette l'uso del comando BADWORDS
CHAN_LEVEL_NOKICK
	Permette di non venire kickati dal bot
CHAN_LEVEL_FANTASIA
	Permette l'uso dei comandi "fantasia" del bot
CHAN_LEVEL_SAY
	Permette l'uso dei comandi SAY e ACT del bot
CHAN_LEVEL_GREET
	Mostra il messaggio di saluto (greet)
CHAN_LEVEL_VOICEME
	Permette di dare/togliere il modo +v (voice) su se stessi
CHAN_LEVEL_VOICE
	Permette l'uso dei comandi VOICE e DEVOICE
CHAN_LEVEL_GETKEY
	Permette l'uso del comando GETKEY
CHAN_LEVEL_OPDEOPME
	Permette di dare/togliere il modo +o (op) su se stessi
CHAN_LEVEL_HALFOPME
	Permette di dare/togliere il modo +h (halfop) su se stessi
CHAN_LEVEL_HALFOP
	Permette l'uso dei comandi HALFOP e DEHALFOP
CHAN_LEVEL_PROTECTME
	Permette di dare/togliere il modo +a (protect) su se stessi
CHAN_LEVEL_PROTECT
	Permette l'uso dei comandi PROTECT e DEPROTECT
CHAN_LEVEL_KICKME
	Permette l'uso del comando KICK su se stessi
CHAN_LEVEL_KICK
	Permette l'uso del comando KICK
CHAN_LEVEL_SIGNKICK
	Non invia kick "firmati" quando si usa SIGNKICK LEVEL is used
CHAN_LEVEL_BANME
	Permette l'uso del comando BAN su se stessi
CHAN_LEVEL_BAN
	Permette l'uso del comando BAN
CHAN_LEVEL_TOPIC
	Permette l'uso del comando TOPIC
CHAN_LEVEL_INFO
	Permette l'uso del comando INFO con l'opzione ALL

# Automatic responses
CHAN_IS_REGISTERED
	Questo canale è stato registrato con %s.
CHAN_NOT_ALLOWED_OP
	Non hai il permesso di avere lo stato di operatore sul canale %s.
CHAN_MAY_NOT_BE_USED
	Questo canale non può essere usato.
CHAN_NOT_ALLOWED_TO_JOIN
	Non hai il permesso di stare su questo canale.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER canale password descrizione
CHAN_REGISTER_DISABLED
	Spiacente, la registrazione dei canali è temporaneamente disabilitata.
CHAN_REGISTER_NOT_LOCAL
	I canali locali non possono essere registrati.
CHAN_MUST_REGISTER_NICK
	Devi prima registrare il tuo nick.
	Digita /msg %s HELP per avere informazioni sulla registrazione dei nick.
CHAN_MUST_IDENTIFY_NICK
	Devi prima identificarti con %s, usando il comando:
	/msg %s IDENTIFY password
CHAN_MAY_NOT_BE_REGISTERED
	Il canale %s non può essere registrato.
CHAN_ALREADY_REGISTERED
	Il canale %s è già registrato!
CHAN_MUST_BE_CHANOP
	Devi essere operatore di canale (+o) per registrare il canale.
CHAN_REACHED_CHANNEL_LIMIT
	Spiacente, hai già raggiunto il limite di %d canali registrati.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Spiacente, hai già superato il limite di %d canali registrati.
CHAN_REGISTRATION_FAILED
	Spiacente, la registrazione non è andata a buon fine.
CHAN_REGISTERED
	Il canale %s è stato registrato sotto il tuo nick: %s.
CHAN_PASSWORD_IS
	La tua password di canale è %s - non dimenticarla!
CHAN_REGISTER_NONE_CHANNEL
	You have attempted to register a none existant channel %s
CHAN_SYMBOL_REQUIRED
	Please use the symbol of # or & when attempting to register


# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY canale password
CHAN_IDENTIFY_FAILED
	Spiacente, l'identificazione è fallita.
CHAN_IDENTIFY_SUCCEEDED
	Password accettata - sei identificato come fondatore di %s.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT canale nick
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT canale [nick]
CHAN_LOGOUT_SUCCEEDED
	L'utente %s non è più identificato per il canale %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	Tutti gli utenti non sono più identificati per il canale %s.

# DROP responses
CHAN_DROP_SYNTAX
	DROP canale
CHAN_DROP_DISABLED
	Spiacente, la deregistrazione dei canali è temporaneamente disabilitata.
CHAN_DROPPED
	Il canale %s è stato deregistrato.

# SET responses
CHAN_SET_SYNTAX
	SET canale opzione parametri
CHAN_SET_DISABLED
	Spiacente, l'impostazione delle opzioni dei canali è temporaneamente disabilitata.
CHAN_SET_UNKNOWN_OPTION
	Opzione SET %s sconosciuta.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s ha registrato troppi nick.
CHAN_FOUNDER_CHANGED
	Il fondatore del canale %s ora è %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Il successore del canale %s ora è %s.
CHAN_SUCCESSOR_UNSET
	Il successore del canale %s è stato annullato.
CHAN_SUCCESSOR_IS_FOUNDER
	%s non può essere il successore del canale %s perché ne è il fondatore.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Spiacente, il cambio di password non è andato a buon fine.
CHAN_PASSWORD_CHANGED
	La password di %s è stata cambiata.
CHAN_PASSWORD_CHANGED_TO
	La password di %s è stata impostata a %s.

# SET DESC responses
CHAN_DESC_CHANGED
	La descrizione di %s è stata impostata a %s.

# SET URL responses
CHAN_URL_CHANGED
	L'URL di %s è stato impostato a %s.
CHAN_URL_UNSET
	L'URL di %s è stato annullato.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	L'indirizzo e-mail di %s è stato impostato a %s.
CHAN_EMAIL_UNSET
	L'indirizzo e-mail di %s è stato annullato.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Il messaggio di ingresso di %s è stato cambiato.
CHAN_ENTRY_MSG_UNSET
	Il messaggio di ingresso di %s è stato annullato.

# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s non è un tipo di ban valido.
CHAN_SET_BANTYPE_CHANGED
	Il tipo di ban del canale %s ora è #%d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	%c è stato ignorato perché è un modo di canale sconosciuto.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Il modo %c è stato ignorato perché non è memorizzabile.
CHAN_SET_MLOCK_L_REQUIRED
	Per bloccare il modo +L devi bloccare anche il modo +l.
CHAN_SET_MLOCK_K_REQUIRED
	Per bloccare il modo +K devi bloccare anche il modo +i.
CHAN_MLOCK_CHANGED
	Il blocco dei modi di canale di %s è stato impostato a %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET canale KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Topic retention option for %s is now ON.
CHAN_SET_KEEPTOPIC_OFF
	Topic retention option for %s is now OFF.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET canale TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Topic lock option for %s is now ON.
CHAN_SET_TOPICLOCK_OFF
	Topic lock option for %s is now OFF.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET canale PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Peace option for %s is now ON.
CHAN_SET_PEACE_OFF
	Peace option for %s is now OFF.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET canale PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Private option for %s is now ON.
CHAN_SET_PRIVATE_OFF
	Private option for %s is now OFF.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET canale SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Secure ops option for %s is now ON.
CHAN_SET_SECUREOPS_OFF
	Secure ops option for %s is now OFF.

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET canale SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Secure founder option for %s is now ON.
CHAN_SET_SECUREFOUNDER_OFF
	Secure founder option for %s is now OFF.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET canale RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Restricted access option for %s is now ON.
CHAN_SET_RESTRICTED_OFF
	Restricted access option for %s is now OFF.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET canale SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Secure option for %s is now ON.
CHAN_SET_SECURE_OFF
	Secure option for %s is now OFF.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET canale SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Signed kick option for %s is now ON.
CHAN_SET_SIGNKICK_LEVEL
	Signed kick option for %s is now ON, but depends of the
	level of the user that is using the command.
CHAN_SET_SIGNKICK_OFF
	Signed kick option for %s is now OFF.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET canale OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice option for %s is now ON.
CHAN_SET_OPNOTICE_OFF
	Op-notice option for %s is now OFF.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET canale XOP {ON | OFF}
CHAN_SET_XOP_ON
	xOP lists system for %s is now ON.
CHAN_SET_XOP_OFF
	xOP lists system for %s is now OFF.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET canale NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Il canale %s non scadrà.
CHAN_SET_NOEXPIRE_OFF
	Il canale %s scadrà.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Spiacente, puoi avere un massimo di %d nick nelle liste SOP/AOP/HOP/VOP di un canale.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	Questo comando non è utilizzabile; usa il comando ACCESS.
	Digita /msg %s HELP ACCESS per avere maggiori informazioni.

# AOP messages
CHAN_AOP_SYNTAX
	AOP canale {ADD|DEL|LIST|CLEAR} [nick | numero-lista]
CHAN_AOP_DISABLED
	Spiacente, la modifica della lista AOP è temporaneamente disabilitata.
CHAN_AOP_NICKS_ONLY
	La lista AOP può contenere soltanto nick registrati.
CHAN_AOP_ADDED
	%s aggiunto alla lista AOP di %s.
CHAN_AOP_MOVED
	%s spostato alla lista AOP di %s.
CHAN_AOP_NO_SUCH_ENTRY
	Il record specificato (%d) non è presente nella lista AOP di %s.
CHAN_AOP_NOT_FOUND
	%s non trovato nella lista AOP di %s.
CHAN_AOP_NO_MATCH
	Nessun record corrispondente nella lista AOP di %s.
CHAN_AOP_DELETED
	%s eliminato dalla lista AOP di %s.
CHAN_AOP_DELETED_ONE
	Eliminato un record dalla lista AOP di %s.
CHAN_AOP_DELETED_SEVERAL
	Eliminati %d record dalla lista AOP di %s.
CHAN_AOP_LIST_EMPTY
	La lista AOP di %s è vuota.
CHAN_AOP_LIST_HEADER
	Lista AOP di %s:
	  Num  Nick
CHAN_AOP_CLEAR
	La lista AOP del canale è stata svuotata.

# HOP messages
CHAN_HOP_SYNTAX
	HOP canale {ADD|DEL|LIST|CLEAR} [nick | numero-lista]
CHAN_HOP_DISABLED
	Spiacente, la modifica della lista HOP è temporaneamente disabilitata.
CHAN_HOP_NICKS_ONLY
	La lista HOP può contenere soltanto nick registrati.
CHAN_HOP_ADDED
	%s aggiunto alla lista HOP di %s.
CHAN_HOP_MOVED
	%s spostato alla lista HOP di %s.
CHAN_HOP_NO_SUCH_ENTRY
	Il record specificato (%d) non è presente nella lista HOP di %s.
CHAN_HOP_NOT_FOUND
	%s non trovato nella lista HOP di %s.
CHAN_HOP_NO_MATCH
	Nessun record corrispondente nella lista HOP di %s.
CHAN_HOP_DELETED
	%s eliminato dalla lista HOP di %s.
CHAN_HOP_DELETED_ONE
	Eliminato un record dalla lista HOP di %s.
CHAN_HOP_DELETED_SEVERAL
	Eliminati %d record dalla lista HOP di %s.
CHAN_HOP_LIST_EMPTY
	La lista HOP di %s è vuota.
CHAN_HOP_LIST_HEADER
	Lista HOP di %s:
	  Num  Nick
CHAN_HOP_CLEAR
	La lista HOP del canale è stata svuotata.

# SOP messages
CHAN_SOP_SYNTAX
	SOP canale {ADD|DEL|LIST|CLEAR} [nick | numero-lista]
CHAN_SOP_DISABLED
	Spiacente, la modifica della lista SOP è temporaneamente disabilitata.
CHAN_SOP_NICKS_ONLY
	La lista SOP può contenere soltanto nick registrati.
CHAN_SOP_ADDED
	%s aggiunto alla lista SOP di %s.
CHAN_SOP_MOVED
	%s spostato alla lista SOP di %s.
CHAN_SOP_NO_SUCH_ENTRY
	Il record specificato (%d) non è presente nella lista SOP di %s.
CHAN_SOP_NOT_FOUND
	%s non trovato nella lista SOP di %s.
CHAN_SOP_NO_MATCH
	Nessun record corrispondente nella lista SOP di %s.
CHAN_SOP_DELETED
	%s eliminato dalla lista SOP di %s.
CHAN_SOP_DELETED_ONE
	Eliminato un record dalla lista SOP di %s.
CHAN_SOP_DELETED_SEVERAL
	Eliminati %d record dalla lista SOP di %s.
CHAN_SOP_LIST_EMPTY
	La lista SOP di %s è vuota.
CHAN_SOP_LIST_HEADER
	Lista SOP di %s:
	  Num  Nick
CHAN_SOP_CLEAR
	La lista SOP del canale è stata svuotata.

# VOP messages
CHAN_VOP_SYNTAX
	VOP canale {ADD|DEL|LIST|CLEAR} [nick | numero-lista]
CHAN_VOP_DISABLED
	Spiacente, la modifica della lista VOP è temporaneamente disabilitata.
CHAN_VOP_NICKS_ONLY
	La lista VOP può contenere soltanto nick registrati.
CHAN_VOP_ADDED
	%s aggiunto alla lista VOP di %s.
CHAN_VOP_MOVED
	%s spostato alla lista VOP di %s.
CHAN_VOP_NO_SUCH_ENTRY
	Il record specificato (%d) non è presente nella lista VOP di %s.
CHAN_VOP_NOT_FOUND
	%s non trovato nella lista VOP di %s.
CHAN_VOP_NO_MATCH
	Nessun record corrispondente nella lista VOP di %s.
CHAN_VOP_DELETED
	%s eliminato dalla lista VOP di %s.
CHAN_VOP_DELETED_ONE
	Eliminato un record dalla lista VOP di %s.
CHAN_VOP_DELETED_SEVERAL
	Eliminati %d record dalla lista VOP di %s.
CHAN_VOP_LIST_EMPTY
	La lista VOP di %s è vuota.
CHAN_VOP_LIST_HEADER
	Lista VOP di %s:
	  Num  Nick
CHAN_VOP_CLEAR
	La lista VOP del canale è stata svuotata.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS canale {ADD|DEL|LIST|CLEAR} [nick [livello] | numero-lista]
CHAN_ACCESS_XOP
	Questo comando non è utilizzabile; usa i comandi SOP, AOP, HOP e VOP.
	Digita /msg %s HELP comando per ottenere maggiori informazioni.
CHAN_ACCESS_DISABLED
	Spiacente, la modifica della lista di accesso dei canali è temporaneamente disabilitata.
CHAN_ACCESS_LEVEL_NONZERO
	Il livello di accesso deve essere diverso da zero.
CHAN_ACCESS_LEVEL_RANGE
	Il livello di accesso deve essere compreso tra %d e %d (inclusi).
CHAN_ACCESS_NICKS_ONLY
	Le liste di accesso del canale possono contenere soltanto nick registrati.
CHAN_ACCESS_REACHED_LIMIT
	Spiacente, puoi avere un massimo di %d nella lista di accesso.
CHAN_ACCESS_LEVEL_UNCHANGED
	Il livello di accesso di %s su %s non è stato modificato da %d.
CHAN_ACCESS_LEVEL_CHANGED
	Il livello di accesso di %s su %s è stato modificato a %d.
CHAN_ACCESS_ADDED
	%s aggiunto alla lista di accesso di %s con il livello %d.
CHAN_ACCESS_NO_SUCH_ENTRY
	Il record specificato (%d) non è presente nella lista di accesso di %s.
CHAN_ACCESS_NOT_FOUND
	%s non trovato nella lista nella lista di accesso di %s.
CHAN_ACCESS_NO_MATCH
	Nessun record corrispondente nella lista di accesso di %s.
CHAN_ACCESS_DELETED
	%s è stato eliminato dalla lista di accesso di %s.
CHAN_ACCESS_DELETED_ONE
	Eliminato un record dalla lista di accesso di %s.
CHAN_ACCESS_DELETED_SEVERAL
	Eliminati %d record dalla lista di accesso di %s.
CHAN_ACCESS_LIST_EMPTY
	La lista di accesso di %s è vuota.
CHAN_ACCESS_LIST_HEADER
	Lista di accesso di %s:
	  Num   Liv  Nick
CHAN_ACCESS_LIST_FOOTER
	End of access list.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	La lista di accesso del canale è stata svuotata.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK canale {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [nick-o-usermask] [motivo]
CHAN_AKICK_DISABLED
	Spiacente, la modifica della lista autokick è temporaneamente disabilitata.
CHAN_AKICK_ALREADY_EXISTS
	%s è già presente nella lista autokick di %s.
CHAN_AKICK_REACHED_LIMIT
	Spiacente, puoi avere un massimo di %d maschere autokick.
CHAN_AKICK_ADDED
	%s aggiunto alla lista autokick di %s.
CHAN_AKICK_NO_SUCH_ENTRY
	Il record specificato (%d) non è presente nella lista autokick di %s.
CHAN_AKICK_NOT_FOUND
	%s non trovato nella lista autokick di %s.
CHAN_AKICK_NO_MATCH
	Nessun record corrispondente nella lista autokick di %s.
CHAN_AKICK_STUCK
	%s da adesso è sempre attivo su %s.
CHAN_AKICK_UNSTUCK
	%s da adesso non è più sempre attivo su %s.
CHAN_AKICK_DELETED
	%s è stato eliminato dalla lista autokick di %s.
CHAN_AKICK_DELETED_ONE
	Eliminato un record dalla lista autokick di %s.
CHAN_AKICK_DELETED_SEVERAL
	Eliminati %d record dalla lista autokick di %s.
CHAN_AKICK_LIST_EMPTY
	La lista autokick di %s è vuota.
CHAN_AKICK_LIST_HEADER
	Lista autokick di %s:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (aggiunto da %s il %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (fisso) (aggiunto da %s il %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	AKICK ENFORCE su %s completato; ne sono stati influenzati %d utenti.
CHAN_AKICK_CLEAR
	La lista autokick del canale è stata svuotata.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS canale {SET | DIS[ABLE] | LIST | RESET} [numero [livello]]
CHAN_LEVELS_XOP
	Questo comando è inutilizzabile in questa modalità.
CHAN_LEVELS_RANGE
	Il livello deve essere compreso tra %d e %d (inclusi).
CHAN_LEVELS_CHANGED
	Il livello di %s sul canale %s è stato modificato a %d.
CHAN_LEVELS_UNKNOWN
	Impostazione %s sconosciuta. Digita /msg %s HELP LEVELS DESC per una lista delle impostazioni valide.
CHAN_LEVELS_DISABLED
	%s disabilitato sul canale %s.
CHAN_LEVELS_LIST_HEADER
	Impostazioni dei livelli di accesso del canale %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (disabilitato)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (solo per il fondatore)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	I livelli di accesso di %s sono stati ripristinati ai valori originali.

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
	INFO canale [ALL]
CHAN_INFO_HEADER
	Informazioni sul canale %s:
CHAN_INFO_FOUNDER
	            Fondatore: %s (%s)
CHAN_INFO_NO_FOUNDER
	            Fondatore: %s
CHAN_INFO_SUCCESSOR
	           Successore: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	           Successore: %s
CHAN_INFO_DESCRIPTION
	          Descrizione: %s
CHAN_INFO_ENTRYMSG
	Messaggio di ingresso: %s
CHAN_INFO_TIME_REGGED
	Data di registrazione: %s
CHAN_INFO_LAST_USED
	      Ultimo utilizzo: %s
CHAN_INFO_LAST_TOPIC
	         Ultimo topic: %s
CHAN_INFO_TOPIC_SET_BY
	   Topic impostato da: %s
CHAN_INFO_URL
	                  URL: %s
CHAN_INFO_EMAIL
	     Indirizzo e-mail: %s
CHAN_INFO_BANTYPE
	          Tipo di ban: %d
CHAN_INFO_OPTIONS
	              Opzioni: %s
CHAN_INFO_OPT_KEEPTOPIC
	Mantenimento del topic
CHAN_INFO_OPT_OPNOTICE
	OP Notice
CHAN_INFO_OPT_PEACE
	Pace
CHAN_INFO_OPT_PRIVATE
	Privato
CHAN_INFO_OPT_RESTRICTED
	Restricted
CHAN_INFO_OPT_SECURE
	Secure
CHAN_INFO_OPT_SECUREOPS
	Secure Ops
CHAN_INFO_OPT_SECUREFOUNDER
	Secure Founder
CHAN_INFO_OPT_SIGNKICK
	Signed kick
CHAN_INFO_OPT_TOPICLOCK
	Blocco del topic
CHAN_INFO_OPT_XOP
	Liste xOP
CHAN_INFO_OPT_NONE
	Nessno
CHAN_INFO_MODE_LOCK
	        Modi bloccati: %s
CHAN_INFO_EXPIRE
	      Expires in: %s
CHAN_INFO_NO_EXPIRE
	Questo canale non scadrà.

# LIST responses
CHAN_LIST_SYNTAX
	LIST pattern
CHAN_LIST_SERVADMIN_SYNTAX
	LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
CHAN_LIST_HEADER
	Lista dei canali che corrispondono a %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Fine della lista - %d/%d risultati mostrati.

# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE canale

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN canale
CHAN_UNBANNED
	I ban che ti riguardavano sono stati rimossi da %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC canale [topic]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR canale cosa
CHAN_CLEARED_BANS
	Tutti i ban del canale %s sono stati rimossi.
CHAN_CLEARED_EXCEPTS
	Tutte le eccezioni del canale %s sonostate rimosse.
CHAN_CLEARED_MODES
	Tutti i modi del canale %s sono stati ripristinati.
CHAN_CLEARED_OPS
	Il modo +o è stato rimosso da tutti gli utenti di %s.
CHAN_CLEARED_HOPS
	Mode +h has been cleared from channel %s.
CHAN_CLEARED_VOICES
	Il modo +v è stato rimosso da tutti gli utenti di %s.
CHAN_CLEARED_USERS
	Tutti gli utenti sono stati kickati dal canale %s.
CHAN_CLEARED_INVITES
	All invites on channel %s have been removed.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS canale
CHAN_GETPASS_UNAVAILABLE
	Il comando GETPASS è inutilizzabile perché è in uso la criptazione dei dati.
CHAN_GETPASS_PASSWORD_IS
	La chiave (password) del canale %s è %s.

# SENDPASS responses
CHAN_GETKEY_SYNTAX
	GETKEY channel
CHAN_GETKEY_NOKEY
	The channel %s has no key.
CHAN_GETKEY_KEY
	Key for channel %s is %s.

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS canale
CHAN_SENDPASS_UNAVAILABLE
	Il comando SENDPASS è inutilizzabile perché è in uso la criptazione dei dati.
CHAN_SENDPASS_SUBJECT
	Password del canale (%s)
CHAN_SENDPASS_HEAD
	Salve,
CHAN_SENDPASS_LINE_1
	Hai richiesto di ricevere via e-mail la password del canale %s.
CHAN_SENDPASS_LINE_2
	La password è %s. Per ragioni di sicurezza, ti invitiamo a cambiarla il più presto possibile con il comando "/cs set canale password".
CHAN_SENDPASS_LINE_3
	Se non sai perché hai ricevuto questa mail, ignorala.
CHAN_SENDPASS_LINE_4
	IMPORTANTE! Non rispondere a questa mail!
CHAN_SENDPASS_LINE_5
	Gli amministratori di %s.
CHAN_SENDPASS_OK
	La password di %s è stata inviata.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID canale [motivo]
CHAN_FORBID_SYNTAX_REASON
	FORBID canale motivo
CHAN_FORBID_SUCCEEDED
	Il canale %s non può più essere registrato o usato.
CHAN_FORBID_FAILED
	Impossibile vietare il canale %s!

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
CHAN_FORBID_REASON
	Questo canale è stato vietato.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND canale [motivo]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND canale motivo
CHAN_SUSPEND_SUCCEEDED
	Il canale %s is ora è sospeso.
CHAN_SUSPEND_FAILED
	Impossibile sospendere il canale %s!
CHAN_SUSPEND_REASON
	Questo canale è stato sospeso.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND canale
CHAN_UNSUSPEND_ERROR
	No # found in front of channel name.
CHAN_UNSUSPEND_SUCCEEDED
	Il canale %s ora è stato rilasciato.
CHAN_UNSUSPEND_FAILED
	Impossibile rilasciare il canale %s!

# Misc responses
CHAN_EXCEPTED
	%s corrisponde a un'eccezione %s e non può essere bannato finché l'eccezione non sarà rimossa.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	Hai un nuovo memo.
MEMO_HAVE_NEW_MEMOS
	Hai %d nuovi memo.
MEMO_TYPE_READ_LAST
	Digita /msg %s READ LAST per leggerlo.
MEMO_TYPE_READ_NUM
	Digita /msg %s READ %d per leggerlo.
MEMO_TYPE_LIST_NEW
	Digita /msg %s LIST NEW per ottenerne la lista.
MEMO_AT_LIMIT
	Attenzione: Hai raggiunto il numero massimo di memo (%d). Non potrai raggiungere nuovi memo finché non ne elimini alcuni.
MEMO_OVER_LIMIT
	Attenzione: Hai superato il numero massimo di memo (%d). Non potrai raggiungere nuovi memo finché non ne elimini alcuni.
MEMO_X_MANY_NOTICE
	Ci sono %d memo sul canale %s.
MEMO_X_ONE_NOTICE
	C'è %d memo sul canale %s.
MEMO_NEW_X_MEMO_ARRIVED
	C'è un nuovo memo sul canale %s.
	Digita /msg %s READ %s %d per leggerlo.
MEMO_NEW_MEMO_ARRIVED
	Hai ricevuto un nuovo memo da %s.
	Digita /msg %s READ %d per leggerlo.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Non hai nuovi memo.
MEMO_X_HAS_NO_MEMOS
	%s non ha nuovi memo.
MEMO_DOES_NOT_EXIST
	Il memo %d non esiste!
MEMO_LIST_NOT_FOUND
	Non è stato trovato nessun memo corrispondente.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {nick | canale} testo-del-memo
MEMO_SEND_DISABLED
	Spiacente, l'invio dei memo è temporaneamente disabilitato.
MEMO_SEND_PLEASE_WAIT
	Attendi %d secondi prima di usare di nuovo il comando SEND.
MEMO_X_GETS_NO_MEMOS
	%s non può ricevere memo.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s attualmente ha troppi memo e non può riceverne altri.
MEMO_SENT
	Memo inviato a %s.

# CANCEL responses
MEMO_MASS_SENT
	A massmemo has been sent to all registered users.
	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {nick | canale}
MEMO_CANCEL_DISABLED
	Spiacente, l'annullamento dei memo è temporaneamente disabilitato.
MEMO_CANCEL_NONE
	Nessun memo è annullabile.
MEMO_CANCELLED
	L'ultimo memo inviato a %s è stato annullato.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [canale] [lista | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	Non hai nuovi memo.
MEMO_X_HAS_NO_NEW_MEMOS
	%s non ha nuovi memo.
MEMO_LIST_MEMOS
	Memo di %s. Per leggerli, digita: /msg %s READ numero
MEMO_LIST_NEW_MEMOS
	Nuovi memo di %s. Per leggerli, digita: /msg %s READ numero
MEMO_LIST_CHAN_MEMOS
	Memo di %s. Per leggerli, digita: /msg %s READ %s numero
MEMO_LIST_CHAN_NEW_MEMOS
	Nuovi memo di %s. Per leggerli, digita: /msg %s READ %s numero
MEMO_LIST_HEADER
	 Num  Mittente          Data/Ora
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [canale] {lista | LAST | NEW}
MEMO_HEADER
	Memo %d da %s (%s). Per eliminarlo, digita: /msg %s DEL %d
MEMO_CHAN_HEADER
	Memo %d da %s (%s). Per eliminarlo, digita: /msg %s DEL %s %d
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [canale] {numero | lista | ALL}
MEMO_DELETED_NONE
	Nessun memo è stato cancellato.
MEMO_DELETED_ONE
	Il memo %d è stato cancellato.
MEMO_DELETED_SEVERAL
	I memo %s sono stati cancellati.
MEMO_DELETED_ALL
	Tutti i tuoi memo sono stati cancellati.

# SET responses
MEMO_CHAN_DELETED_ALL
	All memos for channel %s have been deleted.

# SET responses
MEMO_SET_SYNTAX
	SET opzione parametri
MEMO_SET_DISABLED
	Spiacente, l'impostazione delle opzioni dei memo è temporaneamente disabilitata.
MEMO_SET_UNKNOWN_OPTION
	Opzione SET %s sconosciuta.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s ti informerà dei nuovi memo quando ti connetti e quando li ricevi.
MEMO_SET_NOTIFY_LOGON
	%s ti informerà dei nuovi memo quando ti connetti e quando torni dall'/AWAY.
MEMO_SET_NOTIFY_NEW
	%s ti informerà dei nuovi memo quando li ricevi.
MEMO_SET_NOTIFY_OFF
	%s non ti informerà dei nuovi memo.

# SET LIMIT responses
MEMO_SET_NOTIFY_MAIL
	You will now be informed about new memos via email.
MEMO_SET_NOTIFY_NOMAIL
	You will no longer be informed via email.
MEMO_SET_NOTIFY_INVALIDMAIL
	There's no email address set for your nick.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [canale] limite
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [utente | canale] {limite | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Non puoi modificare limite dei tuoi memo.
MEMO_SET_LIMIT_FORBIDDEN
	Il limite dei memo di %s non può essere modificato.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Non puoi impostare il limite dei tuoi memo ad un valore superiore a %d.
MEMO_SET_LIMIT_TOO_HIGH
	Non puoi impostare il limite dei memo di %s ad un valore superiore a %d.
MEMO_SET_LIMIT_OVERFLOW
	Il limite dei memo è troppo alto; sarà utilizzato il valore %d.
MEMO_SET_YOUR_LIMIT
	Il limite dei tuoi memo è stato impostato a %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Non potrai più ricevere memo.
MEMO_UNSET_YOUR_LIMIT
	Il limite dei tuoi memo è stato disabilitato.
MEMO_SET_LIMIT
	Il limite dei memo di %s è stato impostato a %d.
MEMO_SET_LIMIT_ZERO
	Il limite dei memo di %s è stato impostato a 0.
MEMO_UNSET_LIMIT
	Il limite dei memo di %s è stato disabilitato.

# INFO responses
MEMO_INFO_SYNTAX
	INFO [canale]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [nick | canale]
MEMO_INFO_NO_MEMOS
	Non hai alcun memo.
MEMO_INFO_MEMO
	Hai 1 memo.
MEMO_INFO_MEMO_UNREAD
	Hai 1 memo, e non l'hai ancora letto.
MEMO_INFO_MEMOS
	Hai %d memo.
MEMO_INFO_MEMOS_ONE_UNREAD
	Hai %d memo, di cui 1 non è stato ancora letto.
MEMO_INFO_MEMOS_SOME_UNREAD
	Hai %d memo, di cui %d non sono stati ancora letti.
MEMO_INFO_MEMOS_ALL_UNREAD
	Hai %d memo, nessuno dei quali è stato letto.
MEMO_INFO_LIMIT
	Il limite dei tuoi memo è %d.
MEMO_INFO_HARD_LIMIT
	Il limite dei tuoi memo è %d, e non può essere modificato.
MEMO_INFO_LIMIT_ZERO
	Il limite dei tuoi memo è 0; non riceverai alcun nuovo memo.
MEMO_INFO_HARD_LIMIT_ZERO
	Il limite dei tuoi memo è 0; non riceverai alcun nuovo memo. Non puoi modificare questo limite.
MEMO_INFO_NO_LIMIT
	Non c'è alcun limite al numero di memo che puoi conservare.
MEMO_INFO_NOTIFY_OFF
	Non sarai informato dei nuovi memo.
MEMO_INFO_NOTIFY_ON
	Sarai informato dei nuovi memo quando ti connetti e quando li ricevi.
MEMO_INFO_NOTIFY_RECEIVE
	Sarai informato dei nuovi memo quando li ricevi.
MEMO_INFO_NOTIFY_SIGNON
	Sarai informato dei nuovi memo quando ti connetti.
MEMO_INFO_X_NO_MEMOS
	%s non ha alcun memo.
MEMO_INFO_X_MEMO
	%s ha 1 memo.
MEMO_INFO_X_MEMO_UNREAD
	%s ha 1 memo, e non è stato ancora letto.
MEMO_INFO_X_MEMOS
	%s ha %d memo.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s ha %d memo, di cui 1 non è stato ancora letto.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s ha %d memo, di cui %d non sono stati ancora letti.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s ha %d memo, nessuno dei quali è stato letto.
MEMO_INFO_X_LIMIT
	Il limite dei memo di %s è %d.
MEMO_INFO_X_HARD_LIMIT
	Il limite dei memo di %s è %d, e non può essere modificato.
MEMO_INFO_X_NO_LIMIT
	%s non ha alcun limite dei memo.
MEMO_INFO_X_NOTIFY_OFF
	%s non viene informato dei nuovi memo.
MEMO_INFO_X_NOTIFY_ON
	%s viene informato dei nuovi memo quando si connette e quando li riceve.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s viene informato dei nuovi memo quando li riceve.
MEMO_INFO_X_NOTIFY_SIGNON
	%s viene informato dei nuovi memo quando si connette.

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
	Il bot %s non esiste.
BOT_NOT_ASSIGNED
	Devi assegnare un bot al canale prima di usare questo comando.
	Digita /msg %S HELP ASSIGN per maggiori informazioni.
BOT_NOT_ON_CHANNEL
	Il bot non è sul canale %s.

# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	Non usare la parola %s su questo canale!
BOT_REASON_BADWORD_GENTLE
	Occhio a come parli!
BOT_REASON_BOLD
	Non usare il grassetto su questo canale!
BOT_REASON_CAPS
	Disattiva il caps lock!
BOT_REASON_COLOR
	Non usare i colori su questo canale!
BOT_REASON_FLOOD
	Non floodare!
BOT_REASON_REPEAT
	Smettila di ripetere!
BOT_REASON_REVERSE
	Non usare il reverse su questo canale!
BOT_REASON_UNDERLINE
	Non usare il sottolineato su questo canale!

# !seen replies
BOT_SEEN_BOT
	Mi hai trovato, %s!
BOT_SEEN_YOU
	Cerchi te stesso, eh %s?
BOT_SEEN_ON_CHANNEL
	%s è sul canale in questo momento!
BOT_SEEN_ON_CHANNEL_AS
	%s è sul canale in questo momento, travestito da %s! 
BOT_SEEN_ON
	L'ultima volta che ho visto %s è stato %s fa.
BOT_SEEN_NEVER
	Non ho mai visto %s su questo canale.
BOT_SEEN_UNKNOWN
	Non so chi sia %s.

# BOT responses
BOT_BOT_SYNTAX
	BOT ADD nick ident host realname
	BOT CHANGE vecchionick nuovonick [ident [host [realname]]]
	BOT DEL nick
	BOT LIST
BOT_BOT_ALREADY_EXISTS
	Il bot %s è già esistente.
BOT_BOT_CREATION_FAILED
	Spiacente, la creazione del bot non è andata a buon fine.
BOT_BOT_READONLY
	Spiacente, la modifica dei bot è temporaneamente disabilitata.
BOT_BOT_ADDED
	%s!%s@%s (%s) aggiunto alla lista dei bot.
BOT_BOT_ANY_CHANGES
	Le vecchie informazioni sono identiche a quelle nuove.
BOT_BOT_CHANGED
	Il bot %s è stato modificato; ora si presenta come %s!%s@%s (%s)
BOT_BOT_DELETED
	Il bot %s è stato eliminato.

# BOTLIST responses
BOT_BOTLIST_HEADER
	Lista dei bot disponibili:
BOT_BOTLIST_PRIVATE_HEADER
	Bot riservati agli IrcOp:
BOT_BOTLIST_FOOTER
	%d bot disponibili.
BOT_BOTLIST_EMPTY
	Non ci sono bot disponibili in questo momento.
	Chiedi a un Services Admin di crearne uno!

# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN canale nick-del-bot
BOT_ASSIGN_READONLY
	Spiacente, l'assegnazione dei bot è temporaneamente disabilitata.
BOT_ASSIGN_ALREADY
	Bot %s is already assigned to channel %s.
BOT_ASSIGN_ASSIGNED
	Il bot %s è stato assegnato a %s.

# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN canale
BOT_UNASSIGN_UNASSIGNED
	Non c'è più nessun bot assegnato a %s.

# INFO responses
BOT_INFO_SYNTAX
	INFO {canale | nick-del-bot}
BOT_INFO_NOT_FOUND
	%s non è un bot valido o un canale registrato.
BOT_INFO_BOT_HEADER
	Informazioni sul bot %s:
BOT_INFO_BOT_MASK
	        Hostmask: %s@%s
BOT_INFO_BOT_REALNAME
	        Realname: %s
BOT_INFO_BOT_CREATED
	Data di crazione: %s
BOT_INFO_BOT_USAGE
	        Usato su: %d canale/i
BOT_INFO_BOT_OPTIONS
	         Opzioni: %s
BOT_INFO_OPT_PRIVATE
	Privato
BOT_INFO_CHAN_HEADER
	Informazioni sul canale %s:
BOT_INFO_CHAN_BOT
	Nick del bot: %s
BOT_INFO_CHAN_BOT_NONE
	Nick del bot: nessun bot assegnato.
BOT_INFO_CHAN_KICK_BADWORDS
	   Kick in caso di parolacce: %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	   Kick in caso di parolacce: %s (%d kick portano al ban)
BOT_INFO_CHAN_KICK_BOLDS
	   Kick in caso di grassetto: %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	   Kick in caso di grassetto: %s (%d kick portano al ban)
BOT_INFO_CHAN_KICK_CAPS_ON
	   Kick in caso di maiuscole: %s (minimo %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	   Kick in caso di maiuscole: %s (%d portano al ban; minimo %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	   Kick in caso di maiuscole: %s
BOT_INFO_CHAN_KICK_COLORS
	      Kick in caso di colori: %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	      Kick in caso di colori: %s (%d portano al ban)
BOT_INFO_CHAN_KICK_FLOOD_ON
	       Kick in caso di flood: %s (%d linee in %d secondi)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	       Kick in caso di flood: %s (%d kick portano al ban; %d linee in %d secondi)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	       Kick in caso di flood: %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	 Kick in caso di ripetizioni: %s (%d volte)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	 Kick in caso di ripetizioni: %s (%d kick portano al ban; %d volte)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	 Kick in caso di ripetizioni: %s
BOT_INFO_CHAN_KICK_REVERSES
	     Kick in caso di reverse: %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	     Kick in caso di reverse: %s (%d kick portano al ban)
BOT_INFO_CHAN_KICK_UNDERLINES
	Kick in caso di sottolineato: %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	Kick in caso di sottolineato: %s (%d portano al ban ban)
BOT_INFO_ACTIVE
	attivato
BOT_INFO_INACTIVE
	disattivato
BOT_INFO_CHAN_OPTIONS
	Opzioni: %s
BOT_INFO_OPT_DONTKICKOPS
	Protezione per gli op
BOT_INFO_OPT_DONTKICKVOICES
	Protezione per i voice
BOT_INFO_OPT_FANTASY
	Fantasy
BOT_INFO_OPT_GREET
	Greet
BOT_INFO_OPT_NOBOT
	Nessun bot
BOT_INFO_OPT_SYMBIOSIS
	Symbiosis
BOT_INFO_OPT_NONE
	Nessuna
	
# SET responses
BOT_SET_SYNTAX
	SET canale opzione parametri
BOT_SET_DISABLED
	Spiacente, l'impostazione delle opzioni dei bot è temporaneamente disabilitata.
BOT_SET_UNKNOWN
	Opzione %s sconosciuta.
	Digita /msg %S HELP SET per maggiori informazioni.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET canale DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Il bot non kickerà gli operatori sul canale %s.
BOT_SET_DONTKICKOPS_OFF
	Il bot kickerà anche gli operatori sul canale %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET canale DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Il bot non kickerà i voice sul canale %s.
BOT_SET_DONTKICKVOICES_OFF
	Il bot kickerà anche i voice sul canale %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET canale FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	La modalità fantasy ora è ATTIVA sul canale %s.
BOT_SET_FANTASY_OFF
	La modalità fantasy ora è DISATTIVA sul canale %s.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET canale GREET {ON|OFF}
BOT_SET_GREET_ON
	La modalità greet (saluto) ora è ATTIVA sul canale %s.
BOT_SET_GREET_OFF
	La modalità greet (saluto) ora è DISATTIVA sul canale %s.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET canale NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	La modalità No Bot ora è ATTIVA sul canale %s.
BOT_SET_NOBOT_OFF
	La modalità No Bot ora è DISATTIVA sul canale %s.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET canale PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	La modalità Private del bot %s ora è ATTIVA.
BOT_SET_PRIVATE_OFF
	La modalità Private del bot %s ora è DISATTIVA.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET canale SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	La modalità symbiosis ora è ATTIVA sul canale %s.
BOT_SET_SYMBIOSIS_OFF
	La modalità symbiosis ora è DISATTIVA sul canale %s.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK canale opzione {ON|OFF} [parametri]
BOT_KICK_DISABLED
	Spiacente, la configurazione dei kick è temporaneamente disabilitata.
BOT_KICK_UNKNOWN
	Opzione %s sconosciuta.
	Digita /msg %S HELP KICK per maggiori informazioni.
BOT_KICK_BAD_TTB
	%s non può essere utilizzato come il numero di kick che porta al ban.

BOT_KICK_BADWORDS_ON
	Il bot ora kickerà chi dice le parolacce. Usa il comando BADWORDS
	per aggiungere o rimuovere le parolacce dalla lista.
BOT_KICK_BADWORDS_ON_BAN
	Il bot ora kickerà chi dice le bad words, e piazzerà un ban dopo
	%d kick nei confronti dello stesso utente. Usa il comando BADWORDS
	per aggiungere o rimuovere le parolacce dalla lista.
BOT_KICK_BADWORDS_OFF
	Il bot non kickerà più chi dice le parolacce.
BOT_KICK_BOLDS_ON
	Il bot ora kickerà chi usa il grassetto.
BOT_KICK_BOLDS_ON_BAN
	Il bot ora kickerà chi usa il grassetto, e piazzerà un ban dopo 
	%d kick nei confronti dello stesso utente.
BOT_KICK_BOLDS_OFF
	Il bot non kickerà più chi usa il grassetto.
BOT_KICK_CAPS_ON
	Il bot ora kickerà chi usa le maiuscole (devono costituire almeno
	%d caratteri e il %d%% dell'intero messaggio).
BOT_KICK_CAPS_ON_BAN
	Il bot ora kickerà chi usa le maiuscole (devono costituire almeno
	%d caratteri e il %d%% dell'intero messaggio), e piazzerà un ban
	dopo %d kick nei confronti dello stesso utente.
BOT_KICK_CAPS_OFF
	Il bot non kickerà più chi usa le maiuscole.
BOT_KICK_COLORS_ON
	Il bot ora kickerà chi usa i colori.
BOT_KICK_COLORS_ON_BAN
	Il bot ora kickerà chi usa i colors, e piazzerà un ban dopo %d 
	kick nei confronti dello stesso utente.
BOT_KICK_COLORS_OFF
	Il bot non kickerà più chi usa i colori.
BOT_KICK_FLOOD_ON
	Il bot ora kickerà chi flooda (%d linee in %d secondi).
BOT_KICK_FLOOD_ON_BAN
	Il bot ora kickerà chi flooda (%d linee in %d secondi), e
	piazzerà un ban dopo %d kick nei confronti dello stesso utente.
BOT_KICK_FLOOD_OFF
	Il bot non kickerà più chi flooda.
BOT_KICK_REPEAT_ON
	Il bot ora kickerà chi ripete (gli utenti che ripetono %d
	volte la stessa cosa).
BOT_KICK_REPEAT_ON_BAN
	Il bot ora kickerà chi ripete (gli utenti che ripetono %d
	volte la stessa cosa), e piazzerà un ban dopo %d kick
	nei confronti dello stesso utente.
BOT_KICK_REPEAT_OFF
	Il bot non kickerà più chi ripete.
BOT_KICK_REVERSES_ON
	Il bot ora kickerà chi usa i reverse.
BOT_KICK_REVERSES_ON_BAN
	Il bot ora kickerà chi usa i reverse, e piazzerà un ban dopo %d 
	kick nei confronti dello stesso utente.
BOT_KICK_REVERSES_OFF
	Il bot non kickerà più chi usa i reverse.
BOT_KICK_UNDERLINES_ON
	Il bot ora kickerà chi usa il sottolineato.
BOT_KICK_UNDERLINES_ON_BAN
	Il bot ora kickerà chi usa il sottolineato, e piazzerà un ban dopo %d 
	kick nei confronti dello stesso utente.
BOT_KICK_UNDERLINES_OFF
	Il bot non kickerà più chi usa il sottolineato.
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS canale {ADD|DEL|LIST|CLEAR} [parola | numero-lista] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	Spiacente, la modifica della lista delle parolacce è temporaneamente disabilitata.
BOT_BADWORDS_REACHED_LIMIT
	Spiacente, puoi avere un massimo di %d parolacce nella lista di un canale.
BOT_BADWORDS_ALREADY_EXISTS
	%s è già presente nella lista delle parolacce di %s.
BOT_BADWORDS_ADDED
	%s aggiunto alla lista delle parolacce di %s.
BOT_BADWORDS_NO_SUCH_ENTRY
	Il record specificato (%d) non è presente nella lista delle parolacce di %s.
BOT_BADWORDS_NOT_FOUND
	%s non trovato nella lista delle parolacce di %s.
BOT_BADWORDS_NO_MATCH
	Nessun record corrispondente nella lista delle parolacce di %s.
BOT_BADWORDS_DELETED
	%s è stato eliminato dalla lista delle parolacce di %s.
BOT_BADWORDS_DELETED_ONE
	Eliminato un record dalla lista delle parolacce di %s.
BOT_BADWORDS_DELETED_SEVERAL
	Eliminati %d record dalla lista delle parolacce di %s.
BOT_BADWORDS_LIST_EMPTY
	La lista delle parolacce di %s è vuota.
BOT_BADWORDS_LIST_HEADER
	Lista delle parolacce di %s:
	  Num   Parola                         Tipo
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	La lista delle parolacce è stata svuotata.

# SAY responses
BOT_SAY_SYNTAX
	SAY canale testo

# ACT responses
BOT_ACT_SYNTAX
	ACT canale testo

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
BOT_EXCEPT
	L'utente corrisponde a una delle eccezioni del canale.

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
	I Services non riescono a cambiare i modi. I server sono configurati correttamente?
OPER_BOUNCY_MODES_U_LINE
	I Services non riescono a cambiare i modi. Le U:lines dei server sono configurate correttamente?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL messaggio

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Opzione %s di STATS sconosciuta.
OPER_STATS_CURRENT_USERS
	Utenti correnti: %d (%d operatori)
OPER_STATS_MAX_USERS
	Utenti massimi: %d (%s)
# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	I Services sono up da %d giorni, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	I Services sono up da %d giorno, %02d:%02d
OPER_STATS_UPTIME_HMS
	I Services sono up da %d ore, %d minuti
OPER_STATS_UPTIME_HM1S
	I Services sono up da %d ore, %d minuti
OPER_STATS_UPTIME_H1MS
	I Services sono up da %d ore, %d minuto
OPER_STATS_UPTIME_H1M1S
	I Services sono up da %d ore, %d minuto
OPER_STATS_UPTIME_1HMS
	I Services sono up da %d ora, %d minuti
OPER_STATS_UPTIME_1HM1S
	I Services sono up da %d ora, %d minuti
OPER_STATS_UPTIME_1H1MS
	I Services sono up da %d ora, %d minuto
OPER_STATS_UPTIME_1H1M1S
	I Services sono up da %d ora, %d minuto
OPER_STATS_UPTIME_MS
	I Services sono up da %d minuti, %d secondi
OPER_STATS_UPTIME_M1S
	I Services sono up da %d minuti, %d secondo
OPER_STATS_UPTIME_1MS
	I Services sono up da %d minuto, %d secondi
OPER_STATS_UPTIME_1M1S
	I Services sono up da %d minuto, %d secondo
OPER_STATS_BYTES_READ
	Bytes letti:   %5d kB
OPER_STATS_BYTES_WRITTEN
	Bytes scritti: %5d kB
OPER_STATS_USER_MEM
	Utenti:      %6d records, %5d kB
OPER_STATS_CHANNEL_MEM
	Canali:      %6d records, %5d kB
OPER_STATS_GROUPS_MEM
	Gruppi (NS): %6d records, %5d kB
OPER_STATS_ALIASES_MEM
	Alias (NS):  %6d records, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ:    %6d records, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ:     %6d records, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ:    %6d records, %5d kB
OPER_STATS_SESSIONS_MEM
	Sessioni:    %6d records, %5d kB
OPER_STATS_AKILL_COUNT
	Numero attuale di AKILL: %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Durata di default delle AKILL: %d giorni
OPER_STATS_AKILL_EXPIRE_DAY
	Durata di default delle AKILL: 1 giorno
OPER_STATS_AKILL_EXPIRE_HOURS
	Durata di default delle AKILL: %d ore
OPER_STATS_AKILL_EXPIRE_HOUR
	Durata di default delle AKILL: 1 ora
OPER_STATS_AKILL_EXPIRE_MINS
	Durata di default delle AKILL: %d minuti
OPER_STATS_AKILL_EXPIRE_MIN
	Durata di default delle AKILL: 1 minuto
OPER_STATS_AKILL_EXPIRE_NONE
	Durata di default delle AKILL: infinita
OPER_STATS_SGLINE_COUNT
	Numero attuale di SGLINE: %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Durata di default delle SGLINE: %d giorni
OPER_STATS_SGLINE_EXPIRE_DAY
	Durata di default delle SGLINE: 1 giorno
OPER_STATS_SGLINE_EXPIRE_HOURS
	Durata di default delle SGLINE: %d ore
OPER_STATS_SGLINE_EXPIRE_HOUR
	Durata di default delle SGLINE: 1 ora
OPER_STATS_SGLINE_EXPIRE_MINS
	Durata di default delle SGLINE: %d minuti
OPER_STATS_SGLINE_EXPIRE_MIN
	Durata di default delle SGLINE: 1 minuto
OPER_STATS_SGLINE_EXPIRE_NONE
	Durata di default delle SGLINE: infinita
OPER_STATS_SQLINE_COUNT
	Numero attuale di SQLINE: %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Durata di default delle SQLINE: %d giorni
OPER_STATS_SQLINE_EXPIRE_DAY
	Durata di default delle SQLINE: 1 giorno
OPER_STATS_SQLINE_EXPIRE_HOURS
	Durata di default delle SQLINE: %d ore
OPER_STATS_SQLINE_EXPIRE_HOUR
	Durata di default delle SQLINE: 1 ora
OPER_STATS_SQLINE_EXPIRE_MINS
	Durata di default delle SQLINE: %d minuti
OPER_STATS_SQLINE_EXPIRE_MIN
	Durata di default delle SQLINE: 1 minuto
OPER_STATS_SQLINE_EXPIRE_NONE
	Durata di default delle SQLINE: infinita
OPER_STATS_SZLINE_COUNT
	Numero attuale di SZLINE: %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Durata di default delle SZLINE: %d giorni
OPER_STATS_SZLINE_EXPIRE_DAY
	Durata di default delle SZLINE: 1 giorno
OPER_STATS_SZLINE_EXPIRE_HOURS
	Durata di default delle SZLINE: %d ore
OPER_STATS_SZLINE_EXPIRE_HOUR
	Durata di default delle SZLINE: 1 ora
OPER_STATS_SZLINE_EXPIRE_MINS
	Durata di default delle SZLINE: %d minuti
OPER_STATS_SZLINE_EXPIRE_MIN
	Durata di default delle SZLINE: 1 minuto
OPER_STATS_SZLINE_EXPIRE_NONE
	Durata di default delle SZLINE: infinita
OPER_STATS_RESET
	Le statistiche sono state azzerate.

# MODE responses
OPER_MODE_SYNTAX
	MODE canale modi

# CLEARMODES responses
OPER_UMODE_SYNTAX
	UMODE nick modi

# OLINE responses
OPER_UMODE_SUCCESS
	Modi utente di %s modificati.
OPER_UMODE_CHANGED
	%s ha modificato i tuoi modi utente.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE nick flag
OPER_OLINE_SUCCESS
	Una O:Line con le flag %s è stata aggiunta per %s.

# CLEARMODES responses
OPER_OLINE_IRCOP
	Ora sei un IRC Operator.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES canale [ALL]
OPER_CLEARMODES_DONE
	I Modi binari i ban sono stati annullati sul canale %s.
OPER_CLEARMODES_ALL_DONE
	Tutti i modi sono stat annullati sul canale %s.


# KICK responses
OPER_KICK_SYNTAX
	KICK canale utente motivo
	
# ADMIN responses
OPER_SVSNICK_SYNTAX
	SVSNICK nick newnick 
OPER_SVSNICK_NEWNICK
	The nick %s is now being changed to %s.
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [nick | numero-lista]
OPER_ADMIN_SKELETON
	I Services sono in modalità "scheletro"; il comando ADMIN non è disponibile.
OPER_ADMIN_EXISTS
	%s è già presente nella lista dei Services admin.
OPER_ADMIN_REACHED_LIMIT
	Spiacente, puoi avere un massimo di %d Services admin.
OPER_ADMIN_ADDED
	%s aggiunto alla lista dei Services admin.
OPER_ADMIN_NOT_FOUND
	%s non trovato nella lista dei Services admin.
OPER_ADMIN_NO_MATCH
	Nessun record corrispondente nella lista dei Services admin.
OPER_ADMIN_DELETED
	%s eliminato dalla lista dei Services admin.
OPER_ADMIN_DELETED_ONE
	Eliminato 1 record dalla lista dei Services admin.
OPER_ADMIN_DELETED_SEVERAL
	Eliminati %d record dalla lista dei Services admin.
OPER_ADMIN_LIST_EMPTY
	La lista dei Services admin è vuota.
OPER_ADMIN_LIST_HEADER
	Lista dei Services admin:
	  Num   Nick
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	La lista dei Services admin è stata svuotata.
OPER_ADMIN_MOVED
	%s has been moved to Services Administrators list.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | numero-lista]
OPER_OPER_SKELETON
	I Services sono in modalità "scheletro"; il comando ADMIN non è disponibile.
OPER_OPER_EXISTS
	%s è già presente nella lista dei Services oper.
OPER_OPER_REACHED_LIMIT
	Spiacente, puoi avere un massimo di %d Services oper.
OPER_OPER_ADDED
	%s aggiunto alla lista dei Services oper.
OPER_OPER_NOT_FOUND
	%s non trovato nella lista dei Services oper.
OPER_OPER_NO_MATCH
	Nessun record corrispondente nella lista dei Services oper.
OPER_OPER_DELETED
	%s eliminato dalla lista dei Services oper.
OPER_OPER_DELETED_ONE
	Eliminato 1 record dalla lista dei Services oper.
OPER_OPER_DELETED_SEVERAL
	Eliminati %d record dalla lista dei Services oper.
OPER_OPER_LIST_EMPTY
	La lista dei Services oper è vuota.
OPER_OPER_LIST_HEADER
	Lista dei Services oper:
	  Num   Nick
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	La lista dei Services oper è stata svuotata.
OPER_OPER_MOVED
	%s has been moved to Services Operators list.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+durata] {mask | numero-lista} [motivo]]
OPER_AKILL_EXISTS
	%s è già presente nella lista AKILL.
OPER_AKILL_ALREADY_COVERED
	%s è già coperto da %s.
OPER_AKILL_REACHED_LIMIT
	Spiacente, puoi avere un massimo di %d AKILL.
OPER_AKILL_NO_NICK
	Nota: le mask delle AKILL non possono contenere nick; assicurati di non aver incluso una porzione di nick nella mask.
OPER_AKILL_ADDED
	%s aggiunto alla lista AKILL.
OPER_AKILL_CHANGED
	Durata di %s modificata.
OPER_AKILL_NOT_FOUND
	%s non trovato nella lista AKILL.
OPER_AKILL_NO_MATCH
	Nessun record corrispondente nella lista AKILL.
OPER_AKILL_DELETED
	%s eliminato dalla lista AKILL.
OPER_AKILL_DELETED_ONE
	Eliminato 1 record dalla lista AKILL.
OPER_AKILL_DELETED_SEVERAL
	Eliminati %d record dalla lista AKILL.
OPER_AKILL_LIST_EMPTY
	La lista AKILL è vuota.
OPER_AKILL_LIST_HEADER
	Lista AKILL attuale:
	  Num   Mask                              Motivo
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Lista AKILL attuale:
# number, mask, set-by, set-time, expires, reason
OPER_AKILL_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_AKILL_CLEAR
	La lista AKILL è stata svuotata.
	
# SGLINE responses
OPER_CHANKILL_SYNTAX
	CHANAKILL [+expiry] {#channel} [reason]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+durata] {mask | numero-lista}[:motivo]]
OPER_SGLINE_UNSUPPORTED
	Spiacente, SGLINE non è disponibile su questa rete.
OPER_SGLINE_EXISTS
	%s è già presente nella lista SGLINE.
OPER_SGLINE_ALREADY_COVERED
	%s è già coperto da %s.
OPER_SGLINE_REACHED_LIMIT
	Spiacente, puoi avere un massimo di %d SGLINE.
OPER_SGLINE_ADDED
	%s added to the SGLINE list.
OPER_SGLINE_CHANGED
	Durata di %s modificata.
OPER_SGLINE_NOT_FOUND
	%s non trovato nella lista SGLINE.
OPER_SGLINE_NO_MATCH
	Nessun record corrispondente nella lista SGLINE.
OPER_SGLINE_DELETED
	%s eliminato dalla lista SGLINE.
OPER_SGLINE_DELETED_ONE
	Eliminato 1 record dalla lista SGLINE.
OPER_SGLINE_DELETED_SEVERAL
	Eliminati %d record dalla lista SGLINE.
OPER_SGLINE_LIST_EMPTY
	La lista SGLINE è vuota.
OPER_SGLINE_LIST_HEADER
	Lista SGLINE attuale:
	  Num   Mask                              Motivo
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Lista SGLINE attuale:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SGLINE_CLEAR
	La lista SGLINE è stata svuotata.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+durata] {mask | numero-lista} [motivo]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	Le SQLINE di canale non sono supportate da questo IRCd e sono pertanto inutilizzabili.
OPER_SQLINE_EXISTS
	%s è già prsente nella lista SQLINE.
OPER_SQLINE_ALREADY_COVERED
	%s è già coperto da %s.
OPER_SQLINE_REACHED_LIMIT
	Spiacente, puoi avere un massimo di %d SQLINE.
OPER_SQLINE_ADDED
	%s aggiunto alla lista SQLINE.
OPER_SQLINE_CHANGED
	Durata di %s modificata.
OPER_SQLINE_NOT_FOUND
	%s non trovato nella lista SQLINE.
OPER_SQLINE_NO_MATCH
	Nessun record corrispondente nella lista SQLINE.
OPER_SQLINE_DELETED
	%s eliminato dalla lista SQLINE.
OPER_SQLINE_DELETED_ONE
	Eliminato 1 record dalla lista SQLINE.
OPER_SQLINE_DELETED_SEVERAL
	Eliminati %d record dalla lista SQLINE.
OPER_SQLINE_LIST_EMPTY
	La lista SQLINE è vuota.
OPER_SQLINE_LIST_HEADER
	Lista SQLINE attuale:
	  Num   Mask                              Motivo
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Lista SQLINE attuale:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (da %s su %s; %s)
	      %s
OPER_SQLINE_CLEAR
	La lista SQLINE è stata annullata.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+durata] {mask | numero-lista} [motivo]]
OPER_SZLINE_UNSUPPORTED
	Spiacente, SZLINE non è disponibile su questa rete.
OPER_SZLINE_EXISTS
	%s è già presente sulla lista SZLINE.
OPER_SZLINE_ALREADY_COVERED
	%s è già coperto da %s.
OPER_SZLINE_REACHED_LIMIT
	Spiacente, puoi avere un massimo di %d SZLINE.
OPER_SZLINE_ONLY_IPS
	Nota: puoi aggiungere solo mask dell'ip alla lista SZLINE.
OPER_SZLINE_ADDED
	%s aggiunto alla lista SZLINE.
OPER_SZLINE_CHANGED
	Durata di %s modificata.
OPER_SZLINE_NOT_FOUND
	%s non trovato nella lista SZLINE.
OPER_SZLINE_NO_MATCH
	Nessun record corrispondente nella lista SZLINE.
OPER_SZLINE_DELETED
	%s eliminato dalla lista SZLINE.
OPER_SZLINE_DELETED_ONE
	Eliminato 1 record dalla lista SZLINE.
OPER_SZLINE_DELETED_SEVERAL
	Eliminati %d record dalla lista SZLINE.
OPER_SZLINE_LIST_EMPTY
	La lista SZLINE è vuota.
OPER_SZLINE_LIST_HEADER
	Lista SZLINE attuale:
	  Num   Mask                              Motivo
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Lista SZLINE attuale:
# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SZLINE_CLEAR
	La lista SZLINE è stata svuotata.

# SET responses
OPER_SET_SYNTAX
	SET opzione parametri
OPER_SET_IGNORE_ON
	Il codice di ignore sarà usato.
OPER_SET_IGNORE_OFF
	Il codice di ignore non sarà usato.
OPER_SET_IGNORE_ERROR
	Il parametro dell'opzione IGNORE deve essere ON o OFF.
OPER_SET_READONLY_ON
	I Services ora sono in modalità read-only.
OPER_SET_READONLY_OFF
	I Services ora sono in modalità read-write.
OPER_SET_READONLY_ERROR
	Il parametro dell'opzione READONLY deve essere ON o OFF.
OPER_SET_LOGCHAN_ON
	Services is now reporting log messages to %s.
OPER_SET_LOGCHAN_OFF
	Services is no longer reporting log messages to a channel.
OPER_SET_LOGCHAN_ERROR
	Setting for LOGCHAN must be ON or OFF and LogChannel must be defined.
OPER_SET_DEBUG_ON
	I Services ora sono in modalità debug.
OPER_SET_DEBUG_OFF
	I Services ora non sono in modalità debug.
OPER_SET_DEBUG_LEVEL
	I Services ora sono in modalità debug (livello %d).
OPER_SET_DEBUG_ERROR
	Il parametro dell'opzione DEBUG deve essere ON, OFF, o un numero positivo.
OPER_SET_NOEXPIRE_ON
	I Services ora sono in modalità no expire.
OPER_SET_NOEXPIRE_OFF
	I Services ora sono in modalità expire.
OPER_SET_NOEXPIRE_ERROR
	Il parametro dell'opzione NOEXPIRE deve essere ON o OFF.
OPER_SET_UNKNOWN_OPTION
	Opzione %s sconosciuta.
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
	Tutte le O:lines di %s sono state rimosse.
OPER_NOOP_REVOKE
	Tutte le O:lines di %s sono state ripristinate.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE nomeserver [motivo]

# RAW responses
OPER_JUPE_HOST_ERROR
	Please use a valid server name when juping
	
# RAW responses
OPER_RAW_SYNTAX
	RAW testo

# UPDATE responses
OPER_UPDATING
	Aggiornamento dei database.
	
# RELOAD responses
OPER_RELOAD
	Il file di configurazione dei Services' è stato ricaricato in memoria.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN non è definito; impossibile riavviare. Riesegui lo script \2configure\2 e ricompila i Services per attivare il comando RESTART.

# IGNORE responses
OPER_IGNORE_SYNTAX
	Sintassi: IGNORE {ADD|DEL|LIST|CLEAR} [tempo] [nick]
OPER_IGNORE_VALID_TIME
	Devi inserire un numero valido per indicare il tempo.
OPER_IGNORE_TIME_DONE
	%s sarà ignorato per  %s.
OPER_IGNORE_PERM_DONE
	%s sarà ignorato permanentemente.
OPER_IGNORE_DEL_DONE
	%s non è più ignorato.
OPER_IGNORE_LIST
	Lista ignore dei Services:
OPER_IGNORE_LIST_NOMATCH
	%s non trovato nella lista ignore.
OPER_IGNORE_LIST_EMPTY
	La lista ignore è vuota.
OPER_IGNORE_LIST_CLEARED
	La lista ignore è stata annullata.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	Impossibile trovare l'utente %s.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Lista dei canali:
	Nome                 Utenti Modi    Topic
OPER_CHANLIST_HEADER_USER
	%s channel list:
	Nome                 Utenti Modi    Topic
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	Fine della lista dei canali.
	
# USERLIST responses
OPER_USERLIST_HEADER
	Lista dei nick:
	Nick                 Mask
OPER_USERLIST_HEADER_CHAN
	Lista dei nick di %s:
	Nick                 Mask
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Fine della lista dei nick.

# CACHE responses
OPER_SUPER_ADMIN_ON
	Ora sei un SuperAdmin.
OPER_SUPER_ADMIN_OFF
	Non sei più un SuperAdmin.
OPER_SUPER_ADMIN_SYNTAX
	L'impostazione di SuperAdmin deve essere ON o OFF (deve essere attivato in services.conf).
OPER_SUPER_ADMIN_WALL_ON
	%s ora è un SuperAdmin.
OPER_SUPER_ADMIN_WALL_OFF
	%s non è più un SuperAdmin.

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_SUPER_ADMIN_ONLY
	Solo i SuperAdmin possono usare questo comando.

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
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [parametri]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+durata] mask limite motivo
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {mask | lista}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE num posizione
OPER_EXCEPTION_DISABLED
	Il limite delle sessioni è disabilitato.
OPER_EXCEPTION_ALREADY_PRESENT
	La mask %s è già presente sulla lista delle eccezioni.
OPER_EXCEPTION_TOO_MANY
	La lista delle eccezioni è piena!
OPER_EXCEPTION_ADDED
	Il limite di sessioni per %s è stato impostato a %d.
OPER_EXCEPTION_MOVED
	L'eccezione per %s (numero %d) è stata spostata alla posizione %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Il record specificato (%d) non esiste nella lista delle eccezioni.
OPER_EXCEPTION_NOT_FOUND
	Impossibile trovare %s nella lista delle eccezioni.
OPER_EXCEPTION_NO_MATCH
	Nessun record corrispondente nella lista delle eccezioni.
OPER_EXCEPTION_DELETED
	%s eliminato dalla lista delle eccezioni.
OPER_EXCEPTION_DELETED_ONE
	Eliminato 1 record dalla lista delle eccezioni.
OPER_EXCEPTION_DELETED_SEVERAL
	Eliminati %d record dalla lista delle eccezioni.
OPER_EXCEPTION_LIST_HEADER
	Lista delle eccezioni attuale:
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
	Limite delle sessioni non valido. Deve essere un numero intero maggiore o uguale a zero e minore di %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Hostmask non valida. Solo le hostmask reali sono valide; le eccezioni non vengono confrontate con i nick o con gli username (ident).
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
	La limitazione delle sessioni non è attiva.
OPER_SESSION_INVALID_THRESHOLD
	Valore di soglia non valido. Deve essere un numero intero maggiore di 1.
OPER_SESSION_NOT_FOUND
	%s non trovato nella lista delle sessioni.
OPER_SESSION_LIST_HEADER
	Lista degli host con almeno %d sessioni:
OPER_SESSION_LIST_COLHEAD
	Sessioni  Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	L'host %s ha %d sessioni, con un limite di %d.

# EXCEPTION help

OPER_HELP_EXCEPTION
	Sintassi: EXCEPTION ADD [+durata] mask limite motivo
	          EXCEPTION DEL {mask | lista}
	          EXCEPTION MOVE num posizione
	          EXCEPTION LIST [mask | lista]
	          EXCEPTION VIEW [mask | lista]
	
	Permette ai Services admin di manipolare la lista degli host
	che hanno limiti di sessione specifici, in modo da permettere
	ad alcune macchine, come ad esempio ai server di shell, di
	avere un numero di client maggiore di quello di default.
	Quando un host raggiunge il suo limite di sessioni, tutti i
	client che cercano di connettersi da quell'host verranno
	disconnessi (killati). Prima che l'utente venga killato,
	viene inviato un /NOTICE da %S per fornire delle informazioni
	sulla limitazione delle sessioni. Il contenuto di questo notice
	è impostato nel file di configurazione.
	
	EXCEPTION ADD aggiunge la hostmask specificata alla lista delle
	eccezioni. Bisogna tenere presente che le mask nick!user@host
	e and user@host non sono valide. Soltanto le vere mask di host,
	come ad esempio box.host.dom e *.host.dom, sono permesse,
	poiché la limitazione delle sessioni non tiene conto del nick e
	degli username (ident). limite deve essere un numero maggiore o
	uguale a zero. Questo valore indica quante sessioni contemporanee
	sono permesse a quell'host. Se viene indicato il limite 0, l'host
	non ha limite di sessioni. Guarda l'help del comando AKILL per
	i dettagli sul formato del parametro opzionale durata.
	EXCEPTION DEL rimuove la mask specificata dalla lista delle
	eccezioni.
	EXCEPTION MOVE sposta l'eccezione num alla posizione posizione.
	Le eccezioni che si trovano in mezzo saranno spostate in alto o in
	basso per riempire il vuoto.
	EXCEPTION LIST e EXCEPTION VIEW mostrano tutte le eccezioni
	attuali; se viene specificata la mask opzionale, la lista è limitata
	alle eccezioni che corrispondono alla maschera. La differenza è
	che EXCEPTION VIEW fornisce maggiori informazioni, visualizzando
	il nome della persona che ha aggiunto l'eccezione, il suo limite di
	sessioni, il motivo, la host mask e la data di scadenza.
	
	Bisogna tener conto che un client che si connette "userà" la prima
	eccezione che corrisponde al loro host. Liste di eccezioni molto
	grandi ed mask generiche possono degradare la funzionalità dei
	Services.
	
	Limitato ai Services admin.

OPER_HELP_SESSION
	Sintassi: SESSION LIST soglia
	          SESSION VIEW host
	
	Permette ai Services admins di visualizzare la lista delle sessioni.
	
	SESSION LIST mostra gli host con almeno threshold sessioni.
	La soglia deve essere un numero maggiore di 1, in modo da evitare
	la visualizzazione accidentale di un gran numero di host con una
	sola sessione.
	SESSION VIEW mostra informazioni dettagliate su un host specifico,
	incluso il conteggio attuale delle sessioni e del limite delle stesse.
	Il valore host non può includere wildcards.
	
	Guarda l'help del comando EXCEPTION per maggiori informazioni
	sulla limitazione delle sessioni e come configurare limiti specifici
	per determinati host o gruppi di host.
	
	Limitato ai Services admin.

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
	[Messaggio di ingresso - %s] %s
NEWS_OPER_TEXT
	[Messaggio per gli operatori - %s] %s
NEWS_RANDOM_TEXT
	[Messaggio del giorno - %s] %s

NEWS_LOGON_SYNTAX
	Sintassi: LOGONNEWS {ADD|DEL|LIST} [testo|numero]
NEWS_LOGON_LIST_HEADER
	Lista delle Logon News:
NEWS_LOGON_LIST_ENTRY
	%5d (%s, aggiunto da %s)
	    %s
NEWS_LOGON_LIST_NONE
	Non c'è nessuna Logon News.
NEWS_LOGON_ADD_SYNTAX
	Sintassi: LOGONNEWS ADD testo
NEWS_LOGON_ADD_FULL
	La lista delle news è piena!
NEWS_LOGON_ADDED
	Aggiunta una nuova Logon News (numero %d).
NEWS_LOGON_DEL_SYNTAX
	Sintassi: LOGONNEWS DEL {numero | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Impossibile trovare la Logon News numero %d!
NEWS_LOGON_DELETED
	La Logon News numero %d è stata eliminata.
NEWS_LOGON_DEL_NONE
	Nessuna Logon News da eliminare!
NEWS_LOGON_DELETED_ALL
	Tutte le Logon News sono state eliminate.

NEWS_OPER_SYNTAX
	Sintassi: OPERNEWS {ADD|DEL|LIST} [testo|numero]
NEWS_OPER_LIST_HEADER
	Lista delle Oper News:
NEWS_OPER_LIST_ENTRY
	%5d (%s, aggiunto da %s)
	    %s
NEWS_OPER_LIST_NONE
	Non c'è nessuna Oper News.
NEWS_OPER_ADD_SYNTAX
	Sintassi: OPERNEWS ADD testo
NEWS_OPER_ADD_FULL
	La lista delle news è piena!
NEWS_OPER_ADDED
	Aggiunta una nuova Oper News (numero %d).
NEWS_OPER_DEL_SYNTAX
	Sintassi: OPERNEWS DEL {numero | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Impossibile trovare la Oper News numero %d!
NEWS_OPER_DELETED
	La Oper News numero %d è stata eliminata.
NEWS_OPER_DEL_NONE
	Nessuna Oper News da eliminare!
NEWS_OPER_DELETED_ALL
	Tutte le Oper News sono state eliminate.

NEWS_RANDOM_SYNTAX
	Sintassi: RANDOMNEWS {ADD|DEL|LIST} [testo|numero]
NEWS_RANDOM_LIST_HEADER
	Lista delle Random News:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s, aggiunto da %s)
	    %s
NEWS_RANDOM_LIST_NONE
	Non c'è nessuna Random News.
NEWS_RANDOM_ADD_SYNTAX
	Sintassi: RANDOMNEWS ADD testo
NEWS_RANDOM_ADD_FULL
	La lista delle news è piena!
NEWS_RANDOM_ADDED
	Aggiunta una nuova Random News (numero %d).
NEWS_RANDOM_DEL_SYNTAX
	Sintassi: RANDOMNEWS DEL {numero | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Impossibile trovare la Random News numero %d!
NEWS_RANDOM_DELETED
	La Random News numero %d è stata eliminata.
NEWS_RANDOM_DEL_NONE
	Nessuna Random News da eliminare!
NEWS_RANDOM_DELETED_ALL
	Tutte le Random News sono state eliminate.

NEWS_HELP_LOGON
	Sintassi: LOGONNEWS ADD testo
	          LOGONNEWS DEL {numero | ALL}
	          LOGONNEWS LIST
	
	Modifica o visualizza la lista dei messaggi di ingresso.
	Quando un utente si connette alla rete, gli saranno inviati
	questi messaggi. (In ogni caso, non saranno inviati più di
	tre messaggi per evitare di floodare l'utente. Se ci sono
	più di tre messaggi di ingresso, solo i tre più recenti gli
	saranno inviati).
	
	LOGONNEWS LIST può essere usato da qualsiasi IrcOp per
	visualizzare i messaggi correnti. ADD e DEL possono essere
	utilizzati soltanto dai Services admin.

NEWS_HELP_OPER
	Sintassi: OPERNEWS ADD testo
	          OPERNEWS DEL {numero | ALL}
	          OPERNEWS LIST
	
	Modifica o visualizza la lista dei messaggi per gli operatori.
	Quando un utente diventa operatore (con il comando /OPER), gli
	saranno inviati questi messaggi. (In ogni caso, non saranno
	inviati più di tre messaggi per evitare di floodare l'utente.
	Se ci sono più di tre messaggi, solo i tre più recenti gli
	saranno inviati).
	
	OPERNEWS LIST può essere usato da qualsiasi IrcOp per
	visualizzare i messaggi correnti. ADD e DEL possono essere
	utilizzati soltanto dai Services admin.

NEWS_HELP_RANDOM
	Sintassi: RANDOMNEWS ADD testo
	          RANDOMNEWS DEL {numero | ALL}
	          RANDOMNEWS LIST
	
	Modifica o visualizza la lista delle informazioni del giorno.
	Quando un utente si connette alla rete, una (e una soltanto)
	delle informazioni del giorno sarà scelta casualmente e gli
	sarà inviata.
	
	RANDOMNEWS LIST può essere usato da qualsiasi IrcOp per
	visualizzare i messaggi correnti. ADD e DEL possono essere
	utilizzati soltanto dai Services admin.

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S è un servizio pensato per fornire informazioni sui
	Services. Gli argomenti di aiuto sono accessibili con il
	comando HELP degli altri Services:
	
	/msg %s HELP
	     per informazioni sulla registrazione dei nick
	
	/msg %s HELP
	     per informazioni sulla registrazione e il controllo
	     dei canali
	
	/msg %s HELP
	     per informazioni sull'invio dei messaggi ad utenti
	     non connessi

HELP_HELP_BOT

	/msg %s HELP
	     per informazioni sulla configurazione di un bot
	     per il proprio canale

HELP_HELP_HOST

	/msg %s HELP
	     for information on setting up nick vHosts

###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP
	%S ti permette di "registrare" un nick e di
	evitare che gli altri lo usino. I seguenti
	comandi permettono la registrazione e il mantenimento
	dei nick; per usarli, digita /msg %S comando.
	Per maggiori informazioni su un comando specifico,
	digita /msg %S HELP comando.
	
	    ACCESS     Modifica la lista degli indirizzi autorizzati
	    ALIST      Mostra i canali in cui si ha accesso
	    DROP       Annulla la registrazione del nick
	    GHOST      Disconnette un nick fantasma che non è ancora caduto
	    GLIST      Mostra la lista di tutti i nick nel proprio gruppo
	    GROUP      Entra in un gruppo
	    IDENTIFY   Ti identifica per il tuo nick
	    INFO       Mostra le informazioni su un nick
	    LIST       Mostra l'elenco dei nick registrati
	    LOGOUT     Deidentifica dal proprio nick
	    RECOVER    Disconnette un altro utente che usa il tuo nick
	    REGISTER   Registra un nick
	    RELEASE    Permette di usare di nuovo il proprio nick dopo un RECOVER
	    SENDPASS   Invia la password del proprio nick via e-mail
	    SET        Imposta le opzioni, inclusa la protezione
	    STATUS     Indica lo stato di identificazione di un nick
	
	NOTA: Questo servizio è pensato per fornire agli
	utenti un modo per assicurarsi che loro identità non
	sia compromessa.
	NON è pensato per facilitare il "furto" dei nick
	o per altre azioni maliziose. L'abuso di %S
	risulterà, come minimo, nella perdita del proprio nick.
	
NICK_HELP_EXPIRES
	
	I nick che non sono più usati sono soggetti a
	cancellazione automatica; saranno cioè eliminati
	dopo %d giorni di inutilizzo.

NICK_HELP_REGISTER
	Sintassi: REGISTER password [e-mail]
	
	Registra il tuo nick nel database di %S. Quando il
	nick è registrato, puoi usare i comandi SET e ACCESS
	per modificare a tuo piacimento le impostazioni del tuo
	nick. Assicurati di non dimenticare la password che usi
	per la registrazione - ne avrai bisogno per effettuare
	modifiche sul tuo nick.
	Tieni conto che le maiuscole e le minuscole sono
	differenti! PIPPO, Pippo, pippo e PiPpO sono
	tutte password diverse!
	
	Suggerimenti sulla scelta della password:
	
	Le password non devono essere facili da indovinare. Ad
	esempio, usare il tuo vero nome come password è una
	cattiva idea. Utilizzare il tuo nick come password è
	un'idea anche peggiore ;), infatti %S non ti permetterà
	di farlo. Inoltre, le password brevi sono facili da
	indovinare con meno tentativi, pertanto è meglio usare
	una password di almeno 5 caratteri. Inoltre, lo spazio
	non può essere usato all'interno di una password.
	
	Il parametro e-mail è necessario e serve a memorizzare
	il tuo indirizzo nei parametri del tuo nick.
	Il tuo indirizzo non sarà ceduto a terzi, e sarà usato
	per inviarti la password nel caso tu la dimentichi.
	
	Questo comando crea anche un nuovo gruppo per il tuo nick,
	che ti permetterà di registrare altri nick condividendo
	la stessa configurazione, gli stessi memo e gli stessi
	privilegi sui canali. Per maggiori informazioni su questo
	servizio, digita /msg %S HELP GROUP.
	
NICK_HELP_GROUP
	Sintassi: GROUP destinazione password
	
	Questo comando fa sì che il tuo nick entri nel gruppo del
	nick destinazione. password è la password del nick di
	destinazione.
	
	Entrare in un gruppo ti permetterà di condividere la tua
	configurazione, i tuoi memo e i privilegi sui canali con
	tutti i nick nel tuo gruppo.
	 
	Un gruppo esiste fino a quando è utile. Questo significa
	che anche se uno dei nick del gruppo viene deregistrato,
	non perderai le condivisioni descritte sopra, fino a quando
	nel gruppo resta almeno un nick.
	
	Puoi usare questo comando anche se non hai ancora registrato
	il tuo nick. Se il tuo nick è già registrato, dovrai
	identificarti prima di usare questo comando. Digita
	/msg %S HELP IDENTIFY per maggiori informazioni.
	
	E' consigliato l'uso di questo comando con un nick non
	registrato, in quanto verrà registrato automaticamente
	con l'uso stesso di questo comando.
	
	E' possibile stare in un solo nick per volta. L'unione dei
	gruppi non è possibile
	
	Nota: tutti i nick di un gruppo hanno la stessa password.

NICK_HELP_IDENTIFY
	Sintassi: IDENTIFY password
	
	Indica a %S che sei davvero il proprietario del nick.
	Molti comandi richiedono che tu ti sia autentificato
	con questo comando prima di poterli usare. La password
	deve essere la stessa che è stata utilizzata con il
	comando REGISTER.
	
NICK_HELP_UPDATE
	Syntax: UPDATE

	Updates your current status, i.e. it checks for new memos,
	sets needed chanmodes (ModeonID) and updates your vhost and
	your userflags (lastseentime, etc).
	
NICK_HELP_LOGOUT
	Sintassi: LOGOUT
	
	Annulla l'effetto del comando IDENTIFY, cioè fa in modo
	che tu non sia più riconosciuto come il vero proprietario
	del nick. Tieni presente, però, che non dovrai identificarti
	di nuovo.

NICK_HELP_DROP
	Sintassi: DROP [nick]
	
	Cancella (deregistra) il tuo nick dal database di %S database.
	Un nick deregistrato è a disposizione di chiunque per venire
	registrato nuovamente.
	
	Puoi deregistrare un nick all'interno del tuo gruppo indicandolo
	nel parametro nick.
	
	Per usare questo comando, devi prima identificarti con la tua
	password (digita /msg %S HELP IDENTIFY per avere maggiori
	informazioni).

NICK_HELP_ACCESS
	Sintassi: ACCESS ADD mask
	          ACCESS DEL mask
	          ACCESS LIST
	
	Visualizza o modifica la lista di accesso per il tuo nick.
	Si tratta della lista degli indirizzi che saranno riconosciuti
	automaticamente da %S e che ti permetteranno di utilizzare
	il tuo nick. Se vuoi usare il nick da indirizzi diversi, dovrai
	utilizzare il comando IDENTIFY per farti riconoscere
	da %S.
	
	Esempi:
	
	    ACCESS ADD pippo@*.disney.it
	        Permette l'accesso all'utente con ident pippo da
	        qualsiasi macchina all'interno del dominio disney.it.
	
	    ACCESS DEL pippo@*.disney.it
	        Annulla l'effetto del comando precedente.
	
	    ACCESS LIST
	        Visualizza la lista di accesso attuale.

NICK_HELP_SET
	Sintassi: SET opzione parametri
	
	Imposta varie opzioni del nick. opzione può essere una delle
	seguenti:
	
	    DISPLAY    Imposta il nome del tuo gruppo
	    EMAIL      Associa un indirizzo e-mail al tuo nick
	    GREET      Associa un messaggio di saluto al tuo nick
	    HIDE       Nasconde alcune informazioni sul tuo nick
	    ICQ        Associa un numero ICQ al tuo nick
	    KILL       Attiva o disattiva la protezione
	    LANGUAGE   Imposta la lingua utilizzata dai Services
	    MSG        Cambia il metodo di comunicazione usato dai Services
	    PASSWORD   Imposta la password del tuo nick
	    PRIVATE    Nasconde il nick dalla lista (/msg %S LIST)
	    SECURE     Attiva o disattiva la sicurezza
	    URL        Associa un URL al tuo nick
	
	Per usare questo comando, devi prima identificarti con
	la tua password (digita /msg %S HELP IDENTIFY per
	ottenere maggiori informazioni).
	
	Digita /msg %S HELP SET opzione per avere maggiori
	informazioni su una opzione specifica.

NICK_HELP_SET_DISPLAY
	Sintassi: SET DISPLAY nuovo-nome
	
	Imposta il nome con cui i Services si riferiscono al tuo
	gruppo. Il nuovo nome DEVE essere un nick del gruppo.

NICK_HELP_SET_PASSWORD
	Sintassi: SET PASSWORD nuova-password
	
	Cambia la password utilizzata per identificarti come
	proprietario del nick.

NICK_HELP_SET_LANGUAGE
	Sintassi: SET LANGUAGE numero
	
	Cambia la lingua utilizzata dai Services per inviarti
	i messaggi, ad esempio per rispondere a un comando che
	tu invii.
	numero deve essere scelto dalla lista seguente, che
	elenca le lingue supportate:
	

NICK_HELP_SET_URL
	Sintassi: SET URL url
	
	Associa l'URL indicato al proprio nick. Questo URL
	sarà mostrato quando qualcuno richiede informazioni
	sul proprio nick con il comando INFO.

NICK_HELP_SET_EMAIL
	Sintassi: SET EMAIL indirizzo
	
	Associa l'indirizzo e-mail specificato al proprio nick.
	Questo indirizzo sarà mostrato quando qualcuno richiede
	informazioni sul proprio nick con il comando INFO.

NICK_HELP_SET_ICQ
	Sintassi: SET ICQ numero
	
	Associa il numero ICQ specificato al proprio nick.
	Questo indirizzo sarà mostrato quando qualcuno richiede
	informazioni sul proprio nick con il comando INFO.

NICK_HELP_SET_GREET
	Sintassi: SET GREET messaggio
	
	Imposta come saluto del tuo nick il messaggio specificato,
	che sarà mostrato quando si entra in un canale che ha
	l'opzione GREET attiva, se si ha un livello di accesso
	al canale sufficiente.

NICK_HELP_SET_KILL
	Sintassi: SET KILL {ON | QUICK | IMMED | OFF}
	
	Attiva o disattiva l'opzione di protezione automatica per
	il tuo nick. Quando la protezione è attiva, se un altro
	utente cerca di usare il tuo nick, avrà un minuto per
	cambiarlo, dopo del quale %S gli assegnerà un nick casuale.
	
	Se scegli QUICK, l'utente avrà soltanto 20 secondi per
	cambiare nick invece di 60. Se scegli IMMED, il nick
	sarà cambiato immediatamente, senza essere prima avvisato
	per dargli la possibilità di cambiare nick; non usare questa
	opzione se non è strettamente necessario. L'opzione IMMED,
	inoltre, potrebbe non essere disponibile.

NICK_HELP_SET_SECURE
	Sintassi: SET SECURE {ON | OFF}
	
	Attiva o disattiva sul tuo nick le funzionalità di sicurezza
	di %S. Quando SECURE è attivo, devi identificarti come
	proprietario del nick, anche se il tuo indirizzo compare
	nella lista di accesso. Comunque, se sei nella lista di
	accesso, %S non cambierà il tuo nick, anche se l'opzione KILL
	è attiva.

NICK_HELP_SET_PRIVATE
	Sintassi: SET PRIVATE {ON | OFF}
	
	Attiva o disattiva l'opzione privacy di %S per il tuo
	nick. Quando PRIVATE è attivo, il tuo nick non comparirà nelle
	liste dei nick generate con %S comando LIST. In ogni caso, però,
	chiunque sappia il tuo nick può leggerne le informazioni con il
	comando INFO.

NICK_HELP_SET_HIDE
	Sintassi: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Ti permette di nascondere alcune informazioni quando qualcuno
	effettua un %S INFO sul tuo nick. Puoi nascondere il tuo
	indirizzo e-mail (EMAIL), l'ultima hostmask di accesso
	(USERMASK) e l'ultimo messaggio di quit (QUIT). Il secondo
	parametro specifica se l'informazione indicata debba essere
	mostrata (OFF) o nascosta (ON).

NICK_HELP_SET_MSG
	Sintassi: SET MSG {ON | OFF}
	
	Ti permette di scegliere il modo in cui i Services comunicano
	con te. Se MSG è attivo, i Services usano i messaggi (query),
	altrimenti usano i notices.

NICK_HELP_RECOVER
	Sintassi: RECOVER nick [password]
	
	Ti permette di recuperare il tuo nick se qualcun altro
	lo sta usando; si tratta della stessa cosa che %S
	fa automaticamente se qualcuno cerca di usare un nick
	con la protezione (kill) attivata.
	
	Quando usi questo comando, %S farà connettere un utente
	fittizio il nick specificato, in modo da disconnettere
	l'altro utente. Questo utente fittizio resterà online per
	un minuto, in modo che che l'altro utente non si riappropri
	immediatamente del tuo nick; dopo quel minuto, puoi tornare
	ad usare il tuo nick. In alternativa, puoi usare il comando
	RELEASE (/msg %S HELP RELEASE) per recuperare il tuo
	nick prima che passi un minuto.
	
	Per usare il comando RECOVER su un nick, il tuo indirizzo
	attuale (così come compare nel /WHOIS) deve trovarsi nella
	lista di accesso di quel nick, devi essere identificato e
	nello stesso gruppo di quel nick, o devi fornire la password
	corretta per quel nick.

NICK_HELP_RELEASE
	Sintassi: RELEASE nick [password]
	
	Indica a %S di rimuovere il blocco sul tuo nick causato
	dalla protezione automatica (kill) o dall'uso del comando
	RECOVER. Questi blocchi durano un minuto; questo comando
	ti permette di rimuoverli prima.
	
	Per usare il comando RELEASE su un nick, il tuo indirizzo
	attuale (così come compare nel /WHOIS) deve trovarsi nella
	lista di accesso di quel nick, devi essere identificato e
	nello stesso gruppo di quel nick, o devi fornire la password
	corretta per quel nick.

NICK_HELP_GHOST
	Sintassi: GHOST nick [password]
	
	Disconnette un nick "fantasma" con il tuo nick. Un nick
	"fantasma" è un nick che non è più connesso realmente,
	ma che è ancora su IRC. Questo succede, di solito, se
	il tuo computer ha problemi o se vieni disconnesso mentre
	sei su IRC. Con questo comando, si può fare in modo che
	il nick venga disconnesso senza dover attendere che il
	server lo disconnetta per "ping timeout".

	Per usare il comando GHOST su un nick, il tuo indirizzo
	attuale (così come compare nel /WHOIS) deve trovarsi nella
	lista di accesso di quel nick, devi essere identificato e
	nello stesso gruppo di quel nick, o devi fornire la password
	corretta per quel nick.

NICK_HELP_INFO
	Sintassi: INFO nick [ALL]
	
	Mostra varie informazioni sul nick specificato, come ad
	esempio il proprietario del nick, la data dell'ultimo
	accesso, l'ultimo indirizzo utilizzato e le opzioni stesse
	del nick. Se sei identificato per il nick di cui stai
	richiedendo le informazioni, e specifichi il parametro
	ALL, vedrai tutte le informazioni, che siano nascoste
	o meno.

NICK_HELP_LIST
	Sintassi: LIST pattern
	
	Mostra tutti i nick registrati che corrispondono al
	pattern specificato, nel formato nick!user@host.
	I nick che hanno attivato l'opzione PRIVATE non
	saranno mostrati.
	
	Esempi:
	
	    LIST *!pluto@topolino.disney.it
	        Mostra tutti i nick che appartengono a
	        pluto@topolino.disney.it.
	
	    LIST *bot*!*@*
	        Mostra tutti i nick registrati che contengono bot
	        (maiuscole e minuscole non fanno differenza).
	
	    LIST *!*@*.azienda.it
	        Mostra tutti i nick registrati da utenti del dominio
	        bar.org.

NICK_HELP_ALIST
	Sintassi: ALIST [livello]
	
	Mostra tutti i canali su cui hai accesso. Opzionalmente,
	puoi specificare un livello nel formato XOP o numerico.
	La lista risultante includerà soltanto i canali su cui
	hai il livello di accesso indicato o superiore.
	
	Esempi:
	    ALIST Founder
	        Mostra tutti i canali in cui hai livello di
	    fondatore.
	
	    ALIST AOP
	        Mostra tutti i canali in cui hai livello di
	    AOP o superiore.
	
	    ALIST 10
	        Mostra tutti i canali in cui hai livello 10
	    o superiore.
	
	I canali che hanno l'opzione NOEXPIRE impostata verranno
	mostrati con un punto esclamativo.
	
NICK_HELP_GLIST
	Sintassi: GLIST
	
	Mostra tutti i nick del tuo gruppo.

NICK_HELP_STATUS
	Sintassi: STATUS nick...
	
	Indica se l'utente che usa il nick specificato è riconosciuto
	come proprietario del nick. La risposta ha questo formato:
	
	    nick codice-di-stato
	
	dove nick è il nick inviato con il comando, e codice-di-stato
	può essere:
	
	    0 - l'utente non è online o il nick non è registrato
	    1 - l'utente non è riconosciuto come proprietario del nick
	    2 - l'utente è riconosciuto come proprietario solo attraverso
	        la lista di accesso
	    3 - l'utente è riconosciuto come proprietario attravrso
	        l'identificazione per mezzo della password
	
	Con ogni comando possono essere inviati fino a 16 nick; il
	resto sarà ignorato. If no nickname is given, your status
	will be returned.

NICK_HELP_SENDPASS
	Sintassi: SENDPASS nick
	
	Invia la password del nick specificato all'indirizzo e-mail
	impostato nelle opzioni del nick stesso. Questo comando è
	utile quando un utente ha dimenticato la password.
	
	Potrebbe essere disponibile soltanto per gli IrcOp.
	
	Questo comando non è utilizzabile quando la criptazione è attiva.

NICK_SERVADMIN_HELP
	
	I seguenti comandi sono disponibili per i Services admin:
	
	    GETPASS    Recupera la password di un nick
	                   (solo se la criptazione è disattivata)
	    FORBID     Blocca un nick in modo che non venga usato
	
	I Services admin possono inoltre deregistrare qualsiasi nick
	senza doversi identificare per quel nick, e possono visualizzare
	le liste di accesso per qualunque nick (/msg %S ACCESS LIST nick).

NICK_SERVADMIN_HELP_LOGOUT
	Sintassi: LOGOUT [nick [REVALIDATE]]
	
	Senza nessun parametro, annulla l'effetto del comando IDENTIFY,
	rendendoti non più identificato come proprietario del nick.
	Da notare, comunque, che non ti sarà richiesto di identificarti
	di nuovo.
	
	Con un parametro, il funzionamento è identico ma viene applicato
	al nick specificato. Se si indica anche REVALIDATE, i Services
	chiederanno al nick specificato di reidentificarsi. Questo
	modo di utilizzo è limitato ai Services admin.

NICK_SERVADMIN_HELP_DROP
	Sintassi: DROP [nick]
	
	Senza parametro, annulla (deregistra) il tuo nick dal
	database di %S.
	
	Con un parametro, deregistra il nick specificato dal
	database. Puoi deregistrare qualsiasi nick del tuo
	gruppo senza aver bisogno di privilegi speciali.
	La deregistrazione di qualsiasi nick è limitata ai
	Services admin.

NICK_SERVADMIN_HELP_SET
	
	I Services admin possono inoltre impostare l'opzione
	NOEXPIRE, con cui i nick non scadono. Inoltre, i
	Services admin possono impostare le opzioni per qualsiasi
	nick senza specificarne la password, usando il formato
	SET nick opzione parametri.

NICK_SERVADMIN_HELP_SET_NOEXPIRE
	Sintassi: SET [nick] NOEXPIRE {ON | OFF}
	
	Imposta se il nick specificato scadrà. Se l'opzione è
	impostata a ON, il nick non scadrà. Se non è specificato
	alcun nick, l'opzione viene applicata al proprio.
	
	Limitato ai Services admin.

NICK_SERVADMIN_HELP_INFO
	
	I Services admin possono usare il parametro ALL con tutti i nick.

NICK_SERVADMIN_HELP_LIST
	Sintassi: LIST pattern [FORBIDDEN] [NOEXPIRE] [UNCONFIRMED]
	
	Mostra tutti i nick registrati che corrispondono al
	pattern specificato, nel formato nick!user@host.
	I nick con l'opzione PRIVATE attiva saranno mostrati
	solo ai Services admin. I nick con l'opzione NOEXPIRE
	attiva avranno un ! prima del nick stesso (solo per i
	Services admin).
	
	If the FORBIDDEN, NOEXPIRE or UNCONFIRMED options are given, only
	nicks which, respectively, are FORBIDden, UNCONFIRMED or have the
	NOEXPIRE flag set will be displayed. If all three options are
	given, all three types of nicks will be displayed. These options
	are limited to Services admins.  
	
	Esempi:
	
	    LIST *!joeuser@foo.com
	        Mostra tutti i nick che appartengono a
	        joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Mostra tutti i nick registrati che contengono 
	        Bot (case insensitive).
	
	    LIST * NOEXPIRE
	        Mostra tutti i nick registrati che non
	        scadono.

NICK_SERVADMIN_HELP_ALIST
	Sintassi: ALIST [nick] [livello]
	
	Senza parametri, mostra i canali su cui hai accesso.
	Con un parametro, mostra i canali su cui ha accesso
	l'utente nick. Con due parametri, mostra i canali
	su cui nick ha livello di accesso livello o
	superiore.

	Questo modo d'uso è limitato ai Services admin.

NICK_SERVADMIN_HELP_GLIST
	Sintassi: GLIST [nick]
	
	Senza parametro, mostra tutti i nick del tuo gruppo.
	
	Con un parametro, mostra tutti i nick del gruppo del
	nick specificato.
	Questo modo d'uso è riservato ai Services admin.

NICK_SERVADMIN_HELP_GETPASS
	Sintassi: GETPASS nick
	
	Restituisce la password del nick specificato.
	Da notare che quando questo comando viene usato,
	verrà inviato un WALLOPS o GLOBOPS ad indicare la
	persona che l'ha utilizzato e su quale nick è stato
	utilizzato.
	
	Limitato ai Services admin.
	
	Questo comando non è disponibile quando è attiva la
	criptazione dei database.

NICK_SERVADMIN_HELP_GETEMAIL
	Syntax: GETEMAIL user@emailhost

	Returns the matching nicks that used given email. Note that
	you can not use wildcards for either user or emailhost. Whenever
	this command is used, a message including the person who issued
	the command and the email it was used on will be logged.

	Limited to Services admins.

NICK_SERVADMIN_HELP_FORBID
	Sintassi: FORBID nick [motivo]
	
	Fa sì che un nick non possa venire registrato o usato
	da nessuno. Può essere annullato deregistrando il nick
	con il comando DROP.
	
	Su alcune reti può essere necessario specificare il motivo.
	
	Limitato ai Services admin.

###########################################################################
#
# ChanServ help messages
#
###########################################################################

CHAN_HELP
	%S permette di registrare i canali e di controllarne
	vari aspetti. %S può spesso evitare che utenti non
	autorizzati prendano il controllo dei canali ("take over")
	limitandone l'accesso agli utenti. I comandi disponibili
	sono indicati in basso; per usarli, puoi digitare
	/msg %S comando. Per avere maggiori informazioni su
	un comando specifico, digita /msg %S HELP comando.
	
	    ACCESS     Modifica la lista degli utenti con privilegi
	    AKICK      Modifica la lista AutoKick
	    AOP        Modifica la lista degli utenti AOP
	    BAN        Permette di bannare un utente
	    CLEAR      Annulla varie impostazioni del canale
	    DEOP       Toglie lo stato di operatore a un utente (-o)
	    DEVOICE    Toglie lo stato di voice a un utente (-v)
	    DROP       Annulla la registrazione di un canale
	    GETKEY     Recupera la password (chiave) di un canale
	    IDENTIFY   Ti identifica come fondatore di un canale
	    INFO       Mostra le informazioni su un canale
	    INVITE     Richiede un invito per un canale
	    KICK       Espelle un utente da un canale (kick)
	    LEVELS     Ridefinisce i privilegi dei livelli di accesso
	    LIST       Mostra la lista dei canali
	    LOGOUT     Deidentifica dal canale specificato
	    OP         Assegna lo stato di operatore a un utente (+o)
	    REGISTER   Registra un canale
	    SENDPASS   Invia la password del proprio canale via e-mail
	    SET        Imposta le opzioni e le informazioni di canale
	    SOP        Modifica la lista degli utenti SOP
	    TOPIC      Cambia il topic di un canale
	    UNBAN      Rimuove i ban che impediscono l'accesso a un canale
	    VOICE      Assegna lo stato di voice a un utente (+v)
	    VOP        Modifica la lista degli utenti VOP

CHAN_HELP_UNREAL
	
	Su questa rete sono inoltre disponibili questi comandi:
	    DEHALFOP   Toglie lo stato di mezzo operatore a un utente (-h)
	    DEOWNER    Toglie lo stato di owner a un utente (-q)
	    DEPROTECT  Toglie lo stato di protezione a un utente (-a)
	    HALFOP     Assegna lo stato di mezzo operatore a un utente (+h)
	    HOP        Modifica la lista degli utenti HOP
	    OWNER      Assegna lo stato di owner a un utente (+q)
	    PROTECT    Assegna lo stato di protezione a un utente (+a)

CHAN_HELP_ULTIMATE3
	
	Su questa rete sono inoltre disponibili questi comandi:
	    DEHALFOP, HALFOP, HOP, ADMIN, DEADMIN

CHAN_HELP_IRCD_HALFOP
	
	These commands are also available on this network:
	    DEHALFOP, HALFOP, HOP

CHAN_HELP_IRCD_PROTECT
	
	These commands are also available on this network:
	    DEPROTECT, PROTECT

CHAN_HELP_EXPIRES
	
	Tieni conto che i canali che non vengono utilizzati per %d
	giorni (cioè in quei canali in cui non entra nessun utente
	che si trova nella relativa lista di accesso) saranno
	automaticamente deregistrati.

CHAN_HELP_REGISTER
	Sintassi: REGISTER canale password descrizione
	
	Registra un canale nel database di %S. Per poter usare
	questo comando, devi essere operatore di canale (op) sul
	canale che intendi registrare. La password è usata con
	il comando IDENTIFY per permettere agli altri di
	effettuare in seguito dei cambiamenti sulle varie
	impostazioni di canale. L'ultimo parametro, che deve
	essere incluso, è una descrizione generica del canale
	stesso.
	
	Quando registri un canale, vieni registrato come il
	"fondatore" (founder) del canale. Il fondatore di un
	canale può cambiare tutte le impostazioni del canale
	stesso; inoltre, %S gli assegnerà automaticamente i
	privilegi di operatore di canale (+o) quando entra nel
	canale.
	Per assegnare alcuni di questi privilegi ad altri
	utenti, vedi il comando ACCESS (/msg %S HELP ACCESS).
	
	NOTA: Per registrare un canale, devi aver prima
	registrato il tuo nick. Se non l'hai fatto, digita
	/msg %s HELP per avere informazioni su come farlo.
	
	ATTENZIONE! Il nome del canale deve includere il
	cancelletto (#) all'inizio!

CHAN_HELP_IDENTIFY
	Sintassi: IDENTIFY canale password
	
	Ti identifica con %S come fondatore del canale specificato.
	Molti comandi richiedono che tu sia identificato per
	poter essere eseguiti. La password deve essere la stessa
	utilizzata con il comando REGISTER.
	
CHAN_HELP_LOGOUT
	Sintassi: LOGOUT canale nick
	
	Annulla l'effetto del comando IDENTIFY per il canale
	specificato, cioè fa in modo che tu non sia più riconosciuto
	per quel canale.
	
	Se sei il fondatore del canale puoi deidentificare chiunque,
	altrimenti puoi deidentificare solo te stesso.

CHAN_HELP_DROP
	Sintassi: DROP canale
	
	Deregistra il canale specificato. Può essere usato soltanto
	da un fondatore di canale, che deve prima usare il comando
	IDENTIFY.

CHAN_HELP_SET
	Sintassi: SET canale opzione parametri
	
	Permette al fondatore di un canale di impostare varie
	opzioni e informazioni del canale stesso.
	
	Opzioni disponibili:
	
	    FOUNDER       Imposta il fondatore del canale
	    SUCCESSOR     Imposta il successore del fondatore del canale
	    PASSWORD      Imposta la password del canale
	    DESC          Imposta la descrizione del canale
	    URL           Associa un URL al canale
	    EMAIL         Associa un indirizzo e-mail al canale
	    ENTRYMSG      Imposta un messaggio che sarà inviato agli utenti
	                     che entrano nel canale
	    BANTYPE       Imposta il tipo di ban che sarà usato dai Services
	    MLOCK         Imposta il blocco dei modi di canale
	    KEEPTOPIC     Mantiene il topic quando il canale non è in uso
	    OPNOTICE      Invia una notifica quando vengono usati i comandi
	                     OP/DEOP
	    PEACE         Regola l'uso dei comandi critici
	    PRIVATE       Nasconde il canale dall'output del comando LIST
	    RESTRICTED    Limita l'accesso al canale
	    SECURE        Attiva le funzionalità di sicurezza di %S
	    SECUREOPS     Abilita un controllo più severo dello stato di
	                     operatore
	    SECUREFOUNDER Abilita un controllo più severo dello stato di
	                     fondatore
	    SIGNKICK      "Firma" i kick effettuati con il comando KICK
	    TOPICLOCK     Imposta il blocco del topic (modificabile solo
	                     attraverso il comando TOPIC)
	    XOP           Seleziona il sistema di privilegi utilizzato
	
	Digita /msg %S HELP option per ottenere maggiori
	informazioni su un comando specifico.

CHAN_HELP_SET_FOUNDER
	Sintassi: SET canale FOUNDER nick
	
	Imposta il nuovo fondatore del canale. Il nick specificato
	deve essere registrato.

CHAN_HELP_SET_SUCCESSOR
	Sintassi: SET canale SUCCESSOR nick
	
	Imposta il successore del fondatore del canale. Se il nick
	del fondatore scade, o viene deregistrato mentre il canale
	è ancora registrato, il successore diventerà il nuovo
	fondatore del canale. In ogni caso, se il succsesore ha già
	troppi canali registrati (il massimo è %d), il canale verrà
	invece deregistrato, come se non ci fosse stato nessun
	successore.
	Il nick specificato deve essere registrato.

CHAN_HELP_SET_PASSWORD
	Sintassi: SET canale PASSWORD password
	
	Imposta la password del canale, utilizzata per essere
	identificati come fondatori del canale stesso.

CHAN_HELP_SET_DESC
	Sintassi: SET canale DESC descrizione
	
	Imposta la descrizione del canale, che viene mostrata
	con i comandi LIST e INFO.

CHAN_HELP_SET_URL
	Sintassi: SET canale URL [url]
	
	Associa l'URL specificato al canale. Questo URL sarà
	mostrato quando qualcuno richiede informazioni relative
	al canale con il comando INFO. Se non viene specificato
	nessun URL, l'URL attuale del canale viene eliminato.

CHAN_HELP_SET_EMAIL
	Sintassi: SET canale EMAIL [indirizzo]
	
	Associa l'indirizzo e-mail specificato al canale. Questo
	indirizzo sarà mostrato quando qualcuno richiede
	informazioni relative al canale con il comando INFO. Se
	non viene specificato nessun indirizzo, l'indirizzo attuale
	del canale viene eliminato.

CHAN_HELP_SET_ENTRYMSG
	Sintassi: SET canale ENTRYMSG [messaggio]
	
	Imposta il messaggio che sarà inviato con un /notice
	agli utenti che entrano nel canale. Se non viene
	specificato nessun messaggio, il messaggio attuale
	viene eliminato, e gli utenti che entrano non
	riceveranno alcun messaggio.

CHAN_HELP_SET_BANTYPE
	Sintassi: SET canale BANTYPE tipo
	
	Imposta il tipo di can che sarà usato dai Services
	in tutte le occasioni in cui dovranno bannare
	qualcuno dal canale.
	
	tipo è un numero tra 0 e 3 che indica:
	
	0: ban nella forma *!user@host
	1: ban nella forma *!*user@host
	2: ban nella forma *!*@host
	3: ban nella forma *!*user@*.dominio

CHAN_HELP_SET_KEEPTOPIC
	Sintassi: SET canale KEEPTOPIC {ON | OFF}
	
	Attiva o disattiva il mantenimento del topic per
	il canale. Quando il mantenimento del topic è
	attivo, il topic sarà memorizzato da %S anche quando
	l'ultimo utente esce dal canale, e sarà reimpostato
	la volta successiva che entra qualcuno.

CHAN_HELP_SET_TOPICLOCK
	Sintassi: SET canale TOPICLOCK {ON | OFF}
	
	Attiva o disattiva il blocco del topic per il
	canale. Quando il blocco del topic è attivo,
	%S non permetterà che il topic venga cambiato, se
	non attraverso il comando TOPIC.

CHAN_HELP_SET_MLOCK
	Sintassi: SET canale MLOCK modi
	
	Imposta il blocco dei modi per il canale. %S
	permette di definire vari modi di canale che saranno
	sempre attivi, sempre disattivi, o non bloccati.
	
	Il parametro modi è costituito esattamente nella
	stessa maniera di un comando /MODE; vale a dire
	che i modi che seguono un + sono sempre attivi, e
	i modi che seguono un - sono sempre disattivi.
	Diversamente dal comando /MODE, però, ogni uso del
	comando SET MLOCK rimuove tutti i modi bloccati
	precedentemente!
	
	Attenzione:  Se imposti una password (chiave) bloccata,
	come nel secondo esempio in basso, dovresti anche impostare
	l'opzione RESTRICTED per il canale (per maggiori
	informazioni, digita HELP SET RESTRICTED),
	altrimenti chiunque entra nel canale quando è vuoto potrà
	vedere la chiave!
	
	Esempi:
	
	    SET #canale MLOCK +nt-iklps
	        Blocca come attivi i modi n e t, e come disattivi i
	        modi i, k, l, p e s. Il modo m può essere cambiato
	        dagli utenti.
	
	    SET #canale MLOCK +knst-ilmp miachiave
	        Blocca come attivi i modi k, n, s e t, e come
	        disattivi i modi i, l, m e p. Inoltre, blocca la
	        chiave del canale impostandola a "miachiave".
	
	    SET #canale MLOCK +
	        Rimuove il blocco dei modi; tutti i modi del canale
	        possono essere attivati o disattivati dagli utenti.

CHAN_HELP_SET_PEACE
	Sintassi: SET canale PEACE {ON | OFF}
	
	Attiva o disattiva l'opzione "pace" per il canale.
	Quando l'opzione è attiva, gli utenti non potranno
	kickare, bannare o rimuovere uno stato di canale
	(op, mezzo op, ecc.) di un utente con livello
	superiore, attraverso i comandi di %S.

CHAN_HELP_SET_PRIVATE
	Sintassi: SET canale PRIVATE {ON | OFF}
	
	Attiva o disattiva l'opzione di privacy per il canale.
	Quando questa opzione è attiva, il canale non sarà
	incluso nella lista restituita da /msg %S LIST.

CHAN_HELP_SET_RESTRICTED
	Sintassi: SET canale RESTRICTED {ON | OFF}
	
	Attiva o disattiva l'accesso limitato (restricted)
	per il canale. Quando questa opzione è attiva, gli
	utenti che non potrebbero avere i privilegi di operatore
	di canale (gli utenti con livelli di accesso negativi e,
	se l'opzione secure ops è attiva, tutti gli utenti che
	non si trovano in alcuna lista di accesso) verrano invece
	kickati e bannati dal canale.

CHAN_HELP_SET_SECURE
	Sintassi: SET canale SECURE {ON | OFF}
	
	Attiva o disattiva le funzionalità di sicurezza di
	%S per il canale. Quando SECURE è attivo,
	solo gli utenti che hanno registrato il proprio nick
	con %s e che si sono identificati con la loro password
	avranno l'accesso sul canale secondo le liste di accesso.

CHAN_HELP_SET_SECUREOPS
	Sintassi: SET canale SECUREOPS {ON | OFF}
	
	Attiva o disattiva l'opzione secure ops per il canale.
	Quando l'opzione è attiva, gli utenti che non sono nella
	lista di accesso non potranno avere lo stato di operatore
	di canale.
	
CHAN_HELP_SET_SECUREFOUNDER
	Sintassi: SET canale SECUREFOUNDER {ON | OFF}
	
	Attiva o disattiva l'opzione secure founder per il
	canale. Quando l'opzione è attiva, solo il vero fondatore
	potrà deregistrare il canale (drop), cambiarne la password,
	il fondatore e il successore, mentre gli utenti identificati
	su %S per il canale non potranno.

CHAN_HELP_SET_SIGNKICK
	Sintassi: SET canale SIGNKICK {ON | LEVEL | OFF}
	
	Attiva o disattiva i nick "firmati" per il canale. Quando
	l'opzione è attiva, i kick effettuati con il comando KICK
	di %S indicheranno anche il nick della persona che ha
	utilizzato il comando.
	
	Se usi LEVEL, il nick non sarà comunque mostrato se il
	kick è effettuato da qualcuno il cui livello di accesso
	è uguale o maggiore al livello SIGNKICK del canale.
	Per maggiori informazioni, digita /msg %S HELP LEVELS.

CHAN_HELP_SET_XOP
	Sintassi: SET canale XOP {ON | OFF}
	
	Attiva o disattiva il sistema di liste xOP per il canale.
	Quando XOP è attivo, sarà necessario usare i comandi
	SOP/AOP/HOP/VOP per dare i privilegi sul canale
	agli utenti, altrimenti si dovrà usare il comando ACCESS.
	
	Nota tecnica: quando passi dalle liste di accesso al
	sistema di liste xOP, le definizioni dei livelli e i livelli
	personalizzati saranno cambiati, quindi non troverai gli
	stessi valori se poi torni di nuovo alle liste di accesso!
	
	Dopo essere passato dalle liste di accesso alle liste xOP,
	devi controllare che gli utenti siano nelle liste giuste,
	perché la conversione non sempre è perfetta, in particolare
	nel caso in cui si abbiano livelli personalizzati.
	
	Al contrario, passare dalle liste xOP alle liste di accesso
	non causa nessun problema.

CHAN_HELP_SET_OPNOTICE
	Sintassi: SET canale OPNOTICE {ON | OFF}
	
	Attiva o disattiva l'opzione op-notice per il canale.
	Quando l'opzione è attiva, %S invierà un notice al
	canale quando vengono usati i comandi OP e DEOP.

CHAN_HELP_AOP
	Sintassi: AOP canale ADD nick
	          AOP canale DEL {nick | numero | lista}
	          AOP canale LIST [mask | lista]
	          AOP canale CLEAR
	
	Gestisce la lista AOP (AutoOP) del canale.
	La lista AOP dà agli utenti il diritto di essere auto-oppati,
	di richiedere l'invito, di rimuovere i ban su se stessi, di
	far mostrare il messaggio di saluto (greet) quando entrano,
	e così via.
	
	Il comando AOP ADD aggiunge il nick specificato alla
	lista AOP.
	
	Il comando AOP DEL rimuove il nick specificato dalla
	lista AOP. Se viene specificata una lista, i record
	corrispondenti vengono rimossi. (Guarda l'esempio di
	LIST)
	
	Il comando AOP LIST mostra la lista AOP. Se viene
	utilizzata una wildcard per la mask, solo i record che
	corrispondono alla mask vengono mostrati. Se viene
	specificata una lista, i record corrispondenti vengono
	mostrati.
	Ad esempio:
	
	   AOP #canale LIST 2-5,7-9
	      Mostra i record AOP che hanno numero da 2
	      a 5 e da 7 a 9 (il 6 è quindi escluso).
	      
	Il comando AOP CLEAR svuota la lista AOP.
	
	I comandi AOP ADD e AOP DEL sono limitati ai SOP
	e superiori, mentre il comando AOP CLEAR può essere
	utilizzato soltanto dal fondatore del canale. Comunque,
	tutti gli utenti nella lista AOP possono usare il comando
	AOP LIST.
	
	Questo comando potrebbe essere stato disabilitato per
	il tuo canale; in questo caso dovrai usare la lista di
	accesso. Digita /msg %S HELP ACCESS per
	maggiori informazioni, e /msg %S HELP SET XOP
	per scoprire come passare da un sistema all'altro.

CHAN_HELP_HOP
	Sintassi: HOP channel ADD nick
	          HOP channel DEL {nick | numero | lista}
	          HOP channel LIST [mask | lista]
	          HOP channel CLEAR
	
	Gestisce la lista HOP (HalfOP) del canale.
	La lista HOP dà agli utenti il diritto di essere
	"half-oppati" sul tuo canale.
	
	Il comando HOP ADD aggiunge il nick specificato alla
	lista HOP.
	
	Il comando HOP DEL rimuove il nick specificato dalla
	lista HOP. Se viene specificata una lista, i record
	corrispondenti vengono rimossi. (Guarda l'esempio di
	LIST)
	
	Il comando HOP LIST mostra la lista HOP. Se viene
	utilizzata una wildcard per la mask, solo i record che
	corrispondono alla mask vengono mostrati. Se viene
	specificata una lista, i record corrispondenti vengono
	mostrati.
	Ad esempio:
	
	   HOP #canale LIST 2-5,7-9
	      Mostra i record HOP che hanno numero da 2
	      a 5 e da 7 a 9 (il 6 è quindi escluso).
	
	Il comando HOP CLEAR svuota la lista HOP.
	
	I comandi HOP ADD, HOP DEL e HOP LIST sono limitati
	agli AOP e superiori, mentre il comando HOP CLEAR può
	essere utilizzato soltanto dal fondatore del canale.
	
	Questo comando potrebbe essere stato disabilitato per
	il tuo canale; in questo caso dovrai usare la lista di
	accesso. Digita /msg %S HELP ACCESS per
	maggiori informazioni, e /msg %S HELP SET XOP
	per scoprire come passare da un sistema all'altro.

CHAN_HELP_SOP
	Sintassi: SOP canale ADD nick
	          SOP canale DEL {nick | numero | lista}
	          SOP canale LIST [mask | lista]
	          SOP canale CLEAR
	
	Gestisce la lista SOP (SuperOP) del canale.
	La lista SOP dà agli utenti il diritto di essere auto-oppati
	e protetti (+a), di modificare la lista AutoKick e quella
	delle parolacce (badwords), di leggere le memo di canale,	
	e così via.
	
	Il comando SOP ADD aggiunge il nick specificato alla
	lista SOP.
	
	Il comando SOP DEL rimuove il nick specificato dalla
	lista SOP. Se viene specificata una lista, i record
	corrispondenti vengono rimossi. (Guarda l'esempio di
	LIST)
	
	Il comando SOP LIST mostra la lista SOP. Se viene
	utilizzata una wildcard per la mask, solo i record che
	corrispondono alla mask vengono mostrati. Se viene
	specificata una lista, i record corrispondenti vengono
	mostrati.
	Ad esempio:
	
	   SOP #canale LIST 2-5,7-9
	      Mostra i record SOP che hanno numero da 2
	      a 5 e da 7 a 9 (il 6 è quindi escluso).
	
	Il comando SOP CLEAR svuota la lista SOP.
	
	I comandi SOP ADD, SOP DEL e SOP CLEAR sono limitati
	al fondatore del canale. Comunque, tutti gli utenti nella
	lista SOP possono usare il comando SOP LIST.
	
	Questo comando potrebbe essere stato disabilitato per
	il tuo canale; in questo caso dovrai usare la lista di
	accesso. Digita /msg %S HELP ACCESS per
	maggiori informazioni, e /msg %S HELP SET XOP
	per scoprire come passare da un sistema all'altro.

CHAN_HELP_VOP
	Sintassi: VOP canale ADD nick
	          VOP canale DEL {nick | numero | lista}
	          VOP canale LIST [mask | lists]
	          VOP canale CLEAR
	
	Gestisce la lista VOP (VOicePeople) del canale.	
	La lista VOP dà agli utenti il diritto di essere
	voice sul tuo canale, e di ottenere il +v se non
	lo sono.
	
	Il comando VOP ADD aggiunge il nick specificato alla
	lista VOP.
	
	Il comando VOP DEL rimuove il nick specificato dalla
	lista VOP. Se viene specificata una lista, i record
	corrispondenti vengono rimossi. (Guarda l'esempio di
	LIST)
	
	Il comando VOP LIST mostra la lista VOP. Se viene
	utilizzata una wildcard per la mask, solo i record che
	corrispondono alla mask vengono mostrati. Se viene
	specificata una lista, i record corrispondenti vengono
	mostrati.
	Ad esempio:
	
	   VOP #canale LIST 2-5,7-9
	      Mostra i record VOP che hanno numero da 2
	      a 5 e da 7 a 9 (il 6 è quindi escluso).
	
	Il comando VOP CLEAR svuota la lista VOP.
	
	I comandi VOP ADD, VOP DEL e VOP LIST sono limitati
	agli HOP e superiori, mentre il comando VOP CLEAR può
	essere utilizzato soltanto dal fondatore del canale.
	
	Questo comando potrebbe essere stato disabilitato per
	il tuo canale; in questo caso dovrai usare la lista di
	accesso. Digita /msg %S HELP ACCESS per
	maggiori informazioni, e /msg %S HELP SET XOP
	per scoprire come passare da un sistema all'altro.

CHAN_HELP_ACCESS
	Sintassi: ACCESS canale ADD nick livello
	          ACCESS canale DEL {nick | numero | lista}
	          ACCESS canale LIST [mask | lista]
	          ACCESS canale CLEAR
	
	Gestisce la lista di accesso del canale.
	La lista di accesso specifica quali utenti hanno diritto
	allo stato di operatore di canale e ai vari comandi di
	%S sul canale. Utenti in diversi livelli avranno a
	disposizione diversi privilegi; per avere informazioni
	specifiche, puoi digitare /msg %S HELP ACCESS LEVELS.
	I nick che non si trovano nella lista di accesso hanno
	livello 0.
	
	Il comando ACCESS ADD aggiunge il nick specificato alla
	lista di accesso con il livello specificato; se il nick è
	già presente nella lista, il suo livello viene modificato,
	impostandolo a quello specificato nel comando. Il livello
	del nick specificato deve essere minore di quello del nick
	che invia il comando, e se il nick è già presente nella
	lista, il livello attuale di quel nick deve essere minore di
	quello del nick che invia il comando.
	
	Il comando ACCESS DEL rimuove il nick specificato dalla
	lista di accesso. Se viene specificata una lista, i record
	corrispondenti vengono rimossi. (Guarda l'esempio di
	LIST)

	Il comando ACCESS LIST mostra la lista di accesso. Se viene
	utilizzata una wildcard per la mask, solo i record che
	corrispondono alla mask vengono mostrati. Se viene specificata
	una lista, i record corrispondenti vengono mostrati.
	Ad esempio:
	
	   ACCESS #canale LIST 2-5,7-9
	      Mostra i record della lista che hanno numero da 2
	      a 5 e da 7 a 9 (il 6 è quindi escluso).
	      
	Il comando ACCESS CLEAR svuota completamente la lista di
	accesso.

CHAN_HELP_ACCESS_LEVELS
	Livelli di accesso degli utenti
	
	Di default, sono definiti i seguenti livelli di accesso:
	
	   Fondatore Accesso completo alle funzioni di %S;
	                     op automatico in entrata nel canale.
	                     Soltanto una persona può avere lo
	                     stato di fondatore (non può essere
	                     assegnato con il comando ACCESS).
	        10   Accesso al comando AKICK; op e protezione
	                     automatici (+o e +a).
	         5   Op automatico (+o).
	         3   Voice automatico (+v).
	         0   Nessun privilegio speciale; può essere
	                     oppato dagli altri op (a meno che
	                     non sia attiva l'opzione secure-ops).
	        <0   Non può essere oppato.
	
	Questi livelli possono essere cambiati, e se ne possono
	aggiungere altri, usando il comando LEVELS. Digita
	/msg %S HELP LEVELS per maggiori informazioni.

CHAN_HELP_AKICK
	Sintassi: AKICK canale ADD nick-mask [motivo]
	          AKICK canale STICK mask
	          AKICK canale UNSTICK mask
	          AKICK canale DEL nick-mask
	          AKICK canale LIST [mask]
	          AKICK canale VIEW [mask]
	          AKICK canale ENFORCE
	          AKICK canale CLEAR
	
	Gestice la lista AutoKick. Se un utente che si trova nella
	lista AutoKick cerca di entrare nel canale, %S lo
	bannerà dal canale e poi lo espellerà (kick).
	
	Il comando AKICK ADD aggiunge il nick specificato alla
	lista AutoKick. Se viene specificato un motivo, quella frase
	sarà utilizzato quando l'utente viene kickato; altrimenti,
	verrà utilizzata la frase standard ""You have been banned from
	the channel". In alternativa al nick può essere usata una
	hostmask.
	
	Il comando AKICK STICK mantiene permanentemente il ban
	sulla mask specificata. Se qualcuno cerca di rimuovere il ban,
	%S lo imposterà di nuovo. Questo comando non può essere
	usato contro i nick registrati.
	
	Il comando AKICK UNSTICK annulla l'effetto del comando
	AKICK STICK, permettendo di rimuovere il ban dal canale.
	
	Il comando AKICK DEL rimuove il nick o la mask specificata
	dalla lista AutoKick. Non rimuove, però, i ban piazzati da
	un AutoKick, che devono quindi essere rimossi manualmente.
	
	Il comando AKICK LIST mostra la lista AutoKick, o, se
	specificata, solo i record che corrispondono alla mask
	indicata.
	
	Il comando AKICK VIEW mostra la lista Autokick, fornendo
	più informazioni del comando AKICK LIST.
	
	Il comando AKICK ENFORCE fa sì che %S forzi la
	lista AutoKick attuale, rimuovendo dal canale tutti gli
	utenti che corrispondono alle varie mask presenti nella
	lista.
	
	Il comando AKICK CLEAR svuota la lista AutoKick.

CHAN_HELP_LEVELS
	Sintassi: LEVELS canale SET tipo livello
	          LEVELS canale {DIS | DISABLE} tipo
	          LEVELS canale LIST
	          LEVELS canale RESET
	
	Il comando LEVELS permette un controllo preciso dei
	significati dei livelli di accesso numerici usati nei
	canali. Con questo comando, è possibile definire il
	livello di accesso richiesto per la maggior parte delle
	funzioni di %S.
	(I comandi SET FOUNDER e SET PASSWORD, nonché questo
	comando, sono sempre riservati al fondatore del canale).
	
	LEVELS SET permette di definire il livello di accesso
	necessario per una funzione o per un gruppo di funzioni.
	LEVELS DISABLE (o, abbreviato, DIS) disattiva una
	funzione automatica o non permette più l'accesso a una
	funzione se non al fondatore del canale.
	LEVELS LIST mostra i livelli di accesso attuali per
	ogni funzione e per i gruppi di funzioni.
	LEVELS RESET reimposta i livelli ai valori di default
	di un canale appena creato (per maggiori informazioni,
	digita HELP ACCESS LEVELS).
	
	Per una lista delle funzioni di cui puoi cambiare i
	livelli, digita HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	La seguente lista mostra le funzioni disponibile. Tieni
	conto del fatto che i livelli AUTODEOP e NOJOIN sono i
	livelli massimi per i quali la funzione sarà attiva,
	mentre tutti gli altri sono livelli minimi.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Sintassi: INFO canale [ALL]
	
	Mostra varie informazioni a proposito del canale
	specificato (a patto che sia registrato), tra cui il
	fondatore, il giorno di registrazione, l'ultimo utilizzo,
	la descrizione e il blocco dei modi, se presente. Se
	viene specificata l'opzione ALL, verranno mostrati
	anche il successore e il messaggio di ingresso.
	
	Di default, l'opzione ALL è limitata a coloro che hanno
	l'accesso come fondatore sul canale.

CHAN_HELP_LIST
	Sintassi: LIST pattern
	
	Mostra tutti i canali registrati che corrispondono al
	pattern specificato. I canali con l'opzione PRIVATE
	attiva non saranno mostrati.

CHAN_HELP_OP
	Sintassi: OP [canale [nick]]
	
	Assegna lo stato di operatore (op, +o) a un utente sul
	canale specificato. Se non viene indicato alcun nick,
	"oppa" l'utente che invia il comando. Se non viene
	indicato né canale né nick, opperà l'utente che invia
	il comando su tutti i canali in cui si trova e in cui
	ha abbastanza privilegi.
	
	Di default, il comando è limitato agli AOP o agli utenti
	con livello di accesso 5 o superiori.

CHAN_HELP_DEOP
	Sintassi: DEOP [canale [nick]]
	
	Rimuove lo stato di operatore (op, -o) a un utente sul
	canale specificato. Se non viene indicato alcun nick,
	"deoppa" l'utente che invia il comando. Se non viene
	indicato né canale né nick, deopperà l'utente che invia
	il comando su tutti i canali in cui si trova e in cui
	ha abbastanza privilegi.
	
	Di default, il comando è limitato agli AOP o agli utenti
	con livello di accesso 5 o superiori.
	
CHAN_HELP_VOICE
	Sintassi: VOICE [canale [nick]]
	
	Assegna lo stato di voice (+v) a un utente sul canale
	specificato. Se non viene indicato alcun nick, dà il
	voice all'utente che invia il comando. Se non viene
	indicato né canale né nick, dà il voice all'utente che
	invia il comando su tutti i canali in cui si trova e in
	cui ha abbastanza privilegi.
	
	Di default, il comando è limitato agli AOP o agli utenti
	con livello di accesso 5 o superiori, oppure ai VOP o agli
	utenti con livello 3 o superiori per il voice su se stessi.

CHAN_HELP_DEVOICE
	Sintassi: DEVOICE [canale [nick]]
	
	Rimuove lo stato di voice (-v) da un utente sul canale
	specificato. Se non viene indicato alcun nick, toglie il
	voice all'utente che invia il comando. Se non viene
	indicato né canale né nick, toglie il voice all'utente che
	invia il comando su tutti i canali in cui si trova e in
	cui ha abbastanza privilegi.
	
	Di default, il comando è limitato agli AOP o agli utenti
	con livello di accesso 5 o superiori, oppure ai VOP o agli
	utenti con livello 3 o superiori per il devoice su se stessi.

CHAN_HELP_HALFOP
	Sintassi: HALFOP [canale [nick]]
	
	Assegna lo stato di mezzo operatore (halfop, +h) a un utente
	sul canale specificato. Se non viene indicato alcun nick,
	dà l'halfop all'utente che invia il comando. Se non viene
	indicato né canale né nick, dà l'halfop all'utente che invia
	il comando su tutti i canali in cui si trova e in cui ha
	abbastanza privilegi.
	
	Di default, il comando è limitato agli AOP o agli utenti
	con livello di accesso 5 o superiori, oppure ai HOP o agli
	utenti con livello 4 o superiori per l'halfop su se stessi.

CHAN_HELP_DEHALFOP
	Sintassi: DEHALFOP [canale [nick]]
	
	Rimuove lo stato di mezzo operatore (halfop, -h) da un utente
	sul canale specificato. Se non viene indicato alcun nick,
	toglie l'halfop all'utente che invia il comando. Se non viene
	indicato né canale né nick, toglie l'halfop all'utente che
	invia il comando su tutti i canali in cui si trova e in cui ha
	abbastanza privilegi.
	
	Di default, il comando è limitato agli AOP o agli utenti
	con livello di accesso 5 o superiori, oppure ai HOP o agli
	utenti con livello 4 o superiori per il dehalfop su se stessi.
	
CHAN_HELP_PROTECT
	Sintassi: PROTECT [canale [nick]]
	
	Assegna lo stato di protezione (+a) a un utente sul canale
	specificato. Se non viene indicato alcun nick, dà la
	protezione all'utente che invia il comando. Se non viene
	indicato né canale né nick, dà la protezione all'utente che
	invia il comando su tutti i canali in cui si trova e in
	cui ha abbastanza privilegi.
	
	Di default, il comando è limitato al fondatore del canale,
	oppure ai SOP o agli utenti con livello 10 o superiori per
	la protezione su se stessi.

CHAN_HELP_DEPROTECT
	Sintassi: DEPROTECT [canale [nick]]
	
	Rimuove lo stato di protezione (-a) da un utente sul canale
	specificato. Se non viene indicato alcun nick, toglie la
	protezione all'utente che invia il comando. Se non viene
	indicato né canale né nick, toglie la protezione all'utente
	che invia il comando su tutti i canali in cui si trova e in
	cui ha abbastanza privilegi.
	
	Di default, il comando è limitato al fondatore del canale,
	oppure ai SOP o agli utenti con livello 10 o superiori per
	la rimozione della protezione su se stessi.

CHAN_HELP_OWNER
	Sintassi: OWNER [canale]
	
	Ti assegna lo stato di proprietario (owner, +q) sul canale
	che specifichi. Se non indichi nessun canale, lo stato di
	proprietario ti sarà assegnato su tutti i canali su cui ti
	trovi e in cui hai abbastanza privilegi.
	
	Il comando è limitato agli utenti che hanno accesso di
	fondatore.

CHAN_HELP_DEOWNER
	Sintassi: DEOWNER [canale]
	
	Ti toglie lo stato di proprietario (owner, -q) sul canale
	che specifichi. Se non indichi nessun canale, lo stato di
	proprietario ti sarà rimosso su tutti i canali su cui ti
	trovi e in cui hai abbastanza privilegi.
	
	Il comando è limitato agli utenti che hanno accesso di
	fondatore.

CHAN_HELP_INVITE
	Sintassi: INVITE canale
	
	Chiede a %S di invitarti nel canale specificato.
	
	Di default, il comando è limitato agli AOP o agli utenti con
	livello di accesso 5 o superiori sul canale.

CHAN_HELP_UNBAN
	Sintassi: UNBAN canale
	
	Chiede a %S di rimuovere tutti i ban che non ti
	permettono di entrare sul canale specificato.
	
	Di default, il comando è limitato agli AOP o agli utenti con
	livello di accesso 5 o superiori sul canale.
	
CHAN_HELP_KICK
	Sintassi: KICK [canale [nick [motivo]]]
	
	Espelle (kick) il nick specificato dal canale. Se non viene
	indicato alcun nick, kicka l'utente che ha inviato il comando.
	Se non viene indicato né il nick né il canale, l'utente che
	ha inviato il comando verrà kickato da tutti i canali in cui
	si trova e in cui ha abbastanza privilegi.
	
	Di default, il comando è limitato agli AOP o agli utenti con
	livello di accesso 5 o superiori sul canale.

CHAN_HELP_BAN
	Sintassi: BAN [canale [nick [motivo]]]
	
	Banna (non permette l'accesso) il nick specificato sul
	canale. Se non viene indicato alcun nick, banna l'utente
	che ha inviato il comando. Se non viene indicato né il
	nick né il canale, l'utente che ha inviato il comando verrà
	bannato su tutti i canali in cui si trova e in cui ha
	abbastanza privilegi.
	
	Di default, il comando è limitato agli AOP o agli utenti con
	livello di accesso 5 o superiori sul canale.

CHAN_HELP_TOPIC
	Sintassi: TOPIC canale [topic]
	
	Fa cambiare il topic a %S. Se il nuovo topic
	non viene specificato, il topic attuale sarà rimosso.
	Questo comando è utile soprattutto quando l'opzione
	SET TOPICLOCK è attiva. Per maggiori informazioni,
	digita /msg %S HELP SET TOPICLOCK.
	
	Di default, il comando è limitato agli utenti con
	accesso di fondatore sul canale.

CHAN_HELP_CLEAR
	Sintassi: CLEAR canale cosa
	
	Fa in modo che %S "pulisca" vari settaggi di un
	canale. cosa può essere una delle opzioni seguenti:
	
	     MODES    Annulla tutti i modi di canale (cioè elimina
	                  i modi i,k,l,m,n,p,s,t).
	     BANS     Annulla tutti i ban del canale.
	     OPS      Rimuove lo stato di operatore (modo +o) da
	                  tutti gli operatori del canale.
	     HOPS     Removes channel-halfoperator status (mode +h) from
	                  all channel halfoperators, if supported.
	     VOICES   Rimuove lo stato di voice (modo +v) da tutti
	                  gli utenti voice sul canale.
	     USERS    Rimuove (con un kick) tutti gli utenti del canale.
	
	Di default, il comando è limitato agli utenti con
	accesso di fondatore sul canale.
	
CHAN_HELP_GETKEY
	Sintassi: GETKEY canale
	
	Restituisce la chiave (password) del canale specificato.
	Questo comando è pensato per essere usato dai bot e dagli
	script, e il suo output ha questo formato:
	
	KEY <canale> <chiave>
	
	chiave corrisponde a "NO KEY" se il canale non richiede
	una chiave.
	
CHAN_HELP_SENDPASS
	Sintassi: SENDPASS canale
	
	Invia la password del canale specificato all'indirizzo e-mail
	del fondatore (specificato nelle opzioni del nick del fondatore
	stesso). Questo comando è utile quando un utente ha dimenticato
	la password del canale.
	
	Potrebbe essere disponibile soltanto per gli IrcOp.
	
	Questo comando non è utilizzabile quando la criptazione è attiva.

CHAN_SERVADMIN_HELP
	
	I seguenti comandi sono disponibili soltanto ai Services admin:
	
	    GETPASS  Recupera la password di fondatore di un canale
	                   (solo se la criptazione è disattivata)
	    FORBID    Blocca un canale in modo che non venga usato
	    STATUS    Restituisce il livello di accesso corrente di
	                   un utente su un canale
	
	I Services admin possono inoltre deregistrare qualsiasi canale
	senza che siano identificati con la password, e possono vedere
	la lista di accesso, la lista AKICK e le liste di configurazione
	dei livelli per qualsiasi canale.
	
CHAN_SERVADMIN_HELP_LOGOUT
	Sintassi: LOGOUT canale [nick]
	
	Questo comando fa si' che il nick specificato non sia più
	identificato sul canale indicato.
	
	Se sei il fondatore del canale, puoi deidentificare chiunque,
	altrimenti puoi deidentificare solo te stesso.
	
	Se sei un Services admin, puoi deidentificare chiunque da
	qualsiasi canale senza dover essere il fondatore del canale.
	Inoltre puoi omettere il parametro nick; questo farà in modo
	che tutti gli utenti siano deidentificati dal canale.

CHAN_SERVADMIN_HELP_DROP
	Sintassi: DROP canale
	
	Deregistra il canale indicato. Solo i Services admin
	possono deregistrare un canale per il quale non si siano
	identificati.

CHAN_SERVADMIN_HELP_SET
	
	I services admin possono anche impostare l'opzione NOEXPIRE,
	in modo da non far scadere i canali anche in caso di inutilizzo.
	Inoltre, i Services admin possono impostare le opzioni per
	qualsiasi canale senza aver bisogno di identificarsi sui canali
	stessi.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Sintassi: SET canale NOEXPIRE {ON | OFF}
	
	Imposta se il canale scadrà o meno in caso di inutilizzo. Se
	l'opzione è impostata a ON, il canale non scadrà.
	
	Limitato ai Services admin.

CHAN_SERVADMIN_HELP_INFO
	
	I Services admin possono usare il parametro ALL con qualsiasi
	canale.

CHAN_SERVADMIN_HELP_LIST
	Sintassi: LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
	
	Mostra tutti i canali registrati che corrispondono al
	pattern specificato. I canali con l'opzione PRIVATE
	attiva saranno mostrati solo ai Services admin. I canali
	con l'opzione NOEXPIRE attiva avranno un ! prima del
	nick stesso (solo per i Services admin).
	
	Se sono specificate le opzioni FORBIDDEN, SUSPENDED o NOEXPIRE, solo
	i canali che, rispettivamente, sono vietati, sospesi o hanno l'opzione
	NOEXPIRE attiva verranno mostrati. Se vengono specificate
	entrambe le opzioni, entrambi i tipi di canale saranno
	visualizzati. Queste opzioni sono limitate ai Services admin.

CHAN_SERVADMIN_HELP_GETPASS
	Sintassi: GETPASS canale
	
	Restituisce la password del canale specificato.
	
	Da notare che quando questo comando viene usato,
	verrà inviato un WALLOPS o GLOBOPS ad indicare la
	persona che l'ha utilizzato e su quale canale è stato
	utilizzato.
	
	Limitato ai Services admin.
	
	Questo comando non è disponibile quando è attiva la
	criptazione dei database.

CHAN_SERVADMIN_HELP_FORBID
	Sintassi: FORBID canale [motivo]
	
	Fa in modo che nessuno possa registrare o usare
	il canale indicato. Può essere annullato con
	una deregistrazione (drop) del canale.
	
	Limitato ai to Services admins.

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
	Sintassi: STATUS canale nick
	
	Restituisce il livello di accesso attuale del nick specificato
	sul canale indicato. La risposta usa il formato:
	
	    STATUS canale nick livello-di-accesso
	
	Se si verifica un errore, la risposta avrà il formato
	
	    STATUS ERROR messaggio-di-errore
	
	Limitato ai Services admin.

###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP
	%S è un servizio che permette agli utenti della rete di
	scambiarsi brevi messaggi (memo), anche se i destinatari non
	sono connessi; permette inoltre di inviare messaggi ai canali(*).
	Il nick del mittente e quello del destinatario (o il canale
	di destinazione) devono essere registrati.
	
	I comandi di %S sono:
	
	    SEND   Invia un memo a un nick o a un canale
	    CANCEL Annulla l'ultimo memo inviato
	    LIST   Mostra la lista dei propri memo
	    READ   Legge uno o più memo
	    DEL    Cancella uno o più memo
	    SET    Imposta le opzioni relative ai memo
	
MEMO_HELP_ADMIN
	    SENDALL  Send a memo to all registered users

MEMO_HELP_OPER
	    STAFF  Send a memo to all opers/admins

MEMO_HELP_FOOTER
	
	Digita /msg %S HELP comando per maggiori informazioni
	sull'uso dei comandi sopraelencati.
	
	(*) Di default, qualsiasi utente con almeno livello 10 (SOP) su
	    un canale può leggere i memo di quel canale. Questo livello
	    minimo può essere impostato con il comando LEVELS di
	    %s.

MEMO_HELP_SEND
	Sintassi: SEND {nick | canale} testo-del-memo
	
	Invia un memo che contiene il testo-del-memo al nick
	o al canale specificato. Quando si invia un memo a un
	nick, il destinatario riceverà un messaggio che lo notifica
	che ha un nuovo memo. Il nick/canale di destinazione deve
	esser registrato.
	
MEMO_HELP_CANCEL
	Sintassi: CANCEL {nick | canale}
	
	Annulla l'ultimo memo inviato al nick o al canale specificato,
	sempre che non sia già stato letto prima dell'uso di questo
	comando.

MEMO_HELP_LIST
	Sintassi: LIST [canale] [list | NEW]
	
	Mostra le proprie memo. Quando viene specificato NEW, mostra
	soltanto i nuovi memo (non letti). I memo non letti sono segnati
	con un "*" a sinistra del numero d'ordine. Si può anche indicare
	una lista di numeri, come in quest'esempio:
	
	   LIST 2-5,7-9
	      Mostra i memo che hanno numeri tra 2 e 5 e tra 7 e 9,
	      escludendo quindi il 6.

MEMO_HELP_READ
	Sintassi: READ [canale] {numero | lista | LAST | NEW}
	
	Mostra il testo dei propri memo specificati. Se viene indicato
	LAST, mostra l'ultimo memo ricevuto. Se viene indicato NEW,
	mostra tutti i nuovi memo. Altrimenti, mostra il memo che ha
	numero numero. Si può anche specificare una lista di numeri,
	come nell'esempio:
	
	   READ 2-5,7-9
	      Mostra i memo che hanno numeri tra 2 e 5 e tra 7 e 9,
	      escludendo quindi il 6.

MEMO_HELP_DEL
	Sintassi: DEL [canale] {numero | lista | LAST | ALL}
	
	Elimina il memo o i memo specificati. Si possono indicare
	numei multipli di memo o intervalli di numeri invece di
	un numero singolo, come nel secondo esempio.
	
	If LAST is given, the last memo will be deleted.
	Se viene usato ALL elimina tutti i memo.
	
	Esempi:
	
	   DEL 1
	      Elimina il primo memo.
	
	   DEL 2-5,7-9
	      Elimina i memo che hanno numero numeri tra 2 e 5 e
	      tra 7 e 9, conservando quindi il 6.

MEMO_HELP_SET
	Sintassi: SET opzione parametri
	
	Imposta varie opzioni dei memo. opzione può essere:
	
	    NOTIFY      Cambia il modo in cui si verrà notificati
	                    della ricezione di nuovi memo (solo
	                    per i nick)
	    LIMIT       Imposta il numero massimo di memo che si
	                    possono ricevere
	
	Digita /msg %S HELP SET opzione per ottenere
	maggiori informazioni su un'opzione specifica.

MEMO_HELP_SET_NOTIFY
	Sintassi: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Cambia il modo in cui si verrà informati della ricezione
	di nuovi memo:
	
	    ON      Si verrà informati dei nuovi memo quando ci si
	            connette, quando si torna dall'/AWAY, e quando
	            vengono inviati (se si è online).
	    LOGON   Si verrà informati dei nuovi memo quando ci si
	            connette o quando si torna dall'/AWAY.
	    NEW     Si verrà informati dei nuovi memo solo quando
	            vengono inviati (se si è online).
	    OFF     Non si verrà informati in nessun caso della
	            ricezione dei nuovi memo.
	
	ON è sostanzialmente la combinazione di LOGON e NEW.

MEMO_HELP_SET_LIMIT
	Sintassi: SET LIMIT [canale] limite
	
	Imposta il numero massimo di memo che si possono tenere
	(o che può tenere il canale indicato). Se il limite è
	impostato a 0, nessuno potrà inviare memo. In ogni caso,
	non è possibile impostare il limite a un valore maggiore
	di %d.

MEMO_HELP_INFO
	Sintassi: INFO [canale]
	
	Mostra le informazioni sul numero di memo che hai, quanti
	memo non sono ancora stati letti e quanti memo puoi avere
	in totale. Se viene specificato un canale, mostra le stesse
	informazioni, ma relative al canale indicato.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Sintassi: SET LIMIT [utente | canale] {limite | NONE} [HARD]
	
	Imposta il numero massimo di memo che un utente (nick) o
	canale può avere. Impostare il limite a 0 fa in modo che
	l'utente (o il canale) non possa ricevere alcun memo;
	impostarlo a NONE fa sì che l'utente (o il canale) possa
	ricevere e conservare un numero indefinito di memo. Se non
	viene specificato nessun nick o canale, viene impostato il
	proprio limite.
	
	Aggiungere il parametro HARD fa in modo che l'utente non
	possa cambiare il limite. Non aggiungerlo ha il livello
	opposto, e permette all'utente di cambiarlo, anche se un
	limite precedente era stato impostato con HARD.
	
	Questo uso di SET LIMIT è limitato ai Services admin.
	Gli altri utenti possono impostare il limite soltanto per
	se stessi o per i canali sui quali hanno abbastanza
	privilegi, e potrebbero non essere in grado di rimuovere
	il limite o di impostarlo a un valore maggiore di %d.

MEMO_SERVADMIN_HELP_INFO
	Sintassi: INFO [nick | canale]
	
	Senza parametro, mostra le informazioni sul numero di memo
	che hai, quanti memo non sono ancora stati letti e quanti
	memo puoi ricevere in totale.
	
	Se viene indicato un canale, mostra le stesse informazioni
	ma relative al canale; se viene indicato un nick, mostra
	le stesse informazioni ma relative al nick.
	Questo modo d'uso è limitato ai Services admin.

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
	Comandi di %S:
	    GLOBAL      Invia un messaggio a tutti gli utenti
	    STATS       Mostra lo stato dei Services e della rete
	    OPER LIST   Mostra tutti i Services operator
	    ADMIN LIST  Mostra tutti i Services admin
	    STAFF       Display Services staff and online status

OPER_HELP_OPER_CMD
	Comandi disponibili solo ai Services operator e admin:
	    MODE        Cambia dei modi di un canale
	    KICK        Kicka un utente da un canale
	    CLEARMODES  "Pulisce" i modi di un canale
	    KILLCLONES  Killa tutti gli utenti da un certo host
	    AKILL       Manipola la lista AKILL

OPER_HELP_OPER_CMD_EXTRA
	Following commands are also available

OPER_HELP_OPER_CMD_SGLINE
	    SGLINE      Manipola la lista SGLINE

OPER_HELP_OPER_CMD_SQLINE
	    SQLINE      Manipola la lista SQLINE

OPER_HELP_OPER_CMD_SZLINE
	    SZLINE      Manipola la lista SZLINE

OPER_HELP_ADMIN_CMD
	Comandi disponibili solo ai Services admin:
	    OPER        Modifica la lista dei Services operator
	    CHANLIST    Mostra tutti i canali
	    USERLIST    Mostra tutti gli utenti
	    LOGONNEWS   Definisce i messaggi da mostrare agli utenti
	                    quando si connettono
	    RANDOMNEWS  Definisce i messaggi da mostrare casualmente
	                    agli utenti quando si connettono
	    OPERNEWS    Definisce i messaggi da mostrare agli operatori
	    SESSION     Mostra la lista delle sessioni degli host
	    EXCEPTION   Modifica la lista delle eccezioni al limite delle
	                    sessioni
	    CACHE       Mostra gli host nella cache del proxy detector
	    NOOP        Rimuove temporaneamente tutte le O:line di un server
	    JUPE        Disabilita ("jupiter") un server
	    IGNORE      Modifica la lista ignore dei Services
	    SET         Imposta varie opzioni globali dei Services
	    RELOAD      Rilegge il file di configurazione dei Services
	    UPDATE      Forza la scrittura su disco immediata dei database
	                    dei Services
	    RESTART     Salva i database e riavvia i Services
	    QUIT        Termina i Services senza salvare i database
	    SHUTDOWN    Termina i Services salvando i database

	Comandi disponibili solo al super-user dei Services:
	    ADMIN       Modifica la lista dei Services admin

OPER_HELP_ADMIN_CMD_EXTRA
	Following commands are also available to Service Admins

OPER_HELP_ADMIN_CMD_OLINE
	    OLINE       Imposta le operflag di un utente

OPER_HELP_ADMIN_CMD_UMODE
	    UMODE       Cambia i modi utente di un utente

OPER_HELP_ADMIN_CMD_SVSNICK
	    SVSNICK     Forcefully change a user's nickname

OPER_HELP_ROOT_CMD
	Commands available to Serivces Roots only:
	    MODLOAD     Load a module
	    MODUNLOAD   Un-Load a module
	    MODLIST     List loaded modules
	    MODINFO     Info about a loaded module

OPER_HELP_LOGGED
	Nota: Tutti i comandi inviati a %S sono loggati!

OPER_HELP_GLOBAL
	Sintassi: GLOBAL messaggio
	
	Permette agli IrcOp di inviare un messaggio a tutti
	gli utenti della rete. Il messaggio sarà inviato
	dal nick %s.

OPER_HELP_STATS
	Sintassi: STATS [AKILL | ALL | RESET]
	
	Senza nessuna opzione, mostra il numero attuale di utenti
	e di IrcOp online (Services esclusi), il numero massimo
	di utenti online da quando i Services sono up, e il tempo
	passato da quando i Services sono stati avviati.
	
	Con l'opzione AKILL, mostra la dimensione attuale della
	lista AKILL e la durata di default delle nuove AKILL.
	
	L'opzione ALL è disponibile solo ai Services admin, e
	mostra informazioni sull'utilizzo di memoria dei Services.
	Usare questa opzione può bloccare i Services per un breve
	periodo di tempo sulle grandi reti.
	
	L'opzione RESET imposta il numero massimo di utenti
	registrato al numero degli utenti connessi attualmente
	alla rete.
	
	UPTIME può essere usato come sinonimo di STATS.

OPER_HELP_OPER
	Sintassi: OPER ADD nick
	          OPER DEL {nick | numero | lista}
	          OPER LIST [mask | lista]
	          OPER CLEAR
	
	Permette ai Services admin di aggiungere o rimuovere dei nick
	alla/dalla lista dei Services operator. Un utente il cui nick
	è nella lista dei Services operator e che si è identificato
	con %s avrà accesso ai comandi utilizzabili dai Services
	operator.
	
	Il comando OPER ADD aggiunge il nick specificato alla lista
	dei Services operator.
	
	Il comando OPER DEL rimuove il nick specificato dalla lista
	dei Services operator. Se viene indicata un numero o una lista,
	vengono rimossi i record corrispondenti (si veda l'esempio di
	LIST).
	
	Il comando OPER LIST mostra la lista dei Services operator.
	Se viene usata una wildcard per la mask, solo i record che
	corrispondono alla mask vengono mostrati. Se viene specificata
	una lista di numeri, solo quei record vengono mostrati.
	Ad esempio:
	
	   OPER LIST 2-5,7-9
	      Mostra i record relativi ai Services operator che hanno
	      numeri da 2 a 5 e da 7 a 9 (escluso il 6).
	      
	Il comando OPER CLEAR svuota la lista dei Services operator.
	
	Qualsiasi IrcOp può usare il comando OPER LIST.
	Tutte le altre opzioni sono limitate ai Services admin.

OPER_HELP_ADMIN
	Sintassi: ADMIN ADD nick
	          ADMIN DEL {nick | numero | lista}
	          ADMIN LIST [mask | lista]
	          ADMIN CLEAR
	
	Permette al super-user (root) dei Services di aggiungere o
	rimuovere dei nick dalla/alla lista dei Services admin. Un
	utente il cui nick è nella lista dei Services operator e che
	si è identificato con %s avrà accesso ai comandi utilizzabili
	dai Services admin.
	
	Il comando ADMIN ADD aggiunge il nick specificato alla lista
	dei Services admin.
	
	Il comando ADMIN DEL rimuove il nick specificato dalla lista
	dei Services admin. Se viene indicata un numero o una lista,
	vengono rimossi i record corrispondenti (si veda l'esempio di
	LIST).
	
	Il comando ADMIN LIST mostra la lista dei Services admin.
	Se viene usata una wildcard per la mask, solo i record che
	corrispondono alla mask vengono mostrati. Se viene specificata
	una lista di numeri, solo quei record vengono mostrati.
	Ad esempio:
	
	   ADMIN LIST 2-5,7-9
	      Mostra i record relativi ai Services admin che hanno
	      numeri da 2 a 5 e da 7 a 9 (escluso il 6).
	      
	Il comando ADMIN CLEAR svuota la lista dei Services admin.
	
	Qualsiasi IrcOp può usare il comando ADMIN LIST.
	Tutte le altre opzioni sono limitate al Services root.

OPER_HELP_IGNORE
	Sintassi: IGNORE {ADD|DEL|LIST} [tempo] [nick]

	Permette ai Services Admin di impostare i Services affinché
	ignorino un utente per un certo tempo o fino al prossimo
	riavvio. Il tempo, di default, è in secondi; si possono
	specificare differenti unità. Unità valide sono:
	s per secondi, m per minuti, h per ore e
	d per giorni. Le combinazioni di queste unità non sono
	permesse. Per far sì che i Services ignorino l'utente in
	maniera permanente, è possibile usare 0 come tempo.

OPER_HELP_MODE
	Sintassi: MODE canale modi
	
	Permette ai Services operator di impostare modi di canale
	per qualsiasi canale. I parametri sono gli stessi utilizzati
	con il comando /MODE standard.
	
	Limitato ai Services operator.

OPER_HELP_UMODE
	Sintassi: UMODE nick modi
	
	Permette ai Services Admin di impostare i modi per
	qualsiasi utente. I parametri sono gli stessi del
	comando /MODE standard.
	
	Limitato ai Services admin.

OPER_HELP_OLINE
	Sintassi: OLINE nick flag
	
	Permette ai Services admin di impostare le Operflag
	per qualsiasi utente. Le flag devono essere introdotte
	da "+" o "-". Per rimuovere tutte le flag, è sufficiente
	usare "-" senza altri parametri.
	
	Limitato ai Services admin.

OPER_HELP_CLEARMODES
	Sintassi: CLEARMODES canale [ALL]
	
	Annulla tutti i modi binari (i,k,l,m,n,p,s,t) e i ban
	dal canale specificato. Se viene indicato ALL, verranno
	rimossi anche gli op e i voice (modi +o e +v).
	
	Limitato ai Services operator.

OPER_HELP_KICK
	Sintassi: KICK canale utente motivo
	
	Permette agli IrcOp di kickare un utente da qualsiasi
	canale. I parametri sono gli stessi del comando /KICK
	standard. Il messaggio di kick conterrà all'inizio il
	il nick dell'IrcOp che invia il comando, ad esempio:
	
	*** Spammer has been kicked off channel #canale by %S (OperNick (motivo))
	
	Limitato ai Services operator.

OPER_HELP_SVSNICK
	Syntax: SVSNICK nick newnick
	
	Forcefully changes a user's nickname from nick to newnick.
	
	Limited to Services admins

OPER_HELP_AKILL
	Sintassi: AKILL ADD [+durata] mask motivo
	          AKILL DEL {mask | numero | lista}
	          AKILL LIST [mask | lista]
	          AKILL VIEW [mask | lista]
	          AKILL CLEAR
	
	Permette ai Services operator di manipolare la lista AKILL.
	Se un utente che si trova nella lista AKILL cerca di connettersi,
	i Services manderanno un KILL per quell'utente e, se supportato,
	indicheranno a tutti i server di aggiugnere una K:line per la
	mask che corrispondeva all'utente.
	
	AKILL ADD aggiunge la mask user@host specificata alla lista
	AKILL per il motivo specificato (che deve essere indicato).
	durata  è specifcato come un intero seguito da una lettera,
	che può essere d (giorni), h (ore), o m (minuti). Le
	combinazioni (ad esempio 1h30m) non sono permesse. Se non
	viene specificata una unità, il valore viene inteso in giorni
	(quindi +30 significa 30 giorni). Per aggiungere una AKILL
	senza scadenza, bisogna usare +0. Se la usermask indicata
	inizia con un +, la durata deve essere indicata esplicitamente,
	anche se è la stessa di default. La durata di default per le
	AKILL può essere trovata con il comando STATS AKILL.
	
	Il comando AKILL DEL rimuove la mask specificata dalla lista
	AKILL, se è presente. Se viene indicata una lista di numeri,
	verrano rimossi i record corrispondenti (vedi l'esempio di
	LIST).
	
	Il comando AKILL LIST mostra la lista AKILL. Se viene usata
	una wildcard nella mask, solo i record che corrispondono alla
	mask vengono mostrati. Se viene indicata una lista di numeri,
	soltanto quei record vengono mostrati, ad esempio:
	
	   AKILL LIST 2-5,7-9
	      Mostra i record della lista AKILL che hanno numeri da 2
	      a 5 e da 7 a 9.
	
	AKILL VIEW mostra più informazioni di AKILL LIST, e indica
	chi ha aggiunto una AKILL, quando è stata aggiunta, e quando
	scade, oltre alla mask user@host e al motivo.
	
	AKILL CLEAR svuota la lista AKILL.
	
	Limitato ai Services operator.
	
OPER_HELP_SGLINE
	Sintassi: SGLINE ADD [+durata] mask:motivo
	          SGLINE DEL {mask | numero | lista}
	          SGLINE LIST [mask | lista]
	          SGLINE VIEW [mask | lista]
	          SGLINE CLEAR
	
	Permette ai Services operator di manipolare la lista SGLINE.
	Se un utente che si trova nella lista SGLINE cerca di connettersi,
	i Services non gli permetterano di proseguire la sua sessione
	su IRC.
	
	SGLINE ADD aggiunge la mask di realname specificata alla lista
	sgline per il motivo specificato (che deve essere indicato).
	durata  è specifcato come un intero seguito da una lettera,
	che può essere d (giorni), h (ore), o m (minuti). Le
	combinazioni (ad esempio 1h30m) non sono permesse. Se non
	viene specificata una unità, il valore viene inteso in giorni
	(quindi +30 significa 30 giorni). Per aggiungere una SGLINE
	senza scadenza, bisogna usare +0. Se la usermask indicata
	inizia con un +, la durata deve essere indicata esplicitamente,
	anche se è la stessa di default. La durata di default per le
	SGLINE può essere trovata con il comando STATS AKILL.
	Nota: poiché la mask di realname può contenere spazi, il
	separatore tra la mask e il motivo è un duepunti.
	
	Il comando SGLINE DEL rimuove la mask specificata dalla lista
	SGLINE, se è presente. Se viene indicata una lista di numeri,
	verrano rimossi i record corrispondenti (vedi l'esempio di
	LIST).
	
	Il comando SGLINE LIST mostra la lista SGLINE. Se viene usata
	una wildcard nella mask, solo i record che corrispondono alla
	mask vengono mostrati. Se viene indicata una lista di numeri,
	soltanto quei record vengono mostrati, ad esempio:
	
	   SGLINE LIST 2-5,7-9
	      Mostra i record della lista SGLINE che hanno numeri da 2
	      a 5 e da 7 a 9.
	
	SGLINE VIEW mostra più informazioni di SGLINE LIST, e indica
	chi ha aggiunto una SGLINE, quando è stata aggiunta, e quando
	scade, oltre alla mask di realname e al motivo.
	
	SGLINE CLEAR svuota la lista SGLINE.
	
	Limitato ai Services operator.

OPER_HELP_SQLINE
	Sintassi: SQLINE ADD [+durata] mask motivo
	          SQLINE DEL {mask | numero | lista}
	          SQLINE LIST [mask | lista]
	          SQLINE VIEW [mask | lista]
	          SQLINE CLEAR
	
	Permette ai Services operator di manipolare la lista SQLINE.
	Se un utente il cui nick si trova nella lista SQLINE cerca di,
	connettersi, i Services non gli permetterano di proseguire la
	sua sessione su IRC.
	
	Se il primo carattere della mask è #, i Services bloccheranno
	l'uso dei canali corrispondenti (sugli IRCd che supportano
	questa funzione).
	
	SQLINE ADD aggiunge la mask di realname specificata alla lista
	sgline per il motivo specificato (che deve essere indicato).
	durata  è specifcato come un intero seguito da una lettera,
	che può essere d (giorni), h (ore), o m (minuti). Le
	combinazioni (ad esempio 1h30m) non sono permesse. Se non
	viene specificata una unità, il valore viene inteso in giorni
	(quindi +30 significa 30 giorni). Per aggiungere una SQLINE
	senza scadenza, bisogna usare +0. Se la usermask indicata
	inizia con un +, la durata deve essere indicata esplicitamente,
	anche se è la stessa di default. La durata di default per le
	SQLINE può essere trovata con il comando STATS AKILL.
	
	Il comando SqLINE DEL rimuove la mask specificata dalla lista
	SqLINE, se è presente. Se viene indicata una lista di numeri,
	verrano rimossi i record corrispondenti (vedi l'esempio di
	LIST).
	
	Il comando SQLINE LIST mostra la lista SQLINE. Se viene usata
	una wildcard nella mask, solo i record che corrispondono alla
	mask vengono mostrati. Se viene indicata una lista di numeri,
	soltanto quei record vengono mostrati, ad esempio:
	
	   SQLINE LIST 2-5,7-9
	      Mostra i record della lista SQLINE che hanno numeri da 2
	      a 5 e da 7 a 9.
	      
	SQLINE VIEW mostra più informazioni di SQLINE LIST, e indica
	chi ha aggiunto una SQLINE, quando è stata aggiunta, e quando
	scade, oltre alla mask e al motivo.
	
	SQLINE CLEAR svuota la lista SQLINE.
	
	Limitato ai Services operator.

OPER_HELP_SZLINE
	Sintassi: SZLINE ADD [+durata] mask motivo
	          SZLINE DEL {mask | numero | lista}
	          SZLINE LIST [mask | lista]
	          SZLINE VIEW [mask | lista]
	          SZLINE CLEAR
	
	Permette ai Services operator di manipolare la lista SZLINE.
	Se un utente il cui IP si trova nella lista SGLINE cerca di
	connettersi, i Services non gli permetterano di proseguire
	la sua sessione su IRC (sia che l'IP abbia un host alfa-
	numerico corrispondente o meno).
	
	SZLINE ADD aggiunge la mask di IP specificata alla lista
	sgline per il motivo specificato (che deve essere indicato).
	durata  è specifcato come un intero seguito da una lettera,
	che può essere d (giorni), h (ore), o m (minuti). Le
	combinazioni (ad esempio 1h30m) non sono permesse. Se non
	viene specificata una unità, il valore viene inteso in giorni
	(quindi +30 significa 30 giorni). Per aggiungere una SZLINE
	senza scadenza, bisogna usare +0. Se la usermask indicata
	inizia con un +, la durata deve essere indicata esplicitamente,
	anche se è la stessa di default. La durata di default per le
	SZLINE può essere trovata con il comando STATS AKILL.
	
	Il comando SZLINE DEL rimuove la mask specificata dalla lista
	SZLINE, se è presente. Se viene indicata una lista di numeri,
	verrano rimossi i record corrispondenti (vedi l'esempio di
	LIST).
	
	Il comando SZLINE LIST mostra la lista SZLINE. Se viene usata
	una wildcard nella mask, solo i record che corrispondono alla
	mask vengono mostrati. Se viene indicata una lista di numeri,
	soltanto quei record vengono mostrati, ad esempio:
	
	   SZLINE LIST 2-5,7-9
	      Mostra i record della lista SZLINE che hanno numeri da 2
	      a 5 e da 7 a 9.
	
	SZLINE VIEW mostra più informazioni di SZLINE LIST, e indica
	chi ha aggiunto una SZLINE, quando è stata aggiunta, e quando
	scade, oltre alla mask di realname e al motivo.
	
	SZLINE CLEAR svuota la lista SZLINE.
	
	Limitato ai Services operator.

OPER_HELP_SET
	Sintassi: SET opzione impostazione
	
	Imposta varie opzioni globali dei Services. Le opzioni
	configurabili sono:
	    READONLY   Imposta la modalità read-only o read-write
	    LOGCHAN    Report log messages to a channel
	    DEBUG      Attiva o disattiva la modalità di debug
	    NOEXPIRE   Attiva o disattiva la modalità no expire
	    SUPERADMIN Attiva o disattiva la modalità SuperAdmin
	    SQL   	 Activate or deactivate sql mode
	    IGNORE     Activate or deactivate ignore mode
	    LIST       List the options
	
	Limitato ai Services admin.

OPER_HELP_SET_READONLY
	Sintassi: SET READONLY {ON | OFF}
	
	Attiva o disattiva la modalità read-only. In modalità
	read-only, gli utenti normali non potranno modificare
	nessuna informazione nei database dei Services, incluse
	le liste di accesso dei nick e dei canali, eccetera.
	Gli IrcOp con sufficienti privilegi sui Services potranno
	modificare le liste AKILL e deregistrare (drop) o bloccare
	(forbid) i nick e i canali, ma questi cambiamenti non
	verrano salvati, a meno che la modalità read-only non sia
	disattivata prima che i Services vengano terminati oppure
	riavviati.
	
	Questa opzione è equivalente all'opzione di riga di comando
	-readonly.

OPER_HELP_SET_LOGCHAN
	Syntax: SET LOGCHAN {ON | OFF}

	With this setting on, Services will send its logs to a specified
	channel as well as the log file. LogChannel must also be defined
	in the Services configuration file for this setting to be of any
	use.

	Note: This can have strong security implications if your log
	channel is not properly secured.

OPER_HELP_SET_DEBUG
	Sintassi: SET DEBUG {ON | OFF | numero}
	
	Attiva o disattiva la modalità di debug. In modalità di
	debug, tutti i dati inviati e ricevuti dai Services,
	nonché altri messaggi di debug, saranno scritti nel file
	di log. Se viene specificato un numero, la modalità
	di debug sarà attivata, e il livello di debug sarà
	impostato a numero.
	
	Questa opzione è equivalente all'opzione di riga di comando
	-debug.
	
OPER_HELP_SET_NOEXPIRE
	Sintassi: SET NOEXPIRE {ON | OFF}
	
	Attiva o disattiva la modalità no expire. In modalità
	no expire, i nick, i canali, le akill e le eccezioni
	non scadranno fino a quando l'opzione non viene disattivata.
	
	Questa opzione è equivalente all'opzione di riga di comando
	-noexpire.

OPER_HELP_SET_SUPERADMIN
	Sintassi: SET SUPERADMIN {ON | OFF}

	Questa impostazione aumenterà i privilegi dell'admin che
	la richiede, e permetterà ad esempio di essere fondatore
	su tutti i canali, ecc.
	
	Questa opzione NON è persistente, e dovrebbe essere usata
	solo quando necessario, e reimpostata a OFF al più presto.

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

	Display the various OperServ settings

OPER_HELP_NOOP
	Sintassi: NOOP SET server
	          NOOP REVOKE server
	
	NOOP SET rimuove tutte le O:line del server specificato
	e killa tutti gli IrcOp presenti su quel server, in modo che
	non possano effettuare un rehash del server, in quanto questo
	ne annullerebbe l'effetto.
	
	NOOP REVOKE ripristina tutte le O:line presenti sul
	server specificato.
	
	Nota: il formato del server specificato non è controllato
	dai Services prima dell'esecuzione del comando.
	
	Limitato ai Services admin.

OPER_HELP_JUPE
	Sintassi: JUPE server [motivo]
	
	Fa sì che i Services blocchino un server, creando un server
	"fasullo" connesso ai Services per evitare che il server reale
	possa connettersi alla rete. Questo blocco può essere rimosso
	utilizzando il comando standard SQUIT. Se viene indicato
	un motivo, questo sarà utilizzato nel campo delle informazioni
	del server; altrimenti, le informazioni conterranno il testo
	"Juped by <nick>", indicando il nick della persona che ha
	bloccato il server.
	
	Limitato ai Services admin.

OPER_HELP_RAW
	Sintassi: RAW testo
	
	Invia una stringa di testo direttamente al server a cui sono
	connessi i Services. Questo comando è utilizzabile in maniera
	limitata, e può causare danni enormi alla struttura della
	rete se usato in maniera impropria.
	L'USO DI QUESTO COMANDO DEVE ESSERE EVITATO a meno che non
	si sappia esattamente cosa si sta facendo!
	
	Limitato ai Services admins.

OPER_HELP_UPDATE
	Sintassi: UPDATE
	
	Forza la scrittura su disco immediata dei database dei
	Services nello stesso istante in cui il comando viene
	inviato.
	
	Limitato ai Services admins.
	
OPER_HELP_RELOAD
	Sintassi: RELOAD
	
	Rilegge il file di configurazione dei Services. Alcune
	impostazioni contenute nel file richiedono il riavvio
	effettivo dei Services per essere attivate (ad esempio
	il cambio dei nick dei Services, l'attivazione della
	limitazione delle sessioni, eccetera).
	
	Limitato ai Services admin.

OPER_HELP_QUIT
	Sintassi: QUIT
	
	Termina immediatamente i Services senza salvarne i
	databaase. Questo comando non deve mai essere usato,
	se non in caso di danni alla copia in memoria dei
	database, per evitare che la copia danneggiata venga
	salvata.
	Per terminare normalmente i Services, deve essere
	usato il comando SHUTDOWN.
	
	Limitato ai Services admin.

OPER_HELP_SHUTDOWN
	Sintassi: SHUTDOWN
	
	Salva i database su disco e termina i Services.
	
	Limitato ai Services admins.

OPER_HELP_RESTART
	Sintassi: RESTART
	
	Salva i database su disco e riavvia i Services.
	
	Limitato ai Services admin.

OPER_HELP_KILLCLONES
	Sintassi: KILLCLONES nick
	
	Killa tutti gli utenti che hanno lo stesso host di nick.
	Una AKILL temporanea, nel formato *@host, viene aggiunta
	in modo da evitare che i client killati si riconnettano
	immediatamente. Viene inoltre inviato un wallop per indicare
	chi ha usato il comando, a quale host era rivolto e quanti
	utenti sono stati killati. Questo comando è utile per
	rimuovere numerosi cloni dalla rete.
	
	Limitato ai Services operator.
	
OPER_HELP_CHANLIST
	Sintassi: CHANLIST [{pattern | nick} [SECRET]]
	
	Mostra tutti i canali attivi sulla rete, che siano registrati
	o meno.
	
	Se viene indicato un pattern mostra solo i canali corrispondenti.
	Se viene indicato un nick, mostra solo i canali in cui si trova
	l'utente specificato.
	Se viene indicato SECRET, mostra solo i canali che corrisponodno a
	pattern e che hanno il modo di canale +s o +p attivo.
	
	Limitato ai Services admin.

OPER_HELP_USERLIST
	Sintassi: USERLIST [{pattern | canale} [INVISIBLE]]
	
	Mostra tutti gli utenti connessi alla rete, che siano registrati
	o meno.
	
	Se viene indicato un pattern mostra solo gli utenti corrispondenti
	(il pattern deve essere nel formato nick!user@host).
	Se viene indicato un canale, mostra solo gli utenti che si trovano
	su quel canale.
	Se viene indicato INVISIBLE, mostra solo gli utenti con il modo
	utente +i attivo.
	
	Limitato ai Services admin.

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
	%S ti permette di avere un bot nel tuo canale.
	E' un servizio pensato per gli utenti che non possono
	mantenere un bot. I comandi disponibili sono i seguenti;
	per usarli, digita /msg %S comando.
	Per maggiori informazioni sull'uso di un comando
	specifico, digita /msg %S HELP comando.
	
	ACT          Invia un /me in canale
	ASSIGN       Assegna un bot a un canale
	BADWORDS     Gestisce la lista delle parolacce
	BOTLIST      Mostra la lista dei bot disponibili
	INFO         Mostra informazioni su un bot o su un canale
	KICK         Configura i kick
	SAY          Fa sì che il bot dica qualcosa in canale
	SET          Imposta le opzioni del bot
	UNASSIGN     Rimuove il bot da un canale
	
	Il bot entrerà in canale quando c'è almeno
	%d persona.
	
BOT_HELP_BOTLIST
	Sintassi: BOTLIST
	
	Mostra la lista di tutti i bot disponibili.
	
BOT_HELP_ASSIGN
	Sintassi: ASSIGN canale nick-del-bot
	
	Assegna il bot con il nick specificato al canale
	indicato. Dopo averlo assegnato, il bot può essere
	configurato in base alle necessità.

BOT_HELP_UNASSIGN
	Sintassi: UNASSIGN canale
	
	Rimuove il bot da un canale. Dopo aver usato questo
	comando, il bot non entrerà più nel canale, ma la sua
	configurazione resterà salvata, in modo da non dover
	riconfigurare tutto di nuovo nel caso se ne assegni
	uno di nuovo.

BOT_HELP_INFO
	Sintassi: INFO {canale | nick-del-bot}
	
	Permette di vedere le informazioni di %S relative
	a un canale o a un bot. Se il parametro è un canale,
	mostrerà le informazioni sui kick attivi. Se il
	parametro è il nick di un bot, mostrerà le informazioni
	relative alla data di creazione e al numero di canali
	a cui è assegnato.

BOT_HELP_SET
	Sintassi: SET canale opzione parametri
	
	Configura le opzioni del bot. opzione può essere:
	
	    DONTKICKOPS      Protegge gli op dai kick del bot
	    DONTKICKVOICES   Protegge i voice dai kick del bot
	    FANTASY          Attiva i comandi "di fantasia"
	    GREET            Attiva i messaggi di saluto (greet)
	    SYMBIOSIS        Fa in modo che il bot si comporti come
	                        un bot reale
	
	Digita /msg %S HELP SET opzione per avere maggiori
	informazioni su una opzione specifica.
	
	Nota: l'accesso a questo comando dipende dall'impostazione
	del livello SET.
	
BOT_HELP_SET_DONTKICKOPS
	Sintassi: SET canale DONTKICKOPS {ON|OFF}
	
	Attiva o disattiva la protezione degli op sul
	canale. Quando è attiva, gli op non saranno
	kickati dal bot, anche se non hanno accesso di
	livello NOKICK.
	
BOT_HELP_SET_DONTKICKVOICES
	Sintassi: SET canale DONTKICKVOICES {ON|OFF}
	
	Attiva o disattiva la protezione dei voice sul
	canale. Quando è attiva, i voice non saranno
	kickati dal bot, anche se non hanno accesso di
	livello NOKICK.

BOT_HELP_SET_FANTASY
	Sintassi: SET canale FANTASY {ON|OFF}
	
	Attiva o disattiva l'uso dei comandi di fantasia
	sul canale. Quando è attivo, gli utenti potranno
	usare sul canale i comandi !op, !deop, !voice,
	!devoice, !kick, !kb, !unban, !seen, eccetera.
	Divertiti a scoprire gli altri comandi; provali
	sia specificando un nick che da soli, e aggiungi
	un motivo ad alcuni.
	
	Gli utenti che vogliono usare i comandi di fantasia
	DEVONO avere privilegi sufficienti sia per il livello
	FANTASIA che per il livello del comando richiesto
	(ad esempio, per usare !op, l'utente deve avere
	privilegi sufficienti anche per il livello OPDEOP).

BOT_HELP_SET_GREET
	Sintassi: SET canale GREET {ON|OFF}
	
	Attiva o disattiva il saluto (greet) sul canale.
	Quando è attivo, il bot mostrerà la riga di saluto
	degli utenti che entrano, se hanno privilegi
	sufficienti. Per impostare il messaggio di saluto,
	è necessario usare l'opzione SET GREET di NickServ.

BOT_HELP_SET_SYMBIOSIS
	Sintassi: SET canale SYMBIOSIS {ON|OFF}
	
	Attiva o disattiva la modalità symbiosis sul
	canale. Quando è attiva, il bot farà tutto quello
	che normalmente sarebbe fatto da %s sui canali,
	come l'impostazione dei MODI, l'invio dei KICK,
	la visualizzazione del messaggio di ingresso,
	eccetera.

BOT_HELP_KICK
	Sintassi: KICK canale opzione parametri
	
	Configura i kick del bot.  opzione può essere:
	
	    BOLDS       Imposta se il bot kicka chi usa il grassetto
	    BADWORDS    Imposta se il bot kicka chi dice le parolacce
	    CAPS        Imposta se il bot kicka chi scrive maiuscolo
	    COLORS      Imposta se il bot kicka chi usa i colori
	    FLOOD       Imposta se il bot kicka chi flooda
	    REPEAT      Imposta se il bot kicka chi ripete la stessa cosa
	    REVERSES    Imposta se il bot kicka chi usa il reverse
	    UNDERLINES  Imposta se il bot kicka chi usa il sottolineato
	
	Digita /msg %S HELP KICK opzione per avere maggiori
	informazioni su una opzione specifica.
	
	Nota: l'accesso a questo comando dipende dall'impostazione
	del livello SET.

BOT_HELP_KICK_BOLDS
	Sintassi: KICK canale BOLDS {ON|OFF} [kick-prima-del-ban]
	
	Attiva o disattiva il kick per chi scrive in grassetto.
	
	kick-prima-del-ban è il numero di volte che un utente può
	essere kickato prima di venire bannato. Se il parametro
	non è specificato, l'utente non verrà bannato.

BOT_HELP_KICK_COLORS
	Sintassi: KICK canale COLORS {ON|OFF} [kick-prima-del-ban]
	
	Attiva o disattiva il kick per chi usa i colori.
	
	kick-prima-del-ban è il numero di volte che un utente può
	essere kickato prima di venire bannato. Se il parametro
	non è specificato, l'utente non verrà bannato.

BOT_HELP_KICK_REVERSES
	Sintassi: KICK canale REVERSES {ON|OFF} [kick-prima-del-ban]
	
	Attiva o disattiva il kick per chi usa il reverse.
	
	kick-prima-del-ban è il numero di volte che un utente può
	essere kickato prima di venire bannato. Se il parametro
	non è specificato, l'utente non verrà bannato.

BOT_HELP_KICK_UNDERLINES
	Sintassi: KICK canale UNDERLINES {ON|OFF} [kick-prima-del-ban]
	
	Attiva o disattiva il kick per chi usa il sottolineato.
	
	kick-prima-del-ban è il numero di volte che un utente può
	essere kickato prima di venire bannato. Se il parametro
	non è specificato, l'utente non verrà bannato.

BOT_HELP_KICK_CAPS
	Sintassi: KICK canale CAPS {ON|OFF} [kick-prima-del-ban [minimo [percentuale]]]
	
	Attiva o disattiva il kick per chi scrive maiuscolo.
	
	Il bot kicka soltanto se ci sono almeno minimo lettere
	maiuscole e se costituiscono almeno il percentuale%%
	del totale della linea. I valori di default per questi
	parametri sono 10 caratteri e 25%%.
	
	kick-prima-del-ban è il numero di volte che un utente può
	essere kickato prima di venire bannato. Se il parametro
	non è specificato, l'utente non verrà bannato.

BOT_HELP_KICK_FLOOD
	Sintassi: KICK canale FLOOD {ON|OFF} [kick-prima-del-ban [linee [secondi]]]
	
	Attiva o disattiva il kick per chi flooda.
	Quando è attiva, questa opzione fa sì che il bot kicki
	gli utenti che floodano il canale scrivendo almeno	
	linee linee in secondi secondi. I valori di default
	per questi parametri sono 6 linee e 10 secondi.
	
	kick-prima-del-ban è il numero di volte che un utente può
	essere kickato prima di venire bannato. Se il parametro
	non è specificato, l'utente non verrà bannato.

BOT_HELP_KICK_REPEAT
	Sintassi: KICK canale REPEAT {ON|OFF} [kick-prima-del-ban [numero]]
	
	Attiva o disattiva il kick per chi ripete la stessa cosa.
	Quando è attiva, questa opzione fa sì che il bot kicki
	gli utenti che ripetono la stessa cosa almeno numero
	volte. Il valore di default per questo parametro è 3.
	
	kick-prima-del-ban è il numero di volte che un utente può
	essere kickato prima di venire bannato. Se il parametro
	non è specificato, l'utente non verrà bannato.

BOT_HELP_KICK_BADWORDS
	Sintassi: KICK canale BADWORDS {ON|OFF} [kick-prima-del-ban]
	
	Attiva o disattiva il kick per chi dice le parolacce.
	Quando è attiva, questa opzione fa sì che il bot kicki
	chi dice certe parole sul canale.
	
	Le parolacce possono essere definite usando il comando
	BADWORDS. Digita /msg %S HELP BADWORDS per
	ottenere maggiori informazioni.
	
	kick-prima-del-ban è il numero di volte che un utente può
	essere kickato prima di venire bannato. Se il parametro
	non è specificato, l'utente non verrà bannato.
	
BOT_HELP_BADWORDS
	Sintassi: BADWORDS canale ADD parola [SINGLE | START | END]
	          BADWORDS canale DEL {parola | numero | lista}
	          BADWORDS canale LIST [mask | list]
	          BADWORDS canale CLEAR
	
	Gestisce la lista delle parolacce di un canale.
	La lista delle parolacce indica quali parole devono
	attivare un kick se i kick per chi dice le parolacce
	sono attivi. Per maggiori informazioni, digita
	/msg %S HELP KICK BADWORDS.
	
	Il comando BADWORDS ADD aggiunge la parola specificata
	alla lista delle parolacce. Se viene specificato SINGLE,
	il kick sarà effettuato solo se un utente dice la parola
	integralmente. Se viene specificato START, il kick sarà
	effettuato se un utente dice una parola che inizia con
	parola. Se viene specificato END, il kick sarà effettuato
	se un utente dice una parola che termina con parola.
	Se non viene specificata nessuna opzione, il kick sarà
	effettuato ogni volta che un utente dice la parola
	parola.
	
	Il comando BADWORDS DEL rimuove la parola specificata
	dalla lista delle parolacce. Se viene indicata una lista
	di numeri, saranno rimosse le parole corrispondenti.
	Per maggiori informazioni, guarda l'esempio di LIST.
	
	Il comando BADWORDS LIST mostra la lista delle parolacce.
	Se viene usata una wildcard, solo le parolacce che
	corrispondono alla mask verranno mostrate. Se viene indicata
	una lista di numeri, solo le parolacce corrispondenti
	verranno mostrate.
	Ad esempio:
	
	   BADWORDS canale LIST 2-5,7-9
	      Mostra le parolacce con numero da 2 a 5 e da
	      7 a 9 (escludendo quindi il 6)
	      
	Il comando BADWORDS CLEAR svuota la lista delle parolacce.

BOT_HELP_SAY
	Sintassi: SAY canale testo
	
	Fa sì che il bot dica in canale il testo specificato.

BOT_HELP_ACT
	Sintassi: ACT canale testo
	
	Fa sì che il bot faccia in canale l'equivalente di un
	"/me" utilizzando il testo specificato.

BOT_SERVADMIN_HELP
	
	Il seguente comando è disponibile ai Services admin:
	
	    BOT      Gestisce la lista dei bot disponibili

BOT_SERVADMIN_HELP_BOT
	Sintassi: BOT ADD nick ident host realname
	          BOT CHANGE vecchionick nuovonick [ident [host [realname]]]
	          BOT DEL nick
	          BOT LIST
	
	Permette ai Services admin di creare, modificare
	ed eliminare i bot che potranno essere usati
	dagli utenti sui loro canali.
	
	Il comando BOT ADD aggiunge un bot con il nick, l'ident,
	l'host e il realname specificati. Poiché non viene effettuato
	nessun controllo sui valori indicati, è necessario inviare
	il comando con attenzione.
	Il comando BOT CHANGE permette di cambiare il nick,
	l'ident, l'host e il realname del bot specificato, senza
	cancellarlo realmente (e senza perderne i dati associati).
	Il comando BOT DEL rimuove il bot indicato dalla lista dei
	bot disponibili.
	Il bot BOT LIST è un alias di BOTLIST e serve semplicemente
	a mostrare la lista dei bot disponibili sulla rete.
	
	Nota: se crei un bot con un nick che è già registrato,
	il nick sarà deregistrato; inoltre, se un utente lo sta
	usando, verrà killato.

BOT_SERVADMIN_HELP_SET
	
	Queste opzioni sono riservate ai Services admin:
	
	    NOBOT            Fa in modo che non sia possibile
	                        assegnare un bot al canale
	    PRIVATE          Fa in modo che il bot sia assegnabile
	                        solo dagli IrcOp

BOT_SERVADMIN_HELP_SET_NOBOT
	Sintassi: SET canale NOBOT {ON|OFF}
	
	Questa opzione fa in modo che non sia possibile
	assegnare alcun bot al canale specificato.
	Se un bot è già assegnato, verrà rimosso nel
	momento in cui viene attivata l'opzione.
	
	Limitato ai Services admin.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Sintassi: SET nick-del-bot PRIVATE {ON|OFF}
	
	Questa opzione fa sì che il bot sia assegnabile
	soltanto dagli IrcOp.
	
	Limitato ai Services admin.
HOST_EMPTY
	La lista dei vhost è vuota.
HOST_ENTRY
	#%d Nick:%s, vhost:%s (%s - %s)
HOST_IDENT_ENTRY
	#%d Nick:%s, vhost:%s@%s (%s - %s)
HOST_SET
	Il vhost per %s è stato impostato a %s.
HOST_IDENT_SET
	Il vhost per %s è stato impostato a %s@%s.
HOST_SETALL
	Il vhost per il gruppo di %s è stato impostato a %s.
HOST_DELALL
	vhosts for group %s have been removed.
HOST_DELALL_SYNTAX
	Syntax: DELALL <nick>.
HOST_IDENT_SETALL
	Il vhost per il gruppo di %s è stato impostato a %s@%s.
HOST_SET_ERROR
	Errore! Un vhost può contenere soltanto i caratteri A-Z, a-z, 0-9, '.' e '-'.
HOST_SET_IDENT_ERROR
	A vhost ident must be in the format of a valid ident
HOST_SET_TOOLONG
	Errore! Il vhost è troppo lungo; usane uno che sia %d caratteri o meno.
HOST_SET_IDENTTOOLONG
	Errore! L'ident è troppo lungo; usane uno che sia %d caratteri o meno.
HOST_NOREG
	L'utente %s non è presente nel database di NickServ.
HOST_SET_SYNTAX
	Sintassi: /msg %s set <nick> <hostmask>.
HOST_SETALL_SYNTAX
	Sintassi: /msg %s setall <nick> <hostmask>.
HOST_DENIED
	Accesso negato.
HOST_NOT_ASSIGNED
	Contatta un Operatore per avere un vhost assegnato a questo nick.
HOST_ACTIVATED
	Il tuo vhost (%s) ora è attivato.
HOST_IDENT_ACTIVATED
	Il tuo vhost (%s@%s) ora è attivato.
HOST_ID
	Per favore, identificati con NickServ.
HOST_NOT_REGED
	È necessario registrarsi prima che sia possibile assegnare un vhost a un nick.
HOST_DEL
	Il vhost di %s è stato rimosso.
HOST_DEL_SYNTAX
	Sintassi: /msg %s del <nick>.
HOST_OFF_UNREAL
	Il tuo vhost è stato rimosso. Per riattivare il mascheramento standard del tuo host o IP, digita /mode %s +%s
HOST_NO_VIDENT
	Il tuo IRCD non supporta i vIdent; se li supporta, invia un messaggio al team di sviluppo dei Services indicandolo come possibile bug.

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
	Comandi di %S:
	    ON          Attiva il tuo vhost
	    OFF         Disattiva il tuo vhost
	    GROUP       Sincronizza il vhost per tutti i nick nel gruppo

HOST_OPER_HELP
	
	Comandi disponibili solo ai Services Oper:
	    SET         Imposta il vhost di un altro utente
	    SETALL      Imposta il vhost per tutti i nick di un gruppo
	    DEL         Rimuove il vhost di un altro utente
	    LIST        Mostra uno o più vhost nel database

HOST_ADMIN_HELP

	Comandi disponibili solo ai Services Admin:

HOST_HELP_ON
	Sintassi: ON
	
	Attiva il vhost attualmente assegnato al nick in uso.
	Quando usi questo comando, ogni utente che effettua un
	/whois su di te vedrà il vhost invece del tuo vero indirizzo IP.

HOST_HELP_SET
	Sintassi: SET <nick> <hostmask>.

	Imposta il vhost per il nick indicato a quello della hostmask
	indicata. Se il tuo IRCD supporta i vIdent, la forma
	SET <nick> <ident>@<hostmask> permette di impostare l'ident,
	oltre che il vhost.

	Limitato ai Services oper.

HOST_HELP_DELALL
	Syntax: DELALL <nick>.

	Deletes the vhost for all nick's in the same group as
	that of the given nick.

	Limited to Host Removers.


HOST_HELP_SETALL
	Sintassi: SETALL <nick> <hostmask>.
	
	Imposta il vhost per tutti i nick nel gruppo del nick indicato.
	Se il tuo IRCD supporta i vIdent, la forma SET <nick>
	<ident>@<hostmask> permette di impostare l'ident, oltre che il
	vhost.
	
	NOTA - questo comando non aggiornerà i vhost dei nick
	aggiunti successivamente al gruppo.
	
	Limitato ai Services oper.

HOST_HELP_OFF
	Sintassi: OFF

	Disattiva il vhost attualmente assegnato al nick in uso.
	Quando usi questo comando, ogni utente che effettua un /whois
	su di te vedrà il tuo vero indirizzo IP.

HOST_HELP_DEL
	Sintassi: DEL <nick>

	Elimina dal database il vhost assegnato al nick specificato.

	Limitato ai Services oper.

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
	Sintassi: GROUP
	
	Questo comando permette agli utenti di impostare il vhost del
	loro nick ATTUALE come vhost di tutti i nick nello stesso gruppo.

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

