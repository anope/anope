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

int do_assign(User * u);
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
    c = createCommand("ASSIGN", do_assign, NULL, BOT_HELP_ASSIGN, -1, -1,
                      -1, -1);
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
    notice_lang(s_BotServ, u, BOT_HELP_CMD_ASSIGN);
}

/**
 * The /bs assign command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_assign(User * u)
{
    char *chan = strtok(NULL, " ");
    char *nick = strtok(NULL, " ");
    BotInfo *bi;
    ChannelInfo *ci;

    if (readonly)
        notice_lang(s_BotServ, u, BOT_ASSIGN_READONLY);
    else if (!chan || !nick)
        syntax_error(s_BotServ, u, "ASSIGN", BOT_ASSIGN_SYNTAX);
    else if (!(bi = findbot(nick)))
        notice_lang(s_BotServ, u, BOT_DOES_NOT_EXIST, nick);
    else if (bi->flags & BI_PRIVATE && !is_oper(u))
        notice_lang(s_BotServ, u, PERMISSION_DENIED);
    else if (!(ci = cs_findchan(chan)))
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    else if ((ci->botflags & BS_NOBOT)
             || (!check_access(u, ci, CA_ASSIGN) && !is_services_admin(u)))
        notice_lang(s_BotServ, u, PERMISSION_DENIED);
    else if (ci->flags & CI_VERBOTEN)
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    else if ((ci->bi) && (stricmp(ci->bi->nick, nick) == 0))
        notice_lang(s_BotServ, u, BOT_ASSIGN_ALREADY, ci->bi->nick, chan);
    else {
        if (ci->bi)
            unassign(u, ci);
        ci->bi = bi;
        bi->chancount++;
        if (ci->c && ci->c->usercount >= BSMinUsers) {
            bot_join(ci);
        }
        alog("%s: %s!%s@%s assigned bot %s to %s", s_BotServ, u->nick, u->username,
             u->host, bi->nick, ci->name);
        notice_lang(s_BotServ, u, BOT_ASSIGN_ASSIGNED, bi->nick, ci->name);
        send_event(EVENT_BOT_ASSIGN, 2, ci->name, bi->nick);
    }
    return MOD_CONT;
}
