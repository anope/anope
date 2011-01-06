/* HelpServ core functions
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
    moduleAddCommand(HELPSERV, c, MOD_UNIQUE);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Display the HelpServ help.
 * This core function has been embed in the source for a long time, but
 * it moved into it's own file so we now all can enjoy the joy of
 * modules for HelpServ.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 */
int do_help(User * u)
{
    char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_HelpServ, u, HELP_HELP, s_NickServ, s_ChanServ,
                    s_MemoServ);
        if (s_BotServ) {
            notice_help(s_HelpServ, u, HELP_HELP_BOT, s_BotServ);
        }
        if (s_HostServ) {
            notice_help(s_HelpServ, u, HELP_HELP_HOST, s_HostServ);
        }
        moduleDisplayHelp(7, u);
    } else {
        mod_help_cmd(s_HelpServ, u, HELPSERV, cmd);
    }
    return MOD_CONT;
}
