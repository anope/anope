/* HelpServ functions
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
#include "services.h"
#include "pseudo.h"

void moduleAddHelpServCmds(void);

/*************************************************************************/

/**
 * Setup the commands for HelpServ
 * @return void
 */
void moduleAddHelpServCmds(void)
{
    modules_core_init(HelpServCoreNumber, HelpServCoreModules);
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
