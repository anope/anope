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

int do_setall(User * u);
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

    c = createCommand("SETALL", do_setall, is_host_setter,
                      HOST_HELP_SETALL, -1, -1, -1, -1);
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
    if (is_host_setter(u)) {
        notice_lang(s_HostServ, u, HOST_HELP_CMD_SETALL);
    }
}

/**
 * The /hs setall command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_setall(User * u)
{

    char *nick = strtok(NULL, " ");
    char *rawhostmask = strtok(NULL, " ");
    char *hostmask = smalloc(HOSTMAX);

    NickAlias *na;
    int32 tmp_time;
    char *s;

    char *vIdent = NULL;

    if (!nick || !rawhostmask) {
        syntax_error(s_HostServ, u, "SETALL", HOST_SETALL_SYNTAX);
        free(hostmask);
        return MOD_CONT;
    }

    vIdent = myStrGetOnlyToken(rawhostmask, '@', 0);    /* Get the first substring, @ as delimiter */
    if (vIdent) {
        rawhostmask = myStrGetTokenRemainder(rawhostmask, '@', 1);      /* get the remaining string */
        if (!rawhostmask) {
            syntax_error(s_HostServ, u, "SETALL", HOST_SETALL_SYNTAX);
            free(vIdent);
            free(hostmask);
            return MOD_CONT;
        }
        if (strlen(vIdent) > USERMAX - 1) {
            notice_lang(s_HostServ, u, HOST_SET_IDENTTOOLONG, USERMAX);
            free(vIdent);
            free(rawhostmask);
            free(hostmask);
            return MOD_CONT;
        } else {
            for (s = vIdent; *s; s++) {
                if (!isvalidchar(*s)) {
                    notice_lang(s_HostServ, u, HOST_SET_IDENT_ERROR);
                    free(vIdent);
                    free(rawhostmask);
                    free(hostmask);
                    return MOD_CONT;
                }
            }
        }
        if (!ircd->vident) {
            notice_lang(s_HostServ, u, HOST_NO_VIDENT);
            free(vIdent);
            free(rawhostmask);
            free(hostmask);
            return MOD_CONT;
        }
    }

    if (strlen(rawhostmask) < HOSTMAX)
        snprintf(hostmask, HOSTMAX, "%s", rawhostmask);
    else {
        notice_lang(s_HostServ, u, HOST_SET_TOOLONG, HOSTMAX);
        if (vIdent) {
            free(vIdent);
            free(rawhostmask);
        }
        free(hostmask);
        return MOD_CONT;
    }

    if (!isValidHost(hostmask, 3)) {
        notice_lang(s_HostServ, u, HOST_SET_ERROR);
        if (vIdent) {
            free(vIdent);
            free(rawhostmask);
        }
        free(hostmask);
        return MOD_CONT;
    }

    tmp_time = time(NULL);

    if ((na = findnick(nick))) {
        if (na->status & NS_VERBOTEN) {
            notice_lang(s_HostServ, u, NICK_X_FORBIDDEN, nick);
            if (vIdent) {
                free(vIdent);
                free(rawhostmask);
            }
            free(hostmask);
            return MOD_CONT;
        }
        if (vIdent && ircd->vident) {
            alog("vHost for all nicks in group \002%s\002 set to \002%s@%s\002 by oper \002%s\002", nick, vIdent, hostmask, u->nick);
        } else {
            alog("vHost for all nicks in group \002%s\002 set to \002%s\002 by oper \002%s\002", nick, hostmask, u->nick);
        }
        do_hs_sync(na->nc, vIdent, hostmask, u->nick, tmp_time);
        if (vIdent) {
            notice_lang(s_HostServ, u, HOST_IDENT_SETALL, nick, vIdent,
                        hostmask);
        } else {
            notice_lang(s_HostServ, u, HOST_SETALL, nick, hostmask);
        }
    } else {
        notice_lang(s_HostServ, u, HOST_NOREG, nick);
    }
    if (vIdent) {
        free(vIdent);
        free(rawhostmask);
    }
    free(hostmask);
    return MOD_CONT;
}
