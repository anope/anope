/* HostServ core functions
 *
 * (C) 2003-2008 Anope Team
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

int myDoSet(User * u);
void myHostServHelp(User * u);

class HSSet : public Module
{
 public:
	HSSet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("SET", myDoSet, is_host_setter, HOST_HELP_SET, -1, -1, -1, -1);
		this->AddCommand(HOSTSERV, c, MOD_UNIQUE);

		moduleSetHostHelp(myHostServHelp);
	}
};



/**
 * Add the help response to anopes /hs help output.
 * @param u The user who is requesting help
 **/
void myHostServHelp(User * u)
{
    if (is_host_setter(u)) {
        notice_lang(s_HostServ, u, HOST_HELP_CMD_SET);
    }
}

/**
 * The /hs set command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int myDoSet(User * u)
{
    char *nick = strtok(NULL, " ");
    char *rawhostmask = strtok(NULL, " ");
    char *hostmask = (char *)smalloc(HOSTMAX);

    NickAlias *na;
    int32 tmp_time;
    char *s;

    char *vIdent = NULL;

    if (!nick || !rawhostmask) {
        notice_lang(s_HostServ, u, HOST_SET_SYNTAX, s_HostServ);
        free(hostmask);
        return MOD_CONT;
    }

    vIdent = myStrGetOnlyToken(rawhostmask, '@', 0);    /* Get the first substring, @ as delimiter */
    if (vIdent) {
        rawhostmask = myStrGetTokenRemainder(rawhostmask, '@', 1);      /* get the remaining string */
        if (!rawhostmask) {
            notice_lang(s_HostServ, u, HOST_SET_SYNTAX, s_HostServ);
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
    if (strlen(rawhostmask) < HOSTMAX - 1)
        snprintf(hostmask, HOSTMAX - 1, "%s", rawhostmask);
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
            alog("vHost for user \002%s\002 set to \002%s@%s\002 by oper \002%s\002", nick, vIdent, hostmask, u->nick);
        } else {
            alog("vHost for user \002%s\002 set to \002%s\002 by oper \002%s\002", nick, hostmask, u->nick);
        }
        addHostCore(nick, vIdent, hostmask, u->nick, tmp_time);
        if (vIdent) {
            notice_lang(s_HostServ, u, HOST_IDENT_SET, nick, vIdent,
                        hostmask);
        } else {
            notice_lang(s_HostServ, u, HOST_SET, nick, hostmask);
        }
    } else {
        notice_lang(s_HostServ, u, HOST_NOREG, nick);
    }
    free(hostmask);
    if (vIdent) {
        free(vIdent);
        free(rawhostmask);
    }
    return MOD_CONT;
}

MODULE_INIT("hs_set", HSSet)
