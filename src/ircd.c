/* Main ircd functions.
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */

#include "services.h"
#include "extern.h"

IRCDProto *ircdproto;

/**
 * Globals we want from the protocol file
 **/
IRCDVar *ircd;
IRCDCAPAB *ircdcap;
char *version_protocol;
int UseTSMODE;

void pmodule_ircd_proto(IRCDProto *proto)
{
	ircdproto = proto;
}

void anope_SendNumeric(const char *source, int numeric, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdproto->SendNumeric(source, numeric, dest, buf);
}

/**
 * Set routines for modules to set the prefered function for dealing with things.
 **/
void pmodule_ircd_var(IRCDVar * ircdvar)
{
	ircd = ircdvar;
}

void pmodule_ircd_cap(IRCDCAPAB * cap)
{
	ircdcap = cap;
}

void pmodule_ircd_version(const char *version)
{
	version_protocol = sstrdup(version);
}

void pmodule_ircd_useTSMode(int use)
{
	UseTSMODE = use;
}
