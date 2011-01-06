/* NickServ core functions
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
/*************************************************************************/

#include "module.h"

static int do_info(User * u);
void myNickServHelp(User * u);

/**
 * Create the command, and tell anope about it.
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT to allow the module, MOD_STOP to stop it
 **/
int AnopeInit(int argc, char **argv)
{
    Command *c;

    moduleAddAuthor("Anope");
    moduleAddVersion
        (VERSION_STRING);
    moduleSetType(CORE);

    c = createCommand("INFO", do_info, NULL, NICK_HELP_INFO, -1,
                      NICK_HELP_INFO, NICK_SERVADMIN_HELP_INFO,
                      NICK_SERVADMIN_HELP_INFO);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

    moduleSetNickHelp(myNickServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User * u)
{
    notice_lang(s_NickServ, u, NICK_HELP_CMD_INFO);
}

/**
 * The /ns info command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_info(User * u)
{

/* Show hidden info to nick owners and sadmins when the "ALL" parameter is
 * supplied. If a nick is online, the "Last seen address" changes to "Is
 * online from".
 * Syntax: INFO <nick> {ALL}
 * -TheShadow (13 Mar 1999)
 */

    char *nick = strtok(NULL, " ");
    char *param = strtok(NULL, " ");

    NickAlias *na;
    NickRequest *nr = NULL;
	/* Being an oper is enough from now on -GD */
    int is_servadmin = is_services_oper(u);

    if (!nick) {
        syntax_error(s_NickServ, u, "INFO", NICK_INFO_SYNTAX);
    } else if (!(na = findnick(nick))) {
        if ((nr = findrequestnick(nick))) {
            notice_lang(s_NickServ, u, NICK_IS_PREREG);
            if (param && stricmp(param, "ALL") == 0 && is_servadmin) {
                notice_lang(s_NickServ, u, NICK_INFO_EMAIL, nr->email);
            } else {
                if (is_servadmin) {
                    notice_lang(s_NickServ, u, NICK_INFO_FOR_MORE,
                                s_NickServ, nr->nick);
                }
            }
        } else if (nickIsServices(nick, 1)) {
            notice_lang(s_NickServ, u, NICK_X_IS_SERVICES, nick);
        } else {
            notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
        }
    } else if (na->status & NS_VERBOTEN) {
        if (is_oper(u) && na->last_usermask)
            notice_lang(s_NickServ, u, NICK_X_FORBIDDEN_OPER, nick,
                        na->last_usermask,
                        (na->last_realname ? na->
                         last_realname : getstring(u->na, NO_REASON)));
        else
            notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
    } else {
        struct tm *tm;
        char buf[BUFSIZE], *end;
        const char *commastr = getstring(u->na, COMMA_SPACE);
        int need_comma = 0;
        int nick_online = 0;
        int show_hidden = 0;
        time_t expt;

        /* Is the real owner of the nick we're looking up online? -TheShadow */
        if (na->status & (NS_RECOGNIZED | NS_IDENTIFIED))
            nick_online = 1;

        /* Only show hidden fields to owner and sadmins and only when the ALL
         * parameter is used. -TheShadow */
        if (param && stricmp(param, "ALL") == 0 && u->na
            && ((nick_identified(u) && (na->nc == u->na->nc))
                || is_servadmin))
            show_hidden = 1;

        notice_lang(s_NickServ, u, NICK_INFO_REALNAME, na->nick,
                    na->last_realname);

        if ((nick_identified(u) && (na->nc == u->na->nc)) || is_servadmin) {

            if (nick_is_services_root(na->nc))
                notice_lang(s_NickServ, u, NICK_INFO_SERVICES_ROOT,
                            na->nick);
            else if (nick_is_services_admin(na->nc))
                notice_lang(s_NickServ, u, NICK_INFO_SERVICES_ADMIN,
                            na->nick);
            else if (nick_is_services_oper(na->nc))
                notice_lang(s_NickServ, u, NICK_INFO_SERVICES_OPER,
                            na->nick);

        } else {

            if (nick_is_services_root(na->nc)
                && !(na->nc->flags & NI_HIDE_STATUS))
                notice_lang(s_NickServ, u, NICK_INFO_SERVICES_ROOT,
                            na->nick);
            else if (nick_is_services_admin(na->nc)
                     && !(na->nc->flags & NI_HIDE_STATUS))
                notice_lang(s_NickServ, u, NICK_INFO_SERVICES_ADMIN,
                            na->nick);
            else if (nick_is_services_oper(na->nc)
                     && !(na->nc->flags & NI_HIDE_STATUS))
                notice_lang(s_NickServ, u, NICK_INFO_SERVICES_OPER,
                            na->nick);

        }

        if (nick_online) {
            if (show_hidden || !(na->nc->flags & NI_HIDE_MASK))
                notice_lang(s_NickServ, u, NICK_INFO_ADDRESS_ONLINE,
                            na->last_usermask);
            else
                notice_lang(s_NickServ, u, NICK_INFO_ADDRESS_ONLINE_NOHOST,
                            na->nick);
        } else {
            if (show_hidden || !(na->nc->flags & NI_HIDE_MASK))
                notice_lang(s_NickServ, u, NICK_INFO_ADDRESS,
                            na->last_usermask);
        }

        tm = localtime(&na->time_registered);
        strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
        notice_lang(s_NickServ, u, NICK_INFO_TIME_REGGED, buf);

        if (!nick_online) {
            tm = localtime(&na->last_seen);
            strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT,
                          tm);
            notice_lang(s_NickServ, u, NICK_INFO_LAST_SEEN, buf);
        }

        if (na->last_quit
            && (show_hidden || !(na->nc->flags & NI_HIDE_QUIT)))
            notice_lang(s_NickServ, u, NICK_INFO_LAST_QUIT, na->last_quit);

        if (na->nc->url)
            notice_lang(s_NickServ, u, NICK_INFO_URL, na->nc->url);
        if (na->nc->email
            && (show_hidden || !(na->nc->flags & NI_HIDE_EMAIL)))
            notice_lang(s_NickServ, u, NICK_INFO_EMAIL, na->nc->email);
        if (na->nc->icq)
            notice_lang(s_NickServ, u, NICK_INFO_ICQ, na->nc->icq);

        if (show_hidden) {
            if (s_HostServ && ircd->vhost) {
                if (getvHost(na->nick) != NULL) {
                    if (ircd->vident && getvIdent(na->nick) != NULL) {
                        notice_lang(s_NickServ, u, NICK_INFO_VHOST2,
                                    getvIdent(na->nick),
                                    getvHost(na->nick));
                    } else {
                        notice_lang(s_NickServ, u, NICK_INFO_VHOST,
                                    getvHost(na->nick));
                    }
                }
            }
            if (na->nc->greet)
                notice_lang(s_NickServ, u, NICK_INFO_GREET, na->nc->greet);

            *buf = 0;
            end = buf;

            if (na->nc->flags & NI_KILLPROTECT) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s",
                                getstring(u->na, NICK_INFO_OPT_KILL));
                need_comma = 1;
            }
            if (na->nc->flags & NI_SECURE) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, NICK_INFO_OPT_SECURE));
                need_comma = 1;
            }
            if (na->nc->flags & NI_PRIVATE) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, NICK_INFO_OPT_PRIVATE));
                need_comma = 1;
            }
            if (na->nc->flags & NI_MSG) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, NICK_INFO_OPT_MSG));
                need_comma = 1;
            }
            if (!(na->nc->flags & NI_AUTOOP)) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, NICK_INFO_OPT_AUTOOP));
                need_comma = 1;
            }

            notice_lang(s_NickServ, u, NICK_INFO_OPTIONS,
                        *buf ? buf : getstring(u->na, NICK_INFO_OPT_NONE));

            if (na->nc->flags & NI_SUSPENDED) {
                if (na->last_quit) {
                    notice_lang(s_NickServ, u, NICK_INFO_SUSPENDED,
                                na->last_quit);
                } else {
                    notice_lang(s_NickServ, u,
                                NICK_INFO_SUSPENDED_NO_REASON);
                }
            }

            if (na->status & NS_NO_EXPIRE) {
                notice_lang(s_NickServ, u, NICK_INFO_NO_EXPIRE);
            } else {
                if (is_servadmin) {
                    expt = na->last_seen + NSExpire;
                    tm = localtime(&expt);
                    strftime_lang(buf, sizeof(buf), na->u,
                                  STRFTIME_DATE_TIME_FORMAT, tm);
                    notice_lang(s_NickServ, u, NICK_INFO_EXPIRE, buf);
                }
            }
        }

        if (!show_hidden
            && ((u->na && (na->nc == u->na->nc) && nick_identified(u))
                || is_servadmin))
            notice_lang(s_NickServ, u, NICK_INFO_FOR_MORE, s_NickServ,
                        na->nick);
    }
    return MOD_CONT;
}
