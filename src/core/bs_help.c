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

int do_help(User * u);

/**
 * Create the help command, and tell anope about it.
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
    c = createCommand("HELP", do_help, NULL, -1, -1, -1, -1, -1);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);


    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * The /bs help command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_help(User * u)
{
    char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_BotServ, u, BOT_HELP);
        moduleDisplayHelp(4, u);
        notice_help(s_BotServ, u, BOT_HELP_FOOTER, BSMinUsers);
    } else {
        mod_help_cmd(s_BotServ, u, BOTSERV, cmd);
    }
    return MOD_CONT;
}
