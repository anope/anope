/* Main ircd functions.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#include "services.h"
#include "extern.h"

IRCDProto ircdproto;
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

/**
 * Initiate a protocol struct ready for use
 **/
void initIrcdProto()
{
    ircdproto.ircd_set_mod_current_buffer = NULL;
    ircdproto.ircd_set_umode = NULL;
    ircdproto.ircd_cmd_svsnoop = NULL;
    ircdproto.ircd_cmd_remove_akill = NULL;
    ircdproto.ircd_cmd_topic = NULL;
    ircdproto.ircd_cmd_vhost_off = NULL;
    ircdproto.ircd_cmd_akill = NULL;
    ircdproto.ircd_cmd_svskill = NULL;
    ircdproto.ircd_cmd_svsmode = NULL;
    ircdproto.ircd_cmd_372 = NULL;
    ircdproto.ircd_cmd_372_error = NULL;
    ircdproto.ircd_cmd_375 = NULL;
    ircdproto.ircd_cmd_376 = NULL;
    ircdproto.ircd_cmd_nick = NULL;
    ircdproto.ircd_cmd_guest_nick = NULL;
    ircdproto.ircd_cmd_mode = NULL;
    ircdproto.ircd_cmd_bot_nick = NULL;
    ircdproto.ircd_cmd_kick = NULL;
    ircdproto.ircd_cmd_notice_ops = NULL;
    ircdproto.ircd_cmd_notice = NULL;
    ircdproto.ircd_cmd_notice2 = NULL;
    ircdproto.ircd_cmd_privmsg = NULL;
    ircdproto.ircd_cmd_privmsg2 = NULL;
    ircdproto.ircd_cmd_serv_notice = NULL;
    ircdproto.ircd_cmd_serv_privmsg = NULL;
    ircdproto.ircd_cmd_bot_chan_mode = NULL;
    ircdproto.ircd_cmd_351 = NULL;
    ircdproto.ircd_cmd_quit = NULL;
    ircdproto.ircd_cmd_pong = NULL;
    ircdproto.ircd_cmd_join = NULL;
    ircdproto.ircd_cmd_unsqline = NULL;
    ircdproto.ircd_cmd_invite = NULL;
    ircdproto.ircd_cmd_part = NULL;
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
    ircdproto.ircd_cmd_global = NULL;
    ircdproto.ircd_cmd_global_legacy = NULL;
    ircdproto.ircd_cmd_sqline = NULL;
    ircdproto.ircd_cmd_squit = NULL;
    ircdproto.ircd_cmd_svso = NULL;
    ircdproto.ircd_cmd_chg_nick = NULL;
    ircdproto.ircd_cmd_svsnick = NULL;
    ircdproto.ircd_cmd_vhost_on = NULL;
    ircdproto.ircd_cmd_connect = NULL;
    ircdproto.ircd_cmd_bob = NULL;
    ircdproto.ircd_cmd_svshold = NULL;
    ircdproto.ircd_cmd_release_svshold = NULL;
    ircdproto.ircd_cmd_unsgline = NULL;
    ircdproto.ircd_cmd_unszline = NULL;
    ircdproto.ircd_cmd_szline = NULL;
    ircdproto.ircd_cmd_sgline = NULL;
    ircdproto.ircd_cmd_unban = NULL;
    ircdproto.ircd_cmd_svsmode_chan = NULL;
    ircdproto.ircd_cmd_svid_umode = NULL;
    ircdproto.ircd_cmd_nc_change = NULL;
    ircdproto.ircd_cmd_svid_umode2 = NULL;
    ircdproto.ircd_cmd_svid_umode3 = NULL;
    ircdproto.ircd_cmd_svsjoin = NULL;
    ircdproto.ircd_cmd_svspart = NULL;
    ircdproto.ircd_cmd_swhois = NULL;
    ircdproto.ircd_cmd_eob = NULL;
    ircdproto.ircd_flood_mode_check = NULL;
    ircdproto.ircd_cmd_jupe = NULL;
    ircdproto.ircd_valid_nick = NULL;
    ircdproto.ircd_valid_chan = NULL;
    ircdproto.ircd_cmd_ctcp = NULL;
    ircdproto.ircd_jointhrottle_mode_check = NULL;
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

void anope_set_umode(User * user, int ac, char **av)
{
    ircdproto.ircd_set_umode(user, ac, av);
}

void anope_cmd_svsnoop(char *server, int set)
{
    ircdproto.ircd_cmd_svsnoop(server, set);
}

void anope_cmd_remove_akill(char *user, char *host)
{
    ircdproto.ircd_cmd_remove_akill(user, host);
}

void anope_cmd_topic(char *whosets, char *chan, char *whosetit,
                     char *topic, time_t when)
{
    ircdproto.ircd_cmd_topic(whosets, chan, whosetit, topic, when);
}

void anope_cmd_vhost_off(User * u)
{
    ircdproto.ircd_cmd_vhost_off(u);
}

void anope_cmd_akill(char *user, char *host, char *who, time_t when,
                     time_t expires, char *reason)
{
    ircdproto.ircd_cmd_akill(user, host, who, when, expires, reason);
}

void anope_cmd_svskill(char *source, char *user, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_svskill(source, user, buf);
}

void anope_cmd_svsmode(User * u, int ac, char **av)
{
    ircdproto.ircd_cmd_svsmode(u, ac, av);
}

void anope_cmd_372(char *source, char *msg)
{
    ircdproto.ircd_cmd_372(source, msg);
}

void anope_cmd_372_error(char *source)
{
    ircdproto.ircd_cmd_372_error(source);
}

void anope_cmd_375(char *source)
{
    ircdproto.ircd_cmd_375(source);
}

void anope_cmd_376(char *source)
{
    ircdproto.ircd_cmd_376(source);
}

void anope_cmd_nick(char *nick, char *name, char *modes)
{
    ircdproto.ircd_cmd_nick(nick, name, modes);
}

void anope_cmd_guest_nick(char *nick, char *user, char *host, char *real,
                          char *modes)
{
    ircdproto.ircd_cmd_guest_nick(nick, user, host, real, modes);
}

void anope_cmd_mode(char *source, char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_mode(source, dest, buf);
}

void anope_cmd_bot_nick(char *nick, char *user, char *host, char *real,
                        char *modes)
{
    ircdproto.ircd_cmd_bot_nick(nick, user, host, real, modes);
}

void anope_cmd_kick(char *source, char *chan, char *user, const char *fmt,
                    ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_kick(source, chan, user, buf);
}

void anope_cmd_notice_ops(char *source, char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_notice_ops(source, dest, buf);
}

void anope_cmd_notice(char *source, char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_notice(source, dest, buf);
}

void anope_cmd_notice2(char *source, char *dest, char *msg)
{
    ircdproto.ircd_cmd_notice2(source, dest, msg);
}

void anope_cmd_action(char *source, char *dest, const char *fmt, ...) 
{
    va_list args;
    char buf[BUFSIZE];
    char actionbuf[BUFSIZE];
    *buf = '\0';
    *actionbuf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }

    if (!*buf) {
        return;
    }
    snprintf(actionbuf, BUFSIZE - 1, "%cACTION %s %c", 1, buf, 1);
    ircdproto.ircd_cmd_privmsg(source, dest, actionbuf);
}

void anope_cmd_privmsg(char *source, char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_privmsg(source, dest, buf);
}

void anope_cmd_privmsg2(char *source, char *dest, char *msg)
{
    ircdproto.ircd_cmd_privmsg2(source, dest, msg);
}

void anope_cmd_serv_notice(char *source, char *dest, char *msg)
{
    ircdproto.ircd_cmd_serv_notice(source, dest, msg);
}

void anope_cmd_serv_privmsg(char *source, char *dest, char *msg)
{
    ircdproto.ircd_cmd_serv_privmsg(source, dest, msg);
}

void anope_cmd_bot_chan_mode(char *nick, char *chan)
{
    ircdproto.ircd_cmd_bot_chan_mode(nick, chan);
}

void anope_cmd_351(char *source)
{
    ircdproto.ircd_cmd_351(source);
}

void anope_cmd_quit(char *source, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_quit(source, buf);
}

void anope_cmd_pong(char *servname, char *who)
{
    ircdproto.ircd_cmd_pong(servname, who);
}

void anope_cmd_join(char *user, char *channel, time_t chantime)
{
    ircdproto.ircd_cmd_join(user, channel, chantime);
}

void anope_cmd_unsqline(char *user)
{
    ircdproto.ircd_cmd_unsqline(user);
}

void anope_cmd_invite(char *source, char *chan, char *nick)
{
    ircdproto.ircd_cmd_invite(source, chan, nick);
}

void anope_cmd_part(char *nick, char *chan, const char *fmt, ...)
{
    if (fmt) {
        va_list args;
        char buf[BUFSIZE];
        *buf = '\0';
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
        ircdproto.ircd_cmd_part(nick, chan, buf);
    } else {
        ircdproto.ircd_cmd_part(nick, chan, NULL);
    }
}

void anope_cmd_391(char *source, char *timestr)
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

void anope_cmd_219(char *source, char *letter)
{
    ircdproto.ircd_cmd_219(source, letter);
}

void anope_cmd_401(char *source, char *who)
{
    ircdproto.ircd_cmd_401(source, who);
}

void anope_cmd_318(char *source, char *who)
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

void anope_cmd_global(char *source, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_global(source, buf);
}

void anope_cmd_global_legacy(char *source, char *fmt)
{
    ircdproto.ircd_cmd_global_legacy(source, fmt);
}

void anope_cmd_sqline(char *mask, char *reason)
{
    ircdproto.ircd_cmd_sqline(mask, reason);
}

void anope_cmd_squit(char *servname, char *message)
{
    ircdproto.ircd_cmd_squit(servname, message);
}

void anope_cmd_svso(char *source, char *nick, char *flag)
{
    ircdproto.ircd_cmd_svso(source, nick, flag);
}

void anope_cmd_chg_nick(char *oldnick, char *newnick)
{
    ircdproto.ircd_cmd_chg_nick(oldnick, newnick);
}

void anope_cmd_svsnick(char *source, char *guest, time_t when)
{
    ircdproto.ircd_cmd_svsnick(source, guest, when);
}

void anope_cmd_vhost_on(char *nick, char *vIdent, char *vhost)
{
    ircdproto.ircd_cmd_vhost_on(nick, vIdent, vhost);
}

void anope_cmd_connect(int servernum)
{
    ircdproto.ircd_cmd_connect(servernum);
}

void anope_cmd_bob()
{
    ircdproto.ircd_cmd_bob();
}

void anope_cmd_svshold(char *nick)
{
    ircdproto.ircd_cmd_svshold(nick);
}

void anope_cmd_release_svshold(char *nick)
{
    ircdproto.ircd_cmd_release_svshold(nick);
}

void anope_cmd_unsgline(char *mask)
{
    ircdproto.ircd_cmd_unsgline(mask);
}

void anope_cmd_unszline(char *mask)
{
    ircdproto.ircd_cmd_unszline(mask);
}

void anope_cmd_szline(char *mask, char *reason, char *whom)
{
    ircdproto.ircd_cmd_szline(mask, reason, whom);
}

void anope_cmd_sgline(char *mask, char *reason)
{
    ircdproto.ircd_cmd_sgline(mask, reason);
}

void anope_cmd_unban(char *name, char *nick)
{
    ircdproto.ircd_cmd_unban(name, nick);
}

void anope_cmd_svsmode_chan(char *name, char *mode, char *nick)
{
    ircdproto.ircd_cmd_svsmode_chan(name, mode, nick);
}

void anope_cmd_svid_umode(char *nick, time_t ts)
{
    ircdproto.ircd_cmd_svid_umode(nick, ts);
}

void anope_cmd_nc_change(User * u)
{
    ircdproto.ircd_cmd_nc_change(u);
}

void anope_cmd_svid_umode2(User * u, char *ts)
{
    ircdproto.ircd_cmd_svid_umode2(u, ts);
}

void anope_cmd_svid_umode3(User * u, char *ts)
{
    ircdproto.ircd_cmd_svid_umode3(u, ts);
}

void anope_cmd_svsjoin(char *source, char *nick, char *chan, char *param)
{
    ircdproto.ircd_cmd_svsjoin(source, nick, chan, param);
}

void anope_cmd_svspart(char *source, char *nick, char *chan)
{
    ircdproto.ircd_cmd_svspart(source, nick, chan);
}

void anope_cmd_swhois(char *source, char *who, char *mask)
{
    ircdproto.ircd_cmd_swhois(source, who, mask);
}

void anope_cmd_eob()
{
    ircdproto.ircd_cmd_eob();
}

int anope_flood_mode_check(char *value)
{
    return ircdproto.ircd_flood_mode_check(value);
}

int anope_jointhrottle_mode_check(char *value)
{
    if (ircd->jmode)
        return ircdproto.ircd_jointhrottle_mode_check(value);
    return 0;
}

void anope_cmd_jupe(char *jserver, char *who, char *reason)
{
    ircdproto.ircd_cmd_jupe(jserver, who, reason);
}

int anope_valid_nick(char *nick)
{
    return ircdproto.ircd_valid_nick(nick);
}

int anope_valid_chan(char *chan)
{
    return ircdproto.ircd_valid_chan(chan);
}


void anope_cmd_ctcp(char *source, char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];
    *buf = '\0';
    if (fmt) {
        va_start(args, fmt);
        vsnprintf(buf, BUFSIZE - 1, fmt, args);
        va_end(args);
    }
    ircdproto.ircd_cmd_ctcp(source, dest, buf);
}



/**
 * Set routines for modules to set the prefered function for dealing with things.
 **/

void pmodule_set_mod_current_buffer(void (*func) (int ac, char **av))
{
    ircdproto.ircd_set_mod_current_buffer = func;
}

void pmodule_cmd_svsnoop(void (*func) (char *server, int set))
{
    ircdproto.ircd_cmd_svsnoop = func;
}

void pmodule_cmd_remove_akill(void (*func) (char *user, char *host))
{
    ircdproto.ircd_cmd_remove_akill = func;
}

void pmodule_cmd_topic(void (*func)
                        (char *whosets, char *chan, char *whosetit,
                         char *topic, time_t when))
{
    ircdproto.ircd_cmd_topic = func;
}

void pmodule_cmd_vhost_off(void (*func) (User * u))
{
    ircdproto.ircd_cmd_vhost_off = func;
}

void pmodule_cmd_akill(void (*func)
                        (char *user, char *host, char *who, time_t when,
                         time_t expires, char *reason))
{
    ircdproto.ircd_cmd_akill = func;
}

void
pmodule_cmd_svskill(void (*func) (char *source, char *user, char *buf))
{
    ircdproto.ircd_cmd_svskill = func;
}

void pmodule_cmd_svsmode(void (*func) (User * u, int ac, char **av))
{
    ircdproto.ircd_cmd_svsmode = func;
}

void pmodule_cmd_372(void (*func) (char *source, char *msg))
{
    ircdproto.ircd_cmd_372 = func;
}

void pmodule_cmd_372_error(void (*func) (char *source))
{
    ircdproto.ircd_cmd_372_error = func;
}

void pmodule_cmd_375(void (*func) (char *source))
{
    ircdproto.ircd_cmd_375 = func;
}

void pmodule_cmd_376(void (*func) (char *source))
{
    ircdproto.ircd_cmd_376 = func;
}

void pmodule_cmd_nick(void (*func) (char *nick, char *name, char *modes))
{
    ircdproto.ircd_cmd_nick = func;
}

void pmodule_cmd_guest_nick(void (*func)
                             (char *nick, char *user, char *host,
                              char *real, char *modes))
{
    ircdproto.ircd_cmd_guest_nick = func;
}

void pmodule_cmd_mode(void (*func) (char *source, char *dest, char *buf))
{
    ircdproto.ircd_cmd_mode = func;
}

void pmodule_cmd_bot_nick(void (*func)
                           (char *nick, char *user, char *host, char *real,
                            char *modes))
{
    ircdproto.ircd_cmd_bot_nick = func;
}

void pmodule_cmd_kick(void (*func)
                       (char *source, char *chan, char *user, char *buf))
{
    ircdproto.ircd_cmd_kick = func;
}

void
pmodule_cmd_notice_ops(void (*func) (char *source, char *dest, char *buf))
{
    ircdproto.ircd_cmd_notice_ops = func;
}

void pmodule_cmd_notice(void (*func) (char *source, char *dest, char *buf))
{
    ircdproto.ircd_cmd_notice = func;
}

void
pmodule_cmd_notice2(void (*func) (char *source, char *dest, char *msg))
{
    ircdproto.ircd_cmd_notice2 = func;
}

void
pmodule_cmd_privmsg(void (*func) (char *source, char *dest, char *buf))
{
    ircdproto.ircd_cmd_privmsg = func;
}

void
pmodule_cmd_privmsg2(void (*func) (char *source, char *dest, char *msg))
{
    ircdproto.ircd_cmd_privmsg2 = func;
}

void
pmodule_cmd_serv_notice(void (*func) (char *source, char *dest, char *msg))
{
    ircdproto.ircd_cmd_serv_notice = func;
}

void pmodule_cmd_serv_privmsg(void (*func)
                               (char *source, char *dest, char *msg))
{
    ircdproto.ircd_cmd_serv_privmsg = func;
}

void pmodule_cmd_bot_chan_mode(void (*func) (char *nick, char *chan))
{
    ircdproto.ircd_cmd_bot_chan_mode = func;
}

void pmodule_cmd_351(void (*func) (char *source))
{
    ircdproto.ircd_cmd_351 = func;
}

void pmodule_cmd_quit(void (*func) (char *source, char *buf))
{
    ircdproto.ircd_cmd_quit = func;
}

void pmodule_cmd_pong(void (*func) (char *servname, char *who))
{
    ircdproto.ircd_cmd_pong = func;
}

void
pmodule_cmd_join(void (*func) (char *user, char *channel, time_t chantime))
{
    ircdproto.ircd_cmd_join = func;
}

void pmodule_cmd_unsqline(void (*func) (char *user))
{
    ircdproto.ircd_cmd_unsqline = func;
}

void
pmodule_cmd_invite(void (*func) (char *source, char *chan, char *nick))
{
    ircdproto.ircd_cmd_invite = func;
}

void pmodule_cmd_part(void (*func) (char *nick, char *chan, char *buf))
{
    ircdproto.ircd_cmd_part = func;
}

void pmodule_cmd_391(void (*func) (char *source, char *timestr))
{
    ircdproto.ircd_cmd_391 = func;
}

void pmodule_cmd_250(void (*func) (char *buf))
{
    ircdproto.ircd_cmd_250 = func;
}

void pmodule_cmd_307(void (*func) (char *buf))
{
    ircdproto.ircd_cmd_307 = func;
}

void pmodule_cmd_311(void (*func) (char *buf))
{
    ircdproto.ircd_cmd_311 = func;
}

void pmodule_cmd_312(void (*func) (char *buf))
{
    ircdproto.ircd_cmd_312 = func;
}

void pmodule_cmd_317(void (*func) (char *buf))
{
    ircdproto.ircd_cmd_317 = func;
}

void pmodule_cmd_219(void (*func) (char *source, char *letter))
{
    ircdproto.ircd_cmd_219 = func;
}

void pmodule_cmd_401(void (*func) (char *source, char *who))
{
    ircdproto.ircd_cmd_401 = func;
}

void pmodule_cmd_318(void (*func) (char *source, char *who))
{
    ircdproto.ircd_cmd_318 = func;
}

void pmodule_cmd_242(void (*func) (char *buf))
{
    ircdproto.ircd_cmd_242 = func;
}

void pmodule_cmd_243(void (*func) (char *buf))
{
    ircdproto.ircd_cmd_243 = func;
}

void pmodule_cmd_211(void (*func) (char *buf))
{
    ircdproto.ircd_cmd_211 = func;
}

void pmodule_cmd_global(void (*func) (char *source, char *buf))
{
    ircdproto.ircd_cmd_global = func;
}

void pmodule_cmd_global_legacy(void (*func) (char *source, char *fmt))
{
    ircdproto.ircd_cmd_global_legacy = func;
}

void pmodule_cmd_sqline(void (*func) (char *mask, char *reason))
{
    ircdproto.ircd_cmd_sqline = func;
}

void pmodule_cmd_squit(void (*func) (char *servname, char *message))
{
    ircdproto.ircd_cmd_squit = func;
}

void pmodule_cmd_svso(void (*func) (char *source, char *nick, char *flag))
{
    ircdproto.ircd_cmd_svso = func;
}

void pmodule_cmd_chg_nick(void (*func) (char *oldnick, char *newnick))
{
    ircdproto.ircd_cmd_chg_nick = func;
}

void
pmodule_cmd_svsnick(void (*func) (char *source, char *guest, time_t when))
{
    ircdproto.ircd_cmd_svsnick = func;
}

void
pmodule_cmd_vhost_on(void (*func) (char *nick, char *vIdent, char *vhost))
{
    ircdproto.ircd_cmd_vhost_on = func;
}

void pmodule_cmd_connect(void (*func) (int servernum))
{
    ircdproto.ircd_cmd_connect = func;
}

void pmodule_cmd_bob(void (*func) ())
{
    ircdproto.ircd_cmd_bob = func;
}

void pmodule_cmd_svshold(void (*func) (char *nick))
{
    ircdproto.ircd_cmd_svshold = func;
}

void pmodule_cmd_release_svshold(void (*func) (char *nick))
{
    ircdproto.ircd_cmd_release_svshold = func;
}

void pmodule_cmd_unsgline(void (*func) (char *mask))
{
    ircdproto.ircd_cmd_unsgline = func;
}

void pmodule_cmd_unszline(void (*func) (char *mask))
{
    ircdproto.ircd_cmd_unszline = func;
}

void
pmodule_cmd_szline(void (*func) (char *mask, char *reason, char *whom))
{
    ircdproto.ircd_cmd_szline = func;
}

void pmodule_cmd_sgline(void (*func) (char *mask, char *reason))
{
    ircdproto.ircd_cmd_sgline = func;
}

void pmodule_cmd_unban(void (*func) (char *name, char *nick))
{
    ircdproto.ircd_cmd_unban = func;
}

void
pmodule_cmd_svsmode_chan(void (*func) (char *name, char *mode, char *nick))
{
    ircdproto.ircd_cmd_svsmode_chan = func;
}

void pmodule_cmd_svid_umode(void (*func) (char *nick, time_t ts))
{
    ircdproto.ircd_cmd_svid_umode = func;
}

void pmodule_cmd_nc_change(void (*func) (User * u))
{
    ircdproto.ircd_cmd_nc_change = func;
}

void pmodule_cmd_svid_umode2(void (*func) (User * u, char *ts))
{
    ircdproto.ircd_cmd_svid_umode2 = func;
}

void pmodule_cmd_svid_umode3(void (*func) (User * u, char *ts))
{
    ircdproto.ircd_cmd_svid_umode3 = func;
}

void pmodule_cmd_ctcp(void (*func) (char *source, char *dest, char *buf))
{
    ircdproto.ircd_cmd_ctcp = func;
}

void pmodule_cmd_svsjoin(void (*func)
                          (char *source, char *nick, char *chan,
                           char *param))
{
    ircdproto.ircd_cmd_svsjoin = func;
}

void
pmodule_cmd_svspart(void (*func) (char *source, char *nick, char *chan))
{
    ircdproto.ircd_cmd_svspart = func;
}

void pmodule_cmd_swhois(void (*func) (char *source, char *who, char *mask))
{
    ircdproto.ircd_cmd_swhois = func;
}

void pmodule_cmd_eob(void (*func) ())
{
    ircdproto.ircd_cmd_eob = func;
}

void
pmodule_cmd_jupe(void (*func) (char *jserver, char *who, char *reason))
{
    ircdproto.ircd_cmd_jupe = func;
}

void pmodule_set_umode(void (*func) (User * user, int ac, char **av))
{
    ircdproto.ircd_set_umode = func;
}

void pmodule_valid_nick(int (*func) (char *nick))
{
    ircdproto.ircd_valid_nick = func;
}

void pmodule_valid_chan(int (*func) (char *chan))
{
    ircdproto.ircd_valid_chan = func;
}

void pmodule_flood_mode_check(int (*func) (char *value))
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

void pmodule_ircd_version(char *version)
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

void pmodule_ircd_flood_mode_char_set(char *mode)
{
    flood_mode_char_set = sstrdup(mode);
}

void pmodule_ircd_flood_mode_char_remove(char *mode)
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

void pmodule_jointhrottle_mode_check(int (*func) (char *value))
{
    ircdproto.ircd_jointhrottle_mode_check = func;
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

void pmodule_permchan_mode(int mode)
{
    ircd_modes.chan_perm = mode;
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

int anope_get_permchan_mode()
{
    return ircd_modes.chan_perm;
}

