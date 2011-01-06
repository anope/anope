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

int do_kickcmd(User * u);

void myBotServHelp(User * u);

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
    c = createCommand("KICK", do_kickcmd, NULL, BOT_HELP_KICK, -1, -1, -1,
                      -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("KICK BADWORDS", NULL, NULL, BOT_HELP_KICK_BADWORDS,
                      -1, -1, -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("KICK BOLDS", NULL, NULL, BOT_HELP_KICK_BOLDS, -1,
                      -1, -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("KICK CAPS", NULL, NULL, BOT_HELP_KICK_CAPS, -1, -1,
                      -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("KICK COLORS", NULL, NULL, BOT_HELP_KICK_COLORS, -1,
                      -1, -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("KICK FLOOD", NULL, NULL, BOT_HELP_KICK_FLOOD, -1,
                      -1, -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("KICK REPEAT", NULL, NULL, BOT_HELP_KICK_REPEAT, -1,
                      -1, -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("KICK REVERSES", NULL, NULL, BOT_HELP_KICK_REVERSES,
                      -1, -1, -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("KICK UNDERLINES", NULL, NULL,
                      BOT_HELP_KICK_UNDERLINES, -1, -1, -1, -1);
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
    notice_lang(s_BotServ, u, BOT_HELP_CMD_KICK);
}

/**
 * The /bs kick command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_kickcmd(User * u)
{
    char *chan = strtok(NULL, " ");
    char *option = strtok(NULL, " ");
    char *value = strtok(NULL, " ");
    char *ttb = strtok(NULL, " ");

    ChannelInfo *ci;

    if (readonly)
        notice_lang(s_BotServ, u, BOT_KICK_DISABLED);
    else if (!chan || !option || !value)
        syntax_error(s_BotServ, u, "KICK", BOT_KICK_SYNTAX);
    else if (stricmp(value, "ON") && stricmp(value, "OFF"))
        syntax_error(s_BotServ, u, "KICK", BOT_KICK_SYNTAX);
    else if (!(ci = cs_findchan(chan)))
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    else if (ci->flags & CI_VERBOTEN)
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    else if (!is_services_admin(u) && !check_access(u, ci, CA_SET))
        notice_lang(s_BotServ, u, ACCESS_DENIED);
    else if (!ci->bi)
        notice_help(s_BotServ, u, BOT_NOT_ASSIGNED);
    else {
        if (!stricmp(option, "BADWORDS")) {
            if (!stricmp(value, "ON")) {
                if (ttb) {
                    errno = 0;
                    ci->ttb[TTB_BADWORDS] =
                        strtol(ttb, (char **) NULL, 10);
                    /* Only error if errno returns ERANGE or EINVAL or we are less then 0 - TSL */
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_BADWORDS] < 0) {
                        /* leaving the debug behind since we might want to know what these are */
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_BADWORDS]);
                        }
                        /* reset the value back to 0 - TSL */
                        ci->ttb[TTB_BADWORDS] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else {
                    ci->ttb[TTB_BADWORDS] = 0;
                }
                ci->botflags |= BS_KICK_BADWORDS;
                if (ci->ttb[TTB_BADWORDS]) {
                    alog("%s: %s!%s@%s enabled kicking for badwords on %s with time to ban of %d",
                         s_BotServ, u->nick, u->username, u->host, ci->name, ci->ttb[TTB_BADWORDS]);
                    notice_lang(s_BotServ, u, BOT_KICK_BADWORDS_ON_BAN,
                                ci->ttb[TTB_BADWORDS]);
		} else {
                    alog("%s: %s!%s@%s enabled kicking for badwords on %s",
                         s_BotServ, u->nick, u->username, u->host, ci->name);
                    notice_lang(s_BotServ, u, BOT_KICK_BADWORDS_ON);
		}
            } else {
                ci->botflags &= ~BS_KICK_BADWORDS;
                alog("%s: %s!%s@%s disabled kicking for badwords on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_KICK_BADWORDS_OFF);
            }
        } else if (!stricmp(option, "BOLDS")) {
            if (!stricmp(value, "ON")) {
                if (ttb) {
                    errno = 0;
                    ci->ttb[TTB_BOLDS] = strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_BOLDS] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_BOLDS]);
                        }
                        ci->ttb[TTB_BOLDS] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_BOLDS] = 0;
                ci->botflags |= BS_KICK_BOLDS;
                if (ci->ttb[TTB_BOLDS]) {
                    alog("%s: %s!%s@%s enabled kicking for bolds on %s with time to ban of %d",
                         s_BotServ, u->nick, u->username, u->host, ci->name, ci->ttb[TTB_BOLDS]);
                    notice_lang(s_BotServ, u, BOT_KICK_BOLDS_ON_BAN,
                                ci->ttb[TTB_BOLDS]);
                } else {
                    alog("%s: %s!%s@%s enabled kicking for bolds on %s",
                         s_BotServ, u->nick, u->username, u->host, ci->name);
                    notice_lang(s_BotServ, u, BOT_KICK_BOLDS_ON);
                }
            } else {
                ci->botflags &= ~BS_KICK_BOLDS;
                alog("%s: %s!%s@%s disabled kicking for bolds on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_KICK_BOLDS_OFF);
            }
        } else if (!stricmp(option, "CAPS")) {
            if (!stricmp(value, "ON")) {
                char *min = strtok(NULL, " ");
                char *percent = strtok(NULL, " ");

                if (ttb) {
                    errno = 0;
                    ci->ttb[TTB_CAPS] = strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_CAPS] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_CAPS]);
                        }
                        ci->ttb[TTB_CAPS] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_CAPS] = 0;

                if (!min)
                    ci->capsmin = 10;
                else
                    ci->capsmin = atol(min);
                if (ci->capsmin < 1)
                    ci->capsmin = 10;

                if (!percent)
                    ci->capspercent = 25;
                else
                    ci->capspercent = atol(percent);
                if (ci->capspercent < 1 || ci->capspercent > 100)
                    ci->capspercent = 25;

                ci->botflags |= BS_KICK_CAPS;
                if (ci->ttb[TTB_CAPS]) {
                    alog("%s: %s!%s@%s enabled kicking for caps on %s with time to ban of %d",
                         s_BotServ, u->nick, u->username, u->host, ci->name, ci->ttb[TTB_CAPS]);
                    notice_lang(s_BotServ, u, BOT_KICK_CAPS_ON_BAN,
                                ci->capsmin, ci->capspercent,
                                ci->ttb[TTB_CAPS]);
                } else {
                    alog("%s: %s!%s@%s enabled kicking for caps on %s",
                         s_BotServ, u->nick, u->username, u->host, ci->name);
                    notice_lang(s_BotServ, u, BOT_KICK_CAPS_ON,
                                ci->capsmin, ci->capspercent);
                }
            } else {
                alog("%s: %s!%s@%s disabled kicking for caps on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                ci->botflags &= ~BS_KICK_CAPS;
                notice_lang(s_BotServ, u, BOT_KICK_CAPS_OFF);
            }
        } else if (!stricmp(option, "COLORS")) {
            if (!stricmp(value, "ON")) {
                if (ttb) {
                    errno = 0;
                    ci->ttb[TTB_COLORS] = strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_COLORS] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_COLORS]);
                        }
                        ci->ttb[TTB_COLORS] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_COLORS] = 0;
                ci->botflags |= BS_KICK_COLORS;
                if (ci->ttb[TTB_COLORS]) {
                    alog("%s: %s!%s@%s enabled kicking for colors on %s with time to ban of %d",
                         s_BotServ, u->nick, u->username, u->host, ci->name, ci->ttb[TTB_COLORS]);
                    notice_lang(s_BotServ, u, BOT_KICK_COLORS_ON_BAN,
                                ci->ttb[TTB_COLORS]);
                } else {
                    alog("%s: %s!%s@%s enabled kicking for colors on %s",
                         s_BotServ, u->nick, u->username, u->host, ci->name);
                    notice_lang(s_BotServ, u, BOT_KICK_COLORS_ON);
                }
            } else {
                alog("%s: %s!%s@%s disabled kicking for colors on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                ci->botflags &= ~BS_KICK_COLORS;
                notice_lang(s_BotServ, u, BOT_KICK_COLORS_OFF);
            }
        } else if (!stricmp(option, "FLOOD")) {
            if (!stricmp(value, "ON")) {
                char *lines = strtok(NULL, " ");
                char *secs = strtok(NULL, " ");

                if (ttb) {
                    errno = 0;
                    ci->ttb[TTB_FLOOD] = strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_FLOOD] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_FLOOD]);
                        }
                        ci->ttb[TTB_FLOOD] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_FLOOD] = 0;

                if (!lines)
                    ci->floodlines = 6;
                else
                    ci->floodlines = atol(lines);
                if (ci->floodlines < 2)
                    ci->floodlines = 6;

                if (!secs)
                    ci->floodsecs = 10;
                else
                    ci->floodsecs = atol(secs);
                if (ci->floodsecs < 1 || ci->floodsecs > BSKeepData)
                    ci->floodsecs = 10;

                ci->botflags |= BS_KICK_FLOOD;
                if (ci->ttb[TTB_FLOOD]) {
                    alog("%s: %s!%s@%s enabled kicking for flood on %s with time to ban of %d",
                         s_BotServ, u->nick, u->username, u->host, ci->name, ci->ttb[TTB_FLOOD]);
                    notice_lang(s_BotServ, u, BOT_KICK_FLOOD_ON_BAN,
                                ci->floodlines, ci->floodsecs,
                                ci->ttb[TTB_FLOOD]);
                } else {
                    alog("%s: %s!%s@%s enabled kicking for flood on %s",
                         s_BotServ, u->nick, u->username, u->host, ci->name);
                    notice_lang(s_BotServ, u, BOT_KICK_FLOOD_ON,
                                ci->floodlines, ci->floodsecs);
                }
            } else {
                ci->botflags &= ~BS_KICK_FLOOD;
                alog("%s: %s!%s@%s disabled kicking for flood on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_KICK_FLOOD_OFF);
            }
        } else if (!stricmp(option, "REPEAT")) {
            if (!stricmp(value, "ON")) {
                char *times = strtok(NULL, " ");

                if (ttb) {
                    errno = 0;
                    ci->ttb[TTB_REPEAT] = strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_REPEAT] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_REPEAT]);
                        }
                        ci->ttb[TTB_REPEAT] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_REPEAT] = 0;

                if (!times)
                    ci->repeattimes = 3;
                else
                    ci->repeattimes = atol(times);
                if (ci->repeattimes < 2)
                    ci->repeattimes = 3;

                ci->botflags |= BS_KICK_REPEAT;
                if (ci->ttb[TTB_REPEAT]) {
                    alog("%s: %s!%s@%s enabled kicking for repeating on %s with time to ban of %d",
                         s_BotServ, u->nick, u->username, u->host, ci->name, ci->ttb[TTB_REPEAT]);
                    notice_lang(s_BotServ, u, BOT_KICK_REPEAT_ON_BAN,
                                ci->repeattimes, ci->ttb[TTB_REPEAT]);
                } else {
                    alog("%s: %s!%s@%s enabled kicking for repeating on %s",
                         s_BotServ, u->nick, u->username, u->host, ci->name);
                    notice_lang(s_BotServ, u, BOT_KICK_REPEAT_ON,
                                ci->repeattimes);
                }
            } else {
                ci->botflags &= ~BS_KICK_REPEAT;
                alog("%s: %s!%s@%s disabled kicking for repeating on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_KICK_REPEAT_OFF);
            }
        } else if (!stricmp(option, "REVERSES")) {
            if (!stricmp(value, "ON")) {
                if (ttb) {
                    errno = 0;
                    ci->ttb[TTB_REVERSES] =
                        strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_REVERSES] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_REVERSES]);
                        }
                        ci->ttb[TTB_REVERSES] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_REVERSES] = 0;
                ci->botflags |= BS_KICK_REVERSES;
                if (ci->ttb[TTB_REVERSES]) {
                    alog("%s: %s!%s@%s enabled kicking for reversess on %s with time to ban of %d",
                         s_BotServ, u->nick, u->username, u->host, ci->name, ci->ttb[TTB_REVERSES]);
                    notice_lang(s_BotServ, u, BOT_KICK_REVERSES_ON_BAN,
                                ci->ttb[TTB_REVERSES]);
                } else {
                    alog("%s: %s!%s@%s enabled kicking for reverses on %s",
                         s_BotServ, u->nick, u->username, u->host, ci->name);
                    notice_lang(s_BotServ, u, BOT_KICK_REVERSES_ON);
                }
            } else {
                ci->botflags &= ~BS_KICK_REVERSES;
                alog("%s: %s!%s@%s disabled kicking for reverses on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_KICK_REVERSES_OFF);
            }
        } else if (!stricmp(option, "UNDERLINES")) {
            if (!stricmp(value, "ON")) {
                if (ttb) {
                    errno = 0;
                    ci->ttb[TTB_UNDERLINES] =
                        strtol(ttb, (char **) NULL, 10);
                    if (errno == ERANGE || errno == EINVAL
                        || ci->ttb[TTB_UNDERLINES] < 0) {
                        if (debug) {
                            alog("debug: errno is %d ERANGE %d EINVAL %d ttb %d", errno, ERANGE, EINVAL, ci->ttb[TTB_UNDERLINES]);
                        }
                        ci->ttb[TTB_UNDERLINES] = 0;
                        notice_lang(s_BotServ, u, BOT_KICK_BAD_TTB, ttb);
                        return MOD_CONT;
                    }
                } else
                    ci->ttb[TTB_UNDERLINES] = 0;
                ci->botflags |= BS_KICK_UNDERLINES;
                if (ci->ttb[TTB_UNDERLINES]) {
                    alog("%s: %s!%s@%s enabled kicking for underlines on %s with time to ban of %d",
                         s_BotServ, u->nick, u->username, u->host, ci->name, ci->ttb[TTB_UNDERLINES]);
                    notice_lang(s_BotServ, u, BOT_KICK_UNDERLINES_ON_BAN,
                                ci->ttb[TTB_UNDERLINES]);
                } else {
                    alog("%s: %s!%s@%s enabled kicking for underlines on %s",
                         s_BotServ, u->nick, u->username, u->host, ci->name);
                    notice_lang(s_BotServ, u, BOT_KICK_UNDERLINES_ON);
                }
            } else {
                ci->botflags &= ~BS_KICK_UNDERLINES;
                alog("%s: %s!%s@%s disabled kicking for underlines on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_KICK_UNDERLINES_OFF);
            }
        } else
            notice_help(s_BotServ, u, BOT_KICK_UNKNOWN, option);
    }
    return MOD_CONT;
}
