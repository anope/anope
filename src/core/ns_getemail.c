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
 * A simple call to check for all emails that a user may have registered 
 * with. It returns the nicks that match the email you provide. Wild     
 * Cards are not excepted. Must use user@email-host.                     
 * 
 *
 */
/*************************************************************************/

#include "module.h"

int do_getemail(User * u);
void myNickServHelp(User * u);

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

    c = createCommand("GETEMAIL", do_getemail, is_services_admin, -1, -1,
                      -1, NICK_SERVADMIN_HELP_GETEMAIL,
                      NICK_SERVADMIN_HELP_GETEMAIL);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

    moduleSetNickHelp(myNickServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}

/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User * u)
{
    if (is_services_admin(u)) {
        notice_lang(s_NickServ, u, NICK_HELP_CMD_GETEMAIL);
    }
}

/**
 * The /ns getemail command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_getemail(User * u)
{
    char *email = strtok(NULL, " ");
    int i, j = 0;
    NickCore *nc;

    if (!email) {
        syntax_error(s_NickServ, u, "GETMAIL", NICK_GETEMAIL_SYNTAX);
        return MOD_CONT;
    }
    alog("%s: %s!%s@%s used GETEMAIL on %s", s_NickServ, u->nick,
         u->username, u->host, email);
    for (i = 0; i < 1024; i++) {
        for (nc = nclists[i]; nc; nc = nc->next) {
            if (nc->email) {
                if (stricmp(nc->email, email) == 0) {
                    j++;
                    notice_lang(s_NickServ, u, NICK_GETEMAIL_EMAILS_ARE,
                                nc->display, email);
                }
            }
        }
    }
    if (j <= 0) {
        notice_lang(s_NickServ, u, NICK_GETEMAIL_NOT_USED, email);
        return MOD_CONT;
    }
    return MOD_CONT;
}
