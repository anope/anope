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

int do_set(User * u);
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

    c = createCommand("SET", do_set, NULL, BOT_HELP_SET, -1, -1,
                      BOT_SERVADMIN_HELP_SET, BOT_SERVADMIN_HELP_SET);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("SET DONTKICKOPS", NULL, NULL,
                      BOT_HELP_SET_DONTKICKOPS, -1, -1, -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("SET DONTKICKVOICES", NULL, NULL,
                      BOT_HELP_SET_DONTKICKVOICES, -1, -1, -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("SET FANTASY", NULL, NULL, BOT_HELP_SET_FANTASY, -1,
                      -1, -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("SET GREET", NULL, NULL, BOT_HELP_SET_GREET, -1, -1,
                      -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("SET SYMBIOSIS", NULL, NULL, BOT_HELP_SET_SYMBIOSIS,
                      -1, -1, -1, -1);
    c->help_param1 = s_ChanServ;
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("SET NOBOT", NULL, NULL, -1, -1, -1,
                      BOT_SERVADMIN_HELP_SET_NOBOT,
                      BOT_SERVADMIN_HELP_SET_NOBOT);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);
    c = createCommand("SET PRIVATE", NULL, NULL, -1, -1, -1,
                      BOT_SERVADMIN_HELP_SET_PRIVATE,
                      BOT_SERVADMIN_HELP_SET_PRIVATE);
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
    notice_lang(s_BotServ, u, BOT_HELP_CMD_SET);
}

/**
 * The /bs set command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_set(User * u)
{
    char *chan = strtok(NULL, " ");
    char *option = strtok(NULL, " ");
    char *value = strtok(NULL, " ");
    int is_servadmin = is_services_admin(u);

    ChannelInfo *ci;

    if (readonly)
        notice_lang(s_BotServ, u, BOT_SET_DISABLED);
    else if (!chan || !option || !value)
        syntax_error(s_BotServ, u, "SET", BOT_SET_SYNTAX);
    else if (is_servadmin && !stricmp(option, "PRIVATE")) {
        BotInfo *bi;

        if ((bi = findbot(chan))) {
            if (!stricmp(value, "ON")) {
                bi->flags |= BI_PRIVATE;
                alog("%s: %s!%s@%s set PRIVATE on bot %s",
                     s_BotServ, u->nick, u->username, u->host, bi->nick);
                notice_lang(s_BotServ, u, BOT_SET_PRIVATE_ON, bi->nick);
            } else if (!stricmp(value, "OFF")) {
                bi->flags &= ~BI_PRIVATE;
                alog("%s: %s!%s@%s unset PRIVATE on bot %s",
                     s_BotServ, u->nick, u->username, u->host, bi->nick);
                notice_lang(s_BotServ, u, BOT_SET_PRIVATE_OFF, bi->nick);
            } else {
                syntax_error(s_BotServ, u, "SET PRIVATE",
                             BOT_SET_PRIVATE_SYNTAX);
            }
        } else {
            notice_lang(s_BotServ, u, BOT_DOES_NOT_EXIST, chan);
        }
        return MOD_CONT;
    } else if (!(ci = cs_findchan(chan)))
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    else if (ci->flags & CI_VERBOTEN)
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    else if (!is_servadmin && !check_access(u, ci, CA_SET))
        notice_lang(s_BotServ, u, ACCESS_DENIED);
    else {
        if (!stricmp(option, "DONTKICKOPS")) {
            if (!stricmp(value, "ON")) {
                ci->botflags |= BS_DONTKICKOPS;
                alog("%s: %s!%s@%s set DONTKICKOPS on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_SET_DONTKICKOPS_ON,
                            ci->name);
            } else if (!stricmp(value, "OFF")) {
                ci->botflags &= ~BS_DONTKICKOPS;
                alog("%s: %s!%s@%s unset DONTKICKOPS on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_SET_DONTKICKOPS_OFF,
                            ci->name);
            } else {
                syntax_error(s_BotServ, u, "SET DONTKICKOPS",
                             BOT_SET_DONTKICKOPS_SYNTAX);
            }
        } else if (!stricmp(option, "DONTKICKVOICES")) {
            if (!stricmp(value, "ON")) {
                ci->botflags |= BS_DONTKICKVOICES;
                alog("%s: %s!%s@%s set DONTKICKVOICES on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_SET_DONTKICKVOICES_ON,
                            ci->name);
            } else if (!stricmp(value, "OFF")) {
                ci->botflags &= ~BS_DONTKICKVOICES;
                alog("%s: %s!%s@%s unset DONTKICKVOICES on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_SET_DONTKICKVOICES_OFF,
                            ci->name);
            } else {
                syntax_error(s_BotServ, u, "SET DONTKICKVOICES",
                             BOT_SET_DONTKICKVOICES_SYNTAX);
            }
        } else if (!stricmp(option, "FANTASY")) {
            if (!stricmp(value, "ON")) {
                ci->botflags |= BS_FANTASY;
                alog("%s: %s!%s@%s set FANTASY on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_SET_FANTASY_ON, ci->name);
            } else if (!stricmp(value, "OFF")) {
                ci->botflags &= ~BS_FANTASY;
                alog("%s: %s!%s@%s unset FANTASY on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_SET_FANTASY_OFF, ci->name);
            } else {
                syntax_error(s_BotServ, u, "SET FANTASY",
                             BOT_SET_FANTASY_SYNTAX);
            }
        } else if (!stricmp(option, "GREET")) {
            if (!stricmp(value, "ON")) {
                ci->botflags |= BS_GREET;
                alog("%s: %s!%s@%s set GREET on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_SET_GREET_ON, ci->name);
            } else if (!stricmp(value, "OFF")) {
                ci->botflags &= ~BS_GREET;
                alog("%s: %s!%s@%s unset GREET on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_SET_GREET_OFF, ci->name);
            } else {
                syntax_error(s_BotServ, u, "SET GREET",
                             BOT_SET_GREET_SYNTAX);
            }
        } else if (is_servadmin && !stricmp(option, "NOBOT")) {
            if (!stricmp(value, "ON")) {
                ci->botflags |= BS_NOBOT;
                if (ci->bi)
                    unassign(u, ci);
                alog("%s: %s!%s@%s set NOBOT on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_SET_NOBOT_ON, ci->name);
            } else if (!stricmp(value, "OFF")) {
                ci->botflags &= ~BS_NOBOT;
                alog("%s: %s!%s@%s unset NOBOT on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_SET_NOBOT_OFF, ci->name);
            } else {
                syntax_error(s_BotServ, u, "SET NOBOT",
                             BOT_SET_NOBOT_SYNTAX);
            }
        } else if (!stricmp(option, "SYMBIOSIS")) {
            if (!stricmp(value, "ON")) {
                ci->botflags |= BS_SYMBIOSIS;
                alog("%s: %s!%s@%s set SYMBIOSIS on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_SET_SYMBIOSIS_ON, ci->name);
            } else if (!stricmp(value, "OFF")) {
                ci->botflags &= ~BS_SYMBIOSIS;
                alog("%s: %s!%s@%s unset SYMBIOSIS on %s",
                     s_BotServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_BotServ, u, BOT_SET_SYMBIOSIS_OFF, ci->name);
            } else {
                syntax_error(s_BotServ, u, "SET SYMBIOSIS",
                             BOT_SET_SYMBIOSIS_SYNTAX);
            }
        } else {
            notice_help(s_BotServ, u, BOT_SET_UNKNOWN, option);
        }
    }
    return MOD_CONT;
}
