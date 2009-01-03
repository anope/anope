# French language file.
#	 
# (C) 2003-2008 Anope Team
# Contact us at team@anope.org
#
# Please read COPYING and README for furhter details.
#
# Based on the original code of Epona by Lara.
# Based on the original code of Services by Andy Church.
#
# Updated by Kevin@epiknet.org (03/15/2003)
# www.epiknet.org - www.botstats.com
#
# Updated by illu@rs2i.net (04/09/2005)
# irc.rs2i.net - http://www.rs2i.net
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
	Français (French)

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
	%d %b %Y %H:%M:%S %Z
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
	Dim
	Lun
	Mar
	Mer
	Jeu
	Ven
	Sam
# %A
STRFTIME_DAYS_LONG
	Dimanche
	Lundi
	Mardi
	Mercredi
	Jeudi
	Vendredi
	Samedi
# %b
STRFTIME_MONTHS_SHORT
	Jan
	Fév
	Mar
	Avr
	Mai
	Jun
	Jul
	Aoû
	Sep
	Oct
	Nov
	Déc
# %B
STRFTIME_MONTHS_LONG
	Janvier
	Février
	Mars
	Avril
	Mai
	Juin
	Juillet
	Août
	Septembre
	Octobre
	Novembre
	Décembre


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	, 


# Various error messages.
USER_RECORD_NOT_FOUND
	Erreur interne - impossible d'effectuer la commande.
UNKNOWN_COMMAND
	Commande %s inconnue.
UNKNOWN_COMMAND_HELP
	Commande %s inconnue. Tapez "%R%s HELP" pour obtenir de l'aide.
SYNTAX_ERROR
	Syntaxe: %s
MORE_INFO
	Tapez %R%s HELP %s pour plus d'informations.
NO_HELP_AVAILABLE
	Aucune aide disponible sur %s.
OBSOLETE_COMMAND
	Cette commande est dépassée, utilisez %s à la place.

BAD_USERHOST_MASK
	Le masque doit avoir le format user@host.
BAD_EXPIRY_TIME
	Echéance invalide.
USERHOST_MASK_TOO_WIDE
	La couverture de %s est trop grande, utilisez un masque plus précis.

SERVICE_OFFLINE
	%s n'est pas connecté actuellement.
READ_ONLY_MODE
	Note: les Services sont en mode lecture-seule; les changements ne seront pas sauvegardés!
PASSWORD_INCORRECT
	Mot de passe incorrect.
INVALID_TARGET
	"/msg %s" is no longer supported.  Use "/msg %s@%s" or "/%s" instead.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Accès refusé.
PERMISSION_DENIED
	Permission refusée.
RAW_DISABLED
	L'option RAW n'est pas active. Si vous voulez l'utiliser, desactiver l'option DisableRaw dans la configuration des services.

MORE_OBSCURE_PASSWORD
	Veuillez réessayer avec un mot de passe plus difficile à trouver.  Les mots de passe doivent contenir au moins cinq caractères, ne doivent pas être trop facilement devinables (par exemple votre vrai nom ou votre pseudo), et ne doit pas contenir d'espaces ou de tabulations.
PASSWORD_TOO_LONG
	Your password is too long. Please try again with a shorter password.

NICK_NOT_REGISTERED
	Votre pseudo n'est pas enregistré.
NICK_NOT_REGISTERED_HELP
	Votre pseudo n'est pas enregistré.  Tapez %R%s HELP pour des informations sur l'enregistrement des pseudos.
NICK_X_IS_SERVICES
	Le pseudo %s fait partie des services du réseau.
NICK_X_NOT_REGISTERED
	Le nick %s n'est pas enregistré.
NICK_X_IN_USE
	Le pseudo %s est déja utilisé.
NICK_X_NOT_IN_USE
	Le pseudo %s n'est actuellement pas utilisé.
NICK_X_NOT_ON_CHAN
	%s n'est actuelement pas sur %s.
NICK_X_FORBIDDEN
	Le pseudo %s ne peut être enregistré ou utilisé.
NICK_X_FORBIDDEN_OPER
	Le pseudo %s a été interdit par %s:
	%s
NICK_X_ILLEGAL
	Le pseudo %s est un pseudo illégale et ne peut pas etre utilisé
NICK_X_TRUNCATED
	Le pseudo %s a été tronqué de %d caractères.
NICK_X_SUSPENDED
	Le pseudo %s est actuelement suspendu.
CHAN_X_NOT_REGISTERED
	Le channel %s n'est pas enregistré.
CHAN_X_NOT_IN_USE
	Le channel %s n'existe pas.
CHAN_X_FORBIDDEN
	Le channel %s ne peut être enregistré ou utilisé.
CHAN_X_FORBIDDEN_OPER
	Le channel %s a été interdit par %s:
	%s
CHAN_X_SUSPENDED
	      Suspendu: [%s] %s
NICK_IDENTIFY_REQUIRED
	L'authentification par mot de passe est requise pour cette commande.
	Réessayez après avoir tapé %R%s IDENTIFY motdepasse.
CHAN_IDENTIFY_REQUIRED
	L'authentification par mot de passe est requise pour cette commande.
	Réessayez après avoir tapé %R%s IDENTIFY %s motdepasse.

MAIL_DISABLED
	Les services ont été configurés pour ne pas envoyer de mail.
MAIL_INVALID
	L'adresse e-mail de %s est invalide.
MAIL_X_INVALID
	%s n'est pas une adresse e-mail valide.
MAIL_LATER
	Impossible d'envoyer un e-mail maintenant, veuillez réessayez un peu
	plus tard.
MAIL_DELAYED
	Attendez %d secondes et réessayez.

NO_REASON
	Aucune raison
UNKNOWN
	<inconnu>

DURATION_DAY
	1 jour
DURATION_DAYS
	%d jours
DURATION_HOUR
	1 heure
DURATION_HOURS
	%d heures
DURATION_MINUTE
	1 minute
DURATION_MINUTES
	%d minutes
DURATION_SECOND
	1 seconde
DURATION_SECONDS
	%d secondes

# Human readable expiration
NO_EXPIRE
	n'expire pas
EXPIRES_SOON
	expire à la prochaine mise à jour de la base de données
EXPIRES_M
	expire dans %d minutes
EXPIRES_1M
	expire dans %d minute
EXPIRES_HM
	expire dans %d heures, %d minutes
EXPIRES_H1M
	expire dans %d heures, %d minute
EXPIRES_1HM
	expire dans %d heure, %d minutes
EXPIRES_1H1M
	expire dans %d heure, %d minute
EXPIRES_D
	expire dans %d jours
EXPIRES_1D
	expire dans %d jour

# Generic Footer message
END_OF_ANY_LIST
	Fin du %s list.

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
	Ce nick appartient à quelqu'un d'autre. Veuillez en changer.
	(Si c'est votre nick, tapez %R%s IDENTIFY motdepasse.)
NICK_IS_SECURE
	Ce pseudo est enregistré et protégé. Si c'est votre pseudo,
	tapez %R%s IDENTIFY motdepasse. 
	Sinon, veuillez choisir un pseudo différent.
NICK_MAY_NOT_BE_USED
	Ce pseudo ne peut être utilisé.  Veuillez en choisir un autre.
FORCENICKCHANGE_IN_1_MINUTE
	Si vous ne changez pas d'ici 1 minute, je changerai votre pseudo.
FORCENICKCHANGE_IN_20_SECONDS
	Si vous ne changez pas d'ici 20 secondes, je changerai votre pseudo.
FORCENICKCHANGE_NOW
	Ce pseudo a été enregistré, vous ne pouvez pas l'utiliser.
FORCENICKCHANGE_CHANGING
	Votre pseudo est maintenant changé en %s.

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER motdepasse [email]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER motdepasse email
NICK_REGISTRATION_DISABLED
	Désolés, l'enregistrement de pseudos est temporairement désactivé.
NICK_REGISTRATION_FAILED
	Désolés, l'enregistrement a échoué.
NICK_REG_PLEASE_WAIT
	Veuillez attendre %d secondes avant d'utiliser la commande REGISTER à nouveau.
NICK_CANNOT_BE_REGISTERED
	Le pseudo %s ne peut être enregistré.
NICK_ALREADY_REGISTERED
	Le pseudo %s est déjà enregistré!
NICK_REGISTERED
	Le pseudo %s est maintenant enregistré sous votre masque: %s
NICK_REGISTERED_NO_MASK
	Le pseudo %s est enregistré.
NICK_PASSWORD_IS
	Votre mot de passe est %s - notez le pour une utilisation ultérieure.

# GROUP responses
NICK_REG_DELAY
	Vous devez rester connecté plus de %d secondes pour enregistrer votre pseudo.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP cible motdepasse
NICK_GROUP_DISABLED
	Désolés, le groupage de pseudo est temporairement désactivé.
NICK_GROUP_FAILED
	Désolés, le groupage de pseudo a échoué.
NICK_GROUP_PLEASE_WAIT
	Attendez %d secondes avant d'utiliser la commande GROUP à nouveau.
NICK_GROUP_CHANGE_DISABLED
	Votre pseudo est déjà enregistré, tapez %R%s DROP d'abord.
NICK_GROUP_SAME
	Vous êtes déjà un membre du groupe de %s.
NICK_GROUP_TOO_MANY
	Il y a trop de pseudos dans le groupe de %s; listez-les et 
	effacez-en quelques uns.
	Tapez %R%s HELP GLIST et %R%s HELP DROP pour
	plus d'informations.
NICK_GROUP_JOINED
	Vous êtes maintenant dans le groupe de %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY motdepasse
NICK_IDENTIFY_FAILED
	Désolés, l'identification a échoué.
NICK_IDENTIFY_SUCCEEDED
	Mot de passe accepté - vous êtes maintenant identifié.
NICK_IDENTIFY_EMAIL_REQUIRED
	Vous devez maintenant associer un e-mail à votre pseudo.
	Cet e-mail vous permettra de retrouver votre mot de passe
	au cas où vous le perdriez.
NICK_IDENTIFY_EMAIL_HOWTO
	Tapez %R%S SET EMAIL e-mail pour définir votre e-mail.
	Votre vie privée est respectée, cet e-mail ne sera pas 
	divulgué à des tierces personnes.

# LOGOUT responses
NICK_ALREADY_IDENTIFIED
	Vous êtes déjà identifié.

# UPDATE responses
NICK_UPDATE_SUCCESS
	Status mis à jour (memos, vhost, modes, flags).
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	Votre pseudo n'est plus identifié.
NICK_LOGOUT_X_SUCCEEDED
	Le pseudo %s n'est plus identifié.

# DROP responses
NICK_LOGOUT_SERVICESADMIN
	Vous ne pouvez pas logout %s parceque c'est un administrateur
	des services.

# DROP responses
NICK_DROP_DISABLED
	Désolés, l'effacement de pseudo est temporairement désactivé.
NICK_DROPPED
	Votre pseudo a été effacé.
NICK_X_DROPPED
	Le pseudo %s a été effacé.

# SET responses
NICK_SET_SYNTAX
	SET option paramètres
NICK_SET_SERVADMIN_SYNTAX
	SET [nick] option paramètres
NICK_SET_DISABLED
	Désolés, la configuration des options du pseudo est temporairement désactivée.
NICK_SET_UNKNOWN_OPTION
	L'option SET %s est inconnue.
NICK_SET_OPTION_DISABLED
	L'option %s ne peut être changée sur ce réseau.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	Le nouvel affichage DOIT être un pseudo de votre groupe de pseudos!
NICK_SET_DISPLAY_CHANGED
	Le nouvel affichage est maintenant %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Impossible de changer votre mot de passe.
NICK_SET_PASSWORD_CHANGED
	Votre mot de passe a été changé.
NICK_SET_PASSWORD_CHANGED_TO
	Votre mot de passe est maintenant %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE nombre
NICK_SET_LANGUAGE_UNKNOWN
	Numéro de langue %d inconnu. Tapez %R%s HELP SET LANGUAGE pour une liste de langues.
NICK_SET_LANGUAGE_CHANGED
	Votre langue est maintenant le Français.

# SET URL responses
NICK_SET_URL_CHANGED
	L'adresse de votre site est maintenant %s.
NICK_SET_URL_UNSET
	L'adresse de votre site a été effacée.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	Votre adresse e-mail est maintenant %s.
NICK_SET_EMAIL_UNSET
	Votre adresse e-mail a été effacée.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	Vous ne pouvez pas effacer votre e-mail sur ce réseau.

# SET ICQ responses
NICK_SET_ICQ_CHANGED
	Votre numéro ICQ est maintenant %s.
NICK_SET_ICQ_UNSET
	Votre numéro ICQ a été effacé.
NICK_SET_ICQ_INVALID
	%s n'est pas un nombre valide.

# SET GREET responses
NICK_SET_GREET_CHANGED
	Votre message d'accueil est maintenant %s.
NICK_SET_GREET_UNSET
	Votre message d'accueil a été effacé.

# SET KILL responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	La protection est maintenant activée.
NICK_SET_KILL_QUICK
	La protection est maintenant activée, avec un délai réduit.
NICK_SET_KILL_IMMED
	La protection est maintenant activée, sans délai.
NICK_SET_KILL_IMMED_DISABLED
	L'option IMMED n'est pas disponible sur ce réseau.
NICK_SET_KILL_OFF
	La protection est maintenant désactivée.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	La sécurité est maintenant activé.
NICK_SET_SECURE_OFF
	La sécurité est maintenant désactivé.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Votre pseudo est maintenant privé.
NICK_SET_PRIVATE_OFF
	Votre pseudo est maintenant public.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Votre adresse e-mail ne sera plus affichée par la commande INFO de %s.
NICK_SET_HIDE_EMAIL_OFF
	Votre adresse e-mail sera affichée par la commande INFO de %s.
NICK_SET_HIDE_MASK_ON
	Votre masque user@host ne sera plus affiché par la commande INFO de %s.
NICK_SET_HIDE_MASK_OFF
	Votre masque user@host sera affiché par la commande INFO de %s.
NICK_SET_HIDE_QUIT_ON
	Votre dernière raison de /QUIT ne sera plus affichée par la commande INFO de %s.
NICK_SET_HIDE_QUIT_OFF
	Votre dernière raison de /QUIT sera affichée par la commande INFO de %s.

# SET NOEXPIRE responses
NICK_SET_HIDE_STATUS_ON
	Votre status sera caché dans l'affichage de la commande INFO de %s.
NICK_SET_HIDE_STATUS_OFF
	Votre status sera affiché dans l'affichage de la commande INFO de %s.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_MSG_ON
	Les services vous repondrons avec des messages.	
NICK_SET_MSG_OFF
	Les services vous repondrons avec des notice.

# SET AUTOOP responses
NICK_SET_AUTOOP_SYNTAX
	SET AUTOOP {ON | OFF}
NICK_SET_AUTOOP_ON
	Services will now autoop you in channels.
NICK_SET_AUTOOP_OFF
	Services will no longer autoop you in channels.


# SET NOEXPIRE responses
NICK_SASET_SYNTAX
	SASET pseudo option paramètres
NICK_SASET_DISABLED
	Désole, le changement d'options sur les pseudos est temporairement
	désactivé.
NICK_SASET_UNKNOWN_OPTION
	Option SASET %s inconnue.
NICK_SASET_BAD_NICK
	Le pseudo %s n'est pas enregistré.
NICK_SASET_OPTION_DISABLED
	L'option %s ne peut pas configuré sur ce réseau.

# SASET DISPLAY responses
NICK_SASET_DISPLAY_INVALID
	Le nouvel affichage pour %s DOIT etre un pseudo de son groupe!
NICK_SASET_DISPLAY_CHANGED
	Le nouvel affichage est maintenant %s.

# SASET PASSWORD responses
NICK_SASET_PASSWORD_FAILED
	Désolé, impossible de changé le mot de passe pour %s.
NICK_SASET_PASSWORD_CHANGED
	Mot de passe changé pour %s.
NICK_SASET_PASSWORD_CHANGED_TO
	Le mot de passe pour %s a été changé en %s.

# SASET LANGUAGE responses
NICK_SASET_URL_CHANGED
	L'URL pour %s a été changé en %s.
NICK_SASET_URL_UNSET
	L'URL %s a été effacé.

# SASET EMAIL responses
NICK_SASET_EMAIL_CHANGED
	L'adresse E-mail pour %s a été changé en %s.
NICK_SASET_EMAIL_UNSET
	L'adresse E-mail pour %s a été effacé.
NICK_SASET_EMAIL_UNSET_IMPOSSIBLE
	Vous ne pouvez pas configurer d'E-mail sur ce réseau.
	
# SASET ICQ responses
NICK_SASET_ICQ_CHANGED
	Le numéro ICQ pour %s a été changé en %s.
NICK_SASET_ICQ_UNSET
	Le numéro ICQ pour %s a été effacé.
NICK_SASET_ICQ_INVALID
	%s n'est pas un numéro ICQ valide.
	
# SASET GREET responses
NICK_SASET_GREET_CHANGED
	Le message de greet pour %s a été changé en %s
NICK_SASET_GREET_UNSET
	Le message de greet de %s a été effacé.

# SASET PROTECT responses
NICK_SASET_KILL_SYNTAX
	SASET pseudo KILL {ON | QUICK | OFF}
NICK_SASET_KILL_IMMED_SYNTAX
	SASET pseudo KILL {ON | QUICK | IMMED | OFF}
NICK_SASET_KILL_ON
	La protection est maintenant ACTIVE pour %s.
NICK_SASET_KILL_QUICK
	La protection est maintenant ACTIVE pour %s, avec un délais réduit.
NICK_SASET_KILL_IMMED
	La protection est maintenant ACTIVE pour %s, sans aucun délais.
NICK_SASET_KILL_IMMED_DISABLED
	L'option IMMED est pas accéssible sur ce réseau.
NICK_SASET_KILL_OFF
	La protection est maintenant INACTIVE pour %s.

# SASET SECURE responses
NICK_SASET_SECURE_SYNTAX
	SASET pseudo SECURE {ON | OFF}
NICK_SASET_SECURE_ON
	L'option sécurité est maintenant ACTIVE pour %s.
NICK_SASET_SECURE_OFF
	L'option de sécurité est maintenant INACTIVE pour %s.

# SASET PRIVATE responses
NICK_SASET_PRIVATE_SYNTAX
	SASET pseudo PRIVATE {ON | OFF}
NICK_SASET_PRIVATE_ON
	L'option privé est maintenant ACTIVE pour %s.
NICK_SASET_PRIVATE_OFF
	L'option privé est maintenant INACTIVE pour %s.

# SASET HIDE responses
NICK_SASET_HIDE_SYNTAX
	SET pseudo HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SASET_HIDE_EMAIL_ON
	L'adresse E-mail de %s sera caché dans la commande INFO de %s.
NICK_SASET_HIDE_EMAIL_OFF
	L'adresse email de %s sera maintenant affiché dans la commande INFO de %s.
NICK_SASET_HIDE_MASK_ON
	Le dernier masque user@host de %s sera maintenant caché dans la commande INFO
	de %s.
NICK_SASET_HIDE_MASK_OFF
	Le dernier masque user@host de %s sera maintenant affiché dans la commande INFO
	de %s.
NICK_SASET_HIDE_QUIT_ON
	Le dernier message de quit de %s ne sera plus affiché dans la commande INFO de %s.
NICK_SASET_HIDE_QUIT_OFF
	Le dernier message de quit de %s sera affiché dans la commande INFO de %s.
NICK_SASET_HIDE_STATUS_ON
	Le status sur les services de %s sera maintenant caché dans la commande INFO de %s.
NICK_SASET_HIDE_STATUS_OFF
	Le status sur les services de %s sera maintenant affiché dans la commande INFO de %s.
	
# SASET MSG responses
NICK_SASET_MSG_SYNTAX
	SASET pseudo PRIVATE {ON | OFF}
NICK_SASET_MSG_ON
	Les services repondront à %s par des messages.
NICK_SASET_MSG_OFF
	Les services repondront à %s par des notices.

# SET NOEXPIRE responses
NICK_SASET_NOEXPIRE_SYNTAX
	SASET pseudo NOEXPIRE {ON | OFF}
NICK_SASET_NOEXPIRE_ON
	Le pseudo %s n'expirera pas.
NICK_SASET_NOEXPIRE_OFF
	Le pseudo %s expirera.

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
	Language changed to French.


# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [mask]
NICK_ACCESS_ALREADY_PRESENT
	Le mask %s existe déjà sur votre liste d'accès.
NICK_ACCESS_REACHED_LIMIT
	Vous ne pouvez avoir que %d entrées dans la liste d'accès d'un nick.
NICK_ACCESS_ADDED
	%s ajouté à votre liste d'accès.
NICK_ACCESS_NOT_FOUND
	%s introuvable sur votre liste d'accès.
NICK_ACCESS_DELETED
	%s supprimé de votre liste d'accès.
NICK_ACCESS_LIST
	Liste d'accès:
NICK_ACCESS_LIST_X
	Liste d'accès de %s:
NICK_ACCESS_LIST_EMPTY
	Votre liste d'accès est vide.
NICK_ACCESS_LIST_X_EMPTY
	La liste d'accès de %s est vide.

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
	INFO pseudo [ALL]
NICK_INFO_REALNAME
	%s est %s
NICK_INFO_SERVICES_OPER
	%s est opérateur des services.
NICK_INFO_SERVICES_ADMIN
	%s est administrateur des services.
NICK_INFO_SERVICES_ROOT
	%s est un services root.
NICK_INFO_ADDRESS
	            Adresse: %s
NICK_INFO_ADDRESS_ONLINE
	        Connecté de: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s est actuellement connecté.
NICK_INFO_TIME_REGGED
	      Enregistré le: %s
NICK_INFO_LAST_SEEN
	  Dernière présence: %s
NICK_INFO_LAST_QUIT
	       Dernier quit: %s
NICK_INFO_URL
	Adresse de son site: %s
NICK_INFO_EMAIL
	     Adresse e-mail: %s
NICK_INFO_VHOST
	              vhost: %s
NICK_INFO_VHOST2
	              vhost: %s@%s
NICK_INFO_ICQ
	         Numéro ICQ: %d
NICK_INFO_GREET
	  Message d'accueil: %s
NICK_INFO_OPTIONS
	            Options: %s
NICK_INFO_EXPIRE
	Expire le: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Protection
NICK_INFO_OPT_SECURE
	Sécurité
NICK_INFO_OPT_PRIVATE
	Confidentialité
NICK_INFO_OPT_MSG
	Mode message
NICK_INFO_OPT_AUTOOP
	Auto-op
NICK_INFO_OPT_NONE
	Aucune
NICK_INFO_NO_EXPIRE
	Ce nick n'expirera pas.
NICK_INFO_FOR_MORE
	Pour plus d'informations, tapez %R%s INFO %s ALL.

# LIST responses
NICK_INFO_SUSPENDED
	Ce pseudo est actuelement suspendu, raison: %s
NICK_INFO_SUSPENDED_NO_REASON
	Ce pseudo est actuelement suspendu.


# LIST responses
NICK_LIST_SYNTAX
	LIST modèle
NICK_LIST_SERVADMIN_SYNTAX
	LIST modèle [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Liste des entrées correspondantes à %s:
NICK_LIST_RESULTS
	Fin de la liste - %d/%d correspondances affichées.
	
# GLIST responses
NICK_ALIST_SYNTAX
	ALIST pseudo
NICK_ALIST_HEADER
	Liste des cannaux auquels vous avez accès:
	  Num  Canal                Level    Description 
NICK_ALIST_HEADER_X
	Liste des cannaux auquels %s a accès:
	  Num  Canal                Level    Description 
NICK_ALIST_XOP_FORMAT
	  %3d %c%-20s %-8s %s
NICK_ALIST_ACCESS_FORMAT
	  %3d %c%-20s %-8d %s
NICK_ALIST_FOOTER
	Fin de la liste - %d/%d correspondances affichées.

# GLIST responses
NICK_GLIST_HEADER
	Liste des pseudos dans votre groupe:
NICK_GLIST_HEADER_X
	Liste des pseudos dans le groupe de %s:
NICK_GLIST_FOOTER
	%d pseudos dans le groupe.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (expire le %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER pseudo [motdepasse]
NICK_NO_RECOVER_SELF
	Vous ne pouvez utiliser RECOVER sur vous-même!
NICK_RECOVERED
	L'utilisateur se servant de votre pseudo a été déconnecté.
	%R%s RELEASE %s pour le récupérer avant que la %s ne s'écoule.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE pseudo [motdepasse]
NICK_RELEASE_NOT_HELD
	Le pseudo %s n'est pas en tutelle.
NICK_RELEASED
	La tutelle des Services sur votre pseudo a été enlevée.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST pseudo [motdepasse]
NICK_NO_GHOST_SELF
	Vous ne pouvez utiliser GHOST sur vous-même!
NICK_GHOST_KILLED
	L'utilisateur fantôme utilisant votre pseudo a été déconnecté.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS pseudo
NICK_GETPASS_UNAVAILABLE
	La commande GETPASS est indisponible car l'encryption est utilisée.
NICK_GETPASS_PASSWORD_IS
	Le mot de passe de %s est %s.

# SENDPASS responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host Pas de WildCards!!
NICK_GETEMAIL_EMAILS_ARE
	Emails trouvés %s pour %s.
NICK_GETEMAIL_NOT_USED
	Aucun Emails listé pour %s.
	
# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS pseudo
NICK_SENDPASS_UNAVAILABLE
	La commande SENDPASS est indisponible car l'encryption est utilisée.
NICK_SENDPASS_SUBJECT
	Mot de passe du pseudo %s
NICK_SENDPASS_HEAD
	Bonjour,
NICK_SENDPASS_LINE_1
	Vous avez demandé à recevoir le mot de passe du pseudo %s par e-mail.
NICK_SENDPASS_LINE_2
	Le mot de passe est %s Pour des raisons de sécurité, vous devriez le changer dès que vous lisez cet e-mail.
NICK_SENDPASS_LINE_3
	Si vous ne savez pas de quoi il retourne dans cet e-mail, il s'agit probablement d'une erreur, nous vous prions de nous excuser et vous demandons d'ignorer cet e-mail.
NICK_SENDPASS_LINE_4
	NE REPONDEZ PAS A CE MAIL!
NICK_SENDPASS_LINE_5
	Les administrateurs de %s.
NICK_SENDPASS_OK
	Le mot de passe de %s a été envoyé.

# FORBID responses
NICK_SUSPEND_SYNTAX
	SUSPEND pseudo raison
NICK_SUSPEND_SUCCEEDED
	Le pseudo %s est suspendu.
NICK_SUSPEND_FAILED
	Impossible de suspend le pseudo %s!

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND pseudo
NICK_UNSUSPEND_SUCCEEDED
	Le pseudo %s est liberé.
NICK_UNSUSPEND_FAILED
	Impossible de liberer le pseudo %s!

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID pseudo [raison]
NICK_FORBID_SYNTAX_REASON
	FORBID pseudo raison
NICK_FORBID_SUCCEEDED
	Le pseudo %s est désormais interdit.
NICK_FORBID_FAILED
	Impossible d'interdire le pseudo %s!

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
NICK_REQUESTED
	Une demande d'enregistrement pour ce pseudo est en cours. Merci de patienter 24 heures.
NICK_REG_RESENT
	Votre mot de passe a été renvoyé à %s.
NICK_REG_UNABLE
	Pseudo non enregistré... Re-essayez plus tard.
NICK_IS_PREREG
	Une demande d'enregistrement pour ce pseudo est en cours. La confirmation n'a pas été effectuée.
NICK_ENTER_REG_CODE
	Un passcode a été envoyé à %s, merci de taper %R%s confirm <passcode> une fois que vous l'aurez reçu pour compléter l'enregistrement.
NICK_CONFIRM_NOT_FOUND
	Aucune validation en attente... Tapez: "%R%s register <password> <email>" pour l'enregistrer.
NICK_CONFIRM_INVALID
	Passcode invalide, merci de le vérifier.
NICK_REG_MAIL_SUBJECT
	Enregistrement d'un pseudo (%s)
NICK_REG_MAIL_HEAD
	Bonjour,
NICK_REG_MAIL_LINE_1
	Vous venez de demander l'enregistrement du pseudo %s.
NICK_REG_MAIL_LINE_2
	Veuillez tapez " %R%s confirm %s " sur IRC afin de valider l'enregistrement.
NICK_REG_MAIL_LINE_3
	Si vous ne savez pas pourquoi vous avez reçu cet email, ignorez-le.
NICK_REG_MAIL_LINE_4
	NE REPONDEZ PAS A CE MESSAGE
NICK_REG_MAIL_LINE_5
	%s administrateurs.
NICK_GETPASS_PASSCODE_IS
	Le passcode pour %s est %s.
NICK_FORCE_REG
	Pseudo %s confirmé

###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	Status automatique d'opérateur du channel
CHAN_LEVEL_AUTOVOICE
	Mode +v automatique
CHAN_LEVEL_AUTOHALFOP
	Mode +h automatique
CHAN_LEVEL_AUTOPROTECT
	Mode +a automatique
CHAN_LEVEL_AUTODEOP
	Non autorisé à avoir le status d'opérateur du channel
CHAN_LEVEL_NOJOIN
	Non autorisé à joindre le channel s'il est restreint
CHAN_LEVEL_INVITE
	Autorisé à utiliser la commande INVITE
CHAN_LEVEL_AKICK
	Autorisé à utiliser la commande AKICK
CHAN_LEVEL_SET
	Autorisé à utiliser la commande SET (mais pas FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Autorisé à utiliser la commande CLEAR
CHAN_LEVEL_UNBAN
	Autorisé à utiliser la commande UNBAN
CHAN_LEVEL_OPDEOP
	Autorisé à utiliser les commandes OP/DEOP
CHAN_LEVEL_ACCESS_LIST
	Autorisé à voir la liste d'accès
CHAN_LEVEL_ACCESS_CHANGE
	Autorisé à modifier la liste d'accès
CHAN_LEVEL_MEMO
	Autorisé à lister/lire les memos
CHAN_LEVEL_ASSIGN
	Autorisé à attribuer/retirer un bot
CHAN_LEVEL_BADWORDS
	Autorisé à utiliser la commande BADWORDS
CHAN_LEVEL_NOKICK
	Jamais kické par les kickers du bot
CHAN_LEVEL_FANTASIA
	Autorisé à utiliser les commandes fantaisistes
CHAN_LEVEL_SAY
	Autorisé à utiliser les commandes ACT et SAY
CHAN_LEVEL_GREET
	Message d'accueil affiché
CHAN_LEVEL_VOICEME
	Autorisé à se (de)voicer
CHAN_LEVEL_VOICE
	Autorisé à utiliser les commandes VOICE/DEVOICE
CHAN_LEVEL_GETKEY
	Autorisé à utiliser la commande GETKEY
CHAN_LEVEL_OPDEOPME
	Autorisé à se (de)op
CHAN_LEVEL_HALFOPME
	Autorisé à se (de)halfop
CHAN_LEVEL_HALFOP
	Autorisé à utiliser les commandes HALFOP/DEHALFOP
CHAN_LEVEL_PROTECTME
	Autorisé à se (de)protéger
CHAN_LEVEL_PROTECT
	Autorisé à utiliser les commandes PROTECT/DEPROTECT
CHAN_LEVEL_KICKME
	Autorisé à se kicker
CHAN_LEVEL_KICK
	Autorisé à utiliser la commande KICK
CHAN_LEVEL_SIGNKICK
	Pas de kick signé quand SIGNKICK LEVEL est utilisé
CHAN_LEVEL_BANME
	Autorisé à se bannir
CHAN_LEVEL_BAN
	Autorisé à utiliser la commande BAN
CHAN_LEVEL_TOPIC
	Autorisé à utiliser la commande TOPIC
CHAN_LEVEL_INFO
	Autorisé à utiliser la commande INFO avec l'option ALL

# Automatic responses
CHAN_IS_REGISTERED
	Ce channel a été enregistré avec %s.
CHAN_NOT_ALLOWED_OP
	Vous ne pouvez être opérateur du channel %s.
CHAN_MAY_NOT_BE_USED
	Ce channel ne peut être utilisé.
CHAN_NOT_ALLOWED_TO_JOIN
	Vous n'êtes pas autorisé à être sur ce channel.

# REGISTER responses
CHAN_X_INVALID
	Channel %s is not a valid channel.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER canal motdepasse description
CHAN_REGISTER_DISABLED
	Désolés, l'enregistrement des cannaux est temporairement désactivé.
CHAN_REGISTER_NOT_LOCAL
	Les cannaux locaux ne peuvent être enregistrés.
CHAN_MUST_REGISTER_NICK
	Vous devez d'abord enregistrer votre pseudo.  Tapez %R%s HELP pour savoir comment enregistrer votre pseudo.
CHAN_MUST_IDENTIFY_NICK
	Vous devez d'abord vous identifier avec %s, en utilisant la commande:
	%R%s IDENTIFY motdepasse
CHAN_MAY_NOT_BE_REGISTERED
	Le canal %s ne peut être enregistré.
CHAN_ALREADY_REGISTERED
	Le canal %s est déjà enregistré!
CHAN_MUST_BE_CHANOP
	Vous devez être un opérateur du canal pour l'enregistrer.
CHAN_REACHED_CHANNEL_LIMIT
	Désolés, vous avez déjà atteint votre limite de %d canaux.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Désolés, vous avez déjà dépassé votre limite de %d canaux.
CHAN_REGISTRATION_FAILED
	Désolés, l'enregistrement a échoué.
CHAN_REGISTERED
	Le canal %s a été enregistré sous votre pseudo: %s
CHAN_PASSWORD_IS
	Le mot de passe du canal est %s - notez le pour une utilisation ultérieure.
CHAN_REGISTER_NONE_CHANNEL
	Vous avez essayé d'enregistrer le canal %s qui n'éxiste pas.
CHAN_SYMBOL_REQUIRED
	Veuillez utiliser les symbole # quand vous voulez enregistrer
	un canal.
	
# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY canal motdepasse
CHAN_IDENTIFY_FAILED
	Désolés, l'identification a échoué.
CHAN_IDENTIFY_SUCCEEDED
	Password accepté - vous avez maintenant un level d'accès sur %s équivalent 
	à celui de son propriétaire.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT canal pseudo
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT canal [pseudo]
CHAN_LOGOUT_SUCCEEDED
	L'utilisateur %s n'est plus identifié pour le canal %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	Tous les utilisateurs identifiés au canal %s ne le sont plus (except the channel founder).
CHAN_LOGOUT_FOUNDER_FAILED
	You may not log out of %s (you are founder).

# DROP responses
CHAN_DROP_SYNTAX
	DROP canal
CHAN_DROP_DISABLED
	Désolés, l'effacement de canaux est temporairement désactivé.
CHAN_DROPPED
	Le canal %s a été effacé.

# SET responses
CHAN_SET_SYNTAX
	SET canal option paramètres
CHAN_SET_DISABLED
	Désolés, la configuration des options du canal est temporairement désactivée.
CHAN_SET_UNKNOWN_OPTION
	Option SET %s inconnue.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s a trop de channels enregistrés.
CHAN_FOUNDER_CHANGED
	Le propriétaire de %s est maintenant %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Le successeur de %s est maintenant %s.
CHAN_SUCCESSOR_UNSET
	Il n'y a plus de successeur sur %s.
CHAN_SUCCESSOR_IS_FOUNDER
	%s ne peut être successeur et propriétaire du canal %s.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Impossible de changer le mot de passe.
CHAN_PASSWORD_CHANGED
	Le mot de passe de %s a été changé.
CHAN_PASSWORD_CHANGED_TO
	Le mot de passe de %s est maintenant %s.

# SET DESC responses
CHAN_DESC_CHANGED
	La description de %s est maintenant %s.

# SET URL responses
CHAN_URL_CHANGED
	L'adresse du site de %s est maintenant %s.
CHAN_URL_UNSET
	L'adresse du site de %s a été effacée.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	L'e-mail de %s est maintenant %s.
CHAN_EMAIL_UNSET
	L'e-mail de %s a été effacé.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Le message d'entrée de %s a été changé.
CHAN_ENTRY_MSG_UNSET
	Le message d'entrée de %s a été effacé.
	
# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s n'est pas un type de ban valide.
CHAN_SET_BANTYPE_CHANGED
	Le type de ban du channel %s est maintenant le numéro %d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	Mode %c ignoré car inconnu.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Mode %c ignoré car il ne peut être maintenu.
CHAN_SET_MLOCK_L_REQUIRED
	Vous devez faire garder le mode +l pour en faire autant avec le mode +L.
CHAN_SET_MLOCK_K_REQUIRED
	Vous devez faire garder le mode +i pour en faire autant avec le mode +K.
CHAN_MLOCK_CHANGED
	Les modes maintenus du canal %s sont maintenant %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET canal KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Le maintient du topic est maintenant ACTIF sur %s.
CHAN_SET_KEEPTOPIC_OFF
	Le maintient du topic est maintenant INACTIF sur %s.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET canal TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Le vérouillage du topic pour %s est maintenant ACTIF.
CHAN_SET_TOPICLOCK_OFF
	Le vérouillage du topic pour %s est maintenant INACTIF.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET canal PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Le mode 'paix' est ACTIF pour %s.
CHAN_SET_PEACE_OFF
	Le mode 'paix' est INACTIF pour %s.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET canal PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	L'option privé pour %s est maintenant ACTIVE.
CHAN_SET_PRIVATE_OFF
	L'option privé pour %s est maintenant INACTIVE.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET canal SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	La sécurité des ops pour %s est maintenant ACTIVE.
CHAN_SET_SECUREOPS_OFF
	La sécurité des ops sur %s est maintenant INACTIVE.

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET canal SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	L'option de sécurité du propriétaire pour %s est ACTIVE.
CHAN_SET_SECUREFOUNDER_OFF
	L'option de sécurité du propriétaire pour %s est INACTIVE.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET canal RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Les restrictions d'accès pour %s sont maintenant ACTIVES.
CHAN_SET_RESTRICTED_OFF
	Les restrictions d'accès pour %s sont maintenant INACTIVES.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET canal SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	L'option de sécurité pour %s est ACTIVE.
CHAN_SET_SECURE_OFF
	L'option de sécurité pour %s est INACTIVE.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET canal SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Les kicks seront maintenant signé sur %s.
CHAN_SET_SIGNKICK_LEVEL
	Les kicks seront maintenant signé sur %s en fonction de 
	l'accès de l'user sur le canal.
CHAN_SET_SIGNKICK_OFF
	Les kicks ne seront plus signé sur %s.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET canal OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	L'option 'op-notice' est maintenant ACTVE sur %s.
CHAN_SET_OPNOTICE_OFF
	L'option 'op-notice' est maintenant INACTIVE sur
	%s.

# SET OPNOTICE responses
CHAN_SET_XOP_SYNTAX
	SET canal XOP {ON | OFF}
CHAN_SET_XOP_ON
	Le système de xOP est maintenant ACTIF sur %s.
CHAN_SET_XOP_OFF
	Le système de xOP est maintenant INACTIF sur %s.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET canal NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Le canal %s n'expirera pas.
CHAN_SET_NOEXPIRE_OFF
	Le canal %s expirera.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Désolé, vous ne pouvez avoir que %d entrées sur les listes des AOPs/SOPs/VOPs d'un canal.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	Vous ne pouvez pas utiliser cette commande. 
	Utilisez la commandes ACCESS à la place.
	Tapez %R%s HELP ACCESS pour plus d'informations.
CHAN_XOP_NOT_AVAILABLE
	xOP system is not available.

# AOP messages
CHAN_AOP_SYNTAX
	AOP canal {ADD|DEL|LIST|CLEAR} [pseudo [level] | liste-d'entrées]
CHAN_AOP_DISABLED
	Désolés, la modification des listes des AOPs des canaux est temporairement désactivée.
CHAN_AOP_NICKS_ONLY
	Les listes des AOPs des canaux ne peuvent contenir que des pseudos enregistrés.
CHAN_AOP_ADDED
	%s ajouté à la liste des AOPs de %s.
CHAN_AOP_MOVED
	%s a été bougé dans la liste des AOPs de %s.
CHAN_AOP_NO_SUCH_ENTRY
	Entrée inexistante (#%d) sur la liste des AOPs de %s.
CHAN_AOP_NOT_FOUND
	%s introuvable sur la liste des AOPs de %s.
CHAN_AOP_NO_MATCH
	Aucune entrée correspondante sur la liste des AOPs de %s.
CHAN_AOP_DELETED
	%s supprimé de la liste des AOPs de %s.
CHAN_AOP_DELETED_ONE
	1 entrée supprimée de la liste des AOPs de %s.
CHAN_AOP_DELETED_SEVERAL
	%d entrées supprimées de la liste des AOPs de %s.
CHAN_AOP_LIST_EMPTY
	La liste des AOPs de %s est vide.
CHAN_AOP_LIST_HEADER
	Liste des AOPs de %s:
	  Num  Pseudo
CHAN_AOP_CLEAR
	La liste des AOPs de %s est maintenant vide.

# HOP messages
CHAN_HOP_SYNTAX
	HOP canal {ADD|DEL|LIST|CLEAR} [pseudo [level] | liste-d'entrées]
CHAN_HOP_DISABLED
	Désolés, la modification des listes des HOPs des canaux est temporairement désactivée.
CHAN_HOP_NICKS_ONLY
	Les listes des HOPs des cannaux ne peuvent contenir que des pseudos enregistrés.
CHAN_HOP_ADDED
	%s ajouté à la liste des HOPs de %s.
CHAN_HOP_MOVED
	%s a été bougé dans la liste des HOPs de %s.
CHAN_HOP_NO_SUCH_ENTRY
	Entrée inexistante (#%d) sur la liste des HOPs de %s.
CHAN_HOP_NOT_FOUND
	%s introuvable sur la liste des HOPs de %s.
CHAN_HOP_NO_MATCH
	Aucune entrée correspondante sur la liste des HOPs de %s.
CHAN_HOP_DELETED
	%s supprimé de la liste des HOPs de %s.
CHAN_HOP_DELETED_ONE
	1 entrée supprimée de la liste des HOPs de %s.
CHAN_HOP_DELETED_SEVERAL
	%d entrées supprimées de la liste des HOPs de %s.
CHAN_HOP_LIST_EMPTY
	La liste des HOPs de %s est vide.
CHAN_HOP_LIST_HEADER
	Liste des HOPs de %s:
	  Num  Pseudo
CHAN_HOP_CLEAR
	La liste des HOPs de %s est maintenant vide.

# SOP messages
CHAN_SOP_SYNTAX
	SOP canal {ADD|DEL|LIST|CLEAR} [pseudo [level] | liste-d'entrées]
CHAN_SOP_DISABLED
	Désolés, la modification des listes des SOPs des cannaux est temporairement désactivée.
CHAN_SOP_NICKS_ONLY
	Les listes des SOPs des cannaux ne peuvent contenir que des pseudos enregistrés.
CHAN_SOP_ADDED
	%s ajouté à la liste des SOPs de %s.
CHAN_SOP_MOVED
	%s a été bougé dans la liste des SOPs de %s.
CHAN_SOP_NO_SUCH_ENTRY
	Entrée inexistante (#%d) sur la liste des SOPs de %s.
CHAN_SOP_NOT_FOUND
	%s introuvable sur la liste des SOPs de %s.
CHAN_SOP_NO_MATCH
	Aucune entrée correspondante sur la liste des SOPs de %s.
CHAN_SOP_DELETED
	%s supprimé de la liste des SOPs de %s.
CHAN_SOP_DELETED_ONE
	1 entrée supprimée de la liste des SOPs de %s.
CHAN_SOP_DELETED_SEVERAL
	%d entrées supprimées de la liste des SOPs de %s.
CHAN_SOP_LIST_EMPTY
	La liste des SOPs de %s est vide.
CHAN_SOP_LIST_HEADER
	Liste des SOPs de %s:
	  Num  Pseudo
CHAN_SOP_CLEAR
	La liste des SOPs de %s est maintenant vide.

# VOP messages
CHAN_VOP_SYNTAX
	VOP canal {ADD|DEL|LIST|CLEAR} [pseudo [level] | liste-d'entrées]
CHAN_VOP_DISABLED
	Désolés, la modification des listes des VOPs des canaux est temporairement désactivée.
CHAN_VOP_NICKS_ONLY
	Les listes des VOPs des canaux ne peuvent contenir que des pseudos enregistrés.
CHAN_VOP_ADDED
	%s ajouté à la liste des VOPs de %s.
CHAN_VOP_MOVED
	%s a été bougé dans la liste des VOPs de %s.
CHAN_VOP_NO_SUCH_ENTRY
	Entrée inexistante (#%d) sur la liste des VOPs de %s.
CHAN_VOP_NOT_FOUND
	%s introuvable sur la liste des VOPs de %s.
CHAN_VOP_NO_MATCH
	Aucune entrée correspondante sur la liste des VOPs de %s.
CHAN_VOP_DELETED
	%s supprimé de la liste des VOPs de %s.
CHAN_VOP_DELETED_ONE
	1 entrée supprimée de la liste des VOPs de %s.
CHAN_VOP_DELETED_SEVERAL
	%d entrées supprimées de la liste des VOPs de %s.
CHAN_VOP_LIST_EMPTY
	La liste des VOPs de %s est vide.
CHAN_VOP_LIST_HEADER
	Liste des VOPs de %s:
	  Num  Pseudo
CHAN_VOP_CLEAR
	La liste des VOPs de %s est maintenant vide.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS canal {ADD|DEL|LIST|CLEAR} [pseudo [level] | liste-d'entrées]
CHAN_ACCESS_XOP
	Vous ne pouvez pas utiliser cette commande.
	Utilisez les commandes AOP, SOP et VOP à la place.
	Tapez %R%s HELP commande pour plus d'informations.
CHAN_ACCESS_XOP_HOP
	You can't use this command. 
	Use the AOP, SOP, HOP and VOP commands instead.
	Type %R%s HELP command for more information.
CHAN_ACCESS_DISABLED
	Désolés, la modification des listes d'accès des canaux est temporairement désactivée.
CHAN_ACCESS_LEVEL_NONZERO
	Le level d'accès doit être différent de zéro.
CHAN_ACCESS_LEVEL_RANGE
	Le level d'accès doit être compris entre %d et %d inclus.
CHAN_ACCESS_NICKS_ONLY
	Les listes d'accès des canaux ne peuvent contenir que des pseudos enregistrés.
CHAN_ACCESS_REACHED_LIMIT
	Désolés, vous ne pouvez avoir que %d entrées sur la liste d'accès d'un canal.
CHAN_ACCESS_LEVEL_UNCHANGED
	Le level d'accès de %s sur %s est inchangé de %d.
CHAN_ACCESS_LEVEL_CHANGED
	Le level d'accès de %s sur %s est maintenant de %d.
CHAN_ACCESS_ADDED
	%s ajouté à la liste d'accès de %s au level %d.
CHAN_ACCESS_NO_SUCH_ENTRY
	Entrée inexistante (#%d) sur la liste d'accès de %s.
CHAN_ACCESS_NOT_FOUND
	%s introuvable sur la liste d'accès de %s.
CHAN_ACCESS_NO_MATCH
	Aucune entrée correspondante sur la liste d'accès de %s.
CHAN_ACCESS_DELETED
	%s supprimé de la liste d'accès de %s.
CHAN_ACCESS_DELETED_ONE
	1 entrée supprimée de la liste d'accès de %s.
CHAN_ACCESS_DELETED_SEVERAL
	%d entrées supprimées de la liste d'accès de %s.
CHAN_ACCESS_LIST_EMPTY
	La liste d'accès de %s est vide.
CHAN_ACCESS_LIST_HEADER
	Liste d'accès de %s:
	  Num   Lev  Pseudo
CHAN_ACCESS_LIST_FOOTER
	Fin de liste d'accès.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	La liste d'accès de %s est maintenant vide.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK canal {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [pseudo-ou-usermask]
CHAN_AKICK_DISABLED
	Désolés, la modification de la liste de kicks automatiques du canal 
	est temporairement désactivée.
CHAN_AKICK_ALREADY_EXISTS
	%s existe déjà sur la liste de kicks automatiques de %s.
CHAN_AKICK_REACHED_LIMIT
	Désolés, vous ne pouvez avoir plus de %d entrées de kicks automatiques 
	sur un canal.
CHAN_AKICK_ADDED
	%s ajouté à la liste de kicks automatiques de %s.
CHAN_AKICK_NO_SUCH_ENTRY
	Entrée (#%d) inexistante sur la liste de kicks automatiques de %s.
CHAN_AKICK_NOT_FOUND
	%s introuvable sur la liste de kicks automatiques de %s.
CHAN_AKICK_NO_MATCH
	Aucune entrée correspondante sur la liste de kicks automatiques de %s.
CHAN_AKICK_STUCK
	%s est maintenant maintenu en permanence sur le channel %s.
CHAN_AKICK_UNSTUCK
	%s n'est plus maintenu en permanence sur le channel %s.
CHAN_AKICK_DELETED
	%s supprimé de la liste de kicks automatiques de %s.
CHAN_AKICK_DELETED_ONE
	1 entrée supprimée de la liste de kicks automatiques de %s.
CHAN_AKICK_DELETED_SEVERAL
	%d entrées supprimées de la liste de kicks automatiques de %s.
CHAN_AKICK_LIST_EMPTY
	la liste de kicks automatiques de %s est vide.
CHAN_AKICK_LIST_HEADER
	Liste de kicks automatiques de %s:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (par %s le %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (maintenu) (par %s le %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	L'application des entrées de la liste de kicks automatiques 
	pour %s est terminé, %d utilisateurs étaient concernés.
CHAN_AKICK_CLEAR
	La liste de kicks automatiques de %s est maintenant vide.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS canal {SET | DIS[ABLE] | LIST | RESET} [option [level]]
CHAN_LEVELS_XOP
	Cette commande est inutile dans ce mode.
CHAN_LEVELS_RANGE
	Le level doit être compris entre %d et %d inclus.
CHAN_LEVELS_CHANGED
	Le level pour %s du channel %s est maintenant de %d.
CHAN_LEVELS_UNKNOWN
	L'option %s est inconnue.  Tapez %R%s HELP LEVELS DESC pour une 
	liste d'options valides.
CHAN_LEVELS_DISABLED
	Le level %s est maintenant désactivé sur le canal %s.
CHAN_LEVELS_LIST_HEADER
	Options de level d'accès du channel %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (désactivé)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (propriétaire uniquement)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	Les levels d'accès de %s ont été réinitialisés à leur valeur par défaut.

# Status Messages
CHAN_STATUS_SYNTAX
	ERREUR STATUS : Erreur de syntaxe.
CHAN_STATUS_NOT_REGGED
	ERREUR STATUS : Le canal %s n'est pas enregistré.
CHAN_STATUS_FORBIDDEN
	ERREUR STATUS : Le canal %s est FORBID.
CHAN_STATUS_NOTONLINE
	ERREUR STATUS : Le pseudo %s n'est pas connecté.
CHAN_STATUS_INFO
	STATUS %s %s %d

# INFO responses
CHAN_INFO_SYNTAX
	INFO canal [ALL]
CHAN_INFO_HEADER
	Informations pour le canal %s:
CHAN_INFO_FOUNDER
	        Propriétaire: %s (%s)
CHAN_INFO_NO_FOUNDER
	        Propriétaire: %s
CHAN_INFO_SUCCESSOR
	          Successeur: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	          Successeur: %s
CHAN_INFO_DESCRIPTION
	         Description: %s
CHAN_INFO_ENTRYMSG
	    Message d'entrée: %s
CHAN_INFO_TIME_REGGED
	          Enregistré: %s
CHAN_INFO_LAST_USED
	Dernière utilisation: %s
CHAN_INFO_LAST_TOPIC
	       Dernier topic: %s
CHAN_INFO_TOPIC_SET_BY
	    Topic changé par: %s
CHAN_INFO_URL
	     Adresse du site: %s
CHAN_INFO_EMAIL
	              E-mail: %s
CHAN_INFO_BANTYPE
	         Type de ban: %d
CHAN_INFO_OPTIONS
	             Options: %s
CHAN_INFO_OPT_KEEPTOPIC
	Maintien du topic
CHAN_INFO_OPT_OPNOTICE
	Notice sur OP
CHAN_INFO_OPT_PEACE
	Paix
CHAN_INFO_OPT_PRIVATE
	Confidentialité
CHAN_INFO_OPT_RESTRICTED
	Accès restreint
CHAN_INFO_OPT_SECURE
	Sécurité
CHAN_INFO_OPT_SECUREOPS
	Contrôle des ops
CHAN_INFO_OPT_SECUREFOUNDER
	Contrôle du propriétaire
CHAN_INFO_OPT_SIGNKICK
	Kicks signés
CHAN_INFO_OPT_TOPICLOCK
	Verrouillage du topic
CHAN_INFO_OPT_XOP
	Système de listes de xOPs
CHAN_INFO_OPT_NONE
	Aucune
CHAN_INFO_MODE_LOCK
	     Modes maintenus: %s
CHAN_INFO_EXPIRE
	      Expires in: %s
CHAN_INFO_NO_EXPIRE
	Ce canal n'expirera pas.

# LIST responses
CHAN_LIST_SYNTAX
	LIST modèle
CHAN_LIST_SERVADMIN_SYNTAX
	LIST modèle [FORBIDDEN] [NOEXPIRE]
CHAN_LIST_HEADER
	Liste d'entrées correspondantes à %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Fin de la liste - %d/%d correspondances affichées.

# CLIST responses
CHAN_INVITE_SYNTAX
	INVITE canal

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN canal
CHAN_UNBANNED
	Vous avez été débanni de %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC canal [topic]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR canal raison
CHAN_CLEARED_BANS
	Tous les bans du canal %s ont été supprimés.
CHAN_CLEARED_EXCEPTS
	Tous les excepts du canal %s ont été supprimés.
CHAN_CLEARED_MODES
	Tous les modes du canal %s ont été réinitialisés.
CHAN_CLEARED_OPS
	Tous les ops du canal %s ont été enlevés.
CHAN_CLEARED_HOPS
	Tous les halfop du canal %s ont été enlevés.
CHAN_CLEARED_VOICES
	Tous les voices du canal %s ont été enlevés.
CHAN_CLEARED_USERS
	Tous les utilisateurs du canal %s ont été kickés.
CHAN_CLEARED_INVITES
	Toutes les invites du canal %s ont été enlevés.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS canal
CHAN_GETPASS_UNAVAILABLE
	La commande GETPASS est indisponible car l'encryption est utilisée.
CHAN_GETPASS_PASSWORD_IS
	Le mot de passe du canal %s est %s.
	
# SENDPASS responses
CHAN_GETKEY_SYNTAX
	GETKEY canal
CHAN_GETKEY_NOKEY
	Le canal %s n'a pas de clé.
CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS canal
CHAN_SENDPASS_UNAVAILABLE
	La commande SENDPASS est indisponible car l'encryption est utilisée.
CHAN_SENDPASS_SUBJECT
	Mot de passe du canal %s
CHAN_SENDPASS_HEAD
	Bonjour,
CHAN_SENDPASS_LINE_1
	Vous avez demandé à recevoir le mot de passe du canal %s par e-mail.
CHAN_SENDPASS_LINE_2
	Le mot de passe est %s. Pour des raisons de sécurité, vous devriez le changer dès que vous lisez ce mail.
CHAN_SENDPASS_LINE_3
	Si vous ne savez pas de quoi il retourne dans cet e-mail, il s'agit probablement d'une erreur; nous vous prions de nous excuser et vous demandons d'ignorer cet e-mail.
CHAN_SENDPASS_LINE_4
	NE REPONDEZ PAS A CE MAIL!
CHAN_SENDPASS_LINE_5
	Les administrateurs de %s.
CHAN_SENDPASS_OK
	Le mot de passe de %s a été envoyé.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID canal [raison]
CHAN_FORBID_SYNTAX_REASON
	FORBID canal raison
CHAN_FORBID_SUCCEEDED
	Le canal %s est maintenant interdit.
CHAN_FORBID_FAILED
	Impossible d'interdire le canal %s!

# Misc responses
CHAN_FORBID_REASON
	Ce canal a été FORBID.

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND canal [raison]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND canal raison
CHAN_SUSPEND_SUCCEEDED
	Le canal %s est maintenant suspendu.
CHAN_SUSPEND_FAILED
	Impossible de suspendre le canal %s!
CHAN_SUSPEND_REASON
	Ce canal est suspendu.

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND canal
CHAN_UNSUSPEND_ERROR
	Aucun # trouvé devant le nom du canal.
CHAN_UNSUSPEND_SUCCEEDED
	Le canal %s est maintenant liberé.
CHAN_UNSUSPEND_FAILED
	Impossible de liberer le canal %s!

# Misc responses
CHAN_EXCEPTED
	%s a trouvé un except sur %s et ne peut le bannir tant que l'except n'a pas été retiré.

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	Vous avez 1 nouveau memo.
MEMO_HAVE_NEW_MEMOS
	Vous avez %d nouveaux memos.
MEMO_TYPE_READ_LAST
	Tapez %R%s READ LAST pour le lire.
MEMO_TYPE_READ_NUM
	Tapez %R%s READ %d pour le lire.
MEMO_TYPE_LIST_NEW
	Tapez %R%s LIST NEW pour les lister.
MEMO_AT_LIMIT
	Attention: Vous avez atteint votre nombre maximum de memos (%d).  Vous ne pourrez plus recevoir de nouveaux memos sauf si vous supprimez quelques uns de vos memos actuels. 
MEMO_OVER_LIMIT
	Attention: Vous avez dépassé votre nombre maximum de memos (%d).  Vous ne pourrez plus recevoir de nouveaux memos sauf si vous supprimez quelques uns de vos memos actuels. 
MEMO_X_MANY_NOTICE
	Il y a %d memos sur le canal %s.
MEMO_X_ONE_NOTICE
	Il y a %d memo sur le canal %s.
MEMO_NEW_X_MEMO_ARRIVED
	Il y a un nouveau mémo dans le canal %s.
	Tapez %R%s READ %s %d pour le lire.
MEMO_NEW_MEMO_ARRIVED
	Vous avez un nouveau memo de %s.
	Tapez %R%s READ %d pour le lire.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Vous n'avez pas de memos.
MEMO_X_HAS_NO_MEMOS
	%s n'a pas de memos.
MEMO_DOES_NOT_EXIST
	Memo %d n'existe pas!
MEMO_LIST_NOT_FOUND
	Aucun memo correspondant trouvé.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {pseudo | canal} texte-du-memo
MEMO_SEND_DISABLED
	Désolés, l'envoi de memos est temporairement désactivé.
MEMO_SEND_PLEASE_WAIT
	Veuillez patienter %d secondes avant d'utiliser la commande SEND à nouveau.
MEMO_X_GETS_NO_MEMOS
	%s ne peut recevoir de memos.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s a trop de memos et ne peut en recevoir plus.
MEMO_SENT
	Memo envoyé à %s.
	
# CANCEL responses
MEMO_MASS_SENT
	Un mass memo a été envoyé a tout les pseudos enregistrés.
	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {pseudo | canal}
MEMO_CANCEL_DISABLED
	Désolés, l'annulation de memos est temporairement désactivée
MEMO_CANCEL_NONE
	Aucun memo n'était annulable.
MEMO_CANCELLED
	Le dernier memo envoyé à %s a été supprimé.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [canal] [liste | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	Vous n'avez pas de nouveaux memos.
MEMO_X_HAS_NO_NEW_MEMOS
	%s n'a pas de nouveaux memos.
MEMO_LIST_MEMOS
	Memos pour %s.  Pour lire, tapez: %R%s READ num
MEMO_LIST_NEW_MEMOS
	Nouveaux memos pour %s.  Pour lire, tapez: %R%s READ num
MEMO_LIST_CHAN_MEMOS
	Memos pour %s.  Pour lire, tapez: %R%s READ %s num
MEMO_LIST_CHAN_NEW_MEMOS
	Nouveaux memos pour %s.  Pour lire, tapez: %R%s READ %s num
MEMO_LIST_HEADER
	 Num  Expéditeur        Date/Heure
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [canal] {liste | LAST | NEW}
MEMO_HEADER
	Memo %d de %s (%s).  Pour supprimer, tapez: %R%s DEL %d
MEMO_CHAN_HEADER
	Memo %d de %s (%s).  Pour supprimer, tapez: %R%s DEL %s %d
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [canal] {num | liste | ALL}
MEMO_DELETED_NONE
	Aucun memo supprimé.
MEMO_DELETED_ONE
	Le memo %d a été supprimé.
MEMO_DELETED_SEVERAL
	Les memos %s ont été supprimés.
MEMO_DELETED_ALL
	Tous vos memos ont été supprimés.

# SET responses
MEMO_CHAN_DELETED_ALL
	Tous les mémos du canal %s ont été supprimés.

# SET responses
MEMO_SET_SYNTAX
	SET option paramètres
MEMO_SET_DISABLED
	Désolés, la configuration des options est temporairement désactivée.
MEMO_SET_UNKNOWN_OPTION
	Option SET %s inconnue.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | OFF}
MEMO_SET_NOTIFY_ON
	%s vous notifiera maintenant des memos quand vous vous connectez et quand ils vous sont envoyés.
MEMO_SET_NOTIFY_LOGON
	%s vous notifiera maintenant des memos quand vous vous connectez ou enlevez /AWAY.
MEMO_SET_NOTIFY_NEW
	%s vous notifiera maintenant des memos quand ils vous sont envoyés.
MEMO_SET_NOTIFY_OFF
	%s ne vous enverra aucune notification de memos.

# SET LIMIT responses
MEMO_SET_NOTIFY_MAIL
	Vous serez maintenant averti par mail de l'arrivé de nouveau(x) memo(s).
MEMO_SET_NOTIFY_NOMAIL
	Vous ne serez plus averti par mail de l'arrivé de nouveau(x) memo(s).
MEMO_SET_NOTIFY_INVALIDMAIL
	Vous n'avez configurer aucune adresse email pour votre pseudo.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [canal] limite
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [utilisateur | canal] {limite | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Vous n'êtes pas autorisé à changer votre limite de memos.
MEMO_SET_LIMIT_FORBIDDEN
	La limite de memos pour %s ne peut être changée.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Vous ne pouvez mettre votre limite de memos à plus de %d.
MEMO_SET_LIMIT_TOO_HIGH
	Vous ne pouvez mettre la limite de memos de %s à plus de %d.
MEMO_SET_LIMIT_OVERFLOW
	Limite de memos trop grande, limitation à %d.
MEMO_SET_YOUR_LIMIT
	Votre limite de memos a été placée à %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Vous ne pourrez désormais plus recevoir de memos.
MEMO_UNSET_YOUR_LIMIT
	Votre limite de memos a été désactivée.
MEMO_SET_LIMIT
	Limite de memos de %s définie à %d.
MEMO_SET_LIMIT_ZERO
	Limite de memos de %s définie à 0.
MEMO_UNSET_LIMIT
	Limite de memos de %s désactivée.

# INFO responses
MEMO_INFO_SYNTAX
	INFO [canal]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [pseudo | canal]
MEMO_INFO_NO_MEMOS
	Vous n'avez aucun memo actuellement.
MEMO_INFO_MEMO
	Vous avez actuellement 1 memo.
MEMO_INFO_MEMO_UNREAD
	Vous avez actuellement 1 memo, qui n'a pas encore été lu.
MEMO_INFO_MEMOS
	Vous avez actuellement %d memos.
MEMO_INFO_MEMOS_ONE_UNREAD
	Vous avez actuellement %d memos, desquels 1 n'a pas été lu.
MEMO_INFO_MEMOS_SOME_UNREAD
	Vous avez actuellement %d memos, desquels %d n'ont pas été lus.
MEMO_INFO_MEMOS_ALL_UNREAD
	Vous avez actuellement %d memos, aucun d'entre eux n'a été lu.
MEMO_INFO_LIMIT
	Votre limite de memos est de %d.
MEMO_INFO_HARD_LIMIT
	Votre limite de memos est de %d, et ne peut être changée.
MEMO_INFO_LIMIT_ZERO
	Votre limite de memos est de 0, vous ne recevrez aucun nouveau memo.
MEMO_INFO_HARD_LIMIT_ZERO
	Votre limite de memos est de 0, vous ne recevrez aucun nouveau memo.  Vous ne pouvez pas changer cette limite.
MEMO_INFO_NO_LIMIT
	Vous n'avez aucune limite sur le nombre de memos que vous pouvez garder.
MEMO_INFO_NOTIFY_OFF
	Vous ne serez pas notifié des nouveaux memos.
MEMO_INFO_NOTIFY_ON
	Vous serez notifié de nouveaux memos à la connexion et quand ils arrivent.
MEMO_INFO_NOTIFY_RECEIVE
	Vous serez notifié quand de nouveaux memos arrivent.
MEMO_INFO_NOTIFY_SIGNON
	Vous serez notifié de nouveaux memos à la connexion.
MEMO_INFO_X_NO_MEMOS
	%s n'a actuellement aucun memo.
MEMO_INFO_X_MEMO
	%s a actuellement 1 memo.
MEMO_INFO_X_MEMO_UNREAD
	%s a actuellement 1 memo, qui n'a pas encore été lu.
MEMO_INFO_X_MEMOS
	%s a actuellement %d memos.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s a actuellement %d memos, desquels 1 n'a pas été lu.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s a actuellement %d memos, desquels %d n'ont pas été lus.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s a actuellement %d memos, desquels aucun n'a été lu.
MEMO_INFO_X_LIMIT
	La limite de memos de %s est de %d.
MEMO_INFO_X_HARD_LIMIT
	La limite de memos de %s est de %d, qui ne peut pas être changée.
MEMO_INFO_X_NO_LIMIT
	%s n'a aucune limitation de memos.
MEMO_INFO_X_NOTIFY_OFF
	%s n'est pas notifié de nouveaux memos.
MEMO_INFO_X_NOTIFY_ON
	%s est notifié de nouveaux memos quand ils arrivent et à la connexion.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s est notifié quand de nouveaux memos arrivent.
MEMO_INFO_X_NOTIFY_SIGNON
	%s est notifié de nouveaux memos à la connexion.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_MAIL_SUBJECT
	Nouveau memo
MEMO_MAIL_TEXT1
	Bonjour %s,
MEMO_MAIL_TEXT2
	Vous venez de recevoir un memo de %s. Le numéro de ce memo est %d.
MEMO_MAIL_TEXT3
	Memo:

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_RSEND_PLEASE_WAIT
	Veuillez attendre %d secondes avant d'utiliser RSEND à nouveau.
MEMO_RSEND_DISABLED
	Désolé, RSEND est désactivé sur ce réseau.
MEMO_RSEND_SYNTAX
	RSEND {pseudo | canal} memo-text
MEMO_RSEND_NICK_MEMO_TEXT
	[auto-memo] Le memo que vous avez envoyé a été vu.
MEMO_RSEND_CHAN_MEMO_TEXT
	[auto-memo] Le memo que vous avez envoyé a %s a été vu.
MEMO_RSEND_USER_NOTIFICATION
	Un accusé de reception a été envoyé a %s pour l'informer que vous avez lu son
	memo.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_CHECK_SYNTAX
	CHECK pseudo
MEMO_CHECK_NOT_READ
	Le dernier mémo que vous avez envoyé à %s (le %s) n'a pas encore été lu.
MEMO_CHECK_READ
	Le dernier mémo que vous avez envoyé à %s (le %s) a été lu.
MEMO_CHECK_NO_MEMO
	Le pseudo %s n'a aucun memo de votre part.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
MEMO_NO_RSEND_SELF
	Vous ne pouvez pas recevoir d'accusé de reception quand vous vous
	ètes envoyé un message a vous meme.

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
BOT_DOES_NOT_EXIST
	Le bot %s n'existe pas.
BOT_NOT_ASSIGNED
	Vous devez attribuer un bot au canal avant d'utiliser 
	cette commande.
	Tapez %R%S HELP ASSIGN pour plus d'informations.
BOT_NOT_ON_CHANNEL
	Le bot n'est pas sur le canal %s.
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	Ne dites pas %s sur ce channel!
BOT_REASON_BADWORD_GENTLE
	Surveillez vos paroles!
BOT_REASON_BOLD
	N'utilisez pas de caractères gras sur ce channel!
BOT_REASON_CAPS
	Cessez d'utiliser tant de majuscules!
BOT_REASON_COLOR
	N'utilisez pas de couleurs sur ce channel!
BOT_REASON_FLOOD
	Arrêtez d'envoyer autant de messages!
BOT_REASON_REPEAT
	Arrêtez de vous répéter!
BOT_REASON_REVERSE
	N'utilisez pas de caractères inversés sur ce channel!
BOT_REASON_UNDERLINE
	N'utilisez pas de caractères soulignés sur ce channel!
	
# !seen replies
BOT_SEEN_BOT
	Je suis là, %s!
BOT_SEEN_YOU
	%s est en pleine recherche spirituelle...
BOT_SEEN_ON_CHANNEL
	%s est sur le channel actuellement!
BOT_SEEN_ON_CHANNEL_AS
	%s est sur le channel actuellement (c'est %s) ! 
BOT_SEEN_ON
	%s a été vu pour la dernière fois ici il y a %s.
BOT_SEEN_NEVER
	Je n'ai jamais vu %s sur ce channel.
BOT_SEEN_UNKNOWN
	Je ne sais pas qui est %s.

# BOT responses
BOT_BOT_SYNTAX
	BOT ADD pseudo user host real
	BOT CHANGE ancien-pseudo nouveau-pseudo [user [host [real]]]
	BOT DEL pseudo
BOT_BOT_ALREADY_EXISTS
	Le bot %s existe déjà.
BOT_BOT_CREATION_FAILED
	Désolés, la création du bot a échoué.
BOT_BOT_READONLY
	Désolés, la modification des bots est temporairement désactivée.
BOT_BOT_ADDED
	%s!%s@%s (%s) ajouté à la liste des bots.
BOT_BOT_ANY_CHANGES
	L'ancienne information est égale à la nouvelle.
BOT_BOT_CHANGED
	Le bot %s a été changé en %s!%s@%s (%s)
BOT_BOT_DELETED
	Le bot %s a été supprimé.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Liste des bots:
BOT_BOTLIST_PRIVATE_HEADER
	Bots réservés aux opérateurs IRC:
BOT_BOTLIST_FOOTER
	%d bots disponibles.
BOT_BOTLIST_EMPTY
	Il n'y a aucun bot disponible actuellement.
	Demandez à un administrateur des services d'en créer un!
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN canal pseudo
BOT_ASSIGN_READONLY
	Désolés, l'attribution des bots est temporairement désactivée.
BOT_ASSIGN_ALREADY
	Le bot %s est déja assigné au canal %s.
BOT_ASSIGN_ASSIGNED
	Le bot %s a été attribué à %s.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN canal
BOT_UNASSIGN_UNASSIGNED
	Il n'y a plus de bot attribué à %s.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {canal | pseudo}
BOT_INFO_NOT_FOUND
	%s n'est pas un bot ou canal enregistré valide.
BOT_INFO_BOT_HEADER
	Information pour le bot %s:
BOT_INFO_BOT_MASK
	      Masque : %s@%s
BOT_INFO_BOT_REALNAME
	    Vrai nom : %s
BOT_INFO_BOT_CREATED
	        Créé : %s
BOT_INFO_BOT_USAGE
	 Utilisé sur : %d channel(s)
BOT_INFO_BOT_OPTIONS
	     Options : %s
BOT_INFO_OPT_PRIVATE
	Privé
BOT_INFO_CHAN_HEADER
	Information pour le channel %s:
BOT_INFO_CHAN_BOT
	             Nick du bot : %s
BOT_INFO_CHAN_BOT_NONE
	             Nick du bot : non attribué.
BOT_INFO_CHAN_KICK_BADWORDS
	Kicker de mots interdits : %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	Kicker de mots interdits : %s (%d kick(s) pour un ban)
BOT_INFO_CHAN_KICK_BOLDS
	     Kicker de car. gras : %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	     Kicker de car. gras : %s (%d kick(s) pour un ban)
BOT_INFO_CHAN_KICK_CAPS_ON
	    Kicker de majuscules : %s (minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	    Kicker de majuscules : %s (%d kick(s) pour un ban; minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	    Kicker de majuscules : %s
BOT_INFO_CHAN_KICK_COLORS
	      Kicker de couleurs : %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	      Kicker de couleurs : %s (%d kick(s) pour un ban)
BOT_INFO_CHAN_KICK_FLOOD_ON
	         Kicker de flood : %s (%d lignes en %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	         Kicker de flood : %s (%d kick(s) pour un ban, %d lignes en %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	         Kicker de flood : %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	    Kicker de répétition : %s (%d fois)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	    Kicker de répétition : %s (%d kick(s) pour un ban, %d fois)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	    Kicker de répétition : %s
BOT_INFO_CHAN_KICK_REVERSES
	      Kicker de reverses : %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	      Kicker de reverses : %s (%d kick(s) pour un ban)
BOT_INFO_CHAN_KICK_UNDERLINES
	Kicker de car. soulignés : %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	Kicker de car. soulignés : %s (%d kick(s) pour un ban)
BOT_INFO_ACTIVE
	actif
BOT_INFO_INACTIVE
	inactif
BOT_INFO_CHAN_OPTIONS
	                 Options : %s
BOT_INFO_OPT_DONTKICKOPS
	Protection des ops
BOT_INFO_OPT_DONTKICKVOICES
	Protection des voices
BOT_INFO_OPT_FANTASY
	Fantaisie
BOT_INFO_OPT_GREET
	Message d'accueil
BOT_INFO_OPT_NOBOT
	Pas de bot
BOT_INFO_OPT_SYMBIOSIS
	Symbiose
BOT_INFO_OPT_NONE
	Aucune
	
# SET responses
BOT_SET_SYNTAX
	SET canal option paramètres
BOT_SET_DISABLED
	Désolés, la configuration des options du bot est 
	temporairement désactivée.
BOT_SET_UNKNOWN
	Option %s inconnue.
	Tapez %R%S HELP SET pour plus d'informations.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET canal DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Le bot ne kickera pas les ops du canal %s.
BOT_SET_DONTKICKOPS_OFF
	Le bot kickera les ops du canal %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET canal DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Le bot ne kickera pas les voices du canal %s.
BOT_SET_DONTKICKVOICES_OFF
	Le bot kickera les voices du canal %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET canal FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	La fantaisie est maintenant activée sur le canal %s.
BOT_SET_FANTASY_OFF
	La fantaisie est maintenant désactivée sur le canal %s.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET canal GREET {ON|OFF}
BOT_SET_GREET_ON
	Le message d'accueil est maintenant activé sur le canal %s.
BOT_SET_GREET_OFF
	Le message d'accueil est maintenant désactivé sur le canal %s.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET canal NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Le mode sans bot est maintenant activé sur le canal %s.
BOT_SET_NOBOT_OFF
	Le mode sans bot est maintenant désactivé sur le canal %s.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET canal PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Le mode privé du bot %s est maintenant activé.
BOT_SET_PRIVATE_OFF
	Le mode privé du bot %s est maintenant désactivé.
	
# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET canal SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	La symbiose est maintenant activée sur le canal %s.
BOT_SET_SYMBIOSIS_OFF
	La symbiose est maintenant désactivée sur le canal %s.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK canal option {ON|OFF} [paramètres]
BOT_KICK_DISABLED
	Désolés, la configuration des kickers est temporairement
	désactivée.
BOT_KICK_UNKNOWN
	Option %s inconnue.
	Tapez %R%S HELP KICK pour plus d'informations.
BOT_KICK_BAD_TTB
	%s ne peut être pris comme nombre de fois pour un ban.

BOT_KICK_BADWORDS_ON
	Le bot kickera maintenant les mots interdits. Utilisez
	la commande BADWORDS pour ajouter ou supprimer un mot.
BOT_KICK_BADWORDS_ON_BAN
	Le bot kickera maintenant les mots interdits, et mettra
	un ban après %d kicks pour le même utilisateur. Utilisez
	la commande BADWORDS pour ajouter ou supprimer un mot.
BOT_KICK_BADWORDS_OFF
	Le bot ne kickera plus les mots interdits.
BOT_KICK_BOLDS_ON
	Le bot kickera maintenant les caractères gras.
BOT_KICK_BOLDS_ON_BAN
	Le bot kickera maintenant les caractères gras, et mettra
	un ban après %d kicks pour le même utilisateur.
BOT_KICK_BOLDS_OFF
	Le bot ne kickera plus les caractères gras.
BOT_KICK_CAPS_ON
	Le bot kickera maintenant les majuscules (qui doivent
	constituer au moins %d caractères et %d%% du message 
	entier).
BOT_KICK_CAPS_ON_BAN
	Le bot kickera maintenant les majuscules (qui doivent
	constituer au moins %d caractères et %d%% du message 
	entier), et mettra un ban après %d kicks pour le même 
	utilisateur.
BOT_KICK_CAPS_OFF
	Le bot ne kickera plus les majuscules.
BOT_KICK_COLORS_ON
	Le bot kickera maintenant les couleurs.
BOT_KICK_COLORS_ON_BAN
	Le bot kickera maintenant les couleurs, et mettra
	un ban après %d kicks pour le même utilisateur.
BOT_KICK_COLORS_OFF
	Le bot ne kickera plus les couleurs.
BOT_KICK_FLOOD_ON
	Le bot kickera maintenant le flood (%d lignes en %d 
	secondes).
BOT_KICK_FLOOD_ON_BAN
	Le bot kickera maintenant le flood (%d lignes en %d 
	secondes), et mettra un ban après %d kicks pour le 
	même utilisateur.
BOT_KICK_FLOOD_OFF
	Le bot ne kickera plus le flood.
BOT_KICK_REPEAT_ON
	Le bot kickera maintenant les répétitions (les 
	utilisateurs qui disent %d fois la même chose).
BOT_KICK_REPEAT_ON_BAN
	Le bot kickera maintenant les répétitions (les 
	utilisateurs qui disent %d fois la même chose), et 
	mettra un ban après %d kicks pour le  même utilisateur.
BOT_KICK_REPEAT_OFF
	Le bot ne kickera plus les répétitions.
BOT_KICK_REVERSES_ON
	Le bot kickera maintenant les reverses.
BOT_KICK_REVERSES_ON_BAN
	Le bot kickera maintenant les reverses, et mettra
	un ban après %d kicks pour le même utilisateur.
BOT_KICK_REVERSES_OFF
	Le bot ne kickera plus les reverses.
BOT_KICK_UNDERLINES_ON
	Le bot kickera maintenant les caractères soulignés.
BOT_KICK_UNDERLINES_ON_BAN
	Le bot kickera maintenant les caractères soulignés, 
	et mettra un ban après %d kicks pour le même 
	utilisateur.
BOT_KICK_UNDERLINES_OFF
	Le bot ne kickera plus les caractères soulignés.
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS canal {ADD|DEL|LIST|CLEAR} [mot | liste-d'entrées] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	Désolés, la modification de la liste des mots interdits
	est temporairement désactivée.
BOT_BADWORDS_REACHED_LIMIT
	Désolés, vous ne pouvez avoir que %d entrées sur la
	liste des mots interdits.
BOT_BADWORDS_ALREADY_EXISTS
	%s existe déjà dans la liste des mots interdits de %s.
BOT_BADWORDS_ADDED
	%s ajouté à la liste des mots interdits de %s.
BOT_BADWORDS_NO_SUCH_ENTRY
	Entrée (#%d) introuvable sur la liste des mots 
	interdits de %s.
BOT_BADWORDS_NOT_FOUND
	%s introuvable sur la liste des mots interdits de %s.
BOT_BADWORDS_NO_MATCH
	Aucune entrée correspondante sur la liste des mots interdits 
	de %s.
BOT_BADWORDS_DELETED
	%s supprimé de la liste des mots interdits de %s.
BOT_BADWORDS_DELETED_ONE
	1 entrée supprimée de la liste des mots interdits de %s.
BOT_BADWORDS_DELETED_SEVERAL
	%d entrées supprimées de la liste des mots interdits de %s.
BOT_BADWORDS_LIST_EMPTY
	La liste des mots interdits de %s est vide.
BOT_BADWORDS_LIST_HEADER
	Liste des mots interdits de %s:
	  Num   Mot                            Type
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	La liste des mots interdits est maintenant vide.

# SAY responses
BOT_SAY_SYNTAX
	SAY canal texte
	
# ACT responses
BOT_ACT_SYNTAX
	ACT canal texte

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
BOT_EXCEPT
	Utilisateur trouvé dans les excepts.

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
BOT_BAD_NICK
	Les pseudos des bots ne peuvent contenir que des caractéres valide.
BOT_BAD_HOST
	Les hosts des bots doivent contenir uniquement des caractéres valide.
BOT_BAD_IDENT
	Les identds des bots doivent contenir que des caractères valide.
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
	Les Services ne peuvent changer les modes. Vos serveurs sont-ils configurés correctement?
OPER_BOUNCY_MODES_U_LINE
	Les Services ne peuvent changer les modes. Les U:lines de vos serveurs sont-elles configurées correctement?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL message

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	Option STATS inconnue %s.
OPER_STATS_CURRENT_USERS
	Utilisateurs courants: %d (%d ops)
OPER_STATS_MAX_USERS
	Utilisateurs maximum: %d (%s)
# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Les services tournent depuis %d jours, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Les services tournent depuis %d jour, %02d:%02d
OPER_STATS_UPTIME_HMS
	Les services tournent depuis %d heures, %d minutes
OPER_STATS_UPTIME_HM1S
	Les services tournent depuis %d heures, %d minutes
OPER_STATS_UPTIME_H1MS
	Les services tournent depuis %d heures, %d minute
OPER_STATS_UPTIME_H1M1S
	Les services tournent depuis %d heures, %d minute
OPER_STATS_UPTIME_1HMS
	Les services tournent depuis %d heure, %d minutes
OPER_STATS_UPTIME_1HM1S
	Les services tournent depuis %d heure, %d minutes
OPER_STATS_UPTIME_1H1MS
	Les services tournent depuis %d heure, %d minute
OPER_STATS_UPTIME_1H1M1S
	Les services tournent depuis %d heure, %d minute
OPER_STATS_UPTIME_MS
	Les services tournent depuis %d minutes, %d secondes
OPER_STATS_UPTIME_M1S
	Les services tournent depuis %d minutes, %d seconde
OPER_STATS_UPTIME_1MS
	Les services tournent depuis %d minute, %d secondes
OPER_STATS_UPTIME_1M1S
	Les services tournent depuis %d minute, %d seconde
OPER_STATS_BYTES_READ
	Octets lus    : %5d ko
OPER_STATS_BYTES_WRITTEN
	Octets écrits : %5d ko
OPER_STATS_USER_MEM
	Utilisateur   : %6d enregistrements, %5d ko
OPER_STATS_CHANNEL_MEM
	Channel       : %6d enregistrements, %5d ko
OPER_STATS_GROUPS_MEM
	Groupes NS    : %6d enregistrements, %5d ko
OPER_STATS_ALIASES_MEM
	Aliases NS    : %6d enregistrements, %5d ko
OPER_STATS_CHANSERV_MEM
	ChanServ      : %6d enregistrements, %5d ko
OPER_STATS_BOTSERV_MEM
	BotServ       : %6d enregistrements, %5d ko
OPER_STATS_HOSTSERV_MEM
	HostServ      : %6d enregistrements, %5d ko
OPER_STATS_OPERSERV_MEM
	OperServ      : %6d enregistrements, %5d ko
OPER_STATS_SESSIONS_MEM
	Sessions      : %6d enregistrements, %5d ko
OPER_STATS_AKILL_COUNT
	Nombre d'AKILLs actuel: %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Temps d'échéance des AKILLs par défaut: %d jours
OPER_STATS_AKILL_EXPIRE_DAY
	Temps d'échéance des AKILLs par défaut: 1 jour
OPER_STATS_AKILL_EXPIRE_HOURS
	Temps d'échéance des AKILLs par défaut: %d heures
OPER_STATS_AKILL_EXPIRE_HOUR
	Temps d'échéance des AKILLs par défaut: 1 heure
OPER_STATS_AKILL_EXPIRE_MINS
	Temps d'échéance des AKILLs par défaut: %d minutes
OPER_STATS_AKILL_EXPIRE_MIN
	Temps d'échéance des AKILLs par défaut: 1 minute
OPER_STATS_AKILL_EXPIRE_NONE
	Temps d'échéance des AKILLs par défaut: Pas d'échéance
OPER_STATS_SGLINE_COUNT
	Nombre de SGLINEs actuel: %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Temps d'échéance des SGLINEs par défaut: %d jours
OPER_STATS_SGLINE_EXPIRE_DAY
	Temps d'échéance des SGLINEs par défaut: 1 jour
OPER_STATS_SGLINE_EXPIRE_HOURS
	Temps d'échéance des SGLINEs par défaut: %d heures
OPER_STATS_SGLINE_EXPIRE_HOUR
	Temps d'échéance des SGLINEs par défaut: 1 heure
OPER_STATS_SGLINE_EXPIRE_MINS
	Temps d'échéance des SGLINEs par défaut: %d minutes
OPER_STATS_SGLINE_EXPIRE_MIN
	Temps d'échéance des SGLINEs par défaut: 1 minute
OPER_STATS_SGLINE_EXPIRE_NONE
	Temps d'échéance des SGLINEs par défaut: Pas d'échéance
OPER_STATS_SQLINE_COUNT
	Nombre de SQLINEs actuel: %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Temps d'échéance des SQLINEs par défaut: %d jours
OPER_STATS_SQLINE_EXPIRE_DAY
	Temps d'échéance des SQLINEs par défaut: 1 jour
OPER_STATS_SQLINE_EXPIRE_HOURS
	Temps d'échéance des SQLINEs par défaut: %d heures
OPER_STATS_SQLINE_EXPIRE_HOUR
	Temps d'échéance des SQLINEs par défaut: 1 heure
OPER_STATS_SQLINE_EXPIRE_MINS
	Temps d'échéance des SQLINEs par défaut: %d minutes
OPER_STATS_SQLINE_EXPIRE_MIN
	Temps d'échéance des SQLINEs par défaut: 1 minute
OPER_STATS_SQLINE_EXPIRE_NONE
	Temps d'échéance des SQLINEs par défaut: Pas d'échéance
OPER_STATS_SZLINE_COUNT
	Nombre de SZLINEs actuel: %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Temps d'échéance des SZLINEs par défaut: %d jours
OPER_STATS_SZLINE_EXPIRE_DAY
	Temps d'échéance des SZLINEs par défaut: 1 jour
OPER_STATS_SZLINE_EXPIRE_HOURS
	Temps d'échéance des SZLINEs par défaut: %d heures
OPER_STATS_SZLINE_EXPIRE_HOUR
	Temps d'échéance des SZLINEs par défaut: 1 heure
OPER_STATS_SZLINE_EXPIRE_MINS
	Temps d'échéance des SZLINEs par défaut: %d minutes
OPER_STATS_SZLINE_EXPIRE_MIN
	Temps d'échéance des SZLINEs par défaut: 1 minute
OPER_STATS_SZLINE_EXPIRE_NONE
	Temps d'échéance des SZLINEs par défaut: Pas d'échéance
OPER_STATS_RESET
	Statistiques réinitialisées.

# MODE responses
OPER_STATS_UPLINK_SERVER
	Connecté à: %s
OPER_STATS_UPLINK_CAPAB
	Caractéristiques du link: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Serveurs trouvés: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE canal modes

# CLEARMODES responses
OPER_UMODE_SYNTAX
	UMODE pseudo modes

# OLINE responses
OPER_UMODE_SUCCESS
	Mode de %s changés.
OPER_UMODE_CHANGED
	%s a changé vos modes.

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE pseudo flags
OPER_OLINE_SUCCESS
	Une O:LINE avec les flags %s a été ajoutée pour %s.

# CLEARMODES responses
OPER_OLINE_IRCOP
	Vous êtes maintenant IRCOp.

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES canal [ALL]
OPER_CLEARMODES_DONE
	Les modes binaires et les bans sont supprimés de %s.
OPER_CLEARMODES_ALL_DONE
	Tous les modes supprimés du channel %s.


# KICK responses
OPER_KICK_SYNTAX
	KICK canal utilisateur raison

# ADMIN responses
OPER_SVSNICK_SYNTAX
	SVSNICK pseudo nouveau-pseudo 
OPER_SVSNICK_NEWNICK
	Le pseudo de %s a été changé en %s.
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [pseudo | entry-list]
OPER_ADMIN_SKELETON
	Les services sont en mode skeleton, la commande ADMIN n'est pas disponible.
OPER_ADMIN_EXISTS
	%s est déjà sur la liste des Services admins.
OPER_ADMIN_REACHED_LIMIT
	Vous ne pouvez avoir que %d Services admins.
OPER_ADMIN_ADDED
	%s a été ajouté à la liste des Services admins.
OPER_ADMIN_NOT_FOUND
	%s est introuvable sur la liste des Services admins.
OPER_ADMIN_NO_MATCH
	Aucune entrée correspondante sur la liste des Services admins.
OPER_ADMIN_DELETED
	%s a été supprimé de la liste des Services admins.
OPER_ADMIN_DELETED_ONE
	1 entrée a été supprimée de la liste des Services admins.
OPER_ADMIN_DELETED_SEVERAL
	%d entrées ont été supprimées de la liste des Services admins.
OPER_ADMIN_LIST_EMPTY
	La liste des Services admins est vide.
OPER_ADMIN_LIST_HEADER
	Liste des Services admins:
	  Num   Pseudo
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	La liste des Services admins a été vidée.
OPER_ADMIN_MOVED
	%s a été retiré de la liste des services admins.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [pseudo | entry-list]
OPER_OPER_SKELETON
	Les services sont en mode skeleton, la commande OPER n'est pas disponible.
OPER_OPER_EXISTS
	%s est déjà sur la liste des Services oper.
OPER_OPER_REACHED_LIMIT
	Vous ne pouvez avoir que %d Services oper.
OPER_OPER_ADDED
	%s a été ajouté à la liste des Services oper.
OPER_OPER_NOT_FOUND
	%s est introuvable sur la liste des Services oper.
OPER_OPER_NO_MATCH
	Aucune entrée correspondante sur la liste des Services oper.
OPER_OPER_DELETED
	%s a été supprimé de la liste des Services oper.
OPER_OPER_DELETED_ONE
	1 entrée a été supprimée de la liste des Services oper.
OPER_OPER_DELETED_SEVERAL
	%d entrées ont été supprimées de la liste des Services oper.
OPER_OPER_LIST_EMPTY
	La liste des Services oper est vide.
OPER_OPER_LIST_HEADER
	Liste des Services oper:
	  Num   Pseudo
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	La liste des Services oper a été vidée.
OPER_OPER_MOVED
	%s a été bougé dans la liste des services oper.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+échéance] {masque | liste d'entrées} [raison]]
OPER_AKILL_EXISTS
	%s existe déjà sur la liste d'AKILL.
OPER_AKILL_ALREADY_COVERED
	%s est déjà couvert par %s.
OPER_AKILL_REACHED_LIMIT
	Désolé, vous ne pouvez avoir que %d AKILLs.
OPER_AKILL_NO_NICK
	Rappel: les masques AKILL ne peuvent contenir de pseudo, assurez vous que vous n'avez pas mis de partie pseudo dans le masque.
OPER_AKILL_ADDED
	%s ajouté à la liste d'AKILL.
OPER_AKILL_CHANGED
	L'échéance de %s a été changée.
OPER_AKILL_NOT_FOUND
	%s introuvable sur la liste d'AKILL.
OPER_AKILL_NO_MATCH
	Aucune entrée correspondante sur la liste d'AKILL.
OPER_AKILL_DELETED
	%s supprimé de la liste d'AKILL.
OPER_AKILL_DELETED_ONE
	1 entrée supprimée de la liste d'AKILL.
OPER_AKILL_DELETED_SEVERAL
	%d entrées supprimées de la liste d'AKILL.
OPER_AKILL_LIST_EMPTY
	La liste d'AKILL est vide.
OPER_AKILL_LIST_HEADER
	Liste d'AKILL actuelle:
	  Num   Masque                            Raison
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Liste d'AKILL actuelle:
# number, mask, set-by, set-time, expires, reason
OPER_AKILL_VIEW_FORMAT
	%3d  %s (par %s le %s; %s)
	      %s
OPER_AKILL_CLEAR
	La liste d'AKILL a été vidée.

# SGLINE responses
OPER_CHANKILL_SYNTAX
	CHANKILL [+expiry] {#canal} [raison]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+échéance] {masque | liste d'entrées} [raison]]
OPER_SGLINE_UNSUPPORTED
	Désolé, SGLINE n'est pas disponible sur ce réseau.
OPER_SGLINE_EXISTS
	%s existe déjà sur la liste de SGLINEs.
OPER_SGLINE_ALREADY_COVERED
	%s est déjà couvert par %s.
OPER_SGLINE_REACHED_LIMIT
	Désolé, vous ne pouvez avoir que %d SGLINEs.
OPER_SGLINE_ADDED
	%s ajouté à la liste de SGLINEs.
OPER_SGLINE_CHANGED
	L'échéance de %s a été changée.
OPER_SGLINE_NOT_FOUND
	%s introuvable sur la liste de SGLINEs.
OPER_SGLINE_NO_MATCH
	Aucune entrée correspondante sur la liste de SGLINEs.
OPER_SGLINE_DELETED
	%s supprimé de la liste de SGLINEs.
OPER_SGLINE_DELETED_ONE
	1 entrée supprimée de la liste de SGLINEs.
OPER_SGLINE_DELETED_SEVERAL
	%d entrées supprimées de la liste de SGLINEs.
OPER_SGLINE_LIST_EMPTY
	La liste de SGLINEs est vide.
OPER_SGLINE_LIST_HEADER
	Liste de SGLINEs actuelle:
	  Num   Masque                            Raison
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Liste de SGLINEs actuelle:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (par %s le %s; %s)
	      %s
OPER_SGLINE_CLEAR
	La liste de SGLINEs a été vidée.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+échéance] {masque | liste d'entrées}[:raison]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	Les SQLINEs de channels ne sont pas supportés par votre serveur, vous ne pouvez donc pas les utiliser.
OPER_SQLINE_EXISTS
	%s existe déjà sur la liste de SQLINEs.
OPER_SQLINE_ALREADY_COVERED
	%s est déjà couvert par %s.
OPER_SQLINE_REACHED_LIMIT
	Désolé, vous ne pouvez avoir que %d SQLINEs.
OPER_SQLINE_ADDED
	%s ajouté à la liste de SQLINEs.
OPER_SQLINE_CHANGED
	L'échéance de %s a été changée.
OPER_SQLINE_NOT_FOUND
	%s introuvable sur la liste de SQLINEs.
OPER_SQLINE_NO_MATCH
	Aucune entrée correspondante sur la liste de SQLINEs.
OPER_SQLINE_DELETED
	%s supprimé de la liste de SQLINEs.
OPER_SQLINE_DELETED_ONE
	1 entrée supprimée de la liste de SQLINEs.
OPER_SQLINE_DELETED_SEVERAL
	%d entrées supprimées de la liste de SQLINEs.
OPER_SQLINE_LIST_EMPTY
	La liste de SQLINEs est vide.
OPER_SQLINE_LIST_HEADER
	Liste de SQLINEs actuelle:
	  Num   Masque                            Raison
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Liste de SQLINEs actuelle:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (par %s le %s; %s)
	      %s
OPER_SQLINE_CLEAR
	La liste de SQLINEs a été vidée.
	
# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+échéance] {masque | liste d'entrées} [raison]]
OPER_SZLINE_UNSUPPORTED
	Désolé, SZLINE n'est pas disponible sur ce réseau.
OPER_SZLINE_EXISTS
	%s existe déjà sur la liste de SZLINEs.
OPER_SZLINE_ALREADY_COVERED
	%s est déjà couvert par %s.
OPER_SZLINE_REACHED_LIMIT
	Désolé, vous ne pouvez avoir que %d SZLINEs.
OPER_SZLINE_ONLY_IPS
	Rappel: vous ne pouvez ajouter que des masques d'IP à la liste de SZLINEs.
OPER_SZLINE_ADDED
	%s ajouté à la liste de SZLINEs.
OPER_SZLINE_CHANGED
	L'échéance de %s a été changée.
OPER_SZLINE_NOT_FOUND
	%s introuvable sur la liste de SZLINEs.
OPER_SZLINE_NO_MATCH
	Aucune entrée correspondante sur la liste de SZLINEs.
OPER_SZLINE_DELETED
	%s supprimé de la liste de SZLINEs.
OPER_SZLINE_DELETED_ONE
	1 entrée supprimée de la liste de SZLINEs.
OPER_SZLINE_DELETED_SEVERAL
	%d entrées supprimées de la liste de SZLINEs.
OPER_SZLINE_LIST_EMPTY
	La liste de SZLINEs est vide.
OPER_SZLINE_LIST_HEADER
	Liste de SZLINEs actuelle:
	  Num   Masque                            Raison
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Liste de SZLINEs actuelle:
# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (par %s le %s; %s)
	      %s
OPER_SZLINE_CLEAR
	La liste de SZLINEs a été vidée.

# SET responses
OPER_SET_SYNTAX
	SET option valeur
OPER_SET_IGNORE_ON
	Le code d'ignore sera utilisé.
OPER_SET_IGNORE_OFF
	Le code d'ignore ne sera pas utilisé.
OPER_SET_IGNORE_ERROR
	La valeur pour IGNORE doit être ON ou OFF.
OPER_SET_READONLY_ON
	Les Services sont maintenant en mode lecture-seule.
OPER_SET_READONLY_OFF
	Les Services sont maintenant en mode lecture-écriture.
OPER_SET_READONLY_ERROR
	La valeur pour READONLY doit être ON ou OFF.
OPER_SET_LOGCHAN_ON
	Les services reporteront maintenant les logs sur %s.
OPER_SET_LOGCHAN_OFF
	Les services ne reporteront plus les logs sur un canal.
OPER_SET_LOGCHAN_ERROR
	Les valeurs pour LOGCHAN doivent etre ON ou OFF et LogChannel dois etre défini.
OPER_SET_DEBUG_ON
	Les Services sont maintenant en debug mode.
OPER_SET_DEBUG_OFF
	Les Services sont maintenant en non-debug mode.
OPER_SET_DEBUG_LEVEL
	Les Services sont maintenant en debug mode (level %d).
OPER_SET_DEBUG_ERROR
	La valeur pour DEBUG doit être ON, OFF, ou un nombre positif.
OPER_SET_NOEXPIRE_ON
	Les Services sont maintenant en mode sans expiration.
OPER_SET_NOEXPIRE_OFF
	Les Services sont maintenant en mode expiration.
OPER_SET_NOEXPIRE_ERROR
	La valeur de NOEXPIRE doit être ON ou OFF.
OPER_SET_UNKNOWN_OPTION
	Option inconnue %s.
OPER_SET_SQL_ON
	Le code SQL dois etre utilisé.
OPER_SET_SQL_OFF
	Le code SQL ne dois pas etre utilisé.
OPER_SET_SQL_ERROR
	Les configurations pour SQL doivent etre ON ou OFF.
OPER_SET_SQL_ERROR_INIT
	Erreur durant l'initialisation de SQL, verifier les logs pour corriger l'erreur.
OPER_SET_SQL_ERROR_DISABLED
	SQL n'est pas configuré pour ce réseau. Editer le fichier
	de configuration (services.conf) pour l'activer.
OPER_SET_LIST_OPTION_ON
	%s est activé
OPER_SET_LIST_OPTION_OFF
	%s est désactivé

# NOOP responses
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} serveur
OPER_NOOP_SET
	Toutes les O:lines de %s ont été supprimées.
OPER_NOOP_REVOKE
	Toutes les O:lines de %s ont été réinitialisées.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE nom-du-serveur [raison]

# RAW responses
OPER_JUPE_HOST_ERROR
	Veuillez indiquer un nom de serveur valide pour JUPE.
	
# RAW responses
OPER_RAW_SYNTAX
	RAW texte

# UPDATE responses
OPER_UPDATING
	Mise à jour des bases de données.
	
# RELOAD responses
OPER_RELOAD
	Le fichier de configuration des Services a été rechargé.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN non défini, impossible de redémarrer redémarrer.  
	Relancez le script \2configure\2 et recompilez les Services pour 
	activer la commande RESTART.

# LISTIGNORE responses
OPER_IGNORE_SYNTAX
	Syntaxe: IGNORE {ADD|DEL|LIST|CLEAR} [durée] [nick | mask]
OPER_IGNORE_VALID_TIME
	Vous devez entrer une durée valide
OPER_IGNORE_TIME_DONE
	%s est ignoré pour %s.
OPER_IGNORE_PERM_DONE
	%s est ignoré en permanance.
OPER_IGNORE_DEL_DONE
	%s n'est plus ignoré
OPER_IGNORE_LIST
	Liste des ignores:
OPER_IGNORE_LIST_NOMATCH
	%s n'a pas été trouvé dans la liste des ignores.
OPER_IGNORE_LIST_EMPTY
	La liste des ignores est vide.
	
# KILLCLONES responses
OPER_IGNORE_LIST_CLEARED
	La liste des ignores a été purgé.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES pseudo
OPER_KILLCLONES_UNKNOWN_NICK
	Impossible de trouver l'utilisateur %s.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Liste des channels:
	Nom                  Users Modes   Topic
OPER_CHANLIST_HEADER_USER
	Liste des channels de %s:
	Nom                  Users Modes   Topic
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	Fin de la liste des channels.
	
# USERLIST responses
OPER_USERLIST_HEADER
	Liste des utilisateurs:
	Nick                 Masque
OPER_USERLIST_HEADER_CHAN
	Liste des utilisateurs de %s:
	Nick                 Masque
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Fin de la liste des utilisateurs.

# CACHE responses
OPER_SUPER_ADMIN_ON
	Vous êtes maintenant SuperAdmin.
OPER_SUPER_ADMIN_OFF
	Vous n'êtes plus SuperAdmin
OPER_SUPER_ADMIN_SYNTAX
	Syntaxe incorrect, spécifiez ON ou OFF.
OPER_SUPER_ADMIN_WALL_ON
	%s est maintenant SuperAdmin
OPER_SUPER_ADMIN_WALL_OFF
	%s n'est plus SuperAdmin

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_SUPER_ADMIN_ONLY
	Seulement les SuperAdmins peuvent utiliser cette commande.

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_STAFF_LIST_HEADER
	On Level Pseudo

# DefCon Messages

OPER_STAFF_FORMAT
	 %c %s  %s
OPER_STAFF_AFORMAT
	 %c %s  %s [%s]

# DefCon Messages

OPER_DEFCON_SYNTAX
	Syntaxe: DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	Ce service est temporairement désactivé, veuillez réessayer ultérieurement.
OPER_DEFCON_NO_CONF
	Defcon dois etre activé dans le fichier services.conf.
OPER_DEFCON_CHANGED
	Les services sont maintenant sur DEFCON %d.
OPER_DEFCON_WALL
	%s a changé le niveau de DEFCON en %d.
DEFCON_GLOBAL
	Le niveau de DEFCON est maintenant: %d.

# Modules strings

OPER_MODULE_LOADED
	Le module %s a été chargé.
OPER_MODULE_UNLOADED
	Le module %s a été déchargé.
OPER_MODULE_LOAD_FAIL
	Impossible de charger le module %s.
OPER_MODULE_REMOVE_FAIL
	Impossible de déchager le module %s.
OPER_MODULE_NO_UNLOAD
	Ce module ne peux pas etre déchargé.
OPER_MODULE_LOAD_SYNTAX
	MODLOAD Fichier
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD Fichier
OPER_MODULE_LIST_HEADER
	Liste des modules courant:
OPER_MODULE_LIST
	Module: %s [%s] [%s]
OPER_MODULE_LIST_FOOTER
	%d Modules changés.
OPER_MODULE_INFO_LIST
	Module: %s Version: %s Auteur: %s chargé: %s
OPER_MODULE_CMD_LIST
	Fournir la commande: %R%s %s
OPER_MODULE_MSG_LIST
	Providing IRCD handler for: %s
OPER_MODULE_NO_LIST
	Aucun module n'est chargé
OPER_MODULE_NO_INFO
	Aucune information sur le module %s n'est accéssible.
OPER_MODULE_INFO_SYNTAX
	MODINFO Fichier

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
MODULE_HELP_HEADER
	Cette commande est chargé via un module:

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [paramètres]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+échéance] mask limite raison
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {mask | liste}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE num position
OPER_EXCEPTION_DISABLED
	La limitation de sessions est désactivée.
OPER_EXCEPTION_ALREADY_PRESENT
	Le mask %s est déjà présent sur la liste d'exceptions.
OPER_EXCEPTION_TOO_MANY
	La liste des exceptions à la limitation de sessions est pleine!
OPER_EXCEPTION_ADDED
	La limite de sessions de %s est maintenant de %d.
OPER_EXCEPTION_MOVED
	L'exception pour %s (#%d) a été déplacée à la position %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	L'entrée %d n'existe pas sur la liste des exceptions à la limitation de sessions.
OPER_EXCEPTION_NOT_FOUND
	%s non trouvé sur la liste des exceptions à la limitation de sessions.
OPER_EXCEPTION_NO_MATCH
	Aucune entrée correspondante sur la liste des exceptions à la limitation de sessions.
OPER_EXCEPTION_DELETED
	%s supprimé de la liste des exceptions à la limitation de sessions.
OPER_EXCEPTION_DELETED_ONE
	1 entrée supprimée de la liste des exceptions à la limitation de sessions.
OPER_EXCEPTION_DELETED_SEVERAL
	%d entrées supprimées de la liste des exceptions à la limitation de sessions.
OPER_EXCEPTION_LIST_HEADER
	Liste actuelle des exceptions à la limitation de sessions:
# mask, limit
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d    %s
OPER_EXCEPTION_LIST_COLHEAD
	Num  Limite  Host
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (par %s le %s; %s)
	    Limite: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Limite de session invalide. Elle doit être un entier valide supérieur ou égal à zero et plus petit que %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Hostmask invalide. Seuls les hostmasks réels sont valides puisque les exceptions ne sont pas comparés aux pseudos et usernames.
OPER_EXCEPTION_EXISTS
	%s existe deja dans la liste des exceptions.
OPER_EXCEPTION_CHANGED
	L'exception pour %s a été changé en %d.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST limite | VIEW host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST limite
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW host
OPER_SESSION_DISABLED
	La limitation de session est désactivée.
OPER_SESSION_INVALID_THRESHOLD
	Valeur minimale invalide. Elle doit être un entier valide supérieur à 1.
OPER_SESSION_NOT_FOUND
	%s introuvable sur la liste des sessions.
OPER_SESSION_LIST_HEADER
	Hosts ayant au moins %d sessions:
OPER_SESSION_LIST_COLHEAD
	Sessions  Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	L'host %s a actuellement %d sessions avec une limite de %d.

# EXCEPTION help

OPER_HELP_EXCEPTION
	Syntax: EXCEPTION ADD [+échéance] mask limite raison
	        EXCEPTION DEL {mask | liste}
	        EXCEPTION MOVE num position
	        EXCEPTION LIST [mask | liste]
	        EXCEPTION VIEW [mask | liste]
	
	Permet aux Services admins de manipuler la liste des hosts qui
	ont des limites de session spécifiques - autoriser certaines
	machines, telles que des servers de shell, pour supporter plus
	que le nombre de clients par défaut à la fois. Une fois qu'une
	host atteint sa limite de session, tous les clients essayant
	de se connecter de cette host seront déconnectés. Avant que
	l'utilisateur ne soit déconnecté, ils sont avertis, par un
	/NOTICE de %S, de l'endroit où ils pourront trouver de
	l'aide concernant la limitation de session. Le contenu de cette
	notice est une option de configuration.
	
	EXCEPTION ADD ajoute le mask donné à la liste des exceptions.
	Notez que les masks nick!user@host et user@host sont 
	invalides! Seuls les masks d'host réels, tels que box.host.dom 
	et *.host.dom, sont autorisés parce que la limitation de sessions
	ne prend pas en compte le nick ou l'username. limit doit être un
	nombre supérieur ou égal à zero. Ceci détermine combien de sessions
	sont supportées à la fois. Une valeur de zero signifie que l'host
	a une limite de sessions illimitée. Consultez l'aide de AKILL pour
	les détails sur le format du paramètre optionnel échéance.
	EXCEPTION DEL supprime le mask donné de la liste des exceptions.
	EXCEPTION MOVE déplace l'exception num vers position. Les 
	exceptions se trouvant entre seront déplacées vers le haut ou le
	bas pour combler le trou.
	EXCEPTION LIST et EXCEPTION VIEW montrent toutes les 
	exceptions actuelles; si un mask optionnel est donné, la liste
	est limitée aux exceptions correspondantes au mask. La différence
	est que EXCEPTION VIEW est plus informative, puisqu'elle affiche
	également le nick de la personne qui a ajouté l'exception, sa 
	limite de sessions, la raison, le mask ainsi que la date d'échéance.
	
	Notez qu'un client se connectant "utilisera" la première 
	exception à laquelle leur host correspond. Les
	grandes listes d'exception et les exceptions utilisant
	excessivement les wildcards entrainent une dégradation des
	performances des Services.
	
	Réservée aux Services admins.

OPER_HELP_SESSION
	Syntax: SESSION LIST minimal
	        SESSION VIEW host
	
	Permet aux Services admins de voir la liste des sessions.
	
	SESSION LIST liste les hosts ayant au minimum minimal
	sessions. La limite minimale doit être un nombre supérieur à
	1. Ceci pour prévenir d'un listing accidentel d'un large
	nombre d'hosts à session unique.

	SESSION VIEW affiche des informations détaillées sur une
	host spécifique - comprenant le nombre de sessions actuelles
	ainsi que la limite de sessions. La valeur host ne doit
	pas contenir de wildcards.
	    
	Consultez l'aide de EXCEPTION pour plus d'informations 
	concernant la limitation et comment définir les limites de
	sessions pour certaines hosts et groupes.
	
	Réservée aux Services admins.

###########################################################################
#
# News system messages
#
###########################################################################

OPER_HELP_STAFF
	Syntaxe: STAFF

	Affiche tout les pseudos du staff des services avec leur
	niveau d'accès et leur status (online/offline).

OPER_HELP_DEFCON
	Syntax: DEFCON [1|2|3|4|5]

	Le système defcon utilise et applique des configurations
	pré-definies pour proteger durant une attaque sur le réseau.
	attack on the network.

OPER_HELP_DEFCON_NO_NEW_CHANNELS
	* Enregistrement de nouveaux canaux bloqué
OPER_HELP_DEFCON_NO_NEW_NICKS
	* Enregistrement de nouveaux pseudo bloqué
OPER_HELP_DEFCON_NO_MLOCK_CHANGE
	* Changement des MLOCK bloqué
OPER_HELP_DEFCON_FORCE_CHAN_MODES
	* Application des modes %s sur tout les canaux.
OPER_HELP_DEFCON_REDUCE_SESSION
	* Utilisation du nombre maxi de session a %d
OPER_HELP_DEFCON_NO_NEW_CLIENTS
	* Kill toutes nouvelles connexions client
OPER_HELP_DEFCON_OPER_ONLY
	* Ignore tout les non opers pour les messages
OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* Ignore silencieusement les non-opers
OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* AKILL toutes les nouvelles connexions


###########################################################################
#
# News system messages
#
###########################################################################

OPER_HELP_DEFCON_NO_NEW_MEMOS
	* Aucun(s) nouveau(x) memo(s) envoyé(s)
OPER_HELP_CHANKILL
	Syntaxe: CHANKILL [+expire] canal raison

	Inflige un AKILL a toutes les personnes qui sont sur le canal
	spécifié. Ceci utilise le réel identd@host de tout les pseudos. 

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
	[News Aléatoire - %s] %s

NEWS_LOGON_SYNTAX
	Syntaxe: LOGONNEWS {ADD|DEL|LIST} [texte|nombre]
NEWS_LOGON_LIST_HEADER
	Articles des logon news:
NEWS_LOGON_LIST_ENTRY
	%5d (%s par %s)
	    %s
NEWS_LOGON_LIST_NONE
	Il n'y a pas de logon news.
NEWS_LOGON_ADD_SYNTAX
	Syntaxe: LOGONNEWS ADD texte
NEWS_LOGON_ADD_FULL
	La liste des News est pleine!
NEWS_LOGON_ADDED
	Ajouté un nouvel article de logon news (#%d).
NEWS_LOGON_DEL_SYNTAX
	Syntaxe: LOGONNEWS DEL {num | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Article de logon news #%d non trouvé!
NEWS_LOGON_DELETED
	Article de logon news #%d supprimé.
NEWS_LOGON_DEL_NONE
	Aucun article de logon news à supprimer!
NEWS_LOGON_DELETED_ALL
	Tous les articles de logon news supprimés.

NEWS_OPER_SYNTAX
	Syntaxe: OPERNEWS {ADD|DEL|LIST} [texte|nombre]
NEWS_OPER_LIST_HEADER
	Articles oper news:
NEWS_OPER_LIST_ENTRY
	%5d (%s par %s)
	    %s
NEWS_OPER_LIST_NONE
	Il n'y a pas d'oper news.
NEWS_OPER_ADD_SYNTAX
	Syntaxe: OPERNEWS ADD texte
NEWS_OPER_ADD_FULL
	La liste d'oper news est pleine.
NEWS_OPER_ADDED
	Ajouté un nouvel article d'oper news (#%d).
NEWS_OPER_DEL_SYNTAX
	Syntaxe: OPERNEWS DEL {num | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Article oper news #%d non trouvé!
NEWS_OPER_DELETED
	Article oper news #%d supprimé.
NEWS_OPER_DEL_NONE
	Pas d'articles oper news à supprimer!
NEWS_OPER_DELETED_ALL
	Tous les articles oper news supprimés.

NEWS_RANDOM_SYNTAX
	Syntaxe: RANDOMNEWS {ADD|DEL|LIST} [texte|nombre]
NEWS_RANDOM_LIST_HEADER
	Nouvelles aléatoires:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s par %s)
	    %s
NEWS_RANDOM_LIST_NONE
	Il n'y a pas de nouvelles aléatoires.
NEWS_RANDOM_ADD_SYNTAX
	Syntaxe: RANDOMNEWS ADD texte
NEWS_RANDOM_ADD_FULL
	La liste de nouvelles est pleine!
NEWS_RANDOM_ADDED
	Nouvelle aléatoire ajoutée (n°%d).
NEWS_RANDOM_DEL_SYNTAX
	Syntaxe: RANDOMNEWS DEL {num | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Nouvelle aléatoire n°%d non trouvée!
NEWS_RANDOM_DELETED
	Nouvelle aléatoire n°%d supprimée.
NEWS_RANDOM_DEL_NONE
	Aucune nouvelle aléatoire à supprimer!
NEWS_RANDOM_DELETED_ALL
	Toutes les nouvelles aléatoires ont été supprimés.

NEWS_HELP_LOGON
	Syntaxe: LOGONNEWS ADD texte
	         LOGONNEWS DEL {nombre | ALL}
	         LOGONNEWS LIST
	
	Edite ou affiche la liste de messages logon news. Lorsqu'un
	utilisateur se connecte au réseau, ces messages leur seront
	envoyés. (Cependant, pas plus de %s messages ne seront
	envoyés pour éviter le flood des utilisateurs. Si il y a
	plus messages de news, seuls les plus récents seront envoyés.)
	NewsCount peut être configuré en services.conf.
	
	LOGONNEWS LIST peut être utilisée par tout IRC operator pour
	lister les messages de news courants. ADD et DEL peuvent
	uniquement être utilisés par les Services admins.

NEWS_HELP_OPER
	Syntaxe: OPERNEWS ADD texte
	         OPERNEWS DEL {nombre | ALL}
	         OPERNEWS LIST
	
	Edite ou affiche la liste des messages oper news. Quand un
	utilisateur utilise la commande /OPER, ces messages lui seront
	envoyés. (Cependant, pas plus de %s messages ne seront 
	envoyés afin de ne pas flooder l'utilisateur. Si il y a plus 
	messages news, seuls les plus récents seront envoyés.)
	NewsCount peut être configuré en services.conf.
	
	OPERNEWS LIST peut être utilisée par tout IRC operator pour
	lister les messages oper news actuels. ADD et DEL peuvent 
	uniquement être utilisés par les Services admins.

NEWS_HELP_RANDOM
	Syntaxe: RANDOMNEWS ADD texte
	         RANDOMNEWS DEL {nombre | ALL}
	         RANDOMNEWS LIST
	
	Edite ou affiche la liste des nouvelles aléatoires. Lorsqu'un
	utilisateur se connecte au réseau, une (et une seule) de ces
	nouvelles sera choisie et lui sera envoyée.
	
	RANDOMNEWS LIST peut être utilisée par tout opérateur IRC pour
	lister les nouvelles aléatoires actuellement définies. ADD et 
	DEL ne peuvent être utilisés que par les administrateurs des
	services.

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S est un service conçu pour donner des informations
	sur les Services. Les rubriques d'aide des Services sont
	accessibles par les commandes HELP des différents Services:

	%R%s HELP
	     pour des informations sur l'enregistrement des nicks

	%R%s HELP
	     pour des informations sur l'enregistrement et le
	     contrôle des channels

	%R%s HELP
	     pour des informations sur l'envoi de messages aux
	     utilisateurs non connectés

HELP_HELP_BOT

	%R%s HELP
	     pour des informations sur l'installation d'un bot
	     sur votre channel

HELP_HELP_HOST

	%R%s HELP
	     pour des informations sur la configuration des vhosts.

###########################################################################
#
# NickServ help messages
#
###########################################################################
NICK_HELP_CMD_CONFIRM
	    CONFIRM    Confirme une inscription à NickServ
NICK_HELP_CMD_RESEND
	    RESEND     Renvoie le code de confirmation de 
	               l'inscription à NickServ
NICK_HELP_CMD_REGISTER
	    REGISTER   Enregistre un pseudo
NICK_HELP_CMD_GROUP
	    GROUP      Joint un groupe
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY   Vous identifie avec votre mot de passe
NICK_HELP_CMD_ACCESS
	    ACCESS     Modifie la liste des adresses autorisées
NICK_HELP_CMD_SET
	    SET        Configure les options, protection comprise
NICK_HELP_CMD_SASET
	    SASET      Configure des options sur d'autres pseudos
NICK_HELP_CMD_DROP
	    DROP       Annule l'enregistrement d'un pseudo
NICK_HELP_CMD_RECOVER
	    RECOVER    Change le pseudo d'un utilisateur qui a pris 
	               votre pseudo
NICK_HELP_CMD_RELEASE
	    RELEASE    Reprendre la tutelle de votre pseudo
	               après un RECOVER
NICK_HELP_CMD_SENDPASS
	    SENDPASS   A utiliser en cas de perte de mot de passe
NICK_HELP_CMD_GHOST
	    GHOST      Deconnect un "ghost irc" utilisant votre pseudo
NICK_HELP_CMD_ALIST
	    ALIST      Liste tout les canaux auquels vous avez un accès
NICK_HELP_CMD_GLIST
	    GLIST      Liste tout les pseudos de votre groupe
NICK_HELP_CMD_INFO
	    INFO       Affiche des informations sur un pseudo
NICK_HELP_CMD_LIST
	    LIST       Liste tout les pseudos en rapport avec la recherche
NICK_HELP_CMD_LOGOUT
	    LOGOUT     Inverse la commande LOGIN
NICK_HELP_CMD_STATUS
	    STATUS     Renvoie le statut de propriétaire du pseudo donné
NICK_HELP_CMD_UPDATE
	    UPDATE     Met à jour votre status, ex : verification de nouveau memo
NICK_HELP_CMD_GETPASS
	    GETPASS    Retrouve le mot de passe pour un pseudo
	               (uniquement si l'encryption est désactivée)
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Cherche et retourne la liste des pseudos enregistrés
	    avec un certain email
NICK_HELP_CMD_FORBID
	    FORBID     Empêche l'utilisation d'un pseudo
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Suspend un pseudo
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Unsuspend un pseudo

NICK_HELP
	%S vous permet d'enregistrer un pseudo et d'empêcher les
	autres de l'utiliser. Les commandes suivantes
	permettent l'enregistrement et la gestion des pseudo. Pour
	les utiliser, tapez %R%S commande. Pour plus
	d'informations sur une certaine commande, tapez
	%R%S HELP commande.
	
NICK_HELP_FOOTER
	REMARQUE: Ce service est conçu pour fournir un moyen aux
	utilisateurs IRC d'assurer que leur identité n'est pas
	usurpée. Il n'est PAS destiné à faciliter le "vol" des
	pseudos ou autres choses de ce genre. L'abus de %S entraînera,
	au minimum, la perte du(des) pseudo(s) faisant l'objet de 
	l'abus.
	
NICK_HELP_EXPIRES
	
	Les nicks qui ne sont plus utilisés sont sujets à 
	l'expiration automatique, c'est-à-dire qu'ils seront effacés
	après %d jours de non utilisation.


NICK_HELP_REGISTER
	Syntaxe: REGISTER motdepasse [email]
	
	Enregistre votre pseudo dans la base de données de %S. Une fois
	que votre pseudo est enregistré, vous pouvez utiliser les
	commandes SET et ACCESS pour configurer votre pseudo comme
	vous le souhaitez. Rappelez vous bien du mot de passe utilisé
	pendant l'enregistrement - vous en aurez besoin pour effectuer
	des changements sur votre pseudo plus tard. (Notez que ANOPE, 
	Anope, et anope sont tous des passwords différents!)
	
	Conseils pour choisir un mot de passe:
	
	Les mots de passe ne doivent pas être trop facilement
	devinables. Par exemple, utiliser votre vrai nom comme
	mot de passe est une mauvaise idée. Utiliser votre pseudo
	comme mot de passe est une idée encore pire ;) et, en
	fait, %S ne le permettra pas. De même, les mots de passe
	courts sont vulnérables aux recherches méthodiques,
	donc vous devez choisir un mot de passe d'au moins 
	5 caractères. Le caractère espace ne peut être 
	utilisé dans les mots de passe.
	
	Le paramètre email est optionnel et permet de définir
	un e-mail immédiatement. Toutefois, il peut être obligatoire
	sur certains réseaux.
	Votre vie privée est respectée, cet e-mail ne sera pas 
	divulgué à des tierces personnes.
	
	Cette commande crée également un nouveau groupe pour votre
	pseudo. Celui-ci vous permettra plus tard d'enregistrer
	d'autres nicks en bénéficiant de la même configuration,
	du même jeu de memos, et des même privilèges sur vos
	channels. Pour plus d'information sur cette fonction, 
	tapez %R%S HELP GROUP.

NICK_HELP_GROUP
	Syntaxe: GROUP cible motdepasse
	
	Cette commande joint votre pseudo au groupe du nick cible.
	motdepasse est le mot de passe du pseudo cible.
	
	Joindre un groupe vous permettra de partager votre configuration,
	vos memos et vos privilèges de channel avec tous les nicks
	du groupe, et bien plus encore!
	 
	Un groupe existe tant qu'il est utile. Cela signifie que même
	si un des pseudos du groupe est effacé, les éléments partagés
	sus-nommés sont conservés pour peu qu'il reste d'autres pseudos
	dans le groupe.
	
	Il est préférable d'utiliser cette commande avec un pseudo
	non enregistré puisqu'il sera automatiquement enregistré
	lorsque vous utiliserez cette commande. Vous pouvez l'utiliser
	avec un pseudo enregistré (pour changer votre groupe) uniquement
	si vos administrateurs l'autorisent. (%R%S HELP IDENTIFY 
	pour plus d'informations).
	
	Vous ne pouvez être que dans un groupe à la fois. La fusion
	de groupes n'est pas possible.
	
	Note: tous les pseudos du groupe ont le même mot de passe.

NICK_HELP_IDENTIFY
	Syntaxe: IDENTIFY motdepasse
	
	Informe %S que vous êtes vraiment le propriétaire de
	ce pseudo. Beaucoup de commandes demandent que vous soyez
	identifié avec cette commande avant de les utiliser. Le
	mot de passe est celui que vous avez donné avec la commande 
	REGISTER.

NICK_HELP_UPDATE
	Syntax: UPDATE

	Actualise votre session, ex : vérification de l'arriver de
	nouveau(x) memo(s), l'application des modes sur les canaux
	(ModeonID), met	à jour votre vhost et vos flags
	(dernière fois vu, etc).
	
NICK_HELP_LOGOUT
	Syntaxe: LOGOUT
	
	Cette commande inverse l'effet de la commande IDENTIFY,
	c'est-à-dire qu'elle fait en sorte que vous ne soyez plus
	reconnu en tant que propriétaire réel du pseudo. Notez
	cependant que l'on ne vous demandera pas de vous réidentifier.

NICK_HELP_DROP
	Syntaxe: DROP [nick]
	
	Efface votre pseudo de la base de données de %S. Un pseudo
	qui a été effacé est libre d'être réenregistré par
	n'importe qui.
	
	Vous pouvez effacer un pseudo de votre groupe en le mettant
	dans le paramètre nick.
	
	Pour utiliser cette commande, vous devez d'abord vous
	identifier avec votre mot de passe (%R%S HELP 
	IDENTIFY pour plus d'informations).

NICK_HELP_ACCESS
	Syntaxe: ACCESS ADD mask
	         ACCESS DEL mask
	         ACCESS LIST
	
	Modifie ou affiche la liste d'accès de votre pseudo. C'est
	la liste des adresses qui seront automatiquement reconnue
	par %S comme autorisée à utiliser le pseudo. Si vous voulez
	utiliser le pseudo depuis une adresse différente, vous devez
	envoyer une commande IDENTIFY pour que %S vous reconnaisse.
	
	Exemples:
	
	    ACCESS ADD anyone@*.bepeg.com
	        Autorise l'accès à l'utilisateur anyone d'une
	        machine avec le domaine bepeg.com.
	
	    ACCESS DEL anyone@*.bepeg.com
	        Inverse la commande précédente.
	
	    ACCESS LIST
	        Affiche votre liste d'accès.

NICK_HELP_SET
	Syntaxe: SET option paramètres
	
	Configure diverses options du pseudo. option peut être:
	
	    DISPLAY    Change l'affichage de votre groupe
	                   dans les services
	    PASSWORD   Change le mot de passe de votre pseudo
	    LANGUAGE   Change la langue dans laquelle les Services
	                   vous envoient leurs messages
	    URL        Associe une adresse de site à votre pseudo
	    EMAIL      Associe un e-mail à votre pseudo
	    ICQ        Associe un numéro ICQ à votre pseudo
	    GREET      Associe un message d'accueil à votre pseudo
	    KILL       Active ou désactive la protection
	    SECURE     Active ou désactive la sécurité du pseudo
	    PRIVATE    Empêche votre pseudo d'être affiché par
	                   %R%S LIST
	    HIDE       Cache certaines parties des informations du pseudo
	    MSG        Change le mode de communication des Services
	    AUTOOP     Should services op you automatically.    
	
	Pour utiliser cette commande, vous devez d'abord vous
	identifier avec votre mot de passe (%R%S HELP 
	IDENTIFY pour plus d'informations).
	
	Tapez %R%S HELP SET option pour plus d'information
	sur une option spécifique.

NICK_HELP_SET_DISPLAY
	Syntaxe: SET DISPLAY nouvel-affichage
	
	Change l'affichage utilisé pour nommer votre groupe de
	pseudos dans les services. Le nouvel affichage DOIT être un
	pseudo de votre groupe.

NICK_HELP_SET_PASSWORD
	Syntaxe: SET PASSWORD nouveau-mot-de-passe
	
	Change le mot de passe utilisé pour vous identifier en tant
	que propriétaire du pseudo.

NICK_HELP_SET_LANGUAGE
	Syntaxe: SET LANGUAGE nombre
	
	Change la langue utilisée par les services lorsqu'ils vous
	envoient un message (par exemple, quand ils répondent à une
	commande que vous avez envoyé). nombre doit être choisi
	dans la liste suivante des langues supportées:

NICK_HELP_SET_URL
	Syntaxe: SET URL adresse
	
	Associe l'adresse de votre site web à votre pseudo. Cette adresse 
	sera affichée lorsque quelqu'un demandera des informations sur
	votre pseudo avec la commande INFO.

NICK_HELP_SET_EMAIL
	Syntaxe: SET EMAIL adresse
	
	Associe l'e-mail donné avec votre pseudo. L'email
	sera affiché lorsque quelqu'un demandera des informations 
	sur votre pseudo avec la commande INFO.
	
NICK_HELP_SET_ICQ
	Syntaxe: SET ICQ numéro
	
	Associe le numéro ICQ donné à votre pseudo. Ce numéro 
	sera affiché lorsque quelqu'un demandera des informations sur
	votre pseudo avec la commande INFO.

NICK_HELP_SET_GREET
	Syntaxe: SET GREET message
	
	Définit un message d'accueil pour votre pseudo, qui sera
	affiché lorsque vous joignez un channel qui a l'option
	GREET active, à condition d'avoir un niveau d'accès
	suffisant.

NICK_HELP_SET_KILL
	Syntaxe: SET KILL {ON | QUICK | IMMED | OFF}
	
	Active ou désactive la protection automatique
	pour votre pseudo. Avec la protection activée, si
	un autre utilisateur essaie de prendre votre pseudo, il aura
	une minute pour changer de pseudo, après quoi son pseudo sera
	changé de force par %S.
	
	Si vous sélectionnez QUICK, l'utilisateur aura seulement 20
	secondes pour changer de pseudo au lieu des 60 habituelles. Si
	vous sélectionnez IMMED, l'utilisateur verra son pseudo changé 
	immédiatement sans être d'abord prévenu et sans avoir de chance
	pour changer son pseudo, n'utilisez cette option que si nécessaire.
	Les administrateurs de votre réseau peuvent avoir désactivé cette option.

NICK_HELP_SET_SECURE
	Syntaxe: SET SECURE {ON | OFF}
	
	Active ou désactive la sécurité de %S pour votre pseudo.
	Si la sécurité est active, vous devrez obligatoirement
	entrer votre mot de passe pour être reconnu en tant que 
	propriétaire du pseudo, que votre masque soit dans la liste d'accès
	ou non. Cependant, si vous êtes sur la liste d'accès, %S ignorera
	la configuration de la protection offerte par SET KILL.

NICK_HELP_SET_PRIVATE
	Syntaxe: SET PRIVATE {ON | OFF}
	
	Active ou désactive le mode privé de %S de votre pseudo.
	Si le mode privé est actif, votre pseudo n'apparaitra pas
	dans les listes de pseudos générées par la commande LIST
	de %S. (Cependant, quiconque connaissant votre pseudo peut
	tout de même avoir des informations en utilisant
	la commande INFO).

NICK_HELP_SET_HIDE
	Syntaxe: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Vous permet d'empêcher l'affichage de certaines parties
	des informations quand quelqu'un fait un %S INFO sur votre
	pseudo. Vous pouvez cacher votre adresse e-mail (EMAIL), masque
	user@host vu pour la dernière fois (USERMASK), et la dernière
	raison de /QUIT (QUIT). Le second paramètre indique si 
	l'option doit être affichée (OFF) ou cachée (ON).
	
NICK_HELP_SET_MSG
	Syntaxe: SET MSG {ON | OFF}
	
	Vous permet de choisir la façon dont les services communiquent
	avec vous. Si cette option est activée, ils utiliseront des
	messages, sinon ils utiliseront des notices.

NICK_HELP_SET_AUTOOP
	Syntax: SET AUTOOP {ON | OFF}
	
	Sets whether you will be opped automatically. Set to ON to 
	allow ChanServ to op you automatically when entering channels.

NICK_HELP_SASET
	Syntaxe: SASET pseudo option parametres.
	
	Configure plusieurs options sur un pseudo.  option dois etre une ci dessou:
	
	    DISPLAY    Affiche les pseudos du groupe du pseudo donné
	    PASSWORD   Change le mot de passe du pseudo donné
	    URL        Associe une url au pseudo donné
	    EMAIL      Associe un email au pseudo donné
	    ICQ        Associe un compte ICQ au pseudo donné
	    GREET      Associe un message de greet au pseudo donné
	    KILL       Active ou désactive l'option kill du pseudo donné
	    SECURE     Active ou désactive la securité du pseudo donné
	    PRIVATE    Empêche le pseudo d'apparaître dans un
	                   %R%S LIST
	    HIDE       Cache certaine informations personnel au pseudo
	    MSG        Change le mode de communication des services
	    NOEXPIRE   Empêche le pseudo d'expirer
	    LANGUAGE   Set the language Services will use when
	               sending messages to nickname
	
	Taper %R%S HELP SASET option pour plus d'information
	sur une option spécifique. L'option devra etre mise avec le pseudo
	en prefix. 
	
	Limité aux Services admins.

NICK_HELP_SASET_DISPLAY
	Syntaxe: SASET pseudo DISPLAY nouvel-affichage
	
	Change l'affichage utilisé pour nommer le groupe du
	pseudos dans les services. Le nouvel affichage DOIT être un
	pseudo de votre groupe.
	
	Limité aux Services admins.

NICK_HELP_SASET_PASSWORD
	Syntaxe: SASET pseudo PASSWORD nouveau-mot-de-passe
	
	Change le mot de passe utilisé pour s'identifier comme propiétaire
	du pseudo.
	
	Limité aux Services admins.

NICK_HELP_SASET_URL
	Syntaxe: SASET pseudo URL url
	
	Associe l'URL donné au pseudo. Cette URL sera affiché
	a toutes les personnes demandant des informations sur
	le pseudo via la commande INFO.
	
	Limité aux Services admins.

NICK_HELP_SASET_EMAIL
	Syntaxe: SASET pseudo EMAIL adresse-email
	
	Associe l'E-mail donné au pseudo.
	
	Limité aux Services admins.
	
NICK_HELP_SASET_ICQ
	Syntaxe: SASET pseudo ICQ numéro
	
	Associe le numéro ICQ donné au pseudo. Ce numéro ICQ sera affiché
	a toutes les personnes demandant des informations sur le pseudo
	via la commande INFO.
	
	Limité aux Services admins.
	
NICK_HELP_SASET_GREET
	Syntaxe: SASET pseudo GREET message
	
	Définit un message d'accueil au pseudo, qui sera
	affiché lorsqu'il joint un channel qui a l'option
	GREET active, à condition d'avoir un niveau d'accès
	suffisant.
	
	Limité aux Services admins.

NICK_HELP_SASET_KILL
	Syntaxe: SASET pseudo KILL {ON | QUICK | IMMED | OFF}
	
	Active ou désactive la protection automatique pour le pseudo donné.
	Avec la protection activée, si un autre utilisateur essaie de 
	prendre son pseudo, il aura une minute pour changer de pseudo,
	après quoi son pseudo sera changé de force par %S.
	
	Si vous sélectionnez QUICK, l'utilisateur aura seulement 20
	secondes pour changer de pseudo au lieu des 60 habituelles. Si
	vous sélectionnez IMMED, l'utilisateur verra son pseudo changé
	immédiatement sans être d'abord prévenu et sans avoir de chance
	pour changer son pseudo, n'utilisez cette option que si nécessaire.
	Les administrateurs de votre réseau peuvent avoir désactivé cette option.
	
	Limité aux Services admins.

NICK_HELP_SASET_SECURE
	Syntaxe: SASET pseudo SECURE {ON | OFF}
	
	Active ou désactive la sécurité de %S pour le pseudo donné.
	Si la sécurité est active, il devra obligatoirement
	entrer son mot de passe pour être reconnu en tant que
	propriétaire du pseudo, que son masque soit dans la liste d'accès
	ou non. Cependant, si vous êtes sur la liste d'accès, %S ignorera
	la configuration de la protection offerte par SET KILL.
	
	Limité aux Services admins.

NICK_HELP_SASET_PRIVATE
	Syntaxe: SASET pseudo PRIVATE {ON | OFF}
	
	Active ou désactive le mode privé de %S du pseudo donné.
	Si le mode privé est actif, son pseudo n'apparaitra pas
	dans les listes de pseudos générées par la commande LIST
	de %S. (Cependant, quiconque connaissant son pseudo peut
	tout de même avoir des informations en utilisant
	la commande INFO).
	
	Limité aux Services admins.

NICK_HELP_SASET_HIDE
	Syntaxe: SASET pseudo HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	Vous permet d'empêcher l'affichage de certaines parties
	des informations quand quelqu'un fait un %S INFO sur le pseudo
	donné. Vous pouvez cacher son adresse e-mail (EMAIL), masque
	user@host vu pour la dernière fois (USERMASK), et la dernière
	raison de /QUIT (QUIT). Le second paramètre indique si
	l'option doit être affichée (OFF) ou cachée (ON).
	
	Limité aux Services admins.
	
NICK_HELP_SASET_MSG
	Syntaxe: SASET pseudo MSG {ON | OFF}
	
	Vous permet de choisir la façon dont les services communiquent
	le pseudo donné. Si cette option est activée, ils utiliseront des
	messages, sinon ils utiliseront des notices.
	
	Limité aux Services admins.

NICK_HELP_SASET_NOEXPIRE
	Syntaxe: SASET pseudo NOEXPIRE {ON | OFF}
	
	Permet d'activer le mode NOEXPIRE au pseudo donné.
	Configurez ce paramètres sur ON pour que le 
	pseudo n'expire pas.
	
	Limité aux Services admins.

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
	Syntaxe: RECOVER pseudo [motdepasse]
	
	Vous permet de récupérer votre pseudo si quelqu'un d'autre
	l'a pris, c'est équivalent à ce que ce que %S fait 
	automatiquement si quelqu'un essaie d'utiliser un pseudo
	protégé par SET KILL.
	
	Quand vous utilisez cette commande, %S connectera un faux
	utilisateur avec le même pseudo que l'utilisateur dont
	vous essayez de récupérer le pseudo. Cela provoque la déconnexion
	par les serveurs IRC de l'autre utilisateur. Ce faux utilisateur
	restera connecté pendant %s pour s'assurer que l'autre
	utilisateur ne se reconnecte pas immédiatement. Après ces
	temps, vous pouvez reprendre votre pseudo. Alternativement,
	utilisez la commande RELEASE (%R%S HELP RELEASE) pour
	reprendre votre pseudo plus tôt.
	
	Pour utiliser la commande RECOVER pour un pseudo, l'adresse
	telle qu'elle est montrée dans un /WHOIS doit être sur la
	liste d'accès du pseudo, vous devez être identifié et dans
	le groupe du pseudo, ou vous devez fournir le mot de passe
	correct du pseudo.

NICK_HELP_RELEASE
	Syntaxe: RELEASE pseudo [motdepasse]
	
	Ordonne à %S de supprimer toute tutelle sur votre pseudo
	provoquée par la protection de SET KILL ou 
	l'utilisation de la commande RECOVER. Par défaut, les
	tutelles durent %s, cette commande les arrête plus tôt.
	
	Pour utiliser la commande RELEASE pour un pseudo, l'adresse
	telle qu'elle est montrée dans un /WHOIS doit être sur la
	liste d'accès du pseudo, vous devez être identifié et dans
	le groupe du pseudo, ou vous devez fournir le mot de passe
	correct du pseudo.

NICK_HELP_GHOST
	Syntaxe: GHOST pseudo [motdepasse]
	
	Arrête une session IRC "fantôme" utilisant votre pseudo. Une
	session "fantôme" est un utilisateur non connecté 
	actuellement, mais dont le serveur IRC croit qu'il l'est
	toujours pour une raison ou une autre. Habituellement,
	ceci arrive lorsque votre ordinateur plante ou que votre connexion
	à Internet s'arrête lorsque vous êtes sur IRC.
	
	Pour utiliser la commande GHOST pour un pseudo, l'adresse
	telle qu'elle est montrée dans un /WHOIS doit être sur la
	liste d'accès du pseudo, ou vous devez fournir le mot de passe
	correct pour le pseudo.

NICK_HELP_INFO
	Syntaxe: INFO pseudo [ALL]
	
	Affiche des informations sur le pseudo donné, telles que
	le propriétaire du pseudo, l'adresse avec laquelle il était
	connecté pour la dernière fois, et les options du pseudo. Si
	vous êtes identifié pour le pseudo pour lequel vous demandez
	des informations et que ALL est spécifié, toutes
	les informations seront affichées, qu'elles soient cachées
	ou non.

NICK_HELP_LIST
	Syntaxe: LIST modèle
	
	Liste tous les pseudos enregistrés qui correspondent
	au modèle donné, dans le format pseudo!user@host. Les
	pseudos avec l'option PRIVATE active ne seront pas
	affichés.
	
	Exemples:
	
	    LIST *!joeuser@foo.com
	        Liste tous les pseudos appartenant à joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Liste tous les pseudo enregistrés avec Bot dans leurs
	        noms (ne différencie pas minuscules et majuscules).
	
	    LIST *!*@*.bar.org
	        Liste tous les pseudo appartenant aux utilisateurs dans le
	        domaine bar.org.
	        
NICK_HELP_ALIST
	Syntaxe: ALIST [level]
	
	Liste tout les canaux auquels vous avez accès. Optionnelement, vous
	pouvez spécifier un niveau au format XOP ou ACCESS. Le résultat
	retournera uniquement les canaux auquels vous avez un accès supérieur
	ou égale au niveau donné.
	
	Exemples:
	    ALIST Founder
	        Liste tout les canaux auquels vous avez un accès
		FOUNDER.

	    ALIST AOP
	        Liste tout les canaux ou vous avez un accès AOP ou plus.

	    ALIST 10
	        Liste tout les canaux ou vous avez un accès 10 ou plus.
	
	Les canaux ayant l'option NOEXPIRE active seront retourné avec un point 
	d'exclamation devant.
	
NICK_HELP_GLIST
	Syntaxe: GLIST
	
	Liste tous les pseudos dans votre groupe.

NICK_HELP_STATUS
	Syntaxe: STATUS nick...
	
	Retourne si l'utilisateur utilisant le pseudo donné est reconnu
	en tant que propriétaire du pseudo. La réponse a ce format:
	
	    pseudo code-status
	
	où pseudo est le pseudo envoyé avec la commande, et
	code-status peut être l'un des suivants:
	
	    0 - cet utilisateur n'est pas online ou le pseudo n'est pas 
	        enregistré
	    1 - utilisateur non reconnu comme propriétaire
	    2 - utilisateur reconnu comme propriétaire uniquement via
	        la liste d'accès
	    3 - utilisateur reconnu comme propriétaire via l'identification
	
	Jusqu'à seize pseudos peuvent être envoyés avec chaque commande,
	le reste sera ignoré. Si aucun pseudo n'est donné, votre status
	sera retourné.

NICK_HELP_SENDPASS
	Syntaxe: SENDPASS pseudo
	
	Envoie le mot de passe du pseudo donné à l'adresse e-mail 
	qui a été définie pour ce pseudo. Cette commande est très
	utile pour régler les problèmes de mots de passe perdus.
	
	Peut être limitée aux IRC operators sur certains
	réseaux.
	
	Cette commande est indisponible quand l'encryption est
	activée.

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
	
	Les Services admins peuvent également effacer n'importe quel
	pseudo sans avoir besoin de s'identifier pour le nick, et 
	peuvent voir l'access list pour n'importe quel pseudo
	(%R%S ACCESS LIST nick).
	
NICK_SERVADMIN_HELP_LOGOUT
	Syntaxe: LOGOUT [pseudo [REVALIDATE]]
	
	Sans paramètre, inverse l'effet de la commande IDENTIFY,
	c'est-à-dire qu'elle fait en sorte que vous ne soyez plus
	reconnu en tant que propriétaire réel du pseudo. Notez
	cependant que l'on ne vous demandera pas de vous réidentifier.
	
	Avec un paramètre, fait la même chose avec le pseudo donné. Si
	vous spécifiez également REVALIDATE, les Services demanderont
	au pseudo donné de se ré-identifier. Cette utilisation est 
	réservée aux Services admins.

NICK_SERVADMIN_HELP_DROP
	Syntaxe: DROP [pseudo]
	
	Sans paramètre, efface votre pseudo de la base de données
	de %S.
	
	Avec un paramètre, efface le pseudo donné de la base de
	données. Vous pouvez effacer n'importe quel pseudo de votre
	groupe sans privilèges particuliers. La possibilité 
	d'effacer n'importe quel pseudo est cependant réservée aux 
	Services admins.

NICK_SERVADMIN_HELP_INFO
	
	Les services admins peuvent utiliser le paramètre ALL 
	avec n'importe quel pseudo.

NICK_SERVADMIN_HELP_LIST
	Syntaxe: LIST modèle [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	Liste tous les pseudos enregistrés qui correspondent au modèle
	donné, de format nick!user@host. Les pseudos ayant l'option
	PRIVATE ne seront affichés qu'aux Services admins. Les
	pseudos ayant l'option NOEXPIRE seront préfixés par un !
	devant leur pseudo pour les Services admins. 
	
	Si les option FORBIDDEN, SUSPENDED, NOEXPIRE ou UNCONFIRMED sont donné,
	les pseudos qui sont FORBID, SUSPENDED, NON CONFIRMER ou ont l'option
	NOEXPIRE seront affiché. Si plus options sont indiquées,
	tout les pseudos correspondant aux une des critères seront affichés.
	Réservé aux Services admins.  
	
	Exemples:
	
	    LIST *!joeuser@foo.com
	        Liste tous les pseudos enregistrés appartenant à 
	        joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Liste tous les pseudos enregistrés ayant Bot dans
	        leur nom (ne respecte pas les majuscules).
	
	    LIST * NOEXPIRE
	        Liste tous les pseudos qui n'expireront pas.

NICK_SERVADMIN_HELP_ALIST
	Syntax: ALIST [pseudo] [level]
	
	Sans aucun paramètres, retourne la liste des canaux ou vous avez un accès.
	pseudo liste les canaux auquel le pseudo donné a accès
	   level liste les canaux auquel le pseudo donné a un accès supérieur
	             ou égale au level donné

	Réservé aux Services admins.

NICK_SERVADMIN_HELP_GLIST
	Syntaxe: GLIST [nick]
	
	Sans paramètre, liste tous les pseudos qui sont dans
	votre groupe.
	
	Avec un paramètre, liste tous les pseudos qui sont dans
	le groupe du pseudo donné.
	Cette utilisation est limitée aux administrateurs des 
	Services.

NICK_SERVADMIN_HELP_GETPASS
	Syntaxe: GETPASS nick
	
	Retourne le mot de passe du pseudo donné. Notez que pour
	chaque utilisation de cette commande, un message comprenant
	la personne qui a émis la commande et le pseudo sur lequel
	il a été utilisé sera enregistré et envoyé en WALLOPS/GLOBOPS.
	
	Réservée aux Services admins.
	
	Cette commande est indisponible quand l'encryption est activée.

NICK_SERVADMIN_HELP_GETEMAIL
	Syntax: GETEMAIL user@emailhost

	Retourne la liste des pseudos enregistré avec l'email donné.
	Note vous ne pouvez pas utiliser de wildcards pour plusieurs
	         pseudos. 
		 Lorsque cette commande est utilisé, un message incluant
		 le pseudo de la personne qui a utilisé la commande et
		 vers quel email a été destiné la commande dans les logs.

	Limité aux Services admins.

NICK_SERVADMIN_HELP_FORBID
	Syntaxe: FORBID nick [raison]
	
	Empêche un pseudo d'être enregistré ou utilisé par 
	quelqu'un. Peut être annulé en effaçant le pseudo.
	
	Sur certains réseaux, la raison est requise.
	
	Réservée aux Services admins.

###########################################################################
#
# ChanServ help messages
#
###########################################################################

NICK_SERVADMIN_HELP_SUSPEND
	Syntaxe: SUSPEND pseudo raison / UNSUSPEND pseudo

	Suspend / libere le pseudo pseudo spécifié.
		
	Limité aux Services admins.

###########################################################################
#
# ChanServ help messages
#
###########################################################################

NICK_SERVADMIN_HELP_UNSUSPEND
	Syntaxe: UNSUSPEND pseudo

	Libere un pseudo suspendu.

	Limité aux Services admins.

###########################################################################
#
# ChanServ help messages
#
###########################################################################
CHAN_HELP_CMD_GETPASS
	    GETPASS    Retrouve le mot de passe du propriétaire pour
	               un channel 
CHAN_HELP_CMD_FORBID
	    FORBID     Empêche un channel d'être utilisé
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    Suspend un canal pour ne pas qu'il soit utilisé
	               tout en conservant les données et configurations.
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Libère un canal suspendu
CHAN_HELP_CMD_STATUS
	    STATUS     Retourne le level d'accès courant d'un
	               utilisateur sur un channel
CHAN_HELP_CMD_REGISTER
	    REGISTER   Enregistre un channel
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   Vous identifie avec votre mot de passe
CHAN_HELP_CMD_SET
	    SET        Configure les options et informations du 
	               channel
CHAN_HELP_CMD_AOP
	    AOP        Modifie la liste des utilisateurs AOPs
CHAN_HELP_CMD_SOP
	    SOP        Modifie la liste des utilisateurs SOPs
CHAN_HELP_CMD_ACCESS
	    ACCESS     Modifie la liste des utilisateurs privilégiés
CHAN_HELP_CMD_LEVELS
	    LEVELS     Redéfinit la signification des levels d'accès
CHAN_HELP_CMD_AKICK
	    AKICK      Gère la liste des kicks automatiques
CHAN_HELP_CMD_DROP
	    DROP       Annule l'enregistrement d'un channel
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Aide à retrouver les mots de passe égarés
CHAN_HELP_CMD_BAN
	    BAN        Ban un pseudo spécifié dans un canal
CHAN_HELP_CMD_CLEAR
	    CLEAR      Ordonne à ChanServ de nettoyé certain paramètres
	               sur un canal
CHAN_HELP_CMD_DEVOICE
	    DEVOICE    Devoice le pseudo spécifié sur un canal
CHAN_HELP_CMD_GETKEY
	    GETKEY     Retourne la clé du canal specifié
CHAN_HELP_CMD_INFO
	    INFO       Liste les informations sur un canal enregistré
CHAN_HELP_CMD_INVITE
	    INVITE     Ordone à ChanServ de vous inviter ou d'inviter un pseudo
	               sur un canal
CHAN_HELP_CMD_KICK
	    KICK       Kick le pseudo spécifié d'un canal
CHAN_HELP_CMD_LIST
	    LIST       Liste tout les canaux enregistré en rapport avec la recherche
CHAN_HELP_CMD_LOGOUT
	    LOGOUT     Cette commande inverse la commande IDENTIFY
CHAN_HELP_CMD_OP
	    OP         Donne l'Op au pseudo spécifié sur un canal
CHAN_HELP_CMD_TOPIC
	    TOPIC      Change le topic sur un canal spécifié
CHAN_HELP_CMD_UNBAN
	    UNBAN      Retire tout les bans vous empechant d'entrer sur un canal
CHAN_HELP_CMD_VOICE
	    VOICE      Donne un voice au pseudo spécifié sur un canal
CHAN_HELP_CMD_VOP
	    VOP        Configure la liste des VOP pour un canal (VOicePeople)
CHAN_HELP_CMD_DEHALFOP
	    DEHALFOP   Dehalfop le pseudo spécifié sur un canal
CHAN_HELP_CMD_DEOWNER
	    DEOWNER    Retire votre status d'owner sur un canal
CHAN_HELP_CMD_DEPROTECT
	    DEPROTECT  Deprotège le pseudo donné sur un canal
CHAN_HELP_CMD_HALFOP
	    HALFOP     Donne le status d'halfop au pseudo donné sur un canal
CHAN_HELP_CMD_HOP
	    HOP        Configure la liste de HOP d'un canal (HalfOP)
CHAN_HELP_CMD_OWNER
	    OWNER      Vous donne votre accès owner sur un canal
CHAN_HELP_CMD_PROTECT
	    PROTECT    Protège le pseudo spécifié sur un canal
CHAN_HELP_CMD_ADMIN
	    ADMIN      Protège le pseudo spécifié sur un canal
CHAN_HELP_CMD_DEADMIN
	    DEADMIN    Deprotège le pseudo donné sur un canal
CHAN_HELP_CMD_DEOP
	    DEOP       Deop le pseudo spécifié sur un canal

CHAN_HELP
	%S vous permet d'enregistrer et de contrôler divers
	aspects des canaux. %S empêche souvent de malicieux
	utilisateurs de faire des "take overs" sur les canaux
	en limitant ceux qui sont autorisés à être channel
	operator. Les commandes disponibles sont listées ci-dessous,
	pour les utiliser, tapez %R%S commande. Pour plus 
	d'informations sur une commande spécifique, tapez
	%R%S HELP commande.
	 
	
CHAN_HELP_EXPIRES
	
	Notez que tout canal non utilisé pendant %d jours
	(c'est-à-dire une période pendant laquelle aucun
	utilisateur figurant sur la liste d'accès du canal
	ne vient sur le canal) sera automatiquement effacé.

CHAN_HELP_REGISTER
	Syntaxe: REGISTER canal motdepasse description
	
	Enregistre un canal dans la base de données de %S. Pour
	utiliser cette commande, vous devez d'abord être un
	opérateur sur le canal que vous essayez 
	d'enregistrer. Le mot de passe est utilisé avec la commande
	IDENTIFY pour permettre à d'autres de faire des changements
	dans la configuration du canal plus tard. Le dernier
	paramètre, qui doit être inclus, est une description
	générale du canal.
	
	Quand vous enregistrez un canal, vous êtes enregistré
	comme "propriétaire" du canal. Le propriétaire du canal peut
	tout changer dans la configuration du canal, %S donnera
	aussi automatiquement au propriétaire les droits d'opérateur
	du canal quand il ou elle joint le canal.
	Consultez la commande ACCESS (%R%S HELP ACCESS) pour
	savoir comment donner un sous ensemble de ces droits à
	d'autres utilisateurs du canal.
	
	REMARQUE: Pour enregistrer un canal, vous devez
	d'abord enregistrer votre pseudo. Si ce n'est pas déjà fait,
	tapez %R%s HELP pour savoir comment faire.

CHAN_HELP_IDENTIFY
	Syntaxe: IDENTIFY canal motdepasse
	
	Certifie à %S que vous êtes autorisé à modifier le canal donné.
	Beaucoup de commandes demandent que vous utilisiez cette
	commande avant de les utiliser si vous n'êtes pas le propriétaire. 
	Le mot de passe est le même que celui que vous avez envoyé avec la
	commande REGISTER.
	
CHAN_HELP_LOGOUT
	Syntaxe: LOGOUT canal pseudo
	
	Cette commande supprime l'identification du pseudo sélectionné
	pour le canal donné.
	
	Si vous êtes le propriétaire du canal, vous pouvez utiliser
	la commande sur n'importe qui, sinon vous ne pouvez que
	l'utiliser sur vous même.

CHAN_HELP_DROP
	Syntaxe: DROP canal
	
	Efface le canal donné. Peut uniquement être utilisé par
	le propriétaire du canal, qui doit utiliser la commande
	IDENTIFY d'abord.

CHAN_HELP_SET
	Syntaxe: SET canal option paramètres
	
	Permet au propriétaire du canal de configurer diverses options
	du canal et d'autres informations.
	
	Options disponibles:
	
	    FOUNDER       Définit le propriétaire d'un canal
	    SUCCESSOR     Définit le successeur d'un canal
	    PASSWORD      Définit le mot de passe du canal
	    DESC          Définit la description du canal
	    URL           Associe un site avec un canal
	    EMAIL         Associe un E-mail avec le canal
	    ENTRYMSG      Définit un message envoyé aux utilisateurs
	                      qui entrent sur le canal
	    TOPIC         Change le topic du canal
	    BANTYPE       Définit comment les services mettent les bans
	                      sur le canal
	    MLOCK         Oblige ou interdit des modes de canal
	    KEEPTOPIC     Maintient le topic quand le canal n'est
	                      pas utilisé
	    OPNOTICE      Envoie une notice quand OP/DEOP sont utilisés
	    PEACE         Régule l'utilisation de commandes sensibles
	    PRIVATE       Cacher le canal de la commande LIST
	    RESTRICTED    Restreindre l'accès au canal
	    SECURE        Active les fonctions de sécurité de %S
	    SECUREOPS     Contrôle plus strict du status de chanop
	    SECUREFOUNDER Contrôle plus strict du status de propriétaire 
	                      du canal
	    SIGNKICK      Signe les kicks générés par la commande KICK
	    TOPICLOCK     Le topic peut uniquement être changé avec
	                      TOPIC
	    XOP           Change le système de privilèges utilisateurs
	
	Tapez %R%S HELP SET option pour plus d'informations sur
	une option particulière.

CHAN_HELP_SET_FOUNDER
	Syntaxe: SET canal FOUNDER pseudo
	
	Change le propriétaire d'un canal. Le nouveau pseudo doit être
	enregistré.

CHAN_HELP_SET_SUCCESSOR
	Syntaxe: SET canal SUCCESSOR pseudo
	
	Change le successeur d'un canal. Si le pseudo du propriétaire
	expire ou est effacé alors que le canal est toujours
	enregistré, le successeur deviendra le nouveau propriétaire du
	canal. Cependant, si le successeur a déjà trop de canaux
	enregistrés (%d), le canal sera tout de même effacé, comme
	si aucun successeur n'avait été nommé. Le nouveau pseudo
	doit être enregistré.

CHAN_HELP_SET_PASSWORD
	Syntaxe: SET canal PASSWORD motdepasse
	
	Définit le mot de passe utilisé pour s'identifier en tant
	que propriétaire du canal.

CHAN_HELP_SET_DESC
	Syntaxe: SET canal DESC description
	
	Définit la description du canal, qui apparait dans les
	commandes LIST et INFO.

CHAN_HELP_SET_URL
	Syntaxe: SET canal URL [adresse]
	
	Associe l'adresse de site web donnée avec le canal. Cette adresse 
	sera affichée à chaque fois que quelqu'un demande des informations
	sur le canal avec la commande INFO. Si aucun paramètre
	n'est donné, supprime l'adresse de site du canal.

CHAN_HELP_SET_EMAIL
	Syntaxe: SET canal EMAIL [adresse]
	
	Associe l'e-mail donné avec le canal. Cet
	e-mail sera affiché lorsque quelqu'un demande des informations
	sur le canal avec la commande INFO. Si aucun paramètre
	n'est donné, supprime l'e-mail actuel du canal.

CHAN_HELP_SET_ENTRYMSG
	Syntaxe: SET canal ENTRYMSG [message]
	
	Définit le message qui sera envoyé en /notice aux 
	utilisateurs quand ils entrent dans un canal. Si aucun
	paramètre n'est donné, aucun message n'est envoyé à
	l'entrée.
	
CHAN_HELP_SET_BANTYPE
	Syntaxe: SET canal BANTYPE typedeban
	
	Définit le type de ban qui sera utilisé par les services
	lorsqu'ils doivent bannir quelqu'un de votre canal.
	
	typedeban est un nombre entre 0 et 3 qui signifie:
	
	0: ban de style *!user@host
	1: ban de style *!*user@host
	2: ban de style *!*@host
	3: ban de style *!*user@*.domain

CHAN_HELP_SET_KEEPTOPIC
	Syntaxe: SET canal KEEPTOPIC {ON | OFF}
	
	Active ou désactive l'option de maintien du topic pour
	un canal. Lorsque le maintien du topic est défini, le 
	topic pour le canal sera retenu par %S même après que le
	dernier utilisateur quitte le canal, et sera restauré la
	prochaine fois que le canal est créé. 

CHAN_HELP_SET_TOPICLOCK
	Syntaxe: SET canal TOPICLOCK {ON | OFF}
	
	Active ou désactive l'option de verrouillage du topic pour
	un caanal. Lorsque le verrouillage du topic est actif,
	%S ne permettra pas le changement du topic du canal sauf
	avec la commande TOPIC.

CHAN_HELP_SET_MLOCK
	Syntaxe: SET canal MLOCK modes
	
	Configure les modes maintenus sur le canal. %S vous permet de
	toujours garder certains modes, d'empêcher l'utilisation d'autres
	modes et de laisser les modes restants en libre utilisation par
	les opérateurs du canal.
	
	Le paramètre modes est construit de la même façon que dans la 
	commande /MODE, les modes précédés d'un + étant toujours gardés, 
	et ceux précédés d'un - ne pouvant être utilisés. Notez cependant
	que, contrairement à la commande /MODE, chaque utilisation de 
	SET MLOCK réinitialisera les modes maintenus avant de mettre en 
	place les nouveaux!
	
	Remarque: Si vous choisissez de toujours garder le mode +k, 
	comme dans le second exemple ci-dessous, vous devez également 
	activer l'option RESTRICTED du canal (HELP SET 
	RESTRICTED pour plus d'informations), sinon toute personne 
	entrant sur le canal lorsqu'il est vide verra la clef!  
	
	Exemples:
	
	    SET #canal MLOCK +nt-iklps
	      Garde toujours les modes n et t, et empêche l'utilisation 
	      des modes i, k, l, p, et s. Le mode m est libre d'être 
	      mis ou retiré.
	
	    SET #canal MLOCK +knst-ilmp ma-clef
	      Garde toujours les modes k, n, s, et t, et désactive 
	      les modes i, l, m, et p. La clef du canal sera 
	      "ma-clef".
	
	    SET #canal MLOCK +
	      Réinitialise tous les modes précédemment maintenus, qui
	      pourront désormais être librement utilisés.

CHAN_HELP_SET_PEACE
	Syntaxe: SET canal PEACE {ON | OFF}
	
	Active ou désactive l'option de paix pour un canal.
	Quand elle est active, cette option empêche un 
	utilisateur de kicker, bannir ou enlever un privilège
	de canal d'un user qui a un niveau supérieur ou
	égal au sien via les commandes de %S.

CHAN_HELP_SET_PRIVATE
	Syntaxe: SET canal PRIVATE {ON | OFF}
	
	Active ou désactive l'option private pour un canal.
	Si private est défini, un %R%S LIST ne comprendra
	le canal dans aucune liste.

CHAN_HELP_SET_RESTRICTED
	Syntaxe: SET canal RESTRICTED {ON | OFF}
	
	Active ou désactive l'option d'accès restreint pour un
	canal. Lorsque l'accès restreint est actif, les
	utilisateurs qui n'auraient normalement pas la permission
	d'être opérateur du canal (les utilisateurs avec un niveau
	d'accès négatif et, si le contrôle des ops est actif, les utilisateurs
	qui ne sont pas sur la liste d'accès) seront kickés et bannis
	du canal.

CHAN_HELP_SET_SECURE
	Syntaxe: SET canal SECURE {ON | OFF}
	
	Active ou désactive %S caractéristiques de sécurité
	pour un canal. Lorsque la sécurité est active, seuls les
	utilisateurs qui ont enregistré leurs pseudos avec %s et
	qui sont identifiés avec leur mot de passe auront accès
	au canal sous contrôle de la liste d'accès.

CHAN_HELP_SET_SECUREOPS
	Syntaxe: SET canal SECUREOPS {ON | OFF}
	
	Active ou désactive le contrôle des ops sur un canal.
	Lorsque le contrôle des ops est actif, les utilisateurs 
	qui ne sont pas sur la liste d'accès ne pourront être op.
	
CHAN_HELP_SET_SECUREFOUNDER
	Syntaxe: SET canal SECUREFOUNDER {ON | OFF}
	
	Active ou désactive l'option contrôle du propriétaire pour un canal.
	Lorsque le contrôle du propriétaire est activé, seul le vrai 
	propriétaire pourra effacer le canal, changer son mot de passe, 
	son propriétaire et son successeur, et non pas ceux qui sont identifiés 
	avec %S.
	
CHAN_HELP_SET_SIGNKICK
	Syntaxe: SET canal SIGNKICK {ON | LEVEL | OFF}
	
	Active ou désactive les kicks signés sur un canal.
	Quand cette option est utilisée, les kicks générés
	par la commande %S KICK auront le pseudo qui a utilisé
	la commande dans leur raison.
	
	Si vous sélectionnez LEVEL, ceux qui ont un level qui est
	supérieur ou égal au level SIGNKICK sur le canal ne 
	génèreront pas de kicks signés. Consultez %R%S HELP LEVELS
	pour plus d'informations.

CHAN_HELP_SET_XOP
	Syntaxe: SET canal XOP {ON | OFF}
	
	Active ou désactive le système de listes de xOPs pour
	un canal. Si vous choisissez ce système, vous devez
	utiliser les commandes AOP/SOP/VOP pour donner
	des privilèges aux utilisateurs, sinon vous devez
	utiliser la commande ACCESS.
	
	Info technique: quand vous passez du système de liste 
	d'accès au système de listes de xOPs, les définitions de 
	niveaux et les niveaux des utilisateurs seront changés, et 
	donc vous ne les récupérerez pas tels quels si vous repassez 
	au système de liste d'accès!

	Vous devriez également vérifier que les utilisateurs sont
	dans la bonne liste de xOPs après être passé du système
	de liste d'accès à celui de listes de xOPs, car la
	conversion n'est pas parfaite à tous les coups... en fait,
	elle est déconseillée si vous avez changé les définitions
	des niveaux avec la commande LEVELS.
	
	Passer du système de listes de xOPs au système de liste
	d'accès ne pose aucun problème par contre.

CHAN_HELP_SET_OPNOTICE
	Syntaxe: SET canal OPNOTICE {ON | OFF}
	
	Active ou désactive l'option notice sur OP/DEOP pour un canal.
	Lorsque notice sur OP/DEOP est défini, %S enverra un notice au
	canal à chaque fois que les commandes OP ou DEOP sont
	utilisées par un utilisateur sur le canal.

CHAN_HELP_AOP
	Syntaxe: AOP canal ADD pseudo
	         AOP canal DEL {pseudo | liste d'entrées | liste}
	         AOP canal LIST [masque | liste]
	         AOP canal CLEAR
	
	Gère la liste des AOPs (AutoOPs) d'un canal. La liste des
	AOPs donne aux utilisateurs le droit de recevoir le 
	status d'opérateur automatiquement sur votre canal, 
	de se débannir ou de s'inviter en cas de besoin, d'avoir
	leur message d'accueil affiché à leur arrivée, etc.
	
	La commande AOP ADD ajoute le pseudo donné à la liste des
	AOPs.
	
	La commande AOP DEL supprime le pseudo donné de
	la liste des AOPs. Si une liste de numéros d'entrées est 
	donnée, ces entrées sont supprimées. (Consultez l'exemple
	pour LIST ci-dessous.)
	
	La commande AOP LIST affiche la liste des AOPs. Si un
	masque joker est donné, seules les entrées correspondantes
	au masque sont affichées. Si une liste de numéros d'entrées
	est donnée, seules ces entrées sont affichées, par exemple:

	   AOP #canal LIST 2-5,7-9
	        Liste toutes les entrées comprises entre 2 et 5 et
	        entre 7 et 9.
	
	La commande AOP CLEAR supprime toutes les entrées
	de la liste des AOPs.
	
	Les commandes AOP ADD et AOP DEL sont utilisables
	en étant SOP ou plus, alors que la commande AOP CLEAR
	ne peut être utilisée que par le propriétaire du canal.
	Cependant, la commande AOP LIST peut être utilisée
	en étant AOP ou plus.
	
	Cette commande peut avoir été désactivée pour votre canal,
	et vous devrez dans ce cas utiliser la liste d'accès. Tapez
	%R%S HELP ACCESS pour savoir comment utiliser la liste
	d'accès, et %R%S HELP SET XOP pour savoir comment 
	passer du système de liste d'accès à celui de liste des xOPs.

CHAN_HELP_HOP
	Syntaxe: HOP canal ADD pseudo
	         HOP canal DEL {pseudo | liste d'entrées | liste}
	         HOP canal LIST [masque | liste]
	         HOP canal CLEAR
	
	Gère la liste des HOPs (HalfOPs) d'un canal. La liste des
	HOPs donne aux utilisateurs le droit de recevoir le 
	status de semi-opérateur automatiquement sur votre canal.
	
	La commande HOP ADD ajoute le pseudo donné à la liste des
	HOPs.
	
	La commande HOP DEL supprime le pseudo donné de
	la liste des HOPs. Si une liste de numéros d'entrées est 
	donnée, ces entrées sont supprimées. (Consultez l'exemple
	pour LIST ci-dessous.)
	
	La commande HOP LIST affiche la liste des HOPs. Si un
	masque joker est donné, seules les entrées correspondantes
	au masque sont affichées. Si une liste de numéros d'entrées
	est donnée, seules ces entrées sont affichées, par exemple:

	   HOP #canal LIST 2-5,7-9
	        Liste toutes les entrées comprises entre 2 et 5 et
	        entre 7 et 9.
	
	La commande HOP CLEAR supprime toutes les entrées
	de la liste des HOPs.
	
	Les commandes HOP ADD, HOP DEL et HOP LIST sont 
	utilisables en étant AOP ou plus, alors que la commande 
	HOP CLEAR ne peut être utilisée que par le propriétaire 
	du canal.
	
	Cette commande peut avoir été désactivée pour votre canal,
	et vous devrez dans ce cas utiliser la liste d'accès. Tapez
	%R%S HELP ACCESS pour savoir comment utiliser la liste
	d'accès, et %R%S HELP SET XOP pour savoir comment 
	passer du système de liste d'accès à celui de liste des xOPs.

CHAN_HELP_SOP
	Syntaxe: SOP canal ADD pseudo
	         SOP canal DEL {pseudo | liste d'entrées | liste}
	         SOP canal LIST [masque | liste]
	         SOP canal CLEAR
	
	Gère la liste des SOPs (SuperOPs) d'un canal. La liste 
	des SOPs donne aux utilisateurs tous les droits donnés
	par la liste des AOPs, et ajoute ceux de pouvoir
	utiliser la liste de kicks automatiques et de mots
	interdits, de pouvoir lire et envoyer des memos de
	canal, etc.
	
	La commande SOP ADD ajoute le nick donné à la liste des
	SOPs.
	
	La commande SOP DEL supprime le pseudo donné de
	la liste des SOPs. Si une liste de numéros d'entrées est 
	donnée, ces entrées sont supprimées. (Consultez l'exemple
	pour LIST ci-dessous.)
	
	La commande SOP LIST affiche la liste des SOPs. Si un
	masque joker est donné, seules les entrées correspondantes
	au masque sont affichées. Si une liste de numéros d'entrées
	est donnée, seules ces entrées sont affichées, par exemple:

	   SOP #canal LIST 2-5,7-9
	        Liste toutes les entrées comprises entre 2 et 5 et
	        entre 7 et 9.
	
	La commande SOP CLEAR supprime toutes les entrées
	de la liste des SOPs.
	
	Les commandes SOP ADD, SOP DEL et SOP CLEAR ne sont 
	utilisables que par le propriétaire du canal.
	Cependant, la commande SOP LIST peut être utilisée
	en étant AOP ou plus.
	
	Cette commande peut avoir été désactivée pour votre canal,
	et vous devrez dans ce cas utiliser la liste d'accès. Tapez
	%R%S HELP ACCESS pour savoir comment utiliser la liste
	d'accès, et %R%S HELP SET AOP pour savoir comment 
	passer du système de liste d'accès à celui de liste des xOPs.

CHAN_HELP_VOP
	Syntaxe: VOP canal ADD pseudo
	         VOP canal DEL {pseudo | liste d'entrées | liste}
	         VOP canal LIST [masque | liste]
	         VOP canal CLEAR
	
	Gère la liste des VOPs (VOicePeople) d'un canal. La 
	liste des VOPs permet aux utilisateurs d'obtenir le status
	de voice automatiquement ou de se l'attribuer s'ils
	ne l'ont pas encore.
	
	La commande VOP ADD ajoute le pseudo donné à la liste des
	VOPs.
	
	La commande VOP DEL supprime le pseudo donné de
	la liste des VOPs. Si une liste de numéros d'entrées est 
	donnée, ces entrées sont supprimées. (Consultez l'exemple
	pour LIST ci-dessous.)
	
	La commande VOP LIST affiche la liste des VOPs. Si un
	masque joker est donné, seules les entrées correspondantes
	au masque sont affichées. Si une liste de numéros d'entrées
	est donnée, seules ces entrées sont affichées, par exemple:

	   VOP #canal LIST 2-5,7-9
	        Liste toutes les entrées comprises entre 2 et 5 et
	        entre 7 et 9.
	
	La commande VOP CLEAR supprime toutes les entrées
	de la liste des VOPs.
	
	Les commandes VOP ADD, VOP DEL et VOP LIST sont 
	utilisables en étant AOP ou plus, alors que la commande 
	VOP CLEAR ne peut être utilisée que par le propriétaire 
	du canal.
	
	Cette commande peut avoir été désactivée pour votre canal,
	et vous devrez dans ce cas utiliser la liste d'accès. Tapez
	%R%S HELP ACCESS pour savoir comment utiliser la liste
	d'accès, et %R%S HELP SET AOP pour savoir comment 
	passer du système de liste d'accès à celui de liste des xOPs.

CHAN_HELP_ACCESS
	Syntaxe: ACCESS canal ADD pseudo level
	         ACCESS canal DEL {pseudo | numéros d'entrée | liste}
	         ACCESS canal LIST [mask | liste]
	         ACCESS canal CLEAR
	
	Gère la liste d'accès d'un canal. La liste d'accès
	détermine quels utilisateurs ont le status d'opérateur ou
	ont accès aux commandes de %S pour le canal. Différents
	levels utilisateur donnent accès à différents lots de 
	privilèges, %R%S HELP ACCESS LEVELS pour des informations
	plus détaillées. Tout pseudo qui n'est pas sur la liste
	d'accès a un level utilisateur de 0.
	
	La commande ACCESS ADD ajoute le pseudo donné à la liste
	d'accès avec le level utilisateur choisi, si le pseudo est
	déjà présent dans la liste, son niveau d'accès est remplacé
	par le level spécifié dans la commande. Le level spécifié
	doit être inférieur que celui qui utilise la commande, et
	si le pseudo est déjà dans la liste d'accès, le level
	d'accès actuel de ce nick doit être inférieur à celui de
	la personne utilisant la commande.
	
	La commande ACCESS DEL supprime le pseudo donné de
	la liste d'accès. Si une liste de numéros d'entrées est 
	donnée, ces entrées sont supprimées. (Consultez l'exemple
	pour LIST ci-dessous.)
	
	La commande ACCESS LIST affiche la liste d'accès. Si un
	mask joker est donné, seules les entrées correspondantes
	au mask sont affichées. Si une liste de numéros d'entrée
	est donnée, seules ces entrées sont affichées, par exemple:

	   ACCESS #canal LIST 2-5,7-9
	        Liste toutes les entrées comprises entre 2 et 5 et
	        entre 7 et 9.
	
	La commande ACCESS CLEAR supprime toutes les entrées
	de la liste d'accès.

CHAN_HELP_ACCESS_LEVELS
	Levels d'accès utilisateur
	
	Par défaut, les levels d'accès suivants sont définis:
	
	   Proprio   Plein accès aux fonctions de %S, Op 
	               automatique dès l'entrée sur le canal.
	               Notez que seule une personne peut
	               avoir le status de propriétaire (il ne peut
	               être donné en utilisant la commande
	               ACCESS).
	        10   Accès à la commande AKICK, Op automatique.
	         5   Op automatique.
	         3   Voice automatique.
	         0   Aucun privilège particulier.
	        <0   Ne doit pas être op.
	
	Ces levels peuvent être changés, ou de nouveaux ajoutés,
	en utilisant la commande LEVELS; tapez %R%S HELP LEVELS
	pour plus d'informations.

CHAN_HELP_AKICK
	Syntaxe: AKICK channel ADD {nick | masque} [raison]
	         AKICK channel STICK masque
	         AKICK channel UNSTICK masque
	         AKICK channel DEL {nick | mask | entry-num | liste}
	         AKICK channel LIST [masque | entry-num | liste]
	         AKICK channel VIEW [masque | entry-num | liste]
	         AKICK canal ENFORCE
	         AKICK canal CLEAR
	
	Gère la liste de kicks automatiques d'un canal. Si 
	un utilisateur figurant sur la liste de kicks automatiques tente
	de joindre un canal, %S bannira cet utilisateur du canal, 
	et kickera ensuite l'utilisateur.
	
	La commande AKICK ADD ajoute le pseudo donné ou le
	mask d'utilisateur à la liste de kicks automatiques. Si une 
	raison est donnée avec la commande, cette raison sera utilisée 
	lorsque l'utilisateur est kické, sinon, la raison par défaut est
	"Vous n'ètes plus le bienvenue sur ce canal". 
	When akicking a registered nick the nickserv account
	will be added to the akick list instead of the mask.
	All users within that nickgroup will then be akicked.
	
	La commande AKICK DEL supprime le pseudo ou le masque donné
	de la liste de kicks automatiques. Cependant, il n'enlève pas 
	les bans placés par une des entrées, ceux-là doivent être supprimés
	manuellement.
	
	La commande AKICK STICK bannit en permanence le masque
	donné sur le canal. Si quelqu'un essaie d'enlever le
	ban, %S le remettra automatiquement. Vous ne pouvez l'utiliser
	sur des pseudos enregistrés.
	
	La commande AKICK UNSTICK annule les effets de la commande
	AKICK STICK, et vous pourrez donc à nouveau enlever le
	ban du canal.
	
	La commande AKICK LIST affiche la liste de kicks 
	automatiques, ou optionnellement seulement les entrées de la liste 
	de kicks automatiques qui correspondent au mask donné.
	
	La commande AKICK VIEW est une version plus détaillée de
	la commande AKICK LIST.
	
	La commande AKICK ENFORCE force %S à bannir tous les
	utilisateurs présents sur le canal qui sont concernés 
	par une des entrées de la liste de kicks automatiques.
	
	La commande AKICK CLEAR supprime toutes les entrées
	de la liste de kicks automatiques.

CHAN_HELP_LEVELS
	Syntaxe: LEVELS canal SET type level
	         LEVELS canal {DIS | DISABLE} type
	         LEVELS canal LIST
	         LEVELS canal RESET
	
	La commande LEVELS permet un contrôle précis de la 
	signification des levels d'accès utilisés pour les canaux.
	Avec cette commande, vous pouvez définir le level d'accès
	requis pour la majorité des fonctions de %S. (Les commandes
	SET FOUNDER et SET PASSWORD, ainsi que cette commande,
	sont toujours réservées au propriétaire du canal.)
	
	LEVELS SET permet au level d'accès pour une fonction ou un
	groupe de fonctions d'être changé. LEVELS DISABLE (ou DIS
	en abrégé) désactive une fonction automatique ou empêche l'accès
	à une fonction par quiconque sauf le propriétaire du canal.
	LEVELS LIST montre les levels courants pour chaque fonction
	ou groupe de fonctions. LEVELS RESET réinitialise les levels
	avec les levels par défaut d'un canal fraichement créé 
	(consultez HELP ACCESS LEVELS).

	Pour une liste de caractéristiques et fonctions pour lesquels
	des levels peuvent être définis, consultez HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Les noms de caractéristiques/fonctions suivants sont utilisés.
	Notez que les levels pour AUTODEOP et NOJOIN sont des levels
	maximum, alors que tous les autres sont des levels minimum.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Syntaxe: INFO canal [ALL]
	
	Liste les informations concernant le canal nommé
	enregistré, dont son propriétaire, la date d'enregistrement,
	la date de dernière utilisation, la description, et
	les modes bloqués, si il y en a. Si ALL est donné, le 
	message d'entrée et le successeur seront également affichés.
	
	Par défaut, l'option ALL est limitée à ceux ayant un accès 
	de propriétaire sur le canal.

CHAN_HELP_LIST
	Syntaxe: LIST modèle
	
	Liste tous les canaux enregistrés correspondants au
	modèle donné. (Les canaux avec l'option PRIVATE ne
	sont pas listés.)

CHAN_HELP_OP
	Syntaxe: OP [#canal [pseudo]]
	
	Op le pseudo sélectionné sur un canal. Si pseudo n'est pas
	donné, vous oppera sur le canal donné. Si pseudo et
	#canal ne sont pas donnés, vous oppera sur tous
	les canaux où vous êtes.
	
	Par défaut, limité aux AOPs ou à ceux ayant un niveau
	d'accès de 5 ou plus sur le canal.

CHAN_HELP_DEOP
	Syntaxe: DEOP [#canal [pseudo]]
	
	Deop le pseudo sélectionné sur un canal. Si pseudo n'est pas
	donné, vous deoppera sur le canal donné. Si pseudo et
	#canal ne sont pas donnés, vous deoppera sur tous
	les canaux où vous êtes.

	Par défaut, limité aux AOPs ou à ceux ayant un niveau 
	d'accès de 5 ou plus sur le canal.
	
CHAN_HELP_VOICE
	Syntaxe: VOICE [#canal [pseudo]]
	
	Voice le pseudo sélectionné sur un canal. Si pseudo n'est pas
	donné, vous voicera sur le canal donné. Si pseudo et
	canal ne sont pas donnés, vous voicera sur tous
	les canaux où vous êtes.
	
	Par défaut, limité aux AOPs ou à ceux ayant un level d'accès 
	de 5 ou plus sur le canal, ou aux VOPs ou à ceux ayant un 
	niveau d'accès de 3 ou plus en cas de voice de soi-même.

CHAN_HELP_DEVOICE
	Syntaxe: DEVOICE [#canal [pseudo]]
	
	Devoice le pseudo sélectionné sur un canal. Si pseudo n'est pas
	donné, vous devoicera sur le canal donné. Si pseudo et
	#canal ne sont pas donnés, vous devoicera sur tous
	les canaux où vous êtes.

	Par défaut, limité aux AOPs ou à ceux ayant un level d'accès 
	de 5 ou plus sur le canal, ou aux VOPs ou à ceux ayant un 
	niveau d'accès de 3 ou plus en cas de devoice de soi-même.
	
CHAN_HELP_HALFOP
	Syntaxe: HALFOP [#canal [pseudo]]
	
	Halfop le pseudo sélectionné sur un canal. Si pseudo n'est pas
	donné, vous halfoppera sur le canal donné. Si pseudo et
	#canal ne sont pas donnés, vous halfoppera sur tous
	les canaux où vous êtes.
	
	Par défaut, limité aux AOPs ou à ceux ayant un level d'accès 
	de 5 ou plus sur le canal, ou aux HOPs ou à ceux ayant un 
	niveau d'accès de 4 ou plus en cas de halfop de soi-même.

CHAN_HELP_DEHALFOP
	Syntaxe: DEHALFOP [#canal [pseudo]]
	
	Dehalfop le pseudo sélectionné sur un canal. Si pseudo n'est pas
	donné, vous dehalfoppera sur le canal donné. Si pseudo et
	#canal ne sont pas donnés, vous dehalfoppera sur tous
	les canaux où vous êtes.

	Par défaut, limité aux AOPs ou à ceux ayant un level d'accès 
	de 5 ou plus sur le canal, ou aux HOPs ou à ceux ayant un 
	niveau d'accès de 4 ou plus en cas de dehalfop de soi-même.
	
CHAN_HELP_PROTECT
	Syntaxe: PROTECT [#canal [pseudo]]
	
	Protège le pseudo sélectionné sur un canal. Si pseudo n'est pas
	donné, vous protègera sur le canal donné. Si pseudo et
	#canal ne sont pas donnés, vous protègera sur tous
	les canaux où vous êtes.
	
	Par défaut, limité au propriétaire du channel, ou aux SOPs 
	ou à ceux ayant un niveau d'accès de 10 ou plus en cas de 
	protection de soi-même.

CHAN_HELP_DEPROTECT
	Syntaxe: DEPROTECT [#canal [pseudo]]
	
	Enlève la protection du pseudo sélectionné sur un canal. Si 
	pseudo n'est pas donné, vous déprotègera sur le canal 
	donné. Si pseudo et #canal ne sont pas donnés, vous 
	déprotègera sur tous les canaux où vous êtes.

	Par défaut, limité au propriétaire du canal, ou aux SOPs 
	ou à ceux ayant un niveau d'accès de 10 ou plus en cas de 
	suppression de protection pour soi-même.

CHAN_HELP_OWNER
	Syntaxe: OWNER [#canal]
	
	Vous donne le status de propriétaire sur le canal donné. 
	Si #canal n'est pas donné, vous donnera le status de
	propriétaire sur tous les canaux où vous êtes, à condition
	bien sûr d'y avoir droit.
	
	Limité à ceux ayant un accès de propriétaire sur le canal.

CHAN_HELP_DEOWNER
	Syntaxe: DEOWNER [#canal]
	
	Vous supprime le status de propriétaire sur le canal 
	donné. Si canal n'est pas donné, vous donnera le status 
	de propriétaire sur tous les canaux où vous êtes, à 
	condition bien sûr d'y avoir droit.
	
	Limité à ceux ayant un accès de propriétaire sur le canal.

CHAN_HELP_INVITE
	Syntaxe: INVITE canal
	
	Demande à %S de vous inviter sur le canal donné. 
	
	Par défaut, limité aux AOPs ou à ceux ayant un niveau 
	d'accès de 5 ou plus sur le canal.

CHAN_HELP_UNBAN
	Syntaxe: UNBAN canal
	
	Demande à %S d'enlever tous les bans vous empêchant
	d'entrer sur le canal donné. 

	Par défaut, limité aux AOPs ou à ceux ayant un niveau 
	d'accès de 5 ou plus sur le canal.

CHAN_HELP_KICK
	Syntaxe: KICK [#canal [pseudo [raison]]]
	
	Kicke le pseudo sélectionné sur un canal. Si pseudo n'est pas
	donné, vous kickera sur le canal donné. Si pseudo et
	#canal ne sont pas donnés, vous kickera sur tous
	les canaux où vous êtes.

	Par défaut, limité aux AOPs ou à ceux ayant un niveau 
	d'accès de 5 ou plus sur le canal.

CHAN_HELP_BAN
	Syntaxe: BAN [#canal [pseudo [raison]]]
	
	Bannit le pseudo sélectionné sur un canal. Si pseudo n'est pas
	donné, vous bannira sur le canal donné. Si pseudo et
	canal ne sont pas donnés, vous bannira sur tous
	les canaux où vous êtes.

	Par défaut, limité aux AOPs ou à ceux ayant un niveau 
	d'accès de 5 ou plus sur le canal.

CHAN_HELP_TOPIC
	Syntaxe: TOPIC canal [topic]
	
	%S change le topic du canal pour celui spécifié. Si 
	topic n'est pas donné, alors le topic du canal sera 
	supprimé. Cette commande est surtout utile en conjonction
	avec la commande SET TOPICLOCK. Consultez 
	%R%S HELP SET TOPICLOCK pour plus d'informations.
	
	Par défault, limité à ceux ayant un accès de propriétaire
	sur le canal.

CHAN_HELP_CLEAR
	Syntaxe: CLEAR canal quoi
	
	Demande à %S d'enlever certains aspects d'un canal. 
	quoi peut être un des suivants:
	
	     MODES   Réinitialise tous les modes d'un canal
	     BANS    Enlève tous les bans d'un canal.
	     EXCEPTS Enlève tous les excepts d'un canal.
	     OPS     Enlève le status d'opérateur du canal à
	             tous les opérateurs du canal.
	     HOPS    Enlève le status d'halfop (mode +h) a tout les halfop
	             du canal.
	     VOICES  Enlève le status de "voice" (mode +v) à toute
	             personne ayant ce mode.
	     USERS   Kicke tous les utilisateurs d'un canal.
	
	Par défault, limité à ceux ayant un accès de propriétaire
	sur le canal.
	
CHAN_HELP_GETKEY
	Syntaxe: GETKEY canal
	
	Retourne la clef du canal donné. GETKEY est une commande
	qui est principalement destiné à être utilisée par des
	bots ou des scripts, pour cela, la réponse a le format
	suivant:
	
	KEY <canal> <clef>
	
	La clef sera "NO KEY" si aucune clef n'est définie.
	
CHAN_HELP_SENDPASS
	Syntaxe: SENDPASS canal
	
	Envoie le mot de passe du canal donné à l'adresse e-mail 
	qui a été définie pour le propriétaire de ce canal. Cette 
	commande est très utile pour régler les problèmes de mots 
	de passe perdus.
	
	Peut être limitée aux IRC operators sur certains
	réseaux.
	
	Cette commande est indisponible quand l'encryption est
	activée.

CHAN_SERVADMIN_HELP
	
	Les Services admins peuvent aussi effacer tout canal sans
	avoir besoin de s'identifier avec un mot de pass, et peuvent
	voir l'access, AKICK, et level lists de tout canaux.

CHAN_SERVADMIN_HELP_LOGOUT
	Syntaxe: LOGOUT canal [pseudo]
	
	Cette commande supprime l'identification du pseudo sélectionné
	pour le canal donné.
	
	Si vous êtes le propriétaire du canal, vous pouvez utiliser
	la commande sur n'importe qui, sinon vous ne pouvez que
	l'utiliser sur vous même.
	
	Si vous êtes un Services admin, vous pouvez utiliser
	cette commande sur n'importe qui pour n'importe quel 
	canal sans avoir besoin d'être le propriétaire du canal.
	Vous pouvez également omettre le paramètre pseudo, cela aura
	pour effet de supprimer l'identification pour tous les 
	utilisateurs identifiés au canal.

CHAN_SERVADMIN_HELP_DROP
	Syntaxe: DROP canal
	
	Efface le canal nommé. Seuls les Services admins
	peuvent effacer un canal pour lequel ils n'ont
	pas été identifiés.

CHAN_SERVADMIN_HELP_SET
	
	Les Services admins peuvent aussi définir l'option 
	NOEXPIRE, avec lequel les canaux peuvent être empêchés
	d'expirer. De plus, les Services admins peuvent définir
	les options de tout canal sans s'identifier avec le
	mot de passe du canal. 

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Syntaxe: SET canal NOEXPIRE {ON | OFF}
	
	Définit si le canal donné expirera. Mettre ceci à ON
	empêche le canal d'expirer.
	
	Réservée aux Services admins.

CHAN_SERVADMIN_HELP_INFO
	
	Les services admins peuvent utiliser le paramètre ALL 
	avec n'importe quel canal.

CHAN_SERVADMIN_HELP_LIST
	Syntaxe: LIST modèle
	
	Liste tous les canaux enregistrés correspondants au modèle
	donné. Les canaux avec l'option PRIVATE seront uniquement
	affichés aux Services admins. Les canaux ayant l'option 
	NOEXPIRE seront préfixés par un ! pour les Services admins. 
	
	Si les options FORBIDDEN ou NOEXPIRE sont données, seuls 
	les canaux qui, respectivement, sont interdits ou ont l'option
	NOEXPIRE active seront affichés. Si plus options sont
	données, les deux types de canaux seront affichés. Ces options
	sont réservées aux Services admins.

CHAN_SERVADMIN_HELP_GETPASS
	Syntaxe: GETPASS canal
	
	Retourne le mot de passe du canal donné. Notez que
	lorsque cette commande est utilisée, un message comprenant
	la personne qui a utilisé cette commande et le canal
	pour lequel elle a été utilisée sera loggée et envoyée en
	un WALLOPS/GLOBOPS.
	
	Réservée aux Services admins.

CHAN_SERVADMIN_HELP_FORBID
	Syntaxe: FORBID canal [raison]
	
	Empêche toute personne d'enregistrer ou d'utiliser le
	canal donné. Peut être annulé en effacant le canal.
	
	La raison peut être requise sur certains réseaux.
	
	Réservée aux Services admins.

CHAN_SERVADMIN_HELP_SUSPEND
	Syntax: SUSPEND canal [raison]
	
	Désactive l'enregistrement du canal donné. Ceci peu
	etre inversé via la commande UNSUSPEND et recupèrer
	les données et configurations.
	
	Une raison peut etre demandé sur certain réseaux.
	
	Limité aux Services admins.

CHAN_SERVADMIN_HELP_UNSUSPEND
	Syntax: UNSUSPEND canal
	
	Libère un canal qui a été suspendu. Toutes les données
	et configurations seront restauré tel qu'elles etait
	avant la suspension.
	
	Limité aux Services admins.

CHAN_SERVADMIN_HELP_STATUS
	Syntaxe: STATUS canal pseudo
	
	Retourne le level d'accès actuel du pseudo donné pour le 
	canal donné. La réponse est de la forme:
	
	    STATUS canal pseudo access-level
	
	Si une erreur se produit, la réponse sera de la forme:
	
	    STATUS ERROR error-message
	
	Réservée aux Services admins.

###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP_CMD_SEND
	    SEND   Envoie un memo à un pseudo ou un canal
MEMO_HELP_CMD_CANCEL
	    CANCEL Annule le dernier memo que vous avez envoyé
MEMO_HELP_CMD_LIST
	    LIST   Liste vos memos
MEMO_HELP_CMD_READ
	    READ   Lit un ou des memos
MEMO_HELP_CMD_DEL
	    DEL    Efface un ou des memos
MEMO_HELP_CMD_SET
	    SET    Définit des options relatifs aux memos
MEMO_HELP_CMD_INFO
	    INFO   Affiche des informations à propos de vos memos
MEMO_HELP_CMD_RSEND
	    RSEND  Envoie un memo et demande un accusé de reception
MEMO_HELP_CMD_CHECK
	    CHECK  Vérifie si le dernier memo envoyé a un pseudo a été
	           lu
MEMO_HELP_CMD_SENDALL
	    SENDALL  Envoie un memo a tout les pseudos enregistré
MEMO_HELP_CMD_STAFF
	    STAFF  Envoie un memo à tout les opers/admins

MEMO_HELP_HEADER
	%S est un utilitaire permettant aux utilisateurs IRC
	d'envoyer de courts messages aux autres utilisateurs IRC,
	qu'ils soient online à ce moment là ou non, ou aux 
	canaux(*). Le pseudo de l'expéditeur ET du destinataire ou
	du canal doivent être enregistrés pour pouvoir envoyer un
	memo.
	
	Les commandes de %S sont:
	

MEMO_HELP_ADMIN
	not used.
MEMO_HELP_OPER
	not used.
MEMO_HELP_FOOTER

	Tapez %R%S HELP commande pour de l'aide sur
	une des commandes ci-dessus.

	(*) Par défaut, tout utilisateur avec au moins un level d'accès
	    de 10 sur un canal peut lire ces memos du canal. Ceci
	    peut être changé avec la commande LEVELS de %s.

MEMO_HELP_SEND
	Syntaxe: SEND {pseudo | canal} texte du memo
	
	Envoie un memo au pseudo ou canal nommé contenant
	texte du memo. En envoyant à un pseudo, le destinataire
	sera informé qu'il/elle a un nouveau memo. Le pseudo/canal
	destinataire doit être enregistré.
	
MEMO_HELP_CANCEL
	Syntaxe: CANCEL {pseudo | canal}
	
	Supprime le dernier memo que vous avez envoyé à un pseudo ou
	canal donné, à condition qu'il n'ait pas été lu lorsque vous
	utilisez la commande.

MEMO_HELP_LIST
	Syntaxe: LIST [canal] [liste | NEW]
	
	Liste tous les memos que vous avez actuellement. Avec NEW,
	liste uniquement les nouveaux memos (non lus). Les memos non
	lus sont marqués avec un "*" à gauche du numéro du memo. Vous
	pouvez également spécifier une liste de numéros, comme dans
	l'exemple ci dessous:

	   LIST 2-5,7-9
	      Liste les memos 2 à 5 et 7 à 9.

MEMO_HELP_READ
	Syntaxe: READ [canal] {nombre | liste | LAST | NEW}
	
	Vous envoie le texte des memos spécifiés. Si LAST est donné,
	vous envoie le dernier memo reçu. Si NEW est donné, vous
	envoie tous vos nouveaux memos. Sinon, vous envoie le memo
	numéro nombre. Vous pouvez également donner une liste
	de nombres, comme dans cet exemple:
	
	   READ 2-5,7-9
	      Affiche les memos 2 à 5 et 7 à 9.

MEMO_HELP_DEL
	Syntaxe: DEL [canal] {nombre | liste | LAST | ALL}
	
	Supprime le ou les memos spécifiés. Vous pouvez
	fournir plusieurs numéros de memos ou des ordres de nombres
	au lieu d'un nombre unique, comme dans le second exemple
	ci-dessous.
	
	Si LAST est donné, le dernier mémo sera supprimé.
	Si ALL est donné, cela supprimera tous vos memos.
	
	Exemples:
	
	   DEL 1
	      Supprime votre premier memo.
	
	   DEL 2-5,7-9
	      Supprime les memos compris entre 2 et 5 et entre 7 et 9.

MEMO_HELP_SET
	Syntaxe: SET option paramètres
	
	Définit plusieurs options de memo. option peut être:
	
	    NOTIFY      Définit si vous serez notifiés de vos
	                  nouveaux memos (seulement pour les 
	                  pseudos)
	    LIMIT       Définit le nombre de memos maximum que 
	                  vous pouvez recevoir
	
	Tapez %R%S HELP SET option pour plus 
	d'informations sur une option spécifique.

MEMO_HELP_SET_NOTIFY
	Syntaxe: SET NOTIFY {ON | LOGON | NEW | OFF}
	
	Change quand vous serez notifié de vos nouveaux
	memos:
	
	    ON     Vous serez notifiés lorsque vous vous connectez,
	           lorsque vous enlevez /AWAY, et quand ils vous
	           sont envoyés.	
	    LOGON  Vous serez uniquement notifiés des memos lorsque
	           vous vous connectez ou quand vous enlevez
	           /AWAY.
	    NEW    Vous serez uniquement notifiés des memos 
	           lorsqu'ils vous sont envoyés.
	    OFF    Vous ne recevrez aucune notification.
	
	ON est la combination de LOGON et NEW.

MEMO_HELP_SET_LIMIT
	Syntaxe: SET LIMIT [canal] limite
	
	Définit le nombre maximum de memos que vous (ou le canal
	donné) est autorisé à avoir. Si vous définissez ceci à 0,
	personne ne pourra vous envoyer de memos. Cependant, vous
	ne pouvez pas définir ceci à plus que %d.

MEMO_HELP_INFO
	Syntaxe: INFO [canal]
	
	Affiche des informations sur le nombre de memos que vous
	avez, combien sont non lus, et combien de memos vous pouvez
	recevoir au total. Avec un paramètre, affiche la même
	information pour le canal donné.

MEMO_SERVADMIN_HELP_SET_LIMIT
	Syntaxe: SET LIMIT [utilisateur | canal] {limite | NONE} [HARD]
	
	Définit le nombre maximum de memos qu'un utilisateur ou
	un canal est autorisé à avoir. Définir la limite à 0
	empêche l'utilisateur de recevoir des memos, définir ceci
	à NONE permet à l'utilisateur de recevoir et garder
	autant de memos qu'ils veulent. Si vous ne donnez pas de
	pseudo ou de canal, votre propre limite est définie.
	
	Ajouter HARD empêche l'utilisateur de changer cette limite.
	Ne pas ajouter HARD a l'effet inverse, permettant à 
	l'utilisateur de changer cette limite (même si une limite
	antérieure a été définie avec HARD).
	
	Cette utilisation de la commande SET LIMIT est réservée aux
	Services admins. Les autres utilisateurs peuvent uniquement
	entrer une limite pour eux-mêmes ou un canal sur lequel ils
	ont de tels privilèges, ne peuvent supprimer %d limite, et
	ne peuvent définir de limite stricte.

MEMO_SERVADMIN_HELP_INFO
	Syntaxe: INFO [pseudo | canal]
	
	Sans paramètre, affiche les informations sur le nombre
	de memos que vous avez, combien sont non lus, et combien
	de memos vous pouvez recevoir au total.
	
	Avec un paramètre de canal, affiche la même information
	pour le canal donné.
	
	Avec un paramètre de pseudo, affiche la même information 
	pour le pseudo donné. Cette utilisation réservée aux Services
	admins.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_STAFF
	Syntaxe: STAFF texte

	Envoie un memo à tout le staff des services contenant le
	texte.
	Note: Si vous avez un bot ou des clones dans la liste des
	oper et admins, ils recevront aussi un memo.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_SENDALL
	Syntaxe: SENDALL texte

	Envoie un memo a tout les pseudos enregistré contenant le texte.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_RSEND
	Syntaxe: RSEND {pseudo | canal} texte
	
	Envoie un memo au pseudo ou canal donné contenant le texte.
	Lorsque c'est envoyé a un pseudo, le destinataire recevra une notice
	l'avertissant de l'arrivé de nouveau(x) memo(s). Le destinataire dois
	etre un pseudo ou un canal enregistré.
	Une fois le memo lu, un accusé de réception vous est automatiquement
	envoyé afin de vous assuré que le memo a été lu.

###########################################################################
#
# OperServ help messages
#
###########################################################################

MEMO_HELP_CHECK
	Syntaxe: CHECK pseudo
	
	Vérifie si le dernier memo que vous avez envoyé au pseudo donné
	a été lu ou non.
	Note: Cela fonctionne uniquement avec un pseudo et non pas avec un
	canal.

###########################################################################
#
# OperServ help messages
#
###########################################################################

OPER_HELP_CMD_GLOBAL
	    GLOBAL      Envoie un message à tous les utilisateurs
OPER_HELP_CMD_STATS
	    STATS       Affiche le status des Services et du réseau
OPER_HELP_CMD_OPER
	    OPER        Modifie la liste des opérateurs des Services
OPER_HELP_CMD_ADMIN
	    ADMIN       Modifie la liste des administrateurs des Services
OPER_HELP_CMD_STAFF
	    STAFF       Affiche le staff des services et leurs status 
OPER_HELP_CMD_MODE
	    MODE        Change les modes d'un canal
OPER_HELP_CMD_KICK
	    KICK        Expulse un utilisateur d'un canal
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  Supprime tous les modes d'un canal
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  Déconnecte tous les utilisateurs ayant un
	                certain hostname
OPER_HELP_CMD_AKILL
	    AKILL       Contrôle la liste des AKILLs
OPER_HELP_CMD_SGLINE
	    SGLINE      Contrôle la liste des SGLINEs
OPER_HELP_CMD_SQLINE
	    SQLINE      Contrôle la liste des SQLINEs
OPER_HELP_CMD_SZLINE
	    SZLINE      Contrôle la liste des SZLINEs
OPER_HELP_CMD_CHANLIST
	    CHANLIST    Liste tous les canaux
OPER_HELP_CMD_USERLIST
	    USERLIST    Liste tous les utilisateurs
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   Définit les messages affichés aux utilisateurs
	                à la connexion
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  Définit les messages affichés aléatoirement
	                aux utilisateurs à la connexion
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    Définit les messages affichés aux utilisateurs
	                qui deviennent IRCops
OPER_HELP_CMD_SESSION
	    SESSION     Affiche la liste des sessions par host
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   Modifie la liste d'exceptions à la limite de
	                session
OPER_HELP_CMD_NOOP
	    NOOP        Supprime temporairement toutes les O:lines
	                d'un serveur
OPER_HELP_CMD_JUPE
	    JUPE        "Jupiter" un serveur
OPER_HELP_CMD_IGNORE
	    IGNORE      Modifier la liste d'ignore des services
OPER_HELP_CMD_SET
	    SET         Configure des options globales des Services
OPER_HELP_CMD_RELOAD
	    RELOAD      Recharge le fichier de configuration des Services
OPER_HELP_CMD_UPDATE
	    UPDATE      Oblige les bases de données des Services
	                à être sauvegardées immédiatement.
OPER_HELP_CMD_RESTART
	    RESTART     Sauvegarde les bases de données et relance les
	                Services.
OPER_HELP_CMD_QUIT
	    QUIT        Arrête les Services sans sauvegarde
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    Arrête les Services avec sauvegarde
OPER_HELP_CMD_DEFCON
	    DEFCON      Active/Désactive DefCon
OPER_HELP_CMD_CHANKILL
	    CHANKILL    Kill tout les utilisateurs d'un canal spécifique
OPER_HELP_CMD_OLINE
	    OLINE       Donne les flags d'un Oper à un utilisateur
OPER_HELP_CMD_UMODE
	    UMODE       Change un mode d'un utilisateur
OPER_HELP_CMD_SVSNICK
	    SVSNICK     Force le changement de pseudo d'un utilisateur
OPER_HELP_CMD_MODLOAD
	    MODLOAD     Charge un module
OPER_HELP_CMD_MODUNLOAD
	    MODUNLOAD   Décharge un module
OPER_HELP_CMD_MODINFO
	    MODINFO     Liste les informations sur un module chargé
OPER_HELP_CMD_MODLIST
	    MODLIST     Liste les modules chargés

OPER_HELP
	Commandes %S:

OPER_HELP_LOGGED
	Remarque: Toutes les commandes envoyées à %S sont enregistrées!

OPER_HELP_GLOBAL
	Syntaxe: GLOBAL message
	
	Permet aux Administrators d'envoyer des messages à tous
	les utilisateurs du réseau. Le message sera envoyé avec
	le pseudo %s.

OPER_HELP_STATS
	Syntaxe: STATS [AKILL | ALL | RESET | MEMORY | UPLINK]
	
	Sans option, affiche le nombre d'utilisateurs et d'IRCops
	actuellement online (Services exclus), le plus grand nombre
	d'utilisateurs online depuis le démarrage des Services, et
	le nombre d'heures durant lesquelles les Services ont tourné.
	
	Avec l'option AKILL, affiche la taille actuelle de la liste
	d'AKILL et le temps d'échéance par défaut.
	
	L'option ALL est réservée uniquement aux Services admins,
	et affiche des informations sur l'utilisation de la mémoire
	par les Services. L'utilisation de cette commande peut geler
	les Services pendant un court instant sur de grands réseaux,
	donc n'en abusez pas!
	
	L'option RESET réinitialise le nombre maximum d'utilisateurs
	au nombre courant d'utilisateurs sur le réseau.

	L'option MEMORYaffiche les informations sur l'utilisation
	de la mémoire par les services. L'utilisation de cette commande
	peut paralyser les Services durant un court instant sur les gros
	réseaux; n'en abuser pas!
		
	L'option UPLINK affiche les informations sur le serveur qui est
	utilisé pour linker Anope au réseau.
	
	UPTIME est l'équivalent de STATS.

OPER_HELP_OPER
	Syntaxe: OPER ADD pseudo
	         OPER DEL {pseudo | entrée | liste}
	         OPER LIST [mask | liste]
	         OPER CLEAR
	
	Permet aux Services Root d'ajouter ou de supprimer des
	pseudo à ou de la liste des Services operators. Un utilisateur 
	dont le pseudo est dans la liste des Services operators et qui est 
	identifié auprès de %s aura accès aux commandes de Services
	operateur.
	
	La commande OPER ADD ajoute le pseudo donné à la liste
	des Services operateurs.
	
	La commande OPER DEL supprime le pseudo donné de
	la liste des Services operateurs. Si une liste de numéros 
	d'entrées est donnée, ces entrées sont supprimées. (Consultez 
	l'exemple pour LIST ci-dessous.)
	
	La commande OPER LIST affiche la liste des Services operateurs. 
	Si un mask joker est donné, seules les entrées correspondantes
	au mask sont affichées. Si une liste de numéros d'entrée
	est donnée, seules ces entrées sont affichées, par exemple:

	   OPER LIST 2-5,7-9
	        Liste toutes les entrées comprises entre 2 et 5 et
	        entre 7 et 9.
	
	La commande OPER CLEAR supprime toutes les entrées
	de la liste des Services operateurs.
	
	Un opérateur IRC peut utiliser la forme OPER LIST de 
	la commande.

OPER_HELP_ADMIN
	Syntaxe: ADMIN ADD pseudo
	         ADMIN DEL {pseudo | entrée | liste}
	         ADMIN LIST [mask | liste]
	         ADMIN CLEAR
	
	Permet au Services root d'ajouter ou de supprimer des
	pseudos à ou de la liste des Services admins. Un utilisateur 
	dont le pseudo est dans la liste des Services admins et qui est 
	identifié auprès de %s aura accès aux commandes de Services
	admin.
	
	La commande ADMIN ADD ajoute le pseudo donné à la liste
	des Services admins.
	
	La commande ADMIN DEL supprime le pseudo donné de
	la liste des Services admins. Si une liste de numéros 
	d'entrées est donnée, ces entrées sont supprimées. (Consultez 
	l'exemple pour LIST ci-dessous.)
	
	La commande ADMIN LIST affiche la liste des Services admins. 
	Si un mask joker est donné, seules les entrées correspondantes
	au mask sont affichées. Si une liste de numéros d'entrée
	est donnée, seules ces entrées sont affichées, par exemple:

	   ADMIN LIST 2-5,7-9
	        Liste toutes les entrées comprises entre 2 et 5 et
	        entre 7 et 9.
	
	La commande ADMIN CLEAR supprime toutes les entrées
	de la liste des Services admins.
	
	Un opérateur IRC peut utiliser la forme ADMIN LIST de 
	la commande. Toutes les autres utilisations sont réservées au 
	Services root.

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
	Syntaxe: MODE canal modes
	
	Permet aux Services operateurs de définir les modes d'un
	canal. Les paramètres sont les mêmes que pour la commande
	/MODE normale.
	
	Réservée aux Services operateurs.

OPER_HELP_UMODE
	Syntaxe: UMODE pseudo modes
	
	Permet aux Super Admins de changer un mode d'un utilisateur.

OPER_HELP_OLINE
	Syntaxe: OLINE pseudo flags
	
	Permet de mettre un OperFlag à un utilisateur.
	Préfixes: "+" et "-". Pour tout supprimer, spécifiez "-"

	Limité aux Super Admins.

OPER_HELP_CLEARMODES
	Syntaxe: CLEARMODES canal [ALL]
	
	Enlève tous les modes binaires (i,k,l,m,n,p,s,t) et les bans
	d'un canal. Si ALL est donné, enlève également tous les
	ops et voices (modes +o et +v) du canal.
	
	Réservée aux Services operators.

OPER_HELP_KICK
	Syntaxe: KICK canal utilisateur raison
	
	Permet aux staff de kicker un utilisateur d'un canal.
	Les paramètres sont les mêmes que pour la commande /KICK
	normale. Le message de kick sera préfixé du pseudo de
	l'IRCop ayant utilisé la commande KICK, par exemple:
	
	*** SpamMan a été kické du canal #canal par %S (Alcan (Flood))
	
	Réservée aux Services operators.

OPER_HELP_SVSNICK
	Syntaxe: SVSNICK pseudo nouveau-pseudo
	
	Force le changement de pseudo d'un pseudo.
	Limité aux Super Admins.

OPER_HELP_AKILL
	Syntaxe: AKILL ADD [+échéance] masque raison
	         AKILL DEL {masque | numéro d'entrée | liste}
	         AKILL LIST [masque | liste]
	         AKILL VIEW [masque | liste]
	         AKILL CLEAR
	
	Permet aux opérateurs des Services de manipuler la liste
	d'AKILL. Si un utilisateur correspondant à un masque d'AKILL
	tente de se connecter, les Services enverront un KILL pour
	cet utilisateur et, sur les serveurs le supportant, demandera
	à tous les serveurs d'ajouter un ban (K-line) du masque
	qui correspondait.
	
	AKILL ADD ajoute le masque user@host donné à la liste 
	d'AKILL pour la raison précisée (qui doit être donnée).
	échéance est un nombre entier suivi par un d (jours), h
	(heures), ou m (minutes). Les combinaisons (telles que
	1h30m) ne sont pas permises. Si l'unité n'est pas incluse,
	la valeur est en jours par défaut (donc +30 est équivalent
	à 30 jours). Pour ajouter une AKILL qui n'expire pas, utilisez
	+0. Si le masque utilisateur à ajouter commence par un +,
	une échéance doit être donnée, même si c'est la même que
	celle par défaut. L'échéance par défaut pour les AKILLs peut 
	être consultée par la commande STATS AKILL.
	
	La commande AKILL DEL supprime le masque donné de la liste
	d'AKILL si il existe. Si une liste de numéros d'entrées
	est donnée, ces entrées sont supprimées. (Voyez l'exemple
	pour LIST ci-dessous.)
	
	La commande AKILL LIST affiche la liste d'AKILL. Si un
	masque joker est donné, seules les entrées correspondantes
	au masque sont affichées. Si une liste de numéros d'entrées
	est donnée, seules ces entrées sont affichées, par exemple:

	   AKILL LIST 2-5,7-9
	      Liste les entrées de la liste d'AKILL numéro 2 à 5 
	      et 7 à 9.
	      
	AKILL VIEW est une version plus détaillée de AKILL LIST, 
	et affichera par qui et quand a été ajoutée une AKILL et
	sa date d'expiration, en plus du masque user@host et de la 
	raison. 
	
	AKILL CLEAR vide toutes les entrées de la liste d'AKILL.
	
	Résérvée aux opérateurs des Services.

OPER_HELP_SGLINE
	Syntaxe: SGLINE ADD [+échéance] masque:raison
	         SGLINE DEL {masque | numéro d'entrée | liste}
	         SGLINE LIST [masque | liste]
	         SGLINE VIEW [masque | liste]
	         SGLINE CLEAR
	
	Permet aux opérateurs des Services de manipuler la liste
	de SGLINEs. Si un utilisateur ayant un vrai nom correspondant 
	à un masque de SGLINE tente de se connecter, les Services ne
	lui permettront pas de continuer sa session IRC.
	
	SGLINE ADD ajoute le masque de vrai nom donné à la liste 
	de SGLINEs pour la raison précisée (qui doit être donnée).
	échéance est un nombre entier suivi par un d (jours), h
	(heures), ou m (minutes). Les combinaisons (telles que
	1h30m) ne sont pas permises. Si l'unité n'est pas incluse,
	la valeur est en jours par défaut (donc +30 est équivalent
	à 30 jours). Pour ajouter une SGLINE qui n'expire pas, utilisez
	+0. Si le masque de vrai nom à ajouter commence par un +,
	une échéance doit être donnée, même si c'est la même que
	celle par défaut. L'échéance par défaut pour les SGLINEs peut 
	être consultée par la commande STATS AKILL.
	Note: puisque le masque de vrai nom peut contenir des espaces,
	le séparateur entre lui et la raison est un deux points.
	
	La commande SGLINE DEL supprime le masque donné de la liste
	de SGLINEs si il existe. Si une liste de numéros d'entrées
	est donnée, ces entrées sont supprimées. (Voyez l'exemple
	pour LIST ci-dessous.)
	
	La commande SGLINE LIST affiche la liste des SGLINEs. Si un
	masque joker est donné, seules les entrées correspondantes
	au masque sont affichées. Si une liste de numéros d'entrées
	est donnée, seules ces entrées sont affichées, par exemple:

	   SGLINE LIST 2-5,7-9
	      Liste les entrées de la liste de SGLINEs numéro 2 à 5 
	      et 7 à 9.
	      
	SGLINE VIEW est une version plus détaillée de SGLINE LIST, 
	et affichera par qui et quand a été ajoutée une SGLINE et
	sa date d'expiration, en plus du masque de vrai nom et de la 
	raison. 
	
	SGLINE CLEAR vide toutes les entrées de la liste de SGLINEs.
	
	Résérvée aux opérateurs des Services.

OPER_HELP_SQLINE
	Syntaxe: SQLINE ADD [+échéance] masque raison
	         SQLINE DEL {masque | numéro d'entrée | liste}
	         SQLINE LIST [masque | liste]
	         SQLINE VIEW [masque | liste]
	         SQLINE CLEAR
	
	Permet aux opérateurs des Services de manipuler la liste
	de SQLINEs. Si un utilisateur ayant un nick correspondant 
	à un masque de SQLINE tente de se connecter, les Services ne
	lui permettront pas de continuer sa session IRC.
	
	Si le premier caractère du masque est un #, les services
	empêcheront l'utilisation des channels correspondants (sur
	les IRCds le supportant).
	
	SQLINE ADD ajoute le masque donné à la liste de SQLINEs pour 
	la raison précisée (qui doit être donnée).
	échéance est un nombre entier suivi par un d (jours), h
	(heures), ou m (minutes). Les combinaisons (telles que
	1h30m) ne sont pas permises. Si l'unité n'est pas incluse,
	la valeur est en jours par défaut (donc +30 est équivalent
	à 30 jours). Pour ajouter une SQLINE qui n'expire pas, utilisez
	+0. Si le masque à ajouter commence par un +,
	une échéance doit être donnée, même si c'est la même que
	celle par défaut. L'échéance par défaut pour les SQLINEs peut 
	être consultée par la commande STATS AKILL.
	
	La commande SQLINE DEL supprime le masque donné de la liste
	de SQLINEs si il existe. Si une liste de numéros d'entrées
	est donnée, ces entrées sont supprimées. (Voyez l'exemple
	pour LIST ci-dessous.)
	
	La commande SQLINE LIST affiche la liste des SQLINEs. Si un
	masque joker est donné, seules les entrées correspondantes
	au masque sont affichées. Si une liste de numéros d'entrées
	est donnée, seules ces entrées sont affichées, par exemple:

	   SQLINE LIST 2-5,7-9
	      Liste les entrées de la liste de SQLINEs numéro 2 à 5 
	      et 7 à 9.
	      
	SQLINE VIEW est une version plus détaillée de SQLINE LIST, 
	et affichera par qui et quand a été ajoutée une SQLINE et
	sa date d'expiration, en plus du masque et de la raison. 
	
	SQLINE CLEAR vide toutes les entrées de la liste de SQLINEs.
	
	Résérvée aux opérateurs des services.
	
OPER_HELP_SZLINE
	Syntaxe: SZLINE ADD [+échéance] masque raison
	         SZLINE DEL {masque | numéro d'entrée | liste}
	         SZLINE LIST [masque | liste]
	         SZLINE VIEW [masque | liste]
	         SZLINE CLEAR
	
	Permet aux opérateurs des Services de manipuler la liste
	de SZLINEs. Si un utilisateur ayant une IP correspondante 
	à un masque de SZLINE tente de se connecter, les Services ne
	lui permettront pas de continuer sa session IRC (et ce,
	même si l'IP a un PTR RR).
	
	SZLINE ADD ajoute le masque d'IP donné à la liste 
	de SZLINEs pour la raison précisée (qui doit être donnée).
	échéance est un nombre entier suivi par un d (jours), h
	(heures), ou m (minutes). Les combinaisons (telles que
	1h30m) ne sont pas permises. Si l'unité n'est pas incluse,
	la valeur est en jours par défaut (donc +30 est équivalent
	à 30 jours). Pour ajouter une SZLINE qui n'expire pas, utilisez
	+0. Si le masque de vrai nom à ajouter commence par un +,
	une échéance doit être donnée, même si c'est la même que
	celle par défaut. L'échéance par défaut pour les SZLINEs peut 
	être consultée par la commande STATS AKILL.
	
	La commande SZLINE DEL supprime le masque donné de la liste
	de SZLINEs si il existe. Si une liste de numéros d'entrées
	est donnée, ces entrées sont supprimées. (Voyez l'exemple
	pour LIST ci-dessous.)
	
	La commande SZLINE LIST affiche la liste des SZLINEs. Si un
	masque joker est donné, seules les entrées correspondantes
	au masque sont affichées. Si une liste de numéros d'entrées
	est donnée, seules ces entrées sont affichées, par exemple:

	   SZLINE LIST 2-5,7-9
	      Liste les entrées de la liste de SZLINEs numéro 2 à 5 
	      et 7 à 9.
	      
	SZLINE VIEW est une version plus détaillée de SZLINE LIST, 
	et affichera par qui et quand a été ajoutée une SZLINE et
	sa date d'expiration, en plus du masque d'IP et de la 
	raison.
	
	SZLINE CLEAR vide toutes les entrées de la liste de SZLINEs.
	
	Résérvée aux opérateurs des Services.

OPER_HELP_SET
	Syntaxe: SET option valeur
	
	Contrôle différentes options globales aux Servies. 
	Les noms des options actuelles sont:
	
	    READONLY   Active ou désactive le mode lecture seule.
	    LOGCHAN    Report des logs sur un canal
	    DEBUG      Active ou désactive le mode de déboguage
	    NOEXPIRE   Active ou désactive le mode sans expiration
	    SUPERADMIN Active ou désactive le mode super-admin
	    SQL   	   Active ou désactive le mode SQL
	    IGNORE     Active ou désactive le mode ignore
	    LIST       Liste les options
	
	Réservée aux Services admins.

OPER_HELP_SET_READONLY
	Syntaxe: SET READONLY {ON | OFF}
	
	Active ou désactive le mode lecture seule. Dans le mode 
	lecture-seule, les utilisateurs normaux ne seront pas 
	autorisés à modifier les données des Services, comprenant
	les access lists des canaux et pseudos, etc. Les IRCops
	ayant suffisamment de droits sur les Services pourront 
	modifier l'AKILL list des services et effacer ou interdire
	des pseudos et canaux, mais de tels changements ne seront
	pas sauvegardés à moins que le mode lecture seule ne soit
	désactivé avant que les Services ne soient arrêtés ou
	relancés.
	
	Cette option est équivalente à l'option de la ligne de commande
	-readonly.

OPER_HELP_SET_LOGCHAN
	Syntaxe: SET LOGCHAN {ON | OFF}

	Permet d'envoyer les logs sur un canal en plus d'écrire dans le
	fichier de log. LogChannel doit être défini dans le fichier
	de configuration des services.
	
	Cette option est équivalente à l'option de la ligne de commande
	-logchan.
	
	Note: En mettant cette option sur OFF vous renforcerez la sécurité...

OPER_HELP_SET_DEBUG
	Syntaxe: SET DEBUG {ON | OFF | nombre}
	
	Active ou désactive le debug mode. Dans le debug mode, toutes
	les données envoyées vers et par les Services ainsi qu'un bon
	nombre d'autres messages de déboguage sont écrits dans le 
	fichier de log. Si nombre est donné, le debug mode est activé,
	avec le niveau de déboguage à nombre.
	
	Cette option est équivalente à l'option de la ligne de commande
	-debug.
	
OPER_HELP_SET_NOEXPIRE
	Syntaxe: SET NOEXPIRE {ON | OFF}
	
	Active ou désactive le mode sans expiration. Dans le mode 
	sans expiration, les pseudos, canaux, akills et exceptions 
	n'expireront pas jusqu'à ce que l'option soit désactivée.
	
	Cette option est équivalente à l'option de la ligne de commande
	-noexpire.

OPER_HELP_SET_SUPERADMIN
	Syntax: SET SUPERADMIN {ON | OFF}

	Vous permet d'avoir tous les privilèges comme le droit d'être reconnu
	"founder" de toutes les canaux...

	Utilisation temporaire.

OPER_HELP_SET_SQL
	Syntaxe: SET SQL {ON | OFF}

	Activer cette option fera utiliser SQL a anope, cette option est 
	utilisé pour activé ou désactiver mysql lorsque votre serveur est
	down et que les services sont en cour de fonctionnement.
	
OPER_HELP_SET_IGNORE
	Syntaxe: SET IGNORE {ON | OFF}

	Activer ou désactivé cette option pour l'utilisation de l'ignore.

OPER_HELP_SET_LIST
	Syntaxe: SET LIST

	Afficher diverse option à propos de %S

OPER_HELP_NOOP
	Syntaxe: NOOP SET serveur
	         NOOP REVOKE serveur
	
	NOOP SET supprime toutes les O:lines du serveur
	donné et déconnecte tous les IRCops qui sont dessus
	actuellement pour les empêcher de rehasher le serveur
	(car cela annulerait les effets).
	
	NOOP REVOKE remet en place toutes les O:lines 
	supprimées du serveur donné.
	
	Note: Le paramètre serveur n'est en aucun cas 
	vérifié par les Services.

	Réservée aux administrateurs des Services.

OPER_HELP_JUPE
	Syntaxe: JUPE serveur [raison]
	
	Demande aux Services de juper un serveur -- c'est à dire,
	créer un faux serveur connecté aux Services ce qui empêche
	le vrai serveur ayant ce nom de se connecter. Le jupe peut
	être enlevé en utilisant un SQUIT normal. Si une raison
	est donnée, elle est placée dans le champ d'information du
	serveur, sinon, l'information du serveur contiendra le 
	texte "Juped by <pseudo>", laissant paraître le pseudo
	de la personne qui a jupé le serveur.
	
	Réservée aux Services admins.

OPER_HELP_RAW
	Syntaxe: RAW texte
	
	Envoie une chaîne de texte directement au serveur sur 
	lequel les Services sont connectés. Cette commande a un
	champ d'utilisation très limité, et peut causer des
	troubles dans un réseau en cas de mauvaise utilisation.
	N'UTILISEZ PAS CETTE COMMANDE à moins d'être absolument
	sûr de ce que vous faites!
	
	Réservée aux Services admins.

OPER_HELP_UPDATE
	Syntaxe: UPDATE
	
	Provoque la mise à jour de toutes les bases de données dès
	que vous le demandez.
	
	Réservée aux Services admins.
	
OPER_HELP_RELOAD
	Syntaxe: RELOAD
	
	Provoque le rechargement du fichier de configuration des
	Services. Notez que certaines directives demandent tout
	de même le redémarrage des Services pour prendre effet
	(comme le nom des services, l'activation de la limite
	de sessions, etc.)
	
	Réservée aux Services admins.

OPER_HELP_QUIT
	Syntaxe: QUIT
	
	Provoque un arrêt immédiat des services, les bases de données
	ne sont pas sauvegardées. Cette commande ne devrait pas être
	utilisée à moins que des dommages dans la copie en mémoire des
	bases de données ne soient soupçonnés. Pour des arrêts normaux,
	utilisez la commande SHUTDOWN.
	
	Réservée aux Services admins.

OPER_HELP_SHUTDOWN
	Syntaxe: SHUTDOWN
	
	Provoque la sauvegarde de toutes les bases de données puis
	l'arrêt des services.
	
	Réservée aux Services admins.

OPER_HELP_RESTART
	Syntaxe: RESTART
	
	Provoque la sauvegarde de toutes les bases de données et
	le redémarrage des Services (c'est à dire qu'il arrête
	le programme et le relance immédiatement).
	
	Réservée aux Services admins.

OPER_HELP_CHANLIST
	Syntaxe: CHANLIST [{modèle | pseudo} [SECRET]]
	
	Liste tous les channels actuellement utilisés sur le réseau IRC, qu'ils
	soient enregistrés ou non.
	
	Si modèle est donné, seuls les canaux correspondants sont listés. Si
	un pseudo est donné, les canaux sur lesquels est l'utilisateur ayant
	ce pseudo seront listés. Si SECRET est spécifié, liste seulement les canaux 
	correspondants à modèle qui ont le mode +s ou +p.
	
	Réservée aux Services admins.
	
OPER_HELP_USERLIST
	Syntaxe: USERLIST [{modèle | canal} [INVISIBLE]]
	
	Liste tous les utilisateurs connectés sur le réseau IRC, enregistrés
	ou non.
	
	Si modèle est donné (format nick!user@host), seuls les utilisateurs 
	correspondants seront listés. Si canal est donné, seuls les 
	utilisateurs du canal seront listés. +i INVISIBLE est spécifié,
	seuls les utilisateurs avec le mode +i seront listés.
	
	Réservée aux Services admins.

OPER_HELP_MODLOAD
	Syntaxe: MODLOAD Fichier

	Cette commande charge le module depuis le fichier situé dans le repertoire
	des modules.

	Limité aux Services Roots.

OPER_HELP_MODUNLOAD
	Syntaxe: MODUNLOAD Fichier

	Cette commande décharge le module depuis le fichier situé dans le repertoire
	des modules.

	Limité aux Services Roots.

OPER_HELP_MODINFO
	Syntaxe: MODINFO Fichier

	Cette commande donne des informations sur un module chargé.

	Limité aux Services Roots.

OPER_HELP_MODLIST
	Syntaxe: MODLIST [Core|3rd|protocol|encryption|supported|qatested]

	Liste les modules actuelement chargés.

	Limité aux Services Roots.


###########################################################################
#
# BotServ help messages
#
###########################################################################

BOT_HELP_CMD_BOTLIST
	BOTLIST            Liste les bots disponibles
BOT_HELP_CMD_ASSIGN
	ASSIGN             Attribue un bot à un canal
BOT_HELP_CMD_SET
	SET                Configure les options du bot
BOT_HELP_CMD_KICK
	KICK               Configure les kickers
BOT_HELP_CMD_BADWORDS
	BADWORDS           Gère la liste des mots interdits
BOT_HELP_CMD_ACT
	ACT                Fait faire au bot l'équivalent de la commande /me
BOT_HELP_CMD_INFO
	INFO               Vous permet de voir des informations a propos de BotServ sur
	                   un canal ou un bot
BOT_HELP_CMD_SAY
	SAY                Fait dire au bot le texte donné sur un canal
BOT_HELP_CMD_UNASSIGN
	UNASSIGN           Retire le bot d'un canal
BOT_HELP_CMD_BOT
	BOT                Gère la liste des bots du réseau

BOT_HELP
	%S vous permet d'avoir un bot sur votre canal.
	Il a été conçu pour les utilisateurs qui ne 
	peuvent héberger ou configurer un bot, ou pour
	être utilisé sur les réseaux qui n'autorisent pas
	un bot d'un utilisateur. Les commandes disponibles
	sont listées ci-dessous, pour les utiliser, tapez
	%R%S command. Pour plus d'information sur
	une commande spécifique, tapez %R%S HELP command.
	
BOT_HELP_FOOTER
	Le bot joindra un canal dès qu'il y aura au moins
	%d utilisateur(s) dessus.

BOT_HELP_BOTLIST
	Syntaxe: BOTLIST
	
	Liste tous les bots disponibles sur ce réseau.
	
BOT_HELP_ASSIGN
	Syntaxe: ASSIGN canal pseudo
	
	Attribue le bot désigné par pseudo au canal. Vous
	pouvez ensuite configurer le bot pour qu'il convienne à
	vos besoins.
	
BOT_HELP_UNASSIGN
	Syntaxe: UNASSIGN canal
	
	Retire le bot d'un canal. Si vous utilisez cette commande,
	le bot ne joindra plus le canal. Cependant, la configuration
	du bot est conservée donc vous aurez toujours la possibilité
	de réattribuer un bot plus tard sans avoir à le reconfigurer
	entièrement.
	
BOT_HELP_INFO
	Syntax: INFO {canal | pseudo}
	
	Vous permet de voir les informations %S pour un canal
	ou un bot. Si le paramètre est un canal, vous verrez
	des informations sur le canal tels que les kickers
	actifs. Si le paramètre est un pseudo, vous verrez des
	informations à propos d'un bot, comme le temps de
	création ou le nombre de canaux auxquels il a été
	attribué.
	
BOT_HELP_SET
	Syntaxe: SET canal option paramètres
	
	Configure les options du bot.  option peut être:
	
	    DONTKICKOPS      Pour protéger les ops des kicks du bot
	    DONTKICKVOICES   Pour protéger les voices des kicks du bot
	    FANTASY          Active les commandes fantaisistes
	    GREET            Active les messages d'accueil
	    SYMBIOSIS        Permet au bot d'agir comme un vrai bot
	
	Tapez %R%S HELP SET option pour plus d'infos
	sur une option spécifique.
	
BOT_HELP_SET_DONTKICKOPS
	Syntaxe: SET canal DONTKICKOPS {ON|OFF}
	
	Active ou désactive la protection des ops sur un 
	canal. Quand elle est active, cette option empêche
	le bot de kicker les ops même s'ils ne sont pas concernés
	par le level NOKICK.
	
BOT_HELP_SET_DONTKICKVOICES
	Syntaxe: SET canal DONTKICKVOICES {ON|OFF}
	
	Active ou désactive la protection des voices sur un 
	canal. Quand elle est active, cette option empêche
	le bot de kicker les voices même s'ils ne sont pas concernés
	par le level NOKICK.
	
BOT_HELP_SET_FANTASY
	Syntaxe: SET canal FANTASY {ON|OFF}
	
	Active ou désactive le mode fantaisie sur un
	canal. Quand elle est active, cette option
	permet aux utilisateurs d'utiliser les commandes
	!op, !deop, !voice, !devoice, !kick, !kb, !unban 
	et !seen sur un canal (trouvez comment les utiliser; 
	essayez avec ou sans pseudo pour chacune, et avec 
	une raison pour certaines?).
	
	Notez que les utilisateurs souhaitant utiliser
	les commandes fantaisistes DOIVENT avoir
	suffisamment d'accès pour les levels FANTASIA
	et un autre level qui dépend de la commande
	(par exemple, pour utiliser !op, un utilisateur
	doit avoir suffisamment d'accès pour le level
	OPDEOP).
	
BOT_HELP_SET_GREET
	Syntaxe: SET canal GREET {ON|OFF}
	
	Active ou désactive le mode message d'accueil
	sur un canal. Quand elle est active, cette option
	fait afficher au bot le message d'accueil de chaque
	utilisateur qui a un accès suffisant au canal.
	
BOT_HELP_SET_SYMBIOSIS
	Syntax: SET canal SYMBIOSIS {ON|OFF}
	
	Active ou désactive la symbiose sur un canal.
	Quand elle est active, cette option fait faire
	au bot tout ce qui est normalement effectué
	par %s sur les canaux, tels que les MODEs,
	les KICKs, et même le message d'entrée.
	
BOT_HELP_KICK
	Syntaxe: KICK canal option paramètres
	
	Configure les kickers du bot.  option peut etre:
	
	    BOLDS         Détermine si le bot kicke les
	                     caractères gras
	    BADWORDS      Détermine si le bot kicke les mots 
	                     interdits
	    CAPS          Détermine si le bot kicke les majuscules
	    COLORS        Détermine si le bot kicke les couleurs
	    FLOOD         Détermine si le bot kicke les utilisateurs
	                     qui floodent
	    REPEAT        Détermine si le bot kicke les répétitions
	    REVERSES      Détermine si le bot kicke les reverses
	    UNDERLINES    Détermine si le bot kicke les caractères
	                     soulignés
	
	Tapez %R%S HELP KICK option pour plus d'informations
	sur une option en particulier.

BOT_HELP_KICK_BOLDS
	Syntaxe: KICK canal BOLDS {ON|OFF} [ttb]

	Active ou désactive le kicker de caractères gras. Quand
	elle est active, cette option demande au bot
	de kicker les utilisateurs qui utilisent des 
	caractères gras.

	ttb est le nombre de fois qu'un utilisateur peut
	se faire kicker avant d'être banni. Ne donnez
	pas ttb ou mettez le à 0 pour désactiver le système
	de ban une fois activé.

BOT_HELP_KICK_COLORS
	Syntaxe: KICK canal COLORS {ON|OFF} [ttb]

	Active ou désactive le kicker de couleurs. Quand
	elle est active, cette option demande au bot
	de kicker les utilisateurs qui utilisent des 
	couleurs.

	ttb est le nombre de fois qu'un utilisateur peut
	se faire kicker avant d'être banni. Ne donnez
	pas ttb ou mettez le à 0 pour désactiver le système
	de ban une fois activé.

BOT_HELP_KICK_REVERSES
	Syntaxe: KICK canal REVERSES {ON|OFF} [ttb]

	Active ou désactive le kicker de reverses. Quand
	elle est active, cette option demande au bot
	de kicker les utilisateurs qui utilisent des 
	reverses.

	ttb est le nombre de fois qu'un utilisateur peut
	se faire kicker avant d'être banni. Ne donnez
	pas ttb ou mettez le à 0 pour désactiver le système
	de ban une fois activé.

BOT_HELP_KICK_UNDERLINES
	Syntaxe: KICK canal UNDERLINES {ON|OFF} [ttb]

	Active ou désactive le kicker de caractères soulignés. 
	Quand elle est active, cette option demande au bot
	de kicker les utilisateurs qui utilisent des 
	caractères soulignés.

	ttb est le nombre de fois qu'un utilisateur peut
	se faire kicker avant d'être banni. Ne donnez
	pas ttb ou mettez le à 0 pour désactiver le système
	de ban une fois activé.
	
BOT_HELP_KICK_CAPS
	Syntaxe: KICK canal CAPS {ON|OFF} [ttb [min [percent]]]

	Active ou désactive le kicker de majuscules. Quand 
	elle est active, cette option demande au bot
	de kicker les utilisateurs qui parlent en MAJUSCULES
	sur le canal.

	Le bot ne kicke que s'il y a au minimum min majuscules
	et qu'elles constituent au moins percent%% de la
	ligne de texte entière (si ces paramètres ne sont pas
	donnés, les valeurs par défaut 10 caractères et 25%% 
	seront utilisés).

	ttb est le nombre de fois qu'un utilisateur peut
	se faire kicker avant d'être banni. Ne donnez
	pas ttb ou mettez le à 0 pour désactiver le système
	de ban une fois activé.

BOT_HELP_KICK_FLOOD
	Syntaxe: KICK canal FLOOD {ON|OFF} [ttb [ln [secs]]]

	Active ou désactive le kicker de flood. Quand 
	elle est active, cette option demande au bot
	de kicker les utilisateurs qui floodent le canal
	en utilisant au moins ln lignes en secs secondes
	(si ces paramètres ne sont pas donnés, les valeurs
	par défaut sont 6 lignes en 10 secondes).
	
	ttb est le nombre de fois qu'un utilisateur peut
	se faire kicker avant d'être banni. Ne donnez
	pas ttb ou mettez le à 0 pour désactiver le système
	de ban une fois activé.

BOT_HELP_KICK_REPEAT
	Syntaxe: KICK #canal REPEAT {ON|OFF} [ttb [num]]

	Active ou désactive le kicker de répétitions. Quand 
	elle est active, cette option demande au bot
	de kicker les utilisateurs qui se répètent num fois
	(si num n'est pas donné, la valeur par défaut est 3).

	ttb est le nombre de fois qu'un utilisateur peut
	se faire kicker avant d'être banni. Ne donnez
	pas ttb ou mettez le à 0 pour désactiver le système
	de ban une fois activé.
	
BOT_HELP_KICK_BADWORDS
	Syntaxe: KICK #canal BADWORDS {ON|OFF} [ttb]

	Active ou désactive le kicker de mots interdits. 
	Quand elle est active, cette option demande au bot
	de kicker les utilisateurs qui disent certains mots 
	sur le canal.

	Vous pouvez définir les mots interdits pour votre
	channel en utilisant la commande BADWORDS. Tapez
	%R%S HELP BADWORDS pour plus d'informations.

	ttb est le nombre de fois qu'un utilisateur peut
	se faire kicker avant d'être banni. Ne donnez
	pas ttb ou mettez le à 0 pour désactiver le système
	de ban une fois activé.

BOT_HELP_BADWORDS
	Syntaxe: BADWORDS canal ADD mot [SINGLE | START | END]
	         BADWORDS canal DEL {mot | entry-num | liste}
	         BADWORDS canal LIST [mask | liste]
	         BADWORDS canal CLEAR
	
	Gère la liste des mots interdits d'un canal. La liste
	des mots interdits détermine quels mots doivent être kickés
	quand le kicker de mots interdits est activé. Pour plus
	d'information, tapez %R%S HELP KICK BADWORDS.
	
	La commande BADWORDS ADD ajoute le mot donné à la liste
	de mots interdits. Si SINGLE est donné, un kick ne sera
	effectué que si un utilisateur dit le mot en entier. Si
	START est donné, un kick ne sera effectué que si un
	utilisateur dit un mot qui commande par mot. Si END
	est spécifié, un kick ne sera effectué que si un utilisateur
	dit un mot qui se termine par mot. Si vous ne spécifiez
	rien, un kick sera effectué chaque fois que mot est dit
	par un utilisateur.
	
	La commande BADWORDS DEL supprime le mot donné de la liste
	des mots interdits. Si une liste de numéro d'entrées est
	donné, ces entrées seront supprimées. (Voyez l'exemple pour
	LIST ci-dessous.)
	
	La commande BADWORDS LIST affiche la liste des mots 
	interdits. Si un mask joker est donné, seules les entrées
	correspondantes au mask seront affichées. Si une liste
	de numéro d'entrées est donnée, seules ces entrées seront
	affichées, par exemple:

	   BADWORDS #canal LIST 2-5,7-9
	      Liste les mots interdits 2 à 5 et 7 à 9.
	      
	La commande BADWORDS CLEAR supprime toutes les entrées
	de la liste des mots interdits.

BOT_HELP_SAY
	Syntaxe: SAY canal texte
	
	Fait dire au bot le texte donné sur le canal donné.

BOT_HELP_ACT
	Syntaxe: ACT canal texte
	
	Fait faire au bot l'équivalent d'une commande "/me"
	sur le canal donné avec le texte donné.

BOT_SERVADMIN_HELP_BOT
	Syntax: BOT ADD pseudo user host real
	        BOT CHANGE ancien-pseudo nouveau-pseudo [user [host [real]]]
	        BOT DEL pseudo
	
	Permet aux Services admins de créer, modifier, et
	supprimer les bots que les utilisateurs pourront
	utiliser sur leurs canaux.
	
	BOT ADD ajoute un bot avec le pseudo, username, hostname
	et realname donné. Aucun test d'intégrité ne sont effectués
	pour ces données, donc faites très attention.
	BOT CHANGE permet de changer le pseudo, username, hostname
	ou realname d'un bot tout en conservant les données qui
	lui sont associées.
	BOT DEL supprime le bot donné de la liste des bots.  
	
	Note: si vous créez un bot ayant un pseudo enregistré
	avec NickServ, il sera effacé. De plus, si un utilisateur
	utilise le pseudo du bot lors de sa création ou modification,
	il sera déconnecté.
	
BOT_SERVADMIN_HELP_SET
	
	Ces options sont réservées aux Services admins:
	
	    NOBOT            Empêche un bot d'être attribué à
	                         un canal
	    PRIVATE          Empêche un bot d'être attribué par
	                         des non-opérateurs IRC

BOT_SERVADMIN_HELP_SET_NOBOT
	Syntaxe: SET canal NOBOT {ON|OFF}
	
	Cette option empêche un bot d'être attribué au canal.
	Si un bot est déjà attribué, il est enlevé automatiquement
	quand vous activez l'option.
	
	Réservée aux administrateurs des services.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Syntaxe: SET nick-du-bot PRIVATE {ON|OFF}
	
	Cette option empêche un bot d'être attribué à un
	canal par des utilisateurs qui ne sont pas opérateurs
	IRC.
	
	Réservée aux administrateurs des services.
HOST_EMPTY
	Liste des vhost vide.
HOST_ENTRY
	#%d Pseudo:%s, vhost:%s (%s - %s)
HOST_IDENT_ENTRY
	#%d Pseudo:%s, vhost:%s@%s (%s - %s)
HOST_SET
	Le vhost pour %s enregistrée en %s.
HOST_IDENT_SET
	Le vhost pour %s enregistrée en %s@%s.
HOST_SETALL
	Le vhost pour le groupe %s enregistrée en %s.
HOST_DELALL
	Le vhosts pour le groupe %s a été supprimé.
HOST_DELALL_SYNTAX
	Syntaxe: DELALL <pseudo>.
HOST_IDENT_SETALL
	Le vhost pour le groupe %s enregistrée en %s@%s.
HOST_SET_ERROR
	Erreur! Une vhost peut seulement contenir les caratères A-Z, a-z, 0-9, '.' et '-'.
HOST_SET_IDENT_ERROR
	L'identd du vhost dois etre dans un format d'identd valide
HOST_SET_TOOLONG
	Erreur! Le vhost est top longue, veuillez entrer moins de %d caratères.
HOST_SET_IDENTTOOLONG
	Erreur! l'Ident est trop long, veuillez entrer moins de %d caratères.
HOST_NOREG
	L'utilisateur %s n'a pas été trouvé dans la base de données de NickServ.
HOST_SET_SYNTAX
	Syntaxe %R%s set <pseudo> <hostmask>.
HOST_SETALL_SYNTAX
	Syntaxe %R%s setall <pseudo> <hostmask>.
HOST_DENIED
	Accés refusé.
HOST_NOT_ASSIGNED
	Contactez un opérateur afin d'avoir une vhost sous ce pseudo.
HOST_ACTIVATED
	Votre vhost %s est activée.
HOST_IDENT_ACTIVATED
	Votre vhost %s@%s est activée.
HOST_ID
	Veuillez vous identifier avant d'utiliser cette commande.
HOST_NOT_REGED
	Vous devez être enregistré avant d'utiliser cette commande.
HOST_DEL
	Le vhost pour %s a été supprimé.
HOST_DEL_SYNTAX
	Syntaxe: %R%s del <pseudo>.

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_OFF_UNREAL
	Votre vhost a été enlevée. Pour réactiver la protection de votre IP, tapez /mode %s +%s
HOST_NO_VIDENT
	L'IRCD utilisé ne support par la modification de l'ident.

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_GROUP
	Toutes les vhost du groupe %s ont été enregistrées en %s
HOST_IDENT_GROUP
	Toutes les vhost du groupe %s ont été enregistrées en %s@%s

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_LIST_FOOTER
	Affiche tout les enregistrement (Nb: %d) 
HOST_LIST_RANGE_FOOTER
	Affiche les enregistrement de %d à %d
HOST_LIST_KEY_FOOTER
	Affiche les enregistrement contenant %s (Nb: %d)

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_HELP_CMD_ON
	    ON          Active une vhost
HOST_HELP_CMD_OFF
	    OFF         Désactive une vhost
HOST_HELP_CMD_GROUP
	    GROUP       Synchronise toutes les vhosts d'un group
HOST_HELP_CMD_SET
	    SET         Configure la vhost d'un pseudo
HOST_HELP_CMD_SETALL
	    SETALL      Configure la vhost pour tous les pseudos d'un groupe
HOST_HELP_CMD_DEL
	    DEL         Supprime la vhost d'un pseudo
HOST_HELP_CMD_DELALL
	    DELALL      Supprime la vhost pour tout les pseudo d'un groupe
HOST_HELP_CMD_LIST
	    LIST        Recherche une vhost
HOST_OPER_HELP
	not used.

HOST_ADMIN_HELP
	not used.

HOST_HELP
	%S commandes:

HOST_HELP_ON
	Syntaxe: ON

	Active la vhost actuellement configurée pour ce pseudo.
	Vous verrez cette vhost dans votre /whois à la place de
	votre host actuelle.

HOST_HELP_SET
	Syntaxe: SET <pseudo> <hostmask>.

	Configure la vhost donnée pour le pseudo indiqué.
	Si l'ircd supporte la modification de l'ident, utilisez
	SET <nick> <ident>@<hostmask>

	Limité aux ServicesOpérateurs.

HOST_HELP_DELALL
	Syntaxe: DELALL <pseudo>.

	Supprime tout les vhosts pour tout les pseudos du meme
	groupe que le pseudo donné.

	Limité aux Services Oper.


HOST_HELP_SETALL
	Syntaxe: SETALL <pseudo> <hostmask>.

	Configure la vhost pour toutes les pseudos d'un même
	groupe.	Si l'ircd supporte la modification de l'ident, utilisez
	SET <pseudo> <ident>@<hostmask>

	Limité aux ServicesOpérateurs.

HOST_HELP_OFF
	Syntaxe: OFF

	Désactive la vhost que vous avez actuellement. L'utilisation
	de cette commande rendra visible votre IP.

HOST_HELP_DEL
	Syntaxe: DEL <pseudo>

	Supprime la vhost pour le pseudo indiqué.

	Limité aux ServicesOpérateurs.

HOST_HELP_LIST
	Syntaxe: LIST [<clé>|<#X-Y>]

	Cette commande liste tout les vhosts enregistré, si une clé
	est spécifié, seulement les résultats contenant le pseudo ou
	le vhosts seront afficher, par exemple : Rob* pour toute les
	entrée contenant "Rob"

	Si le style #X-Y est utilisé, seul les entrée entre X et Y
	seront affiché, par exemple: #1-3 affichera les 3 premières
	entrée.
	
	La liste utilise la valeur de NSListMax pour une limite hard
	du nombre de résultats affichés en une fois.

	Limité aux Services operateurs.

HOST_HELP_GROUP
	Syntaxe: GROUP

	Permet d'enregistrer la vHost du pseudo actuellement
	utilisé pour tous les pseudos du groupe. 

OPER_SVSNICK_UNSUPPORTED
	Désolé, SVSNICK n'est pas accéssibe sur ce réseau.

OPER_SQLINE_UNSUPPORTED
	Désolé, SQLINE n'est pas accéssible sur ce réseau.

OPER_SVSO_UNSUPPORTED
	Désolé, OLINE n'est pas accéssible sur ce réseau.

OPER_UMODE_UNSUPPORTED
	Désolé, UMODE n'est pas accéssible sur ce réseau.

OPER_SUPER_ADMIN_NOT_ENABLED
	L'option SuperAdmin n'est pas activé dans le services.conf

