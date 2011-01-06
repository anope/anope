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

int do_botlist(User * u);
void myBotServHelp(User * u);

/**
 * Create the botlist command, and tell anope about it.
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
    c = createCommand("BOTLIST", do_botlist, NULL, BOT_HELP_BOTLIST, -1,
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
    notice_lang(s_BotServ, u, BOT_HELP_CMD_BOTLIST);
}

/**
 * The /bs botlist command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_botlist(User * u)
{
    int i, count = 0;
    BotInfo *bi;

    if (!nbots) {
        notice_lang(s_BotServ, u, BOT_BOTLIST_EMPTY);
        return MOD_CONT;
    }

    for (i = 0; i < 256; i++) {
        for (bi = botlists[i]; bi; bi = bi->next) {
            if (!(bi->flags & BI_PRIVATE)) {
                if (!count)
                    notice_lang(s_BotServ, u, BOT_BOTLIST_HEADER);
                count++;
                notice_user(s_BotServ, u, "   %-15s  (%s@%s)", bi->nick,
                            bi->user, bi->host);
            }
        }
    }

    if (is_oper(u) && count < nbots) {
        notice_lang(s_BotServ, u, BOT_BOTLIST_PRIVATE_HEADER);

        for (i = 0; i < 256; i++) {
            for (bi = botlists[i]; bi; bi = bi->next) {
                if (bi->flags & BI_PRIVATE) {
                    notice_user(s_BotServ, u, "   %-15s  (%s@%s)",
                                bi->nick, bi->user, bi->host);
                    count++;
                }
            }
        }
    }

    if (!count)
        notice_lang(s_BotServ, u, BOT_BOTLIST_EMPTY);
    else
        notice_lang(s_BotServ, u, BOT_BOTLIST_FOOTER, count);
    return MOD_CONT;
}
