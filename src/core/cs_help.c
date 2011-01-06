/* ChanServ core functions
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
 * Create the off command, and tell anope about it.
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
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}

/**
 * The /cs help command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_help(User * u)
{
    char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_ChanServ, u, CHAN_HELP);
        moduleDisplayHelp(2, u);
        if (CSExpire >= 86400)
            notice_help(s_ChanServ, u, CHAN_HELP_EXPIRES,
                        CSExpire / 86400);
        if (is_services_oper(u))
            notice_help(s_ChanServ, u, CHAN_SERVADMIN_HELP);
    } else if (stricmp(cmd, "LEVELS DESC") == 0) {
        int i;
        notice_help(s_ChanServ, u, CHAN_HELP_LEVELS_DESC);
        if (!levelinfo_maxwidth) {
            for (i = 0; levelinfo[i].what >= 0; i++) {
                int len = strlen(levelinfo[i].name);
                if (len > levelinfo_maxwidth)
                    levelinfo_maxwidth = len;
            }
        }
        for (i = 0; levelinfo[i].what >= 0; i++) {
            notice_help(s_ChanServ, u, CHAN_HELP_LEVELS_DESC_FORMAT,
                        levelinfo_maxwidth, levelinfo[i].name,
                        getstring(u->na, levelinfo[i].desc));
        }
    } else {
        mod_help_cmd(s_ChanServ, u, CHANSERV, cmd);
    }
    return MOD_CONT;
}
