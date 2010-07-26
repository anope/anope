/* Main ircd functions.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "extern.h"

IRCDProto *ircdproto;

/**
 * Globals we want from the protocol file
 **/
IRCDVar *ircd;
Anope::string version_protocol;
int UseTSMODE;

void pmodule_ircd_proto(IRCDProto *proto)
{
	ircdproto = proto;
}

/**
 * Set routines for modules to set the prefered function for dealing with things.
 **/
void pmodule_ircd_var(IRCDVar *ircdvar)
{
	ircd = ircdvar;
}

void pmodule_ircd_version(const Anope::string &version)
{
	version_protocol = version;
}

void pmodule_ircd_useTSMode(int use)
{
	UseTSMODE = use;
}
