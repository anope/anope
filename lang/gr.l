# Greek language file.
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
# For example, "Espaρol (Spanish)" or "Franηais (French)".

LANG_NAME
	Ελληνικά (Greek)

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
	Κυρ
	Δευτ
	Τρίτ
	Τετά
	Πέμπ
	Παρα
	Σάββ
# %A
STRFTIME_DAYS_LONG
	Κυριακή
	Δευτέρα
	Τρίτη
	Τετάρτη
	Πέμπτη
	Παρασκευή
	Σάββατο
# %b
STRFTIME_MONTHS_SHORT
	Ιαν
	Φεβ
	Μαρ
	Απρ
	Μαϊ
	Ιουν
	Ιουλ
	Αυγ
	Σεπ
	Οκτ
	Νοε
	Δεκ
# %B
STRFTIME_MONTHS_LONG
	Ιανουάριος
	Φεβρουάριος
	Μάρτιος
	Απρίλιος
	Μάιος
	Ιούνιος
	Ιούλιος
	Αυγουστος
	Σεπτέμβριος
	Οκτώβριος
	Νοέμβριος
	Δεκέμβριος


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	, 


# Various error messages.
USER_RECORD_NOT_FOUND
	Εσωτερικό λάθος - αδύνατη πρόσβαση στη παράκληση.
UNKNOWN_COMMAND
	’γνωστη εντολή %s.
UNKNOWN_COMMAND_HELP
	’γνωστη εντολή %s.  "%R%s HELP" για βοήθεια.
SYNTAX_ERROR
	Σύνταξη εντολής: %s
MORE_INFO
	%R%s HELP %s για περισσότερες πληροφορίες.
NO_HELP_AVAILABLE
	Δεν υπάρχει διαθέσιμη βοήθεια για %s.
OBSOLETE_COMMAND
	Αυτή η εντολή έχει αλλάξει; χρησιμοποιήστε %s αντί για αυτό.

BAD_USERHOST_MASK
	Η μάσκα πρέπει να είναι της μορφής user@host.
BAD_EXPIRY_TIME
	Λανθασμένος χρόνος λήξης.
USERHOST_MASK_TOO_WIDE
	%s η μάσκα είναι πολύ πλατιά; Παρακαλώ χρησιμοποιήστε μια πιο καθορισμένη μάσκα.

SERVICE_OFFLINE
	Ο %s είναι για την ώρα off.
READ_ONLY_MODE
	Σημείωση: Οι υπηρεσίες είναι μόνο αναγνώσιμες; οι όποιες αλλαγές έγιναν θα χαθούν!
PASSWORD_INCORRECT
	Λάθος κωδικός.
INVALID_TARGET
	"/msg %s" is no longer supported.  Use "/msg %s@%s" or "/%s" instead.

# What's the difference between "Access denied" and "Permission denied"?
# Very little; you can safely make them the same message with no real loss
# of meaning.  If you want to make a distinction, "Access denied" is
# usually used more generally; for example, a non-oper trying to access
# OperServ gets "Access denied", while a non-Services admin trying to use
# NickServ SET NOEXPIRE gets "Permission denied".
ACCESS_DENIED
	Δεν έχεις την επιτρεπόμενη άδεια.
PERMISSION_DENIED
	Δεν έχεις άδεια.
RAW_DISABLED
	The RAW option has been disabled. If you must use it, enable the DisableRaw directive in Services configuration.

MORE_OBSCURE_PASSWORD
	Παρακαλώ προσπάθησε ξανά με έναν πιο πολύπλοκο κωδικό. Οι κωδικοί πρέπει να είναι τουλάχιστον έως πέντε χαρακτήρες, να μην είναι εύκολοι να μαντευτούν (π.χ. το πραγματικό ονομά σου ή το ψευδώνυμο σου), και να μην περιέχει κενό ή διάφοπους άλλους χαρακτήρες.
PASSWORD_TOO_LONG
	Your password is too long. Please try again with a shorter password.

NICK_NOT_REGISTERED
	Αυτό το ψευδώνυμο δεν έχει κατοχυρωθεί.
NICK_NOT_REGISTERED_HELP
	Αυτό το ψευδώνυμο δεν έχει κατοχυρωθεί.  Γράψε %R%s HELP για περισσότερες πληροφορίες σχετικά για την κατοχύρωση ψευδωνύμου..
NICK_X_IS_SERVICES
	Nick %s is part of this Network's Services.
NICK_X_NOT_REGISTERED
	Αυτό το ψευδώνυμο %s δεν είναι κατοχυρωμένο.
NICK_X_IN_USE
	Nick %s is currently in use.
NICK_X_NOT_IN_USE
	Αυτό το ψευδώνυμο %s δεν χρησιμοποιείτε αυτή τη στιγμή.
NICK_X_NOT_ON_CHAN
	%s δεν είναι αυτή τη στιγμή μέσα στο κανάλι %s.
NICK_X_FORBIDDEN
	Αυτό το ψευδώνυμο %s δεν μπορεί να κατοχυρωθεί ή να χρησιμοποιηθεί.
NICK_X_FORBIDDEN_OPER
	Αυτό το ψευδώνυμο %s έχει παρακρατηθεί από τον %s:
	%s
NICK_X_ILLEGAL
	Nick %s is an illegal nickname and cannot be used.
NICK_X_TRUNCATED
	Nick %s was truncated to %d characters.
NICK_X_SUSPENDED
	Nick %s is currently suspended.
CHAN_X_NOT_REGISTERED
	Αυτό το κανάλι %s δεν έχει κατοχυρωθεί.
CHAN_X_NOT_IN_USE
	Αυτό το κανάλι %s δεν υπάρχει.
CHAN_X_FORBIDDEN
	Αυτό το κανάλι %s δεν μπορεί να κατοχυρωθεί ή να χρησιμοποιηθεί.
CHAN_X_FORBIDDEN_OPER
	Αυτό το κανάλι %s έχει παρακρατηθεί από τον %s:
	%s
CHAN_X_SUSPENDED
	      Suspended: [%s] %s
NICK_IDENTIFY_REQUIRED
	Ο κωδικός αναγνώρισης απαιτείται για αυτή την εντολή.
	Ξαναπροσπαθήστε αφού γράψετε %R%s IDENTIFY ^_κωδικός^_.
CHAN_IDENTIFY_REQUIRED
	Ο κωδικός αναγνώρισης απαιτείται για αυτή την εντολή.
	Ξαναπροσπαθήστε αφού γράψετε %R%s IDENTIFY %s ^_κωδικός^_.
	
MAIL_DISABLED
	Οι υπηρεσίες έχουν ρυθμιστεί να μην στέλνουν μήνυμα στη διεύθυνση.
MAIL_INVALID
	Η διεύθυνση για %s είναι λανθασμένη.
MAIL_X_INVALID
	%s δεν είναι λανθασμένη διεύθυνση.
MAIL_LATER
	Δεν μπορεί να σταλεί στη διεύθυνση το μήνυμα για την ώρα; παρακαλώ προσπάθησε λίγο αργότερα.
MAIL_DELAYED
	Παρακαλώ περίμενε ^B%d^B δευτερόλεπτα και ξαναπροσπάθησε.
	
NO_REASON
	Χωρίς λόγος
UNKNOWN
	<άγνωστο>

# Duration system
DURATION_DAY
	1 μέρα
DURATION_DAYS
	%d μέρες
DURATION_HOUR
	1 ώρα
DURATION_HOURS
	%d ώρες
DURATION_MINUTE
	1 λεπτό
DURATION_MINUTES
	%d λεπτά
DURATION_SECOND
	1 δευτερόλεπτο
DURATION_SECONDS
	%d δευτερόλεπτα

# Human readable expiration
NO_EXPIRE
	δεν λήγει ποτέ
EXPIRES_SOON
	λήγει στην επόμενη αναβάθμιση υπηρεσιών
EXPIRES_M
	λήγει μέσα σε %d λεπτά
EXPIRES_1M
	λήγει μέσα σε %d λεπτό
EXPIRES_HM
	λήγει μέσα σε %d ώρες, %d λεπτά
EXPIRES_H1M
	λήγει μέσα σε %d ώρες, %d λεπτά
EXPIRES_1HM
	λήγει μέσα σε %d ώρα, %d λεπτά
EXPIRES_1H1M
	λήγει μέσα σε %d ώρα, %d λεπτά
EXPIRES_D
	λήγει μέσα σε %d μέρες
EXPIRES_1D
	λήγει μέσα σε %d μέρα

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
	Αυτό το ψευδώνυμο ανήκει σε κάποιον άλλον.  Παρακαλώ διάλεξε κάποιο άλλο.
	(Αν αυτό είναι το ψευδώνυμό σου, γράψε %R%s IDENTIFY ^_κωδικό^_.)
NICK_IS_SECURE
	Αυτό το ψευδώνυμο είναι κατοχυρωμένο και προστατευμένο.  Αν αυτό είναι το
	ψευδώνυμο σου, γράψε %R%s IDENTIFY ^_κωδικό^_.  Αλλιώς,
	παρακαλώ διάλεξε κάποιο άλλο ψευδώνυμο.
NICK_MAY_NOT_BE_USED
	Αυτό το ψευδώνυμο δεν μπορεί να χρησιμοποιηθεί.  Παρακαλώ διάλεξε κάποιο άλλο.
FORCENICKCHANGE_IN_1_MINUTE
	Αν δεν αλλάξεις το ψευδώνυμο αυτό μέσα σε ένα λεπτό, θα στο αλλάξω αυτόματα εγώ.
FORCENICKCHANGE_IN_20_SECONDS
	Αν δεν αλλάξεις το ψευδώνυμο αυτό μέσα σε 20 δευτερόλεπτα, θα στο αλλάξω αυτόματα εγώ.
FORCENICKCHANGE_NOW
	Αυτό το ψευδώνυμο είναι ήδη κατοχυρωμένο; δεν μπορείς να το χρησιμοποιήσεις.
FORCENICKCHANGE_CHANGING
	Το ψευδώνυμό σου έχει αλλάξει σε %s

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER ^_κωδικό^_ [^_email^_]	
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER ^_κωδικό^_ ^_email^_
NICK_REGISTRATION_DISABLED
	Συγνώμη, η κατοχύρωση ψευδώνυμου είναι  για την ώρα απενεργοποιημένη.
NICK_REGISTRATION_FAILED
	Συγνώμη, η κατοχύρωση απέτυχε.
NICK_REG_PLEASE_WAIT
	Παρακαλώ περίμενε %d δευτερόλεπτα και μετά ξαναχρησιμοποίησε την εντολή κατοχύρωσης ξανά.
NICK_CANNOT_BE_REGISTERED
	Αυτό το ψευδώνυμο %s δεν μπορεί να κατοχυρωθεί.
NICK_ALREADY_REGISTERED
	Αυτό το ψευδώνυμο %s είναι ήδη κατοχυρωμένο!
NICK_REGISTERED
	Αυτό το ψευδώνυμο %s κατοχυρώθηκε υπό την διεύθυνσή σου: %s
NICK_REGISTERED_NO_MASK
	Nickname %s registered.
NICK_PASSWORD_IS
	Ο κωδικός σου είναι %s - θυμήσου τον για αργότερη χρήση.

# GROUP responses
NICK_REG_DELAY
	You have to be connected longer than %d seconds to register your nick.

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP ^_στόχος^_ ^_κωδικό^_
NICK_GROUP_DISABLED
	Συγνώμη, η ομαδοποίηση του ψευδώνυμού σου είναι για την ώρα απενεργοποιημένη.
NICK_GROUP_FAILED
	Συγνώμη, η ομαδοποίηση του ψευδώνυμού σου είναι για την ώρα απενεργοποιημένη.
NICK_GROUP_PLEASE_WAIT
	Παρακαλώ περίμενε %d δευτερόλεπτα και μετά ξαναχρησιμοποίησε την εντολή ομαδοποίησης.
NICK_GROUP_CHANGE_DISABLED
	Αυτό το ψευδώνυμο είναι ήδη κατοχυρωμένο; γράψε %R%s DROP αρχικά.
NICK_GROUP_SAME
	Είσαι ήδη μέλος της ομάδας του %s.
NICK_GROUP_TOO_MANY
	Είναι πάρα πολλά ψευδώνυμα μέσα %s's ομάδα; κράτα μερικά και βγάλε κάποια.
	Γράψε ^B%R%s HELP GLIST^B και %R%s HELP DROP 
	για περισσότερες πληροφορίες.
NICK_GROUP_JOINED
	Είσαι τώρα μέσα στην ομάδα του %s.

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY ^_κωδικό^_
NICK_IDENTIFY_FAILED
	Συγνώμη, η αναγνώριση απέτυχε.
NICK_IDENTIFY_SUCCEEDED
	Ο κωδικός έγινε δεκτός - τώρα είσαι αναγνωρισμένος.
NICK_IDENTIFY_EMAIL_REQUIRED
	Πρέπει να προσθέσεις ένα e-mail για το ψευδώνυμό σου.
	Με αυτό το e-mail θα μπορείς να λάβεις τον κωδικό σου σε
	περίπτωση που τον ξεχάσεις.
NICK_IDENTIFY_EMAIL_HOWTO
	Γράψε %R%S SET EMAIL ^_e-mail^_ για να προσθέσεις το e-mail στις πληροφορίες του ψευδώνυμού σου.
	Η μυστικότητα είναι σεβαστή; αυτό το e-mail και ο κωδικός του δεν πρέπει να δωθεί σε
	κανένα τρίτο πρόσωπο.
	
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
	Το ψευδώνυμο σου δεν το κατέχεις τώρα,πρέπει να κάνεις πάλι εντολή αναγνώρισης για να το κατέχεις.
NICK_LOGOUT_X_SUCCEEDED
	Το ψευδώνυμό σου %s δεν κατέχεται για την ώρα.

# DROP responses
NICK_LOGOUT_SERVICESADMIN
	Can't logout %s because he's a services administrator.

# DROP responses
NICK_DROP_DISABLED
	Συγνώμη, η υπηρεσία για να σβήνει τα  ψευδώνυμα είναι απενεργοποιημένη.
NICK_DROPPED
	Το ψευδώνυμό σου σβήστηκε επιτυχώς από την υπηρεσία.
NICK_X_DROPPED
	Το ψευδώνυμο %s σβήστηκε επιτυχώς από την υπηρεσία.

# SET responses
NICK_SET_SYNTAX
	SET ^_επιλογή^_ ^_παράμετροι^_
NICK_SET_SERVADMIN_SYNTAX
	SET [^_ψευδώνυμο^_] ^_επιλογή^_ ^_παράμετροι^_
NICK_SET_DISABLED
	Συγνώμη, η υπηρεσία για τον καθορισμό και τις επιλογές του ψευδώνυμου είναι για την ώρα απενεργοποιημένη.
NICK_SET_UNKNOWN_OPTION
	’γνωστη SET επιλογή %s.
NICK_SET_OPTION_DISABLED
	Η επιλογή %s δεν μπορεί να χρησιμοποιηθεί στο δίκτυο αυτό.

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	Η νέα επίδειξη ΠΡΕΠΕΙ να αναφέρετε σε ένα ψευδώνυμο που ανήκει στην ομαδοποιήση!
NICK_SET_DISPLAY_CHANGED
	Η νέα επίδειξη είναι τώρα %s.

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	Συγνώμη, δεν γίνετε να αλλάξεις τον κωδικό.
NICK_SET_PASSWORD_CHANGED
	Ο κωδικός άλλαξε.
NICK_SET_PASSWORD_CHANGED_TO
	Ο κωδικός άλλαξε σε %s.

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE ^_number^_
NICK_SET_LANGUAGE_UNKNOWN
	’γνωστο νούμερο γλώσσας %d.  Γράψε %R%s HELP SET LANGUAGE για την λίστα με τις γλώσσες.
NICK_SET_LANGUAGE_CHANGED
	Η γλώσσα άλλαξε σε Ελληνικά.

# SET URL responses
NICK_SET_URL_CHANGED
	URL άλλαξε σε %s.
NICK_SET_URL_UNSET
	URL δεν υπάρχει.

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	Η διεύθυνση του e-mail άλλαξε σε %s.
NICK_SET_EMAIL_UNSET
	Η διεύθυνση του e-mail απενεργοποιήθηκε.
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	Δεν μπορείς να απενεργοποιήσεις την διεύθυνση του e-mail σε αυτό το δίκτυο.
	
# SET ICQ responses
NICK_SET_ICQ_CHANGED
	Το νούμερο του ICQ άλλαξε σε %s.
NICK_SET_ICQ_UNSET
	Το νούμερο του ICQ απενεργοποιήθηκε.
NICK_SET_ICQ_INVALID
	%s δεν είναι σωστός αριθμός.
	
# SET GREET responses
NICK_SET_GREET_CHANGED
	Το μήνυμα χαιρετισμού άλλαξε σε %s.
NICK_SET_GREET_UNSET
	Η επιλογή μήνυμα χαιρετισμού απενεργοποιήθηκε.

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	Η επιλογή προστασίας είναι τώρα ON.
NICK_SET_KILL_QUICK
	Η επιλογή προστασίας είναι τώρα ON, με ελλατωμένο χρόνο για να γίνει εντολή αναγνώριση.
NICK_SET_KILL_IMMED
	Η επιλογή προστασίας είναι τώρα ON, με κανένα περιθώριο να γίνει εντολή αναγνώρισης.
NICK_SET_KILL_IMMED_DISABLED
	Η IMMED επιλογή δεν είναι διαθέσιμη σε αυτό το δίκτυο.
NICK_SET_KILL_OFF
	Η επιλογή προστασίας έιναι τώρα OFF.

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	Η επιλογή ασφάλειας είναι τώρα ON.
NICK_SET_SECURE_OFF
	Η επιλογή ασφάλειας είναι τώρα OFF.

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	Η επιλογή μυστικότητας είναι τώρα ON.
NICK_SET_PRIVATE_OFF
	Η επιλογή ασφάλειας είναι τώρα OFF.

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	Η διεύθυνση του e-mail δεν θα παρουσιάζεται στην %s INFO εντολή
NICK_SET_HIDE_EMAIL_OFF
	Η διεύθυνση του ε-mail θα παρουσιάζεται στην %s INFO εντολή.
NICK_SET_HIDE_MASK_ON
	Δεν θα εμφανίζεται το τελευταίο user@host στην %s INFO εντολή.
NICK_SET_HIDE_MASK_OFF
	Θα εμφανίζεται το τελευταίο user@host στην %s INFO εντολή.
NICK_SET_HIDE_QUIT_ON
	Το τελευταίο αποχαιρετιστήριο μήνυμα απο δω και στο εξής δεν θα παρουσιάζεται στην %s INFO εντολή.
NICK_SET_HIDE_QUIT_OFF
	Το τελευταίο αποχαιρετιστήριο μήνυμα απο δω και στο εξής θα παρουσιάζεται στην %s INFO εντολή.
	
# SET MSG responses
NICK_SET_HIDE_STATUS_ON
	Your services access status will now be hidden from %s INFO displays.
NICK_SET_HIDE_STATUS_OFF
	Your services access status will now be shown in %s INFO displays.
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_MSG_ON
	Οι υπηρεσίες θα σε ειδοποιούν με messages.
NICK_SET_MSG_OFF
	Οι υπηρεσίες θα σε ειδοποιούν με notices.

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
	ACCESS {ADD | DEL | LIST} [mask]
NICK_ACCESS_ALREADY_PRESENT
	Η μάσκα %s είναι ήδη μέσα στην λίστα πρόσβασης.
NICK_ACCESS_REACHED_LIMIT
	Συγνώμη, μπορείς μόνο να έχεις %d πρόσβαση σε κατάταξη του ψευδώνυμου.
NICK_ACCESS_ADDED
	%s έγινε προσθήκη στην λίστα πρόσβασής σου.
NICK_ACCESS_NOT_FOUND
	%s δεν βρέθηκε στη λίστα πρόσβασής σου.
NICK_ACCESS_DELETED
	%s διαγράφτηκε από την λίστα πρόσβασής σου.
NICK_ACCESS_LIST
	Λίστα πρόσβασης:
NICK_ACCESS_LIST_X
	Λίστα πρόσβασης για %s:
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
	%s είναι %s
NICK_INFO_SERVICES_OPER
	%s είναι ένας χειριστής των υπηρεσιών.
NICK_INFO_SERVICES_ADMIN
	%s είναι ο επικεφαλής των υπηρεσιών.
NICK_INFO_SERVICES_ROOT
	%s is a services root administrator.
NICK_INFO_ADDRESS
	Τελευταία διεύθυνση: %s
NICK_INFO_ADDRESS_ONLINE
	  Είναι στο δίκτυο από: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s είναι ήση μέσα στο δίκτυο.
NICK_INFO_TIME_REGGED
	  Κατάσταση κατοχύρωσης με την ημερομηνία: %s
NICK_INFO_LAST_SEEN
	   Τελευταία φορά που ήταν στο δίκτυο: %s
NICK_INFO_LAST_QUIT
	Τελευταίο αποχαιρετιστήριο μήνυμα: %s
NICK_INFO_URL
	              URL: %s
NICK_INFO_EMAIL
	   E-mail διεύθυνση: %s
NICK_INFO_VHOST
	            vhost: %s
NICK_INFO_VHOST2
	            vhost: %s@%s
NICK_INFO_ICQ
	            ICQ #: %d
NICK_INFO_GREET
	    Μήνυμα χαιρετισμού: %s
NICK_INFO_OPTIONS
	          Επιλογές: %s
NICK_INFO_EXPIRE
	Expires in: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	Προστασία
NICK_INFO_OPT_SECURE
	Ασφάλεια
NICK_INFO_OPT_PRIVATE
	Μυστικότητα
NICK_INFO_OPT_MSG
	Επιλογή μηνύματος
NICK_INFO_OPT_AUTOOP
	Auto-op
NICK_INFO_OPT_NONE
	Κανένα
NICK_INFO_NO_EXPIRE
	Αυτό το ψευδώνυμο δεν θα λήξει ποτέ.
NICK_INFO_FOR_MORE
	Για περισσότερες πληροφορίες, γράψε %R%s INFO %s ALL.

# LIST responses
NICK_INFO_SUSPENDED
	This nickname is currently suspended, reason: %s
NICK_INFO_SUSPENDED_NO_REASON
	This nickname is currently suspended

# LIST responses
NICK_LIST_SYNTAX
	LIST pattern
NICK_LIST_SERVADMIN_SYNTAX
	LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	Η λίστα με τις καταχωρήσεις βρέθηκαν %s:
NICK_LIST_RESULTS
	Τέλος της λίστας - %d/%d βρέθηκαν και εμφανίστηκαν.

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
	Τέλος της λίστας - %d/%d βρέθηκαν και εμφανίστηκαν.

# GLIST responses
NICK_GLIST_HEADER
	Η λίστα με τα ψευδώνυμα στην ομάδα:
NICK_GLIST_HEADER_X
	Η λίστα με τα ψευδώνυμα στην ομάδα του %s:
NICK_GLIST_FOOTER
	%d ψευδώνυμα μέσα στην ομάδα.
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (expires in %s)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nickname [password]
NICK_NO_RECOVER_SELF
	Δεν μπορείς να ψάχνεις τον εαυτό σου!
NICK_RECOVERED
	User claiming your nick has been killed.
	%R%s RELEASE %s to get it back before %s timeout.

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nickname [password]
NICK_RELEASE_NOT_HELD
	Το ψευδώνυμο %s δεν κρατείται..
NICK_RELEASED
	Οι υπηρεσίες απελευθέρωααν το ψευδώνυμό σου.

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nickname [password]
NICK_NO_GHOST_SELF
	Δεν μπορείς να σκοτώσεις τον εαυτό σου!
NICK_GHOST_KILLED
	Το ψευδώνυμο σου έγινε kill μέσω ghost εντολής.

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nickname
NICK_GETPASS_UNAVAILABLE
	Η GETPASS εντολή δεν είναι διαθέσιμη γιατί χρησιμοποιείται η κωδικοποίηση.
NICK_GETPASS_PASSWORD_IS
	Ο κωδικός για τον %s είναι %s.

# SENDPASS responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host No WildCards!!
NICK_GETEMAIL_EMAILS_ARE
	Emails Match %s to %s.
NICK_GETEMAIL_NOT_USED
	No Emails listed for %s.
	
# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS nickname
NICK_SENDPASS_UNAVAILABLE
	Η SENDPASS εντολή δεν είναι διαθέσιμη γιατί χρησιμοποιείται η κωδικοποίηση.
NICK_SENDPASS_SUBJECT
	Ο κωδικός του ψευδώνυμου (%s)
NICK_SENDPASS_HEAD
	Γειά,
NICK_SENDPASS_LINE_1
	Είσαι στην λίστα για να λάβεις τον κωδικό του ψευδώνυμού σου %s μέσω e-mail.
NICK_SENDPASS_LINE_2
	Ο κωδικό σου είναι %s Για περισσότερη ασφάλεια, θα πρέπει να αλλάξεις όσο σύντομα γίνεται τον κωδικό σου αφού λάβεις τον κωδικό σου σε e-mail.
NICK_SENDPASS_LINE_3
	Αν δεν γνωρίζεις γιατί αυτό το e-mail στάλθηκε σε σένα, παρακαλώ αγνόησέ το χωρίς ερωτήσεις κλπ.
NICK_SENDPASS_LINE_4
	ΠΑΡΑΚΑΛΩ ΜΗΝ ΑΠΑΝΤΗΣΕΙΣ ΣΕ ΑΥΤΟ ΤΟ E-MAIL!
NICK_SENDPASS_LINE_5
	%s επικεφαλείς υπηρεσιών.
NICK_SENDPASS_OK
	Ο κωδικός του %s έχει σταλθεί.

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
	FORBID nickname [reason]
NICK_FORBID_SYNTAX_REASON
	FORBID nickname reason
NICK_FORBID_SUCCEEDED
	Το ψευδώνυμο %s από τώρα παρακρατείται.
NICK_FORBID_FAILED
	Δεν μπορείς να παρακρατήσεις αυτό το ψευδώνυμο %s!

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
	Το κανάλι δίνει operator status σε κατοχυρωμένα ή μη ψευδώνυμα
CHAN_LEVEL_AUTOVOICE
	Το κανάλι δίνει φωνή ή +v σε κατοχυρωμένα ή μη ψευδώνυμα
CHAN_LEVEL_AUTOHALFOP
	Το κανάλι δίνει half op ή +h σε κατοχυρωμένα ή μη ψευδώνυμα
CHAN_LEVEL_AUTOPROTECT
	Το κανάλι δίνει σε ψευδώνυμα +a δηλαδή ασφάλεια
CHAN_LEVEL_AUTODEOP
	Το κανάλι δεν επιτρέπει το operator status
CHAN_LEVEL_NOJOIN
	Δεν επιτρέπεται να μπείτε στο κανάλι αν είναι RESTRICTED
CHAN_LEVEL_INVITE
	Επιτρέπεται να χρησιμοποιείται INVITE εντολή
CHAN_LEVEL_AKICK
	Επιτρέπεται να χρησιμοποιείται AKICK εντολή
CHAN_LEVEL_SET
	Επιτρέπεται να χρησιμοποιείται SET εντολή (not FOUNDER/PASSWORD)
CHAN_LEVEL_CLEAR
	Επιτρέπεται να χρησιμοποιείται CLEAR εντολή
CHAN_LEVEL_UNBAN
	Επιτρέπεται να χρησιμοποιείται UNBAN εντολή
CHAN_LEVEL_OPDEOP
	Επιτρέπεται να χρησιμοποιείται OP/DEOP εντολές
CHAN_LEVEL_ACCESS_LIST
	Επιτρέπεται να βλέπεται την λίστα πρόσβασης
CHAN_LEVEL_ACCESS_CHANGE
	Επιτρέπεται να παρέμβεται και να αλλάξεται την λίστα πρόσβασης
CHAN_LEVEL_MEMO
	Επιτρέπεται να δείς την λίστα και να διαβάσεις τα μηνύματα που έχουν σταλεί στο κανάλι
CHAN_LEVEL_ASSIGN
	Επιτρέπεται να μπεί/βγεί το bot
CHAN_LEVEL_BADWORDS
	Επιτρέπεται να χρησιμοποιήσεις την BADWORDS εντολή
CHAN_LEVEL_NOKICK
	Τα bot's να μην πετάνε έξω τους kickers
CHAN_LEVEL_FANTASIA
	Επιτρέπεται να χρησιμοποιήσεις fantaisist εντολές
CHAN_LEVEL_SAY
	Επιτρέπεται να χρησιμοποιήσεις SAY και ACT εντολές
CHAN_LEVEL_GREET
	Το μήνυμα χαιρετισμού να διατυπώνεται
CHAN_LEVEL_VOICEME
	Επιτρέπεται να κάνεις (de)voice τον εαυτό σου
CHAN_LEVEL_VOICE
	Επιτρέπεται να χρησιμοποιήσεις VOICE/DEVOICE εντολές
CHAN_LEVEL_GETKEY
	Επιτρέπεται να χρησιμοποιήσεις την GETKEY εντολή
CHAN_LEVEL_OPDEOPME
	Επιτρέπεται να κάνεις (de)op τον εαυτό σου
CHAN_LEVEL_HALFOPME
	Επιτρέπεται να κάνεις (de)halfop τον εαυτό σου
CHAN_LEVEL_HALFOP
	Επιτρέπεται να χρησιμοποιήσεις HALFOP/DEHALFOP εντολές
CHAN_LEVEL_PROTECTME
	Επιτρέπεται να κάνεις (de)protect τον εαυτό σου
CHAN_LEVEL_PROTECT
	Επιτρέπεται να χρησιμοποιήσεις PROTECT/DEPROTECT εντολές
CHAN_LEVEL_KICKME
	Επιτρέπεται να κάνεις kick τον εαυτό σου
CHAN_LEVEL_KICK
	Επιτρέπεται να χρησιμοποιήσεις την KICK εντολή
CHAN_LEVEL_SIGNKICK
	Όχι signed kick όταν το SIGNKICK LEVEL χρησιμοποιείται
CHAN_LEVEL_BANME
	Επιτρέπεται να κάνεις Ban τον εαυτό σου
CHAN_LEVEL_BAN
	Επιτρέπεται να χρησιμοποιήσεις την BAN εντολή
CHAN_LEVEL_TOPIC
	Επιτρέπεται να χρησιμοποιήσεις την TOPIC εντολή
CHAN_LEVEL_INFO
	Επιτρέπεται να χρησιμοποιήσεις την INFO εντολή με την ALL επιλογή

# Automatic responses
CHAN_IS_REGISTERED
	Αυτό το κανάλι έχει κατοχυρωθεί με τον %s.
CHAN_NOT_ALLOWED_OP
	Δεν μπορείς να έχεις chanop status σε αυτό το κανάλι %s.
CHAN_MAY_NOT_BE_USED
	Αυτό το κανάλι δεν μπορεί να χρησιμοποιηθεί.
CHAN_NOT_ALLOWED_TO_JOIN
	Δεν έχεις την άδεια να είσαι σε αυτό το κανάλι.
	
# REGISTER responses
CHAN_X_INVALID
	Channel %s is not a valid channel.

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER channel password description
CHAN_REGISTER_DISABLED
	Συγνώμη, η υπηρεσία κατοχύρωσεις καναλιών δεν λειτουργεί για την ώρα.
CHAN_REGISTER_NOT_LOCAL
	Κανάλια τοπικά δεν μπορούν να κατοχυρωθούν.
CHAN_MUST_REGISTER_NICK
	Πρέπει να κατοχυρώσεις το ψευδώνυμό σου πρώτα.  Γράψε %R%s HELP για πληροφορίες σχετικά με τα ονόματα που έχουν κατοχυρωθεί..
CHAN_MUST_IDENTIFY_NICK
	Παρακαλώ κάνε την εντολή αναγνώρισης με τον %s πρώτα, χρησιμοποιώντας την εντολή:
	%R%s IDENTIFY password
CHAN_MAY_NOT_BE_REGISTERED
	Το κανάλι %s δεν μπορεί να κατοχυρωθεί.
CHAN_ALREADY_REGISTERED
	Το κανάλι %s είναι ήδη κατοχυρωμένο!
CHAN_MUST_BE_CHANOP
	Πρέπει να είσαι να έχεις operator status στο κανάλι για να το κατοχυρώσεις.
CHAN_REACHED_CHANNEL_LIMIT
	Συγνώμη, έχεις ήδη φτάσει στο όριο κατοχύρωσεις των %d καναλιών.
CHAN_EXCEEDED_CHANNEL_LIMIT
	Συγνώμη, έχεις ήδη ξεπεράσει το όριο κατοχύρωσεις των %d καναλιών.
CHAN_REGISTRATION_FAILED
	Συγνώμη, η κατοχύρωση απέτυχε.
CHAN_REGISTERED
	Το κανάλι %s είναι κατοχυρωμένο από το ψευδώνυμό σου: %s
CHAN_PASSWORD_IS
	Ο κωδικός του καναλιού είναι %s - θυμήσου τον γιατί θα τον χρειαστείς μετά.
CHAN_REGISTER_NONE_CHANNEL
	You have attempted to register a nonexistent channel %s
CHAN_SYMBOL_REQUIRED
	Please use the symbol of # when attempting to register


# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY channel password
CHAN_IDENTIFY_FAILED
	Συγνώμη, η αναγνώριση για το κανάλι απέτυχε.
CHAN_IDENTIFY_SUCCEEDED
	Ο κωδικός έγινε δεκτός - τώρα έχεις founder-level πρόσβαση στο %s.

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT channel nickname
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT channel [nickname]
CHAN_LOGOUT_SUCCEEDED
	Ο χρήστης %s έγινε logged out από το κανάλι %s.
CHAN_LOGOUT_ALL_SUCCEEDED
	Όλοι οι χρήστες logged out από το κανάλι %s (except the channel founder).
CHAN_LOGOUT_FOUNDER_FAILED
	You may not log out of %s (you are founder).

# DROP responses
CHAN_DROP_SYNTAX
	DROP channel
CHAN_DROP_DISABLED
	Συγνώμη, η υπηρεσία διαγραφής καναλιών είναι απενεργοποιημένη.
CHAN_DROPPED
	Το κανάλι %s διαγράφτηκε από τις υπηρεσίες.

# SET responses
CHAN_SET_SYNTAX
	SET channel option parameters
CHAN_SET_DISABLED
	Συγνώμη, οι επιλογές για την ρύθμιση των καναλιών είναι απενεργοποιημένες.
CHAN_SET_UNKNOWN_OPTION
	’γνωστη SET επιλογή %s.

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s έχει πολλά κατοχυρωμένα κανάλια.
CHAN_FOUNDER_CHANGED
	Ο Founder του %s άλλαξε και πήγε στον %s.

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	Ο Successor του %s άλλαξε και πήγε στον %s.
CHAN_SUCCESSOR_UNSET
	Ο Successor του %s δεν έχει οριστεί.
CHAN_SUCCESSOR_IS_FOUNDER
	%s δεν μπορεί να είναι successor στο κανάλι %s γιατί είναι founder σε αυτό.

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	Συγνώμη, δεν μπόρεσε να γίνει δεκτός ο κωδικός.
CHAN_PASSWORD_CHANGED
	%s ο κωδικός άλλαξε.
CHAN_PASSWORD_CHANGED_TO
	%s ο κωδικός άλλαξε σε %s.

# SET DESC responses
CHAN_DESC_CHANGED
	Περιγραφή του %s άλλαξε σε %s.

# SET URL responses
CHAN_URL_CHANGED
	URL του %s άλλαξε σε %s.
CHAN_URL_UNSET
	URL του %s δεν έχει οριστεί.

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	E-mail διεύθυνση του %s άλλαξε σε %s.
CHAN_EMAIL_UNSET
	E-mail διεύθυνση του %s δεν έχει οριστεί.

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	Το μήνυμα υποδοχής του %s άλλαξε.
CHAN_ENTRY_MSG_UNSET
	Το μήνυμα υποδοχής του %s δεν έχει οριστεί.
	
# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s δεν είναι σωστός ban τύπος.
CHAN_SET_BANTYPE_CHANGED
	Ban τύπος του καναλιού %s είναι τώρα #%d.

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	’γνωστη επιλογή χαρακτήρα %c αγνοήθηκε.
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	Η επιλογή %c αγνοήθηκε γιατί δεν μπορείς να το κλειδώσεις.
CHAN_SET_MLOCK_L_REQUIRED
	Πρέπει να το κλειδώσεις με +l όπως και με +L.
CHAN_SET_MLOCK_K_REQUIRED
	Πρέπει να κλειδώσεις με +i όπως και με +K.
CHAN_MLOCK_CHANGED
	Η επιλογή κλειδώματος στο κανάλι %s άλλαξε σε %s.

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET channel KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	Topic retention option for %s is now ON.
CHAN_SET_KEEPTOPIC_OFF
	Topic retention option for %s is now OFF.

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET channel TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	Topic lock option for %s is now ON.
CHAN_SET_TOPICLOCK_OFF
	Topic lock option for %s is now OFF.

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET channel PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	Peace option for %s is now ON.
CHAN_SET_PEACE_OFF
	Peace option for %s is now OFF.

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET channel PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	Private option for %s is now ON.
CHAN_SET_PRIVATE_OFF
	Private option for %s is now OFF.

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET channel SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	Secure ops option for %s is now ON.
CHAN_SET_SECUREOPS_OFF
	Secure ops option for %s is now OFF.

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET channel SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	Secure founder option for %s is now ON.
CHAN_SET_SECUREFOUNDER_OFF
	Secure founder option for %s is now OFF.

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET channel RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	Restricted access option for %s is now ON.
CHAN_SET_RESTRICTED_OFF
	Restricted access option for %s is now OFF.

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET channel SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	Secure option for %s is now ON.
CHAN_SET_SECURE_OFF
	Secure option for %s is now OFF.

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET channel SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	Signed kick option for %s is now ON.
CHAN_SET_SIGNKICK_LEVEL
	Signed kick option for %s is now ON, but depends of the
	level of the user that is using the command.
CHAN_SET_SIGNKICK_OFF
	Signed kick option for %s is now OFF.

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET channel OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	Op-notice option for %s is now ON.
CHAN_SET_OPNOTICE_OFF
	Op-notice option for %s is now OFF.

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET channel XOP {ON | OFF}
CHAN_SET_XOP_ON
	xOP lists system for %s is now ON.
CHAN_SET_XOP_OFF
	xOP lists system for %s is now OFF.

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET channel NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	Το κανάλι %s δεν θα λήξει.
CHAN_SET_NOEXPIRE_OFF
	Το κανάλι %s θα λήξει.

# xOP messages
CHAN_XOP_REACHED_LIMIT
	Συγνώμη, μπορείς μόνο να χρησιμοποιήσεις %d AOP/SOP/VOP στο κανάλι.
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	Δεν μπορείς να χρησιμοποιήσεις αυτή την εντολή. Χρησιμοποίησε την ACCESS εντολή αντί για αυτή.
	Γράψε %R%s HELP ACCESS για περισσότερες πληροφορίες.
CHAN_XOP_NOT_AVAILABLE
	xOP system is not available.

# AOP messages
CHAN_AOP_SYNTAX
	AOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]
CHAN_AOP_DISABLED
	Συγνώμη, η υπηρεσία για την εντολή AOP είναι εκτός λειτουργίας.
CHAN_AOP_NICKS_ONLY
	Η εντολή Channel AOP λίστα περιέχει μόνο κατοχυρωμένα ψευδώνυμα.
CHAN_AOP_ADDED
	Ο/η %s προσθέθηκε επιτυχώς στην AOP λίστα του %s.
CHAN_AOP_MOVED
	%s moved to %s AOP list.
CHAN_AOP_NO_SUCH_ENTRY
	Δεν υπάρχει αυτή η θέση (#%d) στην AOP λίστα του %s.
CHAN_AOP_NOT_FOUND
	Ο/η %s δεν βρέθηκε η θέση στην AOP λίστα του %s.
CHAN_AOP_NO_MATCH
	Δεν βρέθηκαν θέσεις στην AOP λίστα του %s.
CHAN_AOP_DELETED
	Ο/η %s διαγράφηκε επιτυχώς από την AOP λίστα του %s.
CHAN_AOP_DELETED_ONE
	Διαγράφηκε 1 θέση από την AOP λίστα του %s.
CHAN_AOP_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από την AOP λίστα του %s.
CHAN_AOP_LIST_EMPTY
	Η AOP λίστα του %s είναι άδεια.
CHAN_AOP_LIST_HEADER
	Η AOP λίστα του %s είναι :
	  Νούμερο  Ψευδώνυμο

# HOP messages
CHAN_AOP_CLEAR
	Channel %s AOP list has been cleared.

# HOP messages
CHAN_HOP_SYNTAX
	HOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]
CHAN_HOP_DISABLED
	Συγνώμη, η υπηρεσία για την εντολή HOP είναι εκτός λειτουργίας.
CHAN_HOP_NICKS_ONLY
	Η εντολή Channel HOP λίστα περιέχει μόνο κατοχυρωμένα ψευδώνυμα.
CHAN_HOP_ADDED
	Ο/η %s προσθέθηκε επιτυχώς στην HOP λίστα του %s.
CHAN_HOP_MOVED
	%s moved to %s HOP list.
CHAN_HOP_NO_SUCH_ENTRY
	Δεν υπάρχει αυτή η θέση (#%d) στην HOP λίστα του %s.
CHAN_HOP_NOT_FOUND
	Ο/η %s δεν βρέθηκε στην HOP λίστα του %s.
CHAN_HOP_NO_MATCH
	Δεν βρέθηκαν θέσεις στην HOP λίστα του %s.
CHAN_HOP_DELETED
	Ο/η %s διαγράφηκε επιτυχώς από την HOP λίστα του %s.
CHAN_HOP_DELETED_ONE
	Διαγράφηκε 1 8έση από την HOP λίστα του %s.
CHAN_HOP_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από την ΗOP λίστα του %s.
CHAN_HOP_LIST_EMPTY
	Η HOP λίστα του %s είναι άδεια.
CHAN_HOP_LIST_HEADER
	Η HOP λίστα του %s είναι:
	  Νούμερο  Ψευδώνυμο

# SOP messages
CHAN_HOP_CLEAR
	Channel %s HOP list has been cleared.

# SOP messages
CHAN_SOP_SYNTAX
	SOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]
CHAN_SOP_DISABLED
	Συγνώμη, η υπηρεσία για την εντολή SOP είναι εκτός λειτουργίας.
CHAN_SOP_NICKS_ONLY
	Η εντολή Channel SOP λίστα περιλαμβάνει μόνο κατοχυρωμένα ψευδώνυμα.
CHAN_SOP_ADDED
	Ο/η %s προσθέθηκε επιτυχώς στην SOP λίστα του %s.
CHAN_SOP_MOVED
	%s moved to %s SOP list.
CHAN_SOP_NO_SUCH_ENTRY
	Δεν υπάρχει αυτή η θέση (#%d) στην SOP λίστα του %s.
CHAN_SOP_NOT_FOUND
	Ο/η %s δεν βρέθηκε στην SOP λίστα του %s.
CHAN_SOP_NO_MATCH
	Δεν βρέθηκαν θέσεις στην SOP λίστα του %s.
CHAN_SOP_DELETED
	Ο/η %s διαγράφηκε επιτυχώς από την SOP λίστα του %s.
CHAN_SOP_DELETED_ONE
	Διαγράφηκε 1 8έση από την SOP λίστα του %s.
CHAN_SOP_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από την SOP λίστα του %s.
CHAN_SOP_LIST_EMPTY
	Η SOP λίστα του %s είναι άδεια.
CHAN_SOP_LIST_HEADER
	Η SOP λίστα του %s είναι:
	  Νούμερο  Ψευδώνυμο

# VOP messages
CHAN_SOP_CLEAR
	Channel %s SOP list has been cleared.

# VOP messages
CHAN_VOP_SYNTAX
	VOP channel {ADD|DEL|LIST|CLEAR} [nick | entry-list]
CHAN_VOP_DISABLED
	Συγνώμη, η υπηρεσία για την εντολή VOP είναι εκτός λειτουργίας.
CHAN_VOP_NICKS_ONLY
	Η εντολή Channel VOP λίστα περιλαμβάνει μόνο κατοχυρωμένα ψευδώνυμα.
CHAN_VOP_ADDED
	Ο/η %s προσθέθηκε επιτυχώς στην VOP λίστα του %s.
CHAN_VOP_MOVED
	%s moved to %s VOP list.
CHAN_VOP_NO_SUCH_ENTRY
	Δεν υπάρχει αυτή η θέση (#%d) στην VOP λίστα του %s.
CHAN_VOP_NOT_FOUND
	Ο/η %s δεν βρέθηκε στην VOP λίστα του %s.
CHAN_VOP_NO_MATCH
	Δεν βρέθηκαν θέσεις στην VOP λίστα του %s.
CHAN_VOP_DELETED
	Ο/η %s διαγράφηκε επιτυχώς από την VOP λίστα του %s.
CHAN_VOP_DELETED_ONE
	Διαγράφηκε 1 8έση από την VOP λίστα του %s.
CHAN_VOP_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από την VOP λίστα του %s.
CHAN_VOP_LIST_EMPTY
	Η VOP λίστα του %s είναι άδεια.
CHAN_VOP_LIST_HEADER
	Η VOP λίστα του %s είναι:
	  Νούμερο  Ψευδώνυμο

# ACCESS messages
CHAN_VOP_CLEAR
	Channel %s VOP list has been cleared.

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS channel {ADD|DEL|LIST|CLEAR} [nick [level] | entry-list]
CHAN_ACCESS_XOP
	Δεν μπορείς να χρησιμοποιήσεις αυτή την εντολή. 
	Χρησιμοποίησε AOP, SOP και VOP εντολές αντί για την access.
	Γράψε %R%s HELP command για περισσότερες πληροφορίες.
CHAN_ACCESS_XOP_HOP
	You can't use this command. 
	Use the AOP, SOP, HOP and VOP commands instead.
	Type %R%s HELP command for more information.
CHAN_ACCESS_DISABLED
	Συγνώμη, η υπηρεσία για την εντολή access είναι εκτός λειτουργίας.
CHAN_ACCESS_LEVEL_NONZERO
	Η Access level δεν πρέπει να είναι ποτέ στο μηδέν.
CHAN_ACCESS_LEVEL_RANGE
	Η Access level πρέπει να είναι ανάμεσα στο %d και %d συμπαραλαμβανόμενου.
CHAN_ACCESS_NICKS_ONLY
	Η Channel access λίστα περιλαμβάνει μόνο κατοχυρωμένα ψευδώνυμα.
CHAN_ACCESS_REACHED_LIMIT
	Συγνώμη, μπορείς μόνο να έχεις %d access στο κανάλι.
CHAN_ACCESS_LEVEL_UNCHANGED
	Η Access level του %s στο %s δεν αλλάζει από τον %d.
CHAN_ACCESS_LEVEL_CHANGED
	Η Access level για τον %s στο %s άλλαξε σε %d.
CHAN_ACCESS_ADDED
	Ο/η %s προσθέθηκε στο access list του %s με level %d.
CHAN_ACCESS_NO_SUCH_ENTRY
	Δεν υπάρχει αυτή η θέση (#%d) στο %s access list.
CHAN_ACCESS_NOT_FOUND
	Ο/η %s δεν βρέθηκε στου %s στην access list.
CHAN_ACCESS_NO_MATCH
	Δεν βρέθηκαν θέσεις στου %s στην access list.
CHAN_ACCESS_DELETED
	%s διαγράφτηκε από το access list του %s.
CHAN_ACCESS_DELETED_ONE
	Διαγράφει 1 θέση από %s access list.
CHAN_ACCESS_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από %s access list.
CHAN_ACCESS_LIST_EMPTY
	Η λίστα του %s access list είναι άδεια.
CHAN_ACCESS_LIST_HEADER
	Η access λίστα του %s είναι:
	  Νούνερο   Βαθμός  Ψευδώνυμο
CHAN_ACCESS_LIST_FOOTER
	Τελος της λίστας προσβασης.
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s

# AKICK responses
CHAN_ACCESS_CLEAR
	Channel %s access list has been cleared.

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK channel {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [nick-or-usermask] [reason]
CHAN_AKICK_DISABLED
	Συγνώμη, η υπηρεσία για την εντολή akick είναι εκτός λειτουργίας.
CHAN_AKICK_ALREADY_EXISTS
	Ο/η %s υπάρχει ήδη στην autokick λίστα του %s.
CHAN_AKICK_REACHED_LIMIT
	Συγνώμη, μπορείς μόνο να έχεις %d autokick μάσκα στο κανάλι.
CHAN_AKICK_ADDED
	Ο/η %s προσθέθηκε επιτυχώς στην autokick λίστα του %s.
CHAN_AKICK_NO_SUCH_ENTRY
	Δεν υπάρχει αυτή η θέση (#%d) στο %s akick list.
CHAN_AKICK_NOT_FOUND
	O %s δεν βρέθηκε στην autokick list του %s.
CHAN_AKICK_NO_MATCH
	Δεν βρέθηκαν θέσεις στην autokick list του %s.
CHAN_AKICK_STUCK
	%s είναι από τωρα ενεργό στο κανάλι %s.
CHAN_AKICK_UNSTUCK
	%s δεν είναι συνέχεια ενεργό πια στο κανάλι %s.
CHAN_AKICK_DELETED
	Ο/η %s διαγράφτηκε από την autokick list του %s.
CHAN_AKICK_DELETED_ONE
	Διαγράφτηκε 1 θέση από την autokick list %s.
CHAN_AKICK_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από την autokick list %s.
CHAN_AKICK_LIST_EMPTY
	H autokick list του %s είναι άδεια.
CHAN_AKICK_LIST_HEADER
	Η Autokick list του %s είναι:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (από %s στο %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (stuck) (από %s στο %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	Η AKICK ENFORCE για τον %s ολοκληρώθηκε; %d χρήστες θα επιρρεαστούν.

# LEVELS responses
CHAN_AKICK_CLEAR
	Channel %s akick list has been cleared.

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS channel {SET | DIS[ABLE] | LIST | RESET} [item [level]]
CHAN_LEVELS_XOP
	Αυτή η εντολή είναι χρήσιμη για αυτή την επιλογή.
CHAN_LEVELS_RANGE
	Ο βαθμός πρέπει να είναι μεταξύ %d και %d.
CHAN_LEVELS_CHANGED
	Ο βαθμός για τον %s στο κανάλι %s άλλξε σε %d.
CHAN_LEVELS_UNKNOWN
	Η επιλογή %s δεν είναι γνωστή.  Γράψε %R%s HELP LEVELS DESC για τη λίστα με τισ αναλυτικές επιλογές.
CHAN_LEVELS_DISABLED
	%s απενεργοποιήθηκαν στο κανάλι %s.
CHAN_LEVELS_LIST_HEADER
	Ο βαθμός access για το κανάλι %s:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (απενεργοποιημένο)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (καναλάρχης μόνο)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	Ο βαθμός Access για το %s σβήστηκε στο αρχικό.

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
	INFO channel [ALL]
CHAN_INFO_HEADER
	Πληροφορίες για το κανάλι %s:
CHAN_INFO_FOUNDER
	        Καναλάρχης: %s (%s)
CHAN_INFO_NO_FOUNDER
	        Καναλάρχης: %s
CHAN_INFO_SUCCESSOR
	      Υποκαναλάρχης: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	      Υποκαναλάρχης: %s
CHAN_INFO_DESCRIPTION
	    Περιγραφή: %s
CHAN_INFO_ENTRYMSG
	  Μήνυμα εισόδου: %s
CHAN_INFO_TIME_REGGED
	     Κατοχυρώθηκε: %s
CHAN_INFO_LAST_USED
	      Τελευταία χρήση: %s
CHAN_INFO_LAST_TOPIC
	     Τελευταίο topic: %s
CHAN_INFO_TOPIC_SET_BY
	   Το Topic γράφτηκε από τον: %s
CHAN_INFO_URL
	            URL: %s
CHAN_INFO_EMAIL
	 E-mail διεύθυνση: %s
CHAN_INFO_BANTYPE
	       Τύπος Ban: %d
CHAN_INFO_OPTIONS
	        Επιλογές: %s
CHAN_INFO_OPT_KEEPTOPIC
	Topic ενναλαγής
CHAN_INFO_OPT_OPNOTICE
	Ποιος χρησιμοποίησε OP
CHAN_INFO_OPT_PEACE
	Ειρηνικό
CHAN_INFO_OPT_PRIVATE
	Μυστικό
CHAN_INFO_OPT_RESTRICTED
	Μόνο όσοι έχουν Access
CHAN_INFO_OPT_SECURE
	Ασφαλές
CHAN_INFO_OPT_SECUREOPS
	Μόνο αυτοί που έχουν access θα παίρνουν op από opers
CHAN_INFO_OPT_SECUREFOUNDER
	Ασφάλεια στον Καναλάρχη
CHAN_INFO_OPT_SIGNKICK
	Δηλωμένα kicks
CHAN_INFO_OPT_TOPICLOCK
	Κλείδωμα του Topic
CHAN_INFO_OPT_XOP
	xOP λίστα συστήματος
CHAN_INFO_OPT_NONE
	Κανένα
CHAN_INFO_MODE_LOCK
	      Επιλογές κλειδώματος: %s
CHAN_INFO_EXPIRE
	      Expires in: %s
CHAN_INFO_NO_EXPIRE
	Αυτό το κανάλι δεν θα λήξει.

# LIST responses
CHAN_LIST_SYNTAX
	LIST pattern
CHAN_LIST_SERVADMIN_SYNTAX
	LIST pattern [FORBIDDEN] [NOEXPIRE]
CHAN_LIST_HEADER
	Οι θέσεις της λίστας ταιριάζουν με το %s:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	Τέλος λίστας - %d/%d βρέθηκαν εμφανίστηκαν.
	
# CLIST responses
CHAN_INVITE_SYNTAX
	INVITE κανάλι

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN κανάλι
CHAN_UNBANNED
	γινες unban από το %s.

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC κανάλι [topic]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR channel what
CHAN_CLEARED_BANS
	Όλα τα bans από το κανάλι %s έχουν απομακρυνθεί.
CHAN_CLEARED_EXCEPTS
	Όλα τα excepts από το κανάλι %s έχουν απομακρυνθεί.
CHAN_CLEARED_MODES
	Όλα τα modes από το κανάλι %s έχουν σβηστεί.
CHAN_CLEARED_OPS
	Όλα τα Mode +o έχει καθαριστεί από το κανάλι %s.
CHAN_CLEARED_HOPS
	Mode +h has been cleared from channel %s.
CHAN_CLEARED_VOICES
	Όλα τα Mode +v έχει καθαριστεί από το κανάλι %s.
CHAN_CLEARED_USERS
	Όλοι οι χρήστες έγιναν kick από το κανάλι %s.
CHAN_CLEARED_INVITES
	All invites on channel %s have been removed.

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS κανάλι
CHAN_GETPASS_UNAVAILABLE
	Η εντολή GETPASS δεν είναι διαθέσιμη γιατί η κωδικοποίηση είναι σε λειτουργία.
CHAN_GETPASS_PASSWORD_IS
	Ο κωδικός για το κανάλι %s είναι %s.
	
# SENDPASS responses
CHAN_GETKEY_SYNTAX
	GETKEY channel
CHAN_GETKEY_NOKEY
	The channel %s has no key.
CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS κανάλι
CHAN_SENDPASS_UNAVAILABLE
	Η εντολή SENDPASS δεν είναι διαθέσιμη γιατί η κωδικοποίηση είναι σε λειτουργία.
CHAN_SENDPASS_SUBJECT
	Ο κωδικός του καναλιού (%s)
CHAN_SENDPASS_HEAD
	Γεια,
CHAN_SENDPASS_LINE_1
	Μας ειδοποίησες να σου στείλουμε τον κωδικό του καναλιού %s μέσω e-mail.
CHAN_SENDPASS_LINE_2
	Ο κωδικός είναι %s. Για περισσότερη ασφάλεια, θα πρέπει να το αλλάξεις το γρηγορόρτερο από τη στιγμή που έλαβες αυτό το e-mail.
CHAN_SENDPASS_LINE_3
	Αν δεν ξέρεις γιατί σου στάλθηκε αυτό το mail, παρακαλώ αγνοησέ το.
CHAN_SENDPASS_LINE_4
	ΠΑΡΑΚΑΛΩ ΜΗΝ ΑΠΑΝΤΗΣΕΙΣ ΣΕ ΑΥΤΟ ΤΟ MAIL!
CHAN_SENDPASS_LINE_5
	%s administrators.
CHAN_SENDPASS_OK
	Ο κωδικός του %s στάλθηκε επιτυχώς.

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID κανάλι [λόγος]
CHAN_FORBID_SYNTAX_REASON
	FORBID κανάλι λόγος
CHAN_FORBID_SUCCEEDED
	Το κανάλι %s παρακρατείτε.
CHAN_FORBID_FAILED
	Δεν μπορεί να παρακρατηθεί το κανάλι %s!

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
	Έχεις 1 νέο μήνυμα.
MEMO_HAVE_NEW_MEMOS
	Έχεις %d νέα μηνύματα.
MEMO_TYPE_READ_LAST
	Γράψε %R%s READ LAST για να το διαβάσεις.
MEMO_TYPE_READ_NUM
	Γράψε %R%s READ %d για να διαβάσεις.
MEMO_TYPE_LIST_NEW
	Γράψε %R%s LIST NEW για να δεις την λίστα με τα μηνύματα.
MEMO_AT_LIMIT
	Προειδοποίηση: Έχει φτάσει στο μέγιστο νούμερο μηνυμάτων (%d).  Δεν θα λάβεις καινούργια μηνύματα αν δεν σβήσεις μερικά από τα παλιά που έχεις.
MEMO_OVER_LIMIT
	Προειδοποίηση: Έχεις ξεπεράσει το νούμερο μηνυμάτων που μπορείς να λάβεις (%d).  Δεν θα μπορείς να λάβεις άλλα αν δεν σβήσεις μερικά από τα παλιά μηνύματα που έχεις.
MEMO_X_MANY_NOTICE
	There are %d memos on channel %s.
MEMO_X_ONE_NOTICE
	There is %d memo on channel %s.
MEMO_NEW_X_MEMO_ARRIVED
	There is a new memo on channel %s.
	Type %R%s READ %s %d to read it.
MEMO_NEW_MEMO_ARRIVED
	Έχεις ένα νέο μήνυμα από %s.
	Γράψε %R%s READ %d για να το διαβάσεις.

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	Δεν έχεις μηνύματα.
MEMO_X_HAS_NO_MEMOS
	%s δεν έχεις μηνύματα.
MEMO_DOES_NOT_EXIST
	Μηνύματα %d δεν υπάρχουν!
MEMO_LIST_NOT_FOUND
	Δεν βρέθηκαν μηνύματα.

# SEND responses
MEMO_SEND_SYNTAX
	SEND {ψευδώνυμο | κανάλι} περιεχόμενο
MEMO_SEND_DISABLED
	Συγνώμη, τα μηνύματα δεν μπορούν να σταλούν γιατί είναι ανενεργή η υπηρεσία.
MEMO_SEND_PLEASE_WAIT
	Παρακαλώ περίμενε %d δευτερόλεπτα πριν χρησιμοποιήσεις την εντολή SEND ξανά.
MEMO_X_GETS_NO_MEMOS
	%s δεν μπορείς να λάβεις άλλα μηνύματα.
MEMO_X_HAS_TOO_MANY_MEMOS
	%s έχεις πολλά μηνύματα και δεν μπορείς να λάβεις άλλα.
MEMO_SENT
	Το μήνυμα στάλθηκε στον %s.
	
# CANCEL responses
MEMO_MASS_SENT
	A mass memo has been sent to all registered users.
	
# STAFF responses
MEMO_STAFF_SYNTAX
	STAFF memo-text
	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {ψευδώνυμο | κανάλι}
MEMO_CANCEL_DISABLED
	Συγνώμη, η ακύρωση μηνύματος είναι εκτός λειτουργίας.
MEMO_CANCEL_NONE
	Κανένα μήνυμα δεν ακυρώθηκε.
MEMO_CANCELLED
	Το τελευταίο μήνυμα που πήγαινε για τον %s ακυρώθηκε.

# LIST responses
MEMO_LIST_SYNTAX
	LIST [κανάλι] [list | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	Δεν έχεις νέα μηνύματα.
MEMO_X_HAS_NO_NEW_MEMOS
	Ο/η %s δεν έχει κανένα νέο μήνυμα.
MEMO_LIST_MEMOS
	Μήνυμα για τον %s.  Για να το διαβάσεις, γράψε: %R%s READ num
MEMO_LIST_NEW_MEMOS
	Νέα μηνύματα για τον %s.  Για να τα διαβάσεις, γράψε: %R%s READ num
MEMO_LIST_CHAN_MEMOS
	Μηνύματα για τον %s.  Για να τα διαβάσεις, γράψε: %R%s READ %s num
MEMO_LIST_CHAN_NEW_MEMOS
	Νέα μηνύματα για το %s.  Για να τα διαβάσεις, γράψε: %R%s READ %s num
MEMO_LIST_HEADER
	 Νούμερο  Αποστολέας            Ημερομηνία/Ώρα
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [κανάλι] {list | LAST | NEW}
MEMO_HEADER
	Μήνυμα %d από το %s (%s).  Για να το σβήσεις, γράψε: %R%s DEL %d
MEMO_CHAN_HEADER
	Μήνυμα %d από %s (%s).  Για να το σβήσεις, γράψε: %R%s DEL %s %d
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [κανάλι] {νου | list | ALL}
MEMO_DELETED_NONE
	Κανένα μήνυμα δεν διαγράφηκε.
MEMO_DELETED_ONE
	Μήνυμα %d διαγράφηκε.
MEMO_DELETED_SEVERAL
	Μηνύματα %s διαγράφηκαν.
MEMO_DELETED_ALL
	Όλα τα μηνύματα διαγράφηκαν.

# SET responses
MEMO_CHAN_DELETED_ALL
	All memos for channel %s have been deleted.

# SET responses
MEMO_SET_SYNTAX
	SET επιλογή παράμετροι
MEMO_SET_DISABLED
	Συγνώμη, η εντολή setting είναι απενεργοποιημένη.
MEMO_SET_UNKNOWN_OPTION
	’γνωστη SET επιλογή %s.

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
MEMO_SET_NOTIFY_ON
	%s από τώρα όταν θα κάνεις εντολή logon θα βλέπεις τα μηνύματα που θα σου στέλνουν καθώς θασαι στο δίκτυο.
MEMO_SET_NOTIFY_LOGON
	%s όταν θα κάνεις log on ή θα επανέρχεσαι από away θα λαμβάνεις τα μηνύματα.
MEMO_SET_NOTIFY_NEW
	%s θα σου γίνεται παρατήρηση οτι έρχονται μηνύματα από την υπηρεσία.
MEMO_SET_NOTIFY_OFF
	%s δεν θα σου έρχεται καμία παρατήρηση από την υπηρεσία.

# SET LIMIT responses
MEMO_SET_NOTIFY_MAIL
	You will now be informed about new memos via email.
MEMO_SET_NOTIFY_NOMAIL
	You will no longer be informed via email.
MEMO_SET_NOTIFY_INVALIDMAIL
	There's no email address set for your nick.

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [κανάλι] όριο
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [χρήστης | κανάλι] {όριο | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	Δεν έχεις το δικαίωμα να αλλάξεις το όριο των μηνυμάτων που θα λαμβάνεις.
MEMO_SET_LIMIT_FORBIDDEN
	Το όριο των μηνυμάτων για τον %s δεν μπορεί να αλλαχτεί.
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	Δεν μπορείς να θέσεις ως όριο μηνυμάτων μεγαλύτερο από %d.
MEMO_SET_LIMIT_TOO_HIGH
	Δεν μπορείς να θέσεις ως όριο μηνυμάτων για τον %s μεγαλύτερο από %d.
MEMO_SET_LIMIT_OVERFLOW
	Το όριο μηνυμάτων είναι πολύ μεγάλο; οριοθέτησέ το %d συμπεριλαμβανομένου.
MEMO_SET_YOUR_LIMIT
	Το όριο μηνυμάτων άλλαξε σε %d.
MEMO_SET_YOUR_LIMIT_ZERO
	Δεν θα είσαι ικανός να λάβεις άλλα μηνύματα.
MEMO_UNSET_YOUR_LIMIT
	Το όριο μηνυμάτων είναι απενεργοποιημένο.
MEMO_SET_LIMIT
	Το όριο μηνυμάτων για τον %s άλλαξε σε %d.
MEMO_SET_LIMIT_ZERO
	Το όριο μηνυμάτων για τον %s άλλαξε σε 0.
MEMO_UNSET_LIMIT
	Το όριο μηνυμάτων απενεργοποιήθηκε για τον %s.

# INFO responses
MEMO_INFO_SYNTAX
	INFO [κανάλι]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [ψευδώνυμο | κανάλι]
MEMO_INFO_NO_MEMOS
	Μέχρις στιγμής δεν έχεις κανένα μήνυμα.
MEMO_INFO_MEMO
	Μέχρι στιγμής έχεις 1 μήνυμα.
MEMO_INFO_MEMO_UNREAD
	Μέχρι στιγμής έχεις 1 memo, και δεν το έχεις διαβάσει.
MEMO_INFO_MEMOS
	Μέχρι στιγμής έχεις %d μηνύματα.
MEMO_INFO_MEMOS_ONE_UNREAD
	Μέχρι στιγμής έχεις %d μηνύματα, εκ των οποίων 1 δεν έχει διαβαστεί.
MEMO_INFO_MEMOS_SOME_UNREAD
	Μέχρι στιγμής έχεις %d μηνύματα, εκ των οποίων %d δεν έχουν διαβαστεί.
MEMO_INFO_MEMOS_ALL_UNREAD
	Μέχρι στιγμής έχεις %d μηνύματα; εκ των οποίων όλα δεν έχουν διαβαστεί.
MEMO_INFO_LIMIT
	Το όριο μηνυμάτων είναι %d.
MEMO_INFO_HARD_LIMIT
	Το όριο μηνυμάτων είναι %d, και δεν μπορεί να αλλαχτεί.
MEMO_INFO_LIMIT_ZERO
	Το όριο μηνυμάτων είναι 0; δεν θα λάβεις ποτέ κανένα άλλο μήνυμα.
MEMO_INFO_HARD_LIMIT_ZERO
	Το όριο μηνυμάτων είναι 0; δεν θα λάβεις ποτέ κανένα άλλο μήνυμα.  Δεν μπορείς να αλλάξεις αυτό το όριο.
MEMO_INFO_NO_LIMIT
	Δεν έχεις θέσει κανένα αριθμό στο όριο ωστε να κρατήσεις μηνύματα.
MEMO_INFO_NOTIFY_OFF
	Δεν θα σου γίνει παρατήρηση για νέα μηνύματα.
MEMO_INFO_NOTIFY_ON
	Θα ειδοποιηθείς για νέα μηνύματα όταν κάνεις logon και όταν φτάσουν.
MEMO_INFO_NOTIFY_RECEIVE
	Θα ειδοποιηθείς για νέα μηνύματα όταν θα φτάσουν.
MEMO_INFO_NOTIFY_SIGNON
	Θα ειδοποιηθείς για νέα μηνύματα στο logon.
MEMO_INFO_X_NO_MEMOS
	%s δεν έχεις κανένα μήνυμα.
MEMO_INFO_X_MEMO
	%s έχεις 1 μήνυμα..
MEMO_INFO_X_MEMO_UNREAD
	%s έχεις 1 μήνυμα, και δεν το έχεις διαβάσει ακόμα.
MEMO_INFO_X_MEMOS
	%s έχεις %d μηνύματα.
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s έχεις %d μηνύματα, εκ των οποίων 1 δεν έχει διαβαστεί.
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s έχεις %d μηνύματα, εκ των οποίων %d δεν έχουν διαβαστεί.
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s έχεις %d μηνύματα; εκ των οποίων όλα δεν έχουν διαβαστεί.
MEMO_INFO_X_LIMIT
	Του/της %s's το όριο μηνυμάτων είναι %d.
MEMO_INFO_X_HARD_LIMIT
	%s's όριο μηνυμάτων είναι %d, και δεν μπορεί να αλλαχτεί.
MEMO_INFO_X_NO_LIMIT
	%s δεν έχεις όριο μηνυμάτων.
MEMO_INFO_X_NOTIFY_OFF
	%s δεν θα παρατηρηθεί για νέα μηνύματα.
MEMO_INFO_X_NOTIFY_ON
	%s θα παρατηρηθεί για νέα μηνύματα στο logon και όταν θα φτάσουν.
MEMO_INFO_X_NOTIFY_RECEIVE
	%s θα παρατηρηθεί όταν τα νέα μηνύματα θα φτάσουν.
MEMO_INFO_X_NOTIFY_SIGNON
	%s θα παρατηρηθεί για νέα μηνύματα στο logon.
	
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
	Το Bot %s δεν υπάρχει.
BOT_NOT_ASSIGNED
	Πρέπει να κάνεις assign στο bot για να μπεί στο κανάλι.
	Γράψε %R%S HELP ASSIGN για περισσότερες πληροφορίες.
BOT_NOT_ON_CHANNEL
	Το Bot δεν είναι στο κανάλι %s.
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	Μην χρησιμοποιείς αυτή τη λέξη %s στο κανάλι!
BOT_REASON_BADWORD_GENTLE
	Πρόσεχε την γλώσσα σου!
BOT_REASON_BOLD
	Μην χρησιμοποιείς έντονους χαρακτήρες μέσα στο κανάλι!
BOT_REASON_CAPS
	Μην γράφεις με κεφαλαία γράμματα!
BOT_REASON_COLOR
	Μην χρησιμοποιείς χρώματα μέσα στο κανάλι!
BOT_REASON_FLOOD
	Μην επαναλαμβάνεις σε πολλές σειρές!
BOT_REASON_REPEAT
	Μην επαναλαμβάνεις τα ίδια!
BOT_REASON_REVERSE
	Μην είσαι αντίθετος μέσα στο κανάλι!
BOT_REASON_UNDERLINE
	Μην χρησιμοποιείς χαρακτήρες υπογράμμισης στο κανάλι!
	
# !seen replies
BOT_SEEN_BOT
	Με βρήκες, %s!
BOT_SEEN_YOU
	Ψάχνεις για τον εαυτό σου, ε %s?
BOT_SEEN_ON_CHANNEL
	%s δεν είναι στο κανάλι αυτή τη στιγμή!
BOT_SEEN_ON_CHANNEL_AS
	%s είναι στο κανάλι αυτή τη στιγμή (as %s) ! 
BOT_SEEN_ON
	%s ήταν εδώ την τελευταία φορά πριν %s.
BOT_SEEN_NEVER
	Δεν έχω δει κανέναν %s σε αυτό το κανάλι.
BOT_SEEN_UNKNOWN
	Δεν ξέρω κανέναν %s ποιος είναι.

# BOT responses
BOT_BOT_SYNTAX
	BOT ADD nick user host real
	BOT CHANGE oldnick newnick [user [host [real]]]
	BOT DEL nick
BOT_BOT_ALREADY_EXISTS
	Bot %s υπάρχει ήδη.
BOT_BOT_CREATION_FAILED
	Συγνώμη, η δημιουργία του bot απέτυχε.
BOT_BOT_READONLY
	Συγνώμη, η υπηρεσία για το bot είναι απενεργοποιημένη.
BOT_BOT_ADDED
	%s!%s@%s (%s) προσθέθηκε στου bot την λίστα.
BOT_BOT_ANY_CHANGES
	Οι παλιές πληροφορίες είναι ίδιες με τις καινούργιες.
BOT_BOT_CHANGED
	Bot %s άλλαξε σε %s!%s@%s (%s)
BOT_BOT_DELETED
	Bot %s διαγράφηκε.
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	Bot λίστα:
BOT_BOTLIST_PRIVATE_HEADER
	Bots είναι μόνο για IRC operators:
BOT_BOTLIST_FOOTER
	%d bots διαθέσιμα.
BOT_BOTLIST_EMPTY
	Δεν υπάρχουν διαθέσιμα bots αυτή τη στιγμή.
	Ρώτα τον Services admin να σου φτιάξει ένα!
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN chan nick
BOT_ASSIGN_READONLY
	Συγνώμη, η υπηρεσία bot assignment είναι απενεργοποιημένη.
BOT_ASSIGN_ALREADY
	Bot %s is already assigned to channel %s.
BOT_ASSIGN_ASSIGNED
	Το Bot %s θα μπει στο %s.
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN chan
BOT_UNASSIGN_UNASSIGNED
	Δεν υπάρχει bot που να είναι στο %s τώρα πια.
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {chan | nick}
BOT_INFO_NOT_FOUND
	%s δεν είναι σωστό bot ή κατοχυρωμένο κανάλι.
BOT_INFO_BOT_HEADER
	Πληροφορίες για το bot %s:
BOT_INFO_BOT_MASK
	       Μάσκα : %s@%s
BOT_INFO_BOT_REALNAME
	  Πραγματικό όνομα : %s
BOT_INFO_BOT_CREATED
	    Δημιουργήθηκε : %s
BOT_INFO_BOT_USAGE
	    Χρησιμοποιείται στο/α : %d κανάλι/α
BOT_INFO_BOT_OPTIONS
	    Επιλογές : %s
BOT_INFO_OPT_PRIVATE
	Μυστικό
BOT_INFO_CHAN_HEADER
	Πληροφορίες για το κανάλι %s:
BOT_INFO_CHAN_BOT
	            Όνομα του Bot : %s
BOT_INFO_CHAN_BOT_NONE
	           Όνομα του bot : δεν υπάρχει για την ώρα.
BOT_INFO_CHAN_KICK_BADWORDS
	   Κακές λέξεις ώστε να κάνει kick : %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	   Κακές λέξεις ώστε να κάνει kick : %s (%d kick(s) to ban)
BOT_INFO_CHAN_KICK_BOLDS
	       Έντονη χαρακτήρες με kick : %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	       Έντονη χαρακτήρες με kick : %s (%d kick(s) to ban)
BOT_INFO_CHAN_KICK_CAPS_ON
	        Κεφαλαία γράμματα με kick : %s (minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	        Κεφαλαία γράμματα με kick : %s (%d kick(s) to ban; minimum %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	        Κεφαλαία γράμματα με kick : %s
BOT_INFO_CHAN_KICK_COLORS
	      Χρώματα με kick : %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	      Χρώματα με kick : %s (%d kick(s) to ban)
BOT_INFO_CHAN_KICK_FLOOD_ON
	       Επανάληψη με kick : %s (%d lines in %ds)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	       Επανάληψη με kick : %s (%d kick(s) to ban; %d lines in %ds)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	       Επανάληψη με kick : %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	      Επανάληψη με kick : %s (%d times)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	      Επανάληψη με kick : %s (%d kick(s) to ban; %d times)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	      Επανάληψη με kick : %s
BOT_INFO_CHAN_KICK_REVERSES
	    Αντιθετος με kick : %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	    Αντιθετος kick : %s (%d kick(s) to ban)
BOT_INFO_CHAN_KICK_UNDERLINES
	  Υπογράμμιση με kick : %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	  Υπογράμμιση με kick : %s (%d kick(s) to ban)
BOT_INFO_ACTIVE
	ενεργοποιημένο
BOT_INFO_INACTIVE
	απενεργοποιημένο
BOT_INFO_CHAN_OPTIONS
	            Επιλογές : %s
BOT_INFO_OPT_DONTKICKOPS
	Προστασία στους Ops
BOT_INFO_OPT_DONTKICKVOICES
	Προστασία στους Voices
BOT_INFO_OPT_FANTASY
	Φαντασία
BOT_INFO_OPT_GREET
	Χαιρετισμός
BOT_INFO_OPT_NOBOT
	Κανένα bot
BOT_INFO_OPT_SYMBIOSIS
	Συμβίωση
BOT_INFO_OPT_NONE
	Κανένα
	
# SET responses
BOT_SET_SYNTAX
	SET channel option settings
BOT_SET_DISABLED
	Συγνώμη, η υπηρεσία bot είναι απενεργοποιημένη.
BOT_SET_UNKNOWN
	’γνωστη επιλογή %s.
	Γράψε %R%S HELP SET για περισσότερες πληροφορίες.
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET channel DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	Το Bot δεν θα κάνει kick τουε ops του καναλιού %s.
BOT_SET_DONTKICKOPS_OFF
	Το Bot θα κάνει kick τους ops του καναλιού %s.
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET channel DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	Το Bot δεν θα κάνει kick τους voices του καναλιού %s.
BOT_SET_DONTKICKVOICES_OFF
	Το Bot θα κάνει kick τους voices του καναλιού %s.

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET channel FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	Η Fantasy επιλογή είναι τώρα ON στο κανάλι %s.
BOT_SET_FANTASY_OFF
	Η Fantasy επιλογή είναι τωρα OFF στο κανάλι %s.
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET channel GREET {ON|OFF}
BOT_SET_GREET_ON
	Η επιλογή χαιρετισμού είναι ON στο κανάλι %s.
BOT_SET_GREET_OFF
	Η επιλογή χαιρετισμού είναι OFF στο κανάλι %s.
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET channel NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	Κανένα Bot έχει επιλογή ON στο κανάλι %s.
BOT_SET_NOBOT_OFF
	Κανένα Bot έχει επιλογή OFF στο κανάλι %s.

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET channel PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	Μυστικότητα επιλογής του bot %s είναι τώρα ON.
BOT_SET_PRIVATE_OFF
	Μυστικότητα επιλογής του bot %s είναι τώρα OFF.

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET channel SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	Επιλογή συμβίωσης είναι τώρα ON στο κανάλι %s.
BOT_SET_SYMBIOSIS_OFF
	Επιλογή συμβίωσης είναι τώρα OFF στο κανάλι %s.
	
# KICK responses
BOT_KICK_SYNTAX
	KICK channel option {ON|OFF} [settings]
BOT_KICK_DISABLED
	Συγνώμη, η υπηρεσία kicker είναι απενεργοποιημένη.
BOT_KICK_UNKNOWN
	’γνωστη επιλογή %s.
	Γράψε %R%S HELP KICK για περισσότερες πληροφορίες.
BOT_KICK_BAD_TTB
	%s δεν μπορεί να πάρει τόσο χρόνο τo ban.

BOT_KICK_BADWORDS_ON
	Το Bot από τώρα θα κάνει kick κακές λέξεις. Χρησιμοποίησε την BADWORDS εντολή
	για να προσθέσεις ή να αφαιρέσεις κακές λέξεις.
BOT_KICK_BADWORDS_ON_BAN
	Το Bot από τώρα θα κάνει kick κακές λέξεις, και θα κάνει και ban μετά από 
	%d kicks για τον ίδιο χρήστη. Χρησιμοποίησε την BADWORDS εντολή
	για να προσθέσεις ή να αφαιρέσεις κακές λέξεις.
BOT_KICK_BADWORDS_OFF
	Το Bot δεν θα κάνει kick κακές λέξεις πλέον.
BOT_KICK_BOLDS_ON
	Το Bot θα κάνει τώρα kick έντονους χαρακτήρες.
BOT_KICK_BOLDS_ON_BAN
	Το Bot θα κάνει kick έντονους χαρακτήρες, και θα κάνει ban μετά  από 
	%d kicks για τον ίδιο χρήστη.
BOT_KICK_BOLDS_OFF
	Το Bot δεν θα κάνει kick έντονους χαρακτήρες πλέον.
BOT_KICK_CAPS_ON
	Το Bot θα κάνει kick κεφαλαία (πρέπει να είναι τουλάχιστον
	%d χαρακτήρες και %d%% από το μήνυμα).
BOT_KICK_CAPS_ON_BAN
	Το Bot θα κάνει kick κεφαλαία (πρέπει να είναι τουλάχιστον
	%d χαρακτήρες και %d%% από το μήνυμα), και θα 
	γίνει ban μετά από %d kicks για τον ίδιο χρήστη.
BOT_KICK_CAPS_OFF
	Το Bot δεν θα κάνει kick κεφαλαία πλέον.
BOT_KICK_COLORS_ON
	Το Bot θα κάνει kick για χρώματα.
BOT_KICK_COLORS_ON_BAN
	Το Bot θα κάνει kick για χρώματα, και θα κάνει ban μετά από %d 
	kicks για τον ίδιο χρήστη.
BOT_KICK_COLORS_OFF
	Το Bot δεν θα κάνει kick για χρώματα πλέον.
BOT_KICK_FLOOD_ON
	Το Bot θα κάνει kick για επαναλήψεις (%d γραμμές σε %d δευτερόλεπτα).
BOT_KICK_FLOOD_ON_BAN
	Το Bot θα κάνει kick για επαναλήψεις (%d γραμμές σε %d δευτερόλεπτα), και 
	θα κάνει ban μετά από %d kicks για τον ίδιο χρήστη.
BOT_KICK_FLOOD_OFF
	Το Bot δεν θα κάνει kick για επαναλήψεις πλέον.
BOT_KICK_REPEAT_ON
	Το Bot θα κάνει kick για επαναλήψεις (χρήστες που λένε %d φορές
	το ίδιο πράγμα).
BOT_KICK_REPEAT_ON_BAN
	Το Bot θα κάνει kick για επαναλήψεις (χρήστες που λένε %d φορές
	το ίδιο πράγμα), και θα γίνεται ban μετά από %d 
	kicks για τον ίδιο χρήστη.
BOT_KICK_REPEAT_OFF
	Το Bot δεν θα κάνει kick για επαναλήψεις πλέον.
BOT_KICK_REVERSES_ON
	Το Bot θα κάνει kick για reserves.
BOT_KICK_REVERSES_ON_BAN
	Το Bot θα κάνει kick για reverses, και θα κάνει ban μετά από %d 
	kicks για τον ίδιο χρήστη.
BOT_KICK_REVERSES_OFF
	Το Bot δεν θα κάνει kick για reverses πλέον.
BOT_KICK_UNDERLINES_ON
	Το Bot θα κάνει kick για υπογραμμισμένους χαρακτήρες.
BOT_KICK_UNDERLINES_ON_BAN
	Το Bot θα κάνει kick για υπογραμμισμένους χαρακτήρες, και θα κάνει ban μετά από %d 
	kicks για τον ίδιο χρήστη.
BOT_KICK_UNDERLINES_OFF
	Το Bot δεν θα κάνει kick για υπογραμμισμένους χαρακτήρες πλέον.
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS channel {ADD|DEL|LIST|CLEAR} [word | entry-list] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	Συγνώμη, η υπηρεσία bad words είναι απενεργοποιημένη.
BOT_BADWORDS_REACHED_LIMIT
	Συγνώμη, μπορείς μόνο να έχεις %d κακές λέξεις για ένα κανάλι.
BOT_BADWORDS_ALREADY_EXISTS
	%s έχεις ήδη %s κακές λέξεις.
BOT_BADWORDS_ADDED
	%s προσθέθηκε στην λίστα των κακών λέξεων του %s.
BOT_BADWORDS_NO_SUCH_ENTRY
	Δεν υπάρχει αυτή η θέση (#%d) στη λίστα των κακών λέξεων του %s.
BOT_BADWORDS_NOT_FOUND
	%s δεν βρέθηκε στη λίστα των κακών λέξεων του %s.
BOT_BADWORDS_NO_MATCH
	Δεν βρέθηκαν οι θέσεις στην λίστα των κακών λέξεων του %s.
BOT_BADWORDS_DELETED
	%s διαγράφτηκε από την λίστα των κακών λέξεων απο το %s.
BOT_BADWORDS_DELETED_ONE
	Διαγράφηκε 1 θέση από του %s λίστα κακών λέξεων.
BOT_BADWORDS_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από το %s ,τη λίστα των κακών λέξεων.
BOT_BADWORDS_LIST_EMPTY
	Η λίοστα των κακών λέξεων του %s είναι άδεια.
BOT_BADWORDS_LIST_HEADER
	Η λίστα με τις κακές λέξεις του %s είναι:
	  Νούμ   Λέξη                           Τύπος
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	Η λίστα των κακών λέξεων είναι άδεια.

# SAY responses
BOT_SAY_SYNTAX
	SAY channel text

# ACT responses
BOT_ACT_SYNTAX
	ACT channel text

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
	Οι υπηρεσίες αδυνατούν να αλλάξουν τις επιλογές.  Οι servers είναι σωστά φτιαγμένοι?
OPER_BOUNCY_MODES_U_LINE
	Οι υπηρεσίες αδυνατούν να αλλάξουν τις επιλογές.  Οι servers' U: γραμμές είναι οι σωστές?

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL message

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	’γνωστη STATS επιλογή %s..
OPER_STATS_CURRENT_USERS
	Οι ως τώρα χρήστες: %d (%d ops)
OPER_STATS_MAX_USERS
	Μέγιστοι χρήστες: %d (%s)
# This is ugly, but at least it's language-portable...
# Note that you can include seconds in any of these--my personal preference
# is to only give resolution to the nearest minute when uptime is one hour
# or more, but just add a %d to get seconds as well.
OPER_STATS_UPTIME_DHMS
	Οι υπηρεσίες είναι συνδεδεμένες %d μέρες, %02d:%02d
OPER_STATS_UPTIME_1DHMS
	Οι υπηρεσίες είναι συνδεδεμένες %d μέρες, %02d:%02d
OPER_STATS_UPTIME_HMS
	Οι υπηρεσίες είναι συνδεδεμένες %d ώρες, %d λεπτά
OPER_STATS_UPTIME_HM1S
	Οι υπηρεσίες είναι συνδεδεμένες %d ώρες, %d λεπτά
OPER_STATS_UPTIME_H1MS
	Οι υπηρεσίες είναι συνδεδεμένες %d ώρες, %d λεπτό
OPER_STATS_UPTIME_H1M1S
	Οι υπηρεσίες είναι συνδεδεμένες %d ώρες, %d λεπτό
OPER_STATS_UPTIME_1HMS
	Οι υπηρεσίες είναι συνδεδεμένες %d ώρα, %d λεπτά
OPER_STATS_UPTIME_1HM1S
	Οι υπηρεσίες είναι συνδεδεμένες %d ώρα, %d λεπτά
OPER_STATS_UPTIME_1H1MS
	Οι υπηρεσίες είναι συνδεδεμένες %d ώρα, %d λεπτό
OPER_STATS_UPTIME_1H1M1S
	Οι υπηρεσίες είναι συνδεδεμένες %d ώρα, %d λεπτό
OPER_STATS_UPTIME_MS
	Οι υπηρεσίες είναι συνδεδεμένες %d λεπτά, %d δευτερόλεπτα
OPER_STATS_UPTIME_M1S
	Οι υπηρεσίες είναι συνδεδεμένες %d λεπτά, %d δευτερόλεπτο
OPER_STATS_UPTIME_1MS
	Οι υπηρεσίες είναι συνδεδεμένες %d λεπτό, %d δευτερόλεπτα
OPER_STATS_UPTIME_1M1S
	Οι υπηρεσίες είναι συνδεδεμένες %d λεπτό, %d δευτερόλεπτο
OPER_STATS_BYTES_READ
	Διαβασμένα Bytes   : %5d kB
OPER_STATS_BYTES_WRITTEN
	Γραμμένα Bytes : %5d kB
OPER_STATS_USER_MEM
	Χρήστης          : %6d βάσεις, %5d kB
OPER_STATS_CHANNEL_MEM
	Κανάλια       : %6d βάσεις, %5d kB
OPER_STATS_GROUPS_MEM
	NS Ομάδες     : %6d βάσεις, %5d kB
OPER_STATS_ALIASES_MEM
	NS Aliases    : %6d βάσεις, %5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ      : %6d βάσεις, %5d kB
OPER_STATS_BOTSERV_MEM
	BotServ       : %6d βάσεις, %5d kB
OPER_STATS_HOSTSERV_MEM
	HostServ      : %6d records, %5d kB
OPER_STATS_OPERSERV_MEM
	OperServ      : %6d βάσεις, %5d kB
OPER_STATS_SESSIONS_MEM
	Sessions      : %6d βάσεις, %5d kB
OPER_STATS_AKILL_COUNT
	Ως τώρα υπάρχουν AKILLs: %d
OPER_STATS_AKILL_EXPIRE_DAYS
	Εξαρχής χρόνος λήξης AKILL: %d μέρες
OPER_STATS_AKILL_EXPIRE_DAY
	Εξαρχής χρόνος λήξης AKILL: 1 μέρα
OPER_STATS_AKILL_EXPIRE_HOURS
	Εξαρχής χρόνος λήξης AKILL: %d ώρες
OPER_STATS_AKILL_EXPIRE_HOUR
	Εξαρχής χρόνος λήξης AKILL: 1 ώρα
OPER_STATS_AKILL_EXPIRE_MINS
	Εξαρχής χρόνος λήξης AKILL: %d λεπτά
OPER_STATS_AKILL_EXPIRE_MIN
	Εξαρχής χρόνος λήξης AKILL: 1 λεπτό
OPER_STATS_AKILL_EXPIRE_NONE
	Εξαρχής χρόνος λήξης AKILL: Δεν λήγει ποτέ
OPER_STATS_SGLINE_COUNT
	Ως τώρα υπάρχουν SGLINEs: %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	Εξαρχής χρόνος λήξης SGLINE: %d μέρες
OPER_STATS_SGLINE_EXPIRE_DAY
	Εξαρχής χρόνος λήξης SGLINE: 1 μέρα
OPER_STATS_SGLINE_EXPIRE_HOURS
	Εξαρχής χρόνος λήξης SGLINE: %d ώρες
OPER_STATS_SGLINE_EXPIRE_HOUR
	Εξαρχής χρόνος λήξης SGLINE: 1 ώρα
OPER_STATS_SGLINE_EXPIRE_MINS
	Εξαρχής χρόνος λήξης SGLINE: %d λεπτά
OPER_STATS_SGLINE_EXPIRE_MIN
	Εξαρχής χρόνος λήξης SGLINE: 1 λεπτό
OPER_STATS_SGLINE_EXPIRE_NONE
	Εξαρχής χρόνος λήξης SGLINE: Δεν λήγει ποτέ
OPER_STATS_SQLINE_COUNT
	Ως τώρα υπάρχουν SQLINEs: %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	Εξαρχής χρόνος λήξης SQLINE: %d μέρες
OPER_STATS_SQLINE_EXPIRE_DAY
	Εξαρχής χρόνος λήξης SQLINE: 1 μέρα
OPER_STATS_SQLINE_EXPIRE_HOURS
	Εξαρχής χρόνος λήξης SQLINE: %d ώρες
OPER_STATS_SQLINE_EXPIRE_HOUR
	Εξαρχής χρόνος λήξης SQLINE: 1 ώρα
OPER_STATS_SQLINE_EXPIRE_MINS
	Εξαρχής χρόνος λήξης SQLINE: %d λεπτά
OPER_STATS_SQLINE_EXPIRE_MIN
	Εξαρχής χρόνος λήξης SQLINE: 1 λεπτό
OPER_STATS_SQLINE_EXPIRE_NONE
	Εξαρχής χρόνος λήξης SQLINE: Δεν λήγει ποτέ
OPER_STATS_SZLINE_COUNT
	Ως τώρα υπάρχουν SZLINEs: %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	Εξαρχής χρόνος λήξης SZLINE: %d μέρες
OPER_STATS_SZLINE_EXPIRE_DAY
	Εξαρχής χρόνος λήξης SZLINE: 1 μέρα
OPER_STATS_SZLINE_EXPIRE_HOURS
	Εξαρχής χρόνος λήξης SZLINE: %d ώρες
OPER_STATS_SZLINE_EXPIRE_HOUR
	Εξαρχής χρόνος λήξης SZLINE: 1 ώρα
OPER_STATS_SZLINE_EXPIRE_MINS
	Εξαρχής χρόνος λήξης SZLINE: %d λεπτά
OPER_STATS_SZLINE_EXPIRE_MIN
	Εξαρχής χρόνος λήξης SZLINE: 1 λεπτό
OPER_STATS_SZLINE_EXPIRE_NONE
	Εξαρχής χρόνος λήξης SZLINE: Δεν λήγει ποτέ
OPER_STATS_RESET
	Τα στατιστικά σβήστηκαν.

# MODE responses
OPER_STATS_UPLINK_SERVER
	Uplink server: %s
OPER_STATS_UPLINK_CAPAB
	Uplink capab: %s
OPER_STATS_UPLINK_SERVER_COUNT
	Servers found: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE channel modes

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
	CLEARMODES channel [ALL]
OPER_CLEARMODES_DONE
	Όλα τα modes και τα bans καθαρίστηκαν από το κανάλι %s.
OPER_CLEARMODES_ALL_DONE
	Όλα τα modes καθαρίστηκαν από το κανάλι %s.


# KICK responses
OPER_KICK_SYNTAX
	KICK channel user reason
	
# ADMIN responses
OPER_SVSNICK_SYNTAX
	SVSNICK nick newnick 
OPER_SVSNICK_NEWNICK
	The nick %s is now being changed to %s.
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [nick | entry-list]
OPER_ADMIN_SKELETON
	Οι υπηρεσίες είναι σε skeleton mode; Η εντολή ADMIN δεν είναι διαθέσιμη.
OPER_ADMIN_EXISTS
	Ο/η %s υπάρχει ήδη στην λίστα των υπηρεσιών.
OPER_ADMIN_REACHED_LIMIT
	Συγνώμη, μπορείς να έχεις στην λίστα των υπηρεσιών μόνο %d.
OPER_ADMIN_ADDED
	%s προσθέθηκε στην λίστα των υπηρεσιών.
OPER_ADMIN_NOT_FOUND
	%s δεν βρέθηκε στην λίστα των υπηρεσιών.
OPER_ADMIN_NO_MATCH
	Δεν βρέθηκαν θέσεις στην λίστα των υπηρεσιών.
OPER_ADMIN_DELETED
	%s διαγράφηκε από την λίστα των υπηρεσιών.
OPER_ADMIN_DELETED_ONE
	Διαγράφηκε 1 θέση από την λίστα των υπηρεσιών.
OPER_ADMIN_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από την λίστα των υπηρεσιών.
OPER_ADMIN_LIST_EMPTY
	Η λίστα των υπηρεσιών είναι άδεια.
OPER_ADMIN_LIST_HEADER
	Η λίστα των υπηρεσιών είναι οι εξής:
	 Νούμ   Ψευδ
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	Η λίστα των υπηρεσιών καθαρίστηκε.
OPER_ADMIN_MOVED
	%s has been moved to Services Administrators list.

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | entry-list]
OPER_OPER_SKELETON
	Οι υπηρεσίες είναι σε skeleton mode; Η εντολή OPER δεν είναι διαθέσιμη.
OPER_OPER_EXISTS
	%s υπάρχει ήδη στην λίστα των υπηρεσιών ως operator.
OPER_OPER_REACHED_LIMIT
	Συγνώμη, μπορείς μόνο να έχεις στην λίστα υπηρεσιών %d operators.
OPER_OPER_ADDED
	%s προσθέθηκε στην λίστα των υπηρεσιών ως operator.
OPER_OPER_NOT_FOUND
	%s δεν βρέθηκε στην λίστα των υπηρεσιών αυτλος ο operator.
OPER_OPER_NO_MATCH
	Δεν βρέθηκαν θέσεις στην λίστα των υπηρεσιών οι opers.
OPER_OPER_DELETED
	%s  διαγράφηκε από την λίστα των operators.
OPER_OPER_DELETED_ONE
	Διαγράφηκε 1 θέση από την λίστα των operators.
OPER_OPER_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από την λίστα των operators.
OPER_OPER_LIST_EMPTY
	Η λίστα των operators είναι άδεια.
OPER_OPER_LIST_HEADER
	Η λίστα των operators είναι:
	  Νου   Ψευδ
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	Η λίστα των operators καθαρίστηκε.
OPER_OPER_MOVED
	%s has been moved to Services Operators list.

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]
OPER_AKILL_EXISTS
	%s υπάρχει ήδη στην λίστα AKILL.
OPER_AKILL_ALREADY_COVERED
	%s έχει ήδη καλυφθεί από τον %s.
OPER_AKILL_REACHED_LIMIT
	Συγνώμη, μπορείς να έχεις μόνο %d AKILLs.
OPER_AKILL_NO_NICK
	Υπενθύμιση: Οι μάσκες των AKILL δεν πρέπει να περιέχουν ψευδώνυμα; σιγουρέψου οτι δεν περιέχεις ψευδώνυμο στην μάσκα.
OPER_AKILL_ADDED
	%s προσθέθηκε στην λίστα AKILL.
OPER_AKILL_CHANGED
	Το χρονικό όριο του/ης %s άλλαξε
OPER_AKILL_NOT_FOUND
	%s δεν βρέθηκε στην λίστα AKILL.
OPER_AKILL_NO_MATCH
	Δεν βρέθηκαν θέσεις στην λίστα AKILL.
OPER_AKILL_DELETED
	%s διαγράφηκε από την λίστα AKILL.
OPER_AKILL_DELETED_ONE
	Διαγράφηκε 1 θέση από την λίστα AKILL.
OPER_AKILL_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από την λίστα AKILL.
OPER_AKILL_LIST_EMPTY
	Η λίστα AKILL είναι άδεια.
OPER_AKILL_LIST_HEADER
	Η εως τώρα AKILL λίστα είναι:
	  Νου   Μάσκα                              Λόγος
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	Η έως τώρα AKILL λίστα είναι:
# number, mask, set-by, set-time, expires, reason
OPER_AKILL_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_AKILL_CLEAR
	Η λίστα AKILL καθαρίστηκε.
	
# SGLINE responses
OPER_CHANKILL_SYNTAX
	CHANKILL [+expiry] {#channel} [reason]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list}[:reason]]
OPER_SGLINE_UNSUPPORTED
	Συγνώμη, η εντολή SGLINE δεν είναι διαθέσιμη σε αυτό το δίκτυο.
OPER_SGLINE_EXISTS
	%s υπάρχει ήδη στην λίστα SGLINE.
OPER_SGLINE_ALREADY_COVERED
	%s έχει ήδη καλυφθεί από τον %s.
OPER_SGLINE_REACHED_LIMIT
	Συγνώμη, μπορείς να έχεις μόνο %d SGLINEs.
OPER_SGLINE_ADDED
	%s προσθέθηκε στην λίστα SGLINE.
OPER_SGLINE_CHANGED
	Το χρονικό όριο του/της %s άλλαξε.
OPER_SGLINE_NOT_FOUND
	%s δεν βρέθηκε στην λίστα SGLINE.
OPER_SGLINE_NO_MATCH
	Δεν βρέθηκαν θέσεις στην λίστα SGLINE.
OPER_SGLINE_DELETED
	%s διαγράφηκε από την λίστα SGLINE.
OPER_SGLINE_DELETED_ONE
	Διαγράφηκε 1 θέση από την λίστα SGLINE.
OPER_SGLINE_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από την λίστα SGLINE.
OPER_SGLINE_LIST_EMPTY
	Η λίστα SGLINE είναι άδεια.
OPER_SGLINE_LIST_HEADER
	Η έως τώρα λίστα SGLINE είναι οι εξης:
	  Νου   Μάσκα                              Λόγος
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	Current SGLINE list:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SGLINE_CLEAR
	Η λίστα SGLINE καθαρίστηκε.

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	Η εντολή Channel SQLINEs δεν υποστηρίζεται από τον IRCd σου,γιαυτο δεν μπορείς να τις χρησιμοποιήσεις.
OPER_SQLINE_EXISTS
	%s υπάρχει ήδη στην λίστα SQLINE.
OPER_SQLINE_ALREADY_COVERED
	%s έχει ήδη καλυφθεί από τον %s.
OPER_SQLINE_REACHED_LIMIT
	Συγνώμη, μπορείς να έχεις μόνο %d SQLINEs.
OPER_SQLINE_ADDED
	%s προσθέθηκε στην λίστα SQLINE.
OPER_SQLINE_CHANGED
	Το χρονικό όριο του/ης %s άλλαξε.
OPER_SQLINE_NOT_FOUND
	%s δεν βρέθηκε στην λίστα SQLINE.
OPER_SQLINE_NO_MATCH
	Δεν βρέθηκαν θέσεις στην λίστα SQLINE.
OPER_SQLINE_DELETED
	%s διαγράφηκε από την λίστα SQLINE.
OPER_SQLINE_DELETED_ONE
	Διαγράφηκε 1 θέση από την λίστα SQLINE.
OPER_SQLINE_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από την λίστα SQLINE.
OPER_SQLINE_LIST_EMPTY
	Η λίστα SQLINE είναι άδεια.
OPER_SQLINE_LIST_HEADER
	Η έως τώρα λίστα SQLINE είναι οι εξής:
	 Νου   Μάσκα                              Λόγος
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	Η έως τώρα λίστα SQLINE είναι οι εξής:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SQLINE_CLEAR
	Η λίστα SQLINE καθαρίστηκε.

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]
OPER_SZLINE_UNSUPPORTED
	Συγνώμη, η εντολή SΖLINE δεν είναι διαθέσιμη σε αυτό το δίκτυο.
OPER_SZLINE_EXISTS
	%s υπάρχει ήδη στην λίστα SZLINE.
OPER_SZLINE_ALREADY_COVERED
	%s έχει ήδη καλυφθεί από τον %s.
OPER_SZLINE_REACHED_LIMIT
	Συγνώμη, μπορείς να έχεις μόνο %d SZLINEs.
OPER_SZLINE_ONLY_IPS
	Υπενθύμιση: μπορείς μόνο να προσθέσεις IP μάσκες στην λίστα SZLINE.
OPER_SZLINE_ADDED
	%s προσθέθηκε στην λίστα SZLINE.
OPER_SZLINE_CHANGED
	Το χρονικό όριο του/ης %s άλλαξε.
OPER_SZLINE_NOT_FOUND
	%s δεν βρέθηκε στην λίστα SZLINE.
OPER_SZLINE_NO_MATCH
	Δεν βρέθηκαν θέσεις στην λίστα SZLINE.
OPER_SZLINE_DELETED
	%s διαγράφηκε από την λίστα SZLINE.
OPER_SZLINE_DELETED_ONE
	Διαγράφηκε 1 θέση από την λίστα SZLINE.
OPER_SZLINE_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από την λίστα SZLINE.
OPER_SZLINE_LIST_EMPTY
	Η λίστα SZLINE είναι άδεια.
OPER_SZLINE_LIST_HEADER
	Η έως τώρα λίστα SZLINE είναι οι εξής:
	  Νου    Μάσκα                              Λόγος
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	Η έως τώρα λίστα SZLINE είναι οι εξής:
# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_SZLINE_CLEAR
	Η λίστα SZLINE καθαρίστηκε.

# SET responses
OPER_SET_SYNTAX
	SET option setting
OPER_SET_IGNORE_ON
	Η εντολή Ignore θα χρησιμοποιείται.
OPER_SET_IGNORE_OFF
	Η εντολή Ignore δεν χρησιμοποιείται.
OPER_SET_IGNORE_ERROR
	Η επιλογή για IGNORE πρέπει να είναι ON ή OFF.
OPER_SET_READONLY_ON
	Τα Services είναι τώρα μόνο ανάγνωσης.
OPER_SET_READONLY_OFF
	Τα Services είναι σε επιλογή ανάγνωσης-εγγραφής.
OPER_SET_READONLY_ERROR
	Η επιλογή για READONLY πρέπει να είναι ON ή OFF.
OPER_SET_LOGCHAN_ON
	Services is now reporting log messages to %s.
OPER_SET_LOGCHAN_OFF
	Services is no longer reporting log messages to a channel.
OPER_SET_LOGCHAN_ERROR
	Setting for LOGCHAN must be ON or OFF and LogChannel must be defined.
OPER_SET_DEBUG_ON
	Τα Services είναι τώρα σε debug επιλογή.
OPER_SET_DEBUG_OFF
	Τα Services είναι τώρα σε μη-debug επιλογή.
OPER_SET_DEBUG_LEVEL
	Τα Services είναι τώρα σε debug επιλογή (βαθμός %d).
OPER_SET_DEBUG_ERROR
	Η επιλογή για DEBUG πρέπει να είναι ON, OFF, ή ένα κατηγοριματικό νούμερο.
OPER_SET_NOEXPIRE_ON
	Τα Services είναι τώρα σε επιλογή να μην λήγει.
OPER_SET_NOEXPIRE_OFF
	Τα Services είναι τώρα σε επιλογή να λήγει.
OPER_SET_NOEXPIRE_ERROR
	Η επιλογή για NOEXPIRE πρέπει να είναι ON ή OFF.
OPER_SET_UNKNOWN_OPTION
	’γνωστη επιλογή %s.
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
	Όλες οι O:γραμμές του %s έχουν απομακρυνθεί.
OPER_NOOP_REVOKE
	Όλες οι O:γραμμές του %s έχουν σβηστεί.

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE servername [reason]

# RAW responses
OPER_JUPE_HOST_ERROR
	Please use a valid server name when juping
OPER_JUPE_INVALID_SERVER
	You can not jupe your services server or your uplink server.
	
# RAW responses
OPER_RAW_SYNTAX
	RAW text

# UPDATE responses
OPER_UPDATING
	Γίνεται Update στις βάσεις δεδομένων.
	
# RELOAD responses
OPER_RELOAD
	Το αρχείο των υπηρεσιών,conf file έγινε reloaded,επαναφορτώθηκε.

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN not defined; δεν μπορεί να γίνει επανεκκίνηση.  Ξανατρέξε το \2configure\2 script και κάνε recompile τα Services για να ενεργοποιήσεις την εντολή RESTART.

# LISTIGNORE responses
OPER_IGNORE_SYNTAX
	IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick | mask]
OPER_IGNORE_VALID_TIME
	You have to enter a valid number as time.
OPER_IGNORE_TIME_DONE
	%s will now be ignored for %s.
OPER_IGNORE_PERM_DONE
	%s will now permanently be ignored.
OPER_IGNORE_DEL_DONE
	%s will no longer be ignored.
OPER_IGNORE_LIST
	Η λίστα ignore για τις υπηρεσίες:
OPER_IGNORE_LIST_NOMATCH
	%s not found on ignore list.
OPER_IGNORE_LIST_EMPTY
	Η λίστα ignore των υπηρεσιών είναι άδεια.

# KILLCLONES responses
OPER_IGNORE_LIST_CLEARED
	Ignore list has been cleared.

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	Δεν μπορεί να βρεθεί ο χρήστης %s.
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	Λίστα Καναλιού:
	Όνομα                 Χρήστες  Επιλογές   Topic
OPER_CHANLIST_HEADER_USER
	%s channel list:
	Όνομα                 Χρήστες  Επιλογές   Topic
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	Τέλος λίστας καναλιού.
	
# USERLIST responses
OPER_USERLIST_HEADER
	Λίστα χρηστών:
	Όνομα                 Μάσκα
OPER_USERLIST_HEADER_CHAN
	%s λίστα χρηστών:
	Όνομα                 Μάσκα
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	Τέλος λίστας χρηστών.

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
	DEFCON [1|2|3|4|5]
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
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [params]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+χρόνος λήξης] μάσκα όριο λόγος	
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {μάσκα | λίστα}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE νούμερο θέση
OPER_EXCEPTION_DISABLED
	Η Session limiting  εντολή είναι απενεργοποιημένη.
OPER_EXCEPTION_ALREADY_PRESENT
	Η μάσκα %s είναι ήδη στην λίστα exception.
OPER_EXCEPTION_TOO_MANY
	Η λίστα Session-limit exception είναι full!
OPER_EXCEPTION_ADDED
	Session limit για τον %s ρυθμίστηκε σε %d.
OPER_EXCEPTION_MOVED
	Exception για %s (#%d) μετακινήθηκε στη θέση %d.
OPER_EXCEPTION_NO_SUCH_ENTRY
	Δεν βρέθηκε θέση (#%d) στη λίστα session-limit exception.
OPER_EXCEPTION_NOT_FOUND
	%s δεν βρέθηκε στην λίστα session-limit exception.
OPER_EXCEPTION_NO_MATCH
	Δεν βρέθηκαν θέσεις στην λίστα session-limit exception.
OPER_EXCEPTION_DELETED
	%s διαγράφηκε από την λίστα session-limit exception.
OPER_EXCEPTION_DELETED_ONE
	Διαγράφηκε 1 θέση από την λίστα session-limit exception.
OPER_EXCEPTION_DELETED_SEVERAL
	Διαγράφηκαν %d θέσεις από την λίστα session-limit exception.
OPER_EXCEPTION_LIST_HEADER
	Η έως τώρα λίστα Session Limit Exception είναι οι εξής:
# mask, limit
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	Νου  Limit  Host
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (by %s on %s; %s)
	    Limit: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	Λάθος session limit. Πρέπει να είναι λάθος ο integer greater ή πρέπει να είναι μεταξύ μηδέν και %d.
OPER_EXCEPTION_INVALID_HOSTMASK
	Λάθος hostmask. Μόνο πραγματικά hostmasks χρησιμοποιούνται και όχι ψευδώνυμα ή usernames.
OPER_EXCEPTION_EXISTS
	%s already exists on the EXCEPTION list.
OPER_EXCEPTION_CHANGED
	Exception for %s has been updated to %d.

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST limit | VIEW host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST όριο
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW host
OPER_SESSION_DISABLED
	Η εντολή Session limiting είναι απενεργοποιημένη.
OPER_SESSION_INVALID_THRESHOLD
	Λάθος threshold value. Πρέπει να είναι μεγαλύτερο από το 1.
OPER_SESSION_NOT_FOUND
	%s δεν βρέθηκε στην λίστα session.
OPER_SESSION_LIST_HEADER
	Hosts με το λιγότερο %d sessions:
OPER_SESSION_LIST_COLHEAD
	Sessions  Host
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	Το host %s έχει ως τώρα %d sessions με όριο το %d.

# EXCEPTION help

OPER_HELP_EXCEPTION
	Σύνταξη: EXCEPTION ADD [+expiry] mask limit reason
	        EXCEPTION DEL {mask | list}
	        EXCEPTION MOVE num position
	        EXCEPTION LIST [mask | list]
	        EXCEPTION VIEW [mask | list]
	
	Επιτρέπουν στους Services admins να ορίσουν την λίστα των hosts τα
	οποία έχουν ένα συγκεκριμένο όριο - αφήνοντας ασφαλής μηχανήματα, 
	όπως είναι τα shell servers, to carry more than the default number
	of clients at a time. Αν κάποιο host περάσει από το session limit,
	όλοι οι clients που θα προσπαθήσουν να συνδεθούν με αυτό το host θα 
	απορριφθούν-σκοτωθούν.Πριν ο χρήστης σκοτωθεί-απορριφθεί, θαn προηδοποιηθεί,
	με /NOTICE από τον %S. Το σημείωμα αυτό είναι επιλογή από το configuration.
	    
	EXCEPTION ADD προσθέτει το hostname στην Exception list.
	Σημείωση,αν γράψετε nick!user@host και user@host σαν μάσκες είναι λάθος! 
	Μόνο πραγματικά host masks, όπως box.host.dom και *.host.dom, 
	είναι επιτρεπόμενα γιατί το όριο των sessions δεν περιέχει ψευδώνυμα ή 
	user names μέσα στο account. όριο πρέπει να είναι ένα νούμερο μεγαλύτερο από
	ή κοντά στο μηδέν. Αυτό εξαρτάται από τα πόσα sessions αυτό το host
	μπορεί να πάρει μέσα στο χρόνο. Ο βαθμός μηδέν σημαίνει οτι το host δεν θα έχει
	όριο στην session limit. Κοίτα το βοήθημα AKILL για περισσότερες πληροφορίες
	με την σύνταξη της εντολής και την παράμετρο expiry.
	EXCEPTION DEL σβήνει την δωσμένη μάσκα από την λίστα exception.
	EXCEPTION MOVE απομακρύνει το exception νούμερο σε θέση.
	EXCEPTION LIST and EXCEPTION VIEW δείχνει όλα τα έως τώρα 
	exceptions; αν η καθορισμένη μάσκα δψθεί, τότε θα εμφανίζεται στη λίστα.
	EXCEPTION VIEW είναι πιο αναλυτική, εμφανίζει το ψευδώνυμο του χρήστη
	που πρόσθεσε αυτό το exception, με λόγο,μάσκα και το χρόνο λήξης σε μέρα και ώρα.
	    
	Σημείωση το συνδεδεμένο client θα "χρησιμοποιεί" την πρώτη exception
	και όπου ταιριάξει το host.
	    
	Μόνο για τους Επικεφαλείς Υπηρεσιών.

OPER_HELP_SESSION
	Σύνταξη: SESSION LIST threshold
	        SESSION VIEW host
	
	Αφήνει τους επικεφαλείς υπηρεσιών να δουν την λίστα session.
	
	SESSION LIST Λίστα με hosts με τουλάχιστον threshold sessions.
	Το threshold πρέπει να είναι ένα νούμερο μεγαλύτερο από 1. Αυτό γίνεται 
	για να αποφευχθούν ατυχήματα στη λίστα μεγάλων νούμερων μονών session hosts. 
	SESSION VIEW εμφανίζει αναλυτικές πληροφορίες σχετικά με τα specific
	host - συμεριλαμβανομένου πρόσφατου session μετρητή και session limit.
	Η μάσκα πρέπει να μην περιλαμβάνεται σε wildcards.
	    
	Κοίτα στο EXCEPTION help για περισσότερες πληροφορίες σχετικά με το session
	limiting και για το πως θα κάνεις session limits σε specific hosts και groups.
	    
	Μόνο για τους Επικεφαλείς Υπηρεσιών.

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
	set of restrictions to services useful during an attempted
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
	[Κανονισμοί ή πληροφορίες - %s] %s
NEWS_OPER_TEXT
	[Νέα για Opers - %s] %s
NEWS_RANDOM_TEXT
	[Νέα σχετικά με το δίκτυο - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD|DEL|LIST} [περιεχόμενο|νούμερο]
NEWS_LOGON_LIST_HEADER
	Κανονισμοί:
NEWS_LOGON_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_LOGON_LIST_NONE
	Δεν υπάρχουν κανονισμοί ή πληροφορίες.
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD περιεχόμενο
NEWS_LOGON_ADD_FULL
	Η λίστα των νέων είναι γεμάτη!
NEWS_LOGON_ADDED
	Προστέθηκε ένας κανονισμός ή πληροφορία στη θέση (#%d).
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {νούμερο | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	Κανονισμός στη θέση #%d δεν βρέθηκε!
NEWS_LOGON_DELETED
	Κανονισμός ή πληροφορία στη θέση #%d διαγράφηκε.
NEWS_LOGON_DEL_NONE
	Κανένας Κανονισμός ή πληροφορία για να διαγραφεί!
NEWS_LOGON_DELETED_ALL
	Όλοι οι κανονισμοί ή οι πληροφορίες διαγράφηκαν.

NEWS_OPER_SYNTAX
	Σύνταξη: OPERNEWS {ADD|DEL|LIST} [περιεχόμενο|νούμερο]	
NEWS_OPER_LIST_HEADER
	Νέα των Opers:
NEWS_OPER_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_OPER_LIST_NONE
	Δεν υπάρχουν νέα για τους opers.
NEWS_OPER_ADD_SYNTAX
	Σύνταξη: OPERNEWS ADD περιεχόμενο
NEWS_OPER_ADD_FULL
	Η λίστα είναι γεμάτη!
NEWS_OPER_ADDED
	Προσθέθηκε ένα νέο μήνυμα για τους opers θέση (#%d).
NEWS_OPER_DEL_SYNTAX
	Σύνταξη: OPERNEWS DEL {νούμερο | ALL}
NEWS_OPER_DEL_NOT_FOUND
	Η θέση #%d στην λίστα δεν βρέθηκε!
NEWS_OPER_DELETED
	Η θέση #%d διαγράφηκε.
NEWS_OPER_DEL_NONE
	Δεν υπάρχουν θέσεις στην λίστα για να διαγράψεις!
NEWS_OPER_DELETED_ALL
	Όλα τα νέα για τους opers διαγράφηκαν.

NEWS_RANDOM_SYNTAX
	RANDOMNEWS {ADD|DEL|LIST} [περιεχόμενο|νούμερο]
NEWS_RANDOM_LIST_HEADER
	Νέα για το δίκτυο:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_RANDOM_LIST_NONE
	Δεν υπάρχουν νέα.
NEWS_RANDOM_ADD_SYNTAX
	RANDOMNEWS ADD περιεχόμενο
NEWS_RANDOM_ADD_FULL
	Η λίστα είναι γεμάτη!
NEWS_RANDOM_ADDED
	Προσθέθηκε ένα νέο μήνυμα στην θέση (#%d).
NEWS_RANDOM_DEL_SYNTAX
	RANDOMNEWS DEL {νούμερο | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	Το μήνυμα στην θέση #%d δεν βρέθηκε!
NEWS_RANDOM_DELETED
	Το μήνυμα στην θέση #%d διαγράφηκε.
NEWS_RANDOM_DEL_NONE
	Δεν βρέθηκαν μηνύματα για να διαγραφούν!
NEWS_RANDOM_DELETED_ALL
	Όλα τα μηνύματα διαγράφηκαν.

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
	Σύνταξη: RANDOMNEWS ADD περιεχόμενα
	        RANDOMNEWS DEL {νούμερο | ALL}
	        RANDOMNEWS LIST
	
	Μπορείς να προσθέσεις, να αφαιρέσεις ή να δεις την λίστα των μηνυμάτων.
	Όταν ένας χρήστης συνδεθεί στο δίκτυο, ένα (και μόνο ένα) από τα
	μηνύματα θα επιλεχθεί τυχαία και θα σταλεί.
	
	RANDOMNEWS may only be used by Services admins.

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S είναι μια υπηρεσία σχεδιασμένη ωστε να σας δίνει πληροφορίες για
	τις υπηρεσίες.  Για περισσότερες βοήθειες εκτός του HELP 
	θα βρείτε στις παρακάτω (υπο)υπηρεσίες με περισσότερη αναλυτικότητ:

	%R%s HELP
	     για πληροφορίες σχετικά με την κατοχύρωση ψευδώνυμων

	%R%s HELP
	     για πληροφορίες σχετικά με την κατοχύρωση και την διαχείρηση
	     των καναλιών.

	%R%s HELP
	     για πληροφορίες σχετικά με την αποστολή μηνυμάτων στους off-line χρήστες

HELP_HELP_BOT

	%R%s HELP
	     για πληροφορίες σχετικά με τις επιλογές του bot στο κανάλι σας

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
	    REGISTER   Κατοχυρώνει ένα ψευδώνυμο
NICK_HELP_CMD_GROUP
	    GROUP      Μπένετε σε ομάδα ψευδώνυμου
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY   Σας αναγνωρίζει με τη χρήση κωδικού
NICK_HELP_CMD_ACCESS
	    ACCESS     Αλλάζει τη λίστα των εξουσιοδοτημένων διευθύνσεων
NICK_HELP_CMD_SET
	    SET        Αλλαγή επιλογών, συμπεριλαμβάνεται η προστασία με αποσύνδεση
NICK_HELP_CMD_SASET
	    SASET      Set SET-options on another nickname
NICK_HELP_CMD_DROP
	    DROP       Διαγράφει ένα ψευδώνυμο
NICK_HELP_CMD_RECOVER
	    RECOVER    Αποσύνδεση ενός άλλου χρήστη που έχει πάρει το ψευδώνυμό σας
NICK_HELP_CMD_RELEASE
	    RELEASE    Ανάκτηση της κατακράτησης του ψευδωνύμου,μετά από RECOVER
NICK_HELP_CMD_SENDPASS
	    SENDPASS   Ξέχασες τον κωδικό? Δοκίμασε αυτό και θα στο στείλουμε εμείς
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
	    GETPASS    Εμφανίζει τον κωδικό για το συγκεκριμένο ψευδώνυμο
	               (μόνο αν η κωδικοποίηση είναι ανενεργή)
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   Matches and returns all users that registered using given email
NICK_HELP_CMD_FORBID
	    FORBID     Καταστέλει ένα ψευδώνυμο,δηλ το κάνει να μην το χρησιμοποιεί
	               κανείς,όποιος το πάρει έχει kill
NICK_HELP_CMD_SUSPEND
	    SUSPEND    Suspend a given nick
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Unsuspend a given nick

NICK_HELP
	O %S σας επιτρέπει να κατοχυρώσετε ένα ψευδώνυμο,
	εμποδίζοντας έτσι άλλους χρήστες να το χρησιμοποιούν.
	Οι παρακάτω εντολές, επιτρέπουν την κατοχύρωση και τη διαχείρηση των
	ψευδωνύμων; για να χρησιμοποιήσετε αυτά, γράψτε %R%S ^_command^_.
	Για περισσότερες πληροφορίες σε μια συγκεκριμένη εντολή,γράψτε
	%R%S HELP ^_command^_.
	
NICK_HELP_FOOTER
	ΠΑΡΑΤΗΡΗΣΗ: Αυτή η υπηρεσία σκοπεύει να παρέχει ένα τρόπο
	στους χρήστες του δικτύου, να διασφαλίσουν ότι η ταυτότητά
	τους δεν εκτίθεται.  ΔΕΝ υφίσταται για να εξυπηρετεί ή να
	διευκολήνει την "κλοπή" των ψευδωνύμων ή άλλες νοσηρές
	ενέργειες.  Η κατάχρηση του %S θα οδηγήσει, τουλάχιστον,
	σε απώλεια των ανάλογων ψευδωνύμων.
	
NICK_HELP_EXPIRES
	
	Ψευδώνυμα τα οποία δεν χρησιμοποιούνται πλέον θα είναι στην διαδικασία 
	της αυτόματης λήξης, π.ς. θα διαγράφονται αυτόματα
	μετά από %d μέρες αν δεν χρησιμοποιούνται.

NICK_HELP_REGISTER
	Σύνταξη: REGISTER password [email]
	
	Κατοχυρώνοντας το ψευδώνυμο σου καταγράφεται στο %S, την βάση δεδομένων.
	Αφού το κατοχύρωσες, μπορείς να χρησιμοποιήσεις τις εντολές
	SET και ACCESS για να σετάρεις το ψευδώνυμο όπως θες εσύ.
	Να είσαι σίγουρος οτι θυμάσαι πάντα το κωδικό που έκανες την κατοχύρωση
	του ψευδώνυμού σου - θα σου χρειαστεί στο να κάνεις αλλαγές
	αργότερα.  (Σημείωση οτι case matters!  ANOPE, Anope, και 
	anope έχουν όλα διαφορετικούς κωδικούς!)
	
	Χρήσιμες οδηγίες για το διάλεγμα κωδικών:
	
	Οι κωδικοί δεν θα πρέπει να είναι εύκολη ώστε να μαντεύονται.  Για παράδειγμα,
	χρησιμοποιώντας το πραγματικό όνομα σαν κωδικός είναι άσχημη ιδέα.
	Χρησιμοποιώντας το ψευδώνυμο ωε κωδικό είναι η πιο χειρότερη
	ιδέα ;) και, στο κάτω κάτω, ο %S δεν θα σε αφήσει να το κάνεις.
	Επίσης, μικροί κωδικοί δεν είναι αξιόπιστοι, γι'αυτό
	θα πρέπει να διαλέξεις ένα κωδικό με τουλάχιστον το λιγότερο 5 χαρακτήρες.
	Τελικά, ο χαρακτήρας το κενό δεν χρησιμοποιείται σε κωδικούς.
	    
	Η παράμετρος email είναι προεπιλεγμένη και θα πρέπει να έχετε 
	σετάρει το email του ψευδώνυμού σας άμεσα. Όπως και να'χει, σε 
	ορισμένα δίκτυα απαιτείται αμέσως στην εντολή κατοχύρωσης.
	Η μυστικότητα είναι σεβαστή; αυτό το e-mail δεν θα το πάρει
	κανένα τρίτο άτομο.
	    
	Αυτή η εντολή δημιουργεί νέες ομάδες για το ψευδώνυμό σας,
	και μπορούν να επιτρέπουν την κατοχύρωση άλλων ψευδονύμων αργότερα 
	με μοίρασμα ίδιων σεταρισμάτων, ίδιων μηνυμάτων και ίδιων
	δικαιωμάτων στα κανάλια. Για περισσότερες πληροφορίες για αυτή 
	τη μελλοντική εντολή, γράψε %R%S HELP GROUP.
	    
NICK_HELP_GROUP
	Σύνταξη: GROUP στόχος κωδικός
	
	Αυτή η εντολή κάνει το ψευδώνυμο σου να μπένει στο στόχο των 
	ψευδωνύμων της ομαδοποίησης. Κωδικός είναι ο κωδικός του 
	στόχου,δηλ του άλλου ψευδωνύμου.
	
	Μπαίνοντας στην ομάδα έχεις πρόσβαση και μοιράζεσαι,τα μηνύματα 
	δηλ memos, και κυριαρχία στα κανάλια που αντιστοιχούν όλα
	τα ψευδώνυμα σε αυτή την ομάδα, και πολλά άλλα!
	     
	Η ομάδα φτιάχεται για να σας εξυπηρετεί καλύτερα. Αυτό σημαίνει 
	οτι αν διαγράψετε κάποιο ψευδώνυμο και είναι στην ομάδα,
	δεν θα χάσετε τις πληροφορίες και τα μοιρασμένα πράγματα που 
	έχετε με τα άλλα ψευδώνυμα, αρκεί όμως στην ομάδα
	να υπάρχει τουλάχιστον ένα ψευδώνυμο.
	    
	Μπορείς να εκτελέσεις αυτή την εντολή ακόμα και όταν δεν
	έχεις κατοχυρώσει κάποιο ψευδώνυμο ακόμα. Αν το ψευδώνυμο είναι
	κατοχυρωμένο, θα πρέπει να κάνεις εντολή αναγνώρισης πρώτα και 
	μετά να χρησιμοποιήσεις αυτή την εντολή. Γράψε
	%R%S HELP IDENTIFY για περισσότερες πληροφορίες. Αυτό το 
	κομμάτι μπορεί να μην υπάρχει στο δικτυό σου.
	    
	Είναι προτιμότερο να χρησιμοποιήσεις αυτή την εντολή με μη-κατοχυρωμένα  
	ψευδώνυμα γιατί θα γίνει αυτόματα η κατοχύρωση όταν θα χρησιμοποιήσεις 
	αυτή την εντολή. Μπορείς και να την χρησιμοποιήσεις και με κατοχυρωμένα 
	ψευδώνυμα (για να αλλάξεις την ομάδα σου) μόνο αν ο επικεφαλής των 
	υπηρεσιών σε αφήσει να το κάνεις.
	    
	Μπορείς να είσαι σε μια ομάδα κάθε φορά. Σε πολλές ομάδες δεν είναι δυνατόν.
	    
	Σημείωση: όλα τα ψευδώνυμα που ανήκουν στην ίδια ομάδα έχουν τον ίδιο 
	κωδικό.
	
NICK_HELP_IDENTIFY
	Σύνταξη: IDENTIFY κωδικό
	
	Λέει στον %S αν είσαι όντος ο κάτοχος αυτού του ψευδώνυμου.
	Πολλές εντολές απαιτούν να κάνεις εντολή αναγνώρισης, δηλ με την εντολή
	αυτή πριν χρησιμοποιήσεις άλλες εντολές.  Ο κωδικός θα πρέπει να είναι
	ο ίδιος με αυτόν που έδωσες για να κατοχυρώσης το ψευδώνυμο,
	δηλ με την REGISTER εντολή.
	
NICK_HELP_UPDATE
	Syntax: UPDATE

	Updates your current status, i.e. it checks for new memos,
	sets needed chanmodes (ModeonID) and updates your vhost and
	your userflags (lastseentime, etc).
	
NICK_HELP_LOGOUT
	Σύνταξη: LOGOUT
	
	Είναι το αντίθετο της εντολής IDENTIFY, δηλ
	η εντολή αυτη σε κάνει να μην είσαι ο κάτοχος αυτού του ψευδώνυμου
	πλέον. Σημείωση, όπως και ναχει, δεν θα σου γίνει παρατήρηση ή να σου 
	ζητηθεί να ξανακάνεις εντολή αναγνώρισης.
	
NICK_HELP_DROP
	Σύνταξη: DROP [ψευδώνυμο]
	
	Σβήνει το ψευδώνυμο σου από του %S την βάση δεδομένων.  Το ψευδώνυμο που
	σβήστηκε είναι ελεύθερο για οποιοδήποτε να το κατοχυρώσει.
	
	Μπορείς να σβήσεις το ψευδώνυμο από την ομάδα παιρνώντας
	από την παράμετρο ψευδώνυμου.
	
	Για να χησιμοποιήσεις αυτή την εντολή, πρέπει πρώτα να κάνεις εντολή 
	αναγνώρισης με τον κωδικό σου για περισσότερες
	with your password (%R%S HELP IDENTIFY for more
	πληροφορίες.)
	    
NICK_HELP_ACCESS
	Σύνταξη: ACCESS ADD ^_μάσκα^_
	    ACCESS DEL ^_μάσκα^_
	    ACCESS LIST
	
	Αλλάζει ή εμφανίζει τη λίστα πρόσβασης του ψευδωνύμου σας.
	Αυτή είναι η λίστα των διευθύνσεων που θα αναγνωρίζονται
	αυτόματα από τον %S σαν επιτρεπτοί για τη χρήση
	του ψευδωνύμου. Αν θέλετε να χρησιμοποιήσετε το ψευδώνυμο
	από μία διαφορετική διεύθυνση, πρέπει να αναγνωριστείτε
	από τον %S με τη χρήση της εντολής IDENTIFY.
	
	Παραδείγματα:
	
	    ACCESS ADD anyone@*.bepeg.com
	        Επιτρέπει πρόσβαση στο χρήστη με ID anyone
	        από οποιοδήποτε υπολογιστή της διεύθυνσης bepeg.com.
	
	    ACCESS DEL anyone@*.bepeg.com
	        Αναιρεί την προηγούμενη εντολή.
	    
	    ACCESS LIST
	        Εμφανίζει την τρέχουσα λίστα πρόσβασης.
	    
NICK_HELP_SET
	Σύνταξη: SET option parameters
	
	Αλλάζει διάφορες επιλογές του ψευδωνύμου.  Η επιλογή μπορεί να είναι:
	
	    DISPLAY    Αλλάζει την λίστα των ομάδων
	    PASSWORD   Αλλάζει τον κωδικό του ψευδωνύμου σας
	    LANGUAGE   Αλλάζει τη γλώσσα που χρησιμοποιούν οι
	                   υπηρεσίες όταν τις χρησιμοποιείτε
	    URL        Συσχετίζει ένα URL με το ψευδώνυμό σας
	    EMAIL      Συσχετίζει μία διεύθυνση e-mail με το ψευδώνυμό σας
	    ICQ        Συσχετίζει ένα νούμερο ICQ με το ψευδώνυμό σας
	    GREET      Συσχετίζει ένα μήνυμα χαιρετισμού με το ψευδώνυμό σας
	    KILL       Ενεργοποιεί/απενεργοποιεί την προστασία με αποσύνδεση
	    SECURE     Ενεργοποιεί/απενεργοποιεί τη λειτουργία ασφάλειας
	    PRIVATE    Εμποδίζει το ψευδώνυμό σας να εμφανίζεται
	                   με την εντολή  %R%S LIST
	    HIDE       Αποκρύπτει συγκεκριμένα τμήματα πληροφοριών του ψευδωνύμου
	    MSG        Αλλάζει την μέθοδο επικοινωνίας με τις υπηρεσίες
	    AUTOOP     Should services op you automatically.    
	
	Για να χρησιμοποιήσετε αυτή την εντολή, πρέπει πρώτα να
	αναγνωριστείτε με τον κωδικό σας (για περισσότερες πληροφορίες,
	%R%S HELP IDENTIFY).
	
	Πληκτρολογήστε %R%S HELP SET επιλογή για περισσότερες πληροφορίες
	σε μία συγκεκριμένη επιλογή.

NICK_HELP_SET_DISPLAY
	Σύνταξη: SET DISPLAY νέο-display
	
	Βλέπεις στη λίστα το ψευδώνυμο σου σε ποια ομάδα είναι των 
	υπηρεσιών. Στη λίστα πρέπει να είναι το ψευδώνυμο που είναι στην ομάδα.

NICK_HELP_SET_PASSWORD
	Σύνταξη: SET PASSWORD νέος-κωδικός
	
	Αλλάζει τον κωδικό που χρησιμοποιείς για την εντολή αναγνώρισης του κάτοχου
	ψευδώνυμου.

NICK_HELP_SET_LANGUAGE
	Σύνταξη: SET LANGUAGE νούμερο
	
	Αλλάζει την γλώσσα των υπηρεσιών που στέλνει με μηνύματα σε σένα
	(για παράδειγμα, όταν ανταποκρίνεσαι σε μια εντολή θα σου στείλει σε άλλη γλώσσα).
	Το νούμερο πρέπει να είναι ένα από τα επόμενα που υπάρχουν στην λίστα
	υποστηριζόμενων γλωσσών:
	

NICK_HELP_SET_URL
	Σύνταξη: SET URL url
	
	Προσθέτει στις πληροφορίες του ψευδώνυμού σου το URL που του δίνεις.  Αυτό
	το URL θα εμφανίζεται κάθε φορά που κάποιος θα θέλει να δει κάποιεσ πληροφορίες
	για το ψευδώνυμο σου χρησιμοποιώντας την εντολή INFO.

NICK_HELP_SET_EMAIL
	Σύνταξη: SET EMAIL διεύθυνση
	
	Δείχνει την δεύθυνση E-mail που του έχεις δώσει για το ψευδώνυμο σου.
	Αυτή η διεύθυνση θα εμφανίζεται κάθε φορά που κάποιος θα κοιτάζει πληροφορίες
	στο κανάλι με την εντολή INFO.
	
NICK_HELP_SET_ICQ
	Σύνταξη: SET ICQ number
	
	Προσθέτει στις πληροφορίες του ψευδώνυμού σου το νούμερο του ICQ.  Αυτό 
	το νούμερο θα εμφανίζεται κάθε φορά που κάποιος θα θέλει να δει κάποιεσ πληροφορίες 
	για το ψευδώνυμο σου χρησιμοποιώντας την εντολή INFO.
	
NICK_HELP_SET_GREET
	Σύνταξη: SET GREET message
	
	Προσθέτει στο ψευδώνυμού σου ένα μήνυμα χαιρετισμού, το
	οποίο θα εμφανίζεται κάθε φορα που θα μπένεις σε κανάλι με την προυπόθεση οτι
	η επιλογή GREET θα είναι ενεργοποιημένη, και με την προυπόθεση οτι θα έχεις
	και την κατάλληλη access σε αυτό. 
	
NICK_HELP_SET_KILL
	Σύνταξη: SET KILL {ON | QUICK | IMMED | OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την αυτόματη προστασία με
	αποσύνδεση για το ψευδώνυμό σας.  Ενεργοποιώντας την
	προστασία με αποσύνδεση, αν κάποιος άλλος χρήστης
	προσπαθήσει να πάρει το ψευδώνυμό σας, θα δωθεί ένα λεπτό
	για να αλλάξει το ψευδώνυμό του, μετά από το οποίο θα
	αποσυνδεθεί από το IRC από τον %S.
	
	Αν επιλέξετε QUICK, θα δωθούν μόνο 20 δευτερόλεπτα στο
	χρήστη για να αλλάξει το ψευδώνυμό του, αντί για τα συνηθισμένα 60.
	Αν επιλέξετε IMMED, ο χρήστης θα αποσυνδεθεί κατευθείαν χωρίς
	πρώτα να προειδοποιηθεί ή να του δωθεί η ευκαιρία να αλλάξει το
	ψευδώνυμό του;Παρακαλούμε μη χρησιμοποιείτε αυτή την επιλογή εκτός α
	είναι απολύτως απαραίτητο.  Επίσης, οι διαχειριστές του
	δικτύου μπορεί να έχουν απενεργοποιήσει αυτή την επιλογή.

NICK_HELP_SET_SECURE
	Σύνταξη: SET SECURE {ON | OFF}
	
	Αλλάζει ο %S την ασφάλεια σε on ή off για το ψευδώνυμο
	Με την ΑΣΦΑΛΗ επιλογή, πρέπει να γράψεις τον κωδικό αναγνώρισης
	πριν αναγνωριστής σαν κάτοχος του ψευδώνυμου,
	με την υπόληψη οτι η διεύθυνση είναι στην λίστα των access.
	Όπως και ναχει, αν είσαι στην λίστα access, ο %S
	δεν θα κάνει auto-kill με την υπόληψη της επιλογής KILL.

NICK_HELP_SET_PRIVATE
	Σύνταξη: SET PRIVATE {ON | OFF}
	
	Ενεργοποιεί/απενεργοποιεί την επιλογή ιδιώτευσης
	του %S για το ψευδώνυμό σας.  Με την ενεργοποίηση του
	PRIVATE το ψευδώνυμό σας δε θα εμφανίζεται στη λίστα
	ψευδωνύμων που προκύπτει με την εντολή LIST του %S.
	(Βέβαια,οποιοσδήποτε γνωρίζει το ψευδώνυμό σας, μπορεί
	ακόμη να πάρει πληροφορίες γι αυτό, χρησιμοποιώντας την
	εντολή INFO)

NICK_HELP_SET_HIDE
	Σύνταξη: SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
	
	Σας επιτρέπει να εμποδίσετε την εμφάνιση συγκεκριμένων
	πληροφοριών όταν κάποιος εκτελεί την εντολή του %S INFO, στο
	ψευδώνυμό σας.  Μπορείτε να αποκρύψετε τη διεύθυνση e-mail
	user@host mask (USERMASK), and last quit message (QUIT).
	σας (EMAIL), την τελευταία διεύθυνση με την οποία συνδεθήκατε
	(USERMASK), και το τελευταίο μήνυμα (QUIT).
	Η δεύτερη παράμετρος καθορίζει το αν η πληροφορία θα πρέπει
	να εμφανίζεται (OFF) ή να αποκρύπτεται (ON).
	
NICK_HELP_SET_MSG
	Σύνταξη: SET MSG {ON | OFF}
	
	Σας επιτρέπει να διαλέξετε το τρόπο με τον οποίο τα Services θα 
	επικοινωνούν μαζί σου.  Με την εντολή MSG, τα Services θα 
	χρησιμοποιούν μηνύματα, διαφορετικά θα χρησιμοποιούν παρατηρήσεις 
	(notices). 
	
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
	Σύνταξη: GHOST ψευδώνυμο [κωδικός]
	
	Διακόπτει μια "εικονική" σύνδεση με το ψευδώνυμό σας.
	"Εικονική" σύνδεση είναι αυτή που στην πραγματικότητα δεν
	υφίσταται, αλλά ο IRC server για κάποιο λόγο πιστεύει ότι
	είναι ακόμα ενεργή.  Τυπικά, αυτό συμβαίνει αν ο υπολογιστής
	ή το modem σας "κρεμάσει" ή η σύνδεσή σας με τον παροχέα
	internet διακοπεί, ενώ βρίσκεστε στο IRC.
	
	Για να χρησιμοποιήσετε την εντολή GHOST για ένα ψευδώνυμο,
	πρέπει η τρέχουσα διεύθυνσή σας όπως εμφανίζεται στο /WHOIS
	να βρίσκεται στη λίστα πρόσβασης του ψευδωνύμου, ή να έχετε
	δώσει τον σωστό κωδικό για το ψευδώνυμο.
	
NICK_HELP_INFO
	Σύνταξη: INFO ψευδώνυμο [ALL]
	
	Εμφανίζει πληροφορίες για το ψευδώνυμο που θα δώσετε, όπως
	τον κάτοχο του ψευδωνύμου, την τελευταία διεύθυνση και ώρα
	σύνδεσης και τις επιλογές του ψευδωνύμου.  Αν έχετε 
	αναγνωριστεί για το ψευδώνυμο που ζητάτε πληροφορίες και
	χρησιμοποιήσετε την παράμετρο ALL στην εντολή, θα δείτε
	όλες τις πληροφορίες, άσχετα αν αυτές αποκρύπτονται ή όχι.

NICK_HELP_LIST
	Σύνταξη: LIST υπόδειγμα
	
	Εμφανίζει όλα τα κατοχυρωμένα ψευδώνυμα, τα οποία ταιριάζουν
	με το υπόδειγμα της μορφής, nick!user@host που έχει δωθεί.
	Ψευδώνυμα που έχουν την επιλογή PRIVATE ενεργοποιημένη,δε 
	θα εμφανιστούν.
	
	Παραδείγματα:
	
	    LIST *!joeuser@foo.com
	        Εμφανίζει όλα τα ψευδώνυμα που είναι κατοχυρωμένα
	    στη διεύθυνση joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Εμφανίζει όλα τα κατοχυρωμένα ψευδώνυμα που
	        περιέχουν τη λέξη Bot (με πεζά ή κεφαλαία).
	
	    LIST *!*@*.bar.org
	        Εμφανίζει όλα τα ψευδώνυμα που ανοίκουν σε χρήστες
	        που συνδέονται με διεύθυνση bar.org.

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
	Σύνταξη: GLIST
	
	Εμφανίζει όλα τα ψευδώνυμα που ανήκουν στην ίδια ομάδα.

NICK_HELP_STATUS
	Σύνταξη: STATUS ψευδώνυμο(α)...
	
	Σας ενημερώνει για το αν ο χρήστης που χρησιμοποιεί το
	ψευδώνυμο που δώσατε, έχει αναγνωριστεί σαν κάτοχός του.
	Το αποτέλεσμα της εντολής έχει τη μορφή:
	
	    ψευδώνυμο κωδικός-κατάστασης
	
	όπου ψευδώνυμο είναι το ψευδώνυμο για το οποίο θέλετε να,μάθετε
	την κατάστασή του και όπου κωδικός-κατάστασης είναι ένας
	από τους παρακάτω:
	
	    0 - δεν υπάρχει τέτοιος χρήστης συνδεδεμένος ή το ψευδώνυμο είναι μη κατοχυρωμένο
	    1 - ο χρήστης δεν αναγνωρίστηκε σαν κάτοχος του ψευδωνύμου
	    2 - ο χρήστης αναγνωρίστηκε σαν κάτοχος μέσω της λίστας πρόσβασης μόνο
	    3 - ο χρήστης αναγνωρίστηκε σαν κάτοχος του ψευδωνύμου μέσω της αναγνώρισης με κωδικό
	
	Μέχρι δεκαέξι ψευδώνυμα μπορούν να χρησιμοποιηθούν με κάθε εντολή; Τα
	υπόλοιπα θα αγνοούνται. If no nickname is given, your status
	will be returned.

NICK_HELP_SENDPASS
	Σύνταξη: SENDPASS ψευδώνυμο
	
	Δίνει τον κωδικό του ψευδώνυμου μέσω the e-mail.
	Αυτή η εντολή είναι πραγματικά χρήσιμη για να πάρει κάποιος τον
	χαμένο κωδικό του και με περισσότερη ασφάλεια.
	
	Μόνο οι IRC operators στα δίκτυα που είναι μπορούν να το κάνουν.
	
	Αυτή η εντολή είναι αδύνατη αν έχει ενεργοποιηθεί η κωδικοποίηση των κωδικών.

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
	Οι Services admins (επικεφαλείς) μπορούν να διαγράψουν ένα ψευδώνυμο χωρίς να
	χρειαστεί να κάνουν εντολή αναγνώρισης και καν αλλαγή ψευδώνυμου, και μπορούν να
	δουν την  λίστα access  για κάθε ψευδώνυμο (%R%S ACCESS LIST nick).
	
NICK_SERVADMIN_HELP_LOGOUT
	Σύνταξη: LOGOUT [ψευδώνυμο [REVALIDATE]]
	
	Χωρίς καμία παράμετρο, κάνει το αντίθετο της εντολής IDENTIFY (αναγνώρισης) 
	δηλ σε κάνει μη αναγνωρίσημο κάτοχο του ψευδώνυμου πλέον.
	Σημείωση, όπως και ναχει, δεν θα σου ζητηθεί να ξανακάνεις εντολή αναγνώρισης.
	    
	Με την παράμετρο αυτή, κάνει το ίδιο για το δωσμένο ψευδώνυμο. Αν κάνεις 
	REVALIDATE, οι υπηρεσίες θα σε ρωτήσουν για ποιο ψευδώνυμο.
	Αυτό το κάνουν μόνο οι (επικεφαλείς) Services admins.
	
NICK_SERVADMIN_HELP_DROP
	Σύνταξη: DROP [ψευδώνυμο]
	
	Χωρίς την παράμετρο, μπορεί να σβηστεί το ψευδώνυμο από του
	%S την βάση δεδομένων..
	
	Με την παράμετρο, σβήνει ο ψευδώνυμο από την βάση δεδομένων.
	Μπορεί να σβήσεις κάποιο ψευδώνυμο που μπορεί να είναι στην ομάδα σου χωρίς 
	να έχεις ξεχωριστά δικαιώματα. Το σβήσιμο του ψευδώνυμου έχουν το δικαίωμα να το 
	κάνουν οι (επικεφαλείς) Services admins.
	
NICK_SERVADMIN_HELP_INFO
	
	Οι Services admins μπορούν να χρησιμοποιούν την ALL παράμετρο για 
	κάθε ψευδώνυμο.

NICK_SERVADMIN_HELP_LIST
	Σύνταξη: LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	Δίνει την λίστα όλων των κατοχυρωμένων ψευδώνυμων που ταιριάζουν με το ανάλογο
	pattern, σε σύνταξη nick!user@host.  Ψευδώνυμα με επιλογή PRIVATE
	θα φαίνονται μόνο στους (επικεφαλείς) Services admins.  Ψευδώνυμα με
	επιλογή NOEXPIRE θα έχουν ένα ! μπροστά από το ψευδώνυμο το οποίο θα το
	λέπουν οι (επικεφαλείς) Services admins.
	    
	Παραδείγματα:
	
	    LIST *!joeuser@foo.com
	        Εμφανίζει όλα τα κατοχυρωμένα ψευδώνυμα που ανήκουν στο joeuser@foo.com.
	
	    LIST *Bot*!*@*
	        Εμφανίζει όλα τα κατοχυρωμένα ψευδώνυμα Bot.
	        
	    LIST * NOEXPIRE
	        Εμφανίζει όλα τα κατοχυρωμένα ψευδώνυμα που είναι σε no-expire.
	        
NICK_SERVADMIN_HELP_ALIST
	Syntax: ALIST [nickname] [level]
	
	With no parameters, lists channels you have access on. With
	one parameter, lists channels that nickname has access 
	on. With two parameters lists channels that nickname has 
	level access or greater on.

	This use limited to Services admins.

NICK_SERVADMIN_HELP_GLIST
	Σύνταξη: GLIST [ψευδώνυμο]
	
	Χωρίς την παράμετρο, εμφανίζει όλα τα ψευδώνυμα που είναι 
	στην ομάδα σου.
	    
	Με την παράμετρο, εμφανίζει όλα τα ψευδώνυμα που είναι στην ομάδα
	του ζητούμενου ψευδώνυμου. Μόνο οι Services admins 
	μπορούν να το κάνουν.
	
NICK_SERVADMIN_HELP_GETPASS
	Σύνταξη: GETPASS ψευδώνυμο
	
	Δίνει τον χαμένο κωδικό στο συγκεκριμένο ψευδώνυμο.  Σημείωση όταν
	η εντολή αυτή χρησιμοποιείται, το μήνυμα που θα πάει για τον
	συγκεκριμένο θα καταγραφεί και η εντολή που εκτελέστηκε και το ψευδώνυμο
	και θα σταλεί μέσω WALLOPS/GLOBOPS.
	
	Μόνο για τους Services admins.
	
	Αυτή η εντολή δεν είναι διαθέσιμη αν είναι ενεργή η κωδικοποίηση.

NICK_SERVADMIN_HELP_GETEMAIL
	Syntax: GETEMAIL user@emailhost

	Returns the matching nicks that used given email. Note that
	you can not use wildcards for either user or emailhost. Whenever
	this command is used, a message including the person who issued
	the command and the email it was used on will be logged.

	Limited to Services admins.

NICK_SERVADMIN_HELP_FORBID
	Σύνταξη: FORBID ψευδώνυμο [λόγος]
	
	Δεν επιτρέπει στο ψευδώνυμο να κατοχυρωθεί ή να χρησιμοποιηθεί από
	κανέναν.  Μπορεί να ακυρωθεί αν διαγραφεί τελείως το ψευδώνυμο.
	
	Σε ορισμένα δίκτυα, ο λόγος που γίνεται αυτό χρειάζεται.
	
	Μόνο για τους Services admins.

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
	    GETPASS    Βλέπεις τον κωδικό του συγκεκριμένου καναλιού
CHAN_HELP_CMD_FORBID
	    FORBID     Καταστέλει το κανάλι,δεν μπορεί να το χρησιμοποιήση κανείς
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    Prevent a channel from being used preserving
	               channel data and settings
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  Releases a suspended channel
CHAN_HELP_CMD_STATUS
	    STATUS     Επιστρέφει την συγκεκριμένη access level του χρήστη στο
	               κανάλι
CHAN_HELP_CMD_REGISTER
	    REGISTER   Κατοχυρώνει ένα κανάλι
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   Σας αναγνωρίζει με κωδικό
CHAN_HELP_CMD_SET
	    SET        Ρυθμίζει τις επιλογές και τις πληροφορίες
	               του καναλιού	
CHAN_HELP_CMD_AOP
	    AOP        Αλλάζει την λίστα των AOP χρηστών
CHAN_HELP_CMD_SOP
	    SOP        Αλλάζει την λίστα των SOP χρηστών
CHAN_HELP_CMD_ACCESS
	    ACCESS     Αλλάζει τη λίστα των εξουσιοδοτημένων χρηστών
CHAN_HELP_CMD_LEVELS
	    LEVELS     Επανακαθορίζει το νόημα των επιπέδων πρόσβασης
CHAN_HELP_CMD_AKICK
	    AKICK      Συντηρεί τη λίστα αυτόματης απόρριψης
CHAN_HELP_CMD_DROP
	    DROP       Ακυρώνει την κατοχύρωση ενός καναλιού
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   Στέλνει τον χαμένο κωδικό του καναλιού
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
	Ο %S σας επιτρέπει να κατοχυρώσετε και να χειριστείτε
	διάφορες λειτουργίες των καναλιών.  Ο %S μπορεί συνήθως
	να αποτρέψει κακόβουλους χρήστες να κάνουν "κατάληψη" σε
	κανάλια, με το να περιορίζει τις αρμοδιότητες των
	διαχειριστών καναλιών.  Οι διαθέσιμες εντολές, βρίσκονται
	παρακάτω.  Για να τις χρησιμοποιήσετε, πληκτρολογήστε
	%R%S εντολή.  Για περισσότερες πληροφορίες σε μία
	συγκεκριμένη εντολή, πληκτρολογήστε %R%S HELP εντολή.
	 
	
CHAN_HELP_EXPIRES
	
	Σημείωση: οποιοδήποτε κανάλι δεν χρησιμοποιείται για %d μέρες
	(δηλ κανένας χρήστης που έχει access δεν μπένει στο κανάλι
	για εκείνη την περίοδο των ημερών) θα διαγραφεί αυτόματα.

CHAN_HELP_REGISTER
	Σύνταξη: REGISTER κανάλι κωδικός περιγραφή
	
	Κατοχυρώνει ένα κανάλι στη βάση δεδομένων του %S.  Για
	να χρησιμοποιήσετε αυτή την εντολή, πρέπει πρώτα να είστε
	διαχειριστής καναλιού μέσα στο κανάλι που προσπαθείτε να
	κατοχυρώσετε.  Ο κωδικός χρησιμοποιείται με την εντολή
	IDENTIFY για να σας επιτρέπει να κάνετε αλλαγές στις
	ρυθμίσεις του καναλιού αργότερα.  Η τελευταία παράμετρος,
	η οποία και πρέπει να συμπεριληφθεί, είναι μία γενική	
	περιγραφή για το σκοπό ύπαρξης του καναλιού.
	
	Όταν κατοχυρώνετε ένα κανάλι, ορίζεστε σαν "ιδρυτής" του.
	Ο ιδρυτής ενός καναλιού, επιτρέπεται να αλλάξει όλες τις
	ρυθμίσεις του;  Ο	%S επίσης θα του δώσει αυτόματα
	αρμοδιότητες ιδρυτή και διαχειριστή καναλιού, με την είσοδό
	του στο κανάλι.Δείτε την εντολή ACCESS command.
	(%R%S HELP ACCESS) για πληροφορίες σχετικά με την παροχή μέρους των
	αρμοδιοτήτων αυτών, σε άλλους χρήστες του καναλιού.
	    
	ΣΗΜΕΙΩΣΗ: Για να κατοχυρώσετε ένα κανάλι, πρέπει να έχετε
	πρώτα κατοχυρώσει το ψευδώνυμό σας.  Αν δεν το έχετε κάνει,
	%R%s HELP για περισσότερες πληροφορίες σχετικά.

CHAN_HELP_IDENTIFY
	Σύνταξη: IDENTIFY κανάλι κωδικός
	
	Σε αναγνωρίζει ο %S ως founder του ζητούμενου καναλιού.
	Πολλές επιλογές απαιτούν αυτή την εντολή πριν συνεχίσετε με τις
	άλλες.  Ο κωδικός θα πρέπει να είναι ο ίδιος με αυτόν που κάνατε
	την εντολή κατοχύρωσης.
	
CHAN_HELP_LOGOUT
	Σύνταξη: LOGOUT κανάλι ψευδώνυμο
	
	Αυτή η εντολή είναι για ψευδώνυμα που δεν κάνουν εντολή αναγνώρισης
	για το ζητούμενο κανάλι.
	
	Αν είσαι ο founder του καναλιού, μπορείς να κάνεις log out οποιοδήποτε,
	εκτός και αν κάνεις μόνο log out τον εαυτό σου.

CHAN_HELP_DROP
	Σύνταξη: DROP κανάλι
	
	Κάνει το κανάλι ξεκατοχυρώσημο,το αφήνει ελεύθερο χωρίς founder. Για να
	γίνει αυτό πρέπει ο founder, να κάνει IDENTIFY πρώτα και ύστερα drop.

CHAN_HELP_SET
	Σύνταξη: SET κανάλι επιλογή παράμετροι
	
	Επιτρέπουν στον founder του καναλιού να επιλέξει και να ρυθμίσει το κανάλι
	του και άλλες πληροφορίες.
	
	Διαθέσιμες επιλογές:
	
	    FOUNDER       Αλλάζει τον founder του καναλιού
	    SUCCESSOR     Αλλάζει τον successor του καναλιού
	    PASSWORD      Αλλάζει τον κωδικό του founder
	    DESC          Αλλάζει την περιγραφή του καναλιού
	    URL           Συσχετίζει ένα URL με το κανάλι
	    EMAIL         Συσχετίζει ένα E-mail με το κανάλι
	    ENTRYMSG      Αλλάζει το μήνυμα εισαγωγής που στέλνετε σε όλους τους
	                     χρήστες που μπένουν στο κανάλι
	    BANTYPE       Αλλάζει τον τύπο του ban που θα κάνουν τα Services
	    MLOCK         Κλείδωμα των Modes του καναλιού
	    KEEPTOPIC     Να αλλάζει το topic όταν δεν χρησιμοποιείται το κανάλι
	    OPNOTICE      Να στέλνει μέσα στο κανάλι μήνυμα ποιος χρησιμοποιεί OP/DEOP 
	         εντολές
	    PEACE         Να ελλατώνει την χρήση "βίαιων" εντολών
	    PRIVATE       Να μην εμφανίζεται το κανάλι όταν γίνεται εντολή LIST
	    RESTRICTED    Απαγόρευση εισόδου στο κανάλι αν δεν έχεις access εκεί
	    SECURE        Ενεργοποίηση του %S ,μελλοντική ασφάλεια
	    SECUREOPS     Μόνο όσοι έχουν access θα έχουν op
	    SECUREFOUNDER Ασφάλεια του founder από το κανάλι
	    SIGNKICK      Εμφαν. kicks τα οποία γίνονται με την εντολή KICK
	    TOPICLOCK     Το Topic μπορεί να αλλάξει με την εντολή TOPIC
	    XOP           Αν είναι ON δουλεύει με AOP/SOP,αν είναι off με access
	
	Γράψε %R%S HELP επιλογή για περισσότερες πληροφορίες
	για την κάθε εντολή.

CHAN_HELP_SET_FOUNDER
	Σύνταξη: SET κανάλι FOUNDER ψευδώνυμο
	
	Αλλάζει τον founder του καναλιού.  Το νέο ψευδώνυμο πρέπει να είναι
	κατοχυρωμένο.

CHAN_HELP_SET_SUCCESSOR
	Σύνταξη: SET κανάλι SUCCESSOR ψευδώνυμο
	
	Αλλάζει τον successor του καναλιού.  Αν το ψευδώνυμο του founder
	λήξει ή σβηστεί (drop) τότε το κανάλι θα είναι ακόμα κατοχυρωμένο.
	Ο successor θα γίνει ο νέος founder του καναλιού.
	Αν όμως, ο successor έχει κατοχυρώσει πολλά κανάλια,
	αν δηλ έχει (%d), το κανάλι θα σβηστεί,σαν να μην είχε οριστεί
	successor.  Το νέο ψευδώνυμο θα πρέπει να είναι κατοχυρωμένο.
	
CHAN_HELP_SET_PASSWORD
	Σύνταξη: SET κανάλι PASSWORD κωδικό
	
	Αλλάζετε τον κωδικό που κάνετε εντολή αναγνώρισης ως founder του
	καναλιού.

CHAN_HELP_SET_DESC
	Σύνταξη: SET κανάλι DESC περιγραφή
	
	Αλλάζετε την περιγραφή του καναλιού, η οποία φένεται με την εντολή
	LIST και INFO.

CHAN_HELP_SET_URL
	Σύνταξη: SET κανάλι URL [url]
	
	Συσχετίζει ένα URL με το κανάλι.  Αυτό το URL θα
	εμφανίζεται όταν κάποιος θα κοιτάζεις τις πληροφορίες του καναλιού
	με την εντολή INFO.  Αν δεν δωθεί καμία παράμετρος,
	θα σβηστεί οποιοδήποτε URL είχε δωθεί παλαιότερα στο κανάλι.

CHAN_HELP_SET_EMAIL
	Σύνταξη: SET κανάλι EMAIL [διεύθυνση]
	
	Συσχετίζει μια διεύθυνση E-mail με το κανάλι.
	Αυτή η διεύθυνση εμφανίζεται όταν κάποιος θέλει να δει
	διάφορες πληροφορίες σχετικά με το κανάλι, με την εντολή INFO.
	Αν δεν δωθεί καμία παράμετρος, διαγράφει οποιαδήποτε διεύθυνση E-mail
	είχε δωθεί στο κανάλι.

CHAN_HELP_SET_ENTRYMSG
	Σύνταξη: SET κανάλι ENTRYMSG [μήνυμα]
	
	Αλλάζει το μήνυμα εισαγωγής το οποίο στέλνετε σε κάθε χρήστη μέσω /notice
	(παρατήρηση) όταν αυτός μπένει στο κανάλι.  Αν δεν δωθεί καμία παράμετρος,
	δεν θα στέλνετε κανένα μήνυμα στο χρήστη,ο οποίος θα εισέρχεται στο κανάλι.

CHAN_HELP_SET_BANTYPE
	Σύνταξη: SET κανάλι BANTYPE τύπος-ban
	
	Αλλάζει τον τύπο του ban, ο οποίος θα χρησιμοποιείται από τις υπηρεσίες όταν
	θα χρειάζεται να κάνει κάποιον ban από το κανάλι (πχ akick).
	
	Οι  τύποι ban είναι από 0 έως 3 και σημαίνει:
	
	0: το ban θα είναι της μορφής *!user@host
	1: το ban θα είναι της μορφής *!*user@host
	2: το ban θα είναι της μορφής *!*@host
	3: το ban θα είναι της μορφής *!*user@*.domain

CHAN_HELP_SET_KEEPTOPIC
	Σύνταξη: SET κανάλι KEEPTOPIC {ON | OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή topic retention του καναλιού.
	Όταν το topic retention είναι on, το topic του καναλιού
	θα υπενθυμίζεται από τον %S μετά τον τελευταίο χρήστη που θα φύγει από
	το κανάλι, και θα αποθηκεύεται για την επόμενη φορά.
	
CHAN_HELP_SET_TOPICLOCK
	Σύνταξη: SET κανάλι KEEPTOPIC {ON | OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή κλείδωμα topic του καναλιού.
	Όταν το κλείδωμα topic είναι on, ο %S δεν θα επιτρέπει σε κανέναν να αλλάξει
	το topic του καναλιού εκτός της εντολής TOPIC του.
	
CHAN_HELP_SET_MLOCK
	Σύνταξη: SET κανάλι MLOCK modes
	    
	Καθορίζει το κλείδωμα των modes για το κανάλι. Ο %S
	σας επιτρέπει να καθορίσετε συγκεκριμένα modes για το κανάλι
	τα οποία θα είναι πάντα ενεργοποιημένα (ή όχι).  Τα modes
	που μπορούν να κλειδωθούν είναι: i, k, l, m, n, p, s, και t.
	Κάθε ένα από αυτά τα modes μπορεί να κλειδωθεί, να ξεκλειδωθεί να
	μη συμπεριληφθεί καθόλου.
	
	Η παράμετρος modes συντάσσεται με τον ίδιο τρόπο όπως η εντολή 
	/MODE. Έτσι τα modes μετά από ένα + κλειδώνονται και τα modes μετά 
	από ένα - ξεκλειδώνονται.
	    
	Προσοχή:  Αν χρησιμοποιείτε το mode κλειδώματος με κλειδί,
	όπως στο δεύτερο παράδειγμα παρακάτω, θα πρέπει επίσης να
	ενεργοποιήσετε την επιλογή RESTRICTED για το κανάλι (δείτε
	HELP SET RESTRICTED), αλλιώς οποιοσδήποτε μπαίνει στο
	κανάλι όταν αυτό είναι άδειο, θα μπορεί να δεί το κλειδί!
	
	Παραδείγματα:
	
	    SET #κανάλι MLOCK +nt-iklps
	        Ενεργοποιεί τα modes n και t, και απενεργοποιεί τα modes i,
	        k, l, p και s. Το mode m, δε συμπεριλαμβάνεται για να μπορεί
	    να είναι ενεργοποιημένο ή μη.
	
	    SET #κανάλι MLOCK +knst-ilmp my-key
	        Ενεργοποιεί τα modes k, n, s και t, και απενεργοποιεί τα
	        modes i, l, m και p.  Επίσης καθορίζει το κλειδί του καναλιού
	        να είναι "my-key".
	
	    SET #κανάλι MLOCK +
	        Αφαιρεί το κλείδωμα των modes.  Όλα τα modes του
	        καναλιού είναι ελεύθερα να ενεργοποιηθούν ή να
	    απενεργοποιηθούν.

CHAN_HELP_SET_PEACE
	Σύνταξη: SET κανάλι PEACE {ON | OFF}
	
	Ενεργοποιεί ή απεεργοποιεί την εντολή peace στο κανάλι.
	Όταν η εντολή peace είναι on, ο χρήστης δεν μπορεί να κάνει kick,
	ban ή να αφαιρέσει op από κάποιον χρήστη ο οποίος έχει μεγαλύτερο
	level από αυτόν ή ίσο με αυτόν μέσω εντολών %S.

CHAN_HELP_SET_PRIVATE
	Σύνταξη: SET κανάλι PRIVATE {ON | OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή private για ένα κανάλι.
	Όταν ενεργοποιείται η private εντολή,  το κανάλι δε συμπεριλαμβάνεται
	στη λίστα που προκύπτει από την εντολή %R%S LIST.

CHAN_HELP_SET_RESTRICTED
	Σύνταξη: SET κανάλι RESTRICTED {ON | OFF}
	
	Enables or disables the restricted access option for a
	channel.  When restricted access is set, users not on
	the access list will instead be kicked and banned from the channel.
	
CHAN_HELP_SET_SECURE
	Σύνταξη: SET κανάλι SECURE {ON | OFF}
	
	Ενεργοποιεί ή απενεργοποιεί τις δυνατότητες ασφάλειας του %S για
	ένα κανάλι.  Όταν ενεργοποιείται το SECURE, μόνο οι χρήστες που
	έχουν κατοχυρωμένα τα ψευδώνυμά τους με το %s και έχουν αναγνωριστεί
	με τον κωδικό τους, θα έχουν πρόσβαση στο κανάλι όπως ορίζεται και
	από τη λίστα πρόσβασης.

CHAN_HELP_SET_SECUREOPS
	Σύνταξη: SET κανάλι SECUREOPS {ON | OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή προστατευμένης διαχείρησης
	για ένα κανάλι.  Όταν ενεργοποιείται η προστατευμένη διαχείρηση, οι
	χρήστες που δεν είναι στη λίστα πρόσβασης, δε μπορούν να πάρουν op.
	
CHAN_HELP_SET_SECUREFOUNDER
	Σύνταξη: SET κανάλι SECUREFOUNDER {ON | OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή ασφαλής founder για ένα κανάλι.
	Όταν ενεργοποιείται η επιλογή ασφαλής founder, μόνο ο πραγματικός founder 
	μπορεί να κάνει drop (σβήσει) το κανάλι, να αλλάξει τον κωδικό του, 
	μόνο ο founder και ο successor, και όχι αυτοί που κάνουν εντολή 
	αναγνώρισης με τον %S.
	    
CHAN_HELP_SET_SIGNKICK
	Σύνταξη: SET κανάλι SIGNKICK {ON | LEVEL | OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή signed kicks του
	καναλιού.  Όταν ενεργοποιείται η επιλογή SIGNKICK, τα kicks θα 
	εκτελούνται  %S KICK command will have the nick that used the
	με εντολή περιέχοντας τον λόγο.
	
	Αν χρησιμοποιείς LEVEL, αυτοί που έχουν μεγαλύτερη level ή ίση 
	με την SIGNKICK level του καναλιού δεν θα έχουν 
	kicks signed. Δες %R%S HELP LEVELS για περισσότερες πληροφορίες. 

CHAN_HELP_SET_XOP
	Σύνταξη: SET κανάλι XOP {ON | OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή xOP system για ένα κανάλι.
	Όταν ενεργοποιείται η επιλογή XOP, χρησιμοποιείς τις 
	εντολές AOP/SOP/VOP, για να δώσεις στους χρήστες που
	θες δικαιώματα στο κανάλι.Αλλιώς χρησιμοποιείς την εντολή ACCESS.
	    
	Τεχνική σημείωση: Αν έχεις απενεργοποιήση την xOP εντολή,χρησιμοποιείς 
	την εντολή access.Αν δώσεις access σε χρήστες και μετά για κάποιο λόγο 
	κάνεις το xOP ενεργό,θα πρέπει να δώσεις δικαιώματα στους χρήστες
	αυτούς με την xOP εντολή!
	    
	Αν από xOP system το γυρίσετε σε access system
	δεν θα υπάρξει κανένα πρόβλημα.

CHAN_HELP_SET_OPNOTICE
	Σύνταξη: SET κανάλι OPNOTICE {ON | OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή ενημέρωσης op για ένα κανάλι.
	Όταν ενεργοποιείται η ενημέρωση op, ο %S στέλνει ένα notice στους ops 
	του καναλιού, κάθε φορά που χρησιμοποιούνται οι εντολές OP, DEOP, AKICK και 
	ACCESS από κάποιο χρήστη για το κανάλι.
	
CHAN_HELP_AOP
	Σύνταξη: AOP κανάλι ADD ψευδώνυμο
	        AOP κανάλι DEL {ψευδώνυμο | νούμερο | λίστα}
	        AOP κανάλι LIST [μάσκα | λίστα]
	        AOP κανάλι CLEAR
	
	Αλλάζει την AOP (AutoOP) λίστα του καναλιού. Η λίστα AOP 
	δίνει στους χρήστες το δικαίωμα να γίνονται αυτόματα opped στο κανάλι σας,
	να κάνουν unban ή invite τους εαυτούς τους αν χρειάζεται, να έχουν το δικό 
	τους μήνυμα χαιρετισμού όταν μπένουν σε ένα κανάλι, και άλλα πολλά.
	    
	Η εντολή AOP ADD προσθέτει ένα ψευδώνυμο στην λίστα AOP.
	    
	Η εντολή AOP DEL αφαιρεί ένα ψευδώνυμο από την λίστα AOP.
	Αν η λίστα δίνεται με θέσεις-νούμερα, τότε αυτές οι θέσεις διαγράφονται.
	(Δες το παράδειγμα με την εντολή LIST.)
	
	Η εντολή AOP LIST εμφανίζει την λίστα AOP.  Αν δωθεί μια
	συγκεκριμένη μάσκα, μόνο οι θέσεις που ταιριάζουν με αυτήν θα εμφανιστούν.
	Αν κάνετε list με τις θέσεις-νούμερα,τότε μόνο τις θέσεις που δώσατε θα 
	δείτε.  Δηλ τα αντίστοιχα ψευδώνυμα που ανήκουν στις θέσεις αυτές.
	Για παράδειγμα:

	   AOP #κανάλι LIST 2-5,7-9
	      Εμφανίζει από την λίστα AOP τις θέσεις με τα 
	      νούμερα 2 έως 5 και 7 έως 9.
	            
	Η εντολή AOP CLEAR καθαρίζει όλες τις θέσεις από την λίστα AOP.
	    
	Οι εντολές AOP ADD και AOP DEL είναι περιορισμένες στους
	SOPs ή επάνω, ενώ η εντολή AOP CLEAR μπορεί μόνο να χρησιμοποιηθεί
	από τον founder του καναλιού. Όμως, οποιοσδήποτε χρήστης μπορεί να
	χρησιμοποιήσει την εντολή AOP LIST.
	
	Αυτή η εντολή μπορεί να είναι απενεργοποιημένη από το κανάλι σας, και
	σε αυτή την περίπτωση θα χρησιμοποιήσετε την εντολή access list. Δείτε 
	%R%S HELP ACCESS για πληροφορίες σχετικά με την εντολή access list,
	και %R%S HELP SET XOP για να ξέρετε πως θα επιλέξετε ανάμεσα στο 
	access list και xOP list σύστημα.

CHAN_HELP_HOP
	Σύνταξη: HOP κανάλι ADD ψευδώνυμο
	        HOP κανάλι DEL {ψευδώνυμο | θέσεις-νου | list}
	    HOP κανάλι LIST [μάσκα | list]	        
	        HOP κανάλι CLEAR
	
	Αλλάζει την λίστα HOP (HalfOP) του καναλιού. Η λίστα HOP 
	δίνει στους χρήστες το δικαίωμα να είναι αυτόματα halfopped στο 
	κανάλι σας.
	
	Η εντολή HOP ADD προσθέτει ένα ψευδώνυμο στην λίστα	HOP.
	    
	Η εντολή HOP DEL διαγράφει ένα ψευδώνυμο από την λίστα HOP.
	Αν δίνετε η λίστα με τις θέσεις-νούμερα, αυτές οι
	θέσεις διαγράφηκαν.  (Δες το παράδειγμα με την εντολή LIST.)
	
	Η εντολή HOP LIST εμφανίζει την λίστα HOP.  Αν δώσετε μια ζητούμενη 
	μάσκα τότε θα εμφανίσει τις θέσεις που ταιριάζει η μάσκα.
	Αν κάνετε list με τις θέσεις-νούμερα,τότε μόνο τις θέσεις που δώσατε θα 
	δείτε. Δηλ τα αντίστοιχα ψευδώνυμα που ανήκουν στις θέσεις αυτές.
	
	Παράδειγμα:
	
	   HOP #κανάλι LIST 2-5,7-9
	      Εμφανίζει από την λίστα HOP τις θέσεις με τα 
	      νούμερα 2 έως 5 και 7 έως 9.
	            
	Η εντολή HOP CLEAR διαγράφει όλες τισ θέσεις της λίστας HOP.
	    
	Οι εντολές HOP ADD, HOP DEL και HOP LIST είναι περιορισμένες 
	μόνο στους AOPs(αυτοί που είναι στην AOP access) ή ανώτερους, 
	ενώ η εντολή HOP CLEAR μπορεί μόνο να χρησιμοποιηθεί από τον
	founder του καναλιού.
	
	Αυτή η εντολή μπορεί να είναι απενεργοποιημένη από το κανάλι σας, και
	σε αυτή την περίπτωση θα χρησιμοποιήσετε την εντολή access list. Δείτε 
	%R%S HELP ACCESS για πληροφορίες σχετικά με την εντολή access list,
	και %R%S HELP SET XOP για να ξέρετε πως θα επιλέξετε ανάμεσα στο 
	access list και xOP list σύστημα.

CHAN_HELP_SOP
	Σύνταξη: SOP κανάλι ADD ψευδώνυμο
	    SOP κανάλι DEL {ψευδώνυμο | θέσεις-νου | list}
	        SOP κανάλι LIST [μάσκα | list]
	        SOP κανάλι CLEAR
	
	Αλλάζει την λίστα SOP (SuperOP) του καναλιού. Η λίστα SOP 
	δίνει το δικαίωμα στους χρήστες να προσθέτουν άλλους στην λίστα AOP,
	να προσθέτουν σε όσους χρειάζεται AutoKick, να εισάγουν στην λίστα 
	BadWord κακές λέξεις,να στέλνουν και να διαβάζουν τα μηνύματα του 
	καναλιού,και άλλα πολλά.
	
	Η εντολή SOP ADD προσθέτει ένα ψευδώνυμο στην λίστα SOP.
	    
	Η eντολή SOP DEL διαγράφει ένα ψευδώνυμο από την λίστα SOP.
	Αν σας δίνετε η λίστα με τα νούμερα-θέσεις, αυτές οι θέσεις
	διαγράφονται. (Δες την εντολή LIST.)
	
	Η εντολή SOP LIST εμφανίζει την λίστα SOP.  Αν δώσετε μια 
	συγκεκριμένη μάσκα, θα εμφανιστούν οι θέσεις που αντιστοιχεί η μάσκα 
	αυτή.  Αν κάνετε list με τις θέσεις-νούμερα,τότε μόνο τις θέσεις που 
	δώσατε θα δείτε.  Δηλ τα αντίστοιχα ψευδώνυμα που ανήκουν στις
	θέσεις αυτές.Παράδειγμα::

	   SOP #κανάλι LIST 2-5,7-9
	      Εμφανίζει από την λίστα SOP τις θέσεις με τα
	      νούμερα 2 έως 5 και 7 έως 9..
	      
	Η εντολή SOP CLEAR διαγράφει όλες τισ θέσεις της λίστας SOP.
	    
	Οι εντολές SOP ADD, SOP DEL και SOP LIST χρησιμοποιούνται από 
	τον founder του καναλιού.Όποιος είναι χρήστης στην λίστα AOP μπορεί
	να εκτελέσει εντολή SOP LIST.
	
	Αυτή η εντολή μπορεί να είναι απενεργοποιημένη από το κανάλι σας, και
	σε αυτή την περίπτωση θα χρησιμοποιήσετε την εντολή access list. Δείτε 
	%R%S HELP ACCESS για πληροφορίες σχετικά με την εντολή access list,
	και %R%S HELP SET XOP για να ξέρετε πως θα επιλέξετε ανάμεσα στο 
	access list και xOP list σύστημα.

CHAN_HELP_VOP
	Σύνταξη: VOP κανάλι ADD ψευδώνυμο
	        VOP κανάλι DEL {ψευδώνυμο | θέσεις-νου | list}
	        VOP κανάλι LIST [μάσκα | list]
	        VOP κανάλι CLEAR
	
	Αλλάζει την λίστα VOP (VOicePeople) του καναλιού.  
	Η λίστα VOP επιτρέπει στους χρήστες να είναι αυτόματα voiced και να δίνουν 
	voice στον εαυτό τους αν δεν είναι.
	    
	Η εντολή VOP ADD προσθέτει ένα ψευδώνυμο στην λίστα	VOP.
	    
	Η εντολή VOP DEL διαγράφει ένα ψευδώνυμο από την λίστα VOP.
	Αν δίνετε η λίστα με τις θέσεις-νούμερα, αυτές οι
	θέσεις διαγράφηκαν.  (Δες το παράδειγμα με την εντολή LIST.)
	
	Η εντολή VOP LIST εμφανίζει την λίστα VOP.  Αν δώσετε μια συγκεκριμένη
	μάσκα, θα εμφανίσει τις θέσεις που αντιστοιχούν στην μάσκα αυτή.
	Αν κάνετε list με τις θέσεις-νούμερα,τότε μόνο τις θέσεις που δώσατε θα 
	δείτε.  Δηλ τα αντίστοιχα ψευδώνυμα που ανήκουν στις θέσεις αυτές.
	
	Παράδειγμα:
	
	   VOP #κανάλι LIST 2-5,7-9
	      Εμφανίζει από την λίστα VOP τις θέσεις με τα
	      νούμερα 2 έως 5 και 7 έως 9.
	      
	Η εντολή VOP CLEAR διαγράφει όλες τισ θέσεις της λίστας VOP.
	    
	Τις εντολές VOP ADD, VOP DEL και VOP LIST τις χρησιμοποιούν 
	μόνο αυτοι που είναι στην AOP ή ανώτεροι, ενώ η εντολή VOP CLEAR 
	χρησιμοποιείται μόνο από τον founder του καναλιού.
	
	Αυτή η εντολή μπορεί να είναι απενεργοποιημένη από το κανάλι σας, και
	σε αυτή την περίπτωση θα χρησιμοποιήσετε την εντολή access list. Δείτε 
	%R%S HELP ACCESS για πληροφορίες σχετικά με την εντολή access list,
	και %R%S HELP SET XOP για να ξέρετε πως θα επιλέξετε ανάμεσα στο 
	access list και xOP list σύστημα.

CHAN_HELP_ACCESS
	Σύνταξη: ACCESS κανάλι ADD ψευδώνυμο βαθμός
	    ACCESS κανάλι DEL {ψευδώνυμο | θέσεις-νου | list}	        
	        ACCESS κανάλι LIST [μάσκα | list]
	        ACCESS κανάλι CLEAR
	
	Αλλάζει την access λίστα του καναλιού.  Η λίστα access
	καθορίζει ποιοι χρήστες θα έχουν op στο κανάλι ή
	access στις εντολές μέσω του %S για το κανάλι.  Διαφορετικός βαθμός
	του χρήστη από την access που έχει,διαφορετικά δικαιώματα θα έχει.
	%R%S HELP ACCESS LEVELS για περισσότερες και συγκεκριμένες
	πληροφορίες.  Οποιοδήποτε ψευδώνυμο που δεν είναι στην λίστα access έχει
	τυπικά access 0, γιατί τα levels από default δεν δίνουν δικαιώματα στο κανάλι
	όποιος έχει 0 να κάνει κάτι στο κανάλι, με access 0 είναι στο auto-deop.
	
	Η εντολή ACCESS ADD προσθέτει ένα ψευδώνυμο στην λίσταμε το ανάλογο
	βαθμό (level).  Αν κάποιο ψευδώνυμο είναι ήση στην λίστα και του δώεσετε ένα
	άλλο βαθμό από αυτό που είχε τότε αλλάζει ο βαθμός του(level).
	    
	Η εντολή ACCESS DEL αφαιρεί ένα ψευδώνυμο από την λίστα access.
	Αν σας δωθεί η λίστα με τις θέσεις-νούμερα,αυτές οι θέσεις έχουν διαγραφεί.
	(Δες το παράδειγμα της εντολής LIST.)
	
	Η εντολή ACCESS LIST εμφανίζει την λίστα access.  Αν δώσετε μια 
	συγκεκριμένη μάσκα, θα σας εμφανίσει τις θέσεις που αντιστοιχεί αυτή η μάσκα.
	Αν κάνετε list με τις θέσεις-νούμερα,τότε μόνο τις θέσεις που δώσατε θα δείτε.
	Δηλ τα αντίστοιχα ψευδώνυμα που ανήκουν στις θέσεις αυτές.
	
	Παράδειγμα:

	   ACCESS #κανάλι LIST 2-5,7-9
	      Εμφανίζει από την λίστα access τις θέσεις με τα
	      νούμερα 2 έως 5 και 7 έως 9.
	      
	Η εντολή ACCESS CLEAR διαγράφει όλες τισ θέσεις της λίστας access.
	
CHAN_HELP_ACCESS_LEVELS
	User access levels
	
	Από default, οι επόμενες access βαθμοί (levels) είναι οι εξής:
	
	   Founder   Full access στον %S για εντολές καναλιού; αυτόματο
	                     opping όταν μπένει στο κανάλι.  Σημείωση
	                     μόνο ένα άτομο έχει πρόσβαση σαν founder
	                     (δεν μπορεί να δώσει μέσω εντολής ACCESS
	                     βαθμό ίδιο με του founder).
	        10   Πρόσβαση στην εντολή AKICK; αυτόματο opping.
	         5   Αυτόματο opping..
	         3   Αυτόματο voicing.
	         0   Κανένα ιδιαίτερο δικαίωμα; μπορεί να πάρει op από άλλους
	                     ops (εκτός και αν είναι ενεργοποιημένα τα secure-ops).
	        <0   Δεν μπορεί να πάρει op.
	
	Αυτοί οι βαθμοί (levels) μπορούν να αλλαχτούν, ή να προσθέσετε νέα δικά σας,
	χρησιμοποιώντας την εντολή LEVELS; γράψτε %R%S HELP LEVELS για
	πληροφορίες.

CHAN_HELP_AKICK
	Σύνταξη: AKICK channel ADD {nick | mask} [reason]
	         AKICK channel STICK mask
	         AKICK channel UNSTICK mask
	         AKICK channel DEL {nick | mask | entry-num | list}
	         AKICK channel LIST [mask | entry-num | list]
	         AKICK channel VIEW [mask | entry-num | list]
	         AKICK κανάλι ENFORCE
	         AKICK κανάλι CLEAR
	
	Αλλάζει τη λίστα αυτόματης απόρριψης για ένα κανάλι.
	Εάν ένας χρήστης που βρίσκεται στη λίστα αυτόματης απόρριψης,
	προσπαθήσει να μπεί στο κανάλι, ο %S θα του απαγορέψει την είσοδο 
	(ban/kick).
	    
	Η εντολή AKICK ADD προσθέτει τη μάσκα που
	δίνεται στη λίστα αυτόματης απόρριψης.  Αν δωθεί μία αιτιολογία
	με την εντολή, αυτή η αιτιολογία θα χρησιμοποιηθεί στο kick του χρήστη.
	Διαφορετικά η προκαθορισμένη αιτιολογία είναι "You have been banned 
	from the channel" ("Έχει απαγορευθεί η είσοδός σας στο κανάλι").
	
	Η εντολή AKICK STICK κάνει μόνιμα bans τη μάσκα 
	που δίνεται. Αν κάποιος προσπαθήσει να το αφαιρέσει το ban, ο %S
	θα το ξαναβάλει αυτόματα. Δεν μπορείται να το χρησιμοποιήσετε για
	κατοχυρωμένα ψευδώνυμα.
	
	Η εντολή AKICK UNSTICK ακυρώνει την εντολή AKICK STICK,
	και έτσι θα μπορείτε να βγάλετε το ban από το κανάλι.
	    
	Η εντολή AKICK DEL διαγράφει τη μάσκα που
	δίνεται, από τη λίστα αυτόματης απόρριψης.  Βάζοντας ALL για
	μάσκα διαγράφονται όλα με τη μια.  Δε διαγράφει όμως τα bans που
	έχουν τοποθετηθεί από την αυτόματη απόρριψη.  Αυτά πρέπει να
	αφαιρεθούν ξεχωριστά.
	
	Η εντολή AKICK LIST εμφανίζει τη λίστα αυτόματης απόρριψης,
	ή επιλεκτικά μόνο εκείνες τις εγγραφές της λίστας που
	ταιριάζουν στη μάσκα που δίνεται.
	
	Η εντολή AKICK VIEW είναι πιο βελτιωμένη έκδοση από την εντολή
	AKICK LIST.
	
	Η εντολή AKICK ENFORCE κάνει τον %S να επαναδράση τη
	συγκεκριμένη AKICK λίστα βγάζοντας έξω τους χρήστες που ταιριάζουν με την μάσκα
	που είναι στην AKICK.
	
	Η εντολή AKICK CLEAR διαγράφει όλες τις θέσεις που περιέχονται στην
	λίστα akick.

CHAN_HELP_LEVELS
	Σύνταξη: LEVELS κανάλι SET τύπος βαθμός
	        LEVELS κανάλι {DIS | DISABLE} τύπος
	    LEVELS κανάλι LIST
	        LEVELS κανάλι RESET
	
	Η εντολή LEVELS επιτρέπει το καλό χειρισμό όσον αφορά
	την ρύθμιση της access levels που χρησιμοποιείται στο κανάλι.
	Με αυτή την εντολή, μπορείτε να χειρίζεστε την access level που
	που εκτελεί ο %S.  (Τις εντολές SET FOUNDER και SET PASSWORD
	τις χειρίζεται μόνο ο founder.)
	    
	Η εντολή LEVELS SET επιτρέπει την αλλαγή των access level (βαθμών).
	Η εντολή LEVELS DISABLE (ή DIS για συντομία)
	απενεργοποιεί αυτόματα μελλοντικά ή δεν δίνει δικαιώματα σε όσους έχουν access
	εκτός του founder καναλιού αυτού.
	Η εντολή αυτή LEVELS LIST δείχνει τα levels (βαθμούς) που έχει η κάθε 
	εντολή.
	Η εντολή LEVELS RESET σβήνει τα levels (βαθμούς) και επαναφέρει τα
	default levels (βαθμούς), δηλ αυτά τα levels που έχει ένα νέο κανάλι (δες
	την εντολή HELP ACCESS LEVELS).
	
	Για να δείς την λίστα με τις εντολές που μπορείς να αλλάξεις τα levels, δες
	την εντολή HELP LEVELS DESC.

CHAN_HELP_LEVELS_DESC
	Τα ακόλουθα feature/function καταλαβαίνονται.  Σημείωση,
	τα levels (βαθμοί) για AUTODEOP και NOJOIN είναι τα μέγιστα levels,
	ενώ όλα τα άλλα είναι τα ελάχιστα levels.

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	Σύνταξη: INFO κανάλι [ALL]
	
	Εμφανίζει πληροφορίες σχετικά με το όνομα του κατοχυρωμένου καναλιού,
	περιλαμβανομένου τον founder, την ώρα της κατοχύρωσης, πότε χρησιμοποιήθηκε 
	τελευταία φορά, και τα κλειδωμένα mode, αν υπάρχουν.
	Αν χρησιμοποιήσετε την παράμετρο ALL  
	θα εμφανιστεί το μήνυμα εισαγωγής του καναλιού και ο successor. 
	    
	Από default, η επιλογή ALL είναι ορισμένη μόνο σε αυτούς που έχουν
	founder access στο συγκεκριμένο κανάλι.

CHAN_HELP_LIST
	Σύνταξη: LIST pattern
	
	Εμφανίζει όλα τα κατοχυρωμένα κανάλια σε σχέση με αυτό που του δίνεις να ψάξει.
	(Τα κανάλια που έχουν επιλογή PRIVATE δεν εμφανίζονται.)

CHAN_HELP_OP
	Σύνταξη: OP [#κανάλι [ψευδώνυμο]]
	
	Η εντολή αυτή κάνει op το ζητούμενο ψευδώνυμο το οποίο είναι στο κανάλι.
	Αν δεν επιλέξετε ψευδώνυμο θα δώσει op στον εαυτό σας.
	Αν δεν δώσετε το κανάλι και το ψευδώνυμο, 8α σας δώσει op
	σε όλα τα κανάλια που είσαστε, με την προυπόθεση οτι έχετε
	το δικαίωμα στα κανάλια.(να έχετε access)
	
	Από default, χρησιμοποιείται από αυτούς που είναι AOPs ή από αυτούς που 
	έχουν βαθμό (level) 5 access και όσους έχουν παραπάνω level op/deop.

CHAN_HELP_DEOP
	Σύνταξη: DEOP [#κανάλι [ψευδώνυμο]]
	
	Η εντολή αυτή κάνει deop το ζητούμενο ψευδώνυμο το οποίο είναι στο κανάλι.
	Αν δεν επιλέξετε το ψευδώνυμο, θα κάνει deop στον εαυτό σας.
	Αν δεν δώσετε το κανάλι και το ψευδώνυμο, 8α κάνει deop τον εαυτό σας
	σε όλα τα κανάλια που είσαστε, με την προυπόθεση οτι έχετε
	το δικαίωμα στα κανάλια.(να έχετε access)
	
	Από default, χρησιμοποιείται από αυτούς που είναι AOPs ή από αυτούς που 
	έχουν βαθμό (level) 5 access και όσους έχουν παραπάνω level op/deop.
	
CHAN_HELP_VOICE
	Σύνταξη: VOICE [#κανάλι [ψευδώνυμο]]
	
	Η εντολή αυτή κάνει voice το ζητούμενο ψευδώνυμο το οποίο είναι στο κανάλι.
	Αν δεν επιλέξετε ψευδώνυμο θα δώσει voice στον εαυτό σας.
	Αν δεν δώσετε το κανάλι και το ψευδώνυμο, 8α δώσει voice στον εαυτό σας
	σε όλα τα κανάλια που είσαστε, με την προυπόθεση οτι έχετε
	το δικαίωμα στα κανάλια.(να έχετε access)
	
	Από default, χρησιμοποιείται από AOPs ή από αυτούς που έχουν level 5 access 
	και παραπάνω στο κανάλι, ή από τους VOPs ή αυτούς που έχουν level 3 
	και παραπάνω για να κάνουν voice τον εαυτό τους.

CHAN_HELP_DEVOICE
	Σύνταξη: DEVOICE [#κανάλι [ψευδώνυμο]]
	
	Η εντολή αυτή κάνει devoice το ζητούμενο ψευδώνυμο το οποίο είναι στο κανάλι.
	Αν δεν επιλέξετε ψευδώνυμο θα κάνει devoice τον εαυτό σας.
	Αν δεν δώσετε το κανάλι και το ψευδώνυμο, 8α σας κάνει devoice
	σε όλα τα κανάλια που είσαστε, με την προυπόθεση οτι έχετε
	το δικαίωμα στα κανάλια (να έχετε access).
	
	Από default, χρησιμοποιείται από AOPs ή από αυτούς που έχουν level 5 access 
	και παραπάνω στο κανάλι, ή από τους VOPs ή αυτούς που έχουν level 3 
	και παραπάνω για να κάνουν devoice τον εαυτό τους.
	
CHAN_HELP_HALFOP
	Σύνταξη: HALFOP [#κανάλι [ψευδώνυμο]]
	
	Η εντολή αυτή κάνει halfop το ζητούμενο ψευδώνυμο το οποίο είναι στο κανάλι.
	Αν δεν επιλέξετε ψευδώνυμο θα κάνει halfop τον εαυτό σας.
	Αν δεν δώσετε το κανάλι και το ψευδώνυμο, 8α σας δώσει halfop
	σε όλα τα κανάλια που είσαστε, με την προυπόθεση οτι έχετε
	το δικαίωμα στα κανάλια (να έχετε access).
	
	Από default, χρησιμοποιείται από AOPs και από αυτούς που έχουν level 5 access 
	και παραπάνω access στο κανάλι, ή από τους HOPs ή αυτούς που έχουν level 4 
	και παραπάνω για να κάνουν halfop τον εαυτό τους.

CHAN_HELP_DEHALFOP
	Σύνταξη: DEHALFOP [#κανάλι [ψευδώνυμο]]
	
	Η εντολή αυτή κάνει dehalfop το ζητούμενο ψευδώνυμο το οποίο είναι στο κανάλι.
	Αν δεν επιλέξετε ψευδώνυμο θα κάνει dehalfop τον εαυτό σας.
	Αν δεν δώσετε το κανάλι και το ψευδώνυμο, 8α σας κάνει dehalfop
	σε όλα τα κανάλια που είσαστε, με την προυπόθεση οτι έχετε 
	το δικαίωμα στα κανάλια (να έχετε access).
	
	Από default, χρησιμοποιείται από AOPs και από αυτούς που έχουν level 5 access 
	και παραπάνω access στο κανάλι, ή από τους HOPs ή αυτούς που έχουν level 4 
	και παραπάνω για να κάνουν dehalfop τον εαυτό τους.
	
CHAN_HELP_PROTECT
	Σύνταξη: PROTECT [#κανάλι [ψευδώνυμο]]
	
	Η εντολή αυτή προστατεύει το ζητούμενο ψευδώνυμο το οποίο είναι στο κανάλι.
	Αν δεν επιλέξετε ψευδώνυμο θα προστατεύσει τον εαυτό σας.
	Αν δεν δώσετε το κανάλι και το ψευδώνυμο, 8α προστατεύσει εσάς
	σε όλα τα κανάλια που είσαστε, με την προυπόθεση οτι έχετε
	το δικαίωμα στα κανάλια (να έχετε access).
	
	Από default, χρησιμοποιείται από τον founder, ή τους SOPs ή αυτούς που έχουν 
	level (βαθμό) 10 ή και παραπάνω για να προστατεύουν τον εαυτό τους.

CHAN_HELP_DEPROTECT
	Σύνταξη: DEPROTECT [#κανάλι [ψευδώνυμο]]
	
	Η εντολή αυτή βγάζει την προστασία το ζητούμενο ψευδώνυμο το οποίο είναι στο 
	κανάλι.  Αν δεν επιλέξετε ψευδώνυμο θα βγάλει την προστασία από τον εαυτό σας.
	Αν δεν δώσετε το κανάλι και το ψευδώνυμο, 8α βγάλει την προστασία από εσά
	σε όλα τα κανάλια που είσαστε, με την προυπόθεση οτι έχετε
	το δικαίωμα στα κανάλια (να έχετε access).
	
	Από default, χρησιμοποιείται από τον founder, ή τους SOPs ή αυτούς που έχουν 
	level (βαθμό) 10 ή και παραπάνω για να βγάλουν την προστασία από τον εαυτό τους.

CHAN_HELP_OWNER
	Σύνταξη: OWNER [#κανάλι]
	
	Σου δίνει owner status στο κανάλι (κάτι σαν founder). Αν δεν δίνετε 
	κανάλι, θα σου δώσει owner status σε όλα τα κανάλια που είσαι, 
	με την προυπόθεση οτι έχεις δικαιώματα.
	
	Μόνο για αυτούς που έχουν founder access στο κανάλι.

CHAN_HELP_DEOWNER
	Σύνταξη: DEOWNER [#κανάλι]
	
	Κάνει το ακριβώς αντίθετπ της εντολής owner. 
	 
	Μόνο για αυτούς που έχουν founder access στο κανάλι.
	
	
CHAN_HELP_INVITE
	Σύνταξη: INVITE κανάλι
	
	Ζητάει από τον %S να καλέσει το ανάλογο ψευδώνυμο να έρθει στο κανάλι.  
	
	Από default, χρησιμοποιείται από τους AOPs ή από αυτούς που έχουν level 5 και
	παραπάνω στο κανάλι.
	
CHAN_HELP_UNBAN
	Σύνταξη: UNBAN κανάλι
	
	Ζητάει από τον %S να αφαιρέσει όλα τα bans ανάλογα για ποιο κανάλι του ζητάς.
	    
	Από default, χρησιμοποιείται από τους AOPs ή αυτούς που έχουν level 5 και
	παραπάνω στο κανάλι.
	    
CHAN_HELP_KICK
	Σύνταξη: KICK [#κανάλι [ψευδώνυμο [λόγος]]]
	
	Κάνει kick το ψευδώνυμο που του δίνεις στο κανάλι. Αν δεν δώσεις ψευδώνυμο,
	θα κάνει kick εσένα. Αν δεν δώσεις κανάλι και ψευδώνυμο,
	θα κάνει kick εσένα σε όλα τα κανάλια που είσαι εφόσον έχεις το δικαίωμα 
	(access).
	    
	Από default, χρησιμοποιείται από τους AOPs ή αυτούς που έχουν level 5 access 
	και παραπάνω στο κανάλι.

CHAN_HELP_BAN
	Σύνταξη: BAN [#κανάλι [ψευδώνυμο [λόγος]]]
	
	Κάνει ban το ψευδώνυμο που του δίνεις στο κανάλι. Αν δεν δώσεις ψευδώνυμο,
	θα κάνει ban εσένα. Αν δεν δώσεις κανάλι και ψευδώνυμο,
	θα κάνει ban εσένα σε όλα τα κανάλια που είσαι εφόσον έχεις το δικαίωμα 
	(access).
	    
	Από default, χρησιμοποιείται από τους AOPs ή αυτούς που έχουν level 5 access 
	και παραπάνω στο κανάλι.

CHAN_HELP_TOPIC
	Σύνταξη: TOPIC κανάλι [topic]
	
	Ζητάς από τον %S να βάλει το topic που του έχεις πει.
	Αν το topic δεν δίνεται, θα βάλει άδειο topic
	στο κανάλι. Αυτή η εντολή είναι χρήσιμη αν έχεις κάνει στο κανάλι
	SET TOPICLOCK. Δες %R%S HELP SET TOPICLOCK
	για περισσότερες πληροφορίες.
	
	Από default, χρησιμοποείται μόνο από αυτούς που έχουν founder access στο
	κανάλι. 

CHAN_HELP_CLEAR
	Σύνταξη: CLEAR κανάλι τι ακριβώς
	
	Ζητάς από τον %S να καθαρίσει τις επιλογές που έχει το κανάλι.  τι ακριβώς
	μπορεί να είναι μια από τις ακόλουθες επιλογές:
	
	     MODES    Καθαρίζει όλα τα modes από το κανάλι (δηλ clears
	                  modes i,k,l,m,n,p,s,t).
	     BANS     Καθαρίζει όλα τα bans από το κανάλι.
	     EXCEPTS  Καθαρίζει όλα τα excepts από το κανάλι.
	     INVITES  Clears all invites on the channel.
	     OPS      Αφαιρεί το channel-operator status (mode +o) από
	                  όλους τους channel operators.
	     HOPS     Removes channel-halfoperator status (mode +h) from
	                  all channel halfoperators, if supported.
	     VOICES   Αφαιρεί τα "voice" status (mode +v) από τον καθένα.
	     USERS    Πετάει (kicks) όλους τους χρήστες από το κανάλι.
	
	Από default, χρησημοποιείται, μόνο από αυτούς που έχουν founder access στο
	κανάλι.
	
CHAN_HELP_GETKEY
	Σύνταξη: GETKEY κανάλι
	
	Σας δείχνει το κλειδί του συγκεκριμένου καναλιού. Αυτή η εντολή
	χρησιμοποιείται κυρίως από τα bots και/ή scripts.Μπορείται να
	βάλετε και εσείς με τον εξής τρόπο:
	
	KEY <κανάλι> <κλειδί>
	
	κλειδί είναι "ΚΑΝΕΝΑ ΚΛΕΙΔΙ" αν δεν υπάρχει κανένα κλειδί.
	
CHAN_HELP_SENDPASS
	Σύνταξη: SENDPASS κανάλι
	
	Στέλνει τον κωδικό του ανάλογου καναλιού μέσω e-mail address
	στον founder του. Αυτή η εντολή είναι χρήσιμη 
	για όσους έχουν χάσει τον κωδικό.
	
	Χρησιμοποιείται μόνο από IRC operators σε συγκεκριμένα δίκτυα.
	
	Αυτή η εντολή είναι αδύνατη όταν είναι ενεργοποιημένη η κωδικοποίηση.

CHAN_SERVADMIN_HELP
	
	Οι Services admins μπορούν να διαγράψουν οποιοδήποτε κανάλι,  χωρίς να κάνουν
	εντολή αναγνώρισης μέσω κωδικού, μπορούν να δούν την access, AKICK,
	και τα level οποιουδήποτε καναλιού.
	
CHAN_SERVADMIN_HELP_LOGOUT
	Σύνταξη: LOGOUT κανάλι [ψευδώνυμο]
	
	Με αυτή την εντολή, ένας founder ή κάποιος που έχει βαθμό founder κάνει το 
	αντίθετο από την εντολή αναγνώρισης (no-identify).
	    
	Μπορεί ένας founder ενός καναλιού, να κάνει log out οποιοδήποτε,
	ακόμα και τον εαυτό του.
	
	Αν είσαι Services admin, μπορείς να κάνεις log out 
	οποιοδήποε από το κανάλι χωρίς να είσαι ο founder ή να γίνεις ο founder
	του καναλιού. Επίσης, μπορείς να χρησιμοποιήσεις αυτή την παράμετρο για
	ψευδώνυμο; με αυτό θα κάνεις log out όλους τους αναγνωρισμένους (identified)
	χρήστες του καναλιού.
	
CHAN_SERVADMIN_HELP_DROP
	Σύνταξη: DROP κανάλι
	
	Ξε-κατοχυρώνει το ζητούμενο κανάλι.  Μόνο οι Services admins
	μπορουν να το κάνουν αυτό σε ένα κανάλι, χωρίς να κάνουν εντολή αναγνώρισης
	για το κανάλι.

CHAN_SERVADMIN_HELP_SET
	
	Οι Services admins μπορούν να επιλέξουν την NOEXPIRE εντολή, σε
	αυτά που δεν θέλουν ποτέ να λήξουν.
	Επίσης, οι Services admins μπορούν να αλλάξουν τις επιλογές για κάθε κανάλι
	χωρίς να κάνουν εντολή αναγνώρισης σε κάθε κανάλι.

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	Σύνταξη: SET κανάλι NOEXPIRE {ON | OFF}
	
	Με αυτή την εντολή μπορείς να κάνεις ένα κανάλι να μην λήγει ποτέ, χωρις να
	είσαι υποχρεωμένος να κάνεις εντολή αναγνώρισης στο χρονικό
	περιθώριο που σου δίνεται.
	
	Μόνο οι Services admins μπορούν να το κάνουν.

CHAN_SERVADMIN_HELP_INFO
	
	Οι Services admins μπορούν να χρησιμοποιήσουν την παράμετρο ALL για κάθε
	κανάλι.

CHAN_SERVADMIN_HELP_LIST
	Σύνταξη: LIST pattern [FORBIDDEN] [NOEXPIRE]
	
	Εμφανίζει όλα τα κατοχυρωμένα κανάλια σε σχέση με το ζητούμενο κανάλι.
	Κανάλια που είναι PRIVATE θα εμφανίζονται μόνο στους Services admins.
	Κανάλια που είναι NOEXPIRE θα έχουν ένα ! δίπλα από το όνομα του 
	καναλιού, μόνο για τους Services admins.
	
	Αν γράψετε για FORBIDDEN ή NOEXPIRE, θα δείτε τα κανάλια αυτά τα
	οποία είναι FORBIDden ή είναι NOEXPIRE.
	Αν γράψετε και τα δυο μαζί θα εμφανιστούν τα κανάλια που έχουν και τα
	δύο,δηλ αυτα που είναι FORBIDden και άλλα που είναι NOEXPIRE.
	Αυτές οι επιλογές είναι για τους Services admins.

CHAN_SERVADMIN_HELP_GETPASS
	Σύνταξη: GETPASS κανάλι
	
	Δίνει τον κωδικό του καναλιού.  Σημείωση όποτε
	χρησιμοποιείς αυτή την εντολή, θα εμφανιστεί το μήνυμα του
	χρήστη που εκτέλεσε αυτή την εντολή και το κανάλι για το οποίο
	χρησιμοποιήθηκε και θα σταλεί με WALLOPS/GLOBOPS.
	
	Μόνο για τους Services admins.
	
CHAN_SERVADMIN_HELP_FORBID
	Σύνταξη: FORBID κανάλι [λόγος]
	
	Δεν επιτρέπει σε οποιοδήποτε να κάνει κατοχύρωση ή χρήση του συγκεκριμένου
	καναλιού.  Μπορεί να ακυρωθεί αυτό με σβήσιμο του καναλιού (drop).
	
	Ο λόγος απαιτείται σε ορισμένα δίκτυα.
	
	Μόνο για τους Services admins.
	
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
	Σύνταξη: STATUS κανάλι ψευδώνυμο
	
	Επιστρέφει την συγκεκριμένη access level του ανάλογου ψευδώνυμου
	στο ανάλογο κανάλι.  Η απάντηση θα είναι της μορφής:
	
	    STATUS κανάλι ψευδώνυμο access-level
	
	Αν βρείτε λάθος, θα δείτε το εξής μήνυμα:
	
	    STATUS ERROR λάθος-μήνυμα
	
	Μόνο για τους Services admins.

###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP_CMD_SEND
	    SEND   Στέλνει ένα μήνυμα σε ένα ψευδώνυμο ή ένα κανάλι
MEMO_HELP_CMD_CANCEL
	    CANCEL Ακυρώνει το τελευταίο μήνυμα που στέλνετε
MEMO_HELP_CMD_LIST
	    LIST   Εμφανίζει λίστα με τα μηνύματα σας
MEMO_HELP_CMD_READ
	    READ   Διαβάζει ένα ή πολλά μηνύματα
MEMO_HELP_CMD_DEL
	    DEL    Διαγράφει ένα ή πολλά μηνύματα
MEMO_HELP_CMD_SET
	    SET    Αλλάζει τις επιλογές των μηνυμάτων
MEMO_HELP_CMD_INFO
	    INFO   Displays information about your memos
MEMO_HELP_CMD_RSEND
	    RSEND  Sends a memo and requests a read receipt
MEMO_HELP_CMD_CHECK
	    CHECK  Checks if last memo to a nick was read
MEMO_HELP_CMD_SENDALL
	    SENDALL  Send a memo to all registered users
MEMO_HELP_CMD_STAFF
	    STAFF  Send a memo to all opers/admins

MEMO_HELP_HEADER
	Ο %S είναι μια υπηρεσία που επιτρέπει στους χρήστες
	του IRC να στέλνουν μικρά γραπτά μηνύματα σε άλλους χρήστες
	του δικτύου (είτε βρίσκονται συνδεδεμένοι εκείνη τη στιγμή,
	είτε όχι) ή σε κανάλια(*).
	Τα ψευδώνυμα του αποστολέα και του παραλήπτη πρέπει να είναι
	κατοχυρωμένα για να χρησιμοποιηθεί η υπηρεσία.
	
	Οι εντολές του %S είναι:
	

MEMO_HELP_ADMIN
	not used.
MEMO_HELP_OPER
	not used.
MEMO_HELP_FOOTER
	
	Γράψτε %R%S HELP εντολή για βοήθεια σε
	οποιαδήποτε από τις παραπάνω εντολές.
	
	(*) Εξ'ορισμού, κάθε χρήστης με ελάχιστο επίπεδο πρόσβασης
	    10 σε ένα κανάλι, μπορεί να διαβάσει τα μηνύματα του
	    καναλιού. Αυτό μπορεί να αλλάξει με την εντολή
	    LEVELS του %s.

MEMO_HELP_SEND
	Σύνταξη: SEND {ψευδώνυμο | κανάλι} κείμενο μηνύματος
	
	Στέλνει στο ψευδώνυμο ή στο κανάλι που δίνεται, ένα μήνυμα που 
	περιέχει το κείμενο-μηνύματος.  Όταν το μήνυμα στέλνεται σε ένα ψευδώνυμο,
	memo-text.  When sending to a nickname, the recipient will
	ο παραλείπτης θα λάβει μια ειδοποίηση οτι έχει ένα νέο μήνυμα.
	Το ψευδώνυμο ή το κανάλι που δίνεται, πρέπει να είναι κατοχυρωμένο.
	    
MEMO_HELP_CANCEL
	Σύνταξη: CANCEL {ψευδώνυμο | κανάλι}
	
	Ακυρώνει το τελευταίο μήνυμα που στείλατε στον παραλείπτη ή το κανάλι,
	με την προυπόθεση οτι δεν έχει διαβαστεί,γιατί η εντολή τότε θα είναι σαν να
	μην έγινε.
	
MEMO_HELP_LIST
	Σύνταξη: LIST [κανάλι] [list | NEW]
	
	Παρουσιάζει την λίστα των μηνυμάτων.  Με την NEW, δείχνει
	μόνο τα νέα μηνύματα (τα μη-διαβασμένα).  Τα μη διαβασμένα μηνύματα
	σημειώνονται με ένα "*"	στα αριστερά που αναγράφεται ο αριθμός του
	μηνύματος.  Μπορείς να δεις μια λίστα με νούμερα,
	σαν το παρακάτω παράδειγμα:
	
	   LIST 2-5,7-9
	     Λίστα μηνυμάτων νούμερο 2 περνά 5 και 7 περνά 9.

MEMO_HELP_READ
	Σύνταξη: READ [κανάλι] {νου | list | LAST | NEW}
	
	Με την εντολή αυτή διαβάζεται τα μηνύματα που σας έρχονται.  Αν δώσετε LAST
	θα διαβάσετε το τελευταίο μήνυμα που δεχθήκατε.  Αν γράψετε
	NEW θα διαβάσετε όλα τα νέα μηνύματα που σας ήρθαν.  Αλλιώς,
	θα δείτε τα νούμερα των μηνυμάτων.  Μπορείτα να δείτε την λίστα με τα
	νούμερα, για παράδειγμα:
	
	   Διάβασε 2-5,7-9
	      Εμφανίζει τα μηνύματα με τα νούμερα 2 έως και 5 και 7 έως και 9.

MEMO_HELP_DEL
	Σύνταξη: DEL [κανάλι] {νου | list | LAST | ALL}
	
	Σβήνει το μήνυμα ή τα μηνύματα.  Σβήνονται μέσω της λίστας
	ανάλογα με τα νούμερα.Για παράδειγμα:
	    
	If LAST is given, the last memo will be deleted.
	Αν επιλέξετε ALL, θα σβήσετε όλα τα μηνύματά σας.
	
	Παράδειγμα:
	
	   DEL 1
	      Διαγράφεται το πρώτο μήνυμα.(νούμερο 1).
	
	   DEL 2-5,7-9
	      Διαγράφονται τα μηνύματα με νούμερα 2 έως 5 και 7 έως 9.

MEMO_HELP_SET
	Σύνταξη: SET επιλογή παράμετροι
	
	Αλλάζει διάφορες επιλογές για τα μηνύματα.
	Η επιλογή μπορεί να είναι μια από:
	    
	    NOTIFY      Αλλάζει το πότε θα ενημερώνεστε για τα
	                    νέα σας μηνύματα (μόνο για ψευδώνυμα)
	    LIMIT       Αλλάζει το μέγιστο αριθμό μηνυμάτων που
	                    μπορείτε να δεχτείτε
	
	Γράψτε %R%S HELP SET επιλογή για περισσότερες πληροφορίες
	σε μια συγκεκριμένη επιλογή.

MEMO_HELP_SET_NOTIFY
	Σύνταξη: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}	
	
	Αλλαγές για το πως θέλετε να ενημερώνεστε για νέα μηνύματα:
	
	    ON      Θα ενημερώνεστε για τα μηνύματα σας όταν θα κάνετε log on,
	               όταν θα επανέρχεστε από /AWAY, και όταν θα σας στέλνουν
	               μηνύματα.
	    LOGON   Θα ενημερώνεστε για τα μηνύματα σας μόνο όταν θα κάνετε log
	               on ή όταν θα επανέρχεστε από /AWAY.
	    NEW     Θα ενημερώνεστε για τα μηνύματα σας μόνο όταν θα σας
	               τα στέλνουν.
	    MAIL    You will be notified of memos by email aswell as
	               any other settings you have.
	    NOMAIL  You will not be notified of memos by email.
	    OFF     Δεν θα ενημερώνεστε ποτέ για νέα μηνύματα ή παλιά.
	
	Η επιλογή ON πιο ουσιώδης,η LOGON και η NEW συνδιαστική.

MEMO_HELP_SET_LIMIT
	Σύνταξη: SET LIMIT [κανάλι] όριο
	
	Ορίζει τον μέγιστο αριθμό μηνυμάτων που μπορείτε να έχετε
	εσείς ή το κανάλι που δίνεται.  Αν οριστεί στο 0, κανείς
	δε θα μπορεί να σας στείλει μηνύματα.  Πάντως δε μπορείτε
	να ορίσετε τον αριθμό πάνω από %d.

MEMO_HELP_INFO
	Σύνταξη: INFO [κανάλι]
	
	Εμφανίζει πληροφορίες στα νούμερα των μηνυμάτων που έχετε, πόσα
	από αυτά δεν έχουν διαβαστεί, και πόσα μηνύματα μπορείτε να λάβετε.
	Με την παράμετρο, εμφανίζονται οι ίδιες πληροφορίες και για ένα κανάλι.
	
MEMO_SERVADMIN_HELP_SET_LIMIT
	Σύνταξη: SET LIMIT [χρήστης | κανάλι] {όριο | NONE} [HARD]
	
	Ρυθμίζεις το μέγιστο νούμερο μηνυμάτων που θες να λάβεις είτε για
	ψευδώνυμο ή για κανάλι.  Αν βάλετε όριο το 0 απαλλάσεστε από
	την υπηρεσία αυτή και δεν λαμβάνεται κανένα μήνυμα.  Αν επιλέξετε NONE
	επιτρέπει στο χρήστη να λάβει όσα θέλει αυτός.  Αν δεν δώσετε
	ψευδώνμο ή κανάλι,  ρυθμίζετε το δικό σας όριο.
	    
	Επιλέγοντας HARD εμποδίζεται το χρήστη να αλλάξει το όριο.  Αν δεν
	επιλέξετε HARD θα έχει το αντίθετο αποτέλεσμα, επιτρέποντας στο
	χρήστη να αλλάξει το όριο 
	(ακόμα και αν η επιλογή η προηγούμενη ήταν HARD).
	    
	Αυτή η χρήση της εντολής SET LIMIT είναι μόνο για 
	τους Services admins.  Οι άλλοι χρήστες μπορούν να αλλάξουν το όριο
	μόνο για τον εαυτό τους ή για τα κανάλια που έχουν κάποια δικαιώματα.
	Δεν μπορούν να ξεπεράσουν το όριο των %d μηνυμάτων, και δεν μπορούν να
	επιλέξουν hard limit.
	
MEMO_SERVADMIN_HELP_INFO
	Σύνταξη: INFO [ψευδώνυμο | κανάλι]
	
	Χωρίς την παράμετρο, εμφανίζει πληροφορίες για τα νουμερα των
	μηνυμάτων που έχετε, πόσα από αυτά δεν έχουν διαβαστεί, και πόσα
	τελικά μηνύματα μπορείτε να λάβετε.
	
	Με την παράμετρο κανάλι,εμφανίζει τις ίδιες πληροφορίες για το
	ανάλογο κανάλι.
	
	Με την παράμετρο ψευδώνυμο, εμφανίζει τις ίδιες πληροφορίες για το
	ανάλογο ψευδώνυμο.  Αυτή η εντολή είναι μόνο για τους Services
	admins.

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
	    GLOBAL      Στέλνει μήνυμα σε όλους τους χρήστες
OPER_HELP_CMD_STATS
	    STATS       Δείχνει στατιστικά σχετικά με τα Services και το δίκτυο
OPER_HELP_CMD_OPER
	    OPER        Αλλάζει την λίστα των Services operator
OPER_HELP_CMD_ADMIN
	    ADMIN       Αλλάζει την λίστα των Services admin
OPER_HELP_CMD_STAFF
	    STAFF       Display Services staff and online status
OPER_HELP_CMD_MODE
	    MODE        Αλλάζει modes σε κανάλι και άλλα
OPER_HELP_CMD_KICK
	    KICK        Απομακρύνει το χρήστη από το κανάλι
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  Καθαρίζει τα modes από το κανάλι
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  Σκοτώνει όλους τους χρήστες που έχουν το ίδιο host
OPER_HELP_CMD_AKILL
	    AKILL       Προσθέτει ip κλπ στην λίστα AKILL
OPER_HELP_CMD_SGLINE
	    SGLINE      Προσθέτει ip στην λίστα SGLINE
OPER_HELP_CMD_SQLINE
	    SQLINE      Προσθέτει ip στην λίστα SQLINE
OPER_HELP_CMD_SZLINE
	    SZLINE      Προσθέτει ip στην λίστα SZLINE
OPER_HELP_CMD_CHANLIST
	    CHANLIST    Εμφανίζει όλα τα κανάλια
OPER_HELP_CMD_USERLIST
	    USERLIST    Εμφανίζει όλους τους χρήστες
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   Αλλάζεις τα μηνύματα που θα εμφανίζονται όταν αυτοί θα μπένουν
	        στο δίκτυο
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  Αλλάζεις τα τυχαία επιλεγόμενα μηνύματα που θα εμφανίζονται 
	                    όταν αυτοί θα μπένουν στο δίκτυο
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    Αλλάζεις τα μηνύματα που θα εμφανίζονται στους χρήστες που
	        γίνονται oper
OPER_HELP_CMD_SESSION
	    SESSION     Βλέπεις την λίστα με τα host sessions
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   Αλλάζεις την λίστα του session-limit exception
OPER_HELP_CMD_NOOP
	    NOOP        Αφαιρεί προσωρινά όλες τις O:γραμμές από τον server 
	                    αυτόματα
OPER_HELP_CMD_JUPE
	    JUPE        "Jupiter" σε server
OPER_HELP_CMD_IGNORE
	    IGNORE      Modify the Services ignore list
OPER_HELP_CMD_SET
	    SET         Ρυθμίζεις διάφορες επιλογές των Services
OPER_HELP_CMD_RELOAD
	    RELOAD      Ξαναφορτώνει το services' configuration αρχείο
OPER_HELP_CMD_UPDATE
	    UPDATE      Ενεργούν τα Services (βάσεις δεδομένων) για
	                    να γίνουν update στο δίσκο αμέσως
OPER_HELP_CMD_RESTART
	    RESTART     Σώζει τις βάσεις δεδομένων και επανεκκινεί τα Services
OPER_HELP_CMD_QUIT
	    QUIT        Τερματίζει το πρόγραμμα των Services χωρίς να σώσει τις αλλαγές
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    Τερματίζει το πρόγραμμα των Services με ταυτόχρονο γράψιμο των
	        αλλαγών
OPER_HELP_CMD_DEFCON
	    DEFCON      Manipulate the DefCon system
OPER_HELP_CMD_CHANKILL
	    CHANKILL    AKILL all users on a specific channel
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
	Οι εντολές του %S είναι::

OPER_HELP_LOGGED
	Σημείωση: Όλες οι εντολές στέλνονται στον %S και καταγράφονται!

OPER_HELP_GLOBAL
	Σύνταξη: GLOBAL μήνυμα
	
	Επιτρέπει στους IRCops να στέλνουν μυνήματα σε όλους τους χρήστες του 
	δικτύου.  Το μήνυμα θα στέλνεται από το ψευδώνυμο υπηρεσίας %s.
	
OPER_HELP_STATS
	Σύνταξη: STATS [AKILL | ALL | RESET]
	
	Χωρίς την επιλογή, εμφανίζει το νούμερο των χρηστών και των
	IRCops που είναι στο δίκτυο (μαζί και των Services), το μεγαλύτερο νούμερο
	των χρηστών που είναι στο δίκτυο όταν τα Services ξεκινάνε, και την
	διάρκεια που τα Services είναι στο δίκτυο.
	
	
	Με την επιλογή AKILL, εμφανίζει την συγκεκριμένη λίστα.
	
	AKILL και τον default χρόνο λήξης.
	
	The RESET option currently resets the maximum user count
	to the number of users currently present on the network.
	
	UPTIME may be used as a synonym for STATS.

OPER_HELP_OPER
	Σύνταξη: OPER ADD ψευδώνυμο
	        OPER DEL {ψευδώνυμο | θέση-νούμερο | λίστα}
	        OPER LIST [μάσκα | λίστα]
	        OPER CLEAR
	
	Η εντολή αυτή επιτρέπει στους Services Root να προσθέσουν ή να αφαιρέσουν
	ψευδώνυμα από την λίστα Services operator.  Ο χρήστης ο οποίος είναι στην λίστα
	Services operator και ο οποίος έχει κάνει εντολή αναγνώρισης με τον  %s
	έχει πρόσβαση στις εντολές των Services operator.
	
	Η εντολή OPER ADD προσθέτει το ανάλογο ψευδώνυμο στη
	λίστα Services operator.
	
	Η εντολή OPER DEL αφαιρεί το ανάλογο ψευδώνυμο από την λίστα
	των Services operator.  Αν η λίστα με τις θέσεις-νούμερα δίνετε, 
	αυτές οι θέσεις διαγράφηκαν.  (Δες το παράδειγμα με την εντολή LIST.)
	
	Η εντολή OPER LIST εμφανίζει την λίστα των Services operator.  
	Αν δίνετε κάτι συγκεκριμένο, θα εμφανιστούν μόνο οι θέσεις που ταιριάζουν 
	με το ανάλογο θα δείτε μόνο τις θέσεις αυτές; για παράδειγμα:
	

	   OPER LIST 2-5,7-9
	    Εμφανίζει την λίστα των Services operator με θέσεις	       
	        που έχουν αντιστοιχούν στα νούμερα 2 έως 5 και 7 έως 9.
	      
	Η εντολή OPER CLEAR καθαρίζει όλες τις θέσεις της λίστας
	Services operator.
	
	Κάθε IRC operator μπορεί να χρησιμοποιήσει την εντολή OPER LIST.

OPER_HELP_ADMIN
	Σύνταξη: ADMIN ADD ψευδώνυμο
	        ADMIN DEL {ψευδώνυμο | θέσεις-νούμερα | λίστα}
	        ADMIN LIST [μάσκα | λίστα]
	        ADMIN CLEAR
	
	Επιτρέπει στον Services root να προσθέσει ή να διαγράψει ψευδώνυμα
	από την λίστα Services admin.  Ο χρήστης που το ψευδώνυμο του είναι
	στην λίστα των Services admin και ο οποίος έχει κάνει εντολή αναγνώρισης με τον
	%s μπορεί να χρησιμοποιήσει τις εντολές των Services admin.
	
	Η εντολή ADMIN ADD προσθέτει ένα ψευδλωνυμο στην λίστα των
	Services admin.
	
	Η εντολή ADMIN DEL διαγράφει ένα ψευδώνυμο από την λίστα των
	Services admin.  Αν η λίστα με τις θέσεις-νούμερα δίνετε,
	αυτές οι θέσεις διαγράφηκαν.  (Δες το παράδειγμα με την εντολή LIST.)
	
	Η εντολή ADMIN LIST εμφανίζει την λίστα των Services operator.  
	Αν δίνετε κάτι συγκεκριμένο, θα εμφανιστούν μόνο οι θέσεις που ταιριάζουν με το
	ανάλογο ζητούμενο.  Αν δίνετε η λίστα θέσεις-νούμερα,θα δείτε μόνο τις θέσεις 
	αυτές; για παράδειγμα:
	
	   ADMIN LIST 2-5,7-9
	      Εμφανίζει την λίστα των Services operator με θέσεις  που έχουν 
	      αντιστοιχούν στα νούμερα 2 έως 5 και 7 έως 9..
	      
	Η εντολή ADMIN CELAR καθαρίζει όλες τις θέσεις της λίστας
	Services admin.
	
	Kάθε IRC operator μπορεί να χρησιμοποιήσει την εντολή ADMIN LIST.
	Όλες τις άλλες επιλογές μόνο ο Services root.

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
	Σύνταξη: MODE κανάλι modes
	
	Επιτρέπει στους Services operators να αλλάζουν τα modes κάθε καναλιού.
	Οι παράμετροι είναι ίδιοι σαν της εντολής /MODE.
	    
	Μόνο για τους Services operators.

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
	Σύνταξη: CLEARMODES κανάλι [ALL]
	
	Καθαρίζει όλα τα binary modes (i,k,l,m,n,p,s,t) και κάνει bans από το
	κανάλι.  Αν η παράμετρος ALL δίνεται, καθαρίζει όλους όσοι είναι ops και
	voices (+o και +v modes) στο κανάλι.
	
	Μόνο για τους Services operators.

OPER_HELP_KICK
	Σύνταξη: KICK κανάλι χρήστη λόγο
	
	Επιτρέπει στους IRCops να κάνουν kick ε΄να χρήστη από το κανάλι.
	Οι παράμετροι είναι ίδιοι με την εντολή /KICK.
	Το μήνυμα του kick θα έχει το ψευδώνυμο του
	IRCop σαν μια προειδοποίηση; Για παράδειγμα:
	
	*** SpamMan has been kicked off channel #my_channel by %S (Alcan (Flood))
	
	Μόνο για τους Services operators.

OPER_HELP_SVSNICK
	Syntax: SVSNICK nick newnick
	
	Forcefully changes a user's nickname from nick to newnick.
	Limited to Super Admins.

OPER_HELP_AKILL
	Σύνταξη: AKILL ADD [+expiry] μάσκα λόγος
	        AKILL DEL {μάσκα | θέσεις-νούμερα | λίστα}
	        AKILL LIST [μάσκα | λίστα]
	        AKILL VIEW [μάσκα | λίστα]
	        AKILL CLEAR
	
	Μπορούν οι Services operators να αλλάξουν την λίστα AKILL.  Αν
	η μάσκα του χρήστη ταιριάξει με μια από αυτές που περιέχονται στην λίστα AKILL,
	τα Services δεν θα επιτρέψουν στο χρήστη να συνδεθεί με το δίκτυο (server) και 
	θα έχει KILL, σε όλους τους servers που είναι μαζί.Δηλ γίνεται προσθήκη
	ban (K-line) για την μάσκα που ταιριάζει στο χρήστη.
	    
	Η εντολή AKILL ADD προσθέτει το ανάλογο user@host μάσκα στην λίστα AKILL
	με έναν λόγο που πρέπει να δώσετε ωστε ο χρήστης να καταλάβει γιατί είναι AKILL.
	Το expiry είναι συγκεκριμένο και επιλέγεται από τα επόμενα, d 
	(μέρες), h (ώρες), ή m (λεπτά).  Συνδιασμοί (όπως 
	1h30m) δεν επιτρέπονται .  Αν δεν δώσετε το χρόνο 
	από default θα είναι σε μέρες (θα είναι +30 που σημαίνει 30 
	μέρες).  Για να προσθέσετε ένα AKILL που να μην λήγει, χρησιμοποιείστε +0.
	Αν η usermask ξεκινάει με +, ο χρόνος λήξης θα πρέπει να δίνετε
	ακόμα και αν είναι ίδιος με τον default.  Ο
	συγκεκριμένος χρόνος λήξης της εντολής AKILL μπορεί να βρεθεί με την εντολή
	STATS AKILL.
	
	Η εντολή AKILL DEL διαγράφει τη συγκεκριμένη μάσκα από την λίστα.  
	AKILL αν υπάρχει.  Αν σας δωθεί λίστα με τις θέσεις-νούμερα
	αυτές οι θέσεις θα διαγραφούν.  (Κοίτα το παράδειγμα με την εντολή LIST.)
	
	Η εντολή AKILL LIST εμφανίζει την λίστα με τα AKILL.
	Αν δίνεται μια συγκεκριμένη μάσκα, θα εμφανιστούν οι θέσεις που αντιστοιχούν σε
	αυτή τη μάσκα.  Αν δίνετε η λίστα με τις θέσεις-νούμερα μπορείτε να τις δείτε και 
	αλλιώς,
	παράδειγμα:

	   AKILL LIST 2-5,7-9
	      Εμφανίζει τις θέσεις με τα νούμερα 2 εως 5 και 7 
	      εως 9 της λίστας AKILL.
	      
	Η εντολή AKILL VIEW είναι πιο καλύτερη από την AKILL LIST, και
	εμφανίζει ποιος συγκεκριμένα έβαλε κάποιον AKILL, την ημερομηνία που έγινε, και
	πότε θα λήξει, όπως και το user@host και τον λόγο.

	Η εντολή _AKILL CLEAR_ καθαρίζει όλες τις θέσεις της λίστας AKILL.

	Μόνο για τους Services operators.	
	    
OPER_HELP_SGLINE
	Σύνταξη: SGLINE ADD [+expiry] μάσκα:λόγος
	    SGLINE DEL {μάσκα | θέσεις-νούμερα | λίστα}
	        SGLINE LIST [μάσκα | λίστα]
	        SGLINE VIEW [μάσκα | λίστα]
	        SGLINE CLEAR
	
	Οι Services operators μπορούν να αλλάξουν την λίστα SGLINE.  Αν
	ο χρήστης με το πραγματικό όνομα ταιριάζει με την SGLINE μάσκα,τα 
	Services δεν θα τον αφήσουν να συνδεθεί.
	    
	Η εντολή SGLINE ADD προσθέτει μια πραγματική μάσκα στην λίστα SGLINE
	με κάποιον λόγο τον οποίο πρέπει να τον δώσετε.
	Το expiry είναι συγκεκριμένο και επιλέγεται από τα επόμενα, d 
	(μέρες), h (ώρες), ή m (λεπτά).  Συνδιασμοί (όπως 
	1h30m) δεν επιτρέπονται .  Αν δεν δώσετε το χρόνο 
	από default θα είναι σε μέρες (θα είναι +30 που σημαίνει 30 
	μέρες).  Για να προσθέσετε ένα SGLINE που να μην λήγει, χρησιμοποιείστε +0.
	usermask ξεκινάει με +, ο χρόνος λήξης θα πρέπει να δίνετε
	ακόμα και αν είναι ίδιος με τον default.  Ο
	συγκεκριμένος χρόνος λήξης της εντολής SGLINE μπορεί να βρεθεί με την εντολή
	STATS AKILL.
	Σημείωση: επειδή το πραγματικό όνομα-μάσκα μπορεί να περιέχει κενά,
	διαχωριστικά μεταξύ τους να είστε προσεκτικοί στο λόγο.
	
	Η εντολή SGLINE DEL αφαιρεί μία μάσκα από την λίστα
	SGLINE list αν υπάρχει.  Αν η λίστα με τις θέσεις-νούμερα δίνετε, 
	αυτές διαγράφονται.  (Δές το παράδειγμα LIST.) 
	    
	Η εντολή SGLINE LIST εμφανίζει την λίστα με τα SGLINE.  
	Αν δίνεται μια συγκεκριμένη μάσκα, θα εμφανιστούν οι θέσεις που αντιστοιχούν σε
	αυτή τη μάσκα
	Αν δίνετε η λίστα με τις θέσεις-νούμερα μπορείτε να τις δείτε και αλλιώς,
	παράδειγμα:

	   SGLINE LIST 2-5,7-9
	      Εμφανίζει τις θέσεις με τα νούμερα 2 εως 5 και 7 
	      εως 9 της λίστας SGLINE.
	      
	Η εντολή SGLINE VIEW είναι πιο βελτιωμένη από την SGLINE LIST, και 
	εμφανίζει ποιος έβαλε το κάθε SGLINE, την ημερομηνία που μπήκε, και πότε 
	λήγει, και φυσικά τη realname μάσκα και τον λόγο.
	
	Η εντολή SGLINE CLEAR καθαρίζει όλες τις θέσεις από την λίστα SGLINE.
	
	Μόνο για τους Services operators.

OPER_HELP_SQLINE
	Σύνταξη: SQLINE ADD [+expiry] μάσκα λόγος
	    SQLINE DEL {μάσκα | θέση-νουμερο | λίστα}
	        SQLINE LIST [μάσκα | λίστα]
	        SQLINE VIEW [μάσκα | λίστα]
	        SQLINE CLEAR
	
	Οι Services operators μπορούν να αλλάξουν την λίστα SQLINE.  Αν
	ο χρήστης μπει με ένα ψευδώνυμο που είναι στην λίστα SQLINE 
	τα Services δεν θα τον αφήσουν να συνδεθεί ή θα του αλλάξουν το
	ψευδώνυμο του(σε μερικούς servers).
	
	Αν ο πρώτος χαρακτήρας της μάσκας είναι #, τα services θα 
	εμποδίσουν την χρήση γιατί αντισοιχούν σε κανάλια αυτοί οι χαρακτήρες.(σε IRCds 
	οι οποίοι το υποστηρίζουν).
	
	Η εντολή SQLINE ADD προσθέτει ένα ψευδώνυμο στην λίστα SQLINE	
	με τον ανάλογο λόγο (ο οποίος πρέπει να δίνετε).
	Το expiry είναι συγκεκριμένο και επιλέγεται από τα επόμενα, d 
	(μέρες), h (ώρες), ή m (λεπτά).  Συνδιασμοί (όπως 
	1h30m) δεν επιτρέπονται .  Αν δεν δώσετε το χρόνο 
	από default θα είναι σε μέρες (θα είναι +30 που σημαίνει 30 
	μέρες).  Για να προσθέσετε ένα SQLINE που να μην λήγει, χρησιμοποιείστε +0.  
	usermask ξεκινάει με +, ο χρόνος λήξης θα πρέπει να δίνετε 
	ακόμα και αν είναι ίδιος με τον default.  Ο
	συγκεκριμένος χρόνος λήξης της εντολής SQLINE μπορεί να βρεθεί με την εντολή
	STATS AKILL.
	
	Η εντολή SQLINE DEL αφαιρεί μία μάσκα από την λίστα
	SQLINE list αν υπάρχει.  Αν η λίστα με τις θέσεις-νούμερα δίνετε, 
	αυτές διαγράφονται.  (Δές το παράδειγμα LIST.) 
	    
	Η εντολή SQLINE LIST εμφανίζει την λίστα με τα SQLINE.  
	Αν δίνεται μια συγκεκριμένη μάσκα, θα εμφανιστούν οι θέσεις που αντιστοιχούν σε
	αυτή τη μάσκα Αν δίνετε η λίστα με τις θέσεις-νούμερα μπορείτε να τις δείτε και
	αλλιώς.
	παράδειγμα:

	   SQLINE LIST 2-5,7-9
	      Εμφανίζει τις θέσεις με τα νούμερα 2 εως 5 και 7 
	      εως 9 της λίστας SQLINE.
	      
	Η εντολή SQLINE VIEW είναι πιο βελτιωμένη από την SQLINE LIST, και 
	εμφανίζει ποιος έβαλε το κάθε SQLINE, την ημερομηνία που μπήκε, και πότε 
	λήγει, και φυσικά τη realname μάσκα και τον λόγο.
	
	Η εντολή SQLINE CLEAR καθαρίζει τις θέσεις της λίστας SQLINE.
	
	Μόνο για τους Services operators.

OPER_HELP_SZLINE
	Σύνταξη: SZLINE ADD [+expiry] μάσκα λόγος
	        SZLINE DEL {μάσκα | θέση-νουμερο | λίστα}
	        SZLINE LIST [μάσκα | λίστα]
	        SZLINE VIEW [μάσκα | λίστα]
	        SZLINE CLEAR
	
	Οι Services operators μπορούν να αλλάξουν την λίστα SZLINE.  Αν
	ο χρήστης με κάποιο IP αντιστοιχεί με ένα από αυτά που έχει η λίστα SZLINE 
	τα Services δεν τον αφήνουν να συνδεθούν στο δίκτυο.
	(και αυτό, είτε το IP έχει PTR RR ή όχι).
	
	Η εντολή SZLINE ADD προσθέτει ένα IP  στην λίστα SZLINE
	με έναν λόγο (ο οποίος πρέπει να δίνετε).
	Το expiry είναι συγκεκριμένο και επιλέγεται από τα επόμενα, d 
	(μέρες), h (ώρες), ή m (λεπτά).  Συνδιασμοί (όπως 
	1h30m) δεν επιτρέπονται .  Αν δεν δώσετε το χρόνο 
	από default θα είναι σε μέρες (θα είναι +30 που σημαίνει 30 
	μέρες).  Για να προσθέσετε ένα SZLINE που να μην λήγει, χρησιμοποιείστε +0.
	usermask ξεκινάει με +, ο χρόνος λήξης θα πρέπει να δίνετε
	ακόμα και αν είναι ίδιος με τον default.  Ο
	συγκεκριμένος χρόνος λήξης της εντολής SZLINE μπορεί να βρεθεί με την εντολήe
	STATS AKILL.
	
	Η εντολή SZLINE DEL αφαιρεί μία μάσκα από την λίστα
	SZLINE list αν υπάρχει.  Αν η λίστα με τις θέσεις-νούμερα δίνετε, 
	αυτές διαγράφονται.  (Δές το παράδειγμα LIST.) 
	    
	Η εντολή SZLINE LIST εμφανίζει την λίστα με τα SZLINE.  
	Αν δίνεται μια συγκεκριμένη μάσκα, θα εμφανιστούν οι θέσεις που αντιστοιχούν σε
	αυτή τη μάσκα.  Αν δίνετε η λίστα με τις θέσεις-νούμερα μπορείτε
	να τις δείτε και αλλιώς,
	παράδειγμα::

	   SZLINE LIST 2-5,7-9
	      Εμφανίζει τις θέσεις με τα νούμερα 2 εως 5 και 7 
	      εως 9 της λίστας SZLINE.
	      
	Η εντολή SZLINE VIEW είναι πιο βελτιωμένη από την SZLINE LIST, και 
	εμφανίζει ποιος έβαλε το κάθε SZLINE, την ημερομηνία που μπήκε, και πότε 
	λήγει, και φυσικά τη realname μάσκα και τον λόγο.
	
	Η εντολή SZLINE CLEAR καθαρίζει όλες τις θέσεις της λίστας SZLINE.
	
	Μόνο για τους Services operators.

OPER_HELP_SET
	Σύνταξη: SET option setting
	
	Ρυθμίσεις σχετικά με τις επιλογές των Services.  Ονομαστικά
	οι επιλογές είναι:
	    READONLY   Αλλάζετε σε διαδικασία read-only ή read-write mode
	    LOGCHAN    Report log messages to a channel
	    DEBUG      Ενεργοποιείτε ή απενεργοποιείτε σε debug mode
	    NOEXPIRE   Ενεργοποιείτε ή απενεργοποιείτε σε no expire mode
	    SUPERADMIN Activate or deactivate super-admin mode
	    SQL   	 Activate or deactivate sql mode
	    IGNORE     Activate or deactivate ignore mode
	    LIST       List the options
	
	Μόνο για τους Services admins.

OPER_HELP_SET_READONLY
	Σύνταξη: SET READONLY {ON | OFF}
	
	Αλλάζετε σε read-only mode on ή off.  Η read-only mode, normal
	users δεν θα επιτρέψει να αλλάξει η βάση δεδομένων των Services,
	συμπεριλαμβανομένου καναλιού και ψευδώνυμου access lists, klp.  IRCops
	me επαρκή δικαιώματα στα Services θα μπορούν να αλλάζουν τα
	Services' την λίστα AKILL να κάνουν drop (διαγραφεί ψευδωνύμων) ή καταστολή
	ψευδωνύμων και καναλιών, αλλά όποιες αλλαγές και να γίνονται
	δεν θα γραφούν εκτός και αν η read-only mode είναι απενεργοποημένη πριν τα 
	Services τερματιστούν ή επανεκκινηθούν.
	    
	Αυτή η επιλογή είναι αντίστοιχη με την εντολή-line επιλογή
	-readonly.

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
	Σύνταξη: SET DEBUG {ON | OFF | num}
	
	Αλλάζει σε debug mode on ή off.  Σε debug mode, όλη η data στέλνετε
	και από τα Services μέσω άλλων debugging
	μηνυμάτων γράφονται σε log αρχείο.  Αν το _num_ δίνεται
	το debug mode ενεργοποιείται, με debugging level ρυθμισμένο
	στο num.
	
	Αυτή η επιλογή είναι αντίστοιχη με την εντολή-line επιλογή	
	-debug.
	
OPER_HELP_SET_NOEXPIRE
	Σύνταξη: SET NOEXPIRE {ON | OFF}
	
	Αλλάζει σε no expire mode on ή off.  Σε no expire mode, ψευδώνυμα,
	κανάλια, akills και exceptions δεν θα λήγουν εκτός και αν
	αυτή η επιλογή δεν ρυθμιστεί.
	
	Αυτή η επιλογή είναι αντίστοιχη με την εντολή-line επιλογή
	-noexpire.

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
	Σύνταξη: NOOP SET server
	        NOOP REVOKE server
	
	Η εντολή NOOP SET αφαιρεί όλα τα O:γραμμές του ανάλογου 
	server και κάνει kill όλους τους IRCops που είναι εκείνη την ώρα 
	για λόγους rehashing του server. 
	    
	Η εντολή NOOP REVOKE κάνει όλα τα  O:γραμμές που είχαν αφαιρεθεί να
	είναι διαθέσιμα και πάλι στο συγκεκριμένο server.
	
	Σημείωση: Ο server δεν ελέγχεται από τα
	Services.
	
	Μόνο για τους Services admins.

OPER_HELP_JUPE
	Σύνταξη: JUPE server [λόγος]
	
	Κάνεις τα Services  jupiter σε έναν server -- που είναι, να δημιουργήσει
	ένα ψεύτικο "server" συνδεδεμένο με τα Services ο οποίος περιέχει
	τον πραγματικό server που είναι συνδεδεμένα.  Το jupe
	μπορεί να αφαιρεθεί χρησιμοποιώντας την εντολή SQUIT.  Αν ο λόγος δίνεται
	θα γραφτεί στου server τις πληροφορίες;
	αλλιώς, οι πληροφορίες θα περιέχουν ένα
	text "Juped by <ψευδώνυμο>", δείχνοντας το ψευδώνυμο του
	χρήστη που έκανε jupitered τον server.
	
	Μόνο για τους Services admins.

OPER_HELP_RAW
	Σύνταξη: RAW μήνυμα
	
	Στέλνεις μηνύματα μέσω του server με τον οποίο τα
	Services είναι συνδεδεμένα.  Αυτή η εντολή έχει περιορισμένη χρήση
	και μπορεί να καταστρέψει το δίκτυο αν χρησιμοποιείται
	ακατάλληλα.  ΜΗΝ ΧΡΗΣΙΜΟΠΟΙΕΙΤΑΙ ΑΥΤΗ ΤΗΝ ΕΝΤΟΛΗ εκτός και αν είστε
	σίγουροι για το τι κάνετε!
	    
	Μόνο για τους Services admins.

OPER_HELP_UPDATE
	Σύνταξη: UPDATE
	
	Έxei ως αποτέλεσμα τα Services να κάνουν update σε όλες τις βάσεις δεδομένων
	το συντομότερο που κάνετε αυτή την εντολή.
	    
	Μόνο για τους Services admins.
	
OPER_HELP_RELOAD
	Σύνταξη: RELOAD
	
	Έχει ως αποτέλεσμα τα Services να επαναφορτώνουν το configuration αρχείο.
	Σημείωση οτι μερικές επιλογές για να έχουν αποτέλεσμα χρειάζονται επανεκκίνηση
	των Services (όπως Services' ψευδώνυμα, ενεργοποίηση της session 
	limitation, κλπ.)
	    
	Μόνο για τους Services admins.

OPER_HELP_QUIT
	Σύνταξη: QUIT
	
	Έχει ως αποτέλεσμα τα Services να κλείσουν αμέσως(να τερματιστούν); οι βάσεις
	δεδομένων δεν γράφονται(δεν σώζονται).  Αυτή η εντολή δεν πρέπει να
	χρησιμοποιείται εκτός και αν υπάρχει βλάβη στην μνήμη των databases
	και δεν γίνεται να γραφτούν(δεν γίνεται να γίνουν save).
	Για κανονικό τερματισμό, κάντε την εντολή SHUTDOWN.
	    
	Μόνο για τους Services admins.

OPER_HELP_SHUTDOWN
	Σύνταξη: SHUTDOWN
	
	Έχει ως αποτέλεσμα τα Services να γράψουν όλη την βάση δεδομένων και να 
	τερματιστούν.
	
	Μόνο για τους Services admins.

OPER_HELP_RESTART
	Σύνταξη: RESTART
	
	Έχει ως αποτέλεσμα τα Services να γράψουν όλη την βάση δεδομένων και να
	επανεκκινηθούν (δηλ να κλείσουν και αμέσως να 
	ξανατρεχτεί το εκτελέσιμο αρχείο).
	
	Μόνο για τους Services admins.	

OPER_HELP_CHANLIST
	Σύνταξη: CHANLIST [{επιλογή | ψευδώνυμο} [SECRET]]	
	
	Εμφανίζει τις λίστες όλων των καναλιών που υπάρχουν συγκεκριμένα εκείνη τη
	στιγμή στο IRC δίκτυο, είτε είναι κατοχυρωμένα είτε όχι.
	    
	Αν δίνετε επιλογή, εμφανίζει την λίστα μόνο με τα κανάλια που ταιριάζουν.
	Αν δίνετε ψευδώνυμο εμφανίζει μόνο τα κανάλια που τα χρησιμοποιεί ο ζητούμενος 
	χρήστης.  Αν δίνετε SECRET εμφανίζει μόνο τα κανάλια που ταιριάζουν με την
	επιλογή που έχουν +s ή +p mode.
	    
	Μόνο για τους Services admins.

OPER_HELP_USERLIST
	Σύνταξη: USERLIST [{επιλογή | κανάλι} [INVISIBLE]]
	
	Εμφανίζει τις λίστες όλων των χρηστών που είναι την συγκεκριμένη στιγμή
	online στο IRC δίκτυο, είτε το ψευδώνυμο είναι κατοχυρωμένο είτε όχι.
	    
	Αν η επιλογή δίνετε, εμφανίζονται οι λίστες με τους χρήστες που
	ταιριάζουν με την επιλογή σας (πρέπει να είναι της μορφής ψευδώνυμο!user@host).
	Αν δίνετε το κανάλι, εμφανίζονται λίστες των χρηστών που είναι στο κανάλι 
	αυτό. Αν δίνετε INVISIBLE, μόνο χρήστες με +i flag θα εμφανίζονται.
	    
	Μόνο για τους Services admins.

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
	Syntax: MODLIST [Core|3rd|protocol|encryption|supported|qatested]

	Lists all currently loaded modules.

	Limited to Services Roots.


###########################################################################
#
# BotServ help messages
#
###########################################################################

BOT_HELP_CMD_BOTLIST
	BOTLIST            Λίστα με διαθέσιμα bots
BOT_HELP_CMD_ASSIGN
	ASSIGN             Ζητάτε από το bot να έρθει στο κανάλι σας
BOT_HELP_CMD_SET
	SET                Αλλάζετε τις επιλογές του bot
BOT_HELP_CMD_KICK
	KICK               Ρυθμίζεται  τι να κάνει στους "άτακτους χρήστες"
BOT_HELP_CMD_BADWORDS
	BADWORDS           Αλλάζετε την λίστα κακών λέξεων
BOT_HELP_CMD_ACT
	ACT                Makes the bot do the equivalent of a "/me" command
BOT_HELP_CMD_INFO
	INFO               Allows you to see BotServ information about a channel or a bot
BOT_HELP_CMD_SAY
	SAY                Makes the bot say the given text on the given channel
BOT_HELP_CMD_UNASSIGN
	UNASSIGN           Unassigns a bot from a channel
BOT_HELP_CMD_BOT
	BOT                Αλλάζει την λίστα των bot του δικτύου

BOT_HELP
	Ο %S σου επιτρέπει να έχεις bot στο δικό σου κανάλι.
	Αυτή η υπηρεσια έχει δημιουργηθεί για τους χρήστες που δεν μπορούν
	να ρυθμίσουν bots ή για δίκτυα που δεν επιτρέπουν οι χρήστες να έχουν
	ικά τους bots για ασφάλεια. 
	Διαθέσημες εντολές είναι οι επόμενες, για να τις δείτε γράψτε
	%R%S command.Για περισσότερες πληροφορίες σε συγκεκριμένες
	εντολές γράψτε %R%S HELP command. 
	    
BOT_HELP_FOOTER
	Το Bot θα μπεί στο κανάλι όταν είναι τουλάχιστον μέσα
	%d χρήστη(ες).
	
BOT_HELP_BOTLIST
	Σύνταξη: BOTLIST
	
	Εμφανίζει την λίστα με τα διαθέσιμα bots αυτού του δικτύου.
	
BOT_HELP_ASSIGN
	Σύνταξη: ASSIGN κανάλι ψευδώνυμο
	
	Με την εντολή αυτή ζητάς από το bot να μπεί στο κανάλι σου. Μπορείς
	μετά να το ρυθμίσεις για τις ανάγκες του καναλιού σου.
	    
BOT_HELP_UNASSIGN
	Σύνταξη: UNASSIGN κανάλι
	
	Με την εντολή αυτή ζητάς από το bot να βγεί από το κανάλι σου.
	Πάντως,οι ρυθμίσεις του bot για το κανάλι που ήταν είναι αποθηκευμένες,
	για την περίπτωση που θες να το βάλεις αργότερα.
	    
BOT_HELP_INFO
	Σύνταξη: INFO {κανάλι | ψευδώνυμο}
	
	Σου επιτρέπει να δεις από το %S πληροφορίες σχετικά με το κανάλι ή το bot.
	Αν η παράμετρος είναι το κανάλι, θα δείς πληροφορίες μόνο αν έχεις
	ενεργοποιη΄σει την επιλογή kickers. Αν η παράμετρος είναι το ψευδώνυμο,
	θα δεις πληροφορίες σχετικά με το bot.
	    
BOT_HELP_SET
	Σύνταξη: SET κανάλι επιλογή παράμετροι
	
	Ρυθμίζει τις επιλογές του bot.  Οι επιλογές μπορεί να είναι:
	
	    DONTKICKOPS      Προστασία στους ops ωστε να μην γίνουν kick από το bot
	    DONTKICKVOICES   Προστασία στους voices ωστε να μην γίνουν kick από το bot
	    GREET            Ενεργοποίηση μηνυμάτων χαιρετισμού
	    FANTASY          Ενεργοποίηση εντολών fantaisist
	    SYMBIOSIS        Επιτρέπεται στο bot να συμπεριφέρεται σαν πραγματικό bot
	
	Γράψε %R%S HELP SET επιλογή για περισσότερες πληροφορίες
	για μια συγκεκριμένη επιλογή.
	
	Σημείωση: η access σε αυτή την εντολή διευθύνεται από την
	level SET.
	
BOT_HELP_SET_DONTKICKOPS
	Σύνταξη: SET κανάλι DONTKICKOPS {ON|OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή op προστασία στο κανάλι.
	Όταν είναι ενεργοποιημένη, οι ops δεν θα τρώνε kick από το bot
	ακόμα και αν δεν ταιριάζοτν με την NOKICK level.
	
BOT_HELP_SET_DONTKICKVOICES
	Σύνταξη: SET κανάλι DONTKICKVOICES {ON|OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή voices προστασία στο κανάλι.
	Όταν είναι ενεργοποιημένη, οι voices δεν θα τρώνε kick από το bot
	ακόμα και αν δεν ταιριάζοτν με την NOKICK level.
	
BOT_HELP_SET_FANTASY
	Σύνταξη: SET κανάλι FANTASY {ON|OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή fantasy στο κανάλι.
	Όταν είναι ενεργοποιημένη, οι χρήστες μπορούν να χρησιμοποιήσουν εντολές
	!op, !deop, !voice, !devoice,!kick, !kb, !unban, !seen
	στο κανάλι (ψάχνετε να δείτε πως δουλεύουν 
	προσπαθήστε με ή χωρίς ψευδώνυμο για κάθε μια,και με λόγο για κάποιες) 
	    
	Σημειώνεται οτι όσοι χρήστες θέλουν να χρησιμοποιήσουν τις fantaisist
	εντολές θα ΠΡΕΠΕΙ να έχουν αρκετό level και για την
	FANTASIA και άλλο level για τις εντολές
	(για παράδειγμα, για να χρησιμοποιήσεις !op, θα πρέπει να έχεις αρκετή 
	access για την OPDEOP level).
	
BOT_HELP_SET_GREET
	Σύνταξη: SET κανάλι GREET {ON|OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή greet στο κανάλι.
	Όταν είναι ενεργοποιημένη, το bot θα εμφανίζει το μήνυμα χαιρετισμού
	στους χρήστες που θα μπένουν στο κανάλι, με την προυπόθεση οτι
	έχουν αρκετή access στο κανάλι.

BOT_HELP_SET_SYMBIOSIS
	Σύνταξη: SET κανάλι SYMBIOSIS {ON|OFF}
	
	Ενεργοποιεί ή απενεργοποιεί την επιλογή symbiosis στο κανάλι.
	Όταν είναι ενεργοποιημένη, το bot θα κάνει τα πάντα
	normal από το %s στα κανάλια, όλα τα MODEs,
	KICKs, και θα εμφανίζει το μήνυμα εισαγωγής.

BOT_HELP_KICK
	Σύνταξη: KICK κανάλι επιλογή παράμετροι
	
	Ρυθμίζει το bot με τις επιλογές του να κάνει kick τους "κακούς χρήστες".
	Οι επιλογές μπορεί να είναι:
	
	    BOLDS         Ρυθμίζει το bot αν θα κάνει kick αυτούς που
	      χρησιμοποιούν έντονους χαρακτήρες
	    BADWORDS      Ρυθμίζει το bot αν θα κάνει kick αυτούς που
	      λένε κακές λέξεις
	    CAPS          Ρυθμίζει το bot αν θα κάνει kick αυτούς που
	      γράφουν με κεφαλαίους χαρακτήρες
	    COLORS        Ρυθμίζει το bot αν θα κάνει kick αυτούς που
	      γράφουν με έγχρωμους χαρακτήρες
	    FLOOD         Ρυθμίζει το bot αν θα κάνει kick αυτούς που
	      γράφουν πολλές σειρές σε λίγα δευτερόλεπτα
	    REPEAT        Ρυθμίζει το bot αν θα κάνει kick αυτούς που
	      επαναλαμβάνουν τα ίδια πράγματα
	    REVERSES      Ρυθμίζει το bot αν θα κάνει kick αυτούς που
	      είναι αντίθετοι(πχ αλλάζουν τα modes)
	    UNDERLINES    Ρυθμίζει το bot αν θα κάνει kick αυτούς που
	      γράφουν με υπογραμμισμένους χαρακτήρες
	
	Γράψε %R%S HELP KICK επιλογή για περισσότερες πληροφορίες
	σε συγκεκριμένες εντολές.
	
	Σημείωση: η access σε αυτή την εντολή ελέγχεται από την
	level SET.

BOT_HELP_KICK_BOLDS
	Σύνταξη: KICK κανάλι BOLDS {ON|OFF} [ttb]

	Ενεργοποιεί την εντολή kick έντονων χαρακτήρων. Αν την
	ενεργοποιήσετε, θα κάνει kick όσους χρησιμοποιούν bolds (έντονους
	χαρακτήρες).

	Το ttb είναι το νούμερο με το οποίο θα γίνεται ο χρήστης αρχικά kick
	και μετά ban. Μην προσπαθήσετε να απενεργοποιήσετε το ttb
	γιατί το σύστημα θα το ενεργοποιήσει ξανά.

BOT_HELP_KICK_COLORS
	Σύνταξη: KICK κανάλι COLORS {ON|OFF} [ttb]

	Ενεργοποιεί την εντολή kick για έγχρωμους χαρακτήρες.Αν την ενεργοποιήσετε,
	αυτή η επιλογή θα κάνει kick όσους χρησιμοποιούν έγχρωμους χαρακτήρες.

	Το ttb είναι το νούμερο με το οποίο θα γίνεται ο χρήστης αρχικά kick
	και μετά ban. Μην προσπαθήσετε να απενεργοποιήσετε το ttb
	γιατί το σύστημα θα το ενεργοποιήσει ξανά.

BOT_HELP_KICK_REVERSES
	Σύνταξη: KICK κανάλι REVERSES {ON|OFF} [ttb]

	Ενεργοποιεί την εντολή kick για τους αντίθετους με τις επιλογές που
	έχει ένα κανάλι.
	
	Το ttb είναι το νούμερο με το οποίο θα γίνεται ο χρήστης αρχικά kick
	και μετά ban. Μην προσπαθήσετε να απενεργοποιήσετε το ttb
	γιατί το σύστημα θα το ενεργοποιήσει ξανά.

BOT_HELP_KICK_UNDERLINES
	Σύνταξη: KICK κανάλι UNDERLINES {ON|OFF} [ttb]

	Ενεργοποιεί την εντολή kick για υπογραμμισμένους χαρακτήρες.Αν την
	ενεργοποιήσετε, αυτή η επιλογή θα κάνει kick όσους χρησιμοποιούν
	υπογραμμισμένους χαρακτήρες.

	Το ttb είναι το νούμερο με το οποίο θα γίνεται ο χρήστης αρχικά kick
	και μετά ban. Μην προσπαθήσετε να απενεργοποιήσετε το ttb
	γιατί το σύστημα θα το ενεργοποιήσει ξανά.

BOT_HELP_KICK_CAPS
	Σύνταξη: KICK κανάλι CAPS {ON|OFF} [ttb [min [ποσοστό]]]

	Ενεργοποιεί την εντολή kick για κεφαλαίους χαρακτήρες.Αν την
	ενεργοποιήσετε, αυτή η επιλογή θα κάνει kick όσους χρησιμοποιούν
	κεφαλαίους χαρακτήρες.

	Το bot θα κάνει kick μόνο αν είναι το λιγότερο min κεφαλαία
	και είναι συνολικά στο ποσοστό%% του συνολικού μηνύματος 
	της γραμμής (αν δεν δωθεί, από default είναι στους 10 χαρακτήρες
	και 25%%).

	Το ttb είναι το νούμερο με το οποίο θα γίνεται ο χρήστης αρχικά kick
	και μετά ban. Μην προσπαθήσετε να απενεργοποιήσετε το ttb
	γιατί το σύστημα θα το ενεργοποιήσει ξανά.

BOT_HELP_KICK_FLOOD
	Σύνταξη: KICK κανάλι FLOOD {ON|OFF} [ttb [ln [secs]]]

	Ενεργοποιεί την εντολή kick για συνεχόμενα μηνύματα.Αν την
	ενεργοποιήσετε, αυτή η επιλογή θα κάνει kick όσους γράφουν
	αρκετά από όσο πρέπει.Οι επιλογές για ένα κανάλι είναι
	το λιγότερο να έχει ln γραμμές σε secs δευτερόλεπτα
	(αν δεν δίνετε, από default είναι 6 γραμμές σε 10 δευτερόλεπτα).
	
	Το ttb είναι το νούμερο με το οποίο θα γίνεται ο χρήστης αρχικά kick
	και μετά ban. Μην προσπαθήσετε να απενεργοποιήσετε το ttb
	γιατί το σύστημα θα το ενεργοποιήσει ξανά.

BOT_HELP_KICK_REPEAT
	Σύνταξη: KICK #κανάλι REPEAT {ON|OFF} [ttb [num]]

	Ενεργοποιεί την εντολή kick για επαναλαμβανόμενα μηνύματα.Αν την
	ενεργοποιήσετε, αυτή η επιλογή θα κάνει kick όσους γράφουν
	τα ίδια πράγματα,επιλέγεται εσείς το num,τις φορές που δεν θέλετε
	να γίνεται (αν δεν δώσετε num, από defaults θα είναι 3).

	Το ttb είναι το νούμερο με το οποίο θα γίνεται ο χρήστης αρχικά kick
	και μετά ban. Μην προσπαθήσετε να απενεργοποιήσετε το ttb
	γιατί το σύστημα θα το ενεργοποιήσει ξανά.

BOT_HELP_KICK_BADWORDS
	Σύνταξη: KICK #κανάλι BADWORDS {ON|OFF} [ttb]

	Ενεργοποιεί την εντολή kick για κακές λέξεις. Όταν ενεργοποιηθεί,
	το bot θα κάνει kick τους χρήστες που λένε αυτές τις κακές λέξεις στα κανάλια.
	
	Μπορείς να βάλεις μια λίστα με τις κακές λέξεις κάνοντας την εντολή
	BADWORDS. Γράψε %R%S HELP BADWORDS για
	περισσότερες πληροφορίες.

	Το ttb είναι το νούμερο με το οποίο θα γίνεται ο χρήστης αρχικά kick
	και μετά ban. Μην προσπαθήσετε να απενεργοποιήσετε το ttb
	γιατί το σύστημα θα το ενεργοποιήσει ξανά.
	
BOT_HELP_BADWORDS
	Σύνταξη: BADWORDS κανάλι ADD λέξη [SINGLE | START | END]
	        BADWORDS κανάλι DEL {λέξη | θέση-νου | list}
	        BADWORDS κανάλι LIST [μάσκα | list]
	        BADWORDS κανάλι CLEAR
	
	Αλλάζει την bad words λίστα του καναλιού. Η λίστα bad
	words δείχνει ποιές λέξεις είναι να γίνεται το kick,
	όταν το bad words kick είναι ενεργοποιημένο. Για περισσότερες πληροφορίες,
	γράφτε %R%S HELP KICK BADWORDS.
	
	Η εντολή BADWORDS ADD προσθέτει μια κακιά λέξη στην λίστα
	badword. Αν πεί αυτη τη λέξη (SINGLE), τότε θα γίνεται
	kick. Αν ξεκινάει με αυτή τη λέξη (START) τότε θα γίνεται 
	kick ο χρήστης επειδή ξεκίνησε με αυτη τη λέξη. Αν στο τέλος της λέξης 
	προσθέσει την κακιά λέξη (END) θα γίνεται kick επειδή την είπε στο τέλος 
	της φράσης/λέξης.  Αν δεν είστε συγκεκριμένος τότε θα γίνεται kick
	όποτε θα λέει συγκεκριμένα αυτή τη λέξη.
	    
	Η εντολή BADWORDS DEL αφαιρεί την λέξη αυτή από την λίστα	badwords.
	Αν σας δίνετε η λίστα με τις θέσεις-νούμερα,  αυτές οι θέσεις θα διαγραφούν
	(Δες το παράδειγμα με την εντολή LIST.)
	
	Η εντολή BADWORDS LIST εμφανίζει την λίστα με τις κακές λέξεις.  Αν δώσετε
	συγκεκριμένη λέξη, θα εμφανιστούν οι θέσεις που ταιριάζουν με αυτήν.
	Παράδειγμα:
	
	   BADWORDS #κανάλι LIST 2-5,7-9
	      Εμφανίζει από την λίστα τις θέσεις με τα νούμερα 2 έως 5 και
	      7 έως 9.
	      
	Η εντολή BADWORDS CLEAR καθαρίζει όλες τις θέσεις από την λίστα
	badword.

BOT_HELP_SAY
	Σύνταξη: SAY κανάλι μήνυμα
	
	Κάνει το bot να λέει το μήνυμα που του δώσατε να το εμφανίσει στο ανάλογο 
	κανάλι.

BOT_HELP_ACT
	Σύνταξη: ACT κανάλι μήνυμα
	
	Κάνει το bot να ενεργεί,δηλ να εκτελεί εντολή "/me"
	εμφανίζοντας το μήνυμα στο ανάλογο κανάλι.


BOT_SERVADMIN_HELP_BOT
	Σύνταξη: BOT ADD ψευδώνυμο ID host πραγματικό όνομα
	        BOT CHANGE παλιό-ψευδώνυμοick νέο-ψευδώνυμο [ID [host [πραγματικό όνομα]]]
	    BOT DEL ψευδώνυμο	        
	
	Επιτρέπει μόνο στους Services admins να δημιουργήσουν, αλλάξουν, και 
	να σβήσουν τα bots στα οποία οι χρήστες θα μπορούν να χρησιμοποιήσουν 
	στα δικά τους τα κανάλια.
	    
	Η εντολή BOT ADD προσθέτει ένα botμε το ψευδώνυμο, ID,
	hostname και πραγματικό όνομα. Αν δεν γίνουν οι κατάλληλοι έλεγχοι 
	με τις επιλογές, να είστε προσεκτικοί.
	Η εντολή BOT CHANGE σας επιτρέπει να αλλάξετε το ψευδώνυμο, ID, hostname
	ή πραγματικό όνομα του bot χωρίς να χρειάζεται να το σβήσετε (και όλες
	οι ρυθμίσεις θα μείνουν).
	Η εντολή BOT DEL αφαιρεί το bot από την λίστα bot.  
	
	Σημείωση: αν φτιάξεις ένα bot που έχει ψευδώνυμο κατοχυρωμένο 
	θα διαγραφεί. Επίσης, αν ένας χρήστης χρησιμοποιεί ήδη το ψευδώνυμο
	την ώρα που φτιάχνεις το bot, θα γίνει kill.

BOT_SERVADMIN_HELP_SET
	
	Αυτές οι επιλογές είναι μόνο για τους Services admins:
	
	    NOBOT            Δείχνει το bot που έγινε assign από το κανάλι 
	    PRIVATE          Δελιχνει το bot που έγινε assigned από μη IRC operators

BOT_SERVADMIN_HELP_SET_NOBOT
	Σύνταξη: SET κανάλι NOBOT {ON|OFF}
	
	Αυτή η επιλογή κάνει το κανάλι να είναι unassignable (να μη μπει bot). Αν 
	το bot είναι ήδη assigned στο κανάλι, θα γίνει unassigned
	αυτόματα, όταν θα ενεργοποιήσεις αυτή την επιλογή.
	
	Μόνο για τους Services admins.

BOT_SERVADMIN_HELP_SET_PRIVATE
	Σύνταξη: SET ψευδώνυμο-bot PRIVATE {ON|OFF}
	
	Αυτή η επιλογή κάνει το bot από assigned σε κανάλια
	των χρηστων που δεν είναι IRC operators.
	
	Μόνο για τους Services admins.
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
	DELALL <nick>.
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
	SET <nick> <hostmask>.
HOST_SETALL_SYNTAX
	SETALL <nick> <hostmask>.
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
	DEL <nick>.

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_OFF
	Your vhost was removed and the normal cloaking restored.
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
	All vhosts in the group %s have been set to %s
HOST_IDENT_GROUP
	All vhosts in the group %s have been set to %s@%s

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
	not used.

HOST_ADMIN_HELP
	not used.

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

