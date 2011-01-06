/* NickServ core functions
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

    c = createCommand("HELP", do_help, NULL, -1, -1, -1, -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}

/**
 * The /ns help command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_help(User * u)
{
    char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_NickServ, u, NICK_HELP);
        moduleDisplayHelp(1, u);
        if (is_services_admin(u)) {
            notice_help(s_NickServ, u, NICK_SERVADMIN_HELP);
        }
        if (NSExpire >= 86400)
            notice_help(s_NickServ, u, NICK_HELP_EXPIRES,
                        NSExpire / 86400);
        notice_help(s_NickServ, u, NICK_HELP_FOOTER);
    } else if (stricmp(cmd, "SET LANGUAGE") == 0) {
        int i;
        notice_help(s_NickServ, u, NICK_HELP_SET_LANGUAGE);
        for (i = 0; i < NUM_LANGS && langlist[i] >= 0; i++)
            notice_user(s_NickServ, u, "    %2d) %s", i + 1,
                        langnames[langlist[i]]);
    } else {
        mod_help_cmd(s_NickServ, u, NICKSERV, cmd);
    }
    return MOD_CONT;
}

/* EOF */
