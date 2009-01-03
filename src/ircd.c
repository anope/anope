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
IRCDModes ircd_modes;

/**
 * Globals we want from the protocol file
 **/
IRCDVar *ircd;
IRCDCAPAB *ircdcap;
char *version_protocol;
CBModeInfo *cbmodeinfos;
CUMode cumodes[128];
char *flood_mode_char_set;
char *flood_mode_char_remove;
CBMode cbmodes[128];
CMMode cmmodes[128];
char csmodes[128];
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
	ircdproto->SendNumeric(source, numeric, dest, *buf ? buf : NULL);
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

void pmodule_ircd_cbmodeinfos(CBModeInfo * modeinfos)
{
	cbmodeinfos = modeinfos;
}

void pmodule_ircd_cumodes(CUMode modes[128])
{
	int i = 0;
	for (i = 0; i < 128; i++) {
		cumodes[i] = modes[i];
	}
}

void pmodule_ircd_flood_mode_char_set(const char *mode)
{
	flood_mode_char_set = sstrdup(mode);
}

void pmodule_ircd_flood_mode_char_remove(const char *mode)
{
	flood_mode_char_remove = sstrdup(mode);
}

void pmodule_ircd_cbmodes(CBMode modes[128])
{
	int i = 0;
	for (i = 0; i < 128; i++) {
		cbmodes[i] = modes[i];
	}
}

void pmodule_ircd_cmmodes(CMMode modes[128])
{
	int i = 0;
	for (i = 0; i < 128; i++) {
		cmmodes[i] = modes[i];
	}
}

void pmodule_ircd_csmodes(char mode[128])
{
	int i = 0;
	for (i = 0; i < 128; i++) {
		csmodes[i] = mode[i];
	}
}

void pmodule_ircd_useTSMode(int use)
{
	UseTSMODE = use;
}

/** mode stuff */

void pmodule_invis_umode(int mode)
{
	ircd_modes.user_invis = mode;
}

void pmodule_oper_umode(int mode)
{
	ircd_modes.user_oper = mode;
}

void pmodule_invite_cmode(int mode)
{
	ircd_modes.chan_invite = mode;
}

void pmodule_secret_cmode(int mode)
{
	ircd_modes.chan_secret = mode;
}

void pmodule_private_cmode(int mode)
{
	ircd_modes.chan_private = mode;
}

void pmodule_key_mode(int mode)
{
	ircd_modes.chan_key = mode;
}

void pmodule_limit_mode(int mode)
{
	ircd_modes.chan_limit = mode;
}

int anope_get_invis_mode()
{
	return ircd_modes.user_invis;
}

int anope_get_oper_mode()
{
	return ircd_modes.user_oper;
}

int anope_get_invite_mode()
{
	return ircd_modes.chan_invite;
}

int anope_get_secret_mode()
{
	return ircd_modes.chan_secret;
}

int anope_get_private_mode()
{
	return ircd_modes.chan_private;
}

int anope_get_key_mode()
{
	return ircd_modes.chan_key;
}

int anope_get_limit_mode()
{
	return ircd_modes.chan_limit;
}
