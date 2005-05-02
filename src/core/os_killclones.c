/* OperServ core functions
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

#include "module.h"

int do_killclones(User * u);
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
    moduleAddVersion
        ("$Id$");
    moduleSetType(CORE);

    c = createCommand("KILLCLONES", do_killclones, is_services_oper,
                      OPER_HELP_KILLCLONES, -1, -1, -1, -1);
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
    if (is_services_oper(u)) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_KILLCLONES);
    }
}

/** Kill all users matching a certain host. The host is obtained from the
 * supplied nick. The raw hostmsk is not supplied with the command in an effort
 * to prevent abuse and mistakes from being made - which might cause *.com to
 * be killed. It also makes it very quick and simple to use - which is usually
 * what you want when someone starts loading numerous clones. In addition to
 * killing the clones, we add a temporary AKILL to prevent them from
 * immediately reconnecting.
 * Syntax: KILLCLONES nick
 * -TheShadow (29 Mar 1999)
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_killclones(User * u)
{
    char *clonenick = strtok(NULL, " ");
    int count = 0;
    User *cloneuser, *user, *tempuser;
    char *clonemask, *akillmask;
    char killreason[NICKMAX + 32];
    char akillreason[] = "Temporary KILLCLONES akill.";

    if (!clonenick) {
        notice_lang(s_OperServ, u, OPER_KILLCLONES_SYNTAX);

    } else if (!(cloneuser = finduser(clonenick))) {
        notice_lang(s_OperServ, u, OPER_KILLCLONES_UNKNOWN_NICK,
                    clonenick);

    } else {
        clonemask = smalloc(strlen(cloneuser->host) + 5);
        sprintf(clonemask, "*!*@%s", cloneuser->host);

        akillmask = smalloc(strlen(cloneuser->host) + 3);
        sprintf(akillmask, "*@%s", cloneuser->host);

        user = firstuser();
        while (user) {
            if (match_usermask(clonemask, user) != 0) {
                tempuser = nextuser();
                count++;
                snprintf(killreason, sizeof(killreason),
                         "Cloning [%d]", count);
                kill_user(NULL, user->nick, killreason);
                user = tempuser;
            } else {
                user = nextuser();
            }
        }

        add_akill(u, akillmask, u->nick,
                  time(NULL) + KillClonesAkillExpire, akillreason);

        anope_cmd_global(s_OperServ,
                         "\2%s\2 used KILLCLONES for \2%s\2 killing "
                         "\2%d\2 clones. A temporary AKILL has been added "
                         "for \2%s\2.", u->nick, clonemask, count,
                         akillmask);

        alog("%s: KILLCLONES: %d clone(s) matching %s killed.",
             s_OperServ, count, clonemask);

        free(akillmask);
        free(clonemask);
    }
    return MOD_CONT;
}
