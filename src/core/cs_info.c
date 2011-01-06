/* ChanServ core functions
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

int do_info(User * u);
void myChanServHelp(User * u);

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
    moduleAddVersion(VERSION_STRING);
    moduleSetType(CORE);

    c = createCommand("INFO", do_info, NULL, CHAN_HELP_INFO, -1,
                      CHAN_SERVADMIN_HELP_INFO, CHAN_SERVADMIN_HELP_INFO,
                      CHAN_SERVADMIN_HELP_INFO);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);

    moduleSetChanHelp(myChanServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_INFO);
}

/**
 * The /cs info command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_info(User * u)
{
/* SADMINS and users, who have identified for a channel, can now cause it's
 * Enstry Message and Successor to be displayed by supplying the ALL parameter.
 * Syntax: INFO channel [ALL]
 * -TheShadow (29 Mar 1999)
 */
    char *chan = strtok(NULL, " ");
    char *param = strtok(NULL, " ");
    ChannelInfo *ci;
    char buf[BUFSIZE], *end;
    struct tm *tm;
    int need_comma = 0;
    const char *commastr = getstring(u->na, COMMA_SPACE);
    int is_servadmin = is_services_admin(u);
    int show_all = 0;
    time_t expt;

    if (!chan) {
        syntax_error(s_ChanServ, u, "INFO", CHAN_INFO_SYNTAX);
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        if (is_oper(u) && ci->forbidby)
            notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN_OPER, chan,
                        ci->forbidby,
                        (ci->forbidreason ? ci->
                         forbidreason : getstring(u->na, NO_REASON)));
        else
            notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!ci->founder) {
        /* Paranoia... this shouldn't be able to happen */
        delchan(ci);
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else {

        /* Should we show all fields? Only for sadmins and identified users */
        if (param && stricmp(param, "ALL") == 0 &&
            (check_access(u, ci, CA_INFO) || is_servadmin))
            show_all = 1;

        notice_lang(s_ChanServ, u, CHAN_INFO_HEADER, chan);
        notice_lang(s_ChanServ, u, CHAN_INFO_NO_FOUNDER,
                    ci->founder->display);

        if (show_all && ci->successor)
            notice_lang(s_ChanServ, u, CHAN_INFO_NO_SUCCESSOR,
                        ci->successor->display);

        notice_lang(s_ChanServ, u, CHAN_INFO_DESCRIPTION, ci->desc);
        tm = localtime(&ci->time_registered);
        strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
        notice_lang(s_ChanServ, u, CHAN_INFO_TIME_REGGED, buf);
        tm = localtime(&ci->last_used);
        strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
        notice_lang(s_ChanServ, u, CHAN_INFO_LAST_USED, buf);
        if (ci->last_topic
            && (show_all || (!(ci->mlock_on & anope_get_secret_mode())
                             && (!ci->c
                                 || !(ci->c->
                                      mode & anope_get_secret_mode()))))) {
            notice_lang(s_ChanServ, u, CHAN_INFO_LAST_TOPIC,
                        ci->last_topic);
            notice_lang(s_ChanServ, u, CHAN_INFO_TOPIC_SET_BY,
                        ci->last_topic_setter);
        }

        if (ci->entry_message && show_all)
            notice_lang(s_ChanServ, u, CHAN_INFO_ENTRYMSG,
                        ci->entry_message);
        if (ci->url)
            notice_lang(s_ChanServ, u, CHAN_INFO_URL, ci->url);
        if (ci->email)
            notice_lang(s_ChanServ, u, CHAN_INFO_EMAIL, ci->email);

        if (show_all) {
            notice_lang(s_ChanServ, u, CHAN_INFO_BANTYPE, ci->bantype);

            end = buf;
            *end = 0;
            if (ci->flags & CI_KEEPTOPIC) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s",
                                getstring(u->na, CHAN_INFO_OPT_KEEPTOPIC));
                need_comma = 1;
            }
            if (ci->flags & CI_OPNOTICE) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, CHAN_INFO_OPT_OPNOTICE));
                need_comma = 1;
            }
            if (ci->flags & CI_PEACE) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, CHAN_INFO_OPT_PEACE));
                need_comma = 1;
            }
            if (ci->flags & CI_PRIVATE) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, CHAN_INFO_OPT_PRIVATE));
                need_comma = 1;
            }
            if (ci->flags & CI_RESTRICTED) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na,
                                          CHAN_INFO_OPT_RESTRICTED));
                need_comma = 1;
            }
            if (ci->flags & CI_SECURE) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, CHAN_INFO_OPT_SECURE));
                need_comma = 1;
            }
            if (ci->flags & CI_SECUREOPS) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, CHAN_INFO_OPT_SECUREOPS));
                need_comma = 1;
            }
            if (ci->flags & CI_SECUREFOUNDER) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na,
                                          CHAN_INFO_OPT_SECUREFOUNDER));
                need_comma = 1;
            }
            if ((ci->flags & CI_SIGNKICK)
                || (ci->flags & CI_SIGNKICK_LEVEL)) {
                end +=
                    snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                             need_comma ? commastr : "", getstring(u->na,
                                                                   CHAN_INFO_OPT_SIGNKICK));
                need_comma = 1;
            }
            if (ci->flags & CI_TOPICLOCK) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, CHAN_INFO_OPT_TOPICLOCK));
                need_comma = 1;
            }
            if (ci->flags & CI_XOP) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, CHAN_INFO_OPT_XOP));
                need_comma = 1;
            }
            notice_lang(s_ChanServ, u, CHAN_INFO_OPTIONS,
                        *buf ? buf : getstring(u->na, CHAN_INFO_OPT_NONE));
            notice_lang(s_ChanServ, u, CHAN_INFO_MODE_LOCK,
                        get_mlock_modes(ci, 1));

        }
        if (show_all) {
            if (ci->flags & CI_NO_EXPIRE) {
                notice_lang(s_ChanServ, u, CHAN_INFO_NO_EXPIRE);
            } else {
                if (is_servadmin) {
                    expt = ci->last_used + CSExpire;
                    tm = localtime(&expt);
                    strftime_lang(buf, sizeof(buf), u,
                                  STRFTIME_DATE_TIME_FORMAT, tm);
                    notice_lang(s_ChanServ, u, CHAN_INFO_EXPIRE, buf);
                }
            }
        }
        if (ci->flags & CI_SUSPENDED) {
            notice_lang(s_ChanServ, u, CHAN_X_SUSPENDED, ci->forbidby,
                        (ci->forbidreason ? ci->
                         forbidreason : getstring(u->na, NO_REASON)));
        }

        if (!show_all && (check_access(u, ci, CA_INFO) || is_servadmin))
            notice_lang(s_ChanServ, u, NICK_INFO_FOR_MORE, s_ChanServ,
                        ci->name);

    }
    return MOD_CONT;
}
