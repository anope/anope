/* BotServ core functions
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
void send_bot_channels(User * u, BotInfo * bi);
void myBotServHelp(User * u);

/**
 * Create the info command, and tell anope about it.
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
    c = createCommand("INFO", do_info, NULL, BOT_HELP_INFO, -1, -1, -1,
                      -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);

    moduleSetBotHelp(myBotServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to Anopes /bs help output.
 * @param u The user who is requesting help
 **/
void myBotServHelp(User * u)
{
    notice_lang(s_BotServ, u, BOT_HELP_CMD_INFO);
}

/**
 * The /bs info command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_info(User * u)
{
    BotInfo *bi;
    ChannelInfo *ci;
    char *query = strtok(NULL, " ");

    int need_comma = 0, is_servadmin = is_services_admin(u);
    char buf[BUFSIZE], *end;
    const char *commastr = getstring(u->na, COMMA_SPACE);

    if (!query)
        syntax_error(s_BotServ, u, "INFO", BOT_INFO_SYNTAX);
    else if ((bi = findbot(query))) {
        char buf[BUFSIZE];
        struct tm *tm;

        notice_lang(s_BotServ, u, BOT_INFO_BOT_HEADER, bi->nick);
        notice_lang(s_BotServ, u, BOT_INFO_BOT_MASK, bi->user, bi->host);
        notice_lang(s_BotServ, u, BOT_INFO_BOT_REALNAME, bi->real);
        tm = localtime(&bi->created);
        strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
        notice_lang(s_BotServ, u, BOT_INFO_BOT_CREATED, buf);
        notice_lang(s_BotServ, u, BOT_INFO_BOT_OPTIONS,
                    getstring(u->na,
                              (bi->
                               flags & BI_PRIVATE) ? BOT_INFO_OPT_PRIVATE :
                              BOT_INFO_OPT_NONE));
        notice_lang(s_BotServ, u, BOT_INFO_BOT_USAGE, bi->chancount);

        if (is_services_admin(u))
            send_bot_channels(u, bi);
    } else if ((ci = cs_findchan(query))) {
        if (!is_servadmin && !is_founder(u, ci)) {
            notice_lang(s_BotServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }
        if (ci->flags & CI_VERBOTEN) {
            notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, query);
            return MOD_CONT;
        }

        notice_lang(s_BotServ, u, BOT_INFO_CHAN_HEADER, ci->name);
        if (ci->bi)
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_BOT, ci->bi->nick);
        else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_BOT_NONE);

        if (ci->botflags & BS_KICK_BADWORDS) {
            if (ci->ttb[TTB_BADWORDS])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BADWORDS_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_BADWORDS]);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BADWORDS,
                            getstring(u->na, BOT_INFO_ACTIVE));
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BADWORDS,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_BOLDS) {
            if (ci->ttb[TTB_BOLDS])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BOLDS_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_BOLDS]);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BOLDS,
                            getstring(u->na, BOT_INFO_ACTIVE));
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_BOLDS,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_CAPS) {
            if (ci->ttb[TTB_CAPS])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_CAPS_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_CAPS], ci->capsmin,
                            ci->capspercent);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_CAPS_ON,
                            getstring(u->na, BOT_INFO_ACTIVE), ci->capsmin,
                            ci->capspercent);
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_CAPS_OFF,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_COLORS) {
            if (ci->ttb[TTB_COLORS])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_COLORS_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_COLORS]);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_COLORS,
                            getstring(u->na, BOT_INFO_ACTIVE));
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_COLORS,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_FLOOD) {
            if (ci->ttb[TTB_FLOOD])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_FLOOD_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_FLOOD], ci->floodlines,
                            ci->floodsecs);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_FLOOD_ON,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->floodlines, ci->floodsecs);
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_FLOOD_OFF,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_REPEAT) {
            if (ci->ttb[TTB_REPEAT])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REPEAT_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_REPEAT], ci->repeattimes);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REPEAT_ON,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->repeattimes);
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REPEAT_OFF,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_REVERSES) {
            if (ci->ttb[TTB_REVERSES])
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REVERSES_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_REVERSES]);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REVERSES,
                            getstring(u->na, BOT_INFO_ACTIVE));
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_REVERSES,
                        getstring(u->na, BOT_INFO_INACTIVE));
        if (ci->botflags & BS_KICK_UNDERLINES) {
            if (ci->ttb[TTB_UNDERLINES])
                notice_lang(s_BotServ, u,
                            BOT_INFO_CHAN_KICK_UNDERLINES_BAN,
                            getstring(u->na, BOT_INFO_ACTIVE),
                            ci->ttb[TTB_UNDERLINES]);
            else
                notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_UNDERLINES,
                            getstring(u->na, BOT_INFO_ACTIVE));
        } else
            notice_lang(s_BotServ, u, BOT_INFO_CHAN_KICK_UNDERLINES,
                        getstring(u->na, BOT_INFO_INACTIVE));

        end = buf;
        *end = 0;
        if (ci->botflags & BS_DONTKICKOPS) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%s",
                            getstring(u->na, BOT_INFO_OPT_DONTKICKOPS));
            need_comma = 1;
        }
        if (ci->botflags & BS_DONTKICKVOICES) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->na, BOT_INFO_OPT_DONTKICKVOICES));
            need_comma = 1;
        }
        if (ci->botflags & BS_FANTASY) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->na, BOT_INFO_OPT_FANTASY));
            need_comma = 1;
        }
        if (ci->botflags & BS_GREET) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->na, BOT_INFO_OPT_GREET));
            need_comma = 1;
        }
        if (ci->botflags & BS_NOBOT) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->na, BOT_INFO_OPT_NOBOT));
            need_comma = 1;
        }
        if (ci->botflags & BS_SYMBIOSIS) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->na, BOT_INFO_OPT_SYMBIOSIS));
            need_comma = 1;
        }
        notice_lang(s_BotServ, u, BOT_INFO_CHAN_OPTIONS,
                    *buf ? buf : getstring(u->na, BOT_INFO_OPT_NONE));

    } else
        notice_lang(s_BotServ, u, BOT_INFO_NOT_FOUND, query);
    return MOD_CONT;
}

void send_bot_channels(User * u, BotInfo * bi)
{
    int i;
    ChannelInfo *ci;
    char buf[307], *end;

    *buf = 0;
    end = buf;

    for (i = 0; i < 256; i++) {
        for (ci = chanlists[i]; ci; ci = ci->next) {
            if (ci->bi == bi) {
                if (strlen(buf) + strlen(ci->name) > 300) {
                    notice_user(s_BotServ, u, "%s", buf);
                    *buf = 0;
                    end = buf;
                }
                end +=
                    snprintf(end, sizeof(buf) - (end - buf), " %s ",
                             ci->name);
            }
        }
    }

    if (*buf)
        notice_user(s_BotServ, u, "%s", buf);
    return;
}
