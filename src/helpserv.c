/* HelpServ functions
 *
 * (C) 2003-2005 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$ 
 *
 */

/*************************************************************************/
#include "services.h"
#include "pseudo.h"

static int do_help(User * u);
void moduleAddHelpServCmds(void);

/*************************************************************************/

/**
 * Setup the commands for HelpServ
 * @return void
 */
void moduleAddHelpServCmds(void)
{
    Command *c;
    c = createCommand("HELP", do_help, NULL, -1, -1, -1, -1, -1);
    addCoreCommand(HELPSERV, c);
}

/*************************************************************************/

/**
 * HelpServ initialization.
 * @return void
 */
void helpserv_init(void)
{
    moduleAddHelpServCmds();
}

/*************************************************************************/

/**
 * Main HelpServ routine.
 * @param u User Struct of the user sending the PRIVMSG
 * @param buf Buffer containing the PRIVMSG data
 * @return void
 */
void helpserv(User * u, char *buf)
{
    char *cmd, *s;

    cmd = strtok(buf, " ");

    if (!cmd) {
        return;
    } else if (stricmp(cmd, "\1PING") == 0) {
        if (!(s = strtok(NULL, ""))) {
            s = "";
        }
        anope_cmd_ctcp(s_HelpServ, u->nick, "PING %s", s);
    } else {
        mod_run_cmd(s_HelpServ, u, HELPSERV, cmd);
    }
}

/*************************************************************************/

/**
 * Display the HelpServ help.
 * This core function has been embed in the source for a long time, but  
 * it moved into it's own file so we now all can enjoy the joy of        
 * modules for HelpServ.
 * @param u User Struct of the user looking for help
 * @return MOD_CONT
 */
static int do_help(User * u)
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
