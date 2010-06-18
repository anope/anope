/* Services configuration.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

#ifndef CONFIG_H
#define CONFIG_H

/* Note that most of the options which used to be here have been moved to
 * services.conf. */

/*************************************************************************/

/******* General configuration *******/

/* Name of log file (in Services directory) */
#define LOG_FILENAME	"services.log"

/******************* END OF USER-CONFIGURABLE SECTION ********************/

/* Size of input buffer (note: this is different from BUFSIZ)
 * This must be big enough to hold at least one full IRC message, or messy
 * things will happen. */
#define BUFSIZE		1024

/* Maximum amount of data from/to the network to buffer (bytes). */
#define NET_BUFSIZE    65536

/**************************************************************************/

#endif	/* CONFIG_H */
