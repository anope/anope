/* HostServ core functions
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

int do_on(User * u);
void myHostServHelp(User * u);

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

    c = createCommand("ON", do_on, NULL, HOST_HELP_ON, -1, -1, -1, -1);
    moduleAddCommand(HOSTSERV, c, MOD_UNIQUE);

    moduleSetHostHelp(myHostServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to anopes /hs help output.
 * @param u The user who is requesting help
 **/
void myHostServHelp(User * u)
{
    notice_lang(s_HostServ, u, HOST_HELP_CMD_ON);
}

/**
 * The /hs on command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_on(User * u)
{
    NickAlias *na;
    char *vHost;
    char *vIdent = NULL;
    if ((na = findnick(u->nick))) {
        if (na->status & NS_IDENTIFIED) {
            vHost = getvHost(u->nick);
            vIdent = getvIdent(u->nick);
            if (vHost == NULL) {
                notice_lang(s_HostServ, u, HOST_NOT_ASSIGNED);
            } else {
                if (vIdent) {
                    notice_lang(s_HostServ, u, HOST_IDENT_ACTIVATED,
                                vIdent, vHost);
                } else {
                    notice_lang(s_HostServ, u, HOST_ACTIVATED, vHost);
                }
                anope_cmd_vhost_on(u->nick, vIdent, vHost);
                if (ircd->vhost) {
                    if (u->vhost)
                        free(u->vhost);
                    u->vhost = sstrdup(vHost);
                }
                if (ircd->vident) {
                    if (vIdent) {
                        if (u->vident)
                            free(u->vident);
                        u->vident = sstrdup(vIdent);
                    }
                }
                set_lastmask(u);
            }
        } else {
            notice_lang(s_HostServ, u, HOST_ID);
        }
    } else {
        notice_lang(s_HostServ, u, HOST_NOT_REGED);
    }
    return MOD_CONT;
}
