/* Services configuration.
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$ 
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

/* Note that most of the options which used to be here have been moved to
 * services.conf. */

/*************************************************************************/

/******* General configuration *******/

/* Name of configuration file (in Services directory) */
#define SERVICES_CONF	"services.conf"

/* Name of anope datbase */
#define DATABASE_FILE	"anope.db"

/* Name of log file (in Services directory) */
#define LOG_FILENAME	"services.log"

/* Maximum amount of data from/to the network to buffer (bytes). */
#define NET_BUFSIZE	65536

/******* OperServ configuration *******/

/* Define this to enable OperServ's svs commands (superadmin only). */
 #define USE_OSSVS 

/******************* END OF USER-CONFIGURABLE SECTION ********************/

/* Size of input buffer (note: this is different from BUFSIZ)
 * This must be big enough to hold at least one full IRC message, or messy
 * things will happen. */
#define BUFSIZE		1024

/* Extra warning:  If you change CHANMAX, your ChanServ database will be
 * unusable. 
 */

/* Maximum length of a password */
#define PASSMAX		32

/* Maximum length of a username */
#define USERMAX		10

/* Maximum length of a domain */
#define HOSTMAX		64

/**************************************************************************/

#endif	/* CONFIG_H */
