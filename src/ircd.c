/* Main ircd functions.
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
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

void anope_ProcessUsermodes(User *user, int ac, const char **av)
{
	ircdproto->ProcessUsermodes(user, ac, av);
}

void anope_SendGlobalPrivmsg(const char *source, const char *dest, const char *msg)
{
	ircdproto->SendGlobalPrivmsg(source, dest, msg);
}

void anope_SendBotOp(const char *nick, const char *chan)
{
	ircdproto->SendBotOp(nick, chan);
}

void anope_SendQuit(const char *source, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdproto->SendQuit(source, buf);
}

void anope_SendPong(const char *servname, const char *who)
{
	ircdproto->SendPong(servname, who);
}

void anope_SendJoin(const char *user, const char *channel, time_t chantime)
{
	ircdproto->SendJoin(user, channel, chantime);
}

void anope_SendSQLineDel(const char *user)
{
	ircdproto->SendSQLineDel(user);
}

void anope_SendInvite(const char *source, const char *chan, const char *nick)
{
	ircdproto->SendInvite(source, chan, nick);
}

void anope_SendPart(const char *nick, const char *chan, const char *fmt, ...)
{
	if (fmt) {
		va_list args;
		char buf[BUFSIZE] = "";
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
		ircdproto->SendPart(nick, chan, buf);
	}
	else ircdproto->SendPart(nick, chan, NULL);
}

void anope_SendGlobops(const char *source, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdproto->SendGlobops(source, buf);
}

void anope_SendSQLine(const char *mask, const char *reason)
{
	ircdproto->SendSQLine(mask, reason);
}

void anope_SendSquit(const char *servname, const char *message)
{
	ircdproto->SendSquit(servname, message);
}

void anope_SendSVSO(const char *source, const char *nick, const char *flag)
{
	ircdproto->SendSVSO(source, nick, flag);
}

void anope_SendChangeBotNick(const char *oldnick, const char *newnick)
{
	ircdproto->SendChangeBotNick(oldnick, newnick);
}

void anope_SendForceNickChange(const char *source, const char *guest, time_t when)
{
	ircdproto->SendForceNickChange(source, guest, when);
}

void anope_SendVhost(const char *nick, const char *vIdent, const char *vhost)
{
	ircdproto->SendVhost(nick, vIdent, vhost);
}

void anope_SendConnect()
{
	ircdproto->SendConnect();
}

void anope_SendSVSHOLD(const char *nick)
{
	ircdproto->SendSVSHOLD(nick);
}

void anope_SendSVSHOLDDel(const char *nick)
{
	ircdproto->SendSVSHOLDDel(nick);
}

void anope_SendSGLineDel(const char *mask)
{
	ircdproto->SendSGLineDel(mask);
}

void anope_SendSZLineDel(const char *mask)
{
	ircdproto->SendSZLineDel(mask);
}

void anope_SendSZLine(const char *mask, const char *reason, const char *whom)
{
	ircdproto->SendSZLine(mask, reason, whom);
}

void anope_SendSGLine(const char *mask, const char *reason)
{
	ircdproto->SendSGLine(mask, reason);
}

void anope_SendBanDel(const char *name, const char *nick)
{
	ircdproto->SendBanDel(name, nick);
}

void anope_SendSVSMode_chan(const char *name, const char *mode, const char *nick)
{
	ircdproto->SendSVSMode_chan(name, mode, nick);
}

void anope_SendSVID(const char *nick, time_t ts)
{
	ircdproto->SendSVID(nick, ts);
}

void anope_SendUnregisteredNick(User *u)
{
	ircdproto->SendUnregisteredNick(u);
}

void anope_SendSVID2(User *u, const char *ts)
{
	ircdproto->SendSVID2(u, ts);
}

void anope_SendSVID3(User *u, const char *ts)
{
	ircdproto->SendSVID3(u, ts);
}

void anope_SendSVSJoin(const char *source, const char *nick, const char *chan, const char *param)
{
	ircdproto->SendSVSJoin(source, nick, chan, param);
}

void anope_SendSVSPart(const char *source, const char *nick, const char *chan)
{
	ircdproto->SendSVSPart(source, nick, chan);
}

void anope_SendSWhois(const char *source, const char *who, const char *mask)
{
	ircdproto->SendSWhois(source, who, mask);
}

void anope_SendEOB()
{
	ircdproto->SendEOB();
}

int anope_IsFloodModeParamValid(const char *value)
{
	return ircdproto->IsFloodModeParamValid(value);
}

void anope_SendJupe(const char *jserver, const char *who, const char *reason)
{
	ircdproto->SendJupe(jserver, who, reason);
}

int anope_IsNickValid(const char *nick)
{
	return ircdproto->IsNickValid(nick);
}

int anope_IsChannelValid(const char *chan)
{
	return ircdproto->IsChannelValid(chan);
}


void anope_SendCTCP(const char *source, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdproto->SendCTCP(source, dest, buf);
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
