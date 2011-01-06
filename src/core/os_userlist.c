/* OperServ core functions
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

#ifdef _WIN32
extern MDE int anope_get_invite_mode();
extern MDE int anope_get_invis_mode();
#endif

int do_userlist(User * u);
void myOperServHelp(User * u);

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

    c = createCommand("USERLIST", do_userlist, NULL,
                      OPER_HELP_USERLIST, -1, -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);

    moduleSetOperHelp(myOperServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
    notice_lang(s_OperServ, u, OPER_HELP_CMD_USERLIST);
}

/**
 * The /os userlist command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/

int do_userlist(User * u)
{
    char *pattern = strtok(NULL, " ");
    char *opt = strtok(NULL, " ");

    Channel *c;
    int modes = 0;

    if (opt && !stricmp(opt, "INVISIBLE"))
        modes |= anope_get_invis_mode();

    if (pattern && (c = findchan(pattern))) {
        struct c_userlist *cu;

        notice_lang(s_OperServ, u, OPER_USERLIST_HEADER_CHAN, pattern);

        for (cu = c->users; cu; cu = cu->next) {
            if (modes && !(cu->user->mode & modes))
                continue;
            notice_lang(s_OperServ, u, OPER_USERLIST_RECORD,
                        cu->user->nick, common_get_vident(cu->user),
                        common_get_vhost(cu->user));
        }
    } else {
        char mask[BUFSIZE];
        int i;
        User *u2;

        notice_lang(s_OperServ, u, OPER_USERLIST_HEADER);

        for (i = 0; i < 1024; i++) {
            for (u2 = userlist[i]; u2; u2 = u2->next) {
                if (pattern) {
                    snprintf(mask, sizeof(mask), "%s!%s@%s", u2->nick,
                             common_get_vident(u2), common_get_vhost(u2));
                    if (!match_wild_nocase(pattern, mask))
                        continue;
                    if (modes && !(u2->mode & modes))
                        continue;
                }
                notice_lang(s_OperServ, u, OPER_USERLIST_RECORD, u2->nick,
                            common_get_vident(u2), common_get_vhost(u2));
            }
        }
    }

    notice_lang(s_OperServ, u, OPER_USERLIST_END);
    return MOD_CONT;
}
