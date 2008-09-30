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

IRCDProto ircdproto;
IRCDProtoNew *ircdprotonew;
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

void pmodule_ircd_proto(IRCDProtoNew *proto)
{
	ircdprotonew = proto;
}

/**
 * Initiate a protocol struct ready for use
 **/
void initIrcdProto()
{
    ircdproto.ircd_set_mod_current_buffer = NULL;
    ircdproto.ircd_set_umode = NULL;
    ircdproto.ircd_cmd_372 = NULL;
    ircdproto.ircd_cmd_372_error = NULL;
    ircdproto.ircd_cmd_375 = NULL;
    ircdproto.ircd_cmd_376 = NULL;
    ircdproto.ircd_cmd_351 = NULL;
    ircdproto.ircd_cmd_391 = NULL;
    ircdproto.ircd_cmd_250 = NULL;
    ircdproto.ircd_cmd_307 = NULL;
    ircdproto.ircd_cmd_311 = NULL;
    ircdproto.ircd_cmd_312 = NULL;
    ircdproto.ircd_cmd_317 = NULL;
    ircdproto.ircd_cmd_219 = NULL;
    ircdproto.ircd_cmd_401 = NULL;
    ircdproto.ircd_cmd_318 = NULL;
    ircdproto.ircd_cmd_242 = NULL;
    ircdproto.ircd_cmd_243 = NULL;
    ircdproto.ircd_cmd_211 = NULL;
    ircdproto.ircd_flood_mode_check = NULL;
    ircdproto.ircd_valid_nick = NULL;
    ircdproto.ircd_valid_chan = NULL;
}

/* Special function, returns 1 if executed, 0 if not */
int anope_set_mod_current_buffer(int ac, char **av)
{
    if (ircdproto.ircd_set_mod_current_buffer) {
        ircdproto.ircd_set_mod_current_buffer(ac, av);
        return 1;
    }

    return 0;
}

void anope_set_umode(User * user, int ac, const char **av)
{
    ircdproto.ircd_set_umode(user, ac, av);
}

void anope_cmd_svsnoop(const char *server, int set)
{
	ircdprotonew->cmd_svsnoop(server, set);
}

void anope_cmd_remove_akill(const char *user, const char *host)
{
	ircdprotonew->cmd_remove_akill(user, host);
}

void anope_cmd_topic(const char *whosets, const char *chan, const char *whosetit, const char *topic, time_t when)
{
	ircdprotonew->cmd_topic(whosets, chan, whosetit, topic, when);
}

void anope_cmd_vhost_off(User *u)
{
	ircdprotonew->cmd_vhost_off(u);
}

void anope_cmd_akill(const char *user, const char *host, const char *who, time_t when, time_t expires, const char *reason)
{
	ircdprotonew->cmd_akill(user, host, who, when, expires, reason);
}

void anope_cmd_svskill(const char *source, const char *user, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdprotonew->cmd_svskill(source, user, buf);
}

void anope_cmd_svsmode(User *u, int ac, const char **av)
{
	ircdprotonew->cmd_svsmode(u, ac, av);
}

void anope_cmd_372(const char *source, const char *msg)
{
    ircdproto.ircd_cmd_372(source, msg);
}

void anope_cmd_372_error(const char *source)
{
    ircdproto.ircd_cmd_372_error(source);
}

void anope_cmd_375(const char *source)
{
    ircdproto.ircd_cmd_375(source);
}

void anope_cmd_376(const char *source)
{
    ircdproto.ircd_cmd_376(source);
}

void anope_cmd_guest_nick(const char *nick, const char *user, const char *host, const char *real, const char *modes)
{
	ircdprotonew->cmd_guest_nick(nick, user, host, real, modes);
}

void anope_cmd_mode(const char *source, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdprotonew->cmd_mode(source, dest, buf);
}

void anope_cmd_bot_nick(const char *nick, const char *user, const char *host, const char *real, const char *modes)
{
	ircdprotonew->cmd_bot_nick(nick, user, host, real, modes);
}

void anope_cmd_kick(const char *source, const char *chan, const char *user, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdprotonew->cmd_kick(source, chan, user, buf);
}

void anope_cmd_notice_ops(const char *source, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdprotonew->cmd_notice_ops(source, dest, buf);
}

void anope_cmd_message(const char *source, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdprotonew->cmd_message(source, dest, buf);
}

void anope_cmd_notice(const char *source, const char *dest, const char *msg)
{
	ircdprotonew->cmd_notice(source, dest, msg);
}

void anope_cmd_action(const char *source, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "", actionbuf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	else return;
	if (!*buf) return;
	snprintf(actionbuf, BUFSIZE - 1, "%cACTION %s%c", 1, buf, 1);
	ircdprotonew->cmd_privmsg(source, dest, actionbuf);
}

void anope_cmd_privmsg(const char *source, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdprotonew->cmd_privmsg(source, dest, buf);
}

void anope_cmd_serv_notice(const char *source, const char *dest, const char *msg)
{
	ircdprotonew->cmd_serv_notice(source, dest, msg);
}

void anope_cmd_serv_privmsg(const char *source, const char *dest, const char *msg)
{
	ircdprotonew->cmd_serv_privmsg(source, dest, msg);
}

void anope_cmd_bot_chan_mode(const char *nick, const char *chan)
{
	ircdprotonew->cmd_bot_chan_mode(nick, chan);
}

void anope_cmd_351(const char *source)
{
    ircdproto.ircd_cmd_351(source);
}

void anope_cmd_quit(const char *source, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdprotonew->cmd_quit(source, buf);
}

void anope_cmd_pong(const char *servname, const char *who)
{
	ircdprotonew->cmd_pong(servname, who);
}

void anope_cmd_join(const char *user, const char *channel, time_t chantime)
{
	ircdprotonew->cmd_join(user, channel, chantime);
}

void anope_cmd_unsqline(const char *user)
{
	ircdprotonew->cmd_unsqline(user);
}

void anope_cmd_invite(const char *source, const char *chan, const char *nick)
{
	ircdprotonew->cmd_invite(source, chan, nick);
}

void anope_cmd_part(const char *nick, const char *chan, const char *fmt, ...)
{
	if (fmt) {
		va_list args;
		char buf[BUFSIZE] = "";
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
		ircdprotonew->cmd_part(nick, chan, buf);
	}
	else ircdprotonew->cmd_part(nick, chan, NULL);
}

void anope_cmd_391(const char *source, const char *timestr)
{
    ircdproto.ircd_cmd_391(source, timestr);
}

void anope_cmd_250(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_250(buf);
}

void anope_cmd_307(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_307(buf);
}

void anope_cmd_311(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_311(buf);
}

void anope_cmd_312(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_312(buf);
}

void anope_cmd_317(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_317(buf);
}

void anope_cmd_219(const char *source, const char *letter)
{
    ircdproto.ircd_cmd_219(source, letter);
}

void anope_cmd_401(const char *source, const char *who)
{
    ircdproto.ircd_cmd_401(source, who);
}

void anope_cmd_318(const char *source, const char *who)
{
    ircdproto.ircd_cmd_318(source, who);
}

void anope_cmd_242(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_242(buf);
}

void anope_cmd_243(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_243(buf);
}

void anope_cmd_211(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_211(buf);
}

void anope_cmd_global(const char *source, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdprotonew->cmd_global(source, buf);
}

void anope_cmd_sqline(const char *mask, const char *reason)
{
	ircdprotonew->cmd_sqline(mask, reason);
}

void anope_cmd_squit(const char *servname, const char *message)
{
	ircdprotonew->cmd_squit(servname, message);
}

void anope_cmd_svso(const char *source, const char *nick, const char *flag)
{
	ircdprotonew->cmd_svso(source, nick, flag);
}

void anope_cmd_chg_nick(const char *oldnick, const char *newnick)
{
	ircdprotonew->cmd_chg_nick(oldnick, newnick);
}

void anope_cmd_svsnick(const char *source, const char *guest, time_t when)
{
	ircdprotonew->cmd_svsnick(source, guest, when);
}

void anope_cmd_vhost_on(const char *nick, const char *vIdent, const char *vhost)
{
	ircdprotonew->cmd_vhost_on(nick, vIdent, vhost);
}

void anope_cmd_connect()
{
	ircdprotonew->cmd_connect();
}

void anope_cmd_svshold(const char *nick)
{
	ircdprotonew->cmd_svshold(nick);
}

void anope_cmd_release_svshold(const char *nick)
{
	ircdprotonew->cmd_release_svshold(nick);
}

void anope_cmd_unsgline(const char *mask)
{
	ircdprotonew->cmd_unsgline(mask);
}

void anope_cmd_unszline(const char *mask)
{
	ircdprotonew->cmd_unszline(mask);
}

void anope_cmd_szline(const char *mask, const char *reason, const char *whom)
{
	ircdprotonew->cmd_szline(mask, reason, whom);
}

void anope_cmd_sgline(const char *mask, const char *reason)
{
	ircdprotonew->cmd_sgline(mask, reason);
}

void anope_cmd_unban(const char *name, const char *nick)
{
	ircdprotonew->cmd_unban(name, nick);
}

void anope_cmd_svsmode_chan(const char *name, const char *mode, const char *nick)
{
	ircdprotonew->cmd_svsmode_chan(name, mode, nick);
}

void anope_cmd_svid_umode(const char *nick, time_t ts)
{
	ircdprotonew->cmd_svid_umode(nick, ts);
}

void anope_cmd_nc_change(User *u)
{
	ircdprotonew->cmd_nc_change(u);
}

void anope_cmd_svid_umode2(User *u, const char *ts)
{
	ircdprotonew->cmd_svid_umode2(u, ts);
}

void anope_cmd_svid_umode3(User *u, const char *ts)
{
	ircdprotonew->cmd_svid_umode3(u, ts);
}

void anope_cmd_svsjoin(const char *source, const char *nick, const char *chan, const char *param)
{
	ircdprotonew->cmd_svsjoin(source, nick, chan, param);
}

void anope_cmd_svspart(const char *source, const char *nick, const char *chan)
{
	ircdprotonew->cmd_svspart(source, nick, chan);
}

void anope_cmd_swhois(const char *source, const char *who, const char *mask)
{
	ircdprotonew->cmd_swhois(source, who, mask);
}

void anope_cmd_eob()
{
	ircdprotonew->cmd_eob();
}

int anope_flood_mode_check(const char *value)
{
    return ircdproto.ircd_flood_mode_check(value);
}

void anope_cmd_jupe(const char *jserver, const char *who, const char *reason)
{
	ircdprotonew->cmd_jupe(jserver, who, reason);
}

int anope_valid_nick(const char *nick)
{
    return ircdproto.ircd_valid_nick(nick);
}

int anope_valid_chan(const char *chan)
{
    return ircdproto.ircd_valid_chan(chan);
}


void anope_cmd_ctcp(const char *source, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	ircdprotonew->cmd_ctcp(source, dest, buf);
}



/**
 * Set routines for modules to set the prefered function for dealing with things.
 **/
void pmodule_cmd_372(void (*func) (const char *source, const char *msg))
{
    ircdproto.ircd_cmd_372 = func;
}

void pmodule_cmd_372_error(void (*func) (const char *source))
{
    ircdproto.ircd_cmd_372_error = func;
}

void pmodule_cmd_375(void (*func) (const char *source))
{
    ircdproto.ircd_cmd_375 = func;
}

void pmodule_cmd_376(void (*func) (const char *source))
{
    ircdproto.ircd_cmd_376 = func;
}

void pmodule_cmd_351(void (*func) (const char *source))
{
    ircdproto.ircd_cmd_351 = func;
}

void pmodule_cmd_391(void (*func) (const char *source, const char *timestr))
{
    ircdproto.ircd_cmd_391 = func;
}

void pmodule_cmd_250(void (*func) (const char *buf))
{
    ircdproto.ircd_cmd_250 = func;
}

void pmodule_cmd_307(void (*func) (const char *buf))
{
    ircdproto.ircd_cmd_307 = func;
}

void pmodule_cmd_311(void (*func) (const char *buf))
{
    ircdproto.ircd_cmd_311 = func;
}

void pmodule_cmd_312(void (*func) (const char *buf))
{
    ircdproto.ircd_cmd_312 = func;
}

void pmodule_cmd_317(void (*func) (const char *buf))
{
    ircdproto.ircd_cmd_317 = func;
}

void pmodule_cmd_219(void (*func) (const char *source, const char *letter))
{
    ircdproto.ircd_cmd_219 = func;
}

void pmodule_cmd_401(void (*func) (const char *source, const char *who))
{
    ircdproto.ircd_cmd_401 = func;
}

void pmodule_cmd_318(void (*func) (const char *source, const char *who))
{
    ircdproto.ircd_cmd_318 = func;
}

void pmodule_cmd_242(void (*func) (const char *buf))
{
    ircdproto.ircd_cmd_242 = func;
}

void pmodule_cmd_243(void (*func) (const char *buf))
{
    ircdproto.ircd_cmd_243 = func;
}

void pmodule_cmd_211(void (*func) (const char *buf))
{
    ircdproto.ircd_cmd_211 = func;
}

void pmodule_set_umode(void (*func) (User * user, int ac, const char **av))
{
    ircdproto.ircd_set_umode = func;
}

void pmodule_valid_nick(int (*func) (const char *nick))
{
    ircdproto.ircd_valid_nick = func;
}

void pmodule_valid_chan(int (*func) (const char *chan))
{
    ircdproto.ircd_valid_chan = func;
}

void pmodule_flood_mode_check(int (*func) (const char *value))
{
    ircdproto.ircd_flood_mode_check = func;
}

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
