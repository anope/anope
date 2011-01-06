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

int do_group(User * u);
void myHostServHelp(User * u);
extern int do_hs_sync(NickCore * nc, char *vIdent, char *hostmask,
                      char *creator, time_t time);


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

    c = createCommand("GROUP", do_group, NULL, HOST_HELP_GROUP, -1, -1, -1,
                      -1);
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
    notice_lang(s_HostServ, u, HOST_HELP_CMD_GROUP);
}

/**
 * The /hs group command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_group(User * u)
{
    NickAlias *na;
    HostCore *tmp;
    char *vHost = NULL;
    char *vIdent = NULL;
    char *creator = NULL;
    HostCore *head = NULL;
    time_t time;
    boolean found = false;

    head = hostCoreListHead();

    if ((na = findnick(u->nick))) {
        if (na->status & NS_IDENTIFIED) {

            tmp = findHostCore(head, u->nick, &found);
            if (found) {
                if (tmp == NULL) {
                    tmp = head; /* incase first in list */
                } else if (tmp->next) { /* we dont want the previous entry were not inserting! */
                    tmp = tmp->next;    /* jump to the next */
                }

                vHost = sstrdup(tmp->vHost);
                if (tmp->vIdent)
                    vIdent = sstrdup(tmp->vIdent);
                creator = sstrdup(tmp->creator);
                time = tmp->time;

                do_hs_sync(na->nc, vIdent, vHost, creator, time);
                if (tmp->vIdent) {
                    notice_lang(s_HostServ, u, HOST_IDENT_GROUP,
                                na->nc->display, vIdent, vHost);
                } else {
                    notice_lang(s_HostServ, u, HOST_GROUP, na->nc->display,
                                vHost);
                }
                alog("%s: %s!%s@%s grouped their vhost",
                     s_HostServ, u->nick, u->username, u->host);
                free(vHost);
                if (vIdent)
                    free(vIdent);
                free(creator);

            } else {
                notice_lang(s_HostServ, u, HOST_NOT_ASSIGNED);
            }
        } else {
            notice_lang(s_HostServ, u, HOST_ID);
        }
    } else {
        notice_lang(s_HostServ, u, HOST_NOT_REGED);
    }
    return MOD_CONT;
}
