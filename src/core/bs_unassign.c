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

int do_unassign(User * u);
void myBotServHelp(User * u);

/**
 * Create the unassign command, and tell anope about it.
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
    c = createCommand("UNASSIGN", do_unassign, NULL, BOT_HELP_UNASSIGN, -1,
                      -1, -1, -1);
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
    notice_lang(s_BotServ, u, BOT_HELP_CMD_UNASSIGN);
}

/**
 * The /bs unassign command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_unassign(User * u)
{
    char *chan = strtok(NULL, " ");
    ChannelInfo *ci;

    if (readonly)
        notice_lang(s_BotServ, u, BOT_ASSIGN_READONLY);
    else if (!chan)
        syntax_error(s_BotServ, u, "UNASSIGN", BOT_UNASSIGN_SYNTAX);
    else if (!(ci = cs_findchan(chan)))
        notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
    else if (ci->flags & CI_VERBOTEN)
        notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
    else if (!is_services_admin(u) && !check_access(u, ci, CA_ASSIGN))
        notice_lang(s_BotServ, u, ACCESS_DENIED);
    else if (!ci->bi)
        notice_help(s_BotServ, u, BOT_NOT_ASSIGNED);
    else {
        alog("%s: %s!%s@%s unassigned bot %s from %s", s_BotServ, u->nick, u->username,
             u->host, ci->bi->nick, ci->name);
        unassign(u, ci);
        notice_lang(s_BotServ, u, BOT_UNASSIGN_UNASSIGNED, ci->name);
    }
    return MOD_CONT;
}
